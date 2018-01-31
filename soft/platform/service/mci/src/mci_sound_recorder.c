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


#if (CSW_EXTENDED_API == 1)

#include "cs_types.h"
#include "mrs_m.h"
#include "mci.h"
#include "mcip_debug.h"
#include "mci_sound_recorder.h"

#include "fs.h"
#include "sxs_io.h"

// TODO Improve MCI error usage.

// =============================================================================
/// MACROS
// =============================================================================

// =============================================================================
/// TYPES
// =============================================================================

// =============================================================================
/// GLOBAL VARIABLES
// =============================================================================
PRIVATE MRS_HANDLE_T g_mciSndRecCurrentHandle = NULL;


// =============================================================================
// mci_SndRecRecord
// -----------------------------------------------------------------------------
/// Start the recording of an audio file, at the specified format.
/// If the file already exists, it is appended. Otherwise, it is created.
/// If the format is not supported, an error is returned.
///
/// @return #MCI_ERR_NO when everything is fine,
/// #MCI_ERR_UNKNOWN_FORMAT when the format is neither recognized nor supported,
/// #MCI_ERR_CANNOT_OPEN_FILE when the file cannot be accessed and
/// #MCI_ERR_ACTION_NOT_ALLOWED when the record cannot be performed.
// =============================================================================
PUBLIC INT32 mci_SndRecRecord(CHAR *fileName, INT16 format)
{
    // Record related variables
    MRS_AUDIO_CODEC_CFG_T   recordAudioCodecCfg;
    //    MRS_VIDEO_CODEC_CFG_T   recordVideoCodecCfg; // We don't care about recording video for now.
    MRS_CONTAINER_CFG_T     recordContainerCfg;
    MRS_RECORD_CFG_T        recordCfg;
    MRS_CFG_T               recordSetup;
    FS_FILE_ATTR            fileAttribute;
    MRS_FILE_MODE_T         fileMode;
    MRS_ERR_T               result;

    // Check that the file exists.
    // If the file  exists, data are appended.
    if (FS_GetFileAttr(fileName, &fileAttribute) >= 0)
    {
        // File exists.
        MCI_TRACE(TSTDOUT, 0, "MCI:SND_REC: File opened in appending mode.");
        fileMode = MRS_FILE_MODE_APPEND;
    }
    else
    {
        MCI_TRACE(TSTDOUT, 0, "MCI:SND_REC: File opened in creating mode.");
        fileMode = MRS_FILE_MODE_CREATE;
    }

    // Record audio codec
    switch (format)
    {
        case MED_TYPE_AMR:
            recordAudioCodecCfg.type = MRS_AUDIO_CODEC_AMR_RING;
            break;

        case MED_TYPE_PCM_8K:
        case MED_TYPE_WAV:
            recordAudioCodecCfg.type = MRS_AUDIO_CODEC_WAV;
            break;

        default:
            return MCI_ERR_UNKNOWN_FORMAT;
            break;
    }


    // No video configuration

    // Container configuration
    recordContainerCfg.type = MRS_CONTAINER_RAW;



    // Bring that together for the record config structure
    recordCfg.audioCodecCfg = &recordAudioCodecCfg;
    recordCfg.videoCodecCfg = NULL; // No video
    recordCfg.containerCfg  = &recordContainerCfg;

    // Audio configuration
    recordCfg.audioCfg.audioPath     = MRS_AUDIO_PATH_HEADSET; // FIXME !
    recordCfg.audioCfg.volume        = 1;
    recordCfg.audioCfg.equalizerMode = MRS_EQUALIZER_MODE_NORMAL;

    // Video configuration (huh ?)
    recordCfg.videoCfg.videoPath     = 0;
    recordCfg.videoCfg.videoRotation = MRS_VIDEO_ROTATION_LANDSCAPE;
    // lazy
    // recordVideoCfg.effects = ... ;
    recordCfg.callback      = NULL;

    recordSetup.audio.audioPath = recordCfg.audioCfg.audioPath;
    recordSetup.audio.volume = recordCfg.audioCfg.volume;
    recordSetup.audio.equalizerMode = recordCfg.audioCfg.equalizerMode;
    recordSetup.video.videoPath = recordCfg.videoCfg.videoPath;
    recordSetup.video.videoRotation = recordCfg.videoCfg.videoRotation;
    // FIXME handle video effects

    // Open service
    mrs_Open();

    // Record a file.
    result = mrs_OpenFile(&g_mciSndRecCurrentHandle, fileName, fileMode, &recordCfg);
    if (result != MRS_ERR_NO || g_mciSndRecCurrentHandle == NULL)
    {
        SXS_TRACE(TSTDOUT, "Can't open file for record\n");
        return MCI_ERR_CANNOT_OPEN_FILE;
    }
    else
    {
        result = mrs_Record(g_mciSndRecCurrentHandle, 0);
        if (result != MRS_ERR_NO)
        {
            // Service not opened, or no handle
            return MCI_ERR_ACTION_NOT_ALLOWED;
        }
    }

    return MCI_ERR_NO;
}


// =============================================================================
// mci_SndRecStop
// -----------------------------------------------------------------------------
/// Stop a record in progress, or does nothing if no recording is occuring.
///
/// @return #MCI_ERR_NO is a recording was properly stopped, or no action
/// was needed. #MCI_ERR_ACTION_NOT_ALLOWED is returned otherwise.
// =============================================================================
PUBLIC INT32 mci_SndRecStop(VOID)
{
    UINT32 result = MCI_ERR_NO;
    if (g_mciSndRecCurrentHandle != NULL)
    {
        if (mrs_CloseFile(g_mciSndRecCurrentHandle) != MRS_ERR_NO)
        {
            // FIXME Densify error management
            result = MCI_ERR_ACTION_NOT_ALLOWED;
        }
        else
        {
            g_mciSndRecCurrentHandle = NULL;
            // TODO what about that error ?
            if (mrs_Close() != MRS_ERR_NO)
            {
                result = MCI_ERR_ACTION_NOT_ALLOWED;
            }
        }
    }

    return result;
}


// =============================================================================
// mci_SndRecPause
// -----------------------------------------------------------------------------
/// Pause a recording in progress, or does nothing if no recording is currently
/// underway.
///
/// @return #MCI_ERR_NO is a recording was properly paused, or no action
/// was needed. #MCI_ERR_ACTION_NOT_ALLOWED is returned otherwise.
// =============================================================================
PUBLIC INT32 mci_SndRecPause(VOID)
{
    UINT32 result = MCI_ERR_NO;
    if (g_mciSndRecCurrentHandle != NULL)
    {
        if (mrs_Pause(g_mciSndRecCurrentHandle) != MRS_ERR_NO)
        {
            result = MCI_ERR_ACTION_NOT_ALLOWED;
        }
    }

    return result;
}


// =============================================================================
// mci_SndRecResume
// -----------------------------------------------------------------------------
/// If a recording is in pause, resumes it.
///
/// @return #MCI_ERR_NO if the operation occured normaly, or if there was no
/// recording to resume. #MCI_ERR_ACTION_NOT_ALLOWED is returned otherwise.
// =============================================================================
PUBLIC INT32 mci_SndRecResume(VOID)
{
    UINT32 result = MCI_ERR_NO;
    if (g_mciSndRecCurrentHandle != NULL)
    {
        if (mrs_Record(g_mciSndRecCurrentHandle, 0) != MRS_ERR_NO)
        {
            result = MCI_ERR_ACTION_NOT_ALLOWED;
        }
    }

    return result;
}



#endif // CSW_EXTENDED_API



