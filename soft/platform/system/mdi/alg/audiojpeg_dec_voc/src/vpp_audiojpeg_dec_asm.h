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
















#ifndef vpp_AudioJpeg_DEC_ASM_H
#define vpp_AudioJpeg_DEC_ASM_H

#include "cs_types.h"

#define RAM_X_BEGIN_ADDR               0x0000
#define RAM_Y_BEGIN_ADDR               0x4000
#include "voc_map_addr.h"

#define vpp_AudioJpeg_DEC_CODE_SIZE       280*4

#define vpp_AudioJpeg_DEC_CODE_ENTRY 0
#define vpp_AudioJpeg_DEC_CRITICAL_SECTION_MIN 0
#define vpp_AudioJpeg_DEC_CRITICAL_SECTION_MAX 0

//#define vpp_AudioJpeg_DEC_STREAM_BUF_STRUCT (   0 + 0x0)
#define vpp_AudioJpeg_DEC_IN_STRUCT           (0   + 0x4000)
#define vpp_AudioJpeg_DEC_OUT_STRUCT        (20  + 0x4000)

#define VPP_Current_Audio_Mode_addr             (36 + 0x4000)


#define VPP_AAC_Code_ExternalAddr_addr             (38 + 0x4000)
#define VPP_AAC_Tab_huffTabSpec_START_addr    (44 + 0x4000)

#define VPP_MP3_Code_ExternalAddr_addr          (52 + 0x4000)
#define VPP_AMR_Code_ExternalAddr_addr          (58 + 0x4000)
#define VPP_JPEG_Code_ExternalAddr_addr         (64+ 0x4000)
#define VPP_H263_Zoom_Code_ExternalAddr_addr     (68+ 0x4000)
#define VPP_SBC_ENC_Code_ExternalAddr_addr          (72+0x4000)

#define VPP_Speak_AntiDistortion_Filter_Coef_addr          (34+0x4000)


#define VPP_GLOBAL_NON_CLIP_HISTORYDATA_L_MP3_ADDR          (74+0x4000)
#define VPP_GLOBAL_NON_CLIP_HISTORYDATA_R_MP3_ADDR          (76+0x4000)

#define VPP_GLOBAL_DIGITAL_GAIN_MP3_ADDR                        (78+0x4000)
#define VPP_GLOBAL_DIGITAL_MAXVALUE_MP3_ADDR               (79+0x4000)

#define VPP_DRC_MODE_Code_ExternalAddr_addr         VPP_JPEG_Code_ExternalAddr_addr
#define VOC_MP3_DEC_MODE  1

#ifdef VOCCODE_IN_PSRAM
//common code
INT32 G_VppCommonDecCode[] =
{
#include "audiojpeg_dec_common.tab"
};



//aac
INT32 G_VppAacDecCode[] =
{
#include "aacdec.tab"
};

INT32 G_VppAacDecConstY[] =
{
#include "vpp_aac_dec_const_y.tab"
};


INT32 G_VppMp3DecCode[] =
{
#include "mp123dec.tab"
};

INT32 G_VppMp3DecConstX[] =
{
#include "vpp_mp3_dec_const_x.tab"
};

INT32 G_VppMp3DecConstY[] =
{
#include "vpp_mp3_dec_const_y_v1.tab"
};

INT32 G_VppMp3Layer12Dec_DMA_ConstY[] =
{
#include "tab_layer_1_2.tab"
};

//amr
#if 0 //replace by speech amr.sheen
INT32 G_VppAmrDecCode[] =
{
#include "amrdec.tab"
};
#endif

INT32 G_VppMp3Dec_EQ_DMA_ConstY[] =
{
#include "mp3decequalizer.tab" // mp3 eq
};

#if 0 //replace by speech amr.sheen
INT32 G_VppAmrDecConstY[] =
{
#include "vpp_amr_dec_const_y.tab"
};
#endif

//jpeg
#ifdef VIDEO_PLAYER_SUPPORT
#if 0 //replace by G_VppJpegDec2Code or software.sheen
INT32 G_VppJpegDecCode[] =
{
#include "jpegdec.tab"
};


INT32 G_VppJpegDecConstY[] =
{
#include "vpp_jpeg_dec_const_y.tab"
};
#endif


// huffTabSpec
INT32 G_VppAacDecHuffTabSpec[] =
{
#include "tab_hufftabspec.tab"
};

// cos4sin4tab
INT32 G_VppAacDecCos4sin4tab[] =
{
#include "tab_cos4sin4tab.tab"
};

// twidTabOdd
INT32 G_VppAacDecTwidTabOdd[] =
{
#include "tab_twidtabodd.tab"
#include "aacdecequalizer.tab"
};

// sinWindow
INT32 G_VppAacDecSinWindow[] =
{
#include "tab_sinwindow.tab"
#include "tab_kbdwindow.tab"
};

// kbdWindow
#if 0 //replace by gouda.sheen
INT32 G_VppH263ZoomCode[] =
{
#include "H263zoom.tab"
};

INT32 G_VppH263ZoomConstX[] =
{
#include "H263_ConstX.tab"
};
#endif

#ifdef MEDIA_VOCVID_SUPPORT
#ifdef MEDIA_RM_SUPPORT
INT32 G_VppRMDecCode[] =
{
#include "rmdec.tab"
};

INT32 G_VppRMDecConsty[] =
{
#include "vpp_rm_dec_const_y.tab"
};
#endif //MEDIA_RM_SUPPORT

#ifdef MEDIA_H264_SUPPORT
INT32 G_VppH264DecCode[] =
{
#include "h264dec.tab"
};

INT32 G_VppH264DecConsty[] =
{
#include "vpp_h264_dec_const_y.tab"
};
#endif //MEDIA_H264_SUPPORT

INT32 G_VppH263DecCode[] =
{
#include "h263dec.tab"
};


INT32 G_VppH263DecConsty[] =
{
#include "vpp_h263_dec_const_y.tab"
};

INT32 G_VppJpegDec2Code[] =
{
#include "jpegdec2.tab"
};

/*
INT32 G_VppJpegDec2Consty[] =
{
#include "vpp_jpeg_dec2_const_y.tab"
};*/

INT32 G_VppMpeg4DecCode[] =
{
#include "mpeg4dec.tab"
};


INT32 G_VppMpeg4DecConsty[] =
{
#include "vpp_mpeg4_dec_const_y.tab"
};
#endif //MEDIA_VOCVID_SUPPORT
#endif //VIDEO_PLAYER_SUPPORT


//SBC ENCODER
INT32 G_VppSBCCode[] =
{
#include "sbcenc.tab"
};

INT32 G_VppSBCConstX[] =
{
#include "sbc_constx.tab"
};

//DRC MODE
INT32 G_VppDRCCode[]=
{
#include "drc_mode.tab"
};

INT32 G_VppDRCConst[]=
{
#include "drc_constY.tab"
};

#else //VOCCODE_IN_PSRAM

//common code
const INT32 G_VppCommonDecCode[] MICRON_ALIGN(32)=
{
#include "audiojpeg_dec_common.tab"
};



//aac
const INT32 G_VppAacDecCode[] MICRON_ALIGN(32)=
{
#include "aacdec.tab"
};

const INT32 G_VppAacDecConstY[] MICRON_ALIGN(32)=
{
#include "vpp_aac_dec_const_y.tab"
};

//mp3



const INT32 G_VppMp3DecCode[] MICRON_ALIGN(32)=
{
#include "mp123dec.tab"
};

const INT32 G_VppMp3DecConstX[] MICRON_ALIGN(32)=
{
#include "vpp_mp3_dec_const_x.tab"
};

const INT32 G_VppMp3DecConstY[] MICRON_ALIGN(32)=
{
#include "vpp_mp3_dec_const_y_v1.tab"
};

const INT32 G_VppMp3Layer12Dec_DMA_ConstY[] MICRON_ALIGN(32)=
{
#include "tab_layer_1_2.tab"
};

//amr

#if 0 //replace by speech amr.sheen
const INT32 G_VppAmrDecCode[] MICRON_ALIGN(32)=
{
#include "amrdec.tab"
};
#endif

const INT32 G_VppMp3Dec_EQ_DMA_ConstY[] MICRON_ALIGN(32)=
{
#include "mp3decequalizer.tab" // mp3 eq
};

#if 0 //replace by speech amr.sheen
const INT32 G_VppAmrDecConstY[] MICRON_ALIGN(32)=
{
#include "vpp_amr_dec_const_y.tab"
};
#endif

//jpeg
#ifdef VIDEO_PLAYER_SUPPORT
#if 0 //replace by G_VppJpegDec2Code or software.sheen
const INT32 G_VppJpegDecCode[] MICRON_ALIGN(32)=
{
#include "jpegdec.tab"
};

const INT32 G_VppJpegDecConstY[] MICRON_ALIGN(32)=
{
#include "vpp_jpeg_dec_const_y.tab"
};
#endif

// huffTabSpec
const INT32 G_VppAacDecHuffTabSpec[] MICRON_ALIGN(32)=
{
#include "tab_hufftabspec.tab"
};

// cos4sin4tab
const INT32 G_VppAacDecCos4sin4tab[] MICRON_ALIGN(32)=
{
#include "tab_cos4sin4tab.tab"
};

// twidTabOdd
const INT32 G_VppAacDecTwidTabOdd[] MICRON_ALIGN(32)=
{
#include "tab_twidtabodd.tab"
#include "aacdecequalizer.tab"
};

// sinWindow
const INT32 G_VppAacDecSinWindow[] MICRON_ALIGN(32)=
{
#include "tab_sinwindow.tab"
#include "tab_kbdwindow.tab"
};

// kbdWindow
#if 0 //replace by gouda.sheen
const INT32 G_VppH263ZoomCode[] MICRON_ALIGN(32)=
{
#include "H263zoom.tab"
};

const INT32 G_VppH263ZoomConstX[] MICRON_ALIGN(32)=
{
#include "H263_ConstX.tab"
};
#endif

#ifdef MEDIA_VOCVID_SUPPORT
#ifdef MEDIA_RM_SUPPORT
const INT32 G_VppRMDecCode[] MICRON_ALIGN(32)=
{
#include "rmdec.tab"
};

const INT32 G_VppRMDecConsty[] MICRON_ALIGN(32)=
{
#include "vpp_rm_dec_const_y.tab"
};
#endif //MEDIA_RM_SUPPORT

#ifdef MEDIA_H264_SUPPORT
const INT32 G_VppH264DecCode[] MICRON_ALIGN(32)=
{
#include "h264dec.tab"
};

const INT32 G_VppH264DecConsty[] MICRON_ALIGN(32)=
{
#include "vpp_h264_dec_const_y.tab"
};
#endif //MEDIA_H264_SUPPORT
//extern const INT32 G_VppH263DecCode[];
//extern const INT32 G_VppH263DecConsty[];

//extern const INT32 G_VppJpegDec2Code[];
/*
const INT32 G_VppJpegDec2Consty[] =
{
#include "vpp_jpeg_dec2_const_y.tab"
};*/

//extern const INT32 G_VppMpeg4DecCode[];


//extern const INT32 G_VppMpeg4DecConsty[];

const INT32 G_VppH263DecCode[] MICRON_ALIGN(32)=
{
#include "h263dec.tab"
};

const INT32 G_VppH263DecConsty[] MICRON_ALIGN(32)=
{
#include "vpp_h263_dec_const_y.tab"
};

const INT32 G_VppJpegDec2Code[] MICRON_ALIGN(32)=
{
#include "jpegdec2.tab"
};

const INT32 G_VppMpeg4DecCode[] MICRON_ALIGN(32)=
{
#include "mpeg4dec.tab"
};

const INT32 G_VppMpeg4DecConsty[] MICRON_ALIGN(32)=
{
#include "vpp_mpeg4_dec_const_y.tab"
};

#endif //MEDIA_VOCVID_SUPPORT
#endif //VIDEO_PLAYER_SUPPORT

//SBC ENCODER
const INT32 G_VppSBCCode[] MICRON_ALIGN(32)=
{
#include "sbcenc.tab"
};

const INT32 G_VppSBCConstX[] MICRON_ALIGN(32)=
{
#include "sbc_constx.tab"
};

//DRC MODE
extern const INT32 G_VppDRCCode[];
extern const INT32 G_VppDRCConst[];

#endif //VOCCODE_IN_PSRAM

//mp3
#if 0 //MP3 VoC decoder not use NonClip, replace by drc mode
#ifdef SMALL_BSS_RAM_SIZE

#define G_Mp3NonCliPLBuf_Len 2304
#define G_Mp3NonCliPRBuf_Len 2304
INT8* G_Mp3NonCliPLBuf_alloc= NULL;
INT8* G_Mp3NonCliPRBuf_alloc= NULL;
INT32* G_Mp3NonCliPLBuf= NULL;
INT32* G_Mp3NonCliPRBuf= NULL;

#else
INT32 G_Mp3NonCliPLBuf[1152/2] =
{
    0
};

INT32 G_Mp3NonCliPRBuf[1152/2] =
{
    0
};
#endif
#endif

INT32 G_Mp3DRCHistoryBuf[1224] =
{
    0
};


//INT32 G_VppSBCConstY[552] = //**must can't be const type**
//{
//#include "sbc_consty.tab"
//};
#endif  // vpp_AudioJpeg_DEC_ASM_H
