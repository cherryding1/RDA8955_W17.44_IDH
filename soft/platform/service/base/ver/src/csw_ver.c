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



#include "chip_id.h"
#include "cs_types.h"
#include "csw_ver.h"

#include "hal_map_engine.h"



// =============================================================================
// GLOBAL VARIABLES
// =============================================================================



// =============================================================================
// g_cswMmiMapVersion
// -----------------------------------------------------------------------------
/// Contains the version of the MMI, when the MMI had filled it with the
/// function csw_VerSetMmiVersion().
// =============================================================================
PROTECTED HAL_MAP_VERSION_T HAL_MAP_VER_LOC g_cswMmiMapVersion;



// =============================================================================
// FUNCTIONS
// =============================================================================


// =============================================================================
//csw_VerGetRevision
// -----------------------------------------------------------------------------
/// Return the revision of a given module.
///
/// @param  moduleId Id the module.
/// @return The SVN revision of that version of the module.
// =============================================================================
PUBLIC UINT32 csw_VerGetRevision(CSW_VER_MODULE_ID_T moduleId)
{
    // HAL function checks the parameter validity.
    CONST HAL_MAP_VERSION_T *version = hal_MapEngineGetVersion(moduleId);

    // Check if the module has been registered.
    if (version == NULL)
    {
        return 0;
    }
    return version->revision;
}


// =============================================================================
//csw_VerGetNumber
// -----------------------------------------------------------------------------
/// Return the version number of a given module.
///
/// @param  moduleId Id the module.
/// @return The version number of that version of the module.
// =============================================================================
PUBLIC UINT32 csw_VerGetNumber(CSW_VER_MODULE_ID_T moduleId)
{
    // HAL function checks the parameter validity.
    CONST HAL_MAP_VERSION_T *version = hal_MapEngineGetVersion(moduleId);

    // Check if the module has been registered.
    if (version == NULL)
    {
        return 0;
    }
    return version->number;
}


// =============================================================================
//csw_VerGetDate
// -----------------------------------------------------------------------------
/// Return the date of build of a given module.
/// The format is 0xYYYYMMDD.
///
/// @param  moduleId Id the module.
/// @return The build date of that version of the module.
// =============================================================================
PUBLIC UINT32 csw_VerGetDate(CSW_VER_MODULE_ID_T moduleId)
{
    // HAL function checks the parameter validity.
    CONST HAL_MAP_VERSION_T *version = hal_MapEngineGetVersion(moduleId);

    // Check if the module has been registered.
    if (version == NULL)
    {
        return 0;
    }
    return version->date;
}


// =============================================================================
//csw_VerGetString
// -----------------------------------------------------------------------------
/// Return the version string of a given module.
///
/// @param  moduleId Id the module.
/// @return The version string of that version of the module.
// =============================================================================
PUBLIC CONST CHAR *csw_VerGetString(CSW_VER_MODULE_ID_T moduleId)
{
    // HAL function checks the parameter validity.
    CONST HAL_MAP_VERSION_T *version = hal_MapEngineGetVersion(moduleId);

    // Check if the module has been registered.
    if (version == NULL)
    {
        return "Not registered";
    }
    return version->string;
}



// =============================================================================
//csw_VerSetMmiVersion
// -----------------------------------------------------------------------------
/// Register the version of the MMI in the CSW version mechanism.
///
/// @param  revision Number of the current revision (the SCM revision).
/// @param  string Pointer to the string version.
// =============================================================================
PUBLIC VOID csw_VerSetMmiVersion(UINT32 revision, CONST CHAR *string)
{
    g_cswMmiMapVersion.revision = revision;
    g_cswMmiMapVersion.string = (CHAR *)string;

    hal_MapEngineRegisterModule(HAL_MAP_ID_MMI, &g_cswMmiMapVersion, NULL);
}



