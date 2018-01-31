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

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#define LILY_DEBUG
#include "cs_types.h"
//Unicode usage
//#define USE_UNICODE_CP1252
//#define USE_UNICODE_CP932
//#define USE_UNICODE_CP950
//#define USE_UNICODE_CP949
#define USE_UNICODE_CP936
/*
// dingjian 20071217 for lcd szie is 176*220
// TYPE1 128*160
// TYPE2 176*220
// TYPE3 240*320
#define TYPE_128_160    1
#define TYPE_176_220    2
#define TYPE_240_320    3

#ifdef LCDSIZE_128_160
#define LCD_SIZE    TYPE_128_160
#endif

#ifdef LCDSIZE_176_220
#define LCD_SIZE    TYPE_176_220
#endif

#ifdef LCDSIZE_240_320
#define LCD_SIZE    TYPE_240_320
#endif

#if (TYPE_128_160 ==  LCD_SIZE)
#define LCD_WIDTH   128
#define LCD_HEIGHT  160
#elif (TYPE_176_220 ==  LCD_SIZE)
#define LCD_WIDTH   176
#define LCD_HEIGHT  220
#elif (TYPE_240_320 ==  LCD_SIZE)
#define LCD_WIDTH   240
#define LCD_HEIGHT  320
#else
#error "no lcd config info"
#endif
*/
//ENABLED OR DISABLED FUNCTION
//#define ID3_ENABLED
#define LILY_USE_LOCAL_UNICODETAB
extern INT32 MMC_LcdWidth;
extern INT32 MMC_LcdHeight;



// dingjian add 20080218
//#if (TYPE_240_320 ==  LCD_SIZE)
//#define CONST_MMC_MEM_SIZE 960 * 1024
//#else
#define CONST_MMC_MEM_SIZE 960 * 1024
//#endif

#define DEVICE_SUPPORT_USB
//#define DEVICE_SUPPORT_NANDFLASH
#define MP4_3GP_SUPPORT
#endif//__GLOBAL_H__


