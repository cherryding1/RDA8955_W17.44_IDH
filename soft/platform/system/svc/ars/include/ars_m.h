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



#ifndef _ARS_M_H_
#define _ARS_M_H_

#include "cs_types.h"

#include "hal_aif.h"
#include "aud_m.h"

/// @file ars_m.h
/// @mainpage Audio Record Service API
/// @page ars_mainpage  Audio Record Service API
///
/// This service provides recording capabilities for various encoded streams.
/// They can be recorded in loop or not and interrupts are generated when the
/// middle and the end of the buffer are reached.
///
/// @par Configuration requirements
/// HAL and AUD must be poperly configured in order to use the aps service.
///
///
/// The API is detailed in the following group: @ref ars
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
/// @defgroup ars Audio Record Service (APS)
/// @{
///

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// ARS_REC_MODE_T
// -----------------------------------------------------------------------------
/// This type describes the encoding mode used in a stream to record.
// =============================================================================
typedef enum
{
    ARS_REC_MODE_AMR475,
    ARS_REC_MODE_AMR515,
    ARS_REC_MODE_AMR59,
    ARS_REC_MODE_AMR67,
    ARS_REC_MODE_AMR74,
    ARS_REC_MODE_AMR795,
    ARS_REC_MODE_AMR102,
    ARS_REC_MODE_AMR122,
    ARS_REC_MODE_FR,
    ARS_REC_MODE_HR,
    ARS_REC_MODE_EFR,
    ARS_REC_MODE_AMR_RING,
    ARS_REC_MODE_PCM,
    ARS_REC_MODE_DAF,
    ARS_REC_MODE_INVALID,

    ARS_REC_MODE_QTY
} ARS_REC_MODE_T;



// =============================================================================
// ARS_ERR_T
// -----------------------------------------------------------------------------
/// Error types for the AUD module.
// =============================================================================
typedef enum
{
    /// No error occured
    ARS_ERR_NO,

    /// An attempt to access a busy resource failed
    /// (Resource use not possible)
    ARS_ERR_RESOURCE_BUSY,

    /// Attempt to open a resource already opened,
    /// (no need to open again to use).
    ARS_ERR_ALREADY_OPENED,

    /// Timeout while trying to access the resource
    ARS_ERR_RESOURCE_TIMEOUT,

    /// Invalid parameter
    ARS_ERR_BAD_PARAMETER,

    /// The specified interface does not exist
    ARS_ERR_NO_ITF,

    /// What ?
    ARS_ERR_UNKNOWN,


    ARS_ERR_QTY
} ARS_ERR_T;


// =============================================================================
// ARS_STATUS_T
// -----------------------------------------------------------------------------
/// Status returned by the ARS to the calling entity. The possible value
/// describes various information about the status of the play.
// =============================================================================
typedef enum
{
    ARS_STATUS_MID_BUFFER_REACHED,
    ARS_STATUS_END_BUFFER_REACHED,
    ARS_STATUS_NO_MORE_DATA,
    ARS_STATUS_ERR,

    ARS_STATUS_QTY
} ARS_STATUS_T;



// =============================================================================
// ARS_USER_HANDLER_T
// -----------------------------------------------------------------------------
/// To give back status to the calling task, the APS needs to be given a
/// callback function. This function is called whenever an event occurs
/// which needs to be reported back to the APS used. This status is reported
/// as the parameter of the function. The function is passed as a parameter to
/// the function #ars_Record. This type is the type describing such a function.
///
/// Note: This function must be as minimalist as possible, as it will be
/// executed during an interrupt. It should for example only send one event
/// to a task, asking for data refilling of the buffer. The data copy is
/// then done by the task, when it is rescheduled, but not directly by the
/// user handler.
// =============================================================================
typedef VOID (*ARS_USER_HANDLER_T)(ARS_STATUS_T);


// =============================================================================
// ARS_AUDIO_CFG_T
// -----------------------------------------------------------------------------
/// Configuration structure.
///
/// A configuration structure allows to record a stream with the proper configuration
/// set as far as the audio interface and the decoding are concerned.
/// @todo Expand this structure whether needed.
// =============================================================================
typedef struct
{
    /// Mic level
    AUD_MIC_LEVEL_T    micLevel;
    AUD_SPK_LEVEL_T     spkLevel;
    /// Pointer to a filter to apply to the stream
    UINT32*     filter;

} ARS_AUDIO_CFG_T;





// =============================================================================
// ARS_ENC_STREAM_T
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
    ARS_REC_MODE_T mode;

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
    ARS_USER_HANDLER_T handler;

} ARS_ENC_STREAM_T;





// =============================================================================
//  FUNCTIONS
// =============================================================================




// =============================================================================
// ars_Setup
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
/// @return #ARS_ERR_NO it can execute the configuration.
// =============================================================================
PUBLIC ARS_ERR_T ars_Setup(AUD_ITF_T itf, CONST ARS_AUDIO_CFG_T* cfg);



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
/// ARS can a user function
/// at the middle or at the end of the playback.
///
/// @param itf Interface on which play that stream
/// @param stream Stream to play. Handler called at the middle and end of the buffer
/// are defined there.
/// @param cfg The configuration set applied to the audio interface
/// @param loop \c TRUE if the buffer is played in loop mode.
/// @return #ARS_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command, or if VPP is unavailable.
///         #ARS_ERR_NO it can execute the command.
// =============================================================================
PUBLIC ARS_ERR_T ars_Record(
    CONST AUD_ITF_T      itf,
    CONST ARS_ENC_STREAM_T* stream,
    CONST ARS_AUDIO_CFG_T* cfg,
    BOOL loop);



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
PUBLIC ARS_ERR_T ars_Pause(BOOL pause);



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
PUBLIC ARS_ERR_T ars_Stop(VOID);


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
PUBLIC ARS_ERR_T ars_RecordStop(VOID);


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
PUBLIC UINT32* ars_GetBufPosition(VOID);


// =============================================================================
// ars_ReadData
// -----------------------------------------------------------------------------
/// When a stream buffer is recorded, already recorded data can be replaced by new
/// ones to record a long song seamlessly. The APS service needs to be informed
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
PUBLIC ARS_ERR_T ars_ReadData(UINT32 addedDataBytes);



// =============================================================================
// ars_AvailableData
// -----------------------------------------------------------------------------
/// @return The number of bytes ready to be read in the buffer.
///
// =============================================================================
PUBLIC UINT32 ars_AvailableData(VOID);


#ifdef CHIP_DIE_8955
PUBLIC VOID ars_EnFMRecNewPath(BOOL en);
#endif
///  @} <- End of the aps group



#endif // _ARS_M_H_

