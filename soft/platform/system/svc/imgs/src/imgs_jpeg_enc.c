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

#include "imgsp_jpeg_enc.h"

#ifndef SHEEN_VC_DEBUG
#include "cs_types.h"

#include "string.h"

#include "fs.h"
#include "sxr_mem.h"

#include "imgsp_debug.h"
#else
#include "stdio.h"
#endif

// Standard quantization tables for luminance and chrominance. Scaled version
// of these are used by the encoder for a given quality.
// These tables come from the IJG code, which takes them from the JPeg specs,
// and are generic quantization tables that give good results on most images.
//

//data array/////////////
CONST INT16 g_imgsHuffmanDc0[24]= //[leng][code]
{

    0x2 ,0x0 ,
    0x3 ,0x2 ,
    0x3 ,0x3 ,
    0x3 ,0x4 ,
    0x3 ,0x5 ,
    0x3 ,0x6 ,
    0x4 ,0xe ,
    0x5 ,0x1e ,
    0x6 ,0x3e ,
    0x7 ,0x7e ,
    0x8 ,0xfe ,
    0x9 ,0x1fe

};

CONST INT16 g_imgsHuffmanDc1[24]= //[leng][code]
{
    0x2 ,0x0 ,
    0x2 ,0x1 ,
    0x2 ,0x2 ,
    0x3 ,0x6 ,
    0x4 ,0xe ,
    0x5 ,0x1e,
    0x6 ,0x3e,
    0x7 ,0x7e,
    0x8 ,0xfe,
    0x9 ,0x1fe,
    0xa ,0x3fe,
    0xb ,0x7fe
};

CONST UINT16 g_imgsHuffmanAc0[324]= //[leng][code]
{
    0x4 ,0xa ,  //00
    0xb ,0x7f9 ,//F0

    0x2 ,0x0 ,
    0x2 ,0x1 ,
    0x3 ,0x4 ,
    0x4 ,0xb ,
    0x5 ,0x1a ,
    0x7 ,0x78 ,
    0x8 ,0xf8 ,
    0xa ,0x3f6 ,
    0x10 ,0xff82 ,
    0x10 ,0xff83 ,

    0x4 ,0xc ,
    0x5 ,0x1b ,
    0x7 ,0x79 ,
    0x9 ,0x1f6 ,
    0xb ,0x7f6 ,
    0x10 ,0xff84 ,
    0x10 ,0xff85 ,
    0x10 ,0xff86 ,
    0x10 ,0xff87 ,
    0x10 ,0xff88 ,

    0x5 ,0x1c ,
    0x8 ,0xf9 ,
    0xa ,0x3f7 ,
    0xc ,0xff4 ,
    0x10 ,0xff89 ,
    0x10 ,0xff8a ,
    0x10 ,0xff8b ,
    0x10 ,0xff8c ,
    0x10 ,0xff8d ,
    0x10 ,0xff8e ,


    0x6 ,0x3a ,
    0x9 ,0x1f7 ,
    0xc ,0xff5 ,
    0x10 ,0xff8f ,
    0x10 ,0xff90 ,
    0x10 ,0xff91 ,
    0x10 ,0xff92 ,
    0x10 ,0xff93 ,
    0x10 ,0xff94 ,
    0x10 ,0xff95 ,

    0x6 ,0x3b ,
    0xa ,0x3f8 ,
    0x10 ,0xff96 ,
    0x10 ,0xff97 ,
    0x10 ,0xff98 ,
    0x10 ,0xff99 ,
    0x10 ,0xff9a ,
    0x10 ,0xff9b ,
    0x10 ,0xff9c ,
    0x10 ,0xff9d ,

    0x7 ,0x7a ,
    0xb ,0x7f7 ,
    0x10 ,0xff9e ,
    0x10 ,0xff9f ,
    0x10 ,0xffa0 ,
    0x10 ,0xffa1 ,
    0x10 ,0xffa2 ,
    0x10 ,0xffa3 ,
    0x10 ,0xffa4 ,
    0x10 ,0xffa5 ,

    0x7 ,0x7b ,
    0xc ,0xff6 ,
    0x10 ,0xffa6 ,
    0x10 ,0xffa7 ,
    0x10 ,0xffa8 ,
    0x10 ,0xffa9 ,
    0x10 ,0xffaa ,
    0x10 ,0xffab ,
    0x10 ,0xffac ,
    0x10 ,0xffad ,

    0x8 ,0xfa ,
    0xc ,0xff7 ,
    0x10 ,0xffae ,
    0x10 ,0xffaf ,
    0x10 ,0xffb0 ,
    0x10 ,0xffb1 ,
    0x10 ,0xffb2 ,
    0x10 ,0xffb3 ,
    0x10 ,0xffb4 ,
    0x10 ,0xffb5 ,

    0x9 ,0x1f8 ,
    0xf ,0x7fc0 ,
    0x10 ,0xffb6 ,
    0x10 ,0xffb7 ,
    0x10 ,0xffb8 ,
    0x10 ,0xffb9 ,
    0x10 ,0xffba ,
    0x10 ,0xffbb ,
    0x10 ,0xffbc ,
    0x10 ,0xffbd ,

    0x9 ,0x1f9 ,
    0x10 ,0xffbe ,
    0x10 ,0xffbf ,
    0x10 ,0xffc0 ,
    0x10 ,0xffc1 ,
    0x10 ,0xffc2 ,
    0x10 ,0xffc3 ,
    0x10 ,0xffc4 ,
    0x10 ,0xffc5 ,
    0x10 ,0xffc6 ,

    0x9 ,0x1fa ,
    0x10 ,0xffc7 ,
    0x10 ,0xffc8 ,
    0x10 ,0xffc9 ,
    0x10 ,0xffca ,
    0x10 ,0xffcb ,
    0x10 ,0xffcc ,
    0x10 ,0xffcd ,
    0x10 ,0xffce ,
    0x10 ,0xffcf ,

    0xa ,0x3f9 ,
    0x10 ,0xffd0 ,
    0x10 ,0xffd1 ,
    0x10 ,0xffd2 ,
    0x10 ,0xffd3 ,
    0x10 ,0xffd4 ,
    0x10 ,0xffd5 ,
    0x10 ,0xffd6 ,
    0x10 ,0xffd7 ,
    0x10 ,0xffd8 ,

    0xa ,0x3fa ,
    0x10 ,0xffd9 ,
    0x10 ,0xffda ,
    0x10 ,0xffdb ,
    0x10 ,0xffdc ,
    0x10 ,0xffdd ,
    0x10 ,0xffde ,
    0x10 ,0xffdf ,
    0x10 ,0xffe0 ,
    0x10 ,0xffe1 ,

    0xb ,0x7f8 ,
    0x10 ,0xffe2 ,
    0x10 ,0xffe3 ,
    0x10 ,0xffe4 ,
    0x10 ,0xffe5 ,
    0x10 ,0xffe6 ,
    0x10 ,0xffe7 ,
    0x10 ,0xffe8 ,
    0x10 ,0xffe9 ,
    0x10 ,0xffea ,

    0x10 ,0xffeb ,
    0x10 ,0xffec ,
    0x10 ,0xffed ,
    0x10 ,0xffee ,
    0x10 ,0xffef ,
    0x10 ,0xfff0 ,
    0x10 ,0xfff1 ,
    0x10 ,0xfff2 ,
    0x10 ,0xfff3 ,
    0x10 ,0xfff4 ,




    0x10 ,0xfff5 ,
    0x10 ,0xfff6 ,
    0x10 ,0xfff7 ,
    0x10 ,0xfff8 ,
    0x10 ,0xfff9 ,
    0x10 ,0xfffa ,
    0x10 ,0xfffb ,
    0x10 ,0xfffc ,
    0x10 ,0xfffd ,
    0x10 ,0xfffe
};

CONST UINT16  g_imgsHuffmanAc1[324]=
{
    0x2 ,0x0 ,  //00
    0xa ,0x3fa ,//F0

    0x2 ,0x1 ,
    0x3 ,0x4 ,
    0x4 ,0xa ,
    0x5 ,0x18 ,
    0x5 ,0x19 ,
    0x6 ,0x38 ,
    0x7 ,0x78 ,
    0x9 ,0x1f4 ,
    0xa ,0x3f6 ,
    0xc ,0xff4 ,


    0x4 ,0xb ,
    0x6 ,0x39 ,
    0x8 ,0xf6 ,
    0x9 ,0x1f5 ,
    0xb ,0x7f6 ,
    0xc ,0xff5 ,
    0x10 ,0xff88 ,
    0x10 ,0xff89 ,
    0x10 ,0xff8a ,
    0x10 ,0xff8b ,


    0x5 ,0x1a ,
    0x8 ,0xf7 ,
    0xa ,0x3f7 ,
    0xc ,0xff6 ,
    0xf ,0x7fc2 ,
    0x10 ,0xff8c ,
    0x10 ,0xff8d ,
    0x10 ,0xff8e ,
    0x10 ,0xff8f ,
    0x10 ,0xff90 ,


    0x5 ,0x1b ,
    0x8 ,0xf8 ,
    0xa ,0x3f8 ,
    0xc ,0xff7 ,
    0x10 ,0xff91 ,
    0x10 ,0xff92 ,
    0x10 ,0xff93 ,
    0x10 ,0xff94 ,
    0x10 ,0xff95 ,
    0x10 ,0xff96 ,


    0x6 ,0x3a ,
    0x9 ,0x1f6 ,
    0x10 ,0xff97 ,
    0x10 ,0xff98 ,
    0x10 ,0xff99 ,
    0x10 ,0xff9a ,
    0x10 ,0xff9b ,
    0x10 ,0xff9c ,
    0x10 ,0xff9d ,
    0x10 ,0xff9e ,


    0x6 ,0x3b ,
    0xa ,0x3f9 ,
    0x10 ,0xff9f ,
    0x10 ,0xffa0 ,
    0x10 ,0xffa1 ,
    0x10 ,0xffa2 ,
    0x10 ,0xffa3 ,
    0x10 ,0xffa4 ,
    0x10 ,0xffa5 ,
    0x10 ,0xffa6 ,


    0x7 ,0x79 ,
    0xb ,0x7f7 ,
    0x10 ,0xffa7 ,
    0x10 ,0xffa8 ,
    0x10 ,0xffa9 ,
    0x10 ,0xffaa ,
    0x10 ,0xffab ,
    0x10 ,0xffac ,
    0x10 ,0xffad ,
    0x10 ,0xffae ,


    0x7 ,0x7a ,
    0xb ,0x7f8 ,
    0x10 ,0xffaf ,
    0x10 ,0xffb0 ,
    0x10 ,0xffb1 ,
    0x10 ,0xffb2 ,
    0x10 ,0xffb3 ,
    0x10 ,0xffb4 ,
    0x10 ,0xffb5 ,
    0x10 ,0xffb6 ,


    0x8 ,0xf9 ,
    0x10 ,0xffb7 ,
    0x10 ,0xffb8 ,
    0x10 ,0xffb9 ,
    0x10 ,0xffba ,
    0x10 ,0xffbb ,
    0x10 ,0xffbc ,
    0x10 ,0xffbd ,
    0x10 ,0xffbe ,
    0x10 ,0xffbf ,


    0x9 ,0x1f7 ,
    0x10 ,0xffc0 ,
    0x10 ,0xffc1 ,
    0x10 ,0xffc2 ,
    0x10 ,0xffc3 ,
    0x10 ,0xffc4 ,
    0x10 ,0xffc5 ,
    0x10 ,0xffc6 ,
    0x10 ,0xffc7 ,
    0x10 ,0xffc8 ,


    0x9 ,0x1f8 ,
    0x10 ,0xffc9 ,
    0x10 ,0xffca ,
    0x10 ,0xffcb ,
    0x10 ,0xffcc ,
    0x10 ,0xffcd ,
    0x10 ,0xffce ,
    0x10 ,0xffcf ,
    0x10 ,0xffd0 ,
    0x10 ,0xffd1 ,


    0x9 ,0x1f9 ,
    0x10 ,0xffd2 ,
    0x10 ,0xffd3 ,
    0x10 ,0xffd4 ,
    0x10 ,0xffd5 ,
    0x10 ,0xffd6 ,
    0x10 ,0xffd7 ,
    0x10 ,0xffd8 ,
    0x10 ,0xffd9 ,
    0x10 ,0xffda ,


    0x9 ,0x1fa ,
    0x10 ,0xffdb ,
    0x10 ,0xffdc ,
    0x10 ,0xffdd ,
    0x10 ,0xffde ,
    0x10 ,0xffdf ,
    0x10 ,0xffe0 ,
    0x10 ,0xffe1 ,
    0x10 ,0xffe2 ,
    0x10 ,0xffe3 ,


    0xb ,0x7f9 ,
    0x10 ,0xffe4 ,
    0x10 ,0xffe5 ,
    0x10 ,0xffe6 ,
    0x10 ,0xffe7 ,
    0x10 ,0xffe8 ,
    0x10 ,0xffe9 ,
    0x10 ,0xffea ,
    0x10 ,0xffeb ,
    0x10 ,0xffec ,


    0xe ,0x3fe0 ,
    0x10 ,0xffed ,
    0x10 ,0xffee ,
    0x10 ,0xffef ,
    0x10 ,0xfff0 ,
    0x10 ,0xfff1 ,
    0x10 ,0xfff2 ,
    0x10 ,0xfff3 ,
    0x10 ,0xfff4 ,
    0x10 ,0xfff5 ,






    0xf ,0x7fc3 ,
    0x10 ,0xfff6 ,
    0x10 ,0xfff7 ,
    0x10 ,0xfff8 ,
    0x10 ,0xfff9 ,
    0x10 ,0xfffa ,
    0x10 ,0xfffb ,
    0x10 ,0xfffc ,
    0x10 ,0xfffd ,
    0x10 ,0xfffe

};

CONST INT32 g_imgsLCQuantTable0[128]=
{
    0x1000 ,0x1745 ,0x1555 ,0x1249 ,0x1555 ,0x1999 ,0x1000 ,0x1249 ,
    0x13b1 ,0x1249 ,0xe38 ,0xf0f ,0x1000 ,0xd79 ,0xaaa ,0x666 ,
    0x9d8 ,0xaaa ,0xba2 ,0xba2 ,0xaaa ,0x539 ,0x750 ,0x6eb ,
    0x8d3 ,0x666 ,0x469 ,0x505 ,0x432 ,0x444 ,0x47d ,0x505 ,
    0x492 ,0x4a7 ,0x400 ,0x38e ,0x2c8 ,0x348 ,0x400 ,0x3c3 ,
    0x2f1 ,0x3b5 ,0x4a7 ,0x492 ,0x333 ,0x259 ,0x329 ,0x2f1 ,
    0x2b1 ,0x29c ,0x27c ,0x276 ,0x27c ,0x421 ,0x353 ,0x243 ,
    0x21d ,0x249 ,0x28f ,0x222 ,0x2c8 ,0x288 ,0x27c ,0x295 ,
    0xf0f ,0xe38 ,0xe38 ,0xaaa ,0xc30 ,0xaaa ,0x572 ,0x9d8 ,
    0x9d8 ,0x572 ,0x295 ,0x3e0 ,0x492 ,0x3e0 ,0x295 ,0x295 ,
    0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,
    0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,
    0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,
    0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,
    0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,
    0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295 ,0x295

};
CONST INT32 g_imgsLCQuantTable1[128]=
{
    0x8000 ,0x10000 ,0x10000 ,0x10000 ,0x10000 ,0x10000 ,0x8000 ,0x10000 ,
    0x10000 ,0x10000 ,0x8000 ,0x8000 ,0x8000 ,0x8000 ,0x8000 ,0x4000 ,
    0x5555 ,0x8000 ,0x8000 ,0x8000 ,0x8000 ,0x3333 ,0x4000 ,0x4000 ,
    0x5555 ,0x4000 ,0x2aaa ,0x3333 ,0x2aaa ,0x2aaa ,0x2aaa ,0x3333 ,
    0x2aaa ,0x2aaa ,0x2aaa ,0x2492 ,0x1c71 ,0x2000 ,0x2aaa ,0x2492 ,
    0x1c71 ,0x2492 ,0x2aaa ,0x2aaa ,0x2000 ,0x1745 ,0x2000 ,0x1c71 ,
    0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x2aaa ,0x2000 ,0x1745 ,
    0x1555 ,0x1745 ,0x1999 ,0x1555 ,0x1c71 ,0x1999 ,0x1999 ,0x1999 ,
    0x8000 ,0x8000 ,0x8000 ,0x8000 ,0x8000 ,0x8000 ,0x3333 ,0x5555 ,
    0x5555 ,0x3333 ,0x1999 ,0x2492 ,0x2aaa ,0x2492 ,0x1999 ,0x1999 ,
    0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,
    0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,
    0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,
    0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,
    0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,
    0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999 ,0x1999

};

CONST UINT8 g_imgsInverseZigZag[64]=
{
    0,1,8,16,9,2,3,10,
    17,24,32,25,18,11,4,5,
    12,19,26,33,40,48,41,34,
    27,20,13,6,7,14,21,28,
    35,42,49,56,57,50,43,36,
    29,22,15,23,30,37,44,51,
    58,59,52,45,38,31,39,46,
    53,60,61,54,47,55,62,63
};

CONST UINT8 g_imgsHead1[20]=
{
    0xff,0xd8,0xff,0xe0,0x0,0x10,0x4a,0x46,0x49,0x46,0x0,0x1,0x1,
    0x0,0x0,0x1,0x0,0x1,0x0,0x0
};

CONST UINT8 g_imgsHeadQL[136]=
{
    0xff,0xdb,0x0,0x84,0x0,0x10,0xb,0xc,0xe,0xc,0xa,0x10,0xe,0xd,0xe,
    0x12,0x11,0x10,0x13,0x18,0x28,0x1a,0x18,0x16,0x16,0x18,0x31,0x23,
    0x25,0x1d,0x28,0x3a,0x33,0x3d,0x3c,0x39,0x33,0x38,0x37,0x40,0x48,
    0x5c,0x4e,0x40,0x44,0x57,0x45,0x37,0x38,0x50,0x6d,0x51,0x57,0x5f,
    0x62,0x67,0x68,0x67,0x3e,0x4d,0x71,0x79,0x70,0x64,0x78,0x5c,0x65,
    0x67,0x63,0x1,0x11,0x12,0x12,0x18,0x15,0x18,0x2f,0x1a,0x1a,0x2f,
    0x63,0x42,0x38,0x42,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,
    0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,
    0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,
    0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,
    0x63,0x63,0xff,0xc0
};

CONST UINT8 g_imgsHeadQH[136]=
{
    0xff,0xdb,0x0,0x84,0x0,0x2,0x1,0x1,0x1,0x1,0x1,0x2,0x1,0x1,0x1,
    0x2,0x2,0x2,0x2,0x2,0x4,0x3,0x2,0x2,0x2,0x2,0x5,0x4,0x4,0x3,
    0x4,0x6,0x5,0x6,0x6,0x6,0x5,0x6,0x6,0x6,0x7,0x9,0x8,0x6,0x7,
    0x9,0x7,0x6,0x6,0x8,0xb,0x8,0x9,0xa,0xa,0xa,0xa,0xa,0x6,0x8,
    0xb,0xc,0xb,0xa,0xc,0x9,0xa,0xa,0xa,0x1,0x2,0x2,0x2,0x2,0x2,
    0x2,0x5,0x3,0x3,0x5,0xa,0x7,0x6,0x7,0xa,0xa,0xa,0xa,0xa,0xa,
    0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,
    0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,
    0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xff,0xc0
};

PROTECTED UINT8 g_imgsHeadSize128411[10]=
{
    0x0,0x11,0x8,0x0,0x60,0x0,0x80,0x3,0x1,0x22,
};


CONST UINT8 g_imgsHead4[441]=
{
    0x0,0x2,0x11,0x1,0x3,0x11,0x1,
    0xff,0xc4,0x1,0xa2,0x0,0x0,0x1,0x5,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0x10,0x0,0x2,0x1,0x3,0x3,0x2,0x4,
    0x3,0x5,0x5,0x4,0x4,0x0,0x0,0x1,0x7d,0x1,0x2,0x3,0x0,0x4,0x11,0x5,0x12,0x21,0x31,0x41,
    0x6,0x13,0x51,0x61,0x7,0x22,0x71,0x14,0x32,0x81,0x91,0xa1,0x8,0x23,0x42,0xb1,0xc1,0x15,
    0x52,0xd1,0xf0,0x24,0x33,0x62,0x72,0x82,0x9,0xa,0x16,0x17,0x18,0x19,0x1a,0x25,0x26,0x27,
    0x28,0x29,0x2a,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
    0x4a,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,
    0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x92,
    0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,
    0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,
    0xca,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,
    0xe8,0xe9,0xea,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0x1,0x0,0x3,0x1,0x1,
    0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,
    0x9,0xa,0xb,0x11,0x0,0x2,0x1,0x2,0x4,0x4,0x3,0x4,0x7,0x5,0x4,0x4,0x0,0x1,0x2,0x77,0x0,
    0x1,0x2,0x3,0x11,0x4,0x5,0x21,0x31,0x6,0x12,0x41,0x51,0x7,0x61,0x71,0x13,0x22,0x32,0x81,
    0x8,0x14,0x42,0x91,0xa1,0xb1,0xc1,0x9,0x23,0x33,0x52,0xf0,0x15,0x62,0x72,0xd1,0xa,0x16,
    0x24,0x34,0xe1,0x25,0xf1,0x17,0x18,0x19,0x1a,0x26,0x27,0x28,0x29,0x2a,0x35,0x36,0x37,
    0x38,0x39,0x3a,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x53,0x54,0x55,0x56,0x57,0x58,
    0x59,0x5a,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x73,0x74,0x75,0x76,0x77,0x78,0x79,
    0x7a,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x92,0x93,0x94,0x95,0x96,0x97,0x98,
    0x99,0x9a,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,
    0xb8,0xb9,0xba,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xd2,0xd3,0xd4,0xd5,0xd6,
    0xd7,0xd8,0xd9,0xda,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xf2,0xf3,0xf4,0xf5,
    0xf6,0xf7,0xf8,0xf9,0xfa,0xff,0xda,0x0,0xc,0x3,0x1,0x0,0x2,0x11,0x3,0x11,0x0,0x3f,0x0
};






//global variable//////////
PRIVATE UINT8* chunk;
PRIVATE CHAR * chunkptr;
PRIVATE CHAR * chunk_end;
PRIVATE INT32 current_byte = 0;
PRIVATE INT32 current_bit = 8;
PRIVATE INT32 dc_y, dc_cb, dc_cr;
PRIVATE CONST INT32*  quant_table=NULL;
PRIVATE UINT32 outputlen=0;
PRIVATE INT32 fileHandle=0;
PRIVATE UINT8 *pJpgOut =NULL;//sheen
PRIVATE UINT8 JPGENC_BUF_MODE;//file or buffer output. 0=file mode, 1=buffer mode.sheen
PRIVATE UINT8 gCameraCreateFileFlag=0;



PRIVATE VOID _jpeg_putw(INT32 w);
PRIVATE VOID _jpeg_putc(INT32 c);
PRIVATE VOID write_header(INT16 quality_select ,INT16 width,INT16 height);
PRIVATE VOID apply_fdct(INT16 *data);
PRIVATE VOID format_number(INT32 num, INT32 *category, INT32 *bits);
PRIVATE VOID _jpeg_flush_bits(VOID);
PRIVATE VOID put_bits(INT32 value, INT32 num_bits);
PRIVATE INT32 encode_pass(INT8 *Buff,INT16 ImgWidth,INT16 ImgHeight);
PRIVATE INT32 encode_block(INT16 *block, INT32 type, INT32 *old_dc);
PRIVATE VOID  Init_GlobalVar(/*FILE* fp_out*/);





//input stream/////////////

//UINT8  ouput_ref[]={
//#include "VGAREG.tab"
//};
//////////////////////////////////function api
//optimized by zouying   2007-9-7
//remove some big tabs
/////////////////////////////////

//summury of the program////////
//const table: 1240 SHORTs -> 2480bytes
//temp  ram:   64*4 = 384 SHORTs  -> 768bytes
//input  buf:   1280*16*2 = 40960bytes
//output buf:   1024bytes
//jpeg size : 218632bytes
//bmp  size : 2621440bytes


PRIVATE VOID  Init_GlobalVar(/*FILE* fp_out*/)
{
    ///init global variable/////////////////
    //pout = fp_out;
    dc_y = dc_cb = dc_cr = 0;
    current_byte = 0;
    current_bit = 8;
    chunkptr = chunk;
    chunk_end = chunk + TEMPOUTBUFSIZE;
    outputlen=0;
}

#ifndef SHEEN_VC_DEBUG
PROTECTED IMGS_ERR_T  imgs_JpegEncode(IMGS_FBW_T* inFbw, PCSTR filename,
                                      IMGS_ENCODING_PARAMS_T* encParams,
                                      IMGS_ENC_CALLBACK_T     callback)
{
    INT16 * Buff;
    INT32 i;
    INT32 j;
    INT32 interval=16;
    // Start of the region of interest, inside the frame buffer.
    INT16* InBuff = inFbw->fb.buffer + (inFbw->fb.width)*inFbw->roi.y + inFbw->roi.x;


    UINT32 fixMeOutLen;
    UINT32* outlen = &fixMeOutLen;
    JPGENC_BUF_MODE=0;

    IMGS_TRACE(TSTDOUT, 0,"MMC_JpegEncode\n");

    if(inFbw->roi.width%8!=0 ||inFbw->roi.height%8!=0 )
    {
        IMGS_TRACE(TSTDOUT, 0,"MMC_JpegEncode,%8 return,inFbw->roi.width is %d,inFbw->roi.height is %d\n",inFbw->roi.width,inFbw->roi.height);
        return IMGS_ERR_WRONG_SIZE;
    }

    gCameraCreateFileFlag = 1;
    fileHandle = FS_Open((UINT8*)filename, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0);
    gCameraCreateFileFlag = 0;

    if(fileHandle>=0)
    {
        IMGS_TRACE(TSTDOUT, 0,"success to creat  file, name: %s, handle: %d\n", filename, fileHandle);
    }
    else
    {
        IMGS_TRACE(TSTDOUT, 0,"error,fail to create file");
        return IMGS_ERR_FILE_CREATE;
    }

    chunk = (UINT8*)sxr_Malloc(TEMPOUTBUFSIZE);

    Init_GlobalVar(/*fp_out*/);
    IMGS_TRACE(TSTDOUT, 0,"MMC_JpegEncode,1111\n");

    write_header(encParams->jpg.quality,inFbw->roi.width,inFbw->roi.height);
    IMGS_TRACE(TSTDOUT, 0,"MMC_JpegEncode,2222\n");

    Buff = (INT16*)sxr_Malloc(inFbw->roi.width*2*16);

    if(inFbw->roi.width%16!=0 || inFbw->roi.height%16!=0)
    {
        interval=8;
    }

    for (i=0; i<(inFbw->roi.height/interval); i++)
    {
        // TODO: avoid unneeded copies
        // read region of interest of buf(and copy it :()
        for (j=0 ; j<interval; j++)
        {
            // Buff is a UINT16*
            memcpy(Buff + inFbw->roi.width*j, InBuff+inFbw->fb.width*(j+i*interval), inFbw->roi.width*2);
        }

        encode_pass((CHAR*)Buff,inFbw->roi.width,inFbw->roi.height);
    }
    IMGS_TRACE(TSTDOUT, 0,"MMC_JpegEncode,444\n");


    _jpeg_flush_bits();
    _jpeg_putw(CHUNK_EOI);
    if((UINT32)chunkptr != (UINT32)(chunk + TEMPOUTBUFSIZE))
    {
        //fwrite(chunk,(UINT32)chunkptr - (UINT32)chunk,1,pout);
        FS_Write(fileHandle, chunk, (UINT32)chunkptr - (UINT32)chunk);
        outputlen+=(UINT32)chunkptr - (UINT32)chunk;
        outputlen+=607;
    }
    else
    {
        outputlen+=607;
    }

    *outlen = outputlen;
    IMGS_TRACE(TSTDOUT, 0,"MMC_JpegEncode,555\n");

    IMGS_TRACE(TSTDOUT, 0,"MMC_JpegEncode,666\n");

    if (FS_Close(fileHandle) < 0)
    {
        IMGS_TRACE(TSTDOUT, 0,"fail to close file!handle: %d \n", fileHandle);
        return IMGS_ERR_FILE_CLOSE;
    }
    else
    {
        IMGS_TRACE(TSTDOUT, 0,"encode ok and close file\n");
    }

    // Add discrimination file/buffer
    if (callback)
    {
        callback(NULL, outputlen);
    }

    return IMGS_ERR_NO;
}
#endif

/*for bufer IN&OUT encoder.sheen
input YUV 420.
quality: 0=low,1=high
imgWidth/imgHeight must be multiple of 16.
bufOut:output buffer. should big enough for one jpg picture.
outSize:encode jpeg size in bufOut.
*/
IMGS_ERR_T  imgs_JpegEncodeBufYUV420(UINT8* Y, UINT8* U, UINT8* V, UINT16 quality, UINT16 imgWidth, UINT16 imgHeight, UINT8* bufOut, UINT32* outSize)
{
    INT32 i;
    INT32 j;
    INT32 k;
    INT32 interval=16;
    INT16 block0[64], block1[64];// two 8x8 temp
    UINT8 *y_ptr, *u_ptr, *v_ptr;
    UINT8 *y16x16_ptr;
    UINT8 *u8x8_ptr, *v8x8_ptr;
    JPGENC_BUF_MODE=1;
    chunk=NULL;

    IMGS_TRACE(TSTDOUT, 0,"MMC_JpegEncodeBuf\n");

    if(!Y || !U || !V || !bufOut || !outSize)
    {
        IMGS_TRACE(TSTDOUT, 0,"MMC_JpegEncodeBuf bad input parameters error! \n");
        return IMGS_ERR_INPUT_PARAMETERS;
    }

    //check for yuv420
    if(imgWidth%16!=0 ||imgHeight%16!=0 )
    {
        IMGS_TRACE(TSTDOUT, 0,"MMC_JpegEncodeBuf error! imgWidth=%d imgHeight=%d not multiple of 16!\n", imgWidth, imgHeight);
        return IMGS_ERR_WRONG_SIZE;
    }

    chunk = (UINT8*)sxr_Malloc(TEMPOUTBUFSIZE);
    if(!chunk)
    {
        IMGS_TRACE(TSTDOUT, 0,"MMC_JpegEncodeBuf chunk malloc error! \n");
        return IMGS_ERR_MALLOC;
    }

    Init_GlobalVar(/*fp_out*/);

    pJpgOut=bufOut;

    //create jpg header
    memcpy(pJpgOut, g_imgsHead1,20);
    pJpgOut+=20;

    if (quality==0)
        memcpy(pJpgOut, g_imgsHeadQL, 136);
    else
        memcpy(pJpgOut, g_imgsHeadQH, 136);
    pJpgOut+=136;

    g_imgsHeadSize128411[3]=imgHeight>>8;
    g_imgsHeadSize128411[4]=imgHeight&0xff;
    g_imgsHeadSize128411[5]=imgWidth>>8;
    g_imgsHeadSize128411[6]=imgWidth&0xff;

    if (imgWidth%16!=0) // yuv444
    {
        g_imgsHeadSize128411[9] = 0x11;
    }
    else if (imgHeight%16!=0)//yuv422
    {
        g_imgsHeadSize128411[9] = 0x21;
    }
    else//yuv420
    {
        g_imgsHeadSize128411[9] = 0x22;
    }

    memcpy(pJpgOut, g_imgsHeadSize128411, 10);
    pJpgOut+=10;

    memcpy(pJpgOut, g_imgsHead4, 441);
    pJpgOut+=441;

    if (quality==1)
    {
        quant_table = g_imgsLCQuantTable1;
    }
    else
    {
        quant_table = g_imgsLCQuantTable0;
    }
    //header.end

    if(imgWidth%16!=0 || imgHeight%16!=0)
    {
        interval=8;
    }

    for (i=0; i<imgHeight; i+=interval)
    {
        y_ptr=Y+i*imgWidth;
        u_ptr=U+i*(imgWidth>>1);
        v_ptr=V+i*(imgWidth>>1);

        for (j = 0; j < imgWidth; j += 16)
        {
            y16x16_ptr=y_ptr+j;
            u8x8_ptr=u_ptr+(j>>1);
            v8x8_ptr=v_ptr+(j>>1);

            //get 2 8x8 y block data
            for(k=0; k<64; k+=8)
            {
                //for block 0
                block0[k]=(INT16)y16x16_ptr[0]-128;
                block0[k+1]=(INT16)y16x16_ptr[1]-128;
                block0[k+2]=(INT16)y16x16_ptr[2]-128;
                block0[k+3]=(INT16)y16x16_ptr[3]-128;

                block0[k+4]=(INT16)y16x16_ptr[4]-128;
                block0[k+5]=(INT16)y16x16_ptr[5]-128;
                block0[k+6]=(INT16)y16x16_ptr[6]-128;
                block0[k+7]=(INT16)y16x16_ptr[7]-128;

                //for block 1
                block1[k]=(INT16)y16x16_ptr[8]-128;
                block1[k+1]=(INT16)y16x16_ptr[9]-128;
                block1[k+2]=(INT16)y16x16_ptr[10]-128;
                block1[k+3]=(INT16)y16x16_ptr[11]-128;

                block1[k+4]=(INT16)y16x16_ptr[12]-128;
                block1[k+5]=(INT16)y16x16_ptr[13]-128;
                block1[k+6]=(INT16)y16x16_ptr[14]-128;
                block1[k+7]=(INT16)y16x16_ptr[15]-128;

                y16x16_ptr+=imgWidth;
            }

            //y 00
            encode_block(block0, LUMINANCE, &dc_y);
            //y 01
            encode_block(block1, LUMINANCE, &dc_y);

            //get 2 8x8 y block data
            for(k=0; k<64; k+=8)
            {
                //for block 0
                block0[k]=(INT16)y16x16_ptr[0]-128;
                block0[k+1]=(INT16)y16x16_ptr[1]-128;
                block0[k+2]=(INT16)y16x16_ptr[2]-128;
                block0[k+3]=(INT16)y16x16_ptr[3]-128;

                block0[k+4]=(INT16)y16x16_ptr[4]-128;
                block0[k+5]=(INT16)y16x16_ptr[5]-128;
                block0[k+6]=(INT16)y16x16_ptr[6]-128;
                block0[k+7]=(INT16)y16x16_ptr[7]-128;

                //for block 1
                block1[k]=(INT16)y16x16_ptr[8]-128;
                block1[k+1]=(INT16)y16x16_ptr[9]-128;
                block1[k+2]=(INT16)y16x16_ptr[10]-128;
                block1[k+3]=(INT16)y16x16_ptr[11]-128;

                block1[k+4]=(INT16)y16x16_ptr[12]-128;
                block1[k+5]=(INT16)y16x16_ptr[13]-128;
                block1[k+6]=(INT16)y16x16_ptr[14]-128;
                block1[k+7]=(INT16)y16x16_ptr[15]-128;

                y16x16_ptr+=imgWidth;
            }

            //y 10
            encode_block(block0, LUMINANCE, &dc_y);
            //y 11
            encode_block(block1, LUMINANCE, &dc_y);

            //get 1 8x8 cb block data
            for(k=0; k<64; k+=8)
            {
                //for block 0
                block0[k]=(INT16)u8x8_ptr[0]-128;
                block0[k+1]=(INT16)u8x8_ptr[1]-128;
                block0[k+2]=(INT16)u8x8_ptr[2]-128;
                block0[k+3]=(INT16)u8x8_ptr[3]-128;

                block0[k+4]=(INT16)u8x8_ptr[4]-128;
                block0[k+5]=(INT16)u8x8_ptr[5]-128;
                block0[k+6]=(INT16)u8x8_ptr[6]-128;
                block0[k+7]=(INT16)u8x8_ptr[7]-128;

                u8x8_ptr+=(imgWidth>>1);
            }

            //cb
            encode_block(block0, CHROMINANCE, &dc_cb);

            //get 1 8x8 cr block data
            for(k=0; k<64; k+=8)
            {
                //for block 0
                block0[k]=(INT16)v8x8_ptr[0]-128;
                block0[k+1]=(INT16)v8x8_ptr[1]-128;
                block0[k+2]=(INT16)v8x8_ptr[2]-128;
                block0[k+3]=(INT16)v8x8_ptr[3]-128;

                block0[k+4]=(INT16)v8x8_ptr[4]-128;
                block0[k+5]=(INT16)v8x8_ptr[5]-128;
                block0[k+6]=(INT16)v8x8_ptr[6]-128;
                block0[k+7]=(INT16)v8x8_ptr[7]-128;

                v8x8_ptr+=(imgWidth>>1);
            }

            //cr
            encode_block(block0, CHROMINANCE, &dc_cr);

        }
    }

    _jpeg_flush_bits();
    _jpeg_putw(CHUNK_EOI);

    if((UINT32)chunkptr != (UINT32)(chunk + TEMPOUTBUFSIZE))
    {
        UINT32 len;
        len=(UINT32)chunkptr - (UINT32)chunk;

        memcpy(pJpgOut, chunk, len);
        pJpgOut+=len;

        outputlen+=len;
        outputlen+=607;
    }
    else
    {
        outputlen+=607;
    }

    *outSize= outputlen;
    sxr_Free(chunk);

    IMGS_TRACE(TSTDOUT, 0,"jpeg encode ok,size=%d\n", outputlen);

    return IMGS_ERR_NO;
}

/*for bufer IN&OUT encoder.sheen
bufIn: YUV 422 (YUYV).
quality: 0=low,1=high
imgWidth/imgHeight must be multiple of 16.
bufOut:output buffer. should big enough for one jpg picture.
outSize:encode jpeg size in bufOut.
*/
IMGS_ERR_T  imgs_JpegEncodeBufYUV422(UINT16* bufIn, UINT16 quality, UINT16 imgWidth, UINT16 imgHeight, UINT8* bufOut, UINT32* outSize)
{
    INT32 i;
    INT32 j;
    INT32 m;
    INT32 n;
    INT16 y1[64], y2[64], u[64], v[64];// four 8x8 temp
    UINT8 *yuyv_ptr;
    UINT8 *yuyv16x16_ptr;
    JPGENC_BUF_MODE=1;
    chunk=NULL;

    IMGS_TRACE(TSTDOUT, 0,"imgs_JpegEncodeBufYUV422\n");

    if(!bufIn || !bufOut || !outSize)
    {
        IMGS_TRACE(TSTDOUT, 0,"imgs_JpegEncodeBufYUV422 bad input parameters error! \n");
        return IMGS_ERR_INPUT_PARAMETERS;
    }

    //check for multiple of 16
    if(imgWidth%16!=0 ||imgHeight%16!=0 )
    {
        IMGS_TRACE(TSTDOUT, 0,"imgs_JpegEncodeBufYUV422 error! imgWidth=%d imgHeight=%d not multiple of 16!\n", imgWidth, imgHeight);
        return IMGS_ERR_WRONG_SIZE;
    }

    chunk = (UINT8*)sxr_Malloc(TEMPOUTBUFSIZE);
    if(!chunk)
    {
        IMGS_TRACE(TSTDOUT, 0,"MMC_JpegEncodeBuf chunk malloc error! \n");
        return IMGS_ERR_MALLOC;
    }

    Init_GlobalVar(/*fp_out*/);

    pJpgOut=bufOut;

    //create jpg header
    memcpy(pJpgOut, g_imgsHead1,20);
    pJpgOut+=20;

    if (quality==0)
        memcpy(pJpgOut, g_imgsHeadQL, 136);
    else
        memcpy(pJpgOut, g_imgsHeadQH, 136);
    pJpgOut+=136;

    g_imgsHeadSize128411[3]=imgHeight>>8;
    g_imgsHeadSize128411[4]=imgHeight&0xff;
    g_imgsHeadSize128411[5]=imgWidth>>8;
    g_imgsHeadSize128411[6]=imgWidth&0xff;
    /*
    if (imgWidth%16!=0) // yuv444
    {
               g_imgsHeadSize128411[9] = 0x11;
    }
    else if (imgHeight%16!=0)//yuv422
    */
    {
        //422 default.
        g_imgsHeadSize128411[9] = 0x21;
    }
    /*else//yuv420
    {
        g_imgsHeadSize128411[9] = 0x22;
    }*/

    memcpy(pJpgOut, g_imgsHeadSize128411, 10);
    pJpgOut+=10;

    memcpy(pJpgOut, g_imgsHead4, 441);
    pJpgOut+=441;

    if (quality==1)
    {
        quant_table = g_imgsLCQuantTable1;
    }
    else
    {
        quant_table = g_imgsLCQuantTable0;
    }
    //header.end
    /*
        if(imgWidth%16!=0 || imgHeight%16!=0)
        {
            interval=8;
        }*/

    for (i=0; i<imgHeight; i+=8)
    {
        yuyv_ptr=(UINT8*)(bufIn+i*imgWidth);

        for (j = 0; j < imgWidth; j += 16)
        {
            yuyv16x16_ptr=yuyv_ptr+(j<<1);

            //get 2 8x8 y block data
            for(m=0; m<64; m+=8)
            {
                //y1 uv
                for(n=0; n<8; n+=2)
                {
                    //y
                    y1[m+n]=(INT16)yuyv16x16_ptr[2*n]-128;
                    //u
                    u[m+(n>>1)]=(INT16)yuyv16x16_ptr[2*n+1]-128;
                    //y
                    y1[m+n+1]=(INT16)yuyv16x16_ptr[2*n+2]-128;
                    //v
                    v[m+(n>>1)]=(INT16)yuyv16x16_ptr[2*n+3]-128;

                }

                //y2 uv
                for(n=0; n<8; n+=2)
                {
                    //y
                    y2[m+n]=(INT16)yuyv16x16_ptr[2*n+16]-128;
                    //u
                    u[m+(n>>1)+4]=(INT16)yuyv16x16_ptr[2*n+17]-128;
                    //y
                    y2[m+n+1]=(INT16)yuyv16x16_ptr[2*n+18]-128;
                    //v
                    v[m+(n>>1)+4]=(INT16)yuyv16x16_ptr[2*n+19]-128;

                }

                yuyv16x16_ptr+=(imgWidth<<1);
            }

            //y1
            encode_block(y1, LUMINANCE, &dc_y);
            //y2
            encode_block(y2, LUMINANCE, &dc_y);
            //cb
            encode_block(u, CHROMINANCE, &dc_cb);
            //cr
            encode_block(v, CHROMINANCE, &dc_cr);

        }
    }

    _jpeg_flush_bits();
    _jpeg_putw(CHUNK_EOI);

    if((UINT32)chunkptr != (UINT32)(chunk + TEMPOUTBUFSIZE))
    {
        UINT32 len;
        len=(UINT32)chunkptr - (UINT32)chunk;

        memcpy(pJpgOut, chunk, len);
        pJpgOut+=len;

        outputlen+=len;
        outputlen+=607;
    }
    else
    {
        outputlen+=607;
    }

    *outSize= outputlen;
    sxr_Free(chunk);

    IMGS_TRACE(TSTDOUT, 0,"jpeg encode ok,size=%d\n", outputlen);

    return IMGS_ERR_NO;
}


PRIVATE INT32 encode_pass(CHAR *buff,INT16 width,INT16 height)
{

    INT16 y1[64], y2[64], y3[64], y4[64], cb[64], cr[64];
    INT16 *y1_ptr, *y2_ptr, *y3_ptr, *y4_ptr, *cb_ptr, *cr_ptr;
    INT32 func_ret;
    INT32 block_x, x, y;
    INT32 addr;
    INT32 temp_width,temp_width2_sub16;
    INT32 temp_width_add16,temp_width_mpy8,temp_width_mpy9,temp_width_mpy8_add16,temp_width_mpy9_add16;


    temp_width = width*2;

    temp_width2_sub16 = (temp_width << 1) - 16;
    temp_width_add16 = temp_width + 16;
    temp_width_mpy8 = temp_width << 3;
    temp_width_mpy9 = temp_width_mpy8 + temp_width;
    temp_width_mpy8_add16 = temp_width_mpy8 + 16;
    temp_width_mpy9_add16 = temp_width_mpy9 + 16;

    if (width%16==0 && height%16==0)//411
    {

        //for (block_y = 0; block_y < raw_height; block_y += 16) {
        //temp_addr = buff ;//+ block_y*temp_width;
        for (block_x = 0; block_x < temp_width; block_x += 32)
        {
            addr = (UINT32) buff+block_x;
            y1_ptr = y1;
            y2_ptr = y2;
            y3_ptr = y3;
            y4_ptr = y4;
            cb_ptr = cb;
            cr_ptr = cr;
            for (y = 0; y < 8; y += 2)
            {
                for (x = 0; x < 8; x += 2)
                {
                    INT32 r, g, b;
                    register UINT16 *ptr = (UINT16 *)addr;

                    r = (INT32)((ptr[0]&0xf800)>>8);
                    g = (INT32)((ptr[0]&0x07e0)>>3);
                    b = (INT32)((ptr[0]&0x001f)<<3);
                    *y1_ptr = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
                    *cb_ptr = (((r * -43) + (g * -85) + (b << 7)) >> 8);
                    *cr_ptr = (((r << 7) + (g * -107) + (b * -21)) >> 8);

                    *(y1_ptr + 1) = (((((ptr[1]&0xf800)>>8) * 76) + (((ptr[1]&0x07e0)>>3) * 151) + (((ptr[1]&0x001f)<<3) * 29)) >> 8) - 128;

                    ptr = (UINT16*) (addr + temp_width);
                    *(y1_ptr + 8) = (((((ptr[0]&0xf800)>>8) * 76) + (((ptr[0]&0x07e0)>>3) * 151) + (((ptr[0]&0x001f)<<3) * 29)) >> 8) - 128;
                    *(y1_ptr + 9) = (((((ptr[1]&0xf800)>>8) * 76) + (((ptr[1]&0x07e0)>>3) * 151) + (((ptr[1]&0x001f)<<3) * 29)) >> 8) - 128;

                    ptr = (UINT16*) (addr + 16);

                    r = (INT32)((ptr[0]&0xf800)>>8);
                    g = (INT32)((ptr[0]&0x07e0)>>3);
                    b = (INT32)((ptr[0]&0x001f)<<3);
                    *y2_ptr = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
                    *(cb_ptr + 4) = (((r * -43) + (g * -85) + (b << 7)) >> 8);
                    *(cr_ptr + 4) = (((r << 7) + (g * -107) + (b * -21)) >> 8);
                    *(y2_ptr + 1) = (((((ptr[1]&0xf800)>>8) * 76) + (((ptr[1]&0x07e0)>>3) * 151) + (((ptr[1]&0x001f)<<3) * 29)) >> 8) - 128;

                    ptr = (UINT16*) (addr + temp_width_add16);
                    *(y2_ptr + 8) = (((((ptr[0]&0xf800)>>8) * 76) + (((ptr[0]&0x07e0)>>3) * 151) + (((ptr[0]&0x001f)<<3) * 29)) >> 8) - 128;
                    *(y2_ptr + 9) = (((((ptr[1]&0xf800)>>8) * 76) + (((ptr[1]&0x07e0)>>3) * 151) + (((ptr[1]&0x001f)<<3) * 29)) >> 8) - 128;

                    ptr = (UINT16*) (addr + temp_width_mpy8);

                    r = (INT32)((ptr[0]&0xf800)>>8);
                    g = (INT32)((ptr[0]&0x07e0)>>3);
                    b = (INT32)((ptr[0]&0x001f)<<3);
                    *y3_ptr = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
                    *(cb_ptr + 32) = (((r * -43) + (g * -85) + (b << 7)) >> 8);
                    *(cr_ptr + 32) = (((r << 7) + (g * -107) + (b * -21)) >> 8);
                    *(y3_ptr + 1) = (((((ptr[1]&0xf800)>>8) * 76) + (((ptr[1]&0x07e0)>>3) * 151) + (((ptr[1]&0x001f)<<3) * 29)) >> 8) - 128;

                    ptr =(UINT16*) (addr + temp_width_mpy9);
                    *(y3_ptr + 8) = (((((ptr[0]&0xf800)>>8) * 76) + (((ptr[0]&0x07e0)>>3) * 151) + (((ptr[0]&0x001f)<<3) * 29)) >> 8) - 128;
                    *(y3_ptr + 9) = (((((ptr[1]&0xf800)>>8) * 76) + (((ptr[1]&0x07e0)>>3) * 151) + (((ptr[1]&0x001f)<<3) * 29)) >> 8) - 128;

                    ptr = (UINT16*) (addr + temp_width_mpy8_add16);

                    r = (INT32)((ptr[0]&0xf800)>>8);
                    g = (INT32)((ptr[0]&0x07e0)>>3);
                    b = (INT32)((ptr[0]&0x001f)<<3);
                    *y4_ptr = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
                    *(cb_ptr + 36) = (((r * -43) + (g * -85) + (b << 7)) >> 8);
                    *(cr_ptr + 36) = (((r << 7) + (g * -107) + (b * -21)) >> 8);
                    *(y4_ptr + 1) = (((((ptr[1]&0xf800)>>8) * 76) + (((ptr[1]&0x07e0)>>3) * 151) + (((ptr[1]&0x001f)<<3) * 29)) >> 8) - 128;

                    ptr = (UINT16*) (addr + temp_width_mpy9_add16);
                    *(y4_ptr + 8) = (((((ptr[0]&0xf800)>>8) * 76) + (((ptr[0]&0x07e0)>>3) * 151) + (((ptr[0]&0x001f)<<3) * 29)) >> 8) - 128;
                    *(y4_ptr + 9) = (((((ptr[1]&0xf800)>>8) * 76) + (((ptr[1]&0x07e0)>>3) * 151) + (((ptr[1]&0x001f)<<3) * 29)) >> 8) - 128;

                    addr += 4;
                    y1_ptr += 2;
                    y2_ptr += 2;
                    y3_ptr += 2;
                    y4_ptr += 2;
                    cb_ptr++;
                    cr_ptr++;
                }
                addr += temp_width2_sub16;
                y1_ptr += 8;
                y2_ptr += 8;
                y3_ptr += 8;
                y4_ptr += 8;
                cb_ptr += 4;
                cr_ptr += 4;
            }
            if((func_ret = encode_block(y1, LUMINANCE, &dc_y)) != IMGS_ERR_NO)
            {
                return func_ret;
            }
            if((func_ret = encode_block(y2, LUMINANCE, &dc_y)) != IMGS_ERR_NO)
            {
                return func_ret;
            }
            if((func_ret = encode_block(y3, LUMINANCE, &dc_y)) != IMGS_ERR_NO)
            {
                return func_ret;
            }
            if((func_ret = encode_block(y4, LUMINANCE, &dc_y)) != IMGS_ERR_NO)
            {
                return func_ret;
            }
            if((func_ret = encode_block(cb, CHROMINANCE, &dc_cb)) != IMGS_ERR_NO)
            {
                return func_ret;
            }
            if((func_ret = encode_block(cr, CHROMINANCE, &dc_cr)) != IMGS_ERR_NO)
            {
                return func_ret;
            }
        }
    }
    else if (width%16==0 && height%16!=0) //422
    {
        temp_width2_sub16 = (temp_width - 16);
        {
            for (block_x = 0; block_x < temp_width; block_x += 32)
            {
                addr = (UINT32) (buff + block_x);
                y1_ptr = y1;
                y2_ptr = y2;
                cb_ptr = cb;
                cr_ptr = cr;
                for (y = 0; y < 8; y++)
                {
                    for (x = 0; x < 8; x += 2)
                    {
                        INT32 r, g, b;
                        register UINT16 *ptr = (UINT16 *)addr;

                        r = (INT32)((ptr[0]&0xf800)>>8);
                        g = (INT32)((ptr[0]&0x07e0)>>3);
                        b = (INT32)((ptr[0]&0x001f)<<3);
                        *y1_ptr = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
                        *cb_ptr = (((r * -43) + (g * -85) + (b << 7)) >> 8);
                        *cr_ptr = (((r << 7) + (g * -107) + (b * -21)) >> 8);
                        *(y1_ptr + 1) = (((((ptr[1]&0xf800)>>8) * 76) + (((ptr[1]&0x07e0)>>3) * 151) + (((ptr[1]&0x001f)<<3) * 29)) >> 8) - 128;

                        ptr = (UINT16 *)(addr + 16);

                        r = (INT32)((ptr[0]&0xf800)>>8);
                        g = (INT32)((ptr[0]&0x07e0)>>3);
                        b = (INT32)((ptr[0]&0x001f)<<3);
                        *y2_ptr = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
                        *(cb_ptr + 4) = (((r * -43) + (g * -85) + (b << 7)) >> 8);
                        *(cr_ptr + 4) = (((r << 7) + (g * -107) + (b * -21)) >> 8);
                        *(y2_ptr + 1) = (((((ptr[1]&0xf800)>>8) * 76) + (((ptr[1]&0x07e0)>>3) * 151) + (((ptr[1]&0x001f)<<3) * 29)) >> 8) - 128;
                        addr += 4;
                        y1_ptr += 2;
                        y2_ptr += 2;
                        cb_ptr ++;
                        cr_ptr ++;
                    }
                    addr += temp_width2_sub16;
                    cb_ptr += 4;
                    cr_ptr += 4;
                }
                if((func_ret = encode_block(y1, LUMINANCE, &dc_y)) != IMGS_ERR_NO)
                {
                    return func_ret;
                }
                if((func_ret = encode_block(y2, LUMINANCE, &dc_y)) != IMGS_ERR_NO)
                {
                    return func_ret;
                }
                if((func_ret = encode_block(cb, CHROMINANCE, &dc_cb)) != IMGS_ERR_NO)
                {
                    return func_ret;
                }
                if((func_ret = encode_block(cr, CHROMINANCE, &dc_cr)) != IMGS_ERR_NO)
                {
                    return func_ret;
                }
            }
        }
    }
    else//444
    {
        temp_width2_sub16 = (temp_width - 16);
        {

            for (block_x = 0; block_x < temp_width; block_x += 16)
            {
                addr = (UINT32)(buff + block_x);
                y1_ptr = y1;
                cb_ptr = cb;
                cr_ptr = cr;
                for (y = 0; y < 8; y++)
                {
                    for (x = 0; x < 8; x += 2)
                    {
                        INT32 r, g, b;
                        register UINT16 *ptr = (UINT16 *)addr;

                        r = (INT32)((ptr[0]&0xf800)>>8);
                        g = (INT32)((ptr[0]&0x07e0)>>3);
                        b = (INT32)((ptr[0]&0x001f)<<3);

                        *y1_ptr = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
                        *cb_ptr = (((r * -43) + (g * -85) + (b << 7)) >> 8);
                        *cr_ptr = (((r << 7) + (g * -107) + (b * -21)) >> 8);

                        r = (INT32)((ptr[1]&0xf800)>>8);
                        g = (INT32)((ptr[1]&0x07e0)>>3);
                        b = (INT32)((ptr[1]&0x001f)<<3);

                        *(y1_ptr + 1) = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
                        *(cb_ptr + 1) = (((r * -43) + (g * -85) + (b << 7)) >> 8);
                        *(cr_ptr + 1) = (((r << 7) + (g * -107) + (b * -21)) >> 8);

                        addr += 4;
                        y1_ptr += 2;
                        cb_ptr += 2;
                        cr_ptr += 2;
                    }
                    addr += temp_width2_sub16;
                }
                if((func_ret = encode_block(y1, LUMINANCE, &dc_y)) != IMGS_ERR_NO)
                {
                    return func_ret;
                }
                if((func_ret = encode_block(cb, CHROMINANCE, &dc_cb)) != IMGS_ERR_NO)
                {
                    return func_ret;
                }
                if((func_ret = encode_block(cr, CHROMINANCE, &dc_cr)) != IMGS_ERR_NO)
                {
                    return func_ret;
                }
            }
        }
    }

    return IMGS_ERR_NO;
}



PRIVATE VOID write_header(INT16 quality_select ,INT16 width,INT16 height)
{

///////////////////////////////////////////////////////////////////////////

    FS_Write(fileHandle, (UINT8*)g_imgsHead1 , 20);//fwrite(g_imgsHead1,20,1,pout);// memcpy(&RAM_EXTERN[RAME_HEADER1_ADDR],g_imgsHead1,20);

    if (quality_select==0)
        FS_Write(fileHandle,(UINT8*)g_imgsHeadQL ,136 );//fwrite(g_imgsHeadQL,136,1,pout);//memcpy(&RAM_EXTERN[RAME_HEAD_Q_ADDR],g_imgsHeadQL,136);
    else
        FS_Write(fileHandle,(UINT8*)g_imgsHeadQH ,136 );   //fwrite(g_imgsHeadQH,136,1,pout);//memcpy(&RAM_EXTERN[RAME_HEAD_Q_ADDR],g_imgsHeadQH,136);


    g_imgsHeadSize128411[3]=height>>8;
    g_imgsHeadSize128411[4]=height&0xff;
    g_imgsHeadSize128411[5]=width>>8;
    g_imgsHeadSize128411[6]=width&0xff;
    if (width%16!=0)
    {
        g_imgsHeadSize128411[9] = 0x11;
    }
    else if (height%16!=0)//422
    {
        g_imgsHeadSize128411[9] = 0x21;
    }
    else
    {
        g_imgsHeadSize128411[9] = 0x22;
    }

    FS_Write(fileHandle,(UINT8*)g_imgsHeadSize128411,10);//fwrite(g_imgsHeadSize128411,10,1,pout);

    FS_Write(fileHandle,(UINT8*)g_imgsHead4 ,441 );//fwrite(g_imgsHead4,441,1,pout);//memcpy(&RAM_EXTERN[RAME_HEAD4_ADDR],g_imgsHead4,443);

    // memcpy(RAM_EXTERN,Buff,TOTAL422_SIZE);
    // fflush(pout);// fwrite(RAM_EXTERN,607,1,pout);
    // return 0;


    if (quality_select==1)
    {
        quant_table = g_imgsLCQuantTable1;
    }
    else
    {
        quant_table = g_imgsLCQuantTable0;
    }
}


PRIVATE VOID _jpeg_putw(INT32 w)
{
    _jpeg_putc((w >> 8)& 0xff);
    _jpeg_putc(w & 0xff);
}

PRIVATE VOID _jpeg_putc(INT32 c)
{
    *chunkptr++ = c;
    if(chunkptr == chunk_end)
    {
        if(JPGENC_BUF_MODE)//sheen
        {
            memcpy(pJpgOut, chunk, TEMPOUTBUFSIZE);
            pJpgOut+=TEMPOUTBUFSIZE;
        }
        else
            FS_Write(fileHandle,chunk,TEMPOUTBUFSIZE);//fwrite(&chunk,TEMPOUTBUFSIZE,1,pout);
        chunkptr = chunk;
        outputlen+=TEMPOUTBUFSIZE;
    }
}

/* encode_block:
 *  Encodes an 8x8 basic block of coefficients of given type (luminance or
 *  chrominance) and writes it to the output stream.
 */
PRIVATE INT32 encode_block(INT16 *block, INT32 type, INT32 *old_dc)
{
    //HUFFMAN_TABLE *dc_table, *ac_table;
    CONST UINT16 *ac_table1,*dc_table1;
    CONST INT32 *pTempQuanTab;//INT32 *quant_table;
    INT32 index;
    INT32 num_zeroes;
    INT32 category, bits;
    INT32 dc_tmp,value0;

    //dc_table = &huffman_dc_table[type];
    //ac_table = &huffman_ac_table[type];
    if (type==0)
    {
        dc_table1 = g_imgsHuffmanDc0;
        ac_table1 = g_imgsHuffmanAc0+4;
    }
    else
    {
        dc_table1 = g_imgsHuffmanDc1;
        ac_table1 = g_imgsHuffmanAc1+4;
    }
    pTempQuanTab = quant_table + 64*type;//quant_table = l_c_quant_table[type];



    apply_fdct(block);
    dc_tmp = (block[0] * pTempQuanTab[0]) >> 19;
    value0 = dc_tmp - *old_dc;
    *old_dc = dc_tmp;
    format_number(value0, &category, &bits);
    {
//      HUFFMAN_ENTRY *entry = dc_table->code[category];
//      if(!entry)
//      return IMGS_ERR_VLC;
//      put_bits(entry->encoded_value,entry->bits_length);
        put_bits((INT32)dc_table1[category*2+1],(INT32)dc_table1[category*2]);

    }
    put_bits(bits, category);

    num_zeroes = 0;
    for (index = 1; index < 64; index++)
    {
        // FIXME What is this 'register' for ?
        INT32 value;
#if 1
        value = block[g_imgsInverseZigZag[index]];
        value = ((value * pTempQuanTab[index]) >> 19) + ((value >> 31) & 0x1);
#else
        value = block[g_imgsInverseZigZag[index]];
        if (value < 0)
        {
            value = ((value * quant_table[index]) - (quant_table[index] >> 1) >> 19) + 1;
        }
        else
            value = ((value * quant_table[index]) + (quant_table[index] >> 1)) >> 19;
#endif
        if (value == 0)
            num_zeroes++;
        else
        {
            while (num_zeroes > 15)
            {

                {
                    //HUFFMAN_ENTRY *entry = ac_table->code[0xf0];
                    //if(!entry)
                    //  return IMGS_ERR_VLC;
                    //put_bits(entry->encoded_value,entry->bits_length);
                    put_bits(ac_table1[-1],ac_table1[-2]);

                }
                num_zeroes -= 16;
            }
            format_number(value, &category, &bits);
            value = (num_zeroes << 4) | category;
            {
//              HUFFMAN_ENTRY *entry = ac_table->code[value];
//              if(!entry)
//                  return IMGS_ERR_VLC;
//              put_bits(entry->encoded_value,entry->bits_length);
                put_bits((INT32)ac_table1[num_zeroes*20 + (category-1)*2 + 1],
                         (INT32)ac_table1[num_zeroes*20 + (category-1)*2]);

            }
            put_bits(bits, category);
            num_zeroes = 0;
        }
    }
    if (num_zeroes > 0)
    {
        {
            //HUFFMAN_ENTRY *entry = ac_table->code[0x0];
            //if(!entry)
            //  return IMGS_ERR_VLC;
            //put_bits(entry->encoded_value,entry->bits_length);
            put_bits(ac_table1[-3],ac_table1[-4]);
        }
    }
    return IMGS_ERR_NO;
}
/* apply_fdct:
 *  Applies the forward discrete cosine transform to the given input block,
 *  in the form of a vector of 64 coefficients.
 *  This uses INT32eger fixed poINT32 math and is based on code by the IJG.
 */
PRIVATE VOID apply_fdct(INT16 *data)
{
    INT32 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    INT32 tmp10, tmp11, tmp12, tmp13;
    INT32 z1, z2, z3, z4, z5;
    INT16 *dataptr = data;
    INT32 i;

    for (i = 8; i; i--)
    {
        tmp0 = dataptr[0] + dataptr[7];
        tmp7 = dataptr[0] - dataptr[7];
        tmp1 = dataptr[1] + dataptr[6];
        tmp6 = dataptr[1] - dataptr[6];
        tmp2 = dataptr[2] + dataptr[5];
        tmp5 = dataptr[2] - dataptr[5];
        tmp3 = dataptr[3] + dataptr[4];
        tmp4 = dataptr[3] - dataptr[4];

        tmp10 = tmp0 + tmp3;
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[0] = (tmp10 + tmp11) << 2;
        dataptr[4] = (tmp10 - tmp11) << 2;

        z1 = (tmp12 + tmp13) * FIX_0_541196100;
        dataptr[2] = (z1 + (tmp13 * FIX_0_765366865)) >> 11;
        dataptr[6] = (z1 + (tmp12 * -FIX_1_847759065)) >> 11;

        z1 = tmp4 + tmp7;
        z2 = tmp5 + tmp6;
        z3 = tmp4 + tmp6;
        z4 = tmp5 + tmp7;
        z5 = (z3 + z4) * FIX_1_175875602;

        tmp4 *= FIX_0_298631336;
        tmp5 *= FIX_2_053119869;
        tmp6 *= FIX_3_072711026;
        tmp7 *= FIX_1_501321110;
        z1 *= -FIX_0_899976223;
        z2 *= -FIX_2_562915447;
        z3 *= -FIX_1_961570560;
        z4 *= -FIX_0_390180644;

        z3 += z5;
        z4 += z5;

        dataptr[7] = (tmp4 + z1 + z3) >> 11;
        dataptr[5] = (tmp5 + z2 + z4) >> 11;
        dataptr[3] = (tmp6 + z2 + z3) >> 11;
        dataptr[1] = (tmp7 + z1 + z4) >> 11;

        dataptr += 8;
    }

    dataptr = data;
    for (i = 8; i; i--)
    {
        tmp0 = dataptr[0] + dataptr[56];
        tmp7 = dataptr[0] - dataptr[56];
        tmp1 = dataptr[8] + dataptr[48];
        tmp6 = dataptr[8] - dataptr[48];
        tmp2 = dataptr[16] + dataptr[40];
        tmp5 = dataptr[16] - dataptr[40];
        tmp3 = dataptr[24] + dataptr[32];
        tmp4 = dataptr[24] - dataptr[32];

        tmp10 = tmp0 + tmp3;
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[0] = (tmp10 + tmp11) >> 2;
        dataptr[32] = (tmp10 - tmp11) >> 2;

        z1 = (tmp12 + tmp13) * FIX_0_541196100;
        dataptr[16] = (z1 + (tmp13 * FIX_0_765366865)) >> 15;
        dataptr[48] = (z1 + (tmp12 * -FIX_1_847759065)) >> 15;

        z1 = tmp4 + tmp7;
        z2 = tmp5 + tmp6;
        z3 = tmp4 + tmp6;
        z4 = tmp5 + tmp7;
        z5 = (z3 + z4) * FIX_1_175875602;

        tmp4 *= FIX_0_298631336;
        tmp5 *= FIX_2_053119869;
        tmp6 *= FIX_3_072711026;
        tmp7 *= FIX_1_501321110;
        z1 *= -FIX_0_899976223;
        z2 *= -FIX_2_562915447;
        z3 *= -FIX_1_961570560;
        z4 *= -FIX_0_390180644;

        z3 += z5;
        z4 += z5;
        dataptr[56] = (tmp4 + z1 + z3) >> 15;
        dataptr[40] = (tmp5 + z2 + z4) >> 15;
        dataptr[24] = (tmp6 + z2 + z3) >> 15;
        dataptr[8] = (tmp7 + z1 + z4) >> 15;

        dataptr++;
    }
}

/* format_number:
 *  Computes the category and bits of a given number.
 */
PRIVATE VOID format_number(INT32 num, INT32 *category, INT32 *bits)
{
    INT32 abs_num, mask, num_sign;
    INT32 cat;
    mask = num >> 31;
    abs_num = (num ^ mask) - mask;
    for (cat = 0; abs_num; cat++)
        abs_num >>= 1;

    *category = cat;

    num_sign = mask & 0x1;//((num>>cat)&0x1);
    *bits = num + (num_sign << cat) - num_sign;
}

/* put_bits:
 *   Writes some bits to the output stream.
 */
PRIVATE VOID put_bits(INT32 value, INT32 num_bits)
{
    INT32 crt_bit = current_bit;
    INT32 crt_byte = current_byte;
    while(num_bits > crt_bit)
    {
        num_bits -= crt_bit;
        crt_byte |= (value >> num_bits);
        *chunkptr++ = crt_byte;
        if(chunkptr == chunk_end)
        {
            if(JPGENC_BUF_MODE)//sheen
            {
                memcpy(pJpgOut, chunk, TEMPOUTBUFSIZE);
                pJpgOut+=TEMPOUTBUFSIZE;
            }
            else
                FS_Write(fileHandle,chunk,TEMPOUTBUFSIZE);//fwrite(&chunk,TEMPOUTBUFSIZE,1,pout);
            chunkptr = chunk;
            outputlen+=TEMPOUTBUFSIZE;
        }
        if ((crt_byte&0xff) == 0xff)
        {
            *chunkptr++ = 0;
            if(chunkptr == chunk_end)
            {
                if(JPGENC_BUF_MODE)//sheen
                {
                    memcpy(pJpgOut, chunk, TEMPOUTBUFSIZE);
                    pJpgOut+=TEMPOUTBUFSIZE;
                }
                else
                    FS_Write(fileHandle,chunk,TEMPOUTBUFSIZE);//fwrite(&chunk,TEMPOUTBUFSIZE,1,pout);
                chunkptr = chunk;
                outputlen+=TEMPOUTBUFSIZE;
            }
        }
        crt_bit = 8;
        crt_byte = 0;
    }
    crt_bit -= num_bits;
    crt_byte |= (value << crt_bit);
    current_bit = crt_bit;
    current_byte = crt_byte;
}
/* _jpeg_put_bit:
 *  Writes a bit to the output stream.
*/
PRIVATE INT32 _jpeg_put_bit(INT32 bit)
{
    current_byte |= (bit << current_bit);
    current_bit--;
    if (current_bit < 0)
    {
        _jpeg_putc(current_byte);
        if (current_byte == 0xff)
            _jpeg_putc(0);
        current_bit = 8;
        current_byte = 0;
    }
    return IMGS_ERR_NO;
}
/* _jpeg_flush_bits:
 *  Flushes the current byte by filling unset bits with 1.
 */
PRIVATE VOID _jpeg_flush_bits(VOID)
{
    current_bit--;
    while (current_bit <7)
        _jpeg_put_bit(1);
}

#ifdef SHEEN_VC_DEBUG
UINT16 yuv420_to_rgb565(INT32 y, INT32 u, INT32 v)
{
    UINT16 res;
    INT32 r, g , b;
    r=( 298 * y + 409 * v - 56992)>> 8;
    if(r<0) r=0; else if(r>255) r=255;
    g=( 298 * y -100 * u -208 * v + 34784)>> 8;
    if(g<0) g=0; else if(g>255) g=255;
    b=( 298 * y + 516 * u - 70688)>> 8;
    if(b<0) b=0; else if(b>255) b=255;
    res=((( (r) & 0xf8) << 8) | (( (g) & 0xfc) << 3) | (( (b) & 0xf8) >> 3));
    return res;
}
//test sheen.
void main()
{
    FILE *pfile;
    FILE *pfile2;
    char *pYUV;
    char *pRGB;
    char *pJPG;
    int size;
    int i,j;
    int w=640;
    int h=480;

    pYUV=malloc(w*h*3);
    pJPG=malloc(w*h*3);
    //pRGB=malloc(320*240*2);
    pfile=fopen("F:\\测试文件\\640x480fromCam_422.yuv","rb");
    //pfile=fopen("F:\\测试文件\\176x144.rgb565","rb");
    pfile2=fopen("F:\\测试文件\\test.jpg","wb");
    //fseek(pfile, 54*176*144*3/2, SEEK_SET);
    fread(pYUV,1, w*h*2, pfile);
    //fread(pRGB,1, 176*144*2, pfile);
#if 0
    {
        //rgb->yuv
        int r,g,b;
        unsigned short *pR1,*pR2;
        unsigned char *pY1,*pY2,*pU,*pV;
        pR1=pRGB;
        pR2=pR1+176;
        pY1=pYUV;
        pY2=pY1+176;
        pU=pY1+176*144;
        pV=pU+176*144/4;
        for(i=0; i<144; i+=2)
        {
            for(j=0; j<176; j+=2)
            {
                r = (int)((pR1[0]&0xf800)>>8);
                g = (int)((pR1[0]&0x07e0)>>3);
                b = (int)((pR1[0]&0x001f)<<3);
                pR1++;

                *pY1 = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
                pY1++;

                *pU = (((r * -43) + (g * -85) + (b << 7)) >> 8);
                *pV = (((r << 7) + (g * -107) + (b * -21)) >> 8);
                pU++;
                pV++;

                r = (int)((pR1[0]&0xf800)>>8);
                g = (int)((pR1[0]&0x07e0)>>3);
                b = (int)((pR1[0]&0x001f)<<3);
                pR1++;

                *pY1 = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
                pY1++;

                r = (int)((pR2[0]&0xf800)>>8);
                g = (int)((pR2[0]&0x07e0)>>3);
                b = (int)((pR2[0]&0x001f)<<3);
                pR2++;

                *pY2 = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
                pY2++;

                r = (int)((pR2[0]&0xf800)>>8);
                g = (int)((pR2[0]&0x07e0)>>3);
                b = (int)((pR2[0]&0x001f)<<3);
                pR2++;

                *pY2 = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
                pY2++;

            }
            pR1+=176;
            pR2+=176;
            pY1+=176;
            pY2+=176;
        }


    }
#endif

#if 0
    {
        unsigned short *pR1,*pR2;
        unsigned char *pY1,*pY2,*pU,*pV;
        pY1=pYUV;
        pY2=pY1+176;
        pU=pYUV+176*144;
        pV=pU+176*144/4;
        pR1=pRGB;
        pR2=pR1+176;
        for(i=0; i<144; i+=2)
        {
            for(j=0; j<176; j+=2)
            {
                *pR1=yuv420_to_rgb565(*pY1,*pU,*pV);
                pR1++;
                pY1++;
                *pR1=yuv420_to_rgb565(*pY1,*pU,*pV);
                pR1++;
                pY1++;

                *pR2=yuv420_to_rgb565(*pY2,*pU,*pV);
                pR2++;
                pY2++;
                *pR2=yuv420_to_rgb565(*pY2,*pU,*pV);
                pR2++;
                pY2++;

                pU++;
                pV++;

            }
            pR1+=176;
            pR2+=176;
            pY1+=176;
            pY2+=176;
        }
    }
#endif
    imgs_JpegEncodeBufYUV422(pYUV,  0 ,w, h, pJPG, &size);
    //imgs_JpegEncodeBuf2(pRGB,  0 ,176, 144, pJPG, &size);

    fwrite(pJPG, 1, size, pfile2);
    fclose(pfile);
    fclose(pfile2);
}
#endif


