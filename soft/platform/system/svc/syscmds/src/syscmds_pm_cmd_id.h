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


#ifndef _SYSCMDS_PM_CMD_ID_H_
#define _SYSCMDS_PM_CMD_ID_H_

#include "syscmds_mod_id.h"

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// SYSCMDS_PM_CMD_T
// -----------------------------------------------------------------------------
/// System pm commands
// =============================================================================
typedef enum
{
    // Modem to AP
    MODEM_PM_CMD_BATT_STATUS = SYS_CMD_MODEM_MSG_ID_START + 1,
    MODEM_PM_CMD_EP_STATUS,
    MODEM_PM_CMD_EP_KEY_STATUS,
    MODEM_PM_CMD_CHIPTEMPER_STATUS,
    MODEM_PM_CMD_BATTTEMPER_STATUS,

    MODEM_PM_CMD_END,

    // AP to modem
    AP_PM_CMD_ENABLE_POWER = SYS_CMD_AP_MSG_ID_START + 1,
    AP_PM_CMD_SET_LEVEL,
    AP_PM_CMD_SET_LDO_VOLT,
    AP_PM_CMD_GET_BATT_VOLT,
    AP_PM_CMD_GET_EP_VOLT,
    AP_PM_CMD_GET_ADC_VALUE, // + 6
    AP_PM_CMD_EP_STATUS,
    AP_PM_CMD_MON_EP_KEY,
    AP_PM_CMD_ENABLE_ADC,
    AP_PM_CMD_GET_ADC_CALIB_VALUE,
    AP_PM_CMD_ENABLE_CHARGER, // + 0xB
    AP_PM_CMD_RESTART_SECOND_PAD_POWER,
    AP_PM_CMD_CHARGER_TYPE,
    AP_PM_CMD_GET_CHARGER_STATUS,
    AP_PM_CMD_GET_CHARGER_VOLT,
    AP_PM_CMD_NOTIFY_VPU_STATUS,
    AP_PM_CMD_ENABLE_CHARGER_CURRENT, // 0x11
    AP_PM_CMD_SET_CHARGER_CURRENT,
    AP_PM_CMD_SET_VOLT_PCT_MIN,
    AP_PM_CMD_SET_VOLT_PCT_MAX,
    AP_PM_CMD_GET_CHARGER_CURRENT, // + 15
    AP_PM_CMD_SET_LEDS_BREATHING_ON, // + 16
    AP_PM_CMD_SET_LEDS_BREATHING_OFF, // + 17

    AP_PM_CMD_END,
} SYSCMDS_PM_CMD_T;

#endif // _SYSCMDS_PM_CMD_ID_H_

