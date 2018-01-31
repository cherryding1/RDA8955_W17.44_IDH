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

#include "chip_id.h"

#include "halp_irq.h"
#include "halp_os_stub.h"
#include "halp_debug.h"
#include "halp_profile.h"
#include "halp_irq_prio.h"

#include "hal_debug.h"
#include "hal_mem_map.h"


// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_IrqDispatch
// -----------------------------------------------------------------------------
/// IRQ dispatcher. Calls the interrupt handler according to their priorities
/// defined in the relevant macros and array.
// =============================================================================
PROTECTED VOID HAL_FUNC_INTERNAL hal_IrqDispatch(VOID)
{
    UINT32 cause = hwp_sysIrq->Cause;
    UINT32 bit = 1;
    UINT8 i;
    UINT32 unused __attribute__ ((unused));
    UINT32 mask;

    sxr_EnterScSchedule();

    // gestion Mask
    // save current Mask
    mask = hwp_sysIrq->Mask_Set;
    // Mask all IT
    hwp_sysIrq->Mask_Clear = SYS_IRQ_MASK_SET_MASK;
    // Re-enable irq
    hwp_sysIrq->SC = 1;

    for(i=0; i<SYS_IRQ_QTY; i++)
    {
        if (cause & bit)
        {
            HAL_PROFILE_IRQ_ENTER(i);
            // clear irq bit of reenabled higher prio irqs as those will be treated
            // by an other interrupt call
            cause &= ~g_halIrqPriorityMask[i];
            // Setup priority mask for this irq
            hwp_sysIrq->Mask_Set = g_halIrqPriorityMask[i];

            if(g_halHwModuleIrqHandler[i])
            {
                (g_halHwModuleIrqHandler[i])(i);
            }

            // Mask all IT
            hwp_sysIrq->Mask_Clear = SYS_IRQ_MASK_SET_MASK;

            HAL_PROFILE_IRQ_EXIT(i);
        }
        bit = bit << 1;
    }

    // Disable irq
    unused = hwp_sysIrq->SC;
    // Restore previous mask
    hwp_sysIrq->Mask_Set = mask;

    sxr_ExitScSchedule();
}


// =============================================================================
// hal_XCpuSleep
// -----------------------------------------------------------------------------
/// Put the XCpu in sleep mode until next irq.
/// @todo see how mike used that one and named it...
// =============================================================================
PROTECTED VOID hal_XCpuSleep()
{
    hwp_sysIrq->WakeUp_Mask = hwp_sysIrq->Mask_Set;
    //hwp_sysIrq->Cpu_Sleep = (SYS_IRQ_SLEEP);
    {
        UINT32 flush_wr_buff __attribute__((unused)) = hwp_sysIrq->Cpu_Sleep;
    }
}

