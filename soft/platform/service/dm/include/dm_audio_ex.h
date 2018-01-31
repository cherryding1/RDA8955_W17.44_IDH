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

#if !defined(__DM_AUDIO_EX_H__)
#define __DM_AUDIO_EX_H__

#include "cos.h"
#include "dm_audio.h"
#include "dm_vois.h"
#include "dm_ars.h"

// Special handling for EV_DM_AUDIO_CONTROL_IND:
// CFW does NOT expect the return value. Instead, it appreciates
// the execution efficiency much. So the audio synchronization
// is bypassed here.
// TODO:
// CFW will NOT request any audio resource, and MMI will
// do that for it.
#define CFW_AUDIO_CONTROL_BYPASS_SYNC 1

typedef struct
{
    CONST HAL_AIF_STREAM_T *stream;
    CONST AUD_LEVEL_T       *cfg;
} DM_AUD_STREAM_START_PARAM_T;

typedef DM_AUD_STREAM_START_PARAM_T DM_AUD_STREAM_RECORD_PARAM_T;

typedef struct
{
    CONST HAL_AIF_STREAM_T *stream;
    CONST AUD_LEVEL_T     *cfg;
    AUD_TEST_T              mode;
} DM_AUD_TEST_MODE_SETUP_PARAM_T;

typedef struct
{
    INT32 *startAddress;
    INT32 length;
    VOIS_USER_HANDLER_T handler;
} DM_VOIS_RECORD_START_PARAM_T;

typedef struct
{
    CONST ARS_ENC_STREAM_T *stream;
    CONST ARS_AUDIO_CFG_T *cfg;
    BOOL loop;
} DM_ARS_RECORD_PARAM_T;


VOID DM_SendAudOperResult(UINT32 slot, UINT32 result);

VOID DM_ToneTimerHandlerEx(COS_EVENT *pEvent);
VOID DM_ToneMsgHandlerEx(COS_EVENT *pEvent);

BOOL DM_StartAudioEX();
BOOL DM_StopAudioEX();

BOOL DM_SetAudioVolumeEX (DM_SpeakerGain nVolume);
BOOL DM_SetMicGainEX (DM_MicGain nGain);
BOOL DM_SetAudioModeEX(UINT8 nAudioMode);

BOOL DM_AudSetupEx(CONST AUD_LEVEL_T *cfg);
BOOL DM_AudStreamStartEx(CONST DM_AUD_STREAM_START_PARAM_T *param);
BOOL DM_AudStreamStopEx(VOID);
BOOL DM_AudStreamPauseEx(BOOL pause);
BOOL DM_AudStreamRecordEx(CONST DM_AUD_STREAM_RECORD_PARAM_T *param);
BOOL DM_AudTestModeSetupEx(CONST DM_AUD_TEST_MODE_SETUP_PARAM_T *param);
BOOL DM_AudForceReceiverMicSelectionEx(BOOL on);
BOOL DM_AudSetCurOutputDeviceEx(SND_ITF_T itf);
BOOL DM_AudMuteOutputDeviceEx(BOOL mute);
BOOL DM_AudLoudspeakerWithEarpieceEx(BOOL on);
#ifdef CHIP_HAS_AP
BOOL DM_AudSetBtNRECFlagEx(BOOL flag);
#endif

BOOL DM_VoisRecordStartEx(CONST DM_VOIS_RECORD_START_PARAM_T *param);
BOOL DM_VoisRecordStopEx(VOID);

BOOL DM_ArsSetupEx(CONST ARS_AUDIO_CFG_T *cfg);
BOOL DM_ArsRecordEx(CONST DM_ARS_RECORD_PARAM_T *param);
BOOL DM_ArsPauseEx(BOOL pause);
BOOL DM_ArsStopEx(VOID);

#endif // __DM_AUDIO_EX_H__
