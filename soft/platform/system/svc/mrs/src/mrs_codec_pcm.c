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
/// Convert characters into the corresponding UINT32 used in the
/// RIFF headers.
#define MPS_RIFF_ID(A, B, C, D) \
    (((A)<<0)|((B)<<8)|((C)<<16)|((D)<<24))

/// Size of the file header, composed of both the 'real' file header and the
/// data packet header.
#define MRS_CODEC_PCM_WAVE_FILE_HEADER_SIZE \
    (sizeof(MRS_CODEC_PCM_WAVE_HEADER_T) + sizeof(MRS_CODEC_PCM_WAVE_DATA_HEADER_T))

// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// MRS_CODEC_PCM_WAVE_HEADER_T
// -----------------------------------------------------------------------------
/// PCM file header.
// =============================================================================
typedef struct
{
    /// "RIFF"
    UINT32  szRiff;
    /// file size
    UINT32  dwFileSize;
    /// "WAVEfmt "
    UINT32  szWaveFmt[2];
    /// 16
    UINT32  dwFmtSize;
    /// format type
    UINT16  wFormatTag;
    /// number of channels (i.e. mono, stereo...)
    UINT16  nChannels;
    /// sample rate
    UINT32  nSamplesPerSec;
    /// for buffer estimation
    UINT32  nAvgBytesPerSec;
    /// block size of data
    UINT16  nBlockAlign;
    /// number of bits per sample of mono data
    UINT16  wBitsPerSample;
} MRS_CODEC_PCM_WAVE_HEADER_T;


// =============================================================================
// MRS_CODEC_PCM_WAVE_DATA_HEADER_T
// -----------------------------------------------------------------------------
/// PCM data header.
// =============================================================================
typedef struct
{
    /// "data"
    UINT8   szData[4];
    /// pcm data size
    UINT32  dwDataSize;
} MRS_CODEC_PCM_WAVE_DATA_HEADER_T;


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================
#ifndef MRS_USES_AVRS
// FIXME Remove after portage on AVRS.
PRIVATE MRS_HANDLE_T g_mrsPcmHandle = NULL;

/// Prevent message sending as long as the answer has not been
/// acknowledged.
PRIVATE BOOL g_mrsPcmMsgAcked = FALSE;
#endif

// =============================================================================
// PRIVATE PROTOTYPE FUNCTIONS
// =============================================================================

PRIVATE UINT32 mrs_PcmWriteAudioData(MRS_HANDLE_T handle, UINT8* data,
                                     UINT32 size);

PRIVATE MRS_ERR_T mrs_PcmOpen(MRS_HANDLE_T handle, MRS_FILE_MODE_T mode, VOID** data);

PRIVATE VOID mrs_PcmClose(MRS_HANDLE_T handle);

PRIVATE VOID mrs_PcmStop(MRS_HANDLE_T handle);

PRIVATE BOOL mrs_PcmRecord( MRS_HANDLE_T handle,
                            AVRS_AUDIO_CFG_T* avrsAudioCfg,
                            AVRS_VIDEO_CFG_T* avrsVideoCfg);


// =============================================================================
// GLOBAL VARIABLES - Needing private functions
// =============================================================================

PROTECTED MRS_CODEC_T g_mrsPcmCodec =
{
    .open               = mrs_PcmOpen,
    .close              = mrs_PcmClose,
    .record             = mrs_PcmRecord,
    .stop               = mrs_PcmStop,
    .writeAudioData     = mrs_PcmWriteAudioData,
    .writeVideoData     = NULL,
    .skipVideo          = NULL,
    .seek               = NULL, // TODO Implement if needed
    .tell               = NULL, // TODO Implement if needed
    .audioCodec         = MRS_AUDIO_CODEC_WAV

};


// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

// =============================================================================
// mrs_PcmWriteWaveHeader
// -----------------------------------------------------------------------------
/// Write the wave header at the beginning of the medium. That header is medium
/// size dependent, so this function shall be called after the record is over.
///
/// @param medium Pointer to the wave medium were to write the header.
/// @return #MRS_ERR_NO, when everything is fine ; #MRS_ERR_FILE_ERROR, when a
/// problem occured while accessing the media.
// =============================================================================
PRIVATE MRS_ERR_T mrs_PcmWriteWaveHeader(MRS_MEDIUM_T* medium)
{
    UINT32 fileSize = 0;
    INT32 result = -1;
    MRS_CODEC_PCM_WAVE_HEADER_T waveFormatHeader;
    MRS_CODEC_PCM_WAVE_DATA_HEADER_T waveDataHeader;



    fileSize = mrs_MediumSize(medium);
    result   = mrs_MediumSeek(medium, 0, FS_SEEK_SET);
    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_PcmWriteWaveHeader, fileSize is %d, result is %d", fileSize, result);
    if(result  < 0 || fileSize < 0)
    {
        return MRS_ERR_FILE_ERROR;
    }

    // Wave header
    // FIXME Some size constants may come from the header sizes.
    memcpy(&waveFormatHeader.szRiff,"RIFF",4);
    waveFormatHeader.dwFileSize = fileSize - 8;
    memcpy(waveFormatHeader.szWaveFmt,"WAVEfmt ",8);
    waveFormatHeader.dwFmtSize = 16;
    waveFormatHeader.wFormatTag = 1;
    waveFormatHeader.nChannels = HAL_AIF_MONO;
    waveFormatHeader.nSamplesPerSec = HAL_AIF_FREQ_8000HZ;
    waveFormatHeader.wBitsPerSample = 16;
    waveFormatHeader.nAvgBytesPerSec =
        waveFormatHeader.nChannels
        * waveFormatHeader.wBitsPerSample
        * waveFormatHeader.nSamplesPerSec/8;
    waveFormatHeader.nBlockAlign = waveFormatHeader.nChannels*waveFormatHeader.wBitsPerSample/8;

    // Write wave header.
    result = mrs_MediumWrite(medium, &waveFormatHeader, sizeof(MRS_CODEC_PCM_WAVE_HEADER_T));
    if(result!=sizeof(MRS_CODEC_PCM_WAVE_HEADER_T))
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "MRS: Write PCM header failed: %d", result);
        return MRS_ERR_FILE_ERROR;
    }

    // Wav data header.
    memcpy(waveDataHeader.szData,"data",4);
    waveDataHeader.dwDataSize = fileSize - MRS_CODEC_PCM_WAVE_FILE_HEADER_SIZE;

    // Write wav data header
    result = mrs_MediumWrite(medium,&waveDataHeader,sizeof(MRS_CODEC_PCM_WAVE_DATA_HEADER_T));
    if(result!=sizeof(MRS_CODEC_PCM_WAVE_DATA_HEADER_T))
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "MRS: Write PCM Data header failed: %d", result);
        return MRS_ERR_FILE_ERROR;
    }

    // Seek back to the end. If the record was paused, it can be resumed !
    result   = mrs_MediumSeek(medium, 0, FS_SEEK_END);
    if(result  < 0)
    {
        return MRS_ERR_FILE_ERROR;
    }

    return MRS_ERR_NO;
}


#ifndef MRS_USES_AVRS
// =============================================================================
// mrs_PcmArsHandler
// -----------------------------------------------------------------------------
/// (ARS handler configured to mimic AVRS behaviour)
/// @todo Use AVRS and remove that guy.
/// Recording state handler for ARS: at half buffer and end buffer, send a
/// message to the MRS task with the pointer configured to the end of the
/// proper half.
// =============================================================================
PRIVATE VOID mrs_PcmArsHandler(ARS_STATUS_T status)
{
    AVRS_EVENT_T ev;

    // HYP: g_mrsPcmHandle is not NULL at that time.
    MRS_TRACE(MRS_INFO_TRC, 0, "sndrec_DrvHandler,status is %d, ack=%d",status, g_mrsPcmMsgAcked);

    if (g_mrsPcmMsgAcked == TRUE)
    {
        if(status == ARS_STATUS_MID_BUFFER_REACHED)
        {
            ev.id           = AVRS_EVENT_ID_AUDIO;
            // First half has finished.
            ev.bufferPos    = (UINT32*)(g_mrsPcmHandle->audioStream.startAddress
                                        + g_mrsPcmHandle->audioStream.length/2);
            ev.frameSize    = 0;
            ev.reserved[0]  = 0;
            sxr_Send(&ev, g_mrsPcmHandle->audioStream.mbx, SXR_SEND_EVT);
            //      ars_ReadData(SNDRECBUF_LEN*2);
            MRS_TRACE(MRS_INFO_TRC, 0, "MRS: PCM: Wait Ack");
            g_mrsPcmMsgAcked = FALSE;
        }
        else if(status == ARS_STATUS_END_BUFFER_REACHED)
        {
            ev.id           = AVRS_EVENT_ID_AUDIO;
            // Second half has finished, we are thus at the beginning again.
            ev.bufferPos    = (UINT32*)(g_mrsPcmHandle->audioStream.startAddress);
            ev.frameSize    = 32;
            ev.reserved[0]  = 0;
            sxr_Send(&ev, g_mrsPcmHandle->audioStream.mbx, SXR_SEND_EVT);
            //      ars_ReadData(SNDRECBUF_LEN*2);
            MRS_TRACE(MRS_INFO_TRC, 0, "MRS: PCM: Wait Ack");
            g_mrsPcmMsgAcked = FALSE;
        }
    }
}
#endif

// =============================================================================
// mrs_PcmOpen
// -----------------------------------------------------------------------------
/// Open the PCM codec to record an PCM file, and complete the handle to use
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
PRIVATE MRS_ERR_T mrs_PcmOpen(MRS_HANDLE_T handle, MRS_FILE_MODE_T mode, VOID** data)
{
    // 'Blank' wave file header.
    UINT8 waveHeader[MRS_CODEC_PCM_WAVE_FILE_HEADER_SIZE]= {0};

    // Variable to receive the file header when appending,
    // for check.
    MRS_CODEC_PCM_WAVE_HEADER_T waveFormatHeader;

#ifndef MRS_USES_AVRS
    // FIXME
    g_mrsPcmHandle = handle;
#endif

    // FIXME Useful data ?
    *data = NULL;

    // Write the file header if the file is created
    if (mode == MRS_FILE_MODE_CREATE)
    {
        // File creation: The header will be written
        // at the end of the record as the file lenght
        // needs to be known. We write '0' in the header
        // place instead.
        if (mrs_MediumWrite(&handle->mediumAudio, waveHeader, MRS_CODEC_PCM_WAVE_FILE_HEADER_SIZE)>=0) // FIXME == MRS_CODEC_PCM_WAVE_FILE_HEADER_SIZE ?
        {
            MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: PCM header written.");
        }
        else
        {
            return MRS_ERR_FILE_ERROR;
        }
    }
    else
    {
        // File appending.
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: PCM appending data.");
        // Check the validity of the file format
        if (mrs_MediumSeek(&handle->mediumAudio, 0, FS_SEEK_SET)>=0)
        {
            if (mrs_MediumRead(&handle->mediumAudio, &waveFormatHeader, sizeof(waveFormatHeader))>=0)
            {
                MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: PCM header read (?).");

                if (waveFormatHeader.szRiff         != MPS_RIFF_ID('R', 'I', 'F', 'F')
                        || waveFormatHeader.szWaveFmt[0]   != MPS_RIFF_ID('W', 'A', 'V', 'E')
                        || waveFormatHeader.dwFmtSize      != 16
                        || waveFormatHeader.wFormatTag     != 1
                        || waveFormatHeader.nChannels      != HAL_AIF_MONO
                        || waveFormatHeader.nSamplesPerSec != HAL_AIF_FREQ_8000HZ
                        || waveFormatHeader.wBitsPerSample != 16)
                {
                    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: PCM header read: BAD CODEC !");
                    return MRS_ERR_INCOMPATIBLE_AUDIO_CODEC;
                }
                else
                {
                    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: PCM header read (OK).");
                }
            }
            else
            {
                MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: PCM header read FAILED !");
                return MRS_ERR_FILE_ERROR;
            }
        }
        else
        {
            MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: PCM header seek failed.");
            return MRS_ERR_FILE_ERROR;
        }

        // Reset pointer at the end of the file to add new data
        // with the codec following calls. (record, etc)
        if (mrs_MediumSeek(&handle->mediumAudio, 0, FS_SEEK_END) <0)
        {
            MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: PCM .");
            return MRS_ERR_FILE_ERROR;
        }
    }


    handle->videoLength                 = 0;
    handle->videoBuffer                 = 0;
    handle->audioLength                 = 16000; // Half a buffer is worth half a second of data.
    handle->audioBuffer                 = (UINT8*) sxr_Malloc(handle->audioLength);
    handle->audioStream.mode            = AVRS_REC_AUDIO_MODE_PCM;
    handle->audioStream.sampleRate      = HAL_AIF_FREQ_8000HZ;
    handle->audioStream.channelNb       = HAL_AIF_MONO;
    handle->audioStream.voiceQuality    = TRUE;

    return MRS_ERR_NO;
}


// =============================================================================
// mrs_PcmStop
// -----------------------------------------------------------------------------
/// Stop the recording of PCM.
/// This function will stop the recording through AVRS. The remaining
/// information in the buffer is flushed following the information sent
/// with the closing of the recording layer.
///
/// @param handle Handle of the file to close.
// =============================================================================
PRIVATE VOID mrs_PcmStop(MRS_HANDLE_T handle)
{
#ifdef MRS_USES_AVRS
    AVRS_ERR_T errorStatus;
    errorStatus = avrs_Stop();
#else
    ARS_ERR_T errorStatus;
    errorStatus = ars_Stop();
#endif

    // Write Wave file header.
    mrs_PcmWriteWaveHeader(&handle->mediumAudio);

    MRS_TRACE(MRS_INFO_TRC, 0,  "mrs_PcmStop called a(v?)rs_Stop and got status:%#x", errorStatus);
//    avrs_Stop(&handle->bufferContext);
}


// =============================================================================
// mrs_PcmClose
// -----------------------------------------------------------------------------
/// Close the PCM codec.
/// This function frees the buffer used to record the data, so ensure everything
/// reliable in the buffer has been saved before. (ie: call #mrs_PcmStop before
/// that function)
/// @todo Assert/Error ensure the correct behaviour ?
/// @param handle Handle of the file to close.
// =============================================================================
PRIVATE VOID mrs_PcmClose(MRS_HANDLE_T handle)
{
    MRS_TRACE(MRS_INFO_TRC, 0,  "mrs_PcmClose called.");

    if (handle->audioBuffer != NULL)
    {
        sxr_Free(handle->audioBuffer);
        handle->audioBuffer = NULL;
    }

#ifndef MRS_USES_AVRS
    g_mrsPcmMsgAcked = FALSE;
    g_mrsPcmHandle = NULL;
#endif
}


// =============================================================================
// mrs_PcmWriteAudioData
// -----------------------------------------------------------------------------
/// Write audio data from a buffer to the file.
// =============================================================================
PRIVATE UINT32 mrs_PcmWriteAudioData(MRS_HANDLE_T handle, UINT8* data, UINT32 size)
{
    INT32 i;

    i = mrs_MediumWrite(&handle->mediumAudio, data, size);

    if(i < 0)
    {
        i = 0;
        // Todo Return an error and handle it upperer ?
    }
#ifndef MRS_USES_AVRS
    g_mrsPcmMsgAcked = TRUE;
    MRS_TRACE(MRS_INFO_TRC, 0, "MRS: PCM: Acked");
#endif

    return i;
}



// =============================================================================
// mrs_PcmRecord
// -----------------------------------------------------------------------------
/// Record audio in the file, and following the configuration specified in the
/// handle parameter.
/// @param handle Handle where to record the recorded data.
/// @param avrsAudioCfg Audio configuration for the record (Interface and
/// levels).
/// @param avrsVideoCfg Ignored. Should be kept to NULL.
///
// =============================================================================
PRIVATE BOOL mrs_PcmRecord(MRS_HANDLE_T handle,
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
    stream.handler = mrs_PcmArsHandler;

    // Audio Configuration for ARS.
    arsAudioCfg.micLevel = avrsAudioCfg->micLevel;
    arsAudioCfg.spkLevel = AUD_SPK_MUTE;
    arsAudioCfg.filter = NULL;

    g_mrsPcmMsgAcked = TRUE;

    // TODO The handle also holds the interface configuration (itf id)
    // What should we do with that ?
    if (ars_Record(avrsAudioCfg->itf, &stream, &arsAudioCfg, TRUE) == ARS_ERR_NO)
    {
        return TRUE;
    }
    return FALSE;
#endif
}


// =============================================================================
// FUNCTIONS
// =============================================================================


