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



#ifndef _SIMD_CONFIG_H_
#define _SIMD_CONFIG_H_

#include "cs_types.h"
#include "hal_spi.h"
#include "hal_gpio.h"

#define SIMD_CONFIG_STRUCT_T SIMD_CONFIG_ET6302_STRUCT_T



// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// SIMD_CONFIG_ET6302_STRUCT_T
// -----------------------------------------------------------------------------
/// Configuration structure for the SIM driver CS
// =============================================================================
struct SIMD_CONFIG_ET6302_STRUCT_T
{
    HAL_SPI_ID_T            spiBus;
    HAL_SPI_CS_T            spiCs;
};



#endif // _SIMD_CONFIG_H_




