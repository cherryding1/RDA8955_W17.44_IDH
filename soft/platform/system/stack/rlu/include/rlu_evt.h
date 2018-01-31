//------------------------------------------------------------------------------
//  $Header: /usr/local/cvsroot/rlu/inc/rlu_evt.h,v 1.2 2006/03/08 00:18:10 mce Exp $
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
  File       : rlu_evt.h  
--------------------------------------------------------------------------------

  Scope      : Declaration of all Events of Rlu task

  History    :
--------------------------------------------------------------------------------
  Jun 17 06  |  MCE   | Added rlu_llcSuspendReqEvt, rlu_llcResumeReqEvt,
             |        | rlu_llcPurgeReqEvt, rlu_TPurgeExpEvt
  Apr 05 06  |  NCE   | Addition of rlu_rrpCellOptIndEvt     
  Jan 31 06  |  MCE   | Addition of rlu_l1TxDiscartIndEvt 
  Jan 03 06  |  MCE   | Addition of rlu_l1TxDiscardReqEvt     
  May 10 05  |  MCE   | Addition of rlu_llcTestModeReqEvt    
  Apr 22 05  |  MCE   | Addition of rlu_macPurgeDQEvt and rlu_l1PurgeDQIndEvt
  Oct 29 04  |  MCE   | Addition of rlu_mmpTReadyReqEvt and rlu_mmpTReadyIndEvt
  Sep 09 04  |  MCE   | Addition of rlu_QCreateIndEvt and rlu_QDataReqEvt 
  Feb 06 04  |  MCE   | Creation            
================================================================================
*/
#ifndef __RLU_EVT_H__
#define __RLU_EVT_H__

#include "fsm.h"         // EvtDsc_t Type definition

/*
** Flags for incoming events
*/




#define RLU_IL          ( 1 << 0 )
#define RLU_NO_FSM_TRC  ( 1 << 1 ) 

/*
==========================================
             Incoming Events
==========================================
*/
//  Incoming RLU <-- RLU events
extern InEvtDsc_t   rlu_IStartTbfReqEvt      ;
extern InEvtDsc_t   rlu_IContinueTxEvt       ;
extern InEvtDsc_t   rlu_IRepeatReqEvt        ;
extern InEvtDsc_t   rlu_ITxFillReqEvt        ;

//  Timers 
extern InEvtDsc_t   rlu_T3182ExpEvt          ;
extern InEvtDsc_t   rlu_TReadyExpEvt         ;
extern InEvtDsc_t   rlu_TPurgeExpEvt         ;

//  Incoming RLU <-- LLC events
extern InEvtDsc_t   rlu_llcQDataReqEvt       ;
extern InEvtDsc_t   rlu_llcTestModeReqEvt    ;
extern InEvtDsc_t   rlu_llcSuspendReqEvt     ;
extern InEvtDsc_t   rlu_llcResumeReqEvt      ;
extern InEvtDsc_t   rlu_llcPurgeReqEvt       ;

//  Incoming RLU <-- RR events
extern InEvtDsc_t   rlu_rrpUlConnectCnfEvt   ;
extern InEvtDsc_t   rlu_rrpUlRelStartReqEvt  ;
extern InEvtDsc_t   rlu_rrpUlRelIndEvt       ;
extern InEvtDsc_t   rlu_rrpCellOptIndEvt     ;

//  Incoming RLU <-- MAC(RR) events
extern InEvtDsc_t   rlu_macContResIndEvt     ;
extern InEvtDsc_t   rlu_macTlliIndEvt        ;
extern InEvtDsc_t   rlu_macAckNackIndEvt     ;
#ifdef __RLU_NO_AUTO_SUSPEND__
extern InEvtDsc_t   rlu_macSuspReqEvt        ;
extern InEvtDsc_t   rlu_macResReqEvt         ;
#endif

//  Incoming RLU <-- L1 events 
extern InEvtDsc_t   rlu_l1TxIndEvt           ;
extern InEvtDsc_t   rlu_l1PurgeDQIndEvt      ;
extern InEvtDsc_t   rlu_l1TxDiscardIndEvt    ;

//  Incoming RLU <-- MMP events 
extern InEvtDsc_t   rlu_mmpTReadyReqEvt      ;
#ifdef __EGPRS__
extern InEvtDsc_t   rlu_mmpEGLoopBackModeEvt ;
#endif

//  Derived events
extern InEvtDsc_t   rlu_llcDataReqEvt        ;
extern InEvtDsc_t   rld_testdataReqEvt;

/*
==========================================
             Outgoing Events
==========================================
*/

//  Outgoing RLU --> RLU events
extern OutEvtDsc_t rlu_OStartTbfReqEvt    ;
extern OutEvtDsc_t rlu_OContinueTxEvt     ;
extern OutEvtDsc_t rlu_ORepeatReqEvt      ;
extern OutEvtDsc_t rlu_OTxFillReqEvt      ;

//  Outgoing  RLU --> RR Events
extern OutEvtDsc_t rlu_rrpConnectReqEvt   ;
extern OutEvtDsc_t rlu_rrpReleaseReqEvt   ;

//  Outgoing  RLU --> MAC(RR) events
extern OutEvtDsc_t rlu_macDataReqEvt      ;
extern OutEvtDsc_t rlu_macPurgeDQEvt      ;

//  Outgoing  RLU --> LLC events
extern OutEvtDsc_t rlu_QCreateIndEvt      ;

//  Outgoing RLU --> MMP events 
extern OutEvtDsc_t rlu_mmpTReadyIndEvt    ;
extern OutEvtDsc_t rlu_mmpSynchIndEvt     ;


extern OutEvtDsc_t rlu_l1TxDiscardReqEvt  ; 
extern OutEvtDsc_t rlu_l1TestModeReqEvt;

extern OutEvtDsc_t rlu_UlCongestIndEvt    ;
extern OutEvtDsc_t  rlu_OTestModeReqEvt;

extern OutEvtDsc_t rlu_rrpTestModeReqEvt;

#endif
