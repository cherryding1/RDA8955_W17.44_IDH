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

#include "sxr_ops.h"
#include "sxs_io.h"

#include "avrs_m.h"
#include "avrsp.h"
#include "avrsp_video_handler.h"


#include "hal_sys.h"

// =============================================================================
// avrs_VideoHandler
// -----------------------------------------------------------------------------
/// This is the video handler called when the encoding of a frame is finished.
/// (This function being the callback of a #IMGS_ENCODED_PICTURE_T structure, it
/// must comply to the #IMGS_ENCODED_PICTURE_T type prototype.)
///
/// @param encodedBuffer Pointer to the beginning of the encoded picture.
/// @param length Length of the picture in the buffer.
// =============================================================================
PROTECTED VOID avrs_VideoHandler(UINT8* encodedBuffer, UINT32 length)
{
    // Change the definition of the latest encoded
    // video frame asynchronously.
    UINT32 csStatus = hal_SysEnterCriticalSection();
    g_avrsLatestEncodedFrame        = encodedBuffer;
    g_avrsLatestEncodedFrameLength  = length;
    hal_SysExitCriticalSection(csStatus);

    // Special case when there is no audio ...
    // Since normaly audio is used as the scheduler to
    // send at the proper time the message with the pointer
    // to the encoded Jpeg image, when there is no audio,
    // we ('re in trouble) need to send the event manually
    // here.
    // FIXME: This won't enforce the frame rate if the
    // source (camera) lags behind.
    if (!g_avrsAudioEnabled && g_avrsVideoStream.mbx != 0xFF)
    {
        AVRS_EVENT_T ev;
        ev.id           = AVRS_EVENT_ID_VIDEO;
        ev.bufferPos    = (UINT32*)(encodedBuffer);
        ev.frameSize    = length;
        ev.reserved[0]  = 0;
        sxr_Send(&ev, g_avrsVideoStream.mbx, SXR_SEND_EVT);
    }
}




