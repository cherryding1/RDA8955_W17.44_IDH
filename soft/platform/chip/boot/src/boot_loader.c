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
#include "chip_id.h"

#include "global_macros.h"
#include "sys_ctrl.h"
#include "bb_sram.h"

#include "boot.h"
#include "boot_host.h"
#include "bootp_debug.h"
#include "bootp_loader.h"

#include "boot_sector.h"
#include "boot_sector_chip.h"
#include "bootp_host.h"

#include "tgt_hal_cfg.h"

#include "hal_host.h"
#include "hal_mem_map.h"
#include "hal_open.h"
#include "hal_timers.h"
#include "hal_sys.h"

#include "hal_debug.h"

#include "calib_m.h"
#include "boot_map.h"
#include "boot_cache.h"
// For the opening of the flash
#include "memd_m.h"
#include "tgt_memd_cfg.h"

#ifdef MMI_SLT_MODE
extern UINT32 SLT_RAM_TEST_FLAG;
extern UINT32 hal_Slt_Ram_Test();
#endif

// Boot loader entry point.
extern VOID boot_LoaderEnter(UINT32 param);

// Standard main entry, user function.
extern int main(int, char*[]);

// Calibration entry point.
extern int calib_StubMain(int, char*[]);


#define BOOT_LOADER_SECTION_START __attribute__((section(".text.boot_loader_start")))


// =============================================================================
//  boot_LoaderEnter
// -----------------------------------------------------------------------------
/// Real implementation of the boot loader, loading code after the stack has
/// been reseted in #boot_LoaderEnter();
///
/// @param param This parameter as a dummy value when we boot normally from ROM,
/// but it has a special value when the function is called by a jump command
/// of a Boot Monitor (Host, USB or UART). This is used to change the main of
/// the code (where we will jump at the end of the boot loading sequence).
// =============================================================================
PROTECTED VOID boot_Loader(UINT32 param);


// =============================================================================
//  boot_LoaderEnter
// -----------------------------------------------------------------------------
/// Entry point for the code out of the boot sector.
///
/// To compensate for the stack usage of the boot romed code, this function
/// resets the stack pointer at the usable top of the internal SRAM, and call
/// the 'real' boot loader without any local variable on the stack that could
/// thus get corrupted.
/// @param param This parameter as a dummy value when we boot normally from ROM,
/// but it has a special value when the function is called by a jump command
/// of a Boot Monitor (Host, USB or UART). This is used to change the main of
/// the code (where we will jump at the end of the boot loading sequence).
// =============================================================================
PROTECTED VOID BOOT_LOADER_SECTION_START boot_LoaderEnter(UINT32 param)
{
    // Initializes IRQ stack top to the top of the available sram
    // and set here stack pointer to the desired location,
    // at the top of the int sram that is not preallocated for, eg,
    // the mailbox. This location is provided through the symbol
    // properly defined in the linker script _sys_sram_top.
    sxr_IrqStack_pointer = (UINT32) &_sys_sram_top;
    // $29 is the SP register.
    asm volatile("move $29, %0" : /* no output */ : "r"(sxr_IrqStack_pointer));

    boot_Loader(param);

    // Sep. 05, 2009 - Hua Zeng - Workaround for stack corruption issue after resetting SP.
    // If we are not using -mips16 (ISA is mips1 by default), with -O2 or more optimazation specified,
    // in case that the final statement is a function call, the compiler will pop the using stack first
    // and then branch to the final function call. Unfortunately, here we just reset the SP to
    // the top of the stack space, so the stack of the final function falls beyond the stack top.
    // The workaournd is to add a dummy statement as the final statement so as to avoid the optimization.
    asm volatile("nop");
}


// =============================================================================
//  boot_BreakBootMain
// -----------------------------------------------------------------------------
/// Called when launching the code in elfmaster debug mode. The purpose is to
/// break when the code starts, to begin a debugging session.
///
/// Hi! This is Elf Master. You got here because you started me
/// with the "Run Program" option. Your code has been restarted,
/// and is currently stalled at the beginning. You can hit
/// "continue" to unstall it, or you can set some additional
/// breakpoints where you need to.
// =============================================================================
PROTECTED VOID boot_BreakBootMain( VOID )
{
    // WELCOME TO ELFMASTER!
    BREAKPOINT;
    main(0, 0);
}


#ifndef CHIP_HAS_AP
#ifndef RAMRUN
// =============================================================================
// boot_FlashProgrammed
// -----------------------------------------------------------------------------
/// Check if the flash has been programmed.
// =============================================================================
PROTECTED BOOL boot_FlashProgrammed(VOID)
{
    return (*(BOOT_FLASH_PROGRAMMED_PATTERN_ADDRESS) == BOOT_FLASH_PROGRAMMED_PATTERN);
}
#endif
#endif


// =============================================================================
//  boot_Loader
// -----------------------------------------------------------------------------
/// Real implementation of the boot loader, loading code after the stack has
/// been reseted in #boot_LoaderEnter();
///
/// @param param This parameter as a dummy value when we boot normally from ROM,
/// but it has a special value when the function is called by a jump command
/// of a Boot Monitor (Host, USB or UART). This is used to change the main of
/// the code (where we will jump at the end of the boot loading sequence).
// =============================================================================
PROTECTED VOID boot_Loader(UINT32 param)
{
    UINT32* src;
    UINT32* dst;

    // NO FUNCTION CALL ALLOWED AS LONG AS THE CODE HAS NOT BEEN
    // COPIED FROM FLASH TO RAM...

    // If a WD occured, treat it before initializing the bb SRAM.
    if (hwp_sysCtrl->Reset_Cause & SYS_CTRL_WATCHDOG_RESET_HAPPENED)
    {
        if  ((g_halWatchdogCtx.validityMarker == 0xC001C001) ||
                (g_halWatchdogCtx.validityMarker == 0xDEADBEEF) )
        {
            // This is a real watchdog and the structure is valid.
            UINT32* saved_context_ptr = (UINT32*) KSEG1(REG_BB_SRAM_BASE);
            UINT32 i;

            // Copy from BB_SRAM into External sram structure.
            for (i = 0; i < sizeof(HAL_DBG_ERROR_CTX_T)/4; i++)
            {
                *(&(g_halWatchdogCtx.errorContext.zero) + i) =
                    *(saved_context_ptr + i);
            }
            // reset the marker to normal
            g_halWatchdogCtx.validityMarker = 0xC001C001;
            hal_TimeWdRecover();
        }
        else
        {
            // The structure is not valid behave as in case of soft reset.
            g_halWatchdogCtx.validityMarker = 0xDEADBEEF;
            //hwp_sysCtrl->Sys_Rst_Clr = SYS_CTRL_WATCHDOG_STATUS;
        }
    }
    else
    {
        g_halWatchdogCtx.validityMarker = 0xC001C001;
    }

    // Set the global variables from ROM not set by the ROM code.
    xcpu_error_code   = 0;
    xcpu_error_status = 0;
    xcpu_sp_context   = 0;

#ifndef CHIP_HAS_AP
    // Copy code & data from ROM space to Dualport SRAM.
    for (src = (UINT32*) &_dualport_sram_flash_start_location,
            dst = (UINT32*)(&_dualport_sram_start);
            src < (UINT32*) &_dualport_sram_flash_end_location;
            src++, dst++)
    {
        *dst = *src;
    }

    // Clear BSS in Dualport SRAM.
    for (dst = (UINT32*) &_dualport_bss_start;
            dst < (UINT32*) &_dualport_bss_end;
            dst++)
    {
        *dst = 0;
    }
#endif

    // Copy irq code from ROM space to internal SRAM.
    for (src = (UINT32*) &_irq_sys_sram_flash_start_location,
            dst = (UINT32*)(&_irq_sys_sram_start);
            src < (UINT32*) &_irq_sys_sram_flash_end_location;
            src++, dst++)
    {
        *dst = *src;
    }

    // Copy code & data from ROM space to internal SRAM.
    for (src = (UINT32*) &_sys_sram_flash_start_location,
            dst = (UINT32*)(&_sys_sram_start);
            src < (UINT32*) &_sys_sram_flash_end_location;
            src++, dst++)
    {
        *dst = *src;
    }

    // Clear BSS in Internal SRAM.
    for (dst = (UINT32*) &_sys_sram_bss_start;
            dst < (UINT32*) &_sys_sram_bss_end;
            dst++)
    {
        *dst = 0;
    }

#ifdef CHIP_HAS_AP
    // Copy ram data from load space to execution space.
    for (src = (UINT32*) &_ram_data_load_start,
            dst = (UINT32*) &_ram_data_start;
            src < (UINT32*) &_ram_data_load_end;
            src++, dst++)
    {
        *dst = *src;
    }
#else
    // Copy code & data from ROM space to RAM.
    for (src = (UINT32*) &_ram_flash_start_location,
            dst = (UINT32*)(&_ram_start);
            src < (UINT32*) &_ram_flash_end_location;
            src++, dst++)
    {
        *dst = *src;
    }
#endif

    // clear BSS in External RAM
    for (dst = (UINT32*) &_ram_bss_start;
            dst < (UINT32*) &_ram_bss_end;
            dst++)
    {
        *dst = 0;
    }

#ifdef REWRITE_ROM_AT_BOOT
#ifdef FPGA
    // Copy data from flash space to internal ROM.

    // Unlock.
    hwp_memBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_ENABLE;

    for (src = (UINT32*) &_int_rom_flash_start_location,
            dst = (UINT32*) &_int_rom_start;
            src < (UINT32*) &_int_rom_flash_end_location;
            src++, dst++)
    {
        *dst = *src;
    }

    // Lock.
    hwp_memBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_DISABLE;
#endif // FPGA
#endif // REWRITE_ROM_AT_BOOT


#if (CHIP_HAS_PROXY_MEMORY == 1)
    // Load proxy memory content.
    for (src = (UINT32*) &_proxy_memory_flash_start_location,
            dst = (UINT32*) &_proxy_memory_start;
            src < (UINT32*) &_proxy_memory_flash_end_location;
            src++, dst++)
    {
        *dst = *src;
    }

    // FIXME For test purpose, move the Irq Stack
    // atop the proxy memory !
    sxr_IrqStack_pointer = (UINT32)&_proxy_memory_top;
#endif

#if(XCPU_CACHE_MODE_WRITE_BACK==1)
    boot_FlushDCache(TRUE);
#ifndef XCPU_CACHE_FLUSH_V2
    boot_InvalidDCache();
#endif
#endif

#ifdef MMI_SLT_MODE
    SLT_RAM_TEST_FLAG = hal_Slt_Ram_Test();
#endif

#ifdef BOOT_RAM_DATA_COPY_CHECK
    // Compare the section ram text with flash.
    for (src = (UINT32*) &_ram_flash_start_location,
            dst = (UINT32*)(&_ram_start);
            src < (UINT32*) &_ram_flash_end_location;
            src++, dst++)
    {
        if(*dst != *src)
        {
            asm volatile("break 1; nop; nop;");
        }

    }
#endif

#ifdef BOOT_MONITOR_NO_HAL_OPEN
    // Some applications (such as flash_programmer) will take care hal_Open
    // by itself (with minimal version).
    main(0, 0); // never return
#endif

    // Enable and configure the page spies.
    hal_DbgPageProtectSetup(HAL_DBG_PAGE_IRQ_STACKWATCH, HAL_DBG_PAGE_WRITE_TRIGGER,
                            (UINT32)&_sys_sram_irq_stackwatch_start, (UINT32)&_sys_sram_irq_stackwatch_end);
    hal_DbgPageProtectEnable(HAL_DBG_PAGE_IRQ_STACKWATCH);

    hal_DbgPageProtectSetup(HAL_DBG_PAGE_CODEWATCH, HAL_DBG_PAGE_WRITE_TRIGGER,
                            (UINT32)&_ram_txt_start, (UINT32)&_ram_txt_end);
    hal_DbgPageProtectEnable(HAL_DBG_PAGE_CODEWATCH);

    hal_DbgPageProtectSetup(HAL_DBG_PAGE_INT_CODEWATCH, HAL_DBG_PAGE_WRITE_TRIGGER,
                            (UINT32)&_sys_sram_txt_start, (UINT32)&_sys_sram_txt_end);
    hal_DbgPageProtectEnable(HAL_DBG_PAGE_INT_CODEWATCH);

#ifdef PAGE_SPY_AT_ZERO
    hal_DbgPageProtectSetup(HAL_DBG_PAGE_4, HAL_DBG_PAGE_READWRITE_TRIGGER, (UINT32)g_SpZero, (UINT32)g_SpZero + sizeof(g_SpZero)-1);
    hal_DbgPageProtectEnable(HAL_DBG_PAGE_4);
#endif

    hal_DbgPageProtectSetup(HAL_DBG_PAGE_6, HAL_DBG_PAGE_WRITE_TRIGGER,
                            (UINT32)&_irq_sys_sram_start, (UINT32)&_irq_sys_sram_end);
    hal_DbgPageProtectEnable(HAL_DBG_PAGE_6);

    hal_DbgPageProtectSetup(HAL_DBG_PAGE_7, HAL_DBG_PAGE_WRITE_TRIGGER,
                            (UINT32)&_boot_sys_sram_txt_start, (UINT32)&_boot_sys_sram_txt_end);
    hal_DbgPageProtectEnable(HAL_DBG_PAGE_7);

#ifndef _FLASH_PROGRAMMER
#ifdef CHIP_HAS_AP
#if (CHIP_HAS_BCPU_ROM) && !defined(NO_BCPU_IMAGE)
    hal_DbgPageProtectSetup(HAL_DBG_PAGE_8, HAL_DBG_PAGE_WRITE_TRIGGER,
                            (UINT32)&_flash_start, (UINT32)&_bcpu_ram_data_start);
    hal_DbgPageProtectEnable(HAL_DBG_PAGE_8);

    hal_DbgPageProtectSetup(HAL_DBG_PAGE_9, HAL_DBG_PAGE_WRITE_TRIGGER,
                            (UINT32)&_bcpu_flash_end, (UINT32)&_ram_data_start);
    hal_DbgPageProtectEnable(HAL_DBG_PAGE_9);
#else
    hal_DbgPageProtectSetup(HAL_DBG_PAGE_8, HAL_DBG_PAGE_WRITE_TRIGGER,
                            (UINT32)&_flash_start, (UINT32)&_ram_data_start);
    hal_DbgPageProtectEnable(HAL_DBG_PAGE_8);
#endif
#else // !CHIP_HAS_AP
    hal_DbgPageProtectSetup(HAL_DBG_PAGE_8, HAL_DBG_PAGE_WRITE_TRIGGER,
                            (UINT32)&_boot_ram_txt_start, (UINT32)&_boot_ram_txt_end);
    hal_DbgPageProtectEnable(HAL_DBG_PAGE_8);

#ifdef CHIP_DIE_8955
    // There are only 4MB PSRAM, but the following addresses are mapped to PSRAM also.
    hal_DbgPageProtectSetup(HAL_DBG_PAGE_9, HAL_DBG_PAGE_WRITE_TRIGGER,
                            (UINT32)0x82400000, 0x84000000);
    hal_DbgPageProtectEnable(HAL_DBG_PAGE_9);
    // TO BE REMOVED: in previous chips, these addresses are mapped to PSRAM also.
    hal_DbgPageProtectSetup(HAL_DBG_PAGE_11, HAL_DBG_PAGE_WRITE_TRIGGER,
                            (UINT32)0x80000000, 0x80400000);
    hal_DbgPageProtectEnable(HAL_DBG_PAGE_11);
    // Set page spy for whole PSRAM access, it will be used later.
    hal_DbgPageProtectSetup(HAL_DBG_PAGE_14, HAL_DBG_PAGE_READWRITE_TRIGGER,
                            (UINT32)0x82000000, 0x84000000);
#else
    hal_DbgPageProtectSetup(HAL_DBG_PAGE_9, HAL_DBG_PAGE_WRITE_TRIGGER,
                            ((UINT32)&_boot_ram_txt_start)&~0x2000000, ((UINT32)&_boot_ram_txt_end)&~0x2000000);
    hal_DbgPageProtectEnable(HAL_DBG_PAGE_9);
#endif
#endif // !CHIP_HAS_AP
#endif // !_FLASH_PROGRAMMER

    // FUNCTIONS CAN BE CALL NOW!
    // Enable exception (gdb breaks), Host, Debug and Page Spy external it,
    // but not regular external it.
    asm volatile("li    $27,    (0x0001 | 0xe000)\n\t"
                 "mtc0  $0, $13\n\t"
                 "nop\n\t"
                 "mtc0  $27, $12\n\t"
                 // k1 is status for os irq scheme.
                 "li    $27, 1");
    hal_HstSendEvent(BOOT_LOADER_EVENT_START_CODE);

    hal_Open(tgt_GetHalConfig());

    BOOT_PROFILE_PULSE(XCPU_MAIN);

    // Selects where we will boot: normal main or calib stub main.
    switch(param)
    {
        case CALIB_MAGIC_TAG:
#ifndef CHIP_HAS_AP
#ifndef RAMRUN
            if (!boot_FlashProgrammed())
            {
                mon_Event(0xf1a5b00e);
                while(1);
            }
#endif
#endif
            calib_StubMain(0, 0);
            break;
        case BOOT_MAIN_TAG_BREAKBOOT:
            // Gdb needs the flash to be open to be able to step in the romulator!!!
            boot_BreakBootMain();
            break;
        default:
            main(0, 0);
            break;
    }
}

