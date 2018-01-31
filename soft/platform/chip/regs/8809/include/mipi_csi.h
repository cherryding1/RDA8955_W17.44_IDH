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


#ifndef _CAMERA_MIPI_CSI_H_
#define _CAMERA_MIPI_CSI_H_





// =============================================================================
//  MACROS
// =============================================================================
// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// CAMERA_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_MIPI_CSI_BASE             0x01A18050

//#define REG_MIPI_CSI_CLK_BASE             0x01A18080


typedef volatile struct
{
    REG32       CSI_CONFIG_REG0;                         //0x00000000
    REG32       CSI_CONFIG_REG1;                       //0x00000004
    REG32       CSI_CONFIG_REG2;                         //0x00000008
    REG32       CSI_CONFIG_REG3;                     //0x0000000C
    REG32       CSI_OBSERVE_REG4;                    //0x00000010
    REG32       CSI_OBSERVE_REG5;                    //0x00000014
    REG32       CSI_OBSERVE_REG6;                      //0x00000018  read only
    REG32       CSI_OBSERVE_REG7;                      //0x0000001C   read only
    REG32       Reserved_00000068[4];                      //0x00000050
    REG32       CSI_OBSERVE_CLK;                        //0x30

} HWP_MIPI_CSI_T;

#define hwp_mipi_csi                 ((HWP_MIPI_CSI_T*) KSEG1(REG_MIPI_CSI_BASE))



// The camera interface specifaction defines an interface between a perihperal device carmera and a host processor.

/*************************************************************/
/***************** CAMERA CSI CONFIGURATION REGS********************/
/*************************************************************/

//--------------------------------
// csi_config_reg0
//--------------------------------

#define CSI_NUM_D_TERM_EN(n)          (((n)&0XFF)<<0)
#define CSI_CUR_FRAME_LINE_NUM(n)          (((n)&0X3FF)<<8)
#define CSI_VC_ID_SET(n)          (((n)&3)<<18)
#define CSI_DATA_LP_IN_CHOOSE(n)          (((n)&3)<<20)
#define CSI_DATA_LP_INV           (1<<22)
#define CSI_CLK_LP_INV           (1<<23)
#define CSI_TRAIL_DATA_WRONG_CHOOSE    (1<<24)
#define CSI_SYNC_BYPASS       (1<<25)
#define CSI_RDATA_BIT_INV_EN               (1<<26)
#define CSI_HS_SYNC_FIND_EN        (1<<27)
#define CSI_LINE_PACKET_ENABLE       (1<<28)
#define CSI_ECC_BYPASS                 (1<<29)
#define CSI_DATA_LANE_CHOOSE            (1<<30)
#define CSI_MODULE_ENABLE          (1<<31)



//--------------------------------
// csi_config_reg01
//--------------------------------

#define CSI_NUM_HS_SETTLE(n)          (((n)&0XFF)<<0)
#define CSI_LP_DATA_LENGTH_CHOSSE(n)          (((n)&7)<<8)
#define CSI_DATA_CLK_LP_POSEEDGE_CHOSSE(n)          (((n)&7)<<11)
#define CSI_CLK_LP_CK_INV        (1<<14)
#define CSI_RCLR_MASK_EN        (1<<15)
#define CSI_RINC_MASK_EN        (1<<16)
#define CSI_HS_ENALBE_MASK_EN        (1<<17)
#define CSI_DEN_INV_BIT        (1<<18)
#define CSI_HSYNC_INV_BIT        (1<<19)
#define CSI_VSYNC_INV_BIT        (1<<20)
#define CSI_HS_DATA2_ENABLE_REG  (1<<21)

#define CSI_HS_TATA1_ENALBE_REG   (1<<22)
#define CSI_HS_DATA1_ENABLE_CHOSSE           (1<<23)
#define CSI_HS_DATA1_ENALBE_DRE    (1<<24)
#define CSI_DATA2_TER_ENABLE_REG         (1<<25)
#define CSI_DATAL_TER_ENABLE_REG           (1<<26)
#define CSI_DATAL_TER_ENABLE_DR    (1<<27)
#define CSI_LP_DATA_INTERRUPT_CLR  (1<<28)
#define CSI_LP_CMD_INTERRUPT_CLR       (1<<29)
#define CSI_LP_DATA_CLR            (1<<30)
#define CSI_LP_CMD_CLR          (1<<31)


//--------------------------------
// csi_config_reg02
//--------------------------------

#define CSI_NUM_HS_CLK_USEFUL(n)          (((n)&0XFFFF)<<0)
#define CSI_NUM_HS_SETTLE_CLK(n)          (((n)&0XFF)<<16)
#define CSI_NUM_C_TERM_EN(n)          (((n)&0xff)<<24)


//--------------------------------
// csi_config_reg03
//--------------------------------

#define CSI_DLY_SEL_REG(n)          (((n)&3)<<0)
#define CSI_DLY_SEL_DATA2_REG(n)          (((n)&3)<<2)
#define CSI_DLY_SEL_DATA1_REG(n)          (((n)&3)<<4)
#define CSI_CLK_LP_IN_CHOOSE_BIT(n)          (((n)&3)<<6)
#define CSI_PD_LPRX_REG        (1<<8)
#define CSI_PD_HSRX_REG        (1<<9)
#define CSI_PD_DR        (1<<10)
#define CSI_AVDD1V8_2V8_SEL_REG        (1<<11)
#define CSI_HS_CLK_ENABLE_REG        (1<<12)
#define CSI_HS_CLK_ENABLE_CHOOSE_BIT        (1<<13)
#define CSI_HS_CLK_ENALBE_DR        (1<<14)
#define CSI_CLK_TERMINAL_ENABLE_REG        (1<<15)
#define CSI_CLK_TERMINAL_ENABLE_DR        (1<<16)



//--------------------------------
// csi_config_reg04
//--------------------------------

#define CSI_HS_DATA_STATE(n)          (((n)&0x3fff)<<0)
#define CSI_PHY_DATA_STATE(n)          (((n)&0x7fff)<<14)


//--------------------------------
// csi_config_reg05
//--------------------------------

#define CSI_LP_CMD_OUT(n)          (((n)&0XFF)<<0)
#define CSI_PHY_CLK_STATE(n)          (((n)&0X1FF)<<10)
#define CSI_HS_DATA_ERROR_FLAG        (1<<28)
#define CSI_ERR_ECC_CORRECTED_FLAG        (1<<29)
#define CSI_ERR_DATA_CORRECTED_FLAG        (1<<30)
#define CSI_ERR_DATA_ZERO_FLAG        (1<<31)


/************************************************
YUV data output format
        3'b000: Y0-U0-Y1-V0                3'b001: Y0-V0-Y1-U0
        3'b010: U0-Y0-V0-Y1                3'b011: U0-Y1-V0-Y0
        3'b100: V0-Y1-U0-Y0                3'b101: V0-Y0-U0-Y1
        3'b110: Y1-V0-Y0-U0                3'b111: Y1-U0-Y0-V0
*************************************************/


//--------------------------------
// csi_observe_reg6 (read only)
//--------------------------------

//--------------------------------
// csi_observe_reg6 (read only)
//--------------------------------


#endif

