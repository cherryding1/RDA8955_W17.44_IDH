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

#ifndef _GPIO_ASM_H_
#define _GPIO_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'gpio'."
#endif

#include "8809_generic_config_asm.h"

#define IDX_GPIO_DCON                            (0)
#define IDX_GPO_CHG                              (0)

//==============================================================================
// gpio
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_GPIO_BASE               0x01A03000

#define REG_GPIO_BASE_HI            BASE_HI(REG_GPIO_BASE)
#define REG_GPIO_BASE_LO            BASE_LO(REG_GPIO_BASE)

#define REG_GPIO_GPIO_OEN_VAL       REG_GPIO_BASE_LO + 0x00000000
#define REG_GPIO_GPIO_OEN_SET_OUT   REG_GPIO_BASE_LO + 0x00000004
#define REG_GPIO_GPIO_OEN_SET_IN    REG_GPIO_BASE_LO + 0x00000008
#define REG_GPIO_GPIO_VAL           REG_GPIO_BASE_LO + 0x0000000C
#define REG_GPIO_GPIO_SET           REG_GPIO_BASE_LO + 0x00000010
#define REG_GPIO_GPIO_CLR           REG_GPIO_BASE_LO + 0x00000014
#define REG_GPIO_GPINT_CTRL_SET     REG_GPIO_BASE_LO + 0x00000018
#define REG_GPIO_GPINT_CTRL_CLR     REG_GPIO_BASE_LO + 0x0000001C
#define REG_GPIO_INT_CLR            REG_GPIO_BASE_LO + 0x00000020
#define REG_GPIO_INT_STATUS         REG_GPIO_BASE_LO + 0x00000024
#define REG_GPIO_CHG_CTRL           REG_GPIO_BASE_LO + 0x00000028
#define REG_GPIO_CHG_CMD            REG_GPIO_BASE_LO + 0x0000002C
#define REG_GPIO_GPO_SET            REG_GPIO_BASE_LO + 0x00000030
#define REG_GPIO_GPO_CLR            REG_GPIO_BASE_LO + 0x00000034

//gpio_oen_val
#define GPIO_OEN_VAL(n)             (((n)&0xFFFFFFFF)<<0)
#define GPIO_OEN_VAL_INPUT          (0x1<<0)
#define GPIO_OEN_VAL_OUTPUT         (0x0<<0)

//gpio_oen_set_out
#define GPIO_OEN_SET_OUT(n)         (((n)&0xFFFFFFFF)<<0)

//gpio_oen_set_in
#define GPIO_OEN_SET_IN(n)          (((n)&0xFFFFFFFF)<<0)

//gpio_val
#define GPIO_GPIO_VAL(n)            (((n)&0xFFFFFFFF)<<0)

//gpio_set
#define GPIO_GPIO_SET(n)            (((n)&0xFFFFFFFF)<<0)

//gpio_clr
#define GPIO_GPIO_CLR(n)            (((n)&0xFFFFFFFF)<<0)

//gpint_ctrl_set
#define GPIO_GPINT_R_SET(n)         (((n)&0xFF)<<0)
#define GPIO_GPINT_F_SET(n)         (((n)&0xFF)<<8)
#define GPIO_DBN_EN_SET(n)          (((n)&0xFF)<<16)
#define GPIO_GPINT_MODE_SET(n)      (((n)&0xFF)<<24)

//gpint_ctrl_clr
#define GPIO_GPINT_R_CLR(n)         (((n)&0xFF)<<0)
#define GPIO_GPINT_F_CLR(n)         (((n)&0xFF)<<8)
#define GPIO_DBN_EN_CLR(n)          (((n)&0xFF)<<16)
#define GPIO_GPINT_MODE_CLR(n)      (((n)&0xFF)<<24)

//int_clr
#define GPIO_GPINT_CLR(n)           (((n)&0xFF)<<0)

//int_status
#define GPIO_GPINT_STATUS(n)        (((n)&0xFF)<<0)
#define GPIO_GPINT_STATUS_MASK      (0xFF<<0)
#define GPIO_GPINT_STATUS_SHIFT     (0)

//chg_ctrl
#define GPIO_OUT_TIME(n)            (((n)&15)<<0)
#define GPIO_WAIT_TIME(n)           (((n)&0x3F)<<4)
#define GPIO_INT_MODE_L2H           (0<<16)
#define GPIO_INT_MODE_H2L           (1<<16)
#define GPIO_INT_MODE_RR            (3<<16)

//chg_cmd
#define GPIO_DCON_MODE_SET          (1<<0)
#define GPIO_CHG_MODE_SET           (1<<4)
#define GPIO_DCON_MODE_CLR          (1<<8)
#define GPIO_CHG_MODE_CLR           (1<<12)
#define GPIO_CHG_DOWN               (1<<24)

//gpo_set
#define GPIO_GPO_SET(n)             (((n)&0xFFF)<<0)

//gpo_clr
#define GPIO_GPO_CLR(n)             (((n)&0xFFF)<<0)




#endif
