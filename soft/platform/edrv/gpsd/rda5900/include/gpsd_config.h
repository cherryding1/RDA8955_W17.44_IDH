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

#ifndef _GPSD_CONFIG_H_
#define _GPSD_CONFIG_H_

#include "cs_types.h"

#include "hal_i2c.h"
#include "hal_gpio.h"


// =============================================================================
// GPSD_CONFIG_T
// -----------------------------------------------------------------------------
/// The type GPSD_CONFIG_T is defined in GPSD_m.h using the structure
/// #GPSD_CONFIG_STRUCT_T, the following macro allow to map the correct structure
/// and keep the actual structures different between each models for the
/// documentation.
///
/// Make sure to include GPSD_config.h before GPSD_m.h !
// =============================================================================
#define GPSD_CONFIG_STRUCT_T GPSD_CONFIG_RDA5900_STRUCT_T

#include "gpsd_m.h"


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// GPSD_CONFIG_T
// -----------------------------------------------------------------------------
/// Configuration structure for the GPSD driver
// =============================================================================
struct GPSD_CONFIG_RDA5900_STRUCT_T
{
	HAL_I2C_BUS_ID_T i2cBusId;
	HAL_APO_ID_T pinPowerOn;
	HAL_APO_ID_T pinReset;
	HAL_APO_ID_T pinBpWakeupGps_gpio;	
	BOOL pinBpWakeupGps_polarity;
	HAL_APO_ID_T pinGpsWakeupBp_gpio;
	BOOL pinGpsWakeupBp_polarity;
};


#define RDAGPS_DELAY(DURATION)				\
    if (DURATION) {							\
        sxr_Sleep(DURATION MS_WAITING);		\
    }

#endif // _GPSD_CONFIG_H_


