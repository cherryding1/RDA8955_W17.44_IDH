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

#ifndef _CAMERA_PRIVATE_H_
#define _CAMERA_PRIVATE_H_

#include "camera_m.h"

#define CAMD_POSSIBLY_UNUSED __attribute__((unused))

//PRIVATE VOID camerap_CameraInfo(CAM_INFO_T *Info);
//
#if 0
PRIVATE VOID camerap_Reserve(CAM_IRQ_HANDLER_T FrameReady);
// =============================================================================
// camerap_Reserve(CAM_FRAME_READY_P FrameReady)
// =============================================================================
/// Reserve the camera resource.  Opens the I2C and puts the camera module into
/// power down and reset.  Enables the camera interface.  Must be called before
/// camerap_PowerOn()  FrameReady is the callback function that will be called
/// with the frame number parameter of the last frame that was received
// =============================================================================


PRIVATE VOID camerap_Release(VOID);


// =============================================================================
// camerap_PowerOn(VOID)
// =============================================================================
/// Power on the camera.  Camera must have first been initialized and
/// reserved by calling camerap_Reserve()
// =============================================================================
PRIVATE VOID camerap_PowerOn(CAM_SIZE_T CamOutSize, UINT16 IspOutWidth, UINT16 IspOutHeight, CAM_FORMAT_T Format);

PRIVATE VOID camerap_PowerOff(VOID);


// =============================================================================
// camerap_PrepareImageCaptureL(VOID)
// =============================================================================
/// Configures the camera to capture a single image with the format described
/// in the ImageFormat structure.
// =============================================================================/PRIVATE void camerap_PrepareImageCaptureL(CAM_FORMAT_T ImageFormat, UINT32 SizeIndex, CAM_IMAGE_T Buffer);

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
PRIVATE UINT8 camerap_GetId(VOID);
#endif
#endif // _CAMERA_M_H_
