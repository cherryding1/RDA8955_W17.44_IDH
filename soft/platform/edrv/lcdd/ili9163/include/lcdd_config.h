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

#include "hal_ebc.h"


#ifndef _LCDD_CONFIG_H_
#define _LCDD_CONFIG_H_



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
// LCDD_SCREEN_CONFIG_T
// -----------------------------------------------------------------------------
/// Type of the configuration structure for a AU screen.
// =============================================================================
typedef struct
{
    /// CS on which the screen is plugged.
    HAL_EBC_CS_T     csNum;
    /// EBC CS configuration.
    HAL_EBC_CS_CFG_T csConfig;
    /// MA line number, on the external address bus, used to select between
    /// the command and data register. Depending one the chip select used,
    /// the addressable size implies that all the address lines cannot be used
    /// All value between 0 and 18 should be correct.
    UINT32  rsLine;
} LCDD_SCREEN_CONFIG_T;


// =============================================================================
// LCDD_CONFIG_T
// -----------------------------------------------------------------------------
/// Configuration structure for the AU LCDD driver, including both screens.
// =============================================================================
typedef struct
{
    LCDD_SCREEN_CONFIG_T main;
    LCDD_SCREEN_CONFIG_T sub;
} LCDD_CONFIG_T;




// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

//

// =============================================================================
//  FUNCTIONS
// =============================================================================

#endif // _LCDD_CONFIG_H_


