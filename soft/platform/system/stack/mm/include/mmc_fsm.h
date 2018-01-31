//------------------------------------------------------------------------------
//  $Log: mmc_fsm.h,v $
//  Revision 1.3  2006/05/23 10:51:30  fch
//  Update Copyright
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
  File       : mm_fsm.h  
--------------------------------------------------------------------------------

  Scope      : Finite State Machine defintions for MM

  History    :
--------------------------------------------------------------------------------
  Aug 06 03  |  OTH   | Creation            
================================================================================
*/

#ifndef __MMC_FSM_H__
#define __MMC_FSM_H__


#if !(STK_MAX_SIM==1)
extern Fsm_t  mmc_Fsm[STK_MAX_SIM];
#else
extern Fsm_t  mmc_Fsm ;
#endif

/*
===================
  MMC definitions
===================
*/

/*
** Circuit Switch MM States definitions
** FSM mmc
*/
#define MMC_GENE   0
#define MMC_NULL   1
#define MMC_IDLE ( 2 | FSM_STABLE )
#define MMC_WRRC   3
#define MMC_WNKR   4
#define MMC_CNTD ( 5 | FSM_STABLE )
#define MMC_WNKC   6

#if !(STK_MAX_SIM==1)
#define MMC_SAVE_EVT( _Evt )          fsm_Save( &mmc_Fsm[pMMCtx->SimId], FALSE, _Evt )
#define MMC_SAVE_MSG_ERASE( _Msg )    fsm_Save( &mmc_Fsm[pMMCtx->SimId], TRUE,  _Msg )
#define MMC_SAVE_MSG_KEEP( _Msg )            \
         fsm_Save( &mmc_Fsm[pMMCtx->SimId], TRUE, _Msg );   \
         pMMCtx->Flags |= MM_KEEP_MSG
#else
#define MMC_SAVE_EVT( _Evt )          fsm_Save( &mmc_Fsm, FALSE, _Evt )
#define MMC_SAVE_MSG_ERASE( _Msg )    fsm_Save( &mmc_Fsm, TRUE,  _Msg )
#define MMC_SAVE_MSG_KEEP( _Msg )            \
         fsm_Save( &mmc_Fsm, TRUE, _Msg );   \
         pMMCtx->Flags |= MM_KEEP_MSG
#endif

/* 
** Transitions prototypes
*/
//--------------
// Generic State
//--------------
u8   mmc_DataReq_Gene     ( void );
u8   mmc_IdReq_Gene       ( void );
u8   mmc_EstabReq_Gene    ( void );
u8   mmc_TmsiRealCmd_Gene ( void );
u8   mmc_AuthReq_Gene     ( void );
u8   mmc_AuthRej_Gene     ( void );
u8   mmc_Abort_Gene       ( void );
u8   mmc_SimOpenInd_Gene  ( void );
u8   mmc_SimClosedInd_Gene( void );
u8   mmc_Information_Gene ( void );
u8   mmc_Release_Gene (void);

//-----------
// Null State
//-----------
void mmc_InitNull         ( void );
u8   mmc_RrCellInd_Null   ( void );
u8   mmc_EstabReq_Null    ( void );
u8   mmc_SimOpenInd_Null  ( void );
u8   mmc_SimClosedInd_Null( void );
u8   mmc_ApiStartReq_Null ( void );
u8   mmc_ApiStopReq_Null  ( void );
u8   mmc_T3212Exp_Null    ( void );

//-----------
// Idle State
//-----------
void mmc_InitIdle         ( void );
u8   mmc_RrCellInd_Idle   ( void );
u8   mmc_RrEndCong_Idle   ( void );
u8   mmc_PagingInd_Idle   ( void );
u8   mmc_SimOpenInd_Idle  ( void );
u8   mmc_SimClosedInd_Idle( void );
u8   mmc_ApiStopReq_Idle  ( void );
u8   mmc_EstabReq_Idle    ( void );
u8   mmc_ReestabReq_Idle  ( void );
u8   mmc_T3211Exp_Idle    ( void );
u8   mmc_T3212Exp_Idle    ( void );
u8   mmc_T3213Exp_Idle    ( void );
u8   mmc_SelfLocup_Idle   ( void );

//-----------------------------
// Wait for RR connection State
//-----------------------------
void mmc_InitWRrCx        ( void );
u8   mmc_RrConnectCnf_WRrC( void );
u8   mmc_PagingInd_WRrC   ( void );
u8   mmc_RrReleaseInd_WRrC( void );
u8   mmc_SimClosedInd_WRrC( void );
u8   mmc_ApiStopReq_WRrC  ( void );
u8   mmc_RrCellInd_WRrC   ( void );
u8   mmc_AbortReq_WRrC    ( void );
u8   mmc_ReleaseReq_WRrC  ( void );
u8   mmc_T3212Exp_WRrC    ( void );
u8   mmc_TDetachExp_WRrC  ( void );

//--------------------------------
// Wait for Network Response State
//--------------------------------
void mmc_InitWNkRsp         ( void );
u8   mmc_ReleaseInd_WNkRsp  ( void );
u8   mmc_CiphInd_WNkRsp     ( void );
u8   mmc_AbortReq_WNkRsp    ( void );
u8   mmc_EstabReq_WNkRsp    ( void );
u8   mmc_ReleaseReq_WNkRsp  ( void );
u8   mmc_ApiStopReq_WNkRsp  ( void );
u8   mmc_LocUpAcc_WNkRsp    ( void );
u8   mmc_LocUpRej_WNkRsp    ( void );
u8   mmc_CmServAcc_WNkRsp   ( void );
u8   mmc_CmServRej_WNkRsp   ( void );
u8   mmc_HdleOtherPd_WNkRsp ( void );
/*added by liujianguo*/
u8 mmc_CmEstabInd_WNkRsp ( void );
/*end of added by liujianguo*/
u8   mmc_T3210Exp_WNkRsp    ( void );
u8   mmc_T3212Exp_WNkRsp    ( void );
u8   mmc_T3230Exp_WNkRsp    ( void );

//----------------
// Connected State
//----------------
void mmc_InitCtnd ( void );
u8   mmc_EstabReq_Cntd      ( void );
u8   mmc_ReleaseReq_Cntd    ( void );
u8   mmc_RrReleaseInd_Cntd  ( void );
u8   mmc_ApiStopReq_Cntd    ( void );
u8   mmc_CmEstabInd_Cntd    ( void );
u8   mmc_AbortReq_Cntd      ( void );
u8   mmc_HdleOtherPd_Cntd   ( void );

//-------------------------------
// Wait for Network Command State
//-------------------------------
void mmc_InitWNkCmd         ( void );
u8   mmc_RrReleaseInd_WNkCmd( void );
u8   mmc_CmEstabInd_WNkCmd  ( void );
u8   mmc_SimClosedInd_WNkCmd( void );
u8   mmc_ApiStopReq_WNkCmd  ( void );
u8   mmc_TWNKCExp_WNkCmd    ( void );
u8   mmc_T3212Exp_WNkCmd    ( void );
u8   mmc_HdleOtherPd_WNkCmd ( void );
u8   mmc_AbortReq_WNkCmd (void); //added by zhanghy. handle MM_ABORT_REQ in WNkCmd state. 20110316

#endif // __MMC_FSM_H__
