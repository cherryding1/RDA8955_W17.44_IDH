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

#ifndef _HALP_TCU_H_
#define _HALP_TCU_H_


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// HAL_TCU_IRQ_HANDLER_T
// -----------------------------------------------------------------------------
/// User IRQ handler type
// =============================================================================
typedef VOID (*HAL_TCU_IRQ_HANDLER_T)(VOID);



// =============================================================================
// HAL_TCU_IRQ_ID_T
// -----------------------------------------------------------------------------
/// Use  to discriminate the tcu irqs
// =============================================================================
typedef enum
{
    HAL_TCU0_IRQ = 2,
    HAL_TCU1_IRQ = 3
} HAL_TCU_IRQ_ID_T;





// =============================================================================
//  FUNCTIONS
// =============================================================================




// =============================================================================
// hal_TcuIrqSetHandler
// -----------------------------------------------------------------------------
/// Set the user function to call when a tcu interrupt occurs.
/// @tcu Choose the TCU interrupt that will call this user handler.
/// @handler Handler called by this TCU interrupt.
// =============================================================================
PROTECTED VOID hal_TcuIrqSetHandler(HAL_TCU_IRQ_ID_T tcu, HAL_TCU_IRQ_HANDLER_T handler);


// =============================================================================
// hal_TcuIrqSetMask
// -----------------------------------------------------------------------------
/// Set the interruption mask for the specified TCU IRQ.
/// @param tcu Id of the TCU  IRQ
/// @mask If \c TRUE, the IRQ can be triggered. Oterwise, it can't occur.
// =============================================================================
PROTECTED VOID hal_TcuIrqSetMask(HAL_TCU_IRQ_ID_T tcu, BOOL mask);



// =============================================================================
// hal_TcuIrqGetMask
// -----------------------------------------------------------------------------
//  Gets the interrupt mask of a TCU  IRQ
/// @param tcu A TCU IRQ Id.
/// @return The mask
// =============================================================================
BOOL hal_TcuIrqGetMask(HAL_TCU_IRQ_ID_T tcu);


// =============================================================================
// hal_TcuIrqHandler
// -----------------------------------------------------------------------------
/// TCU IRQ handler called by the IRQ module
/// @param interruptId of the IRQ calling this handler
// =============================================================================
PROTECTED VOID hal_TcuIrqHandler(UINT8 interruptId);


#endif



