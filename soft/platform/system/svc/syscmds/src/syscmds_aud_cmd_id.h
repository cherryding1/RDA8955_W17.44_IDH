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


#ifndef _SYSCMDS_AUD_CMD_ID_H_
#define _SYSCMDS_AUD_CMD_ID_H_

#include "syscmds_mod_id.h"

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// SYSCMDS_AUDIO_CMD_T
// -----------------------------------------------------------------------------
/// System audio commands
// =============================================================================
typedef enum
{
    // Modem to AP
    MODEM_AUDIO_CMD_VOIS_HANDLER_CALLBACK = SYS_CMD_MODEM_MSG_ID_START + 1,
    AP_AUDIO_CMD_SET_AUDIO_CALIB_EQ,
    AP_AUDIO_CMD_SET_AUDIO_CALIB_ALC,
    MODEM_AUDIO_CMD_END,

    // AP to modem
    AP_AUDIO_CMD_STREAM_START = SYS_CMD_AP_MSG_ID_START + 1,
    AP_AUDIO_CMD_STREAM_RECORD,
    AP_AUDIO_CMD_STREAM_PAUSE,
    AP_AUDIO_CMD_STREAM_STOP,
    AP_AUDIO_CMD_SETUP,
    AP_AUDIO_CMD_SPK_WITH_EP, // + 6
    AP_AUDIO_CMD_TONE_START,
    AP_AUDIO_CMD_TONE_STOP,
    AP_AUDIO_CMD_TONE_PAUSE,
    AP_AUDIO_CMD_TEST_MODE,
    AP_AUDIO_CMD_FORCE_MIC_SEL, // + 0xB
    AP_AUDIO_CMD_MUTE_OUTPUT,
    AP_AUDIO_CMD_BYPASS_NOTCH,
    AP_AUDIO_CMD_SET_APP_MODE,
    AP_AUDIO_CMD_VOIS_RECORD_START,
    AP_AUDIO_CMD_VOIS_RECORD_STOP, // + 0x10
    AP_AUDIO_CMD_GET_AUDIO_CALIB,
    AP_AUDIO_CMD_GET_HOST_AUDIO_CALIB,
    AP_AUDIO_CMD_GET_MP3_BUFFER,
    AP_AUDIO_CMD_GET_AUDIO_IIR_CALIB,
    AP_AUDIO_CMD_GET_AUDIO_EQ_CALIB,
    AP_AUDIO_CMD_GET_AUDIO_DRC_CALIB,
    AP_AUDIO_CMD_AUD_GET_INGAINS_RECORD_CALIB,

    AP_AUDIO_CMD_END,
} SYSCMDS_AUDIO_CMD_T;

#endif // _SYSCMDS_AUD_CMD_ID_H_

