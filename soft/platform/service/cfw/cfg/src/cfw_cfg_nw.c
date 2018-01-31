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

#include <sul.h>
#include <cfw.h>
#include "csw_debug.h"

#include "cfw_cfg.h"
#include <string.h>
UINT8 g_nNwDispFormat = 0;

UINT32 CFW_CfgSetNwStatus(UINT8 n, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetNwStatus);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CfgSetNwStatus, n=0x%x, nSimID=0x%x\n", 0x081001c5), n, nSimID);
    ret = CFW_CfgNumRangeCheck(n, NW_REG_RANGE);
    if (ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CfgNumRangeCheck, ret=0x%x\n", 0x081001c6), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetNwStatus);
        return ret;
    }
    if (CFW_SIM_0 == nSimID)
    {
        MemoryInfo.NwStatus0 = n;
    }
    else if (CFW_SIM_1 == nSimID)
    {
        MemoryInfo.NwStatus1 = n;
    }
    else if ((CFW_SIM_1 + 1) == nSimID)
    {
        MemoryInfo.NwStatus2 = n;
    }
    else if ((CFW_SIM_1 + 2) == nSimID)
    {
        MemoryInfo.NwStatus3 = n;
    }
    else
    {
        ret = ERR_CFW_INVALID_PARAMETER;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetNwStatus);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CfgSetNwStatus exit\n", 0x081001c7));
    return ret;
}

UINT32 CFW_CfgGetNwStatus(UINT8 *n, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CfgGetNwStatus, nSimID=0x%x\n", 0x081001c8), nSimID);
    if (CFW_SIM_0 == nSimID)
    {
        *n = MemoryInfo.NwStatus0;
    }
    else if (CFW_SIM_1 == nSimID)
    {
        *n = MemoryInfo.NwStatus1;
    }
    else if ((CFW_SIM_1 + 1) == nSimID)
    {
        *n = MemoryInfo.NwStatus2;
    }
    else if ((CFW_SIM_1 + 2) == nSimID)
    {
        *n = MemoryInfo.NwStatus3;
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CfgGetNwStatus, *n=0x%x\n", 0x081001c9), *n);
    return ERR_SUCCESS;
}

UINT32 CFW_CfgSetStoredPlmnList(CFW_StoredPlmnList *pPlmnL, CFW_SIM_ID nSimId)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CfgSetStoredPlmnList, nSimID=0x%x\n", 0x081001ca), nSimId);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("pPlmnL->nCount=0x%x\n", 0x081001cb), pPlmnL->nCount);
    CSW_TC_MEMBLOCK(CFW_CFG_TS_ID, (UINT8 *)pPlmnL->ArfcnL, 64, 16);
    if (pPlmnL != NULL)
    {
        if (nSimId <= CFW_SIM_COUNT)
        {
            u8 temp = 0;
            u8 tPlmn[3] = {
                0,
            };
            u16 tempL[CFW_STORE_MAX_COUNT] = {
                0,
            };
            UINT32 len = 0;
            len = sizeof(tPlmn);
            ret = NV_GetNwSimPlmn(nSimId, (UINT8 *)tPlmn, &len);
            if (ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("NV_GetNwSimPlmn, error ret=0x%x\n", 0x081001cc), ret);
                return ret;
            }

            ret = NV_GetNwSimBaCount(nSimId, &temp);
            if (ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("NV_GetNwSimBaCount, error ret=0x%x\n", 0x081001cc), ret);
                return ret;
            }
            len = sizeof(tempL);
            ret = NV_GetNwSimArfceList(nSimId, (UINT8 *)tempL, &len);
            if (ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("NV_GetNwSimArfceList, error ret=0x%x\n", 0x081001cc), ret);
                return ret;
            }
            len = sizeof(pPlmnL->Plmn);
            ret = NV_SetNwSimPlmn(nSimId, (UINT8 *)pPlmnL->Plmn, len);
            if (ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("NV_SetNwSimPlmn, error ret=0x%x\n", 0x081001cc), ret);
                return ret;
            }
            ret = NV_SetNwSimBaCount(nSimId, (UINT8)pPlmnL->nCount);
            if (ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("NV_SetNwSimBaCount, error ret=0x%x\n", 0x081001cc), ret);
                return ret;
            }
            len = sizeof(pPlmnL->ArfcnL);
            ret = NV_SetNwSimArfceList(nSimId, (UINT8 *)pPlmnL->ArfcnL, len);
            if (ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("NV_SetNwSimBaCount, error ret=0x%x\n", 0x081001cc), ret);
                return ret;
            }
            if (pPlmnL->nCount == CFW_STORE_MAX_COUNT)
            {
                ret = NV_SetNwSimStoreLast(nSimId, pPlmnL->ArfcnL[CFW_STORE_MAX_COUNT - 1]);
                if (ERR_SUCCESS != ret)
                {
                    len = sizeof(tPlmn);
                    ret = NV_SetNwSimPlmn(nSimId, (UINT8 *)tPlmn, len);
                    if (ERR_SUCCESS != ret)
                    {
                        CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("NV_SetNwSimPlmn, error ret=0x%x\n", 0x081001cc), ret);
                        return ret;
                    }
                    ret = NV_SetNwSimBaCount(nSimId, (UINT8)temp);
                    if (ERR_SUCCESS != ret)
                    {
                        CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("NV_SetNwSimBaCount, error ret=0x%x\n", 0x081001cc), ret);
                        return ret;
                    }
                    len = sizeof(tempL);
                    ret = NV_SetNwSimArfceList(nSimId, (UINT8 *)tempL, len);
                    if (ERR_SUCCESS != ret)
                    {
                        CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("NV_SetNwSimArfceList, error ret=0x%x\n", 0x081001cc), ret);
                        return ret;
                    }
                }
            }
        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
        }
    }
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CfgSetStoredPlmnList Success\n", 0x081001dc));

    return ret;
}

UINT32 CFW_CfgGetStoredPlmnList(CFW_StoredPlmnList *pPlmnL, CFW_SIM_ID nSimId)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CfgGetStoredPlmnList, nSimID=0x%x\n", 0x081001dd), nSimId);
    if (pPlmnL != NULL)
    {
        if (nSimId <= CFW_SIM_COUNT)
        {
            UINT32 len = 0;
            len = sizeof(pPlmnL->Plmn);
            ret = NV_GetNwSimPlmn(nSimId, (UINT8 *)pPlmnL->Plmn, &len);
            if (ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("NV_GetNwSimPlmn, error ret=0x%x\n", 0x081001cc), ret);
                return ret;
            }

            ret = NV_GetNwSimBaCount(nSimId, &pPlmnL->nCount);
            if (ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("NV_GetNwSimBaCount, error ret=0x%x\n", 0x081001cc), ret);
                return ret;
            }

            if (pPlmnL->nCount > CFW_STORE_MAX_COUNT)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Sim 0 Store Monitor List Nb is %d, beyond 33\n", 0x081001e2), pPlmnL->nCount);
                pPlmnL->nCount = 0;
                return ERR_CFG_READ_REG_FAILURE;
            }
            len = sizeof(pPlmnL->ArfcnL);
            ret = NV_GetNwSimArfceList(nSimId, (UINT8 *)pPlmnL->ArfcnL, &len);
            if (ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("NV_GetNwSimArfceList, error ret=0x%x\n", 0x081001cc), ret);
                return ret;
            }

            if (pPlmnL->nCount == CFW_STORE_MAX_COUNT)
            {
                ret = NV_GetNwSimStoreLast(nSimId, &(pPlmnL->ArfcnL[CFW_STORE_MAX_COUNT - 1]));
                if (ERR_SUCCESS != ret)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("NV_GetNwSimStoreLast, error ret=0x%x\n", 0x081001e6), ret);
                    pPlmnL->nCount = 0;
                    return ret;
                }
            }
        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
        }
    }
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("pPlmnL->nCount=0x%x\n", 0x08100202), pPlmnL->nCount);
    CSW_TC_MEMBLOCK(CFW_CFG_TS_ID, (UINT8 *)pPlmnL->ArfcnL, 64, 16);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CfgGetStoredPlmnList Success\n", 0x08100203));
    return ret;
}

UINT32 CFW_CfgNwSetFrequencyBand(UINT8 nBand, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgNwSetFrequencyBand);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CfgNwSetFrequencyBand, nBand =0x%x, nSimID=0x%x\n", 0x08100204), nBand, nSimID);
    if (nBand >= 0)
    {
        if ((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
        {
            ret = NV_SetNwSimFregBand(nSimID, nBand);
        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetCallWaiting);
            return ret;
        }
    }
    else
    {
        ret = ERR_CFG_PARAM_OUT_RANGR;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwSetFrequencyBand);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CfgNwSetFrequencyBand exit\n", 0x08100205));
    return ret;
}

UINT32 CFW_CfgNwGetFrequencyBand(
    UINT8 *pBand,
    CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgNwGetFrequencyBand);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CfgNwGetFrequencyBand, nSimID=0x%x\n", 0x08100206), nSimID);
    if (pBand != NULL)
    {
        if ((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
        {
            ret = NV_GetNwSimFregBand(nSimID, pBand);
            if (ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("in CFW_CfgNwGetFrequencyBand,1, call function NV_GetNwSimFregBand failed,ret=0x%x\n", 0x08100208), ret);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetFrequencyBand);
                return ret;
            }
        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetCallWaiting);
            return ret;
        }
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetFrequencyBand);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CfgGetNwStatus, *pBand =0x%x\n", 0x0810020a), *pBand);
    return ret;
}

#ifdef __MULTI_RAT__
UINT32 CFW_CfgNwSetFrequencyUMTSBand(UINT8 nBand, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgNwSetFrequencyUMTSBand);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL), "CFW_CfgNwSetFrequencyUMTSBand, nBand =0x%x, nSimID=0x%x\n", nBand, nSimID);
    if (nBand >= 0)
    {

        INT8 simid_c[2] = {0x30, 0x00};
        INT8 name_s[26] = {
            0x00,
        };
        INT8 name_s1[17] = {
            0x00,
        };

        strcpy(name_s, "NW_SIM");
        strcpy(name_s1, "_UMTS_BAND");
        if ((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
        {
            simid_c[0] = simid_c[0] + nSimID;
            strcat(name_s, simid_c);
            strcat(name_s, name_s1);
            ret = cfg_write_8("nw", name_s, nBand);
        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwSetFrequencyUMTSBand);
            return ret;
        }
    }
    else
        ret = ERR_CFG_PARAM_OUT_RANGR;
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwSetFrequencyUMTSBand);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL), "CFW_CfgNwSetFrequencyUMTSBand exit\n");
    return ret;
}

UINT32 CFW_CfgNwGetFrequencyUMTSBand(
    UINT8 *pBand,
    CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    BOOL bDef = FALSE;

    if ((nSimID < CFW_SIM_0) || (nSimID >= CFW_SIM_COUNT))
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgNwGetFrequencyUMTSBand);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CfgNwGetFrequencyUMTSBand, nSimID=0x%x\n", 0x08100206), nSimID);
    if (pBand != NULL)
    {
        ret = NV_GetNwSimFregBand(nSimID, pBand);
        if (ERR_SUCCESS != ret)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL), "in CFW_CfgNwGetFrequencyUMTSBand,1, call function NV_GetNwSimFregBand failed,ret=0x%x\n", ret);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetFrequencyUMTSBand);
            return ret;
        }
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetFrequencyUMTSBand);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL), "CFW_CfgNwGetFrequencyUMTSBand, *pBand =0x%x\n", *pBand);
    return ret;
}

#endif
UINT32 CFW_CfgNwSetNetWorkMode(UINT8 nMode, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgNwSetNetWorkMode);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CfgNwSetNetWorkMode, nMode=0x%x, nSimID=0x%x\n", 0x0810020b), nMode, nSimID);
    ret = CFW_CfgNumRangeCheck(nMode, NW_WORKMODE_RANGE);

    if (ERR_SUCCESS == ret)
    {
        if ((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
        {
            ret = NV_SetNwSimWorkMode(nSimID, nMode);
        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwSetNetWorkMode);
            return ret;
        }
    }
    else
    {
        ret = ERR_CFW_INVALID_PARAMETER;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwSetNetWorkMode);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CfgNwSetNetWorkMode exit\n", 0x0810020c));
    return ret;
}

UINT32 CFW_CfgNwGetNetWorkMode(UINT8 *pMode, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CfgNwGetNetWorkMode, nSimID=0x%x\n", 0x0810020d), nSimID);
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgNwGetNetWorkMode);
    if (NULL == pMode)
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    else
    {
        if ((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
        {
            ret = NV_GetNwSimWorkMode(nSimID, pMode);
            if (ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("NV_GetNwSimWorkMode, error ret=0x%x\n", 0x0810020f), ret);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetNetWorkMode);
                return ret;
            }
        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetNetWorkMode);
            return ret;
        }
        ret = CFW_CfgNumRangeCheck(*pMode, NW_WORKMODE_RANGE);

        if (ERR_SUCCESS != ret)
        {
            CSW_ASSERT(0);
        }
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetNetWorkMode);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CfgNwGetNetWorkMode, *pMode =0x%x\n", 0x08100211), *pMode);
    return ret;
}

#ifdef CFW_GPRS_SUPPORT
#if 0

UINT32 CFW_CfgGprsSetPdpCxt(UINT8 nCid, CFW_GPRS_PDPCONT_INFO *pPdpCont, CFW_SIM_ID nSimID)
{
    UINT32  ret = ERR_SUCCESS;
    UINT8 pdpType[REG_VALUENAME_LEN_MAX + 1];
    UINT8 pdpDCmp[REG_VALUENAME_LEN_MAX + 1];
    UINT8 pdpHCmp[REG_VALUENAME_LEN_MAX + 1];
    UINT8 pdpAPN[REG_VALUENAME_LEN_MAX + 1];
    UINT8 pdpAdr[REG_VALUENAME_LEN_MAX + 1];
    UINT8 *Cid = 0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGprsSetPdpCxt);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgGprsSetPdpCxt, nCid=0x%x, nSimID=0x%x\n",0x08100212), nCid, nSimID);
    ret = CFW_CfgNumRangeCheck(nCid, GPRS_CIND_RANGE);
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgNumRangeCheck, error ret=0x%x\n",0x08100213), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsSetPdpCxt);
        return ERR_CFG_PARAM_OUT_RANGR;
    }
    if(pPdpCont == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsSetPdpCxt);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if((pPdpCont->nPdpType > 2)
            || (pPdpCont->nDComp > 3)
            || (pPdpCont->nHComp > 4)
            || (pPdpCont->nApnSize > 100)
            || (pPdpCont->nPdpAddrSize > 18))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsSetPdpCxt);
        return ERR_CFG_PARAM_OUT_RANGR;
    }
    nCid = nCid + 0x30;
    Cid = &nCid;

    //PDP_TYPE
    if(CFW_SIM_0 == nSimID)
    {
        SUL_StrCopy(pdpType, "PDP_SIM0_TYPE");
    }
    else if (CFW_SIM_1 == nSimID)
    {
        SUL_StrCopy(pdpType, "PDP_SIM1_TYPE");
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    SUL_StrCat(pdpType, Cid);
    pdpType[9] = '\0';

    //PDP_DATA_C
    if(CFW_SIM_0 == nSimID)
    {
        SUL_StrCopy(pdpDCmp, "PDP_SIM0_DATA_C");
    }
    else if (CFW_SIM_1 == nSimID)
    {
        SUL_StrCopy(pdpDCmp, "PDP_SIM1_DATA_C");
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    SUL_StrCat(pdpDCmp, Cid);
    pdpDCmp[11] = '\0';

    //PDP_HEAD_C
    if(CFW_SIM_0 == nSimID)
    {
        SUL_StrCopy(pdpHCmp, "PDP_SIM0_HEAD_C");
    }
    else if (CFW_SIM_1 == nSimID)
    {
        SUL_StrCopy(pdpHCmp, "PDP_SIM1_HEAD_C");
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    SUL_StrCat(pdpHCmp, Cid);
    pdpHCmp[11] = '\0';

    //PDP_APN
    if(CFW_SIM_0 == nSimID)
    {
        SUL_StrCopy(pdpAPN, "PDP_SIM0_APN");
    }
    else if (CFW_SIM_1 == nSimID)
    {
        SUL_StrCopy(pdpAPN, "PDP_SIM1_APN");
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    SUL_StrCat(pdpAPN, Cid);
    pdpAPN[8] = '\0';
    //PDP_ADDRESS
    if(CFW_SIM_0 == nSimID)
    {
        SUL_StrCopy(pdpAdr, "PDP_SIM0_ADR");
    }
    else if (CFW_SIM_1 == nSimID)
    {
        SUL_StrCopy(pdpAdr, "PDP_SIM1_ADR");
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    SUL_StrCat(pdpAdr, Cid);
    pdpAdr[8] = '\0';

    ret = cfg_write_8("PDP", pdpType, pPdpCont->nPdpType);

    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_write_8 pdpType, error ret=0x%x\n",0x08100214), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsSetPdpCxt);
        return ret;
    }
    ret = cfg_write_8("PDP", pdpDCmp, pPdpCont->nDComp);
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_write_8 pdpDCmp, error ret=0x%x\n",0x08100215), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsSetPdpCxt);
        return ret;
    }
    ret = cfg_write_8("PDP", pdpHCmp, pPdpCont->nHComp);
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_write_8 pdpHCmp, error ret=0x%x\n",0x08100216), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsSetPdpCxt);
        return ret;
    }
    ret = cfg_write_str("PDP", pdpAPN, pPdpCont->pApn, pPdpCont->nApnSize);
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_write_8 pdpAPN, error ret=0x%x\n",0x08100217), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsSetPdpCxt);
        return ret;
    }

    ret = cfg_write_str("PDP", pdpAdr, pPdpCont->pPdpAddr, pPdpCont->nPdpAddrSize);
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsSetPdpCxt);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgGprsSetPdpCxt exit\n",0x08100218));
    if(ret != ERR_SUCCESS)
        return ret;
    return ERR_SUCCESS;
}

//=============================================================================
//[in]nCid[1 - 7], [out]pPdpCont
//=============================================================================
UINT32 CFW_CfgGprsGetPdpCxt(UINT8 nCid, CFW_GPRS_PDPCONT_INFO *pPdpCont, CFW_SIM_ID nSimID)
{
    UINT32  ret = ERR_SUCCESS;

    UINT8 pdpType[REG_VALUENAME_LEN_MAX + 1];
    UINT8 pdpDCmp[REG_VALUENAME_LEN_MAX + 1];
    UINT8 pdpHCmp[REG_VALUENAME_LEN_MAX + 1];
    UINT8 pdpAPN[REG_VALUENAME_LEN_MAX + 1];
    UINT8 pdpAdr[REG_VALUENAME_LEN_MAX + 1];
    BOOL bDef = FALSE;
    UINT8 *Cid = 0;
    UINT8 datasize = 0;
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgNwSetNetWorkMode, nCid=0x%x, nSimID=0x%x\n",0x08100219), nCid, nSimID);
    if(pPdpCont == NULL || pPdpCont->pApn == NULL || pPdpCont->pPdpAddr == NULL)
        return ERR_CFW_INVALID_PARAMETER;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGprsGetPdpCxt);

    ret = CFW_CfgNumRangeCheck(nCid, GPRS_CIND_RANGE);
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgNumRangeCheck, error ret=0x%x\n",0x0810021a), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetPdpCxt);
        return ERR_CFG_PARAM_OUT_RANGR;
    }
    nCid = nCid + 0x30;
    Cid = &nCid;

    //PDP_TYPE
    if(CFW_SIM_0 == nSimID)
    {
        SUL_StrCopy(pdpType, "PDP_SIM0_TYPE");
    }
    else if (CFW_SIM_1 == nSimID)
    {
        SUL_StrCopy(pdpType, "PDP_SIM1_TYPE");
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    SUL_StrCat(pdpType, Cid);
    pdpType[9] = '\0';

    //PDP_DATA_C
    if(CFW_SIM_0 == nSimID)
    {
        SUL_StrCopy(pdpDCmp, "PDP_SIM0_DATA_C");
    }
    else if (CFW_SIM_1 == nSimID)
    {
        SUL_StrCopy(pdpDCmp, "PDP_SIM1_DATA_C");
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    SUL_StrCat(pdpDCmp, Cid);
    pdpDCmp[11] = '\0';

    //PDP_HEAD_C
    if(CFW_SIM_0 == nSimID)
    {
        SUL_StrCopy(pdpHCmp, "PDP_SIM0_HEAD_C");
    }
    else if (CFW_SIM_1 == nSimID)
    {
        SUL_StrCopy(pdpHCmp, "PDP_SIM1_HEAD_C");
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    SUL_StrCat(pdpHCmp, Cid);
    pdpHCmp[11] = '\0';

    //PDP_APN
    if(CFW_SIM_0 == nSimID)
    {
        SUL_StrCopy(pdpAPN, "PDP_SIM0_APN");
    }
    else if (CFW_SIM_1 == nSimID)
    {
        SUL_StrCopy(pdpAPN, "PDP_SIM1_APN");
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    SUL_StrCat(pdpAPN, Cid);
    pdpAPN[8] = '\0';

    //PDP_ADDRESS
    if(CFW_SIM_0 == nSimID)
    {
        SUL_StrCopy(pdpAdr, "PDP_SIM0_ADR");
    }
    else if (CFW_SIM_1 == nSimID)
    {
        SUL_StrCopy(pdpAdr, "PDP_SIM1_ADR");
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    SUL_StrCat(pdpAdr, Cid);
    pdpAdr[8] = '\0';

    // nPdpType
    ret = cfg_read_8("PDP", pdpType, &(pPdpCont->nPdpType));
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_read_8, error ret=0x%x\n",0x0810021b), ret);
        ret = cfg_readdefault("PDP", pdpType, &(pPdpCont->nPdpType));
        if( ERR_SUCCESS != ret)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_readdefault, error ret=0x%x\n",0x0810021c), ret);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetPdpCxt);
            return ret;
        }
        bDef = TRUE;
    }
    else
    {
        if(pPdpCont->nPdpType > 2)
        {
            ret = cfg_readdefault("PDP", pdpType, &(pPdpCont->nPdpType));
            if( ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_readdefault, error ret=0x%x\n",0x0810021d), ret);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetPdpCxt);
                return ret;
            }
            bDef = TRUE;
        }
    }
    if(bDef)
        cfg_write_8("PDP", pdpType, pPdpCont->nPdpType);

    // nDComp
    ret = cfg_read_8("PDP", pdpDCmp, &(pPdpCont->nDComp));
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_read_8 pdpDCmp, error ret=0x%x\n",0x0810021e), ret);
        ret = cfg_readdefault("PDP", pdpDCmp, &(pPdpCont->nDComp));
        if(ERR_SUCCESS != ret)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_readdefault, error ret=0x%x\n",0x0810021f), ret);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetPdpCxt);
            return ret;
        }
        bDef = TRUE;
    }
    else
    {
        if(pPdpCont->nDComp > 3)
        {
            ret = cfg_readdefault("PDP", pdpDCmp, &(pPdpCont->nDComp));
            if(ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_readdefault, error ret=0x%x\n",0x08100220), ret);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetPdpCxt);
                return ret;
            }
            bDef = TRUE;
        }
    }
    if(bDef)
        cfg_write_8("PDP", pdpDCmp, pPdpCont->nDComp);

    // nHComp
    ret = cfg_read_8("PDP", pdpHCmp, &(pPdpCont->nHComp));
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_read_8 pdpHCmp, error ret=0x%x\n",0x08100221), ret);
        ret = cfg_readdefault("PDP", pdpHCmp, &(pPdpCont->nHComp));
        if(ERR_SUCCESS != ret)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_readdefault, error ret=0x%x\n",0x08100222), ret);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetPdpCxt);
            return ret;
        }
        bDef = TRUE;
    }
    else
    {
        if(pPdpCont->nHComp > 4)
        {
            ret = cfg_readdefault("PDP", pdpHCmp, &(pPdpCont->nHComp));
            if(ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_readdefault, error ret=0x%x\n",0x08100223), ret);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetPdpCxt);
                return ret;
            }
            bDef = TRUE;
        }
    }
    if(bDef)
        cfg_write_8("PDP", pdpHCmp, pPdpCont->nHComp);

    //  pApn
    datasize = pPdpCont->nApnSize;
    ret = cfg_read_str("PDP", pdpAPN, (pPdpCont->pApn), &datasize);
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_read_str pdpAPN, error ret=0x%x\n",0x08100224), ret);
        ret = cfg_readdefault("PDP", pdpAPN, (pPdpCont->pApn));
        if(ERR_SUCCESS != ret)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_readdefault pdpAPN, error ret=0x%x\n",0x08100225), ret);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetPdpCxt);
            return ret;
        }
        bDef = TRUE;
    }
    pPdpCont->nApnSize = datasize;

    if(bDef)
    {
        bDef = FALSE;
        cfg_write_str("PDP", pdpAPN, pPdpCont->pApn, pPdpCont->nApnSize);
    }

    // pPdpAddr
    datasize = pPdpCont->nPdpAddrSize;
    ret = cfg_read_str("PDP", pdpAdr, (pPdpCont->pPdpAddr), &datasize);
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_read_str pdpAdr, error ret=0x%x\n",0x08100226), ret);
        ret = cfg_readdefault("PDP", pdpAdr, (pPdpCont->pPdpAddr));
        if(ret != ERR_SUCCESS)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_readdefault pdpAdr, error ret=0x%x\n",0x08100227), ret);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetPdpCxt);
            return ret;
        }
        bDef = TRUE;
    }
    pPdpCont->nPdpAddrSize = (UINT8)datasize;

    if(bDef)
        cfg_write_str("PDP", pdpAdr, pPdpCont->pPdpAddr, pPdpCont->nPdpAddrSize);
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetPdpCxt);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgGprsGetPdpCxt exit\n",0x08100228));
    return ERR_SUCCESS;

}


//=============================================================================================
//AT+CGQREQ
//This command allows the TE to specify a Quality of Service Profile
//that is used when the MT sends an Activate PDP Context Request message
///to the network.

//[in]nCid [1 - 7], [in]pQos

//=============================================================================================
UINT32 CFW_CfgGprsSetReqQos(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID)
{
    UINT32  ret = ERR_SUCCESS;
    UINT8 name[REG_VALUENAME_LEN_MAX + 1];
    UINT8 *Cid = 0;
    if( pQos == NULL)
        return ERR_CFW_INVALID_PARAMETER;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGprsSetReqQos);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgGprsSetReqQos, nCid=0x%x, nSimID=0x%x\n",0x08100229), nCid, nSimID);
    ret = CFW_CfgNumRangeCheck(nCid, GPRS_CIND_RANGE);
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgNumRangeCheck, error ret=0x%x\n",0x0810022a), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsSetReqQos);
        return ERR_CFG_PARAM_OUT_RANGR;
    }

    if((pQos->nDelay > 7) || (pQos->nMean > 31) || (pQos->nPeak > 15)
            || (pQos->nPrecedence > 7) || (pQos->nReliability > 7))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsSetReqQos);
        return ERR_CFW_INVALID_PARAMETER;
    }
    nCid = nCid + 0x30;
    Cid = &nCid;
    if(CFW_SIM_0 == nSimID)
    {
        SUL_StrCopy(name, "SIM0_QOS");
    }
    else if (CFW_SIM_1 == nSimID)
    {
        SUL_StrCopy(name, "SIM1_QOS");
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    SUL_StrCat(name, Cid);

    ret = cfg_write_hex("PDP", name, (PVOID) pQos, sizeof(CFW_GPRS_QOS));
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsSetReqQos);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgGprsSetReqQos exit\n",0x0810022b));
    return ret;
}


//=============================================================================================
// [in]nCid[1 - 7], [out]pQos
//=============================================================================================
UINT32 CFW_CfgGprsGetReqQos(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID)
{
    UINT32  ret = ERR_SUCCESS;
    UINT8 name[REG_VALUENAME_LEN_MAX + 1];
    UINT8 *Cid = 0;

    if( pQos == NULL)
        return ERR_CFW_INVALID_PARAMETER;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGprsGetReqQos);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgGprsGetReqQos, nCid=0x%x, nSimID=0x%x\n",0x0810022c), nCid, nSimID);
    ret = CFW_CfgNumRangeCheck(nCid, GPRS_CIND_RANGE);
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgNumRangeCheck, error ret=0x%x\n",0x0810022d), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetReqQos);
        return ERR_CFG_PARAM_OUT_RANGR;
    }

    nCid = nCid + 0x30;
    Cid = &nCid;
    if(CFW_SIM_0 == nSimID)
    {
        SUL_StrCopy(name, "SIM0_QOS");
    }
    else if (CFW_SIM_1 == nSimID)
    {
        SUL_StrCopy(name, "SIM1_QOS");
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    SUL_StrCat(name, Cid);

    ret = cfg_read_hex("PDP", name, (PVOID) pQos, (UINT8)sizeof(CFW_GPRS_QOS));
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_read_hex, error ret=0x%x\n",0x0810022e), ret);
        ret = cfg_readdefault("PDP", name, (PVOID) pQos);
        if(ERR_SUCCESS != ret)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_readdefault, error ret=0x%x\n",0x0810022f), ret);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetReqQos);
            return ret;
        }
    }
    else
    {
        if((pQos->nDelay > 7) || (pQos->nMean > 31) || (pQos->nPeak > 15)
                || (pQos->nPrecedence > 7) || (pQos->nReliability > 7))
        {
            ret = cfg_readdefault("PDP", name, (PVOID) pQos);
            if(ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_readdefault, error ret=0x%x\n",0x08100230), ret);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetReqQos);
                return ret;
            }
        }
    }

    return ret;
}


//===========================================================================
//[in]nCid[1 - 7]
//===========================================================================
UINT32 CFW_CfgGprsSetMinQos(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID)
{
    UINT32  ret = ERR_SUCCESS;
    UINT8 name[REG_VALUENAME_LEN_MAX + 1];
    UINT8 *Cid = 0;

    if( pQos == NULL)
        return ERR_CFW_INVALID_PARAMETER;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGprsSetMinQos);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgGprsSetMinQos, nCid=0x%x, nSimID=0x%x\n",0x08100231), nCid, nSimID);
    ret = CFW_CfgNumRangeCheck(nCid, GPRS_CIND_RANGE);
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgNumRangeCheck, error ret=0x%x\n",0x08100232), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsSetMinQos);
        return ERR_CFG_PARAM_OUT_RANGR;
    }
    if((pQos->nDelay > 7) || (pQos->nMean > 31) || (pQos->nPeak > 15)
            || (pQos->nPrecedence > 7) || (pQos->nReliability > 7))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsSetMinQos);
        return ERR_CFW_INVALID_PARAMETER;
    }

    nCid = nCid + 0x30;
    Cid = &nCid;
    if(CFW_SIM_0 == nSimID)
    {
        SUL_StrCopy(name, "SIM0_MINQOS");
    }
    else if (CFW_SIM_1 == nSimID)
    {
        SUL_StrCopy(name, "SIM1_MINQOS");
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    SUL_StrCat(name, Cid);

    ret = cfg_write_hex("PDP", name, (PVOID) pQos, sizeof(CFW_GPRS_QOS));
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsSetMinQos);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgGprsSetMinQos exit\n",0x08100233));
    return ret;
}

/*===========================================================================
//[in]nCid[1 - 7]
===========================================================================*/
UINT32 CFW_CfgGprsGetMInQos(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID)
{
    UINT32  ret = ERR_SUCCESS;
    UINT8 name[REG_VALUENAME_LEN_MAX + 1];
    UINT8 *Cid = 0;
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgGprsGetReqQos, nCid=0x%x, nSimID=0x%x\n",0x08100234), nCid, nSimID);
    if( pQos == NULL)
        return ERR_CFW_INVALID_PARAMETER;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGprsGetMInQos);

    ret = CFW_CfgNumRangeCheck(nCid, GPRS_CIND_RANGE);
    if(ret != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgNumRangeCheck, error ret=0x%x\n",0x08100235), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetMInQos);
        return ERR_CFG_PARAM_OUT_RANGR;
    }

    nCid = nCid + 0x30;
    Cid = &nCid;
    if(CFW_SIM_0 == nSimID)
    {
        SUL_StrCopy(name, "SIM0_MINQOS");
    }
    else if (CFW_SIM_1 == nSimID)
    {
        SUL_StrCopy(name, "SIM1_MINQOS");
    }
    else
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    SUL_StrCat(name, Cid);

    ret = cfg_read_hex("PDP", name, (PVOID) pQos, sizeof(CFW_GPRS_QOS));
    if(ERR_SUCCESS != ret)
    {
        ret = cfg_readdefault("PDP", name, (PVOID) pQos);
        if(ERR_SUCCESS != ret)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("in CFW_CfgGprsGetReqQos,1, call cfg_readdefault failed\n",0x08100236));
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetMInQos);
            return ret;
        }
    }
    else
    {
        if((pQos->nDelay > 7) || (pQos->nMean > 31) || (pQos->nPeak > 15)
                || (pQos->nPrecedence > 7) || (pQos->nReliability > 7))
        {
            ret = cfg_readdefault("PDP", name, (PVOID) pQos);
            if(ERR_SUCCESS != ret)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("in CFW_CfgGprsGetReqQos,2, call cfg_readdefault failed\n",0x08100237));
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGprsGetMInQos);
                return ret;
            }
        }
    }

    return ret;
}
#endif
//add for gprs data statistics by wuys 2010-09-03
INT32 gprs_sum[CFW_SIM_COUNT][2] = {
    {0},
};
UINT32 gprs_SetGprsSum(INT32 upsum, INT32 downsum, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    if ((CFW_SIM_0 <= nSimID) && (CFW_SIM_COUNT > nSimID))
    {
        ret = NV_SetGprsUpSumSim(nSimID, upsum);
        ret = NV_SetGprsDwnSumSim(nSimID, downsum);
    }
    else
        ret = ERR_CFW_INVALID_PARAMETER;

    return ret;
}
UINT32 gprs_GetGprsSum(INT32 *upsum, INT32 *downsum, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    if ((CFW_SIM_0 <= nSimID) && (CFW_SIM_COUNT > nSimID))
    {
        ret = NV_GetGprsUpSumSim(nSimID, upsum);
        ret = NV_GetGprsDwnSumSim(nSimID, downsum);
    }
    else
        ret = ERR_CFW_INVALID_PARAMETER;

    return ret;
}
UINT32 CFW_GetGprsSum(INT32 *upsum, INT32 *downsum, CFW_SIM_ID nSimID)
{

    *upsum = gprs_sum[nSimID][0];
    *downsum = gprs_sum[nSimID][1];
    return ERR_SUCCESS;
}

UINT32 CFW_ClearGprsSum(CFW_SIM_ID nSimID)
{
    gprs_SetGprsSum(0, 0, nSimID);
    gprs_sum[nSimID][0] = 0;
    gprs_sum[nSimID][1] = 0;
    return ERR_SUCCESS;
}

//add end
#endif

//=================================================================
//CFW_CfgNwSetFM
//
//=================================================================
UINT32 CFW_CfgNwSetFM(CFW_COMM_MODE bMode, CFW_SIM_ID nSimID)
{
    UINT32 nRet = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgNwSetFM);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CfgNwSetFM, bMode =0x%x, nSimID=0x%x\n", 0x08100238), bMode, nSimID);
    if ((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
    {
        nRet = NV_SetNwSimFlightMode(nSimID, bMode);
    }
    else
    {
        nRet = ERR_CFW_INVALID_PARAMETER;
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetCallWaiting);
        return nRet;
    }

    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CfgNwSetFM exit\n", 0x08100239));
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwSetFM);
    return nRet;
}

//=================================================================
//CFW_CfgNwGetFM
//
//=================================================================
UINT32 CFW_CfgNwGetFM(CFW_COMM_MODE *bMode, CFW_SIM_ID nSimID)
{
    UINT32 nRet = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgNwGetFM);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CfgNwGetFM, nSimID=0x%x\n", 0x0810023a), nSimID);

    if (NULL == bMode)
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    else
    {
        if ((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
        {
            nRet = NV_GetNwSimFlightMode(nSimID, (UINT32 *)bMode);
            if (ERR_SUCCESS != nRet)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("in NW_FLIGHT_MODE,1, call NV_GetNwSimFlightMode false,ret=0x%x\n", 0x0810023c), nRet);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetFM);
                return nRet;
            }
            nRet = CFW_CfgNumRangeCheck(*bMode, NW_FLIGHTMODE_RANGE);
            if (ERR_SUCCESS != nRet)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("in CFW_CfgNwGetFM,1, call NV_GetNwSimFlightMode false,ret=0x%x\n", 0x0810023e), nRet);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetFM);
                return nRet;
            }
        }
        else
        {
            nRet = ERR_CFW_INVALID_PARAMETER;
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetFM);
            return nRet;
        }
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetFM);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CfgNwGetFM exit\n", 0x0810023f));
    return ERR_SUCCESS;
}
#if 0
UINT32 CFW_CfgNwSetIpr( UART_BAUDRATE bMode)
{
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgNwSetIpr);
    NV_SetNwIpr(bMode);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgNwSetFM exit\n",0x08100239));
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwSetIpr);
    return ERR_SUCCESS;
}


//=================================================================
//CFW_CfgNwGetFM
//
//=================================================================
UINT32 CFW_CfgNwGetIpr(UART_BAUDRATE *bMode)
{
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgNwGetIpr);
    UINT32 nRet = NV_GetNwIpr((UINT32*)bMode);
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetIpr);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgNwGetFM exit\n",0x0810023f));
    return nRet;

}
#endif

/*==============================================================================


==============================================================================*/
UINT32 CFW_CfgNwGetOperatorId(
    UINT8 **pOperatorId,
    UINT8 *pOperatorName)
{
    //UINT32 ret = ERR_SUCCESS;
    UINT32 nIndex = 0;
    INT32 nCmpResult = 0;
    const UINT8 *pName = 0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgNwGetOperatorId);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CfgNwGetOperatorId enter\n", 0x08100240));
    do
    {
        pName = OperatorInfo[nIndex].OperatorName;
        nCmpResult = CFW_Cfg_strcmp(pOperatorName, pName);

        if (!nCmpResult)
        {
            *pOperatorId = (UINT8 *)OperatorInfo[nIndex].OperatorId;
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetOperatorId);
            return ERR_SUCCESS;
        }
        nIndex++;

    } while (pName != 0);

    *pOperatorId = 0;
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetOperatorId);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CfgNwGetOperatorId exit\n", 0x08100241));
    return ERR_CFW_NOT_EXIST_OPERATOR_NAME;
}

UINT32 CFW_CfgNwGetOperatorName(
    UINT8 pOperatorId[6],
    UINT8 **pOperatorName)
{
    UINT32 nIndex = 0;
    INT32 nCmpResult = 0;
    UINT32 i = 0;
    const UINT8 *pOperID = 0;
    const UINT8 *pName = 0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgNwGetOperatorName);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(6), TSTR("in CFW_CfgNwGetOperatorName,input pOperatorId=%d%d%d%d%d%d\n", 0x08100242), pOperatorId[0], pOperatorId[1], pOperatorId[2], pOperatorId[3], pOperatorId[4], pOperatorId[5]);
    do
    {
        pName = OperatorInfo[nIndex].OperatorName;
        pOperID = OperatorInfo[nIndex].OperatorId;

        if (pName == 0)
        {
            *pOperatorName = 0;
            CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("*pOperatorName = 0\n", 0x08100243));
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetOperatorName);
            return ERR_CFW_NOT_EXIST_OPERATOR_ID;
        }

        nCmpResult = 0;
        for (i = 0; i < 6; i++)
        {
            if (*pOperID == pOperatorId[i])
            {
                pOperID++;
            }
            else
            {
                nCmpResult = 1;

                break;
            }
        }

        if (!nCmpResult)
        {
            *pOperatorName = OperatorInfo[nIndex].OperatorName;
            CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1) | TSMAP(1), TSTR("NwGetOperatorName,output names=%s\n", 0x08100244), OperatorInfo[nIndex].OperatorName);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetOperatorName);
            return ERR_SUCCESS;
        }
        nIndex++;

    } while (pName != 0);

    *pOperatorName = 0;

    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CfgNwGetOperatorName exit\n", 0x08100245));
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetOperatorName);

    return ERR_CFW_NOT_EXIST_OPERATOR_ID;
}

UINT32 CFW_CfgNwGetCompanyName(
    UINT8 pOperatorId[6],
    UINT8 **Company)
{
    // UINT32 ret = ERR_SUCCESS;
    UINT32 nIndex = 0;
    INT32 nCmpResult = 0;
    UINT8 i = 0;
    const UINT8 *pOperID = 0;
    const UINT8 *pName = 0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgNwGetCompanyName);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(6), TSTR("in CFW_CfgNwGetCompanyName,input pOperatorId=%d%d%d%d%d%d\n", 0x08100246), pOperatorId[0], pOperatorId[1], pOperatorId[2], pOperatorId[3], pOperatorId[4], pOperatorId[5]);
    do
    {
        pName = SpecialOperatorInfo[nIndex].CompanyName;
        pOperID = SpecialOperatorInfo[nIndex].OperatorId;

        if (pName == 0)
        {
            *Company = 0;
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetCompanyName);
            return ERR_CFW_NOT_EXIST_OPERATOR_ID;
        }

        nCmpResult = 0;
        for (i = 0; i < 6; i++)
        {
            if (*pOperID == pOperatorId[i])
            {
                pOperID++;
            }
            else
            {
                nCmpResult = 1;

                break;
            }
        }

        if (!nCmpResult)
        {
            *Company = SpecialOperatorInfo[nIndex].CompanyName;
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetCompanyName);
            return ERR_SUCCESS;
        }
        nIndex++;

    } while (pName != 0);

    *Company = 0;
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetCompanyName);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CfgNwGetCompanyName exit\n", 0x08100247));
    return ERR_CFW_NOT_EXIST_OPERATOR_ID;
}

UINT32 CFW_CfgNwGetOperatorInfo(UINT8 **pOperatorId, UINT8 **pOperatorName, UINT32 nIndex)
{
    UINT32 n = 0;
    UINT8 *pName = 0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgNwGetOperatorInfo);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CfgNwGetOperatorId enter\n", 0x08100248));
    do
    {
        pName = OperatorInfo[n].OperatorName;
        if (pName == 0)
        {
            *pOperatorName = 0;
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetOperatorInfo);
            return ERR_CFW_NOT_EXIST_OPERATOR_ID;
        }
        if (n == nIndex)
        {
            *pOperatorId = (UINT8 *)OperatorInfo[nIndex].OperatorId;
            *pOperatorName = (UINT8 *)OperatorInfo[nIndex].OperatorName;
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetOperatorInfo);
            return ERR_SUCCESS;
        }
        n++;
    } while (pName != 0);
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgNwGetOperatorInfo);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CfgNwGetOperatorName exit\n", 0x08100249));
    return ERR_CFW_NOT_EXIST_OPERATOR_ID;
}
/*=============================================================================
Get display format of Network, when register.

==============================================================================*/
UINT32 CFW_CfgSetNwOperDispFormat(UINT8 format)
{
    UINT32 ret = ERR_SUCCESS;

    ret = CFW_CfgNumRangeCheck(format, NW_DispFormat_RANGE);
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CfgSetNwOperDispFormat, format = 0x%x,ret=0x%x\n", 0x0810024a), format, ret);
    if (ERR_SUCCESS == ret)
    {
        g_nNwDispFormat = format;
    }
    return ERR_CFW_NOT_SUPPORT;
}

UINT32 CFW_CfgGetNwOperDispFormat(UINT8 *format)
{
    UINT32 ret = ERR_SUCCESS; //mabo changed,20070726
    //READ_NUMBER_SETTING_8("REG_DISP_FORMAT", *format, "nw");
    *format = g_nNwDispFormat;
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CfgGetNwOperDispFormat, format=%d\n", 0x0810024b), *format);
    return ret;
}

//add for set APN for Jmetoc application by wuys, 2010-08-25
UINT32 CFW_SetJmetocApn(const INT8 *apn)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" CFW_SetJmetocApn apn.", 0x0810024c)));
    ret = NV_SetJmetocApn((UINT8 *)apn, SUL_Strlen(apn));
    return ret;
}

UINT32 CFW_SetJmetocUser(const INT8 *user)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" CFW_SetJmetocApn user.", 0x0810024d)));
    ret = NV_SetJmetocUser((UINT8 *)user, SUL_Strlen(user));
    return ret;
}

UINT32 CFW_SetJmetocPwd(INT8 *pwd)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" CFW_SetJmetocApn pwd.", 0x0810024e)));
    ret = NV_SetJmetocPwd(pwd, SUL_Strlen(pwd));
    return ret;
}

UINT32 CFW_GetJmetocApn(INT8 *apn)
{
    UINT32 ret = ERR_SUCCESS;
    UINT32 nSize = 63;

    ret = NV_GetJmetocApn(apn, &nSize);
    return ret;
}

UINT32 CFW_GetJmetocUser(INT8 *username)
{
    UINT32 ret = ERR_SUCCESS;
    UINT32 nUserSize = 63;

    ret = NV_GetJmetocUser(username, &nUserSize);
    return ret;
}
UINT32 CFW_GetJmetocPwd(INT8 *pwd)
{
    UINT32 ret = ERR_SUCCESS;
    UINT32 nPwdSize = 63;

    ret = NV_GetJmetocPwd(pwd, &nPwdSize);
    return ret;
}
//add end

#ifdef AT_USER_DBS

#include "tgt_m.h"
//INT8 set_test_dbsSeed[2][63]={0};
//INT8 get_test_dbsSeed[2][63]={0};
//int32 CFW_SetDBSSeed_timebegin=0;

UINT32 CFW_SetDBSSeed(INT8 *seed)
{
    UINT32 ret = ERR_SUCCESS;
    TGT_FACTORY_SETTINGS_T *pfact;

    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" CFW_SetDBSSeed DBS\n", 0x0810024f)));
    if (NULL == seed)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" CFW_SetDBSSeed DBS Seed==NULL\n", 0x08100250)));
        return ERR_EMPTY_PRI_DNSSERVER_IP;
    }
    if (0 == strlen(seed) || strlen(seed) > sizeof(pfact->dbs_info.dbsSeed))
    {
        return ERR_CMS_INVALID_LENGTH;
    }
    pfact = tgt_GetFactorySettings();
    if (pfact)
    {
        memcpy(pfact->dbs_info.dbsSeed, seed, strlen(seed));
    }
    ret = tgt_SetFactorySettings(pfact);

    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR(" CFW_SetDBSSeed ret =%x \n", 0x08100251)), ret);
    return ret;
}

UINT32 CFW_GetDBSSeed(INT8 *seed)
{
    UINT32 ret = ERR_SUCCESS;
    TGT_FACTORY_SETTINGS_T *pfact;
    UINT32 seed_size;

    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" CFW_GetDBSSeed DBS\n", 0x08100252)));
    if (NULL == seed)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" CFW_GetDBSSeed DBS Seed==NULL\n", 0x08100253)));
        return ERR_EMPTY_PRI_DNSSERVER_IP;
    }
    pfact = tgt_GetFactorySettings();
    if (pfact)
    {
        if (strlen(pfact->dbs_info.dbsSeed) > 0)
        {
            seed_size = strlen(pfact->dbs_info.dbsSeed) <= sizeof(pfact->dbs_info.dbsSeed) ? strlen(pfact->dbs_info.dbsSeed) : sizeof(pfact->dbs_info.dbsSeed);
            memcpy(seed, pfact->dbs_info.dbsSeed, seed_size);
        }
        else
        {
            ret = ERR_CMS_INVALID_LENGTH;
        }
    }
    else
    {
        ret = ERR_CMS_INVALID_LENGTH;
    }
    CSW_TRACE(_CSW | TLEVEL(CFW_CFG_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR(" CFW_GetDBSSeed ret =%x \n", 0x08100254)), ret);
    return ret;
}

#endif
//frank add end

UINT32 CFW_CfgNwSetDetectMBS(UINT8 nMode)
{
    UINT32 ret = ERR_SUCCESS;

    ret = NV_SetNwDetectMBS(nMode);
    return ret;
}

UINT32 CFW_CfgNwGetDetectMBS(UINT8 *pMode)
{
    UINT32 ret = ERR_SUCCESS;

    ret = NV_GetNwDetectMBS(pMode);
    return ret;
}
