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
#include "sys_irq.h"
#include "cfg_regs.h"
#include "spi.h"

#include "hal_sys.h"
#include "hal_mem_map.h"
#ifdef CHIP_DIE_8909
#include "abb.h"
#include "pmuc.h"
#include "hal_pmu.h"
#include "hal_timers.h"
#endif

#include "boot_sys.h"

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// ============================================================================
// boot_SysSetupPLL
// ----------------------------------------------------------------------------
/// Setup PLL at early system boot time.
// ============================================================================
PUBLIC VOID HAL_BOOT_FUNC_INTERNAL __attribute__ ((nomips16)) boot_SysSetupPLL(VOID)
#ifdef CHIP_DIE_8909
{
    hal_PMU_Open();

    //1.switch clk_slow from rc26m to xtal52m
    hwp_sysCtrl->sel_clock |= SYS_CTRL_SEL_CLK_SLOW; //to fix
    //2. enable pll
    hwp_sysCtrl->pll_ctrl = SYS_CTRL_MCU_PLL_PU | SYS_CTRL_GSM_PLL_PU |  SYS_CTRL_NB_PLL_PU | SYS_CTRL_AU_PLL_PU; //enable pll
    //3. wait pll lock
    while((hwp_sysCtrl->pll_ctrl&SYS_CTRL_MCU_PLL_LOCKED) != SYS_CTRL_MCU_PLL_LOCKED); //wait pll locked
    //4. rand switch the freq
    hwp_sysCtrl->cfg_clk_sys = 2; //0:26M 1:39M 2:52M 3:78M 4:89M 5:104M 6:113M 7:125M 8:139M 9:156M 10:178M 11:208M 12:250M 13:312M
    hwp_sysCtrl->cfg_clk_bb = 2;
    hwp_sysCtrl->cfg_clk_voc = 2;
    //5. change to pll fast clk
    hwp_sysCtrl->sel_clock &= ~SYS_CTRL_SEL_CLK_SLOW; //to fix

    hwp_sysCtrl->sel_clock &= ~SYS_CTRL_SEL_CLK_SYS; //sel fast clk, low active
    hwp_sysCtrl->sel_clock &= ~SYS_CTRL_SEL_CLK_BB;
    hwp_sysCtrl->sel_clock &= ~SYS_CTRL_SEL_CLK_GSM_TCU;
    hwp_sysCtrl->sel_clock &= ~SYS_CTRL_SEL_CLK_NB_TCU;
    hwp_sysCtrl->sel_clock &= ~SYS_CTRL_SEL_CLK_VOC;
    //select 32k clock source.
    hwp_pmuc->xtal_divider_ctrl  |= PMUC_SEL_32K_SRC(2);	//Switch to 26m xtal generate 32k.
    hal_TimDelay(2);
    hwp_pmuc->xtal_ctrl_1 &= ~PMUC_PU_LPO32K;					 //close lpo 32k.

    if (hwp_sysCtrl->sel_clock & SYS_CTRL_SEL_CLK_SLOW)
        hwp_abb->osc_and_doubler_cfg &= (~ABB_PU_OSC26M);             //close rc 26m.

    //7. pu ready as int
    //    install_xcpu_normal_int(SYS_IRQ_PMUC, pu_ready_int);
    //    hwp_sysIrq->Mask_Set_Extra |= SYS_IRQ_SYS_IRQ_PMUC;

    //    hwp_sysCtrl->Sel_Clock &= ~SYS_CTRL_SEL_CLK_PSRAM;
    //    hwp_sysCtrl->Sel_Clock &= ~SYS_CTRL_SEL_CLK_SPIFLASH;

    //    hwp_sysCtrl->pll_ctrl |= SYS_CTRL_MCU_PLL_PU | SYS_CTRL_NB_PLL_PU | SYS_CTRL_GSM_PLL_PU;

    //8. autodisable xcpu/bcpu cache rams when not access cache rams, and disable when sleep
    hwp_sysCtrl->misc_ctrl |= SYS_CTRL_XCPU_CACHE_RAMS_DISABLE | SYS_CTRL_XCPU_AUTODISABLE_CACHE | SYS_CTRL_BCPU_CACHE_RAMS_DISABLE | SYS_CTRL_BCPU_AUTODISABLE_CACHE;

    return ;
}
#else
{

#if (CHIP_TCU_CLK == 26000000)
#define  TCUCLOCK  SYS_CTRL_TCU_13M_SEL_26M
#else
#define  TCUCLOCK  SYS_CTRL_TCU_13M_SEL_13M
#endif
    UINT32 register status = hal_SysEnterCriticalSection();
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
        hwp_sysCtrl->Sel_Clock &= ~(SYS_CTRL_SYS_SEL_FAST_SLOW|SYS_CTRL_BB_SEL_FAST_SLOW);   //Add by Lixp 0822
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
#ifndef CHIP_DIE_8955
                             | SYS_CTRL_BB_SEL_FAST_SLOW
                             | SYS_CTRL_MPMC_SYS_SAME_ENABLE
#endif
#endif
                             | TCUCLOCK
                             ;
#else // !HAL_SYS_SETUP_CLOCK_26M
    hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_RF
                             | SYS_CTRL_SYS_SEL_FAST_FAST
#ifndef CHIP_HAS_AP
                             | SYS_CTRL_BB_SEL_FAST_FAST
#ifndef CHIP_DIE_8955
                             | SYS_CTRL_MPMC_SYS_SAME_ENABLE
#else
                             | SYS_CTRL_SPIFLASH_SEL_FAST_FAST
                             | SYS_CTRL_VOC_SEL_FAST_FAST
                             | SYS_CTRL_MEM_BRIDGE_SEL_FAST_FAST
#endif
#endif
                             | TCUCLOCK
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
                             | SYS_CTRL_SPIFLASH_SEL_FAST_FAST
                             | SYS_CTRL_VOC_SEL_FAST_FAST
                             | SYS_CTRL_MEM_BRIDGE_SEL_FAST_FAST
#endif
#endif
                             | TCUCLOCK
                             ;

#endif // !FPGA
    // Insert delay
     HAL_SYS_LOOP_DELAY(10);
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SysExitCriticalSection(status);
}
#endif



// ============================================================================
// boot_SysShutdownPLL
// ----------------------------------------------------------------------------
/// Switch the system clock to RF clock (26M) and shutdown the PLL.
/// The FLASH and RAM access is considered invalid after switching.
/// So starting from this function, all the codes and the stack
/// should be in internal SRAM, unless the PLL is setup again.
// ============================================================================
PUBLIC VOID HAL_BOOT_FUNC_INTERNAL __attribute__ ((nomips16)) boot_SysShutdownPLL(VOID)
#ifdef CHIP_DIE_8909
{
    // Stop using PLLs - Switch to 26M clock
    hwp_sysCtrl->sel_clock |= SYS_CTRL_SEL_CLK_SYS;
    hwp_sysCtrl->sel_clock |= SYS_CTRL_SEL_CLK_BB;
    hwp_sysCtrl->sel_clock |= SYS_CTRL_SEL_CLK_GSM_TCU;
    hwp_sysCtrl->sel_clock |= SYS_CTRL_SEL_CLK_NB_TCU;

    hwp_sysCtrl->pll_ctrl &= ~SYS_CTRL_MCU_PLL_PU;
    hwp_sysCtrl->pll_ctrl &= ~ SYS_CTRL_NB_PLL_PU;
    hwp_sysCtrl->pll_ctrl &= ~ SYS_CTRL_GSM_PLL_PU ;

    return ;
}
#else
{

#if (CHIP_TCU_CLK == 26000000)
#define  TCUCLOCK  SYS_CTRL_TCU_13M_SEL_26M
#else
#define  TCUCLOCK  SYS_CTRL_TCU_13M_SEL_13M
#endif
    UINT32 register status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;

    // Select clock source to the RF clock
    hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_RF
                             | SYS_CTRL_SYS_SEL_FAST_SLOW
#ifndef CHIP_HAS_AP
                             | SYS_CTRL_BB_SEL_FAST_SLOW
#ifndef CHIP_DIE_8955
                             | SYS_CTRL_MPMC_SYS_SAME_ENABLE
#else
                             | SYS_CTRL_SPIFLASH_SEL_FAST_SLOW
                             | SYS_CTRL_VOC_SEL_FAST_SLOW
                             | SYS_CTRL_MEM_BRIDGE_SEL_FAST_SLOW
#endif
#endif
                             |TCUCLOCK
                             ;

    // Insert delay
     HAL_SYS_LOOP_DELAY(10);
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
#ifdef MODULE_LOCK_PLL

    hwp_sysCtrl->Sel_Clock |= SYS_CTRL_SYS_SEL_FAST_SLOW|SYS_CTRL_BB_SEL_FAST_SLOW;

    hwp_sysCtrl->Pll_Ctrl &= (~SYS_CTRL_PLL_ENABLE_ENABLE);

    //hwp_sysCtrl->Pll_Ctrl &= (~SYS_CTRL_PLL_ENABLE_ENABLE);
#endif

    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SysExitCriticalSection(status);
}
#endif


// =============================================================================
// boot_SysGetChipId
// -----------------------------------------------------------------------------
/// That function gives the chip ID the software is running on.
/// @param part Which part of the chip ID
/// @return The requested part of the chip ID.
// =============================================================================
PUBLIC UINT32 HAL_BOOT_FUNC_INTERNAL boot_SysGetChipId(HAL_SYS_CHIP_ID_T part)
{
    UINT32 chipId = hwp_configRegs->CHIP_ID;

    if (part == HAL_SYS_CHIP_PROD_ID)
    {
        chipId = GET_BITFIELD(chipId, CFG_REGS_PROD_ID);
    }
    else if (part == HAL_SYS_CHIP_BOND_ID)
    {
        chipId = GET_BITFIELD(chipId, CFG_REGS_BOND_ID);
    }
    else if (part == HAL_SYS_CHIP_METAL_ID)
    {
        chipId = GET_BITFIELD(chipId, CFG_REGS_METAL_ID);
    }
#ifdef CHIP_DIE_8955
    else if (part == HAL_SYS_CHIP_SUB_ID)
    {
        chipId = GET_BITFIELD(chipId, CFG_REGS_SUB_ID);
    }
#endif
    else // HAL_SYS_CHIP_ID_WHOLE
    {
        // returning the whole chip ID
    }

    return chipId;
}

// =============================================================================
// boot_SysGetChipId
// -----------------------------------------------------------------------------
/// That function gives the chip ID the software is running on.
/// @param part Which part of the chip ID
/// @return The requested part of the chip ID.
// =============================================================================
PUBLIC HAL_SYS_CHIP_METAL_ID_T boot_SysGetMetalId(VOID)
{
#ifdef CHIP_DIE_8955
    UINT32 chipId = hwp_configRegs->CHIP_ID;
    UINT32 metal = GET_BITFIELD(chipId, CFG_REGS_METAL_ID);

    switch (metal)
    {
    case 0:
        return HAL_SYS_CHIP_METAL_ID_U01;
    case 1:
    default:
        return HAL_SYS_CHIP_METAL_ID_U02;
    }
#endif

    return HAL_SYS_CHIP_METAL_ID_U00;
}

// =============================================================================
// boot_SysGetFreq
// -----------------------------------------------------------------------------
/// Get clk_sys from sys_ctrl registers.
/// @return Current clk_sys.
// =============================================================================
PUBLIC HAL_SYS_FREQ_T boot_SysGetFreq(VOID)
{
#ifdef CHIP_DIE_8955
    UINT32 Sel_Clock = hwp_sysCtrl->Sel_Clock;
    if (Sel_Clock & SYS_CTRL_SYS_SEL_FAST_SLOW)
    {
        if (Sel_Clock & SYS_CTRL_SLOW_SEL_RF_OSCILLATOR)
            return HAL_SYS_FREQ_32K;

        if (GET_BITFIELD(hwp_sysCtrl->CFG_XTAL_DIV, SYS_CTRL_CFG_XTAL_DIV) == 1)
            return HAL_SYS_FREQ_52M;

        // Though it is possible to set divider larger than 2, but we don't use it
        return HAL_SYS_FREQ_26M;
    }
    else
    {
        switch (GET_BITFIELD(hwp_sysCtrl->Cfg_Clk_Sys, SYS_CTRL_SYS_FREQ))
        {
            case SYS_CTRL_SYS_FREQ_26M: return HAL_SYS_FREQ_26M;
            case SYS_CTRL_SYS_FREQ_39M: return HAL_SYS_FREQ_39M;
            case SYS_CTRL_SYS_FREQ_52M: return HAL_SYS_FREQ_52M;
            case SYS_CTRL_SYS_FREQ_78M: return HAL_SYS_FREQ_78M;
            case SYS_CTRL_SYS_FREQ_89M: return HAL_SYS_FREQ_89M;
            case SYS_CTRL_SYS_FREQ_104M: return HAL_SYS_FREQ_104M;
            case SYS_CTRL_SYS_FREQ_113M: return HAL_SYS_FREQ_113M;
            case SYS_CTRL_SYS_FREQ_125M: return HAL_SYS_FREQ_125M;
            case SYS_CTRL_SYS_FREQ_139M: return HAL_SYS_FREQ_139M;
            case SYS_CTRL_SYS_FREQ_156M: return HAL_SYS_FREQ_156M;
            case SYS_CTRL_SYS_FREQ_178M: return HAL_SYS_FREQ_178M;
            case SYS_CTRL_SYS_FREQ_208M: return HAL_SYS_FREQ_208M;
            case SYS_CTRL_SYS_FREQ_250M: return HAL_SYS_FREQ_250M;
            case SYS_CTRL_SYS_FREQ_312M: return HAL_SYS_FREQ_312M;
            default: break;
        }
    }
#else
#ifndef CHIP_DIE_8909
    UINT32 Sel_Clock = hwp_sysCtrl->Sel_Clock;
    if (Sel_Clock & SYS_CTRL_SYS_SEL_FAST_SLOW)
    {
        if (Sel_Clock & SYS_CTRL_SLOW_SEL_RF_OSCILLATOR)
            return HAL_SYS_FREQ_32K;

        return HAL_SYS_FREQ_26M;
    }
    else
    {
        switch (GET_BITFIELD(hwp_sysCtrl->Cfg_Clk_Sys, SYS_CTRL_SYS_FREQ))
        {
            case SYS_CTRL_SYS_FREQ_26M: return HAL_SYS_FREQ_26M;
            case SYS_CTRL_SYS_FREQ_39M: return HAL_SYS_FREQ_39M;
            case SYS_CTRL_SYS_FREQ_52M: return HAL_SYS_FREQ_52M;
            case SYS_CTRL_SYS_FREQ_78M: return HAL_SYS_FREQ_78M;
            case SYS_CTRL_SYS_FREQ_89M: return HAL_SYS_FREQ_89M;
            case SYS_CTRL_SYS_FREQ_104M: return HAL_SYS_FREQ_104M;
            case SYS_CTRL_SYS_FREQ_113M: return HAL_SYS_FREQ_113M;
            case SYS_CTRL_SYS_FREQ_125M: return HAL_SYS_FREQ_125M;
            case SYS_CTRL_SYS_FREQ_139M: return HAL_SYS_FREQ_139M;
            case SYS_CTRL_SYS_FREQ_156M: return HAL_SYS_FREQ_156M;
            case SYS_CTRL_SYS_FREQ_178M: return HAL_SYS_FREQ_178M;
            case SYS_CTRL_SYS_FREQ_208M: return HAL_SYS_FREQ_208M;
            case SYS_CTRL_SYS_FREQ_250M: return HAL_SYS_FREQ_250M;
            case SYS_CTRL_SYS_FREQ_312M: return HAL_SYS_FREQ_312M;
            default: break;
        }
    }
#endif
#endif

    asm("break 1"); // impossible
    return HAL_SYS_FREQ_32K; // to make compiler happy
}

// =============================================================================
// boot_SysWaitMicrosecond
// -----------------------------------------------------------------------------
/// This function wait some time in microseconds.
/// It calculates CPU instruction cycles needed, depending on current system
/// frequency. For 312M, the max wait can be 0.9 second.
/// @param us Microseconds to wait.
// =============================================================================
PUBLIC VOID boot_SysWaitMicrosecond(UINT32 us)
{
#ifndef CHIP_DIE_8909
// 1 (sltu) + 1 (addiu) + 3 (btnez) = 5 cycles
#define WAIT_US_LOOP_CYCLE 5

    register UINT32 counter = ((boot_SysGetFreq() >> 16) * us)/((1000000*WAIT_US_LOOP_CYCLE) >> 16);

    asm volatile("move $7, %0\n\t"
                 "li $6, 0\n"
                 "_counter_check:\n\t"
                 "sltu $6, $7\n\t"
                 "addiu $6, 1\n\t"
                 "btnez _counter_check"
                 : /* no output */ : "r"(counter) : "$6", "$7");
#endif
}

PUBLIC VOID boot_UpdateIspiDivider(VOID)
{
#ifndef CHIP_DIE_8909
    HAL_SYS_FREQ_T sysFreq = boot_SysGetFreq();
    UINT32 divider = (sysFreq + (2*HAL_SYS_FREQ_13M - 1)) / (2*HAL_SYS_FREQ_13M);
    UINT32 cfg = hwp_spi3->cfg;

    divider = (divider == 0)? 0 : (divider - 1);
    cfg &= ~(SPI_CLOCK_DIVIDER_MASK | SPI_CLOCK_LIMITER);
    hwp_spi3->cfg = cfg | SPI_CLOCK_DIVIDER(divider);
#endif
}

PUBLIC VOLATILE UINT32* HAL_BOOT_DATA_INTERNAL g_bootSysCsAddress = (UINT32*)&hwp_sysIrq->SC;

// For library backward compatibility only. To be removed after all the libraries are updated.
PUBLIC VOLATILE UINT32* HAL_BOOT_DATA_INTERNAL g_halSysCsAddress = (UINT32*)&hwp_sysIrq->SC;

