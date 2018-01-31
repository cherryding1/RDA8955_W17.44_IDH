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
#include "ars_m.h"
#include "arsp_cfg.h"
#include "vpp_speech.h"

#include "calib_m.h"
#include "hal_overlay.h"
#include "vpp_audiojpegenc.h"
#include "vppp_audiojpegenc_common.h"



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
#define ARS_AUD_UNMUTE_TRAME_NB 2

/// Number of trames before unmuting VPP
#define ARS_VPP_UNMUTE_TRAME_NB 2



// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
PRIVATE char *g_pcm2mpeg;
extern UINT32 pcmbuf_overlay[4608];


/// Start address of the stream buffer
PRIVATE UINT32* g_arsBufferStart = NULL;

/// End address of the stream buffer
PRIVATE UINT32 g_arsBufferLength= 0;

/// End address of the stream buffer
PRIVATE UINT32* g_arsBufferTop = NULL;

/// Current position in the buffer. Next read data will be
/// read from here.
PRIVATE UINT32* g_arsBufferCurPos= NULL;



/// Number of bytes of data to read from the buffer
PRIVATE UINT32 g_arsBufferAvailBytes= 0;

/// Pointer to the ouput (encoded) buffer of VPP part of VPP decoding buffer.
PRIVATE UINT32* g_arsEncOutput = NULL;

/// Number of played trames
PRIVATE UINT32 g_arsTramePlayedNb = 0;

/// Number of trames dropped
PRIVATE UINT32 g_arsTrameDroppedNb = 0;

/// User handler to call with play status is stored here.
PRIVATE ARS_USER_HANDLER_T g_arsUserHandler= NULL;


/// Audio configuration used to play the stream in normal state.
/// (ie not when started or stopped.
PRIVATE AUD_LEVEL_T g_arsAudioCfg;


/// VPP configuration used to play the stream in normal state.
/// (ie not when started or stopped.
PRIVATE VPP_SPEECH_AUDIO_CFG_T g_arsVppCfg;

/// Interface used to play the current stream.
PRIVATE AUD_ITF_T g_arsItf= 0;

/// Index in the swap buffer (for PCM) which is the one where to copy
/// the new data
PRIVATE UINT32 g_arsSwIdx= 0;


/// Local buffer (TODO will become voc pcm's buffer)
/*PRIVATE*/ HAL_SPEECH_PCM_BUF_T g_arsPcmBuf;

/// To only pulse the mid buffer
PRIVATE BOOL g_arsMidBufDetected;


/// To stop at the end of the buffer if not in loop mode
PRIVATE BOOL g_arsLoopMode= FALSE;



/// Number of bytes composing one trame for the codec currently
/// used
PRIVATE UINT32 g_arsFrameSize  = 0;
PRIVATE UINT32 g_vocWaiting  = 0;


PRIVATE INT16 g_arsMaxValue  = 0;

PRIVATE INT16 g_arsDigitalGain = 0;

/// SRAM overlay usage flag
PRIVATE BOOL g_arsOverlayLoaded = FALSE;

PRIVATE ARS_REC_MODE_T g_arsStreamMode = ARS_REC_MODE_QTY;
PRIVATE  vpp_AudioJpeg_ENC_IN_T g_arsMpegcfg;

PRIVATE BOOL g_arsEnFMRecNewPath = FALSE;
PRIVATE INT32 g_arsAverageVol = 0;

INT16 ars_getArsAverageVol()
{
    return (INT16)g_arsAverageVol;
}


#ifndef CHIP_HAS_AP
static HAL_SYS_VOC_FREQ_T   g_RecPreVocFreq = HAL_SYS_VOC_FREQ_26M;
#endif //CHIP_HAS_AP


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// ars_VppSpeechHandler
// -----------------------------------------------------------------------------
/// Handler for the vpp module, for speech-kind encoded streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PRIVATE VOID ars_VppSpeechHandler(HAL_VOC_IRQ_STATUS_T* IRQStatus)
{
    ARS_PROFILE_FUNCTION_ENTER(ars_VppSpeechHandler);
    ARS_TRACE(_PAL| TLEVEL (1), 0,"ars_VppSpeechHandler");
    // Number of bytes to copy;
    UINT32 cpSz;
    UINT32 i;

    // Position in the decoding structure where to store
    UINT32* encOutBufPos;

    BOOL    endBufferReached = FALSE;
    BOOL    halfBufferReached = FALSE;

    if(vpp_SpeechGetType()==1)  //For processing the mic signal.(for example "noise suppressing","magic sound" etc.)
    {
        vpp_SpeechScheduleOneFrame(VPP_SPEECH_WAKEUP_SW_ENC);

        return;
    }

    g_arsTramePlayedNb++;
    /*
        if (g_arsTramePlayedNb == ARS_AUD_UNMUTE_TRAME_NB)
        {
            // Unmute AUD
            ARS_TRACE(_PAL| TLEVEL (1), 0, "AUD unmuted");
            aud_Setup(g_arsItf, &g_arsAudioCfg);
        }
    */
    if (g_arsTramePlayedNb == ARS_VPP_UNMUTE_TRAME_NB)
    {
        // Unmute VPP
        ARS_TRACE(_PAL| TLEVEL (1), 0, "VPP unmuted");
        g_arsVppCfg.encMute = VPP_SPEECH_UNMUTE;
        vpp_SpeechAudioCfg(&g_arsVppCfg);

    }

    // Exit the handler immediatly if first interrupt
    if (g_arsTramePlayedNb <= 1)
    {
        ARS_PROFILE_FUNCTION_EXIT(ars_VppSpeechHandler);
        return;
    }

    // g_arsBufferAvailBytes is the number of bytes available
    // for the user to read. The room available in the buffer
    // is then length - g_arsBufferAvailBytes
    if (g_arsBufferLength - g_arsBufferAvailBytes < g_arsFrameSize)
    {
        // Drop the current frame
        g_arsTrameDroppedNb++;
        // This Frame was not played
        g_arsTramePlayedNb--;
        ARS_TRACE(TSTDOUT, 0, "Frame dropped: %d", g_arsTrameDroppedNb);

        // And does nothing else, hoping for better days
    }
    else
    {
        encOutBufPos = (UINT32*) HAL_SYS_GET_UNCACHED_ADDR((UINT32)g_arsEncOutput);

        ///
        if (((UINT32)g_arsBufferCurPos + g_arsFrameSize) > (UINT32)g_arsBufferTop)
        {
            if (!g_arsLoopMode)
            {
                // As stop destroy the global vars ...
                ARS_USER_HANDLER_T handler = g_arsUserHandler;

                // Stop the play
                ars_Stop();

                // End of buffer reached
                if (handler)
                {
                    handler(ARS_STATUS_END_BUFFER_REACHED);
                }

                // Leave
                return;
            }
            // else wrap ...


            // Copy first part
            cpSz = (UINT32)(g_arsBufferTop - g_arsBufferCurPos);
            for (i = 0 ; i<(cpSz/4) ; i++)
            {
                *(g_arsBufferCurPos++) = *(encOutBufPos++);
            }

            g_arsBufferCurPos = g_arsBufferStart;
            cpSz = g_arsFrameSize - cpSz;

            // Buffer End Reached
            endBufferReached = TRUE;
            g_arsMidBufDetected = FALSE;
        }
        else
        {
            cpSz = g_arsFrameSize;
        }

        // Finish or full copy
        for (i = 0 ; i<(cpSz/4) ; i++)
        {
            *(g_arsBufferCurPos++) = *(encOutBufPos++);
        }


        // Half buffer reached
        if (((UINT32)g_arsBufferCurPos >= (UINT32)g_arsBufferStart + (g_arsBufferLength/2))
                && !g_arsMidBufDetected)
        {
            halfBufferReached = TRUE;
            g_arsMidBufDetected = TRUE;
        }

        // Report the new data
        UINT32 status = hal_SysEnterCriticalSection();
        // Decrease size in bytes
        g_arsBufferAvailBytes += g_arsFrameSize;
        hal_SysExitCriticalSection(status);
    }

    // Both of them shouldn't happen during the same iteration
    // of this function call.
    ARS_ASSERT(!(halfBufferReached & endBufferReached), "Half buffer and End of buffer reached in the same run");

    if (halfBufferReached)
    {
        if (g_arsUserHandler)
        {
            g_arsUserHandler(ARS_STATUS_MID_BUFFER_REACHED);
        }
    }

    if (endBufferReached)
    {
        if (g_arsUserHandler)
        {
            g_arsUserHandler(ARS_STATUS_END_BUFFER_REACHED);
        }
    }
    ARS_PROFILE_FUNCTION_EXIT(ars_VppSpeechHandler);
}

PRIVATE VOID ars_MpegVocIsr(HAL_VOC_IRQ_STATUS_T* IRQStatus)
{
    ARS_PROFILE_FUNCTION_ENTER(ars_MpegVocIsr);

    UINT32 cpSz;
    vpp_AudioJpeg_ENC_OUT_T  AudiompegEncStatus;
    BOOL    endBufferReached = FALSE;
    BOOL    halfBufferReached = FALSE;

    g_arsTramePlayedNb++;
    // status check .
    vpp_AudioJpegEncStatus(&AudiompegEncStatus);
    g_arsFrameSize = AudiompegEncStatus.output_len;
    ARS_TRACE(_PAL| TLEVEL (1), 0,"ars_MpegVocIsr framesize:%d",g_arsFrameSize);

    if (g_arsTramePlayedNb <= 1 || AudiompegEncStatus.ErrorStatus != 0)
    {
        ARS_TRACE(_PAL| TLEVEL (1), 0,"First frame or encode failed! "
                  "frame num %d,voc status :%d",g_arsTramePlayedNb,AudiompegEncStatus.ErrorStatus);

        g_vocWaiting = TRUE;
        ARS_PROFILE_FUNCTION_EXIT(ars_MpegVocIsr);
        return;
    }
    // output mp3enc data if necessary.
    if (g_arsBufferLength - g_arsBufferAvailBytes < g_arsFrameSize)
    {
        g_arsTrameDroppedNb++;
        g_arsTramePlayedNb--;
        ARS_TRACE(_PAL| TLEVEL (1), 0, "Frame dropped: %d", g_arsTrameDroppedNb);
    }
    else
    {
        if (((UINT32)g_arsBufferCurPos + g_arsFrameSize) > (UINT32)g_arsBufferTop)
        {

            UINT32 leftspace = (UINT32)g_arsBufferTop - (UINT32)g_arsBufferCurPos;
            cpSz = g_arsFrameSize - leftspace;
            g_arsBufferCurPos = g_arsBufferStart;
            memcpy(g_arsBufferCurPos,g_arsBufferTop,cpSz);
            g_arsBufferCurPos +=cpSz/4;
            endBufferReached = TRUE;
            g_arsMidBufDetected = FALSE;
        }
        else
        {
            g_arsBufferCurPos =g_arsBufferCurPos + g_arsFrameSize/4;
        }


        g_arsMpegcfg.outStreamBufAddr = g_arsBufferCurPos;

        if (((UINT32)g_arsBufferCurPos >= (UINT32)g_arsBufferStart + (g_arsBufferLength/2))
                && !g_arsMidBufDetected)
        {
            halfBufferReached = TRUE;
            g_arsMidBufDetected = TRUE;
        }


        UINT32 status = hal_SysEnterCriticalSection();

        g_arsBufferAvailBytes += g_arsFrameSize;
        hal_SysExitCriticalSection(status);
    }

    ARS_ASSERT(!(halfBufferReached & endBufferReached), "Half buffer and End of buffer reached in the same run");

    if (halfBufferReached)
    {
        if (g_arsUserHandler)
        {
            g_arsUserHandler(ARS_STATUS_MID_BUFFER_REACHED);
        }
    }

    if (endBufferReached)
    {
        if (g_arsUserHandler)
        {
            g_arsUserHandler(ARS_STATUS_END_BUFFER_REACHED);
        }
    }

    g_vocWaiting = TRUE;
    ARS_PROFILE_FUNCTION_EXIT(ars_MpegVocIsr);
    return;
}
PRIVATE VOID ars_HalfPcmHandler(VOID)
{

    ARS_TRACE(_PAL| TLEVEL (1), 0,"ars_HalfPcmHandler %d",g_vocWaiting);
    ARS_ASSERT((g_vocWaiting == TRUE),"VOC is busy now");

    g_arsMpegcfg.inStreamBufAddr = g_pcm2mpeg;
    vpp_AudioJpegEncScheduleOneFrame(&g_arsMpegcfg);
    g_vocWaiting = FALSE;// voc is running
    return;
}

PRIVATE VOID ars_EndPcmHandler(VOID)
{

    ARS_TRACE(_PAL| TLEVEL (1), 0,"ars_EndPcmHandler,%d",g_vocWaiting);
    ARS_ASSERT((g_vocWaiting == TRUE),"VOC is busy now");

    g_arsMpegcfg.inStreamBufAddr = g_pcm2mpeg +1152*2*g_arsMpegcfg.channel_num;
    vpp_AudioJpegEncScheduleOneFrame(&g_arsMpegcfg);
    g_vocWaiting = FALSE;//voc is running
    return;
}

PRIVATE VOID ars_AmrHandler(VOID)
{
    ARS_TRACE(_PAL| TLEVEL (1), 0,"ars_AmrHandler");
    INT16* SampleValue;
    U16 i;
    SampleValue=(INT16* )vpp_SpeechGetTxPcmBuffer();
    g_arsAverageVol = 0;
    for (i = 0 ; i<(g_arsFrameSize/2) ; i++)
    {
        if(SampleValue[i]>g_arsMaxValue)
        {
            SampleValue[i]=g_arsMaxValue;
        }
        else if(SampleValue[i]<-g_arsMaxValue)
        {
            SampleValue[i]=-g_arsMaxValue;
        }
        SampleValue[i]=SampleValue[i]*g_arsDigitalGain;
        g_arsAverageVol = (g_arsAverageVol +ABS( SampleValue[i]))>>1;
    }
    vpp_SpeechScheduleOneFrame(VPP_SPEECH_WAKEUP_SW_ENC);
    return;
}


// =============================================================================
// ars_XXXPcmHandler
// -----------------------------------------------------------------------------
/// Handler for the XXX module, to play PCM  streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PRIVATE VOID ars_XXXPcmHandler(VOID)
{
    // Number of bytes to copy;
    UINT32 cpSz;
    UINT32 i;

    // Position in the copy buffer, where data from the stream
    // are going to be copied
    UINT32* cpBufPos;

    INT16* SampleValue;

    BOOL    endBufferReached = FALSE;
    BOOL    halfBufferReached = FALSE;

    ARS_PROFILE_FUNCTION_ENTER(ars_XXXSpeechHandler);

    g_arsTramePlayedNb++;

    /// Special operation according to state
    ///
    /// if (sampleNumber == ...)
    /// ...
    /// ...
    /*
       if (g_arsTramePlayedNb == ARS_AUD_UNMUTE_TRAME_NB)
       {
           // Unmute AUD
           aud_Setup(g_arsItf, &g_arsAudioCfg);
       }
    */
    if (g_arsTramePlayedNb == ARS_VPP_UNMUTE_TRAME_NB)
    {
        // Unmute VPP
        g_arsVppCfg.encMute = VPP_SPEECH_UNMUTE;
        vpp_SpeechAudioCfg(&g_arsVppCfg);
    }

    // Exit the handler immediatly if first interrupt
    if (g_arsTramePlayedNb <= 1)
    {
        ARS_PROFILE_FUNCTION_EXIT(ars_VppSpeechHandler);
        return;
    }

    // g_arsBufferAvailBytes is the number of bytes available
    // for the user to read. The room available in the buffer
    // is then length - g_arsBufferAvailBytes
    if (g_arsBufferLength - g_arsBufferAvailBytes < g_arsFrameSize)
    {
        // Drop the current frame
        g_arsTrameDroppedNb++;
        // This Frame was not played
        g_arsTramePlayedNb--;
        ARS_TRACE(TSTDOUT, 0, "Frame dropped: %d", g_arsTrameDroppedNb);

        // And does nothing else, hoping for better days
    }
    else
    {

        if (g_arsSwIdx == 0)
        {
            cpBufPos =(UINT32*) HAL_SYS_GET_UNCACHED_ADDR((UINT32)&g_arsPcmBuf.pcmBuf[0]);// g_arsEncOutput;
            g_arsSwIdx = 1;
        }
        else
        {
            // == 1
            cpBufPos = (UINT32*) HAL_SYS_GET_UNCACHED_ADDR((UINT32)&g_arsPcmBuf.pcmBuf[1]);// g_arsEncOutput;
            g_arsSwIdx = 0;
        }


#ifdef CHIP_DIE_8955
        if(!g_arsEnFMRecNewPath)
        {
#endif
            SampleValue=(INT16* )cpBufPos;
            g_arsAverageVol = 0;
            for (i = 0 ; i<(g_arsFrameSize/2) ; i++)
            {
                if(SampleValue[i]>g_arsMaxValue)
                {
                    SampleValue[i]=g_arsMaxValue;
                }
                else if(SampleValue[i]<-g_arsMaxValue)
                {
                    SampleValue[i]=-g_arsMaxValue;
                }

                SampleValue[i]=SampleValue[i]*g_arsDigitalGain;
                g_arsAverageVol = (g_arsAverageVol +ABS( SampleValue[i]))>>1;
            }
#ifdef CHIP_DIE_8955
        }

#endif



        ///
        if ((((UINT32)g_arsBufferCurPos) + (g_arsFrameSize)) > (UINT32)g_arsBufferTop)
        {
            if (!g_arsLoopMode)
            {
                // As stop destroy the global vars ...
                ARS_USER_HANDLER_T handler = g_arsUserHandler;

                // Stop the play
                ars_Stop();

                // End of buffer reached
                if (handler)
                {
                    handler(ARS_STATUS_END_BUFFER_REACHED);
                }

                // Leave
                return;
            }
            // else wrap ...


            // Copy first part
            cpSz = (UINT32)(g_arsBufferTop - g_arsBufferCurPos);
            for (i = 0 ; i<(cpSz/4) ; i++)
            {
                *(g_arsBufferCurPos++) = *(cpBufPos++);
            }

            g_arsBufferCurPos = g_arsBufferStart;
            cpSz = g_arsFrameSize - cpSz;

            // Buffer End Reached
            endBufferReached = TRUE;
            g_arsMidBufDetected = FALSE;
        }
        else
        {
            cpSz = g_arsFrameSize;
        }

        // Finish or full copy
        for (i = 0 ; i<(cpSz/4) ; i++)
        {
            *(g_arsBufferCurPos++) = *(cpBufPos++);
        }


        // Half buffer reached
        if (((UINT32)g_arsBufferCurPos >= (UINT32)g_arsBufferStart + (g_arsBufferLength/2))
                && !g_arsMidBufDetected)
        {
            halfBufferReached = TRUE;
            g_arsMidBufDetected = TRUE;
        }

        // Report the new data
        UINT32 status = hal_SysEnterCriticalSection();
        // Decrease size in bytes
        g_arsBufferAvailBytes += g_arsFrameSize;
        hal_SysExitCriticalSection(status);
    }

    // Both of them shouldn't happen during the same iteration
    // of this function call.
    ARS_ASSERT(!(halfBufferReached & endBufferReached), "Half buffer and End of buffer reached in the same run");

    if (halfBufferReached)
    {
        if (g_arsUserHandler)
        {
            g_arsUserHandler(ARS_STATUS_MID_BUFFER_REACHED);
        }
    }

    if (endBufferReached)
    {
        if (g_arsUserHandler)
        {
            g_arsUserHandler(ARS_STATUS_END_BUFFER_REACHED);
        }
    }
    ARS_PROFILE_FUNCTION_EXIT(ars_XXXSpeechHandler);
}









// =============================================================================
// ars_Setup
// -----------------------------------------------------------------------------
/// Configure the ars service..
///
/// This functions configures the playing of a stream on a given audio interface:
/// gain for the side tone and
/// the speaker, input selection for the microphone and output selection for
/// the speaker... (TODO complete)
///
/// @param itf Interface number of the interface to setup.
/// @param cfg The configuration set applied to the audio interface. See
/// #AUD_LEVEL_T for more details.
/// @return #ARS_ERR_NO it can execute the configuration.
// =============================================================================
PUBLIC ARS_ERR_T ars_Setup(AUD_ITF_T itf, CONST ARS_AUDIO_CFG_T* cfg)
{
    AUD_ERR_T audErr;
    INT32 alg;

    CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();

    // Set decoder config
    // ...

    // Set audio interface
    g_arsAudioCfg.spkLevel = AUD_SPK_MUTE;
    g_arsAudioCfg.micLevel = cfg->micLevel;
    g_arsAudioCfg.sideLevel = 0;
    g_arsAudioCfg.toneLevel = 0;

    if (cfg->micLevel == SND_MIC_MUTE)
    {
        alg = CALIB_AUDIO_GAIN_VALUE_MUTE;
    }
    else if (cfg->micLevel == SND_MIC_VOL_CALL)
    {
        alg = calibPtr->bb->audio[itf].audioGains.inGainsCall.alg;
    }
    else
    {
        alg = calibPtr->bb->audio[itf].audioGains.inGainsRecord.alg;
    }

    g_arsDigitalGain=vpp_SpeechInAlgGainDb2Val(alg)/0x60;

    if(g_arsDigitalGain!=0)
    {
        g_arsMaxValue=32700/g_arsDigitalGain;
    }
    else
    {
        g_arsMaxValue=0;
    }

    //Set Mic Gain in VoC
    vpp_SpeechSetInAlgGain(alg);

    // Set all the preprocessing modules
    vpp_SpeechSetEncDecPocessingParams(itf, g_arsAudioCfg.spkLevel, 0);

    audErr = aud_Setup(itf, &g_arsAudioCfg);
    switch (audErr)
    {
        // TODO Add error as they comes

        case AUD_ERR_NO:
            return ARS_ERR_NO;
            break;

        default:
            return ARS_ERR_QTY; // ...unknown error ...
            break;
    }
}



// =============================================================================
// ars_Record
// -----------------------------------------------------------------------------
/// Start the playing of a stream.
///
/// This function  outputs the
/// audio on the audio interface specified as a parameter, on the output selected in
/// the #ars_Setup function. \n
/// In normal operation, when the buffer runs out, the playing will stop. It can
/// loop according to the caller choice.
/// ARS can call a user function
/// at the middle or at the end of the playback.
///
/// @param itf Interface on which play that stream
/// @param stream Stream to play. Handler called at the middle and end of the buffer
/// are defined there.
/// @param cfg The configuration set applied to the audio interface
/// @param loop If \c TRUE, the buffer is recorded in loop mode.
/// @return #ARS_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command, or if VPP is unavailable.
///         #ARS_ERR_NO it can execute the command.
// =============================================================================
PUBLIC ARS_ERR_T ars_Record(
    CONST AUD_ITF_T      itf,
    CONST ARS_ENC_STREAM_T* stream,
    CONST ARS_AUDIO_CFG_T*  cfg,
    BOOL loop)
{
    ARS_PROFILE_FUNCTION_ENTER(ars_Play);
    ARS_TRACE(_PAL| TLEVEL (1), 0, "ARS Start codec %d",stream->mode);
    // TODO
    // BIG switch for PCM and/or other codec
    // Initial audio confguration
    AUD_LEVEL_T audioCfg;
    AUD_ERR_T audErr;
    HAL_AIF_STREAM_T pcmStream;

    CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();

    // VPP encoding buffer pointer.
    CONST HAL_SPEECH_ENC_OUT_T* vppEncBuf;

    // The decoding buffer is used to configure VPP
    HAL_SPEECH_DEC_IN_T* vppDecBuf;
    INT32 alg;

    if (cfg->micLevel == SND_MIC_MUTE)
    {
        alg = CALIB_AUDIO_GAIN_VALUE_MUTE;
    }
    else if (cfg->micLevel == SND_MIC_VOL_CALL)
    {
        alg = calibPtr->bb->audio[itf].audioGains.inGainsCall.alg;
    }
    else
    {
        alg = calibPtr->bb->audio[itf].audioGains.inGainsRecord.alg;
    }

    g_arsDigitalGain=vpp_SpeechInAlgGainDb2Val(alg)/0x60;
    if(g_arsDigitalGain!=0)
    {
        g_arsMaxValue=32700/g_arsDigitalGain;
    }
    else
    {
        g_arsDigitalGain=1;
        g_arsMaxValue=32767;
    }

    ARS_TRACE(_PAL| TLEVEL (1), 0, "ARS Stream - g_arsDigitalGain %d  g_arsMaxValue %d", g_arsDigitalGain,g_arsMaxValue);

    // Register global var
    g_arsItf = itf;
    g_arsBufferStart = (UINT32*) stream->startAddress;
    g_arsBufferTop = (UINT32*)(stream->startAddress + stream->length);
    g_arsBufferCurPos =(UINT32*) stream->startAddress;
    g_arsBufferAvailBytes = 0;
    g_arsBufferLength = stream->length;
    g_arsMidBufDetected = FALSE;

    g_arsTramePlayedNb = 0;
    g_arsTrameDroppedNb = 0;
    g_arsUserHandler = stream->handler;

    g_arsAudioCfg.spkLevel = cfg->spkLevel;
    g_arsAudioCfg.micLevel = cfg->micLevel;
    g_arsAudioCfg.sideLevel = 0;
    g_arsAudioCfg.toneLevel = 0;
    g_arsLoopMode = loop;
    g_arsStreamMode = stream->mode;

    // Set audio interface
    audioCfg.spkLevel = cfg->spkLevel;
    audioCfg.micLevel = cfg->micLevel;
    audioCfg.sideLevel = 0;
    audioCfg.toneLevel = 0;

    switch (stream->mode)
    {
        case ARS_REC_MODE_PCM:
        {

            // Typical global vars
            g_arsEncOutput = (UINT32*)&g_arsPcmBuf.pcmBuf[0][0];
            g_arsFrameSize = HAL_SPEECH_FRAME_SIZE_PCM_BUF;

            // PCM Audio stream, output of VPP
            pcmStream.startAddress = (UINT32*)&g_arsPcmBuf.pcmBuf[0][0];
            pcmStream.length = sizeof(HAL_SPEECH_PCM_BUF_T);
            pcmStream.sampleRate = stream->sampleRate;
            pcmStream.channelNb = stream->channelNb;
            pcmStream.voiceQuality = stream->voiceQuality;
            pcmStream.playSyncWithRecord = FALSE;
            pcmStream.halfHandler = ars_XXXPcmHandler;
            pcmStream.endHandler =  ars_XXXPcmHandler;
            break;
        }
        case ARS_REC_MODE_DAF:
        {
            //alloc pcm buf .
            if (hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_2) != HAL_ERR_NO)
            {
                return ARS_ERR_RESOURCE_BUSY;
            }

#ifndef CHIP_HAS_AP
            g_RecPreVocFreq = hal_SysSetVocClock(HAL_SYS_VOC_FREQ_208M);
#endif //CHIP_HAS_AP

            g_arsOverlayLoaded = TRUE;
            g_pcm2mpeg = pcmbuf_overlay;//(char*)mmc_MemMalloc(1152*4);//if lack of memory,try to use pcmbuf_overlay.

            // dsp config
            g_arsMpegcfg.outStreamBufAddr = stream->startAddress;
            g_arsMpegcfg.inStreamBufAddr = g_pcm2mpeg;
            g_arsMpegcfg.mode = 0;//mp3enc:0
            g_arsMpegcfg.reset = 1;
            g_arsMpegcfg.BitRate = 128;

            g_arsMpegcfg.SampleRate = stream->sampleRate;
            g_arsMpegcfg.channel_num = stream->channelNb;

            if (HAL_ERR_RESOURCE_BUSY == vpp_AudioJpegEncOpen(ars_MpegVocIsr))
            {
#ifndef CHIP_HAS_AP
                if(g_RecPreVocFreq > HAL_SYS_VOC_FREQ_26M)
                    hal_SysSetVocClock(g_RecPreVocFreq);
#endif //CHIP_HAS_AP
                ARS_TRACE(_PAL| TLEVEL (1), 0,"Failed open voc");
                return HAL_ERR_RESOURCE_BUSY;
            }

            //audio card config
            pcmStream.startAddress = (UINT32*)g_arsMpegcfg.inStreamBufAddr;
            pcmStream.length =1152*2*2*stream->channelNb;
            pcmStream.sampleRate = stream->sampleRate;
            pcmStream.channelNb = stream->channelNb;
            pcmStream.voiceQuality = stream->voiceQuality;
            pcmStream.playSyncWithRecord = FALSE;
            pcmStream.halfHandler = ars_HalfPcmHandler;
            pcmStream.endHandler =  ars_EndPcmHandler;
            break;
        }

        default:

            if (hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_5) != HAL_ERR_NO)
            {
                return ARS_ERR_RESOURCE_BUSY;
            }
            g_arsOverlayLoaded = TRUE;

            // ---------------
            // open vpp speech
            // ---------------

            if (HAL_ERR_RESOURCE_BUSY == vpp_SpeechOpen(ars_VppSpeechHandler, VPP_SPEECH_WAKEUP_SW_ENC))
            {
                return ARS_ERR_RESOURCE_BUSY;
            }

            // ---------------
            // dec (mode) buf
            // initialization
            // ---------------

            // init pointers to voc RAM
            vppDecBuf = vpp_SpeechGetRxCodBuffer();

            vppDecBuf->dtxOn = 0;
            vppDecBuf->decFrameType = 0;
            vppDecBuf->bfi = 0;
            vppDecBuf->sid = 0;
            vppDecBuf->taf = 0;
            vppDecBuf->ufi = 0;

            // mode-specific config
            switch (stream->mode)
            {
                case ARS_REC_MODE_AMR475:
                    g_arsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR475;
                    vppDecBuf->codecMode = HAL_AMR475_ENC;
                    break;

                case ARS_REC_MODE_AMR515:
                    g_arsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR515;
                    vppDecBuf->codecMode = HAL_AMR515_ENC;
                    break;

                case ARS_REC_MODE_AMR59:
                    g_arsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR59;
                    vppDecBuf->codecMode = HAL_AMR59_ENC;
                    break;

                case ARS_REC_MODE_AMR67:
                    g_arsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR67 ;
                    vppDecBuf->codecMode = HAL_AMR67_ENC;
                    break;

                case ARS_REC_MODE_AMR74:
                    g_arsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR74 ;
                    vppDecBuf->codecMode = HAL_AMR74_ENC;
                    break;

                case ARS_REC_MODE_AMR795:
                    g_arsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR795;
                    vppDecBuf->codecMode = HAL_AMR795_ENC;
                    break;

                case ARS_REC_MODE_AMR102:
                    g_arsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR102;
                    vppDecBuf->codecMode = HAL_AMR102_ENC;
                    break;

                case ARS_REC_MODE_AMR122:
                    g_arsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR122;
                    vppDecBuf->codecMode = HAL_AMR122_ENC & HAL_AMR122_DEC;
                    break;

                case ARS_REC_MODE_AMR_RING:
                    g_arsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR_RING;
                    vppDecBuf->codecMode = HAL_AMR122_ENC & HAL_AMR122_DEC;
                    g_arsVppCfg.if1 = 1;
                    break;

                case ARS_REC_MODE_EFR:
                    g_arsFrameSize = HAL_SPEECH_FRAME_SIZE_EFR  ;
                    vppDecBuf->codecMode = HAL_EFR;
                    break;

                case ARS_REC_MODE_HR:
                    g_arsFrameSize = HAL_SPEECH_FRAME_SIZE_HR ;
                    vppDecBuf->codecMode = HAL_HR;
                    break;

                case ARS_REC_MODE_FR:
                    g_arsFrameSize = HAL_SPEECH_FRAME_SIZE_FR;
                    vppDecBuf->codecMode = HAL_FR;
                    break;

                default:
                    ARS_ASSERT(FALSE, "Unsupported mode in ARS");
            }

            // ---------------
            // enc (rec) buf
            // initialization
            // ---------------

            vppEncBuf = vpp_SpeechGetTxCodBuffer();

            // Pointer from which to get the encoded data.
            g_arsEncOutput = (UINT32*) vppEncBuf->encOutBuf;

            // FIXME
            UINT32 ij;
            for (ij=0; ij<8 ; ij++)
            {
                g_arsEncOutput[ij] = 0;
            }

            // ---------------
            // VPP audio config
            // ---------------

            // TODO Improve ...
            g_arsVppCfg.echoEsOn = 0;
            g_arsVppCfg.echoEsVad = 0;
            g_arsVppCfg.echoEsDtd = 0;
            g_arsVppCfg.echoExpRel = 0;
            g_arsVppCfg.echoExpMu = 0;
            g_arsVppCfg.echoExpMin = 0;
            g_arsVppCfg.encMute = VPP_SPEECH_MUTE;
            g_arsVppCfg.decMute = VPP_SPEECH_MUTE;
            g_arsVppCfg.sdf = NULL;
            g_arsVppCfg.mdf = NULL;

            vpp_SpeechAudioCfg(&g_arsVppCfg);
            //Set Mic Gain in VoC
            vpp_SpeechSetInAlgGain(alg);

            // Set all the preprocessing modules
            vpp_SpeechSetEncDecPocessingParams(g_arsItf, g_arsAudioCfg.spkLevel, 0);

            // ---------------
            // AUD audio config
            // ---------------

            // PCM Audio stream, input of VPP
            pcmStream.startAddress = (UINT32*)vpp_SpeechGetTxPcmBuffer();
            pcmStream.length = sizeof(HAL_SPEECH_PCM_BUF_T);
            pcmStream.sampleRate = HAL_AIF_FREQ_8000HZ;
            pcmStream.channelNb = HAL_AIF_MONO;
            pcmStream.voiceQuality = TRUE;
            pcmStream.playSyncWithRecord = FALSE;
            pcmStream.halfHandler = ars_AmrHandler; // Mechanical interaction with VPP's VOC
            pcmStream.endHandler = ars_AmrHandler; // Mechanical interaction with VPP's VOC

            break;
    }
    // ...d
    ARS_TRACE(_PAL| TLEVEL (1), 0, "pcmStream - buffer:%#x length:%d "
              "samplerate:%d voiceQuality:%d,ch:%d",
              pcmStream.startAddress, pcmStream.length,
              pcmStream.sampleRate,pcmStream.voiceQuality,
              pcmStream.channelNb);

    audErr = aud_StreamRecord(itf, &pcmStream, &audioCfg);

    // profit
    switch (audErr)
    {
        case AUD_ERR_NO:
            ARS_PROFILE_FUNCTION_EXIT(ars_Play);
            return ARS_ERR_NO;
            break; // :)

        default:
#ifndef CHIP_HAS_AP
            if(g_RecPreVocFreq > HAL_SYS_VOC_FREQ_26M)
                hal_SysSetVocClock(g_RecPreVocFreq);
#endif //CHIP_HAS_AP
            ARS_TRACE(_PAL| TLEVEL (1), 0, "ARS Start failed !!!");
            ARS_PROFILE_FUNCTION_EXIT(ars_Play);
            return ARS_ERR_UNKNOWN;
            break; // :)
    }
}



// =============================================================================
// ars_Stop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback.
/// If the function returns
/// #ARS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #ARS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #ARS_ERR_NO if it can execute the command.
// =============================================================================
#ifndef CHIP_HAS_AP
extern PUBLIC BOOL GetIsEarphonePlugIn(void);
#endif
PUBLIC ARS_ERR_T ars_Stop(VOID)
{
    ARS_PROFILE_FUNCTION_ENTER(ars_Stop);
    // stop stream ...
#ifndef CHIP_HAS_AP
    if((g_arsItf==0)||(g_arsItf==1))
    {
        if(GetIsEarphonePlugIn())
            g_arsItf = 1;
        else
            g_arsItf = 0;
    }
#endif

    aud_StreamStop(g_arsItf);

    if (g_arsStreamMode < ARS_REC_MODE_PCM)
    {

        vpp_SpeechClose();
        if (g_arsOverlayLoaded)
        {
            hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_5);
            g_arsOverlayLoaded = FALSE;
        }
    }
    else if (g_arsStreamMode == ARS_REC_MODE_DAF)
    {

        vpp_AudioJpegEncClose();
        if (g_arsOverlayLoaded)
        {
            hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);
            g_arsOverlayLoaded = FALSE;
        }
    }

    // and reset global state
    g_arsItf = 0;
    g_arsBufferStart = NULL;
    g_arsBufferTop =  NULL;
    g_arsBufferCurPos = NULL;
    g_arsBufferAvailBytes = 0;
    g_arsBufferLength = 0;

    g_arsTramePlayedNb = 0;
    g_arsTrameDroppedNb = 0;
    g_arsUserHandler = NULL;
    g_arsLoopMode = FALSE;
    g_arsFrameSize = 0;

    memset(&g_arsVppCfg,0,sizeof(VPP_SPEECH_AUDIO_CFG_T));
    memset(&g_arsAudioCfg,0,sizeof(AUD_LEVEL_T));
    memset(&g_arsMpegcfg,0,sizeof(vpp_AudioJpeg_ENC_IN_T));
    ARS_PROFILE_FUNCTION_EXIT(ars_Stop);
    return ARS_ERR_NO;
}


// =============================================================================
// ars_RecordStop
// -----------------------------------------------------------------------------
/// This function stops the audio stream record.
/// If the function returns
/// #ARS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #ARS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #ARS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC ARS_ERR_T ars_RecordStop(VOID)
{
    ARS_PROFILE_FUNCTION_ENTER(ars_RecordStop);
    // stop stream ...
    aud_StreamRecordStop(g_arsItf);

    if (g_arsStreamMode < ARS_REC_MODE_PCM)
    {

        vpp_SpeechClose();
        if (g_arsOverlayLoaded)
        {
            hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_5);
            g_arsOverlayLoaded = FALSE;
        }
    }
    else if (g_arsStreamMode == ARS_REC_MODE_DAF)
    {

        vpp_AudioJpegEncClose();
        if (g_arsOverlayLoaded)
        {
            hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);
            g_arsOverlayLoaded = FALSE;
        }
#ifndef CHIP_HAS_AP
        if(g_RecPreVocFreq > HAL_SYS_VOC_FREQ_26M)
            hal_SysSetVocClock(g_RecPreVocFreq);
#endif //CHIP_HAS_AP
    }

    // and reset global state
    g_arsItf = 0;
    g_arsBufferStart = NULL;
    g_arsBufferTop =  NULL;
    g_arsBufferCurPos = NULL;
    g_arsBufferAvailBytes = 0;
    g_arsBufferLength = 0;

    g_arsTramePlayedNb = 0;
    g_arsTrameDroppedNb = 0;
    g_arsUserHandler = NULL;
    g_arsLoopMode = FALSE;
    g_arsFrameSize = 0;

    memset(&g_arsVppCfg,0,sizeof(VPP_SPEECH_AUDIO_CFG_T));
    memset(&g_arsAudioCfg,0,sizeof(AUD_LEVEL_T));
    memset(&g_arsMpegcfg,0,sizeof(vpp_AudioJpeg_ENC_IN_T));
    g_arsFrameSize = 0;

    ARS_PROFILE_FUNCTION_EXIT(ars_RecordStop);
    return ARS_ERR_NO;
}

// =============================================================================
// ars_Pause
// -----------------------------------------------------------------------------
/// This function pauses the audio stream playback.
/// If the function returns
/// #ARS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param pause If \c TRUE, pauses a playing stream. If \c FALSE (and a
/// stream is paused), resumes a paused stream.
/// @return #ARS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #ARS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC ARS_ERR_T ars_Pause(BOOL pause)
{
    aud_StreamPause(g_arsItf, pause);
    return ARS_ERR_NO;
}

// =============================================================================
// ars_GetBufPosition
// -----------------------------------------------------------------------------
/// This function returns the current position in the stream buffer.
/// This position points the next bytes to be played.
/// The bytes before this position might not yet have been played at the
/// time this position is read, but are in the process pipe to be played.
///
/// @return The pointer to the next sample to be fetched from the stream buffer.
// =============================================================================
PUBLIC UINT32* ars_GetBufPosition(VOID)
{
    return (g_arsBufferCurPos);
}



// =============================================================================
// ars_ReadData
// -----------------------------------------------------------------------------
/// When a stream buffer is recorded, already recorded data can be replaced by new
/// ones to record a long song seamlessly. The ARS service needs to be informed
/// about the number of new bytes available in the buffer. This is the role of the
/// #ars_ReadData function.
///
/// If the function returns
/// #ARS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param readDataBytes Number of bytes read from the buffer.
/// @return #ARS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #ARS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC ARS_ERR_T ars_ReadData(UINT32 readDataBytes)
{
    ARS_PROFILE_FUNCTION_ENTER(ars_ReadData);
    UINT32 status = hal_SysEnterCriticalSection();
    // nota : g_arsBufferAvailBytes is the number of bytes in the
    // buffer available for reading
    g_arsBufferAvailBytes -= readDataBytes;
    hal_SysExitCriticalSection(status);
    ARS_PROFILE_FUNCTION_EXIT(ars_ReadData);
    return ARS_ERR_NO;
}




// =============================================================================
// ars_AvailableData
// -----------------------------------------------------------------------------
/// @return The number of bytes ready to be read in the buffer.
///
// =============================================================================
PUBLIC UINT32 ars_AvailableData(VOID)
{
    return (g_arsBufferAvailBytes);
}

#ifdef CHIP_DIE_8955
PUBLIC VOID ars_EnFMRecNewPath(BOOL en)
{
    g_arsEnFMRecNewPath = en;
    aud_EnFMRecNewPath(en);
}
#endif


