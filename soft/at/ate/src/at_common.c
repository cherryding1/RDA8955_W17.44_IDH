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

#include "at.h"
#include "at_sa.h"
#include "at_cmd_gc.h"
#include "at_module.h"
#include "at_utils.h"
#include "at_cfg.h"
#include "at_common.h"
#include "at_cmd_engine.h"

#if defined(CFW_GPRS_SUPPORT) && !defined(AT_WITHOUT_GPRS)
extern VOID AT_CLearGprsCtx(UINT8 nDLCI); // at_cmd_gprs.c
#endif

// //////////////////////////////////////////////////////////////////////////////
// UTI: user transaction id table defines
// add by wangqunyang 2008.06.02
// //////////////////////////////////////////////////////////////////////////////
UINT16 nATUserTransIdSpareHeader = 0;
UINT16 nATUserTransIdSpareTail = 0;
AT_UTI_TABLE ATUserTransIdTable[AT_USER_TRANS_ID_SIZE];
struct ATWaitingEvent gAT_WaitingEvent[AT_WAITING_EVENT_MAX] = {
    {0, 0, 0, 0},
};

void AT_CosEvent2CfwEvent(COS_EVENT *pCosEvent, CFW_EVENT *pCfwEvent)
{
    pCfwEvent->nEventId = pCosEvent->nEventId;
    pCfwEvent->nParam1 = pCosEvent->nParam1;
    pCfwEvent->nParam2 = pCosEvent->nParam2;
    pCfwEvent->nUTI = HIUINT16(pCosEvent->nParam3);
    pCfwEvent->nType = HIUINT8(pCosEvent->nParam3);
    pCfwEvent->nFlag = LOUINT8(pCosEvent->nParam3);
}

PAT_CMD_RESULT AT_CreateRC(UINT32 uReturnValue,
                           UINT32 uResultCode,
                           UINT32 uErrorCode,
                           UINT8 nErrorType,
                           UINT32 nDelayTime,
                           UINT8 *pBuffer,
                           UINT16 nDataSize,
                           UINT8 nDLCI)
{
    // There are additional size in PAT_CMD_RESULT, but "+1" looks better
    PAT_CMD_RESULT pResult = (PAT_CMD_RESULT)AT_MALLOC(sizeof(AT_CMD_RESULT) + nDataSize + 1);
    if (pResult == NULL)
        return NULL;

    AT_MemZero(pResult, sizeof(AT_CMD_RESULT) + nDataSize + 1);
    pResult->uReturnValue = uReturnValue;

    pResult->uResultCode = uResultCode;
    pResult->uErrorCode = uErrorCode;
    pResult->nErrorType = nErrorType;
    pResult->nDataSize = nDataSize;
    pResult->nDelayTime = nDelayTime;
    pResult->engine = at_CmdGetByChannel(nDLCI);

    if (pBuffer != NULL)
    {
        AT_MemCpy(pResult->pData, pBuffer, nDataSize);

        // Make a protection on RSP format.
        AT_Util_TrimRspStringSuffixCrLf(pResult->pData, &pResult->nDataSize);
        pResult->pData[pResult->nDataSize] = '\0';
    }

    return pResult;
}

// AT_20071123_CAOW_B
UINT32 AT_GetPhoneActiveStatus(UINT8 nSim)
{
    UINT32 ret = AT_ACTIVE_STATUS_READY;

    switch (CFW_CcGetCallStatus(nSim))
    {
    case 0x01: // case CC_STATE_ACTIVE:
    case 0x02: // case CC_STATE_HOLD:
    case 0x40: // case CC_STATE_RELEASE:
        ret = AT_ACTIVE_STATUS_CALL_IN_PROGRESS;
        break;

    case 0x04: // case CC_STATE_WAITING:
    case 0x08: // case CC_STATE_INCOMING:
    case 0x10: // case CC_STATE_DIALING:
    case 0x20: // case CC_STATE_ALERTLING:
        ret = AT_ACTIVE_STATUS_RINGING;
        break;

    default:
        break;
    }

    return ret;
}

/******************************************************************************************
Function            :   AT_SetCmeErrorCode
Description     :   set cme error code
Called By           :   ATS moudle
Data Accessed   :
Data Updated    :
Input           :      UINT32 nCfwErrorCode, BOOL bSetParamValid
Output          :
Return          :   UINT32
Others          :   build by author unknown
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
UINT32 AT_SetCmeErrorCode(UINT32 nCfwErrorCode, BOOL bSetParamValid)
{
    switch (nCfwErrorCode)
    {
    case ERR_CFW_INVALID_PARAMETER:
        return bSetParamValid ? ERR_AT_CME_PARAM_INVALID : ERR_AT_CME_EXE_FAIL;

    case ERR_CME_SIM_NOT_INSERTED:
        return ERR_AT_CME_SIM_NOT_INSERTED;

    case ERR_CME_SIM_PIN_REQUIRED:
        return ERR_AT_CME_SIM_PIN_REQUIRED;

    case ERR_CME_SIM_PUK_REQUIRED:
        return ERR_AT_CME_SIM_PUK_REQUIRED;

    case ERR_CME_SIM_PIN2_REQUIRED:
        return ERR_AT_CME_SIM_PIN2_REQUIRED;

    case ERR_CME_SIM_PUK2_REQUIRED:
    case ERR_CME_SIM_CHV_NEED_UNBLOCK:
        return ERR_AT_CME_SIM_PUK2_REQUIRED;

    case ERR_NO_MORE_MEMORY:
        return ERR_AT_CME_MEMORY_FULL;

    case ERR_CME_MEMORY_FULL:
        return ERR_AT_CME_MEMORY_FULL;

    case ERR_CME_INCORRECT_PASSWORD:
    case ERR_CME_SIM_COMMAND_SUCCESS_VERIFICATION_FAIL:
        return ERR_AT_CME_INCORRECT_PASSWORD;

    case ERR_CFW_NOT_SUPPORT:
        return ERR_AT_CME_OPERATION_NOT_SUPPORTED;

    case ERR_CME_INVALID_INDEX:
        return ERR_AT_CME_INVALID_INDEX;

    case ERR_CME_MEMORY_FAILURE:
        return ERR_AT_CME_MEMORY_FAILURE;

    case ERR_CME_SIM_CHV_UNINIT:
    case ERR_CME_SIM_UNBLOCK_FAIL:
    case ERR_CME_SIM_UNBLOCK_FAIL_NO_LEFT:
    case ERR_CME_SIM_VERIFY_FAIL_NO_LEFT:
        return ERR_AT_CME_SIM_FAILURE;

    case ERR_CME_SIM_VERIFY_FAIL:
        return ERR_AT_CME_INCORRECT_PASSWORD;

    case ERR_CME_SIM_BLOCKED:
    case ERR_CME_SIM_PROFILE_ERROR:
    case ERR_CME_SIM_FILEID_NOT_FOUND:
    case ERR_CME_SIM_PATTERN_NOT_FOUND:
    case ERR_CME_SIM_MAXVALUE_REACHED:
    case ERR_CME_SIM_CONTRADICTION_INVALIDATION:
    case ERR_CME_SIM_CONTRADICTION_CHV:
    case ERR_CME_SIM_FILE_UNMATCH_COMMAND:
    case ERR_CME_SIM_NOEF_SELECTED:
    case ERR_CME_SIM_TECHNICAL_PROBLEM:
    case ERR_CME_SIM_WRONG_CLASS:
    case ERR_CME_SIM_UNKNOW_COMMAND:
    case ERR_CME_SIM_CONDITION_NO_FULLFILLED:
        return ERR_AT_CME_SIM_WRONG;

    case ERR_CME_SIM_UNKNOW:
        return ERR_AT_CME_SIM_NOT_INSERTED;

    case ERR_CME_SIM_STK_BUSY:
        return ERR_AT_CME_SIM_BUSY;

    case ERR_CME_PHONE_FAILURE:
        return ERR_AT_CME_PHONE_FAILURE;

    case ERR_CME_NO_CONNECTION_TO_PHONE:
        return ERR_AT_CME_NO_CONNECT_PHONE;

    case ERR_CME_PHONE_ADAPTER_LINK_RESERVED:
        return ERR_AT_CME_PHONE_ADAPTER_LINK_RESERVED;

    case ERR_CME_OPERATION_NOT_ALLOWED:
        return ERR_AT_CME_OPERATION_NOT_ALLOWED;

    case ERR_CME_OPERATION_NOT_SUPPORTED:
        return ERR_AT_CME_OPERATION_NOT_SUPPORTED;

    case ERR_CME_PH_SIM_PIN_REQUIRED:
        return ERR_AT_CME_PHSIM_PIN_REQUIRED;

    case ERR_CME_PH_FSIM_PIN_REQUIRED:
        return ERR_AT_CME_PHFSIM_PIN_REQUIRED;

    case ERR_CME_PH_FSIM_PUK_REQUIRED:
        return ERR_AT_CME_PHFSIM_PUK_REQUIRED;

    case ERR_CME_SIM_FAILURE:
        return ERR_AT_CME_SIM_FAILURE;

    case ERR_CME_SIM_BUSY:
        return ERR_AT_CME_SIM_BUSY;

    case ERR_CME_NOT_FOUND:
        return ERR_AT_CME_NOT_FOUND;

    case ERR_CME_TEXT_STRING_TOO_LONG:
        return ERR_AT_CME_TEXT_LONG;

    case ERR_CME_INVALID_CHARACTERS_IN_TEXT_STRING:
        return ERR_AT_CME_INVALID_CHAR_INTEXT;

    case ERR_CME_DIAL_STRING_TOO_LONG:
        return ERR_AT_CME_DAIL_STR_LONG;

    case ERR_CME_INVALID_CHARACTERS_IN_DIAL_STRING:
        return ERR_AT_CME_INVALID_CHAR_INDIAL;

    case ERR_CME_NO_NETWORK_SERVICE:
        return ERR_AT_CME_NO_NET_SERVICE;

    case ERR_CME_NETWORK_TIMEOUT:
        return ERR_AT_CME_NETWORK_TIMOUT;

    case ERR_CME_NETWORK_NOT_ALLOWED_EMERGENCY_CALLS_ONLY:
        return ERR_AT_CME_NOT_ALLOW_EMERGENCY;

    case ERR_CME_NETWORK_PERSONALIZATION_PIN_REQUIRED:
        return ERR_AT_CME_NET_PER_PIN_REQUIRED;

    case ERR_CME_NETWORK_PERSONALIZATION_PUK_REQUIRED:
        return ERR_AT_CME_NET_PER_PUK_REQUIRED;

    case ERR_CME_NETWORK_SUBSET_PS_PIN_REQUIRED:
        return ERR_AT_CME_NET_SUB_PER_PIN_REQ;

    case ERR_CME_NETWORK_SUBSET_PS_PUK_REQUIRED:
        return ERR_AT_CME_NET_SUB_PER_PUK_REQ;

    case ERR_CME_SERVICE_PROVIDER_PS_PIN_REQUIRED:
        return ERR_AT_CME_SERVICE_PROV_PER_PIN_REQ;

    case ERR_CME_SERVICE_PROVIDER_PS_PUK_REQUIRED:
        return ERR_AT_CME_SERVICE_PROV_PER_PUK_REQ;

    case ERR_CME_CORPORATE_PERSONALIZATION_PIN_REQUIRED:
        return ERR_AT_CME_CORPORATE_PER_PIN_REQ;

    case ERR_CME_CORPORATE_PERSONALIZATION_PUK_REQUIRED:
        return ERR_AT_CME_CORPORATE_PER_PUK_REQ;

    case ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED:
        return ERR_AT_CME_OPERATION_NOT_ALLOWED;

    default:
        return ERR_AT_CME_EXE_FAIL;
    }
}

/******************************************************************************************
Function            :   AT_Get_RetureValue_Name
Description     :      transfer reture value to name string
Called By           :    AT_SMS_RESULT_OK
Data Accessed       :
Data Updated        :
Input           :      uReturnValue, int type
Output          :
Return          :   char *
Others          :   build by wangqunyang 08/04/2008
Mark                     :
*******************************************************************************************/
UINT8 *AT_Get_RetureValue_Name(UINT32 uReturnValue)
{
    switch (uReturnValue)
    {
    case CMD_FUNC_SUCC:
        return "CMD_FUNC_SUCC";

    case CMD_FUNC_SUCC_ASYN:
        return "CMD_FUNC_SUCC_ASYN";

    case CMD_FUNC_FAIL:
        return "CMD_FUNC_FAIL";

    case CMD_FUNC_CONTINUE:
        return "CMD_FUNC_CONTINUE";

    case CMD_FUNC_WAIT_SMS:
        return "CMD_FUNC_WAIT_SMS";
    case CMD_FUNC_WAIT_IP_DATA:
        return "CMD_FUNC_WAIT_IP_DATA";

    case CSW_IND_NOTIFICATION:
        return "CSW_IND_NOTIFICATION";

    default:
        return "Error uReturnValue";
    }
}

/******************************************************************************************
Function            :   AT_Get_ResultCode_Name
Description     :      transfer result code to name string
Called By           :    AT_SMS_RESULT_OK
Data Accessed       :
Data Updated    :
Input           :      uResultCode, int type
Output          :
Return          :   char *
Others          :   build by wangqunyang 08/04/2008
Mark                     :
*******************************************************************************************/
UINT8 *AT_Get_ResultCode_Name(UINT32 uResultCode)
{
    switch (uResultCode)
    {
    case CMD_RC_OK:
        return "CMD_RC_OK";

    case CMD_RC_CONNECT:
        return "CMD_RC_CONNECT";

    case CMD_RC_RING:
        return "CMD_RC_RING";

    case CMD_RC_NOCARRIER:
        return "CMD_RC_NOCARRIER";

    case CMD_RC_ERROR:
        return "CMD_RC_ERROR";

    case CMD_RC_NODIALTONE:
        return "CMD_RC_NODIALTONE";

    case CMD_RC_BUSY:
        return "CMD_RC_BUSY";

    case CMD_RC_NOANSWER:
        return "CMD_RC_NOANSWER";

    case CMD_RC_NOTSUPPORT:
        return "CMD_RC_NOTSUPPORT";

    case CMD_RC_INVCMDLINE:
        return "CMD_RC_INVCMDLINE";

    case CMD_RC_CR:
        return "CMD_RC_CR";

    case CMD_RC_SIMDROP:
        return "CMD_RC_SIM_DROP";

    default:
        return "Error uResultCode";
    }
}

/******************************************************************************************
Function            :   AT_SetCmeErrorCode
Description     :   set cme error code
Called By           :   ATS moudle
Data Accessed   :
Data Updated    :
Input           :      UINT32 nCfwErrorCode, BOOL bSetParamValid
Output          :
Return          :   UINT32
Others          :   build by author unknown
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
VOID AT_Set_MultiLanguage()
{
    at_chset_t eCurrentSet = gATCurrentCfgInfoMt_chset;
    UINT8 nMultiCharset[12] = {0};

    switch (eCurrentSet)
    {

    case cs_gsm:
        AT_MemCpy(nMultiCharset, ML_ISO8859_1, AT_StrLen(ML_ISO8859_1));
        break;

    case cs_gbk:
        AT_MemCpy(nMultiCharset, ML_CP936, AT_StrLen(ML_CP936));
        break;

    case cs_hex:
        // to be continue
        break;

    case cs_ucs2:
        // to be continue
        break;

    default:
        AT_TC(g_sw_UTIL, "AT_Set_MultiLanguage: wrong char set");
        break;
    }

    // Notify the csw char set
    if (ERR_SUCCESS != ML_SetCodePage(nMultiCharset))
    {
        AT_TC(g_sw_UTIL, "AT_Set_MultiLanguage: ML_SetCodePage() is failure");
    }
}

/******************************************************************************************
Function            :   SMS_UnicodeBigend2Unicode
Description     :       unicode BigEnd to unicode normal(LittlteEnd)
Called By           :
Data Accessed       :
Data Updated        :
Input           :       UINT8 * pUniBigData, UINT8 *pUniData, UINT16 nDataLen
Output          :
Return          :   BOOL
Others          :       build by wangqunyang 30/04/2008
*******************************************************************************************/
BOOL AT_UnicodeBigEnd2Unicode(UINT8 *pUniBigData, UINT8 *pUniData, UINT16 nDataLen)
{

    /* check the pointer is NULL or not */
    if ((NULL == pUniData) || (NULL == pUniBigData))
    {
        return FALSE;
    }

    /* there is odd chars and we make
       ** it even, discard the last char */
    if (nDataLen % 2)
    {
        nDataLen = nDataLen - 1;
    }

    while (nDataLen > 0)
    {
        pUniData[nDataLen - 2] = pUniBigData[nDataLen - 1];
        pUniData[nDataLen - 1] = pUniBigData[nDataLen - 2];

        nDataLen -= 2;
    }

    return TRUE;
}

// caoxh [+]2008-05-9
char *strupr(char *pStr)
{
    char *p = pStr;

    if (p == NULL)
        return NULL;

    while (*p != '\0')
    {
        if (*p >= 'a' && *p <= 'z')
            *p = *p - 32;

        p++;
    }

    return pStr;
}

// caoxh [+]2008-05-9

/******************************************************************************************
Function            :   AT_Bytes2String
Description     :       transfer bytes to ascii string
Called By           :   ATS moudle
Data Accessed       :
Data Updated    :
Input           :   UINT8 * pSource, UINT8 * pDest, UINT8 nSourceLen
Output          :
Return          :   INT8
Others          :   build by wangqunyang 2008.05.22
*******************************************************************************************/
BOOL AT_Bytes2String(UINT8 *pDest, UINT8 *pSource, UINT8 *nSourceLen)
{

    UINT8 nTemp = 0;
    UINT8 nDestLen = 0;

    if ((NULL == pSource) || (NULL == pDest))
    {
        AT_TC(g_sw_UTIL, "AT_Bytes2String: pointer is NULL");
        return FALSE;
    }

    AT_TC(g_sw_UTIL, "AT_Bytes2String: nSourceLen = %u", *nSourceLen);

    while (nTemp < *nSourceLen)
    {
        /* get high byte */
        pDest[nDestLen] = (pSource[nTemp] >> 4) & 0x0f;

        if (pDest[nDestLen] < 10)
        {
            pDest[nDestLen] |= '0';
        }
        else
        {
            pDest[nDestLen] += 'A' - 10;
        }

        nDestLen++;

        /* get low byte */
        pDest[nDestLen] = pSource[nTemp] & 0x0f;

        if (pDest[nDestLen] < 10)
        {
            pDest[nDestLen] |= '0';
        }
        else
        {
            pDest[nDestLen] += 'A' - 10;
        }

        nDestLen++;

        nTemp++;
    }

    pDest[nDestLen] = '\0';

    *nSourceLen = nDestLen;

    /* string char counter must be even */

    if (*nSourceLen % 2)
    {
        AT_TC(g_sw_UTIL, "AT_Bytes2String: source len is not even, nSourceLen = %u", nSourceLen);
        return FALSE;
    }

    return TRUE;
}

/******************************************************************************************
Function            :   AT_String2Bytes
Description     :       This functions can transfer ascii string to bytes
Called By           :   ATS moudle
Data Accessed       :
Data Updated        :
Input           :       UINT8 * pDest, UINT8 * pSource, UINT8* pLen
Output          :
Return          :   INT8
Others          :   build by wangqunyang 2008.05.22
*******************************************************************************************/
BOOL AT_String2Bytes(UINT8 *pDest, UINT8 *pSource, UINT8 *pLen)
{
    UINT8 nSourceLen = *pLen;
    UINT8 nTemp = 0;
    UINT8 nByteNumber = 0;

    if ((NULL == pSource) || (NULL == pDest))
    {
        AT_TC(g_sw_UTIL, "AT_String2Bytes: pointer is NULL");
        return FALSE;
    }

    AT_TC(g_sw_UTIL, "AT_String2Bytes: nSourceLen = %u", nSourceLen);

    /* string char counter must be even */
    if (nSourceLen % 2)
    {
        AT_TC(g_sw_UTIL, "AT_String2Bytes: source len is not even");
        return FALSE;
    }

    while (nTemp < nSourceLen)
    {
        /* get high half byte */
        if ((pSource[nTemp] > 0x2f) && (pSource[nTemp] < 0x3a))
        {
            pDest[nByteNumber] = (pSource[nTemp] - '0') << 4;
        }
        else if ((pSource[nTemp] > 0x40) && (pSource[nTemp] < 0x47))
        {
            pDest[nByteNumber] = (pSource[nTemp] - 'A' + 10) << 4;
        }
        else if ((pSource[nTemp] > 0x60) && (pSource[nTemp] < 0x67))
        {
            pDest[nByteNumber] = (pSource[nTemp] - 'a' + 10) << 4;
        }
        else
        {
            AT_TC(g_sw_UTIL, "high half byte more than 'F' of HEX: pSource[nTemp]: = %u ", pSource[nTemp]);
            return FALSE;
        }

        /* get low half byte */
        nTemp++;

        if ((pSource[nTemp] > 0x2f) && (pSource[nTemp] < 0x3a))
        {
            pDest[nByteNumber] += (pSource[nTemp] - '0');
        }
        else if ((pSource[nTemp] > 0x40) && (pSource[nTemp] < 0x47))
        {
            pDest[nByteNumber] += (pSource[nTemp] - 'A' + 10);
        }
        else if ((pSource[nTemp] > 0x60) && (pSource[nTemp] < 0x67))
        {
            pDest[nByteNumber] += (pSource[nTemp] - 'a' + 10);
        }
        else
        {
            AT_TC(g_sw_UTIL, "low half byte more than 'F' of HEX: pSource[nTemp]: = %u ", pSource[nTemp]);
            return FALSE;
        }

        nTemp++;

        nByteNumber++;
    }

    pDest[nByteNumber] = '\0';

    *pLen = nByteNumber;

    return TRUE;
}

#ifdef MMI_ONLY_AT
UINT8 isEarphonePlugged_1 = 0;
UINT8 GetHandsetInPhone(VOID)
{
    return isEarphonePlugged_1;
}
VOID SetHandsetInPhone(UINT8 h_status)
{
    isEarphonePlugged_1 = h_status;
}
#endif

UINT32 AT_GetFreeUTI(UINT32 nServiceId, UINT8 *pUTI)
{
    return ERR_SUCCESS; // CFW_GetFreeUTI(nServiceId, pUTI);
}

/******************************************************************************************
Function            :   AT_InitUtiTable
Description     :       Initial the UTI table when AT Module beiginning
Called By           :   AT moudle
Data Accessed       :
Data Updated        :
Input           :       VOID
Output          :
Return          :   VOID
Others          :   build by wangqunyang 2008.06.02
*******************************************************************************************/
VOID AT_InitUtiTable()
{
    UINT16 nIndex = 0;

    AT_MemZero(&ATUserTransIdTable[0], AT_USER_TRANS_ID_SIZE * sizeof(AT_UTI_TABLE));

    nATUserTransIdSpareHeader = 1;
    nATUserTransIdSpareTail = 0;

    for (nIndex = 1; nIndex < AT_USER_TRANS_ID_SIZE - 1; nIndex++)
    {
        ATUserTransIdTable[nIndex].nNextUTISpareTable = nIndex + 1;
    }

    nATUserTransIdSpareTail = nIndex;
    ATUserTransIdTable[nATUserTransIdSpareTail].nNextUTISpareTable = 0;
}

/******************************************************************************************
Function            :   AT_AllocUserTransID
Description     :       malloc one user transaction id
Called By           :   AT moudle
Data Accessed       :
Data Updated        :
Input           :       VOID
Output          :
Return          :   UINT8
Others          :   build by wangqunyang 2008.06.02
*******************************************************************************************/
UINT16 AT_AllocUserTransID()
{
    UINT16 TempIndex = 0;

    // verify the parameters
    if ((nATUserTransIdSpareHeader <= 0) || (nATUserTransIdSpareTail > AT_USER_TRANS_ID_SIZE - 1) || (nATUserTransIdSpareHeader == nATUserTransIdSpareTail)) // leave at least one node
    {
        AT_TC(g_sw_UTIL, "AT alloc UTI exception");
        return TempIndex;
    }

    // alloc the array element
    TempIndex = nATUserTransIdSpareHeader;
    nATUserTransIdSpareHeader = ATUserTransIdTable[nATUserTransIdSpareHeader].nNextUTISpareTable;
    AT_MemZero(&ATUserTransIdTable[TempIndex], sizeof(AT_UTI_TABLE));

    return TempIndex;
}

/******************************************************************************************
Function            :   AT_FreeUserTransID
Description     :       free one user transaction id
Called By           :   AT moudle
Data Accessed       :
Data Updated        :
Input           :       UINT8 nUTI
Output          :
Return          :   VOID
Others          :   build by wangqunyang 2008.06.02
*******************************************************************************************/
VOID AT_FreeUserTransID(UINT16 nUTI)
{
    // verify the parameters
    if ((nUTI >= AT_USER_TRANS_ID_SIZE) || (ATUserTransIdTable[nUTI].nNextUTISpareTable != 0))
    {
        AT_TC(g_sw_UTIL, "AT free UTI exception");
        return;
    }

    // Free the array element
    ATUserTransIdTable[nATUserTransIdSpareTail].nNextUTISpareTable = nUTI;
    nATUserTransIdSpareTail = nUTI;
    AT_MemZero(&(ATUserTransIdTable[nUTI]), sizeof(AT_UTI_TABLE));
}

UINT32 AT_GetWaitingEventOnDLCI(UINT8 sim, UINT8 dlci)
{
    for (int i = 0; i < AT_WAITING_EVENT_MAX; i++)
    {
        if ((gAT_WaitingEvent[i].enable) &&
            (gAT_WaitingEvent[i].nDLCI == dlci) &&
            (gAT_WaitingEvent[i].nSim == sim))
        {
            ATLOGI("AT get waiting #%d for dlci/%d sim/%d: event/%d ", i, dlci, sim,
                   gAT_WaitingEvent[i].nEvent);
            return gAT_WaitingEvent[i].nEvent;
        }
    }
    ATLOGI("AT fail get waiting for dlci/%d sim/%d ", dlci, sim);
    return 0;
}

VOID AT_AddWaitingEvent(UINT32 event, UINT8 sim, UINT8 dlci)
{
    for (int i = 0; i < AT_WAITING_EVENT_MAX; i++)
    {
        if (gAT_WaitingEvent[i].enable == 0)
        {
            gAT_WaitingEvent[i].nEvent = event;
            gAT_WaitingEvent[i].nSim = sim;
            gAT_WaitingEvent[i].nDLCI = dlci;
            gAT_WaitingEvent[i].enable = 1;
            ATLOGI("AT add waiting to #%d, event/%d sim/%d dlci/%d", i, event, sim, dlci);
            return;
        }
    }
    ATLOGI("AT fail add waiting, event/%d sim/%d dlci/%d", event, sim, dlci);
    return;
}

BOOL AT_isWaitingEvent(UINT32 event, UINT8 sim, UINT8 dlci)
{
    for (int i = 0; i < AT_WAITING_EVENT_MAX; i++)
    {
        if ((gAT_WaitingEvent[i].enable) &&
            (gAT_WaitingEvent[i].nEvent == event) &&
            (gAT_WaitingEvent[i].nSim == sim))
        {
            ATLOGI("AT is waiting #%d, event/%d sim/%d dlci/%d", i, event, sim, dlci);
            return TRUE;
        }
    }
    ATLOGI("AT not waiting, event/%d sim/%d dlci/%d", event, sim, dlci);
    return FALSE;
}

VOID AT_DelWaitingEventOnDLCI(UINT8 dlci)
{
    for (int i = 0; i < AT_WAITING_EVENT_MAX; i++)
    {
        if ((gAT_WaitingEvent[i].enable) && (gAT_WaitingEvent[i].nDLCI == dlci))
        {
            ATLOGI("AT delete waiting #%d, dlci/%d: event/%d sim/%d", i, dlci,
                   gAT_WaitingEvent[i].nEvent, gAT_WaitingEvent[i].nSim);
            gAT_WaitingEvent[i].nEvent = 0;
            gAT_WaitingEvent[i].nSim = 0;
            gAT_WaitingEvent[i].nDLCI = 0;
            gAT_WaitingEvent[i].enable = 0;
            return;
        }
    }
    ATLOGI("AT fail delete waiting, dlci/%d", dlci);
    return;
}

VOID AT_DelWaitingEvent(UINT32 event, UINT8 sim, UINT8 dlci)
{
    for (int i = 0; i < AT_WAITING_EVENT_MAX; i++)
    {
        if ((gAT_WaitingEvent[i].enable) &&
            (gAT_WaitingEvent[i].nEvent == event) &&
            (gAT_WaitingEvent[i].nSim == sim))
        {
            ATLOGI("AT delete waiting #%d, event/%d sim/%d dlci/%d", i, event, sim, dlci);
            gAT_WaitingEvent[i].nEvent = 0;
            gAT_WaitingEvent[i].nSim = 0;
            gAT_WaitingEvent[i].nDLCI = 0;
            gAT_WaitingEvent[i].enable = 0;
            return;
        }
    }
    ATLOGI("AT fail delete waiting, event/%d sim/%d dlci/%d", event, sim, dlci);
    return;
}

UINT8 AT_GetDLCIFromWaitingEvent(UINT32 event, UINT8 sim)
{
    for (int i = 0; i < AT_WAITING_EVENT_MAX; i++)
    {
        if ((gAT_WaitingEvent[i].enable) &&
            (gAT_WaitingEvent[i].nEvent == event) &&
            (gAT_WaitingEvent[i].nSim == sim))
        {
            ATLOGI("AT get waiting #%d, event/%d sim/%d: dlci/%d", i, event, sim,
                   gAT_WaitingEvent[i].nDLCI);
            return gAT_WaitingEvent[i].nDLCI;
        }
    }
    ATLOGI("AT fail get waiting, event/%d sim/%d", event, sim);
    return 0;
}

#if defined(__TTS_FEATURES__) || defined(AT_SOUND_RECORDER_SUPPORT)
UINT8 UnicodeToUCS2Encoding(UINT16 unicode, UINT8 *charLength, UINT8 *arrOut)
{
    UINT8 status = 0;
    UINT8 index = 0;

    if (arrOut != 0)
    {

        if (unicode < 256)
        {
            arrOut[index++] = *((UINT8 *)(&unicode));
            arrOut[index] = 0;
        }
        else
        {
            arrOut[index++] = *((UINT8 *)(&unicode));
            arrOut[index] = *(((UINT8 *)(&unicode)) + 1);
        }
        *charLength = 2;
    }
    else
    {
        status = -1;
    }

    return status;
}

UINT16 AnsiiToUnicodeString(INT8 *pOutBuffer, INT8 *pInBuffer)
{
    INT16 count = -1;
    UINT8 charLen = 0;
    UINT8 arrOut[2];

    while (*pInBuffer != '\0')
    {
        UnicodeToUCS2Encoding((UINT16)*pInBuffer, &charLen, arrOut);
        pOutBuffer[++count] = arrOut[0];
        pOutBuffer[++count] = arrOut[1];
        pInBuffer++;
    }

    pOutBuffer[++count] = '\0';
    pOutBuffer[++count] = '\0';
    return count + 1;
}

#endif
