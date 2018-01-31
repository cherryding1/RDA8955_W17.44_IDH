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

#include "global_macros.h"
#include "sys_irq.h"

#include "hal_fint.h"
#include "hal_mem_map.h"
#include "halp_fint.h"
#include "halp_sys.h"
#include "halp_lps.h"



#define PULSE_ID_2_BITMASK(id) (1<<((id)-SYS_IRQ_TCU0))

// Variable to store the user handler
PRIVATE HAL_FINT_IRQ_HANDLER_T g_fintRegistry = NULL;


// =============================================================================
// hal_FintIrqSetHandler
// -----------------------------------------------------------------------------
/// Set the user function called in case of FINT
/// @param handler Handler function to set
// =============================================================================
PUBLIC VOID hal_FintIrqSetHandler(HAL_FINT_IRQ_HANDLER_T handler)
{
    g_fintRegistry = handler;
}

// =============================================================================
// hal_FintIrqSetMask
// -----------------------------------------------------------------------------
/// Set the mask for the FINT
/// @param mask Mask to set. If \c TRUE, FINT are enabled. If \c FALSE,
/// they don't occur.
// =============================================================================
PUBLIC VOID hal_FintIrqSetMask(BOOL mask)
{
    if (mask)
    {
        hwp_sysIrq->Pulse_Mask_Set = SYS_IRQ_SYS_IRQ_FRAME;
    }
    else
    {
        hwp_sysIrq->Pulse_Mask_Clr = SYS_IRQ_SYS_IRQ_FRAME;
    }
}

// =============================================================================
// hal_FintIrqGetMask
// -----------------------------------------------------------------------------
/// This function is used to recover the FINT mask
/// @return The Fint mask: \c TRUE if FINT can happen, \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_FintIrqGetMask(VOID)
{
    if (hwp_sysIrq->Pulse_Mask_Clr & SYS_IRQ_SYS_IRQ_FRAME)
    {
        return TRUE; // mask set
    }
    else
    {
        return FALSE; // mask not set
    }
}

// =============================================================================
// hal_FintIrqGetStatus
// -----------------------------------------------------------------------------
/// Check the Frame interrupt status.
/// @return \c TRUE if the FINT had occured \n
///         \c FALSE otherwise
// =============================================================================
PUBLIC BOOL HAL_FUNC_INTERNAL hal_FintIrqGetStatus(VOID)
{
    if (hwp_sysIrq->Status & SYS_IRQ_SYS_IRQ_FRAME)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// =============================================================================
// hal_FintIrqHandler
// -----------------------------------------------------------------------------
/// Handles IRQ.
// =============================================================================
PROTECTED VOID HAL_FUNC_INTERNAL hal_FintIrqHandler(UINT8 interruptId)
{
    UINT32 tmpRead;
    hwp_sysIrq->Pulse_Clear = PULSE_ID_2_BITMASK(interruptId);
    tmpRead = hwp_sysIrq->Pulse_Clear;

    // Call user handler
    hal_FintUserHandler();
}

// =============================================================================
// hal_FintUserHandler
// -----------------------------------------------------------------------------
/// called by hal_FintIrqHandler()
// =============================================================================
PROTECTED VOID HAL_FUNC_INTERNAL hal_FintUserHandler(VOID)
{
    // Call user handler
    if (g_fintRegistry)
        g_fintRegistry();
}

