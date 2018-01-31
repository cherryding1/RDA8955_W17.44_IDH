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


#ifndef _CAMERA_H_
#define _CAMERA_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'camera'."
#endif



// =============================================================================
//  MACROS
// =============================================================================
#define FIFORAM_SIZE                             (80)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// CAMERA_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_CAMERA_BASE             0x01A18000

typedef volatile struct
{
    REG32       CTRL;                         //0x00000000
    REG32       STATUS;                       //0x00000004
    REG32       DATA;                         //0x00000008
    REG32       IRQ_MASK;                     //0x0000000C
    REG32       IRQ_CLEAR;                    //0x00000010
    REG32       IRQ_CAUSE;                    //0x00000014
    REG32       CMD_SET;                      //0x00000018
    REG32       CMD_CLR;                      //0x0000001C
    REG32       DSTWINCOL;                    //0x00000020
    REG32       DSTWINROW;                    //0x00000024
    REG32       SCALE_COFIG;                 //0x00000028
    REG32       CAM_SPI_REG_0;                      // 0x2c
    REG32       CAM_SPI_REG_1;                      // 0x30
    REG32       CAM_SPI_REG_2;                      // 0x34
    REG32       CAM_SPI_REG_3;                      // 0x38
    REG32       CAM_SPI_REG_4;                      // 0x3c
    REG32       CAM_SPI_REG_5;                      // 0x40
    REG32       CAM_SPI_REG_6;                      // 0x44
    REG32       CAM_SPI_OBSERVE_REG_0;    // 0x48, read only
    REG32       CAM_SPI_OBSERVE_REG_1;    // 0x4c, read only
    REG32 Reserved_00000068[108];             //0x00000050
    struct
    {
        REG32                      RAMData;                      //0x00000200
    } FIFORAM[FIFORAM_SIZE];
} HWP_CAMERA_T;

#define hwp_camera                  ((HWP_CAMERA_T*) KSEG1(REG_CAMERA_BASE))


//CTRL
#define CAMERA_ENABLE               (1<<0)
#define CAMERA_ENABLE_ENABLE        (1<<0)
#define CAMERA_ENABLE_DISABLE       (0<<0)
#define CAMERA_1_BUFENABLE               (1<<1)
#define CAMERA_DCTENABLE_1_BUFENABLE_ENABLE        (1<<1)
#define CAMERA_DCTENABLE_1_BUFENABLE_DISABLE       (0<<1)
#define CAMERA_DATAFORMAT(n)        (((n)&3)<<4)
#define CAMERA_DATAFORMAT_RGB565    (0<<4)
#define CAMERA_DATAFORMAT_YUV422    (1<<4)
#define CAMERA_DATAFORMAT_JPEG      (2<<4)
#define CAMERA_DATAFORMAT_RESERVE   (3<<4)
#define CAMERA_RESET_POL            (1<<8)
#define CAMERA_RESET_POL_INVERT     (1<<8)
#define CAMERA_RESET_POL_NORMAL     (0<<8)
#define CAMERA_PWDN_POL             (1<<9)
#define CAMERA_PWDN_POL_INVERT      (1<<9)
#define CAMERA_PWDN_POL_NORMAL      (0<<9)
#define CAMERA_VSYNC_POL            (1<<10)
#define CAMERA_VSYNC_POL_INVERT     (1<<10)
#define CAMERA_VSYNC_POL_NORMAL     (0<<10)
#define CAMERA_HREF_POL             (1<<11)
#define CAMERA_HREF_POL_INVERT      (1<<11)
#define CAMERA_HREF_POL_NORMAL      (0<<11)
#define CAMERA_PIXCLK_POL           (1<<12)
#define CAMERA_PIXCLK_POL_INVERT    (1<<12)
#define CAMERA_PIXCLK_POL_NORMAL    (0<<12)
#define CAMERA_VSYNC_DROP           (1<<14)
#define CAMERA_VSYNC_DROP_DROP      (1<<14)
#define CAMERA_VSYNC_DROP_NORMAL    (0<<14)
#define CAMERA_DECIMFRM(n)          (((n)&3)<<16)
#define CAMERA_DECIMFRM_ORIGINAL    (0<<16)
#define CAMERA_DECIMFRM_DIV_2       (1<<16)
#define CAMERA_DECIMFRM_DIV_3       (2<<16)
#define CAMERA_DECIMFRM_DIV_4       (3<<16)
#define CAMERA_DECIMCOL(n)          (((n)&3)<<18)
#define CAMERA_DECIMCOL_ORIGINAL    (0<<18)
#define CAMERA_DECIMCOL_DIV_2       (1<<18)
#define CAMERA_DECIMCOL_DIV_3       (2<<18)
#define CAMERA_DECIMCOL_DIV_4       (3<<18)
#define CAMERA_DECIMROW(n)          (((n)&3)<<20)
#define CAMERA_DECIMROW_ORIGINAL    (0<<20)
#define CAMERA_DECIMROW_DIV_2       (1<<20)
#define CAMERA_DECIMROW_DIV_3       (2<<20)
#define CAMERA_DECIMROW_DIV_4       (3<<20)
#define CAMERA_REORDER(n)           (((n)&7)<<24)
#define CAMERA_CROPEN               (1<<28)
#define CAMERA_CROPEN_ENABLE        (1<<28)
#define CAMERA_CROPEN_DISABLE       (0<<28)
#define CAMERA_BIST_MODE            (1<<30)
#define CAMERA_BIST_MODE_BIST       (1<<30)
#define CAMERA_BIST_MODE_NORMAL     (0<<30)
#define CAMERA_TEST                 (1<<31)
#define CAMERA_TEST_TEST            (1<<31)
#define CAMERA_TEST_NORMAL          (0<<31)

//STATUS
#define CAMERA_OVFL                 (1<<0)
#define CAMERA_VSYNC_R              (1<<1)
#define CAMERA_VSYNC_F              (1<<2)
#define CAMERA_DMA_DONE             (1<<3)
#define CAMERA_FIFO_EMPTY           (1<<4)

//DATA
#define CAMERA_RX_DATA(n)           (((n)&0xFFFFFFFF)<<0)

//IRQ_MASK
//#define CAMERA_OVFL               (1<<0)
//#define CAMERA_VSYNC_R            (1<<1)
//#define CAMERA_VSYNC_F            (1<<2)
//#define CAMERA_DMA_DONE           (1<<3)

//IRQ_CLEAR
//#define CAMERA_OVFL               (1<<0)
//#define CAMERA_VSYNC_R            (1<<1)
//#define CAMERA_VSYNC_F            (1<<2)
//#define CAMERA_DMA_DONE           (1<<3)

//IRQ_CAUSE
//#define CAMERA_OVFL               (1<<0)
//#define CAMERA_VSYNC_R            (1<<1)
//#define CAMERA_VSYNC_F            (1<<2)
//#define CAMERA_DMA_DONE           (1<<3)

//CMD_SET
#define CAMERA_PWDN                 (1<<0)
#define CAMERA_RESET                (1<<4)
#define CAMERA_FIFO_RESET           (1<<8)

//CMD_CLR
//#define CAMERA_PWDN               (1<<0)
//#define CAMERA_RESET              (1<<4)

//DSTWINCOL
#define CAMERA_DSTWINCOLSTART(n)    (((n)&0xFFF)<<0)
#define CAMERA_DSTWINCOLEND(n)      (((n)&0xFFF)<<16)

//DSTWINROW
#define CAMERA_DSTWINROWSTART(n)    (((n)&0xFFF)<<0)
#define CAMERA_DSTWINROWEND(n)      (((n)&0xFFF)<<16)

// SCALE CONFIG
#define CAMERA_SCALE_EN            (1<<0)
#define CAMERA_SCALE_COL(n)      (((n)&0x3)<<8)
#define CAMERA_SCALE_ROW(n)     (((n)&0x3)<<16)

//RAMData
#define CAMERA_DATA(n)              (((n)&0xFFFFFFFF)<<0)
#define CAMERA_DATA_MASK            (0xFFFFFFFF<<0)
#define CAMERA_DATA_SHIFT           (0)

/*************************************************************/
/***************** SPI CAMERA CONFIGURATION ********************/
/*************************************************************/

// Here, spi master mode means the sensor works as the SPI master.
// master mode 1 means 1 data ouput with SSN, master mode 2 means the other master modes.

//--------------------------------
// camera_spi_reg_0
//--------------------------------
// the number of lines per frame
#define CAMERA_LINE_NUM_PER_FRAME(n)     (((n)&0x3ff)<<22)
// the number of words(32 bits) per line
#define CAMERA_BLOCK_NUM_PER_LINE(n)     (((n)&0x3ff)<<12)
// VSYNC high effective
#define CAMERA_VSYNC_INV        (1<<11)
// HREF low effective
#define CAMERA_HREF_INV           (1<<10)
// OVFL low effective
#define CAMERA_OVERFLOW_INV           (1<<9)
// little endian enable
#define CAMERA_BIG_END_DIS        (1<<8)
// module reset when ovfl enable
#define CAMERA_OVERFLOW_RST_EN           (1<<7)
// observe the overflow when vsync effective
#define CAMERA_OVERFLOW_OBSERVE_ONLY_VSYNC_LOW                 (1<<6)
// reset the module when overflow in vsync effective
#define CAMERA_OVERFLOW_RSTN_ONLY_VSYNC_LOW                  (1<<5)
/************************************************
YUV data output format
        3'b000: Y0-U0-Y1-V0                3'b001: Y0-V0-Y1-U0
        3'b010: U0-Y0-V0-Y1                3'b011: U0-Y1-V0-Y0
        3'b100: V0-Y1-U0-Y0                3'b101: V0-Y0-U0-Y1
        3'b110: Y1-V0-Y0-U0                3'b111: Y1-U0-Y0-V0
*************************************************/
#define CAMERA_YUV_OUT_FORMAT(n)      (((n)&0x7)<<2)
// SPI master mode 1 enable
#define CAMERA_CAMERA_SPI_MASTER_EN               (1<<1)
// SPI slave mode enable
#define CAMERA_CAMERA_SPI_SLAVE_EN                  (1<<0)

//--------------------------------
// camera_spi_reg_1
//--------------------------------
#define CAMERA_CLK_DIV_NUM(n)                ((n)&0xffff)      // camera clock divider
#define CAMERA_SSN_CM_INV             (1<<17) // SSN high enable, only for master mode 1

//--------------------------------
// camera_spi_reg_2
//--------------------------------
// only take effect in slave mode which is not supported yet

//--------------------------------
// camera_spi_reg_3
//--------------------------------
// only take effect in slave mode which is not supported yet

//--------------------------------
// camera_spi_reg_4
//--------------------------------
#define CAMERA_BLOCK_NUM_PER_PACKET(n)              (((n)&0x3ff)<<6)
#define CAMERA_IMAGE_WIDTH_CHOOSE_BIT      (1<<5)
#define CAMERA_IMAGE_HEIGHT_CHOOSE_BIT     (1<<4)
#define CAMERA_DATA_SIZE_CHOOSE_BIT       (1<<3)
#define CAMERA_SDO_LINE_CHOOSE_BIT(n)                             (((n)&0x3)<<1)
#define CAMERA_CAMERA_SPI_MASTER_EN_2                      (1<<0)

//--------------------------------
// camera_spi_reg_5
//--------------------------------
#define CAM_SPI_REG_SYNC_CODE(n)                ((n)&0xffffff)

//--------------------------------
// camera_spi_reg_6
//--------------------------------
// frame_start packet id, only in master mode 2
#define CAM_SPI_REG_FRM_START_PKT_ID(n)            (((n)&0xff)<<24)
// frame_end packet id, only in master mode 2
#define CAM_SPI_REG_FRM_ENDN_PKT_ID(n)             (((n)&0xff)<<16)
// line_start packet id, only in master mode 2
#define CAM_SPI_REG_LINE_START_PKT_ID(n)           (((n)&0xff)<<8)
// data packet id, only in master mode 2
#define CAM_SPI_REG_DATA_PKT_ID(n)               ((n)&0xff)

//--------------------------------
// camera_spi_observe_reg_0 (read only)
//--------------------------------

//--------------------------------
// camera_spi_observe_reg_1 (read only)
//--------------------------------


#endif

