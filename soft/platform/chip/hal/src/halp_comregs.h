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


#ifndef  _HALP_COMREGS_H_
#define  _HALP_COMREGS_H_

#include "cs_types.h"

// =============================================================================
// hal_ComregsIrqHandler
// -----------------------------------------------------------------------------
/// Clear the IRQ and call the IRQ handler user function.
/// @param interrupt_id Enables to choose one of the two interrupt
///     lines (\c comregs0_irq or comregs1_irq)
// =============================================================================
PROTECTED VOID hal_ComregsIrqHandler (UINT8 interruptId);


// =============================================================================
// hal_ApComregsIrqHandler
// -----------------------------------------------------------------------------
/// Clear the IRQ and call the IRQ handler user function.
/// @param interrupt_id Enables to choose one of the two interrupt
///     lines (\c comregs0_ap_irq or comregs1_ap_irq)
// =============================================================================
PROTECTED VOID hal_ApComregsIrqHandler (UINT8 interruptId);


// =============================================================================
// hal_WdComregsIrqHandler
// -----------------------------------------------------------------------------
/// Clear the IRQ and call the IRQ handler user function.
/// @param interrupt_id Enables to choose one of the two interrupt
///     lines (\c comregs0_wd_irq or comregs1_wd_irq)
// =============================================================================
PROTECTED VOID hal_WdComregsIrqHandler (UINT8 interruptId);


#endif // HAL_comregs_IRQ_H
