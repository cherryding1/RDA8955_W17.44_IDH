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

#include "hal_sys.h"

#include "sxr_ops.h"
#include <string.h>

// =============================================================================
// PRIVATE PROTOTYPE FUNCTIONS
// =============================================================================

PRIVATE VOID* mrs_openRiff(MRS_HANDLE_T handle);

PRIVATE VOID mrs_closeRiff(MRS_HANDLE_T handle);

PRIVATE VOID mrs_pauseRiff(MRS_HANDLE_T handle);

PRIVATE UINT32 mrs_getAudioDataRiff(MRS_HANDLE_T handle, UINT8* data,
                                    UINT32 size);

PRIVATE UINT32 mrs_getVideoDataRiff(MRS_HANDLE_T handle, UINT8* data,
                                    UINT32 size);

PRIVATE BOOL mrs_playRiff(MRS_HANDLE_T handle, AVPS_CFG_T* avpsCfg);

PRIVATE VOID mrs_skipVideoRiff(MRS_HANDLE_T handle);

PRIVATE UINT32 mrs_tellRiff(MRS_HANDLE_T handle);

PRIVATE BOOL mrs_seekRiff(MRS_HANDLE_T handle, UINT32 position);

// =============================================================================
// MACROS
// =============================================================================

#define RIFF_ID(A, B, C, D) \
    (((A)<<0)|((B)<<8)|((C)<<16)|((D)<<24))

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// MRS_RIFF_HEADER_T
// -----------------------------------------------------------------------------
/// RIFF header file
// =============================================================================
typedef struct
{
    UINT32 riff;
    UINT32 size;
    UINT32 id;
}  __attribute__((packed)) MRS_RIFF_HEADER_T;

// =============================================================================
// MRS_RIFF_CHUNK_T
// -----------------------------------------------------------------------------
/// Chunk header
// =============================================================================
typedef struct
{
    UINT32 id;
    UINT32 size;
}  __attribute__((packed)) MRS_RIFF_CHUNK_T;

// =============================================================================
// MRS_RIFF_FMT_CHUNK_T
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
}  __attribute__((packed)) MRS_RIFF_FMT_CHUNK_T;

// =============================================================================
// MRS_RIFF_AVIH_CHUNK_T
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
}  __attribute__((packed)) MRS_RIFF_AVIH_CHUNK_T;

// =============================================================================
// MRS_RIFF_STRH_VIDEO_CHUNK_T
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
}  __attribute__((packed)) MRS_RIFF_STRH_VIDEO_CHUNK_T;

// =============================================================================
// MRS_RIFF_STRF_VIDEO_CHUNK_T
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
}  __attribute__((packed)) MRS_RIFF_STRF_VIDEO_CHUNK_T;

// =============================================================================
// MRS_RIFF_STRF_AUDIO_CHUNK_T
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
} __attribute__((packed)) MRS_RIFF_STRF_AUDIO_CHUNK_T;

// =============================================================================
// MRS_RIFF_IDX1_ENTRY_T
// -----------------------------------------------------------------------------
/// Video stream format Chunk
// =============================================================================
typedef struct
{
    UINT32 id;
    UINT32 flag;
    UINT32 offset;
    UINT32 size;
}  __attribute__((packed)) MRS_RIFF_IDX1_ENTRY_T;

// =============================================================================
// MRS_RIFF_FORMAT_T
// -----------------------------------------------------------------------------
/// List of riff type support
// =============================================================================
typedef enum
{
    MRS_RIFF_FORMAT_WAVE,
    MRS_RIFF_FORMAT_AVI
} MRS_RIFF_FORMAT_T ;


// =============================================================================
// MRS_CODEC_RIFF_DATA_T
// -----------------------------------------------------------------------------
/// Internal data for riff codec
// =============================================================================
typedef struct
{
    MRS_RIFF_FORMAT_T            format;
    UINT32                       dataChunk;
    UINT32                       idx1Chunk;
    INT32                        audioChunkRemaining;
    INT32                        videoChunkRemaining;
    UINT8                        streamAudio;
    UINT8                        streamVideo;
    UINT32                       videoFrame;
#ifdef MRS_AMR_MJPEG_MODE
    UINT8                        isAmr;
#endif /* MRS_AMR_MJPEG_MODE */
} MRS_CODEC_RIFF_DATA_T ;

// =============================================================================
// PRIVATE VARIABLES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PROTECTED MRS_CODEC_T g_riffCodec =
{
    .open            = mrs_openRiff,
    .close           = mrs_closeRiff,
    .play            = mrs_playRiff,
    .pause           = mrs_pauseRiff,
    .getAudioData    = mrs_getAudioDataRiff,
    .getVideoData    = mrs_getVideoDataRiff,
    .skipVideo       = mrs_skipVideoRiff,
    .seek            = mrs_seekRiff,
    .tell            = mrs_tellRiff
};

// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

PRIVATE BOOL mrs_foundRiffChunk(MRS_MEDIUM_T* medium, UINT32 id,
                                MRS_RIFF_CHUNK_T* chunk, UINT32 offsetMax)
{
    while(1)
    {
        if(mrs_MediumRead(medium, (VOID*)chunk, sizeof(MRS_RIFF_CHUNK_T)) <= 0)
        {
            return FALSE;
        }
        // Test if the first byte is padding byte
        if(*((UINT8*)chunk) == 0)
        {
            mrs_MediumSeek(medium, 1-sizeof(MRS_RIFF_CHUNK_T), FS_SEEK_CUR);
            if(mrs_MediumRead(medium, (VOID*)chunk, sizeof(MRS_RIFF_CHUNK_T)) <= 0)
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
        if(mrs_MediumSeek(medium, HAL_ENDIAN_LITTLE_32(chunk->size), FS_SEEK_CUR) == -1)
        {
            return FALSE;
        }

        if(mrs_MediumTell(medium) >= offsetMax)
        {
            return FALSE;
        }
    }
    return TRUE;
}

PRIVATE BOOL mrs_foundRiffListChunk(MRS_MEDIUM_T* medium, UINT32 id, MRS_RIFF_CHUNK_T* chunk, UINT32 offsetMax)
{
    UINT32 listId;

    while(mrs_foundRiffChunk(medium, RIFF_ID('L', 'I', 'S', 'T'), chunk, offsetMax) == TRUE)
    {
        mrs_MediumRead(medium, (VOID*)&listId, sizeof(UINT32));

        if(id == HAL_ENDIAN_LITTLE_32(listId))
        {
            return TRUE;
        }
        mrs_MediumSeek(medium, HAL_ENDIAN_LITTLE_32(chunk->size)-sizeof(UINT32), FS_SEEK_CUR);

        if(mrs_MediumTell(medium) >= offsetMax)
        {
            return FALSE;
        }
    }
    return FALSE;
}

PRIVATE VOID* mrs_openRiff(MRS_HANDLE_T handle)
{
    MRS_RIFF_HEADER_T               header;
    MRS_RIFF_CHUNK_T                chunk;
    MRS_RIFF_FMT_CHUNK_T            fmt;
    MRS_RIFF_AVIH_CHUNK_T           avih;
    MRS_RIFF_STRH_VIDEO_CHUNK_T     strhVideo;
    MRS_RIFF_STRF_VIDEO_CHUNK_T     strfVideo;
    MRS_RIFF_STRF_AUDIO_CHUNK_T     strfAudio;
    MRS_CODEC_RIFF_DATA_T*          data;
    UINT32                          chunkLimit;
    UINT32                          subChunkLimit;
    UINT32                          subSubChunkLimit;
    UINT32                          id;
    UINT32                          i;

    mrs_MediumRead(&handle->mediumAudio, (VOID*)&header, sizeof(header));

    chunkLimit = HAL_ENDIAN_LITTLE_32(header.size);

    // WAV
    if(header.riff == RIFF_ID('R', 'I', 'F', 'F'))
    {
        switch(header.id)
        {
            case RIFF_ID('W', 'A', 'V', 'E'):
                MRS_TRACE("mrs_OpenFile: WAV");
                if(mrs_foundRiffChunk(&handle->mediumAudio, RIFF_ID('f', 'm', 't', ' '),
                                      &chunk, chunkLimit) == FALSE)
                {
                    return 0;
                }

                if(mrs_MediumRead(&handle->mediumAudio, (VOID*)&fmt, sizeof(fmt)) <= 0)
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

                MRS_TRACE("mrs_OpenFile: WAV PCM numChannel %i", fmt.numChannels);

                if(mrs_foundRiffChunk(&handle->mediumAudio, RIFF_ID('d', 'a', 't', 'a'),
                                      &chunk, chunkLimit) == FALSE)
                {
                    return FALSE;
                }

                data = (MRS_CODEC_RIFF_DATA_T*) sxr_Malloc(sizeof(MRS_CODEC_RIFF_DATA_T));

                data->format                = MRS_RIFF_FORMAT_WAVE;
                data->dataChunk             = mrs_MediumTell(&handle->mediumAudio)-sizeof(MRS_RIFF_CHUNK_T);
                data->audioChunkRemaining   = HAL_ENDIAN_LITTLE_32(chunk.size);
                data->videoChunkRemaining   = 0;

                handle->videoLength         = 0;
                handle->videoBuffer         = 0;
                handle->audioLength         = 1024*32;
                handle->audioBuffer         = (UINT8*) sxr_Malloc(handle->audioLength);
                handle->videoBuffer         = NULL;
                handle->stream.mode         = AVPS_PLAY_MODE_PCM;

                return (VOID*)data;

            case RIFF_ID('A', 'V', 'I', ' '):
                MRS_TRACE("mrs_OpenFile: AVI");

                if(mrs_foundRiffListChunk(&handle->mediumAudio, RIFF_ID('h', 'd', 'r', 'l'),
                                          &chunk, chunkLimit) == FALSE)
                {
                    MRS_TRACE("mrs_OpenFile: AVI hdrl chunk not found");
                    return 0;
                }

                subChunkLimit = mrs_MediumTell(&handle->mediumAudio) + HAL_ENDIAN_LITTLE_32(chunk.size) - sizeof(UINT32);

                if(mrs_foundRiffChunk(&handle->mediumAudio, RIFF_ID('a', 'v', 'i', 'h'),
                                      &chunk, subChunkLimit) == FALSE)
                {
                    MRS_TRACE("mrs_OpenFile: AVI avih chunk not found");
                    return 0;
                }

                mrs_MediumRead(&handle->mediumAudio, (VOID*)&avih, sizeof(MRS_RIFF_AVIH_CHUNK_T));

                data = (MRS_CODEC_RIFF_DATA_T*) sxr_Malloc(sizeof(MRS_CODEC_RIFF_DATA_T));

                data->format                = MRS_RIFF_FORMAT_AVI;
                data->dataChunk             =  0;
                data->idx1Chunk             =  0;
                data->audioChunkRemaining   =  0;
                data->videoChunkRemaining   =  0;
                data->streamAudio           = -1;
                data->streamVideo           = -1;
                data->videoFrame            =  0;

                for(i = 0; i < avih.nbStreams; ++i)
                {
                    if(mrs_foundRiffListChunk(&handle->mediumAudio, RIFF_ID('s', 't', 'r', 'l'),
                                              &chunk, subChunkLimit) == FALSE)
                    {
                        MRS_TRACE("mrs_OpenFile: AVI strl[%i] chunk not found", i);
                        sxr_Free(data);
                        return 0;
                    }

                    subSubChunkLimit = mrs_MediumTell(&handle->mediumAudio) + HAL_ENDIAN_LITTLE_32(chunk.size)
                                       - sizeof(UINT32);
                    if(mrs_foundRiffChunk(&handle->mediumAudio, RIFF_ID('s', 't', 'r', 'h'),
                                          &chunk, subSubChunkLimit) == FALSE)
                    {
                        MRS_TRACE("mrs_OpenFile: AVI strh[%i] chunk not found", i);
                        sxr_Free(data);
                        return 0;
                    }
                    mrs_MediumRead(&handle->mediumAudio, (VOID*)&id, sizeof(UINT32));
                    switch(HAL_ENDIAN_LITTLE_32(id))
                    {
                        case RIFF_ID('a', 'u', 'd', 's'):
                            MRS_TRACE("mrs_OpenFile: stream AUD", i);
                            data->streamAudio = i;
                            mrs_MediumSeek(&handle->mediumAudio, HAL_ENDIAN_LITTLE_32(chunk.size)-sizeof(UINT32), FS_SEEK_CUR);
                            if(mrs_foundRiffChunk(&handle->mediumAudio, RIFF_ID('s', 't', 'r', 'f'),
                                                  &chunk, subSubChunkLimit) == FALSE)
                            {
                                MRS_TRACE("mrs_OpenFile: AVI strf[%i] chunk not found", i);
                                sxr_Free(data);
                                return 0;
                            }
                            mrs_MediumRead(&handle->mediumAudio, (VOID*)&strfAudio, sizeof(MRS_RIFF_STRF_AUDIO_CHUNK_T));
                            MRS_TRACE("mrs_OpenFile: Audio rate %i channels %i depth %i codec %04x",
                                      strfAudio.sampleRate, strfAudio.nbChannels, strfAudio.bitsPerSample,
                                      strfAudio.format);

                            if(strfAudio.format != 0x0055
#ifdef MRS_AMR_MJPEG_MODE
                                    && strfAudio.format != 0x00FE
#endif /* MRS_AMR_MJPEG_MODE */
                              )
                            {
                                // Format not support 0x0055=MP3 0x00FE=AMR RING
                                data->streamAudio = -1;
                            }
                            break;
                        case RIFF_ID('v', 'i', 'd', 's'):
                            MRS_TRACE("mrs_OpenFile: stream VID", i);
                            mrs_MediumRead(&handle->mediumAudio, (VOID*)&strhVideo, sizeof(MRS_RIFF_STRH_VIDEO_CHUNK_T));
                            if(HAL_ENDIAN_LITTLE_32(strhVideo.codec) != RIFF_ID('M', 'J', 'P', 'G'))
                            {
                                MRS_TRACE("mrs_OpenFile: VID not supported codec", i);
                                // Format not support
                                break;
                            }
                            data->streamVideo  = i;
                            mrs_MediumSeek(&handle->mediumAudio,
                                           HAL_ENDIAN_LITTLE_32(chunk.size)-sizeof(MRS_RIFF_STRH_VIDEO_CHUNK_T)-sizeof(UINT32),
                                           FS_SEEK_CUR);
                            if(mrs_foundRiffChunk(&handle->mediumAudio, RIFF_ID('s', 't', 'r', 'f'),
                                                  &chunk, subSubChunkLimit) == FALSE)
                            {
                                MRS_TRACE("mrs_OpenFile: AVI strf[%i] chunk not found", i);
                                sxr_Free(data);
                                return 0;
                            }
                            mrs_MediumRead(&handle->mediumAudio, (VOID*)&strfVideo, sizeof(MRS_RIFF_STRF_VIDEO_CHUNK_T));
                            MRS_TRACE("mrs_OpenFile: Video rate %i scale %i %ix%i depth %i codec %#x",
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
                    mrs_MediumSeek(&handle->mediumAudio, subSubChunkLimit, FS_SEEK_SET);
                }

                mrs_MediumSeek(&handle->mediumAudio, subChunkLimit, FS_SEEK_SET);

                handle->stream.audioSampleRate   = strfAudio.sampleRate;
                handle->stream.channelNb         = strfAudio.nbChannels;
                MRS_TRACE("mrs_OpenFile: Channel Number = %d ratio %i!",
                          handle->stream.channelNb, handle->stream.audioVideoRatio);

                if(data->streamVideo == (UINT8)-1 && data->streamAudio == (UINT8)-1)
                {
                    sxr_Free(data);
                    return 0;
                }
                switch(strfAudio.format)
                {
                    case 0x0055: // MP3
                        handle->stream.mode = AVPS_PLAY_MODE_MP3_MJPEG;
                        if(data->streamVideo == (UINT8)-1)
                        {
                            handle->stream.mode = AVPS_PLAY_MODE_MP3;
                        }
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
#ifdef MRS_AMR_MJPEG_MODE
                        data->isAmr = 0;
#endif /* MRS_AMR_MJPEG_MODE */
                        break;
#ifdef MRS_AMR_MJPEG_MODE
                    case 0x00FE: // AMR Ring
                        handle->stream.mode = AVPS_PLAY_MODE_AMR_MJPEG;
                        if(data->streamVideo == (UINT8)-1)
                        {
                            handle->stream.mode = AVPS_PLAY_MODE_AMR_RING;
                        }
                        handle->stream.audioVideoRatio   = (((UINT32)handle->stream.audioSampleRate<<8) * strhVideo.scale) /
                                                           (strhVideo.rate * 160);
                        data->isAmr = 1;
                        break;
#endif /* MRS_AMR_MJPEG_MODE */
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
                MRS_TRACE("mrs_OpenFile: RIFF VIDEO BUFFER ALLOCATED !");

                data->audioChunkRemaining   = 0;
                data->videoChunkRemaining   = 0;

                if(mrs_foundRiffListChunk(&handle->mediumAudio, RIFF_ID('m', 'o', 'v', 'i'),
                                          &chunk, chunkLimit) == TRUE)
                {
                    data->dataChunk = mrs_MediumTell(&handle->mediumAudio) - sizeof(MRS_RIFF_CHUNK_T);

                    MRS_TRACE("Format movi found offset=%#x", data->dataChunk);
                    subChunkLimit = mrs_MediumTell(&handle->mediumAudio) +
                                    (HAL_ENDIAN_LITTLE_32(chunk.size)-sizeof(UINT32));
                    data->audioChunkRemaining = 0;

                    if(mrs_MediumSeek(&handle->mediumAudio, subChunkLimit, FS_SEEK_SET))
                    {
                        if(mrs_foundRiffChunk(&handle->mediumAudio, RIFF_ID('i', 'd', 'x', '1'),
                                              &chunk, chunkLimit) == TRUE)
                        {
                            MRS_TRACE("mrs_OpenFile: Index V1 found");
                            data->idx1Chunk = mrs_MediumTell(&handle->mediumAudio) - sizeof(MRS_RIFF_CHUNK_T);
                        }
                    }

                    return (VOID*)data;
                }
                sxr_Free(data);
                return 0;
            default:
                MRS_TRACE("mrs_OpenFile: unknown RIFF format");
        }
    }

    return 0;
}

PRIVATE VOID mrs_closeRiff(MRS_HANDLE_T handle)
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

    if (handle->codecData != NULL)
    {
        sxr_Free(handle->codecData);
        handle->codecData = NULL;
    }
}

PRIVATE UINT32 mrs_getVideoDataRiff(MRS_HANDLE_T handle, UINT8* data, UINT32 size)
{
    MRS_CODEC_RIFF_DATA_T*  codecData;
    MRS_RIFF_CHUNK_T        chunk;
    UINT32                  readSize;

    MRS_PROFILE_FUNCTION_ENTER(mrs_codecGetVideoData);

    MRS_TRACE("RIFF Fetch video data start for:%#x", (UINT32)data);
    MRS_TRACE(__func__);

    codecData = (MRS_CODEC_RIFF_DATA_T*) handle->codecData;

    if(codecData->videoChunkRemaining == 0)
    {
        MRS_TRACE("RIFF Fetch video: no more chunk ??");
        MRS_PROFILE_FUNCTION_EXIT(mrs_codecGetVideoData);
        return 0;
    }

    MRS_ASSERT(codecData->videoChunkRemaining < size, "mrs_getVideoDataRiffInfo: overflow");
    readSize = mrs_MediumRead(&handle->mediumVideo, (VOID*)data, codecData->videoChunkRemaining);
    if(readSize <= 0)
    {
        readSize                       = 0;
        codecData->videoChunkRemaining = 0;
    }

    codecData->videoChunkRemaining  -= readSize;

    if(mrs_foundRiffChunk(&handle->mediumVideo, RIFF_ID('0', '0' + codecData->streamVideo, 'd', 'b'),
                          &chunk, -1) == FALSE)
    {
        codecData->videoChunkRemaining = 0;
    }
    else
    {
        codecData->videoChunkRemaining = HAL_ENDIAN_LITTLE_32(chunk.size);
        codecData->videoFrame++;
    }

    MRS_TRACE("RIFF Fetch video: Chunk Fetched !");
    MRS_PROFILE_FUNCTION_EXIT(mrs_codecGetVideoData);

    return readSize;
}

PRIVATE UINT32 mrs_getAudioDataRiff(MRS_HANDLE_T handle, UINT8* data, UINT32 size)
{
    MRS_CODEC_RIFF_DATA_T*  codecData;
    INT32                   i;
    MRS_RIFF_CHUNK_T        chunk;
    UINT32                  readSize;
    UINT32                  dataRead = 0;
    UINT8*                  writePtr = data;

    MRS_PROFILE_FUNCTION_ENTER(mrs_codecGetAudioData);

    MRS_TRACE(__func__);

    codecData = (MRS_CODEC_RIFF_DATA_T*) handle->codecData;

    if(codecData->audioChunkRemaining == 0)
    {
        MRS_PROFILE_FUNCTION_EXIT(mrs_codecGetAudioData);
        return 0;
    }

    do
    {
        readSize = size;
        if(size > codecData->audioChunkRemaining)
        {
            readSize = codecData->audioChunkRemaining;
        }

        i = mrs_MediumRead(&handle->mediumAudio, writePtr, readSize);
        if(i <= 0)
        {
            i                              = 0;
            codecData->audioChunkRemaining = 0;
        }

        codecData->audioChunkRemaining  -= i;
        size                            -= i;
        dataRead                        += i;
        writePtr                        += i;

        if(codecData->format == MRS_RIFF_FORMAT_AVI)
        {
            if(codecData->audioChunkRemaining == 0)
            {
#ifdef MRS_RIFF_DEBUG
                MRS_TRACE("RIFF audio get: Next chunk");
#endif
                // Found next chunk
                if(mrs_foundRiffChunk(&handle->mediumAudio,
                                      RIFF_ID('0', '0' + codecData->streamAudio, 'w', 'b'),
                                      &chunk, -1) == TRUE)
                {
#ifdef MRS_RIFF_DEBUG
                    MRS_TRACE("RIFF audio get: Next chunk found size=%i pos=%i", chunk.size,
                              codecData->audioPosition);
#endif
                    codecData->audioChunkRemaining = HAL_ENDIAN_LITTLE_32(chunk.size);
                }
            }
        }
    }
    while(codecData->audioChunkRemaining && size);

    MRS_PROFILE_FUNCTION_EXIT(mrs_codecGetAudioData);
    return dataRead;
}

PRIVATE BOOL mrs_playRiff(MRS_HANDLE_T handle, AVPS_CFG_T* avpsCfg)
{
    MRS_CODEC_RIFF_DATA_T*  data;
    MRS_RIFF_CHUNK_T        chunk;
    UINT32                  size;
    UINT32                  chunkLimit;
    LCDD_FBW_T              frameWindows;
    UINT32                  audioWriteBuffer;

    MRS_PROFILE_FUNCTION_ENTER(mrs_codecPlay);

    MRS_TRACE(__func__);

    data = (MRS_CODEC_RIFF_DATA_T*) handle->codecData;
    if(data->audioChunkRemaining == 0)
    {
        mrs_MediumSeek(&handle->mediumAudio, data->dataChunk, FS_SEEK_SET);
        if(mrs_MediumRead(&handle->mediumAudio, (VOID*)&chunk, sizeof(chunk)) <= 0)
        {
            MRS_PROFILE_FUNCTION_EXIT(mrs_codecPlay);
            return FALSE;
        }
        if(data->format == MRS_RIFF_FORMAT_AVI)
        {
            data->videoFrame            =  0;
            chunkLimit = data->dataChunk + sizeof(chunk) + HAL_ENDIAN_LITTLE_32(chunk.size);
            mrs_MediumSeek(&handle->mediumAudio, data->dataChunk + sizeof(chunk), FS_SEEK_SET);
            mrs_MediumSeek(&handle->mediumVideo, data->dataChunk + sizeof(chunk), FS_SEEK_SET);
            if(mrs_foundRiffChunk(&handle->mediumAudio, RIFF_ID('0', '0' + data->streamAudio, 'w', 'b'),
                                  &chunk, chunkLimit) == FALSE)
            {
                MRS_TRACE("RIFF audio track not found");
                MRS_PROFILE_FUNCTION_EXIT(mrs_codecPlay);
                return FALSE;
            }
            MRS_TRACE("RIFF audio play: pos %i @@@@", mrs_MediumTell(&handle->mediumAudio));
            data->audioChunkRemaining = HAL_ENDIAN_LITTLE_32(chunk.size);
#ifdef MRS_AMR_MJPEG_MODE
            if(data->isAmr)
            {
                mrs_MediumSeek(&handle->mediumAudio, 6, FS_SEEK_CUR);
                data->audioChunkRemaining -= 6;
            }
#endif /* MRS_AMR_MJPEG_MODE */
            if(data->streamVideo != (UINT8)-1)
            {
                if(mrs_foundRiffChunk(&handle->mediumVideo, RIFF_ID('0', '0' + data->streamVideo, 'd', 'b'),
                                      &chunk, chunkLimit) == FALSE)
                {
                    MRS_TRACE("RIFF video track not found");
                    handle->stream.mode       = AVPS_PLAY_MODE_MP3;
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
            size = mrs_getAudioDataRiff(handle, (UINT8*)audioWriteBuffer, size);
            handle->bufferContext.audioRemainingSize += size;
        }
    }
    if(handle->bufferContext.audioRemainingSize != handle->audioLength)
    {
        size = mrs_getAudioDataRiff(handle, handle->audioBuffer,
                                    handle->audioLength-handle->bufferContext.audioRemainingSize);
        handle->bufferContext.audioRemainingSize += size;
    }

    if(data->streamVideo != (UINT8)-1)
    {
        mrs_getVideoDataRiff(handle, handle->videoBuffer, handle->videoLength/2);
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


    g_mrsBufferWritePos =
        (UINT8*)((UINT32)handle->bufferContext.bufferAudioReadPosition + handle->bufferContext.audioRemainingSize);
    g_mrsBufferWritePos = (UINT8*)((UINT32)g_mrsBufferWritePos | 0x80000000);
    if((UINT32)g_mrsBufferWritePos >= (UINT32)handle->audioBuffer + handle->audioLength)
    {
        g_mrsBufferWritePos = (UINT8*)((UINT32)g_mrsBufferWritePos - handle->audioLength);
    }

    if(avps_Play(handle->config.audioPath, &handle->stream, avpsCfg,
                 &frameWindows, &handle->bufferContext) == AVPS_ERR_NO)
    {
        MRS_PROFILE_FUNCTION_EXIT(mrs_codecPlay);
        return TRUE;
    }
    MRS_PROFILE_FUNCTION_EXIT(mrs_codecPlay);
    return FALSE;
}

PRIVATE VOID mrs_syncAudioVideoRiff(MRS_HANDLE_T handle)
{
    MRS_CODEC_RIFF_DATA_T*  data;
    MRS_RIFF_CHUNK_T        chunk;

    MRS_TRACE(__func__);

    data = (MRS_CODEC_RIFF_DATA_T*) handle->codecData;

    mrs_MediumSeek(&handle->mediumAudio,
                   mrs_MediumTell(&handle->mediumVideo)+data->videoChunkRemaining, FS_SEEK_SET);

    if(mrs_foundRiffChunk(&handle->mediumAudio, RIFF_ID('0', '0' + data->streamAudio, 'w', 'b'), &chunk, -1)
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
        mrs_skipVideoRiff(handle);
    }
}

PRIVATE VOID mrs_pauseRiff(MRS_HANDLE_T handle)
{
    MRS_CODEC_RIFF_DATA_T*  data;

    MRS_TRACE(__func__);

    data = (MRS_CODEC_RIFF_DATA_T*) handle->codecData;

    if(data->format == MRS_RIFF_FORMAT_AVI)
    {
        MRS_TRACE("RIFF pause resync Audio/video");

        // Resync audio/video
        mrs_syncAudioVideoRiff(handle);
    }
}

PRIVATE VOID mrs_skipVideoRiff(MRS_HANDLE_T handle)
{
    MRS_CODEC_RIFF_DATA_T*  codecData;
    MRS_RIFF_CHUNK_T        chunk;

    MRS_TRACE(__func__);

    codecData = (MRS_CODEC_RIFF_DATA_T*) handle->codecData;

    if(codecData->videoChunkRemaining != 0)
    {
        mrs_MediumSeek(&handle->mediumVideo, codecData->videoChunkRemaining, FS_SEEK_CUR);

        if(mrs_foundRiffChunk(&handle->mediumVideo, RIFF_ID('0', '0' + codecData->streamVideo, 'd', 'b'),
                              &chunk, -1) == FALSE)
        {
            codecData->videoChunkRemaining = 0;
        }
        else
        {
            codecData->videoChunkRemaining = HAL_ENDIAN_LITTLE_32(chunk.size);
            codecData->videoFrame++;
        }
    }
}

PRIVATE UINT32 mrs_tellRiff(MRS_HANDLE_T handle)
{
    MRS_CODEC_RIFF_DATA_T*  codecData;

    MRS_TRACE(__func__);

    MRS_PROFILE_FUNCTION_ENTER(mrs_codecTell);

    codecData = (MRS_CODEC_RIFF_DATA_T*) handle->codecData;

    switch(codecData->format)
    {
        case MRS_RIFF_FORMAT_AVI:
            MRS_PROFILE_FUNCTION_EXIT(mrs_codecTell);
            return (codecData->videoFrame<<8)/handle->info.videoRate;

        case MRS_RIFF_FORMAT_WAVE:
            MRS_PROFILE_FUNCTION_EXIT(mrs_codecTell);
            return (mrs_MediumTell(&handle->mediumAudio)-(codecData->dataChunk+sizeof(MRS_RIFF_CHUNK_T))) /
                   (handle->stream.channelNb*sizeof(UINT16)*handle->stream.audioSampleRate);
    }
    MRS_PROFILE_FUNCTION_EXIT(mrs_codecTell);
    return 0;
}

PRIVATE UINT32 mrs_foundVideoFrameRiff(MRS_HANDLE_T handle, UINT32 frame)
{
    MRS_CODEC_RIFF_DATA_T*  codecData;
    MRS_RIFF_CHUNK_T        chunk;
    MRS_RIFF_IDX1_ENTRY_T   indexEntry[64];
    UINT32                  dataRead;
    UINT32                  currentFrame;
    UINT32                  i;

    MRS_TRACE("mrs_foundVideoFrameRiff: frame=%i", frame);

    codecData    = (MRS_CODEC_RIFF_DATA_T*) handle->codecData;
    currentFrame = 0;

    mrs_MediumSeek(&handle->mediumVideo, codecData->idx1Chunk, FS_SEEK_SET);
    if(mrs_MediumRead(&handle->mediumVideo, (VOID*)&chunk,
                      sizeof(MRS_RIFF_CHUNK_T)) <= 0)
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

        if(mrs_MediumRead(&handle->mediumVideo, (VOID*)indexEntry, dataRead) <= 0)
        {
            return 0;
        }

        for(i = 0; i < dataRead/sizeof(MRS_RIFF_IDX1_ENTRY_T); ++i)
        {
            if(HAL_ENDIAN_LITTLE_32(indexEntry[i].id) == RIFF_ID('0', '0' + codecData->streamVideo, 'd', 'b'))
            {
                // is video frame
                if(currentFrame == frame)
                {
                    // Frame found
                    MRS_TRACE("mrs_foundVideoFrameRiff: frame found", frame);
                    return HAL_ENDIAN_LITTLE_32(indexEntry[i].offset);
                }
                currentFrame++;
            }
        }

        chunk.size -= dataRead;
    }

    return 0;
}

PRIVATE BOOL mrs_seekRiff(MRS_HANDLE_T handle, UINT32 position)
{
    MRS_CODEC_RIFF_DATA_T*  codecData;
    MRS_RIFF_CHUNK_T        chunk;
    UINT32                  offset;

    MRS_PROFILE_FUNCTION_ENTER(mrs_codecSeek);

    codecData = (MRS_CODEC_RIFF_DATA_T*) handle->codecData;

    avps_Stop(0);

    switch(codecData->format)
    {
        case MRS_RIFF_FORMAT_AVI:
            if(codecData->idx1Chunk == 0)
            {
                MRS_PROFILE_FUNCTION_EXIT(mrs_codecSeek);
                return FALSE;
            }
            offset = mrs_foundVideoFrameRiff(handle, (position*handle->info.videoRate)>>8);
            if(offset == 0)
            {
                // Not found
                codecData->audioChunkRemaining = 0;
                codecData->videoChunkRemaining = 0;
                MRS_PROFILE_FUNCTION_EXIT(mrs_codecSeek);
                return FALSE;
            }
            codecData->videoFrame = (position*handle->info.videoRate)>>8;

            mrs_MediumSeek(&handle->mediumVideo, codecData->dataChunk+offset+sizeof(UINT32), FS_SEEK_SET);
            if(mrs_MediumRead(&handle->mediumVideo, (VOID*)&chunk, sizeof(chunk)) <= 0)
            {
                codecData->audioChunkRemaining = 0;
                codecData->videoChunkRemaining = 0;
                MRS_PROFILE_FUNCTION_EXIT(mrs_codecSeek);
                return FALSE;
            }
            codecData->videoChunkRemaining = HAL_ENDIAN_LITTLE_32(chunk.size);

            mrs_syncAudioVideoRiff(handle);

            MRS_PROFILE_FUNCTION_EXIT(mrs_codecSeek);
            return TRUE;

        case MRS_RIFF_FORMAT_WAVE:
            mrs_MediumSeek(&handle->mediumAudio, codecData->dataChunk, FS_SEEK_SET);
            if(mrs_MediumRead(&handle->mediumAudio, (VOID*)&chunk, sizeof(chunk)) <= 0)
            {
                codecData->audioChunkRemaining = 0;
                MRS_PROFILE_FUNCTION_EXIT(mrs_codecSeek);
                return FALSE;
            }
            offset = position*handle->stream.channelNb*sizeof(UINT16)*handle->stream.audioSampleRate;
            if(offset > HAL_ENDIAN_LITTLE_32(chunk.size))
            {
                codecData->audioChunkRemaining = 0;
                MRS_PROFILE_FUNCTION_EXIT(mrs_codecSeek);
                return FALSE;
            }
            mrs_MediumSeek(&handle->mediumAudio, offset, FS_SEEK_CUR);
            codecData->audioChunkRemaining = HAL_ENDIAN_LITTLE_32(chunk.size)-offset;

            handle->bufferContext.audioRemainingSize      = 0;
            handle->bufferContext.bufferAudioReadPosition = handle->audioBuffer;

            MRS_PROFILE_FUNCTION_EXIT(mrs_codecSeek);
            return TRUE;
    }
    MRS_PROFILE_FUNCTION_EXIT(mrs_codecSeek);
    return FALSE;
}

// =============================================================================
// FUNCTIONS
// =============================================================================

