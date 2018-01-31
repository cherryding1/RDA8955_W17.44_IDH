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

PRIVATE HAL_I2C_BUS_ID_T g_camdI2cBusId = HAL_I2C_BUS_ID_2;
#define CONT_CAP_CNT 2
#define CAM_GC032a_ID 0x2a

static UINT32 s_prevCount = 0;
PRIVATE HAL_CAMERA_SPI_MODE_E s_gc032a_mode = SPI_MODE_MASTER2_2; //jaczen SPI_MODE_MASTER2_2

PRIVATE BOOL s_camOpened = FALSE;
PRIVATE BOOL gIsCameraStartXfer = FALSE;

extern BOOL isVideoRecMode;

#define CAM_SPI_MCLK 10

PRIVATE VOID camerap_WriteOneReg(UINT8 Addr, UINT8 Data)
{

#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_write_data(gSensorInfo.i2caddress, &Addr, 1, &Data, 1);
#else
    hal_I2cSendRawByte(g_camdI2cBusId, ((gSensorInfo.i2caddress << 1) & 0xfe), PHASE3_WR_PH1);
    hal_I2cSendRawByte(g_camdI2cBusId, Addr, PHASE3_WR_PH2);
    hal_I2cSendRawByte(g_camdI2cBusId, Data, PHASE3_WR_PH3);
#endif
}

PRIVATE VOID camerap_Delay(UINT16 Wait_mS)
{
    UINT32 Ticks_16K;

    // Here is the delay function used by the camera driver.  It can be changed to use a non-blocking
    // wait if necessary, but for now, just use hal_TimDelay
    Ticks_16K = (16348 * Wait_mS) / 1000;
    hal_TimDelay(Ticks_16K);
}

PRIVATE VOID camerap_SendRgSeq(const CAM_REG_T *RegList, UINT16 Len)
{
    UINT16 RegCount;

    for (RegCount = 0; RegCount < Len; ++RegCount)
    {
        camerap_WriteReg(RegList[RegCount]);
    }
}

PRIVATE VOID camerap_WriteReg(CAM_REG_T RegWrite)
{
#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_write_data(gSensorInfo.i2caddress, (unsigned char *)&RegWrite.Addr, 1, &RegWrite.Data, 1);
#else
    hal_I2cSendRawByte(g_camdI2cBusId, ((gSensorInfo.i2caddress << 1) & 0xfe), PHASE3_WR_PH1);
    hal_I2cSendRawByte(g_camdI2cBusId, RegWrite.Addr, PHASE3_WR_PH2);
    hal_I2cSendRawByte(g_camdI2cBusId, RegWrite.Data, PHASE3_WR_PH3);
#endif

    // Do delay here using RegWrite.Wait mS
    camerap_Delay(RegWrite.Wait);
}

PRIVATE UINT8 camerap_ReadReg(UINT8 Addr)
{
#ifdef I2C_BASED_ON_GPIO
    char data[1];
    gpio_i2c_write_data(gSensorInfo.i2caddress, &Addr, 1, (unsigned char *)&data, 0);
    return (gpio_i2c_read_data(gSensorInfo.i2caddress, &Addr, 0, data, 1));
#else
    if (hal_I2cSendRawByte(g_camdI2cBusId, ((gSensorInfo.i2caddress << 1) & 0xfe), PHASE2_WR_PH1) == HAL_ERR_RESOURCE_BUSY)
    {
        return (0x1f);
    }
    if (hal_I2cSendRawByte(g_camdI2cBusId, Addr, PHASE2_WR_PH2) == HAL_ERR_RESOURCE_BUSY)
    {
        return (0x1e);
    }

    if (hal_I2cSendRawByte(g_camdI2cBusId, ((gSensorInfo.i2caddress << 1) | 0x01), PHASE2_RD_PH1) == HAL_ERR_RESOURCE_BUSY)

    {
        return (0x1d);
    }

    return (hal_I2cReadRawByte(g_camdI2cBusId, PHASE2_RD_PH2));
#endif
}

UINT8 clkP_delay = 0;

// for Bug31322, which should be removed after the bug is fixed.
U8 s_contLossDbgCnt = 5;
PRIVATE void camerap_PowerOn(CAM_SIZE_T CamOutSize, UINT16 IspOutWidth, UINT16 IspOutHeight, CAM_FORMAT_T Format)
{
    UINT32 tmp;

    HAL_CAMERA_CFG_T CamConfig = {
        0,
    };
    HAL_CAMERA_IRQ_CAUSE_T mask = {0, 0, 0, 0};
    UINT16 camOutWidth = 0, camOutHeight = 0;

    SXS_TRACE(TSTDOUT, "DRV_CAM : Get in camerap_PowerOn    >2118");

    hal_HstSendEvent(0x4444cce6);
    SXS_TRACE(TSTDOUT, "DRV_CAM :1 Power On, camOutSize %d ispw %d isph %d", CamOutSize, IspOutWidth, IspOutHeight);

    CamConfig.rstActiveH = FALSE;
    CamConfig.pdnActiveH = TRUE;
    CamConfig.dropFrame = FALSE;

    CamConfig.camClkDiv = CAM_SPI_MCLK; // must synchronize with VIDREC_AVI_VIDEO_FRAME_RATE
    CamConfig.endianess = NO_SWAP;
    CamConfig.camId = camera_GetCameraID();
    CamConfig.cropEnable = FALSE;
#ifdef _CAM_USE_DCT_
    CamConfig.camDctEnable = TRUE;
#endif
    CamConfig.colRatio = COL_RATIO_1_1;
    CamConfig.rowRatio = ROW_RATIO_1_1;
    CamConfig.vsync_inv = TRUE;
    CamConfig.spi_pixels_per_line = IspOutWidth;
    CamConfig.spi_pixels_per_column = IspOutHeight;
    //    CamConfig.spi_yuv_out = SPI_OUT_V0_Y1_U0_Y0;
    CamConfig.spi_yuv_out = SPI_OUT_Y0_U0_Y1_V0;

    CamConfig.spi_mode = s_gc032a_mode; //SPI_MODE_MASTER2_2;// s_gc6113_mode;
    CamConfig.spi_little_endian_en = FALSE;
    CamConfig.spi_ctrl_clk_div = 1;

    pmd_EnablePower(PMD_POWER_CAMERA, TRUE);
    //camerap_Delay(100);

    hal_CameraOpen(&CamConfig);
    hal_CameraIrqSetMask(mask);
    hal_CameraPowerDown(FALSE);

#ifdef I2C_BASED_ON_GPIO
    gc6113_gpio_i2c_init_sensor();
#else
    g_camdI2cBusId = tgt_GetCamdConfig()->i2cBusId;
    hal_I2cOpen(g_camdI2cBusId);
#endif

    if (!gSensorInfo.snrrst)
        hal_CameraReset(FALSE);

    //  camerap_SendRgSeq(RG_Init_PowerOn,  (sizeof(RG_Init_PowerOn) / sizeof(CAM_REG_T)));

    s_prevCount = 0;

    s_camOpened = TRUE;

    if (CamOutSize == CAM_NPIX_VGA)
    {
        SXS_TRACE(TSTDOUT, "DRV_CAM : CAM_NPIX_VGA");
        hal_HstSendEvent(0x6666cccf);
        //camerap_SendRgSeq(RG_InitPowerOnQvgainit,  (sizeof(RG_InitPowerOnQvgainit) / sizeof(CAM_REG_T)));
        camerap_SendRgSeq(RG_InitPowerOnVga, (sizeof(RG_InitPowerOnVga) / sizeof(CAM_REG_T)));
    }

    if (CamOutSize == CAM_NPIX_QVGA)
    {
        SXS_TRACE(TSTDOUT, "DRV_CAM : CAM_NPIX_QVGA");
        hal_HstSendEvent(0x9999ccee);
        //camerap_SendRgSeq(RG_InitPowerOnQvgainit,  (sizeof(RG_InitPowerOnQvgainit) / sizeof(CAM_REG_T)));
        camerap_SendRgSeq(RG_Init_QVGA320240, (sizeof(RG_Init_QVGA320240) / sizeof(CAM_REG_T)));
        //   camerap_WriteOneReg(0xfe,0x01);
        //        hal_HstSendEvent(0xeeff0000|camerap_ReadReg(0x51));
        //camerap_WriteOneReg(0xfe,0x01);
        //tmp =    camerap_ReadReg(0x5d);
        //hal_HstSendEvent(0xeeff0000|tmp);
    }

    switch (s_gc032a_mode)
    {
    case SPI_MODE_MASTER2_1:
        SXS_TRACE(TSTDOUT, "DRV_CAM : spi master 2_1 mode in use");

        hal_HstSendEvent(0x2017720e);
        camerap_SendRgSeq(RG_Reinit_MASTER2_1, (sizeof(RG_Reinit_MASTER2_1) / sizeof(CAM_REG_T)));
        break;
    case SPI_MODE_MASTER2_2:
        hal_HstSendEvent(0x2017720f);
        SXS_TRACE(TSTDOUT, "DRV_CAM : spi master 2_2 mode in use");
        camerap_SendRgSeq(RG_Reinit_MASTER2_2, (sizeof(RG_Reinit_MASTER2_2) / sizeof(CAM_REG_T)));
        break;
    case SPI_MODE_NO:
        SXS_TRACE(TSTDOUT, "no spi master in use");
        break;
    case SPI_MODE_MASTER1: // i bit mode with ssn
        SXS_TRACE(TSTDOUT, "master 1bit with ssn in use");
        break;
    default:
        SXS_TRACE(TSTDOUT, "unsupported spi mode");
        break;
    }

    s_contLossDbgCnt = 5;
    camerap_Delay(100);
    SXS_TRACE(TSTDOUT, "DRV_CAM : st mark");
    SXS_TRACE(TSTDOUT, "DRV_CAM : 0xf3=%x", camerap_ReadReg(0xf3));
    SXS_TRACE(TSTDOUT, "DRV_CAM : 0xf5=%x", camerap_ReadReg(0xf5));
    SXS_TRACE(TSTDOUT, "DRV_CAM : 0xf7=%x", camerap_ReadReg(0xf7));
    SXS_TRACE(TSTDOUT, "DRV_CAM : 0xfa=%x", camerap_ReadReg(0xfa));

    SXS_TRACE(TSTDOUT, "DRV_CAM : 0x60=%x", camerap_ReadReg(0x60));
    SXS_TRACE(TSTDOUT, "DRV_CAM : 0x61=%x", camerap_ReadReg(0x61));
    SXS_TRACE(TSTDOUT, "DRV_CAM : 0x62=%x", camerap_ReadReg(0x62));
    SXS_TRACE(TSTDOUT, "DRV_CAM : 0x63=%x", camerap_ReadReg(0x63));
}

PRIVATE void camerap_PowerOff(void)
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0, 0, 0, 0};

    if (s_camOpened)
    {
        hal_CameraIrqSetMask(mask);
        // disable IFC
        if (gIsCameraStartXfer == TRUE)
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
        camerap_Delay(500);

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
        HAL_CAMERA_IRQ_CAUSE_T mask = {0, 0, 0, 0};

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
            aadasd camerap_SendRgSeq(RG_Reinit_QQVGA, (sizeof(RG_Reinit_QQVGA) / sizeof(CAM_REG_T)));
            hal_CameraSetupClockDivider(6);
            s_camOutMode = CAM_OUT_QQVGA;
        }
        mask.fstart = 1;
#else
        s_prevCount = CONT_CAP_CNT;
        mask.fend = 1;
#endif // _CAM_SPECIAL_GC6113_

        hal_CameraIrqSetMask(mask);
        hal_CameraControllerEnable(TRUE);
    }
#if !defined(_CAM_SPECIAL_GC6113_)
    else
    {
        UINT32 status = hal_SysEnterCriticalSection();
        gSensorInfo.vidbuffer = Buffer;
        if (s_prevCount < CONT_CAP_CNT)
            s_prevCount++;
        hal_SysExitCriticalSection(status);
    }
#endif
}

PRIVATE void camerap_CaptureImage()
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0, 0, 0, 0};
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
        uint16 tmp_val_1 = 0, tmp_val_2 = 0;
        uint32 dest_val = 0;

        camerap_WriteOneReg(0xfe, 0x00);
        camerap_WriteOneReg(0xe5, 0x00);
        tmp_val_1 = camerap_ReadReg(0x03);
        tmp_val_2 = camerap_ReadReg(0x04);

        dest_val = ((tmp_val_1 << 8) | (tmp_val_2)) * 104 / 260;
        camerap_WriteOneReg(0x03, (dest_val & 0xff00) >> 8);
        camerap_WriteOneReg(0x04, dest_val & 0x00ff);
        camerap_Delay(120);
        hal_CameraSetupClockDivider(15);
        sdfs camerap_SendRgSeq(RG_Reinit_QVGA, (sizeof(RG_Reinit_QVGA) / sizeof(CAM_REG_T)));
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

//byd3901 id

//0xfc=0x39,0xfd=0x01
PRIVATE UINT8 camerap_GetId(VOID)
{
    hal_TimDelay(163); //add by Michael
                       //return((camerap_ReadReg(0xf0)<<8)|camerap_ReadReg(0xf1));
    SXS_TRACE(TSTDOUT, "camerap_GetId is 0x%x", ((camerap_ReadReg(0xf0) << 8) | camerap_ReadReg(0xf1)));
    return camerap_ReadReg(0xf1);
}

// ask PAS FAE to get the regster list.

PRIVATE void camerap_SetContrastL(UINT32 Contrast)
{

    switch (Contrast)
    {
    case 1:
        camerap_WriteOneReg(0xd3, 0x44);
        break;
    case 2:
        camerap_WriteOneReg(0xd3, 0x48);
        break;
    case 3:
        camerap_WriteOneReg(0xd3, 0x4b);
        break;
    case 4:
        camerap_WriteOneReg(0xd3, 0x50);
        break;
    case 5:
        camerap_WriteOneReg(0xd3, 0x54);
        break;
    default:
        break;
    }
}

// ask PAS FAE to get the regster list.
PRIVATE void camerap_SetBrightnessL(UINT32 Brightness)
{

    switch (Brightness)
    {
    case 1: //0
        camerap_WriteOneReg(0xd5, 0x00);

        break;
    case 2: //-2
        camerap_WriteOneReg(0xd5, 0xd0);

        break;
    case 3: //-1
        camerap_WriteOneReg(0xd5, 0xe0);

        break;
    case 4: //+1
        camerap_WriteOneReg(0xd5, 0x20);

        break;
    case 5: //+2
        camerap_WriteOneReg(0xd5, 0x30);

        break;
    default:
        break;
    }
}
extern UINT32 gCameraID;
PRIVATE void camerap_SetFlashPower(BOOL flash_on)
{
#ifdef __CAMERA_FEATURE_FLASH__
    if (tgt_GetCamdConfig()->camera_flash == PMD_LEVEL_ID_QTY)
    {
        pmd_EnablePower(PMD_POWER_CAMERA_FLASH, flash_on);
    }
    else
    {
        PMD_LEVEL_ID_T flash_id = tgt_GetCamdConfig()->camera_flash;
        if ((flash_id > PMD_LEVEL_LED0) && (flash_id < PMD_LEVEL_VIBRATOR))
        {
            pmd_SetLevel(flash_id, flash_on);
        }
    }
#endif
}

PRIVATE void camerap_SetFlashL(CAM_FLASH_T Flash)
{
    if (gCameraID == 1)
        return; //Only light on flash led when the back sensor is used.

    switch (Flash)
    {
    case CAM_FLASH_NONE:
        camerap_SetFlashPower(FALSE);
        break;

    case CAM_FLASH_FORCED:
        camerap_SetFlashPower(TRUE);
        sxr_Sleep(700 MS_WAITING);
        break;
    case CAM_FLASH_AUTO:
    case CAM_FLASH_FILLIN:
    case CAM_FLASH_REDEYEREDUCE:
    default:
        break;
    }
    return;
}
// ask PAS FAE to get the regster list.

PRIVATE void camerap_SetExposureL(CAM_EXPOSURE_T Exposure)
{
    return;

    switch (Exposure)
    {
    case CAM_EXPOSURE_AUTO: //0
        camerap_WriteOneReg(0xfe, 0x01);
        camerap_WriteOneReg(0x13, 0x60);
        camerap_WriteOneReg(0xfe, 0x00);

        break;
    case CAM_EXPOSURE_NEGATIVE_2: //-2
        camerap_WriteOneReg(0xfe, 0x01);
        camerap_WriteOneReg(0x13, 0x40);
        camerap_WriteOneReg(0xfe, 0x00);

        break;
    case CAM_EXPOSURE_NEGATIVE_1: //-1
        camerap_WriteOneReg(0xfe, 0x01);
        camerap_WriteOneReg(0x13, 0x50);
        camerap_WriteOneReg(0xfe, 0x00);
        break;
    case CAM_EXPOSURE_POSITIVE_1: //+1
        camerap_WriteOneReg(0xfe, 0x01);
        camerap_WriteOneReg(0x13, 0x70);
        camerap_WriteOneReg(0xfe, 0x00);
        break;
    case CAM_EXPOSURE_POSITIVE_2: //+2
        camerap_WriteOneReg(0xfe, 0x01);
        camerap_WriteOneReg(0x13, 0x80);
        camerap_WriteOneReg(0xfe, 0x00);

        break;

    default:
        break;
    }
}

PRIVATE void camerap_SetWhiteBalanceL(CAM_WHITEBALANCE_T WhiteBalance)
{
    UINT8 TempReg;

    //TempReg = camerap_ReadReg(0x22);
    switch (WhiteBalance)
    {
    case CAM_WB_AUTO: /*Auto */
        camerap_WriteOneReg(0x42, 0xcf);
        break;
    case CAM_WB_DAYLIGHT: /*晴天太阳光*/
        camerap_WriteOneReg(0x42, 0xcd);
        // camerap_WriteOneReg(0x49,0x50);  //  50 45 40
        // camerap_WriteOneReg(0x4a,0x45);
        // camerap_WriteOneReg(0x4b,0x40);

        camerap_WriteOneReg(0x49, 0x7b);
        camerap_WriteOneReg(0x4a, 0x40);
        camerap_WriteOneReg(0x4b, 0x40);
        break;
    case CAM_WB_OFFICE: /*日光灯*/
        camerap_WriteOneReg(0x42, 0xcd);
        camerap_WriteOneReg(0x49, 0x40);
        camerap_WriteOneReg(0x4a, 0x42);
        camerap_WriteOneReg(0x4b, 0x50);
        break;
    case CAM_WB_CLOUDY: /*阴天*/
        camerap_WriteOneReg(0x42, 0xcd);
        // camerap_WriteOneReg(0x49,0x5a); //WB_manual_gain // 5a 42 40
        //  camerap_WriteOneReg(0x4a,0x42);
        //  camerap_WriteOneReg(0x4b,0x40);

        camerap_WriteOneReg(0x49, 0xb0);
        camerap_WriteOneReg(0x4a, 0x40);
        camerap_WriteOneReg(0x4b, 0x40);
        break;
    case CAM_WB_TUNGSTEN: /*钨丝灯*/
        camerap_WriteOneReg(0x42, 0xcd);
        //  camerap_WriteOneReg(0x49,0x48);
        // camerap_WriteOneReg(0x4a,0x40);
        //  camerap_WriteOneReg(0x4b,0x5c);

        camerap_WriteOneReg(0x49, 0xcd);
        camerap_WriteOneReg(0x4a, 0x4e);
        camerap_WriteOneReg(0x4b, 0xa8);
        break;
    case CAM_WB_FLUORESCENT:
        camerap_WriteOneReg(0x42, 0xcd);
        camerap_WriteOneReg(0x49, 0x40);
        camerap_WriteOneReg(0x4a, 0x42);
        camerap_WriteOneReg(0x4b, 0x80);
        break;
    default:
        break;
    }
}

PRIVATE VOID camerap_SetSpecialEffect(CAM_SPECIALEFFECT_T SpecialEffect)
{
    switch (SpecialEffect)
    {
    case CAM_SPE_NORMAL:
        camerap_WriteOneReg(0x43, 0x10);
        camerap_WriteOneReg(0x8a, 0x00);
        camerap_WriteOneReg(0x8b, 0x00);
        break;

    case CAM_SPE_ANTIQUE:
        camerap_WriteOneReg(0x43, 0x02);
        camerap_WriteOneReg(0x8a, 0xd0);
        camerap_WriteOneReg(0x8b, 0x28);
        break;

    case CAM_SPE_REDISH:
        camerap_WriteOneReg(0x43, 0x02);
        camerap_WriteOneReg(0x8a, 0x10);
        camerap_WriteOneReg(0x8b, 0x50); //28 terry
        break;

    case CAM_SPE_GREENISH:
        camerap_WriteOneReg(0x43, 0x02);
        camerap_WriteOneReg(0x8a, 0xe0);
        camerap_WriteOneReg(0x8b, 0xe0);
        break;

    case CAM_SPE_BLUEISH:
        camerap_WriteOneReg(0x43, 0x02);
        camerap_WriteOneReg(0x8a, 0x50);
        camerap_WriteOneReg(0x8b, 0xe0);
        break;

    case CAM_SPE_BLACKWHITE:
        camerap_WriteOneReg(0x43, 0x02);
        camerap_WriteOneReg(0x8a, 0x00);
        camerap_WriteOneReg(0x8b, 0x00);
        break;

    case CAM_SPE_NEGATIVE:
        camerap_WriteOneReg(0x43, 0x01);
        camerap_WriteOneReg(0x8a, 0x00);
        camerap_WriteOneReg(0x8b, 0x00);
        break;

    case CAM_SPE_BLACKWHITE_NEGATIVE:
        camerap_WriteOneReg(0x43, 0x03);
        camerap_WriteOneReg(0x8a, 0x00);
        camerap_WriteOneReg(0x8b, 0x00);
        break;

    default:
        break;
    }
}

typedef enum {

#ifdef CHIP_HAS_AP

    /// Sending data to the sim1
    HAL_IFC_SCI_TX,
    /// Receiving data from the sim1
    HAL_IFC_SCI_RX,
    /// Sending data to the sim2
    HAL_IFC_SCI2_TX,
    /// Receiving data from the sim2
    HAL_IFC_SCI2_RX,
    /// Sending data to the sim3
    HAL_IFC_SCI3_TX,
    /// Receiving data from the sim3
    HAL_IFC_SCI3_RX,
    /// Sending data on the SPI1
    HAL_IFC_SPI_TX,
    /// Receiving data from the SPI1
    HAL_IFC_SPI_RX,
    /// Sending data on the SPI2
    HAL_IFC_SPI2_TX,
    /// Receiving data from the SPI2
    HAL_IFC_SPI2_RX,
#ifdef CHIP_HAS_I2C_IFC
    /// Sending data to the I2C
    HAL_IFC_I2C_TX,
    /// Receiving data from the I2C
    HAL_IFC_I2C_RX,
#endif
    /// Sending data on the debug uart
    HAL_IFC_DEBUG_UART_TX,
    /// Receiving data from the debug uart
    HAL_IFC_DEBUG_UART_RX,

#else // !CHIP_HAS_AP

    /// Sending data to the sim
    HAL_IFC_SCI_TX,
    /// Receiving data from the sim
    HAL_IFC_SCI_RX,
    /// Sending data on the SPI
    HAL_IFC_SPI_TX,
    /// Receiving data from the SPI
    HAL_IFC_SPI_RX,
    /// Sending data on the SPI
    HAL_IFC_SPI2_TX,
    /// Receiving data from the SPI
    HAL_IFC_SPI2_RX,
    /// Sending data on the SPI
    HAL_IFC_SPI3_TX,
    /// Receiving data from the SPI
    HAL_IFC_SPI3_RX,
    /// Sending data on the debug uart
    HAL_IFC_DEBUG_UART_TX,
    /// Receiving data from the debug uart
    HAL_IFC_DEBUG_UART_RX,
    /// Sending data on the uart 1
    HAL_IFC_UART1_TX,
    /// Receiving data from uart 1
    HAL_IFC_UART1_RX,
    /// Sending data on the uart 2
    HAL_IFC_UART2_TX,
    /// Receiving data from uart 2
    HAL_IFC_UART2_RX,
    /// Sending data on the SDMMC
    HAL_IFC_SDMMC_TX,
    /// Receiving data from the SDMMC
    HAL_IFC_SDMMC_RX,
    /// Sending data on the CAMERA: Doesn't exist
    HAL_IFC_CAMERA_TX_RESERVED,
    /// Receiving data from the CAMERA
    HAL_IFC_CAMERA_RX,
#if (CHIP_HAS_SDIO == 1)
    /// Sending data on the SDMMC //  HAL_IFC_SDMMC2_TX =54,
    HAL_IFC_SDMMC2_TX,
    /// Receiving data from the SDMMC
    HAL_IFC_SDMMC2_RX, // HAL_IFC_SDMMC2_RX =55,
#endif
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE) && \
    (CHIP_ASIC_ID != CHIP_ASIC_ID_8808) &&    \
    (CHIP_ASIC_ID != CHIP_ASIC_ID_8809) // 8809P or later feature phones
                       /// Sending data to the sim2
    HAL_IFC_SCI2_TX,
    /// Receiving data from the sim2
    HAL_IFC_SCI2_RX,
    /// Sending data to the sim3
    HAL_IFC_SCI3_TX,
    /// Receiving data from the sim3
    HAL_IFC_SCI3_RX,
#endif

#endif // !CHIP_HAS_AP

    /// No transaction requested
    HAL_IFC_NO_REQWEST
} HAL_IFC_REQUEST_ID_T;

extern UINT8 g_halCameraIfcChan;

#if 0 //from byd
PRIVATE VOID camerap_InteruptHandler(HAL_CAMERA_IRQ_CAUSE_T cause)
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0, };
	UINT32 tc1;
	UINT32 tc2;

	
    //hal_HstSendEvent(0x12345609);hal_HstSendEvent(cause);hal_HstSendEvent(gSensorInfo.cammode);
    hal_HstSendEvent(0x20177210);
    SXS_TRACE(TSTDOUT,"DRV_CAM : InteruptHandler mode=%d currentbuf=%x vidbuffer=%x",gSensorInfo.cammode,gSensorInfo.currentbuf,gSensorInfo.vidbuffer);
    switch (gSensorInfo.cammode)
    {
        case CAM_MODE_PREVIEW_STAGE_1:
			SXS_TRACE(TSTDOUT,"DRV_CAM : CAM_MODE_PREVIEW_STAGE_1");
#if defined(_CAM_SPECIAL_GC6113_)
            if (cause.fstart)
#else
            if (cause.fend)
#endif
            {
            
                //hal_HstSendEvent(0xdcdc0001);
                //hal_HstSendEvent(hal_TimGetUpTime());
                hal_CameraIrqSetMask(mask);

                hal_CameraStartXfer((gSensorInfo.npixels<<1), (UINT8*)(gSensorInfo.vidbuffer));
		
		 tc1 = hal_IfcGetTc(HAL_IFC_CAMERA_RX, g_halCameraIfcChan);
		hal_HstSendEvent(0x20177211);
		hal_HstSendEvent(tc1);


		//hal_DbgAssert(0);
                gIsCameraStartXfer = TRUE;
                //hal_HstSendEvent(hal_TimGetUpTime());

                mask.fend = 1;
                hal_CameraIrqSetMask(mask);

                gSensorInfo.cammode = CAM_MODE_PREVIEW_STAGE_2;
            }
            break;
        case CAM_MODE_PREVIEW_STAGE_2:
			SXS_TRACE(TSTDOUT,"DRV_CAM : CAM_MODE_PREVIEW_STAGE_2");
#if defined(_CAM_SPECIAL_GC6113_)

#else
            if (cause.fend)
            {
                //hal_HstSendEvent(0xdcdc0002);
                //hal_HstSendEvent(hal_TimGetUpTime());
               
				tc2 = hal_IfcGetTc(HAL_IFC_CAMERA_RX, g_halCameraIfcChan);
				hal_HstSendEvent(0x20177212);
				hal_HstSendEvent(tc2);

                hal_CameraIrqSetMask(mask);
                gIsCameraStartXfer = FALSE;
                camerap_Delay(1);
                if (hal_CameraStopXfer(FALSE) == XFER_SUCCESS)
                {
             
			 hal_HstSendEvent(0x20177213);
                    s_contLossDbgCnt = 5;
                    gSensorInfo.camirqhandler(0, gSensorInfo.currentbuf);
                }
                else
                {
              
			  hal_HstSendEvent(0x20177214);
                    //gSensorInfo.camirqhandler(1, gSensorInfo.currentbuf);
                    s_prevCount++;
                    s_contLossDbgCnt--;
                    if (!s_contLossDbgCnt) hal_DbgAssert("!!Assert for continuous camera image loss!!");
                }

                if (--s_prevCount)
                {
              
			  hal_HstSendEvent(0x20177215);
                    hal_CameraStartXfer((gSensorInfo.npixels<<1), (UINT8*)(gSensorInfo.vidbuffer));
                    gIsCameraStartXfer = TRUE;
                    mask.fend = 1;
                    hal_CameraIrqSetMask(mask);
		

                }
                else
                {
              
			  hal_HstSendEvent(0x20177216);
                    gSensorInfo.cammode = CAM_MODE_IDLE;
                }
                //hal_HstSendEvent(hal_TimGetUpTime());
            }
#endif
            break;

        case CAM_MODE_CAPTURE_STAGE_1:
			SXS_TRACE(TSTDOUT,"DRV_CAM : CAM_MODE_CAPTURE_STAGE_1");
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
				hal_HstSendEvent(0x20177203);

                mask.fend = 1;
                gIsCameraStartXfer = TRUE;
                hal_CameraIrqSetMask(mask);
                gSensorInfo.cammode = CAM_MODE_CAPTURE_STAGE_2;
            }
            break;
        case CAM_MODE_CAPTURE_STAGE_2:
			SXS_TRACE(TSTDOUT,"DRV_CAM : CAM_MODE_CAPTURE_STAGE_2");
#if defined(_CAM_SPECIAL_GC6113_)

#else
            if (cause.fend)
            {
            
			hal_HstSendEvent(0x20177204);
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
			SXS_TRACE(TSTDOUT,"DRV_CAM : default");
            gSensorInfo.camirqhandler(255, 255);
            break;
    }
}

#else // from gc6133
UINT32 cam_handle_count = 0;
PRIVATE VOID camerap_InteruptHandler(HAL_CAMERA_IRQ_CAUSE_T cause)
{
    SXS_TRACE(TSTDOUT, "DRV_CAM : Get in camerap_InteruptHandler");
    HAL_CAMERA_IRQ_CAUSE_T mask = {
        0,
    };

    switch (gSensorInfo.cammode)
    {
    case CAM_MODE_PREVIEW_STAGE_1:
#if defined(_CAM_SPECIAL_GC6133_)
        if (cause.fstart)
#else
        if (cause.fend)
#endif
        {
            hal_CameraIrqSetMask(mask);

            if (gIsCameraStartXfer == TRUE)
                hal_CameraStopXfer(FALSE);
            hal_CameraStartXfer((gSensorInfo.npixels << 1), (UINT8 *)(gSensorInfo.vidbuffer));
            gIsCameraStartXfer = TRUE;

            mask.fend = 1;
            hal_CameraIrqSetMask(mask);

            gSensorInfo.cammode = CAM_MODE_PREVIEW_STAGE_2;
        }
        break;
    case CAM_MODE_PREVIEW_STAGE_2:
#if defined(_CAM_SPECIAL_GC6133_)
        if (cause.fend)
        {
            hal_CameraIrqSetMask(mask);
            if (gIsCameraStartXfer == TRUE)
            {
                gIsCameraStartXfer = FALSE;
                gSensorInfo.cammode = CAM_MODE_IDLE;
                if (hal_CameraStopXfer(FALSE) == XFER_SUCCESS)
                {
                    gSensorInfo.camirqhandler(0, gSensorInfo.currentbuf);
                }
                else
                {
                    gSensorInfo.camirqhandler(1, gSensorInfo.currentbuf);
                }
            }
        }
#else
        if (cause.fend)
        {
            hal_CameraIrqSetMask(mask);
            if (gIsCameraStartXfer == TRUE)
            {
                gIsCameraStartXfer = FALSE;
                camerap_Delay(1);
                hal_HstSendEvent(cam_handle_count++);
                if (hal_CameraStopXfer(FALSE) == XFER_SUCCESS)
                {
                    gSensorInfo.camirqhandler(0, gSensorInfo.currentbuf);
                }
                else
                {
                    gSensorInfo.camirqhandler(1, gSensorInfo.currentbuf);
                }
            }

            if (--s_prevCount)
            {
                hal_CameraStartXfer((gSensorInfo.npixels << 1), (UINT8 *)(gSensorInfo.vidbuffer));
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
#if defined(_CAM_SPECIAL_GC6133_)
        if (cause.fstart)
#else
        if (cause.fend)
#endif
        {
            hal_CameraIrqSetMask(mask);
            if (gIsCameraStartXfer == TRUE)
                hal_CameraStopXfer(FALSE);
#if defined(_CAM_SPECIAL_GC6133_)
            hal_CameraStartXfer(gTransBufSize, gTransBufPtr[gTransBufIdx]);
            mask.dma = 1;
#else
            hal_CameraStartXfer((gSensorInfo.npixels << 1), (UINT8 *)(gSensorInfo.vidbuffer));
#endif
            mask.fend = 1;
            gIsCameraStartXfer = TRUE;
            hal_CameraIrqSetMask(mask);
            gSensorInfo.cammode = CAM_MODE_CAPTURE_STAGE_2;
        }
        break;
    case CAM_MODE_CAPTURE_STAGE_2:
#if defined(_CAM_SPECIAL_GC6133_)
        if (cause.dma)
        {
            UINT8 idx = gTransBufIdx;
            idx = (idx + 1) & 1;
            hal_CameraStartXfer(gTransBufSize, gTransBufPtr[idx]);
            gSensorInfo.camirqhandler(2, gTransBufIdx);
            gTransBufIdx = idx;

            mask.dma = 1;
            mask.fend = 1;
            hal_CameraIrqSetMask(mask);
        }
        if (cause.fend)
        {
            hal_CameraIrqSetMask(mask);

            hal_CameraStopXfer(FALSE);
            gIsCameraStartXfer = FALSE;
            gSensorInfo.cammode = CAM_MODE_IDLE;
            gSensorInfo.camirqhandler(3, 0);

            //return to Normal Frame Rate(Preview Mode)
            //	camerap_WriteOneReg(0xfe,0x00);
            //	 camerap_WriteOneReg(0xe5,0x01);
            //end
        }
#else
        if (cause.fend)
        {
            if (gIsCameraStartXfer == TRUE)
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
        }
#endif
        break;

    default:
        gSensorInfo.camirqhandler(255, 255);
        break;
    }
}

#endif

PRIVATE BOOL camerap_CheckProductId()
{
    UINT8 cameraID = 0;
    hal_HstSendEvent(0x123456d1);
    HAL_CAMERA_CFG_T CamConfig = {
        0,
    };
    CamConfig.rstActiveH = FALSE;
    CamConfig.pdnActiveH = TRUE;
    CamConfig.dropFrame = FALSE;
    CamConfig.camClkDiv = CAM_SPI_MCLK; // 156MHz/10 = 15.6MHz
    CamConfig.endianess = NO_SWAP;      //BYTE_SWAP
    CamConfig.colRatio = COL_RATIO_1_1;
    CamConfig.rowRatio = ROW_RATIO_1_1;
    CamConfig.camId = camera_GetCameraID();

    pmd_EnablePower(PMD_POWER_CAMERA, TRUE);
    //camerap_Delay(100);

    hal_CameraOpen(&CamConfig);
    hal_CameraReset(TRUE);
    hal_CameraPowerDown(FALSE);

#ifdef I2C_BASED_ON_GPIO
    gc6113_gpio_i2c_init_sensor();
    sdfsafsafaf

#else
    g_camdI2cBusId = tgt_GetCamdConfig()->i2cBusId; //HAL_I2C_BUS_ID_2;
    if (HAL_ERR_NO != hal_I2cOpen(g_camdI2cBusId))
    {

        if (HAL_ERR_NO != hal_I2cOpen(g_camdI2cBusId))
        {
            hal_HstSendEvent(0x123456dd);
            hal_HstSendEvent(hal_I2cOpen(g_camdI2cBusId));
            hal_CameraClose();
            pmd_EnablePower(PMD_POWER_CAMERA, FALSE);
            return FALSE;
        }
    }
#endif

        if (!gSensorInfo.snrrst)
            hal_CameraReset(FALSE);
    {
        SXS_TRACE(TSTDOUT, "DRV_CAM:gc032a: start to get the cam id, i2c id %d", g_camdI2cBusId);
        camerap_Delay(20);
        cameraID = camerap_GetId();
        SXS_TRACE(TSTDOUT, "DRV_CAM:gc032a: got cam id 0x%x", cameraID);

        hal_HstSendEvent(0xcccc5600);
        hal_HstSendEvent(cameraID);
    }

#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_close();
#else
    hal_I2cClose(g_camdI2cBusId);
#endif
    hal_CameraClose();
    pmd_EnablePower(PMD_POWER_CAMERA, FALSE);
    camerap_Delay(100);

    SXS_TRACE(TSTDOUT, "DRV_CAM :(0x%02x): read id is 0x%02x", CAM_GC032a_ID, cameraID);
    hal_HstSendEvent(0x10000001);
    hal_HstSendEvent(cameraID);

    if (cameraID == CAM_GC032a_ID)
    {
        hal_HstSendEvent(0x123456d9);
        hal_HstSendEvent(0xaacc0000 | CAM_GC032a_ID);
        hal_HstSendEvent(0x123456ee);
        return TRUE;
    }
    else
    {
        hal_HstSendEvent(0x123456d5);
        return FALSE;
    }
}

VOID camerap_GetImageInfo(UINT32 *pcamout, UINT16 *pwidth, UINT16 *pheight)
{
#ifdef __USE_VGA_DEF__
    *pcamout = CAM_NPIX_VGA;
    *pwidth = 640;
    *pheight = 480;
#else
    *pcamout = CAM_NPIX_QVGA;
    *pwidth = 320; //240;
    *pheight = 240; //320;
#endif
}

PUBLIC BOOL camera_gc032a_RegInit(CAM_DRV_T *pCamDrv)
{
    if (camerap_CheckProductId())
    {
        pCamDrv->camera_PowerOn = camerap_PowerOn;
        pCamDrv->camera_PowerOff = camerap_PowerOff;
        pCamDrv->camera_Reserve = camerap_Reserve;
        pCamDrv->camera_Release = camerap_Release;
        pCamDrv->camera_PrepareImageCaptureL = camerap_PrepareImageCaptureL;
        pCamDrv->camera_CaptureImage = camerap_CaptureImage;
        pCamDrv->camera_SetZoomFactorL = camerap_SetZoomFactorL;
        pCamDrv->camera_ZoomFactor = camerap_ZoomFactor;
        pCamDrv->camera_SetDigitalZoomFactorL = camerap_SetDigitalZoomFactorL;
        pCamDrv->camera_DigitalZoomFactor = camerap_DigitalZoomFactor;
        pCamDrv->camera_SetContrastL = camerap_SetContrastL;
        pCamDrv->camera_Contrast = camerap_Contrast;
        pCamDrv->camera_SetBrightnessL = camerap_SetBrightnessL;
        pCamDrv->camera_Brightness = camerap_Brightness;
        pCamDrv->camera_SetFlashL = camerap_SetFlashL;
        pCamDrv->camera_Flash = camerap_Flash;
        pCamDrv->camera_SetExposureL = camerap_SetExposureL;
        pCamDrv->camera_Exposure = camerap_Exposure;
        pCamDrv->camera_SetWhiteBalanceL = camerap_SetWhiteBalanceL;
        pCamDrv->camera_WhiteBalance = camerap_WhiteBalance;
        pCamDrv->camera_SetSpecialEffect = camerap_SetSpecialEffect;
        pCamDrv->camera_SpecialEffect = camerap_SpecialEffect;
        pCamDrv->camera_SetNightMode = camerap_SetNightMode;
        pCamDrv->camera_NightMode = camerap_NightMode;
        pCamDrv->camera_StartViewFinderDirectL = camerap_StartViewFinderDirectL;
        pCamDrv->camera_StopViewFinder = camerap_StopViewFinder;
        pCamDrv->camera_ViewFinderActive = camerap_ViewFinderActive;
        pCamDrv->camera_SetViewFinderMirror = camerap_SetViewFinderMirror;
        pCamDrv->camera_ViewFinderMirror = camerap_ViewFinderMirror;
        pCamDrv->camera_CancelCaptureImage = camerap_CancelCaptureImage;
        pCamDrv->camera_EnumerateCaptureSizes = camerap_EnumerateCaptureSizes;
        pCamDrv->camera_PrepareVideoCaptureL = camerap_PrepareVideoCaptureL;
        pCamDrv->camera_StartVideoCapture = camerap_StartVideoCapture;
        pCamDrv->camera_StopVideoCapture = camerap_StopVideoCapture;
        pCamDrv->camera_VideoCaptureActive = camerap_VideoCaptureActive;
        pCamDrv->camera_EnumerateVideoFrameSizes = camerap_EnumerateVideoFrameSizes;
        pCamDrv->camera_EnumerateVideoFrameRates = camerap_EnumerateVideoFrameRates;
        pCamDrv->camera_GetFrameSize = camerap_GetFrameSize;
        pCamDrv->camera_SetFrameRate = camerap_SetFrameRate;
        pCamDrv->camera_FrameRate = camerap_FrameRate;
        pCamDrv->camera_BuffersInUse = camerap_BuffersInUse;
        pCamDrv->camera_FramesPerBuffer = camerap_FramesPerBuffer;
        pCamDrv->camera_GetId = camerap_GetId;
        pCamDrv->camera_GetImageInfo = camerap_GetImageInfo;
        return TRUE;
    }

    return FALSE;
}

PRIVATE void camerap_SetZoomFactorL(UINT32 ZoomFactor)
{
}

PRIVATE UINT32 camerap_ZoomFactor(void)
{
    return (0);
}

PRIVATE void camerap_SetDigitalZoomFactorL(UINT32 DigZoomFactor)
{
}

PRIVATE UINT32 camerap_DigitalZoomFactor(void)
{
    return (0);
}

PRIVATE UINT32 camerap_Contrast(void)
{
    return (0);
}

PRIVATE UINT32 camerap_Brightness(void)
{
    return (0);
}

PRIVATE CAM_FLASH_T camerap_Flash(void)
{
    return (CAM_FLASH_NONE);
}

PRIVATE CAM_EXPOSURE_T camerap_Exposure(void)
{
    return (CAM_EXPOSURE_AUTO);
}

PRIVATE CAM_WHITEBALANCE_T camerap_WhiteBalance(void)
{
    return (CAM_WB_AUTO);
}

PRIVATE CAM_SPECIALEFFECT_T camerap_SpecialEffect(VOID)
{
    return (CAM_SPE_NORMAL);
}

PRIVATE VOID camerap_SetNightMode(UINT32 NightMode)
{

    return;
    if (NightMode)
    {
        PAS6175_NightMode(TRUE);
    }
    else
    {
        PAS6175_NightMode(FALSE);
    }
}

PRIVATE UINT32 camerap_NightMode(VOID)
{
    return (FALSE);
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
    return (FALSE);
}

PRIVATE void camerap_SetViewFinderMirror(BOOL Mirror)
{
}

PRIVATE BOOL camerap_ViewFinderMirror(void)
{
    return (FALSE);
}

PRIVATE void camerap_CancelCaptureImage(void)
{
}

PRIVATE void camerap_EnumerateCaptureSizes(CAM_SIZE_T *Size, UINT32 SizeIndex, CAM_FORMAT_T Format)
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
    return (gSensorInfo.capturevideo);
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
    return (0);
}

PRIVATE UINT32 camerap_BuffersInUse(void)
{
    return (0);
}

PRIVATE UINT32 camerap_FramesPerBuffer(void)
{
    return (0);
}
