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
#include "camera_m.h"
#include "camera_driver.h"
#include "tgt_m.h"

#define HAL_ASSERT(BOOL, format, ...)                   \
    if (!(BOOL)) {            \
        hal_DbgAssert(format, ##__VA_ARGS__);                             \
    }

PRIVATE CAM_DRV_T  g_camera_register_table;
PRIVATE CAM_DRV_T  *pCamDrv;

BOOL (*g_CamRegInit[])(CAM_DRV_T *) = { CAM_REGINIT_LIST  };
#define CAM_REGINIT_QTY   sizeof(g_CamRegInit) /sizeof(UINT32)

// Set default CameraID to 0
PUBLIC UINT32 gCameraID = 0;

PRIVATE VOID  camera_initialize_register_table(CAM_DRV_T  *pCam)
{
    pCam->camera_PowerOn=NULL;
    pCam->camera_PowerOff=NULL;
    pCam->camera_Reserve=NULL;
    pCam->camera_Release=NULL;
    pCam->camera_PrepareImageCaptureL=NULL;
    pCam->camera_CaptureImage=NULL;
    pCam->camera_SetZoomFactorL=NULL;
    pCam->camera_ZoomFactor=NULL;
    pCam->camera_SetDigitalZoomFactorL=NULL;
    pCam->camera_DigitalZoomFactor=NULL;
    pCam->camera_SetContrastL=NULL;
    pCam->camera_Contrast=NULL;
    pCam->camera_SetBrightnessL=NULL;
    pCam->camera_Brightness=NULL;
    pCam->camera_SetFlashL=NULL;
    pCam->camera_Flash=NULL;
    pCam->camera_SetExposureL=NULL;
    pCam->camera_Exposure=NULL;
    pCam->camera_SetBanding=NULL;
    pCam->camera_SetWhiteBalanceL=NULL;
    pCam->camera_WhiteBalance=NULL;
    pCam->camera_SetSpecialEffect=NULL;
    pCam->camera_SpecialEffect=NULL;
    pCam->camera_SetNightMode=NULL;
    pCam->camera_NightMode=NULL;
    pCam->camera_StartViewFinderDirectL=NULL;
    pCam->camera_StopViewFinder=NULL;
    pCam->camera_ViewFinderActive=NULL;
    pCam->camera_SetViewFinderMirror=NULL;
    pCam->camera_ViewFinderMirror=NULL;
    pCam->camera_CancelCaptureImage=NULL;
    pCam->camera_EnumerateCaptureSizes=NULL;
    pCam->camera_PrepareVideoCaptureL=NULL;
    pCam->camera_StartVideoCapture=NULL;
    pCam->camera_StopVideoCapture=NULL;
    pCam->camera_VideoCaptureActive=NULL;
    pCam->camera_EnumerateVideoFrameSizes=NULL;
    pCam->camera_EnumerateVideoFrameRates=NULL;
    pCam->camera_GetFrameSize=NULL;
    pCam->camera_SetFrameRate=NULL;
    pCam->camera_FrameRate=NULL;
    pCam->camera_BuffersInUse=NULL;
    pCam->camera_FramesPerBuffer=NULL;
    pCam->camera_GetId=NULL;
    pCam->camera_GetStringId=NULL;
}

PUBLIC VOID camera_SaveCameraHwId(UINT8 idx, UINT8 camHwId)
{
    TGT_MODULE_ID_T moduleId;

    tgt_GetModuleId(&moduleId);
    moduleId.cam_id[idx&0x1] = camHwId;
    tgt_SetModuleId(&moduleId);
}

PRIVATE BOOL camera_load_drv(VOID)
{
    UINT8 i;
    TGT_MODULE_ID_T moduleId;

    if (gCameraID > 1)
        return FALSE;

    pCamDrv=&g_camera_register_table;
    camera_initialize_register_table(pCamDrv);
#ifdef __MMI_ANALOGTV__
    if (atvif_GetCurModule() == 1)
    {
        atvif_InitForCameraInterface(pCamDrv);
        return TRUE;
    }
#endif

    tgt_GetModuleId(&moduleId);
    if (moduleId.cam_id[gCameraID] < CAM_REGINIT_QTY)
    {
        // Valid ID
        if(g_CamRegInit[moduleId.cam_id[gCameraID]](pCamDrv))
        {
            return TRUE;
        }
    }

    for (i = 0; i < CAM_REGINIT_QTY;  i++)
    {
        if(g_CamRegInit[i](pCamDrv))
        {
            moduleId.cam_id[gCameraID] = i;
            tgt_SetModuleId(&moduleId);
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * This must be called first before camer_PowerOn()
 */
PUBLIC VOID camera_Reserve(CAM_IRQ_HANDLER_T FrameReady)
{
    if(camera_load_drv())
    {
        if((pCamDrv->camera_Reserve!=NULL))
            return(pCamDrv->camera_Reserve(FrameReady));
    }
    else
    {
        SXS_TRACE(TSTDOUT,"camera not foud" );
        return;
    }

    return;
}

// In a dual Cam config, there are Cam0 and Cam1.
// Though Cam1 is configed in target, but it may NOT present in final products.
// In other words, one lod with 2 Cam config could fit for both one and two cam phones.
// camera_Cam1_Present() is used to detect whether Cam1 is mounted or not.
// TRUE is returned when Cam1 is on board, and manus about Cam1 are enabled.
// This function will check Cam1 for only 1 time to save camera power-on time.
PUBLIC BOOL camera_Cam1_Present(VOID)
{

    static BOOL checked = FALSE, present = FALSE;

    if (FALSE == checked) // Check only once
    {
        gCameraID = 1; // Set cam ID to Cam1
        if (camera_load_drv())
        {
            present = TRUE;
        }
        checked = TRUE;
    }

    return present; // Return result of the first time check

}

PUBLIC VOID camera_PowerOn(CAM_SIZE_T CamOutSize, UINT16 IspOutWidth, UINT16 IspOutHeight, CAM_FORMAT_T Format)
{

    if((pCamDrv->camera_PowerOn!=NULL))
        return(pCamDrv->camera_PowerOn(CamOutSize,IspOutWidth,IspOutHeight,Format));

}

PUBLIC VOID camera_PowerOff(VOID)
{
    if((pCamDrv->camera_PowerOff!=NULL))
        return(pCamDrv->camera_PowerOff());


}


PUBLIC VOID camera_Release(VOID)
{
    if((pCamDrv->camera_Release!=NULL))
        return(pCamDrv->camera_Release());

}


PUBLIC VOID camera_SetZoomFactorL(UINT32 ZoomFactor)
{
    if((pCamDrv->camera_SetZoomFactorL!=NULL))
        return(pCamDrv->camera_SetZoomFactorL(ZoomFactor));

}

PUBLIC UINT32 camera_ZoomFactor(VOID)
{
    if((pCamDrv->camera_SetZoomFactorL!=NULL))
        return(pCamDrv->camera_ZoomFactor());

    return 0;
}

PUBLIC VOID camera_SetDigitalZoomFactorL(UINT32 DigZoomFactor)
{
    if((pCamDrv->camera_SetDigitalZoomFactorL!=NULL))
        return(pCamDrv->camera_SetDigitalZoomFactorL(DigZoomFactor));

}

PUBLIC UINT32 camera_DigitalZoomFactor(VOID)
{
    if((pCamDrv->camera_DigitalZoomFactor!=NULL))
        return(pCamDrv->camera_DigitalZoomFactor());

    return 0;
}

PUBLIC VOID camera_SetContrastL(UINT32 Contrast)
{
    if(pCamDrv->camera_SetContrastL!=NULL)
        return(pCamDrv->camera_SetContrastL(Contrast));

}

PUBLIC UINT32 camera_Contrast(VOID)
{
    if((pCamDrv->camera_Contrast!=NULL))
        return(pCamDrv->camera_Contrast());

    return 0;
}

PUBLIC VOID camera_SetBrightnessL(UINT32 Brightness)
{
    if((pCamDrv->camera_SetBrightnessL!=NULL))
        return(pCamDrv->camera_SetBrightnessL(Brightness));

}

PUBLIC UINT32 camera_Brightness(VOID)
{
    if((pCamDrv->camera_Brightness!=NULL))
        return(pCamDrv->camera_Brightness());

    return 0;
}

PUBLIC VOID  camera_SetFlashL(CAM_FLASH_T Flash)
{

    if((pCamDrv->camera_SetFlashL!=NULL))
        return(pCamDrv->camera_SetFlashL(Flash));

}

PUBLIC CAM_FLASH_T camera_Flash(VOID)
{
    if((pCamDrv->camera_Flash!=NULL))
        return(pCamDrv->camera_Flash());

    return 0;

}

PUBLIC VOID camera_SetExposureL(CAM_EXPOSURE_T Exposure)
{
    if((pCamDrv->camera_SetExposureL!=NULL))
        return(pCamDrv->camera_SetExposureL(Exposure));


}

PUBLIC CAM_EXPOSURE_T camera_Exposure(VOID)
{
    if((pCamDrv->camera_Exposure!=NULL))
        return(pCamDrv->camera_Exposure());
    return 0;
}

PUBLIC VOID camera_SetBanding(UINT32 Banding)
{
    if((pCamDrv->camera_SetBanding!=NULL))
        return(pCamDrv->camera_SetBanding(Banding));
}

PUBLIC VOID camera_SetWhiteBalanceL(CAM_WHITEBALANCE_T WhiteBalance)
{
    if((pCamDrv->camera_SetWhiteBalanceL!=NULL))
        return(pCamDrv->camera_SetWhiteBalanceL(WhiteBalance));


}

PUBLIC CAM_WHITEBALANCE_T camera_WhiteBalance(VOID)
{
    if((pCamDrv->camera_WhiteBalance!=NULL))
        return(pCamDrv->camera_WhiteBalance());
    return 0;
}

PUBLIC VOID camera_SetSpecialEffect(CAM_SPECIALEFFECT_T SpecialEffect)
{
    if((pCamDrv->camera_SetSpecialEffect!=NULL))
        return(pCamDrv->camera_SetSpecialEffect(SpecialEffect));


}

PUBLIC CAM_SPECIALEFFECT_T camera_SpecialEffect(VOID)
{
    if((pCamDrv->camera_SpecialEffect!=NULL))
        return(pCamDrv->camera_SpecialEffect());

    return 0;
}

// NightMode : 1: NightModeEN; 0:NightModeDis
PUBLIC VOID camera_SetNightMode(UINT32 NightMode)
{
    if((pCamDrv->camera_SetNightMode!=NULL))
        return(pCamDrv->camera_SetNightMode(NightMode));

}

PUBLIC UINT32 camera_NightMode(VOID)
{
    if((pCamDrv->camera_NightMode!=NULL))
        return(pCamDrv->camera_NightMode());
    return 0;
}

PUBLIC VOID camera_StartViewFinderDirectL()
{

    if((pCamDrv->camera_StartViewFinderDirectL!=NULL))
        return(pCamDrv->camera_StartViewFinderDirectL());

}

PUBLIC VOID camera_StopViewFinder(VOID)
{
    if((pCamDrv->camera_StopViewFinder!=NULL))
        return(pCamDrv->camera_StopViewFinder());

}


PUBLIC BOOL camera_ViewFinderActive(VOID)
{
    if((pCamDrv->camera_ViewFinderActive!=NULL))
        return(pCamDrv->camera_ViewFinderActive());

    return 0;
}

PUBLIC VOID camera_SetViewFinderMirror(BOOL Mirror)
{
    if((pCamDrv->camera_SetViewFinderMirror!=NULL))
        return(pCamDrv->camera_SetViewFinderMirror(Mirror));

}

PUBLIC BOOL camera_ViewFinderMirror(VOID)
{
    if((pCamDrv->camera_ViewFinderMirror!=NULL))
        return(pCamDrv->camera_ViewFinderMirror());

    return 0;
}

PUBLIC VOID camera_PrepareImageCaptureL(CAM_FORMAT_T ImageFormat, UINT32 SizeIndex, CAM_IMAGE_T Buffer)
{
    if((pCamDrv->camera_PrepareImageCaptureL != NULL))
        return(pCamDrv->camera_PrepareImageCaptureL(ImageFormat, SizeIndex, Buffer));
}

PUBLIC VOID camera_CaptureImage()
{
    if((pCamDrv->camera_CaptureImage!=NULL))
        return(pCamDrv->camera_CaptureImage());

}

PUBLIC VOID camera_CancelCaptureImage(VOID)
{
    if((pCamDrv->camera_CancelCaptureImage!=NULL))
        return(pCamDrv->camera_CancelCaptureImage());

}

PUBLIC VOID camera_EnumerateCaptureSizes(CAM_SIZE_T *Size,UINT32 SizeIndex, CAM_FORMAT_T Format)
{
    if((pCamDrv->camera_EnumerateCaptureSizes!=NULL))
        return(pCamDrv->camera_EnumerateCaptureSizes(Size,SizeIndex,Format));

}

PUBLIC VOID camera_PrepareVideoCaptureL(CAM_FORMAT_T Format, UINT32 SizeIndex, UINT32 RateIndex, UINT32 BuffersToUse, UINT32 FramesPerBuffer)
{
    if((pCamDrv->camera_PrepareVideoCaptureL!=NULL))
        return(pCamDrv->camera_PrepareVideoCaptureL(Format,SizeIndex,RateIndex,BuffersToUse,BuffersToUse));

}

PUBLIC VOID camera_StartVideoCapture(VOID)
{
    if((pCamDrv->camera_StartVideoCapture!=NULL))
        return(pCamDrv->camera_StartVideoCapture());

}

PUBLIC VOID camera_StopVideoCapture(VOID)
{
    if((pCamDrv->camera_StopVideoCapture!=NULL))
        return(pCamDrv->camera_StopVideoCapture());

}

PUBLIC BOOL camera_VideoCaptureActive(VOID)
{
    if(pCamDrv->camera_VideoCaptureActive!=NULL)
        return(pCamDrv->camera_VideoCaptureActive());

    return 0;
}

PUBLIC VOID camera_EnumerateVideoFrameSizes(  CAM_SIZE_T Size, UINT32 SizeIndex, CAM_FORMAT_T Format)
{
    if((pCamDrv->camera_EnumerateVideoFrameSizes!=NULL))
        return(pCamDrv->camera_EnumerateVideoFrameSizes(Size,SizeIndex,Format));

}

PUBLIC VOID camera_EnumerateVideoFrameRates()
{
    if((pCamDrv->camera_EnumerateVideoFrameRates!=NULL))
        return(pCamDrv->camera_EnumerateVideoFrameRates());

}

PUBLIC VOID camera_GetFrameSize(CAM_SIZE_T Size)
{
    if((pCamDrv->camera_GetFrameSize!=NULL))
        return(pCamDrv->camera_GetFrameSize(Size));

}

PUBLIC VOID camera_SetFrameRate(UINT32 FrameRate)
{
    if((pCamDrv->camera_SetFrameRate!=NULL))
        return(pCamDrv->camera_SetFrameRate(FrameRate));

}

PUBLIC UINT32 camera_FrameRate(VOID)
{
    if((pCamDrv->camera_FrameRate!=NULL))
        return(pCamDrv->camera_FrameRate());
    return 0;
}

PUBLIC UINT32 camera_BuffersInUse(VOID)
{
    if(pCamDrv->camera_BuffersInUse!=NULL)
        return(pCamDrv->camera_BuffersInUse());

    return 0;
}

PUBLIC UINT32 camera_FramesPerBuffer(VOID)
{
    if((pCamDrv->camera_FramesPerBuffer!=NULL))
        return(pCamDrv->camera_FramesPerBuffer());

    return 0;
}

PUBLIC UINT8 camera_GetId(VOID)
{
    if((pCamDrv->camera_GetId!=NULL))
        return(pCamDrv->camera_GetId());

    return 0;
}

PUBLIC char* camera_GetStringId(VOID)
{
    if(camera_load_drv())
    {
        if((pCamDrv->camera_GetStringId!=NULL))
            return(pCamDrv->camera_GetStringId());
    }
    else
    {
        SXS_TRACE(TSTDOUT,"camera not foud" );
    }
    return NULL;
}

PUBLIC VOID camera_GetImageInfo(CAM_SIZE_T * pcamout,UINT16 *pwidth,UINT16 *pheight)
{
    if ((pCamDrv->camera_GetImageInfo != NULL))
    {
        pCamDrv->camera_GetImageInfo(pcamout,pwidth,pheight);
    }
    else
    {
        *pcamout = *pwidth = *pheight = 0;
    }
}

PUBLIC BOOL camera_ImageIsConvert(VOID)
{
    if(camera_load_drv())
    {
        CAM_SIZE_T c;
        UINT16 w = 0;
        UINT16 h = 0;
        camera_GetImageInfo(&c,&w,&h);
        return (w<h)?TRUE:FALSE;
    }
    else
    {
        SXS_TRACE(TSTDOUT,"camera not foud" );
    }

    return FALSE;
}
// CameraID could be 0, the default one; or 1, the second one.
PUBLIC VOID camera_SetCameraID(UINT32 CameraID)
{
    gCameraID = CameraID;
}
PUBLIC UINT32 camera_GetCameraID()
{
    return gCameraID;
}

//#ifdef SUPPORT_DUAL_CAMERA
PUBLIC VOID camera_SwitchSensor()
{
    gCameraID = (gCameraID==0)?1:0;
}

PUBLIC VOID camera_SwitchSensorReset()
{
    gCameraID = 0;
}
//#endif
