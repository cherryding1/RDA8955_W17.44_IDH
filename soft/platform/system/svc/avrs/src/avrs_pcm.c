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

#include "sxr_ops.h"
#include "sxs_io.h"

#include "avrs_m.h"
#include "avrsp.h"
#include "avrsp_debug.h"



// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// avrs_XXXPcmHandler
// -----------------------------------------------------------------------------
/// Handler for the XXX module, to play PCM  streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PROTECTED VOID avrs_XXXPcmHandler(VOID)
{
    // Number of bytes to copy;
    UINT32 cpSz;

    // Position in the copy buffer, where data from the stream
    // are going to be copied
    UINT32* cpBufPos;

    AVRS_PROFILE_FUNCTION_ENTER(avrs_XXXSpeechHandler);

    g_avrsTramePlayedNb++;

    /// Special operation according to state
    ///
    /// if (sampleNumber == ...)
    /// ...
    /// ...
    if (g_avrsTramePlayedNb == AVRS_AUD_UNMUTE_TRAME_NB)
    {
        // Unmute AUD
        aud_Setup(g_avrsItf, &g_avrsAudioCfg);
    }

    if (g_avrsTramePlayedNb == AVRS_VPP_UNMUTE_TRAME_NB)
    {
        // Unmute VPP
        vpp_SpeechAudioCfg(&g_avrsVppCfg);
    }

    // Exit the handler immediatly if first interrupt
    if (g_avrsTramePlayedNb <= 1)
    {
        AVRS_PROFILE_FUNCTION_EXIT(avrs_XXXSpeechHandler);
        return;
    }

    // g_avrsBufferAvailBytes is the number of bytes available
    // for the user to read. The room available in the buffer
    // is then length - g_avrsBufferAvailBytes
    // We only take the number of available bytes into account
    // in loop mode.
    if (g_avrsBufferLength - g_avrsBufferAvailBytes < g_avrsFrameSize)
    {
        // Drop the current frame
        g_avrsTrameDroppedNb++;
        // This frame was not played.
        g_avrsTramePlayedNb--;
        AVRS_TRACE(AVRS_WARN_TRC, 0, "Frame dropped: %d", g_avrsTrameDroppedNb);

        // And does nothing else, hoping for better days
    }
    else
    {

        if (g_avrsSwIdx == 0)
        {
            cpBufPos = g_avrsEncOutput;
            g_avrsSwIdx = 1;
        }
        else
        {
            // == 1
            // We aim at the middle of the 2 * HAL_SPEECH_FRAME_SIZE_PCM_BUF //2*576*2 UINT32s buffer:
            // cpBufPos is a UINT32*, g_avrsFrameSize is in bytes.
            cpBufPos = g_avrsEncOutput + g_avrsFrameSize/4;
            g_avrsSwIdx = 0;
        }

        /// Do we wrap around the top of the buffer ?
        if ((((UINT32)g_avrsBufferCurPos) + (g_avrsFrameSize)) > (UINT32)g_avrsBufferTop)
        {
            // Copy first part
            cpSz = (UINT32)(g_avrsBufferTop - g_avrsBufferCurPos);
            memcpy(g_avrsBufferCurPos, cpBufPos, cpSz);
            cpBufPos = (UINT32*)((UINT32)cpBufPos+cpSz);

            g_avrsBufferCurPos = g_avrsBufferStart;
            cpSz = g_avrsFrameSize - cpSz;

        }
        else
        {
            cpSz = g_avrsFrameSize;
        }

        // Finish or full copy
        memcpy(g_avrsBufferCurPos, cpBufPos, cpSz);
        g_avrsBufferCurPos = (UINT32*)((UINT32)g_avrsBufferCurPos+cpSz);

        g_avrsTramePlayedNb++;

        // Report the new data
        UINT32 status = hal_SysEnterCriticalSection();
        // Decrease size in bytes
        g_avrsBufferAvailBytes += g_avrsFrameSize;
        hal_SysExitCriticalSection(status);
    }

    // Call the user handler if enough data has been recorded
    if (g_avrsBufferAvailBytes > g_avrsBufferLength/4)
    {
        if (g_avrsAudioMbx != 0xFF && g_avrsBufferingMsgReceived)
        {
            AVRS_SEND_CUR_POS_EVT;
            g_avrsBufferingMsgReceived = FALSE;
        }
    }


    AVRS_PROFILE_FUNCTION_EXIT(avrs_XXXSpeechHandler);
}

