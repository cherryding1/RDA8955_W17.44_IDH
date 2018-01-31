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



#ifndef  _HALP_GPIO_H_
#define  _HALP_GPIO_H_

#include "hal_gpio.h"

// =============================================================================
// MACROS
// =============================================================================

// =============================================================================
// HAL_GPIO_BIT
// -----------------------------------------------------------------------------
/// This macro is used by internal code to convert gpio number to bit.
/// It masks the upper bit so it can be used directly with #HAL_GPIO_GPIO_ID_T.
// =============================================================================
#define HAL_GPIO_BIT(n)    (1<<((n)&0x3f))

// =============================================================================
// HAL_GPO_BIT
// -----------------------------------------------------------------------------
/// This macro is used by internal code to convert gpio number to bit.
/// It masks the upper bit so it can be used directly with #HAL_GPIO_GPO_ID_T.
// =============================================================================
#define HAL_GPO_BIT(n)    (1<<((n)&0x3f))




// =============================================================================
// hal_GpioIrqHandler
// -----------------------------------------------------------------------------
/// GPIO module IRQ handler
///
///     Clear the IRQ and call the IRQ handler
///     user function
///     @param interruptId The interruption ID
// =============================================================================
PROTECTED VOID hal_GpioIrqHandler(UINT8 interruptId);


#endif //HAL_GPIO_H
