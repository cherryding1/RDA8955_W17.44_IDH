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

#include "chip_id.h"

#include "globals.h"
#include "global_macros.h"
#include "comregs.h"

#include "halp_debug.h"
#include "hal_comregs.h"


PRIVATE CONST CHAR *gc_errImproperMbxSlot =
    "Improper mbx slot";

/// Private array for storing user irq handler.
PRIVATE HAL_COMREGS_IRQ_HANDLER_T g_halComregsRegistry[HAL_COMREGS_XCPU_IRQ_SLOT_QTY] =
{NULL,NULL};

#ifdef CHIP_HAS_AP
PRIVATE HAL_APCOMREGS_IRQ_HANDLER_T g_halApComregsRegistry[HAL_APCOMREGS_XCPU_IRQ_SLOT_QTY] =
{NULL,NULL};
#endif

#ifdef CHIP_HAS_WCPU
PRIVATE HAL_WDCOMREGS_IRQ_HANDLER_T g_halWdComregsRegistry[HAL_COMREGS_XCPU_IRQ_SLOT_QTY] =
{NULL,NULL};
#endif


// =============================================================================
// hal_ComregsGetSnap
// -----------------------------------------------------------------------------
/// Get the comregs IT snapshot value
// =============================================================================
PUBLIC UINT32 HAL_FUNC_INTERNAL hal_ComregsGetSnap(VOID)
{
    return hwp_sysComregs->Snapshot;
}

#ifdef CHIP_DIE_8909
// =============================================================================
// hal_ComregsGetSnapGsm
// -----------------------------------------------------------------------------
/// Get the comregs IT snapshot value
// =============================================================================
PUBLIC UINT32 HAL_FUNC_INTERNAL hal_ComregsGetSnapGsm(VOID)
{
    return GET_BITFIELD(hwp_sysComregs->Snapshot, COMREGS_SNAPSHOT_GSM);
}
#endif

// =============================================================================
// hal_ComregsSetSnapCfg
// -----------------------------------------------------------------------------
/// Set the snapshot configuration register
/// @param isTrio When \c TRUE, the snapshot wrap value is 3 \n
///                        \c FALSE, it is 2.
// =============================================================================
PUBLIC VOID hal_ComregsSetSnapCfg(BOOL isTrio)
{
#if (CHIP_HAS_ASYNC_BCPU)
    // No configuration anymore
#else // (!CHIP_HAS_ASYNC_BCPU)
    hwp_sysComregs->Snapshot_Cfg = (isTrio?1:0);
#endif // (CHIP_HAS_ASYNC_BCPU)
}



// IRQ handling functions

// =============================================================================
// hal_ComregsSetIrqHandler
// -----------------------------------------------------------------------------
/// Set an function called when an irq occurs
/// on the corresponding slot
///
/// @param slot The slot to associate an handler with
/// @param handler The handler function
///
// =============================================================================
PUBLIC VOID hal_ComregsSetIrqHandler(HAL_COMREGS_XCPU_IRQ_SLOT_T slot,
                                     HAL_COMREGS_IRQ_HANDLER_T handler)
{
    HAL_ASSERT((slot >= 0 && slot < HAL_COMREGS_XCPU_IRQ_SLOT_QTY), "Mbx overslot %d", slot);
    g_halComregsRegistry[slot] = handler;
}


// =============================================================================
// hal_ComregsSetMask
// -----------------------------------------------------------------------------
/// Set the mask of a slot
///
/// The width is the one specified for each slot.
/// Only the lowest bits are used. This mask operate on a
/// set-clear basis. This is the set function
///
/// @param slot Slot whose mask is to be set
/// @param mask Mask to set. Bits to '1' will be set. The ones to
/// '0' remain unchanged
// =============================================================================
PUBLIC VOID HAL_FUNC_INTERNAL hal_ComregsSetMask(HAL_COMREGS_XCPU_IRQ_SLOT_T slot, HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_COMREGS_XCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_sysComregs->Mask_Set = COMREGS_IRQ0_MASK_SET(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_COMREGS_XCPU_IRQ_SLOT_1, gc_errImproperMbxSlot);
        // built the mask as it should be in the hw register
        hwp_sysComregs->Mask_Set = COMREGS_IRQ1_MASK_SET(u.reg);
    }
}


// =============================================================================
// hal_ComregsClrMask
// -----------------------------------------------------------------------------
/// Clear the mask of a slot
///
/// The width is the one specified for each slot.
/// Only the lowest bits are used. This mask operate on a
/// set-clear basis. This is the clear function
///
/// @param slot Slot whose mask is to be set
/// @param mask Mask to set. Bits to '1' will be cleared. The ones to
/// '0' remain unchanged
// =============================================================================
PUBLIC VOID HAL_FUNC_INTERNAL hal_ComregsClrMask(HAL_COMREGS_XCPU_IRQ_SLOT_T slot, HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_COMREGS_XCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_sysComregs->Mask_Clr = COMREGS_IRQ0_MASK_CLR(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_COMREGS_XCPU_IRQ_SLOT_1, gc_errImproperMbxSlot);
        // built the mask as it should be in the hw register
        hwp_sysComregs->Mask_Clr = COMREGS_IRQ1_MASK_CLR(u.reg);
    }
}


// =============================================================================
// hal_ComregsGetMask
// -----------------------------------------------------------------------------
/// Get the mask of a slot
///
/// @param slot The slot whose masks is wanted
/// @return The value of the mask set for the slot given
/// as a parameter.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_ComregsGetMask(HAL_COMREGS_XCPU_IRQ_SLOT_T slot)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    if(slot == HAL_COMREGS_XCPU_IRQ_SLOT_0)
    {
        u.reg = GET_BITFIELD(hwp_sysComregs->Mask_Set,COMREGS_IRQ0_MASK_SET);
    }
    else
    {
        HAL_ASSERT(slot == HAL_COMREGS_XCPU_IRQ_SLOT_1, gc_errImproperMbxSlot);
        u.reg = GET_BITFIELD(hwp_sysComregs->Mask_Set,COMREGS_IRQ1_MASK_SET);
    }

    return u.bitfield;
}


// =============================================================================
// hal_ComregsIrqHandler
// -----------------------------------------------------------------------------
/// Clear the IRQ and call the IRQ handler user function.
/// @param interrupt_id Enables to choose one of the two interrupt
///     lines (\c comregs0_irq or comregs1_irq)
// =============================================================================
PROTECTED VOID hal_ComregsIrqHandler (UINT8 interruptId)
{
    UINT32 tmpRead;
    HAL_COMREGS_IRQ_STATUS_T status;

    if (interruptId == SYS_IRQ_COM0)
    {
        // mask for the first slot
        status.status =  GET_BITFIELD(hwp_sysComregs->Cause, COMREGS_IRQ0_CAUSE);
        // Clear ITs
        hwp_sysComregs->ItReg_Clr = COMREGS_IRQ0_CAUSE(status.status);

        tmpRead = hwp_sysComregs->ItReg_Clr;

        if(g_halComregsRegistry[HAL_COMREGS_XCPU_IRQ_SLOT_0])
        {
            g_halComregsRegistry[HAL_COMREGS_XCPU_IRQ_SLOT_0](status);
        }
    }
    else if (interruptId == SYS_IRQ_COM1)
    {
        // interruptId == SYS_IRQ_COM1
        // mask for the second slot
        status.status =  GET_BITFIELD(hwp_sysComregs->Cause, COMREGS_IRQ1_CAUSE);
        // Clear ITs
        hwp_sysComregs->ItReg_Clr = COMREGS_IRQ1_CAUSE(status.status);

        tmpRead = hwp_sysComregs->ItReg_Clr;

        if(g_halComregsRegistry[HAL_COMREGS_XCPU_IRQ_SLOT_1])
        {
            g_halComregsRegistry[HAL_COMREGS_XCPU_IRQ_SLOT_1](status);
        }
    }
}


// =============================================================================
// hal_ComregsBcpuIrqTrigger
// -----------------------------------------------------------------------------
/// Trigger IRQs on the BCPU
///
/// @param slot BCPU IRQ slot on which trigger the IRQ.
/// @param mask IRQ cause sent to the BPCU
/// leave the corresponding bit unchanged.
///
// =============================================================================
PUBLIC VOID hal_ComregsBcpuIrqTrigger(HAL_COMREGS_BCPU_IRQ_SLOT_T slot, HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_COMREGS_BCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_sysComregs->ItReg_Set = COMREGS_IRQ0_SET(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_COMREGS_BCPU_IRQ_SLOT_1, gc_errImproperMbxSlot);
        // built the mask as it should be in the hw register
        hwp_sysComregs->ItReg_Set = COMREGS_IRQ1_SET(u.reg);
    }
}


// =============================================================================
// hal_ComregsXcpuIrqTrigger
// -----------------------------------------------------------------------------
/// Trigger IRQs on the XCPU
///
/// @param slot XCPU IRQ slot on which trigger the IRQ.
/// @param mask IRQ cause sent to the BPCU
/// leave the corresponding bit unchanged.
///
// =============================================================================
PUBLIC VOID hal_ComregsXcpuIrqTrigger(HAL_COMREGS_XCPU_IRQ_SLOT_T slot, HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_COMREGS_XCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_bbComregs->ItReg_Set = COMREGS_IRQ0_SET(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_COMREGS_XCPU_IRQ_SLOT_1, gc_errImproperMbxSlot);
        // built the mask as it should be in the hw register
        hwp_bbComregs->ItReg_Set = COMREGS_IRQ1_SET(u.reg);
    }
}


// =============================================================================
// hal_ComregsBcpuIrqGetStatus
// -----------------------------------------------------------------------------
/// Get status of IRQ triggerd on the BCPU
/// @param slot BCPU IRQ slot from which to get the status.
/// @return The interrupt status of the given slot.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_ComregsBcpuIrqGetStatus(HAL_COMREGS_BCPU_IRQ_SLOT_T slot)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    if(slot == HAL_COMREGS_BCPU_IRQ_SLOT_0)
    {
        u.reg = GET_BITFIELD(hwp_bbComregs->ItReg_Clr,COMREGS_IRQ0_SET);
    }
    else
    {
        HAL_ASSERT(slot == HAL_COMREGS_BCPU_IRQ_SLOT_1, gc_errImproperMbxSlot);
        u.reg = GET_BITFIELD(hwp_bbComregs->ItReg_Clr,COMREGS_IRQ1_SET);
    }

    return u.bitfield;
}


// =============================================================================
// hal_ComregsXcpuIrqGetStatus
// -----------------------------------------------------------------------------
/// Get status of IRQ triggerd on the XCPU
/// @param slot BCPU IRQ slot from which to get the status.
/// @return The interrupt status of the given slot.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_ComregsXcpuIrqGetStatus(HAL_COMREGS_XCPU_IRQ_SLOT_T slot)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    if(slot == HAL_COMREGS_XCPU_IRQ_SLOT_0)
    {
        u.reg = GET_BITFIELD(hwp_sysComregs->ItReg_Clr,COMREGS_IRQ0_SET);
    }
    else
    {
        HAL_ASSERT(slot == HAL_COMREGS_XCPU_IRQ_SLOT_1, gc_errImproperMbxSlot);
        u.reg = GET_BITFIELD(hwp_sysComregs->ItReg_Clr,COMREGS_IRQ1_SET);
    }

    return u.bitfield;
}


// =============================================================================
// hal_ComregsBcpuIrqClearCause
// -----------------------------------------------------------------------------
/// Clear
/// @param slot BCPU IRQ slot on which clear the IRQ cause.
/// @param mask IRQ cause to clear, leaving the others unchanged.
// =============================================================================
PUBLIC VOID hal_ComregsBcpuIrqClearCause(HAL_COMREGS_BCPU_IRQ_SLOT_T slot, HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_COMREGS_BCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_bbComregs->ItReg_Clr = COMREGS_IRQ0_CLR(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_COMREGS_BCPU_IRQ_SLOT_1, gc_errImproperMbxSlot);
        // built the mask as it should be in the hw register
        hwp_bbComregs->ItReg_Clr = COMREGS_IRQ1_CLR(u.reg);
    }
}


// =============================================================================
// hal_ComregsXcpuIrqClearCause
// -----------------------------------------------------------------------------
/// Clear IRQs on the XCPU
///
/// @param slot XCPU IRQ slot on which clear the IRQ cause.
/// @param mask IRQ cause to clear, leaving the others unchanged.
// =============================================================================
PUBLIC VOID hal_ComregsXcpuIrqClearCause(HAL_COMREGS_XCPU_IRQ_SLOT_T slot, HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_COMREGS_XCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_sysComregs->ItReg_Clr = COMREGS_IRQ0_CLR(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_COMREGS_XCPU_IRQ_SLOT_1, gc_errImproperMbxSlot);
        // built the mask as it should be in the hw register
        hwp_sysComregs->ItReg_Clr = COMREGS_IRQ1_CLR(u.reg);
    }
}


#ifdef CHIP_HAS_AP
// =============================================================================
// hal_ApComregsSetIrqHandler
// -----------------------------------------------------------------------------
/// Set an function called when an irq occurs
/// on the corresponding slot
///
/// @param slot The slot to associate an handler with
/// @param handler The handler function
///
// =============================================================================
PUBLIC VOID hal_ApComregsSetIrqHandler(HAL_APCOMREGS_XCPU_IRQ_SLOT_T slot, HAL_APCOMREGS_IRQ_HANDLER_T handler)
{
    HAL_ASSERT((slot >= 0 && slot < HAL_APCOMREGS_XCPU_IRQ_SLOT_QTY), "Mbx overslot %d", slot);
    g_halApComregsRegistry[slot] = handler;
}


// =============================================================================
// hal_ApComregsSetMask
// -----------------------------------------------------------------------------
/// Set the mask of a slot
///
/// The width is the one specified for each slot.
/// Only the lowest bits are used. This mask operate on a
/// set-clear basis. This is the set function
///
/// @param slot Slot whose mask is to be set
/// @param mask Mask to set. Bits to '1' will be set. The ones to
/// '0' remain unchanged
// =============================================================================
PUBLIC VOID hal_ApComregsSetMask(HAL_APCOMREGS_XCPU_IRQ_SLOT_T slot, HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_APCOMREGS_XCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_sysComregsAp->Mask_Set = COMREGS_IRQ0_MASK_SET(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_APCOMREGS_XCPU_IRQ_SLOT_1, gc_errImproperMbxSlot);
        // built the mask as it should be in the hw register
        hwp_sysComregsAp->Mask_Set = COMREGS_IRQ1_MASK_SET(u.reg);
    }
}


// =============================================================================
// hal_ApComregsClrMask
// -----------------------------------------------------------------------------
/// Clear the mask of a slot
///
/// The width is the one specified for each slot.
/// Only the lowest bits are used. This mask operate on a
/// set-clear basis. This is the clear function
///
/// @param slot Slot whose mask is to be set
/// @param mask Mask to set. Bits to '1' will be cleared. The ones to
/// '0' remain unchanged
// =============================================================================
PUBLIC VOID hal_ApComregsClrMask(HAL_APCOMREGS_XCPU_IRQ_SLOT_T slot, HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_APCOMREGS_XCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_sysComregsAp->Mask_Clr = COMREGS_IRQ0_MASK_CLR(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_APCOMREGS_XCPU_IRQ_SLOT_1, gc_errImproperMbxSlot);
        // built the mask as it should be in the hw register
        hwp_sysComregsAp->Mask_Clr = COMREGS_IRQ1_MASK_CLR(u.reg);
    }
}


// =============================================================================
// hal_ApComregsGetMask
// -----------------------------------------------------------------------------
/// Get the mask of a slot
///
/// @param slot The slot whose masks is wanted
/// @return The value of the mask set for the slot given
/// as a parameter.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_ApComregsGetMask(HAL_APCOMREGS_XCPU_IRQ_SLOT_T slot)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    if(slot == HAL_APCOMREGS_XCPU_IRQ_SLOT_0)
    {
        u.reg = GET_BITFIELD(hwp_sysComregsAp->Mask_Set,COMREGS_IRQ0_MASK_SET);
    }
    else
    {
        HAL_ASSERT(slot == HAL_APCOMREGS_XCPU_IRQ_SLOT_1, gc_errImproperMbxSlot);
        u.reg = GET_BITFIELD(hwp_sysComregsAp->Mask_Set,COMREGS_IRQ1_MASK_SET);
    }

    return u.bitfield;
}


// =============================================================================
// hal_ApComregsIrqHandler
// -----------------------------------------------------------------------------
/// Clear the IRQ and call the IRQ handler user function.
/// @param interrupt_id Enables to choose one of the two interrupt
///     lines (\c comregs0_ap_irq or comregs1_ap_irq)
// =============================================================================
PROTECTED VOID hal_ApComregsIrqHandler(UINT8 interruptId)
{
    UINT32 tmpRead;
    HAL_COMREGS_IRQ_STATUS_T status;

    if (interruptId == SYS_IRQ_COM0_AP)
    {
        // mask for the first slot
        status.status =  GET_BITFIELD(hwp_sysComregsAp->Cause, COMREGS_IRQ0_CAUSE);
        // Clear COM0 ITs
        do
        {
            hwp_sysComregsAp->ItReg_Clr = COMREGS_IRQ0_CAUSE(status.status);
            tmpRead = hwp_sysComregsAp->ItReg_Clr;
        }
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8810)
        // In order to avoid the race condition in h/w,
        // keep clearing until the same status is updated on the other side
        while ((hwp_apComregs->ItReg_Set & COMREGS_IRQ0_CAUSE(status.status)) != 0);
#else
        while (0);
#endif

        if(g_halApComregsRegistry[HAL_APCOMREGS_XCPU_IRQ_SLOT_0])
        {
            g_halApComregsRegistry[HAL_APCOMREGS_XCPU_IRQ_SLOT_0](status);
        }
    }
    else if (interruptId == SYS_IRQ_COM1_AP)
    {
        // mask for the second slot
        status.status=  GET_BITFIELD(hwp_sysComregsAp->Cause, COMREGS_IRQ1_CAUSE);
        // Clear COM1 ITs
        // In order to avoid the race condition in h/w,
        // keep clearing until the same status is updated on the other side
        do
        {
            hwp_sysComregsAp->ItReg_Clr = COMREGS_IRQ1_CAUSE(status.status);
            tmpRead = hwp_sysComregsAp->ItReg_Clr;
        }
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8810)
        // In order to avoid the race condition in h/w,
        // keep clearing until the same status is updated on the other side
        while ((hwp_apComregs->ItReg_Set & COMREGS_IRQ1_CAUSE(status.status)) != 0);
#else
        while (0);
#endif

        if(g_halApComregsRegistry[HAL_APCOMREGS_XCPU_IRQ_SLOT_1])
        {
            g_halApComregsRegistry[HAL_APCOMREGS_XCPU_IRQ_SLOT_1](status);
        }
    }
}


// =============================================================================
// hal_ApComregsApIrqTrigger
// -----------------------------------------------------------------------------
/// Trigger IRQs on the AP from XCPU
///
/// @param slot AP IRQ slot on which trigger the IRQ.
/// @param mask IRQ cause sent to the AP
/// leave the corresponding bit unchanged.
// =============================================================================
PUBLIC VOID hal_ApComregsApIrqTrigger(HAL_APCOMREGS_AP_IRQ_SLOT_T slot, HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_APCOMREGS_AP_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_sysComregsAp->ItReg_Set = COMREGS_IRQ0_SET(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_APCOMREGS_AP_IRQ_SLOT_1, gc_errImproperMbxSlot);
        // built the mask as it should be in the hw register
        hwp_sysComregsAp->ItReg_Set = COMREGS_IRQ1_SET(u.reg);
    }
}


// =============================================================================
// hal_ApComregsXcpuIrqTrigger
// -----------------------------------------------------------------------------
/// Trigger IRQs on the XCPU from AP
/// TODO: Frequent access to AP registers is not stable?
///
/// @param slot AP IRQ slot on which trigger the IRQ.
/// @param mask IRQ cause sent to the AP
/// leave the corresponding bit unchanged.
// =============================================================================
PUBLIC VOID hal_ApComregsXcpuIrqTrigger(HAL_APCOMREGS_XCPU_IRQ_SLOT_T slot, HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_APCOMREGS_XCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_apComregs->ItReg_Set = COMREGS_IRQ0_SET(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_APCOMREGS_XCPU_IRQ_SLOT_1, gc_errImproperMbxSlot);
        // built the mask as it should be in the hw register
        hwp_apComregs->ItReg_Set = COMREGS_IRQ1_SET(u.reg);
    }
}


// =============================================================================
// hal_ApComregsXcpuIrqGetStatus
// -----------------------------------------------------------------------------
/// Get status of IRQ triggerd on the XCPU from AP
/// @param slot AP XCPU IRQ slot from which to get the status.
/// @return The interrupt status of the given slot.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_ApComregsXcpuIrqGetStatus(HAL_APCOMREGS_XCPU_IRQ_SLOT_T slot)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    if(slot == HAL_APCOMREGS_XCPU_IRQ_SLOT_0)
    {
        u.reg = GET_BITFIELD(hwp_sysComregsAp->ItReg_Clr,COMREGS_IRQ0_SET);
    }
    else
    {
        HAL_ASSERT(slot == HAL_APCOMREGS_XCPU_IRQ_SLOT_1, gc_errImproperMbxSlot);
        u.reg = GET_BITFIELD(hwp_sysComregsAp->ItReg_Clr,COMREGS_IRQ1_SET);
    }

    return u.bitfield;
}


// =============================================================================
// hal_ApComregsXcpuIrqClearCause
// -----------------------------------------------------------------------------
/// Clear IRQs on the XCPU from AP
///
/// @param slot AP XCPU IRQ slot on which clear the IRQ cause.
/// @param mask IRQ cause to clear, leaving the others unchanged.
// =============================================================================
PUBLIC VOID hal_ApComregsXcpuIrqClearCause(HAL_APCOMREGS_XCPU_IRQ_SLOT_T slot, HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_APCOMREGS_XCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_sysComregsAp->ItReg_Clr = COMREGS_IRQ0_CLR(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_APCOMREGS_XCPU_IRQ_SLOT_1, gc_errImproperMbxSlot);
        // built the mask as it should be in the hw register
        hwp_sysComregsAp->ItReg_Clr = COMREGS_IRQ1_CLR(u.reg);
    }
}


// =============================================================================
// hal_ApComregsApIrqGetStatus
// -----------------------------------------------------------------------------
/// Get status of IRQ triggerd on the AP from XCPU
/// @param slot AP XCPU IRQ slot from which to get the status.
/// @return The interrupt status of the given slot.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_ApComregsApIrqGetStatus(HAL_APCOMREGS_AP_IRQ_SLOT_T slot)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    if(slot == HAL_APCOMREGS_AP_IRQ_SLOT_0)
    {
        u.reg = GET_BITFIELD(hwp_sysComregsAp->ItReg_Set,COMREGS_IRQ0_SET);
    }
    else
    {
        HAL_ASSERT(slot == HAL_APCOMREGS_AP_IRQ_SLOT_1, gc_errImproperMbxSlot);
        u.reg = GET_BITFIELD(hwp_sysComregsAp->ItReg_Set,COMREGS_IRQ1_SET);
    }

    return u.bitfield;
}
#endif // CHIP_HAS_AP


#ifdef CHIP_HAS_WCPU
// =============================================================================
// IRQ handling functions
// =============================================================================
// hal_ComregsSetIrqHandler
// -----------------------------------------------------------------------------
/// Set an function called when an irq occurs
/// on the corresponding slot
///
/// @param slot The slot to associate an handler with
/// @param handler The handler function
///
// =============================================================================
PUBLIC VOID hal_WdComregsSetIrqHandler(HAL_WDCOMREGS_XCPU_IRQ_SLOT_T slot, HAL_WDCOMREGS_IRQ_HANDLER_T handler)
{
    HAL_ASSERT((slot >= 0 && slot < HAL_COMREGS_XCPU_IRQ_SLOT_QTY), "Mbx overslot %d", slot);
    g_halWdComregsRegistry[slot] = handler;
}


// =============================================================================
// hal_WdComregsSetMask
// -----------------------------------------------------------------------------
/// Set the mask of a slot
///
/// The width is the one specified for each slot.
/// Only the lowest bits are used. This mask operate on a
/// set-clear basis. This is the set function
///
/// @param slot Slot whose mask is to be set
/// @param mask Mask to set. Bits to '1' will be set. The ones to
/// '0' remain unchanged
// =============================================================================
PUBLIC VOID HAL_FUNC_INTERNAL hal_WdComregsSetMask(HAL_WDCOMREGS_XCPU_IRQ_SLOT_T slot, HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_WDCOMREGS_XCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_sysComregsWd->Mask_Set = COMREGS_IRQ0_MASK_SET(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_COMREGS_XCPU_IRQ_SLOT_1, "Improper mbx slot");
        // built the mask as it should be in the hw register
        hwp_sysComregsWd->Mask_Set = COMREGS_IRQ1_MASK_SET(u.reg);
    }
}


// =============================================================================
// hal_WdComregsClrMask
// -----------------------------------------------------------------------------
/// Clear the mask of a slot
///
/// The width is the one specified for each slot.
/// Only the lowest bits are used. This mask operate on a
/// set-clear basis. This is the clear function
///
/// @param slot Slot whose mask is to be set
/// @param mask Mask to set. Bits to '1' will be cleared. The ones to
/// '0' remain unchanged
// =============================================================================
PUBLIC VOID HAL_FUNC_INTERNAL hal_WdComregsClrMask(HAL_WDCOMREGS_XCPU_IRQ_SLOT_T slot, HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_WDCOMREGS_XCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_sysComregsWd->Mask_Clr = COMREGS_IRQ0_MASK_CLR(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_COMREGS_XCPU_IRQ_SLOT_1, "Improper mbx slot");
        // built the mask as it should be in the hw register
        hwp_sysComregsWd->Mask_Clr = COMREGS_IRQ1_MASK_CLR(u.reg);
    }
}


// =============================================================================
// hal_WdComregsGetMask
// -----------------------------------------------------------------------------
/// Get the mask of a slot
///
/// @param slot The slot whose masks is wanted
/// @return The value of the mask set for the slot given
/// as a parameter.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_WdComregsGetMask(HAL_WDCOMREGS_XCPU_IRQ_SLOT_T slot)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    if(slot == HAL_WDCOMREGS_XCPU_IRQ_SLOT_0)
    {
        u.reg = GET_BITFIELD(hwp_sysComregsWd->Mask_Set,COMREGS_IRQ0_MASK_SET);
        return u.bitfield;
    }
    else
    {
        HAL_ASSERT(slot == HAL_WDCOMREGS_XCPU_IRQ_SLOT_1, "Improper mbx slot");
        u.reg = GET_BITFIELD(hwp_sysComregsWd->Mask_Set,COMREGS_IRQ1_MASK_SET);
        return u.bitfield;
    }
}


// PRIVATE FUNCTION !!!! (internal to hal)
// Discreminate around the slot and call the appropriate
// previously defined handler. This is the function called
// by the irq hw module.
PROTECTED VOID hal_WdComregsIrqHandler (UINT8 interruptId)
{
    UINT32 tmpRead;
    UINT32 status;

    union
    {
        UINT32 slotStatus;
        HAL_COMREGS_IRQ_STATUS_T returnedStatus;
    } u;

    if (interruptId == SYS_IRQ_COM0_WD)
    {
        status =  GET_BITFIELD(hwp_sysComregsWd->Cause, COMREGS_IRQ0_CAUSE);
        // Clear ITs
        hwp_sysComregsWd->ItReg_Clr = COMREGS_IRQ0_CAUSE(status);
        tmpRead = hwp_sysComregsWd->ItReg_Clr;

        // mask for the first slot
        u.slotStatus = status;

        if(g_halWdComregsRegistry[HAL_WDCOMREGS_XCPU_IRQ_SLOT_0])
        {
            g_halWdComregsRegistry[HAL_WDCOMREGS_XCPU_IRQ_SLOT_0](u.returnedStatus);
        }
    }
    else
    {
        // interruptId == SYS_IRQ_COM1
        status =  GET_BITFIELD(hwp_sysComregsWd->Cause, COMREGS_IRQ1_CAUSE);
        // Clear ITs
        hwp_sysComregsWd->ItReg_Clr = COMREGS_IRQ1_CAUSE(status);
        tmpRead = hwp_sysComregsWd->ItReg_Clr;

        u.slotStatus = status;

        if(g_halWdComregsRegistry[HAL_WDCOMREGS_XCPU_IRQ_SLOT_1])
        {
            g_halWdComregsRegistry[HAL_WDCOMREGS_XCPU_IRQ_SLOT_1](u.returnedStatus);
        }
    }

    //If others, do others
    //...
}


// =============================================================================
// hal_WdComregsWcpuIrqTrigger
// -----------------------------------------------------------------------------
/// Trigger IRQs on the WCPU
///
/// @param slot BCPU IRQ slot on which trigger the IRQ.
/// @param mask IRQ cause sent to the BPCU
/// leave the corresponding bit unchanged.
///
// =============================================================================
PUBLIC VOID hal_WdComregsWcpuIrqTrigger(HAL_WDCOMREGS_WCPU_IRQ_SLOT_T slot,HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_WDCOMREGS_WCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_sysComregsWd->ItReg_Set = COMREGS_IRQ0_SET(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_WDCOMREGS_WCPU_IRQ_SLOT_1, "Improper mbx slot");
        // built the mask as it should be in the hw register
        hwp_sysComregsWd->ItReg_Set = COMREGS_IRQ1_SET(u.reg);

    }
}


// =============================================================================
// hal_WdComregsXcpuIrqTrigger
// -----------------------------------------------------------------------------
/// Trigger IRQs on the XCPU
///
/// @param slot XCPU IRQ slot on which trigger the IRQ.
/// @param mask IRQ cause sent to the BPCU
/// leave the corresponding bit unchanged.
///
// =============================================================================
PUBLIC VOID hal_WdComregsXcpuIrqTrigger(HAL_WDCOMREGS_XCPU_IRQ_SLOT_T slot,HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_WDCOMREGS_XCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_wdComregs->ItReg_Set = COMREGS_IRQ0_SET(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_WDCOMREGS_XCPU_IRQ_SLOT_1, "Improper mbx slot");
        // built the mask as it should be in the hw register
        hwp_wdComregs->ItReg_Set = COMREGS_IRQ1_SET(u.reg);
    }
}


// =============================================================================
// hal_WdComregsWcpuIrqGetCauseStatus
// -----------------------------------------------------------------------------
/// Get cause of IRQ triggerd on the BCPU
/// @param slot BCPU IRQ slot from which to get the cause.
/// @return The interrupt status of the given slot.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_WdComregsWcpuIrqGetCauseStatus(HAL_WDCOMREGS_WCPU_IRQ_SLOT_T slot)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    if(slot == HAL_WDCOMREGS_XCPU_IRQ_SLOT_0)
    {
        u.reg = GET_BITFIELD(hwp_wdComregs->ItReg_Set,COMREGS_IRQ0_SET);
        return u.bitfield;
    }
    else
    {
        HAL_ASSERT(slot == HAL_WDCOMREGS_WCPU_IRQ_SLOT_1, "Improper mbx slot");
        u.reg = GET_BITFIELD(hwp_wdComregs->ItReg_Set,COMREGS_IRQ1_SET);
        return u.bitfield;
    }
}


// =============================================================================
// hal_WdComregsXcpuIrqGetCauseStatus
// -----------------------------------------------------------------------------
/// Get cause of IRQ triggerd on the XCPU
/// @param slot BCPU IRQ slot from which to get the cause.
/// @return The interrupt status of the given slot.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_WdComregsXcpuIrqGetCauseStatus(HAL_WDCOMREGS_XCPU_IRQ_SLOT_T slot)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    if(slot == HAL_WDCOMREGS_XCPU_IRQ_SLOT_0)
    {
        u.reg = GET_BITFIELD(hwp_sysComregsWd->ItReg_Set,COMREGS_IRQ0_SET);
        return u.bitfield;
    }
    else
    {
        HAL_ASSERT(slot == HAL_WDCOMREGS_XCPU_IRQ_SLOT_1, "Improper mbx slot");
        u.reg = GET_BITFIELD(hwp_sysComregsWd->ItReg_Set,COMREGS_IRQ1_SET);
        return u.bitfield;
    }
}


// =============================================================================
// hal_WdComregsWcpuIrqClearCause
// -----------------------------------------------------------------------------
/// Clear
/// @param slot WCPU IRQ slot on which clear the IRQ cause.
/// @param mask IRQ cause to clear, leaving the others unchanged.
// =============================================================================
PUBLIC VOID hal_WdComregsWcpuIrqClearCause(HAL_WDCOMREGS_WCPU_IRQ_SLOT_T slot, HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_WDCOMREGS_XCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_wdComregs->ItReg_Clr = COMREGS_IRQ0_CLR(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_WDCOMREGS_WCPU_IRQ_SLOT_1, "Improper mbx slot");
        // built the mask as it should be in the hw register
        hwp_wdComregs->ItReg_Clr = COMREGS_IRQ1_CLR(u.reg);
    }
}


// =============================================================================
// hal_WdComregsXcpuIrqClearCause
// -----------------------------------------------------------------------------
/// Clear IRQs on the XCPU
///
/// @param slot XCPU IRQ slot on which clear the IRQ cause.
/// @param mask IRQ cause to clear, leaving the others unchanged.
///
// =============================================================================
PUBLIC VOID hal_WdComregsXcpuIrqClearCause(HAL_WDCOMREGS_XCPU_IRQ_SLOT_T slot, HAL_COMREGS_IRQ_MASK_T mask)
{
    union
    {
        UINT32 reg;
        HAL_COMREGS_IRQ_MASK_T bitfield;
    } u;

    u.bitfield = mask;

    if(slot == HAL_WDCOMREGS_XCPU_IRQ_SLOT_0)
    {
        // built the mask as it should be in the hw register
        hwp_sysComregsWd->ItReg_Clr = COMREGS_IRQ0_CLR(u.reg);
    }
    else
    {
        HAL_ASSERT(slot == HAL_WDCOMREGS_XCPU_IRQ_SLOT_1, "Improper mbx slot");
        // built the mask as it should be in the hw register
        hwp_sysComregsWd->ItReg_Clr = COMREGS_IRQ1_CLR(u.reg);
    }
}
#endif // CHIP_HAS_WCPU

