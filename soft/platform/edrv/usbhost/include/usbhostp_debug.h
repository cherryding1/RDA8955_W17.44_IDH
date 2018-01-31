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


#ifndef _USBHOSTP_DEBUG_H_
#define _USBHOSTP_DEBUG_H_

#include "hal_debug.h"
#include "usbhost_profile_codes.h"

#define _USE_SEND_EVENT
#ifdef _USE_SEND_EVENT
#define usb_HstSendEvent hal_HstSendEvent
#else
#define usb_HstSendEvent
#endif
// =============================================================================
//  TRACE Level mapping
// -----------------------------------------------------------------------------
/// Important trace that can be enabled all the time (low rate)
#define USBHOST_WARN_TRC EDRV_WARN_TRC
/// Informational trace interesting for USBHOST only
#define USBHOST_INFO_TRC EDRV_MCD_TRC
/// reserved for debug (can be very verbose, but should probably not stay in code)
#define USBHOST_DBG_TRC EDRV_DBG_TRC

// =============================================================================
//  TRACE
// -----------------------------------------------------------------------------
/// Trace macro to use to send a trace. The parameter \c format is a string
/// containing parameters in the "print fashion", but limited for trace to 6
/// parameters. The parameter \c tstmap defines which parameter is a string.
/// (Leave it to 0, if you don't use %s in fmt);
// =============================================================================
// Conditional Printf Usage
#ifndef USBHOST_NO_PRINTF
#include "cs_types.h"
#include "sxs_io.h"
#include "edrvp_debug.h"

/// #warning TRACE DEFINED !!!!
#define USBHOST_TRACE(level, tstmap, format, ...)  EDRV_TRACE(level,tstmap,format, ##__VA_ARGS__)
#else
#define USBHOST_TRACE(level, tstmap, format, ...)
#endif //USBHOST_PRINTF



// =============================================================================
//  ASSERT
// -----------------------------------------------------------------------------
/// Assert: If the boolean condition (first parameter) is false,
/// raise an assert and print the decorated string passed in the other
/// parameter.
// =============================================================================
#ifdef USBHOST_NO_ASSERT
#define USBHOST_ASSERT(boolCondition, format, ...)
#else
#ifndef EDRV_NO_TRACE
#define USBHOST_ASSERT(boolCondition, format, ...)                   \
    if (!(boolCondition)) {            \
        hal_DbgAssert(format, ##__VA_ARGS__);                             \
    }
#else
#define USBHOST_ASSERT(boolCondition, format, ...)                   \
    if (!(boolCondition)) {            \
        hal_DbgAssert("view GDB");                             \
    }

#endif

#endif



// =============================================================================
//  PROFILING
// -----------------------------------------------------------------------------
/// Those are the macros to use for profiling
/// @todo Implement them with hal_DbgProfilingEnter etc instead of this :
// =============================================================================
#ifdef USBHOST_PROFILING


// =============================================================================
//  USBHOST_PROFILE_FUNCTION_ENTRY
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define USBHOST_PROFILE_FUNCTION_ENTER(eventName) \
        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_EDRV, (CP_ ## eventName))


// =============================================================================
//  USBHOST_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define USBHOST_PROFILE_FUNCTION_EXIT(eventName) \
        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_EDRV, (CP_ ## eventName))


// =============================================================================
//  USBHOST_PROFILE_WINDOW_ENTRY
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define USBHOST_PROFILE_WINDOW_ENTRY(eventName) \
        hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_EDRV, (CP_ ## eventName))


// =============================================================================
//  USBHOST_PROFILE_WINDOW_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define USBHOST_PROFILE_WINDOW_EXIT(eventName) \
        hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_EDRV, (CP_ ## eventName))


// =============================================================================
//  USBHOST_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define USBHOST_PROFILE_PULSE(pulseName)  \
                hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_EDRV, (CP_ ## pulseName))


#else // USBHOST_PROFILING

#define USBHOST_PROFILE_FUNCTION_ENTER(eventName)
#define USBHOST_PROFILE_FUNCTION_EXIT(eventName)
#define USBHOST_PROFILE_WINDOW_ENTRY(eventName)
#define USBHOST_PROFILE_WINDOW_EXIT(eventName)
#define USBHOST_PROFILE_PULSE(pulseName)

#endif // USBHOST_PROFILING



#endif // _USBHOSTP_DEBUG_H_







