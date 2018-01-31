//------------------------------------------------------------------------------
//  $Log: sm_fsm.h,v $
//  Revision 1.3  2006/05/22 19:35:41  aba
//  Update of the copyright
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
  File       : sm_fsm.h
--------------------------------------------------------------------------------

  Scope      : Finite State Machine defintions for sm

  History    :
--------------------------------------------------------------------------------
  Jun 28 04  |  PCA  | Creation
================================================================================
*/
#ifndef __SM_FSM_H__
#define __SM_FSM_H__

#include "fsm.h"
#include "sm_data.h"


/*
** Save message and save event macros 
*/
#if !(STK_MAX_SIM==1)
#define SM_SAVE_MSG(_SimId, _Msg)     fsm_Save ( &sm_Fsm[_SimId][psm_Data->NSapiIdx], TRUE, _Msg)
#define SM_SAVE_EVT(_SimId, _Evt)     fsm_Save ( &sm_Fsm[_SimId][psm_Data->NSapiIdx], FALSE, _Evt)
#else
#define SM_SAVE_MSG(_Msg)     fsm_Save ( &sm_Fsm[psm_Data->NSapiIdx], TRUE, _Msg)
#define SM_SAVE_EVT(_Evt)     fsm_Save ( &sm_Fsm[psm_Data->NSapiIdx], FALSE, _Evt)
#endif

u8 sm_SaveMsg (void);


/*
=======================
  SM state definitions
=======================
*/
#define SM_GENE                         0
#define SM_PDP_INACTIVE                (1+FSM_STABLE)
#define SM_WAIT_MMP_CTX_ACTIVATE_ACC   (2+FSM_STABLE)
#define SM_WAIT_SM_ACTIVATE_RSP         3
#define SM_WAIT_MMP_CTX_MODIFY_ACC     (4+FSM_STABLE)
#define SM_WAIT_SM_MODIFY_RSP           5
#define SM_PDP_ACTIVE                  (6+FSM_STABLE)
#define SM_WAIT_MMP_CTX_DEACTIVATE_ACC (7+FSM_STABLE)
#define SM_WAIT_SM_DEACTIVATE_RSP       8
#define SM_WAIT_API_MODIFY_ACC         (9+FSM_STABLE)
/*
** Transitions prototypes
*/
extern u8 sm_DoNothing                                    ( void );

extern u8 sm_Allstate_SmStatusInd                         ( void );
extern u8 sm_Allstate_MmpAbortInd                         ( void );
extern u8 sm_Allstate_SmStatusReq                         ( void );
extern u8 sm_Allstate_MmpRauAccPdpStateInd                ( void ); // DMX add 20161120, __RAUACC_PDP_STATUS_FTR__

extern u8 sm_PdpInactive_ApiCtxActivateReq                ( void );
extern u8 sm_PdpInactive_ApiCtxDeactivateReq              (void);        // send a cnf to API anyhow 20160617
extern u8 sm_PdpInactive_MmpReqCtxActivate                ( void );

extern u8 sm_WaitApiCtxActivateReq_ApiCtxActivateReq      ( void );
extern u8 sm_WaitApiCtxActivateReq_ApiCtxActivateRej      ( void );

extern u8 sm_WaitMmpCtxActivateAcc_MmpCtxActivateAcc      ( void );
extern u8 sm_WaitMmpCtxActivateAcc_T3380                  ( void );
extern u8 sm_WaitMmpCtxActivateAcc_MmpCtxActivateRej      ( void );
extern u8 sm_WaitMmpCtxActivateAcc_ApiCtxDeactivateReq    ( void );
extern u8 sm_WaitMmpCtxActivateAcc_MmpCtxDeactivateReq    ( void );

extern u8 sm_WaitSmActivateRsp_SmActivateRsp              ( void );

extern u8 sm_WaitMmpCtxModifyAcc_MmpCtxModifyAcc          ( void );
extern u8 sm_WaitMmpCtxModifyAcc_MmpCtxModifyRej          ( void );
extern u8 sm_WaitMmpCtxModifyAcc_T3381                    ( void );
extern u8 sm_WaitMmpCtxModifyAcc_ApiCtxDeactivateReq      ( void );
extern u8 sm_WaitMmpCtxModifyAcc_MmpCtxDeactivateReq      ( void );
extern u8 sm_WaitMmpCtxModifyAcc_MmpCtxModifyReq          ( void );

extern u8 sm_WaitSmModifyRsp_SmModifyRsp                  ( void );

extern u8 sm_PdpActive_ApiCtxModifyReq                    ( void );
extern u8 sm_PdpActive_MmpCtxModifyReq                    ( void );
extern u8 sm_PdpActive_ApiCtxDeactivateReq                ( void );
extern u8 sm_PdpActive_MmpCtxDeactivateReq                ( void );
extern u8 sm_PdpActive_ApiCtxActivateReq                  ( void );

extern u8 sm_WaitMmpCtxDeactivateAcc_MmpCtxDeactivateAcc  ( void );
extern u8 sm_WaitMmpCtxDeactivateAcc_T3390                ( void );
extern u8 sm_WaitMmpCtxDeactivateAcc_MmpCtxDeactivateReq  ( void );

extern u8 sm_WaitSmDeactivateRsp_SmDeactivateRsp          ( void );

extern u8 sm_WaitApiCtxModifyAcc_ApiCtxModifyAcc          ( void );
extern u8 sm_WaitApiCtxModifyAcc_ApiCtxModifyRej          ( void );
extern u8 sm_WaitApiCtxModifyAcc_ApiCtxDeactivateReqEvt   ( void );
extern u8 sm_WaitApiCtxModifyAcc_MmpCtxDeactivateReqEvt   ( void );

extern u8 mmpDataInd                                      ( void *p);

/*
** FSM definition
*/
#if !(STK_MAX_SIM==1)
extern Fsm_t sm_Fsm[STK_MAX_SIM][NB_MAX_NSAPI];
#else
extern Fsm_t sm_Fsm[NB_MAX_NSAPI];
#endif

#endif
