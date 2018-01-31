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
#include "cfw_cfg.h"
#include "csw_debug.h"
#include <string.h>
/*===========================================================================
Configure Supplementary service notifications.

bCSSI: 0 ~ 1  b0
bCSSU: 0 ~ 1  b1

KEY: "PUBLIC"
===========================================================================*/
UINT32 CFW_CfgSetSSN(UINT8 bCSSI, UINT8 bCSSU, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    UINT32 nPubVal = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetSSN);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("CFW_CfgSetSSN, bCSSI = 0x%x, bCSSU =0x%x,  nSimID =0x%x\n",0x08100135), bCSSI, bCSSU, nSimID);
    ret = NV_GetCmPublic(&nPubVal);
    if(ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetSSN);
        return ERR_CFG_READ_REG_FAILURE;
    }
    if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
    {
        if((( bCSSI == 0) || (bCSSI == 1)) &&
                (( bCSSU == 0) || (bCSSU == 1)))
        {
            SET_BIT_VALUE(nPubVal, (nSimID * 2), bCSSI);
            SET_BIT_VALUE(nPubVal, (nSimID * 2 + 1), bCSSU);

        }
        else if((bCSSI == 0xFF) && (( bCSSU == 0) || (bCSSU == 1)))
        {
            SET_BIT_VALUE(nPubVal, (nSimID * 2 + 1), bCSSU);

        }
        else if((bCSSU == 0xFF) && (( bCSSI == 0) || (bCSSI == 1)))
        {
            SET_BIT_VALUE(nPubVal, (nSimID * 2), bCSSI);

        }
        else if((bCSSU == 0xFF) && (bCSSI == 0xFF))
        {
             // do nothing.
        }
        else
        {
            ret = ERR_CFG_PARAM_OUT_RANGR;
        }
    }
    else
    {
        ret = ERR_CFW_INVALID_PARAMETER;
    }
    if(ret == ERR_SUCCESS)
    {
        ret = NV_SetCmPublic(nPubVal);
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetSSN);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgSetSSN exit",0x08100138));
    return ret;
}

/*===========================================================================
Get the SSN value.

bCSSI: 0 ~ 1  b0
bCSSU: 0 ~ 1  b1

KEY: "PUBLIC"
===========================================================================*/
UINT32 CFW_CfgGetSSN (UINT8 *bCSSI, UINT8 *bCSSU, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    UINT32 nPubVal = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGetSSN);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetSSN  nSimID =0x%x\n",0x08100139), nSimID);
    ret = NV_GetCmPublic(&nPubVal);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_readdefault, Error ret =0x%x\n",0x0810013b), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetSSN);
        return ERR_CFG_READ_REG_FAILURE;
    }
    if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
    {
        GET_BIT_VALUE(nPubVal, (nSimID * 2), *bCSSI);
        GET_BIT_VALUE(nPubVal, (nSimID * 2 + 1), *bCSSU);
    }
    else
    {
        ret = ERR_CFW_INVALID_PARAMETER;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetSSN);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgGetSSN  *bCSSI = 0x%x, *bCSSU =0x%x\n",0x0810013c), *bCSSI, *bCSSU);
    return ERR_SUCCESS;
}

UINT32 CFW_CfgSetCallWaiting (UINT8 n, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetCallWaiting);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgSetCallWaiting  n = 0x%x, nSimID =0x%x\n",0x0810013d), n, nSimID);
    ret = CFW_CfgNumRangeCheck(n, CM_CCWA_RANGE);
    if(ERR_SUCCESS == ret)
    {
        if( (CFW_SIM_0 <= nSimID) && (CFW_SIM_COUNT > nSimID))
        {
            ret = NV_SetCmCcwaSim(nSimID,n);
        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
        }
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetCallWaiting);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgSetCallWaiting exit",0x0810013e));
    return ret;
}


UINT32 CFW_CfgGetCallWaiting (UINT8 *n, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGetCallWaiting);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetCallWaiting  nSimID =0x%x\n",0x0810013f), nSimID);
    if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
    {
        ret = NV_GetCmCcwaSim(nSimID,n);
        if(ERR_SUCCESS != ret)
        {
                CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_readdefault, error ret = 0x%x\n",0x08100141), ret);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetCallWaiting);
                return ERR_CFG_READ_REG_FAILURE;
        }
    }
    else
    {
        ret = ERR_CFW_INVALID_PARAMETER;
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetCallWaiting);
        return ret;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetCallWaiting);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetCallWaiting  *n =0x%x\n",0x08100144), *n);
    return ERR_SUCCESS;
}

UINT32 SRVAPI CFW_CfgSetClir ( UINT8 nClir,
                               CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetClir);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgSetClir  nClir = 0x%x, nSimID =0x%x\n",0x08100145), nClir, nSimID);
    ret = CFW_CfgNumRangeCheck(nClir, CM_CLIR_RANGE);
    if(ERR_SUCCESS == ret)
    {
        if( (CFW_SIM_0 <= nSimID) && (CFW_SIM_COUNT > nSimID))
        {
            ret = NV_SetCmClirSim(nSimID,nClir);
        }
        else
        {
            ret = ERR_CFW_INVALID_PARAMETER;
        }
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetClir);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgSetClir exit",0x08100146));
    return ret;
}

UINT32 SRVAPI CFW_CfgGetClir ( UINT8 *n,
                               CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGetClir);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetClir  nSimID =0x%x\n",0x08100147), nSimID);
    if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
    {
        ret = NV_GetCmClirSim(nSimID,n);
        if(ERR_SUCCESS != ret)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("cfg_readdefault, error ret = 0x%x\n",0x08100149), ret);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetClir);
            return ERR_CFG_READ_REG_FAILURE;
        }
    }
    else
    {
        ret = ERR_CFW_INVALID_PARAMETER;
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetClir);
        return ret;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetClir);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetClir  *n =0x%x\n",0x0810014c), *n);
    return ERR_SUCCESS;

}

/*==========================================================================
NAME        FUNCTION                PARAMETER   RANGE   DEFAULT
AT+CLIP     CFW_CfgSetClip          N           0-1     1


KEY: "PUBLIC"
==========================================================================*/
UINT32 CFW_CfgSetClip ( UINT8 nEnable, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetClip);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgSetClip  nEnable = 0x%x, nSimID =0x%x\n",0x0810014d), nEnable, nSimID);
    ret = CFW_CfgNumRangeCheck(nEnable, CM_CLIP_RANGE);
    if(ret == ERR_SUCCESS)
    {
        if(CFW_SIM_0 == nSimID)
            MemoryInfo.Clip_Value0 = nEnable;
        else if(CFW_SIM_1 == nSimID)
            MemoryInfo.Clip_Value1 = nEnable;
        else if((CFW_SIM_1 + 1) == nSimID)
            MemoryInfo.Clip_Value2 = nEnable;
        else if((CFW_SIM_1 + 2) == nSimID)
            MemoryInfo.Clip_Value3 = nEnable;
        else
            ret = ERR_CFW_INVALID_PARAMETER;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetClip);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgSetClip exit",0x0810014e));
    return ret;
}


UINT32 CFW_CfgGetClip (UINT8 *pEnable, CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGetClip);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetClip  nSimID =0x%x\n",0x0810014f), nSimID);
    if(CFW_SIM_0 == nSimID)
        *pEnable = MemoryInfo.Clip_Value0;
    else if(CFW_SIM_1 == nSimID)
        *pEnable = MemoryInfo.Clip_Value1;
    else if((CFW_SIM_1 + 1) == nSimID)
        *pEnable = MemoryInfo.Clip_Value2;
    else if((CFW_SIM_1 + 2) == nSimID)
        *pEnable = MemoryInfo.Clip_Value3;
    else
        ret = ERR_CFW_INVALID_PARAMETER;
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetClip);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetClip *pEnable = 0x%x",0x08100150), *pEnable);
    return ret;
}

/*=========================================================================
These functions enable or disable a calling subscriber to get the connected
line identity (COL) of the called party after setting up a mobile originated
call. See 3GPP TS 22.081 [3] for more

=========================================================================*/
UINT32 CFW_CfgSetColp ( UINT8 n,
                        CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgSetColp);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_CfgSetColp  n = 0x%x, nSimID =0x%x\n",0x08100151), n, nSimID);
    ret = CFW_CfgNumRangeCheck(n, CM_COLP_RANGE);
    if(ERR_SUCCESS == ret)
    {
        if( (CFW_SIM_0 <= nSimID) && (CFW_SIM_COUNT > nSimID))
        {
            ret = NV_SetCmColpSim(nSimID,n);
        }
        else
            ret = ERR_CFW_INVALID_PARAMETER;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgSetColp);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgSetColp exit",0x08100152));
    return ret;
}


UINT32 CFW_CfgGetColp ( UINT8 *n,
                        CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgGetColp);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetColp  nSimID =0x%x\n",0x08100153), nSimID);
    if((CFW_SIM_0 <= nSimID) && (nSimID < CFW_SIM_COUNT))
    {
        ret = NV_GetCmColpSim(nSimID,n);
        if(ERR_SUCCESS != ret)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("NV_GetCmColpSim, error ret = 0x%x\n",0x08100155), ret);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetCallWaiting);
            return ERR_CFG_READ_REG_FAILURE;
        }
    }
    else
    {
        ret = ERR_CFW_INVALID_PARAMETER;
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetCallWaiting);
        return ret;
    }

    ret = CFW_CfgNumRangeCheck(*n, CM_CCWA_RANGE);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgNumRangeCheck, error ret = 0x%x\n",0x08100156), ret);
        CSW_ASSERT(0);
    }

    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgGetColp);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetColp  *n =0x%x\n",0x08100158), *n);
    return ERR_SUCCESS;
}

/*===========================================================================
nToneDuration
[in] Specify the tone duration(1-255) in 100ms(for example, the value 4 means
400 milliseconds). The default is zero the tone duration is 300ms.
===========================================================================*/
UINT32 CFW_CfgSetToneDuration ( UINT8  nToneDuration )
{
#if 0
    UINT32 ret = ERR_SUCCESS;
    WRITE_NUMBER_SETTING_8("TONE_DURA", nToneDuration, "cm" );
    return ret;
#endif
    return ERR_CFW_NOT_SUPPORT;
}

UINT32 CFW_CfgGetToneDuration ( UINT8 *pToneDuration )
{
#if 0
    UINT32 ret = ERR_SUCCESS;
    READ_NUMBER_SETTING_8("TONE_DURA", *pToneDuration, "cm" );
    return ret;
#endif
    return ERR_CFW_NOT_SUPPORT;

}


/*==============================================================================
Set the call forwarding number and conditions.

KEY: "CFD_NUMBER_1" "CFD_NUMBER_2" "CFD_NUMBER_3"
     "CFD_NUMBER_4" "CFD_NUMBER_5" "CFD_NUMBER_6"
     "CFD_OTHERS" "CFD_STATUS"
===============================================================================*/
UINT32 CFW_CfgSetCallForwarding ( CFW_CFG_CALLFORWARDING *pCallForwarding)
{
#if 0
    UINT32 ret = ERR_SUCCESS;
#ifndef MF32_BASE_REG_DISABLE
    UINT32 *pInt = 0;
    UINT32 integer = 0;

    UINT8 reason = 0;
    UINT8 mode = 0;
    UINT8 class = 0;
    UINT8 time = 0;

    pInt = (UINT32 *)(&(pCallForwarding->number));

    /*Call fording number*/
    WRITE_NUMBER_SETTING("CFD_NUMBER_1", *pInt, "cm");
    pInt++;

    WRITE_NUMBER_SETTING("CFD_NUMBER_2", *pInt, "cm");
    pInt++;

    WRITE_NUMBER_SETTING("CFD_NUMBER_3", *pInt, "cm");
    pInt++;

    WRITE_NUMBER_SETTING("CFD_NUMBER_4", *pInt, "cm");
    pInt++;

    WRITE_NUMBER_SETTING("CFD_NUMBER_5", *pInt, "cm");
    pInt++;

    WRITE_NUMBER_SETTING("CFD_NUMBER_6", *pInt, "cm");

    /*Call fording other parameters*/
    reason = pCallForwarding->reason;
    mode = pCallForwarding->mode;
    class = pCallForwarding->class;
    time = pCallForwarding->time;

    if(reason >= 0)/* etc...*/
    {
        integer = (reason << 24) | (mode << 16) | (class << 8) | (time) ;
        WRITE_NUMBER_SETTING("CFD_OTHERS", integer, "cm");
        WRITE_NUMBER_SETTING("CFD_STATUS", pCallForwarding->status, "cm");
    }
    else
    {
        ret = ERR_CFG_PARAM_OUT_RANGR;
    }

#endif

    return ret;
#endif
    return ERR_CFW_NOT_SUPPORT;
}

/*================================================================================
Get the call forwarding number and conditions.

KEY: "CFD_NUMBER_1" "CFD_NUMBER_2" "CFD_NUMBER_3"
     "CFD_NUMBER_4" "CFD_NUMBER_5" "CFD_NUMBER_6"
     "CFD_OTHERS" "CFD_STATUS"
================================================================================*/
UINT32 CFW_CfgGetCallForwarding (CFW_CFG_CALLFORWARDING *pCallForwardingArray,
                                 UINT8 *pCount
                                )  /*Need to verify the parameters...*/
{
#if 0
    UINT32 ret = ERR_SUCCESS;
    UINT32 integer = 0;

    UINT8 reason = 0;
    UINT8 mode = 0;
    UINT8 class = 0;
    UINT8 time = 0;

#ifndef MF32_BASE_REG_DISABLE
    READ_NUMBER_SETTING("CFD_NUMBER_1", integer, "cm");
    SUL_MemCopy32(&(pCallForwardingArray->number.nTelNumber[0]), &integer, 4);

    READ_NUMBER_SETTING("CFD_NUMBER_2", integer, "cm");
    SUL_MemCopy32(&(pCallForwardingArray->number.nTelNumber[4]), &integer, 4);

    READ_NUMBER_SETTING("CFD_NUMBER_3", integer, "cm");
    SUL_MemCopy32(&(pCallForwardingArray->number.nTelNumber[8]), &integer, 4);

    READ_NUMBER_SETTING("CFD_NUMBER_4", integer, "cm");
    SUL_MemCopy32(&(pCallForwardingArray->number.nTelNumber[12]), &integer, 4);

    READ_NUMBER_SETTING("CFD_NUMBER_5", integer, "cm");
    SUL_MemCopy32(&(pCallForwardingArray->number.nTelNumber[16]), &integer, 4);

    READ_NUMBER_SETTING("CFD_NUMBER_6", integer, "cm");
    pCallForwardingArray->number.nTelNumber[20] = (UINT8)((integer & 0xff000000) >> 24);
    pCallForwardingArray->number.nTelNumber[21] = (UINT8)((integer & 0x00ff0000) >> 16);
    pCallForwardingArray->number.nTelNumber[22] = (UINT8)((integer & 0x0000ff00) >> 8);
    pCallForwardingArray->number.nType =  (UINT8)((integer & 0x000000ff));


    READ_NUMBER_SETTING("CFD_OTHERS", integer, "cm");

    reason = (UINT8)((integer & 0xFF000000) >> 24);
    mode = (UINT8)((integer & 0xFF0000) >> 16);
    class = (UINT8)((integer & 0xFF00) >> 8);
    time = (UINT8)(integer & 0xFF);

    pCallForwardingArray->reason = reason;
    pCallForwardingArray->mode = mode;
    pCallForwardingArray->class = class;
    pCallForwardingArray->time = time;

    READ_NUMBER_SETTING("CFD_STATUS", integer, "cm");
    pCallForwardingArray->status = (UINT8)integer;

    /*Need someone to fill out...*/
    if( pCallForwardingArray->number.nTelNumber[0] == 0)
        pCount = pCount;

#endif
    return ret;
#endif
    return ERR_CFW_NOT_SUPPORT;

}

/*==========================================================================
NAME    FUNCTION                          PARAMETER   RANGE  DEFAULT
AT+CRC  CFW_CfgSetIncomingCallResultMode  nMode       0-1    0

==========================================================================*/
UINT32 CFW_CfgSetIncomingCallResultMode(UINT8 nMode)
{
#if 0
    UINT32 ret = ERR_SUCCESS;

#ifndef MF32_BASE_REG_DISABLE
    /*Check the range*/
    ret = CFW_CfgNumRangeCheck(nMode, AT_CRC_RANGE);
    if(ret != ERR_SUCCESS)
        return ret;

    MemoryInfo.IncomingCallResultMode = nMode;
#endif

    return ret;
    return ERR_CFW_NOT_SUPPORT;
#endif

    //UINT32 ret = CFW_CfgNumRangeCheck(nMode, AT_CRC_RANGE);
    //if(ret != ERR_SUCCESS)
    //    return ret;
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetIncomingCallResultMode  nMode =0x%x\n",0x08100159), nMode);
    if(nMode > 1)
        return ERR_CFG_PARAM_OUT_RANGR;

    MemoryInfo.IncomingCallResultMode = nMode;
    return ERR_SUCCESS;
}

UINT32 CFW_CfgGetIncomingCallResultMode(UINT8 *nMode)
{
#if 0
    return ERR_CFW_NOT_SUPPORT;
#endif
    *nMode = MemoryInfo.IncomingCallResultMode;

    return ERR_SUCCESS;
}


/*==========================================================================
NAME        FUNCTION                PARAMETER   RANGE                   DEFAULT
AT+CBST     CFW_CfgSetBearServType  nSpeed      0,4,6,7,14,68,70,71,75  7

==========================================================================*/
UINT32 CFW_CfgSetBearServType(UINT8 nSpeed)
{
#if 0
    UINT32 ret = ERR_SUCCESS;

    /*Check the range*/
    ret = CFW_CfgNumRangeCheck(nSpeed, AT_BST_RANGE);
    if(ret != ERR_SUCCESS)
        return ret;

    MemoryInfo.BearServType = nSpeed;

    return ret;
#endif
    return ERR_CFW_NOT_SUPPORT;
}

UINT32 CFW_CfgGetBearServType(UINT8 *nSpeed)
{
#if 0
    *nSpeed = MemoryInfo.BearServType;
    return ERR_SUCCESS;
#endif
    return ERR_CFW_NOT_SUPPORT;

}

/*==========================================================================
NAME    FUNCTION                            PARAMETER   RANGE    DEFAULT
ATX     CFW_CfgSetConnectResultFormatCode   nValue      0-4      4

==========================================================================*/
UINT32 CFW_CfgSetConnectResultFormatCode(UINT8 nValue)
{
#if 1
    //UINT32 ret = ERR_SUCCESS;

    /*Check the range*/
    //ret = CFW_CfgNumRangeCheck(nValue, AT_X_RANGE);
    //if(ret != ERR_SUCCESS)
    //    return ret;
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetConnectResultFormatCode  nValue =0x%x\n",0x0810015a), nValue);
    if(nValue > 4)
        return ERR_CFG_PARAM_OUT_RANGR;

    MemoryInfo.ConnectResultFormatCode = nValue;

    return ERR_SUCCESS;
#endif
    //return ERR_CFW_NOT_SUPPORT;
}

UINT32 CFW_CfgGetConnectResultFormatCode(UINT8 *nValue)
{
#if 1
    //UINT32 ret = ERR_SUCCESS;
    *nValue = MemoryInfo.ConnectResultFormatCode;
    return ERR_SUCCESS;
#endif
    //return ERR_CFW_NOT_SUPPORT;
}


