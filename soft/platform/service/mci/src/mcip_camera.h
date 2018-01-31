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


#ifndef _MCIP_CAMERA_H_
#define _MCIP_CAMERA_H_

#include "cs_types.h"
#include "lcdd_m.h"


// =============================================================================
// MACROS
// =============================================================================
#define MCI_CAM_MAX_PREVIEW_SIZE (320*240)
#define MCI_CAM_MAX_CAPTURE_SIZE (640*480)

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// MCI_CAMERA_INFO_STRUCT_T
// -----------------------------------------------------------------------------
/// Type used to store some useful information about the camera settings.
/// Nota: TODO We should be able, in a certain measure, to get rid of it.
// =============================================================================
typedef struct
{
    UINT16  startX;
    UINT16  startY;
    UINT16  endX;
    UINT16  endY;
    UINT16  imageWidth;
    UINT16  imageHeight;
    UINT16  screenWidth;
    UINT16  screenHeight;
    UINT32  previewZoom;
    INT32   imageQuality;
    BOOL    addFrame;
} MCI_CAMERA_INFO_STRUCT_T;


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================
PRIVATE VOID MCI_CamSaveOsd(UINT16 *buffer, UINT16 startx, UINT16 starty, UINT16 endx, UINT16 endy);
PRIVATE VOID mci_CameraBlendOsd(LCDD_FBW_T *previewFbw, LCDD_FBW_T *osdFbw);
PRIVATE VOID mci_CameraPreviewHandler(LCDD_FBW_T *previewFbw);
PRIVATE VOID mci_CameraCaptureHandler(LCDD_FBW_T *dataFbw, LCDD_FBW_T *previewFbw);



#endif // _MCIP_CAMERA_H_
