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


#ifndef _DRV_CAMERA_GC6103_H_
#define _DRV_CAMERA_GC6103_H_

#include "hal_i2c.h"
#include "hal_camera.h"
#include "camera_m.h"

// These are the commands specific to the SCCB
#define PHASE3_WR_PH1   0x11000     // Start & Write    Slave ID
#define PHASE3_WR_PH2   0x1000      // Write            Slave Register Address
#define PHASE3_WR_PH3   0x1100      // Stop & Write     Slave Register Data

#define PHASE2_WR_PH1   0x11000     // Start & Write    Slave ID
#define PHASE2_WR_PH2   0x1100      // Stop & Write     Slave Register Address (to read)

#define PHASE2_RD_PH1   0x11000     // Start & Write    Slave ID
#define PHASE2_RD_PH2   0x111       // Stop & Read & NACK   Slave Register Read Data

#define I2C_ADDR_6103   0x21
#define I2C_ADDR_ISP     0x40

#define CHIP_ID_6103     0xb0

typedef VOID (*CAM_IRQ_HDL_T)(UINT8 id, UINT8 ActiveBufNum);

typedef struct
{
    UINT8 addr;            // Register Address
    UINT8 data;             // Register Data
    UINT8 wait;            // Time to wait after write
} CAM_REG_T;

typedef enum
{
    CAM_MODE_IDLE,
    CAM_MODE_PREVIEW_STAGE_1,
    CAM_MODE_PREVIEW_STAGE_2,
    CAM_MODE_CAPTURE_STAGE_1,
    CAM_MODE_CAPTURE_STAGE_2,
    CAM_MODE_PREP_VID_CAPTURE,
    CAM_MODE_VID_CAPTURE
} CAM_MODE_E;

typedef struct
{
    CAM_MODE_E camMode;
    UINT16* pvidBuf;
    UINT32 nPixels;
} CAM_RUNTIME_T;

// ISP initialization
PRIVATE const CAM_REG_T RG_ISP_INIT[] =
{
    {0xfe, 0x80,1},
    {0xfc, 0x12,1}, //clk_en

    {0xfa, 0x00,1},

    {0xfe, 0x00,1},
    {0x09, 0x01,1},
    {0x0a, 0x48,1},
    {0x0b, 0x00,1},
    {0x0c, 0xf8,1},
    {0x12, 0x2a,1},
    {0x14, 0x10,1},
    {0x16, 0x06,1},

    {0x1c, 0x49,1},
    {0x1d, 0x95,1},
    {0x20, 0x7f,1},
    {0x21, 0x38,1},
    {0x22, 0x73,1},
    {0x50, 0x14,1},
    /********************************************/
    /******************* BLK ********************/
    /********************************************/
    {0x30, 0xf7,1},
    {0x39, 0x09,1},
    {0x3a, 0x18,1},
    {0x3b, 0x20,1},
    {0x3c, 0x22,1},
    {0x3d, 0x22,1},
    /********************************************/
    /******************* LSC ********************/
    /********************************************/
    {0x88, 0x20,1},
    {0x8b, 0x24,1},
    {0x8c, 0x20,1},
    {0x8d, 0x18,1},

    /********************************************/
    /******************* DNDD ********************/
    /********************************************/
    {0x62, 0x02,1},  // b_base
    {0x64, 0x05,1},

    /********************************************/
    /******************* ASDE********************/
    /********************************************/
    {0x69, 0x38,1},  //[5]auto_disable_dn when b<2, [4] 1->enhance dn when dark, 0->decrease dn when dark,
    //[3:0]c_slope
    {0x6a, 0x88,1},  // [7:4]b_slope , [3:0]n_slope
    {0x6e, 0x50,1},  // ee1_effect,  ee1_slope
    {0x6f, 0x40,1},  // ee2_effect,  ee2_slope


    /********************************************/
    /******************* EEINTP ********************/
    /********************************************/
    {0x72, 0x6c,1},


    /********************************************/
    /******************* AWB ********************/
    /********************************************/
    {0xfe, 0x01,1},
    {0x00, 0xf5,1},
    {0x02, 0x1a,1},
    {0x03, 0x20,1},
    {0x04, 0x10,1},
    {0x05, 0x08,1},
    {0x06, 0x20,1},
    {0x08, 0x20,1},
    {0x0a, 0x40,1},
    {0x11, 0x3f,1},
    {0x12, 0x72,1},
    {0x13, 0x11,1},
    {0x14, 0x42,1},
    {0x15, 0x43,1},
    {0x16, 0xc2,1},
    {0x17, 0xa6,1},

    {0xc0, 0x08,1},
    {0xc1, 0x06,1},
    {0xc2, 0x3e,1},
    {0xc3, 0x4e,1},

    /********************************************/
    /******************* ABS ********************/
    /********************************************/
    {0xb0, 0x02,1},
    {0xb1, 0x01,1},
    {0xb2, 0x00,1},
    {0xb3, 0x10,1},

    /********************************************/
    /******************* YCP ********************/
    /********************************************/
    {0xfe, 0x00,1},
    {0xb1, 0x38,1},
    {0xb2, 0x38,1},

    /********************************************/
    /*******************Gamma********************/
    /********************************************/
    {0x9F, 0x0E,1},
    {0xA0, 0x1C,1},
    {0xA1, 0x34,1},
    {0xA2, 0x48,1},
    {0xA3, 0x5A,1},
    {0xA4, 0x6B,1},
    {0xA5, 0x7B,1},
    {0xA6, 0x95,1},
    {0xA7, 0xAB,1},
    {0xA8, 0xBF,1},
    {0xA9, 0xCE,1},
    {0xAA, 0xD9,1},
    {0xAB, 0xE4,1},
    {0xAC, 0xEC,1},
    {0xAD, 0xF7,1},
    {0xAE, 0xFd,1},
    {0xAF, 0xFF,1},

    /********************************************/
    /******************* AEC ********************/
    /********************************************/
    {0xfe, 0x00,1},
    {0xe5, 0x01,1},  //AEC enable
    {0xfe, 0x01,1},
    {0x30, 0x0b,1},
    {0x31, 0x10,1},
    {0x32, 0x00,1},
    {0x33, 0x48,1}, // Y target
    {0x3b, 0x92,1},
    {0x3c, 0xa5,1},

    {0xfe, 0x00,1},
    {0x01, 0xc3,1},
    {0x02, 0x0e,1},
    {0x0f, 0x07,1},

    {0xfe, 0x01,1},
    {0x42, 0x00,1},
    {0x43, 0x39,1},

    {0x44, 0x01,1},
    {0x45, 0x56,1},
    {0x46, 0x01,1},
    {0x47, 0xc8,1},
    {0x48, 0x02,1},
    {0x49, 0x3a,1},
    {0x4a, 0x03,1},
    {0x4b, 0x57,1},

    {0x4c, 0x20,1},

    {0x4d, 0x02,1},
    {0x4e, 0xc0,1},
    {0x4f, 0x40,1},

    /**********************************************/
    /*******************Receive Mode***************/
    /**********************************************/
    {0xfe, 0x00, 1},
    {0xfd, 0x04, 1},
    {0x43, 0x01, 1},
    {0x19, 0x01, 1},
    {0xfe, 0x40, 1},

    /********************************************/
    {0xfe, 0x00,1},
    {0x1f, 0x15,1},  // bit[7:6] driving SCK current
    //{0x1f, 0x95,1},  // bit[7:6] driving SCK current
    {0x1d, 0x95,1},  // bit[3:2] driving SDO_0 current

    //{0x24, 0xa6,1},  // a2= yuv  a6=rgb
    {0x24, 0xa0,1},  // a2= yuv  a6=rgb
};

// QVGA(240x320) output
PRIVATE const CAM_REG_T RG_ISP_QVGA[] =
{
    {0xfe, 0x00, 1},
    {0xd4, 0x11, 1},
    {0xd5, 0x07, 1},
    {0xd6, 0x00, 1},
    {0xd7, 0x00, 1},
    {0xd8, 0x00, 1},
    {0xd9, 0x00, 1},
    {0xda, 0x00, 1},
    {0xdb, 0x00, 1},
    {0xdc, 0x00, 1},
    {0xdd, 0x00, 1},

    {0x46, 0x80, 1},
    {0x47, 0x00, 1},
    {0x48, 0x00, 1},
    {0x49, 0x01, 1},
    {0x4a, 0x40, 1},
    {0x4b, 0x00, 1},
    {0x4c, 0xf0, 1},

    {0xfe, 0x02, 1},
    {0x0b, 0xf0, 1},
    {0x0d, 0x40, 1},
    {0x0e, 0x01, 1},
    {0x13, 0xf0, 1},
    {0xfe, 0x00, 1},
};

// QQVGA(160x120) output
PRIVATE const CAM_REG_T RG_ISP_QQVGA[] =
{
    {0xfe, 0x00, 1},
    {0xd4, 0x33, 1},
    {0xd5, 0x07, 1},
    {0xd6, 0x00, 1},
    {0xd7, 0x00, 1},
    {0xd8, 0x00, 1},
    {0xd9, 0x01, 1},
    {0xda, 0x00, 1},
    {0xdb, 0x00, 1},
    {0xdc, 0x00, 1},
    {0xdd, 0x01, 1},

    {0x46, 0x80, 1},
    {0x47, 0x2c, 1},
    {0x48, 0x00, 1},
    {0x49, 0x00, 1},
    {0x4a, 0x78, 1},
    {0x4b, 0x00, 1},
    {0x4c, 0xa0, 1},

    {0xfe, 0x02, 1},
    {0x0b, 0xa0, 1},
    {0x0d, 0x78, 1},
    {0x0e, 0x00, 1},
    {0x13, 0xa0, 1},

    {0xfe, 0x00, 1},
};

// SPI dual-wire mode
PRIVATE const CAM_REG_T RG_SPI_MST2_1[] =
{
    {0xfe, 0x02, 1},
    {0x01, 0x03, 1},
    {0x02, 0x82, 1},
    {0x03, 0x20, 1},
    {0x04, 0x20, 1},
    {0x05, 0x00, 1},
    {0x09, 0x0f, 1},
    {0x24, 0x00, 1},

    {0xfe, 0x00, 1},
    {0xf0, 0x11, 1}, //output_en
};

// SPI single-wire mode
PRIVATE const CAM_REG_T RG_SPI_MST2_2[] =
{
    {0xfe, 0x02, 1},
    {0x01, 0x03, 1},
    {0x02, 0x80, 1},
    {0x03, 0x24, 1},
    {0x04, 0x20, 1},
    {0x05, 0x00, 1},
    {0x09, 0x0f, 1},
    {0x24, 0x00, 1},

    {0xfe, 0x00, 1},
    {0xf0, 0x13, 1}, //output_en
};

// 6103 initialization
PRIVATE const CAM_REG_T RG_6103_INIT[] =
{
    // This setting is for GC6103 output to GC_ISP(GC6113)
    {0xfc, 0x12, 1},
    {0xf9, 0x03, 1},  //output enable
    //{0xf9, 0x00, 1},  //output enable
    {0xfa, 0x00, 1},
    {0x60, 0x07, 1},
    {0x64, 0x01, 1},  // div 3+1


    {0x08, 0x04, 1},  // column start  Updated by Mormo 2011/05/31
    {0x0e, 0x08, 1},
    {0x14, 0x10, 1},
    {0x16, 0x06, 1},
    {0x1f, 0x15, 1},  // pad driver current
    {0xd2, 0x80, 1},  // aec enable

    {0x20, 0xa8, 1},  // gamma , lsc

    {0x50, 0x14, 1},  // global gain
    {0x51, 0x40, 1},  // gain to 1x

    {0x1c, 0x49, 1},
    {0x1d, 0x95, 1},

    /********************************************/
    /******************* BLK ********************/
    /********************************************/
    {0x30, 0xf7, 1},  // blk disable
    {0x39, 0x09, 1},  // exp_rate_darkc
    {0x3a, 0x18, 1},
    {0x3b, 0x20, 1},


    /********************************************/
    /******************* AEC ********************/
    /********************************************/

    {0xd0, 0x02, 1},
    {0xd1, 0x10, 1},

    {0xd2, 0xf2, 1},  // aec on
    {0xd3, 0x50, 1},  // Y target
    {0xdb, 0x92, 1},
    {0xdc, 0xa5, 1},

    {0xed, 0x02, 1},   // min_exp
    {0xef, 0x60, 1},   //aec pre gain limit
};

PRIVATE const CAM_REG_T RG_6103_ADP_FR[] =
{
    // based on 19.5M MCLK
    {0x01, 0xeb, 1},  //hb[7:0]
    {0x02, 0x08, 1},  //vb[7:0]
    {0x0f, 0x07, 1},  //vb[11:8] , hb[11:8]

    {0xe2, 0x00, 1},   //step[11:8]
    {0xe3, 0x2a, 1},   //step[7:0]

    {0xe4, 0x01, 1},   // level 0 10fps
    {0xe5, 0x50, 1},
    {0xe6, 0x01, 1},   // level 1
    {0xe7, 0xa4, 1},
    {0xe8, 0x01, 1},   // level 2
    {0xe9, 0xf8, 1},
    {0xea, 0x02, 1},   // level 3 5fps
    {0xeb, 0xa0, 1},

    {0xec, 0x20, 1},   // exp_level
};

PRIVATE const CAM_REG_T RG_6103_FIX_FR[] =
{
    // based on 19.5M MCLK
#ifdef VDOREC_FAST_RECORDING
    // 15 FPS
    {0x01, 0x6b, 1},  //hb[7:0]
    {0x02, 0x14, 1},  //vb[7:0]
    {0x0f, 0x05, 1},  //vb[11:8] , hb[11:8]

    {0xe2, 0x00, 1},   //step[11:8]
    {0xe3, 0x3a, 1},   //step[7:0]

    {0xe4, 0x01, 1},   // level 0 16fps
    {0xe5, 0x5c, 1},
    {0xe6, 0x01, 1},   // level 1
    {0xe7, 0xd0, 1},
    {0xe8, 0x02, 1},   // level 2
    {0xe9, 0x44, 1},
    {0xea, 0x03, 1},   // level 3 5fps
    {0xeb, 0x66, 1},

    {0xec, 0x00, 1},   // exp_level
#else
    // 10 FPS
    {0x01, 0x6e, 1},  //hb[7:0]
    {0x02, 0x20, 1},  //vb[7:0]
    {0x0f, 0x09, 1},  //vb[11:8] , hb[11:8]

    {0xe2, 0x00, 1},   //step[11:8]
    {0xe3, 0x24, 1},   //step[7:0]

    {0xe4, 0x01, 1},   // level 0 10fps
    {0xe5, 0x68, 1},
    {0xe6, 0x01, 1},   // level 1
    {0xe7, 0xb0, 1},
    {0xe8, 0x02, 1},   // level 2
    {0xe9, 0x1c, 1},
    {0xea, 0x02, 1},   // level 3 5fps
    {0xeb, 0xd0, 1},

    {0xec, 0x00, 1},   // exp_level
#endif
};

PRIVATE VOID camerap_InteruptHandler(HAL_CAMERA_IRQ_CAUSE_T cause);

#endif //_DRV_CAMERA_GC6103_H_
