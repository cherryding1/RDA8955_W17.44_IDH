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

#include "memd_m.h"
#include "memdp_debug.h"
#include "memdp.h"

#ifndef MEMD_USE_ROMULATOR

// =============================================================================
// memd_FlashRomulatorSetRamMode
// -----------------------------------------------------------------------------
/// Simili 'Set the  in RAM mode'. Always return \c FALSE and does nothing.
/// This function is only effective when using a romulator device.
///
/// When the romualtor is in RAM mode, it can be accessed as a RAM, that is to
/// say read and written without sending any command, but through direct
/// addressing.
///
/// @param  enable Enable or disable the RAM mode.
/// @return FALSE.
// =============================================================================
PUBLIC BOOL memd_FlashRomulatorSetRamMode(BOOL enable)
{
    MEMD_TRACE(MEMD_WARN_TRC, 0, "MEMD: Ram Mode Feature not available on a combo.");
    return FALSE;
}

#endif // MEMD_USE_ROMULATOR not defined.

