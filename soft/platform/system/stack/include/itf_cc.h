//------------------------------------------------------------------------------
//  $Log: itf_cc.h,v $
//  Revision 1.2  2005/12/29 17:38:44  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
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
  File       : ITF_CC.H
--------------------------------------------------------------------------------

  Scope      : Interface Mesages provided by CC                       

  History    :
--------------------------------------------------------------------------------
  Nov 23 05  |  ABA   | AOC                 
  Aug 06 03  |  OTH   | Creation            
================================================================================
*/

#ifndef __ITF_CC_H__
#define __ITF_CC_H__
 
#include "sxs_type.h"
#include "cmn_defs.h"

#include "itf_api.h"   // Match CC itf from here

/*
** Primitive ID definitions
*/
// API -> CC
#define CC_SETUP_REQ       ( HVY_CC + 0x00 )
#define CC_CALLCONF_REQ    ( HVY_CC + 0x01 )
#define CC_ALERT_REQ       ( HVY_CC + 0x02 )
#define CC_CONNECT_REQ     ( HVY_CC + 0x03 )
#define CC_DISC_REQ        ( HVY_CC + 0x04 )
#define CC_REL_REQ         ( HVY_CC + 0x05 )
#define CC_RELCMPLT_REQ    ( HVY_CC + 0x06 )
#define CC_STARTDTMF_REQ   ( HVY_CC + 0x07 )
#define CC_STOPDTMF_REQ    ( HVY_CC + 0x08 )
#define CC_CALLHOLD_REQ    ( HVY_CC + 0x09 )
#define CC_CALLRTRV_REQ    ( HVY_CC + 0x0A )
#define CC_MODIFY_REQ      ( HVY_CC + 0x0B )
#define CC_MODIFYCMPLT_REQ ( HVY_CC + 0x0C )
#define CC_MODIFYREJ_REQ   ( HVY_CC + 0x0D )
#define CC_NOTIFY_REQ      ( HVY_CC + 0x0E )
#define CC_FACILITY_REQ    ( HVY_CC + 0x0F )
#define CC_USER_INFO_REQ   ( HVY_CC + 0x10 )
                                         
// CC  -> API
#define CC_TI_IND          ( HVY_CC + 0x20 )
#define CC_SETUP_IND       ( HVY_CC + 0x21 )
#define CC_CALLPROC_IND    ( HVY_CC + 0x22 )
#define CC_PROG_IND        ( HVY_CC + 0x23 )
#define CC_ALERT_IND       ( HVY_CC + 0x24 )
#define CC_CONNECT_IND     ( HVY_CC + 0x25 )
#define CC_CONNECTACK_IND  ( HVY_CC + 0x26 )
#define CC_DISC_IND        ( HVY_CC + 0x27 )
#define CC_REL_IND         ( HVY_CC + 0x28 )
#define CC_RELCMPLT_IND    ( HVY_CC + 0x29 )
#define CC_ERROR_IND       ( HVY_CC + 0x2A )
#define CC_CALLHOLD_CNF    ( HVY_CC + 0x2B )
#define CC_CALLRTRV_CNF    ( HVY_CC + 0x2C )
#define CC_MODIFY_IND      ( HVY_CC + 0x2D )
#define CC_MODIFYCMPLT_IND ( HVY_CC + 0x2E )
#define CC_MODIFYREJ_IND   ( HVY_CC + 0x2F )
#define CC_NOTIFY_IND      ( HVY_CC + 0x30 )
#define CC_AUDIOON_IND     ( HVY_CC + 0x31 )
#define CC_FACILITY_IND    ( HVY_CC + 0x32 )
#define CC_USER_INFO_IND   ( HVY_CC + 0x35 )
#define CC_CONG_CTRL_IND   ( HVY_CC + 0x34 )


// add by dingmx
#define CC_REESTAB_BEGIN_IND  ( HVY_CC + 0x36 )
#define CC_REESTAB_CNF_IND  ( HVY_CC + 0x37 )
#define CC_REESTAB_ERR_IND  ( HVY_CC + 0x38 )


/*******************
** API -> CC
********************/
/*
** Structure for the supported SS-codes
*/
typedef api_SsCode_t cc_SsCode_t;
#define CC_SS_NO_CODE API_SS_NO_CODE

/*
** Structure for the applicable operations
*/
typedef api_SsOperation_t cc_SsOperation_t;

/*
** Structure for the Charging Information Sequence -- See 3GPP 22.024
*/
typedef api_CrssChargInfo_t cc_CrssChargInfo_t;
#define CC_CRSS_E_NOT_VALID API_CRSS_E_NOT_VALID

/*
** Structure for the Explicit Call Transfer Indicator
*/
typedef api_CrssECTInd_t cc_CrssECTInd_t;

/*
** Structure for the Name Set
*/
typedef api_CrssNameSet_t cc_CrssNameSet_t;

/*
** Structure for the Name Indicator
*/
typedef api_CrssNameInd_t cc_CrssNameInd_t;

/*
** Structure for the Call Deflection
*/
typedef api_CrssDeflec_t cc_CrssDeflec_t;

/*
** Structure for the Call Related SS parameters
*/
typedef api_CrssUUS_t cc_CrssUUS_t;

/*
** Generic structure for the Facility IE
*/
typedef api_CrssFacilityIndIE_t cc_CrssFacilityIndIE_t;
typedef api_CrssFacilityReqIE_t cc_CrssFacilityReqIE_t;

/*
** Cause possible values
*/
#define CC_SS_REJECT_COMPONENT API_SS_REJECT_COMPONENT
#define CC_SS_RETURN_ERROR     API_SS_RETURN_ERROR

/*
** Defines for the ss_Status field
*/
#define CC_CRSS_NO_STATUS                 API_CRSS_NO_STATUS                
#define CC_CRSS_MOCALL_HAS_BEEN_FORWARDED API_CRSS_MOCALL_HAS_BEEN_FORWARDED
#define CC_CRSS_MTCALL_HAS_BEEN_FORWARDED API_CRSS_MTCALL_HAS_BEEN_FORWARDED
#define CC_CRSS_MTCALL_IS_FORWARDED_CALL  API_CRSS_MTCALL_IS_FORWARDED_CALL 
#define CC_CRSS_CALL_ON_HOLD              API_CRSS_CALL_ON_HOLD             
#define CC_CRSS_CALL_RETRIEVED            API_CRSS_CALL_RETRIEVED           
#define CC_CRSS_CALL_IS_WAITING           API_CRSS_CALL_IS_WAITING          
#define CC_CRSS_CLIR_SUP_REJECTED         API_CRSS_CLIR_SUP_REJECTED        
#define CC_CRSS_STATUS_ACTIVE_OPERATIVE   API_CRSS_STATUS_ACTIVE_OPERATIVE  
#define CC_CRSS_UUS_REQUIRED              API_CRSS_UUS_REQUIRED             
#define CC_CRSS_UUS_NOT_REQUIRED          API_CRSS_UUS_NOT_REQUIRED
#define CC_CRSS_MPTY_INDICATION           API_CRSS_MPTY_INDICATION
#define CC_CRSS_RETURN_RESULT             API_CRSS_RETURN_RESULT
#define CC_CRSS_ERROR_IND                 API_CRSS_ERROR_IND

/*
** Defines needed directly in CC for the Network Error Code Field
*/
#define CC_SS_SYSTEM_FAILURE API_SS_SYSTEM_FAILURE


/*
================================================================================
  Structure  : cc_SetupReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Ask for the establishment of an outgoing call
================================================================================
*/
typedef api_CcSetupReq_t cc_SetupReq_t;
#define CC_REGULAR_CALL   API_CC_REGULAR_CALL
#define CC_EMERGENCY_CALL API_CC_EMERGENCY_CALL

/*
================================================================================
  Structure  : cc_CallConfReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Used to confirm an incoming call request
================================================================================
*/
typedef api_CcCallConfReq_t cc_CallConfReq_t;

/*
================================================================================
  Structure  : cc_AlertReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Indicate that user alerting has been initiated by the MS.
================================================================================
*/
typedef api_CcAlertReq_t cc_AlertReq_t;

/*
================================================================================
  Structure  : cc_ConnectReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Used to inform that the incoming call has been accepted by the MS
================================================================================
*/
typedef api_CcConnectReq_t cc_ConnectReq_t;

/*
================================================================================
  Structure  : cc_DiscReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Used to ask for the disconnection of a call
================================================================================
*/
typedef api_CcDiscReq_t cc_DiscReq_t;

/*
================================================================================
  Structure  : cc_ReleaseReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Indicate that the MS intends to release the TI
================================================================================
*/
typedef api_CcReleaseReq_t cc_ReleaseReq_t;

/*
================================================================================
  Structure  : cc_ReleaseCmpltReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Indicate that the MS has released the TI.
================================================================================
*/
typedef api_CcReleaseCmpltReq_t cc_ReleaseCmpltReq_t;

/*
================================================================================
  Structure  : cc_CallHoldReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Used to ask to hold a call
================================================================================
*/
typedef api_CcCallHoldReq_t cc_CallHoldReq_t;

/*
================================================================================
  Structure  : cc_CallRetrieveReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Used to ask to retrieve a call
================================================================================
*/
typedef api_CcCallRetrieveReq_t cc_CallRetrieveReq_t;

/*
================================================================================
  Structure  : cc_ModifyReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Used to ask to modify an existing call
================================================================================
*/
typedef api_CcModifyReq_t cc_ModifyReq_t;

/*
================================================================================
  Structure  : cc_ModifyCmpltReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Used to ask to send a modify complete message
================================================================================
*/
typedef api_CcModifyCmpltReq_t cc_ModifyCmpltReq_t;

/*
================================================================================
  Structure  : cc_ModifyRejReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Used to ask to send a modify reject message
================================================================================
*/
typedef api_CcModifyRejReq_t cc_ModifyRejReq_t;

/*
================================================================================
  Structure  : cc_NotifyReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Used to ask to send a notify message
================================================================================
*/
typedef api_CcNotifyReq_t cc_NotifyReq_t;

/*
================================================================================
  Structure  : cc_StartDTMFReq_t
  Structure  : cc_StopDTMFReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Used to start and Stop a DTMF transmission
================================================================================
*/
typedef api_CcStartDTMFReq_t   cc_StartDtmfReq_t ;
typedef api_CcStopDTMFReq_t    cc_StopDtmfReq_t ;


/*
================================================================================
  Structure  : cc_FacilityReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Used for the Call Related SS
================================================================================
*/
typedef api_CcFacilityReq_t cc_FacilityReq_t;


/*
================================================================================
  Structure  : api_CcUserInfoReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CC
  Scope      : Request the emission of a User Information message
================================================================================
*/
typedef api_CcUserInfoReq_t cc_UserInfoReq_t;


/********************
** CC  -> API
*********************/
/*
================================================================================
  Structure  : cc_TIInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used to indicate the chosen TI for the outgoing call in progress
================================================================================
*/
typedef api_CcTIInd_t cc_TIInd_t;


/*
================================================================================
  Structure  : cc_SetupInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used to indicate an incoming call
================================================================================
*/
typedef api_CcSetupInd_t cc_SetupInd_t;

#define CC_SIGNAL_INVALID       API_SIGNAL_INVALID
#define CC_ALERT_INVALID        API_ALERT_INVALID
#define CC_CAUSEOFNOCLI_INVALID API_CAUSEOFNOCLI_INVALID
#define CC_PROGIND_INVALID      API_PROGIND_INVALID

/*
================================================================================
  Structure  : cc_CallProcInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Nwk indicate that the requested call estab. info. has been
               received
================================================================================
*/
typedef api_CcCallProcInd_t cc_CallProcInd_t;

#define CC_NWKCCCAP_INVALID API_NWKCCCAP_INVALID

/*
================================================================================
  Structure  : cc_ProgressInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used to indicate the reception of a progress indicator message
================================================================================
*/
typedef api_CcProgressInd_t cc_ProgressInd_t;

/*
================================================================================
  Structure  : cc_AlertInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used to indicate that user alerting has been started by the 
               called party
================================================================================
*/
typedef api_CcAlertInd_t cc_AlertInd_t;

/*
================================================================================
  Structure  : cc_ConnectInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used by the nwk to inform that the outgoing call has been 
               accepted
================================================================================
*/
typedef api_CcConnectInd_t cc_ConnectInd_t;

/*
================================================================================
  Structure  : cc_ConnectAckInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used by the nwk to inform that the MS has been awarded the call
================================================================================
*/
typedef api_CcConnectAckInd_t cc_ConnectAckInd_t;

/*
================================================================================
  Structure  : cc_DiscInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used to indicate the end-to-end connection is cleared
================================================================================
*/
typedef api_CcDiscInd_t cc_DiscInd_t;

/*
================================================================================
  Structure  : cc_ReleaseInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used by the nwk to indicate that the nwk intends to release the
               TI
================================================================================
*/
typedef api_CcReleaseInd_t cc_ReleaseInd_t;

/*
================================================================================
  Structure  : cc_ReleaseCmpltInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used by the nwk to indicate that the nwk has released the TI
================================================================================
*/
typedef api_CcReleaseCmpltInd_t cc_ReleaseCmpltInd_t;

/*
================================================================================
  Structure  : cc_ErrorInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used by API to indicate that something went wrong
================================================================================
*/
typedef api_CcErrorInd_t cc_ErrorInd_t;

// Cause possible values
#define CC_NORMAL_RELEASE       API_NORMAL_RELEASE       
#define CC_LOW_LAYER_FAIL       API_LOW_LAYER_FAIL       
#define CC_REQUEST_IMPOSSIBLE   API_REQUEST_IMPOSSIBLE
#define CC_INCOMING_CALL_BARRED API_INCOMING_CALL_BARRED 
#define CC_OUTGOING_CALL_BARRED API_OUTGOING_CALL_BARRED 
#define CC_CALL_HOLD_REJECTED   API_CALL_HOLD_REJECTED
#define CC_CALL_RTRV_REJECTED   API_CALL_RTRV_REJECTED
#define CC_CC_TIMER_EXPIRY      API_CC_TIMER_EXPIRY
#define CC_PROTOCOL_ERROR       API_CC_PROTOCOL_ERROR

#define CC_DTMF_BUFFER_FULL   API_CC_DTMF_BUFFER_FULL

/*
================================================================================
  Structure  : cc_CallHoldCnf_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used to confirm the hold of a call
================================================================================
*/
typedef api_CcCallHoldCnf_t cc_CallHoldCnf_t;

/*
================================================================================
  Structure  : cc_CallRetrieveCnf_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used to confirm the call has been retrieved
================================================================================
*/
typedef api_CcCallRetrieveCnf_t cc_CallRetrieveCnf_t;

/*
================================================================================
  Structure  : cc_CallWaitingInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used to inform that a second call is waiting
================================================================================
*/
typedef api_CcCallWaitingInd_t cc_CallWaitingInd_t;

/*
================================================================================
  Structure  : cc_ModifyInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used to indicate an incoming MODIFY message
================================================================================
*/
typedef api_CcModifyInd_t cc_ModifyInd_t;

/*
================================================================================
  Structure  : cc_ModifyCmpltInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used to indicate an incoming MODIFY COMPLETE message
================================================================================
*/
typedef api_CcModifyCmpltInd_t cc_ModifyCmpltInd_t;

/*
================================================================================
  Structure  : cc_ModifyRejInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used to indicated an incoming MODIFY REJECT message
================================================================================
*/
typedef api_CcModifyRejInd_t cc_ModifyRejInd_t;

/*
================================================================================
  Structure  : cc_NotifyInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : Used to indicated an incoming NOTIFY message
================================================================================
*/
typedef api_CcNotifyInd_t cc_NotifyInd_t;

/*
================================================================================
  Structure  : cc_FacilityInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : A facility message has been received
================================================================================
*/
typedef api_CcFacilityInd_t cc_FacilityInd_t;

/*
================================================================================
  Structure  : cc_UserInfoInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : indicate the reception of a User Information message
================================================================================
*/
typedef api_CcUserInfoInd_t cc_UserInfoInd_t;

/*
================================================================================
  Structure  : cc_CongCtrlInd_t
--------------------------------------------------------------------------------
  Direction  : CC -> API
  Scope      : indicate the congestion of the network
================================================================================
*/
typedef api_CcCongCtrlInd_t cc_CongCtrlInd_t;



#endif
