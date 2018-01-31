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


#ifndef _MRS_M_H_
#define _MRS_M_H_

#include "cs_types.h"

#include "lcdd_m.h"


/// @mainpage Multimedia Record Service
/// @author Coolsand Technologies, Inc.
/// @version $Revision$
/// @date $Date$
///
/// This service provides a multimedia recorder. You can record audio and/or
/// video, from the phone microphone and camera sensor, to a file or a buffer.
/// @todo The video record capabilities are not implemented yet.
///
/// @par Recording into a buffer: NOT SUPPORTED
/// This feature is <b>not supported</b> yet and its implementation is
/// not planned as recording to a buffer seems useless for now.
///
/// @par Recording in a file
/// When we record in a file, if it doesn't exist it is created. If the file
/// already exists, data can either be replaced or appended, from any where
/// in the file, thanks to the #mrs_Seek function. This feature allows to
/// change parts of a recorded file. The MPS (@ref mps) service should
/// be used to play a recorded file until the desired spot and then get
/// the position in the file where to seek before resuming recording.
/// @todo Implement this feature.
///
/// @par Audio
/// When recording to a file, the codec used for the audio is chosen at the
/// opening of the file. The configuration specific to a codec is also
/// defined at the opening of the file, and it cannot be changed afterwards.
/// If the file already exists, and we are appending or changing data,
/// the codec used and its configuration must
/// be the same. Otherwise the #MRS_ERR_INCOMPATIBLE_AUDIO_CODEC error is
/// returned.
///
/// @par Video
/// Recording video is optional: an audio file can be recorded by MRS,
/// by defining a NULL video codec.
/// @todo Specify how the no codec info is passed. (A null pointer to
/// a configuration structure.)
/// When recording video, the codec used for the video is chosen at the
/// opening of the file. The configuration specific to a codec is also
/// defined at the opening of the file, and it cannot be changed afterwards.
/// If the file already exists, the codec used and its configuration must
/// be the same. Otherwise the #MRS_ERR_INCOMPATIBLE_VIDEO_CODEC error is
/// returned. \n
///
/// @par Audio/Video synchronisation
/// @todo Shall audio/video synchronization be handle at the MRS level
/// or below. What kind of synchronization can we hope for when the
/// system seems to suffer at roughly 7fps ?
///
/// @par Frequency Bars
/// While recording, frequency bars corresponding to the recorded
/// frequencies can be displayed. They are returned through the type
/// #MRS_FREQUENCY_BARS_T by a call to the function #mrs_GetFrequencyBars.
/// This is purely aimed at an aestetic goal.
///
/// @par Handle
/// An opened file or an opened buffer is manipulated in MRS as a
/// handle of type #MRS_HANDLE_T. This structure holds on all the
/// information needed about the media to record, and offers an abstraction
/// above the media kind (buffer or file). Thus, it is used as
/// the parameter to pass to MRS functions (#mrs_Record, #mrs_SetConfig,
/// etc ...).
///
/// @par Information about the record / Tagging
/// Information about the file being recorded can sometimes be recorded to
/// the file, depending on the codec used (For example, the ID3 tags of
/// MP3 files.) When a file is opened, its handle can be used to set
/// these info (#mrs_SetInfo), or to recover them in case that the file
/// already exists (#mrs_GetInfo). When the file format used does not
/// handle tagging, an error is returned (#MRS_ERR_TAGGING_UNAVAILABLE).
///
///
/// The API are described in the following group :
/// - @ref mrs
///
/// @defgroup mrs Multimedia player service (MRS)
/// @{
///

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// MRS_ERR_T
// -----------------------------------------------------------------------------
/// List of MRS error
// =============================================================================
typedef enum
{
    /// No error
    MRS_ERR_NO = 0,
    /// Format not supported
    MRS_ERR_UNKNOWN_FORMAT,
    /// File could no be created.
    MRS_ERR_FILE_ERROR,
    /// You use MRS without call #mrs_Open or after #mrs_Close
    MRS_ERR_SERVICE_NOT_OPEN,
    /// The audio codec to use is not compatible with the file to record
    MRS_ERR_INCOMPATIBLE_AUDIO_CODEC,
    /// The video codec to use is not compatible with the file to record
    MRS_ERR_INCOMPATIBLE_VIDEO_CODEC,
    /// Tagging is not possible for the codec/container used.
    MRS_ERR_TAGGING_UNAVAILABLE,
    /// Action invalid, in the current state of MRS
    MRS_ERR_INVALID
} MRS_ERR_T;


// =============================================================================
// MRS_MSG_STATE_T
// -----------------------------------------------------------------------------
/// MRS Message type
// =============================================================================
typedef enum
{
    MRS_STATE_RECORD,
    MRS_STATE_EOF,
    MRS_STATE_STOP,
    MRS_STATE_PAUSE,
    MRS_STATE_SEEK,
    ///
    MRS_STATE_NO_MORE_DISK_SPACE,
    MRS_STATE_OUT_OF_MEMORY,

} MRS_STATE_T;


// =============================================================================
// MRS_EQUALIZER_MODE_T
// -----------------------------------------------------------------------------
/// List of equalizer mode
/// TODO Has this a sense on MRS ?
// =============================================================================
typedef enum
{
    MRS_EQUALIZER_MODE_OFF         = -1,
    MRS_EQUALIZER_MODE_NORMAL      =  0,
    MRS_EQUALIZER_MODE_BASS        =  1,
    MRS_EQUALIZER_MODE_DANCE       =  2,
    MRS_EQUALIZER_MODE_CLASSICAL   =  3,
    MRS_EQUALIZER_MODE_TREBLE      =  4,
    MRS_EQUALIZER_MODE_PARTY       =  5,
    MRS_EQUALIZER_MODE_POP         =  6,
    MRS_EQUALIZER_MODE_ROCK        =  7,
    MRS_EQUALIZER_MODE_CUSTOM      =  8
} MRS_EQUALIZER_MODE_T;


// =============================================================================
// MRS_AUDIO_PATH_T
// -----------------------------------------------------------------------------
/// List of audio path
// =============================================================================
typedef enum
{
    MRS_AUDIO_PATH_RECEIVER,
    MRS_AUDIO_PATH_HEADSET,
    MRS_AUDIO_PATH_LOUD_SPEAKER,
    MRS_AUDIO_PATH_BLUETOOTH,

    MRS_AUDIO_PATH_QTY
} MRS_AUDIO_PATH_T;


// =============================================================================
// MRS_VIDEO_PATH_T
// -----------------------------------------------------------------------------
/// List of the video path.
/// NOTA: TV is not implemented yet.
// =============================================================================
typedef enum
{
    /// The record source is the camera.
    MRS_VIDEO_PATH_CAMERA,
    /// Record from the phone TV receiver.
    /// @todo Implement.
    MRS_VIDEO_PATH_TV,

    MRS_VIDEO_PATH_QTY
} MRS_VIDEO_PATH_T;


// =============================================================================
// MRS_AUDIO_CFG_T
// -----------------------------------------------------------------------------
/// Audio configuration: interface used, levels ...
// =============================================================================
typedef struct
{
    /// Define input interface
    MRS_AUDIO_PATH_T     audioPath;

    /// Audio volume for the microphone (0-1)
    UINT8                volume;

    /// Equalizer mode
    MRS_EQUALIZER_MODE_T equalizerMode;
} MRS_AUDIO_CFG_T;


// =============================================================================
// MRS_CAMERA_FLASH_T
// -----------------------------------------------------------------------------
/// Possible values for the camera flash parameter.
// =============================================================================
typedef enum
{
    MRS_CAMERA_FLASH_NONE           = 0x00,
    MRS_CAMERA_FLASH_AUTO           = 0x01,
    MRS_CAMERA_FLASH_FORCED         = 0x02,
    MRS_CAMERA_FLASH_FILLIN         = 0x03,
    MRS_CAMERA_FLASH_REDEYEREDUCE   = 0x04
} MRS_CAMERA_FLASH_T;


// =============================================================================
// MRS_CAMERA_EXPOSURE_T
// -----------------------------------------------------------------------------
/// Possible values for the camera exposure parameter.
// =============================================================================
typedef enum
{
    MRS_CAMERA_EXPOSURE_AUTO        = 0x00,
    MRS_CAMERA_EXPOSURE_NEGATIVE_2  = 0x01,
    MRS_CAMERA_EXPOSURE_NEGATIVE_1  = 0x02,
    MRS_CAMERA_EXPOSURE_POSITIVE_1  = 0x03,
    MRS_CAMERA_EXPOSURE_POSITIVE_2  = 0x04
} MRS_CAMERA_EXPOSURE_T;


// =============================================================================
// MRS_CAMERA_WHITE_BALANCE_T
// -----------------------------------------------------------------------------
/// Possible values for the camera white balance
/// parameter.
// =============================================================================
typedef enum
{
    MRS_CAMERA_WHITE_BALANCE_AUTO       = 0x00,
    MRS_CAMERA_WHITE_BALANCE_DAYLIGHT   = 0x01,
    MRS_CAMERA_WHITE_BALANCE_CLOUDY     = 0x02,
    MRS_CAMERA_WHITE_BALANCE_OFFICE     = 0x03,
    MRS_CAMERA_WHITE_BALANCE_TUNGSTEN   = 0x04
} MRS_CAMERA_WHITE_BALANCE_T;


// =============================================================================
// MRS_CAMERA_SPECIAL_EFFECT_T
// -----------------------------------------------------------------------------
/// Possible values for the camera special effect
/// selection.
// =============================================================================
typedef enum
{
    MRS_CAMERA_SPECIAL_EFFECT_NORMAL                = 0x00,
    MRS_CAMERA_SPECIAL_EFFECT_ANTIQUE               = 0x01,
    MRS_CAMERA_SPECIAL_EFFECT_REDISH                = 0x02,
    MRS_CAMERA_SPECIAL_EFFECT_GREENISH              = 0x03,
    MRS_CAMERA_SPECIAL_EFFECT_BLUEISH               = 0x04,
    MRS_CAMERA_SPECIAL_EFFECT_BLACKWHITE            = 0x05,
    MRS_CAMERA_SPECIAL_EFFECT_NEGATIVE              = 0x06,
    MRS_CAMERA_SPECIAL_EFFECT_BLACKWHITE_NEGATIVE   = 0x07
} MRS_CAMERA_SPECIAL_EFFECT_T;


// =============================================================================
// MRS_CAMERA_OPTICAL_ZOOM_T
// -----------------------------------------------------------------------------
/// Possible values for the camera zoom value.
// =============================================================================
typedef enum
{
    MRS_CAMERA_OPTICAL_ZOOM_X1,
    MRS_CAMERA_OPTICAL_ZOOM_X2,
    MRS_CAMERA_OPTICAL_ZOOM_X3,
    MRS_CAMERA_OPTICAL_ZOOM_X4
} MRS_CAMERA_OPTICAL_ZOOM_T;


// =============================================================================
// MRS_CAMERA_DIGITAL_ZOOM_T
// -----------------------------------------------------------------------------
/// Possible values for the camera digital zoom.
// =============================================================================
typedef enum
{
    MRS_CAMERA_DIGITAL_ZOOM_X1,
    MRS_CAMERA_DIGITAL_ZOOM_X2,
    MRS_CAMERA_DIGITAL_ZOOM_X3,
    MRS_CAMERA_DIGITAL_ZOOM_X4
} MRS_CAMERA_DIGITAL_ZOOM_T;


// =============================================================================
// MRS_CAMERA_CONTRAST_T
// -----------------------------------------------------------------------------
/// Possible values for the camera contrast.
// =============================================================================
typedef enum
{
    MRS_CAMERA_CONTRAST_0,
    MRS_CAMERA_CONTRAST_1,
    MRS_CAMERA_CONTRAST_2,
    MRS_CAMERA_CONTRAST_3,
    MRS_CAMERA_CONTRAST_4,
    MRS_CAMERA_CONTRAST_5,
    MRS_CAMERA_CONTRAST_6,
    MRS_CAMERA_CONTRAST_7
} MRS_CAMERA_CONTRAST_T;


// =============================================================================
// MRS_CAMERA_BRIGHTNESS_T
// -----------------------------------------------------------------------------
/// Possible values for the camera brightness.
// =============================================================================
typedef enum
{
    MRS_CAMERA_BRIGHTNESS_0,
    MRS_CAMERA_BRIGHTNESS_1,
    MRS_CAMERA_BRIGHTNESS_2,
    MRS_CAMERA_BRIGHTNESS_3,
    MRS_CAMERA_BRIGHTNESS_4,
    MRS_CAMERA_BRIGHTNESS_5,
    MRS_CAMERA_BRIGHTNESS_6,
    MRS_CAMERA_BRIGHTNESS_7
} MRS_CAMERA_BRIGHTNESS_T;



// =============================================================================
// MRS_CAMERA_NIGHT_MODE_T
// -----------------------------------------------------------------------------
/// Possible values for the camera night mode.
// =============================================================================
typedef enum
{
    MRS_CAMERA_NIGHT_MODE_OFF,
    MRS_CAMERA_NIGHT_MODE_ON
} MRS_CAMERA_NIGHT_MODE_T;



// =============================================================================
// MRS_VIDEO_EFFECTS_T
// -----------------------------------------------------------------------------
/// Configuration of the video effects appliable for MRS.
// =============================================================================
typedef union
{
    /// Camera configuration.
    struct
    {
        MRS_CAMERA_FLASH_T          flash;
        MRS_CAMERA_EXPOSURE_T       exposure;
        MRS_CAMERA_WHITE_BALANCE_T  whiteBalance;
        MRS_CAMERA_SPECIAL_EFFECT_T specialEffect;
        MRS_CAMERA_OPTICAL_ZOOM_T   opticalZoom;
        MRS_CAMERA_DIGITAL_ZOOM_T   digitalZoom;
        MRS_CAMERA_CONTRAST_T       contrast;
        MRS_CAMERA_BRIGHTNESS_T     brightness;
        MRS_CAMERA_NIGHT_MODE_T     nightMode;
    };

    /// TODO TV configuration.
    struct
    {
    };

} MRS_VIDEO_EFFECTS_T;


// =============================================================================
// MRS_VIDEO_ROTATION_T
// -----------------------------------------------------------------------------
/// List of audio path
// =============================================================================
typedef enum
{
    MRS_VIDEO_ROTATION_PORTRAIT = 0,
    MRS_VIDEO_ROTATION_LANDSCAPE
} MRS_VIDEO_ROTATION_T;

// =============================================================================
// MRS_VIDEO_CFG_T
// -----------------------------------------------------------------------------
/// video configuration: interface used, levels ...
// =============================================================================
typedef struct
{
    /// Define output interface
    MRS_VIDEO_PATH_T        videoPath;

    /// Describe rotation for video
    MRS_VIDEO_ROTATION_T    videoRotation;

    /// Special effects
    /// @todo Implement that type at the MRS level ?
    /// @todo These types are (re-)defined from CAMD
    /// to CAMS too ... Isn't there a way to define a kind of
    /// unified multimedia types, share between all those
    /// services.
    MRS_VIDEO_EFFECTS_T     effects;

} MRS_VIDEO_CFG_T;

// =============================================================================
// MRS_CFG_T
// -----------------------------------------------------------------------------
/// Video/audio configuration structure
/// @todo A better name is possible ?
// =============================================================================
typedef struct
{
    /// Audio configuration.
    MRS_AUDIO_CFG_T audio;

    /// Video configuration.
    MRS_VIDEO_CFG_T video;
} MRS_CFG_T;


// =============================================================================
// MRS_FILE_INFO_T
// -----------------------------------------------------------------------------
/// File information.
/// TODO This must be part of the MP3 configuration. To record some info one the
/// record being made.
/// ++ Is is possible to embed such data in other encoding ?
/// ++ Pourquoi on supporte pas l'ogg, au fait ? (<-- That point has nothing to
/// do with the little button).(that schmilbklichly stuff)
// =============================================================================
typedef struct
{
    UINT8 title [128];
    UINT8 artist[128];
    UINT8 album [128];
} MRS_FILE_INFO_T ;


// =============================================================================
// MRS_HANDLE_T
// -----------------------------------------------------------------------------
/// An handle is used for each file
// =============================================================================
typedef struct MRS_HANDLE_STRUCT_T* MRS_HANDLE_T;


// =============================================================================
// MRS_FREQUENCY_BARS_T
// -----------------------------------------------------------------------------
/// Audio track frequency bars:
/// TODO Also cool on the recording side ?
// =============================================================================
typedef INT16 MRS_FREQUENCY_BARS_T[16];


// =============================================================================
// MRS_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback use to notify a state changement on one handle. The different
/// states are describes with #MRS_STATE_T type
/// @param handle Current handle use
/// @param state The new state
// =============================================================================
typedef VOID (*MRS_CALLBACK_T)(MRS_HANDLE_T handle, MRS_STATE_T state);


// =============================================================================
// MRS_AUDIO_CODEC_T
// -----------------------------------------------------------------------------
/// This types describes the codec usable to record an audio stream with MRS.
/// @todo It is possible to reorganise MRS codec (group AMR), and to just keep
/// the AMR RING format.
// =============================================================================
typedef enum
{
    MRS_AUDIO_CODEC_AMR475,
    MRS_AUDIO_CODEC_AMR515,
    MRS_AUDIO_CODEC_AMR59,
    MRS_AUDIO_CODEC_AMR67,
    MRS_AUDIO_CODEC_AMR74,
    MRS_AUDIO_CODEC_AMR795,
    MRS_AUDIO_CODEC_AMR102,
    MRS_AUDIO_CODEC_AMR122,
    MRS_AUDIO_CODEC_FR,
    MRS_AUDIO_CODEC_HR,
    MRS_AUDIO_CODEC_EFR,
    MRS_AUDIO_CODEC_WAV,
    MRS_AUDIO_CODEC_AMR_RING,
    MRS_AUDIO_CODEC_MP3,
    MRS_AUDIO_CODEC_QTY
} MRS_AUDIO_CODEC_T;


// =============================================================================
// MRS_AUDIO_CODEC_AMR122_CFG_T
// -----------------------------------------------------------------------------
/// @todo Nothing to configure here ?
// =============================================================================
typedef struct
{
    /// @todo Nothing to configure here ?
} MRS_AUDIO_CODEC_AMR122_CFG_T;


// =============================================================================
// MRS_AUDIO_CODEC_WAV_CFG_T
// -----------------------------------------------------------------------------
/// Configuration structure for anything recorded as a wave file.
// =============================================================================
typedef struct
{
    UINT16  sampleRate;
    UINT8   channelNb;
    BOOL    voiceQuality;
} MRS_AUDIO_CODEC_WAV_CFG_T;


// =============================================================================
// MRS_AUDIO_CODEC_MP3_CFG_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    /// @todo Do some search to find out what's up
    /// there.
    UINT16  sampleRate;
    UINT8   channelNb;
    BOOL    voiceQuality;
    BOOL    vbr;
} MRS_AUDIO_CODEC_MP3_CFG_T;


// =============================================================================
// MRS_AUDIO_CODEC_CFG_T
// -----------------------------------------------------------------------------
/// That type describes the configuration to encode the audio recorded.
/// The codec used and its configuration are defined.
// =============================================================================
typedef struct
{
    /// Codec to use.
    MRS_AUDIO_CODEC_T type;
    /// Union of configuration. Depending on the
    /// codec used, the proper field of this union
    /// is to be used.
    union
    {
        MRS_AUDIO_CODEC_AMR122_CFG_T    amr122Cfg;
        MRS_AUDIO_CODEC_WAV_CFG_T       wavCfg;
        MRS_AUDIO_CODEC_MP3_CFG_T       mp3Cfg;
    };
} MRS_AUDIO_CODEC_CFG_T;


// =============================================================================
// MRS_VIDEO_CODEC_T
// -----------------------------------------------------------------------------
/// This types describes the codec usable to record a video stream with MRS.
// =============================================================================
typedef enum
{
    MRS_VIDEO_CODEC_MJPEG,
    MRS_VIDEO_CODEC_QTY
} MRS_VIDEO_CODEC_T;


// =============================================================================
// MRS_VIDEO_RESOLUTION_T
// -----------------------------------------------------------------------------
/// Describe the resolution of the video to capture.
// =============================================================================
typedef enum
{
    MRS_VIDEO_RESOLUTION_128X160,
    MRS_VIDEO_RESOLUTION_128X96,

    MRS_VIDEO_RESOLUTION_QTY
} MRS_VIDEO_RESOLUTION_T;


// =============================================================================
// MRS_VIDEO_CODEC_MJPEG_CFG_T
// -----------------------------------------------------------------------------
/// Configure the MJPEG encoder.
// =============================================================================
typedef struct
{
    /// Resolution of the Video
    MRS_VIDEO_RESOLUTION_T    resolution;
    /// Frame rate, per sec.
    UINT8    rate;
    /// JPEG High Quality ?
    BOOL    jpegHighQuality;
} MRS_VIDEO_CODEC_MJPEG_CFG_T;


// =============================================================================
// MRS_VIDEO_CODEC_CFG_T
// -----------------------------------------------------------------------------
/// That type describes the configuration to encode the video recorded.
/// The codec used and its configuration are defined.
// =============================================================================
typedef struct
{
    /// Codec to use.
    MRS_VIDEO_CODEC_T type;
    /// Union of configuration. Depending on the
    /// codec used, the proper field of this union
    /// is to be used.
    union
    {
        MRS_VIDEO_CODEC_MJPEG_CFG_T mjpegCfg;
    };
} MRS_VIDEO_CODEC_CFG_T;



// =============================================================================
// MRS_CONTAINER_T
// -----------------------------------------------------------------------------
/// This types describes the container to use to record a file with MRS.
// =============================================================================
typedef enum
{
    MRS_CONTAINER_RAW,
    MRS_CONTAINER_RIFF,
    MRS_CONTAINER_QTY
} MRS_CONTAINER_T;


// =============================================================================
// MRS_CONTAINER_RIFF_CFG_T
// -----------------------------------------------------------------------------
/// @todo Nothing to configure here ?
// =============================================================================
typedef struct
{
} MRS_CONTAINER_RIFF_CFG_T;


// =============================================================================
// MRS_CONTAINER_CFG_T
// -----------------------------------------------------------------------------
/// That type describes the configuration to encode the video recorded.
/// The codec used and its configuration are defined.
/// @todo Is there any interest at defining an option for to configure
/// that option ? (ie always RIFF.|)
// =============================================================================
typedef struct
{
    /// Codec to use.
    MRS_CONTAINER_T type;
    /// Union of configuration. Depending on the
    /// container used, the proper field of this union
    /// is to be used.
    union
    {
        MRS_CONTAINER_RIFF_CFG_T riffCfg;
    };
} MRS_CONTAINER_CFG_T;



// =============================================================================
// MRS_RECORD_CFG_T;
// -----------------------------------------------------------------------------
/// Configure all the recording parameters needed when opening a file for
/// record, that is:
///   - audio
///   - video
///   - container
///   .
/// When not recording video, just leave the pointer to the video configuration
/// at the NULL value.
/// When not recording any audio data, just leave the pointer to the the audio
/// configuration at the NULL value.
/// @todo Discuss if this mechanism cannot be replaced by a NONE code for video
/// and audio, when none is wished to be recorded.
/// @todo We would gain in usability with an integrated structure, not using
/// pointer (cf the previous todo).
// =============================================================================
typedef struct
{
    /// Pointer to the audio configuration. If NULL, no audio is recorded.
    MRS_AUDIO_CODEC_CFG_T*  audioCodecCfg;

    /// Pointer to the video configuration. If NULL. no video is recorded.
    MRS_VIDEO_CODEC_CFG_T*  videoCodecCfg;

    /// Pointer to the container configuration. (If NULL, a 'raw' file is
    /// recorded (only valid with only audio or only video)?)
    MRS_CONTAINER_CFG_T*    containerCfg;

    /// Audio IO configuration.
    MRS_AUDIO_CFG_T         audioCfg;

    /// Video IO configuration.
    MRS_VIDEO_CFG_T         videoCfg;

    /// Callback used to handle the information sent back by MRS
    /// about this file recording.
    MRS_CALLBACK_T          callback;
} MRS_RECORD_CFG_T;


// =============================================================================
// MRS_FILE_MODE_T
// -----------------------------------------------------------------------------
/// Describes the behaviour to have when opening a file for record.
/// @todo The Seek feature can override this, can't it ?
/// @todo This too could be a field of #MRS_RECORD_CFG_T.
// =============================================================================
typedef enum
{
    /// Create (TODO Replace if exists ?) file.
    MRS_FILE_MODE_CREATE,
    MRS_FILE_MODE_APPEND,

    MRS_FILE_MODE_QTY
} MRS_FILE_MODE_T;


// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// mrs_Open
// -----------------------------------------------------------------------------
/// Open media player service. Create the MRS task.
// =============================================================================
PUBLIC VOID mrs_Open(VOID);

// =============================================================================
// mrs_Close.
// -----------------------------------------------------------------------------
/// Close the Media Record Service and, destroy the MRS task. If a media is
/// recorded, an error is returned (#MRS_ERR_INVALID). If MRS is not opened,
/// another error is returned (#MRS_ERR_SERVICE_NOT_OPEN).
/// @return #MRS_ERR_NO, #MRS_ERR_SERVICE_NOT_OPEN, #MRS_ERR_INVALID.
// =============================================================================
PUBLIC MRS_ERR_T mrs_Close(VOID);

// =============================================================================
// mrs_OpenFile
// -----------------------------------------------------------------------------
/// Prepare a file for recording. If needed, create the file. If the file
/// already exists, its header is checked to verify compatibility with the
/// requested recording.
///
/// @param pHandle Pointer on address of handle allocated by MRS.
/// @param file Path of file.
/// @param mode Mode of the file opening/creation (Replace, append, etc ...)
/// @param config Describe the audio configuration and video configuration.
/// (See #MRS_RECORD_CFG_T)
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if MRS is not opened,
/// #MRS_ERR_FILE_ERROR if the file cannot be created or appended,
/// #MRS_ERR_INCOMPATIBLE_AUDIO_CODEC if the file already exists and must be
/// appened, but is from an incompatible audio codec with the one we want to
/// use, #MRS_ERR_INCOMPATIBLE_VIDEO_CODEC if the file already exists and must
/// be appened, but is from an incompatible audio codec with the one we want to
/// use. Otherwise #MRS_ERR_NO is returned.
/// @todo Is this 'open' name well chosen ? In a sense (replacing, appending,
/// it is, but for the basic 'record' feature, create could be more relevant).
// =============================================================================
PUBLIC MRS_ERR_T mrs_OpenFile(MRS_HANDLE_T*     pHandle,
                              UINT8*            file,
                              MRS_FILE_MODE_T   mode,
                              MRS_RECORD_CFG_T* config);


// =============================================================================
// mrs_CloseFile
// -----------------------------------------------------------------------------
/// Close a file and free handle.
///
/// @param handle Handle use for this action
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if mrs is not open,
/// else #MRS_ERR_NO.
// =============================================================================
PUBLIC MRS_ERR_T mrs_CloseFile(MRS_HANDLE_T handle);


// =============================================================================
// mrs_GetInfo
// -----------------------------------------------------------------------------
/// This function is used to return the information on file.
///
/// @param handle Handle use for this action
/// @return information on file (total length, bit rate, video or audio file,
// ...). See #MRS_FILE_INFO_T
// =============================================================================
PUBLIC MRS_FILE_INFO_T* mrs_GetInfo(MRS_HANDLE_T handle);


// =============================================================================
// mrs_SetInfo
// -----------------------------------------------------------------------------
/// This function is used to set the information on file. The codec used to
/// record a file does not necessarily handle the tagging of such information.
/// In that case, the #MRS_ERR_TAGGING_UNAVAILABLE error is returned.
///
/// @param handle Handle use for this action
/// @param info Tagging information to set.
/// @return
// =============================================================================
PUBLIC MRS_ERR_T mrs_SetInfo(MRS_HANDLE_T handle, MRS_FILE_INFO_T* info);


// =============================================================================
// mrs_Record
// -----------------------------------------------------------------------------
/// Record a file or resume the recording of the file previously paused by
/// #mrs_Pause. The record parameters
/// have been configured when the file was opened.
///
/// @param handle Handle use for this action.
/// @param seek Position in the file after which to record.
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if MRS is not open,
/// #MRS_ERR_INVALID if MRS is already used also #MRS_ERR_NO.
/// @todo This seek parameter can conflict with the mode under which the file
/// has been opened.
/// @todo Define seek unit and a way to understand it at the human level:
/// position in a streamed file, chunk number for RIFF ... and why not a
/// position in a chunk.
// =============================================================================
PUBLIC MRS_ERR_T mrs_Record(MRS_HANDLE_T handle, UINT32 seek);


// =============================================================================
// mrs_Pause
// -----------------------------------------------------------------------------
/// Pause the recording of a file.
///
/// @param handle Handle use for this action
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if mrs is not open,
/// #MRS_ERR_INVALID if MRS don't play with this handle also #MRS_ERR_NO
/// @todo Add a parameter (pointer) to return the current seek position ?
// =============================================================================
PUBLIC MRS_ERR_T mrs_Pause(MRS_HANDLE_T handle);


// =============================================================================
// mrs_SetConfig
// -----------------------------------------------------------------------------
/// Set audio volume
/// @param handle Handle use for this action
/// @param config New config must be apply
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if mrs is not open,
/// #MRS_ERR_INVALID if config is incorrect also #MRS_ERR_NO
// =============================================================================
PUBLIC MRS_ERR_T mrs_SetConfig(MRS_HANDLE_T handle, MRS_CFG_T* config);


// =============================================================================
// mrs_GetConfig
// -----------------------------------------------------------------------------
/// Get the current config
/// @param handle Handle use for this action
/// @param config Pointer where the config is save
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if mrs is not open also #MRS_ERR_NO
// =============================================================================
PUBLIC MRS_ERR_T mrs_GetConfig(MRS_HANDLE_T handle, MRS_CFG_T* config);


// =============================================================================
// mrs_Seek
// -----------------------------------------------------------------------------
/// Change the current position
///
/// @param handle Handle use for this action
/// @param time The new position in stream (second unit)
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if mrs is not open also #MRS_ERR_NO
// =============================================================================
PUBLIC MRS_ERR_T mrs_Seek(MRS_HANDLE_T handle, UINT32 time);


// =============================================================================
// mrs_Tell
// -----------------------------------------------------------------------------
/// Get the current position
///
/// @param handle Handle use for this action
/// @param pos Pointeur use to store the current position in stream
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if mrs is not open also #MRS_ERR_NO
// =============================================================================
PUBLIC MRS_ERR_T mrs_Tell(MRS_HANDLE_T handle, UINT32* pos);


// =============================================================================
// mrs_GetFrequencyBars
// -----------------------------------------------------------------------------
/// Get frequency bars
/// @return Return pointer on frequency bars
// =============================================================================
PUBLIC MRS_FREQUENCY_BARS_T* mrs_GetFrequencyBars(VOID);


///  @} <- End of the umrs group

#endif // _MRS_M_H_
