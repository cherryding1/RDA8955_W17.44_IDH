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


#ifndef _BOOTP_UART_MONITOR_H_
#define _BOOTP_UART_MONITOR_H_

#include "cs_types.h"
#include "hal_uart.h"
#include "boot.h"
// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================
// =============================================================================
// g_bootUartMonitorBaudRate;
// -----------------------------------------------------------------------------
/// Holds the detected baud rate if any, or 0.
// =============================================================================
EXPORT PROTECTED HAL_UART_BAUD_RATE_T g_bootUartMonitorBaudRate;



// =============================================================================
// g_bootUartMonitorBrDetected
// -----------------------------------------------------------------------------
/// Is true if, and only if, a communication baudrate has been detected.
// =============================================================================
EXPORT PROTECTED BOOL g_bootUartMonitorBrDetected;


// =============================================================================
// FUNCTIONS
// =============================================================================

PROTECTED VOID boot_UartMonitorOpen(VOID);

PROTECTED VOID boot_UartMonitorClose(VOID);

PROTECTED BOOT_MONITOR_OP_STATUS_T boot_UartMonitor(VOID);

#endif // _BOOTP_UART_MONITOR_H_
