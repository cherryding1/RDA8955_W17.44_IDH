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



#ifndef STDDEF_H
#define STDDEF_H

#  include "cs_types.h"
/*
typedef unsigned int    size_t;
typedef int ptrdiff_t;
typedef int ssize_t;

#ifdef WITH_LONG_LONG
typedef unsigned long long uintmax_t;
typedef long long intmax_t;
#else
typedef unsigned long uintmax_t;
typedef long intmax_t;
#endif

#define __P(protos)  protos
#define INT_MAX (char *)(1<<31)
#ifdef WITH_LONG_LONG
#define INTMAX_MAX 0x7fffffffffffffff
#define INTMAX_MIN -(0x7fffffffffffffff-1)
#define UINTMAX_MAX 0xffffffffffffffff
#else
#define INTMAX_MAX 0x7fffffff
#define INTMAX_MIN -(0x7fffffff-1)
#define UINTMAX_MAX 0xffffffff
#endif
#define LONG_MAX 0x7fffffff
#define LONG_MIN -(0x7fffffff-1)
#define ULONG_MAX 0xffffffff

#ifndef NULL
#define NULL    0
#endif

#define offsetof(TYPE, MEMBER) ((u32) &((TYPE *)0)->MEMBER)
#define offsetofvar(VAR, MEMBER) (((u32) &(VAR.MEMBER)) - ((u32) &VAR))
*/
/* Remove const cast-away warnings from gcc -Wcast-qual */
// #define __UNCONST(a) ((void *)(unsigned long)(const void *)(a))

#endif
