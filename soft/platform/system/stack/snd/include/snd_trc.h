//------------------------------------------------------------------------------
//  $Log: snd_trc.h,v $
//  Revision 1.5  2006/05/23 14:37:18  aba
//  Update of the copyright
//
//  Revision 1.4  2006/03/16 13:38:33  aba
//  New trace level for RAU
//
//  Revision 1.3  2006/01/13 09:54:52  aba
//  new trace level for discarded frames
//
//  Revision 1.2  2005/12/29 17:39:17  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003 2004 2005 2006

================================================================================
*/

/*
================================================================================
  File       : SND_TRC.H
--------------------------------------------------------------------------------

  Scope      : Defintion of SND Trace Levels

  History    :
--------------------------------------------------------------------------------
  Sep 06 04  |  ABA   | Creation
================================================================================
*/
#ifndef __SND_TRC_H__
#define __SND_TRC_H__

#include "sxs_io.h"     // Trace constants
#include "cmn_defs.h"   // Common definitions



#define  SND_IN_TRC      (_SND | CMN_IN_TRC)   // Incoming messages
#define  SND_DIN_TRC     (_SND | CMN_DIN_TRC)  // Dump incoming messages
#define  SND_OUT_TRC     (_SND | CMN_OUT_TRC)  // Outgoing messages
#define  SND_DOUT_TRC    (_SND | CMN_DOUT_TRC) // Dump outcoming messages
#define  SND_FSM_TRC     (_SND | CMN_FSM_TRC)  // Finite State Machine

#define  SND_REAS_TRC    (_SND | TLEVEL(6))    // Reassembling functions
#define  SND_RX_TRC      (_SND | TLEVEL(7))    // Receiving segments functions
#define  SND_TX_TRC      (_SND | TLEVEL(8))    // Segmentation functions
#define  SND_RECOV_TRC   (_SND | TLEVEL(9))    // Recovery mode managment
#define  SND_MODIFY_TRC  (_SND | TLEVEL(10))   // Processing of SM_MODIFY_IND
#define  SND_DEACT_TRC   (_SND | TLEVEL(11))   // Deactivation, release
#define  SND_ACTIV_TRC   (_SND | TLEVEL(12))   // Activation, establishment
#define  SND_XID_TRC     (_SND | TLEVEL(13))   // XID processing
#define  SND_DISCARD_TRC (_SND | TLEVEL(14))   // Discarded frames
#define  SND_RAU_TRC     (_SND | TLEVEL(15))   // Routing area update
#endif /* endif __SND_TRC_H__ */
