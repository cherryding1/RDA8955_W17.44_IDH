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


#include <cfw.h>
#include "cfw_cfg.h"
#include "cfw_prv.h"
/*==========================================================================
Select the current phonebook memory storage.

KEY: "PH_BK"
==========================================================================*/
#if 0
UINT16 g_Pbk_nStorage1 = 0;
UINT16 g_Pbk_nStorage2 = 0;
UINT16 g_Pbk_nStorage3 = 0;
#endif

UINT8 g_PbkStorage[CFW_SIM_COUNT] = { 2,};


UINT32 CFW_CfgSetPbkStorage (UINT8 nStorage, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgSetPbkStorage, nStorage=0x%x, nSimID=0x%x\n",0x08100255), nStorage, nSimID);
    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CheckSimId, error ret =0x%x\n",0x08100256), ret);
        return ret;
    }
    ret = CFW_CfgNumRangeCheck(nStorage, PBK_STORAGE_RANGE);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgNumRangeCheck, error ret =0x%x\n",0x08100257), ret);
        ret = ERR_CFG_PARAM_OUT_RANGR;
    }
    else
    {
        if((nSimID >= CFW_SIM_0) && (nSimID < CFW_SIM_COUNT))
        {

            g_PbkStorage[nSimID] = nStorage;

        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
        }

    }
    return ret;

}


/*==========================================================================
Get the current phonebook memory storage.

KEY: "PH_BK"
==========================================================================*/
UINT32 CFW_CfgGetPbkStorage (UINT8 *nStorage, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetPbkStorage,  nSimID=0x%x\n",0x08100258), nSimID);

    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CheckSimId, error ret =0x%x\n",0x08100259), ret);
        return ret;
    }
    if((nSimID >= CFW_SIM_0) && (nSimID < CFW_SIM_COUNT))
    {

        *nStorage = g_PbkStorage[nSimID];

    }
    else
    {
        ret = ERR_CFW_INVALID_PARAMETER;
    }

    return ret;
}


/*==========================================================================
typedef struct _CFW_PBK_STRORAGE_INFO {
 UINT16 index;
 UINT8 iNumberLen;
 UINT8 txtLen;
} CFW_PBK_STRORAGE_INFO;
==========================================================================*/
UINT32 SRVAPI CFW_CfgSetPbkStrorageInfo (
    CFW_PBK_STRORAGE_INFO *pStorageInfo
)
{
#if 0
    UINT32 *pInteger = 0;
    pInteger = (UINT32 *)pStorageInfo;

#ifndef MF32_BASE_REG_DISABLE
    WRITE_NUMBER_SETTING("BK_INFO", *pInteger, "pbk");
#endif

    return ERR_SUCCESS;
#endif
    return ERR_CFW_NOT_SUPPORT;
}


UINT32 SRVAPI CFW_CfgGetPbkStrorageInfo (
    CFW_PBK_STRORAGE_INFO *pStorageInfo
)
{
#if 0
    INT32 integer = 0;

#ifndef MF32_BASE_REG_DISABLE
    READ_NUMBER_SETTING("BK_INFO", integer, "pbk");
#endif

    SUL_MemCopy32((void *)pStorageInfo, &integer, 4);

    return ERR_SUCCESS;
#endif
    return ERR_CFW_NOT_SUPPORT;

}

//PRIVATE CFW_PBK_CAPACITY_INFO g_PbkCapInfo[2] = {{0,}, };

UINT32 CFW_CfgSetPbkCapacityInfo (
    UINT8 nStorageId,
    CFW_PBK_CAPACITY_INFO *pStorageInfo
)
{
#if 0
    UINT8 n = 0;

    if(nStorageId == CFW_PBK_ME)
        n = 0;
    else  if(nStorageId == CFW_PBK_SIM)
        n = 1;
    else
        return ERR_INVALID_PARAMETER;

    SUL_MemCopy32(&g_PbkCapInfo[n], pStorageInfo, SIZEOF(CFW_PBK_CAPACITY_INFO));

    return ERR_SUCCESS;
#endif
    return ERR_CFW_NOT_SUPPORT;

}

//PRIVATE CFW_PBK_CAPACITY_INFO g_CalllogCapInfo[7] = {{0, },};

UINT32 CFW_CfgSetCalllogCapacityInfo (
    UINT8 nStorageId,
    CFW_PBK_CAPACITY_INFO *pStorageInfo
)
{
#if 0
    UINT8 n = 0;

    if(nStorageId == CFW_PBK_ME_MISSED)
        n = 0;
    else  if(nStorageId == CFW_PBK_ME_RECEIVED)
        n = 1;
    else  if(nStorageId == CFW_PBK_SIM_FIX_DIALLING)
        n = 2;
    else  if(nStorageId == CFW_PBK_SIM_LAST_DIALLING)
        n = 3;
    else  if(nStorageId == CFW_PBK_ME_LAST_DIALLING)
        n = 4;
    else  if(nStorageId == CFW_PBK_ON)
        n = 5;
    else  if(nStorageId == CFW_PBK_EN)
        n = 6;
    else
        return ERR_INVALID_PARAMETER;

    SUL_MemCopy32(&g_PbkCapInfo[n], pStorageInfo, SIZEOF(CFW_PBK_CAPACITY_INFO));

    return ERR_SUCCESS;
#endif
    return ERR_CFW_NOT_SUPPORT;

}

