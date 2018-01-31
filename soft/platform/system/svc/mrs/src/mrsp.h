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


#ifndef _MRSP_H_
#define _MRSP_H_

#include "cs_types.h"

#include "mrs_m.h"

#include "aud_m.h"
#include "avrs_m.h"

#include "mrsp_medium.h"
#include "mrsp_codec.h"


// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

typedef struct MRS_CODEC_STRUCT_T MRS_CODEC_T;

// =============================================================================
// MRS_HANDLE_STRUCT_T
// -----------------------------------------------------------------------------
/// MRS handle description
// =============================================================================
struct MRS_HANDLE_STRUCT_T
{
    MRS_MEDIUM_T                mediumAudio;
    MRS_MEDIUM_T                mediumVideo;
    UINT32                      nbPlayed;

    // Audio buffer
    UINT8*                      audioBuffer;
    UINT32                      audioLength;
    // Video buffer
    UINT8*                      videoBuffer;
    UINT32                      videoLength;

    AVRS_ENC_AUDIO_STREAM_T     audioStream;
    AVRS_ENC_VIDEO_STREAM_T     videoStream;
    AVRS_BUFFER_CONTEXT_T       bufferContext;

    MRS_FILE_INFO_T             info;

    MRS_CFG_T                   config;
    MRS_CALLBACK_T              callback;

    MRS_CODEC_T*                codec;
    VOID*                       codecData;
};

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================


#endif // _MRSP_H_
