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


#ifndef _MPS_M_H_
#define _MPS_M_H_

#include "cs_types.h"

#include "lcdd_m.h"


/// @file mps_m.h
/// @mainpage Multimedia player service
///
/// This service provides multimedia player. You can play audio and video file
///
/// This document is composed of:
/// - @ref mps
///
/// @addtogroup mps Multimedia player service (mps)
/// @{
///

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// MPS_ERR_T
// -----------------------------------------------------------------------------
/// List of MPS error
// =============================================================================
typedef enum
{
    /// No error
    MPS_ERR_NO = 0,
    /// Format not supported
    MPS_ERR_UNKNOWN_FORMAT,
    /// Format not supported
    MPS_ERR_FILE_NOT_FOUND,
    /// You use MPS without call mps_Open or after mps_Close
    MPS_ERR_SERVICE_NOT_OPEN,
    /// Action invalid, in the current state of mps
    MPS_ERR_INVALID
} MPS_ERR_T;

// =============================================================================
// MPS_MSG_STATE_T
// -----------------------------------------------------------------------------
/// MPS Message type
// =============================================================================
typedef enum
{
    MPS_STATE_PLAY,
    MPS_STATE_EOF,
    MPS_STATE_STOP,
    MPS_STATE_CLOSE,
    MPS_STATE_PAUSE,
    MPS_STATE_SEEK
} MPS_STATE_T;

// =============================================================================
// MPS_EQUALIZER_MODE_T
// -----------------------------------------------------------------------------
/// List of equalizer mode
// =============================================================================
typedef enum
{
    MPS_EQUALIZER_MODE_OFF         = -1,
    MPS_EQUALIZER_MODE_NORMAL      =  0,
    MPS_EQUALIZER_MODE_BASS        =  1,
    MPS_EQUALIZER_MODE_DANCE       =  2,
    MPS_EQUALIZER_MODE_CLASSICAL   =  3,
    MPS_EQUALIZER_MODE_TREBLE      =  4,
    MPS_EQUALIZER_MODE_PARTY       =  5,
    MPS_EQUALIZER_MODE_POP         =  6,
    MPS_EQUALIZER_MODE_ROCK        =  7,
    MPS_EQUALIZER_MODE_CUSTOM      =  8
} MPS_EQUALIZER_MODE_T;

// =============================================================================
// MPS_AUDIO_PATH_T
// -----------------------------------------------------------------------------
/// List of audio path
// =============================================================================
typedef enum
{
    MPS_AUDIO_PATH_RECEIVER,
    MPS_AUDIO_PATH_HEADSET,
    MPS_AUDIO_PATH_LOUD_SPEAKER,
    MPS_AUDIO_PATH_BLUETOOTH
} MPS_AUDIO_PATH_T;

// =============================================================================
// MPS_VIDEO_ROTATION_T
// -----------------------------------------------------------------------------
/// List of audio path
// =============================================================================
typedef enum
{
    MPS_VIDEO_ROTATION_PORTRAIT = 0,
    MPS_VIDEO_ROTATION_LANDSCAPE
} MPS_VIDEO_ROTATION_T;


// =============================================================================
// MPS_BLIT_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback use when the video need blit
// =============================================================================
typedef VOID (*MPS_BLIT_CALLBACK_T)(LCDD_FBW_T* fbw, UINT16 x, UINT16 y);

// =============================================================================
// MPS_CONFIG_T
// -----------------------------------------------------------------------------
/// Video/audio configuration structure
// =============================================================================
typedef struct
{
    /// Define output interface
    MPS_AUDIO_PATH_T           audioPath;
    /// Frame buffer window use for video display
    LCDD_FBW_T*                windows;
    /// Position of video frame
    UINT16                     x, y;
    /// Describe rotation for video
    MPS_VIDEO_ROTATION_T       videoRotation;
    /// Audio volume (0-7)
    UINT8                      volume;
    /// Equalizer mode
    MPS_EQUALIZER_MODE_T       equalizerMode;
    /// Callback use for blit
    MPS_BLIT_CALLBACK_T        blitCallback;
} MPS_CONFIG_T;


// =============================================================================
// MPS_FILE_INFO_T
// -----------------------------------------------------------------------------
/// File information
// =============================================================================
typedef struct
{
    UINT32           height;
    UINT32           width;
    UINT32           nbFrames;
    UINT16           videoRate; /* Format 8.8 */
    UINT8            title [128];
    UINT8            artist[128];
    UINT8            album [128];
} MPS_FILE_INFO_T ;

// =============================================================================
// MPS_HANDLE_T
// -----------------------------------------------------------------------------
/// An handle is used for each file
// =============================================================================
typedef struct MPS_HANDLE_STRUCT_T* MPS_HANDLE_T;


// =============================================================================
// MPS_FREQUENCY_BARS_T
// -----------------------------------------------------------------------------
/// Audio track frequency bars
// =============================================================================
typedef UINT8 MPS_FREQUENCY_BARS_T[16];

// =============================================================================
// MPS_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback use to notify a state changement on one handle. The different
/// states are describes with #MPS_STATE_T type
/// @param handle Current handle use
/// @param state The new state
// =============================================================================
typedef VOID (*MPS_CALLBACK_T)(MPS_HANDLE_T handle, MPS_STATE_T state);

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// mps_Open
// -----------------------------------------------------------------------------
/// Open media player service. Create the mps task
// =============================================================================
PUBLIC VOID mps_Open(VOID);

// =============================================================================
// mps_Close.
// -----------------------------------------------------------------------------
/// Close media player service and stop the current media if is started. Destroy
/// the mps task
// =============================================================================
PUBLIC MPS_ERR_T mps_Close(VOID);

// =============================================================================
// mps_OpenFile
// -----------------------------------------------------------------------------
/// Open file and decode file header
/// @param phandle Pointer on address of handle allocate by mps
/// @param file Path of file
/// @param callback Callback use for this file. See #MPS_CALLBACK_T
/// @param config Describe the audio configuration and video configuration see
/// #MPS_CONFIG_T
/// @return Return #MPS_ERR_SERVICE_NOT_OPEN if mps is not open,
/// #MPS_ERR_FILE_NOT_FOUND if file don't exist, #MPS_ERR_UNKNOWN_FORMAT if
/// format can't be read else #MPS_ERR_NO
// =============================================================================
PUBLIC MPS_ERR_T mps_OpenFile(MPS_HANDLE_T*  phandle,
                              UINT8*         file,
                              MPS_CALLBACK_T callback,
                              MPS_CONFIG_T*  config);

// =============================================================================
// mps_CloseFile
// -----------------------------------------------------------------------------
/// Close file and free handle
/// @param handle Handle use for this action
/// @return Return #MPS_ERR_SERVICE_NOT_OPEN if mps is not open else #MPS_ERR_NO
// =============================================================================
PUBLIC MPS_ERR_T mps_CloseFile(MPS_HANDLE_T handle);

// =============================================================================
// mps_Stop
// -----------------------------------------------------------------------------
/// Stop file
/// @param handle Handle use for this action
/// @return Return #MPS_ERR_SERVICE_NOT_OPEN if mps is not open else #MPS_ERR_NO
// =============================================================================
PUBLIC MPS_ERR_T mps_Stop(MPS_HANDLE_T handle);

// =============================================================================
// mps_OpenBuffer
// -----------------------------------------------------------------------------
/// Open handle and decode file header
/// @param phandle Pointer on address of handle allocate by mps
/// @param buffer Data memory zone
/// @size size Size of data
/// @param callback Callback use for this file. See #MPS_CALLBACK_T
/// @param config Describe the audio configuration and video configuration see
/// #MPS_CONFIG_T
/// @return Return #MPS_ERR_SERVICE_NOT_OPEN if mps is not open,
/// #MPS_ERR_UNKNOWN_FORMAT if format can't be read else #MPS_ERR_NO
// =============================================================================
PUBLIC MPS_ERR_T mps_OpenBuffer(MPS_HANDLE_T*  phandle,
                                UINT8*         buffer,
                                UINT32         size,
                                MPS_CALLBACK_T callback,
                                MPS_CONFIG_T*  config);

// =============================================================================
// mps_OpenBufferPcm8kmono
// -----------------------------------------------------------------------------
/// Open handle on pcm 8k mono mode
/// Use it for test only
/// @param phandle Pointer on address of handle allocate by mps
/// @param buffer Data memory zone
/// @size size Size of data
/// @param callback Callback use for this file. See #MPS_CALLBACK_T
/// @param config Describe the audio configuration and video configuration see
/// #MPS_CONFIG_T
/// @return Return #MPS_ERR_SERVICE_NOT_OPEN else #MPS_ERR_NO
// =============================================================================
DEPRECATED PUBLIC MPS_ERR_T mps_OpenBufferPcm8kmono(MPS_HANDLE_T*  phandle,
        UINT8*         buffer,
        UINT32         size,
        MPS_CALLBACK_T callback,
        MPS_CONFIG_T*  config);

// =============================================================================
// mps_CloseBuffer
// -----------------------------------------------------------------------------
/// Close and free handle
/// @param handle Handle use for this action
/// @return Return #MPS_ERR_SERVICE_NOT_OPEN if mps is not open else #MPS_ERR_NO
// =============================================================================
PUBLIC MPS_ERR_T mps_CloseBuffer(MPS_HANDLE_T handle);

// =============================================================================
// mps_GetInfo
// -----------------------------------------------------------------------------
/// This function is use to return the information on file
/// @param handle Handle use for this action
/// @return information on file (total length, bit rate, video or audio file,
// ...). See #MPS_FILE_INFO_T
// =============================================================================
PUBLIC MPS_FILE_INFO_T* mps_GetInfo(MPS_HANDLE_T handle);

// =============================================================================
// mps_Play
// -----------------------------------------------------------------------------
/// Play the file or resume file
/// @param handle Handle use for this action
/// @param nbPlayed It's use to define number loop of media. If it's 0, the
/// media is played one time
/// @return Return #MPS_ERR_SERVICE_NOT_OPEN if mps is not open,
/// #MPS_ERR_INVALID if MPS is already used also #MPS_ERR_NO
// =============================================================================
PUBLIC MPS_ERR_T mps_Play(MPS_HANDLE_T handle, UINT32 nbPlayed);

// =============================================================================
// mps_Pause
// -----------------------------------------------------------------------------
/// Pause the file
/// @param handle Handle use for this action
/// @return Return #MPS_ERR_SERVICE_NOT_OPEN if mps is not open,
/// #MPS_ERR_INVALID if MPS don't play with this handle also #MPS_ERR_NO
// =============================================================================
PUBLIC MPS_ERR_T mps_Pause(MPS_HANDLE_T handle);

// =============================================================================
// mps_SetConfig
// -----------------------------------------------------------------------------
/// Set audio volume
/// @param handle Handle use for this action
/// @param config New config must be apply
/// @return Return #MPS_ERR_SERVICE_NOT_OPEN if mps is not open,
/// #MPS_ERR_INVALID if config is incorrect also #MPS_ERR_NO
// =============================================================================
PUBLIC MPS_ERR_T mps_SetConfig(MPS_HANDLE_T handle, MPS_CONFIG_T* config);

// =============================================================================
// mps_GetConfig
// -----------------------------------------------------------------------------
/// Get the current config
/// @param handle Handle use for this action
/// @param config Pointer where the config is save
/// @return Return #MPS_ERR_SERVICE_NOT_OPEN if mps is not open also #MPS_ERR_NO
// =============================================================================
PUBLIC MPS_ERR_T mps_GetConfig(MPS_HANDLE_T handle, MPS_CONFIG_T* config);

// =============================================================================
// mps_Seek
// -----------------------------------------------------------------------------
/// Change the current position
/// @param handle Handle use for this action
/// @param time The new position in stream (second unit)
/// @return Return #MPS_ERR_SERVICE_NOT_OPEN if mps is not open also #MPS_ERR_NO
// =============================================================================
PUBLIC MPS_ERR_T mps_Seek(MPS_HANDLE_T handle, UINT32 time);

// =============================================================================
// mps_Tell
// -----------------------------------------------------------------------------
/// Get the current position
/// @param handle Handle use for this action
/// @param pos Pointeur use to store the current position in stream
/// @return Return #MPS_ERR_SERVICE_NOT_OPEN if mps is not open also #MPS_ERR_NO
// =============================================================================
PUBLIC MPS_ERR_T mps_Tell(MPS_HANDLE_T handle, UINT32* pos);

// =============================================================================
// mps_GetFrequencyBars
// -----------------------------------------------------------------------------
/// Get frequency bars
/// @return Return pointer on frequency bars
// =============================================================================
PUBLIC MPS_FREQUENCY_BARS_T* mps_GetFrequencyBars(VOID);

///  @} <- End of the umps group

#endif // _MPS_M_H_
