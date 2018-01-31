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
#include "hal_error.h"

#include "aud_m.h"
#include "aps_m.h"
#include "apsp_cfg.h"
#include "vpp_speech.h"
#include "vpp_amjp.h"

#include "string.h"




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

/// Number of trames before unmuting AUD
#define APS_AUD_UNMUTE_TRAME_NB 6

/// Number of trames before unmuting VPP
#define APS_VPP_UNMUTE_TRAME_NB 3



// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  STREAM GLOBAL VARIABLES
// =============================================================================

/// End address of the stream buffer
PRIVATE UINT32 g_apsBufferLength= 0;

/// Start address of the stream buffer
PRIVATE UINT32* g_apsBufferStart= NULL;

/// End address of the stream buffer
PRIVATE UINT32* g_apsBufferTop = NULL;

/// Number of bytes of data to read in the buffer
PRIVATE INT32 g_apsBufferRemainingBytes = 0;

/// Current position in the buffer. Next read data will be read from here.
PRIVATE UINT32* g_apsBufferCurPos = NULL;

// =============================================================================
//  CONFIG GLOBAL VARIABLES
// =============================================================================

/// Interface used to play the current stream.
PRIVATE AUD_ITF_T g_apsItf = 0;

/// Number of bytes composing one trame for the codec currently used
PRIVATE UINT32 g_apsFrameSize = 0;

/// To stop at the end of the buffer if not in loop mode
PRIVATE BOOL g_apsLoopMode= FALSE;

/// User handler to call with play status is stored here.
PRIVATE APS_USER_HANDLER_T g_apsUserHandler = NULL;

/// Number of played trames
PRIVATE UINT32 g_apsFramesPlayedNb= 0;

/// Pointer to the stream input buffer of VPP decoding buffer
PRIVATE UINT32* g_apsDecInput= NULL;

/// To only pulse the mid buffer
PRIVATE BOOL g_apsMidBufDetected = FALSE;

/// Index in the swap buffer (for PCM) which is the one where to copy
/// the new data
PRIVATE UINT32 g_apsSwIdx = 0;

/// Audio configuration used to play the stream in normal state.
/// (ie not when started or stopped.
PRIVATE AUD_LEVEL_T g_apsAudioCfg;

/// VPP configuration used to play the stream in normal state.
/// (ie not when started or stopped).
PRIVATE VPP_SPEECH_AUDIO_CFG_T g_apsVppCfg;

/// VPP configuration used to play multimedai stream in normal state.
/// (ie not when started or stopped.
PRIVATE VPP_AMJP_AUDIO_CFG_T g_apsVppAmjpAudioCfg;

/// counter for the errors
PRIVATE UINT32 g_apsErrNb;

/// TODO : should be properly dfefined in the header
#define APS_ERR_NB_MAX 20

/// Audio PCM buffer
/// TODO :  Durty patch to play MP3. This bufer should be allocated by MPS!
PRIVATE UINT32 g_apsPcmBuf[2][576*2];



/// TODO: move that in the speech handler specific file.
/// Enable amr ring
PROTECTED BOOL g_apsAmrRing;

/// Are we playing a speech stream ?
PRIVATE BOOL g_apsSpeechUsed = FALSE;


// TODO : This is durty, half/full buff values should be given by MPS
#define  g_apsPcmMono8kBuf_0    &g_apsPcmBuf[0][0];
#define  g_apsPcmMono8kBuf_1    &g_apsPcmBuf[0][576/2]
#define  g_apsPcmMono8kBufSize  2*576*2
#define  g_apsPcmStereoBuf_0    &g_apsPcmBuf[0][0]
#define  g_apsPcmStereoBuf_1    &g_apsPcmBuf[1][0]
#define  g_apsPcmStereoBufSize  2*1152*4


// =============================================================================
//  FUNCTIONS
// =============================================================================
// =============================================================================
// aps_VppAmjpHandler
// -----------------------------------------------------------------------------
// =============================================================================
PRIVATE VOID aps_VppAmjpHandler(HAL_VOC_IRQ_STATUS_T* IRQStatus)
{
    APS_PROFILE_FUNCTION_ENTER(aps_VppAmjpHandler);

    if (g_apsFramesPlayedNb == APS_AUD_UNMUTE_TRAME_NB)
    {
        // Unmute AUD
        APS_TRACE(_PAL| TLEVEL (1), 0, "AUD unmuted");
        aud_Setup(g_apsItf, &g_apsAudioCfg);
    }

    if (g_apsBufferRemainingBytes < 0)
    {

        if (g_apsUserHandler)
        {
            g_apsUserHandler(g_apsBufferRemainingBytes);
        }

        // Stop the play
        aps_Stop();

        // Leave
        return;
    }


    // do not read status during the first callback
    if (g_apsFramesPlayedNb++)
    {
        VPP_AMJP_STATUS_T vppStatus;
        INT32 readBytes;

        // get status
        vpp_AmjpStatus(&vppStatus);


        // check for errors
        if (vppStatus.errorStatus)
        {
            if (g_apsErrNb++ > APS_ERR_NB_MAX)
            {
                // Stop the play
                aps_Stop();

                // Leave
                return;
            }
        }

        // compute the new remaining size
        readBytes = (INT32)vppStatus.inStreamBufAddr - (INT32)g_apsBufferCurPos;

        if (readBytes < 0)
        {
            readBytes += g_apsBufferLength;
        }

        // update the stream buffer status
        g_apsBufferRemainingBytes -= readBytes;

        // update read pointer
        g_apsBufferCurPos = vppStatus.inStreamBufAddr;
    }

    // call the user handler if new data is necessary
    if (g_apsBufferRemainingBytes < (g_apsBufferLength/2))
    {
        if (g_apsUserHandler)
        {
            g_apsUserHandler(g_apsBufferRemainingBytes);
        }
    }

    // use swap buffers
    g_apsVppAmjpAudioCfg.outStreamBufAddr = (!(g_apsFramesPlayedNb & 1)) ? g_apsPcmMono8kBuf_1 : g_apsPcmMono8kBuf_0;

    // configure next frame
    vpp_AmjpAudioCfg(&g_apsVppAmjpAudioCfg);
    vpp_AmjpScheduleOneAudioFrame(g_apsVppAmjpAudioCfg.outStreamBufAddr);

    // profile
    APS_PROFILE_FUNCTION_EXIT(aps_VppAmjpHandler);
}



// =============================================================================
// aps_VppSpeechHandler
// -----------------------------------------------------------------------------
/// Handler for the vpp module, for speech-kind encoded streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PRIVATE VOID aps_VppSpeechHandler(HAL_VOC_IRQ_STATUS_T* IRQStatus)
{
    APS_PROFILE_FUNCTION_ENTER(aps_VppSpeechHandler);
    // Number of bytes to copy;
    UINT32 cpSz;

    // Position in the decoding structure where to store
    // the data from the stream.
    UINT32* decInBufPos;

    BOOL    endBufferReached = FALSE;
    BOOL    halfBufferReached = FALSE;
    HAL_SPEECH_DEC_IN_T* decStruct;

    /// Special operation according to state
    ///
    /// if (sampleNumber == ...)
    /// ...
    /// ...

    if (g_apsFramesPlayedNb == APS_VPP_UNMUTE_TRAME_NB)
    {
        // Unmute VPP
        APS_TRACE(_PAL| TLEVEL (1), 0, "VPP unmuted");
        vpp_SpeechAudioCfg(&g_apsVppCfg);
    }

    decStruct = vpp_SpeechGetRxCodBuffer();

    if(g_apsAmrRing == TRUE)
    {
        g_apsFrameSize = 1;
        if(g_apsBufferRemainingBytes >= 1)
        {
            switch((*((UINT8*)g_apsBufferCurPos) >> 3) & 0xF)
            {
                // Real frame size, as there is no alignment
                case 0:
                    g_apsFrameSize      = 13;
                    decStruct->codecMode = HAL_AMR475_DEC;
                    break;
                case 1:
                    g_apsFrameSize      = 14;
                    decStruct->codecMode = HAL_AMR515_DEC;
                    break;
                case 2:
                    g_apsFrameSize      = 16;
                    decStruct->codecMode = HAL_AMR59_DEC;
                    break;
                case 3:
                    g_apsFrameSize      = 18;
                    decStruct->codecMode = HAL_AMR67_DEC;
                    break;
                case 4:
                    g_apsFrameSize      = 20;
                    decStruct->codecMode = HAL_AMR74_DEC;
                    break;
                case 5:
                    g_apsFrameSize      = 21;
                    decStruct->codecMode = HAL_AMR795_DEC;
                    break;
                case 6:
                    g_apsFrameSize      = 27;
                    decStruct->codecMode = HAL_AMR102_DEC;
                    break;
                case 7:
                    g_apsFrameSize      = 32;
                    decStruct->codecMode = HAL_AMR122_DEC;
                    break;
                default:
                    break;
            }
        }
    }




    if (g_apsBufferRemainingBytes < g_apsFrameSize)
    {
        // FIXME Let's assume something: If the buffer
        // is played in no loop, we don't care about the
        // alignment and guess that there won't be any refill
        if (!g_apsLoopMode)
        {
            // As stop destroy the global vars ...
            APS_USER_HANDLER_T handler = g_apsUserHandler;

            // Stop the play
            aps_Stop();

            // End of buffer reached
            if (handler)
            {
                handler(APS_STATUS_END_BUFFER_REACHED);
            }

            // Leave
            return;
        }
        else
        {

            if (g_apsUserHandler)
                // Data underflow ...
                // Stops here
            {
                // We need and ask for refill
                APS_TRACE(TSTDOUT, 0,"APS famine\n");
                AUD_LEVEL_T audioCfg = g_apsAudioCfg;
                VPP_SPEECH_AUDIO_CFG_T vppCfg = g_apsVppCfg;
                g_apsUserHandler(APS_STATUS_NO_MORE_DATA);
                /// Mute ...
                audioCfg.spkLevel = 0;
                vppCfg.decMute = VPP_SPEECH_MUTE;
                aud_Setup(g_apsItf, &audioCfg);
                vpp_SpeechAudioCfg(&vppCfg);

                /// Set state as for starting again from there ?
                g_apsFramesPlayedNb = 0;
            }
            else
            {
                // Simply wrap, we want to play that buffer again
                // and again
                g_apsBufferCurPos = g_apsBufferStart;
                g_apsBufferRemainingBytes = g_apsBufferLength;
            }
        }

    }
    else
    {
        UINT32 csStatus = hal_SysEnterCriticalSection();
        g_apsBufferRemainingBytes -= g_apsFrameSize;
        hal_SysExitCriticalSection(csStatus);

        decInBufPos = (UINT32*)decStruct->decInBuf;

        ///
        if (((UINT32)g_apsBufferCurPos + g_apsFrameSize) > (UINT32)g_apsBufferTop)
        {
            if (!g_apsLoopMode)
            {
                // As stop destroy the global vars ...
                APS_USER_HANDLER_T handler = g_apsUserHandler;

                // Stop the play
                aps_Stop();

                // End of buffer reached
                if (handler)
                {
                    handler(APS_STATUS_END_BUFFER_REACHED);
                }

                // Leave
                return;
            }
            // else wrap ...

            // Copy first part
            cpSz = (UINT32)(g_apsBufferTop - g_apsBufferCurPos);
            memcpy(decInBufPos, g_apsBufferCurPos, cpSz);
            decInBufPos        = (UINT32*)((UINT32)decInBufPos+cpSz);

            g_apsBufferCurPos = g_apsBufferStart;
            cpSz = g_apsFrameSize - cpSz;

            // Buffer End Reached
            endBufferReached = TRUE;
            g_apsMidBufDetected = FALSE;
        }
        else
        {
            cpSz = g_apsFrameSize;
        }

        // Finish or full copy
        memcpy(decInBufPos, g_apsBufferCurPos, cpSz);
        g_apsBufferCurPos = (UINT32*)((UINT32)g_apsBufferCurPos+cpSz);

        g_apsFramesPlayedNb++;

        // Half buffer reached
        if (((UINT32)g_apsBufferCurPos >= (UINT32)g_apsBufferStart + (g_apsBufferLength/2))
                && !g_apsMidBufDetected)
        {
            halfBufferReached = TRUE;
            g_apsMidBufDetected = TRUE;
        }

    }

    if (halfBufferReached)
    {
        if (g_apsUserHandler)
        {
            g_apsUserHandler(APS_STATUS_MID_BUFFER_REACHED);
        }
    }

    if (endBufferReached)
    {
        if (g_apsUserHandler)
        {
            g_apsUserHandler(APS_STATUS_END_BUFFER_REACHED);
        }
    }

    APS_PROFILE_FUNCTION_EXIT(aps_VppSpeechHandler);
}



// =============================================================================
// aps_XXXPcmHandler
// -----------------------------------------------------------------------------
/// Handler for the XXX module, to play PCM  streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PRIVATE VOID aps_XXXPcmHandler(VOID)
{
    // Number of bytes to copy;
    UINT32 cpSz;
    UINT32 i;

    // Position in the copy buffer, where data from the stream
    // are going to be copied
    UINT32* cpBufPos;

    BOOL    endBufferReached = FALSE;
    BOOL    halfBufferReached = FALSE;

    APS_PROFILE_FUNCTION_ENTER(aps_XXXSpeechHandler);


    /// Special operation according to state
    ///
    /// if (sampleNumber == ...)
    /// ...
    /// ...
    if (g_apsFramesPlayedNb == APS_AUD_UNMUTE_TRAME_NB)
    {
        // Unmute AUD
        aud_Setup(g_apsItf, &g_apsAudioCfg);
    }


    if (g_apsBufferRemainingBytes < g_apsFrameSize)
    {
        // FIXME Let's assume something: If the buffer
        // is played in no loop, we don't care about the
        // alignment and guess that there won't be any refill
        if (!g_apsLoopMode)
        {
            // As stop destroy the global vars ...
            APS_USER_HANDLER_T handler = g_apsUserHandler;

            // Stop the play
            aps_Stop();

            // End of buffer reached
            if (handler)
            {
                handler(APS_STATUS_END_BUFFER_REACHED);
            }

            // Leave
            return;
        }
        else
        {
            if (g_apsUserHandler)
            {
                // Data underflow ...
                // Stops here
                APS_TRACE(TSTDOUT, 0,"APS famine\n");
                AUD_LEVEL_T audioCfg = g_apsAudioCfg;
//                VPP_SPEECH_AUDIO_CFG_T vppCfg = g_apsVppCfg;
                g_apsUserHandler(APS_STATUS_NO_MORE_DATA);
                /// Mute ...
                audioCfg.spkLevel = 0;
//                vppCfg.decMute = VPP_SPEECH_MUTE;
                aud_Setup(g_apsItf, &audioCfg);

                /// Set state as for starting again from there ?
                g_apsFramesPlayedNb = 0;
            }
            else
            {
                // Simply wrap, we want to play that buffer again
                // and again
                g_apsBufferCurPos = g_apsBufferStart;
                g_apsBufferRemainingBytes = g_apsBufferLength;
            }
        }
    }
    else
    {
        UINT32 status = hal_SysEnterCriticalSection();
        // Decrease size in bytes
        g_apsBufferRemainingBytes -= g_apsFrameSize;
        hal_SysExitCriticalSection(status);


        if (g_apsSwIdx == 0)
        {
            cpBufPos = g_apsDecInput;
            g_apsSwIdx = 1;
        }
        else
        {
            // == 1
            cpBufPos =  g_apsDecInput + sizeof(HAL_SPEECH_PCM_BUF_T)/8; // half size in UINT32
            g_apsSwIdx = 0;
        }


        ///
        if ((((UINT32)g_apsBufferCurPos) + (g_apsFrameSize)) > (UINT32)g_apsBufferTop)
        {
            if (!g_apsLoopMode)
            {
                // As stop destroy the global vars ...
                APS_USER_HANDLER_T handler = g_apsUserHandler;

                // Stop the play
                aps_Stop();

                // End of buffer reached
                if (handler)
                {
                    handler(APS_STATUS_END_BUFFER_REACHED);
                }

                // Leave
                return;
            }
            // else wrap ...

            // Copy first part
            cpSz = (UINT32)(g_apsBufferTop - g_apsBufferCurPos);
            for (i = 0 ; i<(cpSz/4) ; i++)
            {
                *(cpBufPos++) = *(g_apsBufferCurPos++) ;
            }

            g_apsBufferCurPos = g_apsBufferStart;
            cpSz = g_apsFrameSize - cpSz;

            // Buffer End Reached
            endBufferReached = TRUE;
            g_apsMidBufDetected = FALSE;
        }
        else
        {
            cpSz = g_apsFrameSize;
        }

        // Finish or full copy
        for (i = 0 ; i<(cpSz/4) ; i++)
        {
            *(cpBufPos++) = *(g_apsBufferCurPos++) ;
        }

        // TODO Call VPP's function that does processing
        // on VPP in case of processing to be made ...

        g_apsFramesPlayedNb++;

        // Half buffer reached
        if (((UINT32)g_apsBufferCurPos >= (UINT32)g_apsBufferStart + (g_apsBufferLength/2))
                && !g_apsMidBufDetected)
        {
            halfBufferReached = TRUE;
            g_apsMidBufDetected = TRUE;
        }
    }

    // Both of them shouldn't happen during the same iteration
    // of this function call.
    APS_ASSERT(!(halfBufferReached & endBufferReached), "Half buffer and End of buffer reached in the same run");
    if (halfBufferReached)
    {
        if (g_apsUserHandler)
        {
            g_apsUserHandler(APS_STATUS_MID_BUFFER_REACHED);
        }
    }

    if (endBufferReached)
    {
        if (g_apsUserHandler)
        {
            g_apsUserHandler(APS_STATUS_END_BUFFER_REACHED);
        }
    }

    APS_PROFILE_FUNCTION_EXIT(aps_XXXSpeechHandler);
}









// =============================================================================
// aps_Setup
// -----------------------------------------------------------------------------
/// Configure the aps service..
///
/// This functions configures the playing of a stream on a given audio interface:
/// gain for the side tone and
/// the speaker, input selection for the microphone and output selection for
/// the speaker... (TODO complete)
///
/// @param itf Interface number of the interface to setup.
/// @param cfg The configuration set applied to the audio interface. See
/// #AUD_LEVEL_T for more details.
/// @return #APS_ERR_NO it can execute the configuration.
// =============================================================================
PUBLIC APS_ERR_T aps_Setup(AUD_ITF_T itf, CONST APS_AUDIO_CFG_T* cfg)
{
    AUD_ERR_T audErr;

    // Set decoder config
    // ...

    // Set audio interface
    g_apsAudioCfg.spkLevel = cfg->spkLevel;
    g_apsAudioCfg.micLevel = AUD_MIC_MUTE;
    g_apsAudioCfg.sideLevel = 0;
    g_apsAudioCfg.toneLevel = 0;

    audErr = aud_Setup(itf, &g_apsAudioCfg);
    switch (audErr)
    {
        // TODO Add error as they comes

        case AUD_ERR_NO:
            return APS_ERR_NO;
            break;

        default:
            return APS_ERR_QTY; // ...unknown error ...
            break;
    }
}







#include "hal_debug.h"
// =============================================================================
// aps_Play
// -----------------------------------------------------------------------------
/// Start the playing of a stream.
///
/// This function  outputs the
/// audio on the audio interface specified as a parameter, on the output selected in
/// the #aps_Setup function. \n
/// In normal operation, when the buffer runs out, the playing will stop. It can
/// loop according to the caller choice. If a handler is defined, it is expected
/// that the handler will refeed data and call #aps_AddedData to tell APS about
/// that. If there is no handler, in loop mode, the buffer will be played in loop
/// without expecting new data.
/// APS can a user function
/// at the middle or at the end of the playback.
///
/// @param itf Interface on which play that stream
/// @param stream Stream to play. Handler called at the middle and end of the buffer
/// are defined there.
/// @param cfg The configuration set applied to the audio interface
/// @param loop \c TRUE if the buffer is played in loop mode.
/// @return #APS_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command, or if VPP is unavailable.
///         #APS_ERR_NO it can execute the command.
// =============================================================================
PUBLIC APS_ERR_T aps_Play(
    CONST AUD_ITF_T      itf,
    CONST APS_ENC_STREAM_T* stream,
    CONST APS_AUDIO_CFG_T*  cfg,
    BOOL loop)
{

    // TODO
    // BIG switch for PCM and/or other codec
    // Initial audio confguration
    HAL_AIF_STREAM_T pcmStream;
    AUD_ERR_T audErr;

    APS_PROFILE_FUNCTION_ENTER(aps_Play);
    APS_TRACE(_PAL| TLEVEL (1), 0, "APS Start");
    APS_TRACE(_PAL| TLEVEL (1), 0, "APS Stream - buffer %#x  %d codec %#x",
              stream->startAddress, stream->length,
              stream->mode);

    // Register global var
    g_apsItf = itf;
    g_apsBufferStart                     = (UINT32*) stream->startAddress;
    g_apsBufferTop                       = (UINT32*)(stream->startAddress + stream->length);
    g_apsBufferCurPos                    = (UINT32*) stream->startAddress;
    g_apsBufferRemainingBytes            = stream->length;
    g_apsBufferLength                    = stream->length;
    g_apsMidBufDetected                  = FALSE;

    g_apsFramesPlayedNb                   = 0;
    g_apsUserHandler                     = stream->handler;

    g_apsAudioCfg.spkLevel               = cfg->spkLevel;
    g_apsAudioCfg.micLevel               = AUD_MIC_MUTE;
    g_apsAudioCfg.sideLevel              = 0;
    g_apsAudioCfg.toneLevel              = 0;

    g_apsLoopMode                        = loop;
    g_apsSpeechUsed                      = FALSE;

    switch (stream->mode)
    {
            HAL_SPEECH_DEC_IN_T decStruct;
            VPP_SPEECH_AUDIO_CFG_T vppCfg;
            UINT32 i;

        case APS_PLAY_MODE_MP3:

            // do not prefill the buffer in MP3 mode
            g_apsBufferStart                     = hal_VocGetDmaiPointer((UINT32*)(stream->startAddress),                  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
            g_apsBufferTop                       = hal_VocGetDmaiPointer((UINT32*)(stream->startAddress + stream->length), HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
            g_apsBufferCurPos                    = hal_VocGetDmaiPointer((UINT32*)(stream->startAddress),                  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
            g_apsBufferRemainingBytes            = 0;

            // TODO : only MP3 8k mono stream is implemented!
            g_apsVppAmjpAudioCfg.mode                 = VPP_AMJP_AUDIO_MODE_MP3;
            g_apsVppAmjpAudioCfg.reset                = 0;
            g_apsVppAmjpAudioCfg.eqType               = -1;
            g_apsVppAmjpAudioCfg.inStreamBufAddrStart = g_apsBufferStart;
            g_apsVppAmjpAudioCfg.inStreamBufAddrEnd   = g_apsBufferTop;
            g_apsVppAmjpAudioCfg.outStreamBufAddr     = g_apsPcmMono8kBuf_0;

            // TODO : only MP3 8k mono stream is implemented!
            pcmStream.startAddress = g_apsPcmMono8kBuf_0;
            pcmStream.length = g_apsPcmMono8kBufSize;
            pcmStream.sampleRate = HAL_AIF_FREQ_8000HZ;
            pcmStream.channelNb = HAL_AIF_MONO;
            pcmStream.voiceQuality = stream->voiceQuality;
            pcmStream.playSyncWithRecord = FALSE;
            // HW event used to wakeup VoC
            pcmStream.halfHandler = NULL;
            pcmStream.endHandler = NULL;

            // Open Amjp
            if (HAL_ERR_RESOURCE_BUSY == vpp_AmjpOpen(aps_VppAmjpHandler, VPP_AMJP_WAKEUP_HW_AUDIO_ONLY))
            {
                return APS_ERR_RESOURCE_BUSY;
            }
            break;

        case APS_PLAY_MODE_PCM:

            g_apsFrameSize = HAL_SPEECH_FRAME_SIZE_PCM_BUF;

            // Typical global vars
            g_apsDecInput = g_apsPcmMono8kBuf_0; //(UINT32*)vpp_SpeechGetRxPcmBuffer();

            // PCM Audio stream, output of VPP
            pcmStream.startAddress = g_apsDecInput;
            pcmStream.length = sizeof(HAL_SPEECH_PCM_BUF_T);
            pcmStream.sampleRate = stream->sampleRate;
            pcmStream.channelNb = stream->channelNb;
            pcmStream.voiceQuality = stream->voiceQuality;
            pcmStream.playSyncWithRecord = FALSE;
            pcmStream.halfHandler = aps_XXXPcmHandler;
            pcmStream.endHandler =  aps_XXXPcmHandler;
            break;

        default:
            // Speech is used
            g_apsSpeechUsed                      = TRUE;

            // vpp speech congiguration structure
            g_apsVppCfg.echoEsOn                 = 0;
            g_apsVppCfg.echoEsVad                = 0;
            g_apsVppCfg.echoEsDtd                = 0;
            g_apsVppCfg.echoExpRel               = 0;
            g_apsVppCfg.echoExpMu                = 0;
            g_apsVppCfg.echoExpMin               = 0;
            g_apsVppCfg.encMute                  = VPP_SPEECH_MUTE;
            g_apsVppCfg.decMute                  = VPP_SPEECH_UNMUTE;
            g_apsVppCfg.sdf                      = NULL;
            g_apsVppCfg.mdf                      = NULL;
            g_apsVppCfg.if1                      = 0;

            decStruct.dtxOn = 0;
            // Codec mode depends on the codec used by the stream
            // and will be set in the switch below
            decStruct.decFrameType = 0;
            decStruct.bfi = 0;
            decStruct.sid = 0;
            decStruct.taf = 0;
            decStruct.ufi = 0;

            // Size is a multiple of 4.
            for (i=0; i<HAL_SPEECH_FRAME_SIZE_COD_BUF/4; i++)
            {
                *((UINT32*)decStruct.decInBuf + i) = 0;
            }

            // AMR mode
            g_apsAmrRing = FALSE;

            // Specific config
            switch (stream->mode)
            {
                case APS_PLAY_MODE_AMR475:
                    g_apsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR475;
                    decStruct.codecMode = HAL_AMR475_DEC;
                    break;

                case APS_PLAY_MODE_AMR515:
                    g_apsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR515;
                    decStruct.codecMode = HAL_AMR515_DEC;
                    break;

                case APS_PLAY_MODE_AMR59:
                    g_apsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR59;
                    decStruct.codecMode = HAL_AMR59_DEC;
                    break;

                case APS_PLAY_MODE_AMR67:
                    g_apsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR67 ;
                    decStruct.codecMode = HAL_AMR67_DEC;
                    break;

                case APS_PLAY_MODE_AMR74:
                    g_apsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR74 ;
                    decStruct.codecMode = HAL_AMR74_DEC;
                    break;

                case APS_PLAY_MODE_AMR795:
                    g_apsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR795;
                    decStruct.codecMode = HAL_AMR795_DEC;
                    break;

                case APS_PLAY_MODE_AMR102:
                    g_apsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR102;
                    decStruct.codecMode = HAL_AMR102_DEC;
                    break;

                case APS_PLAY_MODE_AMR122:
                    g_apsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR122;
                    decStruct.codecMode = HAL_AMR122_DEC;
                    break;

                case APS_PLAY_MODE_AMR_RING:
                    g_apsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR_RING;
                    decStruct.codecMode = HAL_AMR122_DEC;
                    g_apsVppCfg.if1    = 1;
                    g_apsAmrRing = TRUE;
                    break;

                case APS_PLAY_MODE_EFR:
                    g_apsFrameSize = HAL_SPEECH_FRAME_SIZE_EFR  ;
                    decStruct.codecMode = HAL_EFR;
                    break;

                case APS_PLAY_MODE_HR:
                    g_apsFrameSize = HAL_SPEECH_FRAME_SIZE_HR ;
                    decStruct.codecMode = HAL_HR;
                    break;

                case APS_PLAY_MODE_FR:
                    g_apsFrameSize = HAL_SPEECH_FRAME_SIZE_FR;
                    decStruct.codecMode = HAL_FR;
                    break;

                default:
                    APS_ASSERT(FALSE, "Unsupported mode in APS:%d", stream->mode);
            }

            // PCM Audio stream, output of VPP
            pcmStream.startAddress = (UINT32*)vpp_SpeechGetRxPcmBuffer();
            pcmStream.length = sizeof(HAL_SPEECH_PCM_BUF_T);
            pcmStream.sampleRate = HAL_AIF_FREQ_8000HZ;
            pcmStream.channelNb = HAL_AIF_MONO;
            pcmStream.voiceQuality = TRUE;
            pcmStream.playSyncWithRecord = FALSE;
            pcmStream.halfHandler = NULL; // Mechanical interaction with VPP's VOC
            pcmStream.endHandler = NULL; // Mechanical interaction with VPP's VOC

            // VPP audio config
            vppCfg.echoEsOn = 0;
            vppCfg.echoEsVad = 0;
            vppCfg.echoEsDtd = 0;
            vppCfg.echoExpRel = 0;
            vppCfg.echoExpMu = 0;
            vppCfg.echoExpMin = 0;
            vppCfg.encMute = VPP_SPEECH_MUTE;
            vppCfg.decMute = VPP_SPEECH_MUTE;
            vppCfg.sdf = NULL;
            vppCfg.mdf = NULL;
            vppCfg.if1 = g_apsVppCfg.if1;

            // Clear the PCM buffer
            memset(pcmStream.startAddress, 0, pcmStream.length);

            // configure AUD and VPP buffers
            // set vpp
            if (HAL_ERR_RESOURCE_BUSY == vpp_SpeechOpen(aps_VppSpeechHandler, VPP_SPEECH_WAKEUP_HW_DEC))
            {
                return APS_ERR_RESOURCE_BUSY;
            }
            vpp_SpeechAudioCfg(&vppCfg);

            vpp_SpeechSetOutAlgGain(0);

            // Set all the preprocessing modules
            vpp_SpeechSetEncDecPocessingParams(g_apsItf, g_apsAudioCfg.spkLevel, 0);

            // Init the Rx buffer
            *(vpp_SpeechGetRxCodBuffer()) = decStruct;

            // Pointer to the stream buffer in VPP decoding buffer
            g_apsDecInput = (UINT32*) decStruct.decInBuf;

            break;
    }

    // Set audio interface
    audErr = aud_StreamStart(itf, &pcmStream, &g_apsAudioCfg);

    // profit
    switch (audErr)
    {
        case AUD_ERR_NO:
            APS_PROFILE_FUNCTION_EXIT(aps_Play);
            return APS_ERR_NO;
            break; // :)

        default:
            APS_TRACE(_PAL| TLEVEL (1), 0, "APS Start failed !!!");
            APS_PROFILE_FUNCTION_EXIT(aps_Play);
            return APS_ERR_UNKNOWN;
            break; // :)
    }
}



// =============================================================================
// aps_Stop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback.
/// If the function returns
/// #APS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #APS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #APS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC APS_ERR_T aps_Stop(VOID)
{
    APS_PROFILE_FUNCTION_ENTER(aps_Stop);
#ifdef THE_MMI_COMPLY_TO_AUD_SPEC

#else
    aud_Setup(g_apsItf, &g_apsAudioCfg);
#endif
    // stop stream ...
    aud_StreamStop(g_apsItf);

    // VPP Speech is used ?
    if (g_apsSpeechUsed)
    {
        vpp_SpeechClose();
        g_apsSpeechUsed = FALSE;
    }

    // AMJP is used ?
    if (g_apsVppAmjpAudioCfg.mode != VPP_AMJP_AUDIO_MODE_NO)
    {
        vpp_AmjpClose();
    }

    // and reset global state
    g_apsItf = 0;
    g_apsBufferStart = NULL;
    g_apsBufferTop =  NULL;
    g_apsBufferCurPos = NULL;
    g_apsBufferRemainingBytes = 0;
    g_apsBufferLength = 0;

    g_apsFramesPlayedNb = 0;
    g_apsUserHandler = NULL;

    g_apsAudioCfg.spkLevel = AUD_SPK_MUTE;
    g_apsAudioCfg.micLevel = AUD_MIC_MUTE;
    g_apsAudioCfg.sideLevel = 0;
    g_apsAudioCfg.toneLevel = 0;

    g_apsLoopMode = FALSE;

    // TODO : mode dependent switch ...
    g_apsVppCfg.echoEsOn                 = 0;
    g_apsVppCfg.echoEsVad                = 0;
    g_apsVppCfg.echoEsDtd                = 0;
    g_apsVppCfg.echoExpRel               = 0;
    g_apsVppCfg.echoExpMu                = 0;
    g_apsVppCfg.echoExpMin               = 0;
    g_apsVppCfg.encMute = VPP_SPEECH_MUTE;
    g_apsVppCfg.decMute = VPP_SPEECH_MUTE;
    g_apsVppCfg.sdf = NULL;
    g_apsVppCfg.mdf = NULL;
    g_apsVppCfg.if1 = 0;

    // TODO : mode dependent switch ...
    g_apsVppAmjpAudioCfg.mode                 = VPP_AMJP_AUDIO_MODE_NO;
    g_apsVppAmjpAudioCfg.reset                = 0;
    g_apsVppAmjpAudioCfg.eqType               = -1;
    g_apsVppAmjpAudioCfg.inStreamBufAddrStart = g_apsBufferStart;
    g_apsVppAmjpAudioCfg.inStreamBufAddrEnd   = g_apsBufferTop;
    g_apsVppAmjpAudioCfg.outStreamBufAddr     = g_apsPcmMono8kBuf_0;


    g_apsFrameSize = 0;
    g_apsMidBufDetected = FALSE;
    g_apsDecInput = NULL;


    APS_PROFILE_FUNCTION_EXIT(aps_Stop);
    return APS_ERR_NO;
}




// =============================================================================
// aps_Pause
// -----------------------------------------------------------------------------
/// This function pauses the audio stream playback.
/// If the function returns
/// #APS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param pause If \c TRUE, pauses a playing stream. If \c FALSE (and a
/// stream is paused), resumes a paused stream.
/// @return #APS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #APS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC APS_ERR_T aps_Pause(BOOL pause)
{
    APS_PROFILE_FUNCTION_ENTER(aps_Pause);
    aud_StreamPause(g_apsItf, pause);
    APS_PROFILE_FUNCTION_EXIT(aps_Pause);
    return APS_ERR_NO;
}

// =============================================================================
// aps_GetBufPosition
// -----------------------------------------------------------------------------
/// This function returns the current position in the stream buffer.
/// This position points the next bytes to be played.
/// The bytes before this position might not yet have been played at the
/// time this position is read, but are in the process pipe to be played.
///
/// @return The pointer to the next sample to be fetched from the stream buffer.
// =============================================================================
PUBLIC UINT32* aps_GetBufPosition(VOID)
{
    return (g_apsBufferCurPos);
}



// =============================================================================
// aps_AddedData
// -----------------------------------------------------------------------------
/// When a stream buffer is played in loop mode, already played data can be
/// replaced by new ones to play a long song seamlessly. The APS service needs
/// to be informed about the number of new bytes to play in the buffer. This
/// is the role of the #aps_AddedData function.
///
/// It is mandatory to call this function when a stream is played in loop mode
/// with handler defined.
///
/// If the function returns
/// #APS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param addedDataBytes Number of bytes added to the buffer.
/// @return #APS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #APS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC APS_ERR_T aps_AddedData(UINT32 addedDataBytes)
{
    APS_PROFILE_FUNCTION_ENTER(aps_AddedData);
    UINT32 status = hal_SysEnterCriticalSection();
    g_apsBufferRemainingBytes += addedDataBytes;
    hal_SysExitCriticalSection(status);

    APS_PROFILE_FUNCTION_EXIT(aps_AddedData);
    return APS_ERR_NO;
}


