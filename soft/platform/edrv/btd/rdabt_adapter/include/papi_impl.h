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


#ifndef PAPI_IMPL_H
#define PAPI_IMPL_H

#define pNULL NULL

#include "cs_types.h"
/*
#ifndef __CS_TYPE_H__
#ifndef __size_t
#define __size_t   1
//typedef u_int32  size_t;
#endif
#endif*/
typedef u_int32  t_pMutex;
typedef u_int32  t_pTimer;
typedef u_int32  t_pEvent;


//#define pMemcpy memcpy
#define pMemcpy memcpy
#define pMemset memset
#define pMemcmp memcmp
#define pMemchr memchr
#define pStrcmp strcmp
#define pStrncmp strncmp
#define pStrlen strlen
#define pStrcpy strcpy
#define pStrncpy strncpy

/* platform timer is no used in mtk */
#define pSetTimer(millisec)       (0)
#define pChangeTimer(id, millisec)       (0)
#define pCancelTimer(id)       (0)
#define pGetCurrentTicks()              (0)
#define pGetTickDifference(start, end)      (0)

#define pWakeUpScheduler()              rdabt_send_notify()

#if pDEBUG
#define pDebugPrintfEX(args)                (RDA_Debug_PrintEX args)
#define pDebugPrintf(args)                   (RDA_Debug_Print args)
#define pDebugDumpEX(level,layer,buf,len)    (pDebugDumpFuncEX((level),(layer),(buf),(len)))
#define pDebugDump(level,buf,len)            (pDebugDumpFunc((level),(buf),(len)))
#define pDebugCheck()                           (pDebugCheckFunc())
#endif

#define  pMutexCreate(initState)       (0)
#define  pMutexFree(mutex)              (0)
#define  pMutexLock(mutex)              (0)
#define  pMutexUnlock(mutex)          (0)


void rdabt_send_notify(void);
int RDA_Debug_PrintEX(int logLevel,int logLayer, char *formatString, ...) ;
int RDA_Debug_Print(int logLevel, char *formatString, ... );
int pDebugDumpFunc(int logLevel, u_int8 *buffer, u_int32 len);
int pDebugDumpFuncEX(int logLevel, int logLayer, u_int8 *buffer, u_int32 len) ;
int pDebugCheckFunc(void) ;

extern void *memcpy(void *dstpp, const void *srcpp, size_t len);
#endif /* PAPI_IMPL_H */