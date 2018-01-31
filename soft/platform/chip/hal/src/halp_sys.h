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


#ifndef  _HALP_SYS_H_
#define  _HALP_SYS_H_

#include "cs_types.h"
#include "chip_id.h"

#include "global_macros.h"
#include "sys_ctrl.h"
#if (CHIP_HAS_ASYNC_BCPU)
#include "bb_ctrl.h"
#endif
#include "sys_irq.h"

#include "hal_sys.h"
#include "hal_uart.h"
#include "reg_alias.h"


// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================


// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
// HAL_SYS_FREQ_MAX_RESOURCE_NB
// -----------------------------------------------------------------------------
/// Maximum number of resources
// =============================================================================
#define HAL_SYS_FREQ_MAX_RESOURCE_NB  64


// =============================================================================
// HAL_SYS_FREQ_WIDTH
// -----------------------------------------------------------------------------
/// Width of the bitfield holding a frequency value in bits.
/// Not sure that scales pretty well...
// =============================================================================
#define HAL_SYS_FREQ_WIDTH  8



// default clock generation params
// PCM_CLOCKS_POLARITY : INVERT_PCM_BCK | INVERT_PCM_MCK
#define PCM_CLOCKS_POLARITY 0
// ANA_CLOCKS_POLARITY : INVERT_AU_ADC_CLK | INVERT_TX_FS_CLK
//  | INVERT_RX_FS_CLK | INVERT_PA_DAC_CLK | INVERT_AFC_DAC_CLK
#define ANA_CLOCKS_POLARITY ANA_ACCO_GPADC_CLK_POL
// RF_FS_SETTING : RX_FS_IS_13M, RX_FS_IS_6M5
#define RF_FS_SETTING RX_FS_IS_6M5
// AFC_DIVIDER_SETTING : n
#define AFC_DIVIDER_SETTING 8
#define GPADC_DIVIDER_SETTING   0x1F


#define PLL_CONFIG (SYS_CTRL_PLL_R(4-2) | SYS_CTRL_PLL_F(48-2) | SYS_CTRL_PLL_OD_DIV_BY_1)

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// HAL_SYS_FREQ_SCAL_USERS_T
// -----------------------------------------------------------------------------
/// That type lists all the possible entities (software / drivers / peripherals)
/// that need a specific minimum clock.
/// This is the internal type for HAL use only. Specific resource identifiers
/// are exported through the interface for use in upper layer or by upper
/// layers.
/// The list attempts to be exhaustive, but could contain useless fields
/// and lack some others. Don't forget to adjust #HAL_SYS_TENS_QTY consequently.
// =============================================================================
typedef enum
{
    HAL_SYS_FREQ_PWM, // 0
    HAL_SYS_FREQ_SIM,
    HAL_SYS_FREQ_SPI,
    HAL_SYS_FREQ_TRACE,
    HAL_SYS_FREQ_UART,
    HAL_SYS_FREQ_I2C, // 5
    HAL_SYS_FREQ_RF_SPI,
    HAL_SYS_FREQ_DEBUG_HOST,
    HAL_SYS_FREQ_AIF,
    HAL_SYS_FREQ_AUDIO,
    HAL_SYS_FREQ_HAL_NULL_0, // 10
    HAL_SYS_FREQ_SDMMC,
    HAL_SYS_FREQ_SDMMC2,
    HAL_SYS_FREQ_LCD,
    HAL_SYS_FREQ_CLK_OUT,
    HAL_SYS_FREQ_CAMCLK, // 15
    HAL_SYS_FREQ_USB,
    HAL_SYS_FREQ_DMA,
    HAL_SYS_FREQ_VOC,
    HAL_SYS_FREQ_AP,
    HAL_SYS_FREQ_BTCPU, // 20
    // !!! Add new sys users above !!!
    HAL_SYS_FREQ_SYS_USER_END,
    HAL_SYS_FREQ_SYS_LAST_USER = HAL_SYS_FREQ_SYS_USER_END-1,

// The last sys user ID should be less than the first platform user
// ID, which is 25 in hal_sys.h at present.
// This is checked in hal_SysSetupSystemClock, PROTECTED function
// called only once during hal_Open().
    HAL_SYS_USER_START = HAL_SYS_FREQ_PLATFORM_FIRST_USER,

    HAL_SYS_TOTAL_USERS_QTY = HAL_SYS_FREQ_USER_QTY

} HAL_SYS_FREQ_SCAL_USERS_T;


// =============================================================================
// HAL_SYS_FREQ_SCAL_FREQ_T
// -----------------------------------------------------------------------------
/// This list the possible value for the frequency required by the frequence
/// scaling users. This type differs from the one in the interface because
/// the value here have to be stored into the #HAL_SYS_FREQ_WIDTH (8 bits) wide
/// field. It is only used in the managing of the frequency scaling.
// =============================================================================
typedef enum
{
    HAL_SYS_FREQ_SCAL_32K   = 0,
    HAL_SYS_FREQ_SCAL_13M   = 1,
    HAL_SYS_FREQ_SCAL_26M   = 2,
    HAL_SYS_FREQ_SCAL_39M   = 3,
    HAL_SYS_FREQ_SCAL_52M   = 4,
    HAL_SYS_FREQ_SCAL_78M   = 5,
    HAL_SYS_FREQ_SCAL_89M  = 6,
    HAL_SYS_FREQ_SCAL_104M  = 7,
    HAL_SYS_FREQ_SCAL_113M  = 8,
    HAL_SYS_FREQ_SCAL_125M  = 9,
    HAL_SYS_FREQ_SCAL_139M  = 10,
    HAL_SYS_FREQ_SCAL_156M  = 11,
    HAL_SYS_FREQ_SCAL_178M  = 12,
    HAL_SYS_FREQ_SCAL_208M  = 13,
    HAL_SYS_FREQ_SCAL_250M  = 14,
    HAL_SYS_FREQ_SCAL_312M  = 15,
    HAL_SYS_FREQ_SCAL_QTY

} HAL_SYS_FREQ_SCAL_FREQ_T;


// =============================================================================
//  EXPORTED VARIABLES
// =============================================================================

// ============================================================================
// g_halSysSystemFreq
// ----------------------------------------------------------------------------
///  Global var to have a view of the system frequency
// ============================================================================
PROTECTED EXPORT HAL_SYS_FREQ_T g_halSysSystemFreq;


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_SysSetupSystemClock
// -----------------------------------------------------------------------------
/// Configure the initial settings of the system clock.
/// This function is to be called only by hal_open.
/// @param fastClockSetup Initial System Clock.
// =============================================================================
PROTECTED VOID hal_SysSetupSystemClock(UINT32 fastClockSetup);



// =============================================================================
// hal_SysXcpuSleep
// -----------------------------------------------------------------------------
/// Put the XCPU to sleep
/// @param wakeUpMask Set the WakeUp mask, a bitfield where a bit to '1'
/// means that the corresponding IRQ can wake up the CPU.
// =============================================================================
INLINE VOID hal_SysXcpuSleep(VOID)
{
#ifdef CHIP_DIE_8909
    hwp_sysIrq->Cpu_Sleep = SYS_IRQ_SLEEP;
    // flush the write buffer to ensure we sleep before exiting this function
    UINT32 clkSysDisable __attribute__((unused)) = hwp_sysIrq->Cpu_Sleep;
#else
#ifdef __mips16
    asm volatile(".align 4\n"
                 "li $6, %0\n"
                 "sw $6, 0(%1)\n"
                 "lw $6, 0(%1)\n"
                 "nop;nop;nop;nop;nop\n"
                 : /* no output */ : "i"(SYS_IRQ_SLEEP), "r"(&(hwp_sysIrq->Cpu_Sleep)) : "$6");
#else
    asm volatile(".align 4\n"
                 "li $6, %0\n"
                 "sw $6, 0(%1)\n"
                 "lw $6, 0(%1)\n"
                 "nop\n"
                 : /* no output */ : "i"(SYS_IRQ_SLEEP), "r"(&(hwp_sysIrq->Cpu_Sleep)) : "$6");
#endif
#endif // !CHIP_DIE_8909
}

// =============================================================================
// hal_SysEnablePLL
// -----------------------------------------------------------------------------
/// Enable PLL
// =============================================================================
PROTECTED VOID hal_SysEnablePLL(VOID);

// =============================================================================
// hal_SysDisablePLL
// -----------------------------------------------------------------------------
/// Disable PLL and switch on low clock
// =============================================================================
PROTECTED VOID hal_SysDisablePLL(VOID);

// =============================================================================
// hal_SysGetPLLLock
// -----------------------------------------------------------------------------
/// Return PLL lock
/// @return PLL status
///         If \c TRUE, PLL locked.
///         If \c FALSE, PLL not locked.
// =============================================================================
PROTECTED BOOL hal_SysGetPLLLock(VOID);


#ifdef CHIP_HAS_AP
// =============================================================================
// hal_SysApHostEnable
// -----------------------------------------------------------------------------
/// Enable or disable usb host.
/// This is useful to inform hal_SysProcessHostMonitor() that the AP Host functions
/// must be called to process the host commands.
/// @param enable \c TRUE to enable, \c FALSE to disable.
// =============================================================================
PROTECTED VOID hal_SysApHostEnable(BOOL enable);


// =============================================================================
// hal_SysEnableApClock
// -----------------------------------------------------------------------------
/// Enable or disable AP clock.
/// @param enable \c TRUE to enable, \c FALSE to disable.
// =============================================================================
PROTECTED VOID hal_SysEnableApClock(BOOL enable);


// =============================================================================
// hal_SysEnableApIrq
// -----------------------------------------------------------------------------
/// Enable or disable AP irq.
/// @param enable \c TRUE to enable, \c FALSE to disable.
// =============================================================================
PROTECTED VOID hal_SysEnableApIrq(BOOL enable);


// =============================================================================
// hal_SysApIrqActive
// -----------------------------------------------------------------------------
/// Check AP irq status.
/// @return TRUE if AP irq is active; FALSE otherwise
// =============================================================================
PROTECTED BOOL hal_SysApIrqActive(VOID);


#else // !CHIP_HAS_AP
// =============================================================================
// hal_SysSetupClkUart
// -----------------------------------------------------------------------------
/// Setup the uart clock
/// Be careful not to use it with a uart2 which doesn't exist here (yet)
/// or any improper parameter.
/// @param Uart_id Id of the uart whose clock we want to setup
/// @param divider Divider of the clock, from 2 to 1025
// =============================================================================
INLINE VOID hal_SysSetupClkUart(HAL_UART_ID_T uartId, UINT16 divider)
{
    hwp_sysCtrl->Cfg_Clk_Uart[uartId] = divider-2;
}


// =============================================================================
// hal_SysSetupClkPwm
// -----------------------------------------------------------------------------
/// Setup the PWM clock
/// The PWM clock is got by dividing the system clock
/// @param divider The divider used
// =============================================================================
INLINE VOID hal_SysSetupClkPwm(UINT8 divider)
{
#ifndef CHIP_DIE_8909
    hwp_sysCtrl->Cfg_Clk_PWM = divider;
#endif
}
#endif // !CHIP_HAS_AP


// =============================================================================
// hal_SysUsbHostEnable
// -----------------------------------------------------------------------------
/// Enable or disable usb host.
/// This is useful to inform hal_SysProcessIdle() that the USB Host functions
/// must be called to process the host commands.
/// @param enable \c TRUE to enable, \c FALSE to disable.
// =============================================================================
PROTECTED VOID hal_SysUsbHostEnable(BOOL enable);


// =============================================================================
// hal_SysClkOutInit
// -----------------------------------------------------------------------------
/// Initialize the clock-out to OFF state
// =============================================================================
PROTECTED VOID hal_SysClkInit(VOID);


#endif //  HAL_SYS_H


