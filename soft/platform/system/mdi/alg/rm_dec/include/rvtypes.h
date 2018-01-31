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


























#ifndef RVTYPES_H__
#define RVTYPES_H__

/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/rv89combo_c/cdeclib/rvtypes.h,v 1.6 2007/04/17 21:57:52 milko Exp $ */

/* This file defines the fundamental types used by the HIVE/RV interfaces. */
/* These types are intended to be portable across a wide variety of */
/* compilation environments. */
/* */
/* The following identifiers define scalar data types having a known size */
/* and known range of values, regardless of the host compiler. */
/* */
/* Name     Size     Comments      Range of Values */
/* -------+--------+-------------+-------------------------- */
/* U8        8 bits  unsigned                0 ..        255 */
/* I8        8 bits  signed               -128 ..        127 */
/* U16      16 bits  unsigned                0 ..      65535 */
/* I16      16 bits  signed             -32768 ..      32767 */
/* U32      32 bits  unsigned                0 .. 4294967295 */
/* I32      32 bits  signed        -2147483648 .. 2147483647 */
/* */
/* Bool8     8 bits  boolean                 0 ..   non-zero */
/* Bool16   16 bits  boolean                 0 ..   non-zero */
/* Bool32   32 bits  boolean                 0 ..   non-zero */
/* */
/* Enum32   32 bits  enumeration   -2147483648 .. 2147483647 */
/* */
/* F32      32 bits  floating point */
/* F64      64 bits  floating point */
/* */
/*          NOTE:  floating point representations are compiler specific */
/* */
/* The following identifiers define scalar data types whose size is */
/* compiler specific.  They should only be used in contexts where size */
/* is not relevant. */
/* */
/* RV_Boolean       boolean                 0 ..   non-zero */
/* */
/* */
/* The following pointers to the above types are also defined. */
/* */
/* PU8      PI8     PBool8      PEnum32 */
/* PU16     PI16    PBool16 */
/* PU32     PI32    PBool32 */
/* PF32     PF64    PBoolean */
/* */
/* */
/* The following macros are defined to support compilers that provide */
/* a variety of calling conventions.  They expand to nothing (i.e., empty) */
/* for compilation environments where they are not needed. */
/* */
/*     RV_CDECL */
/*     RV_FASTCALL */
/*     RV_STDCALL */
/*         These are the _WIN32 __cdecl, __fastcall and __stdcall conventions. */
/* */
/*     RV_CALL */
/* */
/*         This is the calling convention for HIVE/RV functions. */
/*         We use an explicit calling convention so that the HIVE/RV */
/*         functionality could be packaged in library, and linked to by */
/*         a codec built with an arbitrary calling convention. */
/* */
/*     RV_FREE_STORE_CALL */
/* */
/*         This represents the host compiler's calling convention for */
/*         the C++ new and delete operators. */

#include "helix_types.h"

#if (defined(ARM) || defined(_ARM_)) && defined(_WIN32_WCE)

#define RV_CDECL               __cdecl
#define RV_FASTCALL            __fastcall
#define RV_FREE_STORE_CALL
#define RV_STDCALL

#elif (defined(_WIN32) && defined(_M_IX86)) || (defined(_WIN64) && defined(_M_IA64))

#define RV_CDECL               __cdecl
#define RV_FASTCALL            __fastcall
#define RV_STDCALL             __stdcall

/* The Microsoft compiler uses the __cdecl convention for new */
/* and delete. */

#define RV_FREE_STORE_CALL     __cdecl

#else /* (defined(_WIN32) && defined(_M_IX86)) || (defined(_WIN64) && defined(_M_IA64)) */

#define RV_CDECL
#define RV_FASTCALL
#define RV_STDCALL
#define RV_FREE_STORE_CALL

#endif /* (defined(_WIN32) && defined(_M_IX86)) || (defined(_WIN64) && defined(_M_IA64)) */


/* GNU variants of STDCALL, CDECL and FASTCALL */
/* GD 1/15/01 not supported by SA linux gnu at this point */
#if defined __GNUC__ && !defined(ARM) && !defined(_ARM_) && !defined(_MAC_UNIX)

#ifdef SHEEN_VC_DEBUG
#define GNUSTDCALL __attribute__ ((stdcall))
#define GNUCDECL   __attribute__ ((cdecl))
#define GNUFASTCALL __attribute__ ((regparm (2)))
#else
#define GNUSTDCALL
#define GNUCDECL
#define GNUFASTCALL
#endif

#else /* defined __GNUC__ */

#define GNUSTDCALL
#define GNUCDECL
#define GNUFASTCALL

#endif /* defined __GNUC__ */


#define RV_CALL                RV_STDCALL


#if defined(ARM) || defined(_ARM_)

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned long       U32;
typedef signed long         I32;
typedef float               F32;
typedef double              F64;

#elif defined(_M_IX86) && !defined(_WIN32)

#error "Non-_MSC_VER or 16-bit environments are not supported at this time."

#elif defined(_M_IX86) && defined(_WIN32)

/* Win32 definitions for Windows 95 and Windows NT on Intel */
/* processors compatible with the x86 instruction set. */

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned long       U32;
typedef signed long         I32;
typedef float               F32;
typedef double              F64;

#elif defined(_M_IA64) && defined(_WIN64)

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned long       U32;
typedef signed long         I32;
typedef unsigned __int64    U64;
typedef signed __int64      I64;
typedef float               F32;
typedef double              F64;

#elif defined (_OSF1) || defined(_ALPHA)

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned int        U32;
typedef int                 I32;
typedef unsigned long       U64;
typedef signed long         I64;
typedef float               F32;
typedef double              F64;

#elif defined(__ia64)

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned int        U32;
typedef signed int          I32;
typedef unsigned long       U64;
typedef signed long         I64;
typedef float               F32;
typedef double              F64;

#elif defined(_MACINTOSH)

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned long       U32;
typedef signed long         I32;
/*  typedef unsigned __int64    U64; */
/*  typedef signed __int64      I64; */
typedef float               F32;
typedef double              F64;

#elif defined(_LINUX)

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
#if defined (__amd64__)
typedef unsigned int        U32;
typedef signed int          I32;
#else
typedef unsigned long       U32;
typedef signed long         I32;
#endif
/*  typedef unsigned __int64    U64; */
/*  typedef signed __int64      I64; */
typedef float               F32;
typedef double              F64;

#elif defined(_HPUX)

typedef unsigned char       U8;
typedef signed char         I8;
typedef unsigned short      U16;
typedef signed short        I16;
typedef unsigned long       U32;
typedef signed long         I32;
/*  typedef unsigned __int64    U64; */
/*  typedef signed __int64      I64; */
typedef float               F32;
typedef double              F64;

#else

/* These definitions should work for most other "32-bit" environments. */
/* If not, an additional "#elif" section can be added above. */
#ifdef SHEEN_VC_DEBUG
typedef unsigned char       U8;
typedef unsigned short      U16;
typedef unsigned long       U32;
/*  typedef unsigned __int64    U64; */
/*  typedef signed __int64      I64; */
#endif
typedef float               F32;
typedef double              F64;
typedef signed long         I32;
typedef signed short        I16;
typedef signed char         I8;

#endif


/* Enumerations */
/* */
/* The size of an object declared with an enumeration type is */
/* compiler-specific.  The Enum32 type can be used to represent */
/* enumeration values when the representation is significant. */

typedef I32         Enum32;


/* RV_Boolean values */
/* */
/* The "RV_Boolean" type should be used only when an object's size is not */
/* significant.  Bool8, Bool16 or Bool32 should be used elsewhere. */
/* */
/* "TRUE" is defined here for assignment purposes only, for example */
/* "is_valid = TRUE;".  As per the definition of C and C++, any */
/* non-zero value is considered to be TRUE.  So "TRUE" should not be used */
/* in tests such as "if (is_valid == TRUE)".  Use "if (is_valid)" instead. */

#undef  FALSE
#undef  TRUE

#if defined(_AIX)
/* Work around some porting problems with booleans under _AIX */

typedef int         RV_Boolean;
enum                { FALSE, TRUE };

#if !defined(_bool_is_defined)
#define _bool_is_defined

typedef int         bool;
enum                { false, true };

#endif /* !defined(_bool_is_defined) */

#else  /* defined(_AIX) */

/*typedef enum        { FALSE, TRUE } RV_Boolean; */
typedef int         RV_Boolean;
enum                { FALSE, TRUE };

#endif /* defined(_AIX) */

typedef I8          Bool8;
typedef I16         Bool16;
typedef I32         Bool32;


/* */
/* Define the "P*" pointer types */
/* */
#ifdef SHEEN_VC_DEBUG
typedef U8             *PU8;
typedef U16            *PU16;
typedef U32            *PU32;
#endif
typedef I8             *PI8;
typedef I16            *PI16;
typedef I32            *PI32;
typedef F32            *PF32;
typedef F64            *PF64;
typedef Bool8          *PBool8;
typedef Bool16         *PBool16;
typedef Bool32         *PBool32;
typedef RV_Boolean    *PBoolean;
typedef Enum32         *PEnum32;

/* NULL is defined here so that you don't always have to */
/* include <stdio.h> or some other standard include file. */

#undef  NULL
#define NULL 0


/* */
/* Define some useful macros */
/* */

#undef  ABS
#define ABS(a)          (((a) < 0) ? (-(a)) : (a))

#undef  MAX
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))

#undef  MIN
#define MIN(a, b)       (((a) < (b)) ? (a) : (b))

#undef  SIGN
#define SIGN(a)         ((a) < 0 ? (-1) : (1))

#ifndef SHEEN_VC_DEBUG
#define abs ABS
#endif


/* This alignment macro assumes n is a power of 2. */
/* Note: In order to eliminate compiler warnings on pointer  */
/* truncations, (PU8(p) - PU8(0)) is used to yield ptrdiff_t */
/* integer results.  The effect of doing this is not clearly  */
/* defined, but seems to work so far. */

#undef  RV_ALIGN
#define RV_ALIGN(p, n)  ((PU8)(p) + ((((PU8)(p) - (PU8)(0)) & ((n)-1)) ? \
                            ((n) - (((PU8)(p) - (PU8)(0)) & ((n)-1))) : 0))

#undef  RV_IS_ALIGNED
#define RV_IS_ALIGNED(p, n) (!(((PU8)(p) - (PU8)(0)) & ((n)-1)))


/* Perform byte swapping on a 16 or 32-byte value. */

#define RV_SWAP16(x) (U16( (((x) & 0xff) << 8) | (((x) & 0xff00) >> 8) ))

#define RV_SWAP32(x) (U32( (((x) & 0xff)     << 24) \
                          | (((x) & 0xff00)   <<  8) \
                          | (((x) & 0xff0000) >>  8) \
                          | (((x) >> 24) & 0xff) ))

/* STRINGIZE(x) will "return" its argument in double quotes.  Example: */
/* */
/*   #define  months30  April June September November */
/*   char *shorties = STRINGIZE(months30); */
/* */
/* results in */
/* */
/*   char *shorties = "April June September November"; */

#undef  STRINGIZE
#undef  STRINGIZE2

/* Return "x" before macro-expanding x */
#define STRINGIZE2(x) # x

/* Return "x" after macro-expanding x */
#define STRINGIZE(x) STRINGIZE2(x)


/* XCAT(x,y) will "return" its arguments pasted together with no */
/* intervening white space.  Example: */
/* */
/*   I32 XCAT(tmp, __LINE__); */
/* */
/* yields a local variable with a likely unique name */
/* */
/*   I32 tmp218; */

#undef  XCAT
#undef  XCAT2

/* Return xy before macro-expanding x and y */
#define XCAT2(x,y) x ## y

/* Return xy after macro-expanding x and y */
#define XCAT(x,y) XCAT2(x,y)

/* DEBUG_PARAM can be used to pass parameters to functions which only */
/* exist when DEBUG is enabled.  For example, */
/* */
/*    void foo */
/*        ( */
/*        U32 p1 */
/*    #if defined(DEBUG) */
/*        , char *name */
/*        , I32 p3 */
/*    #endif */
/*        ) */
/*    { */
/*        ... */
/*    } */
/* */
/*    void bar(U32 u) { foo(u DEBUG_PARAM("from bar") DEBUG_PARAM(3)); } */

#if defined(DEBUG)
#define DEBUG_PARAM(p) ,(p)
#else
#define DEBUG_PARAM(p)
#endif

// Duplicate symbol fix-ups
#define ClampTbl           RV30_ClampTbl


#endif /* RVTYPES_H__ */
