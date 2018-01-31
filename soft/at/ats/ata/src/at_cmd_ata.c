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

#ifdef ATA_SUPPORT

#include "at.h"
#include "at_sa.h"
#include "sul.h"

#include "key_defs.h"
#include "hal_key.h"
#include "globalconstants.h"
#include "calib_m.h"
#include "csw_ver.h"
#include "stack_common.h"
#include "stack_config.h"
#ifndef ONLY_AT_SUPPORT
extern VOID ADP_SendEventKey(UINT8 botton,UINT8 type);
extern void FM_LCDShowColor( S32 type );
#endif
extern BOOL checkTCardExist(void);
extern BOOL checkTCard2Exist(void);
extern BOOL PM_GetBatteryInfo(UINT8 *pBcs, UINT8 *pBcl, UINT16 *pMpc);

VOID AT_ATA_Result_OK(UINT32 uReturnValue,
                      UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize, UINT8 nDLCI)
{
    PAT_CMD_RESULT pResult = NULL;

    pResult = AT_CreateRC(uReturnValue,
                          uResultCode,
                          CMD_ERROR_CODE_OK,
                          CMD_ERROR_CODE_TYPE_CME,
                          nDelayTime,
                          pBuffer,
                          nDataSize,
                          nDLCI);
    if(!pResult)
    {
        return;
    }
    AT_Notify2ATM(pResult, nDLCI);
    AT_FREE(pResult);
    pResult = NULL;
    return;
}

VOID AT_ATA_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI)
{
    PAT_CMD_RESULT pResult = NULL;

    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR, uErrorCode, nErrorType, 0, 0, 0, nDLCI);
    AT_Notify2ATM(pResult, nDLCI);
    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }
    return;
}

#ifdef __MMI_BT_SUPPORT__
VOID AT_BT_ATA_AsyncEventProcess(COS_EVENT *pEvent)
{
    UINT8 AckBuff[20] = {0,};
    switch (pEvent->nEventId)
    {
    case 0xF0000013:
        AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0,HIUINT16(pEvent->nParam3) );
        break;
    case 0xF0000015:
        AT_Sprintf(AckBuff, "FIND BT DEV,%d",pEvent->nParam1+1);
        AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, AckBuff, sizeof(AckBuff),HIUINT16(pEvent->nParam3));
        at_enter_bt_window();
        break;
    default:
        break;
    }
    return;
}
#endif

VOID AT_ATA_CmdFunc_KEYHOLD(AT_CMD_PARA *pParam)
{
    INT32 iResult                        = 0;
    UINT8 uParaCount                     = 0;
    UINT8 uIndex                         = 0;
    UINT16 uSize                         = 0;
    UINT16 param1  = 0;
    UINT8 *pRsp = NULL;

    iResult = AT_Util_GetParaCount(pParam->pPara, &uParaCount);
    if ((iResult != ERR_SUCCESS) || (uParaCount > 2))
    {
        AT_TC(g_sw_ATA, "KEYHOLD:get param count fail,iResult = %d.\n",iResult);
        AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }

    if(uParaCount == 1)
    {
        uIndex  = 0;
        uSize  = SIZEOF(param1);
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &param1, &uSize);
        if (iResult != ERR_SUCCESS )
        {
            AT_TC(g_sw_ATA, "KEYHOLD, get parameter fail:iResult = %d\n", iResult);
            return;
        }
#ifndef ONLY_AT_SUPPORT
        ADP_SendEventKey(param1,KEY_LONG_PRESS);
#endif
        AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pRsp, 0,pParam->nDLCI);
    }
    else
    {
        AT_TC(g_sw_ATA, "KEYHOLD, param count error,uParaCount = %d\n",uParaCount);
        AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
}


VOID AT_ATA_CmdFunc_KEY(AT_CMD_PARA *pParam)
{
    INT32 iResult = 0;
    UINT8 uParaCount = 0;
    UINT8 uIndex = 0;
    UINT16 uSize  = 0;
    UINT8 param1 = 0;
    UINT8 *pRsp = NULL;
    UINT8 arrCharacterSet[16] = {0, };
    UINT16 nParamLen  = 0;

    nParamLen           = sizeof(arrCharacterSet);
    iResult = AT_Util_GetParaCount(pParam->pPara, &uParaCount);
    if ((iResult != ERR_SUCCESS) || (uParaCount > 2))
    {
        AT_TC(g_sw_ATA, "KEY, parameter error\n");
        AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }

    if(uParaCount == 2)
    {
        uIndex  = 0;
        uSize  = sizeof(param1);
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &param1, &uSize);
        if (iResult != ERR_SUCCESS )
        {
            AT_TC(g_sw_ATA, "KEY, get parameter error:param1= %d\n", param1);
            AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }

        uIndex++;

        AT_MemZero(arrCharacterSet, sizeof(arrCharacterSet));
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, (PVOID)arrCharacterSet, &nParamLen);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_ATA, "KEY, get parameter error:iResult = %d,arrCharacterSet = %s\n", iResult, arrCharacterSet);
            AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }

        if((AT_StrCmp(arrCharacterSet, "press") == 0) || (AT_StrCmp(arrCharacterSet, "PRESS") == 0))
        {
#ifndef ONLY_AT_SUPPORT
            ADP_SendEventKey(param1,KEY_EVENT_DOWN);
#endif
        }
        else if  ((AT_StrCmp(arrCharacterSet, "release") == 0) || (AT_StrCmp(arrCharacterSet, "RELEASE") == 0))
        {
#ifndef ONLY_AT_SUPPORT
            ADP_SendEventKey(param1,KEY_EVENT_UP);
#endif
        }
        else if  ((AT_StrCmp(arrCharacterSet, "longpress") == 0) || (AT_StrCmp(arrCharacterSet, "LONGPRESS") == 0))
        {
#ifndef ONLY_AT_SUPPORT
            ADP_SendEventKey(param1,KEY_LONG_PRESS);
#endif
        }
        else
        {
            AT_TC(g_sw_ATA, "KEY, get parameter error:No match the presss, release...\n");
            AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pRsp, 0,pParam->nDLCI);
    }
    else
    {
        AT_TC(g_sw_ATA, "KEY, get parameter error:No match the presss, release...\n");
        AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
}


VOID AT_ATA_CmdFunc_TCARD(AT_CMD_PARA *pParam)
{
    INT32 iResult = 0;
    UINT8 uParaCount = 0;
    UINT8 uIndex = 0;
    UINT16 uSize  = 0;
    UINT8 uParam  = 0;
    UINT8 uTCount = 0;
    UINT8 *pRsp = NULL;
    UINT32 uDevCount = 0;
    FS_DEV_INFO *psDevInfo;
    UINT32 i;

    iResult = AT_Util_GetParaCount(pParam->pPara, &uParaCount);
    if ((iResult != ERR_SUCCESS) || (uParaCount != 1))
    {
        AT_TC(g_sw_ATA, "TCARD, parameter error\n");
        AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }

    if(uParaCount == 1)
    {
        uIndex  = 0;
        uSize  = sizeof(uParam);
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uParam, &uSize);
        if (iResult != ERR_SUCCESS )
        {
            AT_TC(g_sw_ATA, "TCARD, get parameter error:param1= %d\n", uParam);
            AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        iResult = FS_GetDeviceInfo(&uDevCount,&psDevInfo);
        if(iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_ATA, "TCARD, get device info error:result= %d\n", iResult);
            AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        for(i = 0; i < uDevCount; i++)
        {
            if(psDevInfo[i].dev_type == FS_DEV_TYPE_TFLASH)
            {
                uTCount ++;
            }
        }
        if((UINT8)uParam!= uTCount)
        {
            AT_TC(g_sw_ATA, "TCARD, T card count no mach. uParam = %d,uTCount = %d.", uParam,uTCount);
            AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
#ifdef DUAL_TFLASH_SUPPORT
        if((UINT8)2 != uParam)
        {
            AT_TC(g_sw_ATA, "TCARD, device count no mach. uTCount = %d,uParam = %d.", uTCount,uParam);
            AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        if(!checkTCard2Exist())
        {
            AT_TC(g_sw_ATA, "TCARD, not exist MMC1. uTCount = %d,uParam = %d.", uTCount,uParam);
            AT_ATA_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
#else
        if((UINT8)1 != uParam)
        {
            AT_TC(g_sw_ATA, "TCARD, not supprot 2 TCard uParam = %d.", uParam);
            AT_ATA_Result_Err(ERR_AT_CME_OPTION_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
#endif
#ifndef ONLY_AT_SUPPORT
        if(!checkTCardExist())
        {
            AT_TC(g_sw_ATA, "TCARD, not exist MMC0. uParam = %d,uTCount = %d.", uParam,uTCount);
            AT_ATA_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
#endif
        pRsp = AT_MALLOC(100);
        AT_StrCpy(pRsp,"+TCARD:");
        AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pRsp, 0,pParam->nDLCI);
        AT_FREE(pRsp);
    }
    else
    {
        AT_TC(g_sw_ATA, "TCARD, param count error,uParaCount = %d.\n",uParaCount);
        AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
}



VOID AT_ATA_CmdFunc_LcdBackLightOn(AT_CMD_PARA *pParam)
{
#ifndef ONLY_AT_SUPPORT
    FM_LCDShowColor(4);
    SetGPIOLevelReq(0/*GPIO_DEV_LED_MAINLCD*/,5);
#endif
    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI);
}


VOID AT_ATA_CmdFunc_LcdBackLightOff(AT_CMD_PARA *pParam)
{
#ifndef ONLY_AT_SUPPORT
    SetGPIOLevelReq(0/*GPIO_DEV_LED_MAINLCD*/,0);
#endif
    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI);
}


VOID AT_ATA_CmdFunc_LcdOn(AT_CMD_PARA *pParam)
{
    INT32 iResult = 0;
    UINT8 uParaCount = 0;
    UINT8 uIndex = 0;
    UINT16 uSize  = 0;
    UINT8 uParam  = 0;
    // UINT8 *pRsp = NULL;

    iResult = AT_Util_GetParaCount(pParam->pPara, &uParaCount);
    if ((iResult != ERR_SUCCESS) || (uParaCount != 1))
    {
        AT_TC(g_sw_ATA, "LcdOn, parameter error,iResult = %d,uParaCount = %d.\n",iResult,uParaCount);
        AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }

    if(uParaCount == 1)
    {
        uIndex  = 0;
        uSize  = sizeof(uParam);
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uParam, &uSize);
        if (iResult != ERR_SUCCESS )
        {
            AT_TC(g_sw_ATA, "LcdOn, get parameter error:param1= %d\n", uParam);
            AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
#ifndef ONLY_AT_SUPPORT
        FM_LCDShowColor((S32)uParam);
#endif
    }
    else
    {
#ifndef ONLY_AT_SUPPORT
        FM_LCDShowColor(1);
#endif
    }
    SetGPIOLevelReq(0/*GPIO_DEV_LED_MAINLCD*/,5);
    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI);
}


VOID AT_ATA_CmdFunc_LcdOff(AT_CMD_PARA *pParam)
{
#ifndef ONLY_AT_SUPPORT
    SetGPIOLevelReq(0/*GPIO_DEV_LED_MAINLCD*/,0);
#endif
    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI);
}


VOID AT_ATA_CmdFunc_KeyPadBackLightOn(AT_CMD_PARA *pParam)
{
#ifndef ONLY_AT_SUPPORT
    SetGPIOLevelReq(5/*GPIO_DEV_LED_KEY*/,5);
#endif
    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI);
}


VOID AT_ATA_CmdFunc_KeyPadBackLightOff(AT_CMD_PARA *pParam)
{
#ifndef ONLY_AT_SUPPORT
    SetGPIOLevelReq(5/*GPIO_DEV_LED_KEY*/,0);
#endif
    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI);
}


extern void mmi_camera_entry_app_screen(void);
extern U8  GpioPlayReq(U8 p_type,U8 p_level);
extern void FlashlightOpen(void);
extern void FlashlightClose(void);

#ifdef __MMI_FLASHLIGHT__
VOID AT_ATA_CmdFunc_FlashLightOn(AT_CMD_PARA *pParam)
{
    FlashlightOpen();
    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI);
}


VOID AT_ATA_CmdFunc_FlashLightOff(AT_CMD_PARA *pParam)
{
    FlashlightClose();
    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI);
}
#endif

#if 0
extern void mmi_bth_send_msg(U32 dest_module, U32 sap_id, U32 msg_id, void *p_local_para, void *p_peer_buff);
VOID AT_BT_CmdFunc_StartBT(AT_CMD_PARA* pParam)
{
    if (pParam->iType == AT_CMD_EXE)
    {

        mmi_bth_send_msg(MOD_MMI, BT_APP_SAP, /*MSG_ID_BT_SETADDR_REQ*/7203, NULL, NULL);
        COS_Sleep(500);
        mmi_bth_send_msg(MOD_BT, BT_APP_SAP, /*MSG_ID_BT_POWERON_REQ*/6701, NULL, NULL);
        AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 60, NULL, 0, pParam->nDLCI);
        return;
    }
    else
    {
        AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
}
#endif

VOID AT_ATA_CmdFunc_VibratorOn(AT_CMD_PARA *pParam)
{
    GpioPlayReq(7/*MMIAPI_DEV_TYPE_VIBRATOR*/, 5);
    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI);
}


VOID AT_ATA_CmdFunc_VibratorOff(AT_CMD_PARA *pParam)
{
    GpioPlayReq(7/*MMIAPI_DEV_TYPE_VIBRATOR*/, 0);
    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI);
}

extern bool FMR_IsActive(void);
extern void FMR_PowerOn();
extern UINT32 MCI_FmPlay(void);
extern void FMR_SetFreq( INT16 curf );
extern void FMR_SetOutputDevice( UINT8 device );
extern void GpioSpeakerModeStart();
extern void FMR_SetOutputVolume( UINT8 volume1, UINT8 volume2 );
extern UINT8 FMR_ValidStop(INT16 freq, INT8 signalvl, BOOL is_step_up);
/*
VOID AT_ATA_CmdFunc_StartFM(AT_CMD_PARA *pParam)
{
    INT32 iResult = 0;
    UINT8 uParaCount = 0;
    UINT8 uIndex = 0;
    UINT16 uSize  = 0;
    UINT16 param1 = 0;
    UINT8 freq_is_valid = 0;

    iResult = AT_Util_GetParaCount(pParam->pPara, &uParaCount);
    if ((iResult != ERR_SUCCESS) || (uParaCount > 1))
    {
        AT_TC(g_sw_ATA, "FM, parameter error\n");
        AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        hal_HstSendEvent(0x12345671);
        return;
    }

    if(uParaCount == 1)
    {

        if(!FMR_IsActive())
        {
            AT_TC(g_sw_ATA, "FM, is not active \n");
            AT_ATA_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            hal_HstSendEvent(0x12345672);
            return;
        }
        uIndex  = 0;
        uSize  = sizeof(param1);
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT16, &param1, &uSize);
        if (iResult != ERR_SUCCESS )
        {
            AT_TC(g_sw_ATA, "FM, get parameter error:param1= %d\n", param1);
            AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            hal_HstSendEvent(0x12345673);
            return;
        }
        FMR_PowerOn();
        MCI_FmPlay();
        FMR_SetFreq(param1);
        FMR_SetOutputDevice(6); //AUDIO_DEVICE_SPEAKER_BOTH);
        FMR_SetOutputDevice(4);//AUDIO_DEVICE_SPEAKER2);
        GpioSpeakerModeStart();
        FMR_SetOutputVolume(7, 7);
        freq_is_valid = FMR_ValidStop(param1,0,0);

        AT_TC(g_sw_ATA, "FM, FMR_GetSignalLevel= %d\n", freq_is_valid);
        if(freq_is_valid)
        {
            AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI);
            hal_HstSendEvent(0x12345674);
        }
        else
        {
            AT_ATA_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME,pParam->nDLCI);
            hal_HstSendEvent(0x12345675);
        }
    }
    else
    {
        AT_TC(g_sw_ATA, "FM, get parameter error:freq num is wrong!\n");
        AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        hal_HstSendEvent(0x12345676);
        return;
    }

}
*/

VOID AT_ATA_CmdFunc_StartFM(AT_CMD_PARA *pParam)
{
#ifdef FM_RADIO_ENABLE
    INT32 iResult = 0;
    UINT8 uParaCount = 0;
    UINT8 uIndex = 0;
    UINT16 uSize  = 0;
    UINT16 param1 = 0;
    UINT8 freq_is_valid = 0;

    iResult = AT_Util_GetParaCount(pParam->pPara, &uParaCount);
    if ((iResult != ERR_SUCCESS) || (uParaCount > 1))
    {
        AT_TC(g_sw_ATA, "FM, parameter error\n");
        AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        hal_HstSendEvent(0x12345671);
        return;
    }

    if(uParaCount == 1)
    {
#if 0
        if(!FMR_IsActive())
        {
            AT_TC(g_sw_ATA, "FM, is not active \n");
            AT_ATA_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            hal_HstSendEvent(0x12345672);
            return;
        }
#endif
        uIndex  = 0;
        uSize  = sizeof(param1);
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT16, &param1, &uSize);
        if (iResult != ERR_SUCCESS )
        {
            AT_TC(g_sw_ATA, "FM, get parameter error:param1= %d\n", param1);
            AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            hal_HstSendEvent(0x12345673);
            return;
        }
        FMR_PowerOn();
        MCI_FmPlay();
        FMR_SetOutputDevice(6);
        FMR_SetOutputDevice(4);
        GpioSpeakerModeStart();
        FMR_SetOutputVolume(7, 7);

        AT_TC(g_sw_ATA, "FM, FMR_ValidStop= %d", param1);

        do
        {
            freq_is_valid = FMR_ValidStop(param1++,0,0);
            if(1080 < param1) break;
        }
        while(!freq_is_valid);


        AT_TC(g_sw_ATA,"FM, FMR_ValidStop= %d", param1);
        AT_TC(g_sw_ATA, "FM, FMR_GetSignalLevel= %d", freq_is_valid);
        if(freq_is_valid)
        {
            FMR_SetFreq(param1);
            AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI);
            hal_HstSendEvent(0x12345674);
        }
        else
        {
            AT_ATA_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME,pParam->nDLCI);
            hal_HstSendEvent(0x12345675);
        }
    }
    else
    {
        AT_TC(g_sw_ATA, "FM, get parameter errorfreq num is wrong");
        AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        hal_HstSendEvent(0x12345676);
        return;
    }
#endif
}

extern void FMR_PowerOff();
VOID AT_ATA_CmdFunc_StopFM(AT_CMD_PARA *pParam)
{
#ifdef FM_RADIO_ENABLE
    FMR_PowerOff();
    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI);
#endif
}


#define MAX_SAMPLE_NUM  6
extern UINT16* sensorbuf;

static VOID bubble_sort(UINT8* punsorted)
{
    INT32  i,j;
    UINT8 temp;

    for (i = 0; i <MAX_SAMPLE_NUM; i++)
    {
        for (j = i; j < MAX_SAMPLE_NUM; j++)
        {
            if (punsorted[i] > punsorted[j])
            {
                temp = punsorted[i];
                punsorted[i] = punsorted[j];
                punsorted[j] = temp;
            }
        }
    }
}


static U8 get_sample(U8* psample)
{
    UINT8 i;
    UINT8 unsort_data[MAX_SAMPLE_NUM];
    UINT16 value = 0;
    for(i=0; i<MAX_SAMPLE_NUM; i++)
    {
        unsort_data[i]=psample[i];
        hal_HstSendEvent(unsort_data[i]);
    }

    bubble_sort(&unsort_data);

    for(i=0; i<MAX_SAMPLE_NUM-2; i++)
        value = value + unsort_data[i+1];
    value = value/(MAX_SAMPLE_NUM-2);

    i = (UINT8)value;
    hal_HstSendEvent(i);
    hal_HstSendEvent(0xffffffff);
    return i;

}
VOID AT_ATA_CmdFunc_CameraCapture(AT_CMD_PARA *pParam)
{
#ifdef __MMI_CAMERA__
    U8* pdata;
    U8 point_value[4];
    U8 sensor_data[MAX_SAMPLE_NUM];
    mmi_fm_camera_entry_preview_screen();
    COS_Sleep(2000);
    pdata=(U8*)sensorbuf;

    point_value[0]=get_sample(pdata);
    pdata = pdata+(128-1)*4 -10;
    point_value[1]=get_sample(pdata);
    pdata = pdata + 128*159*4;
    point_value[2]=get_sample(pdata);
    pdata = pdata + (128*160-1)*4 -10;
    point_value[3]=get_sample(pdata);

    if(sensorbuf)
    {

        if(((point_value[0]>0x50)&&(point_value[1]>0x70)&&(point_value[2]>0x80)&&(point_value[3]>0x80)))
            AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI);
        else
            AT_ATA_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME,pParam->nDLCI);

        mdi_camera_preview_stop();
        MCI_CamPowerDown();
    }
    else
    {
        hal_HstSendEvent(0x11223345);
        AT_ATA_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME,pParam->nDLCI);
    }
#endif
}


/*************************************************************
 *** Get software MMI version
 *************************************************************/
VOID AT_ATA_CmdFunc_GetSWVersion(AT_CMD_PARA *pParam)
{
    char *version = NULL;
    UINT8 *pRsp;

    version = (char *)csw_VerGetString(CSW_VER_MODULE_ID_MMI);
    pRsp = AT_MALLOC(100);

    if(strcmp(version, "CoolMMI (-1)") == 0)
    {
        AT_Sprintf(pRsp, "+SWVERSION:%s", (char *)tgt_GetBuildVerNo());
    }
    else
    {
        AT_Sprintf(pRsp, "+SWVERSION:%s", version);
    }

    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pRsp, AT_StrLen(pRsp), pParam->nDLCI);
    AT_FREE(pRsp);
}


/*************************************************************
//AT+CALIBFLAG :
//bit0 -> reserve, bit1 -> crystal, bit2-5 -> 850 - Pcs Afc
//bit6-9 -> 850 - Pcs Pa profile, bit10 - 13 -> 850 - Pcs Pa offset
//bit14 - 17 850 - Pcs ILoss, bit18 - bit30 -> reserve,
//bit31  -> calib ok
 *************************************************************/
VOID AT_ATA_CmdFunc_GetCalibFlag(AT_CMD_PARA *pParam)
{

    char *version = NULL;
    CALIB_CALIBRATION_T *calibPtr = calib_GetPointers();
    UINT8 *pRsp = NULL;

    pRsp = AT_MALLOC(100);
    if(calibPtr->hstSw->param[14] == 0xffffffff)
        AT_Sprintf(pRsp, "+CALIBFLAG:%04x", 0);
    else
        AT_Sprintf(pRsp, "+CALIBFLAG:%04x", calibPtr->hstSw->param[14]);
    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pRsp, AT_StrLen(pRsp) ,pParam->nDLCI);
    AT_FREE(pRsp);
}

#define KEY_COUNT  128
UINT8 key_collector[KEY_COUNT];
extern void COS_SetAtaKeypadStart(UINT8* key_coll);
extern void COS_SetAtaKeypadEnd(UINT32* pcoll_count);
VOID AT_ATA_CmdFunc_Keypad(AT_CMD_PARA *pParam)
{
    INT32 iResult = 0;
    UINT8 uParaCount = 0;
    UINT8 uIndex = 0;
    UINT16 uSize  = 0;
    UINT8 uParam  = 0;
    UINT8 *pRsp = NULL;
    UINT8 *p = NULL;
    //UINT8 KeyMap[] = {0x19,0x12,0x00,0x15,0x14,0x3b,0x09,0x0e,0x08,0x11,0x07,0x13,0x35};
    //UINT32 uKeyCount;
    UINT32 uCollKeyCount = 0;
    UINT32 i;
    UINT8 szHex[16];

    pRsp = AT_MALLOC(200);
    AT_MemSet(pRsp,0,200);
    iResult = AT_Util_GetParaCount(pParam->pPara, &uParaCount);
    if ((iResult != ERR_SUCCESS) || (uParaCount != 1))
    {
        AT_TC(g_sw_ATA, "Keypad, parameter error,iResult = %d,uParaCount = %d.\n",iResult,uParaCount);
        AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }

    if(uParaCount == 1)
    {
        uIndex  = 0;
        uSize  = sizeof(uParam);
        iResult = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &uParam, &uSize);
        if (iResult != ERR_SUCCESS )
        {
            AT_TC(g_sw_ATA, "Keypad, get parameter error:param1= %d\n", uParam);
            AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            AT_FREE(pRsp);
            return;
        }
        if(uParam == 1)
        {
            COS_SetAtaKeypadStart(key_collector);
            AT_StrCpy(pRsp,"+KEYPAD:");
        }
        else
        {
            COS_SetAtaKeypadEnd(&uCollKeyCount);
            //uKeyCount = sizeof(KeyMap);
            AT_StrCpy(pRsp,"+KEYPAD:{");
            for(i = 0; i < uCollKeyCount; i++)
            {
                p = &key_collector[i];
                AT_MemSet(szHex, 0, 16);
                AT_Sprintf(szHex, "0x%02x,", *p);
                AT_StrCat(pRsp, szHex);
            }
            if(p)
            {
                p = pRsp + AT_StrLen(pRsp) - 1;
                *p = 0;
            }
            AT_StrCat(pRsp, "}");
        }
        AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pRsp, AT_StrLen(pRsp), pParam->nDLCI);
    }
    else
    {
        AT_TC(g_sw_ATA, "Keypad, parameter error,uParaCount = %d.\n",uParaCount);
        AT_ATA_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
    }
    AT_FREE(pRsp);
    return;
}


VOID AT_ATA_CmdFunc_Voltag(AT_CMD_PARA *pParam)
{
    UINT8 Bcs = 0;
    UINT8 Bcl = 0;
    UINT16 Mpc = 0;
    UINT8 *pRsp;
    BOOL result;

    pRsp = AT_MALLOC(100);
    //pmd_EnableCharger(FALSE);
    result = PM_GetBatteryInfo(&Bcs,&Bcl,&Mpc);
    if(result)
    {
        AT_MemSet(pRsp,0,100);
        AT_Sprintf(pRsp, "+VOLTAG:%d",Mpc );
        AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pRsp, AT_StrLen(pRsp), pParam->nDLCI);
    }
    else
    {
        AT_TC(g_sw_ATA, "Voltag, PM_GetBatteryInfo return FALSE.");
        AT_ATA_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
    }
    //pmd_EnableCharger(TRUE);
    AT_FREE(pRsp);
}


VOID AT_ATA_CmdFunc_ChargeOn(AT_CMD_PARA *pParam)
{
    UINT8 *pRsp;

    pRsp = AT_MALLOC(100);
    pmd_EnableCharger(TRUE);
    AT_MemSet(pRsp,0,100);
    AT_StrCpy(pRsp, "+CHARGEON:" );
    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pRsp, AT_StrLen(pRsp), pParam->nDLCI);
    AT_FREE(pRsp);
}


VOID AT_ATA_CmdFunc_ChargeOff(AT_CMD_PARA *pParam)
{
    UINT8 *pRsp;

    pRsp = AT_MALLOC(100);
    pmd_EnableCharger(FALSE);
    AT_MemSet(pRsp,0,100);
    AT_StrCpy(pRsp, "+CHARGEOFF:" );
    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pRsp, AT_StrLen(pRsp), pParam->nDLCI);
    AT_FREE(pRsp);
}


VOID AT_ATA_CmdFunc_Start(AT_CMD_PARA *pParam)
{
    UINT8 *pRsp;

    pRsp = AT_MALLOC(100);
    // pmd_EnableCharger(FALSE);
    EntryNewScreen(11003, NULL, NULL, NULL);
    TurnOnBacklight(0);
    AT_MemSet(pRsp,0,100);
    AT_StrCpy(pRsp, "+START:" );
    AT_ATA_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, pRsp, AT_StrLen(pRsp), pParam->nDLCI);
    AT_FREE(pRsp);
}
#ifdef __MMI_BT_SUPPORT__
extern void at_enter_bt_window(void);
extern void at_search_bt_window(void);
extern void at_pair_bt_dev(S32 index);
extern void set_unique_flag(BOOL flag);

VOID AT_ATA_CmdFunc_BTST(AT_CMD_PARA* pParam)
{
    if (pParam->iType == AT_CMD_EXE)
    {
        at_enter_bt_window();
        set_unique_flag(FALSE);
        AT_TC(g_sw_ATA, "SEND MSG_ID_BT_POWERON_REQ MSG\n");
        AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 40, NULL, 0, pParam->nDLCI);
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
}
VOID AT_ATA_CmdFunc_BTIN(AT_CMD_PARA* pParam)
{
    if (pParam->iType == AT_CMD_EXE)
    {
        at_search_bt_window();
        AT_TC(g_sw_ATA, "SEND MSG_ID_BT_POWERON_REQ MSG\n");
        AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 40, NULL, 0, pParam->nDLCI);
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
}

VOID AT_ATA_CmdFunc_PAIR(AT_CMD_PARA* pParam)
{
    UINT16 uDevCount = 0;
    UINT8 uLen = 0;
    if (pParam->iType == AT_CMD_SET)
    {
        uLen = SIZEOF(uDevCount);
        AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT16, &uDevCount, &uLen);
        at_pair_bt_dev(uDevCount);
        AT_TC(g_sw_ATA, "Enter at_pair_bt_dev\n");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 40, NULL, 0, pParam->nDLCI);
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
}

#if 0
VOID AT_ATA_CmdFunc_BTSP(AT_CMD_PARA* pParam)
{
    if (pParam->iType == AT_CMD_EXE)
    {
        //g_BTAppflag = TRUE;
        //g_BtPowerOffflag = TRUE;
        mmi_bth_send_msg(MOD_BT, BT_APP_SAP, /*MSG_ID_BT_POWEROFF_REQ*/6705, NULL, NULL);
        AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 40, NULL, 0, pParam->nDLCI);
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
}
VOID AT_ATA_CmdFunc_INQ(AT_CMD_PARA* pParam)
{
    UINT8 uParaCount = 0;
    UINT8 uDevCount = 0;
    UINT16 uTimeOut = 0;
    UINT8 uLen = 0;
    UINT32 eParamOk = 0;

    if (pParam->iType == AT_CMD_SET)
    {
        /* GET param count */
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount)))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }

        /* check para count */
        if (uParaCount != 2)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME,pParam->nDLCI);
            return;
        }



        uLen = SIZEOF(uDevCount);
        eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uDevCount, &uLen);
        if (eParamOk != ERR_SUCCESS)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }

        uLen = SIZEOF(uTimeOut);
        eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT16, &uTimeOut, &uLen);
        if (eParamOk != ERR_SUCCESS)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        // g_BTAppflag = TRUE;
        mmi_bth_cm_discovery_req_hdler(uDevCount, uTimeOut, /*MMI_BT_OBJECT_TRANSFER_MAJOR_SERVICE_MASK*/0x100000, TRUE);
        AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
}



VOID AT_ATA_CmdFunc_LINK(AT_CMD_PARA* pParam)
{
    UINT8 uParaCount = 0;
    UINT8 uValue[3] = {0};
    UINT8 uAddr[20] = {0};
    UINT8 uAddrtemp[20] = {0};
    UINT8 uLen = 0;
    UINT32 eParamOk = 0;
    UINT8 *str;

    if (pParam->iType == AT_CMD_SET)
    {
        /* GET param count */
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount)))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        /* check para count */
        if (uParaCount != 1)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME,pParam->nDLCI);
            return;
        }
        uLen = SIZEOF(uAddr);
        eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, uAddr, &uLen);
        if (eParamOk != ERR_SUCCESS)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        if (AT_StrLen(uAddr)>0)
        {
            if(AT_StrLen(uAddr) != 17)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }
            uAddrtemp[0] = uAddr[0];
            uAddrtemp[1] = uAddr[1];
            uAddrtemp[2] = uAddr[3];
            uAddrtemp[3] = uAddr[4];
            uAddrtemp[4] = uAddr[6];
            uAddrtemp[5] = uAddr[7];
            uAddrtemp[6] = uAddr[9];
            uAddrtemp[7] = uAddr[10];
            uAddrtemp[8] = uAddr[12];
            uAddrtemp[9] = uAddr[13];
            uAddrtemp[10] = uAddr[15];
            uAddrtemp[11] = uAddr[16];

            for(UINT8 i=0; i<12; i++)
            {
                if((uAddrtemp[i]>='0' && uAddrtemp[i]<='9') || (uAddrtemp[i]>='a' && uAddrtemp[i]<='f')
                        || (uAddrtemp[i]>='A' && uAddrtemp[i]<='F'))
                    continue;
                else
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    return;
                }
            }


        }
        else if (0 == AT_StrLen(uAddr))
        {
            //AT_MemCpy(&g_LinkAddr,&g_BdAddr,6);
        }
        //g_BtRole = 1;
        //g_BTAppflag = TRUE;
        mmi_bth_send_msg(MOD_MMI, BT_APP_SAP, /*MSG_ID_BT_CONNECT_REQ*/7200, NULL, NULL);
        AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 40, NULL, 0, pParam->nDLCI);
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
}
#endif

typedef struct
{
    UINT8 bytes[6];
} t_bdaddr;
extern t_bdaddr g_PairAddr;

VOID AT_ATA_CmdFunc_BTSH(AT_CMD_PARA* pParam)
{
    UINT8 uParaCount = 0;
    UINT8 uValue[3] = {0};
    UINT16 uAddr[20] = {0};
    UINT8 uAddrtemp[20] = {0};
    UINT32 uTimeOut = 0;
    UINT16 uLen = 0;
    UINT32 eParamOk = 0;
    UINT8 *str;
    UINT8 i;
    if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount)))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }

        uLen = SIZEOF(uAddr);
        eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, &uAddr, &uLen);
        if (eParamOk != ERR_SUCCESS)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        if (AT_StrLen(uAddr)>0)
        {
            if(AT_StrLen(uAddr) != 17)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }
            uAddrtemp[0] = uAddr[0];
            uAddrtemp[1] = uAddr[0]>>8;
            uAddrtemp[2] = uAddr[1]>>8;
            uAddrtemp[3] = uAddr[2];
            uAddrtemp[4] = uAddr[3];
            uAddrtemp[5] = uAddr[3]>>8;
            uAddrtemp[6] = uAddr[4]>>8;
            uAddrtemp[7] = uAddr[5];
            uAddrtemp[8] = uAddr[6];
            uAddrtemp[9] = uAddr[6]>>8;
            uAddrtemp[10] = uAddr[7]>>8;
            uAddrtemp[11] = uAddr[8];
            /*
            for(UINT8 i=0;i<12;i++)
            {
                if((uAddrtemp[i]>='0' && uAddrtemp[i]<='9') || (uAddrtemp[i]>='a' && uAddrtemp[i]<='f')
                     || (uAddrtemp[i]>='A' && uAddrtemp[i]<='F'))
                    continue;
                else
                {
                hal_HstSendEvent(0x0323dddd);
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    return;
                }
            } */

            uValue[0] = uAddrtemp[0];
            uValue[1] = uAddrtemp[1];
            g_PairAddr.bytes[5] = strtol(uValue, (char**)&str, 16);
            uValue[0] = uAddrtemp[2];
            uValue[1] = uAddrtemp[3];
            g_PairAddr.bytes[4] = strtol(uValue, (char**)&str, 16);
            uValue[0] = uAddrtemp[4];
            uValue[1] = uAddrtemp[5];
            g_PairAddr.bytes[3] = strtol(uValue, (char**)&str, 16);
            uValue[0] = uAddrtemp[6];
            uValue[1] = uAddrtemp[7];
            g_PairAddr.bytes[2] = strtol(uValue, (char**)&str, 16);
            uValue[0] = uAddrtemp[8];
            uValue[1] = uAddrtemp[9];
            g_PairAddr.bytes[1] = strtol(uValue, (char**)&str, 16);
            uValue[0] = uAddrtemp[10];
            uValue[1] = uAddrtemp[11];
            g_PairAddr.bytes[0] = strtol(uValue, (char**)&str, 16);

        }
        set_unique_flag(TRUE);
        at_search_bt_window();
        AT_TC(g_sw_ATA, "SEND MSG_ID_BT_POWERON_REQ MSG\n");
        AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 40, NULL, 0, pParam->nDLCI);
        return;
    }
    else if (pParam->iType == AT_CMD_EXE)
    {
        set_unique_flag(FALSE);
        at_search_bt_window();
        AT_TC(g_sw_ATA, "at_search_bt_window\n");
        AT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 40, NULL, 0, pParam->nDLCI);
        return;
    }
    else
    {
        hal_HstSendEvent(0x0323eeee);
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
}
#endif

VOID AT_AUDIO_CmdFunc_MIC(AT_CMD_PARA *pParam)
{
}
VOID AT_AUDIO_CmdFunc_SPEAKER(AT_CMD_PARA *pParam)
{
}

VOID AT_AUDIO_CmdFunc_RECEIVER(AT_CMD_PARA *pParam)
{
}

VOID AT_AUDIO_CmdFunc_HEADSET(AT_CMD_PARA *pParam)
{
}







#endif

