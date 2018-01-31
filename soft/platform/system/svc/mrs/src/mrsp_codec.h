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


#ifndef _CODECP_H_
#define _CODECP_H_

#include "cs_types.h"
#include "mrsp.h"

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// MRS_CODEC_OPEN_T
// -----------------------------------------------------------------------------
/// Callback to detect if the file use this codec
/// @param handle Current handle use
/// @param mode File opening mode: creation, appending, etc ...
/// @param data Pointer to a pointer to a private codec structure, set by the
/// call to the mrs_XxxcodecOpen function.
/// @return #MRS_ERR_NO (OK), #MRS_ERR_FILE_ERROR when it failed
/// to create or access the file, #MRS_ERR_INCOMPATIBLE_AUDIO_CODEC (Improper
/// file format...).
// =============================================================================
typedef MRS_ERR_T(*MRS_CODEC_OPEN_T)(MRS_HANDLE_T handle, MRS_FILE_MODE_T mode, VOID** data);

// =============================================================================
// MRS_CODEC_CLOSE_T
// -----------------------------------------------------------------------------
/// Callback use to close codec and free codec structure
/// @param handle Current handle use
/// @return Private codec structure
// =============================================================================
typedef VOID (*MRS_CODEC_CLOSE_T)(MRS_HANDLE_T handle);

// =============================================================================
// MRS_CODEC_RECORD_T
// -----------------------------------------------------------------------------
/// Callback use to start file recording
/// @param handle Current handle use
/// @return TRUE if the successful else FALSE
// =============================================================================
typedef BOOL (*MRS_CODEC_RECORD_T)( MRS_HANDLE_T handle,
                                    AVRS_AUDIO_CFG_T* avrsAudioCfg,
                                    AVRS_VIDEO_CFG_T* avrsVideoCfg);

// =============================================================================
// MRS_CODEC_STOP_T
// -----------------------------------------------------------------------------
/// Callback use to stop the stream recording.
/// @param handle Current handle use
// =============================================================================
typedef VOID (*MRS_CODEC_STOP_T)(MRS_HANDLE_T handle);

// =============================================================================
// MRS_CODEC_WRITE_AV_DATA_T
// -----------------------------------------------------------------------------
/// Callback use to add data in audio or video buffer
/// @param handle Current handle use
/// @param data Memory chunk of data to add.
/// @param size Size of memory chunk
/// @return TRUE if the successful else FALSE
// =============================================================================
typedef UINT32 (*MRS_CODEC_WRITE_AV_DATA_T)(MRS_HANDLE_T handle, UINT8* data,
        UINT32 size);

// =============================================================================
// MRS_CODEC_SKIP_VIDEO_T
// -----------------------------------------------------------------------------
/// Callback use to skip one video frame
/// @param handle Current handle use
// =============================================================================
typedef VOID (*MRS_CODEC_SKIP_VIDEO_T)(MRS_HANDLE_T handle);


// =============================================================================
// MRS_CODEC_SEEK_T
// -----------------------------------------------------------------------------
/// Callback use to jump under the a specific position of stream
/// @param handle Current handle use
/// @param position The position when you want jump
/// @return TRUE if successful else FALSE
// =============================================================================
typedef BOOL (*MRS_CODEC_SEEK_T)(MRS_HANDLE_T handle, UINT32 position);


// =============================================================================
// MRS_CODEC_TELL_T
// -----------------------------------------------------------------------------
/// Callback use to keep the current position in file (unite second)
/// @param handle Current handle use
/// @return The current position
// =============================================================================
typedef UINT32 (*MRS_CODEC_TELL_T)(MRS_HANDLE_T handle);


// =============================================================================
// MRS_CODEC_T
// -----------------------------------------------------------------------------
/// Codec structure
// =============================================================================
struct MRS_CODEC_STRUCT_T
{
    MRS_CODEC_OPEN_T                        open;
    MRS_CODEC_CLOSE_T                       close;
    MRS_CODEC_RECORD_T                      record;
    MRS_CODEC_STOP_T                        stop;
    MRS_CODEC_WRITE_AV_DATA_T               writeAudioData;
    MRS_CODEC_WRITE_AV_DATA_T               writeVideoData;
    MRS_CODEC_SKIP_VIDEO_T                  skipVideo;
    MRS_CODEC_SEEK_T                        seek;
    MRS_CODEC_TELL_T                        tell;
    MRS_AUDIO_CODEC_T                       audioCodec;
};

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================


#endif // _CODECP_H_
