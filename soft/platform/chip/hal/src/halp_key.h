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


#ifndef  _HALP_KEY_H_
#define  _HALP_KEY_H_

#include "cs_types.h"

// ============================================================================
//      hal_KeyIrqHandler
// ----------------------------------------------------------------------------
///     Internal interrupt handler for the keypad physical interruptions.
///
///     It handles the edge detection by starting the timer IRQ.
///     It handles the key timer interruptions by debouncing and calling the user
///     callback function for each key event (up, down or pressed).
///     It manages the hw interrupts of the keypad (clears the IRQ in the module).
///
///     It handles also the interrupt for the GPIO used for the on-off button.
///     About the interrupt for the on-off button: Physically, this button is
///     mapped on a GPIO. The GPIO HAL handler will call this function when an
///     interrupt is detected on the corresponding GPIO. When this rising edge is
///     detected, the behavior is the same as the one that occurs when a normal
///     key is pressed: it disable the edge IRQ and starts the key timer IRQ.
///
///     @param interruptId The Id of the interrruption
// ============================================================================
PROTECTED VOID hal_KeyIrqHandler(UINT8 interruptId);



#endif //  _HALP_KEY_H_
