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

#ifndef _BIST_ASM_H_
#define _BIST_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'bist'."
#endif



//==============================================================================
// bist
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_SYS_BIST_BASE           0x01A1F000
#define REG_BB_BIST_BASE            0x01909000

#define REG_SYS_BIST_BASE_HI        BASE_HI(REG_SYS_BIST_BASE)
#define REG_SYS_BIST_BASE_LO        BASE_LO(REG_SYS_BIST_BASE)

#define REG_SYS_BIST_CONTROL        REG_SYS_BIST_BASE_LO + 0x00000000
#define REG_SYS_BIST_STATUS         REG_SYS_BIST_BASE_LO + 0x00000004
#define REG_SYS_BIST_MEM_ADDR       REG_SYS_BIST_BASE_LO + 0x00000008
#define REG_SYS_BIST_MEM_SIZE       REG_SYS_BIST_BASE_LO + 0x0000000C
#define REG_SYS_BIST_DATA_MASK      REG_SYS_BIST_BASE_LO + 0x00000010

#define REG_BB_BIST_BASE_HI         BASE_HI(REG_BB_BIST_BASE)
#define REG_BB_BIST_BASE_LO         BASE_LO(REG_BB_BIST_BASE)

#define REG_BB_BIST_CONTROL         REG_BB_BIST_BASE_LO + 0x00000000
#define REG_BB_BIST_STATUS          REG_BB_BIST_BASE_LO + 0x00000004
#define REG_BB_BIST_MEM_ADDR        REG_BB_BIST_BASE_LO + 0x00000008
#define REG_BB_BIST_MEM_SIZE        REG_BB_BIST_BASE_LO + 0x0000000C
#define REG_BB_BIST_DATA_MASK       REG_BB_BIST_BASE_LO + 0x00000010

//control
#define BIST_START                  (1<<0)

//status
#define BIST_ENABLE                 (1<<0)
#define BIST_FAIL                   (1<<4)

//mem_addr
#define BIST_MEM_ADDR(n)            (((n)&0x3FFFFFF)<<0)

//mem_size
#define BIST_MEM_SIZE(n)            (((n)&0xFFFF)<<0)
#define BIST_MEM_WIDTH(n)           (((n)&3)<<24)
#define BIST_MEM_WIDTH_MASK         (3<<24)
#define BIST_MEM_WIDTH_SHIFT        (24)
#define BIST_MEM_WIDTH_MEM_WIDTH_DEFAULT (0<<24)
#define BIST_MEM_WIDTH_MEM_WIDTH_8  (1<<24)
#define BIST_MEM_WIDTH_MEM_WIDTH_16 (2<<24)
#define BIST_MEM_WIDTH_MEM_WIDTH_32 (3<<24)

//data_mask
#define BIST_DATA_MASK(n)           (((n)&0xFFFFFFFF)<<0)




#endif
