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



#include "cs_types.h"

#include "hal_aif.h"
#include "hal_sys.h"
#include "hal_clk.h"

#include "string.h"

#include "aud_m.h"
#include "lcdd_m.h"
#include "vpp_speech.h"
#include "vpp_amjp.h"

#include "sxr_ops.h"
#include "sxs_io.h"

#include "avpsp.h"
#include "avpsp_decode_amjp.h"
#include "avpsp_decode_speech.h"
#include "avpsp_decode_pcm.h"

#include "avpsp_decode.h"


// svn propset svn:keywords "HeadURL Author Date Revision" dummy.h

/// TODO Does this part of the doc belong to the API ?
/// Workflow description
///   Init:
///     Init VPP, and load the first sample in its decoding buffer.
///     Mute mode is set.
///     Lauch play on the output decoded buffer of VPP.
///
///   Data flow:
///     Data are copied on VPP interrupts, meaning the previous set has been
///     decoded. Decoding is triggered by the IFC 'interrupt' directly telling
///     VPP what to do.
///     On IRQ from VPP, we copy the next buffer to be decoded.

// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  STREAM GLOBAL VARIABLES
// =============================================================================

/// End address of the stream buffer
PROTECTED UINT32 g_avpsAudioBufferLength = 0;

/// Start address of the stream buffer
PROTECTED UINT32* g_avpsAudioBufferStart = NULL;

/// End address of the stream buffer
PROTECTED UINT32* g_avpsAudioBufferTop = NULL;

/// Number of bytes of data to read in the buffer
PROTECTED INT32 g_avpsAudioBufferRemainingBytes = 0;

/// Current position in the buffer. Next read data will be read from here.
PROTECTED UINT32* g_avpsAudioBufferCurPos = NULL;

/// Handshake to prevent the same message to be sent several times
/// When a message is sent, this variable is set to FALSE.
/// When data are added, it is set to TRUE.
PROTECTED BOOL g_avpsAudioBufferingMsgReceived = TRUE;

PROTECTED BOOL g_avpsVideoBufferingMsgReceived = TRUE;

// -----------------------------------------------------------------------------
// VIDEO RELATED GLOBAL VARIABLES
// -----------------------------------------------------------------------------
/// Ping pong encoded video buffer, halfes are arithmetically accessed thanks
/// to the buffer length.
PROTECTED UINT8* g_avpsVideoEncodedBuffer = NULL;

/// Length of the encoded video buffer.
PROTECTED UINT32 g_avpsVideoEncodedBufferLength = 0;

/// Encoded video buffer ping-pong index (0 or 1).
PROTECTED UINT32 g_avpsVideoEncodedIndex = 0;

/// Number of bytes of data to read in the buffer
PROTECTED INT32 g_avpsVideoBufferRemainingBytes = 0;

/// Frequency audio bars
PROTECTED UINT8 g_avpsAudioBars[16];

/// Ping-pong Frame Buffer windows to display the decoded video.
/// (Technically, only the buffer of the frame buffer window
/// does need to be ping-ponged, but, hey, that is more fun this
/// way).
PROTECTED LCDD_FBW_T g_avpsDecodedVideoFbw[2] =
{
    {
        .fb =
        {
            .buffer = NULL,
            .width  = 0,
            .height = 0,
            .colorFormat = LCDD_COLOR_FORMAT_RGB_565
        },
        .roi =
        {
            .x = 0,
            .y = 0,
            .width = 0,
            .height = 0,
        }
    }
    ,
    {
        .fb =
        {
            .buffer = NULL,
            .width  = 0,
            .height = 0,
            .colorFormat = LCDD_COLOR_FORMAT_RGB_565
        },
        .roi =
        {
            .x = 0,
            .y = 0,
            .width = 0,
            .height = 0,
        }
    }
};

/// Decoded video buffer ping-pong index (0 or 1).
PROTECTED UINT32 g_avpsVideoDecodedIndex = 0;

/// Decoded video buffer ping-pong index (0 or 1).
PROTECTED UINT32 g_avpsVideoDecodedDisplayIndex = 0;


// =============================================================================
//  CONFIG GLOBAL VARIABLES
// =============================================================================

/// Interface used to play the current stream.
PROTECTED AUD_ITF_T g_avpsItf = 0;

/// Number of bytes composing one trame for the codec currently used
PROTECTED UINT32 g_avpsFrameSize = 0;

/// User mbx to call with play status is stored here.
PROTECTED UINT8 g_avpsUserMbx = 0xFF;

/// Number of played trames
PROTECTED UINT32 g_avpsFramesPlayedNb = 0;

/// It's the audio frame number when the last video frame are played is use for
/// synchronisation audio video
PROTECTED UINT32 g_avpsNextVideoFramePlayed = 0;

PROTECTED UINT32 g_avpsAudioVideoRatio = 0;

/// Audio configuration used to play the stream in normal state.
/// (ie not when started or stopped.
PROTECTED AUD_LEVEL_T g_avpsAudioCfg;

/// counter for the errors
PROTECTED UINT32 g_avpsErrNb;

/// Audio PCM buffer
/// swap buffer of 576 samples of 16 bits (*2 for highest rates) per channel,
/// (*2 for the stereo) that is to say to the most 2*(576*2*2*2) bytes. Thus in UINT32 :
PROTECTED UINT32 g_avpsPcmBuf[2*576*2];

PROTECTED UINT32* g_avpsPcmBuf0;
PROTECTED UINT32* g_avpsPcmBuf1;

/// Define the current decoder use
PRIVATE AVPS_DECODE_T* currentDecode = 0;

/// List of coder module
PRIVATE AVPS_DECODE_T*                 g_mpsDecodeList[] =
{
    &g_avpsPcmDecode,
    &g_avpsSpeechDecode,
    &g_avpsAmjpDecode,
    0
};

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// avps_Setup
// -----------------------------------------------------------------------------
/// Configure the avps service..
///
/// This functions configures the playing of a stream on a given audio interface:
/// gain for the side tone and
/// the speaker, input selection for the microphone and output selection for
/// the speaker... (TODO complete)
///
/// @param itf Interface number of the interface to setup.
/// @param cfg The configuration set applied to the audio interface. See
/// #AUD_LEVEL_T for more details.
/// @return #AVPS_ERR_NO it can execute the configuration.
// =============================================================================
PUBLIC AVPS_ERR_T avps_Setup(CONST AVPS_CFG_T* cfg)
{
    AUD_ERR_T audErr;

    AVPS_ASSERT(cfg,               "AVPS setup: config == NULL");

    if(currentDecode == 0)
    {
        return AVPS_ERR_NO;
    }

    currentDecode->setup(cfg);

    // Set audio interface
    g_avpsAudioCfg.spkLevel  = cfg->spkLevel;
    g_avpsAudioCfg.micLevel  = AUD_MIC_MUTE;
    g_avpsAudioCfg.sideLevel = 0;
    g_avpsAudioCfg.toneLevel = 0;

    audErr = aud_Setup(g_avpsItf, &g_avpsAudioCfg);
    switch (audErr)
    {
        // TODO Add error as they comes

        case AUD_ERR_NO:
            return AVPS_ERR_NO;
            break;

        default:
            return AVPS_ERR_QTY; // ...unknown error ...
            break;
    }
}

// =============================================================================
// avps_Play
// -----------------------------------------------------------------------------
/// Start the playing of a stream.
///
/// This function  outputs the
/// audio on the audio interface specified as a parameter, on the output selected in
/// the #avps_Setup function. \n
/// In normal operation, when the buffer runs out, the playing will stop. The
/// handler is defined, it is expected that the handler will refeed data and
/// call #aps_AddedData to tell APS about that.
/// AVPS can a user function
/// at the middle or at the end of the playback.
///
/// @param itf Interface on which play that stream
/// @param stream Stream to play. Handler called at the middle and end of the buffer
/// are defined there.
/// @param cfg The configuration set applied to the audio interface
/// @param loop \c TRUE if the buffer is played in loop mode.
/// @return #AVPS_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command, or if VPP is unavailable.
///         #AVPS_ERR_NO it can execute the command.
// =============================================================================
PUBLIC AVPS_ERR_T avps_Play(CONST UINT8                    itf,
                            CONST AVPS_ENC_STREAM_T*       stream,
                            CONST AVPS_CFG_T*              cfg,
                            CONST LCDD_FBW_T*              frameWindows,
                            CONST AVPS_BUFFER_CONTEXT_T*   bufContext)
{

    // TODO
    // BIG switch for PCM and/or other codec
    // Initial audio confguration
    AUD_LEVEL_T              audioCfg;
    HAL_AIF_STREAM_T         pcmStream;
    AUD_ERR_T                audErr;
    HAL_AIF_FREQ_T           rate = 0;
    UINT32                   i;
    MPS_DECODE_TEST_RESULT_T result;

    AVPS_PROFILE_FUNCTION_ENTER(avps_Play);
    AVPS_TRACE(TSTDOUT| TLEVEL (1), 0, "AVPS Start");
    AVPS_TRACE(TSTDOUT| TLEVEL (1), 0, "AVPS Stream - buffer %#x  %d codecAudio %#x codecVideo %#x",
               stream->audioAddress, stream->audioLength,
               stream->audioMode, stream->videoMode);

    AVPS_ASSERT(stream,              "AVPS play: stream == NULL");
    AVPS_ASSERT(cfg,                 "AVPS play: cfg == NULL");
    AVPS_ASSERT(itf < AUD_ITF_QTY,   "AVPS play: bad interface");
    AVPS_ASSERT(stream->mbx != 0xFF, "AVPS play: mailbox is mandatory");
    AVPS_ASSERT(currentDecode == 0,  "AVPS play: avps already open");

    switch(stream->audioSampleRate)
    {
        case 8000 :
            rate = HAL_AIF_FREQ_8000HZ;
            break;
        case 11025:
            rate = HAL_AIF_FREQ_11025HZ;
            break;
        case 12000:
            rate = HAL_AIF_FREQ_12000HZ;
            break;
        case 16000:
            rate = HAL_AIF_FREQ_16000HZ;
            break;
        case 22050:
            rate = HAL_AIF_FREQ_22050HZ;
            break;
        case 24000:
            rate = HAL_AIF_FREQ_24000HZ;
            break;
        case 32000:
            rate = HAL_AIF_FREQ_32000HZ;
            break;
        case 44100:
            rate = HAL_AIF_FREQ_44100HZ;
            break;
        case 48000:
            rate = HAL_AIF_FREQ_48000HZ;
            break;
        default:
            rate = 0;
            AVPS_TRACE(TSTDOUT, 0, "AVPS play: sampleRate %i not supported", stream->audioSampleRate);
            return AVPS_ERR_INVALID_SAMPLERATE;
    }
    pcmStream.sampleRate   = rate;
    pcmStream.channelNb    = stream->channelNb;

    AVPS_TRACE(TSTDOUT, 0, "AVPS play: sampleRate %i channel %i",
               stream->audioSampleRate, stream->channelNb);

    memset(g_avpsAudioBars, 0, sizeof(g_avpsAudioBars));

    // Register global var
    g_avpsItf                       = itf;
    g_avpsAudioBufferStart          = (UINT32*) stream->audioAddress;
    g_avpsAudioBufferTop            = (UINT32*)(stream->audioAddress + stream->audioLength);
    g_avpsAudioBufferCurPos         = (UINT32*) stream->audioAddress;
    g_avpsAudioBufferRemainingBytes = 0;

    if(bufContext)
    {
        g_avpsAudioBufferCurPos         = (UINT32*) bufContext->bufferAudioReadPosition;
        g_avpsAudioBufferRemainingBytes = bufContext->audioRemainingSize;
    }
    g_avpsAudioBufferingMsgReceived = TRUE;
    g_avpsVideoBufferingMsgReceived = TRUE;

    g_avpsNextVideoFramePlayed = 0;
    g_avpsAudioVideoRatio      = stream->audioVideoRatio;

    g_avpsAudioBufferLength    = stream->audioLength;

    g_avpsFramesPlayedNb       = 0;
    g_avpsUserMbx              = stream->mbx;

    g_avpsAudioCfg.spkLevel  = cfg->spkLevel;
    g_avpsAudioCfg.micLevel  = AUD_MIC_MUTE;
    g_avpsAudioCfg.sideLevel = 0;
    g_avpsAudioCfg.toneLevel = 0;

    aud_StreamPause(g_avpsItf, FALSE);

    // Set audio interface
    // Initially muted, unmuted after a given number of trame
    audioCfg.spkLevel  = AUD_SPK_MUTE;
    audioCfg.micLevel  = AUD_MIC_MUTE;
    audioCfg.sideLevel = 0;
    audioCfg.toneLevel = 0;

    g_avpsErrNb        = 0;


    result = MPS_DECODE_TEST_RESULT_NO;
    for(i = 0; g_mpsDecodeList[i]; ++i)
    {
        switch(g_mpsDecodeList[i]->test(stream))
        {
            case MPS_DECODE_TEST_RESULT_NO         :
                break;
            case MPS_DECODE_TEST_RESULT_AUDIO      :
                currentDecode = g_mpsDecodeList[i];
                result        = MPS_DECODE_TEST_RESULT_AUDIO;
                break;
            case MPS_DECODE_TEST_RESULT_VIDEO      :
                if(result != MPS_DECODE_TEST_RESULT_AUDIO)
                {
                    currentDecode = g_mpsDecodeList[i];
                    result        = MPS_DECODE_TEST_RESULT_VIDEO;
                }
                break;
            case MPS_DECODE_TEST_RESULT_AUDIO_VIDEO:
                currentDecode = g_mpsDecodeList[i];
                result        = MPS_DECODE_TEST_RESULT_AUDIO_VIDEO;
                break;
        }
        if(result == MPS_DECODE_TEST_RESULT_AUDIO_VIDEO)
        {
            break;
        }
    }

    if(currentDecode == 0)
    {
        return AVPS_ERR_NO_CODEC;
    }

    currentDecode->play(stream, cfg, frameWindows, &pcmStream);

    AVPS_TRACE(TSTDOUT, 0, "AVPS play: decode %i found", i);

    // Clear the PCM buffer
    memset(pcmStream.startAddress, 0, pcmStream.length);

    // Set audio interface
    audioCfg.spkLevel  = AUD_SPK_MUTE;
    audioCfg.micLevel  = AUD_MIC_MUTE;
    audioCfg.sideLevel = 0;
    audioCfg.toneLevel = 0;

    // ...

    audErr = aud_StreamStart(itf, &pcmStream, &audioCfg);

    // profit
    switch (audErr)
    {
        case AUD_ERR_NO:
            AVPS_PROFILE_FUNCTION_EXIT(avps_Play);

            hal_SwRequestClk(FOURCC_AVPS, HAL_SYS_FREQ_39M);
            return AVPS_ERR_NO;
        default:
            AVPS_TRACE(_PAL| TLEVEL (1), 0, "AVPS Start failed !!!");
            AVPS_PROFILE_FUNCTION_EXIT(avps_Play);
    }
    return AVPS_ERR_UNKNOWN;
}



// =============================================================================
// avps_Stop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback.
/// If the function returns
/// #AVPS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #AVPS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AVPS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AVPS_ERR_T avps_Stop(AVPS_BUFFER_CONTEXT_T* bufContext)
{
    AVPS_PROFILE_FUNCTION_ENTER(avps_Stop);

    AVPS_TRACE(TSTDOUT, 0, "avps_Stop");

    if(currentDecode == 0)
    {
        AVPS_PROFILE_FUNCTION_EXIT(avps_Stop);
        return AVPS_ERR_NO;
    }

    // stop stream ...
    aud_StreamStop(g_avpsItf);

    currentDecode->stop();
    currentDecode = 0;

    if(bufContext)
    {
        bufContext->bufferAudioReadPosition = (UINT8*) g_avpsAudioBufferCurPos;
        if(g_avpsAudioBufferRemainingBytes > 0)
        {
            bufContext->audioRemainingSize = (UINT32) g_avpsAudioBufferRemainingBytes;
        }
        g_avpsAudioBufferRemainingBytes     = 0;
    }

    g_avpsUserMbx              = 0xFF;


    hal_SwReleaseClk(FOURCC_AVPS);

    AVPS_PROFILE_FUNCTION_EXIT(avps_Stop);
    return AVPS_ERR_NO;
}


// =============================================================================
// avps_AudioAddedData
// -----------------------------------------------------------------------------
/// When a stream buffer is played in loop mode, already played data can be
/// replaced by new ones to play a long song seamlessly. The AVPS service needs
/// to be informed about the number of new bytes to play in the buffer. This
/// is the role of the #avps_AudioAddedData function.
///
/// It is mandatory to call this function when a stream is played in loop mode
/// with handler defined.
///
///
/// If the function returns
/// #AVPS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param addedDataBytes Number of bytes added to the buffer.
/// @return #AVPS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AVPS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AVPS_ERR_T avps_AudioAddedData(UINT32 addedDataBytes)
{
    UINT32 status;

    AVPS_PROFILE_FUNCTION_ENTER(avps_AudioAddedData);

    AVPS_TRACE(TSTDOUT, 0, "avps_AudioAddedData: %i", addedDataBytes);

    status = hal_SysEnterCriticalSection();
    g_avpsAudioBufferRemainingBytes += addedDataBytes;
    AVPS_ASSERT(g_avpsAudioBufferRemainingBytes <= g_avpsAudioBufferLength,
                "avps_AudioAddedData: Overflow add to many data %d/%d",
                g_avpsAudioBufferRemainingBytes, g_avpsAudioBufferLength);
    // This function is called to tell AVPS the buffer has been
    // refilled, which means the upper layer has received
    // and understood the latest sent message.
    g_avpsAudioBufferingMsgReceived = TRUE;

    hal_SysExitCriticalSection(status);

    AVPS_PROFILE_FUNCTION_EXIT(avps_AudioAddedData);
    return AVPS_ERR_NO;
}


// =============================================================================
// avps_VideoAddedData
// -----------------------------------------------------------------------------
/// This function is use to inform avps of new video frame
///
/// @return #AVPS_ERR_NO No error
// =============================================================================
PUBLIC AVPS_ERR_T avps_VideoAddedData(UINT32 addedDataBytes)
{
    AVPS_PROFILE_FUNCTION_ENTER(avps_VideoAddedData);

    AVPS_ASSERT(g_avpsVideoBufferingMsgReceived == FALSE,
                "avps_VideoAddedData: Overflow add to many data");

    AVPS_TRACE(TSTDOUT, 0, "avps_VideoAddedData: add data %i\n", addedDataBytes);

    g_avpsVideoBufferingMsgReceived = TRUE;
    g_avpsVideoBufferRemainingBytes = addedDataBytes;

    AVPS_PROFILE_FUNCTION_EXIT(avps_VideoAddedData);
    return AVPS_ERR_NO;
}

PUBLIC AVPS_FREQUENCY_BARS_T* avps_GetFreqBars(VOID)
{
    return &g_avpsAudioBars;
}
