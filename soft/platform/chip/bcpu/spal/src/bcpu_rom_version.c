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


// =============================================================================
//  MACROS
// =============================================================================


/// Linker section where the Boot Sector main function must be mapped:
/// Beginning of the first sector of the flash.
#define BCPU_ROM_VERSION_SECTION  __attribute__((section (".bcpu_rom_version_number")))


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// g_bootRomVersionNumber
// -----------------------------------------------------------------------------
/// Contains the version of the ROM.
/// This constant is mapped at a fixed location at the end of the ROM.
// =============================================================================
CONST UINT32 g_bcpuRomVersionNumber BCPU_ROM_VERSION_SECTION = ROM_VERSION_NUMBER;

