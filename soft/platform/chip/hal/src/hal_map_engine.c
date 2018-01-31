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

#include "hal_error.h"
#include "hal_map_engine.h"

#include "halp_debug.h"



// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
#define UNCACHED(n) (UINT32*)(((UINT32)n)|0xa0000000)

#define  HAL_MAP_TABLE_LOC  __attribute__((section(".sramucbss")))

// =============================================================================
// g_halFixedPointer
// -----------------------------------------------------------------------------
/// Address of the fixed pointer. At this address, the remote tools (e.g.
/// CoolWatcher) will find a pointer to the map structure.
// =============================================================================
PROTECTED HAL_MAP_TABLE_T* g_halFixedPointer __attribute__((section (".fixptr")));



// =============================================================================
// g_halMapTable
// -----------------------------------------------------------------------------
/// Structure containing the map informations of the software modules of the
/// system: version of the module and access to the map description.
// =============================================================================
PROTECTED HAL_MAP_TABLE_T g_halMapTable HAL_MAP_TABLE_LOC;



// =============================================================================
//  FUNCTIONS
// =============================================================================



// =============================================================================
// hal_MapEngineOpen
// -----------------------------------------------------------------------------
/// Initialize the map engine so that the remote tools can access the map
/// structures of the software modules.
// =============================================================================
VOID hal_MapEngineOpen(VOID)
{
    UINT16 i;

    // Init the map table.
    for (i = 0; i < HAL_MAP_ID_COUNT; i++)
    {
        g_halMapTable.modules[i].version = NULL;
        g_halMapTable.modules[i].access = NULL;
    }

    // Init the fixed pointer with the address of the map table.
    *UNCACHED(&g_halFixedPointer) = (UINT32)&g_halMapTable;
}



// =============================================================================
// hal_MapEngineRegisterModule
// -----------------------------------------------------------------------------
/// Configure the map access field for the specified module in the table
/// located at the address pointer by the fixed pointer.
/// @param id Id of the module for which the version and the access
/// pointer will be specified.
/// @param version Pointer to a structure giving the version information for
/// the module 'id'. This structure must be instantiated by the module 'id'.
/// @param access Pointer to a structure containing the informations that will
/// be accessible from the remote PC (e.g. through CoolWatcher). This strucutre
/// must be instantiated by the module 'id'.
/// @return HAL_ERR_NO if no error or HAL_ERR_BAD_PARAMETER if the module 'id'
/// cannot be found.
// =============================================================================
HAL_ERR_T hal_MapEngineRegisterModule(HAL_MAP_ID_T id, CONST HAL_MAP_VERSION_T*  version, VOID* access)
{
    // Check the id range.
    if (id < 0 || id >= HAL_MAP_ID_COUNT)
    {
        return HAL_ERR_BAD_PARAMETER;
    }

    // Register the version and the access.
    g_halMapTable.modules[id].version = version;
    g_halMapTable.modules[id].access  = access;

    // Report the version string of the module
//    HAL_TRACE(TSTDOUT, 0, "Module :%d, %08x, %d", id, version->number, version->string);
    return HAL_ERR_NO;
}

// =============================================================================
// hal_MapEnginePrintModuleVersion
// -----------------------------------------------------------------------------
/// Print a module version's string on the Trace, if the module has been
/// registered.
/// @param id Number of the Module whose version is to be put
/// on the Trace.
// =============================================================================
PUBLIC VOID hal_MapEnginePrintModuleVersion(HAL_MAP_ID_T id)
{
    HAL_ASSERT((id >= 0 && id < HAL_MAP_ID_COUNT),
               "Bad Hal Engine Module Number");

    if (g_halMapTable.modules[id].version != NULL)
    {
#if (CHIP_HAS_BCPU_ROM)
#ifdef REWRITE_ROM_AT_BOOT
#ifdef FPGA
        // BCPU ROM is not updated yet, and the contents of version is determined
        // by previous BCPU ROM.
        if (id==HAL_MAP_ID_SPAL || id==HAL_MAP_ID_SPP || id==HAL_MAP_ID_SPC)
        {
            HAL_TRACE(TSTDOUT, 0, "Skpping module in BCPU ROM: %u", id);
        }
        else
#endif // FPGA
#endif // REWRITE_ROM_AT_BOOT
#endif // CHIP_HAS_BCPU_ROM
        {
            HAL_TRACE(TSTDOUT, 1, "%s\n number: %d - date: %d - revision: %d.",
                      g_halMapTable.modules[id].version->string,
                      g_halMapTable.modules[id].version->number,
                      g_halMapTable.modules[id].version->date,
                      g_halMapTable.modules[id].version->revision
                     );
        }
    }
    else
    {
        HAL_TRACE(TSTDOUT, 0, "Module %d not registered !", id);
    }

}


// =============================================================================
// hal_MapEnginePrintAllModulesVersion
// -----------------------------------------------------------------------------
/// Print all module version's string on the Trace.
// =============================================================================
PUBLIC VOID hal_MapEnginePrintAllModulesVersion(VOID)
{
    UINT32 i;
    for (i=0 ; i < HAL_MAP_ID_COUNT ; i++)
    {
        hal_MapEnginePrintModuleVersion(i);
    }
}


// =============================================================================
// hal_MapEngineGetVersion
// -----------------------------------------------------------------------------
/// Get the pointer to the version structure of a given module.
///
/// @param id Number of the Module whose version is to be got.
/// @return Pointer to the version structure of said module.
// =============================================================================
PUBLIC CONST HAL_MAP_VERSION_T* hal_MapEngineGetVersion(HAL_MAP_ID_T id)
{
    HAL_ASSERT((id >= 0 && id < HAL_MAP_ID_COUNT),
               "Bad Hal Engine Module Number");

    return g_halMapTable.modules[id].version;
}

