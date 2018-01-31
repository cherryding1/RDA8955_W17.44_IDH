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




#include "ate.h"
#define HAL_UNCACHED_DATA __attribute__((section(".ucdata")))

//#define PSRAM_NEW_TEST_EN

// define the folowing to see the error code returned by the flash driver as host events
#define DEBUG_EVENTS
#ifdef DEBUG_EVENTS
#include "hal_host.h"
#endif

#include "hal_clk.h"

extern VOID mon_Event(UINT32 ch);
HAL_DMA_CFG_T ram_dmaCfg;

#if (CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
//  #define SEND_EVENT boot_HostUsbEvent
#error
#else
#define SEND_EVENT mon_Event
#endif

//#define SEND_EVENT hal_HstSendEvent

//#define SEND_EVENT(event)

// new mem test as hardware request , default disable
//#define PSRAM_NEW_TEST_EN

// max dma length a one copy (less than 0x40000, total 18 bits)
#define DMA_MAX_BLOCK_LENGTH    (0x20000)

#ifndef FLASH_USB_SEND_EVENT_DELAY
#define FLASH_USB_SEND_EVENT_DELAY    8192 // 0.5s
#endif /* FLASH_USB_SEND_EVENT_DELAY */

//-----------------------------
// Three Data Buffers are available for CoolWatcher
//-----------------------------
#if (FPC_BUFFER_SIZE/4*4 != FPC_BUFFER_SIZE)
#error "FPC_BUFFER_SIZE should be aligned with word boundary"
#endif

#if !(CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
#error "error: You can t use FASTPF_USE_USB without CHIP_HAS_USB"
#endif


static CONST UINT32 pattern[] = { 0x4f35b7da, 0x8e354c91, };
//static CONST UINT32 pattern[] = { 0xa5a5a5a5, 0x5a5a5a5a, 0x4f35b7da, 0x8e354c91, };

UINT32 HAL_UNCACHED_DATA g_dataBuffer_start,psram_check_size,ret_flag=0;
#define _ATE_INFO_LEN_ 30
UINT32 HAL_UNCACHED_DATA  g_ate_info[_ATE_INFO_LEN_];
BOOL fp_CheckPsramData(UINT32  flag, UINT32 g_dataBuffer_start,UINT32 psram_check_size);

PROTECTED BOOL ate_CheckPsramData_noDMA(UINT32  flag, UINT32 dataBuffer_start,UINT32 dataBuffer_end)
{

    for (int i=dataBuffer_start; i<dataBuffer_end; i=i+4)
    {
        *(volatile UINT32 *)((UINT32)i) = flag;
    }


    for (int i=dataBuffer_start; i<dataBuffer_end; i=i+4)
    {
        if (*(volatile UINT32 *)((UINT32)i) != flag)
        {

            SEND_EVENT(0x88500000| (flag&0xff)  );
            SEND_EVENT((UINT32)i);
            SEND_EVENT(flag);
            ret_flag =1;
            g_ate_info[0]=flag;//  *(volatile UINT32 *)(0x81c00020)=flag;
            g_ate_info[1]=i;//   *(volatile UINT32 *)(0x81c00024)=i;
            g_ate_info[2]=(*(UINT32 *)((UINT32)i));//  *(volatile UINT32 *)(0x81c00028)=*(UINT32 *)((UINT32)i);
            g_ate_info[3]=dataBuffer_start;//  *(volatile UINT32 *)(0x81c00030)=dataBuffer_start;
            g_ate_info[4]=dataBuffer_end;//*(volatile UINT32 *)(0x81c00034)=dataBuffer_end;
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

    g_ate_info[5]=flag;// *(volatile UINT32 *)(0x81c0002c)=flag;

    for (i=dataBuffer_start; i<dataBuffer_end; )
    {

        while(!hal_DmaDone());

        if(!(hal_DmaDone()))
        {
            *(volatile UINT32 *)((UINT32)i) = flag;
        }
        else

        {
            // Clear destination buffer
            ram_dmaCfg.srcAddr = NULL;
            ram_dmaCfg.alterDstAddr = NULL;
            ram_dmaCfg.dstAddr=(UINT8*)i;
            ram_dmaCfg.pattern=flag;
            ram_dmaCfg.transferSize=(dataBuffer_end-i)>=DMA_MAX_BLOCK_LENGTH?DMA_MAX_BLOCK_LENGTH:(dataBuffer_end-i);
            ram_dmaCfg.mode = HAL_DMA_MODE_PATTERN;
            ram_dmaCfg.userHandler=NULL;

            while(hal_DmaStart(&ram_dmaCfg)!=HAL_ERR_NO)
            {
            }
            i=i+ram_dmaCfg.transferSize;

            // Wait until cleanup finished
            //              while(!hal_DmaDone())
            //              {
            //              }
        }
    }

    while(!hal_DmaDone())
    {
    }


    for (i=dataBuffer_start; i<dataBuffer_end; i=i+4)
    {
        if (*(volatile UINT32 *)((UINT32)i) != flag)
        {

            SEND_EVENT(0x88500000| (flag&0xff)  );
            SEND_EVENT((UINT32)i);
            SEND_EVENT(flag);
            SEND_EVENT(*(volatile UINT32 *)((UINT32)i));
            SEND_EVENT(*(volatile UINT32 *)((UINT32)(i+4)));
            SEND_EVENT(*(volatile UINT32 *)((UINT32)(i+8)));
            SEND_EVENT(*(volatile UINT32 *)((UINT32)(i+12)));
            ret_flag =1;
            g_ate_info[0]=flag;
            g_ate_info[1]=i;
            g_ate_info[2]=*(UINT32 *)((UINT32)i);
            g_ate_info[3]=dataBuffer_start;
            g_ate_info[4]=dataBuffer_end;
//          while(1);
            return FALSE;
        }
    }
    return TRUE;
}




PROTECTED BOOL fp_CheckDataBuffer_HIGH_ADDR(VOID)
{

#ifndef NO_PSRAM_HIGH_ADDR_CHECK

    typedef struct
    {
        UINT32 startAddr;
        UINT32 len;
    } PSRAM_RANGE_T;

    UINT32 psramBase = (UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM);
    PSRAM_RANGE_T range[] =
    {
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
//              if(!fp_CheckPsramData(pattern[j],range[i].startAddr,range[i].startAddr+range[i].len))
            if(!ate_CheckPsramData_noDMA(pattern[j],range[i].startAddr,range[i].startAddr+range[i].len))
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




u32 test_temp4[8];
u32 jtetmpr;
unsigned char * ptr11;

#define magic_test_ram_apmemory 0x5a5a
#define appmemory_test_bit_line 0x5a

// for 8809e2 psram test
BOOL  test_bit_line_accord_apmemory_advice(VOID)
{ return TRUE;}
BOOL  test_ram_accord_apmemory_advice(VOID)
{

    u32 j ,tt;
    tt = hal_TimGetUpTime();
    ptr11 = (volatile unsigned char *)0xa2000000;
    test_temp4[4] = 0;

    hal_HstSendEvent(0xbb000007);

    for(jtetmpr = 0; jtetmpr < 0x400000; jtetmpr++)
    {
        ptr11[jtetmpr] = magic_test_ram_apmemory;
    }
    for(jtetmpr = 0; jtetmpr < 0x400000; jtetmpr++)
    {

        if(ptr11[jtetmpr] != magic_test_ram_apmemory)
            test_temp4[4] += 1;

    }

    hal_HstSendEvent(0xfffffffa);
    hal_HstSendEvent(((hal_TimGetUpTime()-tt)*1000)/16384);  //  take about 800ms in 78M clk ,if write using UINT16 (0x5a5a),take  400ms
    for(jtetmpr = 0; jtetmpr < 0x400000; jtetmpr++)
    {
        if(ptr11[jtetmpr] != magic_test_ram_apmemory)
        {
            test_temp4[0] = jtetmpr;
            test_temp4[1] = ptr11[jtetmpr];
            test_temp4[2] = &ptr11[jtetmpr];
            // test_temp4[4]= 0xf1;
            hal_HstSendEvent(0xaa000009);
            hal_HstSendEvent(jtetmpr);
            hal_HstSendEvent(ptr11[jtetmpr]);
            hal_HstSendEvent(&ptr11[jtetmpr]);
            hal_HstSendEvent(test_temp4[4]);
            return FALSE;
            // asm("break 2;nop;nop;");

        }
    }

    hal_HstSendEvent(0xfffffffb);
    hal_HstSendEvent(((hal_TimGetUpTime()-tt)*1000)/16384);  //  take about 1200ms in 78M clk. if write using UINT16 (0x5a5a),take  800ms
    return TRUE;


}



#define magic_test_ram 0xa5
BOOL  test_ram_for_special_chip(void)
{
    //return test_bit_line_accord_apmemory_advice();
    return TRUE;

}




PROTECTED BOOL ddr_CheckPsram(VOID)
{

    UINT32 elapsedTime=0,fpsStartMeasTime = hal_TimGetUpTime();
    UINT32 buffer_test_s1 = 0xa2000000;
    UINT32 buffer_test_s2 = 0xa2200000;
    UINT32 buffer_test_e1 = 0xa2200000;
    UINT32 buffer_test_e2 = 0xa2400000;
    //*(volatile UINT32 *)(0xa1c00000)=0x6500b203;

    //20140303 Added As Xiao Yifeng reported this GPIO high low action would increase yield for customer
    //set GPIO FUNCTION
    //*(volatile UINT32 *)(0xa1a24008)=0xffffffff;

    //20140303 Added As Xiao Yifeng reported this GPIO high low action would increase yield for customer
    //set GPIO GPO Low
    //*(volatile UINT32 *)(0xa1a03008)=0xffffffff;
    // *(volatile UINT32 *)(0xa1a03034)=0xfff;

    //20140303 modify test pattern from (0x55555555 0xaaaaaaaa) to (0x5555aaaa 0xaaaa5555) for FT program
    //use 0x55555555 0xaaaaaaaa psram test pattern for FT program
    //use 0xffffffff 0x00000000 0x55555555 0xaaaaaaaa psram test pattern for QA program
    //20140123 use ate_CheckPsramData_noDMA instead of fp_CheckPsramData to avoid xcpu halt while use dual core mode


    //mon_Event(0xcccc001);mon_Event(g_dataBuffer_start);mon_Event(psram_check_size);

    //mon_Event(0xeeffee00);mon_Event(g_dataBuffer_start);mon_Event(0xeeffee02);
    //  ate_CheckPsramData_noDMA(0xffffffff,g_dataBuffer_start,psram_check_size);
    if(FALSE == ate_CheckPsramData_noDMA(0x00000000,buffer_test_s1,buffer_test_e1))
    {
        return FALSE;
    }
    // mon_Event(0xcccc002);

    //20140303 modify test pattern from (0x55555555 0xaaaaaaaa) to (0x5555aaaa 0xaaaa5555) for FT program
    if(FALSE == ate_CheckPsramData_noDMA(0x5555AAAA,buffer_test_s2,buffer_test_e2))
    {
        return FALSE;
    }

    //mon_Event(0xcccc003);

    //20140303 Added As Xiao Yifeng reported this GPIO high low action would increase yield for customer
    //set GPIO GPO High
    // *(volatile UINT32 *)(0xa1a03004)=0xffffffff;
    // *(volatile UINT32 *)(0xa1a03010)=0xffffffff;
    // *(volatile UINT32 *)(0xa1a03030)=0xfff;

    //20140303 modify test pattern from (0x55555555 0xaaaaaaaa) to (0x5555aaaa 0xaaaa5555) for FT program
    if(FALSE == ate_CheckPsramData_noDMA(0xAAAA5555,buffer_test_s1,buffer_test_e1))
    {
        return FALSE;
    }
    //  mon_Event(0xcccc004);

    //elapsedTime = (hal_TimGetUpTime()-fpsStartMeasTime);

    if((ret_flag ==1) ||( FALSE == test_ram_for_special_chip()) )
    {
        g_ate_info[6]=0x22222222;// *(volatile UINT32 *)(0xa1c00000)=0x22222222;
        mon_Event(0x000bad5);
    }

    else
    {
        g_ate_info[6]=0xAAAAAAA8;
        //*(volatile UINT32 *)(0x81c00008)=elapsedTime;
        // *(volatile UINT32 *)(0x81c00004)=hal_TimGetUpTime();
    }

    //mon_Event(0xcccc005);


    return TRUE ;
}


UINT32 ERamSize_Valid_Check(REG16* csBase)
{
    return eram_end_addr2[1];

#if 0
    // Using the CR controlled method
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
        mon_Event(0xcccc0001);
        mon_Event(aptic_size);
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
                            *(volatile UINT32 *)(0xa1c00000)=0x11111111;
                            mon_Event(0xcccc0002);
                            return 0;
                        }
                    }
                    else
                        return eram_end_addr2[index];
                }
            }
        }
    }
    return 0;
#endif
}

INT32                                status = MEMD_ERR_NO;
UINT32 __attribute__((section(".sramucdata"))) s,notmagicNumber=0x2610ffba,write_flsh_addr=0x00000008,readmagicNumber;
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

#if(XCPU_CACHE_MODE_WRITE_BACK==1)
    *(UINT32*)HAL_SYS_GET_UNCACHED_ADDR(&bcpu_stack_base)  = (UINT32) stackStartAddr;
    *(UINT32*)HAL_SYS_GET_UNCACHED_ADDR(&bcpu_main_entry) = (UINT32) bcpuMain;
#else
    bcpu_stack_base = (UINT32) stackStartAddr;
    bcpu_main_entry = (UINT32*) bcpuMain;
#endif

    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Clk_BB_Enable = SYS_CTRL_ENABLE_BB_BCPU;
    hwp_sysCtrl->BB_Rst_Clr = SYS_CTRL_CLR_RST_BCPU;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
}

#define BCPU_MAIN_LOCATION __attribute__((section(".bcpu_main_entry")))
#define CACHED(addr)                ((addr)|0x80000000)

VOID bcpu_main2(VOID);

UINT32 HAL_UNCACHED_DATA flash_check_data_buffer_addr[512]; //Org: 0x82000000

void test_flash(void)
{
    UINT32 bcpuStackBase;

    INT32                                i = 0;
    //UINT32                               cpt;
    //UINT32                               cpt2;
    INT32                                errorDuringCommand;
    INT32                                status = MEMD_ERR_NO;
    // FPC_COMMAND_TYPE_T                   c;
    //UINT32                               s;
    UINT32                               sectorWrite = 0;
    HAL_DMA_FCS_CFG_T                    fcsCtx;
    UINT8                                fcs[3];
    UINT32*                              verifySectorSize;
    UINT32*                              verifySectorAddress;
    UINT32*                              verifyAwaitedFcs;
    UINT32                               verifyFirstBadSector;
    UINT32                               m_CalibRfCrcAdd = 0;



#ifndef CHIP_IS_MODEM
    // hal_SysSetBBClock(HAL_SYS_FREQ_208M);
#endif
    //*(volatile UINT32 *)(0xa1b00030)=0xccccb2a3;

    // bcpu_main2();
    //*(volatile UINT32 *)(0xa1b00040)=0xddddb223;

    extern UINT32 get_flash_ID(UINT32 flash_addr);
    extern  MEMD_SPIFLSH_RDID_T g_spiflash_ID;

    //*(volatile UINT32 *)(0xa1b00000)=0x6500b203;
    // *(volatile UINT32 *)(0xa1b00004)=g_spiflash_ID.manufacturerID;
    // *(volatile UINT32 *)(0xa1b00008)=g_spiflash_ID.device_memory_type_ID;
    // *(volatile UINT32 *)(0xa1b0000c)=g_spiflash_ID.device_capacity_type_ID;

    // UINT32 flash_check_data_buffer_addr = 0x81C0C000; //Org: 0x82000000
    UINT32 flash_check_size = 512;
    UINT32 flash_page_data_size = 32;
    UINT32 flash_prog_addr;
    UINT32 flash_end_addr;
    UINT32 flash_check_addr;
    int buffer_addr_point;
    int flash_64M=0;

    g_ate_info[8]=0x87654321;//  *(volatile UINT32 *)(0xa1b00010)=0x87654321;
    g_ate_info[22]=0x87654321;//  *(volatile UINT32 *)(0xa1b00030)=0x87654321;


    /* // Masked Flash Erase Code 20140303 to avoid HST interface hal after PSRAM BIST
    if(Device_8851KP)
    {
        flash_prog_addr = 0x00000000;
        status =  memd_FlashErase((CHAR*) flash_prog_addr, NULL);
        if (status != MEMD_ERR_NO)
        {
             g_ate_info[8]=0x11111111;
        }

        flash_prog_addr = 0x00001000;
        status =  memd_FlashErase((CHAR*) flash_prog_addr, NULL);
        if (status != MEMD_ERR_NO)
        {
            g_ate_info[8]=0x11112222;
        }

        flash_prog_addr = 0x00002000;
        status =  memd_FlashErase((CHAR*) flash_prog_addr, NULL);
        if (status != MEMD_ERR_NO)
        {
           g_ate_info[8]=0x11113333;
        }

        flash_prog_addr = 0x00003000;
        status =  memd_FlashErase((CHAR*) flash_prog_addr, NULL);
        if (status != MEMD_ERR_NO)
        {
             g_ate_info[8]=0x11114444;
        }

        flash_prog_addr = 0x00004000;
        status =  memd_FlashErase((CHAR*) flash_prog_addr, NULL);
        if (status != MEMD_ERR_NO)
        {
            g_ate_info[8]=0x11115555;
        }
    }
    else
    {

        //*(volatile UINT32 *)(0xa1b00018)=0x76543218;
        flash_prog_addr = 0x00000000;
        status = memd_FlashErase((CHAR*) flash_prog_addr, NULL);
        if (status != MEMD_ERR_NO)
        {
            g_ate_info[8]=0x11116666;
        }

        flash_check_addr = flash_prog_addr | 0x88000000;
        for(int flash_addr_point = flash_check_addr; flash_addr_point < flash_check_addr + flash_check_size; flash_addr_point = flash_addr_point + 4)
        {
            if(*(volatile UINT32 *)(flash_addr_point) != 0xFFFFFFFF)
            {
               g_ate_info[8]=0x11117777;
                break;
            }
        }
        while ( g_ate_info[8]!=0x87654321);


        *(volatile UINT32 *)(0xa1b00018)=0x76543218;
        if (flash_64M)
        {
            flash_prog_addr = 0x00400000;
            status = memd_FlashErase((CHAR*) flash_prog_addr, NULL);
            if (status != MEMD_ERR_NO)
            {
                 g_ate_info[8]=0x11118888;
            }

            flash_check_addr = flash_prog_addr | 0x88000000;
            for(int flash_addr_point = flash_check_addr; flash_addr_point < flash_check_addr + flash_check_size; flash_addr_point = flash_addr_point + 4)
            {
                if(*(volatile UINT32 *)(flash_addr_point) != 0xFFFFFFFF)
                {
                     g_ate_info[8]=0x11119999;
                    break;
                }
            }
        }

    }
    */ // Masked Flash Erase Code 20140303 to avoid HST interface hal after PSRAM BIST

    flash_prog_addr = 0x00000000;

    if(1)//Device_8851KP
    {
        flash_prog_addr = 0x00000000;
        status = memd_FlashErase((CHAR*) flash_prog_addr, NULL);
        if (status != MEMD_ERR_NO)
        {
            g_ate_info[8]=0x33331111;
			return;
        }

        flash_check_addr = flash_prog_addr | 0x88000000;
        for(int flash_addr_point = flash_check_addr; flash_addr_point < flash_check_addr + flash_check_size; flash_addr_point = flash_addr_point + 4)
        {
            if(*(volatile UINT32 *)(flash_addr_point) != 0xFFFFFFFF)
            {
                g_ate_info[8]=0x33332222;
                break;
            }
        }
    }
    if ( g_ate_info[8]!=0x87654321)
    {
        mon_Event(0xf000ebad);
        return;
    }
    hal_TimDelay(163);

    buffer_addr_point = flash_check_data_buffer_addr;
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0x55555555;
        buffer_addr_point = buffer_addr_point + 4;
    }
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0xAAAAAAAA;
        buffer_addr_point = buffer_addr_point + 4;
    }
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0x55555555;
        buffer_addr_point = buffer_addr_point + 4;
    }
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0xAAAAAAAA;
        buffer_addr_point = buffer_addr_point + 4;
    }
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0xAAAAAAAA;
        buffer_addr_point = buffer_addr_point + 4;
    }
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0x55555555;
        buffer_addr_point = buffer_addr_point + 4;
    }
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0xAAAAAAAA;
        buffer_addr_point = buffer_addr_point + 4;
    }
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0x55555555;
        buffer_addr_point = buffer_addr_point + 4;
    }
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0xAAAAAAAA;
        buffer_addr_point = buffer_addr_point + 4;
    }
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0x55555555;
        buffer_addr_point = buffer_addr_point + 4;
    }
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0xAAAAAAAA;
        buffer_addr_point = buffer_addr_point + 4;
    }
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0x55555555;
        buffer_addr_point = buffer_addr_point + 4;
    }
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0x55555555;
        buffer_addr_point = buffer_addr_point + 4;
    }
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0xAAAAAAAA;
        buffer_addr_point = buffer_addr_point + 4;
    }
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0x55555555;
        buffer_addr_point = buffer_addr_point + 4;
    }
    for(int byte_counter = 0; byte_counter < flash_page_data_size; byte_counter++)
    {
        *(volatile UINT32 *)(buffer_addr_point) = 0xAAAAAAAA;
        buffer_addr_point = buffer_addr_point + 4;
    }


    flash_prog_addr = 0x00000000;
    hal_DcacheFlushAll(TRUE);
    // mon_Event(0xffffc111); mon_Event(flash_check_data_buffer_addr);
    status = memd_FlashWrite((CHAR*) flash_prog_addr, flash_check_size, &s, (CHAR*) flash_check_data_buffer_addr);
    if (status != MEMD_ERR_NO)
    {
        g_ate_info[8]=0x22221111;
        mon_Event(0xbadf0001);
        return;
    }


    flash_check_addr = flash_prog_addr | 0xa8000000;
    buffer_addr_point = flash_check_data_buffer_addr;
    for(int flash_addr_point= flash_check_addr; flash_addr_point < flash_check_addr + flash_check_size; flash_addr_point = flash_addr_point + 4)
    {
        if(*(volatile UINT32 *)(flash_addr_point) != *(volatile UINT32 *)(buffer_addr_point))
        {
            g_ate_info[8]=0x22222222;
            mon_Event(0xbadf0002);
            mon_Event((flash_addr_point));
            mon_Event((*(volatile UINT32 *)(flash_addr_point)));
            mon_Event((buffer_addr_point));
            mon_Event((*(volatile UINT32 *)(buffer_addr_point)));
            mon_Event(0x3fffffff);
            break;
        }
        buffer_addr_point = buffer_addr_point + 4;
    }
    if ( g_ate_info[8]!=0x87654321)
    {
        mon_Event(0xf0001bad);
        return;
    }

#if 1

    if(1)//Device_8851KP
    {
        flash_prog_addr = 0x00000000;
        status = memd_FlashErase((CHAR*) flash_prog_addr, NULL);
        if (status != MEMD_ERR_NO)
        {
            g_ate_info[8]=0x33331111;
        }

        flash_check_addr = flash_prog_addr | 0xa8000000;
        for(int flash_addr_point = flash_check_addr; flash_addr_point < flash_check_addr + flash_check_size; flash_addr_point = flash_addr_point + 4)
        {
            if(*(volatile UINT32 *)(flash_addr_point) != 0xFFFFFFFF)
            {

                mon_Event(0xbadf0003);
                mon_Event(flash_addr_point);
                g_ate_info[8]=0x3e332222;

                break;
            }
        }
    }


#endif

    // uart_test_fun();

    /*
    *(volatile UINT32 *)(0xa1b00018)=0x43218765;
    if (flash_64M)
    {
        flash_prog_addr = 0x00400000;
        status = memd_FlashErase((CHAR*) flash_prog_addr, NULL);
        if (status != MEMD_ERR_NO)
        {
            g_ate_info[8]=0x77777777;
        }

        flash_check_addr = flash_prog_addr | 0x88000000;
        for(int flash_addr_point = flash_check_addr; flash_addr_point < flash_check_addr + flash_check_size; flash_addr_point = flash_addr_point + 4)
        {
            if(*(volatile UINT32 *)(flash_addr_point) != 0xFFFFFFFF)
            {
                 g_ate_info[8]=0x99999999;
                break;
            }
        }
    }
    */


    if(g_ate_info[8] == 0x87654321 )
    {
        g_ate_info[8] = 0xAAAAAAAA;

        *(volatile UINT32 *)(0xa1b00010) = g_ate_info[8];

    }
    else
    {
        *(volatile UINT32 *)(0xa1b00010) = 0xbadbad00;

    }
    //mon_Event(g_ate_info[8]);
    //mon_Event(0x11111111);
    g_ate_info[_ATE_INFO_LEN_-1]=0xffff0e0d;
    // hal_SysStopBcpu(); //20140303 Debug
    //while (1);

}

void test_gouda_merge(void)
{
    HAL_GOUDA_WINDOW_T roi;
    UINT16 i,k;
    UINT32 m,j;

    UINT16 test_img_lay1[TEST_WIDTH * TEST_HEIGHT];
    UINT16 test_img_lay2[TEST_WIDTH * TEST_HEIGHT];

    m = 1;
    for(i = 0 ; i< TEST_WIDTH; i++)
    {
        for(k = 0 ; k < TEST_HEIGHT; k++)
        {
            test_img_lay1[i*TEST_WIDTH + k] = k+0x5;
            test_img_lay2[i*TEST_WIDTH + k] = i+20;

        }

    }

    for (i =0; i<VL_IMAGE_WIDTH*VL_IMAGE_HEIGHT/4; i++)
    {
        VL_IMAGE[i] = i+5;

    }
    for (i =0; i<VL_IMAGE_WIDTH*VL_IMAGE_HEIGHT/4; i++)
    {
        VL_IMAGEU[i] = i+2;
        VL_IMAGEV[i] = m++;

    }

    roi.tlPX=0;
    roi.tlPY= 0;
    roi.brPX=TEST_WIDTH - 1;
    roi.brPY=TEST_HEIGHT - 1;


#if 1
    // configure layer 0
    def_OvlLayer0.addr = (UINT32*)test_img_lay1;
    def_OvlLayer0.fmt = HAL_GOUDA_IMG_FORMAT_RGB565;
    def_OvlLayer0.stride = 0; // let hal gouda decide
    def_OvlLayer0.pos.tlPX = 0;
    def_OvlLayer0.pos.tlPY = 0;
    def_OvlLayer0.pos.brPX = TEST_WIDTH - 1;
    def_OvlLayer0.pos.brPY = TEST_HEIGHT - 1;
    def_OvlLayer0.alpha = 255;
    def_OvlLayer0.cKeyEn = FALSE;


    // configure layer 1

    def_OvlLayer1.addr = (UINT32*)test_img_lay2;
    def_OvlLayer1.fmt = HAL_GOUDA_IMG_FORMAT_RGB565;
    def_OvlLayer1.stride = 0; // let hal gouda decide
    def_OvlLayer1.pos.tlPX = 0;
    def_OvlLayer1.pos.tlPY = 0;
    def_OvlLayer1.pos.brPX = TEST_WIDTH- 1;
    def_OvlLayer1.pos.brPY = TEST_HEIGHT - 1;
    def_OvlLayer1.alpha = 100; // 255;
    def_OvlLayer1.cKeyEn = FALSE;
    def_OvlLayer1.cKeyColor = 0;
    def_OvlLayer1.cKeyMask = HAL_GOUDA_CKEY_MASK_OFF;

#if 0
    // configure layer 2
    def_OvlLayer2.addr = (UINT32*)test_img_lay3;
    def_OvlLayer2.fmt = HAL_GOUDA_IMG_FORMAT_RGBA;
    def_OvlLayer2.stride = 0; // let hal gouda decide
    def_OvlLayer2.pos.tlPX = 0;
    def_OvlLayer2.pos.tlPY = 0;
    def_OvlLayer2.pos.brPX = TEST_WIDTH - 1;
    def_OvlLayer2.pos.brPY = TEST_HEIGHT - 1;
    def_OvlLayer2.alpha = 255;
    def_OvlLayer2.cKeyEn = FALSE;
    def_OvlLayer2.cKeyColor = 0;
    def_OvlLayer2.cKeyMask = HAL_GOUDA_CKEY_MASK_OFF;
#endif

#if 1
    // configure video layer
    def_Video.addrY = (u32*)VL_IMAGE;
    def_Video.addrU = (u32*)VL_IMAGEU;
    def_Video.addrV = (u32*)VL_IMAGEV;
    def_Video.fmt = HAL_GOUDA_IMG_FORMAT_IYUV;

    def_Video.height = VL_IMAGE_HEIGHT/2; //...too big
    def_Video.width = VL_IMAGE_WIDTH/3;
    def_Video.stride = VL_IMAGE_WIDTH;
    def_Video.pos.tlPX = 0;
    def_Video.pos.tlPY = 0;
    def_Video.pos.brPX =TEST_WIDTH- 1;;
    def_Video.pos.brPY = TEST_HEIGHT - 1;;
    def_Video.alpha = 152;
    def_Video.cKeyEn = FALSE;
    def_Video.cKeyColor = 0;
    def_Video.cKeyMask = HAL_GOUDA_CKEY_MASK_OFF;
    def_Video.depth = HAL_GOUDA_VID_LAYER_BETWEEN_1_0;
#endif
    // open the layers
    hal_GoudaOvlLayerOpen(HAL_GOUDA_OVL_LAYER_ID0, &def_OvlLayer0);
    hal_GoudaOvlLayerOpen(HAL_GOUDA_OVL_LAYER_ID1, &def_OvlLayer1);
    //hal_GoudaOvlLayerOpen(HAL_GOUDA_OVL_LAYER_ID2, &def_OvlLayer2);
    hal_GoudaVidLayerOpen(&def_Video);
    // use fill rect as layers are enabled, gouda will blit them ;)
    hal_GoudaBlitRoi2Ram(&test_result_gouda[0], 16, &roi, NULL);
// mon_Event(0xee77110a);
    while(hal_GoudaIsActive())
    {
        j++;
        hal_TimDelay(16);
        if(j > 100) break;
    }
    //mon_Event(0xee77113b); mon_Event(test_result_gouda[3]);
    hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID0);
    hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID1);
    // hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID2);
    hal_GoudaVidLayerClose();


    for(i = 0; i < 64 ; i++)
    {
        if(gouda_verify[i] != test_result_gouda[i])
        {

            //*(volatile UINT32 *)(0xa1c01010)=0xb;
            mon_Event(0x21da000b);
            //mon_Event((UINT32 *)test_result_gouda);mon_Event(gouda_verify[0]);
            //  mon_Event(test_result_gouda[0]);mon_Event(test_result_gouda[1]);
            //while(1){};
            g_ate_info[9] =0xf0008002;//gouda fail
            break;
        }
    }

    mon_Event(0x90da90da);


#endif
}


#if 1
typedef enum UART_TEST_STATUS
{
    UART_TEST_STATUS_OK,
    UART_TEST_STATUS_KO,
    UART_TEST_STATUS_SHIP
} UART_TEST_STATUS_T;

HAL_UART_ID_T uartid = HAL_UART_2;
PRIVATE VOID Uart_Clean(VOID)
{
    hal_UartFifoFlush(uartid);
    hal_UartClose(uartid);
}

PRIVATE VOID Uart_Init(HAL_UART_BAUD_RATE_T      rate,
                       HAL_UART_TRANSFERT_MODE_T mode,
                       HAL_UART_DATA_BITS_T      datab)
{
    HAL_UART_CFG_T uartConfig =
    {
        datab,
        HAL_UART_1_STOP_BIT,
        HAL_UART_NO_PARITY,
        HAL_UART_RX_TRIG_1,
        HAL_UART_TX_TRIG_3QUARTER,
        HAL_UART_AFC_LOOP_BACK | HAL_UART_AFC_MODE_RX_TRIG_4,
        HAL_UART_IRDA_MODE_DISABLE,
        rate,
        mode,
        mode
    };

    hal_UartOpen(uartid,&uartConfig);

    hal_UartIrqEnableLoopBackMode(uartid,TRUE);
    hal_UartFifoFlush(uartid);

    hal_UartSetRts(uartid,TRUE);
}

PRIVATE  UART_TEST_STATUS_T Uart_Test_Mode_Dma_Poll(HAL_UART_BAUD_RATE_T rate)
{
    UINT8              buffer_input[64] ;
    UINT8              buffer_output[64];


    UINT32             pos = 0;
    UINT32             len_s,k;
    //mon_Event(0xaaaa0001);

    for(k  = 0; k<sizeof(buffer_output); k++)  buffer_input[k]=k;
    Uart_Init(rate, HAL_UART_TRANSFERT_MODE_DMA_POLLING, HAL_UART_8_DATA_BITS);



    hal_UartClearErrorStatus(uartid);

    len_s = hal_UartSendData(uartid,buffer_input,
                             sizeof(buffer_input));




    if(len_s == 0)
    {


        return(UART_TEST_STATUS_SHIP);
    }
    if(len_s                    != sizeof(buffer_input) ||
            hal_UartGetErrorStatus(uartid) != HAL_ERR_NO)
    {
        Uart_Clean();

        // mon_Event(0xaaaa0003);
        return(UART_TEST_STATUS_KO);
    }


    hal_UartGetData(uartid,&buffer_output[pos],
                    sizeof(buffer_output)-pos);

    do
    {
        if(hal_UartRxDmaDone(uartid) == TRUE)
        {

            ///mon_Event(0xaaaa0005);
            break;
        }
        if(hal_UartGetErrorStatus(uartid) != HAL_ERR_NO)
        {
            Uart_Clean();

            // mon_Event(0xaaaa0004);
            return(UART_TEST_STATUS_KO);
        }

    }
    while(1);



//while(1){};
//   Uart_Clean();


///mon_Event(0xaaaa0012); //mon_Event(buffer_output);mon_Event(sizeof(buffer_output));



    hal_SysInvalidateCache(buffer_output, sizeof(buffer_output));

    //  return UART_TEST_STATUS_OK;

    for(pos = 0; pos < sizeof(buffer_output); pos++)
    {

        //mon_Event(buffer_output[pos]);
        if(buffer_output[pos] != buffer_input[pos])
        {
            // mon_Event(0xaaaa000f);
            return(UART_TEST_STATUS_KO);
        }
    }
    //mon_Event(0xaaaa000e);

    return(UART_TEST_STATUS_OK);
}


#if 0
PUBLIC VOID hal_UartIrqEnableLoopBackMode(HAL_UART_ID_T id, BOOL enable)
{
    HAL_ASSERT(id < HAL_UART_QTY, "Erroneous UART id:%d", id);

    // Set/clear IRQ mask
    if (enable)
    {
        g_halUartHwpArray[id-1]->ctrl |= UART_LOOP_BACK_MODE;
    }
    else
    {
        g_halUartHwpArray[id-1]->ctrl &= ~(UART_LOOP_BACK_MODE);
    }
}


#endif
void uart_test_fun(void)
{

    UINT8 i ;
    for(i = 0 ; i++; i<2)
    {



        if (UART_TEST_STATUS_OK == Uart_Test_Mode_Dma_Poll(HAL_UART_BAUD_RATE_115200))
        {
            g_ate_info[6] =0xaaaaaaac;// *(volatile UINT32*)(0xa1c00000)=0xaaaaaaac;
        }
        else
        {
            g_ate_info[6]=0x0000000c;
            // while(1){};
            g_ate_info[20]=0xf0008003;//uart fail

        }
        uartid = HAL_UART_1;
    }


}


#endif


#if 0

// =============================================================================
// hal_BoardSetup
// -----------------------------------------------------------------------------
/// Apply board dependent configuration to HAL
/// @param halCfg Pointer to HAL configuration structure (from the target
/// module).
// ============================================================================
PROTECTED VOID hal_BoardSetup(CONST HAL_CFG_CONFIG_T* halCfg)
{
    // Store the config pointer for later use in hal
    g_halCfg = halCfg;
//#if defined(_FLASH_PROGRAMMER) && defined(FORCE_GPIO_INPUT_GPO_LOW)   //   请注意这个宏   _FLASH_PROGRAMMER   ，否则很多io配置不对。


#endif
    PROTECTED BOOL psram_up32_write_read(UINT32 start, UINT32 end)
    {
        UINT32 i;
        UINT32 base = start;
        for (i=start; i<end; i=i+4)
        {
            *((volatile UINT32 *)i) = base;
            base = *((volatile UINT32 *)i) +4;
        }
        if(base != end)
        {
            SEND_EVENT(base);
            SEND_EVENT(end);
            return FALSE;
        }
        else
            return TRUE;
    }


    PROTECTED BOOL ddr_dma_block_test(UINT32 start, UINT32 end)
    {
        UINT32 i;
        BOOL ret = TRUE;
        UINT32 base = start;

        // initial pre half buffer with cpu
        ret = psram_up32_write_read(start, start/2 + end/2);
        if(ret)
        {
            // dma copy
            for (i=start/2 + end/2; i<end;)
            {
                while(!hal_DmaDone());

                // Clear destination buffer
                ram_dmaCfg.srcAddr = base;
                ram_dmaCfg.alterDstAddr = NULL;
                ram_dmaCfg.dstAddr=(UINT8*)i;
                ram_dmaCfg.pattern=0;
                ram_dmaCfg.transferSize=(end-i)>=DMA_MAX_BLOCK_LENGTH?DMA_MAX_BLOCK_LENGTH:(end-i);
                ram_dmaCfg.mode = HAL_DMA_MODE_NORMAL;
                ram_dmaCfg.userHandler=NULL;
                mon_Event(i);
                mon_Event(ram_dmaCfg.transferSize);

                while(hal_DmaStart(&ram_dmaCfg)!=HAL_ERR_NO)
                {
                }
                i=i+ram_dmaCfg.transferSize;
                base = base + ram_dmaCfg.transferSize;
                //SEND_EVENT(i);
            }

        }
        else
        {
            mon_Event(0x77777777);
            return FALSE;
        }



        for (i=start; i<(start/2+end/2); i=i+4)
        {
            if (*(volatile UINT32 *)((UINT32)i) != *(volatile UINT32 *)((UINT32)(i+(end-start)/2)))
            {
                mon_Event(0x88500000);
                mon_Event((UINT32)i);
                mon_Event(i+(end-start)/2);
                mon_Event(*(volatile UINT32 *)((UINT32)i));
                mon_Event(0x88500001);
                mon_Event(*(volatile UINT32 *)((UINT32)(i+(end-start)/2)));

//          while(1);
                return FALSE;
            }
        }

        return TRUE;
    }


#ifdef PSRAM_NEW_TEST_EN
// increase addr with 32 bits pattern write
    PROTECTED BOOL fp_psram_up32_pattern_read_after_write(UINT32 start, UINT32 end, UINT32 value)
    {
        UINT32 i;
        for (i=start; i<end; i=i+4)
        {
            *((volatile UINT32 *)i) = value;
        }
        for (i=start; i<end; i=i+4)
        {
            if(*((volatile UINT32 *)i) != value)
            {
                mon_Event(0xcccc0001);
                return FALSE;
            }
        }

        return TRUE;
    }

// increase addr with 16 bits pattern write
    PROTECTED BOOL fp_psram_up16_pattern_read_after_write(UINT32 start, UINT32 end, UINT16 value)
    {
        UINT32 i;
        for (i=start; i<end; i=i+2)
        {
            *((volatile UINT16 *)i) = value;
        }
        for (i=start; i<end; i=i+2)
        {
            if(*((volatile UINT16 *)i) != value)
            {
                mon_Event(0xcccc0002);
                return FALSE;
            }
        }

        return TRUE;
    }

// increase addr with 8 bits pattern write
    PROTECTED BOOL fp_psram_up8_pattern_read_after_write(UINT32 start, UINT32 end, UINT8 value)
    {
        UINT32 i;
        for (i=start; i<end; i=i+1)
        {
            *((volatile UINT8 *)i) = value;
        }
        for (i=start; i<end; i=i+1)
        {
            if(*((volatile UINT8 *)i) != value)
            {
                mon_Event(0xcccc0003);
                return FALSE;
            }
        }

        return TRUE;
    }

// decrease addr with 32 bits pattern write
    PROTECTED BOOL fp_psram_down32_pattern_read_after_write(UINT32 start, UINT32 end, UINT32 value)
    {
        UINT32 i;
        for (i=end-4; i>=start; i=i-4)
        {
            *((volatile UINT32 *)i) = value;
        }
        for (i=end-4; i>=start; i=i-4)
        {
            if(*((volatile UINT32 *)i) != value)
            {
                mon_Event(0xcccc0004);
                return FALSE;
            }
        }

        return TRUE;
    }

//write then read, data and addr increase in 2 way
    PROTECTED BOOL fp_psram_up32_write_read(UINT32 start, UINT32 end)
    {
        UINT32 i;
        UINT32 base = start;
        for (i=start; i<end; i=i+4)
        {
            *((volatile UINT32 *)i) = base;
            base = *((volatile UINT32 *)i) +4;
        }
        if(base != end)
        {
            SEND_EVENT(base);
            SEND_EVENT(end);
            return FALSE;
        }
        else
            return TRUE;
    }

    PROTECTED BOOL fp_psram_dma_block(UINT32 start, UINT32 end)
    {
        UINT32 i;
        BOOL ret = TRUE;
        UINT32 base = start;

        // initial pre half buffer with cpu
        ret = fp_psram_up32_write_read(start, start/2 + end/2);
        if(ret)
        {
            // dma copy
            for (i=start/2 + end/2; i<end;)
            {
                while(!hal_DmaDone());

                // Clear destination buffer
                ram_dmaCfg.srcAddr = base;
                ram_dmaCfg.alterDstAddr = NULL;
                ram_dmaCfg.dstAddr=(UINT8*)i;
                ram_dmaCfg.pattern=0;
                ram_dmaCfg.transferSize=(end-i)>=DMA_MAX_BLOCK_LENGTH?DMA_MAX_BLOCK_LENGTH:(end-i);
                ram_dmaCfg.mode = HAL_DMA_MODE_NORMAL;
                ram_dmaCfg.userHandler=NULL;
                //SEND_EVENT(i);
                //SEND_EVENT(ram_dmaCfg.transferSize);

                while(hal_DmaStart(&ram_dmaCfg)!=HAL_ERR_NO)
                {
                }
                i=i+ram_dmaCfg.transferSize;
                base = base + ram_dmaCfg.transferSize;
                //SEND_EVENT(i);
            }

        }
        else
        {
            SEND_EVENT(0x77777777);
            return FALSE;
        }



        for (i=start; i<(start/2+end/2); i=i+4)
        {
            if (*(volatile UINT32 *)((UINT32)i) != *(volatile UINT32 *)((UINT32)(i+(end-start)/2)))
            {
                SEND_EVENT(0x88500000);
                SEND_EVENT((UINT32)i);
                SEND_EVENT(i+(end-start)/2);
                SEND_EVENT(*(volatile UINT32 *)((UINT32)i));
                SEND_EVENT(0x88500001);
                SEND_EVENT(*(volatile UINT32 *)((UINT32)(i+(end-start)/2)));

//          while(1);
                return FALSE;
            }
        }

        return TRUE;
    }

    PROTECTED BOOL fp_psram_dma_no_aligned(UINT32 start, UINT32 end)
    {
        UINT32 i, src, dst, size;
        BOOL ret = TRUE;
        UINT32 base = start;
        UINT32 length = end - start;

        // initial pre half buffer with cpu
        ret = fp_psram_up32_write_read(start, start + length/2);
        if(ret)
        {
            // case 1, length not aligned, src, dst aligned
            src = start;
            dst = start + length/2;
            size = 0x2f003;
            // dma copy
            // wait prev dma done if it needed
            while(!hal_DmaDone());
            for (i=0; i<size;)
            {
                // Clear destination buffer
                ram_dmaCfg.srcAddr = src;
                ram_dmaCfg.alterDstAddr = NULL;
                ram_dmaCfg.dstAddr=(UINT8*)dst;
                ram_dmaCfg.pattern=0;
                ram_dmaCfg.transferSize = (size-i)<=DMA_MAX_BLOCK_LENGTH? (size-i): DMA_MAX_BLOCK_LENGTH;
                ram_dmaCfg.mode = HAL_DMA_MODE_NORMAL;
                ram_dmaCfg.userHandler=NULL;
                //SEND_EVENT(i);
                //SEND_EVENT(ram_dmaCfg.transferSize);

                while(hal_DmaStart(&ram_dmaCfg)!=HAL_ERR_NO);

                i=i+ram_dmaCfg.transferSize;
                src = src + ram_dmaCfg.transferSize;
                dst = dst + ram_dmaCfg.transferSize;
                //SEND_EVENT(i);
                while(!hal_DmaDone());
            }

            // data check
            src = start;
            dst = start + length/2;
            for(i=0; i<size; i++)
            {
                if (*(volatile UINT8 *)((UINT32)(dst+i)) != *(volatile UINT8 *)((UINT32)(src+i)))
                {
                    SEND_EVENT(0x8809e000);
                    SEND_EVENT(dst+i);
                    SEND_EVENT(src+i);
                    SEND_EVENT(*(volatile UINT8 *)((UINT32)(dst+i)));
                    SEND_EVENT(*(volatile UINT8 *)((UINT32)(src+i)));
                    return FALSE;
                }
            }

            // case 2, length aligned, src, dst not aligned
            src = start+3;
            dst = start + length/2 +3;
            size = 0x22000;
            // dma copy
            // wait prev dma done if it needed
            while(!hal_DmaDone());
            for (i=0; i<size;)
            {
                // Clear destination buffer
                ram_dmaCfg.srcAddr = src;
                ram_dmaCfg.alterDstAddr = NULL;
                ram_dmaCfg.dstAddr=(UINT8*)dst;
                ram_dmaCfg.pattern=0;
                ram_dmaCfg.transferSize = (size-i)<=DMA_MAX_BLOCK_LENGTH? (size-i): DMA_MAX_BLOCK_LENGTH;
                ram_dmaCfg.mode = HAL_DMA_MODE_NORMAL;
                ram_dmaCfg.userHandler=NULL;
                //SEND_EVENT(i);
                //SEND_EVENT(ram_dmaCfg.transferSize);

                while(hal_DmaStart(&ram_dmaCfg)!=HAL_ERR_NO);

                i=i+ram_dmaCfg.transferSize;
                src = src + ram_dmaCfg.transferSize;
                dst = dst + ram_dmaCfg.transferSize;
                //SEND_EVENT(i);
                while(!hal_DmaDone());
            }

            // data check
            src = start+3;
            dst = start + length/2 +3;
            for(i=0; i<size; i++)
            {
                if (*(volatile UINT8 *)((UINT32)(dst+i)) != *(volatile UINT8 *)((UINT32)(src+i)))
                {
                    SEND_EVENT(0x8809e001);
                    SEND_EVENT(dst+i);
                    SEND_EVENT(src+i);
                    SEND_EVENT(*(volatile UINT8 *)((UINT32)(dst+i)));
                    SEND_EVENT(*(volatile UINT8 *)((UINT32)(src+i)));
                    return FALSE;
                }
            }

            // case 3, length,src aligned,  dst not aligned
            src = start;
            dst = start + length/2 +1;
            size = 0x22000;
            // dma copy
            // wait prev dma done if it needed
            while(!hal_DmaDone());
            for (i=0; i<size;)
            {
                // Clear destination buffer
                ram_dmaCfg.srcAddr = src;
                ram_dmaCfg.alterDstAddr = NULL;
                ram_dmaCfg.dstAddr=(UINT8*)dst;
                ram_dmaCfg.pattern=0;
                ram_dmaCfg.transferSize = (size-i)<=DMA_MAX_BLOCK_LENGTH? (size-i): DMA_MAX_BLOCK_LENGTH;
                ram_dmaCfg.mode = HAL_DMA_MODE_NORMAL;
                ram_dmaCfg.userHandler=NULL;
                //SEND_EVENT(i);
                //SEND_EVENT(ram_dmaCfg.transferSize);

                while(hal_DmaStart(&ram_dmaCfg)!=HAL_ERR_NO);

                i=i+ram_dmaCfg.transferSize;
                src = src + ram_dmaCfg.transferSize;
                dst = dst + ram_dmaCfg.transferSize;
                //SEND_EVENT(i);
                while(!hal_DmaDone());
            }

            // data check
            src = start;
            dst = start + length/2 +1;
            for(i=0; i<size; i++)
            {
                if (*(volatile UINT8 *)((UINT32)(dst+i)) != *(volatile UINT8 *)((UINT32)(src+i)))
                {
                    SEND_EVENT(0x8809e002);
                    SEND_EVENT(dst+i);
                    SEND_EVENT(src+i);
                    SEND_EVENT(*(volatile UINT8 *)((UINT32)(dst+i)));
                    SEND_EVENT(*(volatile UINT8 *)((UINT32)(src+i)));
                    return FALSE;
                }
            }

            // case 4, length,dst aligned,  src not aligned
            src = start + 1;
            dst = start + length/2 ;
            size = 0x22000;
            // dma copy
            // wait prev dma done if it needed
            while(!hal_DmaDone());
            for (i=0; i<size;)
            {
                // Clear destination buffer
                ram_dmaCfg.srcAddr = src;
                ram_dmaCfg.alterDstAddr = NULL;
                ram_dmaCfg.dstAddr=(UINT8*)dst;
                ram_dmaCfg.pattern=0;
                ram_dmaCfg.transferSize = (size-i)<=DMA_MAX_BLOCK_LENGTH? (size-i): DMA_MAX_BLOCK_LENGTH;
                ram_dmaCfg.mode = HAL_DMA_MODE_NORMAL;
                ram_dmaCfg.userHandler=NULL;
                //SEND_EVENT(i);
                //SEND_EVENT(ram_dmaCfg.transferSize);

                while(hal_DmaStart(&ram_dmaCfg)!=HAL_ERR_NO);

                i=i+ram_dmaCfg.transferSize;
                src = src + ram_dmaCfg.transferSize;
                dst = dst + ram_dmaCfg.transferSize;
                //SEND_EVENT(i);
                while(!hal_DmaDone());
            }

            // data check
            src = start + 1;
            dst = start + length/2 ;
            for(i=0; i<size; i++)
            {
                if (*(volatile UINT8 *)((UINT32)(dst+i)) != *(volatile UINT8 *)((UINT32)(src+i)))
                {
                    SEND_EVENT(0x8809e003);
                    SEND_EVENT(dst+i);
                    SEND_EVENT(src+i);
                    SEND_EVENT(*(volatile UINT8 *)((UINT32)(dst+i)));
                    SEND_EVENT(*(volatile UINT8 *)((UINT32)(src+i)));
                    return FALSE;
                }
            }

            // case 5, src aligned,  dst, length not aligned
            src = start ;
            dst = start + length/2 +3;
            size = 0x22001;
            // dma copy
            // wait prev dma done if it needed
            while(!hal_DmaDone());
            for (i=0; i<size;)
            {
                // Clear destination buffer
                ram_dmaCfg.srcAddr = src;
                ram_dmaCfg.alterDstAddr = NULL;
                ram_dmaCfg.dstAddr=(UINT8*)dst;
                ram_dmaCfg.pattern=0;
                ram_dmaCfg.transferSize = (size-i)<=DMA_MAX_BLOCK_LENGTH? (size-i): DMA_MAX_BLOCK_LENGTH;
                ram_dmaCfg.mode = HAL_DMA_MODE_NORMAL;
                ram_dmaCfg.userHandler=NULL;
                //SEND_EVENT(i);
                //SEND_EVENT(ram_dmaCfg.transferSize);

                while(hal_DmaStart(&ram_dmaCfg)!=HAL_ERR_NO);

                i=i+ram_dmaCfg.transferSize;
                src = src + ram_dmaCfg.transferSize;
                dst = dst + ram_dmaCfg.transferSize;
                //SEND_EVENT(i);
                while(!hal_DmaDone());
            }

            // data check
            src = start ;
            dst = start + length/2 +3;
            for(i=0; i<size; i++)
            {
                if (*(volatile UINT8 *)((UINT32)(dst+i)) != *(volatile UINT8 *)((UINT32)(src+i)))
                {
                    SEND_EVENT(0x8809e004);
                    SEND_EVENT(dst+i);
                    SEND_EVENT(src+i);
                    SEND_EVENT(*(volatile UINT8 *)((UINT32)(dst+i)));
                    SEND_EVENT(*(volatile UINT8 *)((UINT32)(src+i)));
                    return FALSE;
                }
            }

            // case 6, dst aligned,  src, length not aligned
            src = start +1;
            dst = start + length/2;
            size = 0x22003;
            // dma copy
            // wait prev dma done if it needed
            while(!hal_DmaDone());
            for (i=0; i<size;)
            {
                // Clear destination buffer
                ram_dmaCfg.srcAddr = src;
                ram_dmaCfg.alterDstAddr = NULL;
                ram_dmaCfg.dstAddr=(UINT8*)dst;
                ram_dmaCfg.pattern=0;
                ram_dmaCfg.transferSize = (size-i)<=DMA_MAX_BLOCK_LENGTH? (size-i): DMA_MAX_BLOCK_LENGTH;
                ram_dmaCfg.mode = HAL_DMA_MODE_NORMAL;
                ram_dmaCfg.userHandler=NULL;
                //SEND_EVENT(i);
                //SEND_EVENT(ram_dmaCfg.transferSize);

                while(hal_DmaStart(&ram_dmaCfg)!=HAL_ERR_NO);

                i=i+ram_dmaCfg.transferSize;
                src = src + ram_dmaCfg.transferSize;
                dst = dst + ram_dmaCfg.transferSize;
                //SEND_EVENT(i);
                while(!hal_DmaDone());
            }

            // data check
            src = start +1;
            dst = start + length/2;
            for(i=0; i<size; i++)
            {
                if (*(volatile UINT8 *)((UINT32)(dst+i)) != *(volatile UINT8 *)((UINT32)(src+i)))
                {
                    SEND_EVENT(0x8809e005);
                    SEND_EVENT(dst+i);
                    SEND_EVENT(src+i);
                    SEND_EVENT(*(volatile UINT8 *)((UINT32)(dst+i)));
                    SEND_EVENT(*(volatile UINT8 *)((UINT32)(src+i)));
                    return FALSE;
                }
            }
        }
        else
        {
            SEND_EVENT(0x77777777);
            return FALSE;
        }


        return TRUE;
    }


    PROTECTED UINT32 fs_psram_test(UINT32 start, UINT32 end)
    {
        BOOL ret = TRUE;

        ret = fp_psram_up32_pattern_read_after_write(start, end, 0x5a5a5a5a);
        if(!ret)
            return 1;
        ret = fp_psram_up32_pattern_read_after_write(start, end, 0xa5a5a5a5);
        if(!ret)
            return 2;

        ret = fp_psram_up16_pattern_read_after_write(start, end, 0x55aa);
        if(!ret)
            return 3;
        ret = fp_psram_up16_pattern_read_after_write(start, end, 0xaa55);
        if(!ret)
            return 4;

        ret = fp_psram_up8_pattern_read_after_write(start, end, 0x5a);
        if(!ret)
            return 5;
        ret = fp_psram_up8_pattern_read_after_write(start, end, 0xa5);
        if(!ret)
            return 6;

        ret = fp_psram_down32_pattern_read_after_write(start, end, 0x5555aaaa);
        if(!ret)
            return 7;
        ret = fp_psram_down32_pattern_read_after_write(start, end, 0xaaaa5555);
        if(!ret)
            return 8;

        ret = fp_psram_up32_write_read(start, end);
        if(!ret)
            return 9;

        ret = fp_CheckPsramData(0x12345678,start, end);
        if(!ret)
            return 10;

        ret = fp_psram_dma_block(start,end);
        if(!ret)
            return 11;

        ret = fp_psram_dma_no_aligned(start,end);
        if(!ret)
            return 12;

        return 0;
    }
#endif  // PSRAM_NEW_TEST_EN
    extern UINT32 _bb_sram_romed_globals_start;
    extern  MEMD_SPIFLSH_RDID_T g_spiflash_ID;


    PROTECTED VOID rfd_XcvRegForceWriteSingle(UINT32 addr, UINT32 data)
    {
        UINT8 array[RDA6220_ONE_CMD_BYTE_QTY];
        UINT32 cmdWord = RDA6220_WRITE_FMT(addr, data);

        // Hardcoded
        array[0] = (cmdWord >> (3 * BYTE_SIZE)) & BYTE_MASK;
        array[1] = (cmdWord >> (2 * BYTE_SIZE)) & BYTE_MASK;
        array[2] = (cmdWord >> (BYTE_SIZE)) & BYTE_MASK;
        array[3] =  cmdWord & BYTE_MASK;

        hal_RfspiImmWrite(array, RDA6220_ONE_CMD_BYTE_QTY);
    }

    const HAL_RFSPI_CFG_T g_spiCfg =
    {
        TRUE,  // Enable
        TRUE, // csActiveHi
        TRUE,  // modeDigRfRead
        FALSE, // modeClockBack2Back
        FALSE, // inputEn
        FALSE, // clkFallEdge
        HAL_RFSPI_HALF_CLK_PERIOD_1, // clkDelay
        HAL_RFSPI_HALF_CLK_PERIOD_2, // doDelay
        HAL_RFSPI_HALF_CLK_PERIOD_3, // diDelay
        HAL_RFSPI_HALF_CLK_PERIOD_2, // csDelay
        HAL_RFSPI_HALF_CLK_PERIOD_0, // csEndHold
        HAL_RFSPI_HALF_CLK_PERIOD_3, // csEndPulse
        2,  // turnAroundCycles
        27, // frameSize
        17, // inputFrameSize
    };
#define LOOP_DELAY(count)       for (int n = 0; n < (count); n++) asm volatile ("nop")

#define _TEST_LCK_SYS_   HAL_SYS_FREQ_156M
#define _TEST_DDR_CLK_OFFSET_   2
    extern UINT16  g_ddrIndex ;

    PRIVATE VOID ddr_setController(int clk,int dqs)
    {
        LOOP_DELAY(16);
        hwp_psram8Ctrl->clk_ctrl = clk;
        hwp_psram8Ctrl->dqs_ctrl = (dqs & 0xffff) | (clk << 16);
        LOOP_DELAY(16);

        UINT32 ctrl_time = hwp_psram8Ctrl->ctrl_time;
        ctrl_time &= ~PSRAM8_CTRL_WL(0x3f);
        hwp_psram8Ctrl->ctrl_time = ctrl_time;
    }

    PROTECTED int main(VOID)
    {
        UINT32 bcpuStackBase;

        INT32                                i = 0,k;
        UINT32                               cpt,ddrclk;
        UINT32                               cpt2;
        INT32                                status = MEMD_ERR_NO;
        BOOL                                 ddr_test;
        HAL_DDR_TIMING_T timing;
        DDR_TIMING_T  ddrConfig;
		
        hal_SwRequestClk(FOURCC_TOOL, _TEST_LCK_SYS_);
        pmd_Open(tgt_GetPmdConfig());
        pmd_SetPowerMode(PMD_IDLEPOWER);
        hal_TimWatchDogClose();
        mon_Event(0xbe900000);
        //mon_Event((UINT32)&g_ate_info[0]);
        for(i=0; i<_ATE_INFO_LEN_; i++)
        {
            g_ate_info[i]=0;
        }

        g_ate_info[_ATE_INFO_LEN_-1]= 0xf0000000;
        g_ate_info[6]=0x12345678;

        //g_ate_info[15]= hal_TimGetUpTime();//*(volatile UINT32 *)(0x81c0000c)=hal_TimGetUpTime();
        //mon_Event(0x80000000|hal_TimGetUpTime());

//  enable exception (gdb breaks) but not external it, keep handler in rom
        asm volatile("li    $27,    (0x00400000 | 0x0001 | 0xc000)\n\t"
                     "mtc0  $0, $13\n\t"
                     "nop\n\t"
                     "mtc0  $27, $12\n\t"
//  k1 is status for os irq scheme
                     "li    $27, 1");

        // memd_FlashOpen(tgt_GetMemdFlashConfig());

        // uart_test_fun();
        // mon_Event(0xaaaacc01);
        // BCPU stack start at the end of SRAM
        // bcpuStackBase = CACHED (((UINT32)0x81983260 - 4));
        //bcpuStackBase = CACHED (((UINT32)&_bb_sram_romed_globals_start - 4));

        // Handshake process to be sure BCPU is well started
        //hal_SysStartBcp((VOID*)bcpu_flash, (VOID*)bcpuStackBase);

        // Record the communication structure into HAL Mapping Engine
        // hal_MapEngineRegisterModule(HAL_MAP_ID_FLASH_PROG, &g_flashProgVersion, &g_flashProgDescr);
        g_dataBuffer_start=(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM);//0xa2000000
        //mon_Event(0xfbbbbccc);mon_Event(g_dataBuffer_start);

        psram_check_size =(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM) | ERamSize_Valid_Check((UINT16*)hal_EbcGetCsAddress(HAL_EBC_SRAM));//0xa2400000
        //mon_Event(0x3bbbbcc5);
        //mon_Event(psram_check_size);

        g_ate_info[14]= psram_check_size;//*(volatile UINT32 *)(0x81c00014)=psram_check_size;
        //*(volatile UINT32 *)(0x81c0001c)=g_dataBuffer_start;


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
                    g_ate_info[6]=0x11223345;
                    mon_Event(0xf000bad4);
                    //while(1);
                }
            }
            *(volatile UINT32 *)i=0x5555aaaa;
        }

        if(psram_check_size <= g_dataBuffer_start)
        {
            g_ate_info[6]=0x11112222;
            SEND_EVENT(psram_check_size);
            SEND_EVENT(g_dataBuffer_start);
            mon_Event(0xf000bad2);
            //while(1);
        }

        g_dataBuffer_start=(g_dataBuffer_start&(~0xa0000000))|0x80000000;

        if(psram_check_size>((UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM)|0x00400000))
        {
            psram_check_size=(psram_check_size&(~0xa0000000))|0x80000000;

            if(!fp_CheckDataBuffer_HIGH_ADDR())
            {
                g_ate_info[6]= 0x12366666;//*(volatile UINT32 *)(0xa1c00000)=0x12366666;
                mon_Event(0xf000bad1);
                //while(1);
            }
            psram_check_size=(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM)|0x00400000;
        }

        psram_check_size=(psram_check_size&(~0xa0000000))|0x80000000;

        if(*(volatile UINT32 *)(0x81c00060)==0x12345678)
        {
            if((*(volatile UINT32 *)0x81c00068 & 0xff00ffff) == 0)
                psram_check_size=(*(volatile UINT32 *)(0x81c00068)&(~0xa0000000))|0x80000000;
        }
        g_ate_info[11] = psram_check_size;//*(volatile UINT32 *)(0xa1c00018)=psram_check_size;
        //*(volatile UINT32 *)(0xa1c00000)=0x55667788;
        //  *(volatile UINT32 *)(0xa1c00010)=hal_TimGetUpTime();

        if(g_ate_info[6] == 0x12345678)
        {
            g_ate_info[7]=0xfaaaaaad;// *(volatile UINT32 *)(0xa1c01004)=0xfaaaaaad;   // psram ok
        }
        else
        {
            mon_Event(0xbad0000d); mon_Event(g_ate_info[6]);

        }

        //mon_Event(0xfdddccc1); mon_Event(g_ddrIndex);

        memd_GetCfgDDRIndex(&ddrConfig);
        ddrclk = ddrConfig.ddr.ddr_clk_ctrl;


        ddr_test = TRUE;
        ddrConfig.ddr.ddr_clk_ctrl -= _TEST_DDR_CLK_OFFSET_;
        for( k = 0; k<2; k++)
        {
            //hal_TimDelay(16384);
            ddr_setController(ddrConfig.ddr.ddr_clk_ctrl,ddrConfig.ddr.ddr_dqs_ctrl);

            if(FALSE == ddr_CheckPsram())
            {
                ddr_test = FALSE;
                mon_Event(0xbad0dd21);
                mon_Event(k);
                break;
            }
            ddrConfig.ddr.ddr_clk_ctrl += _TEST_DDR_CLK_OFFSET_ *2;

        }
        if((ddr_test == TRUE) &&( g_ate_info[7]== 0xfaaaaaad))
        {
            g_ate_info[7]=0xaaaaaaad;   // psram ok
        }
        g_ate_info[18]= 0x12345678;//*(volatile UINT32 *)(0xa1c01000)=0x12345678;//
        g_ate_info[9]=0x12345678;//*(volatile UINT32 *)(0xa1c01010)=0x12345678;//
        g_ate_info[10]=0x12345678;//*(volatile UINT32 *)(0xa1c01020)=0x12345678;//

        ddr_test = TRUE;

        ddrConfig.ddr.ddr_clk_ctrl = ddrclk;

        ddrConfig.ddr.ddr_clk_ctrl -= _TEST_DDR_CLK_OFFSET_;
        for( k = 0; k<2; k++)
        {
            ddr_setController(ddrConfig.ddr.ddr_clk_ctrl,ddrConfig.ddr.ddr_dqs_ctrl);
            if(FALSE == ddr_dma_block_test(0xa2001000,0xa2001000+0x1000))
            {
                ddr_test = FALSE;
                mon_Event(0xbad0dd22);
                mon_Event(k);
                break;
            }
            ddrConfig.ddr.ddr_clk_ctrl += _TEST_DDR_CLK_OFFSET_ *2;

        }

        if(ddr_test == TRUE)
        {
            g_ate_info[12]= 0xaaaaaaae;//*(volatile UINT32 *)(0xa1c01008)=0xaaaaaaae;   // psram dma ok
        }



        //mon_Event(0xa000bad4);
#ifdef PSRAM_NEW_TEST_EN
        cpt = fs_psram_test(g_dataBuffer_start,psram_check_size);
        if(cpt == 0)
        {
            mon_Event(0x66666666);
            mon_Event(psram_check_size);
            mon_Event(g_dataBuffer_start);
            mon_Event(0xdddddddd);
        }
        else
        {
            mon_Event(0x99999999);
            mon_Event(cpt);
            mon_Event(psram_check_size);
            mon_Event(g_dataBuffer_start);
            mon_Event(0xeeeeeeee);
            // pass prev test but fail here
            if( g_ate_info[12] == 0xaaaaaaae)
            {
                g_ate_info[12]=0xeeeeeeee;   // psram dma ok
            }
            if(g_ate_info[7] == 0xaaaaaaad)
            {
                g_ate_info[7]=0xdddddddd;   // psram ok
            }
        }

#endif  // PSRAM_NEW_TEST_EN

        voc_test();

        if(g_ate_info[6]==0xAAAAAAA9)
        {
            g_ate_info[6]=0xAAAAAAAA;
        }
        else
        {
            //  while (1);
        }

        uart_test_fun();
        //if(*(volatile UINT32 *)(0xa1c00000)==0xAAAAAAAc)
        {
            g_ate_info[6]=0xAAAAAAAA;
        }

        //*(volatile UINT32 *)(0x81c02000)=0xedededed;//
        if(g_ate_info[18] == 0x12345678)
        {
            *(volatile UINT32 *)(0xa1c01000) = 0xaaaaaaaa;   // voc ok
        }
        if( g_ate_info[10] == 0x12345678)
        {
            *(volatile UINT32 *)(0xa1c01020) = 0xaaaaaaac;   // uart ok
        }
        test_gouda_merge();
        if( g_ate_info[9] == 0x12345678)
        {
            *(volatile UINT32 *)(0xa1c01010) = 0xaaaaaaab;   // gouda ok
        }

        test_flash( );


        *(volatile UINT32 *)(0x81c00014)=g_ate_info[14];
        *(volatile UINT32 *)(0xa1c01008)= g_ate_info[12];
        *(volatile UINT32 *)(0xa1c00018)= g_ate_info[11];
        *(volatile UINT32 *)(0xa1c01004) = g_ate_info[7];

        *(volatile UINT32 *)(0x81c0001c)=g_dataBuffer_start;
        mon_Event(0xa1c01000);
        mon_Event( *(volatile UINT32 *)(0xa1c01000)); // voc ok
        mon_Event(0xa1c01010);
        mon_Event( *(volatile UINT32 *)(0xa1c01010));//gouda ok
        mon_Event(0xa1c01020);
        mon_Event( *(volatile UINT32 *)(0xa1c01020)); // uart ok
        mon_Event(0xa1c01004);
        mon_Event( *(volatile UINT32 *)(0xa1c01004)); //ram ok
        mon_Event(0xa1c01008);
        mon_Event( *(volatile UINT32 *)(0xa1c01008)); //ram dma ok


        mon_Event(0xa1b00010);
        *(volatile UINT32 *)(0xa1b00004)=g_spiflash_ID.manufacturerID;
        *(volatile UINT32 *)(0xa1b00008)=g_spiflash_ID.device_memory_type_ID;
        *(volatile UINT32 *)(0xa1b0000c)=g_spiflash_ID.device_capacity_type_ID;

        if(g_ate_info[8] != 0xAAAAAAAA )
        {
            *(volatile UINT32 *)(0xa1b00010) = 0xbadbad00;
			 mon_Event( g_ate_info[8]); // flash ok
        }

        mon_Event( *(volatile UINT32 *)(0xa1b00010)); // flash ok
        //mon_Event(g_ate_info[_ATE_INFO_LEN_-1]);

        mon_Event(0xffffffed);
        while (1) {};
    }
    void testate_end(void)
    {
    }

