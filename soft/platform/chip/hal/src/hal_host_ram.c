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

#include "boot_host.h"
#include "boot.h"

#include "hal_usb.h"
#include "hal_host.h"
#include "hal_clk.h"
#include "halp_debug.h"
#include "boot_sector_driver.h"
#include "halp_host.h"
#include "halp_sys.h"

#include "sxr_ops.h"
#include "sxs_io.h"


// =============================================================================
//  MACROS
// =============================================================================

#define COMMAND_IN_IDLE(c) ((0xe1<<24)|(c&0xffffff))
#define COMMAND_IN_IRQ(c) ((0xe2<<24)|(c&0xffffff))

// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================


// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// hal_HstRamClientIdle
// -----------------------------------------------------------------------------
/// Read the H2P exchange register to retrieve host commands. If any supported
/// one found (type #BOOT_HST_CMD_T), it is executed during the idle task.
// =============================================================================
PROTECTED VOID hal_HstRamClientIdle(VOID)
{
    static BOOT_HST_CMD_T prevCommand = BOOT_HST_NO_COMMAND;
    BOOT_HST_CMD_T hostCommand;

    hostCommand = GET_BITFIELD(hwp_debugHost->h2p_status, DEBUG_HOST_H2P_STATUS);

    switch (hostCommand)
    {
        case BOOT_HST_NO_COMMAND:
            prevCommand = hostCommand;
            break;

        case BOOT_HST_MONITOR_X_CMD:
            prevCommand = hostCommand;
            // Execute a command place in the execution structure.
            // H2P_Status is cleared in the basic handler.
            boot_HstCmdBasicHandler();
            break;

// boot_RestartInMonitor being a function
// from the boot sector, that is not
// available when ramrunning the code...
#if (CHIP_HAS_USB == 1)
        case BOOT_HST_MONITOR_ENTER:
            prevCommand = hostCommand;
            hal_BootEnterMonitor();
            break;
#endif

        // Add you own commands here:

        // ...

        // -------------------------

        default:
            // Do NOT reset h2p register here since it might be an IRQ command.
            // Complain the case for once only.
            if (prevCommand != hostCommand)
            {
                prevCommand = hostCommand;
                // Unsupported command
                hal_HstSendEvent(BOOT_HST_UNSUPPORTED_CMD);
                // Show current command
                hal_HstSendEvent(COMMAND_IN_IDLE(hostCommand));
            }
            break;
    }
}


// =============================================================================
// hal_HstRamClientIrq
// -----------------------------------------------------------------------------
/// Executed on IRQ from the Host.  WARNING: This interruption is handled as an
/// exception with all its limitations. (No task awareness: Cannot send SX
/// messages, switch tasks...)
///
/// It reads the H2P exchange register to retrieve host commands. If any
/// supported one is found (type #BOOT_HST_CMD_T), it is executed immediately.
///
/// Takes care of Critical Section breaking and coherence etc.
// =============================================================================
PROTECTED VOID hal_HstRamClientIrq(VOID)
{
    BOOT_HST_CMD_T hostCommand;

    hostCommand = GET_BITFIELD(hwp_debugHost->h2p_status, DEBUG_HOST_H2P_STATUS);

    // Clear the Host Irq cause.
    hwp_debugHost->irq = DEBUG_HOST_XCPU_IRQ;

    switch (hostCommand)
    {
        case BOOT_HST_NO_COMMAND:
            break;


        case BOOT_HST_MONITOR_X_IRQ_CMD:
            // Execute a command place in the execution structure
            // H2P_Status is cleared in the basic handler
            boot_HstCmdBasicHandler();
            break;

        case BOOT_HST_CLK_REL:
            hal_HstSleep();
            break;

        case BOOT_HST_CLK_REQ:
            hal_HstWakeup();
            break;

        // Add you own commands here:

        // ...

        // -------------------------

        default:
            // unsupported command
            hal_HstSendEvent(BOOT_HST_UNSUPPORTED_CMD);
            // Show current command
            hal_HstSendEvent(COMMAND_IN_IRQ(hostCommand));
            break;
    }
}

static HAL_CLK_T *gHostUartClk = NULL;
static BOOL gHostUartEnabled = FALSE;

PROTECTED VOID hal_HstSleep(VOID)
{
    // Release the resource.
    if (gHostUartEnabled) {
        hal_ClkDisable(gHostUartClk);
        gHostUartEnabled = FALSE;
    }
}

PROTECTED VOID hal_HstWakeup(VOID)
{
    // Set the resource as active.
    if (!gHostUartEnabled) {
        hal_ClkEnable(gHostUartClk);
        gHostUartEnabled = TRUE;
    }
}

HAL_MODULE_CLK_INIT(HOST_UART)
{
    gHostUartClk = hal_ClkGet(FOURCC_DBG_HOST);
    HAL_ASSERT((gHostUartClk != NULL), "Clk HOST UART not found!");
}
