/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#include "cs_types.h"
#include "string.h"

#include "chip_id.h"
#include "global_macros.h"
#include "mem_bridge.h"
#include "debug_host.h"
#include "timer.h"
#include "sys_ctrl.h"
#include "calib_m.h"

#include "bootp_host.h"
#include "bootp_loader.h"
#include "bootp_sector.h"
#include "bootp_key.h"
#include "boot_host.h"
#include "boot_sector.h"
#include "boot_cache.h"
#include "boot.h"
#include "boot_mem_init.h"
#include "boot_sys.h"
#include "reg_alias.h"
#ifdef CHIP_DIE_8955
#include "cfg_regs.h"
#endif

#include "hal_map_engine.h"
#include "tgt_m.h"

#ifdef USE_OPIDDR_PSRAM
#include "psram8_ctrl.h"
#endif
// =============================================================================
// EXTERNS
// =============================================================================

// ROM version number (defined in ROM)
extern CONST UINT32 g_bootRomVersionNumber;


// =============================================================================
// MACROS
// =============================================================================


/// Linker section where the Boot Sector main function must be mapped:
/// Beginning of the first sector of the flash.
#define BOOT_SECTOR_SECTION_START  __attribute__((section (".boot_sector_start")))


/// Linker sections where the Boot Sector structure pointer must be mapped.
/// At a fixed address in the Internal SRAM, right before the fixed pointer.
#define BOOT_SECTOR_SECTION_STRUCT_PTR __attribute__((section (".boot_sector_struct_ptr")))


/// Linker sections where the Boot Sector reload structure pointer must be mapped.
/// At a fixed address in the Internal SRAM, right before the boot sector struct pointer.
#define BOOT_SECTOR_SECTION_RELOAD_STRUCT_PTR __attribute__((section (".boot_sector_reload_struct_ptr")))


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


/// Boot Sector structure pointer.
///
/// This pointer is filled by HAL during the normal execution of the code.
/// It is used by the Boot Sector to find the Boot Sector structure in
/// Internal RAM.
///
/// This pointer will be also used by the host tool (E.g., by CoolWatcher to configure
/// EBC during a restart). Its location is specified in the ld file for the software
/// on the target, and in the corresponding ???_soft_root.xmd for the host tool.
/// These location definitions should be always kept consistent.
PUBLIC BOOT_SECTOR_STRUCT_T*
BOOT_SECTOR_SECTION_STRUCT_PTR g_bootSectorStructPtr;


/// Boot Sector reload structure.
///
/// This structure contains information written by HAL during the normal
/// execution of the code and used by the Boot Sector to reload the ram image
/// when the phone boots.
PUBLIC BOOT_SECTOR_RELOAD_STRUCT_T*
BOOT_SECTOR_SECTION_RELOAD_STRUCT_PTR g_bootSectorReloadStructPtr;


// Copied by the lod command of CoolWatcher. Useful for Ramruns.
PROTECTED BOOT_HST_EXECUTOR_T g_bootHstExecutorCtx
__attribute__((section (".executor_ctx"))) =
{{ 0, (UINT32)boot_Sector, 0, 0, 0}, {0}};


// =============================================================================
// EXPORTS
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================


// =============================================================================
// boot_BootSectorCheckRomVersion
// -----------------------------------------------------------------------------
/// Check the validity of the ROM version. Reset the system and enter
/// host monitor if the ROM version does NOT match.
// =============================================================================
INLINE VOID boot_BootSectorCheckRomVersion(VOID)
{
#ifndef REWRITE_ROM_AT_BOOT
    // Check that the ROM version in the chip is the same as the ROM version
    // of the ROM Library against which the code has been compiled.
    //if(ROM_LINKED_AGAINST_VERSION == *(UINT32*)ROM_VERSION_LOCATION)
    //if(ROM_LINKED_AGAINST_VERSION == *(UINT32*)&_int_rom_version_number) // for 8808 or later
    if(ROM_LINKED_AGAINST_VERSION == g_bootRomVersionNumber)
    {
        return;
    }

    // Mismatch!
    // Check the chip type, reset the system and enter host monitor

#define TIMEOUT_SEND_EVENT (1000)

#define REG_CHIP_ID_GALLITE (*(VOLATILE UINT32 *)0xA1A24000)
#define REG_DEBUG_EVENT_GALLITE (*(VOLATILE UINT32 *)0xA1A3F008)
#define REG_RESET_CAUSE_GALLITE (*(VOLATILE UINT32 *)0xA1A000A0)

#define REG_CHIP_ID_8810 (*(VOLATILE UINT32 *)0xA1A09000)
#define REG_DEBUG_EVENT_8810 (*(VOLATILE UINT32 *)0xA1A1F008)

    VOLATILE UINT32 count;
    UINT32 productId;
    BOOL found = FALSE;

    productId = REG_CHIP_ID_GALLITE >> 16;
    if (productId == 0x1135 || productId == 0x8808 ||
            productId == 0x880a || productId == 0x8809 ||
            productId == 0x880b)
    {
        found = TRUE;
        count = 0;
        while ((REG_DEBUG_EVENT_GALLITE & 1) == 0 &&
                count++ < TIMEOUT_SEND_EVENT);
        REG_DEBUG_EVENT_GALLITE = BOOT_LOADER_EVENT_ROM_ERROR;
        count = 0;
        while (count++ < TIMEOUT_SEND_EVENT);
        // Set Reset_Cause.Boot_Mode to tell the system enter
        // boot monitor after soft reset
        REG_RESET_CAUSE_GALLITE |= (1<<1)<<16;
    }
    if (!found)
    {
        productId = REG_CHIP_ID_8810 >> 16;
        if (productId == 0x8810 || productId == 0x810e ||
                productId == 0x8850)
        {
            found = TRUE;
            count = 0;
            while ((REG_DEBUG_EVENT_8810 & 1) == 0 &&
                    count++ < TIMEOUT_SEND_EVENT);
            REG_DEBUG_EVENT_8810 = BOOT_LOADER_EVENT_ROM_ERROR;
            count = 0;
            while (count++ < TIMEOUT_SEND_EVENT);
            // No need to set Reset_Cause register on 8810
        }
    }

    // The addresses of sysctrl REG_DBG and Sys_Rst_Set are the same
    // on all the chips.
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Sys_Rst_Set = SYS_CTRL_SOFT_RST;
    while(1);
#endif // !REWRITE_ROM_AT_BOOT
}


// =============================================================================
// boot_BootSectorGetStructValidity
// -----------------------------------------------------------------------------
/// Check the validity of the Boot Sector structure by reading a bit in
/// the Boot Mode register and then by checking that the address contained in
/// the Boot Sector structure pointer is a valid address (in KSEG0 or in
/// KSEG(1) and not impair).
///
/// @return TRUE if valid, FALSE else.
// =============================================================================
INLINE BOOL boot_BootSectorGetStructValidity(VOID)
{
    if (FALSE == BOOT_SECTOR_GET_STRUCT_VALIDITY())
    {
        return FALSE;
    }

    // Is the address word-aligned?
    if (((UINT32)g_bootSectorStructPtr & 0x3) != 0)
    {
        return FALSE;
    }

    // Is the address in INT-SRAM space?
    if ( ! (
                ( (UINT32)g_bootSectorStructPtr >= KSEG0(REG_INT_SRAM_BASE) &&
                  (UINT32)g_bootSectorStructPtr < KSEG0(REG_INT_SRAM_BASE)+sizeof(HWP_INT_SRAM_T) ) ||
                ( (UINT32)g_bootSectorStructPtr >= KSEG1(REG_INT_SRAM_BASE) &&
                  (UINT32)g_bootSectorStructPtr < KSEG1(REG_INT_SRAM_BASE)+sizeof(HWP_INT_SRAM_T) )
            ) )
    {
        return FALSE;
    }

    return TRUE;
}


#ifndef CHIP_HAS_AP
// =============================================================================
// boot_BootSectorGetReloadValid
// -----------------------------------------------------------------------------
/// Check the validity of the Boot Sector reload structure by reading the flag and checksum in
/// the Boot reload structure.
///
/// @return TRUE if valid, FALSE else.
// =============================================================================
INLINE BOOL boot_BootSectorGetReloadValid(VOID)
{
    // Is the address word-aligned?
    if (((UINT32)g_bootSectorReloadStructPtr & 0x3) != 0)
    {
        return FALSE;
    }

    // Is the address in INT-SRAM space?
    if ( ! (
                ( (UINT32)g_bootSectorReloadStructPtr >= KSEG0(REG_INT_SRAM_BASE) &&
                  (UINT32)g_bootSectorReloadStructPtr < KSEG0(REG_INT_SRAM_BASE)+sizeof(HWP_INT_SRAM_T) ) ||
                ( (UINT32)g_bootSectorReloadStructPtr >= KSEG1(REG_INT_SRAM_BASE) &&
                  (UINT32)g_bootSectorReloadStructPtr < KSEG1(REG_INT_SRAM_BASE)+sizeof(HWP_INT_SRAM_T) )
            ) )
    {
        return FALSE;
    }

    if( (g_bootSectorReloadStructPtr->validTag != BOOT_SECTOR_RELOAD_VALID_TAG) ||
            (g_bootSectorReloadStructPtr->checkSum != ~BOOT_SECTOR_RELOAD_VALID_TAG) )
    {
        return FALSE;
    }

    g_bootSectorReloadStructPtr->validTag = 0;
    g_bootSectorReloadStructPtr->checkSum = 0;

    return TRUE;
}


// =============================================================================
// boot_SectorReload
// -----------------------------------------------------------------------------
/// Relocate the ram image to the execution section and jump to the entry point.
///
/// @return TRUE if succeeded, FALSE else.
// =============================================================================
INLINE BOOL boot_SectorReload(VOID)
{
    UINT32 i, j;
    for (j=0; j<BOOT_SECTOR_RELOAD_MAX_NUM; j++)
    {
        UINT32 *mapAddr = (UINT32 *)KSEG0(g_bootSectorReloadStructPtr->ctx[j].mapAddress);
        UINT32 *stroreAddr = (UINT32 *)KSEG0(g_bootSectorReloadStructPtr->ctx[j].storeAddress);
        UINT32 size = g_bootSectorReloadStructPtr->ctx[j].size;

        for (i=0; i<size; i++)
        {
            *mapAddr++ = *stroreAddr++;
        }
    }

#if(XCPU_CACHE_MODE_WRITE_BACK==1)
    boot_FlushDCache(TRUE);
    boot_InvalidCache();
#endif

    // Execute at the PC in the execution context with the provided SP
    boot_HstMonitorX();
    return TRUE;
}
#endif // !CHIP_HAS_AP


// =============================================================================
// boot_Sector
// -----------------------------------------------------------------------------
/// Entry of the Boot Sector in the first flash sector.
///
/// This code is executed right after the Boot ROM and right before the
/// Boot Loader, in normal mode execution. It does the following:
///
/// * Writes the EBC RAM chip select configuration if a valid configuration
/// has been found in the Boot Sector structure.
///
/// * If the command in the Boot Sector structure tells us to enter
/// boot monitor, we do so after sending an event.
///
/// * Else, we enter the Boot Loader, providing it with the #param.
///
/// @param param Generic parameter only used to invoke boot_LoaderEnter(param).
// =============================================================================
PROTECTED VOID BOOT_SECTOR_SECTION_START boot_Sector(UINT32 param)
{
#ifdef CHIP_DIE_8955
    // 8955 U01 is incompatible with later chip
    HAL_SYS_CHIP_METAL_ID_T metal = boot_SysGetMetalId();

    // before chip version is verified, ROM function can't be used.
    hwp_debugHost->event = (0x89550000 | metal);

#ifdef CHIP_DIE_8955_U01
    if (metal > HAL_SYS_CHIP_METAL_ID_U01)
        asm volatile("break 1; nop;");
#else // !CHIP_DIE_8955_U01
    if (metal < HAL_SYS_CHIP_METAL_ID_U02)
        asm volatile("break 1; nop;");
#endif
#endif // CHIP_DIE_8955

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8810)
    // Avoid unpredictable issues when ramrunning for multiple times
    // (via debug host or AP). This should have been done in ROM.
    boot_FlushCache();
#endif

    boot_BootSectorCheckRomVersion();

    BOOL bootSectorStructValid = boot_BootSectorGetStructValidity();

#ifdef CHIP_HAS_AP

    // Use to fix some startup missing feature (like usb clock on greenstone)
    boot_SectorFixRom();

#else // !CHIP_HAS_AP
#ifndef USE_OPIDDR_PSRAM
    // Check whether to reload host monitor codes (from RAM,
    // for T-card flash programmer).
    BOOL bootSectorReloadValid = boot_BootSectorGetReloadValid();

    if(bootSectorReloadValid)
    {
        boot_SectorReload();
    }
#endif
#if !(defined(CHIP_HAS_AP) && defined(RUN_IN_DDR))
    // TODO: The RAM access might fail on some chips unless EBC (and the PLL)
    // is setup along with the CS config and timing.

    // Writes the EBC RAM chip select configuration if a valid configuration
    // has been found in the Boot Sector structure.
    // This is the first try to recover RAM access capability.

#ifdef USE_OPIDDR_PSRAM
#else
    if (bootSectorStructValid)
    {
        if (g_bootSectorStructPtr->ebcConfigValidTag == BOOT_SECTOR_EBC_VALID_TAG)
        {
            mon_Event(BOOT_SECTOR_EVENT_CONFIG_RAM);
            // To be compatible with APTIC PSRAM,
            // the original timing and mode is adjusted.
#ifdef USE_OPIDDR_PSRAM
            hwp_psram8Ctrl->power_up = PSRAM8_CTRL_HW_POWER_PULSE;
            while(!((hwp_psram8Ctrl->power_up & PSRAM8_CTRL_INIT_DONE)== PSRAM8_CTRL_INIT_DONE)) {};
            //#ifdef RAM_CLK_IS_78M
            //  hwp_psram8Ctrl->clk_ctrl = 0x6;
            //  hwp_psram8Ctrl->dqs_ctrl = 0x60000;
            //#else
            //#error please config ddr timing.
            //#endif
#else //!USE_OPIDDR_PSRAM
#if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
#else
            hwp_memBridge->CS_Time_Write = g_bootSectorStructPtr->ebcConfigRamTimings & 0xfffff1ff;

#if defined(CHIP_DIE_8809E2)|| defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)
            hwp_memBridge->CS_Config[0].CS_Mode = g_bootSectorStructPtr->ebcConfigRamMode & 0xffcfffff;
#else
            hwp_memBridge->CS_Config[1].CS_Mode = g_bootSectorStructPtr->ebcConfigRamMode & 0xffcfffff;
#endif
#endif // #if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)

#endif //!USE_OPIDDR_PSRAM
        }
    }
#endif
#endif
    // Use to fix some startup missing feature (like usb clock on greenstone)
    boot_SectorFixRom();

    // Check whether the tool requests the system to enter boot monitor.
    if (bootSectorStructValid)
    {
        BOOT_SECTOR_CMD_T cmd = g_bootSectorStructPtr->command;
        // Execute the command of the Boot Sector structure only once.
        g_bootSectorStructPtr->command = 0;
        // If the command in the Boot Sector structure tells us to enter
        // boot monitor, we do so after sending an event.
        if (cmd == BOOT_SECTOR_CMD_ENTER_BOOT_MONITOR)
        {
            hal_MapEngineOpen();
            tgt_RegisterMonitorModules();
            // Write in the exchange status we are in the
            // monitor, ready for operation.
            hwp_debugHost->p2h_status = DEBUG_HOST_P2H_STATUS(BOOT_HST_STATUS_WAITING_FOR_COMMAND);
            mon_Event(BOOT_SECTOR_EVENT_ENTER_MONITOR);
            boot_SectorEnterBootMonitor();
        }
        else if (cmd == BOOT_SECTOR_CMD_ENTER_CALIB)
        {
            param = CALIB_MAGIC_TAG;
        }
    }

    // Clear any remaining exchange status.
    hwp_debugHost->p2h_status = DEBUG_HOST_P2H_STATUS(BOOT_HST_STATUS_NONE);

    // Check whether boot download keys are pressed and the system
    // should enter boot monitor.
    // This ensures that the system can be recovered even when there
    // is an error during flash/RAM initialization or during boot time
    // with USB debug connection.
    if (param != CALIB_MAGIC_TAG)
        boot_ForceMonitorKeyCheck();

    // Check whether usb boot download keys are pressed and the system
    // should enter boot monitor.
    // This will enable USB download without baterry.
    if (param != CALIB_MAGIC_TAG)
        boot_ForceUsbMonitorKeyCheck();

#endif // !CHIP_HAS_AP

#if defined(HAL_GDB_CONFIG) && !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
    // Early open watchdog, and it will be reopen at hal_Open.
    // It can protect abnormal before hal_Open.
    hwp_timer->WDTimer_LoadVal = TIMER_LOADVAL(5 * 32768); // 2s, it is working on 32K clock rather than 16K clock
    hwp_timer->WDTimer_Ctrl = TIMER_START;
#endif

    // Initialize external RAM and flash.
    // This is the second try to recover RAM access capability.
    boot_MemInit(bootSectorStructValid);
#ifdef USE_OPIDDR_PSRAM
    // Check whether to reload host monitor codes (from RAM,
    // for T-card flash programmer).
    BOOL bootSectorReloadValid = boot_BootSectorGetReloadValid();

    if(bootSectorReloadValid)
    {
        hwp_timer->WDTimer_Ctrl = TIMER_STOP;
        boot_SectorReload();
    }
#endif
#ifndef STANDALONE_RAMINIT_IMAGE
    // Boot loader is now always linked with boot sector.
    boot_LoaderEnter(param);
#endif
}


