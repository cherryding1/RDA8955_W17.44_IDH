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
#include "atv_interface.h"
#include "pmd_m.h"
#include "camera_driver.h"
#include "camera_private.h"
#include "tgt_camd_cfg.h"

PRIVATE BOOL s_camOpened = FALSE;
PRIVATE volatile UINT8 gIfcChan = 0xff;


// ############
// Private
// ############

extern UINT32 gCameraID;

// The camera will be powered on in a particular mode specified here
// Later, when the video capture or image capture is 'prepared', the frame buffer size
// must correspond to the size here unless the format is changed.

#include "atvd_m.h"

//#define ATV_SKIP_hal_Xfer_Data_Error

PRIVATE BOOL IsAtvOn=FALSE;

PRIVATE ATV_ISP_SETTING_T Rda5888E_ISP_Setting =
{
    /*  UINT16 SrcWidth;    (BuffSize)*/                360,
    /*  UINT16 Srcheight;   (BuffSize)*/                288,
    /*  UINT16 colGrabStart;        */                  6,
    /*  UINT16 colGrabEnd;      */                      365,
    /*  UINT16 rowGrabStart;        */                  1,
    /*  UINT16 rowGrabEnd;      */                      288,
    /*  UINT16 ispOutputStartPos;   */                  24,
    /*  UINT16 ispOutputWidth;      */                  336,
    /*  UINT16 ispOutputheight; */                      288,
};

PRIVATE uint8 m_bIsRDAMTVMode = 0;
PRIVATE BOOL m_bIsFullScr = FALSE;

PUBLIC ATV_ISP_SETTING_T * atvif_GetISPSetting(void)
{
    return &Rda5888E_ISP_Setting;
}

PUBLIC void atvif_ISPSettingInit(void)
{
    if (atvd_Is625Lines())
    {
        Rda5888E_ISP_Setting.SrcWidth           =   360;
        Rda5888E_ISP_Setting.Srcheight          =   288;
        Rda5888E_ISP_Setting.colGrabStart       =   6;
        Rda5888E_ISP_Setting.colGrabEnd         =   365;
        Rda5888E_ISP_Setting.rowGrabStart       =   1;
        Rda5888E_ISP_Setting.rowGrabEnd         =   288;
        Rda5888E_ISP_Setting.ispOutputStartPos  =   24;
        Rda5888E_ISP_Setting.ispOutputWidth     =   332;
        Rda5888E_ISP_Setting.ispOutputheight    =   272;
    }
    else
    {
        Rda5888E_ISP_Setting.SrcWidth           =   360;
        Rda5888E_ISP_Setting.Srcheight          =   240;
        Rda5888E_ISP_Setting.colGrabStart       =   2;
        Rda5888E_ISP_Setting.colGrabEnd         =   361;
        Rda5888E_ISP_Setting.rowGrabStart       =   1;
        Rda5888E_ISP_Setting.rowGrabEnd         =   240;
        Rda5888E_ISP_Setting.ispOutputStartPos  =   20;
        Rda5888E_ISP_Setting.ispOutputWidth     =   340;
        Rda5888E_ISP_Setting.ispOutputheight    =   238;
    }
}

BOOL IsAtvPowerOn(void)
{
    return IsAtvOn;
}

PUBLIC void atvif_PreviewSettingInit(void)
{
    HAL_CAMERA_CFG_T CamConfig = {0,};

    atvif_ISPSettingInit();

    CamConfig.rstActiveH = FALSE;
    CamConfig.pdnActiveH = TRUE;
    CamConfig.dropFrame = TRUE; //FALSE;
    CamConfig.camClkDiv = 6; // 156MHz/6 = 26MHz
    CamConfig.endianess = NO_SWAP;//BYTE_SWAP;//
    CamConfig.colRatio= COL_RATIO_1_1;
    CamConfig.rowRatio= ROW_RATIO_1_1;
    CamConfig.camId = camera_GetCameraID();

    CamConfig.dstWinColStart = Rda5888E_ISP_Setting.colGrabStart;
    CamConfig.dstWinColEnd   = Rda5888E_ISP_Setting.colGrabEnd;

    CamConfig.dstWinRowStart = Rda5888E_ISP_Setting.rowGrabStart;
    CamConfig.dstWinRowEnd   = Rda5888E_ISP_Setting.rowGrabEnd;
    CamConfig.cropEnable = TRUE; //as XYF's suggust FALSE but need adjust ISP setting

    SXS_TRACE(TSTDOUT,"DRV_CAM: ISP dstWinRowStart %d dstWinRowEnd %d", CamConfig.dstWinRowStart, CamConfig.dstWinRowEnd);
    SXS_TRACE(TSTDOUT,"DRV_CAM: ISP dstWinColStart %d dstWinColEnd %d", CamConfig.dstWinColStart, CamConfig.dstWinColEnd);

    hal_CameraOpen(&CamConfig);
    s_camOpened = TRUE;

    // Mask all interrupts
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};
    hal_CameraIrqSetMask(mask);
}

PUBLIC uint8 atvif_GetCurModule(void)
{
    //mmi_trace(1," atvif_GetCurModule %d ",m_bIsRDAMTVMode );
    return m_bIsRDAMTVMode;
}

PUBLIC BOOL atvif_IsFullScreen(void)
{
    //mmi_trace(1 , "[rdamtv]atvif_IsFullScreen\n");
    return m_bIsFullScr;
}

PUBLIC void atvif_SetFullScreen(BOOL on)
{
    m_bIsFullScr = on;
}

PUBLIC void atvif_SetCurModule(unsigned char is_atv_app)
{
    //mmi_trace(1," atvif_SetCurModule %d ",is_atv_app );
    m_bIsRDAMTVMode = is_atv_app;
}

PRIVATE void camerap_PowerOn(CAM_SIZE_T CamOutSize, UINT16 IspOutWidth, UINT16 IspOutHeight, CAM_FORMAT_T Format)
{
    SXS_TRACE(TSTDOUT,"DRV_CAM: Power On, camOutSize %d ispw %d isph %d", CamOutSize, IspOutWidth, IspOutHeight);
    s_camOpened = FALSE;
    IsAtvOn=TRUE;
    atvd_Open();

    atvif_ISPSettingInit();
}

PRIVATE void camerap_PowerOff(void)
{
    // Turn off the Camera

    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};
    hal_CameraIrqSetMask(mask);
    // disable IFC
    if(gSensorInfo.cammode==CAM_MODE_IMG_CAPTURE)
    {
        hal_CameraStopXfer(1);
        gSensorInfo.cammode = CAM_MODE_IDLE;
    }
    IsAtvOn=FALSE;
    atvd_Close();
    // Disable Camera Interface Module
    hal_CameraClose();

    // Disable the LDO for the camera
    pmd_EnablePower(PMD_POWER_CAMERA,FALSE);
#ifdef TGT_CAMERA_PDN_CHANGE_WITH_ATV
    hal_CameraPowerDownBoth(FALSE);
#endif
}

/**
 * This must be called first before camer_PowerOn()
 */
PRIVATE void camerap_Reserve(CAM_IRQ_HANDLER_T FrameReady)
{
    gSensorInfo.camirqhandler = FrameReady;
    hal_CameraIrqSetHandler(camerap_InteruptHandler);
    gIfcChan = 0xff;
}

PRIVATE void camerap_Release(void)
{

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

PRIVATE void camerap_SetContrastL(UINT32 Contrast)
{

}

PRIVATE UINT32 camerap_Contrast(void)
{
    return(0);
}

PRIVATE void camerap_SetBrightnessL(UINT32 Brightness)
{

}


PRIVATE UINT32 camerap_Brightness(void)
{
    return(0);
}

// =============================================================================
// camd_SetFlashL
// -----------------------------------------------------------------------------
/// Configure the camera flash light.
///
/// @param flash Flash configuration to apply.
// =============================================================================
PRIVATE void camerap_SetFlashL(CAM_FLASH_T Flash)
{

}

PRIVATE CAM_FLASH_T camerap_Flash(void)
{
    return(CAM_FLASH_NONE);
}

PRIVATE void camerap_SetExposureL(CAM_EXPOSURE_T Exposure)
{

}

PRIVATE CAM_EXPOSURE_T camerap_Exposure(void)
{
    return(CAM_EXPOSURE_AUTO);
}

PRIVATE void camerap_SetWhiteBalanceL(CAM_WHITEBALANCE_T WhiteBalance)
{

}

PRIVATE CAM_WHITEBALANCE_T camerap_WhiteBalance(void)
{
    return(CAM_WB_AUTO);
}

PRIVATE VOID camerap_SetSpecialEffect(CAM_SPECIALEFFECT_T SpecialEffect)
{

}


PRIVATE CAM_SPECIALEFFECT_T camerap_SpecialEffect(VOID)
{
    return(CAM_SPE_NORMAL);

}

// NightMode : 1: NightModeEN; 0:NightModeDis
PRIVATE VOID camerap_SetNightMode(UINT32 NightMode)
{

}


PRIVATE UINT32 camerap_NightMode(VOID)
{
    return(FALSE);

}
// =============================================================================
// camera_StartViewFinderDirectL(some parameters)
// -----------------------------------------------------------------------------
/// Configure the camera to capture frames for viewfinder.  This should be
/// not the maximum resolution of the camera, but a resolution as close to
/// the LCD as possible.  The parameters for the screen should be passed here
/// and this function should automatically 'calculate' the best size based on
/// the possible output formatter configurations.
///
/// @param Some stuff to be defined
// =============================================================================

PRIVATE void camerap_StartViewFinderDirectL()
{

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

PRIVATE void camerap_PrepareImageCaptureL(CAM_FORMAT_T ImageFormat, UINT32 SizeIndex, CAM_IMAGE_T Buffer)
{
    //    SXS_TRACE(TSTDOUT,"DRV_CAM: PrepareCapture %x", Buffer);
    // ImageBuf must be allocated by the upper level and should be large enough for the
    // TODO: Configure the camera to the new image format and size
    // For now, just keep VGA RGB565
    //camerap_SendRgSeq(RG_VidFormatxxx,  (sizeof(RG_VidFormatxxx,) / sizeof(CAM_REG_T)));
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};
    hal_CameraIrqSetMask(mask);

    gSensorInfo.format = ImageFormat;
    gSensorInfo.npixels = SizeIndex;
    gSensorInfo.vidbuffer = Buffer;

    // TODO: Handle the error case.  This just asks hal_Camera to get the IFC Channel
    //    IfcChan = hal_CameraIfcPrepare();
    //    SXS_TRACE(TSTDOUT,"DRV_CAM: Got IFC Chan %d", IfcChan);

    // When the next interrupt arrives (end of frame), the handler will
    // check the cammode and start the IFC and switch to CAM_MODE_IMG_CAPTURE
    gSensorInfo.cammode = CAM_MODE_PREP_IMG_CAPTURE;
}

PRIVATE void camerap_CaptureImage()
{
    // The cammode should now be CAM_MODE_PREP_IMG_CAPTURE.  Can trigger the interrupt
    // to start the capture on the next frame (actually the end of the current frame)
    // Clear all the camera interrupts and enable only the end of frame
    // Stupid.  Only VSYNC interrupt is available in Mode=1
    //    SXS_TRACE(TSTDOUT,"DRV_CAM: Capture");
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};
    mask.fstart = 1;
    hal_CameraIrqSetMask(mask);

    // Enable Camera controller
    hal_CameraControllerEnable(TRUE);

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

}

PRIVATE BOOL camerap_VideoCaptureActive(void)
{
    return TRUE;
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

PRIVATE UINT8 camerap_GetId(VOID)
{
    return TRUE;
}

// ############
// Private
// ############


PRIVATE VOID CAMD_POSSIBLY_UNUSED camerap_DbgSendSequence(UINT8 SeqNum)
{

}

PRIVATE VOID camerap_InteruptHandler(HAL_CAMERA_IRQ_CAUSE_T cause)
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};

//    SXS_TRACE(TSTDOUT,"DRV_CAM: IRQ");
    switch (gSensorInfo.cammode)
    {
        case CAM_MODE_PREP_IMG_CAPTURE:
            // Configure IFC
            gIfcChan = hal_CameraStartXfer((gSensorInfo.npixels*2), (UINT8*)gSensorInfo.vidbuffer);
            // Reset interrupt
            mask.fstart= 1;
            //mask.fend = 1;
            hal_CameraIrqSetMask(mask);
            gSensorInfo.cammode = CAM_MODE_IMG_CAPTURE;
#ifdef _ATV_SPLIT_
            gSensorInfo.camirqhandler(0, gSensorInfo.currentbuf);
#endif
#if ANALOG_TV_SUPPORT_MCP_LOCK
            // check ATV status
            if (atvd_is_checking_status())
            {
                mmc_AnalogTvRealTimeProcess();
            }
#endif
            break;
        case CAM_MODE_IMG_CAPTURE:
            // TODO: When capture is done, should go back to viewfinder mode, or just shut down the camera?
            //SXS_TRACE(TSTDOUT,"DRV_CAM: End of Image Capture");
            hal_CameraIrqSetMask(mask);
            gSensorInfo.cammode = CAM_MODE_IDLE;

#if defined(ATV_SKIP_hal_Xfer_Data_Error)
            hal_CameraStopXfer(FALSE);
            gSensorInfo.camirqhandler(0, gSensorInfo.currentbuf);
#else
            if(hal_CameraStopXfer(FALSE)==XFER_SUCCESS)
            {
                //hal_CameraReset(TRUE, TRUE);
                gSensorInfo.camirqhandler(0, gSensorInfo.currentbuf);
            }
            else
            {
                gSensorInfo.camirqhandler(1, gSensorInfo.currentbuf);
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
    return TRUE;
}

PUBLIC BOOL atvif_InitForCameraInterface(CAM_DRV_T  * pCamDrv)
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
        return TRUE;
    }

    return FALSE;
}



