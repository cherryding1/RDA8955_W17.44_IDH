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


#include "spal_debug.h"
/// @todo include the profile codes here
#include "spp_profile_codes.h"

#ifndef _SPPP_DEBUG_H_
#define _SPPP_DEBUG_H_



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
#ifndef CS_PLATFORM_SIMU
#ifndef SPP_NO_TRACE
#define SPP_TRACE(level, tstmap, format, ...)  spp_DbgTrace(level,tstmap,format, ##__VA_ARGS__)
#else
#define SPP_TRACE(level, tstmap, format, ...)
#endif //SPP_NO_PRINTF
#endif                                                  // #ifndef CS_PLATFORM_SIMU



#ifndef CS_PLATFORM_SIMU
#ifdef SPP_NO_ASSERT
#define SPP_ASSERT(boolCondition, format, ...)
#else
// =============================================================================
//  ASSERT
// -----------------------------------------------------------------------------
/// Assert: If the boolean condition (first parameter) is false,
/// raise an assert and print the decorated string passed in the other
/// parameter.
// =============================================================================
#define SPP_ASSERT(boolCondition, format, ...)                   \
    if (!(boolCondition)) {            \
        spp_DbgAssert(format, ##__VA_ARGS__);                             \
    }
#endif
#endif                                                  // #ifndef CS_PLATFORM_SIMU



// =============================================================================
//  PROFILING
// -----------------------------------------------------------------------------
/// Those are the macros to use for profiling
/// @todo Implement them with spp_DbgProfilingEnter etc instead of this :
// =============================================================================
#ifdef SPP_PROFILING


// =============================================================================
//  SPP_PROFILE_FUNCTION_ENTRY
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define SPP_PROFILE_FUNCTION_ENTRY(eventName) \
        spal_DbgPxtsProfileFunctionEnter(SPAL_DBG_PXTS_SPP, (CP_ ## eventName))


// =============================================================================
//  SPP_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define SPP_PROFILE_FUNCTION_EXIT(eventName) \
        spal_DbgPxtsProfileFunctionExit(SPAL_DBG_PXTS_SPP, (CP_ ## eventName))


// =============================================================================
//  SPP_PROFILE_WINDOW_ENTRY
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define SPP_PROFILE_WINDOW_ENTRY(eventName) \
        spal_DbgPxtsProfileWindowEnter(SPAL_DBG_PXTS_SPP, (CP_ ## eventName))


// =============================================================================
//  SPP_PROFILE_WINDOW_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define SPP_PROFILE_WINDOW_EXIT(eventName) \
        spal_DbgPxtsProfileWindowExit(SPAL_DBG_PXTS_SPP, (CP_ ## eventName))


// =============================================================================
//  SPP_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define SPP_PROFILE_PULSE(pulseName)  \
                spal_DbgPxtsProfilePulse(SPAL_DBG_PXTS_SPP, (CP_ ## pulseName))


#else // SPP_PROFILING

#define SPP_PROFILE_FUNCTION_ENTRY(eventName)
#define SPP_PROFILE_FUNCTION_EXIT(eventName)
#define SPP_PROFILE_WINDOW_ENTRY(eventName)
#define SPP_PROFILE_WINDOW_EXIT(eventName)
#define SPP_PROFILE_PULSE(pulseName)

#endif // SPP_PROFILING


// PORFILE
#define CPSPPIRCOMBSTART 0x4120
#define CPSPPIRCOMBMAX      0x3F



#define SPP_PROFILE_IRCOMBCNT_ENTER(num, mode)  \
    spal_DbgPxtsProfileFunctionEnter(SPAL_DBG_PXTS_SPP, \
            CPSPPIRCOMBSTART+(((num-1)&0x7)|(((mode-1)&0x7)<<3)))
#define SPP_PROFILE_IRCOMBCNT_EXIT(num, mode)  \
    spal_DbgPxtsProfileFunctionExit(SPAL_DBG_PXTS_SPP, \
            CPSPPIRCOMBSTART+(((num-1)&0x7)|(((mode-1)&0x7)<<3)))

#endif // _SPPP_DEBUG_H_







