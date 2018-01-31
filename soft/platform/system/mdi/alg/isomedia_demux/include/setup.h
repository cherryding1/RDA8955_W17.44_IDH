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

#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif

#ifdef MP4_3GP_SUPPORT

#ifndef _GF_SETUP_H_
#define _GF_SETUP_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SHEEN_VC_DEBUG
typedef unsigned char uint8;
typedef char int8;
typedef unsigned char UINT8;
typedef short INT16;

typedef unsigned int uint32;
typedef int int32;
typedef unsigned int UINT32;
typedef int INT32;

#define boolean char
#define HANDLE int
void *mmc_MemMalloc(int size);
#define FS_SEEK_SET SEEK_SET
#define FS_SEEK_CUR SEEK_CUR
#define FS_SEEK_END SEEK_END
#define diag_printf printf
#define FS_Read(a,b,c) fread(b,1,c,a)
#define FS_Seek(a,b,c) fseek(a,b,c)
#define FS_IsEndOfFile(a) 0
#endif

/*WIN32 and WinCE config*/
#if defined(WIN32) || defined(_WIN32_WCE)

/*common win32 parts*/
#include <stdio.h>
#include <stdlib.h>


typedef unsigned __int64 u64;
typedef unsigned int u32;
typedef unsigned short uint16;
typedef unsigned char u8;
typedef __int64 s64;
typedef int s32;
typedef short s16;
typedef char s8;

#if defined(__GNUC__)
#define GFINLINE inline
#else
#define GFINLINE __inline
#endif

#define GF_PATH_SEPARATOR   '\\'
#define GF_MAX_PATH 260


/*WINCE config*/
#if defined(_WIN32_WCE)



/*win32 assert*/
void CE_Assert(uint32 valid);
#ifndef NDEBUG
#define assert( t ) CE_Assert((unsigned int) (t) )
#else
#define assert(t)
#endif

/*performs wide->char and char->wide conversion on a buffer GF_MAX_PATH long*/
void CE_WideToChar(unsigned short *w_str, int8 *str);
void CE_CharToWide(int8 *str, unsigned short *w_str);


#define strdup _strdup
#define stricmp _stricmp
#define strnicmp _strnicmp
#define strupr _strupr

#ifndef _PTRDIFF_T_DEFINED
typedef int ptrdiff_t;
#define PTRDIFF(p1, p2, type)   ((p1) - (p2))
#define _PTRDIFF_T_DEFINED
#endif

#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

#ifndef offsetof
#define offsetof(s,m) ((size_t)&(((s*)0)->m))
#endif

#ifndef getenv
#define getenv(a) 0L
#endif

#ifndef strupr
int8 * my_str_upr(int8 *str);
#define strupr my_str_upr
#endif

#ifndef strlwr
int8 * my_str_lwr(int8 *str);
#define strlwr my_str_lwr
#endif



#else   /*END WINCE*/

/*WIN32 not-WinCE*/
#include <ctype.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <stdarg.h>
#include <assert.h>

#endif  /*END WIN32 non win-ce*/

#else   /*end WIN32 config*/

/*UNIX likes*/

/*force large file support*/
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

/* change shenh
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
*/
#include "cs_types.h"
#include "cs_types.h"
/* change shenh
typedef uint64_t u64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef int64_t s64;
typedef __int64 s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;
*/


typedef unsigned long long u64;
//typedef unsigned int u64;//=uint32

//typedef unsigned int u32;
//typedef unsigned short u16;
//typedef unsigned char u8;
//typedef long long s64;
//typedef int s64;//=s32
//typedef int s32;
//typedef short s16;
//typedef char s8;


#define GFINLINE    inline

/*sorry this was developed under w32 :)*/
#define stricmp     strcasecmp
#define strnicmp    strncasecmp

#ifndef strupr
int8 * my_str_upr(int8 *str);
#define strupr my_str_upr
#endif

#ifndef strlwr
int8 * my_str_lwr(int8 *str);
#define strlwr my_str_lwr
#endif

#define GF_PATH_SEPARATOR   '/'

#ifdef PATH_MAX
#define GF_MAX_PATH PATH_MAX
#else
/*PATH_MAX not defined*/
#define GF_MAX_PATH 1023
#endif


#endif

/*define what's missing*/
#ifndef NULL
#define NULL 0
#endif


typedef double Double;
typedef float Float;
/* 128 bit IDs */
typedef uint8 bin128[16];

#define GF_MAX_FLOAT        FLT_MAX
#define GF_MIN_FLOAT        -GF_MAX_FLOAT
#define GF_EPSILON_FLOAT    FLT_EPSILON
#define GF_SHORT_MAX        SHRT_MAX
#define GF_SHORT_MIN        SHRT_MIN

#ifndef MIN
#define MIN(X, Y) ((X)<(Y)?(X):(Y))
#endif
#ifndef MAX
#define MAX(X, Y) ((X)>(Y)?(X):(Y))
#endif

#define ABSDIFF(a, b)   ( ( (a) > (b) ) ? ((a) - (b)) : ((b) - (a)) )

#ifndef ABS
#define ABS(a)  ( ( (a) > 0 ) ? (a) : - (a) )
#endif

#ifndef Bool
typedef uint32 Bool;
#endif

#ifdef __cplusplus
}
#endif

#endif  /*_GF_SETUP_H_*/

#endif

