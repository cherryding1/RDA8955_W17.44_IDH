//------------------------------------------------------------------------------
//  $Log: mmp_fsm.h,v $
//  Revision 1.9  2006/05/23 10:51:30  fch
//  Update Copyright
//
//  Revision 1.8  2006/05/18 18:18:34  fch
//  Even if it is not a real case, handle AbortCurProc when in NULL or DREG state
//
//  Revision 1.7  2006/05/02 14:47:42  fch
//  In MMP_NULL and MMP_DREG states, if a TBF is still ongoing and a CS outgoing  call is to be made, MMP shall becomes inactive.
//
//  Revision 1.6  2006/04/20 13:47:01  fch
//  Suppress flag __MMP_NO_SUSPEND_REQ__
//
//  Revision 1.5  2006/02/15 15:45:41  fch
//  Add RR_SUSPEND_REQ handling.
//  Flag __MMP_NO_SUSPEND_REQ__ (temporary) allows to revert to MMP not handling RR_SUSPEND_REQ
//
//  Revision 1.4  2006/01/20 11:25:17  fch
//  Update when Fch moves to CVS
//
//  Revision 1.3  2006/01/17 15:09:01  oth
//  Merge from FCH version
//
//  Revision 1.2  2005/12/29 17:38:47  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of Coolsand. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright Coolsand (C) 2003-2006

================================================================================
*/
/*
================================================================================
  File       : mmp_fsm.h  
--------------------------------------------------------------------------------

  Scope      : Finite State Machine defintions for MMP

  History    :
--------------------------------------------------------------------------------
  Apr 01 04  |  OTH   | Creation            
================================================================================
*/

#ifndef __MMP_FSM_H__
#define __MMP_FSM_H__

#include "fsm.h"

#if !(STK_MAX_SIM==1)
extern Fsm_t  mmp_Fsm[STK_MAX_SIM] ;
#else
extern Fsm_t mmp_Fsm ;
#endif
/*
===================
  MMP definitions
===================
*/

/*
** Circuit Switch MM States definitions
** FSM mmp
*/
#define MMP_GENE   ( 0              )
#define MMP_NULL   ( 1 + FSM_STABLE )
#define MMP_DREG   ( 2 + FSM_STABLE )
#define MMP_PIP    ( 3 + FSM_STABLE )
#define MMP_REG    ( 4 + FSM_STABLE )
#define MMP_NDET   ( 5 + FSM_STABLE )
#define MMP_PEND   ( 6              )


/* 
** Transitions prototypes
*/
//--------------
// Generic State
//--------------
u8    mmp_LlcQueueCreateInd_Gene  ( void );
u8    mmp_ApiDetachReq_Gene       ( void );
u8    mmp_T3311Exp_Gene           ( void );
u8    mmp_T3302Exp_Gene           ( void );
u8    mmp_T3310Exp_Gene           ( void );
u8    mmp_T3330Exp_Gene           ( void );
u8    mmp_SmPdpCtxStatusInd_Gene  ( void );
u8    mmp_LlcNewTlliInd_Gene      ( void );
u8    mmp_LlcStatusInd_Gene       ( void );
u8    mmp_TestModeCmd_Gene        ( void );
u8    mmp_GprsPending_Gene        ( void );
u8    mmp_PReleaseInd_Gene        ( void );
u8    mmp_RlcSynchInd_Gene        ( void );
u8    mmp_RrSuspendReq_Gene       ( void );
u8    mmp_mmcAbortCurProc_Gene    ( void );
u8    mmp_TrsDoNothing            ( void );
u8 mmp_L3IDelSaveDataReq_Gene ( void );  // DMX add 20161031.  MMP_DEL_SAVE_DATA_REQ

/*added start by yangyang for EDGE at 2007-03-30*/
  #ifdef __EGPRS__
u8    mmp_ETestModeCmd_Gene ( void ); 
u8	 mmp_AccTechTypeCmd_Gene		(void);
  #endif
/*added end by yangyang for EDGE at 2007-03-30*/
//-----------
// Null State
//-----------
void  mmp_InitNull                ( void );
u8    mmp_RrCellInd_Null          ( void );
u8    mmp_ApiAttachReq_Null       ( void );
u8    mmp_SimOpenInd_Null         ( void );
u8    mmp_StartStack_Null         ( void ); 
u8    mmp_PReleaseInd_NULL(void);//

//-------------------
// Deregistered state
//-------------------
void  mmp_InitDreg                ( void );
u8    mmp_RrCellInd_Dreg          ( void );
u8    mmp_ApiAttachReq_Dreg       ( void );
u8    mmp_T3311Exp_Dreg           ( void );
u8    mmp_T3302Exp_Dreg           ( void );
u8    mmp_mmcReStartPrevProc_Dreg ( void );

//----------------------------
// Procedure In Progress state
//----------------------------
void  mmp_InitPIP                 ( void );
u8    mmp_AttachAcc_PIP           ( void );
u8    mmp_AttachRej_PIP           ( void );
u8    mmp_IdReq_PIP               ( void );
u8    mmp_T3310Exp_PIP            ( void );
u8    mmp_T3330Exp_PIP            ( void );
u8    mmp_ApiDetachReq_PIP        ( void );
u8    mmp_PReleaseInd_PIP         ( void );
u8    mmp_RrCellInd_PIP           ( void );
u8    mmp_DetachReq_PIP           ( void );
u8    mmp_AuthAndCiphReq_PIP      ( void );
u8    mmp_AuthAndCiphRej_PIP      ( void );
u8    mmp_HdleOtherPd_PIP         ( void );
u8    mmp_L3DataReq_PIP           ( void );
u8    mmp_RauAcc_PIP              ( void );
u8    mmp_RauRej_PIP              ( void );
u8    mmp_TmsiReallocCmd_PIP      ( void );
u8    mmp_RlcTReadyInd_PIP        ( void );
u8    mmp_T3312Exp_PIP            ( void );
u8    mmp_T3321Exp_PIP            ( void );
u8    mmp_DetachAcc_PIP           ( void );
u8    mmp_RlcSynchInd_PIP         ( void );
u8    mmp_PagingInd_PIP           ( void );
u8    mmp_Information_PIP         ( void );
u8    mmp_mmcAbortCurProc_PIP     ( void );
u8    mmp_RrSuspendReq_PIP        ( void );
u8    mmp_ApiAttachReq_PIP        ( void );
u8    mmp_SmNoNetrspInd_PIP (void); // DMX add 20140625, PDP active req no respond

//-----------------
// Registered State
//-----------------
void  mmp_InitReg                 ( void );
u8    mmp_RrCellInd_Reg           ( void );
u8    mmp_HdleOtherPd_Reg         ( void );
u8    mmp_IdReq_Reg               ( void );
u8    mmp_ApiDetachReq_REG        ( void );
u8    mmp_T3311Exp_Reg            ( void );
u8    mmp_T3302Exp_Reg            ( void );
u8    mmp_RAUAcc_REG (void);// 20140902, DMX, ADD
u8    mmp_AttachAcc_REG (void); // 20140902, DMX, ADD
u8    mmp_AuthAndCiphReq_Reg      ( void );
u8    mmp_AuthAndCiphRej_Reg      ( void );
u8    mmp_L3DataReq_Reg           ( void );
u8    mmp_RlcTReadyInd_Reg        ( void );
u8    mmp_T3312Exp_Reg            ( void );
u8    mmp_TmsiReallocCmd_Reg      ( void );
u8    mmp_RlcSynchInd_Reg         ( void );
u8    mmp_DetachReq_Reg           ( void );
u8    mmp_PagingInd_Reg           ( void );
u8    mmp_Information_Reg         ( void );
u8    mmp_mmcAbortCurProc_Reg     ( void );
u8    mmp_mmcReStartPrevProc_Reg  ( void );
/*added by liujianguo 07.05.11*/
u8    mmp_PReleaseInd_Reg (void);
u8    mmp_ApiAttachReq_REG(void);
/*ended by liujianguo*/
u8    mmp_SmNoNetrspInd_REG(void);  // DMX add 20140625, PDP active req no respond,
//---------------------
// Network Detach State
//---------------------
void  mmp_InitNdet                ( void );
u8    mmp_RrCellInd_Ndet          ( void );
u8    mmp_ApiAttachReq_Ndet       ( void );
u8    mmp_RlcSynchInd_Ndet        ( void );
u8    mmp_mmcAbortCurProc_Ndet    ( void );
u8    mmp_ApiDetachReq_Ndet       ( void );
u8    mmp_RrSuspendReq_Ndet       ( void );

//-------------------
// GPRS pending State
//-------------------
u8    mmp_RrCellInd_PEND          ( void );
u8    mmp_SaveEvt_PEND            ( void );
u8    mmp_SaveMsg_PEND            ( void );
u8    mmp_mmcAbortCurProc_PEND    ( void );
u8    mmp_RlcTReadyInd_PEND       ( void );
u8    mmp_RlcSynchInd_PEND        ( void );

#endif // __MMP_FSM_H__
