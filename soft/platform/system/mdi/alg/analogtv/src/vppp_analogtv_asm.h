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

#ifndef VPPP_ANALOGTV_ASM_H
#define VPPP_ANALOGTV_ASM_H



#ifndef CS_PLATFORM_SIMU

#define RAM_X_BEGIN_ADDR 0x0
#define RAM_Y_BEGIN_ADDR 0x4000

#endif

#define VPP_ANALOGTV_MAIN_ENTRY           0
#define VPP_ANALOGTV_CRITICAL_SECTION_MIN 0
#define VPP_ANALOGTV_CRITICAL_SECTION_MAX 0

EXPORT  INT32 g_vppAnalogTVMainCodeSize;
EXPORT  INT32 g_vppAnalogTVConstXSize;
EXPORT  INT32 g_vppAnalogTVConstYSize;


EXPORT  UINT16  g_vppAnalogTVConstX[];
EXPORT  UINT16  g_vppAnalogTVConstY[];
EXPORT  INT32 g_vppAnalogTVMainCode[];


#endif  // VPPP_ANALOGTV_ASM_H
