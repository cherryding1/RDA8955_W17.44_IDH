////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2006, Coolsand Technologies, Inc.
//                       All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: cc_fsm.c
// 
// DESCRIPTION:
//   Finite State Machine defintions for CC
// 
// REVISION HISTORY:
//   NAME            DATE            REMAKS
// DMX  | 20130614 | Add quick hangup before call actived 
// 20090731 | DMX | Move DTMF handle from U10 state to Gene state
////////////////////////////////////////////////////////////////////////////////

#ifndef __CC_FSM_H__
#define __CC_FSM_H__

#include "fsm.h"

//===================
//  CC definitions
//===================

extern Fsm_t  cc_Fsm ;

//
// Circuit Switch CC States definitions
// FSM cc
#define CC_GENE  0
#define CC_U0    1
#define CC_U01   2
#define CC_U1    3
#define CC_U6    4
#define CC_U8    5
#define CC_U9    6
#define CC_U10   7
#define CC_U11   8
#define CC_U12   9
#define CC_U19   10
#define CC_REEST 11

// 
// Transitions prototypes
u8 cc_InconState( void );
u8 cc_DoNothing ( void );

//--------------
// Generic State
//--------------
u8   cc_apiDiscReq_Gene    ( void );
u8   cc_Disconnect_Gene    ( void );
u8   cc_Release_Gene       ( void );
u8   cc_ReleaseCmpl_Gene   ( void );
u8   cc_StatusEnquiry_Gene ( void );
u8   cc_rrChanMode_Gene    ( void );
u8   cc_mmReleaseInd_Gene  ( void );
u8   cc_mmEstabErr_Gene    ( void );
u8   cc_mmReestabAsk_Gene  ( void );
u8   cc_Progress_Gene      ( void );
u8   cc_Facility_Gene      ( void );
u8   cc_UserInfo_Gene      ( void );
u8   cc_apiUserInfoReq_Gene( void );
u8   cc_TrsnApiRelCmpt     ( void );
u8   cc_Status_Gene        ( void );

//  [[[+][dingmx][20090731] Move DTMF handle from U10 state to Gene state
// Refer TS 24008 chpter5.5.7
 u8   cc_apiStartDtmfReq_Gene( void );
 u8   cc_apiStopDtmfReq_Gene    ( void );
 u8   cc_AckProcDtmf_Gene       ( void );
 u8   cc_EndProcDtmf_Gene       ( void );
//  ]][+][dingmx][20090731]

//---------
// U0 State
//---------
void cc_InitU0          ( void );
u8   cc_apiSetupReq_U0  ( void );
u8   cc_Setup_U0        ( void );
u8   cc_mmReleaseInd_U0 ( void );
u8   cc_rrChanMode_U0   ( void );

//----------
// U01 State
//----------
u8   cc_mmServReq_U01  ( void );
u8   cc_mmEstabCnf_U01 ( void );
u8   cc_T303Exp_U01    ( void );
u8   cc_apiDiscReq_U01 ( void );
//[[ __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX
u8   cc_mmEstabErr_U01(void );
u8   cc_TRetryExp_U01  ( void );
u8   cc_TWaitLuExp_U01(void);
u8   cc_mmLocUp_U01(void);
u8   cc_mmPageBegin_U01(void);
//]]__CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX

//---------
// U1 State
//---------
u8   cc_CallProceeding_U1 ( void );
u8   cc_Alerting_U1       ( void );
u8   cc_Connect_U1        ( void );
// [[ [+][DMX] [20130614]
// quick hand up call before the call actived
u8   cc_apiDiscReq_U1(void);
// ]] [+][DMX] [20130614]
//[[ __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX
u8   cc_mmReleaseInd_U1   ( void );
u8   cc_TRetryExp_U1      ( void );
u8   cc_mmPageBegin_U1(void);
//]]__CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX
//---------
// U6 State
//---------
u8   cc_apiCallConfReq_U6 ( void );

//---------
// U8 State
//---------
u8   cc_ConnectAck_U8 ( void );

//---------
// U9 State
//---------
u8   cc_apiAlertReq_U9   ( void );
u8   cc_apiConnectReq_U9 ( void );

//----------
// U10 State
//----------
void cc_InitU10               ( void );

u8   cc_mmReleaseInd_U10      ( void );
u8   cc_apiCallHoldReq_U10    ( void );
u8   cc_HoldAck_U10           ( void );
u8   cc_HoldRej_U10           ( void );
u8   cc_apiCallRtrvReq_U10    ( void );
u8   cc_RtrvAck_U10           ( void );
u8   cc_RtrvRej_U10           ( void );

//  [[[-][dingmx][20090731] Move DTMF handle from U10 state to Gene state
// Refer TS 24008 chpter5.5.7
// u8   cc_apiStartDtmfReq_U10   ( void );
// u8   cc_apiStopDtmfReq_U10    ( void );
// u8   cc_AckProcDtmf_U10       ( void );
// u8   cc_EndProcDtmf_U10       ( void );
//  ]][-][dingmx][20090731]


u8   cc_apiFacilityReq_U10    ( void );
u8   cc_TMPTYExp_U10          ( void );
u8   cc_TECTExp_U10           ( void );
u8   cc_apiNotifyReq_U10      ( void );
u8   cc_Notify_U10            ( void );
u8   cc_apiModifyReq_U10      ( void );
u8   cc_ModifyCmplt_U10       ( void );
u8   cc_ModifyRej_U10         ( void );
u8   cc_Modify_U10            ( void );
u8   cc_apiModifyCmpltReq_U10 ( void );
u8   cc_apiModifyRejReq_U10   ( void );
u8   cc_CongCtrl_U10          ( void );

//----------
// U11 State
//----------
u8   cc_T305Exp_U11    ( void );

//----------
// U12 State
//----------
u8   cc_apiRelReq_U12 ( void );

//----------
// U19 State
//----------
u8   cc_Release_U19 ( void );
u8   cc_T308Exp_U19 ( void );

//------------
// REEST State
//------------
u8   cc_apiDiscReq_REEST   ( void );
u8   cc_mmReestabCnf_REEST ( void );
u8   cc_mmReestabErr_REEST ( void );
u8   cc_TS10Exp_REEST      ( void );

#endif // __CC_FSM_H__
