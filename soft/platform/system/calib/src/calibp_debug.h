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


#ifndef  _CALIBP_DEBUG_H_
#define  _CALIBP_DEBUG_H_

#include "hal_debug.h"
#include "calib_profile_codes.h"
#include "sxs_io.h"



// =============================================================================
//  TRACE
// -----------------------------------------------------------------------------
/// Trace macro to use to send a trace. The parameter \c format is a string
/// containing parameters in the "print fashion", but limited for trace to 6
/// parameters. The parameter \c tstmap defines which parameter is a string.
/// (Leave it to 0, if you don't use %s in fmt);
/// @todo implement, replacing level by _CALIB or something
// =============================================================================
#ifndef CALIB_NO_PRINTF
#define CALIB_TRACE(level, tstmap, format, ...)  hal_DbgTrace(level,tstmap,format, ##__VA_ARGS__)
#else
#define CALIB_TRACE(level, tsmap, fmt, ...)
#endif



#ifdef CALIB_NO_ASSERT
#define CALIB_ASSERT(BOOL, format, ...)
#else
// =============================================================================
//  ASSERT
// -----------------------------------------------------------------------------
/// Assert: If the boolean condition (first parameter) is false,
/// raise an assert and print the decorated string passed in the other
/// parameter.
// =============================================================================
#define CALIB_ASSERT(BOOL, format, ...)                   \
    if (!(BOOL)) {            \
        hal_DbgAssert(format, ##__VA_ARGS__);                             \
    }
#endif



// =============================================================================
//  PROFILING
// -----------------------------------------------------------------------------
/// Those are the macros to use for profiling
// =============================================================================
#ifdef CALIB_PROFILING


// =============================================================================
//  CALIB_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define CALIB_PROFILE_PULSE(pulseName)  \
        hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_HAL, (CP_ ## pulseName))


// =============================================================================
//  CALIB_PROFILE_FUNCTION_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define CALIB_PROFILE_FUNCTION_ENTER(eventName) \
        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, (CP_ ## eventName))


// =============================================================================
//  CALIB_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define CALIB_PROFILE_FUNCTION_EXIT(eventName) \
        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, (CP_ ## eventName))


// =============================================================================
//  CALIB_PROFILE_WINDOW_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define CALIB_PROFILE_WINDOW_ENTER(eventName) \
        hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_HAL, (CP_ ## eventName))


// =============================================================================
//  CALIB_PROFILE_WINDOW_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define CALIB_PROFILE_WINDOW_EXIT(eventName) \
        hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_HAL, (CP_ ## eventName))


#else // CALIB_PROFILING

#define CALIB_PROFILE_FUNCTION_ENTER(eventName)
#define CALIB_PROFILE_FUNCTION_EXIT(eventName)
#define CALIB_PROFILE_WINDOW_ENTER(eventName)
#define CALIB_PROFILE_WINDOW_EXIT(eventName)
#define CALIB_PROFILE_PULSE(pulseName)

#endif // CALIB_PROFILING



#endif //_CALIBP_DEBUG_H_


