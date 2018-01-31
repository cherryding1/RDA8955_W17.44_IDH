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



#ifndef _DUALSIM_CONFIG_H_
#define _DUALSIM_CONFIG_H_

#include "cs_types.h"
#include "hal_spi.h"
#include "hal_gpio.h"




// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// DUALSIM_CFG_T
// -----------------------------------------------------------------------------
/// Configuration structure for the DUALSIM driver CS
// =============================================================================
typedef struct
{
    HAL_SPI_ID_T            spiBus;
    HAL_SPI_CS_T            spiCs;

} DUALSIM_CONFIG_T;



#endif // _DUALSIM_CONFIG_H_




