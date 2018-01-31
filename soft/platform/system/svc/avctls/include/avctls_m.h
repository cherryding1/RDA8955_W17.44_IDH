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


#ifndef _AVCTLS_M_H_
#define _AVCTLS_M_H_

#include "cs_types.h"

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// AVCTLS_ERROR_T
// -----------------------------------------------------------------------------
/// List errors
// =============================================================================
typedef enum
{
    AVCTLS_ERROR_NO           = 0,
    AVCTLS_ERROR_BUSY         = 1,
    AVCTLS_ERROR_ALREADY_OPEN = 2,
    AVCTLS_ERROR_NO_OPEN      = 3
} AVCTLS_ERROR_T;

// =============================================================================
// AVCTLS_PRIORITY_T
// -----------------------------------------------------------------------------
/// List of priority
// =============================================================================
typedef enum
{
    AVCTLS_PRIORITY_BACKGROUND = 0,
    AVCTLS_PRIORITY_MEDIUM     = 1,
    AVCTLS_PRIORITY_HIGH       = 2,
    AVCTLS_PRIORITY_VERY_HIGH  = 3,

    AVCTLS_PRIORITY_QTY        = 4
} AVCTLS_PRIORITY_T ;

// =============================================================================
// AVCTLS_HANDLE_T
// -----------------------------------------------------------------------------
/// An handle is used for each audio video instance
// =============================================================================
typedef struct AVCTLS_HANDLE_STRUCT_T* AVCTLS_HANDLE_T;

// =============================================================================
// AVCTLS_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback use to notify a state changement on one handle. The different
/// states are describes with #AVCTLS_STATE_T type
/// @param handle Current handle use
/// @param state The new state
// =============================================================================
typedef VOID (*AVCTLS_CALLBACK_T)(AVCTLS_HANDLE_T handle, AVCTLS_STATE_T state);

// =============================================================================
// AVCTLS_MSG_STATE_T
// -----------------------------------------------------------------------------
/// AVCTLS Message type
// =============================================================================
typedef enum
{
    AVCTLS_STATE_PLAY,
    AVCTLS_STATE_EOF,
    AVCTLS_STATE_STOP,
    AVCTLS_STATE_CLOSE,
    AVCTLS_STATE_PAUSE,
    AVCTLS_STATE_SEEK
} AVCTLS_STATE_T;

// =============================================================================
// AVCTLS_VIDEO_ROTATION_T
// -----------------------------------------------------------------------------
/// List of audio path
// =============================================================================
typedef enum
{
    AVCTLS_VIDEO_ROTATION_PORTRAIT = 0,
    AVCTLS_VIDEO_ROTATION_LANDSCAPE
} AVCTLS_VIDEO_ROTATION_T;

// =============================================================================
// AVCTLS_EQUALIZER_MODE_T
// -----------------------------------------------------------------------------
/// List of equalizer mode
// =============================================================================
typedef enum
{
    AVCTLS_EQUALIZER_MODE_OFF         = -1,
    AVCTLS_EQUALIZER_MODE_NORMAL      =  0,
    AVCTLS_EQUALIZER_MODE_BASS        =  1,
    AVCTLS_EQUALIZER_MODE_DANCE       =  2,
    AVCTLS_EQUALIZER_MODE_CLASSICAL   =  3,
    AVCTLS_EQUALIZER_MODE_TREBLE      =  4,
    AVCTLS_EQUALIZER_MODE_PARTY       =  5,
    AVCTLS_EQUALIZER_MODE_POP         =  6,
    AVCTLS_EQUALIZER_MODE_ROCK        =  7,
    AVCTLS_EQUALIZER_MODE_CUSTOM      =  8
} AVCTLS_EQUALIZER_MODE_T;

// =============================================================================
// AVCTLS_AUDIO_PATH_T
// -----------------------------------------------------------------------------
/// List of audio path
// =============================================================================
typedef enum
{
    AVCTLS_AUDIO_PATH_RECEIVER,
    AVCTLS_AUDIO_PATH_HEADSET,
    AVCTLS_AUDIO_PATH_LOUD_SPEAKER,
    AVCTLS_AUDIO_PATH_BLUETOOTH
} AVCTLS_AUDIO_PATH_T;

// =============================================================================
// AVCTLS_FILE_INFO_T
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
} AVCTLS_FILE_INFO_T;

// =============================================================================
// AVCTLS_HANDLE_T
// -----------------------------------------------------------------------------
/// An handle is used for each file
// =============================================================================
typedef struct AVCTLS_HANDLE_STRUCT_T* AVCTLS_HANDLE_T;


// =============================================================================
// AVCTLS_FREQUENCY_BARS_T
// -----------------------------------------------------------------------------
/// Audio track frequency bars
// =============================================================================
typedef UINT8 AVCTLS_FREQUENCY_BARS_T[16];

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

PUBLIC VOID avctls_Open(VOID);

PUBLIC VOID avctls_Close(VOID);

/// @param config Describe the audio configuration and video configuration see
/// #AVCTLS_CONFIG_T

PUBLIC AVCTLS_ERROR_T avctls_CreateInstance(AVCTLS_HANDLE_T*  pHandler,
        AVCTLS_PRIORITY_T priority,
        AVCTLS_CALLBACK_T callback);

PUBLIC AVCTLS_ERROR_T avctls_DestroyInstance(AVCTLS_HANDLE_T handler);

// =============================================================================
// avctls_OpenFile
// -----------------------------------------------------------------------------
/// Open file and decode file header
/// @param phandle Pointer on address of handle allocate by avctls
/// @param file Path of file
/// @return Return #AVCTLS_ERR_SERVICE_NOT_OPEN if avctls is not open,
/// #AVCTLS_ERR_FILE_NOT_FOUND if file don't exist, #AVCTLS_ERR_UNKNOWN_FORMAT if
/// format can't be read else #AVCTLS_ERR_NO
// =============================================================================
PUBLIC AVCTLS_ERR_T avctls_OpenFile(AVCTLS_HANDLE_T*  phandle,
                                    UINT8*            file);

// =============================================================================
// avctls_OpenBuffer
// -----------------------------------------------------------------------------
/// Open handle and decode file header
/// @param phandle Pointer on address of handle allocate by avctls
/// @param buffer Data memory zone
/// @size size Size of data
/// @param callback Callback use for this file. See #AVCTLS_CALLBACK_T
/// @param config Describe the audio configuration and video configuration see
/// #AVCTLS_CONFIG_T
/// @return Return #AVCTLS_ERR_SERVICE_NOT_OPEN if avctls is not open,
/// #AVCTLS_ERR_UNKNOWN_FORMAT if format can't be read else #AVCTLS_ERR_NO
// =============================================================================
PUBLIC AVCTLS_ERR_T avctls_OpenBuffer(AVCTLS_HANDLE_T*  phandle,
                                      UINT8*            buffer,
                                      UINT32            size);

// =============================================================================
// avctls_CloseFile
// -----------------------------------------------------------------------------
/// Close file and free handle
/// @param handle Handle use for this action
/// @return Return #AVCTLS_ERR_SERVICE_NOT_OPEN if avctls is not open else #AVCTLS_ERR_NO
// =============================================================================
PUBLIC AVCTLS_ERR_T avctls_Close(AVCTLS_HANDLE_T handle);

// =============================================================================
// avctls_Stop
// -----------------------------------------------------------------------------
/// Stop file
/// @param handle Handle use for this action
/// @return Return #AVCTLS_ERR_SERVICE_NOT_OPEN if avctls is not open else #AVCTLS_ERR_NO
// =============================================================================
PUBLIC AVCTLS_ERR_T avctls_Stop(AVCTLS_HANDLE_T handle);

// =============================================================================
// avctls_GetInfo
// -----------------------------------------------------------------------------
/// This function is use to return the information on file
/// @param handle Handle use for this action
/// @return information on file (total length, bit rate, video or audio file,
// ...). See #AVCTLS_FILE_INFO_T
// =============================================================================
PUBLIC AVCTLS_FILE_INFO_T* avctls_GetInfo(AVCTLS_HANDLE_T handle);

// =============================================================================
// avctls_Play
// -----------------------------------------------------------------------------
/// Play the file or resume file
/// @param handle Handle use for this action
/// @param nbPlayed It's use to define number loop of media. If it's 0, the
/// media is played one time
/// @return Return #AVCTLS_ERR_SERVICE_NOT_OPEN if avctls is not open,
/// #AVCTLS_ERR_INVALID if AVCTLS is already used also #AVCTLS_ERR_NO
// =============================================================================
PUBLIC AVCTLS_ERR_T avctls_Play(AVCTLS_HANDLE_T handle, UINT32 nbPlayed);

// =============================================================================
// avctls_Pause
// -----------------------------------------------------------------------------
/// Pause the file
/// @param handle Handle use for this action
/// @return Return #AVCTLS_ERR_SERVICE_NOT_OPEN if avctls is not open,
/// #AVCTLS_ERR_INVALID if AVCTLS don't play with this handle also #AVCTLS_ERR_NO
// =============================================================================
PUBLIC AVCTLS_ERR_T avctls_Pause(AVCTLS_HANDLE_T handle);

// =============================================================================
// avctls_SetConfig
// -----------------------------------------------------------------------------
/// Set audio volume
/// @param handle Handle use for this action
/// @param config New config must be apply
/// @return Return #AVCTLS_ERR_SERVICE_NOT_OPEN if avctls is not open,
/// #AVCTLS_ERR_INVALID if config is incorrect also #AVCTLS_ERR_NO
// =============================================================================
PUBLIC AVCTLS_ERR_T avctls_SetConfig(AVCTLS_HANDLE_T handle, AVCTLS_CONFIG_T* config);

// =============================================================================
// avctls_GetConfig
// -----------------------------------------------------------------------------
/// Get the current config
/// @param handle Handle use for this action
/// @param config Pointer where the config is save
/// @return Return #AVCTLS_ERR_SERVICE_NOT_OPEN if avctls is not open also #AVCTLS_ERR_NO
// =============================================================================
PUBLIC AVCTLS_ERR_T avctls_GetConfig(AVCTLS_HANDLE_T handle, AVCTLS_CONFIG_T* config);

// =============================================================================
// avctls_Seek
// -----------------------------------------------------------------------------
/// Change the current position
/// @param handle Handle use for this action
/// @param time The new position in stream (second unit)
/// @return Return #AVCTLS_ERR_SERVICE_NOT_OPEN if avctls is not open also #AVCTLS_ERR_NO
// =============================================================================
PUBLIC AVCTLS_ERR_T avctls_Seek(AVCTLS_HANDLE_T handle, UINT32 time);

// =============================================================================
// avctls_Tell
// -----------------------------------------------------------------------------
/// Get the current position
/// @param handle Handle use for this action
/// @param pos Pointeur use to store the current position in stream
/// @return Return #AVCTLS_ERR_SERVICE_NOT_OPEN if avctls is not open also #AVCTLS_ERR_NO
// =============================================================================
PUBLIC AVCTLS_ERR_T avctls_Tell(AVCTLS_HANDLE_T handle, UINT32* pos);

// =============================================================================
// avctls_GetFrequencyBars
// -----------------------------------------------------------------------------
/// Get frequency bars
/// @return Return pointer on frequency bars
// =============================================================================
PUBLIC AVCTLS_FREQUENCY_BARS_T* avctls_GetFrequencyBars(VOID);

PUBLIC AVCTLS_ERR_T avctls_SetVolume(AVCTLS_HANDLE_T handle, UINT8 volume);

PUBLIC AVCTLS_ERR_T avctls_GetVolume(AVCTLS_HANDLE_T handle, UINT8* volume);

PUBLIC AVCTLS_ERR_T avctls_SetWindow(AVCTLS_HANDLE_T handle,
                                     UINT16          x,
                                     UINT16          y,
                                     UINT16          height,
                                     UINT16          width);

PUBLIC AVCTLS_ERR_T avctls_SetRotation(AVCTLS_HANDLE_T         handle,
                                       AVCTLS_VIDEO_ROTATION_T rotationMode);

PUBLIC AVCTLS_ERR_T avctls_SetOutputPath(AVCTLS_HANDLE_T         handle,
        AVCTLS_AUDIO_PATH_T     audioPath);

PUBLIC AVCTLS_ERR_T avctls_SetEqualizer(AVCTLS_HANDLE_T         handle,
                                        AVCTLS_EQUALIZER_MODE_T equalizerMode);

#endif // _AVCTLS_M_H_
