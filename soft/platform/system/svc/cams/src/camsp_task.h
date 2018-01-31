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



#ifndef _CAMSP_TASK_H_
#define _CAMSP_TASK_H_

#include "cs_types.h"
#include "sxr_ops.h"

#include "cams_m.h"
#include "camd_m.h"

#define ZOOM_PRECISION 1024
// =============================================================================
// CAMS_INTERNAL_STATUS_T
// -----------------------------------------------------------------------------
/// The status of cams service
// =============================================================================
typedef enum
{
    CAMS_STATUS_CLOSE = 0,
    CAMS_STATUS_OPEN,
    CAMS_STATUS_PREVIEWING
} CAMS_STATUS_T ;

// =============================================================================
// CAMS_OP_ID_T
// -----------------------------------------------------------------------------
/// Type used to describe the camera driver status in the message sent
/// through the CAMS message mailbox.
// =============================================================================
typedef enum
{
    CAMS_OP_ID_CAPTURE_DONE = CAMD_MSG_HEADER_ID,
    CAMS_OP_ID_START_PREVIEW,
    CAMS_OP_ID_STOP_PREVIEW,
    CAMS_OP_ID_START_CAPTURE,

    CAMS_OP_ID_CAPTURE_QTY
} CAMS_OP_ID_T;



// =============================================================================
// CAMS_MSG_T
// -----------------------------------------------------------------------------
/// Type used to encode the messages sent to the mailbox.
// (size must be at most four 32 bits words)
// =============================================================================
typedef struct
{
    CAMS_OP_ID_T    headerId;
    UINT32    status;
} CAMS_MSG_T;


// =============================================================================
// CAMS_MSG_CAPTURE_T
// -----------------------------------------------------------------------------
/// Type used to encode the start capture message. headerId must be set to
/// #CAMS_OP_ID_START_CAPTURE;
// =============================================================================
typedef struct
{
    CAMS_OP_ID_T            headerId;
    LCDD_FBW_T*             dataFbw;
    LCDD_FBW_T*             prvwFbw;
    CAMS_CAPTURE_HANDLER_T  captureHandler;
} CAMS_MSG_CAPTURE_T;


// =============================================================================
// CAMS_MSG_PREVIEW_T
// -----------------------------------------------------------------------------
/// Type used to encode the start capture message. headerId must be set to
/// #CAMS_OP_ID_START_PREVIEW;
// =============================================================================
typedef struct
{
    CAMS_OP_ID_T            headerId;
    LCDD_FBW_T*             prvwFbw;
    UINT8                   frameRate;
    CAMS_PREVIEW_HANDLER_T  prvwHandler;
} CAMS_MSG_PREVIEW_T;


// =============================================================================
// MACROS
// =============================================================================
#define CAMS_TASK_STACK_SIZE    256
#define CAMS_TASK_PRIORITY      50

// =============================================================================
// PROTECTED VARIABLES
// =============================================================================

EXPORT sxr_TaskDesc_t   g_camsTask;
EXPORT UINT8            g_camsIdTask;
EXPORT UINT8            g_camsMbx;
EXPORT CAMS_STATUS_T    g_camsStatus;
EXPORT UINT32           g_camsAvailResolution;
EXPORT UINT32           g_camsValidParam;
EXPORT UINT32           g_camsRotate;
EXPORT BOOL             g_camsPreviewAllFrames;
EXPORT UINT32           g_camsDigitalZoomFactor;
EXPORT LCDD_FBW_T       g_camsCameraRawDataFbw;
EXPORT UINT32           g_camsCameraRawDataSize;


// ==============================================================================
// cams_Task
// ------------------------------------------------------------------------------
/// CAMS task.
// ==============================================================================
PROTECTED VOID cams_Task(VOID* param);

PROTECTED VOID cams_SetCameraResolution(LCDD_FBW_T* Fbw);

#endif // _CAMSP_TASK_H_

