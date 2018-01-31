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


#ifndef _GSENSOR_CONFIG_H_
#define _GSENSOR_CONFIG_H_

#include "cs_types.h"

#include "hal_i2c.h"
#include "hal_gpio.h"



// =============================================================================
// GSENSOR_CONFIG_STRUCT_T
// -----------------------------------------------------------------------------
/// The type GSENSOR_CONFIG_STRUCT_T is defined in tgt_fmd_cfg.h using the structure
/// #GSENSOR_CONFIG_STRUCT_T, the following macro allow to map the correct structure
/// and keep the actual structures different between each models for the
/// documentation.
///
/// Make sure to include gsensor_config.h before gsensor_m.h or tgt_gsensor_cfg.h !
// =============================================================================

#define GSENSOR_CONFIG_STRUCT_T GSENSOR_CONFIG_MMA7660FC_STRUCT_T

#include "gsensor_m.h"


// =============================================================================
// GSENSOR_CONFIG_MMA7660FC_STRUCT_T
// -----------------------------------------------------------------------------
/// Describes the board specific configuration of the gsensor chip
// ============================================================================

#if 0
struct GSENSOR_CONFIG_MMA7660FC_STRUCT_T
{
    /// I2C bus used to configure the chip
    HAL_I2C_BUS_ID_T        i2cBusId;
    HAL_GPIO_GPIO_ID_T      gsensorGpio;
    HAL_APO_ID_T            scl_i2c;
    HAL_GPIO_GPIO_ID_T      sda_i2c;
};
#endif

struct GSENSOR_CONFIG_MMA7660FC_STRUCT_T
{
    /// I2C bus used to configure the chip

    HAL_I2C_BUS_ID_T        i2cBusId;

    HAL_GPIO_GPIO_ID_T      gsensorGpio;

    HAL_GPIO_GPIO_ID_T      scl_i2c_gpio;  // if do not used the gpio SCL ,set it HAL_GPIO_NONE

    HAL_GPIO_GPO_ID_T       scl_i2c_gpo;   // if do not used gpo for SCL ,set it HAL_GPO_NONE

    HAL_GPIO_GPIO_ID_T      sda_i2c;
};

#endif // 


