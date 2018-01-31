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


#ifndef _SYS_IFC2_H_
#define _SYS_IFC2_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'SYS_IFC2'."
#endif


#include "globals.h"

// =============================================================================
//  MACROS
// =============================================================================
#define SYS_IFC2_ADDR_LEN                          (15)
#define SYS_IFC2_ADDR_ALIGN                        (2)
#define SYS_IFC2_TC_LEN                            (8)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SYS_IFC2_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_SYS_IFC2_BASE             0x01921000

typedef volatile struct
{
    /// The Channel 0 conveys data from the AIF to the memory.
    ///  The Channel 1 conveys data from the memory to the AIF.
    /// These Channels only exist with Voice Option.
    struct
    {
        REG32                      control;                      //0x00000000
        REG32                      status;                       //0x00000004
        REG32                      start_addr;                   //0x00000008
        REG32                      Fifo_Size;                    //0x0000000C
        REG32 Reserved_00000010;                //0x00000010
        REG32                      int_mask;                     //0x00000014
        REG32                      int_clear;                    //0x00000018
        REG32                      cur_ahb_addr;                 //0x0000001C
    } ch[2];
    REG32                          ch2_control;                  //0x00000040
    REG32                          ch2_status;                   //0x00000044
    REG32                          ch2_start_addr;               //0x00000048
    REG32                          ch2_end_addr;                 //0x0000004C
    REG32                          ch2_tc;                       //0x00000050
    REG32                          ch2_int_mask;                 //0x00000054
    REG32                          ch2_int_clear;                //0x00000058
    REG32                          ch2_cur_ahb_addr;             //0x0000005C
    REG32                          ch3_control;                  //0x00000060
    REG32                          ch3_status;                   //0x00000064
    REG32                          ch3_start_addr;               //0x00000068
    REG32 Reserved_0000006C;                    //0x0000006C
    REG32                          ch3_tc;                       //0x00000070
    REG32                          ch3_int_mask;                 //0x00000074
    REG32                          ch3_int_clear;                //0x00000078
    REG32                          ch3_cur_ahb_addr;             //0x0000007C
} HWP_SYS_IFC2_T;

#define hwp_sysIfc2                   ((HWP_SYS_IFC2_T*) KSEG1(REG_SYS_IFC2_BASE))


//control
#define SYS_IFC2_ENABLE               (1<<0)
#define SYS_IFC2_DISABLE              (1<<1)
#define SYS_IFC2_AUTO_DISABLE         (1<<4)

//status
//#define SYS_IFC2_ENABLE             (1<<0)
#define SYS_IFC2_FIFO_EMPTY           (1<<4)
#define SYS_IFC2_CAUSE_IEF            (1<<8)
#define SYS_IFC2_CAUSE_IHF            (1<<9)
#define SYS_IFC2_CAUSE_I4F            (1<<10)
#define SYS_IFC2_CAUSE_I3_4F          (1<<11)
#define SYS_IFC2_IEF                  (1<<16)
#define SYS_IFC2_IHF                  (1<<17)
#define SYS_IFC2_I4F                  (1<<18)
#define SYS_IFC2_I3_4F                (1<<19)

//start_addr
#define SYS_IFC2_START_ADDR(n)        (((n)&0xFFFFFF)<<2)

//Fifo_Size
#define SYS_IFC2_FIFO_SIZE(n)         (((n)&0x7FF)<<4)

//int_mask
#define SYS_IFC2_END_FIFO             (1<<8)
#define SYS_IFC2_HALF_FIFO            (1<<9)
#define SYS_IFC2_QUARTER_FIFO         (1<<10)
#define SYS_IFC2_THREE_QUARTER_FIFO   (1<<11)

//int_clear
//#define SYS_IFC2_END_FIFO           (1<<8)
//#define SYS_IFC2_HALF_FIFO          (1<<9)
//#define SYS_IFC2_QUARTER_FIFO       (1<<10)
//#define SYS_IFC2_THREE_QUARTER_FIFO (1<<11)

//cur_ahb_addr
#define SYS_IFC2_CUR_AHB_ADDR(n)      (((n)&0x3FFFFFF)<<0)

//ch2_control
//#define SYS_IFC2_ENABLE             (1<<0)
//#define SYS_IFC2_DISABLE            (1<<1)
#define SYS_IFC2_BURST_SIZE           (1<<16)
#define SYS_IFC2_FIFO_MODE            (1<<17)

//ch2_status
//#define SYS_IFC2_ENABLE             (1<<0)
#define SYS_IFC2_ENABLE_DISABLE       (0<<0)
#define SYS_IFC2_ENABLE_ENABLE        (1<<0)
//#define SYS_IFC2_FIFO_EMPTY         (1<<4)
#define SYS_IFC2_CAUSE_ITC            (1<<8)
//#define SYS_IFC2_CAUSE_IEF          (1<<9)
#define SYS_IFC2_CAUSE_IHTC           (1<<10)
#define SYS_IFC2_ITC                  (1<<12)
//#define SYS_IFC2_IEF                (1<<13)
#define SYS_IFC2_IHTC                 (1<<14)
#define SYS_IFC2_CUR_TC(n)            (((n)&0xFF)<<16)

//ch2_start_addr
//#define SYS_IFC2_START_ADDR(n)      (((n)&0x1FFF)<<2)
#define SYS_IFC2_PAGE_ADDR(n)         (((n)&0x7FF)<<15)

//ch2_end_addr
#define SYS_IFC2_END_ADDR(n)          (((n)&0x1FFF)<<2)
//#define SYS_IFC2_PAGE_ADDR(n)       (((n)&0x7FF)<<15)

//ch2_tc
#define SYS_IFC2_TC(n)                (((n)&0xFF)<<0)

//ch2_int_mask
#define SYS_IFC2_END_TC               (1<<0)
//#define SYS_IFC2_END_FIFO           (1<<1)
#define SYS_IFC2_HALF_TC              (1<<2)

//ch2_int_clear
//#define SYS_IFC2_END_TC             (1<<0)
//#define SYS_IFC2_END_FIFO           (1<<1)
//#define SYS_IFC2_HALF_FIFO          (1<<2)

//ch2_cur_ahb_addr
//#define SYS_IFC2_CUR_AHB_ADDR(n)    (((n)&0x3FFFFFF)<<0)

//ch3_control
//#define SYS_IFC2_ENABLE             (1<<0)
//#define SYS_IFC2_DISABLE            (1<<1)

//ch3_status
//#define SYS_IFC2_ENABLE             (1<<0)
//#define SYS_IFC2_ENABLE_DISABLE     (0<<0)
//#define SYS_IFC2_ENABLE_ENABLE      (1<<0)
//#define SYS_IFC2_FIFO_EMPTY         (1<<4)
//#define SYS_IFC2_CAUSE_ITC          (1<<8)
//#define SYS_IFC2_ITC                (1<<12)
//#define SYS_IFC2_CUR_TC(n)          (((n)&0xFF)<<16)

//ch3_start_addr
//#define SYS_IFC2_START_ADDR(n)      (((n)&0x1FFF)<<2)

//ch3_tc
//#define SYS_IFC2_TC(n)              (((n)&0xFF)<<0)

//ch3_int_mask
//#define SYS_IFC2_END_TC             (1<<0)

//ch3_int_clear
//#define SYS_IFC2_END_TC             (1<<0)

//ch3_cur_ahb_addr
//#define SYS_IFC2_CUR_AHB_ADDR(n)    (((n)&0x3FFFFFF)<<0)





#endif

