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
#ifdef CHIP_DIE_8955

#include "ddr_config.h"
#include "hal_sys.h"
#include "hal_ispi.h"
#include "hal_mem_map.h"
#include "hal_debug.h"
#include "mem_bridge.h"
#include "sys_ctrl.h"
#include "sys_ifc.h"
#include "spi.h"
#include "psram8_ctrl.h"
#include "page_spy.h"
#include "timer.h"
#include "global_macros.h"

#define CHANGE_MR_WITH_FREQ     0
#define SUPPORT_312M            0

#define CYCLE_COUNT_PER_LOOP    5 // based on measurement, mips32 and mips16 are the same
#define LOOP_COUNT_PER_1KUS(hz) ((hz) / ((1000000*CYCLE_COUNT_PER_LOOP)/1024))

#define LOOP_DELAY(count)       for (int n = (count); n > 0; --n) asm volatile ("nop")
#define DELAY_US(us)            LOOP_DELAY(((us) * param->loop_count_per_1kus)/1024)

// =============================================================================
// DDR_CHANGE_PARAM_T
// -----------------------------------------------------------------------------
/// All parameters for vcore and DDR frequency change
// =============================================================================
typedef struct
{
    UINT16  vcore_reg_count;
    UINT16  vcore_reg_idx[8];
    UINT16  vcore_reg_data[8];
    BOOL    ddr_freq_changed;
    BOOL    ddr_use_fast;
    UINT32  ddr_freq;
    UINT32  mr0;
    UINT32  mr4;
    UINT32  ddr_clk_ctrl;
    UINT32  ddr_dqs_ctrl;
    UINT32  ifc_mask;
    volatile UINT32  loop_count_per_1kus;
} DDR_CHANGE_PARAM_T;

PRIVATE DDR_CHANGE_PARAM_T HAL_UNCACHED_BSS_INTERNAL g_ddrChangeParam;

typedef VOID (*DDR_CONFIG_FUNCTION_T)(DDR_CHANGE_PARAM_T *param);

// =============================================================================
// memd_ChangeDdrProtected
// -----------------------------------------------------------------------------
/// The real function to change vcore and/or DDR frequency.
/// 1. The function itself should be located on SRAM, and should be uncached
///    accessed to avoid unpredictable flush write back.
/// 2. The parameter should be a pointer to uncached SRAM.
/// 3. The function itself can't use stack. It is tedious to write assembly to
///    ensure stack won't be used. So, when this function is changed or compiler
///    is changed, it is needed to check the compiling result.
/// 4. The code itself is in cachable section. So, there shouldn't exist RO
///    mixed inside code.
// =============================================================================
PRIVATE VOID HAL_FUNC_INTERNAL __attribute__((noinline)) memd_ChangeDdrProtected(DDR_CHANGE_PARAM_T *param)
{
    hwp_pageSpy->enable = (1 << HAL_DBG_PAGE_14);

    // wait a while to let PSRAM into idle (> 40 PSRAM clock cycles)
    DELAY_US(16);

    if (param->vcore_reg_count > 0)
    {
        for (int n = 0; n < param->vcore_reg_count; n++)
        {
            UINT32 data = SPI_CS(HAL_ISPI_CS_PMU)
                          | (0<<25)
                          | ((param->vcore_reg_idx[n] & 0x1ff) << 16)
                          | param->vcore_reg_data[n];

            // boot_IspiTxFifoAvail
            while (GET_BITFIELD(hwp_spi3->status, SPI_TX_SPACE) < 1)
                ;

            // boot_IspiSendData
            hwp_spi3->rxtx_buffer = data;

            // boot_IspiTxFinished
            for (;;)
            {
                UINT32 status = hwp_spi3->status;
                if ((status & SPI_ACTIVE_STATUS) == 0 &&
                        GET_BITFIELD(status, SPI_TX_SPACE) == SPI_TX_FIFO_SIZE)
                    break;
            }
        }

        DELAY_US(160);
    }

    if (param->ddr_freq_changed)
    {
#if (CHANGE_MR_WITH_FREQ)
        UINT16 read16 __attribute__((unused));
        UINT32 read32 __attribute__((unused));

        UINT32 pageCfg = hwp_pageSpy->enable;
        hwp_pageSpy->disable = pageCfg;
        read16 = *((volatile u16 *)0xa2000000);
        read16 = *((volatile u16 *)0xa2000000);

        LOOP_DELAY(200);     // need 500 times cpu cycle

        hwp_psram8Ctrl->cre = 1;
        *((volatile UINT32 * )0xa2000000) = param->mr0;
        *((volatile UINT32 * )0xa2000000) = param->mr0;
        hwp_psram8Ctrl->cre = 0;

        LOOP_DELAY(16);     // need 10 times cpu cycle

        hwp_psram8Ctrl->cre = 1;
        *((volatile UINT32 * )0xa2000004) = param->mr4;
        *((volatile UINT32 * )0xa2000004) = param->mr4;
        hwp_psram8Ctrl->cre = 0;

        hwp_psram8Ctrl->cre = 1;
        read32 = *((volatile UINT32 * )0xa2000000) ;
        read32 = *((volatile UINT32 * )0xa2000004) ;
        hwp_psram8Ctrl->cre = 0;

        // remove fifo regs access from fifo
        read16 = *((volatile u16 *)0xa2000000);
        read16 = *((volatile u16 *)0xa2000000);
        hwp_pageSpy->enable = pageCfg;

        // flush cpu write buffer
        read32 = hwp_pageSpy->enable;
#endif

        hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;

        if (param->ddr_use_fast)
        {
            if ((hwp_sysCtrl->Sel_Clock & SYS_CTRL_PLL_LOCKED) == 0)
            {
                // Turning off the Pll and reset of the Lock detector
                hwp_sysCtrl->Pll_Ctrl = SYS_CTRL_PLL_ENABLE_POWER_DOWN
                                        | SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE
                                        | SYS_CTRL_PLL_LOCK_RESET_RESET
                                        ;

                // Turning on the Pll
                hwp_sysCtrl->Pll_Ctrl = SYS_CTRL_PLL_ENABLE_ENABLE
                                        | SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE
                                        | SYS_CTRL_PLL_LOCK_RESET_NO_RESET
                                        ;

                // Wait for lock.
                while ((hwp_sysCtrl->Sel_Clock & SYS_CTRL_PLL_LOCKED) == 0);

                // Insert delay for PLL ready
                DELAY_US(2);

                // Enable PLL clock for fast clock
                hwp_sysCtrl->Pll_Ctrl |= SYS_CTRL_PLL_CLK_FAST_ENABLE_ENABLE;

                // Switch to fast clocks
                hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_RF
                                         | SYS_CTRL_SYS_SEL_FAST_FAST
                                         | SYS_CTRL_BB_SEL_FAST_FAST
                                         | SYS_CTRL_SPIFLASH_SEL_FAST_FAST
                                         | SYS_CTRL_VOC_SEL_FAST_FAST
                                         | SYS_CTRL_MEM_BRIDGE_SEL_FAST_FAST
                                         | SYS_CTRL_TCU_13M_SEL_13M
                                         ;

                // Insert delay
                DELAY_US(2);
            }

            UINT32 cfg_clk = SYS_CTRL_MEM_FREQ_156M;
            switch (param->ddr_freq)
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
            }

            hwp_sysCtrl->Cfg_Clk_Mem_Bridge = cfg_clk;
        }
        else
        {
            // Switch to slow clock
            hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_RF
                                     | SYS_CTRL_SYS_SEL_FAST_SLOW
                                     | SYS_CTRL_BB_SEL_FAST_SLOW
                                     | SYS_CTRL_SPIFLASH_SEL_FAST_SLOW
                                     | SYS_CTRL_VOC_SEL_FAST_SLOW
                                     | SYS_CTRL_MEM_BRIDGE_SEL_FAST_SLOW
                                     | SYS_CTRL_TCU_13M_SEL_13M
                                     ;

            // Insert delay
            DELAY_US(2);

            // Turning off PLL
            hwp_sysCtrl->Pll_Ctrl = SYS_CTRL_PLL_ENABLE_POWER_DOWN
                                    | SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE
                                    | SYS_CTRL_PLL_LOCK_RESET_RESET
                                    ;

            hwp_sysCtrl->Cfg_Clk_Mem_Bridge = SYS_CTRL_MEM_FREQ_52M;
        }

        hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    }

    LOOP_DELAY(16);
    hwp_psram8Ctrl->clk_ctrl = param->ddr_clk_ctrl;
    hwp_psram8Ctrl->dqs_ctrl = param->ddr_dqs_ctrl;
    LOOP_DELAY(16);

#if (SUPPORT_312M)
    if (param->ddr_freq_changed)
    {
        UINT32 ctrl_time = hwp_psram8Ctrl->ctrl_time;
        ctrl_time &= ~PSRAM8_CTRL_WL(0x3f);
        if (param->ddr_freq == HAL_SYS_MEM_FREQ_312M)
            ctrl_time |= PSRAM8_CTRL_WL(3);
        hwp_psram8Ctrl->ctrl_time = ctrl_time;
    }
#endif

    hwp_pageSpy->disable = (1 << HAL_DBG_PAGE_14);
    //read hwp_pageSpy->disable to make sure it takes effect 
    UINT32 is_disable = hwp_pageSpy->disable;
    is_disable++;
}

PRIVATE UINT32 memd_WaitDdrMasterDone(VOID)
{
    UINT32 rxmask = 0;
    UINT8 channel;

    // wait DMA done. SRAM to SRAM is rare case, so don't check whether PSRAM is used
    while (!hal_DmaDone())
        ;

    // collect tx/rx mask
    for (channel = 0; channel < SYS_IFC_STD_CHAN_NB; channel++)
    {
        if ((hwp_sysIfc->std_ch[channel].status & SYS_IFC_ENABLE) &&
                (hwp_sysIfc->std_ch[channel].tc != 0))
        {
            UINT32 start_addr = hwp_sysIfc->std_ch[channel].start_addr;
            if (ADDRESS_IN_HWP(start_addr, hwp_cs1))
            {
                UINT32 control = hwp_sysIfc->std_ch[channel].control;
                if ((GET_BITFIELD(control, SYS_IFC_REQ_SRC) & 1) == 1) // RX
                {
                    rxmask |= (1 << channel);
                    hwp_sysIfc->std_ch[channel].control |= SYS_IFC_FLUSH;
                    while ((hwp_sysIfc->std_ch[channel].status & SYS_IFC_FIFO_EMPTY) == 0)
                        ;
                }
            }
        }
    }

    return rxmask;
}

PRIVATE VOID memd_RestoreRxIfc(UINT32 mask)
{
    UINT8 channel;

    for (channel = 0; channel < SYS_IFC_STD_CHAN_NB; channel++)
    {
        if ((mask & (1 << channel)) != 0)
        {
            hwp_sysIfc->std_ch[channel].control &= ~SYS_IFC_FLUSH;
        }
    }
}

PRIVATE BOOL memd_ChangeDdrPermitted(VOID)
{
    // RFSPI IFC will use SRAM, skip it
    if (hal_VocStateActive())
        return FALSE;

    if (hal_GoudaIsActive())
        return FALSE;

    UINT32 channel;
    for (channel = 0; channel < SYS_IFC_STD_CHAN_NB; channel++)
    {
        if ((hwp_sysIfc->std_ch[channel].status & SYS_IFC_ENABLE) &&
                (hwp_sysIfc->std_ch[channel].tc != 0))
        {
            UINT32 start_addr = hwp_sysIfc->std_ch[channel].start_addr;
            if (ADDRESS_IN_HWP(start_addr, hwp_cs1))
            {
                UINT32 control = hwp_sysIfc->std_ch[channel].control;
                if ((GET_BITFIELD(control, SYS_IFC_REQ_SRC) & 1) == 0) // TX
                    return FALSE;
            }
        }
    }

    return TRUE;
}

PUBLIC BOOL memd_ChangeDdr(UINT32 pmuRegCount,
                           UINT16 *pmuRegIdx,
                           UINT16 *pmuRegData,
                           UINT32 freq,
                           BOOL freqChanged,
                           BOOL usePll,
                           HAL_DDR_TIMING_T *timing)
{
    UINT32 sc = hal_SysEnterCriticalSection();

    if (!memd_ChangeDdrPermitted())
    {
        hal_SysExitCriticalSection(sc);
        return FALSE;
    }

    g_ddrChangeParam.ifc_mask = memd_WaitDdrMasterDone();
    g_ddrChangeParam.vcore_reg_count = pmuRegCount;
    for (int n = 0; n < pmuRegCount; n++)
    {
        g_ddrChangeParam.vcore_reg_idx[n] = pmuRegIdx[n];
        g_ddrChangeParam.vcore_reg_data[n] = pmuRegData[n];
    }
    g_ddrChangeParam.ddr_freq_changed = freqChanged;
    g_ddrChangeParam.ddr_freq = freq;
    g_ddrChangeParam.ddr_use_fast = usePll;
    g_ddrChangeParam.mr0 = timing->mr0;
    g_ddrChangeParam.mr4 = timing->mr4;
    g_ddrChangeParam.ddr_clk_ctrl = timing->ddr_clk_ctrl;
    g_ddrChangeParam.ddr_dqs_ctrl = timing->ddr_dqs_ctrl;
    g_ddrChangeParam.loop_count_per_1kus = LOOP_COUNT_PER_1KUS(hal_SysGetFreq());

    // boot_FlushDCache(TRUE);
    memd_ChangeDdrProtected(&g_ddrChangeParam);

    memd_RestoreRxIfc(g_ddrChangeParam.ifc_mask);
    hal_SysExitCriticalSection(sc);
    return TRUE;
}

PUBLIC BOOL memd_ChangeVcore(UINT32 pmuRegCount,
                             UINT16 *pmuRegIdx,
                             UINT16 *pmuRegData,
                             HAL_DDR_TIMING_T *timing)
{
    return memd_ChangeDdr(pmuRegCount, pmuRegIdx, pmuRegData, 0, FALSE, FALSE, timing);
}

#endif

