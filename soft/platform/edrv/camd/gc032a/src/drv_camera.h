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

#define __USE_VGA_DEF__

//#define  DRV_CAM_FRAME_6    1
#define DRV_CAM_FRAME_10 1
//#define  DRV_CAM_FRAME_12    1
typedef struct
{
    UINT8 Addr; // Register Address
    UINT8 Data; // Register Data
    UINT8 Wait; // Time to wait after write
} CAM_REG_T;

typedef struct
{
    UINT16 Len;        // Size of array
    CAM_REG_T *RegVal; // Array of registers
} CAM_REG_GROUP_T;

typedef enum {
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

//SDR
PRIVATE const CAM_REG_T RG_InitPowerOnVga[] =
    {
        /*System*/

        {0xf3, 0x83, 1},
        {0xf5, 0x0c, 1},
        {0xf7, 0x01, 1},
		{0xf8,0x03,1},
        {0xf9, 0x4e, 1},
        {0xfa, 0x10, 1},
        {0xfc, 0x02, 1},
        {0xfe, 0x02, 1},
        {0x81, 0x03, 1},

        /*Analog&Cisctl*/

        {0xfe, 0x00, 1},
        {0x03, 0x01, 1},
        {0x04, 0xc2, 1},
        {0x05, 0x01, 1},
		{0x06,0xad,1}, 
        {0x07, 0x00, 1},
        {0x08, 0x08, 1},
        {0x0a, 0x04, 1},
        {0x0c, 0x04, 1},
        {0x0d, 0x01, 1},
        {0x0e, 0xe8, 1},
        {0x0f, 0x02, 1},
        {0x10, 0x88, 1},
        {0x17, 0x54, 1},
        {0x19, 0x04, 1},
        {0x1a, 0x0a, 1},
        {0x1f, 0x40, 1},
        {0x20, 0x30, 1},
        {0x2e, 0x80, 1},
        {0x2f, 0x2b, 1},
        {0x30, 0x1a, 1},
        {0xfe, 0x02, 1},
        {0x03, 0x02, 1},
        {0x05, 0xd7, 1},
        {0x06, 0x60, 1},
        {0x12, 0x89, 1},

        /*SPI*/

        {0xfe, 0x03, 1},
        {0x51, 0x01, 1},
        {0x52, 0xda, 1}, // 5a
        {0x53, 0x24, 1},
        {0x54, 0x20, 1},
        {0x55, 0x00, 1},
        {0x59, 0x10, 1},
        {0x5a, 0x00, 1},
        {0x5b, 0x80, 1},
        {0x5c, 0x02, 1},
        {0x5d, 0xe0, 1},
        {0x5e, 0x01, 1},
        {0x6a, 0x10, 1},

        // add

        //{0x60, 0x01, 1},  //sync code0
        //{0x61, 0x02, 1},  //sync code1
        //{0x62, 0x40, 1},  //sync code2
        //{0x63, 0x00, 1},  //sync code3
        //sck_always?bit656?
        {0x64, 0x00, 1}, //sync code3

        /*blk*/
        {0xfe, 0x00, 1},
        {0x18, 0x02, 1},
        {0xfe, 0x02, 1},
        {0x40, 0x22, 1},
        {0x45, 0x00, 1},
        {0x46, 0x00, 1},
        {0x49, 0x20, 1},
        {0x4b, 0x3c, 1},
        {0x50, 0x20, 1},
        {0x42, 0x10, 1},

        /*isp*/

        {0xfe, 0x01, 1},
        {0x0a, 0xc5, 1},
        {0x45, 0x00, 1},
        {0xfe, 0x00, 1},
        {0x40, 0xff, 1},
        {0x41, 0x25, 1},
        {0x42, 0xcf, 1},
        {0x43, 0x10, 1},
        {0x44, 0x80, 1},
        {0x46, 0x26, 1},
        {0x49, 0x03, 1},
        {0x4f, 0x01, 1},
        {0xde, 0x84, 1},
        {0xfe, 0x02, 1},
        {0x22, 0xf6, 1},

        /*Shading*/

        {0xfe, 0x01, 1},
        {0xc1, 0x3c, 1},
        {0xc2, 0x50, 1},
        {0xc3, 0x00, 1},
        {0xc4, 0x32, 1},
        {0xc5, 0x24, 1},
        {0xc6, 0x16, 1},
        {0xc7, 0x08, 1},
        {0xc8, 0x08, 1},
        {0xc9, 0x00, 1},
        {0xca, 0x20, 1},
        {0xdc, 0x8a, 1},
        {0xdd, 0xa0, 1},
        {0xde, 0xa6, 1},
        {0xdf, 0x75, 1},

        /*AWB*/

        {0xfe, 0x01, 1},
        {0x7c, 0x09, 1},
        {0x65, 0x06, 1},
        {0x7c, 0x08, 1},
        {0x56, 0xf4, 1},
        {0x66, 0x0f, 1},
        {0x67, 0x84, 1},
        {0x6b, 0x80, 1},
        {0x6d, 0x12, 1},
        {0x6e, 0xb0, 1},
        {0x86, 0x00, 1},
        {0x87, 0x00, 1},
        {0x88, 0x00, 1},
        {0x89, 0x00, 1},
        {0x8a, 0x00, 1},
        {0x8b, 0x00, 1},
        {0x8c, 0x00, 1},
        {0x8d, 0x00, 1},
        {0x8e, 0x00, 1},
        {0x8f, 0x00, 1},
        {0x90, 0xef, 1},
        {0x91, 0xe1, 1},
        {0x92, 0x0c, 1},
        {0x93, 0xef, 1},
        {0x94, 0x65, 1},
        {0x95, 0x1f, 1},
        {0x96, 0x0c, 1},
        {0x97, 0x2d, 1},
        {0x98, 0x20, 1},
        {0x99, 0xaa, 1},
        {0x9a, 0x3f, 1},
        {0x9b, 0x2c, 1},
        {0x9c, 0x5f, 1},
        {0x9d, 0x3e, 1},
        {0x9e, 0xaa, 1},
        {0x9f, 0x67, 1},
        {0xa0, 0x60, 1},
        {0xa1, 0x00, 1},
        {0xa2, 0x00, 1},
        {0xa3, 0x0a, 1},
        {0xa4, 0xb6, 1},
        {0xa5, 0xac, 1},
        {0xa6, 0xc1, 1},
        {0xa7, 0xac, 1},
        {0xa8, 0x55, 1},
        {0xa9, 0xc3, 1},
        {0xaa, 0xa4, 1},
        {0xab, 0xba, 1},
        {0xac, 0xa8, 1},
        {0xad, 0x55, 1},
        {0xae, 0xc8, 1},
        {0xaf, 0xb9, 1},
        {0xb0, 0xd4, 1},
        {0xb1, 0xc3, 1},
        {0xb2, 0x55, 1},
        {0xb3, 0xd8, 1},
        {0xb4, 0xce, 1},
        {0xb5, 0x00, 1},
        {0xb6, 0x00, 1},
        {0xb7, 0x05, 1},
        {0xb8, 0xd6, 1},
        {0xb9, 0x8c, 1},

        /*CC*/

        {0xfe, 0x01, 1},
        {0xd0, 0x40, 1}, //3a
        {0xd1, 0xf8, 1},
        {0xd2, 0x00, 1},
        {0xd3, 0xfa, 1},
        {0xd4, 0x45, 1},
        {0xd5, 0x02, 1},

        {0xd6, 0x30, 1},
        {0xd7, 0xfa, 1},
        {0xd8, 0x08, 1},
        {0xd9, 0x08, 1},
        {0xda, 0x58, 1},
        {0xdb, 0x02, 1},
        {0xfe, 0x00, 1},

        /*Gamma*/
        {0xfe, 0x00, 1},
        {0xba, 0x00, 1},
        {0xbb, 0x04, 1},
        {0xbc, 0x0a, 1},
        {0xbd, 0x0e, 1},
        {0xbe, 0x22, 1},
        {0xbf, 0x30, 1},
        {0xc0, 0x3d, 1},
        {0xc1, 0x4a, 1},
        {0xc2, 0x5d, 1},
        {0xc3, 0x6b, 1},
        {0xc4, 0x7a, 1},
        {0xc5, 0x85, 1},
        {0xc6, 0x90, 1},
        {0xc7, 0xa5, 1},
        {0xc8, 0xb5, 1},
        {0xc9, 0xc2, 1},
        {0xca, 0xcc, 1},
        {0xcb, 0xd5, 1},
        {0xcc, 0xde, 1},
        {0xcd, 0xea, 1},
        {0xce, 0xf5, 1},
        {0xcf, 0xff, 1},

        /*Auto Gamma*/
        {0xfe, 0x00, 1},
        {0x5a, 0x08, 1},
        {0x5b, 0x0f, 1},
        {0x5c, 0x15, 1},
        {0x5d, 0x1c, 1},
        {0x5e, 0x28, 1},
        {0x5f, 0x36, 1},
        {0x60, 0x45, 1},
        {0x61, 0x51, 1},
        {0x62, 0x6a, 1},
        {0x63, 0x7d, 1},
        {0x64, 0x8d, 1},
        {0x65, 0x98, 1},
        {0x66, 0xa2, 1},
        {0x67, 0xb5, 1},
        {0x68, 0xc3, 1},
        {0x69, 0xcd, 1},
        {0x6a, 0xd4, 1},
        {0x6b, 0xdc, 1},
        {0x6c, 0xe3, 1},
        {0x6d, 0xf0, 1},
        {0x6e, 0xf9, 1},
        {0x6f, 0xff, 1},

        /*Gain*/

        {0xfe, 0x00, 1},
        {0x70, 0x50, 1},

        /*AEC*/
        {0xfe, 0x00, 1},
        {0x4f, 0x01, 1},
        {0xfe, 0x01, 1},

        {0x44, 0x04, 1},
        {0x1f, 0x30, 1},
        {0x20, 0x40, 1},
		{0x26,0x4d,1},
        {0x27, 0x01, 1},
		{0x28,0xce,1}, 
        {0x29, 0x03, 1},
		{0x2a,0x02,1},
        {0x2b, 0x03, 1},
        {0x2c, 0xe9, 1},
        {0x2d, 0x07, 1},
        {0x2e, 0xd2, 1},
        {0x2f, 0x0b, 1},
        {0x30, 0x6e, 1},
        {0x31, 0x0e, 1},
        {0x32, 0x70, 1},
        {0x33, 0x12, 1},
        {0x34, 0x0c, 1},
        {0x3c, 0x00, 1}, //[5:4] Max level setting
        {0x3e, 0x20, 1},
        {0x3f, 0x2d, 1},
        {0x40, 0x40, 1},
        {0x41, 0x5b, 1},
        {0x42, 0x82, 1},
        {0x43, 0xb7, 1},
        {0x04, 0x0a, 1},
        {0x02, 0x79, 1},
        {0x03, 0xc0, 1},

        /*measure window*/
        {0xcc, 0x08, 1},
        {0xcd, 0x08, 1},
        {0xce, 0xa4, 1},
        {0xcf, 0xec, 1},

        /*DNDD*/
        {0xfe, 0x00, 1},
        {0x81, 0xb8, 1}, //f8
        {0x82, 0x12, 1},
        {0x83, 0x0a, 1},
        {0x84, 0x01, 1},
        {0x86, 0x50, 1},
        {0x87, 0x18, 1},
        {0x88, 0x10, 1},
        {0x89, 0x70, 1},
        {0x8a, 0x20, 1},
        {0x8b, 0x10, 1},
        {0x8c, 0x08, 1},
        {0x8d, 0x0a, 1},

        /*Intpee*/
        {0xfe, 0x00, 1},
        {0x8f, 0xaa, 1},
        {0x90, 0x9c, 1},
        {0x91, 0x52, 1},
        {0x92, 0x03, 1},
        {0x93, 0x03, 1},
        {0x94, 0x08, 1},
        {0x95, 0x44, 1},
        {0x97, 0x00, 1},
        {0x98, 0x00, 1},

        /*ASDE*/
        {0xfe, 0x00, 1},
        {0xa1, 0x30, 1},
        {0xa2, 0x41, 1},
        {0xa4, 0x30, 1},
        {0xa5, 0x20, 1},
        {0xaa, 0x30, 1},
        {0xac, 0x32, 1},

        /*YCP*/
        {0xfe, 0x00, 1},
        {0xd1, 0x3c, 1},
        {0xd2, 0x3c, 1},
        {0xd3, 0x38, 1},
        {0xd6, 0xf4, 1},
        {0xd7, 0x1d, 1},
        {0xdd, 0x73, 1},
        {0xde, 0x84, 1},

};
//240 320
PRIVATE const CAM_REG_T RG_InitPowerOnQvgainit[] =
    {};

// use this for 320x240 (VGA) capture
PRIVATE const CAM_REG_T RG_Init_QVGA320240[] =
    {

};

//VGA 1W use
PRIVATE const CAM_REG_T RG_Reinit_MASTER2_1[] =
    {
        /*SPI*/

};

PRIVATE const CAM_REG_T RG_Reinit_MASTER2_2[] =
    {

};

PRIVATE const CAM_REG_T RG_Init_QQVGA128160[] =
    {};

PRIVATE const CAM_REG_T RG_Init_SPI_1_Bit[] =
    {};

PRIVATE const CAM_REG_T RG_Reinit_MODE_DEFAULT[] =
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
    char desc[40];

    UINT8 snrtype;         //sensor type(RGB or YUV)
    UINT8 pclk;            //use PCLK of sensor
    UINT8 clkcfg;          //[0]: VSync delay enable; [1]: Fifo VSync delay enable; [2]: Use PCLK Negtive Edge.
    HAL_I2C_BPS_T i2crate; //I2C rate : KHz
    UINT8 i2caddress;      //I2C address
    UINT8 i2cispaddress;   //ISP I2C address
    UINT8 pwrcfg;          //sensor power initial configure(SIF REG801 BIT[0]~BIT[2])
    BOOL rstactiveh;       //sensor reset active hi/low polarity (TRUE = active hi)
    BOOL snrrst;           //Reset sensor enable

    CAM_MODE_T cammode;

    BOOL capturevideo;
    BOOL captureframe;
    UINT8 numactivebuffers;
    UINT8 currentbuf;

    UINT8 brightness; //brightness
    UINT8 contrast;   //contrast

    CAM_FORMAT_T format;
    UINT32 npixels;
    CAM_IMAGE_T vidbuffer;

    CAM_REG_GROUP_T standby;     //sensor standby register
    CAM_REG_GROUP_T Initpoweron; //SENSOR ISP initial configure

    CAM_REG_GROUP_T sifpwronseq; //Sif config sequence(Reg.800 bit0~2) when sensor power on
    CAM_REG_GROUP_T sifstdbyseq; //Sif config sequence(Reg.800 bit0~2) when sensor standby
    CAM_REG_GROUP_T dsif;        //SIF initial configure
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
    CAM_IRQ_HANDLER_T camirqhandler;
} CAM_INFO_T;

PRIVATE volatile CAM_INFO_T gSensorInfo =
    {
        "GC032A", // char desc[40];

        0,                // UINT8 snrtype; (rgb)
        1,                // UINT8 pclk; use or don't use PCLK of sensor???
        0,                // UINT8 clkcfg; ???
        HAL_I2C_BPS_100K, // UINT16 i2crate;
        0x21,             // UINT8 i2caddress; this is the _REAL_ slave address.
        //  The value written in phase 1 of a write cycle will be 0x42
        //  while the value just before the read will be 0x43
        0x21,  // UINT8 i2cispaddress;
        0x02,  // UINT8 pwrcfg;???
        TRUE,  // BOOL rstactiveh;  // active high
        FALSE, // BOOL snrrst;

        CAM_MODE_IDLE, // cammode

        FALSE, // Capture video mode
        FALSE, // Capturing single frame
        0,     // Number of active buffers reserved by the upper layer
        0,     // current frame buffer

        3, // UINT8 brightness;
        4, // UINT8 contrast;

#ifdef __USE_VGA_DEF__
        CAM_FORMAT_RGB565, // Format
        CAM_NPIX_VGA,      // npixels(VGA, QVGA, QQVGA...)
        NULL,              // Video Buffer.  First frame buffer for video or image buffer

        {0, NULL}, // CAM_REG_GROUP_T standby;
        // Should start with QQVGA, but for testing, start with VGA
        //    {sizeof(RG_InitPowerOnQqvga) / sizeof(CAM_REG_T), (CAM_REG_T*)&RG_InitPowerOnQqvga}, // CAM_REG_GROUP_T Initpoweron;
        {sizeof(RG_InitPowerOnVga) / sizeof(CAM_REG_T), (CAM_REG_T *)&RG_InitPowerOnVga}, // CAM_REG_GROUP_T Initpoweron;
#else

        CAM_FORMAT_RGB565, // Format
        CAM_NPIX_QVGA,     // npixels(VGA, QVGA, QQVGA...)
        NULL,              // Video Buffer.  First frame buffer for video or image buffer

        {0, NULL}, // CAM_REG_GROUP_T standby;
        // Should start with QQVGA, but for testing, start with VGA
        //	  {sizeof(RG_InitPowerOnQqvga) / sizeof(CAM_REG_T), (CAM_REG_T*)&RG_InitPowerOnQqvga}, // CAM_REG_GROUP_T Initpoweron;
        {sizeof(RG_Init_QVGA320240) / sizeof(CAM_REG_T), (CAM_REG_T *)&RG_Init_QVGA320240}, // CAM_REG_GROUP_T Initpoweron;
#endif
        {0, NULL}, // CAM_REG_GROUP_T sifpwronseq;
        {0, NULL}, // CAM_REG_GROUP_T sifstdbyseq;
        {0, NULL}, // CAM_REG_GROUP_T dsif; initial value
        NULL       // IRQ Callback
};

// ############
// Private
// ############

// These are the commands specific to the SCCB
#define PHASE3_WR_PH1 0x11000 // Start & Write    Slave ID
#define PHASE3_WR_PH2 0x1000  // Write            Slave Register Address
#define PHASE3_WR_PH3 0x1100  // Stop & Write     Slave Register Data

#define PHASE2_WR_PH1 0x11000 // Start & Write    Slave ID
#define PHASE2_WR_PH2 0x1100  // Stop & Write     Slave Register Address (to read)

#define PHASE2_RD_PH1 0x11000 // Start & Write    Slave ID
#define PHASE2_RD_PH2 0x111   // Stop & Read & NACK   Slave Register Read Data

PRIVATE VOID camerap_WriteOneReg(UINT8 Addr, UINT8 Data);
PRIVATE VOID camerap_Delay(UINT16 Wait_mS);
PRIVATE VOID camerap_WriteReg(CAM_REG_T RegWrite);
PRIVATE UINT8 camerap_ReadReg(UINT8 Addr);
PRIVATE VOID camerap_SendRgSeq(const CAM_REG_T *RegList, UINT16 Len);
PRIVATE VOID camerap_InteruptHandler(HAL_CAMERA_IRQ_CAUSE_T cause);
PRIVATE UINT8 camerap_GetId(VOID);
PRIVATE VOID camerap_Reserve(CAM_IRQ_HANDLER_T FrameReady);
PRIVATE VOID camerap_Release(VOID);
PRIVATE VOID camerap_PowerOn(CAM_SIZE_T CamOutSize, UINT16 IspOutWidth, UINT16 IspOutHeight, CAM_FORMAT_T Format);
PRIVATE VOID camerap_PowerOff(VOID);
PRIVATE VOID camerap_CaptureImage(VOID);
PRIVATE VOID camerap_SetZoomFactorL(UINT32 ZoomFactor);
PRIVATE UINT32 camerap_ZoomFactor(VOID);
PRIVATE VOID camerap_SetDigitalZoomFactorL(UINT32 DigZoomFactor);
PRIVATE UINT32 camerap_DigitalZoomFactor(VOID);
PRIVATE VOID camerap_SetContrastL(UINT32 Contrast);
PRIVATE UINT32 camerap_Contrast(VOID);
PRIVATE VOID camerap_SetBrightnessL(UINT32 Brightness);
PRIVATE UINT32 camerap_Brightness(VOID);
PRIVATE VOID camerap_SetFlashL(CAM_FLASH_T Flash);
PRIVATE CAM_FLASH_T camerap_Flash(VOID);
PRIVATE VOID camerap_SetExposureL(CAM_EXPOSURE_T Exposure);
PRIVATE CAM_EXPOSURE_T camerap_Exposure(VOID);
PRIVATE void camerap_SetBanding(UINT32 iBanding);
PRIVATE VOID camerap_SetWhiteBalanceL(CAM_WHITEBALANCE_T WhiteBalance);
PRIVATE CAM_WHITEBALANCE_T camerap_WhiteBalance(VOID);
PRIVATE VOID camerap_SetSpecialEffect(CAM_SPECIALEFFECT_T SpecialEffect);
PRIVATE CAM_SPECIALEFFECT_T camerap_SpecialEffect(VOID);
PRIVATE VOID camerap_SetNightMode(UINT32 NightMode);
PRIVATE UINT32 camerap_NightMode(VOID);

// This is not _really_ Direct, but it does implement a viewfinder function
PRIVATE VOID camerap_StartViewFinderDirectL();
PRIVATE VOID camerap_StopViewFinder(VOID);
PRIVATE BOOL camerap_ViewFinderActive(VOID);

PRIVATE VOID camerap_SetViewFinderMirror(BOOL Mirror);
PRIVATE BOOL camerap_ViewFinderMirror(VOID);
PRIVATE VOID camerap_CancelCaptureImage(VOID);
PRIVATE VOID camerap_EnumerateCaptureSizes(CAM_SIZE_T *Size, UINT32 SizeIndex, CAM_FORMAT_T Format);
PRIVATE VOID camerap_PrepareVideoCaptureL(CAM_FORMAT_T Format, UINT32 SizeIndex, UINT32 RateIndex, UINT32 BuffersToUse, UINT32 FramesPerBuffer);
PRIVATE VOID camerap_StartVideoCapture(VOID);
PRIVATE VOID camerap_StopVideoCapture(VOID);
PRIVATE BOOL camerap_VideoCaptureActive(VOID);
PRIVATE VOID camerap_EnumerateVideoFrameSizes(CAM_SIZE_T Size, UINT32 SizeIndex, CAM_FORMAT_T Format);
PRIVATE VOID camerap_EnumerateVideoFrameRates();
PRIVATE VOID camerap_GetFrameSize(CAM_SIZE_T Size);
PRIVATE VOID camerap_SetFrameRate(UINT32 FrameRate);
PRIVATE UINT32 camerap_FrameRate(VOID);
PRIVATE UINT32 camerap_BuffersInUse(VOID);
PRIVATE UINT32 camerap_FramesPerBuffer(VOID);
