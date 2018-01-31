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



#ifndef HELIX_TYPES_H
#define HELIX_TYPES_H

#include "hxtypes.h"
#ifdef SHEEN_VC_DEBUG
#include <windows.h>
#endif

/*
 * HXBOOL Type definition
 *
 * HXBOOL is a boolean type
 */
#ifndef HXBOOL

#if defined(_SYMBIAN)
typedef TBool HXBOOL;
#else /* #if defined(_SYMBIAN) */
typedef int HXBOOL;
#endif /* #if defined(_SYMBIAN) */

#endif /* #ifndef HXBOOL */

/*
 * BOOL Type definition
 *
 * BOOL is another name for a HXBOOL
 */
#ifdef SHEEN_VC_DEBUG
typedef HXBOOL BOOL;
#endif


/*
 * HXDOUBLE Type definition
 *
 * HXDOUBLE is a double-precision floating-point type
 */
#ifndef HXDOUBLE

typedef double HXDOUBLE;

#endif /* #ifndef HXDOUBLE */

/*
 * HXNULL definition
 */
#ifndef HXNULL
#define HXNULL ((void *)0)
#endif

#ifdef SHEEN_VC_DEBUG
#define HX_INLINE _inline
#else
#define HX_INLINE inline
#endif

#endif /* #ifndef HELIX_TYPES_H */
