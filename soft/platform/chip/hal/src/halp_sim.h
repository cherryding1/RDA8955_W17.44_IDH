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


#ifndef  _HALP_SIM_H_
#define  _HALP_SIM_H_

#include "cs_types.h"
#include "hal_sys.h"



// ============================================================================
// hal_SimIrqHandler
// ----------------------------------------------------------------------------
/// SIM module irq handler for the IRQ module.
///
/// Clear the IRQ and call the IRQ handler user function.
/// @param interruptId Identifier of the interrupt.
// ============================================================================
PROTECTED VOID hal_SimIrqHandler(UINT8 interruptId);



// ============================================================================
// hal_SimCheckClockStatus
// ----------------------------------------------------------------------------
/// This function checks the clock status of the SIM modules. When it is not
/// needed, the request for a clock is given up (ie request for no clock
/// is done.)
// ============================================================================
PROTECTED VOID hal_SimCheckClockStatus(VOID);



#endif //  _HALP_SIM_H_


