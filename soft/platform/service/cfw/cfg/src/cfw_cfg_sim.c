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
#include "cfw_cfg.h"
#include "drv.h"
#include "dm.h"

#ifdef SIM_SWITCH_ENABLE
#include "drv.h"
#include "dm.h"
#endif

#include "csw_debug.h"

//
// Volatile global variables in RAM.
// You don't need to save to flash.
//
PRIVATE SIM_SMS_PARAM  g_SimSmsParam[CFW_SIM_COUNT];
PRIVATE SIM_CHV_PARAM  g_SimChvParam[CFW_SIM_COUNT];
PRIVATE SIM_PBK_PARAM  g_SimPbkParam[CFW_SIM_COUNT];
PRIVATE SIM_MISC_PARAM g_SimMiscParam[CFW_SIM_COUNT];
PRIVATE SIM_SAT_PARAM  g_SimSatParam[CFW_SIM_COUNT];
UINT32 CFW_CfgSimParamsInit()
{
    SUL_ZeroMemory8(g_SimSmsParam,  CFW_SIM_COUNT * SIZEOF(SIM_SMS_PARAM));
    SUL_ZeroMemory8(g_SimChvParam,  CFW_SIM_COUNT * SIZEOF(SIM_CHV_PARAM));
    SUL_ZeroMemory8(g_SimPbkParam,  CFW_SIM_COUNT * SIZEOF(SIM_PBK_PARAM));
    SUL_ZeroMemory8(g_SimMiscParam, CFW_SIM_COUNT * SIZEOF(SIM_MISC_PARAM));
    SUL_ZeroMemory8(g_SimSatParam,  CFW_SIM_COUNT * SIZEOF(SIM_SAT_PARAM));

    return ERR_SUCCESS;
}

UINT32 CFW_CfgSimGetSmsParam(SIM_SMS_PARAM **pSimSmsParam, CFW_SIM_ID nSimID)
{
    UINT32 nRet = ERR_SUCCESS;

    nRet = CFW_CheckSimId(nSimID);
    if(ERR_SUCCESS == nRet)
    {
        *pSimSmsParam = &(g_SimSmsParam[nSimID]);
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSimGetSmsParam, error ret =0x%x\n",0x0810025a), nRet);
    }

    return nRet;
}

UINT32 CFW_CfgSimGetChvParam(SIM_CHV_PARAM **pSimChvParam,
                             CFW_SIM_ID nSimID)
{
    UINT32 nRet = ERR_SUCCESS;

    //nRet = CFW_CheckSimId(nSimID);
    if(ERR_SUCCESS == nRet)
    {
        *pSimChvParam = &(g_SimChvParam[nSimID]);
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSimGetChvParam, error ret =0x%x\n",0x0810025b), nRet);
    }
    return nRet;
}

UINT32 CFW_CfgSimGetPbkParam(SIM_PBK_PARAM **pSimPbkParam,
                             CFW_SIM_ID nSimID)
{
    UINT32 nRet = ERR_SUCCESS;

    nRet = CFW_CheckSimId(nSimID);
    if(ERR_SUCCESS == nRet)
    {
        *pSimPbkParam = &(g_SimPbkParam[nSimID]);
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSimGetPbkParam, error ret =0x%x\n",0x0810025c), nRet);
    }
    return nRet;
}


UINT32 CFW_CfgSimGetMicParam(SIM_MISC_PARAM **pSimMicParam,
                             CFW_SIM_ID nSimID)
{
    *pSimMicParam = &(g_SimMiscParam[nSimID]);
    return ERR_SUCCESS;
}


UINT32 CFW_CfgSimGetSatParam(SIM_SAT_PARAM **pSimSatParam,
                             CFW_SIM_ID nSimID)
{
    UINT32 nRet = ERR_SUCCESS;

    nRet = CFW_CheckSimId(nSimID);
    if(ERR_SUCCESS == nRet)
    {
        *pSimSatParam = &(g_SimSatParam[nSimID]);
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSimGetSatParam, error ret =0x%x\n",0x0810025d), nRet);
    }
    return nRet;
}


#ifdef SIM_SWITCH_ENABLE
//==============================================================================
//Sim Switch
//==============================================================================
UINT32 SRVAPI CFW_CfgSetSimSwitch(UINT8 n)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetSimSwitch);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetSimSwitch,  n=0x%x\n",0x0810025e), n);
    ret = CFW_CfgNumRangeCheck(n, SIMCARD_SWITCH_RABGE);
    if (ERR_SUCCESS == ret)
    {
        ret = NV_SetSimCardSwitch(n);
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSimSwitch);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetSimSwitch,  ret=0x%x\n",0x0810025f), ret);
    return ret;
}

UINT32 SRVAPI CFW_CfgGetSimSwitch(UINT8 *n)
{
    UINT32 ret    = ERR_SUCCESS;
    BOOL bDefault = FALSE;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGetSimSwitch);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgGetSimSwitch enter\n",0x08100260));
    ret = NV_GetSimCardSwitch(n);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("NV_GetSimCardSwitch,  ret=0x%x\n",0x08100262), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetSimSwitch);
        return ret;
    }
    else
    {
        ret = CFW_CfgNumRangeCheck(*n, SIMCARD_SWITCH_RABGE);
        if (ERR_SUCCESS != ret)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgNumRangeCheck,  ret=0x%x\n",0x08100263), ret);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetSimSwitch);
            return ret;
        }
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetSimSwitch);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_readdefault,  *n=0x%x\n",0x08100265), *n);
    return ERR_SUCCESS;
}

#if 0
#define SIM_1 1 //simcard 1
#define SIM_2 2 //simcard 2
VOID SimSwitch(UINT8 SimNum)
{
    DRV_GpioSetMode(DM_DEVID_GPIO_7, 1);
    DRV_GpioSetDirection(DM_DEVID_GPIO_7, DRV_GPIO_OUT);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SimSwitch,  SimNum=0x%x\n",0x08100266), SimNum);
    {
        DRV_GpioSetLevel(DM_DEVID_GPIO_7, DRV_GPIO_LOW);
    }
    if (SIM_2 == SimNum)
    {
        DRV_GpioSetLevel(DM_DEVID_GPIO_7, DRV_GPIO_HIGH);
    }
}
#endif
#endif



