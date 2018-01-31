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


#include <csw.h>
#include <errorcode.h>
#include <sul.h>
#include <pm.h>
#include <ts.h>

#include "csw_debug.h"
#include "dbg.h"
#include "csw_csp.h"

#include "pmd_m.h"
#if (((CSW_EXTENDED_API_PMD_USB == 1) && (CHIP_HAS_USB == 1))||(defined(AT_USB_MODEM_NO_MMMI)))
#include "uctls_m.h"
#endif

#include "hal_ana_gpadc.h"
#include "hal_lps.h"
#include "hal_host.h"
#include "pmd_m.h"

#include "cos.h" // for HVY_TIMER_IN
#include "sxr_tim.h"
#include "sxr_tls.h"
#include "assert.h"

#ifdef CHIP_HAS_AP
#include "syscmds_pm.h"
#endif

#ifdef PMD_TWO_BATTARY
extern PUBLIC BOOL pmd_GetMainStatus(VOID);
#define MOINTER_TEMP_FOR_PLUGIN_MIAN_BATTERY (220 MS_WAITING)
#endif

#ifdef VMIC_POWER_ON_WORKAROUND
extern BOOL hal_AudVMicValid(VOID);
#endif

#ifdef ABB_HP_DETECT
extern BOOL hal_AudCheckUsrFMOrLineIN(VOID);
#endif
extern VOID xcv_SetStableVolt(UINT16 volt);

// PM_VMIC_POWER_POLICY
// when VMIC is On earpiece detection is active
// 0 : VMIC Always on
// 1 : VMIC off during low power
// 2 : VMIC off every other paging during low power
#ifndef PM_VMIC_POWER_POLICY
#define PM_VMIC_POWER_POLICY                1
#endif

#if defined(VMIC_POWER_ON_WORKAROUND) && (PM_VMIC_POWER_POLICY != 1)
#error "PM_VMIC_POWER_POLICY should be 1 when VMIC_POWER_ON_WORKAROUND is enabled."
#endif

// Whether to debouce earpiece GPADC detection


// Number of debounce for the ear detect
#define PM_AVERAGE_EAR_NB                   10

#define PM_MAX_VALID_BATTERY_STEP           800

// Number of measurements for the MEAN calculation
#define PM_AVERAGE_BATTERY_NB               32

// The index type of battery level array is UINT8
#if (PM_AVERAGE_BATTERY_NB > 255)
#error "Invalid PM_AVERAGE_BATTERY_NB"
#endif

// The median average will contain (1 - 2/PM_BATTERY_MEDIAN_FILTER_FACTOR) samples,
// or, filter 2/PM_BATTERY_MEDIAN_FILTER_FACTOR samples.
// If PM_BATTERY_MEDIAN_FILTER_FACTOR is 0, the s/w will get a real median value
// instead of a median average.
#define PM_BATTERY_MEDIAN_FILTER_FACTOR     0

#if (PM_BATTERY_MEDIAN_FILTER_FACTOR < 3)
#undef PM_BATTERY_MEDIAN_FILTER_FACTOR
#define PM_BATTERY_MEDIAN_FILTER_FACTOR     0
#endif

#ifdef PM_SLOW_BATTERY_MONITOR
// To save power
#define PM_BAT_MONITOR_IDLE_TIME            60 * HAL_TICK1S
#define PM_BAT_MONITOR_IDLE_LOWBAT_TIME     20 * HAL_TICK1S
#define PM_BAT_MONITOR_IDLE_CRITICAL_TIME   5  * HAL_TICK1S
#define PM_BAT_MONITOR_CALL_TIME            5  * HAL_TICK1S
#elif defined(PMD_TWO_BATTARY)
#define PM_BAT_MONITOR_IDLE_TIME            10 * HAL_TICK1S
#define PM_BAT_MONITOR_IDLE_LOWBAT_TIME     5  * HAL_TICK1S
#define PM_BAT_MONITOR_IDLE_CRITICAL_TIME   5  * HAL_TICK1S
#define PM_BAT_MONITOR_CALL_TIME            5  * HAL_TICK1S
#else // !PM_SLOW_BATTERY_MONITOR && !PMD_TWO_BATTARY
#define PM_BAT_MONITOR_IDLE_TIME            4  * HAL_TICK1S
#define PM_BAT_MONITOR_IDLE_LOWBAT_TIME     2  * HAL_TICK1S
#define PM_BAT_MONITOR_IDLE_CRITICAL_TIME   2  * HAL_TICK1S
#define PM_BAT_MONITOR_CALL_TIME            2  * HAL_TICK1S
#endif // !PM_SLOW_BATTERY_MONITOR && !PMD_TWO_BATTARY

/// delay in polling mode between earpiece detection during IDLE but active
#define PM_EARPIECE_DET_POLLING_IDLE_TIME   1 * HAL_TICK1S
/// delay in interrupt mode between earpiece detection during IDLE but active
#ifdef CHIP_HAS_AP
#define PM_EARPIECE_DET_INT_IDLE_TIME       3 * HAL_TICK1S / 20
#else
#define PM_EARPIECE_DET_INT_IDLE_TIME       1 * HAL_TICK1S
#endif
/// delay between earpiece detection during CALL
#define PM_EARPIECE_DET_CALL_TIME           2 * HAL_TICK1S / 10

#define PM_EARPIECE_DET_MAX_TIME            0xFFFFFFFF

#ifdef PM_EARPIECE_DET_DEBOUNCE
// A quick manual earpiece key press lasts for about 120 ms
#define PM_EARPIECE_DET_DEBOUNCE_TIME       3 * HAL_TICK1S / 100
#define PM_EARPIECE_DET_DEBOUNCE_VALUE_MIN  200
#define PM_EARPIECE_DET_DEBOUNCE_VALUE_MAX  500

#if ((PM_EARPIECE_DET_DEBOUNCE_TIME) > (PM_EARPIECE_DET_CALL_TIME))
#error "Earpiece GPADC debounce time should NOT be greater than detection period during a call!"
#endif
#endif // PM_EARPIECE_DET_DEBOUNCE

#define PM_STABLE_BAT_EMA_FACTOR            50

PRIVATE UINT32 g_EarPieceDet_INT_Idle_Time = PM_EARPIECE_DET_INT_IDLE_TIME;

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// PM_BATTERY_STABLE_T
// -----------------------------------------------------------------------------
/// This structure groups the variables needed for battery stable value
/// using averaging (to flatten GPADC variation) and hysteresis (to avoid
/// blinking bar)
// =============================================================================
typedef struct
{
    /// sum the battery measures
    UINT32 sum;
    /// last instant value
    UINT16 instant;
    /// calculated mean level
    UINT16 avg;
    /// hysteresis stable value
    UINT16 stable;
    /// reported percent
    UINT8 percent;
    /// array of measures to maintaint the mean
    UINT16 levels[PM_AVERAGE_BATTERY_NB];
    /// current index in the array
    UINT8 idx;
    /// number of measures (for init)
    UINT8 nb;
    /// array of sorted measure indices
    UINT8 sortedIdx[PM_AVERAGE_BATTERY_NB];

} PM_BATTERY_STABLE_T;

// =============================================================================
// PM_EARDETECT_T
// -----------------------------------------------------------------------------
/// This structure groups the variables needed for detect the earphone
// =============================================================================
typedef struct
{
    UINT8 mode;
    BOOL earon;
    BOOL stateStable;
    BOOL gpadcMonStarted;
    BOOL gpadcMonDisabled;
#ifdef PM_EARPIECE_DET_DEBOUNCE
    BOOL debouncing;
    UINT16 prevValue;
#endif // PM_EARPIECE_DET_DEBOUNCE
} PM_EAR_DETECT_T;

typedef enum
{
    PM_INIT_STABLE_CHECK_NA,
    PM_INIT_STABLE_CHECK_START,
    PM_INIT_STABLE_CHECK_DONE,
} PM_INIT_STABLE_CHECK_STATE_T;



// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

PRIVATE PM_EAR_DETECT_T g_pmEarDetect =
{
    .mode = 0,
    .earon = FALSE,
    .stateStable = FALSE,
    .gpadcMonStarted = FALSE,
    .gpadcMonDisabled = FALSE,
#ifdef PM_EARPIECE_DET_DEBOUNCE
    .debouncing = FALSE,
    .prevValue = 0,
#endif // PM_EARPIECE_DET_DEBOUNCE
};

// =============================================================================
// g_pmBatteryStable
// -----------------------------------------------------------------------------
/// This structure groups the variables needed fot battery averaging
// =============================================================================
PRIVATE PM_BATTERY_STABLE_T g_pmBatteryStable =
{
    .idx     = 0,
    .nb      = 0,
    .avg     = 0,
    .instant = 0,
    .levels  = { [0 ... PM_AVERAGE_BATTERY_NB - 1] = 0 },
    .percent = 0
};

#ifdef PMD_TWO_BATTARY
PRIVATE PM_BATTERY_STABLE_T g_pmBatteryStable_sub =
{
    .idx     = 0,
    .nb      = 0,
    .avg     = 0,
    .instant = 0,
    .levels  = { [0 ... PM_AVERAGE_BATTERY_NB - 1] = 0 },
    .percent = 0
};

PRIVATE UINT16 g_main_battery_detector = PM_AVERAGE_BATTERY_NB;
PRIVATE BOOL g_start_quick_detect_flag = FALSE;
#endif

PRIVATE UINT32 Time_To_MMI_Evt      = 0;

/// battery monitoring perriod
PRIVATE UINT32 g_pmBatMonTime     = PM_BAT_MONITOR_IDLE_CRITICAL_TIME;
PRIVATE UINT32 g_pmBatMonLastDate = 0;
/// earpiece detect perriod
PRIVATE UINT32 g_pmEarPieceDetTime     = PM_EARPIECE_DET_MAX_TIME;
PRIVATE UINT32 g_pmEarPieceDetLastDate = 0;

#if ((PM_VMIC_POWER_POLICY == 1) || (PM_VMIC_POWER_POLICY == 2))
PRIVATE BOOL g_pmVMICActive = FALSE;
#endif

// added private funtion to do the job with pmd
PRIVATE UINT8 g_pmPowerOnVoltPercent = 0;
PRIVATE UINT8 g_pmPowerDownVoltPercent = 0;
#ifdef PMD_TWO_BATTARY
PRIVATE UINT8 g_pmSubPowerOnVoltPercent = 0;
#endif

PRIVATE BOOL g_pmFilterStableBattVolt = FALSE;
PRIVATE UINT32 g_pmInitStableCheckTime;
PRIVATE PM_INIT_STABLE_CHECK_STATE_T g_pmInitStableCheckState =
    PM_INIT_STABLE_CHECK_NA;

#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE) // 8808 or 8809
PRIVATE BOOL g_pmScreenAntiFlickerEn = FALSE;
#endif

PRIVATE PMD_CHARGER_STATUS_T g_pm_oldStatus = PMD_CHARGER_UNKNOWN;


/// @param status to convert
/// @return Bcs value
PRIVATE UINT8 pm_PmdChargerStatus2Bcs(PMD_CHARGER_STATUS_T status)
{
    UINT8 bcs = PM_CHARGER_CONNECTED;

    switch (status)
    {
        case PMD_CHARGER_UNKNOWN:
        case PMD_CHARGER_UNPLUGGED:
            bcs = PM_CHARGER_DISCONNECTED;
            break;
        case PMD_CHARGER_PLUGGED:
            bcs = PM_CHARGER_CONNECTED;
            break;
        case PMD_CHARGER_PRECHARGE:
        case PMD_CHARGER_FAST_CHARGE:
        case PMD_CHARGER_PULSED_CHARGE:
        case PMD_CHARGER_PULSED_CHARGE_WAIT:
            bcs = PM_CHARGER_CHRGING;
            break;
        case PMD_CHARGER_FULL_CHARGE:
            bcs = PM_CHARGER_FINISHED;
            break;
        case PMD_CHARGER_TEMPERATURE_ERROR:
            bcs = PM_CHARGER_ERROR_TEMPERATURE;
            break;
        case PMD_CHARGER_VOLTAGE_ERROR:
            bcs = PM_CHARGER_ERROR_VOLTAGE;
            break;
        case PMD_CHARGER_ERROR:
        case PMD_CHARGER_TIMED_OUT:
        default:
            bcs = PM_CHARGER_ERROR_UNKNOWN;
            break;
    }
    return bcs;
}

/// @param batLevel battery level input to convert
/// @param pBcl pointer on Bcl to update
/// @param pBatState pointer on BatState to load
/// @return FALSE when invalid parameter
PRIVATE BOOL pm_BatteryLevel2State(UINT16 batLevel, UINT8 *pBcl, UINT16 *pBatState)
{
    if (pBcl == NULL || pBatState == NULL)
    {
        return FALSE;
    }
    if (batLevel == PMD_BAD_VALUE)
    {
        *pBcl      = 0;
        *pBatState = PM_BATTERY_STATE_UNKNOWN;
    }
    else
    {
        /// @todo: not hardcode levels
        if (*pBcl <= g_pmPowerDownVoltPercent)
        {
            *pBatState = PM_BATTERY_STATE_SHUTDOWN;
        }
        else if (*pBcl <= (g_pmPowerDownVoltPercent + 5) )
        {
            *pBatState = PM_BATTERY_STATE_CRITICAL;
        }
        else if (*pBcl <= (g_pmPowerDownVoltPercent + 10))
        {
            *pBatState = PM_BATTERY_STATE_LOW;
        }
        else
        {
            *pBatState = PM_BATTERY_STATE_NORMAL;
        }
    }
    return TRUE;
}

PRIVATE VOID pm_UpdateAverageBatteryLevel(PM_BATTERY_STABLE_T *stable)
{
    INT32 i, rank, newRank;
    UINT16 batLevel = stable->instant;

    stable->levels[stable->idx] = batLevel;
    if (stable->nb < PM_AVERAGE_BATTERY_NB)
    {
        stable->nb++;
    }

    for (i = 0; i < PM_AVERAGE_BATTERY_NB; i++)
    {
        if (stable->sortedIdx[i] == stable->idx)
        {
            break;
        }
    }
    if (i == PM_AVERAGE_BATTERY_NB)
    {
        __assert("Corrupted stable->sortedIdx");
    }
    rank = i;

    // Update sortedIdx[]
    if (rank > 0 &&
            stable->levels[stable->sortedIdx[rank - 1]] < batLevel)
    {
        if (rank == 1)
        {
            newRank = 0;
        }
        else
        {
            for (i = rank - 2; i >= 0; i--)
            {
                if (stable->levels[stable->sortedIdx[i]] >= batLevel)
                {
                    break;
                }
            }
            newRank = i + 1;
        }
        for (i = rank; i > newRank; i--)
        {
            stable->sortedIdx[i] = stable->sortedIdx[i - 1];
        }
        stable->sortedIdx[newRank] = stable->idx;
    }
    else if (rank < (stable->nb - 1) &&
             batLevel < stable->levels[stable->sortedIdx[rank + 1]])
    {
        if (rank == stable->nb - 2)
        {
            newRank = stable->nb - 1;
        }
        else
        {
            for (i = rank + 2; i < stable->nb; i++)
            {
                if (batLevel >= stable->levels[stable->sortedIdx[i]])
                {
                    break;
                }
            }
            newRank = i - 1;
        }
        for (i = rank; i < newRank; i++)
        {
            stable->sortedIdx[i] = stable->sortedIdx[i + 1];
        }
        stable->sortedIdx[newRank] = stable->idx;
    }

    // Calculate the average
#if (PM_BATTERY_MEDIAN_FILTER_FACTOR)
    stable->sum = 0;
    if (stable->nb == PM_AVERAGE_BATTERY_NB)
    {
        for (i = PM_AVERAGE_BATTERY_NB / PM_BATTERY_MEDIAN_FILTER_FACTOR;
                i < PM_AVERAGE_BATTERY_NB - PM_AVERAGE_BATTERY_NB / PM_BATTERY_MEDIAN_FILTER_FACTOR;
                i++)
        {
            stable->sum += stable->levels[stable->sortedIdx[i]];
        }
        stable->avg = stable->sum /
                      (PM_AVERAGE_BATTERY_NB - 2 * (PM_AVERAGE_BATTERY_NB / PM_BATTERY_MEDIAN_FILTER_FACTOR));
    }
    else
    {
        for (i = stable->nb / PM_BATTERY_MEDIAN_FILTER_FACTOR;
                i < stable->nb - stable->nb / PM_BATTERY_MEDIAN_FILTER_FACTOR;
                i++)
        {
            stable->sum += stable->levels[stable->sortedIdx[i]];
        }
        stable->avg = stable->sum /
                      (stable->nb - 2 * (stable->nb / PM_BATTERY_MEDIAN_FILTER_FACTOR));
    }
#else
    if (stable->nb & 1)
    {
        stable->avg = stable->levels[stable->sortedIdx[stable->nb / 2]];
    }
    else
    {
        stable->avg = (stable->levels[stable->sortedIdx[stable->nb / 2 - 1]] +
                       stable->levels[stable->sortedIdx[stable->nb / 2]]) / 2;
    }
#endif

    stable->idx++;
    if (stable->idx >= PM_AVERAGE_BATTERY_NB)
    {
        stable->idx = 0;
    }
}

PRIVATE VOID pm_InitStableBatteryLevel(VOID)
{
    int i;
    BOOL ok;
    UINT8 bcl;

#ifdef PMD_TWO_BATTARY
    g_pmBatteryStable_sub.nb = 0;
    g_pmBatteryStable_sub.idx = 0;
    for (i = 0; i < PM_AVERAGE_BATTERY_NB; i++)
    {
        g_pmBatteryStable_sub.levels[i] = 0;
        g_pmBatteryStable_sub.sortedIdx[i] = i;
    }
#endif

    g_pmBatteryStable.nb = 0;
    g_pmBatteryStable.idx = 0;
    for (i = 0; i < PM_AVERAGE_BATTERY_NB; i++)
    {
        g_pmBatteryStable.levels[i] = 0;
        g_pmBatteryStable.sortedIdx[i] = i;
    }

    ok = FALSE;
    while (!ok)
    {
#ifdef PMD_TWO_BATTARY
        pmd_SetCurrentBattery(PMD_BATTERY_SUB);
        g_pmBatteryStable_sub.instant = pmd_GetBatteryLevel(&bcl);
        if (g_pmBatteryStable_sub.instant != PMD_BAD_VALUE)
        {
            pm_UpdateAverageBatteryLevel(&g_pmBatteryStable_sub);
            g_pmBatteryStable_sub.stable = g_pmBatteryStable_sub.instant;
            g_pmBatteryStable_sub.percent = bcl;
            g_pmBatteryStable_sub.percentAdjTime = now;
            ok = TRUE;
        }

        pmd_SetCurrentBattery(PMD_BATTERY_MAIN);
        g_pmBatteryStable.instant = pmd_GetBatteryLevel(&bcl);
        if (g_pmBatteryStable.instant != PMD_BAD_VALUE)
        {
            pm_UpdateAverageBatteryLevel(&g_pmBatteryStable);
            g_pmBatteryStable.stable = g_pmBatteryStable.instant;
            g_pmBatteryStable.percent = bcl;
            ok = TRUE;
        }
        else
        {
            pmd_SetCurrentBattery(PMD_BATTERY_SUB);
        }
#else
        g_pmBatteryStable.instant = pmd_GetBatteryLevel(&bcl);
        if (g_pmBatteryStable.instant != PMD_BAD_VALUE)
        {
            pm_UpdateAverageBatteryLevel(&g_pmBatteryStable);
            g_pmBatteryStable.stable = g_pmBatteryStable.instant;
            g_pmBatteryStable.percent = bcl;
            ok = TRUE;
        }
#endif
    }
}

PRIVATE INT pm_GetStableBatteryLevel(PMD_BATTERY_TYPE_T battType, UINT8 *pPercent)
{
    UINT8 bcl = 0;
    UINT8 avgPercent;
    UINT16 batLevel = PMD_BAD_VALUE;
    UINT16 filteredLevel;
    UINT8 emaFactor;
    PM_BATTERY_STABLE_T *stable;

#ifdef PMD_TWO_BATTARY
    if (battType == PMD_BATTERY_MAIN)
    {
        stable = &g_pmBatteryStable;
        if (pmd_GetMainStatus() == FALSE)
        {
            return PMD_BAD_VALUE;
        }

        if (pmd_GetCurrentBattery() == PMD_BATTERY_SUB)
        {
            pmd_SetCurrentBattery(PMD_BATTERY_MAIN);
            batLevel = pmd_GetRealTimeBatteryLevel(&bcl);
            pmd_SetCurrentBattery(PMD_BATTERY_SUB);
        }
        else
        {
            batLevel = pmd_GetRealTimeBatteryLevel(&bcl);
        }
    }
    else
    {
        stable = &g_pmBatteryStable_sub;
        if (pmd_GetCurrentBattery() == PMD_BATTERY_MAIN)
        {
            pmd_SetCurrentBattery(PMD_BATTERY_SUB);
            batLevel = pmd_GetRealTimeBatteryLevel(&bcl);
            pmd_SetCurrentBattery(PMD_BATTERY_MAIN);
        }
        else
        {
            batLevel = pmd_GetRealTimeBatteryLevel(&bcl);
        }
    }
#else
    stable = &g_pmBatteryStable;
    batLevel = pmd_GetRealTimeBatteryLevel(&bcl);
#endif

    if (batLevel == PMD_BAD_VALUE)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(1), TSTR("pm-%d: PMD_BAD_VALUE",0x08500001), battType);
        goto _err_exit;
    }

    if (batLevel < 2500)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(1), TSTR("pm-%d: PMD_BAD_VALUE too small",0x08500002), battType);
        goto _err_exit;
    }

    if (stable->avg > batLevel)
    {
        if ((stable->avg - batLevel) > PM_MAX_VALID_BATTERY_STEP)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(2), TSTR("pm-%d: too small %dmV",0x08500003), battType, batLevel);
#ifdef PMD_TWO_BATTARY
            if (battType == PMD_BATTERY_SUB)
#endif
            {
                goto _err_exit;
            }
        }
    }
    PMD_CHARGER_STATUS_T  chargerStatus = pmd_GetChargerStatus();
    if(chargerStatus > PMD_CHARGER_UNPLUGGED
            &&batLevel < stable -> stable)
    {
        batLevel = stable -> stable;
    }
    stable->instant = batLevel;
    pm_UpdateAverageBatteryLevel(stable);
    avgPercent = pmd_BatteryLevelToPercent(stable->avg);

    // Try to report a series of monotonously increasing/decreasing levels
    if (!g_pmFilterStableBattVolt)
    {
        stable->stable = stable->avg;
    }
    else
    {
        filteredLevel = stable->stable;

        PMD_CHARGER_STATUS_T status = pmd_GetChargerStatus();
        if (status == PMD_CHARGER_UNPLUGGED)
        {
            // Not in charge
            // The battery levels should be monotonously decreasing,
            // except for DC power supply
            if (stable->stable > stable->avg)
            {
                filteredLevel = stable->avg;
            }
            else if (stable->stable + 200 < stable->avg)
            {
                filteredLevel = stable->avg;
                g_pmFilterStableBattVolt = FALSE;
            }
        }
        else
        {
            // In charge
            // If battery has reached full before, show the actual average voltage;
            // otherwise try to filter the voltages smaller than previous ones
            if (status == PMD_CHARGER_FULL_CHARGE ||
                    stable->stable < stable->avg)
            {
                filteredLevel = stable->avg;
            }
            else if (stable->stable > stable->avg + 50)
            {
                filteredLevel = stable->avg;
                g_pmFilterStableBattVolt = FALSE;
            }
        }

        // Calculate the exponential moving average (EMA) as the stable value
        emaFactor = PM_STABLE_BAT_EMA_FACTOR;
        if (filteredLevel > stable->stable)
        {
            stable->stable += (filteredLevel - stable->stable +
                               (emaFactor - 1)) / emaFactor;
        }
        else if (filteredLevel < stable->stable)
        {
            stable->stable += (filteredLevel - stable->stable -
                               (emaFactor - 1)) / emaFactor;
        }
    }

    // Calculate percentage
    stable->percent = pmd_BatteryLevelToPercent(stable->stable);
#ifdef __BT_PROFILE_BQB__
    {
        extern void rdabt_set_current_battery_level(U8 num);
        rdabt_set_current_battery_level(stable->percent);
    }
#endif

#ifdef PMD_TWO_BATTARY
    if (battType == PMD_BATTERY_SUB)
#endif
    {
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE) // 8808 or 8809
        if (stable->stable <= pmd_GetScreenAntiFlickerVolt(TRUE))
        {
            if (!g_pmScreenAntiFlickerEn)
            {
                g_pmScreenAntiFlickerEn = TRUE;
                pmd_EnableScreenAntiFlicker(TRUE);
            }
        }
        else if (stable->stable > pmd_GetScreenAntiFlickerVolt(FALSE))
        {
            if (g_pmScreenAntiFlickerEn)
            {
                g_pmScreenAntiFlickerEn = FALSE;
                pmd_EnableScreenAntiFlicker(FALSE);
            }
        }
        xcv_SetStableVolt(stable->stable);
#endif // 8808 or 8809

    }

#ifdef PMD_TWO_BATTARY
    if (battType == PMD_BATTERY_MAIN)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(2), TSTR("pm: flag=%d, count=%d",0x08500004),
                  g_start_quick_detect_flag, g_main_battery_detector);
        if (g_start_quick_detect_flag)
        {
            if (g_main_battery_detector <= (PM_AVERAGE_BATTERY_NB - 3))
            {
                g_main_battery_detector++;
            }
        }
    }
#endif

    /*CSW_TRACE(CFW_PM_TS_ID, "pm-%d: instant: %dmV, %d%%; mean: %dmV, %d%%; stable: %dmV, %d%%",
              battType, batLevel, bcl, stable->avg, avgPercent,
              stable->stable, stable->percent);*/

_exit:
    // Return percentage
    if (pPercent != NULL)
    {
        *pPercent = stable->percent;
    }

    return stable->stable;

_err_exit:
    CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(3), TSTR("pm-%d: old stable: %dmV, %d%%",0x08500005),
              battType, stable->stable, stable->percent);

    goto _exit;
}

#if (PM_VMIC_POWER_POLICY != 0)
/// callback to sync with paging perriod
PRIVATE VOID pm_lpsCallback(VOID)
{
    COS_EVENT ev;
    ev.nEventId = EV_CSW_PRV_LPS_WAKEUP;
    ev.nParam1  = 0;
    ev.nParam2  = 0;
    ev.nParam3  = 0;
    COS_SendEvent(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}
#endif

/* send an event to update the battery status */
BOOL pm_BattChargeRemind(PMD_CHARGER_STATUS_T status, BOOL fromChargerHandler)
{
    UINT8  bcl      = 0;
    UINT8  bcs      = 0;
    UINT16 batState = 0;
    UINT16 batLevel = 0;
#ifndef CHIP_HAS_AP
    COS_EVENT ev;
#endif

#ifdef PMD_TWO_BATTARY
    if (fromChargerHandler)
    {
        if(pmd_GetCurrentBattery() == PMD_BATTERY_SUB)
        {
            batLevel = g_pmBatteryStable_sub.stable;
            bcl      = g_pmBatteryStable_sub.percent;
            CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(1), TSTR("pm: Remind sub bcl=%d",0x08500006), bcl);
        }
        else
        {
            batLevel = g_pmBatteryStable.stable;
            bcl      = g_pmBatteryStable.percent;
            CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(1), TSTR("pm: Remind main bcl=%d",0x08500007), bcl);
        }
        // If charger is plugged out when the battery level is too low,
        // shutdown the system immediately to avoid potential crashes.
        if (status == PMD_CHARGER_UNPLUGGED && g_pm_oldStatus >= PMD_CHARGER_PLUGGED)
        {
            if (g_pmBatteryStable.percent <= g_pmPowerDownVoltPercent &&
                    g_pmBatteryStable_sub.percent <= g_pmPowerDownVoltPercent)
            {
                hal_HstSendEvent(0xdeadba44);
                hal_SysShutdown();
            }
        }
    }
    else
    {
        UINT8 bcl_sub = 0;
        batLevel = pm_GetStableBatteryLevel(PMD_BATTERY_MAIN, &bcl);
        UINT16 batLevel_sub = pm_GetStableBatteryLevel(PMD_BATTERY_SUB, &bcl_sub);
        if(pmd_GetCurrentBattery() == PMD_BATTERY_SUB)
        {
            batLevel = batLevel_sub;
            bcl = bcl_sub;
        }
    }
#else
    if (fromChargerHandler)
    {
        batLevel = g_pmBatteryStable.stable;
        bcl      = g_pmBatteryStable.percent;
        // If charger is plugged out when the battery level is too low,
        // shutdown the system immediately to avoid potential crashes.
        if (status == PMD_CHARGER_UNPLUGGED && g_pm_oldStatus >= PMD_CHARGER_PLUGGED)
        {
            if (g_pmBatteryStable.percent <= g_pmPowerDownVoltPercent)
            {
                hal_HstSendEvent(0xdeadba44);
                hal_SysShutdown();
            }
        }
    }
    else
    {
        batLevel = pm_GetStableBatteryLevel(PMD_BATTERY_MAIN, &bcl);
#ifdef  TXPOWER_TEMPERATURE_OFFSET
        pmd_GetGpadcTemperatureSW();
#endif

    }
#endif

    if (batLevel == PMD_BAD_VALUE)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(0), TSTR("pm: Remind PMD_BAD_VALUE",0x08500008));
        return FALSE;
    }

    // Stable voltage check at startup
    if (g_pmInitStableCheckState != PM_INIT_STABLE_CHECK_DONE)
    {
        // If charger is plugged out within 20 seconds since s/w starts,
        // the stable voltage should be re-calculated at once (skipping
        // the filter mechanism), for the charger detection might be
        // false and there is no charger from the very beginning.

        UINT32 now = hal_TimGetUpTime();

        if (g_pmInitStableCheckState == PM_INIT_STABLE_CHECK_START)
        {
            if (now - g_pmInitStableCheckTime > 15 * HAL_TICK1S)
            {
                g_pmFilterStableBattVolt = TRUE;
                g_pmInitStableCheckState = PM_INIT_STABLE_CHECK_DONE;
            }
        }
        else
        {
            if (now - g_pmInitStableCheckTime > 20 * HAL_TICK1S)
            {
                g_pmInitStableCheckState = PM_INIT_STABLE_CHECK_DONE;
            }
        }

        if (g_pmInitStableCheckState == PM_INIT_STABLE_CHECK_NA)
        {
            if (g_pm_oldStatus > PMD_CHARGER_UNPLUGGED &&
                    status == PMD_CHARGER_UNPLUGGED)
            {
                g_pmInitStableCheckState = PM_INIT_STABLE_CHECK_START;
                g_pmInitStableCheckTime = now;
                g_pmFilterStableBattVolt = FALSE;
            }
        }
    }

    if (g_pm_oldStatus != status)
    {
        // always go to the step 'connected' before any charge step
        if (g_pm_oldStatus <= PMD_CHARGER_UNPLUGGED &&
                status > PMD_CHARGER_UNPLUGGED)
        {
            status = PMD_CHARGER_PLUGGED;
        }
        g_pm_oldStatus = status;

        if (g_pmInitStableCheckState != PM_INIT_STABLE_CHECK_START)
        {
            g_pmFilterStableBattVolt = TRUE;
        }
    }

    bcs = pm_PmdChargerStatus2Bcs(status);
    pm_BatteryLevel2State(batLevel, &bcl, &batState);

    // when no measure valid for battery
    if (batState == PM_BATTERY_STATE_UNKNOWN)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(0), TSTR("pm: Remind PM_BATTERY_STATE_UNKNOWN",0x08500009));
        return FALSE;
    }

#ifdef CHIP_HAS_AP
    syscmds_NotifyBatteryStatus(bcl, bcs, batLevel);
#else
    ev.nEventId = EV_PM_BC_IND;
    ev.nParam1  = MAKEUINT32(bcl, bcs);
    //ev.nParam2  = MAKEUINT32(batState, 0);
    ev.nParam2  = MAKEUINT32(PM_BATTERY_STATE_NORMAL, 0);
    ev.nParam3  = MAKEUINT32(batLevel, 0);
    DM_SendPMMessage(&ev);
#endif

    CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(4),
              TSTR("pm: Remind Bcs=%d, Bcl=%d%%, batState=%d, stable_volt=%dmV",0x0850000a),
              bcs, bcl, batState, batLevel);

    return TRUE;
}

VOID pm_ChargerHandler(PMD_CHARGER_STATUS_T status)
{
//#if (CSW_EXTENDED_API_PMD_USB == 1) && (CHIP_HAS_USB == 1)
#if (((CSW_EXTENDED_API_PMD_USB == 1) && (CHIP_HAS_USB == 1))||(defined(AT_USB_MODEM_NO_MMMI)))

    if(status == PMD_CHARGER_PLUGGED)
    {

        uctls_ChangeMode(UCTLS_ID_2);
        uctls_ChargerStatus(UCTLS_CHARGER_STATUS_CONNECTED);
    }
    else if (PMD_CHARGER_UNPLUGGED == status)
    {
        uctls_ChargerStatus(UCTLS_CHARGER_STATUS_DISCONNECTED);
    }
#endif

    pm_BattChargeRemind(status, TRUE);
}

PRIVATE VOID pm_EarPieceDetect(BOOL fromLowPowerEvent)
{
    CFW_COMM_MODE nCommMode = CFW_DISABLE_COMM;
    int i;

    CONST UINT32 vmicChannel = pmd_GetEarpieceGpadcChannel();

#if ( CHIP_ASIC_ID == CHIP_ASIC_ID_8850E)
    //in 8850e chip, the reference voltage the pwoer too solow(about 40ms), so always on in flight modem
    if(fromLowPowerEvent)
    {
        for( i =0; i<CFW_SIM_COUNT; i++)
        {
            CFW_GetCommEX(&nCommMode,i);
            if( CFW_DISABLE_COMM != nCommMode ) break;
        }
        //if it is flight modem, don't entry monitor,havent process monitor timer befor lpscallback
        if(i ==CFW_SIM_COUNT)   return;
    }
#endif

    if (g_pmEarDetect.mode != PMD_EAR_DETECT_POLLING)
    {
#ifdef ABB_HP_DETECT
        if (g_pmVMICActive && !g_pmEarDetect.earon)
#else
        if (g_pmVMICActive &&
                (!g_pmEarDetect.earon || !g_pmEarDetect.gpadcMonStarted))
#endif
        {

            // Disable gpadc channel
            hal_AnaGpadcClose(vmicChannel);
            // Disable ear piece power
            pmd_EnablePower(PMD_POWER_EARPIECE, FALSE);
            // Update status
            g_pmVMICActive = FALSE;

            return;
        }
    }

#if (PM_VMIC_POWER_POLICY == 0)
    // 0: VMIC always on, direct measure
    if (g_pmEarDetect.gpadcMonStarted && g_pmEarDetect.stateStable)
    {
        HAL_ANA_GPADC_MV_T ret = 0;
        COS_EVENT ev;
        ret = hal_AnaGpadcGet(vmicChannel);
        if (ret != HAL_ANA_GPADC_BAD_VALUE)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(1), TSTR("0 earpiece detect: measure = %d",0x0850000b), ret);
            ev.nEventId = EV_DM_GPADC_IND;
            ev.nParam1  = ret;
            ev.nParam2  = g_pmEarDetect.earon;
            ev.nParam3  = g_pmEarDetect.mode;
            COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
            CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(2), TSTR("pm: POLICY 0 send ear volt to mmi = %d,onstatus =%d",0x0850000c), ret, g_pmEarDetect.earon);
        }
    }
#elif ((PM_VMIC_POWER_POLICY == 1) || (PM_VMIC_POWER_POLICY == 2))
    // 1: off during low power
    // 2: every other paging during during low power

    if (
#if (PM_VMIC_POWER_POLICY == 1)
#ifdef VMIC_POWER_ON_WORKAROUND
        // Get VMIC voltage only if VMIC is valid
        hal_AudVMicValid() &&
#endif
#endif
        g_pmEarDetect.gpadcMonStarted &&
        g_pmVMICActive &&
        g_pmEarDetect.stateStable
    )
    {
        HAL_ANA_GPADC_MV_T ret = 0;
        ret = hal_AnaGpadcGet(vmicChannel);

        if (ret != HAL_ANA_GPADC_BAD_VALUE)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(1), TSTR("1 earpiece detect: measure = %d",0x0850000d), ret);
#ifdef PM_EARPIECE_DET_DEBOUNCE
            if (g_pmEarDetect.debouncing)
            {
                UINT32 abs;
                abs = (ret > g_pmEarDetect.prevValue) ?
                      (ret - g_pmEarDetect.prevValue) :
                      (g_pmEarDetect.prevValue - ret);
                if (abs <= PM_EARPIECE_DET_DEBOUNCE_VALUE_MIN)
                {
                    ret = (ret + g_pmEarDetect.prevValue) / 2;
                    g_pmEarDetect.debouncing = FALSE;
                }
                else if (abs >= PM_EARPIECE_DET_DEBOUNCE_VALUE_MAX)
                {
                   if (ret > g_pmEarDetect.prevValue)
				   {
                       ret = g_pmEarDetect.prevValue;
                   }
                   g_pmEarDetect.debouncing = FALSE;
                }
                else
                {
                    g_pmEarDetect.prevValue = ret;
                }
            }
            else
            {
                g_pmEarDetect.debouncing = TRUE;
                g_pmEarDetect.prevValue = ret;
            }
            // If debounced
            if (!g_pmEarDetect.debouncing)
#endif // PM_EARPIECE_DET_DEBOUNCE
            {
#ifdef CHIP_HAS_AP
                syscmds_NotifyEarPieceKeyStatus(ret);
#else
                COS_EVENT ev;
                ev.nEventId = EV_DM_GPADC_IND;
                ev.nParam1  = ret;
                ev.nParam2  = g_pmEarDetect.earon;
                ev.nParam3  = PMD_EAR_GPADC_IND;
                COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
                CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(2), TSTR("pm: POLICY 1 send ear volt to mmi = %d,onstatus =%d",0x0850000e), ret, g_pmEarDetect.earon);
#endif
            }
        }
    }
#ifdef PM_EARPIECE_DET_DEBOUNCE
    else
    {
        g_pmEarDetect.debouncing = FALSE;
    }
#endif // PM_EARPIECE_DET_DEBOUNCE

    if (fromLowPowerEvent == TRUE)
    {
#if (PM_VMIC_POWER_POLICY == 1)
        // recieved low power event, disable VMIC if needed

#if defined(ABB_HP_DETECT) && defined(ABB_HP_DETECT_IN_LP)
        if (g_pmVMICActive &&
                (
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
                    (hal_SysGetChipId(HAL_SYS_CHIP_METAL_ID) & 0xff) < HAL_SYS_CHIP_METAL_ID_U06 ||
#endif
                    !hal_AudCheckUsrFMOrLineIN()
                )
           )
#else
        if (g_pmVMICActive)
#endif
        {
            // disable gpadc channel
            hal_AnaGpadcClose(vmicChannel);
            // disable ear piece power
            pmd_EnablePower(PMD_POWER_EARPIECE, FALSE);
            // update status
            g_pmVMICActive = FALSE;
        }
#else
        // recieved low power event, toggle VMIC
        if (g_pmVMICActive)
        {
            // disable gpadc channel
            hal_AnaGpadcClose(vmicChannel);
            // disable ear piece power
            pmd_EnablePower(PMD_POWER_EARPIECE, FALSE);
            // update status
            g_pmVMICActive = FALSE;
        }
        else
        {
            // set high Vmic for earpiece detect. use the dedicated earpiece ressource
            pmd_EnablePower(PMD_POWER_EARPIECE, TRUE);
            // update status
            g_pmVMICActive = TRUE;
            // ear piece is connected to GPADC channel 1 detection
            hal_AnaGpadcOpen(vmicChannel, HAL_ANA_GPADC_ATP_100MS);
        }
#endif
    }
    else
    {

        // recieved timer, enable VMIC if needed
        if (!g_pmVMICActive)
        {

            // set high Vmic for earpiece detect. use the dedicated earpiece ressource
            pmd_EnablePower(PMD_POWER_EARPIECE, TRUE);
            // update status
            g_pmVMICActive = TRUE;
            // ear piece is connected to GPADC channel 1 detection
            hal_AnaGpadcOpen(vmicChannel, HAL_ANA_GPADC_ATP_100MS);
        }
    }
#endif
}

#ifdef PMD_TWO_BATTARY
#define PM_BATTERY_DETECT_INTERVAL 3700
#else
#define PM_BATTERY_DETECT_INTERVAL 3620
#endif

#define PM_INCREASE_FREQ_DETECT

#ifdef PM_INCREASE_FREQ_DETECT
#define PM_BATTERY_DETECT_INTERVAL_CHARGING  3800
#endif

void pm_Set_EarPieceDetTime(UINT32 ear_piece_det_Time)
{
    g_EarPieceDet_INT_Idle_Time = ear_piece_det_Time;
}

PRIVATE VOID pm_doMonitoring(BOOL fromLowPowerEvent)
{
    UINT32 now;
    // timer are set to the shorter time / paging event occurs about
    // every 500ms, 1s or 2s
    // so we must check elapsed time between each actual measuring
    // to avoid doing too much measures.
    now = hal_TimGetUpTime();
    if((now - g_pmBatMonLastDate) >= g_pmBatMonTime)
    {
        // monitor battery & charger and tell mmi
        PMD_CHARGER_STATUS_T status = pmd_GetChargerStatus();
        if (pm_BattChargeRemind(status, FALSE))
        {
            // successful, record it
            g_pmBatMonLastDate = now;
        }
    }

    // g_pmEarPieceDetLastDate and g_pmEarPieceDetTime might be
    // changed in pm_ear_detect_inter().
    // But it causes no harm even if they are inconsistent here,
    // as the ultimate purpose is to run pm_EarPieceDetect().
    if ( fromLowPowerEvent ||
            (now - g_pmEarPieceDetLastDate) >= g_pmEarPieceDetTime )
    {
        g_pmEarPieceDetLastDate = now;

        // earpiece detect and tell mmi
        pm_EarPieceDetect(fromLowPowerEvent);
    }

    UINT32 callStatus = 0x00;
    if (fromLowPowerEvent)
    {
        // called from lp event, assuming not in call then ;)
    }
    else
    {
        for(UINT8 nTmp = CFW_SIM_0; nTmp < CFW_SIM_COUNT; nTmp++)
        {
            callStatus |= CFW_CcGetCallStatus(nTmp);
        }
    }

    // Battery monitoring time
    if (callStatus == 0 /*CC_STATE_NULL*/)
    {
        // not in call
        UINT16 batState;
        UINT16 batLevel = g_pmBatteryStable.instant;
        UINT8  bcl = pmd_BatteryLevelToPercent(batLevel);
        // if battery level is low, use a different time
        // using pm_BatteryLevel2State for criteria
        pm_BatteryLevel2State(batLevel, &bcl, &batState);
        if (batState == PM_BATTERY_STATE_LOW)
        {
            g_pmBatMonTime = PM_BAT_MONITOR_IDLE_LOWBAT_TIME;

        }
        else if (batState == PM_BATTERY_STATE_CRITICAL)
        {
            g_pmBatMonTime = PM_BAT_MONITOR_IDLE_CRITICAL_TIME;
        }
        else if (batState == PM_BATTERY_STATE_SHUTDOWN)
        {
            g_pmBatMonTime = PM_BAT_MONITOR_IDLE_CRITICAL_TIME;
        }
        else
        {
            if (g_pmBatteryStable.stable > PM_BATTERY_DETECT_INTERVAL)
            {
                g_pmBatMonTime = PM_BAT_MONITOR_IDLE_TIME;
            }
            else
            {
                g_pmBatMonTime = PM_BAT_MONITOR_IDLE_LOWBAT_TIME;
            }
#ifdef PM_INCREASE_FREQ_DETECT
            PMD_CHARGER_STATUS_T status;
            status = pmd_GetChargerStatus();
            if (status != PMD_CHARGER_UNPLUGGED)
            {
                if (g_pmBatteryStable.stable < PM_BATTERY_DETECT_INTERVAL_CHARGING)
                {
                    g_pmBatMonTime = PM_BAT_MONITOR_IDLE_LOWBAT_TIME;
                }
            }

#endif
        }
    }
    else
    {
        // in call
        g_pmBatMonTime = PM_BAT_MONITOR_CALL_TIME;
    }

#ifdef PMD_TWO_BATTARY
    if (g_pmBatMonTime > PM_BAT_MONITOR_IDLE_TIME)
    {
        g_pmBatMonTime = PM_BAT_MONITOR_IDLE_TIME;
    }
    if (pmd_GetMainStatus() == FALSE)
    {
        g_main_battery_detector = 0;
        g_start_quick_detect_flag = FALSE;
    }
    else
    {
        g_start_quick_detect_flag = TRUE;
        if (g_main_battery_detector <= (PM_AVERAGE_BATTERY_NB - 3))
        {
            g_pmBatMonTime = MOINTER_TEMP_FOR_PLUGIN_MIAN_BATTERY;
        }
    }
#endif

    // Ear piece detection time
    if (callStatus == 0 /*CC_STATE_NULL*/)
    {
        if (g_pmEarDetect.mode == PMD_EAR_DETECT_POLLING)
        {
            g_pmEarPieceDetTime = PM_EARPIECE_DET_POLLING_IDLE_TIME;
        }
        else
        {
            // Avoid inconsistent states of g_pmEarDetect.earon and g_pmEarPieceDetTime,
            // as they might be changed in pm_ear_detect_inter and PM_StartEarPieceGpadcMonitor
            UINT32 scStatus = hal_SysEnterCriticalSection();
#ifdef ABB_HP_DETECT
            if ( g_pmVMICActive || g_pmEarDetect.earon )
#else
            if ( g_pmVMICActive ||
                    (g_pmEarDetect.earon && g_pmEarDetect.gpadcMonStarted) )
#endif
            {
                // To close VMIC, or open VMIC, or get GPADC value
#ifdef PM_EARPIECE_DET_DEBOUNCE
                if (g_pmEarDetect.debouncing)
                {
                    g_pmEarPieceDetTime = PM_EARPIECE_DET_DEBOUNCE_TIME;
                }
                else
#endif // PM_EARPIECE_DET_DEBOUNCE
                {
                    //  g_pmEarPieceDetTime = PM_EARPIECE_DET_INT_IDLE_TIME;
                    g_pmEarPieceDetTime =g_EarPieceDet_INT_Idle_Time;
                }
            }
            else
            {
                g_pmEarPieceDetTime = PM_EARPIECE_DET_MAX_TIME;
#ifdef PM_EARPIECE_DET_DEBOUNCE
                g_pmEarDetect.debouncing = FALSE;
#endif // PM_EARPIECE_DET_DEBOUNCE
            }
            hal_SysExitCriticalSection(scStatus);
        }
    }
    else
    {
        // in call
        if (g_pmEarDetect.mode == PMD_EAR_DETECT_POLLING)
        {
            g_pmEarPieceDetTime = PM_EARPIECE_DET_CALL_TIME;
        }
        else
        {
            // Avoid inconsistent states of g_pmEarDetect.earon and g_pmEarPieceDetTime,
            // as they might be changed in pm_ear_detect_inter and PM_StartEarPieceGpadcMonitor
            UINT32 scStatus = hal_SysEnterCriticalSection();
#ifdef ABB_HP_DETECT
            if ( g_pmVMICActive || g_pmEarDetect.earon )
#else
            if ( g_pmVMICActive ||
                    (g_pmEarDetect.earon && g_pmEarDetect.gpadcMonStarted) )
#endif
            {
                // To close VMIC, or open VMIC, or get GPADC value
#ifdef PM_EARPIECE_DET_DEBOUNCE
                if (g_pmEarDetect.debouncing)
                {
                    g_pmEarPieceDetTime = PM_EARPIECE_DET_DEBOUNCE_TIME;
                }
                else
#endif // PM_EARPIECE_DET_DEBOUNCE
                {
                    g_pmEarPieceDetTime = PM_EARPIECE_DET_CALL_TIME;
                }
            }
            else
            {
                g_pmEarPieceDetTime = PM_EARPIECE_DET_MAX_TIME;
#ifdef PM_EARPIECE_DET_DEBOUNCE
                g_pmEarDetect.debouncing = FALSE;
#endif // PM_EARPIECE_DET_DEBOUNCE
            }
            hal_SysExitCriticalSection(scStatus);
        }
    }


    Time_To_MMI_Evt = MIN(g_pmBatMonTime, g_pmEarPieceDetTime);
}

VOID PM_StartEarPieceGpadcMonitor(BOOL start)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    BOOL oldMonState = g_pmEarDetect.gpadcMonStarted;

    if (g_pmEarDetect.mode != PMD_EAR_DETECT_POLLING &&
            !g_pmEarDetect.earon)
    {
        // Earpiece is plugged-out
        start = FALSE;
    }
    if (g_pmEarDetect.gpadcMonDisabled)
    {
        // Earpiece GPADC monitoring is disabled
        start = FALSE;
    }

    g_pmEarDetect.gpadcMonStarted = start;

    if (g_pmEarDetect.mode != PMD_EAR_DETECT_POLLING &&
            oldMonState != start)
    {
        // Start monitoring right away
        // on -- start the earpiece key detection
        // off -- power off vmic
        g_pmEarPieceDetTime = PM_EARPIECE_DET_CALL_TIME;
        g_pmEarPieceDetLastDate = hal_TimGetUpTime() - g_pmEarPieceDetTime;

        hal_SysExitCriticalSection(scStatus);

        COS_ForceDeliverTimerEx(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY), PRV_BAL_BATTERY_TIMER2);
    }
    else
    {
        hal_SysExitCriticalSection(scStatus);
    }
}

BOOL PM_GetEarPieceGpadcMonitorStatus(VOID)
{
    return g_pmEarDetect.gpadcMonStarted;
}

VOID PM_DisableEarPieceGpadcMonitor(BOOL disable)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    g_pmEarDetect.gpadcMonDisabled = disable;

    if (disable && g_pmEarDetect.gpadcMonStarted)
    {
        PM_StartEarPieceGpadcMonitor(FALSE);
    }
    else if (!disable && !g_pmEarDetect.gpadcMonStarted)
    {
        PM_StartEarPieceGpadcMonitor(TRUE);
    }

    hal_SysExitCriticalSection(scStatus);
}


/*Description:
*   this function gets:    pBcs[out]:the state of line.
*                          pBcl[out]:percent
*                          pMpc[out]:current,not surpport now.*/
BOOL PM_GetBatteryInfo(UINT8 *pBcs, UINT8 *pBcl, UINT16 *pMpc)
{
    CSW_PROFILE_FUNCTION_ENTER(PM_GetBatteryInfo);
    if (pBcs == NULL || pBcl == NULL || pMpc == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(PM_GetBatteryInfo);
        return FALSE;
    }
    // *pBcs //Battery Charging  of status
    // from coolmmi/adamptation/src/adp_Events.c
    // 0: VBAT_STATUS
    // 1,2 : CHARGER_IN
    // 3: CHARGE_COMPLETE
    // 4: INVALID_CHARGER
    // 5: OVERBATTEMP
    {
        PMD_CHARGER_STATUS_T status = pmd_GetChargerStatus();
        if (status >= PMD_CHARGER_PLUGGED)
        {
            *pBcs = PM_CHARGER_CONNECTED;
        }
        else
        {
            *pBcs = PM_CHARGER_DISCONNECTED;
        }
        // calling pm_PmdChargerStatus2Bcs(status); here whould prevent event to
        // be sent correctly with the intermediate CHARGING_ADAPTER_IS_CONNECTED
        // state before any charge state that the MMI require
        // or the switch/case of pm_PmdChargerStatus2Bcs could be repeated here
    }

    // *pBcl //Battery Charging  of level
    // from coolmmi/adamptation/src/adp_Events.c
    // 0: no measure
    // 1..100: estimated charge %
    UINT16 batLevel;
    UINT16 batState;

    {
        // don't actually read from battery, get value from g_pmBatteryStable
        // if available. So that the mean calculation is done on the regular reporting basis only.
        CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(1), TSTR("g_pmBatteryStable.nb   =%d",0x0850000f), g_pmBatteryStable.nb);
#ifdef PMD_TWO_BATTARY
        if (g_pmBatteryStable.nb == 0)
        {
            if(pmd_GetCurrentBattery() == PMD_BATTERY_SUB)
            {
                pm_GetStableBatteryLevel(PMD_BATTERY_MAIN, pBcl);
                batLevel = pm_GetStableBatteryLevel(PMD_BATTERY_SUB, pBcl);
            }
            else
            {
                batLevel = pm_GetStableBatteryLevel(PMD_BATTERY_MAIN, pBcl);
            }
        }
        else
        {
            if(pmd_GetCurrentBattery() == PMD_BATTERY_SUB)
            {
                batLevel = g_pmBatteryStable_sub.stable;
                *pBcl = g_pmBatteryStable_sub.percent;

            }
            else
            {
                batLevel = g_pmBatteryStable.stable;
                *pBcl = g_pmBatteryStable.percent;

            }
        }
#else
        if (g_pmBatteryStable.nb == 0)
        {
            batLevel = pm_GetStableBatteryLevel(PMD_BATTERY_MAIN, pBcl);
        }
        else
        {
            batLevel = g_pmBatteryStable.stable;
            *pBcl = g_pmBatteryStable.percent;
        }
#endif
        pm_BatteryLevel2State(batLevel, pBcl, &batState);
        CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(3), TSTR("PM_GetBatteryInfo Bcs=%d, Bcl=%d%% ,batLevel =%d mv",0x08500010), *pBcs, *pBcl, batLevel);
    }
    // *pMpc //Power consumption
    // from coolmmi/adamptation/src/adp_Events.c
    *pMpc = batLevel;

    CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(2), TSTR("PM_GetBatteryInfo Bcs=%d, Bcl=%d%% mv",0x08500011), *pBcs, *pBcl);


    CSW_PROFILE_FUNCTION_EXIT(PM_GetBatteryInfo);
    return TRUE;
}

// The following functions are only called from somewhere in platform

BOOL PM_CheckValidPowerOnVoltage(UINT16 nVoltage)
{
    return TRUE;
}

/*Description:
*   in this function, some numbers of this structure are
*not support now.
*Parameter:
*  pBatInfo[out]:
*Return:
*  BOOL type.TRUE is success,FALSE if fail.*/
BOOL PM_GetBatteryState(PM_BATTERY_INFO *pBatInfo)
{
    CSW_PROFILE_FUNCTION_ENTER(PM_GetBatteryState);

    // get voltage info from g_pmBatteryStable structure
    // any measure done yet ?
    if (g_pmBatteryStable.nb == 0)
    {
        // no! just exit
        CSW_PROFILE_FUNCTION_EXIT(PM_GetBatteryState);
        return FALSE;
    }
    pBatInfo->nBatteryRealTimeVoltage = g_pmBatteryStable.instant;

    pBatInfo->nBatteryVoltage = g_pmBatteryStable.stable;

    pBatInfo->nBatteryLifePercent  = g_pmBatteryStable.percent;

    pBatInfo->nBatteryTemperature  = pmd_GetBatteryTemperature();
    // no way to measure:
    pBatInfo->nBatteryCurrent = 0;
#ifdef BATT_FULL_TIME
    pBatInfo->nBatteryLifeTime     = BATT_FULL_TIME * g_pmBatteryStable.percent / 100;
    pBatInfo->nBatteryFullLifeTime = BATT_FULL_TIME;
#else
    pBatInfo->nBatteryLifeTime     = 0;
    pBatInfo->nBatteryFullLifeTime = 0;
#endif
    {
        UINT8 bcl       = g_pmBatteryStable.percent;
        UINT16 batState = 0;
        pm_BatteryLevel2State(pBatInfo->nBatteryRealTimeVoltage, &bcl, &batState);
        pBatInfo->nBatteryFlag = batState;
    }
    CSW_PROFILE_FUNCTION_EXIT(PM_GetBatteryState);
    return TRUE;
}

/*Description:
*   this function not surpport now
*Parameter:
*  none
*Return:
*  BOOL type.TRUE is success,FALSE if fail.*/
BOOL PM_BatteryChemistry (UINT8 *pChemistryStatus)
{
    CSW_PROFILE_FUNCTION_ENTER(PM_BatteryChemistry);
    if(pChemistryStatus == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(PM_BatteryChemistry);
        return FALSE;
    }
    * pChemistryStatus = PM_BATTERY_CHEMISTRY_UNKNOWN;
    CSW_PROFILE_FUNCTION_EXIT(PM_BatteryChemistry);
    return TRUE;
}

// from charge_mon.c

/*Description:
there are two timer event ,charge event and mmi event ,share one system timer
to monitor the charge and mmi display
*  init PM module parameters
*Parameter:
*  none
*Return:
*  BOOL type.TRUE if successful,FALSE if fail.*/

#ifdef PMD_TWO_BATTARY

//#define POWER_ON_VOL_PER 25
extern VOID pmd_SetPMSub();
extern VOID pmd_SetPMMain();

#else
//#define POWER_ON_VOL_PER 10
#endif

VOID PM_StartEarPieceGpadcMonitorDelay()
{
    CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB, "PM_StartEarPieceGpadcMonitorDelay",  g_pmEarDetect.earon);

    PM_StartEarPieceGpadcMonitor(g_pmEarDetect.earon);
}
#ifdef FM_LININ_AUXMIC
extern BOOL g_halApCommCapModeVersion ;
#endif

VOID pm_ear_detect_inter(PMD_EAR_STATUS_T ear_status)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(1), TSTR("pm: ear int = %d",0x08500012), ear_status);

    if (ear_status == PMD_EAR_STATUS_UNKNOWN)
    {
        g_pmEarDetect.stateStable = FALSE;
        return;
    }
    g_pmEarDetect.stateStable = TRUE;

    g_pmEarDetect.earon = (ear_status == PMD_EAR_STATUS_IN);

#ifdef CHIP_HAS_AP
    if (g_pmEarDetect.mode == PMD_EAR_DETECT_INT)
    {
        syscmds_NotifyEarPieceStatus(g_pmEarDetect.earon);
        // If modem is responsible for earpiece detection, it will not
        // monitor the earpiece key status automatically, unless AP
        // tells modem to start.
    }
    if (g_pmEarDetect.mode == PMD_EAR_DETECT_AP ||
            !g_pmEarDetect.earon)
    {
#ifdef FM_LININ_AUXMIC
        if(g_halApCommCapModeVersion)
        {
            if(g_pmEarDetect.earon)
                pmd_SetLevel(PMD_LEVEL_VIBRATOR, 1);
            else
                pmd_SetLevel(PMD_LEVEL_VIBRATOR, 0);
        }
#endif

        if(g_pmEarDetect.earon)
        {
            sxr_StopFunctionTimer(PM_StartEarPieceGpadcMonitorDelay);
            sxr_StartFunctionTimer(HAL_TICK1S, PM_StartEarPieceGpadcMonitorDelay, NULL, 0);
        }
        else
        {
            PM_StartEarPieceGpadcMonitor(g_pmEarDetect.earon);
        }
    }
#else
    COS_EVENT ev;
    ev.nEventId = EV_DM_GPADC_IND;
    if (g_pmEarDetect.earon)
    {
        ev.nParam1  = 512;
    }
    else
    {
        ev.nParam1  = 2000;
    }
    ev.nParam2  = g_pmEarDetect.earon;
    ev.nParam3  = g_pmEarDetect.mode;
    COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

    if(g_pmEarDetect.earon)
    {
        sxr_StopFunctionTimer(PM_StartEarPieceGpadcMonitorDelay);
        sxr_StartFunctionTimer(HAL_TICK1S, PM_StartEarPieceGpadcMonitorDelay, NULL, 0);
    }
    else
        PM_StartEarPieceGpadcMonitor(g_pmEarDetect.earon);

#endif
}

BOOL pm_BatteryInit(VOID)
{
    CSW_PROFILE_FUNCTION_ENTER(pm_BatteryInit);

#ifdef AT_USB_MODEM_NO_MMMI
    uctls_Open(0xff, 0x0000,0x0000, 0x0000, "USB Controller 1.0");
#endif
    UINT32 now = hal_TimGetUpTime();

    g_pmInitStableCheckTime = now;

    Time_To_MMI_Evt = MIN(g_pmBatMonTime, g_pmEarPieceDetTime);
    // force last date to be in the past so that first reporting will trigger an update
    g_pmBatMonLastDate = now - g_pmBatMonTime;
    g_pmEarPieceDetLastDate = now - g_pmEarPieceDetTime;

#ifdef PMD_TWO_BATTARY
    pmd_SetPMMain();
#endif

    pm_InitStableBatteryLevel();

    g_pmPowerOnVoltPercent = pmd_GetPowerOnVoltPercent();
    g_pmPowerDownVoltPercent = pmd_GetPowerDownVoltPercent();
#ifdef PMD_TWO_BATTARY
    g_pmSubPowerOnVoltPercent = pmd_GetSubPowerOnVoltPercent();
#endif

    PMD_CHARGER_STATUS_T status;
    while ((status = pmd_GetChargerStatus()) == PMD_CHARGER_UNKNOWN);

    UINT8 bcl = g_pmBatteryStable.percent;
#ifdef PMD_TWO_BATTARY
    UINT8 bcl_sub = g_pmBatteryStable_sub.percent;
    if (g_pmBatteryStable.instant == PMD_BAD_VALUE)
    {
        bcl = bcl_sub;
    }
#endif
    if ((status == PMD_CHARGER_UNPLUGGED) && (bcl <= g_pmPowerOnVoltPercent))
    {
#ifdef PMD_TWO_BATTARY
        if (bcl_sub <= g_pmSubPowerOnVoltPercent)
        {
            hal_HstSendEvent(0xdeadba40);
            hal_SysShutdown();
        }
        else
        {
            pmd_SetCurrentBattery(PMD_BATTERY_SUB);
            pmd_SetPMSub();
        }
#else
        hal_HstSendEvent(0xdeadba40);
        hal_SysShutdown();
#endif
    }

    // timer to monitor the bat level for mmi set 100 ms for the first reporting
    COS_SetTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY), PRV_BAL_BATTERY_TIMER2, COS_TIMER_MODE_SINGLE, HAL_TICK1S / 10);

    // Initiate charger status
    pm_ChargerHandler(pmd_GetChargerStatus());
    // register the charger handler to get charger event faster
    pmd_SetChargerStatusHandler(pm_ChargerHandler);

#ifndef MODULE_EAR_DETECT
    if (pmd_GetEarModeStatus() == PMD_EAR_DETECT_POLLING)
    {
        g_pmEarDetect.mode = PMD_EAR_DETECT_POLLING;
        g_pmEarDetect.stateStable = TRUE;
        PM_StartEarPieceGpadcMonitor(TRUE);
        g_pmEarPieceDetTime = PM_EARPIECE_DET_POLLING_IDLE_TIME;
        // Set high Vmic for earpiece detect
        pmd_EnablePower(PMD_POWER_EARPIECE, TRUE);
        g_pmVMICActive = TRUE;
        // Enable earpiece GPADC measurement
        hal_AnaGpadcOpen(pmd_GetEarpieceGpadcChannel(), HAL_ANA_GPADC_ATP_100MS);
    }
#ifdef CHIP_HAS_AP
    else if (pmd_GetEarModeStatus() == PMD_EAR_DETECT_AP)
    {
        g_pmEarDetect.mode = PMD_EAR_DETECT_AP;
        syscmds_SetEarPieceStatusHandler(pm_ear_detect_inter);
        syscmds_SetMonEarPieceKeyHandler(PM_StartEarPieceGpadcMonitor);
    }
#endif
    else
    {
        g_pmEarDetect.mode = PMD_EAR_DETECT_INT;
        pmd_SetEarDetectHandler(pm_ear_detect_inter);
    }
#endif
#if (PM_VMIC_POWER_POLICY != 0)
    hal_LpsSetCallback(pm_lpsCallback);
#endif

    CSW_PROFILE_FUNCTION_EXIT(pm_BatteryInit);
    return TRUE;
}

/*Description:
*  the main monitor process. dispose voltage and charge.
*it is important process.
*Parameter:
*  pev[in]: the event from sysytem.
*Return:
*  BOOL type.TRUE if successful,FALSE if fail.*/
/*first is it a low power event ?
second is it the timer1 event ?
    which elapse 1 ms to verify whether the Adapter is on/off ?
third is it the timer2 event ?
    first it measures all the ADC status, and save the new value of the ADC
    second it deals with  the MMI event
    third it deals with the charge event
    both the MMI event phase and the charge event phase  we do not need to call the functions
    which are related with ADC, we can only get the data from the data structure.*/

BOOL pm_BatteryMonitorProc(COS_EVENT *pEv)
{
    CSW_PROFILE_FUNCTION_ENTER(pm_BatteryMonitorProc);

    HANDLE dmTask = BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY);
    if(EV_CSW_PRV_LPS_WAKEUP == pEv->nEventId)
    {
        //CSW_TRACE(CSW_PM_TRC, 0, "Power UP event");

        // kill the timer, to start a long one (at least 3s) so that
        // paging occur more often than timer
        COS_KillTimerEX(dmTask, PRV_BAL_BATTERY_TIMER2);

        // monitor battery & charger, earpiece detect and tell mmi
        pm_doMonitoring(TRUE);

        COS_SetTimerEX(dmTask, PRV_BAL_BATTERY_TIMER2, COS_TIMER_MODE_SINGLE, MAX(Time_To_MMI_Evt, 3 * HAL_TICK1S));
    }
    else if(EV_TIMER == pEv->nEventId)
    {
        //mmi event timer occured, it makes jade enter low power mode


        if(PRV_BAL_BATTERY_TIMER2 == pEv->nParam1)
        {
            // monitor battery & charger, earpiece detect and tell mmi
            pm_doMonitoring(FALSE);
            COS_SetTimerEX(dmTask, PRV_BAL_BATTERY_TIMER2, COS_TIMER_MODE_SINGLE, Time_To_MMI_Evt);
        }
    }

    CSW_PROFILE_FUNCTION_EXIT(pm_BatteryMonitorProc);
    return ERR_SUCCESS;
}


// functions added to mmi: for fixing battery level accoring to backlight (= mmi activity)
VOID Set_BackLight_Flag_False(VOID)
{
    ;//pmd_SetHighActivity(FALSE);
}

VOID Set_BackLight_Flag_True(VOID)
{
    ;//pmd_SetHighActivity(TRUE);
}

// just for the factory mode now
// note: the battery volt is double of the return value.
//       0xffff mean the bad value

UINT16 PM_GetGpadcValue(PM_GPADC_CHAN_T channel)
{
    if (channel >= HAL_ANA_GPADC_CHAN_QTY)
        return HAL_ANA_GPADC_BAD_VALUE;

    CONST UINT32 vBatChannel = pmd_GetBatteryGpadcChannel();
    CONST UINT32 vMicChannel = pmd_GetEarpieceGpadcChannel();
    CONST UINT32 vChrChannel =  pmd_GetChargerGpadcChannel();
    CONST UINT32 vTpChannel =  pmd_GetTemperatureGpadcChannel();
    UINT32 scStatus;
    // Skip earpiece/battery/charger/temperature measurement channels
    if (channel != vBatChannel && channel != vMicChannel &&
            channel != vChrChannel && channel != vTpChannel)
    {
        hal_AnaGpadcOpen(channel, HAL_ANA_GPADC_ATP_1MS);
    }
    scStatus = hal_SysEnterCriticalSection();
    if (channel == vMicChannel)
    {
        if (g_pmEarDetect.mode != PMD_EAR_DETECT_POLLING && !g_pmEarDetect.earon)
        {
            hal_AnaGpadcOpen(channel, HAL_ANA_GPADC_ATP_1MS);
        }
    }
    else if (channel == vChrChannel)
    {
        if ( !pmd_ChargerChrVoltMonEnabled() ||
                g_pm_oldStatus == PMD_CHARGER_UNKNOWN ||
                g_pm_oldStatus == PMD_CHARGER_UNPLUGGED )
        {
            hal_AnaGpadcOpen(channel, HAL_ANA_GPADC_ATP_1MS);
        }
    }
    else if (channel == vTpChannel)
    {
        if ( !pmd_TemperatureMonEnabled() ||
                g_pm_oldStatus == PMD_CHARGER_UNKNOWN ||
                g_pm_oldStatus == PMD_CHARGER_UNPLUGGED )
        {
            hal_AnaGpadcOpen(channel, HAL_ANA_GPADC_ATP_1MS);
        }
    }
    hal_SysExitCriticalSection(scStatus);
    COS_Sleep(5);

    UINT16 ret;
    for (int i = 0; i < 2; i++) // try once again if failure
    {
        if (channel == vBatChannel)
        {
            ret = pmd_GetGpadcBatteryLevel();
        }
        else if (channel == vChrChannel && pmd_ChargerChrVoltMonEnabled())
        {
            ret = pmd_GetGpadcChargerLevel();
        }
        else if (channel == vTpChannel && pmd_TemperatureMonEnabled())
        {
            ret = pmd_GetGpadcTemperatureLevel();
        }
        else
        {
            ret = hal_AnaGpadcGet(channel);
        }

        if (ret != HAL_ANA_GPADC_BAD_VALUE)
        {
            break;
        }
        COS_Sleep(5);
    }

    // Skip earpiece/battery/charger/temperature measurement channels
    if (channel != vBatChannel && channel != vMicChannel &&
            channel != vChrChannel && channel != vTpChannel)
    {
        hal_AnaGpadcClose(channel);
    }
    scStatus = hal_SysEnterCriticalSection();
    if (channel == vMicChannel)
    {
        if (g_pmEarDetect.mode != PMD_EAR_DETECT_POLLING && !g_pmEarDetect.earon)
        {
            hal_AnaGpadcClose(channel);
        }
    }
    else if (channel == vChrChannel)
    {
        if ( !pmd_ChargerChrVoltMonEnabled() ||
                g_pm_oldStatus == PMD_CHARGER_UNKNOWN ||
                g_pm_oldStatus == PMD_CHARGER_UNPLUGGED )
        {
            hal_AnaGpadcClose(channel);
        }
    }
    else if (channel == vTpChannel)
    {
        if ( !pmd_TemperatureMonEnabled() ||
                g_pm_oldStatus == PMD_CHARGER_UNKNOWN ||
                g_pm_oldStatus == PMD_CHARGER_UNPLUGGED )
        {
            hal_AnaGpadcClose(channel);
        }
    }
    hal_SysExitCriticalSection(scStatus);

    CSW_TRACE(_CSW|TLEVEL(CFW_PM_TS_ID)|TDB|TNB_ARG(2), TSTR("pm: PM_GetGpadcValue = %dmv, channel =%d",0x08500013), ret, channel);
    return ret;
}


VOID pm_ForceUpdateChargerStatus(VOID)
{
    pm_ChargerHandler(pmd_GetChargerStatus());
}

// pm_GetUsbDownLoadStatus() tells whether USB inserted is for USB-Update or not.
// In current design, special USB cables which have short MIC_DET, are made for USB-Update.
// When a USB is plug-in, and this function returns TRUE, the phone goes into USB-Update mode directly.
BOOL pm_GetUsbDownLoadStatus(VOID)
{
    BOOL ret = FALSE;
    if(hal_AnaGpadcGet(pmd_GetEarpieceGpadcChannel()) < 300)
    {
        ret = TRUE;
    }
    return ret;
}

VOID pm_TFlashDetect(BOOL cardPlugged)
{
    COS_EVENT ev;

    ev.nEventId = EV_DM_GPIO_IND;
    ev.nParam1  = cardPlugged;
    ev.nParam2  = 0xff;
    ev.nParam3  = 0;
    COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

