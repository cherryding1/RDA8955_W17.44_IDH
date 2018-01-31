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

#include "camera_private.h"
#include "camera_driver.h"
#include "tgt_camd_cfg.h"

#include "drv_camera_gc2015.h"

#define CAM_GC2015_ID 0x20

PRIVATE volatile UINT8 gIfcChan = 0xff;
PRIVATE BOOL s_camOpened = FALSE;
PRIVATE BOOL gIsCameraStartXfer = FALSE;

PRIVATE CAM_SIZE_T s_camSize = CAM_NPIX_UXGA;

#ifdef I2C_BASED_ON_GPIO
#include "gpio_i2c.h"
#else
PRIVATE HAL_I2C_BUS_ID_T g_camdI2cBusId = HAL_I2C_BUS_ID_INVALID;
#endif

extern BOOL isVideoRecMode;

PRIVATE UINT8 camerap_ReadReg(UINT8 Addr)
{
#ifdef I2C_BASED_ON_GPIO
    UINT8 data = 0;

    gpio_i2c_write_data(gSensorInfo.i2caddress, &Addr, 1, (unsigned char *)&data, 0);
    return(gpio_i2c_read_data(gSensorInfo.i2caddress, &Addr,0, data, 1));
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

PRIVATE VOID  camerap_WriteOneReg(UINT8 Addr, UINT8 Data)
{
#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_write_data(gSensorInfo.i2caddress,&Addr,1, &Data, 1);
#else
    hal_I2cSendRawByte(g_camdI2cBusId, ((gSensorInfo.i2caddress<<1) & 0xfe), PHASE3_WR_PH1);
    hal_I2cSendRawByte(g_camdI2cBusId, Addr, PHASE3_WR_PH2);
    hal_I2cSendRawByte(g_camdI2cBusId, Data, PHASE3_WR_PH3);
#endif
}

PRIVATE VOID camerap_WriteReg (CAM_REG_T RegWrite)
{
#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_write_data(gSensorInfo.i2caddress,(unsigned char * )&RegWrite.Addr,1, &RegWrite.Data, 1);
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

PRIVATE void camerap_PowerOn(CAM_SIZE_T CamOutSize, UINT16 IspOutWidth, UINT16 IspOutHeight, CAM_FORMAT_T Format)
{
    HAL_CAMERA_CFG_T CamConfig = {0,};
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,};

    SXS_TRACE(TSTDOUT,"DRV_CAM: Power On, camOutSize %d ispw %d isph %d", CamOutSize, IspOutWidth, IspOutHeight);

    CamConfig.pdnActiveH = TRUE;
    CamConfig.camClkDiv = 4;
    CamConfig.camId = 1; // front sensor
    CamConfig.reOrder = 2;

    pmd_EnablePower(PMD_POWER_CAMERA,TRUE);

#ifdef I2C_BASED_ON_GPIO
    gc6113_gpio_i2c_init_sensor();
#else
    g_camdI2cBusId = tgt_GetCamdConfig()->i2cBusId;
    hal_I2cOpen(g_camdI2cBusId);
#endif

    hal_CameraOpen(&CamConfig);
    hal_CameraIrqSetMask(mask);

    hal_CameraPowerDown(FALSE);

    if (!gSensorInfo.snrrst)
        hal_CameraReset(FALSE);

    hal_CameraSetVsyncInvert(FALSE);

    // General setting
    camerap_SendRgSeq(RG_Init_PowerOn,  (sizeof(RG_Init_PowerOn)/sizeof(CAM_REG_T)));

    if (CamOutSize == CAM_NPIX_UXGA)
    {
        camerap_SendRgSeq(RG_Init_UXGA, (sizeof(RG_Init_UXGA)/sizeof(CAM_REG_T)));
        s_camSize = CAM_NPIX_UXGA;
    }
    else if (CamOutSize == CAM_NPIX_SXGA)
    {
        camerap_SendRgSeq(RG_Init_SXGA, (sizeof(RG_Init_SXGA)/sizeof(CAM_REG_T)));
        s_camSize = CAM_NPIX_SXGA;
    }
    else if (CamOutSize == CAM_NPIX_VGA)
    {
        camerap_SendRgSeq(RG_Init_VGA, (sizeof(RG_Init_VGA)/sizeof(CAM_REG_T)));
        s_camSize = CAM_NPIX_VGA;
    }
    else
    {
        hal_DbgAssert("GC2015: unsupported size %dx%d", IspOutWidth, IspOutHeight);
    }

    // Self-adaptive frame rate
    if (isVideoRecMode)
        camerap_SendRgSeq(RG_Init_FIXED_FR, (sizeof(RG_Init_FIXED_FR)/sizeof(CAM_REG_T)));
    else
        camerap_SendRgSeq(RG_Init_ADP_FR, (sizeof(RG_Init_ADP_FR)/sizeof(CAM_REG_T)));

    s_camOpened = TRUE;
}

PRIVATE void camerap_PowerOff(void)
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0, };

    if (s_camOpened == TRUE)
    {
        hal_CameraIrqSetMask(mask);
        if(gIsCameraStartXfer == TRUE)
        {
            hal_CameraStopXfer(TRUE);
            gSensorInfo.cammode = CAM_MODE_IDLE;
            gIsCameraStartXfer = FALSE;
        }
        hal_CameraClose();

#ifdef I2C_BASED_ON_GPIO
        gpio_i2c_close();
#else
        hal_I2cClose(g_camdI2cBusId);
#endif
        pmd_EnablePower(PMD_POWER_CAMERA, FALSE);
        s_camOpened = FALSE;
    }
}

PRIVATE void camerap_Reserve(CAM_IRQ_HANDLER_T FrameReady)
{
    gSensorInfo.camirqhandler = FrameReady;
    hal_CameraIrqSetHandler(camerap_InteruptHandler);
}

PRIVATE void camerap_PrepareImageCaptureL(CAM_FORMAT_T ImageFormat, UINT32 SizeIndex, CAM_IMAGE_T Buffer)
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,};

    hal_CameraIrqSetMask(mask);
    if (gIsCameraStartXfer == TRUE)
    {
        hal_CameraStopXfer(FALSE);
        gIsCameraStartXfer = FALSE;
    }

    if (s_camSize != CAM_NPIX_VGA)
    {
        camerap_SendRgSeq(RG_Init_VGA,  (sizeof(RG_Init_VGA) / sizeof(CAM_REG_T)));
        s_camSize = CAM_NPIX_VGA;
    }

    gSensorInfo.cammode = CAM_MODE_VIEWFINDER;
    gSensorInfo.vidbuffer = Buffer;
    gSensorInfo.npixels = SizeIndex;
    gSensorInfo.format = ImageFormat;

    mask.fstart = 1;
    hal_CameraIrqSetMask(mask);
    hal_CameraControllerEnable(TRUE);
}

PRIVATE void camerap_CaptureImage()
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,};

    hal_CameraIrqSetMask(mask);

    if (gIsCameraStartXfer == TRUE)
    {
        hal_CameraStopXfer(FALSE);
        gIsCameraStartXfer = FALSE;
    }

    if (s_camSize != CAM_NPIX_UXGA)
    {
        camerap_SendRgSeq(RG_Init_UXGA,  (sizeof(RG_Init_UXGA) / sizeof(CAM_REG_T)));
        s_camSize = CAM_NPIX_UXGA;
    }

    gSensorInfo.cammode = CAM_MODE_PREP_IMG_CAPTURE;
    gSensorInfo.npixels = CAM_NPIX_UXGA;
    mask.fstart = 1;

    hal_CameraIrqSetMask(mask);
    hal_CameraControllerEnable(TRUE);
}

PRIVATE void camerap_Release(void)
{

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
    // only read the high part of the ID
    return(camerap_ReadReg(0x00));
}

PRIVATE VOID camerap_InteruptHandler(HAL_CAMERA_IRQ_CAUSE_T cause)
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};

    //SXS_TRACE(TSTDOUT, "[MMC_CAMERA]camerap_InteruptHandler tick: %d", hal_TimGetUpTime());

    hal_CameraIrqSetMask(mask);
    switch (gSensorInfo.cammode)
    {
        case CAM_MODE_VIEWFINDER:
            if (cause.fstart)
            {
                SXS_TRACE(TSTDOUT, "[MMC_CAMERA]camerap_InteruptHandler, VIEW_FIND mode, fstart, %d", hal_TimGetUpTime());
                gIfcChan = hal_CameraStartXfer((gSensorInfo.npixels<<1), (UINT8*)gSensorInfo.vidbuffer);
                gIsCameraStartXfer = TRUE;
                mask.fend = 1;
                hal_CameraIrqSetMask(mask);
            }
            else if (cause.fend)
            {
                SXS_TRACE(TSTDOUT, "[MMC_CAMERA]camerap_InteruptHandler, VIEW_FIND mode, fend");
                if (gIsCameraStartXfer)
                {
                    gIsCameraStartXfer = FALSE;
                    gSensorInfo.cammode = CAM_MODE_IDLE;
                    if(hal_CameraStopXfer(FALSE) == XFER_SUCCESS)
                    {
                        gSensorInfo.camirqhandler(0, gSensorInfo.currentbuf);
                    }
                    else
                    {
                        gSensorInfo.camirqhandler(1, gSensorInfo.currentbuf);
                    }
                }
            }
            break;

        case CAM_MODE_PREP_IMG_CAPTURE:
            if (cause.fstart)
            {
//SXS_TRACE(TSTDOUT, "[MMC_CAMERA]camerap_InteruptHandler, PRE_IMG mode, fstart");
                gIfcChan = hal_CameraStartXfer((gSensorInfo.npixels<<1), (UINT8*)gSensorInfo.vidbuffer);
                gIsCameraStartXfer = TRUE;
                gSensorInfo.cammode = CAM_MODE_IMG_CAPTURE;
                mask.fend = 1;
                hal_CameraIrqSetMask(mask);
            }
            else if (cause.fend)
            {
//SXS_TRACE(TSTDOUT, "[MMC_CAMERA]camerap_InteruptHandler, PRE_IMG mode, fend");
                if (gIsCameraStartXfer)
                {
                    gIsCameraStartXfer = FALSE;
                    hal_CameraStopXfer(FALSE);
                }
                mask.fstart = 1;
                hal_CameraIrqSetMask(mask);
            }
            break;

        case CAM_MODE_IMG_CAPTURE:
//SXS_TRACE(TSTDOUT, "[MMC_CAMERA]camerap_InteruptHandler, IMG_CAP mode");
            gIsCameraStartXfer=FALSE;
            if(hal_CameraStopXfer(FALSE)==XFER_SUCCESS)
            {
                gSensorInfo.cammode = CAM_MODE_IDLE;
                hal_CameraIrqSetMask(mask);
                gSensorInfo.camirqhandler(0, gSensorInfo.currentbuf);
            }
            else
            {
                mask.fstart = 1;
                hal_CameraIrqSetMask(mask);
                gSensorInfo.cammode = CAM_MODE_PREP_IMG_CAPTURE;
            }
            break;

        default:
//SXS_TRACE(TSTDOUT, "[MMC_CAMERA]camerap_InteruptHandler, unkown mode");
            gSensorInfo.camirqhandler(255, 255);
            break;
    }
}

PRIVATE BOOL camerap_CheckProductId()
{
    HAL_CAMERA_CFG_T CamConfig = {0,};
    UINT8 cameraID = 0;
    UINT8 i = 0;

    // Power Supply
    pmd_EnablePower(PMD_POWER_CAMERA,TRUE);

#ifdef I2C_BASED_ON_GPIO
    gc6113_gpio_i2c_init_sensor();
#else
    g_camdI2cBusId = tgt_GetCamdConfig()->i2cBusId;
    hal_I2cOpen(g_camdI2cBusId);
#endif

    // PDWN is low effective
    CamConfig.pdnActiveH = TRUE;
    // RST is high effective
    CamConfig.rstActiveH = FALSE;
    CamConfig.camClkDiv = 6;
    //CamConfig.camId = camera_GetCameraID();
    CamConfig.camId = 1;

    // Output MCLK
    hal_CameraOpen(&CamConfig);

    hal_CameraReset(FALSE);
    hal_CameraPowerDown(FALSE);

    do
    {
        camerap_Delay(10);
        cameraID = camerap_GetId();
        if (++i == 0xff)
        {
            i = 0;
            hal_HstSendEvent(cameraID);
        }
    }
    while (cameraID != CAM_GC2015_ID);

    hal_CameraClose();
    pmd_EnablePower(PMD_POWER_CAMERA, FALSE);

    SXS_TRACE(TSTDOUT,"GC2015(0x%02x): read id is 0x%02x", CAM_GC2015_ID, cameraID);

    if(cameraID == CAM_GC2015_ID)
        return TRUE;
    else
        return FALSE;
}

PRIVATE void camerap_SetBrightnessL(UINT32 Brightness)
{

}

PRIVATE UINT32 camerap_Brightness()
{
    return 0;
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

PRIVATE void camerap_SetZoomFactorL(UINT32 ZoomFactor)
{

}

PRIVATE UINT32 camerap_ZoomFactor(void)
{

    return 0;
}

PRIVATE void camerap_SetDigitalZoomFactorL(UINT32 DigZoomFactor)
{

}

PRIVATE UINT32 camerap_DigitalZoomFactor(void)
{
    return 0;
}

PRIVATE void camerap_SetContrastL(UINT32 Contrast)
{

}

PRIVATE UINT32 camerap_Contrast(void)
{
    return 0;
}

PRIVATE UINT32 camerap_NightMode(VOID)
{
    return 0;
}

PRIVATE VOID camerap_SetNightMode(UINT32 NightMode)
{

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
    return FALSE;
}

PRIVATE void camerap_SetViewFinderMirror(BOOL Mirror)
{
}

PRIVATE BOOL camerap_ViewFinderMirror(void)
{
    return FALSE;
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
    return 0;
}

PRIVATE UINT32 camerap_BuffersInUse(void)
{
    return 0;
}

PRIVATE UINT32 camerap_FramesPerBuffer(void)
{
    return 0;
}

PRIVATE void camerap_GetImageInfo(CAM_SIZE_T *pcamout,UINT16 *pwidth, UINT16 *pheight)
{
    if(NULL == pcamout ||NULL == pwidth || NULL == pheight)
    {
        return;
    }

    *pcamout = CAM_NPIX_UXGA;
    *pwidth = 1600;
    *pheight = 1200;
}

PUBLIC BOOL camera_gc2015_RegInit(CAM_DRV_T *pCamDrv)
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
        pCamDrv->camera_SetExposureL = camerap_SetExposureL;
        pCamDrv->camera_SetWhiteBalanceL = camerap_SetWhiteBalanceL;
        pCamDrv->camera_SetSpecialEffect = camerap_SetSpecialEffect;
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



