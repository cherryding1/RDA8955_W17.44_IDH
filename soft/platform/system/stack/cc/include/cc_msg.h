////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2006, Coolsand Technologies, Inc.
//                       All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: cc_msg.h
// 
// DESCRIPTION:
//   Defintion of Msg body for CC Task
// 
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   TODO... 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CC_MSG_H__
#define __CC_MSG_H__

#include "itf_rr.h"
#include "itf_mm.h"
#include "itf_cc.h"
#include "itf_sim.h"

typedef union
{
 // Interlayer header with MM
 u8 toto;
} IlHdr_t;

#include "itf_il.h"

typedef union 
{
 IlBody_t IL;

// Interface with RR
 rr_ChanModeInd_t rrChanMode;

 sim_ReadBinaryReq_t     simReadBinaryReq;
 sim_ElemFileStatusReq_t simElemFileStatusReq;
 sim_ReadBinaryCnf_t     simReadBinaryCnf;
 sim_ElemFileStatusCnf_t simElemFileStatusCnf;

//
// Interface with MM
 mm_EstabReq_t    mmEstabReq;
 mm_ReestabReq_t  mmReestabReq;
 mm_ReleaseInd_t  mmRelInd;
 mm_PDTIInd_t     mmPDTIInd; //used for the ServReqAck and EstabInd

//[[ __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX
 mm_EstabErrInd_t mmEstabErrInd;
 rr_LocUpSta_t       rrLocUpSta;
//]]__CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX

// Interface with API
 cc_SetupReq_t        ccSetupReq        ;
 cc_CallConfReq_t     ccCallConfReq     ;
 cc_AlertReq_t        ccAlertReq        ;
 cc_ConnectReq_t      ccConnectReq      ;
 cc_DiscReq_t         ccDiscReq         ;
 cc_ReleaseReq_t      ccReleaseReq      ;   
 cc_ReleaseCmpltReq_t ccReleaseCmpltReq ;
 cc_CallHoldReq_t     ccCallHoldReq     ;
 cc_CallRetrieveReq_t ccCallRtrvReq     ;
 cc_ModifyReq_t       ccModifyReq       ;
 cc_ModifyCmpltReq_t  ccModifyCmpltReq  ;
 cc_ModifyRejReq_t    ccModifyRejReq    ;
 cc_NotifyReq_t       ccNotifyReq       ;
 cc_StartDtmfReq_t    ccStartDtmfReq    ;
 cc_StopDtmfReq_t     ccStopDtmfReq     ;
 cc_FacilityReq_t     ccFacilityReq     ;
 cc_UserInfoReq_t     ccUserInfoReq     ;

 cc_TIInd_t           ccTIInd           ;
 cc_SetupInd_t        ccSetupInd        ;
 cc_CallProcInd_t     ccCallProcInd     ;
 cc_ProgressInd_t     ccProgressInd     ;
 cc_AlertInd_t        ccAlertInd        ;
 cc_ConnectInd_t      ccConnectInd      ;
 cc_ConnectAckInd_t   ccConnectAckInd   ;
 cc_DiscInd_t         ccDiscInd         ;
 cc_ReleaseInd_t      ccReleaseInd      ;   
 cc_ReleaseCmpltInd_t ccReleaseCmpltInd ;
 cc_ErrorInd_t        ccErrorInd        ;
 cc_CallHoldCnf_t     ccCallHoldCnf     ;
 cc_CallRetrieveCnf_t ccCallRtrvCnf     ;
 cc_CallWaitingInd_t  ccCallWaitingInd  ;
 cc_ModifyInd_t       ccModifyInd       ;
 cc_ModifyCmpltInd_t  ccModifyCmpltInd  ;
 cc_ModifyRejInd_t    ccModifyRejInd    ;
 cc_NotifyInd_t       ccNotifyInd       ;
 cc_FacilityInd_t     ccFacilityInd     ;
 cc_UserInfoInd_t     ccUserInfoInd     ;
 cc_CongCtrlInd_t     ccCongCtrlInd     ;
} MsgBody_t;

#include "itf_msg.h"

#endif
