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

#include "bl_log.h"
#include "bl_platform.h"
#include "cs_types.h"
#include "global_macros.h"
#include "debug_uart.h"
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#define BL_LOG_LINE_MAX 256
#define SYNC_BYTE 0xad
#define FLOWID 0x80

static bool gBlLogInited = false;

static void bl_log_output(const uint8_t *data, int len)
{
    while (len > 0)
    {
        uint32_t avail = DEBUG_UART_TX_FIFO_SIZE -
                         GET_BITFIELD(hwp_debugUart->status, DEBUG_UART_TX_FIFO_LEVEL);
        int trans = (len > avail) ? avail : len;
        for (int n = 0; n < trans; n++)
            hwp_debugUart->rxtx_buffer = *data++;
        len -= trans;
    }
}

static void bl_log_string(const char *s, int len)
{
    uint8_t header[6];
    header[0] = SYNC_BYTE;
    header[1] = ((len + 2) >> 8) & 0xff;
    header[2] = (len + 2) & 0xff;
    header[3] = FLOWID;
    header[4] = 0x00;
    header[5] = 0x00;

    bl_log_output(header, 6);
    bl_log_output((const uint8_t *)s, len);
}

void bl_log(const char *fmt, ...)
{
    if (gBlLogInited)
    {
        char buf[BL_LOG_LINE_MAX];
        va_list ap;

        va_start(ap, fmt);
        int len = vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
        va_end(ap);

        bl_log_string(buf, len + 1);
    }
}

void bl_log_init(void)
{
    hwp_debugUart->irq_mask = 0;
    hwp_debugUart->ctrl = DEBUG_UART_ENABLE |
                          DEBUG_UART_DATA_BITS_8_BITS |
                          DEBUG_UART_TX_STOP_BITS_1_BIT |
                          DEBUG_UART_PARITY_ENABLE_NO |
                          DEBUG_UART_SWRX_FLOW_CTRL(1) |
                          DEBUG_UART_SWTX_FLOW_CTRL(1) |
                          DEBUG_UART_BACKSLASH_EN;

    gBlLogInited = true;
}

void bl_log_close(void)
{
    if (!gBlLogInited)
        return;

    hwp_debugUart->ctrl = DEBUG_UART_DATA_BITS_8_BITS |
                          DEBUG_UART_TX_STOP_BITS_1_BIT |
                          DEBUG_UART_PARITY_ENABLE_NO |
                          DEBUG_UART_SWRX_FLOW_CTRL(1) |
                          DEBUG_UART_SWTX_FLOW_CTRL(1) |
                          DEBUG_UART_BACKSLASH_EN;
}
