//------------------------------------------------------------------------------
//  $Log: llc_fsm.h,v $
//  Revision 1.3  2006/05/23 10:36:46  fch
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:46  pca
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
  File       : LLC_FSM.H
--------------------------------------------------------------------------------

  Scope      : Finite State Machine defintions for LLC

  History    :
--------------------------------------------------------------------------------
  Mar 22 04  |  FCH   | Creation
================================================================================
*/
#ifndef __LLC_FSM_H__
#define __LLC_FSM_H__

#include "fsm.h"

//============================
//  LLE states definition
//============================
#define LLE_GENE        0
#define LLE_NULL        1
#define LLE_ADM         2   // Asynchronous Disconnected Mode
#define LLE_REM_EST     3   // Remote establishment
#define LLE_LOC_EST     4   // Local establishment
#define LLE_ABM         5   // Asynchronous Balanced Mode
#define LLE_LOC_REL     6   // Local release



//============================
// FSM context data
//============================
#ifndef STK_SINGLE_SIM
extern Fsm_t lle_Fsm[][6];//the number of LLE fsms
#else
extern Fsm_t lle_Fsm[];
#endif

//============================
// Transitions prototypes
//============================
extern u8 llc_TrsDoNothing                  ( void );
extern u8 llc_TrsRxUnsolicitedUA            ( void );
extern u8 llc_TrsRxXID                      ( void );
extern u8 llc_TrsXidReq                     ( void );
extern u8 llc_TrsXidRsp                     ( void );

extern u8 llcGene_TrsClearLleCtx              ( void );
extern u8 llcGene_TrsRxUUndefFrame          ( void );
extern u8 llcGene_TrsRxUIFrame              ( void );
extern u8 llcGene_TrsQueueDataReq           ( void );
extern u8 llcGene_TrsReset                  ( void );
extern u8 llcGene_TrsRxSABM                 ( void );
extern u8 llcGene_TrsT200Exp                ( void );
extern u8 llcGene_TrsT201Exp                ( void );
extern u8 llcGene_TrsXidReq                 ( void );
extern u8 llcGene_TrsResumeReq              ( void );
extern u8 llcGene_RxFRMR                    ( void );

extern u8 llcNull_TrsInitLleCtx           ( void );

extern u8 llcAdmAbm_TrsEstabReq             ( void );
extern u8 llcAdm_TrsSndDM_0                 ( void );
extern u8 llcAdm_TrsRxDISC                  ( void );
extern u8 llcAdm_TrsRelReq                  ( void );
extern u8 llcAdm_TrsNullFrame               ( void );
extern u8 llcAdm_TrsTriggerReq              ( void );

extern u8 llcLocEst_TrsT200Exp              ( void );
extern u8 llcLocEst_TrsRxUA                 ( void );
extern u8 llcLocEst_TrsRxDM                 ( void );
extern u8 llcLocEst_TrsRxDISC               ( void );
extern u8 llcLocEst_TrsRelReq               ( void );

extern u8 llcRemEst_TrsEstabRsp             ( void );
extern u8 llcRemEst_TrsRelReq               ( void );

extern u8 llcAbm_TrsRelReq                  ( void );
extern u8 llcAbm_TrsRxDM                    ( void );
extern u8 llcAbm_TrsRxDISC                  ( void );
extern u8 llcAbm_TrsRxIFrm                  ( void );
extern u8 llcAbm_TrsRxSFrm                  ( void );
extern u8 llcAbm_TrsT201Exp                 ( void );
extern u8 llcAbm_TrsSuspendReq              ( void );
extern u8 llcAbm_TrsResumeReq               ( void );
extern u8 llcAbm_TrsTriggerReq              ( void );
extern u8 llcAbm_TrsChangLlcParReq          ( void );
extern u8 llcAbm_TrsRxUUndefFrame           ( void );

extern u8 llcLocRel_TrsRxSABM               ( void );
extern u8 llcLocRel_TrsRxUA_DM              ( void );
extern u8 llcLocRel_TrsRxDISC               ( void );

#endif // endif __LLC_FSM_H__



