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
#define REG98 0x88
#define REG9e 0x84
#define REG9f 0x7c
#define REG97 0x78
#define REG9a 0x84
#define REGa0 0x80
#define REGa1 0x78
#define REG99 0x74
PRIVATE const CAM_REG_T RG_Init_PowerOn[] =
{
//19.5M 2bit 8-14fps
    {0xfd,0x00,1},
    {0x1c,0x00,1},
//spi
//{0x2e,0x71,1},    //d0->d7
    {0x2e,0x51,1},  //d7->d0
    {0x2c,0x08,1},
    {0x2b,0x0a,1},
    {0x2d,0x40,1},
    {0x30,0x04,1},
    {0x31,0x00,1},
#if 1// YUV_OUTPUT
// YUV
    {0x35, 0x00, 1},
    {0x3f, 0x00, 1},
#else
// RGB565
    {0x35,0x04,1},
    {0x3f, 0x01, 1},
#endif
//analog  for 2bit
    {0x0f,0x0c,1},
    {0x10,0x0c,1},
    {0x12,0x0a,1},
    {0x13,0x0e,1},
    {0x15,0x08,1},
    {0x16,0x00,1},
    {0x1a,0xa8,1},
    {0x27,0x88,1},
    {0x28,0x0b,1},
    {0x32,0x00,1},
    {0x29,0x53,1},


//AE 19.5M 4分频 for 2bit 14-8fps
    {0x05,0x00,1},
    {0x06,0x00,1},
    {0x09,0x01,1},
    {0x0a,0x07,1},
//base
    {0x9b,0x2f,1},
    {0x9c,0x00,1},
//exp
    {0xa2,0x34,1},
    {0xa3,0x02,1},
    {0xa4,0x2f,1},
    {0xa5,0x00,1},
    {0xa8,0x2f,1},
    {0xa9,0x00,1},
    {0xaa,0x01,1},
    {0xab,0x00,1},

//pregain
    {0x4b,0x0c,1},
    {0x4c,0x0c,1},
    {0x4d,0x0c,1},
    {0x4e,0x0c,1},

//AWB
    {0xc5,0xc4,1},
    {0xc6,0x9f,1},
    {0xb3,0x7e,1},
    {0xb5,0x10,1},
    {0xd4,0xff,1},
    {0xba,0x2d,1},
    {0xd7,0x90,1},
    {0xd8,0xa0,1},
    {0xd9,0xff,1},
    {0xdd,0xa6,1},
    {0xe1,0xc7,1},
    {0xe3,0xd0,1},
    {0xd5,0xe0,1},

//smooth
    {0x42,0x54,1},
    {0x46,0x0a,1},

//sharpen
    {0x50,0x40,1},
    {0x51,0x10,1},
    {0x52,0xe0,1},
    {0x56,0x30,1},
    {0x57,0x20,1},
//color 艳丽偏红
    {0x79,0x80,1},
    {0x7a,0x00,1},
    {0x7b,0x00,1},
    {0x7c,0xf3,1},
    {0x7d,0x8e,1},
    {0x7e,0x00,1},
    {0x7f,0x00,1},
    {0x80,0xe6,1},
    {0x81,0x9a,1},
    {0x82,0x00,1},
    {0x83,0x03,1},
    {0x84,0x0c,1},
//sat
    {0x4f,0x80,1},
    {0x58,0xff,1},
    {0x59,0xff,1},

//heq
    {0x5b,0x80,1},
    {0x5c,0xa0,1},
//auto lum lowlight
    {0x86,0x20,1},
    {0x87,0x1f,1},
    {0x88,0x15,1},
    {0x89,0x45,1},

//gain
    {0xa6,0x70,1},
    {0xa7,0x2a,1},
    {0xac,0x50,1},
    {0xad,0x2a,1},
    {0x8a,0x50,1},
    {0x8b,0x3e,1},
    {0x8c,0x36,1},
    {0x8d,0x30,1},
    {0x8e,0x30,1},
    {0x8f,0x2e,1},
    {0x90,0x2e,1},
    {0x91,0x2c,1},
    {0x92,0x2c,1},
    {0x93,0x2c,1},
    {0x94,0x2a,1},
    {0x95,0x2a,1},
    {0x96,0x2a,1},
//target
    {0x98,REG98,1},//0x80,
    {0x9e,REG9e,1},//0x7c,
    {0x9f,REG9f,1},//0x74,
    {0x97,REG97,1},//0x70,
    {0x9a,REG9a,1},//0x80,
    {0xa0,REGa0,1},//0x7c,
    {0xa1,REGa1,1},//0x74,
    {0x99,REG99,1},//0x70,
//gama
    {0x65,0x00,1},
    {0x66,0x0c,1},
    {0x67,0x19,1},
    {0x68,0x2c,1},
    {0x69,0x49,1},
    {0x6a,0x61,1},
    {0x6b,0x77,1},
    {0x6c,0x8a,1},
    {0x6d,0x9b,1},
    {0x6e,0xa9,1},
    {0x6f,0xb5,1},
    {0x70,0xc0,1},
    {0x71,0xca,1},
    {0x72,0xd4,1},
    {0x73,0xdd,1},
    {0x74,0xe6,1},
    {0x75,0xef,1},
    {0x76,0xf7,1},
    {0x77,0xff,1},
    {0x9d,0x09,1},
    {0x33,0x0f,1},
    {0x34,0x06,1},
    {0x32,0x0d,1},
    {0xff,0xff,1},

    // test pattern
    //{0x0d, 0x14, 1}
};

// 128 * 160, for camera preview
PRIVATE const CAM_REG_T RG_Reinit_QQVGA[] =
{
//SP0829 无需填写
};

// 320 * 240, for capture
PRIVATE const CAM_REG_T RG_Reinit_QVGA[] =
{
//SP0829 无需填写
};

// specific to video recorder, 160*120
PRIVATE const CAM_REG_T RG_Reinit_QQVGA_2[] =
{
//SP0829 无需填写
};

PRIVATE const CAM_REG_T RG_Reinit_MASTER1[] = // 1-bit 带SSN模式
{
    {0xfd,0x00,1},
    {0x1c,0x00,1},
//spi
    {0x2e,0x00,1},
    {0x2c,0x0b,1},
    {0x2b,0x0b,1},
    {0x2d,0x0b,1},
    {0x30,0x06,1},
    {0x31,0x00,1},
    {0x35,0x40,1},
//analog  for 1bit
    {0x0f,0x06,1},
    {0x10,0x06,1},
    {0x12,0x05,1},
    {0x13,0x07,1},
    {0x15,0x04,1},
    {0x16,0x00,1},

    {0x1a,0xa8,1},
    {0x27,0x88,1},
    {0x28,0x0b,1},

    {0x32,0x00,1},
    {0x29,0x53,1},


//AE 19.5M 8分频 for 1bit 12-8fps
//ae gain
    {0xa6 , 0x70,1},
    {0xac , 0x70,1},
//ae setting
    {0x05 , 0x0 ,1},
    {0x06 , 0x0 ,1},
    {0x09 , 0x0 ,1},
    {0x0a , 0x29,1},
//base
    {0x9b , 0x29,1},
    {0x9c , 0x0 ,1},
//exp
    {0xa2 , 0xec,1},
    {0xa3 , 0x01,1},
    {0xa4 , 0x29,1},
    {0xa5 , 0x0 ,1},
    {0xa8 , 0x29,1},
    {0xa9 , 0x0 ,1},
    {0xaa , 0x1 ,1},
    {0xab , 0x0 ,1},
};


PRIVATE const CAM_REG_T RG_Reinit_MASTER2_1[] =// 1-bit不带SSN
{
    {0xfd,0x00,1},
    {0x1c,0x00,1},
//spi
    {0x2e, 0x50,1},
    {0x30, 0x06,1},//0x06//0x16//PCLCK 不分频  ISP 8分频
    {0x34, 0x00,1},
    {0x2b, 0x0a,1},
    {0x2c, 0x01,1},//0X05
#if 1// YUV_OUTPUT
// YUV
    {0x35, 0x00, 1},
    {0x3f, 0x00, 1},
#else
// RGB565
    {0x35,0x04,1},
    {0x3f,0x01,1},
#endif
    {0x31, 0x00,1},

//analog  for 1bit
    {0x0f,0x06,1},
    {0x10,0x06,1},
    {0x12,0x05,1},
    {0x13,0x07,1},
    {0x15,0x04,1},
    {0x16,0x00,1},
    {0x1a,0xa8,1},
    {0x27,0x88,1},
    {0x28,0x0b,1},
    {0x32,0x0d,1},
    {0x29,0x53,1},


//AE 19.5M 8分频 for 1bit 12-8fps
//ae gain
    {0xa6 , 0x70,1},
    {0xac , 0x70,1},
//ae setting
    {0x05 , 0x0 ,1},
    {0x06 , 0x0 ,1},
    {0x09 , 0x0 ,1},
    {0x0a , 0x29,1},
//base
    {0x9b , 0x29,1},
    {0x9c , 0x0 ,1},
//exp
    {0xa2 , 0xec,1},
    {0xa3 , 0x01,1},
    {0xa4 , 0x29,1},
    {0xa5 , 0x0 ,1},
    {0xa8 , 0x29,1},
    {0xa9 , 0x0 ,1},
    {0xaa , 0x1 ,1},
    {0xab , 0x0 ,1},

};

PRIVATE const CAM_REG_T RG_Reinit_MASTER2_2[] = // 2-bit不带SSN
{
    {0xfd,0x00,1},
    {0x1c,0x00,1},
//spi
//{0x2e,0x71,1},    //0X11
    {0x2e,0x51,1},  //0X11
    {0x2c,0x08,1},
    {0x2b,0x0a,1},
    {0x2d,0x40,1},
    {0x30,0x04,1},
    {0x31,0x00,1},
#if 1// YUV_OUTPUT
// YUV
    {0x35, 0x00, 1},
    {0x3f, 0x00, 1},
#else
// RGB565
    {0x35,0x04,1},
    {0x3f, 0x01, 1},
#endif
//analog  for 2bit
    {0x0f,0x0c,1},
    {0x10,0x0c,1},
    {0x12,0x0a,1},
    {0x13,0x0e,1},
    {0x15,0x08,1},
    {0x16,0x00,1},
    {0x1a,0xa8,1},
    {0x27,0x88,1},
    {0x28,0x0b,1},
    {0x32,0x0d,1},
    {0x29,0x53,1},

//AE 19.5M 4分频 for 2bit 14-8fps
//ae gain
    {0xa6 , 0x70,1},
    {0xac , 0x70,1},
//ae setting
    {0x05 , 0x00,1},
    {0x06 , 0x00,1},
    {0x09 , 0x01,1},
    {0x0a , 0x07,1},
//base
    {0x9b , 0x2f,1},
    {0x9c , 0x00,1},
//exp
    {0xa2 , 0x34,1},
    {0xa3 , 0x02,1},
    {0xa4 , 0x2f,1},
    {0xa5 , 0x00,1},
    {0xa8 , 0x2f,1},
    {0xa9 , 0x00,1},
    {0xaa , 0x01,1},
    {0xab , 0x00,1},
};

PRIVATE const CAM_REG_T RG_Reinit_MASTER2_4[] = //// 4-bit不带SSN
{
    {0xfd,0x00,1},
    {0x1c,0x00,1},
//spi
    {0x2e,0x52,1},//0X12
    {0x2c,0x08,1},
    {0x2b,0x00,1},
    {0x2d,0x00,1},
    {0x30,0x02,1},
    {0x31,0x00,1},
#if 1// YUV_OUTPUT
// YUV
    {0x35, 0x00, 1},
    {0x3f, 0x00, 1},
#else
// RGB565
    {0x35,0x04,1},
    {0x3f, 0x01, 1},
#endif
//analog  for 4bit
    {0x0f,0x18,1},
    {0x10,0x18,1},
    {0x12,0x14,1},
    {0x13,0x1c,1},
    {0x15,0x0c,1},
    {0x16,0x00,1},

    {0x1a,0xa8,1},
    {0x27,0x88,1},
    {0x28,0x0b,1},

    {0x32,0x0d,1},
    {0x29,0x53,1},

//AE 19.5M 2分频 for 4bit 8-14fps
    {0xa6 , 0x70,1},
    {0xac , 0x70,1},
    {0x05 , 0x00,1},
    {0x06 , 0x00,1},
    {0x09 , 0x03,1},
    {0x0a , 0x0d,1},
    {0x9b , 0x2f,1},
    {0x9c , 0x00,1},
    {0xa2 , 0x34,1},
    {0xa3 , 0x02,1},
    {0xa4 , 0x2f,1},
    {0xa5 , 0x00,1},
    {0xa8 , 0x2f,1},
    {0xa9 , 0x00,1},
    {0xaa , 0x01,1},
    {0xab , 0x00,1},


};

#if 0
PRIVATE const CAM_REG_T RG_Reinit_SINGLE_WIRE[] =
{
};

PRIVATE const CAM_REG_T RG_Reinit_DUAL_WIRE[] =
{
};
#endif

PRIVATE const CAM_REG_T RG_Reinit_MODE_DEFAULT[] =
{
};

// use this for 640x480 (VGA) capture
PRIVATE const CAM_REG_T RG_InitPowerOnVga[] =
    {};


// use this for 160*120 (QQVGA) capture
PRIVATE const CAM_REG_T RG_InitPowerOnQqvga[] =
    {};

PRIVATE const CAM_REG_T RG_VidFormatVga[] =
{
};

PRIVATE const CAM_REG_T RG_VidFormatQvga[] =
{
};

PRIVATE const CAM_REG_T RG_VidFormatQqvga[] =
{
#if !defined(VIDREC_FAST_RECORDING)
    // 19.5MHz 2bit fix10 fps
    {0xa6 , 0x70,1},
    {0xac , 0x70,1},
    {0x05 , 0x0 ,1},
    {0x06 , 0x0 ,1},
    {0x09 , 0x01,1},
    {0x0a , 0xcd,1},
    {0x9b , 0x22,1},
    {0x9c , 0x0 ,1},
    {0xa2 , 0x54,1},
    {0xa3 , 0x1 ,1},
    {0xa4 , 0x22,1},
    {0xa5 , 0x0 ,1},
    {0xa8 , 0x22,1},
    {0xa9 , 0x0 ,1},
    {0xaa , 0x1 ,1},
    {0xab , 0x0 ,1},
#else
    // 19.5MHz 2bit fix15 fps
    {0xa6 , 0x70,1},
    {0xac , 0x70,1},
    {0x05 , 0x0 ,1},
    {0x06 , 0x0 ,1},
    {0x09 , 0x00,1},
    {0x0a , 0xde,1},
    {0x9b , 0x33,1},
    {0x9c , 0x0 ,1},
    {0xa2 , 0x32,1},
    {0xa3 , 0x1 ,1},
    {0xa4 , 0x33,1},
    {0xa5 , 0x0 ,1},
    {0xa8 , 0x33,1},
    {0xa9 , 0x0 ,1},
    {0xaa , 0x1 ,1},
    {0xab , 0x0 ,1},
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
    "SP0829",      // char desc[40];

    0,                      // UINT8 snrtype; (rgb)
    1,                      // UINT8 pclk; use or don't use PCLK of sensor???
    0,                      // UINT8 clkcfg; ???
    HAL_I2C_BPS_100K,       // UINT16 i2crate;
    0x43,                   // UINT8 i2caddress; this is the _REAL_ slave address.
    //  The value written in phase 1 of a write cycle will be 0x42
    //  while the value just before the read will be 0x43
    0x43,                   // UINT8 i2cispaddress;
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


