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
#define BOOT_ROM_VERSION_SECTION  __attribute__((section (".boot_rom_version_number")))


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// g_bootRomVersionNumber
// -----------------------------------------------------------------------------
/// Contains the version of the ROM.
/// This constant is mapped at a fixed location at the end of the ROM.
// =============================================================================
CONST UINT32 g_bootRomVersionNumber BOOT_ROM_VERSION_SECTION = ROM_VERSION_NUMBER;


// =============================================================================
// Muted fram data arrays
// -----------------------------------------------------------------------------
/// Used by HAL SPEECH to construct mute frames.
/// These variables are copied from spc_tables.c, and the latter is linked to BCPU ROM.
// =============================================================================
CONST UINT32 MutedFrame_AMR[2]= {0x16c1e364, 0x00000000};
CONST UINT32 MutedFrame_HR[4]= {0x97712c80, 0xffffffff, 0xffffffff, 0x2d41ffff};
CONST UINT32 MutedFrame_EFR[8]=
{
    0x9c4bfcdb, 0xffff600d, 0xc000001f, 0x007ffffd, 0xfff00000, 0x00000fff, 0x3ff3fff0, 0x2fa00000
};
CONST UINT32 MutedFrame_FR[9]=
{
    0xd6174820, 0x0000284, 0x80000000, 0x00000002, 0x2800000, 0x00000000, 0x0028000, 0x00000000, 0x1d3f0000
};

