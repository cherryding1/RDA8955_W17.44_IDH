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

#define CAM_GC0307_ID       0x99

PRIVATE volatile UINT8 gIfcChan = 0xff;
PRIVATE BOOL s_camOpened = FALSE;
PRIVATE BOOL gIsCameraStartXfer=FALSE;

#ifdef I2C_BASED_ON_GPIO
#include "gpio_i2c.h"
#else
PRIVATE HAL_I2C_BUS_ID_T g_camdI2cBusId = HAL_I2C_BUS_ID_INVALID;
#endif

//----------------------------
// CAM_ISP_FUNC
//      -- 1: use crop
//      -- 0: use decimation
//-----------------------------
#define CAM_ISP_FUNC 0

// ############
// Private
// ############

extern UINT32 gCameraID;

// The camera will be powered on in a particular mode specified here
// Later, when the video capture or image capture is 'prepared', the frame buffer size
// must correspond to the size here unless the format is changed.
PRIVATE void camerap_PowerOn(CAM_SIZE_T CamOutSize, UINT16 IspOutWidth, UINT16 IspOutHeight, CAM_FORMAT_T Format)
{
    // Turn on any power required for camera
    // Enable the Camera Interface Module
    // Turn on the Camera
    // Reset & initialize the Camera
    HAL_CAMERA_CFG_T CamConfig = {0,};
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};
    UINT16 camOutWidth = 0, camOutHeight = 0;

    if (CamOutSize == CAM_NPIX_VGA)
    {
        camOutWidth = 640;
        camOutHeight = 480;
    }
    else if (CamOutSize == CAM_NPIX_QVGA)
    {
        camOutWidth = 320;
        camOutHeight = 240;
    }
    else if (CamOutSize == CAM_NPIX_QQVGA)
    {
        camOutWidth = 160;
        camOutHeight = 120;
    }
    else
    {
        SXS_TRACE(TSTDOUT,"DRV_CAM: Unsupported CAM Out Size");
        return;
    }
    if ((camOutWidth < IspOutWidth) || (camOutHeight < IspOutHeight))
    {
        SXS_TRACE(TSTDOUT,"DRV_CAM: ISP out is larger than Cam Out");
        return;
    }

    s_camOpened = FALSE;

#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_open();
#else
    g_camdI2cBusId = tgt_GetCamdConfig()->i2cBusId;
    hal_I2cOpen(g_camdI2cBusId);
#endif

    CamConfig.rstActiveH = FALSE;
    CamConfig.pdnActiveH = TRUE;
    CamConfig.dropFrame = FALSE;
    CamConfig.camClkDiv = 6; // 156MHz/6 = 26MHz
    CamConfig.endianess = NO_SWAP;//BYTE_SWAP;//
    CamConfig.camId = camera_GetCameraID();
    CamConfig.cropEnable = FALSE;
#if CAM_ISP_FUNC
    if ((camOutWidth > IspOutWidth) || (camOutHeight > IspOutHeight))
    {
        CamConfig.dstWinColStart = ((camOutWidth - IspOutWidth)/2)&0xFFF;
        CamConfig.dstWinColEnd = (CamConfig.dstWinColStart + IspOutWidth - 1)&0xFFF;
        CamConfig.dstWinRowStart = ((camOutHeight - IspOutHeight)/2)&0xFFF;
        CamConfig.dstWinRowEnd = (CamConfig.dstWinRowStart + IspOutHeight - 1)&0xFFF;
        CamConfig.cropEnable = TRUE;
    }
    CamConfig.colRatio= COL_RATIO_1_1;
    CamConfig.rowRatio= ROW_RATIO_1_1;
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

    // Enable Camera LDO
    pmd_EnablePower(PMD_POWER_CAMERA, TRUE);
    hal_CameraOpen(&CamConfig);
    s_camOpened = TRUE;

    hal_CameraIrqSetMask(mask);
    hal_CameraPowerDown(FALSE);
    if (gSensorInfo.snrrst)
    {
        // Toggle the reset bit
    }
    else
    {
        // Just bring the sensor out of reset
        // hal_CameraReset(TRUE, FALSE);
        hal_CameraReset(FALSE);
    }

    // Should use CamOutSize and Format to decide which sequence to use
    // CamOutSize and Format may be changed later but the camera and IFC need
    // to be stopped and the IFC and camera re-enabled in the correct order
    // Power On seq
    // After this, the camera will start sending data to the camera module.  The IFC will
    // need to be started at the correct time (typically after the end of frame interrupt
    // so the data will be properly synchronized in the buffer
    if (CamOutSize == CAM_NPIX_VGA)
    {
        camerap_SendRgSeq(RG_InitPowerOnVga,  (sizeof(RG_InitPowerOnVga) / sizeof(CAM_REG_T)));
#ifdef TGT_GALLITE_G800
        if(CamConfig.camId==1)
        {
            //for t800
            camerap_WriteOneReg( 0x0f , 0x92);
            camerap_WriteOneReg( 0x45 , 0x25);
            camerap_WriteOneReg( 0x47 , 0x24);
        }
        else
        {
            //for t800
            camerap_WriteOneReg( 0x0f , 0x82);
            camerap_WriteOneReg( 0x45 , 0x24);
            camerap_WriteOneReg( 0x47 , 0x20);
        }
#endif
    }
    else if(CamOutSize == CAM_NPIX_QVGA)
    {
        camerap_SendRgSeq(RG_InitPowerOnQvga,  (sizeof(RG_InitPowerOnQvga) / sizeof(CAM_REG_T)));
    }
    else
    {
        camerap_SendRgSeq(RG_InitPowerOnQqvga,  (sizeof(RG_InitPowerOnQqvga) / sizeof(CAM_REG_T)));
#ifdef TGT_GALLITE_G800
        if(CamConfig.camId==1)
        {
            //for t800
            camerap_WriteOneReg( 0x0f , 0x92);
            camerap_WriteOneReg( 0x45 , 0x25);
            camerap_WriteOneReg( 0x47 , 0x24);
        }
        else
        {
            //for t800
            camerap_WriteOneReg( 0x0f , 0x82);
            camerap_WriteOneReg( 0x45 , 0x90);
            camerap_WriteOneReg( 0x47 , 0x20);
        }
#endif
    }
    // Sensor Init seq
}

PRIVATE void camerap_PowerOff(void)
{
    // Turn off the Camera
#ifdef TGT_GALLITE_G800
    camerap_WriteOneReg( 0x43 , 0x00);
    camerap_WriteOneReg( 0x44 , 0x0a);
#endif
    if (s_camOpened == TRUE)
    {
        HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};
        hal_CameraIrqSetMask(mask);
        // disable IFC
        if (gIsCameraStartXfer == TRUE )
        {
            gIsCameraStartXfer=FALSE;
            hal_CameraStopXfer(1);
            gSensorInfo.cammode = CAM_MODE_IDLE;
        }
        // Disable Camera Interface Module
        hal_CameraClose();
    }
    // Disable I2C
#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_close();
#else
    hal_I2cClose(g_camdI2cBusId);
#endif

    // Disable the LDO for the camera
    pmd_EnablePower(PMD_POWER_CAMERA, FALSE);
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
    // 'De-initialize' camera
    // Release resource
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
    switch(Contrast)
    {
        case 1:
            camerap_WriteOneReg(0xa1, 0x60);
            break;
        case 2:
            camerap_WriteOneReg(0xa1, 0x50);
            break;
        case 3:
            camerap_WriteOneReg(0xa1, 0x40);
            break;
        case 4:
            camerap_WriteOneReg(0xa1, 0x38);
            break;
        case 5:
            camerap_WriteOneReg(0xa1, 0x30);
            break;
        default: break;
    }
}

PRIVATE UINT32 camerap_Contrast(void)
{
    return(0);
}

PRIVATE void camerap_SetBrightnessL(UINT32 Brightness)
{
    switch(Brightness)
    {
        case 1:
            camerap_WriteOneReg(0x7a, 0x00);
            camerap_WriteOneReg(0xd1, 0x50);
            break;
        case 2:   //-2
            camerap_WriteOneReg(0x7a, 0xe0);
            camerap_WriteOneReg(0xd1, 0x40);
            break;
        case 3: //-1
            camerap_WriteOneReg(0x7a, 0xf0);
            camerap_WriteOneReg(0xd1, 0x48);
            break;
        case 4:  //+1
            camerap_WriteOneReg(0x7a, 0x20);
            camerap_WriteOneReg(0xd1, 0x58);
            break;
        case 5:  //+2
            camerap_WriteOneReg(0x7a, 0x30);
            camerap_WriteOneReg(0xd1, 0x60);
            break;
        default : break;
    }
}

PRIVATE UINT32 camerap_Brightness(void)
{
    return(0);
}

PRIVATE void camerap_SetFlashL(CAM_FLASH_T Flash)
{
    SXS_TRACE(TSTDOUT,"wyk: camera_SetFlashL %x\n",Flash); //wyk reano r1113
    if(gCameraID == 1) return; //Only light on flash led when the back sensor is used.
    switch(Flash)
    {
        case CAM_FLASH_NONE:
            pmd_EnablePower(PMD_POWER_CAMERA_FLASH, FALSE);//FALSE  //wyk reano r1113
            break;

        case CAM_FLASH_FORCED:
            pmd_EnablePower(PMD_POWER_CAMERA_FLASH, TRUE);
            break;
        case CAM_FLASH_AUTO:
        case CAM_FLASH_FILLIN:
        case CAM_FLASH_REDEYEREDUCE:
        default :
            break;
    }
}

PRIVATE CAM_FLASH_T camerap_Flash(void)
{
    return(CAM_FLASH_NONE);
}

PRIVATE void camerap_SetExposureL(CAM_EXPOSURE_T Exposure)
{
    //Actually this is the same with camerap_SetBrightnessL
    switch(Exposure)
    {
        case CAM_EXPOSURE_AUTO:
            camerap_WriteOneReg(0x7a, 0x00);
            camerap_WriteOneReg(0xd1, 0x50);
            break;
        case CAM_EXPOSURE_NEGATIVE_2:   //-2
            camerap_WriteOneReg(0x7a, 0xe0);
            camerap_WriteOneReg(0xd1, 0x40);
            break;
        case CAM_EXPOSURE_NEGATIVE_1: //-1
            camerap_WriteOneReg(0x7a, 0xf0);
            camerap_WriteOneReg(0xd1, 0x48);
            break;
        case CAM_EXPOSURE_POSITIVE_1:  //+1
            camerap_WriteOneReg(0x7a, 0x20);
            camerap_WriteOneReg(0xd1, 0x58);
            break;
        case CAM_EXPOSURE_POSITIVE_2:  //+2
            camerap_WriteOneReg(0x7a, 0x30);
            camerap_WriteOneReg(0xd1, 0x60);
            break;
        default : break;
    }
}

PRIVATE CAM_EXPOSURE_T camerap_Exposure(void)
{
    return(CAM_EXPOSURE_AUTO);
}

PRIVATE void camerap_SetWhiteBalanceL(CAM_WHITEBALANCE_T WhiteBalance)
{
    UINT8 TempReg;
    TempReg = camerap_ReadReg(0x41);

    switch(WhiteBalance)
    {
        case CAM_WB_DAYLIGHT:   /*晴天太阳光*/
            camerap_WriteOneReg(0x41,TempReg&~0x04);   // Enable AWB
            camerap_WriteOneReg(0xc7,0x50);
            camerap_WriteOneReg(0xc8,0x45);
            camerap_WriteOneReg(0xc9,0x40);
            break;
        case CAM_WB_AUTO:  /*Auto */
            camerap_WriteOneReg(0xc7,0x4c); //for AWB can adjust back
            camerap_WriteOneReg(0xc8,0x40);
            camerap_WriteOneReg(0xc9,0x4a);
            camerap_WriteOneReg(0x41,TempReg|0x04);  // Enable AWB
            break;
        case CAM_WB_OFFICE:   /*日光灯*/
            camerap_WriteOneReg(0x41,TempReg&~0x04);   // Enable AWB
            camerap_WriteOneReg(0xc7,0x40);
            camerap_WriteOneReg(0xc8,0x42);
            camerap_WriteOneReg(0xc9,0x50);
            break;
        case CAM_WB_CLOUDY:  /*阴天*/
            camerap_WriteOneReg(0x41,TempReg&~0x04);   // Enable AWB
            camerap_WriteOneReg(0xc7,0x5a); //WB_manual_gain
            camerap_WriteOneReg(0xc8,0x42);
            camerap_WriteOneReg(0xc9,0x40);
            break;
        case CAM_WB_TUNGSTEN:  /*钨丝灯*/
            camerap_WriteOneReg(0x41,TempReg&~0x04);   // Enable AWB
            camerap_WriteOneReg(0xc7,0x40);
            camerap_WriteOneReg(0xc8,0x54);
            camerap_WriteOneReg(0xc9,0x70);
            break;
        default : break;
    }
}

PRIVATE CAM_WHITEBALANCE_T camerap_WhiteBalance(void)
{
    return(CAM_WB_AUTO);
}

PRIVATE VOID camerap_SetSpecialEffect(CAM_SPECIALEFFECT_T SpecialEffect)
{
    UINT8 TempReg;
    TempReg = camerap_ReadReg(0x47);
    switch(SpecialEffect)
    {

        case CAM_SPE_NORMAL :

            //if(sensor_night_mode)
            //camerap_WriteOneReg(0x41,0x0f);
            //else
            camerap_WriteOneReg(0x41,0x2f);

            camerap_WriteOneReg(0x40,0x7e);
            camerap_WriteOneReg(0x42,0x10);
            //camerap_WriteOneReg(0x47,0x2c);
            camerap_WriteOneReg(0x47,TempReg & 0xef);
            camerap_WriteOneReg(0x48,0xc3);
            camerap_WriteOneReg(0x8a,0x50);//60
            camerap_WriteOneReg(0x8b,0x50);
            camerap_WriteOneReg(0x8c,0x07);
            camerap_WriteOneReg(0x50,0x0c);
            camerap_WriteOneReg(0x77,0x80);
            camerap_WriteOneReg(0xa1,0x40);
            camerap_WriteOneReg(0x7a,0x00);
            camerap_WriteOneReg(0x78,0x00);
            camerap_WriteOneReg(0x79,0x00);
            camerap_WriteOneReg(0x7b,0x40);
            camerap_WriteOneReg(0x7c,0x00);
            break;
        case CAM_SPE_ANTIQUE    :
            camerap_WriteOneReg(0x41,0x2f);
            camerap_WriteOneReg(0x40,0x7e);
            camerap_WriteOneReg(0x42,0x10);
            camerap_WriteOneReg(0x47,TempReg | 0x10);
            camerap_WriteOneReg(0x48,0xc3);
            camerap_WriteOneReg(0x8a,0x60);
            camerap_WriteOneReg(0x8b,0x60);
            camerap_WriteOneReg(0x8c,0x07);
            camerap_WriteOneReg(0x50,0x0c);
            camerap_WriteOneReg(0x77,0x80);
            camerap_WriteOneReg(0xa1,0x40);
            camerap_WriteOneReg(0x7a,0x00);
            camerap_WriteOneReg(0x78,0xc0);
            camerap_WriteOneReg(0x79,0x20);
            camerap_WriteOneReg(0x7b,0x40);
            camerap_WriteOneReg(0x7c,0x00);
            break;
        case CAM_SPE_REDISH :
            //Not support
            break;
        case CAM_SPE_GREENISH:
            //if(sensor_night_mode)
            //    camerap_WriteOneReg(0x41,0x0f);
            //else
            camerap_WriteOneReg(0x41,0x2f);
            camerap_WriteOneReg(0x40,0x7e);
            camerap_WriteOneReg(0x42,0x10);
            camerap_WriteOneReg(0x47,TempReg| 0x10);
            camerap_WriteOneReg(0x48,0xc3);
            camerap_WriteOneReg(0x8a,0x60);
            camerap_WriteOneReg(0x8b,0x60);
            camerap_WriteOneReg(0x8c,0x07);
            camerap_WriteOneReg(0x50,0x0c);
            camerap_WriteOneReg(0x77,0x80);
            camerap_WriteOneReg(0xa1,0x40);
            camerap_WriteOneReg(0x7a,0x00);
            camerap_WriteOneReg(0x78,0xc0);
            camerap_WriteOneReg(0x79,0xc0);
            camerap_WriteOneReg(0x7b,0x40);
            camerap_WriteOneReg(0x7c,0x00);
            break;
        case CAM_SPE_BLUEISH    :
            camerap_WriteOneReg(0x41,0x00);
            camerap_WriteOneReg(0x40,0x3e);
            camerap_WriteOneReg(0x42,0x14);
            camerap_WriteOneReg(0x47,TempReg | 0x10);
            camerap_WriteOneReg(0x48,0xc2);
            camerap_WriteOneReg(0x8d,0xff);
            camerap_WriteOneReg(0x8a,0xf0);
            camerap_WriteOneReg(0x8b,0xf0);
            camerap_WriteOneReg(0x8c,0x00);
            camerap_WriteOneReg(0x50,0x08);
            camerap_WriteOneReg(0xdb,0x50);
            camerap_WriteOneReg(0xb0,0xff);
            camerap_WriteOneReg(0x77,0xab);
            camerap_WriteOneReg(0xa1,0xff);
            camerap_WriteOneReg(0x7a,0x7f);
            camerap_WriteOneReg(0x78,0x70);
            camerap_WriteOneReg(0x79,0x00);
            camerap_WriteOneReg(0x7b,0x40);
            camerap_WriteOneReg(0x7c,0x00);
            break;
        case CAM_SPE_BLACKWHITE :
            camerap_WriteOneReg(0x41,0x2f);
            camerap_WriteOneReg(0x40,0x7e);
            camerap_WriteOneReg(0x42,0x10);
            camerap_WriteOneReg(0x47,TempReg | 0x10);
            camerap_WriteOneReg(0x48,0xc3);
            camerap_WriteOneReg(0x8a,0x60);
            camerap_WriteOneReg(0x8b,0x60);
            camerap_WriteOneReg(0x8c,0x07);
            camerap_WriteOneReg(0x50,0x0c);
            camerap_WriteOneReg(0x77,0x80);
            camerap_WriteOneReg(0xa1,0x40);
            camerap_WriteOneReg(0x7a,0x00);
            camerap_WriteOneReg(0x78,0x00);
            camerap_WriteOneReg(0x79,0x00);
            camerap_WriteOneReg(0x7b,0x40);
            camerap_WriteOneReg(0x7c,0x00);
            break;
        case CAM_SPE_NEGATIVE:
            camerap_WriteOneReg(0x41,0x6f);
            //camerap_WriteOneReg(0x41,0x6f);//[6]
            camerap_WriteOneReg(0x40,0x7e);
            camerap_WriteOneReg(0x42,0x10);
            //camerap_WriteOneReg(0x47,0x2c);
            camerap_WriteOneReg(0x47,TempReg & 0xef);
            camerap_WriteOneReg(0x48,0xc3);
            camerap_WriteOneReg(0x8a,0x60);
            camerap_WriteOneReg(0x8b,0x60);
            camerap_WriteOneReg(0x8c,0x07);
            camerap_WriteOneReg(0x50,0x0c);
            camerap_WriteOneReg(0x77,0x80);
            camerap_WriteOneReg(0xa1,0x40);
            camerap_WriteOneReg(0x7a,0x00);
            camerap_WriteOneReg(0x78,0x00);
            camerap_WriteOneReg(0x79,0x00);
            camerap_WriteOneReg(0x7b,0x40);
            camerap_WriteOneReg(0x7c,0x00);
            break;
        case CAM_SPE_BLACKWHITE_NEGATIVE:
            camerap_WriteOneReg(0x41,0x6f);
            //camerap_WriteOneReg(0x41,0x6f);
            camerap_WriteOneReg(0x40,0x7e);
            camerap_WriteOneReg(0x42,0x10);
            camerap_WriteOneReg(0x47,TempReg | 0x10);
            camerap_WriteOneReg(0x48,0xc3);
            camerap_WriteOneReg(0x8a,0x60);
            camerap_WriteOneReg(0x8b,0x60);
            camerap_WriteOneReg(0x8c,0x07);
            camerap_WriteOneReg(0x50,0x0c);
            camerap_WriteOneReg(0x77,0x80);
            camerap_WriteOneReg(0xa1,0x40);
            camerap_WriteOneReg(0x7a,0x00);
            camerap_WriteOneReg(0x78,0x00);
            camerap_WriteOneReg(0x79,0x00);
            camerap_WriteOneReg(0x7b,0x40);
            camerap_WriteOneReg(0x7c,0x00);
            break;

        default: break;

    }
}

PRIVATE CAM_SPECIALEFFECT_T camerap_SpecialEffect(VOID)
{
    return(CAM_SPE_NORMAL);

}

// NightMode : 1: NightModeEN; 0:NightModeDis
PRIVATE VOID camerap_SetNightMode(UINT32 NightMode)
{
#if 0
    INT8 readregval;
    readregval=camerap_ReadReg(0x41);

    if(NightMode==0)
    {
        camerap_WriteOneReg(    0xdd  ,0x22);  //0x12
        camerap_WriteOneReg(    0x41  ,readregval| 0x20);
        camerap_WriteOneReg(    0x21  ,0xc0);
    }
    else
    {
        camerap_WriteOneReg(    0xdd  ,0x32);
        camerap_WriteOneReg(    0x41  ,readregval & ~0x20);
        camerap_WriteOneReg(    0xb0  ,0x10);
        camerap_WriteOneReg(    0x21  ,0xf0);
    }
#endif
}

PRIVATE UINT32 camerap_NightMode(VOID)
{
    return(FALSE);

}
// =============================================================================
// camerap_StartViewFinderDirectL(some parameters)
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

PRIVATE void camerap_PrepareImageCaptureL(CAM_FORMAT_T ImageFormat, UINT32 SizeIndex, CAM_IMAGE_T Buffer)
{
    gSensorInfo.format = ImageFormat;
    gSensorInfo.npixels = SizeIndex;
    gSensorInfo.vidbuffer = Buffer;

#if 1
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};
    hal_CameraIrqSetMask(mask);
    if (gIsCameraStartXfer)
    {
        gIsCameraStartXfer = FALSE;
        hal_CameraStopXfer(FALSE);
    }

    gSensorInfo.cammode = CAM_MODE_VIEWFINDER;
    mask.fstart = 1;
    hal_CameraIrqSetMask(mask);
    // Enable Camera controller
    hal_CameraControllerEnable(TRUE);
#else
    gSensorInfo.cammode = CAM_MODE_PREP_IMG_CAPTURE;
#endif
}

PRIVATE void camerap_CaptureImage()
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};
#if 1
    hal_CameraIrqSetMask(mask);
    if (gIsCameraStartXfer)
    {
        gIsCameraStartXfer = FALSE;
        hal_CameraStopXfer(FALSE);
    }
    gSensorInfo.cammode = CAM_MODE_PREP_IMG_CAPTURE;
    mask.fstart = 1;
    hal_CameraIrqSetMask(mask);
#else
    mask.fend = 1;
    hal_CameraIrqSetMask(mask);

    // Enable Camera controller
    hal_CameraControllerEnable(TRUE);
#endif
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
#if 0
    switch(FrameRate)
    {
        case 12:
            camerap_WriteOneReg(0x11,0x1);
            break;
        case 11:
            camerap_WriteOneReg(0x11, 0x2);
            break;
        case 10:
            camerap_WriteOneReg(0x11, 0x3);
            break;
        case 9:
            camerap_WriteOneReg(0x11, 0x4);
            break;
        case 8:
            camerap_WriteOneReg(0x11, 0x5);
            break;
        case 7:
            camerap_WriteOneReg(0x11, 0x6);
            break;
        case 6:
            camerap_WriteOneReg(0x11, 0x7);
            break;
        case 5:
            camerap_WriteOneReg(0x11, 0x8);
            break;
        case 4:
            camerap_WriteOneReg(0x11, 0x9);
            break;
        case 3:
            camerap_WriteOneReg(0x11, 0x0a);
            break;
        case 2:
            camerap_WriteOneReg(0x11, 0x0b);
            break;
        case 1:
            camerap_WriteOneReg(0x11, 0x0f);
            break;
        default: break;
    }
#endif
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
    return(camerap_ReadReg(0x00));
}

// ############
// Private
// ############

PRIVATE VOID  camerap_WriteOneReg( UINT8 Addr, UINT8 Data)
{
#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_write_data(gSensorInfo.i2caddress,&Addr,1, &Data, 1);
#else
    hal_I2cSendRawByte(g_camdI2cBusId, ((gSensorInfo.i2caddress<<1) & 0xfe), PHASE3_WR_PH1);
    hal_I2cSendRawByte(g_camdI2cBusId, Addr, PHASE3_WR_PH2);
    hal_I2cSendRawByte(g_camdI2cBusId, Data, PHASE3_WR_PH3);
#endif

}

PRIVATE VOID camerap_Delay(UINT16 Wait_mS)
{
    UINT32 Ticks_16K;

    // Here is the delay function used by the camera driver.  It can be changed to use a non-blocking
    // wait if necessary, but for now, just use hal_TimDelay
    Ticks_16K = (16348 * Wait_mS)/1000;
    hal_TimDelay(Ticks_16K);
}

PRIVATE VOID CAMD_POSSIBLY_UNUSED camerap_DbgSendSequence(UINT8 SeqNum)
{
    UINT16 RegCount;
    UINT16 Len;
    CAM_REG_T *RegVal;

    switch (SeqNum)
    {
        case (1) :
            Len = sizeof(RG_InitPowerOnVga) / sizeof(CAM_REG_T);
            RegVal =  (CAM_REG_T*)&RG_InitPowerOnVga;
            break;
        case (2) :
            Len = sizeof(RG_VidFormatVga) / sizeof(CAM_REG_T);
            RegVal =  (CAM_REG_T*)&RG_VidFormatVga;
            break;
        case (3) :
            Len = sizeof(RG_VidFormatQvga) / sizeof(CAM_REG_T);
            RegVal =  (CAM_REG_T*)&RG_VidFormatQvga;
            break;
        case (4) :
            Len = sizeof(RG_VidFormatQqvga) / sizeof(CAM_REG_T);
            RegVal =  (CAM_REG_T*)&RG_VidFormatQqvga;
            break;
        default :
            Len = 0;
            RegVal = NULL;
            break;
    }
    for (RegCount = 0; RegCount < Len; ++RegCount)
    {
        camerap_WriteReg(RegVal[RegCount]);
    }
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
    gpio_i2c_write_data(gSensorInfo.i2caddress,(unsigned char * )&RegWrite.Addr,1, &RegWrite.Data, 1);
#else
    hal_I2cSendRawByte(g_camdI2cBusId, ((gSensorInfo.i2caddress<<1) & 0xfe), PHASE3_WR_PH1);
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
    gpio_i2c_write_data(gSensorInfo.i2caddress,&Addr,1, (unsigned char *)&data, 0);
    return(gpio_i2c_read_data(gSensorInfo.i2caddress,&Addr,0, data, 1));
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

#if 1
PRIVATE VOID camerap_InteruptHandler(HAL_CAMERA_IRQ_CAUSE_T cause)
{
    //static uint8 captureCount=0xFF;
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};

    SXS_TRACE(TSTDOUT, "[MMC_CAMERA]camerap_InteruptHandler tick: %d Cammode: %d",hal_TimGetUpTime(),gSensorInfo.cammode);

    switch (gSensorInfo.cammode)
    {
        case CAM_MODE_VIEWFINDER:
            if (cause.fstart)
            {
                gIfcChan = hal_CameraStartXfer((gSensorInfo.npixels*2), (UINT8*)gSensorInfo.vidbuffer);
                gIsCameraStartXfer = TRUE;
                mask.fend = 1;
                hal_CameraIrqSetMask(mask);
            }
            else if (cause.fend)
            {
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
                gIfcChan = hal_CameraStartXfer((gSensorInfo.npixels*2), (UINT8*)gSensorInfo.vidbuffer);
                gIsCameraStartXfer = TRUE;
                gSensorInfo.cammode = CAM_MODE_IMG_CAPTURE;
                mask.fend = 1;
                hal_CameraIrqSetMask(mask);
            }
            else if (cause.fend)
            {
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
            gSensorInfo.camirqhandler(255, 255);
            break;
    }
}
#else
PRIVATE VOID camerap_InteruptHandler(HAL_CAMERA_IRQ_CAUSE_T cause)
{
    HAL_CAMERA_IRQ_CAUSE_T mask = {0,0,0,0};
    hal_HstSendEvent(0x01);
    hal_HstSendEvent(hal_TimGetUpTime());

    switch (gSensorInfo.cammode)
    {
        case CAM_MODE_PREP_IMG_CAPTURE:
            gIfcChan = hal_CameraStartXfer((gSensorInfo.npixels*2), (UINT8*)gSensorInfo.vidbuffer);
            //mask.vsync = 1;
            mask.fend = 1;
            hal_CameraIrqSetMask(mask);
            gSensorInfo.cammode = CAM_MODE_IMG_CAPTURE;
            gIsCameraStartXfer = TRUE;
            break;
        case CAM_MODE_IMG_CAPTURE:
            hal_CameraIrqSetMask(mask);
            gSensorInfo.cammode = CAM_MODE_IDLE;
            if(hal_CameraStopXfer(FALSE)==XFER_SUCCESS)
            {
                gSensorInfo.camirqhandler(0, gSensorInfo.currentbuf);
            }
            else
            {
                gSensorInfo.camirqhandler(1, gSensorInfo.currentbuf);
            }
            gIsCameraStartXfer = FALSE;
            break;
        default:
            gSensorInfo.camirqhandler(255, 255);
            break;
    }
}
#endif

PRIVATE char* camerap_GetStringId(VOID)
{
    static char gc0307_id_str[] = "gc0307\n";
    return gc0307_id_str;
}
// ============================================================================
// camerap_CheckProductId
// ----------------------------------------------------------------------------
/// read sensor id,compare with define sensor id
/// @return #TRUE, #FALSE
// ============================================================================
PRIVATE BOOL camerap_CheckProductId()
{
    UINT8 cameraID=0;

    camerap_Reserve(NULL);

#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_open();
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

    HAL_CAMERA_CFG_T CamConfig = {0,};
    CamConfig.rstActiveH = FALSE;
    CamConfig.pdnActiveH = TRUE;
    CamConfig.dropFrame = FALSE;
    CamConfig.camClkDiv = 6; // 156MHz/6 = 26MHz
    CamConfig.endianess = NO_SWAP;//BYTE_SWAP;//
    CamConfig.colRatio= COL_RATIO_1_1;
    CamConfig.rowRatio= ROW_RATIO_1_1;
    CamConfig.camId = camera_GetCameraID();
    hal_CameraOpen(&CamConfig);

    hal_CameraReset(TRUE);      // set reset to low
    // Enable Camera LDO
    pmd_EnablePower(PMD_POWER_CAMERA,TRUE);
    hal_TimDelay(819);          // delay 5ms for stable reset low signal

    hal_CameraPowerDown(FALSE);
    if (gSensorInfo.snrrst)
    {
    }
    else
    {
        hal_CameraReset(FALSE);
    }

    //before read,must delay for sensor
    camerap_Delay(20);
    cameraID=camerap_GetId();

    // Disable Camera Interface Module
    hal_CameraClose();
    // Disable I2C

#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_close();
#else
    hal_I2cClose(g_camdI2cBusId);
#endif

    // Disable the LDO for the camera
    pmd_EnablePower(PMD_POWER_CAMERA,FALSE);

    SXS_TRACE(TSTDOUT,"gc0307(0x%02x): read id is 0x%02x", CAM_GC0307_ID, cameraID);


    if(cameraID == CAM_GC0307_ID)
    {
        hal_HstSendEvent(0xaacc0000|CAM_GC0307_ID);
        return TRUE;
    }
    else
    {
        return FALSE;
    }



}

PRIVATE void camerap_GetImageInfo(CAM_SIZE_T *pcamout,UINT16 *pwidth, UINT16 *pheight)
{
    if(NULL == pcamout ||NULL == pwidth || NULL == pheight)
    {
        return;
    }

    *pcamout = CAM_NPIX_VGA;
    *pwidth = 640;
    *pheight = 480;
}


// ############
// Public
// ############


PUBLIC BOOL camera_gc0307_RegInit(CAM_DRV_T  * pCamDrv)
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
        pCamDrv->camera_GetStringId=camerap_GetStringId;
        pCamDrv->camera_GetImageInfo = camerap_GetImageInfo;

        return TRUE;
    }

    return FALSE;
}


