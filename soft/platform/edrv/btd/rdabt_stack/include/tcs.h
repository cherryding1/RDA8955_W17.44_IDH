/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/




#ifndef BLUETOOTH_TCS_API_DECLARED
#define BLUETOOTH_TCS_API_DECLARED


#include "l2cap.h"


#define RDABT_TCS_REGISTER_TCSBIN       0x1
#define RDABT_TCS_REGISTER_TCSCORDLESS  0x2



/* TCS Call Control Parameter Bit Masks */
#define RDABT_TCS_CC_PARAM_CALLCLASS          0x0001
#define RDABT_TCS_CC_PARAM_SENDINGCOMPLETE    0x0002
#define RDABT_TCS_CC_PARAM_BEARERCAPABILITY   0x0004
#define RDABT_TCS_CC_PARAM_CAUSE              0x0008
#define RDABT_TCS_CC_PARAM_PROGRESSINDICATOR  0x0010
#define RDABT_TCS_CC_PARAM_SIGNAL             0x0020
#define RDABT_TCS_CC_PARAM_KEYPADFACILITY     0x0040
#define RDABT_TCS_CC_PARAM_CALLINGPARTYNUMBER 0x0080
#define RDABT_TCS_CC_PARAM_CALLEDPARTYNUMBER  0x0100
#define RDABT_TCS_CC_PARAM_AUDIOCONTROL       0x0200
#define RDABT_TCS_CC_PARAM_SCOHANDLE          0x0400
#define RDABT_TCS_CC_PARAM_DESTCID            0x0800
#define RDABT_TCS_CC_PARAM_COMPANYSPECIFIC    0x1000
#define RDABT_TCS_CC_BEARERHANDLETYPE_MASK    0x0400

#define RDABT_TCS_CC_PARAM_CALLCLASS_EXISTS(X)          ((X) & RDABT_TCS_CC_PARAM_CALLCLASS)
#define RDABT_TCS_CC_PARAM_SENDINGCOMPLETE_EXISTS(X)    (((X) & RDABT_TCS_CC_PARAM_SENDINGCOMPLETE) >> 1)
#define RDABT_TCS_CC_PARAM_BEARERCAPABILITY_EXISTS(X)   (((X) & RDABT_TCS_CC_PARAM_BEARERCAPABILITY) >> 2)
#define RDABT_TCS_CC_PARAM_CAUSE_EXISTS(X)              (((X) & RDABT_TCS_CC_PARAM_CAUSE) >> 3)
#define RDABT_TCS_CC_PARAM_PROGRESSINDICATOR_EXISTS(X) (((X) & RDABT_TCS_CC_PARAM_PROGRESSINDICATOR) >> 4)
#define RDABT_TCS_CC_PARAM_SIGNAL_EXISTS(X)             (((X) & RDABT_TCS_CC_PARAM_SIGNAL) >> 5)
#define RDABT_TCS_CC_PARAM_KEYPADFACILITY_EXISTS(X)            (((X) & RDABT_TCS_CC_PARAM_KEYPADFACILITY) >> 6)
#define RDABT_TCS_CC_PARAM_CALLINGPARTYNUMBER_EXISTS(X) (((X) & RDABT_TCS_CC_PARAM_CALLINGPARTYNUMBER) >> 7)
#define RDABT_TCS_CC_PARAM_CALLEDPARTYNUMBER_EXISTS(X)  (((X) & RDABT_TCS_CC_PARAM_CALLEDPARTYNUMBER) >> 8)
#define RDABT_TCS_CC_PARAM_AUDIOCONTROL_EXISTS(X)       (((X) & RDABT_TCS_CC_PARAM_AUDIOCONTROL) >> 9)
#define RDABT_TCS_CC_PARAM_SCOHANDLE_EXISTS(X)       (((X) & RDABT_TCS_CC_PARAM_SCOHANDLE) >> 10)
#define RDABT_TCS_CC_PARAM_DESTCID_EXISTS(X)       (((X) & RDABT_TCS_CC_PARAM_DESTCID) >> 11)
#define RDABT_TCS_CC_ILLEGAL_BITMASK(X)  (X > 0x1FFF)
#define RDABT_TCS_CC_PARAM_COMPANYSPECIFIC_EXISTS(X)    (((X) & RDABT_TCS_CC_PARAM_COMPANYSPECIFIC) >> 12)

/* Info Elements */
/* Single Octet */
#define RDABT_TCS_IE_SO_SENDING_COMPLETE 0xA1

/* Double Octet */
#define RDABT_TCS_IE_DO_CALL_CLASS 0xC0
#define RDABT_TCS_IE_DO_CAUSE      0xC1
#define RDABT_TCS_IE_DO_PROGRESS_INDICATOR 0xC2
#define RDABT_TCS_IE_DO_SIGNAL     0xC3
#define RDABT_TCS_IE_DO_KEYPAD_FACILITY 0xC4
#define RDABT_TCS_IE_DO_SCO_HANDLE 0xC5

/* Variable Length */
#define RDABT_TCS_IE_VL_CLOCK_OFFSET 0x00
#define RDABT_TCS_IE_VL_CONFIG_DATA  0x01
#define RDABT_TCS_IE_VL_BEARER_CAP   0x02
#define RDABT_TCS_IE_VL_DEST_CID     0x03
#define RDABT_TCS_IE_VL_CALLING_PARTY_NUM 0x04
#define RDABT_TCS_IE_VL_CALLED_PARTY_NUM 0x05
#define RDABT_TCS_IE_VL_AUDIO_CONTROL 0x06
#define RDABT_TCS_IE_VL_COMPANY_SPECIFIC 0x07

/* Audio Control IE  - TCS 7.4.2 */
#define RDABT_TCS_AUDIO_VOL_INC 0x00
#define RDABT_TCS_AUDIO_VOL_DEC 0x01
#define RDABT_TCS_AUDIO_MIC_GAIN_INC 0x02
#define RDABT_TCS_AUDIO_MIC_GAIN_DEC 0x03

/* Bearer Cap - TCS 7.4.3 */
/* octet 3*/
#define RDABT_TCS_BEARER_CAP_LINK_TYPE_SCO 0x00
#define RDABT_TCS_BEARER_CAP_LINK_TYPE_ACL 0x01
#define RDABT_TCS_BEARER_CAP_LINK_TYPE_NONE 0x02

/*octet 4 bits 1-5*/
#define RDABT_TCS_BEARER_CAP_SCO_PTYPE_HV1 0x05
#define RDABT_TCS_BEARER_CAP_SCO_PTYPE_HV2 0x06
#define RDABT_TCS_BEARER_CAP_SCO_PTYPE_HV3 0x07
#define RDABT_TCS_BEARER_CAP_SCO_PTYPE_DV  0x08

/*octet 4 bits 6-8*/
#define RDABT_TCS_BEARER_CAP_SCO_CTYPE_CVSD 0x20
#define RDABT_TCS_BEARER_CAP_SCO_CTYPE_PCM_ALAW 0x40
#define RDABT_TCS_BEARER_CAP_SCO_CTYPE_PCM_ULAW 0x60

#define RDABT_TCS_BEARER_CAP_ACL_RFCOMM_OVER_L2CAP 0x00
#define RDABT_TCS_BEARER_CAP_ACL_PROTO_PPP 0x10
#define RDABT_TCS_BEARER_CAP_ACL_PROTO_IP 0x20

/* Call Class - TCS 7.4.4 */
#define RDABT_TCS_CALL_CLASS_EXTERNAL 0x00
#define RDABT_TCS_CALL_CLASS_INTERCOM 0x01
#define RDABT_TCS_CALL_CLASS_SERVICE 0x02
#define RDABT_TCS_CALL_CLASS_EMERGENCY 0x03

/* Called + Calling Party Number - TCS 7.4.5/6 */
#define RDABT_TCS_NUM_TYPE_UNKNOWN 0x00
#define RDABT_TCS_NUM_TYPE_INTERNATIONAL 0x10
#define RDABT_TCS_NUM_TYPE_NATIONAL 0x20
#define RDABT_TCS_NUM_TYPE_NETWORK_SPECIFIC 0x30
#define RDABT_TCS_NUM_TYPE_SUBSCRIBER 0x40
#define RDABT_TCS_NUM_TYPE_ABBREVIATED 0x60
#define RDABT_TCS_NUM_TYPE_RESERVED 0x70

#define RDABT_TCS_NUM_PLAN_ID_UNKNOWN 0x00
#define RDABT_TCS_NUM_PLAN_ID_E164 0x01
#define RDABT_TCS_NUM_PLAN_ID_X121 0x03
#define RDABT_TCS_NUM_PLAN_ID_RESV 0x04
#define RDABT_TCS_NUM_PLAN_ID_NATIONAL 0x08
#define RDABT_TCS_NUM_PLAN_ID_PRIVATE 0x09

#define RDABT_TCS_CALLING_PRESENTATION_ALLOWED 0x00
#define RDABT_TCS_CALLING_PRESENTATION_RESTRICTED 0x20
#define RDABT_TCS_CALLING_PRESENTATION_UNAVAIL 0x40
#define RDABT_TCS_CALLING_PRESENTATION_RESERVED 0x60

#define RDABT_TCS_CALLING_SCREENED_USER_NOT 0x00
#define RDABT_TCS_CALLING_SCREENED_USER_PASSED 0x01
#define RDABT_TCS_CALLING_SCREENED_USER_FAILED 0x02
#define RDABT_TCS_CALLING_SCREENED_NETWORK 0x03

/* FROM ITU 2.2.5 Q850 - T.B.D. */
/* Cause - TCS 7.4.7 */
#define RDABT_TCS_CAUSE_UNASSIGNEDNUMBER            0x01
#define RDABT_TCS_CAUSE_USERBUSY                    0x11
#define RDABT_TCS_CAUSE_CALLREJECTED                0x15
#define RDABT_TCS_CAUSE_NONSELECTEDUSERCLEARING 0x1A
#define RDABT_TCS_CAUSE_RECOVERYONTIMEREXPIRY       0x66


/* Company Specific */
#define RDABT_TCS_COMPANY_SPEC_ERICSSON 0x00
#define RDABT_TCS_COMPANY_SPEC_NOKIA 0x01
#define RDABT_TCS_COMPANY_SPEC_INTEL 0x02
#define RDABT_TCS_COMPANY_SPEC_IBM 0x03
#define RDABT_TCS_COMPANY_SPEC_TOSHIBA 0x04

/* Signal - TCS 7.4.16 */
#define RDABT_TCS_SIGNAL_EXTERNAL 0x40
#define RDABT_TCS_SIGNAL_INTERNAL 0x41
#define RDABT_TCS_SIGNAL_CALLBACK 0x42


/* Progress Indicator */
#define RDABT_TCS_PROGRESSINDICATOR_INBANDINFOAVAILABLE 0x08

/* BT Character SET */
#define RDABT_TCS_CHARSET_DIALLING_PAUSE 0x05
#define RDABT_TCS_CHARSET_REGISTER_RECALL 0x16


#define RDABT_TCS_PSM_TCSBIN        0x05
#define RDABT_TCS_PSM_TCSCORDLESS 0x07
/* Two Profiles call types - matches psm values used for profiles */
#define RDABT_TCS_CALLTYPE_TCSBIN      RDABT_TCS_PSM_TCSBIN
#define RDABT_TCS_CALLTYPE_TCSCORDLESS RDABT_TCS_PSM_TCSCORDLESS




/* for pending incoming external calls...waiting on response to a broadcast */
#define RDABT_TCS_CALLHANDLEUNASSIGNED 0xFFFF

/* Forward reference to struct st_t_TCS_callInstance
   This structure is defined in the TCS-Bin layer code.
   A forward reference is sufficent in this file, the application
   does not need to access the members of the structure, but just
   maintain the pointer to the struct to be used as a parameter
   in subsequent calls
*/
struct st_t_TCS_callInstance;


typedef struct st_t_TCS_aclBearer
{
    t_L2_ChannelFlowSpec qos;
    u_int8 layerInfo; /* Bits 7-4: Layer 3 | Bits 3-0: Layer 2 */
} t_TCS_aclBearer;



typedef struct st_t_TCS_callParams
{
    u_int16 bitMask;
    u_int8 callClass;
    /* Bearer Capability */
    u_int8 linkType;
    /* union { */
    u_int8 scoPacketType;
    t_TCS_aclBearer *aclBearer;
    /* }; */
    u_int8 cause;
    u_int8 progressIndicator;
    u_int8 signal;
    u_int8 keypadFacility;
    /* Calling Party Number */
    u_int8 callingPartyNumberType;
    u_int8 callingPartyPresScreenIndicator;
    u_int8 callingPartyNumberLength;
    u_int8 *callingPartyNumber;
    /* Called Party Number */
    u_int8 calledPartyNumberType;
    u_int8 calledPartyNumberLength;
    u_int8 *calledPartyNumber;
    /* Audio Control */
    u_int8 audioControlInfoLength;
    u_int8 *audioControlInfo;
    /* Bearer Handle */
    /* union { */
    u_int16 scoHandle;
    u_int16 cid;
    /* }; */
    /* Company Specific Info */
    u_int16 companyID;
    u_int8 companySpecificLength;
    u_int8 *companySpecific;
} t_TCS_callParams;


typedef struct st_t_TCS_wugMember
{
    t_bdaddr BDAddress;
    u_int8 internal_num[2];
} t_TCS_wugMember;


typedef struct st_t_TCS_callbacks
{
    void(*callSetupIndication)(t_bdaddr bdAddress, u_int16 callHandle, u_int16 callType, t_TCS_callParams *callParams);
    void (*setupAckIndication)(t_bdaddr bdAddress, u_int16 callHandle, t_TCS_callParams *callParams);
    void (*informationIndication)(u_int16 callHandle, t_TCS_callParams *callParams);
    void (*callProceedingIndication)(t_bdaddr bdAddress, u_int16 callHandle, t_TCS_callParams *callParams);
    void (*alertingIndication)(t_bdaddr bdAddress, u_int16 callHandle, t_TCS_callParams *callParams);
    void(*connectIndication)(t_bdaddr bdAddress, u_int16 callHandle, t_TCS_callParams *callParams);
    void (*progressIndication)(u_int16 callHandle, t_TCS_callParams *callParams);
    void (*callCompleteIndication)(t_bdaddr address, u_int16 callHandle);
    void(*disconnectIndication)(u_int16 callHandle, t_TCS_callParams *callParams);
    void(*releaseIndication)(u_int16 callHandle, t_TCS_callParams *callParams);
    void (*callReleaseCompleteIndication)(t_bdaddr address, u_int16 callHandle, t_TCS_callParams *callParams);
    void (*sigChannelConnectIndication)(t_bdaddr bdAddress, u_int16 psm, u_int16 ACLHandle);
    void (*sigChannelDisconnectIndication)(t_bdaddr bdAddress, u_int16 psm);
    void(*CLInfoIndication)(t_TCS_callParams *callParams);
    void(*startDTMFIndication)(u_int16 callHandle, t_TCS_callParams *callParams);
    void (*startDTMFAckIndication)(u_int16 callHandle, t_TCS_callParams *callParams);
    void (*startDTMFRejectIndication)(u_int16 callHandle, t_TCS_callParams *callParams);
    void(*stopDTMFIndication)(u_int16 callHandle, t_TCS_callParams *callParams);
    void (*stopDTMFAckIndication)(u_int16 callHandle, t_TCS_callParams *callParams);
    void (*accessRightsResponse)(u_int8 accept);
    int (*accessRightsRequest)(t_TCS_wugMember *wugMember);
    void (*listenSuggest)(void);
    int (*newWugInformationIndication)(t_TCS_wugMember **list, int num_entries);
    int (*listenReject)(u_int8 cause);
    int (*listenAccept)(void);
} t_TCS_callbacks;


/* TCS Call Control API - Common to TL and GW API Interface */
APIDECL1 int APIDECL2 TCS_CallProceeding(u_int16 callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_Information(u_int16 callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_ConnectAcknowledge(u_int16 callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_Disconnect(u_int16 callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_ReleaseComplete(struct st_t_TCS_callInstance *callInstance);
APIDECL1 int APIDECL2 TCS_Release(u_int16 callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_StartDTMF(u_int16 callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_StartDTMFAcknowledge(u_int16 callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_StartDTMFReject(u_int16 callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_StopDTMF(u_int16 callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_StopDTMFAcknowledge(u_int16 callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_Progress(u_int16 callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_Connect(struct st_t_TCS_callInstance *callInstance);
APIDECL1 int APIDECL2 TCS_Alerting(u_int16 callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_SetupAcknowledge(u_int16 callHandle, t_TCS_callParams *callParams);

#ifndef BLUETOOTH_NO_TCS_TERMINAL
#   include "tcs_terminal.h"
#endif


#ifndef BLUETOOTH_NO_TCS_GATEWAY
#   include "tcs_gateway.h"
#endif



#endif /* BLUETOOTH_TCS_API_DECLARED */

