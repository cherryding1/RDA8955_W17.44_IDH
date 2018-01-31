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



#include "hal_debug.h"
/// @todo include the profile codes here
//#include "reftest_profile_codes.h"
#include "sxs_io.h"

#ifndef _REFTESTP_CFG_H_
#define _REFTESTP_CFG_H_



// =============================================================================
//  TRACE
// -----------------------------------------------------------------------------
/// Trace macro to use to send a trace. The parameter \c format is a string
/// containing parameters in the "print fashion", but limited for trace to 6
/// parameters. The parameter \c tstmap defines which parameter is a string.
/// (Leave it to 0, if you don't use %s in fmt);
/// @todo implement, replacing level by _HAL or something
// =============================================================================
#ifdef REFTEST_NO_PRINTF
#define REFTEST_TRACE(level, tsmap, fmt, ...)
#else
#define REFTEST_TRACE(level, tsmap, format, ...) \
            hal_DbgTrace(_MMI | level, tsmap, format, ##__VA_ARGS__)
#endif



#ifdef REFTEST_NO_ASSERT
#define REFTEST_ASSERT(BOOL, format, ...)
#else
// =============================================================================
//  ASSERT
// -----------------------------------------------------------------------------
/// Assert: If the boolean condition (first parameter) is false,
/// raise an assert and print the decorated string passed in the other
/// parameter.
// =============================================================================
#define REFTEST_ASSERT(BOOL, format, ...) \
    if (!(BOOL)) { \
        hal_DbgAssert(format, ##__VA_ARGS__); \
    }
#endif



// =============================================================================
//  PROFILING
// -----------------------------------------------------------------------------
/// Those are the macros to use for profiling
/// @todo Implement them with hal_DbgProfilingEnter etc instead of this :
// =============================================================================
#ifdef REFTEST_PROFILING


// =============================================================================
//  REFTEST_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define REFTEST_PROFILE_PULSE(pulseName) \
        hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_SVC, (CP_ ## pulseName))


// =============================================================================
//  REFTEST_PROFILE_FUNCTION_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the beginning of a profiled function or window.
// =============================================================================
#define REFTEST_PROFILE_FUNCTION_ENTER(eventName) \
        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_SVC, (CP_ ## eventName))


// =============================================================================
//  REFTEST_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define REFTEST_PROFILE_FUNCTION_EXIT(eventName) \
        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_SVC, (CP_ ## eventName))


// =============================================================================
//  REFTEST_PROFILE_WINDOW_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the beginning of a profiled function or window.
// =============================================================================
#define REFTEST_PROFILE_WINDOW_ENTER(eventName) \
        hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_SVC, (CP_ ## eventName))


// =============================================================================
//  REFTEST_PROFILE_WINDOW_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define REFTEST_PROFILE_WINDOW_EXIT(eventName) \
        hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_SVC, (CP_ ## eventName))


#else // REFTEST_PROFILING

#define REFTEST_PROFILE_FUNCTION_ENTER(eventName)
#define REFTEST_PROFILE_FUNCTION_EXIT(eventName)
#define REFTEST_PROFILE_WINDOW_ENTER(eventName)
#define REFTEST_PROFILE_WINDOW_EXIT(eventName)
#define REFTEST_PROFILE_PULSE(pulseName)

#endif // REFTEST_PROFILING



#endif // _REFTESTP_CFG_H_

