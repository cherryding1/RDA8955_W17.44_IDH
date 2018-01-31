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


#include "syscmdsp_hdlr.h"
#include "syscmdsp_debug.h"
#include "syscmds_aud_cmd_id.h"

#include "string.h"

#include "hal_host.h"
#include "aud_m.h"
#include "vois_m.h"
#include "calib_m.h"

#include "pmd_m.h"

// =============================================================================
// MACROS
// =============================================================================

// Tell audio driver to skip the AIF configuration
#define PRE_SYSCMDS_AUD_OPER(userId) \
    { \
        userId = aud_MutexTake(); \
        aud_CodecCommonSetAifConfigByAp(TRUE); \
    }

// Restore the default audio driver behaviour
#define POST_SYSCMDS_AUD_OPER(userId) \
    { \
        aud_CodecCommonSetAifConfigByAp(FALSE); \
        aud_MutexRelease(userId); \
    }

// FIXME,this should be same with ap.
#define PCM_SHARE_BUFFER_SIZE       (640 * 10)

#define MP3_BUFFER_ALIGNMENT        (4 * 1024)

#define MP3_BUFFER_SIZE             (256 * 1024)


// =============================================================================
// TYPES
// =============================================================================

typedef enum
{
    SYSCMDS_AUDIO_APP_CODEC = 0,
    SYSCMDS_AUDIO_APP_FM,
    SYSCMDS_AUDIO_APP_ATV,

    SYSCMDS_AUDIO_APP_QTY,
} SYSCMDS_AUDIO_APP_T;


// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

PRIVATE SYSCMDS_AUDIO_APP_T g_syscmdsAppMode = SYSCMDS_AUDIO_APP_CODEC;

PRIVATE INT32 vois_record_buffer[PCM_SHARE_BUFFER_SIZE / 4];

PRIVATE UINT8 ALIGNED(MP3_BUFFER_ALIGNMENT) g_mp3Buffer[MP3_BUFFER_SIZE];
#ifdef FM_LININ_AUXMIC
extern BOOL g_halApCommCapModeVersion ;
#endif

// =============================================================================
//  FUNCTIONS
// =============================================================================

PRIVATE SND_ITF_T syscmds_ConvertAudioItf(SND_ITF_T itf)
{
    SYSCMDS_ASSERT(
        (itf == SND_ITF_RECEIVER ||
         itf == SND_ITF_EAR_PIECE ||
         itf == SND_ITF_LOUD_SPEAKER||
         itf == SND_ITF_CLOSE ),
        "Invalid autio itf from AP: %d", itf);

    if (g_syscmdsAppMode == SYSCMDS_AUDIO_APP_FM)
    {
        aud_SetCurOutputDevice(itf);
        itf = SND_ITF_FM;
    }
    else if (g_syscmdsAppMode == SYSCMDS_AUDIO_APP_ATV)
    {
        aud_SetCurOutputDevice(itf);
        itf = SND_ITF_TV;
    }

#ifdef FM_LININ_AUXMIC
    aud_SetCurOutputDevice(itf);
#endif

    return itf;
}


PUBLIC VOID syscmds_SetAudioCalibEQ(void *param, UINT32 size)
{

    syscmds_IrqSend(SYSCMDS_MOD_ID_AUDIO,     AP_AUDIO_CMD_SET_AUDIO_CALIB_EQ,param, size);
}

PUBLIC VOID syscmds_SetAudioCalibALC(void *param, UINT32 size)
{

    syscmds_IrqSend(SYSCMDS_MOD_ID_AUDIO,   AP_AUDIO_CMD_SET_AUDIO_CALIB_ALC,param, size);
}

PRIVATE VOID vois_record_handler(VOIS_STATUS_T status)
{
    syscmds_IrqSend(SYSCMDS_MOD_ID_AUDIO, MODEM_AUDIO_CMD_VOIS_HANDLER_CALLBACK, &status, sizeof(status));
}

#ifdef FM_LININ_AUXMIC
BOOL g_syscmds_FM_Playing = FALSE;
#endif

PROTECTED BOOL syscmds_HandleAudioCmd(SYSCMDS_RECV_MSG_T *pRecvMsg)
{
    BOOL result = TRUE;
    UINT32 ret;
    UINT8 userId;

    SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,
                  "syscmds_HandleAudioCmd: pRecvMsg->msgId=0x%x", pRecvMsg->msgId);

    switch (pRecvMsg->msgId)
    {
        case AP_AUDIO_CMD_SETUP:
        {
            SND_ITF_T itf;
            AUD_LEVEL_T cfg;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, sizeof(itf) + sizeof(cfg));

            memcpy(&itf, pRecvMsg->param, sizeof(itf));
            memcpy(&cfg, (UINT8 *)pRecvMsg->param + sizeof(itf), sizeof(cfg));


            SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,  "syscmds:setup Convert Audio itf=%d  \n", itf);

            itf = syscmds_ConvertAudioItf(itf);

            SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,  "syscmds:setup itf=%d g_syscmdsAppMode= %d\n", itf, g_syscmdsAppMode);

            PRE_SYSCMDS_AUD_OPER(userId);

#ifdef FM_LININ_AUXMIC
            if(g_syscmds_FM_Playing && (itf==SND_ITF_FM))
            {
                cfg.micLevel =  AUD_MIC_VOL_FM;
            }
            else if(g_syscmds_FM_Playing )
            {
                itf=SND_ITF_FM;
            }
#endif

            ret = aud_Setup(itf, &cfg);
            POST_SYSCMDS_AUD_OPER(userId);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, ret, pRecvMsg->msgId, NULL, 0);
        }
        break;

        case AP_AUDIO_CMD_STREAM_START:
        case AP_AUDIO_CMD_STREAM_RECORD:
        {
            SND_ITF_T itf;
            HAL_AIF_STREAM_T stream;
            AUD_LEVEL_T cfg;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, sizeof(itf) + sizeof(stream) + sizeof(cfg));

            memcpy(&itf, pRecvMsg->param, sizeof(itf));
            memcpy(&stream, (UINT8 *)pRecvMsg->param + sizeof(itf), sizeof(stream));
            memcpy(&cfg, (UINT8 *)pRecvMsg->param + sizeof(itf) + sizeof(stream), sizeof(cfg));
            SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,  "syscmds:start  Convert Audio itf=%d %d \n", itf,pRecvMsg->msgId);
            itf = syscmds_ConvertAudioItf(itf);
            SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,  "syscmds:start  itf=%d %d \n", itf,pRecvMsg->msgId);


#ifdef AUD_3_IN_1_SPK
#if BUZZY_TEST
            aud_StopBuzzy();
#endif
#endif // AUD_3_IN_1_SPK

            PRE_SYSCMDS_AUD_OPER(userId);
            if (pRecvMsg->msgId == AP_AUDIO_CMD_STREAM_START)
            {

#ifdef FM_LININ_AUXMIC
                if(itf == SND_ITF_FM)
                {
                    ret = aud_FMStreamStart(itf, &cfg);
                    g_syscmds_FM_Playing = TRUE;
                }
                else
#endif
                {
                    ret = aud_StreamStart(itf, &stream, &cfg);
                }
            }
            else
            {
#ifdef FM_LININ_AUXMIC
                if(itf == SND_ITF_FM)
                    ret = AUD_ERR_NO;
                else
#endif
                    ret = aud_StreamRecord(itf, &stream, &cfg);
            }
            POST_SYSCMDS_AUD_OPER(userId);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, ret, pRecvMsg->msgId, NULL, 0);
        }
        break;

        case AP_AUDIO_CMD_STREAM_PAUSE:
        {
        }
        break;

        case AP_AUDIO_CMD_STREAM_STOP:
        {
            SND_ITF_T itf;
            ret = AUD_ERR_NO;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, sizeof(itf));

            memcpy(&itf, pRecvMsg->param, sizeof(itf));
            itf = syscmds_ConvertAudioItf(itf);

            SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,  "syscmds:stop  itf=%d %d \n", itf,pRecvMsg->msgId);

            PRE_SYSCMDS_AUD_OPER(userId);
            if(itf == SND_ITF_CLOSE)
            {
#ifdef FM_LININ_AUXMIC
                if(g_syscmds_FM_Playing)
                {
                    ret = aud_FMStreamStop(SND_ITF_FM);
                    g_syscmds_FM_Playing = FALSE;
                }
                else
#endif
                    ret = aud_StreamStopBusy();
            }
#ifdef FM_LININ_AUXMIC
            else if((itf == SND_ITF_FM) || g_syscmds_FM_Playing)
            {
                if(g_syscmds_FM_Playing)
                {
                    ret = aud_FMStreamStop(SND_ITF_FM);
                }

                g_syscmds_FM_Playing = FALSE;
            }
#endif
            else
                ret = aud_StreamStop(itf);

            POST_SYSCMDS_AUD_OPER(userId);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, ret, pRecvMsg->msgId, NULL, 0);
        }
        break;

        case AP_AUDIO_CMD_TONE_START:
            break;
        case AP_AUDIO_CMD_TONE_STOP:
            break;
        case AP_AUDIO_CMD_TONE_PAUSE:
            break;

        case AP_AUDIO_CMD_TEST_MODE:
        {
            SND_ITF_T itf;
            HAL_AIF_STREAM_T stream;
            AUD_LEVEL_T cfg;
            AUD_TEST_T mode;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, sizeof(itf) + sizeof(stream) + sizeof(cfg) + sizeof(mode));

            memcpy(&itf, pRecvMsg->param, sizeof(itf));
            memcpy(&stream, (UINT8 *)pRecvMsg->param + sizeof(itf), sizeof(stream));
            memcpy(&cfg, (UINT8 *)pRecvMsg->param + sizeof(itf) + sizeof(stream), sizeof(cfg));
            memcpy(&mode, (UINT8 *)pRecvMsg->param + sizeof(itf) + sizeof(stream) + sizeof(cfg), sizeof(mode));

            itf = syscmds_ConvertAudioItf(itf);

            // must not sync when in test mode
            //PRE_SYSCMDS_AUD_OPER(userId);
            ret = aud_TestModeSetup(itf, &stream, &cfg, mode);
            //POST_SYSCMDS_AUD_OPER(userId);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, ret, pRecvMsg->msgId, NULL, 0);
        }
        break;

        case AP_AUDIO_CMD_FORCE_MIC_SEL:
        {
            UINT32 on;
            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&on, pRecvMsg->param, 4);
            SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,  "syscmds:mic sel  on=%d \n",on);

            PRE_SYSCMDS_AUD_OPER(userId);
            aud_ForceReceiverMicSelection((BOOL)on);
            POST_SYSCMDS_AUD_OPER(userId);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, NULL, 0);
        }
        break;

        case AP_AUDIO_CMD_SPK_WITH_EP:
        {
            UINT32 on;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&on, pRecvMsg->param, 4);
            SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,  "syscmds:SPK WITH EP  on=%d \n",on);

            PRE_SYSCMDS_AUD_OPER(userId);
            aud_LoudspeakerWithEarpiece((BOOL)on);
            POST_SYSCMDS_AUD_OPER(userId);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, NULL, 0);
        }
        break;

        case AP_AUDIO_CMD_MUTE_OUTPUT:
            break;
        case AP_AUDIO_CMD_BYPASS_NOTCH:
            break;

        case AP_AUDIO_CMD_SET_APP_MODE:
        {
            UINT32 mode;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&mode, pRecvMsg->param, 4);

            SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,  "syscmds:set mode  g_syscmdsAppMode=%d\n",mode);
            SYSCMDS_ASSERT(mode < SYSCMDS_AUDIO_APP_QTY,
                           "Invalid audio app mode from AP: %d", mode);

            g_syscmdsAppMode = mode;

#ifdef FM_LININ_AUXMIC
            if(g_syscmdsAppMode != SYSCMDS_AUDIO_APP_FM)
            {
                if(g_halApCommCapModeVersion)
                    pmd_SetLevel(PMD_LEVEL_VIBRATOR, 1);

            }

#endif

            SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,
                          "syscmds_HandleAudioCmd: g_syscmdsAppMode=0%d", g_syscmdsAppMode);


            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, NULL, 0);
        }
        break;

        case AP_AUDIO_CMD_VOIS_RECORD_START:
        {
            UINT32 addr = (UINT32)&vois_record_buffer[0];

            ret = vois_RecordStart(vois_record_buffer, PCM_SHARE_BUFFER_SIZE, vois_record_handler);
            SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,  "syscmds:voice record start \n");
            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, ret,
                          pRecvMsg->msgId, &addr, sizeof(addr));
        }
        break;

        case AP_AUDIO_CMD_VOIS_RECORD_STOP:
        {
            vois_RecordStop();
            SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,  "syscmds:voice record stop \n");
            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, NULL, 0);
        }
        break;

        case AP_AUDIO_CMD_GET_AUDIO_CALIB:
        {
            UINT32 value[2];
            CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
            CALIB_AUDIO_ITF_T *audioCalib = &calibPtr->bb->audio[0];

            value[0] = (UINT32)audioCalib;
            value[1] = sizeof(calibPtr->bb->audio);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, &value[0], sizeof(value));
        }
        break;

        case AP_AUDIO_CMD_GET_AUDIO_IIR_CALIB:
        {
            UINT32 value[2];
            CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
            CALIB_AUDIO_IIR_PARAM_T *audioIIRCalib = calibPtr->iirParam;

            value[0] = (UINT32)audioIIRCalib;
            value[1] = 240;

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, &value[0], sizeof(value));
        }
        break;

        case  AP_AUDIO_CMD_GET_AUDIO_DRC_CALIB:
        {
            UINT32 value[2];
            SND_ITF_T itf;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, sizeof(itf) );

            memcpy(&itf, pRecvMsg->param, sizeof(itf));

            CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
            CALIB_AUDIO_MUSIC_DRC_T *audiodrcCalib =   &(calibPtr->audio_music->music[itf].drc);
            value[0] = (UINT32)audiodrcCalib;
            value[1] = sizeof(CALIB_AUDIO_MUSIC_DRC_T);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, &value[0], sizeof(value));
        }
        break;
        case  AP_AUDIO_CMD_GET_AUDIO_EQ_CALIB:
        {
            UINT32 value[2];
            SND_ITF_T itf;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, sizeof(itf) );

            memcpy(&itf, pRecvMsg->param, sizeof(itf));

            CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
            CALIB_AUDIO_IIR_EQ_T *audioEQCalib =  &(  calibPtr->audio_music->music[itf].eq);

            value[0] = (UINT32)audioEQCalib;
            value[1] = sizeof(CALIB_AUDIO_IIR_EQ_T);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, &value[0], sizeof(value));
        }
        break;

        case  AP_AUDIO_CMD_AUD_GET_INGAINS_RECORD_CALIB:
        {
            UINT32 value[2];
            SND_ITF_T itf;
            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, sizeof(itf) );

            memcpy(&itf, pRecvMsg->param, sizeof(itf));

            CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
            CALIB_AUDIO_IN_GAINS_T *ingainsRecordCalib =   &(calibPtr->bb->audio[itf].audioGains.inGainsRecord);

            value[0] = (UINT32)ingainsRecordCalib;
            value[1] = sizeof(CALIB_AUDIO_IN_GAINS_T);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, &value[0], sizeof(value));
        }
        break;

        case AP_AUDIO_CMD_GET_HOST_AUDIO_CALIB:
        {
            UINT32 value[2];
            CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
            CALIB_AUDIO_ITF_T *audioCalib =
                (CALIB_AUDIO_ITF_T *)&calibPtr->hstBb->audio[0];

            value[0] = (UINT32)audioCalib;
            value[1] = sizeof(calibPtr->hstBb->audio);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, &value[0], sizeof(value));
        }
        break;

        case AP_AUDIO_CMD_GET_MP3_BUFFER:
        {
            UINT32 value[2];
            value[0] = (UINT32)&g_mp3Buffer;
            value[1] = sizeof(g_mp3Buffer);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, &value[0], sizeof(value));
        }
        break;

        default:
            result = FALSE;
            break;
    }


    return result;
}


