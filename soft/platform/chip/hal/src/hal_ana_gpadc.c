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



#include "chip_id.h"
#include "cs_types.h"

#include "global_macros.h"
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
#include "gpadc.h"
#else // 8808 or later
#ifdef CHIP_HAS_AP
#include "cfg_regs.h"
#else
#include "pwm.h"
#endif
#endif // 8808 or later

#include "hal_sys.h"
#include "hal_ana_gpadc.h"
#include "hal_gpio.h"

#include "halp_ana_gpadc.h"
#include "halp_sys.h"
#include "halp_debug.h"
#include "halp_calib.h"

#include "calib_m.h"
#include "pmd_m.h"
#include "hal_pwm.h"


#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
BOOL pmd_TsdTscEnabled(VOID);
VOID pmd_TsdEnableTsc(BOOL enable);
#endif


#define GPADC_FAST_CLOSE        1

#define HAL_GPADC_TIMEOUT       (50 MS_WAITING)

// Battery voltage = gpadc voltage * 3
#define HAL_ANA_GPADC_MVOLT_A   1133
#define HAL_ANA_GPADC_MVOLT_B   1400


PRIVATE INT32 g_halGpadcSlope = 0;
PRIVATE INT32 g_halGpadcIntcpt = 0;
PRIVATE BOOL  g_halGpadcCalibrated = FALSE;

PRIVATE UINT32 g_halGpadcCalibHigh = 0;
PRIVATE UINT32 g_halGpadcCalibLow = 0;

#ifndef FPGA
#ifdef CHIP_HAS_AP
PRIVATE UINT32 g_halGpadcWakeupTime = 0;
#endif

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
PRIVATE UINT16 g_halGpadcLastVal[HAL_ANA_GPADC_CHAN_QTY] =
{
    HAL_ANA_GPADC_BAD_VALUE,
    HAL_ANA_GPADC_BAD_VALUE,
    HAL_ANA_GPADC_BAD_VALUE,
    HAL_ANA_GPADC_BAD_VALUE,
};
#endif

// using HAL_ANA_GPADC_ATP_QTY value for disabled channels
// (so min atp is HAL_ANA_GPADC_ATP_QTY when all disabled!)
PRIVATE UINT8 g_halGpadcAtpConfig[HAL_ANA_GPADC_CHAN_QTY] =
{
    HAL_ANA_GPADC_ATP_QTY,
    HAL_ANA_GPADC_ATP_QTY,
    HAL_ANA_GPADC_ATP_QTY,
    HAL_ANA_GPADC_ATP_QTY,
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE)
    HAL_ANA_GPADC_ATP_QTY,
    HAL_ANA_GPADC_ATP_QTY,
    HAL_ANA_GPADC_ATP_QTY,
    HAL_ANA_GPADC_ATP_QTY,
#endif
};
#endif // !FPGA


#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE) && !defined(FPGA)
// =============================================================================
// hal_AnaGpadcPowerOn
// -----------------------------------------------------------------------------
/// Power on the GPADC hardware.
// =============================================================================
PRIVATE BOOL hal_AnaGpadcPowerOn(VOID)
{
    UINT32 status = hal_SysEnterCriticalSection();
    // For async reset: first clear reset, second enable clock
    // Clear GPADC reset
    hwp_sysCtrl->Sys_Rst_Clr = SYS_CTRL_CLR_RST_GPADC;
    // Enable GPADC clock
    hwp_sysCtrl->Clk_Sys_Enable = SYS_CTRL_ENABLE_SYS_GPADC;
    hal_SysExitCriticalSection(status);

    // Setup s/w power control mode
    hwp_gpadc->ctrl = GPADC_PD_MODE_SW;
    // Power on ABB GPADC
    hwp_gpadc->cmd_clr = GPADC_GPADC_PD;
    // Clear ABB GPADC reset
    hwp_gpadc->cmd_clr = GPADC_GPADC_RESET;
    // Clear previous IRQs
    hwp_gpadc->irq_clr = GPADC_THRSHD0_IRQ|GPADC_THRSHD1_IRQ|GPADC_EOC;

    return TRUE;
}


// =============================================================================
// hal_AnaGpadcPowerOff
// -----------------------------------------------------------------------------
/// Power off the GPADC hardware.
// =============================================================================
PRIVATE VOID hal_AnaGpadcPowerOff(VOID)
{
#if (GPADC_FAST_CLOSE == 0)
    // Clear previous IRQs
    hwp_gpadc->irq_clr = GPADC_THRSHD0_IRQ|GPADC_THRSHD1_IRQ|GPADC_EOC;
    // Reset ABB GPADC
    hwp_gpadc->cmd_set = GPADC_GPADC_RESET;
    // Setup s/w power control mode
    hwp_gpadc->ctrl = GPADC_PD_MODE_SW;
    // Power off ABB GPADC
    hwp_gpadc->cmd_set = GPADC_GPADC_PD;
#endif // (GPADC_FAST_CLOSE == 0)

    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    // Disable GPADC clock
    hwp_sysCtrl->Clk_Sys_Disable = SYS_CTRL_DISABLE_SYS_GPADC;
    // Reset GPADC
    hwp_sysCtrl->Sys_Rst_Set = SYS_CTRL_SET_RST_GPADC;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SysExitCriticalSection(status);
}
#endif // gallite && !FPGA


#ifndef FPGA
// =============================================================================
// hal_AnaGpadcMinAtp
// -----------------------------------------------------------------------------
/// @param pCh Pointer to store the GPADC channel with minimum Atp value.
/// @return minimum Atp value for configured channels,
/// #HAL_ANA_GPADC_ATP_QTY when all chnnels are closed.
///
/// Use the global configuration variable \c g_halGpadcAtpConfig.
// =============================================================================
PRIVATE UINT8 hal_AnaGpadcMinAtp(HAL_ANA_GPADC_CHAN_T* pCh)
{
    HAL_ANA_GPADC_CHAN_T ch;
    UINT8 minAtp = HAL_ANA_GPADC_ATP_QTY;

    for (ch=HAL_ANA_GPADC_CHAN_0; ch<HAL_ANA_GPADC_CHAN_QTY; ch++)
    {
        if (g_halGpadcAtpConfig[ch] < minAtp)
        {
            minAtp = g_halGpadcAtpConfig[ch];
            if (pCh != NULL)
            {
                *pCh = ch;
            }
        }
    }

    return minAtp;
}
#endif


// =============================================================================
// hal_AnaGpadcGpadc2Volt
// -----------------------------------------------------------------------------
/// Convert the value measured by the GPADC into mV, using the calibration
/// parameters.
///
/// @param gpadcVal Value read from the GPADC hardware module.
/// @return The corresponding measured tension, in mV.
// =============================================================================
PUBLIC HAL_ANA_GPADC_MV_T hal_AnaGpadcGpadc2Volt(UINT16 gpadcVal)
{
    INT32 y1, y2, x1, x2;
    INT32 voltage;
    HAL_ANA_GPADC_MV_T rVoltage;
#if defined (FPGA) || defined(EFFUSE_NOT_BE_CALIBED)
    return 4000;
#endif
    if (gpadcVal == HAL_ANA_GPADC_BAD_VALUE)
    {
        // Bad values from the GPADC are still Bad Values
        // for the voltage-speaking user.
        return HAL_ANA_GPADC_BAD_VALUE;
    }
    else
    {
        if (!g_halGpadcCalibrated)
        {
            y1 = HAL_ANA_GPADC_MVOLT_A*1000;
            y2 = HAL_ANA_GPADC_MVOLT_B*1000;

#if defined(GALLITE_IS_8805)
            x1 = g_halCalibration->bb->gpadc.sensorGainA;
            x2 = g_halCalibration->bb->gpadc.sensorGainB;
#else // 8806 or later
#if defined(GALLITE_IS_8806)
            if((pmd_GetBatteryGpadcChannel() == HAL_ANA_GPADC_CHAN_1) && g_halGpadcCalibHigh)
#else // 8808 or later
            if((pmd_GetBatteryGpadcChannel() == HAL_ANA_GPADC_CHAN_7) && g_halGpadcCalibHigh)
#endif
            {
                x1 = g_halGpadcCalibLow;
                x2 = g_halGpadcCalibHigh;
            }
            else
            {
                x1 = g_halCalibration->bb->gpadc.sensorGainA;
                x2 = g_halCalibration->bb->gpadc.sensorGainB;
            }
#endif // !GALLITE_IS_8805

            HAL_ASSERT(x1 < x2, "Invalid GPADC calib value: %d, %d", x1, x2);

            g_halGpadcSlope = (y2-y1)/(x2-x1);
            g_halGpadcIntcpt = ((y1*x2)-(x1*y2))/((x2-x1)*1000);
            g_halGpadcCalibrated = TRUE;
        }

        voltage = (((g_halGpadcSlope*gpadcVal)/1000)+(g_halGpadcIntcpt));
        if (voltage < 0)
        {
            rVoltage = 0;   // Should never be negative
        }
        else
        {
            rVoltage = (HAL_ANA_GPADC_MV_T)voltage;
        }
        return rVoltage;
    }
}


#ifndef FPGA
// =============================================================================
// hal_AnaGpadcDisable
// -----------------------------------------------------------------------------
/// Really close the GPADC hardware and stop periodic measurements.
/// parameters.
// =============================================================================
PRIVATE VOID hal_AnaGpadcDisable(VOID)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)

#if (GPADC_FAST_CLOSE == 0)
    // Close the GPADC module ()
    // Check if already enabled
    if (hwp_gpadc->ctrl & (GPADC_CH0_EN|GPADC_CH1_EN|GPADC_CH2_EN|GPADC_CH3_EN))
    {
        while (hwp_gpadc->status & GPADC_BUSY);
    }

    // Disable all measurement
    hwp_gpadc->ctrl = 0;
    // Wait for the GPADC to be ready
    while (hwp_gpadc->status & GPADC_BUSY);
    // Clear EOC status bit
    hwp_gpadc->irq_clr = GPADC_EOC;
#endif // (GPADC_FAST_CLOSE == 0)
    // Power off ABB GPADC
    hal_AnaGpadcPowerOff();

#else // 8808 or later

    HAL_ANA_GPADC_CHAN_T ch;

    // Disable all the configured channels
    for (ch = HAL_ANA_GPADC_CHAN_0; ch < HAL_ANA_GPADC_CHAN_QTY; ch++)
    {
        if (g_halGpadcAtpConfig[ch] != HAL_ANA_GPADC_ATP_QTY)
        {
#ifdef CHIP_HAS_AP
            hwp_configRegs->GPADC_DATA_CH[ch] = 0;
#elif (CHIP_ASIC_ID != CHIP_ASIC_ID_8808) && (CHIP_ASIC_ID != CHIP_ASIC_ID_8809)
            // 8809P or later feature phones
            hwp_pwm->GPADC_DATA_CH[ch] = 0;
#endif
        }
    }

    // Disable PMU GPADC module
    pmd_GpadcDisable();

#endif // 8808 or later
}


// =============================================================================
// hal_AnaGpadcEnable
// -----------------------------------------------------------------------------
/// Really open the GPADC hardware and start periodic measurements.
/// If no channels are enabled, it will call #hal_AnaGpadcDisable to ensure
/// the GPADC hardware is disabled.
///
/// Use the global configuration variable \c g_halGpadcConfig.
// =============================================================================
PRIVATE VOID hal_AnaGpadcEnable(VOID)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)

    // If already enabled, the driver handle that situtation by
    // becoming blocking as long as the GPADC is not ready for the new
    // setting. (few 26Mhz clock cycles)
    UINT32 gpadcControl = 0;
    UINT8 minAtp = hal_AnaGpadcMinAtp(NULL);

    // Only open the GPADC if at least one channel is required.
    if (minAtp != HAL_ANA_GPADC_ATP_QTY)
    {
        // If a channel is already enabled, wait for the fitted time
        // to write the register.
        if (hwp_gpadc->ctrl & (GPADC_CH0_EN|GPADC_CH1_EN|GPADC_CH2_EN|GPADC_CH3_EN))
        {
            while(hwp_gpadc->status & GPADC_BUSY);
        }
        else
        {
            // Only needed for the first time
            // Power on ABB GPADC.
            hal_AnaGpadcPowerOn();
        }

        // Because hardware will actually change it's ATP after the current perriod
        // is elapsed, switching from slow to fast will actually be long:
        // for example switching from GPADC_ATP_2S to GPADC_ATP_1MS will take 2s
        // Limit the atp to 10ms to hide some of this effect
        /// @todo: put this limit in HAL TGT config
        if (minAtp > HAL_ANA_GPADC_ATP_10MS)
        {
            minAtp = HAL_ANA_GPADC_ATP_122US; //HAL_ANA_GPADC_ATP_10MS;
        }

        // Configure the GPADC module.
        // Hardware ATP has a strange order, reorder it there
        switch (minAtp)
        {
            case HAL_ANA_GPADC_ATP_122US :
                gpadcControl |= GPADC_ATP_122US;
                break;
            case HAL_ANA_GPADC_ATP_1MS :
                gpadcControl |= GPADC_ATP_1MS;
                break;
            case HAL_ANA_GPADC_ATP_10MS :
                gpadcControl |= GPADC_ATP_10MS;
                break;
            case HAL_ANA_GPADC_ATP_100MS :
                gpadcControl |= GPADC_ATP_100MS;
                break;
            case HAL_ANA_GPADC_ATP_250MS :
                gpadcControl |= GPADC_ATP_250MS;
                break;
            case HAL_ANA_GPADC_ATP_500MS :
                gpadcControl |= GPADC_ATP_500MS;
                break;
            case HAL_ANA_GPADC_ATP_1S :
                gpadcControl |= GPADC_ATP_1S;
                break;
            case HAL_ANA_GPADC_ATP_2S :
                gpadcControl |= GPADC_ATP_2S;
                break;
            default :
                HAL_ASSERT(FALSE, "Invalid atp value", minAtp);
        }

        if (g_halGpadcAtpConfig[0] != HAL_ANA_GPADC_ATP_QTY)
        {
            gpadcControl |= GPADC_CH0_EN;
        }
        if (g_halGpadcAtpConfig[1] != HAL_ANA_GPADC_ATP_QTY)
        {
            gpadcControl |= GPADC_CH1_EN;
        }
        if (g_halGpadcAtpConfig[2] != HAL_ANA_GPADC_ATP_QTY)
        {
            gpadcControl |= GPADC_CH2_EN;
        }
        if (g_halGpadcAtpConfig[3] != HAL_ANA_GPADC_ATP_QTY)
        {
            gpadcControl |= GPADC_CH3_EN;
        }

        gpadcControl |= GPADC_PD_MODE_HW;
        hwp_gpadc->ctrl = gpadcControl;
        // Clear EOC status bit
        hwp_gpadc->irq_clr = GPADC_EOC;
        hwp_gpadc->cmd_set = GPADC_START_MEASURE;
    }
    else
    {
        // No channels are enabled, so ensure the GPADC module is disabled.
        hal_AnaGpadcDisable();
    }

#else // 8808 or later

    UINT8 minAtp = hal_AnaGpadcMinAtp(NULL);
    if (minAtp == HAL_ANA_GPADC_ATP_QTY)
    {
        pmd_GpadcDisable();
    }
    else
    {
        // Init GPADC: Set the measurement interval to the minimum value
        // 0=125us, 1=1ms, 2=8ms, 3=128ms, 4=250ms, 5=500ms, 6=1s, 7=2s

        // For modem chips, the first measurement after deep sleep is always
        // wrong, and we have to get the second one, so the interval should
        // be set to minimum.

        pmd_GpadcSetInterval(0);

        HAL_ANA_GPADC_CHAN_T ch;
        // Enable all the configured channels
        for (ch = HAL_ANA_GPADC_CHAN_0; ch < HAL_ANA_GPADC_CHAN_QTY; ch++)
        {
#ifdef CHIP_HAS_AP
            if (g_halGpadcAtpConfig[ch] == HAL_ANA_GPADC_ATP_QTY)
            {
                hwp_configRegs->GPADC_DATA_CH[ch] = 0;
            }
            else
            {
                hwp_configRegs->GPADC_DATA_CH[ch] = CFG_REGS_GPADC_CH_EN;
            }
#elif (CHIP_ASIC_ID != CHIP_ASIC_ID_8808) && (CHIP_ASIC_ID != CHIP_ASIC_ID_8809)
            // 8809P or later feature phones
            if (g_halGpadcAtpConfig[ch] == HAL_ANA_GPADC_ATP_QTY)
            {
                hwp_pwm->GPADC_DATA_CH[ch] = 0;
            }
            else
            {
                hwp_pwm->GPADC_DATA_CH[ch] = PWM_GPADC_CH_EN;
            }
#endif
        }

#if (CHIP_ASIC_ID != CHIP_ASIC_ID_8808) && (CHIP_ASIC_ID != CHIP_ASIC_ID_8809)
        // smart phones, 8809P or later feature phones
        // Enable PMU GPADC module
        pmd_GpadcEnable();
#endif
    }

#endif // 8808 or later
}
#endif // !FPGA

// =============================================================================
// hal_AnaGpadcOpen
// -----------------------------------------------------------------------------
/// Open a GPADC channel and start periodic measurements.
///
/// This function requests a resource of #HAL_SYS_FREQ_26M.
///
/// That function starts measurements on one of the 4 periodic GPADC channels.
/// The ATP (Acquisition Time Period)is common, so the minimal value of atp
/// is used. Using atp value above HAL_ANA_GPADC_ATP_100MS allows to enter
/// low power between the measure.
/// The measured channels are selected through
/// #HAL_ANA_GPADC_CFG_T parameter.
///
/// @param channel Channel to enable
/// @param atp Atp value required for that channel
// =============================================================================
PUBLIC VOID hal_AnaGpadcOpen(HAL_ANA_GPADC_CHAN_T channel, HAL_ANA_GPADC_ATP_T atp)
{
#ifndef FPGA
    HAL_ASSERT(g_halCalibration == calib_GetPointers(),
               "GPADC Opening with HAL calib pointer inconsistency");

    if (channel >= HAL_ANA_GPADC_CHAN_QTY)
    {
        return;
    }

    // store the atp to the configuration array:
    g_halGpadcAtpConfig[channel] = atp;

    // enable the GPADC hardware
    hal_AnaGpadcEnable();
#endif
}


// =============================================================================
// hal_AnaGpadcClose
// -----------------------------------------------------------------------------
/// Close a GPADC channel.
///
/// This function release the resource to #HAL_SYS_FREQ_32K when all channels
/// are closed.
///
/// This function stops the periodic measurements on a particular channel.
/// It close the GPADC hardware when all channels are disabled.
// =============================================================================
PUBLIC VOID hal_AnaGpadcClose(HAL_ANA_GPADC_CHAN_T channel)
{
#ifndef FPGA
    if (channel >= HAL_ANA_GPADC_CHAN_QTY)
    {
        return;
    }

    // store HAL_ANA_GPADC_ATP_QTY to the configuration array to disable the channel:
    g_halGpadcAtpConfig[channel] = HAL_ANA_GPADC_ATP_QTY;

    // open remaining enabled channels or close the GPADC hardware
    hal_AnaGpadcEnable();
#endif
}


// =============================================================================
// hal_AnaGpadcGet
// -----------------------------------------------------------------------------
/// Return the more up-to-date value measure for the given channel in mV.
/// If the conversion is not finished yet, #HAL_ANA_GPADC_BAD_VALUE is returned.
///
/// Once the first measured has been correctly done, there will always
/// be a value ready to read for all the enabled channel.
///
/// @param channel Channel whose more up to date measured value
/// will be returned.
/// @return Measured value, in mV, or #HAL_ANA_GPADC_BAD_VALUE if no result is
/// available yet.
// =============================================================================
PUBLIC HAL_ANA_GPADC_MV_T hal_AnaGpadcGet(HAL_ANA_GPADC_CHAN_T channel)
{
#ifndef FPGA
    HAL_ANA_GPADC_MV_T mv = hal_AnaGpadcGpadc2Volt(hal_AnaGpadcGetRaw(channel));
    //HAL_TRACE(HAL_IO_TRC,0,"hal_AnaGpadcGet ch=%d val=%dmV",channel, mv);
    return (mv);
#else
    return 4100;
#endif
}


// =============================================================================
// hal_AnaGpadcGetRaw
// -----------------------------------------------------------------------------
/// Return the more up do date value measured for the specified GPADC channel,
/// if available, as the raw value read in the GPADC register. (no conversion
/// towards voltage is done.)
/// If no result is available because the conversion is not ready
/// yet, #HAL_ANA_GPADC_BAD_VALUE is returned.
///
/// @param channel Channel whose more up to date measured value will be returned.
/// @return Raw measured value for this channel by the GPADC,  or
/// #HAL_ANA_GPADC_BAD_VALUE if no result is available yet.
// =============================================================================
UINT32 g_debugDumpGpadcRawValue = 0;
BOOL g_GpdacBusy = FALSE;
PUBLIC UINT16 hal_AnaGpadcGetRaw(HAL_ANA_GPADC_CHAN_T channel)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();
    if(g_GpdacBusy == FALSE)
    {
        g_GpdacBusy = TRUE;
    }
    else
    {
        return HAL_ANA_GPADC_BAD_VALUE;
    }
    hal_SysExitCriticalSection(scStatus);

    if (channel >= HAL_ANA_GPADC_CHAN_QTY)
    {
        scStatus = hal_SysEnterCriticalSection();
        g_GpdacBusy = FALSE;
        hal_SysExitCriticalSection(scStatus);
        return HAL_ANA_GPADC_BAD_VALUE;
    }
    HAL_PROFILE_FUNCTION_ENTER(hal_AnaGpadcGetRaw);

#if defined(FPGA)

    // GPADC not implemented on FPGA
    HAL_PROFILE_FUNCTION_EXIT(hal_AnaGpadcGetRaw);
    scStatus = hal_SysEnterCriticalSection();
    g_GpdacBusy = FALSE;
    hal_SysExitCriticalSection(scStatus);

    return HAL_ANA_GPADC_BAD_VALUE;

#elif (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)

    UINT32 enabledMask = 0;
    REG32*  dataAddress = NULL;
    if (hwp_gpadc->status & GPADC_EOC)
    {
        // Conversion is done, read the GPADC
        switch (channel)
        {
            case HAL_ANA_GPADC_CHAN_0:
                enabledMask = GPADC_CH0_EN;
                dataAddress = &hwp_gpadc->data_ch0;
                break;

            case HAL_ANA_GPADC_CHAN_1:
                enabledMask = GPADC_CH1_EN;
                dataAddress = &hwp_gpadc->data_ch1;
                break;

            case HAL_ANA_GPADC_CHAN_2:
                enabledMask = GPADC_CH2_EN;
                dataAddress = &hwp_gpadc->data_ch2;
                break;

            case HAL_ANA_GPADC_CHAN_3:
                enabledMask = GPADC_CH3_EN;
                dataAddress = &hwp_gpadc->data_ch3;
                break;

            default:
                HAL_ASSERT(FALSE, "hal_AnaGpadcGet called for an inexisting"\
                           "channel number");
        }

        if (hwp_gpadc->ctrl & enabledMask)
        {
            UINT16 rdata= (UINT16)*dataAddress;
            g_halGpadcLastVal[channel] = rdata;
            if (g_debugDumpGpadcRawValue)
            {
                HAL_TRACE(HAL_IO_TRC,0,"hal_AnaGpadcGetRaw ch=%d val=0x%04x",channel, rdata);
            }
            HAL_PROFILE_FUNCTION_EXIT(hal_AnaGpadcGetRaw);
            scStatus = hal_SysEnterCriticalSection();
            g_GpdacBusy = FALSE;
            hal_SysExitCriticalSection(scStatus);
            return rdata;
        }
        else
        {
            // channel not enabled
            if (g_debugDumpGpadcRawValue)
            {
                HAL_TRACE(HAL_IO_TRC,0,"hal_AnaGpadcGetRaw ch=%d val=0x%04x DISABLED",channel, HAL_ANA_GPADC_BAD_VALUE);
            }
            HAL_PROFILE_FUNCTION_EXIT(hal_AnaGpadcGetRaw);

            scStatus = hal_SysEnterCriticalSection();
            g_GpdacBusy = FALSE;
            hal_SysExitCriticalSection(scStatus);
            return HAL_ANA_GPADC_BAD_VALUE;
        }
    }
    else
    {
        // Conversion is not done, read the saved data at last sleep
        // (will return HAL_ANA_GPADC_BAD_VALUE for disabled channels
        if (g_debugDumpGpadcRawValue)
        {
            HAL_TRACE(HAL_IO_TRC,0,"hal_AnaGpadcGetRaw ch=%d val=0x%04x SAVED",channel, g_halGpadcLastVal[channel]);
        }
        HAL_PROFILE_FUNCTION_EXIT(hal_AnaGpadcGetRaw);

        scStatus = hal_SysEnterCriticalSection();
        g_GpdacBusy = FALSE;
        hal_SysExitCriticalSection(scStatus);
        return g_halGpadcLastVal[channel];
    }

#else // 8808 or later

    if (g_halGpadcAtpConfig[channel] == HAL_ANA_GPADC_ATP_QTY)
    {
        HAL_PROFILE_FUNCTION_EXIT(hal_AnaGpadcGetRaw);

        scStatus = hal_SysEnterCriticalSection();
        g_GpdacBusy = FALSE;
        hal_SysExitCriticalSection(scStatus);
        return HAL_ANA_GPADC_BAD_VALUE;
    }

    UINT32 reg;
    UINT16 rdata = HAL_ANA_GPADC_BAD_VALUE;

#ifdef CHIP_HAS_AP

    // Wait until PMU GPADC is stable (after deep sleep)
    UINT32 interval = hal_TimGetUpTime() - g_halGpadcWakeupTime;
    if (interval < 20)
    {
        hal_TimDelay(20 - interval);
    }
    // Assuming no resistive screen TSC support on modem side

#else // !CHIP_HAS_AP

    BOOL gpadcValid = TRUE;

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
    scStatus = hal_SysEnterCriticalSection();
#endif

#if (CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
    gpadcValid = (pmd_TsdTscEnabled() == FALSE);
    if (!gpadcValid)
#endif
    {
        // TSC shares ADC module with PMU GPADC and has a higher priority.
        // If TSC is being pressed, ADC module will be always working for
        // TSC, and PMU GPADC has no chance to get a value, unless TSC
        // is disabled.
        pmd_TsdEnableTsc(FALSE);
    }

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
    if ( ! (gpadcValid && pmd_GpadcChannelEnabled(channel)) )
    {
        pmd_GpadcEnableChannel(channel);
        // GPADC start time has been configured to 125 us in hal_AnaGpadcOpen
        // Wait for 4 ticks ~= 244 us
        hal_TimDelay(4);
    }
#else // 8809P or later feature phones
    if (!gpadcValid)
    {
        // GPADC start time has been configured to 125 us in hal_AnaGpadcOpen
        // Wait for 4 ticks ~= 244 us
        hal_TimDelay(4);
    }
#endif

#endif // !CHIP_HAS_AP

    UINT32 startTime = hal_TimGetUpTime();
    do
    {
#ifdef CHIP_HAS_AP
        reg = hwp_configRegs->GPADC_DATA_CH[channel];
        if ((reg & CFG_REGS_GPADC_CH_EN) == 0)
        {
            break;
        }

        if (reg & CFG_REGS_GPADC_VALID)
        {
            rdata = (UINT16)GET_BITFIELD(reg, CFG_REGS_GPADC_DATA);
            break;
        }
#elif (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
        reg = hwp_pwm->GPADC_DATA;
        if (reg & PWM_GPADC_VALUE_VALID)
        {
            rdata = (UINT16)GET_BITFIELD(reg, PWM_GPADC_VALUE);
            break;
        }
#else // 8809P or later feature phones
        reg = hwp_pwm->GPADC_DATA_CH[channel];
        if ((reg & PWM_GPADC_CH_EN) == 0)
        {
            break;
        }

        if (reg & PWM_GPADC_VALUE_VALID)
        {
            rdata = (UINT16)GET_BITFIELD(reg, PWM_GPADC_DATA);
            break;
        }
#endif

        if (hal_TimGetUpTime() - startTime > 4)
        {
             hal_AnaGpadcDisable();
             hal_TimDelay(1);
             pmd_GpadcEnableChannel(channel);
             hal_TimDelay(4);
        }
        else  if (hal_TimGetUpTime() - startTime > HAL_GPADC_TIMEOUT)
        {
            HAL_ASSERT(FALSE, "GPADC timeout: ch=%d, reg=0x%08x", channel, reg);
        }
    }
    while (rdata  == HAL_ANA_GPADC_BAD_VALUE);

#ifndef CHIP_HAS_AP

#if (CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
    if (!gpadcValid)
#endif
    {
        pmd_TsdEnableTsc(TRUE);
    }

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
    hal_SysExitCriticalSection(scStatus);
#endif

#endif // !CHIP_HAS_AP

    if (g_debugDumpGpadcRawValue)
    {
        HAL_TRACE(HAL_IO_TRC,0,"hal_AnaGpadcGetRaw ch=%d val=0x%04x", channel, rdata);
    }
    HAL_PROFILE_FUNCTION_EXIT(hal_AnaGpadcGetRaw);
    scStatus = hal_SysEnterCriticalSection();
    g_GpdacBusy = FALSE;
    hal_SysExitCriticalSection(scStatus);

    return rdata;

#endif // 8808 or later
}


// =============================================================================
// hal_AnaGpadcCalibUpdateBandgap
// -----------------------------------------------------------------------------
/// Update the bandgap of the GPADC macro.
// =============================================================================
PUBLIC VOID hal_AnaGpadcCalibUpdateBandgap(VOID)
{
}


// =======================================================
// hal_AnaGpadcSleep
// -------------------------------------------------------
/// called in #hal_LpsSleep to try to shut down the GPADC
/// and release the resource. (only if atp >= 100MS)
// =======================================================
PROTECTED VOID hal_AnaGpadcSleep(VOID)
{
#if defined(FPGA)

    // GPADC not implemented in FPGA
    return;

#else // !FPGA

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)

    if (hwp_gpadc->ctrl & (GPADC_CH0_EN|GPADC_CH1_EN|GPADC_CH2_EN|GPADC_CH3_EN))
    {
        UINT8 minAtp = hal_AnaGpadcMinAtp(NULL);
        if (minAtp != HAL_ANA_GPADC_ATP_QTY)
        {
            HAL_ANA_GPADC_CHAN_T channel;
            // initialyse the last values
            for (channel=HAL_ANA_GPADC_CHAN_0; channel<HAL_ANA_GPADC_CHAN_QTY; channel++)
            {
                g_halGpadcLastVal[channel] = hal_AnaGpadcGetRaw(channel);
            }
        }
    }

#endif

    hal_AnaGpadcDisable();

#endif // !FPGA
}


// =======================================================
// hal_AnaGpadcWakeUp
// -------------------------------------------------------
/// called in #hal_LpsSleep to resume the GPADC operation
/// can take back the resource (if GPADC is open)
// =======================================================
PROTECTED VOID hal_AnaGpadcWakeUp(VOID)
{
#if defined(FPGA)

    // GPADC not implemented in FPGA
    return;

#else // !FPGA

    hal_AnaGpadcEnable();

#ifdef CHIP_HAS_AP
    g_halGpadcWakeupTime = hal_TimGetUpTime();
#endif

#endif // !FPGA
}


// =============================================================================
// hal_AnaGpadcSetCalibData
// -----------------------------------------------------------------------------
/// Set GPADC calibration data
// =============================================================================
PUBLIC VOID hal_AnaGpadcSetCalibData(UINT32 low, UINT32 high)
{
    g_halGpadcCalibLow = low;
    g_halGpadcCalibHigh = high;
    g_halGpadcCalibrated = FALSE;
}


// =============================================================================
// hal_AnaGpadcGetCalibData
// -----------------------------------------------------------------------------
/// Get GPADC calibration data
// =============================================================================
PUBLIC VOID hal_AnaGpadcGetCalibData(UINT32 *low, UINT32 *high)
{
    *low = g_halGpadcCalibLow;
    *high = g_halGpadcCalibHigh;
}


#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE) // 8808 or later
// =============================================================================
// hal_AnaGpadcGetTscX
// -----------------------------------------------------------------------------
/// This function get the touch screen X coordinate value
// =============================================================================
PUBLIC UINT16 hal_AnaGpadcGetTscX(VOID)
{
    UINT32 reg;
    UINT16 data;
#ifndef CHIP_DIE_8955

#ifdef CHIP_HAS_AP
    reg = hwp_configRegs->TSC_DATA;

    if (reg & CFG_REGS_TSC_X_VALUE_VALID)
    {
        data = (UINT16)GET_BITFIELD(reg, CFG_REGS_TSC_X_VALUE_BIT);
    }
    else
    {
        data = HAL_ANA_GPADC_BAD_VALUE;
    }
#else // !CHIP_HAS_AP
    reg = hwp_pwm->TSC_DATA;

    if (reg & PWM_TSC_X_VALUE_VALID)
    {
        data = (UINT16)GET_BITFIELD(reg, PWM_TSC_X_VALUE);
    }
    else
    {
        data = HAL_ANA_GPADC_BAD_VALUE;
    }
#endif // !CHIP_HAS_AP
#endif
    return data;
}


// =============================================================================
// hal_AnaGpadcGetTscY
// -----------------------------------------------------------------------------
/// This function get the touch screen Y coordinate value
// =============================================================================
PUBLIC UINT16 hal_AnaGpadcGetTscY(VOID)
{
    UINT32 reg;
    UINT16 data;
#ifndef CHIP_DIE_8955
#ifdef CHIP_HAS_AP
    reg = hwp_configRegs->TSC_DATA;

    if (reg & CFG_REGS_TSC_Y_VALUE_VALID)
    {
        data = (UINT16)GET_BITFIELD(reg, CFG_REGS_TSC_Y_VALUE_BIT);
    }
    else
    {
        data = HAL_ANA_GPADC_BAD_VALUE;
    }
#else // !CHIP_HAS_AP
    reg = hwp_pwm->TSC_DATA;

    if (reg & PWM_TSC_Y_VALUE_VALID)
    {
        data = (UINT16)GET_BITFIELD(reg, PWM_TSC_Y_VALUE);
    }
    else
    {
        data = HAL_ANA_GPADC_BAD_VALUE;
    }
#endif // !CHIP_HAS_AP
#endif
    return data;
}
#endif // 8808 or later



