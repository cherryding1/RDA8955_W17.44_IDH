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

#include "mrsp_codec.h"
#include "mrsp.h"
#include "mrsp_debug.h"

#include "sxr_ops.h"
#include <string.h>

// =============================================================================
// PRIVATE PROTOTYPE FUNCTIONS
// =============================================================================

PRIVATE VOID* mrs_openMp3(MRS_HANDLE_T handle);

PRIVATE VOID mrs_closeMp3(MRS_HANDLE_T handle);

PRIVATE VOID mrs_pauseMp3(MRS_HANDLE_T handle);

PRIVATE UINT32 mrs_getAudioDataMp3(MRS_HANDLE_T handle, UINT8* data,
                                   UINT32 size);

PRIVATE BOOL mrs_playMp3(MRS_HANDLE_T handle, AVPS_CFG_T* avpsCfg);

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// MRS_CODEC_MP3_DATA_T
// -----------------------------------------------------------------------------
/// Internal data for mp3 codec
// =============================================================================
typedef struct
{
    UINT32                       headerSize;
} MRS_CODEC_MP3_DATA_T ;

// =============================================================================
// PRIVATE VARIABLES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PROTECTED MRS_CODEC_T g_mp3Codec =
{
    .open            = mrs_openMp3,
    .close           = mrs_closeMp3,
    .play            = mrs_playMp3,
    .pause           = mrs_pauseMp3,
    .getAudioData    = mrs_getAudioDataMp3,
    .getVideoData    = 0
};

// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

PRIVATE VOID* mrs_openMp3(MRS_HANDLE_T handle)
{
    UINT8                      buffer[64];
    UINT32                     offset = 0;
    MRS_CODEC_MP3_DATA_T*      data;

    mrs_MediumRead(&handle->medium, buffer, sizeof(buffer));

    // ID3
    if(buffer[0] == 'I' &&
            buffer[1] == 'D' &&
            buffer[2] == '3')
    {
        offset  = (buffer[9]&0x7F) | (((UINT32)buffer[8]&0x7F)<<7) |
                  (((UINT32)buffer[7]&0x7F)<<14) | (((UINT32)buffer[6]&0x7F)<<21);
        offset += 10;
        mrs_MediumSeek(&handle->medium, offset, FS_SEEK_SET);
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: ID3 size %x", offset);

        mrs_MediumRead(&handle->medium, buffer, 64);
    }

    // mp3
    if(buffer[0]        == 0xFF  &&
            ((buffer[1]&0xFE) == 0xFA ||
             (buffer[1]&0xFE) == 0xF2 ||
             (buffer[1]&0xFE) == 0xE2))
    {
        handle->stream.audioSampleRate = 0;

        // Sample rate detection
        switch(buffer[2]&0x0C)
        {
            case 0x00:
                handle->stream.audioSampleRate = 44100;
                break;
            case 0x04:
                handle->stream.audioSampleRate = 48000;
                break;
            case 0x08:
                handle->stream.audioSampleRate = 32000;
                break;
        }

        if(handle->stream.audioSampleRate == 0)
        {
            MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: MP3 sample rate unknown", &handle->medium);
            return FALSE;
        }

        switch(buffer[1]&0xFE)
        {
            case 0xFA:
                break;
            case 0xF2:
                handle->stream.audioSampleRate /= 2;
                break;
            case 0xE2:
                handle->stream.audioSampleRate /= 4;
                break;
        }

        switch(buffer[3]&0xC0)
        {
            case 0x00: // Stereo
            case 0x40: // Joint Stereo
            case 0x80: // 2xmono
                handle->stream.channelNb = 2;
                break;
            case 0xC0: // Mono
                handle->stream.channelNb = 1;
                break;
        }

        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: MP3 offset %#x, [%02x %02x %02x]", offset,
                  buffer[0], buffer[1], buffer[2]);

        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: MP3 sample rate %iHz Channel %i",
                  handle->stream.audioSampleRate, handle->stream.channelNb);

        data = (MRS_CODEC_MP3_DATA_T*) sxr_Malloc(sizeof(MRS_CODEC_MP3_DATA_T));

        data->headerSize            = offset;

        handle->videoLength         = 0;
        handle->videoBuffer         = 0;
        handle->audioLength         = 1024*8;
        handle->audioBuffer         = (UINT8*) sxr_Malloc(handle->audioLength);

        handle->stream.mode         = AVPS_PLAY_MODE_MP3;

        mrs_MediumSeek(&handle->medium, 128, FS_SEEK_END);
        mrs_MediumRead(&handle->medium, buffer, 63);

        if(buffer[0] == 'T' &&
                buffer[1] == 'A' &&
                buffer[2] == 'G')
        {
            memcpy(handle->info.title, &buffer[3], 30);
            handle->info.title[30]  = 0;
            memcpy(handle->info.artist, &buffer[33], 30);
            handle->info.artist[30] = 0;
            mrs_MediumRead(&handle->medium, handle->info.album, 63);
            handle->info.album[30]  = 0;
        }

        return (VOID*)data;
    }

    return 0;
}

PRIVATE VOID mrs_closeMp3(MRS_HANDLE_T handle)
{
    avps_Stop(&handle->bufferContext);
    if (handle->audioBuffer != NULL)
    {
        sxr_Free(handle->audioBuffer);
        handle->audioBuffer = NULL;
    }

    if (handle->codecData != NULL)
    {
        sxr_Free(handle->codecData);
        handle->codecData = NULL;
    }
}

PRIVATE UINT32 mrs_getAudioDataMp3(MRS_HANDLE_T handle, UINT8* data, UINT32 size)
{
    INT32 i;

    i = mrs_MediumRead(&handle->medium, data, size);

    if(i < 0)
    {
        i = 0;
    }

    return i;
}

PRIVATE BOOL mrs_playMp3(MRS_HANDLE_T handle, AVPS_CFG_T* avpsCfg)
{
    MRS_CODEC_MP3_DATA_T*     data;

    data = (MRS_CODEC_MP3_DATA_T*) handle->codecData;
    if(mrs_MediumIsEof(&handle->medium))
    {
        mrs_MediumSeek(&handle->medium, data->headerSize, FS_SEEK_SET);
    }
    if(avps_Play(handle->config.audioPath, &handle->stream, avpsCfg, 0, &handle->bufferContext, TRUE) == AVPS_ERR_NO)
    {
        return TRUE;
    }
    return FALSE;
}

PRIVATE VOID mrs_pauseMp3(MRS_HANDLE_T handle)
{

}

// =============================================================================
// FUNCTIONS
// =============================================================================

