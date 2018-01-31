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


#include "cfw.h"
#include "sul.h"

#include "cfw_cfg.h"
#include "csw_debug.h"
#include <string.h>
//
// SMS Configuration
//
UINT8 g_SmsFormat_Sim0 = 0;
UINT8 g_SmsFormat_Sim1 = 0;
UINT8 g_SmsFormat_Sim2 = 0;
UINT8 g_SmsFormat_Sim3 = 0;
UINT8 g_SmsShowTextMode_Sim0 = 0;
UINT8 g_SmsShowTextMode_Sim1 = 0;
UINT8 g_SmsShowTextMode_Sim2 = 0;
UINT8 g_SmsShowTextMode_Sim3 = 0;

static UINT32 sms_SetParamAoProc(HAO hAo, CFW_EV *pEvent);


UINT32 CFW_CfgSetSmsStorage (UINT8 nStorage,
                             CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetSmsStorage);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgSetSmsStorage  nStorage = 0x%x, nSimID =0x%x\n",0x08100267), nStorage, nSimID);
    ret = CFW_CfgNumRangeCheck(nStorage, SMS_STORAGE_RANGE);
    if(ERR_SUCCESS == ret)
    {
        if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
        {
            ret = NV_SetSmsStorage(nSimID, nStorage);
        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsStorage);
            return ret;
        }
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgNumRangeCheck, error ret =0x%x\n",0x08100268), ret);
        ret = ERR_CFG_PARAM_OUT_RANGR;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsStorage);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgSetSmsStorage exit\n",0x08100269));
    return ret;
}

UINT32 CFW_CfgGetSmsStorage ( UINT8 *pStorage,
                              CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetSmsStorage);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetSmsStorage  nSimID =0x%x\n",0x0810026a), nSimID);
    if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
    {
        ret = NV_GetSmsStorage(nSimID, pStorage);
        if(ERR_SUCCESS != ret)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgGetSmsStorage,1, call cfg_readdefault, error ret=0x%x\n",0x0810026c), ret);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsStorage);
            return ret;
        }
        else
        {
            ret = CFW_CfgNumRangeCheck(*pStorage, SMS_STORAGE_RANGE);
            if(ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgGetSmsStorage,2, call cfg_readdefault, error ret=0x%x\n",0x0810026e), ret);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsStorage);
                return ret;
             }
        }
    }
    else
    {
        ret = ERR_CFW_INVALID_PARAMETER;
    }

    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsStorage);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetSmsStorage  *pStorage =0x%x\n",0x0810026f), *pStorage);
    return ret;
}

UINT32 CFW_CfgSetSmsOverflowInd ( UINT8 nMode, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetSmsOverflowInd);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgSetSmsOverflowInd  nMode = 0x%x, nSimID =0x%x\n",0x08100270), nMode, nSimID);
    ret = CFW_CfgNumRangeCheck(nMode, SMS_OVERFLOW_RANGE);
    if(ERR_SUCCESS == ret)
    {
        if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
        {
            ret = NV_SetSmsOverInd(nSimID, nMode);
        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
        }

    }
    else
        ret = ERR_CFG_PARAM_OUT_RANGR;
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsOverflowInd);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetSmsOverflowInd  ret =0x%x\n",0x08100271), ret);
    return ret;
}

UINT32 CFW_CfgGetSmsOverflowInd ( UINT8 *pMode, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGetSmsOverflowInd);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetSmsOverflowInd, nSimID =0x%x\n",0x08100272), nSimID);
    if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
    {
         ret = NV_GetSmsOverInd(nSimID, pMode);
         if(ERR_SUCCESS != ret)
         {
             CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgGetSmsOverflowInd,1,call NV_GetSmsOverInd falsed, ret=0x%x\n",0x08100274), ret);
             CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetSmsOverflowInd);
             return ret;
         }
    }
    else
    {
        ret = ERR_CFW_INVALID_PARAMETER;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetSmsOverflowInd);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetSmsOverflowInd, *pMode =0x%x\n",0x08100276), *pMode);
    return ret;
}

UINT32 CFW_CfgSetSmsFormat(UINT8 nFormat, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetSmsFormat);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgSetSmsFormat, nFormat = 0x%x,nSimID =0x%x\n",0x08100277), nFormat, nSimID);
    if(nFormat == 0 || nFormat == 1)
    {
        if(CFW_SIM_0 == nSimID)
        {
            g_SmsFormat_Sim0 = nFormat;
        }
        else if(CFW_SIM_1 == nSimID)
        {
            g_SmsFormat_Sim1 = nFormat;
        }
        else if(CFW_SIM_1 + 1 == nSimID)
        {
            g_SmsFormat_Sim2 = nFormat;
        }
        else if(CFW_SIM_1 + 2 == nSimID)
        {
            g_SmsFormat_Sim3 = nFormat;
        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
        }
    }
    else
    {
        ret = ERR_CFG_PARAM_OUT_RANGR;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsFormat);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgSetSmsFormat exit\n",0x08100278));
    return ret;
}

UINT32 CFW_CfgGetSmsFormat(UINT8 *nFormat , CFW_SIM_ID nSimID)
{
    if(CFW_SIM_0 == nSimID)
    {
        *nFormat = g_SmsFormat_Sim0;
    }
    else if(CFW_SIM_1 == nSimID)
    {
        *nFormat = g_SmsFormat_Sim1;
    }
    else if(CFW_SIM_1 + 1 == nSimID)
    {
        *nFormat = g_SmsFormat_Sim2;
    }
    else if(CFW_SIM_1 + 2 == nSimID)
    {
        *nFormat = g_SmsFormat_Sim3;
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgGetSmsFormat, *nFormat = 0x%x,nSimID =0x%x\n",0x08100279), *nFormat, nSimID);
    return ERR_SUCCESS;

}

UINT32 CFW_CfgSetSmsShowTextModeParam ( UINT8 nShow , CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgSetSmsShowTextModeParam, nShow = 0x%x,nSimID =0x%x\n",0x0810027a), nShow, nSimID);
    if((nShow == 0) || (nShow == 1))
    {
        if(CFW_SIM_0 == nSimID)
        {
            g_SmsShowTextMode_Sim0 = nShow;
        }
        else if (CFW_SIM_1 == nSimID)
        {
            g_SmsShowTextMode_Sim1 = nShow;
        }
        else if (CFW_SIM_1 + 1 == nSimID)
        {
            g_SmsShowTextMode_Sim2 = nShow;
        }
        else if (CFW_SIM_1 + 2 == nSimID)
        {
            g_SmsShowTextMode_Sim3 = nShow;
        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
        }
    }
    else
        ret = ERR_CFG_PARAM_OUT_RANGR;

    return ret;

}

UINT32 CFW_CfgGetSmsShowTextModeParam ( UINT8 *pShow , CFW_SIM_ID nSimID)
{
    if(CFW_SIM_0 == nSimID)
    {
        * pShow = g_SmsShowTextMode_Sim0;
    }
    else if (CFW_SIM_1 == nSimID)
    {
        * pShow = g_SmsShowTextMode_Sim1;
    }
    else if (CFW_SIM_1 + 1 == nSimID)
    {
        * pShow = g_SmsShowTextMode_Sim2;
    }
    else if (CFW_SIM_1 + 2 == nSimID)
    {
        * pShow = g_SmsShowTextMode_Sim3;
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgGetSmsShowTextModeParam, *pShow = 0x%x,nSimID =0x%x\n",0x0810027b), *pShow, nSimID);
    return ERR_SUCCESS;
}

UINT32 CFW_CfgSetSmsFullForNew ( UINT8 n, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetSmsFullForNew);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgSetSmsFullForNew, n = 0x%x,nSimID =0x%x\n",0x0810027c), n, nSimID);
    ret = CFW_CfgNumRangeCheck(n, SMS_FULLFORNEW_RANGE);
    if(ERR_SUCCESS == ret)
    {
        if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
        {
            ret = NV_SetSmsFullForNew(nSimID,n);
        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
        }
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsFullForNew);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetSmsFullForNew, ret =0x%x\n",0x0810027d), ret);
    return ret;
}
#if 1
UINT32 CFW_CfgGetSmsFullForNew ( UINT8 *pn, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    if(nSimID >= CFW_SIM_COUNT)
        return ERR_CFW_INVALID_PARAMETER;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGetSmsFullForNew);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetSmsFullForNew, nSimID =0x%x\n",0x0810027e), nSimID);

    ret = NV_GetSmsFullForNew(nSimID,pn);
    if (ERR_SUCCESS == ret)
    {
        ret = CFW_CfgNumRangeCheck(*pn, SMS_FULLFORNEW_RANGE);
    }
    else
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("NV_GetSmsFullForNew, error ret =0x%x\n",0x08100280), ret);

    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetSmsFullForNew, *pn =0x%x\n",0x08100283), *pn);
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetSmsFullForNew);
    return ret;
}

#else
UINT32 CFW_CfgGetSmsFullForNew ( UINT8 *pn, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    BOOL bDef = FALSE;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGetSmsFullForNew);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetSmsFullForNew, nSimID =0x%x\n",0x08100284), nSimID);

    INT8 simid_c[2]  = {0x30, 0x00};
    INT8 name_s[18] = {0x00,};
    strcpy(name_s, "FULL_FOR_NEW_SIM");

    if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
    {
        simid_c[0] = simid_c[0] + nSimID;
        strcat(name_s, simid_c);
        ret = cfg_read_8("sms", name_s, pn);

    }
    else
    {
        ret = ERR_CFW_INVALID_PARAMETER;
    }

    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_read_8, error ret =0x%x\n",0x08100285), ret);
        if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
        {
            simid_c[0] = simid_c[0] + nSimID;
            strcat(name_s, simid_c);
            ret = cfg_readdefault("sms", name_s, pn);

        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
        }


        if(ERR_SUCCESS != ret)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgGetSmsFullForNew,1, call cfg_readdefault falsed, ret =0x%x\n",0x08100286), ret);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetSmsFullForNew);
            return ret;
        }
        bDef = TRUE;
    }
    else
    {
        ret = CFW_CfgNumRangeCheck(*pn, SMS_FULLFORNEW_RANGE);
        if(ERR_SUCCESS != ret)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgGetSmsFullForNew,2, call CFW_CfgNumRangeCheck falsed, ret =0x%x\n",0x08100287), ret);
            if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
            {
                simid_c[0] = simid_c[0] + nSimID;
                strcat(name_s, simid_c);
                ret = cfg_readdefault("sms", name_s, pn);

            }
            else
            {
                ret = ERR_CFW_INVALID_PARAMETER;
            }
            if(ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgGetSmsFullForNew,2, call cfg_readdefault falsed, ret =0x%x\n",0x08100288), ret);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetSmsFullForNew);
                return ret;
            }
        }
        bDef = TRUE;
    }
    if(bDef)
    {
        if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
        {
            simid_c[0] = simid_c[0] + nSimID;
            strcat(name_s, simid_c);
            ret = cfg_write_8("sms", name_s, *pn);

        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
        }
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetSmsFullForNew);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetSmsFullForNew, *pn =0x%x\n",0x08100289), *pn);
    return ERR_SUCCESS;
}
#endif
PUBLIC UINT32 SRVAPI CFW_CfgSetSmsParam (
    CFW_SMS_PARAMETER *pInfo,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
)
{
    UINT32 ret = ERR_SUCCESS;
    UINT8 nIndex;
    if(pInfo == NULL)
        return ERR_CFG_PARAM_OUT_RANGR;
    if(pInfo->nIndex > 4)
        return ERR_CFG_PARAM_OUT_RANGR;
    if(( nSimID < CFW_SIM_0) || (nSimID >= CFW_SIM_COUNT))
        return ERR_CFW_INVALID_PARAMETER;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetSmsParam);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetSmsParam, nSimID =0x%x\n",0x0810028a), nSimID);
    nIndex = pInfo->nIndex;
    if (0 == pInfo->nSaveToSimFlag)
    {
        if(pInfo->bDefault)
        {
            ret = NV_SetSmsDefParamInd(nSimID, nIndex);
            if(ERR_SUCCESS != ret)
            {
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsParam);
                return ret;
            }
        }
        if(pInfo)
        {
            ret = NV_SetSmsSimParaml(nSimID, nIndex,(UINT8*)pInfo,sizeof(CFW_SMS_PARAMETER));
            if(ERR_SUCCESS != ret)
            {
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsParam);
                return ret;
            }
        }
        CFW_RelaseUsingUTI( CFW_APP_SRV_ID , nUTI );
    }
    else
    {
        CFW_SMS_PARAMETER *pParamInfo = NULL;
        CFW_SMS_PARAMETER *pStoredParam = NULL;
        HAO hAo = 0;

        ret = CFW_IsFreeUTI(nUTI, CFW_APP_SRV_ID);

        if(ret != ERR_SUCCESS)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_IsFreeUTI, error ret =0x%x\n",0x0810028b), ret);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SmsWriteMessage);
            return ret;
        }

        pParamInfo = (CFW_SMS_PARAMETER *)CSW_CFG_MALLOC(SIZEOF(CFW_SMS_PARAMETER) * 2);
        if (NULL == pParamInfo)
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsParam);
            return ERR_CMS_MEMORY_FULL;
        }
        SUL_ZeroMemory8(pParamInfo, SIZEOF(CFW_SMS_PARAMETER) * 2);
        SUL_MemCopy8(pParamInfo, pInfo, SIZEOF(CFW_SMS_PARAMETER));

        pStoredParam = (CFW_SMS_PARAMETER *)((UINT32)pParamInfo + SIZEOF(CFW_SMS_PARAMETER));

        ret = CFW_CfgGetSmsParam(pStoredParam, pInfo->nIndex, nSimID);

        if(ERR_SUCCESS != ret)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetSmsParam, error ret =0x%x\n",0x0810028c), ret);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsParam);
            CSW_CFG_FREE(pParamInfo);
            return ret;
        }

        if(pInfo->bDefault)
        {
            ret = NV_SetSmsDefParamInd(nSimID,nIndex);
            if(ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_write_8, error ret =0x%x\n",0x0810028d), ret);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsParam);
                CSW_CFG_FREE(pParamInfo);
                return ret;
            }
        }
        if(pInfo)
        {
            ret = NV_SetSmsSimParaml(nSimID, nIndex,(UINT8*)pInfo,sizeof(CFW_SMS_PARAMETER));
            if(ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_write_hex, error ret =0x%x\n",0x0810028e), ret);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsParam);
                CSW_CFG_FREE(pParamInfo);
                return ret;
            }
        }
        hAo = CFW_RegisterAo(CFW_APP_SRV_ID, pParamInfo, sms_SetParamAoProc, nSimID);
        CFW_SetUTI(hAo, nUTI, 0);
        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsParam);
        return ERR_SUCCESS;
    }

    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsParam);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetSmsParam, exit ret =0x%x\n",0x0810028f), ret);
    return ret;
}

UINT32 CFW_CfgGetSmsParam (
    CFW_SMS_PARAMETER *pInfo,
    UINT8 nIndex,
    CFW_SIM_ID nSimID
)
{
    UINT32 ret = ERR_SUCCESS;
    UINT32 len = 0;

    if((nIndex > 4) || (pInfo == NULL))
        return ERR_CFG_PARAM_OUT_RANGR;
    if(( nSimID < CFW_SIM_0) || (nSimID >= CFW_SIM_COUNT))
        return ERR_CFW_INVALID_PARAMETER;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGetSmsParam);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgGetSmsParam, nIndex= 0x%x, nSimID =0x%x\n",0x08100290), nIndex, nSimID);
    len = sizeof(CFW_SMS_PARAMETER);
    ret = NV_GetSmsSimParaml(nSimID, nIndex,(UINT8*)pInfo,&len);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("NV_GetSmsSimParaml, error ret=0x%x\n",0x08100291), ret);
        SUL_ZeroMemory8(pInfo, sizeof(CFW_SMS_PARAMETER));
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetSmsParam);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgGetSmsParam, exit \n",0x08100292));
    return ret;
}

UINT32 CFW_CfgGetDefaultSmsParam (
    CFW_SMS_PARAMETER *pInfo,
    CFW_SIM_ID nSimID
)
{
    UINT32 ret = ERR_SUCCESS;
    UINT8 index = 0;
    UINT32 len = 0;

    if(pInfo == NULL)
        return ERR_CFG_PARAM_OUT_RANGR;
    if(( nSimID < CFW_SIM_0) || (nSimID >= CFW_SIM_COUNT))
        return ERR_CFW_INVALID_PARAMETER;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGetDefaultSmsParam);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetDefaultSmsParam, nSimID=0x%x\n",0x08100293), nSimID);
    if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
    {
        ret = NV_GetSmsDefParamInd(nSimID,&index);
        if(ERR_SUCCESS != ret)
        {
            SUL_ZeroMemory8(pInfo, sizeof(CFW_SMS_PARAMETER));
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgGetDefaultSmsParam,1, call cfg_readdefault falsed, ret =0x%x\n",0x08100295), ret);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetDefaultSmsParam);
            return ret;
        }
    }
    else
    {
        ret = ERR_CFW_INVALID_PARAMETER;
    }
    len = sizeof(CFW_SMS_PARAMETER);
    ret = NV_GetSmsSimParaml(nSimID, index,(UINT8*)pInfo,&len);
    if(ERR_SUCCESS != ret)
    {
        SUL_ZeroMemory8(pInfo, sizeof(CFW_SMS_PARAMETER));
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetDefaultSmsParam);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgGetDefaultSmsParam, exit \n",0x08100296));
    return ERR_SUCCESS;
}

UINT32 CFW_CfgSetDefaultSmsParam (
    CFW_SMS_PARAMETER *pInfo,
    CFW_SIM_ID nSimID
)
{
    UINT32 ret = ERR_SUCCESS;
    UINT8 index = 0;

    if(pInfo == NULL)
        return ERR_CFG_PARAM_OUT_RANGR;
    if(( nSimID < CFW_SIM_0) || (nSimID >= CFW_SIM_COUNT))
        return ERR_CFW_INVALID_PARAMETER;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetDefaultSmsParam);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetDefaultSmsParam, nSimID=0x%x\n",0x08100297), nSimID);
    ret = NV_SetSmsSimParaml(nSimID, index,(UINT8*)pInfo,sizeof(CFW_SMS_PARAMETER));
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetDefaultSmsParam);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgSetDefaultSmsParam, exit \n",0x0810029a));
    return ret;
}

UINT32 CFW_CfgSetNewSmsOption(
    UINT8 nOption,
    UINT8 nNewSmsStorage,
    CFW_SIM_ID nSimID
)
{
    UINT32 ret = ERR_SUCCESS, ret1 = ERR_SUCCESS;

    if(( nSimID < CFW_SIM_0) || (nSimID >= CFW_SIM_COUNT))
        return ERR_CFW_INVALID_PARAMETER;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetNewSmsOption);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("CFW_CfgSetNewSmsOption, IN[%d,%d],nSimID=0x%x\n",0x0810029b), nOption, nNewSmsStorage, nSimID);
    ret = CFW_CfgNumRangeCheck(nOption, SMS_OPTION_RANGE);
    ret1 = CFW_CfgNumRangeCheck(nNewSmsStorage, SMS_OPT_STORAGE_RABGE);
    if ((ERR_SUCCESS == ret) && (ERR_SUCCESS == ret1))
    {
        ret = NV_SetSmsSimOption(nSimID,nOption);
        ret1 = NV_SetSmsSimOptStor(nSimID,nNewSmsStorage);
        if ((ERR_SUCCESS != ret) || (ERR_SUCCESS != ret1))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTR("in cfg_write_8, error IN[%d,%d],RET[0x%x,ret=0x%x]\n",0x0810029c), nOption, nNewSmsStorage, ret, ret1);
            ret = ERR_CFG_WRITE_REG_FAILURE ;
        }
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTR("in CFW_CfgNumRangeCheck, error IN[%d,%d],RET[0x%x,ret=0x%x]\n",0x0810029d), nOption, nNewSmsStorage, ret, ret1);
        ret = ERR_CFG_PARAM_OUT_RANGR ;
    }

    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetNewSmsOption);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgSetDefaultSmsParam, exit \n",0x0810029e));
    return ret;

}


UINT32 CFW_CfgGetNewSmsOption(  UINT8 *pOption,   UINT8 *pNewSmsStorage, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    if(pOption == NULL || pNewSmsStorage == NULL)
        return ERR_CFW_INVALID_PARAMETER;
    if(( nSimID < CFW_SIM_0) || (nSimID >= CFW_SIM_COUNT))
        return ERR_CFW_INVALID_PARAMETER;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGetNewSmsOption);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetNewSmsOption, nSimID=0x%x\n",0x0810029f), nSimID);
    ret = NV_GetSmsSimOption(nSimID, pOption);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgGetNewSmsOption,1, call NV_GetSmsSimOption error ret=0x%x\n",0x081002a1), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetNewSmsOption);
        return ret;
    }
    ret = NV_GetSmsSimOptStor(nSimID, pNewSmsStorage);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgGetNewSmsOption,1, call NV_GetSmsSimOptStor error ret=0x%x\n",0x081002a1), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetNewSmsOption);
        return ret;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("in CFW_CfgGetNewSmsOption,5, *pNewSmsStorage=%d, *pOption=%d\n",0x081002a4), *pNewSmsStorage, *pOption);
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetNewSmsOption);
    return ERR_SUCCESS;
}


PRIVATE CFW_SMS_STORAGE_INFO g_storageInfo[CFW_SIM_COUNT * 2] = {{0,},};

UINT32 CFW_CfgSetSmsStorageInfo (
    CFW_SMS_STORAGE_INFO *pStorageInfo,
    UINT16 nStorage,
    CFW_SIM_ID nSimID
)
{
    //UINT32 ret= ERR_SUCCESS;
    CFW_SMS_STORAGE_INFO *p;
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgSetSmsStorageInfo  nStorage = 0x%x, nSimID =0x%x\n",0x081002a5), nStorage, nSimID);

    if(pStorageInfo == NULL)
        return ERR_CFW_INVALID_PARAMETER;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetSmsStorageInfo);
    if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
    {
        if(nStorage == CFW_SMS_STORAGE_ME)
        {
#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE
            p = &g_storageInfo[0];
#else
            p = &g_storageInfo[nSimID];
#endif
        }
        else if(nStorage == CFW_SMS_STORAGE_SM)
        {
            p = &g_storageInfo[CFW_SIM_COUNT + nSimID];
        }
        else
        {
            p = g_storageInfo;
        }
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;

    }
    SUL_MemCopy8(p, pStorageInfo, sizeof(CFW_SMS_STORAGE_INFO));
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsStorageInfo);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgSetSmsStorageInfo, exit \n",0x081002a6));
    return ERR_SUCCESS;
}

UINT32 CFW_CfgGetSmsStorageInfo (
    CFW_SMS_STORAGE_INFO *pStorageInfo,
    UINT16 nStorage,
    CFW_SIM_ID nSimID
)
{
    UINT32 ret = ERR_SUCCESS;
    CFW_SMS_STORAGE_INFO *p;

    if(pStorageInfo == NULL)
        return ERR_CFW_INVALID_PARAMETER;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGetSmsStorageInfo);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgGetSmsStorageInfo  nStorage = 0x%x, nSimID =0x%x\n",0x081002a7), nStorage, nSimID);
    if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
    {
        if(nStorage == CFW_SMS_STORAGE_MT)
            nStorage = CFW_SMS_STORAGE_SM;
        if(nStorage == CFW_SMS_STORAGE_ME)
        {
#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE
            p = &g_storageInfo[0];
#else
            p = &g_storageInfo[nSimID];
#endif
        }
        else if(nStorage == CFW_SMS_STORAGE_SM)
        {
            p = &g_storageInfo[CFW_SIM_COUNT + nSimID];
        }
        else
        {
            p = g_storageInfo;
        }
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;

    }
    ret = (UINT32)SUL_MemCopy8(pStorageInfo, p, sizeof(CFW_SMS_STORAGE_INFO));

    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetSmsStorageInfo);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgGetSmsStorageInfo, exit \n",0x081002a8));
    return ERR_SUCCESS;
}



extern UINT32 CFW_GetSimID(HAO hAO, CFW_SIM_ID *pSimID);
//added by fengwei 20080707 for save sca to sim begin.
static UINT32 sms_SetParamAoProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32  nState = 0x0, nGetUti = 0x0; // [-] dingmx20080908 delete unused ret
    CFW_SMS_PARAMETER *pSmsParamInfo = NULL; //Point to private data.
    CFW_SIM_SMS_PARAMETERS pSimSmsParam;
    CSW_PROFILE_FUNCTION_ENTER(sms_SetParamAoProc);
    CFW_SIM_ID nSimID = CFW_SIM_COUNT;
    CFW_GetSimID(hAo, &nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SetParamAoProc() enter\n",0x081002a9));
    //Get private date
    pSmsParamInfo = CFW_GetAoUserData(hAo);
    //Verify memory
    if(!pSmsParamInfo)
    {
        CFW_GetUTI(hAo, &nGetUti);
        CFW_PostNotifyEvent(EV_CFW_SMS_SET_PARAM_RSP, ERR_CMS_MEMORY_FULL, 0, nGetUti | (nSimID << 24), 0xf0);
        CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
        CSW_PROFILE_FUNCTION_EXIT(sms_SetParamAoProc);
        return ERR_NO_MORE_MEMORY;
    }

    //Get current State
    if(pEvent == (CFW_EV *)0xffffffff)
        nState = CFW_SM_STATE_IDLE;
    else
        nState = CFW_GetAoState(hAo);

    //SMS MO State machine process
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sms_SetParamAoProc, nState =0x%x\n",0x081002aa), nState);
    switch(nState)
    {
        case CFW_SM_STATE_IDLE:
        {
            UINT32 uRet;
            SUL_ZeroMemory8(&pSimSmsParam, SIZEOF(CFW_SIM_SMS_PARAMETERS));
            pSimSmsParam.nDCS = pSmsParamInfo->dcs;
            pSimSmsParam.nPID = pSmsParamInfo->pid;
            pSimSmsParam.nVP  = pSmsParamInfo->vp;
            pSimSmsParam.nFlag = pSmsParamInfo->nSaveToSimFlag;
            SUL_MemCopy8(pSimSmsParam.nSMSCInfo, pSmsParamInfo->nNumber, 12);

            CFW_GetUTI(hAo, &nGetUti);
            //we set one set of CFW_SIM_SMS_PARAMETERS each time.
            uRet = CFW_SimSetSmsParameters(&pSimSmsParam, 1, pSmsParamInfo->nIndex, (UINT16)(CFW_APP_UTI_MIN + nGetUti), nSimID);
            if (ERR_SUCCESS != uRet)
            {
                //set sms fail, we think it's memory malloc fail.
                CFW_PostNotifyEvent(EV_CFW_SMS_SET_PARAM_RSP, ERR_CMS_MEMORY_FULL, 0, nGetUti | (nSimID << 24), 0xf0);
                CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(sms_SetParamAoProc);
                return ERR_NO_MORE_MEMORY;
            }
            else
            {
                CFW_SetAoState(hAo, CFW_SM_STATE_WAIT);
            }
        }
        break;
        case CFW_SM_STATE_WAIT:
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("sms_SetParamAoProc, pEvent->nEventId =0x%x, pEvent->nType  =0x%x\n",0x081002ab), pEvent->nEventId, pEvent->nType );
            if(pEvent->nEventId == EV_CFW_SIM_SET_SMS_PARAMETERS_RSP)
            {

                if (pEvent->nType == 0)
                {
                    CFW_GetUTI(hAo, &nGetUti);
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_PARAM_RSP, 0, 0, nGetUti | (nSimID << 24), 0);
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    return ERR_SUCCESS;
                }
                else//set fails
                {
                    UINT32 uRet = 0;
                    //restore param in ME.
                    CFW_SMS_PARAMETER *pStoredParam = (CFW_SMS_PARAMETER *)( (UINT32)pSmsParamInfo + SIZEOF(CFW_SMS_PARAMETER)); // [mod] dingmx20080908 add type cast with (CFW_SMS_PARAMETER*)

                    //set default to the value fo setting param.
                    pStoredParam->bDefault = pSmsParamInfo->bDefault;
                    //restore param in ME, so don't need to restore param in SIM.
                    pStoredParam->nSaveToSimFlag = 0;
                    CFW_GetUTI(hAo, &nGetUti);
                    uRet = CFW_CfgSetSmsParam(pStoredParam, nGetUti, nSimID);

                    if (ERR_SUCCESS != uRet)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Set Param to Sim fails, and restore param in ME fails, WRONING!!!\n",0x081002ac));
                    }
                    CFW_PostNotifyEvent(EV_CFW_SMS_SET_PARAM_RSP, pEvent->nParam1, 0, nGetUti | (nSimID << 24), 0xf0);
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
                    return ERR_CMS_UNKNOWN_ERROR;
                }
            }
        }
        break;
    }
    CSW_PROFILE_FUNCTION_EXIT(sms_SetParamAoProc);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sms_SetParamAoProc() exit\n",0x081002ad));
    return ERR_SUCCESS;
}
PRIVATE UINT8 g_mids[6] = {0,};
PRIVATE UINT8 g_dcss[6] = {0,};
PRIVATE UINT8 g_mode = 0;
/*===================================================================

===================================================================*/
UINT32 CFW_CfgSetSmsCB ( UINT8 mode, UINT8 mids[6], UINT8 dcss[6])
{
    UINT32 ret = ERR_SUCCESS;
    UINT8 i = 0;
    if(mode > 1)
        return ERR_CFG_PARAM_OUT_RANGR;

    for(i = 0; i < SIZEOF(g_mids); i++)
        g_mids[i] = mids[i];
    for(i = 0; i < SIZEOF(g_dcss); i++)
        g_dcss[i] = dcss[i];
    g_mode = mode;

    return ret;
}


UINT32 CFW_CfgGetSmsCB (
    UINT8 *pMode,
    UINT8 mids[6],
    UINT8 dcss[6]
)
{
    UINT8 i = 0;
    for(i = 0; i < SIZEOF(g_mids); i++)
        mids[i] = g_mids[i];
    for(i = 0; i < SIZEOF(g_dcss); i++)
        dcss[i] = g_dcss[i];
    if(pMode)
        *pMode = g_mode;

    return ERR_SUCCESS;
}


/*======================================================================================
NAME    FUNCTION                PARAMETER   RANGE   DEFAULT
AT+CSMS CFW_CfgSelectSmsService nService    0-1     0
pMTType     0-1     1
pMOType     0-1     1
pBMType     0-1     1
=======================================================================================*/
UINT32 CFW_CfgSelectSmsService( UINT8 nService, UINT8 *pSupportType)
{
#if 0
    UINT32 ret = ERR_SUCCESS;

    /*Check the range*/
    ret = CFW_CfgNumRangeCheck(nService, SMS_SERV_SELECT_RANGE);
    if(ret != ERR_SUCCESS)
        return ret;

    MemoryInfo.SelectSmsService = nService;

    if(nService)
        /*GSM 03.40 and 03.41 (the syntax of SMS AT commands is compatible with GSM 07.05
        Phase 2+ version; the requirement of <service> setting 1 is mentioned under corresponding
        command descriptions).
        */
    {
        CFW_SET_BIT(*pSupportType, 0); //*pMTType = 1;
        CFW_SET_BIT(*pSupportType, 1); //*pMOType = 1;
        CFW_SET_BIT(*pSupportType, 2); //*pBMType = 1;
    }
    /*GSM 03.40 and 03.41 (the syntax of SMS AT commands is compatible with GSM 07.05
    Phase 2 version 4.7.0; Phase 2+ features which do not require new command syntax
    may be supported, e.g. correct routing of messages with new Phase 2+ data coding schemes)
    */
    else
    {
        CFW_SET_BIT(*pSupportType, 0); //*pMTType = 1;
        CFW_SET_BIT(*pSupportType, 1); //*pMOType = 1;
        CFW_SET_BIT(*pSupportType, 2); //*pBMType = 1;
    }

    return ret;
#endif
    return ERR_CFW_NOT_SUPPORT;
}

/*==========================================================================
The CFW_CfgQuerySmsService function select the current SMS service.

==========================================================================*/
UINT32 CFW_CfgQuerySmsService( UINT8 *pService )
{
#if 0
    *pService = 0;

    CFW_SET_BIT(*pService, 0);
    CFW_SET_BIT(*pService, 1);
    CFW_SET_BIT(*pService, 2);
    CFW_SET_BIT(*pService, 3);

    return ERR_SUCCESS;
#endif
    return ERR_CFW_NOT_SUPPORT;
}
//added by fengwei 20080707 for save sca to sim end
/*===================================================================
The CFW_CfgSetNewSmsAck function set New SMS message acknowledge to
ME/TE, only phase 2+.
===================================================================*/
UINT32 CFW_CfgSetNewSmsAck ( BOOL bAck )
{
#if 0
    UINT32 ret = ERR_SUCCESS;

#ifndef MF32_BASE_REG_DISABLE
    WRITE_NUMBER_SETTING_8("SMS_ACK", bAck, "sms" );
#endif

    return ret;
#endif
    return ERR_CFW_NOT_SUPPORT;
}

UINT32 CFW_CfgGetNewSmsAck ( UINT8 *pAck)
{
#if 0
    UINT32 ret = ERR_SUCCESS;
    READ_NUMBER_SETTING_8("SMS_ACK", *pAck, "sms" );
    return ret;
#endif
    return ERR_CFW_NOT_SUPPORT;
}

#ifdef CFW_CUSTOM_IMEI_COUNT
UINT32 CFW_CfgSetCustomSendIMEIFlag (UINT8 nFlag,
                                     CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetSmsStorage);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgSetCustomSendIMEIFlag nStorage[0x%x] nSimID[0x%x]\n",0x081002ae), nStorage, nSimID);
    ret = CFW_CfgNumRangeCheck(nFlag, SMS_IMEI_RANGE);
    if(ERR_SUCCESS == ret)
    {
        INT8 simid_c[2]  = {0x30, 0x00};
        INT8 name_s[14] = {0x00,};
        strcpy(name_s, "SMS_IMEI_");

        if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
        {
            simid_c[0] = simid_c[0] + nSimID;
            strcat(name_s, simid_c);
            ret = cfg_write_8("sms", name_s, nFlag);
        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetCallWaiting);
        }
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_CfgSetCustomSendIMEIFlag  ret[0x%x]\n",0x081002af), ret);
        ret = ERR_CFG_PARAM_OUT_RANGR;
    }

    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsStorage);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgSetSmsStorage Exit\n",0x081002b0));
    return ret;
}

UINT32 CFW_CfgGetCustomSendIMEIFlag ( UINT8 *pFlag,
                                      CFW_SIM_ID nSimID)
{

    UINT32 ret = ERR_SUCCESS;
    BOOL bDef = FALSE;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetSmsStorage);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetCustomSendIMEIFlag nSimID[0x%x]\n",0x081002b1), nSimID);

    INT8 simid_c[2]  = {0x30, 0x00};
    INT8 name_s[14] = {0x00,};
    strcpy(name_s, "SMS_IMEI_");

    if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
    {
        simid_c[0] = simid_c[0] + nSimID;
        strcat(name_s, simid_c);
        ret = cfg_read_8("sms", name_s, pFlag);

        if(ERR_SUCCESS != ret)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgGetCustomSendIMEIFlag->cfg_read_8 error ret[0x%x]\n",0x081002b2), ret);
            ret = cfg_readdefault("sms", name_s, pFlag);
            if(ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgGetCustomSendIMEIFlag->cfg_readdefault error ret[0x%x]\n",0x081002b3), ret);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsStorage);
                return ret;
            }
            bDef = TRUE;
        }
        else
        {
            ret = CFW_CfgNumRangeCheck(*pFlag, SMS_STORAGE_RANGE);
            if(ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgGetCustomSendIMEIFlag->CFW_CfgNumRangeCheck error ret[0x%x]\n",0x081002b4), ret);
                ret = cfg_readdefault("sms", name_s, pFlag);
                if(ERR_SUCCESS != ret)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgGetCustomSendIMEIFlag->cfg_readdefault error ret[0x%x]\n",0x081002b5), ret);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsStorage);
                    return ret;
                }
                bDef = TRUE;
            }
        }

    }
    else
    {
        ret = ERR_CFW_INVALID_PARAMETER;
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetCallWaiting);
    }

    if(bDef)
    {
        cfg_write_8("sms", name_s, *pFlag);
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSmsStorage);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetCustomSendIMEIFlag  *pStorage[0x%x]\n",0x081002b6), *pFlag);
    return ret;
}
#endif

/*============================================================================
Set the SMS Service Center telephone number.

  KEY: "SMS_CENTER_1" "SMS_CENTER_2" "SMS_CENTER_3"
  "SMS_CENTER_4" "SMS_CENTER_5" "SMS_CENTER_6"
============================================================================*/
UINT32 CFW_CfgSetSmsCenter (CFW_TELNUMBER *pSmsCenterNumber)
{
#if 0
    UINT32 ret = ERR_SUCCESS;
    UINT32 *pInt = 0;

#ifndef MF32_BASE_REG_DISABLE

    pInt = (UINT32 *)pSmsCenterNumber;

    WRITE_NUMBER_SETTING("SMS_CENTER_1", *pInt, "sms");
    pInt++;

    WRITE_NUMBER_SETTING("SMS_CENTER_2", *pInt, "sms");
    pInt++;

    WRITE_NUMBER_SETTING("SMS_CENTER_3", *pInt, "sms");
    pInt++;

    WRITE_NUMBER_SETTING("SMS_CENTER_4", *pInt, "sms");
    pInt++;

    WRITE_NUMBER_SETTING("SMS_CENTER_5", *pInt, "sms");
    pInt++;

    WRITE_NUMBER_SETTING("SMS_CENTER_6", *pInt, "sms");
#endif

    return ret;
#endif
    return ERR_CFW_NOT_SUPPORT;

}

/*============================================================================
Get the SMS Service Center telephone number.

  KEY: "SMS_CENTER_1" "SMS_CENTER_2" "SMS_CENTER_3"
  "SMS_CENTER_4" "SMS_CENTER_5" "SMS_CENTER_6"
============================================================================*/
UINT32 CFW_CfgGetSmsCenter (CFW_TELNUMBER *pSmsCenterNumber)
{
#if 0
    UINT32 ret = ERR_SUCCESS;

#ifndef MF32_BASE_REG_DISABLE

    UINT32 integer = 0;

    READ_NUMBER_SETTING("SMS_CENTER_1", integer, "sms");
    SUL_MemCopy32(&(pSmsCenterNumber->nTelNumber[0]), &integer, 4);

    READ_NUMBER_SETTING("SMS_CENTER_2", integer, "sms");
    SUL_MemCopy32(&(pSmsCenterNumber->nTelNumber[4]), &integer, 4);

    READ_NUMBER_SETTING("SMS_CENTER_3", integer, "sms");
    SUL_MemCopy32(&(pSmsCenterNumber->nTelNumber[8]), &integer, 4);

    READ_NUMBER_SETTING("SMS_CENTER_4", integer, "sms");
    SUL_MemCopy32(&(pSmsCenterNumber->nTelNumber[12]), &integer, 4);

    READ_NUMBER_SETTING("SMS_CENTER_5", integer, "sms");
    SUL_MemCopy32(&(pSmsCenterNumber->nTelNumber[16]), &integer, 4);

    READ_NUMBER_SETTING("SMS_CENTER_6", integer, "sms");
    pSmsCenterNumber->nTelNumber[20] = (UINT8)((integer & 0xff000000) >> 24);
    pSmsCenterNumber->nTelNumber[21] = (UINT8)((integer & 0x00ff0000) >> 16);
    pSmsCenterNumber->nTelNumber[22] = (UINT8)((integer & 0x0000ff00) >> 8);
    pSmsCenterNumber->nType =  (UINT8)(integer & 0x000000ff);
#endif

    return ret;
#endif
    return ERR_CFW_NOT_SUPPORT;

}

UINT32 CFW_CfgSmsFlush (
    VOID
)
{
    //TODO...

    return ERR_CFW_NOT_SUPPORT;
}
