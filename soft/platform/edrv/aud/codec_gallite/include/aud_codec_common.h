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




#ifndef _AUD_CODEC_COMMON_H_
#define _AUD_CODEC_COMMON_H_

#include "cs_types.h"
#include "snd_types.h"
#include "hal_aif.h"
#include "hal_rda_audio.h"
#include "aud_m.h"

// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

//

// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// aud_CodecCommonSetConfig
// -----------------------------------------------------------------------------
/// Set the audio configuration.
/// @param isRecording Whether it is recording stream or not
/// @param user The audio user
/// @param pAifCfg Pointing to the AIF configuration to be set
/// @return TRUE if success, FALSE otherwise
// =============================================================================
PUBLIC BOOL aud_CodecCommonSetConfig(BOOL isRecording, HAL_AUD_USER_T user, CONST HAL_AIF_CONFIG_T *pAifCfg);



// =============================================================================
// aud_CodecCommonSetup
// -----------------------------------------------------------------------------
/// Configure the audio.
///
/// This functions configures an audio interface: gain for the side tone and
/// the speaker, input selection for the microphone and output selection for
/// the speaker.
///
/// @param cfg The configuration set applied to the audio interface. See
/// #AUD_DEVICE_CFG_T for more details.
/// @return #AUD_ERR_NO it can execute the configuration.
// =============================================================================
PUBLIC AUD_ERR_T aud_CodecCommonSetup(SND_ITF_T itf, CONST AUD_DEVICE_CFG_T* cfg);



// =============================================================================
// aud_CodecCommonStreamStart
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
PUBLIC AUD_ERR_T aud_CodecCommonStreamStart(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T *       cfg);



// =============================================================================
// aud_CodecCommonStreamStop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback or/and record.
/// But it doesn't free the audio out resource. If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_CodecCommonStreamStop(SND_ITF_T itf);


// =============================================================================
//  aud_CodecCommonStreamRecordStop
// -----------------------------------------------------------------------------
/// This function stops the audio stream record.
/// But it doesn't free the audio out resource. If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T  aud_CodecCommonStreamRecordStop(SND_ITF_T itf);


// =============================================================================
// aud_CodecCommonStreamPause
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
PUBLIC AUD_ERR_T aud_CodecCommonStreamPause(SND_ITF_T itf, BOOL pause);



// =============================================================================
// aud_CodecCommonStreamRecord
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
PUBLIC AUD_ERR_T aud_CodecCommonStreamRecord(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T *       cfg);





// =============================================================================
// aud_CodecCommonTone
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
/// @param cfg The configuration set applied to the audio interface
/// @param start If \c TRUE, starts the playing of the tone.
///              If \c FALSE, stops the tone.
///
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_CodecCommonTone(
    SND_ITF_T itf,
    CONST SND_TONE_TYPE_T        tone,
    CONST AUD_DEVICE_CFG_T*             cfg,
    CONST BOOL                   start);


// =============================================================================
// aud_CodecCommonTonePause
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
PUBLIC AUD_ERR_T aud_CodecCommonTonePause(SND_ITF_T itf, BOOL pause);



// =============================================================================
// aud_CodecCommonCalibUpdateValues
// -----------------------------------------------------------------------------
/// Update values depending on calibration parameters.
///
/// @param itf AUD interface on which to stop the tone.
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_CodecCommonCalibUpdateValues(SND_ITF_T itf);


// =============================================================================
// aud_CodecCommonGetLatestLevel
// -----------------------------------------------------------------------------
/// get the audio codec Latest level.
// =============================================================================
PUBLIC AUD_LEVEL_T* aud_CodecCommonGetLatestLevel(VOID);

// =============================================================================
// aud_CodecCommonGetOutAlgGainDb2Val
// -----------------------------------------------------------------------------
/// get the audio codec AlgGainDb2Val.
// =============================================================================
PUBLIC INT16 aud_CodecCommonGetOutAlgGainDb2Val(VOID);

PUBLIC VOID aud_CodecCommonOutAlgGainDb2Val(INT8 alg);

PUBLIC BOOL aud_CodecCommonGetVoiceQuality(VOID);

PUBLIC BOOL aud_CodecCommonGetBtNRECFlag(VOID);

PUBLIC BOOL aud_CodecCommonGetForceReceiverMicSelectionFlag(VOID);
#ifdef CHIP_DIE_8955
PUBLIC VOID aud_CodecCommonEnFMRecNewPath(BOOL en);
#endif

#endif // _AUD_CODEC_COMMON_H_


