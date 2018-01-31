// //////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2002-2006, Coolsand Technologies, Inc.
// All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
// 
// FILENAME: Itf_ss.h
// 
// DESCRIPTION:
// 
// 
// REVISION HISTORY:
// NAME            DATE            REMAKS
// Wangf | 20070529 | Add the ID of SS_ACTION_IND
// Wangf | 20070529 | Define ss_ActionInd_t
// //////////////////////////////////////////////////////////////////////////////

#ifndef __ITF_SS_H__
#define __ITF_SS_H__
 
#include "sxs_type.h"
#include "cmn_defs.h"

#include "itf_api.h"   // Match SS itf from here
#include "itf_mm.h"

// Primitive ID definitions

// API -> SS
#define SS_ACTION_REQ	( HVY_SS + 0x00 )  

// SS -> API
#define SS_ACTION_CNF	( HVY_SS + 0x10 )
#define SS_TI_IND		  ( HVY_SS + 0x11 )
#define SS_ERROR_IND	( HVY_SS + 0x12 )
#define SS_ACTION_IND	( HVY_SS + 0x13 ) // add by wangf 20070529, review by DMX

//SS -> MM
#define SS_ACTIVATION	MM_SS_ACTIVATION
  
//General typedefs
typedef api_SsProblemCodeTag_t				ss_ProblemCodeTag_t;
typedef api_SsGeneralProblemCodes_t			ss_GeneralProblemCodes_t;
typedef api_SsInvokeProblemCodes_t			ss_InvokeProblemCodes_t;
typedef api_SsReturnResultProblemCodes_t	ss_ReturnResultProblemCodes_t;
typedef api_SsReturnErrorProblemCodes_t		ss_ReturnErrorProblemCodes_t;
typedef api_SsProblemCodes_t				ss_ProblemCodes_t;
typedef api_SsRejectComponentTag_t			ss_RejectComponentTag_t;
typedef api_SsRejectComponentU8Tag_t		ss_RejectComponentU8Tag_t;
typedef api_SsCauseIEInfo_t					ss_CauseIEInfo_t;

//================================================================================
//  Structure  : ss_ActionReq_t
//--------------------------------------------------------------------------------
//  Direction  : API -> SS
//  Scope      : API requests a given supplementary serivce
//===============================================================================
typedef api_SsProcessUSSDataErrors_t		ss_ProcessUSSDataErrors_t;
typedef api_SsProcessUSSRequestErrors_t		ss_ProcessUSSRequestErrors_t;
typedef api_SsUSSRequestErrors_t			ss_USSRequestErrors_t;
typedef api_SsUSSNotifyErrors_t				ss_USSNotifyErrors_t;
typedef api_SsCode_t						ss_Code_t;
typedef api_SsOperation_t					ss_Operation_t;
typedef api_SsBasicServiceCodeCategory_t	ss_ServiceCodeCategory_t;
typedef api_SsCallWaitingParams_t			ss_CallWaitingParams_t;
typedef api_SsCallForwardingParams_t		ss_CallForwardingParams_t;
typedef api_SsCallBarringParams_t			ss_CallBarringParams_t;
typedef api_SsUSSDUserData_t				ss_USSDUserData_t;
typedef api_SsUSSDArgs_t					ss_USSDArgs_t;
typedef api_SsUSSDRes_t						ss_USSDRes_t;
typedef api_SsUSSDErrorSel_t				ss_USSDErrorSel_t;
typedef api_SsUSSDErrorPresent_t			ss_USSDErrorPresent_t;
typedef api_SsUSSDErrors_t					ss_USSDErrors_t;
typedef api_SsUSSDParamSet_t				ss_USSDParamSet_t;
typedef api_SsUSSDComponentType_t			ss_USSDComponentType_t;
typedef api_SsUSSDParams_t					ss_USSDParams_t;
typedef api_SsActionReqParams_t				ss_ActionReqParams_t;
typedef api_SsActionReq_t					ss_ActionReq_t;

//Possible values of the ServiceCode field
//Possible Teleservice Codes: 
//Use these if bBasicServiceCodeIsPresent == TRUE && TeleServiceCodeOrBearerServiceCode == TELE_SERVICE_CODE
#define SS_ALL_TELESERVICES					API_SS_ALL_TELESERVICES				
#define SS_ALL_SPEECH_TELESERVICES			API_SS_ALL_SPEECH_TELESERVICES		
#define SS_TELEPHONY						API_SS_TELEPHONY					
#define SS_EMERGENCY_CALLS					API_SS_EMERGENCY_CALLS				
#define SS_ALL_SMS							API_SS_ALL_SMS						
#define SS_SM_MT_PP							API_SS_SM_MT_PP						
#define SS_SM_MO_PP							API_SS_SM_MO_PP	
#define SS_ALL_FAX_SERVICES					API_SS_ALL_FAX_SERVICES
#define SS_FAX_GROUP3_ALTER_SPEECH			API_SS_FAX_GROUP3_ALTER_SPEECH
#define SS_AUTO_FAX_GROUP3					API_SS_AUTO_FAX_GROUP3
#define SS_FAX_GROUP4						API_SS_FAX_GROUP4
#define SS_ALL_DATA_TELESERVICES			API_SS_ALL_DATA_TELESERVICES
#define SS_ALL_TELESERVICES_XCPT_SMS		API_SS_ALL_TELESERVICES_XCPT_SMS	
#define SS_ALL_VOICE_GRP_CALL_SERVICES		API_SS_ALL_VOICE_GRP_CALL_SERVICES
#define SS_VOICE_GROUP_CALL					API_SS_VOICE_GROUP_CALL
#define SS_VOICE_BROADCAST_CALL				API_SS_VOICE_BROADCAST_CALL

#define SS_allPLMN_specificTS				API_SS_allPLMN_specificTS
#define SS_plmn_specificTS_1				API_SS_plmn_specificTS_1
#define SS_plmn_specificTS_2				API_SS_plmn_specificTS_2
#define SS_plmn_specificTS_3				API_SS_plmn_specificTS_3
#define SS_plmn_specificTS_4				API_SS_plmn_specificTS_4
#define SS_plmn_specificTS_5				API_SS_plmn_specificTS_5
#define SS_plmn_specificTS_6				API_SS_plmn_specificTS_6
#define SS_plmn_specificTS_7				API_SS_plmn_specificTS_7
#define SS_plmn_specificTS_8				API_SS_plmn_specificTS_8
#define SS_plmn_specificTS_9				API_SS_plmn_specificTS_9
#define SS_plmn_specificTS_A				API_SS_plmn_specificTS_A
#define SS_plmn_specificTS_B				API_SS_plmn_specificTS_B
#define SS_plmn_specificTS_C				API_SS_plmn_specificTS_C
#define SS_plmn_specificTS_D				API_SS_plmn_specificTS_D
#define SS_plmn_specificTS_E				API_SS_plmn_specificTS_E
#define SS_plmn_specificTS_F				API_SS_plmn_specificTS_F

//Possible Bearerservice Codes: 
//Use these if bBasicServiceCodeIsPresent == TRUE && TeleServiceCodeOrBearerServiceCode == BEARER_SERVICE_CODE
#define SS_AllBearerServices				API_SS_AllBearerServices
#define SS_AllDataCDAServices				API_SS_AllDataCDAServices
#define SS_DataCDA_300bps					API_SS_DataCDA_300bps
#define SS_DataCDA_1200bps					API_SS_DataCDA_1200bps
#define SS_DataCDA_1200_75bps				API_SS_DataCDA_1200_75bps
#define SS_DataCDA_2400bps					API_SS_DataCDA_2400bps
#define SS_DataCDA_4800bps					API_SS_DataCDA_4800bps
#define SS_DataCDA_9600bps					API_SS_DataCDA_9600bps
#define SS_general_dataCDA					API_SS_general_dataCDA

#define SS_AllDataCDSServices				API_SS_AllDataCDSServices
#define SS_DataCDS_1200bps					API_SS_DataCDS_1200bps
#define SS_DataCDS_2400bps					API_SS_DataCDS_2400bps
#define SS_DataCDS_4800bps					API_SS_DataCDS_4800bps
#define SS_DataCDS_9600bps					API_SS_DataCDS_9600bps
#define SS_general_dataCDS					API_SS_general_dataCDS

#define SS_AllPadAccessCAServices			API_SS_AllPadAccessCAServices
#define SS_PadAccessCA_300bps				API_SS_PadAccessCA_300bps
#define SS_PadAccessCA_1200bps				API_SS_PadAccessCA_1200bps
#define SS_PadAccessCA_1200_75bps			API_SS_PadAccessCA_1200_75bps
#define SS_PadAccessCA_2400bps				API_SS_PadAccessCA_2400bps
#define SS_PadAccessCA_4800bps				API_SS_PadAccessCA_4800bps
#define SS_PadAccessCA_9600bps				API_SS_PadAccessCA_9600bps
#define SS_general_padAccessCA				API_SS_general_padAccessCA

#define SS_ALLDataPDSServices				API_SS_ALLDataPDSServices
#define SS_DataPDS_2400bps					API_SS_DataPDS_2400bps
#define SS_DataPDS_4800bps					API_SS_DataPDS_4800bps
#define SS_DataPDS_9600bps					API_SS_DataPDS_9600bps
#define SS_general_dataPDS					API_SS_general_dataPDS

#define SS_allAlternateSpeech_DataCDA		API_SS_allAlternateSpeech_DataCDA
#define SS_allAlternateSpeech_DataCDS		API_SS_allAlternateSpeech_DataCDS
#define SS_allSpeechFollowedByDataCDA		API_SS_allSpeechFollowedByDataCDA
#define SS_allSpeechFollowedByDataCDS		API_SS_allSpeechFollowedByDataCDS
#define SS_allDataCircuitAsynchronous		API_SS_allDataCircuitAsynchronous
#define SS_allAsynchronousServices			API_SS_allAsynchronousServices
#define SS_allDataCircuitSynchronous		API_SS_allDataCircuitSynchronous
#define SS_allSynchronousServices			API_SS_allSynchronousServices

#define SS_allPLMN_specificBS				API_SS_allPLMN_specificBS
#define SS_plmn_specificBS_1				API_SS_plmn_specificBS_1
#define SS_plmn_specificBS_2				API_SS_plmn_specificBS_2
#define SS_plmn_specificBS_3				API_SS_plmn_specificBS_3
#define SS_plmn_specificBS_4				API_SS_plmn_specificBS_4
#define SS_plmn_specificBS_5				API_SS_plmn_specificBS_5
#define SS_plmn_specificBS_6				API_SS_plmn_specificBS_6
#define SS_plmn_specificBS_7				API_SS_plmn_specificBS_7
#define SS_plmn_specificBS_8				API_SS_plmn_specificBS_8
#define SS_plmn_specificBS_9				API_SS_plmn_specificBS_9
#define SS_plmn_specificBS_A				API_SS_plmn_specificBS_A
#define SS_plmn_specificBS_B				API_SS_plmn_specificBS_B
#define SS_plmn_specificBS_C				API_SS_plmn_specificBS_C
#define SS_plmn_specificBS_D				API_SS_plmn_specificBS_D
#define SS_plmn_specificBS_E				API_SS_plmn_specificBS_E
#define SS_plmn_specificBS_F				API_SS_plmn_specificBS_F

#define SS_RELEASE_COMPLETE_MESSAGE_TYPE	API_SS_RELEASE_COMPLETE_MESSAGE_TYPE
#define SS_FACILITY_MESSAGE_TYPE			API_SS_FACILITY_MESSAGE_TYPE 
#define SS_REGISTER_MESSAGE_TYPE			API_SS_REGISTER_MESSAGE_TYPE

typedef api_SsMessageType_t	ss_MessageType_t;


//================================================================================
//  Structure  : ss_TIInd_t
//--------------------------------------------------------------------------------
//  Direction  : API -> MMI
//  Scope      : Used to indicate the chosen TI for the outgoing call in process
//================================================================================
typedef api_SsTIInd_t ss_TIInd_t;

//================================================================================
//  Structure  : ss_ActionCnf_t
//--------------------------------------------------------------------------------
//  Direction  : SS -> API
//  Scope      : SS response to API for a given service request
//===============================================================================
typedef api_SsCliRestrictionOption_t			ss_CliRestrictionOption_t;
typedef api_SsBasicServiceGroupListElement_t	ss_BasicServiceListElement_t;
typedef api_SsCCBSFeatureListElement_t			ss_CCBSFeatureListElement_t;
typedef api_SsGenericServiceInfo_t				ss_GenericServiceInfo_t;
typedef api_SsBasicServiceGroupList_t			ss_BasicServiceGroupList_t;
typedef api_SsForwardingFeatureListElement_t	ss_ForwardingFeatureListElement_t;
typedef api_SsForwardingFeatureList_t			ss_ForwardingFeatureList_t;
typedef api_SsIntRsltChoice_t					ss_IntRsltChoice_t;
typedef	api_SsIntRsltChoicePresence_t			ss_IntRsltChoicePresence_t;
typedef api_SsIntRslt_t							ss_IntRslt_t;
typedef api_SsForwardingInfo_t					ss_ForwardingInfo_t;
typedef api_SsCallBarringFeatureList_t			ss_CallBarringFeatureList_t;
typedef api_SsCallBarringInfo_t					ss_CallBarringInfo_t;
typedef api_SsData_t							ss_SSData_t;
typedef api_SsUSSDNetworkComponentType_t		ss_USSDNetworkComponentType_t;
typedef api_SsUSSDNetworkResults_t				ss_USSDNetworkResults_t;
typedef api_SsUSSDInfo_t						ss_USSDInfo_t;
typedef api_SsRetRsltInfoPresence_t				ss_RetRsltInfoPresence_t;
typedef api_SsRetRslt_t							ss_RetRslt_t;
typedef api_SsReturnResultComponent_t			ss_ReturnResultComponent_t;

typedef api_SsActionCnf_t						ss_ActionCnf_t;
//If SSOperation == InterrogateSS, then the possible values of Status field are:
#define API_SS_OPERATIVE    ((u8)0x08)
#define API_SS_PROVISIONED  ((u8)0x04)
#define API_SS_REGISTERED	((u8)0x02)
#define API_SS_ACTIVE		((u8)0x01)
//otherwise, the Status field is not meaningful.

//================================================================================
//  Structure  : ss_ErrorInd_t
//--------------------------------------------------------------------------------
//  Direction  : SS -> API -> MMI
//  Scope      : indicate that a given request could not be executed
//================================================================================
typedef api_SsErrors_t			ss_Errors_t;
typedef api_SsErrorSource_t		ss_ErrorSourc_t;
typedef api_SsErrorInd_t		ss_ErrorInd_t;

//Local error definitions
#define SS_NOT_IMPLEMENTED			API_SS_NOT_IMPLEMENTED			//the SS has not yet been implemented
#define SS_OP_INVALID				API_SS_OP_INVALID				//the SS operation is not valid for the SS
#define SS_SSCODE_INVALID			API_SS_SSCODE_INVALID				//The SSID field is not a valid ID
#define SS_TI_ALLOC_FAILURE			API_SS_TI_ALLOC_FAILURE			//A TI could not be allocated for the transaction, try again
#define SS_CTX_MEM_ALLOC_FAILURE	API_SS_CTX_MEM_ALLOC_FAILURE	//Memory was not available to support the transaction, try again
#define SS_PARAM_INVALID			API_SS_PARAM_INVALID			//A parameter was invalid
#define SS_NO_PSWD_ERR				API_SS_NO_PSWD_ERR
#define SS_NO_NEW_PSWD_ERR			API_SS_NO_NEW_PSWD_ERR
#define SS_NO_NEW_PSWD_AGAIN_ERR	API_SS_NO_NEW_PSWD_AGAIN_ERR
#define SS_NEW_PSWD_MATCH_ERR		API_SS_NEW_PSWD_MATCH_ERR

#define SS_MM_ESTAB_ERR			API_SS_ESTAB_ERR
#define SS_TRANSACTION_TIMEOUT		API_SS_TRANSACTION_TIMEOUT
#define SS_MM_CONNECTION_RELEASED	API_SS_CONNECTION_LOST	//MM released the connection unexpectedly

#define SS_UNKNOWN_COMP_RECEIVED	API_SS_UNKNOWN_COMP_RECEIVED

#define SS_INVALID_USSD_MSG_TYPE	API_SS_INVALID_USSD_MSG_TYPE
#define SS_INVALID_USSD_COMPONENT	API_SS_INVALID_USSD_COMPONENT
#define SS_USSD_ARG_LEN_TOO_LONG	API_SS_USSD_ARG_LEN_TOO_LONG
#define SS_USSD_IA5_STRING_TOO_LONG	API_SS_USSD_IA5_STRING_TOO_LONG
#define SS_USSD_INVALID_TI			API_SS_USSD_INVALID_TI
#define SS_USSD_INVALID_PROB_CODE_TAG	API_SS_USSD_INVALID_PROB_CODE_TAG
#define SS_USSD_INVALID_ERR_CODE	API_SS_USSD_INVALID_ERR_CODE
#define SS_USSD_INVALID_STATE		API_SS_USSD_INVALID_STATE
#define SS_INVALID_USSD_OPERATION	API_SS_INVALID_USSD_OPERATION
#define SS_INVALID_USSD_CODE		API_SS_INVALID_USSD_CODE

#define SS_REJECT_COMPONENT			API_SS_REJECT_COMPONENT	//Network returned a Reject Component, see Code for Problem Code
#define SS_RETURN_ERROR				API_SS_RETURN_ERROR		//Network returned a ReturnError Component, See Code for Error Code
#define SS_CAUSE_IE					API_SS_CAUSE_IE			//Network returned a cause IE		

//Possible Error Codes if the Cause is Return Error
#define SS_UNKNOWN_SUBSCRIBER						API_SS_UNKNOWN_SUBSCRIBER
#define SS_ILLEGAL_SUBSCRIBER						API_SS_ILLEGAL_SUBSCRIBER
#define SS_BEARER_SERVICE_NOT_PROVISIONED			API_SS_BEARER_SERVICE_NOT_PROVISIONED
#define SS_TELE_SERVICE_NOT_PROVISIONED				API_SS_TELE_SERVICE_NOT_PROVISIONED
#define SS_ILLEGAL_EQUIPMENT						API_SS_ILLEGAL_EQUIPMENT
#define SS_CALL_BARRED								API_SS_CALL_BARRED
#define SS_ILLEGAL_SS_OPERATION						API_SS_ILLEGAL_SS_OPERATION
#define SS_ERROR_STATUS								API_SS_ERROR_STATUS
#define SS_SS_NOT_AVAILABLE							API_SS_SS_NOT_AVAILABLE
#define SS_SUBSCRIPTION_VIOLATION					API_SS_SUBSCRIPTION_VIOLATION
#define SS_INCOMPATIBILITY							API_SS_INCOMPATIBILITY
#define SS_FACILITY_NOT_SUPPORTED					API_SS_FACILITY_NOT_SUPPORTED
#define SS_ABSENT_SUBSCRIBER						API_SS_ABSENT_SUBSCRIBER
#define SS_SHORT_TERMINAL_DENIAL					API_SS_SHORT_TERMINAL_DENIAL
#define SS_LONG_TERM_DENIAL							API_SS_LONG_TERM_DENIAL
#define SS_SYSTEM_FAILURE							API_SS_SYSTEM_FAILURE
#define SS_DATA_MISSING								API_SS_DATA_MISSING
#define SS_UNEXPECTED_DATA_VALUE					API_SS_UNEXPECTED_DATA_VALUE
#define SS_PW_REGISTRATION_FAILURE					API_SS_PW_REGISTRATION_FAILURE
#define SS_NEGATIVE_PW_CHECK						API_SS_NEGATIVE_PW_CHECK
#define SS_NUMBER_OF_PW_ATTEMTS_VIOLATION			API_SS_NUMBER_OF_PW_ATTEMTS_VIOLATION
#define SS_POSITION_METHOD_FAILURE					API_SS_POSITION_METHOD_FAILURE
#define SS_UNKNOWN_ALPHABET							API_SS_UNKNOWN_ALPHABET
#define SS_USSD_BUSY								API_SS_USSD_BUSY
#define SS_NBR_SB_EXCEEDED							API_SS_NBR_SB_EXCEEDED
#define SS_REJECTED_BY_USER							API_SS_REJECTED_BY_USER
#define SS_REJECTED_BY_NETWORK						API_SS_REJECTED_BY_NETWORK
#define SS_DEFLECTION_TO_SERVED_SUBSCRIBER			API_SS_DEFLECTION_TO_SERVED_SUBSCRIBER
#define SS_SPECIAL_SERVICE_CODE						API_SS_SPECIAL_SERVICE_CODE
#define SS_INVALID_DEFLECTED_TO_NUMBER				API_SS_INVALID_DEFLECTED_TO_NUMBER
#define SS_MAX_NUMER_OF_MPTY_PARTICIPANTS_EXCEEDED	API_SS_MAX_NUMER_OF_MPTY_PARTICIPANTS_EXCEEDED
#define SS_RESOURCES_NOT_AVAILABLE					API_SS_RESOURCES_NOT_AVAILABLE

//Possible Problem Codes Tags if the Cause is Reject Component
#define SS_GENERAL_PROBLEM_TAG				API_SS_GENERAL_PROBLEM_TAG
#define SS_INVOKE_PROBLEM_TAG				API_SS_INVOKE_PROBLEM_TAG
#define SS_RETURN_RESULT_PROBLEM_TAG		API_SS_RETURN_RESULT_PROBLEM_TAG
#define SS_RETURN_ERROR_PROBLEM_TAG			API_SS_RETURN_ERROR_PROBLEM_TAG

//Possible Problem Codes if the Cause is Reject Component and ProblemCodeTag is API_SS_GENERAL_PROBLEM_TAG
#define SS_GP_UNRECOGNIZED_COMPONENT			API_SS_GP_UNRECOGNIZED_COMPONENT
#define SS_GP_MISTYPED_COMPONENT				API_SS_GP_MISTYPED_COMPONENT
#define SS_GP_BADLY_STRUCTURED_COMPONENT		API_SS_GP_BADLY_STRUCTURED_COMPONENT

//Possible Problem Codes if the Cause is Reject Component and ProblemCodeTag is API_SS_INVOKE_PROBLEM_TAG
#define SS_IP_DUPLICATE_INVOKE_ID				API_SS_IP_DUPLICATE_INVOKE_ID
#define SS_IP_UNRECOGNIZED_OPERATION			API_SS_IP_UNRECOGNIZED_OPERATION
#define SS_IP_MISTYPED_PARAMETER				API_SS_IP_MISTYPED_PARAMETER
#define SS_IP_RESOURCE_LIMITATION				API_SS_IP_RESOURCE_LIMITATION
#define SS_IP_INITIATING_RELEASE				API_SS_IP_INITIATING_RELEASE
#define SS_IP_UNRECOGNIZED_LINKED_ID			API_SS_IP_UNRECOGNIZED_LINKED_ID
#define SS_IP_LINKED_RESPONSE_UNEXPECTED		API_SS_IP_LINKED_RESPONSE_UNEXPECTED
#define SS_IP_UNEXPECTED_LINKED_OPERATION		API_SS_IP_UNEXPECTED_LINKED_OPERATION

//Possible Problem Codes if the Cause is Reject Component and ProblemCodeTag is API_SS_RETURN_RESULT_PROBLEM_TAG
#define SS_RR_UNRECOGNIZED_INVOKE_ID			API_SS_RR_UNRECOGNIZED_INVOKE_ID
#define SS_RR_RETURN_RESULT_UNEXPECTED			API_SS_RR_RETURN_RESULT_UNEXPECTED
#define SS_RR_MISTYPED_PARAMETER				API_SS_RR_MISTYPED_PARAMETER

//Possible Problem Codes if the Cause is Reject Component and ProblemCodeTag is API_SS_RETURN_ERROR_PROBLEM_TAG
#define SS_RE_UNRECOGNIZED_INVOKE_ID			API_SS_RE_UNRECOGNIZED_INVOKE_ID
#define SS_RE_RETURN_ERROR_UNEXPECTED			API_SS_RE_RETURN_ERROR_UNEXPECTED
#define SS_RE_UNRECOGNIZED_ERROR				API_SS_RE_UNRECOGNIZED_ERROR
#define SS_RE_UNEXPECTED_ERROR					API_SS_RE_UNEXPECTED_ERROR
#define SS_RE_MISTYPED_PARAMETER				API_SS_RE_MISTYPED_PARAMETER

//Cause IE Definitions
//Cause IE
//Coding Standard Field for Cause IE, Octet 3
//Per 24.008, The mobile station need not (and does not) support any other standard than GSM_PLMNS
#define SS_ITU_T_REC_Q_931						API_SS_ITU_T_REC_Q_931
#define SS_RESERVED_INTERNATIONAL_STANDARD		API_SS_RESERVED_INTERNATIONAL_STANDARD
#define SS_NATIONAL_STANDARD					API_SS_NATIONAL_STANDARD
#define SS_GSM_PLMNS							API_SS_GSM_PLMNS

//Location Field for Cause IE, Octet 3
#define SS_User									API_SS_User	
#define SS_PRIVATE_NETWORK_SERVING_LOCAL_USER	API_SS_PRIVATE_NETWORK_SERVING_LOCAL_USER
#define SS_PUBLIC_NETWORK_SERVING_LOCAL_USER	API_SS_PUBLIC_NETWORK_SERVING_LOCAL_USER
#define SS_TRANSIT_NETWORK						API_SS_TRANSIT_NETWORK
#define SS_PUBLIC_NETWORK_SERVING_REMOTE_USER	API_SS_PUBLIC_NETWORK_SERVING_REMOTE_USER
#define SS_PRIVATE_NETWORK_SERVING_REMOTE_USER	API_SS_PRIVATE_NETWORK_SERVING_REMOTE_USER
#define SS_INTERNATIONAL_NETWORK				API_SS_INTERNATIONAL_NETWORK
#define SS_NETWORK_BEYOND_INTERNETWORKING_POINT	API_SS_NETWORK_BEYOND_INTERNETWORKING_POINT

//Cause field for all non GSM PLMNS encoded standards
#define SS_CAUSE_IE_INTERNETWORKING_UNSPECIFIED	API_SS_CAUSE_IE_INTERNETWORKING_UNSPECIFIED

//================================================================================
//  Structure  : ss_ActionInd_t
//--------------------------------------------------------------------------------
//  Direction  : SS -> API
//  Scope      : SS response to API for a nwk initiate USSD
//  author      : Wangf 20070529
//===============================================================================
typedef api_SsActionCnf_t						ss_ActionInd_t; 

#endif
