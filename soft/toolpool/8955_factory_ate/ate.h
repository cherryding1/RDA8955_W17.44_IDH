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


#ifndef _FLASH_PROG_MAP_H_
#define _FLASH_PROG_MAP_H_

#include "cs_types.h"
#include "global_macros.h"
#include "tgt_memd_cfg.h"
#include "memd_m.h"
#include "pmd_m.h"
#include "boot_usb_monitor.h"

#include "hal_sys.h"
#include "hal_debug.h"
#include "hal_dma.h"
#include "hal_sys.h"
#include "hal_timers.h"
#include "hal_ebc.h"
#include "hal_map_engine.h"
#include "mem_bridge.h"
#include "hal_gouda.h"
#include "hal_uart.h"
#include "hal_rfspi.h"
#include "hal_dma.h"
#include "sys_ctrl.h"
#include "tgt_ddr_cfg.h"
#include "memd_m.h"
#include "psram8_ctrl.h"

#define REG_RF_SPI_BASE             0x01A0E000
//Status
#define RF_SPI_ACTIVE_STATUS       (1<<0)
#define RF_SPI_ERROR_CMD           (1<<1)
#define RF_SPI_TABLE_OVF           (1<<6)
#define RF_SPI_TABLE_UDF           (1<<7)
#define RF_SPI_CMD_LEVEL(n)        (((n)&31)<<8)
#define RF_SPI_CMD_LEVEL_MASK      (31<<8)
#define RF_SPI_CMD_LEVEL_SHIFT     (8)
#define RF_SPI_CMD_OVF             (1<<14)
#define RF_SPI_CMD_UDF             (1<<15)
#define RF_SPI_CMD_DATA_LEVEL(n)   (((n)&31)<<16)
#define RF_SPI_CMD_DATA_LEVEL_MASK (31<<16)
#define RF_SPI_CMD_DATA_LEVEL_SHIFT (16)
#define RF_SPI_CMD_DATA_OVF        (1<<22)
#define RF_SPI_CMD_DATA_UDF        (1<<23)
#define RF_SPI_RX_LEVEL(n)         (((n)&7)<<24)
#define RF_SPI_RX_LEVEL_MASK       (7<<24)
#define RF_SPI_RX_LEVEL_SHIFT      (24)
#define RF_SPI_RX_OVF              (1<<30)
#define RF_SPI_RX_UDF              (1<<31)

//Command
#define RF_SPI_SEND_CMD            (1<<0)
#define RF_SPI_FLUSH_CMD_FIFO      (1<<8)
#define RF_SPI_FLUSH_RX_FIFO       (1<<16)
#define RF_SPI_RESTART_GAIN        (1<<24)
#define RF_SPI_RELOAD_GAIN         (1<<28)
#define RF_SPI_DRIVE_ZERO          (1<<31)
typedef volatile struct
{
    REG32                          Ctrl;                         //0x00000000
    REG32                          Status;                       //0x00000004
    REG32                          Rx_Data;                      //0x00000008
    REG32                          Command;                      //0x0000000C
    REG32                          Cmd_Size;                     //0x00000010
    REG32                          Cmd_Data;                     //0x00000014
    REG32                          Gain_Size;                    //0x00000018
    REG32                          Gain_Data;                    //0x0000001C
    REG32                          IRQ;                          //0x00000020
    REG32                          IRQ_Mask;                     //0x00000024
    REG32                          IRQ_Threshold;                //0x00000028
    REG32                          Divider;                      //0x0000002C
} HWP_RF_SPI_T;

#define hwp_rfSpi                   ((HWP_RF_SPI_T*) KSEG1(REG_RF_SPI_BASE))

#define RDA6220_ONE_CMD_BYTE_QTY 4

#define RDA6220_ONE_CMD_BYTE_QTY        4
#define RDA6220_ADDR_MASK               0x1FF
#define RDA6220_ADDR_OFFSET             22
#define RDA6220_DATA_MASK               0x3FFFF
#define RDA6220_DATA_OFFSET             4

#define BYTE_SIZE                       8
#define BYTE_MASK                       0xff


//Cmd_Size
#define RF_SPI_CMD_SIZE(n)         (((n)&0xFF)<<0)
#define RF_SPI_CMD_MARK            (1<<31)

//Cmd_Data
#define RF_SPI_CMD_DATA(n)         (((n)&0xFF)<<0)

//Gain_Size
#define RF_SPI_GAIN_SIZE(n)        (((n)&15)<<0)

//Gain_Data
#define RF_SPI_GAIN_DATA(n)        (((n)&0xFF)<<0)
#define RDA6220_WRITE_FMT(addr, data) (((addr & RDA6220_ADDR_MASK) << RDA6220_ADDR_OFFSET) |\
                                           ((data & RDA6220_DATA_MASK) << RDA6220_DATA_OFFSET))

#define RDA6220_READ_FMT(addr, data)  ( ((addr & RDA6220_ADDR_MASK) << RDA6220_ADDR_OFFSET)|\
                                            ((data & RDA6220_DATA_MASK) << RDA6220_DATA_OFFSET)|\
                                                                                         1<<31);

#define RFSPI_CLOCK_19_5M

#if defined(RFSPI_CLOCK_26M)
#define RFSPI_CLOCK HAL_RFSPI_26M_CLK
#define CMD_DUR_FACTOR 2600
#elif defined(RFSPI_CLOCK_19_5M)
#define RFSPI_CLOCK HAL_RFSPI_19_5M_CLK
#define CMD_DUR_FACTOR 1950
#elif defined(RFSPI_CLOCK_13M)
#define RFSPI_CLOCK HAL_RFSPI_13M_CLK
#define CMD_DUR_FACTOR 1300
#elif defined(RFSPI_CLOCK_6_5M)
#define RFSPI_CLOCK HAL_RFSPI_6_5M_CLK
#define CMD_DUR_FACTOR 650
#elif defined(RFSPI_CLOCK_3_25M)
#define RFSPI_CLOCK HAL_RFSPI_3_25M_CLK
#define CMD_DUR_FACTOR 325
#else
#error "RFSPI clock is not specified"
#endif



#define ERAM_CHECK_PATTERN0  0xa569f0f0
#define ERAM_CHECK_PATTERN1  0x5a960f0f
#define ERAM_ADDR_NUM 6
UINT32 eram_end_addr2[ERAM_ADDR_NUM] = {0x200000, 0x400000, 0x800000, 0x1000000, 0x2000000, 0x4000000};
#define ERAM_CHECK_ERROR_CODE 0xdead0000
#define CRAM_DIDR 0x40000
#define TEST_WIDTH 16
#define TEST_HEIGHT 16
#define VL_IMAGE_WIDTH         16
#define VL_IMAGE_HEIGHT        16

UINT16 test_result_gouda[TEST_WIDTH * TEST_HEIGHT]= {0xffff,};

#if 1
// layer definition
HAL_GOUDA_OVL_LAYER_DEF_T def_OvlLayer0;
HAL_GOUDA_OVL_LAYER_DEF_T def_OvlLayer1;
HAL_GOUDA_OVL_LAYER_DEF_T def_OvlLayer2;
HAL_GOUDA_VID_LAYER_DEF_T def_VideoLay;
HAL_GOUDA_VID_LAYER_DEF_T def_Video;

#endif
// image for Video layer
// ------------------------------------------------
u32 VL_IMAGE[VL_IMAGE_WIDTH*VL_IMAGE_HEIGHT/4]  __attribute__((aligned(8)))=
{
};
u32 VL_IMAGEU[VL_IMAGE_WIDTH*VL_IMAGE_HEIGHT/4] =
{
};
u32 VL_IMAGEV[VL_IMAGE_WIDTH*VL_IMAGE_HEIGHT/4] =
{
};



UINT16 gouda_verify[64] =
{
    0x0189,0x0189,0x0169,0x0169,
    0x0169,0x016A,0x016A,0x016A,
    0x016A,0x016A,0x016B,0x016B,
    0x018B,0x018C,0x018C,0x018C,
    0x0189,0x0189,0x0169,0x016A,
    0x016A,0x016A,0x016A,0x016A,
    0x016B,0x016B,0x016B,0x016B,
    0x018B,0x018C,0x018C,0x018D,
    0x0189,0x0189,0x0189,0x016A,
    0x016A,0x016B,0x016B,0x016B,
    0x016B,0x016B,0x016C,0x018C,
    0x018C,0x018C,0x018C,0x018D,
    0x018A,0x018A,0x016A,0x016A,
    0x016A,0x016B,0x016B,0x016B,
    0x016C,0x016C,0x016C,0x018C,
    0x018C,0x018D,0x018D,0x018D

};


#endif

