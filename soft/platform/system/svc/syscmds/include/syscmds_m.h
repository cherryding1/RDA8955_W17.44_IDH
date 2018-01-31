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


#ifndef _SYSCMDS_M_H_
#define _SYSCMDS_M_H_

#include "cs_types.h"


/// @file syscmds_m.h
/// @mainpage AP system command service
///
/// This service provides an interface for handling AP system commands and
/// sending the responses.
///
/// This document is composed of:
/// - @ref syscmds
///
/// @defgroup syscmds System command service (syscmds)
/// @{
///

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

PUBLIC VOID syscmds_Open(VOID);

PUBLIC VOID syscmds_SendCalibStartMsg(VOID);

PUBLIC VOID syscmds_SendCalibDoneMsg(VOID);


///  @} <- End of the syscmds group


#endif // _SYSCMDS_M_H_
