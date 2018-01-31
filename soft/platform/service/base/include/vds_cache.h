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

#ifndef _VDS_CACHE_H_
#define _VDS_CACHE_H_

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)


// error code define.
// vds uninit.
#define ERR_VDS_CACHE_VDS_UNINIT                 -1

// malloc failed.
#define ERR_VDS_CACHE_VDS_MALLOC_FIALED          -2

// read failed.
#define ERR_VDS_CACHE_READ_FAILED                -3

// write failed.
#define ERR_VDS_CACHE_WRITE_FAILED               -4

// cache node is null.
#define ERR_VDS_CACHE_NODE_NULL                  -5

// vds cache uninit.
#define ERR_VDS_CACHE_UNINIT                     -6

// unallowed flush
#define ERR_VDS_CACHE_UNALLOWED_FLUSH            -7

//// no more free pb
#define _ERR_VDS_CACHE_NO_MORE_FREE_PB           -8

#define ERR_VDS_CACHE_GCING                                  -9
#define ERR_VDS_FLUSH_TIMEOUT                              -10

INT32 VDS_CacheInit(VOID);
INT32 VDS_CacheWriteBlock( UINT32 iPartId, UINT32 iBlkNr, UINT8*pBuff );
INT32 VDS_CacheReadBlock( UINT32 iPartId, UINT32 iBlkNr, UINT8* pBuff );
INT32 VDS_CacheRevertBlock( UINT32 iPartId, UINT32 iBlkNr);

VOID VDS_CacheFlush(VOID);
VOID VDS_CacheFlushAll(VOID);
VOID VDS_CacheFlushAllDirect(UINT32 iTime);
VOID VDS_FlushCacheAllFly(VOID);
VOID VDS_CacheFlushUnAllowed(VOID);
VOID VDS_CacheFlushAllowed(VOID);
BOOL VDS_IsCacheFlushAllowed(VOID);
BOOL VDS_CacheNodeIsLess(VOID);
VOID VDS_CacheFlushDisable(UINT32 iKeepTime);
VOID VDS_CacheFlushEnable(VOID);
UINT32 VDS_CacheGetBlkCnt(VOID);
VOID VDS_CacheSendFlushEvent(VOID);
VOID VDS_CacheSetEraseAll(VOID);

#else


#endif
#endif

