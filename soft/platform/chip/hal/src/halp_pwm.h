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



#ifndef _HALP_PWM_H_
#define _HALP_PWM_H_

#include "cs_types.h"
#include "hal_sys.h"




// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================



// =============================================================================
//  MACROS
// =============================================================================
// =============================================================================
// HAL_PWM_BASE_DIVIDER
// -----------------------------------------------------------------------------
/// This is the base divider used to calculate the PWM frequency from a 13MHZ
/// system clock. This value allow us to keep a constant PWM clock (309523 Hz)
/// whatever the system frequency is --> the done is kept at the same note
/// through system clock changes.
// =============================================================================
#define HAL_PWM_BASE_DIVIDER    10



// =============================================================================
// HAL_PWM_FREQ
// -----------------------------------------------------------------------------
/// This is the value of the PWM clock rate, using the provided base divider.
// =============================================================================
#define HAL_PWM_FREQ   (13000000/HAL_PWM_BASE_DIVIDER)



// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

//

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_PwmResourceMgmt
// -----------------------------------------------------------------------------
/// Checks if any components in the PWM module are active and requests or
/// releases the resource.  The return value is TRUE if the resource is
/// active and FALSE otherwise.
// =============================================================================
PROTECTED BOOL hal_PwmResourceMgmt(VOID);

// =============================================================================
// hal_PwlSetGlowing
// -----------------------------------------------------------------------------
/// Set which pin is the glowing pwl
// =============================================================================
PROTECTED VOID hal_PwlSetGlowing(HAL_PWL_ID_T id);



#endif // _HALP_PWM_H_



