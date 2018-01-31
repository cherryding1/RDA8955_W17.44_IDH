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
#include "hal_gpio.h"


#ifndef _MCD_CONFIG_H_
#define _MCD_CONFIG_H_



// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================


// =============================================================================
//  MACROS
// =============================================================================

#define MCD_CONFIG_STRUCT_T MCD_CONFIG_SDMMC_STRUCT_T

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// MCD_CONFIG_T
// -----------------------------------------------------------------------------
/// Configuration structure for the MCD driver on SDMMC Interface
// =============================================================================
struct MCD_CONFIG_SDMMC_STRUCT_T
{
    /// Gpio Connected to socket to detect card insertion/removal .
    /// (set to #HAL_GPIO_NONE) if not available.
    HAL_GPIO_GPIO_ID_T  cardDetectGpio;
    /// Define the polarity of the above GPIO:
    /// \c TRUE GPIO is high when card is present,
    /// \c FALSE GPIO is low when card is present.
    BOOL                gpioCardDetectHigh;
    /// Wether or not the DAT3/CD line has a pull-down and can be used for
    /// minimalist new card detection (no insertion/removal interrupt.)
    BOOL                dat3HasPullDown;
};




// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

//

// =============================================================================
//  FUNCTIONS
// =============================================================================

#endif // _MCD_CONFIG_H_


