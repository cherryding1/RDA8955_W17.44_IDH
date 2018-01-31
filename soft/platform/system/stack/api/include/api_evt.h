
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2003

================================================================================
*/

/*
================================================================================
  File       : api_evt.h  
--------------------------------------------------------------------------------

  Scope      : Declaration of all Events of api Task               

  History    :
--------------------------------------------------------------------------------
  Jun 09 06  |  ABA   | Abort MMA (sms)
--------------------------------------------------------------------------------
  Mar 31 06  |  MCE   | Added rlu_UlCongInd  
--------------------------------------------------------------------------------
  Jan 11 06  |  MCE   | Added rr_ChanModeInd and api_CcChanModeInd    
--------------------------------------------------------------------------------
  Nov 16 05  |  ABA   | Aoc processing
--------------------------------------------------------------------------------
  Nov 15 05  |  MCE   | Added api_DbgInfoCmd event
--------------------------------------------------------------------------------
  Jun 06 05  |  ABA   | ISSUE 19: poll interval for simtoolkit
--------------------------------------------------------------------------------
  Mar 10 04  |  PCA   | Creation
================================================================================
*/
#ifndef __API_EVT_H__
#define __API_EVT_H__

#include "fsm.h"         // EvtDsc_t Type definition
#include "itf_rr.h"
#include "itf_sim.h"
#include "itf_cc.h"
#include "itf_api.h"
#include "itf_sms.h"

/*
==========================================
             Incoming Events from MMI
==========================================
*/
/*
** General : MMI->API
*/
extern InEvtDsc_t api_StartReq    ;
extern InEvtDsc_t api_StopReq     ;
extern InEvtDsc_t api_ImeiReq     ;
extern InEvtDsc_t api_SimOpenInd  ;
extern InEvtDsc_t api_SimCloseInd ;
extern InEvtDsc_t api_DbgInfoCmd  ;

/*
** CC : MMI->API
*/
extern InEvtDsc_t api_CcSetupReq        ;
extern InEvtDsc_t api_CcCallConfReq     ;
extern InEvtDsc_t api_CcAlertReq        ;
extern InEvtDsc_t api_CcConnectReq      ;
extern InEvtDsc_t api_CcConnectAckReq   ;
extern InEvtDsc_t api_CcDiscReq         ;
extern InEvtDsc_t api_CcReleaseReq      ;
extern InEvtDsc_t api_CcReleaseCmpltReq ;
extern InEvtDsc_t api_CcStartDtmfReq    ;
extern InEvtDsc_t api_CcStopDtmfReq     ;
extern InEvtDsc_t api_CcCallHoldReq     ;
extern InEvtDsc_t api_CcCallRetrieveReq ;
extern InEvtDsc_t api_CcStartDtmfReq    ;
extern InEvtDsc_t api_CcStopDtmfReq     ;
extern InEvtDsc_t api_CcModifyReq       ;
extern InEvtDsc_t api_CcModifyCmpltReq  ;
extern InEvtDsc_t api_CcModifyRejReq    ;
extern InEvtDsc_t api_CcNotifyReq       ;
extern InEvtDsc_t api_CcFacilityReq     ;
extern InEvtDsc_t api_CcUserInfoReq     ;

/*
** RR: MMI->API
*/
extern InEvtDsc_t api_QualReportReq  ;
extern InEvtDsc_t api_NwSelModeReq   ;
extern InEvtDsc_t api_NwListReq      ;
extern InEvtDsc_t api_NwListAbortReq ;
extern InEvtDsc_t api_NwSelReq       ;
extern InEvtDsc_t api_NwReSelReq     ;
extern InEvtDsc_t api_CurCellInfoReq ;
extern InEvtDsc_t api_AdjCellInfoReq ;
extern InEvtDsc_t api_PowerScanReq;
extern InEvtDsc_t api_DetectMBSReq;

/*
** MM: MMI->API
*/
extern InEvtDsc_t api_NwPSAttachReq;
extern InEvtDsc_t api_NwCSDetachReq;
extern InEvtDsc_t api_NwPSDetachReq;

/*
** SMS : MMI->API
*/
// SMS-PP
extern InEvtDsc_t api_SmsPPSendReqEvt   ;
extern InEvtDsc_t api_SmsPPSendMMAReqEvt;
extern InEvtDsc_t api_SmsPPAckReqEvt    ;
extern InEvtDsc_t api_SmsPPErrorReqEvt  ;
extern InEvtDsc_t api_SmsPPSAbortMMAReqEvt;
// SMS-CB
extern InEvtDsc_t api_SmsCBActivateReqEvt  ;
extern InEvtDsc_t api_SmsCBDeActivateReqEvt;

extern InEvtDsc_t api_SmsPPAbortReqEvt;
/*
** SS: MMI -> API
*/
extern InEvtDsc_t api_SsActionReq;

/*
** PDP: MMI -> API
*/
extern InEvtDsc_t api_PdpCtxActivateReqEvt;
extern InEvtDsc_t api_PdpSetMinQoSReqEvt;
extern InEvtDsc_t api_PdpCtxModifyReqEvt;
extern InEvtDsc_t api_PdpCtxModifyAccEvt;
extern InEvtDsc_t api_PdpCtxDeactivateReqEvt;
extern InEvtDsc_t api_IPdpCtxActivateRejEvt;
extern InEvtDsc_t api_PsDataReq;

/*
** SIM: MMI -> API
*/
extern InEvtDsc_t api_SimResetReq;
extern InEvtDsc_t api_SimReadBinaryReq;
extern InEvtDsc_t api_SimUpdateBinaryReq;
extern InEvtDsc_t api_SimReadRecordReq;
extern InEvtDsc_t api_SimUpdateRecordReq;
extern InEvtDsc_t api_SimInvalidateReq;
extern InEvtDsc_t api_SimRehabilitateReq;
extern InEvtDsc_t api_SimSeekReq;
extern InEvtDsc_t api_SimIncreaseReq;
extern InEvtDsc_t api_SimVerifyChvReq;
extern InEvtDsc_t api_SimChangeChvReq;
extern InEvtDsc_t api_SimDisableChvReq;
extern InEvtDsc_t api_SimEnableChvReq;
extern InEvtDsc_t api_SimUnblockChvReq;
extern InEvtDsc_t api_SimElemFileStatusReq;
extern InEvtDsc_t api_SimDedicFileStatusReq;
extern InEvtDsc_t api_SimTerminalProfileReq;
extern InEvtDsc_t api_SimTerminalEnvelopeReq;
extern InEvtDsc_t api_SimTerminalResponseReq;
extern InEvtDsc_t api_SimStkActivateReq;
extern InEvtDsc_t api_SimPollReq;
extern InEvtDsc_t api_SimPollOffReq;

/*
** Circuit Switched Data: MMI -> API
*/
extern InEvtDsc_t api_ICsdCrlpSetReqEvt;
extern InEvtDsc_t api_ICsdCrlpRdReqEvt;

/*
==========================================
             Incoming Events from RR
==========================================
*/
extern InEvtDsc_t rr_NwListInd      ;
extern InEvtDsc_t rr_NwListAbortInd ;
extern InEvtDsc_t stk_StopCnf       ;
extern InEvtDsc_t rr_PowerScanRsp;

extern InEvtDsc_t stk_SimCloseCnf       ; //added by liujg 090806

extern InEvtDsc_t rr_CurCellInfoInd ;
extern InEvtDsc_t rr_AdjCellInfoInd ;
extern InEvtDsc_t rr_ChanModeInd    ;

/*
==========================================
             Incoming Events from MM
==========================================
*/
extern InEvtDsc_t mm_NwkInfoInd     ;

/*
==========================================
             Incoming Events from CC
==========================================
*/
extern InEvtDsc_t cc_TIInd            ;
extern InEvtDsc_t cc_SetupInd         ;
extern InEvtDsc_t cc_CallProcInd      ;
extern InEvtDsc_t cc_ProgInd          ;
extern InEvtDsc_t cc_AlertInd         ;
extern InEvtDsc_t cc_ConnectInd       ;
extern InEvtDsc_t cc_ConnectAckInd    ;
extern InEvtDsc_t cc_DiscInd          ;
extern InEvtDsc_t cc_RelInd           ;
extern InEvtDsc_t cc_RelCmptInd       ;
extern InEvtDsc_t cc_ErrorInd         ;
extern InEvtDsc_t cc_CallHoldCnf      ;
extern InEvtDsc_t cc_CallRtvCnf       ; 
extern InEvtDsc_t cc_CallWaitingInd   ;
extern InEvtDsc_t cc_ModifyInd        ;
extern InEvtDsc_t cc_ModifyCmpltInd   ;
extern InEvtDsc_t cc_ModifyRejInd     ;
extern InEvtDsc_t cc_NotifyInd        ;
extern InEvtDsc_t cc_AudioOnInd       ;
extern InEvtDsc_t cc_FacilityInd      ;
extern InEvtDsc_t cc_UserInfoInd      ;
extern InEvtDsc_t cc_CongCtrlInd      ;
#ifdef __AOC__
extern InEvtDsc_t cc_AocCCMInd        ;
#endif

/*
==========================================
             Incoming Events from SMS
==========================================
*/
extern InEvtDsc_t smspp_AckIndEvt    ;
extern InEvtDsc_t smspp_ErrorIndEvt  ;
extern InEvtDsc_t smspp_ReceiveIndEvt;
extern InEvtDsc_t cb_MsgRecIndEvt    ;

/*
==========================================
             Incoming Events from SS
==========================================
*/
extern InEvtDsc_t ss_ActionCnfEvt    ;
extern InEvtDsc_t ss_TIIndEvt;
extern InEvtDsc_t ss_ErrorIndEvt  ;
extern InEvtDsc_t ss_ActionIndEvt ;

/*
==========================================
             Incoming Events from PDP
==========================================
*/

/*
==========================================
             Incoming Events from SND
==========================================
*/
extern InEvtDsc_t snd_DataIndEvt;
extern InEvtDsc_t snd_QueueCreateIndEvt;

/*
==========================================
             Incoming Events from SM
==========================================
*/
extern InEvtDsc_t sm_PdpCtxActivateCnfEvt;
extern InEvtDsc_t sm_PdpCtxSmIndexIndEvt;
extern InEvtDsc_t sm_PdpCtxDeactivateCnfEvt;
extern InEvtDsc_t sm_PdpCtxActivateRejEvt;
extern InEvtDsc_t sm_PdpCtxModifyRejEvt;
extern InEvtDsc_t sm_PdpCtxActivateIndEvt;
extern InEvtDsc_t sm_PdpCtxModifyCnfEvt;
extern InEvtDsc_t sm_PdpCtxModifyIndEvt;
extern InEvtDsc_t sm_PdpCtxDeactivateIndEvt;

/*
==========================================
             Incoming Events from SIM
==========================================
*/
extern InEvtDsc_t sim_SimResetCnf;
extern InEvtDsc_t sim_SimStatusErrorInd;
extern InEvtDsc_t sim_SimReadBinaryCnf;
extern InEvtDsc_t sim_SimUpdateBinaryCnf;
extern InEvtDsc_t sim_SimReadRecordCnf;
extern InEvtDsc_t sim_SimUpdateRecordCnf;
extern InEvtDsc_t sim_SimInvalidateCnf;
extern InEvtDsc_t sim_SimRehabilitateCnf;
extern InEvtDsc_t sim_SimSeekCnf;
extern InEvtDsc_t sim_SimIncreaseCnf;
extern InEvtDsc_t sim_SimVerifyCHVCnf;
extern InEvtDsc_t sim_SimChangeCHVCnf;
extern InEvtDsc_t sim_SimDisableCHVCnf;
extern InEvtDsc_t sim_SimEnableCHVCnf;
extern InEvtDsc_t sim_SimUnblockCHVCnf;
extern InEvtDsc_t sim_SimElemFileStatusCnf;
extern InEvtDsc_t sim_SimDedicFileStatusCnf;
extern InEvtDsc_t sim_SimTerminalProfileCnf;
extern InEvtDsc_t sim_SimTerminalEnvelopeCnf;
extern InEvtDsc_t sim_SimTerminalResponseCnf;
extern InEvtDsc_t sim_SimFetchInd;
extern InEvtDsc_t sim_SimSelectApplicationCnf;
extern InEvtDsc_t sim_SimSeekAllCnf;

/*
==========================================
             Incoming Events from RLP
==========================================
*/
extern InEvtDsc_t rlp_CrlpRdInd;

/*
==========================================
             Incoming Events from RLU
==========================================
*/
extern InEvtDsc_t rlu_UlCongInd;
/*
==========================================
            Incoming Events from RABM
==========================================
*/
#ifdef LTE_NBIOT_SUPPORT
extern InEvtDsc_t rabmApiFlowCtrlInd;
#endif

/*
==============================================
             Incoming internal Events from API
==============================================
*/
extern InEvtDsc_t api_IMemAlertExp;

/*
==========================================
             Outgoing Events to API
==========================================
*/
/*
** General: API->MMI
*/
extern OutEvtDsc_t api_ImeiInd;
extern OutEvtDsc_t api_StopCnf;
extern OutEvtDsc_t api_SimCloseCnf;// API_SIM_CLOSE_CNF


extern OutEvtDsc_t api_CurCellInfoInd  ;
extern OutEvtDsc_t api_AdjCellInfoInd  ;
extern OutEvtDsc_t api_PowerListInd;
extern OutEvtDsc_t api_SyncInfoInd;

extern OutEvtDsc_t api_StartStackErrIndEvt;

/*
** RR: API -> MMI
*/
extern OutEvtDsc_t api_QualReportCnf;
extern OutEvtDsc_t api_NwCsRegStatusInd;
extern OutEvtDsc_t api_NwPsRegStatusInd;
extern OutEvtDsc_t api_NwListInd;
extern OutEvtDsc_t api_NwListAbortInd;
extern OutEvtDsc_t api_NwStoreListInd;
extern OutEvtDsc_t api_PowerScanRsp;
/*
** MM: API -> MMI
*/
extern OutEvtDsc_t api_NwkInfoInd;

/*
** CC: API -> MMI
*/
extern OutEvtDsc_t api_CcTIInd           ;
extern OutEvtDsc_t api_CcSetupInd        ;
extern OutEvtDsc_t api_CcCallProcInd     ;
extern OutEvtDsc_t api_CcProgInd         ;
extern OutEvtDsc_t api_CcAlertInd        ;
extern OutEvtDsc_t api_CcConnectInd      ;
extern OutEvtDsc_t api_CcConnectAckInd   ;
extern OutEvtDsc_t api_CcDiscInd         ;
extern OutEvtDsc_t api_CcRelInd          ;
extern OutEvtDsc_t api_CcRelCmptInd      ;
extern OutEvtDsc_t api_CcErrorInd        ;
extern OutEvtDsc_t api_CcCallHoldCnf     ;
extern OutEvtDsc_t api_CcCallRtrvCnf     ;
extern OutEvtDsc_t api_CcCallWaitingInd  ;
extern OutEvtDsc_t api_CcModifyInd       ;
extern OutEvtDsc_t api_CcModifyCmpltInd  ;
extern OutEvtDsc_t api_CcModifyRejInd    ;
extern OutEvtDsc_t api_CcNotifyInd       ;
extern OutEvtDsc_t api_CcAudioOnInd      ;
extern OutEvtDsc_t api_CcFacilityInd     ;
extern OutEvtDsc_t api_CcUserInfoInd     ;
extern OutEvtDsc_t api_CcCongCtrlInd     ;
#ifdef __AOC__
extern OutEvtDsc_t api_CcAocCCMInd       ;
#endif
extern OutEvtDsc_t api_CcChanModeInd     ;

extern OutEvtDsc_t api_CcReestabBeginInd;
extern OutEvtDsc_t api_CcReestabCnfInd;
/*
** SMS: API -> MMI
*/
// SMS-PP
extern OutEvtDsc_t api_SmsPPAckIndEvt    ;
extern OutEvtDsc_t api_SmsPPErrorIndEvt  ;
extern OutEvtDsc_t api_SmsPPReceiveIndEvt;
// SMS-CB
extern OutEvtDsc_t api_CbMsgRecInd       ;
extern OutEvtDsc_t api_CbPagRecInd       ;

/*
** SS: API -> MMI
*/
extern OutEvtDsc_t api_SsActionCnf;
extern OutEvtDsc_t api_SsTIInd;
extern OutEvtDsc_t api_SsErrorInd;
extern OutEvtDsc_t api_SsActionInd;

/*
** SND: API -> MMI
*/
extern OutEvtDsc_t api_SndDataInd;
extern OutEvtDsc_t api_SndQueueCreateInd;

/*
** PDP: API -> MMI
*/
extern OutEvtDsc_t api_PdpCtxActivateCnfEvt  ;
extern OutEvtDsc_t api_PdpCtxSmIndexIndEvt   ;
extern OutEvtDsc_t api_PdpCtxDeactivateCnfEvt;
extern OutEvtDsc_t api_PdpCtxActivateRejEvt  ;
extern OutEvtDsc_t api_PdpCtxModifyRejEvt    ;
extern OutEvtDsc_t api_PdpCtxActivateIndEvt  ;
extern OutEvtDsc_t api_PdpCtxModifyCnfEvt    ;
extern OutEvtDsc_t api_PdpCtxModifyIndEvt    ;
extern OutEvtDsc_t api_PdpCtxDeactivateIndEvt;


extern OutEvtDsc_t api_PSDataInd;

/*
** SIM: API -> MMI
*/
extern OutEvtDsc_t api_SimResetCnf;
extern OutEvtDsc_t api_SimStatusErrorInd;
extern OutEvtDsc_t api_SimReadBinaryCnf;
extern OutEvtDsc_t api_SimUpdateBinaryCnf;
extern OutEvtDsc_t api_SimReadRecordCnf;
extern OutEvtDsc_t api_SimUpdateRecordCnf;
extern OutEvtDsc_t api_SimInvalidateCnf;
extern OutEvtDsc_t api_SimRehabilitateCnf;
extern OutEvtDsc_t api_SimSeekCnf;
extern OutEvtDsc_t api_SimIncreaseCnf;
extern OutEvtDsc_t api_SimVerifyCHVCnf;
extern OutEvtDsc_t api_SimChangeCHVCnf;
extern OutEvtDsc_t api_SimDisableCHVCnf;
extern OutEvtDsc_t api_SimEnableCHVCnf;
extern OutEvtDsc_t api_SimUnblockCHVCnf;
extern OutEvtDsc_t api_SimElemFileStatusCnf;
extern OutEvtDsc_t api_SimDedicFileStatusCnf;
extern OutEvtDsc_t api_SimTerminalProfileCnf;
extern OutEvtDsc_t api_SimTerminalEnvelopeCnf;
extern OutEvtDsc_t api_SimTerminalResponseCnf;
extern OutEvtDsc_t api_SimFetchInd;
extern OutEvtDsc_t api_SimSelectApplicationCnf;
extern OutEvtDsc_t api_SimSeekAllCnf;

/*
** RLP: API -> MMI
*/
extern OutEvtDsc_t api_CrlpRdInd;

/*
** Flow Control: API - > MMI
*/
extern OutEvtDsc_t api_FlowCtrIndEvt;


/*
==========================================
             Outgoing Events To RR
==========================================
*/
extern OutEvtDsc_t stk_SimOpenInd   ;
extern OutEvtDsc_t stk_SimClosedInd ;

extern OutEvtDsc_t rr_NwSelModeReq   ;
extern OutEvtDsc_t rr_NwListReq      ;
extern OutEvtDsc_t rr_NwListAbortReq ;
extern OutEvtDsc_t rr_NwSelReq       ;
extern OutEvtDsc_t rr_NwReselReq     ;
extern OutEvtDsc_t rr_QualReportReq  ;
extern OutEvtDsc_t rr_StartReq       ;
extern OutEvtDsc_t rr_CurCellInfoReq ;
extern OutEvtDsc_t rr_AdjCellInfoReq ; 
extern OutEvtDsc_t rr_SyncInfoReq;
extern OutEvtDsc_t rr_PowerScanReq;
extern OutEvtDsc_t rr_DetectMBSReq   ;

/*
==========================================
             Outgoing Events To MM
==========================================
*/   
extern OutEvtDsc_t stk_StopReq    ;
extern OutEvtDsc_t mm_StartReq    ;
extern OutEvtDsc_t mm_CsDetachReq ;
extern OutEvtDsc_t mm_PSAttachReq ;
extern OutEvtDsc_t mm_PSDetachReq ;

/*
==========================================
             Outgoing Events To CC
==========================================
*/
extern OutEvtDsc_t cc_SetupReq      ;
extern OutEvtDsc_t cc_CallConfReq   ;
extern OutEvtDsc_t cc_AlertReq      ;
extern OutEvtDsc_t cc_ConnectReq    ;
extern OutEvtDsc_t cc_ConnectAckReq ;
extern OutEvtDsc_t cc_DiscReq       ;
extern OutEvtDsc_t cc_RelReq        ;
extern OutEvtDsc_t cc_RelCmptReq    ;
extern OutEvtDsc_t cc_CallHoldReq   ;
extern OutEvtDsc_t cc_CallRtrvReq   ;
extern OutEvtDsc_t cc_StartDtmfReq  ;
extern OutEvtDsc_t cc_StopDtmfReq   ;
extern OutEvtDsc_t cc_ModifyReq     ;
extern OutEvtDsc_t cc_ModifyCmpltReq;
extern OutEvtDsc_t cc_ModifyRejReq  ;
extern OutEvtDsc_t cc_NotifyReq     ;
extern OutEvtDsc_t cc_FacilityReq   ;
extern OutEvtDsc_t cc_UserInfoReq   ;


/*
==========================================
             Outgoing Events To SMS
==========================================
*/
// SMS-PP
extern OutEvtDsc_t smspp_SendReqEvt      ;
extern OutEvtDsc_t smspp_SendMMAReqEvt   ;
extern OutEvtDsc_t smspp_AckReqEvt       ;
extern OutEvtDsc_t smspp_ErrorReqEvt     ;
extern OutEvtDsc_t smspp_AbortMMAReqEvt  ;
// SMS-CB
extern OutEvtDsc_t cb_ActivateReq        ;
extern OutEvtDsc_t cb_DeactivateReq      ;

extern OutEvtDsc_t smspp_AbortReqEvt;
/*
==========================================
             Outgoing Events To SS
==========================================
*/
extern OutEvtDsc_t ss_ActionReq;

/*
==========================================
             Outgoing Events To USSD
==========================================
*/

/*
==========================================
             Outgoing Events To PDP
==========================================
*/
extern OutEvtDsc_t sm_PdpCtxActivateReqEvt   ;
extern OutEvtDsc_t sm_PdpSetMinQoSReqEvt     ;
extern OutEvtDsc_t sm_PdpCtxModifyReqEvt     ;
extern OutEvtDsc_t sm_PdpCtxModifyAccEvt     ;
extern OutEvtDsc_t sm_PdpCtxDeactivateReqEvt ;
extern OutEvtDsc_t sm_OPdpCtxActivateRejEvt  ;

/*
==========================================
             Outgoing Events To SIM
==========================================
*/
extern OutEvtDsc_t sim_SimResetReq;
extern OutEvtDsc_t sim_SimReadBinaryReq;
extern OutEvtDsc_t sim_SimUpdateBinaryReq;
extern OutEvtDsc_t sim_SimReadRecordReq;
extern OutEvtDsc_t sim_SimUpdateRecordReq;
extern OutEvtDsc_t sim_SimInvalidateReq;
extern OutEvtDsc_t sim_SimRehabilitateReq;
extern OutEvtDsc_t sim_SimSeekReq;
extern OutEvtDsc_t sim_SimIncreaseReq;
extern OutEvtDsc_t sim_SimVerifyChvReq;
extern OutEvtDsc_t sim_SimChangeChvReq;
extern OutEvtDsc_t sim_SimDisableChvReq;
extern OutEvtDsc_t sim_SimEnableChvReq;
extern OutEvtDsc_t sim_SimUnblockChvReq;
extern OutEvtDsc_t sim_SimElemFileStatusReq;
extern OutEvtDsc_t sim_SimDedicFileStatusReq;
extern OutEvtDsc_t sim_SimTerminalProfileReq;
extern OutEvtDsc_t sim_SimTerminalEnvelopeReq;
extern OutEvtDsc_t sim_SimTerminalResponseReq;
extern OutEvtDsc_t sim_SimStkActivateReq;
extern OutEvtDsc_t sim_SimPollReq;
extern OutEvtDsc_t sim_SimPollOffReq;
extern OutEvtDsc_t sim_SimSelectApplicationReq;
extern OutEvtDsc_t sim_SimSetUsimPbkReq;
extern OutEvtDsc_t sim_simPowerOffReq ; // dmx support SIM_CLSOE 20150812
extern OutEvtDsc_t sim_simRetryReq;
extern OutEvtDsc_t sim_SimSeekAllReq;
/*
==========================================
             Outgoing Events To RLP
==========================================
*/
extern OutEvtDsc_t rlp_OCrlpSetReqEvt;
extern OutEvtDsc_t rlp_OCrlpRdReqEvt;

//add by yanzhili
#ifdef LTE_NBIOT_SUPPORT
/*
==========================================
             Outgoing Events To MMC
==========================================
*/   
extern OutEvtDsc_t api_MmcStartReqEvt;
extern OutEvtDsc_t api_MmcNwSelReqEvt;
extern OutEvtDsc_t api_MmcStopReqEvt;
extern OutEvtDsc_t api_MmcNwListReqEvt;
extern OutEvtDsc_t api_MmcNwListAbortReqEvt;

// to SMS
extern OutEvtDsc_t api_CmcSmsPPSendReqEvt;
extern OutEvtDsc_t api_CmcSmsPPSendMMAReqEvt;
extern OutEvtDsc_t api_CmcSmsPPAbortReqEvt;
extern OutEvtDsc_t api_CmcSmsPPAckReqEvt;
extern OutEvtDsc_t api_CmcSmsPPErrorReqEvt;

// to ESM
extern OutEvtDsc_t api_CmcPdpCtxActivateReqEvt;
#ifdef __MINQOS__
extern OutEvtDsc_t api_CmcPdpSetMinQoSReqEvt;
#endif
extern OutEvtDsc_t api_CmcPdpCtxModifyReqEvt;
extern OutEvtDsc_t api_CmcPdpCtxModifyAccEvt;
extern OutEvtDsc_t api_CmcPdpCtxDeactivateReqEvt;
extern OutEvtDsc_t api_CmcDefaultPdnCtxConfigReqEvt;

// to RABM
extern OutEvtDsc_t api_RabmDataReqLteEvt;


/*
==========================================
             Incoming Events from MMI
==========================================
*/

// From MMI to MMC
extern InEvtDsc_t api_NbStopReqEvt;
extern InEvtDsc_t api_NbNwSelReqEvt;
extern InEvtDsc_t api_NbNwListReqEvt;
extern InEvtDsc_t api_NbNwListAbortReqEvt;

// From MMI to SMS
extern InEvtDsc_t api_NbSmsPPSendReqEvt;
extern InEvtDsc_t api_NbSmsPPSendMMAReqEvt;
extern InEvtDsc_t api_NbSmsPPAbortReqEvt;
extern InEvtDsc_t api_NbSmsPPAckReqEvt;
extern InEvtDsc_t api_NbSmsPPErrorReqEvt;

// From MMI to ESM
extern InEvtDsc_t api_NbPdpCtxActivateReqEvt;
#ifdef __MINQOS__
extern InEvtDsc_t api_NbPdpSetMinQoSReqEvt;
#endif
extern InEvtDsc_t api_NbPdpCtxModifyReqEvt;
extern InEvtDsc_t api_NbPdpCtxModifyAccEvt;
extern InEvtDsc_t api_NbPdpCtxDeactivateReqEvt;
extern InEvtDsc_t api_NbDefaultPdnCtxConfigReqEvt;

//From API to RABM
extern InEvtDsc_t api_NbPsDataReqLteEvt;

//From ESM to CSW
extern InEvtDsc_t nb_ApiPdpCtxActivateCnfEvt;
extern InEvtDsc_t nb_ApiPdpCtxSmIndexIndEvt;
extern InEvtDsc_t nb_ApiPdpCtxDeactivateCnfEvt;
extern InEvtDsc_t nb_ApiPdpCtxActivateRejEvt;
extern InEvtDsc_t nb_ApiPdpCtxModifyRejEvt;
extern InEvtDsc_t nb_ApiPdpCtxActivateIndEvt;
extern InEvtDsc_t nb_ApiPdpCtxModifyCnfEvt;
extern InEvtDsc_t nb_ApiPdpCtxModifyIndEvt;
extern InEvtDsc_t nb_ApiPdpCtxDeactivateIndEvt;

//From GMS to CSW
extern InEvtDsc_t nb_ApiSmsppAckIndEvt;
extern InEvtDsc_t nb_ApiSmsppErrorIndEvt;
extern InEvtDsc_t nb_ApiSmsppReceiveIndEvt;

//From MMC to CSW
extern InEvtDsc_t nb_ApiNwPsRegIndEvt;
extern InEvtDsc_t nb_ApiNwListIndEvt;
extern InEvtDsc_t nb_ApiSimStatusErrorIndEvt;
extern InEvtDsc_t nb_ApiNwkInfoIndEvt;
extern InEvtDsc_t nb_ApiStkStopCnfEvt;

//From RABM to CSW
extern InEvtDsc_t nb_ApiRabmPsDataIndEvt;
#endif


#define API_IL_MSG ( 1 << 0 )
InEvtDsc_t * api_GiveDsc( u32 MsgId );


#endif
