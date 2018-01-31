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




#include "cs_types.h"
#include "fs.h"

#include "mpsp_demux.h"
#include "mpsp.h"
#include "mpsp_debug.h"

#include "sxr_ops.h"
#include <string.h>

// =============================================================================
// PRIVATE PROTOTYPE FUNCTIONS
// =============================================================================

PRIVATE UINT32 mps_getAudioDataAmr(MPS_HANDLE_T handle, UINT8* data,
                                   UINT32 size);

PRIVATE VOID* mps_openAmr(MPS_HANDLE_T handle);

PRIVATE VOID mps_closeAmr(MPS_HANDLE_T handle);

PRIVATE VOID mps_pauseAmr(MPS_HANDLE_T handle);

PRIVATE BOOL mps_playAmr(MPS_HANDLE_T handle, AVPS_CFG_T* avpsCfg);

PRIVATE UINT32 mps_tellAmr(MPS_HANDLE_T handle);

PRIVATE BOOL mps_seekAmr(MPS_HANDLE_T handle, UINT32 position);

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// MPS_DEMUX_AMR_DATA_T
// -----------------------------------------------------------------------------
/// Internal data for amr demux
// =============================================================================
typedef struct
{
    UINT8 frameSize;
} MPS_DEMUX_AMR_DATA_T ;

// =============================================================================
// PRIVATE VARIABLES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PROTECTED MPS_DEMUX_T g_amrDemux =
{
    .open                = mps_openAmr,
    .close               = mps_closeAmr,
    .play                = mps_playAmr,
    .pause               = mps_pauseAmr,
    .getAudioData        = mps_getAudioDataAmr,
    .getVideoData        = 0,
    .skipVideo           = 0,
    .seek                = mps_seekAmr,
    .tell                = mps_tellAmr
};

// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

PRIVATE VOID* mps_openAmr(MPS_HANDLE_T handle)
{
    MPS_DEMUX_AMR_DATA_T*      data;
    UINT8                      buffer[7];

    mps_MediumRead(&handle->mediumAudio, buffer, sizeof(buffer));

    // AMR
    if(buffer[0] == '#' &&
            buffer[1] == '!' &&
            buffer[2] == 'A' &&
            buffer[3] == 'M' &&
            buffer[4] == 'R' &&
            buffer[5] == '\n')
    {
        MPS_TRACE("mps_OpenFile: AMR");

        handle->videoLength              = 0;
        handle->videoBuffer              = 0;
        handle->audioLength              = 1024*8;
        handle->audioBuffer              = (UINT8*) sxr_Malloc(handle->audioLength);
        handle->stream.audioMode         = AVPS_PLAY_AUDIO_MODE_AMR_RING;
        handle->stream.audioSampleRate   = 8000;
        handle->stream.channelNb         = 1;

        data = (MPS_DEMUX_AMR_DATA_T*) sxr_Malloc(sizeof(MPS_DEMUX_AMR_DATA_T));

        switch((buffer[6]>>3) & 0xF)
        {
            case 0:  data->frameSize      = 13; break;
            case 1:  data->frameSize      = 14; break;
            case 2:  data->frameSize      = 16; break;
            case 3:  data->frameSize      = 18; break;
            case 4:  data->frameSize      = 20; break;
            case 5:  data->frameSize      = 21; break;
            case 6:  data->frameSize      = 27; break;
            case 7:  data->frameSize      = 32; break;
            default: data->frameSize      =  0; break;
        }

        mps_MediumSeek(&handle->mediumAudio, 6, FS_SEEK_SET);

        return (VOID*)data;
    }
    return 0;
}

PRIVATE VOID mps_closeAmr(MPS_HANDLE_T handle)
{
    avps_Stop(&handle->bufferContext);
    if (handle->audioBuffer != NULL)
    {
        sxr_Free(handle->audioBuffer);
        handle->audioBuffer = NULL;
    }
    if (handle->demuxData != NULL)
    {
        sxr_Free(handle->demuxData);
        handle->demuxData = NULL;
    }
}

PRIVATE UINT32 mps_getAudioDataAmr(MPS_HANDLE_T handle, UINT8* data, UINT32 size)
{
    INT32 i;

    i = mps_MediumRead(&handle->mediumAudio, data, size);

    if(i < 0)
    {
        i = 0;
    }

    return i;
}

PRIVATE BOOL mps_playAmr(MPS_HANDLE_T handle, AVPS_CFG_T* avpsCfg)
{
    if(mps_MediumIsEof(&handle->mediumAudio))
    {
        mps_MediumSeek(&handle->mediumAudio, 6, FS_SEEK_SET);
    }

    g_mpsBufferWritePos =
        (UINT8*)((UINT32)handle->bufferContext.bufferAudioReadPosition + handle->bufferContext.audioRemainingSize);
    g_mpsBufferWritePos = (UINT8*)((UINT32)g_mpsBufferWritePos | 0x80000000);
    if((UINT32)g_mpsBufferWritePos >= (UINT32)handle->audioBuffer + handle->audioLength)
    {
        g_mpsBufferWritePos = (UINT8*)((UINT32)g_mpsBufferWritePos - handle->audioLength);
    }

    if(avps_Play(handle->config.audioPath, &handle->stream, avpsCfg, 0, &handle->bufferContext) == AVPS_ERR_NO)
    {
        return TRUE;
    }
    return FALSE;
}

PRIVATE VOID mps_pauseAmr(MPS_HANDLE_T handle)
{

}

PRIVATE UINT32 mps_tellAmr(MPS_HANDLE_T handle)
{
    MPS_DEMUX_AMR_DATA_T*     data;

    data = (MPS_DEMUX_AMR_DATA_T*) handle->demuxData;

    if(data->frameSize)
    {
        return (mps_MediumTell(&handle->mediumAudio)-6)/(data->frameSize*50);
    }
    return 0;
}

PRIVATE BOOL mps_seekAmr(MPS_HANDLE_T handle, UINT32 position)
{
    MPS_DEMUX_AMR_DATA_T*     data;

    data = (MPS_DEMUX_AMR_DATA_T*) handle->demuxData;

    handle->bufferContext.audioRemainingSize      = 0;
    handle->bufferContext.bufferAudioReadPosition = handle->audioBuffer;

    if(data->frameSize)
    {
        mps_MediumSeek(&handle->mediumAudio, position*data->frameSize*50+6, FS_SEEK_SET);
        return TRUE;
    }

    return FALSE;
}

// =============================================================================
// FUNCTIONS
// =============================================================================

