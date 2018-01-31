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

#ifdef CFW_GPRS_SUPPORT
#ifndef _CFW_GPRS_H_
#define _CFW_GPRS_H_

#include "cfw_gprs_data.h"
#include "cfw_nw_data.h"
//#define CFW_GPRS_TRACE          CFW_GPRS_TS_ID


//add for Gprs flow control by wuys 2009-01-14
#define FLOW_HOLD    (1<<0)
#define FLOW_RELEASED    (1<<1)


typedef enum _CFW_GPRS_SMCAUSE
{
    CFW_GPRS_SM_LLC_OR_SNDCP_FAILURE = 0x19,
    CFW_GPRS_SM_INSUFFICIENT_RESOURCES,
    CFW_GPRS_SM_MISSING_OR_UNKNOWN_APN,
    CFW_GPRS_SM_UNKNOWN_PDP_ADDRESS_OR_PDP_TYPE,
    CFW_GPRS_SM_USER_AUTHENTICATION_FAILED,
    CFW_GPRS_SM_ACTIVATION_REJECTED_BY_GGSN,
    CFW_GPRS_SM_ACTIVATION_REJECTED_UNSPECIFIED,
    CFW_GPRS_SM_SERVICE_OPTION_NOT_SUPPORTED,
    CFW_GPRS_SM_REQUESTED_SERVICE_OPTION_NOT_SUBSCRIBED,
    CFW_GPRS_SM_SERVICE_OPTION_TEMPORARILY_OUT_OF_ORDER,
    CFW_GPRS_SM_NSAPI_ALREADY_USED,
    CFW_GPRS_SM_REGULAR_DEACTIVATION,
    CFW_GPRS_SM_QOS_NOT_ACCEPTED,
    CFW_GPRS_SM_NETWORK_FAILURE,
    CFW_GPRS_SM_REACTIVATION_REQUIRED,
    CFW_GPRS_SM_FEATURE_NOT_SUPPORTED,
    CFW_GPRS_SM_SEMANTIC_ERROR_IN_THE_TFT_OPERATION,
    CFW_GPRS_SM_SYNTACTICAL_ERROR_IN_THE_TFT_OPERATION,
    CFW_GPRS_SM_UNKNOWN_PDP_CONTEXT,
    CFW_GPRS_SM_PDP_CONTEXT_WITHOUT_TFT_ALREADY_ACTIVATED,
    CFW_GPRS_SM_SEMANTIC_ERRORS_IN_PACKET_FILTER,
    CFW_GPRS_SM_SYNTACTICAL_ERRORS_IN_PACKET_FILTER,
    CFW_GPRS_SM_INVALID_TI_VALUE = 0x51,
    CFW_GPRS_SM_SEMANTICALLY_INCORRECT_MESSAGE = 0x5F,
    CFW_GPRS_SM_INVALID_MANDATORY_INFORMATION,
    CFW_GPRS_SM_MESSAGE_TYPE_NOT_IMPLEMENTED,
    CFW_GPRS_SM_MESSAGE_TYPE_NOT_COMPATIBLE,
    CFW_GPRS_SM_IE_NOT_IMPLEMENTED,
    CFW_GPRS_SM_CONDITIONAL_IE_ERROR,
    CFW_GPRS_SM_MESSAGE_NOT_COMPATIBLE,
    CFW_GPRS_SM_PROTOCOL_ERROR_UNSPECIFIED = 0x6F
} CFW_GPRS_SMCAUSE;

#if 0

//event
#define EV_CFW_GPRS_BASE                    0X100
// the response of the function CFW_GprsAtt
// UTI: 0
// parameter1: 0
// parameter2: 0
// type: 0 or 1 to distinguish the attach or detach
// error code: ERR_CFW_GPRS_GPRS_SERVICES_NOT_ALLOWED: the gprs service not allowed
#define EV_CFW_GPRS_ATT_NOTIFY              EV_CFW_GPRS_BASE + 0


// the response of the function CFW_GprsAct
// UTI: uti
// parameter1: 0
// parameter2: 0
// type: 0 or 1 to distinguish the activate or deactivate
#define EV_CFW_GPRS_ACT_NOTIFY              EV_CFW_GPRS_BASE + 1

// the response of the function CFW_GprsManualAcc
// UTI: uti
// parameter1: 0
// parameter2: 0
// type: 0
#define EV_CFW_GPRS_CXT_ACTIVE_IND_NOTIFY   EV_CFW_GPRS_BASE + 2

// the response of the function CFW_GprsCtxModify
// UTI: uti
// parameter1: 0
// parameter2: 0
// type: 0
//
#define EV_CFW_GPRS_MOD_NOTIFY              EV_CFW_GPRS_BASE + 3
// Ind
// the indication of the network wishes to activate a pdp context.
// UTI: 0
// parameter1: point to the struct CFW_GPRS_PDPCONT_INFO
// parameter2: point to the struct CFW_GPRS_QOS
// type: 0
#define EV_CFW_GPRS_CXT_ACTIVE_IND          EV_CFW_GPRS_BASE + 5
// add remark // should add uti and cid......

//Ind
// the indication of the network wishes to deactive a pdp context or an error has occured.
// UTI: uti
// parameter1: nCid
// parameter2: error Cause
// type: 0
#define EV_CFW_GPRS_CXT_DEACTIVE_IND        EV_CFW_GPRS_BASE + 6

// the indication of the network wishes to change the Qos for the activate PDP context.
// UTI: 0
// parameter1: point to the struct CFW_GPRS_CTX_MODIFY
// parameter2: 0
// type: 0
#define EV_CFW_GPRS_MOD_IND                 EV_CFW_GPRS_BASE + 9

// Ind
// move to Net
// indicate to the application when the status of Gprs registration is changed.
// UTI: 0
// parameter1: point to the struct CFW_NW_STATUS_INFO
// parameter2: 0
// type : 0
#define EV_CFW_GPRS_STATUS_IND              EV_CFW_GPRS_BASE + 10
//
// indicate the MT the data from SNDCP is coming.
// UTI: 0
// parameter1: point to the struct CFW_GPRS_DATA
// parameter2: the length of the struct.
// type: 0
#define EV_CFW_GPRS_DATA_IND                EV_CFW_GPRS_BASE + 11
#endif

#endif //
#endif

