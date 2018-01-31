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

#ifndef _BL_UART_H_
#define _BL_UART_H_

#include <stdbool.h>
#include <stdint.h>

enum DRV_UART_DATA_BITS
{
    DRV_UART_DATA_BITS_7 = 7,
    DRV_UART_DATA_BITS_8 = 8
};

enum DRV_UART_STOP_BITS
{
    DRV_UART_STOP_BITS_1 = 1,
    DRV_UART_STOP_BITS_2 = 2
};

typedef struct
{
    uint32_t baudRate;
    bool ctsEnabled;
    bool rtsEnabled;

    enum DRV_UART_DATA_BITS dataBits;
    enum DRV_UART_STOP_BITS stopBits;
    uint8_t *rxDmaBuf;
    uint8_t *rxBuf;
    uint32_t rxDmaSize;
    uint32_t rxBufSize;
} BL_UART_CFG_T;

struct BL_UART_DEV;
typedef struct BL_UART_DEV BL_UART_DEV_T;

extern uint32_t bl_uart_struct_size();
extern bool bl_uart_init(BL_UART_DEV_T *uart, uint32_t uartid, const BL_UART_CFG_T *cfg);
extern bool bl_uart_open(BL_UART_DEV_T *uart);
extern void bl_uart_close(BL_UART_DEV_T *uart);
extern int bl_uart_write(BL_UART_DEV_T *uart, const void *data, uint32_t size);
extern int bl_uart_read(BL_UART_DEV_T *uart, void *data, uint32_t size);
extern bool bl_uart_wait_tx_finished(BL_UART_DEV_T *uart, int msec_timeout);
extern void bl_uart_rx_purge(BL_UART_DEV_T *uart);
extern const BL_UART_CFG_T *bl_uart_get_cfg(BL_UART_DEV_T *uart);
extern void bl_uart_update_config(BL_UART_DEV_T *uart, const BL_UART_CFG_T *cfg);
extern uint32_t bl_uart_rxbuf_count(BL_UART_DEV_T *uart);

#endif // _BL_UART_H_
