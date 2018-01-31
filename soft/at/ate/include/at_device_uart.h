/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _AT_DEVICE_UART_H_
#define _AT_DEVICE_UART_H_

#include "at_device.h"

struct AT_DEVICE_UART;

typedef struct
{
    int id; // 1/UART1, 2/UART2
    int baud;
    AT_DEVICE_FORMAT_T format;
    AT_DEVICE_PARITY_T parity;
    bool ctsEnabled;
    bool rtsEnabled; // whether to output RTS
    bool autoBaudLC; // TRUE/at, FALSE/AT
} AT_DEVICE_UART_CONFIG_T;

// =============================================================================
// at_UartCreate
// -----------------------------------------------------------------------------
/// Create AT uart device with specified configuration.
// =============================================================================
AT_DEVICE_T *at_UartCreate(AT_DEVICE_UART_CONFIG_T *cfg, uint8_t devno);

#endif