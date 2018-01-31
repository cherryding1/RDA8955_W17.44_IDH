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
#include "avrsp.h"
#include "avrsp_debug.h"
#include "vpp_speech.h"




// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// avrs_VppSpeechHandler
// -----------------------------------------------------------------------------
/// Handler for the vpp module, for speech-kind encoded streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PROTECTED VOID avrs_VppSpeechHandler(HAL_VOC_IRQ_STATUS_T* status)
{
    AVRS_PROFILE_FUNCTION_ENTER(avrs_VppSpeechHandler);
    // Number of bytes to copy;
    UINT32 cpSz;

    // Position in the decoding structure where to store
    UINT32* encOutBufPos;

    // VPP encoding buffer pointer.
    CONST HAL_SPEECH_ENC_OUT_T* vppEncBuf;

    g_avrsTramePlayedNb++;

    if (g_avrsTramePlayedNb == AVRS_AUD_UNMUTE_TRAME_NB)
    {
        // Unmute AUD
        AVRS_TRACE(AVRS_INFO_TRC, 0, "AUD unmuted");
        aud_Setup(g_avrsItf, &g_avrsAudioCfg);
    }

    if (g_avrsTramePlayedNb == AVRS_VPP_UNMUTE_TRAME_NB)
    {
        // Unmute VPP
        AVRS_TRACE(AVRS_INFO_TRC, 0, "VPP unmuted");
        vpp_SpeechAudioCfg(&g_avrsVppCfg);

    }

    // Exit the handler immediatly if first interrupt
    if (g_avrsTramePlayedNb <= 1)
    {
        AVRS_PROFILE_FUNCTION_EXIT(avrs_VppSpeechHandler);
        return;
    }

    // g_avrsBufferAvailBytes is the number of bytes available
    // for the user to read. The room available in the buffer
    // is then length - g_avrsBufferAvailBytes
    // We only take the number of available bytes into account
    // in loop mode.
    if ((g_avrsBufferLength - g_avrsBufferAvailBytes < g_avrsFrameSize))
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
        // Typical global vars
        vppEncBuf = vpp_SpeechGetTxCodBuffer();

        // Pointer from which to get the encoded data.
        encOutBufPos = (UINT32*) vppEncBuf->encOutBuf;

        /// Do we wrap around the top of the buffer ?
        if (((UINT32)g_avrsBufferCurPos + g_avrsFrameSize) > (UINT32)g_avrsBufferTop)
        {
            // Copy first part
            cpSz = (UINT32)(g_avrsBufferTop - g_avrsBufferCurPos);
            memcpy(g_avrsBufferCurPos, encOutBufPos, cpSz);
            encOutBufPos = (UINT32*)((UINT32)encOutBufPos+cpSz);

            g_avrsBufferCurPos = g_avrsBufferStart;
            cpSz = g_avrsFrameSize - cpSz;

        }
        else
        {
            cpSz = g_avrsFrameSize;
        }

        // Finish or full copy
        memcpy(g_avrsBufferCurPos, encOutBufPos, cpSz);
        g_avrsBufferCurPos = (UINT32*)((UINT32)g_avrsBufferCurPos+cpSz);


        g_avrsTramePlayedNb++;

        // Report the new data
        UINT32 csStatus = hal_SysEnterCriticalSection();
        // Increase size in bytes
        g_avrsBufferAvailBytes += g_avrsFrameSize;
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

    AVRS_PROFILE_FUNCTION_EXIT(avrs_VppSpeechHandler);
}


