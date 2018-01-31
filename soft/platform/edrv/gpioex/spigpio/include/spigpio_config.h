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


#ifndef _GPIOSPI_CONFIG_H_
#define _GPIOSPI_CONFIG_H_

#include "cs_types.h"

#include "gpio_spi.h"
#include "hal_gpio.h"



// =============================================================================
//
// -----------------------------------------------------------------------------
///
/// Make sure to include gpioic_config.h before
// =============================================================================

#define GPIOSPI_CONFIG_STRUCT_T GPIO_SPI_CONFIG_STRUCT_T



// =============================================================================
//
// -----------------------------------------------------------------------------
///
// ============================================================================



struct GPIO_SPI_CONFIG_STRUCT_T
{
    /// SPI bus used to configure the chip

    HAL_APO_ID_T      scs_spi;

    HAL_APO_ID_T      scl_spi;

    HAL_APO_ID_T      sdi_spi;

    HAL_APO_ID_T      sdo_spi;

};

#endif // 


