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

#include "boot_usb.h"
#include "bootp_debug.h"


// =============================================================================
// boot_IrqDispatch
// -----------------------------------------------------------------------------
/// BOOT romed IRQ dispatch.
/// Exceptions are handled in the rom (boot_rom.S) assemlby code.
/// Here, we check the cause is really USB before calling the USB handler,
/// otherwise sending an error if the interrupt cause is not supported.
// =============================================================================
PROTECTED VOID boot_IrqDispatch(VOID)
{
    if (hwp_sysIrq->Cause & SYS_IRQ_SYS_IRQ_USBC)
    {
        // Call the USB handler if, and only if,
        // the interruption is really for the USB.
        boot_UsbIrqHandler(SYS_IRQ_USBC);
    }
    else
    {
        // We received an unsupported (here) interruption
        BOOT_PROFILE_PULSE(BOOT_ROMED_XCPU_ERROR);

        // Stay here.
        while (1);
    }
}

