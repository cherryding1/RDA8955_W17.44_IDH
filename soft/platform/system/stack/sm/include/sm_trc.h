//------------------------------------------------------------------------------
//  $Log: sm_trc.h,v $
//  Revision 1.5  2006/05/22 19:35:41  aba
//  Update of the copyright
//
//  Revision 1.4  2006/03/09 07:38:54  aba
//  commentary updating
//
//  Revision 1.3  2006/01/27 18:03:57  aba
//  new trace
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
  File       : SM_TRC.H
--------------------------------------------------------------------------------

  Scope      : Defintion of SM Trace Levels

  History    :
--------------------------------------------------------------------------------
  Aug 12 04  |  PCA   | Creation
================================================================================
*/
#ifndef __SM_TRC_H__
#define __SM_TRC_H__

#include "sxs_io.h"     // Trace constants
#include "cmn_defs.h"   // Common definitions



#define  SM_IN_TRC           (_SM | CMN_IN_TRC)   // Incoming messages
#define  SM_DIN_TRC          (_SM | CMN_DIN_TRC)  // Dump incoming messages
#define  SM_OUT_TRC          (_SM | CMN_OUT_TRC)  // Outgoing messages
#define  SM_DOUT_TRC         (_SM | CMN_DOUT_TRC) // Dump outcoming messages
#define  SM_FSM_TRC          (_SM | CMN_FSM_TRC)  // Finite State Machine

#define  SM_FUNC_TRC         (_SM | TLEVEL(6))    // Function name
#define  SM_CTX_TRC          (_SM | TLEVEL(7))    // PDP context managment
#define  SM_MODIFY_TRC       (_SM | TLEVEL(8))    // PDP CTX modification
#define  SM_ACTIVATION_TRC   (_SM | TLEVEL(9))    // PDP CTX activation
#define  SM_DEACTIVATION_TRC (_SM | TLEVEL(10))   // PDP CTX deactivation
#define  SM_QOS_TRC          (_SM | TLEVEL(11))   // Quality of service
#define  SM_ROUTE_TRC        (_SM | TLEVEL(12))   // Route functions
#define  SM_DEC_MSG_TRC      (_SM | TLEVEL(13))   // Decoding of the network message
#define  SM_TI_TRC           (_SM | TLEVEL(14))   // Transaction identifier
#define  SM_RETRANS_TRC      (_SM | TLEVEL(15))   // Retransmission of message


#define  SM_DBG_TRC    (_SM | TLEVEL(16))   // SM debug



#endif /* endif __SM_TRC_H__ */

