//------------------------------------------------------------------------------
//  $Header: /usr/local/cvsroot/rlu/inc/rlu_trce.h,v 1.2 2006/03/08 00:18:10 mce Exp $
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmission of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : rlu_trce.h
--------------------------------------------------------------------------------

  Scope      : Defintion of trace Levels for rlu  

  History    :
--------------------------------------------------------------------------------
  Feb 06 04  |  MCE   | Creation            
================================================================================
*/
#ifndef __RLU_TRCE_H__
#define __RLU_TRCE_H__

#include "sxs_io.h"
#include "cmn_defs.h"

#define  RLU_IN_TRC    (_RLU | CMN_IN_TRC)    // Incoming messages
#define  RLU_DIN_TRC   (_RLU | CMN_DIN_TRC)   // Dump incoming messages
#define  RLU_OUT_TRC   (_RLU | CMN_OUT_TRC)   // Outgoing messages
#define  RLU_DOUT_TRC  (_RLU | CMN_DOUT_TRC)  // Dump outcoming messages
#define  RLU_FSM_TRC   (_RLU | CMN_FSM_TRC)   // Finite state machine
#define  RLU_DRI_TRC   (_RLU | CMN_DRMIN_TRC) // Dump incoming radio msgs
#define  RLU_DRO_TRC   (_RLU | CMN_DRMOUT_TRC)// Dump outgoing radio msgs

#define  RLU_ARQ_TRC   (_RLU | TLEVEL(8))    // ARQ procedures
#define  RLU_SEG_TRC   (_RLU | TLEVEL(9))    // Segmentation/concatenation procs
#define  RLU_RCF_TRC   (_RLU | TLEVEL(10))   // TBF reconf procedures
#define  RLU_UACK_TRC  (_RLU | TLEVEL(11))   // UnAck mode
#define  RLU_CV_TRC    (_RLU | TLEVEL(12))   // Countdown procedure
#define  RLU_FCT_TRC   (_RLU | TLEVEL(13))   // Flow Control

#define  RLU_TRDY_TRC  (_RLU | TLEVEL(14))   // TReady procedures    
#define  RLU_TX_TRC    (_RLU | TLEVEL(15))   // Transmission timing traces
#define  RLU_GSUS_TRC  (_RLU | TLEVEL(16))   // GPRS suspension / resumption
#define  RLU_RLS_TRC   RLU_ARQ_TRC           // Release of TBF
#define  RLU_RES_TRC   RLU_RCF_TRC           


#endif
