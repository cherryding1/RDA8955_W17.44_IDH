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
#include "at_cmd_id.h"

#define GMI_ID STRINGIFY_VALUE(AT_GMI_ID)
#define GMM_ID STRINGIFY_VALUE(AT_GMM_ID)
#define GMR_ID STRINGIFY_VALUE(AT_GMR_ID)

UINT32 CFW_EmodGetBaseBandVersion(void);

VOID AT_ID_AsyncEventProcess(COS_EVENT *pEvent)
{
    CFW_EVENT CfwEvent;
    UINT8 nSim;
    AT_CosEvent2CfwEvent(pEvent, &CfwEvent);
    nSim = CfwEvent.nFlag;
    if (CfwEvent.nEventId == EV_CFW_SIM_GET_PROVIDER_ID_RSP)
    {
        AT_TC(g_sw_ID, "b    pCfwEvent->nType = %x\n", CfwEvent.nType);

        if (CfwEvent.nType == 0)
        {
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)CfwEvent.nParam1, (UINT16)CfwEvent.nParam2, CfwEvent.nUTI,
                         nSim);
            return;
        }
        else if (CfwEvent.nType == 0xF0)
        {
            UINT32 nErrorCode = AT_SetCmeErrorCode(CfwEvent.nParam1, FALSE);
            AT_TC(g_sw_ID, "EV_CFW_SIM_GET_PROVIDER_ID_RSP: error!!!\
							errorcode--0x%x\n\r",
                  nErrorCode);
            AT_ZERO_PARAM1(pEvent);
            AT_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, CfwEvent.nUTI, nSim);
            return;
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, CfwEvent.nUTI, nSim);
            return;
        }
    }
}

VOID AT_ID_CmdFunc_CGMM(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
    case AT_CMD_EXE:
        at_CmdRespInfoText(pParam->engine, GMM_ID);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
}

VOID AT_ID_CmdFunc_GMM(AT_CMD_PARA *pParam)
{
    AT_ID_CmdFunc_CGMM(pParam);
}

VOID AT_ID_CmdFunc_CGMR(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
    case AT_CMD_EXE:
        at_CmdRespInfoText(pParam->engine, GMR_ID);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
}

VOID AT_ID_CmdFunc_GMR(AT_CMD_PARA *pParam)
{
    AT_ID_CmdFunc_CGMR(pParam);
}

VOID AT_ID_CmdFunc_CGMI(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
    case AT_CMD_EXE:
        at_CmdRespInfoText(pParam->engine, GMI_ID);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
}

VOID AT_ID_CmdFunc_GMI(AT_CMD_PARA *pParam)
{
    AT_ID_CmdFunc_CGMI(pParam);
}

VOID AT_ID_CmdFunc_CGSN(AT_CMD_PARA *pParam)
{
    UINT8 nImei[15];
    UINT8 nImeiLen;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_EXE:

        CFW_EmodGetIMEI(nImei, &nImeiLen, nSim);
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nImei, nImeiLen, pParam->nDLCI, nSim);
        break;

    case AT_CMD_TEST:
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        break;

    default:
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}

VOID AT_ID_CmdFunc_GSN(AT_CMD_PARA *pParam)
{
    AT_ID_CmdFunc_CGSN(pParam);
}

VOID AT_ID_CmdFunc_I(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_EXE)
    {
        if (pParam->paramCount > 1)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        uint32_t par = at_ParamDefUintInRange(pParam->params[0], 0, 0, 1, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        if (par == 0)
        {
            at_CmdRespInfoText(pParam->engine, GMI_ID);
            at_CmdRespInfoText(pParam->engine, GMM_ID);
            at_CmdRespInfoText(pParam->engine, GMR_ID);
        }
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

VOID AT_ID_CmdFunc_CIMI(AT_CMD_PARA *pParam)
{
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_EXE:
    {
        // AT_20071225_CAOW_B for bug#7160
        UINT32 nResult = 0;

        // AT_20071225_CAOW_E
        nResult = CFW_SimGetProviderId(pParam->nDLCI, nSim);
        if (ERR_SUCCESS == nResult)
        {
            AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        }
        else
        {
            UINT32 nErrorCode = AT_SetCmeErrorCode(nResult, FALSE);

            AT_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        break;
    }

    case AT_CMD_TEST:
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        break;

    default:
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}
/*****************************************************************************
* Name          :   AT_ID_CmdFunc_CGBV
* Description       :   AT+CGBV command,get baseband version
* Parameter     :   AT_CMD_PARA *pParam
* Return            :   VOID
* Remark        :   add for android RIL_REQUEST_BASEBAND_VERSION
* Author            :   wulc
* Data          :     2013-2-1
******************************************************************************/

VOID AT_ID_CmdFunc_CGBV(AT_CMD_PARA *pParam)
{
    UINT8 OutStr[15];
    UINT32 basebandVersion;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {

    case AT_CMD_EXE:

        memset(OutStr, 0, 15);
        basebandVersion = CFW_EmodGetBaseBandVersion();
        AT_Sprintf(OutStr, "+CGBV: RDA%x", basebandVersion);

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, OutStr, AT_StrLen(OutStr), pParam->nDLCI, nSim);
        break;

    case AT_CMD_TEST:
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        break;

    default:
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}
