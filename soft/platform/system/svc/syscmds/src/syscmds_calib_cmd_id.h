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


#ifndef _SYSCMDS_CALIB_CMD_ID_H_
#define _SYSCMDS_CALIB_CMD_ID_H_

#include "syscmds_mod_id.h"

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// SYSCMDS_CALIB_CMD_T
// -----------------------------------------------------------------------------
/// System calibration commands
// =============================================================================
typedef enum
{
    // Modem to AP
    MODEM_CALIB_CMD_CALIB_START = SYS_CMD_MODEM_MSG_ID_START + 1,
    MODEM_CALIB_CMD_CALIB_DONE,

    MODEM_CALIB_CMD_END,

    // AP to modem
    AP_CALIB_CMD_FLASH_RESULT = SYS_CMD_AP_MSG_ID_START + 1,
    AP_CALIB_CMD_CALIB_STATUS,
    AP_CALIB_CMD_UPDATE_CALIB,
    AP_CALIB_CMD_UPDATE_FACTORY,

    AP_CALIB_CMD_END,
} SYSCMDS_CALIB_CMD_T;


#endif // _SYSCMDS_CALIB_CMD_ID_H_

