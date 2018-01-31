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


#ifndef _MPSP_DEBUG_H_
#define _MPSP_DEBUG_H_

#include "cs_types.h"

#include "hal_debug.h"

#include "sxs_io.h"

#include "mps_profile_codes.h"


// =============================================================================
// MACROS
// =============================================================================

#ifdef MPS_PRINTF
#define MPS_TRACE(format, ...)                                    \
    hal_DbgTrace(TSTDOUT, 0, format, ##__VA_ARGS__)
#else
#define MPS_TRACE(fmt, ...)
#endif


#ifdef MPS_NO_ASSERT
#define MPS_ASSERT(BOOL, format, ...)
#else
#define MPS_ASSERT(BOOL, format, ...)                   \
    if (!(BOOL)) {            \
        hal_DbgAssert(format, ##__VA_ARGS__);                             \
    }
#endif

#ifdef MPS_PROFILING

// =============================================================================
//  MPS_PROFILE_FUNCTION_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define MPS_PROFILE_FUNCTION_ENTER(eventName) \
        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_SVC, (CP_ ## eventName))

// =============================================================================
//  MPS_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define MPS_PROFILE_FUNCTION_EXIT(eventName) \
        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_SVC, (CP_ ## eventName))

// =============================================================================
//  MPS_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define MPS_PROFILE_PULSE(pulseName)  \
        hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_SVC, (CP_ ## pulseName))

#else

#define MPS_PROFILE_FUNCTION_ENTER(eventName)
#define MPS_PROFILE_FUNCTION_EXIT(eventName)
#define MPS_PROFILE_PULSE(pulseName)

#endif /* MPS_PROFILING */

// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

#endif // _MPSP_DEBUG_H_
