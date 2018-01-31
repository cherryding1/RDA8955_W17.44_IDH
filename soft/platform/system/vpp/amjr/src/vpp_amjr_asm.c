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
#include "vppp_amjr_asm_map.h"


#define RAM_X_BEGIN_ADDR               0x0000
#define RAM_Y_BEGIN_ADDR               0x4000

#define TO32(lo,hi)                    ((lo & 0xffff) | ((hi &0xffff) << 16))



// ******************************
//            COMMON
// ******************************

CONST INT32 g_vppAmjrCommonCode[] MICRON_ALIGN(32)=
{
#include "vpp_amjr_asm_common.tab"
};

CONST INT32 g_vppAmjrCommonCodeSize MICRON_ALIGN(32)= sizeof(g_vppAmjrCommonCode);


// ******************************
//            JPEG
// ******************************

CONST INT32 g_vppAmjrJpegCode[] MICRON_ALIGN(32)=
{
#include "vpp_amjr_asm_jpeg.tab"
};

CONST INT32 g_vppAmjrJpegConstY[] MICRON_ALIGN(32)=
{
#include "vpp_amjr_const_jpeg_y.tab"
};

// ******************************
//            AMR
// ******************************

CONST INT32 g_vppAmjrAmrCode[] MICRON_ALIGN(32)=
{
#include "vpp_amjr_asm_amr.tab"
};

CONST INT32 g_vppAmjrAmrConstX[] MICRON_ALIGN(32)=
{
#include "vpp_amjr_const_amr_x.tab"
};

CONST INT32 g_vppAmjrAmrConstY[] MICRON_ALIGN(32)=
{
#include "vpp_amjr_const_amr_y.tab"
};

// ******************************
//            MP3
// ******************************

CONST INT32 g_vppAmjrMp3Code[] MICRON_ALIGN(32)=
{
#include "vpp_amjr_asm_mp3.tab"
};

CONST INT32 g_vppAmjrMp3ConstX[] MICRON_ALIGN(32)=
{
#include "vpp_amjr_const_mp3_x.tab"
};

CONST INT32 g_vppAmjrMp3ConstY[] MICRON_ALIGN(32)=
{
#include "vpp_amjr_const_mp3_y.tab"
};
