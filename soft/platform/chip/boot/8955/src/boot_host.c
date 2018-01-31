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
#include "sys_irq.h"
#include "timer.h"

#include "boot_host.h"
#include "boot.h"
#include "bootp_host.h"
#include "boot_cache.h"

#ifndef BOOT_HST_SEND_EVENT_DELAY
#define BOOT_HST_SEND_EVENT_DELAY    5
#endif /* BOOT_HST_SEND_EVENT_DELAY */

/// Structures used to exchange data with the host
PROTECTED volatile BOOT_HST_MONITOR_X_CTX_T boot_HstMonitorXCtx
__attribute__((section (".sram_hst_monitor_ctx")));

PROTECTED volatile BOOT_HST_MONITOR_EXTENDED_HANDLER_T boot_HstMonitorExtendedHandler
__attribute__((section (".sram_hst_monitor_ctx")));

PROTECTED VOID mon_Event(UINT32 evt)
{
    UINT32 status    = hwp_sysIrq->SC;
    UINT32 startTime = hwp_timer->HWTimer_CurVal;
    BOOL   timeOut   = FALSE;

    //  wait for host sema to be free
    //  or time out.
    while(hwp_debugHost->event == 0 && !timeOut)
    {
        if ((hwp_timer->HWTimer_CurVal - startTime) >= BOOT_HST_SEND_EVENT_DELAY)
        {
            timeOut = TRUE;
        }
    }

    //  Write the byte in the host event register if possible
    if (!timeOut)
    {
        hwp_debugHost->event = evt;
    }
    hwp_sysIrq->SC       = status;
}


// =============================================================================
// boot_HstMonitor
// -----------------------------------------------------------------------------
/// Main host monitor function. Read the command passed to the platform through
/// the Host port and call the host command handler if appropriate.
/// It read the H2P register to execute commands
/// until the Exit command is received (BOOT_HST_MONITOR_END_CMD).
// =============================================================================
PROTECTED BOOT_MONITOR_OP_STATUS_T boot_HstMonitor(VOID)
{

    BOOT_HST_CMD_T hostCommand = 0;

    // Clear the "enter the monitor" host command

    // if a host command present
    if((hostCommand = GET_BITFIELD(hwp_debugHost->h2p_status,
                                   DEBUG_HOST_H2P_STATUS)) != 0)
    {
        // We received a command: we are not waiting anymore but actually acting
        // as a monitor.
        hwp_debugHost->p2h_status   = BOOT_HST_STATUS_NONE;

        switch (hostCommand)
        {
            case BOOT_HST_MONITOR_START_CMD:
                // That command used to be used to enter into the monitor.
                // We now use a boot mode for that, so this command is
                // just used to send the BOOT_HST_MONITOR_START event
                // to acknowledge to the host (eg Remote PC's coolwatcher)
                // that we actually are in the monitor.
                mon_Event(BOOT_HST_MONITOR_START);
                hwp_debugHost->h2p_status = DEBUG_HOST_H2P_STATUS_RST;
                break;

            case BOOT_HST_MONITOR_X_CMD:
                // Execute a command placed in the execution structure.
                // H2P_Status is cleared in the basic handler
                boot_HstCmdBasicHandler();
                break;

            case BOOT_HST_MONITOR_END_CMD:
                // We are required to leave the monitor.
                mon_Event(BOOT_HST_MONITOR_END);
                hwp_debugHost->h2p_status = DEBUG_HOST_H2P_STATUS_RST;
                return BOOT_MONITOR_OP_STATUS_EXIT;

            default:
                // unsupported command
                mon_Event(BOOT_HST_UNSUPPORTED_CMD);
                hwp_debugHost->h2p_status = DEBUG_HOST_H2P_STATUS_RST;
                break;
        }

        // We received a command, possibly unknown, but different from
        // BOOT_HST_MONITOR_END_CMD;
        return BOOT_MONITOR_OP_STATUS_CONTINUE;
    }
    else
    {
        // No command received.
        return BOOT_MONITOR_OP_STATUS_NONE;
    }
}


/// Host command handler
/// It is called by the host monitor and to handle
/// the debug IRQ (triggered from the host ... )
PUBLIC VOID boot_HstCmdBasicHandler(VOID)
{
    UINT32 cmdType = 0;
    UINT32 status    = hwp_sysIrq->SC;
    asm volatile ("");
    // Flush all caches
    boot_FlushDCache(TRUE);
    hwp_sysIrq->SC       = status;
    asm volatile ("");

    mon_Event(BOOT_HST_START_CMD_TREATMENT);

    // Clear calling cause
    hwp_debugHost->h2p_status = DEBUG_HOST_H2P_STATUS_RST;
    hwp_debugHost->irq = DEBUG_HOST_XCPU_IRQ;

    cmdType = boot_HstMonitorXCtx.cmdType;

    switch (cmdType)
    {
        case BOOT_HST_MONITOR_X_CMD:
            // Execute at the PC in the execution context with the provided SP
            boot_HstMonitorX();

            mon_Event(BOOT_HST_END_CMD_TREATMENT);
            break;

        default:
            if (0 != boot_HstMonitorExtendedHandler.ExtendedCmdHandler)
            {
                ((VOID(*)(VOID*))
                 boot_HstMonitorExtendedHandler.ExtendedCmdHandler)
                ((VOID*)&boot_HstMonitorXCtx);
            }
            mon_Event(BOOT_HST_END_CMD_TREATMENT);
            break;


    }
}

