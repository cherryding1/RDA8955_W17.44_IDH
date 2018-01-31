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


#ifndef _SYSCMDS_GPIO_CMD_ID_H_
#define _SYSCMDS_GPIO_CMD_ID_H_

#include "syscmds_mod_id.h"

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// SYSCMDS_GPIO_CMD_T
// -----------------------------------------------------------------------------
/// System GPIO commands
// =============================================================================
typedef enum
{
    // Modem to AP
    MODEM_GPIO_CMD_IRQ_TRIGGER = SYS_CMD_MODEM_MSG_ID_START + 1,

    MODEM_GPIO_CMD_END,

    // AP to modem
    AP_GPIO_CMD_OPEN = SYS_CMD_AP_MSG_ID_START + 1,
    AP_GPIO_CMD_CLOSE,
    AP_GPIO_CMD_SET_IO,
    AP_GPIO_CMD_GET_VALUE,
    AP_GPIO_CMD_SET_VALUE,
    AP_GPIO_CMD_ENABLE_IRQ, // + 6

    AP_GPIO_CMD_END,
} SYSCMDS_GPIO_CMD_T;

#endif // _SYSCMDS_GPIO_CMD_ID_H_

