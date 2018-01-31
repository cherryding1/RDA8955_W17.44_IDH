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


#ifndef _MPSP_H_
#define _MPSP_H_

#include "cs_types.h"

#include "mps_m.h"

#include "aud_m.h"
#include "avps_m.h"

#include "mpsp_medium.h"


// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

typedef struct MPS_DEMUX_STRUCT_T MPS_DEMUX_T;

// =============================================================================
// MPS_HANDLE_STRUCT_T
// -----------------------------------------------------------------------------
/// MPS handle description
// =============================================================================
struct MPS_HANDLE_STRUCT_T
{
    MPS_MEDIUM_T                 mediumAudio;
    MPS_MEDIUM_T                 mediumVideo;
    UINT32                       nbPlayed;

    // Audio buffer
    UINT8*                       audioBuffer;
    UINT32                       audioLength;
    // Video buffer
    UINT8*                       videoBuffer;
    UINT32                       videoLength;

    AVPS_ENC_STREAM_T            stream;
    AVPS_BUFFER_CONTEXT_T        bufferContext;

    MPS_FILE_INFO_T              info;

    MPS_CONFIG_T                 config;
    MPS_CALLBACK_T               callback;

    MPS_DEMUX_T*                 demux;
    VOID*                        demuxData;
};

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================


#endif // _MPSP_H_
