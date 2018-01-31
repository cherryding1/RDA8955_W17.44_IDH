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

#include "at_errinfo.h"
#include "at_errcode.h"
#include "at_common.h"
#include "at_utils.h"
#include <stdlib.h>

struct AT_CODE_TEXT
{
    int code;
    const char *text;
};

static const struct AT_CODE_TEXT g_sResultCodeInfo[] = {
    {CMD_RC_OK, "OK"},
    {CMD_RC_CONNECT, "CONNECT"},
    {CMD_RC_RING, "RING"},
    {CMD_RC_NOCARRIER, "NO CARRIER"},
    {CMD_RC_ERROR, "ERROR"},
    {CMD_RC_NODIALTONE, "NO DIALTONE"},
    {CMD_RC_BUSY, "BUSY"},
    {CMD_RC_NOANSWER, "NO ANSWER"},
    {CMD_RC_NOTSUPPORT, "NOT SUPPORT"},
    {CMD_RC_INVCMDLINE, "INVALID COMMAND LINE"},
    {CMD_RC_CR, ""},
    {CMD_RC_SIMDROP, "SIM not inserted"},
};

// The CME error information.
static const struct AT_CODE_TEXT g_sCmeErrInfo[] = {
    //
    // General errors
    //
    {ERR_AT_CME_PHONE_FAILURE, "phone failure"},
    {ERR_AT_CME_NO_CONNECT_PHONE, "no connection to phone"},
    {ERR_AT_CME_PHONE_ADAPTER_LINK_RESERVED, "phone-adapter link reserved"},
    {ERR_AT_CME_OPERATION_NOT_ALLOWED, "Operation not allowed"},
    {ERR_AT_CME_OPERATION_NOT_SUPPORTED, "Operation not supported"},
    {ERR_AT_CME_PHSIM_PIN_REQUIRED, "PH-SIM PIN required"},
    {ERR_AT_CME_PHFSIM_PIN_REQUIRED, "PH-FSIM PIN required"},
    {ERR_AT_CME_PHFSIM_PUK_REQUIRED, "PH-FSIM PUK required"},
    {ERR_AT_CME_SIM_NOT_INSERTED, "SIM not inserted"},
    {ERR_AT_CME_SIM_PIN_REQUIRED, "SIM PIN required"},
    {ERR_AT_CME_SIM_PUK_REQUIRED, "SIM PUK required"},
    {ERR_AT_CME_SIM_FAILURE, "SIM failure"},
    {ERR_AT_CME_SIM_BUSY, "SIM busy"},
    {ERR_AT_CME_SIM_WRONG, "SIM wrong"},
    {ERR_AT_CME_INCORRECT_PASSWORD, "Incorrect password"},
    {ERR_AT_CME_SIM_PIN2_REQUIRED, "SIM PIN2 required"},
    {ERR_AT_CME_SIM_PUK2_REQUIRED, "SIM PUK2 required"},
    {ERR_AT_CME_MEMORY_FULL, "Memory full"},
    {ERR_AT_CME_INVALID_INDEX, "invalid index"},
    {ERR_AT_CME_NOT_FOUND, "not found"},
    {ERR_AT_CME_MEMORY_FAILURE, "Memory failure"},
    {ERR_AT_CME_TEXT_LONG, "text string too long"},
    {ERR_AT_CME_INVALID_CHAR_INTEXT, "invalid characters in text string"},
    {ERR_AT_CME_DAIL_STR_LONG, "dial string too long"},
    {ERR_AT_CME_INVALID_CHAR_INDIAL, "invalid characters in dial string"},
    {ERR_AT_CME_NO_NET_SERVICE, "no network service"},
    {ERR_AT_CME_NETWORK_TIMOUT, "Network timeout"},
    {ERR_AT_CME_NOT_ALLOW_EMERGENCY, "Network not allowed emergency calls only"},
    {ERR_AT_CME_NET_PER_PIN_REQUIRED, "Network personalization PIN required"},
    {ERR_AT_CME_NET_PER_PUK_REQUIRED, "Network personalization PUK required"},
    {ERR_AT_CME_NET_SUB_PER_PIN_REQ, "Network subset personalization PIN required"},
    {ERR_AT_CME_NET_SUB_PER_PUK_REQ, "Network subset personalization PUK required"},
    {ERR_AT_CME_SERVICE_PROV_PER_PIN_REQ, "service provider personalization PIN required"},
    {ERR_AT_CME_SERVICE_PROV_PER_PUK_REQ, "service provider personalization PUK required"},
    {ERR_AT_CME_CORPORATE_PER_PIN_REQ, "Corporate personalization PIN required"},
    {ERR_AT_CME_CORPORATE_PER_PUK_REQ, "Corporate personalization PUK required"},
    {ERR_AT_CME_PHSIM_PBK_REQUIRED, "PH-SIM PUK required (PH-SIM PUK may also be referred to as Master Phone Code. For further details"},

    {ERR_AT_CME_EXE_NOT_SURPORT, "The excute command not support"},
    {ERR_AT_CME_EXE_FAIL, "Excute command failure"},
    {ERR_AT_CME_NO_MEMORY, "no memory"},
    {ERR_AT_CME_OPTION_NOT_SURPORT, "The command not support,check your input,pls"},
    {ERR_AT_CME_PARAM_INVALID, "parameters are invalid"},
    {ERR_AT_CME_EXT_REG_NOT_EXIT, "REG not exit in flash"},
    {ERR_AT_CME_EXT_SMS_NOT_EXIT, "SMS not eixt in flash"},
    {ERR_AT_CME_EXT_PBK_NOT_EXIT, "Phone book not eixt in flash"},
    {ERR_AT_CME_EXT_FFS_NOT_EXIT, "file system  not eixt in flash"},
    {ERR_AT_CME_INVALID_COMMAND_LINE, "invalid command line"},
    // {ERR_AT_CME_ITF_DIFFERENT, },
    // {ERR_AT_CME_BURN_FLASH_FAIL, },
    // {ERR_AT_CME_TFLASH_NOT_EXIST, },

    //
    // GPRS-related errors.
    //
    {ERR_AT_CME_GPRS_ILLEGAL_MS_3, "Illegal MS"},
    {ERR_AT_CME_GPRS_ILLEGAL_MS_6, "Illegal ME"},
    {ERR_AT_CME_GPRS_SVR_NOT_ALLOWED, "GPRS services not allowed"},
    {ERR_AT_CME_GPRS_PLMN_NOT_ALLOWED, "PLMN not allowed"},
    {ERR_AT_CME_GPRS_LOCATION_AREA_NOT_ALLOWED, "Location area not allowed"},
    {ERR_AT_CME_GPRS_ROAMING_NOT_ALLOWED, "Roaming not allowed in this location area"},
    {ERR_AT_CME_GPRS_OPTION_NOT_SUPPORTED, "Service option not supported"},
    {ERR_AT_CME_GPRS_OPTION_NOT_SUBSCRIBED, "Requested service option not subscribed"},
    {ERR_AT_CME_GPRS_OPTION_TEMP_ORDER_OUT, "Service option temporarily out of order"},
    {ERR_AT_CME_GPRS_UNSPECIFIED_GPRS_ERROR, "Unspecified GPRS error"},
    {ERR_AT_CME_GPRS_PDP_AUTHENTICATION_FAILURE, "PDP authentication failure"},
    {ERR_AT_CME_GPRS_INVALID_MOBILE_CLASS, "Invalid mobile class"},

    //
    // Sim-related errors.
    //
    {ERR_AT_CME_SIM_VERIFY_FAIL, "SIM card verify failure"},
    {ERR_AT_CME_SIM_UNBLOCK_FAIL, "Unblock SIM card fail"},
    {ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED, "Condition not fullfilled"},
    {ERR_AT_CME_SIM_UNBLOCK_FAIL_NO_LEFT, "Unblock SIM card failed, no rest time"},
    {ERR_AT_CME_SIM_VERIFY_FAIL_NO_LEFT, "Verify SIM card failed, no rest time"},
    {ERR_AT_CME_SIM_INVALID_PARAMETER, "Input parameter is invalid"},
    {ERR_AT_CME_SIM_UNKNOW_COMMAND, "file is inconsistent with the command"},
    {ERR_AT_CME_SIM_WRONG_CLASS, "wrong instruction class given in the command"},
    {ERR_AT_CME_SIM_TECHNICAL_PROBLEM, "SIM returned technical problem"},
    {ERR_AT_CME_SIM_CHV_NEED_UNBLOCK, "CHV need unblock"},
    {ERR_AT_CME_SIM_NOEF_SELECTED, "NO SIM EF selected"},
    {ERR_AT_CME_SIM_FILE_UNMATCH_COMMAND, "SIM file unmatch command"},
    {ERR_AT_CME_SIM_CONTRADICTION_CHV, "contradiction CHV"},
    {ERR_AT_CME_SIM_CONTRADICTION_INVALIDATION, "contradiction invalidation"},
    {ERR_AT_CME_SIM_MAXVALUE_REACHED, "SIM MAX value reached"},
    {ERR_AT_CME_SIM_PATTERN_NOT_FOUND, "SIM returned pattern not found"},
    {ERR_AT_CME_SIM_FILEID_NOT_FOUND, "SIM file ID not found"},
    {ERR_AT_CME_SIM_STK_BUSY, "STK busy"},
    {ERR_AT_CME_SIM_UNKNOW, "SIM returned UNKNOWN"},
    {ERR_AT_CME_SIM_PROFILE_ERROR, "SIM profile ERR"},
};

// The CMS error information.
static const struct AT_CODE_TEXT g_sCmsErrInfo[] = {
    {ERR_AT_CMS_UNASSIGNED_NUM, "Unassigned (unallocated) number"},
    {ERR_AT_CMS_OPER_DETERM_BARR, "Operator determined barring"},
    {ERR_AT_CMS_CALL_BARRED, "Call barred"},
    {ERR_AT_CMS_SM_TRANS_REJE, "Short message transfer rejected"},
    {ERR_AT_CMS_DEST_OOS, "Destination out of service"},
    {ERR_AT_CMS_UNINDENT_SUB, "Unidentified subscriber"},
    {ERR_AT_CMS_FACILIT_REJE, "Facility rejected"},
    {ERR_AT_CMS_UNKONWN_SUB, "Unknown subscriber"},
    {ERR_AT_CMS_NW_OOO, "Network out of order"},
    {ERR_AT_CMS_TMEP_FAIL, "Temporary failure"},
    {ERR_AT_CMS_CONGESTION, "Congestion"},
    {ERR_AT_CMS_RES_UNAVAILABLE, "Resources unavailable, unspecified"},
    {ERR_AT_CMS_REQ_FAC_NOT_SUB, "Requested facility not subscribed"},
    {ERR_AT_CMS_RFQ_FAC_NOT_IMP, "Requested facility not implemented"},
    {ERR_AT_CMS_INVALID_SM_TRV, "Invalid short message transfer reference value"},
    {ERR_AT_CMS_INVALID_MSG, "Invalid message, unspecified"},
    {ERR_AT_CMS_INVALID_MAND_INFO, "Invalid mandatory information"},
    {ERR_AT_CMS_MSG_TYPE_ERROR, "Message type non-existent or not implemented"},
    {ERR_AT_CMS_MSG_NOT_COMP, "Message not compatible with short message protocol state"},
    {ERR_AT_CMS_INFO_ELEMENT_ERROR, "Information element non-existent or not implemented"},
    {ERR_AT_CMS_PROT_ERROR, " Protocol error, unspecified"},
    {ERR_AT_CMS_IW_UNSPEC, "Interworking, unspecified"},
    {ERR_AT_CMS_TEL_IW_NOT_SUPP, "Telematic interworking not supported"},
    {ERR_AT_CMS_SMS_TYPE0_NOT_SUPP, "Short message Type 0 not supported"},
    {ERR_AT_CMS_CANNOT_REP_SMS, "Cannot replace short message"},
    {ERR_AT_CMS_UNSPEC_TP_ERROR, "Unspecified TP-PID error"},
    {ERR_AT_CMS_DCS_NOT_SUPP, "Data coding scheme (alphabet) not supported"},
    {ERR_AT_CMS_MSG_CLASS_NOT_SUPP, "Message class not supported"},
    {ERR_AT_CMS_UNSPEC_TD_ERROR, "Unspecified TP-DCS error"},
    {ERR_AT_CMS_CMD_CANNOT_ACT, "Command cannot be actioned "},
    {ERR_AT_CMS_CMD_UNSUPP, "Command unsupported"},
    {ERR_AT_CMS_UNSPEC_TC_ERROR, "Unspecified TP-Command error"},
    {ERR_AT_CMS_TPDU_NOT_SUPP, "TPDU not supported"},
    {ERR_AT_CMS_SC_BUSY, "SC busy"},
    {ERR_AT_CMS_NO_SC_SUB, "No SC subscription"},
    {ERR_AT_CMS_SC_SYS_FAIL, "SC system failure"},
    {ERR_AT_CMS_INVALID_SME_ADDR, "Invalid SME address"},
    {ERR_AT_CMS_DEST_SME_BARR, "Destination SME barred"},
    {ERR_AT_CMS_SM_RD_SM, "SM Rejected-Duplicate SM"},
    {ERR_AT_CMS_TP_VPF_NOT_SUPP, "TP-VPF not supported"},
    {ERR_AT_CMS_TP_VP_NOT_SUPP, "TP-VP not supported"},
    {ERR_AT_CMS_D0_SIM_SMS_STO_FULL, "D0 SIM SMS storage full"},
    {ERR_AT_CMS_NO_SMS_STO_IN_SIM, "No SMS storage capability in SIM"},
    {ERR_AT_CMS_ERR_IN_MS, "Error in MS"},
    {ERR_AT_CMS_MEM_CAP_EXCCEEDED, "Memory Capacity Exceeded"},
    {ERR_AT_CMS_SIM_APP_TK_BUSY, "SIM Application Toolkit Busy"},
    {ERR_AT_CMS_SIM_DATA_DL_ERROR, "SIM data download error"},
    {ERR_AT_CMS_UNSPEC_ERRO_CAUSE, "Unspecified error cause"},

    {ERR_AT_CMS_ME_FAIL, "ME failure"},
    {ERR_AT_CMS_SMS_SERVIEC_RESERVED, "SMS service of ME reserved"},
    {ERR_AT_CMS_OPER_NOT_ALLOWED, "Operation not allowed"},
    {ERR_AT_CMS_OPER_NOT_SUPP, "Operation not supported"},
    {ERR_AT_CMS_INVALID_PDU_PARAM, "Invalid PDU mode parameter"},
    {ERR_AT_CMS_INVALID_TXT_PARAM, "Invalid text mode parameter"},
    {ERR_AT_CMS_SIM_NOT_INSERT, "SIM not inserted"},
    {ERR_AT_CMS_SIM_PIN_REQUIRED, "SIM PIN required"},
    {ERR_AT_CMS_PH_SIM_PIN_REQUIRED, "PH-SIM PIN required"},
    {ERR_AT_CMS_SIM_FAIL, "SIM failure"},
    {ERR_AT_CMS_SIM_BUSY, "SIM busy"},
    {ERR_AT_CMS_SIM_WRONG, "SIM wrong"},
    {ERR_AT_CMS_SIM_PUK_REQUIRED, "SIM PUK required"},
    {ERR_AT_CMS_SIM_PIN2_REQUIRED, "SIM PIN2 required"},
    {ERR_AT_CMS_SIM_PUK2_REQUIRED, "SIM PUK2 required"},
    {ERR_AT_CMS_MEM_FAIL, "Memory failure"},
    {ERR_AT_CMS_INVALID_MEM_INDEX, "Invalid memory index"},
    {ERR_AT_CMS_MEM_FULL, "Memory full"},
    {ERR_AT_CMS_SCA_ADDR_UNKNOWN, "SMSC address unknown"},
    {ERR_AT_CMS_NO_NW_SERVICE, "no network service"},
    {ERR_AT_CMS_NW_TIMEOUT, "Network timeout"},
    {ERR_AT_CMS_NO_CNMA_ACK_EXPECTED, "NO +CNMA ACK EXPECTED"},
    {ERR_AT_CMS_UNKNOWN_ERROR, "Unknown error"},

    {ERR_AT_CMS_USER_ABORT, "User abort"},
    {ERR_AT_CMS_UNABLE_TO_STORE, "unable to store"},
    {ERR_AT_CMS_INVALID_STATUS, "invalid status"},
    {ERR_AT_CMS_INVALID_ADDR_CHAR, "invalid character in address string"},
    {ERR_AT_CMS_INVALID_LEN, "invalid length"},
    {ERR_AT_CMS_INVALID_PDU_CHAR, "invalid character in pdu"},
    {ERR_AT_CMS_INVALID_PARA, "invalid parameter"},
    {ERR_AT_CMS_INVALID_LEN_OR_CHAR, "invalid length or character"},
    {ERR_AT_CMS_INVALID_TXT_CHAR, "invalid character in text"},
    {ERR_AT_CMS_TIMER_EXPIRED, "timer expired"},
};

static int at_CodeTextCmp(const void *key, const void *p)
{
    int code = (int)key;
    const struct AT_CODE_TEXT *ct = (const struct AT_CODE_TEXT *)p;
    return code - ct->code;
}

static const uint8_t *at_GetCodeInfo(int code, const struct AT_CODE_TEXT *info, unsigned count)
{
    const struct AT_CODE_TEXT *p = bsearch((const void *)code,
                                           info, count, sizeof(*info),
                                           at_CodeTextCmp);

    if (p != NULL)
        return p->text;

    static uint8_t text[16];
    sprintf(text, "%d", code);
    return text;
}

const uint8_t *at_GetResultCodeInfo(int code)
{
    return at_GetCodeInfo(code, g_sResultCodeInfo, ARRAY_SIZE(g_sResultCodeInfo));
}

const uint8_t *at_GetCmeCodeInfo(int code)
{
    return at_GetCodeInfo(code, g_sCmeErrInfo, ARRAY_SIZE(g_sCmeErrInfo));
}

const uint8_t *at_GetCmsCodeInfo(int code)
{
    return at_GetCodeInfo(code, g_sCmsErrInfo, ARRAY_SIZE(g_sCmsErrInfo));
}

static void at_ErrCodeInfoArrayCheck(const struct AT_CODE_TEXT *info, unsigned count)
{
    for (int n = 1; n < count; n++)
    {
        if (info[n].code <= info[n - 1].code)
            ATLOGSE(TSM(1), "AT code array error: %d %s", info[n].code, info[n].text);
    }
}

void at_ErrCodeInfoCheck(void)
{
    at_ErrCodeInfoArrayCheck(g_sResultCodeInfo, ARRAY_SIZE(g_sResultCodeInfo));
    at_ErrCodeInfoArrayCheck(g_sCmeErrInfo, ARRAY_SIZE(g_sCmeErrInfo));
    at_ErrCodeInfoArrayCheck(g_sCmsErrInfo, ARRAY_SIZE(g_sCmsErrInfo));
}
