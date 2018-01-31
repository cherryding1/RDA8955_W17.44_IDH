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

//common code



#ifdef VOCCODE_IN_PSRAM

//common code

#if 0//defined (_CAM_SMALL_MEM_) ||defined ( _CAM_SPECIAL_GC6113_)
INT32 G_VppCommonEncCode[] =
{
#include "audiojpeg_enc_common_qvga.tab"
};
#else
INT32 G_VppCommonEncCode[] =
{
#include "vpp_audiojpegenc_asm_main.tab"
};
#endif

//mp3
#ifdef MP3_ENC_SUPPORT

INT32 G_VppMp3EncCode[] =
{
    #include "vpp_audiojpegenc_asm_mp3enc.tab"
};

INT32 G_VppMp3EncConstX[] =
{
    #include "vpp_mp3_enc_const_x.tab"
};

INT32 G_VppMp3EncConstY[] =
{
   #include "vpp_mp3_enc_const_y.tab"
};


INT32 G_VppMp3EncConst_rqmy[] =
{
    #include "vpp_mp3_enc_const_rqmy.tab"
};

INT32 G_VppMp3EncConst_Zig[] =
{
    #include "vpp_mp3_enc_const_zig.tab"
};
#endif




//amrjpeg

#if  0//defined(_CAM_SMALL_MEM_) ||defined( _CAM_SPECIAL_GC6113_)
INT32 G_VppAmrJpegEncCode[] =
{
#include "amrjpegenc_qvga.tab"
};
#else
INT32 G_VppAmrJpegEncCode[] =
{
#include "vpp_audiojpegenc_asm_amrjpegenc.tab"
};
#endif


INT32 G_VppAmrJpegEncConstX[] =
{
#include "vpp_amrjpeg_enc_const_x.tab"
};

INT32 G_VppAmrJpegEncConstY[] =
{
#include "vpp_amrjpeg_enc_const_y.tab"
};


PROTECTED  INT32 G_VppAudJpegMainCodeSize = sizeof(G_VppCommonEncCode);

INT32 G_vocYUVRomateCode[] MICRON_ALIGN(32)=
{
#include "yuvrotate.tab"
};

#else
#ifndef MICRON_ALIGN

#define MICRON_ALIGN(a)

#endif




//common code

#if  0//defined(_CAM_SMALL_MEM_) ||defined( _CAM_SPECIAL_GC6113_)
const INT32 G_VppCommonEncCode[] MICRON_ALIGN(32)=
{
#include "audiojpeg_enc_common_qvga.tab"
};
#else
const INT32 G_VppCommonEncCode[] MICRON_ALIGN(32)=
{
#include "vpp_audiojpegenc_asm_main.tab"
};
#endif



#ifdef MP3_ENC_SUPPORT

//mp3

const INT32 G_VppMp3EncCode[] MICRON_ALIGN(32)=
{
    #include "vpp_audiojpegenc_asm_mp3enc.tab"
};

const INT32 G_VppMp3EncConstX[] MICRON_ALIGN(32)=
{
    #include "vpp_mp3_enc_const_x.tab"
};

const INT32 G_VppMp3EncConstY[] MICRON_ALIGN(32)=
{
    #include "vpp_mp3_enc_const_y.tab"
};


const INT32 G_VppMp3EncConst_rqmy[] MICRON_ALIGN(32)=
{
    #include "vpp_mp3_enc_const_rqmy.tab"
};

const INT32 G_VppMp3EncConst_Zig[] MICRON_ALIGN(32)=
{
    #include "vpp_mp3_enc_const_zig.tab"
};

#endif

//amrjpeg

#if  0//defined(_CAM_SMALL_MEM_) ||defined( _CAM_SPECIAL_GC6113_)
const INT32 G_VppAmrJpegEncCode[] MICRON_ALIGN(32)=
{
#include "amrjpegenc_qvga.tab"
};
#else
const INT32 G_VppAmrJpegEncCode[] MICRON_ALIGN(32)=
{
#include "vpp_audiojpegenc_asm_amrjpegenc.tab"
};
#endif


const INT32 G_VppAmrJpegEncConstX[] MICRON_ALIGN(32)=
{
#include "vpp_amrjpeg_enc_const_x.tab"
};

const INT32 G_VppAmrJpegEncConstY[] MICRON_ALIGN(32)=
{
#include "vpp_amrjpeg_enc_const_y.tab"
};

const INT32 G_vocYUVRomateCode[] MICRON_ALIGN(32)=
{
#include "yuvrotate.tab"
};

PROTECTED CONST INT32 G_VppAudJpegMainCodeSize MICRON_ALIGN(32)= sizeof(G_VppCommonEncCode);

#endif


