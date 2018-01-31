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



#ifndef __AT_ERRCODE_H__
#define __AT_ERRCODE_H__

#define ERR_AT_OK                                   0xF0F0F0
#define ERR_AT_UNKNOWN                              0xFFFFFF

// CME error code define start
#define ERR_AT_CME_PHONE_FAILURE                    0
#define ERR_AT_CME_NO_CONNECT_PHONE                 1
#define ERR_AT_CME_PHONE_ADAPTER_LINK_RESERVED      2
#define ERR_AT_CME_OPERATION_NOT_ALLOWED            3
#define ERR_AT_CME_OPERATION_NOT_SUPPORTED          4
#define ERR_AT_CME_PHSIM_PIN_REQUIRED               5
#define ERR_AT_CME_PHFSIM_PIN_REQUIRED              6
#define ERR_AT_CME_PHFSIM_PUK_REQUIRED              7
#define ERR_AT_CME_SIM_NOT_INSERTED                 10
#define ERR_AT_CME_SIM_PIN_REQUIRED                 11
#define ERR_AT_CME_SIM_PUK_REQUIRED                 12
#define ERR_AT_CME_SIM_FAILURE                      13
#define ERR_AT_CME_SIM_BUSY                         14
#define ERR_AT_CME_SIM_WRONG                        15
#define ERR_AT_CME_INCORRECT_PASSWORD               16
#define ERR_AT_CME_SIM_PIN2_REQUIRED                17
#define ERR_AT_CME_SIM_PUK2_REQUIRED                18
#define ERR_AT_CME_MEMORY_FULL                      20
#define ERR_AT_CME_INVALID_INDEX                    21
#define ERR_AT_CME_NOT_FOUND                        22
#define ERR_AT_CME_MEMORY_FAILURE                   23
#define ERR_AT_CME_TEXT_LONG                        24
#define ERR_AT_CME_INVALID_CHAR_INTEXT              25
#define ERR_AT_CME_DAIL_STR_LONG                    26
#define ERR_AT_CME_INVALID_CHAR_INDIAL              27
#define ERR_AT_CME_NO_NET_SERVICE                   30
#define ERR_AT_CME_NETWORK_TIMOUT                   31
#define ERR_AT_CME_NOT_ALLOW_EMERGENCY              32
#define ERR_AT_CME_NET_PER_PIN_REQUIRED             40
#define ERR_AT_CME_NET_PER_PUK_REQUIRED             41
#define ERR_AT_CME_NET_SUB_PER_PIN_REQ              42
#define ERR_AT_CME_NET_SUB_PER_PUK_REQ              43
#define ERR_AT_CME_SERVICE_PROV_PER_PIN_REQ         44
#define ERR_AT_CME_SERVICE_PROV_PER_PUK_REQ         45
#define ERR_AT_CME_CORPORATE_PER_PIN_REQ            46
#define ERR_AT_CME_CORPORATE_PER_PUK_REQ            47
#define ERR_AT_CME_PHSIM_PBK_REQUIRED               48

// extern errocode
#define ERR_AT_CME_EXE_NOT_SURPORT                  49
#define ERR_AT_CME_EXE_FAIL                         50
#define ERR_AT_CME_NO_MEMORY                        51
#define ERR_AT_CME_OPTION_NOT_SURPORT               52
#define ERR_AT_CME_PARAM_INVALID                    53
#define ERR_AT_CME_EXT_REG_NOT_EXIT                 54
#define ERR_AT_CME_EXT_SMS_NOT_EXIT                 55
#define ERR_AT_CME_EXT_PBK_NOT_EXIT                 56
#define ERR_AT_CME_EXT_FFS_NOT_EXIT                 57
#define ERR_AT_CME_INVALID_COMMAND_LINE             58
#define ERR_AT_CME_ITF_DIFFERENT                    59
#define ERR_AT_CME_BURN_FLASH_FAIL                  60
#define ERR_AT_CME_TFLASH_NOT_EXIST                 61

//
// GPRS-related errors.
// Errors related to a failure to perform an Attach.(Values in parentheses are GSM 04.08 cause codes.)
//
#define ERR_AT_CME_GPRS_ILLEGAL_MS_3                103 // Illegal MS (#3)
#define ERR_AT_CME_GPRS_ILLEGAL_MS_6                106 // Illegal ME (#6)
#define ERR_AT_CME_GPRS_SVR_NOT_ALLOWED             107 // GPRS services not allowed (#7)
#define ERR_AT_CME_GPRS_PLMN_NOT_ALLOWED            111 // PLMN not allowed (#11)
#define ERR_AT_CME_GPRS_LOCATION_AREA_NOT_ALLOWED   112 // Location area not allowed (#12)
#define ERR_AT_CME_GPRS_ROAMING_NOT_ALLOWED         113 // Roaming not allowed in this location area (#13)

//
// Errors related to a failure to Activate a Context
//
#define ERR_AT_CME_GPRS_OPTION_NOT_SUPPORTED        132 // service option not supported (#32)
#define ERR_AT_CME_GPRS_OPTION_NOT_SUBSCRIBED       133 // requested service option not subscribed (#33)
#define ERR_AT_CME_GPRS_OPTION_TEMP_ORDER_OUT       134 // service option temporarily out of order (#34)
#define ERR_AT_CME_GPRS_PDP_AUTHENTICATION_FAILURE  149 // PDP authentication failure

//
// Other GPRS errors
// Other values in the range 101 - 150 are reserved for use by GPRS
//
#define ERR_AT_CME_GPRS_INVALID_MOBILE_CLASS        150 // invalid mobile class
#define ERR_AT_CME_GPRS_UNSPECIFIED_GPRS_ERROR      148 // unspecified GPRS error

//
// GPRS-related errors end.
//

// AT_20071024_CAOW_B
// updated error code
#define ERR_AT_CME_SIM_VERIFY_FAIL                      264
#define ERR_AT_CME_SIM_UNBLOCK_FAIL                     265
#define ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED          266
#define ERR_AT_CME_SIM_UNBLOCK_FAIL_NO_LEFT             267
#define ERR_AT_CME_SIM_VERIFY_FAIL_NO_LEFT              268
#define ERR_AT_CME_SIM_INVALID_PARAMETER                269
#define ERR_AT_CME_SIM_UNKNOW_COMMAND                   270
#define ERR_AT_CME_SIM_WRONG_CLASS                      271
#define ERR_AT_CME_SIM_TECHNICAL_PROBLEM                272
#define ERR_AT_CME_SIM_CHV_NEED_UNBLOCK                 273
#define ERR_AT_CME_SIM_NOEF_SELECTED                        274
#define ERR_AT_CME_SIM_FILE_UNMATCH_COMMAND         275
#define ERR_AT_CME_SIM_CONTRADICTION_CHV                276
#define ERR_AT_CME_SIM_CONTRADICTION_INVALIDATION       277
#define ERR_AT_CME_SIM_MAXVALUE_REACHED                 278
#define ERR_AT_CME_SIM_PATTERN_NOT_FOUND                279
#define ERR_AT_CME_SIM_FILEID_NOT_FOUND                 280
#define ERR_AT_CME_SIM_STK_BUSY                         281
#define ERR_AT_CME_SIM_UNKNOW                           282
#define ERR_AT_CME_SIM_PROFILE_ERROR                        283
// AT_20071024_CAOW_E

// CME define end

// CMS error code define start
#define ERR_AT_CMS_UNASSIGNED_NUM       1
#define ERR_AT_CMS_OPER_DETERM_BARR     8
#define ERR_AT_CMS_CALL_BARRED          10
#define ERR_AT_CMS_SM_TRANS_REJE        21
#define ERR_AT_CMS_DEST_OOS             27
#define ERR_AT_CMS_UNINDENT_SUB         28
#define ERR_AT_CMS_FACILIT_REJE         29
#define ERR_AT_CMS_UNKONWN_SUB          30
#define ERR_AT_CMS_NW_OOO               38
#define ERR_AT_CMS_TMEP_FAIL            41
#define ERR_AT_CMS_CONGESTION           42
#define ERR_AT_CMS_RES_UNAVAILABLE      47
#define ERR_AT_CMS_REQ_FAC_NOT_SUB      50
#define ERR_AT_CMS_RFQ_FAC_NOT_IMP      69
#define ERR_AT_CMS_INVALID_SM_TRV       81
#define ERR_AT_CMS_INVALID_MSG          95
#define ERR_AT_CMS_INVALID_MAND_INFO    96
#define ERR_AT_CMS_MSG_TYPE_ERROR       97
#define ERR_AT_CMS_MSG_NOT_COMP         98
#define ERR_AT_CMS_INFO_ELEMENT_ERROR   99
#define ERR_AT_CMS_PROT_ERROR           111
#define ERR_AT_CMS_IW_UNSPEC            127
#define ERR_AT_CMS_TEL_IW_NOT_SUPP      128
#define ERR_AT_CMS_SMS_TYPE0_NOT_SUPP   129
#define ERR_AT_CMS_CANNOT_REP_SMS       130
#define ERR_AT_CMS_UNSPEC_TP_ERROR      143
#define ERR_AT_CMS_DCS_NOT_SUPP         144
#define ERR_AT_CMS_MSG_CLASS_NOT_SUPP   145
#define ERR_AT_CMS_UNSPEC_TD_ERROR      159
#define ERR_AT_CMS_CMD_CANNOT_ACT       160
#define ERR_AT_CMS_CMD_UNSUPP           161
#define ERR_AT_CMS_UNSPEC_TC_ERROR      175
#define ERR_AT_CMS_TPDU_NOT_SUPP        176
#define ERR_AT_CMS_SC_BUSY              192
#define ERR_AT_CMS_NO_SC_SUB            193
#define ERR_AT_CMS_SC_SYS_FAIL          194
#define ERR_AT_CMS_INVALID_SME_ADDR     195
#define ERR_AT_CMS_DEST_SME_BARR        196
#define ERR_AT_CMS_SM_RD_SM             197
#define ERR_AT_CMS_TP_VPF_NOT_SUPP      198
#define ERR_AT_CMS_TP_VP_NOT_SUPP       199
#define ERR_AT_CMS_D0_SIM_SMS_STO_FULL  208
#define ERR_AT_CMS_NO_SMS_STO_IN_SIM    209
#define ERR_AT_CMS_ERR_IN_MS            210
#define ERR_AT_CMS_MEM_CAP_EXCCEEDED    211
#define ERR_AT_CMS_SIM_APP_TK_BUSY      212
#define ERR_AT_CMS_SIM_DATA_DL_ERROR    213
#define ERR_AT_CMS_UNSPEC_ERRO_CAUSE    255

#define ERR_AT_CMS_ME_FAIL              300
#define ERR_AT_CMS_SMS_SERVIEC_RESERVED 301
#define ERR_AT_CMS_OPER_NOT_ALLOWED     302
#define ERR_AT_CMS_OPER_NOT_SUPP        303
#define ERR_AT_CMS_INVALID_PDU_PARAM    304
#define ERR_AT_CMS_INVALID_TXT_PARAM    305
#define ERR_AT_CMS_SIM_NOT_INSERT       310
#define ERR_AT_CMS_SIM_PIN_REQUIRED     311
#define ERR_AT_CMS_PH_SIM_PIN_REQUIRED  312
#define ERR_AT_CMS_SIM_FAIL             313
#define ERR_AT_CMS_SIM_BUSY             314
#define ERR_AT_CMS_SIM_WRONG            315
#define ERR_AT_CMS_SIM_PUK_REQUIRED     316
#define ERR_AT_CMS_SIM_PIN2_REQUIRED    317
#define ERR_AT_CMS_SIM_PUK2_REQUIRED    318

#define ERR_AT_CMS_MEM_FAIL             320
#define ERR_AT_CMS_INVALID_MEM_INDEX    321
#define ERR_AT_CMS_MEM_FULL             322
#define ERR_AT_CMS_SCA_ADDR_UNKNOWN     330
#define ERR_AT_CMS_NO_NW_SERVICE        331
#define ERR_AT_CMS_NW_TIMEOUT           332
#define ERR_AT_CMS_NO_CNMA_ACK_EXPECTED 340
#define ERR_AT_CMS_UNKNOWN_ERROR        500

#define ERR_AT_CMS_USER_ABORT           512
#define ERR_AT_CMS_UNABLE_TO_STORE      513
#define ERR_AT_CMS_INVALID_STATUS       514
#define ERR_AT_CMS_INVALID_ADDR_CHAR    515
#define ERR_AT_CMS_INVALID_LEN          516
#define ERR_AT_CMS_INVALID_PDU_CHAR     517
#define ERR_AT_CMS_INVALID_PARA         518
#define ERR_AT_CMS_INVALID_LEN_OR_CHAR  519
#define ERR_AT_CMS_INVALID_TXT_CHAR     520
#define ERR_AT_CMS_TIMER_EXPIRED        521
// CMS define end

#endif
