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




#include "avpsp_cfg.h"
#include "avpsp.h"

#include "vpp_speech.h"
#include "avpsp_decode_speech.h"

#include "sxr_ops.h"
#include "sxs_io.h"

#include "string.h"

#include "hal_voc.h"

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// PRIVATE VARIABLES
// =============================================================================

PRIVATE BOOL                   g_avpsAmrRing;

/// VPP configuration used to play the stream in normal state.
/// (ie not when started or stopped).
PRIVATE VPP_SPEECH_AUDIO_CFG_T g_avpsVppSpeechCfg;

// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

PRIVATE MPS_DECODE_TEST_RESULT_T avps_decodeSpeechTest(CONST AVPS_ENC_STREAM_T* stream);

PRIVATE VOID avps_decodeSpeechStop(VOID);

PRIVATE AVPS_ERR_T avps_decodeSpeechPlay(CONST AVPS_ENC_STREAM_T*       stream,
        CONST AVPS_CFG_T*              cfg,
        CONST LCDD_FBW_T*              frameWindows,
        HAL_AIF_STREAM_T*              pcmStream);

PRIVATE AVPS_ERR_T avps_decodeSpeechSetup(CONST AVPS_CFG_T*              cfg);

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PROTECTED AVPS_DECODE_T g_avpsSpeechDecode =
{
    .test  = avps_decodeSpeechTest,
    .play  = avps_decodeSpeechPlay,
    .stop  = avps_decodeSpeechStop,
    .setup = avps_decodeSpeechSetup
};

// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// avps_VppSpeechHandler
// -----------------------------------------------------------------------------
/// Handler for the vpp module, for speech-kind encoded streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PRIVATE VOID avps_VppSpeechHandler(HAL_VOC_IRQ_STATUS_T* status)
{
    AVPS_PROFILE_FUNCTION_ENTER(avps_Handler);
    // Number of bytes to copy;
    UINT32 cpSz;

    // Position in the decoding structure where to store
    // the data from the stream.
    UINT32* decInBufPos;

    BOOL    endBufferReached  = FALSE;
    BOOL    halfBufferReached = FALSE;
    HAL_SPEECH_DEC_IN_T*    decStruct;

    Msg_t   msg;

    /// Special operation according to state
    ///
    /// if (sampleNumber == ...)
    /// ...
    /// ...


    if (g_avpsFramesPlayedNb == AVPS_AUD_UNMUTE_TRAME_NB)
    {
        // Unmute AUD
        AVPS_TRACE(_PAL| TLEVEL (1), 0, "AUD unmuted");
        aud_Setup(g_avpsItf, &g_avpsAudioCfg);
    }

    if (g_avpsFramesPlayedNb == AVPS_VPP_UNMUTE_TRAME_NB)
    {
        // Unmute VPP
        AVPS_TRACE(_PAL| TLEVEL (1), 0, "VPP unmuted");
        vpp_SpeechAudioCfg(&g_avpsVppSpeechCfg);
    }

    decStruct = vpp_SpeechGetRxCodBuffer();

    if(g_avpsAmrRing == TRUE)
    {
        g_avpsFrameSize = 1;
        if(g_avpsAudioBufferRemainingBytes >= 1)
        {
            switch((*((UINT8*)g_avpsAudioBufferCurPos) >> 3) & 0xF)
            {
                case 0:
                    g_avpsFrameSize      = 13;
                    decStruct->codecMode = HAL_AMR475_DEC;
                    break;
                case 1:
                    g_avpsFrameSize      = 14;
                    decStruct->codecMode = HAL_AMR515_DEC;
                    break;
                case 2:
                    g_avpsFrameSize      = 16;
                    decStruct->codecMode = HAL_AMR59_DEC;
                    break;
                case 3:
                    g_avpsFrameSize      = 18;
                    decStruct->codecMode = HAL_AMR67_DEC;
                    break;
                case 4:
                    g_avpsFrameSize      = 20;
                    decStruct->codecMode = HAL_AMR74_DEC;
                    break;
                case 5:
                    g_avpsFrameSize      = 21;
                    decStruct->codecMode = HAL_AMR795_DEC;
                    break;
                case 6:
                    g_avpsFrameSize      = 27;
                    decStruct->codecMode = HAL_AMR102_DEC;
                    break;
                case 7:
                    g_avpsFrameSize      = 32;
                    decStruct->codecMode = HAL_AMR122_DEC;
                    break;
                default:
                    break;
            }
        }
    }

    if (g_avpsAudioBufferRemainingBytes < g_avpsFrameSize)
    {
        if (g_avpsAudioBufferingMsgReceived)
            // Data underflow ...
            // Stops here
        {
            // We need and ask for refill
            AVPS_TRACE(TSTDOUT, 0,"AVPS famine\n");
            AUD_LEVEL_T audioCfg          = g_avpsAudioCfg;
            VPP_SPEECH_AUDIO_CFG_T vppCfg = g_avpsVppSpeechCfg;

            msg.H.Id        = AVPS_HEADER_ID_AUDIO;
            msg.B.bufferPos = g_avpsAudioBufferCurPos;
            g_avpsAudioBufferingMsgReceived = FALSE;
            sxr_Send(&msg, g_avpsUserMbx, SXR_SEND_EVT);
            /// Mute ...
            audioCfg.spkLevel = 0;
            vppCfg.decMute    = VPP_SPEECH_MUTE;
            aud_Setup(g_avpsItf, &audioCfg);
            vpp_SpeechAudioCfg(&vppCfg);

            if(g_avpsFramesPlayedNb != 0)
            {
                avps_Stop(0);
            }
            /// Set state as for starting again from there ?
            g_avpsFramesPlayedNb = 0;
        }

    }
    else
    {
        UINT32 statusSection = hal_SysEnterCriticalSection();
        g_avpsAudioBufferRemainingBytes -= g_avpsFrameSize;
        hal_SysExitCriticalSection(statusSection);

        decInBufPos = (UINT32*)decStruct->decInBuf;

        if (((UINT32)g_avpsAudioBufferCurPos + g_avpsFrameSize) >= (UINT32)g_avpsAudioBufferTop)
        {
            cpSz = (UINT32)g_avpsAudioBufferTop - (UINT32)g_avpsAudioBufferCurPos;
            memcpy(decInBufPos, g_avpsAudioBufferCurPos, cpSz);
            decInBufPos        = (UINT32*)((UINT32)decInBufPos+cpSz);

            g_avpsAudioBufferCurPos = g_avpsAudioBufferStart;
            cpSz = g_avpsFrameSize - cpSz;

            // Buffer End Reached
            endBufferReached     = TRUE;
        }
        else
        {
            cpSz = g_avpsFrameSize;
        }

        // Finish or full copy
        memcpy(decInBufPos, g_avpsAudioBufferCurPos, cpSz);
        g_avpsAudioBufferCurPos = (UINT32*)((UINT32)g_avpsAudioBufferCurPos+cpSz);

        g_avpsFramesPlayedNb++;

        // Half buffer reached
        if (((UINT32)g_avpsAudioBufferCurPos >= (UINT32)g_avpsAudioBufferStart + (g_avpsAudioBufferLength/2)))
        {
            halfBufferReached    = TRUE;
        }

    }

    if (halfBufferReached || endBufferReached)
    {
        if (g_avpsAudioBufferingMsgReceived)
        {
            msg.H.Id        = AVPS_HEADER_ID_AUDIO;
            msg.B.bufferPos = g_avpsAudioBufferCurPos;
            g_avpsAudioBufferingMsgReceived = FALSE;
            sxr_Send(&msg, g_avpsUserMbx, SXR_SEND_EVT);
        }
    }

    AVPS_PROFILE_FUNCTION_EXIT(avps_Handler);
}


PRIVATE MPS_DECODE_TEST_RESULT_T avps_decodeSpeechTest(CONST AVPS_ENC_STREAM_T* stream)
{
    MPS_DECODE_TEST_RESULT_T result =  MPS_DECODE_TEST_RESULT_NO;

    if(stream->videoMode == AVPS_PLAY_VIDEO_MODE_NO)
    {
        result |= MPS_DECODE_TEST_RESULT_VIDEO;
    }

    switch(stream->audioMode)
    {
        case AVPS_PLAY_AUDIO_MODE_AMR475:
        case AVPS_PLAY_AUDIO_MODE_AMR515:
        case AVPS_PLAY_AUDIO_MODE_AMR59:
        case AVPS_PLAY_AUDIO_MODE_AMR67:
        case AVPS_PLAY_AUDIO_MODE_AMR74:
        case AVPS_PLAY_AUDIO_MODE_AMR795:
        case AVPS_PLAY_AUDIO_MODE_AMR102:
        case AVPS_PLAY_AUDIO_MODE_AMR122:
        case AVPS_PLAY_AUDIO_MODE_AMR_RING:
            result |= MPS_DECODE_TEST_RESULT_AUDIO;
            break;
        default:
            break;
    }

    return result;
}

PRIVATE VOID avps_decodeSpeechStop(VOID)
{
    vpp_SpeechClose();
}

PRIVATE AVPS_ERR_T avps_decodeSpeechSetup(CONST AVPS_CFG_T*              cfg)
{
    return AVPS_ERR_NO;
}

PRIVATE AVPS_ERR_T avps_decodeSpeechPlay(CONST AVPS_ENC_STREAM_T*       stream,
        CONST AVPS_CFG_T*              cfg,
        CONST LCDD_FBW_T*              frameWindows,
        HAL_AIF_STREAM_T*              pcmStream)
{
    HAL_SPEECH_DEC_IN_T    decStruct;
    VPP_SPEECH_AUDIO_CFG_T vppSpeech;
    UINT32                 i;

    // vpp speech congiguration structure
    g_avpsVppSpeechCfg.echoEsOn                 = 0;
    g_avpsVppSpeechCfg.echoEsVad                = 0;
    g_avpsVppSpeechCfg.echoEsDtd                = 0;
    g_avpsVppSpeechCfg.echoExpRel               = 0;
    g_avpsVppSpeechCfg.echoExpMu                = 0;
    g_avpsVppSpeechCfg.echoExpMin               = 0;
    g_avpsVppSpeechCfg.encMute                  = VPP_SPEECH_MUTE;
    g_avpsVppSpeechCfg.decMute                  = VPP_SPEECH_UNMUTE;
    g_avpsVppSpeechCfg.sdf                      = NULL;
    g_avpsVppSpeechCfg.mdf                      = NULL;
    g_avpsVppSpeechCfg.if1                      = 0;

    decStruct.dtxOn                          = 0;
    // Codec mode depends on the codec used by the stream
    // and will be set in the switch below
    decStruct.decFrameType                   = 0;
    decStruct.bfi                            = 0;
    decStruct.sid                            = 0;
    decStruct.taf                            = 0;
    decStruct.ufi                            = 0;

    // Size is a multiple of 4.
    for (i=0; i<HAL_SPEECH_FRAME_SIZE_COD_BUF/4; i++)
    {
        *((UINT32*)decStruct.decInBuf + i) = 0;
    }

    g_avpsAmrRing = FALSE;

    // AMR mode
    // Specific config
    switch (stream->audioMode)
    {
        case AVPS_PLAY_AUDIO_MODE_AMR475:
            g_avpsFrameSize     = HAL_SPEECH_FRAME_SIZE_AMR475;
            decStruct.codecMode = HAL_AMR475_DEC;
            break;

        case AVPS_PLAY_AUDIO_MODE_AMR515:
            g_avpsFrameSize     = HAL_SPEECH_FRAME_SIZE_AMR515;
            decStruct.codecMode = HAL_AMR515_DEC;
            break;

        case AVPS_PLAY_AUDIO_MODE_AMR59:
            g_avpsFrameSize     = HAL_SPEECH_FRAME_SIZE_AMR59;
            decStruct.codecMode = HAL_AMR59_DEC;
            break;

        case AVPS_PLAY_AUDIO_MODE_AMR67:
            g_avpsFrameSize     = HAL_SPEECH_FRAME_SIZE_AMR67 ;
            decStruct.codecMode = HAL_AMR67_DEC;
            break;

        case AVPS_PLAY_AUDIO_MODE_AMR74:
            g_avpsFrameSize     = HAL_SPEECH_FRAME_SIZE_AMR74 ;
            decStruct.codecMode = HAL_AMR74_DEC;
            break;

        case AVPS_PLAY_AUDIO_MODE_AMR795:
            g_avpsFrameSize     = HAL_SPEECH_FRAME_SIZE_AMR795;
            decStruct.codecMode = HAL_AMR795_DEC;
            break;

        case AVPS_PLAY_AUDIO_MODE_AMR102:
            g_avpsFrameSize     = HAL_SPEECH_FRAME_SIZE_AMR102;
            decStruct.codecMode = HAL_AMR102_DEC;
            break;

        case AVPS_PLAY_AUDIO_MODE_AMR122:
            g_avpsFrameSize     = HAL_SPEECH_FRAME_SIZE_AMR122;
            decStruct.codecMode = HAL_AMR122_DEC;
            break;

        case AVPS_PLAY_AUDIO_MODE_AMR_RING:
            g_avpsFrameSize        = 32;
            decStruct.codecMode    = HAL_AMR122_DEC;
            g_avpsVppSpeechCfg.if1 = 1;
            g_avpsAmrRing          = TRUE;
            break;

        case AVPS_PLAY_AUDIO_MODE_EFR:
            g_avpsFrameSize     = HAL_SPEECH_FRAME_SIZE_EFR;
            decStruct.codecMode = HAL_EFR;
            break;

        case AVPS_PLAY_AUDIO_MODE_HR:
            g_avpsFrameSize     = HAL_SPEECH_FRAME_SIZE_HR;
            decStruct.codecMode = HAL_HR;
            break;

        case AVPS_PLAY_AUDIO_MODE_FR:
            g_avpsFrameSize     = HAL_SPEECH_FRAME_SIZE_FR;
            decStruct.codecMode = HAL_FR;
            break;
        default:
            break;
    }

    // PCM Audio stream, output of VPP
    pcmStream->startAddress = (UINT32*)vpp_SpeechGetRxPcmBuffer();
    pcmStream->length       = sizeof(HAL_SPEECH_PCM_BUF_T);
    pcmStream->voiceQuality = TRUE;
    pcmStream->playSyncWithRecord = FALSE;

    // Mechanical interaction with VPP's VOC
    pcmStream->halfHandler  = NULL;
    // Mechanical interaction with VPP's VOC
    pcmStream->endHandler   = NULL;

    // VPP audio config
    vppSpeech.echoEsOn        = 0;
    vppSpeech.echoEsVad       = 0;
    vppSpeech.echoEsDtd       = 0;
    vppSpeech.echoExpRel      = 0;
    vppSpeech.echoExpMu       = 0;
    vppSpeech.echoExpMin      = 0;
    vppSpeech.encMute         = VPP_SPEECH_MUTE;
    vppSpeech.decMute         = VPP_SPEECH_MUTE;
    vppSpeech.sdf             = NULL;
    vppSpeech.mdf             = NULL;
    vppSpeech.if1             = 0;

    // configure AUD and VPP buffers
    // set vpp
    if (HAL_ERR_RESOURCE_BUSY == vpp_SpeechOpen(avps_VppSpeechHandler, VPP_SPEECH_WAKEUP_HW_DEC))
    {
        return AVPS_ERR_RESOURCE_BUSY;
    }
    vpp_SpeechAudioCfg(&vppSpeech);

    // Set all the preprocessing modules
    vpp_SpeechSetEncDecPocessingParams(0, 7, 0);

    // Init the Rx buffer
    *(vpp_SpeechGetRxCodBuffer()) = decStruct;

    return AVPS_ERR_NO;
}
