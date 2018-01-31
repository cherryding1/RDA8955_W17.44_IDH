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


#ifndef _BOOTP_SECTOR_H_
#define _BOOTP_SECTOR_H_

#include "cs_types.h"


// =============================================================================
// MACROS
// =============================================================================


/// Value of the event sent by the Boot Sector when it configures the EBC
/// CS RAM (useful for RAM in burst mode, after a soft reset).
#define BOOT_SECTOR_EVENT_CONFIG_RAM    0x00000055


/// /// Value of the event sent by the Boot Sector when it detects that it
/// must enter in the boot monitor.
#define BOOT_SECTOR_EVENT_ENTER_MONITOR 0x00000056


// =============================================================================
// FUNCTIONS
// =============================================================================


// =============================================================================
// boot_SectorEnterBootMonitor
// -----------------------------------------------------------------------------
/// From the Boot Sector, enter in the Boot Monitor.
/// And, depending on the chip, make the modifications required to have a
/// fully working monitor.
/// This function is implemented in "boot/CT_ASIC/src/boot_sector_chip.c".
// =============================================================================
PROTECTED VOID boot_SectorEnterBootMonitor(VOID);

// =============================================================================
// boot_SectorFixRom
// -----------------------------------------------------------------------------
/// Use to fix some startup missing feature (like usb clock on greenstone)
/// This function is implemented in "boot/CT_ASIC/src/boot_sector_chip.c".
// =============================================================================
PROTECTED VOID boot_SectorFixRom(VOID);


#endif // _BOOTP_SECTOR_H_
