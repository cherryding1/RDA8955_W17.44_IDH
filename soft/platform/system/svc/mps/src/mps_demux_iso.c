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

PRIVATE VOID* mps_openIso(MPS_HANDLE_T handle);

PRIVATE VOID mps_closeIso(MPS_HANDLE_T handle);

/* PRIVATE VOID mps_pauseIso(MPS_HANDLE_T handle); */

PRIVATE UINT32 mps_getAudioDataIso(MPS_HANDLE_T handle, UINT8* data, UINT32 size);

PRIVATE UINT32 mps_getVideoDataIso(MPS_HANDLE_T handle, UINT8* data, UINT32 size);

PRIVATE BOOL mps_playIso(MPS_HANDLE_T handle, AVPS_CFG_T* avpsCfg);

PRIVATE VOID mps_skipVideoIso(MPS_HANDLE_T handle);

/* PRIVATE UINT32 mps_tellIso(MPS_HANDLE_T handle); */

/* PRIVATE BOOL mps_seekIso(MPS_HANDLE_T handle, UINT32 position); */

// =============================================================================
// MACRO
// =============================================================================

#define ISO_ID(A, B, C, D) \
    (HAL_ENDIAN_LITTLE_32((((A)<<0)|((B)<<8)|((C)<<16)|((D)<<24))))

#define PACKED __attribute__((packed))

#define MPS_ISO_CHUNK_BUFFER_SIZE       8
#define MPS_ISO_SAMPLE_SIZE_BUFFER_SIZE 8

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// MPS_ISO_BOX_T
// -----------------------------------------------------------------------------
/// Box header
// =============================================================================
typedef struct
{
    UINT32 size;
    UINT32 id;
}  PACKED MPS_ISO_BOX_T;

// =============================================================================
// MPS_ISO_FULL_BOX_T
// -----------------------------------------------------------------------------
/// Full Box header
// =============================================================================
typedef struct
{
    UINT32 size;
    UINT32 id;
    UINT32 flag    :24;
    UINT32 version :8;
}  PACKED MPS_ISO_FULL_BOX_T;

// =============================================================================
// MPS_ISO_STSD_ENTRY_T
// -----------------------------------------------------------------------------
/// Stsd entry
// =============================================================================
typedef struct
{
    UINT8  reserved[6];
    UINT16 dataReferenceIndex;
} PACKED MPS_ISO_STSD_ENTRY_T ;


// =============================================================================
// MPS_ISO_STSD_VIDEO_ENTRY_T
// -----------------------------------------------------------------------------
/// Video description
// =============================================================================
typedef struct
{
    MPS_ISO_STSD_ENTRY_T     entry;
    UINT32                   reserved1[4];
    UINT16                   width;
    UINT16                   height;
    UINT32                   horizResolution;
    UINT32                   vertResolution;
    UINT32                   reserved2;
    UINT16                   frameCount;
    UINT8                    compressorName[32];
    UINT16                   depth;
    UINT16                   reserved3;
} PACKED MPS_ISO_STSD_VIDEO_ENTRY_T ;

// =============================================================================
// MPS_ISO_STSD_AUDIO_ENTRY_T
// -----------------------------------------------------------------------------
/// Audio description
// =============================================================================
typedef struct
{
    MPS_ISO_STSD_ENTRY_T     entry;
    UINT32                   reserved1[2];
    UINT16                   channelCount;
    UINT16                   sampleSize;
    UINT32                   reserved2;
    UINT32                   sampleRate; // 16.16
} PACKED MPS_ISO_STSD_AUDIO_ENTRY_T ;

// =============================================================================
// MPS_ISO_MOVIE_HEADER_T
// -----------------------------------------------------------------------------
/// Movie header
// =============================================================================
typedef struct
{
    UINT32 creationTime;
    UINT32 modificationTime;
    UINT32 timescale;
    UINT32 duration;

    UINT32 rate;
    UINT16 volume;
    UINT32 matrix[9];
    UINT32 reserved2[6];
    UINT32 nextTrackID;
} PACKED MPS_ISO_MOVIE_HEADER_T ;

// =============================================================================
// MPS_ISO_MOVIE_HEADER_V1_T
// -----------------------------------------------------------------------------
/// Movie header v1
// =============================================================================
typedef struct
{
    UINT64 creationTime;
    UINT64 modificationTime;
    UINT32 timescale;
    UINT64 duration;

    UINT32 rate;
    UINT16 volume;
    UINT16 reserved1[5];
    UINT32 matrix[9];
    UINT32 reserved2[6];
    UINT32 nextTrackID;
} PACKED MPS_ISO_MOVIE_HEADER_V1_T ;

// =============================================================================
// MPS_ISO_TRAK_HEADER_T
// -----------------------------------------------------------------------------
/// Track header
// =============================================================================
typedef struct
{
    UINT32 creationTime;
    UINT32 modificationTime;
    UINT32 trackId;
    UINT32 reserved1;
    UINT32 duration;

    UINT32 reserved2[2];
    UINT16 layer;
    UINT16 alternateGroup;
    UINT16 volume;
    UINT16 reserved3;
    UINT32 matrix[9];
    UINT32 width;
    UINT32 height;
} PACKED MPS_ISO_TRAK_HEADER_T ;


// =============================================================================
// MPS_ISO_TRAK_HEADER_V1_T
// -----------------------------------------------------------------------------
/// Track header
// =============================================================================
typedef struct
{
    UINT64 creationTime;
    UINT64 modificationTime;
    UINT32 trackId;
    UINT32 reserved1;
    UINT64 duration;

    UINT32 reserved2[2];
    UINT16 layer;
    UINT16 alternateGroup;
    UINT16 volume;
    UINT16 reserved3;
    UINT32 matrix[9];
    UINT32 width;
    UINT32 height;
} PACKED MPS_ISO_TRAK_HEADER_V1_T ;


// =============================================================================
// MPS_ISO_CHUNK_SIZE_HEADER_T
// -----------------------------------------------------------------------------
/// Header chunk size box
// =============================================================================
typedef struct
{
    UINT32 sampleSize;
    UINT32 sampleCount;
} PACKED MPS_ISO_CHUNK_SIZE_HEADER_T ;


// =============================================================================
// MPS_ISO_TIME_TO_SAMPLE_ENTRY_T
// -----------------------------------------------------------------------------
/// Entry of time to sample section
// =============================================================================
typedef struct
{
    UINT32 count;
    UINT32 delta;
} PACKED MPS_ISO_TIME_TO_SAMPLE_ENTRY_T ;

// =============================================================================
// MPS_ISO_MEDIA_HEADER_T
// -----------------------------------------------------------------------------
/// Media header
// =============================================================================
typedef struct
{
    UINT32 creationTime;
    UINT32 modificationTime;
    UINT32 timescale;
    UINT32 duration;

    UINT16 pad      :1;
    UINT16 langage1 :5;
    UINT16 langage2 :5;
    UINT16 langage3 :5;
    UINT16 reserved1;
} PACKED MPS_ISO_MEDIA_HEADER_T ;

// =============================================================================
// MPS_ISO_MEDIA_HEADER_V1_T
// -----------------------------------------------------------------------------
/// Media header
// =============================================================================
typedef struct
{
    UINT64 creationTime;
    UINT64 modificationTime;
    UINT32 timescale;
    UINT64 duration;

    UINT16 pad      :1;
    UINT16 langage1 :5;
    UINT16 langage2 :5;
    UINT16 langage3 :5;
    UINT16 reserved1;
} PACKED MPS_ISO_MEDIA_HEADER_V1_T ;

// =============================================================================
// MPS_ISO_ES_DESCRIPTOR_T
// -----------------------------------------------------------------------------
/// ES descriptor (Tag 0x03)
// =============================================================================
typedef struct
{
    UINT16 esId;
    UINT8  streamDependenceFlag :1;
    UINT8  urlFlag              :1;
    UINT8  ocrStreamFlag        :1;
    UINT8  streamPriority       :5;
} PACKED MPS_ISO_ES_DESCRIPTOR_T ;

// =============================================================================
// MPS_ISO_DECODER_CONFIG_DESCRIPTOR_T
// -----------------------------------------------------------------------------
/// Decoder config descriptor (Tag 0x04)
// =============================================================================
typedef struct
{
    UINT8  objectType;
    UINT8  streamType            :6;
    UINT8  upStream              :1;
    UINT8  reserved              :1;
    UINT8  bufferSizeDb[3];
    UINT32 maxBitRate;
    UINT32 avgBitRate;
} PACKED MPS_ISO_DECODER_CONFIG_DESCRIPTOR_T ;

// =============================================================================
// MPS_ISO_SAMPLES_TO_CHUNK_ENTRY_T
// -----------------------------------------------------------------------------
/// Number samples in chunk
// =============================================================================
typedef struct
{
    UINT32 firstChunk;
    UINT32 samplesPerChunk;
    UINT32 sampleDescriptionIndex;
} PACKED MPS_ISO_SAMPLES_TO_CHUNK_ENTRY_T ;

// =============================================================================
// MPS_DEMUX_ISO_SAMPLES_TO_CHUNK_T
// -----------------------------------------------------------------------------
/// List of chunk description
// =============================================================================
typedef struct
{
    UINT32 firstChunk;
    UINT32 samplesPerChunk;
} MPS_DEMUX_ISO_SAMPLES_TO_CHUNK_T ;

// =============================================================================
// MPS_DEMUX_ISO_SAMPLE_T
// -----------------------------------------------------------------------------
/// Sample description
// =============================================================================
typedef struct
{
    UINT32 offset;
    UINT32 size;
} MPS_DEMUX_ISO_SAMPLE_T ;

// =============================================================================
// MPS_DEMUX_ISO_CODEC_T
// -----------------------------------------------------------------------------
/// List of demux
// =============================================================================
typedef enum
{
    MPS_DEMUX_ISO_CODEC_UNKNOW = 0,

    // Audio
    MPS_DEMUX_ISO_CODEC_AAC_MAIN,
    MPS_DEMUX_ISO_CODEC_AAC_LC,
    MPS_DEMUX_ISO_CODEC_AAC_HE,

    MPS_DEMUX_ISO_CODEC_MP3,

    MPS_DEMUX_ISO_CODEC_AMR,


    // Video
    MPS_DEMUX_ISO_CODEC_MPEG4,

    MPS_DEMUX_ISO_CODEC_H263,
} MPS_DEMUX_ISO_CODEC_T ;


// =============================================================================
// MPS_DEMUX_ISO_STREAM_T
// -----------------------------------------------------------------------------
/// Internal data for manage one stream
// =============================================================================
typedef struct
{
    UINT32                           id;

    UINT32                           chunkSize;
    UINT32                           chunkOffset;
    UINT32                           chunkSampleInChunk;

    UINT32                           bufferChunk[MPS_ISO_CHUNK_BUFFER_SIZE];
    UINT32                           nbChunk;
    UINT32                           nbBufferChunk;
    UINT32                           posBufferChunk;
    UINT32                           posInBufferChunk;

    UINT32                           bufferSampleSize[MPS_ISO_SAMPLE_SIZE_BUFFER_SIZE];
    UINT32                           nbSampleSize;
    UINT32                           nbBufferSampleSize;
    UINT32                           posBufferSampleSize;
    UINT32                           posInBufferSampleSize;

    MPS_DEMUX_ISO_SAMPLES_TO_CHUNK_T bufferNbSampleInChunk[MPS_ISO_SAMPLE_SIZE_BUFFER_SIZE];
    UINT32                           nbNbSampleInChunk;
    UINT32                           nbBufferNbSampleInChunk;
    UINT32                           posBufferNbSampleInChunk;
    UINT32                           posInBufferNbSampleInChunk;

    INT32                            posInChunk;
    INT32                            posInSample;
    INT32                            sampleRemaining;

    MPS_DEMUX_ISO_CODEC_T            codec;

    UINT32                           isMpeg4     :1;
} MPS_DEMUX_ISO_STREAM_T ;

// =============================================================================
// MPS_DEMUX_ISO_TRACK_T
// -----------------------------------------------------------------------------
/// Track parameter
// =============================================================================
typedef struct
{
    MPS_DEMUX_ISO_STREAM_T stream;

    UINT32                 timeScale;
    UINT32                 duration;
    UINT16                 sampleSize;
    UINT16                 sampleRate;
    UINT16                 nbChannel;
    UINT32                 delta;
    UINT16                 w, h;
    UINT32                 audioTrack  :1;
    UINT32                 videoTrack  :1;
} MPS_DEMUX_ISO_TRACK_T ;


// =============================================================================
// MPS_DEMUX_ISO_DATA_T
// -----------------------------------------------------------------------------
/// Internal data for iso demux
// =============================================================================
typedef struct
{
    MPS_DEMUX_ISO_STREAM_T streamVideo;
    MPS_DEMUX_ISO_STREAM_T streamAudio;
} MPS_DEMUX_ISO_DATA_T ;

// =============================================================================
// PRIVATE VARIABLES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PROTECTED MPS_DEMUX_T g_isoDemux =
{
    .open            = mps_openIso,
    .close           = mps_closeIso,
    .play            = mps_playIso,
    /*     .pause           = mps_pauseIso, */
    .getAudioData    = mps_getAudioDataIso,
    .getVideoData    = mps_getVideoDataIso,
    .skipVideo       = mps_skipVideoIso,
    /*     .seek            = mps_seekIso, */
    /*     .tell            = mps_tellIso */
};

// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

PRIVATE VOID mps_skipIsoBox(MPS_MEDIUM_T* medium, MPS_ISO_FULL_BOX_T* box)
{
    if(box->size)
    {
        mps_MediumSeek(medium, box->size, FS_SEEK_CUR);
        box->size = 0;
    }
}

PRIVATE UINT32 mps_readDataInIsoBox(MPS_MEDIUM_T* medium, MPS_ISO_FULL_BOX_T* box, VOID* buffer, UINT32 size)
{
    if(box->size == 0)
    {
        return 0;
    }

    if(box->size < size)
    {
        size = box->size;
    }

    mps_MediumRead(medium, buffer, size);

    box->size -= size;

    return size;
}

PRIVATE UINT8 mps_readNextIsoBox(MPS_MEDIUM_T* medium, MPS_ISO_FULL_BOX_T* box, MPS_ISO_FULL_BOX_T* parent)
{
    if(parent && parent->size == 0)
    {
        return FALSE;
    }

    if(mps_MediumRead(medium, (VOID*)box, sizeof(MPS_ISO_BOX_T)) <= 0)
    {
        return FALSE;
    }
    // Test if the first byte is padding byte
    if(*((UINT8*)&box->id) == 0)
    {
        mps_MediumSeek(medium, 1-sizeof(MPS_ISO_BOX_T), FS_SEEK_CUR);
        if(mps_MediumRead(medium, (VOID*)box, sizeof(MPS_ISO_BOX_T)) <= 0)
        {
            return FALSE;
        }
    }

    box->size     = HAL_ENDIAN_BIG_32(box->size);
    if(parent)
    {
        if(parent->size < box->size)
        {
            parent->size -= sizeof(MPS_ISO_BOX_T);
            mps_skipIsoBox(medium, box);
            return FALSE;
        }
        parent->size -= box->size;
    }
    box->size    -= sizeof(MPS_ISO_BOX_T);

    return TRUE;
}

PRIVATE UINT8 mps_readIsoFullBox(MPS_MEDIUM_T* medium, MPS_ISO_FULL_BOX_T* box)
{
    UINT32 buf;
    if(mps_MediumRead(medium, (UINT8*)&buf, sizeof(UINT32)) <= 0)
    {
        return FALSE;
    }

    *((UINT8*)box + sizeof(MPS_ISO_BOX_T)) = HAL_ENDIAN_BIG_32(buf);
    box->size -= sizeof(UINT32);

    return TRUE;
}


// =============================================================================
// FUNCTIONS
// =============================================================================

#define MPS_ISO_DECODE_ES_LENGTH()                                      \
    do                                                                  \
    {                                                                   \
        length = 0;                                                     \
        do                                                              \
        {                                                               \
            length <<= 7;                                               \
            length  += buffer[pos] & 0x7F;                              \
        }                                                               \
        while(buffer[pos++] & 0x80);                                    \
    } while(0)

PRIVATE VOID mps_isoDecodeSpecificAudioDecoderInfo(MPS_DEMUX_ISO_TRACK_T* track, UINT8* buffer, UINT32 size)
{
    UINT32                               type;
    UINT32                               length;
    UINT32                               pos = 0;

    type = buffer[pos++];
    MPS_ISO_DECODE_ES_LENGTH();

    switch(buffer[pos]>>3)
    {
        case 0x02:
            track->stream.codec = MPS_DEMUX_ISO_CODEC_AAC_LC;
            break;
        case 0x05:
            track->stream.codec = MPS_DEMUX_ISO_CODEC_AAC_HE;
            break;
        default:
            track->stream.codec = MPS_DEMUX_ISO_CODEC_AAC_MAIN;
            break;
    }

    MPS_TRACE("%02x %02x  -> %02x\n", buffer[pos], buffer[pos+1], ((buffer[pos]&0x07) << 1) | (buffer[pos+1]>>7));
    MPS_TRACE("type %02x\n", buffer[pos]>>3);
    MPS_TRACE("nbchan %02x\n", (buffer[pos+1]>>3)&0x0F);
    MPS_TRACE("size %02x\n", size);
}

PRIVATE VOID mps_isoDecodeEs(MPS_DEMUX_ISO_TRACK_T* track, MPS_MEDIUM_T* medium, MPS_ISO_FULL_BOX_T* box, UINT8* buffer)
{
    MPS_ISO_FULL_BOX_T                   subBox;
    UINT32                               pos;
    UINT32                               bufferSize;

    UINT32                               type;
    UINT32                               length;
    MPS_ISO_ES_DESCRIPTOR_T*             esDescriptor;
    MPS_ISO_DECODER_CONFIG_DESCRIPTOR_T* decoderConfigDescription;

    while(mps_readNextIsoBox(medium, &subBox, box) == TRUE)
    {
        switch(subBox.id)
        {
            case ISO_ID('e', 's', 'd', 's'):
                pos = 0;

                mps_readIsoFullBox(medium, &subBox);
                bufferSize = mps_readDataInIsoBox(medium, &subBox, buffer, 512);

                while(pos < bufferSize)
                {
                    type    = buffer[pos++];
                    length -= 1;

                    switch(type)
                    {
                        case 0x03:
                            MPS_ISO_DECODE_ES_LENGTH();
                            esDescriptor = (MPS_ISO_ES_DESCRIPTOR_T*)&buffer[pos];
                            MPS_TRACE("ES_ID %i priotity %i\n", HAL_ENDIAN_BIG_16(esDescriptor->esId),
                                      esDescriptor->streamPriority);
                            pos    += sizeof(MPS_ISO_ES_DESCRIPTOR_T);
                            length -= sizeof(MPS_ISO_ES_DESCRIPTOR_T);
                            if(esDescriptor->ocrStreamFlag)
                            {
                                pos    += 2;
                                length -= 2;
                            }
                            if(esDescriptor->urlFlag)
                            {
                                length -= buffer[pos]+1;
                                pos    += buffer[pos]+1;
                            }
                            if(esDescriptor->streamDependenceFlag)
                            {
                                pos    += 2;
                                length -= 2;
                            }
                            break;
                        case 0x04:
                            MPS_ISO_DECODE_ES_LENGTH();

                            decoderConfigDescription = (MPS_ISO_DECODER_CONFIG_DESCRIPTOR_T*)&buffer[pos];

                            MPS_TRACE("type04 length %i indication 0x%02x streamType 0x%02x\n",
                                      length, buffer[pos], buffer[pos+1]>>2);

                            MPS_TRACE("@@@ object %i\n",
                                      decoderConfigDescription->objectType);

                            switch(decoderConfigDescription->objectType)
                            {
                                // Video
                                case 0x20:
                                    break;
                                // Audio
                                case 0x40: // MPEG4
                                    track->stream.isMpeg4 = 1;
                                case 0x66: // MPEG2 Main
                                case 0x67: // MPEG2 LC
                                case 0x68: // MPEG2 HE
                                    if(length > sizeof(MPS_ISO_DECODER_CONFIG_DESCRIPTOR_T))
                                    {
                                        mps_isoDecodeSpecificAudioDecoderInfo(track, &buffer[pos+sizeof(MPS_ISO_DECODER_CONFIG_DESCRIPTOR_T)],
                                                                              length-sizeof(MPS_ISO_DECODER_CONFIG_DESCRIPTOR_T));
                                    }
                                    break;
                                case 0x69: // MP3 MPEG2
                                case 0x6B: // MP3 MPEG1
                                    track->stream.codec = MPS_DEMUX_ISO_CODEC_MP3;
                                    break;
                            }

                            pos += length;
                            break;
                        case 0x06:
                            MPS_ISO_DECODE_ES_LENGTH();

                            MPS_TRACE("type %i size %i\n", type, length);

                            pos += length;
                            break;
                        default:

                            MPS_TRACE("type %i\n", type);
                            pos = bufferSize;
                    }
                }

                break;
        }
        mps_skipIsoBox(medium, &subBox);
    }
}

VOID mps_isoDecodeStbl(MPS_DEMUX_ISO_TRACK_T* track, MPS_MEDIUM_T* medium, MPS_ISO_FULL_BOX_T* box, UINT8* buffer)
{
    MPS_ISO_FULL_BOX_T            subBox;
    MPS_ISO_FULL_BOX_T            subSubBox;
    UINT32                        nbEntry;
    int                           i;
    union
    {
        UINT8*                          buffer;
        MPS_ISO_STSD_ENTRY_T*           stsdEntry;
        MPS_ISO_STSD_AUDIO_ENTRY_T*     stsdAudioEntry;
        MPS_ISO_STSD_VIDEO_ENTRY_T*     stsdVideoEntry;
        MPS_ISO_TIME_TO_SAMPLE_ENTRY_T* timeToSampleEntry;
    } data;

    data.buffer = buffer;

    MPS_ISO_CHUNK_SIZE_HEADER_T   chunkSizeHeader;

    while(mps_readNextIsoBox(medium, &subBox, box) == TRUE)
    {
        switch(subBox.id)
        {
            case ISO_ID('s', 't', 's', 'd'):
                mps_readDataInIsoBox(medium, &subBox, buffer, sizeof(MPS_ISO_STSD_ENTRY_T));
                i = HAL_ENDIAN_BIG_16(data.stsdEntry->dataReferenceIndex);
                for(; i; --i)
                {
                    mps_readNextIsoBox(medium, &subSubBox, &subBox);
                    MPS_TRACE("     Demux %c%c%c%c\n",
                              ((char*)&subSubBox.id)[0], ((char*)&subSubBox.id)[1],
                              ((char*)&subSubBox.id)[2], ((char*)&subSubBox.id)[3]);

                    switch(subSubBox.id)
                    {
                        case ISO_ID('m', 'p', '4', 'a'): // MP4
                            mps_readDataInIsoBox(medium, &subSubBox, buffer, sizeof(MPS_ISO_STSD_AUDIO_ENTRY_T));
                            track->sampleSize = HAL_ENDIAN_BIG_16(data.stsdAudioEntry->sampleSize);
                            track->sampleRate = HAL_ENDIAN_BIG_32(data.stsdAudioEntry->sampleRate) >> 16;
                            track->nbChannel  = HAL_ENDIAN_BIG_16(data.stsdAudioEntry->channelCount);
                            track->audioTrack = 1;
                            mps_isoDecodeEs(track, medium, &subSubBox, buffer);

                            MPS_TRACE("      sampleRate=%i sampleSize=%i channel=%i\n",
                                      track->sampleRate, track->sampleSize, track->nbChannel);
                            break;
                        case ISO_ID('s', 'a', 'm', 'r'): // AMR
                            mps_readDataInIsoBox(medium, &subSubBox, buffer, sizeof(MPS_ISO_STSD_AUDIO_ENTRY_T));
                            track->sampleSize   = HAL_ENDIAN_BIG_16(data.stsdAudioEntry->sampleSize);
                            track->sampleRate   = HAL_ENDIAN_BIG_32(data.stsdAudioEntry->sampleRate) >> 16;
                            track->nbChannel    = HAL_ENDIAN_BIG_16(data.stsdAudioEntry->channelCount);
                            track->audioTrack   = 1;
                            track->stream.codec = MPS_DEMUX_ISO_CODEC_AMR;

                            MPS_TRACE("      sampleRate=%i sampleSize=%i channel=%i\n",
                                      track->sampleRate, track->sampleSize, track->nbChannel);
                            break;
                        case ISO_ID('m', 'p', '4', 'v'): // MPEG4
                            mps_readDataInIsoBox(medium, &subSubBox, buffer, sizeof(MPS_ISO_STSD_VIDEO_ENTRY_T));
                            track->w          = HAL_ENDIAN_BIG_16(data.stsdVideoEntry->width);
                            track->h          = HAL_ENDIAN_BIG_16(data.stsdVideoEntry->height);
                            track->sampleSize = HAL_ENDIAN_BIG_16(data.stsdVideoEntry->depth);
                            track->videoTrack = 1;
                            mps_isoDecodeEs(track, medium, &subSubBox, buffer);

                            MPS_TRACE("      w=%i h=%i depth=%i\n",
                                      track->w, track->h, track->sampleSize);
                            break;
                        case ISO_ID('s', '2', '6', '3'): // H263
                            mps_readDataInIsoBox(medium, &subSubBox, buffer, sizeof(MPS_ISO_STSD_VIDEO_ENTRY_T));
                            track->w          = HAL_ENDIAN_BIG_16(data.stsdVideoEntry->width);
                            track->h          = HAL_ENDIAN_BIG_16(data.stsdVideoEntry->height);
                            track->sampleSize = HAL_ENDIAN_BIG_16(data.stsdVideoEntry->depth);
                            track->videoTrack = 1;
                            mps_isoDecodeEs(track, medium, &subSubBox, buffer);

                            MPS_TRACE("      w=%i h=%i depth=%i\n",
                                      track->w, track->h, track->sampleSize);
                            break;
                        default:
                            MPS_TRACE("      Unsupported demux\n"/*  %c%c%c%c\n", */
                                      /*                            ((char*)&box2.id)[0], ((char*)&box2.id)[1], */
                                      /*                            ((char*)&box2.id)[2], ((char*)&box2.id)[3] */);
                            break;
                    }
                    mps_skipIsoBox(medium, &subSubBox);
                }
                break;
            case ISO_ID('s', 't', 's', 'c'):
                mps_readIsoFullBox(medium, &subBox);
                mps_readDataInIsoBox(medium, &subBox, &nbEntry, sizeof(UINT32));
                nbEntry                          = HAL_ENDIAN_BIG_32(nbEntry);
                track->stream.chunkSampleInChunk = mps_MediumSeek(medium, 0, FS_SEEK_CUR);
                track->stream.nbNbSampleInChunk  = nbEntry;
                MPS_TRACE("     index sample in chunk %i %08x\n", nbEntry, track->stream.chunkSampleInChunk);

                break;
            case ISO_ID('s', 't', 't', 's'):
                mps_readIsoFullBox(medium, &subBox);
                mps_readDataInIsoBox(medium, &subBox, buffer, sizeof(UINT32));
                mps_readDataInIsoBox(medium, &subBox, buffer, 512);
                track->delta = HAL_ENDIAN_BIG_32(data.timeToSampleEntry->delta);

                MPS_TRACE("     count=%i delta=%i\n",
                          HAL_ENDIAN_BIG_32(data.timeToSampleEntry->count), track->delta);
                break;
            case ISO_ID('s', 't', 'c', 'o'):
                mps_readIsoFullBox(medium, &subBox);
                mps_readDataInIsoBox(medium, &subBox, &nbEntry, sizeof(UINT32));
                nbEntry                   = HAL_ENDIAN_BIG_32(nbEntry);
                track->stream.chunkOffset = mps_MediumSeek(medium, 0, FS_SEEK_CUR);
                track->stream.nbChunk     = nbEntry;
                MPS_TRACE("     index chunk %i %08x\n", nbEntry, track->stream.chunkOffset);
                mps_skipIsoBox(medium, &subBox);
                break;
            case ISO_ID('s', 't', 's', 'z'):
                mps_readIsoFullBox(medium, &subBox);
                mps_readDataInIsoBox(medium, &subBox, &chunkSizeHeader, sizeof(MPS_ISO_CHUNK_SIZE_HEADER_T));
                track->stream.chunkSize     = mps_MediumSeek(medium, 0, FS_SEEK_CUR);
                chunkSizeHeader.sampleSize  = HAL_ENDIAN_BIG_32(chunkSizeHeader.sampleSize);
                chunkSizeHeader.sampleCount = HAL_ENDIAN_BIG_32(chunkSizeHeader.sampleCount);
                track->stream.nbSampleSize  = chunkSizeHeader.sampleCount;
                if(chunkSizeHeader.sampleSize != 0)
                {
                    track->stream.bufferSampleSize[0]   = chunkSizeHeader.sampleSize;
                    track->stream.nbSampleSize          = 1;
                    track->stream.nbBufferSampleSize    = 1;
                    track->stream.posBufferSampleSize   = 0;
                    track->stream.posInBufferSampleSize = 1;
                }
                MPS_TRACE("     index size %i %08x\n", track->stream.nbSampleSize, track->stream.chunkSize);
                mps_skipIsoBox(medium, &subBox);
                break;
        }
        mps_skipIsoBox(medium, &subBox);
    }
}

PRIVATE VOID mps_isoResetStream(MPS_DEMUX_ISO_STREAM_T* stream)
{
    stream->posBufferChunk = 0;
    stream->nbBufferChunk  = 0;
}

PRIVATE UINT32 mps_isoGetNextSample(MPS_MEDIUM_T*           medium,
                                    MPS_DEMUX_ISO_STREAM_T* stream,
                                    MPS_DEMUX_ISO_SAMPLE_T* sample)
{
    int                              i;
    MPS_ISO_SAMPLES_TO_CHUNK_ENTRY_T samplesToChunk[MPS_ISO_CHUNK_BUFFER_SIZE];

    MPS_TRACE(__func__);

    if(stream->sampleRemaining == 0)
    {
        MPS_TRACE("mps_isoGetNextSample: sample remaining == 0\n");

        if(stream->posInBufferChunk >= stream->nbBufferChunk)
        {
            MPS_TRACE("fetch new chunk\n");
            // No more chunk
            if(stream->nbChunk == stream->posBufferChunk)
            {
                MPS_TRACE("mps_isoGetNextSample: No more chunk %i %i\n",
                          stream->nbChunk, stream->posBufferChunk);
                return 0;
            }
            // Fetch new chunk offset
            stream->nbBufferChunk = stream->nbChunk - stream->posBufferChunk;
            if(stream->nbBufferChunk > MPS_ISO_CHUNK_BUFFER_SIZE)
            {
                stream->nbBufferChunk = MPS_ISO_CHUNK_BUFFER_SIZE;
            }

            mps_MediumSeek(medium, stream->chunkOffset+sizeof(UINT32)*stream->posBufferChunk, FS_SEEK_SET);
            mps_MediumRead(medium, stream->bufferChunk, sizeof(UINT32)*stream->nbBufferChunk);
            for(i = 0; i < stream->nbBufferChunk; ++i)
            {
                stream->bufferChunk[i] = HAL_ENDIAN_BIG_32(stream->bufferChunk[i]);
            }

            stream->posBufferChunk   += stream->nbBufferChunk;
            stream->posInBufferChunk  = 0;
        }

        if(stream->bufferNbSampleInChunk[stream->nbNbSampleInChunk].firstChunk <=
                stream->posBufferNbSampleInChunk - stream->nbBufferNbSampleInChunk + stream->posInBufferNbSampleInChunk)
        {
            stream->posInBufferNbSampleInChunk++;
        }

        if(stream->posInBufferNbSampleInChunk >= stream->nbNbSampleInChunk-1 &&
                stream->nbNbSampleInChunk          != stream->posBufferNbSampleInChunk)
        {
            MPS_TRACE("fetch new samples in chunk\n");

            // Fetch
            stream->nbBufferNbSampleInChunk = stream->nbNbSampleInChunk - stream->posBufferNbSampleInChunk;
            if(stream->nbBufferNbSampleInChunk > MPS_ISO_CHUNK_BUFFER_SIZE)
            {
                stream->nbBufferNbSampleInChunk = MPS_ISO_CHUNK_BUFFER_SIZE;
            }

            mps_MediumSeek(medium, stream->chunkSampleInChunk+
                           sizeof(MPS_ISO_SAMPLES_TO_CHUNK_ENTRY_T)*stream->posBufferNbSampleInChunk,
                           FS_SEEK_SET);
            mps_MediumRead(medium, samplesToChunk, sizeof(MPS_DEMUX_ISO_SAMPLES_TO_CHUNK_T)*stream->nbBufferChunk);
            for(i = 0; i < stream->nbBufferChunk; ++i)
            {
                stream->bufferNbSampleInChunk[i].firstChunk      = HAL_ENDIAN_BIG_32(samplesToChunk[i].firstChunk);
                stream->bufferNbSampleInChunk[i].samplesPerChunk = HAL_ENDIAN_BIG_32(samplesToChunk[i].samplesPerChunk);
            }
            stream->posBufferNbSampleInChunk   += stream->nbBufferNbSampleInChunk-1;
            stream->posInBufferNbSampleInChunk  = 0;
        }

        stream->sampleRemaining = stream->bufferNbSampleInChunk[stream->posInBufferNbSampleInChunk].samplesPerChunk;

        MPS_TRACE("mps_isoGetNextSample: nbSample %i\n", stream->sampleRemaining);

        stream->posInChunk        = 0;
        ++stream->posInBufferChunk;
    }

    if(stream->nbSampleSize          != 1 &&
            stream->posInBufferSampleSize >= stream->nbBufferSampleSize)
    {
        MPS_TRACE("fetch new size\n");

        // No more chunk
        if(stream->nbSampleSize == stream->posBufferSampleSize)
        {
            MPS_TRACE("mps_isoGetNextSample: No more chunk size %i %i\n",
                      stream->nbSampleSize, stream->posBufferSampleSize);

            return 0;
        }
        // Fetch new chunk size
        stream->nbBufferSampleSize = stream->nbSampleSize - stream->posBufferSampleSize;
        if(stream->nbBufferSampleSize > MPS_ISO_CHUNK_BUFFER_SIZE)
        {
            stream->nbBufferSampleSize = MPS_ISO_CHUNK_BUFFER_SIZE;
        }

        mps_MediumSeek(medium, stream->chunkSize+sizeof(UINT32)*stream->posBufferSampleSize, FS_SEEK_SET);
        mps_MediumRead(medium, stream->bufferSampleSize, sizeof(UINT32)*stream->nbBufferSampleSize);
        MPS_TRACE("size nb = %i\n", stream->nbBufferSampleSize);
        for(i = 0; i < stream->nbBufferSampleSize; ++i)
        {
            stream->bufferSampleSize[i] = HAL_ENDIAN_BIG_32(stream->bufferSampleSize[i]);
        }

        stream->posBufferSampleSize   += stream->nbBufferSampleSize;
        stream->posInBufferSampleSize  = 0;
    }

    sample->offset  = stream->bufferChunk     [stream->posInBufferChunk     -1];
    sample->offset += stream->posInChunk;
    sample->size    = stream->bufferSampleSize[0];
    if(stream->nbSampleSize != 1)
    {
        MPS_TRACE("size offset = %i\n", stream->posInBufferSampleSize);
        sample->size = stream->bufferSampleSize[stream->posInBufferSampleSize];
        stream->posInBufferSampleSize++;
    }

    stream->posInChunk  += sample->size;
    /*     stream->posInSample  = 0; */
    --stream->sampleRemaining;

    MPS_TRACE("mps_isoGetNextSample: sample offset 0x%08x size %i\n",
              sample->offset, sample->size);

    return 1;
}

PRIVATE UINT32 mps_isoGetCurrentSample(MPS_MEDIUM_T*           medium,
                                       MPS_DEMUX_ISO_STREAM_T* stream,
                                       MPS_DEMUX_ISO_SAMPLE_T* sample)
{

    if(stream->nbChunk == stream->posBufferChunk && stream->sampleRemaining == 0)
    {
        return 0;
    }

    if(stream->nbBufferChunk == 0)
    {
        return mps_isoGetNextSample(medium, stream, sample);
    }

    sample->offset  = stream->bufferChunk     [stream->posInBufferChunk     -1];
    sample->offset += stream->posInChunk;
    sample->size    = stream->bufferSampleSize[0];
    if(stream->nbSampleSize != 1)
    {
        sample->size    = stream->bufferSampleSize[stream->posInBufferSampleSize-1];
    }
    sample->offset -= sample->size;

    return 1;
}

/// @todo remove global vars

// 44100
/*     UINT8                  header[] = */
/*         { 0xFF, 0xF1, 0x10, 0x80, 0x00, 0x1F, 0xFC }; */

// 24000
UINT8                  header[] =
{ 0xFF, 0xF1, 0x18, 0x80, 0x00, 0x1F, 0xFC };

PRIVATE UINT32 mps_isoGetDataChunk(MPS_MEDIUM_T* medium, MPS_DEMUX_ISO_STREAM_T* stream,
                                   UINT8* data, UINT32 length)
{
    MPS_DEMUX_ISO_SAMPLE_T  sample;
    UINT32                  read = 0;
    UINT32                  readBytes;

    MPS_TRACE(__func__);

    header[1] |= (stream->isMpeg4) ? 0x00 : 0x08;
    switch(stream->codec)
    {
        case MPS_DEMUX_ISO_CODEC_AAC_MAIN:
            header[2] |= 0x00;
            break;
        case MPS_DEMUX_ISO_CODEC_AAC_LC:
            header[2] |= 0x40;
            break;
        case MPS_DEMUX_ISO_CODEC_AAC_HE:
            header[2] |= 0x80;
            break;
        default:
            break;
    }

    if(mps_isoGetCurrentSample(medium, stream, &sample) == 0)
    {
        MPS_TRACE("Current data failed\n");

        return 0;
    }

    while(read < length)
    {
        MPS_TRACE("Sample 0x%08x Size %i\n", sample.offset, sample.size);

        if(stream->codec == MPS_DEMUX_ISO_CODEC_AAC_HE ||
                stream->codec == MPS_DEMUX_ISO_CODEC_AAC_LC ||
                stream->codec == MPS_DEMUX_ISO_CODEC_AAC_MAIN)
        {
            if(stream->posInSample < 0)
            {
                readBytes = -stream->posInSample;
                if(readBytes > length - read)
                {
                    readBytes = length - read;
                }
                memcpy(&data[read], &header[stream->posInSample+sizeof(header)], readBytes);
                read                += readBytes;
                stream->posInSample += readBytes;
            }
        }

        if(stream->posInSample < 0)
        {
            break;
        }

        readBytes = sample.size - stream->posInSample;
        if(readBytes > length - read)
        {
            readBytes = length - read;
        }

        mps_MediumSeek(medium, sample.offset+stream->posInSample, FS_SEEK_SET);
        mps_MediumRead(medium, &data[read], readBytes);

        read                += readBytes;
        stream->posInSample += readBytes;

        if(stream->posInSample == sample.size)
        {
            stream->posInSample = 0;
            if(mps_isoGetNextSample(medium, stream, &sample) == 0)
            {
                break;
            }
            if(stream->codec == MPS_DEMUX_ISO_CODEC_AAC_HE ||
                    stream->codec == MPS_DEMUX_ISO_CODEC_AAC_LC ||
                    stream->codec == MPS_DEMUX_ISO_CODEC_AAC_MAIN)
            {
                stream->posInSample = -sizeof(header);
            }
        }
    }

    return read;
}

PRIVATE VOID mps_isoDecodeMinf(MPS_DEMUX_ISO_DATA_T* data, MPS_DEMUX_ISO_TRACK_T* track, MPS_MEDIUM_T* medium,
                               MPS_ISO_FULL_BOX_T* box, UINT8* buffer)
{
    MPS_ISO_FULL_BOX_T   subBox;

    while(mps_readNextIsoBox(medium, &subBox, box) == TRUE)
    {
        switch(subBox.id)
        {
            case ISO_ID('s', 't', 'b', 'l'):
                mps_isoDecodeStbl(track, medium, &subBox, buffer);
                break;
        }
        mps_skipIsoBox(medium, &subBox);
    }
}

PRIVATE VOID mps_isoDecodeMdia(MPS_DEMUX_ISO_DATA_T* data, MPS_DEMUX_ISO_TRACK_T* track,
                               MPS_MEDIUM_T* medium, MPS_ISO_FULL_BOX_T* box, UINT8* buffer)
{
    MPS_ISO_FULL_BOX_T   subBox;

    while(mps_readNextIsoBox(medium, &subBox, box) == TRUE)
    {
        switch(subBox.id)
        {
            case ISO_ID('m', 'd', 'h', 'd'):
                mps_readIsoFullBox(medium, &subBox);
                mps_readDataInIsoBox(medium, &subBox, buffer, 512);
                MPS_TRACE("   timescale=%i\n",
                          HAL_ENDIAN_BIG_32(((MPS_ISO_MEDIA_HEADER_T*)buffer)->timescale));
                track->timeScale = HAL_ENDIAN_BIG_32(((MPS_ISO_MEDIA_HEADER_T*)buffer)->timescale);
                break;
            case ISO_ID('m', 'i', 'n', 'f'):
                mps_isoDecodeMinf(data, track, medium, &subBox, buffer);
                break;
        }
        mps_skipIsoBox(medium, &subBox);
    }
}

PRIVATE VOID mps_isoDecodeTrak(MPS_HANDLE_T handle, MPS_DEMUX_ISO_DATA_T* data, MPS_ISO_FULL_BOX_T* box, UINT8* buffer)
{
    MPS_ISO_FULL_BOX_T     subBox;
    MPS_DEMUX_ISO_TRACK_T  track;
    MPS_ISO_TRAK_HEADER_T* trackHeader;

    MPS_TRACE(__func__);

    trackHeader = (MPS_ISO_TRAK_HEADER_T*) buffer;

    memset(&track, 0, sizeof(track));

    while(mps_readNextIsoBox(&handle->mediumAudio, &subBox, box) == TRUE)
    {
        switch(subBox.id)
        {
            case ISO_ID('t', 'k', 'h', 'd'):
                mps_readIsoFullBox(&handle->mediumAudio, &subBox);
                mps_readDataInIsoBox(&handle->mediumAudio, &subBox, buffer, 512);
                track.duration  = HAL_ENDIAN_BIG_32(trackHeader->duration);
                track.stream.id = HAL_ENDIAN_BIG_32(trackHeader->trackId);

                MPS_TRACE("  trackID %i w=%i h=%i duration=%i\n",
                          track.stream.id,
                          HAL_ENDIAN_BIG_32(trackHeader->width )>>16,
                          HAL_ENDIAN_BIG_32(trackHeader->height)>>16,
                          track.duration);
                break;
            case ISO_ID('m', 'd', 'i', 'a'):
                mps_isoDecodeMdia(data, &track, &handle->mediumAudio, &subBox, buffer);
                break;
        }
        mps_skipIsoBox(&handle->mediumAudio, &subBox);
    }

    if(track.audioTrack)
    {
        MPS_TRACE("Track %i (Audio) summary :\n"
                  "| chunkSize 0x%08x chunkOffset 0x%08x  duration %i\n"
                  "| sampleRate %i nbChannel %i sampleSize %i\n",
                  track.stream.id, track.stream.chunkSize, track.stream.chunkOffset,
                  track.duration,
                  track.sampleRate, track.nbChannel, track.sampleSize);

        /*         if(track.sampleSize != 16) */
        /*         { */
        /*             track.stream.codec = MPS_DEMUX_ISO_CODEC_UNKNOW; */
        /*         } */

        switch(track.stream.codec)
        {
            case MPS_DEMUX_ISO_CODEC_AAC_MAIN:
                MPS_TRACE("| codec AAC\n");
                handle->stream.audioMode         = AVPS_PLAY_AUDIO_MODE_AAC;
                break;
            case MPS_DEMUX_ISO_CODEC_AAC_LC:
                MPS_TRACE("| codec AAC-LC\n");
                handle->stream.audioMode         = AVPS_PLAY_AUDIO_MODE_AAC;
                break;
            case MPS_DEMUX_ISO_CODEC_AAC_HE:
                MPS_TRACE("| codec AAC-HE\n");
                handle->stream.audioMode         = AVPS_PLAY_AUDIO_MODE_AAC;
                break;
            case MPS_DEMUX_ISO_CODEC_MP3:
                MPS_TRACE("| codec MP3\n");
                handle->stream.audioMode         = AVPS_PLAY_AUDIO_MODE_MP3;
                break;
            case MPS_DEMUX_ISO_CODEC_AMR:
                MPS_TRACE("| codec AMR\n");
                handle->stream.audioMode         = AVPS_PLAY_AUDIO_MODE_AMR_RING;
                break;
            default:
                MPS_TRACE("| codec ??\n");
                break;
        }

        if(track.stream.codec != MPS_DEMUX_ISO_CODEC_UNKNOW)
        {
            handle->stream.audioSampleRate   = track.sampleRate;
            handle->stream.channelNb         = track.nbChannel;
            handle->audioLength              = 1024*8;
            handle->audioBuffer              = (UINT8*) sxr_Malloc(handle->audioLength);
            handle->videoLength              = 1024*16*2;
            handle->videoBuffer              = (UINT8*) sxr_Malloc(handle->videoLength);
        }

        memcpy(&data->streamAudio, &track.stream, sizeof(MPS_DEMUX_ISO_STREAM_T));
    }
    if(track.videoTrack)
    {
        MPS_TRACE("Track %i (Video) summary :\n"
                  "| chunkSize 0x%08x chunkOffset 0x%08x duration %i\n"
                  "| size %ix%i depth %i FPS %02.02f\n",
                  track.stream.id, track.stream.chunkSize, track.stream.chunkOffset,
                  track.duration,
                  track.w, track.h, track.sampleSize, /* (float)  */track.timeScale/track.delta);

        handle->info.height    = track.h;
        handle->info.width     = track.w;

        switch(track.stream.codec)
        {
            case MPS_DEMUX_ISO_CODEC_MPEG4:
                handle->stream.videoMode         = AVPS_PLAY_VIDEO_MODE_MPEG4;
                break;
            case MPS_DEMUX_ISO_CODEC_H263:
                handle->stream.videoMode         = AVPS_PLAY_VIDEO_MODE_H263;
                break;
            default:
                MPS_TRACE("| codec ??\n");
                break;
        }


        memcpy(&data->streamVideo, &track.stream, sizeof(MPS_DEMUX_ISO_STREAM_T));
    }

}

UINT8                buffer[512];

PRIVATE VOID mps_isoDecodeMoov(MPS_HANDLE_T handle, MPS_DEMUX_ISO_DATA_T* data, MPS_ISO_FULL_BOX_T* box)
{
    MPS_ISO_FULL_BOX_T   subBox;

    MPS_TRACE(__func__);

    while(mps_readNextIsoBox(&handle->mediumAudio, &subBox, box) == TRUE)
    {
        switch(subBox.id)
        {
            case ISO_ID('m', 'v', 'h', 'd'):
                mps_readIsoFullBox(&handle->mediumAudio, &subBox);
                mps_readDataInIsoBox(&handle->mediumAudio, &subBox, buffer, sizeof(buffer));
                /// @todo check box box version
                MPS_TRACE(" timescale %i duration %i\n",
                          HAL_ENDIAN_BIG_32(((MPS_ISO_MOVIE_HEADER_T*)&buffer)->timescale),
                          HAL_ENDIAN_BIG_32(((MPS_ISO_MOVIE_HEADER_T*)&buffer)->duration));
                break;
            case ISO_ID('t', 'r', 'a', 'k'):
                mps_isoDecodeTrak(handle, data, &subBox, buffer);
                break;
        }
        mps_skipIsoBox(&handle->mediumAudio, &subBox);
    }
}

PRIVATE UINT32 mps_getVideoDataIso(MPS_HANDLE_T handle, UINT8* data, UINT32 size)
{
    MPS_DEMUX_ISO_DATA_T*  demuxData;
    MPS_DEMUX_ISO_SAMPLE_T sample;

    MPS_TRACE(__func__);

    demuxData = (MPS_DEMUX_ISO_DATA_T*) handle->demuxData;

    if(mps_isoGetNextSample(&handle->mediumVideo, &demuxData->streamVideo, &sample) == 0)
    {
        return 0;
    }

    mps_MediumSeek(&handle->mediumVideo, sample.offset, FS_SEEK_SET);
    return mps_MediumRead(&handle->mediumVideo, data, sample.size);
}

PRIVATE VOID mps_skipVideoIso(MPS_HANDLE_T handle)
{
    MPS_DEMUX_ISO_DATA_T*  demuxData;
    MPS_DEMUX_ISO_SAMPLE_T sample;

    MPS_TRACE(__func__);

    demuxData = (MPS_DEMUX_ISO_DATA_T*) handle->demuxData;

    mps_isoGetNextSample(&handle->mediumVideo, &demuxData->streamVideo, &sample);
}

PRIVATE UINT32 mps_getAudioDataIso(MPS_HANDLE_T handle, UINT8* data, UINT32 size)
{
    MPS_DEMUX_ISO_DATA_T*  demuxData;

    MPS_TRACE(__func__);

    demuxData = (MPS_DEMUX_ISO_DATA_T*) handle->demuxData;

    return mps_isoGetDataChunk(&handle->mediumAudio, &demuxData->streamAudio, data, size);
}

PRIVATE VOID* mps_openIso(MPS_HANDLE_T handle)
{
    MPS_ISO_FULL_BOX_T              box;
    MPS_DEMUX_ISO_DATA_T*           data;

    MPS_TRACE(__func__);

    data = sxr_Malloc(sizeof(MPS_DEMUX_ISO_DATA_T));

    while(mps_readNextIsoBox(&handle->mediumAudio, &box, 0) == TRUE)
    {
        switch(box.id)
        {
            case ISO_ID('m', 'd', 'a', 't'):
            case ISO_ID('f', 't', 'y', 'p'):
                break;
            case ISO_ID('m', 'o', 'o', 'v'):
                mps_isoDecodeMoov(handle, data, &box);
                break;
        }
        mps_skipIsoBox(&handle->mediumAudio, &box);
    }
    if(data->streamAudio.codec == MPS_DEMUX_ISO_CODEC_UNKNOW)
    {
        sxr_Free(data);
        return 0;
    }
    data->streamAudio.posInSample = -7;

    handle->stream.audioVideoRatio = 1 << 8;

    return (VOID*)data;
}

PRIVATE VOID mps_closeIso(MPS_HANDLE_T handle)
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

PRIVATE BOOL mps_playIso(MPS_HANDLE_T handle, AVPS_CFG_T* avpsCfg)
{
    MPS_DEMUX_ISO_DATA_T*      data;
    UINT32                     audioWriteBuffer;
    UINT32                     size;
    LCDD_FBW_T                 frameWindows;

    data = (MPS_DEMUX_ISO_DATA_T*) handle->demuxData;

    if(data->streamAudio.nbChunk == data->streamAudio.posBufferChunk)
    {
        mps_isoResetStream(&data->streamAudio);
        mps_isoResetStream(&data->streamVideo);
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
            size = mps_getAudioDataIso(handle, (UINT8*)audioWriteBuffer, size);
            handle->bufferContext.audioRemainingSize += size;
        }
    }
    if(handle->bufferContext.audioRemainingSize != handle->audioLength)
    {
        size = mps_getAudioDataIso(handle, handle->audioBuffer,
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

    if(avps_Play(handle->config.audioPath, &handle->stream, avpsCfg, &frameWindows,
                 &handle->bufferContext) == AVPS_ERR_NO)
    {
        return TRUE;
    }
    return FALSE;
}

