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
#include "hal_map_engine.h"

#define SPP_VERSION_NUMBER SPC_IF_VER

#if defined(CHIP_BYPASS_ROM) || defined (REWRITE_ROM_AT_BOOT)
// special build use generated headers
#include "sppp_version.h"
#else
// use something to identify the rom code (here chip ID)
#include "chip_id.h"
// might want to change SPP_VERSION_STRING depending on chips (?)
#define SPP_VERSION_STRING "SPP"
// use generated headers here
#include "sppp_version.h"
#endif


// =============================================================================
// g_sppMapVersion
// -----------------------------------------------------------------------------
/// Contains the version of SPP.
// =============================================================================
PROTECTED CONST HAL_MAP_VERSION_T g_sppMapVersion
    = SPP_VERSION_STRUCT;


