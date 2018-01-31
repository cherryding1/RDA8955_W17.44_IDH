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
// GLOBAL VARIABLES
// =============================================================================
/// Audio configuration for the AMJR decoder.
/// Initial setup is done by #avrs_Record.
PROTECTED VPP_AMJR_AUDIO_CFG_T g_avrsVppAmjrAudioCfg
__attribute__((section (".ars.globalvars.c"))) ;


// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// avrs_VppAmjrAudioHandler
// -----------------------------------------------------------------------------
/// Handler for the audio cause of the VPP AMJR interrupt.
/// @param status VPP AMJR interrupt status.
// =============================================================================
PROTECTED VOID avrs_VppAmjrAudioHandler(HAL_VOC_IRQ_STATUS_T* status)
{
    VPP_AMJR_STATUS_T   amjrStatus;
    UINT32 frameSize = 0;
    UINT32* encOutBufPos = NULL;
    UINT32 cpSz = 0;

    AVRS_PROFILE_FUNCTION_ENTER(avrs_VppAmjrAudioHandler);

    // get status
    vpp_AmjrStatus(&amjrStatus);

    AVRS_TRACE(AVRS_INFO_TRC, 0, "AVRS: AMJR Handler: VPP_AMJR_STRM_ID_AUDIO IRQ %i", g_avrsTramePlayedNb);

    if (g_avrsTramePlayedNb == AVRS_AUD_UNMUTE_TRAME_NB)
    {
        // Unmute AUD
        AVRS_TRACE(AVRS_INFO_TRC, 0, "AUD unmuted");
        aud_Setup(g_avrsItf, &g_avrsAudioCfg);
    }

    // The frame size can change from one frame to the other.
    frameSize    = amjrStatus.outputLen;
    AVRS_TRACE(AVRS_DBG_TRC, 0, "Encoded Frame size: %d", frameSize);

    // g_avrsBufferAvailBytes is the number of bytes available
    // for the user to read. The room available in the buffer
    // is then length - g_avrsBufferAvailBytes
    // We only take the number of available bytes into account
    // in loop mode.
    if ((g_avrsBufferLength - g_avrsBufferAvailBytes < frameSize))
    {
        // Drop the current frame
        g_avrsTrameDroppedNb++;
        // This frame wasn't played.
        g_avrsTramePlayedNb--;
        AVRS_TRACE(AVRS_WARN_TRC, 0, "Frame dropped: %d", g_avrsTrameDroppedNb);

        // And does nothing else, hoping for better days
    }
    else
    {
        // Pointer from which to get the encoded data.
        encOutBufPos = (UINT32*) g_avrsVppAmjrAudioCfg.outBufAddr;

        AVRS_TRACE(AVRS_DBG_TRC, 0, "Encoding buffer: %#x", encOutBufPos);
        AVRS_TRACE(AVRS_DBG_TRC, 0, "Encoded buffer: %#x", g_avrsBufferCurPos);
        AVRS_TRACE(AVRS_DBG_TRC, 0, "Encoded buffer+framesize: %#x", ((UINT32)g_avrsBufferCurPos + frameSize));



        /// Do we wrap around the top of the buffer ?
        if (((UINT32)g_avrsBufferCurPos + frameSize) > (UINT32)g_avrsBufferTop)
        {
            // Copy first part
            cpSz = (UINT32)(g_avrsBufferTop - g_avrsBufferCurPos);
            memcpy(g_avrsBufferCurPos, encOutBufPos, cpSz);
            encOutBufPos = (UINT32*)((UINT32)encOutBufPos+cpSz);

            g_avrsBufferCurPos = g_avrsBufferStart;
            cpSz = frameSize - cpSz;

        }
        else
        {
            cpSz = frameSize;
        }

        // Finish or full copy
        memcpy(g_avrsBufferCurPos, encOutBufPos, cpSz);
        g_avrsBufferCurPos = (UINT32*)((UINT32)g_avrsBufferCurPos+cpSz);


        g_avrsTramePlayedNb++;

        // Report the new data
        UINT32 csStatus = hal_SysEnterCriticalSection();

        // Increase size in bytes
        g_avrsBufferAvailBytes += frameSize;
        hal_SysExitCriticalSection(csStatus);
    }

    // Call the user handler if enough data has been recorded
    if (g_avrsBufferAvailBytes > g_avrsBufferLength/4)
    {
        AVRS_TRACE(AVRS_INFO_TRC, 0, "AVRS: send read req. avail bytes:%d", g_avrsBufferAvailBytes);
        if (g_avrsAudioMbx != 0xFF && g_avrsBufferingMsgReceived)
        {
            AVRS_SEND_CUR_POS_EVT;
            g_avrsBufferingMsgReceived = FALSE;
        }
    }


    // Determine which part of the swap buffer will be used for next frame.
//    if((((UINT32)amjrStatus.inStreamBufIfcStatus)&0xFFFFFF) >= (((UINT32)g_avrsPcmBuf_0)&0xFFFFFF) &&
//       (((UINT32)amjrStatus.inStreamBufIfcStatus)&0xFFFFFF) <  (((UINT32)g_avrsPcmBuf_1)&0xFFFFFF))
    if ((UINT32)g_avrsVppAmjrAudioCfg.inBufAddr     == (UINT32)g_avrsPcmBuf_1)
    {
        g_avrsVppAmjrAudioCfg.inBufAddr     = g_avrsPcmBuf_0;
        g_avrsVppAmjrAudioCfg.outBufAddr    = g_avrsEncBuf_0;
    }
    else
    {
        g_avrsVppAmjrAudioCfg.inBufAddr     = g_avrsPcmBuf_1;
        g_avrsVppAmjrAudioCfg.outBufAddr    = g_avrsEncBuf_1;
    }


    // TODO : these cfg need to be executed only when cfg parameters other than the inout buffers are updated
    vpp_AmjrAudioCfg(&g_avrsVppAmjrAudioCfg);
    // Only one reset.
    g_avrsVppAmjrAudioCfg.reset = 0;

    // Schedule Encoding of this frame for once the IFC has trigged the
    // corresponding VOC Event.
    vpp_AmjrScheduleOneAudioFrame(g_avrsVppAmjrAudioCfg.inBufAddr, g_avrsVppAmjrAudioCfg.outBufAddr);

    // profile
    AVRS_PROFILE_FUNCTION_EXIT(avrs_VppAmjrAudioHandler);
}


