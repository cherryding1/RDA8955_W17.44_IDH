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



#ifndef _MMF_JPEGAMRRING_AVI_ENC_H_
#define _MMF_JPEGAMRRING_AVI_ENC_H_

#include "cs_types.h"
#include "cs_types.h"


#define LILEND2(a) (a)
#define LILEND4(a) (a)
/* 4 bytes */
//typedef int WORD;
//typedef unsigned int DWORD;
/* 1 byte */
//typedef char BYTE;
//typedef long off_t;


/* for use in AVI_avih.flags */
#ifndef AVIF_HASINDEX
#define AVIF_HASINDEX 0x00000010    // index at end of file 
#endif
#ifndef AVIF_MUSTUSEINDEX
#define AVIF_MUSTUSEINDEX 0x00000020
#endif
#ifndef AVIF_ISINTERLEAVED
#define AVIF_ISINTERLEAVED 0x00000100
#endif
#ifndef AVIF_TRUSTCKTYPE
#define AVIF_TRUSTCKTYPE 0x00000800
#endif
#ifndef AVIF_WASCAPTUREFILE
#define AVIF_WASCAPTUREFILE 0x00010000
#endif
#ifndef AVIF_COPYRIGHTED
#define AVIF_COPYRIGHTED 0x00020000
#endif


struct AVI_avih
{
    DWORD us_per_frame;   /* frame display rate (or 0L) */
    DWORD max_bytes_per_sec;  /* max. transfer rate */
    DWORD padding;    /* pad to multiples of this size; */
    /* normally 2K */
    DWORD flags;
    DWORD tot_frames; /* # frames in file */
    DWORD init_frames;
    DWORD streams;
    DWORD buff_sz;
    DWORD width;
    DWORD height;
    DWORD reserved[4];
};


struct AVI_strh
{
    uint8 type[4];      /* stream type */
    uint8 handler[4];
    DWORD flags;
    DWORD priority;
    DWORD init_frames;       /* initial frames (???) */
    DWORD scale;
    DWORD rate;
    DWORD start;
    DWORD length;
    DWORD buff_sz;           /* suggested buffer size */
    DWORD quality;
    DWORD sample_sz;
    short  left;
    short  top;
    short  right;
    short  bottom;
};


struct AVI_strf_video
{
    DWORD sz;
    DWORD width;
    DWORD height;
    DWORD planes_bit_cnt;
    uint8 compression[4];
    DWORD image_sz;
    DWORD xpels_meter;
    DWORD ypels_meter;
    DWORD num_colors;        /* used colors */
    DWORD imp_colors;        /* important colors */
    /* may be more for some codecs */
};

struct AVI_strf_audio
{
    short     wFormatTag;
    short     nChannels;
    DWORD   nSamplesPerSec;
    DWORD   nAvgBytesPerSec;
    short     nBlockAlign;
    short     wBitsPerSample;
    short     cbSize;
};
/*
  AVI_list_hdr

  spc: a very ubiquitous AVI struct, used in list structures
       to specify type and length
*/

struct AVI_list_hdr
{
    uint8 id[4];   /* "LIST" */
    DWORD sz;              /* size of owning struct minus 8 */
    uint8 type[4]; /* type of list */
};


struct AVI_list_odml
{
    struct AVI_list_hdr list_hdr;

    uint8 id[4];
    DWORD sz;
    DWORD frames;
};


struct AVI_list_strl_video
{
    struct AVI_list_hdr list_hdr;

    /* chunk strh */
    uint8 strh_id[4];
    DWORD strh_sz;
    struct AVI_strh strh;

    /* chunk strf */
    uint8 strf_id[4];
    DWORD strf_sz;
    struct AVI_strf_video strf_video;

    /* list odml */
//  struct AVI_list_odml list_odml;
};

struct AVI_list_strl_audio
{
    struct AVI_list_hdr list_hdr;

    /* chunk strh */
    uint8 strh_id[4];
    DWORD strh_sz;
    struct AVI_strh strh;

    /* chunk strf */
    uint8 strf_id[4];
    DWORD strf_sz;
    struct AVI_strf_audio strf_audio;

};

struct AVI_list_hdrl
{
    struct AVI_list_hdr list_hdr;

    /* chunk avih */
    uint8 avih_id[4];
    DWORD avih_sz;
    struct AVI_avih avih;

    /* list strl */
    struct AVI_list_strl_video strl_video;
    struct AVI_list_strl_audio strl_audio;
};

typedef struct
{
    UINT16 video_size;
    UINT16 audio_size;
} AVI_video_index;

typedef struct
{
    unsigned long audio_offset;
    unsigned long audio_size;
} AVI_audio_index;

//err return
#define FUNC_OK 0
#define ERR_NO_INPUT_PARAMETER -1


/******************************

 for user
******************************/

#define AVI_HEAD_SIZE (304+20+4)//avi head size
typedef struct
{
    uint16 Image_Height;
    uint16 Image_Width;
    uint16 Video_fps;//10fps..
    uint16 Amr_BitRate;//12800..
    uint16 Audio_SampleRate;    //48k,44.1k,32k,22.05k,24k,16k,12k,11.025k,8k
    uint8  Audio_Bits;      //16bit or 8bit
    uint8  Audio_Channel;  ///one or two//
} JpegAmr_Avi_Init;


typedef struct
{
    uint8 *Jpeg_inBuf;
    uint8 *Amr_inBuf;
    uint8 *Avi_outBuf;
    uint32 Jpeg_Frame_Size;
    uint32 Amr_Data_Size;
} JpegAmr_Avi_Enc;

typedef struct
{
    uint8 *pAvi_End_Buf;//Avi  end idx data buf
    uint32 Buf_size;//end buf size
} JpegAmr_Avi_End;

typedef struct
{
    uint32  OutByte_size;//write out byte size
    uint32  RemainByte_size;//remain byte size 0=over
} Avi_End_Return;

extern int16 MMF_Avi_Enc_Init(JpegAmr_Avi_Init* pAvi_Init);
extern int32 MMF_Avi_Enc_Frame(JpegAmr_Avi_Enc* pAvi_Enc);//return out byte size
extern int16 MMF_Avi_Enc_Head(uint8* pAvi_Head_Buf);
extern int16 MMF_Avi_Enc_End(JpegAmr_Avi_End* pAvi_End, Avi_End_Return* pAvi_End_Re);//return pAvi_End_Re

//#define AVI_TEST
#ifdef AVI_TEST
void video_record_test();
#endif

#endif
