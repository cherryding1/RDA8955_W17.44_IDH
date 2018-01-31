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
#include "cfg_regs.h"

#include "halp_debug.h"
#include "halp_sys.h"
#include "hal_sys.h"
#include "hal_clk.h"
#include "hal_rda_audio.h"
#include "halp_abb_regs.h"
#include "halp_rda_abb.h"

#include "sxr_tls.h"
#include "pmd_m.h"
#include "rfd_xcv.h"


#if defined(AUD_SPK_ON_WITH_LCD) && defined(AUD_EXT_PA_ON_WITH_LCD)
#error "AUD_SPK_ON_WITH_LCD and AUD_EXT_PA_ON_WITH_LCD cannot be defined at the same time!"
#endif

// TODO: Must we couple HP detect with VMIC power on ???
#if defined(ABB_HP_DETECT) && !defined(VMIC_POWER_ON_WORKAROUND)
#error "VMIC_POWER_ON_WORKAROUND should be enabled when ABB_HP_DETECT is enabled."
#endif

#ifdef ABB_HP_DETECT
// Whether to detect headphone by ABB on idle time
#define ABB_HP_DETECT_ON_IDLE_TIME 0

#if (ABB_HP_DETECT_ON_IDLE_TIME)
extern VOID hal_AudMuteHeadphoneDac(BOOL mute);
#endif // ABB_HP_DETECT_ON_IDLE_TIME
#endif


PRIVATE HAL_AUD_USER_T g_halAudUser = HAL_AUD_USER_NULL;

#ifdef RECV_USE_SPK_LINE
PRIVATE BOOL g_halAudRecvModeEnabled = FALSE;
#endif

PRIVATE HAL_AUD_SEL_T g_halAudOrgSelCfg;

typedef enum
{
    HAL_AUD_FREQ_REQ_SRC_AUD_OPEN,
    HAL_AUD_FREQ_REQ_SRC_NORMAL,
    HAL_AUD_FREQ_REQ_SRC_LCD_OFF,
} HAL_AUD_FREQ_REQ_SRC_T;
PRIVATE UINT32 g_halAudFreqReqSrcBitmap = 0;


#ifdef VMIC_POWER_ON_WORKAROUND
typedef enum
{
    HAL_AUD_EP_DETECT_OFF,
    HAL_AUD_EP_DETECT_ON,
} HAL_AUD_EP_DETECT_T;
PRIVATE HAL_AUD_EP_DETECT_T g_halAudEarpieceDetectionOn = HAL_AUD_EP_DETECT_OFF;

PRIVATE BOOL g_halAudVMicValid = FALSE;
#endif // VMIC_POWER_ON_WORKAROUND


#ifdef AUD_MUTE_FOR_BT_WORKAROUND
PRIVATE BOOL g_halAudMuteOutput = FALSE;
#endif // AUD_MUTE_FOR_BT_WORKAROUND


#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD)
typedef enum
{
    HAL_AUD_DAC_PREV_PWR_OFF,
    HAL_AUD_DAC_PREV_PWR_ON,
    HAL_AUD_DAC_PREV_PWR_RESTORED,
} HAL_AUD_DAC_PREV_PWR_T;
PRIVATE HAL_AUD_DAC_PREV_PWR_T g_halAudDacPrevPwr = HAL_AUD_DAC_PREV_PWR_OFF;

#ifdef RECV_USE_SPK_LINE
PUBLIC BOOL g_halAudRecvUseSpkLineRight = FALSE;
#endif
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD


#ifdef AUD_SPK_ON_WITH_LCD
PRIVATE BOOL g_halAudExtPaConfigNeeded = FALSE;
#endif
#ifdef AUD_EXT_PA_ON_WITH_LCD
typedef enum
{
    HAL_AUD_EXT_PA_PREV_PWR_OFF,
    HAL_AUD_EXT_PA_PREV_PWR_ON,
    HAL_AUD_EXT_PA_PREV_PWR_RESTORED,
} HAL_AUD_EXT_PA_PREV_PWR_T;
PRIVATE HAL_AUD_EXT_PA_PREV_PWR_T g_halAudExtPaPrevPwr = HAL_AUD_EXT_PA_PREV_PWR_OFF;
PRIVATE UINT32 g_halAudExtPaStableTime = (350 MS_WAITING);
#endif
#if defined(AUD_SPK_ON_WITH_LCD) || defined(AUD_EXT_PA_ON_WITH_LCD)
PRIVATE UINT32 g_halAudIntSpkPaStableTime = (20 MS_WAITING);
#endif

/// Whether to always select the MIC on phone (the regular one)
PRIVATE BOOL g_halAudForceReceiverMicSelection = FALSE;
PRIVATE BOOL g_hal_isEarphonePlugged = FALSE;

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
PUBLIC BOOL g_halAudLineInSpeakerInUse = FALSE;
#endif

#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
#ifdef ABB_HP_DETECT
PRIVATE BOOL g_halAudHeadphonePrevState = FALSE;
#endif // ABB_HP_DETECT
#endif // 8809 or later

PUBLIC BOOL hal_SetHandsetInPhone(BOOL plug)
{
    g_hal_isEarphonePlugged = plug;
}

// ===========================================================

#ifdef RECV_USE_SPK_LINE
PUBLIC VOID hal_AudEnableRecvMode(BOOL enable)
{
    g_halAudRecvModeEnabled = enable;
}
#endif

static HAL_CLK_T *gAudioClk = NULL;
PRIVATE VOID hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_T src, BOOL on)
{
    if (on)
    {
        if (g_halAudFreqReqSrcBitmap == 0)
        {
            hal_ClkEnable(gAudioClk);
        }
        g_halAudFreqReqSrcBitmap |= (1<<src);
    }
    else
    {
        g_halAudFreqReqSrcBitmap &= ~(1<<src);
        if (g_halAudFreqReqSrcBitmap == 0)
        {
            hal_ClkDisable(gAudioClk);
        }
    }
}

PRIVATE VOID hal_AudMuteAllChannels(VOID)
{
    // Mute all channels
    hal_AudSetRcvGain(0);
    hal_AudSetHeadGain(0);
    HAL_AUD_SPK_CFG_T spkCfg;
    spkCfg.reg = 0;
    hal_AudSetSpkCfg(spkCfg);
    HAL_AUD_MIC_CFG_T micCfg;
    micCfg.reg = 0;
    hal_AudSetMicCfg(micCfg);
}

#ifdef VMIC_POWER_ON_WORKAROUND
PUBLIC BOOL hal_AudEarpieceDetectInit(VOID)
{
    // Power on audio codec components
    HAL_AUD_PD_T pdStatus, orgPdStatus;
    orgPdStatus = hal_AudGetPowerStatus();
    if (!orgPdStatus.noDeepPd)
    {
        pdStatus.reg = 0;
        pdStatus.noDeepPd = TRUE;
        hal_AudPowerOn(pdStatus);
        hal_TimDelay(AUD_TIME_POWER_ON_DEEP);
    }
    if (!orgPdStatus.noRefPd)
    {
        pdStatus.reg = 0;
        pdStatus.noRefPd = TRUE;
        hal_AudPowerOn(pdStatus);
        hal_TimDelay(AUD_TIME_POWER_ON_REF);
    }

    // Power on both Mic and AuxMic, then no matter which one
    // is selected in audio_sel_cfg, v_mic will be turned on.
    if (!orgPdStatus.noMicPd)
    {
        pdStatus.reg = 0;
        pdStatus.noMicPd = TRUE;
        hal_AudPowerOn(pdStatus);
        hal_TimDelay(AUD_TIME_POWER_ON_OTHER);
    }
    if (!orgPdStatus.noAuxMicPd)
    {
        pdStatus.reg = 0;
        pdStatus.noAuxMicPd = TRUE;
        hal_AudPowerOn(pdStatus);
        hal_TimDelay(AUD_TIME_POWER_ON_OTHER);
    }
    if (!orgPdStatus.noAdcPd)
    {
        pdStatus.reg = 0;
        pdStatus.noAdcPd = TRUE;
        hal_AudPowerOn(pdStatus);
    }

#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
    if (!orgPdStatus.noDacPd)
    {
        pdStatus.reg = 0;
        pdStatus.noDacPd = TRUE;
        hal_AudPowerOn(pdStatus);
        sxr_Sleep(AUD_TIME_POWER_ON_OTHER);
    }
    if (!orgPdStatus.noDacReset)
    {
        pdStatus.reg = 0;
        pdStatus.noDacReset = TRUE;
        hal_AudPowerOn(pdStatus);
    }
#endif

    // Set g_halAudEarpieceDetectionOn first, which will be checked
    // in hal_AudSetSelCfg
    g_halAudEarpieceDetectionOn = HAL_AUD_EP_DETECT_ON;

    // Proceed with some special earpiece handlings
    hal_AudSetSelCfg(hal_AudGetSelCfg());

#if ! ( (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) && defined(MIC_CAPLESS_MODE) )
    g_halAudVMicValid = TRUE;
#endif

#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
    // Start headphone plugging-out detection
    hal_AudEnableHpDetect(TRUE, HP_DETECT_REQ_SRC_AUD);
#endif

    // No need to request freq for earpiece

    return TRUE;
}

PUBLIC BOOL hal_AudVMicValid(VOID)
{
    return g_halAudVMicValid;
}
#endif // VMIC_POWER_ON_WORKAROUND


#ifdef AUD_MUTE_FOR_BT_WORKAROUND
// =============================================================================
// hal_AudMuteOutput
// -----------------------------------------------------------------------------
/// Power on/off the audio when it is playing.
/// The function is called by MCI only to redirect the audio output to the bluetooth device.
/// The function might be removed if MCI changes its way to switch between bluetooth and normal codec.
///
/// @param on TRUE if power on, FALSE otherwise.
/// @return None.
// =============================================================================
PUBLIC VOID hal_AudMuteOutput(BOOL mute)
{
    // Mutex control is done in aud_core.c
    g_halAudMuteOutput = mute;

    if (mute)
    {
        // MCI will mute audio after stopping current stream but before restarting it.
        // So the mute operation is acutally done in hal_AudOpen (also within
        // mutex control).
        hal_AudEnableDac(FALSE);
    }
    else
    {
        // MCI might unmute audio while playing current stream.
        hal_AudEnableDac(TRUE);
    }
}

PUBLIC BOOL hal_AudOutputMuted(VOID)
{
    return g_halAudMuteOutput;
}
#endif // AUD_MUTE_FOR_BT_WORKAROUND

PUBLIC  BOOL hal_AudPreOpen()
{
    HAL_AUD_PD_T pdStatus;

    pmd_EnablePower(PMD_POWER_AUDIO, TRUE);

    pdStatus.reg = hwp_configRegs->audio_pd_set;
    if (!pdStatus.noDeepPd)
    {
        pdStatus.reg = 0;
        pdStatus.noRefPd = TRUE;
        hwp_configRegs->audio_pd_set = pdStatus.reg;
    }

}

PUBLIC BOOL hal_AudOpen(HAL_AUD_USER_T user, HAL_AUD_CALLBACK_T cbFunc)
{
    BOOL openAud = FALSE;

#ifdef VMIC_POWER_ON_WORKAROUND
    if (user == HAL_AUD_USER_EARPIECE)
    {
        if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_ON)
        {
            return TRUE;
        }
        else
        {
            openAud = TRUE;
        }
    }
    else if (g_halAudUser == HAL_AUD_USER_NULL)
    {
        openAud = TRUE;
    }
#else // ! VMIC_POWER_ON_WORKAROUND
    if (g_halAudUser == HAL_AUD_USER_NULL)
    {
        openAud = TRUE;
    }
#endif // ! VMIC_POWER_ON_WORKAROUND

    if (!openAud)
    {
        return FALSE;
    }

    if (user == HAL_AUD_USER_AIF_ONLY)
    {
#ifdef CHIP_HAS_AP
        // Config AIF clock
        if (cbFunc != NULL)
        {
            (*cbFunc)();
        }
#endif
        g_halAudUser = user;
        return TRUE;
    }

    if (user != HAL_AUD_USER_NULL &&
#ifdef VMIC_POWER_ON_WORKAROUND
            user != HAL_AUD_USER_EARPIECE &&
#endif
            user != HAL_AUD_USER_AIF_ONLY)
    {
        pmd_EnablePower(PMD_POWER_AUDIO, TRUE);
        sxr_Sleep(AUD_TIME_POWER_ON_DEEP);
    }

    // Request freq resource (to avoid entering LP mode from now on,
    // including the following sleeps as well)
    hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_AUD_OPEN, TRUE);

#ifdef VMIC_POWER_ON_WORKAROUND
    // Earpiece user needs v_mic power only
    if (user != HAL_AUD_USER_EARPIECE)
#endif // VMIC_POWER_ON_WORKAROUND
    {
        // Mute all channels
        hal_AudMuteAllChannels();

        // Disable codec DSP to start DSP configuration
        hal_AudEnableCodecDsp(FALSE);

        // Set FM mode
        hal_AudSetFmMode(user == HAL_AUD_USER_FM);

        // Invoke callback function before power on codec
        if (cbFunc != NULL)
        {
            (*cbFunc)();
        }

        hal_AudEnableCodecDsp(TRUE);
    }

    // Power on audio codec components
    HAL_AUD_PD_T pdStatus, orgPdStatus;
    orgPdStatus = hal_AudGetPowerStatus();
    if (!orgPdStatus.noDeepPd)
    {
        pdStatus.reg = 0;
        pdStatus.noDeepPd = TRUE;
        hal_AudPowerOn(pdStatus);
        sxr_Sleep(AUD_TIME_POWER_ON_DEEP);
    }
    if (!orgPdStatus.noRefPd)
    {
        pdStatus.reg = 0;
        pdStatus.noRefPd = TRUE;
        hal_AudPowerOn(pdStatus);
        sxr_Sleep(AUD_TIME_POWER_ON_REF);
    }

    if ( user == HAL_AUD_USER_CODEC
            || user == HAL_AUD_USER_LINEIN_WITH_CODEC
#ifdef VMIC_POWER_ON_WORKAROUND
            || user == HAL_AUD_USER_EARPIECE
#endif
#if defined(FM_LINEIN_RECORD) || (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
            || user == HAL_AUD_USER_LINEIN
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
            || user == HAL_AUD_USER_FM
#endif
       )
    {
        // Power on both Mic and AuxMic, then no matter which one
        // is selected in audio_sel_cfg, v_mic will be turned on.
        if (!orgPdStatus.noMicPd)
        {
            pdStatus.reg = 0;
            pdStatus.noMicPd = TRUE;
            hal_AudPowerOn(pdStatus);
            sxr_Sleep(AUD_TIME_POWER_ON_OTHER);
        }
        if (!orgPdStatus.noAuxMicPd)
        {
            pdStatus.reg = 0;
            pdStatus.noAuxMicPd = TRUE;
            hal_AudPowerOn(pdStatus);
            sxr_Sleep(AUD_TIME_POWER_ON_OTHER);
        }
        if (!orgPdStatus.noAdcPd)
        {
            pdStatus.reg = 0;
            pdStatus.noAdcPd = TRUE;
            hal_AudPowerOn(pdStatus);
            sxr_Sleep(AUD_TIME_POWER_ON_OTHER);
        }
    }

    // DAC should be powered on even if the user is earpiece, in
    // case that ABB HP detection is enabled
#ifdef VMIC_POWER_ON_WORKAROUND
#if !(defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME))
    if (user != HAL_AUD_USER_EARPIECE)
#endif
#endif // VMIC_POWER_ON_WORKAROUND
    {
        if (!orgPdStatus.noDacPd)
        {
            pdStatus.reg = 0;
            pdStatus.noDacPd = TRUE;
            hal_AudPowerOn(pdStatus);
            sxr_Sleep(AUD_TIME_POWER_ON_OTHER);
        }
        if (!orgPdStatus.noDacReset)
        {
            pdStatus.reg = 0;
            pdStatus.noDacReset = TRUE;
            hal_AudPowerOn(pdStatus);
        }
    }

#ifdef VMIC_POWER_ON_WORKAROUND
    if (user == HAL_AUD_USER_EARPIECE)
    {
        // Set g_halAudEarpieceDetectionOn first, which will be checked
        // in hal_AudSetSelCfg
        g_halAudEarpieceDetectionOn = HAL_AUD_EP_DETECT_ON;

        // Proceed with some special earpiece handlings
        hal_AudSetSelCfg(hal_AudGetSelCfg());

#if ! ( (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) && defined(MIC_CAPLESS_MODE) )
        g_halAudVMicValid = TRUE;
#endif

#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
        // Start headphone plugging-out detection
        hal_AudEnableHpDetect(TRUE, HP_DETECT_REQ_SRC_AUD);
#endif

        // No need to request freq for earpiece
        hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_AUD_OPEN, FALSE);

        return TRUE;
    }
#endif // VMIC_POWER_ON_WORKAROUND

    // Allow deep sleep if in FM/LINEIN mode
    // (except for 8809 LINEIN -- see hal_AudSetSelCfg)
    if (user != HAL_AUD_USER_FM && user != HAL_AUD_USER_LINEIN)
    {
        hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_NORMAL, TRUE);
    }
    hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_AUD_OPEN, FALSE);

    // Request audio module (no need to enter critical section)
    g_halAudUser = user;

    return TRUE;
}

PUBLIC BOOL hal_AudClose(HAL_AUD_USER_T user)
{
    BOOL closeAud = FALSE;

#ifdef VMIC_POWER_ON_WORKAROUND
    if (user == HAL_AUD_USER_EARPIECE)
    {
        if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_OFF)
        {
            return TRUE;
        }
        else
        {
            if (g_halAudUser == HAL_AUD_USER_NULL)
            {
                closeAud = TRUE;
            }
            else
            {
                // There is another audio user, so just set the flag and do not close the module
                g_halAudEarpieceDetectionOn = HAL_AUD_EP_DETECT_OFF;
                g_halAudVMicValid = FALSE;
#ifdef ABB_HP_DETECT
                // Codec must be reset if in FM mode
                if (g_halAudUser == HAL_AUD_USER_FM)
                {
                    hal_AudEnableCodecDsp(FALSE);
                }
                // Disable headphone detection
                hal_AudEnableHpDetect(FALSE, HP_DETECT_REQ_SRC_AUD);
                if (g_halAudUser == HAL_AUD_USER_FM)
                {
                    hal_AudEnableCodecDsp(TRUE);
                }
                {
                    // If the headphone key detection disabling is triggered
                    // by LP mode, the headphone state must be restored; otherwise
                    // (triggered by plugging out) it can be safely reset to unselected.
                    // Among the audio users, only FM and LINEIN can enter
                    // LP mode.
                    HAL_AUD_SEL_T selCfg;
                    selCfg = hal_AudGetSelCfg();
                    if (g_halAudUser == HAL_AUD_USER_FM ||
                            g_halAudUser == HAL_AUD_USER_LINEIN)
                    {
                        selCfg.headSel = g_halAudHeadphonePrevState;
                    }
                    else
                    {
                        selCfg.headSel = FALSE;
                    }


                    hal_AudSetSelCfg(selCfg);
                }
#endif
#ifdef USE_EP_MIC_WITH_SPK
                {
                    // Not to select aux MIC any longer (after resetting
                    // g_halAudEarpieceDetectionOn)
                    HAL_AUD_SEL_T selCfg;
                    selCfg = hal_AudGetSelCfg();
                    selCfg.auxMicSel = FALSE;
                    hal_AudSetSelCfg(selCfg);
                }
#endif
                return TRUE;
            }
            // Else audio driver is being configured by a user other than earpiece, and we need to wait
        }
        // Else audio driver is being configured by earpiece, and we need to wait
    }
    else if (g_halAudUser == user)
    {
        closeAud = TRUE;
    }
    else
    {
        // 1. Audio driver is already closed
        // 2. Audio driver has been opened by another user (not earpiece either)
        return TRUE;
    }
#else // ! VMIC_POWER_ON_WORKAROUND
    if (g_halAudUser == user)
    {
        closeAud = TRUE;
    }
    else
    {
        // 1. Audio driver is already closed
        // 2. Audio driver has been opened by another user
        return TRUE;
    }
#endif // ! VMIC_POWER_ON_WORKAROUND

    if (!closeAud)
    {
        return FALSE;
    }

#ifdef ABB_HP_DETECT
    g_halAudHeadphonePrevState = FALSE;
#endif

    if (user == HAL_AUD_USER_AIF_ONLY)
    {
        g_halAudUser = HAL_AUD_USER_NULL;
        return TRUE;
    }

#ifdef VMIC_POWER_ON_WORKAROUND
    if (user == HAL_AUD_USER_EARPIECE)
    {
        // Reset earpiece flag before calling hal_AudSetSelCfg and hal_AudPowerDown
        g_halAudEarpieceDetectionOn = HAL_AUD_EP_DETECT_OFF;
        g_halAudVMicValid = FALSE;
#ifdef ABB_HP_DETECT
        // Disable headphone detection before powering down DAC
        hal_AudEnableHpDetect(FALSE, HP_DETECT_REQ_SRC_AUD);
#endif
    }
    else
#endif // VMIC_POWER_ON_WORKAROUND
    {
        // Let the sound fade out naturally
        sxr_Sleep(6 MS_WAITING);
    }

    // Mute all channels
    hal_AudMuteAllChannels();

    // Deselect all modules
    HAL_AUD_SEL_T selCfg;
    selCfg.reg = 0;

    hal_AudSetSelCfg(selCfg);

    // Power down finally
    HAL_AUD_PD_T pdStatus;
    pdStatus.reg = HAL_AUD_PD_MASK;
    hal_AudPowerDown(pdStatus);

#ifdef VMIC_POWER_ON_WORKAROUND
    if (user == HAL_AUD_USER_EARPIECE)
    {
        return TRUE;
    }
#endif // VMIC_POWER_ON_WORKAROUND

    // Disable codec DSP to start DSP configuration
    hal_AudEnableCodecDsp(FALSE);

    // Clear FM mode
    hal_AudSetFmMode(FALSE);
    if (user == HAL_AUD_USER_FM)
    {
        // Must wait for AUD_TIME_SET_FM_MODE if setting from FM to non-FM mode
        sxr_Sleep(AUD_TIME_SET_FM_MODE);
    }

    // Set default voice mode
    hal_AudSetVoiceMode(FALSE);

    hal_AudEnableCodecDsp(TRUE);

    // Power down audio module
    sxr_Sleep(AUD_TIME_POWER_ON_DEEP);

    if(!g_hal_isEarphonePlugged)
        pmd_EnablePower(PMD_POWER_AUDIO, FALSE);


    // Release freq resource
    hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_NORMAL, FALSE);

    // Release audio module (no need to enter critical section)
    g_halAudUser = HAL_AUD_USER_NULL;

    return TRUE;
}

PUBLIC VOID hal_AudPowerOn(HAL_AUD_PD_T pdStatus)
{
#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD)
    if (pdStatus.noDacPd)
    {
        if (g_halAudDacPrevPwr != HAL_AUD_DAC_PREV_PWR_RESTORED)
        {
            g_halAudDacPrevPwr = HAL_AUD_DAC_PREV_PWR_ON;
        }
    }
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD

    hwp_configRegs->audio_pd_set = pdStatus.reg;

#ifdef AUD_EXT_PA_ON_WITH_LCD
    if (pdStatus.noDacPd)
    {
        if (pmd_AudExtPaExists())
        {
            if (g_halAudExtPaPrevPwr != HAL_AUD_EXT_PA_PREV_PWR_RESTORED)
            {
                g_halAudExtPaPrevPwr = HAL_AUD_EXT_PA_PREV_PWR_ON;
            }
            // Do we need any stable time here?
            pmd_EnablePower(PMD_POWER_LOUD_SPEAKER, TRUE);
        }
    }
#endif // AUD_EXT_PA_ON_WITH_LCD

#if defined( CHIP_DIE_8809E2)  || defined (CHIP_DIE_8955) || defined(CHIP_DIE_8909)
    hal_AudEnableAuxpll(TRUE);
#endif

}

PUBLIC VOID hal_AudPowerDown(HAL_AUD_PD_T pdStatus)
{
#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD)
    if (pdStatus.noDacPd)
    {
        // Audio might be stopped after LCD is turned off. In that case,
        // audio codec DAC can be powered off if requested.
        if (g_halAudDacPrevPwr != HAL_AUD_DAC_PREV_PWR_RESTORED)
        {
            g_halAudDacPrevPwr = HAL_AUD_DAC_PREV_PWR_OFF;
            pdStatus.noDacPd = FALSE;
        }
    }
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD

    // Cannot go into deep sleep here
    pdStatus.noDeepPd = FALSE;
    pdStatus.noRefPd = FALSE;
#ifdef VMIC_POWER_ON_WORKAROUND
    if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_ON)
    {
        // Power on both Mic and AuxMic, then no matter which one
        // is selected in audio_sel_cfg, v_mic will be turned on.
        pdStatus.noMicPd = FALSE;
        pdStatus.noAuxMicPd = FALSE;
        pdStatus.noAdcPd = FALSE;
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
        // Power on DAC to detect headphone plugging-out
        pdStatus.noDacPd = FALSE;
        pdStatus.noDacReset = FALSE;
#endif
    }
#endif // VMIC_POWER_ON_WORKAROUND

    hwp_configRegs->audio_pd_clr = pdStatus.reg;

#ifdef AUD_EXT_PA_ON_WITH_LCD
    if (pdStatus.noDacPd)
    {
        if (pmd_AudExtPaExists())
        {
            // Audio might be stopped after LCD is turned off. In that case,
            // the external PA can be powered off if requested.
            if (g_halAudExtPaPrevPwr == HAL_AUD_EXT_PA_PREV_PWR_RESTORED)
            {
                // We will not go to deep sleep here
                sxr_Sleep(g_halAudIntSpkPaStableTime);
                pmd_EnablePower(PMD_POWER_LOUD_SPEAKER, FALSE);
                sxr_Sleep(g_halAudExtPaStableTime);
            }
            else
            {
                g_halAudExtPaPrevPwr = HAL_AUD_EXT_PA_PREV_PWR_OFF;
            }
        }
    }
#endif // AUD_EXT_PA_ON_WITH_LCD

#if defined( CHIP_DIE_8809E2)  || defined (CHIP_DIE_8955) || defined(CHIP_DIE_8909)
    hal_AudEnableAuxpll(FALSE);
#endif

}

#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD) || defined(AUD_EXT_PA_ON_WITH_LCD)
PUBLIC VOID hal_AudLcdPowerOnCallback(VOID)
{
#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD)
    // Power on audio codec DAC once LCD is turned on
    if (g_halAudDacPrevPwr == HAL_AUD_DAC_PREV_PWR_RESTORED)
    {
        HAL_AUD_PD_T pdStatus;
        pdStatus = hal_AudGetPowerStatus();
        if (pdStatus.noDacPd)
        {
            g_halAudDacPrevPwr = HAL_AUD_DAC_PREV_PWR_ON;
        }
        else
        {
            // Set g_halAudDacPrevPwr before calling hal_AudSetSelCfg(),
            // as hal_AudSetSelCfg() needs to check g_halAudDacPrevPwr
            g_halAudDacPrevPwr = HAL_AUD_DAC_PREV_PWR_OFF;

            HAL_AUD_SEL_T selCfg = hal_AudGetSelCfg();
            // Useless? selCfg.spkSel should have been set to zero
            selCfg.spkSel = FALSE;
            hal_AudSetSelCfg(selCfg);

#ifdef VMIC_POWER_ON_WORKAROUND
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
            if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_OFF)
#endif
#endif // VMIC_POWER_ON_WORKAROUND
            {
                // Power on DAC (and its dependent power)
                pdStatus.reg = 0;
                pdStatus.noDeepPd = TRUE;
                pdStatus.noRefPd = TRUE;
                pdStatus.noDacPd = TRUE;
                hwp_configRegs->audio_pd_set = pdStatus.reg;
            }
        }
    }
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD

#ifdef AUD_EXT_PA_ON_WITH_LCD
    if (pmd_AudExtPaExists())
    {
        if (g_halAudExtPaPrevPwr == HAL_AUD_EXT_PA_PREV_PWR_RESTORED)
        {
            if (g_halAudUser != HAL_AUD_USER_CODEC &&
                    g_halAudUser != HAL_AUD_USER_FM &&
                    g_halAudUser != HAL_AUD_USER_LINEIN &&
                    g_halAudUser != HAL_AUD_USER_LINEIN_WITH_CODEC)
            {
                g_halAudExtPaPrevPwr = HAL_AUD_EXT_PA_PREV_PWR_OFF;
            }
            else
            {
                g_halAudExtPaPrevPwr = HAL_AUD_EXT_PA_PREV_PWR_ON;
            }
        }
        // Do we need any stable time here?
        pmd_EnablePower(PMD_POWER_LOUD_SPEAKER, TRUE);
    }
#endif // AUD_EXT_PA_ON_WITH_LCD
}

PUBLIC VOID hal_AudLcdPowerOffCallback(VOID)
{
#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD)
    HAL_AUD_DAC_PREV_PWR_T orgDacPrevPwr = g_halAudDacPrevPwr;
    // Set g_halAudDacPrevPwr before calling hal_AudSetSelCfg(),
    // as hal_AudSetSelCfg() needs to check g_halAudDacPrevPwr
    g_halAudDacPrevPwr = HAL_AUD_DAC_PREV_PWR_RESTORED;

    // Restore the power state of audio codec DAC, or saying,
    // power off audio codec DAC if possible
    if (orgDacPrevPwr == HAL_AUD_DAC_PREV_PWR_OFF)
    {
        HAL_AUD_SEL_T selCfg = hal_AudGetSelCfg();
        selCfg.spkSel = FALSE;
        hal_AudSetSelCfg(selCfg);

#ifdef VMIC_POWER_ON_WORKAROUND
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
        if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_OFF)
#endif
#endif // VMIC_POWER_ON_WORKAROUND
        {
            // Power off DAC
            HAL_AUD_PD_T pdStatus;
            pdStatus.reg = 0;
            pdStatus.noDacPd = TRUE;
            hwp_configRegs->audio_pd_clr = pdStatus.reg;
        }
    }
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD

#ifdef AUD_EXT_PA_ON_WITH_LCD
    if (pmd_AudExtPaExists())
    {
        if (g_halAudExtPaPrevPwr == HAL_AUD_EXT_PA_PREV_PWR_OFF)
        {
            // Audio module has been requested to shutdown before
            // Forbid deep sleep
            hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_LCD_OFF, TRUE);
            sxr_Sleep(g_halAudIntSpkPaStableTime);
            pmd_EnablePower(PMD_POWER_LOUD_SPEAKER, FALSE);
            sxr_Sleep(g_halAudExtPaStableTime);
            // Allow deep sleep from now on
            hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_LCD_OFF, FALSE);
        }
        g_halAudExtPaPrevPwr = HAL_AUD_EXT_PA_PREV_PWR_RESTORED;
    }
#endif // AUD_EXT_PA_ON_WITH_LCD
}
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD || AUD_EXT_PA_ON_WITH_LCD

PUBLIC VOID hal_AudDeepPowerDown(VOID)
{
    if (g_halAudUser != HAL_AUD_USER_NULL)
    {
        // Not to power down audio if there is an audio user
        // (e.g., in FM/LINEIN mode)
        return;
    }

#ifdef RECV_USE_SPK_LINE
    // pd control should have been released
#endif // RECV_USE_SPK_LINE

#ifdef VMIC_POWER_ON_WORKAROUND
    if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_ON)
    {
        g_halAudVMicValid = FALSE;
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
        // TODO: Should we put these codes into INT-SRAM?
        hal_AudEnableHpDetect(FALSE, HP_DETECT_REQ_SRC_AUD);
#endif
    }
#endif // VMIC_POWER_ON_WORKAROUND

    hwp_configRegs->audio_pd_clr = 0x13d;//HAL_AUD_PD_MASK;
}

PUBLIC VOID hal_AudForcePowerDown(VOID)
{
    // Mute all channels
    hal_AudMuteAllChannels();
    // Deselect all channels to disable PA modules
    hwp_configRegs->audio_sel_cfg = 0;
    // Power down all codec modules except for Deep and Ref
    // (Excluding Deep and Ref is to avoid the pop sound, which would
    // occur if all codec modules were down at the same time.)
    HAL_AUD_PD_T pdStatus;
    pdStatus.reg = HAL_AUD_PD_MASK;
    pdStatus.noDeepPd = FALSE;
    pdStatus.noRefPd = FALSE;
    hwp_configRegs->audio_pd_clr = pdStatus.reg;
}

#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
#ifdef ABB_HP_DETECT
PUBLIC BOOL hal_AudCheckUsrFMOrLineIN(VOID)
{
    if (g_halAudUser == HAL_AUD_USER_FM ||
            g_halAudUser == HAL_AUD_USER_LINEIN)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

PUBLIC VOID hal_AudSaveHeadphoneState(BOOL on)
{
    // Save the headphone state
    g_halAudHeadphonePrevState = on;
}
#endif // ABB_HP_DETECT
#endif // 8809 or later

PUBLIC VOID hal_AudForceReceiverMicSelection(BOOL on)
{
    g_halAudForceReceiverMicSelection = on;
    hal_AudSetSelCfg(hal_AudGetSelCfg());
}

PUBLIC BOOL hal_GetAudForceReceiverMicSelectionFlag(VOID)
{
    return g_halAudForceReceiverMicSelection;
}

PUBLIC VOID hal_AudSetSelCfg(HAL_AUD_SEL_T selCfg)
{
#if defined(AUD_SPK_ON_WITH_LCD) || defined(RECV_USE_SPK_LINE)
    BOOL spkPaForceOn = FALSE;
#endif
#ifdef RECV_USE_SPK_LINE
    BOOL spkLeftChanSel = FALSE;
    BOOL spkRightChanSel = FALSE;
#endif

    // Save the orginal selection
#ifdef RECV_USE_SPK_LINE
    if (selCfg.rcvSel)
    {
        // g_halAudRecvModeEnabled should have been set to TRUE
        selCfg.rcvSel = FALSE;
        selCfg.spkSel = TRUE;
    }
#endif

    g_halAudOrgSelCfg.reg = selCfg.reg;

#ifdef VMIC_POWER_ON_WORKAROUND
    if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_ON)
    {
#ifdef USE_EP_MIC_WITH_SPK
        // NOTE: This setting might be overwritten
        // if receiver mic is forced to select.
        selCfg.auxMicSel = TRUE;
#endif
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
        selCfg.headSel = TRUE;
#endif
    }
#endif // VMIC_POWER_ON_WORKAROUND

    if (g_halAudForceReceiverMicSelection)
    {
        selCfg.auxMicSel = FALSE;
    }

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    if (g_halAudLineInSpeakerInUse)
    {
        if ( ! (g_halAudOrgSelCfg.headSel || g_halAudOrgSelCfg.rcvSel ||
                g_halAudOrgSelCfg.spkSel || g_halAudOrgSelCfg.spkMonoSel) )
        {
            // Never power off speaker when speaker is in use
            // (including mute state) in line-in mode on 8808
            selCfg.spkSel = TRUE;
        }
    }
#endif

#ifdef VMIC_POWER_ON_WORKAROUND
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
    if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_ON &&
            ! (g_halAudOrgSelCfg.headSel || g_halAudOrgSelCfg.rcvSel ||
               g_halAudOrgSelCfg.spkSel || g_halAudOrgSelCfg.spkMonoSel) )
    {
        // Headphone should be muted

        // Flag headphone dac mute first, which will be checked
        // in hal_AudEnableDac
        hal_AudMuteHeadphoneDac(TRUE);
        hal_AudEnableDac(FALSE);
    }
    else
    {
        // Headphone is not forced to mute

        // Flag headphone dac mute first, which will be checked
        // in hal_AudEnableDac
        hal_AudMuteHeadphoneDac(FALSE);
        hal_AudEnableDac(TRUE);
    }
#endif
#endif

#if defined(AUD_SPK_ON_WITH_LCD) && !defined(RECV_USE_SPK_LINE)
    // If the LCD is off, there is no need to select speaker, for it
    // will be selected once LCD is turned on
    if (g_halAudDacPrevPwr != HAL_AUD_DAC_PREV_PWR_RESTORED)
    {
        if ( ! (g_halAudOrgSelCfg.headSel || g_halAudOrgSelCfg.rcvSel ||
                g_halAudOrgSelCfg.spkSel || g_halAudOrgSelCfg.spkMonoSel) )
        {
            // Select speaker output even in idle time, so that the capacitor conntected
            // to the speaker can be charged once LCD is turned on
            spkPaForceOn = TRUE;
            selCfg.spkSel = TRUE;
        }
    }
#elif defined(RECV_USE_SPK_LINE)
    if (g_halAudOrgSelCfg.spkSel || g_halAudOrgSelCfg.spkMonoSel)
    {
        selCfg.spkSel = TRUE;
        selCfg.spkMonoSel = FALSE;

        if (!g_halAudRecvModeEnabled)
        {
            if(pmd_IsSpeakerDouble())
            {
                spkLeftChanSel = TRUE;
                spkRightChanSel = TRUE;
            }
            else
            {
                if (g_halAudRecvUseSpkLineRight)
                {
                    spkLeftChanSel = TRUE;
                }
                else
                {
                    spkRightChanSel = TRUE;
                }
            }
        }
        else // Receiver mode
        {
            if (g_halAudRecvUseSpkLineRight)
            {
                spkRightChanSel = TRUE;
            }
            else
            {
                spkLeftChanSel = TRUE;
            }
        }
    }
    if ( ! (g_halAudOrgSelCfg.headSel || g_halAudOrgSelCfg.rcvSel ||
            g_halAudOrgSelCfg.spkSel || g_halAudOrgSelCfg.spkMonoSel) )
    {
        // If the LCD is off, there is no need to select speaker, for it
        // will be selected once LCD is turned on
        if (g_halAudDacPrevPwr != HAL_AUD_DAC_PREV_PWR_RESTORED)
        {
            // Select speaker output even in idle time, so that the capacitor conntected
            // to the receiver/speaker can be charged once LCD is turned on
            spkPaForceOn = TRUE;
            selCfg.spkSel = TRUE;
            selCfg.spkMonoSel = FALSE;
            selCfg.rcvSel = FALSE;

#ifdef AUD_SPK_ON_WITH_LCD
            spkLeftChanSel = TRUE;
            spkRightChanSel = TRUE;
#else // !AUD_SPK_ON_WITH_LCD
            if (g_halAudRecvUseSpkLineRight)
            {
                spkRightChanSel = TRUE;
            }
            else
            {
                spkLeftChanSel = TRUE;
            }
#endif // !AUD_SPK_ON_WITH_LCD
        }
    }
#endif // RECV_USE_SPK_LINE

    // Headphone PA mode
#if (HEADPHONE_CHARGE_PUMP)
    pmd_HeadphonePaPreEnable(g_halAudOrgSelCfg.headSel);
#endif
#ifdef CHIP_DIE_8809E
    if (g_halAudOrgSelCfg.rcvSel == 1)
    {
        hal_AudEnRcv(TRUE);
    }
    else
    {
        hal_AudEnRcv(FALSE);
    }

#endif

    // Speaker PA power
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
    BOOL spkPaReq;
    if (g_halAudOrgSelCfg.spkSel || g_halAudOrgSelCfg.spkMonoSel)
    {
        spkPaReq = TRUE;
    }
    else
    {
        spkPaReq = FALSE;
    }
    pmd_SpeakerPaPreEnable(spkPaReq);
#endif // 8809 or later

    // Check if vmic will be invalid
#ifdef VMIC_POWER_ON_WORKAROUND
    if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_ON)
    {
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifdef MIC_CAPLESS_MODE
        if (!selCfg.auxMicSel)
        {
            g_halAudVMicValid = FALSE;
        }
#endif
#endif
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME == 0)
        if (!selCfg.headSel)
        {
            hal_AudEnableHpDetect(FALSE, HP_DETECT_REQ_SRC_AUD);
        }
#endif
    }
#endif // VMIC_POWER_ON_WORKAROUND

    // ----------------------------------------
    // Setting audio_sel_cfg register
    // ----------------------------------------
    hwp_configRegs->audio_sel_cfg = selCfg.reg;

    // ----------------------------------------
    // End of setting audio_sel_cfg register
    // ----------------------------------------

    // Check if vmic is valid
#ifdef VMIC_POWER_ON_WORKAROUND
    if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_ON)
    {
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifdef MIC_CAPLESS_MODE
        {
            HAL_AUD_PD_T pdStatus;
            pdStatus = hal_AudGetPowerStatus();
            if (selCfg.auxMicSel &&
            pdStatus.noDeepPd && pdStatus.noRefPd &&
            pdStatus.noAuxMicPd && pdStatus.noAdcPd)
            {
                g_halAudVMicValid = TRUE;
            }
        }
#endif
#endif
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME == 0)
        {
            HAL_AUD_PD_T pdStatus;
            pdStatus = hal_AudGetPowerStatus();
            if (selCfg.headSel &&
            pdStatus.noDeepPd && pdStatus.noRefPd &&
            pdStatus.noDacPd && pdStatus.noDacReset)
            {
                hal_AudEnableHpDetect(TRUE, HP_DETECT_REQ_SRC_AUD);
            }
        }
#endif
    }
#endif // VMIC_POWER_ON_WORKAROUND

    // Speaker PA power
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
    pmd_SpeakerPaPostEnable(spkPaReq);
    if (g_halAudUser == HAL_AUD_USER_LINEIN || g_halAudUser == HAL_AUD_USER_FM)
    {
        // Class D/K speaker needs 26M clock. Internal FM can supply 26M to
        // speaker, but the line-in channel cannot do that. So deep sleep
        // will be disallowed when speaker is on in line-in mode, and it
        // will be allowed when speaker is off in line-in mode.
        hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_NORMAL, spkPaReq);
    }
#endif // 8809 or later

    // Headphone PA mode
#if (HEADPHONE_CHARGE_PUMP)
    pmd_HeadphonePaPostEnable(g_halAudOrgSelCfg.headSel);
#endif

    // Config codec
    HAL_AUD_CODEC_MODE_PARAM_T modeParam;
    modeParam.curUser = g_halAudUser;
    modeParam.orgSelCfg = g_halAudOrgSelCfg;
    modeParam.curSelCfg = selCfg;
#if defined(AUD_SPK_ON_WITH_LCD) || defined(RECV_USE_SPK_LINE)
    modeParam.spkPaForceOn = spkPaForceOn;
#endif
#if defined (CHIP_DIE_8955) || defined(CHIP_DIE_8909)
#ifdef RECV_USE_SPK_LINE
    if (g_halAudRecvModeEnabled)
    {
        modeParam.spkLeftChanSel = TRUE;
        modeParam.spkRightChanSel = FALSE;
    }
    else
    {
        modeParam.spkLeftChanSel = FALSE;
        modeParam.spkRightChanSel = TRUE;
    }
    //modeParam.spkLeftChanSel = spkLeftChanSel;
    //modeParam.spkRightChanSel = spkRightChanSel;
#endif
#else

#ifdef RECV_USE_SPK_LINE
    modeParam.spkLeftChanSel = spkLeftChanSel;
    modeParam.spkRightChanSel = spkRightChanSel;
#endif
#endif
    hal_AudSetCodecMode(&modeParam);

#ifdef AUD_SPK_ON_WITH_LCD
    if (pmd_AudExtPaExists())
    {
        if (g_halAudOrgSelCfg.spkSel || g_halAudOrgSelCfg.spkMonoSel)
        {
            pmd_EnablePower(PMD_POWER_LOUD_SPEAKER, TRUE);
            g_halAudExtPaConfigNeeded = TRUE;
        }
        else if (g_halAudExtPaConfigNeeded)
        {
            if ( ! (selCfg.spkSel || selCfg.spkMonoSel) )
            {
                // Internal PA has just been turned off
                // aud_Setup() in interrupts will never run into here?
                sxr_Sleep(g_halAudIntSpkPaStableTime);
            }
            pmd_EnablePower(PMD_POWER_LOUD_SPEAKER, FALSE);
            g_halAudExtPaConfigNeeded = FALSE;
        }
    }
#endif // AUD_SPK_ON_WITH_LCD
}

PUBLIC VOID hal_AudSetMicCfg(HAL_AUD_MIC_CFG_T micCfg)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    if (micCfg.micGain >= 8)
        micCfg.micGain -= 8;
    else
        micCfg.micGain += 8;
#endif

    hwp_configRegs->audio_mic_cfg = micCfg.reg;
}

PUBLIC VOID hal_AudSetSpkCfg(HAL_AUD_SPK_CFG_T spkCfg)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    if ( (g_halAudLineInSpeakerInUse || g_halAudUser == HAL_AUD_USER_LINEIN) &&
            !spkCfg.noSpkMute )
    {
        // Never mute speaker when in line-in mode on 8808
        spkCfg.spkGain = 0;
        spkCfg.noSpkMute = TRUE;
    }
#endif

    hwp_configRegs->audio_spk_cfg = spkCfg.reg;
}

PUBLIC VOID hal_AudSetRcvGain(UINT32 gain)
{
    hwp_configRegs->audio_rcv_gain = CFG_REGS_AU_RCV_GAIN(gain);
}

PUBLIC VOID hal_AudSetHeadGain(UINT32 gain)
{
    hwp_configRegs->audio_head_gain = CFG_REGS_AU_HEAD_GAIN(gain);
}

PUBLIC HAL_AUD_PD_T hal_AudGetPowerStatus(VOID)
{
    HAL_AUD_PD_T pdStatus;
    pdStatus.reg = hwp_configRegs->audio_pd_set;

#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD)
    if (g_halAudDacPrevPwr == HAL_AUD_DAC_PREV_PWR_OFF)
    {
        pdStatus.noDacPd = FALSE;
    }
    else if (g_halAudDacPrevPwr == HAL_AUD_DAC_PREV_PWR_ON)
    {
        pdStatus.noDacPd = TRUE;
    }
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD

    return pdStatus;
}

PUBLIC HAL_AUD_SEL_T hal_AudGetSelCfg(VOID)
{
    return g_halAudOrgSelCfg;
}

PUBLIC HAL_AUD_MIC_CFG_T hal_AudGetMicCfg(VOID)
{
    HAL_AUD_MIC_CFG_T micCfg;
    micCfg.reg = hwp_configRegs->audio_mic_cfg;

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    if (micCfg.micGain >= 8)
        micCfg.micGain -= 8;
    else
        micCfg.micGain += 8;
#endif

    return micCfg;
}

PUBLIC UINT32 hal_AudGetDacVolumn()
{
    UINT32 data;
    hal_AbbRegRead(CODEC_DIG_DAC_GAIN, &data);
    data =  ((ABB_DIG_DAC_VOLUME_MASK&data)>>ABB_DIG_DAC_VOLUME_SHIFT);

    return data;
}

#if defined (CHIP_DIE_8809E)  || defined (CHIP_DIE_8955) || defined(CHIP_DIE_8909)


PUBLIC UINT32 hal_Init_fm(VOID)
{
    hal_AbbRegWrite(CODEC_SETITING_3, 0x3008);
    hal_AbbRegWrite(CODEC_MISC_SETTING, 0xe380);
}
PUBLIC UINT32 hal_AudEnRcv(BOOL enable)
{
    UINT32 setting;
    UINT32 metalid = hal_SysGetChipId(HAL_SYS_CHIP_METAL_ID);
    hal_AbbRegRead(CODEC_SETITING_1,&setting);


    if (enable == 1)
    {
        setting |= (ABB_AU_RCV_SEL_REG);
        hal_AbbRegWrite(CODEC_SETITING_1, setting);


    }
    else
    {
        setting &= (~(ABB_AU_RCV_SEL_REG ));
        hal_AbbRegWrite(CODEC_SETITING_1, setting);
    }



    hal_AbbRegRead(CODEC_LDO_SETTING1,&setting);

    if (enable == 1)
    {
        if(metalid< HAL_SYS_CHIP_METAL_ID_U03)
            setting |= (ABB_PA_EN_RECI_DR |ABB_PA_EN_RECI_REG);
        else
        {
            setting &= (~(ABB_PA_EN_RECI_REG));
            setting |= (ABB_PA_EN_RECI_DR );
        }

    }
    else
    {
        if(metalid< HAL_SYS_CHIP_METAL_ID_U03)
        {
            setting &= (~(ABB_PA_EN_RECI_REG));
            setting |= (ABB_PA_EN_RECI_DR );
        }
        else
            setting |= (ABB_PA_EN_RECI_DR |ABB_PA_EN_RECI_DR );
    }

    hal_AbbRegWrite(CODEC_LDO_SETTING1, setting);

}

#endif
PUBLIC VOID hal_AudSetHPCfg(BOOL ON)
{
    HAL_AUD_SEL_T selCfg;
    selCfg.reg = hwp_configRegs->audio_sel_cfg;
    if(ON)
        selCfg.headSel = 1;
    else
        selCfg.headSel = 0;

    hwp_configRegs->audio_sel_cfg = selCfg.reg;

}


PUBLIC HAL_AUD_SPK_CFG_T hal_AudGetSpkCfg(VOID)
{
    HAL_AUD_SPK_CFG_T spkCfg;
    spkCfg.reg = hwp_configRegs->audio_spk_cfg;
    return spkCfg;
}

PUBLIC UINT32 hal_AudGetRcvGain(VOID)
{
    return hwp_configRegs->audio_rcv_gain & CFG_REGS_AU_RCV_GAIN_MASK;
}

PUBLIC UINT32 hal_AudGetHeadGain(VOID)
{
    return hwp_configRegs->audio_head_gain & CFG_REGS_AU_HEAD_GAIN_MASK;
}

PUBLIC UINT32 hal_AudRcvGainDb2Val(INT32 db)
{
    if (db < 0)
        db = 0;
    else if (db > 21)
        db = 21;
    return db/3*2;
}

PUBLIC UINT32 hal_AudHeadGainDb2Val(INT32 db)
{
    if (db < 0)
        db = 0;
    else if (db > 21)
        db = 21;
    return db/3*2;
}

PUBLIC UINT32 hal_AudSpkGainDb2Val(INT32 db)
{
    UINT32 reg;
#ifdef CHIP_HAS_AP
    // 0:-3dB, 2:10dB, 4:12dB, 6:14db, 8:16dB,
    // 10:18, 12:20, 14:22dB
    if (db < 4)
    {
        reg = 0;
    }
    else
    {
        if (db < 10)
            db = 10;
        else if (db > 22)
            db = 22;
        reg = (db - 10) + 2;
    }
#elif (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE || \
     CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    if (db < 0)
        db = 0;
    else if (db > 21)
        db = 21;
    reg = db/3*2;
#else // 8809 or later
    // 0:-2dB, 2:10dB, 4:16dB, 6:19db, 8:20.5dB,
    // 10:22dB, 12:23dB, 14:24dB
    if (db <= 4)
        reg = 0;
    else if (db <= 13)
        reg = 2;
    else if (db <= 17)
        reg = 4;
    else if (db <= 19)
        reg = 6;
    else if (db <= 21)
        reg = 8;
    else if (db <= 22)
        reg = 10;
    else if (db <= 23)
        reg = 12;
    else // 24 or above
        reg = 14;
#endif

    return reg;
}

PUBLIC UINT32 hal_AudMicGainDb2Val(INT32 db)
{
    if (db < 0)
        db = 0;
    else if (db > 21)
        db = 21;
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    return db/3*2;
#else // 8808 or later
    return 8+db/3;
#endif
}

HAL_MODULE_CLK_INIT(AUDIO)
{
    gAudioClk = hal_ClkGet(FOURCC_AUDIO);
    HAL_ASSERT((gAudioClk != NULL), "Clk AUDIO not found!");
}

