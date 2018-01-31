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
//  MACROS
// =============================================================================

// =============================================================================
// LCDD_CONFIG_T
// -----------------------------------------------------------------------------
/// The type TGT_LCDD_CONFIG_T is defined in tgt_lcdd_cfg.h using the structure
/// #LCDD_CONFIG_STRUCT_T, the following macro allow to map the correct structure
/// and keep the actual structures different between each models for the
/// documentation.
///
/// Make sure to include lcdd_config.h before lcdd_m.h or tgt_lcdd_cfg.h !
// =============================================================================
#define LCDD_CONFIG_STRUCT_T LCDD_CONFIG_RM68130_GOUDA_STRUCT_T

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// LCDD_CONFIG_STRUCT_T
// -----------------------------------------------------------------------------
/// Configuration structure for the Ili9320 LCD driver, using the Gouda module of the chip.
// =============================================================================
struct LCDD_CONFIG_RM68130_GOUDA_STRUCT_T
{
    HAL_GOUDA_LCD_CONFIG_T   config;
    HAL_GOUDA_LCD_TIMINGS_T  timings[HAL_GOUDA_LCD_TIMINGS_QTY];
};


#endif // _LCDD_CONFIG_H_


