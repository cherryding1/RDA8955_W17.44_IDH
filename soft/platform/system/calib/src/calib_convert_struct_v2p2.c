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
#include "chip_id.h"

#include "snd_types.h"
#include "calib_m.h"
#include "hal_rda_abb.h"


// =============================================================================
// AUD_CODEC_CALIB_OUT_GAINS_V2P2_T
// -----------------------------------------------------------------------------
/// This type describes how the gains are recorded in the audio calibration
/// structure for the Analog AUD driver.
// =============================================================================
typedef union
{
    /// The value read from the calibration structure.
    UINT32  calibValue;

    /// Calibrated gains for receiver, earpiece, loud speaker
    struct
    {
        /// Calibrated gains for a voice flow.
        /// Right gain on the stereo macro for the voice flow. (Only for LS or HS).
        UINT32  voiceRight:5;
        /// Left gain on the stereo macro for the voice flow (Only for LS or HS)..
        UINT32  voiceLeft:5;
        /// Preamplifier of the voice flow before the stereo macro (Only for LS or HS)..
        UINT32  voicePreamp:3;
        /// Analog gain of the voice flow.
        UINT32  voiceAnalog:3;
        /// Digital gain of the voice flow.
        UINT32  voiceDigital:2;

        /// Calibrated gains for a music flow.
        /// Right gain on the stereo macro for the music flow
        UINT32  musicRight:5;
        /// Left gain on the stereo macro for the music flow
        UINT32  musicLeft:5;
        /// Digital gain on the stereo macro for the music flow
        UINT32  musicDigital:3;
        /// Stuff bit.
        UINT32  :1;
    } phySpkGains;

    /// Calibrated gains for FM, TV
    struct
    {
        /// Calibrated gains for earpiece
        /// Right gain on the stereo macro
        UINT32  epRight:5;
        /// Left gain on the stereo macro
        UINT32  epLeft:5;
        /// Digital gain on the stereo macro
        UINT32  epDigital:3;
        /// Stuff bit
        UINT32  :5;

        /// Calibrated gains for loud speaker
        /// Right gain on the stereo macro
        UINT32  lsRight:5;
        /// Left gain on the stereo macro
        UINT32  lsLeft:5;
        /// Digital gain on the stereo macro
        UINT32  lsDigital:3;
        /// Stuff bit
        UINT32  :1;
    } appGains;
} AUD_CODEC_CALIB_OUT_GAINS_V2P2_T;

// ============================================================================
// CALIB_AUDIO_GAINS_V2P2_T
// -----------------------------------------------------------------------------
/// Calib audio gain types.
// =============================================================================
typedef struct
{
    /// Params accessible by the API.
    UINT32                         inGain;                       //0x00000000
    AUD_CODEC_CALIB_OUT_GAINS_V2P2_T outGain[CALIB_AUDIO_GAIN_QTY]; //0x00000004
    UINT32                         sideTone[CALIB_AUDIO_GAIN_QTY]; //0x00000024
    UINT32                         amplGain[CALIB_AUDIO_GAIN_QTY]; //0x00000044
} CALIB_AUDIO_GAINS_V2P2_T; //Size : 0x64

// ============================================================================
// CALIB_AUDIO_PARAMS_V2P2_T
// -----------------------------------------------------------------------------
/// Audio calibration parameters.
// =============================================================================
typedef struct
{
    UINT32                        MicDigitalGainInVoC;          //0x00000000
    INT8                           AecEnbleFlag;                 //0x00000004
    INT8                           AgcEnbleFlag;           //0x00000005
    INT8                           StrongEchoFlag;         //0x00000006
    INT8                           reserved_02;  //0x00000007
    INT8                           NoiseGainLimit;               //0x00000008
    INT8                           NoiseMin;                     //0x00000009
    INT8                           NoiseGainLimitStep;           //0x0000000A
    INT8                           AmpThr;                       //0x0000000B
    INT8                           HighPassFilterFlag;           //0x0000000C
    INT8                           NotchFilterFlag;              //0x0000000D
    INT8                           NoiseSuppresserFlag;          //0x0000000E
    INT8                           NoiseSuppresserWithoutSpeechFlag; //0x0000000F
    UINT32                         AudioParams[CALIB_AUDIO_PARAM_QTY-4]; //0x00000010
} CALIB_AUDIO_PARAMS_V2P2_T; //Size : 0x20


PRIVATE INT8 calib_MicAlgVol2GainV2p2(UINT32 vol)
{
    //    { 0x40, "0dB"},  { 0x5a, "3dB"},  { 0x80, "6dB"},  { 0xb5, "9dB"},
    //    { 0x100, "12dB"},{ 0x16a, "15dB"},{ 0x200, "18dB"},{ 0x2d4, "21dB"},
    //    { 0x400, "24dB"},{ 0x5a8, "27dB"},{ 0x800, "30dB"},{ 0xb50, "33dB"},
    //    { 0x1000,"36dB"},{ 0x16a0,"39dB"},{ 0x2000,"42dB"},{ 0x2d40,"45dB"}
    INT8 gain;

    if (vol <= 0x40) gain = 0;
    else if (vol <= 0x5a) gain = 3;
    else if (vol <= 0x80) gain = 6;
    else if (vol <= 0xb5) gain = 9;
    else if (vol <= 0x100) gain = 12;
    else if (vol <= 0x16a) gain = 15;
    else if (vol <= 0x200) gain = 18;
    else if (vol <= 0x2d4) gain = 21;
    else if (vol <= 0x400) gain = 24;
    else if (vol <= 0x5a8) gain = 27;
    else if (vol <= 0x800) gain = 30;
    else if (vol <= 0xb50) gain = 33;
    else if (vol <= 0x1000) gain = 36;
    else if (vol <= 0x16a0) gain = 39;
    else if (vol <= 0x2000) gain = 42;
    else gain = 45;
    return gain;
}

PRIVATE INT8 calib_SideToneVol2GainV2p2(UINT32 vol)
{
    if (vol == 0)
        return -128;
    if (vol > 15)
        vol = 15;
    return -36+(vol-1)*3;
}

PUBLIC VOID calib_ConvertStructV2p2(CALIB_BB_T *pCalibBb)
{
    // 1) Algorithm gain
    // 2) ADC gain
    // 3) DAC gain
    // 4) Sidetone gain

    UINT32 itf, level;
    CALIB_AUDIO_GAINS_T *pAudioGains;
    CALIB_AUDIO_IN_GAINS_T *pInGains;
    CALIB_AUDIO_OUT_GAINS_T *pOutGains;
    CALIB_AUDIO_GAINS_V2P2_T audioGainsV2p2;
    AUD_CODEC_CALIB_OUT_GAINS_V2P2_T *pOutGainsV2p2;

    UINT32 MicDigitalGainInVoC;
    UINT32 voiceReducedLevel, musicReducedLevel;
    BOOL gotVoiceLevel, gotMusicLevel;
    INT8 dac, adjustedDac;

    for (itf=0; itf<CALIB_AUDIO_ITF_QTY; itf++)
    {
        pAudioGains = &pCalibBb->audio[itf].audioGains;
        pInGains = &pAudioGains->inGainsCall;
        pOutGains = pAudioGains->outGains;

        audioGainsV2p2 = *(CALIB_AUDIO_GAINS_V2P2_T *)pAudioGains;
        pOutGainsV2p2 = audioGainsV2p2.outGain;

        // Save original input algorithm gain
        MicDigitalGainInVoC = ((CALIB_AUDIO_PARAMS_V2P2_T *)
                               &pCalibBb->audio[itf].audioParams)->MicDigitalGainInVoC;
        pCalibBb->audio[itf].audioParams.reserv1 = 0;

        // Convert input gains for all interfaces, including
        // BT/FM/TV (e.g., FM line-in recording ...)
        audioGainsV2p2.inGain &= 0xff;
        pInGains->ana = (INT8)(audioGainsV2p2.inGain/2*3);
        pInGains->adc = DEFAULT_ABB_ADC_GAIN;
        pInGains->alg = calib_MicAlgVol2GainV2p2(MicDigitalGainInVoC);
        pInGains->reserv = 0;

        // Convert sidetone gains
        for (level=0; level<CALIB_AUDIO_GAIN_QTY; level++)
        {
            pAudioGains->sideTone[level] =
                calib_SideToneVol2GainV2p2(audioGainsV2p2.sideTone[level]);
        }

        if (itf == SND_ITF_RECEIVER ||
                itf == SND_ITF_EAR_PIECE ||
                itf == SND_ITF_LOUD_SPEAKER)
        {
            if (itf == SND_ITF_RECEIVER)
            {
                dac = DEFAULT_ABB_DAC_GAIN_RECEIVER;
            }
            else if (itf == SND_ITF_EAR_PIECE)
            {
                dac = DEFAULT_ABB_DAC_GAIN_HEAD;
            }
            else if (itf == SND_ITF_LOUD_SPEAKER)
            {
                dac = DEFAULT_ABB_DAC_GAIN_SPEAKER;
            }

            voiceReducedLevel = 0;
            musicReducedLevel = 0;
            gotVoiceLevel = FALSE;
            gotMusicLevel = FALSE;
            // Level 0 is for mute, and level 1 is the real min gain
            for (level=CALIB_AUDIO_GAIN_QTY-1; level>1; level--)
            {
                if (!gotVoiceLevel && pOutGainsV2p2[level].phySpkGains.voiceLeft == 0)
                {
                    voiceReducedLevel = level;
                    gotVoiceLevel = TRUE;
                }
                if (!gotMusicLevel && pOutGainsV2p2[level].phySpkGains.musicLeft == 0)
                {
                    musicReducedLevel = level;
                    gotMusicLevel = TRUE;
                }
            }

            for (level=0; level<CALIB_AUDIO_GAIN_QTY; level++)
            {
                pOutGains[level].reserv1 = 0;
                pOutGains[level].reserv2 = 0;
                // Set voice analog gain
                pOutGains[level].voiceOrEpAna = (INT8)pOutGainsV2p2[level].phySpkGains.voiceLeft/2*3;
                // Set voice algorithm gain
                pOutGains[level].voiceOrEpAlg = (INT8)pOutGainsV2p2[level].phySpkGains.voiceDigital*3;
                // Set voice DAC value
                adjustedDac = dac;
                // Level 0 is for mute
                if (level < voiceReducedLevel && level != 0)
                {
                    adjustedDac = dac - 3*(voiceReducedLevel - level);
                }
                pOutGains[level].voiceOrEpDac = adjustedDac;

                // Set music analog gain
                pOutGains[level].musicOrLsAna = (INT8)pOutGainsV2p2[level].phySpkGains.musicLeft/2*3;
                // Set music algorithm gain
                pOutGains[level].musicOrLsAlg = (INT8)pOutGainsV2p2[level].phySpkGains.musicDigital*3;
                // Set music DAC value
                adjustedDac = dac;
                // Level 0 is for mute
                if (level < musicReducedLevel && level != 0)
                {
                    adjustedDac = dac - 3*(musicReducedLevel - level);
                }
                pOutGains[level].musicOrLsDac = adjustedDac;
            }
        } // end of if (itf ...
        else if (itf == SND_ITF_BLUETOOTH)
        {
            // For BT interface, DAC gains are useless
            for (level=0; level<CALIB_AUDIO_GAIN_QTY; level++)
            {
                pOutGains[level].reserv1 = 0;
                pOutGains[level].reserv2 = 0;
                pOutGains[level].voiceOrEpAna = (INT8)pOutGainsV2p2[level].phySpkGains.voiceLeft/2*3;
                pOutGains[level].voiceOrEpDac = 0;
                pOutGains[level].voiceOrEpAlg = 0;
                pOutGains[level].musicOrLsAna = (INT8)pOutGainsV2p2[level].phySpkGains.musicLeft/2*3;
                pOutGains[level].musicOrLsDac = 0;
                pOutGains[level].musicOrLsAlg = 0;
            }
        }
        else // if (itf == SND_ITF_FM || itf == SND_ITF_TV)
        {
            // For FM/TV interfaces:
            // - I2S output will make use of DAC gains
            // - Reduced levels are not supported
            for (level=0; level<CALIB_AUDIO_GAIN_QTY; level++)
            {
                pOutGains[level].reserv1 = 0;
                pOutGains[level].reserv2 = 0;
                // Set earpiece analog gain
                pOutGains[level].voiceOrEpAna = (INT8)pOutGainsV2p2[level].appGains.epLeft/2*3;
                // Set earpiece DAC gain
                pOutGains[level].voiceOrEpDac = (INT8)DEFAULT_ABB_DAC_GAIN_HEAD;
                // Set earpiece algorithm gain
                pOutGains[level].voiceOrEpAlg = 0;
                // Set loudspeaker analog gain
                pOutGains[level].musicOrLsAna = (INT8)pOutGainsV2p2[level].appGains.lsLeft/2*3;
                // Set loudspeaker DAC gain
                pOutGains[level].musicOrLsDac = (INT8)DEFAULT_ABB_DAC_GAIN_SPEAKER;
                // Set loudspeaker algorithm gain
                pOutGains[level].musicOrLsAlg = 0;
            }
        }
    } // end of for (itf ...
}


