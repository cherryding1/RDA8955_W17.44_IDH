//------------------------------------------------------------------------------
//  $Log: cb_trc.h,v $
//  Revision 1.4  2006/05/23 10:36:37  fch
//  Update Copyright
//
//  Revision 1.3  2006/01/19 18:25:25  fch
//  - In function cb_7To8bitsData, init Tmp and Mask to avoid warning
//  - Merge trace levels with SMS
//
//  Revision 1.2  2005/12/29 17:38:40  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/

/*
================================================================================

  File       : CB_TRC.H
--------------------------------------------------------------------------------

  Scope      : CB trace levels

  History    :
--------------------------------------------------------------------------------
  Jun 08 05  |  FCH   | Creation            
================================================================================
*/
#ifndef __CB_TRC_H__
#define __CB_TRC_H__

#include "sxs_io.h"     // Trace constants
#include "cmn_defs.h"   // Common definitions




//==============================================================
//  TRACE LEVELS DEFINITION
//==============================================================

#define  CB_IN_TRC    (_SMS | CMN_IN_TRC)   // Incoming messages
#define  CB_DIN_TRC   (_SMS | CMN_DIN_TRC)  // Dump incoming messages
#define  CB_OUT_TRC   (_SMS | CMN_OUT_TRC)  // Outgoing messages
#define  CB_DOUT_TRC  (_SMS | CMN_DOUT_TRC) // Dump outcoming messages
#define  CB_FSM_TRC   (_SMS | CMN_FSM_TRC)  // Finite State Machine

#define  CB_BLK_TRC   (_SMS | TLEVEL(13))   // Block
#define  CB_CBMSG_TRC (_SMS | TLEVEL(14))   // CB message (schedule or SMSCB page)
#define  CB_SCHED_TRC (_SMS | TLEVEL(15))   // Schedule purpose
#define  CB_OTHER_TRC (_SMS | TLEVEL(16))   // Other traces

#endif // endif __CB_TRC_H__


