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

#include "chip_id.h"
#include "global_macros.h"
#include "cfg_regs.h"
#include "sys_ctrl.h"
#include "mem_bridge.h"
#include "boot_nand.h"
#include "hal_mem_map.h"
#include "bootp_host.h"
#include "boot_sector.h"

// =============================================================================
// MACROS
// =============================================================================


/// Linker section where the Boot Sector main function must be mapped:
/// Beginning of the first sector of the flash.
#define BOOT_SECTOR_NAND_SECTION_START  __attribute__((section (".boot_sector_nand_start")))


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// EXPORTS
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

#define SDRAM_MEM_BASE_ADDR  0xa2000000
#define SDRAM_REG_BASE_ADDR  0xa1ae4000

#define SDRAM_MEM_SIZE       0x2000000


int sdram_ini()
{
    volatile int *addr;
    int i;
    volatile int temp;

    //### disable mpmc to config
    addr = (int *)(SDRAM_REG_BASE_ADDR);
    *addr = 0;

    //### set little endian
    addr = (int *)(SDRAM_REG_BASE_ADDR + 8);
    *addr = 0;

    //### set clock control
    addr = (int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 3;

    //sprintf(msg,"sdram_reg[0x20]: %08x; \n",(unsigned int)*addr);
    //strcpy(msg,"sdram ini ...\n");      //boot_UartMonitorSend(msg, strlen(msg), &checksum);

    //### set tDAL
    addr = (int *)(SDRAM_REG_BASE_ADDR + 0x40);
    *addr = 4;

    i = 1000;
    while(i--);

    addr = (int *)(SDRAM_REG_BASE_ADDR + 0x24);
    *addr = 0x400;

    addr = (int *)(SDRAM_REG_BASE_ADDR + 0x28);
    *addr = 1;


    ///Dyn config 0
    addr = (int *)(SDRAM_REG_BASE_ADDR + 0x100);
    ///*addr = 0x480;
    *addr = 0x680;

    //### enable mpmc to config
    addr = (int *)(SDRAM_REG_BASE_ADDR);
    *addr = 1;

    i = 1000;
    while(i--);

    /// issue nop
    addr = (int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x183;

    i = 100;
    while(i--);

    /// issue precharge all
    addr = (int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x103;

    i = 100;
    while(i--);

    /// set refresh cycle
    addr = (int *)(SDRAM_REG_BASE_ADDR + 0x24);
    *addr = 1;

    /// issue nop
    addr = (int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x183;

    i = 100;
    while(i--);

    addr = (int *)(SDRAM_REG_BASE_ADDR + 0x24);
    ///*addr = 0x30;
    *addr = 0x18;   /// for 26M HCLK


    /// issue mode command
    addr = (int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x83;

    /// set mode
    //addr = (int *)(SDRAM_MEM_BASE_ADDR + 0x33000);
    addr = (int *)(SDRAM_MEM_BASE_ADDR + 0x33000);
    temp = *addr;

    temp++;


    /// issue nop
    addr = (int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x183;

    i = 100;
    while(i--);

    /// issue normal
    addr = (int *)(SDRAM_REG_BASE_ADDR + 0x20);
    *addr = 0x3;

    i = 1000;
    while(i--);

    addr = (int *)(SDRAM_REG_BASE_ADDR + 0x100);
    *addr = 0x80680;
    //*addr = 0x0480;

    i = 1000;
    while(i--);

    return 1;
}

#if 0
#include "mem_bridge.h"
#include "dma.h"

VOID boot_ebcConfig(VOID)
{
#define PSRAM_BCR 0x08710f
    UINT32 flush __attribute__((unused));
    UINT16 read __attribute__((unused));
    UINT16* csBase;

    csBase = (UINT16*)hwp_cs0;
    hwp_memBridge->FIFO_Ctrl = 1;

    read=*(csBase+PSRAM_BCR);
    hwp_memFifo->EBC_CRE = 1;
    *(csBase+PSRAM_BCR)=read;
    flush = hwp_memFifo->EBC_CRE;
    hwp_memFifo->EBC_CRE = 0;

    flush = hwp_memFifo->Flush;
    hwp_memBridge->CS_Time_Write = 0x100f000d;
    hwp_memBridge->CS_Config[0].CS_Mode = 0x0c305015;
    flush = hwp_memFifo->Flush;
    hwp_memBridge->FIFO_Ctrl = 0;
    read = *((volatile UINT16 *)csBase);
    read = *((volatile UINT16 *)csBase);
}

VOID boot_memRwTest(VOID)
{
    UINT32 d32 __attribute__((unused));
    UINT16 d16 __attribute__((unused));
    UINT8 d8 __attribute__((unused));
    *(VOLATILE UINT32 *)0xa2000100 = 0x12345678;
    *(VOLATILE UINT32 *)0xa2000104 = 0x90abcdef;
    *(VOLATILE UINT32 *)0xa2000108 = 0x87654321;
    *(VOLATILE UINT32 *)0xa200010c = 0xfedcba09;
    d32 = *(VOLATILE UINT32 *)0xa2000100;
    d32 = *(VOLATILE UINT32 *)0xa2000104;
    d32 = *(VOLATILE UINT32 *)0xa2000108;
    d32 = *(VOLATILE UINT32 *)0xa200010c;

    *(VOLATILE UINT16 *)0xa2000130 = 0x1234;
    *(VOLATILE UINT16 *)0xa2000132 = 0x5678;
    *(VOLATILE UINT16 *)0xa2000134 = 0x90ab;
    *(VOLATILE UINT16 *)0xa2000136 = 0xcdef;
    d16 = *(VOLATILE UINT16 *)0xa2000130;
    d16 = *(VOLATILE UINT16 *)0xa2000132;
    d16 = *(VOLATILE UINT16 *)0xa2000134;
    d16 = *(VOLATILE UINT16 *)0xa2000136;

    *(VOLATILE UINT8 *)0xa2000160 = 0x12;
    *(VOLATILE UINT8 *)0xa2000161 = 0x34;
    *(VOLATILE UINT8 *)0xa2000162 = 0x56;
    *(VOLATILE UINT8 *)0xa2000163 = 0x78;
    d8 = *(VOLATILE UINT8 *)0xa2000160;
    d8 = *(VOLATILE UINT8 *)0xa2000161;
    d8 = *(VOLATILE UINT8 *)0xa2000162;
    d8 = *(VOLATILE UINT8 *)0xa2000163;
}

VOID boot_dmaTest(VOID)
{
    if (!((hwp_dma->get_channel) & DMA_GET_CHANNEL))
    {
        return;
    }
    hwp_dma->src_addr = 0xa1c00300;
    hwp_dma->dst_addr = 0xa2000300;
    hwp_dma->sd_dst_addr = 0;
    hwp_dma->pattern  = 0;
    hwp_dma->xfer_size = 32;

    // Complete with the other fields.
    hwp_dma->control = 0
                       | DMA_ENABLE
                       | DMA_INT_DONE_CLEAR;

    {
        // Flush the CPU write buffer.
        UINT32 unused __attribute__((unused)) = hwp_dma->control;
    }

    while(!(hwp_dma->status & DMA_INT_DONE_STATUS));

    hwp_dma->src_addr = 0xa2000400;
    hwp_dma->dst_addr = 0xa1c00400;
    hwp_dma->sd_dst_addr = 0;
    hwp_dma->pattern  = 0;
    hwp_dma->xfer_size = 32;

    // Complete with the other fields.
    hwp_dma->control = 0
                       | DMA_ENABLE
                       | DMA_INT_DONE_CLEAR;

    {
        // Flush the CPU write buffer.
        UINT32 unused __attribute__((unused)) = hwp_dma->control;
    }

    while(!(hwp_dma->status & DMA_INT_DONE_STATUS));
}

#endif


// =============================================================================
// boot_Sector_Nand
// -----------------------------------------------------------------------------
/// Entry of the Boot Sector in the first flash sector.
///
/// This code is executed right after the Boot ROM and right before the
/// Boot Loader, in normal mode execution. It does the following:
///
/// * Writes the EBC RAM chip select configuration if a valid configuration
/// has been found in the Boot Sector structure.
///
/// * If the command in the Boot Sector structure tells us to enter
/// boot monitor, we do so after sending an event.
///
/// * Else, we enter the Boot Loader, providing it with the #param.
///
/// @param param Generic parameter only used to invoke boot_LoaderEnter(param).
// =============================================================================
PROTECTED VOID BOOT_SECTOR_NAND_SECTION_START boot_Sector_Nand(UINT32 param)
{
    UINT32 pageSize = boot_NandGetPageSize();

    sdram_ini();

    boot_NandReadPages(NAND_FLASH_BOOT_CODE_SIZE,
                       (UINT32 *)&_flash_start,
                       (UINT32)&_flash_end - (UINT32)&_flash_start + (pageSize-1));
#if 0
    boot_NandReadPages((UINT32)(&_user_data_start - &_flash_start),
                       (UINT32 *)&_user_data_start,
                       (UINT32)&_user_data_size);
    boot_NandReadPages((UINT32)(&_extra_section_start - &_flash_start),
                       (UINT32 *)&_extra_section_start,
                       (UINT32)&_extra_section_size);
    boot_NandReadPages((UINT32)(&_calib_start - &_flash_start),
                       (UINT32 *)&_calib_start,
                       (UINT32)CALIB_SIZE );
    boot_NandReadPages((UINT32)(&_factory_start - &_flash_start),
                       (UINT32 *)&_factory_start,
                       (UINT32)FACTORY_SIZE + (pageSize-1));
#endif
    // Flush both caches
    boot_FlushCache();
    // Enter normal boot sector
    boot_Sector(param);
}


