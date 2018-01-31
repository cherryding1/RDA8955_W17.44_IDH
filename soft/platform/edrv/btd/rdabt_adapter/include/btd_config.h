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
#include "hal_i2c.h"
#include "hal_gpio.h"


#ifndef _BTD_CONFIG_H_
#define _BTD_CONFIG_H_

// =============================================================================
// BTD_CONFIG_STRUCT_T
// -----------------------------------------------------------------------------
/// The type BTD_CONFIG_STRUCT_T is defined in tgt_btd_cfg.h using the structure
/// #BTD_CONFIG_STRUCT_T, the following macro allow to map the correct structure
/// and keep the actual structures different between each models for the
/// documentation.
///
// =============================================================================
#define BTD_CONFIG_STRUCT_T BTD_CONFIG_RDABT_STRUCT_T


// =============================================================================
// BTD_CONFIG_ASC3600_STRUCT_T
// -----------------------------------------------------------------------------
/// Describes the board specific configuration of the ASC3600 BT chip
// ============================================================================
struct BTD_CONFIG_RDABT_STRUCT_T
{
    HAL_I2C_BUS_ID_T i2cBusId;
    HAL_APO_ID_T pinReset;
    HAL_APO_ID_T pinWakeUp;
    HAL_APO_ID_T pinSleep;
};


// =============================================================================
//  FUNCTIONS
// =============================================================================

///  @} <- End of the btd group

#endif // _BTD_CONFIG_H_


