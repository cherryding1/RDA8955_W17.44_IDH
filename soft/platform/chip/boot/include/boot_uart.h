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



#ifndef _BOOT_UART_H_
#define _BOOT_UART_H_

/// @defgroup boot_uart BOOT Uart Port Driver/Client
/// That group describes briefly the exchange interface
/// to the romed boot uart monitor.
/// @{
///

#include "cs_types.h"
#include "hal_uart.h"

// =============================================================================
// boot_UartMonitorGetBaudRate
// -----------------------------------------------------------------------------
/// Used to recover the value of the detected monitor uart
/// @param baudRate pointer where the detected value, if any, will be
/// written.
/// @return \c TRUE If a connected Uart was detected and the baudrate
/// discover, \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL boot_UartMonitorGetBaudRate(HAL_UART_BAUD_RATE_T* rate);


// =============================================================================
// boot_HstUartDump
// -----------------------------------------------------------------------------
/// Send a buffer of data in a dump packet through the host.
/// @param data Pointer to the data to send.
/// @param length Lenght of the buffer to send.
/// @param access Access width in bytes (1,2 or 4).
/// @return #BOOT_UM_ERR_NO or #BOOT_UM_ERR_TX_FAILED.
// =============================================================================
PUBLIC UINT32 boot_HstUartDump(UINT8* data, UINT32 length, UINT32 access);


/// @} // <-- End of the boot_uart group.

#endif // _BOOT_UART_H_




