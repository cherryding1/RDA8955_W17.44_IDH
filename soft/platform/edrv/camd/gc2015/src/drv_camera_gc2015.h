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


#ifndef _DRV_CAMERA_GC2015_H_
#define _DRV_CAMERA_GC2015_H_

#include "hal_i2c.h"
#include "hal_camera.h"
#include "camera_m.h"

// ############
// Private
// ############

// These are the commands specific to the SCCB
#define PHASE3_WR_PH1   0x11000     // Start & Write    Slave ID
#define PHASE3_WR_PH2   0x1000      // Write            Slave Register Address
#define PHASE3_WR_PH3   0x1100      // Stop & Write     Slave Register Data

#define PHASE2_WR_PH1   0x11000     // Start & Write    Slave ID
#define PHASE2_WR_PH2   0x1100      // Stop & Write     Slave Register Address (to read)

#define PHASE2_RD_PH1   0x11000     // Start & Write    Slave ID
#define PHASE2_RD_PH2   0x111       // Stop & Read & NACK   Slave Register Read Data

typedef struct
{
    UINT8 Addr;            // Register Address
    UINT8 Data;             // Register Data
    UINT8 Wait;            // Time to wait after write
} CAM_REG_T;

typedef struct
{
    UINT16 Len;             // Size of array
    CAM_REG_T *RegVal;      // Array of registers
} CAM_REG_GROUP_T;

typedef enum
{
    CAM_MODE_IDLE,
    CAM_MODE_VIEWFINDER,
    CAM_MODE_PREP_IMG_CAPTURE,
    CAM_MODE_IMG_CAPTURE,
    CAM_MODE_MAX
} CAM_MODE_T;

// General setting
PRIVATE const CAM_REG_T RG_Init_PowerOn[] =
{
    {0xfe, 0x80, 1},//soft reset
    {0xfe, 0x80, 1},//soft reset
    {0xfe, 0x80, 1},//soft reset

    {0xfe, 0x00, 1}, //page0
    {0x45, 0x00, 1},//output_disable

    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////preview capture switch /////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    //preview
    {0x02, 0x01, 1},//preview mode
    {0x2a, 0xca, 1},//[7]col_binning, 0x[6]even skip
    {0x48, 0x40, 1},//manual_gain

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////// preview LSC /////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    {0xfe, 0x01, 1}, //page1
    {0xb0, 0x03, 1},//[4]Y_LSC_en [3]lsc_compensate [2]signed_b4 [1:0]pixel array select
    {0xb1, 0x23, 1},//P_LSC_red_b2
    {0xb2, 0x20, 1},//P_LSC_green_b2
    {0xb3, 0x20, 1},//P_LSC_blue_b2
    {0xb4, 0x24, 1},//P_LSC_red_b4
    {0xb5, 0x20, 1},//P_LSC_green_b4
    {0xb6, 0x22, 1},//P_LSC_blue_b4
    {0xb7, 0x00, 1},//P_LSC_compensate_b2
    {0xb8, 0x80, 1},//P_LSC_row_center, 0x344, 0x (1200/2-344)/2=128, 0x, 0x
    {0xb9, 0x80, 1},//P_LSC_col_center, 0x544, 0x (1600/2-544)/2=128


    ////////////////////////////////////////////////////////////////////////
    ////////////////////////// capture LSC /////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    {0xba, 0x03, 1},//[4]Y_LSC_en [3]lsc_compensate [2]signed_b4 [1:0]pixel array select
    {0xbb, 0x23, 1},//C_LSC_red_b2
    {0xbc, 0x20, 1},//C_LSC_green_b2
    {0xbd, 0x20, 1},//C_LSC_blue_b2
    {0xbe, 0x24, 1},//C_LSC_red_b4
    {0xbf, 0x20, 1},//C_LSC_green_b4
    {0xc0, 0x22, 1},//C_LSC_blue_b4
    {0xc1, 0x00, 1},//C_Lsc_compensate_b2
    {0xc2, 0x80, 1},//C_LSC_row_center, 0x344, 0x (1200/2-344)/2=128
    {0xc3, 0x80, 1},//C_LSC_col_center, 0x544, 0x (1600/2-544)/2=128
    {0xfe, 0x00, 1}, //page0

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////// analog configure ///////////////////////////
    ////////////////////////////////////////////////////////////////////////
    {0xfe, 0x00, 1}, //page0
    {0x29, 0x00, 1},//cisctl mode 1
    {0x2b, 0x06, 1},//cisctl mode 3
    {0x32, 0x1c, 1},//analog mode 1
    {0x33, 0x0f, 1},//analog mode 2
    {0x34, 0x30, 1},//[6:4]da_rsg

    {0x35, 0x88, 1},//Vref_A25
    {0x37, 0x13, 1},//Drive Current

    /////////////////////////////////////////////////////////////////////
    /////////////////////////// ISP Related /////////////////////////////
    /////////////////////////////////////////////////////////////////////
    {0x40, 0xff, 1},
    {0x41, 0x20, 1},//[5]skin_detectionenable[2]auto_gray, 0x[1]y_gamma
    {0x42, 0xf6, 1},//[7]auto_sa[6]auto_ee[5]auto_dndd[4]auto_lsc[3]na[2]abs, 0x[1]awb
    {0x4b, 0xe8, 1},//[1]AWB_gain_mode, 0x1:atpregain0:atpostgain
    {0x4d, 0x03, 1},//[1]inbf_en
    {0x4f, 0x01, 1},//AEC enable

    ////////////////////////////////////////////////////////////////////
    ///////////////////////////  BLK  //////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    {0x63, 0x77, 1},//BLK mode 1
    {0x66, 0x00, 1},//BLK global offset
    {0x6d, 0x00, 1},
    {0x6e, 0x1a, 1},//BLK offset submode,offset R
    {0x6f, 0x20, 1},
    {0x70, 0x1a, 1},
    {0x71, 0x20, 1},
    {0x73, 0x00, 1},
    {0x77, 0x80, 1},
    {0x78, 0x80, 1},
    {0x79, 0x90, 1},

    ////////////////////////////////////////////////////////////////////
    /////////////////////////// DNDD ///////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    {0x80, 0x07, 1},//[7]dn_inc_or_dec [4]zero_weight_mode[3]share [2]c_weight_adap [1]dn_lsc_mode [0]dn_b
    {0x82, 0x0c, 1},//DN lilat b base
    {0x83, 0x03, 1},

    ////////////////////////////////////////////////////////////////////
    /////////////////////////// EEINTP ////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    {0x8a, 0x7c, 1},
    {0x8c, 0x02, 1},
    {0x8e, 0x02, 1},
    {0x8f, 0x45, 1},


    /////////////////////////////////////////////////////////////////////
    /////////////////////////// CC_t ////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    {0xb0, 0x48, 1},
    {0xb1, 0xfe, 1},
    {0xb2, 0x00, 1},
    {0xb3, 0xf0, 1},
    {0xb4, 0x50, 1},
    {0xb5, 0xf8, 1},
    {0xb6, 0x00, 1},
    {0xb7, 0x00, 1},
    {0xb8, 0x00, 1},


    /////////////////////////////////////////////////////////////////////
    /////////////////////////// GAMMA ///////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    //RGB_GAMMA
    {0xbf, 0x08, 1},
    {0xc0, 0x1e, 1},
    {0xc1, 0x33, 1},
    {0xc2, 0x47, 1},
    {0xc3, 0x59, 1},
    {0xc4, 0x68, 1},
    {0xc5, 0x74, 1},
    {0xc6, 0x86, 1},
    {0xc7, 0x97, 1},
    {0xc8, 0xA5, 1},
    {0xc9, 0xB1, 1},
    {0xca, 0xBd, 1},
    {0xcb, 0xC8, 1},
    {0xcc, 0xD3, 1},
    {0xcd, 0xE4, 1},
    {0xce, 0xF4, 1},
    {0xcf, 0xff, 1},

    /////////////////////////////////////////////////////////////////////
    /////////////////////////// YCP_t ///////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    {0xd1, 0x38, 1},//saturation
    {0xd2, 0x38, 1},//saturation
    {0xdd, 0x38, 1},//edge_dec
    {0xde, 0x21, 1},//auto_gray

    ////////////////////////////////////////////////////////////////////
    /////////////////////////// ASDE ///////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    {0x98, 0x3a, 1},
    {0x99, 0x60, 1},
    {0x9b, 0x00, 1},
    {0x9f, 0x12, 1},
    {0xa1, 0x80, 1},
    {0xa2, 0x21, 1},

    {0xfe, 0x01, 1}, //page1
    {0xc5, 0x10, 1},
    {0xc6, 0xff, 1},
    {0xc7, 0xff, 1},
    {0xc8, 0xff, 1},

    ////////////////////////////////////////////////////////////////////
    /////////////////////////// AEC ////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    {0x10, 0x09, 1},//AEC mode 1
    {0x11, 0xb2, 1},//[7]fix target
    {0x12, 0x20, 1},
    {0x13, 0x78, 1},
    {0x17, 0x00, 1},
    {0x1c, 0x96, 1},
    {0x1d, 0x04, 1},// sunlight step
    {0x1e, 0x11, 1},
    {0x21, 0xc0, 1},//max_post_gain
    {0x22, 0x60, 1},//max_pre_gain
    {0x2d, 0x06, 1},//P_N_AEC_exp_level_1[12:8]
    {0x2e, 0x00, 1},//P_N_AEC_exp_level_1[7:0]
    {0x1e, 0x32, 1},
    {0x33, 0x00, 1},//[6:5]max_exp_level [4:0]min_exp_level
    {0x34, 0x04, 1},// min exp

    ////////////////////////////////////////////////////////////////////
    /////////////////////////// Measure Window /////////////////////////
    ////////////////////////////////////////////////////////////////////
    {0x06, 0x07, 1},
    {0x07, 0x03, 1},
    {0x08, 0x64, 1},
    {0x09, 0x4a, 1},

    ////////////////////////////////////////////////////////////////////
    /////////////////////////// AWB ////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    {0x50, 0xf5, 1},
    {0x51, 0x18, 1},
    {0x53, 0x10, 1},
    {0x54, 0x20, 1},
    {0x55, 0x60, 1},
    {0x57, 0x33, 1},//number limit , 33 half , must <0x65 base on measure wnd now
    {0x5d, 0x52, 1},//44
    {0x5c, 0x25, 1},//show mode,close dark_mode
    {0x5e, 0x19, 1},//close color temp
    {0x5f, 0x50, 1},//50
    {0x60, 0x57, 1},//50
    {0x61, 0xdf, 1},
    {0x62, 0x80, 1},//7b
    {0x63, 0x08, 1},//20
    {0x64, 0x5B, 1},
    {0x65, 0x90, 1},
    {0x66, 0xd0, 1},
    {0x67, 0x80, 1},//5a
    {0x68, 0x68, 1},//68
    {0x69, 0x90, 1},//80

    ////////////////////////////////////////////////////////////////////
    /////////////////////////// ABS ////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    {0x80, 0x82, 1},
    {0x81, 0x00, 1},
    {0x83, 0x10, 1},//ABS Y stretch limit
    {0xfe, 0x00, 1},
    ////////////////////////////////////////////////////////////////////
    /////////////////////////// OUT ////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    {0xfe, 0x00, 1},
    //crop
    {0x50, 0x01, 1},
    {0x51, 0x00, 1},
    {0x52, 0x00, 1},
    {0x53, 0x00, 1},
    {0x54, 0x00, 1},
    {0x55, 0x02, 1},
    {0x56, 0x58, 1},
    {0x57, 0x03, 1},
    {0x58, 0x20, 1},

    {0x44, 0xa0, 1},//YUV sequence
    {0x45, 0x0f, 1},//output enable
    {0x46, 0x03, 1},//sync mode
    {0xfe, 0x00, 1},
    {0x32, 0x34, 1},
    {0x34, 0x00, 1},

    {0x12, 0x02, 1}, //Cap HB
    {0x13, 0xA4, 1},
    {0x14, 0x01, 1}, //Cap VB
    {0x15, 0xFA, 1},
    {0xfe, 0x01, 1},
    {0x3c, 0x00, 1}, //Cap Step 64
    {0x3d, 0x52, 1},
    {0x3e, 0x08, 1},
    {0x3f, 0x02, 1},
    {0xfe, 0x00, 1}, //page0

    {0xfe , 0x00, 1},
    {0x2c , 0x01, 1},
    {0x32 , 0x2c, 1},//analog mode 1
    {0x33 , 0x08, 1},//analog mode 2
    {0x34 , 0x10, 1},//[6:4]da_rsg
};

// UXGA size setting, 1600x1200
PRIVATE const CAM_REG_T RG_Init_UXGA[] =
{
    {0x4f, 0x00, 1},
    {0x42, 0x00, 1},
    //UXGA (1600x1200)
    {0xfe, 0x00, 1},
    {0x48, 0x68, 1},
    {0x02, 0x00, 1},    // capture mode
    {0x2a, 0x0a, 1},
    /* Subsample 1/1 */
    {0x59, 0x11, 1},
    {0x5a, 0x06, 1},
    {0x5b, 0x00, 1},
    {0x5c, 0x00, 1},
    {0x5d, 0x00, 1},
    {0x5e, 0x00, 1},
    {0x5f, 0x00, 1},
    {0x60, 0x00, 1},
    {0x61, 0x00, 1},
    {0x62, 0x00, 1},
    /* Crop */
    {0x50, 0x01, 1},
    {0x51, 0x00, 1},
    {0x52, 0x00, 1},
    {0x53, 0x00, 1},
    {0x54, 0x00, 1},
    {0x55, 0x04, 1},
    {0x56, 0xb0, 1},
    {0x57, 0x06, 1},
    {0x58, 0x40, 1},

    {0x6e, 0x19, 1},
    {0x6f, 0x10, 1},
    {0x70, 0x19, 1},
    {0x71, 0x10, 1},
};

// SXGA size setting, 800x600
PRIVATE const CAM_REG_T RG_Init_SXGA[] =
{
    {0xfe, 0x00, 1},
    {0x48, 0x40, 1},
    {0x02, 0x00, 1},
    {0x2a, 0xca, 1},
    /* Subsample 1/1 */
    {0x59, 0x11, 1},
    {0x5a, 0x06, 1},
    {0x5b, 0x00, 1},
    {0x5c, 0x00, 1},
    {0x5d, 0x00, 1},
    {0x5e, 0x00, 1},
    {0x5f, 0x00, 1},
    {0x60, 0x00, 1},
    {0x61, 0x00, 1},
    {0x62, 0x00, 1},

    /* Crop */
    {0x50, 0x01, 1},
    {0x51, 0x00, 1},
    {0x52, 0x00, 1},
    {0x53, 0x00, 1},
    {0x54, 0x00, 1},
    {0x55, 0x02, 1},
    {0x56, 0x58, 1},
    {0x57, 0x03, 1},
    {0x58, 0x20, 1},
};

// VGA size setting, 640x480
PRIVATE const CAM_REG_T RG_Init_VGA[] =
{
    {0xfe, 0x00, 1},
    {0x48, 0x40, 1},
    {0x02, 0x01, 1},
    {0x2a, 0xca, 1},
    /* Subsample 4/5 */
    {0x59, 0x55, 1},
    {0x5a, 0x06, 1},
    {0x5b, 0x00, 1},
    {0x5c, 0x00, 1},
    {0x5d, 0x01, 1},
    {0x5e, 0x23, 1},
    {0x5f, 0x00, 1},
    {0x60, 0x00, 1},
    {0x61, 0x01, 1},
    {0x62, 0x23, 1},
    /* Crop */
    {0x50, 0x01, 1},
    {0x51, 0x00, 1},
    {0x52, 0x00, 1},
    {0x53, 0x00, 1},
    {0x54, 0x00, 1},
    {0x55, 0x01, 1},
    {0x56, 0xe0, 1},
    {0x57, 0x02, 1},
    {0x58, 0x80, 1},

    {0x4f, 0x01, 1},
    {0x42, 0x02, 1},

    {0x6e, 0x18, 1},
    {0x6f, 0x10, 1},
    {0x70, 0x18, 1},
    {0x71, 0x10, 1},
};

// Fixed 8fps frame rate
PRIVATE const CAM_REG_T RG_Init_FIXED_FR[] =
{
    {0x05 , 0x01 , 1},//HB
    {0x06 , 0x60 , 1},
    {0x07 , 0x06 , 1},//VB
    {0x08 , 0x20 , 1},

    {0xfe , 0x01 , 1},
    {0x29 , 0x00 , 1},//Anti Step
    {0x2a , 0xe8 , 1},

    {0x2b , 0x07 , 1},//Level_0
    {0x2c , 0x40 , 1},
    {0x2d , 0x07 , 1},//Level_1
    {0x2e , 0x40 , 1},
    {0x2f , 0x07 , 1},//Level_2
    {0x30 , 0x40 , 1},
    {0x31 , 0x07 , 1},//Level_3
    {0x32 , 0x40 , 1},

    {0xfe , 0x00 , 1},
};

// Self-adaptive frame rate
PRIVATE const CAM_REG_T RG_Init_ADP_FR[] =
{
    {0x05 , 0x01 , 1},//HB
    {0x06 , 0x3a , 1},
    {0x07 , 0x01 , 1},//VB
    {0x08 , 0xe5 , 1},

    {0xfe , 0x01 , 1},
    {0x29 , 0x00 , 1},//Anti Step
    {0x2a , 0xf3 , 1},

    {0x2b , 0x06 , 1},//Level_0
    {0x2c , 0xa5 , 1},
    {0x2d , 0x07 , 1},//Level_1
    {0x2e , 0x98 , 1},
    {0x2f , 0x0b , 1},//Level_2
    {0x30 , 0x64 , 1},
    {0x31 , 0x11 , 1},//Level_3
    {0x32 , 0x16 , 1},

    {0xfe , 0x00 , 1},
};

typedef struct
{
    // Camera information structure
    char                desc[40];

    UINT8               snrtype;            //sensor type(RGB or YUV)
    UINT8               pclk;               //use PCLK of sensor
    UINT8               clkcfg;             //[0]: VSync delay enable; [1]: Fifo VSync delay enable; [2]: Use PCLK Negtive Edge.
    HAL_I2C_BPS_T  i2crate;            //I2C rate : KHz
    UINT8               i2caddress;         //I2C address
    UINT8               i2cispaddress;      //ISP I2C address
    UINT8               pwrcfg;             //sensor power initial configure(SIF REG801 BIT[0]~BIT[2])
    BOOL                rstactiveh;         //sensor reset active hi/low polarity (TRUE = active hi)
    BOOL                snrrst;             //Reset sensor enable

    CAM_MODE_T     cammode;

    BOOL                capturevideo;
    BOOL                captureframe;
    UINT8               numactivebuffers;
    UINT8               currentbuf;

    UINT8               brightness;         //brightness
    UINT8               contrast;           //contrast

    CAM_FORMAT_T  format;
    UINT32              npixels;
    CAM_IMAGE_T    vidbuffer;

    CAM_REG_GROUP_T     standby;            //sensor standby register
    CAM_REG_GROUP_T     Initpoweron;        //SENSOR ISP initial configure

    CAM_REG_GROUP_T     sifpwronseq;        //Sif config sequence(Reg.800 bit0~2) when sensor power on
    CAM_REG_GROUP_T     sifstdbyseq;        //Sif config sequence(Reg.800 bit0~2) when sensor standby
    CAM_REG_GROUP_T     dsif;               //SIF initial configure

    CAM_IRQ_HANDLER_T  camirqhandler;
} CAM_INFO_T;

PRIVATE volatile CAM_INFO_T gSensorInfo =
{
    "GC2005",   // sensor description
    0,  // sensor type
    1,  // PCLK
    0,  // 0: VSYNC Delay Enable; 1: FIFO VSYNC Delay Enable; 2: Use PCLK Negative Edge
    HAL_I2C_BPS_100K,   // I2C Rate
    0x30,   // I2C Address
    0x30,   // I2C ISP Address
    0x02,   // PowerCfg
    TRUE,   // sensor reset active high
    FALSE,  // TRUE: reset sensor; FALSE: no sensor reset
    CAM_MODE_IDLE,
    FALSE,
    FALSE,
    0,
    0,
    3,  // brightness
    4,  // contrast
    CAM_FORMAT_RGB565,
    CAM_NPIX_UXGA,
    NULL,
    {0, NULL},
    {sizeof(RG_Init_PowerOn)/sizeof(CAM_REG_T), (CAM_REG_T*)(&RG_Init_PowerOn)},
    {0, NULL},
    {0, NULL},
    {0, NULL},
    NULL
};

PRIVATE VOID camerap_Delay(UINT16 Wait_mS);
PRIVATE VOID camerap_WriteOneReg( UINT8 Addr, UINT8 Data);
PRIVATE VOID camerap_WriteReg(CAM_REG_T RegWrite);
PRIVATE UINT8 camerap_ReadReg(UINT8 Addr);
PRIVATE VOID camerap_SendRgSeq(const CAM_REG_T *RegList, UINT16 Len);
PRIVATE VOID camerap_InteruptHandler(HAL_CAMERA_IRQ_CAUSE_T cause);
PRIVATE UINT8 camerap_GetId(VOID);

#endif //_DRV_CAMERA_GC2015_H_

