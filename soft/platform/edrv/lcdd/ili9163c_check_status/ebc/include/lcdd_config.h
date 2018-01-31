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

#include "hal_gouda.h"


#ifndef _LCDD_CONFIG_H_
#define _LCDD_CONFIG_H_

// =============================================================================
// LCDD_CONFIG_T
// -----------------------------------------------------------------------------
/// The type TGT_LCDD_CONFIG_T is defined in tgt_lcdd_cfg.h using the structure
/// #LCDD_CONFIG_STRUCT_T, the following macro allow to map the correct structure
/// and keep the actual structures different between each models for the
/// documentation.
///
/// Make sure to include fmd_config.h before fmd_m.h or tgt_fmd_cfg.h !
// =============================================================================
#define LCDD_CONFIG_STRUCT_T LCDD_CONFIG_S6B33BF_EBC_STRUCT_T



// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// LCDD_CONFIG_STRUCT_T
// -----------------------------------------------------------------------------
/// Configuration structure for the S6B33BF LCDD driver, using the EBC bus.
// =============================================================================
struct LCDD_CONFIG_S6B33BF_EBC_STRUCT_T
{
    /// CS on which the screen is plugged.
    HAL_EBC_CS_T     csNum;
    /// EBC CS configuration.
    HAL_EBC_CS_CFG_T csConfig;
    /// MA line number, on the external address bus, used to select between
    /// the command and data register. Depending one the chip select used,
    /// the addressable size implies that all the address lines cannot be used
    /// All value between 0 and 18 should be correct.
    UINT32  rsLine;
};



#endif // _LCDD_CONFIG_H_


