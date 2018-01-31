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


#ifndef _AVPSP_DECODE_H_
#define _AVPSP_DECODE_H_

#include "cs_types.h"
#include "avps_m.h"

#include "hal_aif.h"

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// MPS_DECODE_TEST_RESULT_T
// -----------------------------------------------------------------------------
/// Capability of decoder
// =============================================================================
typedef enum
{
    MPS_DECODE_TEST_RESULT_NO           = 0x00,
    MPS_DECODE_TEST_RESULT_AUDIO        = 0x01,
    MPS_DECODE_TEST_RESULT_VIDEO        = 0x02,
    MPS_DECODE_TEST_RESULT_AUDIO_VIDEO  = 0x03
} MPS_DECODE_TEST_RESULT_T ;


// =============================================================================
// AVPS_DECODE_TEST_T
// -----------------------------------------------------------------------------
/// Test if the decoder can play this audio & video stream
/// @param stream Stream must be tested
/// @return The decoder capability about the audio and video stream
// =============================================================================
typedef MPS_DECODE_TEST_RESULT_T (*AVPS_DECODE_TEST_T)(CONST AVPS_ENC_STREAM_T* stream);

// =============================================================================
// AVPS_DECODE_STOP_T
// -----------------------------------------------------------------------------
/// Stop the decoder and free resources
// =============================================================================
typedef VOID (*AVPS_DECODE_STOP_T)(VOID);

// =============================================================================
// AVPS_DECODE_PLAY_T
// -----------------------------------------------------------------------------
/// Configure decoder to play the stream
/// @param stream Stream played
/// @param cfg Configuration use to play stream
/// @param frameWindows Frame windows for video
// =============================================================================
typedef AVPS_ERR_T (*AVPS_DECODE_PLAY_T)(CONST AVPS_ENC_STREAM_T*       stream,
        CONST AVPS_CFG_T*              cfg,
        CONST LCDD_FBW_T*              frameWindows,
        HAL_AIF_STREAM_T*              pcmStream);

// =============================================================================
// AVPS_DECODE_SETUP_T
// -----------------------------------------------------------------------------
/// Update audio and decoder config
/// @param cfg Configuration use to reconfigure
// =============================================================================
typedef AVPS_ERR_T (*AVPS_DECODE_SETUP_T)(CONST AVPS_CFG_T*              cfg);

// =============================================================================
// AVPS_DECODE_T
// -----------------------------------------------------------------------------
/// Decoder struct
// =============================================================================
typedef struct
{
    AVPS_DECODE_TEST_T  test;
    AVPS_DECODE_PLAY_T  play;
    AVPS_DECODE_STOP_T  stop;
    AVPS_DECODE_SETUP_T setup;
} AVPS_DECODE_T;


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================


#endif // _AVPSP_DECODE_H_
