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

#ifndef CS_PLATFORM_SIMU

#define RAM_X_BEGIN_ADDR 0x0
#define RAM_Y_BEGIN_ADDR 0x4000

#endif

#define VPP_SPEECH_MAIN_ENTRY           0
#define VPP_SPEECH_CRITICAL_SECTION_MIN 0x210
#define VPP_SPEECH_CRITICAL_SECTION_MAX 0x2d2

EXPORT INT32 g_vppEfrAmrConstX[];
EXPORT INT32 g_vppEfrAmrConstY[];
EXPORT INT32 g_vppHrConstX[];
EXPORT INT32 g_vppHrConstY[];

#ifdef SMALL_BSS_RAM_SIZE

#define g_vppSpeechRAMX_Len (2500<<2)
#define g_vppSpeechCode_Len (1880<<2)
#define g_vppAecRAMX_Len (1240<<2)

#define g_vppMorhpRAMX_Len (600<<2)
#define g_vppMicNoiseSuppressRAMX_Len (1640<<2)
#define g_vppReceiverNoiseSuppressRAMX_Len (1640<<2)

#define g_vppMicAgcRAMX_Len (200<<2)
#define g_vppReceiverRAMX_Len (200<<2)

INT8* g_vppSpeechRAMX_alloc = NULL;
INT8* g_vppSpeechCode_alloc = NULL;
INT8* g_vppAecRAMX_alloc = NULL;

INT8* g_vppMicNoiseSuppressRAMX_alloc= NULL;
INT8* g_vppReceiverNoiseSuppressRAMX_alloc= NULL;

INT8* g_vppMicAgcRAMX_alloc= NULL;
INT8* g_vppReceiverAgcRAMX_alloc= NULL;

#ifdef MAGIC_AE_SUPPORT
INT8* g_vppMorhpRAMX_alloc= NULL;
#endif


INT32* g_vppSpeechRAMX= NULL;
INT32* g_vppSpeechCode= NULL;
INT32* g_vppAecRAMX= NULL;


INT32* g_vppMicNoiseSuppressRAMX= NULL;
INT32* g_vppReceiverNoiseSuppressRAMX= NULL;

INT32* g_vppMicAgcRAMX= NULL;
INT32* g_vppReceiverAgcRAMX= NULL;

INT32* g_vppMorhpRAMX= NULL;

#else

EXPORT  INT32 g_vppSpeechRAMX[];
EXPORT  INT32 g_vppSpeechCode[];

EXPORT  INT32   g_vppMicNoiseSuppressRAMX[];
EXPORT  INT32   g_vppReceiverNoiseSuppressRAMX[] ;

EXPORT  INT32 g_vppMicAgcRAMX[];
EXPORT  INT32 g_vppReceiverAgcRAMX[];

EXPORT  INT32   g_vppMorhpRAMX[] ;

EXPORT  INT32 g_vppAecRAMX[];





#endif


EXPORT INT32 g_vppAecConstantX[];
EXPORT INT32 g_vppAecCode[];


EXPORT  INT32   g_vppMorphConstantX[];
EXPORT  INT32   g_vppMorphCode[];

EXPORT INT32   g_vppNoiseSuppressConstantX[];
EXPORT  INT32   g_vppNoiseSuppressCode[];

EXPORT INT32 g_vppAgcConstantX[];
EXPORT INT32 g_vppAgcCode[];

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
