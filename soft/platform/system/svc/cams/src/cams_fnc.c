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

#include "string.h"

#include "cams_m.h"
#include "camd_m.h"
#include "camsp_task.h"

#include "camsp_debug.h"

#include "sxr_ops.h"
//#include "itf_msg.h"

//#include "tgt_uctls_cfg.h"


#define INC_PRECISION 11


// ==============================================================================
// cams_Open
// ------------------------------------------------------------------------------
/// Open the camera service, and start the underlying task. This function must
/// be called before any other function of the camera service can be called.
// ==============================================================================
PUBLIC CAMS_ERR_T cams_Open(VOID)
{
    CAMS_PROFILE_FUNCTION_ENTER(cams_Open);
    CAMS_TRACE(TSTDOUT, 0, "cams_Open");
    if(g_camsIdTask == 0)
    {
        CAMS_TRACE(TSTDOUT, 0, "cams_Open: Create task");
//        g_currentHandler    = 0;
        g_camsTask.TaskBody  = cams_Task;
        g_camsTask.TaskExit  = 0;
        g_camsTask.Name      = "CAMS";
        g_camsTask.StackSize = CAMS_TASK_STACK_SIZE;
        g_camsTask.Priority  = CAMS_TASK_PRIORITY;
        g_camsMbx            = sxr_NewMailBox();
        g_camsIdTask         = sxr_NewTask(&g_camsTask);
        sxr_StartTask(g_camsIdTask, 0);
    }

    camd_Open(g_camsMbx);
    g_camsStatus = CAMS_STATUS_OPEN;
    camd_GetInfo(&g_camsAvailResolution, &g_camsValidParam);
    if (g_camsAvailResolution & (1<<CAMS_RESOLUTION_VGA))
    {
        g_camsCameraRawDataFbw.roi.width        = 640;
        g_camsCameraRawDataFbw.roi.height       = 480;
        g_camsCameraRawDataFbw.roi.x            = 0;
        g_camsCameraRawDataFbw.roi.y            = 0;
        g_camsCameraRawDataFbw.fb.width         = 640;
        g_camsCameraRawDataFbw.fb.height        = 480;
        g_camsCameraRawDataFbw.fb.colorFormat   = LCDD_COLOR_FORMAT_RGB_565;
        // FIXME Cannot do preview here at that resolution.
        g_camsCameraRawDataFbw.fb.buffer        = sxr_Malloc(640*480*2);
        g_camsCameraRawDataSize                 = 640*480*2;
    }
    else if (g_camsAvailResolution & (1<<CAMS_RESOLUTION_QVGA))
    {
        g_camsCameraRawDataFbw.roi.width        = 320;
        g_camsCameraRawDataFbw.roi.height       = 240;
        g_camsCameraRawDataFbw.roi.x            = 0;
        g_camsCameraRawDataFbw.roi.y            = 0;
        g_camsCameraRawDataFbw.fb.width         = 320;
        g_camsCameraRawDataFbw.fb.height        = 240;
        g_camsCameraRawDataFbw.fb.colorFormat   = LCDD_COLOR_FORMAT_RGB_565;
        g_camsCameraRawDataFbw.fb.buffer        = sxr_Malloc(320*240*2*2);
        g_camsCameraRawDataSize                 = 320*240*2*2;
    }
    else if (g_camsAvailResolution & (1<<CAMS_RESOLUTION_QQVGA))
    {
        g_camsCameraRawDataFbw.roi.width        = 160;
        g_camsCameraRawDataFbw.roi.height       = 120;
        g_camsCameraRawDataFbw.roi.x            = 0;
        g_camsCameraRawDataFbw.roi.y            = 0;
        g_camsCameraRawDataFbw.fb.width         = 160;
        g_camsCameraRawDataFbw.fb.height        = 120;
        g_camsCameraRawDataFbw.fb.colorFormat   = LCDD_COLOR_FORMAT_RGB_565;
        g_camsCameraRawDataFbw.fb.buffer        = sxr_Malloc(160*120*2*2);
        g_camsCameraRawDataSize                 = 160*120*2*2;
    }
    else
    {
        CAMS_TRACE(TSTDOUT, 0, "cams_Open : Camera resolution not supported");
        CAMS_PROFILE_FUNCTION_EXIT(cams_Open);
        return CAMS_ERR_NO_DEVICE;
    }

    CAMS_PROFILE_FUNCTION_EXIT(cams_Open);
    return CAMS_ERR_NO;
}


// =============================================================================
// cams_Close
// -----------------------------------------------------------------------------
/// When the camera service is no more needed, this function closes the service
/// and put the task to sleep. No other function of the service can be called,
/// but the #cams_Open function.
/// @return #CAMS_ERR_NO, #CAMS_ERR_RESOURCE_BUSY.
// =============================================================================
PUBLIC CAMS_ERR_T cams_Close(VOID)
{
    CAMS_PROFILE_FUNCTION_ENTER(cams_Close);
//    CAMS_ASSERT(g_camsStatus == CAMS_STATUS_OPEN,
//            "cams_Close can only be called when CAMS status is "
//            "CAMS_STATUS_OPEN, and not %d", g_camsStatus);

    CAMS_TRACE(TSTDOUT, 0, "cams_Close");
    camd_Close();

    // Free capture buffer
    if (g_camsCameraRawDataFbw.fb.buffer != NULL)
    {
        sxr_Free(g_camsCameraRawDataFbw.fb.buffer);

        g_camsCameraRawDataFbw.fb.buffer    =   NULL;
        g_camsCameraRawDataSize             =   0;
    }
    g_camsCameraRawDataFbw.roi.width        = 0;
    g_camsCameraRawDataFbw.roi.height       = 0;
    g_camsCameraRawDataFbw.roi.x            = 0;
    g_camsCameraRawDataFbw.roi.y            = 0;
    g_camsCameraRawDataFbw.fb.width         = 0;
    g_camsCameraRawDataFbw.fb.height        = 0;
    g_camsCameraRawDataFbw.fb.colorFormat   = LCDD_COLOR_FORMAT_RGB_565;
    g_camsCameraRawDataFbw.fb.buffer        = NULL;
    g_camsCameraRawDataSize                 =   0;
    g_camsStatus = CAMS_STATUS_CLOSE;
    // TODO: add some deactive task stuff ?
    CAMS_PROFILE_FUNCTION_EXIT(cams_Close);
    return CAMS_ERR_NO;
}


// =============================================================================
// cams_PreviewInProgress
// -----------------------------------------------------------------------------
/// Check if a preview is in progress.
/// @return \c TRUE if a preview is in progress, \c FALSE otherwise
// =============================================================================
PUBLIC BOOL cams_PreviewInProgress(VOID)
{
    return (g_camsStatus == CAMS_STATUS_PREVIEWING);
}


// =============================================================================
// cams_StartPreview
// -----------------------------------------------------------------------------
/// Start the preview process to aim the camera. The preview picture is
/// displayed/output in a frame buffer window. This frame buffer window is
/// automatically refreshed as long as the #cams_StopPreview function is not
/// called.
///
/// Everytime a preview frame has been filled in the frame buffer window, the
/// \c prvwHandler is called with the filled \c prvwFbw frame buffer window
/// as a parameter.
///
/// @param prvwFbw Frame buffer window where the preview result will be stored.
/// The image will be resized if needed (ie resolution different from the
/// window size).
/// @param prvwHandler Handler called every time a preview frame as been
/// captured in the preview frame buffer window \c prvwFbw.
/// @return #CAMS_ERR_NO, #CAMS_ERR_RESOURCE_BUSY, #CAMS_ERR_NO_DEVICE, ...
/// @todo How do we define the refresh rate ? (15fps/asap by tacit default)
// =============================================================================
PUBLIC CAMS_ERR_T cams_StartPreview(LCDD_FBW_T* prvwFbw,
                                    UINT8 frameRate,
                                    CAMS_PREVIEW_HANDLER_T prvwHandler)
{
    CAMS_PROFILE_FUNCTION_ENTER(cams_StartPreview);
    CAMS_ASSERT(g_camsIdTask != 0, "cams_Open must be called before cams_StartPreview");
    CAMS_ASSERT((g_camsStatus == CAMS_STATUS_OPEN)
                || (g_camsStatus == CAMS_STATUS_PREVIEWING),
                "cams_StartPreview can only be called when CAMS status is "
                "CAMS_STATUS_OPEN, and not %d", g_camsStatus);

    CAMS_MSG_PREVIEW_T* previewMsg = sxr_Malloc(sizeof(CAMS_MSG_PREVIEW_T));
    previewMsg->headerId = CAMS_OP_ID_START_PREVIEW;
    previewMsg->prvwFbw = prvwFbw;
    previewMsg->frameRate = frameRate;
    previewMsg->prvwHandler = prvwHandler;

    sxr_Send(previewMsg, g_camsMbx, SXR_SEND_MSG);
    CAMS_PROFILE_FUNCTION_EXIT(cams_StartPreview);
    return CAMS_ERR_NO;
}


// =============================================================================
// cams_StopPreview
// -----------------------------------------------------------------------------
/// Stop the preview process started by #cams_StartPreview.
/// When the preview is stopped, the #CAMS_MSG_PREVIEW_STOPPED message is sent
/// to the mailbox registered by #cams_Open.
///
/// @return #CAMS_ERR_NO.
// =============================================================================
PUBLIC CAMS_ERR_T cams_StopPreview(VOID)
{
    CAMS_PROFILE_FUNCTION_ENTER(cams_StopPreview);
    CAMS_ASSERT(g_camsIdTask != 0, "cams_Open must be called before cams_StopPreview");
//    CAMS_ASSERT((g_camsStatus == CAMS_STATUS_OPEN)
//            || (g_camsStatus == CAMS_STATUS_PREVIEWING),
//            "cams_StopPreview can only be called when CAMS status is "
//            "CAMS_STATUS_OPEN, and not %d", g_camsStatus);

    CAMS_MSG_T* stopMsg = sxr_Malloc(sizeof(CAMS_MSG_T));
    stopMsg->headerId = CAMS_OP_ID_STOP_PREVIEW;
    stopMsg->status = 0;
    sxr_Send(stopMsg, g_camsMbx, SXR_SEND_MSG);
    CAMS_PROFILE_FUNCTION_EXIT(cams_StopPreview);
    return CAMS_ERR_NO;
}

// =============================================================================
// cams_Capture
// -----------------------------------------------------------------------------
/// Capture one picture. The full size image is stored in the \c dataFbw, and
/// a preview version is stored in the \c prvwFbw frame buffer window. The
/// dataFbw frame buffer window shall be a full (ie the region of interest is
/// the full frame buffer) frame buffer window at the same size as the desired
/// resolution.
///
/// Once the capture is done, the \c captureHandler handler is called. It has
/// two parameters: \c dataFbw, holding the full resolution image, and
/// \c prvwFbw, with a scaled picture for the preview on the screen.
///
/// @param dataFbw Frame buffer window where the full resolution picture is
/// saved. Must have the \c resolution's width and height.
/// @param prvwFbw Frame buffer window where a preview of the picture is stored.
/// It can have any dimension.
/// @return #CAMS_ERR_NO, #CAMS_ERR_RESOURCE_BUSY, #CAMS_ERR_NO_DEVICE, ...
// =============================================================================
PUBLIC CAMS_ERR_T cams_Capture( LCDD_FBW_T*             dataFbw,
                                LCDD_FBW_T*             prvwFbw,
                                CAMS_CAPTURE_HANDLER_T  captureHandler)
{
    CAMS_PROFILE_FUNCTION_ENTER(cams_Capture);
    CAMS_ASSERT(g_camsIdTask != 0, "cams_Open must be called before cams_Capture");
    CAMS_ASSERT(g_camsStatus == CAMS_STATUS_OPEN,
                "cams_Capture can only be called when CAMS status is "
                "CAMS_STATUS_OPEN, and not %d", g_camsStatus);
    CAMS_MSG_CAPTURE_T* captureMsg = sxr_Malloc(sizeof(CAMS_MSG_CAPTURE_T));
    captureMsg->headerId = CAMS_OP_ID_START_CAPTURE;
    captureMsg->dataFbw = dataFbw;
    captureMsg->prvwFbw = prvwFbw;
    captureMsg->captureHandler = captureHandler;

    sxr_Send(captureMsg, g_camsMbx, SXR_SEND_MSG);
    CAMS_PROFILE_FUNCTION_EXIT(cams_Capture);
    return CAMS_ERR_NO;
}


// =============================================================================
// cams_SetParam
// -----------------------------------------------------------------------------
/// Set a camera parameter to a given value.
/// The parameter to set is among the values defined by the type #CAMS_PARAM_T.
/// If this parameter is not available for a given implementation of CAMS,
/// the #CAMS_ERR_UNSUPPORTED_PARAM error must be returned.
/// Depending on the CAMS_PARAM_XXX parameter to set, the value can either
/// be a proper UINT32 number, or a value of the CAMS_PARAM_XXX_T enum, might
/// it exists. This is detailed in the CAMS_PARAM_T definition.
/// If the value is not supported for this parameter, #CAMS_ERR_UNSUPPORTED_VALUE
/// is returned.
///
/// @param param Camera parameter to set.
/// @param value Value to set to the parameter to.
/// @return #CAMS_ERR_NO, #CAMS_ERR_UNSUPPORTED_PARAM, #CAMS_ERR_UNSUPPORTED_VALUE
/// ...
// =============================================================================
PUBLIC CAMS_ERR_T cams_SetParam(CAMS_PARAM_T param, UINT32 value)
{
    CAMS_ASSERT(g_camsIdTask != 0, "cams_Open must be called before cams_SetParam");
    CAMS_ASSERT(g_camsStatus != CAMS_STATUS_CLOSE,
                "cams_SetParam can only be called when CAMS is opened");
    // Let's hope we are keeping that compatibility properly
    CAMS_TRACE(TSTDOUT, 0, "cams_SetParam %d, %d",param, value);
    switch (param)
    {
        case CAMS_PARAM_DIGITAL_ZOOM:
            // Zoom is (100+20*value)
            // value from 0 to 5 => zoomin from 100% to 200%
            g_camsDigitalZoomFactor = (5+value)*ZOOM_PRECISION/5;
            return 0;
        default:
            // By default we pass it directly to CAMD
            return camd_SetParam(param, value);
    }

}


// =============================================================================
// cams_ResizeVgaTo128x160
// -----------------------------------------------------------------------------
/// Fast resize of a VGA frame buffer window into a screen size frame
/// buffer window
/// @param vgaFbw VGA source frame buffer window.
/// @param screenFbw Screem frame buffer window.
/// @param rotate Do we need to do a rotation of the screen?
// =============================================================================
PUBLIC VOID cams_ResizeVgaTo128x160( LCDD_FBW_T* vgaFbw,
                                     LCDD_FBW_T* screenFbw,
                                     UINT32      rotate)
{
    CAMS_PROFILE_FUNCTION_ENTER(cams_ResizeVgaTo128x160);
    CAMS_ASSERT(((vgaFbw->fb.width == 640)
                 && (vgaFbw->fb.height == 480)
                 && (vgaFbw->fb.colorFormat == LCDD_COLOR_FORMAT_RGB_565)
                 && (vgaFbw->roi.width == 640)
                 && (vgaFbw->roi.height == 480))
                || ((vgaFbw->fb.width == 480)
                    && (vgaFbw->fb.height == 640)
                    && (vgaFbw->fb.colorFormat == LCDD_COLOR_FORMAT_RGB_565)
                    && (vgaFbw->roi.width == 480)
                    && (vgaFbw->roi.height == 640)),
                "cams_ResizeVgaToScreen only handle VGA buffers "
                " in input");
    switch (screenFbw->fb.width)
    {
        case 128:
            // TODO Will need further implementation in case
            // of other screen resolutions ...
            CAMS_ASSERT((screenFbw->fb.height == 160)
                        && (screenFbw->fb.colorFormat == LCDD_COLOR_FORMAT_RGB_565)
                        && (screenFbw->roi.width == 128)
                        && (screenFbw->roi.height == 160),
                        "This screen format is not supported by "
                        "cams_ResizeVgaToScreen yet.");

            // We are taking one quarter of the pixels on the
            // 640->160 front, and 1 every 4, 4, 4, 3 pixels on the
            // 480->128 one. Taking the possible rotation into account,
            // that leaves us with:
            UINT32 h = 0;
            UINT32 v = 0;

            //UINT32 i = 0;

            UINT32 j = 0;

            UINT32 x = 0;
            UINT32 y = 0;

            UINT16* vgaPtr      = vgaFbw->fb.buffer;
            UINT16* screenPtr   = screenFbw->fb.buffer;

            switch (rotate)
            {
                case 0:
                    // 640*480 --> 128*160, with a right rotation
                    x = 127;
                    y = 0;
                    for (v=0 ; v<32 ; v++)
                    {
                        for (j=0 ; j<3 ; j++)
                        {
                            vgaPtr = vgaPtr + 4* vgaFbw->fb.width;
                            y = 0;
                            for (h=0 ; h<640 ; h+=4)
                            {
                                screenPtr[x + y*screenFbw->fb.width] = vgaPtr[h];
                                y++;
                            }
                            x--;
                        }

                        vgaPtr = vgaPtr + 3* vgaFbw->fb.width;
                        y = 0;
                        for (h=0 ; h<640 ; h+=4)
                        {
                            screenPtr[x + y*screenFbw->fb.width] = vgaPtr[h];
                            y++;
                        }
                        x--;
                    }
                    break;

                case 1:
                    // 640*480 --> 128*160, with a right rotation, with a vertical mirroring
                    x = 0;
                    y = 0;
                    for (v=0 ; v<32 ; v++)
                    {
                        for (j=0 ; j<3 ; j++)
                        {
                            vgaPtr = vgaPtr + 4* vgaFbw->fb.width;
                            y = 0;
                            for (h=0 ; h<640 ; h+=4)
                            {
                                screenPtr[x + y*screenFbw->fb.width] = vgaPtr[h];
                                y++;
                            }
                            x++;
                        }

                        vgaPtr = vgaPtr + 3* vgaFbw->fb.width;
                        y = 0;
                        for (h=0 ; h<640 ; h+=4)
                        {
                            screenPtr[x + y*screenFbw->fb.width] = vgaPtr[h];
                            y++;
                        }
                        x++;
                    }
                    break;

                case 2:
                    // 640*480 --> 128*160, with a left rotation
                    x = 0;
                    y = 159;
                    for (v=0 ; v<32 ; v++)
                    {
                        for (j=0 ; j<3 ; j++)
                        {
                            vgaPtr = vgaPtr + 4* vgaFbw->fb.width;
                            y = 159;
                            for (h=0 ; h<640 ; h+=4)
                            {
                                screenPtr[x + y*screenFbw->fb.width] = vgaPtr[h];
                                y--;
                            }
                            x++;
                        }

                        vgaPtr = vgaPtr + 3* vgaFbw->fb.width;
                        y = 159;
                        for (h=0 ; h<640 ; h+=4)
                        {
                            screenPtr[x + y*screenFbw->fb.width] = vgaPtr[h];
                            y--;
                        }
                        x++;
                    }
                    break;

                default:
                    // 640*480 --> 128*160, with a left rotation, with a vertical mirroring
                    x = 127;
                    y = 159;
                    for (v=0 ; v<32 ; v++)
                    {
                        for (j=0 ; j<3 ; j++)
                        {
                            vgaPtr = vgaPtr + 4* vgaFbw->fb.width;
                            y = 159;
                            for (h=0 ; h<640 ; h+=4)
                            {
                                screenPtr[x + y*screenFbw->fb.width] = vgaPtr[h];
                                y--;
                            }
                            x--;
                        }

                        vgaPtr = vgaPtr + 3* vgaFbw->fb.width;
                        y = 159;
                        for (h=0 ; h<640 ; h+=4)
                        {
                            screenPtr[x + y*screenFbw->fb.width] = vgaPtr[h];
                            y--;
                        }
                        x--;
                    }
                    break;
            }
            break;

        default:
            // TODO Will need further implementation in case
            // of other screen resolutions ...
            CAMS_ASSERT(FALSE,
                        "This screen format is not supported by "
                        "cams_ResizeVgaToScreen yet.");
            break;
    }

    CAMS_PROFILE_FUNCTION_EXIT(cams_ResizeVgaTo128x160);
}

// =============================================================================
// cams_SetCameraResolution
// -----------------------------------------------------------------------------
// =============================================================================
PROTECTED VOID cams_SetCameraResolution(LCDD_FBW_T* Fbw)
{
    CAMD_PARAM_RESOLUTION_T resolution;
    if (
        (Fbw->roi.width<=160) &&
        (Fbw->roi.height<=120) &&
        (g_camsAvailResolution & (1<<CAMD_PARAM_RESOLUTION_QQVGA))
    )
    {
        // Capture in QQVGA
        resolution = CAMD_PARAM_RESOLUTION_QQVGA;
    }
    else if
    (
        (Fbw->roi.width<=320) &&
        (Fbw->roi.height<=240) &&
        (g_camsAvailResolution & (1<<CAMD_PARAM_RESOLUTION_QVGA))
    )
    {
        // Capture in QVGA
        resolution = CAMD_PARAM_RESOLUTION_QVGA;
    }
    else if
    (
        (Fbw->roi.width<=640) &&
        (Fbw->roi.height<=480) &&
        (g_camsAvailResolution & (1<<CAMD_PARAM_RESOLUTION_VGA))
    )
    {
        resolution = CAMD_PARAM_RESOLUTION_VGA;
    }
    else
    {
        // Take the max resolution available
        if (g_camsAvailResolution & (1<<CAMD_PARAM_RESOLUTION_VGA))
        {
            resolution = CAMD_PARAM_RESOLUTION_VGA;
        }
        else if (g_camsAvailResolution & (1<<CAMD_PARAM_RESOLUTION_QVGA))
        {
            resolution = CAMD_PARAM_RESOLUTION_QVGA;
        }
        else if (g_camsAvailResolution & (1<<CAMD_PARAM_RESOLUTION_QVGA))
        {
            resolution = CAMD_PARAM_RESOLUTION_QQVGA;
        }
        else
        {
            resolution = CAMD_PARAM_RESOLUTION_VGA;
            CAMS_ASSERT(FALSE, "Camera does not support any resolution???");
        }
    }
    switch (resolution)
    {
        case CAMD_PARAM_RESOLUTION_QQVGA:
            g_camsCameraRawDataFbw.roi.width        = 160;
            g_camsCameraRawDataFbw.roi.height       = 120;
            g_camsCameraRawDataFbw.fb.width         = 160;
            g_camsCameraRawDataFbw.fb.height        = 120;
            break;
        case CAMD_PARAM_RESOLUTION_QVGA:
            g_camsCameraRawDataFbw.roi.width        = 320;
            g_camsCameraRawDataFbw.roi.height       = 240;
            g_camsCameraRawDataFbw.fb.width         = 320;
            g_camsCameraRawDataFbw.fb.height        = 240;
            break;
        case CAMD_PARAM_RESOLUTION_VGA:
            g_camsCameraRawDataFbw.roi.width        = 640;
            g_camsCameraRawDataFbw.roi.height       = 480;
            g_camsCameraRawDataFbw.fb.width         = 640;
            g_camsCameraRawDataFbw.fb.height        = 480;
            break;
        default:
            // unsupported resolution
            CAMS_ASSERT(FALSE, "Unsupported resolution by CAMD %d",resolution);
    }

    // Setup sensor
    camd_SetParam(CAMD_PARAM_RESOLUTION, resolution);
}

// =============================================================================
// cams_ZoomAndCrop
// -----------------------------------------------------------------------------
// Resize by dropping integer number of pixels
// Crop ther result to the desired roi
// =============================================================================
VOID cams_ZoomAndCrop(LCDD_FBW_T* SrcFbw, LCDD_FBW_T* DstFbw, UINT32 zoom)
{
    CAMS_PROFILE_FUNCTION_ENTER(cams_ZoomAndCrop);
    UINT16 x,y;
    UINT16* dstptr;
    UINT16* srcptr;
    UINT32 srcStartX, srcStartY;
    UINT32 incr = (1<<INC_PRECISION)*ZOOM_PRECISION/(zoom+1);  // add 1 to zoom to round up
    UINT32 incrX;
    UINT32 incrY;

    //CAMS_TRACE(TSTDOUT, 0, "cams_ZoomAndCrop %d",zoom);

    // set destination on the first pixel to write
    dstptr = DstFbw->fb.buffer + DstFbw->roi.y*DstFbw->fb.width+DstFbw->roi.x;
    // Start src at top left corner

    // Take the data to copy to the roi of the destination
    // from the center of the src buffer
    CAMS_TRACE(TSTDOUT, 0, "cams_ZoomAndCrop  sxc (%d x %d)",SrcFbw->fb.width,SrcFbw->fb.height );
    CAMS_TRACE(TSTDOUT, 0, "cams_ZoomAndCrop  dst (%d x %d)",DstFbw->roi.width,DstFbw->roi.height );
    CAMS_TRACE(TSTDOUT, 0, "cams_ZoomAndCrop  incr %d",incr );

    // Express src dimension in 2048th of pixels
    // Express needed src pixels to construct destination in 2048th of pixels
    // Calculate number of pixels not used to construct destination and divide by 2 to center
    // Make the result even
    srcStartX = ((((SrcFbw->fb.width<<INC_PRECISION)-(DstFbw->roi.width*incr))>>(INC_PRECISION+1))>>1)<<1 ;
    srcStartY = ((((SrcFbw->fb.height<<INC_PRECISION)-(DstFbw->roi.height*incr))>>(INC_PRECISION+1))>>1)<<1;
    CAMS_TRACE(TSTDOUT, 0, "cams_ZoomAndCrop %x StartX %d StartY %d",SrcFbw->fb.buffer, srcStartX, srcStartY);

    srcptr = SrcFbw->fb.buffer + srcStartY*SrcFbw->fb.width + srcStartX;

    // Do the copy
    incrY=0;
    for (y=0; y<DstFbw->roi.height; y++)
    {
        incrX=0;
        for (x=0; x<DstFbw->roi.width; x++)
        {
            UINT32 locincr = (incrX>>INC_PRECISION);
            locincr = locincr - (locincr&1) + ((locincr+1)&1);
            *(dstptr + x) = *(srcptr + locincr);
            incrX+= incr;
        }
        // go one line down
        dstptr += DstFbw->fb.width;
        // go incrY>>INC_PRECISION lines down
        srcptr = SrcFbw->fb.buffer + (incrY>>INC_PRECISION)*SrcFbw->fb.width;
        incrY += incr;
    }
    CAMS_PROFILE_FUNCTION_EXIT(cams_ZoomAndCrop);
}

// =============================================================================
// cams_ResizeRawToRequested
// -----------------------------------------------------------------------------
// =============================================================================
PUBLIC VOID cams_ResizeRawToRequested(  LCDD_FBW_T* SrcFbw,
                                        LCDD_FBW_T* DstFbw,
                                        UINT32      rotate,
                                        UINT32      zoom)
{
    CAMS_PROFILE_FUNCTION_ENTER(cams_ResizeRawToRequested);
    UINT32 scaleFactor;
    UINT32 scaleFactorX;
    UINT32 scaleFactorY;

    CAMS_TRACE(TSTDOUT, 0, "cams_Resize Src: %d x %d",SrcFbw->fb.width,SrcFbw->fb.height);
    CAMS_TRACE(TSTDOUT, 0, "cams_Resize Dst: %d x %d (Startx,Starty)=(%d,%d)",DstFbw->roi.width,DstFbw->roi.height,DstFbw->roi.x,DstFbw->roi.y);
    CAMS_TRACE(TSTDOUT, 0, "cams_Resize zoom: %d",zoom);

    scaleFactorX = ((zoom * DstFbw->roi.width) / SrcFbw->fb.width) ;
    scaleFactorY = ((zoom * DstFbw->roi.height) / SrcFbw->fb.height) ;

    // Select the scaling based on the best dimension
    if (scaleFactorX>scaleFactorY)
    {
        scaleFactor=scaleFactorX;
    }
    else
    {
        scaleFactor=scaleFactorY;
    }
    cams_ZoomAndCrop(SrcFbw,DstFbw,scaleFactor);
    CAMS_PROFILE_FUNCTION_EXIT(cams_ResizeRawToRequested);
}



