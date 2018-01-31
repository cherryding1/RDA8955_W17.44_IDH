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
#include "voc_ahb.h"
#include "voc_cfg.h"
#include "voc_ram.h"
#include "sys_ctrl.h"
#include "hal_voc.h"


//#define MREAD_WORD(addr) *((volatile int *)(addr))
//#define MWRITE_WORD(addr,data) *((volatile int *)(addr)) = data
#define VOC_AHB_DMA_LADDR_MASK      (0xFFFF<<1)
//#define VOC_AHB_DMA_EADDR_MASK      (0x3FFFFFF<<2)

//#define reftest


#define CHECK_VOC_CFG_SEMA 0

#define toVocLAddr(n)    (((INT32)(n)>>1) & VOC_AHB_DMA_LADDR_MASK)
#define toVocEAddr(n)    (((INT32)(n))    & VOC_AHB_DMA_EADDR_MASK)

//#define SYS_CTRL_PROTECT_LOCK                    (0xA50000)
//#define SYS_CTRL_PROTECT_UNLOCK                  (0xA50001)
#define SYS_CTRL_CLR_RST_VOC        			 (1<<0)
//#define SYS_CTRL_VOC_SEL_PLL_PLL    			 (0<<8)
#define VOC_AHB_DMA_DIR_READ        (0<<20)
#define VOC_AHB_DMA_DIR_WRITE       (1<<20)

void hal_VocOpenhx ()
{
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->BB_Rst_Clr = SYS_CTRL_CLR_RST_VOC;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;

    //hwp_sysCtrl->Cfg_Clk_Voc = SYS_CTRL_VOC_SEL_PLL_PLL | (4<<0);//SYS_CTRL_VOC_FREQ_78M=(4<<0)
    hwp_vocAhb->Irq_Status = VOC_AHB_XCPU_DMA_IRQ_STATUS;

    // Clear the interrupt mask and clear all XCPU cause
    hwp_vocAhb->Irq_Mask_Clr = VOC_AHB_XCPU_IRQ_MASK_MASK;
    hwp_vocAhb->Irq_Status = VOC_AHB_XCPU_IRQ_STATUS_MASK;
    hwp_vocAhb->Irq_Mask_Set = VOC_AHB_XCPU_IRQ_MASK_MASK;
    //hwp_vocAhb->DMA_Addr_Offset= 0x10;
//	hwp_vocAhb->DMA_Addr_Offset= 0x0;

    return;
}



void* hal_VocGetPointer_test(INT32 vocLocalAddr)
{
    // return an global pointer to vocLocalAddr
    return ((void*)(&(hwp_vocRam->voc_ram_x_base)[vocLocalAddr*2]));
}


INT32 * hal_VocGetDmaiPointer_tset(INT32 * vocExternAddr, HAL_VOC_DMA_DIR_T wr1Rd0, HAL_VOC_DMA_TRANSFER_T sngl1brst0, HAL_VOC_DMA_B2S_T b2s)
{
    if (NULL == vocExternAddr)
    {
        // protect (do not create write NULL pointers)
        return NULL;
    }
    else
    {
        INT32 DmaiEAddr = ((INT32)vocExternAddr) & VOC_CFG_DMA_EADDR_MASK;

        // configure the single/burst parameter
        if (sngl1brst0 == HAL_VOC_DMA_SINGLE)
        {
            DmaiEAddr |= VOC_CFG_DMA_SINGLE;
        }

        // configure the read/write parameter
        if (wr1Rd0 == HAL_VOC_DMA_WRITE)
        {
            DmaiEAddr |= VOC_CFG_DMA_WRITE_WRITE;
        }

        // return an DMAI configuration pointer for the hwp_vocAhb->DMA_EAddr
        return (INT32 *)DmaiEAddr;
    }
}



BOOL hal_VocDmaDone_tset(void)
{
    return ((hwp_vocAhb->Irq_Status & VOC_AHB_XCPU_DMA_IRQ_STATUS) ? TRUE : FALSE);
}




////////////////////////voc reference test//////////////////////////////

#define DMA_BURST                     (0 << 31)
#define DMA_WRITE                     (1 << 30)
#define DMA_READ                      (0 << 30)



int testTable []=
{
    0x7EC80347, 0xC4427881, 0x7D166BCE, 0x761B7043,
    0x46386985, 0x341B4CDB, 0x552E2787, 0xF5C76157,
    0x38A04277, 0x0186B589, 0x13554DF7, 0x77238788,
    0x1DBC4F32, 0xC64FC530, 0x20664054, 0x331B939D,
    0xFD89900C, 0x5982CBB4, 0x2680A111, 0xA4DD789A,
    0x21464799, 0xD66D5A3B, 0x0CFE4FE3, 0x83D565C4,
    0x136EF6AF, 0x949C4129, 0xFC7E6DC1, 0xC7AFCBA1,
    0x93259CBE, 0x5BDF2DDE, 0x5CF45FDE, 0x7A08AB5B
};


const int voc2_nonreg_tab[] = {
#include "ref_test.tab"
};

int test_out[1000];


void hal_VocStartDmahx_ref (UINT32* codeaddr)
{
    hwp_vocAhb->Irq_Status = VOC_AHB_XCPU_DMA_IRQ_STATUS;

    // load code in RAM_I
    hwp_vocAhb->DMA_LAddr = toVocLAddr(hwp_vocRam->voc_ram_i_base);
    hwp_vocAhb->DMA_EAddr = toVocEAddr(codeaddr);
    hwp_vocAhb->DMA_Cfg   = VOC_AHB_DMA_RUN | VOC_AHB_DMA_SIZE(12000);

    while (!(hwp_vocAhb->Irq_Status & VOC_AHB_XCPU_DMA_IRQ_STATUS))
    {
    }

    return;
}

void hal_VocWakeuphx_ref()
{
    INT32* addr_tmp;

    // set the PC start address
    hwp_vocCfg->PC   = 0;

    // set the 16-bit stack to the top of RAM_X
    hwp_vocCfg->SP16 = VOC_CFG_SP16(toVocLAddr(&(hwp_vocRam->voc_ram_x_base[VOC_RAM_X_SIZE])) - 1);

    // set the 32-bit stack to the top of RAM_Y
    hwp_vocCfg->SP32 = VOC_CFG_SP32(toVocLAddr(&(hwp_vocRam->voc_ram_y_base[VOC_RAM_Y_SIZE])) - 2);

    //cfg external address
    addr_tmp = hal_VocGetDmaiPointer_tset((INT32 *)testTable, HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    hwp_vocCfg->ACC0 = (int)(addr_tmp);

    addr_tmp = hal_VocGetDmaiPointer_tset((INT32 *)test_out,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    hwp_vocCfg->ACC1 = (int)(addr_tmp);
	hal_DcacheFlushAll(TRUE);

    //start VOC
    hwp_vocCfg->Ctrl = VOC_CFG_RUN_START;

    return;
}


PUBLIC void voc_test()
{
    volatile INT32 int_cause;
    volatile INT32 reg_flag;

    hal_VocOpenhx();

    hal_VocStartDmahx_ref((UINT32*)voc2_nonreg_tab);
    hal_VocWakeuphx_ref();
	mon_Event(0x33330002);

    while(*(volatile UINT32 *)(0x81c00000)==0xaaaaaaa8)
    {
        int_cause = hwp_vocAhb->Irq_Status & (VOC_AHB_XCPU_VOC_IRQ_STATUS | VOC_AHB_BCPU_VOC_IRQ_STATUS);

        if(int_cause!=0)
        {
            reg_flag = hwp_vocCfg->REG45;

            if(reg_flag == 0xbad0bad0)
                *(volatile UINT32 *)(0x81c00000)=7;
            *(volatile UINT32 *)(0x81c00038)=hal_TimGetUpTime();
            mon_Event(0x8888);
            //while(1);
            *(volatile UINT32 *)(0x81c01000)=0xf0008001;//voc fail
        }
        else
        {
            reg_flag = hwp_vocCfg->RL7;
            if(reg_flag == 0xc001c001)
            {
                *(volatile UINT32 *)(0x81c00000)=0xaaaaaaa9;
                *(volatile UINT32 *)(0x81c00038)=hal_TimGetUpTime();
                mon_Event(0x9999);
            }
            else
            {
                reg_flag = hwp_vocCfg->REG45;

                if(reg_flag == 0xbad0bad0)
                {
                    *(volatile UINT32 *)(0x81c00000)=9;
                    *(volatile UINT32 *)(0x81c00038)=hal_TimGetUpTime();
                    mon_Event(0x7777);
                    //while(1);
                    *(volatile UINT32 *)(0x81c01000)=0xf0008001;//voc fail
                }
            }

        }
    }



}



