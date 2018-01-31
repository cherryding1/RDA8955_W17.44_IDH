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
#include "tgt_aud_cfg.h"

#include "aud_m.h"

#include "hal_aif.h"
#include "hal_sys.h"
#include "hal_rda_audio.h"
#include "hal_timers.h"

#include "sxr_tls.h"
#include "sxr_mutx.h"
#include "dm.h"

#include "audp_debug.h"
#include "aud_codec_common.h"

#include "aud_fm.h"

// =============================================================================
//  MACROS
// =============================================================================

#define SND_ITF_HAS_PHY_SPK(itf) \
    (itf == SND_ITF_RECEIVER || itf == SND_ITF_EAR_PIECE || itf == SND_ITF_LOUD_SPEAKER)

// 0.5 second for 8K Hz sample rate (16 bits per sample)
#define AUD_SIDE_TEST_BUF_LENGTH  8000

// 64 ms for 8K Hz sample rate (16 bits per sample)
#define AUD_RECVMIC_IN_EARPIECE_OUT_BUF_LENGTH  1024


// =============================================================================
//  GLOBALS
// =============================================================================

PUBLIC BOOL g_audSetHeadMaxDigitalGain = FALSE;
PUBLIC BOOL g_audSetMicMaxGain = FALSE;

PRIVATE AUD_TEST_T  g_audRunningTest        = AUD_TEST_NO;
PRIVATE SND_ITF_T   g_audEnabledRecordItf   = SND_ITF_NONE;
PRIVATE SND_ITF_T   g_audEnabledPlayItf     = SND_ITF_NONE;
PRIVATE SND_ITF_T   g_audEnabledToneItf     = SND_ITF_NONE;

PRIVATE BYTE        g_audMutex          = 0xFF;

/// Current output device for FM/TV
PRIVATE SND_ITF_T   g_audCurOutputDevice    = SND_ITF_NONE;

/// Whether to select loud speaker output along with earpiece
PRIVATE BOOL g_audLoudspeakerWithEarpiece   = FALSE;

// Test mode record buffer
PRIVATE UINT8 *g_audTestModeRecordBuffer = NULL;

// Test mode buffer malloc
PRIVATE AUD_TEST_MODE_MALLOC_FUNC_T g_audTestModeMallocFuncPtr = NULL;

// Test mode buffer free
PRIVATE AUD_TEST_MODE_FREE_FUNC_T g_audTestModeFreeFuncPtr = NULL;

#ifdef AUD_3_IN_1_SPK
PRIVATE UINT32 g_BypassNotchFilterApp = FALSE;
#endif
SND_ITF_T musicItf = SND_ITF_LOUD_SPEAKER;

#ifndef CHIP_HAS_AP
PRIVATE BOOL isMICMute = FALSE;
#endif

#if defined(BT_A2DP_ANA_DAC_MUTE)
PRIVATE BOOL isFMWorking = FALSE;
#endif


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// aud_MutexTake
// -----------------------------------------------------------------------------
/// Take the mutex protecting AUD shared variables.
///
/// @return The user ID to take this mutex.
// =============================================================================
PUBLIC UINT8 aud_MutexTake(VOID)
{
    // Create the semaphore if it doesn't exist.
    UINT32 status = hal_SysEnterCriticalSection();
    if (g_audMutex == 0xFF)
    {
        g_audMutex = sxr_NewMutex();
    }
    hal_SysExitCriticalSection(status);

    return sxr_TakeMutex(g_audMutex);
}


// =============================================================================
// aud_MutexRelease
// -----------------------------------------------------------------------------
/// Release the mutex protecting AUD shared variables.
///
/// @param userId The user ID to release this mutex
// =============================================================================
PUBLIC VOID aud_MutexRelease(UINT8 userId)
{
    sxr_ReleaseMutex(g_audMutex, userId);
}


// =============================================================================
// aud_Setup
// -----------------------------------------------------------------------------
/// Configure the audio.
///
/// This functions configures an audio interface: gain for the side tone and
/// the speaker, input selection for the microphone and output selection for
/// the speaker, enabling of the microphone ... It can be used to change
/// 'on the fly' the input or output used to play/record a stream on the
/// same interface, but cannot be used to change the interface during a
/// playback or a record.
///
/// @param itf Interface number of the interface to setup.
/// @param cfg The configuration set applied to the audio interface. See
/// #AUD_LEVEL_T for more details.
/// @return #AUD_ERR_NO if it can execute the configuration.
// =============================================================================
PUBLIC AUD_ERR_T aud_Setup(SND_ITF_T itf, CONST AUD_LEVEL_T* cfg)
{
    AUD_ERR_T errStatus = AUD_ERR_NO;
    CONST AUD_ITF_CFG_T* audioCfg = tgt_GetAudConfig();

    AUD_ASSERT(itf<SND_ITF_QTY, "This Audio interface does not"
               "Exist on this target");

    musicItf = itf;

#if defined(AUD_SPK_ON_WITH_LCD) || defined(AUD_EXT_PA_ON_WITH_LCD)
    AUD_ASSERT(audioCfg[SND_ITF_LOUD_SPEAKER].spkSel == AUD_SPK_LOUD_SPEAKER,
               "TGT_AUD_CONFIG_LOUD_SPEAKER_OUTPUT_PATH should be set to AUD_SPK_LOUD_SPEAKER");
#endif

    AUD_TRACE(AUD_INFO_TRC, 0, "aud_Setup begin itf: %d (spkLvl=%d, micLvl=%d)",
              itf, cfg->spkLevel, cfg->micLevel);

    UINT8 userId = aud_MutexTake();

    // Only update a configuration if AUD is being used
    // Otherwise, do nothing.
    if (g_audEnabledRecordItf != SND_ITF_NONE || g_audEnabledPlayItf != SND_ITF_NONE || g_audEnabledToneItf != SND_ITF_NONE)
    {
        // Only change the AUD properties of the interface if, and only if
        // this interface is the interface being used.
        // FIXME The MMI is bogous and do not respect this constraint:
        // Fix the MMI and reenable this test. (We use a flag to avoid
        // to comment or #if 0 those line)
#ifdef THE_MMI_COMPLY_TO_AUD_SPEC
        if (((g_audEnabledRecordItf == itf) || (g_audEnabledRecordItf   == SND_ITF_NONE))
                && ((g_audEnabledPlayItf   == itf) || (g_audEnabledPlayItf     == SND_ITF_NONE))
                && ((g_audEnabledToneItf   == itf) || (g_audEnabledToneItf     == SND_ITF_NONE)))
        {
            AUD_DEVICE_CFG_T deviceCfg;
            deviceCfg.spkSel    = audioCfg[itf].spkSel;
            deviceCfg.spkType   = audioCfg[itf].spkType;
            deviceCfg.micSel = audioCfg[itf].micSel;
            deviceCfg.level     = cfg;

            if (!SND_ITF_HAS_PHY_SPK(itf))
            {
                if (SND_ITF_HAS_PHY_SPK(g_audCurOutputDevice))
                {
                    deviceCfg.spkSel = audioCfg[g_audCurOutputDevice].spkSel;
                }
            }

            if (g_audLoudspeakerWithEarpiece)
            {
                if (deviceCfg.spkSel == AUD_SPK_LOUD_SPEAKER)
                {
                    deviceCfg.spkSel = AUD_SPK_LOUD_SPEAKER_EAR_PIECE;
                }
            }

            if (audioCfg[itf].setup)
            {
                // There is a setup function to call
                // for that interface
                errStatus = (audioCfg[itf].setup(itf, &deviceCfg));
            }
            else
            {
                // There is no driver for that interface,
                // ie. the interface does not exist.
                errStatus =  AUD_ERR_NO_ITF;
            }
        }
        else
        {
            // Another interface is using the audio
            // ignore
            AUD_TRACE(AUD_INFO_TRC, 0,
                      "AUD: " __FILE__ ", line:%d, Resource Busy, itf:%d",__LINE__, itf);
            errStatus = AUD_ERR_RESOURCE_BUSY;
        }

#else

        // Interface switching is allowed among physical speaker
        // interfaces only
        if ( ( (g_audEnabledRecordItf == SND_ITF_NONE) ||
                (g_audEnabledRecordItf == itf) ||
                ( SND_ITF_HAS_PHY_SPK(itf) &&
                  SND_ITF_HAS_PHY_SPK(g_audEnabledRecordItf) )
             ) &&
                ( (g_audEnabledPlayItf == SND_ITF_NONE) ||
                  (g_audEnabledPlayItf == itf) ||
                  ( SND_ITF_HAS_PHY_SPK(itf) &&
                    SND_ITF_HAS_PHY_SPK(g_audEnabledPlayItf) )
                ) &&
                ( (g_audEnabledToneItf == SND_ITF_NONE) ||
                  (g_audEnabledToneItf == itf) ||
                  ( SND_ITF_HAS_PHY_SPK(itf) &&
                    SND_ITF_HAS_PHY_SPK(g_audEnabledToneItf) )
                )
           )
        {
            AUD_DEVICE_CFG_T deviceCfg;

            if(g_audEnabledRecordItf!=SND_ITF_NONE)
            {
                g_audEnabledRecordItf=itf;
            }

            if(g_audEnabledPlayItf!=SND_ITF_NONE)
            {
                g_audEnabledPlayItf=itf;
            }

            if(g_audEnabledToneItf!=SND_ITF_NONE)
            {
                g_audEnabledToneItf=itf;
            }

            deviceCfg.spkSel    = audioCfg[itf].spkSel;
            deviceCfg.spkType   = audioCfg[itf].spkType;
            deviceCfg.micSel = audioCfg[itf].micSel;
            deviceCfg.level     = cfg;

            if (!SND_ITF_HAS_PHY_SPK(itf))
            {
                if (SND_ITF_HAS_PHY_SPK(g_audCurOutputDevice))
                {
                    deviceCfg.spkSel = audioCfg[g_audCurOutputDevice].spkSel;
                }
            }

            if (g_audLoudspeakerWithEarpiece)
            {
                if (deviceCfg.spkSel == AUD_SPK_LOUD_SPEAKER)
                {
                    deviceCfg.spkSel = AUD_SPK_LOUD_SPEAKER_EAR_PIECE;
                }
            }

            if(deviceCfg.spkSel == AUD_SPK_EAR_PIECE)
            {
#ifdef AUD_3_IN_1_SPK
                hal_AudBypassNotchFilterReq(TRUE);
#endif
            }
            else
            {
#ifdef AUD_3_IN_1_SPK
                 hal_AudBypassNotchFilterReq(FALSE);
#endif
            }
            if (audioCfg[itf].setup)
            {
                // There is a setup function to call
                // for that interface
                errStatus = (audioCfg[itf].setup(itf, &deviceCfg));
            }
            else
            {
                // There is no driver for that interface,
                // ie. the interface does not exist.
                errStatus =  AUD_ERR_NO_ITF;
            }
        }
        else
        {
            // Another interface is using the audio
            // ignore
            AUD_TRACE(AUD_INFO_TRC, 0,
                      "AUD: " __FILE__ ", line:%d, Resource Busy, itf:%d",__LINE__, itf);
            errStatus = AUD_ERR_RESOURCE_BUSY;
        }

#endif

    }
    else
    {
        // Nothing to do in that case, this can be
        // interpreted as a no error call to the function.
        errStatus = AUD_ERR_NO;
    }

    AUD_TRACE(AUD_INFO_TRC, 0, "aud_Setup end itf: %d", itf);

#if defined(BT_A2DP_ANA_DAC_MUTE)
    if( (errStatus == AUD_ERR_NO) && (SND_ITF_FM == itf))
    {
        aud_SetFMWorkingFlag(TRUE);
    }
#endif

    aud_MutexRelease(userId);
    return errStatus;
}

PUBLIC VOID aud_StreamReset()
{
    g_audEnabledToneItf = SND_ITF_NONE;
}
// =============================================================================
// aud_StreamStart
// -----------------------------------------------------------------------------
/// Start the playing of a stream.
///
/// This function  outputs the audio stream on the audio interface specified as
/// a parameter, on the output selected in the AUD_LEVEL_T cfg parameter. \n
/// When the buffer runs out, the playing will wrap at the
/// beginning of the buffer. If defined in the HAL_AIF_STREAM_T structure,
/// a user function can be called at the middle or at the end of the playback.
///
/// @param itf Interface on which play that stream
/// @param stream Stream to play. Handler called at the middle and end of the
/// buffer are defined there. The length of a stream must be a multiple
/// of 16 bytes.
/// @param cfg The configuration set applied to the audio interface
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command,
///         #AUD_ERR_NO it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_StreamStart(
    CONST SND_ITF_T      itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_LEVEL_T *       cfg)
{
    CONST AUD_ITF_CFG_T* audioCfg = tgt_GetAudConfig();
    AUD_DEVICE_CFG_T deviceCfg;
    AUD_ERR_T errStatus = AUD_ERR_NO;

    AUD_ASSERT(itf<SND_ITF_QTY, "This Audio interface does not"
               "Exist on this target");
    AUD_ASSERT(stream, "Stream == 0");

    musicItf = itf;

    AUD_TRACE(AUD_INFO_TRC, 0, "aud_StreamStart itf: %d (spkLvl=%d, micLvl=%d)",
              itf, cfg->spkLevel, cfg->micLevel);
    AUD_TRACE(AUD_INFO_TRC, 0, "aud_StreamStart stream: %d ",
              stream->sampleRate);

    UINT8 userId = aud_MutexTake();

    if (((g_audEnabledRecordItf == itf)             || (g_audEnabledRecordItf   == SND_ITF_NONE))
            &&  (g_audEnabledPlayItf   == SND_ITF_NONE)
            && ((g_audEnabledToneItf   == itf)             || (g_audEnabledToneItf     == SND_ITF_NONE)))
    {
        // Stop tone first
        if (g_audEnabledToneItf != SND_ITF_NONE)
        {
            if (audioCfg[itf].tone)
            {
                // Only the start fields needs to be specified,
                // set to FALSE to stop the tone generation.
                errStatus = (audioCfg[itf].tone(itf, 0, NULL, FALSE));
                if (errStatus == AUD_ERR_NO)
                {
                    // Stop the tone
                    // Set the new used interface
                    g_audEnabledToneItf = SND_ITF_NONE;
                }
            }
            else
            {
                errStatus = AUD_ERR_NO_ITF;
            }
        }

        if (errStatus == AUD_ERR_NO)
        {
            deviceCfg.spkSel  = audioCfg[itf].spkSel;
            deviceCfg.spkType = audioCfg[itf].spkType;
            deviceCfg.micSel = audioCfg[itf].micSel;
            deviceCfg.level   = cfg;

            if (!SND_ITF_HAS_PHY_SPK(itf))
            {
                if (SND_ITF_HAS_PHY_SPK(g_audCurOutputDevice))
                {
                    deviceCfg.spkSel = audioCfg[g_audCurOutputDevice].spkSel;
                }
            }

            if (g_audLoudspeakerWithEarpiece)
            {
                if (deviceCfg.spkSel == AUD_SPK_LOUD_SPEAKER)
                {
                    deviceCfg.spkSel = AUD_SPK_LOUD_SPEAKER_EAR_PIECE;
                }
            }

            if (audioCfg[itf].play)
            {
                // Interface driver exist: play
                errStatus = (audioCfg[itf].play(itf, stream, &deviceCfg));
                if (errStatus == AUD_ERR_NO)
                {
                     if(deviceCfg.spkSel == AUD_SPK_EAR_PIECE)
                     {
                         #ifdef AUD_3_IN_1_SPK
                         hal_AudBypassNotchFilterReq(TRUE);
                         #endif
                     }
                     else
                     {
                         #ifdef AUD_3_IN_1_SPK
                         hal_AudBypassNotchFilterReq(FALSE);
                         #endif
                     }

                    // The play actually started, set the (new) used interface
                    g_audEnabledPlayItf = itf;
                }
            }
            else
            {
                // No interface
                errStatus = AUD_ERR_NO_ITF;
            }
        }
    }
    else
    {
        // Interface is busy
        AUD_TRACE(AUD_INFO_TRC, 0,
                  "AUD: " __FILE__ ", line:%d, Resource Busy, itf:%d, used itf:%d",__LINE__, itf, g_audEnabledPlayItf);
        errStatus = AUD_ERR_RESOURCE_BUSY;
    }

#if defined(BT_A2DP_ANA_DAC_MUTE)
    if( (errStatus == AUD_ERR_NO) && (SND_ITF_FM == itf))
    {
        aud_SetFMWorkingFlag(TRUE);
    }
#endif

    aud_MutexRelease(userId);
    return errStatus;
}


// =============================================================================
// aud_StreamStop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback or/and record.
/// If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param itf Interface on which to stop the playing.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_StreamStop(CONST SND_ITF_T itf)
{
    // Some applications will try to stop an audio stream in mutiple paths,
    // then it is possible for them to stop a SND_ITF_NONE interface.
    if (itf == SND_ITF_NONE)
    {
        return AUD_ERR_NO;
    }

    CONST AUD_ITF_CFG_T* audioCfg = tgt_GetAudConfig();
    AUD_ERR_T errStatus = AUD_ERR_NO;

    AUD_ASSERT(itf<SND_ITF_QTY, "This Audio interface does not"
               "Exist on this target");
    AUD_TRACE(AUD_INFO_TRC, 0, "aud_StreamStop itf: %d", itf);

    UINT8 userId = aud_MutexTake();

    if ((g_audEnabledRecordItf != SND_ITF_NONE) || (g_audEnabledPlayItf != SND_ITF_NONE) || (g_audEnabledToneItf != SND_ITF_NONE))
    {
        if (((g_audEnabledRecordItf == itf) || (g_audEnabledRecordItf   == SND_ITF_NONE))
                && ((g_audEnabledPlayItf   == itf) || (g_audEnabledPlayItf     == SND_ITF_NONE))
                && ((g_audEnabledToneItf   == itf) || (g_audEnabledToneItf     == SND_ITF_NONE)))
        {
            // Stop tone first
            if (g_audEnabledToneItf != SND_ITF_NONE)
            {
                if (audioCfg[itf].tone)
                {
                    // Only the start fields needs to be specified,
                    // set to FALSE to stop the tone generation.
                    errStatus = (audioCfg[itf].tone(itf, 0, NULL, FALSE));
                    if (errStatus == AUD_ERR_NO)
                    {
                        // Stop the tone
                        // Set the new used interface
                        g_audEnabledToneItf = SND_ITF_NONE;
                    }
                }
                else
                {
                    errStatus = AUD_ERR_NO_ITF;
                }
            }

            if (errStatus == AUD_ERR_NO)
            {
                // Stop stream then
                if (audioCfg[itf].stop)
                {
                    errStatus = audioCfg[itf].stop(itf);
                    if (errStatus == AUD_ERR_NO)
                    {
                        // Freeing currently used interface
                        // Set the new freed interface
                        g_audEnabledRecordItf   = SND_ITF_NONE;
                        g_audEnabledPlayItf     = SND_ITF_NONE;
                        // Set current output device to default
                        g_audCurOutputDevice = SND_ITF_NONE;
                    }
                }
                else
                {
                    errStatus = AUD_ERR_NO_ITF;
                }
            }
        }
        else
        {
            // Another interface is in use. Ignore
            AUD_TRACE(AUD_INFO_TRC, 0,
                      "AUD: " __FILE__ ", line:%d, Resource Busy, itf:%d, used itf:%d",__LINE__, itf, g_audEnabledPlayItf);
            errStatus = AUD_ERR_RESOURCE_BUSY;
        }
    }
    else
    {
        // Nothing to stop, not illegal (though useless)
        errStatus = AUD_ERR_NO;
    }

#if defined(BT_A2DP_ANA_DAC_MUTE)
    if( (errStatus == AUD_ERR_NO) && (SND_ITF_FM == itf))
    {
        aud_SetFMWorkingFlag(FALSE);
    }
#endif

    aud_MutexRelease(userId);

    musicItf = itf;
    return errStatus;
}

// =============================================================================
// aud_StreamRecordStop
// -----------------------------------------------------------------------------
/// This function stops the audio stream record.
/// If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param itf Interface on which to stop the playing.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_StreamRecordStop(SND_ITF_T itf)
{
    // Some applications will try to stop an audio stream in mutiple paths,
    // then it is possible for them to stop a SND_ITF_NONE interface.
    if (itf == SND_ITF_NONE)
    {
        return AUD_ERR_NO;
    }

    AUD_ERR_T errStatus = AUD_ERR_NO;

    AUD_ASSERT(itf<SND_ITF_QTY, "This Audio interface does not"
               "Exist on this target");
    AUD_TRACE(AUD_INFO_TRC, 0, "aud_StreamRecordStop itf: %d", itf);

    UINT8 userId = aud_MutexTake();

    if ((g_audEnabledRecordItf != SND_ITF_NONE) || (g_audEnabledPlayItf != SND_ITF_NONE) || (g_audEnabledToneItf != SND_ITF_NONE))
    {
        if (((g_audEnabledRecordItf == itf) || (g_audEnabledRecordItf   == SND_ITF_NONE))
                && ((g_audEnabledPlayItf   == itf) || (g_audEnabledPlayItf     == SND_ITF_NONE))
                && ((g_audEnabledToneItf   == itf) || (g_audEnabledToneItf     == SND_ITF_NONE)))
        {

            if (errStatus == AUD_ERR_NO)
            {
                errStatus = aud_FmStreamRecordStop(itf);
                if (errStatus == AUD_ERR_NO)
                {
                    // Freeing currently used interface
                    // Set the new freed interface
                    g_audEnabledRecordItf   = SND_ITF_NONE;
                }
            }
        }
        else
        {
            // Another interface is in use. Ignore
            AUD_TRACE(AUD_INFO_TRC, 0,
                      "AUD: " __FILE__ ", line:%d, Resource Busy, itf:%d, used itf:%d",__LINE__, itf, g_audEnabledPlayItf);
            errStatus = AUD_ERR_RESOURCE_BUSY;
        }
    }
    else
    {
        // Nothing to stop, not illegal (though useless)
        errStatus = AUD_ERR_NO;
    }

    aud_MutexRelease(userId);

    musicItf = itf;
    return errStatus;
}



// =============================================================================
// aud_StreamPause
// -----------------------------------------------------------------------------
/// This function pauses or resume the audio stream playback or/and record.
/// If the function returns #AUD_ERR_RESOURCE_BUSY, it means that the driver
/// is busy with another audio command.
///
/// @param itf Interface on which to stop the playing.
/// @param pause If \c TRUE, pauses the stream. If \c FALSE, resumes a paused
/// stream.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_StreamPause(CONST SND_ITF_T itf, CONST BOOL pause)
{
    CONST AUD_ITF_CFG_T* audioCfg = tgt_GetAudConfig();
    AUD_ERR_T errStatus = AUD_ERR_NO;

    AUD_ASSERT(itf<SND_ITF_QTY, "This Audio interface does not"
               "Exist on this target");

    musicItf = itf;

    AUD_TRACE(AUD_INFO_TRC, 0, "aud_StreamPause itf: %d ", itf);

    UINT8 userId = aud_MutexTake();

    if ((g_audEnabledRecordItf != SND_ITF_NONE) || (g_audEnabledPlayItf != SND_ITF_NONE) || (g_audEnabledToneItf != SND_ITF_NONE))
    {
        if (((g_audEnabledRecordItf == itf) || (g_audEnabledRecordItf   == SND_ITF_NONE))
                && ((g_audEnabledPlayItf   == itf) || (g_audEnabledPlayItf     == SND_ITF_NONE))
                && ((g_audEnabledToneItf   == itf) || (g_audEnabledToneItf     == SND_ITF_NONE)))
        {
            // Pause current interface
            if (audioCfg[itf].pause)
            {
                errStatus = (audioCfg[itf].pause(itf, pause));
            }
            else
            {
                errStatus = AUD_ERR_NO_ITF;
            }

        }
        else
        {
            // Another interface is in use. Ignore
            AUD_TRACE(AUD_INFO_TRC, 0,
                      "AUD: " __FILE__ ", line:%d, Resource Busy, itf:%d, used itf:%d",__LINE__, itf, g_audEnabledPlayItf);
            errStatus = AUD_ERR_RESOURCE_BUSY;
        }
    }
    else
    {
        // Nothing to pause, not illegal (though useless)
        errStatus = AUD_ERR_NO;
    }

    AUD_TRACE(AUD_INFO_TRC, 0, "aud_StreamPause itf: %d, errStatus: %d", itf,errStatus);

    aud_MutexRelease(userId);
    return errStatus;
}



// =============================================================================
// aud_StreamRecord
// -----------------------------------------------------------------------------
/// Manage the recording of a stream.
/// This function records the audio stream from the audio interface specified as
/// a parameter, from the input selected in the AUD_LEVEL_T cfg parameter.\n
/// In normal operation, when the buffer runs out, the recording will wrap at the
/// beginning of the buffer. If defined in the HAL_AIF_STREAM_T structure,
/// a user function can be called at the middle or at the end of the record.
///
/// @param itf Interface whose PCM flow is recorded.
/// @param stream Stream recorded (describe the buffer and the possible interrupt
/// of middle-end buffer handlers. The length of a stream must be a multiple
/// of 16 bytes.
/// @param cfg The configuration set applied to the audio interface.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver cannot handle the stream,
///         #AUD_ERR_NO if the stream ca be recorded.
// =============================================================================
PUBLIC AUD_ERR_T aud_StreamRecord(
    CONST SND_ITF_T      itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_LEVEL_T *       cfg)
{
    CONST AUD_ITF_CFG_T* audioCfg = tgt_GetAudConfig();
    AUD_DEVICE_CFG_T deviceCfg;
    AUD_ERR_T errStatus = AUD_ERR_NO;

    AUD_ASSERT(itf<SND_ITF_QTY, "This Audio interface does not"
               "Exist on this target");

    musicItf = itf;

    AUD_TRACE(AUD_INFO_TRC, 0, "aud_StreamRecord itf: %d (spkLvl=%d, micLvl=%d)",
              itf, cfg->spkLevel, cfg->micLevel);
    AUD_TRACE(AUD_INFO_TRC, 0, "aud_StreamRecord stream: %d ",
              stream->sampleRate);

    UINT8 userId = aud_MutexTake();

    if ((g_audEnabledRecordItf  == SND_ITF_NONE)
            && ((g_audEnabledPlayItf   == itf) || (g_audEnabledPlayItf     == SND_ITF_NONE))
            && ((g_audEnabledToneItf   == itf) || (g_audEnabledToneItf     == SND_ITF_NONE)))
    {
        // Record
        deviceCfg.spkSel    = audioCfg[itf].spkSel;
        deviceCfg.spkType   = audioCfg[itf].spkType;
        deviceCfg.micSel = audioCfg[itf].micSel;
        deviceCfg.level     = cfg;

        if (!SND_ITF_HAS_PHY_SPK(itf))
        {
            if (SND_ITF_HAS_PHY_SPK(g_audCurOutputDevice))
            {
                deviceCfg.spkSel = audioCfg[g_audCurOutputDevice].spkSel;
            }
        }

        if (g_audLoudspeakerWithEarpiece)
        {
            if (deviceCfg.spkSel == AUD_SPK_LOUD_SPEAKER)
            {
                deviceCfg.spkSel = AUD_SPK_LOUD_SPEAKER_EAR_PIECE;
            }
        }

        if (audioCfg[itf].record)
        {
            errStatus = (audioCfg[itf].record(itf, stream, &deviceCfg));
            if (errStatus == AUD_ERR_NO)
            {
                // Start recording on the (new) enabled interface
                // Set the (new) used interface
                g_audEnabledRecordItf = itf;
            }
        }
        else
        {
            errStatus = AUD_ERR_NO_ITF;
        }
    }
    else
    {
        // AUD is activated on another interface
        AUD_TRACE(AUD_INFO_TRC, 0,
                  "AUD: " __FILE__ ", line:%d, Resource Busy, itf:%d, used itf:%d",__LINE__, itf, g_audEnabledRecordItf);
        errStatus = AUD_ERR_RESOURCE_BUSY;
    }

    aud_MutexRelease(userId);
    return errStatus;
}


// =============================================================================
// aud_ToneStart
// -----------------------------------------------------------------------------
/// Manage the playing of a tone: DTMF or Comfort Tone.
///
/// This function starts the output of a tone generated in the audio
/// module on the selected interface. It a stream is being played on the same
/// interface, the stream will continue to be played the  its sound won't be
/// output. It will be replaced instead by the generated tone.
///
/// You can call this function several times without calling the
/// #aud_ToneStop function or the #aud_TonePause function in
/// between, if you just need to change the attenuation or the tone type. \n
/// If the function returns #AUD_ERR_RESOURCE_BUSY, it means that the driver is
/// busy with an other audio command.
///
/// @param itf AUD interface on which to play a tone.
/// @param tone The tone to generate.
/// @param cfg The configuration set applied to the audio interface.
///
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_ToneStart(
    CONST SND_ITF_T         itf,
    CONST SND_TONE_TYPE_T   tone,
    CONST AUD_LEVEL_T*      cfg)
{
    CONST AUD_ITF_CFG_T*    audioCfg = tgt_GetAudConfig();
    AUD_DEVICE_CFG_T        deviceCfg;
    AUD_ERR_T               errStatus;

    AUD_ASSERT(itf<SND_ITF_QTY, "This Audio interface does not"
               "Exist on this target");

    AUD_TRACE(AUD_INFO_TRC, 0, "aud_ToneStart itf: %d (spkLvl=%d, micLvl=%d)",
              itf, cfg->spkLevel, cfg->micLevel);

    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_104M);
    UINT8 userId = aud_MutexTake();

    if (((g_audEnabledRecordItf == itf) || (g_audEnabledRecordItf   == SND_ITF_NONE))
            && ((g_audEnabledPlayItf   == itf) || (g_audEnabledPlayItf     == SND_ITF_NONE))
            && ((g_audEnabledToneItf   == SND_ITF_NONE)))
    {
        deviceCfg.spkSel    = audioCfg[itf].spkSel;
        deviceCfg.spkType   = audioCfg[itf].spkType;
        deviceCfg.micSel    = audioCfg[itf].micSel;
        deviceCfg.level     = cfg;

        if (g_audLoudspeakerWithEarpiece)
        {
            if (deviceCfg.spkSel == AUD_SPK_LOUD_SPEAKER)
            {
                deviceCfg.spkSel = AUD_SPK_LOUD_SPEAKER_EAR_PIECE;
            }
        }

        if (audioCfg[itf].tone)
        {
            errStatus = (audioCfg[itf].tone(itf, tone, &deviceCfg, TRUE));
            if (errStatus == AUD_ERR_NO)
            {
                // Start toning on the (new) interface
                // Set the new used interface
                g_audEnabledToneItf = itf;
            }
        }
        else
        {
            errStatus = AUD_ERR_NO_ITF;
        }
    }
    else
    {
        // AUD is activated on another interface
        AUD_TRACE(AUD_INFO_TRC, 0,
                  "AUD: " __FILE__ ", line:%d, Resource Busy, itf:%d, used itf:%d",__LINE__, itf, g_audEnabledToneItf);
        errStatus = AUD_ERR_RESOURCE_BUSY;
    }

    aud_MutexRelease(userId);
    return errStatus;
}

PUBLIC AUD_ERR_T aud_ToneSetup(
    CONST SND_ITF_T         itf)
{
    CONST AUD_ITF_CFG_T*    audioCfg = tgt_GetAudConfig();
    AUD_DEVICE_CFG_T        deviceCfg;
    AUD_ERR_T               errStatus;

    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_104M);
    g_audEnabledToneItf = itf;
    return errStatus;
}


// =============================================================================
// aud_ToneStop
// -----------------------------------------------------------------------------
/// Stop the tone generation.
/// If the function returns  #AUD_ERR_RESOURCE_BUSY, it means that the driver
/// is busy with an other audio command.
///
/// @param itf AUD interface on which to stop the tone.
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_ToneStop(CONST SND_ITF_T itf)
{
    CONST AUD_ITF_CFG_T*    audioCfg    = tgt_GetAudConfig();
    AUD_ERR_T               errStatus   = AUD_ERR_NO;

    AUD_ASSERT(itf<SND_ITF_QTY, "This Audio interface does not"
               "Exist on this target");

    AUD_TRACE(AUD_INFO_TRC, 0, "aud_ToneStop itf: %d", itf);

    UINT8 userId = aud_MutexTake();

    if (g_audEnabledToneItf != SND_ITF_NONE)
    {
        if (g_audEnabledToneItf    == itf)
        {
            if (audioCfg[itf].tone)
            {
                // Only the start fields needs to be specified,
                // set to FALSE to stop the tone generation.
                errStatus = (audioCfg[itf].tone(itf, 0, NULL, FALSE));
                if (errStatus == AUD_ERR_NO)
                {
                    // Stop the tone
                    // Set the new used interface
                    g_audEnabledToneItf = SND_ITF_NONE;
                }
            }
            else
            {
                errStatus = AUD_ERR_NO_ITF;
            }
        }
        else
        {
            // Another AUD interface is activated
            AUD_TRACE(AUD_INFO_TRC, 0,
                      "AUD: " __FILE__ ", line:%d, Resource Busy, itf:%d, used itf:%d",__LINE__, itf, g_audEnabledToneItf);
            errStatus = AUD_ERR_RESOURCE_BUSY;
        }
    }
    else
    {
        // Nothing to stop, not illegal (useless, though)
        errStatus = AUD_ERR_NO;
    }

    aud_MutexRelease(userId);
    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);
    return errStatus;
}


// =============================================================================
// aud_TonePause
// -----------------------------------------------------------------------------
/// This function pauses or resume a tone.
/// If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param itf Interface on which to stop the playing.
/// @param pause If \c TRUE, pauses the tone. If \c FALSE, resumes a paused
/// tone.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_TonePause(CONST SND_ITF_T itf, CONST BOOL pause)
{
    CONST AUD_ITF_CFG_T*    audioCfg    = tgt_GetAudConfig();
    AUD_ERR_T               errStatus   = AUD_ERR_NO;

    AUD_ASSERT(itf<SND_ITF_QTY, "This Audio interface does not"
               "Exist on this target");

    UINT8 userId = aud_MutexTake();

    if (g_audEnabledToneItf != SND_ITF_NONE)
    {
        if (g_audEnabledToneItf    == itf)
        {
            // Pause the tone
            if (audioCfg[itf].tonePause)
            {
                errStatus = (audioCfg[itf].tonePause(itf, pause));
            }
            else
            {
                errStatus = AUD_ERR_NO_ITF;
            }
        }
        else
        {
            // Another AUD interface is activated
            AUD_TRACE(AUD_INFO_TRC, 0,
                      "AUD: " __FILE__ ", line:%d, Resource Busy, itf:%d, used itf:%d",__LINE__, itf, g_audEnabledToneItf);
            errStatus = AUD_ERR_RESOURCE_BUSY;
        }
    }
    else
    {
        // Nothing to pause, but not illegal (useless though)
        errStatus = AUD_ERR_NO;
    }

    aud_MutexRelease(userId);
    return errStatus;
}


// =============================================================================
// aud_TestModeSetup
// -----------------------------------------------------------------------------
/// Enable a test mode.
/// This function enables a test mode, enabling for example the side test
/// or the DAIs.
/// The audio interface must be enabled when this function is called.
/// This function disables the AIF (audio interface), sets the audio
/// interface in the mode specified, and re-enables the AIF.
/// See the <B> Application Note 0018 </B> for more details on the DAI
/// functionality and see 3GPP TS 44014-500 page 40 for the description of the
/// DAI test interface.
///
/// @param itf AUD interface to test.
/// @param stream Depending on the test, this parameter can be ignored, or
/// used to configure it. (This is the case for the side test). This
/// sets things relative to the streaming (sampling rate, etc)
/// @param cfg Depending on the test, this parameter can be ignored, or
/// used to configure it. (This is the case for the side test). Configure
/// the audio interface.
/// @param mode Test mode to set.
/// @return #AUD_ERR_NO if it succeeds.
// =============================================================================
PUBLIC AUD_ERR_T aud_TestModeSetup(CONST SND_ITF_T         itf,
                                   CONST HAL_AIF_STREAM_T* stream,
                                   CONST AUD_LEVEL_T *     cfg,
                                   AUD_TEST_T              mode)
{
    // FIXME How to protect that state machine (It is one, nope ?)
    switch (mode)
    {
        case AUD_TEST_SIDE_TEST:
        case AUD_TEST_RECVMIC_IN_EARPIECE_OUT:
        {
            if (g_audRunningTest != AUD_TEST_NO)
            {
                return AUD_ERR_RESOURCE_BUSY;
            }

            if (mode == AUD_TEST_RECVMIC_IN_EARPIECE_OUT &&
                    itf != SND_ITF_EAR_PIECE)
            {
                return AUD_ERR_BAD_PARAMETER;
            }

            // Play 0 to add side test on it.
            UINT32 i;
            UINT32 len;

            if (mode == AUD_TEST_SIDE_TEST)
            {
                len = AUD_SIDE_TEST_BUF_LENGTH;
            }
            else // AUD_TEST_RECVMIC_IN_EARPIECE_OUT
            {
                len = AUD_RECVMIC_IN_EARPIECE_OUT_BUF_LENGTH;
            }

            if (g_audTestModeRecordBuffer == NULL &&
                    g_audTestModeMallocFuncPtr != NULL)
            {
                g_audTestModeRecordBuffer = (*g_audTestModeMallocFuncPtr)(len);
            }
            if (g_audTestModeRecordBuffer == NULL)
            {
                return AUD_ERR_UNKNOWN;
            }

            for (i=0 ; i<len; i++)
            {
                // Clear test buffer.
                g_audTestModeRecordBuffer[i] = 0;
            }

            if (mode == AUD_TEST_RECVMIC_IN_EARPIECE_OUT)
            {
                hal_AudForceReceiverMicSelection(TRUE);
                // Set max earpiece digital gain flag
                g_audSetHeadMaxDigitalGain = TRUE;
                // Set max MIC gain flag
                g_audSetMicMaxGain= TRUE;
            }

            // Copy configuration from parameter,
            // but play null buffer.
            HAL_AIF_STREAM_T sideTestStream = *stream;
            sideTestStream.startAddress = (UINT32*)g_audTestModeRecordBuffer;
            sideTestStream.length       = len;

            while (aud_StreamStart(itf, &sideTestStream, cfg) != AUD_ERR_NO)
            {
                sxr_Sleep(1 MS_WAITING);
            }

            if (mode == AUD_TEST_SIDE_TEST)
            {
                sxr_Sleep(200 MS_WAITING);
                // BUFFER_LEN_IN_TIME = 1000 * len / (8K * 2) ms
                // The sound is replayed after:
                // [ BUFFER_LEN_IN_TIME - 200 ] % BUFFER_LEN_IN_TIME ms
            }

            while (aud_StreamRecord(itf, &sideTestStream, cfg) != AUD_ERR_NO)
            {
                sxr_Sleep(1 MS_WAITING);
            }

            g_audRunningTest = mode;
            break;
        }

        case AUD_TEST_NO:
            switch (g_audRunningTest)
            {
                case AUD_TEST_SIDE_TEST:
                case AUD_TEST_RECVMIC_IN_EARPIECE_OUT:
                    if (g_audRunningTest == AUD_TEST_RECVMIC_IN_EARPIECE_OUT)
                    {
                        hal_AudForceReceiverMicSelection(FALSE);
                        // Reset max earpiece digital gain flag
                        g_audSetHeadMaxDigitalGain = FALSE;
                        // Reset max MIC gain flag
                        g_audSetMicMaxGain= FALSE;
                    }

                    while( aud_StreamStop(itf) != AUD_ERR_NO)
                    {
                        sxr_Sleep(1 MS_WAITING);
                    }

                    if (g_audTestModeRecordBuffer != NULL &&
                            g_audTestModeFreeFuncPtr != NULL)
                    {
                        (*g_audTestModeFreeFuncPtr)(g_audTestModeRecordBuffer);
                        g_audTestModeRecordBuffer = NULL;
                    }

                    g_audRunningTest = AUD_TEST_NO;
                    break;

                default:
                    break;
            }

        default:
            break;
    }

    return AUD_ERR_NO;
}


//take care the audRecordBuffer length > stream->len
PUBLIC AUD_ERR_T aud_TestModeSetupExt(CONST SND_ITF_T         itf,
                                      CONST HAL_AIF_STREAM_T* stream,
                                      CONST AUD_LEVEL_T *     cfg,
                                      UINT8 *audRecordBuffer)
{
    UINT32 len=stream->length;
    HAL_AIF_STREAM_T RecordStream = *stream;


    if ((audRecordBuffer == NULL) || (len == 0))
    {
        return AUD_ERR_UNKNOWN;
    }

    // Clear test buffer.
    memset(audRecordBuffer, 0,len);

    RecordStream.startAddress = (UINT32*)audRecordBuffer;
    RecordStream.length       = len;

    while (aud_StreamStart(itf, stream, cfg) != AUD_ERR_NO)
    {
        sxr_Sleep(1 MS_WAITING);
    }

    while (aud_StreamRecord(itf, &RecordStream, cfg) != AUD_ERR_NO)
    {
        sxr_Sleep(1 MS_WAITING);
    }

    return AUD_ERR_NO;
}

PUBLIC AUD_ERR_T aud_TestModeStopExt(CONST SND_ITF_T     itf)
{

    while( aud_StreamStop(itf) != AUD_ERR_NO)
    {
        sxr_Sleep(1 MS_WAITING);
    }

    return AUD_ERR_NO;
}
// =============================================================================
// aud_CalibUpdateValues
// -----------------------------------------------------------------------------
/// Update values depending on calibration parameters.
///
/// @param itf AUD interface on which to stop the tone.
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_CalibUpdateValues(CONST SND_ITF_T itf)
{
    CONST AUD_ITF_CFG_T*    audioCfg    = tgt_GetAudConfig();
    AUD_ERR_T               errStatus   = AUD_ERR_NO;

    AUD_ASSERT(itf<SND_ITF_QTY, "This Audio interface does not"
               "Exist on this target");

    UINT8 userId = aud_MutexTake();

    if (((g_audEnabledRecordItf == SND_ITF_NONE)    && (g_audEnabledPlayItf == SND_ITF_NONE))
            || ((g_audEnabledRecordItf == SND_ITF_NONE)    && (g_audEnabledPlayItf == itf ))
            || ((g_audEnabledRecordItf == itf)             && (g_audEnabledPlayItf == SND_ITF_NONE ))
            || ((g_audEnabledRecordItf == itf)             && (g_audEnabledPlayItf == itf )))
    {
        if (audioCfg[itf].calibUpdate)
        {
            errStatus = (audioCfg[itf].calibUpdate(itf));
        }
        else
        {
            errStatus = AUD_ERR_NO_ITF;
        }
    }
    else
    {
        // Another interface is activated, don't mess with it
        AUD_TRACE(AUD_INFO_TRC, 0,
                  "AUD: " __FILE__ ", line:%d, Resource Busy, itf:%d, used itf:%d",__LINE__, itf, g_audEnabledPlayItf);
        errStatus = AUD_ERR_RESOURCE_BUSY;
    }

    aud_MutexRelease(userId);
    return errStatus;
}


PUBLIC VOID aud_ForceReceiverMicSelection(BOOL on)
{
    UINT8 userId = aud_MutexTake();

    hal_AudForceReceiverMicSelection(on);

    aud_MutexRelease(userId);
}

PUBLIC BOOL aud_GetForceReceiverMicSelectionFlag(VOID)
{
    return aud_CodecCommonGetForceReceiverMicSelectionFlag();
}


PUBLIC VOID aud_SetCurOutputDevice(SND_ITF_T itf)
{
    if (itf == SND_ITF_NONE || SND_ITF_HAS_PHY_SPK(itf))
    {
        g_audCurOutputDevice = itf;
    }
#if defined(BT_A2DP_ANA_DAC_MUTE)
    else
        g_audCurOutputDevice = itf;
#endif
}


PUBLIC SND_ITF_T aud_GetCurOutputDevice(VOID)
{
    return g_audCurOutputDevice;
}


PUBLIC VOID aud_LoudspeakerWithEarpiece(BOOL on)
{
    g_audLoudspeakerWithEarpiece = on;
}


PUBLIC BOOL aud_TestModeRegisterMallocFreeFunc(AUD_TEST_MODE_MALLOC_FUNC_T mallocFunc,
        AUD_TEST_MODE_FREE_FUNC_T freeFunc)
{
    if (g_audTestModeMallocFuncPtr == NULL &&
            g_audTestModeFreeFuncPtr == NULL)
    {
        g_audTestModeMallocFuncPtr = mallocFunc;
        g_audTestModeFreeFuncPtr = freeFunc;
        return TRUE;
    }

    return FALSE;
}


#ifdef VMIC_POWER_ON_WORKAROUND
// =============================================================================
// aud_InitEarpieceSetting
// -----------------------------------------------------------------------------
/// Configure audio components for earpiece detection and earpiece key detection
/// at system initialization time when OS is NOT ready yet.
/// The function is called by PMD only.
///
/// @param on TRUE if is about to enable, FALSE otherwise.
/// @return None.
// =============================================================================
PUBLIC VOID aud_InitEarpieceSetting(VOID)
{
    extern BOOL hal_AudEarpieceDetectInit(VOID);
    hal_AudEarpieceDetectInit();
}

// =============================================================================
// aud_EnableEarpieceSetting
// -----------------------------------------------------------------------------
/// Configure audio components for earpiece detection and earpiece key detection
/// after OS is ready.
/// The function is called by PMD only.
///
/// @param on TRUE if is about to enable, FALSE otherwise.
/// @return None.
// =============================================================================
PUBLIC VOID aud_EnableEarpieceSetting(BOOL enable)
{
    BOOL result;

    UINT8 userId = aud_MutexTake();

    if (enable)
    {
        result = hal_AudOpen(HAL_AUD_USER_EARPIECE, NULL);
    }
    else
    {
        result = hal_AudClose(HAL_AUD_USER_EARPIECE);
    }

    if (!result)
    {
        AUD_ASSERT(FALSE, "aud_EnableEarpieceSetting(%d) failed", enable);
    }

    aud_MutexRelease(userId);
}

#ifdef ABB_HP_DETECT
// =============================================================================
// aud_FmEnableHpDetect
// -----------------------------------------------------------------------------
/// Enable or disable headphone detection when FM is changing frequency.
/// The function is called by FMD only.
///
/// @param on TRUE if is about to enable, FALSE otherwise.
/// @return None.
// =============================================================================
PUBLIC VOID aud_FmEnableHpDetect(BOOL enable)
{
    UINT8 userId = aud_MutexTake();

    hal_AudEnableHpDetect(enable, HP_DETECT_REQ_SRC_FM);

    aud_MutexRelease(userId);
}
#endif // ABB_HP_DETECT
#endif // VMIC_POWER_ON_WORKAROUND


#ifdef AUD_MUTE_FOR_BT_WORKAROUND
// =============================================================================
// aud_MuteOutputDevice
// -----------------------------------------------------------------------------
/// Power on/off the audio when it is playing.
/// The function is called by MCI only to redirect the audio output to the bluetooth device.
/// The function might be removed if MCI changes its way to switch between bluetooth and normal codec.
///
/// @param on TRUE if power on, FALSE otherwise.
/// @return None.
// =============================================================================
PUBLIC VOID aud_MuteOutputDevice(BOOL mute)
{
    UINT8 userId = aud_MutexTake();

    extern VOID hal_AudMuteOutput(BOOL);
    hal_AudMuteOutput(mute);

    aud_MutexRelease(userId);
}
#endif // AUD_MUTE_FOR_BT_WORKAROUND


#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD) || defined(AUD_EXT_PA_ON_WITH_LCD)
// =============================================================================
// aud_LcdPowerOnCallback
// -----------------------------------------------------------------------------
/// Power on some audio components along with LCD.
/// The function is called by LCDD only.
///
/// @param on TRUE if power on, FALSE otherwise.
/// @return None.
// =============================================================================
PUBLIC VOID aud_LcdPowerOnCallback(VOID)
{
    UINT8 userId = aud_MutexTake();

    extern VOID hal_AudLcdPowerOnCallback(VOID);
    hal_AudLcdPowerOnCallback();

    aud_MutexRelease(userId);
}

// =============================================================================
// aud_LcdPowerOffCallback
// -----------------------------------------------------------------------------
/// Restore the power down state for some audio components.
/// The function is called by LCDD only.
///
/// @param on TRUE if power on, FALSE otherwise.
/// @return None.
// =============================================================================
PUBLIC VOID aud_LcdPowerOffCallback(VOID)
{
    UINT8 userId = aud_MutexTake();

    extern VOID hal_AudLcdPowerOffCallback(VOID);
    hal_AudLcdPowerOffCallback();

    aud_MutexRelease(userId);
}
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD || AUD_EXT_PA_ON_WITH_LCD


#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
#ifdef AUD_3_IN_1_SPK
// =============================================================================
// aud_BypassNotchFilterReq
// -----------------------------------------------------------------------------
/// Request to bypass notch filter.
///
/// @param on TRUE if is about to bypass, FALSE otherwise.
/// @return None.
// =============================================================================
PUBLIC VOID aud_BypassNotchFilterReq(BOOL bypass)
{
    UINT8 userId = aud_MutexTake();

    hal_AudBypassNotchFilterReq(bypass);

    aud_MutexRelease(userId);
}
#endif // AUD_3_IN_1_SPK

#endif // 8809 or later


///  @} <- End of the audio group

// =============================================================================
// aud_SetMicMuteFlag
// -----------------------------------------------------------------------------
/// aud_SetMicMuteFlag.
// =============================================================================
#ifndef CHIP_HAS_AP
PUBLIC VOID aud_SetMicMuteFlag(BOOL muteflag)
{
    isMICMute = muteflag;
}

// =============================================================================
// aud_GetMicMuteFlag
// -----------------------------------------------------------------------------
/// aud_GetMicMuteFlag.
// =============================================================================
PUBLIC BOOL aud_GetMicMuteFlag(VOID)
{
    return isMICMute;
}
#endif

// =============================================================================
// aud_GetOutAlgGainDb2Val
// -----------------------------------------------------------------------------
/// get the audio codec AlgGainDb2Val.
// =============================================================================
PUBLIC INT16 aud_GetOutAlgGainDb2Val(VOID)
{
    return aud_CodecCommonGetOutAlgGainDb2Val();
}

PUBLIC BOOL aud_GetBtNRECFlag(VOID)
{
    return aud_CodecCommonGetBtNRECFlag();
}

#if defined(BT_A2DP_ANA_DAC_MUTE)
// =============================================================================
// aud_SetFMWorkingFlag
// -----------------------------------------------------------------------------
/// aud_SetFMWorkingFlag.
// =============================================================================
PUBLIC VOID aud_SetFMWorkingFlag(BOOL muteflag)
{
    isFMWorking = muteflag;
}

// =============================================================================
// aud_GetFMWorkingFlag
// -----------------------------------------------------------------------------
/// aud_GetFMWorkingFlag.
// =============================================================================
PUBLIC BOOL aud_GetFMWorkingFlag(VOID)
{
    return isFMWorking;
}
#endif

#ifdef CHIP_DIE_8955
PUBLIC VOID aud_EnFMRecNewPath(BOOL en)
{
    aud_FmEnRecNewPath(en);
}
#endif


