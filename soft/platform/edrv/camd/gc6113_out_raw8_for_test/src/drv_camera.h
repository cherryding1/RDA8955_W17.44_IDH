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



#include "hal_i2c.h"
#include "hal_camera.h"
#include "camera_m.h"

//#define  DRV_CAM_FRAME_6    1
#define  DRV_CAM_FRAME_10    1
//#define  DRV_CAM_FRAME_12    1
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
    CAM_MODE_PREVIEW_STAGE_1,
    CAM_MODE_PREVIEW_STAGE_2,
    CAM_MODE_CAPTURE_STAGE_1,
    CAM_MODE_CAPTURE_STAGE_2,
    CAM_MODE_PREP_VID_CAPTURE,
    CAM_MODE_VID_CAPTURE
} CAM_MODE_T;

// On FPGA platform, Pixel Clock must not be too fast or xfer screws up
#define CAM_CLK_CFG 0x81

/*/ change teh clk
{0xfa, 0x10},
{0xfe, 0x02},
{0x04, 0x21},
{0xfe, 0x00}
*/

// use this for 320x240 (VGA) capture
PRIVATE const CAM_REG_T RG_Init_PowerOn[] =
{
    {0xfe, 0x80,1},
    {0xfc, 0x12,1}, //clk_en

    {0xfa, 0x00,1}, // Cis clock config 1/1

    {0xfe, 0x00,1},
    {0x09, 0x01,1}, //[8]cis_win_height  328
    {0x0a, 0x48,1}, //[7:0]cis_win_height
    {0x0b, 0x00,1}, //[9:8]cis_win_width 248
    {0x0c, 0xf8,1}, //[7:0]cis_win_width
    {0x12, 0x2a,1}, //sh_delay
    {0x14, 0x10,1},
    {0x16, 0x06,1}, //[3:0]number of a/d pipe stages

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
    {0x39, 0x09,1},  // exp_rate_darkc
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
    {0x14, 0x42,1}, //awb_r_5_gain
    {0x15, 0x43,1}, //awb_b_5_gain
    {0x16, 0xc2,1},   //sinT
    {0x17, 0xa6,1}, //cosT

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
    {0xb3, 0x10,1},   //  y strech limit

    /********************************************/
    /******************* YCP ********************/
    /********************************************/
    {0xfe, 0x00,1},
    {0xb1, 0x38,1},
    {0xb2, 0x38,1},

    /*
    {0xb3, 0x40,1},
    {0xbe, 0x21,1},
    {0x93, 0x48,1},
    {0x94, 0xf6,1},
    {0x95, 0xf8,1},
    {0x96, 0x00,1},
    {0x97, 0x50,1},
    {0x98, 0xfa,1},
    {0x9d, 0x03,1},
    */

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
    {0x01, 0xc3,1},  //hb[7:0]
    {0x02, 0x0e,1},  //vb[7:0]
    {0x0f, 0x07,1},  //vb[11:8] , hb[11:8]

    {0xfe, 0x01,1},
    {0x42, 0x00,1},   //step[11:8]
    {0x43, 0x39,1},   //step[7:0]

    {0x44, 0x01,1},   // level_0  16.67fps
    {0x45, 0x56,1},
    {0x46, 0x01,1},   // level_1  12.49fps
    {0x47, 0xc8,1},
    {0x48, 0x02,1},   // level_2  9.99fps
    {0x49, 0x3a,1},
    {0x4a, 0x03,1},   // level_3  6.67fps
    {0x4b, 0x57,1},

    {0x4c, 0x20,1},  //exp_level

    {0x4d, 0x02,1},  //min_exp
    {0x4e, 0xc0,1},  //aec post gain limit
    {0x4f, 0x40,1},  //aec pre gain limit
#if 1
    //////
    /******************* SPI ********************/
    {0xfe, 0x02,1},
    {0x01, 0x03,1},
    {0x02, 0x80,1},  //LSB & Falling edge sample

    {0x03, 0x05,1},  //two wire data not package SDO1/SDO0:bit7/bit6.....bit1:bit0

    {0x04, 0x20,1},
    {0x05, 0x00,1},
    {0x09, 0x0f,1},  //0x00
    {0x0a, 0x00,1},// 00 yuv  01 rgb   02  raw 8
    {0x13, 0xf0,1},

    {0x24, 0x06,1},  //SCK Always
    {0xfd, 0x03,1},  //DVP Mode

    /********************************************/
    {0xfe, 0x00,1},
    //{0x1f, 0x15,1},  // bit[7:6] driving SCK current
    {0x1f, 0x95,1},  // bit[7:6] driving SCK current
    {0x1d, 0x95,1},  // bit[3:2] driving SDO_0 current

    //{0x24, 0xa6,1},  // a2= yuv  a6=rgb

    //  {0x24, 0xa0,1},  // a0= yuv  a6=rgb



    {0x24, 0xb9, 1},  // raw 8

#ifdef _CAM_SPECIAL_GC6113_
    {0xf0, 0x1f,1}, //output_en
#endif

#else
#if 0
    // dual wire
    /******************* SPI ********************/
    {0xfe, 0x02, 1},
    {0x01, 0x03, 1},
    {0x02, 0x80, 1},
    {0x03, 0x05, 1},
    {0x04, 0x20, 1},
    {0x05, 0x00, 1},
    {0x09, 0x00, 1},
    {0x0a, 0x00, 1},
    {0x13, 0xf0, 1},
    {0x24, 0x00, 1},
    {0xfd, 0x00, 1},
    /******************* OUT ********************/
    {0xfe, 0x00, 1},
    {0x1f, 0x15, 1},
    {0x1d, 0x95, 1},
    {0x24, 0xa2, 1},
    {0xf0, 0x1b, 1},
#else
    // single wire
    /******************* SPI ********************/
    {0xfe, 0x02, 1},
    {0x01, 0x03, 1},
    {0x02, 0x80, 1},
    {0x03, 0x00, 1},
    {0x04, 0x20, 1},
    {0x05, 0x00, 1},
    {0x09, 0x00, 1},
    {0x0a, 0x00, 1},
    {0x13, 0xf0, 1},
    {0x24, 0x00, 1},
    {0xfd, 0x00, 1},
    /******************* OUT ********************/
    {0xfe, 0x00, 1},
    {0x1f, 0x15, 1},
    {0x1d, 0x95, 1},
    //{0x24, 0xa0, 1}, // YUV
    {0x24, 0xa6, 1},    // RGB
    {0xf0, 0x19, 1},
#endif
#endif
    //////

    //-----------------Updated By Mormo 2011/05/11 Start-------------//
    {0xfe, 0x00,1},
    {0x60, 0xe7,1},
    {0x6b, 0x55,1},
    //-----------------Updated By Mormo 2011/05/11 End-------------//


    //-----------------Updated By Mormo 2011/05/19 Start-------------//
    {0xfe, 0x00,1},
    // daemon add start
    {0x2e, 0xc0,1},  // open in_buf
    {0x46, 0x80,1},
    {0x48, 0x02,1},  //Crop Start X
    {0x49, 0x01,1},
    {0x4a, 0x40,1},
    {0x4b, 0x00,1},
    {0x4c, 0xf0,1},
    // daemon add end
    //-----------------Updated By Mormo 2011/05/19 End-------------//

    //-----------------Updated By Mormo 2011/05/31 Start-------------//
    {0xfe, 0x00,1},
    {0x2e, 0x40,1},  // close in_buf
    {0x08, 0x04,1},
    {0x48, 0x00,1},  //Crop Start X 0x00
    //-----------------Updated By Mormo 2011/05/31 End-------------//
#if 1
    // ---------------- for 128*160-----------------------//
    {0xfe, 0x00,1},
    {0xd4, 0xff,1},
    {0xd5, 0x07,1},
    {0xd6, 0x02,1},
    {0xd7, 0x46,1},
    {0xd8, 0x8a,1},
    {0xd9, 0xce,1},
    {0xda, 0x02,1},
    {0xdb, 0x46,1},
    {0xdc, 0x8a,1},
    {0xdd, 0xce,1},

    {0x46, 0x80,1},
    {0x47, 0x04,1},
    {0x48, 0x00,1},
    {0x49, 0x00,1},
    {0x4a, 0xa0,1},
    {0x4b, 0x00,1},
    {0x4c, 0x80,1},

    {0xfe, 0x02,1},
    {0x0b, 0x80,1},
    {0x0d, 0xa0,1},
    {0x0e, 0x00,1},
    {0x13, 0x80,1},
    {0xfe, 0x00,1},
#endif
#if 0
    // change the clk
    {0xfa, 0x10, 1},
    {0xfe, 0x02, 1},
    {0x04, 0x21, 1},
    {0xfe, 0x00, 1}
#endif
#ifdef _CAM_SPECIAL_GC6113_ //12MHz  PCLK output 12.5fps
    {0xfe, 0x02, 1},
    {0x04, 0x21, 1},
    {0xfe, 0x00, 1}
#endif
    //-----------
    //Just For Test By Mormo
    {0x2e, 0x42}
};

// 128 * 160, for camera preview
PRIVATE const CAM_REG_T RG_Reinit_QQVGA[] =
{
    {0xfe, 0x00, 1},
    {0xd4, 0xff, 1},
    {0xd5, 0x07, 1},
    {0xd6, 0x02, 1},
    {0xd7, 0x46, 1},
    {0xd8, 0x8a, 1},
    {0xd9, 0xce, 1},
    {0xda, 0x02, 1},
    {0xdb, 0x46, 1},
    {0xdc, 0x8a, 1},
    {0xdd, 0xce, 1},

    {0x46, 0x80, 1},
    {0x47, 0x04, 1},
    {0x48, 0x00, 1},
    {0x49, 0x00, 1},
    {0x4a, 0xa0, 1},
    {0x4b, 0x00, 1},
    {0x4c, 0x80, 1},

    {0xfe, 0x02, 1},
    {0x0b, 0x80, 1},
    {0x0d, 0xa0, 1},
    {0x0e, 0x00, 1},
    {0x13, 0x80, 1},
    {0xfe, 0x00, 1},
};

// 320 * 240, for capture
PRIVATE const CAM_REG_T RG_Reinit_QVGA[] =
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
    {0x0b, 0x78, 1},  // f0  240      78=120------------------------------------------------
    {0x0d, 0x40, 1},
    {0x0e, 0x01, 1},
    //{0x13, 0xf0, 1},
    {0x13, 0x80, 1},
    {0xfe, 0x00, 1},
};

// specific to video recorder, 160*120
PRIVATE const CAM_REG_T RG_Reinit_QQVGA_2[] =
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

PRIVATE const CAM_REG_T RG_Reinit_MASTER1[] =
{
    {0xfe, 0x02, 1},
    {0x03, 0x00, 1},

    {0xfe, 0x00, 1},
};


PRIVATE const CAM_REG_T RG_Reinit_MASTER2_1[] =
{
    {0xfe, 0x02, 1},
    {0x01, 0x03, 1}, // master enable, spi enable
    {0x02, 0x82, 1},
    //{0x02, 0x80, 1},
    {0x03, 0x20, 1},
    {0x04, 0x20, 1},
    {0x05, 0x00, 1},
    {0x09, 0x0f, 1}, //0x00
    {0x24, 0x00, 1}, //SCK not always on
    {0xfd, 0x00, 1}, //SPI

    {0xfe, 0x00, 1},
    {0xf0, 0x11,1}, //output_en
};

PRIVATE const CAM_REG_T RG_Reinit_MASTER2_2[] =
{
    {0xfe, 0x02, 1},
    {0x01, 0x03, 1}, // master enable, spi enable
    {0x02, 0x80, 1},
    {0x03, 0x24, 1},
    {0x04, 0x21, 1},
    {0x05, 0x00, 1},
    {0x09, 0x0f, 1}, //0x00
    {0x24, 0x00, 1}, //SCK not always on
    {0xfd, 0x00, 1}, //SPI

    {0xfe, 0x00, 1},
    {0xf0, 0x13,1}, //output_en
};

PRIVATE const CAM_REG_T RG_Reinit_MASTER2_4[] =
{
    {0xfe, 0x02, 1},
    {0x03, 0x28, 1},

    {0xfe, 0x00, 1},
    {0xf0, 0x1f,1}, //output_en
};

#if 0
PRIVATE const CAM_REG_T RG_Reinit_SINGLE_WIRE[] =
{
    /******************* SPI ********************/
    {0xfe, 0x02, 1},
    {0x01, 0x03, 1},
    {0x02, 0x80, 1},
    {0x03, 0x00, 1},
    {0x04, 0x20, 1},
    {0x05, 0x00, 1},
    {0x09, 0x00, 1},
    {0x0a, 0x00, 1},
    {0x13, 0xf0, 1},
    {0x24, 0x00, 1},
    {0xfd, 0x04, 1},
    /******************* OUT ********************/
    {0xfe, 0x00, 1},
    {0x1f, 0x15, 1},
    {0x1d, 0x95, 1},
    {0x24, 0xa2, 1},
    {0xf0, 0x19, 1},
};

PRIVATE const CAM_REG_T RG_Reinit_DUAL_WIRE[] =
{
    /******************* SPI ********************/
    {0xfe, 0x02, 1},
    {0x01, 0x03, 1},
    {0x02, 0x80, 1},
    {0x03, 0x05, 1},
    {0x04, 0x20, 1},
    {0x05, 0x00, 1},
    {0x09, 0x00, 1},
    {0x0a, 0x00, 1},
    {0x13, 0xf0, 1},
    {0x24, 0x00, 1},
    {0xfd, 0x04, 1},
    /******************* OUT ********************/
    {0xfe, 0x00, 1},
    {0x1f, 0x15, 1},
    {0x1d, 0x95, 1},
    {0x24, 0xa2, 1},
    {0xf0, 0x1d, 1},
};
#endif

PRIVATE const CAM_REG_T RG_Reinit_MODE_DEFAULT[] =
{
    /******************* SPI ********************/
    {0xfe, 0x02,1},
    {0x01, 0x03,1},
    {0x02, 0x80,1},  //LSB & Falling edge sample

    {0x03, 0x05,1},  //two wire data not package SDO1/SDO0:bit7/bit6.....bit1:bit0

    {0x04, 0x20,1},
    {0x05, 0x00,1},
    {0x09, 0x00,1},
    {0x0a, 0x00,1},
    {0x13, 0xf0,1},

    {0x24, 0x06,1},  //SCK Always
    {0xfd, 0x03,1},  //DVP Mode

    /********************************************/
    {0xfe, 0x00,1},
    //{0x1f, 0x15,1},  // bit[7:6] driving SCK current
    {0x1f, 0x95,1},  // bit[7:6] driving SCK current
    {0x1d, 0x95,1},  // bit[3:2] driving SDO_0 current

    //{0x24, 0xa6,1},  // a2= yuv  a6=rgb
    {0x24, 0xa0,1},  // a2= yuv  a6=rgb
    {0xf0, 0x1f,1}, //output_en
};

// use this for 640x480 (VGA) capture
PRIVATE const CAM_REG_T RG_InitPowerOnVga[] =
    {};


// use this for 160*120 (QQVGA) capture
PRIVATE const CAM_REG_T RG_InitPowerOnQqvga[] =
    {};

PRIVATE const CAM_REG_T RG_VidFormatVga[] =
    {};

PRIVATE const CAM_REG_T RG_VidFormatQvga[] =
{
#if !defined(VDOREC_FAST_RECORDING)
    // 10 fps
    {0xfe, 0x00,1},
    {0x01, 0xc3,1},  //hb[7:0]
    {0x02, 0xf2,1},  //vb[7:0]
    {0x0f, 0x07,1},  //vb[11:8] , hb[11:8]

    {0xfe, 0x01,1},
    {0x42, 0x00,1},   //step[11:8]
    {0x43, 0x39,1},   //step[7:0]

    {0x44, 0x01,1},   // level_0  9.99fps
    {0x45, 0x56,1},
    {0x46, 0x01,1},   // level_1  9.99fps
    {0x47, 0xc8,1},
    {0x48, 0x02,1},   // level_2  9.99fps
    {0x49, 0x3a,1},
    {0x4a, 0x03,1},   // level_3  6.67fps
    {0x4b, 0x57,1},

    {0x4c, 0x00,1},  //exp_level

    {0x4d, 0x02,1},  //min_exp
#else
    // 15 fps
    {0xfe, 0x00,1},
    {0x01, 0xc3,1},  //hb[7:0]
    {0x02, 0x0e,1},  //vb[7:0]
    {0x0f, 0x07,1},  //vb[11:8] , hb[11:8]

    {0xfe, 0x01,1},
    {0x42, 0x00,1},   //step[11:8]
    {0x43, 0x39,1},   //step[7:0]

    {0x44, 0x01,1},   // level_0  16.67fps
    {0x45, 0x56,1},
    {0x46, 0x01,1},   // level_1  12.49fps
    {0x47, 0xc8,1},
    {0x48, 0x02,1},   // level_2  9.99fps
    {0x49, 0x3a,1},
    {0x4a, 0x03,1},   // level_3  6.67fps
    {0x4b, 0x57,1},

    {0x4c, 0x00,1},  //exp_level

    {0x4d, 0x02,1},  //min_exp
#endif
};
/*
PRIVATE const CAM_REG_T RG_VidFormatQqvga[] =
{
#if defined(VIDREC_SLOW_RECORDING)
    // 10 fps
    {0xfe, 0x00,1},
    {0x01, 0x2d,1},  //hb[7:0]
    {0x02, 0x1a,1},  //vb[7:0]
    {0x0f, 0x17,1},  //vb[11:8] , hb[11:8]

    {0xfe, 0x01,1},
    {0x42, 0x00,1},   //step[11:8]
    {0x43, 0x3d,1},   //step[7:0]

    {0x44, 0x01,1},   // level_0  10.00fps
    {0x45, 0x90,1},
    {0x46, 0x02,1},   // level_1  8.33fps
    {0x47, 0xdc,1},
    {0x48, 0x03,1},   // level_2  6.67fps
    {0x49, 0x93,1},
    {0x4a, 0x04,1},   // level_3  5.00fps
    {0x4b, 0xc4,1},

    {0x4c, 0x00,1},  //exp_level

    {0x4d, 0x02,1},  //min_exp
    {0x4e, 0xc0,1},  //aec post gain limit
    {0x4f, 0x40,1},  //aec pre gain limit
#else
    // 15 fps
    {0xfe, 0x00,1},
    {0x01, 0x2d,1},  //hb[7:0]
    {0x02, 0x26,1},  //vb[7:0]
    {0x0f, 0x07,1},  //vb[11:8] , hb[11:8]

    {0xfe, 0x01,1},
    {0x42, 0x00,1},   //step[11:8]
    {0x43, 0x3d,1},   //step[7:0]

    {0x44, 0x01,1},   // level_0  15fps
    {0x45, 0x8a,1},
    {0x46, 0x01,1},   // level_1  12.5fps
    {0x47, 0xe8,1},
    {0x48, 0x02,1},   // level_2  8.33fps
    {0x49, 0xdc,1},
    {0x4a, 0x03,1},   // level_3  6.67fps
    {0x4b, 0xd0,1},

    {0x4c, 0x00,1},  //exp_level

    {0x4d, 0x02,1},  //min_exp
    {0x4e, 0xc0,1},  //aec post gain limit
    {0x4f, 0x40,1},  //aec pre gain limit
#endif
};
*/

PRIVATE const CAM_REG_T RG_VidFormatQqvga[] =
{
#if !defined(VDOREC_FAST_RECORDING)
    // 10 fps
    {0xfe, 0x00,1},
    {0x01, 0xc3,1},  //hb[7:0]
    {0x02, 0xf2,1},  //vb[7:0]
    {0x0f, 0x07,1},  //vb[11:8] , hb[11:8]

    {0xfe, 0x01,1},
    {0x42, 0x00,1},   //step[11:8]
    {0x43, 0x39,1},   //step[7:0]

    {0x44, 0x01,1},   // level_0  9.99fps
    {0x45, 0x56,1},
    {0x46, 0x01,1},   // level_1  9.99fps
    {0x47, 0xc8,1},
    {0x48, 0x02,1},   // level_2  9.99fps
    {0x49, 0x3a,1},
    {0x4a, 0x03,1},   // level_3  6.67fps
    {0x4b, 0x57,1},

    {0x4c, 0x00,1},  //exp_level

    {0x4d, 0x02,1},  //min_exp
#else
    // 15 fps
    {0xfe, 0x00,1},
    {0x01, 0xc3,1},  //hb[7:0]
    {0x02, 0x0e,1},  //vb[7:0]
    {0x0f, 0x07,1},  //vb[11:8] , hb[11:8]

    {0xfe, 0x01,1},
    {0x42, 0x00,1},   //step[11:8]
    {0x43, 0x39,1},   //step[7:0]

    {0x44, 0x01,1},   // level_0  16.67fps
    {0x45, 0x56,1},
    {0x46, 0x01,1},   // level_1  12.49fps
    {0x47, 0xc8,1},
    {0x48, 0x02,1},   // level_2  9.99fps
    {0x49, 0x3a,1},
    {0x4a, 0x03,1},   // level_3  6.67fps
    {0x4b, 0x57,1},

    {0x4c, 0x00,1},  //exp_level

    {0x4d, 0x02,1},  //min_exp
#endif
};

typedef struct
{
    // Camera information structure
    char                desc[40];

    UINT8               snrtype;            //sensor type(RGB or YUV)
    UINT8               pclk;               //use PCLK of sensor
    UINT8               clkcfg;             //[0]: VSync delay enable; [1]: Fifo VSync delay enable; [2]: Use PCLK Negtive Edge.
    HAL_I2C_BPS_T       i2crate;            //I2C rate : KHz
    UINT8               i2caddress;         //I2C address
    UINT8               i2cispaddress;      //ISP I2C address
    UINT8               pwrcfg;             //sensor power initial configure(SIF REG801 BIT[0]~BIT[2])
    BOOL                rstactiveh;         //sensor reset active hi/low polarity (TRUE = active hi)
    BOOL                snrrst;             //Reset sensor enable

    CAM_MODE_T          cammode;


    BOOL                capturevideo;
    BOOL                captureframe;
    UINT8               numactivebuffers;
    UINT8               currentbuf;

    UINT8               brightness;         //brightness
    UINT8               contrast;           //contrast

    CAM_FORMAT_T        format;
    UINT32              npixels;
    CAM_IMAGE_T         vidbuffer;

    CAM_REG_GROUP_T     standby;            //sensor standby register
    CAM_REG_GROUP_T     Initpoweron;        //SENSOR ISP initial configure

    CAM_REG_GROUP_T     sifpwronseq;        //Sif config sequence(Reg.800 bit0~2) when sensor power on
    CAM_REG_GROUP_T     sifstdbyseq;        //Sif config sequence(Reg.800 bit0~2) when sensor standby
    CAM_REG_GROUP_T     dsif;               //SIF initial configure
    //    TSnrSizeCfgGroup    snrSizeCfg;         //sensor size configure information

    // This 'callback' mechanism is not necessary because the encapsulation is taken
    // care of by the environment
    //    PSensorSetReg       snrSetRegCall;      //set reg callback
    //    PSensorGetReg       snrGetRegCall;
    //    PSnrIdChk           snrIdChkCall;
    //    PSnrMirrorFlip      snrMirrorFlipCall;
    //    PSnrContrast        snrContrastCall;
    //    PSnrBrightness      snrBrightnessCall;
    //    PSnrMode            snrModeCall;
    //    PSnrGetEt           snrGetEt;
    //    PSnrSetEt           snrSetEt;
    //    TFlashParm          flashparm;
    CAM_IRQ_HANDLER_T   camirqhandler;
} CAM_INFO_T;

PRIVATE volatile CAM_INFO_T gSensorInfo =
{
    "BG0313",      // char desc[40];

    0,                      // UINT8 snrtype; (rgb)
    1,                      // UINT8 pclk; use or don't use PCLK of sensor???
    0,                      // UINT8 clkcfg; ???
    HAL_I2C_BPS_100K,       // UINT16 i2crate;
    0x40,                   // UINT8 i2caddress; this is the _REAL_ slave address.
    //  The value written in phase 1 of a write cycle will be 0x42
    //  while the value just before the read will be 0x43
    0x21,                   // UINT8 i2cispaddress;
    0x02,                   // UINT8 pwrcfg;???
    TRUE,                   // BOOL rstactiveh;  // active high
    FALSE,                  // BOOL snrrst;

    CAM_MODE_IDLE,          // cammode

    FALSE,                  // Capture video mode
    FALSE,                  // Capturing single frame
    0,                      // Number of active buffers reserved by the upper layer
    0,                      // current frame buffer

    3,                      // UINT8 brightness;
    4,                      // UINT8 contrast;
    CAM_FORMAT_RGB565,      // Format
    CAM_NPIX_QVGA,           // npixels(VGA, QVGA, QQVGA...)
    NULL,                   // Video Buffer.  First frame buffer for video or image buffer

    {0, NULL},              // CAM_REG_GROUP_T standby;
    // Should start with QQVGA, but for testing, start with VGA
    //    {sizeof(RG_InitPowerOnQqvga) / sizeof(CAM_REG_T), (CAM_REG_T*)&RG_InitPowerOnQqvga}, // CAM_REG_GROUP_T Initpoweron;
    {sizeof(RG_InitPowerOnVga) / sizeof(CAM_REG_T), (CAM_REG_T*)&RG_InitPowerOnVga}, // CAM_REG_GROUP_T Initpoweron;

    {0, NULL},              // CAM_REG_GROUP_T sifpwronseq;
    {0, NULL},              // CAM_REG_GROUP_T sifstdbyseq;
    {0, NULL},              // CAM_REG_GROUP_T dsif; initial value
    NULL                    // IRQ Callback
};

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

PRIVATE VOID camerap_WriteOneReg( UINT8 Addr, UINT8 Data);
PRIVATE VOID camerap_Delay(UINT16 Wait_mS);
PRIVATE VOID camerap_WriteReg(CAM_REG_T RegWrite);
PRIVATE UINT8 camerap_ReadReg(UINT8 Addr);
PRIVATE VOID camerap_SendRgSeq(const CAM_REG_T *RegList, UINT16 Len);
PRIVATE VOID camerap_InteruptHandler(HAL_CAMERA_IRQ_CAUSE_T cause);
PRIVATE UINT8 camerap_GetId(VOID);

