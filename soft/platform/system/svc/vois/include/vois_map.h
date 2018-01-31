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


#ifndef _VOIS_MAP_H_
#define _VOIS_MAP_H_



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// VOIS_MAP_AUDIO_CFG_T
// -----------------------------------------------------------------------------
/// Configuration structure. A configuration structure allows to record a stream
/// with the proper configuration set as far as the audio interface and the decoding
/// are concerned.
// =============================================================================
typedef struct
{
    /// Type of speaker
    UINT32                         spkSel;                       //0x00000000
    /// Type of speaker
    UINT32                         micSel;                       //0x00000004
    /// Speaker level.
    UINT32                         spkLevel;                     //0x00000008
    /// Mic level
    UINT32                         micLevel;                     //0x0000000C
    /// Side tone
    UINT32                         sideLevel;                    //0x00000010
    /// Tone level
    UINT32                         toneLevel;                    //0x00000014
    /// encoder gain
    INT16                          encMute;                      //0x00000018
    /// decoder gain
    INT16                          decMute;                      //0x0000001A
} VOIS_MAP_AUDIO_CFG_T; //Size : 0x1C



// ============================================================================
// AUD_MAP_LEVEL_T
// -----------------------------------------------------------------------------
/// Configuration structure. A configuration structure allows to start an AUD operation
/// (start stream, start record, or start tone) with the desired parameters.
// =============================================================================
typedef struct
{
    /// Speaker level,
    UINT32                         spkLevel;                     //0x00000000
    /// Microphone level: muted or enabled
    UINT32                         micLevel;                     //0x00000004
    /// Sidetone
    UINT32                         sideLevel;                    //0x00000008
    UINT32                         toneLevel;                    //0x0000000C
} AUD_MAP_LEVEL_T; //Size : 0x10



// ============================================================================
// VPP_MAP_SPEECH_AUDIO_CFG_T
// -----------------------------------------------------------------------------
/// VPP Speech audio configuration for echo cancellation, encoder gain and anti-distorsion
/// filter, decoder gain and anti-distorsion filter.
// =============================================================================
typedef struct
{
    /// enable the echo suppresor
    INT16                          echoEsOn;                     //0x00000000
    /// echo suppresor voice activity detection threshold
    INT16                          echoEsVad;                    //0x00000002
    /// echo suppresor double-talk detection threshold
    INT16                          echoEsDtd;                    //0x00000004
    /// echo canceller relative threshold
    INT16                          echoExpRel;                   //0x00000006
    /// echo canceller mu paramater (NULL to disable the echo canceller)
    INT16                          echoExpMu;                    //0x00000008
    /// echo canceller minimum threshold
    INT16                          echoExpMin;                   //0x0000000A
    /// encoder gain
    INT16                          encMute;                      //0x0000000C
    /// decoder gain
    INT16                          decMute;                      //0x0000000E
    /// pointer to the speaker anti-distorsion filter (SDF) coefficients
    INT32*                         sdf;                          //0x00000010
    /// pointer to the mic anti-distorsion filter (MDF) coefficients
    INT32*                         mdf;                          //0x00000014
    /// push-to-talk enable flag
    INT16                          if1;                          //0x00000018
} VPP_MAP_SPEECH_AUDIO_CFG_T; //Size : 0x1C



// ============================================================================
// VOIS_CONTEXT_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    /// Hold the 'started' or 'not started' state of VOIS
    BOOL                           voisStarted;                  //0x00000000
    /// Pointer to the output (encoded) buffer of VPP This field is to be used casted
    /// as a HAL_SPEECH_ENC_OUT_T.
    VOID*                          voisEncOutput;                //0x00000004
    /// Pointer to the input (decoded) buffer of VPP This field is to be used casted
    /// as a HAL_SPEECH_DEC_IN_T.
    VOID*                          voisDecInput;                 //0x00000008
    /// Number of played trames
    UINT32                         voisTramePlayedNb;            //0x0000000C
    /// Audio configuration used to play the stream in normal state. (ie not when
    /// started or stopped. This field is to be used casted as a AUD_LEVEL_T.
    AUD_MAP_LEVEL_T                voisAudioCfg;                 //0x00000010
    /// VPP configuration used to play the stream in normal state. (ie not when started
    /// or stopped.) This field should be used casted as a VPP_SPEECH_AUDIO_CFG_T.
    VPP_MAP_SPEECH_AUDIO_CFG_T     voisVppCfg;                   //0x00000020
    /// Interface used by vois to get and tell the words To be used casted as a AUD_ITF_ID_T.
    UINT32                         voisItf;                      //0x0000003C
    /// Latest config applied to Vois.
    VOIS_MAP_AUDIO_CFG_T           voisLatestCfg;                //0x00000040
    UINT32                         mode;                         //0x0000005C
    UINT32                         bufferSize;                   //0x00000060
} VOIS_CONTEXT_T; //Size : 0x64



// ============================================================================
// VOIS_MAP_ACCESS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef VOIS_CONTEXT_T VOIS_MAP_ACCESS_T;





// =============================================================================
// vois_RegisterYourself
// -----------------------------------------------------------------------------
/// This function registers the module itself to HAL so that the version and
/// the map accessor are filled. Then, the CSW get version function and the
/// CoolWatcher get version command will work.
// =============================================================================
PUBLIC VOID vois_RegisterYourself(VOID);



#endif

