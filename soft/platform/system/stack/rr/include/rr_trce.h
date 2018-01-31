/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003 - 2006

================================================================================
*/

/*
================================================================================
  File       : rr_trce.h
--------------------------------------------------------------------------------

  Scope      : Defintion of Trace Level for rr  

  History    :
--------------------------------------------------------------------------------
  Jan 10 06  |  MCE   | suppressed RR trace levels ( moved to RRI)
  Nov 13 05  |  MCE   | Split RRI and RRD trace levels
  Jul 12 05  |  PFA   | Merge CSL, RSL, PSL Levels
  Jun 12 03  |  PFA   | Creation            
================================================================================
*/
#ifndef __RR_TRCE_H__
#define __RR_TRCE_H__

#include "sxs_io.h"
#include "cmn_defs.h"




#define  RRI_IN_TRC    (_RRI | CMN_IN_TRC)    // Incoming messages
#define  RRI_DIN_TRC   (_RRI | CMN_DIN_TRC)   // Dump incoming messages
#define  RRI_OUT_TRC   (_RRI | CMN_OUT_TRC)   // Outgoing messages
#define  RRI_DOUT_TRC  (_RRI | CMN_DOUT_TRC)  // Dump outcoming messages
#define  RRI_FSM_TRC   (_RRI | CMN_FSM_TRC)   // Finite state machine
#define  RRI_DRI_TRC   (_RRI | CMN_DRMIN_TRC) // Dump incoming radio msgs
#define  RRI_DRO_TRC   (_RRI | CMN_DRMOUT_TRC)// Dump outgoing radio msgs

#define  RRI_CSL_TRC   (_RRI | TLEVEL(8))    // Cell selection
#define  RRI_RSL_TRC   (_RRI | TLEVEL(8))    // Cell reselction
#define  RRI_PSL_TRC   (_RRI | TLEVEL(8))    // Plmn selection
#define  RRI_FRQ_TRC   (_RRI | TLEVEL(9))    // Frequency lists
#define  RRI_MON_TRC   (_RRI | TLEVEL(10))   // Monitor list
#define  RRI_CBCH_TRC  (_RRI | TLEVEL(13))   // CBCH related traces
#define  RRI_BCH_TRC   (_RRI | TLEVEL(14))   // RR (P)BCCH scheduling
#define  RRI_CODEC_TRC (_RRI | TLEVEL(16))   // RR enCOde DECode traces

#define  RR_IN_TRC     RRI_IN_TRC   
#define  RR_DIN_TRC    RRI_DIN_TRC  
#define  RR_OUT_TRC    RRI_OUT_TRC  
#define  RR_DOUT_TRC   RRI_DOUT_TRC 
#define  RR_FSM_TRC    RRI_FSM_TRC  
#define  RR_DRI_TRC    RRI_DRI_TRC  
#define  RR_DRO_TRC    RRI_DRO_TRC  
#define  RR_CSL_TRC    RRI_CSL_TRC
#define  RR_RSL_TRC    RRI_RSL_TRC
#define  RR_PSL_TRC    RRI_PSL_TRC
#define  RR_MON_TRC    RRI_MON_TRC 
#define  RR_FRQ_TRC    RRI_FRQ_TRC  
#define  RR_CODEC_TRC  RRI_CODEC_TRC



#define  RRD_IN_TRC    (_RRD | CMN_IN_TRC)    // Incoming messages
#define  RRD_DIN_TRC   (_RRD | CMN_DIN_TRC)   // Dump incoming messages
#define  RRD_OUT_TRC   (_RRD | CMN_OUT_TRC)   // Outgoing messages
#define  RRD_DOUT_TRC  (_RRD | CMN_DOUT_TRC)  // Dump outcoming messages
#define  RRD_FSM_TRC   (_RRD | CMN_FSM_TRC)   // Finite state machine
#define  RRD_DRI_TRC   (_RRD | CMN_DRMIN_TRC) // Dump incoming radio msgs
#define  RRD_DRO_TRC   (_RRD | CMN_DRMOUT_TRC)// Dump outgoing radio msgs

//#define  RRD_NCON_TRC  (_RRD | TLEVEL(11))   // RRD proc when no cnx started, no use for this macro, modified by wangqy 2009.12.1
#define  RRD_ERR_TRC    (_RRD | TLEVEL(11))   // RRD Exception Happened traces, added by wangqy 2009.12.1
#define  RRD_ACC_TRC   (_RRD | TLEVEL(12))   // RRD access procedures
#define  RRD_COND_TRC  (_RRD | TLEVEL(13))   // RRD procedures in COND state
#define  RRD_RCF_TRC   (_RRD | TLEVEL(14))   // RRD channel reconf procedures
#define  RRD_BZ_TRC    (_RRD | TLEVEL(15))   // RRD BaZard traces



#endif
