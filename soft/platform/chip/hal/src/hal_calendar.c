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

#include "global_macros.h"
#include "calendar.h"
#include "sys_ctrl.h"
#include "halp_debug.h"

#include "hal_timers.h"
#include "hal_error.h"
#include "hal_host.h"
#include "sxr_tls.h"
#include "reg_alias.h"


// Whether to return a fake time when the real RTC time is not updated
// to the registers yet
// 1) 8805/8806/8808 need to get a correct time to compare with the time
//    saved in flash (workaround for CALENDAR_CALENDAR_NOT_PROG bit).
// 2) All chips need to get a correct time before handling alarms.
#define RTC_TIME_RETURN_FAKE_IF_NOT_UPDATED 0

// The time needed since boot that the real RTC time is updated to
// the registers.
// RTC current time will be updated every 1 second. But the first
// interval to update RTC current value registers after h/w boots
// is random within 1.2 second (or 1 second after setting wakeup line).
// To ensure that we can get a valid value, we have to wait until
// h/w is up for 1.2 second before reading the registers.
#define RTC_TIME_VALID_SINCE_BOOT (1200 MS_WAITING)



// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

// =============================================================================
// g_halRtcRegistry
// -----------------------------------------------------------------------------
/// Variable to store the user irq handler for the calendar interval
/// interruption.
// =============================================================================
PRIVATE HAL_TIM_RTC_IRQ_HANDLER_T g_halRtcRegistry
    = NULL ;


/// =============================================================================
// g_halRtcAlarmRegistry
// -----------------------------------------------------------------------------
/// Variable to store the user handler called when an alarm is reached.
// =============================================================================
PRIVATE HAL_TIM_RTC_ALARM_IRQ_HANDLER_T g_halRtcAlarmRegistry
    = NULL ;


#ifndef FPGA
// =============================================================================
// g_halRtcTimeValid
// -----------------------------------------------------------------------------
/// Variable to tell whether RTC time can be read.
// =============================================================================
PRIVATE BOOL g_halRtcTimeValid
    = FALSE ;
#endif


// ============================================================================
//  FUNCTIONS
// ============================================================================

// =============================================================================
// hal_TimRtcGetTime
// -----------------------------------------------------------------------------
/// This function returns the current RTC time in the
/// HAL_TIM_RTC_TIME_T structure whose pointer is passed as a parameter.
/// The validity of the clock has to be checked beforehand.
///
/// @param rtcTime Pointer to the structure which will store after the call
/// of the function the current RTC time.
/// @return #HAL_ERR_NO or #HAL_ERR_TIM_RTC_NOT_VALID
// =============================================================================
PUBLIC HAL_ERR_T hal_TimRtcGetTime(HAL_TIM_RTC_TIME_T* rtcTime)
{
#ifdef FPGA

    // Return the fake time (2013-01-01 00:00:00, Tue)
    rtcTime->sec   = 0;
    rtcTime->min   = 0;
    rtcTime->hour  = 0;
    rtcTime->day   = 1;
    rtcTime->month = 1;
    rtcTime->year  = 13;
    rtcTime->wDay  = 2;

    return HAL_ERR_NO;

#else // !FPGA

    /////////////////////////////////////////////////////////////////
    //    NOTE:
    //    This function is called in two cases:
    //    1. receive RTC IRQ.
    //    2. at beginning of power on.
    //    In both cases, the RTC time will be read just after all regs
    //    have been updated, so no issue of incoherence.
    /////////////////////////////////////////////////////////////////

    // To store the registers and read only once.
    UINT32 curValLow = 0;
    UINT32 curValHigh = 0;

    // Is the RTC valid
    if (hwp_calendar->Status & CALENDAR_CALENDAR_NOT_PROG)
    {
        return HAL_ERR_TIM_RTC_NOT_VALID;
    }

#if (RTC_TIME_RETURN_FAKE_IF_NOT_UPDATED)

    if (!g_halRtcTimeValid)
    {
        if (hal_TimGetUpTime() < RTC_TIME_VALID_SINCE_BOOT)
        {
            hal_HstSendEvent(0x713eeeee);

            // Return the fake time (2013-01-01 00:00:00, Tue)
            rtcTime->sec   = 0;
            rtcTime->min   = 0;
            rtcTime->hour  = 0;
            rtcTime->day   = 1;
            rtcTime->month = 1;
            rtcTime->year  = 13;
            rtcTime->wDay  = 2;

            return HAL_ERR_NO;
        }
        else
        {
            g_halRtcTimeValid = TRUE;
        }
    }

    curValHigh = hwp_calendar->Calendar_CurVal_H;
    curValLow  = hwp_calendar->Calendar_CurVal_L;

#else // !RTC_TIME_RETURN_FAKE_IF_NOT_UPDATED

    curValHigh = hwp_calendar->Calendar_CurVal_H;
    curValLow  = hwp_calendar->Calendar_CurVal_L;

    if (!g_halRtcTimeValid)
    {
        g_halRtcTimeValid = TRUE;

        UINT32 upTime = hal_TimGetUpTime();

        while (hal_TimGetUpTime() < RTC_TIME_VALID_SINCE_BOOT)
        {
            if (curValHigh == hwp_calendar->Calendar_CurVal_H &&
                    curValLow == hwp_calendar->Calendar_CurVal_L)
            {
                sxr_Sleep(20 MS_WAITING);
            }
            else
            {
                break; // RTC time has been updated
            }
        }

        // hal_HstSendEvent(0x713effff);
        // hal_HstSendEvent(hal_TimGetUpTime() - upTime);

        curValHigh = hwp_calendar->Calendar_CurVal_H;
        curValLow  = hwp_calendar->Calendar_CurVal_L;
    }

#endif // !RTC_TIME_RETURN_FAKE_IF_NOT_UPDATED

    // Did the time wrap to the next time
    if (curValHigh != hwp_calendar->Calendar_CurVal_H)
    {
        // Midnight wrap, reload them all once
        curValLow  = hwp_calendar->Calendar_CurVal_L;
        curValHigh = hwp_calendar->Calendar_CurVal_H;
    }

    // Extract all the fields
    rtcTime->sec   = GET_BITFIELD(curValLow,  CALENDAR_SEC);
    rtcTime->min   = GET_BITFIELD(curValLow,  CALENDAR_MIN);
    rtcTime->hour  = GET_BITFIELD(curValLow,  CALENDAR_HOUR);
    rtcTime->day   = GET_BITFIELD(curValHigh, CALENDAR_DAY);
    rtcTime->month = GET_BITFIELD(curValHigh, CALENDAR_MON);
    rtcTime->year  = GET_BITFIELD(curValHigh, CALENDAR_YEAR);
    rtcTime->wDay  = GET_BITFIELD(curValHigh, CALENDAR_WEEKDAY);

    return HAL_ERR_NO;

#endif // !FPGA
}



// =============================================================================
// hal_TimRtcSetTime
// -----------------------------------------------------------------------------
/// This function loads the RTC timer with the \c rtcTime value and enable the
/// \c intMode interrupt mode. This
/// function should only be used at the initialization of the device. This
/// timer will continue to increment even in deep power down mode.
///
/// @param rtcTime Pointer to the date and time to load in the RTC timer
/// @return #HAL_ERR_NO or #HAL_ERR_RESOURCE_BUSY.
// =============================================================================
PUBLIC HAL_ERR_T hal_TimRtcSetTime(CONST HAL_TIM_RTC_TIME_T * rtcTime)
{
#if defined(FPGA) || defined(CHIP_HAS_AP)

    return HAL_ERR_NO;

#else // !FPGA

    UINT32 loadValLow;
    UINT32 loadValHigh;

    // Convert time to the reg format
    loadValLow =  CALENDAR_SEC(rtcTime->sec)
                  | CALENDAR_MIN(rtcTime->min)
                  | CALENDAR_HOUR(rtcTime->hour);

    loadValHigh = CALENDAR_DAY(rtcTime->day)
                  | CALENDAR_MON(rtcTime->month)
                  | CALENDAR_YEAR(rtcTime->year)
                  | CALENDAR_WEEKDAY(rtcTime->wDay);

    // wait until previous load finishes to aVOID dead loop repetition
    if (hwp_calendar->Cmd & CALENDAR_CALENDAR_LOAD)
    {
        // previous load yet in progress
        return HAL_ERR_RESOURCE_BUSY;
    }
    else
    {
        // Configuration
        hwp_calendar->Calendar_LoadVal_L = loadValLow;
        hwp_calendar->Calendar_LoadVal_H = loadValHigh;

        // Apply command
        hwp_calendar->Cmd = CALENDAR_CALENDAR_LOAD;

        // Make sure the calendar is programmed before exiting this function
        // for coherency.
        while (hwp_calendar->Status & CALENDAR_CALENDAR_NOT_PROG);

        // Mark the time as valid
        g_halRtcTimeValid = TRUE;

        return HAL_ERR_NO;
    }

#endif // !FPGA
}


// =============================================================================
// hal_TimRtcNotProgrammed
// -----------------------------------------------------------------------------
/// This function checks if the RTC has been programmed or not. If it
/// has not been programmed, this function will automatically disable the RTC
/// alarm if it is set.
///
/// @return \c TRUE if the RTC has not been programmed, and clear the RTC alarm
/// in that case if it had been programmed. \c FALSE if the RTC has been
/// programmed.
// =============================================================================
PUBLIC BOOL hal_TimRtcNotProgrammed(VOID)
{
#ifdef FPGA

    return FALSE;

#else // !FPGA

    if (hwp_calendar->Status & CALENDAR_CALENDAR_NOT_PROG)
    {
        // Bit = 1 means calendar not programmed
        return TRUE;
    }
    else
    {
        return FALSE;
    }

#endif // !FPGA
}




// =============================================================================
// hal_TimRtcIrqSetHandler
// -----------------------------------------------------------------------------
/// This function is used to set the handler called when a
/// RTC interrupt occurs
/// @param handler User function called when a RTC interruption occurs
// =============================================================================
PUBLIC VOID hal_TimRtcIrqSetHandler(HAL_TIM_RTC_IRQ_HANDLER_T handler)
{
#if !(defined(FPGA) || defined(CHIP_HAS_AP))
    g_halRtcRegistry = handler;
#endif
}



// =============================================================================
// hal_TimRtcIrqSetMask
// -----------------------------------------------------------------------------
/// This function is used to set or clear the mask of the RTC IRQ
/// @param mask If \c TRUE, RTC IRQ can occur and trig the user handler.
///             IF \c FALSE, they can't.
// =============================================================================
PUBLIC VOID hal_TimRtcIrqSetMask(BOOL mask)
{
#if !(defined(FPGA) || defined(CHIP_HAS_AP))
    if (mask)
    {
        hwp_calendar->Cmd = CALENDAR_ITV_IRQ_MASK_SET;
    }
    else
    {
        hwp_calendar->Cmd = CALENDAR_ITV_IRQ_MASK_CLR;
    }
#endif
}



// =============================================================================
// hal_TimRtcIrqIntervalMode
// -----------------------------------------------------------------------------
/// This function is used to configure the interval of the RTC interval IRQ:
/// second, minute or hour.
/// @param mode Interval of the RTC interrupt.
// =============================================================================
PUBLIC VOID hal_TimRtcSetIrqIntervalMode(HAL_TIM_RTC_INT_MODE_T mode)
{
#if !(defined(FPGA) || defined(CHIP_HAS_AP))
    hwp_calendar->Ctrl = CALENDAR_INTERVAL(mode);
#endif
}



// =============================================================================
// hal_TimRtcIrqGetMask
// -----------------------------------------------------------------------------
/// This function is use to retrieve the RTC IRQ mask.
/// @return \c TRUE is the mask is set or \c FALSE if the RTC IRQ can't occur.
// =============================================================================
PUBLIC BOOL hal_TimRtcIrqGetMask(VOID)
{
    if (hwp_calendar->Cmd & CALENDAR_ITV_IRQ_MASK_SET)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}





// =============================================================================
// hal_TimRtcSetAlarm
// -----------------------------------------------------------------------------
/// This function sets up an alarm which will be triggered when the RTC timer
/// reaches the time in \c alarmTime, and enable it.
///
/// An interrupt will be generated when this alarm is reached. \n
/// <b> The interrupt handler will automatically mask this interrupt, and
/// clear it.
///
/// @param alarmTime Time of alarm
/// @return #HAL_ERR_NO or #HAL_ERR_RESOURCE_BUSY, if a previous load
/// is still in progress.
// =============================================================================
PUBLIC HAL_ERR_T hal_TimRtcSetAlarm(CONST HAL_TIM_RTC_TIME_T* alarmTime)
{
#if defined(FPGA) || defined(CHIP_HAS_AP)

    return HAL_ERR_NO;

#else // !FPGA

    UINT32 loadValLow;
    UINT32 loadValHigh;

    // Convert time to the reg format
    loadValLow =  CALENDAR_SEC(alarmTime->sec)
                  | CALENDAR_MIN(alarmTime->min)
                  | CALENDAR_HOUR(alarmTime->hour);

    loadValHigh = CALENDAR_DAY(alarmTime->day)
                  | CALENDAR_MON(alarmTime->month)
                  | CALENDAR_YEAR(alarmTime->year)
                  | CALENDAR_WEEKDAY(alarmTime->wDay);

    // wait until previous load finishes to avoid dead loop repetition
    if (hwp_calendar->Cmd & CALENDAR_ALARM_LOAD)
    {
        // previous load still in progress
        return HAL_ERR_RESOURCE_BUSY;
    }
    else
    {
        // Set alarm time
        hwp_calendar-> AlarmVal_L = loadValLow;
        hwp_calendar-> AlarmVal_H = loadValHigh;

        // Load alarm
        hwp_calendar->Cmd = CALENDAR_ALARM_LOAD;

        // The alarm needs to be enabled

        return HAL_ERR_NO;
    }

#endif // !FPGA
}



// =============================================================================
// hal_TimRtcAlarmEnable
// -----------------------------------------------------------------------------
/// Enable the RTC alarm.
// =============================================================================
PUBLIC VOID hal_TimRtcAlarmEnable(VOID)
{
#if !(defined(FPGA) || defined(CHIP_HAS_AP))
    // check load - We must wait load is finished before
    // enabling, and ensure that no disabling or clearing
    // is in progress.
    while (hwp_calendar->Cmd &
            (CALENDAR_ALARM_LOAD
             | CALENDAR_ALARM_ENABLE_SET
             | CALENDAR_ALARM_ENABLE_CLR));

    // Alarm loaded, required enabling
    hwp_calendar->Cmd |=  CALENDAR_ALARM_ENABLE_SET;
#endif
}



// =============================================================================
// hal_TimRtcAlarmDisable
// -----------------------------------------------------------------------------
/// Disables the alarm on the RTC
// =============================================================================
PUBLIC VOID hal_TimRtcAlarmDisable(VOID)
{
#if !(defined(FPGA) || defined(CHIP_HAS_AP))
    // wait until previous enable operation finishes as disable has low priority
    while (hwp_calendar->Cmd & (CALENDAR_ALARM_ENABLE_SET|CALENDAR_ALARM_ENABLE_CLR));

    // command to disable alarm
    hwp_calendar->Cmd |= CALENDAR_ALARM_ENABLE_CLR;
#endif
}







// =============================================================================
// hal_TimRtcAlarmIrqSetHandler
// -----------------------------------------------------------------------------
/// This function is used to set the handler called when a
/// RTC alarm occurs
/// @param handler User function called when a RTC alarm interrupt occurs.
// =============================================================================
PUBLIC VOID hal_TimRtcAlarmIrqSetHandler(HAL_TIM_RTC_ALARM_IRQ_HANDLER_T handler)
{
#if !(defined(FPGA) || defined(CHIP_HAS_AP))
    g_halRtcAlarmRegistry = handler;
#endif
}


// =============================================================================
// hal_TimRtcIrqHandler
// -----------------------------------------------------------------------------
/// This function is the IRQ handler for the IRQ module called when a RTC/calendar
/// IRQ occurs. It clears the IRQ and calls the user handler if available.
// =============================================================================
PROTECTED VOID hal_TimRtcIrqHandler(UINT8 interruptId)
{
    // get status
    UINT32 status = hwp_calendar->Status;
    UINT32 clearMask = 0;

    if (status & CALENDAR_ALARM_IRQ_CAUSE)
    {
        // Ensure the wake-up line is high (not automatic, depending
        // on the boot mode). (It is protected since Greenstone)
        hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
#ifndef CHIP_DIE_8909
        hwp_sysCtrl->WakeUp = SYS_CTRL_FORCE_WAKEUP;
#endif
        clearMask = CALENDAR_ALARM_CLR;

#ifndef FPGA
        // ouch !
        // WAIT (:() for the wake up line to be raised.
        while (!(hwp_calendar->Status & CALENDAR_FORCE_WAKEUP));
#endif
    }

    if (status & CALENDAR_ITV_IRQ_CAUSE)
    {
        clearMask |= CALENDAR_ITV_IRQ_CLR;
    }

    // Clear interrupts
    hwp_calendar->Cmd = clearMask;

    if (status & CALENDAR_ITV_IRQ_CAUSE)
    {
        if (g_halRtcRegistry)
        {
            g_halRtcRegistry();
        }
    }

    if (status & CALENDAR_ALARM_IRQ_CAUSE)
    {
        if (g_halRtcAlarmRegistry)
        {
            g_halRtcAlarmRegistry();
        }
    }
}


#ifdef CHIP_HAS_AP
// =============================================================================
// hal_TimRtcAlarmIrqSetModemHandler
// -----------------------------------------------------------------------------
/// This function is used to set the handler called when a
/// RTC alarm occurs for modem
/// @param handler User function called when a RTC alarm interrupt occurs.
// =============================================================================
PUBLIC VOID hal_TimRtcAlarmIrqSetModemHandler(HAL_TIM_RTC_ALARM_IRQ_HANDLER_T handler)
{
    g_halRtcAlarmRegistry = handler;
}
#endif

