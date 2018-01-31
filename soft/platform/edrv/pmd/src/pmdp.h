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
#include "pmd_config.h"
#include "pmd_m.h"
#include "pmd_map.h"

// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================



// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// PMD_PROFILE_MODE_T
// -----------------------------------------------------------------------------
/// Profile register selection.
/// Only low power and normal mode (using profile 2 & 1) are used in the driver.
// =============================================================================
typedef enum
{
    PMD_PROFILE_MODE_NORMAL = 0,
    PMD_PROFILE_MODE_LOWPOWER,

    PMD_PROFILE_MODE_QTY,

} PMD_PROFILE_MODE_T;


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

/// Configuration structure
EXPORT CONST PMD_CONFIG_T* g_pmdConfig;

/// Coolwatcher watchable pmd status structure
EXPORT PROTECTED PMD_MAP_ACCESS_T g_pmdMapAccess;


// =============================================================================
//  FUNCTIONS
// =============================================================================



