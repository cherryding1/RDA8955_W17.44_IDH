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
#include "sxs_io.h"


#ifndef _DLL_CFG_H_
#define _DLL_CFG_H_

#define DLL_TRACE_MAX_LEVEL 16
// =============================================================================
// DLL_ERR_T
// -----------------------------------------------------------------------------
/// This types is used to describes all possible error returned by DLL's
/// API functions.
// =============================================================================
typedef enum
{
    /// No error occured
    DLL_ERR_NO,

    /// Unsupported operation or parameter
    DLL_ERR_UNSUPPORTED,

    DLL_ERR_QTY
} DLL_ERR_T;


// =============================================================================
//  TRACE
// -----------------------------------------------------------------------------
/// Trace macro to use to send a trace. The parameter \c format is a string
/// containing parameters in the "print fashion", but limited for trace to 6
/// parameters. The parameter \c tstmap defines which parameter is a string.
/// (Leave it to 0, if you don't use %s in fmt);
/// @todo implement, replacing level by _HAL or something
// =============================================================================
#ifndef DLL_NO_PRINTF
#define _DL_TRACEOUT(nModuleIdx, pFormat, ...)  TS_OutputText(nModuleIdx, pFormat, ##__VA_ARGS__)
#define DLL_TRACE(level, tsmap, format, ...)  hal_DbgTrace(level,tsmap,format, ##__VA_ARGS__)
#else
#define _DL_TRACEOUT(nModuleIdx, pFormat, ...)
#define DLL_TRACE(level, tsmap, fmt, ...)
#endif



#ifdef DLL_NO_ASSERT
#define DLL_ASSERT(BOOL, format, ...)
#else
// =============================================================================
//  ASSERT
// -----------------------------------------------------------------------------
/// Assert: If the boolean condition (first parameter) is false,
/// raise an assert and print the decorated string passed in the other
/// parameter.
// =============================================================================
#define DLL_ASSERT(BOOL, format, ...)                   \
    if (!(BOOL)) {            \
        hal_DbgAssert(format, ##__VA_ARGS__);                             \
    }
#endif



// =============================================================================
//  PROFILING
// -----------------------------------------------------------------------------
/// Those are the macros to use for profiling
/// @todo Implement them with hal_DbgProfilingEnter etc instead of this :
// =============================================================================
#ifdef DLL_PROFILING


// =============================================================================
//  DLL_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define DLL_PROFILE_PULSE(pulseName)  \
        hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_SVC, (CP_ ## pulseName))


// =============================================================================
//  DLL_PROFILE_FUNCTION_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define DLL_PROFILE_FUNCTION_ENTER(eventName) \
        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_SVC, (CP_ ## eventName))


// =============================================================================
//  DLL_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define DLL_PROFILE_FUNCTION_EXIT(eventName) \
        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_SVC, (CP_ ## eventName))


// =============================================================================
//  DLL_PROFILE_WINDOW_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define DLL_PROFILE_WINDOW_ENTER(eventName) \
        hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_SVC, (CP_ ## eventName))


// =============================================================================
//  DLL_PROFILE_WINDOW_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define DLL_PROFILE_WINDOW_EXIT(eventName) \
        hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_SVC, (CP_ ## eventName))


#else // DLL_PROFILING

#define DLL_PROFILE_FUNCTION_ENTER(eventName)
#define DLL_PROFILE_FUNCTION_EXIT(eventName)
#define DLL_PROFILE_WINDOW_ENTER(eventName)
#define DLL_PROFILE_WINDOW_EXIT(eventName)
#define DLL_PROFILE_PULSE(pulseName)

#endif // DLL_PROFILING



#endif // _DLL_CFG_H_

