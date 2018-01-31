//------------------------------------------------------------------------------
//  $Log: lap_trc.h,v $
//  Revision 1.4  2006/05/22 12:46:12  aba
//  Copyrigth is updated
//
//  Revision 1.3  2006/02/22 16:12:28  aba
//  Updating of the trace levels
//
//  Revision 1.2  2005/12/29 17:38:45  pca
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
  File       : LAP_TRC.H
--------------------------------------------------------------------------------

  Scope      : Defintion of LAP Trace Levels

  History    :
--------------------------------------------------------------------------------
  Mar 22 05  |  LAP   | Creation
================================================================================
*/
#ifndef __LAP_TRC_H__
#define __LAP_TRC_H__

#include "sxs_io.h"     // Trace constants
#include "cmn_defs.h"   // Common definitions


#define  LAP_IN_TRC           (_LAP | CMN_IN_TRC)   // Incoming messages
#define  LAP_DIN_TRC          (_LAP | CMN_DIN_TRC)  // Dump incoming messages
#define  LAP_OUT_TRC          (_LAP | CMN_OUT_TRC)  // Outgoing messages
#define  LAP_DOUT_TRC         (_LAP | CMN_DOUT_TRC) // Dump outcoming messages
#define  LAP_FSM_TRC          (_LAP | CMN_FSM_TRC)  // Finite State Machine

#define  LAP_NTWERROR_TRC     (_LAP | TLEVEL(6))    // Network error traces
#define  LAP_ESTAB_TRC        (_LAP | TLEVEL(7))    // LAP establishement traces
#define  LAP_RELEASE_TRC      (_LAP | TLEVEL(8))    // LAP release traces
#define  LAP_SUSPEND_TRC      (_LAP | TLEVEL(9))    // Lap suspension, resume and reconnect traces
#define  LAP_RETRANS_TRC      (_LAP | TLEVEL(10))   // LAP retransmission, timer recovery
#define  LAP_FRAME_TRC        (_LAP | TLEVEL(11))   // LAP frame processing traces
#define  LAP_SEGMENT_TRC      (_LAP | TLEVEL(12))   // LAP segmentation and reassembling traces
#define  LAP_T200_TRC         (_LAP | TLEVEL(13))   // T200 traces (start/stop T200)
#define  LAP_TX_FRAME_TRC    (_LAP | TLEVEL(14))   // LAP frame Tx
#define  LAP_RX_FRAME_TRC    (_LAP | TLEVEL(15))   // LAP frame Rx




#endif /* endif __SM_TRC_H__ */

