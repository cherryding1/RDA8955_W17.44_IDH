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
// PRIVATE FUNCTIONS
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// avrs_VppAmjrHandler
// -----------------------------------------------------------------------------
// =============================================================================
PROTECTED VOID avrs_VppAmjrHandler(HAL_VOC_IRQ_STATUS_T* status)
{
    VPP_AMJR_STATUS_T   amjrStatus;
    AVRS_PROFILE_FUNCTION_ENTER(avrs_VppAmjrHandler);

    // get status
    vpp_AmjrStatus(&amjrStatus);

    AVRS_TRACE(AVRS_INFO_TRC, 0, "VPP STREAM STATUS %#x %#x", amjrStatus.strmStatus, amjrStatus.inStreamBufIfcStatus);

    switch (amjrStatus.strmStatus)
    {
        // VPP Open init interrupt.
        case VPP_AMJR_STRM_ID_INIT:
            AVRS_TRACE(AVRS_INFO_TRC, 0, "AVRS: AMJR Handler: VPP_AMJR_STRM_ID_INIT IRQ");

            g_avrsTramePlayedNb = 0;
            // Init interrupt: the first buffer to encode is the first half
            // of the PCM swap buffer.
            g_avrsVppAmjrAudioCfg.inBufAddr  = g_avrsPcmBuf_0;
            g_avrsVppAmjrAudioCfg.outBufAddr = g_avrsEncBuf_0;

            // Configure AMJR.
            vpp_AmjrAudioCfg(&g_avrsVppAmjrAudioCfg);

            // Schedule the encode of the first frame.
            vpp_AmjrScheduleOneAudioFrame(g_avrsVppAmjrAudioCfg.inBufAddr, g_avrsVppAmjrAudioCfg.outBufAddr);
            break;

        case VPP_AMJR_STRM_ID_AUDIO:
            avrs_VppAmjrAudioHandler(status);
            break;

        case VPP_AMJR_STRM_ID_VIDEO:
            avrs_VppAmjrVideoHandler(status);
            break;

        default:
            break;
            // We are never in this case
    }

    // profile
    AVRS_PROFILE_FUNCTION_EXIT(avrs_VppAmjrHandler);
}


