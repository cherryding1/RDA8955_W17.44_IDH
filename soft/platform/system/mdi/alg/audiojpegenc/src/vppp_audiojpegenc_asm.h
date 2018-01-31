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

#ifndef VPPP_AUDIOJPEGENC_ASM_H
#define VPPP_AUDIOJPEGENC_ASM_H


#include "cs_types.h"

#ifndef CS_PLATFORM_SIMU

#define RAM_X_BEGIN_ADDR 0x0
#define RAM_Y_BEGIN_ADDR 0x4000

#endif

#define VPP_AUDIOJPEGENC_MAIN_ENTRY           0
#define VPP_AUDIOJPEGENC_CRITICAL_SECTION_MIN 0
#define VPP_AUDIOJPEGENC_CRITICAL_SECTION_MAX 0


//common
EXPORT const INT32 G_VppCommonEncCode[];
#ifdef MP3_ENC_SUPPORT

//mp3
EXPORT const INT32 G_VppMp3EncCode[];
EXPORT const INT32 G_VppMp3EncConstX[];
EXPORT const INT32 G_VppMp3EncConstY[];
EXPORT const INT32 G_VppMp3EncConst_rqmy[];
EXPORT const INT32 G_VppMp3EncConst_Zig[];
#endif
//amrjpeg
EXPORT const INT32 G_VppAmrJpegEncCode[];
EXPORT const INT32 G_VppAmrJpegEncConstX[];
EXPORT const INT32 G_VppAmrJpegEncConstY[];

EXPORT const INT32 G_vocYUVRomateCode[];

EXPORT CONST INT32 G_VppAudJpegMainCodeSize;

#endif  // VPPP_AUDIOJPEGENC_ASM_H
