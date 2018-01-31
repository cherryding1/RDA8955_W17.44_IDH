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

#include "avrs_m.h"
#include "avrsp_amjr.h"
#include "avrsp.h"
#include "avrsp_debug.h"
#include "avrsp_task.h"

#include "aud_m.h"

#include "sxr_ops.h"
#include "sxs_io.h"

#include "hal_host.h"
#include "hal_debug.h"

#include "vpp_amjr.h"

#include <string.h>



// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// PRIVATE VARIABLES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// avrs_VppAmjrVideoHandler
// -----------------------------------------------------------------------------
/// Handler for the video cause of the VPP AMJR interrupt.
/// @param status VPP AMJR interrupt status.
// =============================================================================
PROTECTED VOID avrs_VppAmjrVideoHandler(HAL_VOC_IRQ_STATUS_T* status)
{
    VPP_AMJR_STATUS_T   amjrStatus;
    AVRS_MSG_T*          msg;

    AVRS_PROFILE_FUNCTION_ENTER(avrs_VppAmjrVideoHandler);

    // get status
    vpp_AmjrStatus(&amjrStatus);

    AVRS_TRACE(AVRS_INFO_TRC, 0, "VPP ENCODED LENGTH %d", amjrStatus.imagOutputLen);

    // Send the message to the task that the encoding of the picture frame
    // and the calculation of the preview are done.
    msg = sxr_Malloc(sizeof(AVRS_MSG_T));
    msg->headerId = AVRS_OP_ID_DISPLAY_PREVIEW;
    msg->status = amjrStatus.imagOutputLen;
    sxr_Send(msg, g_avrsMbx, SXR_SEND_MSG);

    // profile
    AVRS_PROFILE_FUNCTION_EXIT(avrs_VppAmjrVideoHandler);
}


