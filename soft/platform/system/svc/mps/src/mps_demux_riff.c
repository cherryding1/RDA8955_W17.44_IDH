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

#include "hal_sys.h"

#include "sxr_ops.h"
#include <string.h>

// =============================================================================
// PRIVATE PROTOTYPE FUNCTIONS
// =============================================================================

PRIVATE VOID* mps_openRiff(MPS_HANDLE_T handle);

PRIVATE VOID mps_closeRiff(MPS_HANDLE_T handle);

PRIVATE VOID mps_pauseRiff(MPS_HANDLE_T handle);

PRIVATE UINT32 mps_getAudioDataRiff(MPS_HANDLE_T handle, UINT8* data,
                                    UINT32 size);

PRIVATE UINT32 mps_getVideoDataRiff(MPS_HANDLE_T handle, UINT8* data,
                                    UINT32 size);

PRIVATE BOOL mps_playRiff(MPS_HANDLE_T handle, AVPS_CFG_T* avpsCfg);

PRIVATE VOID mps_skipVideoRiff(MPS_HANDLE_T handle);

PRIVATE UINT32 mps_tellRiff(MPS_HANDLE_T handle);

PRIVATE BOOL mps_seekRiff(MPS_HANDLE_T handle, UINT32 position);

// =============================================================================
// MACROS
// =============================================================================

#define RIFF_ID(A, B, C, D) \
    (((A)<<0)|((B)<<8)|((C)<<16)|((D)<<24))

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// MPS_RIFF_HEADER_T
// -----------------------------------------------------------------------------
/// RIFF header file
// =============================================================================
typedef struct
{
    UINT32 riff;
    UINT32 size;
    UINT32 id;
}  __attribute__((packed)) MPS_RIFF_HEADER_T;

// =============================================================================
// MPS_RIFF_CHUNK_T
// -----------------------------------------------------------------------------
/// Chunk header
// =============================================================================
typedef struct
{
    UINT32 id;
    UINT32 size;
}  __attribute__((packed)) MPS_RIFF_CHUNK_T;

// =============================================================================
// MPS_RIFF_FMT_CHUNK_T
// -----------------------------------------------------------------------------
/// Fmt chunk
// =============================================================================
typedef struct
{
    UINT16 audioFormat;
    UINT16 numChannels;
    UINT32 sampleRate ;
    UINT32 byteRate ;
    UINT16 blockAlign;
    UINT16 bitsPerSample;
}  __attribute__((packed)) MPS_RIFF_FMT_CHUNK_T;

// =============================================================================
// MPS_RIFF_AVIH_CHUNK_T
// -----------------------------------------------------------------------------
/// Avi header Chunk
// =============================================================================
typedef struct
{
    UINT32 msPerFrame;
    UINT32 paddingGranularity;
    UINT32 reserved;
    UINT32 flag;
    UINT32 nbFrames;
    UINT32 initFrames;
    UINT32 nbStreams;
    UINT32 suggestedBufferSize;
    UINT32 width;
    UINT32 height;
    // MS reserved it. I don't understand this sentence (set 0)
    UINT32 timeScale;
    UINT32 dataRate;
    UINT32 startTime;
    UINT32 dataLength;
}  __attribute__((packed)) MPS_RIFF_AVIH_CHUNK_T;

// =============================================================================
// MPS_RIFF_STRH_VIDEO_CHUNK_T
// -----------------------------------------------------------------------------
/// Video stream header Chunk
// =============================================================================
typedef struct
{
    UINT32 codec;
    UINT32 flag;
    UINT32 reserved;
    UINT32 initFrames;
    UINT32 scale;
    UINT32 rate; // rate/scale == samples/second
}  __attribute__((packed)) MPS_RIFF_STRH_VIDEO_CHUNK_T;

// =============================================================================
// MPS_RIFF_STRF_VIDEO_CHUNK_T
// -----------------------------------------------------------------------------
/// Video stream format Chunk
// =============================================================================
typedef struct
{
    UINT32 size;
    UINT32 width;
    UINT32 height;
    UINT16 planes;
    UINT16 depth;
    UINT32 codec;
    UINT32 rate; // rate/scale == samples/second
}  __attribute__((packed)) MPS_RIFF_STRF_VIDEO_CHUNK_T;

// =============================================================================
// MPS_RIFF_STRF_AUDIO_CHUNK_T
// -----------------------------------------------------------------------------
/// Audio stream format Chunk
// =============================================================================
typedef struct
{
    UINT16 format;
    UINT16 nbChannels;
    UINT32 sampleRate;
    UINT32 bitRate;
    UINT16 blockAlign;
    UINT16 bitsPerSample;
} __attribute__((packed)) MPS_RIFF_STRF_AUDIO_CHUNK_T;

// =============================================================================
// MPS_RIFF_IDX1_ENTRY_T
// -----------------------------------------------------------------------------
/// Video stream format Chunk
// =============================================================================
typedef struct
{
    UINT32 id;
    UINT32 flag;
    UINT32 offset;
    UINT32 size;
}  __attribute__((packed)) MPS_RIFF_IDX1_ENTRY_T;

// =============================================================================
// MPS_RIFF_FORMAT_T
// -----------------------------------------------------------------------------
/// List of riff type support
// =============================================================================
typedef enum
{
    MPS_RIFF_FORMAT_WAVE,
    MPS_RIFF_FORMAT_AVI
} MPS_RIFF_FORMAT_T ;


// =============================================================================
// MPS_DEMUX_RIFF_DATA_T
// -----------------------------------------------------------------------------
/// Internal data for riff demux
// =============================================================================
typedef struct
{
    MPS_RIFF_FORMAT_T            format;
    UINT32                       dataChunk;
    UINT32                       idx1Chunk;
    INT32                        audioChunkRemaining;
    INT32                        videoChunkRemaining;
    UINT8                        streamAudio;
    UINT8                        streamVideo;
    UINT32                       videoFrame;
#ifdef MPS_AMR_MJPEG_MODE
    UINT8                        isAmr;
#endif /* MPS_AMR_MJPEG_MODE */
} MPS_DEMUX_RIFF_DATA_T ;

// =============================================================================
// PRIVATE VARIABLES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PROTECTED MPS_DEMUX_T g_riffDemux =
{
    .open            = mps_openRiff,
    .close           = mps_closeRiff,
    .play            = mps_playRiff,
    .pause           = mps_pauseRiff,
    .getAudioData    = mps_getAudioDataRiff,
    .getVideoData    = mps_getVideoDataRiff,
    .skipVideo       = mps_skipVideoRiff,
    .seek            = mps_seekRiff,
    .tell            = mps_tellRiff
};

// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

PRIVATE BOOL mps_foundRiffChunk(MPS_MEDIUM_T* medium, UINT32 id,
                                MPS_RIFF_CHUNK_T* chunk, UINT32 offsetMax)
{
    while(1)
    {
        if(mps_MediumRead(medium, (VOID*)chunk, sizeof(MPS_RIFF_CHUNK_T)) <= 0)
        {
            return FALSE;
        }
        // Test if the first byte is padding byte
        if(*((UINT8*)chunk) == 0)
        {
            mps_MediumSeek(medium, 1-sizeof(MPS_RIFF_CHUNK_T), FS_SEEK_CUR);
            if(mps_MediumRead(medium, (VOID*)chunk, sizeof(MPS_RIFF_CHUNK_T)) <= 0)
            {
                return FALSE;
            }
        }

        if(HAL_ENDIAN_LITTLE_32(chunk->id) == id)
        {
            // found
            break;
        }

        // next chunk
        if(mps_MediumSeek(medium, HAL_ENDIAN_LITTLE_32(chunk->size), FS_SEEK_CUR) == -1)
        {
            return FALSE;
        }

        if(mps_MediumTell(medium) >= offsetMax)
        {
            return FALSE;
        }
    }
    return TRUE;
}

PRIVATE BOOL mps_foundRiffListChunk(MPS_MEDIUM_T* medium, UINT32 id, MPS_RIFF_CHUNK_T* chunk, UINT32 offsetMax)
{
    UINT32 listId;

    while(mps_foundRiffChunk(medium, RIFF_ID('L', 'I', 'S', 'T'), chunk, offsetMax) == TRUE)
    {
        mps_MediumRead(medium, (VOID*)&listId, sizeof(UINT32));

        if(id == HAL_ENDIAN_LITTLE_32(listId))
        {
            return TRUE;
        }
        mps_MediumSeek(medium, HAL_ENDIAN_LITTLE_32(chunk->size)-sizeof(UINT32), FS_SEEK_CUR);

        if(mps_MediumTell(medium) >= offsetMax)
        {
            return FALSE;
        }
    }
    return FALSE;
}

PRIVATE VOID* mps_openRiff(MPS_HANDLE_T handle)
{
    MPS_RIFF_HEADER_T               header;
    MPS_RIFF_CHUNK_T                chunk;
    MPS_RIFF_FMT_CHUNK_T            fmt;
    MPS_RIFF_AVIH_CHUNK_T           avih;
    MPS_RIFF_STRH_VIDEO_CHUNK_T     strhVideo;
    MPS_RIFF_STRF_VIDEO_CHUNK_T     strfVideo;
    MPS_RIFF_STRF_AUDIO_CHUNK_T     strfAudio;
    MPS_DEMUX_RIFF_DATA_T*          data;
    UINT32                          chunkLimit;
    UINT32                          subChunkLimit;
    UINT32                          subSubChunkLimit;
    UINT32                          id;
    UINT32                          i;

    mps_MediumRead(&handle->mediumAudio, (VOID*)&header, sizeof(header));

    chunkLimit = HAL_ENDIAN_LITTLE_32(header.size);

    // WAV
    if(header.riff == RIFF_ID('R', 'I', 'F', 'F'))
    {
        switch(header.id)
        {
            case RIFF_ID('W', 'A', 'V', 'E'):
                MPS_TRACE("mps_OpenFile: WAV");
                if(mps_foundRiffChunk(&handle->mediumAudio, RIFF_ID('f', 'm', 't', ' '),
                                      &chunk, chunkLimit) == FALSE)
                {
                    return 0;
                }

                if(mps_MediumRead(&handle->mediumAudio, (VOID*)&fmt, sizeof(fmt)) <= 0)
                {
                    return 0;
                }

                handle->stream.audioSampleRate   = fmt.sampleRate;
                handle->stream.channelNb         = fmt.numChannels;
                if(fmt.numChannels > 2 || fmt.numChannels == 0)
                {
                    // Nb channels invalid
                    return 0;
                }
                if(fmt.bitsPerSample != 16)
                {
                    return 0;
                }

                if(fmt.audioFormat != 1) // PCM = 1
                {
                    return 0;
                }

                MPS_TRACE("mps_OpenFile: WAV PCM numChannel %i", fmt.numChannels);

                if(mps_foundRiffChunk(&handle->mediumAudio, RIFF_ID('d', 'a', 't', 'a'),
                                      &chunk, chunkLimit) == FALSE)
                {
                    return FALSE;
                }

                data = (MPS_DEMUX_RIFF_DATA_T*) sxr_Malloc(sizeof(MPS_DEMUX_RIFF_DATA_T));

                data->format                = MPS_RIFF_FORMAT_WAVE;
                data->dataChunk             = mps_MediumTell(&handle->mediumAudio)-sizeof(MPS_RIFF_CHUNK_T);
                data->audioChunkRemaining   = HAL_ENDIAN_LITTLE_32(chunk.size);
                data->videoChunkRemaining   = 0;

                handle->videoLength         = 0;
                handle->videoBuffer         = 0;
                handle->audioLength         = 1024*32;
                handle->audioBuffer         = (UINT8*) sxr_Malloc(handle->audioLength);
                handle->videoBuffer         = NULL;
                handle->stream.audioMode    = AVPS_PLAY_AUDIO_MODE_PCM;

                return (VOID*)data;

            case RIFF_ID('A', 'V', 'I', ' '):
                MPS_TRACE("mps_OpenFile: AVI");

                if(mps_foundRiffListChunk(&handle->mediumAudio, RIFF_ID('h', 'd', 'r', 'l'),
                                          &chunk, chunkLimit) == FALSE)
                {
                    MPS_TRACE("mps_OpenFile: AVI hdrl chunk not found");
                    return 0;
                }

                subChunkLimit = mps_MediumTell(&handle->mediumAudio) + HAL_ENDIAN_LITTLE_32(chunk.size) - sizeof(UINT32);

                if(mps_foundRiffChunk(&handle->mediumAudio, RIFF_ID('a', 'v', 'i', 'h'),
                                      &chunk, subChunkLimit) == FALSE)
                {
                    MPS_TRACE("mps_OpenFile: AVI avih chunk not found");
                    return 0;
                }

                mps_MediumRead(&handle->mediumAudio, (VOID*)&avih, sizeof(MPS_RIFF_AVIH_CHUNK_T));

                data = (MPS_DEMUX_RIFF_DATA_T*) sxr_Malloc(sizeof(MPS_DEMUX_RIFF_DATA_T));

                data->format                = MPS_RIFF_FORMAT_AVI;
                data->dataChunk             =  0;
                data->idx1Chunk             =  0;
                data->audioChunkRemaining   =  0;
                data->videoChunkRemaining   =  0;
                data->streamAudio           = -1;
                data->streamVideo           = -1;
                data->videoFrame            =  0;

                for(i = 0; i < avih.nbStreams; ++i)
                {
                    if(mps_foundRiffListChunk(&handle->mediumAudio, RIFF_ID('s', 't', 'r', 'l'),
                                              &chunk, subChunkLimit) == FALSE)
                    {
                        MPS_TRACE("mps_OpenFile: AVI strl[%i] chunk not found", i);
                        sxr_Free(data);
                        return 0;
                    }

                    subSubChunkLimit = mps_MediumTell(&handle->mediumAudio) + HAL_ENDIAN_LITTLE_32(chunk.size)
                                       - sizeof(UINT32);
                    if(mps_foundRiffChunk(&handle->mediumAudio, RIFF_ID('s', 't', 'r', 'h'),
                                          &chunk, subSubChunkLimit) == FALSE)
                    {
                        MPS_TRACE("mps_OpenFile: AVI strh[%i] chunk not found", i);
                        sxr_Free(data);
                        return 0;
                    }
                    mps_MediumRead(&handle->mediumAudio, (VOID*)&id, sizeof(UINT32));
                    switch(HAL_ENDIAN_LITTLE_32(id))
                    {
                        case RIFF_ID('a', 'u', 'd', 's'):
                            MPS_TRACE("mps_OpenFile: stream AUD", i);
                            data->streamAudio = i;
                            mps_MediumSeek(&handle->mediumAudio, HAL_ENDIAN_LITTLE_32(chunk.size)-sizeof(UINT32), FS_SEEK_CUR);
                            if(mps_foundRiffChunk(&handle->mediumAudio, RIFF_ID('s', 't', 'r', 'f'),
                                                  &chunk, subSubChunkLimit) == FALSE)
                            {
                                MPS_TRACE("mps_OpenFile: AVI strf[%i] chunk not found", i);
                                sxr_Free(data);
                                return 0;
                            }
                            mps_MediumRead(&handle->mediumAudio, (VOID*)&strfAudio, sizeof(MPS_RIFF_STRF_AUDIO_CHUNK_T));
                            MPS_TRACE("mps_OpenFile: Audio rate %i channels %i depth %i demux %04x",
                                      strfAudio.sampleRate, strfAudio.nbChannels, strfAudio.bitsPerSample,
                                      strfAudio.format);

                            if(strfAudio.format != 0x0055
#ifdef MPS_AMR_MJPEG_MODE
                                    && strfAudio.format != 0x00FE
#endif /* MPS_AMR_MJPEG_MODE */
                              )
                            {
                                // Format not support 0x0055=MP3 0x00FE=AMR RING
                                data->streamAudio = -1;
                            }
                            break;
                        case RIFF_ID('v', 'i', 'd', 's'):
                            MPS_TRACE("mps_OpenFile: stream VID", i);
                            mps_MediumRead(&handle->mediumAudio, (VOID*)&strhVideo, sizeof(MPS_RIFF_STRH_VIDEO_CHUNK_T));
                            if(HAL_ENDIAN_LITTLE_32(strhVideo.codec) != RIFF_ID('M', 'J', 'P', 'G'))
                            {
                                MPS_TRACE("mps_OpenFile: VID not supported codec", i);
                                // Format not support
                                break;
                            }
                            data->streamVideo  = i;
                            mps_MediumSeek(&handle->mediumAudio,
                                           HAL_ENDIAN_LITTLE_32(chunk.size)-sizeof(MPS_RIFF_STRH_VIDEO_CHUNK_T)-sizeof(UINT32),
                                           FS_SEEK_CUR);
                            if(mps_foundRiffChunk(&handle->mediumAudio, RIFF_ID('s', 't', 'r', 'f'),
                                                  &chunk, subSubChunkLimit) == FALSE)
                            {
                                MPS_TRACE("mps_OpenFile: AVI strf[%i] chunk not found", i);
                                sxr_Free(data);
                                return 0;
                            }
                            mps_MediumRead(&handle->mediumAudio, (VOID*)&strfVideo, sizeof(MPS_RIFF_STRF_VIDEO_CHUNK_T));
                            MPS_TRACE("mps_OpenFile: Video rate %i scale %i %ix%i depth %i codec %#x",
                                      strhVideo.rate, strhVideo.scale, strfVideo.width, strfVideo.height,
                                      strfVideo.depth, strfVideo.codec);
                            handle->info.height    = strfVideo.height;
                            handle->info.width     = strfVideo.width;
                            handle->info.nbFrames  = avih.nbFrames;
                            handle->info.videoRate = ((UINT32)strhVideo.rate<<8) / strhVideo.scale;
                            break;
                        default:
                            break;
                    }
                    mps_MediumSeek(&handle->mediumAudio, subSubChunkLimit, FS_SEEK_SET);
                }

                mps_MediumSeek(&handle->mediumAudio, subChunkLimit, FS_SEEK_SET);

                handle->stream.audioSampleRate   = strfAudio.sampleRate;
                handle->stream.channelNb         = strfAudio.nbChannels;
                MPS_TRACE("mps_OpenFile: Channel Number = %d ratio %i!",
                          handle->stream.channelNb, handle->stream.audioVideoRatio);

                if(data->streamVideo == (UINT8)-1 && data->streamAudio == (UINT8)-1)
                {
                    sxr_Free(data);
                    return 0;
                }
                if(data->streamVideo != (UINT8)-1)
                {
                    handle->stream.videoMode = AVPS_PLAY_VIDEO_MODE_MJPEG;
                }
                switch(strfAudio.format)
                {
                    case 0x0055: // MP3
                        handle->stream.audioMode = AVPS_PLAY_AUDIO_MODE_MP3;
                        if(handle->stream.audioSampleRate > 16000)
                        {
                            handle->stream.audioVideoRatio   = (((UINT32)handle->stream.audioSampleRate<<8) * strhVideo.scale) /
                                                               (strhVideo.rate * 1152);
                        }
                        else
                        {
                            handle->stream.audioVideoRatio   = (((UINT32)handle->stream.audioSampleRate<<8) * strhVideo.scale) /
                                                               (strhVideo.rate * 576);
                        }
#ifdef MPS_AMR_MJPEG_MODE
                        data->isAmr = 0;
#endif /* MPS_AMR_MJPEG_MODE */
                        break;
#ifdef MPS_AMR_MJPEG_MODE
                    case 0x00FE: // AMR Ring
                        handle->stream.audioMode = AVPS_PLAY_AUDIO_MODE_AMR_RING;
                        handle->stream.audioVideoRatio   = (((UINT32)handle->stream.audioSampleRate<<8) * strhVideo.scale) /
                                                           (strhVideo.rate * 160);
                        data->isAmr = 1;
                        break;
#endif /* MPS_AMR_MJPEG_MODE */
                }

                if(data->streamAudio == (UINT8)-1)
                {
                    /// @todo must implemented video only on avps
                    /*                 handle->stream.mode              = AVPS_PLAY_MODE_MJPEG; */
                    sxr_Free(data);
                    return 0;
                }
                handle->audioLength         = 1024*8;
                handle->audioBuffer         = (UINT8*) sxr_Malloc(handle->audioLength);
                handle->videoLength         = 1024*16*2;
                handle->videoBuffer         = (UINT8*) sxr_Malloc(handle->videoLength);
                MPS_TRACE("mps_OpenFile: RIFF VIDEO BUFFER ALLOCATED !");

                data->audioChunkRemaining   = 0;
                data->videoChunkRemaining   = 0;

                if(mps_foundRiffListChunk(&handle->mediumAudio, RIFF_ID('m', 'o', 'v', 'i'),
                                          &chunk, chunkLimit) == TRUE)
                {
                    data->dataChunk = mps_MediumTell(&handle->mediumAudio) - sizeof(MPS_RIFF_CHUNK_T);

                    MPS_TRACE("Format movi found offset=%#x", data->dataChunk);
                    subChunkLimit = mps_MediumTell(&handle->mediumAudio) +
                                    (HAL_ENDIAN_LITTLE_32(chunk.size)-sizeof(UINT32));
                    data->audioChunkRemaining = 0;

                    if(mps_MediumSeek(&handle->mediumAudio, subChunkLimit, FS_SEEK_SET))
                    {
                        if(mps_foundRiffChunk(&handle->mediumAudio, RIFF_ID('i', 'd', 'x', '1'),
                                              &chunk, chunkLimit) == TRUE)
                        {
                            MPS_TRACE("mps_OpenFile: Index V1 found");
                            data->idx1Chunk = mps_MediumTell(&handle->mediumAudio) - sizeof(MPS_RIFF_CHUNK_T);
                        }
                    }

                    return (VOID*)data;
                }
                sxr_Free(data);
                return 0;
            default:
                MPS_TRACE("mps_OpenFile: unknown RIFF format");
        }
    }

    return 0;
}

PRIVATE VOID mps_closeRiff(MPS_HANDLE_T handle)
{
    avps_Stop(&handle->bufferContext);
    if (handle->audioBuffer != NULL)
    {
        sxr_Free(handle->audioBuffer);
        handle->audioBuffer = NULL;
    }

    if (handle->videoBuffer != NULL)
    {
        sxr_Free(handle->videoBuffer);
        handle->videoBuffer = NULL;
    }

    if (handle->demuxData != NULL)
    {
        sxr_Free(handle->demuxData);
        handle->demuxData = NULL;
    }
}

PRIVATE UINT32 mps_getVideoDataRiff(MPS_HANDLE_T handle, UINT8* data, UINT32 size)
{
    MPS_DEMUX_RIFF_DATA_T*  demuxData;
    MPS_RIFF_CHUNK_T        chunk;
    UINT32                  readSize;

    MPS_PROFILE_FUNCTION_ENTER(mps_demuxGetVideoData);

    MPS_TRACE("RIFF Fetch video data start for:%#x", (UINT32)data);
    MPS_TRACE(__func__);

    demuxData = (MPS_DEMUX_RIFF_DATA_T*) handle->demuxData;

    if(demuxData->videoChunkRemaining == 0)
    {
        MPS_TRACE("RIFF Fetch video: no more chunk ??");
        MPS_PROFILE_FUNCTION_EXIT(mps_demuxGetVideoData);
        return 0;
    }

    MPS_ASSERT(demuxData->videoChunkRemaining < size, "mps_getVideoDataRiffInfo: overflow");
    readSize = mps_MediumRead(&handle->mediumVideo, (VOID*)data, demuxData->videoChunkRemaining);
    if(readSize <= 0)
    {
        readSize                       = 0;
        demuxData->videoChunkRemaining = 0;
    }

    demuxData->videoChunkRemaining  -= readSize;

    if(mps_foundRiffChunk(&handle->mediumVideo, RIFF_ID('0', '0' + demuxData->streamVideo, 'd', 'b'),
                          &chunk, -1) == FALSE)
    {
        demuxData->videoChunkRemaining = 0;
    }
    else
    {
        demuxData->videoChunkRemaining = HAL_ENDIAN_LITTLE_32(chunk.size);
        demuxData->videoFrame++;
    }

    MPS_TRACE("RIFF Fetch video: Chunk Fetched !");
    MPS_PROFILE_FUNCTION_EXIT(mps_demuxGetVideoData);

    return readSize;
}

PRIVATE UINT32 mps_getAudioDataRiff(MPS_HANDLE_T handle, UINT8* data, UINT32 size)
{
    MPS_DEMUX_RIFF_DATA_T*  demuxData;
    INT32                   i;
    MPS_RIFF_CHUNK_T        chunk;
    UINT32                  readSize;
    UINT32                  dataRead = 0;
    UINT8*                  writePtr = data;

    MPS_PROFILE_FUNCTION_ENTER(mps_demuxGetAudioData);

    MPS_TRACE(__func__);

    demuxData = (MPS_DEMUX_RIFF_DATA_T*) handle->demuxData;

    if(demuxData->audioChunkRemaining == 0)
    {
        MPS_PROFILE_FUNCTION_EXIT(mps_demuxGetAudioData);
        return 0;
    }

    do
    {
        readSize = size;
        if(size > demuxData->audioChunkRemaining)
        {
            readSize = demuxData->audioChunkRemaining;
        }

        i = mps_MediumRead(&handle->mediumAudio, writePtr, readSize);
        if(i <= 0)
        {
            i                              = 0;
            demuxData->audioChunkRemaining = 0;
        }

        demuxData->audioChunkRemaining  -= i;
        size                            -= i;
        dataRead                        += i;
        writePtr                        += i;

        if(demuxData->format == MPS_RIFF_FORMAT_AVI)
        {
            if(demuxData->audioChunkRemaining == 0)
            {
#ifdef MPS_RIFF_DEBUG
                MPS_TRACE("RIFF audio get: Next chunk");
#endif
                // Found next chunk
                if(mps_foundRiffChunk(&handle->mediumAudio,
                                      RIFF_ID('0', '0' + demuxData->streamAudio, 'w', 'b'),
                                      &chunk, -1) == TRUE)
                {
#ifdef MPS_RIFF_DEBUG
                    MPS_TRACE("RIFF audio get: Next chunk found size=%i pos=%i", chunk.size,
                              demuxData->audioPosition);
#endif
                    demuxData->audioChunkRemaining = HAL_ENDIAN_LITTLE_32(chunk.size);
                }
            }
        }
    }
    while(demuxData->audioChunkRemaining && size);

    MPS_PROFILE_FUNCTION_EXIT(mps_demuxGetAudioData);
    return dataRead;
}

PRIVATE BOOL mps_playRiff(MPS_HANDLE_T handle, AVPS_CFG_T* avpsCfg)
{
    MPS_DEMUX_RIFF_DATA_T*  data;
    MPS_RIFF_CHUNK_T        chunk;
    UINT32                  size;
    UINT32                  chunkLimit;
    LCDD_FBW_T              frameWindows;
    UINT32                  audioWriteBuffer;

    MPS_PROFILE_FUNCTION_ENTER(mps_demuxPlay);

    MPS_TRACE(__func__);

    data = (MPS_DEMUX_RIFF_DATA_T*) handle->demuxData;
    if(data->audioChunkRemaining == 0)
    {
        mps_MediumSeek(&handle->mediumAudio, data->dataChunk, FS_SEEK_SET);
        if(mps_MediumRead(&handle->mediumAudio, (VOID*)&chunk, sizeof(chunk)) <= 0)
        {
            MPS_PROFILE_FUNCTION_EXIT(mps_demuxPlay);
            return FALSE;
        }
        if(data->format == MPS_RIFF_FORMAT_AVI)
        {
            data->videoFrame            =  0;
            chunkLimit = data->dataChunk + sizeof(chunk) + HAL_ENDIAN_LITTLE_32(chunk.size);
            mps_MediumSeek(&handle->mediumAudio, data->dataChunk + sizeof(chunk), FS_SEEK_SET);
            mps_MediumSeek(&handle->mediumVideo, data->dataChunk + sizeof(chunk), FS_SEEK_SET);
            if(mps_foundRiffChunk(&handle->mediumAudio, RIFF_ID('0', '0' + data->streamAudio, 'w', 'b'),
                                  &chunk, chunkLimit) == FALSE)
            {
                MPS_TRACE("RIFF audio track not found");
                MPS_PROFILE_FUNCTION_EXIT(mps_demuxPlay);
                return FALSE;
            }
            MPS_TRACE("RIFF audio play: pos %i @@@@", mps_MediumTell(&handle->mediumAudio));
            data->audioChunkRemaining = HAL_ENDIAN_LITTLE_32(chunk.size);
#ifdef MPS_AMR_MJPEG_MODE
            if(data->isAmr)
            {
                mps_MediumSeek(&handle->mediumAudio, 6, FS_SEEK_CUR);
                data->audioChunkRemaining -= 6;
            }
#endif /* MPS_AMR_MJPEG_MODE */
            if(data->streamVideo != (UINT8)-1)
            {
                if(mps_foundRiffChunk(&handle->mediumVideo, RIFF_ID('0', '0' + data->streamVideo, 'd', 'b'),
                                      &chunk, chunkLimit) == FALSE)
                {
                    MPS_TRACE("RIFF video track not found");
                    handle->stream.videoMode  = AVPS_PLAY_VIDEO_MODE_NO;
                    data->streamVideo         = -1;
                    data->videoChunkRemaining = 0;
                }
                else
                {
                    data->videoChunkRemaining = HAL_ENDIAN_LITTLE_32(chunk.size);
                }
            }
        }
        else
        {
            data->audioChunkRemaining = HAL_ENDIAN_LITTLE_32(chunk.size);
        }
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
            size = mps_getAudioDataRiff(handle, (UINT8*)audioWriteBuffer, size);
            handle->bufferContext.audioRemainingSize += size;
        }
    }
    if(handle->bufferContext.audioRemainingSize != handle->audioLength)
    {
        size = mps_getAudioDataRiff(handle, handle->audioBuffer,
                                    handle->audioLength-handle->bufferContext.audioRemainingSize);
        handle->bufferContext.audioRemainingSize += size;
    }

    if(data->streamVideo != (UINT8)-1)
    {
        mps_getVideoDataRiff(handle, handle->videoBuffer, handle->videoLength/2);
    }

    // Config frame windows
    if(handle->config.windows)
    {
        memcpy(&frameWindows, handle->config.windows, sizeof(LCDD_FBW_T));
        if(handle->info.height > frameWindows.fb.height)
        {
            frameWindows.roi.y     = (handle->info.height-frameWindows.fb.height) / 2;
            frameWindows.fb.height = handle->info.height;
        }
    }


    g_mpsBufferWritePos =
        (UINT8*)((UINT32)handle->bufferContext.bufferAudioReadPosition + handle->bufferContext.audioRemainingSize);
    g_mpsBufferWritePos = (UINT8*)((UINT32)g_mpsBufferWritePos | 0x80000000);
    if((UINT32)g_mpsBufferWritePos >= (UINT32)handle->audioBuffer + handle->audioLength)
    {
        g_mpsBufferWritePos = (UINT8*)((UINT32)g_mpsBufferWritePos - handle->audioLength);
    }

    if(avps_Play(handle->config.audioPath, &handle->stream, avpsCfg,
                 &frameWindows, &handle->bufferContext) == AVPS_ERR_NO)
    {
        MPS_PROFILE_FUNCTION_EXIT(mps_demuxPlay);
        return TRUE;
    }
    MPS_PROFILE_FUNCTION_EXIT(mps_demuxPlay);
    return FALSE;
}

PRIVATE VOID mps_syncAudioVideoRiff(MPS_HANDLE_T handle)
{
    MPS_DEMUX_RIFF_DATA_T*  data;
    MPS_RIFF_CHUNK_T        chunk;

    MPS_TRACE(__func__);

    data = (MPS_DEMUX_RIFF_DATA_T*) handle->demuxData;

    mps_MediumSeek(&handle->mediumAudio,
                   mps_MediumTell(&handle->mediumVideo)+data->videoChunkRemaining, FS_SEEK_SET);

    if(mps_foundRiffChunk(&handle->mediumAudio, RIFF_ID('0', '0' + data->streamAudio, 'w', 'b'), &chunk, -1)
            == FALSE)
    {
        data->audioChunkRemaining = 0;
    }
    else
    {
        data->audioChunkRemaining = HAL_ENDIAN_LITTLE_32(chunk.size);
    }

    handle->bufferContext.audioRemainingSize      = 0;
    handle->bufferContext.bufferAudioReadPosition = (UINT8*)((UINT32)handle->audioBuffer & ~0x80000000);

    if(data->streamAudio < data->streamVideo)
    {
        mps_skipVideoRiff(handle);
    }
}

PRIVATE VOID mps_pauseRiff(MPS_HANDLE_T handle)
{
    MPS_DEMUX_RIFF_DATA_T*  data;

    MPS_TRACE(__func__);

    data = (MPS_DEMUX_RIFF_DATA_T*) handle->demuxData;

    if(data->format == MPS_RIFF_FORMAT_AVI)
    {
        MPS_TRACE("RIFF pause resync Audio/video");

        // Resync audio/video
        mps_syncAudioVideoRiff(handle);
    }
}

PRIVATE VOID mps_skipVideoRiff(MPS_HANDLE_T handle)
{
    MPS_DEMUX_RIFF_DATA_T*  demuxData;
    MPS_RIFF_CHUNK_T        chunk;

    MPS_TRACE(__func__);

    demuxData = (MPS_DEMUX_RIFF_DATA_T*) handle->demuxData;

    if(demuxData->videoChunkRemaining != 0)
    {
        mps_MediumSeek(&handle->mediumVideo, demuxData->videoChunkRemaining, FS_SEEK_CUR);

        if(mps_foundRiffChunk(&handle->mediumVideo, RIFF_ID('0', '0' + demuxData->streamVideo, 'd', 'b'),
                              &chunk, -1) == FALSE)
        {
            demuxData->videoChunkRemaining = 0;
        }
        else
        {
            demuxData->videoChunkRemaining = HAL_ENDIAN_LITTLE_32(chunk.size);
            demuxData->videoFrame++;
        }
    }
}

PRIVATE UINT32 mps_tellRiff(MPS_HANDLE_T handle)
{
    MPS_DEMUX_RIFF_DATA_T*  demuxData;

    MPS_TRACE(__func__);

    MPS_PROFILE_FUNCTION_ENTER(mps_demuxTell);

    demuxData = (MPS_DEMUX_RIFF_DATA_T*) handle->demuxData;

    switch(demuxData->format)
    {
        case MPS_RIFF_FORMAT_AVI:
            MPS_PROFILE_FUNCTION_EXIT(mps_demuxTell);
            return (demuxData->videoFrame<<8)/handle->info.videoRate;

        case MPS_RIFF_FORMAT_WAVE:
            MPS_PROFILE_FUNCTION_EXIT(mps_demuxTell);
            return (mps_MediumTell(&handle->mediumAudio)-(demuxData->dataChunk+sizeof(MPS_RIFF_CHUNK_T))) /
                   (handle->stream.channelNb*sizeof(UINT16)*handle->stream.audioSampleRate);
    }
    MPS_PROFILE_FUNCTION_EXIT(mps_demuxTell);
    return 0;
}

PRIVATE UINT32 mps_foundVideoFrameRiff(MPS_HANDLE_T handle, UINT32 frame)
{
    MPS_DEMUX_RIFF_DATA_T*  demuxData;
    MPS_RIFF_CHUNK_T        chunk;
    MPS_RIFF_IDX1_ENTRY_T   indexEntry[64];
    UINT32                  dataRead;
    UINT32                  currentFrame;
    UINT32                  i;

    MPS_TRACE("mps_foundVideoFrameRiff: frame=%i", frame);

    demuxData    = (MPS_DEMUX_RIFF_DATA_T*) handle->demuxData;
    currentFrame = 0;

    mps_MediumSeek(&handle->mediumVideo, demuxData->idx1Chunk, FS_SEEK_SET);
    if(mps_MediumRead(&handle->mediumVideo, (VOID*)&chunk,
                      sizeof(MPS_RIFF_CHUNK_T)) <= 0)
    {
        return 0;
    }

    chunk.size = HAL_ENDIAN_LITTLE_32(chunk.size);

    while(chunk.size)
    {
        dataRead = chunk.size;
        if(dataRead > sizeof(indexEntry))
        {
            dataRead = sizeof(indexEntry);
        }

        if(mps_MediumRead(&handle->mediumVideo, (VOID*)indexEntry, dataRead) <= 0)
        {
            return 0;
        }

        for(i = 0; i < dataRead/sizeof(MPS_RIFF_IDX1_ENTRY_T); ++i)
        {
            if(HAL_ENDIAN_LITTLE_32(indexEntry[i].id) == RIFF_ID('0', '0' + demuxData->streamVideo, 'd', 'b'))
            {
                // is video frame
                if(currentFrame == frame)
                {
                    // Frame found
                    MPS_TRACE("mps_foundVideoFrameRiff: frame found", frame);
                    return HAL_ENDIAN_LITTLE_32(indexEntry[i].offset);
                }
                currentFrame++;
            }
        }

        chunk.size -= dataRead;
    }

    return 0;
}

PRIVATE BOOL mps_seekRiff(MPS_HANDLE_T handle, UINT32 position)
{
    MPS_DEMUX_RIFF_DATA_T*  demuxData;
    MPS_RIFF_CHUNK_T        chunk;
    UINT32                  offset;

    MPS_PROFILE_FUNCTION_ENTER(mps_demuxSeek);

    demuxData = (MPS_DEMUX_RIFF_DATA_T*) handle->demuxData;

    avps_Stop(0);

    switch(demuxData->format)
    {
        case MPS_RIFF_FORMAT_AVI:
            if(demuxData->idx1Chunk == 0)
            {
                MPS_PROFILE_FUNCTION_EXIT(mps_demuxSeek);
                return FALSE;
            }
            offset = mps_foundVideoFrameRiff(handle, (position*handle->info.videoRate)>>8);
            if(offset == 0)
            {
                // Not found
                demuxData->audioChunkRemaining = 0;
                demuxData->videoChunkRemaining = 0;
                MPS_PROFILE_FUNCTION_EXIT(mps_demuxSeek);
                return FALSE;
            }
            demuxData->videoFrame = (position*handle->info.videoRate)>>8;

            mps_MediumSeek(&handle->mediumVideo, demuxData->dataChunk+offset+sizeof(UINT32), FS_SEEK_SET);
            if(mps_MediumRead(&handle->mediumVideo, (VOID*)&chunk, sizeof(chunk)) <= 0)
            {
                demuxData->audioChunkRemaining = 0;
                demuxData->videoChunkRemaining = 0;
                MPS_PROFILE_FUNCTION_EXIT(mps_demuxSeek);
                return FALSE;
            }
            demuxData->videoChunkRemaining = HAL_ENDIAN_LITTLE_32(chunk.size);

            mps_syncAudioVideoRiff(handle);

            MPS_PROFILE_FUNCTION_EXIT(mps_demuxSeek);
            return TRUE;

        case MPS_RIFF_FORMAT_WAVE:
            mps_MediumSeek(&handle->mediumAudio, demuxData->dataChunk, FS_SEEK_SET);
            if(mps_MediumRead(&handle->mediumAudio, (VOID*)&chunk, sizeof(chunk)) <= 0)
            {
                demuxData->audioChunkRemaining = 0;
                MPS_PROFILE_FUNCTION_EXIT(mps_demuxSeek);
                return FALSE;
            }
            offset = position*handle->stream.channelNb*sizeof(UINT16)*handle->stream.audioSampleRate;
            if(offset > HAL_ENDIAN_LITTLE_32(chunk.size))
            {
                demuxData->audioChunkRemaining = 0;
                MPS_PROFILE_FUNCTION_EXIT(mps_demuxSeek);
                return FALSE;
            }
            mps_MediumSeek(&handle->mediumAudio, offset, FS_SEEK_CUR);
            demuxData->audioChunkRemaining = HAL_ENDIAN_LITTLE_32(chunk.size)-offset;

            handle->bufferContext.audioRemainingSize      = 0;
            handle->bufferContext.bufferAudioReadPosition = handle->audioBuffer;

            MPS_PROFILE_FUNCTION_EXIT(mps_demuxSeek);
            return TRUE;
    }
    MPS_PROFILE_FUNCTION_EXIT(mps_demuxSeek);
    return FALSE;
}

// =============================================================================
// FUNCTIONS
// =============================================================================

