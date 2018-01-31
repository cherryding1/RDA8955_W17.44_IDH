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

#define IMAGE_NORMAL 
//#define IMAGE_H_MIRROR
//#define IMAGE_V_MIRROR
//#define IMAGE_HV_MIRROR 

#ifdef IMAGE_NORMAL
#define MIRROR 0x54
#define AD_NUM 0x04
#define BLK_VAL 0x46
#endif

#ifdef IMAGE_H_MIRROR
#define MIRROR 0x55
#define AD_NUM 0x08
#define BLK_VAL 0x46
#endif

#ifdef IMAGE_V_MIRROR
#define MIRROR 0x5e
#define AD_NUM 0x04
#define BLK_VAL 0x30
#endif

#ifdef IMAGE_HV_MIRROR
#define MIRROR 0x5f
#define AD_NUM 0x08
#define BLK_VAL 0x30
#endif


// use this for 320x240 (VGA) capture
PRIVATE const CAM_REG_T RG_Init_PowerOn[] =
{
    /////////////////////////////////////////////////////
    //////////////////////   SYS   //////////////////////
    /////////////////////////////////////////////////////
    {0xfe, 0xa0, 1},
    {0xfe, 0xa0, 1},
    {0xfe, 0xa0, 1},

    {0xf6, 0x00, 1},
    {0xfa, 0x11, 1},
    {0xfc, 0x12, 1}, //clock enable
    {0xfe, 0x00, 1}, //Page select

    {0x49, 0x70, 1},  //AWB r gain
    {0x4a, 0x40, 1},  //AWB g gain
    {0x4b, 0x5d, 1},  //AWB b gain
    /////////////////////////////////////////////////////
    ////////////////ANALOG & CISCTL////////////////
    /////////////////////////////////////////////////////
    {0x03, 0x01, 1},
    {0x04, 0x44, 100},
    {0x01, 0x41, 1}, //hb
    {0x02, 0x12, 1}, //vb
    {0x0f, 0x01, 1},
    {0x0d, 0x30, 1},
    {0x12, 0xc8, 1},
    {0x14, MIRROR, 1}, //dark CFA
    {0x15, 0x32, 1}, //1:sdark 0:ndark
    {0x16, AD_NUM, 1},
    {0x17, 0x19, 1},
    {0x1d, 0xb9, 1},
    {0x1f, 0x35, 1}, //PAD_drv
    {0x7a, 0x00, 1},
    {0x7b, 0x14, 1},
    {0x7d, 0x36, 1},
    {0xfe, 0x10, 1}, //cisctl rst [4]



    /////////////////////////////////////////////////////
    //////////////////////   ISP   //////////////////////
    /////////////////////////////////////////////////////
    {0x20, 0x7e, 1},
    {0x22, 0xb8, 1},
    {0x24, 0x54, 1}, //output_format
    {0x26, 0x87, 1}, //[5]Y_switch [4]UV_switch [2]skin_en
    //{0x29, 0x30, 1},  //enable isp quiet mode

    {0x39, 0x00, 1},//crop window
    {0x3a, 0x80, 1},
    {0x3b, 0x01, 1}, //width
    {0x3c, 0x40, 1},
    {0x3e, 0xf0, 1},//height
    /////////////////////////////////////////////////////
    //////////////////////   BLK   //////////////////////
    /////////////////////////////////////////////////////
    {0x2a, 0x2f, 1},
    {0x37, BLK_VAL, 1}, //[4:0]blk_select_row

    /////////////////////////////////////////////////////
    //////////////////////   GAIN   /////////////////////
    /////////////////////////////////////////////////////
    {0x3f, 0x18, 1}, //global gain//10


    /////////////////////////////////////////////////////
    //////////////////////   DNDD   /////////////////////
    /////////////////////////////////////////////////////
    {0x50, 0x3c, 1},
    {0x52, 0x4f, 1},
    {0x53, 0x81, 1},
    {0x54, 0x43, 1},
    {0x56, 0x78, 1},
    {0x57, 0xaa, 1},//20160901 
	{0x58, 0xff, 1},//20160901

    /////////////////////////////////////////////////////
    //////////////////////   ASDE   /////////////////////
    /////////////////////////////////////////////////////
    {0x5b, 0x60, 1}, //dd&ee th
    {0x5c, 0x80, 1}, //60/OT_th
    {0xab, 0x28, 1},
    {0xac, 0xb5, 1},

    /////////////////////////////////////////////////////
    /////////////////////   INTPEE   ////////////////////
    /////////////////////////////////////////////////////
    {0x60, 0x45, 1},
	{0x62, 0x68, 1},
    {0x63, 0x14, 1}, //edge effect//13
    {0x64, 0x43, 1},

    /////////////////////////////////////////////////////
    //////////////////////   CC   ///////////////////////
    /////////////////////////////////////////////////////
    {0x65, 0x13, 1}, //Y
    {0x66, 0x26, 1},
    {0x67, 0x07, 1},
    {0x68, 0xf5, 1}, //Cb
    {0x69, 0xea, 1},
    {0x6a, 0x21, 1},
    {0x6b, 0x21, 1}, //Cr
    {0x6c, 0xe4, 1},
    {0x6d, 0xfb, 1},

    /////////////////////////////////////////////////////
    //////////////////////   YCP   //////////////////////
    /////////////////////////////////////////////////////
    {0x81, 0x30, 1}, //cb
    {0x82, 0x30, 1}, //cr
    {0x83, 0x4a, 1}, //luma contrast
    {0x85, 0x04, 1},//06
    {0x8d, 0x78, 1}, //edge dec sa
    {0x8e, 0x25, 1}, //autogray

    /////////////////////////////////////////////////////
    //////////////////////   AEC   //////////////////////
    /////////////////////////////////////////////////////
    {0x90, 0x38, 1},
    {0x92, 0x50, 1}, //target//50
    {0x9d, 0x32, 1}, //step
    {0x9e, 0x61, 1}, //[7:4]margin  10fps
    {0x9f, 0x2c, 1},

    {0xa3, 0x28, 1}, //pregain
    {0xa4, 0x01, 1}, //open aec

    /////////////////////////////////////////////////////
    //////////////////////   AWB   //////////////////////
    /////////////////////////////////////////////////////
    {0xb1, 0x1e, 1}, //Y_to_C_diff
    {0xb3, 0x20, 1}, //C_max
    {0xbd, 0x70, 1}, //R_limit
    {0xbe, 0x58, 1}, //G_limit
    {0xbf, 0xa0, 1}, //B_limit

	{0xfe, 0x00, 1}, 
	{0x43, 0xa8, 1}, 
	{0xb0, 0xf2, 1}, 
	{0xb5, 0x40, 1}, 
	{0xb8, 0x05, 1}, 
	{0xba, 0x60, 1}, 


    /////////////////////////////////////////////////////
    //////////////////////   SPI   //////////////////////
    /////////////////////////////////////////////////////
    {0xfe, 0x02, 1},
    {0x01, 0x01, 1}, //SPI MASTER enable
    {0x02, 0x80, 1}, //ddr_mode  0x80->0x82
    {0x03, 0x20, 1},  //20
    {0x04, 0x20, 1},  //[4] master_outformat

    {0x0a, 0x00, 1}, //yuv
    {0x13, 0x10, 1}, //50/fifo_prog_full_level
    ////SYNC code default
    //{0x20, 0x01, 1}, //00
    //{0x21, 0x02, 1}, //01
    //{0x22, 0x40, 1}, //a5
    //{0x23, 0x00, 1}, //5a
    {0x28, 0x03, 1}, //clock_div_spi
    {0xfe, 0x00, 1},

    ///////////////output //////////
    {0x22, 0xf8, 1}, //open awb
    {0xf1, 0x03, 1}, //output enable
};

// 128x160, for camera preview
PRIVATE const CAM_REG_T RG_Reinit_QQVGA[] =
    {};

// 240x320, for capture
PRIVATE const CAM_REG_T RG_Reinit_QVGA[] =
{

};

// specific to video recorder, 160x120
PRIVATE const CAM_REG_T RG_Reinit_QQVGA_2[] =
{

};

PRIVATE const CAM_REG_T RG_Reinit_MASTER1[] =
{

};

//one wire 1bit
PRIVATE const CAM_REG_T RG_Reinit_MASTER2_1[] =
{


};
//two wire 2bit
PRIVATE const CAM_REG_T RG_Reinit_MASTER2_2[] =
{

};

PRIVATE const CAM_REG_T RG_Reinit_MASTER2_4[] =
//no support
    {};


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
    {};

PRIVATE const CAM_REG_T RG_VidFormatQvga[] =
{
};
/*
PRIVATE const CAM_REG_T RG_VidFormatQqvga[] =
{
#endif
};
*/

PRIVATE const CAM_REG_T RG_VidFormatQqvga[] =
{
#if !defined(VDOREC_FAST_RECORDING)
    //10 fps
    {0xfe, 0x00,1},
    //MCLK=26MHz
    {0x01, 0x41, 1}, //hb
    {0x02, 0x12, 1}, //vb
    {0x0f, 0x01, 1},
    {0x9d, 0x32, 1}, //step
    {0x9e, 0x61, 1}, //[7:4]margin  10fps//21c
    {0x9f, 0x2c, 1},
#else
    // 15 fps
    {0xfe, 0x00,1},
    //MCLK=26MHz
    {0x01, 0x41, 1}, //hb
    {0x02, 0x12, 1}, //vb
    {0x0f, 0x01, 1},
    {0x9d, 0x32, 1}, //step
    {0x9e, 0x61, 1}, //[7:4]margin  15fps//17a
    {0x9f, 0x2c, 1},

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
    "GC6133",      // char desc[40];

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