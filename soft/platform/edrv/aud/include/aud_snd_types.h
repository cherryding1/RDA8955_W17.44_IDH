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


#ifndef _AUD_SND_TYPES_H_
#define _AUD_SND_TYPES_H_

// =============================================================================
// AUD_SPK_LEVEL_T
// -----------------------------------------------------------------------------
/// This type describes the possible level value for the  speaker volume.
// =============================================================================
#ifdef CHIP_HAS_AP
typedef enum
{
    AUD_SPK_MUTE = 0,
    AUD_SPK_VOL_1,
    AUD_SPK_VOL_2,
    AUD_SPK_VOL_3,
    AUD_SPK_VOL_4,
    AUD_SPK_VOL_5,
    AUD_SPK_VOL_6,
    AUD_SPK_VOL_7,

    AUD_SPK_VOL_QTY
} AUD_SPK_LEVEL_T;
#else
typedef enum
{
    AUD_SPK_MUTE = 0,
    AUD_SPK_VOL_1,
    AUD_SPK_VOL_2,
    AUD_SPK_VOL_3,
    AUD_SPK_VOL_4,
    AUD_SPK_VOL_5,
    AUD_SPK_VOL_6,
    AUD_SPK_VOL_7,
    AUD_SPK_VOL_8,
    AUD_SPK_VOL_9,
    AUD_SPK_VOL_10,
    AUD_SPK_VOL_11,
    AUD_SPK_VOL_12,
    AUD_SPK_VOL_13,
    AUD_SPK_VOL_14,
    AUD_SPK_VOL_15,
    AUD_SPK_VOL_QTY
} AUD_SPK_LEVEL_T;
#endif

// =============================================================================
// AUD_MIC_LEVEL_T
// -----------------------------------------------------------------------------
/// This type describes the possible level value for the mic: mute or enabled, volume
/// in call, volume in recording.
// =============================================================================
typedef enum
{
    AUD_MIC_MUTE = 0,
    AUD_MIC_ENABLE,
    AUD_MIC_VOL_CALL = AUD_MIC_ENABLE,
    AUD_MIC_VOL_RECORD,

    AUD_MIC_VOL_QTY
} AUD_MIC_LEVEL_T;




// =============================================================================
// AUD_SIDE_LEVEL_T
// -----------------------------------------------------------------------------
/// This type describes the possible level value for the side tone volume.
/// The value AUD_SIDE_VOL_TEST is used for mic to spk simple loop back test.
// =============================================================================
#ifdef CHIP_HAS_AP
typedef enum
{
    AUD_SIDE_MUTE = 0,
    AUD_SIDE_VOL_1,
    AUD_SIDE_VOL_2,
    AUD_SIDE_VOL_3,
    AUD_SIDE_VOL_4,
    AUD_SIDE_VOL_5,
    AUD_SIDE_VOL_6,
    AUD_SIDE_VOL_7,
    AUD_SIDE_VOL_TEST,

    AUD_SIDE_VOL_QTY
} AUD_SIDE_LEVEL_T;
#else
typedef enum
{
    AUD_SIDE_MUTE = 0,
    AUD_SIDE_VOL_1,
    AUD_SIDE_VOL_2,
    AUD_SIDE_VOL_3,
    AUD_SIDE_VOL_4,
    AUD_SIDE_VOL_5,
    AUD_SIDE_VOL_6,
    AUD_SIDE_VOL_7,
    AUD_SIDE_VOL_8,
    AUD_SIDE_VOL_9,
    AUD_SIDE_VOL_10,
    AUD_SIDE_VOL_11,
    AUD_SIDE_VOL_12,
    AUD_SIDE_VOL_13,
    AUD_SIDE_VOL_14,
    AUD_SIDE_VOL_15,
    AUD_SIDE_VOL_TEST,

    AUD_SIDE_VOL_QTY
} AUD_SIDE_LEVEL_T;
#endif

// =============================================================================
// AUD_TONE_ATTENUATION_T
// -----------------------------------------------------------------------------
/// Attenuation of the tone.
///
/// The attenuation can be set to 0 dB, -3 dB, -9 dB and -15dB.
// =============================================================================
typedef enum
{
    /// No attenuation
    AUD_TONE_0dB,
    /// -3dB
    AUD_TONE_m3dB,
    /// -9db
    AUD_TONE_m9dB,
    /// -15dB
    AUD_TONE_m15dB,

    AUD_TONE_VOL_QTY
} AUD_TONE_ATTENUATION_T;



// =============================================================================
// AUD_TONE_TYPE_T
// -----------------------------------------------------------------------------
/// Tone types.
/// The DTMF Tones are used to inform the user that the number is being
/// composed. All the standard DTMF are available: 0 to 9, A to D,
/// pound and star. \n
/// The Comfort Tones are used to inform the user on the current state of
/// the call: Ringing, Busy, Unavailable... All frequencies needed to do
/// the standard Comfort Tones are available: 425 Hz, 950 Hz, 1400 Hz and
/// 1800 Hz. \n
// =============================================================================
typedef enum
{
    /// Tone when the '0' key
    AUD_DTMF_0,
    /// Tone when the '1' key
    AUD_DTMF_1,
    /// Tone when the '2' key
    AUD_DTMF_2,
    /// Tone when the '3' key
    AUD_DTMF_3,
    /// Tone when the '4' key
    AUD_DTMF_4,
    /// Tone when the '5' key
    AUD_DTMF_5,
    /// Tone when the '6' key
    AUD_DTMF_6,
    /// Tone when the '7' key
    AUD_DTMF_7,
    /// Tone when the '8' key
    AUD_DTMF_8,
    /// Tone when the '9' key
    AUD_DTMF_9,
    AUD_DTMF_A,
    AUD_DTMF_B,
    AUD_DTMF_C,
    AUD_DTMF_D,
    /// Tone when the * key
    AUD_DTMF_S,
    /// Tone when the # key
    AUD_DTMF_P,
    /// Comfort tone at 425 Hz
    AUD_COMFORT_425,
    /// Comfort tone at 950 Hz
    AUD_COMFORT_950,
    /// Comfort tone at 1400 Hz
    AUD_COMFORT_1400,
    /// Confort tone at 1800 Hz
    AUD_COMFORT_1800,
    /// No tone is emitted
    AUD_NO_TONE
} AUD_TONE_TYPE_T;


// =============================================================================
// AUD_ITF_T
// -----------------------------------------------------------------------------
/// That type provide a way to identify the different audio interface.
// =============================================================================
typedef enum
{
    AUD_ITF_RECEIVER = 0,
    AUD_ITF_EAR_PIECE,
    AUD_ITF_LOUD_SPEAKER,
    AUD_ITF_BLUETOOTH,
    AUD_ITF_FM,
    AUD_ITF_TV,

    /// Number (max) of available interface to the AUD driver
    AUD_ITF_QTY,
    AUD_ITF_NONE = 0xFF

} AUD_ITF_T;

/*add some scenario book here */
typedef struct
{
    AUD_TONE_TYPE_T tone;
    UINT32  tone_term;
} cii_DTMF_term, *cii_DTMF_pterm;

typedef struct
{
    UINT32  term_num;
    cii_DTMF_pterm DTMF_term;
} cii_DTMF_tone, *cii_DTMF_ptone;

#endif
