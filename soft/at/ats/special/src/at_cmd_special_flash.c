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
#include "at_cmd_special_flash.h"
#include "tgt_m.h"

VOID AT_EMOD_CmdFunc_FACTSN(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        uint8_t mbsn[FACT_SN_LEN + 12];
        memcpy(mbsn, "+FACTMBSN: ", 11);
        if (tgt_GetMbSn(mbsn + 11) != 0)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));

        mbsn[FACT_SN_LEN + 11] = 0;
        at_CmdRespInfoText(pParam->engine, mbsn);
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if (pParam->paramCount != 1)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        const uint8_t *pmbsn = at_ParamStr(pParam->params[0], &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        unsigned parlen = strlen(pmbsn);
        uint8_t mbsn[FACT_SN_LEN] = {};
        memcpy(mbsn, pmbsn, parlen > FACT_SN_LEN ? FACT_SN_LEN : parlen);
        if (tgt_SetMbSn(mbsn) != 0)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));

        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}
