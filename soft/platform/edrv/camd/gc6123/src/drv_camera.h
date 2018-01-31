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
    /////////////////////////////////////////////////////
    //////////////////////   SYS   //////////////////////
    /////////////////////////////////////////////////////
    {0xfe, 0xa0, 1},
    {0xfe, 0xa0, 1},
    {0xfe, 0xa0, 1},
    {0xf1, 0x07, 1}, //output enable
    {0xf4, 0x00, 1},
    {0xfc, 0x16, 1}, //clock enable
    {0xfe, 0x00, 1}, //Page select

    /////////////////////////////////////////////////////
    //////////////// , 0x ANALOG & CISCTL , 0x ////////////////
    /////////////////////////////////////////////////////
    {0xfe , 0x00,1},
    // 26M mclk
    /*
    {0x01 , 0xae,1},  //hb[7:0]
    {0x02 , 0x3e,1},  //vb[7:0]
    {0x0f , 0x06,1},  //vb[11:8] , hb[11:8]

    {0x9c , 0x00,1},   //step[11:8]
    {0x9d , 0x41,1},   //step[7:0]

    {0x9e , 0x02,1},   // level_0  9.99fps
    {0x9f , 0x8a,1},
    */
    // 12M mclk
    {0x01 , 0x5d,1},  //hb[7:0]
    {0x02 , 0x44,1},  //vb[7:0]
    {0x0f , 0x05,1},  //vb[11:8] , hb[11:8]

    {0x9c , 0x00,1},   //step[11:8]
    {0x9d , 0x24,1},   //step[7:0]

    {0x9e , 0x01,1},   // level_0  8.48fps
    {0x9f , 0x68,1},

    {0x08 , 0x02, 1}, //col start
    {0x09 , 0x01, 1}, //window height
    {0x0a , 0x48, 1},
    {0x0b , 0x00, 1}, //window width
    {0x0c, 0xf4, 1},
    {0x10 , 0x48, 1}, //sh_width
    {0x11 , 0x1d, 1}, //tx_width
    {0x14 , 0x14, 1}, //dark CFA
    {0x15 , 0x0a, 1}, //sdark
    {0x16 , 0x04, 1}, //AD pipe number
    {0x18 , 0xc2, 1}, //rowsg_gap
    {0x1a , 0x17, 1}, //clk_delay_en
    {0x1b, 0x1a, 1}, //20121107 comv_r solve FPN-W/18 //70/adclk mode
    {0x1c , 0x49, 1}, //txhigh_en
    {0x1d , 0xb0, 1}, //vref
    {0x1e, 0x53, 1}, //20130306/52 //20121107 solve OBI/51 //ADC_r
    {0x1f , 0x15, 1},

    /////////////////////////////////////////////////////
    //////////////////////   ISP   //////////////////////
    /////////////////////////////////////////////////////
    {0x20, 0x5e, 1},
    {0x21, 0x38, 1}, //autogray
    {0x22, 0x92, 1}, //20121107 auto_DD_en/82 //02
    {0x24, 0xa0, 1}, //output_format
    {0x26, 0x03, 1},
    {0x27, 0x90, 1}, //clock gating
    {0x28, 0x8c, 1},
    {0x38, 0x80, 1}, //crop
    {0x3b, 0x01, 1},
    {0x3c, 0x40, 1},
    {0x3d, 0x00, 1},
    {0x3e, 0xf0, 1},

    /////////////////////////////////////////////////////
    //////////////////////   BLK   //////////////////////
    /////////////////////////////////////////////////////
    {0x2a, 0x65, 1}, //2f/BLK row select
    {0x2c, 0x40, 1}, //global offset

    /////////////////////////////////////////////////////
    //////////////////////   GAIN   /////////////////////
    /////////////////////////////////////////////////////
    {0x3f, 0x12 , 1},//20/global gain

    /////////////////////////////////////////////////////
    //////////////////////   DNDD   /////////////////////
    /////////////////////////////////////////////////////
    {0x50, 0x45, 1},
    {0x52, 0x4f , 1},//6c/DN b
    {0x53, 0x81, 1}, //a5/DN C
    {0x58, 0x6f , 1},//20121107 dark_th 64
    {0xc3, 0x96, 1}, //00

    /////////////////////////////////////////////////////
    //////////////////////   ASDE   /////////////////////
    /////////////////////////////////////////////////////
    {0xac, 0xb5, 1},
    {0x5c, 0x80, 1}, //60/OT_th

    /////////////////////////////////////////////////////
    /////////////////////   INTPEE   ////////////////////
    /////////////////////////////////////////////////////
    {0x63, 0x03, 1}, //04/edge effect
    {0x65, 0x23, 1}, //43/42/edge max/min

    /////////////////////////////////////////////////////
    /////////////////////   GAMMA   /////////////////////
    /////////////////////////////////////////////////////

    /////////////////////////////////////////////////////
    //////////////////////   CC   ///////////////////////
    /////////////////////////////////////////////////////
    {0x66, 0x13, 1},
    {0x67, 0x26, 1},
    {0x68, 0x07, 1},
    {0x69, 0xf5, 1},
    {0x6a, 0xea, 1},
    {0x6b, 0x21, 1},
    {0x6c, 0x21, 1},
    {0x6d, 0xe4, 1},
    {0x6e, 0xfb, 1},

    /////////////////////////////////////////////////////
    //////////////////////   YCP   //////////////////////
    /////////////////////////////////////////////////////
    {0x81, 0x48, 1}, //38//cb
    {0x82, 0x48, 1}, //38//cr
    {0x83, 0x4b, 1}, //40/luma contrast
    {0x84, 0x80, 1}, //90/contrast center
    {0x85, 0x00, 1}, //06/luma offset
    {0x86, 0xfb , 1},//skin cb
    {0x87, 0x1d, 1}, //skin cr
    {0x88, 0x18, 1}, //skin radius
    {0x8d, 0x78, 1}, //38/edge dec sa
    {0x8e, 0x25, 1}, //autogray

    /////////////////////////////////////////////////////
    //////////////////////   AEC   //////////////////////
    /////////////////////////////////////////////////////
    {0xa4, 0x01, 1},
    {0x9e, 0x02, 1},
    {0x9f, 0x1c, 1},
    {0xa0, 0x10, 1},
    {0x90, 0x4a, 1}, //0a //48
    {0x92, 0x40, 1}, //40/target Y
    {0xa2, 0x40, 1}, //max_post_gain
    {0xa3, 0x80, 1}, //max_pre_gain

    /////////////////////////////////////////////////////
    //////////////////////   AWB   //////////////////////
    /////////////////////////////////////////////////////
    {0xb0, 0xf8 , 1},//f5/RGB high low
    {0xb1, 0x24, 1}, //18/Y to C
    {0xb3, 0x20, 1}, //0d/C max
    {0xb4, 0x2d, 1}, //22/C inter
    {0xb5, 0x1b, 1}, //22/C inter
    {0xb6, 0x2e, 1}, //C inter2
    {0xb7, 0x18, 1}, //40/C inter3
    {0xb8, 0x13, 1}, //20/number limit
    {0xb9, 0x33, 1},
    {0xba, 0x21, 1},
    {0xbb, 0x61, 1}, //42/speed & margin
    {0xbf, 0x68 , 1},//78/b limit
    {0x4c, 0x08, 1},
    {0x4d, 0x06, 1},
    {0x4e, 0x7b, 1},
    {0x4f, 0xa0, 1},

    /////////////////////////////////////////////////////
    //////////////////////   SPI   //////////////////////
    /////////////////////////////////////////////////////
    {0xfe, 0x02, 1},
    {0x01, 0x03, 1}, //SPI MASTER enable
    {0x0a, 0x00, 1}, //yuv
    {0x0b, 0xf0, 1},
    {0x0c, 0x00, 1},
    {0x0d, 0x40, 1},
    {0x0e, 0x01, 1},
    {0x13, 0xd0, 1}, //50/fifo_prog_full_level
    {0x20, 0x01, 1}, //00 //SYNC code
    {0x21, 0x02, 1}, //01
    {0x22, 0x40, 1}, //a5
    {0x23, 0x00, 1}, //5a

    /////////////////////////////////////////////////////
    /////////////////////// , 0x 2 , 0x , 0x//////////////////////
    /////////////////////////////////////////////////////
    {0x03 , 0x24, 1},  //20
    {0xfd , 0x04, 1},  //00

    {0xfe , 0x00, 1},
};

// 128x160, for camera preview
PRIVATE const CAM_REG_T RG_Reinit_QQVGA[] =
    {};

// 240x320, for capture
PRIVATE const CAM_REG_T RG_Reinit_QVGA[] =
{

    {0x38, 0x80, 1},
    {0x39, 0x00, 1},
    {0x3a, 0x00, 1},
    {0x3b, 0x01, 1},
    {0x3c, 0x40, 1},
    {0x3d, 0x00, 1},
    {0x3e, 0xf0, 1},

    {0xfe, 0x02, 1},
    {0x0b, 0xf0, 1},
    {0x0d, 0x40, 1},
    {0x0e, 0x01, 1},
    {0x13, 0xc0, 1},
    {0xfe, 0x00, 1},
};

// specific to video recorder, 160x120
PRIVATE const CAM_REG_T RG_Reinit_QQVGA_2[] =
    {};

PRIVATE const CAM_REG_T RG_Reinit_MASTER1[] =
{
    {0xfe, 0x02, 1},
    {0x03, 0x00, 1},
    {0xfd, 0x00, 1},
    {0xfe, 0x00, 1},
    {0xf1, 0x05, 1},
};


PRIVATE const CAM_REG_T RG_Reinit_MASTER2_1[] =
{
    {0xfe, 0x02, 1},
    {0x03, 0x20, 1},
    {0xfd, 0x05, 1},
    {0xfe, 0x00, 1},
    {0xf1, 0x05, 1},
};

PRIVATE const CAM_REG_T RG_Reinit_MASTER2_2[] =
{
    {0xfe, 0x02, 1},
    {0x03, 0x24, 1},
    {0xfd, 0x04, 1},
    {0xfe, 0x00, 1},
    {0xf1, 0x07, 1},
};

PRIVATE const CAM_REG_T RG_Reinit_MASTER2_4[] =
//no support
    {};

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
    //  {0x1d, 0x95,1},  // bit[3:2] driving SDO_0 current

    //{0x24, 0xa6,1},  // a2= yuv  a6=rgb
    {0x24, 0xa0,1},  // a2= yuv  a6=rgb
    //{0xf1, 0x1f,1}, //output_en

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
    // 10 fps
    {0xfe, 0x00,1},
    //26M mclk
    /*
    {0x01, 0xae,1},  //hb[7:0]
    {0x02, 0x3e,1},  //vb[7:0]
    {0x0f, 0x06,1},  //vb[11:8] , hb[11:8]

    {0x9c, 0x00,1},   //step[11:8]
    {0x9d, 0x41,1},   //step[7:0]

    {0x9e, 0x02,1},   // level_0  9.99fps
    {0x9f, 0x8a,1},
    */
    // 12M mclk
    {0x01 , 0x5d,1},  //hb[7:0]
    {0x02 , 0x44,1},  //vb[7:0]
    {0x0f , 0x05,1},  //vb[11:8] , hb[11:8]

    {0x9c , 0x00,1},   //step[11:8]
    {0x9d , 0x24,1},   //step[7:0]

    {0x9e , 0x01,1},   // level_0  8.48fps
    {0x9f , 0x68,1},
#else
    // 15 fps
    {0xfe, 0x00,1},
    // 26M mclk
    /*
    {0x01, 0xae,1},  //hb[7:0]
    {0x02, 0x3e,1},  //vb[7:0]
    {0x0f, 0x06,1},  //vb[11:8] , hb[11:8]

    {0x9c, 0x00,1},   //step[11:8]
    {0x9d, 0x41,1},   //step[7:0]

    {0x9e, 0x01,1},   // level_0  16.67fps
    {0x9f, 0x86,1},
    */
    // 12M mclk
    {0x01 , 0x5d,1},  //hb[7:0]
    {0x02 , 0x44,1},  //vb[7:0]
    {0x0f , 0x05,1},  //vb[11:8] , hb[11:8]

    {0x9c , 0x00,1},   //step[11:8]
    {0x9d , 0x24,1},   //step[7:0]

    {0x9e , 0x01,1},   // level_0  8.48fps
    {0x9f , 0x68,1},
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

