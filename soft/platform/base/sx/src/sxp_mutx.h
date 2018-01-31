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



#ifndef __SXP_MUTX_H__
#define __SXP_MUTX_H__

#include "sxs_type.h"
#include "sxr_cnf.h"


#define SXR_NO_MUTEX        0xFF
#define SXR_MUTEX_ALLOCATED 0xFE

typedef struct
{
    u8 SemaId;
    u8 TaskId;
    u8 Count;
    /// chaining for allocation
    u8 Next;
    /// debug info
    u32 CallerAdd;
} Mutex_t;


#ifdef __SXR_MUTX_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif
DefExtern struct
{
    Mutex_t    Queue    [SXR_NB_MAX_MUTEX];
    u8         IdxFree;
    u8         Nb;
} sxr_Mutex;

#undef DefExtern

#ifdef SX_NO_ASSERT
#define SX_MUTX_ASSERT(BOOL, format, ...)
#else
#define SX_MUTX_ASSERT(BOOL, format, ...) \
    if (!(BOOL)) { \
        sxr_CheckMutex(); \
        hal_DbgAssert(format, ##__VA_ARGS__); \
    }
#endif

#endif

