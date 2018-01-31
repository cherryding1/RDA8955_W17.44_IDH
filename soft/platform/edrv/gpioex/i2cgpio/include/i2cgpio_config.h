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


#ifndef _GPIOI2C_CONFIG_H_
#define _GPIOI2C_CONFIG_H_

#include "cs_types.h"

#include "gpio_i2c.h"
#include "hal_gpio.h"



// =============================================================================
//
// -----------------------------------------------------------------------------
///
/// Make sure to include gpioic_config.h before
// =============================================================================

#define GPIOI2C_CONFIG_STRUCT_T GPIOI2C_CONFIG_STRUCT_T



// =============================================================================
//
// -----------------------------------------------------------------------------
///
// ============================================================================



struct GPIOI2C_CONFIG_STRUCT_T
{
    /// I2C bus used to configure the chip
    GPIO_I2C_BPS_T           i2c_gpio_Bps;
    HAL_GPIO_GPIO_ID_T      scl_i2c_gpio;  // if do not used the gpio SCL ,set it HAL_GPIO_NONE
	UINT32					scl_i2c_gpio_reg_offset;
	UINT32					scl_i2c_gpio_reg_val;
    HAL_GPIO_GPO_ID_T       scl_i2c_gpo;   // if do not used gpo for SCL ,set it HAL_GPO_NONE
    HAL_GPIO_GPIO_ID_T      sda_i2c;
	UINT32					sda_i2c_reg_offset;
	UINT32					sda_i2c_reg_val;
};

#endif // 


