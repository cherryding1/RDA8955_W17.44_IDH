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
#include "cfg_regs.h"
#include "sys_ctrl.h"
#include "mem_bridge.h"
#include "page_spy.h"

#include "bootp_host.h"
#include "boot_sector.h"
#include "boot_sector_driver.h"
#include "boot_sys.h"
#include "boot_mem_init.h"
#include "boot_cache.h"
#include "hal_mem_map.h"
#include "memd_m.h"
#include "tgt_memd_cfg.h"
#ifdef USE_OPIDDR_PSRAM
#include "psram8_ctrl.h"
#endif
#include "reg_alias.h"

// =============================================================================
//  ASSERT
// -----------------------------------------------------------------------------
/// Assert: If the boolean condition (first parameter) is false,
/// raise an assert.
// =============================================================================
#define MEM_INIT_ASSERT(condition, format, ...) \
    if (UNLIKELY(!(condition))) { asm volatile("break 2"); }


#if !(defined(CHIP_HAS_AP) && defined(RUN_IN_DDR))

/// Wait for the EBC config to be coherent
/// WARNING: When the BB side is running, on a multi-fifo
/// system, this must be implemented using the EBC_Status
/// CS_Update bit.
/// (Reading the Fifo flush just ensure the fifo is empty).
INLINE VOID boot_EbcWaitReady(VOID)
{
#ifdef USE_OPIDDR_PSRAM
#else

    UINT32 flush __attribute__((unused)) = hwp_memFifo->Flush;
#endif
}


// ============================================================================
// boot_EbcCsSetupWithCallback
// ----------------------------------------------------------------------------
/// Special handler in internal sram, will call a func that must also be in sram that is responsible for
/// switching the flash/sram in burst mode. No function not in internal sram should be called.
/// stack must be also in internal sram
/// no other master (bcpu, dma, voc ...) should access the CS, ebc lock prevent this
///
/// @param cs Chip Select to Enable.
/// @param csConfig Configuration for the chip select. The \c csEn enable bit
/// of the mode configuration must be set to 1 there if the chip select
/// of the opened peripheral have to be enabled !
/// @param cbFunc callback function for enabling the burst mode on sram
/// or NULL if not needed. Note: cbFunc must not be in sram
// ============================================================================

// NOTE: With O1 or above optimization (-ftree-switch-conversion),
// the initializations of the switch statement will be translated
// to initializations from a scalar array in .rodata.
// This will lead to external RAM access, for .rodata in this
// file is located in RAM.
// In order to avoid this issue, no-tree-switch-conversion
// optimization is specified for boot_EbcCsSetupWithCallback.
#define OPT_NO_TREE_SWITCH_CONV __attribute__((optimize("no-tree-switch-conversion")))

PROTECTED VOID HAL_BOOT_FUNC_INTERNAL OPT_NO_TREE_SWITCH_CONV
boot_EbcCsSetupWithCallback(HAL_EBC_CS_T cs, CONST HAL_EBC_CS_CFG_T* csConfig, HAL_EBC_BURSTCALLBACK_T* cbFunc)
{
    // union representing the status bitfield in a 32 bits value
    // loadable in the register
    union
    {
        UINT32 reg;
        HAL_EBC_TIME_CFG_T bitfield;
    } timeCfgUnion;

    // union representing the status bitfield in a 32 bits value
    // loadable in the register
    union
    {
        UINT32 reg;
        HAL_EBC_MODE_CFG_T bitfield;
    } modeCfgUnion;

    UINT32 pageCfg;
    UINT16* csBase;

    timeCfgUnion.bitfield = csConfig->time;
    modeCfgUnion.bitfield = csConfig->mode;

    pageCfg = hwp_pageSpy->enable;
    hwp_pageSpy->disable = pageCfg;
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_8850)
#ifdef USE_OPIDDR_PSRAM
#else
    hwp_memBridge->FIFO_Ctrl = MEM_BRIDGE_LOCK_ENABLE;
#endif
#endif
    // WARNING: In case of two fifos system (one for the BB side,
    // one for the SYS side), we should check here that both fifo
    // are empty. We are currently assuming that the baseband is
    // not running when we are calling those functions.

    switch(cs)
    {
#ifdef CHIP_HAS_AP
        case HAL_EBC_SRAM :
            csBase = (UINT16*)hwp_cs0;
            break;

#else // !CHIP_HAS_AP
        case HAL_EBC_FLASH :
            csBase = (UINT16*)hwp_cs0;
            break;

        case HAL_EBC_SRAM :
            csBase = (UINT16*)hwp_cs1;
            break;

#endif // !CHIP_HAS_AP
        default:
            csBase = NULL;
            break;
    }
#ifdef USE_OPIDDR_PSRAM
    if (cbFunc)
    {
        cbFunc(csBase,&(hwp_psram8Ctrl->cre));
    }

#else
    // call the callback
    if (cbFunc)
    {
        cbFunc(csBase,&(hwp_memFifo->EBC_CRE));
    }

    boot_EbcWaitReady(); // OK because inline
    hwp_memBridge->CS_Time_Write          = timeCfgUnion.reg;
    switch(cs)
    {
#ifdef CHIP_HAS_AP
        case HAL_EBC_SRAM :
            hwp_memBridge->CS_Config[0].CS_Mode   = modeCfgUnion.reg;

            // Save the config in the Boot Sector structure, so that
            // it can be used by the Boot Sector code to configure the
            // EBC CS RAM mode and timings, it is especially useful
            // in burst mode.
            boot_BootSectorSetEbcConfig(timeCfgUnion.reg, modeCfgUnion.reg);

            break;

#else // !CHIP_HAS_AP
        case HAL_EBC_FLASH :
#ifndef __PRJ_WITH_SPIFLSH__
            hwp_memBridge->CS_Config[0].CS_Mode   = modeCfgUnion.reg;
#endif
            break;

        case HAL_EBC_SRAM :

#if defined(CHIP_DIE_8809E2) || defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)
            hwp_memBridge->CS_Config[0].CS_Mode   = modeCfgUnion.reg;
#else
            hwp_memBridge->CS_Config[1].CS_Mode   = modeCfgUnion.reg;
#endif
            // Save the config in the Boot Sector structure, so that
            // it can be used by the Boot Sector code to configure the
            // EBC CS RAM mode and timings, it is especially useful
            // in burst mode.
            boot_BootSectorSetEbcConfig(timeCfgUnion.reg, modeCfgUnion.reg);

            break;

#endif // !CHIP_HAS_AP
        default:
            break;
    }

    boot_EbcWaitReady(); // OK because inline

#if (CHIP_ASIC_ID != CHIP_ASIC_ID_8850)
    hwp_memBridge->FIFO_Ctrl = MEM_BRIDGE_LOCK_DISABLE;
#endif
#endif // !USE_OPIDDR_PSRAM
//if granite_test_chip
    {
        // remove fifo regs access from fifo
        UINT16 read __attribute__((unused)) = *((volatile UINT16 *)csBase);
        read = *((volatile u16 *)csBase);
    }
//endif

    hwp_pageSpy->enable = pageCfg;
    // flush cpu write buffer
    {
        UINT32 unused __attribute__((unused)) = hwp_pageSpy->enable;
    }

}


// ============================================================================
// boot_EbcSramOpen
// ----------------------------------------------------------------------------
/// Enable a CS for SRAM. The chip selects for
/// The enabling of a CS returns the base address of the configured space.
/// The settings are given at the opening of the peripheral on the given
/// chip select, and must support the external maximal clock
///
/// @param csConfig Configuration for the chip select. The \c csEn enable bit
/// of the mode configuration must be set to 1 there if the chip select
/// of the opened peripheral have to be enabled !
/// @param cbFunc callback function for enabling the burst mode on sram
/// or NULL if not needed. Note: cbFunc must not be in sram
/// @return Base address of the configured space
// ============================================================================
PUBLIC VOID* boot_EbcSramOpen(CONST HAL_EBC_CS_CFG_T* csConfig, HAL_EBC_BURSTCALLBACK_T* cbFunc)
{
    VOID* retval = NULL;

    //ENTER CRITICAL
    UINT32 status = hal_SysEnterCriticalSection();

    boot_EbcCsSetupWithCallback(HAL_EBC_SRAM, csConfig, cbFunc);

#ifdef CHIP_HAS_AP
    retval = (VOID*)hwp_cs0;
#else
    retval = (VOID*)hwp_cs1;
#endif

    //EXIT CRITICAL
    hal_SysExitCriticalSection(status);

    return retval;
}


// ============================================================================
// boot_EbcCsOpen
// ----------------------------------------------------------------------------
/// Enable a CS other than the one for FLASH.
/// The enabling of a CS returns the base address of the configured space.
/// The settings are given at the opening of the peripheral on the given
/// chip select, and must support the external maximal clock.
///
/// @param cs Chip Select to Enable. (HAL_EBC_SRAM and CS2 or above)
/// @param csConfig Configuration for the chip select. The \c csEn enable bit
/// of the mode configuration must be set to 1 there if the chip select
/// of the opened peripheral have to be enabled !
/// @return Base address of the configured space
// ============================================================================
PUBLIC VOID* boot_EbcCsOpen(HAL_EBC_CS_T cs, CONST HAL_EBC_CS_CFG_T* csConfig)
{
    union
    {
        UINT32 reg;
        HAL_EBC_TIME_CFG_T bitfield;
    } timeCfgUnion; // union representing the status bitfield in a 32 bits value
    // loadable in the register

    union
    {
        UINT32 reg;
        HAL_EBC_MODE_CFG_T bitfield;
    } modeCfgUnion; // union representing the status bitfield in a 32 bits value
    // loadable in the register

    VOID* retval = NULL;

    //ENTER CRITICAL
    UINT32 status = hal_SysEnterCriticalSection();

    timeCfgUnion.bitfield = csConfig->time;
    modeCfgUnion.bitfield = csConfig->mode;

    // Wait if the lock is locked
    boot_EbcWaitReady();
    switch(cs)
    {
#ifdef CHIP_HAS_AP
        case HAL_EBC_SRAM :
            // Write the pointer into the table
            //g_halEbcCsConfigArray[1]         = csConfig;
            hwp_memBridge->CS_Time_Write          = timeCfgUnion.reg;
            hwp_memBridge->CS_Config[0].CS_Mode   = modeCfgUnion.reg;

            // Save the config in the Boot Sector structure, so that
            // it can be used by the Boot Sector code to configure the
            // EBC CS RAM mode and timings, it is especially useful
            // in burst mode.
            boot_BootSectorSetEbcConfig(timeCfgUnion.reg, modeCfgUnion.reg);

            retval = (VOID*)hwp_cs0;
            break;

#else // !CHIP_HAS_AP
        case HAL_EBC_FLASH :
            MEM_INIT_ASSERT(FALSE, "Improper use of the boot_EbcCsOpen function."
                            "It cannot be used to open CS0 ! Use hal_EbcFlashOpen"
                            " to do that.");
            break;
#ifdef USE_OPIDDR_PSRAM
#else

        case HAL_EBC_SRAM :
            // Write the pointer into the table
            //g_halEbcCsConfigArray[1]         = csConfig;
            hwp_memBridge->CS_Time_Write          = timeCfgUnion.reg;

#if defined(CHIP_DIE_8809E2) || defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)
            hwp_memBridge->CS_Config[0].CS_Mode   = modeCfgUnion.reg;
            if(hwp_memBridge->CS_Config[0].CS_Time != timeCfgUnion.reg)
            	{
            	    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
					hwp_sysCtrl->Sys_Rst_Set= SYS_CTRL_SOFT_RST;
            	}
#else
            hwp_memBridge->CS_Config[1].CS_Mode   = modeCfgUnion.reg;
            if(hwp_memBridge->CS_Config[1].CS_Time != timeCfgUnion.reg)
            	{
            		hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
            		hwp_sysCtrl->Sys_Rst_Set= SYS_CTRL_SOFT_RST;
            	}

#endif
            // Save the config in the Boot Sector structure, so that
            // it can be used by the Boot Sector code to configure the
            // EBC CS RAM mode and timings, it is especially useful
            // in burst mode.

            boot_BootSectorSetEbcConfig(timeCfgUnion.reg, modeCfgUnion.reg);

            retval = (VOID*)hwp_cs1;
            break;
#if !defined(CHIP_DIE_8955) && !defined(CHIP_DIE_8909)

        case HAL_EBC_CS2 :
            // Write the pointer into the table
            //g_halEbcCsConfigArray[2]         = csConfig;
            hwp_memBridge->CS_Time_Write          = timeCfgUnion.reg;
            hwp_memBridge->CS_Config[2].CS_Mode   = modeCfgUnion.reg;

            retval = (VOID*)hwp_cs2;
            break;

        case HAL_EBC_CS3 :
            //g_halEbcCsConfigArray[3]         = csConfig;
            hwp_memBridge->CS_Time_Write          = timeCfgUnion.reg;
            hwp_memBridge->CS_Config[3].CS_Mode   = modeCfgUnion.reg;

            retval = (VOID*)hwp_cs3;
            break;

        case HAL_EBC_CS4:
            //g_halEbcCsConfigArray[4]         = csConfig;
            hwp_memBridge->CS_Time_Write          = timeCfgUnion.reg;
            hwp_memBridge->CS_Config[4].CS_Mode   = modeCfgUnion.reg;

            retval = (VOID*)hwp_cs4;
            break;
#endif
#endif //!USE_OPIDDR_PSRAM
#endif // !CHIP_HAS_AP
        default:
            break;
    }

    //EXIT CRITICAL
    hal_SysExitCriticalSection(status);

    return retval;
}

#if 0 //def CHIP_DIE_8955

// ============================================================================
// boot_EbcCsSetup
// ----------------------------------------------------------------------------
/// Configure the CS settings according to the system clock frequency
/// This function configures the CS time and mode settings.
///
/// @param sysFreq System frequency to which adjust
/// the CS settings
// ============================================================================


PUBLIC VOID boot_EbcCsSetup(HAL_SYS_MEM_FREQ_T ebcFreq)
{
    UINT32 reg;

    // We need the Pll to do that. Check it is locked
    // Not on the FPGA ..
#ifdef FPGA
#ifdef FPGA_SLOW
    ebcFreq = HAL_SYS_MEM_FREQ_39M;
#else
    ebcFreq = HAL_SYS_MEM_FREQ_52M;
#endif // FPGA_SLOW
#endif // FPGA
// the top clk is 624M .  if used slow top clk ,the clk is 52M.
    boot_EbcWaitReady();
    switch (ebcFreq)
    {

        case HAL_SYS_MEM_FREQ_26M:
            reg = (SYS_STRL_CFG_NUM_BIT(1)|SYS_STRL_CFG_DENOM(24))&SYS_CTRL_CFG_MEM_BRIDGE_MASK;  //624/26=24
            break;

        case HAL_SYS_MEM_FREQ_39M:
            reg = (SYS_STRL_CFG_NUM_BIT(1)|SYS_STRL_CFG_DENOM(16))&SYS_CTRL_CFG_MEM_BRIDGE_MASK;
            break;

        case HAL_SYS_MEM_FREQ_52M:
            reg = (SYS_STRL_CFG_NUM_BIT(1)|SYS_STRL_CFG_DENOM(12))&SYS_CTRL_CFG_MEM_BRIDGE_MASK;
            break;

        case HAL_SYS_MEM_FREQ_78M:
            reg = (SYS_STRL_CFG_NUM_BIT(1)|SYS_STRL_CFG_DENOM(8))&SYS_CTRL_CFG_MEM_BRIDGE_MASK;
            break;

        case HAL_SYS_MEM_FREQ_89M:
            reg = (SYS_STRL_CFG_NUM_BIT(1)|SYS_STRL_CFG_DENOM(7))&SYS_CTRL_CFG_MEM_BRIDGE_MASK;
            break;

        case HAL_SYS_MEM_FREQ_104M:
            reg = (SYS_STRL_CFG_NUM_BIT(1)|SYS_STRL_CFG_DENOM(6))&SYS_CTRL_CFG_MEM_BRIDGE_MASK;
            break;

        case HAL_SYS_MEM_FREQ_113M:
            reg = (SYS_STRL_CFG_NUM_BIT(2)|SYS_STRL_CFG_DENOM(11))&SYS_CTRL_CFG_MEM_BRIDGE_MASK;
            break;

        case HAL_SYS_MEM_FREQ_125M:
            reg = (SYS_STRL_CFG_NUM_BIT(10)|SYS_STRL_CFG_DENOM(49))&SYS_CTRL_CFG_MEM_BRIDGE_MASK;
            break;

        case HAL_SYS_MEM_FREQ_139M:
            reg = (SYS_STRL_CFG_NUM_BIT(1)|SYS_STRL_CFG_DENOM(16))&SYS_CTRL_CFG_MEM_BRIDGE_MASK;
            break;

        case HAL_SYS_MEM_FREQ_156M:
            reg = (SYS_STRL_CFG_NUM_BIT(1)|SYS_STRL_CFG_DENOM(4))&SYS_CTRL_CFG_MEM_BRIDGE_MASK;
            break;

        case HAL_SYS_MEM_FREQ_178M:
            reg = (SYS_STRL_CFG_NUM_BIT(2)|SYS_STRL_CFG_DENOM(7))&SYS_CTRL_CFG_MEM_BRIDGE_MASK;
            break;

        case HAL_SYS_MEM_FREQ_208M:
            reg = (SYS_STRL_CFG_NUM_BIT(1)|SYS_STRL_CFG_DENOM(3))&SYS_CTRL_CFG_MEM_BRIDGE_MASK;
            break;

        case HAL_SYS_MEM_FREQ_250M:
            reg = (SYS_STRL_CFG_NUM_BIT(2)|SYS_STRL_CFG_DENOM(5))&SYS_CTRL_CFG_MEM_BRIDGE_MASK;
            break;

        case HAL_SYS_MEM_FREQ_312M:
            reg = (SYS_STRL_CFG_NUM_BIT(1)|SYS_STRL_CFG_DENOM(2))&SYS_CTRL_CFG_MEM_BRIDGE_MASK;
            break;

        default:
            MEM_INIT_ASSERT(FALSE, "Configuring the EBC clock with a bad frequency: %d",
                            (UINT32)ebcFreq);
            reg = 0;
            break;
    }

    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Sel_Clock &= (~SYS_CTRL_SOFT_SEL_MEM_BRIDGE);
    hwp_sysCtrl->CFG_PLL_MEM_BRIDGE_DIV = reg;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;

    // Hold that in the global var --> the membridge clock is set
    boot_BootSectorSetEbcFreq(ebcFreq);
}


#else

static UINT32 sysFreqToRealClkRegCfg(HAL_SYS_MEM_FREQ_T freq)
{
    switch(freq)
    {
#ifdef CHIP_DIE_8909 //TODO: clean up clk definitions on 8909
        case HAL_SYS_MEM_FREQ_26M:  return SYS_CTRL_PSRAM_FREQ_56MHZ;
        case HAL_SYS_MEM_FREQ_39M:  return SYS_CTRL_PSRAM_FREQ_56MHZ;
        case HAL_SYS_MEM_FREQ_52M:  return SYS_CTRL_PSRAM_FREQ_67MHZ;
        case HAL_SYS_MEM_FREQ_78M:  return SYS_CTRL_PSRAM_FREQ_75MHZ;
        case HAL_SYS_MEM_FREQ_89M:  return SYS_CTRL_PSRAM_FREQ_84MHZ;
        case HAL_SYS_MEM_FREQ_104M: return SYS_CTRL_PSRAM_FREQ_96MHZ;
        case HAL_SYS_MEM_FREQ_113M: return SYS_CTRL_PSRAM_FREQ_112MHZ;
        case HAL_SYS_MEM_FREQ_125M: return SYS_CTRL_PSRAM_FREQ_112MHZ;
        case HAL_SYS_MEM_FREQ_139M: return SYS_CTRL_PSRAM_FREQ_134MHZ;
        case HAL_SYS_MEM_FREQ_156M: return SYS_CTRL_PSRAM_FREQ_134MHZ;
        case HAL_SYS_MEM_FREQ_178M: return SYS_CTRL_PSRAM_FREQ_168MHZ;
        case HAL_SYS_MEM_FREQ_208M: return SYS_CTRL_PSRAM_FREQ_168MHZ;
        case HAL_SYS_MEM_FREQ_250M: return SYS_CTRL_PSRAM_FREQ_224MHZ;
        case HAL_SYS_MEM_FREQ_312M: return SYS_CTRL_PSRAM_FREQ_336MHZ;
#else
        case HAL_SYS_MEM_FREQ_26M:  return SYS_CTRL_MEM_FREQ_26M;
        case HAL_SYS_MEM_FREQ_39M:  return SYS_CTRL_MEM_FREQ_39M;
        case HAL_SYS_MEM_FREQ_52M:  return SYS_CTRL_MEM_FREQ_52M;
        case HAL_SYS_MEM_FREQ_78M:  return SYS_CTRL_MEM_FREQ_78M;
        case HAL_SYS_MEM_FREQ_89M:  return SYS_CTRL_MEM_FREQ_89M;
        case HAL_SYS_MEM_FREQ_104M: return SYS_CTRL_MEM_FREQ_104M;
        case HAL_SYS_MEM_FREQ_113M: return SYS_CTRL_MEM_FREQ_113M;
        case HAL_SYS_MEM_FREQ_125M: return SYS_CTRL_MEM_FREQ_125M;
        case HAL_SYS_MEM_FREQ_139M: return SYS_CTRL_MEM_FREQ_139M;
        case HAL_SYS_MEM_FREQ_156M: return SYS_CTRL_MEM_FREQ_156M;
        case HAL_SYS_MEM_FREQ_178M: return SYS_CTRL_MEM_FREQ_178M;
        case HAL_SYS_MEM_FREQ_208M: return SYS_CTRL_MEM_FREQ_208M;
        case HAL_SYS_MEM_FREQ_250M: return SYS_CTRL_MEM_FREQ_250M;
        case HAL_SYS_MEM_FREQ_312M: return SYS_CTRL_MEM_FREQ_312M;
#endif
        default:
        MEM_INIT_ASSERT(FALSE, "Configuring the EBC clock with a bad frequency: %d",
                        (UINT32)freq);
        return 0;
    }
}

// ============================================================================
// boot_EbcCsSetup
// ----------------------------------------------------------------------------
/// Configure the CS settings according to the system clock frequency
/// This function configures the CS time and mode settings.
///
/// @param sysFreq System frequency to which adjust
/// the CS settings
// ============================================================================
PUBLIC VOID boot_EbcCsSetup(HAL_SYS_MEM_FREQ_T ebcFreq)
{
    UINT32 reg;

    // We need the Pll to do that. Check it is locked
    // Not on the FPGA ..
#ifdef FPGA
#ifdef FPGA_SLOW
    ebcFreq = HAL_SYS_MEM_FREQ_39M;
#else
    ebcFreq = HAL_SYS_MEM_FREQ_52M;
#endif // FPGA_SLOW
#endif // FPGA

#ifdef FPGA
#ifdef FPGA_TOP_CLK_312M
    ebcFreq = HAL_SYS_MEM_FREQ_156M; // the actual clk is 52m
#endif
#ifdef USE_OPIDDR_PSRAM
// V6 board set ebc   clk to 78M.  K7 board set  ebc clk to 156M  ddr=39M
    ebcFreq = HAL_SYS_MEM_FREQ_156M;
#endif
#endif
    boot_EbcWaitReady();
    reg = sysFreqToRealClkRegCfg(ebcFreq);

#ifndef CHIP_HAS_AP
#if !defined(CHIP_DIE_8955) && !defined(CHIP_DIE_8909)
    reg |= SYS_CTRL_MEM_DDR_DISABLE;
#endif
#endif
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Cfg_Clk_Mem_Bridge = reg;
#ifdef __PRJ_WITH_SDRAM__
    boot_SysSetSdramTiming(HAL_SYS_MEM_FREQ_26M);
    boot_SysSetMPMCTiming(HAL_SYS_MEM_FREQ_26M);
    hwp_sysCtrl->Cfg_Clk_Sys = SYS_CTRL_SYS_FREQ_26M | SYS_CTRL_FORCE_DIV_UPDATE;
#endif
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;

    // Hold that in the global var --> the membridge clock is set
    boot_BootSectorSetEbcFreq(ebcFreq);
}

#endif
#ifdef RAM_WITH_DDR_MODE

PRIVATE VOID boot_EbcDDRmodeSetup(VOID)
{
    UINT32 reg = hwp_sysCtrl->Cfg_Clk_Mem_Bridge;

    hwp_memBridge->EBC_Ctrl = 0x11001;
    reg &= 0xfffe00bf;
#ifdef RAM_CLK_IS_104M
    reg |= SYS_CTRL_MEM_DDR_ENABLE |
           SYS_CTRL_MEM_DDR_DQSL_I(3) |
           SYS_CTRL_MEM_DDR_DQSU_I(3) |
           SYS_CTRL_MEM_DDR_DQSL_O(3) |
           SYS_CTRL_MEM_DDR_DQSU_O(3) |
           SYS_CTRL_MEM_DDR_PLL_OFF;
#else
    reg |= SYS_CTRL_MEM_DDR_ENABLE |
           SYS_CTRL_MEM_DDR_DQSL_I(1) |
           SYS_CTRL_MEM_DDR_DQSU_I(1) |
           SYS_CTRL_MEM_DDR_DQSL_O(1) |
           SYS_CTRL_MEM_DDR_DQSU_O(1) |
           SYS_CTRL_MEM_DDR_PLL_OFF_N;
#endif

    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Cfg_Clk_Mem_Bridge = reg;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
}
#endif


#ifndef CHIP_HAS_AP
#ifndef GALLITE_IS_8805
// =============================================================================
// boot_SpiFlashControllerInit
// -----------------------------------------------------------------------------
/// Initialize SPI flash controller if any.
// =============================================================================
PRIVATE VOID boot_SpiFlashControllerInit(VOID)
{
    //set the second 32/64 Mbit flash chip  enable
    //if no the second 32/64 Mbit flash chip, it will be cleared in memd_FlashOpen function.
#ifdef __PRJ_WITH_2_32MBIT_SPIFLSH__
    *(volatile unsigned int *)0xa1a2501c = 0x01;
#endif
#ifdef __PRJ_WITH_2_64MBIT_SPIFLSH__
    *(volatile unsigned int *)0xa1a2501c = 0x03;
#endif
#ifdef __PRJ_WITH_2_128MBIT_SPIFLSH__
    *(volatile unsigned int *)0xa1a2501c = 0x08;
#endif

    //change spi flash controller frequency divider.
#ifdef __PRJ_WITH_SPIFLSH__
    *(volatile unsigned int *)0xa1a25014 = 0x210;
#endif
}
#endif // !GALLITE_IS_8805
#endif // !CHIP_HAS_AP


#define ERAM_CHECK_PATTERN0  0xa569f0f0
#define ERAM_CHECK_PATTERN1  0x5a960f0f
#define ERAM_ADDR_NUM 6
#define ERAM_CHECK_ERROR_CODE 0xdead0000
#define CRAM_DIDR 0x40000

PRIVATE CONST UINT32 eram_end_addr[ERAM_ADDR_NUM] =
{0x200000, 0x400000, 0x800000, 0x1000000, 0x2000000, 0x4000000};

// =============================================================================
// boot_ExtRamSizeCheck
// -----------------------------------------------------------------------------
/// Check whether the ram size configuration is correct, and raise
/// an exception if not.
/// The ram size is checked by ram ID or actual size.
// =============================================================================
PRIVATE VOID boot_ExtRamSizeCheck(VOID)
{
#ifdef USE_OPIDDR_PSRAM
    return;
#endif
#if !defined(_FLASH_PROGRAMMER) && !defined(_T_UPGRADE_PROGRAMMER)
    REG16 *csBase;
    UINT16 index;

    mon_Event(RAM_SIZE >> 20);

#ifdef __PRJ_WITH_SDRAM__
    return;
#endif

#ifdef CHIP_HAS_AP
    csBase = (UINT16*)hwp_cs0;
#else
    csBase = (UINT16*)hwp_cs1;
#endif

    VOLATILE UINT32 *ERam_Base = (UINT32 *)csBase;

    UINT32 contentBackup;
    // Save the original RAM data (possibly some boot codes)
    contentBackup = *ERam_Base;

    *ERam_Base = ERAM_CHECK_PATTERN0;

    for (index = 0; index < ERAM_ADDR_NUM && eram_end_addr[index] < RAM_SIZE; index++)
    {
        if (*(VOLATILE UINT32 *)((UINT32)ERam_Base + eram_end_addr[index]) == ERAM_CHECK_PATTERN0)
        {
            *ERam_Base = ERAM_CHECK_PATTERN1;
            if (*(VOLATILE UINT32 *)((UINT32)ERam_Base + eram_end_addr[index]) == ERAM_CHECK_PATTERN1)
            {
                if (RAM_SIZE > eram_end_addr[index])
                {
                    mon_Event(ERAM_CHECK_ERROR_CODE | (eram_end_addr[index] >> 20)); //EXT ramsize error
                    asm("break 2");
                }
                else
                {
                    break;
                }
            }
        }
    }

    // Restore the original RAM data (possibly some boot codes)
    *ERam_Base = contentBackup;
#endif // !_FLASH_PROGRAMMER && !_T_UPGRADE_PROGRAMMER
}


// =============================================================================
//  boot_EbcRamIdValid
// -----------------------------------------------------------------------------
/// Check whether an external RAM ID is valid.
///
/// @param ramId the RAM ID to be checked.
// =============================================================================
#ifdef USE_OPIDDR_PSRAM
#else
PRIVATE BOOL boot_EbcRamIdValid(UINT32 ramId)
{
#ifdef USE_OPIDDR_PSRAM
    return 0;
#else
    // TODO: Could flash programmer or T-Card upgrade programmer always force
    //       to read the new RAM ID ? If yes, they can work even when the RAM
    //       ID enum list is changed.
#if defined(FORCE_READ_RAM_ID) || \
        defined(NO_RDA_PSRAM_ADAPTIVE) || \
        defined(CHIP_HAS_AP) || \
        (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    return FALSE;
#else
    return (ramId < RDA_PSRAM_QTY);
#endif
#endif
}
#endif

// =============================================================================
//  boot_EbcReadRamId
// -----------------------------------------------------------------------------
/// Read the RAM ID from the external RAM chipset.
// =============================================================================
PRIVATE UINT32 boot_EbcReadRamId(VOID)
{
#ifdef USE_OPIDDR_PSRAM
    UINT32 ramId = RDA_PSRAM_UNKNOWN;
    return ramId;

#else
#if defined(NO_RDA_PSRAM_ADAPTIVE) || \
        defined(CHIP_HAS_AP) || \
        (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)

    return RDA_PSRAM_DEFAULT;

#else // psram adaptive

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    if (boot_SysGetChipId(HAL_SYS_CHIP_PROD_ID) == HAL_SYS_CHIP_PROD_ID_8808)
    {
        return RDA_PSRAM_DEFAULT;
    }
#endif

    // Using the CR controlled method
    // Configure DIDR
    // CRAM_DIDR[15] : ROW length
    // CRAM_DIDR[14:11] : Device Version
    // CRAM_DIDR[10:8] : Device density
    // CRAM_DIDR[7:5] : CRAM Generation
    // CRAM_DIDR[4:0] : Vendor ID
    UINT16 index;
    UINT32 ramId = RDA_PSRAM_DEFAULT;
    REG16* csBase = (UINT16*)hwp_cs1;
    REG32* pCRE = &(hwp_memFifo->EBC_CRE);

    *pCRE = 1; // MEM_FIFO_CRE
    index = *(csBase + CRAM_DIDR);
    UINT32 flush __attribute__((unused)) = *pCRE;
    *pCRE = 0;

    if ((index & 0x1f) == 0xd)//aptic
    {
        ramId = RDA_PSRAM_APTIC;
    }
    else if ((index & 0x1f) == 0x9)//etron
    {
        ramId = RDA_PSRAM_ETRON;
    }
    else if ((index & 0x1f) == 0x6)//winbond
    {
        ramId = RDA_PSRAM_WINBOND;
    }
    else if ((index & 0x1f) == 0xa)//emc
    {
        ramId = RDA_PSRAM_EMC;
    }
    else
    {
        mon_Event(0x66660000|index);
        ramId =  RDA_PSRAM_UNKNOWN;
    }

    return ramId;

#endif // psram adaptive
#endif
}


#ifdef __PRJ_WITH_SDRAM__

VOID boot_time_delay(UINT32 time)
{
    UINT32 startTime;

    startTime = hwp_timer->HWTimer_CurVal;

    while((hwp_timer->HWTimer_CurVal - startTime) < (time * 16384 / 1000));

}

#define CONFIG_DRAM_INIT_DF   10
#define CONFIG_DRAM_START     0x80000000    /* 32bit, 32M Bytes */
#define CONFIG_DRAM_SIZE      0x01900000


#define SDRAM_MEM_BASE_ADDR  CONFIG_DRAM_START
#define SDRAM_MEM_SIZE       CONFIG_DRAM_SIZE

#define SDRAM_REG_BASE_ADDR  0xa1ae4000

//#pragma GCC push_options
//#pragma GCC optimize ("O0")

void dram_init(void)
{
    volatile int *addr;
    //int *addr;
    //volatile int i;
    //int optimize_multi = CONFIG_DRAM_INIT_DF;
    volatile int temp;
    //int temp;


    /// added
    //addr = (unsigned int *)(0xa1a000b0);
    //*addr = (0x0aab8180);
    ///////////


    //### disable mpmc to config
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR);
    *addr = 0;


    //### set little endian
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 8);
    *addr = 0;

    //### set clock control
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 3;

    //### set tDAL
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x40);
    *addr = 4;

    boot_time_delay(1); //1 ms

    ///for ddr
    ///set cas latency to 2, for chip select 4
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x104);
    //*addr = 0x203;
    ///set cas latency to 2,(for ddr, it is 0x100), ras latency to 4, write latency to 1
    //*addr = 0xa04;

    /// for 2Gb / 1Gb
    ///set cas latency to 3,(for ddr, it is 0x110), ras latency to 4, write latency to 1
    *addr = 0x504; //26MHz

    //*addr = 0x604; //for 78M~250M
    //*addr = 0x704;
    /////////////

    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x24);
    *addr = 0x400;

    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x28);
    *addr = 1;

    ////changed for ddr, 256Mb
    //addr = (int *)(SDRAM_REG_BASE_ADDR + 0x100);
    //*addr = 0x690;
    /// for 512Mb 32bit
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x100);
    *addr = 0x690;

    //### enable mpmc to config
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR);
    *addr = 1;

    boot_time_delay(1); //100 ms

    /// issue nop
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x183;

    boot_time_delay(1); //100 ms

    /// issue precharge all
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x103;

    boot_time_delay(1); //100 ms

    /////added for ddr
    //// issue nop
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x183;

    boot_time_delay(1); //100 ms

    // issue LD_extend_MODE, enable DLL
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x83;

    /// set extend mode
    ///banksel[1:0] <-> addr[11:10] = 2'b01
    addr = (unsigned int *)(SDRAM_MEM_BASE_ADDR + 0x800);
    temp = *addr;

    boot_time_delay(1); //100 ms

    /// issue nop
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x183;

    // issue LD_MODE, reset DLL
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x83;

    addr = (unsigned int *)(SDRAM_MEM_BASE_ADDR + 0x23000);
    temp = *addr;

    boot_time_delay(2); //200 ms

    /// issue PALL: precharge all
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x103;
    ////////////////////

    /// set refresh cycle
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x24);
    *addr = 1;

    /// issue nop
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x183;

    boot_time_delay(5); //500 ms

    ////refresh cycle
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x24);
    *addr = 0x60;

    // issue LD_MODE, not rest dll
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x83;
    /// set mode
    addr = (unsigned int *)(SDRAM_MEM_BASE_ADDR + 0x23000);
    //addr = (unsigned int *)(SDRAM_MEM_BASE_ADDR + 0xaaaa8);
    temp = *addr;

    boot_time_delay(2); //100 ms

    /// issue nop
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x183;

    boot_time_delay(1); //100 ms

    /// issue normal
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x3;

    boot_time_delay(50); //1000 ms

    ////sdram configration, buffer enable
    addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x100);
    *addr = 0x80690;

    boot_time_delay(50); //1000 ms

    ///check DyConfigReg and DyRasCasReg
    //addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x100);
    //temp = *addr;

    //i = 1000 * optimize_multi;
    //while(i--);

    //addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x104);
    //temp = *addr;

    //return 1;

}
//#pragma GCC pop_options


//this funcion is support the real DDR ram ,only  for chip of  ct8852 .  don't support by ct8851 sram.
PUBLIC VOID boot_SysSetSdramTiming(HAL_SYS_MEM_FREQ_T MPMCFreq)
{
    //volatile int *addr;

    ///for ddr
    ///set cas latency to 2, for chip select 4
    //addr = (unsigned int *)(SDRAM_REG_BASE_ADDR + 0x104);
    //*addr = 0x203;
    ///set cas latency to 2,(for ddr, it is 0x100), ras latency to 4, write latency to 1
    //*addr = 0xa04;

    /// for 2Gb / 1Gb
    ///set cas latency to 3,(for ddr, it is 0x110), ras latency to 4, write latency to 1
    //*addr = 0x504; //26MHz

    //*addr = 0x604; //for 78M~250M
    /////////////

    switch (MPMCFreq)
    {
        case HAL_SYS_MEM_FREQ_26M:
        case HAL_SYS_MEM_FREQ_39M:
        case HAL_SYS_MEM_FREQ_52M:
            *(volatile unsigned int *)(SDRAM_REG_BASE_ADDR + 0x104) = 0x504;
            break;

        case HAL_SYS_MEM_FREQ_78M:
        case HAL_SYS_MEM_FREQ_89M:
        case HAL_SYS_MEM_FREQ_104M:
        case HAL_SYS_MEM_FREQ_113M:
        case HAL_SYS_MEM_FREQ_125M:
        case HAL_SYS_MEM_FREQ_139M:
        case HAL_SYS_MEM_FREQ_156M:
        case HAL_SYS_MEM_FREQ_178M:
        case HAL_SYS_MEM_FREQ_208M:
        case HAL_SYS_MEM_FREQ_250M:
        case HAL_SYS_MEM_FREQ_312M:
            *(volatile unsigned int *)(SDRAM_REG_BASE_ADDR + 0x104) = 0x604;
            break;
        default:
            *(volatile unsigned int *)(SDRAM_REG_BASE_ADDR + 0x104) = 0x504;
            break;
    }
}

//this funcion is support the real DDR ram ,only  for chip of  ct8852 .  don't support by ct8851 sram.

PUBLIC VOID boot_SysSetMPMCTiming(HAL_SYS_MEM_FREQ_T MPMCFreq)
{
    //UINT32 reg;
    //UINT32 sysFreq;

    //hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;

    switch (MPMCFreq)
    {
        case HAL_SYS_MEM_FREQ_26M:
            hwp_sysCtrl->Cfg_Clk_Mpmc  = SYS_CTRL_MPMC_FREQ_26M
                                         | SYS_CTRL_MPMC_DELAY(14)
                                         | SYS_CTRL_MPMC_DELAY_POL
                                         | SYS_CTRL_MPMC_FEEDBACK(1)
                                         | SYS_CTRL_MPMC_FEEDBACK_POL_INV
                                         | SYS_CTRL_MPMC_SD_PLL_OFF_N
                                         | SYS_CTRL_MPMC_SD_DQSL_I(3)
                                         | SYS_CTRL_MPMC_SD_DQSU_I(3)
                                         | SYS_CTRL_MPMC_SD_DQSL_O(3)
                                         | SYS_CTRL_MPMC_SD_DQSU_O(3)
                                         | SYS_CTRL_MPMC_SD_OEN(3)
                                         | SYS_CTRL_MPMC_DQS_SINGLE_N;

            //sysFreq = SYS_CTRL_SYS_FREQ_26M;
            break;

        case HAL_SYS_MEM_FREQ_39M:
            hwp_sysCtrl->Cfg_Clk_Mpmc  = SYS_CTRL_MPMC_FREQ_39M
                                         | SYS_CTRL_MPMC_DELAY(14)
                                         | SYS_CTRL_MPMC_DELAY_POL
                                         | SYS_CTRL_MPMC_FEEDBACK(3)
                                         | SYS_CTRL_MPMC_FEEDBACK_POL_INV
                                         | SYS_CTRL_MPMC_SD_PLL_OFF_N
                                         | SYS_CTRL_MPMC_SD_DQSL_I(3)
                                         | SYS_CTRL_MPMC_SD_DQSU_I(3)
                                         | SYS_CTRL_MPMC_SD_DQSL_O(3)
                                         | SYS_CTRL_MPMC_SD_DQSU_O(3)
                                         | SYS_CTRL_MPMC_SD_OEN(3)
                                         | SYS_CTRL_MPMC_DQS_SINGLE_N;

            //sysFreq = SYS_CTRL_SYS_FREQ_39M;
            break;

        case HAL_SYS_MEM_FREQ_52M:
            hwp_sysCtrl->Cfg_Clk_Mpmc  = SYS_CTRL_MPMC_FREQ_52M
                                         | SYS_CTRL_MPMC_DELAY(14)
                                         | SYS_CTRL_MPMC_DELAY_POL
                                         | SYS_CTRL_MPMC_FEEDBACK(3)
                                         | SYS_CTRL_MPMC_FEEDBACK_POL_INV
                                         | SYS_CTRL_MPMC_SD_PLL_OFF_N
                                         | SYS_CTRL_MPMC_SD_DQSL_I(2)
                                         | SYS_CTRL_MPMC_SD_DQSU_I(2)
                                         | SYS_CTRL_MPMC_SD_DQSL_O(2)
                                         | SYS_CTRL_MPMC_SD_DQSU_O(2)
                                         | SYS_CTRL_MPMC_SD_OEN(2)
                                         | SYS_CTRL_MPMC_DQS_SINGLE_N;

            //sysFreq = SYS_CTRL_SYS_FREQ_52M;
            break;

        case HAL_SYS_MEM_FREQ_78M:
            hwp_sysCtrl->Cfg_Clk_Mpmc = SYS_CTRL_MPMC_FREQ_78M
                                        | SYS_CTRL_MPMC_DELAY(14)
                                        | SYS_CTRL_MPMC_DELAY_POL
                                        | SYS_CTRL_MPMC_FEEDBACK(3)
                                        | SYS_CTRL_MPMC_FEEDBACK_POL_INV
                                        | SYS_CTRL_MPMC_SD_PLL_OFF_N
                                        | SYS_CTRL_MPMC_SD_DQSL_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSL_O(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_O(2)
                                        | SYS_CTRL_MPMC_SD_OEN(2)
                                        | SYS_CTRL_MPMC_DQS_SINGLE_N;

            //sysFreq = SYS_CTRL_SYS_FREQ_78M;
            break;

        case HAL_SYS_MEM_FREQ_89M:
            hwp_sysCtrl->Cfg_Clk_Mpmc = SYS_CTRL_MPMC_FREQ_89M
                                        | SYS_CTRL_MPMC_DELAY(14)
                                        | SYS_CTRL_MPMC_DELAY_POL
                                        | SYS_CTRL_MPMC_FEEDBACK(3)
                                        | SYS_CTRL_MPMC_FEEDBACK_POL_INV
                                        | SYS_CTRL_MPMC_SD_PLL_OFF_N
                                        | SYS_CTRL_MPMC_SD_DQSL_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSL_O(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_O(2)
                                        | SYS_CTRL_MPMC_SD_OEN(2)
                                        | SYS_CTRL_MPMC_DQS_SINGLE_N;

            //sysFreq = SYS_CTRL_SYS_FREQ_89M;
            break;

        case HAL_SYS_MEM_FREQ_104M:
            hwp_sysCtrl->Cfg_Clk_Mpmc = SYS_CTRL_MPMC_FREQ_104M
                                        | SYS_CTRL_MPMC_DELAY(14)
                                        | SYS_CTRL_MPMC_DELAY_POL
                                        | SYS_CTRL_MPMC_FEEDBACK(3)
                                        | SYS_CTRL_MPMC_FEEDBACK_POL_INV
                                        | SYS_CTRL_MPMC_SD_PLL_OFF_N
                                        | SYS_CTRL_MPMC_SD_DQSL_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSL_O(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_O(2)
                                        | SYS_CTRL_MPMC_SD_OEN(2)
                                        | SYS_CTRL_MPMC_DQS_SINGLE_N;

            //sysFreq = SYS_CTRL_SYS_FREQ_104M;
            break;

        case HAL_SYS_MEM_FREQ_113M:
            hwp_sysCtrl->Cfg_Clk_Mpmc = SYS_CTRL_MPMC_FREQ_113M
                                        | SYS_CTRL_MPMC_DELAY(14)
                                        | SYS_CTRL_MPMC_DELAY_POL
                                        | SYS_CTRL_MPMC_FEEDBACK(3)
                                        | SYS_CTRL_MPMC_FEEDBACK_POL_INV
                                        | SYS_CTRL_MPMC_SD_PLL_OFF_N
                                        | SYS_CTRL_MPMC_SD_DQSL_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSL_O(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_O(2)
                                        | SYS_CTRL_MPMC_SD_OEN(2)
                                        | SYS_CTRL_MPMC_DQS_SINGLE_N;

            //sysFreq = SYS_CTRL_SYS_FREQ_113M;
            break;

        case HAL_SYS_MEM_FREQ_125M:
            hwp_sysCtrl->Cfg_Clk_Mpmc = SYS_CTRL_MPMC_FREQ_125M
                                        | SYS_CTRL_MPMC_DELAY(14)
                                        | SYS_CTRL_MPMC_DELAY_POL
                                        | SYS_CTRL_MPMC_FEEDBACK(3)
                                        | SYS_CTRL_MPMC_FEEDBACK_POL_INV
                                        | SYS_CTRL_MPMC_SD_PLL_OFF_N
                                        | SYS_CTRL_MPMC_SD_DQSL_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSL_O(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_O(2)
                                        | SYS_CTRL_MPMC_SD_OEN(2)
                                        | SYS_CTRL_MPMC_DQS_SINGLE_N;

            //sysFreq = SYS_CTRL_SYS_FREQ_125M;
            break;

        case HAL_SYS_MEM_FREQ_139M:
            hwp_sysCtrl->Cfg_Clk_Mpmc = SYS_CTRL_MPMC_FREQ_139M
                                        | SYS_CTRL_MPMC_DELAY(14)
                                        | SYS_CTRL_MPMC_DELAY_POL
                                        | SYS_CTRL_MPMC_FEEDBACK(3)
                                        | SYS_CTRL_MPMC_FEEDBACK_POL_INV
                                        | SYS_CTRL_MPMC_SD_PLL_OFF_N
                                        | SYS_CTRL_MPMC_SD_DQSL_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSL_O(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_O(2)
                                        | SYS_CTRL_MPMC_SD_OEN(2)
                                        | SYS_CTRL_MPMC_DQS_SINGLE_N;

            //sysFreq = SYS_CTRL_SYS_FREQ_139M;
            break;

        case HAL_SYS_MEM_FREQ_156M:
            hwp_sysCtrl->Cfg_Clk_Mpmc = SYS_CTRL_MPMC_FREQ_156M
                                        | SYS_CTRL_MPMC_DELAY(14)
                                        | SYS_CTRL_MPMC_DELAY_POL
                                        | SYS_CTRL_MPMC_FEEDBACK(3)
                                        | SYS_CTRL_MPMC_FEEDBACK_POL_INV
                                        | SYS_CTRL_MPMC_SD_PLL_OFF_N
                                        | SYS_CTRL_MPMC_SD_DQSL_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSL_O(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_O(2)
                                        | SYS_CTRL_MPMC_SD_OEN(2)
                                        | SYS_CTRL_MPMC_DQS_SINGLE_N;

            //sysFreq = SYS_CTRL_SYS_FREQ_156M;
            break;

        case HAL_SYS_MEM_FREQ_178M:
            hwp_sysCtrl->Cfg_Clk_Mpmc = SYS_CTRL_MPMC_FREQ_178M
                                        | SYS_CTRL_MPMC_DELAY(14)
                                        | SYS_CTRL_MPMC_DELAY_POL
                                        | SYS_CTRL_MPMC_FEEDBACK(3)
                                        | SYS_CTRL_MPMC_FEEDBACK_POL_INV
                                        | SYS_CTRL_MPMC_SD_PLL_OFF_N
                                        | SYS_CTRL_MPMC_SD_DQSL_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSL_O(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_O(2)
                                        | SYS_CTRL_MPMC_SD_OEN(2)
                                        | SYS_CTRL_MPMC_DQS_SINGLE_N;

            //sysFreq = SYS_CTRL_SYS_FREQ_178M;
            break;

        case HAL_SYS_MEM_FREQ_208M:
            hwp_sysCtrl->Cfg_Clk_Mpmc = SYS_CTRL_MPMC_FREQ_208M
                                        | SYS_CTRL_MPMC_DELAY(14)
                                        | SYS_CTRL_MPMC_DELAY_POL
                                        | SYS_CTRL_MPMC_FEEDBACK(3)
                                        | SYS_CTRL_MPMC_FEEDBACK_POL_INV
                                        | SYS_CTRL_MPMC_SD_PLL_OFF_N
                                        | SYS_CTRL_MPMC_SD_DQSL_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_I(2)
                                        | SYS_CTRL_MPMC_SD_DQSL_O(2)
                                        | SYS_CTRL_MPMC_SD_DQSU_O(2)
                                        | SYS_CTRL_MPMC_SD_OEN(2)
                                        | SYS_CTRL_MPMC_DQS_SINGLE_N;

            //sysFreq = SYS_CTRL_SYS_FREQ_208M;
            break;

        case HAL_SYS_MEM_FREQ_250M:
            hwp_sysCtrl->Cfg_Clk_Mpmc = SYS_CTRL_MPMC_FREQ_250M
                                        | SYS_CTRL_MPMC_DELAY(14)
                                        | SYS_CTRL_MPMC_DELAY_POL
                                        | SYS_CTRL_MPMC_FEEDBACK(14)
                                        | SYS_CTRL_MPMC_FEEDBACK_POL_INV
                                        | SYS_CTRL_MPMC_SD_PLL_OFF_N
                                        | SYS_CTRL_MPMC_SD_DQSL_I(0)
                                        | SYS_CTRL_MPMC_SD_DQSU_I(0)
                                        | SYS_CTRL_MPMC_SD_DQSL_O(0)
                                        | SYS_CTRL_MPMC_SD_DQSU_O(0)
                                        | SYS_CTRL_MPMC_SD_OEN(0)
                                        | SYS_CTRL_MPMC_DQS_SINGLE_N;

            //sysFreq = SYS_CTRL_SYS_FREQ_250M;
            break;

        case HAL_SYS_MEM_FREQ_312M:
            hwp_sysCtrl->Cfg_Clk_Mpmc = SYS_CTRL_MPMC_FREQ_312M
                                        | SYS_CTRL_MPMC_DELAY(14)
                                        | SYS_CTRL_MPMC_DELAY_POL
                                        | SYS_CTRL_MPMC_FEEDBACK(14)
                                        | SYS_CTRL_MPMC_FEEDBACK_POL_INV
                                        | SYS_CTRL_MPMC_SD_PLL_OFF_N
                                        | SYS_CTRL_MPMC_SD_DQSL_I(0)
                                        | SYS_CTRL_MPMC_SD_DQSU_I(0)
                                        | SYS_CTRL_MPMC_SD_DQSL_O(0)
                                        | SYS_CTRL_MPMC_SD_DQSU_O(0)
                                        | SYS_CTRL_MPMC_SD_OEN(0)
                                        | SYS_CTRL_MPMC_DQS_SINGLE_N;

            //sysFreq = SYS_CTRL_SYS_FREQ_312M;
            break;

        default:
            hwp_sysCtrl->Cfg_Clk_Mpmc = 0;
            //sysFreq = SYS_CTRL_SYS_FREQ_26M;
            asm("break 7");
            break;
    }

    //hwp_sysCtrl->Cfg_Clk_Mpmc = 0xfed10e0; // for 26_39M
    //hwp_sysCtrl->Cfg_Clk_Mpmc = 0xaa930e0; // for52_ 208M
    //hwp_sysCtrl->Cfg_Clk_Mpmc = 0x1e0e0; // for 250M
    //hwp_sysCtrl->Cfg_Clk_Mpmc = reg;

    //hwp_sysCtrl->Cfg_Clk_Sys = sysFreq | SYS_CTRL_FORCE_DIV_UPDATE;
    //hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
}

#endif // __PRJ_WITH_SDRAM__

#endif // !(CHIP_HAS_AP && RUN_IN_DDR)
#ifdef USE_OPIDDR_PSRAM
extern VOID HAL_FUNC_INTERNAL pmd_SetVcoreToDefaultVolt(VOID);
#endif
extern VOID hal_DisableUsbDp(VOID);

// =============================================================================
// boot_MemInit
// -----------------------------------------------------------------------------
/// Initialize flash memory and RAM memory.
///
/// @param bootSectorStructValid whether boot sector structure is valid.
// =============================================================================
PUBLIC VOID boot_MemInit(BOOL bootSectorStructValid)
{
    UINT32* src;
    UINT32* dst;
#if !(defined(CHIP_HAS_AP) && defined(RUN_IN_DDR))
    HAL_SYS_MEM_FREQ_T memClk = HAL_SYS_MEM_FREQ_78M;
    BOOL ramIdValid = FALSE;
    UINT8 ramId = 0;
#endif

#ifdef CHIP_HAS_AP
    // Copy booting data from load space to execution space.
    for (src = (UINT32*) &_boot_sys_sram_data_load_start,
            dst = (UINT32*)( &_boot_sys_sram_data_start);
            src < (UINT32*) &_boot_sys_sram_data_load_end;
            src++, dst++)
    {
        *dst = *src;
    }
#endif

    // Copy booting code & data from ROM space to internal SRAM.
    for (src = (UINT32*) &_boot_sys_sram_flash_start_location,
            dst = (UINT32*)( &_boot_sys_sram_start);
            src < (UINT32*) &_boot_sys_sram_flash_end_location;
            src++, dst++)
    {
        *dst = *src;
    }

    // Clear booting BSS in internal SRAM
    for (dst = (UINT32*) &_boot_sys_sram_bss_start;
            dst < (UINT32*) &_boot_sys_sram_bss_end;
            dst++)
    {
        *dst = 0;
    }
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
    boot_FlushDCache(TRUE);
#ifndef XCPU_CACHE_FLUSH_V2
    boot_InvalidDCache();
#endif
#endif
#if !(defined(CHIP_HAS_AP) && defined(RUN_IN_DDR))
    if (bootSectorStructValid)
    {
        if (g_bootSectorStructPtr->ebcConfigValidTag == BOOT_SECTOR_EBC_VALID_TAG &&
                g_bootSectorStructPtr->ebcExtConfigValidTag == BOOT_SECTOR_EBC_EXT_VALID_TAG)
        {
#ifdef USE_OPIDDR_PSRAM

#else
            switch (g_bootSectorStructPtr->ebcFreq)
            {
                case HAL_SYS_MEM_FREQ_52M:
                case HAL_SYS_MEM_FREQ_78M:
                case HAL_SYS_MEM_FREQ_104M:
                case HAL_SYS_MEM_FREQ_125M:
                case HAL_SYS_MEM_FREQ_139M:
                    memClk = g_bootSectorStructPtr->ebcFreq;
                    ramIdValid = boot_EbcRamIdValid(g_bootSectorStructPtr->ramId);
                    break;
                default:
                    memClk = HAL_SYS_MEM_FREQ_78M;
                    break;
            }
#endif
        }
    }
#endif

#ifdef CHIP_DIE_8955
    // Setup clk_sys and XTAL_DIV for consistent
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
#ifdef CHIP_XTAL_CLK_IS_52M
    hwp_sysCtrl->CFG_XTAL_DIV = SYS_CTRL_CFG_XTAL_DIV(1)|SYS_CTRL_CFG_XTAL_DIV_UPDATE;
    hwp_sysCtrl->Cfg_Clk_Sys = SYS_CTRL_SYS_FREQ_52M;
#else // !CHIP_XTAL_CLK_IS_52M
    hwp_sysCtrl->CFG_XTAL_DIV = SYS_CTRL_CFG_XTAL_DIV(2)|SYS_CTRL_CFG_XTAL_DIV_UPDATE;
    hwp_sysCtrl->Cfg_Clk_Sys = SYS_CTRL_SYS_FREQ_26M;
#endif // CHIP_XTAL_CLK_IS_52M
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
#endif // CHIP_DIE_8955

    // Setup system PLL first
    boot_SysSetupPLL();

    boot_UpdateIspiDivider();

#if !(defined(CHIP_HAS_AP) && defined(RUN_IN_DDR))
    // Setup EBC clock according to boot sector structure

#ifdef USE_OPIDDR_PSRAM
#else
    boot_EbcCsSetup(memClk);
#endif
    if (ramIdValid)
    {
        ramId = g_bootSectorStructPtr->ramId;
    }
    else
    {
        ramId = boot_EbcReadRamId();
        if (ramId == RDA_PSRAM_UNKNOWN)
        {
            ramId = RDA_PSRAM_APTIC;
        }
    }
    // Save the RAM ID for future reference (and for s/w upgrade
    // to a version with a different boot sector struct location)
#ifdef USE_OPIDDR_PSRAM
#else
    boot_BootSectorSetEbcRamId(ramId);
#endif
#ifdef RAM_CLK_IS_39M
    memClk = HAL_SYS_MEM_FREQ_39M;
#else
#ifdef RAM_CLK_IS_52M
    memClk = HAL_SYS_MEM_FREQ_52M;
#else
#ifdef RAM_CLK_IS_78M
    memClk = HAL_SYS_MEM_FREQ_78M;
#else
#ifdef RAM_CLK_IS_104M
    memClk = HAL_SYS_MEM_FREQ_104M;
#else
#ifdef RAM_CLK_IS_125M
    memClk = HAL_SYS_MEM_FREQ_125M;
#else
#ifdef RAM_CLK_IS_139M
    memClk = HAL_SYS_MEM_FREQ_139M;
#else
#ifdef RAM_CLK_IS_156M
    memClk = HAL_SYS_MEM_FREQ_156M;
#else
#ifdef RAM_CLK_IS_178M
    memClk = HAL_SYS_MEM_FREQ_178M;
#else
#ifdef RAM_CLK_IS_208M
    memClk = HAL_SYS_MEM_FREQ_208M;
#else
#ifdef RAM_CLK_IS_250M
    memClk = HAL_SYS_MEM_FREQ_250M;
#else
#ifdef RAM_CLK_IS_312M
    memClk = HAL_SYS_MEM_FREQ_312M;
#else
    memClk = HAL_SYS_MEM_FREQ_52M; // EBC use 52M clk in default.
#endif//RAM_CLK_IS_139M
#endif//RAM_CLK_IS_125M
#endif//RAM_CLK_IS_104M
#endif//RAM_CLK_IS_78M
#endif//RAM_CLK_IS_52M
#endif//RAM_CLK_IS_39M
#endif
#endif
#endif
#endif
#endif

    // Setup EBC clock again according to target configuration
#ifdef USE_OPIDDR_PSRAM
    //  hwp_psram8Ctrl->power_up = PSRAM8_CTRL_HW_POWER_PULSE;

    // while(!((hwp_psram8Ctrl->power_up & PSRAM8_CTRL_INIT_DONE)== PSRAM8_CTRL_INIT_DONE)) {};


    boot_EbcCsSetup(memClk);
#ifdef CHIP_DIE_8909 //TODO: clean up by using boot_EbcCsSetup
    hwp_psram8Ctrl->cre = PSRAM8_CTRL_CRE;
    hwp_psram8Ctrl->clk_ctrl = 0x02;
    hwp_psram8Ctrl->dqs_ctrl = 0x020000;
    hwp_psram8Ctrl->mr0 = PSRAM8_CTRL_MR0(0x0b);
    hwp_psram8Ctrl->power_up = PSRAM8_CTRL_HW_POWER_PULSE;
    while(!((hwp_psram8Ctrl->power_up & PSRAM8_CTRL_INIT_DONE_STATE)== PSRAM8_CTRL_INIT_DONE_STATE)) {};
    hwp_psram8Ctrl->mr0 = PSRAM8_CTRL_MR0(0x0b);
    hwp_psram8Ctrl->cre = 0x0;
    mon_Event(0xdd200003);
#endif
#if 0 // def RAM_CLK_IS_78M    // V6 board set sys clk to 78M.  K7 board set sys clk to 156M
    hwp_psram8Ctrl->clk_ctrl = 0x6;
    hwp_psram8Ctrl->dqs_ctrl = 0x60000;
#else
//#error please config ddr timing.
#endif
#else
    boot_EbcCsSetup(memClk);
#endif
#ifdef __PRJ_WITH_SDRAM__  // only for ct8852 chip. real ddr ram.
    dram_init();
#else
#ifndef _FLASH_PROGRAMMER
    hal_DisableUsbDp();
#endif
    // Load target RAM parameter to speed up the booting
    // NOTE: System should NOT access RAM during RAM opening process,
    //       otherwise exception will occur.
#ifdef USE_OPIDDR_PSRAM
    pmd_SetVcoreToDefaultVolt();
#endif
    memd_RamOpen(tgt_GetMemdRamConfig());
#ifdef USE_DDR_PSRAM_HIGH_CLK_312M
    // boot_EbcCsSetup(HAL_SYS_MEM_FREQ_312M);
    memd_AdjustRamClk(HAL_SYS_MEM_FREQ_312M,TRUE);
#endif

#endif

#endif // !(CHIP_HAS_AP && RUN_IN_DDR)

    // Copy booting code & data from ROM space to RAM.
    for (src = (UINT32*) &_boot_ram_flash_start_location,
            dst = (UINT32*)( &_boot_ram_start);
            src < (UINT32*) &_boot_ram_flash_end_location;
            src++, dst++)
    {
        *dst = *src;
    }

    // Clear booting BSS in External RAM
    for (dst = (UINT32*) &_boot_ram_bss_start;
            dst < (UINT32*) &_boot_ram_bss_end;
            dst++)
    {
        *dst = 0;
    }
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
    boot_FlushDCache(TRUE);
#ifndef XCPU_CACHE_FLUSH_V2
    boot_InvalidDCache();
#endif
#endif
#ifndef CHIP_HAS_AP
#ifndef GALLITE_IS_8805
    boot_SpiFlashControllerInit();
#endif
#endif

#if !(defined(CHIP_HAS_AP) && defined(RUN_IN_DDR))
    boot_ExtRamSizeCheck();
#endif

#ifndef CHIP_HAS_AP
    // Load target FLASH parameter to speed up the booting
    memd_FlashOpen(tgt_GetMemdFlashConfig());
#endif
}



