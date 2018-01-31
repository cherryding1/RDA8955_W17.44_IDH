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


#include "sul.h"
#include "cfw.h"
#include "cfw_cfg.h"
#include "csw_debug.h"
//PRIVATE TM_PARAMETERS g_Cfg_Tm_Params;

//[[hameina[+]2006.8.23 :EMOD ADD TWO NEW FUNCTIONS
UINT32 CFW_CfgGetEmodParam(EMOD_PARAMETERS  *pEmodParam)
{
    UINT32 ret = ERR_SUCCESS;

    if(NULL == pEmodParam)
        return ERR_CFW_INVALID_PARAMETER;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGetEmodParam);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgGetEmodParam enter\n",0x081002b7));
    ret = NV_GetEmodParam(&pEmodParam->CC_Cause);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgGetEmodParam,1, call cfg_read_hex failed,ret = 0x%x, zero memory here\n",0x081002b8), ret);
        SUL_ZeroMemory8(pEmodParam, sizeof(EMOD_PARAMETERS));
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetEmodParam);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetEmodParam exit ret= 0x%x\n",0x081002b9), ret);
    return ret;
}

UINT32 CFW_CfgSetEmodParam(EMOD_PARAMETERS  *pEmodParam)
{
    UINT32 ret = ERR_SUCCESS;

    if(NULL == pEmodParam)
        return ERR_CFW_INVALID_PARAMETER;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetEmodParam);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgSetEmodParam enter\n",0x081002ba));
    ret = NV_SetEmodParam(pEmodParam->CC_Cause);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgSetEmodParam,1, call cfg_write_hex failed,ret = 0x%x\n",0x081002bb), ret);
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetEmodParam);
    return ret;
}
//]]hameina[+]2006.8.23

//[[hameina [+] 2006.10.13
#if 0
typedef struct _BIND_SIM_NM
{
    UINT8 simNo[90];
    UINT8 pad[2];
} BIND_SIM_NM;

typedef struct _BIND_NW_NM
{
    UINT8 nwNo[15];
    UINT8 pad;
} BIND_NW_NM;
#endif
UINT32 Cfg_Emod_ReadSIMNm(BIND_SIM_NM *simnm)
{
    UINT32 ret = ERR_SUCCESS;
    UINT32 len = 0;

    if(NULL == simnm)
        return ERR_CFG_PARAM_OUT_RANGR;
    //    CSW_PROFILE_FUNCTION_ENTER(Cfg_Emod_ReadSIMNm);
    len = 48;
    ret = NV_GetEmodSimnml(0, (UINT8*)(&simnm->simNo[0]), &len);
    if(ERR_SUCCESS == ret)
    {
        len = sizeof(simnm->simNo) - 48;
        ret = NV_GetEmodSimnml(1, (UINT8*)&(simnm->simNo[48]), &len);
        if(ERR_SUCCESS == ret)
        {
            //          CSW_PROFILE_FUNCTION_EXIT(Cfg_Emod_ReadSIMNm);
            return ret;
        }
    }
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Cfg_Emod_ReadSIMNm error ret = 0x%x\n",0x081002bc), ret);
        SUL_ZeroMemory8(simnm, sizeof( BIND_SIM_NM));
    }

    //  CSW_PROFILE_FUNCTION_EXIT(Cfg_Emod_ReadSIMNm);
    return ret;
}


UINT32 Cfg_Emod_UpdateSIMNm(BIND_SIM_NM *simnm)
{
    UINT32 ret = ERR_SUCCESS;
    UINT32 len = 48;

    if(NULL == simnm)
        return ERR_CFG_PARAM_OUT_RANGR;
    //    CSW_PROFILE_FUNCTION_ENTER(Cfg_Emod_UpdateSIMNm);
    len = 48;
    ret = NV_SetEmodSimnml(0, (UINT8*)(&simnm->simNo[0]), len);
    if(ERR_SUCCESS == ret)
    {
        len = 42;
        ret = NV_SetEmodSimnml(0, (UINT8*)(&simnm->simNo[48]), len);
    }
    //  CSW_PROFILE_FUNCTION_EXIT(Cfg_Emod_UpdateSIMNm);
    return ret;
}


UINT32 Cfg_Emod_ReadNWNm(BIND_NW_NM *nwnm)
{
    UINT32 ret = ERR_SUCCESS;
    UINT32 len = 0;

    if(NULL == nwnm)
        return ERR_CFG_PARAM_OUT_RANGR;

    //    CSW_PROFILE_FUNCTION_ENTER(Cfg_Emod_ReadNWNm);
    len = sizeof(nwnm->nwNo);
    ret = NV_GetEmodNwnm((UINT8*)(&nwnm->nwNo[0]), &len);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in Cfg_Emod_ReadNWNm,1, call cfg_read_hex failed,ret = 0x%x, zero memory here\n",0x081002bd), ret);
        SUL_ZeroMemory8(nwnm, sizeof(BIND_NW_NM));
    }
    //  CSW_PROFILE_FUNCTION_EXIT(Cfg_Emod_ReadNWNm);
    return ret;
}


UINT32 Cfg_Emod_UpdateNWNm(BIND_NW_NM *nwnm)
{
    UINT32 ret = ERR_SUCCESS;
    UINT32 len = 0;

    if(NULL == nwnm)
        return ERR_CFG_PARAM_OUT_RANGR;
    //    CSW_PROFILE_FUNCTION_ENTER(Cfg_Emod_UpdateNWNm);
    len = sizeof(nwnm->nwNo);
    ret = NV_SetEmodNwnm((UINT8*)(&nwnm->nwNo[0]), len);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in Cfg_Emod_UpdateNWNm,1,write hex err!, ret = 0x%x\n",0x081002bf), ret);
    }
    //    CSW_PROFILE_FUNCTION_EXIT(Cfg_Emod_UpdateNWNm);
    return ret;
}


UINT32 Cfg_Emod_ReadSign(BOOL *sim_Sign, BOOL *nw_Sign)
{
    UINT32 ret = ERR_SUCCESS;
    UINT8 value = 0;

    //    CSW_PROFILE_FUNCTION_ENTER(Cfg_Emod_ReadSign);
    ret = NV_GetEmodSign((UINT8*)(&value));
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in Cfg_Emod_ReadSign,1, call NV_GetEmodSign falsed, ret =0x%x\n",0x081002c0), ret);
        return ret;
    }
    *sim_Sign = value & 1;
    *nw_Sign = (value & 2) ? 1 : 0;
    //    CSW_PROFILE_FUNCTION_EXIT(Cfg_Emod_ReadSign);
    return ret;
}


UINT32 Cfg_Emod_UpdateSign(BOOL sim_Sign, BOOL nw_Sign)
{
    UINT32 ret = ERR_SUCCESS;
    UINT8 value = 0;

    //    CSW_PROFILE_FUNCTION_ENTER(Cfg_Emod_UpdateSign);
    value = (sim_Sign & 1) + ((nw_Sign & 1) << 1);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("in Cfg_Emod_UpdateSign,1, input sim_Sign=%d, nw_Sign=%d,value=0x%x\n",0x081002c2), sim_Sign, nw_Sign, value);
    ret = NV_SetEmodSign(value);
    if(ERR_SUCCESS != ret)
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in Cfg_Emod_UpdateSign,2, call NV_SetEmodSign falsed, ret =0x%x\n",0x081002c3), ret);

    //    CSW_PROFILE_FUNCTION_EXIT(Cfg_Emod_UpdateSign);
    return ret;
}


UINT32 CFW_CfgGetTimeParam(TM_FILETIME  *pTM)
{
    UINT32 ret = ERR_SUCCESS;
    UINT32 len = sizeof(TM_FILETIME);

    if(NULL == pTM)
        return ERR_CFW_INVALID_PARAMETER;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGetTimeParam);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgGetTimeParam enter\n",0x081002c4));
    ret = NV_GetTmTime((UINT8*)pTM, &len);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgGetTimeParam,1, call NV_GetTmTime failed,ret = 0x%x, zero memory here\n",0x081002c5), ret);
        SUL_ZeroMemory8(pTM, sizeof(TM_FILETIME));
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetTimeParam);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetTimeParam exit ret= 0x%x\n",0x081002c6), ret);
    return ret;
}

UINT32 CFW_CfgSetTimeParam(TM_FILETIME  *pTM)
{
    UINT32 ret = ERR_SUCCESS;
    UINT32 len;

    if(NULL == pTM)
        return ERR_CFW_INVALID_PARAMETER;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetTimeParam);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgSetTimeParam enter\n",0x081002c7));
    len = sizeof(TM_FILETIME);
    ret = NV_SetTmTime((UINT8*)pTM, len);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in CFW_CfgSetTimeParam,1, call NV_SetTmTime failed,ret = 0x%x\n",0x081002c8), ret);
    }

    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetTimeParam);
    return ret;
}


//]]hameina[+]2006.10.13

