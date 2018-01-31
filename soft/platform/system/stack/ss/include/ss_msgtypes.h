////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2006, Coolsand Technologies, Inc.
//                       All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: ss_msgtypes.h
// 
// DESCRIPTION:
//   
// 
// REVISION HISTORY:
//  NAME  | DATE            REMAKS
//  DMX   | 20070419 | Add SS_STATUS_MASK define
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _SS_MSGTYPES_H_
#define _SS_MSGTYPES_H_

#include "itf_ss.h"

//Universal Tags defined by ASN.1 and used by SS 
#define SS_ANS1_INTEGER_TAG             ((u8)0x02)
#define SS_ANS1_SEQUENCE_TAG            ((u8)0x10)
#define SS_ASN1_OCTET_STRING_TAG        ((u8)0x04)
#define SS_ASN1_ENUM_TAG                ((u8)0x0A)
#define SS_ASN1_NUMERIC_STRING_TAG      ((u8)18)
#define SS_ASN1_IA5_STRING_TAG          ((u8)22)

#define SS_LENGTH_OCTET                 ((u8)0x80)  //If the length field of a TLV == 0x80, the end of the TLV is 0x00 0x00
#define SS_END_OF_CONTENTS_OCTET        ((u8)0x00)  //2 in a row signify the end of a TLV
#define SEQUENCE_TAG_C                  ((u8)0x30)  //Constructed, length will be set to 0x80

//Macros and definitions to extract Implicit Tags
#define SS_TAG_MASK                     ((u8)0x1F)
#define SS_TAG_IMPLICIT_MASK            ((u8)0x9F)
#define SS_TAG_VALUE(_VALUE)            (_VALUE & SS_TAG_MASK)
#define SS_TAG_IMPLICIT_VALUE(_VALUE)   (_VALUE & SS_TAG_IMPLICIT_MASK)

//Protocol discriminator and transaction ID
#define SS_NON_CALL_RELATED_PD          ((u8)0x0B) //LS nibble of byte 0

//Possible Value of TI Flag for received messages
#define SS_MS_SOURCED_TRANSACTION       ((u8)0x80)

//Possible IEI(s) Tags
#define SS_CAUSE_IEI_TAG                ((u8)0x08)
#define SS_FACILITY_IEI_TAG             ((u8)0x1C)
#define SS_VERSION_IEI_TAG              ((u8)0x7F)

// ss status value add by dingmx 20070419
#define SS_STATUS_PROVISION_MASK        ((u8)0x04)
#define SS_STATUS_REGISTE_MASK          ((u8)0x02)
#define SS_STATUS_ACTIVE_MASK           ((u8)0x01)

typedef enum
{
  CauseIEI_Tag = SS_CAUSE_IEI_TAG,
  FacilityIEI_Tag = SS_FACILITY_IEI_TAG,
  SSVersionIEI_Tag = SS_VERSION_IEI_TAG,
  ss_InvalidSSIE
} ss_IEITag_t;


//Facility IEI Component Type Tags
#define SS_INVOKE_COMPONENT_TAG             ((u8)0xA1)
#define SS_RETURN_RESULT_COMPONENT_TAG      ((u8)0xA2)
#define SS_RETURN_ERROR_COMPONENT_TAG       ((u8)0xA3)
#define SS_REJECT_COMPONENT_TAG             ((u8)0xA4)
#define SS_COMPONENT_ID_LENGTH          ((u8)0x01)
typedef enum
{
  InvokeComponent_Tag       = SS_INVOKE_COMPONENT_TAG,
  ReturnResultComponent_Tag = SS_RETURN_RESULT_COMPONENT_TAG,
  ReturnErrorComponent_Tag  = SS_RETURN_ERROR_COMPONENT_TAG,
  RejectComponent_Tag       = SS_REJECT_COMPONENT_TAG,
  ss_InvalidSSComponent
} ss_ComponentTag_t;

//Component ID Tag
#define SS_INVOKE_ID_TAG        ((u8)0x02)
#define SS_INVOKE_ID_LENGTH     ((u8)0x01)
typedef enum
{
  InvokeIDTag = SS_INVOKE_ID_TAG
} ss_InvokeIDTag_t;

//Linked ID Tag
#define SS_LINKED_ID_TAG        ((u8)0x80)
#define SS_LINKED_ID_LENGTH     ((u8)0x01)
typedef enum
{
  LinkedIDTag = SS_LINKED_ID_TAG
} ss_LinkedIDTag_t;

//Operation Code Tag
#define SS_OPERATION_CODE_TAG      ((u8)0x02)
typedef enum
{
  ss_OperationTag = SS_OPERATION_CODE_TAG
} ss_OperationTag_t;
#define SS_OPERATION_CODE_LENGTH      ((u8)0x01)

//Sequence (params) tag
#define SS_PARAMS_TAG        ((u8)0x30)
typedef enum
{
 ss_ParamsTag = SS_PARAMS_TAG
} ss_ParamsTag_t;

//Return Result Parameter Choices
#define SS_FORWARDING_INFO_TAG      ((u8)0x00)        
#define SS_CALL_BARRING_INFO_TAG    ((u8)0x01)
#define SS_DATA_TAG                 ((u8)0x03)
typedef enum
{
  ss_ForwardingInfo_Tag   = SS_FORWARDING_INFO_TAG,
  ss_CallBarringInfo_Tag  = SS_CALL_BARRING_INFO_TAG,
  ss_Data_Tag             = SS_DATA_TAG
} ss_ReturnResultParams_t;

//SS Code Tags
#define SS_CODE_TAG          ((u8)0x04)      //Primitive
typedef enum
{
  ss_Code_Tag = SS_CODE_TAG
} ss_Code_Tag_t;

//Tags for SSOperations Parameters
#define SS_FORWARDED_TO_NUMBER_TAG      ((u8)0x84)
#define SS_FORWARDED_TO_SUBADDRESS_TAG  ((u8)0x86)
#define SS_NO_REPLY_CONDITION_TIME_TAG  ((u8)0x85)
#define SS_NO_REPLY_CONDITION_TIME_LEN  ((u8)1)

//Sequence Tags
#define SS_SEQUENCE_TAG          ((u8)0x30)
typedef enum
{
  ss_Sequence_Tag = SS_SEQUENCE_TAG
} ss_Sequence_Tag_t;

//Interrogate - Return Result component defines
#define SS_STATUS_TAG_PI                         ((u8)0x80)  //Primitive, Implicit
#define SS_STATUS_TAG_CI                         ((u8)0xA0)  //Constructed, Implicit
#define SS_STATUS_LEN                            ((u8)0x01)
#define SS_BASIC_SERVICE_LIST_TAG_CI             ((u8)0xA2) //Constructed, Implicit
#define SS_BASIC_SERVICE_LIST_TAG_PI             ((u8)0x82) //Primitive, Implicit
#define SS_GENERIC_SERVICES_INFO_TAG_CI          ((u8)0xA4) //Constructed, Implicit
#define SS_FORWARDING_FEATURE_LIST_TAG_CI        ((u8)0xA3) //Constructed, implicit
#define SS_FORWARDING_FEATURE_LIST_STATUS_TAG_PI ((u8)0x84) //Constructed, implicit
#define SS_FORWARDING_FEATURE_LIST_FTN_TAG_PI    ((u8)0x85) //Constructed, implicit

//Register - Return Result Component definitions
#define SS_FORWARDING_INFO_TAG_CI            ((u8)0xA0) //Constructed, Implicit
#define SS_FORWARDING_INFO_TAG_PI            ((u8)0x80) //Primitive Implicit
#define SS_FORWARDING_INFO_STATUS_TAG_CI     ((u8)0xA4) //Constructed, Implicit
#define SS_FORWARDING_INFO_STATUS_TAG_PI     ((u8)0x84) //Primitive, Implicit
#define SS_FORWARDING_INFO_STATUS_FTN_TAG_PI ((u8)0x85) //Primitive, Implicit 
#define SS_FORWARDING_INFO_NRCT_TAG_PI       ((u8)0x87) //Primitive, Implicit

//Return Reject Component Defines
#define PROBLEM_CODE_LENGTH    ((u8)0x01)

//Return Error Component Defines
#define SS_ERROR_CODE_TAG       ((u8)0x02)
#define SS_ERROR_CODE_LENGTH    ((u8)0x01)

//Length of SS version IE
#define  SS_VERSION_IE_LEN      ((u8)1)
#define SS_VERSION              ((u8)0)

//Password enumeration for getPassword and registerPassword Operations
#define SS_ENTER_PW             ((u8)0)
#define SS_ENTER_NEW_PW         ((u8)1)
#define SS_ENTER_NEW_PW_AGAIN   ((u8)2) 
typedef enum
{
  ss_enterPW  = SS_ENTER_PW,      //to check password, 
                    //network invokes getpassword with paramter == ss_enterPW, 
                    //MS returns current password as result
  ss_enterNewPW = SS_ENTER_NEW_PW,  //network invokes enter new password
  ss_enterNewPWAgain = SS_ENTER_NEW_PW_AGAIN
} ss_guidanceInfo_t;

typedef struct
{
  u8  transactionID;
  u8  invokeID;
} ss_RxMsgHdr_t;

typedef struct
{
  u8 errorCodeTag;
  u8 errorCodeLength;
  u8 errorCode;
} ss_ReturnErrorComponent_t;

typedef struct
{
  u8 problemCodeTag;
  u8 problemCodeLength;
  u8 problemCode;
} ss_RejectComponent_t;

typedef struct
{
  u8 CodingStandard;
  u8 Location;
  u8 Cause;
} ss_CauseIE_t;

typedef union
{
  ss_RejectComponent_t      reject;
  ss_ReturnErrorComponent_t    returnError;
  ss_ReturnResultComponent_t    returnResult;
} ss_RxComponent_t;


typedef enum
{
  ss_Invoke_GetPassword_EnterPw_Present = SS_ENTER_PW,
  ss_Invoke_GetPassword_EnterNewPw_Present = SS_ENTER_NEW_PW,
  ss_Invoke_GetPassword_EnterNewPwAgain_Present = SS_ENTER_NEW_PW_AGAIN
} ss_CallBarringPasswordRI_t;

typedef union
{
  ss_CallBarringPasswordRI_t ss_CallBarringPasswordRI;
} ss_OperationInfo_t;


typedef enum
{
  ss_RejectComponentPresent = SS_REJECT_COMPONENT_TAG,
  ss_ReturnErrorComponentPresent = SS_RETURN_ERROR_COMPONENT_TAG,
  ss_ReturnResultComponentPresent = SS_RETURN_RESULT_COMPONENT_TAG,
  ss_NoRxComponentPresent = 0xFF,
} ss_RxComponentPresence_t;

typedef struct
{
  ss_MessageType_t        ss_RxMessageType;
  ss_IEITag_t            ss_RxIEIType;
  ss_ComponentTag_t        ss_RxComponentType;
  ss_Operation_t          ss_OperationType;
  ss_OperationInfo_t        ss_OperationInfo;
  ss_Code_t            ss_Code;
  u8                ss_InvokeID;
} ss_RxMessageRoutingInfo_t;

typedef struct
{
  ss_RxMsgHdr_t            MsgHdr;
  ss_RxComponent_t          Component;
  ss_CauseIE_t            causeIE;
} ss_RxMsgInfo_t;

#endif //end of file
