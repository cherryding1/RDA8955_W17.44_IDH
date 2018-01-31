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
#include "sxs_io.h"
#include "csw_profile_codes.h"
#include "chip_id.h"

#ifndef CSWP_CFG_H
#define CSWP_CFG_H



// =============================================================================
//  PROFILING
// -----------------------------------------------------------------------------
/// Those are the macros to use for profiling
// =============================================================================
#ifdef CSW_PROFILING


// =============================================================================
//  CSW_PROFILE_FUNCTION_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define CSW_PROFILE_FUNCTION_ENTER(eventName) \
        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_CSW, (CP_ ## eventName))


// =============================================================================
//  CSW_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define CSW_PROFILE_FUNCTION_EXIT(eventName) \
        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_CSW, (CP_ ## eventName))


// =============================================================================
//  CSW_PROFILE_WINDOW_ENTRY
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define CSW_PROFILE_WINDOW_ENTRY(eventName) \
        hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_CSW, (CP_ ## eventName))


// =============================================================================
//  CSW_PROFILE_WINDOW_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define CSW_PROFILE_WINDOW_EXIT(eventName) \
        hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_CSW, (CP_ ## eventName))


// =============================================================================
//  CSW_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define CSW_PROFILE_PULSE(pulseName)  \
                hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_CSW, (CP_ ## pulseName))


#else // CSW_PROFILING

#define CSW_PROFILE_FUNCTION_ENTER(eventName)
#define CSW_PROFILE_FUNCTION_EXIT(eventName)
#define CSW_PROFILE_WINDOW_ENTRY(eventName)
#define CSW_PROFILE_WINDOW_EXIT(eventName)
#define CSW_PROFILE_PULSE(pulseName)

#endif // CSW_PROFILING

// =============================================================================
//  TRACE Level mapping
// -----------------------------------------------------------------------------
/// Important trace that can be enabled all the time (low rate)
#define CSW_WARN_TRC        (_CSW | TLEVEL(1))
/// BAL specific trace
#define CSW_BAL_TRC         (_CSW | TLEVEL(2))
/// PM specific trace
#define CSW_PM_TRC          (_CSW | TLEVEL(3))
/// @todo add levels and use them !
// add more here
/// reserved for debug (can be very verbose, but should probably not stay in code)
#define CSW_DBG_TRC         (_CSW | TLEVEL(16))

// =============================================================================
//  TRACE
// -----------------------------------------------------------------------------
/// Trace macro to use to send a trace. The parameter \c format is a string
/// containing parameters in the "print fashion", but limited for trace to 6
/// parameters. The parameter \c tstmap defines which parameter is a string.
/// (Leave it to 0, if you don't use %s in fmt);
// =============================================================================
// Conditional Printf Usage
/*
#ifndef CSW_NO_PRINTF
#define CSW_TRACE(level, tstmap, format, ...) \
     hal_DbgTrace(_CSW |(level),tstmap,format, ##__VA_ARGS__)
#else
#define CSW_TRACE(level, tsmap, fmt, ...)
#endif
*/

#endif // CSWP_CFG_H







