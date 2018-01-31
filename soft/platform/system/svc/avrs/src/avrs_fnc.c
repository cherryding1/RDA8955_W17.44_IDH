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

#include "avrs_m.h"
#include "camd_m.h"

#include "avrsp_task.h"
#include "avrsp.h"
#include "avrsp_debug.h"

#include "sxr_ops.h"



#define INC_PRECISION 11


// ==============================================================================
// avrs_Open
// ------------------------------------------------------------------------------
/// Open the camera service, and start the underlying task. This function must
/// be called before any other function of the camera service can be called.
// ==============================================================================
PUBLIC AVRS_ERR_T avrs_Open(VOID)
{
    AVRS_PROFILE_FUNCTION_ENTER(avrs_Open);
    AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs_Open");
    if(g_avrsIdTask == 0)
    {
        AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs_Open: Create task");
//        g_currentHandler    = 0;
        g_avrsTask.TaskBody  = avrs_Task;
        g_avrsTask.TaskExit  = 0;
        g_avrsTask.Name      = "AVRS";
        g_avrsTask.StackSize = AVRS_TASK_STACK_SIZE;
        g_avrsTask.Priority  = AVRS_TASK_PRIORITY;
        g_avrsMbx            = sxr_NewMailBox();
        g_avrsIdTask         = sxr_NewTask(&g_avrsTask);
        sxr_StartTask(g_avrsIdTask, 0);
    }
    camd_Open(g_avrsMbx);
    g_avrsStatus = AVRS_STATUS_OPEN;
    camd_GetInfo(&g_avrsAvailResolution, &g_avrsValidParam);
    if (g_avrsAvailResolution & (1<<CAMD_PARAM_RESOLUTION_VGA))
    {
        g_avrsCameraRawDataFbw.roi.width        = 640;
        g_avrsCameraRawDataFbw.roi.height       = 480;
        g_avrsCameraRawDataFbw.roi.x            = 0;
        g_avrsCameraRawDataFbw.roi.y            = 0;
        g_avrsCameraRawDataFbw.fb.width         = 640;
        g_avrsCameraRawDataFbw.fb.height        = 480;
        g_avrsCameraRawDataFbw.fb.colorFormat   = LCDD_COLOR_FORMAT_RGB_565;
        // FIXME Cannot do preview here at that resolution.
        g_avrsCameraRawDataFbw.fb.buffer        = sxr_Malloc(640*480*2);
        g_avrsCameraRawDataSize                 = 640*480*2;
    }
    else if (g_avrsAvailResolution & (1<<CAMD_PARAM_RESOLUTION_QVGA))
    {
        g_avrsCameraRawDataFbw.roi.width        = 320;
        g_avrsCameraRawDataFbw.roi.height       = 240;
        g_avrsCameraRawDataFbw.roi.x            = 0;
        g_avrsCameraRawDataFbw.roi.y            = 0;
        g_avrsCameraRawDataFbw.fb.width         = 320;
        g_avrsCameraRawDataFbw.fb.height        = 240;
        g_avrsCameraRawDataFbw.fb.colorFormat   = LCDD_COLOR_FORMAT_RGB_565;
        g_avrsCameraRawDataFbw.fb.buffer        = sxr_Malloc(320*240*2*2);
        g_avrsCameraRawDataSize                 = 320*240*2*2;
    }
    else if (g_avrsAvailResolution & (1<<CAMD_PARAM_RESOLUTION_QQVGA))
    {
        g_avrsCameraRawDataFbw.roi.width        = 160;
        g_avrsCameraRawDataFbw.roi.height       = 120;
        g_avrsCameraRawDataFbw.roi.x            = 0;
        g_avrsCameraRawDataFbw.roi.y            = 0;
        g_avrsCameraRawDataFbw.fb.width         = 160;
        g_avrsCameraRawDataFbw.fb.height        = 120;
        g_avrsCameraRawDataFbw.fb.colorFormat   = LCDD_COLOR_FORMAT_RGB_565;
        g_avrsCameraRawDataFbw.fb.buffer        = sxr_Malloc(160*120*2*2);
        g_avrsCameraRawDataSize                 = 160*120*2*2;
    }
    else
    {
        AVRS_TRACE(AVRS_WARN_TRC, 0, "avrs_Open : Camera resolution not supported");
        AVRS_PROFILE_FUNCTION_EXIT(avrs_Open);
        return AVRS_ERR_NO_DEVICE;
    }

    AVRS_PROFILE_FUNCTION_EXIT(avrs_Open);
    return AVRS_ERR_NO;
}


// =============================================================================
// avrs_Close
// -----------------------------------------------------------------------------
/// When the camera service is no more needed, this function closes the service
/// and put the task to sleep. No other function of the service can be called,
/// but the #avrs_Open function.
/// @return #AVRS_ERR_NO, #AVRS_ERR_RESOURCE_BUSY.
// =============================================================================
PUBLIC AVRS_ERR_T avrs_Close(VOID)
{
    AVRS_PROFILE_FUNCTION_ENTER(avrs_Close);
//    AVRS_ASSERT(g_avrsStatus == AVRS_STATUS_OPEN,
//            "avrs_Close can only be called when AVRS status is "
//            "AVRS_STATUS_OPEN, and not %d", g_avrsStatus);

    AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs_Close");

// FIXME: restore call to
// camd_Close();

    // Free capture buffer
    if (g_avrsCameraRawDataFbw.fb.buffer != NULL)
    {
        sxr_Free(g_avrsCameraRawDataFbw.fb.buffer);

        g_avrsCameraRawDataFbw.fb.buffer    =   NULL;
        g_avrsCameraRawDataSize             =   0;
    }
    g_avrsCameraRawDataFbw.roi.width        = 0;
    g_avrsCameraRawDataFbw.roi.height       = 0;
    g_avrsCameraRawDataFbw.roi.x            = 0;
    g_avrsCameraRawDataFbw.roi.y            = 0;
    g_avrsCameraRawDataFbw.fb.width         = 0;
    g_avrsCameraRawDataFbw.fb.height        = 0;
    g_avrsCameraRawDataFbw.fb.colorFormat   = LCDD_COLOR_FORMAT_RGB_565;
    g_avrsCameraRawDataFbw.fb.buffer        = NULL;
    g_avrsCameraRawDataSize             =   0;
    g_avrsStatus = AVRS_STATUS_CLOSE;
    // TODO: add some deactive task stuff ?
    AVRS_PROFILE_FUNCTION_EXIT(avrs_Close);
    return AVRS_ERR_NO;
}


// =============================================================================
// avrs_PictureStreamInProgress
// -----------------------------------------------------------------------------
/// Check if a preview is in progress.
/// @return \c TRUE if a preview is in progress, \c FALSE otherwise
// =============================================================================
PUBLIC BOOL avrs_PictureStreamInProgress(VOID)
{
    return (g_avrsStatus == AVRS_STATUS_PICTURE_STREAMING);
}


// =============================================================================
// avrs_StartPictureStream
// -----------------------------------------------------------------------------
/// TODO: Protect this function ?
/// Start the picture stream process. This is in fact a two way mechanism:
/// - a flow of raw picture is produced, to be used for preview features for
/// example.
/// - a flow of encoded picture is produced, to be used when recording a video
/// stream.
/// Either of those flows is optional: just set the corresponding parameter
/// to the \c NULL pointer value when calling avrs_StartPictureStream.
/// Typically, the camera preview would call this function that way:
/// <code> avrs_StartPictureStream(&previewPicture, NULL, &fbwStream, NULL)</code>
///
/// The frame buffer window and the encoded buffer are automatically refreshed
/// as long as the #avrs_StopPictureStream function is not called. To avoid
/// race conditions, swap buffers are used. The swap buffer used for the preview
/// frame buffer window and the encoded frame buffer are defined by the two
/// other parameters of the function: \c fbwStream and \c encBufStream.
///
/// Everytime a preview frame has been filled in the frame buffer window, the
/// corresponding <c>previewPicture->callback</c> handler is called. The same
/// way, every time an encoded picture has been encoded, the corresponding
/// <c>encodedFrame->callback</c> is called.
///
/// Please refer to the #AVRS_DECODED_PICTURE_T documentation for details
/// about how this structure packs together the frame buffer window where
/// the picture is put and the user handler used to display this frame buffer
/// window, and to #AVRS_ENCODED_PICTURE_T for the respective information on
/// the encoded picture.
///
/// @param previewPicture Preview picture. Its frame buffer window field should
/// be \c NULL, since it is ignored as the \c fbwStream parameter will provide
/// the successive frame buffer window to share during streaming.
/// @param encodedFrame Encoded frame. Its buffer field should
/// be \c NULL, since it is ignored as the \c fbwStream parameter will provide
/// the successive frame buffer window to share during streaming.
/// @param fbwStream Pointer to an array of frame buffer windows, defined by the
/// #AVRS_FBW_STREAM_T type. During streaming, all the frame buffer window
/// of that structure will be used successively as the value of the frame buffer
/// window of \c previewPicture.
/// @param encBufStream Pointer to an array of buffers, defined by the
/// #AVRS_ENC_BUFF_STREAM_T type. During streaming, all the buffers
/// of that structure will be used successively as the value of the \c buffer
/// field of \c encodedFrame.
///
/// @return #AVRS_ERR_NO, #AVRS_ERR_RESOURCE_BUSY, #AVRS_ERR_NO_DEVICE, ...
/// @todo How do we define the refresh rate ? (15fps/asap by tacit default)
// =============================================================================
PUBLIC AVRS_ERR_T avrs_StartPictureStream(AVRS_DECODED_PICTURE_T*   previewPicture,
        AVRS_ENCODED_PICTURE_T*   encodedFrame,
        AVRS_FBW_STREAM_T*        fbwStream,
        AVRS_ENC_BUFFER_STREAM_T* encBufStream)
{
    AVRS_PROFILE_FUNCTION_ENTER(avrs_StartPictureStream);
    AVRS_ASSERT(g_avrsIdTask != 0, "avrs_Open must be called before avrs_StartPictureStream");
    AVRS_ASSERT((g_avrsStatus == AVRS_STATUS_OPEN)
                || (g_avrsStatus == AVRS_STATUS_PICTURE_STREAMING),
                "avrs_StartPictureStream can only be called when AVRS status is "
                "AVRS_STATUS_OPEN, and not %d", g_avrsStatus);
    AVRS_TRACE(AVRS_INFO_TRC, 1, "In %s function.", __FUNCTION__);

    AVRS_MSG_PICTURE_STREAM_T* previewMsg = sxr_Malloc(sizeof(AVRS_MSG_PICTURE_STREAM_T));
    previewMsg->headerId        = AVRS_OP_ID_START_PICTURE_STREAM;
    previewMsg->previewPicture  = previewPicture;
    previewMsg->encodedFrame    = encodedFrame;
    // FIXME Global variable use is bad.
    previewMsg->resolution      = g_avrsVideoStream.resolution;
    previewMsg->fbwStream       = fbwStream;
    previewMsg->encBufStream    = encBufStream;
    previewMsg->streamDepth     = AVRS_PICTURE_STREAM_DEPTH;

    sxr_Send(previewMsg, g_avrsMbx, SXR_SEND_MSG);
    AVRS_PROFILE_FUNCTION_EXIT(avrs_StartPictureStream);
    return AVRS_ERR_NO;
}


// =============================================================================
// avrs_StopPictureStream
// -----------------------------------------------------------------------------
/// Stop the preview process started by #avrs_StartPictureStream.
/// When the preview is stopped, the #AVRS_MSG_PICTURE_STREAM_STOPPED message is sent
/// to the mailbox registered by #avrs_Open.
///
/// @return #AVRS_ERR_NO.
// =============================================================================
PUBLIC AVRS_ERR_T avrs_StopPictureStream(VOID)
{
    AVRS_PROFILE_FUNCTION_ENTER(avrs_StopPictureStream);
    AVRS_ASSERT(g_avrsIdTask != 0, "avrs_Open must be called before avrs_StopPictureStream");
    AVRS_TRACE(AVRS_INFO_TRC, 1, "In %s function.", __FUNCTION__);
//    AVRS_ASSERT((g_avrsStatus == AVRS_STATUS_OPEN)
//            || (g_avrsStatus == AVRS_STATUS_PICTURE_STREAMING),
//            "avrs_StopPictureStream can only be called when AVRS status is "
//            "AVRS_STATUS_OPEN, and not %d", g_avrsStatus);

    AVRS_MSG_T* stopMsg = sxr_Malloc(sizeof(AVRS_MSG_T));
    stopMsg->headerId = AVRS_OP_ID_STOP_PICTURE_STREAM;
    stopMsg->status = 0;
    sxr_Send(stopMsg, g_avrsMbx, SXR_SEND_MSG);
    AVRS_PROFILE_FUNCTION_EXIT(avrs_StopPictureStream);
    return AVRS_ERR_NO;
}


// =============================================================================
// avrs_Capture
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
/// @return #AVRS_ERR_NO, #AVRS_ERR_RESOURCE_BUSY, #AVRS_ERR_NO_DEVICE, ...
// =============================================================================
PUBLIC AVRS_ERR_T avrs_Capture( LCDD_FBW_T*             dataFbw,
                                LCDD_FBW_T*             prvwFbw,
                                AVRS_CAPTURE_HANDLER_T  captureHandler)
{
    AVRS_PROFILE_FUNCTION_ENTER(avrs_Capture);
    AVRS_ASSERT(g_avrsIdTask != 0, "avrs_Open must be called before avrs_Capture");
    AVRS_ASSERT(g_avrsStatus == AVRS_STATUS_OPEN,
                "avrs_Capture can only be called when AVRS status is "
                "AVRS_STATUS_OPEN, and not %d", g_avrsStatus);
    AVRS_TRACE(AVRS_INFO_TRC, 1, "In %s function.", __FUNCTION__);
    AVRS_MSG_CAPTURE_T* captureMsg = sxr_Malloc(sizeof(AVRS_MSG_CAPTURE_T));
    captureMsg->headerId = AVRS_OP_ID_START_CAPTURE;
    captureMsg->dataFbw = dataFbw;
    captureMsg->prvwFbw = prvwFbw;
    captureMsg->captureHandler = captureHandler;

    sxr_Send(captureMsg, g_avrsMbx, SXR_SEND_MSG);
    AVRS_PROFILE_FUNCTION_EXIT(avrs_Capture);
    return AVRS_ERR_NO;
}

# if 0
// FIXME Do something to set params to configure the video stuff

// =============================================================================
// avrs_SetParam
// -----------------------------------------------------------------------------
/// Set a camera parameter to a given value.
/// The parameter to set is among the values defined by the type #AVRS_PARAM_T.
/// If this parameter is not available for a given implementation of AVRS,
/// the #AVRS_ERR_UNSUPPORTED_PARAM error must be returned.
/// Depending on the AVRS_PARAM_XXX parameter to set, the value can either
/// be a proper UINT32 number, or a value of the AVRS_PARAM_XXX_T enum, might
/// it exists. This is detailed in the AVRS_PARAM_T definition.
/// If the value is not supported for this parameter, #AVRS_ERR_UNSUPPORTED_VALUE
/// is returned.
///
/// @param param Camera parameter to set.
/// @param value Value to set to the parameter to.
/// @return #AVRS_ERR_NO, #AVRS_ERR_UNSUPPORTED_PARAM, #AVRS_ERR_UNSUPPORTED_VALUE
/// ...
// =============================================================================
PUBLIC AVRS_ERR_T avrs_SetParam(AVRS_PARAM_T param, UINT32 value)
{
    AVRS_ASSERT(g_avrsIdTask != 0, "avrs_Open must be called before avrs_SetParam");
    AVRS_ASSERT(g_avrsStatus != AVRS_STATUS_CLOSE,
                "avrs_SetParam can only be called when AVRS is opened");
    // Let's hope we are keeping that compatibility properly
    AVRS_TRACE(AVRS_INFO_TRC, 0, "cams_SetParam %d, %d",param, value);
    switch (param)
    {
        case AVRS_PARAM_DIGITAL_ZOOM:
            // Zoom is (100+20*value)
            // value from 0 to 5 => zoomin from 100% to 200%
            g_avrsDigitalZoomFactor = (5+value)*ZOOM_PRECISION/5;
            return 0;
        default:
            // By default we pass it directly to CAMD
            // FIXME: restore call to
            // return camd_SetParam(param, value);
    }

}
#endif

// =============================================================================
// avrs_ResizeVgaTo128x160
// -----------------------------------------------------------------------------
/// Fast resize of a VGA frame buffer window into a screen size frame
/// buffer window
/// @param vgaFbw VGA source frame buffer window.
/// @param screenFbw Screem frame buffer window.
/// @param rotate Do we need to do a rotation of the screen?
// =============================================================================
PUBLIC VOID avrs_ResizeVgaTo128x160( LCDD_FBW_T* vgaFbw,
                                     LCDD_FBW_T* screenFbw,
                                     UINT32      rotate)
{
    AVRS_PROFILE_FUNCTION_ENTER(avrs_ResizeVgaTo128x160);
    AVRS_ASSERT(((vgaFbw->fb.width == 640)
                 && (vgaFbw->fb.height == 480)
                 && (vgaFbw->fb.colorFormat == LCDD_COLOR_FORMAT_RGB_565)
                 && (vgaFbw->roi.width == 640)
                 && (vgaFbw->roi.height == 480))
                || ((vgaFbw->fb.width == 480)
                    && (vgaFbw->fb.height == 640)
                    && (vgaFbw->fb.colorFormat == LCDD_COLOR_FORMAT_RGB_565)
                    && (vgaFbw->roi.width == 480)
                    && (vgaFbw->roi.height == 640)),
                "avrs_ResizeVgaToScreen only handle VGA buffers "
                " in input");
    switch (screenFbw->fb.width)
    {
        case 128:
            // TODO Will need further implementation in case
            // of other screen resolutions ...
            AVRS_ASSERT((screenFbw->fb.height == 160)
                        && (screenFbw->fb.colorFormat == LCDD_COLOR_FORMAT_RGB_565)
                        && (screenFbw->roi.width == 128)
                        && (screenFbw->roi.height == 160),
                        "This screen format is not supported by "
                        "avrs_ResizeVgaToScreen yet.");

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
            AVRS_ASSERT(FALSE,
                        "This screen format is not supported by "
                        "avrs_ResizeVgaToScreen yet.");
            break;
    }

    AVRS_PROFILE_FUNCTION_EXIT(avrs_ResizeVgaTo128x160);
}

// =============================================================================
// avrs_SetCameraResolution
// -----------------------------------------------------------------------------
// =============================================================================
PROTECTED VOID avrs_SetCameraResolution(LCDD_FBW_T* Fbw)
{
    CAMD_PARAM_RESOLUTION_T resolution;
    if (
        (Fbw->roi.width<=160) &&
        (Fbw->roi.height<=120) &&
        (g_avrsAvailResolution & (1<<CAMD_PARAM_RESOLUTION_QQVGA))
    )
    {
        // Capture in QQVGA
        resolution = CAMD_PARAM_RESOLUTION_QQVGA;
    }
    else if
    (
        (Fbw->roi.width<=320) &&
        (Fbw->roi.height<=240) &&
        (g_avrsAvailResolution & (1<<CAMD_PARAM_RESOLUTION_QVGA))
    )
    {
        // Capture in QVGA
        resolution = CAMD_PARAM_RESOLUTION_QVGA;
    }
    else if
    (
        (Fbw->roi.width<=640) &&
        (Fbw->roi.height<=480) &&
        (g_avrsAvailResolution & (1<<CAMD_PARAM_RESOLUTION_VGA))
    )
    {
        resolution = CAMD_PARAM_RESOLUTION_VGA;
    }
    else
    {
        // Take the max resolution available
        if (g_avrsAvailResolution & (1<<CAMD_PARAM_RESOLUTION_VGA))
        {
            resolution = CAMD_PARAM_RESOLUTION_VGA;
        }
        else if (g_avrsAvailResolution & (1<<CAMD_PARAM_RESOLUTION_QVGA))
        {
            resolution = CAMD_PARAM_RESOLUTION_QVGA;
        }
        else if (g_avrsAvailResolution & (1<<CAMD_PARAM_RESOLUTION_QVGA))
        {
            resolution = CAMD_PARAM_RESOLUTION_QQVGA;
        }
        else
        {
            resolution = CAMD_PARAM_RESOLUTION_VGA;
            AVRS_ASSERT(FALSE, "Camera does not support any resolution???");
        }
    }
    switch (resolution)
    {
        case CAMD_PARAM_RESOLUTION_QQVGA:
            g_avrsCameraRawDataFbw.roi.width        = 160;
            g_avrsCameraRawDataFbw.roi.height       = 120;
            g_avrsCameraRawDataFbw.fb.width         = 160;
            g_avrsCameraRawDataFbw.fb.height        = 120;
            break;
        case CAMD_PARAM_RESOLUTION_QVGA:
            g_avrsCameraRawDataFbw.roi.width        = 320;
            g_avrsCameraRawDataFbw.roi.height       = 240;
            g_avrsCameraRawDataFbw.fb.width         = 320;
            g_avrsCameraRawDataFbw.fb.height        = 240;
            break;
        case CAMD_PARAM_RESOLUTION_VGA:
            g_avrsCameraRawDataFbw.roi.width        = 640;
            g_avrsCameraRawDataFbw.roi.height       = 480;
            g_avrsCameraRawDataFbw.fb.width         = 640;
            g_avrsCameraRawDataFbw.fb.height        = 480;
            break;
        default:
            // unsupported resolution
            AVRS_ASSERT(FALSE, "Unsupported resolution by CAMD %d",resolution);
    }

    // Setup sensor
    camd_SetParam(CAMD_PARAM_RESOLUTION, resolution);
}

// =============================================================================
// avrs_ZoomAndCrop
// -----------------------------------------------------------------------------
// Resize by dropping integer number of pixels
// Crop ther result to the desired roi
// =============================================================================
VOID avrs_ZoomAndCrop(LCDD_FBW_T* SrcFbw, LCDD_FBW_T* DstFbw, UINT32 zoom)
{
    AVRS_PROFILE_FUNCTION_ENTER(avrs_ZoomAndCrop);
    UINT16 x,y;
    UINT16* dstptr;
    UINT16* srcptr;
    UINT32 srcStartX, srcStartY;
    UINT32 incr = (1<<INC_PRECISION)*ZOOM_PRECISION/(zoom+1);  // add 1 to zoom to round up
    UINT32 incrX;
    UINT32 incrY;

    //AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs_ZoomAndCrop %d",zoom);

    // set destination on the first pixel to write
    dstptr = DstFbw->fb.buffer + DstFbw->roi.y*DstFbw->fb.width+DstFbw->roi.x;
    // Start src at top left corner

    // Take the data to copy to the roi of the destination
    // from the center of the src buffer
    AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs_ZoomAndCrop  sxc (%d x %d)",SrcFbw->fb.width,SrcFbw->fb.height );
    AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs_ZoomAndCrop  dst (%d x %d)",DstFbw->roi.width,DstFbw->roi.height );
    AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs_ZoomAndCrop  incr %d",incr );

    // Express src dimension in 2048th of pixels
    // Express needed src pixels to construct destination in 2048th of pixels
    // Calculate number of pixels not used to construct destination and divide by 2 to center
    // Make the result even
    srcStartX = ((((SrcFbw->fb.width<<INC_PRECISION)-(DstFbw->roi.width*incr))>>(INC_PRECISION+1))>>1)<<1 ;
    srcStartY = ((((SrcFbw->fb.height<<INC_PRECISION)-(DstFbw->roi.height*incr))>>(INC_PRECISION+1))>>1)<<1;
    AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs_ZoomAndCrop %x StartX %d StartY %d",SrcFbw->fb.buffer, srcStartX, srcStartY);

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
    AVRS_PROFILE_FUNCTION_EXIT(avrs_ZoomAndCrop);
}

// =============================================================================
// avrs_ResizeRawToRequested
// -----------------------------------------------------------------------------
// =============================================================================
PUBLIC VOID avrs_ResizeRawToRequested(  LCDD_FBW_T* SrcFbw,
                                        LCDD_FBW_T* DstFbw,
                                        UINT32      rotate,
                                        UINT32      zoom)
{
    AVRS_PROFILE_FUNCTION_ENTER(avrs_ResizeRawToRequested);
    UINT32 scaleFactor;
    UINT32 scaleFactorX;
    UINT32 scaleFactorY;

    AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs_Resize Src: %d x %d",SrcFbw->fb.width,SrcFbw->fb.height);
    AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs_Resize Dst: %d x %d (Startx,Starty)=(%d,%d)",DstFbw->roi.width,DstFbw->roi.height,DstFbw->roi.x,DstFbw->roi.y);
    AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs_Resize zoom: %d",zoom);

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
    avrs_ZoomAndCrop(SrcFbw,DstFbw,scaleFactor);
    AVRS_PROFILE_FUNCTION_EXIT(avrs_ResizeRawToRequested);
}



