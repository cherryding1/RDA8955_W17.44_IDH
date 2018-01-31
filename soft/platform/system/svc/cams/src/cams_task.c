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

#include "cams_m.h"
#include "camd_m.h"
#include "imgs_m.h"
#include "cams_config.h"

#include "camsp_task.h"
#include "camsp_debug.h"

#include "hal_sys.h"
#include "hal_clk.h"
#include "hal_debug.h"

#ifdef CAMS_SHOW_FPS
// Enable the display of the framerate on the
// screen.
// NOTA: the delay timer wrap is not taken into account,
// as it will only make one FPS estimation false.
#include "fmg_m.h"
#include "hal_timers.h"
#define CAMS_FPS_MEAS_FRAME_NUMBER       5

#endif

#include "hal_debug.h"


// =============================================================================
// PROTECTED VARIABLES
// =============================================================================
#define CAMS_MAX_FAILED_CAPTURE 15

PROTECTED sxr_TaskDesc_t    g_camsTask;
PROTECTED UINT8             g_camsIdTask = 0;
PROTECTED UINT8             g_camsMbx;
PROTECTED CAMS_STATUS_T     g_camsStatus = CAMS_STATUS_CLOSE;
PROTECTED UINT32            g_camsAvailResolution = 0;
PROTECTED UINT32            g_camsValidParam = 0;
PROTECTED UINT32            g_camsRotate = 0;
PROTECTED BOOL              g_camsPreviewAllFrames = TRUE;
PROTECTED UINT32            g_camsDigitalZoomFactor = ZOOM_PRECISION;
PROTECTED LCDD_FBW_T        g_camsCameraRawDataFbw;
PROTECTED UINT32            g_camsCameraRawDataSize = 0;

// This macro initialisez all the state variable form the CAMS
// task state machine.
// We keep it here so that we can think more easily of
// adding any new variable to it
#define CAMS_TASK_RESET_STATE    \
    previewMode                         = FALSE; \
    stopPreviewRequested                = FALSE; \
    restartPreviewRequested             = FALSE; \
    previewHandler                      = NULL; \
    captureHandler                      = NULL; \
    captureFbw                      = NULL; \
    previewFbw                          = NULL; \
    failedCapture                       = 0; \


// ==============================================================================
// cams_Task
// ------------------------------------------------------------------------------
/// CAMS task.
/// @param param @todo Do we need a parameter here ?
// ==============================================================================
PROTECTED VOID cams_Task(VOID* param)
{
    // Message accessors
    UINT32              evt[4];
    CAMS_MSG_T*         msg         = NULL;
    CAMS_MSG_CAPTURE_T* captureMsg  = NULL;
    CAMS_MSG_PREVIEW_T* previewMsg  = NULL;
    CAMD_MSG_T*         camdMsg     = NULL;

    // state variables
    BOOL        previewMode;
    BOOL        stopPreviewRequested;
    BOOL        restartPreviewRequested;
    CAMS_PREVIEW_HANDLER_T previewHandler;
    CAMS_CAPTURE_HANDLER_T captureHandler;
    LCDD_FBW_T* captureFbw;
    LCDD_FBW_T* previewFbw;
    UINT8 failedCapture = 0;
    UINT16*     cameraBufferOrigin = NULL;

    CAMS_TASK_RESET_STATE;

#ifdef CAMS_SHOW_FPS
    UINT32 elapsedTime = 1;
    UINT32 fps = 1;
    UINT32 fpsStartMeasTime   = 0;
    UINT32 fpsElapsedFrame    = 0;

#endif
    UINT32 csStatus;

    while (1)
    {
        CAMS_TRACE(TSTDOUT, 0, "Cams task: waiting for a message");
        msg = sxr_Wait(evt, g_camsMbx);

        if (msg == NULL)
        {
            // What is this event ?
            // Do nothing
            CAMS_TRACE(TSTDOUT, 0, "Cams task: What is this event ? :%x", evt[0]);
        }
        else
        {
            // We received a message
            switch (msg->headerId)
            {
                case CAMS_OP_ID_START_PREVIEW:
                    CAMS_PROFILE_WINDOW_ENTER(cams_PreviewWin);
                    if (!previewMode)
                    {
                        CAMS_TRACE(TSTDOUT, 0, "Cams task: start preview, first time");
                        // This is like a reinitialization of the state machine:
                        CAMS_TASK_RESET_STATE;
#ifdef CAMS_SHOW_FPS
                        fpsStartMeasTime   = 0;
                        fpsElapsedFrame    = 0;
#endif


                        // We need a high frequency to sustain the throughput needed

                        hal_SwRequestClk(FOURCC_CAMS, HAL_SYS_FREQ_104M);
                        stopPreviewRequested = FALSE;
                        previewMode = TRUE;
                        g_camsStatus = CAMS_STATUS_PREVIEWING;
                        previewMsg = (CAMS_MSG_PREVIEW_T*)msg;
                        previewFbw = previewMsg->prvwFbw;
                        previewHandler = previewMsg->prvwHandler;

                        // Select camera resolution based on requested preview
                        cams_SetCameraResolution(previewFbw);

                        // Set Format
                        switch (previewFbw->fb.colorFormat)
                        {
                            case LCDD_COLOR_FORMAT_RGB_565:
                                camd_SetParam(CAMD_PARAM_FORMAT, CAMD_PARAM_FORMAT_RGB565);
                                break;

                            default:
                                CAMS_ASSERT(FALSE, "Only RGB 565 is supported by CAMS");
                                break;
                        }
                        camd_SetParam(CAMD_PARAM_FRAME_RATE, previewMsg->frameRate*2);



                        // Get image
                        CAMS_TRACE(TSTDOUT, 0, "Cams task: preview: Take capture");
                        // Save the mallocated pointer of the buffer to restore it
                        // when we stop. (This pointer is freed by cams_Close)
                        cameraBufferOrigin = g_camsCameraRawDataFbw.fb.buffer;
                        camd_ImageStreamStart(g_camsCameraRawDataFbw.fb.buffer, g_camsCameraRawDataSize,TRUE);
                    }
                    break;

                case CAMS_OP_ID_STOP_PREVIEW:
                    CAMS_TRACE(TSTDOUT, 0, "Cams task: Stop preview");
                    // The critical section here aims at having
                    // the new behaviour of CAMD interrupt handler
                    // and of the CAMS task for the same camera
                    // VSYNC interrupt.
                    csStatus = hal_SysEnterCriticalSection();
                    // Stop the picture stream
                    camd_ImageStreamStop();
                    // Prevent restarting
                    stopPreviewRequested = TRUE;
                    hal_SysExitCriticalSection(csStatus);
                    break;

                case CAMS_OP_ID_START_CAPTURE:
                    if (!previewMode)
                    {
                        CAMS_PROFILE_WINDOW_ENTER(cams_CaptureWin);
                        CAMS_TRACE(TSTDOUT, 0, "Cams task: Capture Picture");
                        // We need a high frequency to sustain the throughput needed

                        hal_SwRequestClk(FOURCC_CAMS, HAL_SYS_FREQ_104M);
                        captureMsg = (CAMS_MSG_CAPTURE_T*)msg;
                        captureFbw = captureMsg->dataFbw;
                        previewFbw = captureMsg->prvwFbw;
                        captureHandler = captureMsg->captureHandler;
                        previewMode = FALSE;

                        CAMS_ASSERT(previewFbw->fb.colorFormat == LCDD_COLOR_FORMAT_RGB_565,
                                    "Only RGB 565 is supported by CAMS");

                        // Set Format
                        switch (captureFbw->fb.colorFormat)
                        {
                            case LCDD_COLOR_FORMAT_RGB_565:
                                camd_SetParam(CAMD_PARAM_FORMAT, CAMD_PARAM_FORMAT_RGB565);
                                break;

                            default:
                                CAMS_ASSERT(FALSE, "Only RGB 565 is supported by CAMS");
                                break;
                        }

                        // Select camera resolution based on requested capture
                        cams_SetCameraResolution(captureFbw);

                        // Save the mallocated pointer of the buffer to restore it
                        // when we stop. (This pointer is freed by cams_Close)
                        cameraBufferOrigin = g_camsCameraRawDataFbw.fb.buffer;
                        camd_GetImage(g_camsCameraRawDataFbw.fb.buffer);
                    }
                    else
                    {
                        // Return an error ?
                        CAMS_TRACE(TSTDOUT, 0, "Cams task: Capture while in preview, skip !");
                    }
                    break;

                case CAMS_OP_ID_CAPTURE_DONE:
                    camdMsg = (CAMD_MSG_T*) msg;
                    if (camdMsg->status == CAMD_STATUS_CAPTURE_DONE)
                    {
                        g_camsCameraRawDataFbw.fb.buffer = (UINT16*) camdMsg->imgPtr;
                        failedCapture = 0;
                        if (previewMode == TRUE)
                        {
                            // PREVIEW MODE
                            CAMS_TRACE(TSTDOUT, 0, "Cams task: Preview captured one frame");
                            //  preview mode
                            // Resize taken picture to fit in the preview zone
                            cams_ResizeRawToRequested(&g_camsCameraRawDataFbw, previewFbw, g_camsRotate, g_camsDigitalZoomFactor);
                            CAMS_TRACE(TSTDOUT, 0, "Cams task: Resized one frame");

                            // Call preview handler
                            if (previewHandler)
                            {
                                CAMS_TRACE(TSTDOUT, 0, "Call handler");
                                CAMS_PROFILE_FUNCTION_ENTER(cams_PreviewHandler);
                                previewHandler(previewFbw);
                                CAMS_PROFILE_FUNCTION_EXIT(cams_PreviewHandler);
                                CAMS_TRACE(TSTDOUT, 0, "Handler called");
                            }
#ifdef CAMS_SHOW_FPS
                            // Display FPS
                            fpsElapsedFrame++;
                            if (fpsElapsedFrame >= CAMS_FPS_MEAS_FRAME_NUMBER)
                            {
                                elapsedTime = (hal_TimGetUpTime()-fpsStartMeasTime);
                                fps = (1 SECOND * fpsElapsedFrame)/(elapsedTime?elapsedTime:1);

                                CAMS_TRACE(TSTDOUT, 0, "FPS: elapsed frame: %d", fpsElapsedFrame);
                                CAMS_TRACE(TSTDOUT, 0, "FPS: elapsed time: %d", elapsedTime);
                                CAMS_TRACE(TSTDOUT, 0, "FPS: fps: %d", fps);
                                // Prepare next run
                                fpsElapsedFrame = 0;
                                fpsStartMeasTime = hal_TimGetUpTime();
                            }

                            // Display FPS at each frame.
#if 0
                            fmg_PrintfSetXY(12,1);
                            fmg_Printf("%d", fps);
#endif
#endif

                            if (stopPreviewRequested)
                            {
                                CAMS_TRACE(TSTDOUT, 0, "Cams task: Stopping preview");
                                stopPreviewRequested = FALSE;
                                previewMode = FALSE;
                                // We don't need a high frequency anymore

                                hal_SwReleaseClk(FOURCC_CAMS);
                                g_camsStatus = CAMS_STATUS_OPEN;
                                // Reset the buffer pointer by g_camsCameraRawDataFbw.fb.buffer
                                // to its initial value to allow free to liberate the buffer.
                                g_camsCameraRawDataFbw.fb.buffer = cameraBufferOrigin;
                            }


                            // CAMD launches automatically the capture of a new frame
                        }
                        else
                        {
                            // CAPTURE MODE
                            CAMS_PROFILE_WINDOW_EXIT(cams_CaptureWin);
                            CAMS_TRACE(TSTDOUT, 0, "Cams task: Capture captured one frame");
                            // Resize taken picture to fit in the preview zone
                            cams_ResizeRawToRequested(&g_camsCameraRawDataFbw, previewFbw, g_camsRotate, g_camsDigitalZoomFactor);
                            // Resize taken picture to fit in the capture zone
                            cams_ResizeRawToRequested(&g_camsCameraRawDataFbw, captureFbw, g_camsRotate, g_camsDigitalZoomFactor);

                            // Call capture handler
                            if (captureHandler)
                            {
                                captureHandler(captureFbw, previewFbw);
                            }

                            // We don't need a high frequency anymore

                            hal_SwReleaseClk(FOURCC_CAMS);
                            // Reset the buffer pointer by g_camsCameraRawDataFbw.fb.buffer
                            // to its initial value to allow free to liberate the buffer.
                            g_camsCameraRawDataFbw.fb.buffer = cameraBufferOrigin;
                        }
                    }
                    else if (camdMsg->status == CAMD_STATUS_CAPTURE_ERROR)
                    {
                        failedCapture++;
                        if (previewMode == TRUE)
                        {
                            CAMS_PROFILE_WINDOW_EXIT(cams_PreviewWin);
                            CAMS_TRACE(TSTDOUT, 0, "Cams task: Preview failed to capture one frame, retry");
                            if (stopPreviewRequested)
                            {
                                CAMS_TRACE(TSTDOUT, 0, "Cams task: Stopping preview");
                                stopPreviewRequested = FALSE;
                                previewMode = FALSE;
                                // We don't need a high frequency anymore

                                hal_SwReleaseClk(FOURCC_CAMS);
                                g_camsStatus = CAMS_STATUS_OPEN;
                            }
                            else
                            {
                                if (failedCapture>CAMS_MAX_FAILED_CAPTURE)
                                {
                                    CAMS_TRACE(TSTDOUT, 0, "Cams task: Stopping preview, Too many errors");
                                    camd_ImageStreamStop();
                                    stopPreviewRequested = TRUE;
                                    // TODO should send an error message
                                }
                            }
                        }
                        else
                        {
                            // CAPTURE MODE
                            CAMS_PROFILE_WINDOW_EXIT(cams_CaptureWin);
                            CAMS_TRACE(TSTDOUT, 0, "Cams task: Capture failed to capture one frame,retry");
                            if (failedCapture < CAMS_MAX_FAILED_CAPTURE)
                            {
                                cams_Capture(captureFbw,previewFbw, captureHandler);
                            }
                            else
                            {
                                CAMS_TRACE(TSTDOUT, 0, "Cams task: Stopping Capture, Too many errors");
                                if (captureHandler)
                                {
                                    captureHandler(captureFbw, previewFbw);
                                }
                            }
                        }
                    }
                    else
                    {
                        CAMS_TRACE(TSTDOUT, 0, "Cams task: CAMD returned that status ? :%x", camdMsg->status);
                    }
                    break;



                default:
                    // What is this message ?
                    // Do nothing, it will be freed
                    CAMS_TRACE(TSTDOUT, 0, "Cams task: What is this message ? :%x", msg->headerId);
                    break;
            }



            // Free the message, if it was not an event
            sxr_Free(msg);
        }
    }
}

