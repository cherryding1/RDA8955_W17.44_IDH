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
#include "debug_host.h"
#include "debug_uart.h"
#include "sys_irq.h"

#include "hal_mem_map.h"
#include "hal_sys.h"
#include "hal_timers.h"

#include "boot_usb_monitor.h"
#include "hal_host.h"



// =============================================================================
// HAL_HST_SEND_EVENT_TIMEOUT
// -----------------------------------------------------------------------------
/// Timeout after which we give up sending an event, in 16384 Hz ticks.
// =============================================================================
#define HAL_HST_SEND_EVENT_DELAY    5



// TODO: re-implement the timeout in hal_HstSendEvent.
// It should not be done in critical section:
// Now: EnterCS, Loop { Test }, ExitCS.
// Should be: Loop { EnterCS, Test, IfOk { SendEvent, ExitLoop=1 } ExitCS [Sleep,] }

// =============================================================================
// hal_HstSendEvent
// -----------------------------------------------------------------------------
/// Send an event through the host port
/// @param ch Event sent through the host port.
// =============================================================================
PUBLIC BOOL HAL_BOOT_FUNC_INTERNAL hal_HstSendEvent(UINT32 ch)
{
#if !(CHIP_HAS_USB == 1) && defined(HAL_EVENT_USB)
#error "You can't use HAL_EVENT_USB without CHIP_HAS_USB"
#endif

#ifdef HAL_EVENT_USB
    boot_HostUsbEvent(ch);
    return 0;

#else // HAL_EVENT_USB
    UINT32 status    = hal_SysEnterCriticalSection();
    UINT32 startTime = hal_TimGetUpTime();
    BOOL   timeOut   = FALSE;

    //  wait for host sema to be free
    //  or time out.
    while ((hwp_debugHost->event & 1) == 0 && !timeOut)
    {
        if ((hal_TimGetUpTime() - startTime) >= HAL_HST_SEND_EVENT_DELAY)
        {
            timeOut = TRUE;
        }
    }

    //  Write the byte in the host event register if possible
    if (!timeOut)
    {
        hwp_debugHost->event = ch;
    }

    hal_SysExitCriticalSection(status);
    return timeOut;
#endif // HAL_EVENT_USB
}



// =============================================================================
// hal_HstWaitLastEventSent
// -----------------------------------------------------------------------------
/// Check if the last event has been sent on the host port.
/// You can't send new event after this function
// =============================================================================
PUBLIC VOID hal_HstWaitLastEventSent(VOID)
{
#if !(CHIP_HAS_USB == 1) && defined(HAL_EVENT_USB)
#error "You can't use HAL_EVENT_USB without CHIP_HAS_USB"
#endif

#ifndef HAL_EVENT_USB
    // There is a pending event in the event fifo (!= UART fifo).
    while((hwp_debugHost->event & DEBUG_HOST_EVENT0_SEMA) == 0);

    // Is there anything left in the UART fifo or in the Tx machine?
    while((hwp_debugUart->status &
            (DEBUG_UART_TX_ACTIVE | DEBUG_UART_TX_FIFO_LEVEL_MASK)) != 0);
#endif // !HAL_EVENT_USB
}


