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

#include "tgt_memd_cfg.h"
#include "tgt_ddr_cfg.h"
#include "tgt_ddr_freq_cfg.h"
#include "memd_m.h"
#include "pmd_m.h"
#include "boot_usb_monitor.h"

#include "hal_sys.h"
#include "hal_debug.h"
#include "hal_dma.h"
#include "hal_clk.h"
#include "hal_timers.h"
#include "hal_ebc.h"
#include "hal_map_engine.h"
#include "global_macros.h"
#include "sys_ctrl.h"
#include "psram8_ctrl.h"
#include "debug_host.h"

typedef struct _DDR_CHECK_CASE_T
{
    UINT8 vcore;
    UINT32 freq;
    UINT32 clk_min;
    UINT32 clk_max;
    UINT32 clk;
    UINT32 dqs;
    UINT32 mr0;
    UINT32 mr4;
} DDR_CHECK_CASE_T;

PRIVATE DDR_CHECK_CASE_T g_ddrCases[] =
{
#ifdef CHIP_DIE_8955_U01
#include "ddrinit_u01.h"
#else
#include "ddrinit_u02.h"
#endif
    { 0, 0, 0, 0, 0}
};

#define LOOP_DELAY(count)       for (int n = 0; n < (count); n++) asm volatile ("nop")

// Ouput event without timeout
PRIVATE VOID mon_Event(UINT32 evt)
{
    while (hwp_debugHost->event == 0);
    hwp_debugHost->event = evt;
}

PRIVATE UINT32 g_ddrCheckPattern[] =
{
    0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
    0x5a5a5a5a, 0x5a5a5a5a, 0x5a5a5a5a, 0x5a5a5a5a,
    0x00000000, 0xffffffff, 0x00000000, 0xffffffff,
    0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f,
    0x00000000, 0x00000001, 0x00000000, 0x00000000,
    0xffffffff, 0xffffffff, 0x7fffffff, 0xffffffff,
    0x4f35b7da, 0x8e354c91, 0x4f35b7da, 0x8e354c91,
    0x00000000, 0x00000000, 0x00100000, 0x00000000
};

PRIVATE BOOL ddr_verify_rw(UINT32* address)
{
    int count = sizeof(g_ddrCheckPattern)/sizeof(g_ddrCheckPattern[0]);

    for (int n = 0; n < count; n++)
        address[n] = g_ddrCheckPattern[n];

    for (int n = 0; n < count; n++)
        if (address[n] != g_ddrCheckPattern[n])
            return FALSE;
    return TRUE;
}

PRIVATE BOOL ddr_verify(VOID)
{
    if (ddr_verify_rw((UINT32*)0xa2000000) &&
            ddr_verify_rw((UINT32*)0xa20cccc0) &&
            ddr_verify_rw((UINT32*)0xa2155550) &&
            ddr_verify_rw((UINT32*)0xa23aaaa0))
        return TRUE;
    return FALSE;
}

PRIVATE VOID ddr_setController(UINT32 freq, HAL_DDR_TIMING_T *timing, int clk)
{
    LOOP_DELAY(16);
    hwp_psram8Ctrl->clk_ctrl = clk;
    hwp_psram8Ctrl->dqs_ctrl = (timing->ddr_dqs_ctrl & 0xffff) | (clk << 16);
    LOOP_DELAY(16);
}

PRIVATE VOID ddr_setFreq(UINT32 freq)
{
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;

    if (freq == HAL_SYS_MEM_FREQ_52M)
    {
        UINT32 Sel_Clock = hwp_sysCtrl->Sel_Clock;
        Sel_Clock |= SYS_CTRL_MEM_BRIDGE_SEL_FAST;
        hwp_sysCtrl->Sel_Clock = Sel_Clock;
        hwp_sysCtrl->CFG_XTAL_DIV = SYS_CTRL_CFG_XTAL_DIV(1)|SYS_CTRL_CFG_XTAL_DIV_UPDATE;
    }
    else
    {
        UINT32 Sel_Clock = hwp_sysCtrl->Sel_Clock;
        Sel_Clock &= ~SYS_CTRL_MEM_BRIDGE_SEL_FAST;
        hwp_sysCtrl->Sel_Clock = Sel_Clock;

        UINT32 cfg_clk;
        switch (freq)
        {
            case HAL_SYS_MEM_FREQ_26M:  cfg_clk = SYS_CTRL_MEM_FREQ_26M; break;
            case HAL_SYS_MEM_FREQ_39M:  cfg_clk = SYS_CTRL_MEM_FREQ_39M; break;
            case HAL_SYS_MEM_FREQ_52M:  cfg_clk = SYS_CTRL_MEM_FREQ_52M; break;
            case HAL_SYS_MEM_FREQ_78M:  cfg_clk = SYS_CTRL_MEM_FREQ_78M; break;
            case HAL_SYS_MEM_FREQ_89M:  cfg_clk = SYS_CTRL_MEM_FREQ_89M; break;
            case HAL_SYS_MEM_FREQ_104M: cfg_clk = SYS_CTRL_MEM_FREQ_104M; break;
            case HAL_SYS_MEM_FREQ_113M: cfg_clk = SYS_CTRL_MEM_FREQ_113M; break;
            case HAL_SYS_MEM_FREQ_125M: cfg_clk = SYS_CTRL_MEM_FREQ_125M; break;
            case HAL_SYS_MEM_FREQ_139M: cfg_clk = SYS_CTRL_MEM_FREQ_139M; break;
            case HAL_SYS_MEM_FREQ_156M: cfg_clk = SYS_CTRL_MEM_FREQ_156M; break;
            case HAL_SYS_MEM_FREQ_178M: cfg_clk = SYS_CTRL_MEM_FREQ_178M; break;
            case HAL_SYS_MEM_FREQ_208M: cfg_clk = SYS_CTRL_MEM_FREQ_208M; break;
            case HAL_SYS_MEM_FREQ_250M: cfg_clk = SYS_CTRL_MEM_FREQ_250M; break;
            case HAL_SYS_MEM_FREQ_312M: cfg_clk = SYS_CTRL_MEM_FREQ_312M; break;
            default: asm("break 1"); break;
        }

        hwp_sysCtrl->Cfg_Clk_Mem_Bridge = cfg_clk;
    }
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
}

PRIVATE BOOL ddr_timingCheck(UINT32 freq, HAL_DDR_TIMING_T *timing, int clk)
{
    ddr_setController(freq, timing, clk);
    return ddr_verify();
}

PRIVATE int ddr_FindLeft(UINT32 freq, HAL_DDR_TIMING_T *timing, int clk)
{
    for (int next = clk-1; next >= 0; next--)
    {
        if (!ddr_timingCheck(freq, timing, next))
            return next+1;
    }
    return 0;
}

PRIVATE int ddr_FindRight(UINT32 freq, HAL_DDR_TIMING_T *timing, int clk)
{
    for (int next = clk+1; next <= 127; next++)
    {
        if (!ddr_timingCheck(freq, timing, next))
            return next-1;
    }
    return 127;
}

PRIVATE int ddr_FindAround(UINT32 freq, HAL_DDR_TIMING_T *timing, int clk)
{
    int clkmin = clk - 1;
    int clkmax = clk + 1;

    for (;;)
    {
        if (clkmin < 0 && clkmax > 127)
            return -1;

        if (clkmin >= 0 && ddr_timingCheck(freq, timing, clkmin))
            return clkmin;
        if (clkmax <= 127 && ddr_timingCheck(freq, timing, clkmax))
            return clkmax;

        clkmin--;
        clkmax++;
    }
}

PRIVATE VOID ddr_check(UINT32 freq, HAL_DDR_TIMING_T *timing)
{
    int clk = timing->ddr_clk_ctrl;
    int clkmin = -1;
    int clkmax = -1;
    if (ddr_timingCheck(freq, timing, clk))
    {
        clkmin = ddr_FindLeft(freq, timing, clk);
        clkmax = ddr_FindRight(freq, timing, clk);
    }
    else
    {
        int clkvalid = ddr_FindAround(freq, timing, clk);
        if (clkvalid >= 0)
        {
            if (clkvalid > clk)
            {
                clkmin = clkvalid;
                clkmax = ddr_FindRight(freq, timing, clkmin);
            }
            else
            {
                clkmax = clkvalid;
                clkmin = ddr_FindLeft(freq, timing, clkmax);
            }
        }
    }

    if (clkmin == -1 || clkmax == -1)
    {
        mon_Event(0xDD3F0000);
    }
    else
    {
        mon_Event(0xDD300000 | (clkmin << 8) | clkmax);
        mon_Event(0xDD400000 | (timing->ddr_dqs_ctrl&0xffff));
        mon_Event(0xDD500000 | (timing->mr4 << 8) | timing->mr0);
    }
}

PROTECTED int main(VOID)
{
    mon_Event(0xDDCA0000);

    UINT32 sc = hal_SysEnterCriticalSection();

    // Initialize the map engine.
    hal_MapEngineOpen();

    // Initialize the System IFC.
    hal_IfcOpen();

    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->CFG_XTAL_DIV = SYS_CTRL_CFG_XTAL_DIV(1)|SYS_CTRL_CFG_XTAL_DIV_UPDATE;  // set 26M to 52M
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;

    hal_BoardSetup(tgt_GetHalConfig());

    hal_SysSetupSystemClock(HAL_SYS_FREQ_26M);

    rfd_XcvReadChipId();

    hal_IspiOpen(NULL);

    pmd_Open(tgt_GetPmdConfig());
    pmd_SetPowerMode(PMD_IDLEPOWER);

    rfd_XcvOpen(0, tgt_GetXcvConfig());

    hal_SwRequestClk(FOURCC_TOOL, HAL_CLK_RATE_104M);
    hal_TimWatchDogClose();

    mon_Event(0xDDCA0000);

    for (int n = 0; n < sizeof(g_ddrCases)/sizeof(g_ddrCases[0]); n++)
    {
        DDR_CHECK_CASE_T *tc = &g_ddrCases[n];
        if (tc->freq == 0)
            break;

        pmd_ForceSetCoreVoltage((tc->vcore & 0x80)? FALSE:TRUE, tc->vcore);

        HAL_DDR_TIMING_T timing;
        timing.ddr_clk_ctrl = tc->clk;
        timing.ddr_dqs_ctrl = tc->dqs | (tc->clk << 16);
        timing.mr0 = 0xf;
        timing.mr4 = 0;

        ddr_setController(tc->freq, &timing, tc->clk);
        ddr_setFreq(tc->freq);

        hal_TimDelay(164); // 10ms

        UINT32 mhz = (tc->freq + 500000) / 1000000;
        mon_Event(0xDD000000 | (tc->vcore << 16) | mhz);
        ddr_check(tc->freq, &timing);
    }

    mon_Event(0xDDCA0000);
    while (1);
    return 0;
}

