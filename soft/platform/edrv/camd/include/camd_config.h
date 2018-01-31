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



#ifndef _CAMD_CONFIG_H_
#define _CAMD_CONFIG_H_

#include "hal_i2c.h"
#include "pmd_m.h"
#ifdef __PRJ_WITH_SPICAM__
#include "hal_gpio.h"
#endif
// =============================================================================
// CAMD_CONFIG_T
// -----------------------------------------------------------------------------
/// Configuration structure for camera driver.
/// Defines parameters such as the I2C bus ID, etc.
// =============================================================================
typedef struct
{
    /// I2C bus ID
    HAL_I2C_BUS_ID_T i2cBusId;
#ifdef __PRJ_WITH_SPICAM__
    /// SPI camera pin :CSB
    HAL_APO_ID_T  spiCamPinCSB;
    /// SPI camera pin :overflow_m
    HAL_APO_ID_T  spiCamPinOverflowM;
    /// SPI camera pin :overflow_s
    HAL_APO_ID_T  spiCamPinOverflowS;
#endif
    PMD_LEVEL_ID_T camera_flash;
} CAMD_CONFIG_T;


#endif // _CAMD_CONFIG_H_











