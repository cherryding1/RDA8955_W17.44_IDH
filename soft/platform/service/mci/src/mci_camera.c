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


#if (CSW_EXTENDED_API == 1)

#include "cs_types.h"
#include "csw_lcd.h"
#include "mci.h"
#include "mcip_debug.h"
#include "mcip_camera.h"
#include "cos.h"
#include "sxr_mem.h"
#include "sxr_tls.h"

#include "hal_debug.h"
#include "hal_host.h"

#include "cams_m.h"
#include "lcdd_m.h"
#include "imgs_m.h"
#include "string.h"


// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================

// TODO Get that from a proper header file
// (Comes from the MMI, naming is self explanatory)
EXPORT UINT32 get_lcd_frame_buffer_address(VOID);

// =============================================================================
//  MACROS
// =============================================================================
//#define GLOBAL_LOCK


// Should be comming from a header of the mmi
#define MCI_CAMERA_TRANS_COLOR 0


#define MCI_CAM_IMG_SIZE_SCREEN 1
#define MCI_CAM_IMG_SIZE_SCREEN_UNSUPPORTED 2
#define MCI_CAM_IMG_SIZE_QVGA   3
#define MCI_CAM_IMG_SIZE_VGA    4
#define MCI_CAM_IMG_SIZE_SXGA    5
#define MCI_CAM_IMG_SIZE_QQVGA    6


// =============================================================================
//  TYPES
// =============================================================================
// Must be identical to the med_api.h one
enum
{
    CAM_PARAM_NONE = 0,
    CAM_PARAM_ZOOM_FACTOR,
    CAM_PARAM_CONTRAST,
    CAM_PARAM_BRIGHTNESS,
    CAM_PARAM_HUE,
    CAM_PARAM_GAMMA,
    CAM_PARAM_WB,
    CAM_PARAM_EXPOSURE,
    CAM_PARAM_EFFECT,
    CAM_PARAM_BANDING,    //9  //add by WeiD
    CAM_PARAM_SATURATION,
    CAM_PARAM_NIGHT_MODE,
    CAM_PARAM_EV_VALUE,
    CAM_PARAM_FLASH,
    CAM_PARAM_FLASH_MODE = CAM_PARAM_FLASH,
    CAM_PARAM_AE_METERING,
    CAM_PARAM_AF_KEY,
    CAM_PARAM_AF_METERING,
    CAM_PARAM_AF_MODE,
    CAM_PARAM_MANUAL_FOCUS_DIR,
    CAM_PARAM_SHUTTER_PRIORITY,
    CAM_PARAM_APERTURE_PRIORITY,
    CAM_PARAM_ISO,
    CAM_PARAM_SCENE_MODE,
    CAM_PARAM_QUALITY, //2//23  //add by WeiD
    CAM_PARAM_IMAGESIZE, //24 add by WeiD
    CAM_PARAM_ADD_FRAME,//25,chenhe add
    CAM_NO_OF_PARAM
};


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
PRIVATE MCI_CAMERA_INFO_STRUCT_T g_mciCameraInfo =
{
    .startX = 0,
    .startY = 0,
    .endX = 0,
    .endY = 0,
    .imageWidth = 0,
    .imageHeight = 0,
    .screenWidth = 0,
    .screenHeight = 0,
    .previewZoom = 0,
    .imageQuality = 0,
    .addFrame = FALSE
};

BOOL g_mciLcdBypassStatus = FALSE;
BOOL g_mciLcdOsdRefreshed = FALSE;

// =============================================================================
// g_mciCameraPreviewFbw
// -----------------------------------------------------------------------------
/// This frame buffer window represents the whole screen. Its buffer is
/// allocated when the MCI camera service is opened (MCI_CamPowerUp), and
/// is released when is it closed (MCI_CamPowerDown).
/// This buffer is the parameter passed to the cams_StartPreview function
/// to receive the preview image.
// =============================================================================
PRIVATE LCDD_FBW_T g_mciCameraPreviewFbw =
{
    .fb =
    {
        .buffer = NULL,
        .width  = 0,
        .height = 0,
        .colorFormat = 0
    },
    .roi =
    {
        .x      = 0,
        .y      = 0,
        .width  = 0,
        .height = 0
    }
};
// =============================================================================
// g_mciCameraOsdFbw
// -----------------------------------------------------------------------------
/// This frame buffer window represents the whole screen. Its buffer is
/// allocated when the MCI camera service is opened (MCI_CamPowerUp), and
/// is released when is it closed (MCI_CamPowerDown).
/// This buffer is storing the OSD picture to be merged with the camera
/// image
// =============================================================================
PRIVATE LCDD_FBW_T  g_mciCameraOsdFbw =
{
    .fb =
    {
        .buffer = NULL,
        .width  = 0,
        .height = 0,
        .colorFormat = 0
    },
    .roi =
    {
        .x      = 0,
        .y      = 0,
        .width  = 0,
        .height = 0
    }
};


// =============================================================================
// g_mciCameraCaptureFbw
// -----------------------------------------------------------------------------
/// This frame buffer window represents the whole picture. Its buffer is
/// allocated when the MCI capture is called and
/// is released when it is  saved
/// This buffer is the parameter passed to the cams_Capture function
/// to receive the captured image.
// =============================================================================
PRIVATE LCDD_FBW_T g_mciCameraCaptureFbw =
{
    .fb =
    {
        .buffer = NULL,
        .width  = 0,
        .height = 0,
        .colorFormat = 0
    },
    .roi =
    {
        .x      = 0,
        .y      = 0,
        .width  = 0,
        .height = 0
    }
};


// =============================================================================
//  FUNCTIONS
// =============================================================================
PRIVATE VOID MCI_CamSaveOsd(UINT16 *buffer, UINT16 startx, UINT16 starty, UINT16 endx, UINT16 endy)
{
    memcpy( g_mciCameraOsdFbw.fb.buffer,
            buffer,
            g_mciCameraInfo.screenWidth * g_mciCameraInfo.screenHeight * 2
          );
    g_mciLcdOsdRefreshed = TRUE;
}
// =============================================================================
// MCI_CamPowerUp
// -----------------------------------------------------------------------------
/// Power up and opening of the camera service.
/// @param videoMode Ignored.
// =============================================================================
PUBLIC UINT32 MCI_CamPowerUp(INT32 videoMode)
{
    CAMS_ERR_T errStatus;
    LCDD_SCREEN_INFO_T screenInfo;
    UINT32 i;

    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamPowerUp");
    // Bring up service
    errStatus = cams_Open();
    if (errStatus != CAMS_ERR_NO)
    {
        return errStatus;
    }

    // Record Screen info
    lcdd_GetScreenInfo(&screenInfo);
    g_mciCameraInfo.screenWidth = screenInfo.width;
    g_mciCameraInfo.screenHeight = screenInfo.height;

    // Initialize Preview buffer
    // Allocate the max size
    g_mciCameraPreviewFbw.fb.width  = 0;
    g_mciCameraPreviewFbw.fb.height = 0;
    g_mciCameraPreviewFbw.fb.colorFormat = screenInfo.bitdepth;
    g_mciCameraPreviewFbw.roi.x = 0;
    g_mciCameraPreviewFbw.roi.y = 0;
    g_mciCameraPreviewFbw.roi.width = 0;
    g_mciCameraPreviewFbw.roi.height = 0;
    g_mciCameraPreviewFbw.fb.buffer = sxr_Malloc(screenInfo.width * screenInfo.height * 2);
    if (g_mciCameraPreviewFbw.fb.buffer == NULL)
    {
        // Anyhow the malloc function will assert in case of
        // memory shortage.
        return MCI_ERR_OUT_OF_MEMORY;
    }
    // Clear preview buffer
    for (i = 0; i < screenInfo.width * screenInfo.height; i++)
    {
        *(g_mciCameraPreviewFbw.fb.buffer + i) = 0;
    }
    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "g_mciCameraPreviewFbw.fb.buffer allocated:%08x", (UINT32)g_mciCameraPreviewFbw.fb.buffer);

    // Initialize Osd buffer
    // Allocate the max size
    g_mciCameraOsdFbw.fb.width  = screenInfo.width;
    g_mciCameraOsdFbw.fb.height = screenInfo.height;
    g_mciCameraOsdFbw.fb.colorFormat = screenInfo.bitdepth;
    g_mciCameraOsdFbw.roi.x = 0;
    g_mciCameraOsdFbw.roi.y = 0;
    g_mciCameraOsdFbw.roi.width = screenInfo.width;
    g_mciCameraOsdFbw.roi.height = screenInfo.height;
    g_mciCameraOsdFbw.fb.buffer = sxr_Malloc(screenInfo.width * screenInfo.height * 2);
    if (g_mciCameraOsdFbw.fb.buffer == NULL)
    {
        // Anyhow the malloc function will assert in case of
        // memory shortage.
        return MCI_ERR_OUT_OF_MEMORY;
    }
    // Clear OSD buffer
    for (i = 0; i < screenInfo.width * screenInfo.height; i++)
    {
        *(g_mciCameraOsdFbw.fb.buffer + i) = MCI_CAMERA_TRANS_COLOR;
    }
    g_mciLcdOsdRefreshed = FALSE;
    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "g_mciCameraOsdFbw.fb.buffer allocated:%08x", (UINT32)g_mciCameraOsdFbw.fb.buffer);


    // Initialize Capture buffer
    // Allocate the max size
    g_mciCameraCaptureFbw.fb.width  = 0;
    g_mciCameraCaptureFbw.fb.height = 0;
    g_mciCameraCaptureFbw.fb.colorFormat = screenInfo.bitdepth;
    g_mciCameraCaptureFbw.roi.x = 0;
    g_mciCameraCaptureFbw.roi.y = 0;
    g_mciCameraCaptureFbw.roi.width = 0;
    g_mciCameraCaptureFbw.roi.height = 0;
    // Let's assume 16 bits per pixels
    g_mciCameraCaptureFbw.fb.buffer = sxr_Malloc(MCI_CAM_MAX_CAPTURE_SIZE * 2);
    if (g_mciCameraCaptureFbw.fb.buffer == NULL)
    {
        // Anyhow the malloc function will assert in case of
        // memory shortage.
        return MCI_ERR_OUT_OF_MEMORY;
    }
    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "g_mciCameraCaptureFbw.fb.buffer allocated:%08x", (UINT32)g_mciCameraCaptureFbw.fb.buffer);

    g_mciLcdBypassStatus = mci_LcdStartBypass(MCI_CamSaveOsd);
    // Lock the LCD for preview
    if (g_mciLcdBypassStatus == FALSE)
    {
        // Could not lock the LCD
        // return on error
        MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamPowerUp : Could not get LCD lock");
        return MCI_ERR_BUSY;
    }

    return  MCI_ERR_NO;
}


// =============================================================================
// MCI_CamPowerDown
// -----------------------------------------------------------------------------
/// Close the CAMS service and power it off.
/// WARNING !!!!!! This function is called several times by the MMI.
// =============================================================================
PUBLIC UINT32 MCI_CamPowerDown(VOID)
{
    CAMS_ERR_T errStatus;
    UINT32 status;

    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamPowerDown");
    // Stop the preview, just in case
    cams_StopPreview();

    // Wait for end of preview
    while (cams_PreviewInProgress())
    {
        // Todo evaluate optimal sleep duration
        sxr_Sleep(64);
    }
    // Release the lock on LCD
    if (g_mciLcdBypassStatus)
    {
        mci_LcdStopBypass();
        g_mciLcdOsdRefreshed = FALSE;
        g_mciLcdBypassStatus = FALSE;
    }

    // Free all memory.
    // MMI calling that several times fropm different locations
    // Make the check thread safe to avoid issues
    status = hal_SysEnterCriticalSection();
    if (g_mciCameraPreviewFbw.fb.buffer != NULL)
    {
        MCI_TRACE(MCI_CAM_TRC_LVL, 0, "g_mciCameraPreviewFbw.fb.buffer to free:%08x", (UINT32)g_mciCameraPreviewFbw.fb.buffer);
        sxr_Free(g_mciCameraPreviewFbw.fb.buffer);
        g_mciCameraPreviewFbw.fb.buffer = NULL;
    }

    if (g_mciCameraOsdFbw.fb.buffer != NULL)
    {
        MCI_TRACE(MCI_CAM_TRC_LVL, 0, "g_mciCameraOsdFbw.fb.buffer to free:%08x", (UINT32)g_mciCameraOsdFbw.fb.buffer);
        sxr_Free(g_mciCameraOsdFbw.fb.buffer);
        g_mciCameraOsdFbw.fb.buffer = NULL;
    }

    if (g_mciCameraCaptureFbw.fb.buffer != NULL)
    {
        MCI_TRACE(MCI_CAM_TRC_LVL, 0, "g_mciCameraCaptureFbw.fb.buffer to free:%08x", (UINT32)g_mciCameraCaptureFbw.fb.buffer);
        sxr_Free(g_mciCameraCaptureFbw.fb.buffer);
        g_mciCameraCaptureFbw.fb.buffer = NULL;
    }

    // Close the service
    errStatus = cams_Close();

    hal_SysExitCriticalSection(status);
    if (errStatus != CAMS_ERR_NO)
    {
        return errStatus;
    }

    return MCI_ERR_NO;
}


// =============================================================================
// MCI_CamSetPara
// -----------------------------------------------------------------------------
/// Configure camera special effects.
/// @param effectCode Effect to configure
/// @param value Value to set the parameter to.
/// @return 0 (#CAMS_ERR_NO), when everything works fine, a CAMS error code
/// when something has failed.
// =============================================================================
PUBLIC UINT32 MCI_CamSetPara(INT32 effectCode, INT32 value)
{
    UINT32 localEffectCode = 0;

    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamSetPara: effect %d value %d", effectCode, value );

    switch(effectCode)
    {
        case CAM_PARAM_CONTRAST:
            return cams_SetParam(CAMS_PARAM_CONTRAST, value);
            break;

        case CAM_PARAM_BRIGHTNESS:
            return cams_SetParam(CAMS_PARAM_BRIGHTNESS, value);
            break;

        case CAM_PARAM_EXPOSURE:
            MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamSetPara: Exposure %d", value);
            return cams_SetParam(CAMS_PARAM_EXPOSURE, CAMS_PARAM_EXPOSURE_NEGATIVE_2 + value );
            break;

        case CAM_PARAM_IMAGESIZE:
            switch(value)
            {
                case MCI_CAM_IMG_SIZE_QQVGA:
                    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamSetPara: IMAGESIZE QQVGA");
                    g_mciCameraInfo.imageWidth =  160;
                    g_mciCameraInfo.imageHeight = 120;
                    break;

                case MCI_CAM_IMG_SIZE_SCREEN:
                    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamSetPara: IMAGESIZE SCREEN");
                    g_mciCameraInfo.imageWidth =  g_mciCameraInfo.screenWidth;
                    g_mciCameraInfo.imageHeight = g_mciCameraInfo.screenHeight;
                    break;

                case MCI_CAM_IMG_SIZE_QVGA:
                    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamSetPara: IMAGESIZE QVGA");
                    g_mciCameraInfo.imageWidth =  320;
                    g_mciCameraInfo.imageHeight = 240;
                    break;

                case MCI_CAM_IMG_SIZE_VGA:
                    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamSetPara: IMAGESIZE VGA");
                    g_mciCameraInfo.imageWidth =  640;
                    g_mciCameraInfo.imageHeight = 480;
                    break;

                case MCI_CAM_IMG_SIZE_SXGA:
                    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamSetPara: IMAGESIZE XGA");
                    g_mciCameraInfo.imageWidth =  640;
                    g_mciCameraInfo.imageHeight = 480;
                    break;

                default:
                    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamSetPara: IMAGESIZE %d (default to screen)", value);
                    g_mciCameraInfo.imageWidth =  g_mciCameraInfo.screenWidth;
                    g_mciCameraInfo.imageHeight = g_mciCameraInfo.screenHeight;
                    break;
            }
            if (cams_PreviewInProgress())
            {
                // Stop the current preview
                cams_StopPreview();
                while (cams_PreviewInProgress())
                {
                    // Wait until preview is really finished
                    sxr_Sleep(64);
                }
                // Select the new preview size based on the requested resolution
                switch (value)
                {
                    case MCI_CAM_IMG_SIZE_QQVGA:
                    case MCI_CAM_IMG_SIZE_QVGA:
                    case MCI_CAM_IMG_SIZE_VGA:
                    case MCI_CAM_IMG_SIZE_SXGA:
                        // TODO this is ahrdcoded and really bad
                        // In the case of a change of mode, the mmi should close
                        // and re-open in the new mode
                        g_mciCameraPreviewFbw.fb.width = g_mciCameraInfo.screenWidth;
                        g_mciCameraPreviewFbw.fb.height =  g_mciCameraInfo.screenHeight;
                        g_mciCameraPreviewFbw.roi.x = 0;
                        g_mciCameraPreviewFbw.roi.y = 30;
                        g_mciCameraPreviewFbw.roi.width = g_mciCameraInfo.screenWidth;
                        g_mciCameraPreviewFbw.roi.height = 96;
                        break;
                    case MCI_CAM_IMG_SIZE_SCREEN:
                    default:
                        g_mciCameraPreviewFbw.fb.width = g_mciCameraInfo.screenWidth;
                        g_mciCameraPreviewFbw.fb.height =  g_mciCameraInfo.screenHeight;
                        g_mciCameraPreviewFbw.roi.x = 0;
                        g_mciCameraPreviewFbw.roi.y = 0;
                        g_mciCameraPreviewFbw.roi.width = g_mciCameraInfo.screenWidth;
                        g_mciCameraPreviewFbw.roi.height = g_mciCameraInfo.screenHeight;
                        break;
                }
                return cams_StartPreview(&g_mciCameraPreviewFbw, 10, mci_CameraPreviewHandler);
            }
            break;
        case CAM_PARAM_ADD_FRAME:
            g_mciCameraInfo.addFrame = value;
            return 0;
            break;

        // The followings is ought to return each case.
        case CAM_PARAM_ZOOM_FACTOR:
            g_mciCameraInfo.previewZoom = value;
            return cams_SetParam(CAMS_PARAM_DIGITAL_ZOOM, g_mciCameraInfo.previewZoom);
            break;

        case CAM_PARAM_QUALITY:
            if(value == 0)
            {
                g_mciCameraInfo.imageQuality = 0;
            }
            else
            {
                g_mciCameraInfo.imageQuality = 1;
            }
            return 0;
            break;

        case CAM_PARAM_EFFECT:
            localEffectCode = CAMS_PARAM_SPECIAL_EFFECT_NORMAL;
            switch(value)
            {
                case 0:
                    localEffectCode = CAMS_PARAM_SPECIAL_EFFECT_NORMAL;
                    break;
                case 17:
                    localEffectCode = CAMS_PARAM_SPECIAL_EFFECT_ANTIQUE;
                    break;
                case 18:
                    localEffectCode = CAMS_PARAM_SPECIAL_EFFECT_REDISH;
                    break;

                case 19:
                    localEffectCode = CAMS_PARAM_SPECIAL_EFFECT_GREENISH;
                    break;
                case 20:
                    localEffectCode = CAMS_PARAM_SPECIAL_EFFECT_BLUEISH;
                    break;

                case 21:
                    localEffectCode = CAMS_PARAM_SPECIAL_EFFECT_BLACKWHITE;
                    break;
                case 22:
                    localEffectCode = CAMS_PARAM_SPECIAL_EFFECT_NEGATIVE;
                    break;

                default:
                    break;
            }
            return cams_SetParam(CAMS_PARAM_SPECIAL_EFFECT, localEffectCode);
            break;

        case CAM_PARAM_NIGHT_MODE:
            return cams_SetParam(CAMS_PARAM_NIGHT_MODE, (value ? TRUE : FALSE));
            break;

        case CAM_PARAM_WB:
            return cams_SetParam(CAMS_PARAM_WHITE_BALANCE, value);
            break;

        default:
            MCI_ASSERT(FALSE, "MCI_CamSetParam: unknown parameter:%d", effectCode);
            return 0;
            break;
    }

    return 0;
}

// =============================================================================
/// mci_CameraBlendOsd
// -----------------------------------------------------------------------------
// =============================================================================
PRIVATE VOID mci_CameraBlendOsd(LCDD_FBW_T *previewFbw, LCDD_FBW_T *osdFbw)
{
    UINT16 x, y;
    UINT16 *dstPtr;
    UINT16 *osdPtr;

    //MCI_TRACE(MCI_CAM_TRC_LVL, 0, "mci_CameraBlendOsd src:%x osd:%x",previewFbw->fb.buffer,osdFbw->fb.buffer);
    //MCI_TRACE(MCI_CAM_TRC_LVL, 0, "mci_CameraBlendOsd fb w:%d h:%d",previewFbw->fb.width,previewFbw->fb.height);
    //MCI_TRACE(MCI_CAM_TRC_LVL, 0, "mci_CameraBlendOsd roi x:%d y:%d",previewFbw->roi.x,previewFbw->roi.y);

    dstPtr = previewFbw->fb.buffer;
    osdPtr = osdFbw->fb.buffer;

    // Copy the OSD top lines if the roi of the preview frame does not start at 0
    for (y = 0; y < previewFbw->roi.y; y++)
    {
        for (x = 0; x < previewFbw->fb.width; x++)
        {
            *(dstPtr + x) = *(osdPtr + x);
        }
        // go one line down
        dstPtr += previewFbw->fb.width;
        osdPtr += osdFbw->fb.width;
    }

    for (y = previewFbw->roi.y; y < previewFbw->roi.y + previewFbw->roi.height; y++)
    {
        for (x = 0; x < previewFbw->fb.width; x++)
        {
            // Replace preview picture pixels by OSD screen
            // non transparent pixels
            if (*(osdPtr + x) != MCI_CAMERA_TRANS_COLOR)
            {
                *(dstPtr + x) = *(osdPtr + x);
            }
        }
        // go one line down
        dstPtr += previewFbw->fb.width;
        osdPtr += osdFbw->fb.width;
    }
    // Copy the OSD bottom lines if the roi of the preview frame does not end at
    // previewFbw->fb.height
    for (y = previewFbw->roi.y + previewFbw->roi.height; y < previewFbw->fb.height; y++)
    {
        for (x = 0; x < previewFbw->fb.width; x++)
        {
            *(dstPtr + x) = *(osdPtr + x);
        }
        // go one line down
        dstPtr += previewFbw->fb.width;
        osdPtr += osdFbw->fb.width;
    }
}

// =============================================================================
// mci_CameraPreviewHandler
// -----------------------------------------------------------------------------
/// Handler called every time a preview capture has been made.
/// It is blended with something coming from the MMI and displayed on the
/// screen.
// =============================================================================
PRIVATE VOID mci_CameraPreviewHandler(LCDD_FBW_T *previewFbw)
{
    // We can use the buffer from g_mciCameraPreviewFbw
    // directly as CAMS uses an
    // internal buffer to store the camera data, which is
    // different from g_mciCameraPreviewFbw. (No race
    // condition).
    LCDD_ERR_T err;
    LCDD_FBW_T displayFbw;

    //MCI_TRACE(MCI_CAM_TRC_LVL, 0, "mci_CameraPreviewHandler");


    if (g_mciLcdOsdRefreshed)
    {
        MCI_TRACE(MCI_CAM_TRC_LVL, 0, "mci_CameraPreviewHandler : Update Preview Frame");
        mci_CameraBlendOsd(previewFbw, &g_mciCameraOsdFbw);

        displayFbw.fb.buffer = previewFbw->fb.buffer;
        displayFbw.fb.width = g_mciCameraInfo.screenWidth;
        displayFbw.fb.height = g_mciCameraInfo.screenHeight;
        displayFbw.fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565;
        displayFbw.roi.width = g_mciCameraInfo.screenWidth;
        displayFbw.roi.height = g_mciCameraInfo.screenHeight;
        displayFbw.roi.x = 0;
        displayFbw.roi.y = 0;
        err = lcdd_Blit16(&displayFbw, 0 , 0);
        // Display the blended image
        while (LCDD_ERR_NO != err)
        {
            MCI_TRACE(MCI_CAM_TRC_LVL, 0, "mci_CameraPreviewHandler Blit Preview error %d", err);
            sxr_Sleep(10);
            err = lcdd_Blit16(&displayFbw, 0 , 0);
        };

        MCI_TRACE(MCI_CAM_TRC_LVL, 0, "mci_CameraPreviewHandler Done");
    }
    else
    {
        MCI_TRACE(MCI_CAM_TRC_LVL, 0, "mci_CameraPreviewHandler : Skip Preview Frame No OSD");
    }
}
// =============================================================================
// MCI_CamPreviewOpen
// -----------------------------------------------------------------------------
/// Wrapper at MCI level for cams_StartPreview
/// @param data Describe the area to fill with the preview picture. (ignored)
// =============================================================================
PUBLIC UINT32 MCI_CamPreviewOpen(CAM_PREVIEW_STRUCT *data)
{
    UINT16 width = data->end_x - data->start_x + 1;
    UINT16 height = data->end_y - data->start_y + 1;
    UINT32 image_size;

#ifndef GLOBAL_LOCK
    if (g_mciLcdBypassStatus == FALSE)
    {
        g_mciLcdBypassStatus = mci_LcdStartBypass(MCI_CamSaveOsd);
        // Lock the LCD for preview
        if (g_mciLcdBypassStatus == FALSE)
        {
            // Could not lock the LCD
            // return on error
            MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamPreviewOpen : Could not get LCD lock");
            return MCI_ERR_BUSY;
        }
    }
#endif

    if ((width * height) > g_mciCameraInfo.screenWidth * g_mciCameraInfo.screenHeight)
    {
        // Unsupported preview size
        MCI_TRACE(MCI_CAM_TRC_LVL, 0,
                  "MCI_CamPreviewOpen : Unsupported preview resolution %d x %d",
                  width, height
                 );
        return MCI_ERR_INVALID_PARAMETER;

    }
    g_mciCameraPreviewFbw.fb.width = g_mciCameraInfo.screenWidth;
    g_mciCameraPreviewFbw.fb.height =  g_mciCameraInfo.screenHeight;
    g_mciCameraPreviewFbw.roi.x = data->start_x;
    g_mciCameraPreviewFbw.roi.y = data->start_y;
    g_mciCameraPreviewFbw.roi.width = width;
    g_mciCameraPreviewFbw.roi.height = height;

    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamPreviewOpen : image resolution %d x %d", data->image_width, data->image_height);
    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamPreviewOpen : roi H: %d -> %d", data->start_x, data->end_x);
    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamPreviewOpen : roi V: %d -> %d", data->start_y, data->end_y);

    switch (data->image_width)
    {
        case 1280:
            image_size = MCI_CAM_IMG_SIZE_SXGA;
            break;
        case 640:
            image_size = MCI_CAM_IMG_SIZE_VGA;
            break;
        case 320:
            image_size = MCI_CAM_IMG_SIZE_QVGA;
            break;
        case 160:
            image_size = MCI_CAM_IMG_SIZE_QQVGA;
            break;
        default:
            image_size = MCI_CAM_IMG_SIZE_SCREEN;
            break;
    }
    MCI_CamSetPara(CAM_PARAM_IMAGESIZE, image_size);
    MCI_CamSetPara(CAM_PARAM_NIGHT_MODE, data->nightmode);
    MCI_CamSetPara(CAM_PARAM_QUALITY, data->imageQuality);
    MCI_CamSetPara(CAM_PARAM_ZOOM_FACTOR, data->factor);
    MCI_CamSetPara(CAM_PARAM_CONTRAST, data->contrast);
    MCI_CamSetPara(CAM_PARAM_EFFECT, data->specialEffect);
    MCI_CamSetPara(CAM_PARAM_BRIGHTNESS, data->brightNess);
    MCI_CamSetPara(CAM_PARAM_WB, data->whiteBlance);
    MCI_CamSetPara(CAM_PARAM_EXPOSURE, data->exposure);
    MCI_CamSetPara(CAM_PARAM_ADD_FRAME, data->addFrame);


    return cams_StartPreview(&g_mciCameraPreviewFbw, 10, mci_CameraPreviewHandler);
}

// =============================================================================
// MCI_CamPreviewClose
// -----------------------------------------------------------------------------
/// Wrapper at MCI level for cams_StopPreview.
// =============================================================================
PUBLIC UINT32 MCI_CamPreviewClose(VOID)
{
    CAMS_ERR_T errStatus;

    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamPreviewClose");
    errStatus = cams_StopPreview();
    if (errStatus == CAMS_ERR_NO)
    {
        // Wait for end of preview
        while (cams_PreviewInProgress())
        {
            // Todo evaluate optimal sleep duration
            sxr_Sleep(64);
        }
#ifndef GLOBAL_LOCK
        // Release the lock on LCD
        if (g_mciLcdBypassStatus)
        {
            UINT32 i;

            mci_LcdStopBypass();
            // Clear OSD buffer
            for (i = 0; i < g_mciCameraInfo.screenWidth * g_mciCameraInfo.screenHeight; i++)
            {
                *(g_mciCameraOsdFbw.fb.buffer + i) = MCI_CAMERA_TRANS_COLOR;
            }
            g_mciLcdOsdRefreshed = FALSE;
            g_mciLcdBypassStatus = FALSE;
        }
#endif
    }

    return errStatus;
}

// =============================================================================
// mci_CameraCaptureHandler
// -----------------------------------------------------------------------------
/// Handler called every time a capture has been made.
/// It is blended with something coming from the MMI and displayed on the
/// screen.
// =============================================================================
PRIVATE VOID mci_CameraCaptureHandler(LCDD_FBW_T *dataFbw, LCDD_FBW_T *previewFbw)
{
    // We can use the buffer from g_mciCameraPreviewFbw
    // directly as CAMS uses an
    // internal buffer to store the camera data, which is
    // different from g_mciCameraPreviewFbw. (No race
    // condition).

    mci_CameraPreviewHandler(previewFbw);

#ifndef GLOBAL_LOCK
    // Release the lock on LCD
    if (g_mciLcdBypassStatus)
    {
        mci_LcdStopBypass();
        g_mciLcdOsdRefreshed = FALSE;
        g_mciLcdBypassStatus = FALSE;
    }
#endif

    // In case of AddFrame, we must add the frame on top of the capture buffer
    if (g_mciCameraInfo.addFrame)
    {
        if ((dataFbw->fb.width == g_mciCameraInfo.screenWidth) &&
                (dataFbw->fb.height == g_mciCameraInfo.screenHeight) )
        {
            // AddFrame only supported if Capture resolution
            // is screen resolution
            LCDD_FBW_T addedFrameFbw;

            addedFrameFbw.fb.buffer = (UINT16 *) get_lcd_frame_buffer_address();
            addedFrameFbw.fb.width = g_mciCameraInfo.screenWidth;
            addedFrameFbw.fb.height = g_mciCameraInfo.screenHeight;
            addedFrameFbw.roi.width = g_mciCameraInfo.screenWidth;
            addedFrameFbw.roi.height = g_mciCameraInfo.screenHeight;
            addedFrameFbw.roi.x = 0;
            addedFrameFbw.roi.y = 0;

            mci_CameraBlendOsd(dataFbw, &addedFrameFbw);
        }
    }
}

// =============================================================================
// mci_CameraCapture
// -----------------------------------------------------------------------------
/// Capture one frame
// =============================================================================
PUBLIC UINT32 MCI_CamCapture (CAM_CAPTURE_STRUCT *data)
{
    CAMS_ERR_T err;


    if (cams_PreviewInProgress())
    {
        MCI_CamPreviewClose();
    }

#ifndef GLOBAL_LOCK
    g_mciLcdBypassStatus = mci_LcdStartBypass(MCI_CamSaveOsd);
    // Lock the LCD for preview
    if (g_mciLcdBypassStatus == FALSE)
    {
        // Could not lock the LCD
        // return on error
        MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamCapture : Could not get LCD lock");
        return MCI_ERR_BUSY;
    }
    else
    {
        MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamCapture : LCD bypassed");
    }
#endif

    g_mciCameraCaptureFbw.fb.width  = data->image_width;
    g_mciCameraCaptureFbw.fb.height = data->image_height;
    g_mciCameraCaptureFbw.roi.x = 0;
    g_mciCameraCaptureFbw.roi.y = 0;
    g_mciCameraCaptureFbw.roi.width = data->image_width;
    g_mciCameraCaptureFbw.roi.height = data->image_height;

    err = cams_Capture(&g_mciCameraCaptureFbw, &g_mciCameraPreviewFbw, mci_CameraCaptureHandler);
    if (CAMS_ERR_NO == err)
    {
        return MCI_ERR_NO;
    }
    else
    {
        return MCI_ERR_BUSY;
    }
}

// =============================================================================
// MCI_CamSavePhoto
// -----------------------------------------------------------------------------
/// Save previously captured frame
// =============================================================================
PUBLIC UINT32 MCI_CamSavePhoto(UINT8 *filename)
{
    IMGS_ENCODING_PARAMS_T encParam;
    IMGS_ERR_T err;

    MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamSavePhoto");
    encParam.format = IMGS_IMG_FORMAT_JPG;
    encParam.jpg.quality = g_mciCameraInfo.imageQuality;


    err = imgs_EncodeImageFile(&g_mciCameraCaptureFbw, filename, &encParam, NULL);

    if (IMGS_ERR_NO == err)
    {
        return MCI_ERR_NO;
    }
    else
    {
        MCI_TRACE(MCI_CAM_TRC_LVL, 0, "MCI_CamSavePhoto: Error");
        return MCI_ERR_ACTION_NOT_ALLOWED;
    }
}
















#endif // CSW_EXTENDED_API




