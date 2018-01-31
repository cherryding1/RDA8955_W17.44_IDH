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

#include "audp_debug.h"
#include "aud_codec_common.h"
#include "aud_tv.h"
#include "sxr_tls.h"


#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifndef TV_USING_I2S
extern BOOL g_halAudLineInSpeakerInUse;
#endif
#endif


// =============================================================================
//  MACROS
// =============================================================================

#define MAX_TV_RETRY_NUM 5

// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

PRIVATE HAL_AIF_SERIAL_CFG_T POSSIBLY_UNUSED g_audTvAifSerialCfg;
PRIVATE HAL_AIF_CONFIG_T POSSIBLY_UNUSED     g_audTvAifCfg;


// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// aud_TvPopulateAifCfg
// -----------------------------------------------------------------------------
/// Populate the AIF configuration structure.
/// @param stream Stream to play or record.
/// @return
// =============================================================================
PRIVATE VOID POSSIBLY_UNUSED aud_TvPopulateAifCfg(CONST HAL_AIF_STREAM_T* stream)
{
#ifdef CHIP_HAS_AP

    g_audTvAifCfg.interface=HAL_AIF_IF_PARALLEL_STEREO;
    g_audTvAifCfg.sampleRate=stream->sampleRate;
    g_audTvAifCfg.channelNb=stream->channelNb;
    g_audTvAifCfg.serialCfg=NULL;

#else // !CHIP_HAS_AP

    // AIF Serial Interface configuration
    g_audTvAifSerialCfg.mode=HAL_SERIAL_MODE_I2S;
    g_audTvAifSerialCfg.aifIsMaster=TRUE;
    g_audTvAifSerialCfg.lsbFirst=FALSE;
    g_audTvAifSerialCfg.polarity=FALSE;
    g_audTvAifSerialCfg.rxDelay=HAL_AIF_RX_DELAY_2;
    g_audTvAifSerialCfg.txDelay=HAL_AIF_TX_DELAY_ALIGN;
    // FM I2S will output the same data on both channels if it is configured in mono mode.
    // AIF I2S has no idea of the actual mono/stereo mode.
    g_audTvAifSerialCfg.rxMode=(stream->channelNb==HAL_AIF_MONO) ?
                               HAL_AIF_RX_MODE_STEREO_MONO_FROM_L :
                               HAL_AIF_RX_MODE_STEREO_STEREO;
    g_audTvAifSerialCfg.txMode=HAL_AIF_TX_MODE_STEREO_STEREO;
    g_audTvAifSerialCfg.fs=stream->sampleRate;
    g_audTvAifSerialCfg.bckLrckRatio=32;
    g_audTvAifSerialCfg.invertBck=TRUE;
    g_audTvAifSerialCfg.outputHalfCycleDelay=FALSE;
    g_audTvAifSerialCfg.inputHalfCycleDelay=FALSE;
    g_audTvAifSerialCfg.enableBckOutGating=FALSE;

    // AIF configuration
    g_audTvAifCfg.interface=HAL_AIF_IF_SERIAL_1_IN_PARALLEL_OUT; // I2S input 1
    g_audTvAifCfg.sampleRate=stream->sampleRate;
    g_audTvAifCfg.channelNb=stream->channelNb;
    g_audTvAifCfg.serialCfg=&g_audTvAifSerialCfg;

#endif // CHIP_HAS_AP
}


// =============================================================================
// aud_TvSetup
// -----------------------------------------------------------------------------
/// Configure the audio.
///
/// This functions configures an audio interface: gain for the side tone and
/// the speaker, input selection for the microphone and output selection for
/// the speaker.
///
/// @param cfg The configuration set applied to the audio interface. See
/// #AUD_DEVICE_CFG_T for more details.
/// @return #AUD_ERR_NO if it can execute the configuration.
// =============================================================================
PUBLIC AUD_ERR_T aud_TvSetup(SND_ITF_T itf, CONST AUD_DEVICE_CFG_T* cfg)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifndef TV_USING_I2S
    if (aud_GetCurOutputDevice() == SND_ITF_LOUD_SPEAKER)
    {
        g_halAudLineInSpeakerInUse = TRUE;
    }
    else
    {
        g_halAudLineInSpeakerInUse = FALSE;
    }
#endif
#endif

    return aud_CodecCommonSetup(itf, cfg);
}


// =============================================================================
// aud_TvStreamStart
// -----------------------------------------------------------------------------
/// Start the playing of a stream.
///
/// This function  outputs the
/// audio on the audio interface specified as a parameter, on the output selected in
/// the #aud_Setup function. \n
/// In normal operation, when
/// the buffer runs out, the playing will wrap at the beginning. Here, there are two ways
/// you can handle your buffer: HAL can call a user function at the middle or
/// at the end of the playback or, depending
/// on your application, you can simply poll the playing state using the "reached half"
/// and "reached end functions" TODO Ask if needed, and then implement !
///
/// @param stream Stream to play. Handler called at the middle and end of the buffer
/// are defined there.
/// @param cfg The configuration set applied to the audio interface
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command,
///         #AUD_ERR_NO it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_TvStreamStart(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T *       cfg)
{
    AUD_ERR_T status;
    BOOL ret;

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifndef TV_USING_I2S
    if (aud_GetCurOutputDevice() == SND_ITF_LOUD_SPEAKER)
    {
        g_halAudLineInSpeakerInUse = TRUE;
    }
    else
    {
        g_halAudLineInSpeakerInUse = FALSE;
    }
#endif
#endif

    HAL_AUD_USER_T user;
    HAL_AIF_CONFIG_T *aifCfg = NULL;

#ifdef TV_USING_I2S
    user = HAL_AUD_USER_CODEC;
    aud_TvPopulateAifCfg(stream);
    aifCfg = &g_audTvAifCfg;
#else // !TV_USING_I2S
#ifdef CHIP_HAS_AP
    user = HAL_AUD_USER_LINEIN_WITH_CODEC;
    aud_TvPopulateAifCfg(stream);
    aifCfg = &g_audTvAifCfg;
#else
    user = HAL_AUD_USER_LINEIN;
#endif
#endif // !TV_USING_I2S

    ret = aud_CodecCommonSetConfig(FALSE, user, aifCfg);
    if (!ret)
    {
        AUD_TRACE(AUD_INFO_TRC, 0,
                  "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
        return AUD_ERR_RESOURCE_BUSY;
    }

    status = aud_CodecCommonStreamStart(itf, stream, cfg);

#ifdef TV_USING_I2S
    if (status == AUD_ERR_NO)
    {
        // Buffer loop back is needed for I2S playing
        status = aud_CodecCommonStreamRecord(itf, stream, cfg);
    }
#endif

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifndef TV_USING_I2S
    if (status != AUD_ERR_NO)
    {
        g_halAudLineInSpeakerInUse = FALSE;
    }
#endif
#endif

    return status;
}


// =============================================================================
// aud_TvStreamStop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback or/and record.
/// But it doesn't free the audio out resource. If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_TvStreamStop(SND_ITF_T itf)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifndef TV_USING_I2S
    g_halAudLineInSpeakerInUse = FALSE;
#endif
#endif

    return aud_CodecCommonStreamStop(itf);
}


// =============================================================================
// aud_TvStreamPause
// -----------------------------------------------------------------------------
/// This function pauses the audio stream playback or/and record.
/// But it doesn't free the audio out resource. If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param pause If \c TRUE, pauses the stream. If \c FALSE, resumes a paused
/// stream.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_TvStreamPause(SND_ITF_T itf, BOOL pause)
{
    return aud_CodecCommonStreamPause(itf, pause);
}


// =============================================================================
// aud_TvStreamRecord
// -----------------------------------------------------------------------------
/// Manage the recording of a stream.
///
///
/// The #aud_StreamRecord send a message to the driver which begins the
/// dataflow from the audio and the compression. If the function returns \c
/// FALSE, it means that the driver cannot handle the record. The value \c len
/// should be the maximum size of the recording buffer and when the buffer is
/// full, the recording will automatically wrap. A
/// user function can be called, either when the middle or the end of the
/// buffer is reached. (TODO add iatus about polling if available)
///
/// @param stream Stream recorded (describe the buffer and the possible interrupt
/// of middle-end buffer handlers.
/// @param cfg The configuration set applied to the audio interface.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver cannot handle the stream,
///         #AUD_ERR_NO if the stream ca be recorded.
// =============================================================================
PUBLIC AUD_ERR_T aud_TvStreamRecord(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T *       cfg)
{
#if 0
    AUD_ERR_T status;
    HAL_AUD_USER_T user;
    SND_ITF_T curOutputDevice;

#ifdef TV_USING_I2S
    user = HAL_AUD_USER_CODEC;
#else // !TV_USING_I2S
#ifdef CHIP_HAS_AP
    user = HAL_AUD_USER_LINEIN_WITH_CODEC;
#else
    user = HAL_AUD_USER_LINEIN;
#endif
#endif // !TV_USING_I2S

    // Save current output device
    curOutputDevice = aud_GetCurOutputDevice();
    // Stop playing
    for (int i=0; i<MAX_TV_RETRY_NUM; i++)
    {
        status = aud_TvStreamStop(itf);
        if (status == AUD_ERR_NO)
        {
            break;
        }
        sxr_Sleep(AUD_TIME_RETRY);
    }
    if (status != AUD_ERR_NO)
    {
        return status;
    }
    // Restore the output device
    aud_SetCurOutputDevice(curOutputDevice);
    // Start to configure recording stream
    aud_TvPopulateAifCfg(stream);

    if (!aud_CodecCommonSetConfig(TRUE, user, &g_audTvAifCfg))
    {
        AUD_TRACE(AUD_INFO_TRC, 0,
                  "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
        return AUD_ERR_RESOURCE_BUSY;
    }

    HAL_AIF_STREAM_T playStream = *stream;
    playStream.halfHandler = NULL;
    playStream.endHandler = NULL;
    status = aud_CodecCommonStreamStart(itf, &playStream, cfg);
    if (status == AUD_ERR_NO)
    {
        status = aud_CodecCommonStreamRecord(itf, stream, cfg);
    }

    return status;

#else

    return AUD_ERR_NO;

#endif
}


// =============================================================================
// aud_TvTone
// -----------------------------------------------------------------------------
//  Manage the playing of a tone: DTMF or Comfort Tone.
///
/// Outputs a DTMF or comfort tone
///
/// When the audio output is enabled, a DTMF or a comfort tones can be output
/// as well. This function starts the output of a tone generated in the audio
/// module. \n
/// You can call this function several times without calling the
/// #aud_ToneStop function or the #aud_TonePause function in
/// between, if you just need to change the attenuation or the tone type. \n
/// If the function returns #AUD_ERR_RESOURCE_BUSY, it means that the driver is
/// busy with an other audio command.
///
/// @param tone The tone to generate
/// @param attenuation The attenuation level of the tone generator
/// @param cfg The configuration set applied to the audio interface
/// @param start If \c TRUE, starts the playing of the tone.
///              If \c FALSE, stops the tone.
///
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_TvTone(
    SND_ITF_T itf,
    CONST SND_TONE_TYPE_T        tone,
    CONST AUD_DEVICE_CFG_T*             cfg,
    CONST BOOL                   start)
{
    return aud_CodecCommonTone(itf, tone, cfg, start);
}



// =============================================================================
// aud_TvTonePause
// -----------------------------------------------------------------------------
/// This function pauses the audio tone.
/// But it doesn't free the audio out resource. If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param pause If \c TRUE, pauses the tone. If \c FALSE, resumes a paused
/// tone.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_TvTonePause(SND_ITF_T itf, BOOL pause)
{
    return aud_CodecCommonTonePause(itf, pause);
}


// =============================================================================
// aud_TvCalibUpdateValues
// -----------------------------------------------------------------------------
/// Update values depending on calibration parameters.
///
/// @param itf AUD interface on which to stop the tone.
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_TvCalibUpdateValues(SND_ITF_T itf)
{
    return aud_CodecCommonCalibUpdateValues(itf);
}


