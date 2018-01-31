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



#ifndef _AUD_M_H_
#define _AUD_M_H_

#include "cs_types.h"
#include "snd_types.h"
// TODO: remove once all types has been changed cleanly in all branches
#include "aud_snd_types.h"

#include "hal_aif.h"


/// @defgroup aud EDRV Audio Driver (AUD)
/// @{
///
/// This document describes the characteristics of the  Audio
/// Driver and how to use it via its API.
///
/// This audio driver allows you to:
///  - Record an audio stream in PCM;
///  - Play an audio stream in PCM;
///  - Manage all the audio configuration (gain, in/out selection...)
///  - Generate Comfort Tone or DTMF generation, even while playing
/// a PCM stream.
///  .
///
/// @par Organization
/// The AUD Audio Driver allows the use of several Audio Interfaces on the
/// same Mobile Station, but not at the same time. As though using different
/// Audio Interface at the same time might not prove itself very useful.
/// These interface can be implemented on different devices.
/// The ability to use different interfaces on different devices offers an easy
/// way to implement
/// and integrate drivers for I2S chips or bluetooth, for example.
/// \n
/// Each device for the AUD driver needs its own driver implementation.
/// This driver must offer full compatibility to the following API, defined
/// by the types:
///  - <code> typedef AUD_ERR_T (*AUD_STREAM_RECORD_T) (CONST HAL_AIF_STREAM_T* stream, CONST AUD_DEVICE_CFG_T* cfg);</code>
///  - <code> typedef AUD_ERR_T (*AUD_STREAM_STOP_T) (VOID);</code>
///  - <code> typedef AUD_ERR_T (*AUD_STREAM_PAUSE_T) (BOOL);</code>
///  - <code> typedef AUD_ERR_T (*AUD_STREAM_SETUP_T) (CONST AUD_DEVICE_CFG_T* cfg);</code>
///  - <code> typedef AUD_ERR_T (*AUD_TONE_PLAY_T) (
///                CONST SND_TONE_TYPE_T         tone,
///                CONST AUD_DEVICE_CFG_T* cfg,
///                CONST BOOL start);</code>
///  - <code> typedef AUD_ERR_T (*AUD_TONE_PAUSE_T) (BOOL);</code>
///  .
/// That is each Audio Device Driver must implement functions of the above
/// types.\n
/// \n
/// Moreover, each device can provide several inputs and/or outputs. The
/// definition of an AUD receiver interface, for example, requires the set
/// of driver functions of a device and of the input/output (probaly #AUD_SPK_RECEIVER
/// and #AUD_MIC_RECEIVER of the device)
/// \n
/// An Audio Interface is then defined a an instance of the AUD_ITF_CFG_T structures: \n
/// <code> typedef struct
/// {
///    AUD_STREAM_PLAY_T   play;
///    AUD_STREAM_RECORD_T record;
///    AUD_STREAM_STOP_T   stop;
///    AUD_STREAM_PAUSE_T  pause;
///    AUD_STREAM_SETUP_T  setup;
///    AUD_TONE_PLAY_T     tone;
///    AUD_TONE_PAUSE_T    tonePause;
///    AUD_SPK_SEL         spkSel;
///    AUD_MIC_SEL         micSel;
/// } AUD_ITF_CFG_T; </code>
/// which gather all the function needed, and the input/output to use to
/// implement the interface on this device.\n
/// \n
/// AUD needs this API in order to be able to use the audio device
/// corresponding to the Audio Interface.\n
/// \n
/// To specify to the AUD driver which interface to use for a given action
/// (play stream, record stream, etc ...), a #SND_ITF_T parameter is used.\n
/// \n
/// New interfaces can them easily be added, following the examples of some
/// implementations already done. The SND_ITF_QTY value can be updated
/// according to specific needs, where more than 6 interfaces might be
/// needed.
/// \n
/// The definition of the correspondance between the AUD interface and their
/// implementation on the devices is explicited in the target module, thanks
/// to a #AUD_CONFIG_T structure.
///
/// @par Audio Operation
/// - <B> Audio Management for a Call: </B> \n
///      The Audio Management for a Call is not managed directly by the
///      AUD Audio Driver. It is managed by the VoiS Service. The VOIS
///      service uses AUD internally, and thus that is not a concern for a
///      AUD user. AUD developer of new interface drivers should be very
///      careful to implement the AUD API for their interface, as it is
///      used by VOIS to manage call's audio.
///      \n
/// - <B> Stream Record and Playback: </B> \n
///      The driver allows to
///      record an audio input and store it in a specified word buffer (i.e.
///      aligned on 32-bit words) to play it back. \n
///      At the end and at the middle of the audio buffer (during the record or
///      the play) an interruption can be triggered and call a user function,
///      typically to refill or empty the said buffer. \n
///      The data played and recorded by the audio driver are encoded in PCM.
///      The sample rate and the channel number (mono or stereo) are configurable.
///      The size of the buffer must be a multiple of 16 bytes.
///      \n
///      <B>Nota: The receiver mode can only be used with voice quality streams.
///      A voice quality stream is a mono, 8kHz, 13 bits encoded stream.
///      Such a stream is described by a HAL_AIF_STREAM_T structure with the
///      \c voiceQuality field set to \c TRUE.</B>
///      \n
/// - @b Tone @b Generation: \n
///      The tones are fully generated by hardware. The CPU is not used to
///      output the tone signals and therefore, remains available. Only one
///      tone (Comfort or DTMF) can be generated at the same time. \n
///      The DTMF Tones are used to inform the user that the number is being
///      composed. All the standard DTMF are available: 0 to 9, A to D,
///      pound and star. \n
///      The Comfort Tones are used to inform the user on the current state of
///      the call: Ringing, Busy, Unavailable... All frequencies needed to do
///      the standard Comfort Tones are available: 425 Hz, 950 Hz, 1400 Hz and
///      1800 Hz. \n
///      The attenuation can be set to 0 dB, -3 dB, -9 dB and -15dB.
///      When a tone is generated while a stream is played, the sound from the tone
///      is heard instead of the stream, which continue being played at the same
///      time, if the stream is a voice quality one. A tone cannot be generated
///      when another kind of stream is being played. The stream must be stopped
///      before the tone can be generated. And the stream must be resumed after
///      the tone is finished.
///      \n
/// - <B> Input/Output Selection: </B>
///      The choice of using different speaker/microphone to play or record stream
///      is made at the call of the concerned functiom: #aud_StreamStart,
///      #aud_StreamRecord, etc. It cannot be changed after that.
///      To change from a hanset mode to a earpiece mode, for example, the stream
///      must be stopped and restarted with a different parameter set applied to
///      #aud_StreamStart and #aud_StreamRecord.
///
/// AUD is composed of this module:
/// - @ref aud
/// .
///
/// @par Flow Organisation
/// @image latex vois.png
/// @image html vois.png
/// The picture above describes the place taken by AUD in the VoiS flow
/// organistation. \n
/// For the APS and ARS services, the flow is organised the same way,
/// APS being restricted to the Rx flow and ARS to the Tx flow.\n
/// \n
///
/// @par Resource Management
/// A resource might be needed to allow the AUD driver to have a fast enough
/// clock. This need depends on the Audio Interface used and of their own
/// implementation. For example, the AUD Null Driver (No interface available
/// for that interface Id) doesn't take a resource). \n
/// It is thus needed to take care at which Audio Interface is used and
/// how which are its frequency needs. \n
/// The resource is managed internally by the Audio Interface Drivers, that is
/// a resource is taken when the driver needs it and freed when possible. Typically,
/// when a stream is played, a resource is taken until the stream is stopped.
/// A stream paused still hold the resource.\n
/// A common resource is shared with the stream record and tone capabilities
/// of the Audio Interface Driver: an Audio Interface Driver takes one resource
/// when it needs a given frequency, and frees it when it doesn't need it
/// anymore. This is not a concern for the AUD user. It IS a concern for
/// the Audio Interface Driver developpers.
///
/// @par Nota:
/// This driver uses extensively the HAL_AIF_STREAM_T type, whose documentation
/// can be found in the HAL API Documentation.
///

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// AUD_ERR_T
// -----------------------------------------------------------------------------
/// Error types for the AUD module.
// =============================================================================
typedef enum
{
    /// No error occured
    AUD_ERR_NO,

    /// An attempt to access a busy resource failed
    /// (Resource use not possible at the time)
    AUD_ERR_RESOURCE_BUSY,

    /// Attempt to open a resource already opened,
    /// (no need to open again to use).
    AUD_ERR_ALREADY_OPENED,

    /// Timeout while trying to access the resource
    AUD_ERR_RESOURCE_TIMEOUT,

    /// Invalid parameter
    AUD_ERR_BAD_PARAMETER,

    /// The specified interface does not exist
    AUD_ERR_NO_ITF,

    /// Unspecified error
    AUD_ERR_UNKNOWN,

    AUD_ERR_QTY
} AUD_ERR_T;


// =============================================================================
// AUD_SPK_T
// -----------------------------------------------------------------------------
/// Speaker output selection.
// =============================================================================
typedef enum
{
    /// Output on receiver
    /// This output can only use voice quality streams (Mono, 8kHz,
    /// voiceQuality field set to TRUE).
    AUD_SPK_RECEIVER = 0,
    /// Output on ear-piece
    AUD_SPK_EAR_PIECE,
    /// Output on hand-free loud speaker
    AUD_SPK_LOUD_SPEAKER,
    /// Output on both hand-free loud speaker and ear-piece
    AUD_SPK_LOUD_SPEAKER_EAR_PIECE,

    AUD_SPK_QTY,

    AUD_SPK_DISABLE=255
} AUD_SPK_T;


// =============================================================================
// AUD_MIC_T
// -----------------------------------------------------------------------------
/// Microphone input selection.
// =============================================================================
typedef enum
{
    /// Input from the regular microphone port
    AUD_MIC_RECEIVER = 0,
    /// Input from the ear-piece port,
    AUD_MIC_EAR_PIECE,
    /// Input from regular microphone, but for loudspeaker mode.
    AUD_MIC_LOUD_SPEAKER,

    AUD_MIC_QTY,

    AUD_MIC_DISABLE = 255
} AUD_MIC_T;


// =============================================================================
// AUD_SPEAKER_TYPE_T
// -----------------------------------------------------------------------------
/// Describes how the speaker is plugged on the stereo output:
///   - is it a stereo speaker ? (speakers ?)
///   - is it a mono speaker on the left channel ?
///   - is it a mono speaker on the right channel ?
///   - is this a mono output ?
// =============================================================================
typedef enum
{
    AUD_SPEAKER_STEREO,
    AUD_SPEAKER_MONO_RIGHT,
    AUD_SPEAKER_MONO_LEFT,
    /// The output is mono only.
    AUD_SPEAKER_STEREO_NA,

    AUD_SPEAKER_QTY
} AUD_SPEAKER_TYPE_T;


// =============================================================================
// AUD_LEVEL_T
// -----------------------------------------------------------------------------
/// Level configuration structure.
///
/// A level configuration structure allows to start an AUD operation (start
/// stream, start record, or start tone) with the desired gains on an interface.
// =============================================================================
typedef struct
{
    /// Speaker level,
    SND_SPK_LEVEL_T spkLevel;

    /// Microphone level: muted or enabled
    SND_MIC_LEVEL_T micLevel;

    /// Sidetone
    SND_SIDE_LEVEL_T sideLevel;

    SND_TONE_ATTENUATION_T toneLevel;

} AUD_LEVEL_T;


// =============================================================================
// AUD_DEVICE_CFG_T
// -----------------------------------------------------------------------------
/// Configuration structure for device drivers implementation.
///
/// A configuration structure allows to start an AUD operation (start stream, start
/// record, or start tone) with the desired parameters.
// =============================================================================
typedef struct
{
    /// Type of speaker
    AUD_SPK_T    spkSel;

    /// Kind of the speaker (stereo, mono, etc)
    AUD_SPEAKER_TYPE_T spkType;

    /// Type of mic
    AUD_MIC_T    micSel;

    /// Speaker level,
    CONST AUD_LEVEL_T* level;

} AUD_DEVICE_CFG_T;


// =============================================================================
//  AUD_TEST_T
// -----------------------------------------------------------------------------
/// Used to choose the audio mode: normal, test, dai's ...
// =============================================================================
typedef enum
{
    /// No test mode.
    AUD_TEST_NO,

    /// For audio test loop; analog to DAI loop + DAI to analog loop.
    AUD_TEST_LOOP_ACOUSTIC,

    /// For audio test loop; radio loop on DAI interface
    AUD_TEST_LOOP_RF_DAI,

    /// For audio test loop; radio loop on analog interface
    AUD_TEST_LOOP_RF_ANALOG,

    /// For board check: mic input is fedback to the speaker output.
    AUD_TEST_SIDE_TEST,

    /// For board check; audio loop in VOC
    AUD_TEST_LOOP_VOC,

    /// For board check; regular mic input and earpiece output
    AUD_TEST_RECVMIC_IN_EARPIECE_OUT,

    AUD_TEST_MODE_QTY
} AUD_TEST_T;


// =============================================================================
// AUD_STREAM_PLAY_T
// -----------------------------------------------------------------------------
/// This type is the one describing the prototype for stream playing functions.
/// Each configured audio interface allowing to play a stream must implement
/// one of them.
// =============================================================================
typedef AUD_ERR_T (*AUD_STREAM_PLAY_T) (SND_ITF_T itf,
                                        CONST HAL_AIF_STREAM_T* stream,
                                        CONST AUD_DEVICE_CFG_T* cfg);


// =============================================================================
// AUD_STREAM_RECORD_T
// -----------------------------------------------------------------------------
/// This type is the one describing the prototype for stream recording functions.
/// Each configured audio interface allowing to record a stream must implement
/// one of them.
// =============================================================================
typedef AUD_ERR_T (*AUD_STREAM_RECORD_T) (SND_ITF_T itf,
        CONST HAL_AIF_STREAM_T* stream,
        CONST AUD_DEVICE_CFG_T* cfg);


// =============================================================================
// AUD_STREAM_STOP_T
// -----------------------------------------------------------------------------
/// This type is the one describing the prototype for stream stopping functions.
/// Each configured audio interface allowing to stop a stream must implement
/// one of them. This function stops both playing or recording stream.
// =============================================================================
typedef AUD_ERR_T (*AUD_STREAM_STOP_T) (SND_ITF_T itf);


// =============================================================================
// AUD_CALIB_UPDATE_T
// -----------------------------------------------------------------------------
/// This type is the one describing the prototype for the functions which
/// update the values depending on the calibration. This is used when
/// calibration values have been changed manually, and must be applied so that
/// their effect is observed.
// =============================================================================
typedef AUD_ERR_T (*AUD_CALIB_UPDATE_T) (SND_ITF_T itf);


// =============================================================================
// AUD_STREAM_PAUSE_T
// -----------------------------------------------------------------------------
/// This type is the one describing the prototype for stream pausing functions.
/// Each configured audio interface allowing to pause a stream must implement
/// one of them. This function pauses both playing or recording stream.
// =============================================================================
typedef AUD_ERR_T (*AUD_STREAM_PAUSE_T) (SND_ITF_T itf, BOOL);


// =============================================================================
// AUD_STREAM_SETUP_T
// -----------------------------------------------------------------------------
/// This type is the one describing the prototype for configuring functions.
/// This enables to set the speaker and mic used and adjust some the gains.
// =============================================================================
typedef AUD_ERR_T (*AUD_STREAM_SETUP_T) (SND_ITF_T itf, CONST AUD_DEVICE_CFG_T* cfg);


// =============================================================================
// AUD_TONE_PLAY_T
// -----------------------------------------------------------------------------
/// This type is the one describing the prototype for tone playing or stopping
/// functions.
/// Each configured audio interface allowing to play a tone must implement
/// one.
// =============================================================================
typedef AUD_ERR_T (*AUD_TONE_PLAY_T) (
    SND_ITF_T            itf,
    CONST SND_TONE_TYPE_T   tone,
    CONST AUD_DEVICE_CFG_T*        cfg,
    CONST BOOL              start);


// =============================================================================
// AUD_TONE_PAUSE_T
// -----------------------------------------------------------------------------
/// This type is the one describing the prototype for tone pausing functions.
/// Each configured audio interface allowing to pause a tone must implement
/// one of them.
// =============================================================================
typedef AUD_ERR_T (*AUD_TONE_PAUSE_T) (SND_ITF_T itf, BOOL);


// =============================================================================
// AUD_ITF_CFG_T
// -----------------------------------------------------------------------------
/// That type describes an interface, providing fields to record all provided
/// functionalities.
/// @todo Extend with test managing functions. (?)
// =============================================================================
typedef struct
{
    AUD_STREAM_PLAY_T   play;
    AUD_STREAM_RECORD_T record;
    AUD_STREAM_STOP_T   stop;
    AUD_STREAM_PAUSE_T  pause;
    AUD_STREAM_SETUP_T  setup;
    AUD_TONE_PLAY_T     tone;
    AUD_TONE_PAUSE_T    tonePause;
    AUD_CALIB_UPDATE_T  calibUpdate;
    // A (set) of parameters specifics to a driver
    UINT32              parameter;
    /// Speaker to use on that device
    AUD_SPK_T           spkSel;
    /// Type of speaker
    AUD_SPEAKER_TYPE_T  spkType;
    /// Mic to use on that device
    AUD_MIC_T           micSel;
} AUD_ITF_CFG_T;


// =============================================================================
// AUD_CONFIG_T
// -----------------------------------------------------------------------------
/// That type is used to describe the configuration specific to a given target
/// for the AUD audio driver. It defines all the interfaces available in this
/// target, and the functions implementing the drivers for each interface
/// (in the AUD_ITF_CFG_T array).
// =============================================================================
typedef CONST AUD_ITF_CFG_T AUD_CONFIG_T[SND_ITF_QTY];


// Test mode buffer malloc function type
typedef VOID* (*AUD_TEST_MODE_MALLOC_FUNC_T)(UINT32);


// Test mode buffer free function type
typedef VOID (*AUD_TEST_MODE_FREE_FUNC_T)(VOID*);


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// aud_MutexTake
// -----------------------------------------------------------------------------
/// Take the mutex protecting AUD shared variables.
///
/// @return The user ID to take this mutex.
// =============================================================================
PUBLIC UINT8 aud_MutexTake(VOID);


// =============================================================================
// aud_MutexRelease
// -----------------------------------------------------------------------------
/// Release the mutex protecting AUD shared variables.
///
/// @param userId The user ID to release this mutex
// =============================================================================
PUBLIC VOID aud_MutexRelease(UINT8 userId);


// =============================================================================
// aud_Setup
// -----------------------------------------------------------------------------
/// Configure the audio.
///
/// This functions configures an audio interface: gain for the side tone and
/// the speaker, input selection for the microphone and output selection for
/// the speaker, enabling of the microphone ... It can be used to change
/// 'on the fly' the input or output used to play/record a stream on the
/// same interface, but cannot be used to change the interface during a
/// playback or a record.
///
/// @param itf Interface number of the interface to setup.
/// @param cfg The configuration set applied to the audio interface. See
/// #AUD_LEVEL_T for more details.
/// @return #AUD_ERR_NO if it can execute the configuration.
// =============================================================================
PUBLIC AUD_ERR_T aud_Setup(SND_ITF_T itf, CONST AUD_LEVEL_T* cfg);


// =============================================================================
// aud_StreamStart
// -----------------------------------------------------------------------------
/// Start the playing of a stream.
///
/// This function  outputs the audio stream on the audio interface specified as
/// a parameter, on the output selected in the AUD_LEVEL_T cfg parameter. \n
/// When the buffer runs out, the playing will wrap at the
/// beginning of the buffer. If defined in the HAL_AIF_STREAM_T structure,
/// a user function can be called at the middle or at the end of the playback.
///
/// @param itf Interface on which play that stream
/// @param stream Stream to play. Handler called at the middle and end of the
/// buffer are defined there. The length of a stream must be a multiple
/// of 16 bytes.
/// @param cfg The configuration set applied to the audio interface
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command,
///         #AUD_ERR_NO it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_StreamStart(
    CONST SND_ITF_T      itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_LEVEL_T *       cfg);


// =============================================================================
// aud_StreamStop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback or/and record.
/// If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param itf Interface on which to stop the playing.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_StreamStop(CONST SND_ITF_T itf);


// =============================================================================
// aud_StreamRecordStop
// -----------------------------------------------------------------------------
/// This function stops the audio stream  record.
/// If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param itf Interface on which to stop the playing.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_StreamRecordStop(SND_ITF_T itf);


// =============================================================================
// aud_StreamPause
// -----------------------------------------------------------------------------
/// This function pauses or resume the audio stream playback or/and record.
/// If the function returns #AUD_ERR_RESOURCE_BUSY, it means that the driver
/// is busy with another audio command.
///
/// @param itf Interface on which to stop the playing.
/// @param pause If \c TRUE, pauses the stream. If \c FALSE, resumes a paused
/// stream.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_StreamPause(CONST SND_ITF_T itf, CONST BOOL pause);


// =============================================================================
// aud_StreamRecord
// -----------------------------------------------------------------------------
/// Manage the recording of a stream.
/// This function records the audio stream from the audio interface specified as
/// a parameter, from the input selected in the AUD_LEVEL_T cfg parameter.\n
/// In normal operation, when the buffer runs out, the recording will wrap at the
/// beginning of the buffer. If defined in the HAL_AIF_STREAM_T structure,
/// a user function can be called at the middle or at the end of the record.
///
/// @param itf Interface whose PCM flow is recorded.
/// @param stream Stream recorded (describe the buffer and the possible interrupt
/// of middle-end buffer handlers. The length of a stream must be a multiple
/// of 16 bytes.
/// @param cfg The configuration set applied to the audio interface.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver cannot handle the stream,
///         #AUD_ERR_NO if the stream ca be recorded.
// =============================================================================
PUBLIC AUD_ERR_T aud_StreamRecord(
    CONST SND_ITF_T      itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_LEVEL_T *       cfg);


// =============================================================================
// aud_ToneStart
// -----------------------------------------------------------------------------
/// Manage the playing of a tone: DTMF or Comfort Tone.
///
/// This function starts the output of a tone generated in the audio
/// module on the selected interface. It a stream is being played on the same
/// interface, the stream will continue to be played the  its sound won't be
/// output. It will be replaced instead by the generated tone.
///
/// You can call this function several times without calling the
/// #aud_ToneStop function or the #aud_TonePause function in
/// between, if you just need to change the attenuation or the tone type. \n
/// If the function returns #AUD_ERR_RESOURCE_BUSY, it means that the driver is
/// busy with an other audio command.
///
/// @param itf AUD interface on which to play a tone.
/// @param tone The tone to generate.
/// @param cfg The configuration set applied to the audio interface.
///
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_ToneStart(
    CONST SND_ITF_T           itf,
    CONST SND_TONE_TYPE_T        tone,
    CONST AUD_LEVEL_T*             cfg);


// =============================================================================
// aud_ToneStop
// -----------------------------------------------------------------------------
/// Stop the tone generation.
/// If the function returns  #AUD_ERR_RESOURCE_BUSY, it means that the driver
/// is busy with an other audio command.
///
/// @param itf AUD interface on which to stop the tone.
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_ToneStop(CONST SND_ITF_T itf);


// =============================================================================
// aud_TonePause
// -----------------------------------------------------------------------------
/// This function pauses or resume a tone.
/// If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param itf Interface on which to stop the playing.
/// @param pause If \c TRUE, pauses the tone. If \c FALSE, resumes a paused
/// tone.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_TonePause(CONST SND_ITF_T itf, CONST BOOL pause);


// =============================================================================
// aud_TestModeSetup
// -----------------------------------------------------------------------------
/// Enable a test mode.
/// This function enables a test mode, enabling for example the side test
/// or the DAIs.
/// The audio interface must be enabled when this function is called.
/// This function disables the AIF (audio interface), sets the audio
/// interface in the mode specified, and re-enables the AIF.
/// See the <B> Application Note 0018 </B> for more details on the DAI
/// functionality and see 3GPP TS 44014-500 page 40 for the description of the
/// DAI test interface.
///
/// @param itf AUD interface to test.
/// @param stream Depending on the test, this parameter can be ignored, or
/// used to configure it. (This is the case for the side test). This
/// sets things relative to the streaming (sampling rate, etc)
/// @param cfg Depending on the test, this parameter can be ignored, or
/// used to configure it. (This is the case for the side test). Configure
/// the audio interface.
/// @param mode Test mode to set.
/// @return #AUD_ERR_NO if it succeeds.
// =============================================================================
PUBLIC AUD_ERR_T aud_TestModeSetup(CONST SND_ITF_T         itf,
                                   CONST HAL_AIF_STREAM_T* stream,
                                   CONST AUD_LEVEL_T *     cfg,
                                   AUD_TEST_T              mode);


// =============================================================================
// aud_CalibUpdateValues
// -----------------------------------------------------------------------------
/// Update values depending on calibration parameters.
///
/// @param itf AUD interface on which to stop the tone.
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_CalibUpdateValues(CONST SND_ITF_T itf);


PUBLIC VOID aud_ForceReceiverMicSelection(BOOL on);

PUBLIC BOOL aud_GetForceReceiverMicSelectionFlag(VOID);


PUBLIC VOID aud_SetCurOutputDevice(SND_ITF_T itf);


PUBLIC SND_ITF_T aud_GetCurOutputDevice(VOID);


PUBLIC VOID aud_LoudspeakerWithEarpiece(BOOL on);


PUBLIC BOOL aud_TestModeRegisterMallocFreeFunc(AUD_TEST_MODE_MALLOC_FUNC_T mallocFunc,
        AUD_TEST_MODE_FREE_FUNC_T freeFunc);


PUBLIC VOID aud_CodecCommonSetAifConfigByAp(BOOL on);


PUBLIC AUD_ERR_T aud_TestModeSetupExt(CONST SND_ITF_T         itf,
                                      CONST HAL_AIF_STREAM_T* stream,
                                      CONST AUD_LEVEL_T *     cfg,
                                      UINT8 *audRecordBuffer);
PUBLIC AUD_ERR_T aud_TestModeStopExt(CONST SND_ITF_T     itf);
#ifndef CHIP_HAS_AP
PUBLIC VOID aud_SetMicMuteFlag(BOOL muteflag);

PUBLIC BOOL aud_GetMicMuteFlag(VOID);
#endif

///  @} <- End of the aud group
PUBLIC INT16 aud_GetOutAlgGainDb2Val(VOID);
PUBLIC BOOL aud_GetBtNRECFlag(VOID);

#if defined(BT_A2DP_ANA_DAC_MUTE)
PUBLIC VOID aud_SetFMWorkingFlag(BOOL muteflag);

PUBLIC BOOL aud_GetFMWorkingFlag(VOID);
#endif

#ifdef CHIP_DIE_8955
PUBLIC VOID aud_EnFMRecNewPath(BOOL en);
#endif
#endif // _AUD_M_H_

