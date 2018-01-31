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


#ifndef _SYSCMDS_GEN_CMD_ID_H_
#define _SYSCMDS_GEN_CMD_ID_H_

#include "syscmds_mod_id.h"

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// SYSCMDS_GENERAL_CMD_T
// -----------------------------------------------------------------------------
/// System general commands
// =============================================================================
typedef enum
{
    // Modem to AP
    MODEM_GENERAL_CMD_UNUSED = SYS_CMD_MODEM_MSG_ID_START + 1,
    MODEM_GENERAL_CMD_UNKNOWN_CMD_RECV,
    MODEM_GENERAL_CMD_RTC_TRIGGER,

    MODEM_GENERAL_CMD_END,

    // AP to modem
    AP_GENERAL_CMD_SHUTDOWN = SYS_CMD_AP_MSG_ID_START + 1,
    AP_GENERAL_CMD_RESTART,
    AP_GENERAL_CMD_TRACE_PATH,
    AP_GENERAL_CMD_CLK_OUT,
    AP_GENERAL_CMD_AUX_CLK,
    AP_GENERAL_CMD_CLK_32K, // + 6
    AP_GENERAL_CMD_MODEM_VER,
    AP_GENERAL_CMD_GET_BT_INFO,
    AP_GENERAL_CMD_TRACE_ENABLE,
    AP_GENERAL_CMD_GET_TRACE_STATUS,
    AP_GENERAL_CMD_SET_BT_INFO, // + 0xB
    AP_GENERAL_CMD_GET_WIFI_INFO,
    AP_GENERAL_CMD_SET_WIFI_INFO,
    AP_GENERAL_CMD_GET_BOOTLOGO_NAME,
    AP_GENERAL_CMD_SET_BOOTLOGO_NAME,
    AP_GENERAL_CMD_GET_MOBILE_BOARD_SN,
    AP_GENERAL_CMD_GET_MOBILE_PHONE_SN, // + 0x11

    AP_GENERAL_CMD_END,
} SYSCMDS_GENERAL_CMD_T;

#endif // _SYSCMDS_GEN_CMD_ID_H_

