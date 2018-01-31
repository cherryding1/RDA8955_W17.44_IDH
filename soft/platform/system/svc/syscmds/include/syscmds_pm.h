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


#ifndef _SYSCMDS_PM_H_
#define _SYSCMDS_PM_H_

#include "cs_types.h"
#include "pmd_m.h"


/// @file syscmds_pm.h
/// @mainpage AP system command service
///
/// This service provides an interface for handling AP system commands and
/// sending the responses.
///
/// This document is composed of:
/// - @ref syscmds
///
/// @defgroup syscmds System command PM service (syscmds_pm)
/// @{
///

// =============================================================================
// MACROS
// =============================================================================



// =============================================================================
// TYPES
// =============================================================================

typedef VOID (*SYSCMDS_EAR_STATUS_HANDLER_T)(PMD_EAR_STATUS_T status);

typedef VOID (*SYSCMDS_MON_EAR_KEY_HANDLER_T)(BOOL start);


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

PUBLIC VOID syscmds_NotifyBatteryStatus(UINT8 percent, UINT8 status, UINT16 level);
PUBLIC VOID syscmds_NotifyChipTemplaterStatus( UINT16 bHigetTemp, UINT16 nTemplater);
PUBLIC VOID syscmds_NotifyBattTemplaterStatus( UINT16 bHigetTemp, UINT16 nTemplater);

PUBLIC VOID syscmds_NotifyEarPieceStatus(UINT32 on);

PUBLIC VOID syscmds_NotifyEarPieceKeyStatus(UINT32 gpadc);

PUBLIC VOID syscmds_SetEarPieceStatusHandler(SYSCMDS_EAR_STATUS_HANDLER_T handler);

PUBLIC VOID syscmds_SetMonEarPieceKeyHandler(SYSCMDS_MON_EAR_KEY_HANDLER_T handler);


///  @} <- End of the syscmds_pm group


#endif // _SYSCMDS_PM_H_
