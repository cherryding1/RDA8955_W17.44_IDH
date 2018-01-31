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

// #define LOCAL_LOG_LEVEL LOG_LEVEL_VERBOSE

#include "at_device_uart.h"
#include "at_dispatch.h"
#include "at_lp_ctrl.h"
#include "at_utils.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "hal_sys.h"
#include "hal_timers.h"
#include "cos.h"
#include "event.h"
#include "drv.h"
#include <stdlib.h>

#define XCPU_CACHE_LINE_SIZE (16)
#define XCPU_CACHE_LINE_MASK (XCPU_CACHE_LINE_SIZE - 1)

#define UART_RX_DMA_SIZE (256)
#define UART_RX_BUF_SIZE (4 * 1024)
#define UART_TX_BUF_SIZE (4 * 1024)
#define UART_RX_POLL_PERIOD (15) // ms

#define UART_WAIT_TX_DONE_TIMEOUT (500)

#define UART_POOL_SIZE (UART_RX_DMA_SIZE + UART_RX_BUF_SIZE + UART_TX_BUF_SIZE + XCPU_CACHE_LINE_SIZE)

typedef struct
{
    AT_DEVICE_T ops; // API
    DRV_UART_T *drv;
    AT_DEVICE_UART_CONFIG_T config; // UART configuration

    bool sleep; // whether in sleep
} AT_DEVICE_UART_T;

static void at_UartDestroy(AT_DEVICE_T *th);
static bool at_UartOpen(AT_DEVICE_T *th);
static void at_UartClose(AT_DEVICE_T *th);
static int at_UartWrite(AT_DEVICE_T *th, const uint8_t *data, unsigned length);
static int at_UartRead(AT_DEVICE_T *th, uint8_t *data, unsigned size);
static void at_UartSleep(AT_DEVICE_T *th, bool wakeupEnabled);
static void at_UartWakeup(AT_DEVICE_T *th);
static bool at_UartIsBaudSupported(AT_DEVICE_T *th, int baud);
static void at_UartCallback(DRV_UART_T *drv, void *param, DRV_UART_EVENT_T evt);
static bool at_UartSetFlowCtrl(AT_DEVICE_T *th, uint8_t rxfc, uint8_t txfc);
static void at_UartSetFormat(AT_DEVICE_T *th, int baud, AT_DEVICE_FORMAT_T format, AT_DEVICE_PARITY_T parity);

static inline enum DRV_UART_PARITY at_UartDrvParity(AT_DEVICE_PARITY_T parity)
{
    switch (parity)
    {
    case AT_DEVICE_PARITY_ODD:
        return DRV_UART_ODD_PARITY;
    case AT_DEVICE_PARITY_EVEN:
        return DRV_UART_EVEN_PARITY;
    case AT_DEVICE_PARITY_MARK:
        return DRV_UART_MARK_PARITY;
    case AT_DEVICE_PARITY_SPACE:
        return DRV_UART_SPACE_PARITY;
    default:
        return DRV_UART_NO_PARITY;
    }
}

static void at_UartDrvFormat(DRV_UART_CFG_T *cfg, AT_DEVICE_FORMAT_T format, AT_DEVICE_PARITY_T parity)
{
    cfg->dataBits = (format == AT_DEVICE_FORMAT_7N2 ||
                     format == AT_DEVICE_FORMAT_711 ||
                     format == AT_DEVICE_FORMAT_7N1)
                        ? DRV_UART_DATA_BITS_7
                        : DRV_UART_DATA_BITS_8;
    cfg->stopBits = (format == AT_DEVICE_FORMAT_8N2 ||
                     format == AT_DEVICE_FORMAT_7N2)
                        ? DRV_UART_STOP_BITS_2
                        : DRV_UART_STOP_BITS_1;
    cfg->parity = (format == AT_DEVICE_FORMAT_811 ||
                   format == AT_DEVICE_FORMAT_711)
                      ? at_UartDrvParity(parity)
                      : DRV_UART_NO_PARITY;
}

// =============================================================================
// at_UartCreate
// =============================================================================
AT_DEVICE_T *at_UartCreate(AT_DEVICE_UART_CONFIG_T *cfg, uint8_t devno)
{
    // assert(cfg != NULL)
    unsigned drvsize = DRV_UartStructSize();
    unsigned memsize = sizeof(AT_DEVICE_UART_T) + UART_POOL_SIZE + drvsize;
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)at_malloc(memsize);
    memset(uart, 0, sizeof(*uart));

    uart->drv = (DRV_UART_T *)((uint32_t)uart + sizeof(*uart));
    uint32_t rxDmaBuf = ((uint32_t)uart->drv + drvsize + XCPU_CACHE_LINE_MASK) & ~XCPU_CACHE_LINE_MASK;
    uint32_t rxBuf = rxDmaBuf + UART_RX_DMA_SIZE;
    uint32_t txBuf = rxBuf + UART_RX_BUF_SIZE;

    uart->config = *cfg;
    ATUARTLOG(I, "AT UART%d create: baud/%d", uart->config.id, cfg->baud);

    uart->ops.devno = devno;
    uart->ops.destroy = at_UartDestroy;
    uart->ops.open = at_UartOpen;
    uart->ops.close = at_UartClose;
    uart->ops.write = at_UartWrite;
    uart->ops.read = at_UartRead;
    uart->ops.sleep = at_UartSleep;
    uart->ops.wakeup = at_UartWakeup;
    uart->ops.isBaudSupported = at_UartIsBaudSupported;
    uart->ops.setFlowCtrl = at_UartSetFlowCtrl;
    uart->ops.setFormat = at_UartSetFormat;

    DRV_UART_CFG_T drvcfg;
    drvcfg.baud = cfg->baud;
    at_UartDrvFormat(&drvcfg, cfg->format, cfg->parity);
    drvcfg.ctsEnabled = cfg->ctsEnabled;
    drvcfg.rtsEnabled = cfg->rtsEnabled;
    drvcfg.autoFlowCtrlLevel = drvcfg.rtsEnabled ? DRV_UART_DEFAULT_AUTO_FLOW_CTRL_LEVEL : DRV_UART_DISABLE_AUTO_FLOW_CTRL;
    drvcfg.autoBaudLC = cfg->autoBaudLC;
    drvcfg.forceDivider = 0;
    drvcfg.rxDmaBuf = (uint8_t *)HAL_SYS_GET_UNCACHED_ADDR(rxDmaBuf);
    drvcfg.rxBuf = (uint8_t *)rxBuf;
    drvcfg.txBuf = (uint8_t *)HAL_SYS_GET_UNCACHED_ADDR(txBuf);
    drvcfg.rxDmaSize = UART_RX_DMA_SIZE;
    drvcfg.rxBufSize = UART_RX_BUF_SIZE;
    drvcfg.txBufSize = UART_TX_BUF_SIZE;
    drvcfg.rxPollPeriod = UART_RX_POLL_PERIOD;
    drvcfg.evtMask = DRV_UART_RX_ARRIVED | DRV_UART_WAKE_UP;
    drvcfg.callback = at_UartCallback;
    drvcfg.callbackParam = uart;

    DRV_UartInit(uart->drv, uart->config.id, &drvcfg);
    return (AT_DEVICE_T *)uart;
}

// =============================================================================
// at_UartDestroy
// =============================================================================
static void at_UartDestroy(AT_DEVICE_T *th)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;

    if (uart == NULL)
        return;

    ATUARTLOG(I, "AT UART%d destroy", uart->config.id);
    DRV_UartClose(uart->drv, true);
    at_free(uart);
}

// =============================================================================
// Sleep
// =============================================================================
static void at_UartSleep(AT_DEVICE_T *th, bool wakeupEnabled)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;
    if (uart->sleep)
        return;

    ATUARTLOG(I, "AT UART%d sleep", uart->config.id);
    DRV_UartSleep(uart->drv, false, wakeupEnabled);
    uart->sleep = true;
}

// =============================================================================
// Wakeup
// =============================================================================
static void at_UartWakeup(AT_DEVICE_T *th)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;
    if (!uart->sleep)
        return;

    ATUARTLOG(I, "AT UART%d wakeup", uart->config.id);
    DRV_UartWakeup(uart->drv);
    uart->sleep = false;
}

// =============================================================================
// Write
// =============================================================================
static int at_UartWrite(AT_DEVICE_T *th, const uint8_t *data, unsigned length)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;

    if (data == NULL || length == 0)
        return 0;

    ATUARTLOG(I, "AT UART%d write: len=%d", uart->config.id, length);
    if (uart->sleep && !at_LpCtrlOutputRequest())
    {
        ATUARTLOG(W, "AT UART%d skip output due to wakeup fail", uart->config.id);
        return 0;
    }

    int trans = DRV_UartWrite(uart->drv, data, length, UART_WAIT_TX_DONE_TIMEOUT);
    if (trans < 0)
        ATUARTLOG(E, "AT UART%d output error %d", uart->config.id, trans);
    else if (trans < length)
        ATUARTLOG(E, "AT UART%d output overflow, drop %d bytes", uart->config.id, length - trans);
    return trans;
}

static void at_UartCallback(DRV_UART_T *drv, void *param, DRV_UART_EVENT_T evt)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)param;

    if (evt & DRV_UART_RX_ARRIVED)
        at_DeviceScheduleRead((AT_DEVICE_T *)uart);

    if (evt & (DRV_UART_RX_ARRIVED | DRV_UART_WAKE_UP))
        at_TaskCallbackNotif((COS_CALLBACK_FUNC_T)at_LpCtrlInputReceived, NULL);
}

// =============================================================================
// Read
// =============================================================================
static int at_UartRead(AT_DEVICE_T *th, uint8_t *data, unsigned size)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;
    if (uart->sleep)
        return 0;
    if (data == NULL || size == 0)
        return 0;

    int trans = DRV_UartRead(uart->drv, data, size);
    if (trans < 0)
        ATUARTLOG(E, "AT UART%d read error %d", uart->config.id, trans);
    else
        ATUARTLOG(E, "AT UART%d read %d, got %d", uart->config.id, size, trans);
    return trans;
}

// =============================================================================
// Open
// =============================================================================
static bool at_UartOpen(AT_DEVICE_T *th)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;

    ATUARTLOG(I, "AT UART%d open", uart->config.id);
    DRV_UartOpen(uart->drv);
    uart->sleep = false;
    return true;
}

// =============================================================================
// Close
// =============================================================================
static void at_UartClose(AT_DEVICE_T *th)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;

    ATUARTLOG(I, "AT UART%d close", uart->config.id);
    DRV_UartClose(uart->drv, false);
    uart->sleep = true;
}

// =============================================================================
// Check whether baud rate is supported. Ideally, it shoule be integrated with
// AT+IPR
// =============================================================================
static bool at_UartIsBaudSupported(AT_DEVICE_T *th, int baud)
{
    static const int list[] = {0, 1200, 2400, 4800, 9600, 14400, 19200,
                               28800, 33600, 38400, 57600, 115200,
                               230400, 460800, 921600};
    for (int n = 0; n < ARRAY_SIZE(list); n++)
        if (baud == list[n])
            return true;
    return false;
}

// =============================================================================
// Set flow control
// =============================================================================
static bool at_UartSetFlowCtrl(AT_DEVICE_T *th, uint8_t rxfc, uint8_t txfc)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;
    ATUARTLOG(I, "AT UART%d set flow ctrl %d/%d", uart->config.id, rxfc, txfc);

    DRV_UartWaitTxFinish(uart->drv, UART_WAIT_TX_DONE_TIMEOUT);
    DRV_UartClose(uart->drv, true);

    DRV_UART_CFG_T drvcfg = DRV_UartGetCfg(uart->drv);
    uart->config.rtsEnabled = drvcfg.rtsEnabled = (rxfc == AT_DEVICE_RXFC_HW);
    uart->config.ctsEnabled = drvcfg.ctsEnabled = (txfc == AT_DEVICE_TXFC_HW);
    drvcfg.autoFlowCtrlLevel = drvcfg.rtsEnabled ? DRV_UART_DEFAULT_AUTO_FLOW_CTRL_LEVEL : DRV_UART_DISABLE_AUTO_FLOW_CTRL;
    DRV_UartInit(uart->drv, uart->config.id, &drvcfg);
    DRV_UartOpen(uart->drv);
    return true;
}

// =============================================================================
// Set baud, format and parity
// =============================================================================
static void at_UartSetFormat(AT_DEVICE_T *th, int baud, AT_DEVICE_FORMAT_T format, AT_DEVICE_PARITY_T parity)
{
    AT_DEVICE_UART_T *uart = (AT_DEVICE_UART_T *)th;
    ATUARTLOG(I, "AT UART%d change format baud=%d, format=%d, parity=%d",
              uart->config.id, baud, format, parity);

    DRV_UartWaitTxFinish(uart->drv, UART_WAIT_TX_DONE_TIMEOUT);
    DRV_UartClose(uart->drv, true);

    uart->config.baud = baud;
    uart->config.format = format;
    uart->config.parity = parity;

    DRV_UART_CFG_T drvcfg = DRV_UartGetCfg(uart->drv);
    drvcfg.baud = uart->config.baud;
    at_UartDrvFormat(&drvcfg, uart->config.format, uart->config.parity);
    DRV_UartInit(uart->drv, uart->config.id, &drvcfg);
    DRV_UartOpen(uart->drv);
}
