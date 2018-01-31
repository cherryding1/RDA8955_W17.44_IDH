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
#include "vppp_amjp_asm_map.h"

#define RAM_X_BEGIN_ADDR               0x0000
#define RAM_Y_BEGIN_ADDR               0x4000

#define TO32(lo,hi)                    ((lo & 0xffff) | ((hi &0xffff) << 16))


// ******************************
//            COMMON
// ******************************

CONST INT32 g_vppAmjpCommonCode[] MICRON_ALIGN(32)=
{
#include "vpp_amjp_asm_common.tab"
};

CONST INT32 g_vppAmjpCommonCodeSize MICRON_ALIGN(32)= sizeof(g_vppAmjpCommonCode);

// ******************************
//            JPEG
// ******************************

CONST INT32 g_vppAmjpJpegCode[] MICRON_ALIGN(32)=
{
#include "vpp_amjp_asm_jpeg.tab"
};

CONST INT32 g_vppAmjpJpegConstY[] MICRON_ALIGN(32)=
{
#include "vpp_amjp_const_jpeg_y.tab"
};

// ******************************
//            MP12
// ******************************

CONST INT32 g_vppAmjpMp12Code[] MICRON_ALIGN(32)=
{
#include "vpp_amjp_asm_mp12.tab"
};

CONST INT32 g_vppAmjpMp12ConstY[] MICRON_ALIGN(32)=
{
#include "vpp_amjp_const_mp12_y.tab"
};

// ******************************
//            MP3
// ******************************

CONST INT32 g_vppAmjpMp3Code[] MICRON_ALIGN(32)=
{
#include "vpp_amjp_asm_mp3.tab"
};

CONST INT32 g_vppAmjpMp3ConstX[] MICRON_ALIGN(32)=
{
#include "vpp_amjp_const_mp3_x.tab"
};

CONST INT32 g_vppAmjpMp3ConstY[] MICRON_ALIGN(32)=
{
#include "vpp_amjp_const_mp3_y.tab"
};

// ******************************
//            AMR
// ******************************

CONST INT32 g_vppAmjpAmrCode[] MICRON_ALIGN(32)=
{
#include "vpp_amjp_asm_amr.tab"
};

CONST INT32 g_vppAmjpAmrConstY[] MICRON_ALIGN(32)=
{
#include "vpp_amjp_const_amr_y.tab"
};

// ******************************
//            AAC
// ******************************

CONST INT32 g_vppAmjpAacCode[] MICRON_ALIGN(32)=
{
#include "vpp_amjp_asm_aac.tab"
};

CONST INT32 g_vppAmjpAacConstY[] MICRON_ALIGN(32)=
{
#include "vpp_amjp_const_aac_y.tab"
};

// ******************************
//            ZOOM
// ******************************

CONST INT32 g_vppAmjpZoomCode[] MICRON_ALIGN(32)=
{
#include "vpp_amjp_asm_zoom.tab"
};

CONST INT32 g_vppAmjpZoomConstY[] MICRON_ALIGN(32)=
{
#include "vpp_amjp_const_zoom_y.tab"
};

// ******************************
//            EQUALIZER
// ******************************

CONST INT16 g_vppAmjpConstAudioEq[] MICRON_ALIGN(32)=
{
#include "vpp_amjp_const_audio_eq.tab"
};

