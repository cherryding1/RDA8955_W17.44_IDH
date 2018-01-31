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

#include "pmd_config.h"
#include "pmd_m.h"
#include "pmdp_debug.h"
#include "pmdp_charger_common.h"
#include "pmdp.h"

#include "hal_ana_gpadc.h"
#include "hal_timers.h"
#include "hal_lps.h"
#include "hal_host.h"

// for function timers
#include "sxr_tim.h"
#include "sxr_tls.h"

#include "string.h"

#include "tgt_pmd_cfg.h"

// defines
#define PMD_TIME_CHG_MON_INTERVAL (500 MILLI_SECONDS)

#ifdef USING_SAMLL_BATTERY
#define PMD_VOLTAGE_CHG_INIT_ON_MEAS (3950)
#else
#define PMD_VOLTAGE_CHG_INIT_ON_MEAS (3500)
#endif

#define PMD_COUNT_CHG_STATUS_STABLE (10)

#define PMD_INTERVAL_CHG_BATT_OFFSET_STABLE (30 SECONDS)

#define PMD_INTERVAL_CHG_PULSE_RESULT_VALID (600 SECONDS) //2016.05.27 Yayuan 120 --> 600, to reduce times of pulse-charge.

#define CHARGING_TIMEOUT_AND_STOPR_CHARGE_TIME_SECS (2 SECONDS)
#if (PMD_INTERVAL_CHG_PULSE_RESULT_VALID <= PMD_INTERVAL_CHG_BATT_OFFSET_STABLE)
#error "Invalid PMD_INTERVAL_CHG_PULSE_RESULT_VALID configuration"
#endif

#define PMD_COUNT_TEMPERATURE_STABLE (5)

#define PMD_COUNT_CHG_CHR_VOLT_STABLE (5)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// PMD_CHARGER_VOLTAGE_VALIDITY_T
// ----------------------------------------------------------------------------
/// This enum lists the validity status
// ============================================================================
typedef enum {
    PMD_CHARGER_VOLTAGE_VALIDITY_UNKNOWN,
    PMD_CHARGER_VOLTAGE_VALIDITY_VALID,
    PMD_CHARGER_VOLTAGE_VALIDITY_INVALID,
} PMD_CHARGER_VOLTAGE_VALIDITY_T;

// ============================================================================
// PMD_TEMPERATURE_VALIDITY_T
// ----------------------------------------------------------------------------
/// This enum lists the validity status
// ============================================================================
typedef enum {
    PMD_TEMPERATURE_VALIDITY_UNKNOWN,
    PMD_TEMPERATURE_VALIDITY_VALID,
    PMD_TEMPERATURE_VALIDITY_INVALID,
} PMD_TEMPERATURE_VALIDITY_T;

// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

PRIVATE PMD_MAP_CHARGER_STATE_T g_pmdChargerState =
    {
        .status = PMD_CHARGER_UNKNOWN,
        .current = PMD_CHARGE_CURRENT_INVALID,
        .handler = NULL,
        .batteryLevel = PMD_BAD_VALUE,
        .highActivityState = TRUE,
};

// Whether charger is on when powering on phone (for handler registration)
PRIVATE BOOL g_pmdChargerInitOnForHandler = FALSE;
// Whether charger is on when powering on phone (for battery voltage measurement)
PRIVATE BOOL g_pmdChargerInitOnForMeas = FALSE;

// Whether the charger is requested to cut off the charge current
// (the charge current is cut off, but the charger detection is still on)
PRIVATE BOOL g_pmdChargerCutOff = FALSE;
// Whether charging has been stopped
PRIVATE BOOL g_pmdChargingStopped = FALSE;

// Whether the battery temperature monitor is enabled
PRIVATE BOOL g_pmdTemperatureMonEnabled = FALSE;
// Previous battery temperature status
PRIVATE PMD_TEMPERATURE_VALIDITY_T g_pmdTemperatureValidity = PMD_TEMPERATURE_VALIDITY_UNKNOWN;
// Battery temperature array
PRIVATE INT16 g_pmdTemperatureAry[PMD_COUNT_TEMPERATURE_STABLE];
// Battery temperature array index
PRIVATE UINT16 g_pmdTemperatureIdx = 0;

// Whether the charger voltage monitor is enabled
PRIVATE BOOL g_pmdChargerChrVoltMonEnabled = FALSE;
// Previous charger voltage status
PRIVATE PMD_CHARGER_VOLTAGE_VALIDITY_T g_pmdChargerChrVoltValidity = PMD_CHARGER_VOLTAGE_VALIDITY_UNKNOWN;
// Charger voltage array
PRIVATE UINT16 g_pmdChrVoltAry[PMD_COUNT_CHG_CHR_VOLT_STABLE];
// Charger voltage array index
PRIVATE UINT16 g_pmdChrVoltIdx = 0;

// Battery voltage array
PRIVATE UINT16 g_pmdBattVoltAry[PMD_COUNT_CHG_STATUS_STABLE];
// Battery voltage array index
PRIVATE UINT16 g_pmdBattVoltIdx = 0;
// Battery measurement Failure counts
PRIVATE UINT16 g_pmdBattMeasFails = 0;
// Battery voltage array index after the battery level becomes stable
PRIVATE UINT16 g_pmdBattVoltStableIdx = 0;
// Charger charging stage
PRIVATE PMD_CHARGING_STAGE_T g_pmdChargingStage = PMD_CHARGING_STAGE_OFF;
// Charger pulse charging period (one stop every x runs)
PRIVATE UINT16 g_pmdChargerPulsePeriod = 2;
// The pulse counter in charger pulse charging
PRIVATE UINT16 g_pmdChargerPulseCnt = 0;
// Whether charging is off within a charger pulse charging period
PRIVATE BOOL g_pmdChargerPulseCurChrOff = FALSE;
// Whether charger is in small current mode
PRIVATE BOOL g_pmdChargerSmallCurrentMode = FALSE;
// Whether it is the first time to check the battery voltage
PRIVATE BOOL g_pmdChargerFirstVoltageCheck = FALSE;
// Whether the charge measurement threshold is reached
PRIVATE BOOL g_pmdChargerMeasThreshReachedFlag = FALSE;
// The time when the measurement threshold is first reached
PRIVATE UINT32 g_pmdChargerMeasThreshReachedTime = 0;

// The time to get the battery voltage before charging
PRIVATE UINT32 g_pmdChargerBattVoltPreChrGetTime = 0;
// The battery voltage before charging
PRIVATE UINT16 g_pmdChargerBattVoltPreChr = PMD_BAD_VALUE;
// The battery voltage offset when charging
PRIVATE INT16 g_pmdChargerBattVoltOffset = 0;
// Whether the battery voltage offset is valid
PRIVATE BOOL g_pmdChargerBattVoltOffsetValid = FALSE;

// Whether the battery capacity curve is valid
PRIVATE BOOL g_pmdBattCapCurveValid = FALSE;
// The battery capacity curve array
PRIVATE CONST TGT_PMD_VOLT_CAP_T *g_pmdBattCapCurveArray = NULL;
// The length of the battery capacity curve array
PRIVATE UINT8 g_pmdBattCapCurveLen = 0;

// The maximum allowable charge current
PRIVATE PMD_CHARGE_CURRENT_T g_pmdMaxChargeCurrentCfg[PMD_MAX_CURRENT_USER_QTY];
PRIVATE PMD_CHARGE_CURRENT_T g_pmdMaxChargeCurrent;
// The charge current to be set
PRIVATE PMD_CHARGE_CURRENT_T g_pmdDesiredChargeCurrent;
VOLATILE BOOL g_usbCharging = FALSE;

PRIVATE BOOL g_pmdReduceChargeCu = FALSE;

// ============================================================================
//  PRIVATE FUNCTIONS PROTOTYPING
// ============================================================================

// ============================================================================
//  FUNCTIONS
// ============================================================================

// ============================================================================
// pmd_SetHighActivity
// ----------------------------------------------------------------------------
/// Set the high activity state for battery measurement compensation.
///
/// @param on the desired state \c TRUE is on, \c FALSE is off.
// ============================================================================
PUBLIC VOID pmd_SetHighActivity(BOOL on)
{
    g_pmdChargerState.highActivityState = on;
}

// ============================================================================
// pmd_GetHighActivity
// ----------------------------------------------------------------------------
/// Get the high activity state for battery measurement compensation.
///
/// @return \c TRUE is on, \c FALSE is off.
// ============================================================================
PUBLIC BOOL pmd_GetHighActivity(VOID)
{
    return g_pmdChargerState.highActivityState;
}

PROTECTED UINT32 pmd_GetTemperatureStableCount(VOID)
{
    return PMD_COUNT_TEMPERATURE_STABLE;
}

PROTECTED INT32 pmd_CalculateMeanTemperature(VOID)
{
    INT32 mean;
    UINT32 count;

    if (g_pmdTemperatureIdx > 0)
    {
        // Get the mean temperature
        if (g_pmdTemperatureIdx < PMD_COUNT_TEMPERATURE_STABLE)
        {
            count = g_pmdTemperatureIdx;
        }
        else
        {
            count = PMD_COUNT_TEMPERATURE_STABLE;
        }
#if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
        mean = 0;
        for (int i = 0; i < count; i++)
        {
            mean += g_pmdTemperatureAry[i];
        }
        mean /= count;
#else
        mean = sxr_ComputeMedian_s16(g_pmdTemperatureAry, count);
#endif
    }
    else
    {
        mean = PMD_BAD_TEMPERATURE;
    }

    return mean;
}

// ============================================================================
// pmd_CheckTemperatureValidity
// ----------------------------------------------------------------------------
/// Check the status of the battery temperature.
///
/// @return PMD_TEMPERATURE_VALIDITY_T.
// ============================================================================
PRIVATE PMD_TEMPERATURE_VALIDITY_T pmd_CheckTemperatureValidity(VOID)
{
    if (gc_pmdChargerProcOps.checkTemperatureValidity &&
        !gc_pmdChargerProcOps.checkTemperatureValidity())
    {
        // Charge aux adc is monitoring charger level
        return g_pmdTemperatureValidity;
    }

    // Get current battery temperature
    INT16 curTp = pmd_TemperatureVolt2Degree(pmd_GetGpadcTemperatureLevel());
    if (curTp != PMD_BAD_TEMPERATURE)
    {
        g_pmdTemperatureAry[g_pmdTemperatureIdx++ % PMD_COUNT_TEMPERATURE_STABLE] = curTp;
        if (g_pmdTemperatureIdx == 0) // handle wrap-around issue
        {
            g_pmdTemperatureIdx = PMD_COUNT_TEMPERATURE_STABLE;
        }

        if (g_pmdTemperatureIdx >= PMD_COUNT_TEMPERATURE_STABLE)
        {
            INT32 meanTp = pmd_CalculateMeanTemperature();

            if (g_pmdTemperatureIdx % PMD_COUNT_TEMPERATURE_STABLE == 0)
            {
                PMD_TRACE(PMD_INFO_TRC, 0, "CheckTemperature: mean=%d", meanTp);
            }

            if (meanTp >= g_pmdConfig->temperatureUpperLimit)
            {
                return PMD_TEMPERATURE_VALIDITY_INVALID;
            }
            else if (g_pmdTemperatureValidity != PMD_TEMPERATURE_VALIDITY_INVALID ||
                     meanTp < g_pmdConfig->temperatureOffsetBackToNormal + g_pmdConfig->temperatureUpperLimit)
            {
                return PMD_TEMPERATURE_VALIDITY_VALID;
            }
        }
    }
    // Current status unknown yet
    return PMD_TEMPERATURE_VALIDITY_UNKNOWN;
}

PROTECTED UINT32 pmd_GetChargerVoltageStableCount(VOID)
{
    return PMD_COUNT_CHG_CHR_VOLT_STABLE;
}

PROTECTED UINT32 pmd_CalculateMeanChargerLevel(VOID)
{
    UINT32 mean;
    UINT32 count;

    if (g_pmdChrVoltIdx > 0)
    {
        // Get the mean charger voltage
        if (g_pmdChrVoltIdx < PMD_COUNT_CHG_CHR_VOLT_STABLE)
        {
            count = g_pmdChrVoltIdx;
        }
        else
        {
            count = PMD_COUNT_CHG_CHR_VOLT_STABLE;
        }
#if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
        mean = 0;
        for (int i = 0; i < count; i++)
        {
            mean += g_pmdChrVoltAry[i];
        }
        mean /= count;
#else
        mean = sxr_ComputeMedian_u16(g_pmdChrVoltAry, count);
#endif
    }
    else
    {
        mean = PMD_BAD_VALUE;
    }

    return mean;
}

// ============================================================================
// pmd_CheckChargerVoltageValidity
// ----------------------------------------------------------------------------
/// Check the status of the charger voltage.
///
/// @return PMD_CHARGER_VOLTAGE_VALIDITY_T.
// ============================================================================
PRIVATE PMD_CHARGER_VOLTAGE_VALIDITY_T pmd_CheckChargerVoltageValidity(VOID)
{
    if (gc_pmdChargerProcOps.checkChargerVoltageValidity &&
        !gc_pmdChargerProcOps.checkChargerVoltageValidity())
    {
        return g_pmdChargerChrVoltValidity;
    }

    // Get current charger voltage
    UINT16 curChargerVolt = pmd_GetGpadcChargerLevel();
    if (curChargerVolt != PMD_BAD_VALUE)
    {
        g_pmdChrVoltAry[g_pmdChrVoltIdx++ % PMD_COUNT_CHG_CHR_VOLT_STABLE] = curChargerVolt;
        if (g_pmdChrVoltIdx == 0) // handle wrap-around issue
        {
            g_pmdChrVoltIdx = PMD_COUNT_CHG_CHR_VOLT_STABLE;
        }

        if (g_pmdChrVoltIdx >= PMD_COUNT_CHG_CHR_VOLT_STABLE)
        {
            UINT32 meanVolt = pmd_CalculateMeanChargerLevel();

            if (g_pmdChrVoltIdx % PMD_COUNT_CHG_CHR_VOLT_STABLE == 0)
            {
                PMD_TRACE(PMD_INFO_TRC, 0, "CheckChargerVolt: mean=%d", meanVolt);
            }

            if (meanVolt >= g_pmdConfig->chargerLevelUpperLimit)
            {
                return PMD_CHARGER_VOLTAGE_VALIDITY_INVALID;
            }
            else if (meanVolt < (g_pmdConfig->chargerOffsetBackToNormal + g_pmdConfig->chargerLevelUpperLimit))
            {
                return PMD_CHARGER_VOLTAGE_VALIDITY_VALID;
            }
            else if (meanVolt >= g_pmdConfig->chargerOffsetBackToNormal + g_pmdConfig->chargerLevelUpperLimit)
            {

                if (g_pmdChargerChrVoltValidity == PMD_CHARGER_VOLTAGE_VALIDITY_VALID || g_pmdChargerChrVoltValidity == PMD_CHARGER_VOLTAGE_VALIDITY_UNKNOWN)
                {
                    return PMD_CHARGER_VOLTAGE_VALIDITY_VALID;
                }
                else
                {
                    return PMD_CHARGER_VOLTAGE_VALIDITY_INVALID;
                }
            }
        }
    }
    // Current status unknown yet
    return PMD_CHARGER_VOLTAGE_VALIDITY_UNKNOWN;
}

BOOL pmd_GetChargerVoltageValidity(VOID)
{
    if (g_pmdChargerChrVoltValidity == PMD_CHARGER_VOLTAGE_VALIDITY_VALID || g_pmdChargerChrVoltValidity == PMD_CHARGER_VOLTAGE_VALIDITY_UNKNOWN)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
// ============================================================================
// pmd_GetGpadcBatteryLevel
// ----------------------------------------------------------------------------
/// Return the more up do date value measured for the battery level
///
/// @return the battery level in mV or
/// #PMD_BAD_VALUE if no result is available yet.
// ============================================================================
PUBLIC UINT16 pmd_GetGpadcBatteryLevel(VOID)
{
//PMD_TRACE(PMD_INFO_TRC, 0, "pmd: highActivityState %d", g_pmdChargerState.highActivityState);

// Starting from Gallite, battery measurement is stable at the time the system gets out from LP mode.
// That is because, LDO power supply of GPADC is controlled by h/w LP state machine, and
// there are 2+ ms between applying LDO power and LP power up done.

#ifdef FPGA
    return 4100;
#endif

    // VBAT_VOLT Stable: measure it
    UINT16 readBat = hal_AnaGpadcGet(g_pmdConfig->batteryGpadcChannel);
    if (HAL_ANA_GPADC_BAD_VALUE == readBat)
    {
        return PMD_BAD_VALUE;
    }
    else
    {
        // Battery voltage divider is 2 : 1
        return (readBat * 3);
    }
}

// ============================================================================
// pmd_GetRealTimeBatteryLevel
// ----------------------------------------------------------------------------
/// Return current battery level
///
/// @param pPercent receive a precentage of battery charge (can be NULL)
/// @return the battery level in mV or
/// #PMD_BAD_VALUE if no result is available yet, in that case \p pPercent is
/// not updated.
// ============================================================================
PUBLIC UINT16 pmd_GetRealTimeBatteryLevel(UINT8 *pPercent)
{
    UINT16 batLevel, orgLevel, highActLevel;
    char desc[30];

    batLevel = orgLevel = pmd_GetGpadcBatteryLevel();
    if (batLevel == PMD_BAD_VALUE)
    {
        PMD_TRACE(PMD_INFO_TRC, 0, "pmd_GetBatteryLevel: Bad value");
        return PMD_BAD_VALUE;
    }

    UINT32 scStatus = hal_SysEnterCriticalSection();

    batLevel += pmd_GetHighActivityVoltOffset();
    highActLevel = batLevel;

    strncpy(desc, "ChrOff", sizeof(desc));

    switch (g_pmdChargerState.status)
    {
    case PMD_CHARGER_UNPLUGGED:
        // There is no charger, and therefore no charger timer is running,
        // so the battery voltage array is free to use
        g_pmdBattVoltAry[g_pmdBattVoltIdx++ % PMD_COUNT_CHG_STATUS_STABLE] = batLevel;
        if (g_pmdBattVoltIdx == 0) // handle wrap-around issue
        {
            g_pmdBattVoltIdx = PMD_COUNT_CHG_STATUS_STABLE;
        }
        break;

    case PMD_CHARGER_PLUGGED:
    case PMD_CHARGER_FAST_CHARGE:
    case PMD_CHARGER_FULL_CHARGE:
        if (g_pmdChargingStage == PMD_CHARGING_STAGE_ON)
        {
            if (g_pmdChargingStopped)
            {
                // No charging compensation
                break;
            }

            if (g_pmdChargerBattVoltOffsetValid)
            {
                batLevel += g_pmdChargerBattVoltOffset;
                strncpy(desc, "ChrOn NewVal", sizeof(desc));
            }
            else
            {
                batLevel = PMD_BAD_VALUE;
                strncpy(desc, "ChrOn BadOff", sizeof(desc));
            }
        }
        else if (g_pmdChargingStage == PMD_CHARGING_STAGE_PULSE)
        {
            batLevel = PMD_BAD_VALUE;
            strncpy(desc, "ChrPulse", sizeof(desc));
        }
        break;

    default:
        // Charger is plugged-in but disabled due to errors in
        // charger voltage or battery temperature, etc.
        break;
    }

    hal_SysExitCriticalSection(scStatus);

    desc[sizeof(desc) - 1] = '\0';
    PMD_TRACE(PMD_INFO_TRC, (1 << 3), "pmd_GetBatteryLevel: %d / %d / %d mV (%s)",
              orgLevel, highActLevel, batLevel, desc);

    if (batLevel != PMD_BAD_VALUE)
    {
        g_pmdChargerState.batteryLevel = batLevel;
    }

    if (pPercent)
    {
        *pPercent = pmd_BatteryLevelToPercent(batLevel);
    }

    if (gc_pmdChargerProcOps.postGetBatteryLevel)
    {
        gc_pmdChargerProcOps.postGetBatteryLevel();
    }

    return batLevel;
}

// ============================================================================
// pmd_GetBatteryLevel
// ----------------------------------------------------------------------------
/// Return the more up do date value measured for the battery level
///
/// @param pPercent receive a precentage of battery charge (can be NULL)
/// @return the battery level in mV or
/// #PMD_BAD_VALUE if no result is available yet, in that case \p pPercent is
/// not updated.
// ============================================================================
PUBLIC UINT16 pmd_GetBatteryLevel(UINT8 *pPercent)
{
    UINT16 batLevel;

    batLevel = pmd_GetRealTimeBatteryLevel(pPercent);
    if (batLevel == PMD_BAD_VALUE)
    {
        // Report previous battery voltage
        batLevel = g_pmdChargerState.batteryLevel;
        if (pPercent)
        {
            *pPercent = pmd_BatteryLevelToPercent(batLevel);
        }
    }

    return batLevel;
}

PROTECTED UINT32 pmd_GetAverageGpadcBatteryLevel(UINT32 count)
{
    UINT32 total, volt, i;

    total = 0;
#ifdef FPGA
    return 4100;
#endif

    for (i = 0; i < count; i++)
    {
        while ((volt = pmd_GetGpadcBatteryLevel()) == PMD_BAD_VALUE)
            ;
        total += volt;
        hal_SysWaitMicrosecond(300);
    }

    return total / count;
}

PROTECTED UINT32 pmd_CalculateMeanBatteryLevel(VOID)
{
    UINT32 mean;
    UINT32 count;

    if (g_pmdBattVoltIdx > 0)
    {
        // Get the mean battery voltage
        if (g_pmdBattVoltIdx < PMD_COUNT_CHG_STATUS_STABLE)
        {
            count = g_pmdBattVoltIdx;
        }
        else
        {
            count = PMD_COUNT_CHG_STATUS_STABLE;
        }
#if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
        mean = 0;
        for (int i = 0; i < count; i++)
        {
            mean += g_pmdBattVoltAry[i];
        }
        mean /= count;
#else
        mean = sxr_ComputeMedian_u16(g_pmdBattVoltAry, count);
#endif
    }
    else
    {
        mean = PMD_BAD_VALUE;
    }

    return mean;
}

// ============================================================================
// pmd_BatteryLevelToPercent
// ----------------------------------------------------------------------------
/// Return the \p batteryLevel converted into Percent
///
/// @param batteryLevel the battery level in mV
/// @return a precentage of the given battery level
// ============================================================================
PUBLIC UINT8 pmd_BatteryLevelToPercent(UINT16 batteryLevel)
{
    INT32 percent;
    UINT32 i;
    UINT16 prevVolt, prevCap, step;

    if (batteryLevel <= g_pmdConfig->powerDownVoltageMV)
    {
        return 0;
    }
    else if (batteryLevel >= g_pmdConfig->batteryLevelFullMV)
    {
        return 100;
    }

    if (g_pmdBattCapCurveValid)
    {
        for (i = 1; i < g_pmdBattCapCurveLen; i++)
        {
            if (batteryLevel < g_pmdBattCapCurveArray[i].volt)
            {
                break;
            }
        }
        PMD_ASSERT(i < g_pmdBattCapCurveLen,
                   "Failed to get battery capacity for level %d mV",
                   batteryLevel);
        prevVolt = g_pmdBattCapCurveArray[i - 1].volt;
        prevCap = g_pmdBattCapCurveArray[i - 1].cap;
        step = g_pmdBattCapCurveArray[i].volt - prevVolt;
        percent = prevCap + (g_pmdBattCapCurveArray[i].cap - prevCap) * (batteryLevel - prevVolt) / step;
    }
    else
    {
        percent = 100 * (batteryLevel - g_pmdConfig->powerDownVoltageMV) / (g_pmdConfig->batteryLevelFullMV - g_pmdConfig->powerDownVoltageMV);
    }

    return percent;
}

// ============================================================================
// pmd_GetChargerStatus
// ----------------------------------------------------------------------------
/// Return the charger status
///
/// @return the charger status
// ============================================================================
PUBLIC PMD_CHARGER_STATUS_T pmd_GetChargerStatus(VOID)
{
    return g_pmdChargerState.status;
}

PROTECTED PMD_CHARGING_STAGE_T pmd_GetChargingStage(VOID)
{
    return g_pmdChargingStage;
}

// ============================================================================
// pmd_SetChargerStatusHandler
// ----------------------------------------------------------------------------
/// Set the handler used to detect charger status (plugged, charge ...)
// ============================================================================
PUBLIC VOID pmd_SetChargerStatusHandler(PMD_CHARGER_HANDLER_T handler)
{
    g_pmdChargerState.handler = handler;

    // Avoid reentrance of pmd_DcOnHandler/pmd_ChargerManager
    // coming from TIMER IRQ
    UINT32 scStatus = hal_SysEnterCriticalSection();
    if (g_pmdChargerInitOnForHandler)
    {
        g_pmdChargerInitOnForHandler = FALSE;
        pmd_DcOnHandler(TRUE);
    }
    hal_SysExitCriticalSection(scStatus);
}

PROTECTED VOID pmd_UpdateChargeCurrentInternal(PMD_CHARGE_CURRENT_T current)
{
    if (current > g_pmdMaxChargeCurrent)
    {
        current = g_pmdMaxChargeCurrent;
    }

    if (current != g_pmdChargerState.current)
    {
        // Calculate the new battery voltage offset after the new current takes effect
        g_pmdBattVoltStableIdx = 0;
        g_pmdChargerBattVoltOffsetValid = FALSE;

        if (gc_pmdChargerProcOps.setChargerCurrent)
        {
            gc_pmdChargerProcOps.setChargerCurrent(current);
        }

        g_pmdChargerState.current = current;
    }
}

VOID pmd_ReduceChargingCurrentDynamic(UINT32 battvolt)
{
    if (g_pmdReduceChargeCu == TRUE)
    {
        if (battvolt > 3800)
        {
            pmd_UpdateChargeCurrentInternal(PMD_CHARGER_100MA);
        }
        else if (battvolt < 3600)
        {
            pmd_UpdateChargeCurrentInternal(g_pmdConfig->batteryChargeCurrent);
        }
    }
}

//MMI can call the function if temperature too high when playing mp4 ,etc
// low_current [ 1 ... 3 ]
VOID pmd_ReduceChargeCurrent(UINT8 low_current)
{
    if (g_pmdChargerState.status == PMD_CHARGER_FAST_CHARGE)
    {
        if (low_current == 0)
        {
            pmd_UpdateChargeCurrentInternal(g_pmdConfig->batteryChargeCurrent);
            g_pmdReduceChargeCu = FALSE;
        }
        else
        {
            g_pmdReduceChargeCu = TRUE;
            //if(g_pmdChargerState.current >= PMD_CHARGER_400MA)
            //{
            // pmd_UpdateChargeCurrentInternal(PMD_CHARGER_300MA);
            //}
        }
    }
    else if (low_current == 0)
    {
        g_pmdReduceChargeCu = FALSE;
        pmd_UpdateChargeCurrentInternal(g_pmdConfig->batteryChargeCurrent);
    }
}

// ============================================================================
// pmd_SetChargeCurrent
// ----------------------------------------------------------------------------
/// Set the current for the charge.
// ============================================================================
PROTECTED VOID pmd_SetChargeCurrent(PMD_CHARGE_CURRENT_T current)
{
    g_pmdDesiredChargeCurrent = current;
    pmd_UpdateChargeCurrentInternal(current);
}

PROTECTED PMD_CHARGE_CURRENT_T pmd_GetChargeCurrent(VOID)
{
    return g_pmdChargerState.current;
}

PROTECTED VOID pmd_SetMaxChargeCurrent(PMD_MAX_CURRENT_USER_T user, PMD_CHARGE_CURRENT_T current)
{
    g_pmdMaxChargeCurrentCfg[user] = current;

    // Get the min MaxChargeCurrentCfg
    for (user = 0; user < PMD_MAX_CURRENT_USER_QTY; user++)
    {
        if (current > g_pmdMaxChargeCurrentCfg[user])
        {
            current = g_pmdMaxChargeCurrentCfg[user];
        }
    }

    g_pmdMaxChargeCurrent = current;

    if (current < g_pmdChargerState.current)
    {
        pmd_UpdateChargeCurrentInternal(current);
    }
    else if (g_pmdDesiredChargeCurrent > g_pmdChargerState.current)
    {
        pmd_UpdateChargeCurrentInternal(g_pmdDesiredChargeCurrent);
    }
}

PROTECTED VOID pmd_InitMaxChargeCurrent(VOID)
{
    PMD_MAX_CURRENT_USER_T user;
    PMD_CHARGE_CURRENT_T current;

    g_pmdMaxChargeCurrentCfg[PMD_MAX_CURRENT_CHR_VOLT] = g_pmdConfig->batteryChargeCurrent;

    if (g_pmdConfig->batteryChargeCurrentOnUsb == PMD_CHARGE_CURRENT_INVALID)
    {
        g_pmdMaxChargeCurrentCfg[PMD_MAX_CURRENT_CHR_TYPE] =
            g_pmdConfig->batteryChargeCurrent;
    }
    else
    {
        g_pmdMaxChargeCurrentCfg[PMD_MAX_CURRENT_CHR_TYPE] =
            g_pmdConfig->batteryChargeCurrentOnUsb;
    }

    // Get min MaxChargeCurrentCfg
    current = g_pmdMaxChargeCurrentCfg[0];
    for (user = 1; user < PMD_MAX_CURRENT_USER_QTY; user++)
    {
        if (current > g_pmdMaxChargeCurrentCfg[user])
        {
            current = g_pmdMaxChargeCurrentCfg[user];
        }
    }

    g_pmdMaxChargeCurrent = current;
}

PUBLIC VOID pmd_SetChargerType(PMD_CHARGER_TYPE_T type)
{
    // CAUSION:
    // This function is outside the control of the charge timer,
    // and exclusive mutual access is needed over shared resources.

    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_SetChargerType: type=%d", type);

    if (type == PMD_CHARGER_TYPE_AC &&
        g_pmdConfig->batteryChargeCurrentOnUsb != PMD_CHARGE_CURRENT_INVALID)
    {
        UINT32 scStatus = hal_SysEnterCriticalSection();
        pmd_SetMaxChargeCurrent(PMD_MAX_CURRENT_CHR_TYPE,
                                g_pmdConfig->batteryChargeCurrent);
        hal_SysExitCriticalSection(scStatus);
    }

    if (gc_pmdChargerProcOps.setChargerType)
    {
        gc_pmdChargerProcOps.setChargerType(type);
    }
}

#ifdef __BATTERY_TEMPERATURE_DETECT__
#define TEMPERATURE_CHAN HAL_ANA_GPADC_CHAN_0
#endif
PROTECTED VOID pmd_ChargerPlugin(VOID)
{
    UINT32 curTime = hal_TimGetUpTime();

    // Save context to calculate a more accurate battery voltage
    if (!g_pmdChargerBattVoltOffsetValid)
    {
        if (g_pmdBattVoltIdx > 0)
        {
            int end;
            end = (g_pmdBattVoltIdx > PMD_COUNT_CHG_STATUS_STABLE) ? PMD_COUNT_CHG_STATUS_STABLE : g_pmdBattVoltIdx;
#if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
            g_pmdChargerBattVoltPreChr = 0;
            for (int i = 0; i < end; i++)
            {
                g_pmdChargerBattVoltPreChr += g_pmdBattVoltAry[i];
            }
            g_pmdChargerBattVoltPreChr /= end;
#else
            g_pmdChargerBattVoltPreChr =
                sxr_ComputeMedian_u16(g_pmdBattVoltAry, end);
#endif
        }
        else
        {
            g_pmdChargerBattVoltPreChr = pmd_GetGpadcBatteryLevel();
            if (g_pmdChargerBattVoltPreChr == PMD_BAD_VALUE)
            {
                g_pmdChargerBattVoltPreChr = g_pmdChargerState.batteryLevel;
            }
        }
        g_pmdChargerBattVoltPreChrGetTime = curTime;
        PMD_TRACE(PMD_INFO_TRC, 0, "pmd_ChargerPlugin: PrevBattVolt=%d, VoltIdx=%d",
                  g_pmdChargerBattVoltPreChr, g_pmdBattVoltIdx);
    }

    g_pmdChargerState.startTime = curTime;
    g_pmdBattMeasFails = 0;
    g_pmdBattVoltIdx = 0;
    // First check the battery voltage while charger is active
    g_pmdChargerFirstVoltageCheck = TRUE;
    g_pmdChargerMeasThreshReachedFlag = FALSE;
    g_pmdChargingStage = PMD_CHARGING_STAGE_ON;

    // Start charger voltage monitor
    g_pmdChrVoltIdx = 0;
    g_pmdChargerChrVoltValidity = PMD_CHARGER_VOLTAGE_VALIDITY_UNKNOWN;
    if (g_pmdChargerChrVoltMonEnabled)
    {
        if (!g_pmdTemperatureMonEnabled ||
            g_pmdConfig->temperatureGpadcChannel != g_pmdConfig->chargerGpadcChannel)
        {
            hal_AnaGpadcOpen(g_pmdConfig->chargerGpadcChannel, HAL_ANA_GPADC_ATP_2S);
#ifdef __BATTERY_TEMPERATURE_DETECT__
            hal_AnaGpadcOpen(TEMPERATURE_CHAN, HAL_ANA_GPADC_ATP_2S);
#endif /* __BATTERY_TEMPERATURE_DETECT__ */
        }
    }

    // Start temperature monitor
    g_pmdTemperatureIdx = 0;
    g_pmdTemperatureValidity = PMD_TEMPERATURE_VALIDITY_UNKNOWN;

    // Init the maximum allowable charge current
    pmd_InitMaxChargeCurrent();
// Set charge current to lowest to avoid the damage of P-channel MOSFET
// when the charger voltage is too high
#ifdef USING_SAMLL_BATTERY
    pmd_SetChargeCurrent(PMD_CHARGER_200MA);
#else
    pmd_SetChargeCurrent(PMD_CHARGER_50MA);
#endif
    // Set VFB to max to avoid over charge
    pmd_SetChargerVfb(PMD_CHARGER_VFB_MAX);

    // Enable charging (h/w thresholds take effect)
    pmd_StopCharging(FALSE);

    if (gc_pmdChargerProcOps.chargerPlugin)
    {
        gc_pmdChargerProcOps.chargerPlugin();
    }
}

PROTECTED VOID pmd_ChargerPlugout(VOID)
{
    g_pmdChargingStage = PMD_CHARGING_STAGE_OFF;
    g_usbCharging = FALSE;
    // To record recent battery voltages when no charger is plugged-in
    g_pmdBattVoltIdx = 0;
    // Reset battery voltage offset valid flag
    g_pmdChargerBattVoltOffsetValid = FALSE;

    // Stop charger voltage monitor
    if (g_pmdChargerChrVoltMonEnabled)
    {
        if (!g_pmdTemperatureMonEnabled ||
            g_pmdConfig->temperatureGpadcChannel != g_pmdConfig->chargerGpadcChannel)
        {
            hal_AnaGpadcClose(g_pmdConfig->chargerGpadcChannel);
        }
    }

    // Exit the small current mode
    pmd_ChargerSmallCurrent(FALSE);
    g_pmdChargerSmallCurrentMode = FALSE;

    // Set charge current to lowest to avoid the damage of P-channel MOSFET
    // when the charger voltage is too high
    pmd_SetChargeCurrent(PMD_CHARGER_50MA);
    // Set VFB to max to avoid over charge
    pmd_SetChargerVfb(PMD_CHARGER_VFB_MAX);

    // Restore the default charging config (charging enabled)
    pmd_StopCharging(FALSE);

    if (gc_pmdChargerProcOps.chargerPlugout)
    {
        gc_pmdChargerProcOps.chargerPlugout();
    }
}

PROTECTED VOID pmd_ChargerEnterPulseStage(VOID)
{
    g_pmdChargerPulseCnt = 0;
    g_pmdChargerPulseCurChrOff = FALSE;
    g_pmdChargingStage = PMD_CHARGING_STAGE_PULSE;
}

PROTECTED VOID pmd_ChargerMonChargerVolt(VOID)
{
    PMD_CHARGER_VOLTAGE_VALIDITY_T chrValid;

    if (g_pmdChargerChrVoltMonEnabled)
    {
        // Check charger voltage
        chrValid = pmd_CheckChargerVoltageValidity();
        if (g_pmdChargerChrVoltValidity != chrValid)
        {
            if (chrValid == PMD_CHARGER_VOLTAGE_VALIDITY_INVALID)
            {
                // Set to error stage
                g_pmdBattVoltIdx = 0;
                g_pmdChargingStage = PMD_CHARGING_STAGE_ERROR;
                // Force to disable charging
                pmd_StopCharging(TRUE);
            }
            else if (g_pmdChargerChrVoltValidity == PMD_CHARGER_VOLTAGE_VALIDITY_INVALID)
            {
                // If the charger voltage changes from invalid to valid,
                // enter pulse stage.
                g_pmdChargerState.startTime = hal_TimGetUpTime();
                g_pmdBattVoltIdx = 0;
                pmd_SetChargeCurrent(g_pmdConfig->batteryChargeCurrent);
                pmd_ChargerEnterPulseStage();
            }
            // If the charger voltage is unknown, current stage is charging-on.

            g_pmdChargerChrVoltValidity = chrValid;
        }
    }
    else
    {
        g_pmdChargerChrVoltValidity = PMD_CHARGER_VOLTAGE_VALIDITY_VALID;
    }
}

PROTECTED VOID pmd_ChargerMonBattTemperature(VOID)
{
    PMD_TEMPERATURE_VALIDITY_T tpValid;

    if (g_pmdTemperatureMonEnabled)
    {
        // Check temperature
        tpValid = pmd_CheckTemperatureValidity();
        if (g_pmdTemperatureValidity != tpValid)
        {
            if (tpValid == PMD_TEMPERATURE_VALIDITY_INVALID)
            {
                // Set to error stage
                g_pmdBattVoltIdx = 0;
                g_pmdChargingStage = PMD_CHARGING_STAGE_ERROR;
                // Force to disable charging
                pmd_StopCharging(TRUE);
            }
            else if (g_pmdTemperatureValidity == PMD_TEMPERATURE_VALIDITY_INVALID)
            {
                // If the battery temperature changes from invalid to valid,
                // enter pulse stage.
                g_pmdChargerState.startTime = hal_TimGetUpTime();
                g_pmdBattVoltIdx = 0;
                pmd_ChargerEnterPulseStage();
            }
            // If the battery temperature is unknown, current stage is charging-on.

            g_pmdTemperatureValidity = tpValid;
        }
    }
    else
    {
        g_pmdTemperatureValidity = PMD_TEMPERATURE_VALIDITY_VALID;
    }
}

PROTECTED INT16 pmd_GetVfbWindowOffset(VOID)
{
    if (gc_pmdChargerProcOps.getVfbWindowOffset)
    {
        return gc_pmdChargerProcOps.getVfbWindowOffset();
    }

    return pmd_GetHighActivity() ? gc_pmdChargerProcCfg.highActVfbWindowOffset : 0;
}

PROTECTED PMD_CHARGER_STATUS_T pmd_ChargerChargingState(UINT16 *battLevel, INT16 *battOffset)
{
    PMD_CHARGER_STATUS_T nextState = g_pmdChargerState.status;
    UINT32 meanBattVolt;
    UINT32 curTime = hal_TimGetUpTime();
    UINT32 battVoltVfbUpperLimit, battVoltVfbLowerLimit;
    UINT32 battVoltVfbLowerLimitSmallCurrent;
    UINT16 curBattVolt, curBattOffset;
    INT16 vfbWinOffset;
    BOOL needMeas = FALSE;
    BOOL recalChrOffset = FALSE;

    // Get current raw battery voltage (VFB adjustment requires raw voltage)
    //in case adc gets a voltage with big error,
    //use average value of two times.
    //2016.06.03 by Yayuan

    curBattVolt = *battLevel = pmd_GetGpadcBatteryLevel() / 2 + pmd_GetGpadcBatteryLevel() / 2;
    curBattOffset = *battOffset = pmd_GetHighActivityVoltOffset();

    if (curBattVolt >= (PMD_BAD_VALUE - 1) / 2)
    {
        *battLevel = PMD_BAD_VALUE;
        PMD_TRACE(PMD_INFO_TRC, 0, "PMD_BAD_VALUE");

        g_pmdBattMeasFails++;
        return nextState;
    }

    /*
    curBattVolt = *battLevel = pmd_GetGpadcBatteryLevel();
    PMD_TRACE(PMD_INFO_TRC, 0, "curBatVolt=%d", curBattVolt);

        curBattOffset = *battOffset = pmd_GetHighActivityVoltOffset();
     if (curBattVolt > PMD_BAD_VALUE)
        {
            g_pmdBattMeasFails++;
            return nextState;
        }

    */

    else if (g_pmdChargingStage != PMD_CHARGING_STAGE_PULSE ||
             g_pmdChargerPulseCurChrOff)
    {
        if (g_pmdBattVoltIdx == 0)
        {
            // Reset g_pmdBattVoltStableIdx as well if g_pmdBattVoltIdx is reset
            g_pmdBattVoltStableIdx = 0;
        }
        if (++g_pmdBattVoltStableIdx == 0) // handle wrap-around issue
        {
            g_pmdBattVoltStableIdx = PMD_COUNT_CHG_STATUS_STABLE;
        }

        // Compensate battery voltage according to high activity state
        g_pmdBattVoltAry[g_pmdBattVoltIdx % PMD_COUNT_CHG_STATUS_STABLE] =
            curBattVolt + curBattOffset;
        if (++g_pmdBattVoltIdx == 0) // handle wrap-around issue
        {
            g_pmdBattVoltIdx = PMD_COUNT_CHG_STATUS_STABLE;
        }
    }

    if (g_pmdChargingStage == PMD_CHARGING_STAGE_PULSE)
    {
        if (g_pmdChargerPulsePeriod < 2)
        {
            // At least 50% charging-active ratio
            g_pmdChargerPulsePeriod = 2;
        }

        if (g_pmdChargerPulseCnt++ % g_pmdChargerPulsePeriod == 0)
        {
            // Force to disable charging
            pmd_StopCharging(TRUE);
            g_pmdChargerPulseCurChrOff = TRUE;
        }
        else
        {
            // Enable charging (h/w thresholds take effect)
            pmd_StopCharging(FALSE);
            g_pmdChargerPulseCurChrOff = FALSE;
            return nextState;
        }
    }

    if (g_pmdBattVoltIdx < PMD_COUNT_CHG_STATUS_STABLE)
    {
        return nextState;
    }

    // Get the mean battery voltage
    meanBattVolt = pmd_CalculateMeanBatteryLevel();
    pmd_ReduceChargingCurrentDynamic(meanBattVolt);

    if (gc_pmdChargerProcOps.chargerChargingState)
    {
        gc_pmdChargerProcOps.chargerChargingState(meanBattVolt);
    }

    switch (g_pmdChargingStage)
    {
    case PMD_CHARGING_STAGE_OFF:
        if (meanBattVolt >= g_pmdConfig->batteryLevelRechargeMV)
        {
            break;
        }
        // Otherwise going through pulse stage
        pmd_ChargerEnterPulseStage();
    case PMD_CHARGING_STAGE_PULSE:
        // Save context to calculate a more accurate battery voltage
        g_pmdChargerInitOnForMeas = FALSE;
        g_pmdChargerBattVoltOffsetValid = FALSE;
        g_pmdChargerBattVoltPreChr = meanBattVolt;
        g_pmdChargerBattVoltPreChrGetTime = curTime;
        PMD_TRACE(PMD_INFO_TRC, 0, "ChrState: @@@ Pulse meanBattVolt=%d @@@", meanBattVolt);

        // Reset the measurement threshold reached flag
        g_pmdChargerMeasThreshReachedFlag = FALSE;
        // Reset voltage array index
        g_pmdBattVoltIdx = 0;
        // Check the mean voltage
        if (meanBattVolt >= g_pmdConfig->batteryLevelChargeTermMV + 10) //tagrget 4220 --> 4200, so, -10 --> 10
        {
            // Force to disable charging
            pmd_StopCharging(TRUE);
            // Set stage to off
            g_pmdChargingStage = PMD_CHARGING_STAGE_OFF;
            // Set next state to full
            nextState = PMD_CHARGER_FULL_CHARGE;
        }
        else
        {
            if (!g_pmdChargerSmallCurrentMode &&
                meanBattVolt >= gc_pmdChargerProcCfg.battOffsetSmallCurrent +
                                    g_pmdConfig->batteryLevelChargeTermMV)
            {
                PMD_TRACE(PMD_INFO_TRC, 0, "ChrState: Enter small current mode");
                pmd_ChargerSmallCurrent(TRUE);
                g_pmdChargerSmallCurrentMode = TRUE;

                //2016.05.27, Yayuan Remove this if case, when enter small current mode
                // not change vfb value
                /*
                                if (pmd_SetChargerVfb(PMD_CHARGER_VFB_DEC))
                                {
                                    PMD_TRACE(PMD_INFO_TRC, 0, "ChrState:SmallCur: Dec VFB");
                                }
                            */
            }
            // Enable charging (h/w thresholds take effect)
            pmd_StopCharging(FALSE);
            // set stage to on
            g_pmdChargingStage = PMD_CHARGING_STAGE_ON;
            // Set next state to fast but keep full state
            if (nextState != PMD_CHARGER_FULL_CHARGE)
            {
                nextState = PMD_CHARGER_FAST_CHARGE;
            }
        }
        break;
    case PMD_CHARGING_STAGE_ON:
        vfbWinOffset = pmd_GetVfbWindowOffset();
        battVoltVfbUpperLimit =
            gc_pmdChargerProcCfg.battOffsetVfbUpperLimit +
            vfbWinOffset +
            g_pmdConfig->batteryLevelChargeTermMV;
        battVoltVfbLowerLimit =
            gc_pmdChargerProcCfg.battOffsetVfbLowerLimit +
            vfbWinOffset +
            g_pmdConfig->batteryLevelChargeTermMV;
        battVoltVfbLowerLimitSmallCurrent =
            gc_pmdChargerProcCfg.battOffsetVfbLowerLimitSmallCurrent +
            g_pmdConfig->batteryLevelChargeTermMV;

        //**************************************************************//
        //in the following area, vfb adjust method was changed by Yayuan, 2016.05.27
        //to fulfil vfb dynamic upper/lower limit
        //**************************************************************//

        // Using current battery voltage instead of the mean value to
        // speed up the VFB adjustment
        PMD_TRACE(PMD_INFO_TRC, 0, "ChrState: CurBattVolt=%d,BattVoltOffset=%d!!!!", curBattVolt, g_pmdChargerBattVoltOffset);

        if (curBattVolt >= battVoltVfbUpperLimit)
        {
            if ((curBattVolt >= battVoltVfbUpperLimit + 20) || (g_pmdChargerBattVoltOffset < -100) || (g_pmdChargerBattVoltOffset == 0))

                if (pmd_SetChargerVfb(PMD_CHARGER_VFB_INC))
                {
                    PMD_TRACE(PMD_INFO_TRC, 0, "ChrState: Inc VFB, curBatt=%d, meanBattVolt=%d, BattVoltOffset=%d",
                              curBattVolt, meanBattVolt, g_pmdChargerBattVoltOffset);
                    recalChrOffset = TRUE;
                }
        }

        if (curBattVolt < battVoltVfbLowerLimit + 20)
        {

            if ((g_pmdChargerBattVoltOffset == 0) || (g_pmdChargerBattVoltOffset < -100))
            {

                if (g_pmdChargerSmallCurrentMode)
                {
                    PMD_TRACE(PMD_INFO_TRC, 0, "ChrState: Exit small current mode");
                    pmd_ChargerSmallCurrent(FALSE);
                    g_pmdChargerSmallCurrentMode = FALSE;
                    needMeas = TRUE; //2016.05.26 add needMeas = TRUE;   when exit
                    recalChrOffset = TRUE;
                }

                else if (curBattVolt < battVoltVfbLowerLimit)
                {

                    if (pmd_SetChargerVfb(PMD_CHARGER_VFB_DEC))
                    {
                        PMD_TRACE(PMD_INFO_TRC, 0, "&&&&ChrState: Dec VFB, curBatt=%d,  meanBattVolt=%d, VfbLowerLimit=%d, BattVoltOffset=%d",
                                  curBattVolt, meanBattVolt, battVoltVfbLowerLimit, g_pmdChargerBattVoltOffset);
                        recalChrOffset = TRUE;
                    }
                }
            }

            else if ((g_pmdChargerBattVoltOffset > -100) && (g_pmdChargerBattVoltOffset < 0))
            {

                if (g_pmdChargerSmallCurrentMode)
                {
                    PMD_TRACE(PMD_INFO_TRC, 0, "ChrState: Exit small current mode");
                    pmd_ChargerSmallCurrent(FALSE);
                    g_pmdChargerSmallCurrentMode = FALSE;
                    recalChrOffset = TRUE;
                }

                else
                {

                    if (pmd_SetChargerVfb(PMD_CHARGER_VFB_DEC))
                    {
                        PMD_TRACE(PMD_INFO_TRC, 0, "$$$$ChrState: Dec VFB, curBatt=%d, meanBattVolt=%d,  VfbLowerLimit=%d, BattVoltOffset=%d",
                                  curBattVolt, meanBattVolt, battVoltVfbLowerLimit + 30, g_pmdChargerBattVoltOffset);
                        recalChrOffset = TRUE;
                    }
                }
            }
        }

        //**************************************************************//
        //in the upward area, vfb adjust method was changed by Yayuan, 2016.05.27
        //to fulfil vfb dynamic upper/lower limit
        //**************************************************************//

        if (recalChrOffset)
        {
            // Calculate the new battery voltage offset after the new VFB takes effect
            g_pmdBattVoltStableIdx = 0;
            g_pmdChargerBattVoltOffsetValid = FALSE;
        }

        if (!g_pmdChargerBattVoltOffsetValid)
        {
            if (g_pmdChargerBattVoltPreChr == PMD_BAD_VALUE ||
                curTime - g_pmdChargerBattVoltPreChrGetTime >= PMD_INTERVAL_CHG_PULSE_RESULT_VALID)
            {
                needMeas = TRUE;
            }
            else if (g_pmdBattVoltStableIdx >= PMD_COUNT_CHG_STATUS_STABLE &&
                     curTime - g_pmdChargerBattVoltPreChrGetTime >= PMD_INTERVAL_CHG_BATT_OFFSET_STABLE)
            {
                // Calculate the battery voltage offset
                g_pmdChargerBattVoltOffsetValid = TRUE;
                if (meanBattVolt > g_pmdChargerBattVoltPreChr)
                {
                    g_pmdChargerBattVoltOffset = g_pmdChargerBattVoltPreChr - meanBattVolt;
                }
                else
                {
                    g_pmdChargerBattVoltOffset = 0;
                }
                PMD_TRACE(PMD_INFO_TRC, 0, "ChrState: ### BattVoltOffset=%d (%d - %d) ###",
                          g_pmdChargerBattVoltOffset, g_pmdChargerBattVoltPreChr, meanBattVolt);
            }
        }

        if (meanBattVolt >= g_pmdConfig->batteryLevelChargeTermMV) //2016.05.26, add (-10) by Yayuan, to speed refreshing 'real' voltage.;;//tagrget 4220 --> 4200, so, -10 --> 10
        {
            if (!g_pmdChargerMeasThreshReachedFlag)
            {
                // Save the measurement threshold reached time
                g_pmdChargerMeasThreshReachedTime = curTime;
                g_pmdChargerMeasThreshReachedFlag = TRUE;
            }

            if (g_pmdChargerFirstVoltageCheck ||
                curTime - g_pmdChargerMeasThreshReachedTime >=
                    gc_pmdChargerProcCfg.intervalBattLevelCheck ||
                (g_pmdChargerBattVoltOffsetValid &&
                 meanBattVolt + g_pmdChargerBattVoltOffset >=
                     g_pmdConfig->batteryLevelChargeTermMV + 15)) //2016.05.26, add (-5) by Yayuan;;;; //tagrget 4220 --> 4200, so, -5 --> 15
            {
                needMeas = TRUE;
            }
        }

        // If charger is on when system starts, measure the actual battery voltage
        if (!needMeas &&
            g_pmdChargerInitOnForMeas &&
            // No need to measure if the battery level is not stable
            g_pmdBattVoltStableIdx >= PMD_COUNT_CHG_STATUS_STABLE)
        {
            needMeas = TRUE;
        }

        if (needMeas &&
            // Measure only if the estimated battery voltage is high enough
            meanBattVolt >= PMD_VOLTAGE_CHG_INIT_ON_MEAS)
        {
            g_pmdBattVoltIdx = 0;
            pmd_ChargerEnterPulseStage();
        }

        g_pmdChargerFirstVoltageCheck = FALSE;
        break;
    case PMD_CHARGING_STAGE_ERROR:
    default:
        break;
    }

    // For bad or old battery
    if (g_pmdChargingStage != PMD_CHARGING_STAGE_ERROR &&
        curTime - g_pmdChargerState.startTime > g_pmdConfig->batteryChargeTimeout)
    {
        nextState = PMD_CHARGER_FULL_CHARGE;
        PMD_TRACE(PMD_INFO_TRC, 0, "pmd_ChargerChargingState:  timeout and disale charging curTime = %d ", curTime);
        pmd_StopCharging(TRUE); // fot a tip
        if (curTime - g_pmdChargerState.startTime > g_pmdConfig->batteryChargeTimeout + CHARGING_TIMEOUT_AND_STOPR_CHARGE_TIME_SECS)
        {
            g_pmdChargerState.startTime = curTime;
            PMD_TRACE(PMD_INFO_TRC, 0, "pmd_ChargerChargingState:  timeout and enable charging curTime = %d ", curTime);
#ifdef CHARGING_TIMEOUT_AND_STOP_CHARGE
#else
            pmd_StopCharging(FALSE);

            g_pmdBattVoltIdx = 0;
            pmd_ChargerEnterPulseStage();
#endif
        }
    }

    return nextState;
}

// ============================================================================
// pmd_ChargerManager
// ----------------------------------------------------------------------------
/// The charger manager is only called on timer interrupt this way there is
/// no risk of reentrance and so no critical section is needed.
///
// note: sxr_StopFunctionTimer(pmd_ChargerManager); not needed in this function
// as it is allways called by an expired timer.
// ============================================================================
#ifdef __BATTERY_TEMPERATURE_DETECT__
extern void mmi_temperature_check_handle();
#endif

PROTECTED VOID pmd_ChargerManager(VOID *param)
{
    PMD_CHARGER_STATUS_T nextState = g_pmdChargerState.status;
    PMD_CHARGING_STAGE_T POSSIBLY_UNUSED oldStage = g_pmdChargingStage;
    UINT32 interval;
    UINT16 battLevel = PMD_BAD_VALUE;
    INT16 battOffset = 0;
#if defined(__MMX_TEMP_CHARGER_FUNCTION__)
    U8 tempture_check;
#endif

    if (gc_pmdChargerProcOps.chargerManager)
    {
        interval = gc_pmdChargerProcOps.chargerManager();
        if (interval)
        {
            sxr_StopFunctionTimer(pmd_ChargerManager);
            sxr_StartFunctionTimer(interval, pmd_ChargerManager, 0, 0);

            PMD_TRACE(PMD_INFO_TRC, 0, "ChrMgr: restart mgr @ %d ticks", interval);
            return;
        }
    }

    // Taper and term status are not checked any more, as these h/w signals are
    // inaccurate on some 8805 chipsets, and the case is even worse for 8806
    RDA_PMU_CHR_STATUS_T status = pmd_GetChargerHwStatus();
    if (status == CHR_STATUS_AC_OFF)
    {
        nextState = PMD_CHARGER_UNPLUGGED;

        pmd_ChargerPlugout();

        sxr_StopFunctionTimer(pmd_ChargerManager);
    }
    else // Charger is unknown or plugged in, and the timer should be alive
    {
        if (status == CHR_STATUS_UNKNOWN)
        {
            // Do nothing but wait for the next monitor interval to check charger h/w status
        }
        else // Charger is plugged in
        {
            if (nextState == PMD_CHARGER_UNKNOWN ||
                nextState == PMD_CHARGER_UNPLUGGED)
            {
                // Upper layer thinks there is no charger at this time
                // Notify upper layer that charger is just plugged in
                nextState = PMD_CHARGER_PLUGGED;
            }
            else
            {
                // Upper layer has known that charger is plugged in

                pmd_ChargerMonChargerVolt();

                pmd_ChargerMonBattTemperature();

                if (gc_pmdChargerProcOps.chargerMonitor)
                {
                    gc_pmdChargerProcOps.chargerMonitor();
                }

                if (g_pmdChargerChrVoltValidity == PMD_CHARGER_VOLTAGE_VALIDITY_VALID &&
                    g_pmdTemperatureValidity == PMD_TEMPERATURE_VALIDITY_VALID)
                {
                    if (nextState == PMD_CHARGER_PLUGGED)
                    {
                        // Restore the charger current.
                        // These codes are executed only once each time the charger is plugged in.
                        // (The state will be changed from PLUGGED to FAST_CHARGE or something else below)

                        // Set the charger current to user-configured max value
                        pmd_SetChargeCurrent(g_pmdConfig->batteryChargeCurrent);
                        // Re-calculate the battery mean volatge as the charger current is restored
                        g_pmdBattVoltIdx = 0;
                    }

                    // Report charging progress to upper layer
                    nextState = pmd_ChargerChargingState(&battLevel, &battOffset);
                    if (nextState == PMD_CHARGER_PLUGGED)
                    {
                        nextState = PMD_CHARGER_FAST_CHARGE;
                    }
                }
                else if (g_pmdChargerChrVoltValidity == PMD_CHARGER_VOLTAGE_VALIDITY_INVALID)
                {
                    // Notify upper layer that charger voltage is too high
                    nextState = PMD_CHARGER_VOLTAGE_ERROR;
                }
                else if (g_pmdTemperatureValidity == PMD_TEMPERATURE_VALIDITY_INVALID)
                {
                    // Notify upper layer that battery temperature is too high
                    nextState = PMD_CHARGER_TEMPERATURE_ERROR;
                }

#ifdef __BATTERY_TEMPERATURE_DETECT__
#if defined(__MMX_TEMP_CHARGER_FUNCTION__)

                tempture_check = pmd_CheckChargerTemperature();
                if (tempture_check == PMD_TEMPERATURE_STATUS_OVER_55)
                {
                    pmd_ForceChargerFinished(TRUE);
                    PMD_TRACE(PMD_INFO_TRC, 0, "charger_OVER_55: nextState=%d ,oldstatus=%d, pulseCnt=%d ",
                              nextState, g_pmdChargerState.status, g_pmdChargerState.pulsedOnCount);

                    if (nextState != g_pmdChargerState.status)
                    {
                        g_pmdChargerState.status = nextState;
                        if (g_pmdChargerState.handler != NULL)
                        {
                            g_pmdChargerState.handler(nextState);
                        }
                    }

                    sxr_StopFunctionTimer(pmd_ChargerManager);
                    sxr_StartFunctionTimer(PMD_TIME_CHG_MON_INTERVAL << 2, pmd_ChargerManager, 0, 0);
                    g_pmd_over_battemp_flag = TRUE;
                    PMD_TRACE(PMD_INFO_TRC, 0, "charger_OVER_55: g_pmd_over_battemp_flag=%d ", g_pmd_over_battemp_flag);
                    mmi_temperature_check_handle();

                    return;
                }
                else if (tempture_check == PMD_TEMPERATURE_STATUS_50_55)
                {

                    PMD_TRACE(PMD_INFO_TRC, 0, "charger_50_55: g_pmd_over_battemp_flag=%d ", g_pmd_over_battemp_flag);

                    if (g_pmd_over_battemp_flag)
                    {
                        pmd_ForceChargerFinished(TRUE);

                        PMD_TRACE(PMD_INFO_TRC, 0, "charger_50_55: nextState=%d ,oldstatus=%d, pulseCnt=%d ",
                                  nextState, g_pmdChargerState.status, g_pmdChargerState.pulsedOnCount);

                        if (nextState != g_pmdChargerState.status)
                        {
                            g_pmdChargerState.status = nextState;

                            if (g_pmdChargerState.handler != NULL)
                            {
                                g_pmdChargerState.handler(nextState);
                            }
                        }

                        sxr_StopFunctionTimer(pmd_ChargerManager);
                        sxr_StartFunctionTimer(PMD_TIME_CHG_MON_INTERVAL << 2, pmd_ChargerManager, 0, 0);

                        mmi_temperature_check_handle();

                        return;
                    }

                    else
                    {
                        PMD_TRACE(PMD_INFO_TRC, 0, "charger_50_55");

                        //    g_pmdChargingStage = PMD_CHARGING_STAGE_MEAS  ;
                        if (g_pmdChargingStage == PMD_CHARGING_STAGE_ON)
                            pmd_ForceChargerFinished(FALSE);

                        mmi_temperature_check_handle();
                    }
                }

                else if (tempture_check == PMD_TEMPERATURE_STATUS_NORMAL)
                {

                    PMD_TRACE(PMD_INFO_TRC, 0, "charger_normal");

#if defined(__MMX_HI_TEMP_STOP_RECHARGER_LO_RESTART__)
                    if (g_pmdChargingStage == PMD_CHARGING_STAGE_ON)
                        pmd_ForceChargerFinished(FALSE);

                    //if(g_pmd_over_battemp_flag)
                    mmi_temperature_check_handle();

                    g_pmd_over_battemp_flag = FALSE;
#else
                    if ((g_pmdChargingStage == PMD_CHARGING_STAGE_ON) && (g_pmd_over_battemp_flag == FALSE))
                        pmd_ForceChargerFinished(FALSE);
#endif
                    PMD_TRACE(PMD_INFO_TRC, 0, "normal: g_pmd_over_battemp_flag=%d ", g_pmd_over_battemp_flag);
                }

                PMD_TRACE(PMD_INFO_TRC, 0, "g_pmd_over_battemp_flag=%d ", g_pmd_over_battemp_flag);

#else  /*__MMX_TEMP_CHARGER_FUNCTION__*/

                mmi_temperature_check_handle();

                if (!pmd_CheckChargerTemperature())
                {
                    pmd_ForceChargerFinished(TRUE);
                    PMD_TRACE(PMD_INFO_TRC, 0, "charger: nextState=%d ,oldstatus=%d, pulseCnt=%d ",
                              nextState, g_pmdChargerState.status, g_pmdChargerState.pulsedOnCount);

                    if (nextState != g_pmdChargerState.status)
                    {
                        g_pmdChargerState.status = nextState;
                        if (g_pmdChargerState.handler != NULL)
                        {
                            ((PMD_CHARGER_HANDLER_T)g_pmdChargerState.handler)(nextState);
                        }
                    }

                    sxr_StopFunctionTimer(pmd_ChargerManager);
                    sxr_StartFunctionTimer(PMD_TIME_CHG_MON_INTERVAL, pmd_ChargerManager, 0, 0);
                    return;
                }
                else
                {
                    if (g_pmdChargingStage == PMD_CHARGING_STAGE_ON)
                    {
                        pmd_ForceChargerFinished(FALSE);
                    }
                }
#endif /*__MMX_TEMP_CHARGER_FUNCTION__*/
#endif /* __BATTERY_TEMPERATURE_DETECT__ */
            }
        }

        if (g_pmdChargerChrVoltValidity == PMD_CHARGER_VOLTAGE_VALIDITY_UNKNOWN ||
            g_pmdTemperatureValidity == PMD_TEMPERATURE_VALIDITY_UNKNOWN)
        {
            interval = PMD_TIME_CHG_MON_INTERVAL >> 2;
        }
        else
        {
            interval = PMD_TIME_CHG_MON_INTERVAL;
        }

        sxr_StopFunctionTimer(pmd_ChargerManager);
        sxr_StartFunctionTimer(interval, pmd_ChargerManager, 0, 0);
    }

    PMD_TRACE(PMD_INFO_TRC, 0,
              "ChrMgr: state %d -> %d, stage %d -> %d (batt %d + %d)",
              g_pmdChargerState.status, nextState,
              oldStage, g_pmdChargingStage,
              battLevel, battOffset);

    if (nextState != g_pmdChargerState.status)
    {
        g_pmdChargerState.status = nextState;
        if (g_pmdChargerState.handler != NULL)
        {
            ((PMD_CHARGER_HANDLER_T)g_pmdChargerState.handler)(nextState);
        }
    }
}

// ============================================================================
// pmd_DcOnHandler
// ----------------------------------------------------------------------------
/// Registerred handler for GPIO_0 used as DC-ON Detect.
///
// ============================================================================
PUBLIC VOID pmd_DcOnHandler(BOOL on)
{
    if (on)
    {
#if (CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
        pmd_ForceChargerFinished(TRUE);
        return;
#endif
        pmd_ChargerPlugin();

        // Start a timer to read the acutal charger status in non-blocking mode
        // (PMD_TIME_CHG_MON_INTERVAL>>2) is the debounce time
        sxr_StopFunctionTimer(pmd_ChargerManager);
        sxr_StartFunctionTimer(PMD_TIME_CHG_MON_INTERVAL >> 2, pmd_ChargerManager, 0, 0);

        PMD_TRACE(PMD_WARN_TRC, 0, "charger: DC-ON");
    }
    else
    {
        // charger DC off will be handled by timer
        PMD_TRACE(PMD_WARN_TRC, 0, "charger: DC-OFF");
    }
}

// ============================================================================
// pmd_GetBattCapCurve
// ----------------------------------------------------------------------------
/// Get the battery capacity curve from target config.
// ============================================================================
PRIVATE VOID pmd_GetBattCapCurve(VOID)
{
    UINT32 i;

    tgt_GetPmdBattCapCurve(&g_pmdBattCapCurveArray,
                           &g_pmdBattCapCurveLen);

    if (g_pmdBattCapCurveArray == NULL ||
        g_pmdBattCapCurveLen == 0)
    {
        return;
    }

    for (i = 0; i < g_pmdBattCapCurveLen; i++)
    {
        if (i == 0)
        {
            PMD_ASSERT(g_pmdBattCapCurveArray[i].cap == 0,
                       "BatteryCurve: The first capacity must be 0: %d",
                       g_pmdBattCapCurveArray[i].cap);
            PMD_ASSERT(g_pmdBattCapCurveArray[i].volt == g_pmdConfig->powerDownVoltageMV,
                       "BatteryCurve: The first voltage (%d) must = powerDownVoltageMV (%d)",
                       g_pmdBattCapCurveArray[i].volt, g_pmdConfig->powerDownVoltageMV);
        }
        else
        {
            if (i == g_pmdBattCapCurveLen - 1)
            {
                PMD_ASSERT(g_pmdBattCapCurveArray[i].cap == 100,
                           "BatteryCurve: The last capacity must be 100: %d",
                           g_pmdBattCapCurveArray[i].cap);
                PMD_ASSERT(g_pmdBattCapCurveArray[i].volt == g_pmdConfig->batteryLevelFullMV,
                           "BatteryCurve: The last voltage (%d) must = batteryLevelFullMV (%d)",
                           g_pmdBattCapCurveArray[i].volt, g_pmdConfig->batteryLevelFullMV);
            }

            PMD_ASSERT(g_pmdBattCapCurveArray[i - 1].cap <= g_pmdBattCapCurveArray[i].cap,
                       "BatteryCurve: The capacity of previous point must <= that of next point:"
                       "IdxP=%d CapP=%d, IdxN=%d CapN=%d",
                       i - 1, g_pmdBattCapCurveArray[i - 1].cap, i, g_pmdBattCapCurveArray[i].cap);
            PMD_ASSERT(g_pmdBattCapCurveArray[i - 1].volt < g_pmdBattCapCurveArray[i].volt,
                       "BatteryCurve: The voltage of previous point must < that of next point:"
                       "IdxP=%d VoltP=%d, IdxN=%d VoltN=%d",
                       i - 1, g_pmdBattCapCurveArray[i - 1].volt, i, g_pmdBattCapCurveArray[i].volt);
        }
    }

    g_pmdBattCapCurveValid = TRUE;
}

PROTECTED VOID pmd_InitCharger(VOID)
{
    PMD_ASSERT(g_pmdConfig->batteryChargeCurrent !=
                   PMD_CHARGE_CURRENT_INVALID,
               "batteryChargeCurrent must be configured");
    PMD_ASSERT(g_pmdConfig->batteryChargeCurrentOnUsb <=
                   g_pmdConfig->batteryChargeCurrent,
               "batteryChargeCurrentOnUsb (%u) must <= batteryChargeCurrent (%u)",
               g_pmdConfig->batteryChargeCurrentOnUsb,
               g_pmdConfig->batteryChargeCurrent);

    PMD_ASSERT(g_pmdConfig->batteryLevelRechargeMV,
               "batteryLevelRechargeMV must be configured");
    PMD_ASSERT(g_pmdConfig->batteryLevelRechargeMV <=
                   g_pmdConfig->batteryLevelChargeTermMV,
               "batteryLevelRechargeMV (%u) must <= batteryLevelChargeTermMV (%u)",
               g_pmdConfig->batteryLevelRechargeMV,
               g_pmdConfig->batteryLevelChargeTermMV);
    PMD_ASSERT(g_pmdConfig->batteryLevelFullMV <=
                   g_pmdConfig->batteryLevelChargeTermMV,
               "batteryLevelFullMV (%u) must <= batteryLevelChargeTermMV (%u)",
               g_pmdConfig->batteryLevelFullMV,
               g_pmdConfig->batteryLevelChargeTermMV);

    PMD_ASSERT(gc_pmdChargerProcCfg.battOffsetVfbUpperLimit >=
                   gc_pmdChargerProcCfg.battOffsetVfbLowerLimit,
               "battOffsetVfbUpperLimit (%d) must >= battOffsetVfbLowerLimit (%d)",
               gc_pmdChargerProcCfg.battOffsetVfbUpperLimit,
               gc_pmdChargerProcCfg.battOffsetVfbLowerLimit);
    PMD_ASSERT(gc_pmdChargerProcCfg.battOffsetVfbUpperLimit >= 0,
               "battOffsetVfbUpperLimit (%d) must >= 0",
               gc_pmdChargerProcCfg.battOffsetVfbUpperLimit);
    PMD_ASSERT(gc_pmdChargerProcCfg.battOffsetVfbLowerLimitSmallCurrent >= 0,
               "battOffsetVfbLowerLimitSmallCurrent (%d) must >= 0",
               gc_pmdChargerProcCfg.battOffsetVfbLowerLimitSmallCurrent);
    PMD_ASSERT(gc_pmdChargerProcCfg.highActVfbWindowOffset >= 0,
               "highActVfbWindowOffset (%d) must >= 0",
               gc_pmdChargerProcCfg.highActVfbWindowOffset);

#ifndef _USE_HIGH_VOLTAGE_4350_BATTERY_
    PMD_ASSERT(g_pmdConfig->batteryLevelChargeTermMV <= 4300,
               "batteryLevelChargeTermMV large than 4300,it shoulb be define the macro _USE_HIGH_VOLTAGE_4350_BATTERY_");
#endif

    RDA_PMU_CHR_STATUS_T chrStatus = CHR_STATUS_AC_OFF;

    g_pmdMapAccess.chargerState = (PMD_MAP_CHARGER_STATE_T *)&g_pmdChargerState;

    // Check whether to enable charger voltage monitoring
    if (g_pmdConfig->chargerGpadcChannel >= HAL_ANA_GPADC_CHAN_0 &&
        g_pmdConfig->chargerGpadcChannel < HAL_ANA_GPADC_CHAN_QTY &&
        g_pmdConfig->chargerLevelUpperLimit != 0)
    {
        g_pmdChargerChrVoltMonEnabled = TRUE;
    }

    // Check whether to enable temperature monitoring
    if (g_pmdConfig->temperatureGpadcChannel >= HAL_ANA_GPADC_CHAN_0 &&
        g_pmdConfig->temperatureGpadcChannel < HAL_ANA_GPADC_CHAN_QTY &&
        g_pmdConfig->temperatureUpperLimit != 0)
    {
        g_pmdTemperatureMonEnabled = TRUE;
        // Start temperature monitor
        hal_AnaGpadcOpen(g_pmdConfig->temperatureGpadcChannel, HAL_ANA_GPADC_ATP_2S);
    }

    // Get the battery capacity curve
    pmd_GetBattCapCurve();

    // Init the maximum allowable charge current
    pmd_InitMaxChargeCurrent();
// Set charge current to lowest to avoid the damage of P-channel MOSFET
// when the charger voltage is too high

#if (CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
    pmd_SetChargeCurrent(PMD_CHARGER_150MA);
#else
#ifdef USING_SAMLL_BATTERY
    pmd_SetChargeCurrent(PMD_CHARGER_200MA);
#else
    pmd_SetChargeCurrent(PMD_CHARGER_50MA);
#endif
#endif
    // Set VFB to max to avoid over charge
    pmd_SetChargerVfb(PMD_CHARGER_VFB_MAX);

#if (CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
    return;
#endif

    // Init the charger status
    if (gc_pmdChargerProcOps.initCharger)
    {
        chrStatus = gc_pmdChargerProcOps.initCharger();
    }

// Init the battery level (after GPADC is setup in individual
// initCharger callback)
#if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
    g_pmdChargerState.batteryLevel = 3600;
#else
    g_pmdChargerState.batteryLevel = pmd_GetAverageGpadcBatteryLevel(5);
#endif

    if (chrStatus == CHR_STATUS_AC_ON)
    {
        g_pmdChargerState.status = PMD_CHARGER_PLUGGED;
        g_pmdChargingStage = PMD_CHARGING_STAGE_ON;
        g_pmdChargerInitOnForHandler = TRUE;
        // Compensate the battery level
        if (g_pmdChargerState.batteryLevel +
                g_pmdConfig->batteryOffsetPowerDownChargeMV <
            PMD_VOLTAGE_CHG_INIT_ON_MEAS)
        {
            g_pmdChargerInitOnForMeas = TRUE;
            g_pmdChargerBattVoltOffset =
                g_pmdConfig->batteryOffsetPowerDownChargeMV;
        }
        else
        {
            // Current battery level is high enough, and the charger
            // can be stopped for measurement in a short interval
            pmd_StopCharging(TRUE);
            hal_TimDelay(200 MS_WAITING);
            UINT32 voltNoChr = pmd_GetAverageGpadcBatteryLevel(5);
            if (g_pmdChargerState.batteryLevel <= voltNoChr)
            {
                g_pmdChargerBattVoltOffset = 0;
            }
            else
            {
                g_pmdChargerBattVoltOffset =
                    voltNoChr - g_pmdChargerState.batteryLevel;
            }
            pmd_StopCharging(FALSE);
        }
        g_pmdChargerState.batteryLevel += g_pmdChargerBattVoltOffset;
        // Not to save this offfset, and still mark it as invalid, so as to
        // avoid recording the voltages when playing startup animation
        // and sound
        g_pmdChargerBattVoltOffsetValid = FALSE;
    }
    else
    {
        g_pmdChargerState.status = PMD_CHARGER_UNPLUGGED;
    }
    g_pmdChargerState.batteryLevel += pmd_GetHighActivityVoltOffset();
    if (chrStatus != CHR_STATUS_AC_ON)
    {
        if (g_pmdChargerState.batteryLevel < g_pmdConfig->powerOnVoltageMV)
        {
            hal_TimDelay(1638);
            hal_SysShutdown();
        }
    }
}

PUBLIC UINT8 pmd_GetPowerOnVoltPercent(VOID)
{
    return pmd_BatteryLevelToPercent(g_pmdConfig->powerOnVoltageMV);
}

PUBLIC UINT8 pmd_GetPowerDownVoltPercent(VOID)
{
    return pmd_BatteryLevelToPercent(g_pmdConfig->powerDownVoltageMV);
}

PUBLIC UINT32 pmd_GetChargerGpadcChannel(VOID)
{
    return g_pmdConfig->chargerGpadcChannel;
}

PUBLIC BOOL pmd_ChargerChrVoltMonEnabled(VOID)
{
    return g_pmdChargerChrVoltMonEnabled;
}

PUBLIC UINT32 pmd_GetTemperatureGpadcChannel(VOID)
{
    return g_pmdConfig->temperatureGpadcChannel;
}

PUBLIC BOOL pmd_TemperatureMonEnabled(VOID)
{
    return g_pmdTemperatureMonEnabled;
}

PROTECTED VOID pmd_StopCharging(BOOL stop)
{
    if (g_pmdChargerCutOff)
    {
        stop = TRUE;
    }

    if (stop != g_pmdChargingStopped)
    {
        if (gc_pmdChargerProcOps.stopCharging)
        {
            gc_pmdChargerProcOps.stopCharging(stop);
        }

        g_pmdChargingStopped = stop;
    }
}

// ============================================================================
// pmd_CutOffCharger
// ----------------------------------------------------------------------------
/// Cut off the charge current, but allow the charger detection.
// ============================================================================
PUBLIC VOID pmd_CutOffCharger(VOID)
{
    g_pmdChargerCutOff = TRUE;
    pmd_StopCharging(TRUE);
}

// ============================================================================
// pmd_EnableCharger
// ----------------------------------------------------------------------------
/// Enable/disable the charger function both in s/w and h/w.
// ============================================================================
PUBLIC VOID pmd_EnableCharger(BOOL enable)
{
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_EnableCharger: %d", enable);

    UINT32 scStatus = hal_SysEnterCriticalSection();

    if (gc_pmdChargerProcOps.enableCharger)
    {
        gc_pmdChargerProcOps.enableCharger(enable);
    }

    if (enable)
    {
        // Enable charging (h/w thresholds take effect)
        pmd_StopCharging(FALSE);
    }
    else
    {
        // Proceed pluging-out handling
        pmd_ChargerPlugout();

        // Force to disable charging
        pmd_StopCharging(TRUE);

        sxr_StopFunctionTimer(pmd_ChargerManager);

        PMD_CHARGER_STATUS_T nextState = PMD_CHARGER_UNPLUGGED;
        if (nextState != g_pmdChargerState.status)
        {
            g_pmdChargerState.status = nextState;
            if (g_pmdChargerState.handler != NULL)
            {
                ((PMD_CHARGER_HANDLER_T)g_pmdChargerState.handler)(nextState);
            }
        }
    }

    hal_SysExitCriticalSection(scStatus);
}

// ============================================================================
// pmd_EnableChargerDynamic
// ----------------------------------------------------------------------------
/// Enable/disable the charger function dynamic.
// ============================================================================
PUBLIC VOID pmd_EnableChargerDynamic(BOOL enable)
{
    if (gc_pmdChargerProcOps.enableCharger)
    {
        gc_pmdChargerProcOps.enableCharger(enable);
    }

    if (enable)
    {
        g_pmdChargingStage = PMD_CHARGING_STAGE_ON;
        pmd_ForceChargerFinished(FALSE);
        g_pmdChargerCutOff = FALSE;
        pmd_StopCharging(FALSE);
    }
    else
    {
        g_pmdChargingStage = PMD_CHARGING_STAGE_OFF;
        pmd_ForceChargerFinished(TRUE);
        g_pmdChargerCutOff = TRUE;
        pmd_StopCharging(TRUE);
    }
}
