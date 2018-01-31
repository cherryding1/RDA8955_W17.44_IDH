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

#include "avrs_m.h"

#ifndef MRS_USES_AVRS
#include "ars_m.h"
#endif

#include "sxr_ops.h"
#include "sxs_io.h"

#include "mrs_m.h"
#include "mrsp.h"

#include "mrsp_debug.h"

#include "aud_m.h"
#include "lcdd_m.h"

#include "string.h"

#include "hal_sys.h"
#include "hal_host.h"
#include "hal_debug.h"

#include "mrsp_codec.h"
#include "mrsp_codec_mp3.h"
#include "mrsp_codec_amr.h"
#include "mrsp_codec_pcm.h"
#include "mrsp_codec_riff.h"


// =============================================================================
// MACROS
// =============================================================================

#define MRS_HEADER_ID 0x9357EE53

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// MRS_CMD_T
// -----------------------------------------------------------------------------
/// List of action on mrs task
// =============================================================================
typedef enum
{
    /// Play stream if it's possible
    MRS_CMD_RECORD,
    /// Free all ressources and stop stream
    MRS_CMD_CLOSE,
    /// Pause the stream
    MRS_CMD_PAUSE,
    /// Jump in new position in file
    MRS_CMD_SEEK,
    /// Lauch pause play for audio path switch
    MRS_CMD_RESET
} MRS_CMD_T;

// =============================================================================
// MRS_BODY_MSG_T
// -----------------------------------------------------------------------------
/// Internal message structure
// =============================================================================
typedef struct
{
    MRS_CMD_T       cmd;
    MRS_HANDLE_T    handle;
    UINT32          time;
} MRS_BODY_MSG_T ;


typedef union
{
    MRS_BODY_MSG_T  mrs;
} MsgBody_t;

#include "itf_msg.h"


// =============================================================================
// PRIVATE VARIABLES
// =============================================================================
// TODO Privatise
PROTECTED sxr_TaskDesc_t               g_mrsTask;
PROTECTED sxr_TaskDesc_t               g_mrsTaskAudio;
PROTECTED UINT8                        g_mrsIdTask = 0;
PROTECTED UINT8                        g_mrsIdTaskAudio = 0;
PROTECTED UINT8                        g_mrsMbx = 0xFF;
PROTECTED UINT8                        g_mrsMbxAudio = 0xFF;

PROTECTED VOLATILE MRS_HANDLE_T        g_mrsCurrentHandle = 0;

PROTECTED MRS_CODEC_T*                 g_mrsCodecList[] =
{
    &g_mrsAmrCodec,
    &g_mrsPcmCodec,
    0,//&g_mp3Codec,
    0,//&g_riffCodec,
    0
};

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

/// Read pointer in the buffer shared with AVRS.
PROTECTED UINT8*           g_mrsBufferReadPos   = 0;

/// Write pointer in the buffer shared with AVRS.
PROTECTED UINT8*           g_mrsBufferWritePos  = 0;


// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

// =============================================================================
// mrs_TaskAudio
// -----------------------------------------------------------------------------
/// This task is dedicated to the audio management. The main MRS task (mrs_Task)
/// will forward audio related messages to this task. The reason is
/// @todo Ask Lilian the reason.
// =============================================================================
PRIVATE VOID mrs_TaskAudio(VOID* param)
{
//    Msg_t           msg;
    UINT32          size;
    UINT32          j;
    AVRS_EVENT_T    avrsEvent;
    while(1)
    {
        if(sxr_Wait((UINT32*)&avrsEvent, g_mrsMbxAudio))
        {
            continue;
        }
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_TaskAudio: new message");

        // It is obviously an audio (from AVRS) message
        // (It is only a private internal thing.)

        // Write new audio data
        g_mrsBufferWritePos = (UINT8*)avrsEvent.bufferPos;
        if(g_mrsBufferReadPos == 0)
        {
            g_mrsBufferReadPos  = g_mrsCurrentHandle->audioBuffer;
        }
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Task: Read pos %#x Write pos %#x",
                  g_mrsBufferReadPos, g_mrsBufferWritePos);

        MRS_ASSERT(g_mrsBufferWritePos == 0 ||
                   ((UINT32)g_mrsBufferWritePos >= (UINT32)g_mrsCurrentHandle->audioBuffer &&
                    (UINT32)g_mrsBufferWritePos < (UINT32)g_mrsCurrentHandle->audioBuffer + g_mrsCurrentHandle->audioLength),
                   "Read pointer out of range");
        size = 0;

// FIXME: In order to test the MP3 recording,
// we will temporarily not use a stream buffer but
// directly the encoded swap buffer.
// The fact that this swap buffer is used is
// detected a non zero frame size.
// This special case shall be removed in the final
// implementation.
        if (avrsEvent.frameSize != 0)
        {
            // special MP3 case to merge into STD one
            size += g_mrsCurrentHandle->codec->writeAudioData(g_mrsCurrentHandle, (UINT8*)avrsEvent.bufferPos,
                    avrsEvent.frameSize);

        }
        else
        {
            // Normal case unindented on purpose
            if((UINT32)g_mrsBufferWritePos <= (UINT32)g_mrsBufferReadPos)
            {
                size += g_mrsCurrentHandle->codec->writeAudioData(g_mrsCurrentHandle, g_mrsBufferReadPos,
                        g_mrsCurrentHandle->audioLength +
                        (UINT32)g_mrsCurrentHandle->audioBuffer -
                        (UINT32)g_mrsBufferReadPos);
                g_mrsBufferReadPos           += size;

                // Wrap ??
                if((UINT32)g_mrsBufferReadPos ==
                        (UINT32)g_mrsCurrentHandle->audioBuffer + g_mrsCurrentHandle->audioLength)
                {
                    g_mrsBufferReadPos = g_mrsCurrentHandle->audioBuffer;
                }
            }

            if(size !=  g_mrsCurrentHandle->audioLength &&
                    (UINT32)g_mrsBufferWritePos > (UINT32)g_mrsBufferReadPos)
            {
                j = g_mrsCurrentHandle->codec->writeAudioData(g_mrsCurrentHandle, g_mrsBufferReadPos,
                        (UINT32)g_mrsBufferWritePos - (UINT32)g_mrsBufferReadPos);
                g_mrsBufferReadPos      += j;
                size                    += j;
            }
            if((UINT32)g_mrsBufferReadPos ==
                    (UINT32)g_mrsCurrentHandle->audioBuffer + g_mrsCurrentHandle->audioLength)
            {
                g_mrsBufferReadPos = g_mrsCurrentHandle->audioBuffer;
            }
        }
        if(size == 0)
        {
            // Stop recording
            g_mrsCurrentHandle->codec->stop(g_mrsCurrentHandle);

            MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Task: No more room for data");
            MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Task: End of file");

            g_mrsCurrentHandle->bufferContext.audioRemainingSize      = 0;
            g_mrsCurrentHandle->bufferContext.bufferAudioReadPosition = g_mrsCurrentHandle->audioBuffer;


            if(g_mrsCurrentHandle->callback)
            {
                g_mrsCurrentHandle->callback((MRS_HANDLE_T)g_mrsCurrentHandle,
                                             MRS_STATE_EOF);
            }

            // Close the codec.
            g_mrsCurrentHandle->codec->close(g_mrsCurrentHandle);

            mrs_MediumClose(&g_mrsCurrentHandle->mediumAudio);
            mrs_MediumClose(&g_mrsCurrentHandle->mediumVideo);

            sxr_Free(g_mrsCurrentHandle);
            g_mrsCurrentHandle = 0;

        }
        else
        {
            MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Task: AudioAddedData %i", size);
#ifdef MRS_USES_AVRS
            avrs_ReadData(size);
#else
            ars_ReadData(size);
#endif
        }
    }
}

// =============================================================================
// mrs_Task
// -----------------------------------------------------------------------------
/// MRS Main task. All audio processing is forwarded to mrs_TaskAudio
// =============================================================================
PRIVATE VOID mrs_Task(VOID* param __attribute__((unused)))
{
    Msg_t               msg;
    AVRS_AUDIO_CFG_T    avrsAudioCfg;
    AVRS_VIDEO_CFG_T    avrsVideoCfg;

    MRS_PROFILE_FUNCTION_ENTER(mrs_task);

    while(1)
    {
        if(sxr_Wait((UINT32*)&msg, g_mrsMbx))
        {
            continue;
        }

        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Task: new message");
        // Internal message
        if(msg.H.Id == MRS_HEADER_ID)
        {
            MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Task: internal msg cmd=%i handle=%#x",
                      msg.B.mrs.cmd, msg.B.mrs.handle);

            MRS_ASSERT(msg.B.mrs.handle, "mrs_Task: msg handle == NULL");
            switch(msg.B.mrs.cmd)
            {
                case MRS_CMD_RECORD:
                    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Task: play cmd");

                    if(g_mrsCurrentHandle == 0)
                    {
                        g_mrsBufferReadPos   = msg.B.mrs.handle->audioBuffer;
                        g_mrsBufferWritePos  = msg.B.mrs.handle->audioBuffer;

                        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Task: Read pos %#x Write pos %#x",
                                  g_mrsBufferReadPos, g_mrsBufferWritePos);

                        avrsAudioCfg.itf        = msg.B.mrs.handle->config.audio.audioPath;
                        avrsAudioCfg.equalizer  = msg.B.mrs.handle->config.audio.equalizerMode;
                        avrsAudioCfg.micLevel   = msg.B.mrs.handle->config.audio.volume;
                        avrsVideoCfg.rotation   = msg.B.mrs.handle->config.video.videoRotation;

                        if(msg.B.mrs.handle->codec->record(msg.B.mrs.handle, &avrsAudioCfg, &avrsVideoCfg) == TRUE)
                        {
                            g_mrsCurrentHandle = msg.B.mrs.handle;

                            if(msg.B.mrs.handle->callback)
                            {
                                msg.B.mrs.handle->callback(msg.B.mrs.handle,
                                                           MRS_STATE_RECORD);
                            }
                        }
                    }
                    break;

                case MRS_CMD_SEEK:
                    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Task: seek cmd");

                    if(g_mrsCurrentHandle == msg.B.mrs.handle)
                    {
                        msg.B.mrs.handle->codec->seek(msg.B.mrs.handle, msg.B.mrs.time);
                        msg.B.mrs.handle->codec->record(msg.B.mrs.handle, &avrsAudioCfg, &avrsVideoCfg);
                    }
                    else
                    {
                        msg.B.mrs.handle->codec->seek(msg.B.mrs.handle, msg.B.mrs.time);
                    }

                    if(msg.B.mrs.handle->callback)
                    {
                        msg.B.mrs.handle->callback(msg.B.mrs.handle, MRS_STATE_SEEK);
                    }
                    break;

                case MRS_CMD_RESET:
                    MRS_TRACE(MRS_INFO_TRC, 0, "mps_Task: reset cmd");
                    if(g_mrsCurrentHandle == msg.B.mrs.handle)
                    {
                        // Stop the record.
                        g_mrsCurrentHandle->codec->stop(g_mrsCurrentHandle);
                        avrsAudioCfg.itf        = msg.B.mrs.handle->config.audio.audioPath;
                        avrsAudioCfg.equalizer  = msg.B.mrs.handle->config.audio.equalizerMode;
                        avrsAudioCfg.micLevel   = msg.B.mrs.handle->config.audio.volume;
                        avrsVideoCfg.rotation   = msg.B.mrs.handle->config.video.videoRotation;

                        // Start record with new configuration.
                        g_mrsCurrentHandle->codec->record(g_mrsCurrentHandle, &avrsAudioCfg, &avrsVideoCfg);
                    }
                    break;


                case MRS_CMD_CLOSE:
                    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Task: stop cmd");

                    // Stop the record and close the codec.
                    msg.B.mrs.handle->codec->stop(msg.B.mrs.handle);

                    // FIXME TODO Recover the content of the buffer
                    // before closing the codec (For ARS, get back the
                    // last half ?)
                    msg.B.mrs.handle->codec->close(msg.B.mrs.handle);

                    if(g_mrsCurrentHandle == msg.B.mrs.handle)
                    {
                        g_mrsCurrentHandle        = 0;
                    }
                    mrs_MediumClose(&msg.B.mrs.handle->mediumAudio);
                    mrs_MediumClose(&msg.B.mrs.handle->mediumVideo);

                    if(msg.B.mrs.handle->callback)
                    {
                        msg.B.mrs.handle->callback(msg.B.mrs.handle, MRS_STATE_STOP);
                    }

                    sxr_Free(msg.B.mrs.handle);
                    break;

                case MRS_CMD_PAUSE:
                    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Task: pause cmd");

                    if(g_mrsCurrentHandle == msg.B.mrs.handle)
                    {
                        // Pause is implemented by stopping the record, then
                        // starting is again (codec 'stop' and 'record'
                        // functions respectively.)
                        MRS_ASSERT(g_mrsCurrentHandle->codec->stop, "mrs_Task: stop not implemented");
                        g_mrsCurrentHandle->codec->stop(g_mrsCurrentHandle);

                        if(msg.B.mrs.handle->callback)
                        {
                            msg.B.mrs.handle->callback(msg.B.mrs.handle, MRS_STATE_PAUSE);
                        }
                        g_mrsCurrentHandle        = 0;
                    }

                    break;
            }
        }

        if(g_mrsCurrentHandle == 0)
        {
            continue;
        }

        switch(msg.H.Id)
        {
            case AVRS_EVENT_ID_AUDIO:
                sxr_Send(&msg, g_mrsMbxAudio, SXR_SEND_EVT);
                break;

            case AVRS_EVENT_ID_VIDEO:
                // Write the new video data
                // FIXME IMPLEMENT
#if 0
                // AVRS will switch along the two halves of the video
                // buffer to use it as a ping-pong buffer. Frame will
                // be alternatively fetched in an half, then the other,
                // at the will of the underlying AVRS.
                MRS_ASSERT(g_mrsCurrentHandle->codec->getVideoData, "mrs: getVideoData not implemented in codec");
                if(g_mrsCurrentHandle->codec->getVideoData(g_mrsCurrentHandle,
                        (UINT8*)msg.B.avrs.bufferPos,
                        g_mrsCurrentHandle->videoLength/2) != 0)
                {
                    avrs_VideoAddedData();
                }
#endif
                break;

#if 0
            case AVRS_HEADER_ID_VIDEO_DISPLAY:
                // The pointer embodied in this message is of a frame buffer
                // window to display.
                MRS_PROFILE_FUNCTION_ENTER(mrs_blit);
                lcdd_Blit16((LCDD_FBW_T*)msg.B.avrs.bufferPos,
                            g_mrsCurrentHandle->config.x, g_mrsCurrentHandle->config.y);
                MRS_PROFILE_FUNCTION_EXIT(mrs_blit);
                break;

            case AVRS_HEADER_ID_VIDEO_SKIP:
                MRS_ASSERT(g_mrsCurrentHandle->codec->skipVideo, "mrs: skipVideo not implemented in codec");
                break;
#endif
        }
    }

    MRS_PROFILE_FUNCTION_EXIT(mrs_task);
}

// =============================================================================
// FUNCTIONS
// =============================================================================
// =============================================================================
// mrs_Open
// -----------------------------------------------------------------------------
/// Open media player service. Create the MRS task.
// =============================================================================
PUBLIC VOID mrs_Open(VOID)
{
    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Open");

    if(g_mrsIdTask == 0)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Open: Create task");
        g_mrsCurrentHandle     = 0;
        g_mrsTask.TaskBody  = mrs_Task;
        g_mrsTask.TaskExit  = 0;
        g_mrsTask.Name      = "MRS";
        g_mrsTask.StackSize = 128 * 3; // FIXME Rationalize
        g_mrsTask.Priority  = 50;
        g_mrsMbx            = sxr_NewMailBox();
        g_mrsIdTask         = sxr_NewTask(&g_mrsTask);

        g_mrsTaskAudio.TaskBody  = mrs_TaskAudio;
        g_mrsTaskAudio.TaskExit  = 0;
        g_mrsTaskAudio.Name      = "MRS Task Audio";
        g_mrsTaskAudio.StackSize = 384;
        g_mrsTaskAudio.Priority  = 51;
        g_mrsMbxAudio            = sxr_NewMailBox();
        g_mrsIdTaskAudio         = sxr_NewTask(&g_mrsTaskAudio);

        sxr_StartTask(g_mrsIdTask, 0);
        sxr_StartTask(g_mrsIdTaskAudio, 0);
    }
}


// =============================================================================
// mrs_Close.
// -----------------------------------------------------------------------------
/// Close the Media Record Service and, destroy the MRS task. If a media is
/// recorded, an error is returned (#MRS_ERR_INVALID). If MRS is not opened,
/// another error is returned (#MRS_ERR_SERVICE_NOT_OPEN).
/// @return #MRS_ERR_NO, #MRS_ERR_SERVICE_NOT_OPEN, #MRS_ERR_INVALID.
// =============================================================================
PUBLIC MRS_ERR_T mrs_Close(VOID)
{
    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Close");

    if(g_mrsIdTask == 0)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Close: Err=Not open");
        return MRS_ERR_SERVICE_NOT_OPEN;
    }

    if(g_mrsCurrentHandle)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Close: Err=Busy");
        return MRS_ERR_INVALID;
    }

    // TODO Remove that check of the stack
    // sizes once we are really confident as
    // of their sizes: They might be oversized.
    sxr_ChkTask(g_mrsIdTask);
    sxr_ChkTask(g_mrsIdTaskAudio);

    sxr_StopTask(g_mrsIdTaskAudio);
    sxr_FreeTask(g_mrsIdTaskAudio);
    sxr_FreeMailBox(g_mrsMbxAudio);
    g_mrsIdTaskAudio = 0;
    g_mrsMbxAudio    = 0xFF;

    sxr_StopTask(g_mrsIdTask);
    sxr_FreeTask(g_mrsIdTask);
    sxr_FreeMailBox(g_mrsMbx);
    g_mrsIdTask = 0;
    g_mrsMbx    = 0xFF;

    return MRS_ERR_NO;
}


// =============================================================================
// mrs_GetInfo
// -----------------------------------------------------------------------------
/// This function is used to return the information on file.
///
/// @param handle Handle use for this action
/// @return information on file (total length, bit rate, video or audio file,
// ...). See #MRS_FILE_INFO_T
// ============================================================================
PUBLIC MRS_FILE_INFO_T* mrs_GetInfo(MRS_HANDLE_T handle)
{
    MRS_ASSERT(handle, "mrs_GetInfo: handle == NULL");

    if(g_mrsIdTask == 0)
    {
        return 0;
    }

    return &(handle->info);
}

PUBLIC MRS_ERR_T mrs_OpenBuffer(MRS_HANDLE_T*  pHandle,
                                UINT8*         buffer,
                                UINT32         size,
                                MRS_CALLBACK_T callback,
                                MRS_CFG_T*  config)
{
#if 0
    MRS_HANDLE_T               handle;
    int                        i;
    VOID*                      codecData = 0;

    MRS_ASSERT(pHandle, "mrs_OpenBuffer: pHandler == NULL");
    MRS_ASSERT(buffer,  "mrs_OpenBuffer: buffer == NULL");
    MRS_ASSERT(config,  "mrs_OpenBuffer: config == NULL");

    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenBuffer buffer=%#x", buffer);

    if(g_mrsIdTask == 0)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenBuffer: Err=Not open");
        *pHandle = 0;
        return MRS_ERR_SERVICE_NOT_OPEN;
    }

    *pHandle = (MRS_HANDLE_T)sxr_Malloc(sizeof(struct MRS_HANDLE_STRUCT_T));
    handle   = *pHandle;
    memset(&handle->info, 0, sizeof(handle->info));

    mrs_MediumOpenBuffer(&handle->mediumAudio, buffer, size);
    mrs_MediumOpenBuffer(&handle->mediumVideo, buffer, size);

    memcpy(&handle->config, config, sizeof(MRS_CFG_T));

    handle->stream.mbx  = g_mrsMbx;
    handle->callback    = callback;
    handle->stream.mode = AVRS_RECORD_MODE_INVALID;

    for(i = 0; g_mrsCodecList[i]; ++i)
    {
        mrs_MediumSeek(&handle->mediumAudio, 0, FS_SEEK_SET);
        if((codecData = g_mrsCodecList[i]->open(handle)) != NULL)
        {
            break;
        }
    }

    handle->codec               = g_mrsCodecList[i];
    handle->codecData           = codecData;
    handle->stream.audioAddress = (UINT32)handle->audioBuffer;
    handle->stream.videoAddress = (UINT32)handle->videoBuffer;
    handle->stream.audioLength  = handle->audioLength;
    handle->stream.videoLength  = handle->videoLength;

    handle->bufferContext.bufferAudioReadPosition = handle->audioBuffer;
    handle->bufferContext.audioRemainingSize      = 0;

    if(handle->codec == NULL)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenBuffer: Err=Format not supported");
        sxr_Free(handle);
        *pHandle = 0;
        return MRS_ERR_UNKNOWN_FORMAT;
    }
#endif
    return MRS_ERR_NO;
}


PUBLIC MRS_ERR_T mrs_CloseBuffer(MRS_HANDLE_T handle)
{
#if 0
    Msg_t  msg;

    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_CloseFile");

    if(handle == NULL)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_CloseFile: Err=handle null");
        return MRS_ERR_INVALID;
    }

    if(g_mrsIdTask == 0)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_CloseFile: Err=Not open");
        return MRS_ERR_SERVICE_NOT_OPEN;
    }

    // Only close an existing handle.
    msg.H.Id          = MRS_HEADER_ID;
    msg.B.mrs.cmd     = MRS_CMD_CLOSE;
    msg.B.mrs.handle  = handle;
    sxr_Send(&msg, g_mrsMbx, SXR_SEND_EVT);
#endif
    return MRS_ERR_NO;
}


// =============================================================================
// mrs_CloseFile
// -----------------------------------------------------------------------------
/// Close a file and free handle.
///
/// @param handle Handle use for this action
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if mrs is not open,
/// else #MRS_ERR_NO.
// =============================================================================
PUBLIC MRS_ERR_T mrs_CloseFile(MRS_HANDLE_T handle)
{
    Msg_t  msg;

    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_CloseFile");

    if(handle == NULL)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_CloseFile: Err=handle null");
        return MRS_ERR_INVALID;
    }

    if(g_mrsIdTask == 0)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_CloseFile: Err=Not open");
        return MRS_ERR_SERVICE_NOT_OPEN;
    }

    // Only close an existing handle.
    msg.H.Id          = MRS_HEADER_ID;
    msg.B.mrs.cmd     = MRS_CMD_CLOSE;
    msg.B.mrs.handle  = handle;
    sxr_Send(&msg, g_mrsMbx, SXR_SEND_EVT);
    return MRS_ERR_NO;
}



// =============================================================================
// mrs_OpenFile
// -----------------------------------------------------------------------------
/// Prepare a file for recording. If needed, create the file. If the file
/// already exists, its header is checked to verify compatibility with the
/// requested recording.
///
/// @param pHandle Pointer on address of handle allocated by MRS.
/// @param file Path of file.
/// @param mode Mode of the file opening/creation (Replace, append, etc ...)
/// @param config Describe the audio configuration and video configuration.
/// (See #MRS_RECORD_CFG_T)
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if MRS is not opened,
/// #MRS_ERR_FILE_ERROR if the file cannot be created or appended,
/// #MRS_ERR_INCOMPATIBLE_AUDIO_CODEC if the file already exists and must be
/// appened, but is from an incompatible audio codec with the one we want to
/// use, #MRS_ERR_INCOMPATIBLE_VIDEO_CODEC if the file already exists and must
/// be appened, but is from an incompatible audio codec with the one we want to
/// use. Otherwise #MRS_ERR_NO is returned.
/// @todo Is this 'open' name well chosen ? In a sense (replacing, appending,
/// it is, but for the basic 'record' feature, create could be more relevant).
// =============================================================================
PUBLIC MRS_ERR_T mrs_OpenFile(MRS_HANDLE_T*     pHandle,
                              UINT8*            file,
                              MRS_FILE_MODE_T   mode,
                              MRS_RECORD_CFG_T* config)
{
    MRS_HANDLE_T            handle;
    MRS_ERR_T               result = MRS_ERR_NO;
    INT32                   i;
    VOID*                   codecData = 0;

    MRS_ASSERT(pHandle, "mrs_OpenFile: pHandler == NULL");
    MRS_ASSERT(file,    "mrs_OpenFile: file == NULL");
    MRS_ASSERT(config,  "mrs_OpenFile: config == NULL");

    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile");

    if(g_mrsIdTask == 0)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: Err=Not open");
        *pHandle = 0;
        return MRS_ERR_SERVICE_NOT_OPEN;
    }

    // Create file handle
    *pHandle = (MRS_HANDLE_T)sxr_Malloc(sizeof(struct MRS_HANDLE_STRUCT_T));
    handle   = *pHandle;
    memset(&handle->info, 0, sizeof(handle->info));

    // Populate audio medium
    if(mrs_MediumOpenFs(&handle->mediumAudio, file, mode) < 0)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: Err=Open file failed FS_Err=%i",
                  handle->mediumAudio.data.fs.fd);
        sxr_Free(handle);
        *pHandle = 0;
        return MRS_ERR_FILE_ERROR;
    }

    // Populate video medium
    if(mrs_MediumOpenFs(&handle->mediumVideo, file, mode) < 0)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: Err=Open file failed FS_Err=%i",
                  handle->mediumVideo.data.fs.fd);
        sxr_Free(handle);
        *pHandle = 0;
        return MRS_ERR_FILE_ERROR;
    }

    // Set handle IO configuration
    handle->config.audio = config->audioCfg;
    handle->config.video = config->videoCfg;

    // Set event configuration for streams.
    handle->audioStream.mbx     = g_mrsMbx;
    handle->audioStream.mode    = AVRS_REC_AUDIO_MODE_INVALID;
    handle->videoStream.mbx     = g_mrsMbx;
    handle->audioStream.mode    = AVRS_REC_VIDEO_MODE_INVALID;
    handle->callback            = config->callback;

    // Load the proper codec.
    for(i = 0; g_mrsCodecList[i]; ++i)
    {
        // Check that this codec supports the required encoding.
        if (g_mrsCodecList[i]->audioCodec == config->audioCodecCfg->type)
        {
            handle->codec                       = g_mrsCodecList[i];
            break;
        }
    }

    if(handle->codec == NULL)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_OpenFile: Err=Format not supported");
        sxr_Free(handle);
        *pHandle = 0;
        return MRS_ERR_UNKNOWN_FORMAT;
    }

    // Open the medium.
    result  = g_mrsCodecList[i]->open(handle, mode, &codecData);

    if (result == MRS_ERR_NO)
    {
        // Fill the handle structure (With results from the
        // call to the codec 'open' function.)
        handle->codecData                   = codecData;
        handle->audioStream.startAddress    = (UINT32)handle->audioBuffer;
        handle->audioStream.length          = handle->audioLength;
        handle->videoStream.startAddress    = (UINT32)handle->videoBuffer;
        handle->videoStream.length          = handle->videoLength;

        handle->bufferContext.bufferAudioReadPosition = handle->audioBuffer;
        handle->bufferContext.audioRemainingSize      = 0;
    }

    return result;
}


// =============================================================================
// mrs_Record
// -----------------------------------------------------------------------------
/// Record a file or resume the recording of the file previously paused by
/// #mrs_Pause. The record parameters
/// have been configured when the file was opened.
///
/// @param handle Handle use for this action.
/// @param seek Position in the file after which to record.
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if MRS is not open,
/// #MRS_ERR_INVALID if MRS is already used also #MRS_ERR_NO.
/// @todo This seek parameter can conflict with the mode under which the file
/// has been opened.
/// @todo Define seek unit and a way to understand it at the human level:
/// position in a streamed file, chunk number for RIFF ... and why not a
/// position in a chunk.
// =============================================================================
PUBLIC MRS_ERR_T mrs_Record(MRS_HANDLE_T handle, UINT32 seek)
{
    Msg_t           msg;

    MRS_ASSERT(handle, "mrs_Record: handle == NULL");

    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Record");

    if(g_mrsIdTask == 0)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Record: mrs not open");
        return MRS_ERR_SERVICE_NOT_OPEN;
    }

    if(g_mrsCurrentHandle != 0)
    {
        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_Record: Busy");
        return MRS_ERR_INVALID;
    }

    // FIXME Implement the seek feature
    handle->nbPlayed         = seek;

    msg.H.Id                 = MRS_HEADER_ID;
    msg.B.mrs.cmd            = MRS_CMD_RECORD;
    msg.B.mrs.handle         = handle;
    sxr_Send(&msg, g_mrsMbx, SXR_SEND_EVT);

    return MRS_ERR_NO;
}

// =============================================================================
// mrs_Pause
// -----------------------------------------------------------------------------
/// Pause the recording of a file.
///
/// @param handle Handle use for this action
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if mrs is not open,
/// #MRS_ERR_INVALID if MRS don't play with this handle also #MRS_ERR_NO
/// @todo Add a parameter (pointer) to return the current seek position ?
// =============================================================================
PUBLIC MRS_ERR_T mrs_Pause(MRS_HANDLE_T handle)
{
    Msg_t           msg;

    MRS_ASSERT(handle, "mrs_Pause: handle == NULL");

    MRS_TRACE(MRS_INFO_TRC, 0, __func__);

    if(g_mrsIdTask == 0)
    {
        return MRS_ERR_SERVICE_NOT_OPEN;
    }

    if(g_mrsCurrentHandle != handle)
    {
        return MRS_ERR_INVALID;
    }

    msg.H.Id                 = MRS_HEADER_ID;
    msg.B.mrs.cmd            = MRS_CMD_PAUSE;
    msg.B.mrs.handle         = handle;
    sxr_Send(&msg, g_mrsMbx, SXR_SEND_EVT);
    return MRS_ERR_NO;
}


// =============================================================================
// mrs_Seek
// -----------------------------------------------------------------------------
/// Change the current position
///
/// @param handle Handle use for this action
/// @param time The new position in stream (second unit)
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if mrs is not open also #MRS_ERR_NO
// =============================================================================
PUBLIC MRS_ERR_T mrs_Seek(MRS_HANDLE_T handle, UINT32 time)
{
#if 0
    Msg_t           msg;

    MRS_ASSERT(handle, "mrs_Seeek: handle == NULL");

    MRS_TRACE(MRS_INFO_TRC, 0, __func__);

    if(g_mrsIdTask == 0)
    {
        return MRS_ERR_SERVICE_NOT_OPEN;
    }

    if(handle->codec->seek)
    {
        msg.H.Id                 = MRS_HEADER_ID;
        msg.B.mrs.cmd            = MRS_CMD_SEEK;
        msg.B.mrs.handle         = handle;
        msg.B.mrs.time           = time;
        sxr_Send(&msg, g_mrsMbx, SXR_SEND_EVT);
    }
#endif
    return MRS_ERR_NO;
}



// =============================================================================
// mrs_Tell
// -----------------------------------------------------------------------------
/// Get the current position
///
/// @param handle Handle use for this action
/// @param pos Pointeur use to store the current position in stream
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if mrs is not open also #MRS_ERR_NO
// =============================================================================
PUBLIC MRS_ERR_T mrs_Tell(MRS_HANDLE_T handle, UINT32* pos)
{
#if 0
    MRS_ASSERT(handle, "mrs_Tell: handle == NULL");
    MRS_ASSERT(pos, "mrs_Tell: pos == NULL");

    MRS_TRACE(MRS_INFO_TRC, 0, __func__);

    if(g_mrsIdTask == 0)
    {
        return MRS_ERR_SERVICE_NOT_OPEN;
    }

    *pos = 0;
    if(handle->codec->tell)
    {
        *pos = handle->codec->tell(handle);
    }
#endif
    return MRS_ERR_NO;
}


// =============================================================================
// mrs_SetConfig
// -----------------------------------------------------------------------------
/// Set audio volume
/// @param handle Handle use for this action
/// @param config New config must be apply
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if mrs is not open,
/// #MRS_ERR_INVALID if config is incorrect also #MRS_ERR_NO
// =============================================================================
PUBLIC MRS_ERR_T mrs_SetConfig(MRS_HANDLE_T handle, MRS_CFG_T* config)
{
#ifdef MRS_USES_AVRS
    AVRS_AUDIO_CFG_T avrsAudioCfg;
    AVRS_VIDEO_CFG_T avrsVideoCfg;
#else
    ARS_AUDIO_CFG_T     arsCfg;
#endif
    Msg_t      msg;

    MRS_ASSERT(handle, "mrs_SetConfig: handle == NULL");
    MRS_ASSERT(config, "mrs_SetConfig: config == NULL");
    MRS_ASSERT(config->audio.volume < AUD_MIC_VOL_QTY, "mrs_SetConfig: config.volume too high");

    MRS_TRACE(MRS_INFO_TRC, 0, __func__);

    if(g_mrsIdTask == 0)
    {
        return MRS_ERR_SERVICE_NOT_OPEN;
    }

    if(g_mrsCurrentHandle == handle)
    {
        if(g_mrsCurrentHandle->config.audio.volume        != config->audio.volume        ||
                g_mrsCurrentHandle->config.audio.equalizerMode != config->audio.equalizerMode ||
                g_mrsCurrentHandle->config.video.videoPath     != config->video.videoPath     ||
                g_mrsCurrentHandle->config.video.videoRotation != config->video.videoRotation)
// TODO           g_mrsCurrentHandle->config.video.effects       != config->video.effects)
        {
            MRS_TRACE(MRS_INFO_TRC, 0, "mrs_SetConfig: Set new config");
#ifdef MRS_USES_AVRS
            // Audio Setup
            avrsAudioCfg.itf        = (AUD_ITF_T) handle->config.audio.audioPath;
            avrsAudioCfg.micLevel   = config->audio.volume;
            avrsAudioCfg.equalizer  = config->audio.equalizerMode;

            // Video Setup
            avrsVideoCfg.rotation   = config->video.videoRotation;

            // Apply
            avrs_Setup(&avrsAudioCfg, &avrsVideoCfg);
#else
            arsCfg.micLevel = config->audio.volume;
            arsCfg.filter = NULL;

            // Set new config
            ars_Setup((AUD_ITF_T) handle->config.audio.audioPath, &arsCfg);
#endif
            // Record new config for the handle
            g_mrsCurrentHandle->config.audio.volume        = config->audio.volume;
            g_mrsCurrentHandle->config.audio.equalizerMode = config->audio.equalizerMode;
            g_mrsCurrentHandle->config.video.videoPath     = config->video.videoPath;
            g_mrsCurrentHandle->config.video.videoRotation = config->video.videoRotation;
//            g_mrsCurrentHandle->config.video.effects       = config->video.effects;

        }

        MRS_TRACE(MRS_INFO_TRC, 0, "mrs_SetConfig: path %i %i", handle->config.audio.audioPath, config->audio.audioPath);
        if(handle->config.audio.audioPath != config->audio.audioPath)
        {
            handle->config.audio.audioPath = config->audio.audioPath;
            msg.H.Id                 = MRS_HEADER_ID;
            msg.B.mrs.cmd            = MRS_CMD_RESET;
            msg.B.mrs.handle         = handle;
            sxr_Send(&msg, g_mrsMbx, SXR_SEND_EVT);
        }

    }

    return MRS_ERR_NO;
}


// =============================================================================
// mrs_GetConfig
// -----------------------------------------------------------------------------
/// Get the current config
/// @param handle Handle use for this action
/// @param config Pointer where the config is save
/// @return Return #MRS_ERR_SERVICE_NOT_OPEN if mrs is not open also #MRS_ERR_NO
// =============================================================================
PUBLIC MRS_ERR_T mrs_GetConfig(MRS_HANDLE_T handle, MRS_CFG_T* config)
{
#if 0
    MRS_ASSERT(handle, "mrs_SetConfig: handle == NULL");
    MRS_ASSERT(config, "mrs_SetConfig: config == NULL");

    MRS_TRACE(MRS_INFO_TRC, 0, __func__);

    if(g_mrsIdTask == 0)
    {
        return MRS_ERR_SERVICE_NOT_OPEN;
    }

    memcpy(config, &handle->config, sizeof(MRS_CFG_T));
#endif
    return MRS_ERR_NO;
}


// =============================================================================
// mrs_GetFrequencyBars
// -----------------------------------------------------------------------------
/// Get frequency bars
/// @return Return pointer on frequency bars
// =============================================================================
PUBLIC MRS_FREQUENCY_BARS_T* mrs_GetFrequencyBars(VOID)
{
#if 0
    return avrs_GetFreqBars();
#else
    return NULL;
#endif
}
