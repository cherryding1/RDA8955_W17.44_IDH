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

#include "global_macros.h"
#include "sys_irq.h"
#include "string.h"
#include "boot_usb.h"
#include "bootp_debug.h"
#include "boot_cache.h"
#include "xcpu_cache.h"
#include "hal_sys.h"
// =============================================================================
//  VARIABLES
// =============================================================================


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
//  VARIABLES
// =============================================================================


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// boot_FlushDCache
// -----------------------------------------------------------------------------
/// BOOT flush All DCache 
/// @param invalid used to invalid the cache or not(FALSE by default)
// =============================================================================
#ifdef XCPU_CACHE_FLUSH_V2
VOID boot_FlushDCache(BOOL invalid)
{
    UINT32 j;
    UINT32 i,rs;
    
    for(i=0;i<XCPU_CACHE_SIZE; i+=XCPU_CACHE_LINE_SIZE)
    {
        UINT32 status    = hwp_sysIrq->SC;
        asm volatile ("nop;nop;");

        for(j=0; j<XCPU_CACHE_WAY_SIZE; j++)
        {
            rs = i + (j << XCPU_CACHE_WAY_POS);
            asm volatile ("cache 3,%0"::"r"(rs));
        }
        if(invalid)
        {
              asm volatile ("cache 7,%0"::"r"(i));
        }
        asm volatile ("");
        hwp_sysIrq->SC       = status;
        asm volatile ("");
    }
}

// =============================================================================
// boot_DcacheFlushAddrRange
// -----------------------------------------------------------------------------
/// BOOT flush DCache by address 
/// @param startAddr define the start address
/// @param endAddr define the end address
/// @param invalid used to invalid the cache or not(FALSE by default)
// =============================================================================
VOID boot_DcacheFlushAddrRange(UINT32 startAddr, UINT32 endAddr, BOOL invalid)
{
    UINT32 j;
    UINT32 startAddrMask = startAddr  & ~XCPU_CACHE_LINE_MASK;
    UINT32 endAddrMask = (endAddr + XCPU_CACHE_LINE_SIZE -1) & ~XCPU_CACHE_LINE_MASK;
    UINT32 addr, rs;
    if(endAddr - startAddr >= XCPU_CACHE_SIZE)
    {
        boot_FlushDCache(invalid);
    }
    else
    {
        for(addr = startAddrMask; addr < endAddrMask; addr += XCPU_CACHE_LINE_SIZE)
        {

            UINT32 status    = hwp_sysIrq->SC;
            asm volatile ("nop;nop;");           
            
            for(j=0; j<XCPU_CACHE_WAY_SIZE; j++)
            {
                rs = addr + (j << XCPU_CACHE_WAY_POS);
                asm volatile ("cache 3,%0"::"r"(rs));
            }

             if(invalid)
            {
                  asm volatile ("cache 7,%0"::"r"(addr));
            }
            asm volatile ("");
            hwp_sysIrq->SC       = status;
            asm volatile ("");
            }
           
      }
    
}

#else

// =============================================================================
// boot_FlushDCache
// -----------------------------------------------------------------------------
/// BOOT flush All DCache 
/// @param invalid used to invalid the cache or not(FALSE by default)
// =============================================================================
VOID boot_FlushDCache(BOOL invalid)
{
        asm volatile (".align    4");
        asm volatile ("nop");
        asm volatile (".align    4");
        asm volatile (".word 0xbc000003");
        asm volatile (".word 0xbc000403");
        asm volatile (".word 0xbc000803");
        asm volatile (".word 0xbc000c03");
        asm volatile (".word 0xbc000013");
        asm volatile (".word 0xbc000413");
        asm volatile (".word 0xbc000813");
        asm volatile (".word 0xbc000c13");
        asm volatile (".word 0xbc000023");
        asm volatile (".word 0xbc000423");
        asm volatile (".word 0xbc000823");
        asm volatile (".word 0xbc000c23");
        asm volatile (".word 0xbc000033");
        asm volatile (".word 0xbc000433");
        asm volatile (".word 0xbc000833");
        asm volatile (".word 0xbc000c33");
        asm volatile (".word 0xbc000043");
        asm volatile (".word 0xbc000443");
        asm volatile (".word 0xbc000843");
        asm volatile (".word 0xbc000c43");
        asm volatile (".word 0xbc000053");
        asm volatile (".word 0xbc000453");
        asm volatile (".word 0xbc000853");
        asm volatile (".word 0xbc000c53");
        asm volatile (".word 0xbc000063");
        asm volatile (".word 0xbc000463");
        asm volatile (".word 0xbc000863");
        asm volatile (".word 0xbc000c63");
        asm volatile (".word 0xbc000073");
        asm volatile (".word 0xbc000473");
        asm volatile (".word 0xbc000873");
        asm volatile (".word 0xbc000c73");
        asm volatile (".word 0xbc000083");
        asm volatile (".word 0xbc000483");
        asm volatile (".word 0xbc000883");
        asm volatile (".word 0xbc000c83");
        asm volatile (".word 0xbc000093");
        asm volatile (".word 0xbc000493");
        asm volatile (".word 0xbc000893");
        asm volatile (".word 0xbc000c93");
        asm volatile (".word 0xbc0000a3");
        asm volatile (".word 0xbc0004a3");
        asm volatile (".word 0xbc0008a3");
        asm volatile (".word 0xbc000ca3");
        asm volatile (".word 0xbc0000b3");
        asm volatile (".word 0xbc0004b3");
        asm volatile (".word 0xbc0008b3");
        asm volatile (".word 0xbc000cb3");
        asm volatile (".word 0xbc0000c3");
        asm volatile (".word 0xbc0004c3");
        asm volatile (".word 0xbc0008c3");
        asm volatile (".word 0xbc000cc3");
        asm volatile (".word 0xbc0000d3");
        asm volatile (".word 0xbc0004d3");
        asm volatile (".word 0xbc0008d3");
        asm volatile (".word 0xbc000cd3");
        asm volatile (".word 0xbc0000e3");
        asm volatile (".word 0xbc0004e3");
        asm volatile (".word 0xbc0008e3");
        asm volatile (".word 0xbc000ce3");
        asm volatile (".word 0xbc0000f3");
        asm volatile (".word 0xbc0004f3");
        asm volatile (".word 0xbc0008f3");
        asm volatile (".word 0xbc000cf3");
        asm volatile (".word 0xbc000103");
        asm volatile (".word 0xbc000503");
        asm volatile (".word 0xbc000903");
        asm volatile (".word 0xbc000d03");
        asm volatile (".word 0xbc000113");
        asm volatile (".word 0xbc000513");
        asm volatile (".word 0xbc000913");
        asm volatile (".word 0xbc000d13");
        asm volatile (".word 0xbc000123");
        asm volatile (".word 0xbc000523");
        asm volatile (".word 0xbc000923");
        asm volatile (".word 0xbc000d23");
        asm volatile (".word 0xbc000133");
        asm volatile (".word 0xbc000533");
        asm volatile (".word 0xbc000933");
        asm volatile (".word 0xbc000d33");
        asm volatile (".word 0xbc000143");
        asm volatile (".word 0xbc000543");
        asm volatile (".word 0xbc000943");
        asm volatile (".word 0xbc000d43");
        asm volatile (".word 0xbc000153");
        asm volatile (".word 0xbc000553");
        asm volatile (".word 0xbc000953");
        asm volatile (".word 0xbc000d53");
        asm volatile (".word 0xbc000163");
        asm volatile (".word 0xbc000563");
        asm volatile (".word 0xbc000963");
        asm volatile (".word 0xbc000d63");
        asm volatile (".word 0xbc000173");
        asm volatile (".word 0xbc000573");
        asm volatile (".word 0xbc000973");
        asm volatile (".word 0xbc000d73");
        asm volatile (".word 0xbc000183");
        asm volatile (".word 0xbc000583");
        asm volatile (".word 0xbc000983");
        asm volatile (".word 0xbc000d83");
        asm volatile (".word 0xbc000193");
        asm volatile (".word 0xbc000593");
        asm volatile (".word 0xbc000993");
        asm volatile (".word 0xbc000d93");
        asm volatile (".word 0xbc0001a3");
        asm volatile (".word 0xbc0005a3");
        asm volatile (".word 0xbc0009a3");
        asm volatile (".word 0xbc000da3");
        asm volatile (".word 0xbc0001b3");
        asm volatile (".word 0xbc0005b3");
        asm volatile (".word 0xbc0009b3");
        asm volatile (".word 0xbc000db3");
        asm volatile (".word 0xbc0001c3");
        asm volatile (".word 0xbc0005c3");
        asm volatile (".word 0xbc0009c3");
        asm volatile (".word 0xbc000dc3");
        asm volatile (".word 0xbc0001d3");
        asm volatile (".word 0xbc0005d3");
        asm volatile (".word 0xbc0009d3");
        asm volatile (".word 0xbc000dd3");
        asm volatile (".word 0xbc0001e3");
        asm volatile (".word 0xbc0005e3");
        asm volatile (".word 0xbc0009e3");
        asm volatile (".word 0xbc000de3");
        asm volatile (".word 0xbc0001f3");
        asm volatile (".word 0xbc0005f3");
        asm volatile (".word 0xbc0009f3");
        asm volatile (".word 0xbc000df3");
        asm volatile (".word 0xbc000203");
        asm volatile (".word 0xbc000603");
        asm volatile (".word 0xbc000a03");
        asm volatile (".word 0xbc000e03");
        asm volatile (".word 0xbc000213");
        asm volatile (".word 0xbc000613");
        asm volatile (".word 0xbc000a13");
        asm volatile (".word 0xbc000e13");
        asm volatile (".word 0xbc000223");
        asm volatile (".word 0xbc000623");
        asm volatile (".word 0xbc000a23");
        asm volatile (".word 0xbc000e23");
        asm volatile (".word 0xbc000233");
        asm volatile (".word 0xbc000633");
        asm volatile (".word 0xbc000a33");
        asm volatile (".word 0xbc000e33");
        asm volatile (".word 0xbc000243");
        asm volatile (".word 0xbc000643");
        asm volatile (".word 0xbc000a43");
        asm volatile (".word 0xbc000e43");
        asm volatile (".word 0xbc000253");
        asm volatile (".word 0xbc000653");
        asm volatile (".word 0xbc000a53");
        asm volatile (".word 0xbc000e53");
        asm volatile (".word 0xbc000263");
        asm volatile (".word 0xbc000663");
        asm volatile (".word 0xbc000a63");
        asm volatile (".word 0xbc000e63");
        asm volatile (".word 0xbc000273");
        asm volatile (".word 0xbc000673");
        asm volatile (".word 0xbc000a73");
        asm volatile (".word 0xbc000e73");
        asm volatile (".word 0xbc000283");
        asm volatile (".word 0xbc000683");
        asm volatile (".word 0xbc000a83");
        asm volatile (".word 0xbc000e83");
        asm volatile (".word 0xbc000293");
        asm volatile (".word 0xbc000693");
        asm volatile (".word 0xbc000a93");
        asm volatile (".word 0xbc000e93");
        asm volatile (".word 0xbc0002a3");
        asm volatile (".word 0xbc0006a3");
        asm volatile (".word 0xbc000aa3");
        asm volatile (".word 0xbc000ea3");
        asm volatile (".word 0xbc0002b3");
        asm volatile (".word 0xbc0006b3");
        asm volatile (".word 0xbc000ab3");
        asm volatile (".word 0xbc000eb3");
        asm volatile (".word 0xbc0002c3");
        asm volatile (".word 0xbc0006c3");
        asm volatile (".word 0xbc000ac3");
        asm volatile (".word 0xbc000ec3");
        asm volatile (".word 0xbc0002d3");
        asm volatile (".word 0xbc0006d3");
        asm volatile (".word 0xbc000ad3");
        asm volatile (".word 0xbc000ed3");
        asm volatile (".word 0xbc0002e3");
        asm volatile (".word 0xbc0006e3");
        asm volatile (".word 0xbc000ae3");
        asm volatile (".word 0xbc000ee3");
        asm volatile (".word 0xbc0002f3");
        asm volatile (".word 0xbc0006f3");
        asm volatile (".word 0xbc000af3");
        asm volatile (".word 0xbc000ef3");
        asm volatile (".word 0xbc000303");
        asm volatile (".word 0xbc000703");
        asm volatile (".word 0xbc000b03");
        asm volatile (".word 0xbc000f03");
        asm volatile (".word 0xbc000313");
        asm volatile (".word 0xbc000713");
        asm volatile (".word 0xbc000b13");
        asm volatile (".word 0xbc000f13");
        asm volatile (".word 0xbc000323");
        asm volatile (".word 0xbc000723");
        asm volatile (".word 0xbc000b23");
        asm volatile (".word 0xbc000f23");
        asm volatile (".word 0xbc000333");
        asm volatile (".word 0xbc000733");
        asm volatile (".word 0xbc000b33");
        asm volatile (".word 0xbc000f33");
        asm volatile (".word 0xbc000343");
        asm volatile (".word 0xbc000743");
        asm volatile (".word 0xbc000b43");
        asm volatile (".word 0xbc000f43");
        asm volatile (".word 0xbc000353");
        asm volatile (".word 0xbc000753");
        asm volatile (".word 0xbc000b53");
        asm volatile (".word 0xbc000f53");
        asm volatile (".word 0xbc000363");
        asm volatile (".word 0xbc000763");
        asm volatile (".word 0xbc000b63");
        asm volatile (".word 0xbc000f63");
        asm volatile (".word 0xbc000373");
        asm volatile (".word 0xbc000773");
        asm volatile (".word 0xbc000b73");
        asm volatile (".word 0xbc000f73");
        asm volatile (".word 0xbc000383");
        asm volatile (".word 0xbc000783");
        asm volatile (".word 0xbc000b83");
        asm volatile (".word 0xbc000f83");
        asm volatile (".word 0xbc000393");
        asm volatile (".word 0xbc000793");
        asm volatile (".word 0xbc000b93");
        asm volatile (".word 0xbc000f93");
        asm volatile (".word 0xbc0003a3");
        asm volatile (".word 0xbc0007a3");
        asm volatile (".word 0xbc000ba3");
        asm volatile (".word 0xbc000fa3");
        asm volatile (".word 0xbc0003b3");
        asm volatile (".word 0xbc0007b3");
        asm volatile (".word 0xbc000bb3");
        asm volatile (".word 0xbc000fb3");
        asm volatile (".word 0xbc0003c3");
        asm volatile (".word 0xbc0007c3");
        asm volatile (".word 0xbc000bc3");
        asm volatile (".word 0xbc000fc3");
        asm volatile (".word 0xbc0003d3");
        asm volatile (".word 0xbc0007d3");
        asm volatile (".word 0xbc000bd3");
        asm volatile (".word 0xbc000fd3");
        asm volatile (".word 0xbc0003e3");
        asm volatile (".word 0xbc0007e3");
        asm volatile (".word 0xbc000be3");
        asm volatile (".word 0xbc000fe3");
        asm volatile (".word 0xbc0003f3");
        asm volatile (".word 0xbc0007f3");
        asm volatile (".word 0xbc000bf3");
        asm volatile (".word 0xbc000ff3");
        asm volatile ("nop");
        asm volatile ("nop");
        if(invalid)
        {
            asm volatile ("cache 2,0");
            asm volatile ("nop");
            asm volatile ("nop");
        }
}


// =============================================================================
// boot_DcacheFlushAddrRange
// -----------------------------------------------------------------------------
/// BOOT flush DCache by address 
/// @param startAddr define the start address
/// @param endAddr define the end address
/// @param invalid used to invalid the cache or not(FALSE by default)
// =============================================================================
VOID boot_DcacheFlushAddrRange(UINT32 startAddr, UINT32 endAddr, BOOL invalid)
{
    UINT32 addr,j;
    UINT32 startAddrMask = startAddr  & ~XCPU_CACHE_LINE_MASK;
    UINT32 endAddrMask = (endAddr + XCPU_CACHE_LINE_SIZE -1) & ~XCPU_CACHE_LINE_MASK;
    boot_cacheFlushLine_T  boot_cacheFlushLine;
    boot_cacheFlushLine_T *_func = (boot_cacheFlushLine_T *)HAL_SYS_GET_UNCACHED_ADDR(&boot_cacheFlushLine);
    
    if(endAddr - startAddr >= XCPU_CACHE_SIZE)
    {
        UINT32 status    = hwp_sysIrq->SC;
        asm volatile ("nop;nop");
        boot_FlushDCache(invalid);
        asm volatile ("");
        hwp_sysIrq->SC       = status;
        asm volatile ("");
    }
    else
    {
        memset(&boot_cacheFlushLine,0,sizeof(boot_cacheFlushLine_T));
        boot_cacheFlushLine.jr = 0x03e00008;
        memcpy( (VOID*)_func, // destination
            (VOID*) &boot_cacheFlushLine, // source
            sizeof(boot_cacheFlushLine_T)); // size
            
        for(addr = startAddrMask; addr < endAddrMask; addr += XCPU_CACHE_LINE_SIZE)
        {
            for(j=0; j<XCPU_CACHE_WAY_SIZE; j++)
            {
                boot_cacheFlushLine.cache_ins[j] = 0xbc000003 |(addr & XCPU_CACHE_LINE_INDEX_MASK) |(j<<XCPU_CACHE_WAY_POS);
                _func->cache_ins[j] = boot_cacheFlushLine.cache_ins[j];
            }
            boot_cacheFlushLine.inv_ins = 0;
            
            if(invalid)
            {
                boot_cacheFlushLine.inv_ins = 0xbc000006 |(addr & XCPU_CACHE_LINE_INDEX_MASK);
            }
            _func->inv_ins = boot_cacheFlushLine.inv_ins;
            
            UINT32 status    = hwp_sysIrq->SC;
            asm volatile ("nop;nop;");
            asm volatile ("jalr %0"::"r"(_func));
            asm volatile ("");
            asm volatile ("nop;");
            hwp_sysIrq->SC       = status;
            asm volatile ("");
            
        }
    }
}
#endif
// =============================================================================
// boot_IrqDispatch
// -----------------------------------------------------------------------------
/// BOOT romed IRQ dispatch.
/// Exceptions are handled in the rom (boot_rom.S) assemlby code.
/// Here, we check the cause is really USB before calling the USB handler,
/// otherwise sending an error if the interrupt cause is not supported.
// =============================================================================
PROTECTED VOID boot_IrqDispatch(VOID)
{
    if (hwp_sysIrq->Cause & SYS_IRQ_SYS_IRQ_USBC)
    {
        // Call the USB handler if, and only if,
        // the interruption is really for the USB.
        boot_UsbIrqHandler(SYS_IRQ_USBC);
    }
    else
    {
        // We received an unsupported (here) interruption
        // BOOT_PROFILE_PULSE(BOOT_ROMED_XCPU_ERROR);
        // Stay here.
        while (1);
    }
}

