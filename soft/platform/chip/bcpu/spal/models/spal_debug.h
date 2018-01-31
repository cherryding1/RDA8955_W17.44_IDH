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


#ifndef SPAL_DEBUG_H
#define SPAL_DEBUG_H

#include "cs_types.h"
#include "global_macros.h"

// =============================================================================
//  MACROS
// =============================================================================

#define CPMASK 0x3fff
#define CPEXITFLAG 0x8000
#define CPFINT 0x7fff

#define CPBCPU 0x4000 // hardcoded in spal_PXTS_SendProfilingCode function

#define CPPULSESSPACESTART 0x2f00
#define CPPULSESSPACEMASK 0x7ff

#define CPWINDOWSPACESTART 0x3700
#define CPWINDOWSPACEMASK 0x7ff

#define CPTASKSPACESTART 0x3f00
#define CPTASKSPACEMASK 0x1f

#define CPIRQSPACESTART 0x3f20
#define CPIRQSPACEMASK 0x1f

#define CPJOBPACESTART 0x3f40
#define CPJOBSPACEMASK 0x1f

// SPAL DEBUG PROFILING FUNCTIONS

///@defgroup spal_debug SPAL Debug Functions
///@{


// =============================================================================
// spal_DbgPxtsSendCode
// -----------------------------------------------------------------------------
/// Profiling tool (reserved for Platform profiling). To use this feature,
/// the PXTS device must have been enabled on the debug bus (spal_DbgPortSetup)
/// This function is reserved for platform profiling. It is used to send
/// a serie of event to a profiling tool (like CSBJ SA Tool) through the
/// romulator. \n
/// When this function is called, it sends an event represented by its \c code
/// to the PXTS Fifo. This Fifo is read by en external tool. \n
/// \n
/// (Nota: SA tool and Coolprofile cannot be used at the same time)
///
/// @param code The event code to send to the profiling tool
// =============================================================================
INLINE VOID spal_DbgPxtsSendCode(UINT16 code)
{
    ;
}



// =============================================================================
// spal_DbgPxtsProfileFunctionEnter
// -----------------------------------------------------------------------------
/// This function has to be called when entering the function you want to profile.
///
/// @param functionId Representing the function.
// =============================================================================
INLINE VOID spal_DbgPxtsProfileFunctionEnter(UINT16 functionId)
{
    spal_DbgPxtsSendCode((functionId & CPMASK) | CPBCPU);
}



// =============================================================================
// spal_DbgPxtsProfileFunctionExit
// -----------------------------------------------------------------------------
/// This function has to be called when exiting the function you want to profile.
///
/// @param functionId Code representing the function.
// =============================================================================
INLINE VOID spal_DbgPxtsProfileFunctionExit(UINT16 functionId)
{
    spal_DbgPxtsSendCode((functionId & CPMASK) | CPEXITFLAG | CPBCPU);
}



// =============================================================================
// spal_DbgPxtsProfileWindowEnter
// -----------------------------------------------------------------------------
/// This function has to be called to mark the entrance in a window
///
/// @param windowId Code representing the window which has been entered in.
// =============================================================================
INLINE VOID spal_DbgPxtsProfileWindowEnter(UINT16 windowId)
{
    spal_DbgPxtsSendCode((windowId & CPMASK) | CPBCPU);
}



// =============================================================================
// spal_DbgPxtsProfileWindowExit
// -----------------------------------------------------------------------------
/// This function has to be called to mark the exit of a window
///
/// @param windowId Code representing the window which has been exited.
// =============================================================================
INLINE VOID spal_DbgPxtsProfileWindowExit(UINT16 windowId)
{
    spal_DbgPxtsSendCode((windowId & CPMASK) | CPEXITFLAG | CPBCPU);
}



// =============================================================================
// spal_DbgPxtsProfileTaskEnter
// -----------------------------------------------------------------------------
/// Send to the profiling tool the code corresponding to scheduled task Id..
///
/// @param taskId Task id of the scheduled task.
// =============================================================================
INLINE VOID spal_DbgPxtsProfileTask(UINT16 taskId)
{
    spal_DbgPxtsSendCode(CPTASKSPACESTART | ((taskId) & CPTASKSPACEMASK) | CPBCPU);
}


// =============================================================================
// spal_DbgPxtsProfilePulse
// -----------------------------------------------------------------------------
/// Send to the profiling tool the code correponding to a pulse
///
/// @param code Code representing the pulse
// =============================================================================
INLINE VOID spal_DbgPxtsProfilePulse(UINT16 code)
{
    spal_DbgPxtsSendCode((code & CPMASK) | CPBCPU);
}


//======================================================================
// spal_DbgHstSendEvent
//----------------------------------------------------------------------
/// Send event to host
/// @param ch channel
//======================================================================
PUBLIC BOOL spal_DbgHstSendEvent(UINT32 ch);


// =============================================================================
// spal_GdbOpen
// -----------------------------------------------------------------------------
/// Initializes the variables used by GDB.
// =============================================================================
VOID spal_GdbOpen(VOID);

///@}


#endif // SPAL_DEBUG_H
