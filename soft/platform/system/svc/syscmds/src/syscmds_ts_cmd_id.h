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


#ifndef _SYSCMDS_TS_CMD_ID_H_
#define _SYSCMDS_TS_CMD_ID_H_

#include "syscmds_mod_id.h"

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// SYSCMDS_TS_CMD_T
// -----------------------------------------------------------------------------
/// System touch screen commands
// =============================================================================
typedef enum
{
    // Modem to AP
    MODEM_TS_CMD_I2C_SEND_DONE = SYS_CMD_MODEM_MSG_ID_START + 1,
    MODEM_TS_CMD_I2C_RECV_DONE,

    MODEM_TS_CMD_END,

    // AP to modem
    AP_TS_CMD_I2C_INIT = SYS_CMD_AP_MSG_ID_START + 1,
    AP_TS_CMD_I2C_SEND,
    AP_TS_CMD_I2C_RECV,
    AP_TS_CMD_I2C_SUSPEND,
    AP_TS_CMD_I2C_RESUME,
    AP_TS_CMD_I2C_REBOOT,
    AP_TS_CMD_I2C_BUFF_ADD,
    AP_TS_CMD_I2C_INFO,
    AP_TS_CMD_SWITCH_PROXIMITY_MODE,
    AP_TS_CMD_END,
} SYSCMDS_TS_CMD_T;

#endif // _SYSCMDS_TS_CMD_ID_H_

