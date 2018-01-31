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

#include "flash_prog_map.h"

#include "tgt_memd_cfg.h"
#include "memd_m.h"
#include "pmd_m.h"
#include "boot_usb_monitor.h"

#include "hal_sys.h"
#include "hal_debug.h"
#include "hal_dma.h"
#include "hal_clk.h"
#include "hal_timers.h"
#include "hal_ebc.h"
#include "hal_map_engine.h"
#include "mem_bridge.h"

#include "hal_dma.h"
#include "sys_ctrl.h"

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

BOOT_HOST_USB_MONITOR_CTX_T g_flashProgUsbContext;

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

static CONST UINT32 pattern[] = { 0x4f35b7da, 0x8e354c91, };
//static CONST UINT32 pattern[] = { 0xa5a5a5a5, 0x5a5a5a5a, 0x4f35b7da, 0x8e354c91, };

UINT32 g_dataBuffer_start,psram_check_size,ret_flag=0;

// Check whether flash programmer data buffer is accessible
PROTECTED BOOL fp_CheckDataBuffer(VOID)
{
    int i, j;
    static VOLATILE UINT32* CONST dataAry[] = {
        (VOLATILE UINT32*)&g_dataBufferA[0],
        (VOLATILE UINT32*)&g_dataBufferA[FPC_BUFFER_SIZE/4 - 1],
        (VOLATILE UINT32*)&g_dataBufferB[0],
        (VOLATILE UINT32*)&g_dataBufferB[FPC_BUFFER_SIZE/4 - 1],
        (VOLATILE UINT32*)&g_dataBufferC[0],
        (VOLATILE UINT32*)&g_dataBufferC[FPC_BUFFER_SIZE/4 - 1],
    };

    for (i=0; i<sizeof(dataAry)/sizeof(dataAry[0]); i++)
    {
        for (j=0; j<sizeof(pattern)/sizeof(pattern[0]); j++)
        {
            *(dataAry[i]) = pattern[j];
            if (*(dataAry[i]) != pattern[j])
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}


BOOL fp_CheckPsramData(UINT32  flag, UINT32 g_dataBuffer_start,UINT32 psram_check_size);


PROTECTED BOOL fp_CheckDataBuffer_HIGH_ADDR(VOID)
{

#ifndef NO_PSRAM_HIGH_ADDR_CHECK
    typedef struct
    {
        UINT32 startAddr;
        UINT32 len;
    } PSRAM_RANGE_T;

    UINT32 psramBase = (UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM);
    PSRAM_RANGE_T range[] = {
        { psramBase+0xc0000, 0x10000, },
        { psramBase+0xd0000, 0x10000, },
        { psramBase+0xe0000, 0x10000, },
        { psramBase+0xf0000, 0x10000, },
    };

    range[0].startAddr=(psram_check_size-0x100000)|range[0].startAddr;
    range[1].startAddr=(psram_check_size-0x100000)|range[1].startAddr;

    range[2].startAddr=(psram_check_size-0x100000)|range[2].startAddr;
    range[3].startAddr=(psram_check_size-0x100000)|range[3].startAddr;




    for (int i=0; i<sizeof(range)/sizeof(range[0]); i++)
    {

        for (int j=0; j<sizeof(pattern)/sizeof(pattern[0]); j++)
        {
            if(!fp_CheckPsramData(pattern[j],range[i].startAddr,range[i].startAddr+range[i].len))
                return FALSE;
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
    return TRUE;
}








PROTECTED BOOL fp_CheckPsramData_noDMA(UINT32  flag, UINT32 dataBuffer_start,UINT32 dataBuffer_end)
{
    for (int i=dataBuffer_start; i<dataBuffer_end; i=i+4)
    {
        *(volatile UINT32 *)((UINT32)i) = flag;
    }

    for (int i=dataBuffer_start; i<dataBuffer_end; i=i+4)
    {
        if (*(volatile UINT32 *)((UINT32)i) != flag)
        {
            SEND_EVENT(0x88500000|(flag&0xff));
            SEND_EVENT((UINT32)i);
            SEND_EVENT(flag);
            ret_flag =1;
            *(volatile UINT32 *)(0x81c00020)=flag;
            *(volatile UINT32 *)(0x81c00024)=i;
            *(volatile UINT32 *)(0x81c00028)=*(UINT32 *)((UINT32)i);
            *(volatile UINT32 *)(0x81c00030)=dataBuffer_start;
            *(volatile UINT32 *)(0x81c00034)=dataBuffer_end;
            return FALSE;
        }
    }
    return TRUE;
}


PROTECTED BOOL fp_CheckPsramData(UINT32  flag, UINT32 dataBuffer_start,UINT32 dataBuffer_end)
{

    /*
            for (int i=g_dataBuffer_start; i<psram_check_size; i=i+4)
            {
                *(volatile UINT32 *)((UINT32)i) = flag;
            }
    */

    int i;

    *(volatile UINT32 *)(0x81c0002c)=flag;

    for (i=dataBuffer_start; i<dataBuffer_end; i=i+4)
    {

//      while(!hal_DmaDone());

        if(!(hal_DmaDone()))
        {
            *(volatile UINT32 *)((UINT32)i) = flag;
        }
        else
        {
            // Clear destination buffer
            ram_dmaCfg.dstAddr=(UINT8*)i;
            ram_dmaCfg.pattern=flag;
            ram_dmaCfg.transferSize=(dataBuffer_end-i)>=0x40000?0x40000:(dataBuffer_end-i);
            ram_dmaCfg.mode = HAL_DMA_MODE_PATTERN;
            ram_dmaCfg.userHandler=NULL;

            while(hal_DmaStart(&ram_dmaCfg)!=HAL_ERR_NO)
            {
            }
            i=i+(((dataBuffer_end-i)>=0x40000)?0x40000:(dataBuffer_end-i))-4;

            // Wait until cleanup finished
            //				while(!hal_DmaDone())
            //				{
            //				}
        }
    }

    while(!hal_DmaDone())
    {
    }

    for (i=dataBuffer_start; i<dataBuffer_end; i=i+4)
    {
        if (*(volatile UINT32 *)((UINT32)i) != flag)
        {
            SEND_EVENT(0x88500000|(flag&0xff));
            SEND_EVENT((UINT32)i);
            SEND_EVENT(flag);
            ret_flag =1;
            *(volatile UINT32 *)(0x81c00020)=flag;
            *(volatile UINT32 *)(0x81c00024)=i;
            *(volatile UINT32 *)(0x81c00028)=*(UINT32 *)((UINT32)i);
            *(volatile UINT32 *)(0x81c00030)=dataBuffer_start;
            *(volatile UINT32 *)(0x81c00034)=dataBuffer_end;
            return FALSE;
        }
    }
    return TRUE;
}

PROTECTED VOID fp_CheckPsram(VOID)
{
    UINT32 elapsedTime=0,fpsStartMeasTime = hal_TimGetUpTime();

    *(volatile UINT32 *)(0x81c00000)=0x6500b203;


    fp_CheckPsramData(0x00000000,g_dataBuffer_start,psram_check_size);
    fp_CheckPsramData_noDMA(0xFFFFFFFF,g_dataBuffer_start,psram_check_size);

    elapsedTime = (hal_TimGetUpTime()-fpsStartMeasTime);

    if(ret_flag == 1)
        *(volatile UINT32 *)(0x81c00000)=0x22222222;
    else
        *(volatile UINT32 *)(0x81c00000)=0xAAAAAAA8;
    *(volatile UINT32 *)(0x81c00008)=elapsedTime;
    *(volatile UINT32 *)(0x81c00004)=hal_TimGetUpTime();
    return ;
}


#define ERAM_CHECK_PATTERN0  0xa569f0f0
#define ERAM_CHECK_PATTERN1  0x5a960f0f
#define ERAM_ADDR_NUM 6
UINT32 eram_end_addr2[ERAM_ADDR_NUM] = {0x200000, 0x400000, 0x800000, 0x1000000, 0x2000000, 0x4000000};
#define ERAM_CHECK_ERROR_CODE 0xdead0000
#define CRAM_DIDR 0x40000

UINT32 ERamSize_Valid_Check(REG16* csBase)
{
    // Using the CR controlled method
    // Configure DIDR
    // CRAM_DIDR[15] : ROW length
    // CRAM_DIDR[14:11] : Device Version
    // CRAM_DIDR[10:8] : Device density
    // CRAM_DIDR[7:5] : CRAM Generation
    // CRAM_DIDR[4:0] : Vendor ID
    UINT16 index=0;
    REG32* pCRE = &(hwp_memFifo->EBC_CRE);
    UINT32 is_aptic=0,aptic_size=0;

    *pCRE = 1; // MEM_FIFO_CRE
    index = *(csBase + CRAM_DIDR);
    UINT32 flush __attribute__((unused)) = *pCRE;
    *pCRE = 0;

    if((index & 0x1f) == 0xd)//aptic
    {
        index = (index >> 8) & 7;
        is_aptic=1;
        aptic_size=eram_end_addr2[index];
        hal_HstSendEvent(aptic_size);
    }

    {
        volatile UINT32 *ERam_Base = (UINT32 *)csBase;
        *ERam_Base = ERAM_CHECK_PATTERN0;

        for(index = 0; index < ERAM_ADDR_NUM; index++)
        {
            if(*(volatile UINT32 *)((UINT32)ERam_Base + eram_end_addr2[index]) == ERAM_CHECK_PATTERN0)
            {
                *ERam_Base = ERAM_CHECK_PATTERN1;
                if(*(volatile UINT32 *)((UINT32)ERam_Base + eram_end_addr2[index]) == ERAM_CHECK_PATTERN1)
                {
                    if(is_aptic)
                    {
                        if(aptic_size==eram_end_addr2[index])
                            return eram_end_addr2[index];
                        else
                        {
                            *(volatile UINT32 *)(0x81c00000)=0x11111111;
                            while(1);
                        }
                    }
                    else
                        return eram_end_addr2[index];
                }
            }
        }
    }
    return 0;
}

INT32								 status = MEMD_ERR_NO;
UINT32 __attribute__((section(".sramucdata"))) s,notmagicNumber=0x2610ffba,write_flsh_addr=0x00000008,readmagicNumber;




VOID __attribute__((section (".sramtext"))) fp_Check_SPI_FLASH(VOID)
{

    extern UINT32 get_flash_ID(UINT32 flash_addr);
    extern	MEMD_SPIFLSH_RDID_T g_spiflash_ID;


    /*
    	    spi_flash_ini(FALSE,0x2,2,0x1);

    		g_spiflash_ID.manufacturerID=0;
    		g_spiflash_ID.device_memory_type_ID=0;
    		g_spiflash_ID.device_capacity_type_ID=0;

    	    spi_flash_ini_0(TRUE,0x1,2,0x0);
    */

//		get_flash_ID(0);



    *(volatile UINT32 *)(0xa1b00000)=0x6500b203;
    *(volatile UINT32 *)(0xa1b00004)=g_spiflash_ID.manufacturerID;
    *(volatile UINT32 *)(0xa1b00008)=g_spiflash_ID.device_memory_type_ID;
    *(volatile UINT32 *)(0xa1b0000c)=g_spiflash_ID.device_capacity_type_ID;


    if(g_spiflash_ID.manufacturerID&&(g_spiflash_ID.manufacturerID!=0xff))
    {

        //				memd_FlashRead((CHAR*)write_flsh_addr,
        //												   sizeof(UINT32),&s,
        //												   (CHAR*)&readmagicNumber);

        readmagicNumber=*(UINT32 *)(0xa8000000 | write_flsh_addr);


        if(readmagicNumber!=0xffffffff)
        {
            write_flsh_addr=write_flsh_addr+4;
            for(; (write_flsh_addr<0x200000);)
            {
                //						memd_FlashRead((CHAR*)write_flsh_addr,
                //														   sizeof(UINT32),&s,
                //														   (CHAR*)&readmagicNumber);

                readmagicNumber=*(UINT32 *)(0xa8000000 | write_flsh_addr);

                //									hal_HstSendEvent(write_flsh_addr);
                //									hal_HstSendEvent(readmagicNumber);

                if(!((write_flsh_addr<0x200000)&&((readmagicNumber==0)||readmagicNumber==0x2610ffba)))
                    break;

                write_flsh_addr=write_flsh_addr+4;


            }
        }

//								hal_HstSendEvent(write_flsh_addr);
//								hal_HstSendEvent(readmagicNumber);


        if(write_flsh_addr>=0x200000)
        {
            *(volatile UINT32 *)(0xa1b00000)=0x12000000|g_spiflash_ID.manufacturerID;
            while(1);
        }
        else if(write_flsh_addr!=0x00000008)
        {
            notmagicNumber=0;
        }


        status = memd_FlashWrite((CHAR*)write_flsh_addr,
                                 sizeof(UINT32), &s,
                                 (CHAR*)&notmagicNumber);

        if (status != MEMD_ERR_NO)
        {
            *(volatile UINT32 *)(0xa1b00000)=0x34000000|g_spiflash_ID.manufacturerID;
        }
        else
        {

            //						memd_FlashRead((CHAR*)write_flsh_addr,
            //														   sizeof(UINT32),&s,
            //														   (CHAR*)&readmagicNumber);



            readmagicNumber=*(UINT32 *)(0xa8000000 | write_flsh_addr);

            if(readmagicNumber==notmagicNumber)
                *(volatile UINT32 *)(0xa1b00000)=0xaaaaaaaa;
            else
                *(volatile UINT32 *)(0xa1b00000)=0x56000000|g_spiflash_ID.manufacturerID;
        }
    }
    else
        *(volatile UINT32 *)(0xa1b00000)=g_spiflash_ID.manufacturerID;



    *(volatile UINT32 *)(0xa1b00024)=hal_TimGetUpTime();
    *(volatile UINT32 *)(0xa1b00010)=notmagicNumber;
    *(volatile UINT32 *)(0xa1b00014)=readmagicNumber;

//	    spi_flash_ini(TRUE,0x2,2,0x1);

}
// This xcpu_main replaces the regular xcpu_main present in the platform library

extern PUBLIC void voc_test();



// =============================================================================
// hal_SysStartBcpu
// -----------------------------------------------------------------------------
/// Start the BCPU with a stack to at the given address.
/// @param bcpuMain Main function to run on the BCPU.
/// @param stackStartAddr Stack top for the program running on the BCPU
// =============================================================================
PUBLIC VOID hal_SysStartBcp(VOID* bcpuMain, VOID* stackStartAddr)
{

    bcpu_stack_base = (UINT32) stackStartAddr;
    bcpu_main_entry = (UINT32*) bcpuMain;

    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Clk_BB_Enable = SYS_CTRL_ENABLE_BB_BCPU;
    hwp_sysCtrl->BB_Rst_Clr = SYS_CTRL_CLR_RST_BCPU;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
}

#define BCPU_MAIN_LOCATION __attribute__((section(".bcpu_main_entry")))
#define CACHED(addr)                ((addr)|0x80000000)

VOID bcpu_main2(VOID);

PROTECTED int main(VOID)
{
    VOLATILE FPC_COMMAND_DATA_T*         cmds =
        (FPC_COMMAND_DATA_T*) HAL_SYS_GET_UNCACHED_ADDR((UINT32)g_flashProgDescr.commandDescr);
    UINT32 bcpuStackBase;


    hal_SwRequestClk(FOURCC_TOOL, HAL_CLK_RATE_250M);

    *(volatile UINT32 *)(0x81c00000)=0x12345678;

    cmds[0].cmd = FPC_NONE;
    cmds[1].cmd = FPC_NONE;

    *(volatile UINT32 *)(0x81c0000c)=hal_TimGetUpTime();

//  enable exception (gdb breaks) but not external it, keep handler in rom
    asm volatile("li    $27,    (0x00400000 | 0x0001 | 0xc000)\n\t"
                 "mtc0  $0, $13\n\t"
                 "nop\n\t"
                 "mtc0  $27, $12\n\t"
//  k1 is status for os irq scheme
                 "li    $27, 1");

//if(*(UINT32 *)(0x81c00064)!=0x12345678)
//    memd_FlashOpen(tgt_GetMemdFlashConfig());

//    memd_RamOpen(tgt_GetMemdRamConfig());

    if (!fp_CheckDataBuffer())
    {
        *(volatile UINT32 *)(0x81c00000)=0x12344444;
        SEND_EVENT(EVENT_FLASH_PROG_MEM_RESET);
        hal_SysResetOut(TRUE);
        pmd_EnableMemoryPower(FALSE);
        hal_TimDelay(2 * HAL_TICK1S);
        pmd_EnableMemoryPower(TRUE);
        hal_TimDelay(10);
        hal_SysResetOut(FALSE);
        hal_TimDelay(10);

//       memd_FlashOpen(tgt_GetMemdFlashConfig());
        memd_RamOpen(tgt_GetMemdRamConfig());

        if (!fp_CheckDataBuffer())
        {
            SEND_EVENT(EVENT_FLASH_PROG_MEM_ERROR);
            *(volatile UINT32 *)(0x81c00000)=0x12355555;
            while(1);
        }
    }

    // Record the communication structure into HAL Mapping Engine
    hal_MapEngineRegisterModule(HAL_MAP_ID_FLASH_PROG, &g_flashProgVersion, &g_flashProgDescr);


    *(volatile UINT32 *)(0x81c00000)=0x11223344;

    g_dataBuffer_start=(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM);

    psram_check_size =(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM) | ERamSize_Valid_Check((UINT16*)hal_EbcGetCsAddress(HAL_EBC_SRAM));


    *(volatile UINT32 *)(0x81c00014)=psram_check_size;
    *(volatile UINT32 *)(0x81c0001c)=g_dataBuffer_start;


    for(int i =(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM) ,j=15; i<psram_check_size; i=(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM) |(1<<j),j++)
    {
        *(volatile UINT32 *)i=0xaaaa5555;
    }

    for(int i =(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM) ,j=15; i<psram_check_size; i=(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM) |(1<<j),j++)
    {
        *(volatile UINT32 *)i=i;
        for(int k =i,m=15; k<psram_check_size; k=(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM) |(1<<m),m++)
        {
            if((*(volatile UINT32 *)k==*(volatile UINT32 *)i)&(k!=i))
            {
                hal_HstSendEvent(k);
                hal_HstSendEvent(i);
                *(volatile UINT32 *)(0x81c00000)=0x11223345;
                while(1);
            }
        }
        *(volatile UINT32 *)i=0x5555aaaa;
    }


    if(psram_check_size <= g_dataBuffer_start)
    {
        *(volatile UINT32 *)(0x81c00000)=0x11112222;
        SEND_EVENT(psram_check_size);
        SEND_EVENT(g_dataBuffer_start);
        SEND_EVENT(0x06210222);
        while(1);
    }

    g_dataBuffer_start=(g_dataBuffer_start&(~0xa0000000))|0x80000000;


    if(psram_check_size>((UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM)|0x00400000))
    {
        psram_check_size=(psram_check_size&(~0xa0000000))|0x80000000;

        if(!fp_CheckDataBuffer_HIGH_ADDR())
        {
            *(volatile UINT32 *)(0x81c00000)=0x12366666;
            while(1);
        }
        psram_check_size=(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM)|0x00400000;
    }

    psram_check_size=(psram_check_size&(~0xa0000000))|0x80000000;

    if(*(volatile UINT32 *)(0x81c00060)==0x12345678)
    {
        if((*(volatile UINT32 *)0x81c00068 & 0xff00ffff) == 0)
            psram_check_size=(*(volatile UINT32 *)(0x81c00068)&(~0xa0000000))|0x80000000;
    }


    *(volatile UINT32 *)(0x81c00018)=psram_check_size;
    *(volatile UINT32 *)(0x81c00000)=0x55667788;
    *(volatile UINT32 *)(0x81c00010)=hal_TimGetUpTime();

    fp_CheckPsram();

    hal_SwRequestClk(FOURCC_TOOL, HAL_CLK_RATE_104M);
    hal_EbcCsSetup(HAL_SYS_MEM_FREQ_78M);


    *(volatile UINT32 *)(0xa1b00020)=hal_TimGetUpTime();

    fp_Check_SPI_FLASH();


    if(*(volatile UINT32 *)(0x81c00000)==0xAAAAAAA8)
    {
        voc_test();
    }

    if(*(volatile UINT32 *)(0x81c00000)==0xAAAAAAA9)
    {
        // BCPU stack start at the end of SRAM
        bcpuStackBase = CACHED (((UINT32)0x81983260 - 4));

        // Handshake process to be sure BCPU is well started
        hal_SysStartBcp((VOID*)bcpu_main2, (VOID*)bcpuStackBase);
    }
    else
        while (1);
    hal_HstSendEvent(*(volatile unsigned int *)0xa1a02014);

    while (1);
}

