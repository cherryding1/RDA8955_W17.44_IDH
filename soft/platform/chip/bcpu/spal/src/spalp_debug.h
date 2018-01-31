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
#include "spal_profile_codes.h"

#ifndef _SPALP_DEBUG_H_
#define _SPALP_DEBUG_H_

// =============================================================================
//  BB IRQ defines for profiling
// =============================================================================
#define BB_IRQ_OTHER    0xF

// =============================================================================
//  PROFILING
// -----------------------------------------------------------------------------
/// Those are the macros to use for profiling
/// @todo Implement them with spal_DbgProfilingEnter etc instead of this :
// =============================================================================
#ifdef SPAL_PROFILING


// =============================================================================
//  SPAL_PROFILE_FUNCTION_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define SPAL_PROFILE_FUNCTION_ENTER(eventName) \
        spal_DbgPxtsProfileFunctionEnter(SPAL_DBG_PXTS_SPAL, (CP_ ## eventName))


// =============================================================================
//  SPAL_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define SPAL_PROFILE_FUNCTION_EXIT(eventName) \
        spal_DbgPxtsProfileFunctionExit(SPAL_DBG_PXTS_SPAL, (CP_ ## eventName))


// =============================================================================
//  SPAL_PROFILE_WINDOW_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define SPAL_PROFILE_WINDOW_ENTER(eventName) \
        spal_DbgPxtsProfileWindowEnter(SPAL_DBG_PXTS_SPAL, (CP_ ## eventName))


// =============================================================================
//  SPAL_PROFILE_WINDOW_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define SPAL_PROFILE_WINDOW_EXIT(eventName) \
        spal_DbgPxtsProfileWindowExit(SPAL_DBG_PXTS_SPAL, (CP_ ## eventName))


// =============================================================================
//  SPAL_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define SPAL_PROFILE_PULSE(pulseName)  \
                spal_DbgPxtsProfilePulse(SPAL_DBG_PXTS_SPAL, (CP_ ## pulseName))


#else // SPAL_PROFILING

#define SPAL_PROFILE_FUNCTION_ENTER(eventName)
#define SPAL_PROFILE_FUNCTION_EXIT(eventName)
#define SPAL_PROFILE_WINDOW_ENTER(eventName)
#define SPAL_PROFILE_WINDOW_EXIT(eventName)
#define SPAL_PROFILE_PULSE(pulseName)

#endif // SPAL_PROFILING


#ifdef SPAL_IRQ_PROFILING
#define SPAL_PROFILE_IRQ_ENTER(eventName) \
        spal_DbgPxtsProfileWindowEnter(SPAL_DBG_PXTS_BB_IRQ, \
        (CPIRQSPACESTART |((eventName) & CPIRQSPACEMASK)) )
#define SPAL_PROFILE_IRQ_EXIT(eventName) \
        spal_DbgPxtsProfileWindowExit(SPAL_DBG_PXTS_BB_IRQ, \
        (CPIRQSPACESTART | ((eventName) & CPIRQSPACEMASK)) )
#else // SPAL_IRQ_PROFILING
#define SPAL_PROFILE_IRQ_ENTER(eventName)
#define SPAL_PROFILE_IRQ_EXIT(eventName)
#endif

// TODO: Always active?
#define SPAL_PROFILE_FINT spal_DbgPxtsProfilePulse(SPAL_DBG_PXTS_BB_IRQ, CPFINT)

#define SPAL_PROFILE_BB_IFC2_SYM_CNT(num) \
        spal_DbgPxtsProfilePulse(SPAL_DBG_PXTS_BB_IFC2_SYM_CNT, \
        (CPBBIFC2SYMCNTSTART + ((num>=CPBBIFC2SYMCNTMAX)?CPBBIFC2SYMCNTMAX:num)) )


#endif // _SPALP_DEBUG_H_







