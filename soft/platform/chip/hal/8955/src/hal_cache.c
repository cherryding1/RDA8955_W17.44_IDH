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



#include "cs_types.h"
#include "string.h"
#include "hal_mem_map.h"
#include "global_macros.h"
#include "xcpu_cache.h"
#include "hal_cache.h"



#ifdef XCPU_CACHE_FLUSH_V2

// =============================================================================
//  FUNCTIONS
// =============================================================================

VOID hal_DcacheFlushAll( BOOL invalid)
{
    boot_FlushDCache( invalid);
}

VOID hal_DcacheFlushAddrRange(UINT32 startAddr, UINT32 endAddr, BOOL invalid)
{
    boot_DcacheFlushAddrRange(startAddr, endAddr, invalid);
}

#else

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
hal_cacheFlushLine_T HAL_UNCACHED_DATA hal_cacheFlushLine __attribute__((aligned(16))) = {.nop1={0,0,0,0},.cache_ins={0,0,0,0},.nop2={0,0}, .inv_ins=0,.nop3={0,0},.jr=0x03e00008, .slot={0,0}};

// =============================================================================
//  FUNCTIONS
// =============================================================================

//flush all cache lines
VOID hal_DcacheFlushAll( BOOL invalid)
{
    UINT32 status = hal_SysEnterCriticalSection();
    boot_FlushDCache(invalid);
    if(invalid)
    {
        boot_InvalidDCache();
    }
    hal_SysExitCriticalSection(status);
}


PRIVATE  UINT32  __attribute__ ((noinline)) _hal_DcacheFlushLineAsm()
{
    __asm__(
        "lw $2, %0 \n"
        "jr $2 \n"
        : /* no output */ : "g"(&hal_cacheFlushLine)
        );
}

//flush one dedicated line indicated by para index
PRIVATE VOID __attribute__ ((noinline))  _hal_DcacheFlushLine(UINT32 index, BOOL invalid)
{
    hal_cacheFlushLine_T *_func = (hal_cacheFlushLine_T *)&hal_cacheFlushLine;

   UINT32 status = hal_SysEnterCriticalSection();

     _func->cache_ins[0] = 0xbc000003|(index);   
     _func->cache_ins[1] = 0xbc000003|(index)|(1<<XCPU_CACHE_WAY_POS);   
     _func->cache_ins[2] = 0xbc000003|(index)|(2<<XCPU_CACHE_WAY_POS);  
     _func->cache_ins[3] = 0xbc000003|(index)|(3<<XCPU_CACHE_WAY_POS);
     _func->inv_ins = 0x03e00008;
     
    if(invalid)
    {
        _func->inv_ins = 0xbc000006 | (index);
    }
    
    _hal_DcacheFlushLineAsm();
    
    hal_SysExitCriticalSection(status);

}

//flush cache by address range

#ifdef HAL_FLUSH_ADDR_BY_ALL
VOID hal_DcacheFlushAddrRange(UINT32 startAddr, UINT32 endAddr,BOOL invalid)
{
    UINT32 addr,j;
    UINT32 startAddrMask = startAddr  & ~XCPU_CACHE_LINE_MASK;
    UINT32 endAddrMask = (endAddr + XCPU_CACHE_LINE_SIZE -1) & ~XCPU_CACHE_LINE_MASK;
    hal_DcacheFlushAll(invalid);
}
#else

VOID hal_DcacheFlushAddrRange(UINT32 startAddr, UINT32 endAddr,BOOL invalid)
{
    UINT32 addr,j;
    UINT32 startAddrMask = startAddr  & ~XCPU_CACHE_LINE_MASK;
    UINT32 endAddrMask = (endAddr + XCPU_CACHE_LINE_SIZE -1) & ~XCPU_CACHE_LINE_MASK;
    
    if(endAddr - startAddr >= XCPU_CACHE_SIZE)
    {
        hal_DcacheFlushAll(invalid);
    }
    else
    {
        for(addr = startAddrMask; addr < endAddrMask; addr += XCPU_CACHE_LINE_SIZE)
        {
           _hal_DcacheFlushLine(addr & XCPU_CACHE_LINE_INDEX_MASK, invalid);
        }
    }
}

#endif

#endif
