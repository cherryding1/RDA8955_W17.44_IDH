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


#ifndef  _MEMDP_DEBUG_H_
#define  _MEMDP_DEBUG_H_

#include "hal_debug.h"
#include "memd_profile_codes.h"
#include "sxs_io.h"
#include "edrvp_debug.h"


// =============================================================================
//  TRACE Level mapping
// -----------------------------------------------------------------------------
/// Important trace that can be enabled all the time (low rate)
#define MEMD_WARN_TRC EDRV_WARN_TRC
/// Informational trace interesting for MEMD only
#define MEMD_INFO_TRC EDRV_MEMD_TRC
/// reserved for debug (can be very verbose, but should probably not stay in code)
#define MEMD_DBG_TRC EDRV_DBG_TRC

// =============================================================================
//  TRACE
// -----------------------------------------------------------------------------
/// Trace macro to use to send a trace. The parameter \c format is a string
/// containing parameters in the "print fashion", but limited for trace to 6
/// parameters. The parameter \c tstmap defines which parameter is a string.
/// (Leave it to 0, if you don't use %s in fmt);
// =============================================================================
#if 0 //ndef MEMD_NO_PRINTF
#define MEMD_TRACE(level, tstmap, format, ...)  EDRV_TRACE(level,tstmap,format, ##__VA_ARGS__)
#else
#define MEMD_TRACE(level, tsmap, fmt, ...)
#endif


#ifdef MEMD_NO_ASSERT

#define MEMD_ASSERT(BOOL, format, ...)

#else // !MEMD_NO_ASSERT

// =============================================================================
//  ASSERT
// -----------------------------------------------------------------------------
/// Assert: If the boolean condition (first parameter) is false,
/// raise an assert and print the decorated string passed in the other
/// parameter.
// =============================================================================
#if 0
#ifndef EDRV_NO_TRACE
#define MEMD_ASSERT(BOOL, format, ...)                   \
    if (UNLIKELY(!(BOOL))) {            \
        hal_DbgAssert(format, ##__VA_ARGS__);                             \
    }
#else // EDRV_NO_TRACE
#define MEMD_ASSERT(BOOL, format, ...)                   \
    if (UNLIKELY(!(BOOL))) {            \
        hal_DbgAssert("pls viewGDB");                             \
    }
#endif // EDRV_NO_TRACE
#else
#define MEMD_ASSERT(condition, format, ...) \
    if (UNLIKELY(!(condition))) { asm volatile("break 2"); }
#endif

#endif // !MEMD_NO_ASSERT



// =============================================================================
//  PROFILING
// -----------------------------------------------------------------------------
/// Those are the macros to use for profiling
// =============================================================================
#ifdef MEMD_PROFILING


// =============================================================================
//  MEMD_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define MEMD_PROFILE_PULSE(pulseName)  \
        hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_EDRV, (CP_ ## pulseName))


// =============================================================================
//  MEMD_PROFILE_FUNCTION_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define MEMD_PROFILE_FUNCTION_ENTER(eventName) \
        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_EDRV, (CP_ ## eventName))


// =============================================================================
//  MEMD_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define MEMD_PROFILE_FUNCTION_EXIT(eventName) \
        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_EDRV, (CP_ ## eventName))


// =============================================================================
//  MEMD_PROFILE_WINDOW_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define MEMD_PROFILE_WINDOW_ENTER(eventName) \
        hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_EDRV, (CP_ ## eventName))


// =============================================================================
//  MEMD_PROFILE_WINDOW_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define MEMD_PROFILE_WINDOW_EXIT(eventName) \
        hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_EDRV, (CP_ ## eventName))


#else // MEMD_PROFILING

#define MEMD_PROFILE_FUNCTION_ENTER(eventName)
#define MEMD_PROFILE_FUNCTION_EXIT(eventName)
#define MEMD_PROFILE_WINDOW_ENTER(eventName)
#define MEMD_PROFILE_WINDOW_EXIT(eventName)
#define MEMD_PROFILE_PULSE(pulseName)

#endif // MEMD_PROFILING



#endif //_MEMDP_DEBUG_H_


