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

#ifndef _TSD_CONFIG_H_
#define _TSD_CONFIG_H_

#include "cs_types.h"
#include "hal_gpio.h"

// =============================================================================
// TSD_CONFIG_T
// -----------------------------------------------------------------------------
/// The type TSD_CONFIG_T is defined in tsd_m.h using the structure
/// #TSD_CONFIG_STRUCT_T, the following macro allow to map the correct structure
/// and keep the actual structures different between each models for the
/// documentation.
///
/// Make sure to include tsd_config.h before tsd_m.h !
// =============================================================================
#define TSD_CONFIG_STRUCT_T TSD_CONFIG_CST016_STRUCT_T

#include "tsd_m.h"

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// TSD_CONFIG_T
// -----------------------------------------------------------------------------
/// Configuration structure for the TSD driver CS
// =============================================================================
struct TSD_CONFIG_CST016_STRUCT_T
{
    /// I2C bus used to configure the chip
    HAL_I2C_BUS_ID_T        i2cBusId;
    /// I2C bus used to configure the chip
    HAL_I2C_BPS_T           i2cBps;
    /// GPIO Pin used for Rst
    HAL_GPIO_GPIO_ID_T      pinReset;
    /// GPIO Pin used for PEN IRQ Detection
    HAL_GPIO_GPIO_ID_T      penGpio;
    /// Timier value for debouncing check (in ticks) after pen Irq
    /// This is the smaller time unit used in the driver.
    /// #downPeriod, #upPeriod and TSD_REPORTING_MODE_T.repetitionPeriod
    /// are multiple of this value.
    UINT32 debounceTime;
    /// Period of debouncing time ( in # checks ) after what we consider the pen down.
    UINT16 downPeriod;
    /// Period of debouncing time ( in # checks ) after what we consider the pen up.
    UINT16 upPeriod;
    /// Maximum difference between two samples of the same point.
    UINT32 maxError;
};

#endif // _TSD_CONFIG_H_
