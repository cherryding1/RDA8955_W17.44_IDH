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


#include "bcpu_registermap.h"


// =============================================================================
// g_spalMapVersion
// -----------------------------------------------------------------------------
/// Contains the version of SPAL.
// =============================================================================
EXPORT PROTECTED HAL_MAP_VERSION_T g_spalMapVersion;


// =============================================================================
// g_sppMapVersion
// -----------------------------------------------------------------------------
/// Contains the version of SPP.
// =============================================================================
EXPORT PROTECTED HAL_MAP_VERSION_T g_sppMapVersion;


// =============================================================================
// g_spcMapVersion
// -----------------------------------------------------------------------------
/// Contains the version of SPC.
// =============================================================================
EXPORT PROTECTED HAL_MAP_VERSION_T g_spcMapVersion;


// =============================================================================
// g_spalMapAccess
// -----------------------------------------------------------------------------
/// Contains the addresses of the structures of SPAL that will be accessible from
/// a remote PC (e.g. through CoolWatcher).
/// XCPU does not need to know the type of this, so just get the symbol by it's
/// name.
// =============================================================================
EXPORT PROTECTED VOID* g_spalMapAccess;


// =============================================================================
// g_spcMapAccess
// -----------------------------------------------------------------------------
/// Contains the addresses of the structures of SPC that will be accessible from
/// a remote PC (e.g. through CoolWatcher).
/// XCPU does not need to know the type of this, so just get the symbol by it's
/// name.
// =============================================================================
EXPORT PROTECTED VOID* g_spcMapAccess;


// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// bcpu_RegisterYourself
// -----------------------------------------------------------------------------
/// This function registers the module itself to HAL so that the version and
/// the map accessor are filled. Then, the CSW get version function and the
/// CoolWatcher get version command will work.
/// THIS FUNCTION IS IN BCPU MODULE BUT WILL BE EXECUTED BY XCPU !!
// =============================================================================
PUBLIC VOID bcpu_RegisterYourself(VOID)
{
    // Fill the map structure with HAL's version and accessible structure map
    hal_MapEngineRegisterModule(HAL_MAP_ID_SPAL,
                                &g_spalMapVersion,
#if (SPC_IF_VER >= 5)
                                &g_spalMapAccess
#else
                                NULL
#endif
                               );
    hal_MapEngineRegisterModule(HAL_MAP_ID_SPP,
                                &g_sppMapVersion,
                                NULL);
#if defined(FPGA_NBCPU) && !defined(BUILD_BCPU_PSRAM)
    hal_MapEngineRegisterModule(HAL_MAP_ID_SPC,
                                &g_spcMapVersion,
                                NULL);
#else
    hal_MapEngineRegisterModule(HAL_MAP_ID_SPC,
                                &g_spcMapVersion,
                                g_spcMapAccess);
#endif
}
