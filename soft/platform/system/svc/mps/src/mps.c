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

#include "avps_m.h"

#include "sxr_ops.h"
#include "sxs_io.h"

#include "mps_m.h"
#include "mpsp.h"

#include "mpsp_debug.h"

#include "aud_m.h"
#include "lcdd_m.h"

#include "string.h"

#include "hal_sys.h"
#include "hal_host.h"

#include "mpsp_demux.h"
#include "mpsp_demux_adts.h"
#include "mpsp_demux_amr.h"
#include "mpsp_demux_pcm8kmono.h"
#include "mpsp_demux_riff.h"
#include "mpsp_demux_iso.h"


// =============================================================================
// MACROS
// =============================================================================

#define MPS_HEADER_ID 0x9357EE53

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// MPS_CMD_T
// -----------------------------------------------------------------------------
/// List of action on mps task
// =============================================================================
typedef enum
{
    // Play stream if it's possible
    MPS_CMD_PLAY,
    // Free all ressources and stop stream
    MPS_CMD_CLOSE,
    // Pause the stream
    MPS_CMD_PAUSE,
    // stop the stream
    MPS_CMD_STOP,
    // Jump in new position in file
    MPS_CMD_SEEK,
    // Lauch pause play for audio path switch
    MPS_CMD_RESET
} MPS_CMD_T;

// =============================================================================
// MPS_BODY_MSG_T
// -----------------------------------------------------------------------------
/// Internal message structure
// =============================================================================
typedef struct
{
    MPS_CMD_T       cmd;
    MPS_HANDLE_T    handle;
    UINT32          time;
} MPS_BODY_MSG_T ;


typedef union
{
    MPS_BODY_MSG_T  mps;
    AVPS_BODY_MSG_T avps;
} MsgBody_t;

#include "itf_msg.h"


// =============================================================================
// PRIVATE VARIABLES
// =============================================================================

PRIVATE sxr_TaskDesc_t               g_mpsTask;
PRIVATE sxr_TaskDesc_t               g_mpsAudioTask;
PRIVATE UINT8                        g_mpsIdTask = 0;
PRIVATE UINT8                        g_mpsIdAudioTask = 0;
PRIVATE UINT8                        g_mpsMbx;
PRIVATE UINT8                        g_mpsAudioMbx;

PRIVATE VOLATILE MPS_HANDLE_T        g_mpsCurrentHandle = 0;

/// List of demux module
PRIVATE MPS_DEMUX_T*                 g_mpsDemuxList[] =
{
    &g_adtsDemux,
    &g_riffDemux,
    &g_amrDemux,
    &g_isoDemux,
    0
};

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PROTECTED UINT8*           g_mpsBufferReadPos   = 0;
PROTECTED UINT8*           g_mpsBufferWritePos  = 0;

// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

PRIVATE VOID mps_AudioTask(VOID* param)
{
    Msg_t            msg;
    UINT32           size;
    UINT32           j;

    while(1)
    {
        if(sxr_Wait((UINT32*)&msg, g_mpsAudioMbx))
        {
            continue;
        }
        MPS_TRACE("mps_AudioTask: new message");

        // Fetch new audio data
        g_mpsBufferReadPos = (UINT8*)msg.B.avps.bufferPos;
        if(g_mpsBufferWritePos == 0)
        {
            g_mpsBufferWritePos  = g_mpsCurrentHandle->audioBuffer;
        }
        MPS_TRACE("mps_Task: Read pos %#x Write pos %#x",
                  g_mpsBufferReadPos, g_mpsBufferWritePos);

        MPS_ASSERT(g_mpsBufferReadPos == 0 ||
                   ((UINT32)g_mpsBufferReadPos >= (UINT32)g_mpsCurrentHandle->audioBuffer &&
                    (UINT32)g_mpsBufferReadPos < (UINT32)g_mpsCurrentHandle->audioBuffer + g_mpsCurrentHandle->audioLength),
                   "Read pointer out of range");
        size = 0;
        if((UINT32)g_mpsBufferReadPos <= (UINT32)g_mpsBufferWritePos)
        {
            size += g_mpsCurrentHandle->demux->getAudioData(g_mpsCurrentHandle, g_mpsBufferWritePos,
                    g_mpsCurrentHandle->audioLength +
                    (UINT32)g_mpsCurrentHandle->audioBuffer -
                    (UINT32)g_mpsBufferWritePos);
            g_mpsBufferWritePos           += size;

            // Wrap ??
            if((UINT32)g_mpsBufferWritePos ==
                    (UINT32)g_mpsCurrentHandle->audioBuffer + g_mpsCurrentHandle->audioLength)
            {
                g_mpsBufferWritePos = g_mpsCurrentHandle->audioBuffer;
            }
        }

        if(size !=  g_mpsCurrentHandle->audioLength &&
                (UINT32)g_mpsBufferReadPos > (UINT32)g_mpsBufferWritePos)
        {
            j = g_mpsCurrentHandle->demux->getAudioData(g_mpsCurrentHandle, g_mpsBufferWritePos,
                    (UINT32)g_mpsBufferReadPos - (UINT32)g_mpsBufferWritePos);
            g_mpsBufferWritePos           += j;
            size                          += j;
        }
        if((UINT32)g_mpsBufferWritePos ==
                (UINT32)g_mpsCurrentHandle->audioBuffer + g_mpsCurrentHandle->audioLength)
        {
            g_mpsBufferWritePos = g_mpsCurrentHandle->audioBuffer;
        }

        if(size == 0)
        {
            MPS_TRACE("mps_Task: No more data");
            if((UINT32)g_mpsBufferReadPos == (UINT32)g_mpsBufferWritePos)
            {
                MPS_TRACE("mps_Task: End of file");
                g_mpsCurrentHandle->bufferContext.audioRemainingSize      = 0;
                g_mpsCurrentHandle->bufferContext.bufferAudioReadPosition = g_mpsCurrentHandle->audioBuffer;

                if(g_mpsCurrentHandle->nbPlayed <= 1)
                {
                    if(g_mpsCurrentHandle->callback)
                    {
                        g_mpsCurrentHandle->callback((MPS_HANDLE_T)g_mpsCurrentHandle,
                                                     MPS_STATE_EOF);
                    }
                    g_mpsCurrentHandle = 0;
                }
                else
                {
                    g_mpsCurrentHandle->nbPlayed--;
                    msg.H.Id                 = MPS_HEADER_ID;
                    msg.B.mps.cmd            = MPS_CMD_PLAY;
                    msg.B.mps.handle         = g_mpsCurrentHandle;
                    g_mpsCurrentHandle       = 0;
                    sxr_Send(&msg, g_mpsMbx, SXR_SEND_EVT);
                }
            }
            else
            {
                MPS_TRACE("mps_Task: AudioAddedData %i", size);
                avps_AudioAddedData(size);
            }
        }
        else
        {
            MPS_TRACE("mps_Task: AudioAddedData %i", size);
            avps_AudioAddedData(size);
        }
    }
}

PRIVATE VOID mps_Task(VOID* param)
{
    Msg_t            msg;
    AVPS_CFG_T       avpsCfg;
    UINT32           size;

    MPS_PROFILE_FUNCTION_ENTER(mps_task);

    param = param;

    while(1)
    {
        if(sxr_Wait((UINT32*)&msg, g_mpsMbx))
        {
            continue;
        }

        MPS_TRACE("mps_Task: new message");
        // Internal message
        if(msg.H.Id == MPS_HEADER_ID)
        {
            MPS_TRACE("mps_Task: internal msg cmd=%i handle=%#x",
                      msg.B.mps.cmd, msg.B.mps.handle);

            MPS_ASSERT(msg.B.mps.handle, "mps_Task: msg handle == NULL");
            switch(msg.B.mps.cmd)
            {
                case MPS_CMD_PLAY:
                    MPS_TRACE("mps_Task: play cmd");

                    if(g_mpsCurrentHandle == 0)
                    {
                        g_mpsBufferReadPos   = msg.B.mps.handle->audioBuffer;
                        g_mpsBufferWritePos  = msg.B.mps.handle->audioBuffer;

                        MPS_TRACE("mps_Task: Read pos %#x Write pos %#x",
                                  g_mpsBufferReadPos, g_mpsBufferWritePos);

                        avpsCfg.equalizer      = msg.B.mps.handle->config.equalizerMode;
                        avpsCfg.spkLevel       = msg.B.mps.handle->config.volume;
                        avpsCfg.videoRotation  = msg.B.mps.handle->config.videoRotation;

                        if(msg.B.mps.handle->demux->play(msg.B.mps.handle, &avpsCfg) == TRUE)
                        {
                            g_mpsCurrentHandle = msg.B.mps.handle;

                            if(msg.B.mps.handle->callback)
                            {
                                msg.B.mps.handle->callback(msg.B.mps.handle,
                                                           MPS_STATE_PLAY);
                            }
                        }
                    }
                    break;
                case MPS_CMD_SEEK:
                    MPS_TRACE("mps_Task: seek cmd time=%i", msg.B.mps.time);

                    if(g_mpsCurrentHandle == msg.B.mps.handle)
                    {
                        avps_Stop(0);

                        msg.B.mps.handle->demux->seek(msg.B.mps.handle, msg.B.mps.time);
                        avpsCfg.equalizer      = msg.B.mps.handle->config.equalizerMode;
                        avpsCfg.spkLevel       = msg.B.mps.handle->config.volume;
                        avpsCfg.videoRotation  = msg.B.mps.handle->config.videoRotation;
                        msg.B.mps.handle->demux->play(msg.B.mps.handle, &avpsCfg);
                    }
                    else
                    {
                        msg.B.mps.handle->demux->seek(msg.B.mps.handle, msg.B.mps.time);
                    }
                    if(msg.B.mps.handle->callback)
                    {
                        msg.B.mps.handle->callback(msg.B.mps.handle, MPS_STATE_SEEK);
                    }
                    break;
                case MPS_CMD_RESET:
                    MPS_TRACE("mps_Task: reset cmd");
                    if(g_mpsCurrentHandle == msg.B.mps.handle)
                    {
                        avps_Stop(&g_mpsCurrentHandle->bufferContext);
                        g_mpsCurrentHandle->demux->pause(g_mpsCurrentHandle);
                        avpsCfg.equalizer      = msg.B.mps.handle->config.equalizerMode;
                        avpsCfg.spkLevel       = msg.B.mps.handle->config.volume;
                        avpsCfg.videoRotation  = msg.B.mps.handle->config.videoRotation;
                        g_mpsCurrentHandle->demux->play(g_mpsCurrentHandle, &avpsCfg);
                    }

                    break;
                case MPS_CMD_CLOSE:
                    MPS_TRACE("mps_Task: stop cmd");

                    msg.B.mps.handle->demux->close(msg.B.mps.handle);
                    if(g_mpsCurrentHandle == msg.B.mps.handle)
                    {
                        g_mpsCurrentHandle        = 0;
                    }
                    mps_MediumClose(&msg.B.mps.handle->mediumAudio);
                    mps_MediumClose(&msg.B.mps.handle->mediumVideo);

                    if(msg.B.mps.handle->callback)
                    {
                        msg.B.mps.handle->callback(msg.B.mps.handle,
                                                   MPS_STATE_CLOSE);
                    }

                    sxr_Free(msg.B.mps.handle);
                    break;
                case MPS_CMD_STOP:
                    MPS_TRACE("mps_Task: stop cmd");

                    if(g_mpsCurrentHandle == msg.B.mps.handle)
                    {
                        avps_Stop(&g_mpsCurrentHandle->bufferContext);
                        g_mpsCurrentHandle->demux->pause(g_mpsCurrentHandle);
                        g_mpsCurrentHandle->demux->seek(g_mpsCurrentHandle, 0);
                        g_mpsCurrentHandle = 0;
                    }
                    if(msg.B.mps.handle->callback)
                    {
                        msg.B.mps.handle->callback(msg.B.mps.handle,
                                                   MPS_STATE_STOP);
                    }
                    break;
                case MPS_CMD_PAUSE:
                    MPS_TRACE("mps_Task: pause cmd");

                    if(g_mpsCurrentHandle == msg.B.mps.handle)
                    {
                        MPS_ASSERT(g_mpsCurrentHandle->demux->pause, "mps_Task: pause not implemented");

                        avps_Stop(&g_mpsCurrentHandle->bufferContext);

                        g_mpsCurrentHandle->demux->pause(g_mpsCurrentHandle);

                        if(msg.B.mps.handle->callback)
                        {
                            msg.B.mps.handle->callback(msg.B.mps.handle,
                                                       MPS_STATE_PAUSE);
                        }
                        g_mpsCurrentHandle        = 0;
                    }

                    break;
            }
        }

        if(g_mpsCurrentHandle == 0)
        {
            continue;
        }

        switch(msg.H.Id)
        {
            case AVPS_HEADER_ID_AUDIO:
                sxr_Send(&msg, g_mpsAudioMbx, SXR_SEND_EVT);
                break;

            case AVPS_HEADER_ID_VIDEO:
                // Fetch new video data
                // AVPS will switch along the two halves of the video
                // buffer to use it as a ping-pong buffer. Frame will
                // be alternatively fetched in an half, then the other,
                // at the will of the underlying AVPS.
                MPS_ASSERT(g_mpsCurrentHandle->demux->getVideoData, "mps: getVideoData not implemented in demux");
                size = g_mpsCurrentHandle->demux->getVideoData(g_mpsCurrentHandle,
                        (UINT8*)msg.B.avps.bufferPos,
                        g_mpsCurrentHandle->videoLength/2);
                if(size != 0)
                {
                    avps_VideoAddedData(size);
                }
                break;

            case AVPS_HEADER_ID_VIDEO_DISPLAY:
                // The pointer embodied in this message is of a frame buffer
                // window to display.
                MPS_PROFILE_FUNCTION_ENTER(mps_blit);
                if(g_mpsCurrentHandle->config.blitCallback)
                {
                    g_mpsCurrentHandle->config.blitCallback((LCDD_FBW_T*)msg.B.avps.bufferPos,
                                                            g_mpsCurrentHandle->config.x,
                                                            g_mpsCurrentHandle->config.y);
                }
                else
                {
                    lcdd_Blit16((LCDD_FBW_T*)msg.B.avps.bufferPos,
                                g_mpsCurrentHandle->config.x, g_mpsCurrentHandle->config.y);
                }
                MPS_PROFILE_FUNCTION_EXIT(mps_blit);
                break;

            case AVPS_HEADER_ID_VIDEO_SKIP:
                MPS_ASSERT(g_mpsCurrentHandle->demux->skipVideo, "mps: skipVideo not implemented in demux");
                break;
        }
    }

    MPS_PROFILE_FUNCTION_EXIT(mps_task);

}

// =============================================================================
// FUNCTIONS
// =============================================================================

PUBLIC VOID mps_Open(VOID)
{
    MPS_TRACE("mps_Open");

    if(g_mpsIdTask == 0)
    {
        MPS_TRACE("mps_Open: Create task");
        g_mpsCurrentHandle       = 0;
        g_mpsTask.TaskBody       = mps_Task;
        g_mpsTask.TaskExit       = 0;
        g_mpsTask.Name           = "MPS";
        g_mpsTask.StackSize      = 768;
        g_mpsTask.Priority       = 50;
        g_mpsMbx                 = sxr_NewMailBox();
        g_mpsIdTask              = sxr_NewTask(&g_mpsTask);

        g_mpsAudioTask.TaskBody  = mps_AudioTask;
        g_mpsAudioTask.TaskExit  = 0;
        g_mpsAudioTask.Name      = "MPS2";
        g_mpsAudioTask.StackSize = 512;
        g_mpsAudioTask.Priority  = 51;
        g_mpsAudioMbx            = sxr_NewMailBox();
        g_mpsIdAudioTask         = sxr_NewTask(&g_mpsAudioTask);

        sxr_StartTask(g_mpsIdTask, 0);
        sxr_StartTask(g_mpsIdAudioTask, 0);
    }
}

PUBLIC MPS_ERR_T mps_Close(VOID)
{
    MPS_TRACE("mps_Close");

    if(g_mpsIdTask == 0)
    {
        MPS_TRACE("mps_Close: Err=Not open");
        return MPS_ERR_SERVICE_NOT_OPEN;
    }

    if(g_mpsCurrentHandle)
    {
        MPS_TRACE("mps_Close: Err=Busy");
        return MPS_ERR_INVALID;
    }

    sxr_StopTask(g_mpsIdTask);
    sxr_FreeTask(g_mpsIdTask);
    sxr_FreeMailBox(g_mpsMbx);
    g_mpsIdTask = 0;
    g_mpsMbx    = 0;

    sxr_StopTask(g_mpsIdAudioTask);
    sxr_FreeTask(g_mpsIdAudioTask);
    sxr_FreeMailBox(g_mpsAudioMbx);
    g_mpsIdAudioTask = 0;
    g_mpsAudioMbx    = 0;

    return MPS_ERR_NO;
}

PUBLIC MPS_FILE_INFO_T* mps_GetInfo(MPS_HANDLE_T handle)
{
    MPS_ASSERT(handle, "mps_GetInfo: handle == NULL");

    if(g_mpsIdTask == 0)
    {
        return 0;
    }

    return &handle->info;
}

PRIVATE MPS_ERR_T mps_OpenHandle(MPS_HANDLE_T* phandle,  MPS_MEDIUM_TYPE_T mediumType,
                                 UINT8* buffer, UINT32 size, MPS_CALLBACK_T callback,
                                 MPS_CONFIG_T* config)
{
    MPS_HANDLE_T               handle;
    int                        i;
    VOID*                      demuxData = 0;

    MPS_ASSERT(phandle, "mps_OpenHandle: phandler == NULL");
    MPS_ASSERT(buffer,  "mps_OpenHandle: buffer == NULL");
    MPS_ASSERT(config,  "mps_OpenHandle: config == NULL");

    MPS_TRACE("mps_OpenHandle");

    if(g_mpsIdTask == 0)
    {
        MPS_TRACE("mps_OpenHandle: Err=Not open");
        *phandle = 0;
        return MPS_ERR_SERVICE_NOT_OPEN;
    }

    *phandle = (MPS_HANDLE_T)sxr_Malloc(sizeof(struct MPS_HANDLE_STRUCT_T));
    handle   = *phandle;
    memset(&handle->info, 0, sizeof(handle->info));

    if(mps_MediumOpen(&handle->mediumAudio, mediumType, buffer, size) < 0)
    {
        MPS_TRACE("mps_Openhandle: Err=Open file failed FS_Err=%i",
                  handle->mediumAudio.data.fs.fd);
        sxr_Free(handle);
        *phandle = 0;
        return MPS_ERR_FILE_NOT_FOUND;
    }

    if(mps_MediumOpen(&handle->mediumVideo, MPS_MEDIUM_TYPE_FS, buffer, size) < 0)
    {
        MPS_TRACE("mps_OpenHandle: Err=Open file failed FS_Err=%i",
                  handle->mediumVideo.data.fs.fd);
        sxr_Free(handle);
        *phandle = 0;
        return MPS_ERR_FILE_NOT_FOUND;
    }

    memcpy(&handle->config, config, sizeof(MPS_CONFIG_T));

    handle->stream.mbx       = g_mpsMbx;
    handle->callback         = callback;
    handle->stream.audioMode = AVPS_PLAY_AUDIO_MODE_NO;
    handle->stream.videoMode = AVPS_PLAY_VIDEO_MODE_NO;

    for(i = 0; g_mpsDemuxList[i]; ++i)
    {
        mps_MediumSeek(&handle->mediumAudio, 0, FS_SEEK_SET);
        if((demuxData = g_mpsDemuxList[i]->open(handle)) != NULL)
        {
            break;
        }
    }

    handle->demux               = g_mpsDemuxList[i];
    handle->demuxData           = demuxData;
    handle->stream.audioAddress = (UINT32)handle->audioBuffer;
    handle->stream.videoAddress = (UINT32)handle->videoBuffer;
    handle->stream.audioLength  = handle->audioLength;
    handle->stream.videoLength  = handle->videoLength;

    handle->bufferContext.bufferAudioReadPosition = handle->audioBuffer;
    handle->bufferContext.audioRemainingSize      = 0;


    if(handle->demux == NULL)
    {
        MPS_TRACE("mps_OpenHandle: Err=Format not supported");
        mps_MediumClose(&handle->mediumVideo);
        mps_MediumClose(&handle->mediumAudio);
        sxr_Free(handle);
        *phandle = 0;
        return MPS_ERR_UNKNOWN_FORMAT;
    }

    return MPS_ERR_NO;
}

PRIVATE MPS_ERR_T mps_CloseHandle(MPS_HANDLE_T handle)
{
    Msg_t  msg;

    MPS_TRACE("mps_CloseHandle");

    if(handle == NULL)
    {
        MPS_TRACE("mps_CloseHandle: Err=handle null");
        return MPS_ERR_INVALID;
    }

    if(g_mpsIdTask == 0)
    {
        MPS_TRACE("mps_CloseHandle: Err=Not open");
        return MPS_ERR_SERVICE_NOT_OPEN;
    }

    // Only close an existing handle.
    msg.H.Id          = MPS_HEADER_ID;
    msg.B.mps.cmd     = MPS_CMD_CLOSE;
    msg.B.mps.handle  = handle;
    sxr_Send(&msg, g_mpsMbx, SXR_SEND_EVT);

    return MPS_ERR_NO;
}

PUBLIC MPS_ERR_T mps_OpenBuffer(MPS_HANDLE_T*  phandle,
                                UINT8*         buffer,
                                UINT32         size,
                                MPS_CALLBACK_T callback,
                                MPS_CONFIG_T*  config)
{
    return mps_OpenHandle(phandle, MPS_MEDIUM_TYPE_MEM, buffer, size, callback, config);
}

DEPRECATED PUBLIC MPS_ERR_T mps_OpenBufferPcm8kmono(MPS_HANDLE_T*  phandle,
        UINT8*         buffer,
        UINT32         size,
        MPS_CALLBACK_T callback,
        MPS_CONFIG_T*  config)
{
    MPS_HANDLE_T               handle;
    VOID*                      demuxData = 0;

    MPS_ASSERT(phandle, "mps_OpenBufferPcm8kmono: phandler == NULL");
    MPS_ASSERT(buffer,  "mps_OpenBufferPcm8kmono: buffer == NULL");
    MPS_ASSERT(config,  "mps_OpenBufferPcm8kmono: config == NULL");

    MPS_TRACE("mps_OpenBufferPcm8kmono buffer=%#x", buffer);

    if(g_mpsIdTask == 0)
    {
        MPS_TRACE("mps_OpenBufferPcm8kmono: Err=Not open");
        *phandle = 0;
        return MPS_ERR_SERVICE_NOT_OPEN;
    }

    *phandle = (MPS_HANDLE_T)sxr_Malloc(sizeof(struct MPS_HANDLE_STRUCT_T));
    handle   = *phandle;
    memset(&handle->info, 0, sizeof(handle->info));

    mps_MediumOpen(&handle->mediumAudio, MPS_MEDIUM_TYPE_MEM, buffer, size);
    mps_MediumOpen(&handle->mediumVideo, MPS_MEDIUM_TYPE_MEM, buffer, size);

    memcpy(&handle->config, config, sizeof(MPS_CONFIG_T));

    handle->stream.mbx       = g_mpsMbx;
    handle->callback         = callback;
    handle->stream.audioMode = AVPS_PLAY_AUDIO_MODE_NO;
    handle->stream.videoMode = AVPS_PLAY_VIDEO_MODE_NO;

    demuxData                   = g_pcm8kmonoDemux.open(handle);

    handle->demux               = &g_pcm8kmonoDemux;
    handle->demuxData           = demuxData;
    handle->stream.audioAddress = (UINT32)handle->audioBuffer;
    handle->stream.videoAddress = (UINT32)handle->videoBuffer;
    handle->stream.audioLength  = handle->audioLength;
    handle->stream.videoLength  = handle->videoLength;

    handle->bufferContext.bufferAudioReadPosition = handle->audioBuffer;
    handle->bufferContext.audioRemainingSize      = 0;

    return MPS_ERR_NO;
}

PUBLIC MPS_ERR_T mps_CloseBuffer(MPS_HANDLE_T handle)
{
    return mps_CloseHandle(handle);
}


#ifndef WITHOUT_FS
PUBLIC MPS_ERR_T mps_CloseFile(MPS_HANDLE_T handle)
{
    return mps_CloseHandle(handle);
}

PUBLIC MPS_ERR_T mps_OpenFile(MPS_HANDLE_T* phandle,  UINT8* file, MPS_CALLBACK_T callback,
                              MPS_CONFIG_T* config)
{
    return mps_OpenHandle(phandle, MPS_MEDIUM_TYPE_FS, file, 0, callback, config);
}
#endif /* WITHOUT_FS */

PUBLIC MPS_ERR_T mps_Play(MPS_HANDLE_T handle, UINT32 nbPlayed)
{
    Msg_t           msg;

    MPS_ASSERT(handle, "mps_Play: handle == NULL");

    MPS_TRACE("mps_Play");

    if(g_mpsIdTask == 0)
    {
        MPS_TRACE("mps_Play: mps not open");
        return MPS_ERR_SERVICE_NOT_OPEN;
    }

    if(g_mpsCurrentHandle != 0)
    {
        MPS_TRACE("mps_Play: Busy");
        return MPS_ERR_INVALID;
    }

    handle->nbPlayed         = nbPlayed;

    msg.H.Id                 = MPS_HEADER_ID;
    msg.B.mps.cmd            = MPS_CMD_PLAY;
    msg.B.mps.handle         = handle;
    sxr_Send(&msg, g_mpsMbx, SXR_SEND_EVT);

    return MPS_ERR_NO;
}


PUBLIC MPS_ERR_T mps_Pause(MPS_HANDLE_T handle)
{
    Msg_t           msg;

    MPS_ASSERT(handle, "mps_Pause: handle == NULL");

    MPS_TRACE(__func__);

    if(g_mpsIdTask == 0)
    {
        return MPS_ERR_SERVICE_NOT_OPEN;
    }

    if(g_mpsCurrentHandle != handle)
    {
        return MPS_ERR_INVALID;
    }

    msg.H.Id                 = MPS_HEADER_ID;
    msg.B.mps.cmd            = MPS_CMD_PAUSE;
    msg.B.mps.handle         = handle;
    sxr_Send(&msg, g_mpsMbx, SXR_SEND_EVT);

    return MPS_ERR_NO;
}

PUBLIC MPS_ERR_T mps_Stop(MPS_HANDLE_T handle)
{
    Msg_t           msg;

    MPS_ASSERT(handle, "mps_Stop: handle == NULL");

    MPS_TRACE(__func__);

    if(g_mpsIdTask == 0)
    {
        return MPS_ERR_SERVICE_NOT_OPEN;
    }

    msg.H.Id                 = MPS_HEADER_ID;
    msg.B.mps.cmd            = MPS_CMD_STOP;
    msg.B.mps.handle         = handle;
    sxr_Send(&msg, g_mpsMbx, SXR_SEND_EVT);

    return MPS_ERR_NO;
}

PUBLIC MPS_ERR_T mps_Seek(MPS_HANDLE_T handle, UINT32 time)
{
    Msg_t           msg;

    MPS_ASSERT(handle, "mps_Seek: handle == NULL");

    MPS_TRACE(__func__);

    if(g_mpsIdTask == 0)
    {
        return MPS_ERR_SERVICE_NOT_OPEN;
    }

    if(handle->demux->seek)
    {
        msg.H.Id                 = MPS_HEADER_ID;
        msg.B.mps.cmd            = MPS_CMD_SEEK;
        msg.B.mps.handle         = handle;
        msg.B.mps.time           = time;
        sxr_Send(&msg, g_mpsMbx, SXR_SEND_EVT);
    }

    return MPS_ERR_NO;
}

PUBLIC MPS_ERR_T mps_Tell(MPS_HANDLE_T handle, UINT32* pos)
{
    MPS_ASSERT(handle, "mps_SetConfig: handle == NULL");
    MPS_ASSERT(pos, "mps_Tell: pos == NULL");

    MPS_TRACE(__func__);

    if(g_mpsIdTask == 0)
    {
        return MPS_ERR_SERVICE_NOT_OPEN;
    }

    *pos = 0;
    if(handle->demux->tell)
    {
        *pos = handle->demux->tell(handle);
    }
    return MPS_ERR_NO;
}

PUBLIC MPS_ERR_T mps_SetConfig(MPS_HANDLE_T handle, MPS_CONFIG_T* config)
{
    AVPS_CFG_T avpsCfg;
    Msg_t      msg;

    MPS_ASSERT(handle, "mps_SetConfig: handle == NULL");
    MPS_ASSERT(config, "mps_SetConfig: config == NULL");
    MPS_ASSERT(config->volume < AUD_SPK_VOL_QTY, "mps_SetConfig: config.volume too high");

    MPS_TRACE(__func__);

    if(g_mpsIdTask == 0)
    {
        return MPS_ERR_SERVICE_NOT_OPEN;
    }

    if(g_mpsCurrentHandle == handle)
    {
        if(handle->config.volume        != config->volume        ||
                handle->config.equalizerMode != config->equalizerMode ||
                handle->config.videoRotation != config->videoRotation )
        {
            avpsCfg.equalizer      = config->equalizerMode;
            avpsCfg.spkLevel       = config->volume;
            avpsCfg.videoRotation  = config->videoRotation;

            MPS_TRACE("mps_SetConfig: Set new config");

            avps_Setup(&avpsCfg);
        }
        MPS_TRACE("mps_SetConfig: path %i %i", handle->config.audioPath, config->audioPath);
        if(handle->config.audioPath != config->audioPath)
        {
            handle->config.audioPath = config->audioPath;
            msg.H.Id                 = MPS_HEADER_ID;
            msg.B.mps.cmd            = MPS_CMD_RESET;
            msg.B.mps.handle         = handle;
            sxr_Send(&msg, g_mpsMbx, SXR_SEND_EVT);
        }
    }

    memcpy(&handle->config, config, sizeof(MPS_CONFIG_T));

    return MPS_ERR_NO;
}

PUBLIC MPS_ERR_T mps_GetConfig(MPS_HANDLE_T handle, MPS_CONFIG_T* config)
{
    MPS_ASSERT(handle, "mps_SetConfig: handle == NULL");
    MPS_ASSERT(config, "mps_SetConfig: config == NULL");

    MPS_TRACE(__func__);

    if(g_mpsIdTask == 0)
    {
        return MPS_ERR_SERVICE_NOT_OPEN;
    }

    memcpy(config, &handle->config, sizeof(MPS_CONFIG_T));

    return MPS_ERR_NO;
}

PUBLIC MPS_FREQUENCY_BARS_T* mps_GetFrequencyBars(VOID)
{
    return avps_GetFreqBars();
}
