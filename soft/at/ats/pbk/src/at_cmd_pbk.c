/******************************************************************************************
    Copyright (C), 2002-2010, Coolsand. Co., Ltd.
    File name     : at_cmd_gc.c
    Author         : wangqunyang
    Version        : 2.0
    Date            : 2008.03.20
    Description   : This file implements AT General Command functions
    Others          :
    History         :

   1.        Date  : 2007/11/25
           Author  : unknown
      Description : build this file
          Version  : V1.0

   2.        Date  : 2008.03.20
           Author  : wangqunyang
      Description : modify some code and added comment
          Version  : V2.0


*******************************************************************************************/

#include "at.h"
#include "at_module.h"
#include "at_cmd_pbk.h"
#include "at_cfg.h"
#include "cfw_sim_prv.h"
#include "cfw_sim_pbk_nv.h"


/* declare function called from at_cmd_gc.c */
extern at_chset_t cfg_GetTeChset(void);

// //////////////////////////////////////////////////////////////////////////////////////////
//
// Define and desclare the variable
//
// //////////////////////////////////////////////////////////////////////////////////////////

static UINT32 g_PBK_CurCmdStamp                   = 0;  /* This stamp is mark which command is executed */
static UINT8 g_arrFindText[SIM_PBK_NAME_SIZE + 3] = { 0 };  /* Find text content buffer */
static UINT16 g_nFindStrLen                        = 0; /* Find text length */

static UINT8 g_nMaxNumberSize          = 0;  /* Max number length, "on","ld","fd","sm" are the same value */
static UINT8 g_nMaxTextSize            = 0;  /* Max PBK name length,  "on","ld","fd","sm" are the same value */
static UINT16 g_nMaxSMPBKIndex          = 0;  /* Max SM PBK(default pbk) index value */
static UINT8 g_nInitStorageInfoReqFlag = 1; /* When init, storage info request or not */

static UINT16 g_nListEntryMaxIndex    = 0;  /* Begin list entry index */
static UINT16 g_nListEntryMinIndex    = 0;  /* End list entry num */
static UINT16 g_nListEntryLastStep    = 0;  /* step length */
static UINT16 g_nListEntryStepCounter = 0; /* list step counter */
static UINT32 g_nBeforeCNUMStorage   = 0; /* Save  the storage type before execute CNUM */

extern UINT32 cfw_SimParsePBKRecData(UINT8 *pData, CFW_SIM_PBK_ENTRY_INFO *pGetPBKEntry,
                                     void *proc, UINT8 txtlength, UINT8 nApiNum);
VOID AT_PBK_AsyncEventProcess_GetFDNRecord(CFW_EVENT CfwEvent);

AT_FDN_PBK_LIST *pgATFNDlist[CFW_SIM_COUNT] = {NULL,};
// //////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////

/******************************************************************************************
Function            :   AT_PBK_Init
Description     :       PBK module init
Called By           :   AT_CFW_InitProcess()
Data Accessed   :
Data Updated    :
Input           :       VOID
Output          :
Return          :   void
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
VOID AT_PBK_Init(UINT8 nSim)
{
    AT_TC(g_sw_AT_PBK, "PBK init beginning .......");

    /* support multi language */
    ML_Init();

    /* check set SIM as a default value */

    if (ERR_SUCCESS != CFW_CfgSetPbkStorage(CFW_PBK_SIM, nSim))
    {
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "PBK init: getpbkstorage error", 0, nSim);
        return;
    }

    AT_TC(g_sw_AT_PBK, "PBK init, the phonebook is: gATCurrentnTempStorage = %u", gATCurrentnTempStorage);

    /* Get the Max number size and name size and set request flag */
    AT_TC(g_sw_AT_PBK, "This request is getting max size of PBK number and PBK text!");
    //g_nInitStorageInfoReqFlag = 1;
    AT_PBK_GetStorageInfoRequest(TRUE, 0, nSim);

    AT_FDN_Init(nSim);
    NV_PBK_Init();

    // Just for debug
    AT_TC(g_sw_AT_PBK, "PBK init OK!");

}

/******************************************************************************************
Function            :   AT_PBK_AsyncEventProcess
Description     :       ATS PBK message deliver fuctions, sometimes we used cmd stamp to
                    distinguish reponse of the different AT command
Called By           :   ATS moudle
Data Accessed   :
Data Updated    :
Input           :   COS_EVENT *pEvent
Output          :
Return          :   VOID
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
VOID AT_PBK_AsyncEventProcess(COS_EVENT *pEvent)
{
    // /////////////////////////////////////////////////////////////////////////////
    //
    // In order to display event the PBK received in this function, add trace of which event recieved.
    // add by wangqunyang 2008.04.15
    //
    // //////////////////////////////////////////////////////////////////////////////

    CFW_EVENT CfwEvent = { 0 };
    UINT8 nSim;

    AT_ASSERT(NULL != pEvent);
    AT_CosEvent2CfwEvent(pEvent, &CfwEvent);
    nSim = CfwEvent.nFlag;
    switch (CfwEvent.nEventId)
    {
#if TEST_GET_ALPHA
    extern UINT8 object;
    case EV_CFW_SIM_SEARCH_PBK_RSP:
    {
        CFW_SIM_SEARCH_PBK* pSearchPbk = (CFW_SIM_SEARCH_PBK*)CfwEvent.nParam1;
        AT_TC(g_sw_AT_PBK, "============EV_CFW_SIM_SEARCH_PBK_RSP=============");
        if(CfwEvent.nType != 0)
        {
            AT_PBK_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, NULL, 0, "PBK init: getpbkstorage error", 0, nSim);
            return;
        }

        AT_TC(g_sw_AT_PBK, "<<<<<matched: %d, used = %d, total = %d>>>>>", pSearchPbk->nMatchRecords, pSearchPbk->nUsedRecords, pSearchPbk->nTotalRecords);
        UINT16 i = 0;
        if(pSearchPbk == NULL)
        {
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, CfwEvent.nUTI, nSim);
        }
        else
        {
            if(object == SEEK_SIM_USED)
            {
                UINT8 nResponse[20]={0};
                AT_Sprintf(nResponse,"Used = %d,Total = %d",pSearchPbk->nUsedRecords, pSearchPbk->nTotalRecords);
                AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, nResponse, strlen(nResponse), CfwEvent.nUTI, nSim);
            }
            else
            {
                CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSearchPbk->nData, 100, 16);
                UINT16 offset = 0;
                UINT8 nResponse[100]={0};
                UINT8 len = 0;
                AT_TC(g_sw_AT_PBK, "<<<<< Matched = %d, Total = %d>>>>>", pSearchPbk->nMatchRecords, pSearchPbk->nTotalRecords);

                for(i = 0; i< pSearchPbk->nMatchRecords; i++)
                {
                    CFW_SIM_PBK_INFO pbkinfo;
                    len = 0;
#if 1
                    UINT8 value = CFW_GetPbkInfo(pSearchPbk, &pbkinfo, i+1);
                    if(value == 0xFF)
                    {
                        AT_PBK_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, NULL, 0, "Data is not correct!", 0, nSim);
                        return;
                    }
                    AT_TC(g_sw_AT_PBK, "<<<<< pbkinfo.nIndex = %d, pbkinfo.nType = %d>>>>>", pbkinfo.nIndex, pbkinfo.nType);
                    AT_TC(g_sw_AT_PBK, "<<<<< pbkinfo.nAlpha = %s, pbkinfo.nNumber = %s>>>>>", pbkinfo.nAlpha, pbkinfo.nNumber);
                    if(pbkinfo.nType == 145)
                        len = AT_Sprintf(nResponse, "%d, %d, %s, +%s", pbkinfo.nIndex, pbkinfo.nType, pbkinfo.nAlpha, pbkinfo.nNumber);
                    else if(pbkinfo.nType == 0xFF)
                        len = AT_Sprintf(nResponse, "%d, NULL", pbkinfo.nIndex);
                    else
                        len = AT_Sprintf(nResponse, "%d, %d, %s, %s", pbkinfo.nIndex, pbkinfo.nType, pbkinfo.nAlpha, pbkinfo.nNumber);
#else
                    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSearchPbk->nData + offset + 3, pSearchPbk->nData[offset + 2], 'c');
                    len = AT_Sprintf(nResponse, "%d,%d,",*(UINT16*)(pSearchPbk->nData + offset),*(pSearchPbk->nData + offset + 2));
                    offset += 3;
                    memcpy(nResponse + len, pSearchPbk->nData + offset + 1, pSearchPbk->nData[offset]);
                    len += pSearchPbk->nData[offset];
                    UINT8 l = pSearchPbk->nData[offset];
                    offset += pSearchPbk->nData[offset] + 1;

                    nResponse[len++] = ',';
                    AT_TC(g_sw_AT_PBK, "--->>> len = %d", len);
                    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSearchPbk->nData + offset + 1, pSearchPbk->nData[offset], 'c');
                    memcpy(nResponse + len, pSearchPbk->nData + offset + 1, pSearchPbk->nData[offset]);
                    len += pSearchPbk->nData[offset];
                    AT_TC(g_sw_AT_PBK, "--->>> len = %d",len);
                    offset += pSearchPbk->nData[offset] + 1;
                    AT_TC(g_sw_AT_PBK, "--->>> len = %d",len);
                    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, nResponse, 50, 'c');
                    if((offset & 0x01) != 0)
                        offset++;
#endif
                    AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, nResponse, len, CfwEvent.nUTI, nSim);
                }

                len = AT_Sprintf(nResponse, "\r\n%d Matched, %d Totals",pSearchPbk->nMatchRecords,pSearchPbk->nTotalRecords);
                AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, nResponse, len, CfwEvent.nUTI, nSim);
            }
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, CfwEvent.nUTI, nSim);
        }
        break;
    }
#endif
        case EV_CFW_SIM_GET_FILE_STATUS_RSP:
        {
                if(CfwEvent.nType != 0)
                {
ERROR_PROCESS:
                    AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "PBK init: getpbkstorage error", 0, nSim);
                    AT_ZERO_PARAM1(pEvent);
                    return;
                }
#define FCP_TEMPLATE_TAG        0x62
#define FILE_DESCRIPTOR_TAG     0x82
                UINT8* pData = CfwEvent.nParam1;
                UINT8 nRecordLength = 0;
                UINT8 offset = 0;
                if(CFW_GetSimType(nSim) == 1)     //USIM card
                {
                    if(pData[offset++] != FCP_TEMPLATE_TAG)
                        goto ERROR_PROCESS;

                    offset++;
                    if(pData[offset++] != FILE_DESCRIPTOR_TAG)
                        goto ERROR_PROCESS;

                    if((pData[offset] != 2) && (pData[offset] != 5))
                        goto ERROR_PROCESS;
                    offset += 2;
                    if(pData[offset++] != 0x21)
                        goto ERROR_PROCESS;

                    nRecordLength = (pData[offset]<< 8) | pData[offset + 1];
                }
                else
                {
                    nRecordLength = pData[14];
                    if(nRecordLength == 0)
                        goto ERROR_PROCESS;
                }
                UINT8 nRespone[30] = {0};
                AT_Sprintf(nRespone, "+CPBF: %d,%d", 20, nRecordLength - 14);
                AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nRespone, strlen(nRespone), CfwEvent.nUTI, nSim);
                break;
        }
        case EV_CFW_SIM_SEARCH_PBK_RSP:
        {
            CFW_SIM_SEARCH_PBK* pSearchPbk = (CFW_SIM_SEARCH_PBK*)CfwEvent.nParam1;
            if(CfwEvent.nType != 0)
            {
                AT_PBK_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, NULL, 0, "PBK init: getpbkstorage error", 0, nSim);
                return;
            }

            UINT16 i = 0;
            if(pSearchPbk == NULL)
            {
                AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, CfwEvent.nUTI, nSim);
            }
            else
            {
                    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSearchPbk->nData, 100, 16);
                    UINT16 offset = 0;
                    UINT8 nResponse[100]={0};
                    UINT8 len = 0;
                    AT_TC(g_sw_AT_PBK, "<<<<< Matched = %d, Total = %d>>>>>", pSearchPbk->nMatchRecords, pSearchPbk->nTotalRecords);

                    for(i = 0; i< pSearchPbk->nMatchRecords; i++)
                    {
                        CFW_SIM_PBK_INFO pbkinfo;
                        len = 0;
                        UINT8 value = CFW_GetPbkInfo(pSearchPbk, &pbkinfo, i+1);
                        if(value == 0xFF)
                        {
                            AT_PBK_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, NULL, 0, "Data is not correct!", 0, nSim);
                            return;
                        }
                        AT_TC(g_sw_AT_PBK, "<<<<< pbkinfo.nIndex = %d, pbkinfo.nType = %d>>>>>", pbkinfo.nIndex, pbkinfo.nType);
                        AT_TC(g_sw_AT_PBK, "<<<<< pbkinfo.nAlpha = %s, pbkinfo.nNumber = %s>>>>>", pbkinfo.nAlpha, pbkinfo.nNumber);
                        if(pbkinfo.nType == 145)
                            len = AT_Sprintf(nResponse, "+CPBF:%d, \"+%s\", %d, \"%s\"", pbkinfo.nIndex, pbkinfo.nNumber, pbkinfo.nType, pbkinfo.nAlpha);
                        else
                        {
                            if(strlen(pbkinfo.nAlpha) == 0)
                            {
                                if(strlen(pbkinfo.nNumber) == 0)
                                    continue;
                                else
                                    len = AT_Sprintf(nResponse, "+CPBF:%d, \"%s\", %d,\"\"\r\n", pbkinfo.nIndex, pbkinfo.nNumber, pbkinfo.nType);
                            }
                            else
                                len = AT_Sprintf(nResponse, "+CPBF:%d, \"%s\", %d, \"%s\"\r\n", pbkinfo.nIndex, pbkinfo.nNumber, pbkinfo.nType, pbkinfo.nAlpha);
                        }
                        AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, nResponse, len, CfwEvent.nUTI, nSim);
                    }
                }
                AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, CfwEvent.nUTI, nSim);
            }
            break;




    /*************************************************************
    ** Requset: CFW_SimGetPbkStorage()
    ** CPBS: Read mode, because of need data of totalSlot and usedSlot
    ** CPBF:  Set mode
    ** CNUM: Exe mode
    ***************************************************************/
    case EV_CFW_SIM_GET_PBK_STRORAGE_RSP:

        AT_TC(g_sw_AT_PBK, "<<<<<===== AT PBK Get Async Event:  EV_CFW_SIM_GET_PBK_STRORAGE_RSP\n ");
        if ((AT_IsAsynCmdAvailable("+CPBS", g_PBK_CurCmdStamp, CfwEvent.nUTI))
                || (AT_IsAsynCmdAvailable("+CPBF", g_PBK_CurCmdStamp, CfwEvent.nUTI))
                || (AT_IsAsynCmdAvailable("+CNUM", g_PBK_CurCmdStamp, CfwEvent.nUTI)))
        {
            AT_PBK_AsyncEventProcess_GetPbkStorage(CfwEvent);
        }
        else
        {
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, CfwEvent.nUTI, nSim);
            AT_TC(g_sw_AT_PBK, "##### PBK recieved event, but not for our request, discard it !");
        }

        break;

    /*****************************************************
    ** Request: CFW_SimGetPbkStorageInfo()
    ** CPBS: Set mode
    ** CPBR: Test mode
    ** CPBF: Test mode
    ** CPBW: Test mode
    ** PBK init also call this function
    *****************************************************/
    case EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP:
        AT_TC(g_sw_AT_PBK, "<<<<<===== AT PBK Get Async Event:  EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP\n ");
        if (1 == g_nInitStorageInfoReqFlag)
        {
            /* As PBK init , request for getting max number size, max text length
               ** and max pbk index value and process this response event */
            AT_PBK_AsyncEventProcess_InitSaveMaxSize(CfwEvent);
            g_nInitStorageInfoReqFlag = 0;
        }
         else if ((AT_IsAsynCmdAvailable("+CPBS", g_PBK_CurCmdStamp, CfwEvent.nUTI))
                || (AT_IsAsynCmdAvailable("+CPBR", g_PBK_CurCmdStamp, CfwEvent.nUTI))
                || (AT_IsAsynCmdAvailable("+CPBF", g_PBK_CurCmdStamp, CfwEvent.nUTI))
                || (AT_IsAsynCmdAvailable("+CPBW", g_PBK_CurCmdStamp, CfwEvent.nUTI))
                || (AT_IsAsynCmdAvailable("+CNUM", g_PBK_CurCmdStamp, CfwEvent.nUTI)))
        {
            AT_PBK_AsyncEventProcess_GetPbkStorageInfo(CfwEvent);
        }
        else
        {
            AT_TC(g_sw_AT_PBK, "##### PBK recieved event, but not for our request, discard it !");

        }

        break;

    /*****************************************************
    ** Request: CFW_SimListPbkEntries()
    ** CPBR: Set mode
    ** CPBF:  Set mode
    ** CNUM: Exe mode
    *****************************************************/
    case EV_CFW_SIM_LIST_PBK_ENTRY_RSP:

        AT_TC(g_sw_AT_PBK, "<<<<<===== AT PBK Get Async Event:  EV_CFW_SIM_LIST_PBK_ENTRY_RSP\n ");
        if ((AT_IsAsynCmdAvailable("+CPBR", g_PBK_CurCmdStamp, CfwEvent.nUTI))
                || (AT_IsAsynCmdAvailable("+CPBF", g_PBK_CurCmdStamp, CfwEvent.nUTI))
                || (AT_IsAsynCmdAvailable("+CNUM", g_PBK_CurCmdStamp, CfwEvent.nUTI)))
        {
            AT_PBK_AsyncEventProcess_ListPbkEntry(CfwEvent);
        }
        else
        {
            AT_TC(g_sw_AT_PBK, "##### PBK recieved event, but not for our request, discard it !");

        }

        break;

    /*****************************************************
    ** Request: CFW_SimDeletePbkEntry()
    ** CPBW: Set mode
    *****************************************************/
    case EV_CFW_SIM_DELETE_PBK_ENTRY_RSP:
        AT_TC(g_sw_AT_PBK, "<<<<<===== AT PBK Get Async Event:  EV_CFW_SIM_DELETE_PBK_ENTRY_RSP\n ");
        if (AT_IsAsynCmdAvailable("+CPBW", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            AT_PBK_AsyncEventProcess_DeletePbkEntry(CfwEvent);
        }
        else
        {
            AT_TC(g_sw_AT_PBK, "##### PBK recieved event, but not for our request, discard it !");
        }

        break;

    /*****************************************************
    ** Request: CFW_SimAddPbkEntry()
    ** CPBW: Set mode
    ** CPBW: Exe mode
    *****************************************************/
    case EV_CFW_SIM_ADD_PBK_RSP:

        AT_TC(g_sw_AT_PBK, "<<<<<===== AT PBK Get Async Event:  EV_CFW_SIM_ADD_PBK_RSP\n ");
        if (AT_IsAsynCmdAvailable("+CPBW", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            AT_PBK_AsyncEventProcess_AddPbkEntry(CfwEvent);
        }
        else
        {
            AT_TC(g_sw_AT_PBK, "##### PBK recieved event, but not for our request, discard it !");
        }

        break;
    case EV_CFW_SIM_GET_PBK_ENTRY_RSP:

        if( pgATFNDlist[nSim] != NULL)
        {
            AT_PBK_AsyncEventProcess_GetFDNRecord(CfwEvent);
        }
        else
        {
            AT_TC(g_sw_AT_PBK, "##### PBK recieved event, but FDN List is NULL, discard it !");
        }

        break;

    default:

        // do nothing for others
        AT_TC(g_sw_AT_PBK, "AT PBK-eventid-%d---not be processed!\n", CfwEvent.nEventId);
    }

    return;

}

/******************************************************************************************
Function            :   AT_PBK_CmdFunc_CPBS
Description     :       AT CPBS command procedure function
Called By           :   ATS moudle
Data Accessed       :
Data Updated        :
Input           :   AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
VOID AT_PBK_CmdFunc_CPBS(AT_CMD_PARA *pParam)
{

    UINT8 nPromptBuff[64]     = { 0 };
    INT32 nOperationRet       = ERR_SUCCESS;
    UINT8 nParamCount         = 0;
    UINT8 InputStoFlagBuff[8] = { 0 };
    UINT16 InputBuffLen        = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if(g_nInitStorageInfoReqFlag == 1)
    {
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "PBK is initializing", pParam->nDLCI, nSim);
        return;
    }

    AT_ASSERT(NULL != pParam);
    g_PBK_CurCmdStamp = pParam->uCmdStamp;

    switch (pParam->iType)
    {
    case AT_CMD_TEST:
        AT_StrCpy(nPromptBuff, "+CPBS: (\"SM\",\"ON\",\"FD\",\"LD\",\"ME\")");
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), pParam->nDLCI, nSim);
        break;

    case AT_CMD_SET:

        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);
        if ((nOperationRet != ERR_SUCCESS) || (nParamCount != 1))
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbs: input param validate", pParam->nDLCI, nSim);
            return;
        }

        InputBuffLen  = sizeof(InputStoFlagBuff);
        nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING,
                                                InputStoFlagBuff, &InputBuffLen);

        if (nOperationRet != ERR_SUCCESS)
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbs: param invalid", pParam->nDLCI, nSim);
            return;
        }

        /* to capital letter of input string */
        AT_StrUpr(InputStoFlagBuff);
        if (!AT_PBK_SetPBKCurrentStorage(InputStoFlagBuff))
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbs: wrong storage", pParam->nDLCI, nSim);
            return;
        }

        /* Call common request function */

        AT_TC(g_sw_AT_PBK, "CPBS: gATCurrentnTempStorage = %d", gATCurrentnTempStorage);
        AT_TC(g_sw_AT_PBK, "CPBS: InputStoFlagBuff = %s", InputStoFlagBuff);
        if(gATCurrentnTempStorage == CFW_PBK_SIM && CFW_GetSimType(nSim) == 1)
            AT_PBK_GetStorageInfoRequest(TRUE, pParam->nDLCI, nSim);
        else if(gATCurrentnTempStorage == CFW_PBK_ME)
            AT_PBK_GetMEStorgeInfo(pParam->nDLCI, nSim);
    	else
            AT_PBK_GetStorageInfoRequest(FALSE, pParam->nDLCI, nSim);
        break;

    case AT_CMD_READ:
    {
        /* Call common request function */
        AT_TC(g_sw_AT_PBK, "CPBS: gATCurrentnTempStorage %d", gATCurrentnTempStorage);
        if(gATCurrentnTempStorage == CFW_PBK_SIM && CFW_GetSimType(nSim) == 1)
            AT_PBK_GetStorageInfoRequest(TRUE, pParam->nDLCI, nSim);
        else if(gATCurrentnTempStorage == CFW_PBK_ME)
            AT_PBK_GetMEStorgeInfo(pParam->nDLCI, nSim);
        else
            AT_PBK_GetStorageInfoRequest(FALSE, pParam->nDLCI, nSim);
    }
    break;
    default:
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "cpbs: wrong cmd mode", pParam->nDLCI, nSim);
        break;
    }

    return;
}

/******************************************************************************************
Function            :   AT_PBK_CmdFunc_CPBR
Description     :       AT CPBR command procedure function
Called By           :   ATS moudle
Data Accessed   :
Data Updated        :
Input           :   AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
VOID AT_PBK_CmdFunc_CPBR(AT_CMD_PARA *pParam)
{
    INT32 nOperationRet        = ERR_SUCCESS;
    UINT8 nParamCount          = 0;
    UINT16 nIndexStartListEntry = 0;
    UINT16 nIndexEndListEntry   = 0;
    UINT16 nParamLen            = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    AT_ASSERT(NULL != pParam);
    g_PBK_CurCmdStamp = pParam->uCmdStamp;
    if(g_nInitStorageInfoReqFlag == 1)
    {
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "PBK is initializing", pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_TEST:
        if(gATCurrentnTempStorage == CFW_PBK_ME)
            AT_PBK_GetMEStorgeInfo(pParam->nDLCI, nSim);
        else
            AT_PBK_GetStorageInfoRequest(TRUE, pParam->nDLCI, nSim);

        break;

    case AT_CMD_SET:

        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);
        if ((nOperationRet != ERR_SUCCESS) || (0 == nParamCount) || (nParamCount > 2))
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbr: param > 2 ", pParam->nDLCI, nSim);
            return;
        }

        /* para1: nIndexStartListEntry */
        if (nParamCount > 0)
        {
            nParamLen     = 2;
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT16,
                                                    &nIndexStartListEntry, &nParamLen);

            if ((nOperationRet != ERR_SUCCESS) || (0 == nIndexStartListEntry))
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbr: param 1 invalid", pParam->nDLCI, nSim);
                return;
            }

        }

        /* para2: nIndexEndListEntry */
        if (nParamCount > 1)
        {
            nParamLen     = 2;
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT16,
                                                    &nIndexEndListEntry, &nParamLen);

            if ((nOperationRet != ERR_SUCCESS) || (0 == nIndexEndListEntry))
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbr: param 2 invalid", pParam->nDLCI, nSim);
                return;
            }
        }

        /* only have one para */
        if (nParamCount == 1)
        {
            nIndexEndListEntry = nIndexStartListEntry;
        }
        if(gATCurrentnTempStorage == CFW_PBK_ME)
        {
            AT_PBK_ReadMEEntry(nIndexStartListEntry, nIndexEndListEntry, pParam->nDLCI, nSim);
            return;
        }
        /* check startindex and endIndex, and check "SM" pbk max index */
        if ((nIndexStartListEntry > nIndexEndListEntry) || (nIndexEndListEntry > g_nMaxSMPBKIndex))
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbr: wrong index value", pParam->nDLCI, nSim);
            return;
        }

        /* Reset the multi list param */
        g_nListEntryMaxIndex    = 0;
        g_nListEntryMinIndex    = 0;
        g_nListEntryLastStep    = 0;
        g_nListEntryStepCounter = 0;

        /* get the min index and max index */
        g_nListEntryMaxIndex = nIndexEndListEntry;
        g_nListEntryMinIndex = nIndexStartListEntry;

        g_nListEntryStepCounter++;

        /* multi step list if need */
        if ((nIndexEndListEntry - nIndexStartListEntry + 1) <= AT_PBK_LIST_ENTRY_STEP * g_nListEntryStepCounter)
        {
            g_nListEntryLastStep = 1;
            AT_PBK_ListPBKEntryRequest(nIndexStartListEntry, nIndexEndListEntry, FALSE, pParam->nDLCI, nSim);
        }
        else
        {
            AT_PBK_ListPBKEntryRequest(nIndexStartListEntry, nIndexStartListEntry + AT_PBK_LIST_ENTRY_STEP - 1, FALSE,
                                       pParam->nDLCI, nSim);
        }

        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "cpbr: wrong cmd mode ", pParam->nDLCI, nSim);
        break;

    }

    return;

}

/******************************************************************************************
Function            :   AT_PBK_CmdFunc_CPBR
Description     :       AT CPBF command procedure function
Called By           :   ATS moudle
Data Accessed   :
Data Updated    :
Input           :   AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
UINT8 object = 0;
VOID AT_PBK_CmdFunc_CPBF(AT_CMD_PARA *pParam)
{
    INT32 nOperationRet = ERR_SUCCESS;
    UINT8 nParamCount   = 0;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_ASSERT(NULL != pParam);
    g_PBK_CurCmdStamp = pParam->uCmdStamp;
    UINT8 nSimType = CFW_GetSimType(nSim);
    UINT8 nType = 0;
    UINT8 nEF = 0;
    switch(gATCurrentnTempStorage)
    {
    case CFW_PBK_SIM:
        nType = SEEK_SIM_ADN;
        if(nSimType == 1)
            nEF = getADNID(0, nSim);
        else
            nEF = 1; // API_SIM_EF_ADN;
        break;

    case CFW_PBK_SIM_FIX_DIALLING:
        nType = SEEK_SIM_FDN;
        if(nSimType == 1)
            nEF = 68; // API_USIM_EF_FDN;
        else
            nEF = 1;  //API_SIM_EF_FDN;
        break;

    case CFW_PBK_SIM_LAST_DIALLING:
        nType = SEEK_SIM_LND;
        nEF = 7;  //API_SIM_EF_LND;
        break;

    case CFW_PBK_SIM_SERVICE_DIALLING:
        nType = SEEK_SIM_SDN;
        if(nSimType == 1)
            nEF = 80;   //API_USIM_EF_SDN;
        else
            nEF = 9;    //API_SIM_EF_SDN;
        break;

    case CFW_PBK_ON:
        nType = SEEK_SIM_MSISDN;
        if(nSimType == 1)
            nEF = 72;   //API_USIM_EF_MSISDN;
        else
            nEF = 4;    //API_SIM_EF_MSISDN;
        break;

    case CFW_PBK_ME:
        //do nothing
        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: EXE ERROR", pParam->nDLCI, nSim);
        return;
#if 0
    case CFW_PBK_EN:
        if(CFW_GetSimType(nSim) == 1)
            nType = API_USIM_EF_ECC;
        else
            nType = API_SIM_EF_ECC;
        break;
#endif
    }

    switch (pParam->iType)
    {
    case AT_CMD_TEST:
    {
        UINT32 value = 0;
        if(gATCurrentnTempStorage == CFW_PBK_ME)
            AT_PBK_GetMEStorgeInfo(pParam->nDLCI, nSim);
#if 0
        else if(gATCurrentnTempStorage == CFW_PBK_EN)
        {
            value = CFW_SimGetFileStatus(nType, pParam->nDLCI, nSim);
        }
#endif
        else
            value = CFW_SimGetFileStatus(nEF, pParam->nDLCI, nSim);

        if(value != ERR_SUCCESS)
            AT_PBK_Result_Err(value, NULL, 0, "cpbf: EXE ERROR", pParam->nDLCI, nSim);
        break;
    }
    case AT_CMD_SET:

//Test GetAlpha
#if TEST_GET_ALPHA
{
    UINT8 nEF[10]={0};
    UINT16 nLen = 10;
    UINT32 value = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, nEF, &nLen);
    if(value != ERR_SUCCESS)
    {
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: first parameter is invalidate", pParam->nDLCI, nSim);
        return;
    }

    UINT8 nNumber[20]={0};
    UINT16 nLength = 20;
    value = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, nNumber, &nLength);
    if(value != ERR_SUCCESS)
    {
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: second parameter is invalidate", pParam->nDLCI, nSim);
        return;
    }

    UINT8 nType = 0;
    if(memcmp(nEF,"ADN", 3) == 0)
        nType = SEEK_SIM_ADN;
    else if(memcmp(nEF,"FDN",3) == 0)
        nType = SEEK_SIM_FDN;
    else if(memcmp(nEF,"MSISDN",6) ==0)
        nType = SEEK_SIM_MSISDN;
//    else if(memcmp(nEF,"BDN",3) == 0)
//        nType = SEEK_SIM_BDN;
    else if(memcmp(nEF,"SDN",3) == 0)
        nType = SEEK_SIM_SDN;
    else if(memcmp(nEF,"LND",3) == 0)
        nType = SEEK_SIM_LND;
    else
    {
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: first parameter is not allowed", pParam->nDLCI, nSim);
        return;
    }

    UINT8 nObject[10] = {0};
    nLength = 10;
    value = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, &nObject, &nLength);
    if(value != ERR_SUCCESS)
    {
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: third parameter is invalidate", pParam->nDLCI, nSim);
        return;
    }

    if(memcmp(nObject, "NAME", 4) == 0)
        object = SEEK_SIM_NAME;
    else if(memcmp(nObject, "NUMBER", 6) == 0)
        object = SEEK_SIM_NUMBER;
    else if(memcmp(nObject, "NULL", 4) == 0)
        object = SEEK_SIM_USED;
    else
    {
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: third parameter is not allowed", pParam->nDLCI, nSim);
        return;
    }
    AT_TC(g_sw_AT_PBK, "CPBF:nEF = %s, nNumber = %s, nObject = %s", nEF, nNumber, nObject);

    value = CFW_SimSearchPbk(nType, object, ALL_MATCH, nNumber, pParam->nDLCI, nSim);
    if(value != ERR_SUCCESS)
    {
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: CFW_SimGetPbkAlpha", pParam->nDLCI, nSim);
        return;
    }
    AT_PBK_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
    return 0;
}
#endif
//End Testing

        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);
        if ((nOperationRet != ERR_SUCCESS) || (nParamCount != 1))
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: nParamCount !=1", pParam->nDLCI, nSim);
            return;
        }

        /* Before get find text , zero the buffer */
        AT_MemZero(g_arrFindText, sizeof(g_arrFindText));
        g_nFindStrLen = sizeof(g_arrFindText);

        /*
           ** UCS2 parser, get ucs2 string and transfer to bytes
         */
        if (cfg_GetTeChset() == cs_ucs2)
        {
            UINT8 InputASCIIFindText[2 * SIM_PBK_NAME_SIZE + 3] = { 0 }; /* Input ascii string buffer */
            UINT8 UCSBigEndFindText[SIM_PBK_NAME_SIZE + 3]     = { 0 }; /* UCS big ending string buffer */
            UINT8 UCSLittleEndFindText[SIM_PBK_NAME_SIZE + 3]  = { 0 };  /* UCS little ending string buffer */
            UINT16 nInputASCIIFindTextLen                       = sizeof(InputASCIIFindText);  /* Input ascii string len */

            UINT8 *pLocalTextString     = NULL; /* Local language text string */
            UINT32 nLocalTextStringSize = 0;  /* local language text string len */

            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING,
                                                    InputASCIIFindText, &nInputASCIIFindTextLen);

            /* verify the parser result */
            if (nOperationRet != ERR_SUCCESS)
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf ucs2: input param error ", pParam->nDLCI, nSim);
                return;
            }

            AT_TC(g_sw_AT_PBK, "nInputASCIIFindTextLen  = %u", nInputASCIIFindTextLen);

            /* ucs string char conter must be multiple of  4 */
            if (nInputASCIIFindTextLen & 3)
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: input ucs ascii is not multiple of 4",
                                  pParam->nDLCI, nSim);
                return;
            }

            if (!(AT_String2Bytes(UCSBigEndFindText, InputASCIIFindText, (UINT8 *)&nInputASCIIFindTextLen)))
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: AT_String2Bytes error", pParam->nDLCI, nSim);
                return;
            }

            /* just for debug */
            AT_TC(g_sw_AT_PBK, "UCSBigEndFindText == > ");
            AT_TC_MEMBLOCK(g_sw_AT_PBK, UCSBigEndFindText, nInputASCIIFindTextLen, 16);

            /* set current language types */
            AT_Set_MultiLanguage();

            /* transfer ucs2 big ending to little ending */
            if (!(AT_UnicodeBigEnd2Unicode(UCSBigEndFindText, UCSLittleEndFindText, (UINT16)nInputASCIIFindTextLen)))
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: ucs bigend to little ending wrong", pParam->nDLCI,
                                  nSim);
                return;
            }

            /* ucs2 to local language */
            if (ERR_SUCCESS !=
                    ML_Unicode2LocalLanguage(UCSLittleEndFindText, (UINT32)(nInputASCIIFindTextLen), &pLocalTextString,
                                             &nLocalTextStringSize, NULL))
            {
                AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "cpbf: unicode to local language exception", pParam->nDLCI,
                                  nSim);
                return;
            }

            /* check copy string length */
            if (nLocalTextStringSize > g_nFindStrLen)
            {
                AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "cpbf: local text len > find buffer len", pParam->nDLCI, nSim);
                return;
            }

            AT_MemCpy(g_arrFindText, pLocalTextString, nLocalTextStringSize);
            g_nFindStrLen = nLocalTextStringSize;

            /* check the input text length less than Max text size or not,
               ** the unicode first octet is: [80], or [81], or [82], so the length -1 */
            if (g_nFindStrLen > g_nMaxTextSize - 1)
            {
                AT_PBK_Result_Err(ERR_AT_CME_TEXT_LONG, NULL, 0, "cpbf ucs2: too long text input", pParam->nDLCI, nSim);
                return;
            }

            // Free inout para resource
            AT_FREE(pLocalTextString);

        }
        else
        {
            /*
               ** general parser, get gsm string
             */
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING,
                                                    g_arrFindText, &g_nFindStrLen);

            /* verify the parser result */
            if (nOperationRet != ERR_SUCCESS)
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf non-ucs2: input param error ", pParam->nDLCI, nSim);
                return;
            }

            /* check the input text length less than Max text size or not */
            if (g_nFindStrLen > g_nMaxTextSize)
            {
                AT_PBK_Result_Err(ERR_AT_CME_TEXT_LONG, NULL, 0, "cpbf non-ucs2: too long text input ", pParam->nDLCI, nSim);
                return;
            }

        }

        /* just for debug */
        AT_TC(g_sw_AT_PBK, "Input string: g_nFindStrLen = %u, and  find text is: ==> ", g_nFindStrLen);
        AT_TC_MEMBLOCK(g_sw_AT_PBK, g_arrFindText, g_nFindStrLen, 16);
        if(gATCurrentnTempStorage == CFW_PBK_ME)
        {
            AT_PBK_FindMEEntry(pParam->nDLCI, nSim);
        }
        else
        {
        /* CPBF: step 1: get current totalSlot */
            if(strlen(g_arrFindText) != 0)
                CFW_SimSearchPbk(nType, SEEK_SIM_NAME, WILDCARD_MATCH, g_arrFindText, pParam->nDLCI, nSim);
            else
                CFW_SimSearchPbk(nType, SEEK_SIM_USED, ALL_MATCH, g_arrFindText, pParam->nDLCI, nSim);
            AT_PBK_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_CR, 0, NULL, 0, pParam->nDLCI, nSim);
        }

        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "cpbf: wrong cmd mode", pParam->nDLCI, nSim);
        break;

    }
    return;
}

/******************************************************************************************
Function            :   AT_PBK_CmdFunc_CPBW
Description     :       AT CPBW command procedure function
Called By           :   ATS moudle
Data Accessed       :
Data Updated        :
Input           :   AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
VOID AT_PBK_CmdFunc_CPBW(AT_CMD_PARA *pParam)
{

    CFW_SIM_PBK_ENTRY_INFO sEntryToBeAdded = { 0 };

    UINT8 pPhoneFullName[SIM_PBK_NAME_SIZE + 3]    = { 0 };  /* include one '\0' and two ' " ' */
    UINT16 nPhoneFullNameSize                       = 0;
    UINT8 pPhoneNumber[SIM_PBK_NUMBER_SIZE * 2 + 4] = { 0 }; /* include one '\0' and two ' " ' and one + */
    UINT16 nPhoneNumSize                            = 0;
    UINT8 nPhoneNumType                            = 0;
    UINT16 nPBKEntryIndex                           = 0;

    BOOL bInternationFlag                             = FALSE;
    UINT8 PhoneNumberBCDBuff[SIM_PBK_NUMBER_SIZE + 1] = { 0 };  /* include one '\0'  */
    UINT8 nBCDNumberLen                               = 0;

    INT32 nOperationRet = ERR_SUCCESS;
    UINT8 nParamCount   = 0;
    UINT8 nStorage      = 0;
    UINT16 nUTI         = 0;
    UINT16 nParamLen     = 0;

    /* For ucs2 pbk entry */
    UINT8 InputAsciiPhoneFullName[4 * SIM_PBK_NAME_SIZE + 3] = { 0 }; /* include one '\0' and two ' " ' */
    UINT8 InputBytesPhoneFullName[SIM_PBK_NAME_SIZE + 3]    = { 0 }; /* include one '\0' and two ' " ' */
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if(g_nInitStorageInfoReqFlag == 1)
    {
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "PBK is initializing", pParam->nDLCI, nSim);
        return;
    }

    AT_ASSERT(NULL != pParam);
    g_PBK_CurCmdStamp = pParam->uCmdStamp;

    switch (pParam->iType)
    {
    case AT_CMD_TEST:
        if(gATCurrentnTempStorage == CFW_PBK_ME)
            AT_PBK_GetMEStorgeInfo(pParam->nDLCI, nSim);
        else
            AT_PBK_GetStorageInfoRequest(TRUE, pParam->nDLCI, nSim);

        break;

    case AT_CMD_SET:
        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);
        if ((nOperationRet != ERR_SUCCESS) || (nParamCount > 4) || (nParamCount == 0))
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw: param input wrong", pParam->nDLCI, nSim);
            return;
        }

        // get cuurent storage
        nStorage = gATCurrentnTempStorage;

        if (!((nStorage == CFW_PBK_ON)
                || (nStorage == CFW_PBK_SIM_FIX_DIALLING)
                || (nStorage == CFW_PBK_SIM_LAST_DIALLING) || (nStorage == CFW_PBK_SIM)
                || (nStorage == CFW_PBK_ME)))
        {
            AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "cpbw: input wrong storage ", pParam->nDLCI, nSim);
            return;
        }

        // /////////////////////////////////////////////////////////////
        // input string have para1
        // /////////////////////////////////////////////////////////////
        if (nParamCount > 0)
        {
            nParamLen     = 2;
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT16, &nPBKEntryIndex, &nParamLen);

            if (ERR_AT_UTIL_CMD_PARA_NULL == nOperationRet)
            {
                nPBKEntryIndex = 0; /* non para input, default value is set */
            }
            else
            {
                if (nOperationRet != ERR_SUCCESS)
                {
                    AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:param 1 invalid", pParam->nDLCI, nSim);
                    return;
                }
            }

        }

        // /////////////////////////////////////////////////////////////
        // input string have para2
        // /////////////////////////////////////////////////////////////
        if (nParamCount > 1)
        {
            nPhoneNumSize = sizeof(pPhoneNumber);
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, pPhoneNumber, &nPhoneNumSize);

            if (nOperationRet != ERR_SUCCESS)
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:param 2 invalid", pParam->nDLCI, nSim);
                return;
            }

            if (!(AT_PBK_IsValidPhoneNumber(pPhoneNumber, nPhoneNumSize, &bInternationFlag)))
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:param 2 invalid phone number", pParam->nDLCI, nSim);
                return;
            }

            if (bInternationFlag)
            {
                /* verify the max number size, include '+' char */
                if (nPhoneNumSize > (g_nMaxNumberSize + 1))
                {
                    AT_PBK_Result_Err(ERR_AT_CME_DAIL_STR_LONG, NULL, 0, "cpbw: param 2 - 1 > max size ", pParam->nDLCI, nSim);
                    return;
                }

                nBCDNumberLen = SUL_AsciiToGsmBcd(&(pPhoneNumber[1]), nPhoneNumSize - 1, PhoneNumberBCDBuff);

            }
            else
            {
                /* verify the max number size, without '+' char */
                if (nPhoneNumSize > g_nMaxNumberSize)
                {
                    AT_PBK_Result_Err(ERR_AT_CME_DAIL_STR_LONG, NULL, 0, "cpbw: param 2 - 2 > max size ", pParam->nDLCI, nSim);
                    return;
                }
                nBCDNumberLen = SUL_AsciiToGsmBcd(pPhoneNumber, nPhoneNumSize, PhoneNumberBCDBuff);
            }

        }

        // /////////////////////////////////////////////////////////////
        // input string have para3
        // /////////////////////////////////////////////////////////////
        if (nParamCount > 2)
        {
            nParamLen = 1;

            /* Reset the value */
            nPhoneNumType = 0;

            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT8, &nPhoneNumType, &nParamLen);

            if (ERR_AT_UTIL_CMD_PARA_NULL == nOperationRet)
            {
                nPhoneNumType = CFW_TELNUMBER_TYPE_UNKNOWN; /* Default value is set */
            }
            else
            {
                if (nOperationRet != ERR_SUCCESS)
                {
                    AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:param 3 invalid", pParam->nDLCI, nSim);
                    return;
                }
            }

            if ((nPhoneNumType != CFW_TELNUMBER_TYPE_INTERNATIONAL)
                    && (nPhoneNumType != CFW_TELNUMBER_TYPE_NATIONAL) && (nPhoneNumType != CFW_TELNUMBER_TYPE_UNKNOWN))
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:param 3 dial number type wrong", pParam->nDLCI,
                                  nSim);
                return;
            }

        }

        // /////////////////////////////////////////////////////////////
        // input string have para4
        // /////////////////////////////////////////////////////////////
        if (nParamCount > 3)
        {

            /*
               ** UCS2 parser, get ucs2 string and transfer to bytes
             */
            if (cfg_GetTeChset() == cs_ucs2)
            {

                nPhoneFullNameSize = sizeof(InputAsciiPhoneFullName);
                nOperationRet      = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_STRING,
                                     InputAsciiPhoneFullName, &nPhoneFullNameSize);

                /* ucs string char conter must be multiple of  4 */
                if (nPhoneFullNameSize & 3)
                {
                    AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw: input ucs ascii is not multiple of 4",
                                      pParam->nDLCI, nSim);
                    return;
                }

                /* just for debug */
                AT_TC(g_sw_AT_PBK, "Before StringToBytes: ascii string len = %u, and input ascii data is: == > ",
                      nPhoneFullNameSize);
                AT_TC_MEMBLOCK(g_sw_AT_PBK, InputAsciiPhoneFullName, nPhoneFullNameSize, 16);

                if (!(AT_String2Bytes(InputBytesPhoneFullName, InputAsciiPhoneFullName, (UINT8 *)&nPhoneFullNameSize)))
                {
                    AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:param 4 AT_String2Bytes error", pParam->nDLCI,
                                      nSim);
                    return;
                }

                /* just for debug */
                AT_TC(g_sw_AT_PBK, "After StringToBytes: bytes len = %u, and input ascii data is: == > ", nPhoneFullNameSize);
                AT_TC_MEMBLOCK(g_sw_AT_PBK, InputBytesPhoneFullName, nPhoneFullNameSize, 16);

            }
            else
            {
                /*
                   ** general parser, get gsm string
                 */
                nPhoneFullNameSize = sizeof(InputBytesPhoneFullName);
                nOperationRet      = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_STRING,
                                     InputBytesPhoneFullName, &nPhoneFullNameSize);
            }

            /* process the input PBK entry text string */
            if (!(AT_PBK_ProcessInputName(pPhoneFullName, sizeof(pPhoneFullName), InputBytesPhoneFullName, (UINT8 *)&nPhoneFullNameSize)))
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:process input name error", pParam->nDLCI, nSim);
                return;
            }

            /* just for debug */
            AT_TC(g_sw_AT_PBK, "g_nMaxTextSize  = %u nPhoneFullNameSize = %u ", g_nMaxTextSize, nPhoneFullNameSize);

            /* check the parse result and save the text at buffer */
            if (ERR_AT_UTIL_CMD_PARA_NULL == nOperationRet)
            {
                AT_MemZero(pPhoneFullName, sizeof(pPhoneFullName));;  /* text is null */
                nPhoneFullNameSize = 0;
            }
            else
            {
                /* check the input parameters */
                if (nOperationRet != ERR_SUCCESS)
                {
                    AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:param 4 error ", pParam->nDLCI, nSim);
                    return;
                }

                /* check the input text length less than Max text size or not */
                if (nPhoneFullNameSize > g_nMaxTextSize)
                {
                    AT_PBK_Result_Err(ERR_AT_CME_TEXT_LONG, NULL, 0, "cpbw:input text too long", pParam->nDLCI, nSim);
                    return;
                }
            }

        }

        /*
           ** Process only have nWriteEntryIndex
         */
        if ((0 != nPBKEntryIndex) && (1 == nParamCount))
        {
  	     if(CFW_PBK_ME == nStorage)
            {
                AT_PBK_DeleteMEEntry(nPBKEntryIndex, pParam->nDLCI, nSim);
                return;
            }
            // if current pbk is "ON", delete operation is not allowed.
#if 0
            if (CFW_PBK_ON == nStorage)
            {
                AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, NULL, 0, "CPBW error: ON phonebook can't be delete",
                                  pParam->nDLCI, nSim);
                return;
            }
#endif
            /* get the UTI and free it after finished calling */
            if (0 == (nUTI = AT_AllocUserTransID()))
            {
                AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "CPBW: malloc UTI error", pParam->nDLCI, nSim);
                return;
            }
            nOperationRet = CFW_SimDeletePbkEntry(nStorage, nPBKEntryIndex, pParam->nDLCI, nSim);
            if(CFW_PBK_SIM_FIX_DIALLING == nStorage)
            {
                AT_FDN_DelRecord(nPBKEntryIndex, nSim);
            }

            AT_FreeUserTransID(nUTI);
            if (nOperationRet != ERR_SUCCESS)
            {
                nOperationRet = AT_SetCmeErrorCode(nOperationRet, TRUE);
                AT_PBK_Result_Err(nOperationRet, NULL, 0, "cpbw:CFW_SimDeletePbkEntry check param error", pParam->nDLCI, nSim);
                return;
            }

            AT_PBK_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_CR, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }

        /* the number must not be NULL when the counters
           ** of param is more than one, if only have one param: index,
           ** it not be zero, otherwise, error happened.bug 8315 related */
        else if ((0 == nPBKEntryIndex) && (1 == nParamCount))
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:only one param but index is zero", pParam->nDLCI,
                              nSim);
            return;
        }

        /* according to spec, we check dial number first */
        /* Judge the num type, if is 0, set default value */
        if (bInternationFlag)
        {
            nPhoneNumType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
        }
        else
        {
            if (0 == nPhoneNumType)
            {
                /* no input num type  */
                nPhoneNumType = CFW_TELNUMBER_TYPE_UNKNOWN;
            }
            else if (CFW_TELNUMBER_TYPE_INTERNATIONAL == nPhoneNumType)
            {
                /* have no '+' , but the num type is 145, replace 129 with is  */
                nPhoneNumType = CFW_TELNUMBER_TYPE_UNKNOWN;
            }
            else
            {
                /* get the input num type  */
            }
        }

        /* Before write the entry printf the data, debug these */
        AT_TC(g_sw_AT_PBK, "need to write pbk text: ==>");
        AT_TC_MEMBLOCK(g_sw_AT_PBK, pPhoneFullName, nPhoneFullNameSize, 16);

        /* Set value to Entry written */
        sEntryToBeAdded.phoneIndex = nPBKEntryIndex;
        sEntryToBeAdded.nType      = nPhoneNumType;

        sEntryToBeAdded.pNumber     = PhoneNumberBCDBuff;
        sEntryToBeAdded.nNumberSize = nBCDNumberLen;

        sEntryToBeAdded.pFullName     = pPhoneFullName;
        sEntryToBeAdded.iFullNameSize = nPhoneFullNameSize;

        // if current pbk is "LD", write operation is not allowed.
        if (nStorage == CFW_PBK_SIM_LAST_DIALLING)
        {
            AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, NULL, 0, "CPBW error: LD phonebook can't be written!!!",
                              pParam->nDLCI, nSim);
            return;
        }

        /* get the UTI and free it after finished calling */
        if (0 == (nUTI = AT_AllocUserTransID()))
        {
            AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "CPBW: malloc UTI error", pParam->nDLCI, nSim);
            return;
        }

        // process add entry in the pbk
        if(CFW_PBK_SIM_FIX_DIALLING == nStorage)
        {
            AT_FDN_AddRecord(nPBKEntryIndex, &sEntryToBeAdded, nSim);
        }
        else if(CFW_PBK_ME == nStorage)
        {
            AT_PBK_WriteMEEntry(nPBKEntryIndex, nPhoneNumType, PhoneNumberBCDBuff, nBCDNumberLen,
                                pPhoneFullName, nPhoneFullNameSize, pParam->nDLCI, nSim);
            return;
        }
        nOperationRet = CFW_SimAddPbkEntry(nStorage, &sEntryToBeAdded, pParam->nDLCI, nSim);
        AT_FreeUserTransID(nUTI);
        if (nOperationRet != ERR_SUCCESS)
        {
            nOperationRet = AT_SetCmeErrorCode(nOperationRet, TRUE);
            AT_PBK_Result_Err(nOperationRet, NULL, 0, "CPBW: CFW_SimAddPbkEntry check param  error", pParam->nDLCI, nSim);
            return;
        }

        AT_PBK_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_CR, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "cpbw: wrong cmd mode", pParam->nDLCI, nSim);
        break;

    }

    return;

}

/******************************************************************************************
Function            :   AT_PBK_CmdFunc_CNUM
Description     :       AT CNUM command,get own number
Called By           :   ATS moudle
Data Accessed       :
Data Updated        :
Input           :   AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
VOID AT_PBK_CmdFunc_CNUM(AT_CMD_PARA *pParam)
{
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_ASSERT(NULL != pParam);
    g_PBK_CurCmdStamp = pParam->uCmdStamp;

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_EXE)
    {
        /* CUNM step 1: */
        if(g_nInitStorageInfoReqFlag == 1)
        {
            AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "PBK is initializing", pParam->nDLCI, nSim);
        }

        g_nBeforeCNUMStorage = gATCurrentnTempStorage;
        /* we backup the current stroage to restore */
        gATCurrentnTempStorage = CFW_PBK_ON;
        AT_PBK_GetStorageInfoRequest(FALSE, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "cnum: wrong cmd mode", pParam->nDLCI, nSim);
        return;
    }
}

/******************************************************************************************
Function            :   AT_PBK_Result_OK
Description     :     create PBK Successful result code and notify ATE
Called By           :   by SMS module
Data Accessed   :
Data Updated    :
Input           :     UINT32 uReturnValue, indication the event type
                            UINT32 uResultCode, command execute result code
                            UINT8  nDelayTime, delay time
                            UINT8* pBuffer, data pointer
                            UINT16 nDataSize, data length
Output          :
Return          :   void
Others          :   build by wangqunyang 2008.04.10
*******************************************************************************************/
VOID AT_PBK_Result_OK(UINT32 uReturnValue,
                      UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize, UINT8 nDLCI, UINT8 nSim)
{
    PAT_CMD_RESULT pResult = NULL;
    AT_BACKSIMID(nSim);

    // 填充结果码
    pResult = AT_CreateRC(uReturnValue,
                          uResultCode, CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, nDelayTime, pBuffer, nDataSize, nDLCI);
    AT_Notify2ATM(pResult, nDLCI);

    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }

    // Add by wangqunyang 2008.04.09
    AT_TC(g_sw_AT_PBK, "=====>>>>> AT PBK Notify To ATE, Return Value: %s , Result Code: %s\n",
          AT_Get_RetureValue_Name(uReturnValue), AT_Get_ResultCode_Name(uResultCode));

    return;
}

/******************************************************************************************
Function            :   AT_PBK_Result_Err
Description     :      create PBK Error result code and notify ATE and print the trace
Called By           :   PBK module
Data Accessed   :
Data Updated    :
Input           :      UINT32 uErrorCode, UINT8* pBuffer, UINT16 nDataSize,UINT8* pExtendErrInfo
Output          :
Return          :   void
Others          :   build by wangqunyang 2008.04.10
*******************************************************************************************/
VOID AT_PBK_Result_Err(UINT32 uErrorCode, UINT8 *pBuffer, UINT16 nDataSize, UINT8 *pExtendErrInfo, UINT8 nDLCI,
                       UINT8 nSim)
{
    PAT_CMD_RESULT pResult      = NULL;
    UINT8 DefaultExtendInfo[32] = "No Extend Info";

    AT_BACKSIMID(nSim);

    // 填充结果码
    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR, uErrorCode, CMD_ERROR_CODE_TYPE_CME, 0, pBuffer, nDataSize, nDLCI);
    AT_Notify2ATM(pResult, nDLCI);
    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }

    // If exception happened and have executed CNUM ,
    // then restore storage's value before execute CNUM's
    if (0 != g_nBeforeCNUMStorage)
    {
        // have AT+CNUM procedure
        gATCurrentnTempStorage = g_nBeforeCNUMStorage;
        g_nBeforeCNUMStorage   = 0;
    }

    // Add by wangqunyang 2008.04.09
    if (NULL == pExtendErrInfo)
    {
        pExtendErrInfo = DefaultExtendInfo;
    }
    return;
}

/******************************************************************************************
Function            :   AT_PBK_ProcessInputName
Description     :
Called By           :   AT_PBK_CmdFunc_CPBW()
Data Accessed       :
Data Updated    :
Input           :       UINT8* arrDest, UINT8* arrSource, UINT8* pLen
Output          :
Return          :   UINT32
Others          :   build  by wangqunyang 2008.05.22
*******************************************************************************************/
BOOL AT_PBK_ProcessInputName(UINT8 *arrDest, UINT8 nDestLen, UINT8 *arrSource, UINT8 *pLen)
{

    if ((arrDest == NULL) || (arrSource == NULL))
    {
        AT_TC(g_sw_AT_PBK, "AT_PBK_ProcessInputName: input pointer is NULL");
        return FALSE;
    }

    if (cfg_GetTeChset() == cs_ucs2)
    {
        arrDest[0] = 0x80;

        if (*pLen > (nDestLen - 1))
        {
            AT_TC(g_sw_AT_PBK, "AT_PBK_ProcessInputName: source size > dest buffer size 1");
            return FALSE;
        }

        AT_MemCpy(&arrDest[1], arrSource, *pLen);

        *pLen += 1;
    }
    else
    {
        if (*pLen > nDestLen)
        {
            AT_TC(g_sw_AT_PBK, "AT_PBK_ProcessInputName: source size > dest buffer size 2");
            return FALSE;
        }

        AT_MemCpy(arrDest, arrSource, *pLen);

    }

    return TRUE;

}

/******************************************************************************************
Function            :   AT_PBK_ProcessOutputName
Description     :
Called By           :   AT_PBK_AsyncEventProcess_ListPbkEntry()
Data Accessed       :
Data Updated    :
Input           :       UINT8* arrDest, UINT8* arrSource, UINT8* pLen
Output          :
Return          :   UINT32
Others          :   build by wangqunyang 2008.05.22
*******************************************************************************************/
BOOL AT_PBK_ProcessOutputName(UINT8 *arrDest, UINT8 nDestLen, UINT8 *arrSource, UINT8 *pLen)
{
    UINT8 *pTempTextString       = NULL;  /* include one char: '\0' */
    UINT32 nTextSzie             = 0;
    UINT8 pUCSLittleEndData[142] = { 0 };

    if ((arrDest == NULL) || (arrSource == NULL))
    {
        AT_TC(g_sw_AT_PBK, "AT_PBK_ProcessOutputName: input pointer is NULL");
        return FALSE;
    }

    if (0x80 == arrSource[0])
    {

        AT_Set_MultiLanguage();

        if (!(AT_UnicodeBigEnd2Unicode((UINT8 *)(&arrSource[1]), pUCSLittleEndData, (UINT16)(*pLen - 1))))
        {
            AT_TC(g_sw_AT_PBK, "AT_PBK_ProcessOutputName: BigEnd to LittleEnd exception");
            return FALSE;
        }

        if (ERR_SUCCESS !=
                ML_Unicode2LocalLanguage(pUCSLittleEndData, (UINT32)(*pLen - 1), &pTempTextString, &nTextSzie, NULL))
        {
            AT_TC(g_sw_AT_PBK, "AT_PBK_ProcessOutputName: ucs2 to local language exception");
            return FALSE;
        }

        if (nTextSzie > nDestLen)
        {
            AT_TC(g_sw_AT_PBK, "AT_PBK_ProcessOutputName: text size > dest buffer size");
            return FALSE;
        }

        AT_MemCpy(arrDest, pTempTextString, nTextSzie);

        *pLen = nTextSzie;

        // Free inout para resource
        AT_FREE(pTempTextString);
    }
    else
    {
        if (*pLen > nDestLen)
        {
            AT_TC(g_sw_AT_PBK, "AT_PBK_ProcessOutputName: source size > dest buffer size");
            return FALSE;
        }

        AT_MemCpy(arrDest, arrSource, *pLen);
    }

    return TRUE;

}

/******************************************************************************************
Function            :   AT_PBK_IsValidPhoneNumber
Description     :       arrNumber[in]; pNumberSize[in]; bIsInternational[out]
                    legal phone number char: 0-9,*,#,+ (+ can only be the first position)
Called By           :   AT_PBK_CmdFunc_CPBW()
Data Accessed       :
Data Updated    :
Input           :       UINT8 * arrNumber, UINT8 nNumberSize, BOOL * bIsInternational
Output          :
Return          :   UINT32
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
BOOL AT_PBK_IsValidPhoneNumber(UINT8 *arrNumber, UINT8 nNumberSize, BOOL *bIsInternational)
{
    UINT8 nTemp = 0;

    *bIsInternational = FALSE;

    /* The number must not be NULL and  nNumberSize is not zero */
    if ((NULL == arrNumber) || (0 == nNumberSize))
    {
        AT_TC(g_sw_AT_PBK, "AT_PBK_IsValidPhoneNumber: number or num size is 0");
        return FALSE;
    }

    if (arrNumber[0] == '+')
    {
        *bIsInternational = TRUE;
        nTemp             = 1;
    }

    while (nTemp < nNumberSize)
    {
        if ((arrNumber[nTemp] != '#')
                && (arrNumber[nTemp] != '*') && ((arrNumber[nTemp] < '0') || (arrNumber[nTemp] > '9')))
        {
            AT_TC(g_sw_AT_PBK, "AT_PBK_IsValidPhoneNumber: invalid char in the number");
            return FALSE;
        }

        nTemp++;
    }

    return TRUE;

}

/******************************************************************************************
Function            :   AT_PBK_SetPBKCurrentStorage
Description     :
Called By           :   PBK moudle
Data Accessed       :
Data Updated    :
Input           :   UINT8 *pStorageFlag
Output          :
Return          :   BOOL
Others          :   build by wangqunyang 2008.04.22
*******************************************************************************************/
BOOL AT_PBK_SetPBKCurrentStorage(UINT8 *pStorageFlag)
{
    if (AT_StrCmp(pStorageFlag, "SM") == 0)
    {
        gATCurrentnTempStorage = CFW_PBK_SIM;
    }
    else if (AT_StrCmp(pStorageFlag, "FD") == 0)
    {
        gATCurrentnTempStorage = CFW_PBK_SIM_FIX_DIALLING;
    }
    else if (AT_StrCmp(pStorageFlag, "LD") == 0)
    {
        gATCurrentnTempStorage = CFW_PBK_SIM_LAST_DIALLING;
    }
    else if (AT_StrCmp(pStorageFlag, "ON") == 0)
    {
        gATCurrentnTempStorage = CFW_PBK_ON;
    }
    else if (AT_StrCmp(pStorageFlag, "ME") == 0)
    {
        gATCurrentnTempStorage = CFW_PBK_ME;
    }
    else
    {
        AT_TC(g_sw_AT_PBK, "AT_PBK_SetPBKCurrentStorage: storage invalid");
        return FALSE;

    }

    return TRUE;
}

/******************************************************************************************
Function            :   AT_PBK_SetPBKCurrentStorage
Description     :
Called By           :   PBK moudle
Data Accessed       :
Data Updated    :
Input           :   UINT8 *pStorageFlag
Output          :
Return          :   BOOL
Others          :   build by wangqunyang 2008.04.22
*******************************************************************************************/
BOOL AT_PBK_GetPBKCurrentStorage(UINT16 nStorageId, UINT8 *pStorageBuff)
{

    if (nStorageId == CFW_PBK_SIM)
    {
        AT_StrCpy(pStorageBuff, "SM");
    }
    else if (nStorageId == CFW_PBK_ON)
    {
        AT_StrCpy(pStorageBuff, "ON");
    }
    else if (nStorageId == CFW_PBK_SIM_FIX_DIALLING)
    {
        AT_StrCpy(pStorageBuff, "FD");
    }
    else if (nStorageId == CFW_PBK_SIM_LAST_DIALLING)
    {
        AT_StrCpy(pStorageBuff, "LD");
    }
    else if (nStorageId == CFW_PBK_ME)
    {
        AT_StrCpy(pStorageBuff, "ME");
    }
    else
    {
        AT_TC(g_sw_AT_PBK, "AT_PBK_GetPBKCurrentStorage: storage invalid");
        return FALSE;
    }

    return TRUE;

}

VOID AT_PBK_GetMEStorgeInfo(UINT8 nDLCI, UINT8 nSim)
{
    INT32 usenum = 0;
    UINT8 nPromptBuff[64]     = { 0 };
    if(AT_IsAsynCmdAvailable("+CPBS", g_PBK_CurCmdStamp, nDLCI))
    {
        usenum = NV_GET_STORGE_ME_PBK(MAX_ME_PHONE_ENTRIES);
        if(usenum < 0)
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbs: NVRAM read failed!", nDLCI, nSim);
            return;
        }
        AT_Sprintf(nPromptBuff, "+CPBS: \"ME\", %u, %u", usenum, MAX_ME_PHONE_ENTRIES);
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), nDLCI, nSim);
    }
    else if(AT_IsAsynCmdAvailable("+CPBR", g_PBK_CurCmdStamp, nDLCI))
    {
        AT_Sprintf(nPromptBuff, "+CPBR: (1-%u),%u,%u",
                    MAX_ME_PHONE_ENTRIES, ME_PBK_NUMBER_SIZE * 2, ME_PBK_NAME_SIZE);
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), nDLCI, nSim);
    }
    else if(AT_IsAsynCmdAvailable("+CPBW", g_PBK_CurCmdStamp, nDLCI))
    {
        AT_Sprintf(nPromptBuff, "+CPBW: (1-%u),%u,(129,145,161),%u",
                    MAX_ME_PHONE_ENTRIES, ME_PBK_NUMBER_SIZE * 2, ME_PBK_NAME_SIZE);
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), nDLCI, nSim);
    }
    else if(AT_IsAsynCmdAvailable("+CPBF", g_PBK_CurCmdStamp, nDLCI))
    {
        AT_Sprintf(nPromptBuff, "+CPBF: %u,%u", ME_PBK_NUMBER_SIZE * 2, ME_PBK_NAME_SIZE);
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), nDLCI, nSim);
    }
    return;
}
VOID AT_PBK_ReadMEEntry(UINT16 nStartIndex, UINT16 nEndIndex, UINT8 nDLCI, UINT8 nSim)
{
    if((nStartIndex > nEndIndex) || (nEndIndex > MAX_ME_PHONE_ENTRIES) || (nStartIndex < 1))
    {
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbr: (ME) wrong index value", nDLCI, nSim);
        return;
    }

    INT32 iRet;
    NV_ME_PHB_INFO sEntryForME = { 0 };
    UINT8 pMeBuff[NV_ME_BUFF_LEN] = { 0 };
    UINT8 pPhoneNumber[SIM_PBK_NUMBER_SIZE * 2 + 4] = { 0 };
    UINT8 nPromptBuff[64]     = { 0 };
    UINT8 InternationBuff[2] = { 0 };

    for(int i = nStartIndex; i <= nEndIndex; i++)
    {
        SUL_ZeroMemory8(InternationBuff, 2);
        iRet = NV_READ_ME_PBK(i, pMeBuff);
        if(iRet == ERR_SUCCESS)
        {
            Buf2PBKI(pMeBuff, &sEntryForME);
            if(sEntryForME.index != (NV_ME_DEFAULT_VALUE << 8 | NV_ME_DEFAULT_VALUE))
            {
                SUL_GsmBcdToAscii(sEntryForME.tel, sEntryForME.tel_len, pPhoneNumber);
                if (CFW_TELNUMBER_TYPE_INTERNATIONAL == sEntryForME.type)
                {
                    AT_StrCpy(InternationBuff, "+");
                }
                AT_Sprintf(nPromptBuff, "+CPBR: %u,\"%s%s\",%u,\"%s\"", i,
                            InternationBuff, pPhoneNumber, sEntryForME.type, sEntryForME.name);
                AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), nDLCI, nSim);
            }
            else
                continue;
        }
        else
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw: NVRAM read failed!", nDLCI, nSim);
            return;
        }
    }

    AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, nDLCI, nSim);
    return;
}
VOID AT_PBK_WriteMEEntry(UINT16 nIndex, UINT8 nNumType, UINT8 *pBCDNumBuf, UINT8 nBCDNumLen,
                                UINT8 *pNameBuf, UINT16 nNameLen, UINT8 nDLCI, UINT8 nSim)
{
    if((nIndex > MAX_ME_PHONE_ENTRIES) || (nIndex < 1))
    {
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw: (ME) wrong index value", nDLCI, nSim);
        return;
    }

    INT32 iRet;
    NV_ME_PHB_INFO sEntryForME = { 0 };
    UINT8 pMeBuff[NV_ME_BUFF_LEN] = { 0 };
    sEntryForME.index = nIndex;
    sEntryForME.type = nNumType;
    SUL_MemCopy8(sEntryForME.tel, pBCDNumBuf, nBCDNumLen);
    sEntryForME.tel_len = nBCDNumLen;
    SUL_MemCopy8(sEntryForME.name, pNameBuf, nNameLen);
    sEntryForME.name_len= nNameLen;
    PBKI2Buf(&sEntryForME, pMeBuff);

    iRet = NV_WRITE_ME_PBK(nIndex, pMeBuff, NV_ME_WRITE_ADD);
    if(iRet == ERR_SUCCESS)
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, nDLCI, nSim);
    else
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw: NVRAM write failed!", nDLCI, nSim);

    return;
}
VOID AT_PBK_DeleteMEEntry(UINT16 nIndex, UINT8 nDLCI, UINT8 nSim)
{
    if((nIndex > MAX_ME_PHONE_ENTRIES) || (nIndex < 1))
    {
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw: (ME) wrong index value", nDLCI, nSim);
        return;
    }

    INT32 iRet;
    UINT8 pMeBuff[NV_ME_BUFF_LEN] = { 0 };
    SUL_MemSet8(pMeBuff, NV_ME_DEFAULT_VALUE, NV_ME_BUFF_LEN);

    iRet = NV_WRITE_ME_PBK(nIndex, pMeBuff, NV_ME_WRITE_DEL);
    if(iRet == ERR_SUCCESS)
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, nDLCI, nSim);
    else
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw: NVRAM write failed!", nDLCI, nSim);

    return;
}
VOID AT_PBK_FindMEEntry(UINT8 nDLCI, UINT8 nSim)
{
    UINT8 pTempbuff[SIM_PBK_NAME_SIZE + 1] = { 0 }; /* include one char: '\0' */
    UINT8 pMeBuff[NV_ME_BUFF_LEN] = { 0 };
    UINT8 pLocolLangTextString[SIM_PBK_NAME_SIZE + 1] = { 0 };  /* include one char: '\0' */
    UINT8 pOutputTextString[SIM_PBK_NAME_SIZE + 1]    = { 0 }; /* include one char: '\0' */
    NV_ME_PHB_INFO sEntryForME = { 0 };
    UINT8 pPhoneNumber[SIM_PBK_NUMBER_SIZE * 2 + 4] = { 0 };
    UINT8 nPromptBuff[64]     = { 0 };
    INT32 iRet;
    BOOL bFindPBKEntry = FALSE;
    UINT8 InternationBuff[2] = { 0 };

    for(int i = 1; i <= MAX_ME_PHONE_ENTRIES; i++)
    {
        SUL_ZeroMemory8(pTempbuff, SIM_PBK_NAME_SIZE + 1);
        SUL_ZeroMemory8(pLocolLangTextString, SIM_PBK_NAME_SIZE + 1);
        SUL_ZeroMemory8(pOutputTextString, SIM_PBK_NAME_SIZE + 1);
        SUL_ZeroMemory8(InternationBuff, 2);
        iRet = NV_READ_ME_PBK(i, pMeBuff);
        if(iRet == ERR_SUCCESS)
        {
            Buf2PBKI(pMeBuff, &sEntryForME);
            if(sEntryForME.index != (NV_ME_DEFAULT_VALUE << 8 | NV_ME_DEFAULT_VALUE))
            {
                AT_PBK_ProcessOutputName(pOutputTextString, sizeof(pOutputTextString),
                                         sEntryForME.name, &(sEntryForME.name_len));
                AT_MemCpy(pLocolLangTextString, pOutputTextString, sEntryForME.name_len);
                AT_MemCpy(pTempbuff, pLocolLangTextString, AT_StrLen(pLocolLangTextString));
                if ((!AT_StrCmp(pLocolLangTextString, g_arrFindText))
                        || (!AT_StrCmp(AT_StrUpr(pLocolLangTextString), AT_StrUpr(g_arrFindText))))
                {
                    bFindPBKEntry = TRUE;
                }
                else if ((strstr(pLocolLangTextString, g_arrFindText) == (INT8 *)pLocolLangTextString)
                         || (strstr(AT_StrUpr(pLocolLangTextString), AT_StrUpr(g_arrFindText)) ==
                             (INT8 *)pLocolLangTextString))
                {
                    bFindPBKEntry = TRUE;
                }
                if (CFW_TELNUMBER_TYPE_INTERNATIONAL == sEntryForME.type)
                {
                    AT_StrCpy(InternationBuff, "+");
                }
                SUL_GsmBcdToAscii(sEntryForME.tel, sEntryForME.tel_len, pPhoneNumber);
                if (bFindPBKEntry)
                {
                    AT_Sprintf(nPromptBuff, "+CPBF: %u,\"%s%s\",%u,\"%s\"", sEntryForME.index,
                               InternationBuff, pPhoneNumber, sEntryForME.type, pTempbuff);
                    AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), nDLCI, nSim);
                    bFindPBKEntry = FALSE;
                }
            }
            else
            {
                continue;
            }
        }
        else
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: NV read failed!", nDLCI, nSim);
            return;
        }
    }

    AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, nDLCI, nSim);
    return;
}

/******************************************************************************************
Function            :   AT_PBK_GetStorageInfo
Description     :
Called By           :   ATS PBK moudle
Data Accessed   :
Data Updated        :
Input           :   PBK_STORAEINFO_CALLER ePbkStorageInfoCaller, the flag mark call function with
                    CFW_SimGetPbkStrorageInfo() or function with CFW_SimGetPbkStorage()
Output          :
Return          :   VOID
Others          :   build by wangqunyang 2008.04.23
*******************************************************************************************/
VOID AT_PBK_GetStorageInfoRequest(BOOL bCallStorageInofFuncFlag, UINT8 nDLCI, UINT8 nSim)
{
    UINT8 nStorage           = 0;
    UINT32 nGetStorageResult = ERR_SUCCESS;
    UINT16 nUTI              = nDLCI;

    nStorage = gATCurrentnTempStorage;

    /* get the UTI and free it after finished calling */
    if (0 == (nUTI = AT_AllocUserTransID()))
    {
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "PBKGetstorageinfo request:  malloc UTI error", nDLCI, nSim);
        return;
    }

    if (bCallStorageInofFuncFlag)
    {
        AT_TC(g_sw_ATE, "CFW_SimGetPbkStrorageInfo");
        nGetStorageResult = CFW_SimGetPbkStrorageInfo(nStorage, nDLCI, nSim);
    }
    else
    {
        AT_TC(g_sw_ATE, "CFW_SimGetPbkStorage");
        nGetStorageResult = CFW_SimGetPbkStorage(nStorage, nDLCI, nSim);
    }

    AT_FreeUserTransID(nUTI);

    if (nGetStorageResult != ERR_SUCCESS)
    {
        nGetStorageResult = AT_SetCmeErrorCode(nGetStorageResult, FALSE);
        AT_PBK_Result_Err(nGetStorageResult, NULL, 0, "PBKGetstorageinfo request: check param failure", nDLCI, nSim);
        return;
    }

    AT_TC(g_sw_ATE, "GetStorageInfo");
    AT_PBK_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_CR, 100, NULL, 0, nDLCI, nSim);
    return;

}

/******************************************************************************************
Function            :   AT_PBK_ListPBKEntryRequest
Description     :       1. initial a request of list pbk
                    2. multi steps of list, and the step we can custom
Called By           :   ATS PBK moudle
Data Accessed   :
Data Updated        :
Input           :   UINT8 nIndexStart,
                    UINT8 nIndexEnd
                    BOOL bContinueFlag, according to AT_Notify2ATM(), we used different
                             macro: CMD_FUNC_CONTINUE or CMD_FUNC_SUCC_ASYN
Output          :
Return          :   VOID
Others          :   build by wangqunyang 2008.04.23
*******************************************************************************************/
VOID AT_PBK_ListPBKEntryRequest(UINT16 nIndexStart, UINT16 nIndexEnd, BOOL bContinueFlag, UINT8 nDLCI, UINT8 nSim)
{
    UINT8 nStorage          = 0;
    UINT32 nListEntryResult = ERR_SUCCESS;
    UINT16 nUTI             = nDLCI;

    nStorage = gATCurrentnTempStorage;

    AT_TC(g_sw_AT_PBK, "pbk list: nIndexStart = %u,  nIndexEnd = %u", nIndexStart, nIndexEnd);

    /* get the UTI and free it after finished calling */
    if (0 == (nUTI = AT_AllocUserTransID()))
    {
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "pbk list entry:malloc UTI error", nDLCI, nSim);
        return;
    }
    nListEntryResult = CFW_SimListPbkEntries(nStorage, nIndexStart, nIndexEnd, nDLCI, nSim);
    AT_FreeUserTransID(nUTI);

    /* pbk list entry: param check failure */
    if (ERR_SUCCESS != nListEntryResult)
    {
        nListEntryResult = AT_SetCmeErrorCode(nListEntryResult, TRUE);
        AT_PBK_Result_Err(nListEntryResult, NULL, 0, "pbk list entry: param check failure ", nDLCI, nSim);
        return;
    }

    if (bContinueFlag)
    {
        AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, NULL, 0, nDLCI, nSim);
    }
    else
    {
        AT_PBK_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_CR, 0, NULL, 0, nDLCI, nSim);
    }

    return;

}

/******************************************************************************************
Function            :   AT_PBK_AsyncEventProcess_GetPbkStorage
Description     :
Called By           :   ATS moudle
Data Accessed       :
Data Updated        :
Input           :   CFW_EVENT CfwEvent
Output          :
Return          :   VOID
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
VOID AT_PBK_AsyncEventProcess_GetPbkStorage(CFW_EVENT CfwEvent)
{
    UINT8 PromptBuff[64]     = { 0 };
    UINT8 StorageInfoBuff[8] = { 0 };
    UINT32 nErrorCode        = 0;

    CFW_PBK_STORAGE *pPbkStorage = NULL;
    UINT8 nStorage = 0;

    UINT8 nSim = CfwEvent.nFlag;
    switch (CfwEvent.nType)
    {
    case 0:

        pPbkStorage = (CFW_PBK_STORAGE *)CfwEvent.nParam1;
        nStorage    = gATCurrentnTempStorage;

        if (!AT_PBK_GetPBKCurrentStorage(pPbkStorage->storageId, StorageInfoBuff))
        {
            AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "GetPbkStorage resp: GetCurrentStorage is false", CfwEvent.nUTI,
                              nSim);
            return;
        };

        // //////////////////////////////////////////////////////////
        // CPBS Read mode
        // /////////////////////////////////////////////////////////
        if (AT_IsAsynCmdAvailable("+CPBS", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            AT_Sprintf(PromptBuff, "+CPBS: \"%s\",%u,%u", StorageInfoBuff, pPbkStorage->usedSlot, pPbkStorage->totalSlot);
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            return;
        }

        // //////////////////////////////////////////////////////////
        // CPBF Read mode
        // //////////////////////////////////////////////////////////
        else if (AT_IsAsynCmdAvailable("+CPBF", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            /* Reset the multi step list param */
            g_nListEntryMaxIndex    = 0;
            g_nListEntryLastStep    = 0;
            g_nListEntryMinIndex    = 0;
            g_nListEntryStepCounter = 0;

            /* CPBF: step 2: list usedslot */
            /* get the total list number of entry */
            g_nListEntryMinIndex = 1;
            g_nListEntryMaxIndex = pPbkStorage->totalSlot;
            g_nListEntryStepCounter++;
            if (g_nListEntryMaxIndex <= AT_PBK_LIST_ENTRY_STEP * g_nListEntryStepCounter)
            {
                g_nListEntryLastStep = 1;
                AT_PBK_ListPBKEntryRequest(1, pPbkStorage->totalSlot, TRUE, CfwEvent.nUTI, nSim);
            }
            else
            {
                AT_PBK_ListPBKEntryRequest(1, AT_PBK_LIST_ENTRY_STEP, TRUE, CfwEvent.nUTI, nSim);
            }

            return;

        }

        // //////////////////////////////////////////////////////////
        // CNUM Read mode
        // //////////////////////////////////////////////////////////
        else if (AT_IsAsynCmdAvailable("+CNUM", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            if (!(nStorage == CFW_PBK_ON))
            {
                AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "GetPbkStorage resp of cnum: storage is not 'ON' and failure",
                                  CfwEvent.nUTI, nSim);
                return;

            }

            /* Reset the multi step list param */
            g_nListEntryMaxIndex    = 0;
            g_nListEntryLastStep    = 0;
            g_nListEntryMinIndex    = 0;
            g_nListEntryStepCounter = 0;

            /* CUNM step 2: */
            /* get the total list number of entry */
            g_nListEntryMinIndex = 1;
            g_nListEntryMaxIndex = pPbkStorage->totalSlot;
            g_nListEntryStepCounter++;
            if (g_nListEntryMaxIndex <= AT_PBK_LIST_ENTRY_STEP * g_nListEntryStepCounter)
            {
                g_nListEntryLastStep = 1;
                AT_PBK_ListPBKEntryRequest(1, pPbkStorage->totalSlot, TRUE, CfwEvent.nUTI, nSim);
            }
            else
            {
                AT_PBK_ListPBKEntryRequest(1, AT_PBK_LIST_ENTRY_STEP, TRUE, CfwEvent.nUTI, nSim);
            }

            return;

        }
        else
        {
            AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "GetPbkStorage resp : wrong cmd stamp", CfwEvent.nUTI, nSim);
            return;
        }

        break;

    case 0xF0:

        nErrorCode = AT_SetCmeErrorCode(CfwEvent.nParam1, FALSE);
        AT_PBK_Result_Err(nErrorCode, NULL, 0, "GetPbkStorage resp :CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "GetPbkStorage resp : unknown CfwEvent.nType", CfwEvent.nUTI, nSim);
        break;

    }

    return;

}

/******************************************************************************************
Function            :   AT_PBK_AsyncEventProcess_GetPbkStorageInfo
Description     :
Called By           :   ATS moudle
Data Accessed   :
Data Updated        :
Input           :   CFW_EVENT CfwEvent
Output          :
Return          :   VOID
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
VOID AT_PBK_AsyncEventProcess_GetPbkStorageInfo(CFW_EVENT CfwEvent)
{
    UINT8 PromptBuff[64] = { 0 };
    UINT8 pStorageBuff[8] = { 0 };
    UINT32 nErrorCode    = ERR_SUCCESS;
    CFW_PBK_STRORAGE_INFO *pPbkStorageInfo = {0};
    UINT8 nSim = CfwEvent.nFlag;
    AT_TC(g_sw_AT_PBK, "========AT_PBK_AsyncEventProcess_GetPbkStorageInfo========");
    switch (CfwEvent.nType)
    {
    case 0:

        pPbkStorageInfo = (CFW_PBK_STRORAGE_INFO *)CfwEvent.nParam1;

        if (AT_IsAsynCmdAvailable("+CPBS", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            UINT8* pBitmap = pPbkStorageInfo->Bitmap;
            UINT16 nIndex = pPbkStorageInfo->index;
            nIndex = (nIndex + 7) >> 3; //bytes of bitmap
            AT_TC(g_sw_AT_PBK, "nIndex = %d, total = %d", nIndex, pPbkStorageInfo->index);
	        UINT16 nUsed = 0;
            UINT16 i = 0;

            for(i = 0; i < nIndex; i++)
            {
                nUsed += pBitmap[i] & 0x01;
                AT_TC(g_sw_AT_PBK, "===== pBitmap[i] = 0x%x =====", pBitmap[i]);
                AT_TC(g_sw_AT_PBK, "nUsed = %d", nUsed);
                for(UINT8 j = 1; j < 8; j++)
                {
                    pBitmap[i] = pBitmap[i] >> 1;
                    nUsed += pBitmap[i] & 0x01;
//                    AT_TC(g_sw_AT_PBK, "pBitmap[%d] = 0x%x, j = %d", i, pBitmap[i], j);
//                    AT_TC(g_sw_AT_PBK, "nUsed = %d", nUsed);
	            }
            }
		if (gATCurrentnTempStorage == CFW_PBK_ON)
		{
			AT_StrCpy(pStorageBuff, "ON");
		}
		else if (gATCurrentnTempStorage == CFW_PBK_SIM_FIX_DIALLING)
		{
			AT_StrCpy(pStorageBuff, "FD");
		}
		else if (gATCurrentnTempStorage == CFW_PBK_SIM_LAST_DIALLING)
		{
			AT_StrCpy(pStorageBuff, "LD");
		}
		else
		{
			AT_StrCpy(pStorageBuff, "SM");
		}
		AT_TC(g_sw_AT_PBK, "nUsed = %d, nTotal = %d", nUsed, pPbkStorageInfo->index);
            AT_Sprintf(PromptBuff, "+CPBS: \"%s\",%u,%u", pStorageBuff, nUsed,pPbkStorageInfo->index);
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            return;
        }

        if (AT_IsAsynCmdAvailable("+CPBR", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            AT_Sprintf(PromptBuff, "+CPBR: (1-%u),%u,%u",
                       pPbkStorageInfo->index, (pPbkStorageInfo->iNumberLen) * 2, pPbkStorageInfo->txtLen);
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            return;

        }

        if (AT_IsAsynCmdAvailable("+CPBF", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            AT_Sprintf(PromptBuff, "+CPBF: %u,%u", (pPbkStorageInfo->iNumberLen) * 2, pPbkStorageInfo->txtLen);
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            return;
        }

        if (AT_IsAsynCmdAvailable("+CPBW", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            AT_Sprintf(PromptBuff, "+CPBW: (1-%u),%u,(129,145,161),%u",
                       pPbkStorageInfo->index, (pPbkStorageInfo->iNumberLen) * 2, pPbkStorageInfo->txtLen);
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            return;
        }
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "GetPbkStorageInfo resp: wrong cmd stamp", CfwEvent.nUTI, nSim);
        break;

    case 0xF0:
        nErrorCode = AT_SetCmeErrorCode(CfwEvent.nParam1, FALSE);
        AT_PBK_Result_Err(nErrorCode, NULL, 0, "GetPbkStorageInfo resp: CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "GetPbkStorageInfo resp:: unknown CfwEvent.nType", CfwEvent.nUTI,
                          nSim);
        break;
    }

    return;

}

/******************************************************************************************
Function            :   AT_PBK_AsyncEventProcess_ListPbkEntry
Description     :
Called By           :   ATS moudle
Data Accessed   :
Data Updated        :
Input           :   CFW_EVENT CfwEvent
Output          :
Return          :   VOID
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
VOID AT_PBK_AsyncEventProcess_ListPbkEntry(CFW_EVENT CfwEvent)
{
    UINT8 PromptBuff[128] = { 0 };
    UINT32 nErrorCode     = 0;
    UINT8 nEntryCount     = 0;

    CFW_SIM_PBK_ENTRY_INFO *pPbkEntryInfo = NULL;
    UINT8 MarkCmdStampBuff[8] = { 0 };
    BOOL bFindPBKEntry        = FALSE;
    UINT8 nSim = CfwEvent.nFlag;

    switch (CfwEvent.nType)
    {
    case 0:
    {
        nEntryCount   = CfwEvent.nParam2;
        pPbkEntryInfo = (CFW_SIM_PBK_ENTRY_INFO *)CfwEvent.nParam1;

        // ///////////////////////////////////////
        /* Get the AT command cmd stamp          */
        // ///////////////////////////////////////
        if (AT_IsAsynCmdAvailable("+CPBR", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            AT_StrCpy(MarkCmdStampBuff, "+CPBR");

        }
        else if (AT_IsAsynCmdAvailable("+CPBF", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            AT_StrCpy(MarkCmdStampBuff, "+CPBF");

        }
        else if (AT_IsAsynCmdAvailable("+CNUM", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            AT_StrCpy(MarkCmdStampBuff, "+CNUM");
        }
        else
        {
            AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "ListPbkEntry resp: wrong cmd stamp", CfwEvent.nUTI, nSim);
            return;
        }

        /* Just for pbk debug */
        AT_TC(g_sw_AT_PBK, "-----> ListPbkEntry resp: Entry counter = %u ", nEntryCount);

        // ////////////////////////////////////////////////
        // **         have Entry found
        // ////////////////////////////////////////////////
        while (nEntryCount-- > 0)
        {

            UINT8 InternationBuff[2] = { 0 };

            UINT8 LocolLangNumber[SIM_PBK_NUMBER_SIZE * 2 + 1] = { 0 }; /* include one char: '\0' */
            UINT32 nLocolLangNumLen                           = 0;

            UINT8 pLocolLangTextString[SIM_PBK_NAME_SIZE + 1] = { 0 };  /* include one char: '\0' */
            UINT8 pOutputTextString[SIM_PBK_NAME_SIZE + 1]    = { 0 }; /* include one char: '\0' */

            /* clear buffer of prompt  */
            AT_MemZero(PromptBuff, sizeof(PromptBuff));

            /* check the input number sizeless than Max number size or not
               ** if more than the Max size, then discard part of them
             */
            if (2 * (pPbkEntryInfo->nNumberSize) > g_nMaxNumberSize)
            {
                pPbkEntryInfo->nNumberSize = g_nMaxNumberSize / 2;
            }

            /* check the input text length less than Max text size or not
               ** if more than the Max size, then discard part of them
             */
            if (pPbkEntryInfo->iFullNameSize > g_nMaxTextSize)
            {
                pPbkEntryInfo->iFullNameSize = g_nMaxTextSize;
            }

            // BCD to asscii
            nLocolLangNumLen = SUL_GsmBcdToAscii(pPbkEntryInfo->pNumber, pPbkEntryInfo->nNumberSize, LocolLangNumber);

            /* debug for pbk fullname from message node */
            AT_TC(g_sw_AT_PBK, "ListPbkEntry resp: FullName from message node => ");
            AT_TC_MEMBLOCK(g_sw_AT_PBK, pPbkEntryInfo->pFullName, pPbkEntryInfo->iFullNameSize, 16);

            /* process text string from csw  */
            if (!
                    (AT_PBK_ProcessOutputName
                     (pOutputTextString, sizeof(pOutputTextString), pPbkEntryInfo->pFullName, &(pPbkEntryInfo->iFullNameSize))))
            {
                AT_TC(g_sw_AT_PBK, "ListPbkEntry resp: some message node exception");

                /* get list next node, then continue */
                goto label_next_list_node;
            }

            /* buffer overflow */
            if (pPbkEntryInfo->iFullNameSize > sizeof(pLocolLangTextString))
            {
                AT_TC(g_sw_AT_PBK, "ListPbkEntry resp:name size of message node more than local buffer len");

                /* get list next node, then continue */
                goto label_next_list_node;
            }

            /* save the string to local language string buffer */
            AT_MemCpy(pLocolLangTextString, pOutputTextString, pPbkEntryInfo->iFullNameSize);

            /* debug for local text name */
            AT_TC(g_sw_AT_PBK, "ListPbkEntry resp: after tranfer local text string => ");
            AT_TC_MEMBLOCK(g_sw_AT_PBK, pOutputTextString, pPbkEntryInfo->iFullNameSize, 16);

            /* international or not */
            if (CFW_TELNUMBER_TYPE_INTERNATIONAL == pPbkEntryInfo->nType)
            {
                AT_StrCpy(InternationBuff, "+");
            }

            // ///////////////////////////////////////////////////////////
            /* Get the AT command type                  */
            // ///////////////////////////////////////////////////////////
            if (!AT_StrCmp(MarkCmdStampBuff, "+CPBR"))
            {
                AT_Sprintf(PromptBuff, "%s: %u,\"%s%s\",%u,\"%s\"", MarkCmdStampBuff, pPbkEntryInfo->phoneIndex,
                           InternationBuff, LocolLangNumber, pPbkEntryInfo->nType, pLocolLangTextString);
                AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            }
            else if (!AT_StrCmp(MarkCmdStampBuff, "+CPBF"))
            {

                UINT8 pTempbuff[SIM_PBK_NAME_SIZE + 1] = { 0 }; /* include one char: '\0' */

                /* buffer overflow */
                if (AT_StrLen(pLocolLangTextString) > sizeof(pTempbuff))
                {
                    AT_TC(g_sw_AT_PBK, "ListPbkEntry resp: local language text string is too large");

                    /* get list next node, then continue */
                    goto label_next_list_node;
                }

                AT_MemCpy(pTempbuff, pLocolLangTextString, AT_StrLen(pLocolLangTextString));

                /*
                        If the find text buffer is NULL, return all entry
                                if(NULL == g_arrFindText)
                                {
                                    bFindPBKEntry = TRUE;;
                                }
                */

                if ((!AT_StrCmp(pLocolLangTextString, g_arrFindText))
                        || (!AT_StrCmp(AT_StrUpr(pLocolLangTextString), AT_StrUpr(g_arrFindText))))
                {
                    bFindPBKEntry = TRUE;
                }

                /* have part of words matching at beginning */
                else if ((strstr(pLocolLangTextString, g_arrFindText) == (INT8 *)pLocolLangTextString)
                         || (strstr(AT_StrUpr(pLocolLangTextString), AT_StrUpr(g_arrFindText)) ==
                             (INT8 *)pLocolLangTextString))
                {
                    bFindPBKEntry = TRUE;
                }

                /* there are item of entry founded */
                if (bFindPBKEntry)
                {
                    AT_Sprintf(PromptBuff, "%s: %u,\"%s%s\",%u,\"%s\"", MarkCmdStampBuff, pPbkEntryInfo->phoneIndex,
                               InternationBuff, LocolLangNumber, pPbkEntryInfo->nType, pTempbuff);

                    AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
                    bFindPBKEntry = FALSE;
                }

            }
            else if (!AT_StrCmp(MarkCmdStampBuff, "+CNUM"))
            {
                AT_Sprintf(PromptBuff, "%s: \"%s\",\"%s%s\",%u", MarkCmdStampBuff, pLocolLangTextString,
                           InternationBuff, LocolLangNumber, pPbkEntryInfo->nType);

                AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            }
            else
            {
                AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "ListPbkEntry resp: CmdStampBuff error", CfwEvent.nUTI, nSim);
                return;
            }

            // ///////////////////////////////////
            // Get the next list node
            // ///////////////////////////////////
label_next_list_node:
            pPbkEntryInfo =
                (CFW_SIM_PBK_ENTRY_INFO *)((UINT32)pPbkEntryInfo + sizeof(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE);

        }

        /*
           **check multi list entry finished or not
         */
        if (1 == g_nListEntryLastStep)
        {
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, CfwEvent.nUTI, nSim);
            /* Reset the multi list param */
            g_nListEntryMaxIndex    = 0;
            g_nListEntryMinIndex    = 0;
            g_nListEntryLastStep    = 0;
            g_nListEntryStepCounter = 0;

            /* Restore the storage before execute AT + CNUM */
            if (!AT_StrCmp(MarkCmdStampBuff, "+CNUM"))
            {
                gATCurrentnTempStorage = g_nBeforeCNUMStorage;
                g_nBeforeCNUMStorage   = 0;
            }
        }
        else
        {
            /* Length is reach the end point and finished this time */
            if ((g_nListEntryMaxIndex - g_nListEntryMinIndex + 1) <= AT_PBK_LIST_ENTRY_STEP * (g_nListEntryStepCounter + 1))
            {
                g_nListEntryLastStep = 1;
                AT_PBK_ListPBKEntryRequest(g_nListEntryMinIndex + AT_PBK_LIST_ENTRY_STEP * g_nListEntryStepCounter,
                                           g_nListEntryMaxIndex, TRUE, CfwEvent.nUTI, nSim);
            }

            /* have some items to be continue to list  */
            else
            {
                AT_PBK_ListPBKEntryRequest(g_nListEntryMinIndex + AT_PBK_LIST_ENTRY_STEP * g_nListEntryStepCounter,
                                           g_nListEntryMinIndex + AT_PBK_LIST_ENTRY_STEP * (g_nListEntryStepCounter + 1) - 1,
                                           TRUE, CfwEvent.nUTI, nSim);
                g_nListEntryStepCounter++;
            }
            AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, NULL, 0, CfwEvent.nUTI, nSim);
        }

        break;

    }
    case 0xF0:
        nErrorCode = AT_SetCmeErrorCode(CfwEvent.nParam1, FALSE);
        AT_PBK_Result_Err(nErrorCode, NULL, 0, "ListPbkEntry resp: CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "ListPbkEntry resp: unknown CfwEvent.nType", CfwEvent.nUTI, nSim);
        break;
    }

    return;

}

/******************************************************************************************
Function            :   AT_PBK_AsyncEventProcess_DeletePbkEntry
Description     :
Called By           :   ATS moudle
Data Accessed   :
Data Updated    :
Input           :   CFW_EVENT CfwEvent
Output          :
Return          :   VOID
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
VOID AT_PBK_AsyncEventProcess_DeletePbkEntry(CFW_EVENT CfwEvent)
{
    UINT32 nErrorCode = 0;
    UINT8 nSim = CfwEvent.nFlag;

    if (CfwEvent.nType == 0)
    {
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, CfwEvent.nUTI, nSim);
        return;
    }
    else if (CfwEvent.nType == 0xF0)
    {
        nErrorCode = AT_SetCmeErrorCode(CfwEvent.nParam1, FALSE);
        AT_PBK_Result_Err(nErrorCode, NULL, 0, "DeletePbkEntry resp: CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        return;
    }
    else
    {
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "DeletePbkEntry resp: wrong CfwEvent.nType", CfwEvent.nUTI, nSim);
        return;
    }

}

/******************************************************************************************
Function            :   AT_PBK_AsyncEventProcess_AddPbkEntry
Description     :
Called By           :   ATS moudle
Data Accessed       :
Data Updated    :
Input           :   CFW_EVENT CfwEvent
Output          :
Return          :   VOID
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
VOID AT_PBK_AsyncEventProcess_AddPbkEntry(CFW_EVENT CfwEvent)
{
    UINT32 nErrorCode = 0;
    UINT8 nSim = CfwEvent.nFlag;

    if (CfwEvent.nType == 0)
    {
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, CfwEvent.nUTI, nSim);
        return;
    }
    else if (CfwEvent.nType == 0xF0)
    {
        nErrorCode = AT_SetCmeErrorCode(CfwEvent.nParam1, FALSE);
        AT_PBK_Result_Err(nErrorCode, NULL, 0, "AddPbkEntry resp: CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        return;
    }
    else
    {
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "AddPbkEntry resp: wrong CfwEvent.nType", CfwEvent.nUTI, nSim);
        return;
    }

}

/******************************************************************************************
Function        :   AT_PBK_AsyncEventProcess_InitSaveMaxSize
Description     :   when PBK init, send request for get PBK max number size and
                max text length, so this fuction process csw response event
                and save the value to gobal variable
Called By       :   ATS moudle
Data Accessed       :
Data Updated        :
Input           :   CFW_EVENT CfwEvent
Output          :
Return          :   VOID
Others          :   build by wangqunyang 2008.05.13
*******************************************************************************************/
VOID AT_PBK_AsyncEventProcess_InitSaveMaxSize(CFW_EVENT CfwEvent)
{
    UINT32 nErrorCode = ERR_SUCCESS;
    CFW_PBK_STRORAGE_INFO *pPbkStorageInfo = { 0 };
    UINT8 nSim = CfwEvent.nFlag;

    switch (CfwEvent.nType)
    {
    case 0:
        pPbkStorageInfo = (CFW_PBK_STRORAGE_INFO *)CfwEvent.nParam1;
        /* save the max number size and max text length
           ** whether the storage is "on", "fd","ld" or "sm", the
           ** size and text length are the same, but the max index
           ** are different */
        g_nMaxNumberSize = (pPbkStorageInfo->iNumberLen) * 2;
        g_nMaxTextSize   = pPbkStorageInfo->txtLen;

        /* here, the "SM" pbk(default pbk), max index value */
        g_nMaxSMPBKIndex = pPbkStorageInfo->index;

        // AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_CR, 0, NULL, 0, CfwEvent.nUTI);

        /****************************************************************/
        // if called AT_PBK_Result_OK() and the CMD_RC_CR is effective, the
        // cursor enter new line when AT module beginning, just kill timer here
        /****************************************************************/
        AT_KillAsyncTimer();
        break;

    case 0xF0:

        nErrorCode = AT_SetCmeErrorCode(CfwEvent.nParam1, FALSE);
        AT_PBK_Result_Err(nErrorCode, NULL, 0, "InitSaveMaxSize: CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "InitSaveMaxSize: wrong CfwEvent.nType", CfwEvent.nUTI, nSim);
        break;
    }

    return;

}

VOID AT_FDN_Init(UINT8 nSimID)
{
    SIM_PBK_PARAM *pG_Pbk_Param = NULL;

    CFW_CfgSimGetPbkParam(&pG_Pbk_Param, nSimID);
    AT_TC(g_sw_AT_PBK, "AT_FDN_Init start: ADNStatus:%d,FDNTotal:%d,RecordSize:%d!\n", pG_Pbk_Param->bADNStatus, pG_Pbk_Param->nFDNTotalNum, pG_Pbk_Param->nFDNRecordSize);
    if( pG_Pbk_Param->nFDNTotalNum > 0)
    {
        pgATFNDlist[nSimID] = AT_MALLOC( sizeof(AT_FDN_PBK_LIST) + sizeof(AT_FDN_PBK_RECORD) * (pG_Pbk_Param->nFDNTotalNum - 1));
        if(NULL == pgATFNDlist[nSimID])
        {
            AT_TC(g_sw_AT_PBK, "AT_FDN_Init ERROR:FDN List Malloc is fail !\n");
            return ;
        }
        AT_MemSet(pgATFNDlist[nSimID], 0xff, sizeof(AT_FDN_PBK_LIST) + sizeof(AT_FDN_PBK_RECORD) * (pG_Pbk_Param->nFDNTotalNum - 1));

        pgATFNDlist[nSimID]->nFDNSatus = pG_Pbk_Param->bADNStatus ? EF_DISABLE : EF_ENABLE;
        pgATFNDlist[nSimID]->nCurrentRecordIndx = 0;
        pgATFNDlist[nSimID]->nTotalRecordNum = pG_Pbk_Param->nFDNTotalNum;
        pgATFNDlist[nSimID]->nRealRecordNum = 0;

        //get fist FDN record
        if(ERR_SUCCESS != CFW_SimGetPbkEntry(CFW_PBK_SIM_FIX_DIALLING, 1, 0, nSimID) )
        {
            AT_TC(g_sw_AT_PBK, "AT_FDN_Init WARNING:Get FDN Record is fail !\n");
        }

    }
    else
    {
        AT_TC(g_sw_AT_PBK, "AT_FDN_Init WARNING:FDN Record is NULL !\n");
    }
    return ;
}

VOID AT_PBK_AsyncEventProcess_GetFDNRecord(CFW_EVENT CfwEvent)
{
    CFW_SIM_PBK_ENTRY_INFO *pRecord = NULL;
    UINT8 nSim = CfwEvent.nFlag;

    if(pgATFNDlist[nSim]->nTotalRecordNum < 1)
    {
        AT_TC(g_sw_AT_PBK, "GetFDNRecord WARNING:FDN List is NULL !\n");
    }

    AT_TC(g_sw_AT_PBK, "GetFDNRecord start: nType:0x%x,nSim:%d!\n", CfwEvent.nType, nSim);

    if (CfwEvent.nType == 0)
    {
        if(pgATFNDlist[nSim]->nRealRecordNum >= pgATFNDlist[nSim]->nTotalRecordNum)
        {
            AT_TC(g_sw_AT_PBK, "GetFDNRecord ERROR:Get FDN Record is err !\n");
            return ;
        }

        pRecord = (CFW_SIM_PBK_ENTRY_INFO *)CfwEvent.nParam1;
        if(pRecord != NULL)
        {
            AT_MemCpy(pgATFNDlist[nSim]->sRecord[pgATFNDlist[nSim]->nRealRecordNum].nName, pRecord->pFullName, pRecord->iFullNameSize);
            AT_MemCpy(pgATFNDlist[nSim]->sRecord[pgATFNDlist[nSim]->nRealRecordNum].nNumber, pRecord->pNumber, pRecord->nNumberSize);
            pgATFNDlist[nSim]->sRecord[pgATFNDlist[nSim]->nRealRecordNum].nIndex = (UINT8)(CfwEvent.nParam2 & 0x000000ff);
            pgATFNDlist[nSim]->sRecord[pgATFNDlist[nSim]->nRealRecordNum].nTpye = pRecord->nType;
            pgATFNDlist[nSim]->nRealRecordNum++;
        }
        else
        {
            AT_TC(g_sw_AT_PBK, "GetFDNRecord WARNING:FDN Record is NULL !\n");
        }
        pgATFNDlist[nSim]->nCurrentRecordIndx++;

        //get next FDN record
        if(pgATFNDlist[nSim]->nCurrentRecordIndx < pgATFNDlist[nSim]->nTotalRecordNum)
        {
            if(ERR_SUCCESS != CFW_SimGetPbkEntry(CFW_PBK_SIM_FIX_DIALLING, pgATFNDlist[nSim]->nCurrentRecordIndx + 1, 0, nSim) )
            {
                AT_TC(g_sw_AT_PBK, "GetFDNRecord WARNING:Get FDN Record is fail !\n");
            }
        }
        else
        {
            AT_TC(g_sw_AT_PBK, "GetFDNRecord :Get FDN Record is end realRecord:%d!\n", pgATFNDlist[nSim]->nRealRecordNum);
        }
    }
    else if (CfwEvent.nType == 0xF0)
    {
        AT_TC(g_sw_AT_PBK, "GetFDNRecord ERROR:Get FDN Record is fail !\n");
    }
    else
    {
        AT_TC(g_sw_AT_PBK, "GetFDNRecord ERROR:Get FDN para is fail !\n");
    }

    return ;
}
BOOL AT_FDN_CompareNumber(UINT8 *pNumber, UINT8 nLen, UINT8 nTpye,UINT8 nSim)
{
    UINT8 nIdx = 0;
    INT16 nret = 0;
    UINT8 nI = 0;
    UINT8 nNumLen = 0;
    UINT8 *pNum = pNumber;
    UINT8 *pRecNum = NULL;
    AT_TC(g_sw_AT_PBK, "CompareNumber nLen:%d ,RealNum:%d,nTpye:%d,nSim:%d!\n", nLen, pgATFNDlist[nSim]->nRealRecordNum,nTpye, nSim);
    if(NULL == pNum || 0 == nLen)
    {
        AT_TC(g_sw_AT_PBK, "CompareNumber ERROR:no input data !\n");
        return FALSE;
    }

    AT_TC_MEMBLOCK(g_sw_AT_PBK, pNum, nLen, 16);
    /*
    if( (CFW_TELNUMBER_TYPE_INTERNATIONAL == nTpye)&&(nTpye != 0xff) )
    {
        if(nLen < 2)
            return FALSE;
        nLen -= 1;
        pNum =&pNumber[1];
    }
    */
    for(nIdx = 0; nIdx < pgATFNDlist[nSim]->nRealRecordNum; nIdx++)
    {
        nNumLen = 20;
        for(nI = 0; nI < 10; nI++)
        {
            if( (pgATFNDlist[nSim]->sRecord[nIdx].nNumber[nI] & 0x0f) > 9)
            {
                nNumLen = 2 * nI;
                break;
            }
            if( ( (pgATFNDlist[nSim]->sRecord[nIdx].nNumber[nI] & 0xf0) >> 4 ) > 9)
            {
                nNumLen = 2 * nI + 1;
                break;
            }
        }
        /*
        if((CFW_TELNUMBER_TYPE_INTERNATIONAL == pgATFNDlist[nSim]->sRecord[nIdx].nTpye)&&(nTpye != 0xff))
        {
            if(nNumLen < 2)
                return FALSE;
            nNumLen -= 1;
            pRecNum =&pgATFNDlist[nSim]->sRecord[nIdx].nNumber[1];
        }
        else*/

        pRecNum = pgATFNDlist[nSim]->sRecord[nIdx].nNumber;


        if( (nNumLen % 2) == 0)
        {
            nret = AT_MemCmp(pRecNum, pNum, nNumLen / 2);

        }
        else
        {
            nret = AT_MemCmp(pRecNum, pNum, nNumLen / 2);

            if(nret == 0)
            {
                if( (pRecNum[nNumLen / 2] & 0x0f) != (pNum[nNumLen / 2 ] & 0x0f) )
                {
                    nret = -1;
                }

            }
        }
        if(0 == nret)
        {
            AT_TC(g_sw_AT_PBK, "CompareNumber  ret=%d, nIdx:%d,nNumLen:%d !\n", nret, nIdx, nNumLen);
            return TRUE;
        }
    }
    AT_TC(g_sw_AT_PBK, "CompareNumber fail ret=%d, nIdx:%d ,nNumLen:%d!\n", nret, nIdx, nNumLen);
    return FALSE;
}
VOID AT_FDN_SetStatus(UINT8 nSim)
{
    SIM_PBK_PARAM *pG_Pbk_Param = NULL;
    UINT32 nRet = ERR_SUCCESS;
    nRet = CFW_CfgSimGetPbkParam(&pG_Pbk_Param, nSim);
    if(ERR_SUCCESS == nRet)
    {
        if( pG_Pbk_Param->nFDNTotalNum > 0)
        {
            pgATFNDlist[nSim]->nFDNSatus = pG_Pbk_Param->bADNStatus ? EF_DISABLE : EF_ENABLE;
            AT_TC(g_sw_AT_PBK, "SetStatus ADNStatus:%d ,FDNSatus:%d,nSim:%d!\n", pG_Pbk_Param->bADNStatus, pgATFNDlist[nSim]->nFDNSatus, nSim);
        }
        else
        {
            AT_TC(g_sw_AT_PBK, "SetStatus ERROR no FDN number!!!\n" );
        }
    }
    else
    {
        AT_TC(g_sw_AT_PBK, "SetStatus ERROR ret=%d, nSim:%d\n", nRet, nSim);
    }
    return ;
}
VOID AT_FDN_UpdateRecord(UINT8 nIdx, UINT8 nRecordSize, UINT8 *pData, UINT8 nSim)
{
    AT_TC(g_sw_AT_PBK, "UpdateRecord start nIdx:%d ,nRecordSize:%d,nSim:%d!\n", nIdx, nRecordSize, nSim);
    AT_FDN_UPDATERECORD * pUpdateFDN = (AT_FDN_UPDATERECORD *)AT_MALLOC(SIZEOF(AT_FDN_UPDATERECORD));
    if (pUpdateFDN == NULL)
    {
        AT_TC(g_sw_AT_PBK, "UpdateRecord ERROR:malloc UpdateFDN mem is fail !\n");
        return ;
    }

    CFW_SIM_PBK_ENTRY_INFO * pGetPBKEntry = (CFW_SIM_PBK_ENTRY_INFO *)AT_MALLOC(SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE);
    if (pGetPBKEntry == NULL)
    {
        AT_TC(g_sw_AT_PBK, "UpdateRecord ERROR:malloc GetPBKEntry mem is fail !\n");
        AT_FREE(pUpdateFDN);
        return ;
    }

    AT_MemSet(pUpdateFDN, 0xff, SIZEOF(AT_FDN_UPDATERECORD));
    pUpdateFDN->nCurrRecInd = nIdx;
    pUpdateFDN->nRecordSize = nRecordSize;

    //get FDN record form android
    pGetPBKEntry->pNumber   = &pGetPBKEntry->iFullNameSize + (UINT32)SIZEOF(CFW_SIM_PBK_ENTRY_INFO);
    pGetPBKEntry->pFullName = &pGetPBKEntry->iFullNameSize + (UINT32)(SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_NUMBER_SIZE);
    UINT8 txtlength = pUpdateFDN->nRecordSize - AT_SIM_REMAINLENGTH;
    cfw_SimParsePBKRecData(pData, pGetPBKEntry, pUpdateFDN, txtlength, API_AT_GETPBK);
    AT_TC_MEMBLOCK(g_sw_AT_PBK, (UINT8 *)pGetPBKEntry, (SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE), 16);
    if((0 == pGetPBKEntry->iFullNameSize) && (0 == pGetPBKEntry->nNumberSize))
    {
        //del FDN record
        AT_FDN_DelRecord(nIdx, nSim);
    }
    else
    {
        //add or edit record
        AT_FDN_AddRecord(nIdx, pGetPBKEntry, nSim);
    }

    AT_FREE(pUpdateFDN);
    AT_FREE(pGetPBKEntry);
    return ;
}
VOID AT_FDN_AddRecord(UINT8 nIdx, CFW_SIM_PBK_ENTRY_INFO *pGetPBKEntry, UINT8 nSim)
{
    UINT8 realnum = 0;
    UINT8 i = 0;

    AT_TC(g_sw_AT_PBK, "AddRecord start nIdx:%d ,nSim:%d!\n", nIdx, nSim);

    realnum = pgATFNDlist[nSim]->nRealRecordNum;
    for(i = 0; i < realnum; i++)
    {
        if(nIdx == pgATFNDlist[nSim]->sRecord[i].nIndex)
        {
            break;
        }
    }
    AT_MemCpy(pgATFNDlist[nSim]->sRecord[i].nName, pGetPBKEntry->pFullName, pGetPBKEntry->iFullNameSize);
    AT_MemCpy(pgATFNDlist[nSim]->sRecord[i].nNumber, pGetPBKEntry->pNumber, pGetPBKEntry->nNumberSize);
    pgATFNDlist[nSim]->sRecord[i].nTpye = pGetPBKEntry->nType;
    if(i == realnum )
    {
        pgATFNDlist[nSim]->nRealRecordNum++;
        pgATFNDlist[nSim]->sRecord[i].nIndex = nIdx;
    }

    //just for testing
    if(pgATFNDlist[nSim]->nRealRecordNum > pgATFNDlist[nSim]->nTotalRecordNum)
    {
        AT_ASSERT(0);
    }
    return ;
}
VOID AT_FDN_DelRecord(UINT8 nIdx, UINT8 nSim)
{
    UINT8 realnum = 0;
    UINT8 nNext = 0;
    UINT8 i = 0;
    UINT8 j = 0;

    AT_TC(g_sw_AT_PBK, "DelRecord start nIdx:%d ,nSim:%d!\n", nIdx, nSim);
    //del FDN record
    realnum = pgATFNDlist[nSim]->nRealRecordNum;
    for(i = 0; i < realnum; i++)
    {
        if(nIdx == pgATFNDlist[nSim]->sRecord[i].nIndex)
        {
            break;
        }
    }
    if(i < realnum)
    {
        //del AT FDNlist Record
        pgATFNDlist[nSim]->nRealRecordNum--;
        for(j = i; j < realnum - 1; j++)
        {
            nNext = j + 1;
            AT_MemSet(&pgATFNDlist[nSim]->sRecord[j], 0xff, SIZEOF(AT_FDN_PBK_RECORD));
            pgATFNDlist[nSim]->sRecord[j].nIndex = pgATFNDlist[nSim]->sRecord[nNext].nIndex;
            pgATFNDlist[nSim]->sRecord[j].nTpye  = pgATFNDlist[nSim]->sRecord[nNext].nTpye;
            AT_MemCpy(pgATFNDlist[nSim]->sRecord[j].nName, pgATFNDlist[nSim]->sRecord[nNext].nName, AT_FDN_NAME_SIZE);
            AT_MemCpy(pgATFNDlist[nSim]->sRecord[j].nNumber, pgATFNDlist[nSim]->sRecord[nNext].nNumber, AT_FDN_NUMBER_SIZE);
        }
        //write last record to zero
        AT_MemSet(&pgATFNDlist[nSim]->sRecord[realnum - 1], 0xff, SIZEOF(AT_FDN_PBK_RECORD));

    }
    return ;
}

// ///////////////////////////////////////////////////////////////////////////////////////////////
// The end of the file
// ///////////////////////////////////////////////////////////////////////////////////////////////

