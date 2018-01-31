//------------------------------------------------------------------------------
//  $Header: /usr/local/cvsroot/rlu/inc/rlu_fsm.h,v 1.2 2006/03/08 00:18:10 mce Exp $
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
  File       : rlu_fsm.h  
--------------------------------------------------------------------------------

  Scope      : Finite State Machine defintions for rlu

  History    :
--------------------------------------------------------------------------------
  Jul 07 06  |  MCE   | Removed rluVoid_TrsPurgeReq
  Jun 17 06  |  MCE   | Added rluGene_TrsSuspendReq, rluGene_TrsResumeReq, 
             |        | rluVoid_TrsPrurgeReq, rluVoid_TrsResumeReq
  Apr 05 06  |  MCE   | Added rluGene_TrsCellOptInd      
  Jan 31 06  |  MCE   | Added rlu_TrsTxDiscardInd and rlu_TrsTxDiscardInd
  Aug 24 05  |  MCE   | Added rluCond_TrsUlRelReq
  Fev 06 04  |  MCE   | Creation
================================================================================
*/

#ifndef __RLU_FSM_H__
#define __RLU_FSM_H__
#include "rlu_data.h"

/*
** Save message and save event macros 
*/
#ifdef STK_SINGLE_SIM
#define RLU_SAVE_MSG(_Msg)     fsm_Save ( &rlu_Fsm, TRUE, _Msg)
#define RLU_SAVE_EVT(_Evt)     fsm_Save ( &rlu_Fsm, FALSE, _Evt)
#else
#define RLU_SAVE_MSG(_Msg)     fsm_Save ( &rlu_Fsm[pRLUCtx->SimIndex], TRUE, _Msg)
#define RLU_SAVE_EVT(_Evt)     fsm_Save ( &rlu_Fsm[pRLUCtx->SimIndex], FALSE, _Evt)
#endif
u8 rlu_TrsSaveMsg (void);

/*
==========================
  RLU States definitions
==========================
*/
#define RLU_GENE 0
#define RLU_VOID (RLU_GENE + 1 + FSM_STABLE) // no connection is started
#define RLU_WCON (RLU_GENE + 2)              // waiting for RR connection
#define RLU_COND (RLU_GENE + 3 + FSM_STABLE) // Uplink TBF established
#define RLU_WRLS (RLU_GENE + 4 + FSM_STABLE) // CV=0, waiting for release
#define RLU_NACK (RLU_GENE + 5)              // RLU is repeating NACKed PDUs
#define RLU_TEST  (RLU_GENE + 6)  		//RLU is In test ModeB
/* 
==========================
   Transitions prototypes
==========================
*/
// Common to several states but not used in RLU_GENE state (rlu_trsc.c)
u8 rlu_TrsContResInd          ( void ) ;
u8 rlu_TrsAckNackInd          ( void ) ;
u8 rlu_TrsTxFillReq           ( void ) ;
u8 rlu_TrsT3182               ( void ) ;
u8 rlu_TrsSuspReq             ( void ) ;
u8 rlu_TrsResReq              ( void ) ;
u8 rlu_TrsPurgeDQInd          ( void ) ;
u8 rlu_TrsTxDiscardInd        ( void ) ;

// RLU_GENE
u8 rluGene_TrsTlliInd         ( void ) ;
u8 rluGene_TrsQDataReq        ( void ) ;
u8 rluGene_TrsTxInd           ( void ) ;
u8 rluGene_TrsRelInd          ( void ) ;
u8 rluGene_TrsTReadyReq       ( void ) ;
u8 rluGene_TrsTReady          ( void ) ;
u8 rluGene_TrsPurgeDQInd      ( void ) ;
u8 rluGene_TrsCellOptInd      ( void ) ;
u8 rluGene_TrsSuspendReq      ( void ) ;
u8 rluGene_TrsResumeReq       ( void ) ;
u8 rluGene_TrsPurgeReq        ( void ) ;
u8 rluGene_TrsTPurgeExp       ( void ) ;

// RLU_VOID
u8   rluVoid_TrsQDataReq      ( void ) ;
u8   rluVoid_TrsTestModeReq   ( void ) ;
u8   rluVoid_TrsResumeReq     ( void ) ;
#ifdef __EGPRS__
u8   rluVoid_TrsLoopBackReq   ( void ) ;
#endif
// RLU_WCON
u8 rluWcon_TrsConnectCnf      ( void ) ;
u8 rluWcon_TrsContResInd      ( void ) ;

// RLU_COND
u8 rluCond_TrsContinueTx      ( void ) ;
u8 rluCond_TrsConnectCnf      ( void ) ;
u8 rluCond_TrsUlRelReq        ( void ) ;

// RLU_WRLS
u8 rluWrls_TrsQDataReq        ( void ) ;

// RLU_NACK
u8 rluNack_TrsRepeatReq       ( void ) ;
u8 rluNack_TrsAckNackInd      ( void ) ;
u8 TestMode_TrsConnectCnf   (void);
u8 TestMode_DataProcess (void);
u8 TestMode_MMTestModeReq(void);

#endif

