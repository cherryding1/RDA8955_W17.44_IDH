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

#include "aud_codec_common.h"
#include "audp_debug.h"

#include "hal_aif.h"
#include "hal_error.h"
#include "hal_sys.h"

#include "hal_rda_abb.h"
#include "hal_rda_audio.h"
#include "hal_ispi.h"
#include "pmd_m.h"
#include "sxr_tls.h"
#include "calib_m.h"

#ifdef GALLITE_IS_8806
#include "rfd_xcv.h"
#endif

#include "tgt_aud_cfg.h"

#ifdef BT_BTHSNREC_SUPPORT
#include "rdabt_hfp.h"
#endif

#define MUTE_ANA_GAIN ((INT8)0)
#define MUTE_DAC_GAIN ((INT8)-100)
#define MUTE_ADC_GAIN ((INT8)-100)


// aud_StreamStart() and aud_StreamStop(), aud_Setup() are of mutually exclusive access,
// with the help of semaphore. Then here we do not need mutex any more.

#ifdef ABB_HP_DETECT
extern VOID hal_AudSaveHeadphoneState(BOOL on);
#endif

extern BOOL g_audSetHeadMaxDigitalGain;
extern BOOL g_audSetMicMaxGain;

PUBLIC SND_SPK_LEVEL_T   aud_CodecCommonGetCalibMicLevel(SND_ITF_T itf, BOOL isIncall);
PRIVATE  SND_SPK_LEVEL_T g_audCodecLatestMicLevel = SND_SPK_MUTE;

// =============================================================================
//  MACROS
// =============================================================================

// Define VCOM_BAT_LARGE_CAP to 1 if there is no external loud speaker and
// the capacitor connected to VCOM_BAT is larger than 1uF
#define VCOM_BAT_LARGE_CAP 0

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
#define AUD_EARPIECE_PA_POWER_STABLE_TIME (1000 MS_WAITING)
#elif (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#define AUD_EARPIECE_PA_POWER_STABLE_TIME (200 MS_WAITING)
#else
#define AUD_EARPIECE_PA_POWER_STABLE_TIME (50 MS_WAITING)
#endif

#define AUD_FIRST_OPEN_OFF 0
#define AUD_FIRST_OPEN_ON 1
#define AUD_FIRST_OPEN_DONE 2

#define IS_APP_SND_ITF(itf) (itf == SND_ITF_FM || itf == SND_ITF_TV)


// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

/// Playing status
PRIVATE BOOL  g_audCodecPlaying  = FALSE;

/// Recording status
PRIVATE BOOL  g_audCodecRecording  = FALSE;

PRIVATE BOOL  g_audTonePlaying  = FALSE;

/// Openness status
PRIVATE BOOL  g_audCodecOpened  = FALSE;
PRIVATE BOOL  g_audAIFOpened  = FALSE;

/// Remember the last settings applied
/// They will be set again to take into account any potential
/// calibration change.
PRIVATE AUD_LEVEL_T g_audCodecLatestLevel  =
{
    .spkLevel   = SND_SPK_MUTE,
    .micLevel   = SND_MIC_MUTE,
    .sideLevel  = SND_SIDE_MUTE,
    .toneLevel  = SND_TONE_0DB,
};

PRIVATE AUD_DEVICE_CFG_T g_audCodecLatestCfg   =
{
    .spkSel = AUD_SPK_DISABLE,
    .spkType = AUD_SPEAKER_STEREO_NA,
    .micSel = AUD_MIC_DISABLE,
    .level  = &g_audCodecLatestLevel
};

PRIVATE HAL_AIF_FREQ_T g_audCodecSampleRate = HAL_AIF_FREQ_8000HZ;

PRIVATE BOOL g_audCodecVoiceQuality = FALSE;

PRIVATE HAL_AUD_USER_T g_audCodecCurrentUser = HAL_AUD_USER_NULL;

PRIVATE CONST HAL_AIF_CONFIG_T *g_audCodecCurrentAifCfgPtr = NULL;

PRIVATE CONST HAL_AIF_CONFIG_T g_audCodecToneAifCfg =
{
    .interface = HAL_AIF_IF_PARALLEL_STEREO,
    .sampleRate = HAL_AIF_FREQ_8000HZ,
    .channelNb = HAL_AIF_MONO,
    .serialCfg = NULL,
};

PRIVATE BOOL g_audCodecMicInput = FALSE;

PRIVATE UINT8 g_audFirstOpen = AUD_FIRST_OPEN_OFF;

#ifdef CHIP_HAS_AP
PRIVATE BOOL g_audCodecAifConfigByAp = FALSE;
#endif

PRIVATE INT16 g_audCodecOutAlgGainDb2Val = 16384;

#ifdef CHIP_HAS_AP
PRIVATE BOOL g_audCodecBtNRECFlag = FALSE;
#endif

#ifdef CHIP_DIE_8955
PRIVATE BOOL g_audCodecEnFMRecNew = FALSE;
#endif

// =============================================================================
//  FUNCTIONS
// =============================================================================
// calc as power(10, dB/20) * 16384
PUBLIC VOID aud_CodecCommonOutAlgGainDb2Val(INT8 alg)
{
    switch(alg)
    {
        case -45: //mute
            g_audCodecOutAlgGainDb2Val = 0;
            break;
        case -42: //level1 -42db
            g_audCodecOutAlgGainDb2Val = 130;
            break;
        case -39: //level2 -39db
            g_audCodecOutAlgGainDb2Val = 184;
            break;
        case -36: //level3 -36db
            g_audCodecOutAlgGainDb2Val = 260;
            break;
        case -33: //level4 -33db
            g_audCodecOutAlgGainDb2Val = 367;
            break;
        case -30: //level5 -30db
            g_audCodecOutAlgGainDb2Val = 518;
            break;
        case -27: //level6 -27db
            g_audCodecOutAlgGainDb2Val = 732;
            break;
        case -24: //level7 -24db
            g_audCodecOutAlgGainDb2Val = 1034;
            break;
        case -21: //level8 -21db
            g_audCodecOutAlgGainDb2Val = 1460;
            break;
        case -18: //level9 -18db
            g_audCodecOutAlgGainDb2Val = 2063;
            break;
        case -15: //level10 -15db
            g_audCodecOutAlgGainDb2Val = 2914;
            break;
        case -12: //level11 -12db
            g_audCodecOutAlgGainDb2Val = 4115;
            break;
        case -9: //level12 -9db
            g_audCodecOutAlgGainDb2Val = 5813;
            break;
        case -6: //level13 -6db
            g_audCodecOutAlgGainDb2Val = 8211;
            break;
        case -3: //level14 -3db
            g_audCodecOutAlgGainDb2Val = 11599;
            break;
        case 0: //level15 0db
            g_audCodecOutAlgGainDb2Val = 16384;
            break;
        default:
            g_audCodecOutAlgGainDb2Val = 16384;
            break;
    }

    return;
}

PRIVATE VOID aud_CodecCommonSetRcvGain(INT32 ana, INT32 dac)
{
    UINT32 anaGain = 0;
    UINT32 dacVol  = 0;
    UINT32 oldAnaGain = 0;
    INT32 target  = 0;
    INT32 origin = 0;

#ifdef GALLITE_IS_8806
    UINT32 xcvChipId = rfd_XcvGetChipId();
    if (! (xcvChipId == GALLITE_CHIP_ID_8806_U03 ||
            xcvChipId == GALLITE_CHIP_ID_8806_U05) )
    {
        if(ana >= 6)
        {
            ana -= 6;
        }
        else if(ana >= 3)
        {
            ana -= 3;
            dac -= 3;
        }
        else
        {
            ana = 0;
            dac -= 6;
        }
    }
#endif

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
#ifndef AUD_DONT_ADPATER_8809U06_RCVGAIN
    // U06 or later
    if ((hal_SysGetChipId(HAL_SYS_CHIP_METAL_ID)&0xff) >= HAL_SYS_CHIP_METAL_ID_U06)
    {
        if(ana >= 6)
        {
            ana -= 6;
        }
        else if(ana >= 3)
        {
            ana -= 3;
            dac -= 3;
        }
        else
        {
            ana = 0;
            dac -= 6;
        }
    }
#endif
#endif

    anaGain = hal_AudRcvGainDb2Val(ana);
    dacVol = hal_AudDacGainDb2Val(dac);
    // Set ABB DAC digital gain
    target = dacVol;
    origin =  hal_AudGetDacVolumn();

#ifdef CHIP_8955_AUD_WAVE_TEST
    // set dig gain from cur volume to 1
    for (origin--; origin>=1; origin--)
    {
        hal_AudSetDacVolume((UINT32)(origin));
        sxr_Sleep(1 MS_WAITING);
    }
#endif // CHIP_8955_AUD_WAVE_TEST

    // set ana gain to target volume
    hal_AudSetRcvGain(anaGain);

#ifdef CHIP_8955_AUD_WAVE_TEST
	// set dig gain from volume 1 to target
    target = dacVol;
    origin = 1;
    if (origin > target)
    {
        for (origin--; origin>=target; origin--)
        {
            hal_AudSetDacVolume((UINT32)(origin));
            sxr_Sleep(1 MS_WAITING);
        }
    }
    else
    {
        for (origin++; origin<=target; origin++)
        {
            hal_AudSetDacVolume((UINT32)(origin));
            sxr_Sleep(1 MS_WAITING);
        }
    }
#endif // CHIP_8955_AUD_WAVE_TEST
    hal_AudSetDacVolume(target);
}

PRIVATE VOID aud_CodecCommonSetHeadGain(INT32 ana, INT32 dac)
{
    UINT32 anaGain = 0;
    UINT32 dacVol  = 0;
    INT32 target  = 0;
    INT32 origin = 0;

#ifdef GALLITE_IS_8806
    UINT32 xcvChipId = rfd_XcvGetChipId();
    if (! (xcvChipId == GALLITE_CHIP_ID_8806_U03 ||
            xcvChipId == GALLITE_CHIP_ID_8806_U05) )
    {
        if(ana >= 6)
        {
            ana -= 6;
        }
        else if(ana >= 3)
        {
            ana -= 3;
            dac -= 3;
        }
        else
        {
            ana = 0;
            dac -= 6;
        }
    }
#endif

    anaGain = hal_AudHeadGainDb2Val(ana);
    dacVol = hal_AudDacGainDb2Val(dac);

    // Set ABB DAC digital gain
    target = dacVol;
    origin =  hal_AudGetDacVolumn();

    AUD_TRACE(AUD_INFO_TRC, 0,
              "aud_CodecCommonSetHeadGain origin=%d targt=%d",origin, target);

#ifdef CHIP_8955_AUD_WAVE_TEST
    // set dig gain from cur volume to 1
    for (origin--; origin>=1; origin--)
    {
        hal_AudSetDacVolume((UINT32)(origin));
        sxr_Sleep(1 MS_WAITING);
    }
#endif // CHIP_8955_AUD_WAVE_TEST

    // set ana gain to target volume
    hal_AudSetHeadGain(anaGain);

#ifdef CHIP_8955_AUD_WAVE_TEST
	// set dig gain from 1 to target volume
    target = dacVol;
    origin = 1;

    if (origin > target)
    {
        for (origin--; origin>=target; origin--)
        {
            hal_AudSetDacVolume((UINT32)(origin));
            sxr_Sleep(1 MS_WAITING);
        }
    }
    else
    {
        for (origin++; origin<=target; origin++)
        {
            hal_AudSetDacVolume((UINT32)(origin));
            sxr_Sleep(1 MS_WAITING);
        }

    }
#endif // CHIP_8955_AUD_WAVE_TEST
    hal_AudSetDacVolume(target);
}

PRIVATE VOID aud_CodecCommonSetSpkCfg(BOOL mute, INT32 ana, INT32 dac)
{
    INT32 target =0;
    INT32 origin= 0;

#ifdef GALLITE_IS_8806
    UINT32 xcvChipId = rfd_XcvGetChipId();
    if (! (xcvChipId == GALLITE_CHIP_ID_8806_U03 ||
            xcvChipId == GALLITE_CHIP_ID_8806_U05) )
    {
        if(ana >= 6)
        {
            ana -= 6;
        }
        else if(ana >= 3)
        {
            ana -= 3;
            dac -= 3;
        }
        else
        {
            ana = 0;
            dac -= 6;
        }
    }
#endif

    HAL_AUD_SPK_CFG_T spkCfg;
    spkCfg.noSpkMute = !mute;
    spkCfg.spkGain = hal_AudSpkGainDb2Val(ana);
    UINT32 dacVol = hal_AudDacGainDb2Val(dac);

    // Set ABB DAC digital gain
    target = dacVol;
    origin =  hal_AudGetDacVolumn();

#ifdef CHIP_8955_AUD_WAVE_TEST
	// set dig gain from cur to volume 1
    for (origin--; origin>=1; origin--)
    {
        hal_AudSetDacVolume((UINT32)(origin));
        sxr_Sleep(1 MS_WAITING);
    }
#endif // CHIP_8955_AUD_WAVE_TEST

    if (!spkCfg.noSpkMute)
    {
        spkCfg.spkGain = 0;
    }

#ifdef CHIP_8955_AUD_WAVE_TEST
    HAL_AUD_SPK_CFG_T oldCfg = hal_AudGetSpkCfg();
    if (!oldCfg.noSpkMute)
    {
        oldCfg.spkGain = 0;
    }

    target = (INT32)(spkCfg.spkGain);
    origin = (INT32)(oldCfg.spkGain);

    if (origin > target)
    {
        for (origin--; origin>=target; origin--)
        {
            oldCfg.spkGain = (UINT32)(origin);
            hal_AudSetSpkCfg(oldCfg);
            sxr_Sleep(1 MS_WAITING);
        }
        if (!spkCfg.noSpkMute && oldCfg.noSpkMute)
        {
            // Mute at last
            oldCfg.noSpkMute = FALSE;
            hal_AudSetSpkCfg(oldCfg);
        }
    }
    else
    {
        for (origin++; origin<=target; origin++)
        {
            oldCfg.spkGain = (UINT32)(origin);
            hal_AudSetSpkCfg(oldCfg);
            sxr_Sleep(1 MS_WAITING);
        }

    }
#endif // CHIP_8955_AUD_WAVE_TEST

	// set ana gain to target volume
    hal_AudSetSpkCfg(spkCfg);

#ifdef CHIP_8955_AUD_WAVE_TEST
	target = dacVol;
	origin = hal_AudGetDacVolumn();

    for (origin++; origin<=target; origin++)
    {
        hal_AudSetDacVolume((UINT32)(origin));
        sxr_Sleep(1 MS_WAITING);
    }

#endif // CHIP_8955_AUD_WAVE_TEST
    hal_AudSetDacVolume(dacVol);
}

PRIVATE VOID aud_CodecCommonSetMicCfg(BOOL mute, INT32 ana, INT32 adc)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8810 && !defined(CHIP_DIE_8810E))
    if (hal_SysGetChipId(HAL_SYS_CHIP_METAL_ID) < HAL_SYS_CHIP_METAL_ID_U09 &&
            adc > -8)
    {
        // Avoid adc overflow issue
        adc = -8;
    }
#endif

    HAL_AUD_MIC_CFG_T micCfg;
    micCfg.noMicMute = !mute;
    micCfg.micGain = hal_AudMicGainDb2Val(ana);
    UINT32 adcVol = hal_AudAdcGainDb2Val(adc);

    // No need to change MIC gain in a sequential manner
    hal_AudSetMicCfg(micCfg);
    // Set ABB ADC digital gain
    hal_AudSetAdcVolume(adcVol);
}

// =============================================================================
// aud_CodecCommonSetConfig
// -----------------------------------------------------------------------------
/// Set the audio configuration.
/// @param isRecording Whether it is recording stream or not
/// @param user The audio user
/// @param pAifCfg Pointing to the AIF configuration to be set
/// @return TRUE if success, FALSE otherwise
// =============================================================================
PUBLIC BOOL aud_CodecCommonSetConfig(BOOL isRecording, HAL_AUD_USER_T user, CONST HAL_AIF_CONFIG_T *pAifCfg)
{

    AUD_TRACE(AUD_INFO_TRC, 0,
              "aud_CodecCommonSetConfig g_audCodecOpened=%d",g_audCodecCurrentUser);

    // It is required to call StreamStart before StreamRecord
    if (isRecording &&( g_audCodecCurrentUser == user)&&g_audAIFOpened)
    {
        // The audio mode was set when starting stream
        return TRUE;
    }

    if ((g_audCodecCurrentUser != HAL_AUD_USER_NULL)&&g_audAIFOpened)
    {
        return FALSE;
    }

    if (user == HAL_AUD_USER_CODEC ||
            user == HAL_AUD_USER_AIF_ONLY ||
            user == HAL_AUD_USER_LINEIN_WITH_CODEC)
    {
        AUD_ASSERT(pAifCfg != NULL, "Missing AIF cfg");
    }

#ifdef CHIP_HAS_AP
    if (g_audCodecAifConfigByAp)
    {
        //    g_audCodecCurrentAifCfgPtr = NULL;
        g_audCodecCurrentAifCfgPtr = pAifCfg;
        // Always enable MIC
        g_audCodecMicInput = TRUE;
    }
    else
#endif
    {
        g_audCodecCurrentAifCfgPtr = pAifCfg;
        g_audCodecMicInput = FALSE;

        if (g_audCodecCurrentAifCfgPtr != NULL)
        {
            HAL_AIF_IF_T aifIf = g_audCodecCurrentAifCfgPtr->interface;
            // Check if there is MIC input
            if (aifIf == HAL_AIF_IF_PARALLEL ||
                    aifIf == HAL_AIF_IF_PARALLEL_STEREO)
            {
                g_audCodecMicInput = TRUE;
            }
#ifdef CHIP_DIE_8955
            if(g_audCodecEnFMRecNew == TRUE)
                g_audCodecMicInput = FALSE;
#endif
        }
    }

    // g_audCodecCurrentUser will be cleared in aud_CodecCommonClose()
    g_audCodecCurrentUser = user;

    return TRUE;
}


// =============================================================================
// aud_CodecCommonCodecPrePowerOnConfig
// -----------------------------------------------------------------------------
/// Set registers before powering on codec.
/// @return
// =============================================================================
PRIVATE VOID aud_CodecCommonCodecPrePowerOnConfig(VOID)
{
    // Set voice mode
    hal_AudSetVoiceMode(g_audCodecVoiceQuality);
    // Set sample rate
    hal_AudSetSampleRate(g_audCodecCurrentUser, g_audCodecSampleRate);
}


// =============================================================================
// aud_CodecCommonChipConfig
// -----------------------------------------------------------------------------
/// Configure the CodecGallite chip, through I2C.
/// The stream parameter is needed to know the format of the PCM
/// thing to play. The cfg one is to know which mic, spk etc use
/// Interfaces matching table:
/// -
/// -
/// .
///
///
/// @param sampleRate Sample Rate of the audio to play
/// @param voice \c TRUE if in voice mode.
/// @param mono  \c TRUE if the stream played/recorded is in Mono
/// @return
// =============================================================================
PRIVATE AUD_ERR_T aud_CodecCommonChipConfig(
    HAL_AIF_FREQ_T sampleRate,
    BOOL voice,
    BOOL mono)
{
    AUD_PROFILE_FUNCTION_ENTRY(aud_ChipConfig);

    if (voice)
    {
        AUD_ASSERT(sampleRate == HAL_AIF_FREQ_8000HZ,
                   "AUD: CodecGallite: Voice mode only supports 8kHz frequency.");
        AUD_ASSERT(mono == TRUE,
                   "AUD: CodecGallite: Voice mode is mono only.");
        // TODO add a bunch of assert/check
    }

    HAL_AIF_FREQ_T oldSampleRate = g_audCodecSampleRate;
    BOOL oldVoiceQuality = g_audCodecVoiceQuality;

    g_audCodecSampleRate = sampleRate;
    g_audCodecVoiceQuality = voice;

    HAL_AUD_CALLBACK_T callback = NULL;

    if (g_audCodecCurrentUser == HAL_AUD_USER_CODEC ||
            g_audCodecCurrentUser == HAL_AUD_USER_LINEIN_WITH_CODEC)
    {
        callback = &aud_CodecCommonCodecPrePowerOnConfig;
    }
    else if (g_audCodecCurrentUser == HAL_AUD_USER_FM)
    {
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
        callback = &aud_CodecCommonCodecPrePowerOnConfig;
#endif
    }
    else if (g_audCodecCurrentUser == HAL_AUD_USER_AIF_ONLY)
    {
#ifdef CHIP_HAS_AP
        // AIF clock depends on the sample rate setting
        callback = &aud_CodecCommonCodecPrePowerOnConfig;
#endif
    }
    else if (g_audCodecCurrentUser == HAL_AUD_USER_LINEIN)
    {
    }
    else
    {
        AUD_ASSERT(FALSE, "Invalid audio mode: %d", g_audCodecCurrentUser);
    }

    // Configure audio codec
    BOOL ret = hal_AudOpen(g_audCodecCurrentUser, callback);
    if (!ret)
    {
        g_audCodecSampleRate = oldSampleRate;
        g_audCodecVoiceQuality = oldVoiceQuality;
        AUD_PROFILE_FUNCTION_EXIT(aud_ChipConfig);
        return AUD_ERR_RESOURCE_BUSY;
    }

    AUD_PROFILE_FUNCTION_EXIT(aud_ChipConfig);
    return AUD_ERR_NO;
}


// =============================================================================
// aud_CodecCommonAifConfig
// -----------------------------------------------------------------------------
/// Configure the AIF interface for use with the CodecGallite chip.
/// @return AUD_ERR_NO
// =============================================================================
INLINE AUD_ERR_T aud_CodecCommonAifConfig(VOID)
{

    AUD_TRACE(AUD_INFO_TRC, 0,
              "aud_CodecCommonAifConfig g_audCodecCurrentAifCfgPtr=%x",g_audCodecCurrentAifCfgPtr);

    if (g_audCodecCurrentAifCfgPtr != NULL)
    {
        AUD_TRACE(AUD_INFO_TRC, 0, "aud_CodecCommonAifConfig =%d",g_audCodecCurrentAifCfgPtr->sampleRate);

        if (hal_AifOpen(g_audCodecCurrentAifCfgPtr) != HAL_ERR_NO)
        {
            return AUD_ERR_RESOURCE_BUSY;
        }
#ifdef CHIP_DIE_8955
        hal_AifEnFMRecNewPath(g_audCodecEnFMRecNew);
#endif
        g_audAIFOpened = TRUE;
    }

    return AUD_ERR_NO;
}


// =============================================================================
// aud_CodecCommonClose
// -----------------------------------------------------------------------------
/// Close the driver and configure the CodecGallite and AIF.
/// @return AUD_ERR_NO or ...
// =============================================================================
PRIVATE AUD_ERR_T aud_CodecCommonClose(VOID)
{
#ifdef CHIP_HAS_AP
    if (g_audCodecAifConfigByAp)
    {
        //    AUD_ASSERT(g_audCodecCurrentAifCfgPtr == NULL,
        //    "AIF cfg ptr is not null while in AP mode");
    }
#endif

    if (g_audCodecCurrentAifCfgPtr != NULL)
    {
        hal_AifClose();
        g_audCodecCurrentAifCfgPtr = NULL;
    }

    if (g_audCodecCurrentUser != HAL_AUD_USER_NULL)
    {
        // Except for earpiece detection, HAL AUD is always
        // used in EDRV AUD, and is protected by EDRV AUD mutex.
        while(!hal_AudClose(g_audCodecCurrentUser))
        {
            sxr_Sleep(AUD_TIME_RETRY);
        }

        g_audCodecCurrentUser = HAL_AUD_USER_NULL;
    }

#ifdef SWITCH_AUDIO_CODEC_FREQ
    hal_AudRestoreSampleRate();
#endif

    g_audCodecSampleRate = HAL_AIF_FREQ_8000HZ;
    g_audCodecVoiceQuality = FALSE;

    g_audCodecLatestCfg.spkSel = AUD_SPK_DISABLE;
    g_audCodecLatestCfg.spkType = AUD_SPEAKER_STEREO_NA;
    g_audCodecLatestCfg.micSel = AUD_MIC_DISABLE;

    g_audCodecLatestLevel.spkLevel = SND_SPK_MUTE;
    g_audCodecLatestLevel.micLevel = SND_MIC_MUTE;
    g_audCodecLatestLevel.sideLevel = SND_SIDE_MUTE;
    g_audCodecLatestLevel.toneLevel = SND_TONE_0DB;

    g_audCodecOpened = FALSE;
    g_audAIFOpened = FALSE;

    return AUD_ERR_NO;
}


// =============================================================================
// aud_CodecCommonOpen
// -----------------------------------------------------------------------------
/// Open the driver and configure the CodecGallite and AIF.
/// @param sampleRate Sample Rate of the audio to play
/// @param voice \c TRUE if in voice mode.
/// @param mono  \c TRUE if the stream played/recorded is in Mono
/// @return AUD_ERR_NO or ...
// =============================================================================
PRIVATE AUD_ERR_T aud_CodecCommonOpen(HAL_AIF_FREQ_T sampleRate, BOOL voice, BOOL mono)
{
    AUD_ERR_T errStatus;

    errStatus = aud_CodecCommonChipConfig(sampleRate, voice, mono);
    AUD_TRACE(AUD_INFO_TRC, 0,
              "aud_CodecCommonOpen errStatus=%d",errStatus);

    if (errStatus == AUD_ERR_NO)
    {
        errStatus = aud_CodecCommonAifConfig();
        if (errStatus == AUD_ERR_NO)
        {
            g_audCodecOpened = TRUE;

            if (g_audFirstOpen == AUD_FIRST_OPEN_OFF)
            {
                g_audFirstOpen = AUD_FIRST_OPEN_ON;
            }
            else
            {
                g_audFirstOpen = AUD_FIRST_OPEN_DONE;
            }
        }
    }

    if (errStatus != AUD_ERR_NO)
    {
        aud_CodecCommonClose();
    }

    return errStatus;
}


// =============================================================================
// aud_CodecCommonSetup
// -----------------------------------------------------------------------------
/// Configure the audio.
///
/// This functions configures an audio interface: gain for the side tone and
/// the speaker, input selection for the microphone and output selection for
/// the speaker.
///
/// @param cfg The configuration set applied to the audio interface. See
/// #AUD_DEVICE_CFG_T for more details.
/// @return #AUD_ERR_NO if it can execute the configuration.
// =============================================================================
PUBLIC AUD_ERR_T aud_CodecCommonSetup(SND_ITF_T itf, CONST AUD_DEVICE_CFG_T* devCfg)
{
    INT8 regAnaLvl, regDacLvl,AlgGain;

#ifdef RECV_USE_SPK_LINE
    AUD_DEVICE_CFG_T cfgCopy = *devCfg;
    if (AUD_SPK_RECEIVER == cfgCopy.spkSel)
    {
        cfgCopy.spkSel = AUD_SPK_LOUD_SPEAKER;
    }
    CONST AUD_DEVICE_CFG_T *cfg = &cfgCopy;
    // Receiver mode should be set BEFORE calling hal_AudSetSelCfg
    hal_AudEnableRecvMode(SND_ITF_RECEIVER == itf);
#else
    CONST AUD_DEVICE_CFG_T *cfg = devCfg;
#endif
    // Check parameters
    AUD_ASSERT(cfg->spkSel<AUD_SPK_QTY || cfg->spkSel == AUD_SPK_DISABLE,
               "AUD: Improper Spk Id:%d", cfg->spkSel);
    AUD_ASSERT(cfg->micSel<AUD_MIC_QTY || cfg->micSel == AUD_MIC_DISABLE,
               "AUD: Improper Mic Id:%d", cfg->micSel);
    AUD_ASSERT(cfg->level->spkLevel<AUD_SPK_VOL_QTY, "AUD: Improper Spk Level");
    AUD_ASSERT(cfg->level->micLevel<AUD_MIC_VOL_QTY, "AUD: Improper Mic Level");
    AUD_ASSERT(cfg->level->sideLevel<AUD_SIDE_VOL_QTY, "AUD: Improper Side Level");
    AUD_ASSERT(cfg->level->toneLevel<AUD_TONE_VOL_QTY, "AUD: Improper Tone Level: %d",cfg->level->toneLevel );


    AUD_TRACE(AUD_INFO_TRC, 0,
              "aud_CodecCommonSetup devCfg.spkSel=%d  devCfg.level.spkLevel=%d",devCfg->spkSel, cfg->level->spkLevel);

    CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
    UINT8 calibItf = itf;
    if (g_audCodecVoiceQuality)
    {
        if((itf == SND_ITF_BLUETOOTH) && (aud_CodecCommonGetBtNRECFlag()))
        {
            calibItf = CALIB_ITF_BLUETOOTH_NREC;
        }
        if((itf == SND_ITF_EAR_PIECE) && (aud_CodecCommonGetForceReceiverMicSelectionFlag()))
        {
            calibItf = CALIB_ITF_EAR_PIECE_THREE;
        }
    }

    if(g_audTonePlaying)
        calibItf = SND_ITF_FM;

    CALIB_AUDIO_OUT_GAINS_T *pCalibOutGains =
        &calibPtr->bb->audio[calibItf].audioGains.outGains[cfg->level->spkLevel];

    if (IS_APP_SND_ITF(itf))
    {
        if (aud_GetCurOutputDevice() == SND_ITF_EAR_PIECE)
        {
            regAnaLvl = pCalibOutGains->voiceOrEpAna;
            regDacLvl = pCalibOutGains->voiceOrEpDac;
            AlgGain =  pCalibOutGains->voiceOrEpAlg;
        }
        else
        {
            regAnaLvl = pCalibOutGains->musicOrLsAna;
            regDacLvl = pCalibOutGains->musicOrLsDac;
            AlgGain = pCalibOutGains->musicOrLsAlg;
        }
    }
    else
    {
        if (g_audCodecVoiceQuality)
        {
            regAnaLvl = pCalibOutGains->voiceOrEpAna;
            regDacLvl = pCalibOutGains->voiceOrEpDac;
            AlgGain = pCalibOutGains->voiceOrEpAlg;
        }
        else
        {
            regAnaLvl = pCalibOutGains->musicOrLsAna;
            regDacLvl = pCalibOutGains->musicOrLsDac;
#if defined(BT_A2DP_ANA_DAC_MUTE)
            if((SND_ITF_LOUD_SPEAKER == itf) && (SND_ITF_BLUETOOTH == aud_GetCurOutputDevice()))
            {
                AlgGain = calibPtr->bb->audio[SND_ITF_BLUETOOTH].audioGains.outGains[cfg->level->spkLevel].musicOrLsAlg;
            }
            else if((SND_ITF_EAR_PIECE== itf) && (SND_ITF_BLUETOOTH == aud_GetCurOutputDevice()))
            {
                AlgGain = calibPtr->bb->audio[SND_ITF_BLUETOOTH].audioGains.outGains[cfg->level->spkLevel].musicOrLsAlg;
            }
            else
            {
                AlgGain =  pCalibOutGains->musicOrLsAlg;
            }
#else
            AlgGain = pCalibOutGains->musicOrLsAlg;
#endif
        }
    }

    // For test mode AUD_TEST_RECVMIC_IN_EARPIECE_OUT
    if (g_audSetHeadMaxDigitalGain)
    {
        regDacLvl = MAX_ABB_DAC_GAIN_HEAD;
    }
    aud_CodecCommonOutAlgGainDb2Val( AlgGain);
#if defined(CHIP_DIE_8809E2)  || defined (CHIP_DIE_8955) || (CHIP_ASIC_ID == CHIP_ASIC_ID_8850E)
    if ( cfg->spkSel != g_audCodecLatestCfg.spkSel)
    {
        if( ((cfg->spkSel == AUD_SPK_EAR_PIECE)|| (cfg->spkSel == AUD_SPK_LOUD_SPEAKER_EAR_PIECE))
                &&((g_audCodecLatestCfg.spkSel != AUD_SPK_EAR_PIECE)&&(g_audCodecLatestCfg.spkSel != AUD_SPK_LOUD_SPEAKER_EAR_PIECE)) )
        {
            hal_AudSetHPMode((cfg->spkSel == AUD_SPK_EAR_PIECE)||(cfg->spkSel == AUD_SPK_LOUD_SPEAKER_EAR_PIECE));
        }
        else if( ((cfg->spkSel != AUD_SPK_EAR_PIECE)&& (cfg->spkSel != AUD_SPK_LOUD_SPEAKER_EAR_PIECE))
                 &&((g_audCodecLatestCfg.spkSel == AUD_SPK_EAR_PIECE)|| (g_audCodecLatestCfg.spkSel == AUD_SPK_LOUD_SPEAKER_EAR_PIECE)) )
        {
            hal_AudSetHPMode((cfg->spkSel == AUD_SPK_EAR_PIECE)||(cfg->spkSel == AUD_SPK_LOUD_SPEAKER_EAR_PIECE));
        }
    }
#endif

    AUD_TRACE(AUD_INFO_TRC, 0,
              "aud_CodecCommonSetup g_audCodecLatestCfg.spkSel=%d  devCfg.spkType=%d",g_audCodecLatestCfg.spkSel, g_audCodecLatestLevel.spkLevel);

    // Check if we need to mute the original audio output channel gradually
    if ( cfg->spkSel != g_audCodecLatestCfg.spkSel ||
            (cfg->level->spkLevel == SND_SPK_MUTE &&
             g_audCodecLatestLevel.spkLevel != SND_SPK_MUTE) )
    {
        switch (g_audCodecLatestCfg.spkSel)
        {
            case AUD_SPK_RECEIVER:
            {
                aud_CodecCommonSetRcvGain(MUTE_ANA_GAIN, MUTE_DAC_GAIN);
                break;
            }
            case AUD_SPK_EAR_PIECE:
            {
                aud_CodecCommonSetHeadGain(MUTE_ANA_GAIN, MUTE_DAC_GAIN);
                break;
            }
            case AUD_SPK_LOUD_SPEAKER:
            {
                aud_CodecCommonSetSpkCfg(TRUE, MUTE_ANA_GAIN, MUTE_DAC_GAIN);
                break;
            }
            case AUD_SPK_LOUD_SPEAKER_EAR_PIECE:
            {
                // Loudspeaker
                aud_CodecCommonSetSpkCfg(TRUE, MUTE_ANA_GAIN, MUTE_DAC_GAIN);
                // Earpiece
                aud_CodecCommonSetHeadGain(MUTE_ANA_GAIN, MUTE_DAC_GAIN);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    // Check if we need to mute the original MIC channel gradually
    if ( cfg->micSel != g_audCodecLatestCfg.micSel ||
            (cfg->level->micLevel == SND_MIC_MUTE &&
             g_audCodecLatestLevel.micLevel != SND_MIC_MUTE) )
    {
        aud_CodecCommonSetMicCfg(TRUE, MUTE_ANA_GAIN, MUTE_ADC_GAIN);
    }
    AUD_TRACE(AUD_INFO_TRC, 0,
              "aud_CodecCommonSetup g_audCodecPlaying=%d  g_audCodecRecording=%d, g_audTonePlaying=%d",g_audCodecPlaying, g_audCodecRecording,g_audTonePlaying);
    if(!g_audCodecPlaying && !g_audCodecRecording && !g_audTonePlaying )
    {
        switch (cfg->spkSel)
        {
            case AUD_SPK_RECEIVER:
            {
                hal_AudSetRcvGain(MUTE_ANA_GAIN);
                break;
            }
            case AUD_SPK_EAR_PIECE:
            {
                hal_AudSetHeadGain(MUTE_ANA_GAIN);
                break;
            }
            case AUD_SPK_LOUD_SPEAKER:
            {
                HAL_AUD_SPK_CFG_T spkCfg;
                spkCfg.noSpkMute = 0;
                spkCfg.spkGain = 0;
                hal_AudSetSpkCfg(spkCfg);
                break;
            }
            case AUD_SPK_LOUD_SPEAKER_EAR_PIECE:
            {
                // Loudspeaker
                HAL_AUD_SPK_CFG_T spkCfg;
                spkCfg.noSpkMute = 0;
                spkCfg.spkGain = 0;
                hal_AudSetSpkCfg(spkCfg);
                // Earpiece
                hal_AudSetHeadGain(MUTE_ANA_GAIN);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    // Enable/disable corresponding audio PAs
    HAL_AUD_SEL_T selCfg;
    selCfg.reg = 0;

    if (g_audCodecCurrentUser == HAL_AUD_USER_FM)
    {
        selCfg.fmSel = TRUE;
    }

    switch (cfg->spkSel)
    {
        case AUD_SPK_RECEIVER:
        {
            if(cfg->level->spkLevel == SND_SPK_MUTE)
            {
                selCfg.rcvSel = FALSE;
            }
            else
            {
                selCfg.rcvSel = TRUE;
            }
            break;
        }
        case AUD_SPK_EAR_PIECE:
        {
            if(cfg->level->spkLevel == SND_SPK_MUTE)
            {
                selCfg.headSel = FALSE;
            }
            else
            {
                selCfg.headSel = TRUE;
            }
            break;
        }
        case AUD_SPK_LOUD_SPEAKER:
        {
            if (cfg->level->spkLevel == SND_SPK_MUTE)
            {
                selCfg.spkSel = FALSE;
            }
            else
            {
                if (cfg->spkType == AUD_SPEAKER_STEREO)
                {
                    selCfg.spkSel = TRUE;
                }
                else
                {
                    selCfg.spkMonoSel = TRUE;
                }
            }
            break;
        }
        case AUD_SPK_LOUD_SPEAKER_EAR_PIECE:
        {
            if (cfg->level->spkLevel == SND_SPK_MUTE)
            {
                // Loudspeaker
                selCfg.spkSel = FALSE;
                // Earpiece
                selCfg.headSel = FALSE;
            }
            else
            {
                // Loudspeaker
                if (cfg->spkType == AUD_SPEAKER_STEREO)
                {
                    selCfg.spkSel = TRUE;
                }
                else
                {
                    selCfg.spkMonoSel = TRUE;
                }
                // Earpiece
                selCfg.headSel = TRUE;
            }
            break;
        }
        default:
        {
            break;
        }
    }

    if ( cfg->micSel == AUD_MIC_RECEIVER ||
            cfg->micSel == AUD_MIC_EAR_PIECE ||
            cfg->micSel == AUD_MIC_LOUD_SPEAKER )
    {
        switch (cfg->micSel)
        {
            case AUD_MIC_EAR_PIECE:
            {
                selCfg.auxMicSel = TRUE;
                break;
            }
            case AUD_MIC_RECEIVER:
            case AUD_MIC_LOUD_SPEAKER:
            {
                break;
            }
            default:
            {
                break;
            }
        }
    }

    if ( cfg->spkSel != g_audCodecLatestCfg.spkSel ||
            cfg->micSel != g_audCodecLatestCfg.micSel ||
            ( cfg->spkSel == AUD_SPK_LOUD_SPEAKER &&
              cfg->spkType != g_audCodecLatestCfg.spkType ) ||
            ( (cfg->level->spkLevel == SND_SPK_MUTE ||
               g_audCodecLatestLevel.spkLevel == SND_SPK_MUTE) &&
              cfg->level->spkLevel != g_audCodecLatestLevel.spkLevel ) )
    {
        hal_AudSetSelCfg(selCfg);
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
#ifdef ABB_HP_DETECT
        // Save the headphone state here. It will be restored when the
        // headphone key detection is disabled due to entering LP mode
        hal_AudSaveHeadphoneState(selCfg.headSel);
#endif
#endif
    }

    // Set audio gains
    switch (cfg->spkSel)
    {
        case AUD_SPK_RECEIVER:
        {
            if(cfg->level->spkLevel == SND_SPK_MUTE)
            {
                aud_CodecCommonSetRcvGain(MUTE_ANA_GAIN, MUTE_DAC_GAIN);
            }
            else
            {
                aud_CodecCommonSetRcvGain(regAnaLvl, regDacLvl);
            }
            break;
        }
        case AUD_SPK_EAR_PIECE:
        {
            if(cfg->level->spkLevel == SND_SPK_MUTE)
            {
                aud_CodecCommonSetHeadGain(MUTE_ANA_GAIN, MUTE_DAC_GAIN);
            }
            else
            {
                aud_CodecCommonSetHeadGain(regAnaLvl, regDacLvl);
            }
            break;
        }
        case AUD_SPK_LOUD_SPEAKER:
        {
            BOOL mute;
            if (cfg->level->spkLevel == SND_SPK_MUTE)
            {
                mute = TRUE;
                regAnaLvl = MUTE_ANA_GAIN;
                regDacLvl = MUTE_DAC_GAIN;
            }
            else
            {
                mute = FALSE;
            }
            aud_CodecCommonSetSpkCfg(mute, regAnaLvl, regDacLvl);
            break;
        }
        case AUD_SPK_LOUD_SPEAKER_EAR_PIECE:
        {
            if (cfg->level->spkLevel == SND_SPK_MUTE)
            {
                // Loudspeaker
                aud_CodecCommonSetSpkCfg(TRUE, MUTE_ANA_GAIN, MUTE_DAC_GAIN);
                // Earpiece
                aud_CodecCommonSetHeadGain(MUTE_ANA_GAIN, MUTE_DAC_GAIN);
            }
            else
            {
                // Loudspeaker
                aud_CodecCommonSetSpkCfg(FALSE, regAnaLvl, regDacLvl);
                // Earpiece
                INT8 epAnaLevel, epDacLevel;
                if (IS_APP_SND_ITF(itf))
                {
                    epAnaLevel = pCalibOutGains->voiceOrEpAna;
                    epDacLevel = pCalibOutGains->voiceOrEpDac;
                }
                else
                {
                    CALIB_AUDIO_OUT_GAINS_T *pEpGains;
                    pEpGains = &calibPtr->bb->audio[SND_ITF_EAR_PIECE].
                               audioGains.outGains[cfg->level->spkLevel];
                    if (g_audCodecVoiceQuality)
                    {
                        epAnaLevel = pEpGains->voiceOrEpAna;
                        epDacLevel = pEpGains->voiceOrEpDac;
                    }
                    else
                    {
                        epAnaLevel = pEpGains->musicOrLsAna;
                        epDacLevel = pEpGains->musicOrLsDac;
                    }
                }
                aud_CodecCommonSetHeadGain(epAnaLevel, epDacLevel);
            }
            break;
        }
        default:
        {
            break;
        }
    }

    if ( cfg->micSel == AUD_MIC_RECEIVER ||
            cfg->micSel == AUD_MIC_EAR_PIECE ||
            cfg->micSel == AUD_MIC_LOUD_SPEAKER )
    {
        BOOL mute = TRUE;
        INT32 anaVol = MUTE_ANA_GAIN;
        INT32 adcVol = MUTE_ADC_GAIN;
        if (cfg->level->micLevel != SND_MIC_MUTE)
        {
            mute = FALSE;

            CALIB_AUDIO_IN_GAINS_T inGains;
            if (cfg->level->micLevel == SND_MIC_VOL_CALL)
            {
                inGains = calibPtr->bb->audio[calibItf].audioGains.inGainsCall;
            }
            else
            {
                inGains = calibPtr->bb->audio[calibItf].audioGains.inGainsRecord;
            }
            anaVol = inGains.ana;
            adcVol = inGains.adc;

            // For test mode AUD_TEST_RECVMIC_IN_EARPIECE_OUT
            if (g_audSetMicMaxGain)
            {
                anaVol = 21;
                adcVol = MAX_ABB_ADC_GAIN;
            }
        }

        aud_CodecCommonSetMicCfg(mute, anaVol, adcVol);
    }

    if (
#ifdef CHIP_HAS_AP
        !g_audCodecAifConfigByAp &&
#endif
        (g_audCodecCurrentUser == HAL_AUD_USER_CODEC ||
         g_audCodecCurrentUser == HAL_AUD_USER_LINEIN_WITH_CODEC
        )
    )
    {
        // Side tone volume controlled in the AIF. Normal use or loop test use.
        if (AUD_SIDE_VOL_TEST == cfg->level->sideLevel)
        {
            // If in loop test mode, the side tone and mic are set to
            // large values (Note: the max values might cause echo or
            // squeaking noise)
            aud_CodecCommonSetMicCfg(FALSE, 18, MAX_ABB_ADC_GAIN - 3);
            hal_AifSetSideTone(10);
        }
        else
        {
            hal_AifSetSideTone(
                hal_AifSideToneGainDb2Val(calibPtr->bb->audio[calibItf].audioGains.sideTone[cfg->level->spkLevel])
            );
        }
    }

    // Record last config
    g_audCodecLatestCfg.spkSel = cfg->spkSel;
    g_audCodecLatestCfg.spkType = cfg->spkType;
    g_audCodecLatestCfg.micSel = cfg->micSel;
    g_audCodecLatestLevel = *(cfg->level);

#if defined(BT_A2DP_ANA_DAC_MUTE)
    if((SND_ITF_LOUD_SPEAKER == itf) && (SND_ITF_BLUETOOTH == aud_GetCurOutputDevice()))
    {
        aud_CodecCommonSetSpkCfg(TRUE, MUTE_ANA_GAIN, MUTE_DAC_GAIN);
    }
    if((SND_ITF_EAR_PIECE== itf) && (SND_ITF_BLUETOOTH == aud_GetCurOutputDevice()))
    {
        aud_CodecCommonSetHeadGain(MUTE_ANA_GAIN, MUTE_DAC_GAIN);
    }
#endif

    return AUD_ERR_NO;
}


// =============================================================================
// aud_CodecCommonStreamStart
// -----------------------------------------------------------------------------
/// The common codes to start the playing of a stream.
///
/// This function  outputs the
/// audio on the audio interface specified as a parameter, on the output selected in
/// the #aud_Setup function. \n
/// In normal operation, when
/// the buffer runs out, the playing will wrap at the beginning. Here, there are two ways
/// you can handle your buffer: HAL can call a user function at the middle or
/// at the end of the playback or, depending
/// on your application, you can simply poll the playing state using the "reached half"
/// and "reached end functions" TODO Ask if needed, and then implement !
///
/// @param stream Stream to play. Handler called at the middle and end of the buffer
/// are defined there.
/// @param cfg The configuration set applied to the audio interface
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command,
///         #AUD_ERR_NO it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_CodecCommonStreamStart(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T *       cfg)
{
    AUD_DEVICE_CFG_T cfgCopy = *cfg;
    AUD_LEVEL_T        levelcfg;

    AUD_ERR_T errStatus = AUD_ERR_NO;
    BOOL needCloseAud = FALSE;

    AUD_TRACE(AUD_INFO_TRC, 0,
              "aud_CodecCommonStreamStart g_audCodecPlaying=%d",g_audCodecPlaying);

    if (g_audCodecPlaying)
    {
        AUD_TRACE(AUD_INFO_TRC, 0,
                  "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
        return AUD_ERR_RESOURCE_BUSY;
    }

    if (!g_audCodecOpened)
    {
        errStatus = aud_CodecCommonOpen(stream->sampleRate, stream->voiceQuality,
                                        (stream->channelNb == HAL_AIF_MONO));

        if (errStatus != AUD_ERR_NO)
        {
            return errStatus;
        }
        else
        {
            needCloseAud = TRUE;
        }
    }

    if (g_audFirstOpen == AUD_FIRST_OPEN_ON && cfg->spkSel == AUD_SPK_EAR_PIECE)
    {
        sxr_Sleep(AUD_EARPIECE_PA_POWER_STABLE_TIME);
        g_audFirstOpen = AUD_FIRST_OPEN_DONE;
    }

    levelcfg.spkLevel =0;
    levelcfg.micLevel = 0;
    levelcfg.sideLevel = 0;
    levelcfg.toneLevel = 0;
    cfgCopy.level = &levelcfg;
    errStatus = aud_CodecCommonSetup(itf, &cfgCopy);

    if (g_audCodecCurrentAifCfgPtr != NULL)
    {
        if (errStatus == AUD_ERR_NO)
        {
#if (VCOM_BAT_LARGE_CAP)
            // If the capacitor connected to the loud speaker is large (larger
            // than 1uF), it might take an extra long time to be charged,
            // and then the audio PA can work. If the sound being played is
            // very short (e.g., camera shutter sound), it might NOT be
            // output via the loud speaker.
            // Workaround: Wait for a while before playing the sound.
            if (itf == SND_ITF_LOUD_SPEAKER)
            {
                sxr_Sleep(300 MS_WAITING);
            }
#endif // VCOM_BAT_LARGE_CAP

            // Send the stream through the IFC
            if (hal_AifPlayStream(stream) != HAL_ERR_NO)
            {

                AUD_TRACE(AUD_INFO_TRC, 0,
                          "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
                errStatus = AUD_ERR_RESOURCE_BUSY;
            }
            errStatus = aud_CodecCommonSetup(itf, cfg);

        }
    } // (g_audCodecCurrentAifCfgPtr != NULL)

    if (errStatus == AUD_ERR_NO)
    {
        needCloseAud = FALSE;
        g_audCodecPlaying = TRUE;
    }

    if (needCloseAud)
    {
        aud_CodecCommonClose();
    }

    return errStatus;
}


// =============================================================================
// aud_CodecCommonStreamStop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback or/and record.
/// But it doesn't free the audio out resource. If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_CodecCommonStreamStop(SND_ITF_T itf)
{
    AUD_TRACE(AUD_INFO_TRC, 0,
              "aud_CodecCommonStreamStop g_audCodecPlaying=%d , g_audCodecOpened=%d",g_audCodecPlaying, g_audCodecOpened);

#if 1
    if (g_audCodecPlaying && g_audCodecOpened)
    {
        switch (g_audCodecLatestCfg.spkSel)
        {
            case AUD_SPK_RECEIVER:
            {
                aud_CodecCommonSetRcvGain(MUTE_ANA_GAIN, MUTE_DAC_GAIN);
                break;
            }
            case AUD_SPK_EAR_PIECE:
            {

                aud_CodecCommonSetHeadGain(MUTE_ANA_GAIN, MUTE_DAC_GAIN);
                break;
            }
            case AUD_SPK_LOUD_SPEAKER:
            {
                aud_CodecCommonSetSpkCfg(TRUE, MUTE_ANA_GAIN, MUTE_DAC_GAIN);
                break;
            }
            case AUD_SPK_LOUD_SPEAKER_EAR_PIECE:
            {
                // Loudspeaker
                aud_CodecCommonSetSpkCfg(TRUE, MUTE_ANA_GAIN, MUTE_DAC_GAIN);
                // Earpiece
                aud_CodecCommonSetHeadGain(MUTE_ANA_GAIN, MUTE_DAC_GAIN);
                break;
            }
        }
    }
#else
    if (g_audCodecOpened)
    {
        // Gradually mute audio channels
        g_audCodecLatestLevel.spkLevel = SND_SPK_MUTE;
        g_audCodecLatestLevel.micLevel = SND_MIC_MUTE;
        g_audCodecLatestLevel.sideLevel = SND_SIDE_MUTE;
        g_audCodecLatestLevel.toneLevel = SND_TONE_0DB;
        aud_CodecCommonSetup(itf, &g_audCodecLatestCfg);
    }
#endif

    if (g_audCodecPlaying)
    {
        if (g_audCodecCurrentAifCfgPtr != NULL)
        {
            hal_AifStopPlay();
        }
        g_audCodecPlaying = FALSE;
    }

    if (g_audCodecRecording)
    {
        if (g_audCodecCurrentAifCfgPtr != NULL)
        {
            hal_AifStopRecord();
        }
        if (g_audCodecMicInput)
        {
            pmd_EnablePower(PMD_POWER_MIC, FALSE);
        }
        g_audCodecRecording = FALSE;
    }

    if (g_audCodecOpened)
    {
#if 1
        // Gradually mute audio channels
        g_audCodecLatestLevel.spkLevel = SND_SPK_MUTE;
        g_audCodecLatestLevel.micLevel = SND_MIC_MUTE;
        g_audCodecLatestLevel.sideLevel = SND_SIDE_MUTE;
        g_audCodecLatestLevel.toneLevel = SND_TONE_0DB;
        aud_CodecCommonSetup(itf, &g_audCodecLatestCfg);
#endif
        // Close audio codec
        return aud_CodecCommonClose();
    }
    else
    {
        return AUD_ERR_NO;
    }
}


// =============================================================================
// aud_CodecCommonStreamRecordStop
// -----------------------------------------------------------------------------
/// This function stops the audio stream record.
/// But it doesn't free the audio out resource. If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_CodecCommonStreamRecordStop(SND_ITF_T itf)
{
    if (g_audCodecRecording)
    {
        if (g_audCodecCurrentAifCfgPtr != NULL)
        {
            hal_AifStopRecord();
        }
        if (g_audCodecMicInput)
        {
            pmd_EnablePower(PMD_POWER_MIC, FALSE);
        }
        g_audCodecRecording = FALSE;
    }

    return AUD_ERR_NO;
}

// =============================================================================
// aud_CodecCommonStreamPause
// -----------------------------------------------------------------------------
/// This function pauses the audio stream playback or/and record.
/// But it doesn't free the audio out resource. If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param pause If \c TRUE, pauses the stream. If \c FALSE, resumes a paused
/// stream.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_CodecCommonStreamPause(SND_ITF_T itf, BOOL pause)
{
    if (g_audCodecCurrentAifCfgPtr != NULL)
    {
        if (hal_AifPause(pause) == HAL_ERR_NO)
        {
            return AUD_ERR_NO;
        }
    }

    return AUD_ERR_NO;
}


// =============================================================================
// aud_CodecCommonStreamRecord
// -----------------------------------------------------------------------------
/// The common codes to manage the recording of a stream.
///
///
/// The #aud_StreamRecord send a message to the driver which begins the
/// dataflow from the audio and the compression. If the function returns \c
/// FALSE, it means that the driver cannot handle the record. The value \c len
/// should be the maximum size of the recording buffer and when the buffer is
/// full, the recording will automatically wrap. A
/// user function can be called, either when the middle or the end of the
/// buffer is reached. (TODO add iatus about polling if available)
///
/// @param stream Stream recorded (describe the buffer and the possible interrupt
/// of middle-end buffer handlers.
/// @param cfg The configuration set applied to the audio interface.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver cannot handle the stream,
///         #AUD_ERR_NO if the stream ca be recorded.
// =============================================================================
PUBLIC AUD_ERR_T aud_CodecCommonStreamRecord(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T *       cfg)
{
    AUD_ERR_T errStatus = AUD_ERR_NO;
    BOOL needCloseAud = FALSE;

    AUD_TRACE(AUD_INFO_TRC, 0,
              "aud_CodecCommonStreamRecord g_audCodecRecording=%d",g_audCodecRecording);

    AUD_TRACE(AUD_INFO_TRC, 0,
              "aud_CodecCommonStreamRecord g_audCodecOpened=%d",g_audCodecOpened);


    if (g_audCodecRecording)
    {
        AUD_TRACE(AUD_INFO_TRC, 0,
                  "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
        return AUD_ERR_RESOURCE_BUSY;
    }

    if (!g_audCodecOpened)
    {
        errStatus = aud_CodecCommonOpen(stream->sampleRate, stream->voiceQuality,
                                        (stream->channelNb == HAL_AIF_MONO));

        if (errStatus != AUD_ERR_NO)
        {
            return errStatus;
        }
        else
        {
            needCloseAud = TRUE;
        }
    }
    else if(!g_audAIFOpened)
    {
        errStatus = aud_CodecCommonAifConfig();
        if (errStatus != AUD_ERR_NO)
        {
            return errStatus;
        }
    }

    // when aud_StreamRecord called after aud_StreamStart,
    //     g_audCodecSampleRate is set when play, so it is out samplerate
    // when aud_StreamRecord callled singly, g_audCodecSampleRate is set as dac sample rate,
    //     at the same time, g_audCodecSampleRate  will be treated as adc sample rate
    hal_AudSetAdcSampleRate(g_audCodecSampleRate, stream->sampleRate, g_audCodecPlaying);
    if(g_audCodecPlaying)
        hal_AIFSetTxStb(g_audCodecSampleRate);

    errStatus = aud_CodecCommonSetup(itf, cfg);

    if (g_audCodecCurrentAifCfgPtr != NULL)
    {
        if (errStatus == AUD_ERR_NO)
        {
            // Send the stream through the IFC
            if (hal_AifRecordStream(stream) != HAL_ERR_NO)
            {
                AUD_TRACE(AUD_INFO_TRC, 0,
                          "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
                errStatus = AUD_ERR_RESOURCE_BUSY;
            }
        }
    }

    if (errStatus == AUD_ERR_NO)
    {
        if (g_audCodecMicInput)
        {
            pmd_EnablePower(PMD_POWER_MIC, TRUE);
        }
        needCloseAud = FALSE;
        g_audCodecRecording = TRUE;
    }

    if (needCloseAud)
    {
        aud_CodecCommonClose();
    }

    return errStatus;
}


// =============================================================================
// aud_CodecCommonTone
// -----------------------------------------------------------------------------
//  Manage the playing of a tone: DTMF or Comfort Tone.
///
/// Outputs a DTMF or comfort tone
///
/// When the audio output is enabled, a DTMF or a comfort tones can be output
/// as well. This function starts the output of a tone generated in the audio
/// module. \n
/// You can call this function several times without calling the
/// #aud_ToneStop function or the #aud_TonePause function in
/// between, if you just need to change the attenuation or the tone type. \n
/// If the function returns #AUD_ERR_RESOURCE_BUSY, it means that the driver is
/// busy with an other audio command.
///
/// @param tone The tone to generate
/// @param cfg The configuration set applied to the audio interface
/// @param start If \c TRUE, starts the playing of the tone.
///              If \c FALSE, stops the tone.
///
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_CodecCommonTone(
    SND_ITF_T itf,
    CONST SND_TONE_TYPE_T        tone,
    CONST AUD_DEVICE_CFG_T*             cfg,
    CONST BOOL                   start)
{
    AUD_ERR_T errStatus = AUD_ERR_NO;
    BOOL needCloseAud = FALSE;

#ifdef CHIP_HAS_AP
    if (g_audCodecAifConfigByAp)
    {
        return AUD_ERR_NO;
    }
#endif

    if (g_audCodecCurrentUser != HAL_AUD_USER_NULL &&
            g_audCodecCurrentUser != HAL_AUD_USER_CODEC &&
            g_audCodecCurrentUser != HAL_AUD_USER_LINEIN_WITH_CODEC &&
            g_audCodecCurrentUser != HAL_AUD_USER_AIF_ONLY&&
            (itf == SND_ITF_BLUETOOTH  || itf == SND_ITF_FM)
       )
    {
        // The tone cannot be played unless the user is null (for tone in idle screen) or codec.
        return AUD_ERR_NO;
    }

    if (!g_audCodecOpened && start)
    {
        // Open the codec
        // FIXME Use the stream phantom !
        if (!aud_CodecCommonSetConfig(FALSE, HAL_AUD_USER_CODEC, &g_audCodecToneAifCfg))
        {
            return AUD_ERR_RESOURCE_BUSY;
        }
        errStatus = aud_CodecCommonOpen(HAL_AIF_FREQ_8000HZ, TRUE, TRUE);
        needCloseAud = TRUE;
    }

    if (start)
    {
        if (errStatus == AUD_ERR_NO)
        {
            errStatus = aud_CodecCommonSetup(itf, cfg);

            if (errStatus == AUD_ERR_NO)
            {
                hal_AifTone(tone, cfg->level->toneLevel, TRUE);
                needCloseAud = FALSE;
                g_audTonePlaying = TRUE;
            }
        }

        if (needCloseAud)
        {
            aud_CodecCommonClose();
        }
    }
    else
    {
        // Stop tone anyway
        hal_AifTone(0, 0, FALSE);
        g_audTonePlaying = FALSE;

        if(!g_audCodecPlaying && !g_audCodecRecording)
        {
            // Gradually mute audio channels
            g_audCodecLatestLevel.spkLevel = SND_SPK_MUTE;
            g_audCodecLatestLevel.micLevel = SND_MIC_MUTE;
            g_audCodecLatestLevel.sideLevel = SND_SIDE_MUTE;
            g_audCodecLatestLevel.toneLevel = SND_TONE_0DB;
            aud_CodecCommonSetup(itf, &g_audCodecLatestCfg);
            // Close audio codec
            aud_CodecCommonClose();
        }
    }

    return errStatus;
}


// =============================================================================
// aud_CodecCommonTonePause
// -----------------------------------------------------------------------------
/// This function pauses the audio tone.
/// But it doesn't free the audio out resource. If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param pause If \c TRUE, pauses the tone. If \c FALSE, resumes a paused
/// tone.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_CodecCommonTonePause(SND_ITF_T itf, BOOL pause)
{
#ifdef CHIP_HAS_AP
    if (g_audCodecAifConfigByAp)
    {
        return AUD_ERR_NO;
    }
#endif

    switch (hal_AifTonePause(pause))
    {
        case HAL_ERR_NO:
        default:
            return AUD_ERR_NO;
            break;
    }
}


// =============================================================================
// aud_CodecCommonCalibUpdateValues
// -----------------------------------------------------------------------------
/// Update values depending on calibration parameters.
///
/// @param itf AUD interface on which to stop the tone.
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_CodecCommonCalibUpdateValues(SND_ITF_T itf)
{
    // FIXME --> Open the i2c before ...
    return HAL_ERR_NO;
}


#ifdef CHIP_HAS_AP
// =============================================================================
// aud_CodecCommonSetAifConfigByAp
// -----------------------------------------------------------------------------
/// Set a flag to tell whether AIF should be configured by AP.
///
/// @param on Whether AIF should be configured by AP.
// =============================================================================
PUBLIC VOID aud_CodecCommonSetAifConfigByAp(BOOL on)
{
    g_audCodecAifConfigByAp = on;
}
#endif

PUBLIC VOID aud_CodecCommonSetMicLevel(SND_ITF_T itf, BOOL isIncall, SND_SPK_LEVEL_T level )
{
    INT32 anaVol = MUTE_ANA_GAIN;
    INT32 adcVol = MUTE_ADC_GAIN;
    CALIB_AUDIO_IN_GAINS_T inGains;
    CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();

    if (isIncall)
    {
        inGains = calibPtr->bb->audio[itf].audioGains.inGainsCall;
    }
    else
    {
        inGains = calibPtr->bb->audio[itf].audioGains.inGainsRecord;
    }

    anaVol = inGains.ana;
#if (AUDIO_CALIB_VER == 1)
    adcVol =-13+level*2;
#else
    adcVol =-13+level*4;
#endif
	 g_audCodecLatestMicLevel = level;

    if(SND_SPK_MUTE == level)
        aud_CodecCommonSetMicCfg(TRUE, MUTE_ANA_GAIN, MUTE_ADC_GAIN);
    else
        aud_CodecCommonSetMicCfg(FALSE, anaVol, adcVol);

}

PUBLIC SND_SPK_LEVEL_T aud_CodecCommonGetCalibMicLevel(SND_ITF_T itf, BOOL isIncall)
{
        INT32 anaVol = MUTE_ANA_GAIN;
        INT32 adcVol = MUTE_ADC_GAIN;
        CALIB_AUDIO_IN_GAINS_T inGains;
        CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();

       if (isIncall)
       {
           inGains = calibPtr->bb->audio[itf].audioGains.inGainsCall;
       }
       else
       {
           inGains = calibPtr->bb->audio[itf].audioGains.inGainsRecord;
       }

	anaVol = inGains.ana;
       #if (AUDIO_CALIB_VER == 1)
	return (adcVol +13)/2;
	#else
	return (adcVol +13)/4;
	#endif
}

PUBLIC SND_SPK_LEVEL_T aud_CodecCommonGetMicLevel()
{
    return g_audCodecLatestMicLevel;
}

PUBLIC VOID aud_CodecCommonSetSidetoneLevel(SND_ITF_T itf,     SND_SPK_LEVEL_T spkLevel )
{
        CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
        hal_AifSetSideTone(
                hal_AifSideToneGainDb2Val(calibPtr->bb->audio[itf].audioGains.sideTone[spkLevel]));

        g_audCodecLatestLevel.sideLevel = spkLevel;
}

PUBLIC SND_SPK_LEVEL_T aud_CodecCommonGetSidetoneLevel( )
{
     return g_audCodecLatestLevel.sideLevel;
}
// =============================================================================
// aud_CodecCommonGetLatestLevel
// -----------------------------------------------------------------------------
/// get the audio codec Latest level.
// =============================================================================
PUBLIC AUD_LEVEL_T* aud_CodecCommonGetLatestLevel(VOID)
{
    return (&g_audCodecLatestLevel);
}

// =============================================================================
// aud_CodecCommonGetOutAlgGainDb2Val
// -----------------------------------------------------------------------------
/// get the audio codec AlgGainDb2Val.
// =============================================================================
PUBLIC INT16 aud_CodecCommonGetOutAlgGainDb2Val(VOID)
{
    return g_audCodecOutAlgGainDb2Val;
}

PUBLIC BOOL aud_CodecCommonGetVoiceQuality(VOID)
{
    return g_audCodecVoiceQuality;
}

PUBLIC BOOL aud_CodecCommonGetBtNRECFlag(VOID)
{
#ifdef CHIP_HAS_AP
    return g_audCodecBtNRECFlag;
#else
#ifdef BT_BTHSNREC_SUPPORT
    if(rdabt_get_NREC_val())
        return 1;
    else
        return 0;
#else
    return 0;
#endif
#endif
}

#ifdef CHIP_HAS_AP
PUBLIC VOID aud_CodecCommonSetBtNRECFlag(BOOL flag)
{
    g_audCodecBtNRECFlag = flag;
}
#endif

// =============================================================================
//aud_CodecCommonGetForceReceiverMicSelectionFlag
// -----------------------------------------------------------------------------
/// get the audio codec ForceReceiverMicSelectionFlag.
// =============================================================================
PUBLIC BOOL aud_CodecCommonGetForceReceiverMicSelectionFlag(VOID)
{
    return hal_GetAudForceReceiverMicSelectionFlag();
}

#ifdef CHIP_DIE_8955
PUBLIC VOID aud_CodecCommonEnFMRecNewPath(BOOL en)
{
    g_audCodecEnFMRecNew = en;
}
#endif


