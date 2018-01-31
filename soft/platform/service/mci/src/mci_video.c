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


#if (CSW_EXTENDED_API_AUDIO_VIDEO == 1)


#include "mci.h"
#include "mcip_debug.h"
#include "mps_m.h"
#include "lcdd_m.h"

#include "mcip_media.h"

#include "mcip_callback.h"

// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================



// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

PRIVATE UINT8 *g_file;

// =============================================================================
// g_mciVideoHandle
// -----------------------------------------------------------------------------
/// Handle on an open video object
// =============================================================================
PROTECTED LCDD_FBW_T g_mciVideoFbw;

// =============================================================================
//  FUNCTIONS
// =============================================================================

VOID MCI_VideoPlayerCallback(MPS_HANDLE_T handle, MPS_STATE_T state)
{
    MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoPlayerCallback handle=%#x state=%i", handle, state);

    if (handle == g_mciMediaHandle)
    {
        switch (state)
        {
            case MPS_STATE_PLAY:
                MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MPS_STATE PLAY");
                break;
            case MPS_STATE_EOF:
                MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MPS_STATE EOF");
                // UGLY
                // TODO : FIND a WAY to DO DIFFERENTLY
                vid_send_play_finish_ind(0);
                break;
            case MPS_STATE_CLOSE:
                MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MPS_STATE STOP");
                // UGLY
                // TODO : FIND a WAY to DO DIFFERENTLY
                vid_send_play_finish_ind(0);
                break;
            case MPS_STATE_STOP:
                break;
            case MPS_STATE_PAUSE:
                MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MPS_STATE PAUSE");
                break;
            case MPS_STATE_SEEK:
                MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MPS_STATE SEEK");
                // UGLY
                // TODO : FIND a WAY to DO DIFFERENTLY
                vid_send_seek_done_ind(0);
                break;
            default:
                MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MPS_STATE UNKNOWN");
                break;
        }
    }
}

// =============================================================================
// MCI_VideoOpenFile
// -----------------------------------------------------------------------------
/// Starts playing a file
// =============================================================================
UINT32 MCI_VideoOpenFile (INT32 OutputPath, UINT8 *file)
{
    MPS_ERR_T          err;
    MPS_FILE_INFO_T   *fileInfo;

    MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoOpenFile OutputPath=%i", OutputPath) ;

    g_file       = file;

    if (g_mciMediaHandle == NULL)
    {
        g_mciVideoFbw.roi.x     = 0;
        g_mciVideoFbw.roi.y     = 0;

        g_mciVideoFbw.roi.width  = 128;
        g_mciVideoFbw.roi.height = 112;
        g_mciVideoFbw.fb.width   = 128;
        g_mciVideoFbw.fb.height  = 112;

        g_mciVideoFbw.fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565;

        g_mciConfig.windows          = &g_mciVideoFbw;

        switch (OutputPath)
        {
            case MCI_PATH_NORMAL:
                MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoOpenFile on Receiver");
                g_mciConfig.audioPath = MPS_AUDIO_PATH_LOUD_SPEAKER;
                break;
            case MCI_PATH_HP:
                MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoOpenFile on Headset");
                g_mciConfig.audioPath = MPS_AUDIO_PATH_HEADSET;
                break;
            case MCI_PATH_LSP:
                MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoOpenFile on Louspeaker");
                g_mciConfig.audioPath = MPS_AUDIO_PATH_LOUD_SPEAKER;
                break;
            default:
                // Mode not supported
                MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoOpenFile Error: Unsupported Outputpath");
                return (UINT32) MCI_ERR_INVALID_PARAMETER;
        }
        mps_Open();
        err = mps_OpenFile(&g_mciMediaHandle, file, MCI_VideoPlayerCallback, &g_mciConfig);
        switch(err)
        {
            case MPS_ERR_UNKNOWN_FORMAT:
                // File format not recognized
                MCI_TRACE(MCI_VID_TRC_LVL, 0x1, "MCI_VideoOpenFile Error : wrong file format %s", file) ;
                return (UINT32) MCI_ERR_UNKNOWN_FORMAT;
            case MPS_ERR_FILE_NOT_FOUND:
                // Open file failed
                MCI_TRACE(MCI_VID_TRC_LVL, 0x1, "MCI_VideoOpenFile Error : could not open %s", file) ;
                return (UINT32) MCI_ERR_INVALID_PARAMETER;
            default:
                break;
        }

        fileInfo = mps_GetInfo(g_mciMediaHandle);
        vid_file_info(fileInfo->width, fileInfo->height,
                      fileInfo->nbFrames, (fileInfo->nbFrames << 8) / fileInfo->videoRate);

        vid_send_file_ready_ind(0);

        return err;
    }
    return (UINT32) MCI_ERR_BUSY;
}

// =============================================================================
// MCI_VideoPlay
// -----------------------------------------------------------------------------
/// Starts playing a video
// =============================================================================
UINT32 MCI_VideoPlay (UINT16 startX, UINT16 startY)
{
    MPS_ERR_T          err;

    MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoPlay") ;

    if (g_mciMediaHandle == NULL)
    {
        return (UINT32) MCI_ERR_BUSY;
    }

    g_mciConfig.x = startX;
    g_mciConfig.y = startY;
    err = mps_SetConfig(g_mciMediaHandle, &g_mciConfig);
    err = mps_Play(g_mciMediaHandle, 1);
    switch(err)
    {
        case MPS_ERR_SERVICE_NOT_OPEN:
            MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoPlay Error %d", err) ;
            return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
        case MPS_ERR_INVALID:
            MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoPlay Error %d", err) ;
            return (UINT32) MCI_ERR_BUSY;
        default:
            return (UINT32) MCI_ERR_NO;
    }
    MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoPlay Error: No Open File") ;
    return (UINT32) MCI_ERR_BUSY;
}
// =============================================================================
// MCI_VideoPause
// -----------------------------------------------------------------------------
/// Pause a video object
// =============================================================================
UINT32 MCI_VideoPause(VOID)
{
    MPS_ERR_T err;

    MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoPause") ;

    if (g_mciMediaHandle != NULL)
    {
        err = mps_Pause(g_mciMediaHandle);
        if (err == MPS_ERR_NO)
        {
            return (UINT32) MCI_ERR_NO;
        }
        MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoPause Error %d", err) ;
        return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
    }
    MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoPause Error: No Open File") ;
    return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
}

// =============================================================================
// MCI_VideoSeek
// -----------------------------------------------------------------------------
// Seek a video to the time playtime
// =============================================================================
UINT32 MCI_VideoSeek (long long playtime, INT32 time_mode, UINT16 startX, UINT16 startY)
{
    MPS_ERR_T        err;
    UINT32           seektime = 0;

    MCI_TRACE(TSTDOUT, 0x0, "MCI_VideoSeek mode %i time %i", time_mode, playtime) ;
    if (g_mciMediaHandle != NULL)
    {
        if (time_mode == 1)
        {
            // relative seek time
            err       = mps_Tell(g_mciMediaHandle, &seektime);
            seektime += playtime;
        }
        else
        {
            // absolute seek time
            seektime  = playtime;
        }

        g_mciConfig.x = startX;
        g_mciConfig.y = startY;

        err = mps_SetConfig(g_mciMediaHandle, &g_mciConfig);
        if (err != MPS_ERR_NO)
        {
            MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoSeek Error %d", err) ;
            return (UINT32) MCI_ERR_INVALID_PARAMETER;
        }
        err = mps_Seek(g_mciMediaHandle, seektime);
        if (err == MPS_ERR_NO)
        {
            return (UINT32) MCI_ERR_NO;
        }
        MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoSeek Error %d", err) ;
        return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
    }

    MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoSeek Error: No Open File") ;
    return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
}

// =============================================================================
// MCI_VideoStop
// -----------------------------------------------------------------------------
// stop a video
// =============================================================================
UINT32 MCI_VideoStop(VOID)
{
    MPS_ERR_T err;

    MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoStop") ;

    if (g_mciMediaHandle != NULL)
    {
        err = mps_Stop(g_mciMediaHandle);
        if (err == MPS_ERR_NO)
        {
            return (UINT32) MCI_ERR_NO;
        }
        MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoStop Error %d", err) ;
        return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
    }
    MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoStop Error: No Open File") ;
    return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
}

// =============================================================================
// MCI_VideoSet
// -----------------------------------------------------------------------------
// Sets the video parameters
// ZoomWidth and ZoomHeight are currently not used
// =============================================================================
UINT32 MCI_VideoSet (UINT16 ZoomWidth, UINT16 ZoomHeight, UINT16 startX, UINT16 startY)
{
    MPS_ERR_T        err;

    MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoSet %d, %d, %d, %d", ZoomWidth, ZoomHeight, startX, startY);

    g_mciConfig.x = startX;
    g_mciConfig.y = startY;

    if (g_mciMediaHandle != NULL)
    {
        err = mps_SetConfig(g_mciMediaHandle, &g_mciConfig);
        if (err == MPS_ERR_NO)
        {

            return (UINT32) MCI_ERR_NO;
        }
        MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoSet Error %d", err) ;
        return (UINT32) MCI_ERR_INVALID_PARAMETER;
    }
    return (UINT32) MCI_ERR_NO;
}

// =============================================================================
// MCI_VideoGetPlayTime
// -----------------------------------------------------------------------------
// Get current play time
// =============================================================================
VOID  MCI_VideoGetInfo(UINT16 *image_width, UINT16 *image_height, UINT32 *total_time)
{
    MPS_FILE_INFO_T   *fileInfo;

    fileInfo = mps_GetInfo(g_mciMediaHandle);

    if(fileInfo)
    {
        *image_width  = fileInfo->width;
        *image_height = fileInfo->height;
        *total_time   = (fileInfo->nbFrames << 8) / fileInfo->videoRate;
    }
    else
    {
        *image_width  = 0;
        *image_height = 0;
        *total_time   = 0;
    }
}

// =============================================================================
// MCI_VideoGetPlayTime
// -----------------------------------------------------------------------------
// Get current play time
// =============================================================================
UINT32 MCI_VideoGetPlayTime(VOID)
{
    MPS_ERR_T        err;
    UINT32           position = 0;

    MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoGetPlayTime") ;

    if (g_mciMediaHandle != NULL)
    {
        err = mps_Tell(g_mciMediaHandle, &position);
        return position;
    }
    MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoGetPlayTime Error: No Open File") ;
    return 0;
}

// =============================================================================
// MCI_VideoClose
// -----------------------------------------------------------------------------
// Close Video
// =============================================================================
UINT32 MCI_VideoClose(VOID)
{
    MPS_ERR_T err;

    MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoClose");

    if (g_mciMediaHandle != NULL)
    {
        err = mps_CloseFile(g_mciMediaHandle);
        g_mciMediaHandle = 0;
        if (err == MPS_ERR_NO)
        {
            // Reset the audio handle
            return (UINT32) MCI_ERR_NO;
        }
        MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoClose Error %d", err) ;
        return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
    }
    MCI_TRACE(MCI_VID_TRC_LVL, 0x0, "MCI_VideoClose Error: No Open File") ;
    return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
}
#endif // CSW_EXTENDED_API_AUDIO_VIDEO




