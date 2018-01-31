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


#ifdef AT_MIPL_SUPPORT

#include "at.h"
#include <drv_prv.h>
#include "at_module.h"
#include "at_cmd_special_mipl.h"
#include "at_cfg.h"
#include "sockets.h"

#define LWM2M_STRING_SIZE 60
#define LWM2M_PARAM_SIZE 20

static UINT8 nDlc = 0;

static VOID AT_MIPL_LWM2M_AsyncEventProcess(COS_EVENT *pEvent)
{
    char **argv = (char **) pEvent->nParam2;
    UINT8 uParamCount = (UINT8) pEvent->nParam1;
    INT32 iRcvRet = (INT32) pEvent->nParam3;

    switch (pEvent->nEventId)
    {
        case EV_CFW_MIPL_LWM2M_COMMAND_RSP:
            if (iRcvRet) {
                AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, nDlc);
            }
            else
            {
                AT_TCPIP_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, nDlc);
            }
            break;
        case EV_CFW_MIPL_LWM2M_DISCONN_RSP:
            break;
        default:
            break;
    }
    COS_FREE(pEvent);
}


VOID CFW_MIPL_LWM2MPostEvent(UINT32 nEventId, UINT32 nParam1, UINT32 nParam2, UINT32 nParam3)
{
    COS_EVENT *ev = COS_MALLOC(sizeof(COS_EVENT));
    ev->nEventId = nEventId;
    ev->nParam1  = nParam1;
    ev->nParam2  = nParam2;
    ev->nParam3 =  nParam3;
    COS_TaskCallback(COS_GetDefaultATTaskHandle(), AT_MIPL_LWM2M_AsyncEventProcess, ev);
}

void process_mipl_lwm2m_return_data(void *param, const unsigned char *data, size_t len)
{
    AT_CMD_ENGINE_T * engine = (AT_CMD_ENGINE_T *)param;
    AT_AsyncWriteUart(engine, data,len);
}

void report_mipl_at_command_result(int success)
{
    lwm2m_set_report_data_handler(NULL,NULL);
    lwm2m_set_at_command_handler(NULL);
    CFW_MIPL_LWM2MPostEvent(EV_CFW_MIPL_LWM2M_COMMAND_RSP, 0, 0, success);
}

int parseMiplType(char *cmdStr, MIPL_TYPE_T type)
{
    switch (type)
    {
        case MIPL_READ:
            AT_Sprintf(cmdStr, "read");
            break;
        case MIPL_WRITE:
            AT_Sprintf(cmdStr, "change");
            break;
        case MIPL_EXECUTE:
            AT_Sprintf(cmdStr, "exec");
            break;
        case MIPL_OBSERVE:
            AT_Sprintf(cmdStr, "observe");
            break;
        default:
            return 0;;
    }

    return 1;
}

VOID AT_MIP_CmdFunc_MIPLCR(AT_CMD_PARA *pParam)
{
    if (NULL == pParam)
    {
        AT_TC(g_sw_AT_NW, "AT_MIP_CmdFunc_MIPLCR: pParam is NULL");
        goto MIPLCR_ERROR;
    }

    UINT8 config[2048]      = { 0x00, };

    switch (pParam->iType)
    {
        case AT_CMD_SET:
        {
            UINT32 iResult = 0;
            UINT16 uSize = 0;
            UINT8 uNumOfParam       = 0;
            UINT16 uLen             = 0;
            UINT8 index = 0;
            UINT8 idx = 0;
            UINT8 ref;
            UINT16 size;

            if (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uNumOfParam))
            {
                AT_TC(g_sw_AT_NW, "AT_MIP_CmdFunc_MIPLCR: AT_Util_GetParaCount error");
                goto MIPLCR_ERROR;
            }

            AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLCR NumOfParam %d", uNumOfParam);
            if (uNumOfParam != 3)
            {
                AT_TC(g_sw_AT_NW, "AT_MIP_CmdFunc_MIPLCR: uNumOfParam is not equal to 3");
                goto MIPLCR_ERROR;
            }

            uLen = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_UINT8, &size, &uLen);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLCR: get ref error");
                goto MIPLCR_ERROR;
            }

            idx++;
            uLen = 2048;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_STRING, &config, &uLen);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLCR: get arrBuf error");
                goto MIPLCR_ERROR;
            }

            idx++;
            uLen = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_UINT8, &index, &uLen);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLCR: get size error");
                goto MIPLCR_ERROR;
            }

            ref = mipl_ConfigKit(index, config, size);
            AT_FREE(config);
            if (ref > 0)
            {
                UINT8 aucBuffer[40] = { 0 };
                AT_Sprintf(aucBuffer, "+MIPLCR:%d", ref);

                at_CmdRespInfoText(pParam->engine, aucBuffer);
                at_CmdRespOK(pParam->engine);
            }
            else
            {
                AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLCR: ref exceed the maximum");
                goto MIPLDEL_EXE_ERROR;
            }

            break;
        }
    }
    return;
MIPLCR_ERROR:
    if (config)
    {
        AT_FREE(config);
    }

    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
MIPLDEL_EXE_ERROR:
    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
}

VOID AT_MIP_CmdFunc_MIPLDEL(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    INT32 iRet = ERR_SUCCESS;

    if (NULL == pParam)
    {
        AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLDEL pParam is NULL");
        goto MIPLDEL_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        if (NULL == Param->pPara)
        {
            AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLDEL pParam->pPara is NULL");
            goto MIPLDEL_ERROR;
        }

        switch (Param->iType)
        {
            case AT_CMD_SET:
            {
                UINT8 uNumOfParam       = 0;
                UINT8 ref = 0;
                UINT16 uLen;
                UINT32 iResult;
                UINT8 result;

                if (ERR_SUCCESS != AT_Util_GetParaCount(Param->pPara, &uNumOfParam))
                {
                    AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLDEL AT_Util_GetParaCount error");
                    goto MIPLDEL_ERROR;
                }

                AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLDEL NumOfParam %d", uNumOfParam);
                if (uNumOfParam != 1)
                {
                    AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLDEL uNumOfParam is error");
                    goto MIPLDEL_ERROR;
                }

                uLen = 1;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &ref, &uLen);
                if (iResult != ERR_SUCCESS)
                {
                    AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLDEL: get ref error");
                    goto MIPLDEL_ERROR;
                }

                // ref exists and not be registered
                if (isRefExist(ref) && !isRefRegistered(ref))
                {
                    result = mipl_DeleteREF(ref);
                }
                else
                {
                    AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLDEL ref doesn't exist or has been registered");
                    goto MIPLDEL_EXE_ERROR;
                }

                if (result)
                {
                    // need to free the suite information
                    mipl_freeConfigByRef(ref);

                    UINT8 aucBuffer[40] = { 0 };
                    AT_Sprintf(aucBuffer, "+MIPLDEL:%d", ref);

                    at_CmdRespInfoText(pParam->engine, aucBuffer);
                    at_CmdRespOK(pParam->engine);
                }
                else
                {
                    AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLDEL: delete suite(%d) fail", ref);
                    goto MIPLDEL_EXE_ERROR;
                }
                return;
            }
            default:
            {
                goto MIPLDEL_ERROR;
            }
        }
    }

MIPLDEL_ERROR:
    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
MIPLDEL_EXE_ERROR:
    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
}

VOID AT_MIP_CmdFunc_MIPLOPEN(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    INT32 iRet = ERR_SUCCESS;

    if(NULL == pParam)
    {
        AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLOPEN pParam is NULL");
        goto iOpen_ERROR;
    }
    else
    {
        nDlc = pParam->nDLCI;

        AT_CMD_PARA *Param = pParam;
        if (NULL == Param->pPara)
        {
            AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLOPEN Param->pPara is NULL");
            goto iOpen_ERROR;
        }

        AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLOPEN iOpen Param->iType %d", Param->iType);
        switch (Param->iType)
        {
            case AT_CMD_SET:
            {
                UINT8 uNumOfParam       = 0;
                UINT16 uLen              = 0;
                UINT8 ref = 0;
                UINT32 iResult = 0;

                if (ERR_SUCCESS != AT_Util_GetParaCount(Param->pPara, &uNumOfParam))
                {
                    AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLOPEN AT_Util_GetParaCount error");
                    goto iOpen_ERROR;
                }

                AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLOPEN NumOfParam %d", uNumOfParam);
                if (uNumOfParam != 1)
                {
                    AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLOPEN uNumOfParam is not equal to 1");
                    goto iOpen_ERROR;
                }

                uLen = 1;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &ref, &uLen);
                if (iResult != ERR_SUCCESS)
                {
                    AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLOPEN: get ref error");
                    goto iOpen_ERROR;
                }

                // the ref suite has been regisered
                if (isRefRegistered(ref) && isLWM2MRegistered())
                {
                    AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                }
                // ref exists and is not be registered
                else if (isRefExist(ref) && !isLWM2MRegistered())
                {
                    lwm2m_set_report_data_handler(process_mipl_lwm2m_return_data,pParam->engine);
                    lwm2m_set_at_command_handler(report_mipl_at_command_result);
                    if (nbiot_lwm2m_register(ref) == TRUE)
                    {
                        AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
                    }
                    // lwip_app_task is running
                    else
                    {
                        lwm2m_set_report_data_handler(NULL,NULL);
                        lwm2m_set_at_command_handler(NULL);
                        goto iOpen_EXE_ERROR;
                    }
                }
                else
                {
                    AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLOPEN: ref is valid");
                    goto iOpen_EXE_ERROR;
                }
                return;
            }
            default:
            {
                goto iOpen_ERROR;
            }
        }
    }
iOpen_ERROR:
    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
iOpen_EXE_ERROR:
    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
}

VOID AT_MIP_CmdFunc_MIPLCLOSE(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    INT32 iRet = ERR_SUCCESS;

    if(NULL == pParam)
    {
        AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLCLOSE: pParam is NULL");
        goto MIPLCLOSE_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        if (NULL == Param->pPara)
        {
            AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLCLOSE: Param->pPara is NULL");
            goto MIPLCLOSE_ERROR;
        }

        AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLCLOSE iOpen Param->iType %d", Param->iType);
        switch (Param->iType)
        {
            case AT_CMD_SET:
            {
                UINT8 uNumOfParam       = 0;
                UINT16 uLen             = 0;
                UINT8 ref = 0;
                UINT16 iResult;

                if (ERR_SUCCESS != AT_Util_GetParaCount(Param->pPara, &uNumOfParam))
                {
                    AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLCLOSE AT_Util_GetParaCount error");
                    goto MIPLCLOSE_ERROR;
                }

                AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLCLOSE NumOfParam %d", uNumOfParam);
                if (uNumOfParam != 1)
                {
                    AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLCLOSE uNumOfParam is not equal to 1");
                    goto MIPLCLOSE_ERROR;
                }

                uLen = 1;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &ref, &uLen);
                if (iResult != ERR_SUCCESS)
                {
                    AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLCLOSE: get ref error");
                    goto MIPLCLOSE_ERROR;
                }

                // the ref suite is being registered
                if (isRefRegistered(ref) && isLWM2MRegistered())
                {
                    lwm2m_set_report_data_handler(process_mipl_lwm2m_return_data,pParam->engine);
                    lwm2m_set_at_command_handler(report_mipl_at_command_result);
                    mipl_unregister(ref);
                    AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
                }
                else
                {
                    AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLCLOSE: wrong ref or lwm2m is not registered");
                    goto MIPLCLOSE_EXE_ERROR;
                }
                return;
            }
            default:
            {
                goto MIPLCLOSE_ERROR;
            }
        }
    }
    return;
MIPLCLOSE_ERROR:
    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
MIPLCLOSE_EXE_ERROR:
    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
}

VOID AT_MIP_CmdFunc_MIPLCHG(AT_CMD_PARA *pParam)
{
    INT32 iResult = 0;
    UINT8 uParamCount = 0;
    UINT32 err;
    UINT8 pCmdParams[LWM2M_PARAM_SIZE] = { 0 };
    UINT32 id;
    char *argv = NULL;

    AT_TC(g_sw_AT_NW, "AT_MIP_CmdFunc_MIPLCHG pParam->nDLCI= %d", pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_AT_NW, "AT_MIP_CmdFunc_MIPLCHG: pParam is NULL");
        goto release_mem;
    }

    switch (pParam->iType)
    {
        case AT_CMD_SET:
        {
            UINT8 ref = 0;
            UINT8 uLen              = 0;
            UINT8 arrBuf[5]         = {0x00,};

            iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
            if (iResult != ERR_SUCCESS || uParamCount < 4 || uParamCount > 5)
            {
                AT_TC(g_sw_AT_NW, "AT_MIP_CmdFunc_MIPLCHG: pParam is NULL or too much param");
                goto release_mem;
            }

            uLen = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &ref, &uLen);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLCHG AT_Util_GetPara iResult Err 0x%x", iResult);
                goto release_mem;
            }

            argv = AT_MALLOC(LWM2M_STRING_SIZE);
            memset(argv, 0, LWM2M_STRING_SIZE);
            snprintf(argv + strlen(argv), LWM2M_STRING_SIZE, "change ", pCmdParams);
            for (int i = 1; i < uParamCount; i++)
            {
                // when we need to this value, it should be add to this at command
                if (i == 4)
                {
                    uLen = LWM2M_PARAM_SIZE;
                    iResult = AT_Util_GetParaWithRule(pParam->pPara, i, AT_UTIL_PARA_TYPE_STRING, &pCmdParams, &uLen);
                }
                else
                {
                    uLen = sizeof(UINT32);
                    iResult = AT_Util_GetParaWithRule(pParam->pPara, i, AT_UTIL_PARA_TYPE_UINT32, &id, &uLen);
                }

                if (iResult != ERR_SUCCESS)
                {
                    AT_TC(g_sw_AT_NW, "AT_MIP_CmdFunc_MIPLCHG get parameter error\n");
                    goto release_mem;
                }

                if (i == 4)
                {
                    snprintf(argv + strlen(argv), LWM2M_STRING_SIZE, " %s", pCmdParams);
                }
                else
                {
                    snprintf(argv + strlen(argv), LWM2M_STRING_SIZE, "/%d", id);
                }
            }
            snprintf(argv + strlen(argv), LWM2M_STRING_SIZE, "\n");

            if (isRefRegistered(ref) && isLWM2MRegistered())
            {
                lwm2m_set_report_data_handler(process_mipl_lwm2m_return_data,pParam->engine);
                lwm2m_set_at_command_handler(report_mipl_at_command_result);
                excute_lwm2m_command(argv);
            }
            else
            {
                AT_TC(g_sw_GPRS, "AT_MIP_CmdFunc_MIPLCHG lwm2m is not ready");
                goto release_exe_mem;
            }

            AT_FREE(argv);
            break;
        }
    }
    return;
release_mem:
    AT_FREE(argv);
    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
release_exe_mem:
    AT_FREE(argv);
    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
}

VOID AT_MIP_CmdFunc_MIPLSND(AT_CMD_PARA *pParam)
{
    INT32 iResult = 0;
    UINT8 uParamCount = 0;
    UINT16 uSize = 0;
    UINT32 err;
    UINT8 pCmdParams[LWM2M_PARAM_SIZE] = { 0 };
    char *argv = NULL;
    UINT8 ref = 0;
    UINT32 param;

    AT_TC(g_sw_AT_NW, "AT_MIP_CmdFunc_MIPLSND pParam->nDLCI= %d", pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_AT_NW, "AT_MIP_CmdFunc_MIPLSND: pParam is NULL");
        goto release_mem;
    }

    switch (pParam->iType)
    {
        case AT_CMD_SET:
        {
            iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
            if (iResult != ERR_SUCCESS || uParamCount < 5 || uParamCount > 6)
            {
                AT_TC(g_sw_AT_NW, "AT_MIP_CmdFunc_MIPLSND: pParam is NULL or too much param");
                goto release_mem;
            }

            uSize = 1;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &ref, &uSize);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLSND get ref Err 0x%x", iResult);
                goto release_mem;
            }

            if (isRefRegistered(ref) && isLWM2MRegistered())
            {
                argv = AT_MALLOC(LWM2M_STRING_SIZE);
                memset(argv, 0, LWM2M_STRING_SIZE);
                for (int i = 1; i < uParamCount; i++)
                {
                    uSize = sizeof(UINT32);
                    iResult = AT_Util_GetParaWithRule(pParam->pPara, i, AT_UTIL_PARA_TYPE_UINT32, &param, &uSize);
                    if (i == 1)
                    {
                        int success = parseMiplType(pCmdParams, param);
                        if (success)
                        {
                            snprintf(argv + strlen(argv), LWM2M_STRING_SIZE, "%s ", pCmdParams);
                        }
                        else
                        {
                            AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLSND error mipl type");
                            goto release_mem;
                        }
                    }
                    else if (i == 5)
                    {
                        snprintf(argv + strlen(argv), LWM2M_STRING_SIZE, " %d", param);
                    }
                    else
                    {
                        snprintf(argv + strlen(argv), LWM2M_STRING_SIZE, "/%d", param);
                    }
                }
                snprintf(argv + strlen(argv), LWM2M_STRING_SIZE, "\n");

                lwm2m_set_report_data_handler(process_mipl_lwm2m_return_data,pParam->engine);
                lwm2m_set_at_command_handler(report_mipl_at_command_result);
                excute_lwm2m_command(argv);
                AT_FREE(argv);

                AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
            }
            else
            {
                AT_TC(g_sw_GPRS, "AT_MIP_CmdFunc_MIPLSND ref is wrong or ref is being registered");
                goto release_exe_mem;
            }

            AT_FREE(argv);
            break;
        }
    }
    return;
release_mem:
    AT_FREE(argv);
    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
release_exe_mem:
    AT_FREE(argv);
    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
}

VOID AT_MIP_CmdFunc_MIPLRCV(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    INT32 iRet = ERR_SUCCESS;

    if(NULL == pParam)
    {
        goto iOpen_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        if (NULL == Param->pPara)
        {
            goto iOpen_ERROR;
        }

        AT_TC(g_sw_SPEC, "iOpen Param->iType %d", Param->iType);
        switch (Param->iType)
        {
        case AT_CMD_SET:
        {
            UINT8 uNumOfParam       = 0;
            UINT8 uLen              = 0;
            UINT8 arrBuf[5]         = {0x00,};
            U16 ref = -1; 

            if (ERR_SUCCESS != AT_Util_GetParaCount(Param->pPara, &uNumOfParam))
            {
                goto iOpen_ERROR;
            }

            AT_TC(g_sw_SPEC, "NumOfParam %d", uNumOfParam);
            if (uNumOfParam != 1)
            {
                goto iOpen_ERROR;
            }

            uLen = 5;
            if (ERR_SUCCESS != (iRet = AT_Util_GetPara(Param->pPara, 0, arrBuf, &uLen)))
            {
                AT_TC(g_sw_SPEC, "AT_Util_GetPara iRet Err 0x%x", iRet);
                goto iOpen_ERROR;
            }
            ref = AT_StrAToI(arrBuf);
            //mipl_receive(ref);

            pResult = AT_CreateRC(CMD_FUNC_SUCC, CMD_RC_OK,
                                  CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
            if(NULL != pResult)
            {
                AT_Notify2ATM(pResult, pParam->nDLCI);
                AT_FREE(pResult);
                pResult = NULL;
            }
            return;
        }

        default:
        {
            goto iOpen_ERROR;
        }
        }
    }

iOpen_ERROR:

    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR,
                          CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
    if(NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }
    return;
}

VOID AT_MIP_CmdFunc_MIPLNFY(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    INT32 iRet = ERR_SUCCESS;

    if(NULL == pParam)
    {
        goto MIPLNFY_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        if (NULL == Param->pPara)
        {
            goto MIPLNFY_ERROR;
        }

        AT_TC(g_sw_SPEC, "iOpen Param->iType %d", Param->iType);
        switch (Param->iType)
        {
        case AT_CMD_SET:
        {
            UINT8 uNumOfParam       = 0;
            UINT8 uLen              = 0;
            UINT8 arrBuf[5]         = {0x00,};
            U16 ref = -1; 
            int type = -1;

            if (ERR_SUCCESS != AT_Util_GetParaCount(Param->pPara, &uNumOfParam))
            {
                goto MIPLNFY_ERROR;
            }

            AT_TC(g_sw_SPEC, "NumOfParam %d", uNumOfParam);
            if (uNumOfParam != 1)
            {
                goto MIPLNFY_ERROR;
            }

            uLen = 5;
            if (ERR_SUCCESS != (iRet = AT_Util_GetPara(Param->pPara, 0, arrBuf, &uLen)))
            {
                AT_TC(g_sw_SPEC, "AT_Util_GetPara iRet Err 0x%x", iRet);
                goto MIPLNFY_ERROR;
            }
            ref = AT_StrAToI(arrBuf);

            uLen = 5;
            if (ERR_SUCCESS != (iRet = AT_Util_GetPara(Param->pPara, 2, arrBuf, &uLen)))
            {
                AT_TC(g_sw_SPEC, "AT_Util_GetPara iRet Err 0x%x", iRet);
                goto MIPLNFY_ERROR;
            }

            I_AM_HERE();

            type = AT_StrAToI(arrBuf);

            //mipl_notify(ref, type);

            pResult = AT_CreateRC(CMD_FUNC_SUCC, CMD_RC_OK,
                                  CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
            if(NULL != pResult)
            {
                AT_Notify2ATM(pResult, pParam->nDLCI);
                AT_FREE(pResult);
                pResult = NULL;
            }
            return;
        }

        default:
        {
            goto MIPLNFY_ERROR;
        }
        }
    }

MIPLNFY_ERROR:

    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR,
                          CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
    if(NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }
    return;
}

VOID AT_MIP_CmdFunc_MIPLVER(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    INT32 iRet = ERR_SUCCESS;
    UINT8 ver = 0;
    UINT8 arrBuf[5]         = {0x00,};

    if (NULL == pParam)
    {
        goto MIPLVER_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        if (NULL == Param->pPara)
        {
            goto MIPLVER_ERROR;
        }

        AT_TC(g_sw_SPEC, "AT_MIP_CmdFunc_MIPLVER iOpen Param->iType %d", Param->iType);
        switch (Param->iType)
        {
            case AT_CMD_READ:
            {
                ver = mipl_GetVersion();

                UINT8 aucBuffer[40] = { 0 };
                AT_Sprintf(aucBuffer, "+MIPLVER:%d", ver);

                at_CmdRespInfoText(pParam->engine, aucBuffer);
                at_CmdRespOK(pParam->engine);
                break;
            }
            default:
            {
                goto MIPLVER_ERROR;
            }
        }
    }
    return;
MIPLVER_ERROR:
    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
}

#endif
