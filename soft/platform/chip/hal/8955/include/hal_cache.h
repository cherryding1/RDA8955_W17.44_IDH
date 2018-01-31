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



#ifndef _HAL_CACHE_H_
#define _HAL_CACHE_H_



#include "cs_types.h"
#include "global_macros.h"
// =============================================================================
//  MACROS
// =============================================================================
#define XCPU_CACHE_LINE_NB_BIT (4)
#define XCPU_CACHE_LINE_SIZE    EXP2(XCPU_CACHE_LINE_NB_BIT)
#define XCPU_CACHE_LINE_MASK  (XCPU_CACHE_LINE_SIZE-1)

#define XCPU_CACHE_CODE_POS     (0)
#define XCPU_CACHE_CODE_NB_BIT (4)
#define XCPU_CACHE_CODE_SIZE    EXP2(XCPU_CACHE_CODE_NB_BIT)
#define XCPU_CACHE_CODE_MASK  (XCPU_CACHE_CODE_SIZE-1)

#define XCPU_CACHE_LINE_INDEX_POS  (XCPU_CACHE_CODE_POS+XCPU_CACHE_CODE_NB_BIT)
#define XCPU_CACHE_LINE_INDEX_NB_BIT (6)
#define XCPU_CACHE_LINE_INDEX_SIZE  EXP2(XCPU_CACHE_LINE_INDEX_NB_BIT)
#define XCPU_CACHE_LINE_INDEX_MASK ((XCPU_CACHE_LINE_INDEX_SIZE - 1) << XCPU_CACHE_LINE_INDEX_POS)
#define XCPU_CACHE_LINE_INDEX(addr)   (((addr)&XCPU_CACHE_LINE_INDEX_MASK)>>XCPU_CACHE_LINE_INDEX_POS)

#define XCPU_CACHE_WAY_POS  (XCPU_CACHE_LINE_INDEX_POS+XCPU_CACHE_LINE_INDEX_NB_BIT)
#define XCPU_CACHE_WAY_NB_BIT (2)
#define XCPU_CACHE_WAY_SIZE  EXP2(XCPU_CACHE_WAY_NB_BIT)
#define XCPU_CACHE_WAY_MASK ((XCPU_CACHE_WAY_SIZE - 1) << XCPU_CACHE_WAY_POS)
#define XCPU_CACHE_WAY(addr)   (((addr)&XCPU_CACHE_WAY_MASK)>>XCPU_CACHE_WAY_POS)

#define XCPU_CACHE_SIZE   EXP2(XCPU_CACHE_LINE_NB_BIT+XCPU_CACHE_LINE_INDEX_NB_BIT)
// =============================================================================
//  TYPES
// =============================================================================

typedef struct
{
    UINT32 nop1[4];
    UINT32 cache_ins[XCPU_CACHE_WAY_SIZE];
    UINT32 nop2[2];
    UINT32 inv_ins;
    UINT32 nop3[2];
    UINT32 jr;
    UINT32 slot[2];
} hal_cacheFlushLine_T;

// =============================================================================
//  FUNCTIONS
// =============================================================================

VOID hal_DcacheFlushAll(BOOL invalid);
VOID hal_DcacheFlushAddrRange(UINT32 startAddr, UINT32 endAddr, BOOL invalid);
VOID hal_FlushDCache();
#endif // _HAL_CACHE_H_

