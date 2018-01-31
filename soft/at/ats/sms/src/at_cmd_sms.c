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
#include "at_module.h"
#include "at_cmd_sms.h"
#include "at_cmd_gc.h"
#include "at_cfg.h"

#include "at_sa.h"
#include "at_common.h"
#include "at_cmd_pbk.h"
#ifdef AT_MODULE_SUPPORT_OTA
#include "ddldefines.h"
#include "jdi_hashalg.h"
#include "jdi_wspdecoder.h"
#include "at_cmd_gprs.h"
#endif

//#include "cfw_sim_cb.h"
// =============================================================================
//
// Define and desclare the variable
//
// =============================================================================

// =============================================================================
//  check number valid befor input data
//  cmgw only use g_cmgw_cmgs save BCD number
// =============================================================================

CFW_TELNUMBER g_cmgw_cmgs =
    {
        .nTelNumber = {0},
        .nType = 0,
        .nSize = 0,
        .padding = {0} // save sms status
};

static UINT8 g_cmgw_cmgs_pdu_length = 0;
static UINT8 g_cmgw_cmgs_pdu_status = 0;

// [+]2007.10.22 for +CIEV
static UINT8 g_SMS_CIND_NewMsg = 0; // [mod]2007.10.30

// =============================================================================
// memfull according to different storage, we support storage
// "ME" and "SM", this gloable variable set flag as following:
// bit 7 6 5 4 3 2 1 0
// bit0  "ME" memfull flag, 0: not full 1: memfull
// bit1  "SM" memfull flag, 0: not full 1: memfull
// bit 2~bit7 reserved for other storage future
// =============================================================================
static UINT8 g_SMS_CIND_MemFull[2] = {
    0x00,
};

// =============================================================================
// CMSS: Recv resp message, for check it ,
// we should record when execute CMSS command
// =============================================================================
static UINT8 g_CMSS_BCD[12] = {0};
static UINT8 g_CMSS_BCDLen = 0;
static UINT8 g_CMSS_StringLen = 0;
static UINT8 g_CMSS_nToDA = 0;
static UINT8 g_CMSS_DA_Flag = 0;
static UINT8 g_CMSS_Index = 0;
static UINT8 g_CMSS_Status = 0;

/* SMS cmd stamp define */
static UINT32 g_SMS_CurCmdStamp = 0;

extern BOOL g_SMSConcat[CFW_SIM_COUNT];
UINT8 g_CmdLine_CMSS_Number = 0; //the number of +CMSS in input CMD line.
UINT8 g_CmdLine_CMSS_Number_Mux[MAX_DLC_NUM] = {
    0,
}; //the number of +CMSS in input CMD line.
UINT32 CFW_CfgGetDefaultSmsParam(
    CFW_SMS_PARAMETER *pInfo,
    CFW_SIM_ID nSimID);
UINT32 CFW_CfgSetDefaultSmsParam(
    CFW_SMS_PARAMETER *pInfo,
    CFW_SIM_ID nSimID);
UINT8 AT_CBS_CheckDCS(UINT8 uDCS);
extern CFW_INIT_INFO cfwInitInfo;
#ifdef __MODEM_LP_MODE_ENABLE__
UINT8 *g_sms_delaybuffer = NULL;
#endif

extern AT_FDN_PBK_LIST *pgATFNDlist[CFW_SIM_COUNT];
BOOL AT_SMS_GetPDUNUM(UINT8 *pPDUData, UINT8 *pPDUNUM, UINT8 *pType, UINT8 *nPDULen);
BOOL AT_SMS_GetSCANUM(UINT8 *pPDUData, UINT8 *pSACNUM, UINT8 *nPDULen);
VOID AT_SMS_RecvCbMessage_Indictation(COS_EVENT *pEvent);

// =============================================================================
//  Function            :   AT_SMS_INIT
//  Description     :       SMS module init procedure
//  Called By           :   SMS module
//  Data Accessed       :
//  Data Updated        :
//  Input           :       void
//  Output          :
//  Return          :   UINT32
//  Others          :
// =============================================================================
HANDLE g_hAtInheritTask = HNULL;
extern UINT32 AT_SMS_INIT_EX(UINT8 nSim);
UINT32 AT_SMS_INIT(UINT8 nSim)
{
#if 0
    //Send message to AT Inherit task    COS_EVENT nEvent = { 0 };
    COS_EVENT nEvent = {0};
    nEvent.nEventId = AT_INHERIT_EV_SMS_INIT;
    nEvent.nParam1 = nSim;
    COS_SendEvent(g_hAtInheritTask, &nEvent, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
#else
    AT_SMS_INIT_EX(nSim);
#endif
    return ERR_SUCCESS;
}

UINT32 AT_SMS_INIT_EX(UINT8 nSim)
{

    hal_HstSendEvent(0x08290000);
    hal_HstSendEvent(hal_TimGetUpTime() * 1000 / 16384);

    /* Define the variable */
    CFW_SMS_PARAMETER sInfo = {0};
    CFW_SMS_STORAGE_INFO sStorageInfo = {0};
    UINT8 nOption = 0;
    UINT8 nNewSmsStorage = 0;
    //UINT8 ReportBuff[32] = {0};
    UINT32 nOperationRet = 0;

    // Just for debug
    AT_TC(g_sw_AT_SMS, "SMS init beginning .......");
    gATCurrentuCmer_ind[nSim] = 2;
    /* set default show text mode parameters, show all header or not */
    nOperationRet = CFW_CfgSetSmsShowTextModeParam(1, nSim);
    if (nOperationRet != ERR_SUCCESS)
    {
        return ERR_AT_CMS_MEM_FAIL;
    }

    /* set defualt value for +ciev, the buffer overflow, notify to AT or not */
    nOperationRet = CFW_CfgSetSmsOverflowInd(1, nSim);
    if (nOperationRet != ERR_SUCCESS)
    {
        return ERR_AT_CMS_MEM_FAIL;
    }

    // ====================================================
    // update SMS parameters, we first get the current value ,then
    // modify the segments we needed.
    // ====================================================

    nOperationRet = CFW_CfgGetSmsParam(&sInfo, 0, nSim);
    if (nOperationRet != ERR_SUCCESS)
    {
        return ERR_AT_CMS_MEM_FAIL;
    }

// ====================================================
// modify the segments we need to change in default value
// according to the 3GPP 27.005 the default value is:
// "fo" = 17, "vp" = 167, "dcs" = 0, "pid" = 0
// ====================================================
// ====================================================
// "fo" composition :
// bit    7    6      5     4      3     2    1     0
// RP UDHI SSR VPF  VPF  RD MTI MTI
// ====================================================
#if 0
    sInfo.rp   = (17 & 0x80) >> 7;
    sInfo.udhi = (17 & 0x40) >> 6;
#endif
    sInfo.ssr = (17 & 0x20) >> 5;

#if 0
    sInfo.vpf = (17 & 0x18) >> 3;
    sInfo.rd  = (17 & 0x04) >> 2;
    sInfo.mti = 17 & 0x03;
#endif

    sInfo.vp = 167;
    sInfo.dcs = 0;
    sInfo.pid = 0;

#if 0
    /* set param location */
    sInfo.nIndex         = 0; // the first profile of parameters
    sInfo.nSaveToSimFlag = 0x00;  // set param to mobile flash , sync procedure calling
    sInfo.bDefault       = TRUE;  // as current parameters

    /* get the UTI and free it after finished calling */
    if (0 == (nUTI = AT_AllocUserTransID()))
    {
        return ERR_AT_CMS_MEM_FAIL;
    }

    /* set sms parameters request, sync procedure calling */
    nOperationRet = CFW_CfgSetSmsParam(&sInfo, nUTI);
    AT_FreeUserTransID(nUTI);
#else

    nOperationRet = CFW_CfgSetSmsParam(&sInfo, 0, nSim);
#endif

    /* asyn procedure calling */
    if (ERR_SUCCESS != nOperationRet)
    {
        return ERR_AT_CMS_MEM_FAIL;
    }
    // ====================================================
    //  status report value, get the current value and
    //  then update we needed
    // ====================================================

    if (ERR_SUCCESS != CFW_CfgGetNewSmsOption(&nOption, &nNewSmsStorage, nSim))
    {
        return ERR_AT_CMS_MEM_FAIL;
    }
    if (ERR_SUCCESS != CFW_CfgSetNewSmsOption(nOption | CFW_SMS_ROUT_DETAIL_INFO, gATCurrentAtSMSInfo[nSim].nStorage3, nSim))
    {
        return ERR_AT_CMS_MEM_FAIL;
    }

    //===========================================
    /* support multi language                                   */
    //===========================================
    ML_Init();

    // ====================================================
    // get SM storage info and save the init value
    // ====================================================

    if (ERR_SUCCESS != CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSim))
    {
        return ERR_AT_CMS_MEM_FAIL;
    }

    /* SM is default storage of Mobile */
    gATCurrentAtSMSInfo[nSim].nTotal1 = gATCurrentAtSMSInfo[nSim].nTotal2 = gATCurrentAtSMSInfo[nSim].nTotal3 = sStorageInfo.totalSlot;
    gATCurrentAtSMSInfo[nSim].nUsed1 = gATCurrentAtSMSInfo[nSim].nUsed2 = gATCurrentAtSMSInfo[nSim].nUsed3 = sStorageInfo.usedSlot;

    /* if SM storage buffer is full and set flag */
    if ((sStorageInfo.totalSlot == sStorageInfo.usedSlot) && (sStorageInfo.usedSlot != 0))
    {
        /* SM storage is full, set bit1 is 1 */
        g_SMS_CIND_MemFull[nSim] |= CFW_SMS_STORAGE_SM;
    }

    if (sStorageInfo.unReadRecords != 0)
    {
        g_SMS_CIND_NewMsg += sStorageInfo.unReadRecords;
    }

    // ====================================================
    // ME storage info and save the init value
    // ====================================================

    if (ERR_SUCCESS != CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_ME, nSim))
    {
        return ERR_AT_CMS_MEM_FAIL;
    }

    /* ME storage buffer is full and set flag */
    if ((sStorageInfo.totalSlot == sStorageInfo.usedSlot) && (sStorageInfo.usedSlot != 0))
    {
        /* ME storage is full, set bit0 is 1 */
        g_SMS_CIND_MemFull[nSim] |= CFW_SMS_STORAGE_ME;
    }

    if (sStorageInfo.unReadRecords != 0) // [+]for cind command new message 2007.11.05
    {
        g_SMS_CIND_NewMsg += sStorageInfo.unReadRecords;
    }

    AT_TC(g_sw_AT_SMS, ("We Got gATCurrentuCmer_ind nSim[%d] to %d\n"), nSim, gATCurrentuCmer_ind[nSim]);
#if 0
    // ====================================================
    //  Report to CMER, if necessary
    // ====================================================
    if (gATCurrentuCmer_ind[nSim])
    {
        /* if SM and ME storage bit flag are both "1", bit0 = 1 and bit1 =1 , then the SMSFULL is 1 */
        AT_Sprintf(ReportBuff, "+CIEV: \"SMSFULL\",%u",
                   ((CFW_SMS_STORAGE_SM == (g_SMS_CIND_MemFull[nSim] & CFW_SMS_STORAGE_SM))
                    && (CFW_SMS_STORAGE_ME == (g_SMS_CIND_MemFull[nSim] & CFW_SMS_STORAGE_ME))) ? 1 : 0);
        AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, ReportBuff, (UINT16)AT_StrLen(ReportBuff), 0, nSim);
        AT_MemZero(ReportBuff, 32);
        AT_Sprintf(ReportBuff, "+CIEV: \"MESSAGE\",%u", g_SMS_CIND_NewMsg ? 1 : 0);
        AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, ReportBuff, (UINT16)AT_StrLen(ReportBuff), 0, nSim);
    }
#endif
    // Just for debug
    AT_TC(g_sw_AT_SMS, "SMS init OK!");
    hal_HstSendEvent(0x08290001);
    hal_HstSendEvent(hal_TimGetUpTime() * 1000 / 16384);

    return ERR_SUCCESS;
}

/******************************************************************************************
Function            :   AT_SMS_AsyncEventProcess
Description     :       async event process function
Called By           :   AT SMS module
Data Accessed       :
Data Updated        :
Input           :       COS_EVENT *pEvent
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/
extern BOOL gATSATSendSMSFlag[];
VOID AT_SMS_AsyncEventProcess(COS_EVENT *pEvent)
{

    // =============================================================
    //
    // In order to display event the SMS received in this function, add trace of which event recieved.
    // add by wangqunyang 2008.04.08
    //
    // =============================================================

    AT_ASSERT(NULL != pEvent);
    CFW_EVENT CfwEvent = {0};

    AT_CosEvent2CfwEvent(pEvent, &CfwEvent);

    switch (pEvent->nEventId)
    {

    case EV_CFW_SMS_SEND_MESSAGE_RSP:

        AT_TC(g_sw_AT_SMS, " AT SMS Get Async Event:  EV_CFW_SMS_SEND_MESSAGE_RSP\n ");
        if (AT_IsAsynCmdAvailable("+CMGS", g_SMS_CurCmdStamp, HIUINT16(pEvent->nParam3)) || AT_IsAsynCmdAvailable("+CMSS", g_SMS_CurCmdStamp, HIUINT16(pEvent->nParam3)))
        {
            AT_SMS_CMGS_CMSS_SendMessage_rsp(pEvent);
        }
        else if (gATSATSendSMSFlag[CfwEvent.nFlag])
        {

            if (0 == CfwEvent.nType)
            {
                CFW_SatResponse(0x13, 0x00, 0x00, NULL, 0x00, 12, CfwEvent.nFlag);
            }
            else
            {
                CFW_SatResponse(0x13, 0x35, 0x00, NULL, 0x00, 12, CfwEvent.nFlag);
            }
            //gATSATSendSMSFlag[CfwEvent.nFlag] = FALSE;
        }
        else
        {
            AT_TC(g_sw_AT_SMS, " SMS recieved event, and not for us, discard it !");
        }

        break;

    case EV_CFW_SMS_DELETE_MESSAGE_RSP:
        AT_TC(g_sw_AT_SMS, " AT SMS Get Async Event:  EV_CFW_SMS_DELETE_MESSAGE_RSP\n ");
        if (AT_IsAsynCmdAvailable("+CMGD", g_SMS_CurCmdStamp, HIUINT16(pEvent->nParam3)))
        {
            AT_SMS_DeleteMessage_rsp(pEvent);
        }
        else
        {
            AT_TC(g_sw_AT_SMS, " SMS recieved event, and not for us, discard it !");
        }

        break;

    case EV_CFW_SMS_READ_MESSAGE_RSP:

        AT_TC(g_sw_AT_SMS, " AT SMS Get Async Event:  EV_CFW_SMS_READ_MESSAGE_RSP\n ");
        CFW_EVENT nCfwEvent = {0};
        nCfwEvent.nFlag = CfwEvent.nFlag;

        BOOL bReturn = AT_GetCfwInitSmsInfo(&nCfwEvent);
        if ((bReturn == 0) || (nCfwEvent.nParam1 == 1)) // me init not finished
        //if (bReturn == 0)  // me init not finished
        {
            AT_SMS_MOInit_ReadMessage_rsp(pEvent);
        }

        if (AT_IsAsynCmdAvailable("+CMGR", g_SMS_CurCmdStamp, HIUINT16(pEvent->nParam3)))
        {
            AT_SMS_CMGR_ReadMessage_rsp(pEvent);
        }
        else if (AT_IsAsynCmdAvailable("+CMSS", g_SMS_CurCmdStamp, HIUINT16(pEvent->nParam3)))
        {
            AT_SMS_CMSS_ReadMessage_rsp(pEvent);
        }
        else
        {
            AT_TC(g_sw_AT_SMS, " SMS recieved event, and not for us, discard it !");
        }

        break;

    case EV_CFW_SMS_WRITE_MESSAGE_RSP:

        AT_TC(g_sw_AT_SMS, " AT SMS Get Async Event:  EV_CFW_SMS_WRITE_MESSAGE_RSP\n ");
        if (AT_IsAsynCmdAvailable("+CMGW", g_SMS_CurCmdStamp, HIUINT16(pEvent->nParam3)))
        {
            AT_SMS_WriteMessage_rsp(pEvent);
        }
        else
        {
            AT_TC(g_sw_AT_SMS, " SMS recieved event, and not for us, discard it !");
        }

        break;

    case EV_CFW_SMS_LIST_MESSAGE_RSP:

        AT_TC(g_sw_AT_SMS, " AT SMS Get Async Event:  EV_CFW_SMS_LIST_MESSAGE_RSP\n ");
        if (AT_IsAsynCmdAvailable("+CMGL", g_SMS_CurCmdStamp, HIUINT16(pEvent->nParam3)))
        {
            AT_SMS_ListMessage_rsp(pEvent);
        }
        else
        {
            AT_TC(g_sw_AT_SMS, " SMS recieved event, and not for us, discard it !");
        }

        break;

    case EV_CFW_NEW_SMS_IND:
        AT_TC(g_sw_AT_SMS, " AT SMS Get Async Event:  EV_CFW_NEW_SMS_IND\n ");
        AT_SMS_RecvNewMessage_Indictation(pEvent);
        break;

    case EV_CFW_CB_MSG_IND:
    case EV_CFW_CB_PAGE_IND:
        AT_SMS_RecvCbMessage_Indictation(pEvent);
        break;

    case EV_CFW_SMS_INFO_IND:

        AT_TC(g_sw_AT_SMS, " AT SMS Get Async Event:  EV_CFW_SMS_INFO_IND\n ");
        AT_SMS_RecvBuffOverflow_Indictation(pEvent);
        break;
#ifdef AT_MODULE_SUPPORT_OTA
    case EV_CFW_NEW_EMS_IND:
        AT_TC(g_sw_AT_SMS, "AT_WY AT_SMS_AsyncEventProcess");

        AT_SMS_NewPushMessage(pEvent);
        break;
#endif
    default:

        AT_TC(g_sw_AT_SMS, " AT SMS Get Async Event:  Unknown EventId = %u\n ", pEvent->nEventId);
        break;
    }
    return;
}

/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CMSS
Description     :       CMSS procedure function
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by wangqunyang 2008.04.07
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CMSS(AT_CMD_PARA *pParam)
{
    CFW_EVENT nCfwEvent = {
        0,
    };
    CFW_SMS_STORAGE_INFO sStorageInfo = {0};

    UINT8 BCDDataBuff[12] = {0};
    UINT8 BCDDataLength = 0;

    UINT8 DialNumberBuff[TEL_NUMBER_MAX_LEN] = {0};
    UINT16 DialNumberLen = 24;
    UINT8 nDialNumType = 0;
    UINT16 nParamLen = 0;
    UINT8 nParamCount = 0;
    UINT16 nIndex = 0;
    UINT32 nOperationRet = 0;
    BOOL bReturn;
    BOOL bIsInternational = FALSE;
    UINT16 nUTI = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    nCfwEvent.nFlag = nSim;

    AT_ASSERT(NULL != pParam);

    g_SMS_CurCmdStamp = pParam->uCmdStamp;

    // Reset the record of SMSS variable
    AT_MemZero(g_CMSS_BCD, 12);
    g_CMSS_BCDLen = 0;
    g_CMSS_StringLen = 0;
    g_CMSS_DA_Flag = 0;
    g_CMSS_nToDA = 0;
    g_CMSS_Index = 0;
    g_CMSS_Status = 0;

    bReturn = AT_GetCfwInitSmsInfo(&nCfwEvent);

    if ((bReturn == 0) || (nCfwEvent.nParam1 == 1)) // me init not finished
    {
        AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmss: AT_GetCfwInitSmsInfo() failure", pParam->nDLCI, nSim);
        return;
    }

    /***********************************************/
    // Operation is set mode
    /***********************************************/
    if (pParam->iType == AT_CMD_SET)
    {
        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);
        if ((nOperationRet != ERR_SUCCESS) || (nParamCount == 0) || (nParamCount > 3)) // [mod]2007.111.20 max param count = 3
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmss: get param counter error", pParam->nDLCI, nSim);
            return;
        }

        //===========================================
        // process different part according to para counts
        //===========================================
        if (1 == nParamCount)
            goto CMSS_label_para1;

        if (2 == nParamCount)
            goto CMSS_label_para2;

        if (3 == nParamCount)
            goto CMSS_label_para3;

    CMSS_label_para3:
        //===========================================
        // para 3
        //===========================================
        AT_TC(g_sw_AT_SMS, "CMSS==== para3");
        nParamLen = 1;
        nOperationRet =
            AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT8, (void *)(&nDialNumType), &nParamLen);

        /* Dial number can omit and set default value */
        if (ERR_AT_UTIL_CMD_PARA_NULL == nOperationRet)
        {
            nDialNumType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
        }
        else
        {
            if (nOperationRet != ERR_SUCCESS)
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmss: get param 3 error", pParam->nDLCI, nSim);
                return;
            }
        }

        if ((nDialNumType != CFW_TELNUMBER_TYPE_INTERNATIONAL) && (nDialNumType != CFW_TELNUMBER_TYPE_NATIONAL) && (nDialNumType != CFW_TELNUMBER_TYPE_UNKNOWN))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmss: dial number wrong type", pParam->nDLCI, nSim);
            return;
        }

    CMSS_label_para2:
        //===========================================
        // para 2
        //===========================================
        AT_TC(g_sw_AT_SMS, "CMSS === para2");
        nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, DialNumberBuff, &DialNumberLen);

        if ((ERR_SUCCESS != nOperationRet) || (0 == DialNumberLen))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmss: get param 2 error", pParam->nDLCI, nSim);
            return;
        }

        if (!AT_SMS_IsValidPhoneNumber(DialNumberBuff, DialNumberLen, &bIsInternational))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmss: nuber invalid error!", pParam->nDLCI, nSim);
            return;
        }

        // Set user input data to handle
        if (bIsInternational)
        {
            DialNumberLen--;
            if (DialNumberLen > 20)
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "cmss with '+': dial num len too long", pParam->nDLCI, nSim);
                return;
            }

            BCDDataLength = SUL_AsciiToGsmBcd(&DialNumberBuff[1], DialNumberLen, BCDDataBuff);
        }
        else
        {
            if (DialNumberLen > 20)
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "cmss without '+': dial num len too long", pParam->nDLCI,
                                  nSim);
                return;
            }
            BCDDataLength = SUL_AsciiToGsmBcd(DialNumberBuff, DialNumberLen, BCDDataBuff);
        }

        // mark the flag : dial number is input:
        g_CMSS_DA_Flag = 1;

    CMSS_label_para1:
        //===========================================
        // para 1
        //===========================================
        AT_TC(g_sw_AT_SMS, "CMSS==== para1");
        nParamLen = 2;
        nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT16, (void *)(&nIndex), &nParamLen);
        if ((nOperationRet != ERR_SUCCESS) || (0 == nIndex))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmss: get param 1 error", pParam->nDLCI, nSim);
            return;
        }

        // From storage2's message
        nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, gATCurrentAtSMSInfo[nSim].nStorage2, nSim);
        if ((ERR_SUCCESS != nOperationRet) || (nIndex > sStorageInfo.totalSlot))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_MEM_INDEX, NULL, 0, "cmss: index > total slot", pParam->nDLCI, nSim);
            return;
        }

        /* according to spec, we check dial number first */
        if (bIsInternational)
        {
            nDialNumType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
        }
        else
        {
            if (0 == nDialNumType)
            {
                /* no input num type  */
                nDialNumType = CFW_TELNUMBER_TYPE_UNKNOWN;
            }
            else if (CFW_TELNUMBER_TYPE_INTERNATIONAL == nDialNumType)
            {
                /* have no '+' , but the num type is 145, replace 129 with is  */
                nDialNumType = CFW_TELNUMBER_TYPE_UNKNOWN;
            }
            else
            {
                /* get the input num type  */
            }
        }

        /*****************************************************/
        // Record the CMSS data and reference by cmss read msg resp
        /*****************************************************/
        AT_MemCpy(g_CMSS_BCD, BCDDataBuff, BCDDataLength);
        g_CMSS_nToDA = nDialNumType;
        g_CMSS_BCDLen = BCDDataLength;
        g_CMSS_StringLen = DialNumberLen;
        g_CMSS_Index = nIndex;

        /* get the UTI and free it after finished calling */
        if (0 == (nUTI = AT_AllocUserTransID()))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmss: malloc UTI error", pParam->nDLCI, nSim);
            return;
        }

        if (0) // (bMuxActiveStatus)
        {
            if (g_SMSConcat[nSim] && !(--g_CmdLine_CMSS_Number_Mux[pParam->nDLCI]))
            {
                g_SMSConcat[nSim] = FALSE;
            }
        }
        else
        {
            if (g_SMSConcat[nSim] && !(--g_CmdLine_CMSS_Number))
            {
                g_SMSConcat[nSim] = FALSE;
            }
        }
        UINT8 nStorageToRead = gATCurrentAtSMSInfo[nSim].nStorage2;
        if (CFW_SMS_STORAGE_MT == gATCurrentAtSMSInfo[nSim].nStorage2)
            nStorageToRead = CFW_SMS_STORAGE_SM;

        nOperationRet = CFW_SmsReadMessage(nStorageToRead, CFW_SMS_TYPE_PP, nIndex, pParam->nDLCI, nSim);
        AT_FreeUserTransID(nUTI);
        if (nOperationRet == ERR_SUCCESS)
        {
            AT_SMS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_CR, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            nOperationRet = AT_SMS_ERR_Change(nOperationRet);
            AT_SMS_Result_Err(nOperationRet, NULL, 0, "cmss: check read message param exception", pParam->nDLCI, nSim);
            return;
        }
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, "cmss: wrong cmd mode", pParam->nDLCI, nSim);
        return;
    }
    return;
}

const U8 ATExtendedAsciiToDefaultArray[] =
    {
        0, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 27, 32, 32,
        32, 32, 10, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 60, 47, 62, 20, 32, 32, 32, 32, 32,
        32, 101, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 40, 64, 41, 61, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 101, 32, 32, 32, 32, 32, 32, 32, /*  Changed character at 162 to 101 (old value was 32)  */
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32};
const U8 ATAsciiToDefaultArray[] =
    {
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        10, 32, 32, 13, 32, 32, 16, 32, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 27, 32, 32,
        32, 32, 32, 33, 34, 35, 2, 37, 38, 39,
        40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
        50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
        60, 61, 62, 63, 0, 65, 66, 67, 68, 69,
        70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
        80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
        90, 32, 32, 32, 32, 17, 32, 97, 98, 99,
        100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
        110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
        120, 121, 122, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 64, 32, 1, 36, 3, 32, 95, 32, 32,
        32, 32, 32, 64, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 96, 32, 32, 32, 32, 91, 14, 28, 9,
        31, 32, 32, 32, 32, 32, 32, 32, 32, 93,
        32, 32, 32, 32, 92, 32, 11, 32, 32, 32,
        94, 32, 32, 30, 127, 32, 32, 32, 123, 15,
        29, 32, 4, 5, 32, 32, 7, 32, 32, 32,
        32, 125, 8, 32, 32, 32, 124, 32, 12, 6,
        32, 32, 126, 32, 32, 32};

const U8 ATDefaultToAsciiArray[128] =
    {
        64, 163, 36, 165, 232, 233, 249, 236, 242, 199,
        10, 216, 248, 13, 197, 229, 16, 95, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 27, 198, 230,
        223, 200, 32, 33, 34, 35, 164, 37, 38, 39,
        40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
        50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
        60, 61, 62, 63, 161, 65, 66, 67, 68, 69,
        70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
        80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
        90, 196, 214, 209, 220, 167, 191, 97, 98, 99,
        100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
        110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
        120, 121, 122, 228, 246, 241, 252, 224};
const U8 ATDefaultToExtendedAsciiArray[128] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        32, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* Page Break at 10=32 Change Later*/
        94, 0, 0, 0, 0, 0, 0, 32, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        123, 125, 0, 0, 0, 0, 0, 92, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        91, 126, 93, 0, 124, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 162, 0, 0, 0, 0, 0, 0, 0, 0, /*  Euro character at 101 changed to 162 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0};

#define AT_TEST_GSM_EXTENDED(c) \
    ((c == 0xA2) || (c == '^') || (c == '{') || (c == '}') || (c == '\\') || (c == '[') || (c == '~') || (c == ']') || (c == '|'))

void ATConvertAsciiEncodingToGSM7BitDefault(U8 *message, U8 *msg, U16 length, U16 *outLen)
{
    U16 i, j;
    for (i = 0, j = 0; i < length; i++, j++)
    {
        if (AT_TEST_GSM_EXTENDED(msg[i]))
        {
            if (i > 0 && msg[i - 1] == 0x1b)
            {
                message[j] = ATExtendedAsciiToDefaultArray[msg[i]];
            }
            else
            {
                message[j++] = 0x1b;
                message[j] = ATExtendedAsciiToDefaultArray[msg[i]];
            }
        }
        else
        {
            message[j] = ATAsciiToDefaultArray[msg[i]];
        }
    }
    if (outLen != NULL)
        *outLen = j;
}

void ATConvertGSM7BitDefaultEncodingToAscii(U8 *message, U8 *msg, U16 length, U16 *outLen)
{
    U16 i, j = 0;
    for (i = 0; i < length; i++)
    {
        if (msg[i] != 27) //escape char ?
        {
            // if(msg[i]!=13)   //CR ?
            message[j++] = ATDefaultToAsciiArray[msg[i]];
        }
        else
        {
            //treat escape char as a space
            //message[j++] = 32;
            //if current char is escape, then mean next char is extend ascii char
            i++;
            message[j++] = ATDefaultToExtendedAsciiArray[msg[i]];
        }
    }
    if (outLen != NULL)
        *outLen = j;
}
UINT32 CFW_SmsSendSetting()
{
    CFW_SMS_PARAMETER sInfo;

    SUL_ZeroMemory8(&sInfo, SIZEOF(CFW_SMS_PARAMETER));
    CFW_CfgGetSmsParam(&sInfo, 0, 0);
    sInfo.bearer = 0; //GSM Only.
    sInfo.ssr = 1;    //A status report is requested.
    sInfo.vp = 0;
    sInfo.dcs = 8; //0:gsm 7bit, 8:ucs2
    sInfo.mti = 1;
    sInfo.pid = 0;
    sInfo.rp = 0;
    CFW_CfgSetSmsParam(&sInfo, 0, 0);
    return ERR_SUCCESS;
}

UINT32 CFW_SmsSendPresetMessage(UINT8 *pSendNum, UINT8 *pSendData, UINT8 nUTI, UINT8 nSim)
{
    UINT8 sTmpNumber[50];
    UINT16 nDataSize = 0x0;
    UINT32 ret = 0x0;
    UINT8 nFormat = 0x0, nNumSize = 0x0, nTmpSize = 0x0;
    CFW_DIALNUMBER sNumber;
    CFW_SMS_PARAMETER sInfo;

    AT_TC(g_sw_AT_SMS, "CFW_SmsSendPresetMessage pSendNum=%s, pSendData=%s ", pSendNum, pSendData);
    SUL_ZeroMemory8(&sTmpNumber, 50);
    SUL_ZeroMemory8(&sInfo, SIZEOF(CFW_SMS_PARAMETER));
    SUL_ZeroMemory8(&sNumber, SIZEOF(CFW_DIALNUMBER));

    CFW_SmsSendSetting();
    CFW_CfgGetSmsFormat(&nFormat, nSim);
    nTmpSize = (UINT8)SUL_Strlen(pSendNum);
    SUL_AsciiToGsmBcd(pSendNum, nTmpSize, sTmpNumber);
    nNumSize = nTmpSize / 2 + nTmpSize % 2;

    sNumber.nDialNumberSize = (UINT8)nNumSize;
    sNumber.pDialNumber = (UINT8 *)CSW_Malloc(nNumSize);
    sNumber.nType = CFW_TELNUMBER_TYPE_UNKNOWN; //0x81
    if (sNumber.pDialNumber == NULL)
    {
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory8(sNumber.pDialNumber, nNumSize);
    SUL_MemCopy8(sNumber.pDialNumber, sTmpNumber, nNumSize);
    CFW_CfgSetSmsFormat(1, nSim);
    CFW_CfgGetSmsParam(&sInfo, 0, nSim);
    nDataSize = (UINT16)SUL_Strlen(pSendData);
    TS_OutputText(CFW_SMS_TS_ID, "Current DCS =  %d\n ", sInfo.dcs);
    if (sInfo.dcs == 8)
    {
        ret = CFW_SmsSendMessage(&sNumber, pSendData, 6, nUTI, nSim);
    }
    else
    {
        ret = CFW_SmsSendMessage(&sNumber, pSendData, nDataSize, nUTI, nSim);
    }
    CFW_CfgSetSmsFormat(nFormat, nSim);
    return ERR_SUCCESS;
}

/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CMGS
Description     :       CMGS procedure function
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CMGS(AT_CMD_PARA *pParam)
{
    UINT8 *pSmsSendData = NULL;
    UINT32 nSmsSendDatalen = 0;

    UINT8 *PduData = NULL;
    UINT8 *pduaddsca = NULL;

    CFW_DIALNUMBER Number = {0};
    CFW_SMS_PARAMETER sInfoForChinese = {0};

    UINT8 nParamCount = 0;
    UINT8 nFormat = 0;
    //UINT8 PromptMsg[8] = {0};
    UINT8 DialNumberBuff[TEL_NUMBER_MAX_LEN] = {0};
    UINT16 DialNumberLen = 24;
    UINT8 nDialNumType = 0;
    UINT16 nParamLen = 0;

    UINT8 *pUCS2Data = NULL;

    UINT8 BCDDataBuff[TEL_NUMBER_MAX_LEN] = {0};
    UINT8 BCDDataLength = 0;

    UINT8 nPduTotalDataLen = 0;
    UINT16 pdulen;
    UINT32 nUCS2DataSize = 0;
    UINT16 nRetGetChineseInfo = 0;

    UINT32 nOperationRet = ERR_SUCCESS;
    UINT32 nRetSendMsg = ERR_SUCCESS;

    BOOL bIsInternational = FALSE;
    UINT16 nUTI = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_ASSERT(NULL != pParam);

    // Get cmd stamp
    g_SMS_CurCmdStamp = pParam->uCmdStamp;

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        /***************************************
            ** Prompt input send message data
            ***************************************/
        if (!pParam->iExDataLen)
        {
            nFormat = gATCurrentAtSmsSettingSg_SMSFormat;

            // get param count
            nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);

            // nOperationRet is failure, or nFormat is not 0 or 1
            if ((nOperationRet != ERR_SUCCESS) || (nParamCount > 2) || (!((nFormat == 0) || (nFormat == 1))))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmgs text mode: get param counter error ", pParam->nDLCI,
                                  nSim);
                return;
            }

            //===========================================
            // text mode prompt
            //===========================================
            if (nFormat == 1)
            {
                // First reset the value
                memset(&g_cmgw_cmgs, 0, sizeof(CFW_TELNUMBER)); // text mode

                // //////////////////////
                // param 1
                // //////////////////////
                if (nParamCount > 0)
                {
                    // get the first param, da in string mode
                    nOperationRet =
                        AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, DialNumberBuff, &DialNumberLen);
                    if (nOperationRet != ERR_SUCCESS)
                    {
                        AT_SMS_Result_Err(nOperationRet, NULL, 0, "cmgs text mode: get param 1 error ", pParam->nDLCI, nSim);
                        return;
                    }

                    // number is empty return error
                    if (DialNumberLen == 0)
                    {
                        AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgs text mode: DialNumberLen==0", pParam->nDLCI,
                                          nSim);
                        return;
                    }
                    if (!AT_SMS_IsValidPhoneNumber(DialNumberBuff, DialNumberLen, &bIsInternational))
                    {
                        AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_CHAR, NULL, 0, "cmgs text mode: number invalid", pParam->nDLCI,
                                          nSim);
                        return;
                    }

                    // Internal format of tel num
                    if (bIsInternational)
                    {
                        DialNumberLen--;
                        if (DialNumberLen > 20)
                        {
                            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "cmgs text mode with '+': number len too long",
                                              pParam->nDLCI, nSim);
                            return;
                        }

                        BCDDataLength = SUL_AsciiToGsmBcd(&DialNumberBuff[1], DialNumberLen, BCDDataBuff);

                        g_cmgw_cmgs.nSize = BCDDataLength;
                        AT_MemCpy(g_cmgw_cmgs.nTelNumber, BCDDataBuff, BCDDataLength);
                    }

                    // national format tel num
                    else
                    {
                        if (DialNumberLen > 20)
                        {
                            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "cmgs text mode without '+': number len too long",
                                              pParam->nDLCI, nSim);
                            return;
                        }
                        BCDDataLength = SUL_AsciiToGsmBcd(DialNumberBuff, DialNumberLen, BCDDataBuff);

                        g_cmgw_cmgs.nSize = BCDDataLength;
                        AT_MemCpy(g_cmgw_cmgs.nTelNumber, BCDDataBuff, BCDDataLength);
                    }
                }

                // param 2
                if (nParamCount > 1)
                {
                    nParamLen = 1;
                    nOperationRet =
                        AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, (void *)(&nDialNumType), &nParamLen);

                    /* Dial number can omit and set default value */
                    if (ERR_AT_UTIL_CMD_PARA_NULL == nOperationRet)
                    {
                        nDialNumType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
                    }
                    else
                    {
                        if (nOperationRet != ERR_SUCCESS)
                        {
                            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgs text mode: get param 2 error",
                                              pParam->nDLCI, nSim);
                            return;
                        }
                    }

                    if ((nDialNumType != CFW_TELNUMBER_TYPE_INTERNATIONAL) && (nDialNumType != CFW_TELNUMBER_TYPE_NATIONAL) && (nDialNumType != CFW_TELNUMBER_TYPE_UNKNOWN))
                    {
                        AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgs text mode: dial number type error",
                                          pParam->nDLCI, nSim);
                        return;
                    }

                    // set cmgs dial num type
                    g_cmgw_cmgs.nType = nDialNumType;
                }

                /* according to spec, we check dial number first */
                if (bIsInternational)
                {
                    nDialNumType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
                }
                else
                {
                    if (0 == nDialNumType)
                    {
                        /* no input num type  */
                        nDialNumType = CFW_TELNUMBER_TYPE_UNKNOWN;
                    }
                    else if (CFW_TELNUMBER_TYPE_INTERNATIONAL == nDialNumType)
                    {
                        /* have no '+' , but the num type is 145, replace 129 with is  */
                        nDialNumType = CFW_TELNUMBER_TYPE_UNKNOWN;
                    }
                    else
                    {
                        /* get the input num type  */
                    }
                }
                g_cmgw_cmgs.nType = nDialNumType;
            }

            // pdu mode prompt
            if (nFormat == 0)
            {
                g_cmgw_cmgs_pdu_length = 0;
                if (nParamCount > 1)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0, "cmgs pdu mode: get param counter error",
                                      pParam->nDLCI, nSim);
                    return;
                }

                nParamLen = 1;
                nOperationRet =
                    AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, (void *)(&nPduTotalDataLen), &nParamLen);
                AT_TC(g_sw_AT_SMS, "CMGS GetPara: 0x%x", nOperationRet);

                if (nOperationRet != ERR_SUCCESS)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0, "cmgs pdu mode: get param error", pParam->nDLCI,
                                      nSim);
                    return;
                }

                // Pdu total data length is valid or not
                if ((nPduTotalDataLen > 164) || (nPduTotalDataLen == 0))
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0, "cmgs pdu mode: pdu len too long", pParam->nDLCI,
                                      nSim);
                    return;
                }
                g_cmgw_cmgs_pdu_length = nPduTotalDataLen;
            }

            // display in the screen, and need input
            at_CmdRespOutputPrompt(pParam->engine);

            /* time  waiting for input sms data , set 180s now, if update we can change it */
            AT_SMS_Result_OK(CMD_FUNC_WAIT_SMS, CMD_RC_OK, 180, NULL, 0, pParam->nDLCI, nSim);
            return;
        }

        /*******************************************
            **  with msg body,input data process is starting
            *******************************************/
        else
        {
            if (pParam->pExData[pParam->iExDataLen - 1] == 27) // end with esc, cancel send
            {
                /* if escape , reset gloable variable */
                AT_MemSet(&g_cmgw_cmgs, 0, sizeof(CFW_TELNUMBER));
                AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
                return;
            }
            else if (pParam->pExData[pParam->iExDataLen - 1] != 26) // not end with ctl+z, err happen
            //else if( (pParam->pExData[pParam->iExDataLen - 1] != 26)&&(pParam->pExData[pParam->iExDataLen - 1] != 13))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmgs data: not with 'ctr+Z' ", pParam->nDLCI, nSim);
                return;
            }
            nFormat = gATCurrentAtSmsSettingSg_SMSFormat;

            // text mode procedure and send message
            if (nFormat)
            {
                BOOL bParaVerifyErrorFlag = FALSE;

                nRetGetChineseInfo = CFW_CfgGetSmsParam(&sInfoForChinese, 0, nSim);
                if (nRetGetChineseInfo != ERR_SUCCESS)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmgs text mode data: get sms prarameters error",
                                      pParam->nDLCI, nSim);
                    return;
                }

                /* Just for debug */
                AT_TC(g_sw_AT_SMS, "cmgs text mode data: pParam->iExDataLen = %u", pParam->iExDataLen);

                AT_TC(g_sw_AT_SMS, "sInfoForChinese.dcs = %d", sInfoForChinese.dcs);

                /* check the dcs value */
                if (sInfoForChinese.dcs == 0)
                {
                    /*if (pParam->iExDataLen - 1 > 160)
                    {
                        bParaVerifyErrorFlag = TRUE;
                    }*/
                    nSmsSendDatalen = pParam->iExDataLen - 1;
                }
                else if (sInfoForChinese.dcs == 4)
                {
                    /*if (pParam->iExDataLen - 1 > 140)
                    {
                        bParaVerifyErrorFlag = TRUE;
                    }*/

                    nSmsSendDatalen = pParam->iExDataLen - 1;
                }
                else if (sInfoForChinese.dcs == 8) // This for chinese message
                {
                    UINT32 UCSlength = 0;
                    INT32 nLocal2UCSReturn = ERR_SUCCESS;

                    UCSlength = SUL_CharacterLen(pParam->pExData, pParam->iExDataLen - 1);

                    if (UCSlength == -1 /*|| UCSlength - 1 > 70*/)
                    {
                        bParaVerifyErrorFlag = TRUE;
                    }
                    else
                    {
                        /* we must use uniform transport mode: big ending */
                        AT_Set_MultiLanguage();
                        nLocal2UCSReturn =
                            ML_LocalLanguage2Unicode(pParam->pExData, pParam->iExDataLen - 1, &pUCS2Data, &nUCS2DataSize, NULL);

                        if (ERR_SUCCESS != nLocal2UCSReturn)
                        {
                            nLocal2UCSReturn = AT_SMS_ERR_Change(nLocal2UCSReturn);
                            AT_SMS_Result_Err(nLocal2UCSReturn, NULL, 0, "cmgs text mode data:  local to ucs 2", pParam->nDLCI, nSim);
                            return;
                        }

                        nSmsSendDatalen = nUCS2DataSize;
                        AT_MemZero(pParam->pExData, AT_CMD_LINE_BUFF_LEN);

                        // buffer overflow
                        if (nSmsSendDatalen > AT_CMD_LINE_BUFF_LEN)
                        {
                            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgs text mode data:  buffer overflow",
                                              pParam->nDLCI, nSim);
                            return;
                        }

                        // move the data to pSmsSendData
                        AT_MemCpy(pParam->pExData, pUCS2Data, nSmsSendDatalen);

                        // Free inout para resource
                        AT_FREE(pUCS2Data);
                    }
                }

                /* Exception happened  */
                if (bParaVerifyErrorFlag)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "cmgs text mode data: input data's length error",
                                      pParam->nDLCI, nSim);
                    return;
                }

                // get message context,dynamic alloc
                pSmsSendData = (UINT8 *)AT_MALLOC(nSmsSendDatalen * 2);

                if (pSmsSendData == NULL)
                {
                    AT_FREE(pUCS2Data);
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgs text mode data: MALLOC ERROR 11",
                                      pParam->nDLCI, nSim);
                    return;
                }
                AT_MemZero(pSmsSendData, nSmsSendDatalen * 2);
                if (sInfoForChinese.dcs == 0)
                {
                    ATConvertAsciiEncodingToGSM7BitDefault(pSmsSendData, pParam->pExData, nSmsSendDatalen, (UINT16 *)&nSmsSendDatalen);
                }
                else
                    AT_MemCpy(pSmsSendData, pParam->pExData, nSmsSendDatalen);

                // validate the value dial number
                Number.nDialNumberSize = g_cmgw_cmgs.nSize;
                Number.pDialNumber = g_cmgw_cmgs.nTelNumber;
                Number.nType = g_cmgw_cmgs.nType;

                /* just for debug */
                AT_TC(g_sw_AT_SMS, "Text mode data to need to send: nSmsSendDatalen = %u", nSmsSendDatalen);
                AT_TC_MEMBLOCK(g_sw_AT_SMS, pSmsSendData, nSmsSendDatalen, 16);

                /* get the UTI and free it after finished calling */
                if (0 == (nUTI = AT_AllocUserTransID()))
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgs text mode dat: malloc UTI error", pParam->nDLCI, nSim);
                    return;
                }
                if (pgATFNDlist[nSim])
                {
                    if (1 == pgATFNDlist[nSim]->nFDNSatus)
                    {
                        if (FALSE == AT_FDN_CompareNumber(Number.pDialNumber, Number.nDialNumberSize, Number.nType, nSim))
                        {
                            AT_TC(g_sw_AT_CC, "CmdFunc_D Warning don't FDN record number!\n\r");
                            SUL_StrPrint(g_pCeer, "+CEER: %d", ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED);
                            AT_CC_Result_Err(ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }
                    }
                }
                nRetSendMsg = CFW_SmsSendMessage(&Number, pSmsSendData, nSmsSendDatalen, pParam->nDLCI, nSim);
                AT_FreeUserTransID(nUTI);

                if (ERR_SUCCESS != nRetSendMsg)
                {
                    AT_FREE(pSmsSendData);
                    nRetSendMsg = AT_SMS_ERR_Change(nRetSendMsg);
                    AT_SMS_Result_Err(nRetSendMsg, NULL, 0, " cmgs text mode data: sand message check param error", pParam->nDLCI,
                                      nSim);
                    return;
                }

                /* Different networks have different response time, according to networks status,
                   ** we set general enough time(here is 60s ) to waiting for networks response */
                AT_SMS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 60, NULL, 0, pParam->nDLCI, nSim);
                AT_FREE(pSmsSendData);

                return;
            }

            // pdu mode procedure and send message
            else
            {
                /* Get PDU mode input param */
                nPduTotalDataLen = g_cmgw_cmgs_pdu_length;

                /* Starting send pdu message */
                nSmsSendDatalen = pParam->iExDataLen - 1;

                pSmsSendData = (UINT8 *)AT_MALLOC(nSmsSendDatalen);
                if (pSmsSendData == NULL)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_MEM_FAIL, NULL, 0, "cmgs pdu mode data: MALLOC ERROR 22", pParam->nDLCI, nSim);
                    return;
                }

                /* clear buffer and copy data */
                AT_MemSet(pSmsSendData, 0, nSmsSendDatalen);
                AT_MemCpy(pSmsSendData, &(pParam->pExData[0]), pParam->iExDataLen - 1);

                pdulen = (nSmsSendDatalen % 2) ? (nSmsSendDatalen / 2 + 1) : (nSmsSendDatalen / 2);
                PduData = (UINT8 *)AT_MALLOC(pdulen);
                if (PduData == NULL)
                {
                    AT_FREE(pSmsSendData);
                    AT_SMS_Result_Err(ERR_AT_CMS_MEM_FAIL, NULL, 0, "cmgs pdu mode data: MALLOC ERROR 33", pParam->nDLCI, nSim);
                    return;
                }

                /* clear buffer */
                AT_MemSet(PduData, 0, pdulen);

                pdulen = SMS_PDUAsciiToBCD(pSmsSendData, nSmsSendDatalen, PduData);
                if (pdulen == 0)
                {
                    AT_FREE(pSmsSendData);
                    AT_FREE(PduData);
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0, "cmgs pdu mode data: pdulen == 0", pParam->nDLCI,
                                      nSim);
                    return;
                }

                /*
                   ** check param1(length:TP DATA UNIT) pdu length not include sca length
                   ** check total pdu length
                 */
                if ((nSmsSendDatalen + 1 > 353) || (pdulen != nPduTotalDataLen + 1 + PduData[0]))
                {
                    AT_FREE(pSmsSendData);
                    AT_FREE(PduData);
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "cmgs pdu mode data: (SmsDatalen+1 > 353)", pParam->nDLCI,
                                      nSim);
                    return;
                }

                pduaddsca = (UINT8 *)AT_MALLOC(pdulen + 23);
                if (pduaddsca == NULL) // [+]2007.10.16
                {
                    AT_FREE(pSmsSendData);
                    AT_FREE(PduData);
                    AT_SMS_Result_Err(ERR_AT_CMS_MEM_FAIL, NULL, 0, "cmgs pdu mode data:MALLOC ERROR!55", pParam->nDLCI, nSim);
                    return;
                }

                /* clear buffer */
                AT_MemSet(pduaddsca, 0, pdulen + 23);
                if (!(SMS_AddSCA2PDU(PduData, pduaddsca, &pdulen, nSim)))
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0, "cmgs pdu mode data: MALLOC ERROR!66", pParam->nDLCI,
                                      nSim);
                    return;
                }
                AT_TC_MEMBLOCK(g_sw_AT_SMS, pduaddsca, pdulen, 16);
                if (!AT_SMS_CheckPDUIsValid(pduaddsca, (UINT8 *)&pdulen, FALSE))
                {

                    AT_FREE(pSmsSendData);
                    AT_FREE(PduData);
                    AT_FREE(pduaddsca);
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_CHAR, NULL, 0, "cmgs pdu mode data: Check Valid_PDU Error ",
                                      pParam->nDLCI, nSim);
                    return;
                }

                //===========================================
                /* starting send message     */
                //===========================================
                /* just for debug */
                AT_TC(g_sw_AT_SMS, "Pdu mode data to need to send: pdulen = %u", pdulen);
                AT_TC_MEMBLOCK(g_sw_AT_SMS, pduaddsca, pdulen, 16);

                /* get the UTI and free it after finished calling */
                if (0 == (nUTI = AT_AllocUserTransID()))
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgs pdu mode data: malloc UTI error", pParam->nDLCI, nSim);
                    return;
                }
                AT_TC(g_sw_AT_SMS, "CMGS pgATFNDlist[nSim]: %d!\n\r", pgATFNDlist[nSim]);
                if (pgATFNDlist[nSim])
                {
                    AT_TC(g_sw_AT_SMS, "CMGS pgATFNDlist[nSim]->nFDNSatus: %d!\n\r", pgATFNDlist[nSim]->nFDNSatus);
                    if (1 == pgATFNDlist[nSim]->nFDNSatus)
                    {
                        UINT8 nNumSize = 0;
                        UINT8 nSCANumSize = 0;
                        UINT8 nType = 0;
                        UINT8 nFDNNumbers[20] = {
                            0xff,
                        };
                        UINT8 nSCANumbers[20] = {
                            0xff,
                        };

                        AT_MemSet(nFDNNumbers, 0xff, 20);
                        if (FALSE == AT_SMS_GetPDUNUM(pduaddsca, nFDNNumbers, &nType, &nNumSize))
                            return;

                        AT_MemSet(nSCANumbers, 0xff, 20);
                        if (FALSE == AT_SMS_GetSCANUM(pduaddsca, nSCANumbers, &nSCANumSize))
                        {
                            AT_TC(g_sw_AT_SMS, "CMGS Warning don't FDN SCA number!\n\r");

                            SUL_StrPrint(g_pCeer, "+CEER: %d", ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED);
                            AT_CC_Result_Err(ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        }
                        /*if( FALSE == AT_FDN_CompareNumber(nSCANumbers, nSCANumSize, 0xff, nSim) )
                        {
                            AT_TC(g_sw_AT_SMS, "CMGS Warning don't FDN SCA record number!\n\r");

                            SUL_StrPrint(g_pCeer, "+CEER: %d", ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED); //adapt to android by wulc
                            AT_CC_Result_Err(ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }
												*/
                        if (0x91 == nType)
                        {
                            nType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
                        }

                        if (FALSE == AT_FDN_CompareNumber(nFDNNumbers, nNumSize, nType, nSim))
                        {
                            AT_TC(g_sw_AT_SMS, "CMGS Warning don't FDN record number!\n\r");

                            SUL_StrPrint(g_pCeer, "+CEER: %d", ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED); //adapt to android by wulc
                            AT_CC_Result_Err(ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }
                    }
                }

                /* Send message and free UTI */

                nRetSendMsg = CFW_SmsSendMessage(NULL, pduaddsca, pdulen, pParam->nDLCI, nSim);
                AT_FreeUserTransID(nUTI);
                if (ERR_SUCCESS != nRetSendMsg)
                {
                    AT_FREE(pSmsSendData);
                    AT_FREE(PduData);
                    AT_FREE(pduaddsca);

                    nRetSendMsg = AT_SMS_ERR_Change(nRetSendMsg);
                    AT_SMS_Result_Err(nRetSendMsg, NULL, 0, "cmgs pdu mode data: Send msg check param error 22", pParam->nDLCI,
                                      nSim);
                    return;
                }

                AT_FREE(pSmsSendData);
                AT_FREE(PduData);
                AT_FREE(pduaddsca);

                /* Different networks have different response time, according to networks status,
                   ** we set general enough time(here is 60s ) to waiting for networks response */
                AT_SMS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 60, NULL, 0, pParam->nDLCI, nSim);
                break;
            }
        }

        break;

    case AT_CMD_TEST:
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;
    case AT_CMD_EXE:
        CFW_SmsSendPresetMessage("10086", "Hello World!", pParam->nDLCI, nSim); //Add for gcf test.
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;
    default:
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, "cmgs pdu mode data: wrong cmd mode", pParam->nDLCI, nSim);
        break;
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CMGW
Description     :       CMGW procedure function
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CMGW(AT_CMD_PARA *pParam)
{
    CFW_SMS_WRITE SmsWrite;

    CFW_SMS_MO_INFO_ADD *pSmsMOInfo = NULL;
    CFW_EVENT nCfwEvent = {
        0,
    };

    CFW_SMS_PARAMETER sInfoForChinese = {0};
    UINT8 nParamCount = 0;
    UINT8 BCDDataBuff[AT_SMS_BODY_BUFF_LEN] = {0};
    UINT8 nFormat = 0;

    UINT8 DialNumberBuff[TEL_NUMBER_MAX_LEN] = {0};
    UINT16 nDialNumberLen = 24;
    UINT8 nDialNumType = 0;
    UINT16 nParamLen = 0;

    INT8 InputState[16] = {0};
    UINT8 nPDUParamLen = 0;

    UINT8 *pUCS2Data = NULL;
    UINT32 nUCS2DataSize = 0;
    UINT16 retForChinese = 0;
    INT32 nOperationRet = ERR_SUCCESS;
    INT32 nWriteMsgRet = ERR_SUCCESS;

    UINT8 *pSmsDataBuff = NULL;
    UINT32 SmsDatalen = 0;

    BOOL bReturn = FALSE;
    BOOL bIsInternational = FALSE;

    UINT8 PromptMsg[8] = {0};

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    nCfwEvent.nFlag = nSim;

    /********************************************************************/

    CFW_SMS_STORAGE_INFO sStorageInfo = {0};
    if (ERR_SUCCESS != CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSim))
    {
        return;
    }
    if (gATCurrentAtSMSInfo[nSim].nStorage2 == CFW_SMS_STORAGE_MT)
    {
        gATCurrentAtSMSInfo[nSim].nStorage2 = CFW_SMS_STORAGE_SM;
    }
    AT_TC(g_sw_AT_SMS, "sStorageInfo totalSlot = %d,usedSlot = %d", sStorageInfo.totalSlot, sStorageInfo.usedSlot);
    /* if SM storage buffer is full and set flag */
    if ((sStorageInfo.totalSlot == sStorageInfo.usedSlot) && (sStorageInfo.usedSlot != 0))
    {
        /* SM storage is full, set bit1 is 1 */
        g_SMS_CIND_MemFull[nSim] |= CFW_SMS_STORAGE_SM;
    }
    else if (sStorageInfo.totalSlot != sStorageInfo.usedSlot)
    {
        g_SMS_CIND_MemFull[nSim] = 0;
    }

    AT_ASSERT(NULL != pParam);
    g_SMS_CurCmdStamp = pParam->uCmdStamp;

    bReturn = AT_GetCfwInitSmsInfo(&nCfwEvent);
    if ((bReturn == 0) || (nCfwEvent.nParam1 == 1))
    {
        AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmgw: SMS INIT FAIL", pParam->nDLCI, nSim);
        return;
    }

    // check the mode and process
    switch (pParam->iType)
    {
    case AT_CMD_SET:

        //===========================================
        /* return error if the buff is overflow, and check "SM" storage */
        /* and "ME" storage eparately, 2008.07.03, by wangqy           */
        //===========================================
        if ((gATCurrentAtSMSInfo[nSim].nStorage2 & CFW_SMS_STORAGE_MT) ||
            (gATCurrentAtSMSInfo[nSim].nStorage2 & CFW_SMS_STORAGE_SM))
        {

            /* bit0 flag of SM is "1", SM memfull */
            if (CFW_SMS_STORAGE_SM == (g_SMS_CIND_MemFull[nSim] & CFW_SMS_STORAGE_SM))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_MEM_FULL, NULL, 0, "cmgw set mode: message buffer of SM is overflow",
                                  pParam->nDLCI, nSim);
                return;
            }
        }
        else if (gATCurrentAtSMSInfo[nSim].nStorage2 & CFW_SMS_STORAGE_ME)
        {
            /* bit1 flag of ME is "1", ME memfull */
            if (CFW_SMS_STORAGE_ME == (g_SMS_CIND_MemFull[nSim] & CFW_SMS_STORAGE_ME))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_MEM_FULL, NULL, 0, "cmgw set mode: message buffer of ME is overflow",
                                  pParam->nDLCI, nSim);
                return;
            }
        }
        else
        {
            AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, NULL, 0, "cmgw set mode: not support this storage", pParam->nDLCI,
                              nSim);
            return;
        }

        /***************************************
            **
            ** Prompt input send message data
            **
            ***************************************/
        if (pParam->pExData == NULL)
        {

            nFormat = gATCurrentAtSmsSettingSg_SMSFormat;

            // get param count
            nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);

            if ((nOperationRet != ERR_SUCCESS) || (nParamCount == 0) || (nParamCount > 3))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmgw text mode: get param counter error", pParam->nDLCI,
                                  nSim);
                return;
            }

            if (!(nFormat == 0 || nFormat == 1))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgw text mode: Format exception", pParam->nDLCI, nSim);
                return;
            }

            /******************************************************************
                **              nFormat==1
                ******************************************************************/
            if (nFormat == 1)
            {

                /* set default value of status */
                AT_MemSet(&g_cmgw_cmgs, 0, sizeof(CFW_TELNUMBER));
                g_cmgw_cmgs.padding[0] = CFW_SMS_STORED_STATUS_UNSENT;

                //===========================================
                // process different part according to para counts
                //===========================================
                if (1 == nParamCount)
                    goto CMGW_label_para1;

                if (2 == nParamCount)
                    goto CMGW_label_para2;

                if (3 == nParamCount)
                    goto CMGW_label_para3;

            CMGW_label_para3:

                //===========================================
                // para 3
                //===========================================

                nParamLen = 16;
                nOperationRet =
                    AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, (void *)(InputState), &nParamLen);

                /* Dial number can omit and set default value */
                if (ERR_AT_UTIL_CMD_PARA_NULL == nOperationRet)
                {
                    AT_MemZero(InputState, 16);
                    nParamLen = 0;
                }
                else
                {
                    if (nOperationRet != ERR_SUCCESS)
                    {
                        AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgw text mode: get param 3 error", pParam->nDLCI,
                                          nSim);
                        return;
                    }

                    AT_StrUpr(InputState);
                    if (!AT_SMS_StringOrDataToStatusMacroFlag(InputState, g_cmgw_cmgs.padding, nFormat))
                    {
                        AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgw text mode: wrong status", pParam->nDLCI,
                                          nSim);
                        return;
                    }
                }

            CMGW_label_para2:

                //===========================================
                // para 2
                //===========================================
                nParamLen = 1;

                nOperationRet =
                    AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, (void *)(&nDialNumType), &nParamLen);

                /* Dial number can omit and set default value */
                if (ERR_AT_UTIL_CMD_PARA_NULL == nOperationRet)
                {
                    nDialNumType = CFW_TELNUMBER_TYPE_UNKNOWN;
                }
                else
                {
                    if (nOperationRet != ERR_SUCCESS)
                    {
                        AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgw text mode: get param 3 error", pParam->nDLCI,
                                          nSim);
                        return;
                    }
                }

                if ((nDialNumType != CFW_TELNUMBER_TYPE_INTERNATIONAL) && (nDialNumType != CFW_TELNUMBER_TYPE_NATIONAL) && (nDialNumType != CFW_TELNUMBER_TYPE_UNKNOWN))
                {
                    AT_MemSet(&g_cmgw_cmgs, 0, sizeof(CFW_TELNUMBER));
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgw text mode: wrong type of num", pParam->nDLCI,
                                      nSim);
                    return;
                }

            CMGW_label_para1:
                //===========================================
                // para 1
                //===========================================
                nOperationRet =
                    AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, (void *)DialNumberBuff, &nDialNumberLen);

                /* cmgw: write message: dialnumber can be null and omit */
                if (ERR_AT_UTIL_CMD_PARA_NULL == nOperationRet)
                {
                    AT_MemZero(DialNumberBuff, TEL_NUMBER_MAX_LEN);
                    nDialNumberLen = 0;
                }
                else
                {
                    if (nOperationRet != ERR_SUCCESS)
                    {
                        AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgw text mode: get param 1 error", pParam->nDLCI,
                                          nSim);
                        return;
                    }
                }

                /* have dialnumber input and check it */
                if (0 != nDialNumberLen)
                {
                    if (!AT_SMS_IsValidPhoneNumber(DialNumberBuff, nDialNumberLen, &bIsInternational))
                    {
                        AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmgw text mode: number invalid", pParam->nDLCI, nSim);
                        return;
                    }
                }

                if (bIsInternational)
                {
                    nDialNumberLen--;
                    if (nDialNumberLen > 20)
                    {
                        AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "cmgw text mode with '+': number len too long",
                                          pParam->nDLCI, nSim);
                        return;
                    }
                    else
                    {
                        if (nDialNumberLen)
                        {
                            g_cmgw_cmgs.nSize = SUL_AsciiToGsmBcd(&DialNumberBuff[1], nDialNumberLen, g_cmgw_cmgs.nTelNumber);
                        }
                    }
                }
                else
                {
                    if (nDialNumberLen > 20)
                    {
                        AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "cmgw text mode without '+': number len too long",
                                          pParam->nDLCI, nSim);
                        return;
                    }
                    else
                    {

                        if (nDialNumberLen)
                        {
                            g_cmgw_cmgs.nSize = SUL_AsciiToGsmBcd(DialNumberBuff, nDialNumberLen, g_cmgw_cmgs.nTelNumber);
                        }
                    }
                }

                /* according to spec, we check dial number first */
                if (bIsInternational)
                {
                    nDialNumType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
                }
                else
                {
                    if (0 == nDialNumType)
                    {
                        /* no input num type  */
                        nDialNumType = CFW_TELNUMBER_TYPE_UNKNOWN;
                    }
                    else if (CFW_TELNUMBER_TYPE_INTERNATIONAL == nDialNumType)
                    {
                        /* have no '+' , but the num type is 145, replace 129 with is  */
                        nDialNumType = CFW_TELNUMBER_TYPE_UNKNOWN;
                    }
                    else
                    {
                        /* get the input num type  */
                    }
                }

                g_cmgw_cmgs.nType = nDialNumType;
            }

            /******************************************************************
                **              nFormat==0
                ******************************************************************/
            else
            {

                if (nParamCount > 2)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0, "cmgw pdu mode : get param counter exception",
                                      pParam->nDLCI, nSim);
                    return;
                }

                // set default value of status
                g_cmgw_cmgs_pdu_length = 0;
                g_cmgw_cmgs_pdu_status = CFW_SMS_STORED_STATUS_UNSENT;

                /******************************/
                // procedure this pdu imput message
                /******************************/
                if (1 == nParamCount)
                    goto CMGW_PDU_label_para1;

                if (2 == nParamCount)
                    goto CMGW_PDU_label_para2;

            CMGW_PDU_label_para2:

                nParamLen = 1;

                nOperationRet =
                    AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, (void *)(&InputState[0]), &nParamLen);
                if (nOperationRet != ERR_SUCCESS)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0, "cmgw pdu mode: get param 2 error", pParam->nDLCI,
                                      nSim);
                    return;
                }

                if (InputState[0] < 3)
                {
                    g_cmgw_cmgs_pdu_status = 1 << (InputState[0]);
                }
                else if (InputState[0] == 3)
                {
                    g_cmgw_cmgs_pdu_status = CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ;
                }
                else
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0, "cmgw pdu mode: wrong status value", pParam->nDLCI,
                                      nSim);
                    return;
                }

            CMGW_PDU_label_para1:

                nParamLen = 1;

                nOperationRet =
                    AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, (void *)(&nPDUParamLen), &nParamLen);
                if (nOperationRet != ERR_SUCCESS)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0, "cmgw pdu mode: get param 1 error", pParam->nDLCI,
                                      nSim);
                    return;
                }

                // in cmgw pdu lengt can be 0 but cmgs length!=0
                if ((nPDUParamLen > 164) || (nPDUParamLen == 0))
                {

                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0, "cmgw pdu mode: nPDUParamLen > 164 ", pParam->nDLCI,
                                      nSim);
                    return;
                }
                else
                {
                    g_cmgw_cmgs_pdu_length = nPDUParamLen;
                }
            }

            //===========================================
            // display in the screen, and need input
            //===========================================
            at_CmdRespOutputPrompt(pParam->engine);

            /* time  waiting for input sms data , set 180s now, if update we can change it */
            AT_SMS_Result_OK(CMD_FUNC_WAIT_SMS, CMD_RC_OK, 180, NULL, 0, pParam->nDLCI, nSim);
            return;
        }

        /*******************************************
             **
             **  with msg body,input data process is starting
             **
            *******************************************/
        else
        {
            if (pParam->pExData[pParam->iExDataLen - 1] == 27) // end with esc, cancel write
            {
                AT_MemSet(&g_cmgw_cmgs, 0, sizeof(CFW_TELNUMBER));
                AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
                return;
            }
            else if (pParam->pExData[pParam->iExDataLen - 1] != 26) // not end with ctl+z, err happen
            //else if( (pParam->pExData[pParam->iExDataLen - 1] != 26)&&(pParam->pExData[pParam->iExDataLen - 1] != 13))
            {
                AT_MemSet(&g_cmgw_cmgs, 0, sizeof(CFW_TELNUMBER));
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmgw text mode data: msg body not end with ctrl+z",
                                  pParam->nDLCI, nSim);
                return;
            }

            nFormat = gATCurrentAtSmsSettingSg_SMSFormat;

            if (!(nFormat == 0 || nFormat == 1))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgw text mode data: Format exception", pParam->nDLCI, nSim);
                return;
            }

            // ///////////////////////////////
            // do text write message nFormat == 1
            // ///////////////////////////////
            if (nFormat == 1)
            {

                BOOL bParaVerifyErrorFlag = FALSE;

                pSmsMOInfo =
                    (CFW_SMS_MO_INFO_ADD *)AT_MALLOC(sizeof(CFW_SMS_MO_INFO_ADD) + g_cmgw_cmgs.nSize + pParam->iExDataLen - 1);

                if (!pSmsMOInfo)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_MEM_FAIL, NULL, 0, "cmgw text mode data: malloc is error", pParam->nDLCI, nSim);
                    return;
                }

                AT_MemSet(pSmsMOInfo, 0, sizeof(CFW_SMS_MO_INFO_ADD) + g_cmgw_cmgs.nSize + pParam->iExDataLen - 1);

                /* get dianumber information */
                pSmsMOInfo->sNumber.nType = g_cmgw_cmgs.nType;
                pSmsMOInfo->sNumber.nDialNumberSize = g_cmgw_cmgs.nSize;

                if (g_cmgw_cmgs.nSize == 0)
                {
                    pSmsMOInfo->sNumber.pDialNumber = (UINT8 *)AT_MALLOC(g_cmgw_cmgs.nSize);
                    AT_MemZero(pSmsMOInfo->sNumber.pDialNumber, g_cmgw_cmgs.nSize);
                }
                else
                {
                    pSmsMOInfo->sNumber.pDialNumber = (UINT8 *)((UINT8 *)pSmsMOInfo + sizeof(CFW_SMS_MO_INFO_ADD));
                    AT_MemCpy(pSmsMOInfo->sNumber.pDialNumber, g_cmgw_cmgs.nTelNumber, g_cmgw_cmgs.nSize);
                }

                /* get SMS message content */
                pSmsMOInfo->nDataSize = pParam->iExDataLen - 1;
                pSmsMOInfo->pData = (UINT8 *)((UINT8 *)pSmsMOInfo + sizeof(CFW_SMS_MO_INFO_ADD) + g_cmgw_cmgs.nSize);
                pSmsMOInfo->nStatus = g_cmgw_cmgs.padding[0];
                pSmsMOInfo->nFormat = nFormat;
                pSmsMOInfo->nLocation = gATCurrentAtSMSInfo[nSim].nStorage2;

                // just it is chinese message send or not

                retForChinese = CFW_CfgGetSmsParam(&sInfoForChinese, 0, nSim);
                if (retForChinese != ERR_SUCCESS)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgw text mode data: Get chinese info error",
                                      pParam->nDLCI, nSim);
                    AT_FREE(pSmsMOInfo);
                    return;
                }

                /* Just for debug */
                AT_TC(g_sw_AT_SMS, "cmgw text mode: pParam->iExDataLen = %u", pParam->iExDataLen);

                /* according to the dcs value and judge */
                if (sInfoForChinese.dcs == 0)
                {
                    if (pParam->iExDataLen - 1 > 160)
                    {
                        bParaVerifyErrorFlag = TRUE;
                    }

                    SmsDatalen = pSmsMOInfo->nDataSize;
                    pSmsDataBuff = pParam->pExData;
                }
                else if (sInfoForChinese.dcs == 4)
                {
                    if (pParam->iExDataLen - 1 > 140)
                    {
                        bParaVerifyErrorFlag = TRUE;
                    }

                    SmsDatalen = pSmsMOInfo->nDataSize;
                    pSmsDataBuff = pParam->pExData;
                }
                else if (sInfoForChinese.dcs == 8)
                {

                    UINT32 UCSlength = 0;
                    INT32 nLocal2UCSReturn = ERR_SUCCESS;

                    UCSlength = SUL_CharacterLen(pParam->pExData, pParam->iExDataLen - 1);

                    if (UCSlength == -1 || UCSlength - 1 > 70)
                    {
                        bParaVerifyErrorFlag = TRUE;
                    }
                    else
                    {

                        /* When send data to low layer, used uniform transport mode: big ending */
                        AT_Set_MultiLanguage();

                        nLocal2UCSReturn =
                            ML_LocalLanguage2Unicode(pParam->pExData, pParam->iExDataLen - 1, &pUCS2Data, &nUCS2DataSize, NULL);

                        if (ERR_SUCCESS != nLocal2UCSReturn)
                        {
                            nLocal2UCSReturn = AT_SMS_ERR_Change(nLocal2UCSReturn);
                            AT_SMS_Result_Err(nLocal2UCSReturn, NULL, 0, "cmgw text mode data: local to ucs error", pParam->nDLCI,
                                              nSim);
                            return;
                        }

                        // get message context
                        SmsDatalen = nUCS2DataSize;
                        pSmsDataBuff = pUCS2Data;
                    }
                }

                if (bParaVerifyErrorFlag)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "cmgw text mode data: input data's length error",
                                      pParam->nDLCI, nSim);
                    AT_FREE(pSmsMOInfo);
                    return;
                }

                /* just for debug */
                AT_TC(g_sw_AT_SMS, "Text mode data to need to write: SmsDatalen = %u", SmsDatalen);
                AT_TC_MEMBLOCK(g_sw_AT_SMS, pSmsDataBuff, SmsDatalen, 16);

                // pSmsMOInfo->sNumber.pDialNumber
                AT_TC_MEMBLOCK(g_sw_AT_SMS, pSmsMOInfo->sNumber.pDialNumber, pSmsMOInfo->sNumber.nDialNumberSize, 16);

                // wulc add begin 2011-12-28
                SmsWrite.sNumber = pSmsMOInfo->sNumber;
                SmsWrite.pData = pSmsDataBuff;
                SmsWrite.nDataSize = SmsDatalen;

                if (CFW_SMS_STORAGE_MT == pSmsMOInfo->nLocation)
                    pSmsMOInfo->nLocation = CFW_SMS_STORAGE_SM;

                SmsWrite.nStorage = pSmsMOInfo->nLocation;
                SmsWrite.nStatus = pSmsMOInfo->nStatus;
                SmsWrite.nIndex = 0;
                SmsWrite.nType = CFW_SMS_TYPE_PP;

                // end
                AT_TC_MEMBLOCK(g_sw_AT_SMS, SmsWrite.sNumber.pDialNumber, SmsWrite.sNumber.nDialNumberSize, 16);

                // wulc  20110524
                nWriteMsgRet = CFW_SmsWriteMessage(&SmsWrite, pParam->nDLCI, nSim);
                // Free inout para resource
                AT_FREE(pUCS2Data);
            }

            // ////////////////////////////////////////
            // pdu write message procedure nFormat == 0
            // ////////////////////////////////////////
            else
            {
                // judge the max data len is valid
                if (pParam->iExDataLen > 353)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0, "cmgw pdu mode data: iExDataLen>353 error",
                                      pParam->nDLCI, nSim);
                    return;
                }

                pParam->iExDataLen = SMS_PDUAsciiToBCD((pParam->pExData), pParam->iExDataLen - 1, BCDDataBuff);

                if (pParam->iExDataLen == 0)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0, "cmgw pdu mode data: iExDataLen is 0 ",
                                      pParam->nDLCI, nSim);
                    return;
                }

                // check param1(length:TP DATA UNIT) pdu length not include sca length
                if (pParam->iExDataLen != g_cmgw_cmgs_pdu_length + 1 + BCDDataBuff[0])
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0,
                                      "cmgw pdu mode data: input pdu data length not match", pParam->nDLCI, nSim);
                    return;
                }

                pSmsMOInfo = (CFW_SMS_MO_INFO_ADD *)AT_MALLOC(sizeof(CFW_SMS_MO_INFO_ADD) + AT_SMS_BODY_BUFF_LEN);

                if (!pSmsMOInfo)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_MEM_FAIL, NULL, 0, "cmgw pdu mode data: malloc is error 22", pParam->nDLCI,
                                      nSim);
                    return;
                }

                AT_MemSet(pSmsMOInfo, 0, (sizeof(CFW_SMS_MO_INFO_ADD) + AT_SMS_BODY_BUFF_LEN));

                pSmsMOInfo->nDataSize = pParam->iExDataLen; // -1?;
                pSmsMOInfo->pData = (UINT8 *)((UINT8 *)pSmsMOInfo + sizeof(CFW_SMS_MO_INFO_ADD));
                pSmsMOInfo->nStatus = g_cmgw_cmgs_pdu_status;
                pSmsMOInfo->nLocation = gATCurrentAtSMSInfo[nSim].nStorage2;

                if (!(SMS_AddSCA2PDU(BCDDataBuff, pSmsMOInfo->pData, &(pSmsMOInfo->nDataSize), nSim)))
                {
                    AT_FREE(pSmsMOInfo);
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0, "cmgw pdu mode data: AddSCA2PDU error",
                                      pParam->nDLCI, nSim);
                    return;
                }

                // check it add one sca or not
                if (!AT_SMS_CheckPDUIsValid(pSmsMOInfo->pData, (UINT8 *)&(pSmsMOInfo->nDataSize), FALSE))
                {
                    AT_FREE(pSmsMOInfo);
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_CHAR, NULL, 0, "cmgw pdu mode data: check pdu is invalid",
                                      pParam->nDLCI, nSim);
                    return;
                }

                /* just for debug */
                AT_TC(g_sw_AT_SMS, "Pdu mode data to need to write: pSmsMOInfo->nDataSize = %u", pSmsMOInfo->nDataSize);
                AT_TC_MEMBLOCK(g_sw_AT_SMS, pSmsMOInfo->pData, pSmsMOInfo->nDataSize, 16);

                /* get the UTI and free it after finished calling */

                // starting write message

                // SmsWrite.sNumber = pSmsMOInfo->sNumber =;
                SmsWrite.pData = pSmsMOInfo->pData;
                SmsWrite.nDataSize = pSmsMOInfo->nDataSize;

                if (CFW_SMS_STORAGE_MT == pSmsMOInfo->nLocation)
                    pSmsMOInfo->nLocation = CFW_SMS_STORAGE_SM;

                SmsWrite.nStorage = pSmsMOInfo->nLocation;
                SmsWrite.nStatus = pSmsMOInfo->nStatus;
                SmsWrite.nIndex = 0;
                SmsWrite.nType = CFW_SMS_TYPE_PP;
                nWriteMsgRet = CFW_SmsWriteMessage(&SmsWrite, pParam->nDLCI, nSim);
            }

            //===========================================
            // sms:    write message result: text mode and pdu mode
            //===========================================
            if (nWriteMsgRet == ERR_SUCCESS)
            {
                AT_SMS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            }
            else
            {
                nWriteMsgRet = AT_SMS_ERR_Change(nWriteMsgRet);
                AT_SMS_Result_Err(nWriteMsgRet, NULL, 0, "cmgw data: write message check param error", pParam->nDLCI, nSim);
            }

            AT_FREE(pSmsMOInfo);

            return;
        }

        break;

    case AT_CMD_EXE:

        //===========================================
        /* return error if the buff is overflow, and check "SM" storage */
        /* and "ME" storage eparately, 2008.07.03, by wangqy           */
        //===========================================
        if (gATCurrentAtSMSInfo[nSim].nStorage2 & CFW_SMS_STORAGE_SM)
        {

            /* bit0 flag of SM is "1", SM memfull */
            if (CFW_SMS_STORAGE_SM == (g_SMS_CIND_MemFull[nSim] & CFW_SMS_STORAGE_SM))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_MEM_FULL, NULL, 0, "cmgw exe mode: message buffer of SM is overflow",
                                  pParam->nDLCI, nSim);
                return;
            }
        }
        else if (gATCurrentAtSMSInfo[nSim].nStorage2 & CFW_SMS_STORAGE_ME)
        {
            /* bit1 flag of ME is "1", ME memfull */
            if (CFW_SMS_STORAGE_ME == (g_SMS_CIND_MemFull[nSim] & CFW_SMS_STORAGE_ME))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_MEM_FULL, NULL, 0, "cmgw exe mode: message buffer of ME is overflow",
                                  pParam->nDLCI, nSim);
                return;
            }
        }
        else
        {
            AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, NULL, 0, "cmgw exe mode: not support this storage", pParam->nDLCI,
                              nSim);
            return;
        }

        {
            //===========================================
            // display in the screen, and write null message in the text mode
            //===========================================
            CFW_DIALNUMBER NumBuff = {0};

            // UINT8 DataBuff[8] = {0};

            PromptMsg[0] = AT_GC_CfgGetS3Value();
            PromptMsg[1] = AT_GC_CfgGetS4Value();
            AT_StrCat(PromptMsg, "> ");
            PromptMsg[AT_StrLen(PromptMsg)] = 0x1A; /* ctrl+Z */
            PromptMsg[AT_StrLen(PromptMsg)] = '\0'; /* end of the string */

            /* no time delay in this case */
            AT_SMS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, (UINT8 *)PromptMsg, AT_StrLen(PromptMsg), pParam->nDLCI, nSim);
            /* set value of NumBuff */
            NumBuff.nType = CFW_TELNUMBER_TYPE_UNKNOWN;
            NumBuff.pDialNumber = (UINT8 *)AT_MALLOC(0);
            AT_MemZero(NumBuff.pDialNumber, 0);

            /* Text mode have execute mode */
            if (gATCurrentAtSmsSettingSg_SMSFormat)
            {
                SmsWrite.sNumber = NumBuff;
                SmsWrite.pData = NULL;
                SmsWrite.nDataSize = 0;
                SmsWrite.nStorage = gATCurrentAtSMSInfo[nSim].nStorage2;

                if (CFW_SMS_STORAGE_MT == SmsWrite.nStorage)
                    pSmsMOInfo->nLocation = CFW_SMS_STORAGE_SM;

                SmsWrite.nStatus = CFW_SMS_STORED_STATUS_UNSENT;
                SmsWrite.nIndex = 0;
                SmsWrite.nType = CFW_SMS_TYPE_PP;

                nWriteMsgRet = CFW_SmsWriteMessage(&SmsWrite, pParam->nDLCI, nSim);
            }

            /* PDU mode have execute mode */
            else
            {
                AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, NULL, 0, "cmgw exe mode: pdu mode can't write NULL msg",
                                  pParam->nDLCI, nSim);
                return;
            }

            //===========================================
            // sms:    write message result: text mode and pdu mode
            //===========================================
            if (nWriteMsgRet == ERR_SUCCESS)
            {
                AT_SMS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            }
            else
            {
                nWriteMsgRet = AT_SMS_ERR_Change(nWriteMsgRet);
                AT_SMS_Result_Err(nWriteMsgRet, NULL, 0, "cmgw exe mode: write NULL msg check param error", pParam->nDLCI,
                                  nSim);
            }
        }

        break;
    case AT_CMD_TEST:

        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    default:
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, "cmgw default: wrong cmd mode", pParam->nDLCI, nSim);
        break;
        ;
    }

    return;

} // end with msg

/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CPMS
Description     :       CPMS procedure function
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CPMS(AT_CMD_PARA *pParam)
{

    CFW_EVENT nCfwEvent = {
        0,
    };
    CFW_SMS_STORAGE_INFO sStorageInfo = {0};
    UINT8 MemBuff[8] = {0};
    UINT16 nParamLen = 5;
    UINT8 nParamCount = 0;

    UINT8 PromptBuff[64] = {0};
    UINT8 nStorage1Last = 0;
    UINT8 nStorage2Last = 0;
    UINT8 nStorage3Last = 0;

    UINT32 nOperationRet = ERR_SUCCESS;
    BOOL bReturn = FALSE;

    UINT8 ReadMem1[3] = {0};
    UINT8 ReadMem2[3] = {0};
    UINT8 ReadMem3[3] = {0};

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    nCfwEvent.nFlag = nSim;

    AT_ASSERT(NULL != pParam);

    bReturn = AT_GetCfwInitSmsInfo(&nCfwEvent);
    if ((!bReturn) || (nCfwEvent.nParam1 == 1))
    {
        AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cpms: AT_GetCfwInitSmsInfo() false", pParam->nDLCI, nSim);
        return;
    }

    //===========================================
    // command execute mode
    //===========================================
    switch (pParam->iType)
    {
    case AT_CMD_SET:

        nStorage1Last = gATCurrentAtSMSInfo[nSim].nStorage1;
        nStorage2Last = gATCurrentAtSMSInfo[nSim].nStorage2;
        nStorage3Last = gATCurrentAtSMSInfo[nSim].nStorage3;

        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);

        if (nOperationRet != ERR_SUCCESS || (nParamCount == 0) || (nParamCount > 3))
        {
            nOperationRet = ERR_AT_CMS_INVALID_PARA;
        }

        //===========================================
        // para1, para 2 and para3 process these code
        //===========================================
        if (nOperationRet == ERR_SUCCESS && nParamCount > 0)
        {
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, (void *)MemBuff, &nParamLen);

            if (nOperationRet == ERR_SUCCESS)
            {

                AT_StrUpr(MemBuff);

                if (!strcmp(MemBuff, "ME"))
                {
                    gATCurrentAtSMSInfo[nSim].nStorage1 = CFW_SMS_STORAGE_ME;
                }
                else if (!strcmp(MemBuff, "SM"))
                {
                    gATCurrentAtSMSInfo[nSim].nStorage1 = CFW_SMS_STORAGE_SM;
                }
                else if (!strcmp(MemBuff, "MT"))
                {
                    gATCurrentAtSMSInfo[nSim].nStorage1 = CFW_SMS_STORAGE_MT;
                }
                else
                {
                    nOperationRet = ERR_AT_CMS_INVALID_PARA;
                }

                if (ERR_SUCCESS == nOperationRet)
                {

                    nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, gATCurrentAtSMSInfo[nSim].nStorage1, nSim);
                    if (ERR_SUCCESS == nOperationRet)
                    {
                        gATCurrentAtSMSInfo[nSim].nTotal1 = sStorageInfo.totalSlot;
                        gATCurrentAtSMSInfo[nSim].nUsed1 = sStorageInfo.usedSlot;
                    }
                    else
                    {
                        nOperationRet = ERR_AT_CMS_MEM_FAIL;
                    }
                }
            }
        }

        //===========================================
        // para 2 and para3 process these code
        //===========================================
        if (nOperationRet == ERR_SUCCESS && nParamCount > 1)
        {
            nParamLen = 5;
            AT_MemSet(MemBuff, 0, 8);
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, (void *)MemBuff, &nParamLen);

            if (nOperationRet == ERR_SUCCESS)
            {
                AT_StrUpr(MemBuff);

                if (!strcmp(MemBuff, "ME"))
                {
                    gATCurrentAtSMSInfo[nSim].nStorage2 = CFW_SMS_STORAGE_ME;
                }
                else if (!strcmp(MemBuff, "SM"))
                {
                    gATCurrentAtSMSInfo[nSim].nStorage2 = CFW_SMS_STORAGE_SM;
                }
                else if (!strcmp(MemBuff, "MT"))
                {
                    gATCurrentAtSMSInfo[nSim].nStorage2 = CFW_SMS_STORAGE_MT;
                }
                else
                {
                    nOperationRet = ERR_AT_CMS_INVALID_PARA;
                }

                if (ERR_SUCCESS == nOperationRet)
                {
                    nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, gATCurrentAtSMSInfo[nSim].nStorage2, nSim);
                    if (ERR_SUCCESS == nOperationRet)
                    {
                        gATCurrentAtSMSInfo[nSim].nTotal2 = sStorageInfo.totalSlot;
                        gATCurrentAtSMSInfo[nSim].nUsed2 = sStorageInfo.usedSlot;
                    }
                    else
                    {
                        nOperationRet = ERR_AT_CMS_MEM_FAIL;
                    }
                }
            }
        }

        //===========================================
        // para3 process these code
        //===========================================
        if (nOperationRet == ERR_SUCCESS && nParamCount > 2)
        {
            nParamLen = 5;
            AT_MemSet(MemBuff, 0, 8);
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, (void *)MemBuff, &nParamLen);

            if (nOperationRet == ERR_SUCCESS)
            {
                AT_StrUpr(MemBuff);

                if (!strcmp(MemBuff, "ME"))
                {
                    gATCurrentAtSMSInfo[nSim].nStorage3 = CFW_SMS_STORAGE_ME;
                }
                else if (!strcmp(MemBuff, "SM"))
                {
                    gATCurrentAtSMSInfo[nSim].nStorage3 = CFW_SMS_STORAGE_SM;
                }
                else if (!strcmp(MemBuff, "MT"))
                {
                    gATCurrentAtSMSInfo[nSim].nStorage3 = CFW_SMS_STORAGE_MT;
                }
                else
                {
                    nOperationRet = ERR_AT_CMS_INVALID_PARA;
                }

                if (ERR_SUCCESS == nOperationRet)
                {
                    UINT8 nOption = 0;
                    UINT8 nNewSmsStorage = 0;
                    nOperationRet = CFW_CfgGetNewSmsOption(&nOption, &nNewSmsStorage, nSim);
                    nOperationRet = CFW_CfgSetNewSmsOption(nOption, gATCurrentAtSMSInfo[nSim].nStorage3, nSim);
                    nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, gATCurrentAtSMSInfo[nSim].nStorage3, nSim);
                    if (ERR_SUCCESS == nOperationRet)
                    {
                        gATCurrentAtSMSInfo[nSim].nTotal3 = sStorageInfo.totalSlot;
                        gATCurrentAtSMSInfo[nSim].nUsed3 = sStorageInfo.usedSlot;
                    }
                    else
                    {
                        nOperationRet = ERR_AT_CMS_MEM_FAIL;
                    }
                }
            }
        }

        if (ERR_SUCCESS == nOperationRet)
        {

            AT_Sprintf(PromptBuff, "+CPMS: %u,%u,%u,%u,%u,%u",
                       gATCurrentAtSMSInfo[nSim].nUsed1,
                       gATCurrentAtSMSInfo[nSim].nTotal1,
                       gATCurrentAtSMSInfo[nSim].nUsed2,
                       gATCurrentAtSMSInfo[nSim].nTotal2,
                       gATCurrentAtSMSInfo[nSim].nUsed3,
                       gATCurrentAtSMSInfo[nSim].nTotal3);
            AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, (UINT16)AT_StrLen(PromptBuff), pParam->nDLCI, nSim);
            return;
        }
        else
        {

            gATCurrentAtSMSInfo[nSim].nStorage1 = nStorage1Last;
            gATCurrentAtSMSInfo[nSim].nStorage2 = nStorage2Last;
            gATCurrentAtSMSInfo[nSim].nStorage3 = nStorage3Last;
            AT_SMS_Result_Err(nOperationRet, NULL, 0, "cpms set mode: CfgGetSmsStorageInfo() exception", pParam->nDLCI, nSim);
            return;
        }

        break;

    case AT_CMD_READ:
        // ///////////////////////////////
        /* read mem1 buffer                        */
        // ///////////////////////////////

        nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, gATCurrentAtSMSInfo[nSim].nStorage1, nSim);
        if (ERR_SUCCESS == nOperationRet)
        {
            gATCurrentAtSMSInfo[nSim].nTotal1 = sStorageInfo.totalSlot;
            gATCurrentAtSMSInfo[nSim].nUsed1 = sStorageInfo.usedSlot;
        }
        else
        {
            nOperationRet = ERR_AT_CMS_MEM_FAIL;
        }

        if (gATCurrentAtSMSInfo[nSim].nStorage1 & CFW_SMS_STORAGE_ME)
        {
            strcpy(ReadMem1, "ME");
        }
        else if (gATCurrentAtSMSInfo[nSim].nStorage1 & CFW_SMS_STORAGE_SM)
        {
            strcpy(ReadMem1, "SM");
        }
        else if (gATCurrentAtSMSInfo[nSim].nStorage1 & CFW_SMS_STORAGE_MT)
        {
            strcpy(ReadMem1, "MT");
        }

        // ///////////////////////////////
        /* read mem2 buffer                        */
        // ///////////////////////////////

        nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, gATCurrentAtSMSInfo[nSim].nStorage2, nSim);
        if (ERR_SUCCESS == nOperationRet)
        {
            gATCurrentAtSMSInfo[nSim].nTotal2 = sStorageInfo.totalSlot;
            gATCurrentAtSMSInfo[nSim].nUsed2 = sStorageInfo.usedSlot;
        }
        else
        {
            nOperationRet = ERR_AT_CMS_MEM_FAIL;
        }

        if (gATCurrentAtSMSInfo[nSim].nStorage2 & CFW_SMS_STORAGE_ME)
        {
            strcpy(ReadMem2, "ME");
        }
        else if (gATCurrentAtSMSInfo[nSim].nStorage2 & CFW_SMS_STORAGE_SM)
        {
            strcpy(ReadMem2, "SM");
        }
        else if (gATCurrentAtSMSInfo[nSim].nStorage2 & CFW_SMS_STORAGE_MT)
        {
            strcpy(ReadMem2, "MT");
        }

        // ///////////////////////////////
        /* read mem3 buffer                        */
        // ///////////////////////////////

        nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, gATCurrentAtSMSInfo[nSim].nStorage3, nSim);
        if (ERR_SUCCESS == nOperationRet)
        {
            gATCurrentAtSMSInfo[nSim].nTotal3 = sStorageInfo.totalSlot;
            gATCurrentAtSMSInfo[nSim].nUsed3 = sStorageInfo.usedSlot;
        }
        else
        {
            nOperationRet = ERR_AT_CMS_MEM_FAIL;
        }

        if (gATCurrentAtSMSInfo[nSim].nStorage3 & CFW_SMS_STORAGE_ME)
        {
            strcpy(ReadMem3, "ME");
        }
        else if (gATCurrentAtSMSInfo[nSim].nStorage3 & CFW_SMS_STORAGE_SM)
        {
            strcpy(ReadMem3, "SM");
        }
        else if (gATCurrentAtSMSInfo[nSim].nStorage3 & CFW_SMS_STORAGE_MT)
        {
            strcpy(ReadMem3, "MT");
        }

        if (ERR_SUCCESS == nOperationRet)
        {
            AT_Sprintf(PromptBuff, "+CPMS: \"%s\",%u,%u,\"%s\",%u,%u,\"%s\",%u,%u",
                       ReadMem1,
                       gATCurrentAtSMSInfo[nSim].nUsed1,
                       gATCurrentAtSMSInfo[nSim].nTotal1,
                       ReadMem2,
                       gATCurrentAtSMSInfo[nSim].nUsed2,
                       gATCurrentAtSMSInfo[nSim].nTotal2,
                       ReadMem3,
                       gATCurrentAtSMSInfo[nSim].nUsed3,
                       gATCurrentAtSMSInfo[nSim].nTotal3);
            AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, (UINT16)AT_StrLen(PromptBuff), pParam->nDLCI, nSim);
            return;
        }
        else
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cpms read mode: CfgGetSmsStorageInfo() exception", pParam->nDLCI,
                              nSim);
            return;
        }

        break;

    case AT_CMD_TEST:

        AT_Sprintf(PromptBuff, "+CPMS: (\"ME\",\"SM\",\"MT\"),(\"ME\",\"SM\",\"MT\"),(\"ME\",\"SM\",\"MT\")");
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, (UINT16)AT_StrLen(PromptBuff), pParam->nDLCI, nSim);
        break;

    default:
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, "cpms: wrong cmd mode", pParam->nDLCI, nSim);
        break;
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CMGL
Description     :       CMGL procedure function
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CMGL(AT_CMD_PARA *pParam)
{
    CFW_EVENT nCfwEvent = {
        0,
    };
    CFW_SMS_LIST ListInfo;

    CFW_SMS_MO_INFO_ADD *pSmsMOInfo = NULL;
    CFW_SMS_STORAGE_INFO sStorageInfo = {0};

    UINT8 nParamCount = 0;
    UINT8 nFormat = 0;
    UINT32 nOperationRet = ERR_SUCCESS;
    UINT32 nListMsgRet = ERR_SUCCESS;
    BOOL bReturn = FALSE;
    UINT8 nState[16] = {0};
    UINT16 nParamLen = 0;
    UINT8 PromptBuff[64] = {0};
    UINT8 nUTI = pParam->nDLCI; // change by wulc

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    nCfwEvent.nFlag = nSim;

    AT_ASSERT(NULL != pParam);

    g_SMS_CurCmdStamp = pParam->uCmdStamp;
    bReturn = AT_GetCfwInitSmsInfo(&nCfwEvent);

    if ((!bReturn) || (nCfwEvent.nParam1 == 1))
    {
        AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgl: AT_GetCfwInitSmsInfo() failure", pParam->nDLCI, nSim);
        return;
    }

    nFormat = gATCurrentAtSmsSettingSg_SMSFormat;
    nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, gATCurrentAtSMSInfo[nSim].nStorage1, nSim);
    if (ERR_SUCCESS == nOperationRet)
    {
        gATCurrentAtSMSInfo[nSim].nTotal1 = sStorageInfo.totalSlot;
        gATCurrentAtSMSInfo[nSim].nUsed1 = sStorageInfo.usedSlot;
    }

    /* debug for current storage total and used slot */
    AT_TC(g_sw_AT_SMS, "gATCurrentAtSMSInfo[%d].nTotal1 = %u", nSim, gATCurrentAtSMSInfo[nSim].nTotal1);
    AT_TC(g_sw_AT_SMS, "gATCurrentAtSMSInfo[%d].nUsed1 = %u", nSim, gATCurrentAtSMSInfo[nSim].nUsed1);

    if ((nOperationRet != ERR_SUCCESS) || (nFormat > 1))
    {
        AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgl: Get sms storage or nFormat error", pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:

        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);

        if (nOperationRet != ERR_SUCCESS || nParamCount != 1)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgl: set mode get param counter error", pParam->nDLCI,
                              nSim);
            return;
        }

        if (nFormat)
        {
            nParamLen = 16;
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, (void *)(nState), &nParamLen);
            if (nOperationRet != ERR_SUCCESS)
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgl text mode: get param error", pParam->nDLCI,
                                  nSim);
                return;
            }

            AT_StrUpr(nState);

            if (!AT_SMS_StringOrDataToStatusMacroFlag(nState, nState, nFormat))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgl text mode: StatusMacroFlagToStringOrData 1",
                                  pParam->nDLCI, nSim);
                return;
            }
        }
        else // pdu mode, state in int mode
        {
            nParamLen = 1;

            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, (void *)nState, &nParamLen);

            if (nOperationRet != ERR_SUCCESS)
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0, "cmgl pdu mode: get param error", pParam->nDLCI, nSim);
                return;
            }

            if (!AT_SMS_StringOrDataToStatusMacroFlag(nState, nState, nFormat))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_PARAM, NULL, 0, "cmgl pdu mode: StatusMacroFlagToStringOrData 2",
                                  pParam->nDLCI, nSim);
                return;
            }
        }

        //===========================================
        // 1. list status is: READ, but readRecords is 0, return OK immediately
        // 2. list status is: UNREAD, but unReadRecords is 0, return OK immediately
        // 3. list status is: SENT, but sentRecords is 0, return OK immediately
        // 4. list status is: UNSENT, but unsentRecords is 0, return OK immediately
        // 5. list status is: ALL, but usedSlot is 0, return OK immediately
        //===========================================
        if (((CFW_SMS_STORED_STATUS_READ == nState[0]) && (0 == sStorageInfo.readRecords)) || ((CFW_SMS_STORED_STATUS_UNREAD == nState[0]) && (0 == sStorageInfo.unReadRecords)) || (((CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ & nState[0]) != 0) && (0 == sStorageInfo.sentRecords)) || ((CFW_SMS_STORED_STATUS_UNSENT == nState[0]) && (0 == sStorageInfo.unsentRecords)) || ((CFW_SMS_STORED_STATUS_STORED_ALL == nState[0]) && (0 == sStorageInfo.usedSlot)))
        {
            AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }

        pSmsMOInfo = (CFW_SMS_MO_INFO_ADD *)AT_MALLOC(sizeof(CFW_SMS_MO_INFO_ADD));
        if (!pSmsMOInfo)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_MEM_FAIL, NULL, 0, "cmgl: malloc memory error 1", pParam->nDLCI, nSim);
            return;
        }

        AT_MemSet(pSmsMOInfo, 0, sizeof(CFW_SMS_MO_INFO_ADD));

        //===========================================
        // set list message para value
        //===========================================

        UINT16 nUsedSlot = 0x00;
        UINT16 nMaxSlot = 0x00;
        UINT8 nStatusStorageInfo = nState[0];

        if (nStatusStorageInfo == CFW_SMS_STORED_STATUS_STORED_ALL)
            nStatusStorageInfo = 0x7f;

        SMS_GetStorageInfoEX(&nUsedSlot, &nMaxSlot, nSim, nStatusStorageInfo);

        pSmsMOInfo->i = 0x00;
        pSmsMOInfo->nLocation = gATCurrentAtSMSInfo[nSim].nStorage1;

        if (CFW_SMS_STORAGE_MT == pSmsMOInfo->nLocation)
            pSmsMOInfo->nLocation = CFW_SMS_STORAGE_SM;

        ListInfo.nStorage = pSmsMOInfo->nLocation;
        ListInfo.nCount = pSmsMOInfo->nListCount = gATCurrentAtSMSInfo[nSim].nTotal1; //sStorageInfo.totalSlot;
        ListInfo.nStatus = pSmsMOInfo->nStatus = nState[0];
        ListInfo.nStartIndex = pSmsMOInfo->nListStartIndex = 1;
        ListInfo.nType = CFW_SMS_TYPE_PP;
        ListInfo.nOption = CFW_SMS_LIST_OPTION__TIME_DESCEND_ORDER;

        AT_TC(g_sw_AT_SMS, "ListInfo.nCount = %d ListInfo.nStatus %d", ListInfo.nCount, ListInfo.nStatus);

        /* get the UTI and free it after finished calling */
        if (0 == (nUTI = AT_AllocUserTransID()))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgl: malloc UTI error", pParam->nDLCI, nSim);
            return;
        }
        nListMsgRet = CFW_SmsListMessages(&ListInfo, pParam->nDLCI, nSim);
        AT_TC(g_sw_AT_SMS, "cmgl: pParam->nDLCI nListMsgRet%x ", nListMsgRet);

        AT_FreeUserTransID(nUTI);

        if (nListMsgRet != ERR_SUCCESS)
        {
            AT_FREE(pSmsMOInfo);

            nListMsgRet = AT_SMS_ERR_Change(nListMsgRet);
            AT_SMS_Result_Err(nListMsgRet, NULL, 0, "cmgl: set mode list msg check param error", pParam->nDLCI, nSim);
            return;
        }
        AT_SMS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        AT_FREE(pSmsMOInfo);

        break;
    case AT_CMD_EXE:
    {
        /* if the unReadRecords is 0, return OK immediately */
        if (0 == sStorageInfo.unReadRecords)
        {
            AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }

        /* get the UTI and free it after finished calling */
        if (0 == (nUTI = AT_AllocUserTransID()))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgl: exe mode malloc UTI error", pParam->nDLCI, nSim);
            return;
        }
        nListMsgRet = CFW_SmsListMessages(&ListInfo, pParam->nDLCI, nSim);

        AT_FreeUserTransID(nUTI);
        if (nListMsgRet != ERR_SUCCESS)
        {
            nListMsgRet = AT_SMS_ERR_Change(nListMsgRet);
            AT_SMS_Result_Err(nListMsgRet, NULL, 0, "cmgl: exe mode list msg check param error ", pParam->nDLCI, nSim);
            return;
        }
        AT_SMS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;
    }

    case AT_CMD_TEST:

        if (nFormat)
        {
            AT_Sprintf(PromptBuff, "+CMGL: (\"REC UNREAD\",\"REC READ\",\"STO UNSENT\",\"STO SENT\",\"ALL\")");
        }
        else
        {
            AT_Sprintf(PromptBuff, "+CMGL: (0,1,2,3,4)");
        }
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, (UINT16)AT_StrLen(PromptBuff), pParam->nDLCI, nSim);
        break;

    default:
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, "cmgl: wrong cmd mode", pParam->nDLCI, nSim);
        break;
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CSCA
Description     :       CSCA procedure function
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CSCA(AT_CMD_PARA *pParam)
{

    CFW_EVENT sCfwEvent = {0};
    UINT8 InputSCABuff[32] = {0};
    CFW_SMS_PARAMETER sParaInfo = {0};
    UINT8 *pSCANumBuff = NULL;
    UINT8 nTypeOfSCA = 0;

    UINT8 PromptBuff[128] = {0};
    UINT8 nParamCount = 0;
    UINT16 nParamLen = 0;
    INT32 nOperationRet = 0;
    UINT8 nNumLen = 0;

    BOOL bReturn = FALSE;
    BOOL bIsInternational = FALSE;
    UINT16 nBCDIndex = 0;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    sCfwEvent.nFlag = nSim;
    AT_ASSERT(NULL != pParam);
    g_SMS_CurCmdStamp = pParam->uCmdStamp;
    bReturn = AT_GetCfwInitSmsInfo(&sCfwEvent);
    if ((!bReturn) || (sCfwEvent.nParam1 == 1))
    {
        AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "csca: AT_GetCfwInitSmsInfo() failure", pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:

        /* get current information and update value of some segment */
        nOperationRet = CFW_CfgGetSmsParam(&sParaInfo, 0, nSim);
        if (ERR_SUCCESS != nOperationRet)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "csca: get sms parameters error", pParam->nDLCI, nSim);
            return;
        }

        /* parse the input parameters */
        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);
        if (nParamCount == 0 || nParamCount > 2 || nOperationRet != ERR_SUCCESS)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "csca: get param counter error", pParam->nDLCI, nSim);
            return;
        }

        //===========================================
        // para1, para 2 process these code
        //===========================================
        if (nParamCount > 0)
        {
            nParamLen = 24;
            nOperationRet =
                AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, (void *)InputSCABuff, &nParamLen);

            if ((nOperationRet != ERR_SUCCESS) || (0 == nParamLen))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "csca: AT_Util_GetParaWithRule 1", pParam->nDLCI, nSim);
                return;
            }

            if (!AT_SMS_IsValidPhoneNumber(InputSCABuff, nParamLen, &bIsInternational))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "csca: check nuber invalid", pParam->nDLCI, nSim);
                return;
            }

            if (bIsInternational)
            {
                nParamLen--;
                if (nParamLen > 20)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "csca: number len error 1", pParam->nDLCI, nSim);
                    return;
                }
                pSCANumBuff = &InputSCABuff[1];
            }
            else
            {
                if (nParamLen > 20)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "csca: number len error 2", pParam->nDLCI, nSim);
                    return;
                }

                pSCANumBuff = InputSCABuff;
            }

            nBCDIndex = SUL_AsciiToGsmBcd(pSCANumBuff, nParamLen, &(sParaInfo.nNumber[2]));
        }

        //===========================================
        // para 2 process these code
        //===========================================
        if (nParamCount > 1)
        {
            nParamLen = 1;
            nOperationRet =
                AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, (void *)&nTypeOfSCA, &nParamLen);
            if (ERR_AT_UTIL_CMD_PARA_NULL == nOperationRet)
            {
                nTypeOfSCA = CFW_TELNUMBER_TYPE_UNKNOWN;
            }
            else
            {
                if (ERR_SUCCESS != nOperationRet)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "csca: AT_Util_GetParaWithRule 1", pParam->nDLCI, nSim);
                    return;
                }
            }

            if ((nTypeOfSCA != CFW_TELNUMBER_TYPE_INTERNATIONAL) && (nTypeOfSCA != CFW_TELNUMBER_TYPE_NATIONAL) && (nTypeOfSCA != CFW_TELNUMBER_TYPE_UNKNOWN))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "csca: sca number type error", pParam->nDLCI, nSim);
                return;
            }
        }

        /* according to spec, we check dial number first */
        if (bIsInternational)
        {
            nTypeOfSCA = CFW_TELNUMBER_TYPE_INTERNATIONAL;
        }
        else
        {
            if (0 == nTypeOfSCA)
            {
                /* no input num type  */
                nTypeOfSCA = CFW_TELNUMBER_TYPE_UNKNOWN;
            }
            else if (CFW_TELNUMBER_TYPE_INTERNATIONAL == nTypeOfSCA)
            {
                /* have no '+' , but the num type is 145, replace 129 with is  */
                nTypeOfSCA = CFW_TELNUMBER_TYPE_UNKNOWN;
            }
            else
            {
                /* get the input num type  */
            }
        }

        // set param value
        sParaInfo.nNumber[0] = nBCDIndex + 1;
        sParaInfo.nNumber[1] = nTypeOfSCA;

        AT_TC(g_sw_AT_SMS, "=====  SCA  Value ===== ");
        AT_TC_MEMBLOCK(g_sw_AT_SMS, &sParaInfo.nNumber[0], sParaInfo.nNumber[0] + 1, 16);

        //===========================================
        // update SCA value to sim card
        //===========================================
        sParaInfo.nSaveToSimFlag = 0x01; // set sca to sim card
        nOperationRet = CFW_CfgSetSmsParam(&sParaInfo, pParam->nDLCI, nSim);

        if (ERR_SUCCESS != nOperationRet)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_MEM_FAIL, NULL, 0, "csca: check set para error", pParam->nDLCI, nSim);
            return;
        }

        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    case AT_CMD_READ:

        nOperationRet = CFW_CfgGetSmsParam(&sParaInfo, 0, nSim);

        if (ERR_SUCCESS != nOperationRet)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "csca: get prarameters error 11!", pParam->nDLCI, nSim);
            break;
        }
        if (sParaInfo.nNumber[0])
        {
            nOperationRet = SUL_GsmBcdToAscii(&(sParaInfo.nNumber[2]), sParaInfo.nNumber[0] - 1, InputSCABuff);

            if ((sParaInfo.nNumber[sParaInfo.nNumber[0]] & 0xf0) == 0xf0)
            {
                nNumLen = (sParaInfo.nNumber[0] - 1) * 2 - 1;
            }
            else
            {
                nNumLen = (sParaInfo.nNumber[0] - 1) * 2;
            }
        }
        else
        {
            nNumLen = 0x00;
        }

        InputSCABuff[nNumLen] = '\0';

        if (sParaInfo.nNumber[1] == 0x91)
        {
            nTypeOfSCA = '+';
            AT_Sprintf(PromptBuff, "+CSCA: \"%c%s\",%u", nTypeOfSCA, InputSCABuff, sParaInfo.nNumber[1]);
        }
        else
        {
            AT_Sprintf(PromptBuff, "+CSCA: \"%s\",%u", InputSCABuff, sParaInfo.nNumber[1]);
        }
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), pParam->nDLCI, nSim);
        break;

    case AT_CMD_TEST:

        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    default:
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, "csca: wrong cmd mode", pParam->nDLCI, nSim);
        break;
    }
    return;
}

/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CSMS
Description     :       CMGS procedure function
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CSMS(AT_CMD_PARA *pParam)
{
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_ASSERT(NULL != pParam);
#ifdef DEBUG_AT
    AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
#else

    switch (pParam->iType)
    {
    /* continue to implements...... */
    case AT_CMD_TEST:
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_NOTSUPPORT, 0, NULL, 0, pParam->nDLCI, nSim);
        break;
    case AT_CMD_SET:

        /* continue to implements...... */
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_NOTSUPPORT, 0, NULL, 0, pParam->nDLCI, nSim);
        break;
    case AT_CMD_READ:

        /* continue to implements...... */
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_NOTSUPPORT, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    default:
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, NULL, pParam->nDLCI, nSim);
        break;
    }
#endif
    return;
}

/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CMGD
Description     :       CMGD procedure function
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CMGD(AT_CMD_PARA *pParam)
{
    CFW_SMS_MO_INFO_ADD *pSmsMOInfo = NULL;
    CFW_EVENT nCfwEvent = {
        0,
    };
    INT32 nOperationRet = ERR_SUCCESS;
    BOOL bReturn = FALSE;
    UINT8 nStorage = 0;
    CFW_SMS_STORAGE_INFO sStorageInfo = {0};
    UINT8 PromptInfo[32] = {0};
    UINT16 nUTI = 0;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    nCfwEvent.nFlag = nSim;

    AT_ASSERT(NULL != pParam);

    g_SMS_CurCmdStamp = pParam->uCmdStamp;

    bReturn = AT_GetCfwInitSmsInfo(&nCfwEvent);
    if ((!bReturn) || (nCfwEvent.nParam1 == 1))
    {
        AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgd: SMS INIT FAIL", pParam->nDLCI, nSim);
        return;
    }

    /* check storage1 have no error and get totalslot */
    nStorage = gATCurrentAtSMSInfo[nSim].nStorage1;

    nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, nStorage, nSim);
    if (nOperationRet != ERR_SUCCESS)
    {
        AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgd: CFW_CfgGetSmsStorageInfo", pParam->nDLCI, nSim);
        return;
    }

    //===========================================
    // /      procedure
    //===========================================
    switch (pParam->iType)
    {

    case AT_CMD_SET:
    {
        UINT8 nParamCount = 0;
        UINT8 nDeleteFlag = 0;
        UINT16 nDelMsgIndex = 0;
        UINT16 nParamLen = 0;
        UINT8 nStatus = 0;

        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);

        if (nOperationRet != ERR_SUCCESS || nParamCount > 2)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmgd: nParamCount >2", pParam->nDLCI, nSim);
            return;
        }

        //===========================================
        // para1, para 2 process these code
        //===========================================
        if (nParamCount > 0)
        {
            nParamLen = 2;

            nOperationRet =
                AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT16, (void *)(&nDelMsgIndex), &nParamLen);
            if (nOperationRet != ERR_SUCCESS)
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmgd: get para 1 error", pParam->nDLCI, nSim);
                return;
            }

            // if(nDelMsgIndex < 0 || nDelMsgIndex > sStorageInfo.totalSlot)  //modify by wulc
            if (nDelMsgIndex < 1 || nDelMsgIndex > sStorageInfo.totalSlot)
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_MEM_INDEX, NULL, 0, "cmgd: nDelMsgIndex > sStorageInfo.totalSlot",
                                  pParam->nDLCI, nSim);
                return;
            }
        }

        //===========================================
        // para 2 process these code
        //===========================================
        if (nParamCount > 1)
        {
            nParamLen = 1;

            nOperationRet =
                AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, (void *)(&nDeleteFlag), &nParamLen);
            if (nOperationRet != ERR_SUCCESS)
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmgd: get para2 error", pParam->nDLCI, nSim);
                return;
            }
        }

        /* Get the delete flag flag */
        switch (nDeleteFlag)
        {
        case 0:
            nStatus = 0;
            break;

        case 1:

            nStatus = CFW_SMS_STORED_STATUS_READ;
            if (0 == sStorageInfo.readRecords)
            {
                AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
                return;
            }
            break;

        case 2:

            nStatus =
                CFW_SMS_STORED_STATUS_READ | CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ |
                CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV | CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE |
                CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE;
            if ((0 == sStorageInfo.readRecords) && (0 == sStorageInfo.unReadRecords))
            {
                AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
                return;
            }

            break;

        case 3:

            nStatus =
                CFW_SMS_STORED_STATUS_READ | CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ |
                CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV | CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE |
                CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE | CFW_SMS_STORED_STATUS_UNSENT;
            if ((0 == sStorageInfo.readRecords) && (0 == sStorageInfo.unReadRecords) && (0 == sStorageInfo.sentRecords) && (0 == sStorageInfo.unsentRecords))
            {
                AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
                return;
            }

            break;

        case 4:

            nStatus = CFW_SMS_STORED_STATUS_STORED_ALL;
            if ((0 == sStorageInfo.readRecords) && (0 == sStorageInfo.unReadRecords) && (0 == sStorageInfo.sentRecords) && (0 == sStorageInfo.unsentRecords))
            {
                AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
                return;
            }
            break;

        default:
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmgd: delete flag para error", pParam->nDLCI, nSim);
            return;
        }

        // add by wulc for  AT+CMGD=0  begin
        if (0 == nDelMsgIndex)
        {
            nStatus = CFW_SMS_STORED_STATUS_STORED_ALL;
        }

        // add by wulc end
        //===========================================
        // 1. list status is: READ, but readRecords is 0, return OK immediately
        // 2. list status is: UNREAD, but unReadRecords is 0, return OK immediately
        // 3. list status is: SENT, but sentRecords is 0, return OK immediately
        // 4. list status is: UNSENT, but unsentRecords is 0, return OK immediately
        // 5. list status is: ALL, but usedSlot is 0, return OK immediately
        //===========================================
        /* starting delete message */
        pSmsMOInfo = (CFW_SMS_MO_INFO_ADD *)AT_MALLOC(sizeof(CFW_SMS_MO_INFO_ADD));

        if (pSmsMOInfo == NULL)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_MEM_FAIL, NULL, 0, "cmgd: pSmsMOInfo malloc error", pParam->nDLCI, nSim);
            return;
        }

        AT_MemSet(pSmsMOInfo, 0, sizeof(CFW_SMS_MO_INFO_ADD));

        // If the nDeleteFlag is given, then we delete kinds of message,
        // and the nDelMsgIndex is omit and value is set to 0
        if (nDeleteFlag != 0)
        {
            nDelMsgIndex = 0;
        }

        pSmsMOInfo->nLocation = gATCurrentAtSMSInfo[nSim].nStorage1;
        pSmsMOInfo->nIndex = nDelMsgIndex;
        pSmsMOInfo->nStatus = nStatus;

        /* get the UTI and free it after finished calling */
        if (0 == (nUTI = AT_AllocUserTransID()))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgd:  malloc UTI error", nSim, pParam->nDLCI);
            return;
        }

        if (CFW_SMS_STORAGE_MT == pSmsMOInfo->nLocation)
            pSmsMOInfo->nLocation = CFW_SMS_STORAGE_SM;

        nOperationRet = CFW_SmsDeleteMessage(pSmsMOInfo->nIndex, pSmsMOInfo->nStatus,
                                             pSmsMOInfo->nLocation, 1, pParam->nDLCI, nSim);
        AT_FreeUserTransID(nUTI);

        if (nOperationRet != ERR_SUCCESS)
        {
            AT_FREE(pSmsMOInfo);
            nOperationRet = AT_SMS_ERR_Change(nOperationRet);
            AT_SMS_Result_Err(nOperationRet, NULL, 0, "cmgd: CFW_SmsDeleteMessage error", pParam->nDLCI, nSim);
            return;
        }

        /* update usedslot at async handler, no need to set again here --- by qidd
                    if((pSmsMOInfo->nLocation = CFW_SMS_STORAGE_SM) || (nStorage = CFW_SMS_STORAGE_SM))
                    {
                        sStorageInfo.usedSlot--;
                        CFW_CfgSetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSim);
                        AT_TC(g_sw_AT_SAT, "CMGD ok usedSlot= %d", sStorageInfo.usedSlot);
                    }
        */
        AT_FREE(pSmsMOInfo);
        AT_SMS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;
    }

    case AT_CMD_TEST:
    {

        AT_Sprintf(PromptInfo, "+CMGD: (1-%u),(0,1,2,3,4)", sStorageInfo.totalSlot);
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptInfo, AT_StrLen(PromptInfo), pParam->nDLCI, nSim);
        break;
        ;
    }

    default:
    {
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, "cmgd: wrong cmd mode ", pParam->nDLCI, nSim);
        break;
    }
    }

    return;
}

/******************************************************************************************
Function            :   AT_CmdFunc_CMGF
Description     :       CMGF command procedure function
                    Set SMS Format include "text"(1) and "PDU"(0)
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/

VOID AT_SMS_CmdFunc_CMGF(AT_CMD_PARA *pParam)
{
    UINT16 nParaLen = 0;
    UINT8 nParaCount = 0;
    UINT8 nFormat = 0;
    UINT8 PromptInfoBuff[16] = {0};
    UINT32 nOperationRet = ERR_SUCCESS;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_ASSERT(NULL != pParam);

    switch (pParam->iType)
    {
    case AT_CMD_SET:

        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParaCount);
        if (nParaCount > 1 || nOperationRet != ERR_SUCCESS || (nParaCount == 0))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmgf: get param counter error", pParam->nDLCI, nSim);
            return;
        }

        nParaLen = 1;
        nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, (void *)&nFormat, &nParaLen);
        if ((nOperationRet != ERR_SUCCESS) || (nFormat > 1))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmgf: get para 1 error", pParam->nDLCI, nSim);
            return;
        }

        nOperationRet = CFW_CfgSetSmsFormat(nFormat, nSim);
        if (nOperationRet != ERR_SUCCESS)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgf: CFW_CfgSetSmsFormat error 1", pParam->nDLCI, nSim);
            return;
        }

        gATCurrentAtSmsSettingSg_SMSFormat = nFormat;
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    case AT_CMD_TEST:

        AT_Sprintf(PromptInfoBuff, "+CMGF:(0,1)");
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)PromptInfoBuff, AT_StrLen(PromptInfoBuff), pParam->nDLCI,
                         nSim);
        break;

    case AT_CMD_READ:

        nFormat = gATCurrentAtSmsSettingSg_SMSFormat;

        AT_Sprintf(PromptInfoBuff, "+CMGF: %u", nFormat);
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)PromptInfoBuff, AT_StrLen(PromptInfoBuff), pParam->nDLCI,
                         nSim);
        break;

    default:
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, " cmgf: wrong cmd mode", pParam->nDLCI, nSim);
        break;
    }

    return;
}

/******************************************************************************************
Function            :   AT_CmdFunc_CMMS
Description     :       CMMS command procedure function
                    Set SMS Concat include "long sms"(ture) and "common sms"(false)
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CMMS(AT_CMD_PARA *pParam)
{
    UINT16 nParaLen = 0;
    UINT8 nParaCount = 0;
    UINT8 nConcat = 0;
    UINT8 PromptInfoBuff[16] = {0};
    UINT32 nOperationRet = ERR_SUCCESS;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_ASSERT(NULL != pParam);

    switch (pParam->iType)
    {
    case AT_CMD_SET:

        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParaCount);
        if (nParaCount > 1 || nOperationRet != ERR_SUCCESS || (nParaCount == 0))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmms: get param counter error", pParam->nDLCI, nSim);
            return;
        }

        nParaLen = 1;
        nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, (void *)&nConcat, &nParaLen);
        if ((nOperationRet != ERR_SUCCESS) || (nConcat > 1))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "cmms: get para 1 error", pParam->nDLCI, nSim);
            return;
        }

        g_SMSConcat[nSim] = (BOOL)nConcat;
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    case AT_CMD_TEST:

        AT_Sprintf(PromptInfoBuff, "+CMMS:(0,1)");
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)PromptInfoBuff, AT_StrLen(PromptInfoBuff), pParam->nDLCI,
                         nSim);
        break;

    case AT_CMD_READ:

        nConcat = (UINT8)g_SMSConcat[nSim];

        AT_Sprintf(PromptInfoBuff, "+CMMS: %u", nConcat);
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)PromptInfoBuff, AT_StrLen(PromptInfoBuff), pParam->nDLCI,
                         nSim);
        break;

    default:
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, " cmms: wrong cmd mode", pParam->nDLCI, nSim);
        break;
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CMGR
Description     :       CMGR procedure function
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CMGR(AT_CMD_PARA *pParam)
{
    CFW_EVENT nCfwEvent = {
        0,
    };

    CFW_SMS_MO_INFO_ADD *pSmsMOInfo = NULL;
    INT32 nOperationRet = ERR_SUCCESS;
    INT32 nReadMsgRet = ERR_SUCCESS;
    BOOL bReturn = FALSE;
    CFW_SMS_STORAGE_INFO sStorageInfo = {0};
    UINT8 nStorage = 0;
    UINT16 nReadMsgIndex = 0;
    UINT16 nParamLen = 0;
    UINT8 nParamCount = 0;
    UINT16 nUTI = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    nCfwEvent.nFlag = nSim;

    AT_ASSERT(NULL != pParam);

    g_SMS_CurCmdStamp = pParam->uCmdStamp;
    bReturn = AT_GetCfwInitSmsInfo(&nCfwEvent);

    if ((bReturn == 0) || (nCfwEvent.nParam1 == 1))
    {
        AT_TC(g_sw_AT_SAT, "AT_GetCfwInitSmsInfo bReturn %d,nCfwEvent.nParam1 %d", bReturn, nCfwEvent.nParam1);
        AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgr: SMS INIT FAIL\n", pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
    {
        /* get param count */
        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);
        if (nOperationRet != ERR_SUCCESS || nParamCount != 1)
        {
            AT_TC(g_sw_AT_SAT, "AT_Util_GetParaCount %d", nOperationRet);
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgr: get param counter error", pParam->nDLCI, nSim);
            return;
        }

        /* check storage1 have no error and get totalslot */
        nParamLen = 2;
        nStorage = gATCurrentAtSMSInfo[nSim].nStorage1;
        if (nStorage == CFW_SMS_STORAGE_MT)
        {
            nStorage = CFW_SMS_STORAGE_SM;
        }
        AT_TC(g_sw_AT_SAT, "nStorage %d", nStorage);
        nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, nStorage, nSim);

        if (nOperationRet != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SAT, "CFW_CfgGetSmsStorageInfo error %d", nOperationRet);
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgr: get sms storage error", pParam->nDLCI, nSim);
            return;
        }

        nOperationRet =
            AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT16, (void *)(&nReadMsgIndex), &nParamLen);

        if (nOperationRet != ERR_SUCCESS)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_TXT_PARAM, NULL, 0, "cmgr: get sms storage error", pParam->nDLCI, nSim);
            return;
        }

        if (nReadMsgIndex > sStorageInfo.totalSlot)
        {
            AT_TC(g_sw_AT_SAT, "nReadMsgIndex %d, sStorageInfo.totalSlot %d", nReadMsgIndex, sStorageInfo.totalSlot);
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_MEM_INDEX, NULL, 0, "cmgr: nReadMsgIndex > sStorageInfo.totalSlot",
                              pParam->nDLCI, nSim);
            return;
        }

        //===========================================
        // Starting read message from csw
        //===========================================
        pSmsMOInfo = (CFW_SMS_MO_INFO_ADD *)AT_MALLOC(sizeof(CFW_SMS_MO_INFO_ADD));

        if (NULL == pSmsMOInfo)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_MEM_FAIL, NULL, 0, "cmgr: pSmsMOInfo malloc error", pParam->nDLCI, nSim);
            return;
        }

        AT_MemSet(pSmsMOInfo, 0, sizeof(CFW_SMS_MO_INFO_ADD));
        pSmsMOInfo->nFormat = gATCurrentAtSmsSettingSg_SMSFormat;
        nOperationRet = CFW_CfgGetSmsShowTextModeParam(&(pSmsMOInfo->nPath), nSim);

        if (ERR_SUCCESS != nOperationRet)
        {
            AT_TC(g_sw_AT_SAT, "CFW_CfgGetSmsShowTextModeParam %d", nOperationRet);
            AT_FREE(pSmsMOInfo);
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgr: status exception", pParam->nDLCI, nSim);
            return;
        }

        pSmsMOInfo->nIndex = nReadMsgIndex;
        pSmsMOInfo->nLocation = nStorage;

        /* get the UTI and free it after finished calling */
        if (0 == (nUTI = AT_AllocUserTransID()))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgr: malloc UTI error", pParam->nDLCI, nSim);
            return;
        }

        nReadMsgRet = CFW_SmsReadMessage(pSmsMOInfo->nLocation, 1, pSmsMOInfo->nIndex, pParam->nDLCI, nSim);
        AT_FreeUserTransID(nUTI);

        if (ERR_SUCCESS != nReadMsgRet)
        {
            AT_TC(g_sw_AT_SAT, "CFW_SmsReadMessage err %d", nReadMsgRet);
            AT_FREE(pSmsMOInfo);

            nReadMsgRet = AT_SMS_ERR_Change(nReadMsgRet);
            AT_SMS_Result_Err(nReadMsgRet, NULL, 0, "cmgr: read message check param error", pParam->nDLCI, nSim);
            return;
        }

        AT_FREE(pSmsMOInfo);
        AT_SMS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 10, NULL, 0, pParam->nDLCI, nSim);
        break;
    }
    case AT_CMD_TEST:
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    default:
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, "cmgr: wrong cmd mode", pParam->nDLCI, nSim);
        break;
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CSDH
Description     :       CSDH procedure function
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CSDH(AT_CMD_PARA *pParam)
{

    UINT8 PromptInfoBuff[16] = {0};
    UINT16 nParamLen = 0;
    UINT8 nParamCount = 0;
    UINT8 nHeaderShow = 0;
    INT32 nOperationRet = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_ASSERT(NULL != pParam);

    switch (pParam->iType)
    {
    case AT_CMD_SET:
    {
        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);

        if (nOperationRet != ERR_SUCCESS || nParamCount == 0 || nParamCount > 1)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "csdh: get param counter error", pParam->nDLCI, nSim);
            return;
        }

        nParamLen = 1;
        nOperationRet =
            AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, (void *)&nHeaderShow, &nParamLen);

        if (ERR_SUCCESS != nOperationRet)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "csdh: get param error", pParam->nDLCI, nSim);
            return;
        }

        if (nHeaderShow == 1 || nHeaderShow == 0)
        {
            gATCurrentAtSmsSettingSCSDH_show = nHeaderShow;
            AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        }
        else
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "csdh: HeaderShow exception", pParam->nDLCI, nSim);
        }

        break;
    }

    case AT_CMD_TEST:
    {

        AT_Sprintf(PromptInfoBuff, "+CSDH: (0,1)");
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)PromptInfoBuff, AT_StrLen(PromptInfoBuff), pParam->nDLCI,
                         nSim);
        break;
    }

    case AT_CMD_READ:
    {

        nHeaderShow = gATCurrentAtSmsSettingSCSDH_show;
        AT_Sprintf(PromptInfoBuff, "+CSDH: %u", nHeaderShow);
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)PromptInfoBuff, AT_StrLen(PromptInfoBuff), pParam->nDLCI,
                         nSim);
        break;
    }

    default:
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, "csdh: wrong cmd mode", pParam->nDLCI, nSim);
        break;
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CNMI
Description     :       selects the procedure, how receiving of new messages from the
                    network is indicated to the TE when TE is active
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CNMI(AT_CMD_PARA *pParam)
{

    UINT8 mode = 0, mt = 0, bm = 0, ds = 0, bfr = 0;
    UINT8 modeLast = 0, mtLast = 0, bmLast = 0, dsLast = 0, bfrLast = 0;
    UINT16 nParamLen = 1;
    UINT8 nParamCount = 1;
    INT32 nOperationRet = ERR_SUCCESS;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_ASSERT(NULL != pParam);

    switch (pParam->iType)
    {
    case AT_CMD_SET:
    {
        //===========================================
        // Save the lates value and restore if exception
        //===========================================
        modeLast = gATCurrentAtSMSInfo[nSim].sCnmi.nMode;
        mtLast = gATCurrentAtSMSInfo[nSim].sCnmi.nMt;
        bmLast = gATCurrentAtSMSInfo[nSim].sCnmi.nBm;
        dsLast = gATCurrentAtSMSInfo[nSim].sCnmi.nDs;
        bfrLast = gATCurrentAtSMSInfo[nSim].sCnmi.nBfr;

        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);
        if (nOperationRet != ERR_SUCCESS || nParamCount > 5)
        {
            nOperationRet = ERR_AT_CMS_INVALID_PARA;
        }

        //===========================================
        // //all param is omit , 0 param
        //===========================================
        if (0 == nParamCount)
        {
            nOperationRet = ERR_SUCCESS;
            gATCurrentAtSMSInfo[nSim].sCnmi.nMode = 0;
        }

        //===========================================
        // //           get para1: mode = 0
        //===========================================
        if (nOperationRet == ERR_SUCCESS && nParamCount)
        {
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, (void *)&mode, &nParamLen);

            if ((mode != 0) && (mode != 1) && (mode != 2) & (mode != 3))
            {
                nOperationRet = ERR_AT_CMS_INVALID_PARA;
            }
            else
            {
                gATCurrentAtSMSInfo[nSim].sCnmi.nMode = mode;
            }
        }

        //===========================================
        // //           get para2: mt 0-2
        //===========================================
        if (nOperationRet == ERR_SUCCESS && nParamCount > 1)
        {
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, (void *)&mt, &nParamLen);

            // if(mt > 2)//[mod]2007.10.18
            if ((mt != 0) && (mt != 1) && (mt != 2) && (mt != 3))
            {
                nOperationRet = ERR_AT_CMS_INVALID_PARA;
            }
            else
            {
                gATCurrentAtSMSInfo[nSim].sCnmi.nMt = mt;
            }
        }

        //===========================================
        // //           get para3: bm 0-3
        //===========================================
        if (nOperationRet == ERR_SUCCESS && nParamCount > 2)
        {
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT8, (void *)&bm, &nParamLen);
            if ((bm != 0) && (bm != 2)) // (bm > 3)---->(bm != 0)
            {
                nOperationRet = ERR_AT_CMS_INVALID_PARA;
            }
            else
            {
                gATCurrentAtSMSInfo[nSim].sCnmi.nBm = bm;
            }
        }

        //===========================================
        // //           get para4: ds 0-2
        //===========================================
        if (nOperationRet == ERR_SUCCESS && nParamCount > 3)
        {
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_UINT8, (void *)&ds, &nParamLen);
            if (ds > 1) // (ds > 2)--->(ds > 1)//[mod]2007.10.18
            {
                nOperationRet = ERR_AT_CMS_INVALID_PARA;
            }
            else
            {
                gATCurrentAtSMSInfo[nSim].sCnmi.nDs = ds;
            }
        }

        //===========================================
        // //           get para5:  bfr 0-1
        //===========================================
        if (nOperationRet == ERR_SUCCESS && nParamCount > 4)
        {
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 4, AT_UTIL_PARA_TYPE_UINT8, (void *)&bfr, &nParamLen);
            if ((bfr != 0) && (bfr != 1))
            {
                nOperationRet = ERR_AT_CMS_INVALID_PARA;
            }
            else
            {
                gATCurrentAtSMSInfo[nSim].sCnmi.nBfr = bfr;
            }
        }

        /* Proces the result  */
        if (nOperationRet == ERR_SUCCESS)
        {
            UINT8 nOption = 0;
            UINT8 nNewSmsStorage = 0;

            nOperationRet = CFW_CfgGetNewSmsOption(&nOption, &nNewSmsStorage, nSim);
            if (nOperationRet == ERR_SUCCESS)
            {
                nOption = nOption & 0x1f;
                nOperationRet =
                    CFW_CfgSetNewSmsOption(nOption | CFW_SMS_ROUT_DETAIL_INFO, gATCurrentAtSMSInfo[nSim].nStorage3, nSim);
                if (nOperationRet == ERR_SUCCESS)
                {
                    AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
                }
            }

            if (ERR_SUCCESS != nOperationRet)
            {

                nOperationRet = ERR_AT_CMS_CMD_CANNOT_ACT;
            }
        }
        else
        {

            gATCurrentAtSMSInfo[nSim].sCnmi.nMode = modeLast;
            gATCurrentAtSMSInfo[nSim].sCnmi.nMt = mtLast;
            gATCurrentAtSMSInfo[nSim].sCnmi.nBm = bmLast;
            gATCurrentAtSMSInfo[nSim].sCnmi.nDs = dsLast;
            gATCurrentAtSMSInfo[nSim].sCnmi.nBfr = bfrLast;
        }
        CFW_SMS_PARAMETER sInfo;
        CFW_CfgGetDefaultSmsParam(&sInfo, nSim);
        sInfo.ssr = gATCurrentAtSMSInfo[nSim].sCnmi.nDs;
        CFW_CfgSetDefaultSmsParam(&sInfo, nSim);
        break;
    }
    case AT_CMD_READ:
    {
        UINT8 AtTri[64] = {0};
        AT_Sprintf(AtTri, "+CNMI: %u,%u,%u,%u,%u",
                   gATCurrentAtSMSInfo[nSim].sCnmi.nMode, gATCurrentAtSMSInfo[nSim].sCnmi.nMt, gATCurrentAtSMSInfo[nSim].sCnmi.nBm,
                   gATCurrentAtSMSInfo[nSim].sCnmi.nDs, gATCurrentAtSMSInfo[nSim].sCnmi.nBfr);
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtTri, (UINT16)AT_StrLen(AtTri), pParam->nDLCI, nSim);
        break;
    }
    case AT_CMD_TEST:
    {
        UINT8 AtTri[64] = {0};

        AT_Sprintf(AtTri, "+CNMI: (0-3),(0-3),(0,2),(0-1),(0,1)");
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AtTri, (UINT16)AT_StrLen(AtTri), pParam->nDLCI, nSim);
        break;
    }
    default:
    {
        nOperationRet = ERR_AT_CMS_OPER_NOT_SUPP;
        break;
    }
    }

    if (nOperationRet != ERR_SUCCESS)
    {
        AT_SMS_Result_Err(nOperationRet, NULL, 0, "cnmi: exception happened", pParam->nDLCI, nSim);
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CSMP
Description     :       CSMP procedure function
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CSMP(AT_CMD_PARA *pParam)
{

    CFW_SMS_PARAMETER sInfo = {0};

    UINT16 nParamLen = 1;
    UINT8 nParamCount = 0;
    UINT8 fo = 0, vp = 0, pid = 0, dcs = 0;

    UINT8 PromptInfoBuff[20] = {0};
    UINT32 nOperationRet = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_ASSERT(NULL != pParam);

    switch (pParam->iType)
    {
    case AT_CMD_SET:
    {

        /* get the current SMS parameters and then update them */

        nOperationRet = CFW_CfgGetSmsParam(&sInfo, 0, nSim);
        if (ERR_SUCCESS != nOperationRet)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "csmp:CfgGetSmsParam exception ", pParam->nDLCI, nSim);
            return;
        }

        /* parser the input parameters */
        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);
        if (nParamCount > 4 || nOperationRet != ERR_SUCCESS) // [mod]2007.10.17
        {
            nOperationRet = ERR_AT_CMS_INVALID_PARA;
        }

        // get NO.1 param fo
        if (nOperationRet == ERR_SUCCESS && nParamCount)
        {
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, (void *)&fo, &nParamLen);
            if (nOperationRet == ERR_SUCCESS)
            {

                /****************************************/
                // "fo" composition :
                // bit    7    6      5     4      3     2    1     0
                // RP UDHI SSR VPF  VPF  RD MTI MTI
                /****************************************/
                // sInfo.mti = fo;
                // for version 1.0 20070918
                // [+]2007.11.20 check param fo only=17
                if (fo != 17)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "csmp: get param failure", pParam->nDLCI, nSim);
                    return;
                }
            }
            else
            {
                nOperationRet = ERR_AT_CMS_INVALID_PARA;
            }
        }

        // get NO.2 param vp
        if (nOperationRet == ERR_SUCCESS && nParamCount > 1)
        {
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, (void *)&vp, &nParamLen);

            if (nOperationRet == ERR_SUCCESS)
            {
                sInfo.vp = vp;
            }
            else
            {
                nOperationRet = ERR_AT_CMS_INVALID_PARA;
            }
        }

        // get NO.3 param pid
        if (nOperationRet == ERR_SUCCESS && nParamCount > 2)
        {
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT8, (void *)&pid, &nParamLen);
            if (nOperationRet == ERR_SUCCESS)
            {
                sInfo.pid = pid;
            }
            else
            {
                nOperationRet = ERR_AT_CMS_INVALID_PARA;
            }
        }

        // get NO.4 param dcs
        if (nOperationRet == ERR_SUCCESS && nParamCount > 3)
        {
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_UINT8, (void *)&dcs, &nParamLen);
            if (nOperationRet == ERR_SUCCESS)
            {
                if (dcs == 0 || dcs == 4 || dcs == 8) // [+]bug 7035
                {
                    sInfo.dcs = dcs;
                }
                else
                {
                    nOperationRet = ERR_AT_CMS_DCS_NOT_SUPP;
                }
            }
            else
            {
                nOperationRet = ERR_AT_CMS_INVALID_PARA;
            }
        }

        if (ERR_SUCCESS == nOperationRet)
        {
            nOperationRet = CFW_CfgSetSmsParam(&sInfo, pParam->nDLCI, nSim);

            if (ERR_SUCCESS != nOperationRet)
            {
                nOperationRet = ERR_AT_CMS_MEM_FAIL;
            }
        }

        if (ERR_SUCCESS == nOperationRet)
        {
            AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        }
        else
        {
            AT_SMS_Result_Err(nOperationRet, NULL, 0, NULL, pParam->nDLCI, nSim);
        }

        break;
    }

    case AT_CMD_TEST:
    {

        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;
    }

    case AT_CMD_READ:
    {
        /*
           ** default value of 'fo' is 17, we can get the value
           ** of CFW_SMS_PARAMETER
         */
        UINT8 nSMSParamFo = 0;
        nOperationRet = CFW_CfgGetSmsParam(&sInfo, 0, nSim);

        if (ERR_SUCCESS == nOperationRet)
        {
            nSMSParamFo = 17;
            AT_Sprintf(PromptInfoBuff, "+CSMP: %u,%u,%u,%u", nSMSParamFo, sInfo.vp, sInfo.pid, sInfo.dcs);
            AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)PromptInfoBuff, AT_StrLen(PromptInfoBuff), pParam->nDLCI,
                             nSim);
        }
        else
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, (UINT8 *)PromptInfoBuff, AT_StrLen(PromptInfoBuff), NULL, pParam->nDLCI,
                              nSim);
        }
        break;
    }

    default:
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, "csmp: wrong cmd mode ", pParam->nDLCI, nSim);
        break;
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CNMA
Description     :       CNMA procedure function
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CNMA(AT_CMD_PARA *pParam)
{
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_ASSERT(NULL != pParam);

    switch (pParam->iType)
    {
    /* continue to implements...... */
    case AT_CMD_TEST:
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_NOTSUPPORT, 0, NULL, 0, pParam->nDLCI, nSim);
        break;
    case AT_CMD_SET:

        /* continue to implements...... */
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;
    case AT_CMD_READ:

        /* continue to implements...... */
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_NOTSUPPORT, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    default:
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, NULL, pParam->nDLCI, nSim);
        break;
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CSAS
Description     :       save +CSAS and +CSMP in non-volatile memory(usually SIM), and non-supported
                    settings are not saved.
Test command        :   AT+CSAS?
Exe command         :       AT+CSAS
Called By           :   AT_RunCommand()
Data Accessed       :
Data Updated        :
Input           :       pParam
Output          :
Return          :   void
Others          :   build by wangqunyang 21/03/2008
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CSAS(AT_CMD_PARA *pParam)
{
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_ASSERT(NULL != pParam);

    switch (pParam->iType)
    {
    case AT_CMD_TEST:
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_NOTSUPPORT, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    case AT_CMD_SET:
        /* continue to implements...... */
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_NOTSUPPORT, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    default:
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, NULL, pParam->nDLCI, nSim);
        break;
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CRES
Description     :       restores or recover SMS settings(+CSMP and +CSCA parameters) from the
                    non-volatile memeory,usually SIM.

Test command        :   AT+CRES?
Exe command         :       AT+CRES
Called By           :   AT_RunCommand()
Data Accessed       :
Data Updated        :
Input           :       pParam
Output          :
Return          :   void
Others          :   build by wangqunyang 21/03/2008
*******************************************************************************************/
VOID AT_SMS_CmdFunc_CRES(AT_CMD_PARA *pParam)
{
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_ASSERT(NULL != pParam);

    switch (pParam->iType)
    {
    case AT_CMD_TEST:
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_NOTSUPPORT, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    case AT_CMD_SET:
        /* continue to implements...... */
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_NOTSUPPORT, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    default:
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, NULL, pParam->nDLCI, nSim);
        break;
    }

    return;
}

#if 0
//extern CB_CTX_T;
typedef struct
{
    UINT16 SimFileSize ;
    UINT8   CbActiveStatus;
    UINT8   CbType;
    UINT8   MidNb;
    UINT16   Mid[20];
    UINT8   DcsNb;
    UINT8   Dcs[20];
} CB_CTX_TYPE;
#endif
typedef struct
{
    UINT16 SimFileSize;
    UINT8 CbActiveStatus;
    UINT8 CbType;
    UINT8 MidNb;
    UINT16 Mid[20];
    UINT8 MirNb;
    UINT16 Mir[20];
    UINT8 DcsNb;
    UINT8 Dcs[17];
} CB_CTX_TYPE;
#define AT_CSCB_MODE_MASK 0
/******************************************************************************************
Function            :   AT_SMS_CmdFunc_CSCB
Description     :
Test command        :   AT+CSCB?
Exe command         :       AT+CSCB
Called By           :   AT_RunCommand()
Data Accessed       :
Data Updated        :
Input           :       pParam
Output          :
Return          :   void
Others          :
*******************************************************************************************/
extern UINT32 CFW_CbSetContext(VOID *CtxData, CFW_SIM_ID nSimID);
extern UINT32 CFW_CbGetContext(VOID *CtxData, CFW_SIM_ID nSimID);
extern UINT32 CFW_CbAct(UINT16 nUTI, CFW_SIM_ID nSimID);
extern UINT32 CFW_CbDeAct(UINT16 nUTI, CFW_SIM_ID nSimID);
#ifdef CB_SUPPORT
VOID AT_SMS_CmdFunc_CSCB(AT_CMD_PARA *pParam)
{
    UINT8 nParamCount = 0x00;
    UINT16 mode = 0, nParamLen = 0, i = 0, j = 0, k = 0, m = 0;
    UINT8 mids[40] = {
        0,
    };
    UINT8 dcss[34] = {
        0,
    };
    UINT16 val_mids[20] = {
        0,
    };
    UINT8 val_dcss[17] = {
        0,
    };
    UINT8 CscbAckBuff[80] = {
        0,
    };
    UINT8 midnb = 0, dscnb = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT32 nGetParaRet = 0;
    CB_CTX_TYPE nCbCtxSet, nCbCtxRead;

    AT_TC(g_sw_AT_SMS, "+CSCB\n ");

    memset(&nCbCtxRead, 0x00, sizeof(CB_CTX_TYPE));
    memset(&nCbCtxSet, 0x00, sizeof(CB_CTX_TYPE));
    AT_ASSERT(NULL != pParam);
    switch (pParam->iType)
    {
    case AT_CMD_SET:
        nGetParaRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);
        if ((nParamCount > 3) || (nGetParaRet != ERR_SUCCESS))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "ParamCount Error", pParam->nDLCI, nSim);
            return;
        }
        nParamLen = 1;
        nGetParaRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, (void *)&mode, &nParamLen);
        if (nGetParaRet != ERR_SUCCESS)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "Get Param1 Error", pParam->nDLCI, nSim);
            return;
        }
        if (nParamCount > 1)
        {
            nParamLen = 20;
            nGetParaRet = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, mids, &nParamLen);
            if (nGetParaRet != ERR_SUCCESS)
            {
                AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "Get Param2 Error", pParam->nDLCI, nSim);
                return;
            }

            for (i = 0; i < sizeof(mids); i++)
            {
                if (mids[i] == ',')
                {
                    midnb++;
                }
                else
                {
                    if (mids[i] >= '0')
                    {
                        k = i;
                        m = 0;
                        while (1)
                        {
                            k++;
                            if (mids[k] == ',')
                                break;
                            if (mids[k] == 0)
                                break;

                            m++;
                        }
                        val_mids[j] = (UINT16)SUL_StrAToI(&mids[i]);
                        i = i + m;
                        j++;
                    }
                    if (j >= 20)
                        break;
                }
            }
            midnb++;
        }
        if (nParamCount > 2)
        {
            nParamLen = 20;
            nGetParaRet = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, dcss, &nParamLen);
            if (nGetParaRet != ERR_SUCCESS)
            {
                AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "Get Param3 Error", pParam->nDLCI, nSim);
                return;
            }
            j = 0;
            for (i = 0; i < sizeof(dcss); i++)
            {
                if (dcss[i] == ',')
                {
                    dscnb++;
                }
                else
                {
                    if (dcss[i] >= '0')
                    {
                        k = i;
                        m = 0;
                        while (1)
                        {
                            k++;
                            if (dcss[k] == ',')
                                break;
                            if (dcss[k] == 0)
                                break;

                            m++;
                        }
                        val_dcss[j] = (UINT8)SUL_StrAToI(&dcss[i]);
                        i = i + m;
                        j++;
                    }
                    if (j >= 17)
                        break;
                }
            }
            dscnb++;
        }

        AT_TC(g_sw_AT_SMS, "AT_SMS_CmdFunc_CSCB midnb=%d,dscnb=%d", midnb, dscnb);
        AT_MemCpy(&nCbCtxSet.Mid, val_mids, sizeof(val_mids));
        AT_MemCpy(&nCbCtxSet.Dcs, val_dcss, sizeof(val_dcss));
        nCbCtxSet.MidNb = midnb;
        nCbCtxSet.DcsNb = dscnb;
        for (i = 0; i < 10; i++)
        {
            AT_TC(g_sw_AT_SMS, "AT_SMS_CmdFunc_CSCB val_mids=%d,val_dcss=%d", val_mids[i], val_dcss[i]);
        }
        AT_TC(g_sw_AT_SMS, "AT_SMS_CmdFunc_CSCB nCbCtxSet midnb=%d,dscnb=%d", nCbCtxSet.MidNb, nCbCtxSet.DcsNb);
        CFW_CbSetContext(&nCbCtxSet, nSim);
        if (mode == CB_ACTIVE)
        {
            CFW_CbAct(pParam->nDLCI, nSim);
        }
        else
        {
            CFW_CbDeAct(pParam->nDLCI, nSim);
        }
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;
    case AT_CMD_READ:
        CFW_CbGetContext(&nCbCtxRead, nSim);

        if (nCbCtxRead.CbActiveStatus == CB_DEACTIVE)
            mode = CB_ACTIVE;
        else
            mode = CB_DEACTIVE;
        UINT8 nTmpMids[5];
        memset(mids, 0x00, 40);
        for (i = 0; i < nCbCtxRead.MidNb; i++)
        {
            memset(nTmpMids, 0x00, 5);
            SUL_Itoa(nCbCtxRead.Mid[i], nTmpMids, 10);
            SUL_StrCat(mids, nTmpMids);
            if (i < nCbCtxRead.MidNb - 1)
            {
                SUL_StrCat(mids, ",");
            }
        }
        UINT8 nTmpDcss[5];
        memset(dcss, 0x00, 34);
        for (i = 0; i < nCbCtxRead.DcsNb; i++)
        {
            memset(nTmpDcss, 0x00, 5);
            SUL_Itoa(nCbCtxRead.Dcs[i], nTmpDcss, 10);
            SUL_StrCat(dcss, nTmpDcss);
            if (i < nCbCtxRead.DcsNb - 1)
            {
                SUL_StrCat(dcss, ",");
            }
        }
        //        AT_Sprintf(CscbAckBuff, "+CSCB:%d,\"%s\",\"%s\"",mode,nCbCtxRead.Mid, nCbCtxRead.Dcs);
        AT_Sprintf(CscbAckBuff, "+CSCB:%d,\"%s\",\"%s\"", mode, mids, dcss);
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, CscbAckBuff, sizeof(CscbAckBuff), pParam->nDLCI, nSim);
        break;
    case AT_CMD_TEST:
        AT_Sprintf(CscbAckBuff, "+CSCB:(0,1),(0,1,5,320-478,922),(0-3,5)");
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, CscbAckBuff, AT_StrLen(CscbAckBuff), pParam->nDLCI, nSim);
        break;
    default:
        AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_SUPP, NULL, 0, NULL, pParam->nDLCI, nSim);

        break;
    }

    return;
}
#endif

/******************************************************************************************
Function            :   AT_SMS_CMGS_SendMessage_rsp
Description     :       process the response of all send message about at command
Called By           :   AT_SMS_AsyncEventProcess
Data Accessed       :
Data Updated        :
Input           :       COS_EVENT *pEvent
Output          :
Return          :   VOID
Others          :   modify and add comment by wangqunyang 06/04/2008
*******************************************************************************************/
VOID AT_SMS_CMGS_CMSS_SendMessage_rsp(COS_EVENT *pEvent)
{

    CFW_EVENT CfwEvent = {0};
    UINT8 PromptInfoBuff[32] = {0};
    UINT8 CMDHeaderInfo[32] = {0};
    UINT32 nOperationRet = ERR_SUCCESS;

    UINT8 nSim;
    /* Get event from csw for send message response */
    AT_CosEvent2CfwEvent(pEvent, &CfwEvent);
    nSim = CfwEvent.nFlag;
    /* just for debug */
    AT_TC(g_sw_AT_SMS, "cmss/cmgs send msg resp: CfwEvent.nType = 0x%x, CfwEvent.nParam1 =  0x%x, CfwEvent.nParam2 =  u%",
          CfwEvent.nType, CfwEvent.nParam1, CfwEvent.nParam2);

    /* Get  prompt of AT command */
    if ((AT_IsAsynCmdAvailable("+CMGS", g_SMS_CurCmdStamp, CfwEvent.nUTI)) /*||gATSATSendSMSFlag[CfwEvent.nFlag] == TRUE*/)
    {
        AT_TC(g_sw_AT_SMS, "AT_SMS_CMGS_CMSS_SendMessage_rsp: +CMGS\n ");
        AT_MemCpy(CMDHeaderInfo, "+CMGS", AT_StrLen("+CMGS"));
    }
    else if (AT_IsAsynCmdAvailable("+CMSS", g_SMS_CurCmdStamp, CfwEvent.nUTI))
    {

        if (CFW_SMS_STORED_STATUS_UNSENT == g_CMSS_Status)
        {
            UINT16 nUTI = 0;

            /* get the UTI and free it after finished calling */
            if (0 == (nUTI = AT_AllocUserTransID()))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "send msg resp:  malloc UTI error", CfwEvent.nUTI, nSim);
                return;
            }

            if (ERR_SUCCESS !=
                CFW_SmsSetUnSent2Sent(gATCurrentAtSMSInfo[nSim].nStorage2, 0x01, g_CMSS_Index,
                                      CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ, CfwEvent.nUTI, nSim))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "send msg resp: change status error 11", CfwEvent.nUTI,
                                  nSim);
                AT_FreeUserTransID(nUTI);
                return;
            }
            AT_FreeUserTransID(nUTI);
        }

        AT_MemCpy(CMDHeaderInfo, "+CMSS", AT_StrLen("+CMSS"));
    }

    /* check event type */
    if (0 == CfwEvent.nType)
    {
        AT_Sprintf(PromptInfoBuff, "%s: %u", CMDHeaderInfo, (unsigned int)CfwEvent.nParam1);
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptInfoBuff, AT_StrLen(PromptInfoBuff), CfwEvent.nUTI, nSim);
    }
    else if (0xF0 == CfwEvent.nType)
    {
        nOperationRet = AT_SMS_ERR_Change(CfwEvent.nParam1);
        AT_SMS_Result_Err(nOperationRet, NULL, 0, "cmss/cmgs send msg resp: CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
    }
    else
    {
        AT_SMS_Result_Err(ERR_AT_CMS_UNKNOWN_ERROR, NULL, 0, "cmss/cmgs send msg resp: exception happened", CfwEvent.nUTI,
                          nSim);
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_ListMessage_rsp
Description     :       AT list message response
Called By           :   AT_SMS_AsyncEventProcess
Data Accessed       :
Data Updated        :
Input           :       COS_EVENT *pEvent
Output          :
Return          :   UINT32
Others          :       //[+]for support CMER command :+ciev message 0/1 and cind command 2007.11.06
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
VOID AT_SMS_ListMessage_rsp(COS_EVENT *pEvent)
{
    CFW_EVENT CfwEvent = {0};

    CFW_SMS_NODE *pPendSmsMsgNode = NULL;

    UINT8 ATAckBuff[480] = {0};
    UINT8 nNodeIndex = 0;
    UINT8 nFormat = 0;
    UINT8 nstatus_tmp[12] = {0};
    UINT8 nDialNumBuff[TEL_NUMBER_MAX_LEN] = {0};

    UINT32 nGBKDATASize = 0;
    UINT16 nATBAckLen = 0;
    UINT8 *pUCS2Data = NULL;

    UINT8 pUCSLittleEndData[142] = {0};
    UINT32 nOperationRet = ERR_SUCCESS;

    UINT8 nSim;
    // Get event from csw and valicate the parameters
    AT_CosEvent2CfwEvent(pEvent, &CfwEvent);
    nSim = CfwEvent.nFlag;
    /* just for debug */
    AT_TC(g_sw_AT_SMS,
          "list msg resp: CfwEvent.nType = 0x%x, CfwEvent.nParam1 = 0x%x, CfwEvent.nParam2 >> 16 (total list counter) = %d",
          CfwEvent.nType, CfwEvent.nParam1, CfwEvent.nParam2 >> 16);

    /* get SMS format and check it */
    nFormat = gATCurrentAtSmsSettingSg_SMSFormat;
    if (nFormat > 1)
    {
        AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "list msg resp: nFormat > 1 ", CfwEvent.nUTI, nSim);
        return;
    }

    /* check event type */
    if (0 != CfwEvent.nType)
    {
        if (0xF0 == CfwEvent.nType)
        {
            nOperationRet = AT_SMS_ERR_Change(CfwEvent.nParam1);
            AT_SMS_Result_Err(nOperationRet, NULL, 0, "list msg resp: CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        }
        else
        {
            AT_SMS_Result_Err(ERR_AT_CMS_UNKNOWN_ERROR, NULL, 0, "list msg resp:exception happened", CfwEvent.nUTI, nSim);
        }

        return;
    }

    //===========================================
    // modify by wangqunyang 2008.04.09
    //===========================================
    for (nNodeIndex = 0; nNodeIndex < (CfwEvent.nParam2 >> 16); nNodeIndex++)
    {
        /* Every cycle we must clear buffer */
        AT_MemSet(nstatus_tmp, 0, 12);
        AT_MemSet(nDialNumBuff, 0, TEL_NUMBER_MAX_LEN);
        AT_MemSet(ATAckBuff, 0, sizeof(ATAckBuff));

        /* Get each list message node and process */

        nOperationRet = CFW_SmsGetMessageNode((CFW_SMS_NODE *)(CfwEvent.nParam1), nNodeIndex, &pPendSmsMsgNode, nSim);
        if (nOperationRet != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SMS, "list sms: some one message node exception");
            continue; /* Find next node */
        }

        nstatus_tmp[0] = pPendSmsMsgNode->nStatus & 0x0f;

        if (CFW_SMS_STORED_STATUS_UNREAD == nstatus_tmp[0])
        {
            UINT16 nUTI = 0;

            /* decrease the message couter */
            if (g_SMS_CIND_NewMsg > 0)
            {
                g_SMS_CIND_NewMsg--;
            }

            /* get the UTI and free it after finished calling */
            if (0 == (nUTI = AT_AllocUserTransID()))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "list msg resp:  malloc UTI error", /* CfwEvent.nUTI */ nUTI,
                                  nSim);
                AT_FREE(pPendSmsMsgNode);
                return;
            }

            if (ERR_SUCCESS !=
                CFW_SmsSetUnRead2Read(pPendSmsMsgNode->nStorage, 0x01, pPendSmsMsgNode->nConcatCurrentIndex,
                                      /* CfwEvent.nUTI */ nUTI, nSim))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PARA, NULL, 0, "list msg resp: change status unread error", CfwEvent.nUTI,
                                  nSim);
                AT_FreeUserTransID(nUTI);
                AT_FREE(pPendSmsMsgNode);
                return;
            }

            AT_FreeUserTransID(nUTI);
        }

        if (!AT_SMS_StatusMacroFlagToStringOrData(nstatus_tmp, nFormat))
        {
            nOperationRet = ERR_AT_CMS_INVALID_STATUS;
            AT_TC(g_sw_AT_SMS, "list sms: status exception 11");
            AT_FREE(pPendSmsMsgNode);
            continue; /* Find next node */
        }

        // printf the sms message node type
        AT_TC(g_sw_AT_SMS, "list msg: pPendSmsMsgNode->nType = %u", pPendSmsMsgNode->nType);

        // modif csdh so ptype0,ptype2 not use 2007.09.27
        // delete ptype0,ptype2 2007.10.10
        // text mode,show param, state = unread/read msg
        if (pPendSmsMsgNode->nType == 1)
        {
            UINT8 type1[2] = "+"; // [mod]bug 7228 2007.12.12
            UINT8 count = 0;

            CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *pType1Data = NULL;
            pType1Data = (CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)(pPendSmsMsgNode->pNode);

            /* just for debug */
            AT_TC(g_sw_AT_SMS, "pType1Data->length = %u", pType1Data->length);
            AT_TC_MEMBLOCK(g_sw_AT_SMS, pType1Data->data, pType1Data->length, 16);

            if (0xD0 != pType1Data->tooa)
            {
                // change to ascii and get size
                count = SUL_GsmBcdToAscii(pType1Data->oa, (pType1Data->oa_size), nDialNumBuff);
            }
            else
            {
                count = SUL_Decode7Bit(pType1Data->oa, nDialNumBuff, (pType1Data->oa_size));
            }
            if (0xf0 == (*(pType1Data->oa + pType1Data->oa_size - 1) & 0xf0))
            {
                pType1Data->oa_size = (UINT8)(pType1Data->oa_size * 2 - 1);
            }
            else
            {
                pType1Data->oa_size = (UINT8)(pType1Data->oa_size * 2);
            }

            // decide has + or not
            if (pType1Data->tooa != CFW_TELNUMBER_TYPE_INTERNATIONAL)
            {
                type1[0] = '\0';
            }

            if (gATCurrentAtSmsSettingSCSDH_show == 1) /* CSDH=1 */
            {
                UINT8 *str1 = "+CMGL: %u,\"%s\",\"%s%s\",,\"%u/%02u/%02u,%02u:%02u:%02u+%02d\",%u,%u\r\n";
                UINT8 *str2 = "+CMGL: %u,\"%s\",\"%s%s\",,\"%u/%02u/%02u,%02u:%02u:%02u%03d\",%u,%u\r\n";
                UINT8 *str = NULL;

                if (pType1Data->scts.iZone < 0)
                {
                    str = str2;
                }
                else
                {
                    str = str1;
                }

                // output param first
                AT_Sprintf(ATAckBuff, str,
                           pPendSmsMsgNode->nConcatCurrentIndex,
                           nstatus_tmp,
                           type1,
                           nDialNumBuff,
                           pType1Data->scts.uYear,
                           pType1Data->scts.uMonth,
                           pType1Data->scts.uDay,
                           pType1Data->scts.uHour,
                           pType1Data->scts.uMinute,
                           pType1Data->scts.uSecond, pType1Data->scts.iZone, pType1Data->tooa, pType1Data->length);
            }
            else /* csdh=0 */
            {
                // [+]for csdh use global 2007.09.27
                // [mod]2007.10.17 show scts
                UINT8 *str1 = "+CMGL: %u,\"%s\",\"%s%s\",,\"%u/%02u/%02u,%02u:%02u:%02u+%02d\"\r\n";
                UINT8 *str2 = "+CMGL: %u,\"%s\",\"%s%s\",,\"%u/%02u/%02u,%02u:%02u:%02u%03d\"\r\n";
                UINT8 *str = NULL;

                if (pType1Data->scts.iZone < 0)
                {
                    str = str2;
                }
                else
                {
                    str = str1;
                }

                // output param first
                AT_Sprintf(ATAckBuff, str,
                           pPendSmsMsgNode->nConcatCurrentIndex,
                           nstatus_tmp,
                           type1,
                           nDialNumBuff,
                           pType1Data->scts.uYear,
                           pType1Data->scts.uMonth,
                           pType1Data->scts.uDay,
                           pType1Data->scts.uHour,
                           pType1Data->scts.uMinute,
                           pType1Data->scts.uSecond, pType1Data->scts.iZone, pType1Data->tooa, pType1Data->length);
            }

            /* get prompt buffer string length */
            nATBAckLen = AT_StrLen(ATAckBuff);

            /* Chinese message list */
            if (pType1Data->dcs == 2)
            {

                /* From low layer data is big ending and transfer to little ending */
                AT_Set_MultiLanguage();

                if (!(AT_UnicodeBigEnd2Unicode((UINT8 *)(pType1Data->data), pUCSLittleEndData, pType1Data->length)))
                {
                    nOperationRet = ERR_AT_CMS_ME_FAIL;
                    AT_FREE(pPendSmsMsgNode);
                    AT_TC(g_sw_AT_SMS, "list sms: BigEnd to LittleEnd exception 11");
                    continue; /* Find next node */
                }

                if (ERR_SUCCESS !=
                    ML_Unicode2LocalLanguage((UINT8 *)pUCSLittleEndData, pType1Data->length, &pUCS2Data, &nGBKDATASize, NULL))
                {
                    nOperationRet = ERR_AT_CMS_ME_FAIL;
                    AT_TC(g_sw_AT_SMS, "list sms: unicode to local language exception");
                    AT_FREE(pPendSmsMsgNode);
                    continue; /* Find next node */
                }

                /* buffer overflow */
                if ((pType1Data->length + nATBAckLen) > sizeof(ATAckBuff))
                {
                    nOperationRet = ERR_AT_CMS_INVALID_LEN;
                    AT_TC(g_sw_AT_SMS, "list sms: buffer overflow of pType1Data dcs == 2");
                    AT_FREE(pPendSmsMsgNode);
                    continue; /* Find next node */
                }

                AT_MemCpy(&ATAckBuff[nATBAckLen], pUCS2Data, pType1Data->length);
                nATBAckLen += pType1Data->length;

                // Free inout para resource
                AT_FREE(pUCS2Data);
            }
            else
            {

                /* buffer overflow */
                if ((pType1Data->length + nATBAckLen) > sizeof(ATAckBuff))
                {
                    nOperationRet = ERR_AT_CMS_INVALID_LEN;
                    AT_TC(g_sw_AT_SMS, "list sms: buffer overflow pType1Data dcs != 2");
                    AT_FREE(pPendSmsMsgNode);
                    continue; /* Find next node */
                }

                AT_MemCpy(&ATAckBuff[nATBAckLen], (UINT8 *)(pType1Data->data), pType1Data->length);
                nATBAckLen += pType1Data->length;
            }

            // [mod]2007.12.21 for format(based on common.c change)
            AT_SMS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, ATAckBuff, nATBAckLen, CfwEvent.nUTI, nSim);
        }
        else if (pPendSmsMsgNode->nType == 3) // text mode, show param, state =sent/unsent msg
        {
            CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO *pType3Data = NULL;

            UINT8 type1[2] = "+";
            UINT8 count = 0;

            pType3Data = (CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO *)(pPendSmsMsgNode->pNode);

            /* just for debug */
            AT_TC(g_sw_AT_SMS, "pType3Data->length = %u", pType3Data->length);
            AT_TC_MEMBLOCK(g_sw_AT_SMS, pType3Data->data, pType3Data->length, 16);

            // change to ascii and get size
            count = SUL_GsmBcdToAscii(pType3Data->da, (pType3Data->da_size), nDialNumBuff);

            if (0xf0 == (*(pType3Data->da + pType3Data->da_size - 1) & 0xf0))
            {
                pType3Data->da_size = (UINT8)(pType3Data->da_size * 2 - 1);
            }
            else
            {
                pType3Data->da_size = (UINT8)(pType3Data->da_size * 2);
            }

            // decide has + or not
            if (pType3Data->toda != CFW_TELNUMBER_TYPE_INTERNATIONAL)
            {
                type1[0] = '\0';
            }

            if (gATCurrentAtSmsSettingSCSDH_show == 1) // CSDH=1
            {

                // output param first
                AT_Sprintf(ATAckBuff, "+CMGL: %u,\"%s\",\"%s%s\",,%u,%u\r\n",
                           pPendSmsMsgNode->nConcatCurrentIndex,
                           nstatus_tmp, type1, nDialNumBuff, pType3Data->toda, pType3Data->length);
            }
            else // csdh=0
            {
                // [+]for csdh use global 2007.09.27
                // output param first
                AT_Sprintf(ATAckBuff, "+CMGL: %u,\"%s\",\"%s%s\"\r\n", // [mod]2007.10.17 remove ",,,"
                           pPendSmsMsgNode->nConcatCurrentIndex, nstatus_tmp, type1, nDialNumBuff);
            }

            /* get prompt buffer string length */
            nATBAckLen = AT_StrLen(ATAckBuff);

            if (pType3Data->dcs == 2) // [mod]2007.09.26 csw param ==2 for chinese sms
            {

                /* From low layer data is big ending and transfer to little ending */
                AT_Set_MultiLanguage();

                if (!(AT_UnicodeBigEnd2Unicode((UINT8 *)(pType3Data->data), pUCSLittleEndData, pType3Data->length)))
                {
                    nOperationRet = ERR_AT_CMS_ME_FAIL;
                    AT_FREE(pPendSmsMsgNode);
                    AT_TC(g_sw_AT_SMS, "list sms: BigEnd to LittleEnd exception 22");
                    continue; /* Find next node */
                }

                if (ERR_SUCCESS !=
                    ML_Unicode2LocalLanguage((UINT8 *)pUCSLittleEndData, pType3Data->length, &pUCS2Data, &nGBKDATASize, NULL))
                {
                    nOperationRet = ERR_AT_CMS_ME_FAIL;
                    AT_TC(g_sw_AT_SMS, "list sms: unicode to local language exception 22");
                    AT_FREE(pPendSmsMsgNode);
                    continue; /* Find next node */
                }

                /* buffer overflow */
                if ((pType3Data->length + nATBAckLen) > sizeof(ATAckBuff))
                {
                    nOperationRet = ERR_AT_CMS_INVALID_LEN;
                    AT_TC(g_sw_AT_SMS, "list sms: buffer overflow pType3Data dcs == 2");
                    AT_FREE(pPendSmsMsgNode);
                    continue; /* Find next node */
                }

                AT_MemCpy(&ATAckBuff[nATBAckLen], pUCS2Data, pType3Data->length);
                nATBAckLen += pType3Data->length;

                // Free inout para resource
                AT_FREE(pUCS2Data);
            }
            else
            {
                /* buffer overflow */
                if ((pType3Data->length + nATBAckLen) > sizeof(ATAckBuff))
                {
                    nOperationRet = ERR_AT_CMS_INVALID_LEN;
                    AT_TC(g_sw_AT_SMS, "list sms: buffer overflow pType3Data dcs != 2");
                    AT_FREE(pPendSmsMsgNode);
                    continue; /* Find next node */
                }

                AT_MemCpy(&ATAckBuff[nATBAckLen], (UINT8 *)(pType3Data->data), pType3Data->length);
                nATBAckLen += pType3Data->length;
            }
            AT_SMS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, ATAckBuff, nATBAckLen, nSim, CfwEvent.nUTI);
        }
        else if (pPendSmsMsgNode->nType == 7) // PDU mode, pp msg
        {
            CFW_SMS_PDU_INFO *pType7Data = NULL;
            UINT8 BCDDataBuff[200] = {0};
            UINT8 ASCIIDataBuff[400] = {0};
            UINT16 nTotalDataLen = 0;

            pType7Data = (CFW_SMS_PDU_INFO *)(pPendSmsMsgNode->pNode);

            /* just for debug */
            AT_TC(g_sw_AT_SMS, "Original Data: pType7Data->nDataSize = %u", pType7Data->nDataSize);

            /* check the PDU and discard unused char */
            if (!AT_SMS_CheckPDUIsValid(pType7Data->pData, (UINT8 *)&(pType7Data->nDataSize), TRUE))
            {
                nOperationRet = ERR_AT_CMS_INVALID_PDU_CHAR;
                AT_TC(g_sw_AT_SMS, "list sms: check pdu is invalid ");
                AT_FREE(pPendSmsMsgNode);
                continue; /* find next node */
            }

            /* just for debug */
            AT_TC(g_sw_AT_SMS, "Worked Data: pType7Data->nDataSize = %u", pType7Data->nDataSize);
            AT_TC_MEMBLOCK(g_sw_AT_SMS, pType7Data->pData, pType7Data->nDataSize, 16);

            /* get status from the MACRO STATUS */
            if (!(AT_SMS_StatusMacroFlagToStringOrData(&(pType7Data->nStatus), 0)))
            {
                nOperationRet = ERR_AT_CMS_INVALID_STATUS;
                AT_TC(g_sw_AT_SMS, "list sms: status exception 22");
                AT_FREE(pPendSmsMsgNode);
                continue; /* find next node */
            }

            /* output param first */
            AT_Sprintf(ATAckBuff, "+CMGL: %u,%u,,%u\r\n", pPendSmsMsgNode->nConcatCurrentIndex, pType7Data->nStatus, pType7Data->nDataSize - (pType7Data->pData[0] + 1) // old  pType7Data->nDataSize - pType7Data->pData[0] - 1
                       );

            /* get prompt buffer string length */
            nATBAckLen = AT_StrLen(ATAckBuff);

            /* add sca if have not sca */
            nTotalDataLen = pType7Data->nDataSize;
            if (!(SMS_AddSCA2PDU(pType7Data->pData, BCDDataBuff, &(nTotalDataLen), nSim)))
            {
                nOperationRet = ERR_AT_CMS_INVALID_PDU_CHAR;
                AT_TC(g_sw_AT_SMS, "list sms: AddSCA2PDU exception ");
                AT_FREE(pPendSmsMsgNode);
                continue; /* find next node */
            }

            /* BCD to ASCII */
            if (!(SMS_PDUBCDToASCII(BCDDataBuff, &(nTotalDataLen), ASCIIDataBuff)))
            {
                nOperationRet = ERR_AT_CMS_INVALID_PDU_CHAR;
                AT_TC(g_sw_AT_SMS, "list sms: PDUBCDToASCII exception ");
                AT_FREE(pPendSmsMsgNode);
                continue; /* find next node */
            }

            /* buffer overflow */
            if ((nATBAckLen + nTotalDataLen) > sizeof(ATAckBuff))
            {
                nOperationRet = ERR_AT_CMS_INVALID_LEN;
                AT_TC(g_sw_AT_SMS, "list sms: pType7Data buffer overflow ");
                AT_FREE(pPendSmsMsgNode);
                continue; /* find next node */
            }

            AT_MemCpy(&ATAckBuff[nATBAckLen], ASCIIDataBuff, nTotalDataLen);
            nATBAckLen += nTotalDataLen;
            AT_SMS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, ATAckBuff, nATBAckLen, CfwEvent.nUTI, nSim);
        }
        else if (pPendSmsMsgNode->nType == 8) // PDU mode, CB msg
        {
            nOperationRet = ERR_AT_CMS_RFQ_FAC_NOT_IMP;
        }
        else
        {
            nOperationRet = ERR_AT_CMS_UNKNOWN_ERROR;
        }

        /* finished getting one node then free mem */
        AT_FREE(pPendSmsMsgNode);
    }

    //===========================================
    // check process result is successful or not by wangqunyang 2008.04.09
    //===========================================
    if (nOperationRet == ERR_SUCCESS)
    {
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, CfwEvent.nUTI, nSim);
    }
    else
    {
        AT_SMS_Result_Err(nOperationRet, NULL, 0, "List msg resp: exception happened", CfwEvent.nUTI, nSim);
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_WriteMessage_rsp
Description     :       AT write message response
Called By           :   AT_SMS_AsyncEventProcess
Data Accessed       :
Data Updated        :
Input           :       COS_EVENT *pEvent
Output          :
Return          :   UINT32
Others          :
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
VOID AT_SMS_WriteMessage_rsp(COS_EVENT *pEvent)
{
    CFW_EVENT CfwEvent = {0};
    CFW_SMS_STORAGE_INFO sStorageInfo = {0};

    UINT8 ATAckBuff[32] = {0};
    UINT32 nOperationRet = ERR_SUCCESS;

    UINT8 nSim;
    // Get event from csw
    AT_CosEvent2CfwEvent(pEvent, &CfwEvent);
    nSim = CfwEvent.nFlag;
    /* just for debug */
    AT_TC(g_sw_AT_SMS, "write msg resp: CfwEvent.nType = 0x%x, CfwEvent.nParam1 = 0x%x, CfwEvent.nParam2 = %u",
          CfwEvent.nType, CfwEvent.nParam1, CfwEvent.nParam2);

    /* check event type */
    if (0 == CfwEvent.nType)
    {
        // output information to user
        AT_Sprintf(ATAckBuff, "+CMGW: %u", (unsigned int)(pEvent->nParam1 & 0x0000ffff));
        nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, gATCurrentAtSMSInfo[nSim].nStorage2, nSim);
        if (ERR_SUCCESS == nOperationRet)
        {
            gATCurrentAtSMSInfo[nSim].nTotal2 = sStorageInfo.totalSlot;
            gATCurrentAtSMSInfo[nSim].nUsed2 = sStorageInfo.usedSlot;
        }
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, ATAckBuff, AT_StrLen(ATAckBuff), CfwEvent.nUTI, nSim);
    }
    else if (0xF0 == CfwEvent.nType)
    {
        nOperationRet = AT_SMS_ERR_Change(CfwEvent.nParam1);
        if (nOperationRet == ERR_AT_CMS_MEM_FULL)
        {
            AT_Sprintf(ATAckBuff, "+CIEV:  \"SMSFULL\"");
            AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, ATAckBuff, AT_StrLen(ATAckBuff), CfwEvent.nUTI, nSim);
        }
        AT_SMS_Result_Err(nOperationRet, NULL, 0, "write msg resp: CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
    }
    else
    {
        AT_SMS_Result_Err(ERR_AT_CMS_UNKNOWN_ERROR, NULL, 0, "write msg resp:exception happened", CfwEvent.nUTI, nSim);
    }

    return;
}
typedef struct _CFW_EV
{
    UINT32 nEventId;
    UINT32 nTransId; // only low 8bit is valid for all CFW applications.
    UINT32 nParam1;
    UINT32 nParam2;
    UINT8 nType;
    UINT8 nFlag;
    UINT8 padding[2];
} CFW_EV;

UINT32 CFW_MeGetStorageInfo(CFW_SIM_ID nSimId, UINT8 nStatus, CFW_EV *pEvent);

VOID AT_SMS_MOInit_MEMessage()
{
    UINT8 nIsFail = 0x0;
    CFW_SIM_ID nSimId;

    for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
    {
        CFW_EV sMeEvent = {
            0x00,
        };
        sMeEvent.nParam1 = (UINT32)((UINT8 *)AT_MALLOC(SIZEOF(CFW_SMS_STORAGE_INFO)));

        if ((UINT8 *)sMeEvent.nParam1 == NULL)
        {
            CSW_TRACE(CFW_SMS_TS_ID, "AT_SMS_MOInit_MEMessage  sMeEvent.nParam1  malloc error!!! \n ");
        }

        SUL_ZeroMemory8((UINT8 *)sMeEvent.nParam1, SIZEOF(CFW_SMS_STORAGE_INFO));

        CFW_SMS_STORAGE_INFO *pMEStorageInfo = (CFW_SMS_STORAGE_INFO *)(sMeEvent.nParam1);
        CFW_MeGetStorageInfo(nSimId, CFW_SMS_STORED_STATUS_STORED_ALL, (CFW_EV *)&sMeEvent);
        CFW_SMS_STORAGE_INFO sStorageInfo;
        SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));

        if (sMeEvent.nType == 0)
        {
            sStorageInfo.storageId = CFW_SMS_STORAGE_ME;
            sStorageInfo.totalSlot = pMEStorageInfo->totalSlot;
            sStorageInfo.usedSlot = pMEStorageInfo->usedSlot;

            //Get ME unReadRecords
            CFW_SMS_STORAGE_INFO *nTmpEventP1 = (CFW_SMS_STORAGE_INFO *)sMeEvent.nParam1;

            SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
            SUL_ZeroMemory8((UINT8 *)nTmpEventP1, SIZEOF(CFW_SMS_STORAGE_INFO));

            sMeEvent.nParam1 = (UINT32)nTmpEventP1;

            CFW_MeGetStorageInfo(nSimId, CFW_SMS_STORED_STATUS_UNREAD, &sMeEvent);
            if (sMeEvent.nType == 0)
            {
                sStorageInfo.unReadRecords = pMEStorageInfo->usedSlot;
                //Get ME readRecords
                nTmpEventP1 = (CFW_SMS_STORAGE_INFO *)sMeEvent.nParam1;
                SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
                SUL_ZeroMemory8((UINT8 *)nTmpEventP1, SIZEOF(CFW_SMS_STORAGE_INFO));

                sMeEvent.nParam1 = (UINT32)nTmpEventP1;
                CFW_MeGetStorageInfo(CFW_SMS_STORAGE_ME, CFW_SMS_STORED_STATUS_READ, &sMeEvent);
                if (sMeEvent.nType == 0)
                {
                    UINT8 nSentStatus = 0x0;

                    sStorageInfo.readRecords = pMEStorageInfo->usedSlot;
                    //Get ME sentRecords
                    nTmpEventP1 = (CFW_SMS_STORAGE_INFO *)sMeEvent.nParam1;

                    SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
                    SUL_ZeroMemory8((UINT8 *)nTmpEventP1, SIZEOF(CFW_SMS_STORAGE_INFO));

                    sMeEvent.nParam1 = (UINT32)nTmpEventP1;

                    nSentStatus = CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ | CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV |
                                  CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE | CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE;
                    CFW_MeGetStorageInfo(nSimId, nSentStatus, &sMeEvent);

                    if (sMeEvent.nType == 0)
                    {
                        sStorageInfo.sentRecords = pMEStorageInfo->usedSlot;

                        //Get ME unsentRecords
                        nTmpEventP1 = (CFW_SMS_STORAGE_INFO *)sMeEvent.nParam1;

                        SUL_ZeroMemory8(&sMeEvent, SIZEOF(CFW_EV));
                        SUL_ZeroMemory8((UINT8 *)nTmpEventP1, SIZEOF(CFW_SMS_STORAGE_INFO));

                        sMeEvent.nParam1 = (UINT32)nTmpEventP1;
                        CFW_MeGetStorageInfo(nSimId, CFW_SMS_STORED_STATUS_UNSENT, &sMeEvent);
                        CSW_TRACE(CFW_SMS_TS_ID, TSTXT("MeGetSI 0\n"));

                        if (sMeEvent.nType == 0)
                        {
                            sStorageInfo.unsentRecords = pMEStorageInfo->usedSlot;
                            CFW_CfgSetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_ME, nSimId);
                            CSW_TRACE(CFW_SMS_TS_ID, TSTXT("MeGetSI 1 \n"));

                            //Add by lixp at 20090213 for free crash issue
                            if (nSimId == (CFW_SIM_COUNT - 1))
                            {
                                CSW_SMS_FREE((UINT8 *)sMeEvent.nParam1);
                            }
                        } //unsentRecords
                        else
                        {
                            nIsFail = 1;
                        }
                    } //sentRecords
                    else
                    {
                        nIsFail = 2;
                    }
                } //readRecords
                else
                {
                    nIsFail = 3;
                }
            } //unReadRecords
            else
            {
                nIsFail = 4;
            }
        } //totalSlot and usedSlot
        else
        {
            nIsFail = 5;
        }

        if (nIsFail)
        {
            CSW_TRACE(CFW_SMS_TS_ID, TSTXT("AT_SMS_MOInit_MEMessage failed  nIsFail %d\n"), nIsFail);
            break;
        }
    }
}

VOID AT_SMS_MOInit_ReadMessage_rsp(COS_EVENT *pEvent)
{
    static UINT8 nIndex[NUMBER_OF_SIM] = {1, 1};

    UINT8 nFormat = 0;
    UINT8 nstatus_tmp[12] = {0};
    UINT8 urc_str[100] = {0};
    CFW_EVENT CfwEvent = {0};
    CFW_SMS_NODE *pNode = NULL;
    PAT_CMD_RESULT pResult = NULL;
    UINT16 nUTI = 0;
    AT_CosEvent2CfwEvent(pEvent, &CfwEvent);
    UINT8 nSim = CfwEvent.nFlag;
    ;

    /* just for debug */
    AT_TC(g_sw_AT_SMS, "AT_SMS_MOInit_ReadMessage_rsp: CfwEvent.nType = 0x%x, CfwEvent.nParam1 = 0x%x, CfwEvent.nParam2 = %u,nSim %d",
          CfwEvent.nType, CfwEvent.nParam1, CfwEvent.nParam2, nSim);

    CFW_SMS_STORAGE_INFO sStorageInfo;
    SUL_ZeroMemory8(&sStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));

    CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSim);
    AT_TC(g_sw_AT_SMS, "sStorageInfo.totalSlot 0x%x nIndex[nSim] %d nSim %d", sStorageInfo.totalSlot, nIndex[nSim], nSim);
    //hal_HstSendEvent(0xA1300000 + (nIndex[nSim]<<8) + nSim);
    if (0x00 != CfwEvent.nType)
    {
        if (0xF0 == CfwEvent.nType)
        {
            if (++nIndex[nSim] <= sStorageInfo.totalSlot)
            {

                AT_TC(g_sw_AT_SMS, "Begin Read nIndex[nSim] %d nSim %d", nIndex[nSim], nSim);

                UINT32 nRet = CFW_SmsReadMessage(CFW_SMS_STORAGE_SM, CFW_SMS_TYPE_PP, nIndex[nSim], CfwEvent.nUTI, nSim);

                if (ERR_SUCCESS != nRet)
                {
                    AT_TC(g_sw_AT_SMS, "CFW_SmsReadMessage Error 0x%x", nRet);
                    AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "SMS MO Init: ReadMessage error", nUTI, nSim);
                    at_ModuleSetInitResult(AT_MODULE_INIT_SIM_SMS_ERROR);
                    return;
                }
            }
            else
            {
                //Init Over
                AT_TC(g_sw_AT_SMS, "SMS INIT Success usedSlot %d total %d nSim %d", sStorageInfo.usedSlot, sStorageInfo.totalSlot, nSim);

                CFW_EVENT cfw_event;
                //UINT32 ats_stat = 0;

                at_ModuleSetInitResult(AT_MODULE_INIT_SIM_SMS_OK);

                /* get the UTI and free it after finished calling */
                if (0 == (nUTI = AT_AllocUserTransID()))
                {
                    //hal_HstSendEvent(0xa130001);

                    AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgr: malloc UTI error", cfw_event.nUTI, nSim);
                    return;
                }

                UINT32 ret = CFW_SmsInitComplete(nUTI, nSim);
                if (ERR_SUCCESS != ret)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "SMS Init Complete failed", cfw_event.nUTI, nSim);
                    return;
                }
                AT_SMS_INIT(nSim);

                SUL_ZeroMemory8(&cfw_event, SIZEOF(cfw_event));

                cfw_event.nType = CFW_INIT_STATUS_SMS;
                cfw_event.nParam1 = sStorageInfo.totalSlot;
                AT_MemCpy(&(cfwInitInfo.sms[nSim]), &cfw_event, sizeof(cfw_event));

                // Notify SA that the SMS (ATS) module is ready for use.
                at_ModuleSetSmsReady(TRUE);

                AT_Sprintf(urc_str, "^CINIT: SMS %u, %u, %u", cfw_event.nType, cfw_event.nParam1, cfw_event.nParam2);
                pResult = AT_CreateRC(CSW_IND_NOTIFICATION,
                                      CMD_RC_CR,
                                      CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, urc_str, (UINT16)AT_StrLen(urc_str), 0);
                AT_TC(g_sw_AT_SMS, "send an SMS URC to ATM, URC string=%s", urc_str);
                AT_Notify2ATM(pResult, cfw_event.nUTI);
                AT_TC(g_sw_AT_SMS, TSTXT("set gATCurrentuCmer_ind nSim[%d] to 2\n"), nSim);

                gATCurrentuCmer_ind[nSim] = 2;
                gATCurrentAtSMSInfo[nSim].sCnmi.nMt = 2;
                AT_SMS_MOInit_MEMessage();
            }
        }
        else
        {
            AT_SMS_Result_Err(ERR_AT_CMS_UNKNOWN_ERROR, NULL, 0, "MO Init read msg resp: exception happened", CfwEvent.nUTI,
                              nSim);
        }

        return;
    }
    pNode = (CFW_SMS_NODE *)CfwEvent.nParam1;
    nFormat = gATCurrentAtSmsSettingSg_SMSFormat;
    nstatus_tmp[0] = pNode->nStatus & 0x0f;

    if (nstatus_tmp[0] == CFW_SMS_STORED_STATUS_READ)
    {
        sStorageInfo.readRecords++;
    }
    else if (nstatus_tmp[0] == CFW_SMS_STORED_STATUS_UNREAD)
    {
        sStorageInfo.unReadRecords++;
    }
    else if (nstatus_tmp[0] == CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ ||
             nstatus_tmp[0] == CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV ||
             nstatus_tmp[0] == CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE ||
             nstatus_tmp[0] == CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE)
    {
        sStorageInfo.sentRecords++;
    }
    else if (nstatus_tmp[0] == CFW_SMS_STORED_STATUS_UNSENT)
    {
        sStorageInfo.unsentRecords++;
    }
    else
    {
        AT_TC(g_sw_AT_SMS, "Exception Status 0x%x", nstatus_tmp[0]);
        sStorageInfo.unknownRecords++;
        //return ;
    }

    sStorageInfo.usedSlot++;

    CFW_CfgSetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSim);

    if (++nIndex[nSim] <= sStorageInfo.totalSlot)
    {

        AT_TC(g_sw_AT_SMS, "Current Index %d,Total %d nSim %d", nIndex[nSim], sStorageInfo.totalSlot, nSim);
        UINT32 nRet = CFW_SmsReadMessage(CFW_SMS_STORAGE_SM, CFW_SMS_TYPE_PP, nIndex[nSim], CfwEvent.nUTI, nSim);

        if (ERR_SUCCESS != nRet)
        {
            AT_TC(g_sw_AT_SMS, "CFW_SmsReadMessage Error 0x%x", nRet);
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "SMS MO Init: ReadMessage error", nUTI, nSim);
            at_ModuleSetInitResult(AT_MODULE_INIT_SIM_SMS_ERROR);
            //hal_HstSendEvent(0xa130003);

            return;
        }
    }
    else
    {
        CFW_EVENT cfw_event;
        //Init Over
        AT_TC(g_sw_AT_SMS, "SMS INIT Success usedSlot %d total %d nSim %d", sStorageInfo.usedSlot, sStorageInfo.totalSlot, nSim);
        //UINT32 ats_stat = 0;

        at_ModuleSetInitResult(AT_MODULE_INIT_SIM_SMS_OK);

        /* get the UTI and free it after finished calling */
        if (0 == (nUTI = AT_AllocUserTransID()))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgr: malloc UTI error", cfw_event.nUTI, nSim);
            return;
        }

        CFW_SmsInitComplete(nUTI, nSim);
        AT_SMS_INIT(nSim); // hameina[+]

        SUL_ZeroMemory8(&cfw_event, SIZEOF(cfw_event));
        cfw_event.nType = CFW_INIT_STATUS_SMS;
        cfw_event.nParam1 = 0;

        AT_Sprintf(urc_str, "^CINIT: SMS %u, %u, %u", cfw_event.nType, cfw_event.nParam1, cfw_event.nParam2);
        pResult = AT_CreateRC(CSW_IND_NOTIFICATION,
                              CMD_RC_CR,
                              CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, urc_str, (UINT16)AT_StrLen(urc_str), 0);
        AT_TC(g_sw_AT_SMS, "send an SMS URC to ATM, URC string=%s", urc_str);

        AT_TC(g_sw_AT_SMS, "sStorageInfo.usedSlot 0x%x", sStorageInfo.usedSlot);

        AT_Notify2ATM(pResult, cfw_event.nUTI);
        AT_TC(g_sw_AT_SMS, TSTXT("Set gATCurrentuCmer_ind nSim[%d] to 2\n"), nSim);
        gATCurrentuCmer_ind[nSim] = 2;
        gATCurrentAtSMSInfo[nSim].sCnmi.nMt = 2;
        AT_MemCpy(&(cfwInitInfo.sms[nSim]), &cfw_event, sizeof(cfw_event));

        // Notify SA that the SMS (ATS) module is ready for use.
        at_ModuleSetSmsReady(true);
        AT_SMS_MOInit_MEMessage();
        //Init Over
    }

    return;
}
/******************************************************************************************
Function            :   AT_SMS_CMSS_ReadMessage_rsp
Description     :       AT CMSS COMMAND read message response
Called By           :
Data Accessed       :
Data Updated        :
Input           :       COS_EVENT *pEvent
Output          :
Return          :   VOID
Others          :
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
VOID AT_SMS_CMSS_ReadMessage_rsp(COS_EVENT *pEvent)
{

    CFW_DIALNUMBER Number = {0};
    CFW_EVENT CfwEvent = {0};
    UINT8 *SmsData = NULL;

    CFW_SMS_NODE *pNode = NULL;

    UINT8 nToDA = CFW_TELNUMBER_TYPE_UNKNOWN;
    UINT8 nFormat = 0;
    UINT8 nstatus_tmp[12] = {0};

    UINT32 nOperationRet = ERR_SUCCESS;
    UINT32 nSendMsgRet = ERR_SUCCESS;

    UINT16 SmsDatalen = 0;

    UINT16 nUTI = 0;

    UINT8 nSim;
    AT_CosEvent2CfwEvent(pEvent, &CfwEvent);
    nSim = CfwEvent.nFlag;
    /* just for debug */
    AT_TC(g_sw_AT_SMS, "cmss read msg resp: CfwEvent.nType = 0x%x, CfwEvent.nParam1 = 0x%x, CfwEvent.nParam2 = %u",
          CfwEvent.nType, CfwEvent.nParam1, CfwEvent.nParam2);

    /* check event type */
    if (0 != CfwEvent.nType)
    {
        if (0xF0 == CfwEvent.nType)
        {
            nOperationRet = AT_SMS_ERR_Change(CfwEvent.nParam1);
            AT_SMS_Result_Err(nOperationRet, NULL, 0, "cmss read msg resp: CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        }
        else
        {
            AT_SMS_Result_Err(ERR_AT_CMS_UNKNOWN_ERROR, NULL, 0, "cmss read msg resp: exception happened", CfwEvent.nUTI,
                              nSim);
        }
        return;
    }

    pNode = (CFW_SMS_NODE *)CfwEvent.nParam1;
    nFormat = gATCurrentAtSmsSettingSg_SMSFormat;
    nstatus_tmp[0] = pNode->nStatus & 0x0f;

    // save the cmss related status
    g_CMSS_Status = nstatus_tmp[0];

    if (!AT_SMS_StatusMacroFlagToStringOrData(nstatus_tmp, nFormat))
    {
        AT_SMS_Result_Err(ERR_AT_CMS_INVALID_STATUS, NULL, 0,
                          "cmss read msg resp: AT_SMS_StatusMacroFlagToStringOrData Error", CfwEvent.nUTI, nSim);
        return;
    }

    /* just for debug */
    AT_TC(g_sw_AT_SMS, "cmss read msg resp: pNode->nType = %u", pNode->nType);

    //===========================================/
    // modif csdh so ptype0,ptype2 not use 2007.09.27
    // delete ptype0,ptype2 2007.10.10
    //===========================================
    if (pNode->nType == 1) /* text mode,show param, state = unread/read msg */
    {
        CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *pType1Data = NULL;
        pType1Data = (CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)(pNode->pNode);

        /* just for debug */
        AT_TC(g_sw_AT_SMS, "pType1Data->length = %u", pType1Data->length);
        AT_TC_MEMBLOCK(g_sw_AT_SMS, pType1Data->data, pType1Data->length, 16);

        SmsDatalen = pType1Data->length;
        SmsData = (UINT8 *)AT_MALLOC(SmsDatalen);

        if (!SmsData)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_MEM_FAIL, NULL, 0, "cmss read msg resp: malloc failure 1", CfwEvent.nUTI, nSim);
            return;
        }

        AT_MemSet(SmsData, 0, SmsDatalen);
        AT_MemCpy(SmsData, pType1Data->data, SmsDatalen);

        if (g_CMSS_DA_Flag == 1)
        {
            Number.nDialNumberSize = g_CMSS_BCDLen;
            Number.pDialNumber = g_CMSS_BCD;
            Number.nType = g_CMSS_nToDA;
        }
        else
        {

            Number.nDialNumberSize = pType1Data->oa_size;
            Number.pDialNumber = pType1Data->oa;
            Number.nType = nToDA;
        }

        /* get the UTI and free it after finished calling */
        if (0 == (nUTI = AT_AllocUserTransID()))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmss read msg resp:  malloc UTI error", CfwEvent.nUTI, nSim);
            return;
        }

        /* Send message and free UTI */
        if (pgATFNDlist[nSim])
        {
            if (1 == pgATFNDlist[nSim]->nFDNSatus)
            {
                if (FALSE == AT_FDN_CompareNumber(Number.pDialNumber, Number.nDialNumberSize, Number.nType, nSim))
                {
                    AT_TC(g_sw_AT_CC, "CmdFunc_D Warning don't FDN record number!\n\r");
                    SUL_StrPrint(g_pCeer, "+CEER: %d", ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED);
                    AT_CC_Result_Err(ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED, CMD_ERROR_CODE_TYPE_CME, CfwEvent.nUTI, nSim);
                    return;
                }
            }
        }
        nSendMsgRet = CFW_SmsSendMessage(&Number, SmsData, SmsDatalen, CfwEvent.nUTI, nSim);
        AT_FreeUserTransID(nUTI);
        AT_FREE(SmsData);
    }
    else if (pNode->nType == 3) /* text mode, show param, state =sent/unsent msg */
    {
        CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO *pType3Data;
        pType3Data = (CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO *)(pNode->pNode);

        /* just for debug */
        AT_TC(g_sw_AT_SMS, "pType3Data->length = %u", pType3Data->length);
        AT_TC_MEMBLOCK(g_sw_AT_SMS, pType3Data->data, pType3Data->length, 16);

        SmsDatalen = pType3Data->length;
        SmsData = (UINT8 *)AT_MALLOC(SmsDatalen);

        if (!SmsData)
        {
            AT_SMS_Result_Err(ERR_AT_CMS_MEM_FAIL, NULL, 0, "cmss read msg resp: malloc failure 2", CfwEvent.nUTI, nSim);
            return;
        }

        AT_MemSet(SmsData, 0, SmsDatalen);
        AT_MemCpy(SmsData, pType3Data->data, SmsDatalen);

        if (g_CMSS_DA_Flag == 1)
        {
            Number.nDialNumberSize = g_CMSS_BCDLen;
            Number.pDialNumber = g_CMSS_BCD;
            Number.nType = g_CMSS_nToDA;
        }
        else
        {

            Number.nDialNumberSize = pType3Data->da_size;
            Number.pDialNumber = pType3Data->da;
            Number.nType = nToDA;
        }

        /* get the UTI and free it after finished calling */
        if (0 == (nUTI = AT_AllocUserTransID()))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmss read msg resp: malloc UTI error", nSim, CfwEvent.nUTI);
            return;
        }

        /* Send message and free UTI */
        if (pgATFNDlist[nSim])
        {
            if (1 == pgATFNDlist[nSim]->nFDNSatus)
            {
                if (FALSE == AT_FDN_CompareNumber(Number.pDialNumber, Number.nDialNumberSize, Number.nType, nSim))
                {
                    AT_TC(g_sw_AT_CC, "CmdFunc_D Warning don't FDN record number!\n\r");
                    SUL_StrPrint(g_pCeer, "+CEER: %d", ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED);
                    AT_CC_Result_Err(ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED, CMD_ERROR_CODE_TYPE_CME, CfwEvent.nUTI, nSim);
                    return;
                }
            }
        }
        nSendMsgRet = CFW_SmsSendMessage(&Number, SmsData, SmsDatalen, CfwEvent.nUTI, nSim);
        AT_FreeUserTransID(nUTI);
        AT_FREE(SmsData);
    }
    else if (pNode->nType == 7) /* PDU mode, pp msg */
    {
        CFW_SMS_PDU_INFO *pType7Data = NULL;
        UINT8 BCDDataBuff[200] = {0};

        pType7Data = (CFW_SMS_PDU_INFO *)(pNode->pNode);

        /* just for debug */
        AT_TC(g_sw_AT_SMS, "Original Data: pType7Data->nDataSize = %u", pType7Data->nDataSize);

        /* check recv pdu and discard unused char */
        if (!(AT_SMS_CheckPDUIsValid((UINT8 *)pType7Data->pData, (UINT8 *)&(pType7Data->nDataSize), TRUE)))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_CHAR, NULL, 0, "cmss read msg resp: check pdu is invalid", CfwEvent.nUTI,
                              nSim);
            return;
        }

        /* just for debug */
        AT_TC(g_sw_AT_SMS, "Worked Data: pType7Data->nDataSize = %u", pType7Data->nDataSize);
        AT_TC_MEMBLOCK(g_sw_AT_SMS, pType7Data->pData, pType7Data->nDataSize, 16);

        if (g_CMSS_DA_Flag == 1)
        {

            UINT8 nSCAlen = 0, nDAlen = 0, PDUCopyIndex = 0;
            UINT8 oldDABcdlen = 0;

            nSCAlen = pType7Data->pData[0];
            nDAlen = pType7Data->pData[nSCAlen + 1 + 2];
            oldDABcdlen = (nDAlen % 2 ? (nDAlen / 2) + 1 : (nDAlen / 2));
            SmsDatalen = pType7Data->nDataSize + g_CMSS_BCDLen - oldDABcdlen;

            SmsData = (UINT8 *)AT_MALLOC(SmsDatalen);
            AT_MemSet(SmsData, 0, SmsDatalen);
            if (!SmsData)
            {
                AT_SMS_Result_Err(ERR_AT_CMS_MEM_FAIL, NULL, 0, "cmss read msg resp: malloc failure 3 ", CfwEvent.nUTI, nSim);
                return;
            }

            SmsData[nSCAlen + 3] = g_CMSS_StringLen;

            if (g_CMSS_nToDA != 0)
            {
                SmsData[nSCAlen + 3 + 1] = g_CMSS_nToDA;
            }
            else
            {
                if (g_CMSS_BCD[0] == 0x68)
                {
                    SmsData[nSCAlen + 3 + 1] = 145;
                }
                else
                {
                    SmsData[nSCAlen + 3 + 1] = 129;
                }
            }

            AT_MemCpy(&SmsData[nSCAlen + 5], g_CMSS_BCD, g_CMSS_BCDLen);

            PDUCopyIndex = nSCAlen + 1 + 2 + oldDABcdlen + 2;

            AT_MemCpy(&(SmsData[PDUCopyIndex + g_CMSS_BCDLen - oldDABcdlen]), &(pType7Data->pData[PDUCopyIndex]),
                      pType7Data->nDataSize - PDUCopyIndex);
            AT_MemCpy(SmsData, pType7Data->pData, nSCAlen + 3);
        }

        else
        {

            SmsDatalen = pType7Data->nDataSize;
            SmsData = (UINT8 *)AT_MALLOC(SmsDatalen);

            AT_MemSet(SmsData, 0, SmsDatalen);
            if (!SmsData)
            {
                AT_SMS_Result_Err(ERR_AT_CMS_MEM_FAIL, NULL, 0, "cmss read msg resp: malloc failure 4", CfwEvent.nUTI, nSim);
                return;
            }

            AT_MemCpy(SmsData, pType7Data->pData, SmsDatalen);
        }

        /* add sca for read data if have not SCA */
        if (!(SMS_AddSCA2PDU(SmsData, BCDDataBuff, &(SmsDatalen), nSim)))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_CHAR, NULL, 0, "CDSPDUReport: format is not match", CfwEvent.nUTI, nSim);
            return;
        }

        /* get the UTI and free it after finished calling */
        if (0 == (nUTI = AT_AllocUserTransID()))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmss read msg resp:  malloc UTI error", nSim, CfwEvent.nUTI);
            return;
        }

        /* Send message and free UTI */
        if (pgATFNDlist[nSim])
        {
            AT_TC(g_sw_AT_SMS, "CMGS pgATFNDlist[nSim]->nFDNSatus: %d!\n\r", pgATFNDlist[nSim]->nFDNSatus);
            if (1 == pgATFNDlist[nSim]->nFDNSatus)
            {
                UINT8 nNumSize = 0;
                UINT8 nSCANumSize = 0;
                UINT8 nType = 0;
                UINT8 nFDNNumbers[20] = {
                    0xff,
                };
                UINT8 nSCANumbers[20] = {
                    0xff,
                };

                AT_MemSet(nFDNNumbers, 0xff, 20);
                if (FALSE == AT_SMS_GetPDUNUM(BCDDataBuff, nFDNNumbers, &nType, &nNumSize))
                {
                    AT_TC(g_sw_AT_SMS, "CMGS Warning don't FDN get PDU NUM error!\n\r");

                    SUL_StrPrint(g_pCeer, "+CEER: %d", ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED);
                    AT_CC_Result_Err(ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED, CMD_ERROR_CODE_TYPE_CME, CfwEvent.nUTI, nSim);
                    return;
                }

                AT_MemSet(nSCANumbers, 0xff, 20);
                if (FALSE == AT_SMS_GetSCANUM(BCDDataBuff, nSCANumbers, &nSCANumSize))
                {
                    AT_TC(g_sw_AT_SMS, "CMGS Warning don't FDN SCA number!\n\r");

                    SUL_StrPrint(g_pCeer, "+CEER: %d", ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED);
                    AT_CC_Result_Err(ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED, CMD_ERROR_CODE_TYPE_CME, CfwEvent.nUTI, nSim);
                    return;
                }
                if (0x91 == nType)
                {
                    nType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
                }

                if (FALSE == AT_FDN_CompareNumber(nFDNNumbers, nNumSize, nType, nSim))
                {
                    AT_TC(g_sw_AT_SMS, "CMGS Warning don't FDN record number!\n\r");

                    SUL_StrPrint(g_pCeer, "+CEER: %d", ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED);
                    AT_CC_Result_Err(ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED, CMD_ERROR_CODE_TYPE_CME, CfwEvent.nUTI, nSim);
                    return;
                }
            }
        }

        nSendMsgRet = CFW_SmsSendMessage(NULL, BCDDataBuff, SmsDatalen, CfwEvent.nUTI, nSim);
        AT_FreeUserTransID(nUTI);
        AT_FREE(SmsData);
    }
    else if (pNode->nType == 8) // PDU mode, CB msg
    {
        nSendMsgRet = ERR_AT_CMS_ERR_IN_MS;
    }
    else
    {
        nSendMsgRet = ERR_AT_CMS_ERR_IN_MS;
    }

    /* Judge the send message successful or not */
    if (ERR_SUCCESS != nSendMsgRet)
    {
        AT_SMS_Result_Err(nSendMsgRet, NULL, 0, "cmss read msg resp: send read msg check param failure", CfwEvent.nUTI,
                          nSim);
    }
    else
    {
        /* Different networks have different response time, according to networks status,
           ** we set general enough time(here is 60s ) to waiting for networks response */
        AT_SMS_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, NULL, 0, CfwEvent.nUTI, nSim);
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_ReadMessage_rsp
Description     :       AT SMS module read message response
Called By           :   At SMS module
Data Accessed       :
Data Updated        :
Input           :       COS_EVENT *pEvent
Output          :
Return          :   VOID
Others          :
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
VOID AT_SMS_CMGR_ReadMessage_rsp(COS_EVENT *pEvent)
{
    CFW_EVENT CfwEvent = {0};

    CFW_SMS_NODE *pNode = NULL;

    UINT8 ATAckBuff[480] = {0};
    UINT16 nATBAckLen = 0;
    UINT8 nFormat = 0;
    UINT8 nstatus_tmp[12] = {0};
    UINT8 nNumber[TEL_NUMBER_MAX_LEN] = {0};
    UINT8 nNumberSca[TEL_NUMBER_MAX_LEN] = {0};
    UINT8 pUCSLittleEndData[142] = {0};

    UINT8 *pUCS2Data = NULL;
    UINT32 nGBKDATASize = 0;
    UINT8 nDcs = 0;

    UINT32 count = 0;
    UINT32 nOperationRet = ERR_SUCCESS;

    UINT8 nSim;
    /* get event */
    AT_CosEvent2CfwEvent(pEvent, &CfwEvent);
    nSim = CfwEvent.nFlag;
    /* just for debug */
    AT_TC(g_sw_AT_SMS, "cmgr read msg resp: CfwEvent.nType = 0x%x, CfwEvent.nParam1 = 0x%x, CfwEvent.nParam2 = %u",
          CfwEvent.nType, CfwEvent.nParam1, CfwEvent.nParam2);

    /* check event type */
    if (0 != CfwEvent.nType)
    {
        if (0xF0 == CfwEvent.nType)
        {
            nOperationRet = AT_SMS_ERR_Change(CfwEvent.nParam1);
            AT_SMS_Result_Err(nOperationRet, NULL, 0, "cmgr read msg resp: CfwEvent.nType == 0xF0", nSim, CfwEvent.nUTI);
        }
        else
        {
            AT_SMS_Result_Err(ERR_AT_CMS_UNKNOWN_ERROR, NULL, 0, "cmgr read msg resp:  exception happened", CfwEvent.nUTI,
                              nSim);
        }

        return;
    }

    pNode = (CFW_SMS_NODE *)CfwEvent.nParam1;
    nFormat = gATCurrentAtSmsSettingSg_SMSFormat;
    nstatus_tmp[0] = pNode->nStatus & 0x0f;

    if (CFW_SMS_STORED_STATUS_UNREAD == nstatus_tmp[0])
    {
        /* decrease the message couter */
        if (g_SMS_CIND_NewMsg > 0)
        {
            g_SMS_CIND_NewMsg--;
        }
    }

    if (!AT_SMS_StatusMacroFlagToStringOrData(nstatus_tmp, nFormat))
    {
        AT_SMS_Result_Err(ERR_AT_CMS_INVALID_STATUS, NULL, 0, "cmgr read msg resp: status exception", CfwEvent.nUTI, nSim);
        return;
    }

    /* just for debug */
    AT_TC(g_sw_AT_SMS, "cmgr read msg resp: pNode->nType = %u", pNode->nType);

    //===========================================/////
    // text mode,show param, state = unread/read msg
    //===========================================
    if (pNode->nType == 1)
    {
        UINT8 type1[2] = "+";
        UINT8 type2[2] = "+";

        CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *pType1Data = NULL;

        pType1Data = (CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)(pNode->pNode);

        /* just for debug */
        AT_TC(g_sw_AT_SMS, "pType1Data->length = %u", pType1Data->length);
        AT_TC_MEMBLOCK(g_sw_AT_SMS, pType1Data->data, pType1Data->length, 16);

        // change to ascii and get size
        count = SUL_GsmBcdToAscii(pType1Data->oa, (pType1Data->oa_size), nNumber);

        if (0xf0 == (*(pType1Data->oa + pType1Data->oa_size - 1) & 0xf0))
        {
            pType1Data->oa_size = (UINT8)(pType1Data->oa_size * 2 - 1);
        }
        else
        {
            pType1Data->oa_size = (UINT8)(pType1Data->oa_size * 2);
        }
        if (0xD0 != pType1Data->tosca)
        {
            // sca  change to ascii and get size
            count = SUL_GsmBcdToAscii(pType1Data->sca, (pType1Data->sca_size), nNumberSca);
        }
        else
        {
            count = SUL_Decode7Bit(pType1Data->sca, nNumberSca, (pType1Data->sca_size));
        }

        if (0xf0 == (*(pType1Data->sca + pType1Data->sca_size - 1) & 0xf0))
        {
            pType1Data->sca_size = (UINT8)(pType1Data->sca_size * 2 - 1);
        }
        else
        {
            pType1Data->sca_size = (UINT8)(pType1Data->sca_size * 2);
        }

        // decide has + or not
        if (pType1Data->tooa != CFW_TELNUMBER_TYPE_INTERNATIONAL)
        {
            type1[0] = '\0'; // [mod]bug7227
        }

        if (pType1Data->tosca != CFW_TELNUMBER_TYPE_INTERNATIONAL)
        {
            type2[0] = '\0'; // [mod]bug7277
        }

        //===========================================
        // output param first
        // [+] for dcs change from csw to correct value(3GPP) 2007.10.10
        //===========================================
        nDcs = AT_SMS_DCSChange(pType1Data->dcs, nDcs);

        if (gATCurrentAtSmsSettingSCSDH_show == 1) /* CSDH=1 */
        {

            UINT8 *str1 = "+CMGR: \"%s\",\"%s%s\",,\"%u/%02u/%02u,%02u:%02u:%02u+%02d\",%u,%u,%u,%u,\"%s%s\",%u,%u\r\n";
            UINT8 *str2 = "+CMGR: \"%s\",\"%s%s\",,\"%u/%02u/%02u,%02u:%02u:%02u%03d\",%u,%u,%u,%u,\"%s%s\",%u,%u\r\n";
            UINT8 *str = NULL;

            if (pType1Data->scts.iZone < 0)
            {
                str = str2;
            }
            else
            {
                str = str1;
            }

            AT_Sprintf(ATAckBuff, str, nstatus_tmp, type1, nNumber, pType1Data->scts.uYear, pType1Data->scts.uMonth, pType1Data->scts.uDay, pType1Data->scts.uHour, pType1Data->scts.uMinute, pType1Data->scts.uSecond, pType1Data->scts.iZone, pType1Data->tooa, pType1Data->fo, pType1Data->pid, nDcs, // modify value 20070926
                       type2, nNumberSca, pType1Data->tosca, pType1Data->length);
        }
        else /* CSDH = 0 */
        {

            UINT8 *str1 = "+CMGR: \"%s\",\"%s%s\",,\"%u/%02u/%02u,%02u:%02u:%02u+%02d\"\r\n";
            UINT8 *str2 = "+CMGR: \"%s\",\"%s%s\",,\"%u/%02u/%02u,%02u:%02u:%02u%03d\"\r\n";
            UINT8 *str = NULL;

            if (pType1Data->scts.iZone < 0)
            {
                str = str2;
            }
            else
            {
                str = str1;
            }

            AT_Sprintf(ATAckBuff, str,
                       nstatus_tmp,
                       type1,
                       nNumber,
                       pType1Data->scts.uYear,
                       pType1Data->scts.uMonth,
                       pType1Data->scts.uDay,
                       pType1Data->scts.uHour, pType1Data->scts.uMinute, pType1Data->scts.uSecond, pType1Data->scts.iZone);
        }

        /* get prompt buffer sting length */
        nATBAckLen = AT_StrLen(ATAckBuff);

        if (pType1Data->dcs == 2) /* for chinese sms */
        {
            /* From low layer data is big ending and transfer to little ending */
            AT_Set_MultiLanguage();

            if (!(AT_UnicodeBigEnd2Unicode((UINT8 *)(pType1Data->data), pUCSLittleEndData, pType1Data->length)))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgr read msg resp:  BigEnd to LittleEnd error 1",
                                  CfwEvent.nUTI, nSim);
                return;
            }

            if (ERR_SUCCESS !=
                ML_Unicode2LocalLanguage((UINT8 *)pUCSLittleEndData, pType1Data->length, &pUCS2Data, &nGBKDATASize, NULL))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgr read msg resp:  unicode to local language error 1",
                                  CfwEvent.nUTI, nSim);
                return;
            }

            /* buffer overflow */
            if ((pType1Data->length + nATBAckLen) > sizeof(ATAckBuff))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "cmgr read msg resp: pType1Data buffer overflow dcs == 2",
                                  CfwEvent.nUTI, nSim);
                return;
            }

            AT_MemCpy(&ATAckBuff[nATBAckLen], pUCS2Data, pType1Data->length);
            nATBAckLen += pType1Data->length;

            // Free inout para resource
            AT_FREE(pUCS2Data);
        }
        else
        {
            /* buffer overflow */
            if ((pType1Data->length + nATBAckLen) > sizeof(ATAckBuff))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "cmgr read msg resp: pType1Data buffer overflow dcs != 2",
                                  CfwEvent.nUTI, nSim);
                return;
            }

            AT_MemCpy(&ATAckBuff[nATBAckLen], (UINT8 *)(pType1Data->data), pType1Data->length);
            nATBAckLen += pType1Data->length;
        }
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, ATAckBuff, nATBAckLen, CfwEvent.nUTI, nSim);
    }
    else if (nOperationRet == ERR_SUCCESS && pNode->nType == 3) // text mode, show param, state =sent/unsent msg
    {
        CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO *pType3Data = NULL;

        UINT8 type1[2] = "+";
        UINT8 type2[2] = "+";

        pType3Data = (CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO *)(pNode->pNode);

        /* just for debug */
        AT_TC(g_sw_AT_SMS, "pType3Data->length = %u", pType3Data->length);
        AT_TC_MEMBLOCK(g_sw_AT_SMS, pType3Data->data, pType3Data->length, 16);

        // change to ascii and get size
        count = SUL_GsmBcdToAscii(pType3Data->da, (pType3Data->da_size), nNumber);

        if (0xf0 == (*(pType3Data->da + pType3Data->da_size - 1) & 0xf0))
        {
            pType3Data->da_size = (UINT8)(pType3Data->da_size * 2 - 1);
        }
        else
        {
            pType3Data->da_size = (UINT8)(pType3Data->da_size * 2);
        }

        // sca  change to ascii and get size

        count = SUL_GsmBcdToAscii(pType3Data->sca, (pType3Data->sca_size), nNumberSca);

        // sms_tool_BCD2ASCII(nNumber, pType1Data->oa, (pType1Data->oa_size));
        if (0xf0 == (*(pType3Data->sca + pType3Data->sca_size - 1) & 0xf0))
        {
            pType3Data->sca_size = (UINT8)(pType3Data->sca_size * 2 - 1);
        }
        else
        {
            pType3Data->sca_size = (UINT8)(pType3Data->sca_size * 2);
        }

        // decide has + or not
        if (pType3Data->toda != CFW_TELNUMBER_TYPE_INTERNATIONAL)
        {
            type1[0] = '\0'; // [mod]bug7277
        }

        if (pType3Data->tosca != CFW_TELNUMBER_TYPE_INTERNATIONAL)
        {
            type2[0] = '\0'; // [mod]bug7277
        }

        // for dcs change from csw to correct value(3GPP) 2007.10.10
        nDcs = AT_SMS_DCSChange(pType3Data->dcs, nDcs);

        // output param first: CSDH = 1
        if (gATCurrentAtSmsSettingSCSDH_show == 1)
        {

            AT_Sprintf(ATAckBuff, "+CMGR: \"%s\",\"%s%s\",,%u,%u,%u,%u,%u,\"%s%s\",%u,%u\r\n", nstatus_tmp, type1, nNumber, pType3Data->toda, pType3Data->fo, pType3Data->pid, nDcs, // modify value 20070926
                       pType3Data->vp, type2, nNumberSca, pType3Data->tosca, pType3Data->length);
        }
        else /* CSDH = 0 */
        {
            AT_Sprintf(ATAckBuff, "+CMGR: \"%s\",\"%s%s\"\r\n", // [mod]2007.10.17 remove ",,"
                       nstatus_tmp, type1, nNumber);
        }

        /* get prompt buffer string length */
        nATBAckLen = AT_StrLen(ATAckBuff);

        if (pType3Data->dcs == 2) /* chinese message */
        {

            // receive from low layer and the data is big ending
            // so, we must transfer to little ending, the use it
            AT_Set_MultiLanguage();

            if (!(AT_UnicodeBigEnd2Unicode((UINT8 *)(pType3Data->data), pUCSLittleEndData, pType3Data->length)))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgr read msg resp:  BigEnd to LittleEnd error 2", nSim,
                                  CfwEvent.nUTI);
                return;
            }

            if (ERR_SUCCESS !=
                ML_Unicode2LocalLanguage((UINT8 *)pUCSLittleEndData, pType3Data->length, &pUCS2Data, &nGBKDATASize, NULL))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgr read msg resp:  unicode to local language error 2",
                                  CfwEvent.nUTI, nSim);
                return;
            }

            /* buffer overflow */
            if ((pType3Data->length + nATBAckLen) > sizeof(ATAckBuff))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "cmgr read msg resp: pType3Data buffer overflow dcs == 2",
                                  CfwEvent.nUTI, nSim);
                return;
            }

            AT_MemCpy(&ATAckBuff[nATBAckLen], pUCS2Data, pType3Data->length);
            nATBAckLen += pType3Data->length;

            // Free inout para resource
            AT_FREE(pUCS2Data);
        }
        else
        {
            /* buffer overflow */
            if ((pType3Data->length + nATBAckLen) > sizeof(ATAckBuff))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "cmgr read msg resp: pType3Data buffer overflow dcs != 2",
                                  CfwEvent.nUTI, nSim);
                return;
            }

            AT_MemCpy(&ATAckBuff[nATBAckLen], (UINT8 *)(pType3Data->data), pType3Data->length);
            nATBAckLen += pType3Data->length;
        }
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, ATAckBuff, nATBAckLen, CfwEvent.nUTI, nSim);
    }
    else if (nOperationRet == ERR_SUCCESS && pNode->nType == 7) // PDU mode, pp msg
    {
        CFW_SMS_PDU_INFO *pType7Data = NULL;
        UINT8 nDataSizeWithoutSCA = 0;
        UINT8 BCDDataBuff[200] = {0};
        UINT8 ASCIIDataBuff[400] = {0};
        UINT16 nTotalDataLen = 0;

        pType7Data = (CFW_SMS_PDU_INFO *)(pNode->pNode);

        /* just for debug */
        AT_TC(g_sw_AT_SMS, "Original Data: pType7Data->nDataSize = %u", pType7Data->nDataSize);

        /* check recv pdu and discard unused char */
        if (!(AT_SMS_CheckPDUIsValid((UINT8 *)pType7Data->pData, (UINT8 *)&(pType7Data->nDataSize), TRUE)))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_CHAR, NULL, 0, "cmgr read msg resp: check pdu is invalid", CfwEvent.nUTI,
                              nSim);
            return;
        }

        /* just for debug */
        AT_TC(g_sw_AT_SMS, "Worked Data: pType7Data->nDataSize = %u", pType7Data->nDataSize);
        AT_TC_MEMBLOCK(g_sw_AT_SMS, pType7Data->pData, pType7Data->nDataSize, 16);

        /* get status from the MACRO STATUS */
        if (!(AT_SMS_StatusMacroFlagToStringOrData(&(pType7Data->nStatus), 0)))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_STATUS, NULL, 0, "cmgr read msg resp: get pdu status exception",
                              CfwEvent.nUTI, nSim);
            return;
        }

        nDataSizeWithoutSCA = pType7Data->nDataSize - (pType7Data->pData[0] + 1); /* output size exclude scasize */

        AT_Sprintf(ATAckBuff, "+CMGR: %u,,%u\r\n", pType7Data->nStatus, nDataSizeWithoutSCA);

        /* add sca if have not */
        nTotalDataLen = pType7Data->nDataSize;
        if (!(SMS_AddSCA2PDU(pType7Data->pData, BCDDataBuff, &(nTotalDataLen), nSim)))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_CHAR, NULL, 0, "cmgr read msg resp: AddSCA2PDU error", CfwEvent.nUTI,
                              nSim);
            return;
        }

        // output msg body
        if (!(SMS_PDUBCDToASCII(BCDDataBuff, &(nTotalDataLen), ASCIIDataBuff)))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_CHAR, NULL, 0, "cmgr read msg resp: PDUBCDToASCII error", CfwEvent.nUTI,
                              nSim);
            return;
        }

        nATBAckLen = AT_StrLen(ATAckBuff);

        /* buffer overflow */
        if ((nATBAckLen + nTotalDataLen) > sizeof(ATAckBuff))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "cmgr read msg resp: buffer overflow dcs != 2", CfwEvent.nUTI,
                              nSim);
            return;
        }

        AT_MemCpy(&ATAckBuff[nATBAckLen], ASCIIDataBuff, nTotalDataLen);
        nATBAckLen += nTotalDataLen;
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, ATAckBuff, nATBAckLen, CfwEvent.nUTI, nSim);
        return;
    }
    else if (nOperationRet == ERR_SUCCESS && pNode->nType == 8) // PDU mode, CB msg
    {
        nOperationRet = ERR_AT_CMS_ERR_IN_MS;
    }
    else if (nOperationRet == ERR_SUCCESS)
    {
        nOperationRet = ERR_AT_CMS_ERR_IN_MS;
    }

    if (ERR_SUCCESS != nOperationRet) // err happen,
    {
        AT_SMS_Result_Err(nOperationRet, NULL, 0, "cmgr read msg resp: exception happened", CfwEvent.nUTI, nSim);
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_DeleteMessage_rsp
Description     :   AT SMS module delete message response
Called By           :   At SMS module
Data Accessed       :
Data Updated        :
Input           :       COS_EVENT *pEvent
Output          :
Return          :   VOID
Others          :
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
VOID AT_SMS_DeleteMessage_rsp(COS_EVENT *pEvent)
{
    CFW_EVENT CfwEvent = {0};
    CFW_SMS_STORAGE_INFO sStorageInfo = {0};
    CFW_SMS_STORAGE_INFO sUpdateStorageInfoSM = {0};
    CFW_SMS_STORAGE_INFO sUpdateStorageInfoME = {0};
    UINT8 nLastSMSCINDNewMsg = 0;
    UINT32 nOperationRet = ERR_SUCCESS;

    UINT8 nSim;
    AT_CosEvent2CfwEvent(pEvent, &CfwEvent);
    nSim = CfwEvent.nFlag;
    /* just for debug */
    AT_TC(g_sw_AT_SMS, "cmgd msg resp: CfwEvent.nType = 0x%x, CfwEvent.nParam1 = 0x%x, CfwEvent.nParam2 = %u",
          CfwEvent.nType, CfwEvent.nParam1, CfwEvent.nParam2);
    if (0 == CfwEvent.nType)
    {
        nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, gATCurrentAtSMSInfo[nSim].nStorage1, nSim);
        if (ERR_SUCCESS == nOperationRet)
        {
            gATCurrentAtSMSInfo[nSim].nTotal1 = sStorageInfo.totalSlot;
            gATCurrentAtSMSInfo[nSim].nUsed1 = sStorageInfo.usedSlot;
        }
        nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, gATCurrentAtSMSInfo[nSim].nStorage2, nSim);
        if (ERR_SUCCESS == nOperationRet)
        {
            gATCurrentAtSMSInfo[nSim].nTotal2 = sStorageInfo.totalSlot;
            gATCurrentAtSMSInfo[nSim].nUsed2 = sStorageInfo.usedSlot;
        }
        nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, gATCurrentAtSMSInfo[nSim].nStorage3, nSim);
        if (ERR_SUCCESS == nOperationRet)
        {
            gATCurrentAtSMSInfo[nSim].nTotal3 = sStorageInfo.totalSlot;
            gATCurrentAtSMSInfo[nSim].nUsed3 = sStorageInfo.usedSlot;
        }

        /* save temp value of  g_SMS_CIND_NewMsg,  if we delete
           ** the unread message ,we should update the unReadRecords */
        nLastSMSCINDNewMsg = g_SMS_CIND_NewMsg;
        g_SMS_CIND_NewMsg = 0;

        /* update the unReadcordes of SM */

        if (ERR_SUCCESS != CFW_CfgGetSmsStorageInfo(&sUpdateStorageInfoSM, CFW_SMS_STORAGE_SM, nSim))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgd msg resp: update SM error ", CfwEvent.nUTI, nSim);
            return;
        }
        g_SMS_CIND_NewMsg += sUpdateStorageInfoSM.unReadRecords;

        /* update the unReadcordes of ME */

        if (ERR_SUCCESS != CFW_CfgGetSmsStorageInfo(&sUpdateStorageInfoME, CFW_SMS_STORAGE_ME, nSim))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgd msg resp: update ME error ", CfwEvent.nUTI, nSim);
            g_SMS_CIND_NewMsg = nLastSMSCINDNewMsg;
            return;
        }
        g_SMS_CIND_NewMsg += sUpdateStorageInfoME.unReadRecords;

        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, CfwEvent.nUTI, nSim);
    }
    else if (0xF0 == CfwEvent.nType)
    {
        // [+] this index  no sms  but return ok(refer simens)2007.10.11
        // Invalid memory index #define ERR_CMS_INVALID_MEMORY_INDEX 0x10100142
        if (ERR_CMS_INVALID_MEMORY_INDEX == CfwEvent.nParam1)
        {
            AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, CfwEvent.nUTI, nSim);
        }

        // others return error
        else
        {
            nOperationRet = AT_SMS_ERR_Change(CfwEvent.nParam1);
            AT_SMS_Result_Err(nOperationRet, NULL, 0, "cmgd msg resp:  CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        }
    }
    else
    {
        AT_SMS_Result_Err(ERR_AT_CMS_UNKNOWN_ERROR, NULL, 0, "cmgd msg resp: exception happened", CfwEvent.nUTI, nSim);
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_RecieveOverFlow_Indictation
Description     :       AT receive indication of overflow from csw
Called By           :   At SMS module
Data Accessed       :
Data Updated        :
Input           :       COS_EVENT *pEvent
Output          :
Return          :   VOID
Others          :
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
VOID AT_SMS_RecvBuffOverflow_Indictation(COS_EVENT *pEvent)
{
    CFW_EVENT CfwEvent = {0};
    UINT8 ATAckBuff[32] = {0};
    UINT32 nOperationRet = ERR_SUCCESS;

    UINT8 nSim;
    /* get event */
    AT_CosEvent2CfwEvent(pEvent, &CfwEvent);
    nSim = CfwEvent.nFlag;
    /* just for debug */
    AT_TC(g_sw_AT_SMS,
          "msg buffer overflow indication: CfwEvent.nType = 0x%x, CfwEvent.nParam1 = 0x%x, CfwEvent.nParam2 = %u",
          CfwEvent.nType, CfwEvent.nParam1, CfwEvent.nParam2);

    /* check event type */
    if (0 == CfwEvent.nType)
    {

        //===========================================
        /* according to the MEMFULL status, then set the          */
        /* message full flag, and check "SM" storage and              */
        /* "ME" storage eparately, 2008.07.03, by wangqy                 */
        //===========================================
        switch (CfwEvent.nParam1)
        {

        case 0: /* delete message and storage is spare */
            if (gATCurrentAtSMSInfo[nSim].nStorage1 & CFW_SMS_STORAGE_SM)
            {
                /* save msgfull flag of SM, bit1 is set to "0" */
                g_SMS_CIND_MemFull[nSim] &= ~CFW_SMS_STORAGE_SM;
            }
            else if (gATCurrentAtSMSInfo[nSim].nStorage1 & CFW_SMS_STORAGE_ME)
            {
                /* save msgfull flag of ME, bit0 is set to "0" */
                g_SMS_CIND_MemFull[nSim] &= ~CFW_SMS_STORAGE_ME;
            }
            else
            {
                AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, NULL, 0, "cmgd msg resp: not support this storage 0",
                                  CfwEvent.nUTI, nSim);
                return;
            }

            break;

        case 1: /* mem full and no message waiting for */
        case 2: /* mem full and have message waiting for */
            if ((gATCurrentAtSMSInfo[nSim].nStorage2 & CFW_SMS_STORAGE_SM) || (gATCurrentAtSMSInfo[nSim].nStorage3 & CFW_SMS_STORAGE_SM))
            {
                /* save msgfull flag of SM, bit1 is set to "1" */
                g_SMS_CIND_MemFull[nSim] |= CFW_SMS_STORAGE_SM;
            }
            else if ((gATCurrentAtSMSInfo[nSim].nStorage2 & CFW_SMS_STORAGE_ME) || (gATCurrentAtSMSInfo[nSim].nStorage3 & CFW_SMS_STORAGE_ME))
            {
                /* save msgfull flag of ME, bit0 is set to "1" */
                g_SMS_CIND_MemFull[nSim] |= CFW_SMS_STORAGE_ME;
            }
            else
            {
                AT_SMS_Result_Err(ERR_AT_CMS_OPER_NOT_ALLOWED, NULL, 0,
                                  "msg buffer overflow indication: not support this storage 12", CfwEvent.nUTI, nSim);
                return;
            }

            break;

        default:
            AT_TC(g_sw_AT_SMS, "msg buffer overflow indication: unknown CfwEvent.nParam1 value");

            break;
        }
        AT_TC(g_sw_AT_SMS, ("We Got gATCurrentuCmer_ind nSim[%d] to %d\n"), nSim, gATCurrentuCmer_ind[nSim]);

        /* report to CIEV or not  */
        if (gATCurrentuCmer_ind[nSim])
        {
            AT_Sprintf(ATAckBuff, "+CIEV: \"SMSFULL\",%u", (unsigned int)CfwEvent.nParam1);
            AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, ATAckBuff, (UINT16)AT_StrLen(ATAckBuff), CfwEvent.nUTI,
                             nSim);
        }
        else
        {
            AT_TC(g_sw_AT_SMS, "msg buffer overflow indication: received indiation, but not report to CIEV");
        }
    }
    else if (0xF0 == CfwEvent.nType)
    {
        nOperationRet = AT_SMS_ERR_Change(CfwEvent.nParam1);
        AT_SMS_Result_Err(nOperationRet, NULL, 0, "msg buffer overflow indication: CfwEvent.nType == 0xF0", nSim,
                          CfwEvent.nUTI);
    }
    else
    {
        AT_SMS_Result_Err(ERR_AT_CMS_UNKNOWN_ERROR, NULL, 0, "msg buffer overflow indication: exception happened",
                          CfwEvent.nUTI, nSim);
    }

    return;
}

// ZSC-T20121222-s
#ifdef __MODEM_LP_MODE_ENABLE__
VOID AT_ResendCMTMux(UINT8 nDLCI)
{
    UINT8 nSim = AT_SIM_ID(nDLCI);

    AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, g_sms_delaybuffer, AT_StrLen(g_sms_delaybuffer), nDLCI, nSim); // hameina[+] ring info should be set out
    AT_FREE(g_sms_delaybuffer);
    return;
}
#endif
// ZSC-T20121222-E
/******************************************************************************************
Function            :   AT_SMS_RecieveNew_Indictation
Description     :       AT receive indication of new
Called By           :   At SMS module
Data Accessed       :
Data Updated        :
Input           :       COS_EVENT *pEvent
Output          :
Return          :   VOID
Others          :   modify and add comment by wangqunyang 06/04/2008
*******************************************************************************************/
VOID AT_SMS_RecvNewMessage_Indictation(COS_EVENT *pEvent)
{

    CFW_SMS_STORAGE_INFO sStorageInfo = {0};
    CFW_EVENT CfwEvent = {0};

    CFW_NEW_SMS_NODE *pNewMsgNode = NULL;

    UINT8 ATAckBuff[480] = {0};
    UINT8 nFormat = 0;
    UINT16 nATBAckLen = 0;
    UINT32 nGBKDATASize = 0;
    UINT8 *pUCS2Data = NULL;
    UINT8 pUCSLittleEndData[142] = {0};
    UINT8 nDcs = 0;
    UINT32 nOperationRet = ERR_SUCCESS;

    UINT8 nSim;
    /* get event */
    AT_CosEvent2CfwEvent(pEvent, &CfwEvent);
    nSim = CfwEvent.nFlag;
    CfwEvent.nUTI = AT_ASYN_GET_DLCI(nSim);
    /* just for debug */
    AT_TC(g_sw_AT_SMS, "new msg indication: CfwEvent.nType = 0x%x, CfwEvent.nParam1 = 0x%x, CfwEvent.nParam2 = %u",
          CfwEvent.nType, CfwEvent.nParam1, CfwEvent.nParam2);

    /* get SMS format and check it */
    nFormat = gATCurrentAtSmsSettingSg_SMSFormat;
    if (nFormat > 1)
    {
        AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "new msg indication: nFormat > 1 ", CfwEvent.nUTI, nSim);
        return;
    }

    /* get new message node and check it */
    pNewMsgNode = (CFW_NEW_SMS_NODE *)CfwEvent.nParam1;
    if (NULL == pNewMsgNode)
    {
        AT_SMS_Result_Err(ERR_AT_CMS_MEM_FAIL, NULL, 0, "new msg indication: pNewMsgNode is NULL", CfwEvent.nUTI, nSim);
        return;
    }

    /* check event type */
    if (0 != CfwEvent.nType)
    {
        if (0xF0 == CfwEvent.nType)
        {
            nOperationRet = AT_SMS_ERR_Change(CfwEvent.nParam1);
            AT_SMS_Result_Err(nOperationRet, NULL, 0, "new msg indication: CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        }
        else
        {
            AT_SMS_Result_Err(ERR_AT_CMS_UNKNOWN_ERROR, NULL, 0, "new msg indication: exception happened", CfwEvent.nUTI,
                              nSim);
        }

        return;
    }

    /**********************************************************************/
    // Check message node type and mti, then process them
    /**********************************************************************/
    AT_TC(g_sw_AT_SMS, "pNewMsgNode->nType = 0x%x", pNewMsgNode->nType);

    /* check the msg node type */
    switch (pNewMsgNode->nType)
    {

    // ///////////////////////////////////
    // PDU Message Content
    // ///////////////////////////////////
    case 0x20:

        //===========================================
        // update the new message counter, and indicate to
        // CIEV that new message recieved if necessary
        //===========================================

        g_SMS_CIND_NewMsg++;

        // /////////////////////////////////////
        /* Report to CIEV         */
        // /////////////////////////////////////

        AT_TC(g_sw_AT_SMS, ("We Got gATCurrentuCmer_ind nSim[%d] to %d\n"), nSim, gATCurrentuCmer_ind[nSim]);

        if (gATCurrentuCmer_ind[nSim])
        {
            UINT8 nRptCMERBuff[24] = {0};

            AT_Sprintf(nRptCMERBuff, "+CIEV: \"MESSAGE\",%u", g_SMS_CIND_NewMsg ? 1 : 0);
            AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, nRptCMERBuff, (UINT16)AT_StrLen(nRptCMERBuff), CfwEvent.nUTI,
                             nSim);
        }
        else
        {
            /* Recieved new messgage but report CIEV flag is 0,
             ** and not reptort to CIEV, this is just for debug
             */
            AT_TC(g_sw_AT_SMS, "new msg indication: Received new message, but not report to CMER");
        }

        //===========================================
        // indicate to CMTI that new message recieved if necessary
        //===========================================

        /* No report MT == 0 */
        if (gATCurrentAtSMSInfo[nSim].sCnmi.nMt == 0)
        {
            AT_TC(g_sw_AT_SMS, "new msg indication: Received new pdu format message, but not report to CNMI");
            return;
        }

        /* dump report MT == 1 */
        else if (gATCurrentAtSMSInfo[nSim].sCnmi.nMt == 1)
        {
            if (pNewMsgNode->nStorage == 1)
            {
                AT_Sprintf(ATAckBuff, "+CMTI: \"ME\",%u", pNewMsgNode->nConcatCurrentIndex);
            }
            else
            {
                AT_Sprintf(ATAckBuff, "+CMTI: \"SM\",%u", pNewMsgNode->nConcatCurrentIndex);
            }
            AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, ATAckBuff, (UINT16)AT_StrLen(ATAckBuff), CfwEvent.nUTI,
                             nSim);
            return;
        }

        /* dump report MT == 2 */
        else if (gATCurrentAtSMSInfo[nSim].sCnmi.nMt == 2)
        {
            UINT8 *pPduData = NULL;
            UINT8 ATAsciiBuff[400] = {0};

            CFW_SMS_PDU_INFO *pPduNodeInfo = NULL;

            if (0 != nFormat)
            {
                AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "new msg indication: Format exception", CfwEvent.nUTI, nSim);
                return;
            }

            pPduNodeInfo = (CFW_SMS_PDU_INFO *)(pNewMsgNode->pNode);

            /* just for debug */
            AT_TC(g_sw_AT_SMS, "new msg indication: pPduNodeInfo->nDataSize = %u", pPduNodeInfo->nDataSize);
            AT_TC_MEMBLOCK(g_sw_AT_SMS, pPduNodeInfo->pData, pPduNodeInfo->nDataSize, 16);

            /* check recv pdu and discard unused char */
            if (!(AT_SMS_CheckPDUIsValid((UINT8 *)pPduNodeInfo->pData, (UINT8 *)&(pPduNodeInfo->nDataSize), TRUE)))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_CHAR, NULL, 0, "new msg indication: check pdu is invalid",
                                  CfwEvent.nUTI, nSim);
                return;
            }

            AT_Sprintf(ATAckBuff, "+CMT: ,%u\r\n", pPduNodeInfo->nDataSize - pPduNodeInfo->pData[0] - 1);

            /* get prompt buffer string length */
            nATBAckLen = AT_StrLen(ATAckBuff);

            pPduData = pPduNodeInfo->pData;

            if (!(SMS_PDUBCDToASCII(pPduData, &(pPduNodeInfo->nDataSize), ATAsciiBuff)))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_CHAR, NULL, 0, "new msg indication: PDUBCDToASCII error",
                                  CfwEvent.nUTI, nSim);
                return;
            }

            /* buffer overflow */
            if ((nATBAckLen + pPduNodeInfo->nDataSize) > sizeof(ATAckBuff))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0, "new msg indication: buffer overflow", CfwEvent.nUTI, nSim);
                return;
            }

            AT_MemCpy(&ATAckBuff[nATBAckLen], ATAsciiBuff, pPduNodeInfo->nDataSize);
            nATBAckLen += pPduNodeInfo->nDataSize;
#ifdef __MODEM_LP_MODE_ENABLE__
            if (sync_IsApAllowBpSleep())
            {
                sync_BpWakeUpAP();
                AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, ATAckBuff, nATBAckLen, CfwEvent.nUTI, nSim);
            }
            else
            {
                AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, ATAckBuff, nATBAckLen, CfwEvent.nUTI, nSim);
            }
#else /* if not define lower function then Executive below program */

            AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, ATAckBuff, nATBAckLen, CfwEvent.nUTI, nSim);
#endif
        }

        /* MT support 0, 1 and 2, other values not implements */
        else
        {
            AT_TC(g_sw_AT_SMS, "new msg indication: Received pdu format message, but this type of MT is not implements");
        }

        break;

    // //////////////////////////////////////
    // 1. Text Message Content with header
    // 2. Text Message Content without header
    // //////////////////////////////////////
    case 0x21:
    case 0x22:

        //===========================================
        // update the new message counter, and indicate to
        // CIEV that new message recieved if necessary
        //===========================================

        g_SMS_CIND_NewMsg++;

        // /////////////////////////////////////
        /* Report to CIEV         */
        // /////////////////////////////////////
        AT_TC(g_sw_AT_SMS, ("We Got gATCurrentuCmer_ind nSim[%d] to %d\n"), nSim, gATCurrentuCmer_ind[nSim]);

        if (gATCurrentuCmer_ind[nSim])
        {
            UINT8 nRptCMERBuff[24] = {0};

            AT_Sprintf(nRptCMERBuff, "+CIEV: \"MESSAGE\",%u", g_SMS_CIND_NewMsg ? 1 : 0);
            AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, nRptCMERBuff, (UINT16)AT_StrLen(nRptCMERBuff), CfwEvent.nUTI,
                             nSim);
        }
        else
        {
            /* Recieved new messgage but report CIEV flag is 0,
             ** and not reptort to CIEV, this is just for debug
             */
            AT_TC(g_sw_AT_SMS, "new msg indication: Received new message, but not report to CMER");
        }

        //===========================================
        // indicate to CMTI that new message recieved if necessary
        //===========================================

        /* No report MT == 0 */
        if (gATCurrentAtSMSInfo[nSim].sCnmi.nMt == 0)
        {
            AT_TC(g_sw_AT_SMS, "new msg indication: Received new text format message, but not report to CNMI");
            return;
        }

        /* dump report MT == 1 */
        else if (gATCurrentAtSMSInfo[nSim].sCnmi.nMt == 1)
        {
            if (pNewMsgNode->nStorage == 1)
            {
                AT_Sprintf(ATAckBuff, "+CMTI: \"ME\",%u", pNewMsgNode->nConcatCurrentIndex);
            }
            else
            {
                AT_Sprintf(ATAckBuff, "+CMTI: \"SM\",%u", pNewMsgNode->nConcatCurrentIndex);
            }
            AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, ATAckBuff, (UINT16)AT_StrLen(ATAckBuff), CfwEvent.nUTI,
                             nSim);
            return;
        }

        /* dump report MT == 2 */
        else if (gATCurrentAtSMSInfo[nSim].sCnmi.nMt == 2)
        {
            CFW_SMS_TXT_HRD_V1_IND *pTextWithHead = NULL;
            UINT8 tooa = 0, tosca = 0;
            UINT8 nNumber[TEL_NUMBER_MAX_LEN] = {0};
            UINT8 nNumberSca[TEL_NUMBER_MAX_LEN] = {0};

            pTextWithHead = (CFW_SMS_TXT_HRD_V1_IND *)(pNewMsgNode->pNode);

            /* just for debug */
            AT_TC(g_sw_AT_SMS, "new msg indication: pTextWithHead->nDataLen = %u", pTextWithHead->nDataLen);
            AT_TC_MEMBLOCK(g_sw_AT_SMS, pTextWithHead->pData, pTextWithHead->nDataLen, 16);

            if (0xD0 != pTextWithHead->tooa)
            {
                // change to ascii and get size
                if (0 == SUL_GsmBcdToAscii(pTextWithHead->oa, (pTextWithHead->oa_size), nNumber))
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_CHAR, NULL, 0, "new msg indication: GsmBcdToAscii 11", CfwEvent.nUTI,
                                      nSim);
                    return;
                }
            }

            /*
                  if (0xf0 == (*(pTextWithHead->oa + pTextWithHead->oa_size - 1) & 0xf0))
                  {
                    pTextWithHead->oa_size = (UINT8)(pTextWithHead->oa_size * 2 - 1);
                  }
                  else
                  {
                    pTextWithHead->oa_size = (UINT8)(pTextWithHead->oa_size * 2);
                  }
            */

            if (0xD0 != pTextWithHead->tosca)
            {
                // sca  change to ascii and get size
                if (0 == SUL_GsmBcdToAscii(pTextWithHead->sca, (pTextWithHead->sca_size), nNumberSca))
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_CHAR, NULL, 0, "new msg indication: GsmBcdToAscii 22", CfwEvent.nUTI,
                                      nSim);
                    return;
                }
            }

            /*

            if (0xf0 == (*(pTextWithHead->sca + pTextWithHead->sca_size - 1) & 0xf0))
            {
              pTextWithHead->sca_size = (UINT8)(pTextWithHead->sca_size * 2 - 1);
            }
            else
            {
              pTextWithHead->sca_size = (UINT8)(pTextWithHead->sca_size * 2);
            }
            */
            if (CFW_TELNUMBER_TYPE_INTERNATIONAL == pTextWithHead->tooa)
            {
                tooa = '+';
            }
            else if (0xd0 == pTextWithHead->tooa)
            {
                if (0 == SUL_Decode7Bit(pTextWithHead->oa, nNumber, (pTextWithHead->oa_size)))
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_CHAR, NULL, 0, "new msg indication: GsmBcdToAscii 11", CfwEvent.nUTI,
                                      nSim);
                    return;
                }
            }
            else
            {
                tooa = '\0';
            }

            if (CFW_TELNUMBER_TYPE_INTERNATIONAL == pTextWithHead->tosca)
            {
                tosca = '+';
            }
            else if (0xd0 == pTextWithHead->tooa)
            {
                if (0 == SUL_Decode7Bit(pTextWithHead->sca, nNumberSca, (pTextWithHead->sca_size)))
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_CHAR, NULL, 0, "new msg indication: GsmBcdToAscii 11", CfwEvent.nUTI,
                                      nSim);
                    return;
                }
            }
            else
            {
                tosca = '\0';
            }

            /*
             ** CSDH=1 show header
             */
            if (gATCurrentAtSmsSettingSCSDH_show)
            {
                nDcs = AT_SMS_DCSChange(pTextWithHead->dcs, nDcs);

                UINT8 *str1 = "+CMT: \"%c%s\",,\"%u/%02u/%02u,%02u:%02u:%02u+%02d\",%u,%u,%u,%u,\"%c%s\",%u,%u\r\n";
                UINT8 *str2 = "+CMT: \"%c%s\",,\"%u/%02u/%02u,%02u:%02u:%02u%03d\",%u,%u,%u,%u,\"%c%s\",%u,%u\r\n";
                UINT8 *str = NULL;

                if (pTextWithHead->scts.iZone < 0)
                {
                    str = str2;
                }
                else
                {
                    str = str1;
                }

                // output param
                AT_Sprintf(ATAckBuff, str,
                           tooa,
                           nNumber,
                           pTextWithHead->scts.uYear,
                           pTextWithHead->scts.uMonth,
                           pTextWithHead->scts.uDay,
                           pTextWithHead->scts.uHour,
                           pTextWithHead->scts.uMinute,
                           pTextWithHead->scts.uSecond,
                           pTextWithHead->scts.iZone,
                           pTextWithHead->tooa,
                           pTextWithHead->fo,
                           pTextWithHead->pid, nDcs, tosca, nNumberSca, pTextWithHead->tosca, pTextWithHead->nDataLen);
            }

            /*
             ** CSDH= 0 not show header
             */
            else
            {
                UINT8 *str1 = "+CMT: \"%c%s\",,\"%u/%02u/%02u,%02u:%02u:%02u+%02d\"\r\n";
                UINT8 *str2 = "+CMT: \"%c%s\",,\"%u/%02u/%02u,%02u:%02u:%02u%03d\"\r\n";
                UINT8 *str = NULL;

                if (pTextWithHead->scts.iZone < 0)
                {
                    str = str2;
                }
                else
                {
                    str = str1;
                }

                AT_Sprintf(ATAckBuff, str,
                           tooa,
                           nNumber,
                           pTextWithHead->scts.uYear,
                           pTextWithHead->scts.uMonth,
                           pTextWithHead->scts.uDay,
                           pTextWithHead->scts.uHour,
                           pTextWithHead->scts.uMinute, pTextWithHead->scts.uSecond, pTextWithHead->scts.iZone);
            }

            /* get prompt buffer string length */
            nATBAckLen = AT_StrLen(ATAckBuff);

            // chinese message
            if (pTextWithHead->dcs == 2)
            {
                /* we must transfer the uniform big ending */
                /* to little ending and using */
                AT_Set_MultiLanguage();

                if (!(AT_UnicodeBigEnd2Unicode((UINT8 *)(pTextWithHead->pData), pUCSLittleEndData, pTextWithHead->nDataLen)))
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "new msg indication: BigEnd to LittleEnd error", CfwEvent.nUTI,
                                      nSim);
                    return;
                }

                //===========================================
                /* process invalid char in the unicode string                 */
                //===========================================
                if (ERR_SUCCESS !=
                    ML_Unicode2LocalLanguage((UINT8 *)pUCSLittleEndData, pTextWithHead->nDataLen, &pUCS2Data, &nGBKDATASize,
                                             NULL))
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "new msg indication: unicode to local language error",
                                      CfwEvent.nUTI, nSim);
                    return;
                }

                /* check buffer overflow or not */
                if ((pTextWithHead->nDataLen + nATBAckLen) > sizeof(ATAckBuff))
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0,
                                      "new msg indication: pTextWithHead buffer overflow dcs == 2", CfwEvent.nUTI, nSim);

                    return;
                }

                AT_MemCpy(&ATAckBuff[nATBAckLen], pUCS2Data, pTextWithHead->nDataLen);
                nATBAckLen += pTextWithHead->nDataLen;

                // Free inout para resource
                AT_FREE(pUCS2Data);
            }
            else
            {

                /* buffer overflow */
                if ((pTextWithHead->nDataLen + nATBAckLen) > sizeof(ATAckBuff))
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_INVALID_LEN, NULL, 0,
                                      "new msg indication: pTextWithHead buffer overflow dcs != 2", CfwEvent.nUTI, nSim);
                    return;
                }

                if (pTextWithHead->dcs == 0x00)
                {
                    UINT8 *pGsm7Bit = (UINT8 *)AT_MALLOC(pTextWithHead->nDataLen);
                    UINT16 nGsm7BitLen = 0x00;

                    ATConvertGSM7BitDefaultEncodingToAscii(pGsm7Bit, pTextWithHead->pData, pTextWithHead->nDataLen, &nGsm7BitLen);
                    AT_MemCpy(&ATAckBuff[nATBAckLen], (UINT8 *)(pGsm7Bit), nGsm7BitLen);
                }
                else
                    AT_MemCpy(&ATAckBuff[nATBAckLen], (UINT8 *)(pTextWithHead->pData), pTextWithHead->nDataLen);
                nATBAckLen += pTextWithHead->nDataLen;
            }
            AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, ATAckBuff, nATBAckLen, CfwEvent.nUTI, nSim);
        }

        /* MT support 0, 1 and 2, other values not implements */
        else
        {
            AT_TC(g_sw_AT_SMS, "new msg indication: Received text format message, but this type of MT is not implements");
        }

        break;

    // ///////////////////////////////////
    // PDU status report
    // ///////////////////////////////////
    case 0x40:

        /* DS support 0 and 1, other values not implements */
        if (gATCurrentAtSMSInfo[nSim].sCnmi.nDs == 0)
        {
            AT_TC(g_sw_AT_SMS, "new msg indication: Received PDU status message, but not report to CNMI");
        }
        else if (gATCurrentAtSMSInfo[nSim].sCnmi.nDs == 1)
        {
            AT_SMS_CDSPDUReport(pNewMsgNode, CfwEvent.nUTI, nSim);
        }
        else
        {
            AT_TC(g_sw_AT_SMS, "new msg indication: Received PDU status message, but DS of this type not implements");
        }

        break;

    // ///////////////////////////////////
    // text status report
    // ///////////////////////////////////
    case 0x41:

        /* DS support 0 and 1, other values not implements */
        if (gATCurrentAtSMSInfo[nSim].sCnmi.nDs == 0)
        {
            AT_TC(g_sw_AT_SMS, "new msg indication: Received TEXT status message, but not report to CNMI");
        }
        else if (gATCurrentAtSMSInfo[nSim].sCnmi.nDs == 1)
        {
            AT_SMS_CDSTextReport(pNewMsgNode, CfwEvent.nUTI, nSim);
        }
        else
        {
            AT_TC(g_sw_AT_SMS, "new msg indication: Received TEXT status message, but DS of this type not implements");
        }

        break;

    //===========================================
    /* these are not implements from csw following      */
    //===========================================
    case 0x30:
        AT_TC(g_sw_AT_SMS,
              "new msg indication: CBM PDU format broadcast message received, but nType of this case not implements");
        break;

    case 0x31:
        AT_TC(g_sw_AT_SMS,
              "new msg indication: CBM TEXT format broadcast message received, but nType of this case not implements");
        break;

    default:

        AT_TC(g_sw_AT_SMS, "new msg indication: Unknown New Message nType");
        break;
    }

    // update the storage3

    nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, gATCurrentAtSMSInfo[nSim].nStorage3, nSim);

    if (ERR_SUCCESS == nOperationRet)
    {
        gATCurrentAtSMSInfo[nSim].nTotal3 = sStorageInfo.totalSlot;
        gATCurrentAtSMSInfo[nSim].nUsed3 = sStorageInfo.usedSlot;
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_RecvCbMessage_Indictation
Description     :       AT receive indication of cb
Called By           :   At SMS module
Data Accessed       :
Data Updated        :
Input           :       COS_EVENT *pEvent
Output          :
Return          :   VOID
*******************************************************************************************/
typedef struct
{
    u16 MId; // Message Id the SMS-CB belongs to
    u16 DataLen;
    u8 DCS;     // Data Coding Scheme applicable to the SMS-CB message
    u8 Data[1]; // In fact, DataLen elts must be allocated for this
    // array
} CB_MSG_IND_STRUCT_EX;

typedef enum _AT_MSG_ALPHABET {
    AT_MSG_GSM_7 = 0,
    AT_MSG_8_BIT_DATA,
    AT_MSG_UCS2,
    AT_MSG_RESERVED,
} AT_MSG_ALPHABET;

UINT8 gnRptCBMBuff[680] = {0};
UINT8 gATAsciiBuff[600] = {0};

VOID AT_SMS_RecvCbMessage_Indictation(COS_EVENT *pEvent)
{

    CFW_SMS_STORAGE_INFO sStorageInfo = {0};
    CFW_EVENT CfwEvent = {0};
    UINT16 nATBAckLen = 0;
    UINT8 nFormat = 0;
    UINT32 nOperationRet = ERR_SUCCESS;
    UINT16 nPduSize = 0;

    UINT8 nSim;

    AT_MemSet(gATAsciiBuff, 0x00, SIZEOF(gATAsciiBuff));
    AT_MemSet(gnRptCBMBuff, 0x00, SIZEOF(gnRptCBMBuff));

    /* get event */
    AT_CosEvent2CfwEvent(pEvent, &CfwEvent);
    nSim = CfwEvent.nFlag;
    CfwEvent.nUTI = AT_ASYN_GET_DLCI(nSim);

    /* just for debug */
    AT_TC(g_sw_AT_SMS, "new msg indication: CfwEvent.nType = 0x%x, CfwEvent.nParam1 = 0x%x, CfwEvent.nParam2 = %u",
          CfwEvent.nType, CfwEvent.nParam1, CfwEvent.nParam2);

    /* get SMS format and check it */
    nFormat = gATCurrentAtSmsSettingSg_SMSFormat;

    /* get new message node and check it */
    //pNewMsgNode = (CFW_NEW_SMS_NODE *)CfwEvent.nParam1;
    CB_MSG_IND_STRUCT_EX *pSmsPduInfo = AT_MALLOC(sizeof(CB_MSG_IND_STRUCT_EX));
    pSmsPduInfo = (CB_MSG_IND_STRUCT_EX *)CfwEvent.nParam1;
    ;

    nPduSize = pSmsPduInfo->DataLen;
    AT_Sprintf(gnRptCBMBuff, "+CBM:%d,", nPduSize);
    nATBAckLen = AT_StrLen(gnRptCBMBuff);

    //AT_Sprintf(nRptCBMBuff, "+CBM: 44,2D250032011154192C39AC425535DB4D85BBDD46D269EDD56C341A8D46A3D168341A8D46A3D168341A8D46A3\n");
    AT_TC(g_sw_AT_SMS, "CSW_TC_MEMBLOCK CFW_SMSCB_RECEIVE_IND Data !");
    AT_TC_MEMBLOCK(g_sw_AT_SMS, pSmsPduInfo->Data, nPduSize, 16);

    //if(nPduSize <= 198)
    if (nPduSize <= 300)
    {
        AT_TC(g_sw_AT_SMS, "cfg_GetTeChset=%d\n", cfg_GetTeChset());

        if (cs_hex == cfg_GetTeChset())
        {
            AT_TC(g_sw_AT_SMS, "_GenerateCUSData cs_hex == cfg_GetTeChset()\n");
            AT_MSG_ALPHABET nUSSDDCS = AT_CBS_CheckDCS(pSmsPduInfo->DCS);

            if (nUSSDDCS == AT_MSG_GSM_7 || nUSSDDCS == AT_MSG_8_BIT_DATA)
            {
                UINT8 *in = NULL;
                UINT8 nSwitchData = 0x00;
                UINT8 i = 0x00;

                in = pSmsPduInfo->Data;

                for (i = 0x00; i < pSmsPduInfo->DataLen; i++)
                {
                    nSwitchData = (in[i] >> 4) + (in[i] << 4);
                    in[i] = nSwitchData;
                }
            }
            SUL_GsmBcdToAsciiEx(pSmsPduInfo->Data, pSmsPduInfo->DataLen, gATAsciiBuff);
        }
        else
        {
            AT_TC(g_sw_AT_SMS, "_GenerateCUSData TE Chset not HEX\n");
            SUL_MemCopy8(gATAsciiBuff, pSmsPduInfo->Data, pSmsPduInfo->DataLen);
        }

        AT_MemCpy(&gnRptCBMBuff[nATBAckLen], gATAsciiBuff, 2 * nPduSize);

        //AT_StrCat(nRptCBMBuff,ATAsciiBuff);
    }
    else
    {
        AT_TC(g_sw_AT_SMS, "ERROR! PDU SIZE TO LONG!");
    }

    AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, gnRptCBMBuff, (UINT16)AT_StrLen(gnRptCBMBuff), CfwEvent.nUTI,
                     nSim);
    pSmsPduInfo = NULL;
    nOperationRet = CFW_CfgGetSmsStorageInfo(&sStorageInfo, gATCurrentAtSMSInfo[nSim].nStorage3, nSim);
    if (ERR_SUCCESS == nOperationRet)
    {
        gATCurrentAtSMSInfo[nSim].nTotal3 = sStorageInfo.totalSlot;
        gATCurrentAtSMSInfo[nSim].nUsed3 = sStorageInfo.usedSlot;
    }
    return;
}

/******************************************************************************************
Function                :   AT_SMS_Result_OK
Description            :    create SMS Successful result code and notify ATE
Called By              :    by SMS module
Data Accessed       :
Data Updated         :
Input                     :     UINT32 uReturnValue, indication the event type
                             UINT32 uResultCode, command execute result code
                             UINT8  nDelayTime, delay time
                             UINT8* pBuffer, data pointer
                             UINT16 nDataSize, data length
Output                   :
Return                   :  void
Others                   :  build by unknown     2007.10.18
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
VOID AT_SMS_Result_OK(UINT32 uReturnValue,
                      UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize, UINT8 nDLCI, UINT8 nSim)
{
    PAT_CMD_RESULT pResult = NULL;
    AT_BACKSIMID(nSim);

    // 填充结果码
    pResult = AT_CreateRC(uReturnValue,
                          uResultCode, CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CMS, nDelayTime, pBuffer, nDataSize, nDLCI);

    AT_TC(g_sw_AT_SMS, "> func = %s, nDLCI = %d", __func__, nDLCI);
    AT_Notify2ATM(pResult, nDLCI);

    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }

    // Add by wangqunyang 2008.04.09
    AT_TC(g_sw_AT_SMS, "> AT SMS Notify To ATM, Return Value: %s , Result Code: %s, nDLCI = %d,pBuffer= %s",
          AT_Get_RetureValue_Name(uReturnValue), AT_Get_ResultCode_Name(uResultCode), nDLCI, pBuffer);

    return;
}

/******************************************************************************************
Function            :   AT_SMS_Result_Err
Description     :       create SMS Error result code and notify ATE
Called By           :   by SMS module
Data Accessed   :
Data Updated    :
Input           :       UINT32 uErrorCode, UINT8* pBuffer, UINT16 nDataSize
Output          :
Return          :   void
Others          :   build by wangqunyang 03/04/2008
*******************************************************************************************/
VOID AT_SMS_Result_Err(UINT32 uErrorCode, UINT8 *pBuffer, UINT16 nDataSize, UINT8 *pExtendErrInfo, UINT8 nDLCI,
                       UINT8 nSim)
{
    PAT_CMD_RESULT pResult = NULL;
    UINT8 DefaultExtendInfo[32] = "No Extend Info";

    if (pExtendErrInfo)
        AT_TC(g_sw_AT_SMS, "%s[%s]", __func__, pExtendErrInfo);

    AT_BACKSIMID(nSim);

    // 填充结果码
    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR, uErrorCode, CMD_ERROR_CODE_TYPE_CMS, 0, pBuffer, nDataSize, nDLCI);

    AT_Notify2ATM(pResult, nDLCI);

    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }

    // Add by wangqunyang 2008.04.09
    if (NULL == pExtendErrInfo)
    {
        pExtendErrInfo = DefaultExtendInfo;
    }

    return;
}

/******************************************************************************************
Function            :   AT_SMS_StatusMacroFlagToStringOrData
Description     :   macro status flag to string or data
Called By           :   AT_SMS_RESULT_OK
Data Accessed   :
Data Updated    :
Input           :   UINT8* pSetStatusBuff, UINT8 nFormat
Output          :
Return          :   BOOL
Others          :   build by wangqunyang 09/04/2008
*******************************************************************************************/
BOOL AT_SMS_StatusMacroFlagToStringOrData(UINT8 *pStatusMacroFlagBuff, UINT8 nFormat)
{

    if (nFormat) /* text mode */
    {

        if (pStatusMacroFlagBuff[0] == CFW_SMS_STORED_STATUS_READ)
        {
            AT_StrCpy(pStatusMacroFlagBuff, "REC READ");
        }
        else if (pStatusMacroFlagBuff[0] == CFW_SMS_STORED_STATUS_UNREAD)
        {
            AT_StrCpy(pStatusMacroFlagBuff, "REC UNREAD");
        }
        else if (pStatusMacroFlagBuff[0] == 8 || pStatusMacroFlagBuff[0] == 0)
        {
            AT_StrCpy(pStatusMacroFlagBuff, "STO SENT");
        }
        else if (pStatusMacroFlagBuff[0] == CFW_SMS_STORED_STATUS_UNSENT)
        {
            AT_StrCpy(pStatusMacroFlagBuff, "STO UNSENT");
        }
        else
        {
            AT_TC(g_sw_AT_SMS, "text mode: get status exception 1");
            return FALSE;
        }
    }
    else /* pdu mode */
    {

        if (pStatusMacroFlagBuff[0] == CFW_SMS_STORED_STATUS_READ)
        {
            pStatusMacroFlagBuff[0] = 1;
        }
        else if (pStatusMacroFlagBuff[0] == CFW_SMS_STORED_STATUS_UNREAD)
        {
            pStatusMacroFlagBuff[0] = 0;
        }
        else if (pStatusMacroFlagBuff[0] == 8 || pStatusMacroFlagBuff[0] == 0)
        {
            pStatusMacroFlagBuff[0] = 3;
        }
        else if (pStatusMacroFlagBuff[0] == CFW_SMS_STORED_STATUS_UNSENT)
        {
            pStatusMacroFlagBuff[0] = 2;
        }
        else
        {
            AT_TC(g_sw_AT_SMS, "pdu mode: get status exception 2");
            return FALSE;
        }
    }

    return TRUE;
}

/******************************************************************************************
Function            :   AT_SMS_StringOrDataToStatusMacroFlag
Description     :   string or data to macro flag
Called By           :   AT_SMS_RESULT_OK
Data Accessed   :
Data Updated    :
Input           :   UINT8* pStatusFlag, UINT8* pGetSaveStatus, UINT8 nFormat
Output          :
Return          :   BOOL
Others          :   build by wangqunyang 09/04/2008
*******************************************************************************************/
BOOL AT_SMS_StringOrDataToStatusMacroFlag(UINT8 *pStatusValue, UINT8 *pStatusFlag, UINT8 nFormat)
{
    AT_TC(g_sw_AT_SMS, " AT_SMS_StringOrDataToStatusMacroFlag nState %d", *pStatusValue);

    if (nFormat) /* text mode */
    {

        if (!strcmp("REC UNREAD", pStatusValue))
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_UNREAD;
        }
        else if (!strcmp("REC READ", pStatusValue))
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_READ;
        }
        else if (!strcmp("STO UNSENT", pStatusValue))
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_UNSENT;
        }
        else if (!strcmp("STO SENT", pStatusValue))
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ;
        }
        else if (!strcmp("ALL", pStatusValue))
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_STORED_ALL;
        }
        else
        {
            AT_TC(g_sw_AT_SMS, "text mode: get status exception 3");
            return FALSE;
        }
    }
    else /* pdu mode */
    {

        if (pStatusValue[0] == 0)
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_UNREAD;
        }
        else if (pStatusValue[0] == 1)
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_READ;
        }
        else if (pStatusValue[0] == 2)
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_UNSENT;
        }
        else if (pStatusValue[0] == 3)
        {
            pStatusFlag[0] =
                CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ | CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV |
                CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE | CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE;
        }
        else if (pStatusValue[0] == 4)
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_STORED_ALL;
        }
        else
        {
            AT_TC(g_sw_AT_SMS, "pdu mode: get status exception 4");
            return FALSE;
        }
    }

    return TRUE;
}

/******************************************************************************************
Function            :   AT_SMS_CheckPDUIsValid
Description     :       check pdu is valid or not
Called By           :   by SMS module
Data Accessed   :
Data Updated        :
Input           :       UINT8 * pPDUData,
                    UINT8 * nPDULen,
                    BOOL bReadMsgFlag
Output          :
Return          :   BOOL
Others          :   build by wangqunyang 2008.05.13
*******************************************************************************************/
BOOL AT_SMS_CheckPDUIsValid(UINT8 *pPDUData, UINT8 *nPDULen, BOOL bReadOrListMsgFlag)
{
    UINT8 nIndex = 0;
    UINT8 nUserDataLen = 0;
    UINT8 nDCSFlag = 0;
    UINT8 nDABytes = 0;
    BOOL bSubmitFlag = FALSE;
    UINT8 nDABCDLen = 0;
    UINT8 nVPLen = 0;

    /* Input data check */
    if (pPDUData == NULL || *nPDULen == 0)
    {
        AT_TC(g_sw_AT_SMS, "check pdu: PDUData pointer is NULL or PDU len is 0");
        return FALSE;
    }

    /* Get the index following SCA */
    nIndex = pPDUData[0] + 1;

    /********************************************************
    ** first , we get the MTI flag,  message or not
    ** MTI 00 : deliver mti 01 : submit
    ** other bits ,we omit and now implement
    *********************************************************/
    if (0x01 == (pPDUData[nIndex] & 0x03))
    {
        bSubmitFlag = TRUE;
    }

    //===========================================/////
    // PDU is to write message or send message
    //===========================================/////
    if (bSubmitFlag)
    {

        /* FO flag parse Beginning........
           ** MTI 00 : deliver mti 01 : submit
         */

        /* FO is omit parse */

        /* RD is omit parse */

        /* VPF parse and set the VP lenght */
        if ((pPDUData[nIndex] & 0x18) == 0x00) /* 00 000 */
        {
            nVPLen = 0;
        }
        else if ((pPDUData[nIndex] & 0x18) == 0x08) /* 01 000  reserved */
        {
            nVPLen = 8;
        }
        else if ((pPDUData[nIndex] & 0x18) == 0x10) /* 10 000 */
        {
            nVPLen = 8;
        }
        else /* 11 000 */
        {
            nVPLen = 4;
        }

        /* SRR is omit parse */

        /* UDHI is omit parse */

        /* RP is omit parse */

        /*
           ** FO flag parsed completely and increase the index
         */
        //
        nIndex++;

        /* MR: message reference is omit parse */
        nIndex++;

        /* DA: How many Dial number's */
        if (pPDUData[nIndex] % 2)
        {
            nDABCDLen = pPDUData[nIndex] / 2 + 1; // odd number
        }
        else
        {
            nDABCDLen = pPDUData[nIndex] / 2; // even number
        }

        // All DA area is less than 12 bytes
        if ((1 + 1 + nDABCDLen) > 12)
        {
            AT_TC(g_sw_AT_SMS, "MO check pdu: SCA field octs length is more than 12 ");
            return FALSE;
        }
        nIndex = nIndex + 1 + 1 + nDABCDLen;

        /*
           ** PID byte index, set default 0x00 for PID
         */
        if (0x00 != pPDUData[nIndex])
        {
            //pPDUData[nIndex] = 0x00; //Modify by Lixp for GCF test
        }
        nIndex++;

        //===========================================
        // here: only parse the encode bits of DCS byte,
        // other bits omit and not implement
        //
        // bit7..bit4 - encode group
        // bit7 - reserved
        // bit6 - reserved
        // bit5 - 0:text uncompress 1: GSM default compress
        // bit4 - 0: bit0 and bit1 no use 1: bit0 and bit1 useful
        //
        // bit0: bit1:
        // 0     0 class1
        // 0     1 class2
        // 1     0 class3
        // 1     1 class4
        //
        // bit2: bit3:
        // 0 0 GSM default 7 bit
        // 0 1 8 bit dcs
        // 1 0 16bit ucs
        // 1 1 reserved
        //===========================================
        if (0x00 == (pPDUData[nIndex] & 0x0C))
        {
            nDCSFlag = 7;
        }
        else if (0x04 == (pPDUData[nIndex] & 0x0C))
        {
            nDCSFlag = 8;
        }
        else if (0x08 == (pPDUData[nIndex] & 0x0C))
        {
            nDCSFlag = 16;
        }
        else
        {
            AT_TC(g_sw_AT_SMS, "MO check pdu: DCS field value is not supported by us ");
            return FALSE;
        }

        nIndex++;

        /*
           ** parse VP is exist or not
         */
        if (0 == nVPLen) /* No VP */
        {
        }
        else if (8 == nVPLen) /* one byte */
        {
            nIndex++;
        }
        else /* half byte */
        {
            nIndex++;

            /* read the spec and make sure which half byte, high or low?
               continue ...... to do
               if(0x00!= pPDUData[nIndex] & 0xF0)
               {
               return FALSE;
               } */
        }

        /* UDL parse */
        if (7 == nDCSFlag)
        {
            if (pPDUData[nIndex] > 160)
            {
                AT_TC(g_sw_AT_SMS, "MO check pdu: 7bit encode and user data len more 160 ");
                return FALSE;
            }

            if ((pPDUData[nIndex] * 7) % 8)
            {
                nUserDataLen = (pPDUData[nIndex] * 7) / 8 + 1;
            }
            else
            {
                nUserDataLen = (pPDUData[nIndex] * 7) / 8;
            }
        }
        else
        {
            if (pPDUData[nIndex] > 140)
            {
                AT_TC(g_sw_AT_SMS, "MO check pdu: 8bit or 16bit encode and user data len more than 140 ");
                return FALSE;
            }

            nUserDataLen = pPDUData[nIndex];
        }

        if (bReadOrListMsgFlag)
        {
            /* out parameters of  nOutputPDULen */
            *nPDULen = nUserDataLen + nIndex + 1;
            pPDUData[*nPDULen] = '\0';
        }
        else
        {

            /* check the input length is valid */
            if ((pPDUData[0] + 1 > *nPDULen) || ((*nPDULen > AT_SMS_BODY_BUFF_LEN)))
            {

                AT_TC(g_sw_AT_SMS, "MO check pdu: PDU len overflow");
                return FALSE;
            }

            /* check the total length */
            if (nUserDataLen + nIndex + 1 != *nPDULen)
            {
                AT_TC(g_sw_AT_SMS, "MO check pdu: length is not match");
                return FALSE;
            }
        }

        return TRUE;
    }

    //===========================================
    // parse PDU is recv from others
    //===========================================/////
    else
    {
        nIndex++;

        /* DA parser */
        if (pPDUData[nIndex] % 2)
        {
            nDABytes = (pPDUData[nIndex] + 1) / 2;
        }
        else
        {
            nDABytes = pPDUData[nIndex] / 2;
        }

        nIndex = nIndex + 1 + 1 + nDABytes; /* length, toda DA */

        /* PID omit parser */
        nIndex++;

        //===========================================
        // here: only parse the encode bits of  DCS byte,
        // other bits omit and not implement now,
        // the bit meaning as above description
        //===========================================
        if (0x00 == (pPDUData[nIndex] & 0x0C))
        {
            nDCSFlag = 7;
        }
        else if (0x04 == (pPDUData[nIndex] & 0x0C))
        {
            nDCSFlag = 8;
        }
        else if (0x08 == (pPDUData[nIndex] & 0x0C))
        {
            nDCSFlag = 16;
        }
        else
        {
            AT_TC(g_sw_AT_SMS, "MT check pdu: DCS field value is not supported by us ");
            return FALSE;
        }
        nIndex++;

        /* 7 bytes of time stamp */
        nIndex = nIndex + 7;

        /* UDL parse */
        if (7 == nDCSFlag)
        {
            if (pPDUData[nIndex] > 160)
            {
                AT_TC(g_sw_AT_SMS, "MT check pdu: 7bit encode and user data len more 160 ");
                return FALSE;
            }

            if ((pPDUData[nIndex] * 7) % 8)
            {
                nUserDataLen = (pPDUData[nIndex] * 7) / 8 + 1;
            }
            else
            {
                nUserDataLen = (pPDUData[nIndex] * 7) / 8;
            }
        }
        else
        {
            if (pPDUData[nIndex] > 140)
            {
                AT_TC(g_sw_AT_SMS, "MT check pdu: 8bit or 16bit encode and user data len more 140 ");
                return FALSE;
            }

            nUserDataLen = pPDUData[nIndex];
        }

        if (bReadOrListMsgFlag)
        {
            /* check the total length */
            if (nUserDataLen + nIndex + 1 > AT_SMS_BODY_BUFF_LEN)
            {
                AT_TC(g_sw_AT_SMS, "MT check pdu: totol pdu len is overflow ");
                return FALSE;
            }

            /* out parameters of  nOutputPDULen */
            *nPDULen = nUserDataLen + nIndex + 1;
            pPDUData[*nPDULen] = '\0';
        }
        else
        {
            AT_TC(g_sw_AT_SMS, "nUserDataLen = %u, nIndex = %u, *nPDULen = %u", nUserDataLen, nIndex, *nPDULen);

            /* check the input length is valid */
            if ((pPDUData[0] + 1 > *nPDULen) || ((*nPDULen > AT_SMS_BODY_BUFF_LEN)))
            {
                AT_TC(g_sw_AT_SMS, "MT check pdu: totol pdu len is overflow ");
                return FALSE;
            }

            /* check the total length */
            if (nUserDataLen + nIndex + 1 != *nPDULen)
            {
                AT_TC(g_sw_AT_SMS, "MT check pdu: PDU length is not match ");
                return FALSE;
            }
        }

        return TRUE;
    }
}

/******************************************************************************************
Function            :   AT_SMS_IsValidPhoneNumber
Description     :       check number copy from pbk module is valid or not
Called By           :   by SMS module
Data Accessed       :
Data Updated        :
Input           :       UINT8 * arrNumber, UINT8 nNumberSize, BOOL * bIsInternational
Output          :
Return          :   BOOL
Others          :       //[+]2007.11.29 for check number copy from pbk module
                    //arrNumber[in]; pNumberSize[in]; bIsInternational[out]
                    //legal phone number char: 0-9,*,#,+ (+ can only be the first position)
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
BOOL AT_SMS_IsValidPhoneNumber(UINT8 *arrNumber, UINT8 nNumberSize, BOOL *bIsInternational)
{
    UINT8 nTemp = 0;

    if ((arrNumber == NULL) || (0 == nNumberSize))
    {
        AT_TC(g_sw_AT_SMS, "SMS check phone number: pointer is NULL or number size is 0 ");
        return FALSE;
    }

    *bIsInternational = FALSE;

    if (arrNumber[0] == '+')
    {
        *bIsInternational = TRUE;
        nTemp = 1;
    }

    while (nTemp < nNumberSize)
    {
        if ((arrNumber[nTemp] != '#') && (arrNumber[nTemp] != '*') && ((arrNumber[nTemp] < '0') || (arrNumber[nTemp] > '9')))
        {
            AT_TC(g_sw_AT_SMS, "SMS check phone number: invalid char in the number");
            return FALSE;
        }

        nTemp++;
    }

    return TRUE;
}

/******************************************************************************************
Function            :   AT_SMS_ERR_Change
Description     :       transfer Error Code between CSW and AT
Called By           :   by SMS module
Data Accessed       :
Data Updated        :
Input           :       UINT32 nInErrcode
Output          :
Return          :   UINT32
Others          :       //[+]CSW ERROR CODE change to  AT ERROR CODE 2007.10.12-10.16
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
UINT32 AT_SMS_ERR_Change(UINT32 nInErrcode)
{

    switch (nInErrcode)
    {
    case ERR_CMS_INVALID_MEMORY_INDEX: // [+]from rsp(param1) return error 2007.10.15
        return ERR_AT_CMS_INVALID_MEM_INDEX;
        break;

    case ERR_CMS_MEMORY_FULL:
        return ERR_AT_CMS_MEM_FULL;
        break;

    case ERR_CMS_TEMPORARY_FAILURE:
        return ERR_AT_CMS_TMEP_FAIL;
        break;

    case ERR_CMS_OPERATION_NOT_ALLOWED:
        return ERR_AT_CMS_OPER_NOT_ALLOWED;
        break;

    case ERR_CMS_MEMORY_CAPACITY_EXCEEDED:
        return ERR_AT_CMS_MEM_CAP_EXCCEEDED;
        break;

    case ERR_CMS_SMSC_ADDRESS_UNKNOWN:
        return ERR_AT_CMS_SCA_ADDR_UNKNOWN;
        break;

    case ERR_CMS_INVALID_CHARACTER_IN_PDU:
        return ERR_AT_CMS_INVALID_PDU_CHAR;
        break;

    case ERR_CMS_INVALID_CHARACTER_IN_ADDRESS_STRING:
        return ERR_AT_CMS_INVALID_ADDR_CHAR;
        break;

    case ERR_CMS_INVALID_PDU_MODE_PARAMETER:
        return ERR_AT_CMS_INVALID_PDU_PARAM;
        break;

    case ERR_CMS_INVALID_TEXT_MODE_PARAMETER:
        return ERR_AT_CMS_INVALID_TXT_PARAM;
        break;

    case ERR_CFW_INVALID_PARAMETER: // [+]csw function return error 2007.10.15
        return ERR_AT_CMS_INVALID_PARA;
        break;

    case ERR_CMS_TP_VPF_NOT_SUPPORTED:
        return ERR_AT_CMS_TP_VPF_NOT_SUPP;
        break;

    case ERR_CMS_INVALID_STATUS:
        return ERR_AT_CMS_INVALID_STATUS;
        break;

    case ERR_CMS_UNSPECIFIED_TP_PID_ERROR: // csw send() return error
    case ERR_CMS_UNSPECIFIED_TP_DCS_ERROR:
    case ERR_CMS_UNKNOWN_ERROR:
    case ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED:
    case ERR_CFW_NOT_EXIST_FREE_UTI: // [+]csw function return error 2007.10.15
    case ERR_CFW_UTI_IS_BUSY:
    default:
        return ERR_AT_CMS_UNKNOWN_ERROR;
        break;
    }
}

/******************************************************************************************
Function            :   SMS_PDUBCDToASCII
Description     :       transfer PDU BCD Code to ASCII Code
Called By           :   by SMS module
Data Accessed   :
Data Updated    :
Input           :       UINT8 * pBCD,
                    UINT16 *pLen,
                    UINT8 * pStr
Output          :
Return          :   UINT16
Others          :       build by unknown and unknown date
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
BOOL SMS_PDUBCDToASCII(UINT8 *pBCD, UINT16 *pLen, UINT8 *pStr)
{
    UINT8 Tmp = 0;

    UINT32 i;

    if (pStr == NULL || pBCD == NULL)
    {
        AT_TC(g_sw_AT_SMS, "BCD to ASCII: pointer is NULL");
        return FALSE;
    }

    AT_MemSet(pStr, 0, (*pLen) * 2);

    for (i = 0; i < *pLen; i++)
    {
        // ////////////////////////////
        // high 4 bits
        // ////////////////////////////
        Tmp = pBCD[i] >> 4;
        if (Tmp < 10) // 0~9
        {
            pStr[i * 2] = Tmp + '0';
        }
        else if (Tmp > 9 && Tmp < 16) // 10~15
        {
            pStr[i * 2] = Tmp + 55;
        }
        else
        {
            AT_TC(g_sw_AT_SMS, "BCD to ASCII: high bits some char is more than 'F' of HEX");
            return FALSE;
        }

        // ////////////////////////////
        // low 4 bits
        // ////////////////////////////
        Tmp = pBCD[i] & 0x0f;
        if (Tmp < 10) // 0~9
        {
            pStr[i * 2 + 1] = Tmp + '0';
        }
        else if (Tmp > 9 && Tmp < 16) // 10~15
        {
            pStr[i * 2 + 1] = Tmp + 55;
        }
        else
        {
            AT_TC(g_sw_AT_SMS, "BCD to ASCII: low bits some char is more than 'F' of HEX");
            return FALSE;
        }
    }

    *pLen = i * 2;

    return TRUE;
}

/******************************************************************************************
Function            :   AT_CIND_SMS
Description     :   CIND Command inner procedure function
Called By           :   AT_GC_CmdFunc_CIND
Data Accessed       :
Data Updated        :
Input           :       UINT8* pNewMsg,UINT8* pMemFull
Output          :
Return          :   BOOL
Others          :       //[+]2007.10.30 for +cind interface func
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
BOOL AT_CIND_SMS(UINT8 *pNewMsg, UINT8 *pMemFull, UINT8 nSim)
{
    *pNewMsg = g_SMS_CIND_NewMsg ? 1 : 0;

    /* if ME storage and SM storage are both full, SMSFULL is 1, orthers is 0 */
    *pMemFull = ((CFW_SMS_STORAGE_SM == (g_SMS_CIND_MemFull[nSim] & CFW_SMS_STORAGE_SM)) && (CFW_SMS_STORAGE_ME == (g_SMS_CIND_MemFull[nSim] & CFW_SMS_STORAGE_ME))) ? 1 : 0;
    if (*pNewMsg > 1 || *pMemFull > 1)
    {
        AT_TC(g_sw_AT_SMS, "AT_CIND_SMS: new msg or msg full indicator more than 1");
        return FALSE;
    }

    return TRUE;
}

/******************************************************************************************
Function            :   AT_SMS_CDS_TEXT
Description     :       AT SMS CDS Command inner procedure function
Called By           :   AT_GC_CmdFunc_CIND
Data Accessed   :
Data Updated        :
Input           :       CFW_NEW_SMS_NODE* nParam1ToSmsNode
Output          :
Return          :   VOID
Others          :       //[+]2007.10.24 for +cds text
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
VOID AT_SMS_CDSTextReport(CFW_NEW_SMS_NODE *pNewMessageData, UINT8 nUTI, UINT8 nSim)
{

    CFW_SMS_TXT_STATUS_IND *pTextWithHeadSR = NULL; // nType=0x41

    UINT8 ATBAckBuff[128] = {0};
    UINT8 nNumberRa[TEL_NUMBER_MAX_LEN] = {0};
    UINT8 nCount = 0, nRaSize = 0;
    UINT8 nType[2] = {0}, nTypeScts[2] =
                              {
                                  0},
          nTypeDt[2] =
              {
                  0};

    pTextWithHeadSR = (CFW_SMS_TXT_STATUS_IND *)(pNewMessageData->pNode);

    /* just for debug */
    AT_TC(g_sw_AT_SMS, "new msg CDSTextReport: pTextWithHeadSR->ra_size = %u", pTextWithHeadSR->ra_size);

    /* ra  change to ascii and get size */
    nCount = SUL_GsmBcdToAscii(pTextWithHeadSR->ra, (pTextWithHeadSR->ra_size), nNumberRa);

    if (0xf0 == (*(pTextWithHeadSR->ra + pTextWithHeadSR->ra_size - 1) & 0xf0))
    {
        nRaSize = (UINT8)(pTextWithHeadSR->ra_size * 2 - 1);
    }
    else
    {
        nRaSize = (UINT8)(pTextWithHeadSR->ra_size * 2);
    }

    /* decide has + or not */
    if (pTextWithHeadSR->tora == CFW_TELNUMBER_TYPE_INTERNATIONAL)
    {
        AT_StrCpy(nType, "+");
    }

    /* scts iZone */
    if (pTextWithHeadSR->scts.iZone >= 0)
    {
        AT_StrCpy(nTypeScts, "+");
    }
    else
    {
        pTextWithHeadSR->scts.iZone = 0 - pTextWithHeadSR->scts.iZone;
        AT_StrCpy(nTypeScts, "-");
    }

    /* dt iZone */
    if (pTextWithHeadSR->dt.iZone >= 0)
    {
        AT_StrCpy(nTypeDt, "+");
    }
    else
    {
        pTextWithHeadSR->dt.iZone = 0 - pTextWithHeadSR->dt.iZone;
        AT_StrCpy(nTypeDt, "-");
    }

    AT_Sprintf(ATBAckBuff,
               "+CDS: %u,%u,\"%s%s\",%u,\"%u/%02u/%02u,%02u:%02u:%02u%s%02d\",\"%u/%02u/%02u,%02u:%02u:%02u%s%02d\",%u",
               pTextWithHeadSR->fo, pTextWithHeadSR->mr, nType, nNumberRa, pTextWithHeadSR->tora,
               pTextWithHeadSR->scts.uYear, pTextWithHeadSR->scts.uMonth, pTextWithHeadSR->scts.uDay,
               pTextWithHeadSR->scts.uHour, pTextWithHeadSR->scts.uMinute, pTextWithHeadSR->scts.uSecond, nTypeScts,
               pTextWithHeadSR->scts.iZone, pTextWithHeadSR->dt.uYear, pTextWithHeadSR->dt.uMonth,
               pTextWithHeadSR->dt.uDay, pTextWithHeadSR->dt.uHour, pTextWithHeadSR->dt.uMinute,
               pTextWithHeadSR->dt.uSecond, nTypeDt, pTextWithHeadSR->dt.iZone, pTextWithHeadSR->st);

    AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, ATBAckBuff, (UINT16)AT_StrLen(ATBAckBuff), nUTI, nSim);

    return;
}

/******************************************************************************************
Function            :   AT_SMS_CDS_PDU
Description     :       cds pdu execute inner procedure function
Called By           :   AT_SMS_RecieveNew_Indictation
Data Accessed       :
Data Updated        :
Input           :       CFW_NEW_SMS_NODE* nParam1ToSmsNode
Output          :
Return          :   VOID
Others          :       //[+]2007.10.24 for +cds pdu
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
VOID AT_SMS_CDSPDUReport(CFW_NEW_SMS_NODE *pNewMessageData, UINT8 nUTI, UINT8 nSim)
{

    CFW_SMS_PDU_INFO *pPduSR = NULL;
    UINT8 ASCIIBuff[AT_SMS_BODY_BUFF_LEN] = {0};
    UINT8 ATBAckBuff[256] = {0};
    UINT16 nATBAckLen = 0;

    pPduSR = (CFW_SMS_PDU_INFO *)(pNewMessageData->pNode);

    /* just for debug */
    AT_TC(g_sw_AT_SMS, "new msg CDSPDUReport: pPduSR->nDataSize = %u", pPduSR->nDataSize);
    AT_TC_MEMBLOCK(g_sw_AT_SMS, pPduSR->pData, pPduSR->nDataSize, 16);

    AT_Sprintf(ATBAckBuff, "+CDS: %u\r\n", pPduSR->nDataSize);
    nATBAckLen = AT_StrLen(ATBAckBuff);

    /* convert PDU BCD to ascii string */
    if (!(SMS_PDUBCDToASCII(pPduSR->pData, &(pPduSR->nDataSize), ASCIIBuff)))
    {
        AT_SMS_Result_Err(ERR_AT_CMS_INVALID_PDU_CHAR, NULL, 0, "CDSPDUReport: format is not match", nUTI, nSim);
        return;
    }

    AT_MemCpy(&ATBAckBuff[nATBAckLen], ASCIIBuff, pPduSR->nDataSize);
    nATBAckLen += pPduSR->nDataSize;
    AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, ATBAckBuff, nATBAckLen, nUTI, nSim);

    return;
}

/******************************************************************************************
Function            :   AT_SMS_DCSChange
Description     :       csw dcs transfer to AT dcs
Called By           :   unknown
Data Accessed   :
Data Updated    :
Input           :       UINT8 InDCS,UINT8 OutDCS
Output          :
Return          :   UINT8
Others          :       //[+]csw dcs change to AT dcs 2007.10.10

*******************************************************************************************/
UINT8 AT_SMS_DCSChange(UINT8 InDCS, UINT8 OutDCS)
{

    if (InDCS == 0)
    {
        OutDCS = GSM_7BIT_UNCOMPRESSED;
    }
    else if (InDCS == 1)
    {
        OutDCS = GSM_8BIT_UNCOMPRESSED;
    }
    else if (InDCS == 2)
    {
        OutDCS = GSM_UCS2_UNCOMPRESSED;
    }
    else if (InDCS == 0x10)
    {
        OutDCS = GSM_7BIT_COMPRESSED;
    }
    else if (InDCS == 0x11)
    {
        OutDCS = GSM_8BIT_COMPRESSED;
    }
    else if (InDCS == 0x12)
    {
        OutDCS = GSM_UCS2_COMPRESSED;
    }
    else if (InDCS == 4)
    {
        OutDCS = GSM_8BIT_UNCOMPRESSED;
    }

    return OutDCS;
}

/******************************************************************************************
Function            :   SMS_AddSCA2PDU
Description     :       add sca to pdu
Called By           :   SMS module
Data Accessed       :
Data Updated        :
Input           :       UINT8 *InPDU, UINT8 *OutPDU, UINT16* pSize
Output          :
Return          :   UINT32
Others          :
                        modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
BOOL SMS_AddSCA2PDU(UINT8 *InPDU, UINT8 *OutPDU, UINT16 *pSize, UINT8 nSim)
{
    CFW_SMS_PARAMETER sInfo = {0};

    if (InPDU == NULL || OutPDU == NULL || *pSize == 0)
    {
        AT_TC(g_sw_AT_SMS, "AddSCA2PDU: pointer is NULL or size is 0");
        return FALSE;
    }

    // SCA not present
    if (InPDU[0] == 0)
    {

        if (ERR_SUCCESS != CFW_CfgGetSmsParam(&sInfo, 0, nSim))
        {
            AT_TC(g_sw_AT_SMS, "AddSCA2PDU: get SMS param error");
            return FALSE;
        }

        AT_MemCpy(OutPDU, sInfo.nNumber, sInfo.nNumber[0] + 1);

        AT_MemCpy(&OutPDU[OutPDU[0] + 1], &InPDU[1], (*pSize) - 1);
        *pSize = OutPDU[0] + *pSize;
    }
    else
    {
        AT_MemCpy(OutPDU, InPDU, *pSize);
    }

    return TRUE;
}

/******************************************************************************************
Function            :   SMS_PDUAsciiToBCD
Description     :       this function used to check the input PDU has the SCA or not,
                                if not, add the SCA to the begin of the OUTPDU string.
Called By           :   AT_SMS_CmdFunc_CMGS
                                AT_SMS_CmdFunc_CMGW
Data Accessed       :
Data Updated        :
Input           :       UINT8 * pNumber, UINT16 nNumberLen, UINT8 * pBCD
Output          :
Return          :   UINT16
Others          :       modify and add comment by wangqunyang 06/04/2008
*******************************************************************************************/
UINT16 SMS_PDUAsciiToBCD(UINT8 *pNumber, UINT16 nNumberLen, UINT8 *pBCD)
{

    UINT8 *pTmp = pBCD;
    UINT8 Tmp = 0;
    UINT32 i = 0;
    UINT16 nBcdSize = 0;

    if (pNumber == NULL || pBCD == NULL)
    {
        AT_TC(g_sw_AT_SMS, "AsciiToBCD: ponter is NULL");
        return FALSE;
    }

    AT_MemSet(pBCD, 0, nNumberLen >> 1);

    for (i = 0; i < nNumberLen; i++)
    {
        if ((*pNumber >= 65) && (*pNumber <= 70))
        {
            Tmp = (INT8)(*pNumber - 55);
        }
        else if ((*pNumber >= 97) && (*pNumber <= 102))
        {
            Tmp = (INT8)(*pNumber - 87);
        }
        else if ((*pNumber >= '0') && (*pNumber <= '9'))
        {
            Tmp = (INT8)(*pNumber - '0');
        }
        else
        {
            AT_TC(g_sw_AT_SMS, "AsciiToBCD: some ascii char is more than 'F' of HEX");
            return FALSE;
        }

        if (i % 2)
        {
            *pTmp++ |= (INT8)(Tmp & 0x0F);
        }
        else
        {
            *pTmp = (Tmp << 4) & 0xF0;
        }

        pNumber++;
    }

    nBcdSize = nNumberLen >> 1;

    if (i % 2)
    {
        *pTmp |= 0xF0;
        nBcdSize += 1;
    }

    return nBcdSize;
}
BOOL AT_SMS_GetPDUNUM(UINT8 *pPDUData, UINT8 *pPDUNUM, UINT8 *pType, UINT8 *nPDULen)
{
    UINT8 nIndex = 0;

    /* Input data check */
    if (pPDUData == NULL || *pPDUNUM == 0)
    {
        AT_TC(g_sw_AT_SMS, "GetPDUNUM is NULL or PDU len is 0");
        return FALSE;
    }

    /* Get the index following SCA */
    nIndex = pPDUData[0] + 1;
    /* SRR is omit parse */
    /* UDHI is omit parse */
    /* RP is omit parse */
    /*
    ** FO flag parsed completely and increase the index
    */
    //
    nIndex++;

    /* MR: message reference is omit parse */
    nIndex++;

    /* DA: How many Dial number's */
    if (pPDUData[nIndex] % 2)
    {
        *nPDULen = pPDUData[nIndex] / 2 + 1; // odd number
    }
    else
    {
        *nPDULen = pPDUData[nIndex] / 2; // even number
    }
    // All DA area is less than 12 bytes
    if ((1 + 1 + *nPDULen) > 12)
    {
        AT_TC(g_sw_AT_SMS, "GetPDUNUM: SCA field octs length is more than 12 ");
        return FALSE;
    }
    *pType = pPDUData[nIndex + 1];

    AT_MemCpy(pPDUNUM, &(pPDUData[nIndex + 2]), *nPDULen);

    AT_TC(g_sw_AT_SMS, "GetPDUNUM is end\n");
    //  AT_TC_MEMBLOCK(g_sw_AT_SMS, pPDUNUM, *nPDULen, 16);
    return TRUE;
}
BOOL AT_SMS_GetSCANUM(UINT8 *pPDUData, UINT8 *pSACNUM, UINT8 *nPDULen)
{
    UINT8 nIndex = 0;

    /* Input data check */
    if (pPDUData == NULL || *pSACNUM == 0)
    {
        AT_TC(g_sw_AT_SMS, "GetPDUNUM is NULL or PDU len is 0");
        return FALSE;
    }

    /* Get the index following SCA */
    nIndex = 1;

    /* DA: How many Dial number's */
    *nPDULen = pPDUData[0] - 1;
    // All DA area is less than 12 bytes
    if ((1 + 1 + *nPDULen) > 12)
    {
        AT_TC(g_sw_AT_SMS, "GetPDUNUM: SCA field octs length is more than 12 ");
        return FALSE;
    }
    AT_MemCpy(pSACNUM, &(pPDUData[nIndex + 1]), *nPDULen);
    AT_TC(g_sw_AT_SMS, "GetSCANUM is end\n");
    //  AT_TC_MEMBLOCK(g_sw_AT_SMS, pSACNUM, *nPDULen, 16);
    return TRUE;
}

#ifdef AT_MODULE_SUPPORT_OTA
/***************************************************************************
 * Global Declarations
 ***************************************************************************/
static const JC_INT8 *gpcBrwAppID = (const JC_INT8 *)"w2";
static const JC_INT8 *gpcMMSAppID = (const JC_INT8 *)"w4";
#if 1
static const JC_INT8 *gpcNullString = (const JC_INT8 *)"(null)";
static const JC_INT8 *gpcGPRSString = (const JC_INT8 *)"GSM-GPRS";
static const JC_INT8 *gpcCSDString = (const JC_INT8 *)"GSM-CSD";
static const JC_INT8 *gpcModemString = (const JC_INT8 *)"ANALOG_MODEM";
static const JC_INT8 *gpcX31String = (const JC_INT8 *)"X.31";
#endif
//===========================================/////
static const JC_INT8 *gpcCoWspString = (const JC_INT8 *)"CO-WSP";
static const JC_INT8 *gpcClWspString = (const JC_INT8 *)"CL-WSP";
static const JC_INT8 *gpcCoSecWspString = (const JC_INT8 *)"CO-SEC-WSP";
static const JC_INT8 *gpcClSecWspString = (const JC_INT8 *)"CL-SEC-WSP";

// ZSC-T20121222-E

UINT8 ADP_GetFreeUti()
{
    UINT8 uti, ret;
    ret = CFW_GetFreeUTI(0, &uti);
    //	mmi_trace_b(_MMI|TLEVEL(g_sw_ADP_SIM)|TDB|TNB_ARG(3)|TSMAP(1),TSTR("UTI: Func: %s uti=%d, ret=0x%x",0x090000eb), __FUNCTION__, uti, ret);
    return uti;
}
#define FREE_UTI (ADP_GetFreeUti())

JC_RETCODE jMMS_CommCB(void *pvAppArg, E_COMM_INDICATION eCommIndicationType,
                       void *pvIndication)
{
    JC_RETCODE rCode = JC_OK;

    AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d eCommIndicationType = %d \n", __func__, __LINE__, eCommIndicationType);
    switch (eCommIndicationType)
    {
    case E_PUSH_INDICATION: /**< Specifies the push data that is received by the lower layer (Argument #ST_COMM_PUSH_IND). */
    {
        //mmi_trace_b (_MMI|TLEVEL(11)|TDB|TNB_ARG(0), TSTR("JDD_LOG: COMM CB. EventType: E_PUSH_INDICATION",0x09100ffe)) ;
        AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d \n", __func__, __LINE__);
        rCode = AT_HandlePushMsg((ST_COMM_PUSH_IND *)pvIndication);
        break;
    }
    case E_SECURITY_INDICATION:         /**< Specifies details of the security that is negotiated by the lower layer (Argument #ST_COMM_SECURITY_IND). */
    case E_SECURITY_FAILURE_INDICATION: /**< Specifies details of security failure (if any) (Argument #ST_COMM_SECURITY_FAILURE_IND) . */
    case E_CERT_RESP_INDICATION:        /**< Specifies the response of the certificate request which was previously sent by the application (Arguement #ST_COMM_CERT_RESP_IND) */
    case E_COMM_MAX_TRANSPORT_REQUESTS: /**< Specifies the maximum number of requests that can be handled in transport at a time. (Argument passed #JC_INT32*) */
    default:
        //mmi_trace_b (_MMI|TLEVEL(11)|TDB|TNB_ARG(0), TSTR("JDD_LOG: COMM CB. EventType: E_SECURITY_INDICATION",0x09100fff)) ;
        break;
    }
    return rCode;
}

void jMms_comm_init()
{
    INT iErr;
    JC_TASK_INFO stTaskInfo;
    JC_TASK_INFO stTrTaskInfo;
    ST_COMM_PROFILE stCommProf;
    ST_GENERIC_DATA_CONN stDataConn;

    jc_memset(&stTaskInfo, 0, sizeof(JC_TASK_INFO));
    jc_memset(&stTrTaskInfo, 0, sizeof(JC_TASK_INFO));
    jc_memset(&stCommProf, 0, sizeof(ST_COMM_PROFILE));
    jc_memset(&stDataConn, 0, sizeof(ST_GENERIC_DATA_CONN));

    stTaskInfo.iTaskID = (JC_INT32)E_TASK_MMS_CORE;
    stTaskInfo.iAppID = (JC_INT32)E_TASK_MMS_UI;
    stTrTaskInfo.iTaskID = (JC_INT32)E_TASK_TRANSPORT;
    stTrTaskInfo.iAppID = (JC_INT32)E_TASK_TRANSPORT;
    AT_TC(g_sw_AT_SMS, "AT_WY jMms_comm_init");

    iErr = jdi_CommunicationInitialize(&stTaskInfo, &stTrTaskInfo, JC_NULL, jMMS_CommCB,
                                       JC_NULL, &hCommunicator);
}

#ifdef __MMI_MULTI_SIM__
JC_RETCODE MMS_SendPushMsgToTransport(INT8 *pcBuffer, INT32 ulBuffrLen, UINT8 nSimID)
#else
JC_RETCODE MMS_SendPushMsgToTransport(INT8 *pcBuffer, INT32 ulBuffrLen)
#endif
{
    JC_RETCODE retCode = JC_ERR_UNKNOWN;
    JC_EVENT_DATA stEventData;
    JC_UINT32 temp = 0;

#ifdef PUSH_WRITE_TO_FILE
    dumpPushBufferToFile(pcBuffer, ulBuffrLen);
#endif
    //jMms_comm_init();
    AT_TC(g_sw_AT_SMS, "AT_WY MMS_SendPushMsgToTransport");

    stEventData.uiEventInfoSize = sizeof(ST_TR_PUSH_SMS_CONTENT) + ulBuffrLen + 1;
    stEventData.destTaskInfo.iAppID = (JC_INT32)E_TASK_TRANSPORT;
    stEventData.destTaskInfo.iTaskID = (JC_INT32)E_TASK_TRANSPORT;
    stEventData.srcTaskInfo.iAppID = (JC_INT32)E_TASK_TRANSPORT;
    stEventData.srcTaskInfo.iTaskID = (JC_INT32)E_TASK_TRANSPORT;
    stEventData.iEventType = E_TR_SMS_PUSH_MSG;
    retCode = jdd_QueueAlloc(&stEventData);
    hal_HstSendEvent(0x555550 | retCode);
    if (retCode != JC_OK)
    {
        retCode = JC_ERR_MEMORY_ALLOCATION;
    }
    else
    {
        ST_TR_PUSH_SMS_CONTENT *pstPushContent;

        pstPushContent = (ST_TR_PUSH_SMS_CONTENT *)stEventData.pEventInfo;
        jc_memset(pstPushContent, 0x00, stEventData.uiEventInfoSize);
        pstPushContent->iPushLen = ulBuffrLen;

#ifdef __MMI_MULTI_SIM__
        jc_sprintf(pstPushContent->acSmeAddr, "%d", nSimID);
//mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("JDD_LOG: MMS_SendPushMsgToTransport dual sim nSimID[%s] ",0x09101031),pstPushContent->acSmeAddr) ;
#endif

        jc_memcpy(&pstPushContent->ucPushMsg, pcBuffer, ulBuffrLen);
        for (temp = 0; temp < ulBuffrLen; temp++)
        {
            //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(3),TSTR("JDD_LOG: buffer value is [%x-%c-%d]",0x09101032), pstPushContent->ucPushMsg[temp],
            //pstPushContent->ucPushMsg[temp], temp);
        }
#ifdef __MMI_WLAN_FEATURES__
        gmms_transfer = 1;
#endif
        hal_HstSendEvent(0x44444440);

        retCode = jdd_QueueSend(&stEventData);
    }
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JDD_LOG: MMS_SendPushMsgToTransport dual sim End",0x09101033)) ;
    return retCode;
}

/******************************************************************************************
Function			:	AT_SMS_NewPushMessage
Description		:   	AT receive indication of overflow from csw
Called By			:	At SMS module
Data Accessed   	:
Data Updated    	:
Input			:   	COS_EVENT *pEvent
Output			:
Return			:	VOID
Others			:
					modify and add comment by wangyue 01/10/2017
*******************************************************************************************/
VOID AT_SMS_NewPushMessage(COS_EVENT *pCosEvent)
{

    UINT32 nParam1;
    UINT32 nParam2;
    UINT32 nParam3;
    UINT8 nType;
    UINT16 nUTI;

    ASSERT(NULL != pCosEvent);

    nParam1 = pCosEvent->nParam1;
    nParam2 = pCosEvent->nParam2;
    nParam3 = pCosEvent->nParam3;
    nType = HIUINT8(pCosEvent->nParam3);
    nUTI = HIUINT16(pCosEvent->nParam3);

    AT_TC(g_sw_AT_SMS, "AT_WY AT_SMS_NewPushMessage");
    CFW_NEW_SMS_NODE *pSmsNode = (CFW_NEW_SMS_NODE *)nParam1;
    CFW_SMS_TXT_HRD_V1_IND *pTxtHrdNode = pSmsNode->pNode;

    //UINT8 nType = HIUINT8(nParam3);
    UINT8 nSimID = LOUINT8(nParam3); //nSimID is for jataayu

    CFW_EMS_INFO *pstEmsInfo = (CFW_EMS_INFO *)nParam2;
    S8 *ucPushBuff;
    INT iPushLen;
    UINT32 nRet;

    /*	CFW_EVENT CfwEvent   = { 0 };
	UINT8 nType;
    UINT8 nSimID;


	CFW_EMS_INFO *pstEmsInfo = (CFW_EMS_INFO *)CfwEvent.nParam2 ;
	S8			 *ucPushBuff ;
	INT			 iPushLen ;
	UINT32       nRet;

	 get event
	AT_CosEvent2CfwEvent(pEvent, &CfwEvent);
	 just for debug

	CFW_NEW_SMS_NODE *pSmsNode = (CFW_NEW_SMS_NODE*)CfwEvent.nParam1;
	CFW_SMS_TXT_HRD_V1_IND *pTxtHrdNode = pSmsNode->pNode ;

	nType = CfwEvent.nType;
    nSimID = CfwEvent.nFlag; */

    if (0 == nType)
    {

        switch (pSmsNode->nType)
        {
        /*Jataayu Push*/
        case 0x21: // 0x04:
        case 0x22: //0x05:

            iPushLen = 12 + pTxtHrdNode->nDataLen;
            ucPushBuff = COS_MALLOC(iPushLen);
            ucPushBuff[0] = 0x0B; //Header Length
            ucPushBuff[1] = 0x05; //UDH IE Identifier
            ucPushBuff[2] = 0x04; //UDH IE Identifier length

            ucPushBuff[3] = (S8)(pstEmsInfo->nDesPortNum);
            ucPushBuff[4] = (S8)(pstEmsInfo->nDesPortNum >> 8);
            ucPushBuff[5] = (S8)(pstEmsInfo->nOriPortNum);
            ucPushBuff[6] = (S8)(pstEmsInfo->nOriPortNum >> 8);

            ucPushBuff[7] = 0x00;                     //UDH IE Identifier SAR
            ucPushBuff[8] = 0x03;                     //UDH IE Identifier SAR Length
            ucPushBuff[9] = pstEmsInfo->nRerNum;      //Refrence Number
            ucPushBuff[10] = pstEmsInfo->nMaxNum;     //Total segment count
            ucPushBuff[11] = pstEmsInfo->nCurrentNum; //Current segment count

            if (ucPushBuff[10] == 0 && ucPushBuff[11] == 0)
            {
                ucPushBuff[10] = 1;
                ucPushBuff[11] = 1;
            }

            if (pTxtHrdNode->pData)
            {
                SUL_MemCopy8((void *)(ucPushBuff + 12), pTxtHrdNode->pData,
                             pTxtHrdNode->nDataLen);
            }
#ifdef ENABLE_PAGESPY
            COS_PageProtectSetup(5, 2, (UINT32)ucPushBuff - 16, (UINT32)ucPushBuff + ((((iPushLen + 1 + (4 - (iPushLen & 3)) + 4) >> 2)) << 2));
            COS_PageProtectEnable(5);
#endif

#ifdef __MMI_MULTI_SIM__
            MMS_SendPushMsgToTransport((S8 *)ucPushBuff, iPushLen, nSimID);
#else
            MMS_SendPushMsgToTransport((S8 *)ucPushBuff, iPushLen);
#endif

            //AT_HandlePushMsg((ST_COMM_PUSH_IND*) ucPushBuff);
            //jdi_WAPGSMParseMessage(NULL, NULL, nSimID, (S8*)ucPushBuff, iPushLen);
            nRet = CFW_SmsDeleteMessage(pSmsNode->nConcatCurrentIndex, CFW_SMS_STORED_STATUS_STORED_ALL, pSmsNode->nStorage, CFW_SMS_TYPE_PP, FREE_UTI, nSimID);
#ifdef ENABLE_PAGESPY
            COS_PageProtectDisable(5);
#endif
            COS_FREE(ucPushBuff);
            //DealPushMsgTxtHRDV1(pSmsNode,seg);//seg is port number
            break;
        default:
            break;
        }

        //COS_FREE((PVOID)CfwEvent.nParam2);
        COS_FREE((PVOID)nParam2);
    }

    //return TRUE ;
}

JC_RETCODE AT_HandlePushMsg(ST_COMM_PUSH_IND *pstPushData)
{
    JC_RETCODE rCode = TRUE;
    //JC_UINT32 			uiSimId = 0 ; // deafult is SIM1 for single SIM
    int i = 0;
    AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d \n", __func__, __LINE__);
#if 0
    for(i=0;i<=150;i++)
    {
    	AT_TC(g_sw_AT_SMS, "%x", pstPushData[i]);
        i++;
    }
#endif
    if (pstPushData != NULL && pstPushData->pcContentType != NULL &&
        pstPushData->pucBuffer != NULL)
    {
        if (strstr(pstPushData->pcContentType, CS_OTA_CONTENT_TYPE1) ||
            strstr(pstPushData->pcContentType, CS_OTA_CONTENT_TYPE2))
        {

            prv_AddToProvList(pstPushData);
            prv_ProcessProvInfo();

            //jprv_MMIDisplayNewProvAlertScreen();
        }
        /* pstPushData data to be freed */
    }
    return rCode;
}

ST_PROV_CONTEXT gstProvContext = {
    JC_NULL,
};

JC_HANDLE hCommunicator; //hComm ;

/* ===========================================================================
 *  PUBLIC FUNCTIONS
 * ===========================================================================
 */

void prv_AddToProvList(ST_COMM_PUSH_IND *pstProvPushData)
{
    ST_PROV_INFO *pstNewNode = JC_NULL;
    ST_PROV_INFO *pstTemp = JC_NULL;

    //mmi_trace_b(_MMI|TLEVEL(12)|TDB|TNB_ARG(0),TSTR("JPRV_LOG:prv_AddToProvList called",0x09101144));

    pstNewNode = (ST_PROV_INFO *)jdd_MemAlloc(1, sizeof(ST_PROV_INFO));

    AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d \n", __func__, __LINE__);
    if (!pstNewNode)
        return;

    pstNewNode->pstNext = JC_NULL;
    pstNewNode->bIsHTTPHeader = E_FALSE;
#ifdef __MMI_MULTI_SIM__
    pstNewNode->nSimID = jc_atoi(pstProvPushData->pcPPGHostAddress);
#else
    pstNewNode->nSimID = 0;
#endif
    pstNewNode->pucBuffer = pstProvPushData->pucBuffer;

    pstNewNode->uiBufferLen = pstProvPushData->uiLength;
    if (pstProvPushData->uiLength > 0)
    {
        pstNewNode->pucBuffer = jdd_MemAlloc(1, pstProvPushData->uiLength);
        if (pstNewNode->pucBuffer)
            jc_memcpy(pstNewNode->pucBuffer, pstProvPushData->pucBuffer, pstProvPushData->uiLength);
    }

    pstNewNode->uiHeaderLen = pstProvPushData->uiHeaderLength;
    if (pstProvPushData->uiHeaderLength > 0)
    {
        pstNewNode->pucHeader = jdd_MemAlloc(1, pstProvPushData->uiHeaderLength);
        if (pstNewNode->pucHeader)
            jc_memcpy(pstNewNode->pucHeader, pstProvPushData->pucHeader, pstProvPushData->uiHeaderLength);
    }

    if (gstProvContext.pstProvInfo == JC_NULL)
    {
        //mmi_trace_b(_MMI|TLEVEL(12)|TDB|TNB_ARG(0),TSTR("JPRV_LOG:added as root node",0x09101145));
        gstProvContext.pstProvInfo = pstNewNode;
    }
    else
    {
        pstTemp = gstProvContext.pstProvInfo;
        while (pstTemp->pstNext != JC_NULL)
            pstTemp = pstTemp->pstNext;
        //mmi_trace_b(_MMI|TLEVEL(12)|TDB|TNB_ARG(0),TSTR("JPRV_LOG:added to the link list",0x09101146));
        pstTemp->pstNext = pstNewNode;
    }
}

/**
 * @ingroup app_OTA
 * @brief   Processes the bootstrap File
 */
void prv_ProcessProvInfo()
{
    JC_RETCODE rCode = JC_OK;
    JC_HANDLE vDecHandle = JC_NULL;
    ST_MIME_HEADERS *pstReqHeader = JC_NULL;
    U_PROV_CALLBACK uProvCallBack = {
        JC_NULL,
    };
    ST_PROV_INFO *pstProvInfo = gstProvContext.pstProvInfo;
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0), TSTR("JPRV_LOG:prv_ProcessDataFromFile invoked",0x09101149));
    //stProvInfo has provision info
    AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d \n", __func__, __LINE__);
    if (pstProvInfo->pucBuffer && pstProvInfo->pucHeader)
    {
        if (E_FALSE == pstProvInfo->bIsHTTPHeader)
        {
            rCode = jdi_WSPDecInitialize(pstProvInfo->pucHeader, pstProvInfo->uiHeaderLen,
                                         E_FALSE, JC_NULL, &vDecHandle);
            //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1), TSTR("JPRV_LOG:jdi_WSPDecInitialize returns %d",0x0910114a), rCode);
            AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d rCode = %d \n", __func__, __LINE__, rCode);
            if (JC_OK == rCode)
            {
                rCode = jdi_WSPDecAllHeaders(vDecHandle, E_TRUE, &pstReqHeader);
                //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1), TSTR("JPRV_LOG:jdi_WSPDecAllHeaders returns %d",0x0910114b), rCode);
                AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d rCode = %d \n", __func__, __LINE__, rCode);
            }
        }
        else
        {
            //rCode = jdi_HTTPDecInitialize (pstProvInfo->pucHeader, pstProvInfo->uiHeaderLen,
            //E_FALSE, JC_NULL, &vDecHandle) ;
            //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1), TSTR("JPRV_LOG:jdi_HTTPDecInitialize returns %d",0x0910114c), rCode);
            if (JC_OK == rCode)
            {
                //rCode = jdi_HTTPDecAllHeaders (vDecHandle, E_TRUE, &pstReqHeader) ;
                //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1), TSTR("JPRV_LOG:jdi_HTTPDecAllHeaders returns %d",0x0910114d), rCode);
            }
        }
        if (JC_OK == rCode && JC_NULL == gstProvContext.vProvHandle)
        {
            rCode = jdi_ProvisionInitialize(&(gstProvContext.vProvHandle));
            //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1), TSTR("JPRV_LOG:jdi_ProvisionInitialize returns %d",0x0910114e), rCode);
            AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d rCode = %d \n", __func__, __LINE__, rCode);
        }
        if (JC_OK == rCode)
        {
            rCode = jdi_ProvisionProcess(gstProvContext.vProvHandle,
                                         pstProvInfo->pucBuffer, pstProvInfo->uiBufferLen, pstReqHeader, &(gstProvContext.eProvType),
                                         &(gstProvContext.pcProvURL), &(gstProvContext.eSecType));
            AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d rCode = %d sectype[%d] provtype[%d] \n", __func__, __LINE__, rCode, gstProvContext.eSecType, gstProvContext.eProvType);
            if (JC_OK == rCode && (E_USERPIN == gstProvContext.eSecType || E_NETWPIN == gstProvContext.eSecType))
            {
                uProvCallBack.pfHmacSha = (PROV_CALLBACK_HMAC)jdi_HMACSHA1DigestHex;
                rCode = jdi_ProvisionRegisterCallback(gstProvContext.vProvHandle,
                                                      E_AUTHENTICATE_HMAC_SHA_CALLBACK, &uProvCallBack);
                //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1), TSTR("JPRV_LOG:jdi_ProvisionRegisterCallback returns %d",0x09101150), rCode);
            }
        }
        if (JC_NULL != vDecHandle)
        {
            if (E_FALSE == pstProvInfo->bIsHTTPHeader)
            {
                AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d \n", __func__, __LINE__);
                jdi_WSPDecDeInitialize(vDecHandle);
            }
            else
            {
                //jdi_HTTPDecDeInitialize (vDecHandle) ;
            }
        }
    }
    else
    {
        rCode = JC_ERR_NULL_POINTER;
    }

    if (JC_OK == rCode)
    {
        hal_HstSendEvent(0x33333336);

        prv_DeleteProfList();

        if (E_NETWPIN == gstProvContext.eSecType)
        {
            UINT8 pIMSI[32] = {0}, pIMSIasc[32] = {
                                       0,
                                   };
            UINT32 ret;
            UINT8 OutLen = 0;
            //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0), TSTR("JPRV_LOG:network pin enabled ",0x09101151));

            ret = CFW_CfgGetIMSI(pIMSI, pstProvInfo->nSimID);

            cfw_IMSItoASC(pIMSI, pIMSIasc, &OutLen);

            //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(2)|TSMAP(1),TSTR("JPRV_LOG: IMSI for sim0 %s %d",0x09101152), pIMSIasc, OutLen);

            jc_memset(pIMSI, 0, 32);
            OutLen = 0;

            ConvertIMSIToSemiOctet(pIMSIasc, pIMSI, &OutLen);
            if (OutLen > 0)
            {
                //jdi_CUtilsCharToTchar(JC_NULL, pIMSIasc, &gstProvContext.pmPinValue);
                //prv_CheckAuthentication();
                rCode = prv_CheckNETPINAuthentication(pIMSI, OutLen);
            }
            else // considering as "no security defined"
            {
                AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d \n", __func__, __LINE__);
                prv_FormProfList();
            }
        }
        else if (E_USERPIN == gstProvContext.eSecType)
        {
            //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0), TSTR("JPRV_LOG:enter user pin screen ",0x09101153));
            jdd_AssertOnFail((JC_NULL == gstProvContext.pmPinValue), "JPRV_LOG:inavlid pin");
            gstProvContext.pmPinValue = (JC_CHAR *)jdd_MemAlloc(
                sizeof(JC_CHAR), WAP_MAX_PROFILE_PASSWD_LEN);
            if (gstProvContext.pmPinValue)
            {
                //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG:gstProvContext.pmPinValue %d",0x09101154),gstProvContext.pmPinValue );
                gstProvContext.ucNumOfAttempts = 0;
                //prv_MMIDisplayEnterPINScreen();
            }
            else
            {
                rCode = JC_ERR_MEMORY_ALLOCATION;
            }
        }
        else // no security defined
        {
            prv_FormProfList();
        }
    }
    if (JC_OK != rCode)
    {
        prv_DeInitContext();
        //prv_MMIDisplayProvFailScreen();
    }
}

void prv_DeleteProfList()
{
    ST_PROV_INFO_LIST *pstProfList = JC_NULL;
    while (JC_NULL != gstProvContext.pstProfList)
    {
        pstProfList = gstProvContext.pstProfList;
        gstProvContext.pstProfList = gstProvContext.pstProfList->pstNext;
        jdd_MemFree(pstProfList);
    }
}

void ConvertIMSIToSemiOctet(const JC_UINT8 *pucIMISI, JC_UINT8 *pucIMSISemiOctet, JC_UINT8 *pucLength)
{
    JC_UINT8 ucIMISIndex = 0, ucSOIndex = 0, ucLength = 0;

    ucLength = jc_strlen((const char *)pucIMISI);
    pucIMSISemiOctet[ucSOIndex] = (ucLength % 2) ? 0x09 : 0x01;
    pucIMSISemiOctet[ucSOIndex++] |= (pucIMISI[ucIMISIndex++] - '0') << 4;
    while (ucIMISIndex < ucLength)
    {
        pucIMSISemiOctet[ucSOIndex] = (pucIMISI[ucIMISIndex++] - '0');
        if (ucIMISIndex < ucLength)
        {
            pucIMSISemiOctet[ucSOIndex++] |= (pucIMISI[ucIMISIndex++] - '0') << 4;
        }
        else
        {
            pucIMSISemiOctet[ucSOIndex++] |= 0xF0;
        }
    }
    *pucLength = ucSOIndex;
    while (ucSOIndex < 8)
    {
        pucIMSISemiOctet[ucSOIndex++] = 0xFF;
    }
}

JC_RETCODE prv_CheckNETPINAuthentication(UINT8 *pIMSIasc, UINT8 OutLen)
{
    JC_RETCODE rCode = JC_ERR_GENERAL_FAILURE;
    ST_PROVISION_INFO *pstProvInfo = NULL;

    pstProvInfo = (ST_PROVISION_INFO *)gstProvContext.vProvHandle;

    if (pstProvInfo->pcMACBuffer == NULL)
        return rCode;

    rCode = jdi_ProvisionAuthenticate(gstProvContext.vProvHandle,
                                      pIMSIasc, OutLen);

    if (rCode == JC_OK)
    {
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG:authentication success",0x09101155));
        prv_FormProfList();
    }
    else
    {
        //mmi_trace(1,"JPRV_LOG:authentication faild 0x%x", rCode);
    }
    return rCode;
}

void prv_FormProfList()
{
    JC_RETCODE rCode;
    rCode = prv_HandleContinuousAndBootstrapProvisioning();
    AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d rCode = %d \n", __func__, __LINE__, rCode);
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG:prv_HandleContinuousAndBootstrapProvisioning returns %d",0x09101156), rCode);

    if (JC_OK != rCode)
    {
        prv_DeInitContext();
        //prv_MMIDisplayProvFailScreen();
    }
    else
    {
        if (E_BOOTSTRAP_FLAG == gstProvContext.eProvType)
        {
            AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d rCode = %d \n", __func__, __LINE__, rCode);
            prv_FormCSProfiles();
            gstProvContext.pstCurrCsProfInfo = gstProvContext.pstCsProfInfo;
            //prv_GetNextNewCSProfile();
            //prv_MMIDisplayNewSettingsSummaryScreen();
        }
        else
        {
            //prv_MMIDisplayContProvProfDetails();
            AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d rCode = %d \n", __func__, __LINE__, rCode);
            prv_HandleDownloadContinousProvData();
        }
    }
}

void prv_HandleDownloadContinousProvData()
{
    prv_DownloadContinousProvData();
    //prv_DisplayDownloadScreen();
}

/*****************************************************************************
*
* @fn           : prv_DownloadContinousProvData
*
* @brief        : This function downloads 'continuous' provisioning data
*
* @param[in]    : pProfile  Profile information received
*
* @retval       : JC_OK On Successful and others on failure specific error code.
*
******************************************************************************/
void prv_DownloadContinousProvData()
{
    ST_COMM_FETCH_URL stFetchUrl = {
        JC_NULL,
    };
    JC_RETCODE rCode = JC_OK;
    JC_TASK_INFO stAppTaskInfo;
    JC_TASK_INFO stTransportTaskInfo;
    ST_COMM_PROFILE stCommProfile = {
        E_COMM_WSP_CONNECTION_ORIENTED,
    };
    ST_GENERIC_DATA_CONN stCommDataConn = {
        E_DATA_CONN_CSD_BEARER_TYPE,
    };
    ST_PROV_INFO *pstProvInfo = gstProvContext.pstProvInfo;

    ST_PROV_INFO_LIST *pstProfile = gstProvContext.pstContProfList;
    memset(&stFetchUrl, 0, sizeof(ST_COMM_FETCH_URL));

    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: prv_DownloadContinousProvData inovoked",0x0910118c));
    //pstProvInfo = prv_GetProvInfo (0, E_TRUE) ;
    if (JC_NULL == pstProfile || JC_NULL == pstProvInfo)
    {
        //jdd_AssertOnFail(0,"Invalid State");
        // mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0), TSTR("JC_NULL == pstProfile || JC_NULL == pstProvInfo",0x0910118d));
    }
    else
    {
        //stCommDataConn.uiProfileID = pstProfile->stProfile.profile_id;
        stAppTaskInfo.iTaskID = (JC_INT32)E_TASK_WAP_PROVISIONING_CORE;
        stAppTaskInfo.iAppID = (JC_INT32)E_TASK_WAP_PROVISIONING_UI;
        stTransportTaskInfo.iTaskID = (JC_INT32)E_TASK_TRANSPORT;
        stTransportTaskInfo.iAppID = (JC_INT32)E_TASK_TRANSPORT;
        prv_DeInitCommunicator();
        rCode = jdi_CommunicationInitialize(&stAppTaskInfo, &stTransportTaskInfo, &gstProvContext,
                                            prv_CBCommProvIndication, JC_NULL, &(gstProvContext.vCommHandle));
        prv_updateMissingProfileInfo(&pstProfile->stProfile);
        if (0 != pstProfile->stProfile.port)
        {
            stCommProfile.uiPortNumber = pstProfile->stProfile.port;
        }
        else
        {
            if (E_DC_CONNECTION_ORIENTED == pstProfile->stProfile.conn_mode)
            {
                stCommProfile.uiPortNumber = PRV_PORT_9201;
            }
            else if (E_DC_CONNECTION_ORIENTED_SECURE == pstProfile->stProfile.conn_mode)
            {
                stCommProfile.uiPortNumber = PRV_PORT_9203;
            }
        }

        if ('\0' != *pstProfile->stProfile.proxy_ip)
        {
            jc_strcpy(stCommProfile.acHostName, pstProfile->stProfile.proxy_ip);
        }

        /* Fill Data Conn Info */
        stCommDataConn.eBearerType = E_DATA_CONN_GPRS_BEARER_TYPE;

        jc_strncpy(stCommDataConn.uDataConnType.stGPRSDataConn.acAPN, pstProfile->stProfile.apn, MAX_DATA_CONN_USERNAME_SIZE);
        //mmi_trace_b (_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("JPRV_LOG:settings: Apn name :[%s]",0x0910118e),stCommDataConn.uDataConnType.stGPRSDataConn.acAPN);
        jc_strncpy(stCommDataConn.acUserName, pstProfile->stProfile.username, MAX_DATA_CONN_USERNAME_SIZE);
        jc_strncpy(stCommDataConn.acPassword, pstProfile->stProfile.password, MAX_DATA_CONN_PASSWORD_SIZE);

        if (E_DC_CONN_MODE_MAX != pstProfile->stProfile.conn_mode)
        {
            switch (pstProfile->stProfile.conn_mode)
            {
            case E_DC_CONNECTION_ORIENTED:
                stCommProfile.eConnMode = E_COMM_WSP_CONNECTION_ORIENTED;
                break;
            case E_DC_CONNECTION_ORIENTED_SECURE:
                stCommProfile.eConnMode = E_COMM_WSP_CONNECTION_ORIENTED_SECURE;
                break;
            case E_DC_WPHTTP_PROXY:
                stCommProfile.eConnMode = E_COMM_WPHTTP_PROXY;
                break;
            case E_DC_WPHTTP_DIRECT:
                stCommProfile.eConnMode = E_COMM_WPHTTP_DIRECT;
                break;
            default:
                stCommProfile.eConnMode = E_COMM_WSP_CONNECTION_ORIENTED;
                break;
            }
        }
        else
        {
            switch (pstProfile->stProfile.port)
            {
            case PRV_PORT_9200:
            case PRV_PORT_9201:
                stCommProfile.eConnMode = E_COMM_WSP_CONNECTION_ORIENTED;
                break;

            case PRV_PORT_443:
            case PRV_PORT_9202:
            case PRV_PORT_9203:
                stCommProfile.eConnMode = E_COMM_WSP_CONNECTION_ORIENTED_SECURE;
                break;

            case PRV_PORT_80:
            case PRV_PORT_9401:
            case PRV_PORT_3128:
            case PRV_PORT_8080:
            case PRV_PORT_8008:
                stCommProfile.eConnMode = E_COMM_WPHTTP_PROXY;
                break;

            default:
                if (0 == jc_strlen(stCommProfile.acHostName))
                {
                    stCommProfile.eConnMode = E_COMM_WPHTTP_DIRECT;
                }
                else
                {
                    stCommProfile.eConnMode = E_COMM_WPHTTP_PROXY;
                }
                break;
            }
        }
//mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(3)|TSMAP(4),TSTR("JPRV_LOG: eConnMode  %d port %d host %s",0x0910118f),stCommProfile.eConnMode,stCommProfile.uiPortNumber,stCommProfile.acHostName);

// for testing
//pstProvInfo->nSimID = 0;
#ifdef __MMI_MULTI_SIM__
        stCommDataConn.uiConnID = WAP_MMS_UICONNID_SIM1_FLAG << pstProvInfo->nSimID;
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG: nSimID %d",0x09101190),stCommDataConn.uiConnID);
        stCommDataConn.uiConnID |= WAP_MMS_UICONNID_PROV_FLAG;
#else
        stCommDataConn.uiConnID = 3; /* 3 for provisioning */
#endif

        // hard coding for testing . remove later
        //stCommProfile.eConnMode = E_COMM_WPHTTP_DIRECT ;
        rCode = jdi_CommunicationProfile(gstProvContext.vCommHandle, &stCommProfile);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG: jdi_CommunicationProfile returns %d",0x09101191),rCode);
        if (JC_OK == rCode)
        {
            rCode = jdi_CommunicationDataConnection(gstProvContext.vCommHandle, &stCommDataConn);
            //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG: jdi_CommunicationDataConnection returns %d",0x09101192),rCode);
        }
    }

    if (JC_OK == rCode)
    {
        stFetchUrl.eCommFetchType = E_COMM_FETCH_GET;
        stFetchUrl.eCommRequestType = E_COMM_MAIN_REQUEST;
        rCode = jdi_CUtilsCharToTchar(JC_NULL, gstProvContext.pcProvURL, &(stFetchUrl.pmURL));
        if (JC_OK == rCode)
        {
            //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: jdi_CommunicationFetchURL inovoked",0x09101193));
            if (gstProvContext.vCommHandle)
            {
                rCode = jdi_CommunicationFetchURL(gstProvContext.vCommHandle, &stFetchUrl,
                                                  &(pstProvInfo->uiRequestID));
            }
            else
            {
                //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0), TSTR("gstProvContext.vCommHandle is NULL",0x09101194));
            }
            //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG: jdi_CommunicationFetchURL returns %d",0x09101195),rCode);
            jdd_MemFree(stFetchUrl.pmURL);
        }
    }
    if (JC_OK != rCode)
    {
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1), TSTR("JPRV_LOG:error occured[%d]",0x09101196), rCode);
    }
    //return rCode ;
}

/*****************************************************************************
*
* @fn           : prv_CBCommProvIndication
*
* @brief        : Handles the events from the communicator.
*
* @param[in]    : pvAppArg  The callback argument.
* @param[in]    : eCommIndicationType   Event type.
* @param[in]    : pvIndication Event specific data.
*
* @retval       : JC_OK On Successful and others on failure specific error code.
*
******************************************************************************/
JC_RETCODE prv_CBCommProvIndication(
    void *pvAppArg,
    E_COMM_INDICATION eCommIndicationType,
    void *pvIndication)
{
    JC_RETCODE rCode = JC_OK;

    //mmi_trace_b(_MMI|TLEVEL(12)|TDB|TNB_ARG(1), TSTR("JPRV_LOG: prv_CBCommProvIndication called with %d ",0x0910119e), eCommIndicationType);
    switch (eCommIndicationType)
    {
    case E_HEADER_INDICATION:
        rCode = prv_HeaderIndication((ST_COMM_HEADER_IND *)pvIndication);
        // mmi_trace_b (_MMI|TSTDOUT|TDB|TNB_ARG(1),TSTR("JPRV_LOG: prv_HeaderIndication () returned rCode as [%x]",0x0910119f), -rCode) ;
        break;
    case E_FETCH_INDICATION:
        //rCode = prv_FetchIndication ((ST_COMM_FETCH_IND *)pvIndication) ;
        // mmi_trace_b (_MMI|TSTDOUT|TDB|TNB_ARG(1), TSTR("JPRV_LOG: prv_FetchIndication () returned rCode as [%x]",0x091011a0), -rCode) ;
        break;
    case E_ERROR_INDICATION:
    {
        ST_COMM_ERR_IND *pstErroInd = (ST_COMM_ERR_IND *)pvIndication;
        //mmi_trace_b (_MMI|TSTDOUT|TDB|TNB_ARG(0), TSTR("JPRV_LOG: E_ERROR_INDICATION recvd",0x091011a1)) ;
        if (JC_NULL != pstErroInd)
        {
            //pstErroInd->rCode
            prv_ResetContinousProvdata();
            //DeleteUptoScrID(SCR_ID_PROV_CONT_DEATILS_SCREEN);
            //GoBackHistory();
            //prv_MMIDisplayNetErrorPopUp();
        }
    }
    break;
    case E_PROGRESS_INDICATION:
        break;
    case E_STOP_INDICATION:
    {
        //mmi_trace_b (_MMI|TSTDOUT|TDB|TNB_ARG(1), TSTR("JPRV_LOG: E_STOP_INDICATION recvd, %d",0x091011a2),gstProvContext.bEndKeyPressed) ;
        prv_ResetContinousProvdata();
        //U8 val = DeleteUptoScrID(SCR_ID_PROV_CONT_DEATILS_SCREEN);
        if (gstProvContext.bEndKeyPressed == E_TRUE)
        {
            gstProvContext.bEndKeyPressed = E_FALSE;
            //DeleteUptoScrID(IDLE_SCREEN_ID);
            //prv_MMIDisplayIDLEScreen();
        }
        else
        {
            //DeleteUptoScrID(SCR_ID_PROV_CONT_DETAILS_SCREEN);
            //GoBackHistory();
        }
    }
    break;
    case E_STOP_ALL_INDICATION:
        break;
    case E_PROFILE_CONFIRMATION_INDICATION:
        break;
    case E_USER_DATA_INDICATION:
        break;
    case E_PUSH_INDICATION:
        break;
    case E_SECURITY_INDICATION:
        break;
    case E_SECURITY_FAILURE_INDICATION:
        break;
    case E_CERT_RESP_INDICATION:
        break;
    default:
        break;
    }
    return JC_OK;
}

/*****************************************************************************
*
* @fn           : prv_HeaderIndication
*
* @brief        : Function which handles the prov header indication
*
* @param[in]    : pstHeaderIndication Pointer to header indication
*
* @retval       : JC_OK On Successful and others on failure specific error code.
*
******************************************************************************/
JC_RETCODE prv_HeaderIndication(ST_COMM_HEADER_IND *pstHeaderIndication)
{
    JC_RETCODE rCode = JC_OK;
    JC_BOOLEAN bIsHHTPHeader = E_FALSE;
    //mmi_trace_b(_MMI|TLEVEL(12)|TDB|TNB_ARG(1),TSTR("JPRV_LOG: prv_HeaderIndication recvd %d",0x091011a3),pstHeaderIndication->uiContentLength);
    if (JC_NULL == pstHeaderIndication || JC_NULL == pstHeaderIndication->pcContentType ||
        JC_NULL == pstHeaderIndication->pucHeader || 0 >= pstHeaderIndication->uiHeaderLength)
    {
        rCode = JC_ERR_INVALID_PARAMETER;
    }
    else
    {
        if (E_FALSE == pstHeaderIndication->bIsBinary)
        {
            bIsHHTPHeader = E_TRUE;
        }
        rCode = prv_AppendHeader(pstHeaderIndication->pucHeader, pstHeaderIndication->uiHeaderLength,
                                 bIsHHTPHeader, pstHeaderIndication->uiRequestID);
    }
    return rCode;
}

/*****************************************************************************
*
* @fn           : prv_AppendHeader
*
* @brief        : This function appends the header of the provisioning content.
*
* @param[in]    : pHeader   Header buffer.
*
* @param[in]    : uiHeaderLen   Header buffer length.
*
* @param[in]    : bIsHTTPHeader Specify HTTP header or WSP header.
*
* @param[in]    : uiRequestID   Unique request ID.
*
* @retval       : JC_OK On Success, On failure specific error code.
*
******************************************************************************/
JC_RETCODE prv_AppendHeader(const JC_UINT8 *pucHeader, const JC_UINT32 uiHeaderLen,
                            const JC_BOOLEAN bIsHTTPHeader,
                            const JC_UINT32 uiRequestID)
{
    JC_RETCODE rCode = JC_OK;
    JC_UINT8 *pucTempHeader = JC_NULL;
    JC_UINT32 uiNewHeaderLen = 0;
    ST_PROV_INFO *pstProvInfo = gstProvContext.pstProvInfo;

    //pstProvInfo = prv_GetProvInfo (uiRequestID, E_TRUE) ;

    if (JC_NULL != pstProvInfo)
    {
        if (0 != pstProvInfo->uiBufferLen)
        {
            jdd_MemFree(pstProvInfo->pucBuffer);
            pstProvInfo->uiBufferLen = 0;
        }
        if (0 != pstProvInfo->uiHeaderLen)
        {
            jdd_MemFree(pstProvInfo->pucHeader);
            pstProvInfo->uiHeaderLen = 0;
        }
        if (0 == uiHeaderLen)
        {
            rCode = JC_ERR_INVALID_PARAMETER;
        }
        if (JC_OK == rCode)
        {
            uiNewHeaderLen = pstProvInfo->uiHeaderLen + uiHeaderLen;
            pucTempHeader = (JC_UINT8 *)jdd_MemAlloc(sizeof(JC_UINT8), uiNewHeaderLen + 1);
            if (JC_NULL != pucTempHeader)
            {
                if (0 != pstProvInfo->uiHeaderLen)
                {
                    jc_memcpy(pucTempHeader, pstProvInfo->pucHeader, pstProvInfo->uiHeaderLen);
                    jdd_MemFree(pstProvInfo->pucHeader);
                    pstProvInfo->pucHeader = pucTempHeader;
                    pucTempHeader = pucTempHeader + pstProvInfo->uiHeaderLen;
                }
                else
                {
                    pstProvInfo->pucHeader = pucTempHeader;
                }
                jc_memcpy(pucTempHeader, pucHeader, uiHeaderLen);
                pstProvInfo->uiHeaderLen = uiNewHeaderLen;
                pstProvInfo->uiRequestID = uiRequestID;
            }
            else
            {
                rCode = JC_ERR_MEMORY_ALLOCATION;
            }
        }
        pstProvInfo->bIsHTTPHeader = bIsHTTPHeader;
    }
    else
    {
        jdd_AssertOnFail(0, "inavlid state")
    }

    if (JC_OK != rCode)
    {
        prv_DeInitContext();
    }
    return rCode;
}

void prv_ResetContinousProvdata()
{
    ST_PROV_INFO *pstProvInfo = JC_NULL;
    pstProvInfo = gstProvContext.pstProvInfo;

    if (JC_NULL != pstProvInfo)
    {
        if (0 != pstProvInfo->uiBufferLen)
        {
            jdd_MemFree(pstProvInfo->pucBuffer);
            pstProvInfo->pucBuffer = JC_NULL;
            pstProvInfo->uiBufferLen = 0;
        }
        if (0 != pstProvInfo->uiHeaderLen)
        {
            jdd_MemFree(pstProvInfo->pucHeader);
            pstProvInfo->pucHeader = JC_NULL;
            pstProvInfo->uiHeaderLen = 0;
        }
    }
    /*if (JC_NULL != gstProvContext.vCommHandle)
    {
        prv_DeInitCommunicator () ;
    }*/
    gstProvContext.eSecType = E_SEC_NOT_DEFINED;
}

JC_RETCODE prv_HandleContinuousAndBootstrapProvisioning(void)
{
    JC_RETCODE rCode = JC_OK;
    ST_PROFILE_DETAIL_INFO *pstProfileDetail = JC_NULL;
    ST_PROVISION_INFO *pstProvisionInfo = JC_NULL;

    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: prv_HandleContinuousAndBootstrapProvisioning",0x0910116c));
    if (E_CONTINUOUS_PROVISION_FLAG == gstProvContext.eProvType &&
        JC_NULL != gstProvContext.pcProvURL)
    {
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: E_CONTINUOUS_PROVISION_FLAG",0x0910116d));
        pstProvisionInfo = (ST_PROVISION_INFO *)gstProvContext.vProvHandle;
        pstProfileDetail = pstProvisionInfo->pstProvDetails;
        rCode = prv_FillPxLogicalInfo(pstProfileDetail->pstPxLogical,
                                      &(gstProvContext.pstContProfList));

        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("prv_FillPxLogicalInfo returns %d",0x0910116e), rCode);
    }
    else
    {
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: E_BOOTSTRAP_FLAG",0x0910116f));
        rCode = prv_FillApplicationInfo(&gstProvContext.pstProfList);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("prv_FillApplicationInfo returns %d",0x09101170), rCode);
        if (JC_OK == rCode)
        {
            prv_updateMissingProfileInfo(&(gstProvContext.pstProfList->stProfile));
        }
    }
    LogProfileList();
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG: prv_HandleContinuousAndBootstrapProvisioning returns %d",0x09101171), rCode);
    return rCode;
}

/*****************************************************************************
*
* @fn 			: prv_FillPxLogicalInfo
*
* @brief		: Fills provisioning (PXLOGICAL) information into profile structure
*
* @param[in]	: pPxLogic	Provisioning (PXLOGICAL) information
* @param[out]	: ppProfList	Profile list to be filled with provisioning information
*
* @retval		: JC_OK	On Successful and Others on failure specific error code.
*
******************************************************************************/
JC_RETCODE prv_FillPxLogicalInfo(ST_PROV_PXLOGICAL *pstPxLogic,
                                 ST_PROV_INFO_LIST **ppstProfList)
{

    JC_RETCODE rCode = JC_OK;
    ST_PROV_INFO_LIST *pstProfList = JC_NULL;
    ST_PROV_PXPHYSICAL *pstPxPhys = JC_NULL;
    ST_PROV_PORT *pstTempPort = JC_NULL;
    ST_PROV_INFO_LIST *pstTempProfList = JC_NULL;
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0), TSTR("prv_FillPxLogicalInfo invoked",0x09101174));

    if (JC_NULL != pstPxLogic && JC_NULL != ppstProfList)
    {
        pstPxPhys = pstPxLogic->pstPxlogicalPxphysical;
        pstTempPort = pstPxLogic->pstPxlogicalPort;
        while (JC_NULL != pstPxPhys)
        {
            rCode = prv_FillPxPhysicalInfo(pstPxPhys, &pstProfList);
            if (JC_OK == rCode)
            {
                if (JC_NULL == *ppstProfList)
                {
                    *ppstProfList = pstProfList;
                }
                else if (JC_NULL != pstTempProfList)
                {
                    pstTempProfList->pstNext = pstProfList;
                }

                while (JC_NULL != pstProfList)
                {
                    if (JC_NULL != pstPxLogic->pcStartPage)
                    {
                        jc_strncpy(pstProfList->stProfile.homepage,
                                   pstPxLogic->pcStartPage, E_DC_MAX_URL_LEN);
                    }
                    if (JC_NULL != pstPxLogic->pcProxyId && 0 < jc_strlen(pstPxLogic->pcProxyId))
                    {
                        // mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("pstPxLogic->pcProxyId is %s",0x09101175),pstPxLogic->pcProxyId);
                        jdi_CUtilsCharToTcharMem(pstPxLogic->pcProxyId, E_DC_MAX_PROFILE_NAME + 1,
                                                 (JC_CHAR *)pstProfList->stProfile.profile_name);
                        jc_strncpy(pstProfList->stProfile.proxy_id,
                                   pstPxLogic->pcProxyId, E_DC_MAX_PROXYID_LEN);
                    }
                    if (JC_NULL != pstPxLogic->pcName && 0 < jc_strlen(pstPxLogic->pcName))
                    {
                        jdi_CUtilsCharToTcharMem(pstPxLogic->pcName, E_DC_MAX_PROFILE_NAME + 1,
                                                 (JC_CHAR *)pstProfList->stProfile.profile_name);
                        jc_strncpy(pstProfList->stProfile.proxy_id,
                                   pstPxLogic->pcName, E_DC_MAX_PROXYID_LEN);
                    }
                    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("pstProfList->stProfile.profile_name is %s ",0x09101176),pstPxLogic->pcName);
                    // mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("pstProfList->stProfile.proxy_id is %s",0x09101177),pstProfList->stProfile.proxy_id);
                    if (JC_NULL != pstTempPort)
                    {
                        prv_FillPortServiceInfo(pstTempPort, pstProfList);
                    }
                    if (JC_NULL == pstProfList->pstNext)
                    {
                        pstTempProfList = pstProfList;
                    }
                    pstProfList = pstProfList->pstNext;
                }
                pstProfList = JC_NULL;
            }
            else
            {
                break;
            }
            pstPxPhys = pstPxPhys->pstNextPxphysical;
        }
    }
    else
    {
        rCode = JC_ERR_INVALID_PARAMETER;
    }
    return rCode;
}

/*****************************************************************************
*
* @fn 			: prv_FillPxPhysicalInfo
*
* @brief		: Fills provisioning (PXPHYSICAL) information into profile structure
*
* @param[in]	: pPxPhys	Provisioning (PXPHYSICAL) information
* @param[out]	: ppProfList	Profile list to be filled with provisioning information
*
* @retval		: JC_OK	On Successful and Others on failure specific error code.
*
******************************************************************************/
JC_RETCODE prv_FillPxPhysicalInfo(ST_PROV_PXPHYSICAL *pstPxPhys,
                                  ST_PROV_INFO_LIST **ppstProfList)
{
    JC_RETCODE rCode = JC_OK;
    ST_PROV_INFO_LIST *pstProfList = JC_NULL;
    ST_PROV_NAPDEF *pstNapDef = JC_NULL;
    ST_PROV_VALUES *pstToNapid = JC_NULL;
    ST_PROV_PORT *pstTempPort = JC_NULL;
    ST_PROV_INFO_LIST *pstTempProfList = JC_NULL;
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0), TSTR("JPRV_LOG:prv_FillPxPhysicalInfo invoked",0x09101178));

    if (JC_NULL != pstPxPhys && JC_NULL != ppstProfList)
    {
        pstToNapid = pstPxPhys->pstToNapid;
        pstTempPort = pstPxPhys->pstPxphysicalPort;
        while (JC_NULL != pstToNapid)
        {
            prv_GetNapDef(pstToNapid, &pstNapDef);
            if (JC_NULL != pstNapDef)
            {
                rCode = prv_FillNapDefInfo(pstNapDef, &pstProfList);
                if (JC_OK == rCode)
                {
                    if (JC_NULL != pstPxPhys->pcPxaddr)
                    {
                        jc_strncpy(pstProfList->stProfile.proxy_ip,
                                   pstPxPhys->pcPxaddr, E_DC_MAX_IP_LEN);
                    }
                    if (JC_NULL != pstTempPort)
                    {
                        prv_FillPortServiceInfo(pstTempPort, pstProfList);
                    }
                    if (JC_NULL == *ppstProfList)
                    {
                        *ppstProfList = pstProfList;
                        pstTempProfList = pstProfList;
                    }
                    else if (JC_NULL != pstTempProfList)
                    {
                        pstTempProfList->pstNext = pstProfList;
                        pstTempProfList = pstTempProfList->pstNext;
                    }
                }
            }
            pstToNapid = pstToNapid->pstNext;
        }
    }
    return rCode;
}

/*****************************************************************************
*
* @fn 			: prv_GetNapDef
*
* @brief		: Gives provisioning information (NAPDEF)
*
* @param[in]	: pstToNapid	Provisioning information
* @param[out]	: ppstNapDef	Provisioning information (NAPDEF)
*
* @retval		: void
*
******************************************************************************/
void prv_GetNapDef(ST_PROV_VALUES *pstToNapid, ST_PROV_NAPDEF **ppstNapDef)
{
    ST_PROVISION_INFO *pstProvInfo = JC_NULL;
    ST_PROFILE_DETAIL_INFO *pstProfileDetail = JC_NULL;
    ST_PROV_NAPDEF *pstNapDef = JC_NULL;
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: prv_GetNapDef invoked",0x0910117f));

    pstProvInfo = (ST_PROVISION_INFO *)gstProvContext.vProvHandle;
    pstProfileDetail = pstProvInfo->pstProvDetails;
    pstNapDef = pstProfileDetail->pstNapdef;

    while (JC_NULL != pstNapDef)
    {
        if (0 == jc_strcasecmp(pstToNapid->pcValue, pstNapDef->pcNapid))
        {
            *ppstNapDef = pstNapDef;
            break;
        }
        pstNapDef = pstNapDef->pstNextNapdef;
    }
}

/*****************************************************************************
*
* @fn 			: prv_FillNapDefInfo
*
* @brief		: Fills provisioning (NAPDEF) information into profile structure
*
* @param[in]	: pNapDef	Provisioning (NAPDEF) information
* @param[out]	: ppProfList	Profile list to be filled with provisioning information
*
* @retval		: JC_OK	On Successful and Others on failure specific error code.
*
******************************************************************************/
JC_RETCODE prv_FillNapDefInfo(ST_PROV_NAPDEF *pstNapDef, ST_PROV_INFO_LIST **ppstProfList)
{
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: prv_FillNapDefInfo invoked",0x09101180));

    JC_RETCODE rCode = JC_OK;
    ST_PROV_INFO_LIST *pstProfile = JC_NULL;
    ST_PROV_VALUES *pstDNS = JC_NULL;

    if (JC_NULL == ppstProfList || JC_NULL == pstNapDef)
    {
        return JC_ERR_INVALID_PARAMETER;
    }

    pstProfile = (ST_PROV_INFO_LIST *)jdd_MemAlloc(1, sizeof(ST_PROV_INFO_LIST));
    if (JC_NULL != pstProfile)
    {
        pstProfile->stProfile.conn_mode = E_DC_CONN_MODE_MAX;
        pstProfile->stProfile.bear_type = E_DC_BEARER_MAX;
        pstProfile->stProfile.csd_call_type = E_DC_CSD_CALL_MAX;
#if 0
		pstProfile->stProfile.gprs_auth_type = GPRS_AUTH_NONE ;
#endif
        if (JC_NULL != pstNapDef->pcName && 0 < jc_strlen(pstNapDef->pcName))
        {
            jdi_CUtilsCharToTcharMem(pstNapDef->pcName, E_DC_MAX_PROFILE_NAME + 1,
                                     (JC_CHAR *)pstProfile->stProfile.profile_name);
            jc_strncpy(pstProfile->stProfile.nap_id, pstNapDef->pcName, E_DC_MAX_NAPID_LEN);
        }
        else
            jc_strncpy(pstProfile->stProfile.nap_id, pstNapDef->pcNapid, E_DC_MAX_NAPID_LEN);

        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(2)|TSMAP(1)|TSMAP(2),TSTR(" pstProfile->stProfile.nap_id is %s  profile_name is %s",0x09101181),pstProfile->stProfile.nap_id,pstNapDef->pcName);
        if (JC_NULL != pstNapDef->pstNapdefNapAuthInfo)
        {
            if (JC_NULL != pstNapDef->pstNapdefNapAuthInfo->pcAuthName)
            {
                if (0 != jc_strcasecmp(pstNapDef->pstNapdefNapAuthInfo->pcAuthName,
                                       gpcNullString))
                {
                    jc_strncpy(pstProfile->stProfile.username,
                               pstNapDef->pstNapdefNapAuthInfo->pcAuthName,
                               MAX_DATA_CONN_USERNAME_SIZE);
                }
            }
            if (JC_NULL != pstNapDef->pstNapdefNapAuthInfo->pcAuthSecret)
            {
                if (0 != jc_strcasecmp(pstNapDef->pstNapdefNapAuthInfo->pcAuthSecret,
                                       gpcNullString))
                {
                    jc_strncpy(pstProfile->stProfile.password,
                               pstNapDef->pstNapdefNapAuthInfo->pcAuthSecret,
                               MAX_DATA_CONN_PASSWORD_SIZE);
                }
            }
            if (JC_NULL != pstNapDef->pstNapdefNapAuthInfo->pcAuthType)
            {
#if 0
				if (0 == jc_strcasecmp (pstNapDef->pstNapdefNapAuthInfo->pcAuthType,
					gpcOtaAuthTypePAP))
				{
					pstProfile->stProfile.gprs_auth_type = GPRS_AUTH_PAP ;
				}
				else if (0 == jc_strcasecmp (pstNapDef->pstNapdefNapAuthInfo->pcAuthType,
					gpcOtaAuthTypeCHAP))
				{
					pstProfile->stProfile.gprs_auth_type = GPRS_AUTH_CHAP ;
				}
#endif
            }
        }

        if (JC_NULL != pstNapDef->pstBearer)
        {

            if (0 == jc_strcasecmp(pstNapDef->pstBearer->pcValue, gpcGPRSString))
            {

                if ((JC_NULL != pstNapDef->pcNapAddress) && (0 == jc_strcasecmp(pstNapDef->pcNapAddrType, "APN")))
                {
                    jc_strncpy(pstProfile->stProfile.apn,
                               pstNapDef->pcNapAddress,
                               MAX_DATA_CONN_APN_SIZE);
                }
                pstProfile->stProfile.bear_type = E_DC_BEARER_GPRS;
            }
            else if (0 == jc_strcasecmp(pstNapDef->pstBearer->pcValue, gpcCSDString))
            {
                jc_strncpy(pstProfile->stProfile.dialup_number, pstNapDef->pcNapAddress,
                           E_DC_MAX_PHONE_NUMBER_LEN);
                if (JC_NULL != pstNapDef->pcLinkSpeed)
                {
                    pstProfile->stProfile.baudrate = jc_atoi(pstNapDef->pcLinkSpeed);
                }
                pstProfile->stProfile.bear_type = E_DC_BEARER_CSD;

                if (0 == jc_strcasecmp(pstNapDef->pcCallType, gpcModemString))
                {
                    pstProfile->stProfile.csd_call_type = E_DC_CSD_CALL_ANALOG;
                }
                else if (0 == jc_strcasecmp(pstNapDef->pcCallType, gpcX31String))
                {
                    pstProfile->stProfile.csd_call_type = E_DC_CSD_CALL_ISDN;
                }
            }
            else
            {
                rCode = JC_ERR_INVALID_PARAMETER;
            }
            if (JC_NULL != pstNapDef->pcFirstRetryTimeout)
            {
                pstProfile->stProfile.timeout = jc_atoi(pstNapDef->pcFirstRetryTimeout);
            }
        }

        pstDNS = pstNapDef->pstDnsAddr;
        if (JC_NULL != pstDNS && JC_NULL != pstDNS->pcValue)
        {
            jc_strncpy(pstProfile->stProfile.dns1, pstDNS->pcValue, E_DC_MAX_IP_LEN);
            pstDNS = pstDNS->pstNext;
            if (JC_NULL != pstDNS && JC_NULL != pstDNS->pcValue)
            {
                jc_strncpy(pstProfile->stProfile.dns2, pstDNS->pcValue, E_DC_MAX_IP_LEN);
            }
        }

        (*ppstProfList) = pstProfile;
        pstProfile->pstNext = JC_NULL;
    }
    else
    {
        rCode = JC_ERR_MEMORY_ALLOCATION;
    }

    return rCode;
}

/*****************************************************************************
*
* @fn 			: prv_FillPortServiceInfo
*
* @brief		: Gives provisioning information (NAPDEF)
*
* @param[in]	: pstToNapid	Provisioning information
* @param[out]	: pstProfile	Profile list to be filled with provisioning information
*
* @retval		: void
*
******************************************************************************/
void prv_FillPortServiceInfo(ST_PROV_PORT *pstPortService, ST_PROV_INFO_LIST *pstProfile)
{
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: prv_FillPortServiceInfo invoked",0x09101182));

    ST_PROV_VALUES *pstService = JC_NULL;

    if (JC_NULL != pstPortService)
    {
        pstService = pstPortService->pstService;
        if (JC_NULL != pstPortService->pcPortNumber)
        {
            pstProfile->stProfile.port = jc_atoi(pstPortService->pcPortNumber);
        }
        if (JC_NULL != pstService)
        {
            if (JC_NULL != pstService->pcValue)
            {
                if (0 == jc_strcasecmp(pstService->pcValue, gpcCoWspString))
                {
                    pstProfile->stProfile.conn_mode = E_DC_CONNECTION_ORIENTED;
                }
                else if (0 == jc_strcasecmp(pstService->pcValue, gpcClWspString))
                {
                    pstProfile->stProfile.conn_mode = E_DC_CONNECTION_ORIENTED;
                }
                else if (0 == jc_strcasecmp(pstService->pcValue, gpcCoSecWspString))
                {
                    pstProfile->stProfile.conn_mode = E_DC_CONNECTION_ORIENTED_SECURE;
                }
                else if (0 == jc_strcasecmp(pstService->pcValue, gpcClSecWspString))
                {
                    pstProfile->stProfile.conn_mode = E_DC_CONNECTION_ORIENTED_SECURE;
                }
            }
        }
    }
}

/*****************************************************************************
*
* @fn 			: prv_FillApplicationInfo
*
* @brief		: Fills all provisioning information into profile structure
*
* @param[in]	: No parameter.
* @param[out]	: ppProfList	Profile list to be filled with provisioning information
*
* @retval		: JC_OK	On Successful and Others on failure specific error code.
*
******************************************************************************/
JC_RETCODE prv_FillApplicationInfo(ST_PROV_INFO_LIST **ppstProfList)
{
    JC_RETCODE rCode = JC_ERR_INVALID_PARAMETER;
    ST_PROV_INFO_LIST *pstProfList = JC_NULL;
    ST_PROV_PXLOGICAL *pstPxLogic = JC_NULL;
    ST_PROV_APPLICATION *pstAppList = JC_NULL;
    JC_INT8 *pcURL = JC_NULL;
    E_PROV_PROFILE_TYPE eProfType;
    ST_PROVISION_INFO *pstProvInfo = JC_NULL;
    ST_PROFILE_DETAIL_INFO *pstProfileDetail = JC_NULL;
    ST_PROV_INFO_LIST *pstTempProfList = gstProvContext.pstProfList;
    ST_PROV_NAPDEF *pstNapDef = JC_NULL;
    JC_INT8 *pcName = JC_NULL;

    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: prv_FillApplicationInfo invoked",0x09101179));

    pstProvInfo = (ST_PROVISION_INFO *)gstProvContext.vProvHandle;
    pstProfileDetail = pstProvInfo->pstProvDetails;

    pstAppList = pstProfileDetail->pstApplication;

    if (JC_NULL == pstAppList)
    {
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: prv_FillApplicationInfo JC_NULL == pstAppList",0x0910117a));
        rCode = prv_FillPxLogicalInfo(pstProfileDetail->pstPxLogical, &pstProfList);

        if (JC_OK == rCode)
        {
            *ppstProfList = pstProfList;
        }
    }
    else
    {
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: prv_FillApplicationInfo JC_NULL != pstAppList",0x0910117b));
        while (JC_NULL != pstAppList)
        {
            if (JC_NULL != pstAppList->pcAppid)
            {
                if (JC_NULL != jc_strstr(pstAppList->pcAppid, gpcBrwAppID))
                {
                    eProfType = E_PROF_BROWSER;
                }
#ifdef JATAAYU_SUPPORT_MMS
                else if (JC_NULL != jc_strstr(pstAppList->pcAppid, gpcMMSAppID))
                {
                    eProfType = E_PROF_MMS;
                }
#endif
                else
                {
                    pstAppList = pstAppList->pstNextApplication;
                    continue;
                }
            }
            else
            {
                eProfType = E_PROF_BROWSER;
            }

            if (E_PROF_BROWSER == eProfType)
            {
                if (JC_NULL != pstAppList->pstApplicationResource &&
                    JC_NULL != pstAppList->pstApplicationResource->pcUri)
                {
                    pcURL = pstAppList->pstApplicationResource->pcUri;
                }
            }
            else
            {
                if (JC_NULL != pstAppList->pstAddr &&
                    JC_NULL != pstAppList->pstAddr->pcValue)
                {
                    pcURL = pstAppList->pstAddr->pcValue;
                }
                else if (JC_NULL != pstAppList->pstApplicationAppaddr &&
                         JC_NULL != pstAppList->pstApplicationAppaddr->pcAddr)
                {
                    pcURL = pstAppList->pstApplicationAppaddr->pcAddr;
                }
            }

            if (JC_NULL != pstAppList->pstToProxy && JC_NULL != pstAppList->pstToProxy->pcValue)
            {
                prv_GetPxLogic(pstAppList->pstToProxy, &pstPxLogic);
                if (JC_NULL != pstPxLogic)
                {
                    rCode = prv_FillPxLogicalInfo(pstPxLogic, &pstProfList);
                    pcName = pstAppList->pcName;
                    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("prv_FillApplicationInfo pcName is %s",0x0910117c),pcName);
                }
                else
                {
                    rCode = prv_FillPxLogicalInfo(pstProfileDetail->pstPxLogical, &pstProfList);
                }
            }
            else if (JC_NULL != pstAppList->pstToNapid && JC_NULL != pstAppList->pstToNapid->pcValue)
            {
                prv_GetNapDef(pstAppList->pstToNapid, &pstNapDef);
                if (JC_NULL != pstNapDef)
                {
                    rCode = prv_FillNapDefInfo(pstNapDef, &pstProfList);
                }
            }
            if (JC_OK == rCode)
            {
                if (JC_NULL != pstProfList)
                {
                    pstProfList->eProfType = eProfType;
                    /*if (E_PROF_MMS == eProfType)
					{
						pstProfList->stProfile.profile_type = E_DC_TYPE_MMS ;
					}
					else
					{
						pstProfList->stProfile.profile_type = E_DC_TYPE_WEB ;
					}*/

                    if (JC_NULL == *ppstProfList)
                    {
                        *ppstProfList = pstProfList;
                    }
                    else
                    {
                        pstTempProfList->pstNext = pstProfList;
                    }

                    while (JC_NULL != pstProfList)
                    {
                        if (JC_NULL != pcURL)
                        {
                            jc_strncpy(pstProfList->stProfile.homepage, pcURL, E_DC_MAX_URL_LEN);
                        }
                        if (JC_NULL != pcName)
                        {
                            //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("prv_FillApplicationInfo profile_name is %s",0x0910117d),pcName);
                            jdi_CUtilsCharToTcharMem(pcName, E_DC_MAX_PROFILE_NAME + 1,
                                                     (JC_CHAR *)pstProfList->stProfile.profile_name);
                        }
                        if (JC_NULL == pstProfList->pstNext)
                        {
                            pstTempProfList = pstProfList;
                        }
                        pstProfList = pstProfList->pstNext;
                    }
                    pstProfList = JC_NULL;
                }
                else
                {
                    rCode = JC_ERR_INVALID_PARAMETER;
                }
            }

            pstAppList = pstAppList->pstNextApplication;
        }
    }
    return rCode;
}

/*****************************************************************************
*
* @fn 			: prv_GetPxLogic
*
* @brief		: Gives provisioning information (PXLOGICAL)
*
* @param[in]	: pstToProxy	Provisioning information
* @param[out]	: ppPxLogic	Provisioning information (PXLOGICAL)
*
* @retval		: void
*
******************************************************************************/
void prv_GetPxLogic(ST_PROV_VALUES *pstToProxy, ST_PROV_PXLOGICAL **ppstPxLogic)
{
    ST_PROVISION_INFO *pstProvInfo = JC_NULL;
    ST_PROFILE_DETAIL_INFO *pstProfileDetail = JC_NULL;
    ST_PROV_PXLOGICAL *pstPxLogic = JC_NULL;

    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: prv_GetPxLogic invoked",0x0910117e));

    pstProvInfo = (ST_PROVISION_INFO *)gstProvContext.vProvHandle;
    pstProfileDetail = pstProvInfo->pstProvDetails;
    pstPxLogic = pstProfileDetail->pstPxLogical;

    while (JC_NULL != pstPxLogic)
    {
        if (0 == jc_strcasecmp(pstToProxy->pcValue, pstPxLogic->pcProxyId))
        {
            *ppstPxLogic = pstPxLogic;
            break;
        }
        pstPxLogic = pstPxLogic->pstNextPxlogical;
    }
}

/*****************************************************************************
*
* @fn 			:	prv_updateMissingProfileInfo
*
* @brief		:	This function updates the missing/incorrect profile information
*					to the structure passed. The missing/incorrect information are
*					port number, connection mode or bearer type.
*
* @param[in/out]:	pstDestProf	The pointer to the profile structure that needs to be updated.
*
* @retval		:	void.
*
******************************************************************************/
void prv_updateMissingProfileInfo(DATACONN_PROFILE *pstDestProf)
{
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: prv_updateMissingProfileInfo",0x09101197));
    if (pstDestProf == NULL) // fixbug 32309
        return;
    if (0 == pstDestProf->port)
    {
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: 0 == pstDestProf->port",0x09101198));
        if (E_DC_CONNECTION_ORIENTED == pstDestProf->conn_mode)
        {
            pstDestProf->port = PRV_PORT_9201;
        }
        else if (E_DC_CONNECTION_ORIENTED_SECURE == pstDestProf->conn_mode)
        {
            pstDestProf->port = PRV_PORT_9203;
        }
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG: port[%d]",0x09101199), pstDestProf->port);
    }

    if (E_DC_CONN_MODE_MAX == pstDestProf->conn_mode)
    {
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: E_DC_CONN_MODE_MAX",0x0910119a));
        switch (pstDestProf->port)
        {
        case PRV_PORT_9200:
        case PRV_PORT_9201:
            pstDestProf->conn_mode = E_DC_CONNECTION_ORIENTED;
            break;

        case PRV_PORT_443:
        case PRV_PORT_9202:
        case PRV_PORT_9203:
            pstDestProf->conn_mode = E_DC_CONNECTION_ORIENTED_SECURE;
            break;

        case PRV_PORT_80:
        case PRV_PORT_9401:
        case PRV_PORT_3128:
        case PRV_PORT_8080:
        case PRV_PORT_8008:
            pstDestProf->conn_mode = E_DC_WPHTTP_PROXY;
            break;

        default:
            if ('\0' == *pstDestProf->proxy_ip)
            {
                pstDestProf->conn_mode = E_DC_WPHTTP_DIRECT;
            }
            else
            {
                pstDestProf->conn_mode = E_DC_WPHTTP_PROXY;
            }
            break;
        }
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG: conn_mode %d",0x0910119b), pstDestProf->conn_mode);
    }

    if (E_DC_BEARER_MAX == pstDestProf->bear_type)
    {
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: E_DC_BEARER_MAX",0x0910119c));
        if ('\0' == *pstDestProf->apn)
        {
            pstDestProf->bear_type = E_DC_BEARER_GPRS;
        }
        else
        {
            pstDestProf->bear_type = E_DC_BEARER_CSD;
        }
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG: bear_type %d",0x0910119d), pstDestProf->bear_type);
    }

    if (E_DC_BEARER_GPRS == pstDestProf->bear_type)
    {
#if 0
		if (GPRS_AUTH_NONE == pstDestProf->gprs_auth_type)
		{
			pstDestProf->gprs_auth_type = GPRS_AUTH_PAP ;
		}
#endif
    }
    else
    {
        if (E_DC_CSD_CALL_MAX == pstDestProf->csd_call_type)
        {
            pstDestProf->csd_call_type = E_DC_CSD_CALL_ANALOG;
        }
        if (0 == pstDestProf->baudrate)
        {
            pstDestProf->baudrate = PRV_DEFAULT_BAUDRATE;
        }
        if (0 == pstDestProf->timeout)
        {
            pstDestProf->timeout = PRV_DEFAULT_TIMEOUT;
        }
    }
}

void LogProfileList()
{
#if 1
    ST_PROV_INFO_LIST *pstTempProfList = gstProvContext.pstProfList;
    while (pstTempProfList)
    {
        JC_INT8 pctemp[255] = {
            0,
        };
        AT_TC(g_sw_AT_SMS, "JPRV_LOG:**************profile start***************");
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG:**************profile start***************",0x09101158));
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG:eProfType %d",0x09101159),pstTempProfList->eProfType);
        jdi_CUtilsStrTcsNCpy(pstTempProfList->stProfile.profile_name, 50, pctemp);
        AT_TC(g_sw_AT_SMS, "JPRV_LOG:profile_name %s", pctemp);
        AT_TC(g_sw_AT_SMS, "JPRV_LOG:homepage %s", pstTempProfList->stProfile.homepage);
        AT_TC(g_sw_AT_SMS, "JPRV_LOG:apn %s", pstTempProfList->stProfile.apn);
        AT_TC(g_sw_AT_SMS, "JPRV_LOG:username %s", pstTempProfList->stProfile.username);
        AT_TC(g_sw_AT_SMS, "JPRV_LOG:password %s", pstTempProfList->stProfile.password);
        AT_TC(g_sw_AT_SMS, "JPRV_LOG:proxy_ip %s", pstTempProfList->stProfile.proxy_ip);
        AT_TC(g_sw_AT_SMS, "JPRV_LOG:nap_id %s", pstTempProfList->stProfile.nap_id);
        AT_TC(g_sw_AT_SMS, "JPRV_LOG:proxy_id %s", pstTempProfList->stProfile.proxy_id);
        AT_TC(g_sw_AT_SMS, "JPRV_LOG:port %d", pstTempProfList->stProfile.port);
        AT_TC(g_sw_AT_SMS, "JPRV_LOG:bear_type %d", pstTempProfList->stProfile.bear_type);
        AT_TC(g_sw_AT_SMS, "JPRV_LOG:conn_mode %d", pstTempProfList->stProfile.conn_mode);
        AT_TC(g_sw_AT_SMS, "JPRV_LOG:csd_call_type %d", pstTempProfList->stProfile.csd_call_type);

        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("JPRV_LOG:profile_name %s",0x0910115a),pctemp);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("JPRV_LOG:homepage %s",0x0910115b),pstTempProfList->stProfile.homepage);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("JPRV_LOG:apn %s",0x0910115c),pstTempProfList->stProfile.apn);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("JPRV_LOG:username %s",0x0910115d),pstTempProfList->stProfile.username);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("JPRV_LOG:password %s",0x0910115e),pstTempProfList->stProfile.password);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("JPRV_LOG:proxy_ip %s",0x0910115f),pstTempProfList->stProfile.proxy_ip);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("JPRV_LOG:dialup_number %s",0x09101160),pstTempProfList->stProfile.dialup_number);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("JPRV_LOG:dns1 %s",0x09101161),pstTempProfList->stProfile.dns1);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("JPRV_LOG:dns2 %s",0x09101162),pstTempProfList->stProfile.dns2);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("JPRV_LOG:nap_id %s",0x09101163),pstTempProfList->stProfile.nap_id);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("JPRV_LOG:proxy_id %s",0x09101164),pstTempProfList->stProfile.proxy_id);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG:baudrate %d",0x09101165),pstTempProfList->stProfile.baudrate);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG:timeout %d",0x09101166),pstTempProfList->stProfile.timeout);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG:port %d",0x09101167),pstTempProfList->stProfile.port);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG:bear_type %d",0x09101168),pstTempProfList->stProfile.bear_type);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG:conn_mode %d",0x09101169),pstTempProfList->stProfile.conn_mode);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG:csd_call_type %d",0x0910116a),pstTempProfList->stProfile.csd_call_type);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG:**************profile end***************",0x0910116b));
        pstTempProfList = pstTempProfList->pstNext;
    }
#endif
}

JC_RETCODE prv_DeInitContext()
{
    ST_PROV_INFO_LIST *pstProfListHead = JC_NULL;
    //	ST_PROV_INFO		*pstProvInfo = JC_NULL ;

    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: prv_DeInitContext",0x09101172));

    DeleteHeadProvList();
    prv_DeleteAllCSProfiles();
    prv_DeleteProfList();
    while (JC_NULL != gstProvContext.pstContProfList)
    {
        pstProfListHead = gstProvContext.pstContProfList;

        gstProvContext.pstContProfList = (gstProvContext.pstContProfList)->pstNext;
        jdd_MemFree(pstProfListHead);
    }
    /*
	pstProvInfo = gstProvContext.pstProveInfo ;

	if (JC_NULL != pstProvInfo)
	{
		if (0 != pstProvInfo->uiBufferLen)
		{
			jdd_MemFree (pstProvInfo->pucBuffer) ;
		}
		if (0 != pstProvInfo->uiHeaderLen)
		{
			jdd_MemFree (pstProvInfo->pucHeader) ;
		}
	}*/

    /*
	while (JC_NULL != gstProvContext.pstProfList)
	{
		pstProfListHead = gstProvContext.pstProfList ;
		gstProvContext.pstProfList = (gstProvContext.pstProfList)->pstNext ;
		jdd_MemFree (pstProfListHead) ;
	}*/
    if (JC_NULL != gstProvContext.vProvHandle)
    {
        jdi_ProvisionDeinitialize(gstProvContext.vProvHandle);
    }
    if (JC_NULL != gstProvContext.vCommHandle)
    {
        prv_DeInitCommunicator();
    }

    /*
	if (JC_NULL != pstProvInfo)
	{
		gstProvContext.pstProveInfo = pstProvInfo->pstNext ;
		jdd_MemFree (pstProvInfo) ;
		pstProvInfo = JC_NULL ;
	}*/

    gstProvContext.eSecType = E_SEC_NOT_DEFINED;

    jdd_MemFree(gstProvContext.pmPinValue);
    gstProvContext.pmPinValue = JC_NULL;
    gstProvContext.ucNumOfAttempts = 0;
    jc_memset((void *)&gstProvContext, '\0', sizeof(ST_PROV_CONTEXT));
    return JC_OK;
}

void DeleteHeadProvList()
{
    ST_PROV_INFO *pstTempNode;
    pstTempNode = gstProvContext.pstProvInfo;
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0), TSTR("JPRV_LOG:DeleteHeadProvList()",0x09101147));

    if (pstTempNode)
    {
        gstProvContext.pstProvInfo = pstTempNode->pstNext;
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG:head node %d",0x09101148),gstProvContext.pstProvInfo);
        jdd_MemFree(pstTempNode->pucHeader);
        jdd_MemFree(pstTempNode->pucBuffer);
        jdd_MemFree(pstTempNode);
    }
}

void prv_DeleteAllCSProfiles()
{
    ST_CS_PROFILE_NODE *pstTempNode, *pstPrevNode;
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG:prv_DeleteAllCSProfiles()",0x091011d4));

    pstTempNode = gstProvContext.pstCsProfInfo;

    while (pstTempNode)
    {
        pstPrevNode = pstTempNode;
        pstTempNode = pstTempNode->pNext;
        jdd_MemFree(pstPrevNode->pProfile);
        jdd_MemFree(pstPrevNode);
    }
    gstProvContext.pstCsProfInfo = JC_NULL;
}

JC_RETCODE prv_DeInitCommunicator(void)
{
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG: prv_DeInitCommunicator",0x09101173));
    if (JC_NULL != gstProvContext.vCommHandle)
    {
        jdi_CommunicationDeinitialize(gstProvContext.vCommHandle);
        gstProvContext.vCommHandle = JC_NULL;
    }
    return JC_OK;
}

// this function will form the CS compatible profile information from
// gstProvContext.pstProfList
void prv_FormCSProfiles()
{
    ST_PROV_INFO_LIST *pstProfNode;
    UINT8 count = 0;
    BOOL result = TRUE;
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG:prv_FormCSProfiles()",0x091011ca));
    AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d \n", __func__, __LINE__);
    pstProfNode = gstProvContext.pstProfList;

    while (pstProfNode)
    {
        AT_TC(g_sw_AT_SMS, "AT_WY: %s: %d profile_type = %d \n", __func__, __LINE__, pstProfNode->eProfType);
        prv_FormCSDataConnProfile(pstProfNode);
        prv_FormCSProxyProfile(pstProfNode);
        if (pstProfNode->eProfType == E_PROF_BROWSER)
        {
            prv_SaveProfile(pstProfNode, count);
            count++;
        }
        pstProfNode = pstProfNode->pstNext;
    }
    result = at_CfgSetAtSettings(0);
    AT_TC(g_sw_AT_SMS, "AT_WY: %s Line: %d  result= %d \n", __func__, __LINE__, result);
    //At_synprofile();
}
//extern  AT_Gprs_CidInfo g_staAtGprsCidInfo_e[4][AT_PDPCID_MAX + 1] ;

U16 UnicodeToAnsii(S8 *pOutBuffer, S8 *pInBuffer)
{

    U16 count = 0;

    while (!((*pInBuffer == 0) && (*(pInBuffer + 1) == 0)))
    {
        *pOutBuffer = *(pInBuffer);

#ifdef __FOR_TESTING //MMI_ON_HARDWARE_P
        *pOutBuffer = *(pInBuffer + 1);
#endif
        pInBuffer += 2;
        pOutBuffer++;
        count++;
    }

    *pOutBuffer = 0;
    return count;
}
#if 0
void At_synprofile(void)
{
    UINT8 nSim = gATOtaDataconnProfile[0].nSimID;

    g_staAtGprsCidInfo_e[nSim][1].uCid = 1;
    g_staAtGprsCidInfo_e[nSim][1].nDLCI = nSim;
    g_staAtGprsCidInfo_e[nSim][1].nApnSize = jc_strlen(gATOtaDataconnProfile[0].apn);
    jc_strcpy(g_staAtGprsCidInfo_e[nSim][1].pApn, gATOtaDataconnProfile[0].apn);

    g_staAtGprsCidInfo_e[nSim][1].nUsernameSize= jc_strlen(gATOtaDataconnProfile[0].username);
    jc_strcpy(g_staAtGprsCidInfo_e[nSim][1].uaUsername, gATOtaDataconnProfile[0].username);

    g_staAtGprsCidInfo_e[nSim][1].nPasswordSize= jc_strlen(gATOtaDataconnProfile[0].password);
    jc_strcpy(g_staAtGprsCidInfo_e[nSim][1].uaPassword, gATOtaDataconnProfile[0].password);

    //AT_TC(g_sw_AT_SMS, "prv_SaveProfile:profile_name %s",gATOtaDataconnProfile[0].profile_name);
    AT_TC(g_sw_AT_SMS, "prv_SaveProfile:apn %s",g_staAtGprsCidInfo_e[nSim][1].pApn);
    //AT_TC(g_sw_AT_SMS, "prv_SaveProfile:proxy_ip %s",);
    AT_TC(g_sw_AT_SMS, "prv_SaveProfile:username %s",g_staAtGprsCidInfo_e[nSim][1].uaUsername);
    AT_TC(g_sw_AT_SMS, "prv_SaveProfile:password %s",g_staAtGprsCidInfo_e[nSim][1].uaPassword);

    //AT_TC(g_sw_AT_SMS, "JPRV_LOG:port %s",gstProvContext.pstProfList->stProfile.port);
}

#endif
void prv_SaveProfile(ST_PROV_INFO_LIST *pstProfNode, UINT8 count)
{

    UINT8 nSim = gstProvContext.pstProvInfo->nSimID;
    UINT8 pPdpAddrStr[AT_GPRS_PDPADDR_MAX_LEN] = {
        0,
    };
    UINT16 uSize;
    UINT8 *pApn = NULL;
    UINT8 *userName = NULL;
    UINT8 *passWord = NULL;
    UINT8 *pPdpAddr = NULL;
    INT32 iResult;
    UINT8 uCid = 1;
    S8 PLMNNum[AT_MAX_OPERATOR_NAME + 1] = {
        0,
    };
    CFW_GPRS_PDPCONT_INFO sPdpCont;

    AT_Gprs_CidInfo *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];

    if (count >= 3)
        return;

    if (NULL == pApn)
    {
        pApn = AT_MALLOC(AT_GPRS_APN_MAX_LEN + 1);
    }
    if (NULL == pPdpAddr)
    {
        pPdpAddr = AT_MALLOC(AT_GPRS_PDPADDR_MAX_LEN + 1);
    }
    if (NULL == userName)
    {
        userName = AT_MALLOC(AT_GPRS_USR_MAX_LEN + 1);
    }
    if (NULL == passWord)
    {
        passWord = AT_MALLOC(AT_GPRS_PAS_MAX_LEN + 1);
    }

    AT_MemSet(&sPdpCont, 0, SIZEOF(CFW_GPRS_PDPCONT_INFO));

    jc_strcpy(PLMNNum, gATOtaDataconnProfile[nSim][count].szOperatorName);
    AT_MemSet(&gATOtaDataconnProfile[nSim][count], 0, SIZEOF(AT_DATACONN_PROFILE));
    jc_strcpy(gATOtaDataconnProfile[nSim][count].szOperatorName, PLMNNum);

    /* copy apn profile to  gAtCurrentSetting  */
    jc_strcpy(gATOtaDataconnProfile[nSim][count].apn, pstProfNode->stProfile.apn);
    //jc_strcpy(gATOtaDataconnProfile[count].nap_id, pstProfNode->stProfile.nap_id);

    jc_strcpy(gATOtaDataconnProfile[nSim][count].username, pstProfNode->stProfile.username);
    jc_strcpy(gATOtaDataconnProfile[nSim][count].password, pstProfNode->stProfile.password);
    gATOtaDataconnProfile[nSim][count].bear_type = pstProfNode->stProfile.bear_type;
    gATOtaDataconnProfile[nSim][count].gprs_auth_type = pstProfNode->stProfile.gprs_auth_type;

    gATOtaDataconnProfile[nSim][count].nSimID = gstProvContext.pstProvInfo->nSimID;
    //jc_strcpy(gATOtaDataconnProfile[count].homepage,pstProfNode->stProfile.homepage);
    jc_strcpy(gATOtaDataconnProfile[nSim][count].proxy_ip, pstProfNode->stProfile.proxy_ip);
    //jc_strcpy(gATOtaDataconnProfile[count].dns1,pstProfNode->stProfile.dns1);
    //jc_strcpy(gATOtaDataconnProfile[count].dns2,pstProfNode->stProfile.dns2);

    /*if(pstProfNode->stProfile.proxy_id[0]!=0)
	    jc_strcpy(gATOtaDataconnProfile[count].proxy_id ,pstProfNode->stProfile.proxy_id);
	else
		jc_strcpy(gATOtaDataconnProfile[count].proxy_id ,pstProfNode->stProfile.nap_id);
*/

    gATOtaDataconnProfile[nSim][count].port = pstProfNode->stProfile.port;
    gATOtaDataconnProfile[nSim][count].conn_mode = pstProfNode->stProfile.conn_mode;
    gATOtaDataconnProfile[nSim][count].profile_type = pstProfNode->eProfType;

    sPdpCont.nApnSize = jc_strlen(gATOtaDataconnProfile[nSim][count].apn);
    jc_strcpy(pApn, gATOtaDataconnProfile[nSim][count].apn);
    if (0 != sPdpCont.nApnSize)
    {
        sPdpCont.pApn = pApn;
    }

    sPdpCont.nApnUserSize = jc_strlen(gATOtaDataconnProfile[nSim][count].username);
    jc_strcpy(userName, gATOtaDataconnProfile[nSim][count].username);
    if (0 != sPdpCont.nApnUserSize)
    {
        sPdpCont.pApnUser = userName;
    }
    sPdpCont.nApnPwdSize = jc_strlen(gATOtaDataconnProfile[nSim][count].password);
    jc_strcpy(passWord, gATOtaDataconnProfile[nSim][count].password);
    if (0 != sPdpCont.nApnPwdSize)
    {
        sPdpCont.pApnPwd = passWord;
    }

    /* parse the addr */
    AT_MemSet(pPdpAddrStr, 0, AT_GPRS_PDPADDR_MAX_LEN + 1);
    jc_strcpy(pPdpAddrStr, gATOtaDataconnProfile[nSim][count].proxy_ip);
    uSize = jc_strlen(pPdpAddrStr);

    if (0 != uSize)
    {
        iResult = AT_GPRS_IPStringToPDPAddr(pPdpAddrStr, uSize, pPdpAddr, (UINT8 *)&uSize);
        AT_TC(g_sw_GPRS, "SaveProfile:AT_GPRS_IPStringToPDPAddr, ret = %d.uSize=%d", iResult, uSize);

        if (iResult < 0)
        {
            //uErrCode = ERR_AT_CME_PARAM_INVALID;
            // goto _func_fail;

            sPdpCont.nPdpAddrSize = 0;
        }
        else
        {

            AT_TC(g_sw_GPRS, "pPdpAddr: %d", pPdpAddr[0]);
            AT_TC(g_sw_GPRS, "pPdpAddr: %d", pPdpAddr[1]);
            AT_TC(g_sw_GPRS, "pPdpAddr: %d", pPdpAddr[2]);
            AT_TC(g_sw_GPRS, "pPdpAddr: %d", pPdpAddr[3]);
            sPdpCont.pPdpAddr = pPdpAddr;

            sPdpCont.nPdpAddrSize = uSize;
        }
    }
    else
    {
        sPdpCont.nPdpAddrSize = 0;
    }
    /* save apn to  g_staAtGprsCidInfo_e       */
    g_staAtGprsCidInfo[uCid].uCid = uCid;
    g_staAtGprsCidInfo[uCid].nDLCI = nSim;

    if (sPdpCont.nApnSize != 0)
    {
        g_staAtGprsCidInfo[uCid].nApnSize = sPdpCont.nApnSize;
        AT_StrCpy(g_staAtGprsCidInfo[uCid].pApn, sPdpCont.pApn);
    }

    if (sPdpCont.nPdpAddrSize != 0)
    {
        g_staAtGprsCidInfo[uCid].nPdpAddrSize = sPdpCont.nPdpAddrSize;
        AT_MemCpy(g_staAtGprsCidInfo[uCid].pPdpAddr, sPdpCont.pPdpAddr, sPdpCont.nPdpAddrSize);
    }
    if (sPdpCont.nApnPwdSize != 0)
    {
        g_staAtGprsCidInfo[uCid].nPasswordSize = sPdpCont.nApnPwdSize;
        AT_StrCpy(g_staAtGprsCidInfo[uCid].uaPassword, sPdpCont.pApnPwd);
    }

    if (sPdpCont.nApnUserSize != 0)
    {
        g_staAtGprsCidInfo[uCid].nUsernameSize = sPdpCont.nApnUserSize;
        AT_StrCpy(g_staAtGprsCidInfo[uCid].uaUsername, sPdpCont.pApnUser);
    }
#ifndef AT_NO_GPRS

    // Call csw api to set the pdp cxt.
    iResult = CFW_GprsSetPdpCxt(uCid, &sPdpCont, nSim);
#endif

    AT_TC(g_sw_AT_SMS, "SaveProfile:**************profile start count = %d***************", count);
    // AT_TC(g_sw_AT_SMS, "prv_SaveProfile:profile_name %s",gATOtaDataconnProfile[count].profile_name);
    AT_TC(g_sw_AT_SMS, "prv_SaveProfile:apn %s", gATOtaDataconnProfile[nSim][count].apn);
    AT_TC(g_sw_AT_SMS, "prv_SaveProfile:proxy_ip %s", gATOtaDataconnProfile[nSim][count].proxy_ip);
    AT_TC(g_sw_AT_SMS, "prv_SaveProfile:username %s", gATOtaDataconnProfile[nSim][count].username);
    AT_TC(g_sw_AT_SMS, "prv_SaveProfile:password %s", gATOtaDataconnProfile[nSim][count].password);
    AT_TC(g_sw_AT_SMS, "SaveProfile:**************GprsCid profile nSim = %d***************", nSim);
    AT_TC(g_sw_AT_SMS, "prv_SaveProfile:apn %s", g_staAtGprsCidInfo[uCid].pApn);
    AT_TC(g_sw_AT_SMS, "prv_SaveProfile:nPdpAddrSize %d", g_staAtGprsCidInfo[uCid].nPdpAddrSize);
    if (NULL != pPdpAddr)
    {
        AT_FREE(pPdpAddr);
        pPdpAddr = NULL;
    }
    if (NULL != pApn)
    {
        AT_FREE(pApn);
        pApn = NULL;
    }

    if (NULL != passWord)
    {
        AT_FREE(passWord);
        passWord = NULL;
    }
    if (NULL != userName)
    {
        AT_FREE(userName);
        userName = NULL;
    }
}
void AT_GetApnCfg(UINT8 nSimID)
{
    UINT8 nSim = nSimID;
    UINT8 pPdpAddrStr[AT_GPRS_PDPADDR_MAX_LEN] = {
        0,
    };
    UINT16 uSize;
    UINT8 *pApn = NULL;
    UINT8 *userName = NULL;
    UINT8 *passWord = NULL;
    UINT8 *pPdpAddr = NULL;
    INT32 iResult;
    CFW_GPRS_PDPCONT_INFO sPdpCont;
    UINT8 uCid = 1;

    AT_Gprs_CidInfo *g_staAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];

    if (NULL == pApn)
    {
        pApn = AT_MALLOC(AT_GPRS_APN_MAX_LEN + 1);
    }
    if (NULL == pPdpAddr)
    {
        pPdpAddr = AT_MALLOC(AT_GPRS_PDPADDR_MAX_LEN + 1);
    }
    if (NULL == userName)
    {
        userName = AT_MALLOC(AT_GPRS_USR_MAX_LEN + 1);
    }
    if (NULL == passWord)
    {
        passWord = AT_MALLOC(AT_GPRS_PAS_MAX_LEN + 1);
    }

    AT_MemSet(&sPdpCont, 0, SIZEOF(CFW_GPRS_PDPCONT_INFO));

    sPdpCont.nApnSize = jc_strlen(gATOtaDataconnProfile[nSimID][0].apn);
    jc_strcpy(pApn, gATOtaDataconnProfile[nSimID][0].apn);
    if (0 != sPdpCont.nApnSize)
    {
        sPdpCont.pApn = pApn;
    }

    sPdpCont.nApnUserSize = jc_strlen(gATOtaDataconnProfile[nSimID][0].username);
    jc_strcpy(userName, gATOtaDataconnProfile[nSimID][0].username);
    if (0 != sPdpCont.nApnUserSize)
    {
        sPdpCont.pApnUser = userName;
    }
    sPdpCont.nApnPwdSize = jc_strlen(gATOtaDataconnProfile[nSimID][0].password);
    jc_strcpy(passWord, gATOtaDataconnProfile[nSimID][0].password);
    if (0 != sPdpCont.nApnPwdSize)
    {
        sPdpCont.pApnPwd = passWord;
    }

    /* parse the addr */
    AT_MemSet(pPdpAddrStr, 0, AT_GPRS_PDPADDR_MAX_LEN + 1);
    jc_strcpy(pPdpAddrStr, gATOtaDataconnProfile[nSimID][0].proxy_ip);
    uSize = jc_strlen(pPdpAddrStr);

    if (0 != uSize)
    {
        iResult = AT_GPRS_IPStringToPDPAddr(pPdpAddrStr, uSize, pPdpAddr, (UINT8 *)&uSize);
        AT_TC(g_sw_GPRS, "AT_GetApnCfg:AT_GPRS_IPStringToPDPAddr, ret = %d.uSize=%d", iResult, uSize);

        if (iResult < 0)
        {
            //uErrCode = ERR_AT_CME_PARAM_INVALID;
            // goto _func_fail;

            sPdpCont.nPdpAddrSize = 0;
        }
        else
        {

            AT_TC(g_sw_GPRS, "pPdpAddr: %d", pPdpAddr[0]);
            AT_TC(g_sw_GPRS, "pPdpAddr: %d", pPdpAddr[1]);
            AT_TC(g_sw_GPRS, "pPdpAddr: %d", pPdpAddr[2]);
            AT_TC(g_sw_GPRS, "pPdpAddr: %d", pPdpAddr[3]);
            sPdpCont.pPdpAddr = pPdpAddr;

            sPdpCont.nPdpAddrSize = uSize;
        }
    }
    else
    {
        sPdpCont.nPdpAddrSize = 0;
    }
    /* save apn to  g_staAtGprsCidInfo_e       */
    g_staAtGprsCidInfo[uCid].uCid = uCid;
    g_staAtGprsCidInfo[uCid].nDLCI = nSim;

    if (sPdpCont.nApnSize != 0)
    {
        g_staAtGprsCidInfo[uCid].nApnSize = sPdpCont.nApnSize;
        AT_StrCpy(g_staAtGprsCidInfo[uCid].pApn, sPdpCont.pApn);
    }

    if (sPdpCont.nPdpAddrSize != 0)
    {
        g_staAtGprsCidInfo[uCid].nPdpAddrSize = sPdpCont.nPdpAddrSize;
        AT_MemCpy(g_staAtGprsCidInfo[uCid].pPdpAddr, sPdpCont.pPdpAddr, sPdpCont.nPdpAddrSize);
    }
    if (sPdpCont.nApnPwdSize != 0)
    {
        g_staAtGprsCidInfo[uCid].nPasswordSize = sPdpCont.nApnPwdSize;
        AT_StrCpy(g_staAtGprsCidInfo[uCid].uaPassword, sPdpCont.pApnPwd);
    }

    if (sPdpCont.nApnUserSize != 0)
    {
        g_staAtGprsCidInfo[uCid].nUsernameSize = sPdpCont.nApnUserSize;
        AT_StrCpy(g_staAtGprsCidInfo[uCid].uaUsername, sPdpCont.pApnUser);
    }
#ifndef AT_NO_GPRS

    // Call csw api to set the pdp cxt.
    iResult = CFW_GprsSetPdpCxt(uCid, &sPdpCont, nSim);
#endif

    AT_TC(g_sw_AT_SMS, "AT_GetApnCfg:**************GprsCid profile ***************");
    AT_TC(g_sw_AT_SMS, "AT_GetApnCfg:apn %s", g_staAtGprsCidInfo[uCid].pApn);
    AT_TC(g_sw_AT_SMS, "AT_GetApnCfg:nPdpAddrSize %d", g_staAtGprsCidInfo[uCid].nPdpAddrSize);

    if (NULL != pPdpAddr)
    {
        AT_FREE(pPdpAddr);
        pPdpAddr = NULL;
    }
    if (NULL != pApn)
    {
        AT_FREE(pApn);
        pApn = NULL;
    }

    if (NULL != passWord)
    {
        AT_FREE(passWord);
        passWord = NULL;
    }
    if (NULL != userName)
    {
        AT_FREE(userName);
        userName = NULL;
    }
}
void prv_FormCSDataConnProfile(ST_PROV_INFO_LIST *pstProfNode)
{
    ST_CS_DATACONN_PROFILE *pstCSDataConn = NULL;

    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG:prv_FormCSDataConnProfile()",0x091011cb));

    pstCSDataConn = (ST_CS_DATACONN_PROFILE *)jdd_MemAlloc(sizeof(ST_CS_DATACONN_PROFILE), 1);
    if (pstCSDataConn)
    {
        jc_strcpy(pstCSDataConn->apn, pstProfNode->stProfile.apn);
        jc_strcpy(pstCSDataConn->nap_id, pstProfNode->stProfile.nap_id);

        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("pstCSDataConn->nap_id is %s ",0x091011cc),pstCSDataConn->nap_id);

        jc_strcpy(pstCSDataConn->username, pstProfNode->stProfile.username);
        jc_strcpy(pstCSDataConn->password, pstProfNode->stProfile.password);
        pstCSDataConn->bear_type = pstProfNode->stProfile.bear_type;
        pstCSDataConn->gprs_auth_type = pstProfNode->stProfile.gprs_auth_type;
        prv_AddToCSProfiles((void *)pstCSDataConn, E_CS_DATACONN_PROFILE);
    }
}

void prv_AddToCSProfiles(void *pProfile, E_CS_PROFILE_TYPE eCSProfileType)
{

    ST_CS_PROFILE_NODE *pstNewNode = JC_NULL;
    ST_CS_PROFILE_NODE *pstTemp = JC_NULL;

    //mmi_trace_b(_MMI|TLEVEL(12)|TDB|TNB_ARG(0),TSTR("JPRV_LOG:prv_AddToCSProfiles called",0x091011d1));

    pstNewNode = (ST_CS_PROFILE_NODE *)jdd_MemAlloc(1, sizeof(ST_CS_PROFILE_NODE));

    if (!pstNewNode)
        return;

    pstNewNode->pNext = JC_NULL;
    pstNewNode->pPrev = JC_NULL;
    pstNewNode->IsProfileDiscarded = E_FALSE;
    pstNewNode->pProfile = pProfile;
    pstNewNode->eCSProfileType = eCSProfileType;

    if (gstProvContext.pstCsProfInfo == JC_NULL)
    {
        //mmi_trace_b(_MMI|TLEVEL(12)|TDB|TNB_ARG(0),TSTR("JPRV_LOG:added as root node",0x091011d2));
        gstProvContext.pstCsProfInfo = pstNewNode;
    }
    else
    {
        pstTemp = gstProvContext.pstCsProfInfo;
        while (pstTemp->pNext != JC_NULL)
            pstTemp = pstTemp->pNext;
        //mmi_trace_b(_MMI|TLEVEL(12)|TDB|TNB_ARG(0),TSTR("JPRV_LOG:added to the link list",0x091011d3));
        pstNewNode->pPrev = pstTemp;
        pstTemp->pNext = pstNewNode;
    }
}

void prv_FormCSProxyProfile(ST_PROV_INFO_LIST *pstProfNode)
{
    ST_CS_PROXY_PROFILE *pstCSProxyProfile = NULL;

    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0),TSTR("JPRV_LOG:prv_FormCSProxyProfile()",0x091011cd));

    pstCSProxyProfile = (ST_CS_PROXY_PROFILE *)jdd_MemAlloc(sizeof(ST_CS_PROXY_PROFILE), 1);
    if (pstCSProxyProfile)
    {
        pstCSProxyProfile->nSimID = gstProvContext.pstProvInfo->nSimID;
        jc_strcpy(pstCSProxyProfile->homepage, pstProfNode->stProfile.homepage);
        jc_strcpy(pstCSProxyProfile->proxy_ip, pstProfNode->stProfile.proxy_ip);
        jc_strcpy(pstCSProxyProfile->dns1, pstProfNode->stProfile.dns1);
        jc_strcpy(pstCSProxyProfile->dns2, pstProfNode->stProfile.dns2);
        jc_strcpy(pstCSProxyProfile->nap_id, pstProfNode->stProfile.nap_id);
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("pstCSProxyProfile->nap_id is %s ",0x091011ce),pstCSProxyProfile->nap_id);

        if (pstProfNode->stProfile.proxy_id[0] != 0)
            jc_strcpy(pstCSProxyProfile->proxy_id, pstProfNode->stProfile.proxy_id);
        else
            jc_strcpy(pstCSProxyProfile->proxy_id, pstProfNode->stProfile.nap_id);

        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1)|TSMAP(1),TSTR("pstCSProxyProfile->proxy_id is %s ",0x091011cf),pstCSProxyProfile->proxy_id);

        pstCSProxyProfile->port = pstProfNode->stProfile.port;
        pstCSProxyProfile->conn_mode = pstProfNode->stProfile.conn_mode;
        pstCSProxyProfile->profile_type = pstProfNode->eProfType;
        //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("JPRV_LOG:prv_FormCSProxyProfile profile %d",0x091011d0), pstCSProxyProfile->profile_type);
        prv_AddToCSProfiles((void *)pstCSProxyProfile, E_CS_PROXY_PROFILE);
    }
}

#endif
//===========================================/////
// The end of the file
//===========================================/////
