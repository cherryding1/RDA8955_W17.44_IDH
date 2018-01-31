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
#include "fs.h"

#include "mrsp_codec.h"
#include "mrsp.h"
#include "mrsp_debug.h"

#include "avrs_m.h"

#ifndef MRS_USES_AVRS
#include "ars_m.h"
#endif

#include "sxr_ops.h"
#include <string.h>

#include "hal_aif.h"


// =============================================================================
// MACROS
// =============================================================================




// =============================================================================
// GLOBAL VARIABLES
// =============================================================================
#ifndef MRS_USES_AVRS
/// FIXME Remove after portage on AVRS.
PRIVATE MRS_HANDLE_T g_mrsAmrHandle = NULL;


/// Prevent message sending as long as the answer has not been
/// acknowledged.
PRIVATE BOOL g_mrsAmrMsgAcked = FALSE;
#endif

// =============================================================================
// PRIVATE PROTOTYPE FUNCTIONS
// =============================================================================

PRIVATE UINT32 mrs_AmrWriteAudioData(MRS_HANDLE_T handle, UINT8* data,
                                     UINT32 size);

PRIVATE MRS_ERR_T mrs_AmrOpen(MRS_HANDLE_T handle, MRS_FILE_MODE_T mode, VOID** data);

PRIVATE VOID mrs_AmrClose(MRS_HANDLE_T handle);

PRIVATE VOID mrs_AmrStop(MRS_HANDLE_T handle);

PRIVATE BOOL mrs_AmrRecord( MRS_HANDLE_T handle,
                            AVRS_AUDIO_CFG_T* avrsAudioCfg,
                            AVRS_VIDEO_CFG_T* avrsVideoCfg);


#ifndef MRS_USES_AVRS
// =============================================================================
// mrs_AmrArsHandler
// -----------------------------------------------------------------------------
/// (ARS handler configured to mimic AVRS behaviour)
/// @todo Use AVRS and remove that guy.
/// Recording state handler for ARS: at half buffer and end buffer, send a
/// message to the MRS task with the pointer configured to the end of the
/// proper half.
// =============================================================================
PRIVATE VOID mrs_AmrArsHandler(ARS_STATUS_T status)
{
    AVRS_EVENT_T ev;

    // HYP: g_mrsAmrHandle is not NULL at that time.
    MRS_TRACE(MRS_INFO_TRC, 0, "sndrec_DrvHandler,status is %d, ack=%d",status, g_mrsAmrMsgAcked);

    if (g_mrsAmrMsgAcked == TRUE)
    {
        if(status == ARS_STATUS_MID_BUFFER_REACHED)
        {
            ev.id           = AVRS_EVENT_ID_AUDIO;
            // First half has finished.
            ev.bufferPos    = (UINT32*)(g_mrsAmrHandle->audioStream.startAddress
                                        + g_mrsAmrHandle->audioStream.length/2);
            ev.frameSize    = 0;
            ev.reserved[0]  = 0;
            sxr_Send(&ev, g_mrsAmrHandle->audioStream.mbx, SXR_SEND_EVT);
            //      ars_ReadData(SNDRECBUF_LEN*2);
            MRS_TRACE(MRS_INFO_TRC, 0, "MRS: AMR: Wait Ack");
            g_mrsAmrMsgAcked = FALSE;
        }
        else if(status == ARS_STATUS_END_BUFFER_REACHED)
        {
            ev.id           = AVRS_EVENT_ID_AUDIO;
            // Second half has finished, we are thus at the beginning again.
            ev.bufferPos    = (UINT32*)(g_mrsAmrHandle->audioStream.startAddress);
            ev.frameSize    = 32;
            ev.reserved[0]  = 0;
            sxr_Send(&ev, g_mrsAmrHandle->audioStream.mbx, SXR_SEND_EVT);
            //      ars_ReadData(SNDRECBUF_LEN*2);
            MRS_TRACE(MRS_INFO_TRC, 0, "MRS: AMR: Wait Ack");
            g_mrsAmrMsgAcked = FALSE;
        }
    }
}
#endif

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

PROTECTED MRS_CODEC_T g_mrsAmrCodec =
{
    .open               = mrs_AmrOpen,
    .close              = mrs_AmrClose,
    .record             = mrs_AmrRecord,
    .stop               = mrs_AmrStop,
    .writeAudioData     = mrs_AmrWriteAudioData,
    .writeVideoData     = NULL,
    .skipVideo          = NULL,
    .seek               = NULL, // TODO Implement if needed
    .tell               = NULL, // TODO Implement if needed
    .audioCodec         = MRS_AUDIO_CODEC_AMR_RING

};


// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

// =============================================================================
// mrs_AmrOpen
// -----------------------------------------------------------------------------
/// Open the AMR codec to record an AMR file, and complete the handle to use
/// to access the file. The handle's medium information must have been
/// set previoulsy by calling the #mrs_OpenFile function.
///
/// @param handle Handle filled with the file information.
/// @param mode File opening mode: creation, appending, etc ...
/// @param data Pointer to a private codec structure.. (unused)
/// @return #MRS_ERR_NO when the opening is successful, #MRS_ERR_FILE_ERROR when it failed
/// to create or access the file,  #MRS_ERR_INCOMPATIBLE_AUDIO_CODEC when
/// the audio format is incompatible.
/// @todo Densify the return value.
// =============================================================================
PRIVATE MRS_ERR_T mrs_AmrOpen(MRS_HANDLE_T handle, MRS_FILE_MODE_T mode, VOID** data)
{
    // Header plus a silence frame
    UINT8   buffer[6+32];

#ifndef MRS_USES_AVRS
    // FIXME
    g_mrsAmrHandle = handle;
#endif

    // FIXME Useful data ?
    *data = NULL;

    // Write the file header if the file is created
    // and add a silence frame at the beginning so that
    // the file is never 'empty'.
    if (mode == MRS_FILE_MODE_CREATE)
    {
        // File creation.
        buffer[0] = '#';
        buffer[1] = '!';
        buffer[2] = 'A';
        buffer[3] = 'M';
        buffer[4] = 'R';
        buffer[5] = '\n';
        buffer[6] = 0x3c;
        buffer[7] = 0x55;
        buffer[8] = 0x00;
        buffer[9] = 0x88;
        buffer[10] = 0xb6;
        buffer[11] = 0x66;
        buffer[12] = 0x79;
        buffer[13] = 0xe1;
        buffer[14] = 0xe0;
        buffer[15] = 0x01;
        buffer[16] = 0xe7;
        buffer[17] = 0xcf;
        buffer[18] = 0xf0;
        buffer[19] = 0x00;
        buffer[20] = 0x00;
        buffer[21] = 0x00;
        buffer[22] = 0x80;
        buffer[23] = 0x00;
        buffer[24] = 0x00;
        buffer[25] = 0x00;
        buffer[26] = 0x00;
        buffer[27] = 0x00;
        buffer[28] = 0x00;
        buffer[29] = 0x00;
        buffer[30] = 0x00;
        buffer[31] = 0x00;
        buffer[32] = 0x00;
        buffer[33] = 0x00;
        buffer[34] = 0x00;
        buffer[35] = 0x00;
        buffer[36] = 0x00;
        buffer[37] = 0x00;

        if (mrs_MediumWrite(&handle->mediumAudio, buffer, sizeof(buffer))>=0)
        {
            MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: AMR header written.");
        }
        else
        {
            return MRS_ERR_FILE_ERROR;
        }
    }
    else
    {
        // File appending.
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: AMR appending data.");
        // Check the validity of the file format
        if (mrs_MediumSeek(&handle->mediumAudio, 0, FS_SEEK_SET)>=0)
        {
            if (mrs_MediumRead(&handle->mediumAudio, buffer, sizeof(buffer))>=0)
            {
                MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: AMR header read (?).");
                if (buffer[0] != '#'
                        || buffer[1] != '!'
                        || buffer[2] != 'A'
                        || buffer[3] != 'M'
                        || buffer[4] != 'R'
                        || buffer[5] != '\n')
                {
                    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: AMR header read: BAD CODEC !");
                    return MRS_ERR_INCOMPATIBLE_AUDIO_CODEC;
                }
                else
                {
                    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: AMR header read (OK).");
                }
            }
            else
            {
                MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: AMR header read FAILED !");
                return MRS_ERR_FILE_ERROR;
            }
        }
        else
        {
            MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: AMR header seek failed.");
            return MRS_ERR_FILE_ERROR;
        }

        // Reset pointer at the end of the file to add new data
        // with the codec following calls. (record, etc)
        if (mrs_MediumSeek(&handle->mediumAudio, 0, FS_SEEK_END) <0)
        {
            MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: AMR .");
            return MRS_ERR_FILE_ERROR;
        }
    }


    handle->videoLength                 = 0;
    handle->videoBuffer                 = 0;
    handle->audioLength                 = 1600; //1024*8; // Half a buffer is worth half a second of data.
    handle->audioBuffer                 = (UINT8*) sxr_Malloc(handle->audioLength);
    handle->audioStream.mode            = AVRS_REC_AUDIO_MODE_AMR_RING;
    handle->audioStream.sampleRate      = HAL_AIF_FREQ_8000HZ;
    handle->audioStream.channelNb       = HAL_AIF_MONO;
    handle->audioStream.voiceQuality    = TRUE;

    return MRS_ERR_NO;
}


// =============================================================================
// mrs_AmrStop
// -----------------------------------------------------------------------------
/// Stop the recording of AMR.
/// This function will stop the recording through AVRS. The remaining
/// information in the buffer is flushed following the information sent
/// with the closing of the recording layer.
///
/// @param handle Handle of the file to close.
// =============================================================================
PRIVATE VOID mrs_AmrStop(MRS_HANDLE_T handle)
{
#ifdef MRS_USES_AVRS
    // FIXME Implement
    AVRS_ERR_T errorStatus;
    errorStatus = avrs_Stop();
#else
    ARS_ERR_T errorStatus;
    errorStatus = ars_Stop();
#endif
    MRS_TRACE(MRS_INFO_TRC, 0,  "mrs_AmrStop called ars_Stop and got status:%#x", errorStatus);
}


// =============================================================================
// mrs_AmrClose
// -----------------------------------------------------------------------------
/// Close the AMR codec.
/// This function frees the buffer used to record the data, so ensure everything
/// reliable in the buffer has been saved before. (ie: call #mrs_AmrStop before
/// that function)
/// @todo Assert/Error ensure the correct behaviour ?
/// @param handle Handle of the file to close.
// =============================================================================
PRIVATE VOID mrs_AmrClose(MRS_HANDLE_T handle)
{
    MRS_TRACE(MRS_INFO_TRC, 0,  "mrs_AmrClose called.");

    if (handle->audioBuffer != NULL)
    {
        sxr_Free(handle->audioBuffer);
        handle->audioBuffer = NULL;
    }

#ifndef MRS_USES_AVRS
    g_mrsAmrMsgAcked = FALSE;
    g_mrsAmrHandle = NULL;
#endif
}


// =============================================================================
// mrs_AmrWriteAudioData
// -----------------------------------------------------------------------------
/// Write audio data from a buffer to the file.
/// FIXME This function needs word aligned and sized buffer.
// =============================================================================
PRIVATE UINT32 mrs_AmrWriteAudioData(MRS_HANDLE_T handle, UINT8* data, UINT32 size)
{
    INT32 i;

    i = mrs_MediumWrite(&handle->mediumAudio, data, size);

    if(i < 0)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "MRS: mrs_AmrWriteAudioData returned %d", i);
        i = 0;
        // Todo Return an error and handle it upperer ?
    }
#ifndef MRS_USES_AVRS
    g_mrsAmrMsgAcked = TRUE;
#endif
    MRS_TRACE(MRS_INFO_TRC, 0, "MRS: AMR: Acked");

    return i;
}



// =============================================================================
// mrs_AmrRecord
// -----------------------------------------------------------------------------
/// Record audio in the file, and following the configuration specified in the
/// handle parameter.
/// @param handle Handle where to record the recorded data.
/// @param avrsAudioCfg Audio configuration for the record (Interface and
/// levels).
/// @param avrsVideoCfg Ignored. Should be kept to NULL.
///
// =============================================================================
PRIVATE BOOL mrs_AmrRecord(MRS_HANDLE_T handle,
                           AVRS_AUDIO_CFG_T* avrsAudioCfg,
                           AVRS_VIDEO_CFG_T* avrsVideoCfg)
{
#ifdef MRS_USES_AVRS
    // We only record audio.
    if(avrs_Record(&handle->audioStream, NULL, avrsAudioCfg, avrsVideoCfg) == AVRS_ERR_NO)
    {
        return TRUE;
    }
    // else
    return FALSE;


#else
    ARS_ENC_STREAM_T stream;
    ARS_AUDIO_CFG_T  arsAudioCfg;

    // Audio Stream for ARS.
    stream.startAddress = (UINT32)handle->audioStream.startAddress;
    stream.length =  handle->audioStream.length;
    stream.mode = handle->audioStream.mode;
    stream.sampleRate = handle->audioStream.sampleRate;
    stream.channelNb = handle->audioStream.channelNb;
    stream.voiceQuality = handle->audioStream.voiceQuality;
    stream.handler = mrs_AmrArsHandler;

    // Audio Configuration for ARS.
    arsAudioCfg.micLevel = avrsAudioCfg->micLevel;
    arsAudioCfg.spkLevel = AUD_SPK_MUTE;
    arsAudioCfg.filter = NULL;

    g_mrsAmrMsgAcked = TRUE;

    // TODO The handle also holds the interface configuration (itf id)
    // What should we do with that ?
    if (ars_Record(avrsAudioCfg->itf, &stream, &arsAudioCfg, TRUE) == ARS_ERR_NO)
    {
        return TRUE;
    }
    return FALSE;
#endif
}



