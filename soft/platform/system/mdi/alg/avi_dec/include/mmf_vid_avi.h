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



#ifndef _MMf_VID_AVI_H
#define _MMf_VID_AVI_H

#include "string.h"

#ifndef SHEEN_VC_DEBUG

#include "cs_types.h"
#include "cs_types.h"
#include "cswtype.h"
#include "fs.h"
#include "mcip_debug.h"

#else
#include <stdio.h>
//#include "mp4_common.h"
//#include "mcip_debug.h"



//#define __MED_MJPG_MOD__
//#define __MED_MJPG_AUDIO__
//#define MJPEGTRACE 1

typedef unsigned char           uint8;
typedef char             int8;
typedef unsigned short       uint16;
typedef short        int16;
typedef unsigned int            uint32;
typedef int             int32;
typedef unsigned char UINT8;
typedef unsigned int UINT32;
typedef int INT32;

//typedef UINT64       kal_uint64;
//typedef INT64     kal_int64;
#define FS_SEEK_SET SEEK_SET
#define FS_SEEK_CUR SEEK_CUR
#define FS_SEEK_END SEEK_END

#define diag_printf printf
#define mmc_MemMalloc malloc
#endif

#define strncasecmp(a,b,c) strncmp((int8*)a,(int8*)b,c)
//#define kal_mem_cpy(a,b,c) memcpy(a,b,c)
//#define kal_mem_set(a,b,c) memset(a,b,c)

/* fseek a file whence status */


////////////////////shenh2007.9.13 add/////////////////
#define FS_NO_ERROR                     0
#define FS_ERROR_RESERVED              -1
#define FS_PARAM_ERROR                 -2    /* User */
#define FS_INVALID_FILENAME            -3    /* User */
#define FS_DRIVE_NOT_FOUND             -4    /* User or Fatal */
#define FS_TOO_MANY_FILES              -5    /* User or Normal: use over max file handle number or more than 256 files in sort */
#define FS_NO_MORE_FILES               -6    /* Normal */
#define FS_WRONG_MEDIA                 -7    /* Fatal */
#define FS_INVALID_FILE_SYSTEM         -8    /* Fatal */
#define FS_FILE_NOT_FOUND              -9    /* User or Normal */
#define FS_INVALID_FILE_HANDLE         -10   /* User or Normal */
#define FS_UNSUPPORTED_DEVICE          -11   /* User */
#define FS_UNSUPPORTED_DRIVER_FUNCTION -12   /* User or Fatal */
#define FS_CORRUPTED_PARTITION_TABLE   -13   /* fatal */
#define FS_TOO_MANY_DRIVES             -14   /* not use so far */
#define FS_INVALID_FILE_POS            -15   /* User */
#define FS_ACCESS_DENIED               -16   /* User or Normal */
#define FS_STRING_BUFFER_TOO_SMALL     -17   /* User */
#define FS_GENERAL_FAILURE             -18   /* Normal */
#define FS_PATH_NOT_FOUND              -19   /* User */
#define FS_FAT_ALLOC_ERROR             -20   /* Fatal: disk crash */
#define FS_ROOT_DIR_FULL               -21   /* Normal */
#define FS_DISK_FULL                   -22   /* Normal */
#define FS_TIMEOUT                     -23   /* Normal: FS_CloseMSDC with nonblock */
#define FS_BAD_SECTOR                  -24   /* Normal: NAND flash bad block */
#define FS_DATA_ERROR                  -25   /* Normal: NAND flash bad block */
#define FS_MEDIA_CHANGED               -26   /* Normal */
#define FS_SECTOR_NOT_FOUND            -27   /* Fatal */
#define FS_ADDRESS_MARK_NOT_FOUND      -28   /* not use so far */
#define FS_DRIVE_NOT_READY             -29   /* Normal */
#define FS_WRITE_PROTECTION            -30   /* Normal: only for MSDC */
#define FS_DMA_OVERRUN                 -31   /* not use so far */
#define FS_CRC_ERROR                   -32   /* not use so far */
#define FS_DEVICE_RESOURCE_ERROR       -33   /* Fatal: Device crash */
#define FS_INVALID_SECTOR_SIZE         -34   /* Fatal */
#define FS_OUT_OF_BUFFERS              -35   /* Fatal */
#define FS_FILE_EXISTS                 -36   /* User or Normal */
#define FS_LONG_FILE_POS               -37   /* User : FS_Seek new pos over sizeof int */
#define FS_FILE_TOO_LARGE              -38   /* User: filesize + pos over sizeof int */
#define FS_BAD_DIR_ENTRY               -39   /* Fatal */
#define FS_ATTR_CONFLICT               -40   /* User: Can't specify FS_PROTECTION_MODE and FS_NONBLOCK_MOD */
#define FS_CHECKDISK_RETRY             -41   /* System: don't care */
#define FS_LACK_OF_PROTECTION_SPACE    -42   /* Fatal: Device crash */
#define FS_SYSTEM_CRASH                -43   /* Normal */
#define FS_FAIL_GET_MEM                -44   /* Normal */
#define FS_READ_ONLY_ERROR             -45   /* User or Normal */
#define FS_DEVICE_BUSY                 -46   /* Normal */
#define FS_ABORTED_ERROR               -47   /* Normal */
#define FS_FLASH_MOUNT_ERROR           -48   /* Normal */
#define FS_PATH_OVER_LEN_ERROR         -49   /* Normal */
#define FS_APP_QUOTA_FULL              -50   /* Normal */
#define FS_VF_MAP_ERROR                -51   /* User or Normal */
#define FS_DEVICE_EXPORTED_ERROR       -52   /* User or Normal */

#define FS_MSDC_MOUNT_ERROR            -100  /* Normal */
#define FS_MSDC_READ_SECTOR_ERROR      -101  /* Normal */
#define FS_MSDC_WRITE_SECTOR_ERROR     -102  /* Normal */
#define FS_MSDC_DISCARD_SECTOR_ERROR   -103  /* Normal */
#define FS_MSDC_PRESNET_NOT_READY      -104  /* System */
#define FS_MSDC_NOT_PRESENT            -105  /* Normal */

#define FS_FLASH_ERASE_BUSY            -120   /* Normal: only for nonblock mode */
#define FS_LOCK_MUTEX_FAIL             -121   /* System: don't care */
#define FS_NO_NONBLOCKMODE             -122  /* User: try to call nonblock mode other than NOR flash */
#define FS_NO_PROTECTIONMODE           -123  /* User: try to call protection mode other than NOR flash */
#define FS_FILE_OPENED                 -124
#define FS_REN_SAME_NAME           -125

#define FS_FOLDER_HANDLE               0xFF
////////////////shenh add end/////////////////////////


typedef struct
{
    uint32 key;
    uint32 indexNum;//add for part index record. max 65535. shenh
    uint32 pos;
    uint32 len;
} video_index_entry;

typedef struct
{
    uint32 indexNum;//add for part index record. max 65535. shenh
    uint32 pos;
    uint32 len;
    uint32 tot;
} audio_index_entry;

typedef struct
{
    uint32   fdes;              /* File descriptor of AVI file */

    uint8 avi_fs_mode;/*stream file mode(0 file or 1 buffer).sheen*/
    uint32 avi_fs_bufLen;/*file buffer length when avi_fs_mode!=0. sheen*/
    uint8 *pAvi_fs_point;/*file buffer read point. sheen*/
#ifdef   __NO_LIMIT_AVI__
    int32   vi_fdes;              /* File descriptor of Visual */
    int32   au_fdes;              /* File descriptor of Audio */
    int32   idx_fdes;              /* File descriptor of Idx */
#endif
    int32   mode;              /* 0 for reading, 1 for writing */

    uint32   width;             /* Width  of a video frame */
    uint32   height;            /* Height of a video frame */
    uint32 fps;                   /* Frames per second */
    int8   compressor[8];     /* Type of compressor, 4 bytes + padding for 0 byte */
    int8   compressor2[8];     /* Type of compressor, 4 bytes + padding for 0 byte */
    uint32   video_strn;        /* Video stream number */
    uint32   video_frames;      /* Number of video frames */
    int8   video_tag[4];          /* Tag of video data */
    uint32   video_pos;         /* Number of next frame to be read
                                  (if index present) */

    uint32 max_len;              /* maximum video chunk present */

    uint32   a_fmt;             /* Audio format, see #defines below */
    uint32   a_chans;           /* Audio channels, 0 for no audio */
    uint32   a_rate;            /* Rate in Hz */
    uint32   a_bits;            /* bits per audio sample */
    uint32   mp3rate;           /* mp3 bitrate kbs*/
    uint32   audio_strn;        /* Audio stream number */
    uint32   audio_bytes;       /* Total number of bytes of audio data */
    uint32   audio_chunks;      /* Chunks of audio data in the file */
    int8   audio_tag[4];      /* Tag of audio data */
    uint32   audio_posc;        /* Audio position: chunk */
    uint32   audio_posb;        /* Audio position: byte within chunk */

    uint32   pos;               /* position in file */
    int32   n_idx;             /* number of index entries actually filled */
    int32   max_idx;           /* number of index entries actually allocated */

    int32  a_codech_off;       /* absolut offset of audio codec information */
    int32  a_codecf_off;       /* absolut offset of audio codec information */
    int32  v_codech_off;       /* absolut offset of video codec (strh) info */
    int32  v_codecf_off;       /* absolut offset of video codec (strf) info */

    uint8 (*idx)[16];            /* index entries (AVI idx1 tag) */
    video_index_entry * video_index;
    audio_index_entry * audio_index;
    uint32   last_pos;          /* Position of last frame written */
    uint32   last_len;          /* Length of last frame written */
    int32    must_use_index;    /* Flag if frames are duplicated */
    int32   movi_start;
}
avi_struct;

#define AVI_MODE_WRITE  0
#define AVI_MODE_READ   1
#define AVI_MODE_READ_ARRAY   2


#define AVI_ERR_SIZELIM      1
#define AVI_ERR_OPEN         2
#define AVI_ERR_READ         3
#define AVI_ERR_WRITE        4
#define AVI_ERR_WRITE_INDEX  5
#define AVI_ERR_CLOSE        6
#define AVI_ERR_NOT_PERM     7
#define AVI_ERR_NO_MEM       8
#define AVI_ERR_NO_AVI       9
#define AVI_ERR_NO_HDRL     10
#define AVI_ERR_NO_MOVI     11
#define AVI_ERR_NO_VIDS     12
#define AVI_ERR_NO_IDX      13
#define AVI_ERR_FORTMAT     14
#define AVI_ERR_NO_INDX     15

/* Possible Audio formats */
/*
#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_UNKNOWN             (0x0000)
#define WAVE_FORMAT_PCM                 (0x0001)
#define WAVE_FORMAT_ADPCM               (0x0002)
#define WAVE_FORMAT_IBM_CVSD            (0x0005)
#define WAVE_FORMAT_ALAW                (0x0006)
#define WAVE_FORMAT_MULAW               (0x0007)
#define WAVE_FORMAT_OKI_ADPCM           (0x0010)
#define WAVE_FORMAT_DVI_ADPCM           (0x0011)
#define WAVE_FORMAT_DIGISTD             (0x0015)
#define WAVE_FORMAT_DIGIFIX             (0x0016)
#define WAVE_FORMAT_YAMAHA_ADPCM        (0x0020)
#define WAVE_FORMAT_DSP_TRUESPEECH      (0x0022)
#define WAVE_FORMAT_GSM610              (0x0031)
#define IBM_FORMAT_MULAW                (0x0101)
#define IBM_FORMAT_ALAW                 (0x0102)
#define IBM_FORMAT_ADPCM                (0x0103)
#endif
*/
//add shenh
#define AVI_FORMAT_MJPG (0x47504a4d) //"MJPG"
#define AVI_FORMAT_XVID (0x44495658) // "XVID"
#define AVI_FORMAT_XVID2    (0x64697678)    // "xvid"
#define AVI_FORMAT_DX50 (0x30355844) // "DX50"
#define AVI_FORMAT_DIV3 (0x33564944) // "DIV3"
#define AVI_FORMAT_DIVX (0x58564944) // "DIVX"
#define AVI_FORMAT_divx     (0x78766964) // "divx"
#define AVI_FORMAT_PM4V (0x56344d50) // "PM4V"=="DIV4"
#define AVI_FORMAT_FMP4 (0x34504d46) // "FMP4" ffmpeg
#define AVI_FORMAT_AVC1 (0x31637661) // "avc1==H264"
#define AVI_FORMAT_MP3 (0x0055)//layer3
#define AVI_FORMAT_MP2 (0x0050)//mpeg1 audio layer2
#define AVI_FORMAT_AMR (0x00fe)
#define AVI_FORMAT_AMR_2 (0x0057)//==AMR
#define AVI_FORMAT_PCM (0x0001)
#define AVI_FORMAT_ADPCM (0x0011)

#ifdef __NO_LIMIT_AVI__
uint32 avi_extend_to_cluster_size(FS_HANDLE FileHandle, const WCHAR *FileName);
int32 avi_get_audio_format(uint32 audio_format, uint32 audio_rate);
void avi_toggle_chunk_buffer(void);
void avi_toggle_idx_buffer(void);
int32 avi_dump_chunk_buffer_to_file(int32 fd);
int32 avi_dump_idx_buffer_to_file(int32 fd);
int32 avi_residual_bytes_in_active_buffer(avi_struct *avi);
int32 avi_residual_idx_in_active_buffer(avi_struct *avi);
#endif

avi_struct* avi_open_output_file(const uint16 *filename);
int32  avi_close_output_file(avi_struct *avi);
void avi_set_video(avi_struct *avi, uint32 width, uint32 height, uint32 fps, int8 *compressor);
void avi_set_audio(avi_struct *avi, uint32 channels, uint32 rate, uint32 bits, uint32 format, uint32 mp3rate);
int32  avi_write_frame(avi_struct *avi, int8 *data, uint32 bytes, int32 keyframe);
int32  avi_dup_frame(avi_struct *avi);
int32  avi_write_audio(avi_struct *avi, int8 *data, uint32 bytes);
int32  avi_append_audio(avi_struct *avi, int8 *data, uint32 bytes);
int32 avi_bytes_remain(avi_struct *avi);
int32 avi_bytes_written(avi_struct *avi);

void avi_set_mem_cache(uint16 cache);
#ifdef SHEEN_VC_DEBUG
avi_struct *avi_open_input_file(FILE* fhd, uint8 file_mode, uint32 data_Len, int32 getIndex);
#else
avi_struct *avi_open_input_file(HANDLE fhd, uint8 file_mode, uint32 data_Len, int32 getIndex);
#endif
int32  avi_close_input_file(avi_struct *avi);
avi_struct *avi_open_fd(int32 fd, int32 getIndex);
avi_struct *avi_open_input_array(uint8 *data, uint32 len, int32 getIndex);
int32 avi_close_input_array(avi_struct *avi);
int32 avi_parse_input_file(avi_struct *avi, int32 getIndex);
uint32 avi_video_frames(avi_struct *avi);
uint32  avi_video_width(avi_struct *avi);
uint32  avi_video_height(avi_struct *avi);
uint32 avi_frame_rate(avi_struct *avi);
int8* avi_video_compressor(avi_struct *avi);

uint32 avi_frame_size(avi_struct *avi, uint32 frame);
uint32 avi_audio_size(avi_struct *avi, uint32 frame);
int32  avi_set_video_position(avi_struct *avi, uint32 frame);
int32 avi_read_frame(avi_struct *avi, int8 *vidbuf, uint32 bufSize, int32 *keyframe);
uint32 avi_get_video_position(avi_struct *avi, uint32 frame);
uint32 avi_get_video_keyframe_num(avi_struct *avi, uint32 frame);
int32 avi_read_audio(avi_struct *avi, int8 *audbuf, int32 bytes);


int32  avi_set_audio_position(avi_struct *avi, uint32 byteNum);
int32 avi_set_audio_position2(avi_struct *avi, uint32 chunkNum);
int32  avi_set_audio_bitrate(avi_struct *avi, uint32 bitrate);

int32  avi_read_data(avi_struct *avi, int8 *vidbuf, uint32 max_vidbuf,
                     int8 *audbuf, uint32 max_audbuf,
                     uint32 *len);

int32 avi_get_error(void);

struct riff_struct
{
    uint8 id[4];   /* RIFF */
    uint32 len;
    uint8 wave_id[4]; /* WAVE */
};


struct chunk_struct
{
    uint8 id[4];
    uint32 len;
};

struct common_struct
{
    uint16 wFormatTag;
    uint16 wChannels;
    uint32 dwSamplesPerSec;
    uint32 dwAvgBytesPerSec;
    uint16 wBlockAlign;
    uint16 wBitsPerSample;  /* Only for PCM */
};

struct wave_header
{
    struct riff_struct   riff;
    struct chunk_struct  format;
    struct common_struct common;
    struct chunk_struct  data;
};



struct AviStreamHeader
{
    int32  fccType;
    int32  fccHandler;
    int32  dwFlags;
    int32  dwPriority;
    int32  dwInitialFrames;
    int32  dwScale;
    int32  dwRate;
    int32  dwStart;
    int32  dwLength;
    int32  dwSuggestedBufferSize;
    int32  dwQuality;
    int32  dwSampleSize;
};

typedef struct
{
    uint8* mem_start;
    uint8* mem_end;
    uint8* cur_p;
    uint8* top_p;
    uint32 len;
}
avi_chunk_buffer_struct;

//for ext indx

// bIndexType codes
#define AVI_INDEX_OF_INDEXES 0x00 // when each entry in aIndex
// array points to an index chunk
#define AVI_INDEX_OF_CHUNKS 0x01 // when each entry in aIndex
// array   points to a chunk in the file
#define AVI_INDEX_IS_DATA 0x80 // when each entry is aIndex is
// really the data
// bIndexSubtype codes for INDEX_OF_CHUNKS
#define AVI_INDEX_2FIELD 0x01 // when fields within frames
// are also indexed

//ext base indx
typedef struct
{
    uint32 fcc;// 'indx' in base. 'ix##' in standard / field /super indx.
    uint32  cb;//size
    uint16   wLongsPerEntry;   // size of each entry in aIndex array
    uint8   bIndexSubType;    // future use.  must be 0. AVI_INDEX_2FIELD or 0
    uint8   bIndexType;       // one of AVI_INDEX_* codes. AVI_INDEX_OF_INDEXES/ AVI_INDEX_OF_CHUNKS /AVI_INDEX_IS_DATA
    uint32  nEntriesInUse;    // entry number in aIndex array
    uint32  dwChunkId;        // fcc of what is indexed. '00db' '01wb' ...
    uint32  dwReserved[3];    /* meaning differs for each index . (1)no use in base.(2)in standard and field[0] [1] compose uint64 qwBaseOffset;
                        all dwOffsets in aIndex array are relative to this. (3)no use in super.
                                    */

//every entry struct
    /* base
    struct _aviindex_entry {
        uint32 adw[wLongsPerEntry];
     } aIndex[ ];
     */

    /* standard . normally use
        struct _avistdindex_entry {
          uint32    dwOffset;     // qwBaseOffset + this is absolute file offset
          uint32    dwSize;       // bit 31 is set if this is NOT a keyframe
       } aIndex[ ];
       */

    /* field
       struct _avifieldindex_entry {
          uint32    dwOffset;
          uint32    dwSize;         // size of all fields // (bit 31 set for NON-keyframes)
          uint32    dwOffsetField2; // offset to second field
       } aIndex[  ];
    */

    /*super
          struct _avisuperindex_entry {
          uint64 qwOffset;   // absolute file offset, offset 0 is    // unused entry??
          uint32    dwSize;     // size of index chunk at this offset
          uint32    dwDuration; // time span in stream ticks
       } aIndex[ ];*/
}
_aviindex_chunk;


#endif


