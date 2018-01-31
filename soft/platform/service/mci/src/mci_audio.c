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




#include "mci.h"
#include "mcip_debug.h"
#include "mps_m.h"
#include "mcip_callback.h"

#if (CSW_EXTENDED_API_AUDIO_VIDEO == 1)
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
// =============================================================================
// g_mciAudioHandle
// -----------------------------------------------------------------------------
/// Handle on an open audio object
// =============================================================================
PROTECTED MPS_HANDLE_T g_mciMediaHandle = NULL;

// =============================================================================
// g_mciAudioPlayCallback
// -----------------------------------------------------------------------------
/// Callback to call in case of state change
// =============================================================================
PROTECTED MCI_AUDIO_PLAY_CALLBACK_T g_mciAudioPlayCallback = NULL;


// =============================================================================
// g_mciAudioConfig
// -----------------------------------------------------------------------------
/// Audio configuration
// =============================================================================
PROTECTED MPS_CONFIG_T g_mciConfig =
{
    .audioPath     = MPS_AUDIO_PATH_HEADSET,
    .windows       = NULL,
    .x             = 0,
    .y             = 0,
    .volume        = 4,
    .equalizerMode = MPS_EQUALIZER_MODE_NORMAL
};

// =============================================================================
// g_mciAudioSavedVolume
// -----------------------------------------------------------------------------
/// To save the volume when the audio is muted
// =============================================================================
PROTECTED UINT8 g_mciAudioSavedVolume = 0;



// =============================================================================
//  FUNCTIONS
// =============================================================================

VOID MCI_AudioPlayerCallback(MPS_HANDLE_T handle, MPS_STATE_T state)
{
    MCI_AUDIO_PLAY_CALLBACK_T tempCallback = g_mciAudioPlayCallback;

    MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioPlayerCallback %d", state);
    if (handle == g_mciMediaHandle)
    {
        if ( state == MPS_STATE_EOF )
        {
            // End of file reached, close the Audio
            MCI_AudioStop();
        }
        // Call the callback function if it has been defined
        if (tempCallback)
        {
            tempCallback(state);
        }
        else
        {
            MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioPlayerCallback : No handler defined");
        }
    }
}
// =============================================================================
// MCI_AudioPlay
// -----------------------------------------------------------------------------
/// Starts playing a file
// =============================================================================
UINT32 MCI_AudioPlay (INT32 OutputPath, UINT8 *file, MCI_AUDIO_PLAY_CALLBACK_T callback)
{
    MPS_ERR_T err;


    if (g_mciMediaHandle == NULL)
    {
        // Save callback function
        g_mciAudioPlayCallback = callback;

        switch (OutputPath)
        {
            case MCI_PATH_NORMAL:
                MCI_TRACE(MCI_AUD_TRC_LVL, 0x1, "MCI_AudioPlay on Receiver");
                g_mciConfig.audioPath = MPS_AUDIO_PATH_RECEIVER;
                break;
            case MCI_PATH_HP:
                MCI_TRACE(MCI_AUD_TRC_LVL, 0x1, "MCI_AudioPlay on Headset");
                g_mciConfig.audioPath = MPS_AUDIO_PATH_HEADSET;
                break;
            case MCI_PATH_LSP:
                MCI_TRACE(MCI_AUD_TRC_LVL, 0x1, "MCI_AudioPlay on Louspeaker");
                g_mciConfig.audioPath = MPS_AUDIO_PATH_LOUD_SPEAKER;
                break;
            default:
                // Mode not supported
                MCI_TRACE(MCI_AUD_TRC_LVL, 0x1, "MCI_AudioPlay Error: Unsupported Outputpath");
                return (UINT32) MCI_ERR_INVALID_PARAMETER;
        }
        mps_Open();
        err = mps_OpenFile(&g_mciMediaHandle, file, MCI_AudioPlayerCallback, &g_mciConfig);
        if (MPS_ERR_NO != err)
        {
            if (MPS_ERR_UNKNOWN_FORMAT == err)
            {
                // File format not recognized
                MCI_TRACE(MCI_AUD_TRC_LVL, 0x1, "MCI_AudioPlay Error : wrong file format %s", file) ;
                return (UINT32) MCI_ERR_UNKNOWN_FORMAT;
            }
            else if (MPS_ERR_FILE_NOT_FOUND == err)
            {
                // Open file failed
                MCI_TRACE(MCI_AUD_TRC_LVL, 0x1, "MCI_AudioPlay Error : could not open %s", file) ;
                return (UINT32) MCI_ERR_INVALID_PARAMETER;
            }
        }
        err = mps_Play(g_mciMediaHandle, 1);
        if (err == MPS_ERR_NO)
        {
            return (UINT32) MCI_ERR_NO;
        }
        MCI_TRACE(MCI_AUD_TRC_LVL, 0x1, "MCI_AudioPlay Error %d", err) ;
        return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
    }
    return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
}

// =============================================================================
// MCI_AudioPause
// -----------------------------------------------------------------------------
/// Pause an audio object
// =============================================================================
UINT32 MCI_AudioPause(VOID)
{
    MPS_ERR_T err;

    MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioPause") ;

    if (g_mciMediaHandle != NULL)
    {
        err = mps_Pause(g_mciMediaHandle);
        if (err == MPS_ERR_NO)
        {
            return (UINT32) MCI_ERR_NO;
        }
        MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioPause Error %d", err) ;
        return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
    }
    MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioPause Error: No Open File") ;
    return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
}

// =============================================================================
// MCI_AudioResume
// -----------------------------------------------------------------------------
// Resume an already paused audio object
// =============================================================================
UINT32 MCI_AudioResume(VOID)
{
    MPS_ERR_T err;

    MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioResume") ;
    if (g_mciMediaHandle != NULL)
    {
        err = mps_Play(g_mciMediaHandle, 1);
        if (err == MPS_ERR_NO)
        {
            return (UINT32) MCI_ERR_NO;
        }
        MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioResume Error %d", err) ;
        return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
    }
    MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioResume Error: No Open File") ;
    return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
}

// =============================================================================
// MCI_AudioStop
// -----------------------------------------------------------------------------
// Resume an already paused audio object
// =============================================================================
UINT32 MCI_AudioStop(VOID)
{
    MPS_ERR_T err;

    MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioStop") ;

    if (g_mciMediaHandle != NULL)
    {
        err = mps_CloseFile(g_mciMediaHandle);
        if (err == MPS_ERR_NO)
        {
            // Reset the audio handle
            g_mciMediaHandle = NULL;
            // Reset the callback handle
            g_mciAudioPlayCallback = NULL;
            return (UINT32) MCI_ERR_NO;
        }
        MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioStop Error %d", err) ;
        return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
    }
    MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioStop Error: No Open File") ;
    return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
}

// =============================================================================
// MCI_AudioSetEQ
// -----------------------------------------------------------------------------
// Resume an already paused audio object
// =============================================================================
UINT32 MCI_AudioSetEQ(AUDIO_EQ EQMode)
{
    MPS_ERR_T err;

    MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioSetEQ %d", EQMode) ;

    // Update the configuration
    switch (EQMode)
    {
        case NORMAL:
            g_mciConfig.equalizerMode = MPS_EQUALIZER_MODE_NORMAL;
            break;
        case BASS:
            g_mciConfig.equalizerMode = MPS_EQUALIZER_MODE_BASS;
            break;
        case DANCE:
            g_mciConfig.equalizerMode = MPS_EQUALIZER_MODE_DANCE;
            break;
        case CLASSICAL:
            g_mciConfig.equalizerMode = MPS_EQUALIZER_MODE_CLASSICAL;
            break;
        case TREBLE:
            g_mciConfig.equalizerMode = MPS_EQUALIZER_MODE_TREBLE;
            break;
        case PARTY:
            g_mciConfig.equalizerMode = MPS_EQUALIZER_MODE_PARTY;
            break;
        case POP:
            g_mciConfig.equalizerMode = MPS_EQUALIZER_MODE_POP;
            break;
        case ROCK:
            g_mciConfig.equalizerMode = MPS_EQUALIZER_MODE_ROCK;
            break;
        default:
            // Mode not supported
            return (UINT32) MCI_ERR_INVALID_PARAMETER;
    }

    // Only update now if a file is opened
    // if no file is opened, it will be applied in the Open function
    if (g_mciMediaHandle != NULL)
    {
        err = mps_SetConfig(g_mciMediaHandle, &g_mciConfig);
        if (err == MPS_ERR_NO)
        {
            return (UINT32) MCI_ERR_NO;
        }
        MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioSetEQ Error:%d ", err) ;
        return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
    }
    return (UINT32) MCI_ERR_NO;
}

UINT32 MCI_AudioSetOutputPath(UINT16 OutputPath, UINT16 Mute)
{
    MPS_ERR_T err;

    MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioSetOutputPath OutputPath:%d Mute:%d", OutputPath, Mute) ;

    // Only allow set output path on an opened file
    // as the output path is setup in the open function
    if (g_mciMediaHandle != NULL)
    {
        if (Mute == 1)
        {
            g_mciAudioSavedVolume = g_mciConfig.volume;
            g_mciConfig.volume = 0;
        }
        else
        {
            g_mciConfig.volume = g_mciAudioSavedVolume;
        }
        switch (OutputPath)
        {
            case MCI_PATH_NORMAL:
                g_mciConfig.audioPath = MPS_AUDIO_PATH_RECEIVER;
                break;
            case MCI_PATH_HP:
                g_mciConfig.audioPath = MPS_AUDIO_PATH_HEADSET;
                break;
            case MCI_PATH_LSP:
                g_mciConfig.audioPath = MPS_AUDIO_PATH_LOUD_SPEAKER;
                break;
            default:
                // Mode not supported
                return (UINT32) MCI_ERR_INVALID_PARAMETER;
        }

        err = mps_SetConfig(g_mciMediaHandle, &g_mciConfig);
        if (err == MPS_ERR_NO)
        {
            return (UINT32) MCI_ERR_NO;
        }
        MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioSetOutputPath Error:%d ", err) ;
        return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
    }
    MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioSetOutputPath Error: No Open File") ;
    return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
}

UINT32 MCI_AudioSetVolume(UINT16 volume)
{
    MPS_ERR_T err;

    MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioSetVolume: %d ", volume) ;


    g_mciConfig.volume = volume - 1;
    g_mciAudioSavedVolume   = volume - 1;

    // Only update now if a file is opened
    // if no file is opened, it will be applied in the Open function
    if (g_mciMediaHandle != NULL)
    {
        err = mps_SetConfig(g_mciMediaHandle, &g_mciConfig);

        if (err == MPS_ERR_NO)
        {
            return (UINT32) MCI_ERR_NO;
        }
        MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioSetVolume Error:%d ", err) ;
        return (UINT32) MCI_ERR_ACTION_NOT_ALLOWED;
    }
    MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioSetVolume: No handler");
    return (UINT32) MCI_ERR_NO;
}

VOID MCI_AudioGetInfo()
{
    MPS_FILE_INFO_T *fileInfo;


    if (g_mciMediaHandle != NULL)
    {
        fileInfo = mps_GetInfo(g_mciMediaHandle);
    }
    else
    {
        MCI_TRACE(MCI_AUD_TRC_LVL, 0x0, "MCI_AudioGetInfo Error: No Open File") ;
    }

}

#endif // CSW_EXTENDED_API_AUDIO_VIDEO




