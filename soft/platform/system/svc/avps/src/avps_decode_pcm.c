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




#include "avpsp.h"
#include "avpsp_decode_pcm.h"

#include "sxr_ops.h"
#include "sxs_io.h"

#include "string.h"


// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// PRIVATE VARIABLES
// =============================================================================

PRIVATE UINT32* g_avpsLastReadPointer = 0;

// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

PRIVATE MPS_DECODE_TEST_RESULT_T avps_decodePcmTest(CONST AVPS_ENC_STREAM_T* stream);

PRIVATE VOID avps_decodePcmStop(VOID);

PRIVATE AVPS_ERR_T avps_decodePcmPlay(CONST AVPS_ENC_STREAM_T*       stream,
                                      CONST AVPS_CFG_T*              cfg,
                                      CONST LCDD_FBW_T*              frameWindows,
                                      HAL_AIF_STREAM_T*              pcmStream);

PRIVATE AVPS_ERR_T avps_decodePcmSetup(CONST AVPS_CFG_T*              cfg);

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PROTECTED AVPS_DECODE_T g_avpsPcmDecode =
{
    .test  = avps_decodePcmTest,
    .play  = avps_decodePcmPlay,
    .stop  = avps_decodePcmStop,
    .setup = avps_decodePcmSetup
};

// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// avps_XXXPcmHandler
// -----------------------------------------------------------------------------
/// Handler for the XXX module, to play PCM  streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PRIVATE VOID avps_XXXPcmHandler(VOID)
{
    // Number of bytes to copy;
    UINT32 cpSz;

    // Position in the copy buffer, where data from the stream
    // are going to be copied
    UINT32* cpBufPos;
    UINT32  addr;

    Msg_t   msg;


    AVPS_PROFILE_FUNCTION_ENTER(avps_Handler);


    /// Special operation according to state
    ///
    /// if (sampleNumber == ...)
    /// ...
    /// ...
    if (g_avpsFramesPlayedNb == AVPS_AUD_UNMUTE_TRAME_NB)
    {
        // Unmute AUD
        aud_Setup(g_avpsItf, &g_avpsAudioCfg);
    }


    if (g_avpsAudioBufferRemainingBytes < g_avpsFrameSize)
    {
        if(g_avpsLastReadPointer != g_avpsAudioBufferCurPos)
        {
            if (g_avpsAudioBufferingMsgReceived)
            {
                // Data underflow ...
                // Stops here
                addr             = (UINT32)g_avpsAudioBufferCurPos + g_avpsAudioBufferRemainingBytes;
                if((UINT32)addr < (UINT32)g_avpsAudioBufferStart)
                {
                    addr += (UINT32)g_avpsAudioBufferTop - (UINT32)g_avpsAudioBufferStart;
                }
                if((UINT32)addr >= (UINT32)g_avpsAudioBufferTop)
                {
                    addr -= (UINT32)g_avpsAudioBufferTop - (UINT32)g_avpsAudioBufferStart;
                }

                AVPS_TRACE(TSTDOUT, 0,"AVPS famine\n");
                AUD_LEVEL_T audioCfg = g_avpsAudioCfg;

                msg.H.Id        = AVPS_HEADER_ID_AUDIO;
                msg.B.bufferPos = (UINT32*)addr;
                g_avpsAudioBufferingMsgReceived = FALSE;
                sxr_Send(&msg, g_avpsUserMbx, SXR_SEND_EVT);
                /// Mute ...
                audioCfg.spkLevel = 0;
                aud_Setup(g_avpsItf, &audioCfg);

                if(g_avpsFramesPlayedNb != 0)
                {
                    avps_Stop(0);
                }

                /// Set state as for starting again from there ?
                g_avpsFramesPlayedNb = 0;
            }
            else
            {
                if(g_avpsFramesPlayedNb != 0)
                {
                    avps_Stop(0);
                }
            }
        }
        g_avpsLastReadPointer = g_avpsAudioBufferCurPos;
    }
    else
    {
        g_avpsLastReadPointer = 0;
        UINT32 status = hal_SysEnterCriticalSection();
        // Decrease size in bytes
        g_avpsAudioBufferRemainingBytes -= g_avpsFrameSize;
        hal_SysExitCriticalSection(status);

        // use swap buffer
        if((((UINT32)*hal_AifGetIfcStatusRegPtr())&0xFFFFFF) >= (((UINT32)g_avpsPcmBuf0)&0xFFFFFF) &&
                (((UINT32)*hal_AifGetIfcStatusRegPtr())&0xFFFFFF) <  (((UINT32)g_avpsPcmBuf1)&0xFFFFFF))
        {
            cpBufPos = g_avpsPcmBuf1;
        }
        else
        {
            cpBufPos = g_avpsPcmBuf0;
        }

        ///
        if ((((UINT32)g_avpsAudioBufferCurPos) + (g_avpsFrameSize)) > (UINT32)g_avpsAudioBufferTop)
        {
            cpSz = (UINT32)g_avpsAudioBufferTop - (UINT32)g_avpsAudioBufferCurPos;
            memcpy(cpBufPos, g_avpsAudioBufferCurPos, cpSz);

            cpBufPos                = (UINT32*)((UINT32)cpBufPos + cpSz);
            g_avpsAudioBufferCurPos = g_avpsAudioBufferStart;
            cpSz                    = g_avpsFrameSize - cpSz;
        }
        else
        {
            cpSz = g_avpsFrameSize;
        }


        // Finish or full copy
        memcpy(cpBufPos, g_avpsAudioBufferCurPos, cpSz);
        g_avpsAudioBufferCurPos = (UINT32*)((UINT32)g_avpsAudioBufferCurPos+cpSz);

        if((UINT32)g_avpsAudioBufferCurPos == (UINT32)g_avpsAudioBufferTop)
        {
            g_avpsAudioBufferCurPos = g_avpsAudioBufferStart;
        }

        g_avpsFramesPlayedNb++;
    }

    if (g_avpsAudioBufferRemainingBytes < (g_avpsAudioBufferLength/2))
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

PRIVATE MPS_DECODE_TEST_RESULT_T avps_decodePcmTest(CONST AVPS_ENC_STREAM_T* stream)
{
    MPS_DECODE_TEST_RESULT_T result =  MPS_DECODE_TEST_RESULT_NO;

    if(stream->videoMode == AVPS_PLAY_VIDEO_MODE_NO)
    {
        result |= MPS_DECODE_TEST_RESULT_VIDEO;
    }

    if(stream->audioMode == AVPS_PLAY_AUDIO_MODE_PCM)
    {
        result |= MPS_DECODE_TEST_RESULT_AUDIO;
    }

    return result;
}

PRIVATE VOID avps_decodePcmStop(VOID)
{

}

PRIVATE AVPS_ERR_T avps_decodePcmSetup(CONST AVPS_CFG_T*              cfg)
{
    return AVPS_ERR_NO;
}


PRIVATE AVPS_ERR_T avps_decodePcmPlay(CONST AVPS_ENC_STREAM_T*       stream,
                                      CONST AVPS_CFG_T*              cfg,
                                      CONST LCDD_FBW_T*              frameWindows,
                                      HAL_AIF_STREAM_T*              pcmStream)
{
    g_avpsFrameSize = HAL_SPEECH_FRAME_SIZE_PCM_BUF;

    g_avpsPcmBuf0   = g_avpsPcmBuf;
    g_avpsPcmBuf1   = (UINT32*)((UINT32)g_avpsPcmBuf + sizeof(HAL_SPEECH_PCM_HALF_BUF_T));

    // PCM Audio stream, output of VPP
    pcmStream->startAddress = g_avpsPcmBuf;
    pcmStream->length       = sizeof(HAL_SPEECH_PCM_HALF_BUF_T)*2;
    pcmStream->voiceQuality = stream->voiceQuality;
    pcmStream->playSyncWithRecord = FALSE;
    pcmStream->halfHandler  = avps_XXXPcmHandler;
    pcmStream->endHandler   = avps_XXXPcmHandler;

    return AVPS_ERR_NO;
}
