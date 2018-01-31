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


#include "cs_types.h"

#include "bootp_sector.h"
#include "boot.h"
#include "global_macros.h"
#include "bootp_host.h"
#include "bootp_mode.h"


// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================


/// Entry of the Boot Monitor in ROM.
EXPORT PROTECTED VOID boot_Monitor(VOID);


// =============================================================================
// boot_SectorEnterBootMonitor
// -----------------------------------------------------------------------------
/// From the Boot Sector, enter in the Boot Monitor.
/// And, depending on the chip, make the modifications required to have a
/// fully working monitor.
/// This function is implemented in "boot/CT_ASIC/src/boot_sector_chip.c".
// =============================================================================
PROTECTED VOID boot_SectorEnterBootMonitor(VOID)
{
    /// Force entering the boot monitor.
    /// This function is at a fixed location, in ROM.
    g_bootBootMode |= BOOT_MODE_FORCE_MONITOR;
    boot_Monitor();
}

// =============================================================================
// boot_SectorFixRom
// -----------------------------------------------------------------------------
/// Use to fix some startup missing feature:
/// This function is implemented in "boot/CT_ASIC/src/boot_sector_chip.c".
// =============================================================================
VOID boot_SectorFixRom(VOID)
{
}


