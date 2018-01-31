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

#include "flash_prog_map.h"

#include "tgt_memd_cfg.h"
#include "memd_m.h"
#include "pmd_m.h"
#include "boot_usb_monitor.h"

#include "hal_sys.h"
#include "hal_clk.h"
#include "hal_debug.h"
#include "hal_dma.h"
#include "hal_sys.h"
#include "hal_timers.h"
#include "hal_ebc.h"
#include "hal_map_engine.h"

#include "hal_dma.h"

#include "global_macros.h"

#include "hal_mem_map.h"


#include "bb_ifc.h"
#include "i2c_master.h"
#include "keypad.h"
#include "rf_spi.h"
#include "sys_ctrl.h"
#include "sys_ifc.h"
#include "voc_ahb.h"
#include "voc_cfg.h"
#include "voc_ram.h"
#include "sci.h"
#include "sdmmc.h"
#include "comregs.h"
#include "xcor.h"



HAL_DMA_CFG_T ram_dmaCfg;


// define the folowing to see the error code returned by the flash driver as host events
#define DEBUG_EVENTS
#ifdef DEBUG_EVENTS
#include "hal_host.h"
#endif

extern VOID mon_Event(UINT32 ch);
extern VOID boot_HostUsbEvent(UINT32 ch);


#if (CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
//  #define SEND_EVENT boot_HostUsbEvent
#else
//  #define SEND_EVENT mon_Event
#endif

//#define SEND_EVENT hal_HstSendEvent

#define SEND_EVENT(event)


#ifndef FLASH_USB_SEND_EVENT_DELAY
#define FLASH_USB_SEND_EVENT_DELAY    8192 // 0.5s
#endif /* FLASH_USB_SEND_EVENT_DELAY */

#define FP_DATA_BUFFER_LOCATION __attribute__((section(".ramucbss")))

//BOOT_HOST_USB_MONITOR_CTX_T g_flashProgUsbContext;

//-----------------------------
// Three Data Buffers are available for CoolWatcher
//-----------------------------
#if (FPC_BUFFER_SIZE/4*4 != FPC_BUFFER_SIZE)
#error "FPC_BUFFER_SIZE should be aligned with word boundary"
#endif

PROTECTED UINT32 FP_DATA_BUFFER_LOCATION g_dataBufferA[FPC_BUFFER_SIZE/4];
PROTECTED UINT32 FP_DATA_BUFFER_LOCATION g_dataBufferB[FPC_BUFFER_SIZE/4];
PROTECTED UINT32 FP_DATA_BUFFER_LOCATION g_dataBufferC[FPC_BUFFER_SIZE/4];

EXPORT PUBLIC UINT32 _boot_loader_magic_number_in_flash;

PROTECTED HAL_MAP_VERSION_T g_flashProgVersion = {0, 0x20080121, 0, "Flash Programmer Version 1"};
PUBLIC FLASH_PROG_MAP_ACCESS_T g_flashProgDescr =
{   {FPC_PROTOCOL_MAJOR,FPC_PROTOCOL_MINOR},{{FPC_NONE, (UINT32)0, 0, 0} ,{FPC_NONE, (UINT32)0, 0, 0}},
    (UINT8*)g_dataBufferA, (UINT8*)g_dataBufferB, (UINT8*)g_dataBufferC, FPC_BUFFER_SIZE
};

#if !(CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
#error "error: You can t use FASTPF_USE_USB without CHIP_HAS_USB"
#endif

// =============================================================================
// FLASH_PROGRAMMER_SPECIAL_SECTOR_T
// -----------------------------------------------------------------------------
/// Describe the special sector (use a magic number the first word)
// =============================================================================
typedef struct
{
    UINT8* address;
    UINT32 magicNumber;
} FLASH_PROGRAMMER_SPECIAL_SECTOR_T;

//static CONST UINT32 pattern[] = { 0x4f35b7da, 0x8e354c91, };
static CONST UINT32 pattern[] = { 0xa5a5a5a5, 0x5a5a5a5a, 0x4f35b7da, 0x8e354c91, };

UINT32 g_dataBuffer_start=0, psram_check_size=0, ret_flag=0;




PUBLIC BOOL hal_HstSendEvent2(UINT32 ch)
//#endif
{
    UINT32 startTime = *(UINT32 *)0xa1a02014;
    BOOL   timeOut   = FALSE;

    //  wait for host sema to be free
    //  or time out.
    while ((*(UINT32 *)0xa1a3f008 & 1) == 0 && !timeOut)
    {
        if ((*(UINT32 *)0xa1a02014 - startTime) >= 5)
        {
            timeOut = TRUE;
        }
    }

    //  Write the byte in the host event register if possible
    if (!timeOut)
    {
        *(UINT32 *)0xa1a3f008 = ch;
    }

    return timeOut;
}


BOOL fp_CheckPsramData(UINT32  flag, UINT32 g_dataBuffer_start,UINT32 psram_check_size);


PROTECTED BOOL fp_CheckDataBuffer_RAM_BIST(VOID)
{

#ifndef NO_PSRAM_HIGH_ADDR_CHECK
    typedef struct
    {
        UINT32 startAddr;
        UINT32 len;
    } PSRAM_RANGE_T;

    UINT32 psramBase = (UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM);
    PSRAM_RANGE_T range[] = {
//			{ psramBase+0xc0000, 0x10000, },
//			{ psramBase+0xd0000, 0x10000, },
//			{ psramBase+0xe0000, 0x10000, },
//			{ psramBase+0xf0000, 0x10000, },
//			{ 0xa1960000, 0x5000, }
        { 0xa1b00000, 0x800, },
        { 0xa1980000, 0x4000, },

        { 0x81940000, 0x5000, },
        { 0xa1948000, 0x5000, },
        { 0xa1960000, 0x5000, }
    };

//		range[0].startAddr=(psram_check_size-0x100000)|range[0].startAddr;
//		range[1].startAddr=(psram_check_size-0x100000)|range[1].startAddr;
//		range[2].startAddr=(psram_check_size-0x100000)|range[2].startAddr;
//		range[3].startAddr=(psram_check_size-0x100000)|range[3].startAddr;

    UINT32 psramAddr;
    for (int i=0; i<sizeof(range)/sizeof(range[0]); i++)
    {

        for (int j=0; j<sizeof(pattern)/sizeof(pattern[0]); j++)
        {
            if(!fp_CheckPsramData(pattern[j],range[i].startAddr,range[i].startAddr+range[i].len))
            {
//						*(UINT32 *)(0x81c00000)=1<<i;
//						*(UINT32 *)(0x81c00000)=(*(UINT32 *)(0x81c00000))|(1<<i);
//					return FALSE;
            }
        }

        /*
        			for (psramAddr=range[i].startAddr;
        				 psramAddr+4<=range[i].startAddr+range[i].len;
        				 psramAddr+=4)
        			{
        				for (int j=0; j<sizeof(pattern)/sizeof(pattern[0]); j++)
        				{
        					*(VOLATILE UINT32*)psramAddr = pattern[j];
        					if (*(VOLATILE UINT32*)psramAddr != pattern[j])
        					{
        						SEND_EVENT(psramAddr);
        						return FALSE;
        					}
        				}
        			}
        */

    }
#endif
    if(ret_flag ==1)
        return FALSE;
    else
        return TRUE;
}



PROTECTED BOOL fp_CheckSYSRAMData(UINT32  flag, UINT32 dataBuffer_start,UINT32 u_psram_check_size)
{
    unsigned i=dataBuffer_start;
    for (i=dataBuffer_start; i<u_psram_check_size; i=i+4)
    {
        *(volatile UINT32 *)((UINT32)i) = flag;
    }

    for (i=dataBuffer_start; i<u_psram_check_size; i=i+4)
    {
        if(*(volatile UINT32 *)i != flag)
        {
            *(volatile UINT32 *)0x81c00000 = i;
            return FALSE;
        }
    }
    return TRUE;
}


PROTECTED BOOL fp_CheckDataBuffer_SYS_RAM_BIST(VOID)
{

    if(!fp_CheckSYSRAMData(0x5a697887,0x81c00000,0x81c00000+0x18000))
    {
        return 0;
    }

    if(!fp_CheckSYSRAMData(0xa5968778,0x81c00000,0x81c00000+0x18000))
    {
        return 0;
    }

    return 1;
}



PROTECTED BOOL fp_CheckDataBuffer_USB_RAM_BIST(VOID)
{

    if(!fp_CheckSYSRAMData(0x5a697887,0x81aa0000,0x81aa0420))
    {
        *(UINT32 *)(0x81c00000)=(*(UINT32 *)(0x81c00000))|(1<<(5));
//			while(1);
    }

    if(!fp_CheckSYSRAMData(0xa5968778,0x81aa0000,0x81aa0420))
    {
//			while(1);
    }
}




PROTECTED BOOL fp_CheckSYSRAMHalfWord(UINT32  flag, UINT32 dataBuffer_start,UINT32 psram_check_size)
{


    for (int i= dataBuffer_start; i<psram_check_size; i=i+2)
    {
        *(UINT16 *)((UINT32)i) = flag;
    }

    for (int i= dataBuffer_start; i<psram_check_size; i=i+2)
    {
        if (*(UINT16 *)((UINT32)i) != flag)
        {

            SEND_EVENT((UINT32)flag);
            SEND_EVENT((UINT32)i);
            SEND_EVENT(*(UINT32 *)((UINT32)i));


            *(UINT32 *)(0x81c00000)=i;
//            return FALSE;
        }
    }

    return TRUE;
}



/*  gouda ram is 1344 half word  */
PROTECTED BOOL fp_CheckDataBuffer_GOUDA_RAM_BIST(VOID)
{

//		if(!fp_CheckSYSRAMHalfWord(0x5555,0x81ae0000,0x81ae0000+2688))
    if(!fp_CheckSYSRAMHalfWord(0x5555,0x81ae0000,0x81ae0000+1344))
    {
        *(UINT32 *)(0x81c00000)=(*(UINT32 *)(0x81c00000))|(1<<(5));
//			while(1);
    }

    if(!fp_CheckSYSRAMHalfWord(0xaaaa,0x81ae0000,0x81ae0000+1344))
    {
//			while(1);
    }
}


extern UINT32 _binary_mem_bridge_rom_CHIP_bin_start;
extern UINT32 _binary_bcpu_rom_CHIP_bin_start;


PROTECTED BOOL fp_CheckPsramData(UINT32  flag, UINT32 g_dataBuffer_start,UINT32 psram_check_size)
{

    /*
            for (int i=g_dataBuffer_start; i<psram_check_size; i=i+4)
            {
                *(UINT32 *)((UINT32)i) = flag;
            }
    */

    int i;


    for (i=g_dataBuffer_start; i<psram_check_size; i=i+4)
    {

//      while(!hal_DmaDone());

        if(!(hal_DmaDone()))
        {
            *(UINT32 *)((UINT32)i) = flag;
        }
        else

        {
            // Clear destination buffer
            ram_dmaCfg.dstAddr=i;
            ram_dmaCfg.pattern=flag;
            ram_dmaCfg.transferSize=(psram_check_size-i)>=0x40000?0x40000:(psram_check_size-i);
            ram_dmaCfg.mode = HAL_DMA_MODE_PATTERN;
            ram_dmaCfg.userHandler=NULL;

            while(hal_DmaStart(&ram_dmaCfg)!=HAL_ERR_NO)
            {
            }
            i=i+(((psram_check_size-i)>=0x40000)?0x40000:(psram_check_size-i))-4;

            // Wait until cleanup finished
            //				while(!hal_DmaDone())
            //				{
            //				}
        }
    }

    while(!hal_DmaDone())
    {
    }


    for (int i=g_dataBuffer_start; i<psram_check_size; i=i+4)
    {
        if (*(UINT32 *)((UINT32)i) != flag)
        {

//            SEND_EVENT(0x88500000| (flag&0xff)  );
            SEND_EVENT(flag);
            SEND_EVENT((UINT32)i);
            SEND_EVENT(*(UINT32 *)((UINT32)i));
            ret_flag =1;

            *(UINT32 *)(0x80000000)=1;

            *(UINT32 *)(0x81c00000)=i;


            *(UINT32 *)(0x81c00020)=flag;
            *(UINT32 *)(0x81c00024)=i;
            *(UINT32 *)(0x81c00028)=*(UINT32 *)((UINT32)i);
            *(UINT32 *)(0x81c00030)=g_dataBuffer_start;
            *(UINT32 *)(0x81c00034)=psram_check_size;
//			while(1);
//            return FALSE;
        }
    }
    return TRUE;
}





PROTECTED BOOL fp_CheckROM(UINT32 rom_start , UINT32 rom_norm,UINT32 rom_size)
{
    int j=rom_norm;

    for (int i=rom_start; i<(rom_start+rom_size); i=i+4,j=j+4)
    {
        if (*(UINT32 *)((UINT32)i) != *(UINT32 *)((UINT32)j))
        {

            ret_flag =1;
            *(UINT32 *)(0x80000000)=1;
            SEND_EVENT(0x88888888);
            SEND_EVENT((UINT32)i);
            SEND_EVENT(*(UINT32 *)((UINT32)i));

            SEND_EVENT((UINT32)j);
            SEND_EVENT(*(UINT32 *)((UINT32)j));


            *(UINT32 *)(0x81c00020)=i;
            *(UINT32 *)(0x81c00024)=j;
            *(UINT32 *)(0x81c00028)=*(UINT32 *)((UINT32)i);
            *(UINT32 *)(0x81c00030)=*(UINT32 *)((UINT32)i);
            *(UINT32 *)(0x81c00034)=*(UINT32 *)((UINT32)j);
            *(UINT32 *)(0x81c00000)=i;

//			while(1);
//            return FALSE;
        }
    }
    return TRUE;
}



PROTECTED VOID fp_CheckPsram(VOID)
{

    UINT32 elapsedTime=0,fpsStartMeasTime = hal_TimGetUpTime();

    *(UINT32 *)(0x81c00000)=0x6500b203;


    fp_CheckPsramData(0x55555555,g_dataBuffer_start,psram_check_size);
    fp_CheckPsramData(0xAAAAAAAA,g_dataBuffer_start,psram_check_size);

    elapsedTime = (hal_TimGetUpTime()-fpsStartMeasTime);

    if(ret_flag ==1)
        *(UINT32 *)(0x81c00000)=0x22222222;
    else
        *(UINT32 *)(0x81c00000)=0xAAAAAAAA;
    *(UINT32 *)(0x81c00008)=elapsedTime;
    *(UINT32 *)(0x81c00004)=hal_TimGetUpTime();
    return ;
}


#define ERAM_CHECK_PATTERN0  0xa569f0f0
#define ERAM_CHECK_PATTERN1  0x5a960f0f
#define ERAM_ADDR_NUM 6
UINT32 eram_end_addr2[ERAM_ADDR_NUM] = {0x200000, 0x400000, 0x800000, 0x1000000, 0x2000000, 0x4000000};
#define ERAM_CHECK_ERROR_CODE 0xdead0000
#define CRAM_DIDR 0x40000


INT32								 status = MEMD_ERR_NO;
UINT32 __attribute__((section(".sramucdata"))) s,notmagicNumber=0x2610ffba,write_flsh_addr=0x00000008,readmagicNumber;


// This xcpu_main replaces the regular xcpu_main present in the platform library
PROTECTED int main(VOID)
{

    hal_HstSendEvent(0x11111111);


    *(UINT32 *)(0x81c00000)=0x12345678;


    *(UINT32 *)(0x81c0000c)=hal_TimGetUpTime();

//  enable exception (gdb breaks) but not external it, keep handler in rom
    asm volatile("li    $27,    (0x00400000 | 0x0001 | 0xc000)\n\t"
                 "mtc0  $0, $13\n\t"
                 "nop\n\t"
                 "mtc0  $27, $12\n\t"
//  k1 is status for os irq scheme
                 "li    $29, (0x82080000)\n\t"
                 "li    $27, 1"
                );

//if(*(UINT32 *)(0x81c00064)==0x12345678)
//    memd_FlashOpen(tgt_GetMemdFlashConfig());



    *(UINT32 *)(0x81c00000)=0x11223344;

    //	g_dataBuffer_start=(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM);

    //	g_dataBuffer_start=(g_dataBuffer_start&(~0xa0000000))|0x80000000;
//

//*(UINT32 *)(0x81c00000)=0;

//		{
//			psram_check_size=(psram_check_size&(~0xa0000000))|0x80000000;
//
    /*
    check:
    			  { 0xa1b00000, 0x800, },
    			  { 0xa1980000, 0x4000, },

    			  { 0x81940000, 0x5000, },
    			  { 0xa1948000, 0x5000, },
    			  { 0xa1960000, 0x5000, }
    */
    //   if(!fp_CheckDataBuffer_RAM_BIST())
//	          	{
//			  	while(1);
//	          	}
    //
    //	  psram_check_size=(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM)|0x00400000;
//		}

    //	psram_check_size=(psram_check_size&(~0xa0000000))|0x80000000;




//		if(!fp_CheckROM(0x81e00000,&_binary_mem_bridge_rom_CHIP_bin_start,0x5000))
//		  {
//			hal_HstSendEvent(0x5555);
//		  while(1);
//		  }


    /*

    					  if(!fp_CheckDataBuffer_USB_RAM_BIST())
    						{
    						  hal_HstSendEvent(0xbbbb);
    		//				while(1);
    						}

    					  if(!fp_CheckDataBuffer_GOUDA_RAM_BIST())
    						{
    						  hal_HstSendEvent(0xaaaa);
    		//				while(1);


    						}



    	          if(!fp_CheckROM(0x81e80000,&_binary_bcpu_rom_CHIP_bin_start,0x18000))
    	          	{
    				  hal_HstSendEvent2(0x6666);
    //			  	while(1);
    	          	}

    */
//    *(UINT32 *)(0x80000000)=0x45612399;
    //            *(UINT32 *)(0x81c00000)=0x45612388;


    if(!fp_CheckDataBuffer_SYS_RAM_BIST())
    {
        *(UINT32 *)(0x81c00000)=0x22222222;
    }
    else
    {
        *(UINT32 *)(0x81c00000)=0xAAAAAAA8;
    }



//	if(*(UINT32 *)(0x80000000) ==0)
//		*(UINT32 *)(0x81c00000)=0xAAAAAAAA;


//		start_Bcpu();


//		fp_CheckPsram();

    hal_SwRequestClk(FOURCC_TOOL, HAL_CLK_RATE_104M);
    hal_EbcCsSetup(HAL_SYS_MEM_FREQ_78M);


    *(UINT32 *)(0xa1b00020)=hal_TimGetUpTime();

//	fp_Check_SPI_FLASH();

    hal_HstSendEvent(0x9999);

    while (1);
}



// Chip specific
#define UNCACHED(addr)              ((addr)|0xa0000000)
#define CACHED(addr)                ((addr)|0x80000000)

typedef volatile UINT32* VOLPTR;


PROTECTED VOID bcpu_Main(VOID)
{
    VOLPTR ptr;
    volatile UINT32 value __attribute__((unused));


    *(UINT32 *)(0x80000000)=0;


    if(!fp_CheckROM(0x81e80000,&_binary_bcpu_rom_CHIP_bin_start,0x18000))
    {
        hal_HstSendEvent2(0x6666);
//			  	while(1);
    }



    if(!fp_CheckDataBuffer_SYS_RAM_BIST())
    {
        hal_HstSendEvent2(0x7777);
//			  	while(1);
    }


    if(*(UINT32 *)(0x80000000) ==0)
        *(UINT32 *)(0x81c00000)=0xAAAAAAAA;



    while(1);
}

extern UINT32 _bb_sram_romed_globals_start;
EXPORT UINT32 bcpu_stack_base;
EXPORT UINT32* bcpu_main_entry;

PROTECTED VOID start_Bcpu(VOID)
{
    UINT32 bcpuStackBase;
    // BCPU stack start at the end of SRAM
    bcpuStackBase = CACHED(((UINT32)&_bb_sram_romed_globals_start - 4));

    bcpu_stack_base = (UINT32)bcpuStackBase;
    bcpu_main_entry = (UINT32*)&bcpu_Main;

    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Clk_BB_Enable = SYS_CTRL_ENABLE_BB_BCPU;
    hwp_sysCtrl->BB_Rst_Clr = SYS_CTRL_CLR_RST_BCPU;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
}




