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


#include "cs_types.h"
#include "sxs_io.h"

#include "hal_camera.h"
#include "hal_gpio.h"
#include "drv_camera.h"
#include "pmd_m.h"
#include "camera_driver.h"
#include "camera_private.h"
#include "tgt_camd_cfg.h"

#define PAS6175_3a01_FAST


#ifdef I2C_BASED_ON_GPIO
#include "gpio_i2c.h"
#error "PAS6175_3a01 use the hardware I2C"
#else
PRIVATE HAL_I2C_BUS_ID_T g_camdI2cBusId = HAL_I2C_BUS_ID_INVALID;
#endif

PRIVATE UINT8 protect = 0;
#define CAM_BY3A01_ID   0x01

#if defined(VDOREC_MODE_VGA) || defined(VDOREC_MODE_QVGA)
#error "VGA/QVGA recording not supported on GC6113 yet."
#endif

#if defined(_CAM_SPECIAL_GC6113_)
typedef enum
{
    CAM_OUT_NULL = 0,
    CAM_OUT_QQVGA,
    CAM_OUT_QVGA,
    CAM_OUT_MODE_MAX
} CAM_OUT_MODE_E;
PRIVATE CAM_OUT_MODE_E s_camOutMode = CAM_OUT_NULL;
extern UINT8* gTransBufPtr[2];
extern UINT8 gTransBufIdx;
extern UINT32 gTransBufSize;
PRIVATE HAL_CAMERA_SPI_MODE_E s_gc6113_mode = SPI_MODE_MASTER1;
#else
static UINT32 s_prevCount = 0;
PRIVATE HAL_CAMERA_SPI_MODE_E s_gc6113_mode = SPI_MODE_MASTER2_1;
#endif

PRIVATE BOOL s_camOpened = FALSE;
PRIVATE BOOL gIsCameraStartXfer = FALSE;

extern BOOL isVideoRecMode;



const UINT8 PAS6175_3a01_AeStage_GainStage[][6] =
{
    //R_AE_maxStage[4:0], R_AG_stage_UB, R_AE_stage_LL[4:0], R_AE_stage_NL[4:0], R_Gamma_Strength_Delta[3:0], R_Saturation_LL[4:0]
    {0x14,0x20,0x14,0x10,0x02,0x09},//Normal,
    {0x1B,0x22,0x1B,0x14,0x04,0x07},//Night,
};
const UINT8 PAS6175_3a01_WB_MODE[][4] =
{
    /* Cmd_Cgn_Gb,  Cmd_Cgn_B, Cmd_Cgn_Gr, Cmd_Cgn_R */
    {   9,  13,   9,  15},//Cloud  //熬屡
    {  10,  14,  10,  14},//DAYLIGHT び锭 //熬独
    {   9,  23,   9,   9},//INCANDESCENCE フ荐 //
    {   9,  15,   9,  15},//FLUORESCENT 棵 //熬屡
    {   7,  20,   7,   7},//TUNGSTEN 码捣縊 //熬屡
};
const UINT8 PAS6175_3a01_Effect[][4]    =
{
    {0x00, 0x01,0x00, 0x00}, //CAM_EFFECT_ENC_GRAYSCALE
    {0x03, 0x01, 100,  141}, //CAM_EFFECT_ENC_SEPIA 確
    {0x02, 0x01,0x00, 0x00}, //CAM_EFFECT_ENC_COLORINV
    {0x03, 0x01, 105,  113}, //CAM_EFFECT_ENC_SEPIAGREEN
    {0x03, 0x01, 151,  132}, //CAM_EFFECT_ENC_SEPIABLUE 確屡
};
enum
{
    CAM_EFFECT_ENC_NORMAL=0,
    CAM_EFFECT_ENC_GRAYSCALE,
    CAM_EFFECT_ENC_SEPIA,
    CAM_EFFECT_ENC_SEPIAGREEN,
    CAM_EFFECT_ENC_SEPIABLUE,
    CAM_EFFECT_ENC_COLORINV,
    CAM_EFFECT_ENC_GRAYINV,
    CAM_EFFECT_ENC_BLACKBOARD,
    CAM_EFFECT_ENC_WHITEBOARD,
    CAM_EFFECT_ENC_COPPERCARVING,
    CAM_EFFECT_ENC_EMBOSSMENT,
    CAM_EFFECT_ENC_BLUECARVING,
    CAM_EFFECT_ENC_CONTRAST,
    CAM_EFFECT_ENC_JEAN,
    CAM_EFFECT_ENC_SKETCH,
    CAM_EFFECT_ENC_OIL,
    CAM_NO_OF_EFFECT_ENC,
    //add by gugogi
    CAM_EFFECT_ENC_ANTIQUE,
    CAM_EFFECT_ENC_REDRISH,
    CAM_EFFECT_ENC_GREENRISH,
    CAM_EFFECT_ENC_BLUERISH,
    CAM_EFFECT_ENC_BLACKWHITE,
    CAM_EFFECT_ENC_NEGATIVENESS

};

BOOL for_serial_sensor_3a01 = FALSE;

typedef struct
{
    UINT16 HSize;
    UINT16 WSize;

} S_SENSOR_FRAME_T;

PRIVATE S_SENSOR_FRAME_T g_sensor_frame;
extern UINT32 gCameraID;

//###
UINT32 dummy_pixels_3a01 = 0;
typedef enum SensorMirror
{
    SENSOR_NO_FLIP = 0x00,
    SENSOR_X_FLIP  = 0x04,
    SENSOR_Y_FLIP  = 0x08,
    SENSOR_XY_FLIP = 0x0c
} SensorMirror;

UINT16 Hsize_3a01 = 0;

UINT16 Vsize_3a01 = 0;
UINT16 ny_value_3a01 = 0;
UINT16 ne_value_3a01 = 0;
PRIVATE UINT8 preview_state = FALSE;
PRIVATE UINT8 capture_state = FALSE;
PRIVATE UINT8 preview_first = FALSE;
PRIVATE UINT8 power_on = FALSE;

BOOL PAS6175_3a01_LCD_BW =  FALSE;//KAL_TRUE, KAL_FALSE;

BOOL PAS6167_3a01_Output_Continuous = TRUE;//KAL_TRUE, KAL_FALSE;
UINT16 lpf_value_3a01 = 0;
UINT32 exposure_pixels_3a01 = 0;

const UINT16 PAS6175_3a01_DUMMY_PIXELS[]        = {100,1800,1000};/* Preview, Capture, Video */
const UINT8 PAS6175_3a01_NP[]               = {8, 12, 4};   /* Preview, Capture, Video */

PRIVATE UINT32 Sysclk = 0;
BOOL PAS6167_3a01_Support_220x176 = FALSE; //KAL_TRUE, KAL_FALSE

#ifdef TEMP_C006S_SPICAM_CFG_MACRO
PRIVATE UINT8 PAS6175_3a01_Image_Flip[]     = {SENSOR_XY_FLIP};/* SENSOR_NO_FLIP , SENSOR_XY_FLIP, SENSOR_X_FLIP, SENSOR_Y_FLIP */
#else
PRIVATE UINT8 PAS6175_3a01_Image_Flip[]     = {SENSOR_NO_FLIP};/* SENSOR_NO_FLIP , SENSOR_XY_FLIP, SENSOR_X_FLIP, SENSOR_Y_FLIP */
#endif

PRIVATE UINT8 Cmd_Cgn_Gb = 0;
PRIVATE UINT8 Cmd_Cgn_B = 0;
PRIVATE UINT8 Cmd_Cgn_Gr = 0;
PRIVATE UINT8 Cmd_Cgn_R = 0;
PRIVATE UINT8 Brightness_Value_LL = 0;
PRIVATE UINT8 Brightness_Value_NL = 0;
PRIVATE volatile UINT8 gIfcChan = 0xff;
PRIVATE BOOL s_spi_interFinished = TRUE;
PRIVATE BOOL s_camrst = FALSE;
PRIVATE UINT16 pas_id = 0xff;
PRIVATE BOOL pas_capture = FALSE;
PRIVATE UINT8 preview_pclk_division = 0;
PRIVATE UINT8 capture_pclk_division = 0;
PRIVATE HAL_SYS_CLOCK_OUT_ID_T g_SerialClockOutId = HAL_SYS_CLOCK_OUT_ID_QTY;
PRIVATE BOOL g_SerialClockOut = FALSE;

const UINT8 PAS6175_3a01_OUTPUT_FORMAT[]        = {0,0,0};  /* Preview (RGB565), Capture(YUV422), Capture(RGB565), */

const UINT8 PAS6175_3a01_DELAY_FRAME[]      = {2};// PREVIEW_DELAY_FRAME

PRIVATE UINT8 camerap_ReadReg(UINT8 Addr)
{

//if(protect ==1)  hal_DbgAssert("PAS6175_3a01:");

#ifdef I2C_BASED_ON_GPIO
    UINT8 data = 0;
    pas_gpio_i2c_read_data_sensor(0x40, &Addr, 1, &data, 1);
    return data;
#else
    if (hal_I2cSendRawByte(g_camdI2cBusId, ((gSensorInfo.i2caddress<<1) & 0xfe), PHASE2_WR_PH1) == HAL_ERR_RESOURCE_BUSY)
    {
        return(0x1f);
    }
    if (hal_I2cSendRawByte(g_camdI2cBusId, Addr, PHASE2_WR_PH2) == HAL_ERR_RESOURCE_BUSY)
    {
        return(0x1e);
    }

    if (hal_I2cSendRawByte(g_camdI2cBusId, ((gSensorInfo.i2caddress<<1) | 0x01), PHASE2_RD_PH1) == HAL_ERR_RESOURCE_BUSY)
    {
        return(0x1d);
    }

    return(hal_I2cReadRawByte(g_camdI2cBusId, PHASE2_RD_PH2));
#endif
}



PRIVATE VOID  camerap_WriteOneReg( UINT8 Addr, UINT8 Data)
{
    if(protect ==1) hal_HstSendEvent(0xccccdddd);

#if 1

    if(HAL_ERR_NO != hal_I2cSendByte(g_camdI2cBusId,0x6e,Addr, Data))
        hal_DbgAssert("0x%x: i2c no ack when writing reg ", Addr);

#else
#ifdef I2C_BASED_ON_GPIO
    UINT8 i = 0;

    while (i++ < 3)
        if( pas_gpio_i2c_write_data_sensor(0x40, &Addr, 1, &Data, 1) == TRUE)
            return;

    hal_DbgAssert("PAS6175_3a01: i2c no ack when writing reg 0x%x", Addr);
#else
    hal_I2cSendRawByte(g_camdI2cBusId, ((gSensorInfo.i2caddress<<1) & 0xfe), PHASE3_WR_PH1);
    hal_I2cSendRawByte(g_camdI2cBusId, Addr, PHASE3_WR_PH2);
    hal_I2cSendRawByte(g_camdI2cBusId, Data, PHASE3_WR_PH3);
#endif

#endif

}


PRIVATE VOID camerap_WriteReg(CAM_REG_T RegWrite)
{
    if(protect ==1) hal_HstSendEvent(0xccccdddd);

#if 1
    if(HAL_ERR_NO != hal_I2cSendByte(g_camdI2cBusId,0x6e,RegWrite.Addr, RegWrite.Data))
        hal_DbgAssert("PAS6175_3a01: i2c no ack when writing ");

#else

    UINT8 i = 0;
#ifdef I2C_BASED_ON_GPIO
    while (i++ < 3)
        if( pas_gpio_i2c_write_data_sensor(0x40, (UINT8*)(&(RegWrite.Addr)), 1, &(RegWrite.Data), 1) == TRUE)
            return;

    hal_DbgAssert("PAS6175_3a01: i2c no ack when writing reg 0x%x", RegWrite.Addr);

#else
    hal_I2cSendRawByte(g_camdI2cBusId, ((gSensorInfo.i2caddress<<1) & 0xfe), PHASE3_WR_PH1);
    hal_I2cSendRawByte(g_camdI2cBusId, RegWrite.Addr, PHASE3_WR_PH2);
    hal_I2cSendRawByte(g_camdI2cBusId, RegWrite.Data, PHASE3_WR_PH3);
#endif
    // Do delay here using RegWrite.Wait mS
    camerap_Delay(RegWrite.Wait);




#endif

}

PRIVATE VOID camerap_SendRgSeq(const CAM_REG_T *RegList, UINT16 Len)
{
    UINT16 RegCount;
    for (RegCount = 0; RegCount < Len; ++RegCount)
    {
        camerap_WriteReg(RegList[RegCount]);
    }
}





VOID PAS6175_3a01_Output_Format(UINT16 OutputFormat)
{
    camerap_WriteOneReg(0xef,0x02);
    camerap_WriteOneReg(0xC0,OutputFormat);
    camerap_WriteOneReg(0x00,0x01);
}

VOID PAS6175_3a01_Clock_Set(UINT8 Sensor_NP)
{
    UINT32 R_SysClk_freq = 0;
    camerap_WriteOneReg(0xef,0x01);
    camerap_WriteOneReg(0x02,Sensor_NP);
    camerap_WriteOneReg(0x11,0x01);

    Sysclk =26000000/Sensor_NP;

    R_SysClk_freq = Sysclk>>8;
    camerap_WriteOneReg(0xef,0x00);
    camerap_WriteOneReg(0x67,R_SysClk_freq&0x00ff);
    camerap_WriteOneReg(0x68,0x80|((R_SysClk_freq&0xff00)>>8));
    camerap_WriteOneReg(0xed,0x01);
}

VOID PAS6175_3a01_Set_Image_Size(UINT16 HSize_3a01, UINT16 VSize)
{
    UINT8 Cmd_HStart = 0;
    UINT8 Cmd_VStart = 0;
    UINT16 Cmd_HSize = 255;
    UINT16 Cmd_VSize = 335;

    UINT16 R_ISP_HSize = 242;
    UINT16 R_ISP_VSize = 320;

    UINT8 R_AE_Window_X_By4 = 58;
    UINT8 R_AE_Window_Y_By4 = 78;
    UINT8 R_AWB_Window_X_By4 = 58;
    UINT8 R_AWB_Window_Y_By4 = 60;

    UINT8 R_ScalingFIFO_En = 1;
    UINT8 R_ScalingFIFO_out_NP = 4; //For Master Mode,

    UINT16 R_Ptr_ScalingFIFO = 0;

    UINT16 R_FIFO_Hsync = 24;
    UINT16 R_FIFO_Hsize = HSize_3a01;

    UINT16 R_ISP_WOIa_HSize = 242;
    UINT16 R_ISP_WOIa_VSize = 320;
    UINT16 R_ISP_WOIa_HOffset = 0;
    UINT16 R_ISP_WOIa_VOffset = 0;

    UINT16 R_ISP_WOIb_HSize = HSize_3a01;
    UINT16 R_ISP_WOIb_VSize = VSize;
    UINT16 R_ISP_WOIb_HOffset = 0;
    UINT16 R_ISP_WOIb_VOffset = 0;

    UINT8 Scaling = 0;
    UINT8 R_EncDecimationNo = 0;

    UINT8 R_SPI_FIFO_Ptr = 0x1e;


    if((HSize_3a01 == 176)&&(VSize == 220))
    {
        R_Ptr_ScalingFIFO = 144;
        R_ISP_WOIb_VOffset = 6;
        Scaling = 22;
    }
    else if((HSize_3a01 == 176)&&(VSize == 176))
    {
        R_Ptr_ScalingFIFO = 144;
        R_ISP_WOIb_VOffset = 6;
        Scaling = 22;
    }
    else if((HSize_3a01 == 176)&&(VSize == 144))
    {
        R_Ptr_ScalingFIFO = 144;
        R_ISP_WOIb_VOffset = 50;
        Scaling = 22;
    }
    else if((HSize_3a01 == 176)&&(VSize == 88))
    {
        //R_ScalingFIFO_out_NP=4;
        R_Ptr_ScalingFIFO=144;
        R_FIFO_Hsync=24;
        R_FIFO_Hsize=176;

        R_ISP_WOIa_HSize=242;
        R_ISP_WOIa_VSize=320;
        R_ISP_WOIa_HOffset=0;
        R_ISP_WOIa_VOffset=0;

        R_ISP_WOIb_HSize = 176;
        R_ISP_WOIb_VSize = 88;
        R_ISP_WOIb_HOffset = 0;
        R_ISP_WOIb_VOffset=50;
        Scaling=22;
    }
    else if((HSize_3a01 == 160)&&(VSize == 128))
    {
        R_Ptr_ScalingFIFO = 144;
        R_ISP_WOIb_VOffset = 42;
        Scaling = 24;
    }
    else if((HSize_3a01 == 160)&&(VSize == 120))
    {
        R_Ptr_ScalingFIFO = 144;
        R_ISP_WOIb_VOffset = 42;
        Scaling = 24;
    }
    else if((HSize_3a01 == 152)&&(VSize == 124))
    {
        R_Ptr_ScalingFIFO = 144;
        R_ISP_WOIb_VOffset = 43;
        Scaling = 24;
    }
    else if((HSize_3a01 == 144)&&(VSize == 176))
    {
        R_Ptr_ScalingFIFO = 100;
        R_ISP_WOIb_VOffset = 10;
        Scaling = 26;
    }
    else if ((HSize_3a01 == 132)&&(VSize == 162))
    {
        R_Ptr_ScalingFIFO = 100;
        Scaling = 28;
    }
    else if((HSize_3a01 == 128)&&(VSize == 160))
    {
        R_Ptr_ScalingFIFO = 100;
        R_ISP_WOIb_VOffset = 5;
        Scaling = 30;
    }

    else if((HSize_3a01 == 128)&&(VSize == 128))
    {
        R_Ptr_ScalingFIFO = 100;
        R_ISP_WOIb_VOffset = 21;
        Scaling = 30;
    }
    else if ((HSize_3a01 == 128)&&(VSize == 104))
    {
        R_Ptr_ScalingFIFO = 100;
        R_ISP_WOIb_VOffset = 33;
        Scaling = 30;
    }
    else if ((HSize_3a01 == 128)&&(VSize == 96))
    {
        R_Ptr_ScalingFIFO = 100;
        R_ISP_WOIb_VOffset = 37;
        Scaling = 30;
    }
    else if((HSize_3a01 == 124)&&(VSize == 152))
    {
        R_Ptr_ScalingFIFO = 100;
        R_ISP_WOIb_VOffset = 6;
        Scaling = 31;
    }

    else if ((HSize_3a01 == 112)&&(VSize == 96))
    {
        R_Ptr_ScalingFIFO = 100;
        R_ISP_WOIb_VOffset = 27;
        R_EncDecimationNo = 0x11;
        Scaling = 17;//34;
    }
    else if ((HSize_3a01 == 104)&&(VSize == 128))
    {
        R_Ptr_ScalingFIFO = 90;
        R_ISP_WOIb_VOffset = 5;
        Scaling = 37;
    }
    else if ((HSize_3a01 == 96)&&(VSize == 128))
    {
        R_Ptr_ScalingFIFO = 80;
        Scaling = 40;
    }
    else if ((HSize_3a01 == 96)&&(VSize == 112))
    {
        R_Ptr_ScalingFIFO = 80;
        R_ISP_WOIb_VOffset = 8;
        Scaling = 40;
    }
    else if ((HSize_3a01 == 88)&&(VSize == 72))
    {
        R_Ptr_ScalingFIFO = 80;
        R_ISP_WOIb_VOffset = 22;
        R_EncDecimationNo = 0x11;
        Scaling = 22;
    }
    else if ((HSize_3a01 == 80)&&(VSize == 64))
    {
        R_Ptr_ScalingFIFO = 70;
        R_ISP_WOIb_VOffset = 21;
        Scaling = 48;
    }
    else if ((HSize_3a01 == 72)&&(VSize == 88))
    {
        R_Ptr_ScalingFIFO = 60;
        R_ISP_WOIb_VOffset = 8;
        Scaling = 53;
    }
    else if ((HSize_3a01 == 64)&&(VSize == 80))
    {
        R_Ptr_ScalingFIFO = 50;
        R_ISP_WOIb_VOffset = 3;
        Scaling = 60;
    }
    else if ((HSize_3a01 == 64)&&(VSize == 52))
    {
        R_Ptr_ScalingFIFO = 50;
        R_ISP_WOIb_VOffset = 3;
        Scaling = 60;
    }
    else if ((HSize_3a01 == 52)&&(VSize == 64))
    {
        R_Ptr_ScalingFIFO = 40;
        R_ISP_WOIb_HOffset = 4;
        R_ISP_WOIb_VOffset = 8;
        Scaling = 63;
    }
    else if ((HSize_3a01 == 240)&&(VSize == 320))
    {
        R_ScalingFIFO_En = 1;
        R_ScalingFIFO_out_NP = 2;
        R_Ptr_ScalingFIFO = 0;

        R_FIFO_Hsync = 24;
        R_FIFO_Hsize = HSize_3a01;

        R_ISP_WOIa_HSize = 242;
        R_ISP_WOIa_VSize = 320;
        R_ISP_WOIa_HOffset = 0;
        R_ISP_WOIa_VOffset = 0;

        R_ISP_WOIb_HSize = HSize_3a01;
        R_ISP_WOIb_VSize = VSize;
        R_ISP_WOIb_HOffset = 0;
        R_ISP_WOIb_VOffset = 0;
        Scaling = 16;
        R_EncDecimationNo = 0;
        R_SPI_FIFO_Ptr = 0x1e;
    }
    else
    {
        hal_DbgAssert(" error Resolution");
    };


    // Set LPF & Min Lpf

    Hsize_3a01=Cmd_HSize+1;
    Vsize_3a01=Cmd_VSize+16; /* 將Dark Line也包含進來，不加進來可能會造成AE不動作。*/

    camerap_WriteOneReg(0xef,0x00);
    camerap_WriteOneReg(0x8c,0x00);
    camerap_WriteOneReg(0xE0,R_ISP_HSize&0x00ff);   /* R_ISP_HSize[7:0] */
    camerap_WriteOneReg(0xE1,(R_ISP_HSize&0x0300)>>8);      /* R_ISP_HSize[9:8] */
    camerap_WriteOneReg(0xE2,R_ISP_VSize&0x00ff);   /* R_ISP_VSize[7:0] */
    camerap_WriteOneReg(0xE3,(R_ISP_VSize&0x0300)>>8);      /* R_ISP_HSize[7:0] */
    camerap_WriteOneReg(0x15,R_AE_Window_X_By4);        /* R_AE_Window_X[7:0] */
    camerap_WriteOneReg(0x17,R_AE_Window_Y_By4);        /* R_AE_Window_Y[7:0] */
    camerap_WriteOneReg(0x0f,R_AWB_Window_X_By4);   /* R_AWB_Window_X[7:0] */
    camerap_WriteOneReg(0x11,R_AWB_Window_Y_By4);   /* R_AWB_Window_Y[7:0] */
    camerap_WriteOneReg(0xed,0x01);

    camerap_WriteOneReg(0xef,0x01);
    camerap_WriteOneReg(0x17,Cmd_HStart);       /* Cmd_HStart[7:0] */
    camerap_WriteOneReg(0x19,Cmd_VStart);       /* Cmd_VStart[7:0] */
    camerap_WriteOneReg(0x1b,Cmd_HSize&0x00FF);         /* Cmd_HSize[7:0] */
    camerap_WriteOneReg(0x1c,(Cmd_HSize&0x0100)>>8);            /* Cmd_VSize[7:0] */
    camerap_WriteOneReg(0x1d,Cmd_VSize&0x00FF);         /* Cmd_HSize[7:0] */
    camerap_WriteOneReg(0x1e,(Cmd_VSize&0x0100)>>8);            /* Cmd_VSize[7:0] */
    camerap_WriteOneReg(0x11,0x01);

    camerap_WriteOneReg(0xef,0x02);
    camerap_WriteOneReg(0xA3,R_ScalingFIFO_En); /* R_ScalingFIFO_En */
    camerap_WriteOneReg(0xA4,R_ScalingFIFO_out_NP); /* R_ScalingFIFO_En */
    camerap_WriteOneReg(0xA5,(R_Ptr_ScalingFIFO&0x0100)>>8);    /* R_Ptr_ScalingFIFO[8] */
    camerap_WriteOneReg(0xA6,R_Ptr_ScalingFIFO&0x00FF); /* R_Ptr_ScalingFIFO[7:0] */
    camerap_WriteOneReg(0xA8,(R_FIFO_Hsync&0x0100)>>8);     /* R_FIFO_Hsync[8] */
    camerap_WriteOneReg(0xA9,R_FIFO_Hsync&0x00FF);      /* R_FIFO_Hsync[7:0] */
    camerap_WriteOneReg(0xAA,(R_FIFO_Hsize&0x0100)>>8);     /* R_FIFO_Hsize[8] */
    camerap_WriteOneReg(0xAB,R_FIFO_Hsize&0x00FF);      /* R_FIFO_Hsize[7:0] */
    camerap_WriteOneReg(0x9b,(R_ISP_WOIa_HSize&0x0100)>>8); /* R_ISP_WOIa_HSize[8] */
    camerap_WriteOneReg(0x9c,R_ISP_WOIa_HSize&0x00FF);  /* R_ISP_WOIa_HSize[7:0] */
    camerap_WriteOneReg(0x9d,(R_ISP_WOIa_VSize&0x0100)>>8); /* R_ISP_WOIa_VSize[8] */
    camerap_WriteOneReg(0x9e,R_ISP_WOIa_VSize&0x00FF);  /* R_ISP_WOIa_VSize[7:0] */
    camerap_WriteOneReg(0x9f,(R_ISP_WOIa_HOffset&0x0100)>>8);   /* R_ISP_WOIb_HOffset[8] */
    camerap_WriteOneReg(0xa0,R_ISP_WOIa_HOffset&0x00FF);    /* R_ISP_WOIb_HOffset[7:0] */
    camerap_WriteOneReg(0xa1,(R_ISP_WOIa_VOffset&0x0100)>>8);   /* R_ISP_WOIb_VOffset[8] */
    camerap_WriteOneReg(0xa2,R_ISP_WOIa_VOffset&0x00FF);    /* R_ISP_WOIb_VOffset[7:0] */
    camerap_WriteOneReg(0xb4,(R_ISP_WOIb_HSize&0x0300)>>8); /* R_ISP_WOIb_HSize[9:8] */
    camerap_WriteOneReg(0xb5,R_ISP_WOIb_HSize&0x00FF);  /* R_ISP_WOIb_HSize[7:0] */
    camerap_WriteOneReg(0xb6,(R_ISP_WOIb_VSize&0x0300)>>8); /* R_ISP_WOIb_VSize[9:8] */
    camerap_WriteOneReg(0xb7,R_ISP_WOIb_VSize&0x00FF);  /* R_ISP_WOIb_VSize[7:0] */
    camerap_WriteOneReg(0xb8,(R_ISP_WOIb_HOffset&0x0100)>>8);   /* R_ISP_WOIb_HOffset[8] */
    camerap_WriteOneReg(0xb9,R_ISP_WOIb_HOffset&0x00FF);    /* R_ISP_WOIb_HOffset[7:0] */
    camerap_WriteOneReg(0xba,(R_ISP_WOIb_VOffset&0x0100)>>8);   /* R_ISP_WOIb_VOffset[8] */
    camerap_WriteOneReg(0xbb,R_ISP_WOIb_VOffset&0x00FF);    /* R_ISP_WOIb_VOffset[7:0] */
    camerap_WriteOneReg(0xb0,0x80|Scaling);     /* Scaling */
    camerap_WriteOneReg(0xb1,0x80|Scaling);     /* Scaling */
    camerap_WriteOneReg(0xb2,R_EncDecimationNo);        /*R_EncDecimationNo_Y[3:0],R_EncDecimationNo_X[3:0] */
    camerap_WriteOneReg(0xC5,R_SPI_FIFO_Ptr);
    camerap_WriteOneReg(0x00,0x01);
}


void PAS6175_3a01_Set_H_Blank(UINT32 HBlank)
{
    return ;
    UINT32 lpf_min = 0;

    camerap_WriteOneReg(0xef,0x01);
    camerap_WriteOneReg(0x03,HBlank&0xFF);
    camerap_WriteOneReg(0x80,HBlank>>8);
    camerap_WriteOneReg(0x11,0x01);
    if(capture_state == TRUE)//Capture Mode
        return;

    if((PAS6167_3a01_Output_Continuous == TRUE) ||(PAS6175_3a01_LCD_BW==TRUE))
        lpf_min = 366;
    else
        lpf_min = 400;  //For Output One Frame
    // 500 => 9.25 fps
    // 400 => 11.49 fps
    lpf_min = 366;
#if 1
    camerap_WriteOneReg(0xef,0x01);
    camerap_WriteOneReg(0x04,lpf_min&0x00FF);       /* Cmd_Lpf[7:0] */
    camerap_WriteOneReg(0x05,(lpf_min&0x0700)>>8);  /* Cmd_Lpf[13:8] */
    camerap_WriteOneReg(0x0E,0x09);     /* Cmd_OffNy[7:0] */
    camerap_WriteOneReg(0x0F,0x00); /* Cmd_OffNy[13:8] */
    camerap_WriteOneReg(0x11,0x01);

    camerap_WriteOneReg(0xef,0x00);
    camerap_WriteOneReg(0x13,lpf_min&0x00FF);               /* R_lpf_min[7:0] */
    camerap_WriteOneReg(0x14,(9<<4)|((lpf_min&0x0700)>>8)); /* R_lpf_min[10:8] */
    camerap_WriteOneReg(0xed,0x01);
#endif
}

UINT32 PAS6175_3a01_Read_Shutter(void)
{
    return TRUE;
    UINT16 nov_size = 0;
    UINT32 ex_pixels = 0;
    camerap_WriteOneReg(0xef,0x01);
    lpf_value_3a01 = ((camerap_ReadReg(0x05)&0x3F)<<8)|camerap_ReadReg(0x04);   /* Cmd_Lpf[13:0] */
    ny_value_3a01 = ((camerap_ReadReg(0x0f)&0x3F)<<8)|camerap_ReadReg(0x0e);    /* Cmd_OffNy[13:0] */
    ne_value_3a01 = (camerap_ReadReg(0x91)<<8)|camerap_ReadReg(0x0d);           /* Cmd_OffNe[15:0] */
    nov_size = (camerap_ReadReg(0x80)<<8)|camerap_ReadReg(0x03);            /* Cmd_Nov_Size[15:0] */

    ex_pixels = (lpf_value_3a01+1-ny_value_3a01)*(Hsize_3a01+nov_size+1)-ne_value_3a01;

    return ex_pixels;
}

void PAS6175_3a01_Write_Shutter(UINT32 ex_pixels)
{
    return ;
    UINT16 lpf = 0;
    UINT16 ny = 0;
    UINT16 ne = 0;
    UINT16 shutter = 0;
    UINT16 line_size = 0;
    UINT8 AE_stage = 0;
    UINT8 AG_stage = 0;
    camerap_WriteOneReg(0xef,0x01);
    if(preview_state ==TRUE)
    {
        /* 如果是預覽模式，回填剛剛拍照時所以讀取的曝光參數。 */
        camerap_WriteOneReg(0x0d,(ne_value_3a01&0x00ff));       /* Cmd_OffNe[7:0] */
        camerap_WriteOneReg(0x91,(ne_value_3a01&0xff00)>>8);    /* Cmd_OffNe[15:8] */
        camerap_WriteOneReg(0x04,(lpf_value_3a01&0x00ff));      /* Cmd_Lpf[7:0] */
        camerap_WriteOneReg(0x05,(lpf_value_3a01&0x3f00)>>8);   /* Cmd_Lpf[13:8] */
        camerap_WriteOneReg(0x0e,(ny_value_3a01&0x00ff));       /* Cmd_OffNy[7:0] */
        camerap_WriteOneReg(0x0f,(ny_value_3a01&0x3f00)>>8);    /* Cmd_OffNy[13:8] */
    }
    else
    {
        line_size = Hsize_3a01 +  dummy_pixels_3a01+1;//((read_cmos_sensor(0x80)<<8)|read_cmos_sensor(0x03));

        if(ex_pixels > line_size)/*  超過一條 */
        {
            shutter = ex_pixels/line_size;
            shutter++;
        }
        else
            shutter = 1;

        lpf = lpf_value_3a01; //366;

        ny = lpf - shutter + 1;

        ne=  (lpf+1-ny)*line_size- ex_pixels;

        if(ny%2!=0)
        {
            lpf+=1;
            ny+=1;
        }

        camerap_WriteOneReg(0x0d,(ne&0x00ff));      /* Cmd_OffNe[7:0] */
        camerap_WriteOneReg(0x91,(ne&0x1f00)>>8);   /* Cmd_OffNe[15:8] */
        camerap_WriteOneReg(0x04,(lpf&0x00ff));     /* Cmd_Lpf[7:0] */
        camerap_WriteOneReg(0x05,(lpf&0x3f00)>>8);  /* Cmd_Lpf[13:8] */
        camerap_WriteOneReg(0x0e,(ny&0x00ff));      /* Cmd_OffNy[7:0] */
        camerap_WriteOneReg(0x0f,(ny&0x3f00)>>8);   /* Cmd_OffNy[13:8] */
    }
    camerap_WriteOneReg(0x11,0x01);
//解在高亮度拍照回預覽會畫面過曝的問題
    if(preview_state == TRUE)
    {
        camerap_WriteOneReg(0xef,0x00);
        AG_stage=camerap_ReadReg(0xBC);
        AE_stage=camerap_ReadReg(0xBD);

        if(AG_stage<=1)
            camerap_WriteOneReg(0xBC,2);
        else
            camerap_WriteOneReg(0xBC,AG_stage-1);

        if(AE_stage<=1)
            camerap_WriteOneReg(0xBD,2);
        else
            camerap_WriteOneReg(0xBD,AE_stage-1);

        camerap_WriteOneReg(0xed,0x01);
    }
}

VOID PAS6175_3a01_AWB_Mode(BOOL AWB_Enable)
{
    return TRUE;
    UINT8 Reg_vlaue = 0;
    camerap_WriteOneReg(0xef,0x00);
    Reg_vlaue = camerap_ReadReg(0x72);
    if(AWB_Enable == TRUE)
        Reg_vlaue |= 0x01;
    else
        Reg_vlaue &= 0xfe;

    camerap_WriteOneReg(0x72,Reg_vlaue);


}

void PAS6175_3a01_AE_Mode(BOOL AE_Enable)
{
    return TRUE;
    UINT8 Reg_vlaue = 0;

    camerap_WriteOneReg(0xef,0x00);
    Reg_vlaue = camerap_ReadReg(0x66);


    if(AE_Enable == TRUE)
        Reg_vlaue |= 0x10;
    else
        Reg_vlaue &= 0xef;

    camerap_WriteOneReg(0x66,Reg_vlaue);
    camerap_WriteOneReg(0xed,0x01);




}

VOID PAS6175_3a01_Skip_Frame(UINT8 SkipFrameNumber)
{
    return TRUE;
    UINT8 FrameCnt1 = 0;
    UINT8 FrameCnt2 = 0;
    UINT8 SkipFrameCnt = 0;
    UINT32 i = 0;
    UINT32 j = 0;


    SkipFrameCnt = SkipFrameNumber;

    camerap_WriteOneReg(0xef,0x00);

    for(j=0; j<(SkipFrameCnt/3); j++)
    {
        FrameCnt1 = (camerap_ReadReg(0xb9)&0x70)>>4;

        for(i=0; i<0xffffffff; i++)
        {
            FrameCnt2 = (camerap_ReadReg(0xb9)&0x70)>>4;

            if((FrameCnt1-FrameCnt2>=3)||(FrameCnt2-FrameCnt1>=3))
                break;
        };
    }

    SkipFrameCnt -= (j*3);

    if(SkipFrameCnt == 0)
        return;

    FrameCnt1 = (camerap_ReadReg(0xb9)&0x70)>>4;

    for(i=0; i<0xffffffff; i++)
    {
        FrameCnt2 = (camerap_ReadReg(0xb9)&0x70)>>4;

        if((FrameCnt1-FrameCnt2>=SkipFrameCnt)||(FrameCnt2-FrameCnt1>=SkipFrameCnt))
            break;
    };
}


void PAS6175_3a01_SPI_Slave_Mode_Enable(void)
{
    return TRUE;
    camerap_WriteOneReg(0xef,0x00);
    camerap_WriteOneReg(0x8C,0x00);
    camerap_WriteOneReg(0xef,0x02);// enter slave mode
    camerap_WriteOneReg(0xc1,0x00);//
    camerap_WriteOneReg(0xbf,0x00);//  YC swap [2]
    camerap_WriteOneReg(0xc4,0x04);//{ 5'b0,R_SPI_Slave_En,R_SPI_SSn_VsyncTim, R_SPI_En };
//write_cmos_sensor(0xC5,0x1e);// R_SPI_FIFO_Ptr[5:0]
    camerap_WriteOneReg(0xC6,0x01);//
    camerap_WriteOneReg(0xa4,0x02);//
    camerap_WriteOneReg(0xa5,0x00);//


//  camerap_WriteOneReg(0xa6,0x04);//set ready trig when > n words .  2=>32words
    camerap_WriteOneReg(0xa6,0x20);//set ready trig when > n words .  2=>32words
    camerap_WriteOneReg(0x00,0x01);
}

void PAS6175_3a01_SPI_Master_Mode_Enable(void)
{
    return TRUE;

    camerap_WriteOneReg(0xef,0x00);
    camerap_WriteOneReg(0x8C,0x00);
    camerap_WriteOneReg(0xef,0x02);
    camerap_WriteOneReg(0xc1,0x10);
    camerap_WriteOneReg(0xC4,0x43);//SPI Master Mode enable
    camerap_WriteOneReg(0xC6,0x02);//SPI Master Mode enable
    camerap_WriteOneReg(0xa4,0x04);//
    camerap_WriteOneReg(0xa5,0x00);//
//  write_cmos_sensor(0xa6,0x64);//
    camerap_WriteOneReg(0x00,0x01);
}


VOID PAS6167_3a01_Output_Enable(BOOL Output_Enable)
{
    return TRUE;
    UINT8 Reg_vlaue = 0;
    camerap_WriteOneReg(0xef,0x02);
    if(Output_Enable == TRUE)
        Reg_vlaue = 1;
    else
        Reg_vlaue = 0;
    camerap_WriteOneReg(0xbf,Reg_vlaue);//camerap_WriteOneReg(0xbf,0x02); //output one frame
    camerap_WriteOneReg(0x00,0x01);
}


void PAS6175_3a01_Output_Enable(BOOL Output_Enable)
{
    return TRUE;
    UINT8 Reg_vlaue = 0;


    camerap_WriteOneReg(0xef,0x02);
    if(Output_Enable == TRUE)
        Reg_vlaue = 1;
    else
        Reg_vlaue = 0;
    camerap_WriteOneReg(0xbd,Reg_vlaue);
    camerap_WriteOneReg(0x00,0x01);
}

void PAS6175_3a01_IO_TriState(BOOL TriState_Enable)
{
    return TRUE;
    UINT8 Reg_vlaue = 0;

    camerap_WriteOneReg(0xef,0x01);
    Reg_vlaue = camerap_ReadReg(0x22);

    Reg_vlaue= 0x8;// for test ytt
    if(TriState_Enable == TRUE)
        Reg_vlaue |= 0x04;  // chris 0906
    else
        Reg_vlaue &= 0xFB;  // chris 0906

    camerap_WriteOneReg(0x22,Reg_vlaue);
}

UINT32 PAS6175_3a01_Set_Para_WB(UINT32 para)
{
    return TRUE;
    UINT8 Index = 0;
    UINT8 temp = 0;
    camerap_WriteOneReg(0xef,0x01);
    temp = camerap_ReadReg(0x80);
    //hal_HstSendEvent(0x77013);
// hal_HstSendEvent((UINT32)temp); // first 0x0;
    if(para == 0)
    {
        camerap_WriteOneReg(0xef,0x01);
        camerap_WriteOneReg(0x06,Cmd_Cgn_Gb);//Cmd_Cgn_Gb
        camerap_WriteOneReg(0x07,Cmd_Cgn_B);//Cmd_Cgn_B
        camerap_WriteOneReg(0x08,Cmd_Cgn_Gr);//Cmd_Cgn_Gr
        camerap_WriteOneReg(0x09,Cmd_Cgn_R);//Cmd_Cgn_R
        camerap_WriteOneReg(0x11,0x01);

        camerap_WriteOneReg(0xef,0x00);
        camerap_WriteOneReg(0x72,0x01);
        camerap_WriteOneReg(0xed,0x01);
        camerap_WriteOneReg(0x80,temp|0x78);  // set[11~14] to 1
        camerap_WriteOneReg(0xed,0x01);

        return TRUE;
    }

    if(preview_state==TRUE)
    {
        //if(para!=CAM_WB_AUTO)
        {
            camerap_WriteOneReg(0xef,0x00);
            camerap_WriteOneReg(0x72,0x10);
            camerap_WriteOneReg(0xed,0x01);
            camerap_WriteOneReg(0x80, temp&0x87);  // set[11~14] to 0
            camerap_WriteOneReg(0xed,0x01);

        }



        camerap_WriteOneReg(0xef,0x01);
        camerap_WriteOneReg(0x06,PAS6175_3a01_WB_MODE[Index-1][0]);//Cmd_Cgn_Gb
        camerap_WriteOneReg(0x07,PAS6175_3a01_WB_MODE[Index-1][1]);//Cmd_Cgn_B
        camerap_WriteOneReg(0x08,PAS6175_3a01_WB_MODE[Index-1][2]);//Cmd_Cgn_Gr
        camerap_WriteOneReg(0x09,PAS6175_3a01_WB_MODE[Index-1][3]);//Cmd_Cgn_R
        camerap_WriteOneReg(0x11,0x01);
    }
    return TRUE;

}


UINT32 PAS6175_3a01_Set_Para_Effect(UINT32 iPara)
{
    return TRUE;
    UINT8 index = 0;
    switch (iPara)
    {
        case CAM_EFFECT_ENC_NORMAL:
            index = 0;
            break;

        case CAM_EFFECT_ENC_GRAYSCALE:
            index = 1;
            break;

        case CAM_EFFECT_ENC_SEPIA:
            index = 2;
            break;

        case CAM_EFFECT_ENC_COLORINV:
            index = 3;
            break;

        case CAM_EFFECT_ENC_SEPIAGREEN:
            index = 4;
            break;

        case CAM_EFFECT_ENC_SEPIABLUE:
            index = 5;
            break;

        default:
            index = 0; //CAM_EFFECT_ENC_NORMAL
            break;
    }
    camerap_WriteOneReg(0xef,0x00);
    if(index == 0)
    {
        camerap_WriteOneReg(0x94,0x00);//R_ISP_ImgEffect_En
    }
    else
    {
        camerap_WriteOneReg(0x8f,PAS6175_3a01_Effect[index-1][2]);//R_ImgEffect_c0
        camerap_WriteOneReg(0x90,PAS6175_3a01_Effect[index-1][3]);//R_ImgEffect_c1
        camerap_WriteOneReg(0x93,PAS6175_3a01_Effect[index-1][0]);//R_ImgEffectMode
        camerap_WriteOneReg(0x94,PAS6175_3a01_Effect[index-1][1]);//R_ISP_ImgEffect_En
    }
    camerap_WriteOneReg(0xed,0x01);
    return TRUE;
}

void PAS6175_3a01_NightMode(BOOL bEnable)
{
    return TRUE;
    UINT8 Index = 0;


    if (bEnable == TRUE)
        Index = 1;
    else
        Index = 0;

    camerap_WriteOneReg(0xef,0x00);
    camerap_WriteOneReg(0x6C,PAS6175_3a01_AeStage_GainStage[Index][0]);//R_AE_maxStage[4:0]
    camerap_WriteOneReg(0x6D,PAS6175_3a01_AeStage_GainStage[Index][1]);//R_AG_stage_UB
    camerap_WriteOneReg(0xed,0x01);

    camerap_WriteOneReg(0xef,0x02);
    camerap_WriteOneReg(0x2F,PAS6175_3a01_AeStage_GainStage[Index][2]);//R_AE_stage_LL[4:0]
    camerap_WriteOneReg(0x30,PAS6175_3a01_AeStage_GainStage[Index][3]);//R_AE_stage_NL[4:0]
    camerap_WriteOneReg(0x36,PAS6175_3a01_AeStage_GainStage[Index][4]);//R_Gamma_Strength_Delta[3:0]
//    write_cmos_sensor(0x5F,PAS6175_3a01_AeStage_GainStage[Index][5]);//R_Saturation_LL[4:0]
    camerap_WriteOneReg(0x00,0x01);
}


enum
{
    CAM_EV_NEG_4_3 = 0,
    CAM_EV_NEG_3_3,
    CAM_EV_NEG_2_3,
    CAM_EV_NEG_1_3,
    CAM_EV_ZERO,
    CAM_EV_POS_1_3,
    CAM_EV_POS_2_3,
    CAM_EV_POS_3_3,
    CAM_EV_POS_4_3,
    CAM_EV_NIGHT_SHOT,
    CAM_NO_OF_EV
};

UINT32 PAS6175_3a01_Set_Para_EV(UINT32 iPara)
{
    return TRUE;
    UINT8 Brightness_Offset = 0;

    switch (iPara)
    {
        case CAM_EV_NEG_4_3:
            Brightness_Offset = 255-80;
            break;

        case CAM_EV_NEG_3_3:
            Brightness_Offset = 255-60;
            break;

        case CAM_EV_NEG_2_3:
            Brightness_Offset = 255-40;
            break;

        case CAM_EV_NEG_1_3://EV-1
            Brightness_Offset = 255-20;
            break;

        case CAM_EV_ZERO:   //EV 0

            break;

        case CAM_EV_POS_1_3://EV +1
            Brightness_Offset = 20;
            break;

        case CAM_EV_POS_2_3:
            Brightness_Offset = 40;
            break;

        case CAM_EV_POS_3_3:
            Brightness_Offset = 60;
            break;

        case CAM_EV_POS_4_3:
            Brightness_Offset = 80;
            break;

        default:
            return FALSE;

    }
    camerap_WriteOneReg(0xef,0x02);
    Brightness_Value_LL = camerap_ReadReg(0x69);

    Brightness_Value_NL = camerap_ReadReg(0x6A);

    camerap_WriteOneReg(0x69,Brightness_Value_LL + Brightness_Offset);
    camerap_WriteOneReg(0x6A,Brightness_Value_NL + Brightness_Offset);
    camerap_WriteOneReg(0x00,0x01);

    return TRUE;
}



BOOL g_bBanding50Hz_3a01 = TRUE;
enum
{
    CAM_BANDING_50HZ=0,
    CAM_BANDING_60HZ,
    CAM_NO_OF_BANDING
};


UINT32 PAS6175_3a01_Set_Para_Banding(UINT32 iPara)
{
    UINT8 Reg_vlaue = 0;

    return 1;
    camerap_WriteOneReg(0xef,0x00);
    Reg_vlaue = camerap_ReadReg(0x66)&0xFE;

    switch (iPara)
    {
        case CAM_BANDING_50HZ:
            g_bBanding50Hz_3a01 = TRUE;
            break;

        case CAM_BANDING_60HZ:
            g_bBanding50Hz_3a01 = FALSE;
            break;

        default:
            return FALSE;
    }
    camerap_WriteOneReg(0x66,Reg_vlaue|(!g_bBanding50Hz_3a01));
    camerap_WriteOneReg(0xed,0x01);

    return TRUE;
}


PRIVATE VOID camerap_PowerOnForPreview(CAM_SIZE_T CamOutSize, UINT16 IspOutWidth, UINT16 IspOutHeight, CAM_FORMAT_T Format)
{

}
UINT8 test_color_3a01 = 3;
void PAS6175_3a01_Previewqqvga(UINT16 image_target_width, UINT16 image_target_height,UINT8 clk_NP,UINT32 HBlank)
{

}



void PAS6175_3a01_Preview(UINT16 image_target_width, UINT16 image_target_height,UINT8 clk_NP,UINT32 HBlank)
{

    camerap_PowerOnForPreview(CAM_NPIX_QVGA,128,160,CAM_FORMAT_RGB565);

#if  1
    test_color_3a01++;
    if(test_color_3a01 ==7) test_color_3a01 = 0;
    camerap_WriteOneReg(0xef,0x0);
    camerap_WriteOneReg(0x0c,test_color_3a01);//test_color
    camerap_WriteOneReg(0xed,0x01);
    if(test_color_3a01==0) test_color_3a01=5;
#endif

#if 0
    PAS6175_3a01_Output_Enable(FALSE);
    PAS6175_3a01_IO_TriState(FALSE);
    PAS6175_3a01_AE_Mode(FALSE);
    //UINT32 exposure_pixels_3a01 = PAS6175_3a01_Read_Shutter();
#if  0
    test_color++;
    if(test_color ==7) test_color = 0;
    camerap_WriteOneReg(0xef,0x0);
    camerap_WriteOneReg(0x0c,test_color);//test_color
    camerap_WriteOneReg(0xed,0x01);
    if(test_color==0) test_color=5;
#endif

    dummy_pixels = HBlank;
    preview_pclk_division = clk_NP;
    // PAS6175_3a01_Output_Format(0x10);
    PAS6175_3a01_Output_Format(0x0);


    PAS6175_3a01_Clock_Set(clk_NP);//preview_pclk_division


//   PAS6175_3a01_Set_Image_Size(image_target_width,image_target_height);

    PAS6175_3a01_Set_Image_Size(240,320);

    PAS6175_3a01_Set_H_Blank(HBlank);//60 preview
//  if(lpf_value_3a01!=0)
    // PAS6175_3a01_Write_Shutter(exposure_pixels_3a01);


    PAS6175_3a01_AWB_Mode(TRUE);
    PAS6175_3a01_AE_Mode(TRUE);
    //PAS6175_3a01_AE_Mode(FALSE);
#if 1
    PAS6175_3a01_Set_Para_WB(0);
    PAS6175_3a01_Set_Para_Effect(0);//CAM_EFFECT_ENC_GRAYSCALE);
    PAS6175_3a01_Set_Para_EV(4);
    PAS6175_3a01_NightMode(FALSE);
    PAS6175_3a01_Set_Para_Banding(0);
#endif
    // turn on AE write to bank1
    camerap_WriteOneReg(0xef,0x00);
    camerap_WriteOneReg(0x7f,0xff);
    camerap_WriteOneReg(0x80,0xfe);
    PAS6175_3a01_Skip_Frame(5);
    PAS6175_3a01_SPI_Master_Mode_Enable();

    // PAS6175_3a01_IO_TriState(TRUE);

    PAS6175_3a01_IO_TriState(FALSE);
    PAS6175_3a01_Output_Enable(TRUE);
#endif
}


/*寄存器位置在Bank 2的0xC2 [Bit 6:4]，一般3個bit，0 表示不delay，7則是最
大delay。*/
void PAS6175_3a01_Clkdelay(UINT8 pclk_delay)
{

    camerap_WriteOneReg(0xef,0x02);
    camerap_WriteOneReg(0xc2,pclk_delay<<4);
    camerap_WriteOneReg(0x00,0x01);

}

//UINT8  clkP_delay = 0;
uint16 tmp=0;

PRIVATE void camerap_PowerOn(CAM_SIZE_T CamOutSize, UINT16 IspOutWidth, UINT16 IspOutHeight, CAM_FORMAT_T Format)
{
    hal_HstSendEvent(0x4444cce);
    HAL_CAMERA_CFG_T CamConfig = {0,};
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};
    UINT16 camOutWidth = 0, camOutHeight = 0;

    SXS_TRACE(TSTDOUT,"DRV_CAM:1 Power On, camOutSize %d ispw %d isph %d", CamOutSize, IspOutWidth, IspOutHeight);

    CamConfig.rstActiveH = FALSE;
    CamConfig.pdnActiveH = TRUE;
    CamConfig.dropFrame = FALSE;
#if 0// def PAS6175_3a01_FAST
    CamConfig.camClkDiv = 8;     // 156/6 = 26M
    //#else
    CamConfig.camClkDiv = 12;     // 156/6 = 26M
#endif

    if (CamOutSize == CAM_NPIX_QQVGA)
        CamConfig.camClkDiv = 9;
    else
        CamConfig.camClkDiv = 12;  // 156/10 = 15.6M

    CamConfig.endianess = NO_SWAP;
    CamConfig.camId = camera_GetCameraID();
    CamConfig.cropEnable = FALSE;
#ifdef _CAM_USE_DCT_
    CamConfig.camDctEnable = TRUE;
#endif
    CamConfig.colRatio= COL_RATIO_1_1;
    CamConfig.rowRatio= ROW_RATIO_1_1;
    CamConfig.vsync_inv = TRUE;
    CamConfig.spi_pixels_per_line = IspOutWidth;
    CamConfig.spi_pixels_per_column = IspOutHeight;
    CamConfig.spi_yuv_out = SPI_OUT_V0_Y1_U0_Y0;
    CamConfig.spi_mode = SPI_MODE_MASTER2_1;// s_gc6113_mode;
    CamConfig.spi_little_endian_en = FALSE;
    CamConfig.spi_ctrl_clk_div = 1;

    pmd_EnablePower(PMD_POWER_CAMERA,TRUE);

    hal_CameraOpen(&CamConfig);
    hal_CameraIrqSetMask(mask);
    hal_CameraPowerDown(FALSE);

#ifdef I2C_BASED_ON_GPIO
    gc6113_gpio_i2c_init_sensor();
#else
    g_camdI2cBusId =HAL_I2C_BUS_ID_2;//tgt_GetCamdConfig()->i2cBusId;
    hal_I2cOpen(g_camdI2cBusId);
#endif

    if (!gSensorInfo.snrrst)
        hal_CameraReset(FALSE);

    //  camerap_SendRgSeq(RG_Init_PowerOn,  (sizeof(RG_Init_PowerOn) / sizeof(CAM_REG_T)));
    switch (s_gc6113_mode)
    {
        case SPI_MODE_MASTER2_1:
            SXS_TRACE(TSTDOUT,"spi master 2_1 mode in use");
            //camerap_SendRgSeq(RG_Reinit_MASTER2_1,  (sizeof(RG_Reinit_MASTER2_1) / sizeof(CAM_REG_T)));
            break;
        case SPI_MODE_MASTER2_2:
            SXS_TRACE(TSTDOUT,"spi master 2_2 mode in use");
            //camerap_SendRgSeq(RG_Reinit_MASTER2_2,  (sizeof(RG_Reinit_MASTER2_2) / sizeof(CAM_REG_T)));
            break;
        case SPI_MODE_NO:
            SXS_TRACE(TSTDOUT,"no spi master in use");
            break;
        case SPI_MODE_MASTER1:  // i bit mode with ssn
            SXS_TRACE(TSTDOUT,"master 1bit with ssn in use");
            break;
        default:
            SXS_TRACE(TSTDOUT,"unsupported spi mode");
            break;
    }

#if defined(_CAM_SPECIAL_GC6113_)
    s_camOutMode = CAM_OUT_QQVGA;
#else
    s_prevCount = 0;
#endif
    s_camOpened = TRUE;

    if (CamOutSize == CAM_NPIX_QQVGA)
    {
        hal_HstSendEvent(0x6666cccf);
        camerap_SendRgSeq(RG_InitPowerOnQvgainit,  (sizeof(RG_InitPowerOnQvgainit) / sizeof(CAM_REG_T)));
        //camerap_SendRgSeq(RG_Init_QQVGA128160,  (sizeof(RG_Init_QQVGA128160) / sizeof(CAM_REG_T)));
        camerap_SendRgSeq(RG_Init_QVGA320240,  (sizeof(RG_Init_QVGA320240) / sizeof(CAM_REG_T)));//// added by zhou 20140721
        camerap_SendRgSeq(RG_Init_QQVGA128160,  (sizeof(RG_Init_QQVGA128160) / sizeof(CAM_REG_T))); //// added by zhou 20140723

    }

    if (CamOutSize == CAM_NPIX_QVGA)
    {
        hal_HstSendEvent(0x9999ccee);
        camerap_SendRgSeq(RG_InitPowerOnQvgainit,  (sizeof(RG_InitPowerOnQvgainit) / sizeof(CAM_REG_T)));
        camerap_SendRgSeq(RG_Init_QVGA320240,  (sizeof(RG_Init_QVGA320240) / sizeof(CAM_REG_T)));
//                  camerap_WriteOneReg(0xfe,0x01);
//        hal_HstSendEvent(0xeeff0000|camerap_ReadReg(0x51));
        camerap_WriteOneReg(0xfe,0x01);
        tmp =    camerap_ReadReg(0x5d);
        hal_HstSendEvent(0xeeff0000|tmp);
    }
    protect  =1 ;
}

PRIVATE void camerap_PowerOff(void)
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};
    protect  =0 ;
    if (s_camOpened)
    {
        hal_CameraIrqSetMask(mask);
        // disable IFC
        if(gIsCameraStartXfer == TRUE)
        {
            hal_CameraStopXfer(TRUE);
            gSensorInfo.cammode = CAM_MODE_IDLE;
            gIsCameraStartXfer = FALSE;
        }
        // Disable Camera Interface Module
        hal_CameraClose();

        // Disable I2C
#ifdef I2C_BASED_ON_GPIO
        gpio_i2c_close();
#else
        hal_I2cClose(g_camdI2cBusId);
#endif
        // Disable the LDO for the camera
        pmd_EnablePower(PMD_POWER_CAMERA, FALSE);
        s_camOpened = FALSE;
    }
#if defined(_CAM_SPECIAL_GC6113_)
    s_camOutMode = CAM_OUT_NULL;
#else
    s_prevCount = 0;
#endif
}

PRIVATE void camerap_Reserve(CAM_IRQ_HANDLER_T FrameReady)
{
    gSensorInfo.camirqhandler = FrameReady;
    hal_CameraIrqSetHandler(camerap_InteruptHandler);
}

PRIVATE void camerap_PrepareImageCaptureL(CAM_FORMAT_T ImageFormat, UINT32 SizeIndex, CAM_IMAGE_T Buffer)
{
// hal_HstSendEvent(0x12345603);
#if !defined(_CAM_SPECIAL_GC6113_)
    if (s_prevCount == 0)
#endif
    {
        HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};

        hal_CameraIrqSetMask(mask);

        if (gIsCameraStartXfer == TRUE)
        {
            hal_CameraStopXfer(FALSE);
            gIsCameraStartXfer = FALSE;
        }

        gSensorInfo.cammode = CAM_MODE_PREVIEW_STAGE_1;
        gSensorInfo.vidbuffer = Buffer;
        gSensorInfo.format = ImageFormat;
        gSensorInfo.npixels = SizeIndex;

#if defined(_CAM_SPECIAL_GC6113_)
        if (s_camOutMode != CAM_OUT_QQVGA)
        {
            // change the clock divider
            aadasd         camerap_SendRgSeq(RG_Reinit_QQVGA, (sizeof(RG_Reinit_QQVGA) / sizeof(CAM_REG_T)));
            hal_CameraSetupClockDivider(6);
            s_camOutMode = CAM_OUT_QQVGA;
        }
        mask.fstart = 1;
#else
        s_prevCount = 2;
        mask.fend = 1;
#endif // _CAM_SPECIAL_GC6113_

        hal_CameraIrqSetMask(mask);
        hal_CameraControllerEnable(TRUE);
    }
#if !defined(_CAM_SPECIAL_GC6113_)
    else
    {
        UINT32 status = hal_SysEnterCriticalSection();
        s_prevCount++;
        hal_SysExitCriticalSection(status);
    }
#endif
}

PRIVATE void camerap_CaptureImage()
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};
    hal_HstSendEvent(0x12345602);
    hal_CameraIrqSetMask(mask);
    if (gIsCameraStartXfer == TRUE)
    {
        hal_CameraStopXfer(FALSE);
        gIsCameraStartXfer = FALSE;
    }
    gSensorInfo.cammode = CAM_MODE_CAPTURE_STAGE_1;
#if defined(_CAM_SPECIAL_GC6113_)
    if (s_camOutMode != CAM_OUT_QVGA)
    {
        uint16 tmp_val_1=0,tmp_val_2=0;
        uint32 dest_val=0;

        camerap_WriteOneReg(0xfe,0x00);
        camerap_WriteOneReg(0xe5,0x00);
        tmp_val_1=camerap_ReadReg(0x03);
        tmp_val_2=camerap_ReadReg(0x04);

        dest_val=((tmp_val_1<<8)|(tmp_val_2))*104/260;
        camerap_WriteOneReg(0x03,(dest_val&0xff00)>>8);
        camerap_WriteOneReg(0x04,dest_val&0x00ff);
        camerap_Delay(120);
        hal_CameraSetupClockDivider(15);
        sdfs     camerap_SendRgSeq(RG_Reinit_QVGA, (sizeof(RG_Reinit_QVGA) / sizeof(CAM_REG_T)));
        s_camOutMode = CAM_OUT_QVGA;
    }
    mask.fstart = 1;
#else
    s_prevCount = 0;
    mask.fend = 1;
#endif // _CAM_SPECIAL_GC6113_

    hal_CameraIrqSetMask(mask);
    hal_CameraControllerEnable(TRUE);
}

PRIVATE void camerap_Release(void)
{
    // 'De-initialize' camera
    // Release resource
}

PRIVATE VOID camerap_Delay(UINT16 Wait_mS)
{
    UINT32 Ticks_16K;

    // Here is the delay function used by the camera driver.  It can be changed to use a non-blocking
    // wait if necessary, but for now, just use hal_TimDelay
    Ticks_16K = (16348 * Wait_mS)/1000;
    hal_TimDelay(Ticks_16K);
}


//byd3901 id

//0xfc=0x39,0xfd=0x01
PRIVATE UINT8 camerap_GetId(VOID)
{

    if(protect ==1) return;
    return(camerap_ReadReg(0xfd));
}
// ask PAS FAE to get the regster list.

PRIVATE void camerap_SetContrastL(UINT32 Contrast)
{
    switch(Contrast)
    {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default: break;
    }
}

// ask PAS FAE to get the regster list.
PRIVATE void camerap_SetBrightnessL(UINT32 Brightness)
{
    return;
    switch(Brightness)
    {
        case 1:    //0
            camerap_WriteOneReg(0x56,0x40);
            break;
        case 2:   //-2
            camerap_WriteOneReg(0x56,0x20);
            break;
        case 3: //-1
            camerap_WriteOneReg(0x56,0x30);
            break;
        case 4:  //+1
            camerap_WriteOneReg(0x56,0x50);
            break;
        case 5:  //+2
            camerap_WriteOneReg(0x56,0x5d);
            break;
        default :
            break;
    }
}

PRIVATE void camerap_SetFlashL(CAM_FLASH_T Flash)
{
    return;
}
// ask PAS FAE to get the regster list.

PRIVATE void camerap_SetExposureL(CAM_EXPOSURE_T Exposure)
{
    return;
    switch(Exposure)
    {
        case CAM_EXPOSURE_AUTO:     //0
            camerap_WriteOneReg(0x55,0x00);
            break;
        case CAM_EXPOSURE_NEGATIVE_2:   //-2
            camerap_WriteOneReg(0x55,0xf8);

            break;
        case CAM_EXPOSURE_NEGATIVE_1: //-1

            camerap_WriteOneReg(0x55,0xb8);
            break;
        case CAM_EXPOSURE_POSITIVE_1:  //+1
            camerap_WriteOneReg(0x55,0x38);

            break;
        case CAM_EXPOSURE_POSITIVE_2:  //+2
            camerap_WriteOneReg(0x55,0x78);

            break;

        default : break;
    }
}
// ask PAS FAE to get the regster list.

PRIVATE void camerap_SetWhiteBalanceL(CAM_WHITEBALANCE_T WhiteBalance)
{
    UINT8 TempReg;
    return;
    //TempReg = camerap_ReadReg(0x22);
    switch(WhiteBalance)
    {
        case CAM_WB_AUTO:  /*Auto */
            camerap_WriteOneReg(0x13,0x07);
            break;
        case CAM_WB_DAYLIGHT:   /*晴天太阳光*/
            camerap_WriteOneReg(0x13,0x05);
            camerap_WriteOneReg(0x1,0x13);
            camerap_WriteOneReg(0x2,0x26);
            break;
        case CAM_WB_OFFICE:   /*日光灯*/

            break;
        case CAM_WB_CLOUDY:  /*阴天*/
            camerap_WriteOneReg(0x13,0x05);
            camerap_WriteOneReg(0x1,0x10);
            camerap_WriteOneReg(0x2,0x28);
            break;
        case CAM_WB_TUNGSTEN:  /*钨丝灯*/

            break;

        default : break;
    }
}

PRIVATE VOID camerap_SetSpecialEffect(CAM_SPECIALEFFECT_T SpecialEffect)
{

    return;
    switch(SpecialEffect)
    {
        case CAM_SPE_NORMAL :
            camerap_WriteOneReg(0x70,0x0f);
            camerap_WriteOneReg(0x69,0x00);
            camerap_WriteOneReg(0x67,0x80);
            camerap_WriteOneReg(0x68,0x80);
            camerap_WriteOneReg(0xb4,0x60);
            camerap_WriteOneReg(0x98,0x1a);
            camerap_WriteOneReg(0x56,0x40);

            break;

        case CAM_SPE_ANTIQUE    :

            break;

        case CAM_SPE_REDISH :

            break;

        case CAM_SPE_GREENISH:
            camerap_WriteOneReg(0x70,0x0f);
            camerap_WriteOneReg(0x69,0x20);
            camerap_WriteOneReg(0x67,0x60);
            camerap_WriteOneReg(0x68,0x70);
            camerap_WriteOneReg(0xb4,0x60);
            camerap_WriteOneReg(0x98,0x0a);
            camerap_WriteOneReg(0x56,0x40);


            break;

        case CAM_SPE_BLUEISH    :

            break;

        case CAM_SPE_BLACKWHITE :

            break;

        case CAM_SPE_NEGATIVE:

            break;

        case CAM_SPE_BLACKWHITE_NEGATIVE:

            break;

        default: break;
    }
}

PRIVATE VOID camerap_InteruptHandler(HAL_CAMERA_IRQ_CAUSE_T cause)
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0, };
    //hal_HstSendEvent(0x12345609);hal_HstSendEvent(cause);hal_HstSendEvent(gSensorInfo.cammode);
    switch (gSensorInfo.cammode)
    {
        case CAM_MODE_PREVIEW_STAGE_1:
#if defined(_CAM_SPECIAL_GC6113_)
            if (cause.fstart)
#else
            if (cause.fend)
#endif
            {
                hal_CameraIrqSetMask(mask);

                hal_CameraStartXfer((gSensorInfo.npixels<<1), (UINT8*)(gSensorInfo.vidbuffer));
                gIsCameraStartXfer = TRUE;

                mask.fend = 1;
                hal_CameraIrqSetMask(mask);

                gSensorInfo.cammode = CAM_MODE_PREVIEW_STAGE_2;
            }
            break;
        case CAM_MODE_PREVIEW_STAGE_2:
#if defined(_CAM_SPECIAL_GC6113_)

#else
            if (cause.fend)
            {
                hal_CameraIrqSetMask(mask);
                gIsCameraStartXfer = FALSE;
                camerap_Delay(1);
                if (hal_CameraStopXfer(FALSE) == XFER_SUCCESS)
                {
                    gSensorInfo.camirqhandler(0, gSensorInfo.currentbuf);
                }
                else
                {
                    gSensorInfo.camirqhandler(1, gSensorInfo.currentbuf);
                }

                if (--s_prevCount)
                {
                    hal_CameraStartXfer((gSensorInfo.npixels<<1), (UINT8*)(gSensorInfo.vidbuffer));
                    gIsCameraStartXfer = TRUE;
                    mask.fend = 1;
                    hal_CameraIrqSetMask(mask);
                }
                else
                {
                    gSensorInfo.cammode = CAM_MODE_IDLE;
                }
            }
#endif
            break;

        case CAM_MODE_CAPTURE_STAGE_1:
#if defined(_CAM_SPECIAL_GC6113_)
            if (cause.fstart)
#else
            if (cause.fend)
#endif
            {
                hal_CameraIrqSetMask(mask);
#if defined(_CAM_SPECIAL_GC6113_)
#else
                hal_CameraStartXfer((gSensorInfo.npixels<<1), (UINT8*)(gSensorInfo.vidbuffer));
#endif
                mask.fend = 1;
                gIsCameraStartXfer = TRUE;
                hal_CameraIrqSetMask(mask);
                gSensorInfo.cammode = CAM_MODE_CAPTURE_STAGE_2;
            }
            break;
        case CAM_MODE_CAPTURE_STAGE_2:
#if defined(_CAM_SPECIAL_GC6113_)

#else
            if (cause.fend)
            {
                gIsCameraStartXfer = FALSE;
                if (hal_CameraStopXfer(FALSE) == XFER_SUCCESS)
                {
                    hal_CameraIrqSetMask(mask);
                    gSensorInfo.cammode = CAM_MODE_IDLE;
                    gSensorInfo.camirqhandler(0, gSensorInfo.currentbuf);
                }
                else
                {
                    mask.fend = 1;
                    hal_CameraIrqSetMask(mask);
                    gSensorInfo.cammode = CAM_MODE_CAPTURE_STAGE_1;
                }
            }
#endif
            break;

        default:
            gSensorInfo.camirqhandler(255, 255);
            break;
    }
}
PRIVATE BOOL camerap_CheckProductId()
{
    UINT8 cameraID = 0;
    hal_HstSendEvent(0x123456d1);
    HAL_CAMERA_CFG_T CamConfig = {0,};
    CamConfig.rstActiveH = FALSE;
    CamConfig.pdnActiveH = TRUE;
    CamConfig.dropFrame = FALSE;
    CamConfig.camClkDiv = 10; // 156MHz/10 = 15.6MHz
    CamConfig.endianess = NO_SWAP;//BYTE_SWAP
    CamConfig.colRatio= COL_RATIO_1_1;
    CamConfig.rowRatio= ROW_RATIO_1_1;
    CamConfig.camId = camera_GetCameraID();

    pmd_EnablePower(PMD_POWER_CAMERA,TRUE);

    hal_CameraOpen(&CamConfig);
    hal_CameraReset(TRUE);
    hal_CameraPowerDown(FALSE);

#ifdef I2C_BASED_ON_GPIO
    gc6113_gpio_i2c_init_sensor();
#else
    g_camdI2cBusId=HAL_I2C_BUS_ID_2;// = tgt_GetCamdConfig()->i2cBusId;
    hal_I2cOpen(g_camdI2cBusId);
#endif

    if (!gSensorInfo.snrrst)
        hal_CameraReset(FALSE);
    {
        SXS_TRACE(TSTDOUT,"gc6113: start to get the cam id, i2c id %d",g_camdI2cBusId);
        camerap_Delay(20);
        cameraID = camerap_GetId();
        SXS_TRACE(TSTDOUT,"PAS6175_3a01: got cam id 0x%x", cameraID);

        hal_HstSendEvent(0xcccc5600); hal_HstSendEvent(cameraID);

    }
    hal_CameraClose();
    pmd_EnablePower(PMD_POWER_CAMERA, FALSE);

    SXS_TRACE(TSTDOUT,"PAS6175_3a01(0x%02x): read id is 0x%02x", CAM_BY3A01_ID, cameraID);
    hal_HstSendEvent(0xaac256dd); hal_HstSendEvent(cameraID);



    if(cameraID == CAM_BY3A01_ID)
    {
        hal_HstSendEvent(0xaacc0000|CAM_BY3A01_ID); hal_HstSendEvent(0x123456ee);
        return TRUE;
    }
    else
    {
        return FALSE;
    }







}

VOID camerap_GetImageInfo(UINT32 * pcamout,UINT16 *pwidth,UINT16 *pheight)
{
#if  !defined(_CAM_SPECIAL_GC6113_)
    *pcamout = CAM_NPIX_QVGA;
    *pwidth = 240;
    *pheight = 320;
#else
    *pcamout = CAM_NPIX_QQVGA;
    *pwidth =128;// 128;
    *pheight = 160;//160


#endif
}

PRIVATE char* camerap_GetStringId(VOID)
{
    static char sp3a01_id_str[40] = "bf3a01_C399_AOXUN_20140723\n";
    return sp3a01_id_str;
}

PUBLIC BOOL camera_bf3a01serial_RegInit(CAM_DRV_T  * pCamDrv)
{
    if(camerap_CheckProductId())
    {
        pCamDrv->camera_PowerOn=camerap_PowerOn;
        pCamDrv->camera_PowerOff=camerap_PowerOff;
        pCamDrv->camera_Reserve=camerap_Reserve;
        pCamDrv->camera_Release=camerap_Release;
        pCamDrv->camera_PrepareImageCaptureL=camerap_PrepareImageCaptureL;
        pCamDrv->camera_CaptureImage=camerap_CaptureImage;
        pCamDrv->camera_SetZoomFactorL=camerap_SetZoomFactorL;
        pCamDrv->camera_ZoomFactor=camerap_ZoomFactor;
        pCamDrv->camera_SetDigitalZoomFactorL=camerap_SetDigitalZoomFactorL;
        pCamDrv->camera_DigitalZoomFactor=camerap_DigitalZoomFactor;
        pCamDrv->camera_SetContrastL=camerap_SetContrastL;
        pCamDrv->camera_Contrast=camerap_Contrast;
        pCamDrv->camera_SetBrightnessL=camerap_SetBrightnessL;
        pCamDrv->camera_Brightness=camerap_Brightness;
        pCamDrv->camera_SetFlashL=camerap_SetFlashL;
        pCamDrv->camera_Flash=camerap_Flash;
        pCamDrv->camera_SetExposureL=camerap_SetExposureL;
        pCamDrv->camera_Exposure=camerap_Exposure;
        pCamDrv->camera_SetWhiteBalanceL=camerap_SetWhiteBalanceL;
        pCamDrv->camera_WhiteBalance=camerap_WhiteBalance;
        pCamDrv->camera_SetSpecialEffect=camerap_SetSpecialEffect;
        pCamDrv->camera_SpecialEffect=camerap_SpecialEffect;
        pCamDrv->camera_SetNightMode=camerap_SetNightMode;
        pCamDrv->camera_NightMode=camerap_NightMode;
        pCamDrv->camera_StartViewFinderDirectL=camerap_StartViewFinderDirectL;
        pCamDrv->camera_StopViewFinder=camerap_StopViewFinder;
        pCamDrv->camera_ViewFinderActive=camerap_ViewFinderActive;
        pCamDrv->camera_SetViewFinderMirror=camerap_SetViewFinderMirror;
        pCamDrv->camera_ViewFinderMirror=camerap_ViewFinderMirror;
        pCamDrv->camera_CancelCaptureImage=camerap_CancelCaptureImage;
        pCamDrv->camera_EnumerateCaptureSizes=camerap_EnumerateCaptureSizes;
        pCamDrv->camera_PrepareVideoCaptureL=camerap_PrepareVideoCaptureL;
        pCamDrv->camera_StartVideoCapture=camerap_StartVideoCapture;
        pCamDrv->camera_StopVideoCapture=camerap_StopVideoCapture;
        pCamDrv->camera_VideoCaptureActive=camerap_VideoCaptureActive;
        pCamDrv->camera_EnumerateVideoFrameSizes=camerap_EnumerateVideoFrameSizes;
        pCamDrv->camera_EnumerateVideoFrameRates=camerap_EnumerateVideoFrameRates;
        pCamDrv->camera_GetFrameSize=camerap_GetFrameSize;
        pCamDrv->camera_SetFrameRate=camerap_SetFrameRate;
        pCamDrv->camera_FrameRate=camerap_FrameRate;
        pCamDrv->camera_BuffersInUse=camerap_BuffersInUse;
        pCamDrv->camera_FramesPerBuffer=camerap_FramesPerBuffer;
        pCamDrv->camera_GetId=camerap_GetId;
        pCamDrv->camera_GetImageInfo = camerap_GetImageInfo;
        pCamDrv->camera_GetStringId=camerap_GetStringId;
        return TRUE;
    }

    return FALSE;
}

PRIVATE void camerap_SetZoomFactorL(UINT32 ZoomFactor)
{
}

PRIVATE UINT32 camerap_ZoomFactor(void)
{
    return(0);
}

PRIVATE void camerap_SetDigitalZoomFactorL(UINT32 DigZoomFactor)
{
}

PRIVATE UINT32 camerap_DigitalZoomFactor(void)
{
    return(0);
}

PRIVATE UINT32 camerap_Contrast(void)
{
    return(0);
}

PRIVATE UINT32 camerap_Brightness(void)
{
    return(0);
}

PRIVATE CAM_FLASH_T camerap_Flash(void)
{
    return(CAM_FLASH_NONE);
}

PRIVATE CAM_EXPOSURE_T camerap_Exposure(void)
{
    return(CAM_EXPOSURE_AUTO);
}

PRIVATE CAM_WHITEBALANCE_T camerap_WhiteBalance(void)
{
    return(CAM_WB_AUTO);
}

PRIVATE CAM_SPECIALEFFECT_T camerap_SpecialEffect(VOID)
{
    return(CAM_SPE_NORMAL);
}

PRIVATE VOID camerap_SetNightMode(UINT32 NightMode)
{

    return;
    if (NightMode)
    {
        PAS6175_3a01_NightMode(TRUE);
    }
    else
    {
        PAS6175_3a01_NightMode(FALSE);
    }

}

PRIVATE UINT32 camerap_NightMode(VOID)
{
    return(FALSE);

}

PRIVATE void camerap_StartViewFinderDirectL()
{
    // Not really direct, but sets the camera into viewfinder mode (lower resolution to fit the screen)
    // Needs
}

PRIVATE void camerap_StopViewFinder(void)
{
}

PRIVATE BOOL camerap_ViewFinderActive(void)
{
    return(FALSE);
}

PRIVATE void camerap_SetViewFinderMirror(BOOL Mirror)
{
}

PRIVATE BOOL camerap_ViewFinderMirror(void)
{
    return(FALSE);
}

PRIVATE void camerap_CancelCaptureImage(void)
{
}

PRIVATE void camerap_EnumerateCaptureSizes(CAM_SIZE_T *Size,UINT32 SizeIndex, CAM_FORMAT_T Format)
{
}

PRIVATE void camerap_PrepareVideoCaptureL(CAM_FORMAT_T Format, UINT32 SizeIndex, UINT32 RateIndex, UINT32 BuffersToUse, UINT32 FramesPerBuffer)
{
}

PRIVATE void camerap_StartVideoCapture(void)
{
}

PRIVATE void camerap_StopVideoCapture(void)
{
    if (gSensorInfo.capturevideo)
    {
    }
}

PRIVATE BOOL camerap_VideoCaptureActive(void)
{
    return(gSensorInfo.capturevideo);
}

PRIVATE void camerap_EnumerateVideoFrameSizes(
    CAM_SIZE_T Size,
    UINT32 SizeIndex,
    CAM_FORMAT_T Format)
{
}

PRIVATE void camerap_EnumerateVideoFrameRates()
{
}

PRIVATE void camerap_GetFrameSize(CAM_SIZE_T Size)
{
}

PRIVATE VOID camerap_SetFrameRate(UINT32 FrameRate)
{
}

PRIVATE UINT32 camerap_FrameRate(void)
{
    return(0);
}

PRIVATE UINT32 camerap_BuffersInUse(void)
{
    return(0);
}

PRIVATE UINT32 camerap_FramesPerBuffer(void)
{
    return(0);
}


