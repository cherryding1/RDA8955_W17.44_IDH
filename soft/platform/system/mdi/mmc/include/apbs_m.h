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



#ifndef _APBS_M_H_
#define _APBS_M_H_

#include "cs_types.h"

#include "hal_aif.h"
#include "aud_m.h"
#include "mci.h"
#include "cos.h"


/// @file apbs_m.h
/// @mainpage Audio Plays Service API
/// @page apbs_mainpage  Audio Plays Service API
///
/// This service provides playing capabilities for various encoded streams.
/// They can be played in loop or not and interrupts are generated when the middle
/// and the end of the buffer are reached. \n
/// If a handler for those interrupt is passed to APBS, it is expected that they
/// are used to refill the buffer with data, and APBS will make a count of available
/// data and suspend playing in case of underflow. #apbs_AddedData needs to be called
/// to tell APBS of the added data. \n
/// On the other hand, if no handler is defined, APBS will loop on the buffer
/// whatever happens. It doesn't take #apbs_AddedData into account, and no
/// call to it is needed.
///
///
///
/// @par Configuration requirements
/// HAL and AUD must be poperly configured in order to use the apbs service.
///
///
/// The API is detailed in the following group: @ref apbs
///
///
/// @defgroup apbs Play Service (APBS)
/// @{
///

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================





// =============================================================================
// APBS_ERR_T
// -----------------------------------------------------------------------------
/// Error types for the AUD module.
// =============================================================================
typedef enum
{
    /// No error occured
    APBS_ERR_NO,

    /// An attempt to access a busy resource failed
    /// (Resource use not possible)
    APBS_ERR_RESOURCE_BUSY,

    /// Attempt to open a resource already opened,
    /// (no need to open again to use).
    APBS_ERR_ALREADY_OPENED,

    /// Timeout while trying to access the resource
    APBS_ERR_RESOURCE_TIMEOUT,

    /// Invalid parameter
    APBS_ERR_BAD_PARAMETER,

    /// The specified interface does not exist
    APBS_ERR_NO_ITF,

    /// What ?
    APBS_ERR_UNKNOWN,


    APBS_ERR_QTY
} APBS_ERR_T;


// =============================================================================
// APBS_STATUS_T
// -----------------------------------------------------------------------------
/// Status returned by the APBS to the calling entity. The possible value
/// describes various information about the status of the play.
// =============================================================================
typedef enum
{
    APBS_STATUS_MID_BUFFER_REACHED,
    APBS_STATUS_END_BUFFER_REACHED,
    APBS_STATUS_NO_MORE_DATA,
    APBS_STATUS_ERR,

    APBS_STATUS_QTY
} APBS_STATUS_T;




// =============================================================================
// MMC_ANALOG_MSGID
// -----------------------------------------------------------------------------
///
///
// =============================================================================

typedef enum
{
    //STOP  MESSAGE
    APBS_STOP=0x20,
    APBS_SETUP=0x21,
    APBS_ERR =  0x22,
    APBS_REQUEST=0x23,
    APBS_NO_MORE_DATA = 0x24,

} APBS_MSGID;


// =============================================================================
// APBS_USER_HANDLER_T
// -----------------------------------------------------------------------------
/// To give back status to the calling task, the APBS needs to be given a
/// callback function. This function is called whenever an event occurs
/// which needs to be reported back to the APBS used. This status is reported
/// as the parameter of the function. The function is passed as a parameter to
/// the function #apbs_Play. This type is the type describing such a function.
///
/// Note: This function must be as minimalist as possible, as it will be
/// executed during an interrupt. It should for example only send one event
/// to a task, asking for data refilling of the buffer. The data copy is
/// then done by the task, when it is rescheduled, but not directly by the
/// user handler.
// =============================================================================
typedef VOID (*APBS_USER_HANDLER_T)(APBS_STATUS_T);


// =============================================================================
// APBS_AUDIO_CFG_T
// -----------------------------------------------------------------------------
/// Configuration structure.
///
/// A configuration structure allows to play a stream with the proper configuration
/// set as far as the audio interface and the decoding are concerned.
/// @todo Expand this structure whether needed.
// =============================================================================
typedef struct
{
    /// Speaker level, 0=MUTE, MAX=AUD_LEVEL_MAX
    AUD_SPK_LEVEL_T spkLevel;

    /// Pointer to a filter to apply to the stream
    UINT32*     filter;

} APBS_AUDIO_CFG_T;





// =============================================================================
// APBS_ENC_STREAM_T
// -----------------------------------------------------------------------------
/// This type defines an encoded stream. All the fields might not be needed
/// depending on the encoding scheme used.
// =============================================================================
typedef struct
{
    ///
    UINT32 startAddress;

    /// Length of the buffer. That is not the total length of the song
    /// to play, as data can be added to the buffer while playing.
    /// cf. API functions for details.
    UINT32 length;

    /// Mode used to play that stream
    MCI_PLAY_MODE_T mode;

    /// Sample Rate of the stream. Optional, this value is ignored
    /// by mode not needing it.
    HAL_AIF_FREQ_T sampleRate;

    /// Number of channel of the stream. Optional,this value is ignored
    /// by mode not needing it.
    HAL_AIF_CH_NB_T channelNb;

    /// Is this encoded in voice mode (voice quality, mono, 8kHz)
    BOOL    voiceQuality;

    /// Function called to transmit play status to the caller
    /// If null, no handler is called.
    APBS_USER_HANDLER_T handler;
    UINT32  bitPerSample;   // for pcm stream

} APBS_ENC_STREAM_T;





// =============================================================================
//  FUNCTIONS
// =============================================================================




// =============================================================================
// apbs_Setup
// -----------------------------------------------------------------------------
/// Configure the apbs service..
///
/// This functions configures the playing of a stream on a given audio interface:
/// gain for the side tone and
/// the speaker, input selection for the microphone and output selection for
/// the speaker... (TODO complete)
///
/// @param itf Interface number of the interface to setup.
/// @param cfg The configuration set applied to the audio interface. See
/// #AUD_CFG_T for more details.
/// @return #APBS_ERR_NO it can execute the configuration.
// =============================================================================
DEPRECATED PUBLIC APBS_ERR_T apbs_Setup(AUD_ITF_T itf, CONST APBS_AUDIO_CFG_T* cfg);



// =============================================================================
// apbs_Play
// -----------------------------------------------------------------------------
/// Start the playing of a stream.
///
/// This function  outputs the
/// audio on the audio interface specified as a parameter, on the output selected in
/// the #apbs_Setup function. \n
/// In normal operation, when the buffer runs out, the playing will stop. It can
/// loop according to the caller choice. If a handler is defined, it is expected
/// that the handler will refeed data and call #apbs_AddedData to tell APBS about
/// that. If there is no handler, in loop mode, the buffer will be played in loop
/// without expecting new data.
/// APBS can a user function
/// at the middle or at the end of the playback.
///
/// @param itf Interface on which play that stream
/// @param stream Stream to play. Handler called at the middle and end of the buffer
/// are defined there.
/// @param cfg The configuration set applied to the audio interface
/// @param loop \c TRUE if the buffer is played in loop mode.
/// @return #APBS_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command, or if VPP is unavailable.
///         #APBS_ERR_NO it can execute the command.
// =============================================================================
DEPRECATED PUBLIC APBS_ERR_T apbs_Play(
    CONST AUD_ITF_T      itf,
    CONST APBS_ENC_STREAM_T* stream,
    CONST APBS_AUDIO_CFG_T* cfg,
    BOOL loop);



// =============================================================================
// apbs_Pause
// -----------------------------------------------------------------------------
/// This function pauses the audio stream playback.
/// If the function returns
/// #APBS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param pause If \c TRUE, pauses a playing stream. If \c FALSE (and a
/// stream is paused), resumes a paused stream.
/// @return #APBS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #APBS_ERR_NO if it can execute the command.
// =============================================================================
DEPRECATED PUBLIC APBS_ERR_T apbs_Pause(BOOL pause);



// =============================================================================
// apbs_Stop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback.
/// If the function returns
/// #APBS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #APBS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #APBS_ERR_NO if it can execute the command.
// =============================================================================
DEPRECATED PUBLIC APBS_ERR_T apbs_Stop(VOID);


// =============================================================================
// apbs_GetBufPosition
// -----------------------------------------------------------------------------
/// This function returns the current position in the stream buffer.
/// This position points the next bytes to be played.
/// The bytes before this position might not yet have been played at the
/// time this position is read, but are in the process pipe to be played.
///
/// @return The pointer to the next sample to be fetched from the stream buffer.
// =============================================================================
DEPRECATED PUBLIC UINT32* apbs_GetBufPosition(VOID);

DEPRECATED PUBLIC UINT32 apbs_GetRemain(VOID);
PUBLIC void apbs_GetWriteBuffer( UINT32 **buffer, UINT32 *buf_len );

DEPRECATED PUBLIC void apbs_DataFinished( void );

// =============================================================================
// apbs_AddedData
// -----------------------------------------------------------------------------
/// When a stream buffer is played in loop mode, already played data can be
/// replaced by new ones to play a long song seamlessly. The APBS service needs
/// to be informed about the number of new bytes to play in the buffer. This
/// is the role of the #apbs_AddedData function.
///
/// It is mandatory to call this function when a stream is played in loop mode
/// with handler defined.
///
/// If the function returns
/// #APBS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param addedDataBytes Number of bytes added to the buffer.
/// @return #APBS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #APBS_ERR_NO if it can execute the command.
// =============================================================================
DEPRECATED PUBLIC APBS_ERR_T apbs_AddedData(UINT32 addedDataBytes);


PUBLIC void apbs_SetBuffer(UINT32 *buffer, UINT32 buf_len);







// =============================================================================
// apbs_UserMsgHandle
// -----------------------------------------------------------------------------
// =============================================================================

PUBLIC VOID apbs_UserMsgHandle(COS_EVENT *pnMsg);


///  @} <- End of the apbs group



#endif // _APBS_M_H_


