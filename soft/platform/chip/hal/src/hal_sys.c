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

#include "tgt_hal_cfg.h"
#include "string.h"

#include "global_macros.h"
#include "sys_ctrl.h"
#include "tcu.h"

#if (CHIP_HAS_ASYNC_BCPU)
#include "bb_ctrl.h"
#endif

#include "cfg_regs.h"
#include "mem_bridge.h"
#include "calendar.h"
#include "sys_irq.h"

#include "halp_lps.h"
#include "halp_config.h"
#include "halp_sys.h"
#include "halp_ebc.h"
#include "halp_host.h"
#include "halp_camera.h"
#include "halp_gdb_stub.h"
#ifdef CHIP_HAS_AP
#include "halp_ap_monitor.h"
#endif
#include "halp_debug.h"

#include "hal_debug.h"
#include "hal_mem_map.h"
#include "hal_sys.h"
#include "hal_clk.h"
#include "hal_lps.h"
#include "hal_host.h" // this is to send debug events
#include "hal_rda_audio.h"
#include "hal_sys_cfg.h"
#include "boot.h"
#include "boot_sys.h"
#include "boot_sector_driver.h"

#if (CHIP_HAS_USB == 1)
#include "halp_usb_monitor.h"
#include "halp_usb.h"
#include "boot_usb_monitor.h"
#endif
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
#include "hal_cache.h"
#endif
#include "pmd_m.h"
#include "memd_m.h"

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#include "rfd_xcv.h"
#endif


// =============================================================================
//  MACROS
// =============================================================================

#define HAL_SYS_MAX_SPEED   CHIP_MAX_SPEED

// Don't limit the clock if we use PXTS on RAM or TRACE.
#if (defined(PXTS_MAX_SUPPORTED_FREQUENCY) && !defined(HAL_PROFILE_NOT_ON_ROMULATOR))
#if (PXTS_MAX_SUPPORTED_FREQUENCY < HAL_SYS_MAX_SPEED)
#undef HAL_SYS_MAX_SPEED
#define HAL_SYS_MAX_SPEED PXTS_MAX_SUPPORTED_FREQUENCY
#endif
#endif

/// extern HAL_SYS_CLOCK_OUT_FREQ_T enum
#define HAL_SYS_CLOCK_OUT_OFF 0

#define SYS_SHUTDOWN_MAGIC_NUM 0xa1b2c3fe

#define HAL_SRAM_SCRATCH_SIZE   512

#define HAL_TCU_DEFAULT_WRAP_COUNT  4999


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
#if defined(__RESTORE_RESET_NORMAL_POWERON_MODE__)
PRIVATE UINT32 HAL_DATA_BACKUP g_halRestoretoShutdownFlag;
#endif

PRIVATE UINT32 HAL_BSS_INTERNAL g_halSramScratch[HAL_SRAM_SCRATCH_SIZE/4];

// ============================================================================
// g_halSysSystemFreq
// ----------------------------------------------------------------------------
/// Global var to have a view of the system frequency.
/// At boot, we are first on the 26Mhz Rf clock.
// ============================================================================
#ifdef CHIP_XTAL_CLK_IS_52M
PROTECTED HAL_SYS_FREQ_T HAL_BOOT_DATA_INTERNAL g_halSysSystemFreq = HAL_SYS_FREQ_52M;
#else
PROTECTED HAL_SYS_FREQ_T HAL_BOOT_DATA_INTERNAL g_halSysSystemFreq = HAL_SYS_FREQ_26M;
#endif

// ============================================================================
// g_halNumberOfUserPerFreq
// ----------------------------------------------------------------------------
/// This array is used to calculate the minimum system frequency to set
/// (that is the maximum required by a module among all the module) in a
/// constant time (relative to the number of users).
///
// ============================================================================
PROTECTED UINT32 g_halNumberOfUserPerFreq[HAL_SYS_FREQ_SCAL_QTY];

// =============================================================================
// g_halSysClockOutRegistry
// -----------------------------------------------------------------------------
/// This array is used to store the frequency required by each users for clock out.
// =============================================================================
PROTECTED UINT32 g_halSysClockOutRegistry[HAL_SYS_CLOCK_OUT_ID_QTY] = {0};

#ifdef CHIP_DIE_8809E2
HAL_SYS_PLL_CONFIG_02H_T g_halSysPllCfg02h[HAL_SYS_PLL_MODE_QTY] =
{
    {{
            .Pll_lock = 0,
            .Pll_sdm_clk_test_en = 0,
            .Pll_refmulti2 = 0,
            .Pll_vco_high_test = 0,
            .Pll_vco_low_test = 0,
            .Pll_test_en = 0,
            .Pll_vreg_bit = 8,
            .Pd_pll_dr = 0,
            .Pd_pll_reg = 1,
            .Pll_reset_dr = 0,
            .Pll_reset_reg = 1,
        }
    },
    {{
            .Pll_lock = 0,
            .Pll_sdm_clk_test_en = 0,
            .Pll_refmulti2 = 0,
            .Pll_vco_high_test = 0,
            .Pll_vco_low_test = 0,
            .Pll_test_en = 0,
            .Pll_vreg_bit = 1,
            .Pd_pll_dr = 0,
            .Pd_pll_reg = 1,
            .Pll_reset_dr = 0,
            .Pll_reset_reg = 1,
        }
    },
};

HAL_SYS_PLL_CONFIG_01H_T g_halSysPllCfg01h[HAL_SYS_PLL_MODE_QTY] =
{
    {{
            .Pll_clkout_en =1,
            .Pll_cpaux_bit =4,
            .Pll_filter_ibit =4,
            .Pll_cp_bit =4,
            .Pll_int_mode = 1,
            .Pll_sdm_clk_sel_rst = 1,
            .Pll_sdm_clk_sel_nor =0,
        }
    },
    {{
            .Pll_clkout_en =1,
            .Pll_cpaux_bit =1,
            .Pll_filter_ibit =1,
            .Pll_cp_bit =1,
            .Pll_int_mode =1 ,
            .Pll_sdm_clk_sel_rst = 1,
            .Pll_sdm_clk_sel_nor =0,
        }
    },
};

UINT32 g_halXcvReg_07h;
UINT32 g_halXcvReg_20h;
#endif

PRIVATE UINT32 g_halSysAuxClkUserNum = 0;

PRIVATE UINT32 g_halSys32kClkUserNum = 0;

PRIVATE VOID (*g_halBootBeforeMonCallback)() = NULL;

#ifdef __PRJ_WITH_SDRAM__
PRIVATE UINT32 HAL_DATA_INTERNAL g_Freq4Sdram;
#endif

#if(XCPU_CACHE_MODE_WRITE_BACK==1)
PRIVATE UINT32 HAL_UNCACHED_DATA_BACKUP g_halSysStateShutdownFlag;
#else
PRIVATE UINT32 HAL_DATA_BACKUP g_halSysStateShutdownFlag;
#endif

PRIVATE BOOL g_halSysBootFromShutdown = FALSE;

#if (CHIP_HAS_USB == 1)
PRIVATE BOOL g_halUsbHostEnable = FALSE;
#endif /* CHIP_HAS_USB */

#ifdef CHIP_HAS_AP
PRIVATE BOOL g_halApHostEnable = FALSE;
#endif

typedef struct
{
    BOOL  ddrIsHighFreq;
    UINT8 activeSimCount;
    UINT32 tcuWrapValue;
} HAL_SYS_CONDITION_T;

PRIVATE HAL_SYS_CONDITION_T g_halSysCondition =
{
    .ddrIsHighFreq = TRUE,
    .activeSimCount = 0,
    .tcuWrapValue = HAL_TCU_DEFAULT_WRAP_COUNT,
};

// =============================================================================
//  FUNCTIONS
// =============================================================================
#ifdef CHIP_DIE_8809E2
extern BOOL rfd_XcvRegForceWrite(UINT32 page, UINT32 addr, UINT32 data);
extern BOOL rfd_XcvRegForceRead(UINT32 page, UINT32 addr, UINT32 *pData);
#endif

// =============================================================================
// hal_SysStartBcpu
// -----------------------------------------------------------------------------
/// Start the BCPU with a stack to at the given address.
/// @param bcpuMain Main function to run on the BCPU.
/// @param stackStartAddr Stack top for the program running on the BCPU
// =============================================================================
PUBLIC VOID hal_SysStartBcpu(VOID* bcpuMain, VOID* stackStartAddr)
{
#if (CHIP_HAS_BCPU_ROM)
#ifdef REWRITE_ROM_AT_BOOT
#ifdef FPGA
    // Copy data from flash space to internal ROM.
    // This must be done before starting BCPU, as the addresses of
    // bcpu_stack_base and bcpu_main_entry might have been changed.

    UINT32 *src, *dst;
    // Unlock.
    hwp_bbMemBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_ENABLE;

    for (src = (UINT32*) &_bcpu_rom_flash_start_location,
            dst = (UINT32*) &_bcpu_rom_start;
            src < (UINT32*) &_bcpu_rom_flash_end_location;
            src++, dst++)
    {
        *dst = *src;
    }

    // Lock.
    hwp_bbMemBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_DISABLE;
#endif // FPGA
#endif // REWRITE_ROM_AT_BOOT
#endif // CHIP_HAS_BCPU_ROM

    UINT32 status = hal_SysEnterCriticalSection();
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
    *(UINT32*)HAL_SYS_GET_UNCACHED_ADDR(&bcpu_stack_base)  = (UINT32) stackStartAddr;
    *(UINT32*)HAL_SYS_GET_UNCACHED_ADDR(&bcpu_main_entry) = (UINT32) bcpuMain;
#else
    bcpu_stack_base = (UINT32) stackStartAddr;
    bcpu_main_entry = (UINT32*) bcpuMain;
#endif
#if (!CHIP_HAS_ASYNC_BCPU)
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Clk_BB_Enable = SYS_CTRL_ENABLE_BB_BCPU;
    hwp_sysCtrl->BB_Rst_Clr = SYS_CTRL_CLR_RST_BCPU;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
#else // (CHIP_HAS_ASYNC_BCPU)
    hwp_bbCtrl->REG_DBG = BB_CTRL_PROTECT_UNLOCK;
    hwp_bbCtrl->Clk_BB_Enable = BB_CTRL_ENABLE_BB_BCPU;
    hwp_bbCtrl->BB_Rst_Clr = BB_CTRL_CLR_RST_BCPU;
    hwp_bbCtrl->REG_DBG = BB_CTRL_PROTECT_LOCK;
#endif // (!CHIP_HAS_ASYNC_BCPU)
    hal_SysExitCriticalSection(status);
}


// =============================================================================
// hal_SysStopBcpu
// -----------------------------------------------------------------------------
/// Stop the BCPU
// =============================================================================
PUBLIC VOID hal_SysStopBcpu(VOID)
{
    UINT32 status = hal_SysEnterCriticalSection();
#if (!CHIP_HAS_ASYNC_BCPU)
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->BB_Rst_Set = SYS_CTRL_SET_RST_BCPU;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
#else // (CHIP_HAS_ASYNC_BCPU)
    hwp_bbCtrl->REG_DBG = BB_CTRL_PROTECT_UNLOCK;
    hwp_bbCtrl->BB_Rst_Set = BB_CTRL_SET_RST_BCPU;
    hwp_bbCtrl->REG_DBG = BB_CTRL_PROTECT_LOCK;
#endif // (!CHIP_HAS_ASYNC_BCPU)
    hal_SysExitCriticalSection(status);
}


#ifdef CHIP_HAS_WCPU

// =============================================================================
// hal_SysWcpuClkOut
// -----------------------------------------------------------------------------
/// open WD MODERM clock
// =============================================================================
VOID hal_SysWcpuClkOut(VOID)
{
    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->WD_Pll_Ctrl = SYS_CTRL_WD_PLL_ENABLE|SYS_CTRL_WD_PLL_LOCK_RESET|SYS_CTRL_WD_PLL_CLK_FAST_ENABLE;
    hwp_sysCtrl->WD_Sel_Clock = hwp_sysCtrl->WD_Sel_Clock&(~SYS_CTRL_WD_SYS_SEL_FAST_SLOW);
    hwp_sysCtrl->Clk_Other_Enable = SYS_CTRL_ENABLE_OC_WD_ADC | SYS_CTRL_ENABLE_OC_WD_CHIP|
                                    SYS_CTRL_ENABLE_OC_WD_TURBO | SYS_CTRL_ENABLE_OC_WD_OSC;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SysExitCriticalSection(status);
}

// =============================================================================
// hal_SysStartWcpu
// -----------------------------------------------------------------------------
/// Start the WCPU with a stack to at the given address.
/// @param wcpuMain Main function to run on the WCPU.
/// @param stackStartAddr Stack top for the program running on the WCPU
// =============================================================================
PUBLIC VOID hal_SysStartWcpu(VOID* wcpuMain, VOID* stackStartAddr)
{
    hal_SysWcpuClkOut();
    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->WD_Rst_Clr = SYS_CTRL_CLR_WD_RST_WCPU;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SysExitCriticalSection(status);
}


// =============================================================================
// hal_SysStopWcpu
// -----------------------------------------------------------------------------
/// Stop the WCPU
// =============================================================================
PUBLIC VOID hal_SysStopWcpu(VOID)
{
    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->WD_Rst_Set = SYS_CTRL_CLR_WD_RST_WCPU;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SysExitCriticalSection(status);
}
#endif

PRIVATE VOID hal_SysCheckSetTraceEnable(HAL_SYS_CONDITION_T *cond)
{
    if (!cond->ddrIsHighFreq)
    {
        if (cond->activeSimCount > 1
                || cond->tcuWrapValue < HAL_TCU_DEFAULT_WRAP_COUNT)
        {
            sxs_SetTraceEnabled(FALSE);
        }
        else
        {
            sxs_SetTraceEnabled(TRUE);
        }
    }
}

#ifdef CHIP_XTAL_CLK_IS_52M
PUBLIC BOOL hal_SysRequestDdrHighFreq(BOOL active)
{
    HAL_SYS_CONDITION_T *cond = &g_halSysCondition;
    if (active == cond->ddrIsHighFreq)
        return TRUE;

    if (!pmd_IsOpen())
        return FALSE;

    UINT32 sc = hal_SysEnterCriticalSection();
    BOOL changed;
    if (active)
    {
        HAL_PROFILE_WINDOW_EXIT(XTAL_SEL_52M);
        changed = memd_AdjustRamClk(HAL_SYS_MEM_DEFAULT_FREQ, TRUE);
        HAL_PROFILE_WINDOW_ENTER(XTAL_SEL_52M);

        if (!changed)
        {
            // we should make sure turn on PLL must be success
            asm("break 1");
        }

        cond->ddrIsHighFreq = active;
        HAL_PROFILE_WINDOW_EXIT(XTAL_SEL_52M);

        hal_HstTraceSetDdrEnabled(TRUE);
        sxs_SetTraceEnabled(TRUE);
    }
    else
    {
        changed = FALSE;
        if (hal_HstTraceSetDdrEnabled(FALSE))
        {
            HAL_PROFILE_WINDOW_ENTER(XTAL_SEL_52M);
            changed = memd_AdjustRamClk(HAL_SYS_MEM_LPS_FREQ, FALSE);
            HAL_PROFILE_WINDOW_EXIT(XTAL_SEL_52M);

            if (!changed)
            {
                hal_HstTraceSetDdrEnabled(TRUE);
            }
            else
            {
                cond->ddrIsHighFreq = active;
                hal_SysCheckSetTraceEnable(cond);

                HAL_PROFILE_WINDOW_ENTER(XTAL_SEL_52M);
            }
        }
    }

    hal_SysExitCriticalSection(sc);
    return changed;
}

BOOL hal_SysDdrIsHighFreq(VOID)
{
    return g_halSysCondition.ddrIsHighFreq;
}
#endif

VOID hal_SysSetActiveSimCount(UINT8 count)
{
    UINT32 sc = hal_SysEnterCriticalSection();

    g_halSysCondition.activeSimCount = count;
    hal_SysCheckSetTraceEnable(&g_halSysCondition);

    hal_SysExitCriticalSection(sc);
}

VOID hal_SysSetTcuWrapValue(UINT32 value)
{
    UINT32 sc = hal_SysEnterCriticalSection();

    g_halSysCondition.tcuWrapValue = value;
    hal_SysCheckSetTraceEnable(&g_halSysCondition);

    hal_SysExitCriticalSection(sc);
}

// =============================================================================
// hal_SysGetFreq
// -----------------------------------------------------------------------------
/// Get the current system clock frequency.
/// @return The current system clock frequency.
// =============================================================================
PUBLIC HAL_SYS_FREQ_T hal_SysGetFreq(VOID)
{
    HAL_CLK_T *clk = hal_ClkGet(FOURCC_SYSTEM);
    return (clk != NULL) ? hal_ClkGetRate(clk) : HAL_CLK_RATE_52M;
}

#if defined(__RESTORE_RESET_NORMAL_POWERON_MODE__)
void Hal_SetRestoretoShutdownFlag(void)
{
    g_halRestoretoShutdownFlag = SYS_SHUTDOWN_MAGIC_NUM;
}

PUBLIC BOOL  Hal_GetRestoretoShutdownFlag(void)
{
    if(g_halRestoretoShutdownFlag == SYS_SHUTDOWN_MAGIC_NUM)
    {
        hal_HstSendEvent(0x20160001);
        return TRUE;
    }
    hal_HstSendEvent(0x20160002);
    return FALSE;
}

void Hal_ClrRestoretoShutdownFlag(void)
{
    g_halRestoretoShutdownFlag = 0;
}
#endif

// =============================================================================
// hal_SysShutdownBottom
// -----------------------------------------------------------------------------
/// The bottom half of hal_SysShutdown. In 8955, this function shouldn't use
/// RAM as stack.
// =============================================================================
PRIVATE VOID __attribute__ ((nomips16, noinline, noreturn)) HAL_FUNC_ROM hal_SysShutdownBottom(VOID)
{
#ifdef CHIP_DIE_8955
    hal_DcacheFlushAll(TRUE);

    // DDR shouldn't be access from here
    pmd_Deactive();
#endif

    // Power down
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->WakeUp  = 0;

    // Force to flush the write buffer.
    UINT32 dummyRead __attribute__((unused)) = hwp_sysCtrl->WakeUp;

    // Wait 1 second ...
    hal_TimDelay(HAL_TICK1S);

    // Just reset so we don't get stuck here!
#ifdef CHIP_DIE_8955
    hwp_sysCtrl->Sys_Rst_Set= SYS_CTRL_SOFT_RST;
#else
    hal_SysRestart();
#endif

    while (TRUE);
}

// =============================================================================
// hal_SysShutdown
// -----------------------------------------------------------------------------
/// A call to this function shuts the system down.
// =============================================================================
PUBLIC VOID hal_SysShutdown(VOID)
{
    // Check that the alarm has finished to be set
    // if an alarm has been programmed before shutting
    // down
    while (hwp_calendar->Cmd & CALENDAR_ALARM_ENABLE_SET);

    UINT32 status __attribute__((unused))  = hal_SysEnterCriticalSection();

    hal_HstSendEvent(0x1a11dead);

    // Enter power-save mode
    pmd_Close();

//  rfd_XcvClose();

    // Set the shutdown flag to record that we just went through the shutdown
    // process
    g_halSysStateShutdownFlag = SYS_SHUTDOWN_MAGIC_NUM;

    hal_SysShutdownBottom();
}


// =============================================================================
// hal_SysSoftReset
// -----------------------------------------------------------------------------
/// Does a soft reset on the chip.
/// Generally, you should NOT use this function, use hal_SysRestart() instead.
// =============================================================================
PUBLIC VOID HAL_FUNC_RAM hal_SysSoftReset(VOID)
{
    UINT32 status __attribute__((unused))  = hal_SysEnterCriticalSection();

#ifdef __SPIFLSH_SUPPORT_SOFTRESET__
    memd_FlashRst();
#endif

    hal_HstSendEvent(0x1a110011);

    // Enter power-save mode
    //  pmd_Close();  //fix bug: can not reboot from restore factory setting

    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;

#if (CHIP_HAS_ASYNC_BCPU)
    hwp_sysCtrl->Sys_Rst_Set= SYS_CTRL_SYS_SOFT_RST;
#else
    hwp_sysCtrl->Sys_Rst_Set= SYS_CTRL_SOFT_RST;
#endif

    while (1); // We die here ...

    // hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;

    // hal_SysExitCriticalSection(status);
}
// =============================================================================
PUBLIC VOID HAL_FUNC_RAM hal_SysSoftResetNoFlash(VOID)
{
    UINT32 status __attribute__((unused))  = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
#if (CHIP_HAS_ASYNC_BCPU)
    hwp_sysCtrl->Sys_Rst_Set= SYS_CTRL_SYS_SOFT_RST;
#else
    hwp_sysCtrl->Sys_Rst_Set= SYS_CTRL_SOFT_RST;
#endif
    while (1); // We die here ...

}



// =============================================================================
// hal_SysRestart
// -----------------------------------------------------------------------------
/// A call to this function reboots the system.
/// First, this function does a power cycle on USB
// =============================================================================
PUBLIC VOID HAL_FUNC_RAM hal_SysRestart(VOID)
{
#if (CHIP_HAS_USB == 1)
    UINT32 startTime;

    startTime = hal_TimGetUpTime();
    hal_UsbReset(TRUE);
    while(hal_TimGetUpTime() < startTime + 8000);
    hal_UsbReset(FALSE);
#endif // (CHIP_HAS_USB == 1)

    hal_SysSoftReset();
}



// =============================================================================
// hal_SysDisablePLL
// -----------------------------------------------------------------------------
/// Disable PLL and switch on low clock
// =============================================================================
PROTECTED VOID hal_SysDisablePLL(VOID)
{
    boot_SysShutdownPLL();
}



// =============================================================================
// hal_SysEnablePLL
// -----------------------------------------------------------------------------
/// Enable PLL
// =============================================================================
PROTECTED VOID hal_SysEnablePLL(VOID)
{
    UINT32 tcuClock;
#ifdef CHIP_HAS_AP
#if (CHIP_TCU_CLK == 26000000)
    tcuClock = SYS_CTRL_TCU_13M_L_26M;
#else
    tcuClock = SYS_CTRL_TCU_13M_L_13M;
#endif
#else // !CHIP_HAS_AP
#if (CHIP_TCU_CLK == 26000000)
    tcuClock = SYS_CTRL_TCU_13M_SEL_26M;
#else
    tcuClock = SYS_CTRL_TCU_13M_SEL_13M;
#endif
#endif // !CHIP_HAS_AP

    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;

    // Avoid power up PLL twice
    // ( If PLL must be powered up again, first the clock must be switched to 26M.
    //   Then the external RAM is considered unaccessible, meaning that the
    //   related codes should be located in internal SRAM.
    //   After that PLL can be powered down and then powered up as usual.)
    if ((hwp_sysCtrl->Sel_Clock & SYS_CTRL_PLL_LOCKED) == 0)
    {

#ifdef HAL_SYS_SETUP_CLOCK_32K

        // Turning off the Pll and reset of the Lock detector
        hwp_sysCtrl->Pll_Ctrl = SYS_CTRL_PLL_ENABLE_POWER_DOWN
#ifndef CHIP_DIE_8955
                                | SYS_CTRL_PLL_BYPASS_PASS//no
#endif
                                | SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE
                                | SYS_CTRL_PLL_LOCK_RESET_RESET
#ifndef CHIP_HAS_AP
                                | SYS_CTRL_BB_PLL_ENABLE_POWER_DOWN
                                | SYS_CTRL_BB_PLL_BYPASS_PASS
                                | SYS_CTRL_BB_PLL_CLK_FAST_ENABLE_DISABLE
                                | SYS_CTRL_BB_PLL_LOCK_RESET_RESET
#endif
                                ;

#else // !HAL_SYS_SETUP_CLOCK_32K

        // Turning off the Pll and reset of the Lock detector
        hwp_sysCtrl->Pll_Ctrl = SYS_CTRL_PLL_ENABLE_POWER_DOWN
#ifndef CHIP_DIE_8955
                                | SYS_CTRL_PLL_BYPASS_PASS
#endif
                                | SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE
                                | SYS_CTRL_PLL_LOCK_RESET_RESET
#ifndef CHIP_HAS_AP
#ifndef CHIP_DIE_8955

                                | SYS_CTRL_BB_PLL_ENABLE_POWER_DOWN
                                | SYS_CTRL_BB_PLL_BYPASS_PASS
                                | SYS_CTRL_BB_PLL_CLK_FAST_ENABLE_DISABLE
                                | SYS_CTRL_BB_PLL_LOCK_RESET_RESET
#endif
#endif
                                ;

#ifdef USE_DIG_CLK_ONLY_AT_IDLE
        hwp_sysCtrl->Pll_Ctrl = SYS_CTRL_PLL_ENABLE_POWER_DOWN
#ifndef CHIP_DIE_8955
                                | SYS_CTRL_PLL_BYPASS_BYPASS
#endif
                                | SYS_CTRL_PLL_CLK_FAST_ENABLE_ENABLE
                                | SYS_CTRL_PLL_LOCK_RESET_RESET
#ifndef CHIP_HAS_AP
#ifndef CHIP_DIE_8955

                                | SYS_CTRL_BB_PLL_ENABLE_POWER_DOWN
                                | SYS_CTRL_BB_PLL_BYPASS_BYPASS
                                | SYS_CTRL_BB_PLL_CLK_FAST_ENABLE_ENABLE
                                | SYS_CTRL_BB_PLL_LOCK_RESET_RESET;
#endif
#endif
#else // !USE_DIG_CLK_ONLY_AT_IDLE
        hwp_sysCtrl->Pll_Ctrl = SYS_CTRL_PLL_ENABLE_ENABLE
#ifndef CHIP_DIE_8955
                                | SYS_CTRL_PLL_BYPASS_PASS
#endif
                                | SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE
                                | SYS_CTRL_PLL_LOCK_RESET_NO_RESET
#ifndef CHIP_HAS_AP
#ifndef CHIP_DIE_8955

                                | SYS_CTRL_BB_PLL_ENABLE_ENABLE
                                | SYS_CTRL_BB_PLL_BYPASS_PASS
                                | SYS_CTRL_BB_PLL_CLK_FAST_ENABLE_DISABLE
                                | SYS_CTRL_BB_PLL_LOCK_RESET_NO_RESET
#endif
#endif
                                ;
#endif // !USE_DIG_CLK_ONLY_AT_IDLE

        // Wait for lock.
        while((hwp_sysCtrl->Sel_Clock & SYS_CTRL_PLL_LOCKED) == 0);

        // Allow clock through
#ifndef USE_DIG_CLK_ONLY_AT_IDLE
        hwp_sysCtrl->Pll_Ctrl |= SYS_CTRL_PLL_CLK_FAST_ENABLE_ENABLE
#ifndef CHIP_HAS_AP
#ifndef CHIP_DIE_8955
                                 | SYS_CTRL_BB_PLL_CLK_FAST_ENABLE_ENABLE
#endif
#endif
                                 ;
#endif // !USE_DIG_CLK_ONLY_AT_IDLE

#ifdef MODULE_LOCK_PLL
#ifndef CHIP_DIE_8955

        hwp_sysCtrl->Sel_Clock &= ~(SYS_CTRL_SYS_SEL_FAST_SLOW|SYS_CTRL_BB_SEL_FAST_SLOW);   //Add by Lixp 0822
#else

        hwp_sysCtrl->Sel_Clock &= ~(SYS_CTRL_SYS_SEL_FAST_SLOW);     //Add by Lixp 0822
#endif
        //hwp_sysCtrl->Pll_Ctrl |= SYS_CTRL_PLL_ENABLE_ENABLE;
#endif



#ifdef USE_DIGITAL_CLK
#ifndef CHIP_DIE_8955
        hwp_sysCtrl->Sel_Clock |= (SYS_CTRL_DIGEN_H_ENABLE | SYS_CTRL_MPMC_SYS_SAME_ENABLE); //no
#endif
#endif // USE_DIGITAL_CLK

#endif // !HAL_SYS_SETUP_CLOCK_32K

    } // (hwp_sysCtrl->Sel_Clock & SYS_CTRL_PLL_LOCKED) == 0

    // Select clock source
#ifdef FPGA

    // Only 39 or 52 MHz available through PLL on FPGA:
    // - 26 MHz is done with the RF clock.
    // - 39 or 52 Mhz is done with the Pll.
    // UINT32 sysCtrlRegConfig = (SYS_STRL_CFG_PLL_FAST_NUM_BIT(1)|SYS_STRL_CFG_PLL_FAST_DENOM(16))&SYS_CTRL_CFG_PLL_FAST_MASK;
    //hwp_sysCtrl->CFG_PLL_FAST_DIV = sysCtrlRegConfig;
#ifdef HAL_SYS_SETUP_CLOCK_26M
    hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_RF
                             | SYS_CTRL_SYS_SEL_FAST_SLOW
#ifndef CHIP_HAS_AP
                             | SYS_CTRL_BB_SEL_FAST_SLOW
#ifndef CHIP_DIE_8955
                             | SYS_CTRL_MPMC_SYS_SAME_ENABLE
#endif
#endif
                             | tcuClock
                             ;
#else // !HAL_SYS_SETUP_CLOCK_26M
    hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_RF
                             | SYS_CTRL_SYS_SEL_FAST_FAST
#ifndef CHIP_HAS_AP
                             | SYS_CTRL_BB_SEL_FAST_FAST
#ifndef CHIP_DIE_8955
                             | SYS_CTRL_MPMC_SYS_SAME_ENABLE
#else
                             | SYS_CTRL_MEM_BRIDGE_SEL_FAST_FAST
                             | SYS_CTRL_SPIFLASH_SEL_FAST_FAST
                             | SYS_CTRL_VOC_SEL_FAST_FAST
#endif
#endif
                             | tcuClock
                             ;
#endif // !HAL_SYS_SETUP_CLOCK_26M

#else // !FPGA

    hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_RF
                             | SYS_CTRL_SYS_SEL_FAST_FAST
#ifdef USE_DIGITAL_CLK
#ifndef CHIP_DIE_8955
                             | SYS_CTRL_DIGEN_H_ENABLE
#endif
#endif // USE_DIGITAL_CLK
#ifndef CHIP_HAS_AP
                             | SYS_CTRL_BB_SEL_FAST_FAST
#ifndef CHIP_DIE_8955
                             | SYS_CTRL_MPMC_SYS_SAME_ENABLE
#else
                             | SYS_CTRL_MEM_BRIDGE_SEL_FAST_FAST
                             | SYS_CTRL_SPIFLASH_SEL_FAST_FAST
                             | SYS_CTRL_VOC_SEL_FAST_FAST
#endif
#endif
                             | tcuClock
                             ;

#endif // !FPGA

    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
}



// =============================================================================
// hal_SysGetPLLLock
// -----------------------------------------------------------------------------
/// Return PLL lock
/// @return PLL status
///         If \c TRUE, PLL locked.
///         If \c FALSE, PLL not locked.
// =============================================================================
PROTECTED BOOL hal_SysGetPLLLock(VOID)
{
    if(hwp_sysCtrl->Sel_Clock & SYS_CTRL_PLL_LOCKED)
    {
        return TRUE;
    }
    return FALSE;
}


// =============================================================================
// hal_SysGetFrequencyConfiguration
// -----------------------------------------------------------------------------
/// This function returns the frequency configuration to apply to the system
/// when required to set a given frequency. If the system cannot produce that
/// frequency, the closest available higher frequency will be applied.
/// The real frequency to apply is returned. The second pointer parameter will
/// store the bitfield value to use in the chip register.
///
/// @param fastClock Minimum frequency the system must output.
/// @param sysCtrlRegConfig Value to set in the sysCtrl bitfield to get the
/// configured frequency.
/// @param enableCpuTurbo Pointer to a variable set to TRUE if, and only if,
/// the X-CPU clock doubler needs to be enabled at that frequency. This is
/// enabled only on chip supporting that feature.
/// @return The actual frequency that is going to be applied with that
/// sysCtrl bitfield value.
// =============================================================================
PRIVATE HAL_SYS_FREQ_T hal_SysGetFrequencyConfiguration(HAL_SYS_FREQ_T fastClock,
        UINT32*        sysCtrlRegConfig,
        BOOL*          enableCpuTurbo)
{
    HAL_SYS_FREQ_T realFreq = fastClock;
    *enableCpuTurbo = FALSE;

#if defined(FPGA)

#ifdef FPGA_SLOW
    *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_39M;
    realFreq = HAL_SYS_FREQ_39M;
#else
    // Fixed to 52M for FPGA
#ifdef FPGA_TOP_CLK_312M
    *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_104M;
    realFreq = HAL_SYS_FREQ_104M;
#else
    *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_52M;
    realFreq = HAL_SYS_FREQ_52M;
#endif
#endif

#else // !FPGA

    switch(fastClock)
    {
        case HAL_SYS_FREQ_32K:
            // Not relevant, as we stay on the slow clock.
            // No frequency requested, fall back to 26MHz, 32kHz is only switched
            // in hal_LpsDeepSleep()
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_26M;
            realFreq = HAL_SYS_FREQ_26M;
            break;

        case HAL_SYS_FREQ_13M:
        case HAL_SYS_FREQ_26M:
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_26M;
            realFreq = HAL_SYS_FREQ_26M;
            break;

        case HAL_SYS_FREQ_39M:
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_39M;
            break;

        case HAL_SYS_FREQ_52M:
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_52M;
            break;

        case HAL_SYS_FREQ_78M:
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_78M;
            break;

        case HAL_SYS_FREQ_89M:
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_89M;
            break;

        case HAL_SYS_FREQ_104M:
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_104M;
            break;

        case HAL_SYS_FREQ_113M:
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_113M;
            break;

        case HAL_SYS_FREQ_125M:
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_125M;
            break;

        case HAL_SYS_FREQ_139M:
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_139M;
            break;

        case HAL_SYS_FREQ_156M:
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_156M;
            break;

        case HAL_SYS_FREQ_178M:
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_178M;
            break;

        case HAL_SYS_FREQ_208M:
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_208M;
            break;

        case HAL_SYS_FREQ_250M:
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_250M;
            break;

        case HAL_SYS_FREQ_312M:
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_312M;
            break;

        default:
            *sysCtrlRegConfig = SYS_CTRL_SYS_FREQ_26M;
            realFreq = HAL_SYS_FREQ_26M;
            HAL_TRACE(HAL_WARN_TRC, 0, "HAL SYS: Requested Freq %d unsupported, fall back to %d",
                      fastClock, realFreq);
    }

#endif // !FPGA

    return realFreq;
}



// =============================================================================
// hal_SysSetupSystemClock
// -----------------------------------------------------------------------------
/// Configure the initial settings of the system clocks.
/// This function is to be called only by hal_init.
/// It also checks the validity of the public enum for HAL_SYS_FREQ_T and
/// the internal one HAL_SYS_FREQ_SCAL_USERS_T use for frequency scaling.
/// @param fastClockSetup Initial System Clock.
// =============================================================================
PROTECTED VOID hal_SysSetupSystemClock(UINT32 fastClockSetup)
{
    HAL_SYS_FREQ_T fastClock      = fastClockSetup;
    UINT32         sysFreq        = 2; //52M

#ifdef CHIP_XTAL_CLK_IS_52M
    if (fastClock < HAL_SYS_FREQ_52M)
        fastClock = HAL_SYS_FREQ_52M;
#endif

    // Frequency clamping
    if (fastClock > HAL_SYS_MAX_SPEED)
    {
        HAL_TRACE(HAL_WARN_TRC, 0, "hal_SysSetupSystemClock clamped the request for a %d frequency. %d", fastClock, HAL_SYS_MAX_SPEED);
        fastClock = HAL_SYS_MAX_SPEED;
    }

#ifdef __PRJ_WITH_SDRAM__
    g_Freq4Sdram = fastClock;
#endif

    // Check the validity of the two enums
    // The HAL user Id are encroaching the system ones
    HAL_ASSERT(HAL_SYS_FREQ_SYS_LAST_USER<HAL_SYS_USER_START,
               "Bad value for the HAL_SYS_FREQ_USER_ID_T enum\n  "
               "The first public user,%d, is less than the "
               "last hal private one,%d.",HAL_SYS_USER_START,HAL_SYS_FREQ_SYS_LAST_USER);

    HAL_ASSERT(HAL_SYS_FREQ_PLATFORM_LAST_USER<HAL_SYS_FREQ_APP_FIRST_USER,
               "Bad value for the HAL_SYS_FREQ_USER_ID_T enum\n  "
               "The first application user,%d, is less than the "
               "last platform one,%d.",HAL_SYS_FREQ_APP_FIRST_USER,HAL_SYS_FREQ_PLATFORM_LAST_USER);

    HAL_ASSERT(HAL_SYS_TOTAL_USERS_QTY<=HAL_SYS_FREQ_MAX_RESOURCE_NB,
               "Bad value for the HAL_SYS_FREQ_USER_ID_T enum\n  "
               "The last public user,%d, is greater than the "
               "total available number in hal %d.",HAL_SYS_TOTAL_USERS_QTY,HAL_SYS_FREQ_MAX_RESOURCE_NB);

    // Do not set VCORE here for PMU is initialized after setting up sys clk
    {

        UINT32 status = hal_SysEnterCriticalSection();
        hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;

#ifndef CHIP_DIE_8955
        hwp_sysCtrl->Cfg_Clk_Sys = sysFreq | SYS_CTRL_FORCE_DIV_UPDATE;
#ifndef CHIP_HAS_AP
#if (CHIP_TCU_CLK == 26000000)
        hwp_sysCtrl->Cfg_Clk_BB = SYS_CTRL_BB_FREQ_39M | SYS_CTRL_BB_FORCE_DIV_UPDATE;
#else
        hwp_sysCtrl->Cfg_Clk_BB = SYS_CTRL_BB_FREQ_26M | SYS_CTRL_BB_FORCE_DIV_UPDATE;
#endif
#endif
#ifdef __PRJ_WITH_SDRAM__
        boot_SysSetSdramTiming(g_Freq4Sdram);
        boot_SysSetMPMCTiming(g_Freq4Sdram);
#endif
#else
        hwp_sysCtrl->Cfg_Clk_Sys = sysFreq;
#endif
        hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
        hal_SysExitCriticalSection(status);
    }

    // setup gsm clocks: (TCU clock)
    hwp_tcu->Cfg_Clk_Div= TCU_ENABLE_QBIT_ENABLED;

    // update global variables
    g_halSysSystemFreq = fastClock;
}

#ifdef CHIP_DIE_8955
PUBLIC VOID HAL_BOOT_FUNC_INTERNAL hal_SysSetSpiFlashClkToFast(VOID)
{
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Sel_Clock &= ~SYS_CTRL_SPIFLASH_SEL_FAST;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
}
#endif

#ifndef CHIP_HAS_AP
// =============================================================================
// hal_SysSetBBClock
// -----------------------------------------------------------------------------
/// Set the BB clock.
/// @param newClock New BB clock.
// =============================================================================
PUBLIC VOID hal_SysSetBBClock(HAL_SYS_FREQ_T newClock)
{
    UINT32 newClkField;
    UINT32 oldClkField;

#if defined(FPGA)
#ifdef FPGA_SLOW
    newClock = HAL_SYS_FREQ_39M;
#else
    // Fixed to 52M for FPGA
#ifdef FPGA_TOP_CLK_312M
    newClock = HAL_SYS_FREQ_104M;
#else
    newClock = HAL_SYS_FREQ_52M;
#endif
#endif
#endif

    // Default to the lowest safe freq
#if (CHIP_TCU_CLK == 26000000)
    newClkField = SYS_CTRL_BB_FREQ_39M;
    if (newClock < HAL_SYS_FREQ_39M)
    {
        newClock = HAL_SYS_FREQ_39M;
    }
#else
    newClkField = SYS_CTRL_BB_FREQ_26M;
#endif

    switch(newClock)
    {
        case HAL_SYS_FREQ_32K  : newClkField = SYS_CTRL_BB_FREQ_26M; break;
        case HAL_SYS_FREQ_26M  : newClkField = SYS_CTRL_BB_FREQ_26M; break;
        case HAL_SYS_FREQ_39M  : newClkField = SYS_CTRL_BB_FREQ_39M; break;
        case HAL_SYS_FREQ_52M  : newClkField = SYS_CTRL_BB_FREQ_52M; break;
        case HAL_SYS_FREQ_78M  : newClkField = SYS_CTRL_BB_FREQ_78M; break;
        case HAL_SYS_FREQ_104M : newClkField = SYS_CTRL_BB_FREQ_104M; break;
        case HAL_SYS_FREQ_156M : newClkField = SYS_CTRL_BB_FREQ_156M; break;
        case HAL_SYS_FREQ_208M : newClkField = SYS_CTRL_BB_FREQ_208M; break;
        case HAL_SYS_FREQ_312M : newClkField = SYS_CTRL_BB_FREQ_312M; break;
#if defined(CHIP_DIE_8955)
        case HAL_SYS_FREQ_89M  : newClkField = SYS_CTRL_BB_FREQ_89M; break;
        case HAL_SYS_FREQ_113M : newClkField = SYS_CTRL_BB_FREQ_113M; break;
        case HAL_SYS_FREQ_125M : newClkField = SYS_CTRL_BB_FREQ_125M; break;
        case HAL_SYS_FREQ_139M : newClkField = SYS_CTRL_BB_FREQ_139M; break;
        case HAL_SYS_FREQ_178M : newClkField = SYS_CTRL_BB_FREQ_178M; break;
        case HAL_SYS_FREQ_250M : newClkField = SYS_CTRL_BB_FREQ_250M; break;
#else
        case HAL_SYS_FREQ_89M  : newClkField = SYS_CTRL_BB_FREQ_78M; break;
        case HAL_SYS_FREQ_113M : newClkField = SYS_CTRL_BB_FREQ_104M; break;
        case HAL_SYS_FREQ_125M : newClkField = SYS_CTRL_BB_FREQ_104M; break;
        case HAL_SYS_FREQ_139M : newClkField = SYS_CTRL_BB_FREQ_104M; break;
        case HAL_SYS_FREQ_178M : newClkField = SYS_CTRL_BB_FREQ_156M; break;
        case HAL_SYS_FREQ_250M : newClkField = SYS_CTRL_BB_FREQ_208M; break;
#endif
        default                : break;
    }

    oldClkField = hwp_sysCtrl->Cfg_Clk_BB;
    if (oldClkField == newClkField)
        return;

    hwp_sysCtrl->Cfg_Clk_BB = newClkField;

#ifdef HAL_CLK_PROFILING
    switch (oldClkField)
    {
        case SYS_CTRL_BB_FREQ_26M  : HAL_PROFILE_WINDOW_EXIT(BB_CLK_IS_26M); break;
        case SYS_CTRL_BB_FREQ_39M  : HAL_PROFILE_WINDOW_EXIT(BB_CLK_IS_39M); break;
        case SYS_CTRL_BB_FREQ_52M  : HAL_PROFILE_WINDOW_EXIT(BB_CLK_IS_52M); break;
        case SYS_CTRL_BB_FREQ_78M  : HAL_PROFILE_WINDOW_EXIT(BB_CLK_IS_78M); break;
        case SYS_CTRL_BB_FREQ_104M : HAL_PROFILE_WINDOW_EXIT(BB_CLK_IS_104M); break;
        case SYS_CTRL_BB_FREQ_156M : HAL_PROFILE_WINDOW_EXIT(BB_CLK_IS_156M); break;
        case SYS_CTRL_BB_FREQ_208M : HAL_PROFILE_WINDOW_EXIT(BB_CLK_IS_208M); break;
        case SYS_CTRL_BB_FREQ_312M : HAL_PROFILE_WINDOW_EXIT(BB_CLK_IS_312M); break;
#if defined(CHIP_DIE_8955)
        case SYS_CTRL_BB_FREQ_89M  : HAL_PROFILE_WINDOW_EXIT(BB_CLK_IS_89M); break;
        case SYS_CTRL_BB_FREQ_113M : HAL_PROFILE_WINDOW_EXIT(BB_CLK_IS_113M); break;
        case SYS_CTRL_BB_FREQ_125M : HAL_PROFILE_WINDOW_EXIT(BB_CLK_IS_125M); break;
        case SYS_CTRL_BB_FREQ_139M : HAL_PROFILE_WINDOW_EXIT(BB_CLK_IS_139M); break;
        case SYS_CTRL_BB_FREQ_178M : HAL_PROFILE_WINDOW_EXIT(BB_CLK_IS_178M); break;
        case SYS_CTRL_BB_FREQ_250M : HAL_PROFILE_WINDOW_EXIT(BB_CLK_IS_250M); break;
#endif // CHIP_DIE_8955
        default :                    HAL_PROFILE_WINDOW_EXIT(BB_CLK_IS_OTHERS); break;
    }
    switch (newClkField)
    {
        case SYS_CTRL_BB_FREQ_26M  : HAL_PROFILE_WINDOW_ENTER(BB_CLK_IS_26M); break;
        case SYS_CTRL_BB_FREQ_39M  : HAL_PROFILE_WINDOW_ENTER(BB_CLK_IS_39M); break;
        case SYS_CTRL_BB_FREQ_52M  : HAL_PROFILE_WINDOW_ENTER(BB_CLK_IS_52M); break;
        case SYS_CTRL_BB_FREQ_78M  : HAL_PROFILE_WINDOW_ENTER(BB_CLK_IS_78M); break;
        case SYS_CTRL_BB_FREQ_104M : HAL_PROFILE_WINDOW_ENTER(BB_CLK_IS_104M); break;
        case SYS_CTRL_BB_FREQ_156M : HAL_PROFILE_WINDOW_ENTER(BB_CLK_IS_156M); break;
        case SYS_CTRL_BB_FREQ_208M : HAL_PROFILE_WINDOW_ENTER(BB_CLK_IS_208M); break;
        case SYS_CTRL_BB_FREQ_312M : HAL_PROFILE_WINDOW_ENTER(BB_CLK_IS_312M); break;
#if defined(CHIP_DIE_8955)
        case SYS_CTRL_BB_FREQ_89M  : HAL_PROFILE_WINDOW_ENTER(BB_CLK_IS_89M); break;
        case SYS_CTRL_BB_FREQ_113M : HAL_PROFILE_WINDOW_ENTER(BB_CLK_IS_113M); break;
        case SYS_CTRL_BB_FREQ_125M : HAL_PROFILE_WINDOW_ENTER(BB_CLK_IS_125M); break;
        case SYS_CTRL_BB_FREQ_139M : HAL_PROFILE_WINDOW_ENTER(BB_CLK_IS_139M); break;
        case SYS_CTRL_BB_FREQ_178M : HAL_PROFILE_WINDOW_ENTER(BB_CLK_IS_178M); break;
        case SYS_CTRL_BB_FREQ_250M : HAL_PROFILE_WINDOW_ENTER(BB_CLK_IS_250M); break;
#endif // CHIP_DIE_8955
        default :                    HAL_PROFILE_WINDOW_ENTER(BB_CLK_IS_OTHERS); break;
    }
#endif // HAL_CLK_PROFILING
}


// =============================================================================
// hal_SysSetVocClock
// -----------------------------------------------------------------------------
/// Set the VOC clock.
/// @param newClock New VOC clock.
/// @return Old VOC clock.
// =============================================================================
PUBLIC HAL_SYS_VOC_FREQ_T hal_SysSetVocClock(HAL_SYS_VOC_FREQ_T newClock)
{
    UINT32 oldClkField;
    UINT32 newClkField;
    HAL_SYS_VOC_FREQ_T oldClock;

    UINT32 status = hal_SysEnterCriticalSection();

    newClkField = SYS_CTRL_VOC_FREQ_26M;
    oldClock = SYS_CTRL_VOC_FREQ_26M;

    oldClkField = hwp_sysCtrl->Cfg_Clk_Voc & SYS_CTRL_VOC_FREQ_MASK;

    switch(oldClkField)
    {
        case SYS_CTRL_VOC_FREQ_26M  : oldClock = HAL_SYS_VOC_FREQ_26M; break;
        case SYS_CTRL_VOC_FREQ_39M  : oldClock = HAL_SYS_VOC_FREQ_39M; break;
        case SYS_CTRL_VOC_FREQ_52M  : oldClock = HAL_SYS_VOC_FREQ_52M; break;
        case SYS_CTRL_VOC_FREQ_78M  : oldClock = HAL_SYS_VOC_FREQ_78M; break;
        case SYS_CTRL_VOC_FREQ_89M  : oldClock = HAL_SYS_VOC_FREQ_89M; break;
        case SYS_CTRL_VOC_FREQ_104M : oldClock = HAL_SYS_VOC_FREQ_104M; break;
        case SYS_CTRL_VOC_FREQ_113M : oldClock = HAL_SYS_VOC_FREQ_113M; break;
        case SYS_CTRL_VOC_FREQ_125M : oldClock = HAL_SYS_VOC_FREQ_125M; break;
        case SYS_CTRL_VOC_FREQ_139M : oldClock = HAL_SYS_VOC_FREQ_139M; break;
        case SYS_CTRL_VOC_FREQ_156M : oldClock = HAL_SYS_VOC_FREQ_156M; break;
        case SYS_CTRL_VOC_FREQ_178M : oldClock = HAL_SYS_VOC_FREQ_178M; break;
        case SYS_CTRL_VOC_FREQ_208M : oldClock = HAL_SYS_VOC_FREQ_208M; break;
        case SYS_CTRL_VOC_FREQ_250M : oldClock = HAL_SYS_VOC_FREQ_250M; break;
        case SYS_CTRL_VOC_FREQ_312M : oldClock = HAL_SYS_VOC_FREQ_312M; break;
        default                     : break;
    }

    switch(newClock)
    {
        case HAL_SYS_VOC_FREQ_26M  : newClkField = SYS_CTRL_VOC_FREQ_26M; break;
        case HAL_SYS_VOC_FREQ_39M  : newClkField = SYS_CTRL_VOC_FREQ_39M; break;
        case HAL_SYS_VOC_FREQ_52M  : newClkField = SYS_CTRL_VOC_FREQ_52M; break;
        case HAL_SYS_VOC_FREQ_78M  : newClkField = SYS_CTRL_VOC_FREQ_78M; break;
        case HAL_SYS_VOC_FREQ_89M  : newClkField = SYS_CTRL_VOC_FREQ_89M; break;
        case HAL_SYS_VOC_FREQ_104M : newClkField = SYS_CTRL_VOC_FREQ_104M; break;
        case HAL_SYS_VOC_FREQ_113M : newClkField = SYS_CTRL_VOC_FREQ_113M; break;
        case HAL_SYS_VOC_FREQ_125M : newClkField = SYS_CTRL_VOC_FREQ_125M; break;
        case HAL_SYS_VOC_FREQ_139M : newClkField = SYS_CTRL_VOC_FREQ_139M; break;
        case HAL_SYS_VOC_FREQ_156M : newClkField = SYS_CTRL_VOC_FREQ_156M; break;
        case HAL_SYS_VOC_FREQ_178M : newClkField = SYS_CTRL_VOC_FREQ_178M; break;
        case HAL_SYS_VOC_FREQ_208M : newClkField = SYS_CTRL_VOC_FREQ_208M; break;
        case HAL_SYS_VOC_FREQ_250M : newClkField = SYS_CTRL_VOC_FREQ_250M; break;
        case HAL_SYS_VOC_FREQ_312M : newClkField = SYS_CTRL_VOC_FREQ_312M; break;
        default                    : break;
    }
#ifdef CHIP_DIE_8809E2
    if (oldClock <= HAL_SYS_VOC_FREQ_78M && newClock > HAL_SYS_VOC_FREQ_78M)
    {
        pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_VOC, PMD_CORE_VOLTAGE_MEDIUM);
    }

    if (oldClock <= HAL_SYS_VOC_FREQ_104M && newClock > HAL_SYS_VOC_FREQ_104M)
    {
        pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_VOC, PMD_CORE_VOLTAGE_HIGH);
    }
#endif

#ifdef CHIP_DIE_8955
    hwp_sysCtrl->Cfg_Clk_Voc = newClkField;
#else
    hwp_sysCtrl->Cfg_Clk_Voc = SYS_CTRL_VOC_SEL_PLL_PLL | newClkField;
#endif

#ifdef CHIP_DIE_8809E2
    if (oldClock > HAL_SYS_VOC_FREQ_78M && newClock <=  HAL_SYS_VOC_FREQ_78M)
    {
        pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_VOC, PMD_CORE_VOLTAGE_LOW);
    }
#endif

#ifdef HAL_CLK_PROFILING
    switch(oldClock)
    {
        case HAL_SYS_VOC_FREQ_26M  : HAL_PROFILE_WINDOW_EXIT(VOC_CLK_IS_26M); break;
        case HAL_SYS_VOC_FREQ_39M  : HAL_PROFILE_WINDOW_EXIT(VOC_CLK_IS_39M); break;
        case HAL_SYS_VOC_FREQ_52M  : HAL_PROFILE_WINDOW_EXIT(VOC_CLK_IS_52M); break;
        case HAL_SYS_VOC_FREQ_78M  : HAL_PROFILE_WINDOW_EXIT(VOC_CLK_IS_78M); break;
        case HAL_SYS_VOC_FREQ_89M  : HAL_PROFILE_WINDOW_EXIT(VOC_CLK_IS_89M); break;
        case HAL_SYS_VOC_FREQ_104M : HAL_PROFILE_WINDOW_EXIT(VOC_CLK_IS_104M); break;
        case HAL_SYS_VOC_FREQ_113M : HAL_PROFILE_WINDOW_EXIT(VOC_CLK_IS_113M); break;
        case HAL_SYS_VOC_FREQ_125M : HAL_PROFILE_WINDOW_EXIT(VOC_CLK_IS_125M); break;
        case HAL_SYS_VOC_FREQ_139M : HAL_PROFILE_WINDOW_EXIT(VOC_CLK_IS_139M); break;
        case HAL_SYS_VOC_FREQ_156M : HAL_PROFILE_WINDOW_EXIT(VOC_CLK_IS_156M); break;
        case HAL_SYS_VOC_FREQ_178M : HAL_PROFILE_WINDOW_EXIT(VOC_CLK_IS_178M); break;
        case HAL_SYS_VOC_FREQ_208M : HAL_PROFILE_WINDOW_EXIT(VOC_CLK_IS_208M); break;
        case HAL_SYS_VOC_FREQ_250M : HAL_PROFILE_WINDOW_EXIT(VOC_CLK_IS_250M); break;
        case HAL_SYS_VOC_FREQ_312M : HAL_PROFILE_WINDOW_EXIT(VOC_CLK_IS_312M); break;
        default :                    HAL_PROFILE_WINDOW_EXIT(VOC_CLK_IS_OTHERS); break;
    }

    switch(newClock)
    {
        case HAL_SYS_VOC_FREQ_26M  : HAL_PROFILE_WINDOW_ENTER(VOC_CLK_IS_26M); break;
        case HAL_SYS_VOC_FREQ_39M  : HAL_PROFILE_WINDOW_ENTER(VOC_CLK_IS_39M); break;
        case HAL_SYS_VOC_FREQ_52M  : HAL_PROFILE_WINDOW_ENTER(VOC_CLK_IS_52M); break;
        case HAL_SYS_VOC_FREQ_78M  : HAL_PROFILE_WINDOW_ENTER(VOC_CLK_IS_78M); break;
        case HAL_SYS_VOC_FREQ_89M  : HAL_PROFILE_WINDOW_ENTER(VOC_CLK_IS_89M); break;
        case HAL_SYS_VOC_FREQ_104M : HAL_PROFILE_WINDOW_ENTER(VOC_CLK_IS_104M); break;
        case HAL_SYS_VOC_FREQ_113M : HAL_PROFILE_WINDOW_ENTER(VOC_CLK_IS_113M); break;
        case HAL_SYS_VOC_FREQ_125M : HAL_PROFILE_WINDOW_ENTER(VOC_CLK_IS_125M); break;
        case HAL_SYS_VOC_FREQ_139M : HAL_PROFILE_WINDOW_ENTER(VOC_CLK_IS_139M); break;
        case HAL_SYS_VOC_FREQ_156M : HAL_PROFILE_WINDOW_ENTER(VOC_CLK_IS_156M); break;
        case HAL_SYS_VOC_FREQ_178M : HAL_PROFILE_WINDOW_ENTER(VOC_CLK_IS_178M); break;
        case HAL_SYS_VOC_FREQ_208M : HAL_PROFILE_WINDOW_ENTER(VOC_CLK_IS_208M); break;
        case HAL_SYS_VOC_FREQ_250M : HAL_PROFILE_WINDOW_ENTER(VOC_CLK_IS_250M); break;
        case HAL_SYS_VOC_FREQ_312M : HAL_PROFILE_WINDOW_ENTER(VOC_CLK_IS_312M); break;
        default :                    HAL_PROFILE_WINDOW_ENTER(VOC_CLK_IS_OTHERS); break;
    }
#endif // CHIP_DIE_8955

    hal_SysExitCriticalSection(status);
    return oldClock;
}
#endif // !CHIP_HAS_AP

// =============================================================================
//  hal_SysGetVocClock
// -----------------------------------------------------------------------------
/// Get the VOC clock.
/// @return Old VOC clock.
// =============================================================================

PUBLIC HAL_SYS_VOC_FREQ_T hal_SysGetVocClock()
{
    UINT32 oldClkField;
    UINT32 newClkField;
    HAL_SYS_VOC_FREQ_T oldClock;

    UINT32 status = hal_SysEnterCriticalSection();

    oldClock = SYS_CTRL_VOC_FREQ_26M;

    oldClkField = hwp_sysCtrl->Cfg_Clk_Voc & SYS_CTRL_VOC_FREQ_MASK;

    switch(oldClkField)
    {
        case SYS_CTRL_VOC_FREQ_26M  : oldClock = HAL_SYS_VOC_FREQ_26M; break;
        case SYS_CTRL_VOC_FREQ_39M  : oldClock = HAL_SYS_VOC_FREQ_39M; break;
        case SYS_CTRL_VOC_FREQ_52M  : oldClock = HAL_SYS_VOC_FREQ_52M; break;
        case SYS_CTRL_VOC_FREQ_78M  : oldClock = HAL_SYS_VOC_FREQ_78M; break;
        case SYS_CTRL_VOC_FREQ_89M  : oldClock = HAL_SYS_VOC_FREQ_89M; break;
        case SYS_CTRL_VOC_FREQ_104M : oldClock = HAL_SYS_VOC_FREQ_104M; break;
        case SYS_CTRL_VOC_FREQ_113M : oldClock = HAL_SYS_VOC_FREQ_113M; break;
        case SYS_CTRL_VOC_FREQ_125M : oldClock = HAL_SYS_VOC_FREQ_125M; break;
        case SYS_CTRL_VOC_FREQ_139M : oldClock = HAL_SYS_VOC_FREQ_139M; break;
        case SYS_CTRL_VOC_FREQ_156M : oldClock = HAL_SYS_VOC_FREQ_156M; break;
        case SYS_CTRL_VOC_FREQ_178M : oldClock = HAL_SYS_VOC_FREQ_178M; break;
        case SYS_CTRL_VOC_FREQ_208M : oldClock = HAL_SYS_VOC_FREQ_208M; break;
        case SYS_CTRL_VOC_FREQ_250M : oldClock = HAL_SYS_VOC_FREQ_250M; break;
        case SYS_CTRL_VOC_FREQ_312M : oldClock = HAL_SYS_VOC_FREQ_312M; break;
        default                     : break;
    }

    hal_SysExitCriticalSection(status);
    return oldClock;
}

// =============================================================================
// hal_SysResetOut
// -----------------------------------------------------------------------------
/// A call to this function resets an external device through the reset-out
/// line, if the parameter is \c TRUE.
/// @param resetOut If \c TRUE, rises the reset-out line.
///                 If \c FALSE, pulls it down.
// =============================================================================
PUBLIC VOID hal_SysResetOut(BOOL resetOut)
{
    if (resetOut == FALSE)
    {
        hwp_sysCtrl->Sys_Rst_Clr = SYS_CTRL_CLR_RST_OUT;
    }
    else
    {
        UINT32 status = hal_SysEnterCriticalSection();
        hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
        hwp_sysCtrl->Sys_Rst_Set = SYS_CTRL_SET_RST_OUT;
        hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
        hal_SysExitCriticalSection(status);
    }
}



// =============================================================================
// hal_SysInvalidateCache
// -----------------------------------------------------------------------------
/// This function invalidates in the cache the line corresponding to the given
/// memory area, referred by a pointer and a size.
/// It must be called with the destination buffer after a DMA copy.
/// @param buffer Pointer to the first address whose cache line must be
/// invalidated
/// @param size Size of the buffer.
// =============================================================================
PUBLIC VOID hal_SysInvalidateCache(VOID* buffer, UINT32 size)
{
    if (size > 512)
    {
        // flush cache
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
#else
        hal_GdbFlushCacheData();
        return;
#endif
    }

    // do a memcpy from uncached to cached to insure coherency of cache
    memcpy( buffer, // destination
            (VOID*) HAL_SYS_GET_UNCACHED_ADDR((UINT32)buffer), // source
            size); // size

}



// =============================================================================
// hal_SysGetChipId
// -----------------------------------------------------------------------------
/// That function gives the chip ID the software is running on.
/// @param part Which part of the chip ID
/// @return The requested part of the chip ID.
// =============================================================================
PUBLIC UINT32 hal_SysGetChipId(HAL_SYS_CHIP_ID_T part)
{
    return boot_SysGetChipId(part);
}


// =============================================================================
// hal_SysGetMetalId
// -----------------------------------------------------------------------------
/// That function gives the chip metal ID the software is running on.
/// @return The chip metal ID enum, which is not the bitfield in chip register.
// =============================================================================
PUBLIC HAL_SYS_CHIP_METAL_ID_T hal_SysGetMetalId(VOID)
{
    return boot_SysGetMetalId();
}


// =============================================================================
// hal_SysGetThermal_cal
// -----------------------------------------------------------------------------
/// That function gives the  thermal_cal_value.
/// @return  thermal_cal_value.
// =============================================================================
PUBLIC  UINT16 hal_SysGetThermal_cal(VOID)
{
 #ifdef CHIP_DIE_8955  
    UINT16 thermal_cal_value=0;
    UINT16 thermal_cal=hwp_configRegs->thermal_cal;
    UINT16 thermal_cal_valid =((thermal_cal>>6) & 0x1);
    if(thermal_cal_valid)
    {
	    thermal_cal_value=(thermal_cal & 0x3f);
    }
    else
    {
           thermal_cal_value=0xff;
    }
    return   thermal_cal_value;
 #endif
}


// =============================================================================
// hal_SysClkOutInit
// -----------------------------------------------------------------------------
/// Initialize the clock-out to OFF state
// =============================================================================
PROTECTED VOID hal_SysClkInit(VOID)
{
    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;

    // By default, turn the CLK_OUT   *** OFF ***
    // The clock can be reconfigured, i.e. set to 32k or 26M /
    // enabled or disabled later.
#ifndef CHIP_DIE_8955
    hwp_sysCtrl->Clk_Other_Disable = SYS_CTRL_DISABLE_OC_CLK_OUT;
#endif
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SysExitCriticalSection(status);
}


// =============================================================================
// hal_SysClkOut
// -----------------------------------------------------------------------------
/// Manage the output of the clock-out, to an external device
///
/// This function requests a resource of #HAL_SYS_FREQ_26M or release it
/// to #HAL_SYS_FREQ_32K depending on the \p freq parameter value.
///
/// @param freq the required frequency or #HAL_SYS_CLOCK_OUT_OFF
// =============================================================================
PRIVATE VOID hal_SysClkOut(HAL_SYS_CLOCK_OUT_FREQ_T freq)
{
    HAL_CLK_T *clkOut = hal_ClkGet(FOURCC_CLKOUT);

    HAL_ASSERT((clkOut != NULL), "CLK OUT not found!");

    if (freq == HAL_SYS_CLOCK_OUT_FREQ_26M)
    {
        // Need the RF clock
        if (!hal_ClkIsEnabled(clkOut))
            hal_ClkEnable(clkOut);
    }

    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;

    // TODO Implement the divider
    // management.
    if (freq == HAL_SYS_CLOCK_OUT_FREQ_26M)
    {
#ifndef CHIP_DIE_8955
        hwp_sysCtrl->Cfg_Clk_Out = SYS_CTRL_CLKOUT_SEL_RF;
#endif
    }
    else
    {
#ifndef CHIP_DIE_8955
        hwp_sysCtrl->Cfg_Clk_Out = SYS_CTRL_CLKOUT_SEL_OSC;
#else
        hwp_sysCtrl->Cfg_Clk_Out = SYS_CTRL_CLKOUT_ENABLE_ENABLE;
#endif
    }

    if (freq != HAL_SYS_CLOCK_OUT_OFF)
    {
#ifndef CHIP_DIE_8955
        hwp_sysCtrl->Clk_Other_Enable = SYS_CTRL_ENABLE_OC_CLK_OUT;
#endif
    }
    else
    {
#ifndef CHIP_DIE_8955
        hwp_sysCtrl->Clk_Other_Disable = SYS_CTRL_DISABLE_OC_CLK_OUT;
#endif
    }
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SysExitCriticalSection(status);

    if (freq != HAL_SYS_CLOCK_OUT_FREQ_26M)
    {
        // Free the resource
        hal_ClkDisable(clkOut);
    }
}


// =============================================================================
// hal_SysClkOutOpen
// -----------------------------------------------------------------------------
/// Manage the output of the clock-out, to an external device
///
/// This function requests a resource of #HAL_SYS_FREQ_26M or not
/// (keep #HAL_SYS_FREQ_32K) depending on the \p freq parameter value.
///
/// @param freq the required frequency
/// @return the user id to be used for #hal_SysClkOutClose
///         or #HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE when the clock is already
///         in use at a different frequency.
// =============================================================================
PUBLIC HAL_SYS_CLOCK_OUT_ID_T hal_SysClkOutOpen(HAL_SYS_CLOCK_OUT_FREQ_T freq)
{
    INT32 id = HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE;
    INT32 loopId;
    UINT8 curFreq=HAL_SYS_CLOCK_OUT_OFF;
    UINT32 status;

    HAL_ASSERT(((freq <= HAL_SYS_CLOCK_OUT_FREQ_26M)
                && ( freq >= HAL_SYS_CLOCK_OUT_FREQ_32K))
               , "Clock Out Frequency out of range: %d", freq);

    status = hal_SysEnterCriticalSection();

    for(loopId = 0; loopId < HAL_SYS_CLOCK_OUT_ID_QTY; loopId ++)
    {
        // empty spot?
        if (HAL_SYS_CLOCK_OUT_OFF == g_halSysClockOutRegistry[loopId])
        {
            id = loopId;
        }
        else
        {
            curFreq = g_halSysClockOutRegistry[loopId];
        }
    }
    // no more spot
    if (HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE == id)
    {
        hal_SysExitCriticalSection(status);
        return HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE;
    }
    // no freqency yet or same freq, ok open it !
    if ((HAL_SYS_CLOCK_OUT_OFF == curFreq) || (freq == curFreq))
    {
        g_halSysClockOutRegistry[id] = freq;
        if (HAL_SYS_CLOCK_OUT_OFF == curFreq)
        {
            hal_SysClkOut(freq);
        }
        hal_SysExitCriticalSection(status);
        return id;
    }
    // else an other frequency is already set
    hal_SysExitCriticalSection(status);
    return HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE;
}



// =============================================================================
// hal_SysClkOutClose
// -----------------------------------------------------------------------------
/// Manage the output of the clock-out, to an external device
///
/// This function release a resource to #HAL_SYS_FREQ_32K if no more
/// Clock Out user needs it.
///
/// @param id the user id returned by #hal_SysClkOutOpen
// =============================================================================
PUBLIC VOID hal_SysClkOutClose(HAL_SYS_CLOCK_OUT_ID_T id)
{
    INT32 loopId;
    UINT8 curFreq=HAL_SYS_CLOCK_OUT_OFF;
    UINT32 status;
    HAL_ASSERT(((id < HAL_SYS_CLOCK_OUT_ID_QTY)
                && (id >= HAL_SYS_CLOCK_OUT_ID_1))
               , "Clock Out Id out of range: %d", id);
    HAL_ASSERT(g_halSysClockOutRegistry[id] != HAL_SYS_CLOCK_OUT_OFF, "Invalid clock out Id (not opened): %d", id);
    status = hal_SysEnterCriticalSection();
    // clear request
    g_halSysClockOutRegistry[id] = HAL_SYS_CLOCK_OUT_OFF;
    for(loopId = 0; loopId < HAL_SYS_CLOCK_OUT_ID_QTY; loopId ++)
    {
        // freq request?
        if (HAL_SYS_CLOCK_OUT_OFF != g_halSysClockOutRegistry[loopId])
        {
            curFreq = g_halSysClockOutRegistry[loopId];
            // one found, stop all must match or be off
            hal_SysExitCriticalSection(status);
            return ;
        }
    }
    // No clock out needed.
    hal_SysClkOut(HAL_SYS_CLOCK_OUT_OFF);
    hal_SysExitCriticalSection(status);
}



// ==========================================================================
// hal_SysAuxClkOut
// --------------------------------------------------------------------------
/// Manage the output of the 26M AUX Clock, to an external device
/// @param enable If \c TRUE, enable AuxClk output.
///               If \c FALSE, disable AuxClk output.
// ==========================================================================
PUBLIC VOID hal_SysAuxClkOut(BOOL enable)
{
    UINT32 status = hal_SysEnterCriticalSection();
    if (enable)
    {
        if (g_halSysAuxClkUserNum == 0)
        {
            hwp_sysCtrl->Cfg_Clk_Auxclk = SYS_CTRL_AUXCLK_EN_ENABLE;
        }
        g_halSysAuxClkUserNum++;
    }
    else
    {
        if (g_halSysAuxClkUserNum > 0)
        {
            g_halSysAuxClkUserNum--;
            if (g_halSysAuxClkUserNum == 0)
            {
                hwp_sysCtrl->Cfg_Clk_Auxclk = SYS_CTRL_AUXCLK_EN_DISABLE;
            }
        }
    }
    hal_SysExitCriticalSection(status);
}


// ==========================================================================
// hal_Sys32kClkOut
// --------------------------------------------------------------------------
/// Manage the output of the 32K Clock, to an external device
/// @param enable If \c TRUE, enable 32K clock output.
///               If \c FALSE, disable 32K clock output.
// ==========================================================================
PUBLIC VOID hal_Sys32kClkOut(BOOL enable)
{
    UINT32 status = hal_SysEnterCriticalSection();
    if (enable)
    {
        if (g_halSys32kClkUserNum == 0)
        {
            hal_BoardConfigClk32k(enable);
        }
        g_halSys32kClkUserNum++;
    }
    else
    {
        if (g_halSys32kClkUserNum > 0)
        {
            g_halSys32kClkUserNum--;
            if (g_halSys32kClkUserNum == 0)
            {
                hal_BoardConfigClk32k(enable);
            }
        }
    }
    hal_SysExitCriticalSection(status);
}



// =============================================================================
// hal_SysIrqPending
// -----------------------------------------------------------------------------
/// This function checks the IRQ pending status. It should be called in critical
/// section to know if some IRQ are pending and then decide to release the
/// critical section.
/// @return \c TRUE if IRQ are pending.
///         \c FALSE Otherwise.
// =============================================================================
PUBLIC BOOL hal_SysIrqPending(VOID)
{
    if (hwp_sysIrq->Cause != 0)
    {
        // IRQ are pending
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



// =============================================================================
// hal_SysGetResetCause
// -----------------------------------------------------------------------------
/// Get the cause of the last reset.
/// This function only returns the reset cause. It does not proceed to any test
/// to now if the boot was due to a press on the Power On button or because
/// the charger was plugged. Those tests are to be done by the user code,
/// when this function returns HAL_SYS_RESET_CAUSE_ALARM.
/// @return The last reset cause
// =============================================================================
PUBLIC HAL_SYS_RESET_CAUSE_T hal_SysGetResetCause(VOID)
{
    // Check and clear the shutdown flag
    if (g_halSysStateShutdownFlag == SYS_SHUTDOWN_MAGIC_NUM)
    {
        g_halSysBootFromShutdown = TRUE;
    }
    g_halSysStateShutdownFlag = 0;

    // NOTE:
    // The checking order should comply with the priority of reset causes.
    // In case of multiple reset causes in place simultaneously, only the
    // reset cause with highest priority is returned.
    // E.g.,
    // If an alarm is triggered during the shutdown process, the system will
    // reboot after the shutdown process, for the wakeup signal is kept high
    // by the RTC alarm. After the system boots up, ALARM and RESTART causes
    // will be valid, but only ALARM should be returned.

    if (g_bootResetCause & SYS_CTRL_WATCHDOG_RESET_HAPPENED)
    {
        return HAL_SYS_RESET_CAUSE_WATCHDOG;
    }

    if (g_bootResetCause & SYS_CTRL_HOSTDEBUG_RESET_HAPPENED)
    {
        return HAL_SYS_RESET_CAUSE_HOST_DEBUG;
    }

    if (g_bootResetCause & SYS_CTRL_ALARMCAUSE_HAPPENED)
    {
        return HAL_SYS_RESET_CAUSE_ALARM;
    }

    // Global soft reset has the second-lowest priority
    if (g_bootResetCause & SYS_CTRL_GLOBALSOFT_RESET_HAPPENED)
    {
        // If the system is restarted, check whether it just went through a
        // shutdown process. If yes, there should be other reset causes
        // (e.g., charger or power-on key), and this should be tagged as
        // a normal boot.
        if (g_halSysBootFromShutdown)
        {
            return HAL_SYS_RESET_CAUSE_NORMAL;
        }
        else
        {
            return HAL_SYS_RESET_CAUSE_RESTART;
        }
    }

    // Normal has the lowest priority
    return HAL_SYS_RESET_CAUSE_NORMAL;
}



// =============================================================================
// hal_SysUsbHostEnable
// -----------------------------------------------------------------------------
/// Enable or disable usb host.
/// This is useful to inform hal_SysProcessHostMonitor() that the USB Host functions
/// must be called to process the host commands.
/// @param enable \c TRUE to enable, \c FALSE to disable.
// =============================================================================
PROTECTED VOID hal_SysUsbHostEnable(BOOL enable)
{
#if (CHIP_HAS_USB == 1)
    g_halUsbHostEnable = enable;
#else
    enable             = enable;
#endif/* CHIP_HAS_USB */
}



#ifdef CHIP_HAS_AP
// =============================================================================
// hal_SysApHostEnable
// -----------------------------------------------------------------------------
/// Enable or disable usb host.
/// This is useful to inform hal_SysProcessHostMonitor() that the AP Host functions
/// must be called to process the host commands.
/// @param enable \c TRUE to enable, \c FALSE to disable.
// =============================================================================
PROTECTED VOID hal_SysApHostEnable(BOOL enable)
{
    g_halApHostEnable = enable;
}


// =============================================================================
// hal_SysEnableApClock
// -----------------------------------------------------------------------------
/// Enable or disable AP clock.
/// @param enable \c TRUE to enable, \c FALSE to disable.
// =============================================================================
PROTECTED VOID hal_SysEnableApClock(BOOL enable)
{
    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    if (enable)
    {
        hwp_sysCtrl->Sys_Rst_Clr = SYS_CTRL_CLR_RST_AP_CLKEN;
    }
    else
    {
        hwp_sysCtrl->Sys_Rst_Set = SYS_CTRL_SET_RST_AP_CLKEN;
    }
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SysExitCriticalSection(status);
}


// =============================================================================
// hal_SysEnableApIrq
// -----------------------------------------------------------------------------
/// Enable or disable AP irq.
/// @param enable \c TRUE to enable, \c FALSE to disable.
// =============================================================================
PROTECTED VOID hal_SysEnableApIrq(BOOL enable)
{
    UINT32 status = hal_SysEnterCriticalSection();
    if (enable)
    {
        hwp_sysCtrl->AP_Ctrl |= SYS_CTRL_AP_INT_MASK;
    }
    else
    {
        hwp_sysCtrl->AP_Ctrl &= ~SYS_CTRL_AP_INT_MASK;
    }
    hal_SysExitCriticalSection(status);
}


// =============================================================================
// hal_SysApIrqActive
// -----------------------------------------------------------------------------
/// Check AP irq status.
/// @return TRUE if AP irq is active; FALSE otherwise
// =============================================================================
PROTECTED BOOL hal_SysApIrqActive(VOID)
{
    return ((hwp_sysCtrl->AP_Ctrl & SYS_CTRL_AP_INT_STATUS) != 0);
}
#endif // CHIP_HAS_AP



// =============================================================================
// hal_SysProcessHostMonitor
// -----------------------------------------------------------------------------
/// This function is called in the idle task or in gdb loop.
/// It is used to execute the function processing possible commands
/// from the Host.
// =============================================================================
PUBLIC VOID hal_SysProcessHostMonitor(VOID)
{
    static BOOL locked = FALSE;

    if (locked)
    {
        return;
    }

    locked = TRUE;

    // Process commands from host monitor

#if (CHIP_HAS_USB == 1)
    static BOOL gdbUsbOpened = FALSE;

    if (hal_GdbIsInGdbLoop())
    {
        if (!gdbUsbOpened)
        {
            hal_UsbReset(TRUE);
            hal_TimDelay(HAL_TICK1S);
            hal_UsbReset(FALSE);
#ifdef HAL_USB_DRIVER_DONT_USE_ROMED_CODE
            hal_UsbOpen(hal_HostUsbDeviceCallback);
#else
            hal_UsbOpen(boot_HostUsbDeviceCallback);
#endif
            hal_HostUsbOpen();
            gdbUsbOpened = TRUE;
        }
        // Handle USB irqs if any
        hal_UsbHandleIrqInGdb();
    }

    if(g_halUsbHostEnable)
    {
#ifdef HAL_USB_DRIVER_DONT_USE_ROMED_CODE
        hal_HostUsbRecv();
        hal_HostUsbSend();
#else
        boot_HostUsbRecv();
        boot_HostUsbSend();
#endif // HAL_USB_DRIVER_DONT_USE_ROMED_CODE
    }
#endif // CHIP_HAS_USB

#ifdef CHIP_HAS_AP
    if (g_halApHostEnable)
    {
        hal_HostApRecv();
        hal_HostApSend();
    }
#endif

    locked = FALSE;
}



// =============================================================================
// hal_SysProcessIdle
// -----------------------------------------------------------------------------
/// This function is called in the idle task.
/// It is used to execute in idle the function processing possible commands
/// from the Host, and thus, for example, call the executor. etc ...
/// @return \c TRUE polling is needed
///         \c FALSE otherwise
// =============================================================================
PUBLIC BOOL hal_SysProcessIdle(VOID)
{
    BOOL pollingNeeded = FALSE;

    // Process commands from host
    hal_SysProcessHostMonitor();
    hal_HstRamClientIdle();

#ifdef CHIP_XTAL_CLK_IS_52M
    UINT32 sc = hal_SysEnterCriticalSection();
    if (hal_SysGetFreq() <= HAL_SYS_FREQ_52M)
    {
        if (!hal_SysRequestDdrHighFreq(FALSE))
            pollingNeeded = TRUE;
    }
    hal_SysExitCriticalSection(sc);
#endif

    return pollingNeeded;
}


// =============================================================================
// hal_SysWaitMicrosecond
// -----------------------------------------------------------------------------
/// This function wait some time in microseconds.
/// It calculates CPU instruction cycles needed, depending on current system frequency.
/// Note that it does NOT have critical section operations.
/// @param us Microseconds to wait.
// =============================================================================
PUBLIC VOID HAL_BOOT_FUNC_INTERNAL hal_SysWaitMicrosecond(UINT32 us)
{
// 1 (sltu) + 1 (addiu) + 3 (btnez) = 5 cycles
#define WAIT_US_LOOP_CYCLE 5

    register UINT32 counter = g_halSysSystemFreq/1000000*us/WAIT_US_LOOP_CYCLE;

    asm volatile("move $7, %0\n\t"
                 "li $6, 0\n"
                 "_counter_check:\n\t"
                 "sltu $6, $7\n\t"
                 "addiu $6, 1\n\t"
                 "btnez _counter_check"
                 : /* no output */ : "r"(counter) : "$6", "$7");
}



// =============================================================================
// hal_BootEnterMonitor
// -----------------------------------------------------------------------------
/// Call this function to enter the boot monitor.
// =============================================================================
PUBLIC VOID hal_BootEnterMonitor(VOID)
{
    // We want to reboot now, and not get interrupted by any trivial stuff.
    UINT32 __attribute__((unused)) status = hal_SysEnterCriticalSection();

    if (g_halBootBeforeMonCallback != NULL)
    {
        (*g_halBootBeforeMonCallback)();
    }

    // To restart in the Boot Monitor, by going through
    // the Boot Sector.
    boot_BootSectorSetCommand(BOOT_SECTOR_CMD_ENTER_BOOT_MONITOR);

    hal_SysRestart();
}



// =============================================================================
// hal_BootRegisterBeforeMonCallback
// -----------------------------------------------------------------------------
/// Call this function to register a callback function which is run
/// before entering the boot monitor.
// =============================================================================
PUBLIC VOID hal_BootRegisterBeforeMonCallback(VOID (*funcPtr)(VOID))
{
    g_halBootBeforeMonCallback = funcPtr;
}

#ifdef CHIP_DIE_8809E2
// =============================================================================
// hal_SysSwitchPllAux
// -----------------------------------------------------------------------------
/// Call this function to switch to/from PLL_AUX
/// Mode=1 flight mode
/// Mode=0 normal mode
// =============================================================================
PUBLIC VOID hal_SysSwitchPll(HAL_SYS_PLL_PROFILE_T  Mode)
{
    u32 pd_pll_reg;
    u32 ret;

    //config pll register

    ret = rfd_XcvRegForceWrite(4,SYS_RDA6220_Reg102h,g_halSysPllCfg02h[Mode].reg);

    ret = rfd_XcvRegForceWrite(4,SYS_RDA6220_Reg101h,g_halSysPllCfg01h[Mode].reg);

    if(HAL_SYS_PLL_MODE_FLIGHT == Mode)
    {
        rfd_XcvRegForceRead(0,SYS_RDA6220_Reg07H,&g_halXcvReg_07h );
        rfd_XcvRegForceRead(0,SYS_RDA6220_Reg20H,&g_halXcvReg_20h );
        rfd_XcvRegForceWrite(0,SYS_RDA6220_Reg07H, 0x19700);
        rfd_XcvRegForceWrite(0,SYS_RDA6220_Reg20H, 0xc091);
    }
    else
    {
        rfd_XcvRegForceWrite(0,SYS_RDA6220_Reg07H, g_halXcvReg_07h);
        rfd_XcvRegForceWrite(0,SYS_RDA6220_Reg20H, g_halXcvReg_20h);
    }

    hal_AudSetAuxPllConfig(Mode);

}

#endif

PUBLIC UINT32 *hal_GetSramScratchAddress(VOID)
{
    return &g_halSramScratch[0];
}

PUBLIC UINT32 *hal_GetSramScratchTop(VOID)
{
    return &g_halSramScratch[HAL_SRAM_SCRATCH_SIZE/4 - 1];
}
