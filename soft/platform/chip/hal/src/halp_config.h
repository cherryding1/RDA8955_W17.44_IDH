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


#ifndef _HALP_CONFIG_H_
#define _HALP_CONFIG_H_

#include "cs_types.h"
#include "hal_config.h"


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

EXPORT CONST HAL_CFG_CONFIG_T* g_halCfg;


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_BoardConfigClk32k
// -----------------------------------------------------------------------------
/// Configure CLK_32K output.
/// @param enable TRUE to configure, and FALSE to disable.
// ============================================================================
PROTECTED VOID hal_BoardConfigClk32k(BOOL enable);


// =============================================================================
// hal_BoardSetup
// -----------------------------------------------------------------------------
/// Apply board dependent configuration to HAL
/// @param halCfg Pointer to HAL configuration structure (from the target
/// module).
// =============================================================================
PROTECTED VOID hal_BoardSetup(CONST HAL_CFG_CONFIG_T* halCfg);


#endif // _HALP_CONFIG_H_

