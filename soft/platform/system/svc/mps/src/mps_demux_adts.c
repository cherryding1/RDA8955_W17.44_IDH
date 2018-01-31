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

PRIVATE VOID* mps_openAdts(MPS_HANDLE_T handle);

PRIVATE VOID mps_closeAdts(MPS_HANDLE_T handle);

PRIVATE VOID mps_pauseAdts(MPS_HANDLE_T handle);

PRIVATE UINT32 mps_getAudioDataAdts(MPS_HANDLE_T handle, UINT8* data,
                                    UINT32 size);

PRIVATE BOOL mps_playAdts(MPS_HANDLE_T handle, AVPS_CFG_T* avpsCfg);

PRIVATE UINT32 mps_tellAdts(MPS_HANDLE_T handle);

PRIVATE BOOL mps_seekAdts(MPS_HANDLE_T handle, UINT32 position);

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// MPS_DEMUX_ADTS_DATA_T
// -----------------------------------------------------------------------------
/// Internal data for adts demux
// =============================================================================
typedef struct
{
    UINT32                       headerSize;
    UINT32                       bitRate;
} MPS_DEMUX_ADTS_DATA_T ;

// =============================================================================
// PRIVATE VARIABLES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PROTECTED MPS_DEMUX_T g_adtsDemux =
{
    .open            = mps_openAdts,
    .close           = mps_closeAdts,
    .play            = mps_playAdts,
    .pause           = mps_pauseAdts,
    .getAudioData    = mps_getAudioDataAdts,
    .getVideoData    = 0,
    .skipVideo       = 0,
    .seek            = mps_seekAdts,
    .tell            = mps_tellAdts
};

// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

PRIVATE VOID* mps_openAdts(MPS_HANDLE_T handle)
{
    UINT8                      buffer[64];
    UINT32                     offset = 0;
    MPS_DEMUX_ADTS_DATA_T*      data;

    mps_MediumRead(&handle->mediumAudio, buffer, sizeof(buffer));

    // ID3
    if(buffer[0] == 'I' &&
            buffer[1] == 'D' &&
            buffer[2] == '3')
    {
        offset  = (buffer[9]&0x7F) | (((UINT32)buffer[8]&0x7F)<<7) |
                  (((UINT32)buffer[7]&0x7F)<<14) | (((UINT32)buffer[6]&0x7F)<<21);
        offset += 10;
        mps_MediumSeek(&handle->mediumAudio, offset, FS_SEEK_SET);
        MPS_TRACE("mps_OpenFile: ID3 size %x", offset);

        mps_MediumRead(&handle->mediumAudio, buffer, 64);
    }

    // aac
    if(buffer[0]        == 0xFF  &&
            (buffer[1]&0xF6) == 0xF0)
    {
        handle->stream.audioSampleRate = 0;

        if((buffer[2] & 0xC0) != 0x40)
        {
            return 0;
        }

        // Sample rate detection
        switch(buffer[2]&0x3C)
        {
            case 0x00:
            case 0x04: return 0;
            case 0x08: handle->stream.audioSampleRate = 64000; break;
            case 0x0C: handle->stream.audioSampleRate = 48000; break;
            case 0x10: handle->stream.audioSampleRate = 44100; break;
            case 0x14: handle->stream.audioSampleRate = 32000; break;
            case 0x18: handle->stream.audioSampleRate = 24000; break;
            case 0x1C: handle->stream.audioSampleRate = 22050; break;
            case 0x20: handle->stream.audioSampleRate = 16000; break;
            case 0x24: handle->stream.audioSampleRate = 12000; break;
            case 0x28: handle->stream.audioSampleRate = 11025; break;
            case 0x2C: handle->stream.audioSampleRate =  8000; break;
        }

        switch((((UINT16)buffer[2]<<8) | buffer[3]) & 0x01C0)
        {
            case 0x0040: handle->stream.channelNb = 1; break;
            case 0x0080: handle->stream.channelNb = 2; break;
            default: return 0;
        }
        data = (MPS_DEMUX_ADTS_DATA_T*) sxr_Malloc(sizeof(MPS_DEMUX_ADTS_DATA_T));


        handle->videoLength         = 0;
        handle->videoBuffer         = 0;
        handle->audioLength         = 1024*8;
        handle->audioBuffer         = (UINT8*) sxr_Malloc(handle->audioLength);

        handle->stream.audioMode    = AVPS_PLAY_AUDIO_MODE_AAC;

        data->headerSize            = offset;

        handle->bufferContext.audioRemainingSize      = 0;
        handle->bufferContext.bufferAudioReadPosition = handle->audioBuffer;

        mps_MediumSeek(&handle->mediumAudio, data->headerSize, FS_SEEK_SET);

        return (VOID*)data;
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
            MPS_TRACE("mps_OpenFile: MP3 sample rate unknown");
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

        MPS_TRACE("mps_OpenFile: MP3 offset %#x, [%02x %02x %02x]", offset,
                  buffer[0], buffer[1], buffer[2]);

        MPS_TRACE("mps_OpenFile: MP3 sample rate %iHz Channel %i",
                  handle->stream.audioSampleRate, handle->stream.channelNb);

        data = (MPS_DEMUX_ADTS_DATA_T*) sxr_Malloc(sizeof(MPS_DEMUX_ADTS_DATA_T));

        data->headerSize            = offset;

        if((buffer[1]&0xFE) == 0xF2 ||
                (buffer[1]&0xFE) == 0xE2)
        {
            // mpeg v2, v2.5
            switch(buffer[2]>>4)
            {
                case 0x1: data->bitRate =   8; break;
                case 0x2: data->bitRate =  16; break;
                case 0x3: data->bitRate =  24; break;
                case 0x4: data->bitRate =  32; break;
                case 0x5: data->bitRate =  40; break;
                case 0x6: data->bitRate =  48; break;
                case 0x7: data->bitRate =  56; break;
                case 0x8: data->bitRate =  64; break;
                case 0x9: data->bitRate =  80; break;
                case 0xA: data->bitRate =  96; break;
                case 0xB: data->bitRate = 112; break;
                case 0xC: data->bitRate = 128; break;
                case 0xD: data->bitRate = 144; break;
                case 0xE: data->bitRate = 160; break;
                default:  data->bitRate =   0; break;
            }
        }
        else
        {
            switch(buffer[2]>>4)
            {
                case 0x1: data->bitRate =  32; break;
                case 0x2: data->bitRate =  40; break;
                case 0x3: data->bitRate =  48; break;
                case 0x4: data->bitRate =  56; break;
                case 0x5: data->bitRate =  64; break;
                case 0x6: data->bitRate =  80; break;
                case 0x7: data->bitRate =  96; break;
                case 0x8: data->bitRate = 112; break;
                case 0x9: data->bitRate = 128; break;
                case 0xA: data->bitRate = 160; break;
                case 0xB: data->bitRate = 192; break;
                case 0xC: data->bitRate = 224; break;
                case 0xD: data->bitRate = 256; break;
                case 0xE: data->bitRate = 320; break;
                default:  data->bitRate =   0; break;
            }
        }

        handle->videoLength         = 0;
        handle->videoBuffer         = 0;
        handle->audioLength         = 1024*8;
        handle->audioBuffer         = (UINT8*) sxr_Malloc(handle->audioLength);

        handle->stream.audioMode    = AVPS_PLAY_AUDIO_MODE_MP3;

        mps_MediumSeek(&handle->mediumAudio, 128, FS_SEEK_END);
        mps_MediumRead(&handle->mediumAudio, buffer, 63);

        if(buffer[0] == 'T' &&
                buffer[1] == 'A' &&
                buffer[2] == 'G')
        {
            memcpy(handle->info.title, &buffer[3], 30);
            handle->info.title[30]  = 0;
            memcpy(handle->info.artist, &buffer[33], 30);
            handle->info.artist[30] = 0;
            mps_MediumRead(&handle->mediumAudio, handle->info.album, 63);
            handle->info.album[30]  = 0;
        }

        handle->bufferContext.audioRemainingSize      = 0;
        handle->bufferContext.bufferAudioReadPosition = handle->audioBuffer;

        mps_MediumSeek(&handle->mediumAudio, data->headerSize, FS_SEEK_SET);

        return (VOID*)data;
    }

    return 0;
}

PRIVATE VOID mps_closeAdts(MPS_HANDLE_T handle)
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

PRIVATE UINT32 mps_getAudioDataAdts(MPS_HANDLE_T handle, UINT8* data, UINT32 size)
{
    INT32 i;

    i = mps_MediumRead(&handle->mediumAudio, data, size);

    if(i < 0)
    {
        i = 0;
    }

    return i;
}

PRIVATE BOOL mps_playAdts(MPS_HANDLE_T handle, AVPS_CFG_T* avpsCfg)
{
    MPS_DEMUX_ADTS_DATA_T*     data;
    UINT32                     audioWriteBuffer;
    UINT32                     size;

    data = (MPS_DEMUX_ADTS_DATA_T*) handle->demuxData;
    if(mps_MediumIsEof(&handle->mediumAudio))
    {
        mps_MediumSeek(&handle->mediumAudio, data->headerSize, FS_SEEK_SET);
        handle->bufferContext.audioRemainingSize      = 0;
        handle->bufferContext.bufferAudioReadPosition = handle->audioBuffer;
    }

    if(((UINT32)handle->bufferContext.bufferAudioReadPosition|0x80000000) != (UINT32)handle->audioBuffer)
    {
        audioWriteBuffer = ((UINT32)handle->bufferContext.bufferAudioReadPosition|0x80000000)
                           + handle->bufferContext.audioRemainingSize;
        if(audioWriteBuffer >= (UINT32)handle->audioBuffer+handle->audioLength)
        {
            audioWriteBuffer -= handle->audioLength;
        }

        size = (UINT32)handle->audioBuffer+handle->audioLength-audioWriteBuffer;
        if(size > handle->audioLength-handle->bufferContext.audioRemainingSize)
        {
            size = handle->audioLength-handle->bufferContext.audioRemainingSize;
        }

        if(size != 0)
        {
            size = mps_getAudioDataAdts(handle, (UINT8*)audioWriteBuffer, size);
            handle->bufferContext.audioRemainingSize += size;
        }
    }
    if(handle->bufferContext.audioRemainingSize != handle->audioLength)
    {
        size = mps_getAudioDataAdts(handle, handle->audioBuffer,
                                    handle->audioLength-handle->bufferContext.audioRemainingSize);
        handle->bufferContext.audioRemainingSize += size;
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

PRIVATE VOID mps_pauseAdts(MPS_HANDLE_T handle)
{

}

PRIVATE UINT32 mps_tellAdts(MPS_HANDLE_T handle)
{
    MPS_DEMUX_ADTS_DATA_T*     data;

    data = (MPS_DEMUX_ADTS_DATA_T*) handle->demuxData;

    if(data->bitRate)
    {
        return mps_MediumTell(&handle->mediumAudio)*8/(data->bitRate*1000);
    }
    return 0;
}

PRIVATE BOOL mps_seekAdts(MPS_HANDLE_T handle, UINT32 position)
{
    MPS_DEMUX_ADTS_DATA_T*     data;

    data = (MPS_DEMUX_ADTS_DATA_T*) handle->demuxData;

    if(data->bitRate)
    {
        mps_MediumSeek(&handle->mediumAudio, position*data->bitRate*1000/8, FS_SEEK_SET);

        handle->bufferContext.audioRemainingSize      = 0;
        handle->bufferContext.bufferAudioReadPosition = (UINT8*)((UINT32)handle->audioBuffer & ~0x80000000);

        return TRUE;
    }

    handle->bufferContext.audioRemainingSize      = 0;
    handle->bufferContext.bufferAudioReadPosition = (UINT8*)((UINT32)handle->audioBuffer & ~0x80000000);

    return FALSE;
}

// =============================================================================
// FUNCTIONS
// =============================================================================

