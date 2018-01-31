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

// for OS timers
#include "sxr_tim.h"
#include "sxr_tls.h"
#include "sxr_tksd.h"



#include "avrs_m.h"
#include "camd_m.h"
#include "imgs_m.h"

// FIXME Reenable that thing if needed
//#include "avrs_config.h"
#include "avrsp.h"
#include "avrsp_task.h"
#include "avrsp_debug.h"

#include "hal_sys.h"
#include "hal_clk.h"
#include "hal_debug.h"
#include "vpp_amjr.h"

#ifdef AVRS_SHOW_FPS
// Enable the display of the framerate on the
// screen and/or traces.
// NOTA: the delay timer wrap is not taken into account,
// as it will only make one FPS estimation false.
#include "fmg_m.h"
#include "hal_timers.h"
#define AVRS_FPS_MEAS_FRAME_NUMBER       5

#endif

#include "hal_debug.h"



// =============================================================================
// PROTECTED VARIABLES
// =============================================================================
#define AVRS_MAX_FAILED_CAPTURE 15

PROTECTED sxr_TaskDesc_t    g_avrsTask;
PROTECTED UINT8             g_avrsIdTask        = 0;
PROTECTED UINT8             g_avrsMbx           = 0xFF;
PROTECTED AVRS_STATUS_T     g_avrsStatus        = AVRS_STATUS_CLOSE;
PROTECTED UINT32            g_avrsAvailResolution = 0;
PROTECTED UINT32            g_avrsValidParam    = 0;

PROTECTED UINT32            g_avrsRotate = 0;
PROTECTED BOOL              g_avrsPreviewAllFrames = TRUE;
PROTECTED UINT32 g_avrsDigitalZoomFactor = ZOOM_PRECISION;
LCDD_FBW_T g_avrsCameraRawDataFbw;
UINT32     g_avrsCameraRawDataSize = 0;

// This macro initializes all the state variables from the AVRS
// task state machine.
// We keep it here so that we can think more easily of
// adding any new variable to it
// FIXME Check the up-to-date state of that reset macro.
#define AVRS_TASK_RESET_STATE                       \
    pictureStreamMode               = FALSE;        \
    stopPictureStreamRequested      = FALSE;        \
    restartPictureStreamRequested   = FALSE;        \
    captureHandler                  = NULL;         \
    captureFbw                      = NULL;         \
    previewFbw                      = NULL;         \
    failedCapture                   = 0;            \
    pictureProcessingInProgress     = FALSE;        \


// ==============================================================================
// avrs_Task
// ------------------------------------------------------------------------------
/// AVRS task.
/// @param param ...
/// @todo Do we need a parameter here ?
// ==============================================================================
PROTECTED VOID avrs_Task(VOID* param)
{
    // FIXME Wait for newest CAMD in trunk
    // Message accessors
    UINT32                      evt[4];
    AVRS_MSG_T*                 msg                 = NULL;
    AVRS_MSG_CAPTURE_T*         captureMsg          = NULL;
    AVRS_MSG_PICTURE_STREAM_T*  pictureStreamMsg    = NULL;
    CAMD_MSG_T*                 camdMsg             = NULL;

    // state variables
    BOOL        pictureStreamMode;
    BOOL        stopPictureStreamRequested;
    BOOL        restartPictureStreamRequested;
    // With status is set as long a picture from
    // the camera is being processed (encode and
    // resize for preview). A new picture arriving
    // when this is set to TRUE will be ignored.
    BOOL        pictureProcessingInProgress = FALSE;

    // Operational configuration
    AVRS_DECODED_PICTURE_T*     previewPicture  = NULL;
    AVRS_ENCODED_PICTURE_T*     encodedFrame    = NULL;
    AVRS_FBW_STREAM_T*          fbwStream       = NULL;
    AVRS_ENC_BUFFER_STREAM_T*   encBufStream    = NULL;
    UINT32                      streamDepth     = 0;
    UINT32                      currentDepth    = 0;

    AVRS_CAPTURE_HANDLER_T captureHandler;
    LCDD_FBW_T* captureFbw;
    LCDD_FBW_T* previewFbw;
    UINT8 failedCapture = 0;
    UINT16*     cameraBufferOrigin = NULL;



    AVRS_TASK_RESET_STATE;

#ifdef AVRS_SHOW_FPS
    UINT32 elapsedTime = 1;
    UINT32 fps = 1;
    UINT32 fpsStartMeasTime   = 0;
    UINT32 fpsElapsedFrame    = 0;

#endif
    UINT32 csStatus;

    while (1)
    {
        AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs task: waiting for a message");
        msg = sxr_Wait(evt, g_avrsMbx);

        if (msg == NULL)
        {
            // We received an event
            // Do nothing
            AVRS_TRACE(AVRS_WARN_TRC, 0, "avrs task: What is this event ? :%x", evt[0]);
        }
        else
        {
            // We received a message
            switch (msg->headerId)
            {
                case AVRS_OP_ID_START_PICTURE_STREAM:
                    AVRS_PROFILE_WINDOW_ENTER(avrs_PictureStreamWin);
                    if (!pictureStreamMode)
                    {
                        AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs task: start preview, first time");
                        // This is like a reinitialization of the state machine:
                        AVRS_TASK_RESET_STATE;
                        AVRS_PROFILE_WINDOW_EXIT(avrs_PictureProcessingInProgress);
#ifdef AVRS_SHOW_FPS
                        fpsStartMeasTime   = 0;
                        fpsElapsedFrame    = 0;
#endif


                        // We need a high frequency to sustain the throughput needed

                        hal_SwRequestClk(FOURCC_AVRS, HAL_SYS_FREQ_104M);
                        stopPictureStreamRequested = FALSE;
                        pictureStreamMode = TRUE;
                        g_avrsStatus = AVRS_STATUS_PICTURE_STREAMING;
                        pictureStreamMsg    = (AVRS_MSG_PICTURE_STREAM_T*)msg;
                        previewPicture      = pictureStreamMsg->previewPicture;
                        encodedFrame        = pictureStreamMsg->encodedFrame;
                        fbwStream           = pictureStreamMsg->fbwStream;
                        encBufStream        = pictureStreamMsg->encBufStream;
                        streamDepth         = pictureStreamMsg->streamDepth;

                        // Select camera resolution based on requested preview
                        // or record size.
                        if (encodedFrame)
                        {
                            // Record defines the size.
                            // Create a pseudo FBW to define the resolution to
                            // set.
                            // TODO: Take rotation into account.
                            LCDD_FBW_T pseudoFbw;
                            switch (pictureStreamMsg->resolution)
                            {
                                case AVRS_REC_VIDEO_RESOLUTION_QQVGA:
                                    pseudoFbw.fb.width      = 160;
                                    pseudoFbw.fb.height     = 120;
                                    pseudoFbw.roi.width     = 160;
                                    pseudoFbw.roi.height    = 120;
                                    break;

                                case AVRS_REC_VIDEO_RESOLUTION_QVGA:
                                    pseudoFbw.fb.width      = 320;
                                    pseudoFbw.fb.height     = 240;
                                    pseudoFbw.roi.width     = 320;
                                    pseudoFbw.roi.height    = 240;
                                    break;
                            }

                            avrs_SetCameraResolution(&pseudoFbw);
                        }
                        else
                        {
                            // Preview define sizes.
                            avrs_SetCameraResolution(previewPicture->fbw);
                        }

                        // Set Format
                        switch (previewPicture->fbw->fb.colorFormat)
                        {
                            case LCDD_COLOR_FORMAT_RGB_565:
                                camd_SetParam(CAMD_PARAM_FORMAT, CAMD_PARAM_FORMAT_RGB565);
                                break;

                            default:
                                AVRS_ASSERT(FALSE, "Only RGB 565 is supported by AVRS");
                                break;
                        }

                        // We are skipping odd frames, so ask for twice the frame rate.
                        camd_SetParam(CAMD_PARAM_FRAME_RATE, g_avrsVideoStream.frameRate*2);

                        // Configure swap buffer
                        currentDepth = 0;
                        if (fbwStream && previewPicture)
                        {
                            previewPicture->fbw     = &((*fbwStream)[currentDepth]);
                        }
                        else
                        {
                            // TODO Check the usefulness of that else statement.
                        }

                        if (encBufStream && encodedFrame)
                        {
                            encodedFrame->buffer    = (*encBufStream)[currentDepth];
                        }
                        else
                        {
                            // TODO Check the usefulness of that else statement.
                        }


                    }

                    // Get image
                    AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs task: preview: Take capture");
                    // Save the mallocated pointer of the buffer to restore it
                    // when we stop. (This pointer is freed by avrs_Close)
                    cameraBufferOrigin = g_avrsCameraRawDataFbw.fb.buffer;
                    // Start stream, skipping odd frames.
                    camd_ImageStreamStart(g_avrsCameraRawDataFbw.fb.buffer, g_avrsCameraRawDataSize, TRUE);
                    break;


                case AVRS_OP_ID_STOP_PICTURE_STREAM:
                    AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs task: Stop preview");
                    // The critical section here aims at having
                    // the new behaviour of CAMD interrupt handler
                    // and of the AVRS task for the same camera
                    // VSYNC interrupt.
                    csStatus = hal_SysEnterCriticalSection();
                    // Stop the picture stream
                    camd_ImageStreamStop();
                    // Prevent restarting
                    stopPictureStreamRequested = TRUE;
                    hal_SysExitCriticalSection(csStatus);

                    if (!pictureStreamMode)
                    {
                        // No stream is in progress, clear the status here
                        AVRS_TRACE(AVRS_INFO_TRC, 0, "Streams freed @ stop message reception.");
                        avrs_FreeStreams();
                        g_avrsStatus = AVRS_STATUS_OPEN;
                    }
                    break;


                case AVRS_OP_ID_START_CAPTURE:
                    if (!pictureStreamMode)
                    {
                        AVRS_PROFILE_WINDOW_ENTER(avrs_CaptureWin);
                        AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs task: Capture Picture");
                        // We need a high frequency to sustain the throughput needed

                        hal_SwRequestClk(FOURCC_AVRS, HAL_SYS_FREQ_104M);
                        captureMsg = (AVRS_MSG_CAPTURE_T*)msg;
                        captureFbw = captureMsg->dataFbw;
                        previewFbw = captureMsg->prvwFbw;
                        captureHandler = captureMsg->captureHandler;
                        pictureStreamMode = FALSE;
                        // We are not in stream mode, but in the process
                        // of handling a frame. This status is used to
                        // mean we are expcting pictures (cf test for
                        // the AVRS_OP_ID_CAPTURE_DONE message).
                        g_avrsStatus = AVRS_STATUS_PICTURE_STREAMING;

                        AVRS_ASSERT(previewFbw->fb.colorFormat == LCDD_COLOR_FORMAT_RGB_565,
                                    "Only RGB 565 is supported by AVRS");

                        // Set Format
                        switch (captureFbw->fb.colorFormat)
                        {
                            case LCDD_COLOR_FORMAT_RGB_565:
                                camd_SetParam(CAMD_PARAM_FORMAT, CAMD_PARAM_FORMAT_RGB565);
                                break;

                            default:
                                AVRS_ASSERT(FALSE, "Only RGB 565 is supported by AVRS");
                                break;
                        }

                        // Select camera resolution based on requested capture
                        avrs_SetCameraResolution(captureFbw);

                        // Save the mallocated pointer of the buffer to restore it
                        // when we stop. (This pointer is freed by avrs_Close)
                        cameraBufferOrigin = g_avrsCameraRawDataFbw.fb.buffer;
                        camd_GetImage(g_avrsCameraRawDataFbw.fb.buffer);
                    }
                    else
                    {
                        // Return an error ?
                        AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs task: Capture while in preview, skip !");
                    }
                    break;

                case AVRS_OP_ID_CAPTURE_DONE:
                    if (pictureProcessingInProgress)
                    {
                        // Drop that frame
                        AVRS_TRACE(AVRS_INFO_TRC, 0, "Picture processing in progress, frame dropped.");
                    }
                    else if (g_avrsStatus != AVRS_STATUS_PICTURE_STREAMING)
                    {
                        // Unexcpected message, we are not streaming picture
                        // This happens when we are stopping the stream, the camera
                        // has sent picture before it was stopped, but while
                        // we are processing the previous image. We don't want
                        // this picture, as global variables have been deallocated,
                        // and we stopped ...
                        AVRS_TRACE(AVRS_INFO_TRC, 0, "Picture received while not in streaming mode, frame ignored.");
                    }
                    else
                    {
                        camdMsg = (CAMD_MSG_T*) msg;
                        if (camdMsg->status == CAMD_STATUS_CAPTURE_DONE)
                        {
                            AVRS_PROFILE_WINDOW_ENTER(avrs_HandleCaptureDoneEvent);
                            // Process frame
                            AVRS_PROFILE_WINDOW_ENTER(avrs_PictureProcessingInProgress);
                            pictureProcessingInProgress = TRUE;

                            g_avrsCameraRawDataFbw.fb.buffer = (UINT16*) camdMsg->imgPtr;
                            failedCapture = 0;
                            if (pictureStreamMode == TRUE)
                            {
                                AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs task: PictureStream captured one frame");
                                UINT32* encodedOutput = NULL;
                                UINT32* previewOutput = NULL;
                                HAL_ERR_T vppErr = HAL_ERR_NO;

                                // Define encoder inputs
                                if (encodedFrame && g_avrsVideoCfg.recordOn)
                                {
                                    encodedOutput = (UINT32*)encodedFrame->buffer;
                                }

                                if (previewPicture && g_avrsVideoCfg.previewOn)
                                {
                                    previewOutput = (UINT32*)previewPicture->fbw->fb.buffer;
                                }


                                // Call the function to:
                                //  - Resize the picture for the display (AVRS user) handler and call
                                //  the handler with it.
                                //  - Resize and encode the picture for the
                                // NOTA: If one of those
                                // is NULL, the corresponding operation is not performed.
                                // NOTA bis: AMJR picture encoding configuration is done once
                                // in avrs_Record();

                                // Schedule the encoding and picture resizing.
                                vppErr = vpp_AmjrScheduleOneVideoFrame((UINT32*)g_avrsCameraRawDataFbw.fb.buffer,
                                                                       encodedOutput, previewOutput);
                                AVRS_TRACE(AVRS_INFO_TRC, 0, "vpp_AmjrScheduleOneVideoFrame status :%d", vppErr);
#if 0
                                // FIXME This is a manual resize because voc code is not nice enough.
                                // g_avrsCameraRawDataFbw.fb.buffer
                                {
                                    // We only need to rotate.
                                    UINT32 x = 0;
                                    UINT32 y = 0;
                                    for (y=0; y<120 ; y++)
                                    {
                                        for (x=0; x<128 ; x++)
                                        {
                                            if (x&1)
                                            {
                                                previewPicture->fbw->fb.buffer[x+y*128] = g_avrsCameraRawDataFbw.fb.buffer[(x-1)+y*160];
                                            }
                                            else
                                            {
                                                previewPicture->fbw->fb.buffer[x+y*128] = g_avrsCameraRawDataFbw.fb.buffer[(x+1)+y*160];
                                            }
                                        }
                                    }
                                }
                                // END OF FIXME
#endif

#ifdef AVRS_SHOW_FPS
                                // Display FPS
                                fpsElapsedFrame++;
                                if (fpsElapsedFrame >= AVRS_FPS_MEAS_FRAME_NUMBER)
                                {
                                    elapsedTime = (hal_TimGetUpTime()-fpsStartMeasTime);
                                    fps = (1 SECOND * fpsElapsedFrame)/(elapsedTime?elapsedTime:1);

                                    AVRS_TRACE(AVRS_INFO_TRC, 0, "FPS: elapsed frame: %d", fpsElapsedFrame);
                                    AVRS_TRACE(AVRS_INFO_TRC, 0, "FPS: elapsed time: %d", elapsedTime);
                                    AVRS_TRACE(AVRS_INFO_TRC, 0, "FPS: fps: %d", fps);
                                    // Prepare next run
                                    fpsElapsedFrame = 0;
                                    fpsStartMeasTime = hal_TimGetUpTime();
                                }

#endif
                            }
                            else
                            {
                                AVRS_PROFILE_WINDOW_EXIT(avrs_CaptureWin);
                                AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs task: Capture captured one frame");
                                // Resize taken picture to fit in the preview zone
                                avrs_ResizeRawToRequested(&g_avrsCameraRawDataFbw, previewFbw, g_avrsRotate, g_avrsDigitalZoomFactor);
                                // Resize taken picture to fit in the capture zone
                                avrs_ResizeRawToRequested(&g_avrsCameraRawDataFbw, captureFbw, g_avrsRotate, g_avrsDigitalZoomFactor);

                                // Call capture handler
                                if (captureHandler)
                                {
                                    captureHandler(captureFbw, previewFbw);
                                }

                                // We don't need a high frequency anymore

                                hal_SwReleaseClk(FOURCC_AVRS);
                                // Reset the buffer pointer by g_avrsCameraRawDataFbw.fb.buffer
                                // to its initial value to allow free to liberate the buffer.
                                g_avrsCameraRawDataFbw.fb.buffer = cameraBufferOrigin;
                            }
                            AVRS_PROFILE_WINDOW_EXIT(avrs_HandleCaptureDoneEvent);
                        }
                        else if (camdMsg->status == CAMD_STATUS_CAPTURE_ERROR)
                        {
                            failedCapture++;
                            if (pictureStreamMode == TRUE)
                            {
                                AVRS_PROFILE_WINDOW_EXIT(avrs_PictureStreamWin);
                                AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs task: PictureStream failed to capture one frame, retry, progress being:%d",
                                           pictureProcessingInProgress);
                                if (stopPictureStreamRequested)
                                {
                                    AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs task: Stopping preview");
                                    stopPictureStreamRequested = FALSE;
                                    pictureStreamMode = FALSE;
                                    // We don't need a high frequency anymore

                                    hal_SwReleaseClk(FOURCC_AVRS);
                                    // Clear the strean variables here.
                                    AVRS_TRACE(AVRS_INFO_TRC, 0, "Streams freed @ one failure.");
                                    avrs_FreeStreams();
                                    // Free event and message possibly waiting
                                    // (camera might have taken other shots)

                                    g_avrsStatus = AVRS_STATUS_OPEN;
                                }
                                else
                                {
                                    if (failedCapture<AVRS_MAX_FAILED_CAPTURE)
                                    {
                                        if (failedCapture == AVRS_MAX_FAILED_CAPTURE-1)
                                        {
                                            // Image streaming need one picture (latency)
                                            // to stop. Hypothesis: if we got AVRS_MAX_FAILED_CAPTURE-1
                                            // failed pictures, we will get a AVRS_MAX_FAILED_CAPTURE th.
                                            camd_ImageStreamStop();
                                            stopPictureStreamRequested = TRUE;
                                        }
                                    }
                                    else
                                    {
                                        AVRS_TRACE(AVRS_WARN_TRC, 0, "avrs task: Stopping preview, Too many errors");
                                        stopPictureStreamRequested = FALSE;
                                        pictureStreamMode = FALSE;
                                        // We don't need a high frequency anymore

                                        hal_SwReleaseClk(FOURCC_AVRS);
                                        // Clear the strean variables here.
                                        AVRS_TRACE(AVRS_INFO_TRC, 0, "Streams freed @ too many failures.");
                                        avrs_FreeStreams();
                                        g_avrsStatus = AVRS_STATUS_OPEN;
                                        // TODO should send an error message
                                    }
                                }
                            }
                            else
                            {
                                AVRS_PROFILE_WINDOW_EXIT(avrs_CaptureWin);
                                AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs task: Capture failed to capture one frame,retry");
                                if (failedCapture < AVRS_MAX_FAILED_CAPTURE)
                                {
                                    // FIXME change that prototype !
                                    avrs_Capture(captureFbw,previewFbw, captureHandler);
                                }
                                else
                                {
                                    AVRS_TRACE(AVRS_WARN_TRC, 0, "avrs task: Stopping Capture, Too many errors");
                                    if (captureHandler)
                                    {
                                        captureHandler(captureFbw, previewFbw);
                                    }
                                }
                            }
                        }
                        else
                        {
                            AVRS_TRACE(AVRS_WARN_TRC, 0, "avrs task: CAMD returned that status ? :%x", camdMsg->status);
                        }
                    }
                    break;

                case AVRS_OP_ID_DISPLAY_PREVIEW:
                    // Preview redimensionning and encoding done.
                    // Call handlers.

                    AVRS_TRACE(AVRS_INFO_TRC, 0, "Call handler");
                    AVRS_PROFILE_FUNCTION_ENTER(avrs_PictureStreamHandler);
                    // FIXME Shouldn't the encoding handler (just setting
                    // some variables and needing a kind of higher latency
                    // be called directly by the IRQ handler and not here ?)

                    // Encoding done handler (Message status is the encoded frame size)
                    if (encodedFrame && g_avrsVideoCfg.recordOn)
                    {
                        if (encodedFrame->callback)
                        {
                            encodedFrame->callback(encodedFrame->buffer, msg->status);
                        }
                    }

                    // Preview ready handler

                    if (previewPicture && g_avrsVideoCfg.previewOn)
                    {
                        if (previewPicture->callback)
                        {
                            previewPicture->callback(previewPicture->fbw);
                        }
                    }

                    AVRS_PROFILE_FUNCTION_EXIT(avrs_PictureStreamHandler);
                    AVRS_TRACE(AVRS_INFO_TRC, 0, "Handler called");

                    // Processing done - allow new pictures from the camera.
                    pictureProcessingInProgress = FALSE;
                    AVRS_PROFILE_WINDOW_EXIT(avrs_PictureProcessingInProgress);

                    // TODO Implement the gestion of the frame buffer swapping
                    // here: wait for a message to trig the swap, and decide
                    // of things if the message is not received. This message
                    // being sent to the task by the handler of encoded picture
                    // (or implemented through a global variable)

                    // Update swap buffer
                    currentDepth += 1;
                    if (currentDepth >= streamDepth)
                    {
                        currentDepth = 0;
                    }

                    if (fbwStream && previewPicture)
                    {
                        previewPicture->fbw = &((*fbwStream)[currentDepth]);
                    }
                    else
                    {
                        // TODO Check the usefulness of that else statement.
                    }

                    if (encBufStream && encodedFrame)
                    {
                        encodedFrame->buffer = (*encBufStream)[currentDepth];
                    }
                    else
                    {
                        // TODO Check the usefulness of that else statement.
                    }


                    // The stream is to be stopped here if such is requested.
                    if (stopPictureStreamRequested)
                    {
                        AVRS_TRACE(AVRS_INFO_TRC, 0, "avrs task: Stopping preview");
                        stopPictureStreamRequested = FALSE;
                        pictureStreamMode = FALSE;
                        // We don't need a high frequency anymore

                        hal_SwReleaseClk(FOURCC_AVRS);

                        // Clear the strean variables here.
                        AVRS_TRACE(AVRS_INFO_TRC, 0, "Streams freed after a capture done.");
                        avrs_FreeStreams();
                        g_avrsStatus = AVRS_STATUS_OPEN;

                        // Reset the buffer pointer by g_avrsCameraRawDataFbw.fb.buffer
                        // to its initial value to allow free to liberate the buffer.
                        g_avrsCameraRawDataFbw.fb.buffer = cameraBufferOrigin;
                    }

                    break;



                default:
                    // What is this message ?
                    // Do nothing, it will be freed
                    AVRS_TRACE(AVRS_WARN_TRC, 0, "avrs task: What is this message ? :%x", msg->headerId);
                    break;
            }



            // Free the message, if it was not an event
            sxr_Free(msg);
        }
    }
}



