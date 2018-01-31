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


#ifndef _SYSCMDS_MOD_ID_H_
#define _SYSCMDS_MOD_ID_H_

// =============================================================================
// MACROS
// =============================================================================

#define SYS_CMD_MODEM_MSG_ID_START (0x0)

#define SYS_CMD_AP_MSG_ID_START    (0x1000)

#define SYS_CMD_REPLY_MSG_FLAG     (0x8000)


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// SYSCMDS_MOD_ID_T
// -----------------------------------------------------------------------------
/// The module IDs
// =============================================================================
typedef enum
{
    SYSCMDS_MOD_ID_GENERAL = 0,
    SYSCMDS_MOD_ID_CALIB,
    SYSCMDS_MOD_ID_PM,
    SYSCMDS_MOD_ID_AUDIO,
    SYSCMDS_MOD_ID_TS,
    SYSCMDS_MOD_ID_GPIO,

    SYSCMDS_MOD_ID_END,
} SYSCMDS_MOD_ID_T;

#endif // _SYSCMDS_MOD_ID_H_

