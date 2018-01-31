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

#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif

#ifdef MP4_3GP_SUPPORT

#ifndef _GF_ISOMEDIA_DEV_H_
#define _GF_ISOMEDIA_DEV_H_

#ifndef SHEEN_VC_DEBUG
#include "cs_types.h"
#include "fs.h"
#include "string.h"
#include "mmc.h"
#endif

#include"mdia_tools.h"
#include "list.h"
#include "tools.h"
#include "mpeg4_odf.h"
#include "bitstream.h"


/*used to classify boxes in the UserData GF_Box*/

#define GF_ISOM_BOX     \
uint32 type;        \
uint8 uuid[16];   \
u64  size;    \

#define GF_ISOM_FULL_BOX \
  GF_ISOM_BOX  \
  uint8 version;  \
  uint32 flags ; \

typedef struct
{
    GF_ISOM_BOX
    GF_List *recordList;
} GF_UserDataBox;

#define GF_ISOM_DATAENTRY_FIELDS    \
    int8 *location;

typedef struct
{
    GF_ISOM_FULL_BOX
    GF_ISOM_DATAENTRY_FIELDS
} GF_DataEntryBox;


typedef struct __tag_data_map
{
    uint8   type;
    u64 curPos;
    uint8   mode;
    GF_BitStream *bs;
} GF_DataMap;

typedef struct
{
    uint8   type;
    u64 curPos;
    uint8   mode;
    GF_BitStream *bs;
    HANDLE *stream;
    boolean last_acces_was_read;
} GF_FileDataMap;


typedef struct
{
    uint32 boxType;
    uint8 uuid[16];
    GF_List *boxList;
} GF_UserDataMap;



typedef struct
{
    GF_ISOM_BOX
    uint16 dataReferenceIndex;
    int8 reserved[ 6 ];
    uint16 version;
    uint16 revision;
    uint32 vendor;
    uint32 temporal_quality;
    uint32 spacial_quality;
    uint16 Width, Height;
    uint32 horiz_res, vert_res;
    uint32 entry_data_size;
    uint16 frames_per_sample;
    uint8 compressor_name[32];
    uint16 bit_depth;
    s16 color_table_index;
} GF_VisualSampleEntryBox;


typedef struct
{
    GF_ISOM_BOX
} GF_Box;

typedef struct
{
    GF_ISOM_FULL_BOX
} GF_FullBox;

#define BASE_DESCRIPTOR \
        uint8 tag;

typedef struct
{
    BASE_DESCRIPTOR
} GF_Descriptor;

typedef struct
{
    BASE_DESCRIPTOR
    uint32 dataLength;
    int8 *data;
} GF_DefaultDescriptor;

typedef struct
{
    BASE_DESCRIPTOR
    uint8 objectTypeIndication;
    uint8 streamType;
    uint8 upstream;
    uint32 bufferSizeDB;
    uint32 maxBitrate;
    uint32 avgBitrate;
    GF_DefaultDescriptor *decoderSpecificInfo;
    //GF_List *profileLevelIndicationIndexDescriptor;
} GF_DecoderConfig;

/*the Sync Layer config descriptor*/
typedef struct
{
    BASE_DESCRIPTOR
    uint8 predefined;
    uint8 useAccessUnitStartFlag;
    uint8 useAccessUnitEndFlag;
    uint8 useRandomAccessPointFlag;
    uint8 hasRandomAccessUnitsOnlyFlag;
    uint8 usePaddingFlag;
    uint8 useTimestampsFlag;
    uint8 useIdleFlag;
    uint8 durationFlag;
    uint32 timestampResolution;
    uint32 OCRResolution;
    uint8 timestampLength;
    uint8 OCRLength;
    uint8 AULength;
    uint8 instantBitrateLength;
    uint8 degradationPriorityLength;
    uint8 AUSeqNumLength;
    uint8 packetSeqNumLength;
    uint32 timeScale;
    uint16 AUDuration;
    uint16 CUDuration;
    u64 startDTS;
    u64 startCTS;
} GF_SLConfig;

typedef struct
{
    uint8 tag;
    uint16 ESID;
    uint8 streamPriority;
    GF_DecoderConfig *decoderConfig;
    GF_SLConfig *slConfig;
} GF_ESD;

typedef struct
{
    GF_ISOM_FULL_BOX
    GF_ESD *desc;
} GF_ESDBox;


typedef struct
{
    GF_ISOM_BOX
    uint16 dataReferenceIndex;
    int8 reserved[ 6 ];
    uint16 version;
    uint16 revision;
    uint32 vendor;
    uint32 temporal_quality;
    uint32 spacial_quality;
    uint16 Width, Height;
    uint32 horiz_res, vert_res;
    uint32 entry_data_size;
    uint16 frames_per_sample;
    uint8 compressor_name[32];
    uint16 bit_depth;
    s16 color_table_index;
    GF_ESDBox *esd;
    GF_SLConfig *slc;
} GF_MPEGVisualSampleEntryBox;

typedef struct
{
    GF_ISOM_BOX
    uint16 dataReferenceIndex;
    int8 reserved[ 6 ];
    uint16 version;
    uint16 revision;
    uint32 vendor;
    uint16 channel_count;
    uint16 bitspersample;
    uint16 compression_id;
    uint16 packet_size;
    uint16 samplerate_hi;
    uint16 samplerate_lo;
    GF_ESDBox *esd;
    GF_SLConfig *slc;
} GF_MPEGAudioSampleEntryBox;

//add shenh

typedef struct
{
    GF_ISOM_BOX
    uint8 configurationVersion;
    uint8 AVCProfileIndication;
    uint8 profile_compatibility;
    uint8 AVCLevelIndication;
    uint8 lengthSizeMinusOne; //high bit(6) reserved=111111, use low bit(2);
    uint8 numOfSequenceParameterSets;//high bit(3) reserved=111, use low bit(5)
    GF_DefaultDescriptor *sps;//N sequence Parameter Set
    uint8 numOfPictureParameterSets;
    GF_DefaultDescriptor *pps; //N picture Parameter Set
} GF_AVCConfig;

typedef struct
{
    GF_ISOM_BOX
    uint16 dataReferenceIndex;
    int8 reserved[ 6 ];
    uint16 version;
    uint16 revision;
    uint32 vendor;
    uint32 temporal_quality;
    uint32 spacial_quality;
    uint16 Width, Height;
    uint32 horiz_res, vert_res;
    uint32 entry_data_size;
    uint16 frames_per_sample;
    uint8 compressor_name[32];
    uint16 bit_depth;
    s16 color_table_index;
    GF_AVCConfig *avcC;
} GF_AVCVisualSampleEntryBox;
typedef struct
{
    GF_ISOM_FULL_BOX
    uint32 vendor;
    //uint32 avg_bitrate;
    // uint32 max_bitrate;
    uint8  decoder_version;
    uint8  level;
    uint8  profile;
} h263DecSpecStrc;

typedef struct
{
    GF_ISOM_FULL_BOX
    uint32 vendor;
    uint8  decoder_version;
    uint16  mode_set;
    uint8  mode_change_period;
    uint8   frames_per_sample;
} amrDecSpecStrc;

typedef struct
{
    GF_ISOM_BOX
    uint16 dataReferenceIndex;
    int8 reserved[ 6 ];
    uint16 version;
    uint16 revision;
    uint32 vendor;
    uint32 temporal_quality;
    uint32 spacial_quality;
    uint16 Width, Height;
    uint32 horiz_res, vert_res;
    uint32 entry_data_size;
    uint16 frames_per_sample;
    uint8 compressor_name[32];
    uint16 bit_depth;
    s16 color_table_index;
    h263DecSpecStrc *h263Sp;
    GF_SLConfig *slc;
} GF_3gpH263SampleEntryBox;

typedef struct
{
    GF_ISOM_BOX
    uint16 dataReferenceIndex;
    int8 reserved[ 6 ];
    uint16 version;
    uint16 revision;
    uint32 vendor;
    uint32 temporal_quality;
    uint32 spacial_quality;
    uint16 Width, Height;
    uint32 horiz_res, vert_res;
    uint32 entry_data_size;
    uint16 frames_per_sample;
    uint8 compressor_name[32];
    uint16 bit_depth;
    s16 color_table_index;
} GF_3gpJpegSampleEntryBox;

typedef struct
{
    GF_ISOM_BOX
    uint16 dataReferenceIndex;
    int8 reserved[ 6 ];
    uint16 version;
    uint16 revision;
    uint32 vendor;
    uint16 channel_count;
    uint16 bitspersample;
    uint16 compression_id;
    uint16 packet_size;
    uint16 samplerate_hi;
    uint16 samplerate_lo;
    amrDecSpecStrc *amrSp;
    GF_SLConfig *slc;
} GF_3gpAmrSampleEntryBox;
//

typedef struct
{
    GF_ISOM_BOX
    uint16 dataReferenceIndex;
    int8 reserved[ 6 ];
    uint16 version;
    uint16 revision;
    uint32 vendor;
    uint16 channel_count;
    uint16 bitspersample;
    uint16 compression_id;
    uint16 packet_size;
    uint16 samplerate_hi;
    uint16 samplerate_lo;
} GF_AudioSampleEntryBox;


typedef struct
{
    GF_ISOM_BOX
    GF_ESDBox *esd;
    /*used for hinting when extracting the OD stream...*/
    GF_SLConfig *slc;
} GF_MPEGSampleEntryBox;


typedef struct
{
    GF_ISOM_FULL_BOX
    u64 creationTime;
    u64 modificationTime;
    uint32 timeScale;
    u64 duration;
    uint32 nextTrackID;
    uint32 preferredRate;
    uint16 preferredVolume;
    int8 reserved[10];
    uint32 matrixA;
    uint32 matrixB;
    uint32 matrixU;
    uint32 matrixC;
    uint32 matrixD;
    uint32 matrixV;
    uint32 matrixW;
    uint32 matrixX;
    uint32 matrixY;
    uint32 previewTime;
    uint32 previewDuration;
    uint32 posterTime;
    uint32 selectionTime;
    uint32 selectionDuration;
    uint32 currentTime;
} GF_MovieHeaderBox;

typedef struct __tag_isom GF_ISOFile;

typedef struct
{
    GF_ISOM_BOX
    GF_MovieHeaderBox *mvhd;
    //GF_ObjectDescriptorBox *iods;
    //GF_UserDataBox *udta;

    /*meta box if any*/
    //struct __tag_meta_box *meta;
    /*track boxes*/
    GF_List *trackList;
    GF_ISOFile *mov;
} GF_MovieBox;

typedef struct
{

    GF_ISOM_FULL_BOX
    uint32 *rates;
    uint32 *times;
    uint32 count;
} GF_ProgressiveDownloadBox;

typedef struct
{
    GF_ISOM_BOX
    /*note: the data is NEVER loaded to the mdat in this lib*/
    u64 dataSize;
    int8 *data;
} GF_MediaDataBox;

typedef struct
{

    GF_ISOM_BOX
    uint32 majorBrand;
    uint32 minorVersion;
    uint32 altCount;
    uint32 *altBrand;
} GF_FileTypeBox;

struct __tag_isom
{
    GF_Err LastError;
    int8 *fileName;
    int8 *finalName;
    GF_DataMap *movieFileMap;
    GF_FileTypeBox *brand;
    GF_MovieBox *moov;
    GF_MediaDataBox *mdat;
    GF_ProgressiveDownloadBox *pdin;
    GF_List *TopBoxes;
};

typedef struct
{
    GF_ISOM_FULL_BOX
    u64 creationTime;
    u64 modificationTime;
    uint32 trackID;
    uint32 reserved1;
    u64 duration;
    uint32 reserved2[2];
    uint16 layer;
    uint16 alternate_group;
    uint16 volume;
    uint16 reserved3;
    uint32 matrix[9];
    uint32 width, height;
} GF_TrackHeaderBox;

typedef struct
{
    GF_ISOM_BOX
    GF_TrackHeaderBox *Header;
    struct __tag_media_box *Media;
    GF_MovieBox *moov;
} GF_TrackBox;

typedef struct
{
    GF_ISOM_FULL_BOX
    u64 creationTime;
    u64 modificationTime;
    uint32 timeScale;
    u64 duration;
    int8 packedLanguage[4];
    uint16 reserved;
} GF_MediaHeaderBox;

typedef struct
{
    GF_ISOM_FULL_BOX
    uint32 reserved1;
    uint32 handlerType;
    uint8 reserved2[12];
    uint32 nameLength;
    int8 *nameUTF8;
} GF_HandlerBox;

typedef struct
{
    GF_ISOM_FULL_BOX
    GF_List *boxList;
} GF_DataReferenceBox;

typedef struct
{
    GF_ISOM_BOX
    GF_DataReferenceBox *dref;
} GF_DataInformationBox;

typedef struct
{
    GF_ISOM_BOX
    uint16 dataReferenceIndex;
    int8 reserved[ 6 ];
} GF_SampleEntryBox;


typedef struct
{
    GF_ISOM_FULL_BOX
    GF_List *entryList;
    uint32 r_FirstSampleInEntry;
    uint32 r_currentEntryIndex;
    uint32 r_CurrentDTS;

} GF_TimeToSampleBox;

typedef struct
{
    GF_ISOM_FULL_BOX
    GF_List *boxList;
} GF_SampleDescriptionBox;

#define INDEX_BUF_SIZE 512//2048 //every one 4bytes, total 4 buffer=2000*4*4 bytes. add shenh

typedef struct
{
    GF_ISOM_FULL_BOX
    /*if this is the compact version, sample size is actually fieldSize*/
    uint32 sampleSize;
    uint32 sampleCount;
    uint32 alloc_size;
    uint32 *sizes;

    //shenh add for index read
    uint32 offsetInFile;
    uint32 countInBuf;
    uint32 firstRealInBuf;
    //
} GF_SampleSizeBox;

typedef struct
{
    GF_ISOM_FULL_BOX
    uint32 entryCount;
    uint32 alloc_size;
    uint32 *offsets;

    //shenh add for index read
    uint32 offsetInFile;
    uint32 countInBuf;
    uint32 firstRealInBuf;
    //
} GF_ChunkOffsetBox;

typedef struct
{
    GF_ISOM_FULL_BOX
    uint32 entryCount;
    uint32 alloc_size;
    u64 *offsets;
} GF_ChunkLargeOffsetBox;

typedef struct
{
    uint32 firstChunk;
    uint32 nextChunk;
    uint32 samplesPerChunk;
    uint32 sampleDescriptionIndex;
} GF_StscEntry;

typedef struct
{
    uint32 sampleCount;
    uint32 sampleDelta;
} GF_SttsEntry;

typedef struct
{
    GF_ISOM_FULL_BOX
    GF_List *entryList;
    GF_StscEntry *currentEntry;
    uint32 currentIndex;
    /*first sample number in this chunk*/
    uint32 firstSampleInCurrentChunk;
    uint32 currentChunk;
    uint32 ghostNumber;
} GF_SampleToChunkBox;

typedef struct
{
    GF_ISOM_FULL_BOX
    uint32 entryCount;
    uint32 *sampleNumbers;
    /*cache for READ mode (in write we realloc no matter what)*/
    uint32 r_LastSyncSample;
    /*0-based index in the array*/
    uint32 r_LastSampleIndex;
} GF_SyncSampleBox;


typedef struct
{
    GF_ISOM_FULL_BOX
    u64 reserved;
} GF_VideoMediaHeaderBox;

typedef struct
{
    GF_ISOM_FULL_BOX
    uint32 entryCount;
    uint32 *offset;
} GF_ChunkOffset ;

typedef struct
{
    GF_ISOM_FULL_BOX
    uint32 reserved;
} GF_SoundMediaHeaderBox;

typedef struct
{
    GF_ISOM_BOX
    GF_TimeToSampleBox *TimeToSample;
    //GF_CompositionOffsetBox *CompositionOffset;
    GF_SyncSampleBox *SyncSample;
    GF_ChunkOffset *ChunkOffset;
    GF_SampleDescriptionBox *SampleDescription;
    GF_SampleSizeBox *SampleSize;
    GF_SampleToChunkBox *SampleToChunk;
} GF_SampleTableBox;

typedef struct __tag_media_info_box
{
    GF_ISOM_BOX
    GF_DataInformationBox       *dataInformation;
    GF_SampleTableBox *sampleTable;
    GF_Box *InfoHeader;
    struct __tag_data_map *dataHandler;
    uint32 dataEntryIndex;

//add shenh
    unsigned int lastSampleNumber;
    unsigned int lastSampleSize;
    unsigned int lastFirstSample;
    unsigned int lastOffset;

} GF_MediaInformationBox;

typedef struct __tag_media_box
{
    GF_ISOM_BOX
    GF_TrackBox *mediaTrack;
    GF_MediaHeaderBox *mediaHeader;
    GF_HandlerBox *handler;
    struct __tag_media_info_box *information;
    u64 BytesMissing;
} GF_MediaBox;

typedef struct
{
    GF_ISOM_BOX
    int8 *data;
    uint32 dataSize;
} GF_UnknownBox;

typedef struct
{
    GF_ISOM_FULL_BOX
    int8 *location;
} GF_DataEntryURLBox;

typedef struct
{
    GF_ISOM_FULL_BOX
    int8 *location;
    int8 *nameURN;
} GF_DataEntryURNBox;

typedef struct
{
    /*data size*/
    uint32 dataLength;
    /*data with padding if requested*/
    int8 *data;
    /*decoding time*/
    uint32 DTS;
    /*relative offset for composition if needed*/
    uint32 CTS_Offset;
    /*Random Access Point flag - 1 is regular RAP (read/write) , 2 is SyncShadow (read mode only)*/
    uint8 IsRAP;
} GF_ISOSample;

typedef struct __track_exporter
{
    GF_ISOFile *file;
    uint32 trackID;
    /*sample number to export for GF_EXPORT_RAW_SAMPLES only*/
    uint32 sample_num;
    /*out name WITHOUT extension*/
    int8 *out_name;
    void *user_data;
} GF_MediaExporter;

typedef struct
{
    uint32 nb_chan;
    uint32 base_object_type, base_sr, base_sr_index;
    /*SBR*/
    boolean has_sbr;
    uint32 sbr_object_type, sbr_sr, sbr_sr_index;
    /*PL indication*/
    uint8 audioPL;
} GF_M4ADecSpecInfo;

typedef struct
{
    u64 FPS;
    uint32 width;
    uint32 height;
    uint32 count;
} Outinfo;

typedef struct
{
    /*codec tag is the containing box's tag*/
    uint32 codec_tag;
    /*if any, otherwise 0*/
    bin128 UUID;

    uint16 version;
    uint16 revision;
    uint32 vendor_code;

    /*video codecs only*/
    uint32 temporal_quality;
    uint32 spacial_quality;
    uint16 width, height;
    uint32 h_res, v_res;
    uint16 depth;
    uint16 color_table_index;
    int8 szCompressorName[33];

    /*audio codecs only*/
    uint16 NumChannels;
    uint16 bitsPerSample;
    uint32 SampleRate;

    /*if present*/
    uint8 *extension_buf;
    uint32 extension_buf_size;
} GF_GenericSampleDescription;

typedef struct
{
    GF_ISOM_BOX
    uint16 dataReferenceIndex;
    int8 reserved[ 6 ];
    /*box type as specified in the file (not this box's type!!)*/
    uint32 EntryType;

    int8 *data;
    uint32 data_size;
} GF_GenericSampleEntryBox;


#define GF_ISOM_AUDIO_SAMPLE_ENTRY      \


typedef struct
{
    GF_ISOM_BOX
    uint16 version;
    uint16 revision;
    uint32 vendor;
    uint32 temporal_quality;
    uint32 spacial_quality;
    uint16 Width, Height;
    uint32 horiz_res, vert_res;
    uint32 entry_data_size;
    uint16 frames_per_sample;
    uint8 compressor_name[33];  //应该32字节!!!!
    uint16 bit_depth;
    s16 color_table_index;
    /*box type as specified in the file (not this box's type!!)*/
    uint32 EntryType;
    /*opaque description data (ESDS in MP4, SMI in SVQ3, ...)*/
    int8 *data;
    uint32 data_size;
} GF_GenericVisualSampleEntryBox;

typedef struct
{
    GF_ISOM_BOX
    uint16 version;
    uint16 revision;
    uint32 vendor;
    uint16 channel_count;
    uint16 bitspersample;
    uint16 compression_id;
    uint16 packet_size;
    uint16 samplerate_hi;
    uint16 samplerate_lo;
    /*box type as specified in the file (not this box's type!!)*/
    uint32 EntryType;
    /*opaque description data (ESDS in MP4, ...)*/
    int8 *data;
    uint32 data_size;
} GF_GenericAudioSampleEntryBox;


static const uint32 GF_M4ASampleRates[] =
{
    96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050,
    16000, 12000, 11025, 8000, 7350, 0, 0, 0
};

enum
{
    /*!MPEG-4 Object Descriptor Stream*/
    GF_STREAM_OD        = 0x01,
    /*!MPEG-4 Object Clock Reference Stream*/
    GF_STREAM_OCR       = 0x02,
    /*!MPEG-4 Scene Description Stream*/
    GF_STREAM_SCENE     = 0x03,
    /*!Visual Stream (Video, Image or MPEG-4 SNHC Tools)*/
    GF_STREAM_VISUAL    = 0x04,
    /*!Audio Stream (Audio, MPEG-4 Structured-Audio Tools)*/
    GF_STREAM_AUDIO     = 0x05,
    /*!MPEG-7 Description Stream*/
    GF_STREAM_MPEG7     = 0x06,
    /*!MPEG-4 Intellectual Property Management and Protection Stream*/
    GF_STREAM_IPMP      = 0x07,
    /*!MPEG-4 Object Content Information Stream*/
    GF_STREAM_OCI       = 0x08,
    /*!MPEG-4 MPEGlet Stream*/
    GF_STREAM_MPEGJ     = 0x09,
    /*!MPEG-4 User Interaction Stream*/
    GF_STREAM_INTERACT  = 0x0A,
    /*!MPEG-4 IPMP Tool Stream*/
    GF_STREAM_IPMP_TOOL = 0x0B,
    /*!MPEG-4 Font Data Stream*/
    GF_STREAM_FONT      = 0x0C,
    /*!MPEG-4 Streaming Text Stream*/
    GF_STREAM_TEXT      = 0x0D,
    GF_STREAM_PRIVATE_SCENE = 0x20,
};

enum
{
    GF_ISOM_BOX_TYPE_CO64   = GF_FOUR_CHAR_INT( 'c', 'o', '6', '4' ),
    GF_ISOM_BOX_TYPE_STCO   = GF_FOUR_CHAR_INT( 's', 't', 'c', 'o' ),
    GF_ISOM_BOX_TYPE_CRHD   = GF_FOUR_CHAR_INT( 'c', 'r', 'h', 'd' ),
    GF_ISOM_BOX_TYPE_CTTS   = GF_FOUR_CHAR_INT( 'c', 't', 't', 's' ),
    GF_ISOM_BOX_TYPE_CPRT   = GF_FOUR_CHAR_INT( 'c', 'p', 'r', 't' ),
    GF_ISOM_BOX_TYPE_CHPL   = GF_FOUR_CHAR_INT( 'c', 'h', 'p', 'l' ),
    GF_ISOM_BOX_TYPE_URL    = GF_FOUR_CHAR_INT( 'u', 'r', 'l', ' ' ),
    GF_ISOM_BOX_TYPE_URN    = GF_FOUR_CHAR_INT( 'u', 'r', 'n', ' ' ),
    GF_ISOM_BOX_TYPE_DINF   = GF_FOUR_CHAR_INT( 'd', 'i', 'n', 'f' ),
    GF_ISOM_BOX_TYPE_DREF   = GF_FOUR_CHAR_INT( 'd', 'r', 'e', 'f' ),
    GF_ISOM_BOX_TYPE_STDP   = GF_FOUR_CHAR_INT( 's', 't', 'd', 'p' ),
    GF_ISOM_BOX_TYPE_EDTS   = GF_FOUR_CHAR_INT( 'e', 'd', 't', 's' ),
    GF_ISOM_BOX_TYPE_ELST   = GF_FOUR_CHAR_INT( 'e', 'l', 's', 't' ),
    GF_ISOM_BOX_TYPE_UUID   = GF_FOUR_CHAR_INT( 'u', 'u', 'i', 'd' ),
    GF_ISOM_BOX_TYPE_FREE   = GF_FOUR_CHAR_INT( 'f', 'r', 'e', 'e' ),
    GF_ISOM_BOX_TYPE_HDLR   = GF_FOUR_CHAR_INT( 'h', 'd', 'l', 'r' ),
    GF_ISOM_BOX_TYPE_HMHD   = GF_FOUR_CHAR_INT( 'h', 'm', 'h', 'd' ),
    GF_ISOM_BOX_TYPE_HINT   = GF_FOUR_CHAR_INT( 'h', 'i', 'n', 't' ),
    GF_ISOM_BOX_TYPE_MDIA   = GF_FOUR_CHAR_INT( 'm', 'd', 'i', 'a' ),
    GF_ISOM_BOX_TYPE_MDAT   = GF_FOUR_CHAR_INT( 'm', 'd', 'a', 't' ),
    GF_ISOM_BOX_TYPE_MDHD   = GF_FOUR_CHAR_INT( 'm', 'd', 'h', 'd' ),
    GF_ISOM_BOX_TYPE_MINF   = GF_FOUR_CHAR_INT( 'm', 'i', 'n', 'f' ),
    GF_ISOM_BOX_TYPE_MOOV   = GF_FOUR_CHAR_INT( 'm', 'o', 'o', 'v' ),
    GF_ISOM_BOX_TYPE_MVHD   = GF_FOUR_CHAR_INT( 'm', 'v', 'h', 'd' ),
    GF_ISOM_BOX_TYPE_STSD   = GF_FOUR_CHAR_INT( 's', 't', 's', 'd' ),
    GF_ISOM_BOX_TYPE_STSZ   = GF_FOUR_CHAR_INT( 's', 't', 's', 'z' ),
    GF_ISOM_BOX_TYPE_STZ2   = GF_FOUR_CHAR_INT( 's', 't', 'z', '2' ),
    GF_ISOM_BOX_TYPE_STBL   = GF_FOUR_CHAR_INT( 's', 't', 'b', 'l' ),
    GF_ISOM_BOX_TYPE_STSC   = GF_FOUR_CHAR_INT( 's', 't', 's', 'c' ),
    GF_ISOM_BOX_TYPE_STSH   = GF_FOUR_CHAR_INT( 's', 't', 's', 'h' ),
    GF_ISOM_BOX_TYPE_SKIP   = GF_FOUR_CHAR_INT( 's', 'k', 'i', 'p' ),
    GF_ISOM_BOX_TYPE_SMHD   = GF_FOUR_CHAR_INT( 's', 'm', 'h', 'd' ),
    GF_ISOM_BOX_TYPE_STSS   = GF_FOUR_CHAR_INT( 's', 't', 's', 's' ),
    GF_ISOM_BOX_TYPE_STTS   = GF_FOUR_CHAR_INT( 's', 't', 't', 's' ),
    GF_ISOM_BOX_TYPE_TRAK   = GF_FOUR_CHAR_INT( 't', 'r', 'a', 'k' ),
    GF_ISOM_BOX_TYPE_TKHD   = GF_FOUR_CHAR_INT( 't', 'k', 'h', 'd' ),
    GF_ISOM_BOX_TYPE_TREF   = GF_FOUR_CHAR_INT( 't', 'r', 'e', 'f' ),
    GF_ISOM_BOX_TYPE_UDTA   = GF_FOUR_CHAR_INT( 'u', 'd', 't', 'a' ),
    GF_ISOM_BOX_TYPE_VMHD   = GF_FOUR_CHAR_INT( 'v', 'm', 'h', 'd' ),
    GF_ISOM_BOX_TYPE_FTYP   = GF_FOUR_CHAR_INT( 'f', 't', 'y', 'p' ),
    GF_ISOM_BOX_TYPE_FADB   = GF_FOUR_CHAR_INT( 'p', 'a', 'd', 'b' ),
    GF_ISOM_BOX_TYPE_PDIN   = GF_FOUR_CHAR_INT( 'p', 'd', 'i', 'n' ),
    GF_ISOM_BOX_TYPE_META   = GF_FOUR_CHAR_INT( 'm', 'e', 't', 'a' ),
    GF_ISOM_BOX_TYPE_MP4S   = GF_FOUR_CHAR_INT( 'm', 'p', '4', 's' ),
    GF_ISOM_BOX_TYPE_ESDS   = GF_FOUR_CHAR_INT( 'e', 's', 'd', 's' ),
    GF_ISOM_BOX_TYPE_MP4A   = GF_FOUR_CHAR_INT( 'm', 'p', '4', 'a' ),
    GF_ISOM_BOX_TYPE_MP4V   = GF_FOUR_CHAR_INT( 'm', 'p', '4', 'v' ),
    GF_ISOM_BOX_TYPE_GNRM   = GF_FOUR_CHAR_INT( 'g', 'n', 'r', 'm' ),
    GF_ISOM_BOX_TYPE_GNRV   = GF_FOUR_CHAR_INT( 'g', 'n', 'r', 'v' ),
    GF_ISOM_BOX_TYPE_GNRA   = GF_FOUR_CHAR_INT( 'g', 'n', 'r', 'a' ),
    //add shenh
    GF_ISOM_BOX_TYPE_S263   = GF_FOUR_CHAR_INT( 's', '2', '6', '3' ),
    GF_ISOM_BOX_TYPE_SAMR   = GF_FOUR_CHAR_INT( 's', 'a', 'm', 'r' ),
    GF_ISOM_BOX_TYPE_D263   = GF_FOUR_CHAR_INT( 'd', '2', '6', '3' ),
    GF_ISOM_BOX_TYPE_DAMR   = GF_FOUR_CHAR_INT( 'd', 'a', 'm', 'r' ),
    GF_ISOM_BOX_TYPE_MP3        = GF_FOUR_CHAR_INT( '.', 'm', 'p', '3' ),
    GF_ISOM_BOX_TYPE_AVC1   = GF_FOUR_CHAR_INT( 'a', 'v', 'c', '1' ),
    GF_ISOM_BOX_TYPE_AVCC   = GF_FOUR_CHAR_INT( 'a', 'v', 'c', 'C' ),
    GF_ISOM_BOX_TYPE_JPEG   = GF_FOUR_CHAR_INT( 'j', 'p', 'e', 'g' ),
    //
};

enum
{
    GF_ISOM_MEDIA_OD            = GF_FOUR_CHAR_INT( 'o', 'd', 's', 'm' ),
    GF_ISOM_MEDIA_OCR           = GF_FOUR_CHAR_INT( 'c', 'r', 's', 'm' ),
    GF_ISOM_MEDIA_BIFS      = GF_FOUR_CHAR_INT( 's', 'd', 's', 'm' ),
    GF_ISOM_MEDIA_VISUAL        = GF_FOUR_CHAR_INT( 'v', 'i', 'd', 'e' ),
    GF_ISOM_MEDIA_AUDIO     = GF_FOUR_CHAR_INT( 's', 'o', 'u', 'n' ),
    GF_ISOM_MEDIA_MPEG7     = GF_FOUR_CHAR_INT( 'm', '7', 's', 'm' ),
    GF_ISOM_MEDIA_OCI           = GF_FOUR_CHAR_INT( 'o', 'c', 's', 'm' ),
    GF_ISOM_MEDIA_IPMP      = GF_FOUR_CHAR_INT( 'i', 'p', 's', 'm' ),
    GF_ISOM_MEDIA_MPEGJ     = GF_FOUR_CHAR_INT( 'm', 'j', 's', 'm' ),
    GF_ISOM_MEDIA_TEXT  = GF_FOUR_CHAR_INT( 't', 'e', 'x', 't' ),
    GF_ISOM_MEDIA_HINT      = GF_FOUR_CHAR_INT( 'h', 'i', 'n', 't' ),
    GF_ISOM_MEDIA_FLASH     = GF_FOUR_CHAR_INT( 'f', 'l', 's', 'h' )
};


#define GF_ISOM_DATA_FILE           0x01


GF_Box *ftyp_New();
GF_Box *free_New();
GF_Box *mdat_New();
GF_Box *moov_New();
GF_Box *mvhd_New();
GF_Box *mdhd_New();
GF_Box *vmhd_New();
GF_Box *smhd_New();
GF_Box *stbl_New();
GF_Box *dinf_New();
GF_Box *url_New();
GF_Box *urn_New();
GF_Box *cprt_New();
GF_Box *chpl_New();
GF_Box *hdlr_New();
GF_Box *iods_New();
GF_Box *trak_New();
GF_Box *mp4s_New();
GF_Box *mp4v_New();
GF_Box *mp4a_New();
GF_Box *avc1_New();
GF_Box *avcC_New();
GF_Box *mp3_New();
GF_Box *s263_New();
GF_Box *jpeg_New();
GF_Box *samr_New();
GF_Box *d263_New();
GF_Box *damr_New();
//GF_Box *edts_New();
GF_Box *udta_New();
GF_Box *dref_New();
GF_Box *stsd_New();
GF_Box *stts_New();
GF_Box *ctts_New();
//GF_Box *stsh_New();
//GF_Box *elst_New();
GF_Box *stsc_New();
GF_Box *stsz_New();
GF_Box *stco_New();
GF_Box *stss_New();
//GF_Box *stdp_New();
GF_Box *co64_New();
GF_Box *esds_New();
GF_Box *minf_New();
GF_Box *tkhd_New();
//GF_Box *tref_New();
GF_Box *mdia_New();
GF_Box *defa_New();
GF_Box *void_New();
GF_Box *pdin_New();
GF_Box *gnra_New();

//GF_Err reftype_Read(GF_Box *s, GF_BitStream *bs);
GF_Err free_Read(GF_Box *s, GF_BitStream *bs);
GF_Err mdat_Read(GF_Box *s, GF_BitStream *bs);
GF_Err moov_Read(GF_Box *s, GF_BitStream *bs);
GF_Err mvhd_Read(GF_Box *s, GF_BitStream *bs);
GF_Err mdhd_Read(GF_Box *s, GF_BitStream *bs);
GF_Err vmhd_Read(GF_Box *s, GF_BitStream *bs);
GF_Err smhd_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err hmhd_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err nmhd_Read(GF_Box *s, GF_BitStream *bs);
GF_Err stbl_Read(GF_Box *s, GF_BitStream *bs);
GF_Err dinf_Read(GF_Box *s, GF_BitStream *bs);
GF_Err url_Read(GF_Box *s, GF_BitStream *bs);
GF_Err urn_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err chpl_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err cprt_Read(GF_Box *s, GF_BitStream *bs);
GF_Err hdlr_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err iods_Read(GF_Box *s, GF_BitStream *bs);
GF_Err trak_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err mp4s_Read(GF_Box *s, GF_BitStream *bs);
GF_Err mp4v_Read(GF_Box *s, GF_BitStream *bs);
GF_Err mp4a_Read(GF_Box *s, GF_BitStream *bs);
GF_Err avc1_Read(GF_Box *s, GF_BitStream *bs);
GF_Err avcC_Read(GF_Box *s, GF_BitStream *bs);
GF_Err mp3_Read(GF_Box *s, GF_BitStream *bs);
GF_Err s263_Read(GF_Box *s, GF_BitStream *bs);
GF_Err jpeg_Read(GF_Box *s, GF_BitStream *bs);
GF_Err samr_Read(GF_Box *s, GF_BitStream *bs);
GF_Err d263_Read(GF_Box *s, GF_BitStream *bs);
GF_Err damr_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err edts_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err udta_Read(GF_Box *s, GF_BitStream *bs);
GF_Err dref_Read(GF_Box *s, GF_BitStream *bs);
GF_Err stsd_Read(GF_Box *s, GF_BitStream *bs);
GF_Err stts_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err ctts_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err stsh_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err elst_Read(GF_Box *s, GF_BitStream *bs);
GF_Err stsc_Read(GF_Box *s, GF_BitStream *bs);
GF_Err stsz_Read(GF_Box *s, GF_BitStream *bs);
GF_Err stco_Read(GF_Box *s, GF_BitStream *bs);
GF_Err stss_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err stdp_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err co64_Read(GF_Box *s, GF_BitStream *bs);
GF_Err esds_Read(GF_Box *s, GF_BitStream *bs);
GF_Err minf_Read(GF_Box *s, GF_BitStream *bs);
GF_Err tkhd_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err tref_Read(GF_Box *s, GF_BitStream *bs);
GF_Err mdia_Read(GF_Box *s, GF_BitStream *bs);
GF_Err defa_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err void_Read(GF_Box *s, GF_BitStream *bs);
//GF_Err stsf_Read(GF_Box *s, GF_BitStream *bs);
GF_Err pdin_Read(GF_Box *s, GF_BitStream *bs);
GF_Err ftyp_Read(GF_Box *s, GF_BitStream *bs);
GF_Err gf_isom_audio_sample_entry_read(GF_AudioSampleEntryBox *ptr, GF_BitStream *bs);
GF_Err gf_isom_video_sample_entry_read(GF_VisualSampleEntryBox *ptr, GF_BitStream *bs);
int gf_isom_open_file(HANDLE file_handle, uint8 file_mode, uint32 data_Len, GF_ISOFile **movie);
int gf_isom_close_file(GF_ISOFile * movie);//add shenh
GF_Err gf_isom_parse_box(GF_Box **outBox, GF_BitStream *bs);
void gf_isom_full_box_init(GF_Box *a);
GF_Err gf_isom_full_box_read(GF_Box *ptr, GF_BitStream *bs);
u64 gf_bs_available(GF_BitStream *bs);
GF_Box *gf_isom_box_new(uint32 boxType);
void gf_bs_skip_bytes(GF_BitStream *bs, u64 nbBytes);
GF_Err gf_isom_full_box_get_size(GF_Box *ptr);
//GF_Err gf_isom_box_size(GF_Box *ptr);
GF_Err gf_isom_box_get_size(GF_Box *ptr);
//GF_Err gf_isom_box_array_size(GF_Box *parent, GF_List *list);
void gf_isom_audio_sample_entry_init(GF_AudioSampleEntryBox *ptr);
uint32 gf_bs_read_int(GF_BitStream *bs, uint32 nBits);
uint8 gf_bs_read_bit(GF_BitStream *bs);
void gf_isom_audio_sample_entry_size(GF_AudioSampleEntryBox *ptr);
void gf_isom_video_sample_entry_size(GF_VisualSampleEntryBox *ent);
void gf_isom_video_sample_entry_init(GF_VisualSampleEntryBox *ent);
//GF_Err gf_isom_video_sample_entry_read(GF_VisualSampleEntryBox *ptr, GF_BitStream *bs);
GF_Err gf_isom_read_box_list(GF_Box *parent, GF_BitStream *bs, GF_Err (*add_box)(GF_Box *par, GF_Box *b));
GF_Err gf_odf_parse_descriptor(GF_BitStream *bs, GF_Descriptor **desc, uint32 *desc_size);
GF_Descriptor *gf_odf_create_descriptor(uint8 tag);
s32 gf_odf_size_field_size(uint32 size_desc);
GF_Err gf_odf_read_descriptor(GF_BitStream *bs, GF_Descriptor *desc, uint32 DescSize);
GF_Err gf_odf_read_esd(GF_BitStream *bs, GF_ESD *esd, uint32 DescSize);
GF_Err gf_odf_read_dcd(GF_BitStream *bs, GF_DecoderConfig *dcd, uint32 DescSize);
GF_Err gf_odf_read_slc(GF_BitStream *bs, GF_SLConfig *sl, uint32 DescSize);
GF_Err gf_odf_read_default(GF_BitStream *bs, GF_DefaultDescriptor *dd, uint32 DescSize);
GF_Err gf_odf_desc_read(int8 *raw_desc, uint32 descSize, GF_Descriptor * *outDesc);
GF_Descriptor *gf_odf_new_default();
GF_Descriptor *gf_odf_new_dcd();
GF_Descriptor *gf_odf_new_slc(uint8 predef);
GF_Descriptor *gf_odf_new_esd();
uint32 gf_bs_peek_bits(GF_BitStream *bs, uint32 numBits, uint32 byte_offset);
GF_Err gf_media_export_info(GF_ISOFile *file /*GF_MediaExporter *dumper*/);
uint32 gf_isom_get_track_by_id(GF_ISOFile *the_file, uint32 trackID);
GF_TrackBox *gf_isom_get_track_from_file(GF_ISOFile *movie, uint32 trackNumber);
uint32 gf_isom_get_media_type(GF_ISOFile *movie, uint32 trackNumber);
uint32 gf_isom_get_media_subtype(GF_ISOFile *the_file, uint32 trackNumber, uint32 DescriptionIndex);
uint32 gf_bs_write_data(GF_BitStream *bs, uint8 *data, uint32 nbBytes);
uint32 gf_isom_get_sample_count(GF_ISOFile *the_file, uint32 trackNumber);
u64 gf_isom_get_sample_FPS(GF_ISOFile *the_file, uint32 trackNumber);
uint32  gf_isom_get_sample_height(GF_ISOFile *the_file, uint32 trackNumber);
uint32  gf_isom_get_sample_width(GF_ISOFile *the_file, uint32 trackNumber);
uint32 gf_isom_get_track_count(GF_ISOFile *movie);
uint32 gf_isom_get_sample_totaltime(GF_ISOFile *the_file, uint32 trackNumber);
uint32 gf_isom_get_total_KeySamp_count(GF_ISOFile *the_file, uint32 trackNumber );
uint16  gf_isom_get_sample_samplerate(GF_ISOFile *the_file, uint32 trackNumber);
uint16  gf_isom_get_sample_bitspersample(GF_ISOFile *the_file, uint32 trackNumber);
uint16  gf_isom_get_sample_channel_count(GF_ISOFile *the_file, uint32 trackNumber);
GF_ISOSample *gf_isom_get_sample(GF_ISOFile *the_file, GF_TrackBox *trak/*uint32 trackNumber*/, uint32 sampleNumber, uint32 *sampleDescriptionIndex, GF_ISOSample *samp);//change shenh
int32 gf_isom_get_multSample(GF_MediaInformationBox *mInfo, uint32 startSampleNumber, int8 *outbuf, uint32 bufLenght, uint32 *gotBytes, uint32 *gotCount);//shenh add
uint32 gf_isom_get_keySampleNumber(GF_TrackBox *trak, uint32 sampleNumber );//add shenh
uint32 gf_isom_get_nextKey(GF_TrackBox *trak, uint32 sampleNumber );//add shenh
uint32 gf_isom_fdm_get_data(GF_FileDataMap *ptr, int8 *buffer, uint32 bufferLength, u64 fileOffset);
GF_Err stbl_GetSampleDTS(GF_TimeToSampleBox *stts, uint32 SampleNumber, uint32 *DTS, uint32 *sampleCount, uint32 *sampleDelta);
GF_Err stbl_GetSampleRAP(GF_SyncSampleBox *stss, uint32 SampleNumber, uint8 *IsRAP, uint32 *prevRAP, uint32 *nextRAP);
GF_Err stbl_GetSampleSize(GF_MediaInformationBox *mInfo /*GF_SampleSizeBox *stsz*/, uint32 SampleNumber, uint32 *Size);//shenh change
GF_Err stbl_GetSampleInfos(GF_MediaInformationBox *mInfo/*GF_SampleTableBox *stbl*/, uint32 sampleNumber, u64 *offset, uint32 *chunkNumber, uint32 *descIndex, uint8 *isEdited);//shenh change
GF_Err stbl_GetSampleOffset(GF_MediaInformationBox *mInfo, uint32 sampleNumber, u64 *offset);
uint32 stbl_GetSampleDTSdelta(GF_TimeToSampleBox *stts, uint32 SampleNumber);
uint32 stbl_GetSampleDTScount(GF_TimeToSampleBox *stts);
GF_Err stbl_GetSampleSizeFile(GF_SampleSizeBox *stsz, GF_FileDataMap *ptr, uint32 SampleNumber, uint32 *Size);
GF_Err stbl_GetChunkOffsetFile(GF_ChunkOffsetBox *stco, GF_FileDataMap *ptr, uint32 ChunkNumber, u64 *offset);
GF_Err gf_bs_seek(GF_BitStream *bs, u64 offset);
uint8 gf_bs_align(GF_BitStream *bs);
GF_Err Media_GetSampleDesc(GF_MediaBox *mdia, uint32 SampleDescIndex, GF_SampleEntryBox **out_entry, uint32 *dataRefIndex);
GF_DecoderConfig *gf_isom_get_decoder_config(GF_ISOFile *the_file, uint32 trackNumber, uint32 StreamDescriptionIndex);
h263DecSpecStrc * gf_isom_get_h263_spec(GF_ISOFile *the_file, uint32 trackNumber);//add shenh
amrDecSpecStrc * gf_isom_get_amr_spec(GF_ISOFile *the_file, uint32 trackNumber);//add shenh
GF_AVCConfig * gf_isom_get_avc_config(GF_ISOFile *the_file, uint32 trackNumber);//add sheen
GF_Err gf_isom_datamap_new(HANDLE file_handle, uint8 file_mode, uint32 data_Len, GF_DataMap **outDataMap);
void gf_isom_sample_del(GF_ISOSample **samp);
GF_Err gf_m4a_get_config(int8 *dsi, uint32 dsi_size, GF_M4ADecSpecInfo *cfg);
void gf_bs_del(GF_BitStream *bs);
void gf_bs_write_int(GF_BitStream *bs, s32 value, s32 nBits);
GF_GenericSampleDescription *gf_isom_get_generic_sample_description(GF_ISOFile *the_file, uint32 trackNumber, uint32 StreamDescriptionIndex);
GF_TrackBox *gf_isom_get_track(GF_MovieBox *moov, uint32 trackNumber);

//GF_BitStream *bs_glob;//add shenh
//uint8 *pBufGlob;//temp buf. 512bytes .add shenh
#define BUFGLOB_SIZE 512
#define MAX_TRACK_NUM 20//add shenh
#define MAX_TOPBOX_SLOT_NUM 40 //add shenh


#endif

#endif


