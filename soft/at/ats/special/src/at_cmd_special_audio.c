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




#include "aud_m.h"
#include "at.h"
#include <drv_prv.h>
#include "at_module.h"
#include "at_cmd_special_audio.h"
#include "at_cfg.h"
#include "dm_audio.h"
#include "mci.h"
#include "aud_snd_types.h"
/**************************************************************************************************
 *                                          CONSTANTS
 **************************************************************************************************/
// AT_20071113_CAOW_B
const UINT32 AT_Sound1_mid[] =
{
    /*#include "chd10_mid.ring.txt"*/
};

const UINT32 AT_Sound2_mid[] =
{
    /*#include "chd11_mid.ring.txt"*/
};

const UINT32 AT_Sound3_mid[] =
{
    /*#include "s1imy05_imy.ring.txt"*/
};
const UINT32 AT_Sound3_amr[] =
{
 //   #include "at_sound3_amr.ring.txt"
};
// AT_20071113_CAOW_E

/**************************************************************************************************
 *                                           MACROS
 **************************************************************************************************/
// dm_tone do not support bluetooth_nr and headphone
// supportted by at
#define DM_AUDIO_MODE_BLUETOOTH_NR 4
#define DM_AUDIO_MODE_HEADPHONE    5

#ifdef CHIP_HAS_AP
#define DM_AUDIO_MODE_BLUETOOTH_NREC 3
#define DM_AUDIO_MODE_BLUETOOTH_NO_NREC 4
#endif


UINT32  g_PlayBuffSetup  = 0;
UINT8 g_nDLCI;
UINT8 g_nSim;
/**************************************************************************************************
 *                                          TYPEDEFS
 **************************************************************************************************/

/**************************************************************************************************
 *                                       GLOBAL VARIABLES
 **************************************************************************************************/
extern BOOL hal_HstSendEvent(UINT32 ch);
extern BOOL DM_ResumMic(VOID);
extern BOOL DM_MuteMic(VOID);
extern AUD_LEVEL_T audio_cfg;
extern BOOL Speeching;
BOOL g_bTesting = FALSE;  // added by yangtt at 2008-05-05 for bug 8274
static CSW_SYS_FREQ_T def_sys_freq = CSW_SYS_FREQ_52M;

VOID AT_AUDIO_StartRecord();
VOID AT_AUDIO_StartRecordFile();
VOID AT_AUDIO_StopRecord();

struct
{
    UINT8 nSpeakerGain;
    UINT8 nMicGain;
    UINT8 nSideGain;
} g_stGain =
{
    0, 0, 0
};

VOID AT_Result_OK(UINT32 uReturnValue,
                  UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize, UINT8 nDLCI, UINT8 nSim)
{

    PAT_CMD_RESULT pResult = NULL;

    AT_BACKSIMID(nSim);
    // 填充结果码
    pResult = AT_CreateRC(uReturnValue,
                          uResultCode, CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, nDelayTime, pBuffer, nDataSize, nDLCI);

    AT_Notify2ATM(pResult, nDLCI);

    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }

    return;
}

VOID AT_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI, UINT8 nSim)
{
    PAT_CMD_RESULT pResult = NULL;

    AT_BACKSIMID(nSim);
    // 填充结果码
    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR, uErrorCode, nErrorType, 0, 0, 0, nDLCI);

    AT_Notify2ATM(pResult, nDLCI);

    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }

    return;
}

VOID AT_AUDIO_CmdFunc_VGR(AT_CMD_PARA *pParam)
{
    INT32 iRetValue;
    UINT8 uParaCount;
    UINT8 uData;
    UINT16 uStrLen;
    BOOL iRet;
    UINT8 uOutputStr[20] = { 0 };

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    // Check [in] Param

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET:

            if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                if (1 != uParaCount)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                uStrLen = 1;

                iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uData, &uStrLen);

                if (iRetValue != ERR_SUCCESS)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                if (uData > 8)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

#ifdef CHIP_HAS_AP
                if(FALSE == Speeching)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
#endif


                switch (uData)
                {

#if !defined(VOLUME_WITH_15_LEVEL) && !defined(VOLUME_WITH_7_LEVEL)

                case 5:
                    iRet = DM_SetAudioVolume(DM_AUDIO_SPK_3dB);
                    break;

                case 6:
                    iRet = DM_SetAudioVolume(DM_AUDIO_SPK_6dB);
                    break;

                case 7:
                    iRet = DM_SetAudioVolume(DM_AUDIO_SPK_15dB);
                    break;

                case 8:
                    iRet = DM_SetAudioVolume(DM_AUDIO_SPK_MUTE);
                    break;
#else
		   case 5:
                    iRet = DM_SetAudioVolume(DM_AUDIO_SPK_VOL_3);
                    break;

                case 6:
                    iRet = DM_SetAudioVolume(DM_AUDIO_SPK_VOL_6);
                    break;

                case 7:
                    iRet = DM_SetAudioVolume(DM_AUDIO_SPK_VOL_9);
                    break;

                case 8:
                    iRet = DM_SetAudioVolume(DM_AUDIO_SPK_VOL_15);
                    break;	
#endif
                default:
                    iRet = FALSE;
                    break;

                }

                if (!iRet)
                {
                    AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                }
                else
                {
                    gATCurrentVGR_Volume = uData;
                    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                }

                return;
            }

            break;

        case AT_CMD_TEST:
            SUL_ZeroMemory8(uOutputStr, 20);

            SUL_StrCat(uOutputStr, "+VGR: (5-8)");
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        case AT_CMD_READ:
            SUL_ZeroMemory8(uOutputStr, 20);

            SUL_StrPrint(uOutputStr, "+VGR: %d", gATCurrentVGR_Volume);
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        default:
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }
    }

    return;
}

VOID AT_AUDIO_CmdFunc_VGT(AT_CMD_PARA *pParam)
{
    INT32 iRetValue;
    UINT8 uParaCount;
    UINT8 uData;
    UINT16 uStrLen;
    BOOL iRet;
    UINT8 uOutputStr[20] = { 0 };

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    // Check [in] Param

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET:

            if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                if (1 != uParaCount)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                uStrLen = 1;

                iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uData, &uStrLen);

                if (iRetValue != ERR_SUCCESS)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                if (uData > 16)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

#ifdef CHIP_HAS_AP
                if(FALSE == Speeching)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
#endif

                switch (uData)
                {
                // Remove spk gain maybe some problem by lixp 20120817
                /*case 0:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_0dB);
                          break;

                        case 1:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_3dB);
                          break;

                        case 2:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_6dB);
                          break;

                        case 3:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_9dB);
                          break;

                        case 4:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_12dB);
                          break;

                        case 5:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_15dB);
                          break;

                        case 6:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_18dB);
                          break;

                        case 7:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_21dB);
                          break;

                        case 8:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_24dB);
                          break;

                        case 9:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_27dB);
                          break;

                        case 10:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_30dB);
                          break;

                        case 11:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_33dB);
                          break;

                        case 12:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_36dB);
                          break;

                        case 13:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_39dB);
                          break;

                        case 14:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_42dB);
                          break;

                        case 15:
                          iRet = DM_SetMicGain(DM_AUDIO_MIC_45dB);
                          break;*/

                case 16:
                    iRet = DM_SetMicGain(DM_AUDIO_MIC_MUTE);
                    break;

                default:
                    iRet = FALSE;
                    break;
                }

                if (!iRet)
                {
                    AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                }
                else
                {
                    gATCurrentVGT_Volume = uData;
                    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                }

                return;
            }
            break;

        case AT_CMD_TEST:
            SUL_ZeroMemory8(uOutputStr, 20);
            SUL_StrCat(uOutputStr, "+VGT: 16");

            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        case AT_CMD_READ:
            SUL_ZeroMemory8(uOutputStr, 20);

            SUL_StrPrint(uOutputStr, "+VGT: %d", gATCurrentVGT_Volume);

            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        default:
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;

        }
    }

    return;
}

VOID AT_AUDIO_CmdFunc_CMUT(AT_CMD_PARA *pParam)
{
    INT32 iRetValue;
    UINT8 uParaCount;
    UINT8 uData;
    UINT16 uStrLen;
    BOOL iRet            = FALSE;
    UINT8 uOutputStr[20] = { 0 };

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    // Check [in] Param

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET:
            if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
#ifdef CHIP_HAS_AP
            else if(FALSE == Speeching)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
#endif
            else
            {
                if (1 != uParaCount)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                uStrLen = 1;

                iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uData, &uStrLen);

                if (iRetValue != ERR_SUCCESS)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                if (uData > 1)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                if (0 == uData)
                {
                    // AT_20071122_CAOW_B  for bug#7091
                    if (gATCurrentCMUT_mute != 0)
                    {
                        // AT_20071122_CAOW_E
                        iRet = DM_ResumMic();

                        if (!iRet)
                        {
                            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }

                        gATCurrentCMUT_mute = 0;
                    }

                }
                else if (1 == uData)
                {
                    // AT_20071122_CAOW_B for bug#7091
                    if (gATCurrentCMUT_mute != 1)
                    {
                        // AT_20071122_CAOW_E
                        iRet = DM_MuteMic();

                        if (!iRet)
                        {
                            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                            return;
                        }

                        gATCurrentCMUT_mute = 1;
                    }

                }

                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                return;
            }

            break;

        case AT_CMD_TEST:
            SUL_ZeroMemory8(uOutputStr, 20);

            SUL_StrCat(uOutputStr, "+CMUT: (0,1)");

            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        case AT_CMD_READ:
            SUL_ZeroMemory8(uOutputStr, 20);

            SUL_StrPrint(uOutputStr, "+CMUT: %d", gATCurrentCMUT_mute);

            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        default:
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;

        }
    }

    return;
}

VOID AT_AUDIO_CmdFunc_CLVL(AT_CMD_PARA *pParam)
{
    INT32 iRetValue;
    UINT8 uParaCount;
    UINT8 uData;
    UINT8 uSoundType;// add for android SetCallVolume(HRilClient client, SoundType type, int vol_level),value is (0-3),
    UINT16 uStrLen;
    BOOL iRet;
    UINT8 uOutputStr[20] = { 0 };
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    // Check [in] Param

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET:

            if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                if ((uParaCount > 2) || (uParaCount < 1))
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                uStrLen = 1;

                iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uData, &uStrLen);

                if (iRetValue != ERR_SUCCESS)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                if((uData <5)||(uData >8))
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &uSoundType, &uStrLen);
#ifdef CHIP_HAS_AP
                //Remove for changing speaker Gain parameter to index value
                /*
                        if(uData <= 0)
                            uData = 0;
                        else if(uData >= 100)
                            uData = 100;
                        else
                            uData = (uData - 1)*7/100+1;
                */
                iRet = DM_SetAudioVolume(uData);
#else
#if !defined(VOLUME_WITH_15_LEVEL) && !defined(VOLUME_WITH_7_LEVEL)
                switch (uData)
                {
                // Remove spk gain maybe some problem by lixp 20120817
                /* case 0:
                          iRet = DM_SetAudioVolume(DM_AUDIO_SPK_m15dB);
                          break;

                        case 1:
                          iRet = DM_SetAudioVolume(DM_AUDIO_SPK_m12dB);
                          break;

                        case 2:
                          iRet = DM_SetAudioVolume(DM_AUDIO_SPK_m9dB);
                          break;

                        case 3:
                          iRet = DM_SetAudioVolume(DM_AUDIO_SPK_m6dB);
                          break;

                        case 4:
                          iRet = DM_SetAudioVolume(DM_AUDIO_SPK_m3dB);
                          break;*/

                case 5:
                    iRet = DM_SetAudioVolume(DM_AUDIO_SPK_0dB);
                    break;

                case 6:
                    iRet = DM_SetAudioVolume(DM_AUDIO_SPK_3dB);
                    break;

                case 7:
                    iRet = DM_SetAudioVolume(DM_AUDIO_SPK_6dB);
                    break;

                case 8:
                    iRet = DM_SetAudioVolume(DM_AUDIO_SPK_MUTE);
                    break;

                default:
                    iRet = FALSE;
                    break;

                }
#else
		   iRet = DM_SetAudioVolume(uData);
#endif
#endif
                if (!iRet)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                }
                else
                {
                    gATCurrentVGR_Volume = uData;
                    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                }

                return;
            }

            break;

        case AT_CMD_TEST:
            SUL_ZeroMemory8(uOutputStr, 20);

            SUL_StrCat(uOutputStr, "+CLVL: (5-8)");

            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        case AT_CMD_READ:
            AT_TC(g_sw_SPEC, "uOutputStr adress ======= %x", uOutputStr);
            SUL_ZeroMemory8(uOutputStr, 20);

            SUL_StrPrint(uOutputStr, "+CLVL: %d", gATCurrentVGR_Volume);

            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            AT_TC(g_sw_SPEC, "uOutputStr adress ======= %x", uOutputStr);
            break;

        default:
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;

        }
    }

    return;
}

// AT_20071113_CAOW_B
VOID AT_AUDIO_CmdFunc_CRMP(AT_CMD_PARA *pParam)
{
    UINT8 arrRes[40];
    UINT8 iRet       = 0;
    UINT8 uParaCount = 0;
    UINT32 eParamOk  = 0;
    UINT8 uCallType  = 0;
    UINT8 uVolume    = 5;
    UINT8 uType      = 0;
    UINT8 uIndex     = 0;
    UINT16 uStrLen    = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_MemZero(arrRes, sizeof(arrRes));
        AT_StrCpy(arrRes, "+CRMP: 0, (0-11), 0, 0");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        /* check para count */
        if ((1 > uParaCount) || (4 < uParaCount))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (uParaCount == 3)  // if param count equal to 3,then the forth param must be filled
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        /* get all param */
        uStrLen = 1;

        eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uCallType, &uStrLen);

        if (eParamOk != ERR_SUCCESS)  /* get uCallType failed */
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        /* get uCallType successful */
        if (uCallType > 3)  /* veritify uCallType */
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (uParaCount > 1)
        {
            uStrLen  = 1;
            eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &uVolume, &uStrLen);

            if (eParamOk != ERR_SUCCESS)  /* get uVolume failed */
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            if (uVolume > 11)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            if (uParaCount > 2)
            {
                uStrLen  = 1;
                eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT8, &uType, &uStrLen);

                if (eParamOk != ERR_SUCCESS)  /* get uType failed */
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                if (uType != 0)
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                if (uParaCount > 3)
                {
                    uStrLen  = 1;
                    eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_UINT8, &uIndex, &uStrLen);

                    if (eParamOk != ERR_SUCCESS)  /* get uIndex failed */
                    {
                        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }

                    if (uIndex > 11)
                    {
                        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }
                }
            }
        }

        AT_TC(g_sw_SPEC, "uParaCount = %d,uCallType = %d,uVolume = %d,uType = %d,uIndex = %d,", uParaCount, uCallType,
              uVolume, uType, uIndex);

#ifdef CHIP_HAS_AP

        if(FALSE == Speeching)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
#endif

        if (uParaCount == 4 && uIndex == 0)
        {
            if (DM_Audio_StopStream())
            {
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            }
            else
            {
                AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
        }
        else
        {
            if (!DM_Audio_StopStream())
            {
                AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }

            if (!DM_SetAudioVolume(uVolume))
            {
                AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }

            if (uCallType == 0)
            {
                switch (uIndex)
                {

                case 1:
                    iRet = DM_Audio_StartStream((UINT32 *)AT_Sound1_mid, sizeof(AT_Sound1_mid) / 4, DM_DICTAPHONE_MR122, 1);
                    break;
                case 2:
                    iRet = DM_Audio_StartStream((UINT32 *)AT_Sound2_mid, sizeof(AT_Sound2_mid) / 4, DM_DICTAPHONE_MR122, 1);
                    break;

                case 3:
                    iRet = DM_Audio_StartStream((UINT32 *)AT_Sound3_mid, sizeof(AT_Sound3_mid) / 4, DM_DICTAPHONE_MR122, 1);
                    break;

                case 4:
                    iRet = DM_Audio_StartStream((UINT32 *)AT_Sound2_mid, sizeof(AT_Sound2_mid) / 4, DM_DICTAPHONE_MR122, 1);
                    break;

                default:
                    iRet = DM_Audio_StartStream((UINT32 *)AT_Sound1_mid, sizeof(AT_Sound1_mid) / 4, DM_DICTAPHONE_MR122, 1);
                    break;
                }
            }
            else if (uCallType == 1)
            {
                /* switch()
                   {
                   case:
                   break;
                   default:
                   break;
                   } */
                // DM_Audio_StartStream(UINT32 * buffer, UINT32 len, DM_PlayRingType codec, BOOL loop_mode);
            }
            else if (uCallType == 2)
            {
                /* switch()
                   {
                   case:
                   break;
                   default:
                   break;
                   } */
                // DM_Audio_StartStream(UINT32 * buffer, UINT32 len, DM_PlayRingType codec, BOOL loop_mode);
            }
            else if (uCallType == 3)
            {
                switch (uIndex)
                {

                case 1:
                    iRet = DM_Audio_StartStream((UINT32 *)AT_Sound1_mid, sizeof(AT_Sound1_mid) / 4, DM_DICTAPHONE_MR122, 0);
                    break;

                case 2:
                    iRet = DM_Audio_StartStream((UINT32 *)AT_Sound2_mid, sizeof(AT_Sound2_mid) / 4, DM_DICTAPHONE_MR122, 0);
                    break;

                case 3:
                    iRet = DM_Audio_StartStream((UINT32 *)AT_Sound3_mid, sizeof(AT_Sound3_mid) / 4, DM_DICTAPHONE_MR122, 0);
                    break;

                case 4:
                    iRet = DM_Audio_StartStream((UINT32 *)AT_Sound2_mid, sizeof(AT_Sound2_mid) / 4, DM_DICTAPHONE_MR122, 0);
                    break;

                default:
                    iRet = DM_Audio_StartStream((UINT32 *)AT_Sound1_mid, sizeof(AT_Sound1_mid) / 4, DM_DICTAPHONE_MR122, 0);
                    break;
                }
            }

            if (iRet)
            {
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            }
            else
            {
                AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
        }

    }
    else
    {
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

}


extern BOOL Speeching;

// yangtt at 2008-04-25 for bug 8171
BOOL AT_GC_GetAudioTest(VOID)
{
    if (g_bTesting == TRUE)
        return FALSE;
    else
        return TRUE;

}

// ^aust: select and execute audio cycle test mode :0--aux,1--mic,2--loud
VOID AT_AUDIO_CmdFunc_AUST(AT_CMD_PARA *pParam)
{

    UINT8   arrRes[20];
    INT32   iRet = ERR_SUCCESS;
    UINT8   paraCount = 0;
    UINT8     nTestMode = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_MemZero(arrRes, sizeof(arrRes));
        AT_StrCpy(arrRes, "+AUST: (0-2)"); // yangtt at 2008-04-28 for bug 8224
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes),pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        //if testing audio cycle, operation is not allowed.
        if (g_bTesting)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        //if cc is on, operation is not allowed.
        CFW_CC_CURRENT_CALL_INFO arrCurrentCall[7];

        UINT8 nCount = 0;
        UINT32 nResult =    CFW_CcGetCurrentCall(arrCurrentCall, &nCount,nSim);

        if (nResult == ERR_SUCCESS)
        {

            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (nCount != 0)
        {
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);

        if (iRet != ERR_SUCCESS || paraCount != 1)
        {

            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        UINT16 nParamLen = 1;

        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8,
                                       &nTestMode , &nParamLen);

        if ((iRet != ERR_SUCCESS) || (nTestMode > 2))
        {

            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        gATCurrentnAudioTestMode = nTestMode;

        DM_SetAudioMode(gATCurrentnAudioTestMode);  // add  by yangtt
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_EXE)
    {
        //if testing audio cycle, operation is not allowed.
        if (g_bTesting)
        {

            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        //if cc is on, operation is not allowed.
        CFW_CC_CURRENT_CALL_INFO arrCurrentCall[7];

        UINT8 nCount = 0;
        UINT32 nResult =    CFW_CcGetCurrentCall(arrCurrentCall, &nCount,nSim);
        if (nResult == ERR_SUCCESS)
        {

            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (nCount != 0)
        {
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        Speeching = TRUE;  // add by ytt

        //store current gains
        //g_stGain.nSpeakerGain = audio_cfg.spkGain;
        //g_stGain.nMicGain = audio_cfg.micGain;
        //g_stGain.nSideGain = audio_cfg.sideGain;

        AT_TC(g_sw_SPEC, "^AUST :gATCurrentnAudioTestMode--%d\n", gATCurrentnAudioTestMode);

        if (gATCurrentnAudioTestMode == AT_AUDIO_TEST_EARPIECE)
        {
            CFW_EmodAudioTestStart(AT_AUDIO_TEST_EARPIECE_CFW);
        }
        else if (gATCurrentnAudioTestMode == AT_AUDIO_TEST_HANDSET)
        {
            CFW_EmodAudioTestStart(AT_AUDIO_TEST_HANDSET_CFW);
        }
        else if (gATCurrentnAudioTestMode == AT_AUDIO_TEST_LOUDSPEAKER)
        {
            CFW_EmodAudioTestStart(AT_AUDIO_TEST_LOUDSPEAKER_CFW);
        }
        else
        {

            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        //set sidegain as -6db
        //DM_SetSideGain(DM_AUDIO_SIDE_m6dB);
        //change by wulc because #define DM_SetSideGain DM_SetAudioVolume  in dm_tone.c
        DM_SetAudioVolume(DM_AUDIO_SIDE_m6dB);

        g_bTesting = TRUE;

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    return;

}

// ^auend: end audio cycle test
VOID AT_AUDIO_CmdFunc_AUEND(AT_CMD_PARA *pParam)
{

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (pParam->iType == AT_CMD_EXE)
    {
        if (g_bTesting)
        {
            AT_TC(g_sw_SPEC, "^AUEND :gATCurrentnAudioTestMode--%d\n", gATCurrentnAudioTestMode);
            CFW_EmodAudioTestEnd();
            g_bTesting = FALSE;
            Speeching = FALSE;      // add by ytt
            //restore to previous testmode and gains
            gATCurrentnAudioTestMode = AT_AUDIO_TEST_EARPIECE;
            DM_SetAudioMode(gATCurrentnAudioTestMode);// add by yangtt for bug 8029 at 04-18

            //set speaker gain

            //if (!DM_SetSpeakerGain(g_stGain.nSpeakerGain))// remove by wulc
            if (!DM_SetAudioVolume(g_stGain.nSpeakerGain))
            {
                COS_Sleep(10);

                //if (!DM_SetSpeakerGain(g_stGain.nSpeakerGain))
                if (!DM_SetAudioVolume(g_stGain.nSpeakerGain))
                {
                    AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
            }

            //set mic gain
            if (!DM_SetMicGain(g_stGain.nMicGain))
            {
                COS_Sleep(10);

                if (!DM_SetMicGain(g_stGain.nMicGain))
                {
                    AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
            }

            //set side gain
            //if (!DM_SetSideGain(g_stGain.nSideGain)) remove by wulc
            if (!DM_SetAudioVolume(g_stGain.nSideGain))
            {
                COS_Sleep(10);

                //if (!DM_SetSideGain(g_stGain.nSideGain))
                if (!DM_SetAudioVolume(g_stGain.nSideGain))
                {
                    AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
            }
        }

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    return;

}

// ^saic: set audio gain: speaker gain, mic gain and sidegain
VOID AT_AUDIO_CmdFunc_SAIC(AT_CMD_PARA *pParam)
{
    /*
        INT32 iRet = ERR_SUCCESS;
        UINT8 paraCount = 0;
        UINT8 nParamLen = 0;
        UINT8 operInfoBuf[40];
        UINT8 arrParam[3];
        BOOL  arrIsParamSet[3];

        if (pParam == NULL)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }

        if (pParam->iType == AT_CMD_TEST)
        {
            AT_MemSet(operInfoBuf, 0, sizeof(operInfoBuf));
            AT_StrCpy(operInfoBuf, "+SAIC: (0-8),(0-16),(0-15)");// yangtt at 2008-04-28 for bug 8224
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, operInfoBuf, AT_StrLen(operInfoBuf), pParam->nDLCI);
            return;
        }
        else if (pParam->iType == AT_CMD_SET)
        {
            iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);

            if (iRet != ERR_SUCCESS || paraCount != 3) // yangtt 08-23-2008 for bug 8048
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }

            if (paraCount == 0)// yangtt 08-23-2008 for bug 8048
            {
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
                return;
            }

            AT_MemZero(arrParam, sizeof(arrParam));

            AT_MemZero(arrIsParamSet, sizeof(arrIsParamSet));

            //get and check params' validity

            while (paraCount-- > 0)
            {
                nParamLen = 1;
                iRet = AT_Util_GetParaWithRule(pParam->pPara, paraCount, AT_UTIL_PARA_TYPE_UINT8,
                                               &arrParam[paraCount], &nParamLen);

                if (nParamLen == 0)// yangtt 08-23-2008 for bug 8048
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    return;
                }

                if (iRet == ERR_SUCCESS)
                {
                    arrIsParamSet[paraCount] = TRUE;

                    switch (paraCount)
                    {

                    case 2:

                        if (arrParam[paraCount] > 15)
                        {
                            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                            return;
                        }

                        break;

                    case 1:

                        if (arrParam[paraCount] > 16)
                        {
                            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                            return;
                        }

                        break;

                    case 0:

                        if (arrParam[paraCount] > 8)
                        {
                            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                            return;
                        }

                        break;

                    default:
                        break;
                    }
                }
                else  if (iRet == ERR_AT_UTIL_CMD_PARA_NULL)
                {
                    arrIsParamSet[paraCount] = FALSE;
                }
                else
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    return;
                }
            }

            //set speaker gain
            if (arrIsParamSet[0])
            {
                if (!DM_SetSpeakerGain(arrParam[0]))
                {
                    COS_Sleep(10);

                    if (!DM_SetSpeakerGain(arrParam[0]))
                    {
                        AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                        return;
                    }
                }

            }

            //set mic gain
            if (arrIsParamSet[1])
            {
                if (!DM_SetMicGain(arrParam[1]))
                {
                    COS_Sleep(10);

                    if (!DM_SetMicGain(arrParam[1]))
                    {
                        AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                        return;
                    }
                }
            }

            //set side gain
            if (arrIsParamSet[2])
            {
                if (!DM_SetSideGain(arrParam[2]))
                {
                    COS_Sleep(10);

                    if (!DM_SetSideGain(arrParam[2]))
                    {
                        AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                        return;
                    }
                }
            }

            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);

            return;
        }
        else if (pParam->iType == AT_CMD_READ)
        {
            //get speaker gain
            if (!DM_GetSpeakerVolume(&arrParam[0]))
            {
                AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }

            //get mic gain
            //arrParam[1] = audio_cfg.micGain;

            //arrParam[1] = 0;

            //get side gain
            //arrParam[2] = audio_cfg.sideGain;

            //arrParam[2] = 0;

            AT_MemSet(operInfoBuf, 0, sizeof(operInfoBuf));

            AT_Sprintf(operInfoBuf, "+SAIC: %u,%u,%u", arrParam[0], arrParam[1], arrParam[2]); // yangtt at 2008-06-06 for bug 8609

            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, operInfoBuf, AT_StrLen(operInfoBuf), pParam->nDLCI);

            return;
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        */
    return;
}

// ^caudio:open(1) or close(0) audio
VOID AT_AUDIO_CmdFunc_CAUDIO(AT_CMD_PARA *pParam)
{
    INT32 iRet      = ERR_SUCCESS;
    UINT8 nAudio    = 0;
    UINT8 paraCount = 0;
    UINT16 nParamLen = 0;
    UINT8 operInfoBuf[20];

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_MemSet(operInfoBuf, 0, sizeof(operInfoBuf));

        // AT_20071220_CAOW_B for bug#7179
        AT_StrCpy(operInfoBuf, "+CAUDIO: (0-1)"); // yangtt at 2008-05-06 for bug 8224
        // AT_20071220_CAOW_E
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, operInfoBuf, AT_StrLen(operInfoBuf), pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        // if testing audio cycle, operation is not allowed.
        if (g_bTesting)
        {
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);

        if (iRet != ERR_SUCCESS || paraCount != 1)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        nParamLen = 1;

        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nAudio, &nParamLen);

        if ((iRet != ERR_SUCCESS) || nAudio > 1)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

#ifdef CHIP_HAS_AP

        if(FALSE == Speeching)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
#endif

        if (nAudio == 0)
        {
            DM_StopAudio();

            /*
               if (!DM_StopAudio())
               {
               AT_TC(g_sw_SPEC,"DM_StopAudio-first time--error !\n");
               COS_Sleep(10);
               if (!DM_StopAudio())
               {
               AT_TC(g_sw_SPEC,"DM_StopAudio-first time--error !\n");
               AT_Result_Err(ERR_AT_CME_EXE_FAIL,CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
               return;
               }
               }
               AT_TC(g_sw_SPEC,"DM_StopAudio success !\n");
             */
        }
        else
        {
            /*
               if (!DM_StopAudio())
               {
               AT_TC(g_sw_SPEC,"DM_StopAudio-first time--error !\n");
               COS_Sleep(10);
               if (!DM_StopAudio())
               {
               AT_TC(g_sw_SPEC,"DM_StopAudio-first time--error !\n");
               AT_Result_Err(ERR_AT_CME_EXE_FAIL,CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
               return;
               }
               }
               AT_TC(g_sw_SPEC,"DM_StopAudio success !\n");
             */
            DM_StopAudio(); // modified for bug 7025
            DM_StartAudio();

            /*
               if (!DM_StartAudio())
               {
               AT_TC(g_sw_SPEC,"DM_StartAudio-first time--error !\n");
               COS_Sleep(10);
               if (!DM_StartAudio())
               {
               AT_TC(g_sw_SPEC,"DM_StartAudio-second time--error !\n");
               AT_Result_Err(ERR_AT_CME_EXE_FAIL,CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
               return;
               }
               }
               AT_TC(g_sw_SPEC,"DM_StartAudio success !\n");
             */
        }

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

}

VOID AT_AUDIO_CmdFunc_CRSL(AT_CMD_PARA *pParam)
{
    INT32 iRet = ERR_SUCCESS;

    // UINT8 nAudio  = 0;
    UINT8 paraCount = 0;
    UINT16 nParamLen = 0;
    UINT8 operInfoBuf[20];
    UINT8 volumelevel;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_MemSet(operInfoBuf, 0, sizeof(operInfoBuf));
        AT_StrCpy(operInfoBuf, "+CRSL: (0-15)");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, operInfoBuf, AT_StrLen(operInfoBuf), pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        // need more work

        if (DM_GetSpeakerVolume(&volumelevel))
        {

            AT_TC(g_sw_SPEC, "volumelevel--%d\n", volumelevel);
            SUL_ZeroMemory8(operInfoBuf, 20);
            SUL_StrPrint(operInfoBuf, "+CRSL: %d", volumelevel);
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, operInfoBuf, AT_StrLen(operInfoBuf), pParam->nDLCI, nSim);
        }

        return;

    }
    else if (pParam->iType == AT_CMD_SET)
    {

        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);

        if (iRet != ERR_SUCCESS || paraCount != 1)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        nParamLen = 1;

        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &volumelevel, &nParamLen);

        if ((iRet != ERR_SUCCESS) || volumelevel > 15 || volumelevel < 0)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

#ifdef CHIP_HAS_AP

        if(FALSE == Speeching)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
#endif

        if (DM_SetAudioVolume(volumelevel))
        {
            // need more work
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;

        }
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

}

// ^snfs:select audio hardware set, that is selecting audio mode:0--aux,1--mic,2--loud
VOID AT_AUDIO_CmdFunc_SNFS(AT_CMD_PARA *pParam)
{
    UINT16 nParamLen = 0;
    UINT8 arrRes[20];
    INT32 iRet       = ERR_SUCCESS;
    UINT8 paraCount  = 0;
    UINT8 nAudioMode = 0;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_MemZero(arrRes, sizeof(arrRes));
        AT_StrCpy(arrRes, "+SNFS: (0-2)");  // yangtt at 2008-04-28 for bug 8224
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        // if testing audio cycle, operation is not allowed.
        if (g_bTesting)
        {
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);

        if (iRet != ERR_SUCCESS || paraCount != 1)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        nParamLen = 1;

        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nAudioMode, &nParamLen);

        if (iRet != ERR_SUCCESS)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (DM_AUDIO_MODE_HEADPHONE == nAudioMode)
        {
            if(!DM_AudForceReceiverMicSelection(FALSE))
            {
                AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            // conver to headset ( android call headset, we call earpiece, why ?)
            nAudioMode = DM_AUDIO_MODE_HANDSET;
        }
        else
        {
            if(!DM_AudForceReceiverMicSelection(TRUE))
            {
                AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

        }

#ifdef CHIP_HAS_AP
        if (DM_AUDIO_MODE_BLUETOOTH_NREC == nAudioMode)
        {
            DM_AudSetBtNRECFlag(TRUE);
        }
        if (DM_AUDIO_MODE_BLUETOOTH_NO_NREC == nAudioMode)
        {
            nAudioMode = DM_AUDIO_MODE_BLUETOOTH_NREC;
            DM_AudSetBtNRECFlag(FALSE);
        }
#endif

        if (!DM_SetAudioMode(nAudioMode))
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }

    }
    else if (pParam->iType == AT_CMD_READ)
    {
        // if testing audio cycle, operation is not allowed.
        if (g_bTesting)
        {
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (!DM_GetAudioMode(&nAudioMode))
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            AT_TC(g_sw_SPEC, "+SNFS :nAudioMode--%d\n", nAudioMode);  // added by yangtt at 2008-6-17 for bug 8609
            AT_MemZero(arrRes, sizeof(arrRes));
            AT_Sprintf(arrRes, "+SNFS: %u", nAudioMode);  // added by yangtt at 2008-6-17 for bug 8609
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
            return;
        }

    }
    else
    {
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}
#if defined AT_SOUND_RECORDER_SUPPORT  || defined AT_AUDIO_PLAY
#define AT_RECORD_BUF_SIZE 1024*30
#define ST_SUCCESS                     1
#define        ST_FAILURE                      0
#define AMR_HEADER   "#!AMR\n"
UINT8 gATRecordBuf[AT_RECORD_BUF_SIZE] = {0x00,};
#define   AT_TFLASH_ROOT                  "/t"
#define   AT_FLASH                  "/t1"
#define MIC_RECODER_VOLUME 13
#define AT_RECORD_BUF_SIZE 1024*30
UINT32 nATRecordBufUsedSize = 0;
INT32 iTcardRet = 0;
INT32 iFlashRet = 0;
UINT8 g_recName[30]= {0,};
INT32 iFd = -1;
INT32 iRecordStatus = 0;
INT32 iPlayStatus = 0;
const UINT8 AMR_HEADER_BUF[]=AMR_HEADER;

extern void Delay_for_audio(UINT32 time);

// extern BOOL DM_SetSideGain ( DM_SideToneGain nGain);

UINT8 audio_mute = 0;
VOID AT_Audio_Init(VOID)
{
    BOOL ret = 0;

    Delay_for_audio(5);
    ret = DM_SetAudioVolume(gATCurrentVGR_Volume);
    AT_TC(g_sw_SPEC, "  DM_ResumMic  AT_Audio_Init  ret = %d\n", ret);

    if (0 == gATCurrentCMUT_mute)
    {
        Delay_for_audio(5);
        ret = DM_ResumMic();
        AT_TC(g_sw_SPEC, "  DM_ResumMic  AT_Audio_Init  ret = %d\n", ret);
    }
    else if (1 == gATCurrentCMUT_mute)
    {
        Delay_for_audio(5);
        ret        = DM_MuteMic();
        audio_mute = 1;
        AT_TC(g_sw_SPEC, "   DM_MuteMic  AT_Audio_Init  ret =  %d\n", ret);

    }

    Delay_for_audio(5);

    if (audio_mute == 0)
    {
        ret = DM_SetMicGain(gATCurrentVGT_Volume);
        AT_TC(g_sw_SPEC, "    AT_Audio_Init  gATCurrentVGT_Volume =%d gATCurrentVGR_Volume =%d ret = %d\n",
              gATCurrentVGT_Volume, gATCurrentVGR_Volume, ret);
    }
    // mount T card
    if(TRUE != iTcardRet)
    {
        AT_TC(g_sw_SPEC, "    AT_Audio_Init  Tcard mount");
        iTcardRet = AT_INIT_TCard();
    }
    //write record file
    if(TRUE == iTcardRet || TRUE == iFlashRet)
    {
        INT32 uiULen = 0;
        UINT8 g_szTestResultFile[60] = {0,};
        INT32 iResult = 0;

        if(iTcardRet)
            SUL_StrPrint(g_szTestResultFile,"/t/record.amr");
        else
            SUL_StrPrint(g_szTestResultFile,"/t1/record.amr");
        AnsiiToUnicodeString(g_recName, g_szTestResultFile);

        iFd = FS_Open(g_recName, (FS_O_RDONLY), 0);
        if (iFd < 0)
        {

            iFd = FS_Open(g_recName, (FS_O_RDWR | FS_O_CREAT ), 0);

            if (iFd < 0)
            {
                AT_TC(g_sw_GC, "[%s]openfailing.h=%d", g_szTestResultFile,iFd);
                iFd = FS_Create((PCSTR)g_recName, 0);
                return;
            }

            iResult = FS_Write(iFd , AMR_HEADER_BUF , sizeof(AMR_HEADER_BUF));
        }
        FS_Close(iFd);
    }
}

VOID ATRecordBufPlayCallBack(unsigned char *pBuf, unsigned int len, unsigned int *recordLen)
{
    //AT_TC(g_sw_SPEC, "ATRecordBufPlayCallBack addr0x%x,len:%d,result%d", pBuf, len, *recordLen);

    //sxs_Dump(MCI_AUDIO_TRC,0,pBuf,len);
    if((AT_RECORD_BUF_SIZE - nATRecordBufUsedSize) >= len)
    {
        SUL_MemCopy8(&gATRecordBuf[nATRecordBufUsedSize], pBuf, len);
        nATRecordBufUsedSize += len;
    }
    else
    {
        nATRecordBufUsedSize = 0x00;
        SUL_MemCopy8(&gATRecordBuf[nATRecordBufUsedSize], pBuf, len);
        nATRecordBufUsedSize += len;
    }

    *recordLen = len;//AT_RECORD_BUF_SIZE - nATRecordBufUsedSize;
}

//MCI_AUDIO_FILE_RECORD_CALLBACK_T
VOID ATStartRecordCallBack(AT_RECORD_ERR_T result)
{
    UINT8 arrRes[120];
    AT_TC(g_sw_SPEC, "ATStartRecordCallBack result %x", result);
    hal_HstSendEvent(0x15090001);
    hal_HstSendEvent(result);
    if(AT_RECORD_ERR_DISK_FULL == result)
    {
        AT_Sprintf(arrRes, "+SRD: disk full  %d", AT_RECORD_ERR_DISK_FULL);
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), g_nDLCI, g_nSim);
    }
}
VOID ATPlayRecordCallBack(AT_RECORD_ERR_T result)
{
  //  AT_TC(g_sw_SPEC, "ATPlayRecordCallBack result %x", result)jkk
    iRecordStatus = 0;
    FS_Close(iFd);
    hal_HstSendEvent(0x05090001);
    hal_HstSendEvent(result);
}
VOID ATPlayCallBack(AT_RECORD_ERR_T result)
{
    iPlayStatus = 0;
    FS_Close(iFd);
}
VOID AT_AUDIO_StartRecord()
{
    nATRecordBufUsedSize = 0;
    MCI_AudioRecordStart(2000, AT_RECORD_TYPE_AMR, AT_RECORD_PLAY_MODE_AMR122, ATStartRecordCallBack, ATRecordBufPlayCallBack);
}
VOID AT_AUDIO_StartRecordFile()
{
    nATRecordBufUsedSize = 0;
    MCI_AudioRecordStart(iFd, AT_RECORD_TYPE_AMR, AT_RECORD_PLAY_MODE_AMR122, ATStartRecordCallBack, NULL);
}

VOID AT_AUDIO_StopRecord()
{
    MCI_AudioRecordStop();
}

VOID AT_AUDIO_StopRecordFile()
{
    MCI_AudioRecordStop();
    fs_close(iFd);
    iFd = -1;
}

VOID AT_AUDIO_ResumeRecord()
{
    MCI_AudioRecordResume();
}

VOID AT_AUDIO_PauseRecord()
{
    MCI_AudioRecordPause();
}

VOID AT_AUDIO_PlayRecord()
{
    MCI_AudioPlayBuffer(gATRecordBuf, nATRecordBufUsedSize, FALSE, ATPlayRecordCallBack, AT_RECORD_PLAY_MODE_AMR_RING, 0);
}

VOID AT_AUDIO_PlayRecordFile()
{
    iFd = FS_Open(g_recName, (FS_O_RDONLY), 0);
    if (iFd < 0)
    {
        AT_TC(g_sw_GC, "open failing.h=%d", iFd);
        iRecordStatus = 0;
        iFd = FS_Create(g_recName, 0);
    }
    AT_TC(g_sw_GC, "open read file  OK.h=%d", iFd);

    if(MCI_AudioPlay(0,iFd,3,ATPlayRecordCallBack,0))
    {
        iRecordStatus = 0;
    }
    DM_SetAudioMode(DM_AUDIO_MODE_LOUDSPEAKER);
}

INT32 AT_AUDIO_PlayFile(char *recname, mci_type_enum fielType,char volume, char channel)
{

    iFd = FS_Open(recname, (FS_O_RDONLY), 0);
    if (iFd < 0)
    {
        AT_TC(g_sw_GC, "open failing.h=%d", iFd);
        return 1;
    }
    iPlayStatus = 1;
    AT_TC(g_sw_GC, "open read file  OK.h=%d", iFd);
    //fs_getfilesize(ifd)
    if(100 < volume) volume = 100;
    if(0 > volume) volume = 0;
    if(volume)
    {
        volume = volume*15/100;
        if(0 == volume) volume =1;
    }
    INT32 ret = MCI_AudioPlay(0,iFd,fielType,ATPlayCallBack,0);
    if(0 != ret)
    {
        iPlayStatus = 0;
        return 4; // play error
    }

    COS_Sleep(200);

    //DM_AudForceReceiverMicSelection(TRUE);
    if(channel)
    {
        DM_SetAudioMode(DM_AUDIO_MODE_LOUDSPEAKER);
    }
    else
    {
        DM_SetAudioMode(DM_AUDIO_MODE_EARPIECE);
    }
    DM_SetAudioVolume(volume);

    return 0;
}

VOID AT_AUDIO_Stop()
{
    MCI_AudioStop();
    fs_close(iFd);
    iFd = -1;
}
INT AT_INIT_TCard(void)
{
    UINT8 i = 0;
    UINT8 pUName[50]    = {0,};
    UINT8 szTFPoint[50] = {0,};
    INT32 iResult   = 0x00;
    UINT32 uiDevCount   = 0;
    UINT32 uiULen   = 0;

    FS_DEV_INFO* pDevInfo   = NULL;
    BOOL bIsMountT  = FALSE;
    BOOL bRet   = TRUE;
    FS_INFO psFSInfo;
    PFS_FIND_DATA pFindData;

    FS_GetFSInfo("VDS0", &psFSInfo);
    AT_TC(g_sw_SPEC, "+SRD :FS_GetFSInfo --vds0.iTotalSize = %d, vds1.iUsedSize = %d \n", psFSInfo.iTotalSize,psFSInfo.iUsedSize);

    iResult = FS_GetDeviceInfo(&uiDevCount, &pDevInfo);
    if(ERR_SUCCESS != iResult)
    {
        AT_TC(g_sw_SPEC, "Device not register.\n");
        uiDevCount = 0;
        bRet = FALSE;
    }

    for(i = 0; i < uiDevCount; i++)
    {
        if(pDevInfo[i].dev_type == FS_DEV_TYPE_FLASH)
        {
            if( 0 ==    SUL_StrCompare(pDevInfo[i].dev_name,"VDS1"))
            {
                AnsiiToUnicodeString(pUName, AT_FLASH);
                iResult = FS_MakeDir(pUName,0);
                if(ERR_SUCCESS == iResult || ERR_FS_FILE_EXIST == iResult)
                {
                    iResult = FS_Mount(pDevInfo[i].dev_name,pUName,0,FS_TYPE_FAT);
                    if(ERR_SUCCESS == iResult)
                    {
                        AT_TC(g_sw_SPEC, "1 %dMount the flash device[%s] success.",i,pDevInfo[i].dev_name);
                        iFlashRet = TRUE;
                    }
                    else
                    {
                        AT_TC(g_sw_SPEC, "2 %dMount the flash device[%s] fail.err code = %d.",i,pDevInfo[i].dev_name,iResult);
                        bRet = FALSE;
                    }
                }
                else
                {
                    AT_TC(g_sw_SPEC, "3 %d Mount [%s] on flash device[%s] fail.err code = %d.root=%d",i,pUName,pDevInfo[i].dev_name,iResult,pDevInfo[i].is_root);
                    bRet = FALSE;
                }
            }

            continue;
        }

        //  SUL_StrCopy(szTFPoint,AT_TFLASH_ROOT);
        //  iResult= ML_LocalLanguage2Unicode((const UINT8*)szTFPoint,SUL_Strlen(szTFPoint),&pUName, &uiULen,NULL);
        AnsiiToUnicodeString(pUName, AT_TFLASH_ROOT);
        if(ERR_SUCCESS == iResult)
        {

            iResult = FS_MakeDir(pUName,0);
            if(ERR_SUCCESS == iResult || ERR_FS_FILE_EXIST == iResult)
            {
                iResult = FS_Mount(pDevInfo[i].dev_name,pUName,0,FS_TYPE_FAT);
                if(ERR_SUCCESS == iResult)
                {
                    AT_TC(g_sw_SPEC, "1 %dMount the t-flash device[%s] success.",i,pDevInfo[i].dev_name);
                    bIsMountT = TRUE;
                }
                else
                {
                    AT_TC(g_sw_SPEC, "2 %dMount the t-flash device[%s] fail.err code = %d.",i,pDevInfo[i].dev_name,iResult);
                    bRet = FALSE;
                }
            }
            else
            {
                AT_TC(g_sw_SPEC, "3 %d Mount [%s] on t-flash device[%s] fail.err code = %d.root=%d",i,pUName,pDevInfo[i].dev_name,iResult,pDevInfo[i].is_root);
                bRet = FALSE;
            }
        }
        else
        {
            AT_TC(g_sw_SPEC, "Local2Unicode fail.err code = %d.",iResult);
            bRet = FALSE;
        }
    }
    return bIsMountT;
}
#ifdef AT_SOUND_RECORDER_SUPPORT
VOID AT_AUDIO_StopAll()
{
    if(iRecordStatus == 2)
    {
        AT_AUDIO_StopRecordFile();
        iRecordStatus = 0;
    }
    else if(iRecordStatus == 8)
    {
        COS_Sleep(200);
        AT_AUDIO_Stop();
        iRecordStatus = 0;
    }
    if(iPlayStatus) iPlayStatus =0;
    AT_AUDIO_Stop();
    // MCI_AudioStop();
}
VOID AT_AUDIO_StopRing(VOID)
{
    DM_StopAudio();
    DM_StopTone();
    AT_AUDIO_Stop();
    DM_Audio_StopStream();
}
VOID aud_media_Ring_play_ind_hdlr(MCI_ERR_T result ) //         MCI_MEDIA_PLAY_FINISH_IND,
{


    switch (result)
    {
        case MCI_ERR_END_OF_FILE:
            // mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("aud_media_Ring_play_ind_hdlr: State %d",0x09000003),result );
            hal_HstSendEvent(0x11112222);hal_HstSendEvent(0x11113531);
            // ind_p->event = MEDIA_END;
            break;

        case MCI_ERR_INVALID_FORMAT:
            //   mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("aud_media_Ring_play_ind_hdlr: State %d",0x09000004),result );
            hal_HstSendEvent(0x11112222);hal_HstSendEvent(0x11113537);
            // Send indicator to the MEDIA task
            // ind_p->event = MEDIA_ERROR;
            break;

        default:
            hal_HstSendEvent(0x11112222);hal_HstSendEvent(0x11113542);
            //   mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1),TSTR("aud_media_Ring_play_ind_hdlr: Unknown State %d",0x09000005),result);
            break;
    }


}
U8 g_ring_output_path_default = DM_AUDIO_MODE_LOUDSPEAKER;
VOID AT_AUDIO_PlayRing(VOID)
{
    int ret;
    UINT8 pVolume;

    DM_GetSpeakerVolume(&pVolume);

    if(g_PlayBuffSetup)
    {
        DM_PlayToneSetup(1,  pVolume);
    }
    else
        g_PlayBuffSetup = 1;

    ret = MCI_AudioPlayBuffer((UINT32 *)AT_Sound3_amr, sizeof(AT_Sound3_amr) / 4,0,aud_media_Ring_play_ind_hdlr, AT_RECORD_PLAY_MODE_AMR_RING,0);
    if (!DM_SetAudioVolume(pVolume))
    {
        //AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    }

    if(DM_AUDIO_MODE_LOUDSPEAKER == g_ring_output_path_default )
    {
        DM_SetAudioMode(DM_AUDIO_MODE_LOUDSPEAKER);
        MCI_AudioSetOutputPath(MCI_PATH_LSP, 0);
    }
    else
    {
        DM_SetAudioMode(DM_AUDIO_MODE_EARPIECE);
        MCI_AudioSetOutputPath(MCI_PATH_NORMAL, 0);
    }

}
VOID AT_AUDIO_CmdFunc_SRD(AT_CMD_PARA *pParam)
{
    UINT16 nParamLen;
    UINT8 arrRes[120];

    INT32 iRet       = ERR_SUCCESS;
    UINT8 paraCount;
    UINT8 nAudioMode;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    g_nDLCI = pParam->nDLCI;
    g_nSim = nSim;
    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_MemZero(arrRes, sizeof(arrRes));
        AT_StrCpy(arrRes, "+SRD: file record(2-3-8)");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if (g_bTesting)
        {
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        UINT32 uStat = CFW_CcGetCallStatus(nSim);

        if(uStat != 0x00)  //CC_STATE_NULL
        {
            AT_Sprintf(arrRes, "+SRD:in  calling %d. )",uStat);
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
            return;
        }

        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if (iRet != ERR_SUCCESS || paraCount < 1)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        nParamLen = 1;

        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nAudioMode, &nParamLen);
        if (iRet != ERR_SUCCESS)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if(iPlayStatus)
        {
            AT_Sprintf(arrRes, "+SRD: player busy now\n");
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
            return;
        }
        if(2 == nAudioMode)
        {
            UINT16 uONSize = 0;
            UINT32 iResult = ERR_SUCCESS;

            if((TRUE == iTcardRet || TRUE == iFlashRet)&& g_recName[0]&& iRecordStatus == 0)
            {

                if( FS_Delete(g_recName) )
                {
                    COS_Sleep(500);
                    MCI_AudioStop();
                    fs_close(iFd);
                    iFd = -1;
                    COS_Sleep(500);
                    FS_Delete(g_recName);
                }

                iFd = FS_Open(g_recName, (FS_O_RDWR |FS_O_CREAT ), 0);
                if (iFd < 0)
                {
                   // AT_TC(g_sw_GC, "openfailing.h=%d", iFd);
                    //iFd = FS_Create((PCSTR)g_recName, 0);
                    AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                iResult = FS_Write(iFd , AMR_HEADER_BUF , sizeof(AMR_HEADER_BUF));
                iRecordStatus = 2;
                AT_AUDIO_StartRecordFile();
            }
            else
            {
                AT_MemZero(arrRes, sizeof(arrRes));
                if((FALSE == iTcardRet) && (FALSE == iFlashRet))
                    AT_Sprintf(arrRes, "+SRD: need mount Tcard or Flash\n");
                else
                {
                    if(0 == g_recName[0])
                    {
                        iFd = FS_Create((PCSTR)g_recName, 0);
                        AT_Sprintf(arrRes, "+SRD: no record file ,try again");
                    }
                }
                if(8 == iRecordStatus)
                    AT_Sprintf(arrRes, "+SRD: playing now\n");
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
                return;
            }
           // AT_TC(g_sw_GC, "================");

        }
        else if(3 == nAudioMode)
        {
            if(iRecordStatus == 2){
                AT_AUDIO_StopRecordFile();
                iRecordStatus = 0;
            }
            else if(iRecordStatus == 8)
            {
                COS_Sleep(500);
                AT_AUDIO_Stop();
                COS_Sleep(500);
                iRecordStatus = 0;
            }
            else
            {
                AT_Sprintf(arrRes, "+SRD: no need stop\n");
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
                return;
            }
        }

        else if(8 == nAudioMode)
        {
            if(g_recName[0] && iRecordStatus == 0)
            {
                iRecordStatus = 8;
                AT_AUDIO_PlayRecordFile();
            }
            else
            {
                AT_MemZero(arrRes, sizeof(arrRes));
                if(0 ==g_recName[0] )
                    AT_Sprintf(arrRes, "+SRD: need record first");
                if(2 == iRecordStatus)
                    AT_Sprintf(arrRes, "+SRD: recording now,please stop recorder");
                if(8 == iRecordStatus)
                    AT_Sprintf(arrRes, "+SRD: playing now,");
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
                return;
            }
        }
        else if(9 == nAudioMode)
        {
            AT_MemZero(arrRes, sizeof(arrRes));
            AT_AUDIO_PlayRing();
            AT_Sprintf(arrRes, "+SRD: Buffer playing now,");
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
            return;
        }
        else if(10 == nAudioMode)
        {
            AT_MemZero(arrRes, sizeof(arrRes));
            AT_AUDIO_StopRing();
            AT_Sprintf(arrRes, "+SRD: Buffer playing stop");
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
            return;
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;

    }
    else if (pParam->iType == AT_CMD_READ)
    {
        // if testing audio cycle, operation is not allowed.
        if (g_bTesting)
        {
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        {
            //AT_TC(g_sw_SPEC, "+SRD :--%d\n", iRecordStatus);
            AT_MemZero(arrRes, sizeof(arrRes));
            AT_Sprintf(arrRes, "+SRD: %u", iRecordStatus);
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
            return;
        }

    }
    else
    {
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

}
#endif
#ifdef AT_AUDIO_PLAY
VOID AT_AUDIO_CmdFunc_PLY(AT_CMD_PARA *pParam)
{
    INT32 IRET = ERR_SUCCESS;
    PAT_CMD_RESULT pResult;
    UINT8 nOutputStr[36] = { 0, };

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    if (NULL ==pParam)
    {
        goto PLY_ERROR;
    }
    else
    {
        AT_CMD_PARA *param = pParam;
        if (NULL == param->pPara)
            goto PLY_ERROR;

        switch (param->iType)
        {
            case AT_CMD_TEST:
                SUL_StrCat(nOutputStr, "+CAUDPLA:(1-2),(filename),(0-1-2),(0-1),(0-100)");
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI, nSim);
                return;
            case AT_CMD_READ:
                SUL_StrPrint(nOutputStr, "+CAUDPLAY: %d", iPlayStatus);//is playing
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI, nSim);
                return;
            case AT_CMD_SET:
                {
#define MAX_FILENAME_LEN 64
                    uint8 uParaCount = 0;
                    uint8 mode = 0;
                    uint32 nRet =ERR_SUCCESS;
                    uint8 filename[MAX_FILENAME_LEN * 2] = {0,};
                    uint8 format = 0;
                    uint8 channel = 0;
                    uint8 volume = 0;
                    uint16 uSize ;
                    uint8 i=0;
                    uint8 g_szTestResultFile[MAX_FILENAME_LEN *2] = {0,};

                    nRet = AT_Util_GetParaCount(pParam->pPara, &uParaCount);

                    if(ERR_SUCCESS != nRet)
                        goto PLY_ERROR;

                    if (uParaCount < 0)
                        goto PLY_ERROR;

                uSize  = 1;
                AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &mode, &uSize);
                if(5 == uParaCount)
                {
                    uSize  = MAX_FILENAME_LEN;//sizeof(filename) / 2;
                    AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, filename, &uSize);

                    uSize = 1;
                    AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT8, &format, &uSize);
                    AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_UINT8, &channel, &uSize);
                    AT_Util_GetParaWithRule(pParam->pPara, 4, AT_UTIL_PARA_TYPE_UINT8, &volume, &uSize);
                    if(mode > 2 || mode < 1||volume > 100 ||channel > 1 || format > 2 || filename ==NULL)
                        goto PLY_ERROR;
                }
                else if(1 == uParaCount)
                {
                    if(mode != 2)
                        goto PLY_ERROR;
                }
                else
                    goto PLY_ERROR;

                pParam->pPara = NULL;

                if(mode == 1) // play audio
                {
                    UINT32 uStat = CFW_CcGetCallStatus(nSim);

                    if(uStat != 0x00)  //CC_STATE_NULL
                    {
                        AT_Sprintf(nOutputStr, "+CALLREC:in  calling %d. )",uStat);
                        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI, nSim);
                        return;
                    }

                    int32 ret = 0;
                    if(1 == iPlayStatus)
                    {
                        ret = 2;
                    }
                    else
                    {
                        SUL_StrPrint(g_szTestResultFile,filename);
                        AnsiiToUnicodeString(filename, g_szTestResultFile);

                        if(0 == format)
                            {
                            if(strstr(g_szTestResultFile,".wav") ||strstr(g_szTestResultFile,".WAV") )
                                ret = AT_AUDIO_PlayFile(filename,MCI_TYPE_WAV, volume, channel);
                            else
                                ret = 3;
                            }
                        if(1 == format)
                            {
                            if(strstr(g_szTestResultFile,".amr") ||strstr(g_szTestResultFile,".AMR") )
                                ret = AT_AUDIO_PlayFile(filename,MCI_TYPE_AMR, volume, channel);
                            else
                                ret = 3;
                            }
                        if(2 == format)
                            {
                            //SUL_StrPrint(nOutputStr, "+CAUDPLAY: 1, %s ||%s",g_szTestResultFile,filename );
                            //AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,30, nOutputStr, AT_StrLen(nOutputStr),pParam->nDLCI, nSim);
                                if(strstr(g_szTestResultFile,".mp3") || strstr(g_szTestResultFile,".MP3"))
                                {
                                   ret = AT_AUDIO_PlayFile(filename,MCI_TYPE_DAF, volume, channel);
                                }
                                else
                                    ret = 3;
                            }
                    }

                    if(2 == ret)
                        SUL_StrPrint(nOutputStr, "+CAUDPLAY: 1, %d, must stop the previous play",ret);//is playing
                   else if(3 == ret)
                        SUL_StrPrint(nOutputStr, "+CAUDPLAY: 1, %d,format error",ret);
                   else if(1 == ret)
                        SUL_StrPrint(nOutputStr, "+CAUDPLAY: 1, %d,not find file",ret);
                   else if(4 == ret)
                        SUL_StrPrint(nOutputStr, "+CAUDPLAY: 1, %d,invalid file",ret);
                   else
                        SUL_StrPrint(nOutputStr, "+CAUDPLAY: 1, %d",ret);

                    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,30, nOutputStr, AT_StrLen(nOutputStr),pParam->nDLCI, nSim);
                    return;
                }
                else if(mode == 2) // stop
                {
                    iPlayStatus= 0;
                    COS_Sleep(500);
                    AT_AUDIO_Stop();
                    COS_Sleep(500);

                    SUL_StrPrint(g_szTestResultFile,"/t/record.amr");
                    AnsiiToUnicodeString(g_recName, g_szTestResultFile);
                }
                else
                {
                    goto PLY_ERROR;
                }
                if(ERR_SUCCESS == nRet)
                    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 30, NULL, 0, pParam->nDLCI, nSim);
                else
                    AT_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            default:
                break;
        }
    }

PLY_ERROR:
    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR,
                          CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);

    if (pResult != NULL)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }
    return;

}
#endif

#ifdef ATA_AUDIOLBTEST
/*
#define AT_AUDIO_TEST_EARPIECE_CFW  0
    // 听筒
#define AT_AUDIO_TEST_HANDSET_CFW   1
    // 耳机
#define AT_AUDIO_TEST_LOUDSPEAKER_CFW   2
    // 免提
*/
UINT8 gATLBRecordBuf[AT_RECORD_BUF_SIZE] = {0x00,};
UINT32 nATLBRecordBufUsedSize = 0;
UINT8 g8kSamp16bitPCM[AT_RECORD_BUF_SIZE]= {0x00};
UINT8 gATLBNDLCI = 0x00;
AT_SIM_ID ngATLBSimID = CFW_SIM_0;
VOID ATStartLBRecordCallBack(AT_RECORD_ERR_T result)
{
    //AT_TC(g_sw_SPEC, "ATStartRecordCallBack result %x", result);
    hal_HstSendEvent(0x05090001);
    hal_HstSendEvent(result);
}
VOID ATLBRecordBufPlayCallBack(unsigned char *pBuf, unsigned int len, unsigned int *recordLen)
{
   // AT_TC(g_sw_SPEC, "ATRecordBufPlayCallBack addr0x%x,len:%d,result%d", pBuf, len, *recordLen);

    //sxs_Dump(MCI_AUDIO_TRC,0,pBuf,len);
    if((AT_RECORD_BUF_SIZE - nATLBRecordBufUsedSize) >= len)
    {
        SUL_MemCopy8(&gATLBRecordBuf[nATLBRecordBufUsedSize], pBuf, len);
        nATLBRecordBufUsedSize += len;
    }
    else
    {
        nATRecordBufUsedSize = 0x00;
        SUL_MemCopy8(&gATLBRecordBuf[nATLBRecordBufUsedSize], pBuf, len);
        nATLBRecordBufUsedSize += len;
    }

    *recordLen = len;//AT_RECORD_BUF_SIZE - nATRecordBufUsedSize;

    //Do KissFFT
    //
    MCI_AudioRecordStop();
    UINT8 uOutputStr[20] = { 0 };

    SUL_StrPrint(uOutputStr, "+MIC: ");
    AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), gATLBNDLCI, ngATLBSimID);

}
UINT32 ATAPBSStreamUserHandler(APBS_STREAM_STATUS_T nSST)
{
   // AT_TC(g_sw_SPEC, "ATAPBSStreamUserHandler  nSST%x", nSST);
}

VOID AT_AUDIO_LBTest(SND_ITF_T  nMod)
{
    DM_AudSetCurOutputDevice(nMod);
    //Record firstly
    MCI_AudioRecordStart(2000, AT_RECORD_TYPE_PCM_8K, AT_RECORD_PLAY_MODE_STREAM_PCM, ATStartLBRecordCallBack, ATLBRecordBufPlayCallBack);

    //Play secondly
    MCI_AUD_StreamPlayPCM(g8kSamp16bitPCM,AT_RECORD_BUF_SIZE,MCI_PLAY_MODE_STREAM_PCM,ATAPBSStreamUserHandler,8000,16);
}

VOID AT_AUDIO_CmdFunc_MIC(AT_CMD_PARA *pParam)
{
    INT32 iRetValue;
    UINT8 uParaCount;
    UINT8 uData;
    UINT16 uStrLen;
    BOOL iRet;
    UINT8 uOutputStr[20] = { 0 };

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    // Check [in] Param

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET:

            if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                //TODO
                SUL_ZeroMemory8(uOutputStr, 20);

                SUL_StrCat(uOutputStr, "+MIC:");
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);

                return;
            }
            break;

        case AT_CMD_TEST:
            SUL_ZeroMemory8(uOutputStr, 20);

            SUL_StrCat(uOutputStr, "+MIC");
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        case AT_CMD_READ:
            SUL_ZeroMemory8(uOutputStr, 20);

            SUL_StrPrint(uOutputStr, "+MIC");
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        case AT_CMD_EXE:
            AT_AUDIO_LBTest(SND_ITF_RECEIVER  );
            gATLBNDLCI = pParam->nDLCI;
            ngATLBSimID = nSim;
            break;
        default:
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }
    }

    return;
}
VOID AT_AUDIO_CmdFunc_SPEAKER(AT_CMD_PARA *pParam)
{
    INT32 iRetValue;
    UINT8 uParaCount;
    UINT8 uData;
    UINT16 uStrLen;
    BOOL iRet;
    UINT8 uOutputStr[20] = { 0 };

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    // Check [in] Param

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET:

            if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                //TODO
                SUL_ZeroMemory8(uOutputStr, 20);

                SUL_StrCat(uOutputStr, "+SPEAKER:");
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);

                return;
            }
            break;

        case AT_CMD_TEST:
            SUL_ZeroMemory8(uOutputStr, 20);

            SUL_StrCat(uOutputStr, "+SPEAKER");
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        case AT_CMD_READ:
            SUL_ZeroMemory8(uOutputStr, 20);

            SUL_StrPrint(uOutputStr, "+SPEAKER");
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        case AT_CMD_EXE:
            AT_AUDIO_LBTest(SND_ITF_LOUD_SPEAKER );
            gATLBNDLCI = pParam->nDLCI;
            ngATLBSimID = nSim;
            break;
        default:
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }
    }

    return;
}
VOID AT_AUDIO_CmdFunc_RECEIVER(AT_CMD_PARA *pParam)
{
    INT32 iRetValue;
    UINT8 uParaCount;
    UINT8 uData;
    UINT16 uStrLen;
    BOOL iRet;
    UINT8 uOutputStr[20] = { 0 };

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    // Check [in] Param

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET:

            if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                //TODO
                SUL_ZeroMemory8(uOutputStr, 20);

                SUL_StrCat(uOutputStr, "+RECEIVER:");
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);

                return;
            }
            break;

        case AT_CMD_TEST:
            SUL_ZeroMemory8(uOutputStr, 20);

            SUL_StrCat(uOutputStr, "+RECEIVER");
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        case AT_CMD_READ:
            SUL_ZeroMemory8(uOutputStr, 20);

            SUL_StrPrint(uOutputStr, "+RECEIVER");
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        case AT_CMD_EXE:
            AT_AUDIO_LBTest(SND_ITF_RECEIVER);
            gATLBNDLCI = pParam->nDLCI;
            ngATLBSimID = nSim;
            break;
        default:
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }
    }

    return;
}
VOID AT_AUDIO_CmdFunc_HEADSET(AT_CMD_PARA *pParam)
{
    INT32 iRetValue;
    UINT8 uParaCount;
    UINT8 uData;
    UINT16 uStrLen;
    BOOL iRet;
    UINT8 uOutputStr[20] = { 0 };

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    // Check [in] Param

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        switch (pParam->iType)
        {

        case AT_CMD_SET:

            if ((pParam->pPara == NULL) || (ERR_SUCCESS != (iRetValue = AT_Util_GetParaCount(pParam->pPara, &uParaCount)))) /* GET param count */
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            else
            {
                //TODO
                SUL_ZeroMemory8(uOutputStr, 20);

                SUL_StrCat(uOutputStr, "+HEADSET:");
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);

                return;
            }
            break;

        case AT_CMD_TEST:
            SUL_ZeroMemory8(uOutputStr, 20);

            SUL_StrCat(uOutputStr, "+HEADSET");
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        case AT_CMD_READ:
            SUL_ZeroMemory8(uOutputStr, 20);

            SUL_StrPrint(uOutputStr, "+HEADSET");
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutputStr, AT_StrLen(uOutputStr), pParam->nDLCI, nSim);
            break;

        case AT_CMD_EXE:
            AT_AUDIO_LBTest(SND_ITF_EAR_PIECE);
            gATLBNDLCI = pParam->nDLCI;
            ngATLBSimID = nSim;
            break;
        default:
            AT_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }
    }

    return;
}

#endif
#endif

typedef enum
{
    AT_PLAY_TONE_NORMAL,
    AT_PLAY_TONE_CYCLE,
} AT_PLAY_TONE_MODE;

struct dtmf_code {
    UINT16 tone;
    UINT32 persistence;
    UINT32 silence;
};

struct qwdtmf_info {
    UINT8 size;
    UINT8 idx;
    UINT8 dl_volume;
    UINT8 ul_volume;
    struct dtmf_code *dtmf_array;
};

struct qwdtmf_info g_qwdtmf_info = {0, 0, NULL};
struct QLDTMF_INFO {
    struct dtmf_code *dtmf_code;
    UINT8 vol;
    UINT8 size;
    UINT8 idx;
    BOOL timer;
};
static struct QLDTMF_INFO ldtmf_info = {NULL, 0,0,0,0};
static AT_PLAY_TONE_MODE at_tone_mode = AT_PLAY_TONE_NORMAL;
static UINT16 qltone_duration = 0;
static VOID ldtmf_info_reset(struct QLDTMF_INFO *di)
{
    if (di->dtmf_code != NULL)
        AT_FREE(di->dtmf_code);
    di->dtmf_code = NULL;
    di->size = 0;
    di->idx = 0;
    return;
}
static void tone_stop()
{
    DM_StopTone();
    qltone_duration = 0;
    at_tone_mode = AT_PLAY_TONE_NORMAL;
    ldtmf_info_reset(&ldtmf_info);
}

VOID DTMF_CallBack(UINT8 key)
{
    COS_EVENT nEvent = { 0 };
    nEvent.nEventId = EV_CFW_SPECIAL_DTMF_KEY_IND;
    nEvent.nParam1 = key;
    nEvent.nParam2 = 0;
    nEvent.nParam3 = 0;
    COS_SendEvent(g_hAtTask, &nEvent, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

VOID AT_AUDIO_AsyncEventProcess(COS_EVENT *pEvent)
{
    CFW_EVENT ev;
    CFW_EVENT *pEv = &ev;
    UINT8 OutStr[50];
    memset(OutStr, 0, 50);
    AT_CosEvent2CfwEvent(pEvent, pEv);
    UINT8 nSim = pEv->nFlag;

    switch (pEvent->nEventId)
    {
        case EV_CFW_SPECIAL_DTMF_KEY_IND:
            if (cs_hex == cfg_GetTeChset())
                AT_Sprintf(OutStr, "+DTMF: %d", pEv->nParam1);
            else
                AT_Sprintf(OutStr, "+DTMF: %c", pEv->nParam1);

            AT_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, OutStr, AT_StrLen(OutStr), pEv->nUTI, nSim);
            break;

        default :
            break;
    }
}

VOID AT_AUDIO_DTMF_DetachStop()
{
    if (DTMF_GetState())
    {
        DTMF_DetectStop();
        if (csw_GetResourceActivity(CSW_LP_RESOURCE_UNUSED_1) != def_sys_freq)
            csw_SetResourceActivity(CSW_LP_RESOURCE_UNUSED_1, def_sys_freq);
    }
}

cii_DTMF_term localtone[2] =
{
    {AUD_COMFORT_425, 2000},
    {AUD_NO_TONE, 1000}
};

static INT8 tone_char2type(UINT8 c)
{
    if (c >= '0' && c <= '9')
        return DM_TONE_DTMF_0 + (c -'0');
    else if (c >= 'A' && c <= 'D')
        return DM_TONE_DTMF_A + (c -'A');
    else if (c == '*')
        return DM_TONE_DTMF_S;
    else if (c == '#')
        return DM_TONE_DTMF_P;
    else
        return -1;
}

/* str format is "1,2,3,..." */
static INT32 AT_AUDIO_TONE_char2type(UINT8 *str, UINT8 *type, UINT8 size)
{
    INT8 t;
    UINT8 i,count = 0;
    UINT8 *p;

    if (size > 39)
        return -1;

    for (i = 0; i < size; i++)
    {
        p = str + i;
        if (i % 2)
        {
            if (*p != ',')
                return -1;
        }
        else
        {
            t = tone_char2type(*p);
            if (t == -1)
                return -1;
            *type++ = t;
            count ++;
        }
    }
    return count;
}

VOID AT_PlayLocalTone(AUD_TONE_TYPE_T tone, UINT32 persistence, UINT32 silence, UINT8 vol)
{
    localtone[0].tone = tone;
    localtone[0].tone_term = persistence;
    localtone[1].tone = AUD_NO_TONE;
    localtone[1].tone_term = silence;

    DM_PlaySelfdefineTone(&localtone, DM_TONE_m3dB, persistence + silence, vol);
}

VOID qltone_set_next_tone(struct QLDTMF_INFO *di, UINT16 *duration)
{
    di->idx = 0; /* because qltone only have one tone, come here, idx have increase */
    struct dtmf_code *p = &(di->dtmf_code[0]);
    UINT16 left_time = *duration;

    if (left_time <= p->persistence)
    {
        p->persistence = left_time;
        p->silence = 0;
        left_time = 0;
    }
    else if (left_time <= (p->persistence + p->silence))
    {
        p->silence = left_time - p->persistence;
        left_time = 0;
    }
    else
    {
        left_time -= (p->persistence + p->silence);
    }
    *duration = left_time;
}

VOID AT_AUDIO_PlayToneContinue()
{
    if (ldtmf_info.idx == ldtmf_info.size)
    {
        if (at_tone_mode == AT_PLAY_TONE_CYCLE && (qltone_duration > 0))
        {
            qltone_set_next_tone(&ldtmf_info, &qltone_duration);
        }
        else
        {
            tone_stop();
            AT_TC(g_sw_SPEC, "QLDTMF# AT_AUDIO_PlayToneContinue stop");
            return;
        }
    }

    struct dtmf_code *p = &(ldtmf_info.dtmf_code[ldtmf_info.idx]);

    AT_TC(g_sw_SPEC, "QLDTMF# continue tone=%d,%d,%d", p->tone,p->persistence,p->silence);
    AT_PlayLocalTone(p->tone, p->persistence, p->silence, ldtmf_info.vol);
    at_StopCallbackTimer(AT_AUDIO_PlayToneContinue, NULL);
    at_StartCallbackTimer(p->persistence + p->silence, AT_AUDIO_PlayToneContinue, NULL);
    ldtmf_info.idx ++;
    return;
}

BOOL is_tone_character(UINT8 tone)
{
    if (((tone >= '0') && (tone <= '9')) ||
            (tone == 'A') ||
            (tone == 'B') ||
            (tone == 'C') ||
            (tone == 'D') ||
            (tone == '*') ||
            (tone == '#'))
        return TRUE;
    else
        return FALSE;
}

INT8 qwdtmf_getcodesize(UINT8 *dtmf_str)
{
    UINT8 codec_size = 0;
    UINT8 *p = dtmf_str;
    UINT8 *code_start = dtmf_str;
    UINT8 comma =  0;

    while (*p++ != NULL)
    {
        if (*p == ',')
            comma++;
    }

    /* check dtmf code string seg is right */
    if ((comma < 2) || ((comma-2) % 3))
        return -1;

    p = dtmf_str;
    while (*p++ != NULL)
    {
        while (*p != ',') p++;
        codec_size += (p - code_start);
        while (code_start != p)
        {
            if (is_tone_character(*code_start))
                code_start += 1;
            else
                return -1;
        }
        p++;

        /* skip tone persistence time string */
        while (*p++ != ',');
        /* skip tone silence time string */
        while (1)
        {
            if (*p == ',')
            {
                p++;
                code_start = p;
                break;
            }
            else if (*p == NULL)
            {
                break;
            }
            p++;
        }
    }

    return codec_size;
}

struct dtmf_code *qwdtmf_createtonearray(UINT8 *dtmf_str)
{
    INT8 new_code;
    UINT8 i, idx = 0;
    UINT32 persistence, silence = 0;
    UINT8 *p = NULL;
    UINT8 *code_start = dtmf_str;
    UINT8 *code_base = dtmf_str;
    struct dtmf_code *array = NULL;

    new_code = qwdtmf_getcodesize(dtmf_str);
    if (new_code < 0)
        return NULL;

    array = (struct dtmf_code *)AT_MALLOC(sizeof(struct dtmf_code) * new_code);
    if (array == NULL) return NULL;

    p = dtmf_str;
    while (*p++ != NULL)
    {
        while (*p != ',') p++;
        new_code = (p - code_start);
        p++;

        persistence = atoi(p);
        /* skip tone persistence time string */
        while (*p != ',') p++;
        p++;
        /* skip tone silence time string */
        silence = atoi(p);
        while (1)
        {
            if (*p == ',')
            {
                p++;
                code_base = code_start;
                code_start = p;
                break;
            }
            else if (*p == NULL)
            {
                code_base = code_start;
                break;
            }
            p++;
        }

        for ( i = 0; i < new_code; i++)
        {
            UINT8 code = *(code_base + i);
            if (code == 'E')
                array[idx].tone = 1400;
            else if (code == 'F')
                array[idx].tone = 2300;
            else if (code == 'G')
                array[idx].tone = 1000;
            else
                array[idx].tone = code;
            array[idx].persistence = persistence;
            array[idx].silence = silence;
            AT_TC(g_sw_SPEC, "QWDTMF# idx=%c, persistence=%d, silence=%d", array[idx].tone, persistence, silence);
            idx ++;
        }
    }

    return array;
}

VOID qwdtmf_callback()
{
    if (g_qwdtmf_info.idx < g_qwdtmf_info.size)
    {
        StartDtmfTone(g_qwdtmf_info.dtmf_array[g_qwdtmf_info.idx].tone, g_qwdtmf_info.dtmf_array[g_qwdtmf_info.idx].persistence,
                g_qwdtmf_info.dtmf_array[g_qwdtmf_info.idx].silence, g_qwdtmf_info.ul_volume,  8000, qwdtmf_callback);
    }
    else
    {
        AT_FREE(g_qwdtmf_info.dtmf_array);
        g_qwdtmf_info.dtmf_array = NULL;
        g_qwdtmf_info.idx = 0;
        g_qwdtmf_info.size = 0;
        return;
    }
    g_qwdtmf_info.idx++;
}

static VOID qwdtmf2ldtmf(struct qwdtmf_info *qw, struct QLDTMF_INFO *ldtmf_info)
{
    UINT8 i = 0;
    UINT8 tone_type[2] = {0, 0};

    ldtmf_info->dtmf_code = (struct dtmf_code *)AT_MALLOC(sizeof(struct dtmf_code) * qw->size);
    if (ldtmf_info->dtmf_code == NULL)
        return;
    for (; i < qw->size; i++)
    {
        ldtmf_info->dtmf_code[i].tone = tone_char2type(qw->dtmf_array[i].tone);
        ldtmf_info->dtmf_code[i].persistence = qw->dtmf_array[i].persistence;
        ldtmf_info->dtmf_code[i].silence = qw->dtmf_array[i].silence;
    }

    ldtmf_info->idx = 0;
    ldtmf_info->size = qw->size;
    ldtmf_info->vol = qw->dl_volume;
    ldtmf_info->timer = FALSE;
}

VOID AT_AUDIO_CmdFunc_QWDTMF(AT_CMD_PARA *pParam)
{
    UINT8 arrRes[70];
    INT32 iRet = ERR_SUCCESS;
    UINT8 paraCount = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 ul_volume = 0;
    UINT8 dl_volume = 0;
    UINT8 channel = 0;
    UINT8 mode = 0;
    UINT8 dtmfcode[100] = {0x00,};

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_MemZero(arrRes, sizeof(arrRes));
        AT_StrCpy(arrRes, "+QWDTMF:(0-7),(0-7),( , , ),(0),(0)");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes),pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if (iRet != ERR_SUCCESS || (paraCount < 3))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        UINT16 nParamLen = 1;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &ul_volume, &nParamLen);
        if ((iRet != ERR_SUCCESS) || (ul_volume > 7))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        nParamLen = 1;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &dl_volume, &nParamLen);
        if ((iRet != ERR_SUCCESS) || (dl_volume > 7))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        nParamLen = 100;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, &dtmfcode, &nParamLen);
        if (iRet != ERR_SUCCESS)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (paraCount > 3)
        {
            nParamLen = 1;
            iRet = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_UINT8, &channel, &nParamLen);
            if (iRet != ERR_SUCCESS)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }

        if (paraCount > 4)
        {
            nParamLen = 1;
            iRet = AT_Util_GetParaWithRule(pParam->pPara, 4, AT_UTIL_PARA_TYPE_UINT8, &mode, &nParamLen);
            if (iRet != ERR_SUCCESS)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }

        g_qwdtmf_info.size = qwdtmf_getcodesize(dtmfcode);
        if (g_qwdtmf_info.size < 0)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        g_qwdtmf_info.dtmf_array = qwdtmf_createtonearray(dtmfcode);
        if (g_qwdtmf_info.dtmf_array == NULL)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        g_qwdtmf_info.idx = 0;
        g_qwdtmf_info.dl_volume = dl_volume * 2;
        g_qwdtmf_info.ul_volume = ul_volume * 2;
        g_qwdtmf_info.size = qwdtmf_getcodesize(dtmfcode);

        qwdtmf2ldtmf(&g_qwdtmf_info, &ldtmf_info);
        AT_AUDIO_PlayToneContinue();

        StartDtmfTone(g_qwdtmf_info.dtmf_array[g_qwdtmf_info.idx].tone, g_qwdtmf_info.dtmf_array[g_qwdtmf_info.idx].persistence,
                g_qwdtmf_info.dtmf_array[g_qwdtmf_info.idx].silence, g_qwdtmf_info.ul_volume, 8000, qwdtmf_callback);
        g_qwdtmf_info.idx ++;

        AT_StrCpy(arrRes, "+QWDTMF: 5");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, strlen(arrRes),pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    return;
}

VOID AT_AUDIO_CmdFunc_QLDTMF(AT_CMD_PARA *pParam)
{
    UINT8 arrRes[40] = {0x00,};
    INT32 iRet = ERR_SUCCESS;
    UINT8 paraCount = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT16 persistence = 0;
    UINT8 dtmf_str[40] = {0x00,};
    UINT8 type_str[21] = {0x00,};

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_StrCpy(arrRes, "+QLDTMF:(1-1000),(0-9,A-D,*,#)");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes),pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if (iRet != ERR_SUCCESS || (paraCount == 0) || (paraCount > 2))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        UINT16 nParamLen = 2;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT16, &persistence, &nParamLen);
        if (iRet != ERR_SUCCESS || persistence == 0 || persistence > 1000)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (paraCount == 1)
        {
            return;
        }

        if (paraCount > 1)
        {
            nParamLen = sizeof(dtmf_str);
            if (AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, &dtmf_str, &nParamLen) != ERR_SUCCESS)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            iRet = AT_AUDIO_TONE_char2type(dtmf_str, type_str, nParamLen);
            if (iRet < 0)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            int i = 0;
            ldtmf_info.size = iRet;
            ldtmf_info.idx = 0;
            ldtmf_info.vol = gATCurrentVGR_Volume;
            ldtmf_info.dtmf_code = (struct dtmf_code *)AT_MALLOC(sizeof(struct dtmf_code) * ldtmf_info.size);
            if (ldtmf_info.dtmf_code == NULL)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            for (i = 0; i < ldtmf_info.size; i++)
            {
                ldtmf_info.dtmf_code[i].tone = type_str[i];
                ldtmf_info.dtmf_code[i].persistence = persistence * 100;
                ldtmf_info.dtmf_code[i].silence = persistence * 100;
            }

            AT_TC(g_sw_SPEC, "QLDTMF# str=%s, size=%d,persistence=%d", dtmf_str, ldtmf_info.size, persistence);
            AT_AUDIO_PlayToneContinue();
        }

        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_EXE)
    {
        ldtmf_info_reset(&ldtmf_info);
        DM_StopTone();
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI, nSim);
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    return;
}

VOID AT_AUDIO_CmdFunc_CMIC(AT_CMD_PARA *pParam)
{
    UINT8 arrRes[40] = {0x00,};
    INT32 iRet = ERR_SUCCESS;
    UINT8 paraCount = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 channel = 0;
    UINT8 level = 0;

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_Sprintf(arrRes, "+CMIC:(0-2),0-%d", SND_SPK_VOL_QTY-1);
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes),pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if (iRet != ERR_SUCCESS || paraCount != 2)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        UINT16 nParamLen = 1;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &channel, &nParamLen);
        if ((iRet != ERR_SUCCESS) || (channel > SND_ITF_LOUD_SPEAKER))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        nParamLen = 1;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &level, &nParamLen);
        if ((iRet != ERR_SUCCESS) || (level >= SND_SPK_VOL_QTY))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        AT_TC(g_sw_SPEC, "CMIC# channel=%d, level=%d", channel, level);
        aud_CodecCommonSetMicLevel(channel, AT_CC_GetCCCount(nSim), level);
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        level = aud_CodecCommonGetMicLevel();
        AT_Sprintf(arrRes, "%d,%d,%d", level, level, level);
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, strlen(arrRes), pParam->nDLCI, nSim);
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    return;
}

VOID AT_AUDIO_CmdFunc_QLTONE(AT_CMD_PARA *pParam)
{
    AUD_TONE_TYPE_T tone;
    UINT8 arrRes[70];
    INT32 iRet = ERR_SUCCESS;
    UINT8 paraCount = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 mode = 0;
    UINT16 freq = 0;
    UINT16 periodon = 0;
    UINT16 periodoff = 0;
    UINT16 duration = 0;

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_MemZero(arrRes, sizeof(arrRes));
        AT_StrCpy(arrRes, "+QLTONE:(0,1),(425,950,1400,1800),(1-25500),(0-25500),(1-25500)");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes),pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if (iRet != ERR_SUCCESS || (paraCount != 5) && (paraCount != 1))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        UINT16 nParamLen = 1;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &mode, &nParamLen);
        if ((iRet != ERR_SUCCESS) || ((mode != 0) && (mode != 1)))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (mode == 0)
        {
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI, nSim);
            tone_stop();
            return;
        }

        nParamLen = 2;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT16, &freq, &nParamLen);
        if ((iRet != ERR_SUCCESS) || ((freq < AUD_COMFORT_425) && (freq > AUD_COMFORT_1800)))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        nParamLen = 2;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT16, &periodon, &nParamLen);
        if (iRet != ERR_SUCCESS || periodon == 0 || periodon > 25500)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        nParamLen = 2;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_UINT16, &periodoff, &nParamLen);
        if (iRet != ERR_SUCCESS || periodoff > 25500)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        nParamLen = 2;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, 4, AT_UTIL_PARA_TYPE_UINT16, &duration, &nParamLen);
        if (iRet != ERR_SUCCESS || duration == 0 || duration > 25500 || duration < (periodon + periodoff))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        /* set period on time */
        switch (freq) {
            case 425:
                tone = AUD_COMFORT_425;
                break;
            case 950:
                tone = AUD_COMFORT_950;
                break;
            case 1400:
                tone = AUD_COMFORT_1400;
                break;
            case 1800:
                tone = AUD_COMFORT_1800;
                break;
            default:
                AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
        }
        AT_TC(g_sw_SPEC, "tone period on=%d", periodon);
        AT_TC(g_sw_SPEC, "tone period off=%d", periodoff);
        AT_TC(g_sw_SPEC, "tone duration=%d", duration);
        AT_TC(g_sw_SPEC, "tone volume=%d", gATCurrentVGR_Volume);


        ldtmf_info.dtmf_code = (struct dtmf_code *)AT_MALLOC(sizeof(struct dtmf_code) * 1);
        if (ldtmf_info.dtmf_code == NULL)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        qltone_duration = duration - periodon - periodoff;

        ldtmf_info.size = 1;
        ldtmf_info.idx = 0;
        ldtmf_info.vol = gATCurrentVGR_Volume;
        ldtmf_info.dtmf_code[0].tone = tone;
        ldtmf_info.dtmf_code[0].persistence = periodon;
        ldtmf_info.dtmf_code[0].silence = periodoff;
        at_tone_mode = AT_PLAY_TONE_CYCLE;
        AT_AUDIO_PlayToneContinue();
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0,pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    return;
}

VOID AT_AUDIO_CmdFunc_DTMF(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount = 0;
    INT32 iResult = 0;
    UINT8 tmpString[60] = {0x00,};
    UINT16 uSize = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 dtmf_switch = 0;
    CSW_SYS_FREQ_T curr_sys_freq = 0;

    if (NULL == pParam)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    switch (pParam->iType)
    {
        case AT_CMD_SET:
            {
                iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
                if ((iResult != ERR_SUCCESS) || (uParamCount != 1))
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                uSize = 1;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &dtmf_switch, &uSize);
                if ((iResult != ERR_SUCCESS)||((dtmf_switch != 0) && (dtmf_switch != 1)))
                {
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }

                if (dtmf_switch == 1)
                {
                    DTMF_DetectStart(DTMF_CallBack);
                }
                else
                {
                    DTMF_DetectFinish();
                }
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
            }
            break;
        case AT_CMD_READ:
            AT_Sprintf(tmpString, "DTMF:%d", IsDTMF_Detect_Start());
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, tmpString, AT_StrLen(tmpString), pParam->nDLCI, nSim);
            break;
        default:
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
    }
}


