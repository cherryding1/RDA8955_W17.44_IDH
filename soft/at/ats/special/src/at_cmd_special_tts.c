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
#include <drv_prv.h>
#include "at_module.h"
#include "at_cmd_special_wifi.h"
#include "at_cfg.h"

#ifdef __TTS_FEATURES__

extern UINT32 ejTTS_PlayToPCM(char *pText, size_t nSize, BOOL UCS2);
extern int ejTTSPlayerStop(void);
extern BOOL IsTTSRun();
extern int g_TTS_Mix;
extern int g_TTS_MixState;
u16_t htons(u16_t n) { return ((n & 0xff) << 8) | ((n & 0xff00) >> 8); }

#define ERROR_RETURN(pResult, reason, nDLCI)           \
    do                                                 \
    {                                                  \
        pResult = AT_CreateRC(CMD_FUNC_FAIL,           \
                              CMD_RC_ERROR,            \
                              reason,                  \
                              CMD_ERROR_CODE_TYPE_CME, \
                              0,                       \
                              NULL,                    \
                              0,                       \
                              nDLCI);                  \
        AT_Notify2ATM(pResult, nDLCI);                 \
        if (pResult != NULL)                           \
        {                                              \
            AT_FREE(pResult);                          \
            pResult = NULL;                            \
        }                                              \
        return;                                        \
    } while (0)
#define OK_RETURN(pResult, pString, nDLCI)             \
    do                                                 \
    {                                                  \
        pResult = AT_CreateRC(CMD_FUNC_SUCC,           \
                              CMD_RC_OK,               \
                              CMD_ERROR_CODE_OK,       \
                              CMD_ERROR_CODE_TYPE_CME, \
                              0,                       \
                              pString,                 \
                              strlen(pString),         \
                              nDLCI);                  \
        AT_Notify2ATM(pResult, nDLCI);                 \
        if (pResult != NULL)                           \
        {                                              \
            AT_FREE(pResult);                          \
            pResult = NULL;                            \
        }                                              \
        return;                                        \
    } while (0)

VOID AT_TTS_Result_OK(UINT32 uReturnValue,
                      UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize, UINT8 nDLCI)

{
    PAT_CMD_RESULT pResult = NULL;

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

VOID AT_TTS_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI)
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
#if 0
int GBK_to_unicode_one(unsigned short gbk,
    unsigned long *ucs)
{
    assert(ucs != NULL)

    unsigned char *p = (unsigned char *) &gbk;
    unsigned char *phibyte = p+1;

    if(*phibyte < 0x80)
     {
        *ucs = *phibyte;
        return 1;      
    }
    else
    {
        if( gbk < tab_GBK_to_UCS2[0][0] || 
            gbk > tab_GBK_to_UCS2[NUMOF_TAB_GBK_UCS2 -1][0])
            {
            return 0;
            }
        *ucs = tab_GBK_to_UCS2[gbk - tab_GBK_to_UCS2[0][0]][1];
    }
    return 2;
    
}
#endif
VOID AT_EMOD_CmdFunc_DTAM(AT_CMD_PARA *pParam)
{

    INT32 iRet = ERR_SUCCESS;
    PAT_CMD_RESULT pResult;
    UINT8 nOutputStr[16] = {
        0,
    };
    if (NULL == pParam)
    {
        goto DTAM_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        if (NULL == Param->pPara)
            goto DTAM_ERROR;

        switch (Param->iType)
        {
        case AT_CMD_TEST:
            SUL_StrCat(nOutputStr, "+DTAM: (0-2)");
            AT_TTS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI);
            return;
        case AT_CMD_READ:
            SUL_StrPrint(nOutputStr, "+DTAM: %d", g_TTS_MixState);
            AT_TTS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI);
            return;
        case AT_CMD_SET:
        {
            UINT8 uParaCount;
            UINT8 mode;
            UINT32 nRet = ERR_SUCCESS;
            UINT16 uSize;
            UINT8 i = 0;

            iRet = AT_Util_GetParaCount(pParam->pPara, &uParaCount);

            if (ERR_SUCCESS != iRet)
                goto DTAM_ERROR;

            if (uParaCount < 1)
                goto DTAM_ERROR;

            uSize = 1;
            AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &mode, &uSize);
            if (1 != uParaCount)
            {
                goto DTAM_ERROR;
            }
            if (mode <= 2)
                g_TTS_MixState = mode;
            else
            {
                goto DTAM_ERROR;
            }
            if (ERR_SUCCESS == nRet)
                AT_TTS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 30, NULL, 0, pParam->nDLCI);
            else
                AT_TTS_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        default:
            break;
        }
    }

DTAM_ERROR:
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
//========================================================
//VOID AT_EMOD_CmdFunc_CTTS(AT_CMD_PARA* pParam)
//Get ip address,without parameter
//========================================================
VOID AT_EMOD_CmdFunc_CTTS(AT_CMD_PARA *pParam)
{
    INT32 iRet = ERR_SUCCESS;
    PAT_CMD_RESULT pResult;
    UINT8 nOutputStr[16] = {
        0,
    };
    if (NULL == pParam)
    {
        goto TTS_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        if (NULL == Param->pPara)
            goto TTS_ERROR;

        switch (Param->iType)
        {
        case AT_CMD_TEST:
            SUL_StrCat(nOutputStr, "+CTTS: (0-2)");
            AT_TTS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI);
            return;
        case AT_CMD_READ:
            SUL_StrPrint(nOutputStr, "+CTTS: %d", IsTTSRun());
            AT_TTS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI);
            return;
        case AT_CMD_SET:
        {
            UINT8 uParaCount;
            UINT8 mode;
            UINT32 nRet = ERR_SUCCESS;
            UINT8 tts_text[512] = {
                0,
            };
            UINT8 tts_unicode[512] = {
                0,
            };
            UINT16 *pUnicode = (UINT16 *)tts_unicode;
            UINT16 uSize;
            UINT8 i = 0;
            UINT8 Disp[100];

            UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

            iRet = AT_Util_GetParaCount(pParam->pPara, &uParaCount);

            if (ERR_SUCCESS != iRet)
                goto TTS_ERROR;

            if (uParaCount < 1)
                goto TTS_ERROR;

            uSize = 1;
            AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &mode, &uSize);
            if (2 == uParaCount)
            {
                uSize = SIZEOF(tts_text);
                AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, tts_text, &uSize);
            }

            UINT32 uStat = CFW_CcGetCallStatus(nSim);
            if (uStat != 0x00) //CC_STATE_NULL
                g_TTS_Mix = 1;
            else
                g_TTS_Mix = 0;

            if (mode == 0)
            {
                ejTTSPlayerStop();
            }
            else if (mode == 1) // UNICODE
            {
                /*
                    if(SUL_ascii2hex( tts_text, tts_unicode) == 0)
                    {
                        goto TTS_ERROR;
                    }
                    */
                for (i = 0; i < uSize; i += 2)
                {
                    tts_unicode[i] = tts_text[i + 1];
                    tts_unicode[i + 1] = tts_text[i];
                    //  *(pUnicode)=htons((UINT16 *)(tts_text));
                    //  pUnicode++;
                    // (tts_unicode[j]) = ((UINT16)tts_text[i+1]<<8)  &&  0xff00 | (tts_text[i]&& 0xff);
                    //pUnicode++;
                    //AT_Sprintf(Disp,"tts_unicode[%d]=0x%x\r\n",i,tts_unicode[i]);
                    //AT_SendString(Disp);
                    //AT_Sprintf(Disp,"tts_unicode[%d]=0x%x\r\n",i+1,tts_unicode[i+1]);
                    //AT_SendString(Disp);
                }
                nRet = ejTTS_Init();
                nRet = AT_TTS_Play(tts_unicode, uSize, 1);
            }
            else if (mode == 2) // ASCII GBK
            {
#ifdef AT_TTS_MIX
                UINT32 outlen = 0;
                char *tts_ucs2 = NULL;

                ML_LocalLanguage2UnicodeBigEnding(tts_text, uSize, &tts_ucs2, &outlen, ML_CP936);

                nRet = ejTTS_Init();
                nRet = AT_TTS_Play(tts_ucs2, outlen, 1);

                if (NULL != tts_ucs2)
                {
                    COS_FREE(tts_ucs2);
                }
#else
                nRet = AT_TTS_Play(tts_text, uSize, 0);
#endif
            }
            else
            {
                goto TTS_ERROR;
            }
            if (ERR_SUCCESS == nRet)
                AT_TTS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 30, NULL, 0, pParam->nDLCI);
            else
                AT_TTS_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        default:
            break;
        }
    }

TTS_ERROR:
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
