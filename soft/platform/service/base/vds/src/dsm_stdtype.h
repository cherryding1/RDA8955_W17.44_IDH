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

#if !defined(__DSM_STD_TYPE_H__)
#define __DSM_STD_TYPE_H__

#if !defined(MAX_INT8)
#define MAX_INT8            +127
#endif

#if !defined(MIN_INT8)
#define MIN_INT8            -128
#endif

#if !defined(MAX_UINT8)
#define MAX_UINT8           255
#endif

#if !defined(MIN_UINT8)
#define MIN_UINT8           0
#endif

#if !defined(MAX_INT16)
#define MAX_INT16           +32767
#endif

#if !defined(MIN_INT16)
#define MIN_INT16           -32768
#endif

#if !defined(MAX_UINT16)
#define MAX_UINT16          65535
#endif

#if !defined(MIN_UINT16)
#define MIN_UINT16          0
#endif

#if !defined(MAX_INT32)
#define MAX_INT32           +2147483647L
#endif

#if !defined(MIN_INT32)
#define MIN_INT32           -2147483648L
#endif

#if !defined(MAX_UINT32)
#define MAX_UINT32          4294967295U
#endif

#if !defined(MIN_UINT32)
#define MIN_UINT32          0U
#endif

#if !defined(MAX)
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#if !defined(MIN)
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#if !defined(MAKEINT16)
#define MAKEINT16(a, b)     ((INT16)(((UINT8)(a)) | ((INT16)((UINT8)(b))) << 8))
#endif

#if !defined(MAKEINT32)
#define MAKEINT32(a, b)     ((INT32)(((UINT16)(a)) | ((INT32)((UINT16)(b))) << 16))
#endif

#if !defined(LOINT8)
#define LOINT8(w)           ((INT8)(w))
#endif

#if !defined(HIINT8)
#define HIINT8(w)           ((INT8)((UINT16)(w) >> 8))
#endif

#if !defined(LOINT16)
#define LOINT16(l)          ((INT16)(l))
#endif

#if !defined(HIINT16)
#define HIINT16(l)          ((INT16)((UINT32)(l) >> 16))
#endif

#define MSB(x)    (((x) >> 8) & 0xff) // most signif byte of 2-byte integer
#define LSB(x)    ((x) & 0xff)        // least signif byte of 2-byte integer

#ifndef MAKE_WORD
#define MAKE_WORD(p)    (((UINT16)(p)[0] << 8) | (UINT16)(p)[1])
#endif
#ifndef MAKE_DWORD
#define MAKE_DWORD(p)   (((UINT32)(p)[0] << 24) | ((UINT32)(p)[1] << 16) | ((UINT32)(p)[2] << 8) | (UINT32)(p)[3])
#endif

#ifndef SWAP16
#define SWAP16(p)    (((UINT16)(p)[1] << 8) | (UINT16)(p)[0])
#endif

#ifndef SWAP32
#define SWAP32(p)    (((UINT32)(p)[3] << 24) | ((UINT32)(p)[2] << 16) | ((UINT32)(p)[1] << 8) | (UINT32)(p)[0])
#endif

#define PRIVATE static // function scope
#define PUBLIC

//#define NULL    0   note by nie to resolve the redefinition of NULL both in stdio.h and in this fils.
#define HNULL 0
#define CONST const
#define VOID void

typedef unsigned int   size_t;

#ifdef _FS_SIMULATOR_
#define TRUE    1
#define FALSE   0
#define _CSW    0
#define TDB     0

#ifdef WIN32
typedef __int64             UINT64;
typedef __int64             INT64;
#else
typedef unsigned long long  UINT64;
typedef long long       INT64;
#endif
typedef unsigned int       UINT32;
typedef unsigned short      UINT16;
typedef int                INT32;
typedef short               INT16;
typedef char                INT8;
typedef unsigned char   UINT8;

typedef long                DWORD;
typedef UINT32                BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef void               *PVOID;
typedef UINT32               HANDLE;
typedef void               *HDC;
typedef void               *HBMP;
typedef void               *HWND;
typedef void               *HFONT;
typedef void               *HICON;
typedef void               *HDATA;
typedef long                LRESULT;
typedef unsigned char       UCHAR;
typedef char               *PSZ;
typedef unsigned long       ULONG;
typedef unsigned short      USHORT;
//typedef UINT32              RESID;



typedef unsigned short wchar_t;
typedef char  CHAR;
typedef short SHORT;
typedef long  LONG;

typedef wchar_t WCHAR;

#if defined(_UNICODE)
#define SIZEOF(type) (sizeof(type)/sizeof(UINT8))
typedef UINT16 *PSTR;
typedef CONST UINT16 *PCSTR;
#error code.
//typedef INT16 TCHAR;
#define TEXT(x) L ## x
#else
//#define SIZEOF(type) (sizeof(type)/sizeof(UINT16))
typedef UINT8 *PSTR;
//typedef INT8 TCHAR;
typedef CONST UINT8 *PCSTR;
#define TEXT(x) x
#endif

#ifndef LEN_FOR_NULL_CHAR
#define LEN_FOR_NULL_CHAR 1
#endif
#endif
//
// Get the address offset of the specified memeber.
//
#ifndef OFFSETOF
#define OFFSETOF(s,m)       (UINT32)&(((s *)0)->m)
#endif

#define ALIGN(val,exp)      (((val) + ((exp)-1)) & ~((exp)-1))

#define ARRAY_SIZE(a)       (sizeof(a)/sizeof(a[0]))

#define LAST_ELEMENT(x)     (&x[ARRAY_SIZE(x)-1])

#define TCHAR_SIZEOF(sz)    (sizeof(sz)/sizeof(TCHAR))
#define BOUND(x, min, max)  ( (x) < (min) ? (min) : ((x) > (max) ? (max):(x)) )

#define roundedsizeof(t)    ((sizeof(t)+sizeof(int)-1)&~(sizeof(int)-1))

#define IS_32BIT_ALIGN(nAdd) ((UINT32)(((UINT32)(nAdd)) & 0x3)==0)
#define IS_16_ALIGN(nSize)   ((UINT32)(((UINT32)(nSize))& 0x0f) == 0)

#define ABS(x) ((x<0)?(-x):(x))
typedef VOID (*PTASK_ENTRY)(PVOID pParameter);
typedef struct _MAIN_ENTRY
{
    PTASK_ENTRY pEntry;
    PVOID pData;
} MAIN_ENTRY;

typedef VOID (*PFN_CallBack)(VOID);

#ifdef __GNUC__

#ifdef _FS_SIMULATOR_
typedef    void *va_list;
#define    va_start(list, name) (void) (list = (void *)((char *)&name + \
           ((sizeof (name) + (sizeof (int) - 1)) & ~(sizeof (int) - 1))))
#define    va_arg(list, mode) ((mode *)(list = (char *)list + sizeof (mode)))[-1]
#define    va_end(list) (void)0
typedef INT8 Int8_32Aligned __attribute__ ((aligned (4)));
#endif
#endif
//#pragma pack (push)
//#pragma pack (1)
// Optimization option for compiler
//

#if defined(_M_IX86)
#undef INLINE
#define INLINE __inline
#else
#if defined(__GNUC__) // for MIPS CPU LITTLE_ENDIAN
#define INLINE inline //Do compiler directives
#else
#error "Don't define INLINE!"
#endif //__GNUC__
#endif //defined(_M_IX86) && !defined(_WIN32)

#define CPP_START  extern "C" {
#define CPP_END    }

#if defined(DEBUG) && !defined(DISABLE_ASSERTS)
#define DEBUG_BREAK()   *((UINT32 *)0) = 55   // Brute-force debug break
#else
#undef DEBUG_BREAK
#define DEBUG_BREAK()
#endif //DEBUG

#if defined(_WIN32)
#define APPENTRY  __declspec(dllexport)
#else
#define APPENTRY // todo...
#endif

#ifdef __cplusplus
# define EXTERN_C_START extern "C" {
# define EXTERN_C_END }
#else
# define EXTERN_C_START
# define EXTERN_C_END
#endif

#define SRVAPI

#endif // _H_

