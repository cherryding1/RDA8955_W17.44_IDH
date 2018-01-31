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



#ifndef _BOOT_CACHE_H_
#define _BOOT_CACHE_H_



#include "cs_types.h"
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
#include "hal_cache.h"
#endif
// =============================================================================
//  MACROS
// =============================================================================
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
#define  BOOT_DST_LOC(n)   (UINT32*)(((UINT32)n)|0xa0000000)
#else
#define  BOOT_DST_LOC(n) (UINT32*)n
#endif

#if(XCPU_CACHE_MODE_WRITE_BACK==1)
// =============================================================================
//  TYPES
// =============================================================================

typedef struct
{
    UINT32 nop1[4];
    UINT32 cache_ins[XCPU_CACHE_WAY_SIZE];
    UINT32 nop2[2];
    UINT32 inv_ins;
    UINT32 nop3[3];
    UINT32 jr;
    UINT32 slot[1];
} boot_cacheFlushLine_T;

// =============================================================================
//  FUNCTIONS
// =============================================================================
// =============================================================================
// boot_probe_cache
// -----------------------------------------------------------------------------
/// probe I/D cache size
/// @param
// =============================================================================
VOID boot_probe_cache();

// =============================================================================
// boot_FlushDCache
// -----------------------------------------------------------------------------
/// BOOT flush All DCache
/// @param invalid used to invalid the cache or not(FALSE by default)
// =============================================================================
VOID boot_FlushDCache(BOOL invalid);

// =============================================================================
// boot_DcacheFlushAddrRange
// -----------------------------------------------------------------------------
/// BOOT flush DCache by address
/// @param startAddr define the start address
/// @param endAddr define the end address
/// @param invalid used to invalid the cache or not(FALSE by default)
// =============================================================================
VOID boot_DcacheFlushAddrRange(UINT32 startAddr, UINT32 endAddr, BOOL invalid);

#endif //XCPU_CACHE_MODE_WRITE_BACK

#endif // _BOOT_CACHE_H_

