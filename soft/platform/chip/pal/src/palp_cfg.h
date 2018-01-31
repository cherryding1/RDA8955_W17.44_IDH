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
#include "pal_profile_codes.h"

#ifndef PALP_CFG_H
#define PALP_CFG_H

// =============================================================================
//  TRACE
// -----------------------------------------------------------------------------
/// Trace macro to use to send a trace. The parameter \c format is a string
/// containing parameters in the "print fashion", but limited for trace to 6
/// parameters. The parameter \c tstmap defines which parameter is a string.
/// (Leave it to 0, if you don't use %s in fmt);
/// @todo implement
// =============================================================================
// Conditional Printf Usage
#ifndef PAL_NO_TRACE
#define PAL_TRACE(level, tstmap, format, ...)  hal_DbgTrace(_PAL|(level),tstmap,format, ##__VA_ARGS__)
#else
#define PAL_TRACE(level, tstmap, format, ...)
#endif // PAL_NO_TRACE



#ifdef PAL_NO_ASSERT
#define PAL_ASSERT(boolCondition, format, ...)
#else
// =============================================================================
//  ASSERT
// -----------------------------------------------------------------------------
/// Assert: If the boolean condition (first parameter) is false,
/// raise an assert and print the decorated string passed in the other
/// parameter.
// =============================================================================
#define PAL_ASSERT(boolCondition, format, ...)                   \
    if (!(boolCondition)) {            \
        hal_DbgAssert(format, ##__VA_ARGS__);                             \
    }
#endif



// =============================================================================
//  PROFILING
// -----------------------------------------------------------------------------
/// Those are the macros to use for profiling
/// @todo Implement them with hal_DbgProfilingEnter etc instead of this :
// =============================================================================
#ifdef PAL_PROFILING


// =============================================================================
//  PAL_PROFILE_FUNCTION_ENTRY
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define PAL_PROFILE_FUNCTION_ENTRY(eventName) \
        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_PAL, (CP_ ## eventName))


// =============================================================================
//  PAL_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define PAL_PROFILE_FUNCTION_EXIT(eventName) \
        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_PAL, (CP_ ## eventName))


// =============================================================================
//  PAL_PROFILE_WINDOW_ENTRY
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define PAL_PROFILE_WINDOW_ENTRY(eventName) \
        hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_PAL, (CP_ ## eventName))


// =============================================================================
//  PAL_PROFILE_WINDOW_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define PAL_PROFILE_WINDOW_EXIT(eventName) \
        hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_PAL, (CP_ ## eventName))


// =============================================================================
//  PAL_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define PAL_PROFILE_PULSE(pulseName)  \
                hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_PAL, (CP_ ## pulseName))


#else // PAL_PROFILING

#define PAL_PROFILE_FUNCTION_ENTRY(eventName)
#define PAL_PROFILE_FUNCTION_EXIT(eventName)
#define PAL_PROFILE_WINDOW_ENTRY(eventName)
#define PAL_PROFILE_WINDOW_EXIT(eventName)
#define PAL_PROFILE_PULSE(pulseName)

#endif // PAL_PROFILING



#endif // PALP_CFG_H







