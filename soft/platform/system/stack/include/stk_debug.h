////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL$ //
//	$Author$                                                        // 
//	$Date$                     //   
//	$Revision$                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file stk_debug.h                                                          //
/// That file provides assert, trace and profiling macros for use in          //
/// Stack implementation                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "hal_debug.h"
#include "stk_profile_codes.h"
#include "stkp_profile.h"

#ifndef STK_DEBUG_H
#define STK_DEBUG_H

// =============================================================================
//  PROFILING
// -----------------------------------------------------------------------------
/// Those are the macros to use for profiling
/// @todo Implement them with hal_DbgProfilingEnter etc instead of this :
// =============================================================================
#ifdef STK_PROFILING

// =============================================================================
//  STK_PROFILE_FUNCTION_ENTRY
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define STK_PROFILE_FUNCTION_ENTRY(eventName, flag) \
{\
    if((flag) & STACK_PROFILE_TO_HAL){ \
        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_STACK, (CP_ ## eventName));	\
    }   \
    if((flag) & STACK_PROFILE_TO_STACK){ \
    	stk_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_STACK, (CP_ ## eventName));	\
    }   \
}


// =============================================================================
//  STK_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define STK_PROFILE_FUNCTION_EXIT(eventName, flag) \
{\
	if((flag) & STACK_PROFILE_TO_HAL){	\
        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_STACK, (CP_ ## eventName));	\
    }   \
    if((flag) & STACK_PROFILE_TO_STACK){	\
    	stk_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_STACK, (CP_ ## eventName));	\
    }	\
}


// =============================================================================
//  STK_PROFILE_WINDOW_ENTRY
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define STK_PROFILE_WINDOW_ENTRY(eventName)  \
        hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_STACK, (CP_ ## eventName))



// =============================================================================
//  STK_PROFILE_WINDOW_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define STK_PROFILE_WINDOW_EXIT(eventName)  \
        hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_STACK, (CP_ ## eventName))



// =============================================================================
//  STK_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define STK_PROFILE_PULSE(pulseName)  \
hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_STACK, (CP_ ## pulseName))



#else // STK_PROFILING

#define STK_PROFILE_FUNCTION_ENTRY(eventName,flag)
#define STK_PROFILE_FUNCTION_EXIT(eventName,flag)
#define STK_PROFILE_WINDOW_ENTRY(eventName)
#define STK_PROFILE_WINDOW_EXIT(eventName)
#define STK_PROFILE_PULSE(pulseName)

#endif // STK_PROFILING



#endif // STK_DEBUG_H







