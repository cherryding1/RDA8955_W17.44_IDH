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
#include "pmd_m.h"
#include "drv_camera_gc6103.h"
#include "camera_driver.h"
#include "camera_private.h"
#include "tgt_camd_cfg.h"

#ifdef I2C_BASED_ON_GPIO
#include "gpio_i2c.h"
#else
PRIVATE HAL_I2C_BUS_ID_T g_camdI2cBusId = HAL_I2C_BUS_ID_INVALID;
#endif

UINT8 s_i2c_addr = 0;

static UINT32 s_prevCount = 0;
PRIVATE HAL_CAMERA_SPI_MODE_E s_spi_mode = SPI_MODE_MASTER2_2;
PRIVATE BOOL s_camOpened = FALSE;
PRIVATE BOOL gIsCameraStartXfer = FALSE;

PRIVATE CAM_RUNTIME_T s_cam_run = {0,};
PRIVATE CAM_IRQ_HDL_T s_cam_hdl = NULL;

extern BOOL isVideoRecMode;

PRIVATE VOID camerap_Delay(UINT16 Wait_mS)
{
    UINT32 Ticks_16K;    // Here is the delay function used by the camera driver.  It can be changed to use a non-blocking
    // wait if necessary, but for now, just use hal_TimDelay
    Ticks_16K = (16348 * Wait_mS)/1000;
    hal_TimDelay(Ticks_16K);
}

PRIVATE UINT8 camerap_ReadReg(UINT8 Addr)
{
#ifdef I2C_BASED_ON_GPIO
    UINT8 data = 0;

    pas_gpio_i2c_read_data_sensor(s_i2c_addr, &Addr, 1, &data, 1);
    return data;
#else
    if (hal_I2cSendRawByte(g_camdI2cBusId, ((s_i2c_addr<<1) & 0xfe), PHASE2_WR_PH1) == HAL_ERR_RESOURCE_BUSY)
    {
        return(0x1f);
    }
    if (hal_I2cSendRawByte(g_camdI2cBusId, Addr, PHASE2_WR_PH2) == HAL_ERR_RESOURCE_BUSY)
    {
        return(0x1e);
    }
    if (hal_I2cSendRawByte(g_camdI2cBusId, ((s_i2c_addr<<1) | 0x01), PHASE2_RD_PH1) == HAL_ERR_RESOURCE_BUSY)
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
        if( pas_gpio_i2c_write_data_sensor(s_i2c_addr, &Addr, 1, &Data, 1) == TRUE)
            return;

    hal_DbgAssert("gc6113: i2c no ack when writing reg 0x%x", Addr);
#else
    hal_I2cSendRawByte(g_camdI2cBusId, ((s_i2c_addr<<1) & 0xfe), PHASE3_WR_PH1);
    hal_I2cSendRawByte(g_camdI2cBusId, Addr, PHASE3_WR_PH2);
    hal_I2cSendRawByte(g_camdI2cBusId, Data, PHASE3_WR_PH3);
#endif
}

PRIVATE VOID camerap_WriteReg(CAM_REG_T RegWrite)
{
#ifdef I2C_BASED_ON_GPIO
    UINT8 i = 0;
    while (i++ < 3)
        if( pas_gpio_i2c_write_data_sensor(s_i2c_addr, (UINT8*)(&(RegWrite.addr)), 1, &(RegWrite.data), 1) == TRUE)
            return;

    hal_DbgAssert("gc6113: i2c no ack when writing reg 0x%x", RegWrite.Addr);

#else
    hal_I2cSendRawByte(g_camdI2cBusId, ((s_i2c_addr<<1) & 0xfe), PHASE3_WR_PH1);
    hal_I2cSendRawByte(g_camdI2cBusId, RegWrite.addr, PHASE3_WR_PH2);
    hal_I2cSendRawByte(g_camdI2cBusId, RegWrite.data, PHASE3_WR_PH3);
#endif
    // Do delay here using RegWrite.Wait mS
    camerap_Delay(RegWrite.wait);
}

PRIVATE VOID camerap_SendRgSeq(const CAM_REG_T *RegList, UINT16 Len)
{
    UINT16 RegCount;
    for (RegCount = 0; RegCount < Len; ++RegCount)
    {
        camerap_WriteReg(RegList[RegCount]);
    }
}

PRIVATE void camerap_SetI2CAddr(UINT8 addr)
{
    s_i2c_addr = addr;
}

PRIVATE UINT8 camerap_GetI2CAddr(UINT8 addr)
{
    return s_i2c_addr;
}

PRIVATE void camerap_PowerOn(CAM_SIZE_T CamOutSize, UINT16 IspOutWidth, UINT16 IspOutHeight, CAM_FORMAT_T Format)
{

    HAL_CAMERA_CFG_T CamConfig = {0,};
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,};

    CamConfig.pdnActiveH = TRUE;
    CamConfig.camClkDiv = 8;        // 156/8 = 19.5MHz
    CamConfig.camId = camera_GetCameraID();

    CamConfig.spi_pixels_per_line = IspOutWidth;
    CamConfig.spi_pixels_per_column = IspOutHeight;
    CamConfig.spi_yuv_out = SPI_OUT_Y1_V0_Y0_U0;
    CamConfig.spi_mode = s_spi_mode;
    CamConfig.spi_little_endian_en = FALSE;
    CamConfig.spi_ctrl_clk_div = 1;

    pmd_EnablePower(PMD_POWER_CAMERA,TRUE);
    hal_CameraOpen(&CamConfig);
    hal_CameraIrqSetMask(mask);

#ifdef I2C_BASED_ON_GPIO
    gc6113_gpio_i2c_init_sensor();
#else
    g_camdI2cBusId = tgt_GetCamdConfig()->i2cBusId;
    hal_I2cOpen(g_camdI2cBusId);
#endif

    camerap_SetI2CAddr(I2C_ADDR_ISP);
    // To open ISP
    camerap_SendRgSeq(RG_ISP_INIT, (sizeof(RG_ISP_INIT)/sizeof(CAM_REG_T)));
    // To set output size
    if (CamOutSize == CAM_NPIX_QVGA)
        camerap_SendRgSeq(RG_ISP_QVGA, (sizeof(RG_ISP_QVGA)/sizeof(CAM_REG_T)));
    else
        camerap_SendRgSeq(RG_ISP_QQVGA, (sizeof(RG_ISP_QQVGA)/sizeof(CAM_REG_T)));
    // To set SPI mode
    switch(s_spi_mode)
    {
        case SPI_MODE_MASTER2_1:
            camerap_SendRgSeq(RG_SPI_MST2_1, (sizeof(RG_SPI_MST2_1)/sizeof(CAM_REG_T)));
            break;
        case SPI_MODE_MASTER2_2:
            camerap_SendRgSeq(RG_SPI_MST2_2, (sizeof(RG_SPI_MST2_2)/sizeof(CAM_REG_T)));
            break;
        case SPI_MODE_MASTER2_4:
        case SPI_MODE_MASTER1:
        default:
            hal_DbgAssert("GC6113_6103: unsupported SPI mode %d", s_spi_mode);
            break;
    }

    camerap_SetI2CAddr(I2C_ADDR_6103);
    camerap_SendRgSeq(RG_6103_INIT, (sizeof(RG_6103_INIT)/sizeof(CAM_REG_T)));

    if (isVideoRecMode)
    {
        camerap_SendRgSeq(RG_6103_FIX_FR, (sizeof(RG_6103_FIX_FR)/sizeof(CAM_REG_T)));
    }
    else
    {
        camerap_SendRgSeq(RG_6103_FIX_FR, (sizeof(RG_6103_FIX_FR)/sizeof(CAM_REG_T)));
    }

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
            s_cam_run.camMode = CAM_MODE_IDLE;
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
    }

    s_camOpened = FALSE;
    s_prevCount = 0;
}

PRIVATE void camerap_Reserve(CAM_IRQ_HANDLER_T FrameReady)
{
    s_cam_hdl = FrameReady;
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

        s_cam_run.camMode = CAM_MODE_PREVIEW_STAGE_1;
        s_cam_run.pvidBuf = Buffer;
        s_cam_run.nPixels = SizeIndex;

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
    s_cam_run.camMode = CAM_MODE_CAPTURE_STAGE_1;
    s_prevCount = 0;
    mask.fend = 1;

    hal_CameraIrqSetMask(mask);
    hal_CameraControllerEnable(TRUE);
}

PRIVATE void camerap_Release(void)
{

}

PRIVATE UINT8 camerap_GetId(VOID)
{
    UINT8 id = 0;

    camerap_SetI2CAddr(I2C_ADDR_6103);

    camerap_WriteOneReg(0xfc,0x12);
    camerap_WriteOneReg(0xfe,0x00);

    hal_TimDelay(10);
    id = camerap_ReadReg(0x00);

    hal_HstSendEvent(0xaacc0100 | id);
    return id;
}

PRIVATE void camerap_SetContrastL(UINT32 Contrast)
{

}

PRIVATE void camerap_SetBrightnessL(UINT32 Brightness)
{

}

PRIVATE void camerap_SetFlashL(CAM_FLASH_T Flash)
{

}

PRIVATE void camerap_SetExposureL(CAM_EXPOSURE_T Exposure)
{

}

PRIVATE void camerap_SetWhiteBalanceL(CAM_WHITEBALANCE_T WhiteBalance)
{

}

PRIVATE VOID camerap_SetSpecialEffect(CAM_SPECIALEFFECT_T SpecialEffect)
{

}

PRIVATE VOID camerap_InteruptHandler(HAL_CAMERA_IRQ_CAUSE_T cause)
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0, };

    switch (s_cam_run.camMode)
    {
        case CAM_MODE_PREVIEW_STAGE_1:
            if (cause.fend)
            {
                hal_CameraIrqSetMask(mask);

                if (gIsCameraStartXfer == TRUE)
                    hal_CameraStopXfer(FALSE);

                hal_CameraStartXfer((s_cam_run.nPixels<<1), (UINT8*)(s_cam_run.pvidBuf));
                gIsCameraStartXfer = TRUE;

                mask.fend = 1;
                hal_CameraIrqSetMask(mask);

                s_cam_run.camMode = CAM_MODE_PREVIEW_STAGE_2;
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
                        s_cam_hdl(0, 1);
                    }
                    else
                    {
                        s_cam_hdl(1, 1);
                    }
                }

                if (--s_prevCount > 0)
                {
                    hal_CameraStartXfer((s_cam_run.nPixels<<1), (UINT8*)(s_cam_run.pvidBuf));
                    gIsCameraStartXfer = TRUE;
                    mask.fend = 1;
                    hal_CameraIrqSetMask(mask);
                }
                else
                {
                    s_cam_run.camMode = CAM_MODE_IDLE;
                }
            }
            break;

        case CAM_MODE_CAPTURE_STAGE_1:
            if (cause.fend)
            {
                hal_CameraIrqSetMask(mask);
                if (gIsCameraStartXfer == TRUE)
                    hal_CameraStopXfer(FALSE);

                hal_CameraStartXfer((s_cam_run.nPixels<<1), (UINT8*)(s_cam_run.pvidBuf));

                mask.fend = 1;
                gIsCameraStartXfer = TRUE;
                hal_CameraIrqSetMask(mask);
                s_cam_run.camMode = CAM_MODE_CAPTURE_STAGE_2;
            }
            break;

        case CAM_MODE_CAPTURE_STAGE_2:
            if (cause.fend)
            {
                if (gIsCameraStartXfer)
                {
                    gIsCameraStartXfer = FALSE;
                    if (hal_CameraStopXfer(FALSE) == XFER_SUCCESS)
                    {
                        hal_CameraIrqSetMask(mask);
                        s_cam_run.camMode = CAM_MODE_IDLE;
                        s_cam_hdl(0, 1);
                    }
                    else
                    {
                        mask.fend = 1;
                        hal_CameraIrqSetMask(mask);
                        s_cam_run.camMode = CAM_MODE_CAPTURE_STAGE_1;
                    }
                }
                else
                {
                    mask.fend = 1;
                    hal_CameraIrqSetMask(mask);
                    s_cam_run.camMode = CAM_MODE_CAPTURE_STAGE_1;
                }
            }
            break;

        default:
            s_cam_hdl(255, 255);
            break;
    }
}

PRIVATE BOOL camerap_CheckProductId()
{
    HAL_CAMERA_CFG_T CamConfig = {0,};
    UINT8 camId = 0;

    if (camera_GetCameraID() != 1)
    {
        hal_HstSendEvent(0xdcdc0000);
        return FALSE;
    }
    else
    {
        hal_HstSendEvent(0xdcdc0001);
    }

    CamConfig.pdnActiveH = TRUE;
    CamConfig.camClkDiv = 15; // 156MHz/15 = 10.4MHz
    CamConfig.camId = camera_GetCameraID();

    pmd_EnablePower(PMD_POWER_CAMERA,TRUE);
    hal_CameraOpen(&CamConfig);

#ifdef I2C_BASED_ON_GPIO
    gc6113_gpio_i2c_init_sensor();
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

    camerap_SetI2CAddr(I2C_ADDR_6103);
    camId = camerap_GetId();
    hal_HstSendEvent(0xaacc0000 | camId);

    // Disable I2C
#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_close();
#else
    hal_I2cClose(g_camdI2cBusId);
#endif

    hal_CameraClose();
    pmd_EnablePower(PMD_POWER_CAMERA, FALSE);

    if (camId != CHIP_ID_6103)
        return FALSE;

    return TRUE;
}

PRIVATE VOID camerap_GetImageInfo(CAM_SIZE_T * pcamout,UINT16 *pwidth,UINT16 *pheight)
{
    if (!pcamout || !pwidth || !pheight)
        return;

    *pcamout = CAM_NPIX_QVGA;
    *pwidth = 240;
    *pheight = 320;
}

PUBLIC BOOL camera_gc6103_RegInit(CAM_DRV_T  * pCamDrv)
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

PRIVATE void camerap_SetZoomFactorL(UINT32 ZoomFactor) {}

PRIVATE UINT32 camerap_ZoomFactor(void)
{
    return(0);
}

PRIVATE void camerap_SetDigitalZoomFactorL(UINT32 DigZoomFactor) {}

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

PRIVATE VOID camerap_SetNightMode(UINT32 NightMode) {}

PRIVATE UINT32 camerap_NightMode(VOID)
{
    return(FALSE);
}

PRIVATE void camerap_StartViewFinderDirectL() {}

PRIVATE void camerap_StopViewFinder(void) {}

PRIVATE BOOL camerap_ViewFinderActive(void)
{
    return(FALSE);
}

PRIVATE void camerap_SetViewFinderMirror(BOOL Mirror) {}

PRIVATE BOOL camerap_ViewFinderMirror(void)
{
    return(FALSE);
}

PRIVATE void camerap_CancelCaptureImage(void) {}

PRIVATE void camerap_EnumerateCaptureSizes(CAM_SIZE_T *Size,UINT32 SizeIndex, CAM_FORMAT_T Format) {}

PRIVATE void camerap_PrepareVideoCaptureL(CAM_FORMAT_T Format, UINT32 SizeIndex, UINT32 RateIndex, UINT32 BuffersToUse, UINT32 FramesPerBuffer) {}

PRIVATE void camerap_StartVideoCapture(void) {}

PRIVATE void camerap_StopVideoCapture(void) {}

PRIVATE BOOL camerap_VideoCaptureActive(void)
{
    return isVideoRecMode;
}

PRIVATE void camerap_EnumerateVideoFrameSizes(CAM_SIZE_T Size, UINT32 SizeIndex, CAM_FORMAT_T Format) {}

PRIVATE void camerap_EnumerateVideoFrameRates() {}

PRIVATE void camerap_GetFrameSize(CAM_SIZE_T Size) {}

PRIVATE VOID camerap_SetFrameRate(UINT32 FrameRate) {}

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
