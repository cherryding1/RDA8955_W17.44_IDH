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

// 1 - yuv
// 0 - RGB565
#define YUV_OUTPUT  1
//#define YUV_OUTPUT  0
//#define RGB565_OUTPUT

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

// use this for 320x240 (VGA) capture
PRIVATE const CAM_REG_T RG_Init_PowerOn[] =
{
    {0x00, 0x00,1}, //// Block Select
    {0x04, 0x00,1}, //// CNTR_B

    {0x08, 0x12,1}, //// PLL disable   //0x12 0x92
    {0x09, 0x04,1}, //// PLL setting for preview mode//0x01 0x04
    {0x0a, 0x04,1},   //// PLL setting for still mode0x01 0x04
    {0x10, 0x13,1},
    {0x11, 0x25,1},
    {0x12, 0x21,1},
    {0x13, 0x12,1},
    {0x14, 0x8d,1},

    {0x20, 0x00,1}, //// H/V blank    for preview mode
    {0x21, 0x24,1}, //// H   blank    for preview mode
    {0x22, 0x00,1}, //// frame fille  for preview mode
    {0x23, 0x11,1}, //// V   blank    for preview mode

    {0x24, 0x40,1}, //// H/V blank    for still mode
    {0x25, 0x34,1}, //// H   blank    for still mode
    {0x26, 0x00,1}, //// frame fille  for still mode
    {0x27, 0x11,1}, //// V   blank    for still mode

    {0x00, 0x01,1},
    {0x34, 0x19,1},
    {0x35, 0x05,1},
    {0x36, 0x0d,1},
    {0x37, 0x40,1},

//////// Sensor Setting : AE //////////////
    {0x00, 0x01,1}, //// Block Select
    {0x10, 0x8e,1},
    {0x11, 0x0c,1},
    {0x12, 0x70,1},
    {0x14, 0x60,1},
    {0x16, 0x40,1},
    {0x19, 0x00,1},
    {0x1c, 0x02,1},

    {0x34, 0x17,1},
    {0x35, 0x03,1},
    {0x36, 0x08,1},

    {0x40, 0x28,1},
    {0x6d, 0x01,1},

//////// Sensor Setting : AWB //////////////
    {0x00, 0x02,1}, //// Block Select
    {0x10, 0xd3,1}, ////  IPFUN,

    {0x11, 0xc1,1},
    {0x13, 0x7c,1},
    {0x14, 0x7c,1},
    {0x15, 0xfe,1}, // R gain Top
    {0x16, 0x98,1}, // R gain bottom
    {0x17, 0xb0,1}, // B gain Top
    {0x18, 0x70,1}, // B gain bottom 0x80
    {0x19, 0x98,1}, // Cr top value 0x90
    {0x1a, 0x67,1}, // Cr bottom value 0x70
    {0x1b, 0x94,1}, // Cb top value 0x90
    {0x1c, 0x70,1}, // Cb bottom value 0x70
    {0x1d, 0xb0,1}, // 0xa0
    {0x1e, 0x70,1}, // 0x60
    {0x20, 0xe8,1}, // AWB luminous top value
    {0x21, 0x30,1}, // AWB luminous bottom value 0x20
    {0x22, 0xa4,1},
    {0x23, 0x20,1},
    {0x25, 0x20,1},
    {0x26, 0x0f,1},
    {0x27, 0x04,1},  //0x60 // BRTSRT
    {0x28, 0xa8,1}, // BRTRGNTOP result 0xc9
    {0x29, 0xa8,1}, // BRTRGNBOT
    {0x2a, 0x80,1}, // BRTBGNTOP result 0x9c
    {0x2b, 0x80,1},      // BRTBGNBOT
    {0x2c, 0x04,1},      // GAINCONT

    {0x30 ,0x0a,1},      //AWBWINHS
    {0x31 ,0x1a,1},      //AWBWINVS
    {0x32 ,0xa6,1},      //AWBWINHE
    {0x33 ,0xc2,1},      //AWBWINVE

    {0x40 ,0x01,1},
    {0x41 ,0x04,1},
    {0x42 ,0x08,1},
    {0x43 ,0x10,1},
    {0x44 ,0x12,1},
    {0x45 ,0x35,1},
    {0x46 ,0x64,1},

    {0x62 ,0x80,1}, //0x88  // G gain

    {0x63 ,0xb0,1}, // R D30 to D20
    {0x64 ,0xaa,1}, // B D30 to D20
    {0x65 ,0xb0,1}, // R D20 to D30
    {0x66 ,0xaa,1}, // B D

//////// Sensor Setting : IDP //////////////
    {0x00, 0x03,1}, //// Block Select
    {0x10, 0xff,1}, ////  IPFUN,


#if defined(RGB565_OUTPUT)
    {0x12, 0x1B,1}, ////  OUTFMT, RGB565 mode;  0xB5  OUTFMT, YCbCr  mode; 0x80  OUTFMT, Bayer  mode
#else
    {0x12, 0x1d,1}, ////  OUTFMT, RGB565 mode;  0xB5  OUTFMT, YCbCr  mode; 0x80  OUTFMT, Bayer  mode
#endif

    {0x13, 0xf2,1}, ////  IPFUN2,
    {0x14, 0x85,1}, ////  IPFUN3,
    {0x15, 0x00,1},

//DPC
    {0x17, 0xa7,1},       //DCTRL
    {0x18, 0x00,1},       //NTHLD
    {0x19, 0x20,1},       //DSTRT
    {0x1a, 0x40,1},       //DSLOP

//filter setting
    {0x60, 0x48,1},       //filcntl
    {0x61, 0x0c,1},       //glpfth
    {0x62, 0xff,1},       //glpfslop
    {0x63, 0x10,1},       //glpfend
    {0x64, 0x0c,1},       //clpfth
    {0x65, 0xff,1},       //clpfslop
    {0x66, 0x10,1},       //clpfend
    {0x67, 0x10,1},       //lpfstrt
    {0x68, 0x44,1},       //glevel

//windowting 128x160
    {0xc0, 0x00,1},
    {0xc1, 0xb0,1},
    {0xc2, 0x00,1},
    {0xc3, 0xdc,1},

//Gamma
    {0x30, 0x00,1},     //0x0
    {0x31, 0x04,1},     //0x3
    {0x32, 0x0e,1}, //0xb
    {0x33, 0x20,1}, //0x1f
    {0x34, 0x44,1}, //0x43
    {0x35, 0x62,1}, //0x5f
    {0x36, 0x7a,1}, //0x74
    {0x37, 0x8e,1}, //0x85
    {0x38, 0x9e,1}, //0x94
    {0x39, 0xac,1}, //0xA2
    {0x3a, 0xb9,1}, //0xAF
    {0x3b, 0xce,1}, //0xC6
    {0x3c, 0xE0,1}, //0xDB
    {0x3d, 0xf2,1}, //0xEF
    {0x3e, 0xfA,1}, //0xF8
    {0x3f, 0xFF,1}, //0xFF

// SHADING
    {0x40, 0x13,1},    // SPOSIA
    {0x41, 0x33,1},    // SPOSIB
    {0x42, 0x31,1},    // SPOSIC
    {0x43, 0x11,1},    // SPOSID
    {0x44, 0x11,1},    // SPOSIE
    {0x45, 0x11,1},    // SPOSIF
    {0x46, 0x11,1},    // SPOSIG

    {0x47, 0x00,1},    // SHDHORR         left R gain[7:4], right R gain[3:0]
    {0x48, 0x00,1},    // SHDVERR         top R gain[7:4], bottom R gain[3:0]
    {0x49, 0x00,1},    // SHDHORG         left G gain[7:4], right Gr gain[3:0] 0x21
    {0x4a, 0x00,1},    // SHDVERG         top G gain[7:4], bottom Gr gain[3:0]
    {0x4d, 0x00,1},    // SHDHORB         left B gain[7:4], right B gain[3:0]
    {0x4e, 0x00,1},    // SHDVERB         top B gain[7:4], bottom B gain[3:0]
    {0x50, 0x5c,1},    // SHDCNTX         X-axis center
    {0x51, 0x78,1},    // SHDCNTY         Y-axis center

    {0x53, 0x00,1},    // SHDOFSR         Shading R Offset
    {0x54, 0x00,1},    // SHDOFSG         Shading G Offset
    {0x56, 0x00,1},    // SHDOFSB         Shading B Offset


    {0x70, 0x08,1},
// Color matrix (D65) - Daylight
    {0x71, 0x3d,1}, //0x40
    {0x72, 0xc6,1}, //0xb9
    {0x73, 0xfd,1}, //0x07
    {0x74, 0x0f,1}, //0x15
    {0x75, 0x21,1}, //0x21
    {0x76, 0x10,1}, //0x0a
    {0x77, 0xf9,1}, //0xf8
    {0x78, 0xba,1}, //0xc5
    {0x79, 0x4d,1}, //0x46

// Color matrix (D30) - CWF
    {0x7a, 0x3a,1}, //0x38
    {0x7b, 0xcd,1},  //0xbc
    {0x7c, 0xfa,1},  //0x0c
    {0x7d, 0x12,1},  //0x14
    {0x7e, 0x2a,1},  //0x1e
    {0x7f, 0x04,1},  //0x0e
    {0x80, 0xf2,1},  //0xf6
    {0x81, 0xb6,1},  //0xcd
    {0x82, 0x58,1},  //0x3d

    {0x8c, 0x08,1},  //CMASEL

//
    {0x90, 0x30,1}, // Green upper gain 0x18
    {0x91, 0x30,1}, // Green down gain 0x18
    {0x92, 0x04,1}, // upper coring value 0x00
    {0x93, 0x08,1}, // upper coring value 0x00
    {0x96, 0x04,1}, // down coring value 0x00
    {0x9a, 0x20,1},
    {0x9b, 0x20,1}, // upper coring value 0x00
    {0x9c, 0x08,1},
    {0x9d, 0x30,1}, // upper coring value 0x00

    {0x9f, 0x20,1}, // Y upper gain 0x10
    {0xa0, 0x20,1}, // Y down gain 0x10
    {0xa1, 0x02,1}, // Y upper coring 0x02
    {0xa2, 0x02,1}, // Y down coring 0x02
    {0xa3, 0x20,1},
    {0xa4, 0x20,1},

// Color Effect
    {0xa9, 0x11,1}, // Cr saturation 0x12
    {0xaa, 0x12,1}, // Cb saturation 0x12
    {0xb9, 0x10,1}, // 0x20 lowlux color
    {0xba, 0x2a,1}, // 0x10 lowlux color (nightmode 0x00)

// Digital Gain Control
    {0xbe, 0x6c,1},

// LPF

    {0xd9, 0x08,1},
    {0xda, 0x10,1},
    {0xdb, 0x04,1},
    {0xde, 0x9a,1}, // NOIZCTRL 0xfa y edge off
    {0xe5, 0x40,1}, ////  MEMSPDA,
    {0xe6, 0x28,1},
    {0xe7, 0x04,1},
//{0xe8, 0x50,1},

    {0x5C, 0x01,1}, ////  FIFO_SIZE_H
    {0x5D, 0x00,1}, ////  FIFO_SIZE_L

    {0x5B, 0x86,1}, ////  FIFO_CNTR, for preview mode
    {0x5A, 0x80,1}, ////  SPI_CNTR,  for preview mode

    {0x00, 0x01,1}, //// Block Select
    {0x10, 0x8e,1},

//////// Sensor Setting : SNR //////////////
    {0x00, 0x00,1}, //// Block Select
    {0x03, 0x05,1}, //// CNTR_A

};


PRIVATE const CAM_REG_T RG_Reinit_QQVGA[] =
{
    {0x00, 0x00,1}, //// Block Select
    {0x04, 0x00,1}, //// CNTR_B

    {0x08, 0x12,1}, //// PLL disable   //0x12 0x92
    {0x09, 0x04,1}, //// PLL setting for preview mode//0x01 0x04
    {0x0a, 0x04,1},   //// PLL setting for still mode0x01 0x04
    {0x10, 0x13,1},
    {0x11, 0x25,1},
    {0x12, 0x21,1},
    {0x13, 0x12,1},
    {0x14, 0x8d,1},

    {0x20, 0x00,1}, //// H/V blank    for preview mode
    {0x21, 0x24,1}, //// H   blank    for preview mode
    {0x22, 0x00,1}, //// frame fille  for preview mode
    {0x23, 0x11,1}, //// V   blank    for preview mode

    {0x24, 0x40,1}, //// H/V blank    for still mode
    {0x25, 0x34,1}, //// H   blank    for still mode
    {0x26, 0x00,1}, //// frame fille  for still mode
    {0x27, 0x11,1}, //// V   blank    for still mode

    {0x00, 0x01,1},
    {0x34, 0x19,1},
    {0x35, 0x05,1},
    {0x36, 0x0d,1},
    {0x37, 0x40,1},

//////// Sensor Setting : AE //////////////
    {0x00, 0x01,1}, //// Block Select
    {0x10, 0x8e,1},
    {0x11, 0x0a,1},
    {0x12, 0x70,1},
    {0x14, 0x60,1},
    {0x16, 0x40,1},
    {0x19, 0x00,1},
    {0x1c, 0x02,1},

    {0x34, 0x17,1},
    {0x35, 0x03,1},
    {0x36, 0x08,1},

    {0x40, 0x28,1},
    {0x6d, 0x01,1},

//////// Sensor Setting : AWB //////////////
    {0x00, 0x02,1}, //// Block Select
    {0x10, 0xd3,1}, ////  IPFUN,

    {0x11, 0xc1,1},
    {0x13, 0x7c,1},
    {0x14, 0x7c,1},
    {0x15, 0xfe,1}, // R gain Top
    {0x16, 0x98,1}, // R gain bottom
    {0x17, 0xb0,1}, // B gain Top
    {0x18, 0x70,1}, // B gain bottom 0x80
    {0x19, 0x98,1}, // Cr top value 0x90
    {0x1a, 0x67,1}, // Cr bottom value 0x70
    {0x1b, 0x94,1}, // Cb top value 0x90
    {0x1c, 0x70,1}, // Cb bottom value 0x70
    {0x1d, 0xb0,1}, // 0xa0
    {0x1e, 0x70,1}, // 0x60
    {0x20, 0xe8,1}, // AWB luminous top value
    {0x21, 0x30,1}, // AWB luminous bottom value 0x20
    {0x22, 0xa4,1},
    {0x23, 0x20,1},
    {0x25, 0x20,1},
    {0x26, 0x0f,1},
    {0x27, 0x04,1},  //0x60 // BRTSRT
    {0x28, 0xa8,1}, // BRTRGNTOP result 0xc9
    {0x29, 0xa8,1}, // BRTRGNBOT
    {0x2a, 0x80,1}, // BRTBGNTOP result 0x9c
    {0x2b, 0x80,1},      // BRTBGNBOT
    {0x2c, 0x04,1},      // GAINCONT

    {0x30 ,0x0a,1},      //AWBWINHS
    {0x31 ,0x1a,1},      //AWBWINVS
    {0x32 ,0xa6,1},      //AWBWINHE
    {0x33 ,0xc2,1},      //AWBWINVE

    {0x40 ,0x01,1},
    {0x41 ,0x04,1},
    {0x42 ,0x08,1},
    {0x43 ,0x10,1},
    {0x44 ,0x12,1},
    {0x45 ,0x35,1},
    {0x46 ,0x64,1},

    {0x62 ,0x80,1}, //0x88  // G gain

    {0x63 ,0xb0,1}, // R D30 to D20
    {0x64 ,0xaa,1}, // B D30 to D20
    {0x65 ,0xb0,1}, // R D20 to D30
    {0x66 ,0xaa,1}, // B D

//////// Sensor Setting : IDP //////////////
    {0x00, 0x03,1}, //// Block Select
    {0x10, 0xff,1}, ////  IPFUN,
#if defined(RGB565_OUTPUT)
    {0x12, 0x1B,1}, ////  OUTFMT, RGB565 mode;  0xB5  OUTFMT, YCbCr  mode; 0x80  OUTFMT, Bayer  mode
#else
    {0x12, 0x1d,1}, ////  OUTFMT, RGB565 mode;  0xB5  OUTFMT, YCbCr  mode; 0x80  OUTFMT, Bayer  mode
#endif
    {0x13, 0xf2,1}, ////  IPFUN2,
    {0x14, 0x85,1}, ////  IPFUN3,
    {0x15, 0x00,1},

//DPC
    {0x17, 0xa7,1},       //DCTRL
    {0x18, 0x00,1},       //NTHLD
    {0x19, 0x20,1},       //DSTRT
    {0x1a, 0x40,1},       //DSLOP

//filter setting
    {0x60, 0x48,1},       //filcntl
    {0x61, 0x0c,1},       //glpfth
    {0x62, 0xff,1},       //glpfslop
    {0x63, 0x10,1},       //glpfend
    {0x64, 0x0c,1},       //clpfth
    {0x65, 0xff,1},       //clpfslop
    {0x66, 0x10,1},       //clpfend
    {0x67, 0x10,1},       //lpfstrt
    {0x68, 0x44,1},       //glevel

//windowting 128x160
    {0xc0, 0x08,1},
    {0xc1, 0xa0,1},
    {0xc2, 0x32,1},
    {0xc3, 0x78,1},

//Gamma
    {0x30, 0x00,1},     //0x0
    {0x31, 0x04,1},     //0x3
    {0x32, 0x0e,1}, //0xb
    {0x33, 0x20,1}, //0x1f
    {0x34, 0x44,1}, //0x43
    {0x35, 0x62,1}, //0x5f
    {0x36, 0x7a,1}, //0x74
    {0x37, 0x8e,1}, //0x85
    {0x38, 0x9e,1}, //0x94
    {0x39, 0xac,1}, //0xA2
    {0x3a, 0xb9,1}, //0xAF
    {0x3b, 0xce,1}, //0xC6
    {0x3c, 0xE0,1}, //0xDB
    {0x3d, 0xf2,1}, //0xEF
    {0x3e, 0xfA,1}, //0xF8
    {0x3f, 0xFF,1}, //0xFF

// SHADING
    {0x40, 0x13,1},    // SPOSIA
    {0x41, 0x33,1},    // SPOSIB
    {0x42, 0x31,1},    // SPOSIC
    {0x43, 0x11,1},    // SPOSID
    {0x44, 0x11,1},    // SPOSIE
    {0x45, 0x11,1},    // SPOSIF
    {0x46, 0x11,1},    // SPOSIG

    {0x47, 0x00,1},    // SHDHORR         left R gain[7:4], right R gain[3:0]
    {0x48, 0x00,1},    // SHDVERR         top R gain[7:4], bottom R gain[3:0]
    {0x49, 0x00,1},    // SHDHORG         left G gain[7:4], right Gr gain[3:0] 0x21
    {0x4a, 0x00,1},    // SHDVERG         top G gain[7:4], bottom Gr gain[3:0]
    {0x4d, 0x00,1},    // SHDHORB         left B gain[7:4], right B gain[3:0]
    {0x4e, 0x00,1},    // SHDVERB         top B gain[7:4], bottom B gain[3:0]
    {0x50, 0x5c,1},    // SHDCNTX         X-axis center
    {0x51, 0x78,1},    // SHDCNTY         Y-axis center

    {0x53, 0x00,1},    // SHDOFSR         Shading R Offset
    {0x54, 0x00,1},    // SHDOFSG         Shading G Offset
    {0x56, 0x00,1},    // SHDOFSB         Shading B Offset


    {0x70, 0x08,1},
// Color matrix (D65) - Daylight
    {0x71, 0x3d,1}, //0x40
    {0x72, 0xc6,1}, //0xb9
    {0x73, 0xfd,1}, //0x07
    {0x74, 0x0f,1}, //0x15
    {0x75, 0x21,1}, //0x21
    {0x76, 0x10,1}, //0x0a
    {0x77, 0xf9,1}, //0xf8
    {0x78, 0xba,1}, //0xc5
    {0x79, 0x4d,1}, //0x46

// Color matrix (D30) - CWF
    {0x7a, 0x3a,1}, //0x38
    {0x7b, 0xcd,1},  //0xbc
    {0x7c, 0xfa,1},  //0x0c
    {0x7d, 0x12,1},  //0x14
    {0x7e, 0x2a,1},  //0x1e
    {0x7f, 0x04,1},  //0x0e
    {0x80, 0xf2,1},  //0xf6
    {0x81, 0xb6,1},  //0xcd
    {0x82, 0x58,1},  //0x3d

    {0x8c, 0x08,1},  //CMASEL

//
    {0x90, 0x30,1}, // Green upper gain 0x18
    {0x91, 0x30,1}, // Green down gain 0x18
    {0x92, 0x04,1}, // upper coring value 0x00
    {0x93, 0x08,1}, // upper coring value 0x00
    {0x96, 0x04,1}, // down coring value 0x00
    {0x9a, 0x20,1},
    {0x9b, 0x20,1}, // upper coring value 0x00
    {0x9c, 0x08,1},
    {0x9d, 0x30,1}, // upper coring value 0x00

    {0x9f, 0x20,1}, // Y upper gain 0x10
    {0xa0, 0x20,1}, // Y down gain 0x10
    {0xa1, 0x02,1}, // Y upper coring 0x02
    {0xa2, 0x02,1}, // Y down coring 0x02
    {0xa3, 0x20,1},
    {0xa4, 0x20,1},

// Color Effect
    {0xa9, 0x11,1}, // Cr saturation 0x12
    {0xaa, 0x12,1}, // Cb saturation 0x12
    {0xb9, 0x10,1}, // 0x20 lowlux color
    {0xba, 0x2a,1}, // 0x10 lowlux color (nightmode 0x00)

// Digital Gain Control
    {0xbe, 0x6c,1},

// LPF

    {0xd9, 0x08,1},
    {0xda, 0x10,1},
    {0xdb, 0x04,1},
    {0xde, 0x9a,1}, // NOIZCTRL 0xfa y edge off
    {0xe5, 0x40,1}, ////  MEMSPDA,
    {0xe6, 0x28,1},
    {0xe7, 0x04,1},
//{0xe8, 0x50,1},

    {0x5C, 0x01,1}, ////  FIFO_SIZE_H
    {0x5D, 0x00,1}, ////  FIFO_SIZE_L

    {0x5B, 0x86,1}, ////  FIFO_CNTR, for preview mode
    {0x5A, 0x80,1}, ////  SPI_CNTR,  for preview mode

    {0x00, 0x01,1}, //// Block Select
    {0x10, 0x8e,1},

//////// Sensor Setting : SNR //////////////
    {0x00, 0x00,1}, //// Block Select
    {0x03, 0x05,1}, //// CNTR_A
};


// use this for 160*120 (QQVGA) capture
PRIVATE const CAM_REG_T RG_Reinit_QVGA[] =
{
};


PRIVATE const CAM_REG_T RG_Reinit_MASTER1[] =
{
};

PRIVATE const CAM_REG_T RG_Reinit_MASTER2_1[] =
{
};

PRIVATE const CAM_REG_T RG_Reinit_MASTER2_2[] =
{
};

PRIVATE const CAM_REG_T RG_Reinit_MASTER2_4[] =
{

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
    {};


PRIVATE const CAM_REG_T RG_VidFormatQqvga[] =
    {};

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
    "siq310a",      // char desc[40];

    0,                      // UINT8 snrtype; (rgb)
    1,                      // UINT8 pclk; use or don't use PCLK of sensor???
    0,                      // UINT8 clkcfg; ???
    HAL_I2C_BPS_100K,       // UINT16 i2crate;
    0x31,                   // UINT8 i2caddress; this is the _REAL_ slave address.
    //  The value written in phase 1 of a write cycle will be 0x42
    //  while the value just before the read will be 0x43
    0x31,                   // UINT8 i2cispaddress;
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


