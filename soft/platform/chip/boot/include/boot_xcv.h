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

#ifndef _BOOT_XCV_H_
#define _BOOT_XCV_H_

#include "cs_types.h"
#include "hal_mem_map.h"

/// @defgroup boot_xcv BOOT XCV Driver
///
/// This document describes the characteristics of the XCV Driver
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
// boot_XcvOpen
// -----------------------------------------------------------------------------
/// Open XCV for read and write XCV registers.
// =============================================================================
PUBLIC VOID boot_XcvOpen(VOID);

// =============================================================================
// boot_XcvClose
// -----------------------------------------------------------------------------
/// Close XCV (mini) driver.
// =============================================================================
PUBLIC VOID boot_XcvClose(VOID);

// =============================================================================
// boot_XcvRead
// -----------------------------------------------------------------------------
/// Read XCV register
/// @param address XCV register address
/// @return XCV register value
// =============================================================================
PUBLIC UINT32 boot_XcvRead(UINT16 address);

// =============================================================================
// boot_XcvWrite
// -----------------------------------------------------------------------------
/// Write XCV register
/// @param address XCV register address
/// @param data Value to be written
// =============================================================================
PUBLIC VOID boot_XcvWrite(UINT16 address, UINT32 data);

///  @} <- End of the boot_xcv group

#endif // _BOOT_XCV_H_
