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



#ifndef _AVRS_M_H_
#define _AVRS_M_H_

#include "cs_types.h"

#include "hal_aif.h"
#include "aud_m.h"
#include "lcdd_m.h"
#include "imgs_m.h"

/// @file avrs_m.h
/// @mainpage Audio Record Service API
/// @page avrs_mainpage  Audio Record Service API
///
/// This service provides recording capabilities for various encoded streams.
/// They can be recorded in loop or not and interrupts are generated when the
/// middle and the end of the buffer are reached.
///
/// @par Configuration requirements
/// HAL and AUD must be poperly configured in order to use the aps service.
///
///
/// The API is detailed in the following group: @ref avrs
///
/// @todo Change the preview mechanism to only use one frame buffer window,
/// without relying on a frame buffer window stream/swap buffer.
///
/// @defgroup avrs Audio Video Record Service (AVRS)
/// @{
///

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// AVRS_EVENT_ID_T
// -----------------------------------------------------------------------------
/// This type describes the event Id used by the various events
/// sent by AVRS.
// =============================================================================
typedef enum
{
    /// Event sent when enough data are recorded
    /// and need to be fetched from the buffer to be used
    /// by the upper layer (Typically, record in a file)
    /// (4000000001)
    AVRS_EVENT_ID_AUDIO                = 0xEE6B2801,

    /// Event sent when a video frame has been recorded.
    /// The event core contains details about the recorded
    /// data: Pointer to the frame and its size. (cf.
    /// #AVRS_EVENT_T.)
    AVRS_EVENT_ID_VIDEO                = 0xEE6B2802,

} AVRS_EVENT_ID_T;

// =============================================================================
// AVRS_REC_AUDIO_MODE_T
// -----------------------------------------------------------------------------
/// This type describes the encoding mode used in a stream to record.
// =============================================================================
typedef enum
{
    AVRS_REC_AUDIO_MODE_AMR475,
    AVRS_REC_AUDIO_MODE_AMR515,
    AVRS_REC_AUDIO_MODE_AMR59,
    AVRS_REC_AUDIO_MODE_AMR67,
    AVRS_REC_AUDIO_MODE_AMR74,
    AVRS_REC_AUDIO_MODE_AMR795,
    AVRS_REC_AUDIO_MODE_AMR102,
    AVRS_REC_AUDIO_MODE_AMR122,
    AVRS_REC_AUDIO_MODE_FR,
    AVRS_REC_AUDIO_MODE_HR,
    AVRS_REC_AUDIO_MODE_EFR,
    AVRS_REC_AUDIO_MODE_PCM,
    AVRS_REC_AUDIO_MODE_AMR_RING,
    AVRS_REC_AUDIO_MODE_MP3,
    AVRS_REC_AUDIO_MODE_INVALID,
    // FIXME: Can't we spare this ?
    AVRS_REC_AUDIO_MODE_NONE,

    AVRS_REC_AUDIO_MODE_QTY
} AVRS_REC_AUDIO_MODE_T;



// =============================================================================
// AVRS_REC_VIDEO_MODE_T
// -----------------------------------------------------------------------------
/// This type describes the encoding mode used in a stream to record.
// =============================================================================
typedef enum
{
    AVRS_REC_VIDEO_MODE_NONE,
    AVRS_REC_VIDEO_MODE_RAW,
    AVRS_REC_VIDEO_MODE_MJPEG,
    AVRS_REC_VIDEO_MODE_INVALID,

    AVRS_REC_VIDEO_MODE_QTY
} AVRS_REC_VIDEO_MODE_T;



// =============================================================================
// AVRS_ERR_T
// -----------------------------------------------------------------------------
/// Error types for the AUD module.
// =============================================================================
typedef enum
{
    /// No error occured
    AVRS_ERR_NO,

    /// An attempt to access a busy resource failed
    /// (Resource use not possible)
    AVRS_ERR_RESOURCE_BUSY,

    /// An attempt to use an inexisting device was
    /// intercepted.
    AVRS_ERR_NO_DEVICE,

    /// Attempt to open a resource already opened,
    /// (no need to open again to use).
    AVRS_ERR_ALREADY_OPENED,

    /// Timeout while trying to access the resource
    AVRS_ERR_RESOURCE_TIMEOUT,

    /// Invalid parameter
    AVRS_ERR_BAD_PARAMETER,
    AVRS_ERR_UNSUPPORTED_PARAM,
    AVRS_ERR_UNSUPPORTED_VALUE,

    /// The specified interface does not exist
    AVRS_ERR_NO_ITF,

    /// What ?
    AVRS_ERR_UNKNOWN,


    AVRS_ERR_QTY
} AVRS_ERR_T;


// =============================================================================
// AVRS_EVENT_T
// -----------------------------------------------------------------------------
/// Type of the events sent by AVRS.
/// Events are 4 32 bits words.
// =============================================================================
typedef struct
{
    /// Id of the event, to discriminate it from the other
    /// events in the system.
    AVRS_EVENT_ID_T  id;
    /// AVRS write pointer position.
    /// This pointer is either in the audio buffer or
    /// in the video buffer, depending on the kind of the
    /// \c id event.
    UINT32* bufferPos;
    /// Size of the frame, in bytes.
    UINT32  frameSize;
    UINT32  reserved[1];
} AVRS_EVENT_T ;



// =============================================================================
// AVRS_AUDIO_CFG_T
// -----------------------------------------------------------------------------
/// Configuration structure.
///
/// A configuration structure allows to record an audio stream with the proper
/// configuration set as far as the volume and equalisation are concerned.
/// @todo Expand this structure whether needed.
// =============================================================================
typedef struct
{
    /// Audio interface on which to record.
    AUD_ITF_T   itf;

    /// Mic level
    UINT8       micLevel;

    /// Equalizer mode
    UINT8       equalizer;

} AVRS_AUDIO_CFG_T;


// =============================================================================
// AVRS_VIDEO_ROTATION_T
// -----------------------------------------------------------------------------
/// List of audio path
// =============================================================================
typedef enum
{
    AVRS_VIDEO_ROTATION_PORTRAIT = 0,
    AVRS_VIDEO_ROTATION_LANDSCAPE
} AVRS_VIDEO_ROTATION_T;


// =============================================================================
// AVRS_VIDEO_CFG_T
// -----------------------------------------------------------------------------
/// Video configuration structure.
///
/// A configuration structure allows to record a video stream with the proper
/// configuration set.
/// This configuration concers whether the preview must be produced or not,
/// whether the recording is on or not, and the image properties
/// (rotation, ...)
/// @todo Add the configuration of the camera sensor (brightness etc) here ?
/// @todo Study the interest of keeping #AVRS_AUDIO_CFG_T and #AVRS_VIDEO_CFG_T
/// separated.
// =============================================================================
typedef struct
{
    /// Record the video ?
    BOOL    recordOn;

    /// Display the preview ?
    BOOL    previewOn;

    /// Video rotation.
    AVRS_VIDEO_ROTATION_T rotation;
    UINT16 zoom;

} AVRS_VIDEO_CFG_T;


// =============================================================================
// AVRS_ENC_AUDIO_STREAM_T
// -----------------------------------------------------------------------------
/// This type defines an encoded audio stream. All the fields might not be needed
/// depending on the encoding scheme used.
// =============================================================================
typedef struct
{
    ///
    UINT32          startAddress;

    /// Length of the audio buffer. That is not the total length of the song
    /// to record, as data can be fetched out of the buffer
    /// while recording.
    /// cf. API functions for details.
    UINT32          length;

    /// Mode used to record that stream
    AVRS_REC_AUDIO_MODE_T  mode;

    /// Sample Rate of the stream. Optional, this value is ignored
    /// by mode not needing it.
    HAL_AIF_FREQ_T  sampleRate;

    /// Number of channel of the stream. Optional,this value is ignored
    /// by mode not needing it.
    HAL_AIF_CH_NB_T channelNb;

    /// Is this encoded in voice mode (voice quality, mono, 8kHz)
    BOOL            voiceQuality;

    /// Mailbox where the events handling the audio record will be sent.
    /// If 0xFF, no event related to the audio recording is sent.
    UINT8           mbx;

} AVRS_ENC_AUDIO_STREAM_T;


// =============================================================================
// AVRS_FREQUENCY_BARS_T
// -----------------------------------------------------------------------------
/// Audio track frequency bars
// =============================================================================
typedef INT16 AVRS_FREQUENCY_BARS_T[16];


// =============================================================================
// AVRS_REC_VIDEO_RESOLUTION_T
// -----------------------------------------------------------------------------
/// Resolution of the video stream to record.
// =============================================================================
typedef enum
{
    AVRS_REC_VIDEO_RESOLUTION_QQVGA,
    AVRS_REC_VIDEO_RESOLUTION_QVGA
} AVRS_REC_VIDEO_RESOLUTION_T;


// =============================================================================
// AVRS_ENC_VIDEO_STREAM_T
// -----------------------------------------------------------------------------
/// This type defines an encoded video stream. All the fields might not be needed
/// depending on the encoding scheme used.
/// @todo Encoding could need more configuration parameters !
// =============================================================================
typedef struct
{
    ///
    UINT32                  startAddress;

    /// Length of the video buffer. That is not the total length of the moving
    /// picture to record, as data can be fetched out of the buffer
    /// while recording.
    /// cf. API functions for details.
    UINT32                  length;

    /// Mode used to record that stream
    AVRS_REC_VIDEO_MODE_T   mode;

    /// Video size, defined as a resolution
    AVRS_REC_VIDEO_RESOLUTION_T resolution;

    /// Sample Rate of the stream. Optional, this value is ignored
    /// by mode not needing it.
    /// @todo Define a type with a predefined set of possible
    /// frame rate values ? An enum whose value could be 1, 2, 3
    /// 4, 5, 6, 7, 8, 9, 10, 12, 13, 15, 24, 25, 29.997, 30. etc ?
    UINT32                  frameRate;

    /// Is this encoded in low quality mode. (faster encode time)
    BOOL                    lowQuality;

    /// Mailbox where the events handling the video record will be sent.
    /// If 0xFF, no event related to the video recording is sent.
    UINT8                   mbx;

    /// Pointer to the Frame Buffer Window used to display the record
    /// preview. Its Region Of Interest defines the area where to fit
    /// the preview picture.
    /// FIXME The <c> fb.buffer </c> field of this frame buffer window
    /// is not used. This frame  buffer window is only used to define
    /// the size and area where to display the preview.
    LCDD_FBW_T*             previewFbw;

    /// User callback used to handle the preview: it uses the \c previewFbw
    /// frame buffer window as a parameter to get the image to display
    /// on the screen, after having blent an OSD, for example.
    /// Setting this parameter to \c NULL will disable the preview.
    IMGS_DEC_CALLBACK_T     previewCallback;

} AVRS_ENC_VIDEO_STREAM_T;


// =============================================================================
// AVRS_PICTURE_STREAM_DEPTH
// -----------------------------------------------------------------------------
/// Depth of the stream for #avrs_StartPictureStream function.
/// This defines the number of swap buffer used by the streaming mechanism, in
/// order to avoid data corruption.
// =============================================================================
#define AVRS_PICTURE_STREAM_DEPTH   2


// =============================================================================
// AVRS_FBW_STREAM_T
// -----------------------------------------------------------------------------
/// Swap buffer of frame buffer window. Used by #avrs_StartPictureStream
// =============================================================================
typedef LCDD_FBW_T AVRS_FBW_STREAM_T[AVRS_PICTURE_STREAM_DEPTH];


// =============================================================================
// AVRS_ENC_BUFFER_STREAM_T
// -----------------------------------------------------------------------------
/// Swap buffer of encoded buffer. The #avrs_StartPictureStream will
/// successively used those parts to store the encoded frame.
// =============================================================================
typedef UINT8* AVRS_ENC_BUFFER_STREAM_T[AVRS_PICTURE_STREAM_DEPTH];





// =============================================================================
// AVRS_BUFFER_CONTEXT_T
// -----------------------------------------------------------------------------
/// Describe state of input buffer
/// This context is used for .... uh, what ?
/// TODO: Remove if really unnneeded !
// =============================================================================
typedef struct
{
    UINT8*        bufferAudioReadPosition;
    UINT32        audioRemainingSize;
} AVRS_BUFFER_CONTEXT_T ;



// =============================================================================
// AVRS_CAPTURE_HANDLER_T
// -----------------------------------------------------------------------------
/// This type describes the user function called by the capture process
/// after a frame has been captured. The parameters are two frame buffer
/// window pointers. The first receive the full resolution image, that the
/// handler can encode and save in memory. The second is  a preview quality
/// image, that can be used to show to the user.
// =============================================================================
typedef VOID (*AVRS_CAPTURE_HANDLER_T)(LCDD_FBW_T*, LCDD_FBW_T*);


// =============================================================================
// AVRS_DECODED_PICTURE_T
// -----------------------------------------------------------------------------
/// That structure is used to describe a non encoded picture, destination of
/// a IMGS operation.
///
/// The \c fbw field points towards a frame buffer window, where the picture
/// will be stored. The region of interest of \c fbw describes the size of
/// the picture. The \c rotation field describes the transformation to apply to
/// the picture.
///
/// When the operation for which such a structure is used is finished, the
/// callback recorded in the callback field, if any, is called with the \c fbw
/// frame buffer window as parameter.
// =============================================================================
typedef struct
{
    LCDD_FBW_T*             fbw;
    /// Rotate (transform) the image.
    /// TODO Define a better way to describe possible picture
    /// transformation.
    UINT32                  rotation;
    /// User handler called when the operation on this
    /// preview frame buffer window is over.
    IMGS_DEC_CALLBACK_T     callback;
} AVRS_DECODED_PICTURE_T;


// =============================================================================
// AVRS_ENCODED_PICTURE_T
// -----------------------------------------------------------------------------
/// That structure is used to describe an encoded picture, destination of
/// a IMGS operation.
///
/// The \c buffer field points towards the buffer where the encoded picture
/// will be stored. The encoding is configured by the \c encCfg field.
///
/// When the operation for which such a structure is used is finished, the
/// callback recorded in the callback field, if any, is called with the encoded
/// image length as a parameter.
// =============================================================================
typedef struct
{
    /// Pointer to the buffer where the encoded
    /// frame is recorded.
    UINT8*                  buffer;
    /// Rotate (transform) the image.
    /// TODO Define a better way to describe possible picture
    /// transformation.
    UINT32                  rotation;
    IMGS_ENCODING_PARAMS_T  encCfg;
    IMGS_ENC_CALLBACK_T     callback;
} AVRS_ENCODED_PICTURE_T;


// =============================================================================
//  FUNCTIONS
// =============================================================================




// =============================================================================
// avrs_Setup
// -----------------------------------------------------------------------------
/// Configure the avrs service..
///
/// This functions configures the playing of a stream on a given audio interface:
/// gain for the side tone and
/// the speaker, input selection for the microphone and output selection for
/// the speaker... (TODO complete)
///
/// @param audioCfg The configuration set applied to the audio interface. See
/// #AVRS_AUDIO_CFG_T for more details.
/// @param videoCfg The configuration set applied to the video recorder. See
/// #AVRS_VIDEO_CFG_T for more details.
/// @return #AVRS_ERR_NO it can execute the configuration.
// =============================================================================
PUBLIC AVRS_ERR_T avrs_Setup(CONST AVRS_AUDIO_CFG_T* audioCfg,
                             CONST AVRS_VIDEO_CFG_T* videoCfg);


// =============================================================================
// avrs_Record
// -----------------------------------------------------------------------------
/// Start the recording of an audio and/or video stream buffer : When the
/// record reach the end of the buffer, it carries on from the beginning.
///
/// Events are sent to the mailbox recorded in the stream configuration, to
/// give accounts on how the recording behaves.
///
/// This function records an audio and/or a video stream. Only one stream (video
/// or audio) can be recorded. When one stream is not to be recorded, leave
/// the corresponding parameter (audioStream or videoStream) at the \c NULL
/// value. One of the stream must not be NULL !
///
/// Configuration of the streams are passed as the \c audioCfg or
/// \c videoCfg parameter. In the case where one of the stream is not
/// recorded, its configuration is ignored, and thus its configuration
/// parameter can be (should be) \c NULL.
///
/// @param audioStream Audio stream to record. If the pointer is NULL, only
/// a video stream will be record.
/// @param videoStream Video stream to record. If the pointer is NULL, only
/// an audio stream will be record.
/// @param audioCfg The configuration set applied to the audio recording. Can
/// only be NULL if audioStream is NULL too.
/// @param videoCfg The configuration set applied to the video recording. Can
/// only be NULL if videoStream is NULL too.
/// @return #AVRS_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command, or if VPP is unavailable.
///         #AVRS_ERR_NO it can execute the command.
// =============================================================================
PUBLIC AVRS_ERR_T avrs_Record(CONST AVRS_ENC_AUDIO_STREAM_T*  audioStream,
                              CONST AVRS_ENC_VIDEO_STREAM_T*  videoStream,
                              CONST AVRS_AUDIO_CFG_T*         audioCfg,
                              CONST AVRS_VIDEO_CFG_T*         videoCfg);


// =============================================================================
// avrs_Stop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback.
/// If the function returns
/// #AVRS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #AVRS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AVRS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AVRS_ERR_T avrs_Stop(VOID);


// =============================================================================
// avrs_ReadData
// -----------------------------------------------------------------------------
/// When a stream buffer is recorded, already recorded data can be replaced by new
/// ones to record a long song seamlessly. The APS service needs to be informed
/// about the number of new bytes available in the buffer. This is the role of the
/// #avrs_ReadData function.
///
/// If the function returns
/// #AVRS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param readDataBytes Number of bytes read from the buffer.
/// @return #AVRS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AVRS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AVRS_ERR_T avrs_ReadData(UINT32 readDataBytes);






// ###############################################
// Function from the camera, to easily integrate
// CAMS features in AVRS !
// TODO Synthesize all of this into one integrated
// module that bind them all.
// ###############################################

// ==============================================================================
// avrs_Open
// ------------------------------------------------------------------------------
/// Open the AVRS service, and start the underlying task. This function must
/// be called before any other function of AVRS can be called.
// ==============================================================================
PUBLIC AVRS_ERR_T avrs_Open(VOID);


// =============================================================================
// avrs_Close
// -----------------------------------------------------------------------------
/// When the AVRS service is no more needed, this function closes the service
/// and put the task to sleep. No other function of the service can be called,
/// but the #avrs_Open function.
/// @return #AVRS_ERR_NO, #AVRS_ERR_RESOURCE_BUSY.
// =============================================================================
PUBLIC AVRS_ERR_T avrs_Close(VOID);


// =============================================================================
// avrs_StartPictureStream
// -----------------------------------------------------------------------------
/// TODO: Protect this function ?
/// Start the picture stream process. This is in fact a two way mechanism:
/// - a flow of raw picture is produced, to be used for preview features for
/// example.
/// - a flow of encoded picture is produced, to be used when recording a video
/// stream.
/// Either if those flows is optional: just set the corresponding parameter
/// to the \c NULL pointer value when calling avrs_StartPictureStream.
/// Typically, the camera preview would call this function that way:
/// <code> avrs_StartPictureStream(&previewPicture, NULL, &fbwStream, NULL)</code>
///
/// The frame buffer window and the encoded buffer are automatically refreshed
/// as long as the #avrs_StopPictureStream function is not called. To avoid
/// race conditions, swap buffers are used. The swap buffer used for the preview
/// frame buffer window and the encoded frame buffer are defined by the two
/// other parameters of the fucntion: \c fbwStream and \c encBufStream.
///
/// Everytime a preview frame has been filled in the frame buffer window, the
/// corresponding <c>previewPicture->callback</c> handler is called. The same
/// way, every time an encoded picture has been encoded, the corresponding
/// <c>encodedFrame->callback</c> is called.
///
/// Please refer to the #AVRS_DECODED_PICTURE_T documentation for details
/// about how this structure packs together the frame buffer window where
/// the picture is put and the user handler used to display this frame buffer
/// window, and to #AVRS_ENCODED_PICTURE_T for the respective information on
/// the encoded picture.
///
/// @param previewPicture Preview picture. Its frame buffer window field should
/// be \c NULL, since it is ignored as the \c fbwStream parameter will provide
/// the successive frame buffer window to share during streaming.
/// @param encodedFrame Encoded frame. Its buffer field should
/// be \c NULL, since it is ignored as the \c fbwStream parameter will provide
/// the successive frame buffer window to share during streaming.
/// @param fbwStream Pointer to an array of frame buffer windows, defined by the
/// #AVRS_FBW_STREAM_T type. During streaming, all the frame buffer window
/// of that structure will be used successively as the value of the frame buffer
/// window of \c previewPicture.
/// @param encBufStream Pointer to an array of buffers, defined by the
/// #AVRS_ENC_BUFFER_STREAM_T type. During streaming, all the buffers
/// of that structure will be used successively as the value of the \c buffer
/// field of \c encodedFrame.
///
/// @return #AVRS_ERR_NO, #AVRS_ERR_RESOURCE_BUSY, #AVRS_ERR_NO_DEVICE, ...
/// @todo How do we define the refresh rate ? (15fps/asap by tacit default)
// =============================================================================
PUBLIC AVRS_ERR_T avrs_StartPictureStream(AVRS_DECODED_PICTURE_T*   previewPicture,
        AVRS_ENCODED_PICTURE_T*   encodedFrame,
        AVRS_FBW_STREAM_T*        fbwStream,
        AVRS_ENC_BUFFER_STREAM_T* encBufStream);





// =============================================================================
// avrs_StopPictureStream
// -----------------------------------------------------------------------------
/// Stop the preview process started by #avrs_StartPictureStream. It is stopped after
/// the current frame processing is finished.
///
/// @return #AVRS_ERR_NO.
// =============================================================================
PUBLIC AVRS_ERR_T avrs_StopPictureStream(VOID);


// =============================================================================
// avrs_PictureStreamInProgress
// -----------------------------------------------------------------------------
/// Check if a preview is in progress.
/// @return \c TRUE if a preview is in progress, \c FALSE otherwise
// =============================================================================
PUBLIC BOOL avrs_PictureStreamInProgress(VOID);


// =============================================================================
// avrs_Capture
// -----------------------------------------------------------------------------
/// Capture one picture. The full size image is stored in the \c dataFbw, and
/// a preview version is stored in the \c prvwFbw frame buffer window. The
/// dataFbw frame buffer window shall be a full (ie the region of interest is
/// the full frame buffer) frame buffer window at the same size as the desired
/// resolution.
///
/// Once the capture is done, the \c captureHandler handler is called. It has
/// two parameters: \c dataFbw, holding the full resolution image, and
/// \c prvwFbw, with a scaled picture for the preview on the screen.
///
/// @param dataFbw Frame buffer window where the full resolution picture is
/// saved. Must have the \c resolution's width and height.
/// @param prvwFbw Frame buffer window where a preview of the picture is stored.
/// It can have any dimension.
/// @param captureHandler User handler called when a picture is captured.
/// The two frame buffer window are passed as parameters to the handler:
/// the first one with the full resolution captured photograph, and the other
/// one for the picture preview.
/// @return #AVRS_ERR_NO, #AVRS_ERR_RESOURCE_BUSY, #AVRS_ERR_NO_DEVICE, ...
// =============================================================================
PUBLIC AVRS_ERR_T avrs_Capture( LCDD_FBW_T*             dataFbw,
                                LCDD_FBW_T*             prvwFbw,
                                AVRS_CAPTURE_HANDLER_T  captureHandler);

#if 0
// =============================================================================
// avrs_SetParam
// -----------------------------------------------------------------------------
/// Set a camera parameter to a given value.
/// The parameter to set is among the values defined by the type #AVRS_PARAM_T.
/// If this parameter is not available for a given implementation of AVRS,
/// the #AVRS_ERR_UNSUPPORTED_PARAM error must be returned.
/// Depending on the AVRS_PARAM_XXX parameter to set, the value can either
/// be a proper UINT32 number, or a value of the AVRS_PARAM_XXX_T enum, might
/// it exists. This is detailed in the AVRS_PARAM_T definition.
/// If the value is not supported for this parameter, #AVRS_ERR_UNSUPPORTED_VALUE
/// is returned.
///
/// @param param Camera parameter to set.
/// @param value Value to set to the parameter to.
/// @return #AVRS_ERR_NO, #AVRS_ERR_UNSUPPORTED_PARAM, #AVRS_ERR_UNSUPPORTED_VALUE
/// ...
// =============================================================================
PUBLIC AVRS_ERR_T avrs_SetParam(AVRS_PARAM_T param, UINT32 value);
#endif

// =============================================================================
// avrs_ResizeRawToRequested
// -----------------------------------------------------------------------------
/// Fast resize of a Raw frame buffer window into a requested size frame
/// buffer window
/// @param rawFbw Camera source frame buffer window.
/// @param reqFbw Requested frame buffer window.
/// @param rotate Do we need to do a rotation of the screen?
/// @param zoom Zoom factor of the screen.
// =============================================================================
PUBLIC VOID avrs_ResizeRawToRequested( LCDD_FBW_T* rawFbw,
                                       LCDD_FBW_T* reqFbw,
                                       UINT32      rotate,
                                       UINT32      zoom);










///  @} <- End of the aps group



#endif // _AVRS_M_H_

