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



#ifndef _MEMD_CONFIG_H
#define _MEMD_CONFIG_H

#include "cs_types.h"
#include "chip_id.h"
#include "hal_ebc.h"

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
// MEMD_FLASH_CONFIG_T
// -----------------------------------------------------------------------------
/// That type describes the settings to apply to configure the flash driver
/// to get the optimal performances.
// =============================================================================
typedef struct
{
    CONST HAL_EBC_CS_CFG_T csConfig;
} MEMD_FLASH_CONFIG_T;

// =============================================================================
// MEMD_RAM_CONFIG_T
// -----------------------------------------------------------------------------
/// That type describes the settings to apply to configure the ram driver
/// to get the optimal performances.
// =============================================================================
typedef struct
{
    CONST HAL_EBC_CS_CFG_T csConfig;
} MEMD_RAM_CONFIG_T;

// =============================================================================
// RDA_PSRAM_TYPE_T
// -----------------------------------------------------------------------------
/// PSRAM type IDs.
// =============================================================================
typedef enum
{
    RDA_PSRAM_DEFAULT,
    RDA_PSRAM_APTIC,
    RDA_PSRAM_ETRON,
    RDA_PSRAM_WINBOND,
    RDA_PSRAM_EMC,

    RDA_PSRAM_QTY,
    RDA_PSRAM_UNKNOWN = RDA_PSRAM_QTY,

} RDA_PSRAM_TYPE_T;


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================


// =============================================================================
//  FUNCTIONS
// =============================================================================


#endif // _MEMD_CONFIG_H

