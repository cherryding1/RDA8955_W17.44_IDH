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


#ifndef _BOOT_SYS_H_
#define _BOOT_SYS_H_

#include "cs_types.h"
#include "hal_sys.h"


/// @defgroup system BOOT Sytem Driver
/// description
/// ...
/// @{



// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  FUNCTIONS
// =============================================================================


// ============================================================================
// boot_SysSetupPLL
// ----------------------------------------------------------------------------
/// Setup PLL at early system boot time.
// ============================================================================
PUBLIC VOID __attribute__ ((nomips16)) boot_SysSetupPLL(VOID);


// ============================================================================
// boot_SysShutdownPLL
// ----------------------------------------------------------------------------
/// Switch the system clock to RF clock (26M) and shutdown the PLL.
/// The FLASH and RAM access is considered invalid after switching.
/// So starting from this function, all the codes and the stack
/// should be in internal SRAM, unless the PLL is setup again.
// ============================================================================
PUBLIC VOID __attribute__ ((nomips16)) boot_SysShutdownPLL(VOID);


// =============================================================================
// boot_SysGetChipId
// -----------------------------------------------------------------------------
/// That function gives the chip ID the software is running on.
/// @param part Which part of the chip ID
/// @return The requested part of the chip ID.
// =============================================================================
PUBLIC UINT32 boot_SysGetChipId(HAL_SYS_CHIP_ID_T part);


// =============================================================================
// boot_SysGetMetalId
// -----------------------------------------------------------------------------
/// That function gives the chip metal ID the software is running on.
/// @return The chip metal ID enum, which is not the bitfield in chip register.
// =============================================================================
PUBLIC HAL_SYS_CHIP_METAL_ID_T boot_SysGetMetalId(VOID);


// =============================================================================
// boot_SysGetFreq
// -----------------------------------------------------------------------------
/// Get clk_sys from sys_ctrl registers.
/// @return Current clk_sys.
// =============================================================================
PUBLIC HAL_SYS_FREQ_T boot_SysGetFreq(VOID);


// =============================================================================
// boot_SysWaitMicrosecond
// -----------------------------------------------------------------------------
/// This function wait some time in microseconds.
/// It calculates CPU instruction cycles needed, depending on current system
/// frequency. For 312M, the max wait can be 0.9 second.
/// @param us Microseconds to wait.
// =============================================================================
PUBLIC VOID boot_SysWaitMicrosecond(UINT32 us);


// =============================================================================
// boot_UpdateIspiDivider
// -----------------------------------------------------------------------------
/// Update ISPI clock divider. System clock may be different from ROM after
/// XTAL and PLL is setup.
// =============================================================================
PUBLIC VOID boot_UpdateIspiDivider(VOID);

/// @} // end of the system group



#endif // _BOOT_SYS_H_

