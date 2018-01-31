
#ifdef AT_LWM2M_SUPPORT

#include "at_common.h"

#include "at_cmd_lwm2m.h"

static UINT8 nDlc = 0;

static VOID AT_LWM2M_AsyncEventProcess(COS_EVENT *pEvent)
{
    char **argv = (char **) pEvent->nParam2;
    UINT8 uParamCount = (UINT8) pEvent->nParam1;
    INT32 iRcvRet = (INT32) pEvent->nParam3;

    switch (pEvent->nEventId)
    {
        case EV_CFW_LWM2M_COMMAND_RSP:
            if (iRcvRet == 0) {
                AT_TCPIP_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, nDlc);
            } else {
                AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, nDlc);
            }
            break;
        case EV_CFW_LWM2M_DISCONN_RSP:
            for (int i = 0; i < uParamCount; i++) {
                if (argv[i]) AT_FREE(argv[i]);
            }
            AT_FREE(argv);
            break;
        default:
            break;
    }
    COS_FREE(pEvent);
}

VOID CFW_LWM2MPostEvent(UINT32 nEventId, UINT32 nParam1, UINT32 nParam2, UINT32 nParam3)
{    
    COS_EVENT *ev = COS_MALLOC(sizeof(COS_EVENT));
    ev->nEventId = nEventId;
    ev->nParam1  = nParam1;
    ev->nParam2  = nParam2;
    ev->nParam3 =  nParam3;
    COS_TaskCallback(COS_GetDefaultATTaskHandle(),AT_LWM2M_AsyncEventProcess,ev);
}

void process_lwm2m_return_data(void *param, const unsigned char *data, size_t len)
{
    AT_CMD_ENGINE_T * engine = (AT_CMD_ENGINE_T *)param;
    AT_AsyncWriteUart(engine, data,len);
}

void report_at_command_result(int success)
{
    lwm2m_set_report_data_handler(NULL,NULL);
    lwm2m_set_at_command_handler(NULL);
    CFW_LWM2MPostEvent(EV_CFW_LWM2M_COMMAND_RSP, 0, 0, success);
}

VOID AT_LWM2M_CmdFunc_START(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount = 0;
    UINT32 iResult = 0;
    UINT8 pCmdParams[LWM2M_STRING_SIZE] = { 0 };
    UINT16 uSize = 0;
    char **argv = NULL;

    AT_TC(g_sw_AT_NW, "AT_LWM2M_CmdFunc_CONN pParam->nDLCI= %d", pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_AT_NW, "AT_LWM2M_CmdFunc_CONN: pParam is NULL");
        AT_TCPIP_Result_Err(ERR_AT_CMS_INVALID_LEN, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
    nDlc = pParam->nDLCI;
    switch (pParam->iType)
    {
        case AT_CMD_SET:
        {
            iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
            AT_TC(g_sw_AT_NW, "AT_LWM2M_CmdFunc_CONN uParamCount= %d", uParamCount);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_AT_NW, "AT_LWM2M_CmdFunc_CONN parameters error\n");
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }

            argv = AT_MALLOC(uParamCount * 4);
            memset(argv, 0, uParamCount * 4);

            for (int i = 0; i < uParamCount; i++) {
                uSize = SIZEOF(pCmdParams);
                iResult = AT_Util_GetParaWithRule(pParam->pPara, i, AT_UTIL_PARA_TYPE_STRING, pCmdParams, &uSize);
                AT_TC(g_sw_AT_NW, "AT_LWM2M_CmdFunc_CONN... ... pCmdParams = %s", pCmdParams);
                if (iResult != ERR_SUCCESS)
                {
                    AT_TC(g_sw_AT_NW, "AT_LWM2M_CmdFunc_CONN get parameter error\n");
                    AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    goto release_mem;
                }
                argv[i] = AT_MALLOC(uSize + 1);
                strcpy(argv[i], pCmdParams);
            }

            if (isLWM2MRegistered())
            {
                AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
            }
            else
            {
                lwm2m_set_report_data_handler(process_lwm2m_return_data,pParam->engine);
                lwm2m_set_at_command_handler(report_at_command_result);
                if (lunch_lwm2m_client(uParamCount, argv) == TRUE)
                {
                    AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
                }
                else
                {
                    lwm2m_set_report_data_handler(NULL,NULL);
                    lwm2m_set_at_command_handler(NULL);
                    at_CmdRespInfoText(pParam->engine,"Lwip task is already running, please wait\n");
                    AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    goto release_mem;
                }
            }

            break;
        }
    }
    return;
release_mem:
    for (int i = 0; i < uParamCount; i++) {
        if (argv[i]) AT_FREE(argv[i]);
    }
    AT_FREE(argv);
}

VOID AT_LWM2M_CmdFunc_COMMAND(AT_CMD_PARA *pParam)
{
    INT32 iResult = 0;
    UINT8 uParamCount = 0;
    UINT16 uSize = 0;
    UINT32 err;
    UINT8 pCmdParams[LWM2M_PARAM_SIZE] = { 0 };
    char *argv = NULL;

    AT_TC(g_sw_AT_NW, "AT_LWM2M_CmdFunc_COMMAND pParam->nDLCI= %d", pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_AT_NW, "AT_LWM2M_CmdFunc_COMMAND: pParam is NULL");
        AT_TCPIP_Result_Err(ERR_AT_CMS_INVALID_LEN, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
    nDlc = pParam->nDLCI;
    switch (pParam->iType)
    {
        case AT_CMD_SET:
        {
            iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
            if (iResult != ERR_SUCCESS || uParamCount > 3)
            {
                AT_TC(g_sw_AT_NW, "AT_LWM2M_CmdFunc_COMMAND: pParam is NULL or too much param");
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }

            argv = AT_MALLOC(LWM2M_STRING_SIZE);
            memset(argv, 0, LWM2M_STRING_SIZE);
            for (int i = 0; i < uParamCount; i++) {
                uSize = LWM2M_PARAM_SIZE;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, i, AT_UTIL_PARA_TYPE_STRING, pCmdParams, &uSize);
                AT_TC(g_sw_AT_NW, "AT_LWM2M_CmdFunc_COMMAND... ... pCmdParams = %s", pCmdParams);
                if (iResult != ERR_SUCCESS)
                {
                    AT_TC(g_sw_AT_NW, "AT_LWM2M_CmdFunc_COMMAND get parameter error\n");
                    AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    goto release_mem;
                }

                if (i == 0)
                {
                    snprintf(argv + strlen(argv), LWM2M_STRING_SIZE, "%s", (char *) pCmdParams);
                }
                else
                {
                    snprintf(argv + strlen(argv), LWM2M_STRING_SIZE, " %s", (char *) pCmdParams);
                }
            }
            snprintf(argv + strlen(argv), LWM2M_STRING_SIZE, "\n");

            if (isLWM2MRegistered())
            {
                lwm2m_set_report_data_handler(process_lwm2m_return_data,pParam->engine);
                lwm2m_set_at_command_handler(report_at_command_result);
                excute_lwm2m_command(argv);
                AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
            }
            else
            {
                AT_TC(g_sw_GPRS, "AT_LWM2M_CmdFunc_COMMAND lwm2m is not ready");
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            }

            AT_FREE(argv);
            break;
        }
    }
    return;
release_mem:
    AT_FREE(argv);
}

#endif
