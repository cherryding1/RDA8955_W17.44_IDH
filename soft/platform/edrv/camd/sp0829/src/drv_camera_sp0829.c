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

#ifdef I2C_BASED_ON_GPIO
#include "gpio_i2c.h"
#else
PRIVATE HAL_I2C_BUS_ID_T g_camdI2cBusId = HAL_I2C_BUS_ID_INVALID;
#endif


#define CAM_SP0829_ID   0x9a

#if defined(VDOREC_MODE_VGA) || defined(VDOREC_MODE_QVGA)
#error "VGA/QVGA recording not supported on SP0829 yet."
#endif

static INT32 s_prevCount = 0;
PRIVATE HAL_CAMERA_SPI_MODE_E s_SP0829_mode = SPI_MODE_MASTER2_2;

PRIVATE BOOL s_camOpened = FALSE;
PRIVATE BOOL gIsCameraStartXfer = FALSE;

extern BOOL isVideoRecMode;

PRIVATE UINT8 camerap_ReadReg(UINT8 Addr)
{
#ifdef I2C_BASED_ON_GPIO
    UINT8 data = 0;
    pas_gpio_i2c_read_data_sensor(0x86, &Addr, 1, &data, 1);
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
#ifdef I2C_BASED_ON_GPIO
    UINT8 i = 0;

    while (i++ < 3)
        if( pas_gpio_i2c_write_data_sensor(0x86, &Addr, 1, &Data, 1) == TRUE)
            return;

    hal_DbgAssert("SP0829: i2c no ack when writing reg 0x%x", Addr);
#else
    hal_I2cSendRawByte(g_camdI2cBusId, ((gSensorInfo.i2caddress<<1) & 0xfe), PHASE3_WR_PH1);
    hal_I2cSendRawByte(g_camdI2cBusId, Addr, PHASE3_WR_PH2);
    hal_I2cSendRawByte(g_camdI2cBusId, Data, PHASE3_WR_PH3);
#endif
}

PRIVATE VOID camerap_WriteReg(CAM_REG_T RegWrite)
{
#ifdef I2C_BASED_ON_GPIO
    UINT8 i = 0;
    while (i++ < 3)
        if( pas_gpio_i2c_write_data_sensor(0x86, (UINT8*)(&(RegWrite.Addr)), 1, &(RegWrite.Data), 1) == TRUE)
            return;

    hal_DbgAssert("SP0829: i2c no ack when writing reg 0x%x", RegWrite.Addr);

#else
    hal_I2cSendRawByte(g_camdI2cBusId, ((gSensorInfo.i2caddress<<1) & 0xfe), PHASE3_WR_PH1);
    hal_I2cSendRawByte(g_camdI2cBusId, RegWrite.Addr, PHASE3_WR_PH2);
    hal_I2cSendRawByte(g_camdI2cBusId, RegWrite.Data, PHASE3_WR_PH3);
#endif
    // Do delay here using RegWrite.Wait mS
    camerap_Delay(RegWrite.Wait);

}

PRIVATE VOID camerap_SendRgSeq(const CAM_REG_T *RegList, UINT16 Len)
{
    UINT16 RegCount;
    for (RegCount = 0; RegCount < Len; ++RegCount)
    {
        camerap_WriteReg(RegList[RegCount]);
    }
}

//#define USE_CROP 1
PRIVATE void camerap_PowerOn(CAM_SIZE_T CamOutSize, UINT16 IspOutWidth, UINT16 IspOutHeight, CAM_FORMAT_T Format)
{
    HAL_CAMERA_CFG_T CamConfig = {0,};
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};
    UINT16 camOutWidth = 240, camOutHeight = 320;

    SXS_TRACE(TSTDOUT,"DRV_CAM: Power On, camOutSize %d ispw %d isph %d", CamOutSize, IspOutWidth, IspOutHeight);

    if (camOutWidth < IspOutWidth || camOutHeight < IspOutHeight)
        hal_DbgAssert("unsupported cam output size");

    CamConfig.rstActiveH = TRUE;
    CamConfig.pdnActiveH = TRUE;
    CamConfig.dropFrame = FALSE;
    CamConfig.camClkDiv = 8;        // 156/6 = 26M
    CamConfig.endianess = NO_SWAP;
    CamConfig.camId = camera_GetCameraID();
    CamConfig.vsync_inv = TRUE;

    CamConfig.spi_pixels_per_line = IspOutWidth;
    CamConfig.spi_pixels_per_column = IspOutHeight;
    CamConfig.spi_yuv_out = SPI_OUT_Y1_V0_Y0_U0;
    CamConfig.spi_mode = s_SP0829_mode;
    CamConfig.spi_little_endian_en = FALSE;
    CamConfig.spi_ctrl_clk_div = 1;
#if 0
#if USE_CROP
    if (camOutWidth > IspOutWidth || camOutHeight > IspOutHeight)
    {
        CamConfig.cropEnable = TRUE;
        CamConfig.dstWinColStart = ((camOutWidth - IspOutWidth)/2)&0xFFF;
        CamConfig.dstWinColEnd = (CamConfig.dstWinColStart + IspOutWidth - 1)&0xFFF;
        CamConfig.dstWinRowStart = ((camOutHeight - IspOutHeight)/2)&0xFFF;
        CamConfig.dstWinRowEnd = (CamConfig.dstWinRowStart + IspOutHeight - 1)&0xFFF;
    }
#else
    UINT32 ratioCol = camOutWidth / IspOutWidth;
    UINT32 ratioRow = camOutHeight / IspOutHeight;
    if ((ratioCol*IspOutWidth == camOutWidth) && (ratioRow*IspOutHeight == camOutHeight))
    {
        CamConfig.colRatio= ratioCol - 1;
        CamConfig.rowRatio= ratioRow - 1;
        if (ratioCol != 1 && ratioRow != 1)
            CamConfig.reOrder = 4;
    }
    else
    {
        SXS_TRACE(TSTDOUT, "DRV_CAM: failed to set decimation, %d, %d, %d, %d", camOutWidth, IspOutWidth, camOutHeight, IspOutHeight);
        //hal_HstSendEvent(0x20);
        return;
    }
#endif
#endif
    pmd_EnablePower(PMD_POWER_CAMERA,TRUE);

    hal_CameraOpen(&CamConfig);
    hal_CameraIrqSetMask(mask);
    hal_CameraPowerDown(FALSE);

#ifdef I2C_BASED_ON_GPIO
    SP0829_gpio_i2c_init_sensor();
#else
    g_camdI2cBusId = tgt_GetCamdConfig()->i2cBusId;
    hal_I2cOpen(g_camdI2cBusId);
#endif

    if (!gSensorInfo.snrrst)
        hal_CameraReset(FALSE);

    camerap_SendRgSeq(RG_Init_PowerOn,  (sizeof(RG_Init_PowerOn) / sizeof(CAM_REG_T)));

    switch (s_SP0829_mode)
    {
        case SPI_MODE_MASTER2_1:
            SXS_TRACE(TSTDOUT,"spi master 2_1 mode in use");
            camerap_SendRgSeq(RG_Reinit_MASTER2_1,  (sizeof(RG_Reinit_MASTER2_1) / sizeof(CAM_REG_T)));
            break;
        case SPI_MODE_MASTER2_2:
            SXS_TRACE(TSTDOUT,"spi master 2_2 mode in use");
            camerap_SendRgSeq(RG_Reinit_MASTER2_2,  (sizeof(RG_Reinit_MASTER2_2) / sizeof(CAM_REG_T)));
            break;
        case SPI_MODE_MASTER2_4:
            SXS_TRACE(TSTDOUT,"spi master 2_4 mode in use");
            camerap_SendRgSeq(RG_Reinit_MASTER2_4,  (sizeof(RG_Reinit_MASTER2_4) / sizeof(CAM_REG_T)));
            break;
        case SPI_MODE_NO://并口
            SXS_TRACE(TSTDOUT,"no spi master in use");
            break;
        default:
            SXS_TRACE(TSTDOUT,"unsupported spi mode");
            break;
    }

#if 0
    if (CamOutSize == CAM_NPIX_QVGA)
        camerap_SendRgSeq(RG_Reinit_QVGA,  (sizeof(RG_Reinit_QVGA) / sizeof(CAM_REG_T)));
    if (isVideoRecMode == TRUE)
    {
        camerap_SendRgSeq(RG_Reinit_QQVGA_2,  (sizeof(RG_Reinit_QQVGA_2) / sizeof(CAM_REG_T)));
    }
#endif

    s_prevCount = 0;
    s_camOpened = TRUE;

}

PRIVATE void camerap_PowerOff(void)
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};

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
    s_prevCount = 0;
}

PRIVATE void camerap_Reserve(CAM_IRQ_HANDLER_T FrameReady)
{
    gSensorInfo.camirqhandler = FrameReady;
    hal_CameraIrqSetHandler(camerap_InteruptHandler);
}

PRIVATE void camerap_PrepareImageCaptureL(CAM_FORMAT_T ImageFormat, UINT32 SizeIndex, CAM_IMAGE_T Buffer)
{
    if (s_prevCount == 0)
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

        s_prevCount = 2;
        mask.fend = 1;

        hal_CameraIrqSetMask(mask);
        hal_CameraControllerEnable(TRUE);
    }
    else
    {
        UINT32 status = hal_SysEnterCriticalSection();
        s_prevCount++;
        hal_SysExitCriticalSection(status);
    }
}

PRIVATE void camerap_CaptureImage()
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};

    hal_CameraIrqSetMask(mask);
    if (gIsCameraStartXfer == TRUE)
    {
        hal_CameraStopXfer(FALSE);
        gIsCameraStartXfer = FALSE;
    }
    gSensorInfo.cammode = CAM_MODE_CAPTURE_STAGE_1;

    s_prevCount = 0;
    mask.fend = 1;

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

PRIVATE UINT8 camerap_GetId(VOID)
{
    hal_TimDelay(10);
    return(camerap_ReadReg(0x02));
}

PRIVATE void camerap_SetContrastL(UINT32 Contrast)
{

    switch(Contrast)
    {
        case 1:
            camerap_WriteOneReg(0x5c, 0xc0);
            break;
        case 2:
            camerap_WriteOneReg(0x5c, 0xb0);
            break;
        case 3:
            camerap_WriteOneReg(0x5c, 0xa0);
            break;
        case 4:
            camerap_WriteOneReg(0x5c, 0x90);
            break;
        case 5:
            camerap_WriteOneReg(0x5c, 0x80);
            break;
        default: break;
    }
}

PRIVATE void camerap_SetBrightnessL(UINT32 Brightness)
{

    switch(Brightness)
    {
        case 1:    //0
            camerap_WriteOneReg(0x78, 0x00);
            break;
        case 2:   //-2
            camerap_WriteOneReg(0x78, 0xd0);
            break;
        case 3: //-1
            camerap_WriteOneReg(0x78, 0xe8);
            break;
        case 4:  //+1
            camerap_WriteOneReg(0x78, 0x18);
            break;
        case 5:  //+2
            camerap_WriteOneReg(0x78, 0x30);
            break;
        default :
            break;
    }
}

PRIVATE void camerap_SetFlashL(CAM_FLASH_T Flash)
{
    return;
}

PRIVATE void camerap_SetExposureL(CAM_EXPOSURE_T Exposure)
{

    switch(Exposure)
    {
        case CAM_EXPOSURE_AUTO:     //0
            camerap_WriteOneReg(0x98,REG98);
            camerap_WriteOneReg(0x9e,REG9e);
            camerap_WriteOneReg(0x9f,REG9f);
            camerap_WriteOneReg(0x97,REG97);

            camerap_WriteOneReg(0x9a,REG9a);
            camerap_WriteOneReg(0xa0,REGa0);
            camerap_WriteOneReg(0xa1,REGa1);
            camerap_WriteOneReg(0x99,REG99);
            break;
        case CAM_EXPOSURE_NEGATIVE_2:   //-2
            camerap_WriteOneReg(0x98,REG98-0x20);
            camerap_WriteOneReg(0x9e,REG9e-0x20);
            camerap_WriteOneReg(0x9f,REG9f-0x20);
            camerap_WriteOneReg(0x97,REG97-0x20);

            camerap_WriteOneReg(0x9a,REG9a-0x20);
            camerap_WriteOneReg(0xa0,REGa0-0x20);
            camerap_WriteOneReg(0xa1,REGa1-0x20);
            camerap_WriteOneReg(0x99,REG99-0x20);
            break;
        case CAM_EXPOSURE_NEGATIVE_1: //-1
            camerap_WriteOneReg(0x98,REG98-0x10);
            camerap_WriteOneReg(0x9e,REG9e-0x10);
            camerap_WriteOneReg(0x9f,REG9f-0x10);
            camerap_WriteOneReg(0x97,REG97-0x10);

            camerap_WriteOneReg(0x9a,REG9a-0x10);
            camerap_WriteOneReg(0xa0,REGa0-0x10);
            camerap_WriteOneReg(0xa1,REGa1-0x10);
            camerap_WriteOneReg(0x99,REG99-0x10);
            break;
        case CAM_EXPOSURE_POSITIVE_1:  //+1
            camerap_WriteOneReg(0x98,REG98+0x10);
            camerap_WriteOneReg(0x9e,REG9e+0x10);
            camerap_WriteOneReg(0x9f,REG9f+0x10);
            camerap_WriteOneReg(0x97,REG97+0x10);

            camerap_WriteOneReg(0x9a,REG9a+0x10);
            camerap_WriteOneReg(0xa0,REGa0+0x10);
            camerap_WriteOneReg(0xa1,REGa1+0x10);
            camerap_WriteOneReg(0x99,REG99+0x10);
            break;
        case CAM_EXPOSURE_POSITIVE_2:  //+2
            camerap_WriteOneReg(0x98,REG98+0x20);
            camerap_WriteOneReg(0x9e,REG9e+0x20);
            camerap_WriteOneReg(0x9f,REG9f+0x20);
            camerap_WriteOneReg(0x97,REG97+0x20);

            camerap_WriteOneReg(0x9a,REG9a+0x20);
            camerap_WriteOneReg(0xa0,REGa0+0x20);
            camerap_WriteOneReg(0xa1,REGa1+0x20);
            camerap_WriteOneReg(0x99,REG99+0x20);
            break;

        default : break;
    }
}

PRIVATE void camerap_SetWhiteBalanceL(CAM_WHITEBALANCE_T WhiteBalance)
{
    UINT8 TempReg;

    //TempReg = camerap_ReadReg(0x22);
    switch(WhiteBalance)
    {
        case CAM_WB_AUTO:  /*Auto */
            camerap_WriteOneReg(0xc5, 0xc4);
            camerap_WriteOneReg(0xc6, 0x9f);
            camerap_WriteOneReg(0xe7, 0x03);
            camerap_WriteOneReg(0xe7, 0x00);
            camerap_WriteOneReg(0x32, 0x0d);
            break;
        case CAM_WB_DAYLIGHT:   /*晴天太阳光*/
            camerap_WriteOneReg(0x32, 0x05);  // disable AWB
            camerap_WriteOneReg(0xc5, 0xc5);
            camerap_WriteOneReg(0xc6, 0x8a);
            break;
        case CAM_WB_OFFICE:   /*日光灯*/
            camerap_WriteOneReg(0x32, 0x05);  // disable AWB
            camerap_WriteOneReg(0xc5, 0x9c);
            camerap_WriteOneReg(0xc6, 0xaf);
            break;
        case CAM_WB_CLOUDY:  /*阴天*/
            camerap_WriteOneReg(0x32, 0x05);  // disable AWB
            camerap_WriteOneReg(0xc5, 0xe5);
            camerap_WriteOneReg(0xc6, 0x80);
            break;
        case CAM_WB_TUNGSTEN:  /*钨丝灯*/
            camerap_WriteOneReg(0x32, 0x05);  // disable AWB
            camerap_WriteOneReg(0xc5, 0xa8);
            camerap_WriteOneReg(0xc6, 0xb7);
            break;

        default : break;
    }
}

PRIVATE VOID camerap_SetSpecialEffect(CAM_SPECIALEFFECT_T SpecialEffect)
{
    // UINT8 TempReg;
    // TempReg = camera_ReadReg(0x47);
    switch(SpecialEffect)
    {
        case CAM_SPE_NORMAL :
            camerap_WriteOneReg(0x62, 0x00);
            camerap_WriteOneReg(0x63, 0x80);
            camerap_WriteOneReg(0x64, 0x80);
            break;
        case CAM_SPE_ANTIQUE    :
            camerap_WriteOneReg(0x62, 0x04);
            camerap_WriteOneReg(0x63, 0xc0);
            camerap_WriteOneReg(0x64, 0x20);
            break;
        case CAM_SPE_REDISH :
            camerap_WriteOneReg(0x62, 0x04);
            camerap_WriteOneReg(0x63, 0xf0);
            camerap_WriteOneReg(0x64, 0x20);
            break;
        case CAM_SPE_GREENISH:
            camerap_WriteOneReg(0x62, 0x04);
            camerap_WriteOneReg(0x63, 0x50);
            camerap_WriteOneReg(0x64, 0x50);
            break;
        case CAM_SPE_BLUEISH    :
            camerap_WriteOneReg(0x62, 0x04);
            camerap_WriteOneReg(0x63, 0x20);
            camerap_WriteOneReg(0x64, 0xf0);
            break;
        case CAM_SPE_BLACKWHITE :
            camerap_WriteOneReg(0x62, 0x08);
            camerap_WriteOneReg(0x63, 0x80);
            camerap_WriteOneReg(0x64, 0x80);
            break;
        case CAM_SPE_NEGATIVE:
            camerap_WriteOneReg(0x62, 0x01);
            camerap_WriteOneReg(0x63, 0x80);
            camerap_WriteOneReg(0x64, 0x80);
            break;
        case CAM_SPE_BLACKWHITE_NEGATIVE:
            camerap_WriteOneReg(0x62, 0x02);
            camerap_WriteOneReg(0x63, 0x80);
            camerap_WriteOneReg(0x64, 0x80);
            break;
        default: break;
    }
}

PRIVATE VOID camerap_InteruptHandler(HAL_CAMERA_IRQ_CAUSE_T cause)
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0, };

    switch (gSensorInfo.cammode)
    {
        case CAM_MODE_PREVIEW_STAGE_1:
            if (cause.fend)
            {
                hal_CameraIrqSetMask(mask);

                if (gIsCameraStartXfer == TRUE) hal_CameraStopXfer(FALSE);

                hal_CameraStartXfer((gSensorInfo.npixels<<1), (UINT8*)(gSensorInfo.vidbuffer));
                gIsCameraStartXfer = TRUE;

                mask.fend = 1;
                hal_CameraIrqSetMask(mask);

                gSensorInfo.cammode = CAM_MODE_PREVIEW_STAGE_2;
            }
            break;

        case CAM_MODE_PREVIEW_STAGE_2:
            if (cause.fend)
            {
                hal_CameraIrqSetMask(mask);
                if (gIsCameraStartXfer == TRUE)
                {
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
                }

                if (--s_prevCount > 0)
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
            break;

        case CAM_MODE_CAPTURE_STAGE_1:
            if (cause.fend)
            {
                hal_CameraIrqSetMask(mask);
                if (gIsCameraStartXfer == TRUE)
                    hal_CameraStopXfer(FALSE);

                hal_CameraStartXfer((gSensorInfo.npixels<<1), (UINT8*)(gSensorInfo.vidbuffer));
                gIsCameraStartXfer = TRUE;

                mask.fend = 1;
                hal_CameraIrqSetMask(mask);
                gSensorInfo.cammode = CAM_MODE_CAPTURE_STAGE_2;
            }
            break;

        case CAM_MODE_CAPTURE_STAGE_2:
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
                else
                {
                    mask.fend = 1;
                    hal_CameraIrqSetMask(mask);
                    gSensorInfo.cammode = CAM_MODE_CAPTURE_STAGE_1;
                }
            }
            break;

        default:
            gSensorInfo.camirqhandler(255, 255);
            break;
    }
}
PRIVATE BOOL camerap_CheckProductId()
{
    UINT8 cameraID=0;

    HAL_CAMERA_CFG_T CamConfig = {0,};
    CamConfig.rstActiveH = TRUE;
    CamConfig.pdnActiveH = TRUE;
    CamConfig.dropFrame = FALSE;
    CamConfig.camClkDiv = 6; // 156MHz/4 = 39MHz
    CamConfig.endianess = NO_SWAP;//BYTE_SWAP
    CamConfig.colRatio= COL_RATIO_1_1;
    CamConfig.rowRatio= ROW_RATIO_1_1;
    CamConfig.camId = camera_GetCameraID();

    pmd_EnablePower(PMD_POWER_CAMERA,TRUE);

    hal_CameraOpen(&CamConfig);
    hal_CameraReset(TRUE);
    hal_CameraPowerDown(FALSE);

#ifdef I2C_BASED_ON_GPIO
    SP0829_gpio_i2c_init_sensor();
#else
    g_camdI2cBusId = tgt_GetCamdConfig()->i2cBusId;
    UINT8 i = 0;
    do
    {
        if (HAL_ERR_NO == hal_I2cOpen(g_camdI2cBusId)) break;
    }
    while(++i != 3);
    if (i == 3) return FALSE;
#endif

    if (!gSensorInfo.snrrst)
        hal_CameraReset(FALSE);

    SXS_TRACE(TSTDOUT,"SP0829: start to get the cam id");
    camerap_Delay(20);
    cameraID = camerap_GetId();
    SXS_TRACE(TSTDOUT,"SP0829: got cam id 0x%x", cameraID);
    // Disable I2C
#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_close();
#else
    hal_I2cClose(g_camdI2cBusId);
#endif

    hal_CameraClose();
    pmd_EnablePower(PMD_POWER_CAMERA, FALSE);

    SXS_TRACE(TSTDOUT,"SP0829(0x%02x): read id is 0x%02x", CAM_SP0829_ID, cameraID);

    if(cameraID == CAM_SP0829_ID)
        return TRUE;
    else
        return FALSE;
}

PRIVATE VOID camerap_GetImageInfo(UINT32 * pcamout,UINT16 *pwidth,UINT16 *pheight)
{
    *pcamout = CAM_NPIX_QVGA;
    *pwidth = 240;
    *pheight = 320;
}

PUBLIC BOOL camera_sp0829_RegInit(CAM_DRV_T  * pCamDrv)
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
    if (NightMode)
    {
        camerap_WriteOneReg(0x86,0x40);
        camerap_WriteOneReg(0x87,0x1f);
    }
    else
    {
        camerap_WriteOneReg(0x86,0x20);
        camerap_WriteOneReg(0x87,0x1f);
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


