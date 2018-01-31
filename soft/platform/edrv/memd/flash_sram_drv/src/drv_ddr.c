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

#if defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)
#include "cs_types.h"
#include "hal_ebc.h"
#include "hal_sys.h"
#include "string.h"
#include "memd_m.h"
#include "memdp_debug.h"
#include "memdp.h"
#include "hal_host.h"
#include "sys_ctrl.h"
#include "spi_flash.h"
#include "global_macros.h"
#include "psram8_ctrl.h"
#include "page_spy.h"
#include "tgt_ddr_cfg.h"
#include "pmd_m.h"


HAL_DATA_INTERNAL  DDR_TIMING_T*  g_ddrConfig;
HAL_DATA_INTERNAL UINT16  g_ddr_table_size;
HAL_DATA_INTERNAL UINT16  g_ddrIndex = 0;
PRIVATE HAL_DATA_INTERNAL UINT16  g_ddrIndex_last= 0;
PRIVATE HAL_DATA_INTERNAL  HAL_SYS_MEM_FREQ_T ddrFreq_last = 0;
HAL_DATA_INTERNAL MEMD_RAM_CONFIG_T* memd_cfg;
HAL_DATA_INTERNAL BOOL memd_cfg_done = TRUE;

#define _USE_ARRAY_DDR_CONFIG_

#define DDR_CONFIG_DELAY   {  \
        asm("nop");           \
        asm("nop");           \
        asm("nop");           \
        asm("nop");           \
        asm("nop");           \
        asm("nop");           \
        asm("nop");           \
        asm("nop");           \
        asm("nop");           \
        asm("nop");           \
        asm("nop");           \
        asm("nop");           \
        asm("nop");           \
        asm("nop");           \
        asm("nop");           \
        asm("nop");           \
    }




UINT16  MEMD_BOOT_FUNC  DDR_Get_Config(UINT8  vcore,HAL_SYS_MEM_FREQ_T ebcFreq )
{
    UINT8 i;
    DRV_DDR_FREQ_T ddr_clk;
    switch (ebcFreq)
    {
        case HAL_SYS_MEM_FREQ_26M:
            ddr_clk = DDR_FREQ_26M;
            break;

        case HAL_SYS_MEM_FREQ_39M:
            ddr_clk = DDR_FREQ_39M;
            break;

        case HAL_SYS_MEM_FREQ_52M:
            ddr_clk = DDR_FREQ_52M;
            break;

        case HAL_SYS_MEM_FREQ_78M:
            ddr_clk = DDR_FREQ_78M;
            break;

        case HAL_SYS_MEM_FREQ_89M:
            ddr_clk = DDR_FREQ_89M;
            break;

        case HAL_SYS_MEM_FREQ_104M:
            ddr_clk = DDR_FREQ_104M;
            break;

        case HAL_SYS_MEM_FREQ_113M:
            ddr_clk = DDR_FREQ_113M;
            break;

        case HAL_SYS_MEM_FREQ_125M:
            ddr_clk = DDR_FREQ_125M;
            break;

        case HAL_SYS_MEM_FREQ_139M:
            ddr_clk = DDR_FREQ_139M;
            break;

        case HAL_SYS_MEM_FREQ_156M:
            ddr_clk = DDR_FREQ_156M;
            break;

        case HAL_SYS_MEM_FREQ_178M:
            ddr_clk = DDR_FREQ_178M;
            break;

        case HAL_SYS_MEM_FREQ_208M:
            ddr_clk = DDR_FREQ_208M;
            break;

        case HAL_SYS_MEM_FREQ_250M:
            ddr_clk = DDR_FREQ_250M;
            break;

        case HAL_SYS_MEM_FREQ_312M:
            ddr_clk = DDR_FREQ_312M;
            break;
        default:
            ddr_clk = DDR_FREQ_156M;
            break;
    }
    for(i = 0; i < g_ddr_table_size; i++)
    {
        if((g_ddrConfig[i].vcore == vcore)&&((g_ddrConfig[i].mem_clk == ddr_clk)))
        {
            break;
        }

    }

    MEMD_ASSERT(i < g_ddr_table_size,"ddr vcore config invalid i= %d vcore =0x%x  ebcFreq =0x%x.",i,vcore,ebcFreq);
    return  i;
}

PRIVATE VOID HAL_FUNC_INTERNAL
memd_RamConfigureBurstMode(REG16* csBase, REG32* pCRE)
{
#ifdef CHIP_DIE_8909
    *pCRE = 1;
    *(csBase+0)=0xb; //TODO: 8909 mow using RAM_CLK_IS_39M
    *pCRE = 0;
#else
    UINT32 mr0,mr4,read1,read2;
    mr0 = g_ddrConfig[g_ddrIndex].ddr.mr0;
    mr4 = g_ddrConfig[g_ddrIndex].ddr.mr4;
    mon_Event(0x66660021);
    mon_Event(mr0);
    mon_Event(mr4);
    DDR_CONFIG_DELAY
    *pCRE = 1;
    *((volatile UINT32 * )0xa2000000) = mr0;
    *((volatile UINT32 * )0xa2000000) = mr0;
    *pCRE = 0;
    DDR_CONFIG_DELAY
    *pCRE = 1;
    *((volatile UINT32 * )0xa2000004) = mr4;
    *((volatile UINT32 * )0xa2000004) = mr4;
    *pCRE = 0;

    *pCRE = 1;
    read1 = *((volatile UINT32 * )0xa2000000) ;
    read2 = *((volatile UINT32 * )0xa2000004) ;
    *pCRE = 0;
#endif
}

VOID MEMD_BOOT_FUNC  DDR_Init_Config(VOID)
{
    if(NULL == g_ddrConfig)
    {
        g_ddrConfig = tgt_GetDDRConfig(&g_ddr_table_size);
    }
}

PUBLIC VOID memd_DdrGetTiming(UINT8 vcore, HAL_SYS_MEM_FREQ_T ddrFreq, HAL_DDR_TIMING_T* timing)
{
    UINT16 index = DDR_Get_Config(vcore, ddrFreq);
    *timing = g_ddrConfig[index].ddr;
    ddrFreq_last = ddrFreq;
    g_ddrIndex = index;
}

PUBLIC HAL_SYS_MEM_FREQ_T MEMD_BOOT_FUNC memd_GetRamClk(VOID)
{
    return ddrFreq_last;
}

PUBLIC BOOL memd_AdjustRamClk(HAL_SYS_MEM_FREQ_T ddrFreq, BOOL usePll)
{
    UINT32 sc = hal_SysEnterCriticalSection();

    UINT16 regIdx[8];
    UINT16 regData[8];
    PMD_CORE_VOLTAGE_T oldVolt = pmd_GetCoreVoltageRequest(PMD_CORE_VOLTAGE_USER_DDR);
    PMD_CORE_VOLTAGE_T volt = (ddrFreq <= HAL_SYS_MEM_FREQ_52M)?
                              PMD_CORE_VOLTAGE_LOW : PMD_CORE_VOLTAGE_MEDIUM;
    UINT32 regCount = pmd_SetCoreVoltageDelayed(PMD_CORE_VOLTAGE_USER_DDR, volt, regIdx, regData);

    HAL_DDR_TIMING_T timing;
    UINT8 vcore = pmd_GetVoreVoltageDDRIndex();
    memd_DdrGetTiming(vcore, ddrFreq, &timing);

    BOOL changed = memd_ChangeDdr(regCount,
                                  regIdx,
                                  regData,
                                  ddrFreq,
                                  TRUE,
                                  usePll,
                                  &timing);
    if (!changed)
    {
        pmd_SetCoreVoltageDelayed(PMD_CORE_VOLTAGE_USER_DDR, oldVolt, regIdx, regData);
    }
    hal_SysExitCriticalSection(sc);
    return changed;
}

VOID HAL_FUNC_INTERNAL memd_GetCfgDDRIndex(DDR_TIMING_T*  ddrConfig)
{
    ddrConfig->ddr.ddr_clk_ctrl = g_ddrConfig[g_ddrIndex].ddr.ddr_clk_ctrl;
    ddrConfig->ddr.ddr_dqs_ctrl = g_ddrConfig[g_ddrIndex].ddr.ddr_dqs_ctrl;
    ddrConfig->mem_clk = g_ddrConfig[g_ddrIndex].mem_clk;
    ddrConfig->vcore = g_ddrConfig[g_ddrIndex].vcore;
}

PUBLIC VOID MEMD_BOOT_FUNC memd_RamOpen(CONST MEMD_RAM_CONFIG_T* cfg)
{
#ifdef CHIP_DIE_8909
    hal_EbcSramOpen(&cfg->csConfig, memd_RamConfigureBurstMode);

    return;
#endif
    UINT8 vcore = pmd_GetVoreVoltageDDRIndex();

    DDR_Init_Config();
    memd_cfg = cfg;
    hwp_psram8Ctrl->power_up = PSRAM8_CTRL_HW_POWER_PULSE;
    while (!((hwp_psram8Ctrl->power_up & PSRAM8_CTRL_INIT_DONE) == PSRAM8_CTRL_INIT_DONE)) {};

#if defined(RAM_CLK_IS_78M)
    ddrFreq_last = HAL_SYS_MEM_FREQ_78M;
#elif defined(RAM_CLK_IS_104M)
    ddrFreq_last = HAL_SYS_MEM_FREQ_104M;
#elif defined(RAM_CLK_IS_139M)
    ddrFreq_last = HAL_SYS_MEM_FREQ_139M;
#elif defined(RAM_CLK_IS_156M)
    ddrFreq_last = HAL_SYS_MEM_FREQ_156M;
#elif defined(RAM_CLK_IS_178M)
    ddrFreq_last = HAL_SYS_MEM_FREQ_178M;
#elif defined(RAM_CLK_IS_208M)
    ddrFreq_last = HAL_SYS_MEM_FREQ_208M;
#elif defined(RAM_CLK_IS_250M)
    ddrFreq_last = HAL_SYS_MEM_FREQ_250M;
#else
#error please config ddr timing.
#endif

    g_ddrIndex = DDR_Get_Config(vcore, ddrFreq_last);
#ifdef FPGA
    hwp_psram8Ctrl->clk_ctrl = 0x9;//0x6 old
    hwp_psram8Ctrl->dqs_ctrl = 0x90000;//0x60000 old value ,new borad is 9
#else // FPGA
    hwp_psram8Ctrl->timeout_val = PSRAM8_CTRL_TIMEOUT_VALUE(0x7ffff);
    hwp_psram8Ctrl->clk_ctrl = g_ddrConfig[g_ddrIndex].ddr.ddr_clk_ctrl;
    hwp_psram8Ctrl->dqs_ctrl = g_ddrConfig[g_ddrIndex].ddr.ddr_dqs_ctrl;
#endif // FPGA
    hal_EbcSramOpen(&cfg->csConfig, memd_RamConfigureBurstMode);
}

#endif
