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
















#ifndef VPPP_SPEECH_ASM_H
#define VPPP_SPEECH_ASM_H

#include "cs_types.h"

#define RAM_X_BEGIN_ADDR 0x0
#define RAM_Y_BEGIN_ADDR 0x4000

#include "vppp_speech_map.h"

#define VPP_SPEECH_MAIN_ENTRY           0
#define VPP_SPEECH_CRITICAL_SECTION_MIN 0
#define VPP_SPEECH_CRITICAL_SECTION_MAX 0

EXPORT INT32 g_vppSpeechMainCodeSize;
EXPORT INT32 g_vppSpeechMainCode[];
EXPORT INT32 g_vppEfrAmrCode[];
EXPORT INT32 g_vppFrCode[];
EXPORT INT32 g_vppHrCode[];
EXPORT INT32 g_vppAmrCode[];
EXPORT INT32 g_vppEfrCode[];
EXPORT INT32 g_vppAmrEncCode475[];
EXPORT INT32 g_vppAmrEncCode515[];
EXPORT INT32 g_vppAmrEncCode59[];
EXPORT INT32 g_vppAmrEncCode67[];
EXPORT INT32 g_vppAmrEncCode74[];
EXPORT INT32 g_vppAmrEncCode795[];
EXPORT INT32 g_vppAmrEncCode102[];
EXPORT INT32 g_vppAmrEncCode122[];
EXPORT INT32 g_vppAmrDecCode475[];
EXPORT INT32 g_vppAmrDecCode515[];
EXPORT INT32 g_vppAmrDecCode59[];
EXPORT INT32 g_vppAmrDecCode67[];
EXPORT INT32 g_vppAmrDecCode74[];
EXPORT INT32 g_vppAmrDecCode795[];
EXPORT INT32 g_vppAmrDecCode102[];
EXPORT INT32 g_vppAmrDecCode122[];


#endif  // VPPP_SPEECH_ASM_H
