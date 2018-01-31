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


#ifndef _BOOT_KEY_H_
#define _BOOT_KEY_H_

#include "cs_types.h"

/// @defgroup boot_key BOOT KEY Driver
///
/// This document describes the characteristics of the mem init Driver
/// and how to use it via its Hardware Abstraction Layer API.
///
///
/// @{
///


// ============================================================================
// TYPES
// ============================================================================

// ============================================================================
// FUNCTIONS
// ============================================================================


// =============================================================================
// boot_ForceMonitorKeyCheck
// -----------------------------------------------------------------------------
/// Enter boot monitor if all of the force-monitor keys are pressed.
// =============================================================================
PROTECTED VOID boot_ForceMonitorKeyCheck(VOID);

// =============================================================================
// boot_ForceUsbMonitorKeyCheck
// -----------------------------------------------------------------------------
/// Enter boot monitor if all of the usb-force-monitor keys are pressed with AC_ON.
// =============================================================================
PROTECTED VOID boot_ForceUsbMonitorKeyCheck(VOID);



///  @} <- End of the EBC group


#endif // _BOOT_KEY_H_

