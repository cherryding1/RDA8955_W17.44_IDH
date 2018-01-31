//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
/// @file spal_misc.c
///
/// This file contains miscellanous functions for SPAL
/// !!! THIS FILE IS COMPILED IN MIPS32 !!!
///
/// @date 27/06/07
/// @author Laurent Bastide, Guillaume LEBRUN
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

// include
#include "cs_types.h"
#if(BCPU_CACHE_MODE_WRITE_BACK==1)
#include "spal_cache.h"
#endif
#include "spal_irq.h"
//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
#if(BCPU_CACHE_MODE_WRITE_BACK==1)

// =============================================================================
// spal_FlushDCache
// -----------------------------------------------------------------------------
///  flush All DCache 
/// @param invalid used to invalid the cache or not(FALSE by default)
// =============================================================================
VOID spal_FlushDCache(BOOL invalid)
{
    UINT32 j;
    UINT32 i,rs;
    
    for(i=0;i<BCPU_CACHE_SIZE; i+=BCPU_CACHE_LINE_SIZE)
    {
        UINT32 critical_sec = spal_IrqEnterCriticalSection();

        for(j=0; j<BCPU_CACHE_WAY_SIZE; j++)
        {
            rs = i + (j << BCPU_CACHE_WAY_POS);
            asm volatile ("cache 3,%0"::"r"(rs));
        }
        if(invalid)
        {
              asm volatile ("cache 7,%0"::"r"(i));
        }
        spal_IrqExitCriticalSection(critical_sec);
    }
}

// =============================================================================
// spal_DcacheFlushAddrRange
// -----------------------------------------------------------------------------
///  flush DCache by address 
/// @param startAddr define the start address
/// @param endAddr define the end address
/// @param invalid used to invalid the cache or not(FALSE by default)
// =============================================================================
VOID spal_DcacheFlushAddrRange(UINT32 startAddr, UINT32 endAddr, BOOL invalid)
{
    UINT32 j;
    UINT32 startAddrMask = startAddr  & ~BCPU_CACHE_LINE_MASK;
    UINT32 endAddrMask = (endAddr + BCPU_CACHE_LINE_SIZE -1) & ~BCPU_CACHE_LINE_MASK;
    UINT32 addr, rs;
    if(endAddr - startAddr >= BCPU_CACHE_SIZE)
    {
        spal_FlushDCache(invalid);
    }
    else
    {
        for(addr = startAddrMask; addr < endAddrMask; addr += BCPU_CACHE_LINE_SIZE)
        {

            UINT32 critical_sec = spal_IrqEnterCriticalSection();
            
            for(j=0; j<BCPU_CACHE_WAY_SIZE; j++)
            {
                rs = addr + (j << BCPU_CACHE_WAY_POS);
                asm volatile ("cache 3,%0"::"r"(rs));
            }

             if(invalid)
            {
                  asm volatile ("cache 7,%0"::"r"(addr));
            }
             
            spal_IrqExitCriticalSection(critical_sec);
        }
           
    }
    
}
#endif

//======================================================================
// spal_InvalidateDcache
//----------------------------------------------------------------------
// Data cache invalidation
// !!! THIS FUNCTION SHOULD BE COMPILED IN MIPS32 !!!
//
//======================================================================
PROTECTED VOID spal_InvalidateDcache_asm(VOID)
{
    asm volatile (".align    4");
    asm volatile ("nop ");
    asm volatile (".align    4");
    asm volatile (".word 0xbc000002");
    asm volatile ("nop ");
    asm volatile ("nop ");
    asm volatile ("nop ");
}

//======================================================================
// spal_InvalidateCache
//----------------------------------------------------------------------
// Both caches invalidation
// !!! THIS FUNCTION SHOULD BE COMPILED IN MIPS32 !!!
//
//======================================================================
PROTECTED VOID spal_InvalidateCache_asm(VOID)
{
    asm volatile (".align    4");
    asm volatile ("nop ");
    asm volatile (".align    4");
    asm volatile (".word 0xbc000000");
    asm volatile ("nop ");
    asm volatile ("nop ");
    asm volatile ("nop ");
}

PROTECTED VOID spal_InvalidateDcache()
{

#if(BCPU_CACHE_MODE_WRITE_BACK==1)
    spal_FlushDCache(TRUE);
#else
    spal_InvalidateDcache_asm();
#endif
}

PROTECTED VOID spal_InvalidateCache(VOID)
{
    UINT32 critical_sec = spal_IrqEnterCriticalSection();
#if(BCPU_CACHE_MODE_WRITE_BACK==1)
    spal_FlushDCache(TRUE);
#endif
    spal_InvalidateCache_asm();
    spal_IrqExitCriticalSection(critical_sec);
}
