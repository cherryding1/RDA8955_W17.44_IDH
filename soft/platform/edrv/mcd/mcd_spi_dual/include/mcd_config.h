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
#include "hal_spi.h"
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

#define MCD_CONFIG_STRUCT_T MCD_CONFIG_DUAL_SDMMC_STRUCT_T

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// MCD_CONFIG_T
// -----------------------------------------------------------------------------
/// Configuration structure for the MCD_SPI driver CS
// =============================================================================
struct MCD_CONFIG_DUAL_SDMMC_STRUCT_T
{
    HAL_SPI_ID_T            usedSpiId;
    HAL_SPI_CS_T            usedSpiCs0;
    HAL_SPI_CS_T            usedSpiCs1;
    HAL_GPIO_GPIO_ID_T      tmpGpioOverCs0;
    HAL_GPIO_GPIO_ID_T      tmpGpioOverCs1;
    HAL_GPIO_GPIO_ID_T      cardDetectGpio;
} ;




// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

//

// =============================================================================
//  FUNCTIONS
// =============================================================================

#endif // _MCD_CONFIG_H_


