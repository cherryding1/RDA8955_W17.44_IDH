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
#include "spal_mem.h"
#include "spal_map.h"
#include "hal_map_engine.h"

/// @todo: for next chips place the version information in rom

#define SPAL_VERSION_NUMBER SPC_IF_VER

#if defined(CHIP_BYPASS_ROM) || defined (REWRITE_ROM_AT_BOOT)
// special build use generated headers
#include "spalp_version.h"
#else
// use something to identify the rom code (here chip ID)
#include "chip_id.h"
// might want to change SPAL_VERSION_STRING depending on chips (?)
#define SPAL_VERSION_STRING "SPAL"
// use generated headers here
#include "spalp_version.h"
#endif


// =============================================================================
// g_spalProfileControl
// -----------------------------------------------------------------------------
/// Contains the profile control configuration.
// =============================================================================
PROTECTED SPAL_PROFILE_CONTROL_T SPAL_BBSRAM_GLOBAL g_spalProfileControl;


// =============================================================================
// g_spalMapAccess
// -----------------------------------------------------------------------------
/// Contains the addresses of the structures of SPAL that will be accessible from
/// a remote PC (e.g. through CoolWatcher).
// =============================================================================
PROTECTED CONST SPAL_MAP_ACCESS_T g_spalMapAccess
    = { &g_spalProfileControl };


// =============================================================================
// g_spalMapVersion
// -----------------------------------------------------------------------------
/// Contains the version of SPAL.
// =============================================================================
PROTECTED CONST HAL_MAP_VERSION_T g_spalMapVersion
    = SPAL_VERSION_STRUCT;

