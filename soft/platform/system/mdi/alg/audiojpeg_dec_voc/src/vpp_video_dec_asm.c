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
















#ifndef vpp_video_DEC_ASM_C
#define vpp_video_DEC_ASM_C

#include "cs_types.h"
/*
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

const INT32 G_VppAmrDecCode[] MICRON_ALIGN(32)=
{
#include "amrdec.tab"
};

const INT32 G_VppMp3Dec_EQ_DMA_ConstY[] MICRON_ALIGN(32)=
{
#include "mp3decequalizer.tab" // mp3 eq
};

const INT32 G_VppAmrDecConstY[] MICRON_ALIGN(32)=
{
#include "vpp_amr_dec_const_y.tab"
};
*/
//jpeg

//#ifdef VIDEO_PLAYER_SUPPORT
#if 0
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

/*
const INT32 G_VppJpegDec2Consty[] =
{
#include "vpp_jpeg_dec2_const_y.tab"
};*/

const INT32 G_VppMpeg4DecCode[] MICRON_ALIGN(32)=
{
#include "mpeg4dec.tab"
};


const INT32 G_VppMpeg4DecConsty[] MICRON_ALIGN(32)=
{
#include "vpp_mpeg4_dec_const_y.tab"
};

#endif
#endif
