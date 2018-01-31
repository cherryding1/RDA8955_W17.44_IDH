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
#include "hal_i2c.h"


#ifndef _ATVD_CONFIG_H_
#define _ATVD_CONFIG_H_

// =============================================================================
// ATVD_CONFIG_STRUCT_T
// -----------------------------------------------------------------------------
/// The type ATVD_CONFIG_STRUCT_T is defined in tgt_atvd_cfg.h using the structure
/// #ATVD_CONFIG_STRUCT_T, the following macro allow to map the correct structure
/// and keep the actual structures different between each models for the
/// documentation.
/// Describes the board specific configuration of the RDA5888S BT chip
// ============================================================================
struct ATVD_CONFIG_STRUCT_T
{
    HAL_I2C_BUS_ID_T    i2cBusId;
    HAL_APO_ID_T        pinPdn;
};



// =============================================================================
//  FUNCTIONS
// =============================================================================

///  @} <- End of the atvd group

#endif // _ATVD_CONFIG_H_


