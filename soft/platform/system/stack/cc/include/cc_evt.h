// //////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2002-2006, Coolsand Technologies, Inc.
// All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
// 
// FILENAME: cc_evt.c
// 
// DESCRIPTION:
// Declaration of all Events od mm Task
// 
// REVISION HISTORY:
// NAME            DATE            REMAKS
// DMX | 2006-12-28 | Add two interface about Reeset
// //////////////////////////////////////////////////////////////////////////////

#ifndef __CC_EVT_H__
#define __CC_EVT_H__

#include "fsm.h"  // EvtDsc_t Type definition
#include "itf_rr.h"
#include "itf_sim.h"

// ==========================================
// Incoming Events
// ==========================================

// 
// Flags for the incoming events
// 
#define CC_MM_EVT  ( 1 << 0 )
#define CC_ALL     ( 1 << 1 ) // Evt to be broadcasted to all events

//
// Interface with RR
//
extern InEvtDsc_t cc_rrIChanModeInd;

//
//  Internal Interface with MM
//
extern InEvtDsc_t cc_mmIReleaseInd;
extern InEvtDsc_t cc_mmIEstabCnf;
extern InEvtDsc_t cc_mmIEstabErr;
extern InEvtDsc_t cc_mmIReestabCnf;
extern InEvtDsc_t cc_mmIReestabErr;
extern InEvtDsc_t cc_mmIServReq;
extern InEvtDsc_t cc_mmIDataInd;
//[[ __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX
extern InEvtDsc_t cc_mmILocUpStaInd;
extern InEvtDsc_t cc_mmIPageBeginInd;
//]] __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX

//
//  Internal Interface with API
//
extern InEvtDsc_t cc_apiISetupReq;
extern InEvtDsc_t cc_apiICallConfReq;
extern InEvtDsc_t cc_apiIAlertReq;
extern InEvtDsc_t cc_apiIConnectReq;
extern InEvtDsc_t cc_apiIDiscReq;
extern InEvtDsc_t cc_apiIRelReq;
extern InEvtDsc_t cc_apiIRelCmptReq;
extern InEvtDsc_t cc_apiIStartDtmfReq;
extern InEvtDsc_t cc_apiIStopDtmfReq;
extern InEvtDsc_t cc_apiICallHoldReq;
extern InEvtDsc_t cc_apiICallRtrvReq;
extern InEvtDsc_t cc_apiIFacilityReq;
extern InEvtDsc_t cc_apiIModifyReq;
extern InEvtDsc_t cc_apiIModifyCmpltReq;
extern InEvtDsc_t cc_apiIModifyRejReq;
extern InEvtDsc_t cc_apiINotifyReq;
extern InEvtDsc_t cc_apiIUserInfoReq;

/*
**  Timers expiration
*/
extern InEvtDsc_t T303Exp;
extern InEvtDsc_t T305Exp;
extern InEvtDsc_t T308Exp;
extern InEvtDsc_t T310Exp;
extern InEvtDsc_t T313Exp;
extern InEvtDsc_t T323Exp;
extern InEvtDsc_t T336Exp;
extern InEvtDsc_t T337Exp;
extern InEvtDsc_t TMPTYExp;
extern InEvtDsc_t TECTExp;
extern InEvtDsc_t TS10Exp;
//[[ __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX
extern InEvtDsc_t TWAITLUExp ;
extern InEvtDsc_t TRETRYExp;
//]] __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX

/*
** Derived Events
*/
extern InEvtDsc_t cc_Setup;
extern InEvtDsc_t cc_CallProceeding;
extern InEvtDsc_t cc_Progress;
extern InEvtDsc_t cc_Alert;
extern InEvtDsc_t cc_Connect;
extern InEvtDsc_t cc_ConnectAck;
extern InEvtDsc_t cc_Disconnect;
extern InEvtDsc_t cc_Release;
extern InEvtDsc_t cc_ReleaseCmpl;
extern InEvtDsc_t cc_StatusEnquiry;
extern InEvtDsc_t cc_HoldAck;
extern InEvtDsc_t cc_HoldRej;
extern InEvtDsc_t cc_RtrvAck;
extern InEvtDsc_t cc_RtrvRej;
extern InEvtDsc_t cc_StartDtmfAck;
extern InEvtDsc_t cc_StartDtmfRej;
extern InEvtDsc_t cc_StopDtmfAck;
extern InEvtDsc_t cc_Facility;
extern InEvtDsc_t cc_Modify;
extern InEvtDsc_t cc_ModifyCmplt;
extern InEvtDsc_t cc_ModifyRej;
extern InEvtDsc_t cc_Notify;
extern InEvtDsc_t cc_UserInfo;
extern InEvtDsc_t cc_CongCtrl;
extern InEvtDsc_t cc_Status;

InEvtDsc_t* cc_GiveDsc(u32 MsgId);

// Routage function for the DATA_IND
InEvtDsc_t* cc_GiveDataIndDsc(void);

// ==========================================
//             Outgoing Events
// ==========================================
// Flags for the outgoing events
#define CC_IL_MSG ( 1 << 0 )

extern OutEvtDsc_t cc_simOReadBinReq;
extern OutEvtDsc_t cc_simOElemFileStatusReq;


// 
// Interface with MM
// 
extern OutEvtDsc_t cc_mmOEstabReq;
extern OutEvtDsc_t cc_mmOEstabInd;
extern OutEvtDsc_t cc_mmOReleaseReq;
extern OutEvtDsc_t cc_mmOAbortReq;
extern OutEvtDsc_t cc_mmOReestabReq;
extern OutEvtDsc_t cc_mmODataReq   ;

//
// Interface with API
//
extern OutEvtDsc_t cc_apiOTIInd;
extern OutEvtDsc_t cc_apiOSetupInd;
extern OutEvtDsc_t cc_apiOCallProcInd;
extern OutEvtDsc_t cc_apiOProgInd;
extern OutEvtDsc_t cc_apiOAlertInd;
extern OutEvtDsc_t cc_apiOConnectInd;
extern OutEvtDsc_t cc_apiOConnectAckInd;
extern OutEvtDsc_t cc_apiODiscInd;
extern OutEvtDsc_t cc_apiORelInd;
extern OutEvtDsc_t cc_apiORelCompInd;
extern OutEvtDsc_t cc_apiOErrorInd;
extern OutEvtDsc_t cc_apiOAudioOnInd;
extern OutEvtDsc_t cc_apiOCallHoldCnf;
extern OutEvtDsc_t cc_apiOCallRtrvCnf;
extern OutEvtDsc_t cc_apiOFacilityInd;
extern OutEvtDsc_t cc_apiOModifyInd;
extern OutEvtDsc_t cc_apiOModifyCmpltInd;
extern OutEvtDsc_t cc_apiOModifyRejInd;
extern OutEvtDsc_t cc_apiONotifyInd;
extern OutEvtDsc_t cc_apiOUserInfoInd;
extern OutEvtDsc_t cc_apiOCongCtrlInd;

extern OutEvtDsc_t cc_apiOCCReestBeginInd;
extern OutEvtDsc_t cc_apiOCCReestCnfInd   ;

// 为方便单双卡模式统一代码增加的接口
extern OutEvtDsc_t cc_simOReadBinReq;
extern OutEvtDsc_t cc_simOReadFileStatusReq ;

#endif
