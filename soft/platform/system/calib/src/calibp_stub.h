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



#ifndef _CALIBP_STUB_H_
#define _CALIBP_STUB_H_

#include "cs_types.h"
#include "calib_m.h"
#include "calib_stub_nst.h"


// =============================================================================
// g_calibStubGlobalCtxPtr
// -----------------------------------------------------------------------------
/// Pointert to the calibration context written by the Host to send command
/// to the CES.
// =============================================================================
EXPORT PROTECTED VOLATILE CALIB_STUB_CTX_T*    g_calibStubGlobalCtxPtr;
EXPORT PROTECTED VOLATILE  CALIB_STUB_NST_CTX_T g_calibStubGlobalNstCtx;

// =============================================================================
// g_calibStubLocalCtx
// -----------------------------------------------------------------------------
/// Context used by the CES, copied from the #g_calibStubGlobalCtx.
// =============================================================================
EXPORT PROTECTED CALIB_STUB_CTX_T            g_calibStubLocalCtx;



// =============================================================================
// calib_DispState
// -----------------------------------------------------------------------------
/// Display depending on the states.
/// @param full If \c TRUE,  clear the screen and rewrite everything.
// =============================================================================
PROTECTED VOID calib_DispState(BOOL full);



// =============================================================================
// calib_PulseLight
// -----------------------------------------------------------------------------
/// I am alive and I claim it ! function, through a PWL (Keyboard backlighting
/// presumably. The pulsing speed is used to display the calibration state.
///
/// @param speed Speed of the glow.
// =============================================================================
PROTECTED VOID calib_PulseLight(INT32 speed);




// =============================================================================
// calib_StubOpen
// -----------------------------------------------------------------------------
/// Initializes the calibration stub. Among operation is the registering of the
/// calib stub state machine as the FINT Irq handler.
// =============================================================================
PROTECTED VOID calib_StubOpen(BOOL Flag);



// =============================================================================
// calib_StubTaskInit
// -----------------------------------------------------------------------------
/// Calib Stub OS task.
// =============================================================================
PROTECTED VOID calib_StubTaskInit(VOID);


// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================





#endif // _CALIBP_STUB_H_

