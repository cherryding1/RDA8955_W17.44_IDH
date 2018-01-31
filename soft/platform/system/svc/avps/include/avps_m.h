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



#ifndef _AVPS_M_H_
#define _AVPS_M_H_

#include "cs_types.h"

#include "hal_aif.h"

#include "lcdd_m.h"

/// @file avps_m.h
/// @mainpage Audio Plays Service API
/// @page avps_mainpage  Audio Plays Service API
///
/// This service provides playing capabilities for various encoded streams.
/// They can be played in loop or not and interrupts are generated when the middle
/// and the end of the buffer are reached. \n
/// If a handler for those interrupt is passed to AVPS, it is expected that they
/// are used to refill the buffer with data, and AVPS will make a count of available
/// data and suspend playing in case of underflow. #avps_AudioAddedData needs to be called
/// to tell AVPS of the added data. \n
/// On the other hand, if no handler is defined, AVPS will loop on the buffer
/// whatever happens. It doesn't take #avps_AudioAddedData into account, and no
/// call to it is needed.
/// @par Configuration requirements
/// HAL and AUD must be poperly configured in order to use the avps service.
///
///
/// The API is detailed in the following group: @ref avps
///
///
/// @defgroup avps Play Service (AVPS)
/// @{
///

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
// AVPS_HEADER_ID_T
// -----------------------------------------------------------------------------
/// This type describes the header Id used by the various messages
/// sent by AVPS.
// =============================================================================
typedef enum
{
    /// Msg send on audio underflow
    AVPS_HEADER_ID_AUDIO                = 0xA0C01F56,

    /// Msg send on video underflow
    AVPS_HEADER_ID_VIDEO                = 0xA0C01F57,

    /// Msg send on video display request
    AVPS_HEADER_ID_VIDEO_DISPLAY        = 0xA0C01F58,

    /// Msg send when video trame is not present on sync audio/video
    AVPS_HEADER_ID_VIDEO_SKIP           = 0xA0C01F59
} AVPS_HEADER_ID_T;






// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
// AVPS_PLAY_AUDIO_MODE_T
// -----------------------------------------------------------------------------
/// This type describes the encoding mode used in a audio stream to play.
// =============================================================================
typedef enum
{
    AVPS_PLAY_AUDIO_MODE_NO,
    AVPS_PLAY_AUDIO_MODE_AMR475,
    AVPS_PLAY_AUDIO_MODE_AMR515,
    AVPS_PLAY_AUDIO_MODE_AMR59,
    AVPS_PLAY_AUDIO_MODE_AMR67,
    AVPS_PLAY_AUDIO_MODE_AMR74,
    AVPS_PLAY_AUDIO_MODE_AMR795,
    AVPS_PLAY_AUDIO_MODE_AMR102,
    AVPS_PLAY_AUDIO_MODE_AMR122,
    AVPS_PLAY_AUDIO_MODE_FR,
    AVPS_PLAY_AUDIO_MODE_HR,
    AVPS_PLAY_AUDIO_MODE_EFR,
    AVPS_PLAY_AUDIO_MODE_PCM,
    AVPS_PLAY_AUDIO_MODE_AMR_RING,
    AVPS_PLAY_AUDIO_MODE_MP3,
    AVPS_PLAY_AUDIO_MODE_AAC,

    AVPS_PLAY_AUDIO_MODE_QTY
} AVPS_PLAY_AUDIO_MODE_T;

// =============================================================================
// AVPS_PLAY_MODE_VIDEO_T
// -----------------------------------------------------------------------------
/// This type describes the encoding mode used in a video stream to play.
// =============================================================================
typedef enum
{
    AVPS_PLAY_VIDEO_MODE_NO,
    AVPS_PLAY_VIDEO_MODE_MJPEG,
    AVPS_PLAY_VIDEO_MODE_H263,
    AVPS_PLAY_VIDEO_MODE_MPEG4,

    AVPS_PLAY_VIDEO_MODE_QTY
} AVPS_PLAY_VIDEO_MODE_T;

// =============================================================================
// AVPS_ERR_T
// -----------------------------------------------------------------------------
/// Error types for the AUD module.
// =============================================================================
typedef enum
{
    /// No error occured
    AVPS_ERR_NO,

    /// An attempt to access a busy resource failed
    /// (Resource use not possible)
    AVPS_ERR_RESOURCE_BUSY,

    /// Attempt to open a resource already opened,
    /// (no need to open again to use).
    AVPS_ERR_ALREADY_OPENED,

    /// Timeout while trying to access the resource
    AVPS_ERR_RESOURCE_TIMEOUT,

    /// Sample rate not supported
    AVPS_ERR_INVALID_SAMPLERATE,

    /// No codec found for this stream
    AVPS_ERR_NO_CODEC,

    /// The specified interface does not exist
    AVPS_ERR_NO_ITF,

    /// What ?
    AVPS_ERR_UNKNOWN,


    AVPS_ERR_QTY
} AVPS_ERR_T;


// =============================================================================
// AVPS_BODY_MSG_T
// -----------------------------------------------------------------------------
/// Body of AVPS message
// =============================================================================
typedef struct
{
    UINT32* bufferPos;
} AVPS_BODY_MSG_T ;

// =============================================================================
// AVPS_VIDEO_ROTATION_T
// -----------------------------------------------------------------------------
/// List of audio path
// =============================================================================
typedef enum
{
    AVPS_VIDEO_ROTATION_PORTRAIT = 0,
    AVPS_VIDEO_ROTATION_LANDSCAPE
} AVPS_VIDEO_ROTATION_T;

// =============================================================================
// AVPS_CFG_T
// -----------------------------------------------------------------------------
/// Configuration structure.
///
/// A configuration structure allows to play a stream with the proper configuration
/// set as far as the audio/video interface and the decoding are concerned.
/// @todo Expand this structure whether needed.
// =============================================================================
typedef struct
{
    /// Speaker level, 0=MUTE, MAX=AUD_LEVEL_MAX
    UINT8                     spkLevel;

    /// Equalizer mode
    UINT8                     equalizer;
    /// Video rotation
    AVPS_VIDEO_ROTATION_T     videoRotation;
} AVPS_CFG_T;





// =============================================================================
// AVPS_ENC_STREAM_T
// -----------------------------------------------------------------------------
/// This type defines an encoded stream. All the fields might not be needed
/// depending on the encoding scheme used.
// =============================================================================
typedef struct
{
    AVPS_PLAY_AUDIO_MODE_T audioMode;

    ///
    UINT32                 audioAddress;

    /// Length of the stream buffer. That is not the total length of the song
    /// to play, as data can be added to the buffer while playing.
    /// cf. API functions for details.
    UINT32                 audioLength;

    /// Audio Sample Rate of the stream. Optional, this value is ignored
    /// by mode not needing it.
    UINT16                 audioSampleRate;

    /// Number of channel of the stream. Optional,this value is ignored
    /// by mode not needing it.
    UINT8                  channelNb;

    /// Is this encoded in voice mode (voice quality, mono, 8kHz)
    BOOL                   voiceQuality;



    /// Mode used to play that stream
    AVPS_PLAY_VIDEO_MODE_T videoMode;

    UINT32                 videoAddress;

    /// Length of the video buffer
    UINT32                 videoLength;

    /// Describe ratio between audio frame rate and video frame rate
    UINT16                 audioVideoRatio;



    /// Function called to transmit play status to the caller
    /// If null, no handler is called.
    UINT8            mbx;
} AVPS_ENC_STREAM_T;

// =============================================================================
// AVPS_FREQUENCY_BARS_T
// -----------------------------------------------------------------------------
/// Audio track frequency bars
// =============================================================================
typedef UINT8 AVPS_FREQUENCY_BARS_T[16];


// =============================================================================
// AVPS_BUFFER_CONTEXT_T
// -----------------------------------------------------------------------------
/// Describe state of input buffer
/// This context is used to do pause play with mps
// =============================================================================
typedef struct
{
    UINT8*        bufferAudioReadPosition;
    UINT32        audioRemainingSize;
} AVPS_BUFFER_CONTEXT_T ;



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
/// @param cfg The configuration set applied to the audio interface. See
/// #AUD_LEVEL_T for more details.
/// @return #AVPS_ERR_NO it can execute the configuration.
// =============================================================================
PUBLIC AVPS_ERR_T avps_Setup(CONST AVPS_CFG_T* cfg);



// =============================================================================
// avps_Play
// -----------------------------------------------------------------------------
/// Start the playing of a stream.
///
/// This function  outputs the
/// audio on the audio interface specified as a parameter, on the output selected in
/// the #avps_Setup function. \n
/// In normal operation, when the buffer runs out, the playing will stop. An
/// handler
/// is defined, it is expected that the handler will refeed data and call #avps_AudioAddedData to tell APS about
/// that.
/// AVPS can a user function
/// at the middle or at the end of the playback.
///
/// @param itf Interface on which play that stream
/// @param stream Stream to play. Handler called at the middle and end of the buffer
/// are defined there.
/// @param cfg The configuration set applied to the audio interface
/// @param bufContext Define the current buffer state
/// @param frameWindows Region use to display video
/// @return #AVPS_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command, or if VPP is unavailable.
///         #AVPS_ERR_NO it can execute the command.
// =============================================================================
PUBLIC AVPS_ERR_T avps_Play(CONST UINT8                    itf,
                            CONST AVPS_ENC_STREAM_T*       stream,
                            CONST AVPS_CFG_T*              cfg,
                            CONST LCDD_FBW_T*              frameWindows,
                            CONST AVPS_BUFFER_CONTEXT_T*   bufContext);


// =============================================================================
// avps_Stop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback.
/// If the function returns
/// #AVPS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param bufContext Return the current buffer state
/// @return #AVPS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AVPS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AVPS_ERR_T avps_Stop(AVPS_BUFFER_CONTEXT_T* bufContext);


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
PUBLIC AVPS_ERR_T avps_AudioAddedData(UINT32 addedDataBytes);


// =============================================================================
// avps_VideoAddedData
// -----------------------------------------------------------------------------
/// This function is use to inform avps of new video frame
///
/// @return #AVPS_ERR_NO No error
// =============================================================================
PUBLIC AVPS_ERR_T avps_VideoAddedData(UINT32 addedDataBytes);


// =============================================================================
// avps_GetFreqBars
// -----------------------------------------------------------------------------
/// This function return the frequency bars of audio track
///
/// @param bars Pointer on aray where the frequency bars is stored
// =============================================================================
PUBLIC AVPS_FREQUENCY_BARS_T* avps_GetFreqBars(VOID);

///  @} <- End of the avps group



#endif // _AVPS_M_H_

