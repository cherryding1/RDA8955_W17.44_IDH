#ifdef AT_COAP_SUPPORT

#include "stdio.h"
#include "at.h"
#include "cfw.h"
#include "at_module.h"
#include "at_cmd_gprs.h"
#include "at_cmd_coap.h"
#include "base_prv.h"
#include "at_cfg.h"
#include "at_common.h"
#include "at_cmd_nw.h"
#include "sockets.h"

static UINT8 nDlc = 0;

static VOID AT_COAP_AsyncEventProcess(COS_EVENT *pEvent)
{
    char **argv = (char **)pEvent->nParam2;
    UINT8 uParamCount = (UINT8)pEvent->nParam1;
    INT32 iRcvRet = (INT32)pEvent->nParam3;

    sxs_fprintf(TSTDOUT, "AT_COAP_AsyncEventProcess() - uParamCount = %d,iRcvRet=%d\n", uParamCount,iRcvRet);
    switch (pEvent->nEventId)
    {
        case EV_CFW_COAP_CLIENT_RSP:
            for (int i=0;i<uParamCount;i++) {
                if (argv[i]) AT_FREE(argv[i]);
            }
            AT_FREE(argv);
            if (iRcvRet <0)
                AT_TCPIP_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, nDlc);
            else
                AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, nDlc);
        break;
    }
    COS_FREE(pEvent);
}

VOID CFW_CoapPostEvent(UINT32 nEventId, UINT32 nParam1, UINT32 nParam2, UINT32 nParam3)
{    
    COS_EVENT *ev = COS_MALLOC(sizeof(COS_EVENT));
    ev->nEventId = nEventId;
    ev->nParam1  = nParam1;
    ev->nParam2  = nParam2;
    ev->nParam3 =  nParam3;
    COS_TaskCallback(COS_GetDefaultATTaskHandle(),AT_COAP_AsyncEventProcess,ev);
}

void process_return_data(void *param, const unsigned char *data, size_t len){
    UINT8 OutStr[100];    
    AT_CMD_ENGINE_T * engine = (AT_CMD_ENGINE_T *)param;
    AT_Sprintf(OutStr, "+COAP(%d):%s",len,data);
    AT_AsyncWriteUart(engine, OutStr, AT_StrLen(OutStr));
    AT_AsyncWriteUart(engine, "\r\n", AT_StrLen("\r\n"));
}

/*****************************************************************************
* Name:         AT_COAP_CmdFunc_START
* Description: Start up COAP transfer
* Parameter:  AT_CMD_PARA *pParam
* Return:       VOID
* Remark:      n/a
* Author:       Qidd
* Data:          2017-3-3
******************************************************************************/
VOID AT_COAP_CmdFunc_START(AT_CMD_PARA *pParam)
{
    INT32 iResult                       = 0;
    UINT16 uSize                         = 0;
    UINT8 uParamCount                   = 0;
    UINT8 pCmdParams[60]  = { 0 };
    UINT8 PromptMsg[8] = { 0 };
    UINT8 uIndex                        = 0;
    UINT8 nCID = 0;
    UINT32 filesize = 0;
    char **argv = NULL;
    INT8 f_paramIndex = -1;
    INT8 b_paramIndex = -1;
    int wait_seconds = 95;

    AT_TC(g_sw_AT_NW, "AT_COAP_CmdFunc_START... ... pParam->nDLCI= %d", pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_AT_NW, "AT_COAP_CmdFunc_START: pParam is NULL");
        AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }

    if (AT_CMD_SET == pParam->iType)
    {
            iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
            AT_TC(g_sw_AT_NW, "AT_COAP_CmdFunc_START... ... uParamCount= %d", uParamCount);
            // need at least <CID> <method> <IP address> <port>
            if (iResult != ERR_SUCCESS || uParamCount > 15 )
            {
                AT_TC(g_sw_AT_NW,
                      "exe       in AT_COAP_CmdFunc_START, parameters error or parameter count > 15\n");
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }
            argv = AT_MALLOC(15*4);
            memset(argv,0,15*4);
            for (int i=0;i<uParamCount;i++) {
                
                uSize   = SIZEOF(pCmdParams);
                iResult = AT_Util_GetParaWithRule(pParam->pPara, i, AT_UTIL_PARA_TYPE_STRING, pCmdParams, &uSize);
                AT_TC(g_sw_AT_NW, "AT_COAP_CmdFunc_START... ... pCmdParams= %s", pCmdParams);
                if (iResult != ERR_SUCCESS)
                {
                    AT_TC(g_sw_AT_NW, "exe       in AT_COAP_CmdFunc_START, get parameter error1\n");
                    AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    goto release_mem;
                }
                argv[i] = AT_MALLOC(uSize+1);
                strcpy(argv[i],pCmdParams);
                if (strcmp(pCmdParams,"-f")==0)
                    f_paramIndex = i;
                if (strcmp(pCmdParams,"-B")==0)
                    b_paramIndex = i;
            }

            if (b_paramIndex != -1 && atoi(argv[b_paramIndex+1]) > 90)
                wait_seconds = atoi(argv[b_paramIndex+1])+5;

            if (f_paramIndex != -1 && strcmp(argv[f_paramIndex+1],"-")==0) {
                if (!pParam->iExDataLen)
                {
                    at_CmdRespOutputPrompt(pParam->engine);
                    
                    AT_TC(g_sw_TCPIP, "exe in AT_COAP_CmdFunc_START(), auto sending timer:5 seconds");
                    AT_TCPIP_Result_OK(CMD_FUNC_WAIT_IP_DATA, CMD_RC_OK, 5, NULL, 0, pParam->nDLCI);
                    
                    AT_CMD_ENGINE_T *th = at_CmdGetByChannel(pParam->nDLCI);
                    at_CmdSetPromptMode(th);
                    goto release_mem;
                } else {
                    
                    if (pParam->pExData[pParam->iExDataLen - 1] == 27)
                    {
                        AT_TC(g_sw_TCPIP, "end with esc, cancel send");
                        AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
                        goto release_mem;
                    }
                    else if (pParam->pExData[pParam->iExDataLen - 1] != 26)
                    {
                        AT_TC(g_sw_TCPIP, "not end with ctl+z, err happen");
                        AT_TCPIP_Result_Err(ERR_AT_CMS_INVALID_LEN, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                        goto release_mem;
                    }
                    if (!coap_client_set_payload(pParam->pExData,pParam->iExDataLen - 1,0)) {
                        AT_TC(g_sw_TCPIP, "not end with ctl+z, err happen");
                        AT_TCPIP_Result_Err(ERR_AT_CMS_MEM_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                        goto release_mem;
                    }
                }
            } else if (f_paramIndex != -1) {
                filesize = atoi(argv[f_paramIndex+1]);
                AT_TC(g_sw_TCPIP, "filesize is %d\n",filesize);
                if (!coap_client_set_payload(NULL,filesize,1)) {
                    AT_TC(g_sw_TCPIP, "not end with ctl+z, err happen");
                    AT_TCPIP_Result_Err(ERR_AT_CMS_MEM_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    goto release_mem;
                }
            }
            coap_client_set_returnhander(process_return_data,pParam->engine);
            if (lunch_coap_client(uParamCount,argv) == TRUE)
            {
                AT_TC(g_sw_TCPIP, "AT_COAP_CmdFunc_START wait_seconds = %d", wait_seconds);
                AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, wait_seconds, NULL, 0, pParam->nDLCI);
            }
            else
            {
                AT_TC(g_sw_TCPIP, "AT_COAP_CmdFunc_START Coap is Running, please wait\n");
                AT_TCPIP_Result_Err(ERR_AT_CMS_MEM_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                goto release_mem;
            }
    }
    return;
release_mem:
    for (int i=0;i<uParamCount;i++) {
        if (argv[i]) AT_FREE(argv[i]);
    }
    AT_FREE(argv);
}

#endif
