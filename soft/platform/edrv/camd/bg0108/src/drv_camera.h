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

// use this for 240x320 (QVGA) capture
PRIVATE const CAM_REG_T RG_InitPowerOnQvgainit[] =
{

    //
    {0xf0,0x00,1},
    {0x1c,0x01,1},
    {0x05,0x0c,1},
    {0x06,0x00,1},
    {0x07,0xf0,1},
    {0x08,0x01,1},
    {0x09,0x40,1},
    {0x0c,0x00,1},
    {0x0d,0xf0,1},
    {0x0e,0x00,1},
    {0x0f,0x00,1},
    {0x10,0x00,1},
    {0x11,0xf0,1},
    {0x0a,0x07,1},
    {0x0b,0x30,1},
    {0x21,0x00,1},
    {0x33,0x07,1},
    {0x34,0x0f,1},
    {0x3d,0x00,1},
    {0xb0,0x00,1},

    {0xb5,0x07,1},
    {0xb6,0x3f,1},
    {0xc6,0x03,1},

    {0xc4,0x1a,1},
    {0xf0,0x02,1},
    {0x2e,0x40,1}, //ofmode

    {0x20,0x07,1}, //YUVCTRL

    {0x21,0xa0,1}, //adjust saturation according to brightness
    {0x22,0x40,1},
    {0x23,0xff,1},
    {0x24,0x03,1},
    {0x25,0x00,1},

    {0x26,0xfe,1},
    {0x27,0x01,1},
    {0x28,0xfe,1},
    {0x29,0x01,1},
    {0x2a,0xfe,1},
    {0x2b,0x01,1},

    {0x40,0x00,1},  //gamma
    {0x41,0x60,1},
    {0x42,0x00,1},
    {0x43,0x60,1},
    {0x44,0x80,1},
    {0x45,0x74,1},
    {0x46,0x50,1},
    {0x47,0x50,1},
    {0x48,0x48,1},
    {0x49,0x48,1},
    {0x4a,0x40,1},
    {0x4b,0x40,1},
    {0x4c,0x3c,1},
    {0x4d,0x38,1},
    {0x4e,0x34,1},
    {0x4f,0x34,1},
    {0x50,0x30,1},
    {0x51,0x30,1},
    {0x52,0x00,1},

    {0x71,0x17,1},

    {0x73,0x01,1}, //g1g2 diff&edge
    {0x74,0x0a,1}, //g1g2 diff th
    {0x76,0x0a,1},
    {0x77,0x08,1},
    {0x7b,0x00,1},

    {0x90,0x03,1}, //CCMODE
    {0x80,0x40,1}, //CCMAT
    {0x81,0x00,1},
    {0x82,0x00,1},
    {0x83,0xfc,1},
    {0x84,0x45,1},
    {0x85,0xfe,1},
    {0x86,0x03,1},
    {0x87,0x07,1},
    {0x88,0x34,1},
    {0x89,0x00,1},
    {0x8a,0x00,1},
    {0x8b,0x00,1},

    {0x8e,0x90,1}, //CON
    {0x8f,0x80,1}, //CON_CENTER
    {0x8c,0x3f,1}, //COS_HUE
    {0x8d,0x06,1}, //SIN_HUE

    {0x91,0x0e,1}, //auto mode
    {0x92,0xc0,1}, //SAT
    {0x93,0xc0,1}, //SAT
    {0x94,0x10,1},
    {0x95,0x20,1},
    {0x96,0x60,1},

    {0x98,0x10,1}, //ee start gain
    {0x99,0x20,1}, //ee end gain
    {0x97,0x05,1}, //ee offset
    {0x9a,0x02,1}, //ee slope

    {0x9b,0x01,1}, //Denoise r offset
    {0x9c,0x01,1}, //Denoise g offset
    {0x9d,0x01,1}, //Denoise b offset
    {0x9e,0x01,1}, //grgb offset
    {0x9f,0x10,1}, //Denoise start gain
    {0xa0,0x20,1}, //denoise end gain
    {0xa1,0x02,1}, //denoies slope
    {0xa2,0x00,1}, //grgb slope

    {0xf0,0x03,1},
    {0x00,0x00,1}, //AEC OFF
    {0x01,0x11,1}, //AEC CNT
    {0x02,0x70,1}, //AEC Target
    {0x03,0x08,1}, //range1
    {0x04,0x10,1}, //range2
    {0x05,0x18,1}, //range3
    {0x06,0x0c,1}, //MAX texp
    {0x07,0x04,1}, //MIN texp

    {0x0a,0x00,1},
    {0x0b,0x34,1}, //texp unit

    {0x0f,0x06,1}, //Texp LUT
    {0x10,0x08,1},
    {0x11,0x0a,1},
    {0x12,0x0c,1},
    {0x13,0x14,1},

#if 0 // max gain=3x
    {0x08,0x10,1}, //MAX gain
    {0x09,0x16,1}, //MIN gain

    {0x14,0x00,1}, //Gain LUT
    {0x15,0x2f,1}, //gain0

    {0x16,0x00,1},
    {0x17,0x2b,1}, //gain4

    {0x18,0x00,1},
    {0x19,0x2b,1}, //gain5

    {0x1a,0x00,1},
    {0x1b,0x2b,1}, //gain6

    {0x1c,0x00,1},
    {0x1d,0x40,1}, //gain7

    {0x1e,0x00,1},
    {0x1f,0x40,1}, //gain8

    {0x20,0x00,1},
    {0x21,0x40,1}, //gain9
#endif

#if 1 // max gain=4x
    {0x08,0x15,1}, //MAX gain
    {0x09,0x16,1}, //MIN gain

    {0x14,0x00,1}, //Gain LUT
    {0x15,0x2f,1}, //gain0

    {0x16,0x00,1},
    {0x17,0x2b,1}, //gain4

    {0x18,0x00,1},
    {0x19,0x2b,1}, //gain5

    {0x1a,0x00,1},
    {0x1b,0x2b,1}, //gain6

    {0x1c,0x00,1},
    {0x1d,0x56,1}, //gain7

    {0x1e,0x00,1},
    {0x1f,0x56,1}, //gain8

    {0x20,0x00,1},
    {0x21,0x56,1}, //gain9
#endif

#if 0  //max gain=5x
    {0x08,0x1a,1}, //MAX gain
    {0x09,0x16,1}, //MIN gain

    {0x14,0x00,1}, //Gain LUT
    {0x15,0x2f,1}, //gain0

    {0x16,0x00,1},
    {0x17,0x2b,1}, //gain4

    {0x18,0x00,1},
    {0x19,0x2b,1}, //gain5

    {0x1a,0x00,1},
    {0x1b,0x2b,1}, //gain6

    {0x1c,0x00,1},
    {0x1d,0x68,1}, //gain7

    {0x1e,0x00,1},
    {0x1f,0x68,1}, //gain8

    {0x20,0x00,1},
    {0x21,0x68,1}, //gain9

#endif

#if 0  //max gain=6x
    {0x08,0x20,1}, //MAX gain
    {0x09,0x16,1}, //MIN gain

    {0x14,0x00,1}, //Gain LUT
    {0x15,0x2f,1}, //gain0

    {0x16,0x00,1},
    {0x17,0x2b,1}, //gain4

    {0x18,0x00,1},
    {0x19,0x2b,1}, //gain5

    {0x1a,0x00,1},
    {0x1b,0x2b,1}, //gain6

    {0x1c,0x00,1},
    {0x1d,0x80,1}, //gain7

    {0x1e,0x00,1},
    {0x1f,0x80,1}, //gain8

    {0x20,0x00,1},
    {0x21,0x80,1}, //gain9


#endif



    {0x22,0x8a,1}, //exp_cal
    {0x23,0xcc,1},
    {0x24,0xdc,1},
    {0x25,0xcd,1},
    {0x26,0x09,1},

    {0x27,0x2c,1},

    {0x00,0xd1,1}, //AEC ON

    {0x60,0x00,1}, //AWB Off

    {0x6d,0x10,1},

#if 0 //awb opt for office

    {0x6e,0x40,1},
    {0x6f,0x43,1},
    {0x70,0x23,1},
    {0x71,0x25,1},

    {0x72,0x2a,1},
    {0x73,0x34,1},
    {0x74,0x2a,1},
    {0x75,0x2f,1},

    {0x76,0x20,1},
    {0x77,0x23,1},
    {0x78,0x38,1},
    {0x79,0x43,1},
#endif

    {0x7a,0x04,1},
    {0x7b,0x7f,1},

    {0x60,0x1d,1}, //AWB On

    {0xf0,0x00,1},
    {0xb4,0x00,1},
    {0x1d,0x01,1},


};

// 128 * 160, for camera preview
PRIVATE const CAM_REG_T RG_Reinit_QQVGA[] =
{

};

// 320 * 240, for capture
PRIVATE const CAM_REG_T RG_Reinit_QVGA[] =
{
};

// specific to video recorder, 160*120
PRIVATE const CAM_REG_T RG_Reinit_QQVGA_2[] =
{

};

PRIVATE const CAM_REG_T RG_Reinit_MASTER1[] =
{
#if 0
    {0xf0, 0x00, 1},
    {0xb0, 0x00, 1},
    {0xc4, 0x1a, 1},
#endif

};


PRIVATE const CAM_REG_T RG_Reinit_MASTER2_1[] =
{
#if 0
    {0xf0, 0x00, 1},
    {0x0a, 0x07, 1},
    {0x0b, 0x80, 1},
    {0xb0, 0x00, 1},
    {0xb4, 0x00, 1},
    {0xc4, 0x1a, 1},
    {0xc6, 0x03, 1},
    {0xf0, 0x02, 1},


#endif

};

PRIVATE const CAM_REG_T RG_Reinit_MASTER2_2[] =
{
};

PRIVATE const CAM_REG_T RG_Reinit_MASTER2_4[] =
{
};


PRIVATE const CAM_REG_T RG_Reinit_MODE_DEFAULT[] =
{
#if 0
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
#else
#if 0
    {0xf0, 0x00, 1},
    {0xb0, 0x00, 1},
    {0xc4, 0x04, 1},
#endif

#endif
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
#if 0
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
    "BG0108",      // char desc[40];

    0,                      // UINT8 snrtype; (rgb)
    1,                      // UINT8 pclk; use or don't use PCLK of sensor???
    0,                      // UINT8 clkcfg; ???
    HAL_I2C_BPS_100K,       // UINT16 i2crate;
    0x52,                   // UINT8 i2caddress; this is the _REAL_ slave address.
    //  The value written in phase 1 of a write cycle will be 0x42
    //  while the value just before the read will be 0x43
    0x52,                   // UINT8 i2cispaddress;
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

