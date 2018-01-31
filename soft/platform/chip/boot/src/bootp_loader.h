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


#ifndef _BOOTP_LOADER_H_
#define _BOOTP_LOADER_H_

#include "cs_types.h"

// =============================================================================
// MACROS
// =============================================================================


/// Value of the event sent by the Boot Loader when it starts the main() of
/// the code.
#define BOOT_LOADER_EVENT_START_CODE 0x00000057


/// Value of the event sent by the Boot Loader when the version of the ROM of
/// the chip mismatch the version of the ROM against which the code is linked.
#define BOOT_LOADER_EVENT_ROM_ERROR 0x00000054


// =============================================================================
// FUNCTIONS
// =============================================================================
// =============================================================================
//  boot_LoaderEnter
// -----------------------------------------------------------------------------
/// The parameter "param" has a dummy value when we boot normally from ROM,
/// but it has a special value when the function is called by a jump command
/// of a Boot Monitor (Host, USB or UART). This is used to change the main of
/// the code (where we will jump at the end of the boot loading sequence).
// =============================================================================
PROTECTED VOID boot_LoaderEnter(UINT32 param);

#endif // _BOOTP_LOADER_H_



