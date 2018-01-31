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

// #define LOCAL_LOG_LEVEL LOG_LEVEL_VERBOSE

#include "drv.h"
#include "global_macros.h"
#include "reg_alias.h"
#include "uart.h"
#include "sys_ifc.h"
#include "sys_ctrl.h"
#include "mem_bridge.h"
#include "hal_uart.h"
#include "hal_sys_ifc.h"
#include "hal_sys.h"
#include "cos.h"

#if defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)
#include "iomux.h"
#endif

// A (not so) generic UART driver
//
// Through UART hardware support both FIFO mode and DMA mode, this driver will
// only support DMA mode.
//
// RX: RX will use manual IFC channel. RX IFC will be transfered into a smaller
// DMA buffer. Also a circle buffer will be used. At interrupt, data will be
// copied from DMA buffer to circle buffer. Application can extract data from
// circle buffer to application buffer. Though there are multiple data copy,
// performance impact will be small due to UART is slow speed device.
//
// Optional (and recommended) RX polling timer will poll RX IFC. Without the
// timer, it is possible to lose data at IFC done interrupt.
//
// RX callback will be executed in IRQ context. So, it is recommended not to
// handle data in callback. Rather, just send an event.
//
// At sleep, UART is still enabled. RX IFC flush bit will be set to avoid
// write to RAM, due to external may be unaccessible at sleep.
//
// TX: TX will use auto IFC channel. IFC channel count is smaller than possible
// requests. For example, all of the 4 UARTs in 8909 are used, IFC channel will
// be not enough. So, it is needed to check whether TX IFC is got. When TX IFC
// is unavailable, it is needed to start a timer to poll TX IFC.
//
// TX IFC will from circle buffer directly. It will consume more cycles to copy
// data to circle buffer (due to uncachable).

#define DBG_GLOBAL_INST // global variables just for debug

#if defined(CHIP_DIE_8809E2) || defined(CHIP_DIE_8955)
#define HWP_UARTS hwp_uart, hwp_uart2
#define UART_RX_IFC_IDS HAL_IFC_UART1_RX, HAL_IFC_UART2_RX
#define UART_TX_IFC_IDS HAL_IFC_UART1_TX, HAL_IFC_UART2_TX
static DRV_UART_T *gDrvUarts[2];
#elif defined(CHIP_DIE_8909)
#define HWP_UARTS hwp_uart1, hwp_uart2, hwp_uart3, hwp_uart4
#define UART_RX_IFC_IDS HAL_IFC_UART1_RX, HAL_IFC_UART2_RX, HAL_IFC_UART3_RX, HAL_IFC_UART4_RX
#define UART_TX_IFC_IDS HAL_IFC_UART1_TX, HAL_IFC_UART2_TX, HAL_IFC_UART3_TX, HAL_IFC_UART4_TX
static DRV_UART_T *gDrvUarts[4];
#endif

#if defined(CHIP_DIE_8809E2)
#define AUTO_BAUD_SUPPORTED 0
#else
#define AUTO_BAUD_SUPPORTED 1
#endif

static HWP_UART_T *const hwp_uarts[] = {HWP_UARTS};
static const HAL_IFC_REQUEST_ID_T rx_ids[] = {UART_RX_IFC_IDS};
static const HAL_IFC_REQUEST_ID_T tx_ids[] = {UART_TX_IFC_IDS};
#define HW_UART_CNT (sizeof(hwp_uarts) / sizeof(hwp_uarts[0]))

#define TX_IFC_UNAVAIL_POLL_PERIOD 5 // ms
#define UART_AUTO_BAUD_POLL_PERIOD 5 // ms

struct DRV_UART
{
    HAL_UART_ID_T id;
    DRV_UART_STATE_T state;
    HWP_UART_T *hwp;
    DRV_UART_CFG_T cfg;
    DRV_CIRCLE_BUF_T rxcb;
    DRV_CIRCLE_BUF_T txcb;
    uint32_t irqMask;
    uint32_t dividerMode; // not for 8955, 8909
    uint32_t divider;
    uint8_t rxIfcCh;
    uint8_t txIfcCh;
    bool wakeupEnabled;
    unsigned txPendingSize;
    HAL_IFC_REQUEST_ID_T rxIfcId;
    HAL_IFC_REQUEST_ID_T txIfcId;
};

static void DRV_UartRxPoll(void *uart);
static void DRV_UartTxPoll(DRV_UART_T *uart);
static void DRV_UartIrq(HAL_UART_IRQ_STATUS_T status, HAL_UART_ERROR_STATUS_T error, UINT32 uart);
static bool DRV_UartStartRxIfc(DRV_UART_T *uart);

unsigned DRV_UartStructSize(void)
{
    return sizeof(struct DRV_UART);
}

bool DRV_UartInit(DRV_UART_T *uart, int uartid, DRV_UART_CFG_T *cfg)
{
    if (uartid < 1 || uartid > HW_UART_CNT)
        return false;

#ifdef DBG_GLOBAL_INST
    gDrvUarts[uartid - 1] = uart;
#endif
    uart->id = uartid;
    uart->cfg = *cfg;
    uart->state = DRV_UART_ST_CLOSE;
    uart->hwp = hwp_uarts[uartid - 1];
    uart->rxIfcId = rx_ids[uartid - 1];
    uart->txIfcId = tx_ids[uartid - 1];
    DRV_CBufInit(&uart->rxcb, uart->cfg.rxBuf, uart->cfg.rxBufSize);
    DRV_CBufInit(&uart->txcb, uart->cfg.txBuf, uart->cfg.txBufSize);
    uart->rxIfcCh = HAL_UNKNOWN_CHANNEL;
    uart->txIfcCh = HAL_UNKNOWN_CHANNEL;
    uart->txPendingSize = 0;
    return true;
}

DRV_UART_CFG_T DRV_UartGetCfg(DRV_UART_T *uart)
{
    return uart->cfg;
}

DRV_UART_STATE_T DRV_UartState(DRV_UART_T *uart)
{
    return uart->state;
}

static void DRV_UartCalcBaudConfig(DRV_UART_T *uart)
{
    uart->dividerMode = 0;
    if (uart->cfg.baud != 0 && uart->cfg.forceDivider != 0)
    {
        uart->divider = uart->cfg.forceDivider;
        return;
    }

#if defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)
#ifdef CHIP_DIE_8909
#define SYS_CTRL_CFG_UART_NUM SYS_CTRL_UART_DIV_NUM
#define SYS_CTRL_CFG_UART_DENOM SYS_CTRL_UART_DIV_DENOM
#endif
    if (uart->cfg.baud == 0)
    {
        uart->divider = SYS_CTRL_CFG_UART_NUM(1) | SYS_CTRL_CFG_UART_DENOM(1);
    }
    else
    {
        switch (uart->cfg.baud)
        {
        case 1200:
            uart->divider = SYS_CTRL_CFG_UART_NUM(1) | SYS_CTRL_CFG_UART_DENOM(10833);
            break;
        case 2400:
            uart->divider = SYS_CTRL_CFG_UART_NUM(3) | SYS_CTRL_CFG_UART_DENOM(16250);
            break;
        default:
            // It works from 4800 to 921600: 52M/4/8125 = 1600
            uart->divider = SYS_CTRL_CFG_UART_NUM(uart->cfg.baud / 1600) | SYS_CTRL_CFG_UART_DENOM(8125);
            break;
        }
    }
#else
    if (uart->cfg.baud <= 4800)
    {
        uart->divider = SYS_CTRL_UART_DIVIDER((26000000 + (8 * uart->cfg.baud)) / (16 * uart->cfg.baud) - 2);
        uart->dividerMode = UART_DIVISOR_MODE;
    }
    else
    {
        uart->divider = SYS_CTRL_UART_DIVIDER((26000000 + (2 * uart->cfg.baud)) / (4 * uart->cfg.baud) - 2);
        uart->dividerMode = 0;
    }
#endif
}

static void DRV_UartSetSleepClock(DRV_UART_T *uart)
{
#if defined(CHIP_DIE_8955)
    hwp_sysCtrl->Cfg_Clk_Uart[uart->id] = SYS_CTRL_CFG_UART_NUM(1) | SYS_CTRL_CFG_UART_DENOM(1);
#elif defined(CHIP_DIE_8909)
// TODO
#else
    uart->hwp->ctrl &= ~UART_DIVISOR_MODE;
    hwp_sysCtrl->Cfg_Clk_Uart[uart->id] = SYS_CTRL_UART_DIVIDER(0) | SYS_CTRL_UART_SEL_PLL_SLOW;
#endif
}

static void DRV_UartRestoreClock(DRV_UART_T *uart)
{
#if defined(CHIP_DIE_8955)
    hwp_sysCtrl->Cfg_Clk_Uart[uart->id] = uart->divider;
#elif defined(CHIP_DIE_8909)
// TODO
#else
    uart->hwp->ctrl |= uart->dividerMode;
    hwp_sysCtrl->Cfg_Clk_Uart[uart->id] = SYS_CTRL_UART_DIVIDER(uart->divider) | SYS_CTRL_UART_SEL_PLL_SLOW;
#endif
}

static void DRV_UartAutoBaudCheck(void *param)
{
#if AUTO_BAUD_SUPPORTED
    uint32_t sc = hal_SysEnterCriticalSection();

    DRV_UART_T *uart = (DRV_UART_T *)param;
    if (uart->state != DRV_UART_ST_BAUD_LOCKING)
        goto unlock_exit;

    uint32_t status = uart->hwp->status;
    if (status & UART_AUTO_BAUD_LOCKED)
    {
        DRVLOGI("UART%d baud locked, status=0x%08x", uart->id, status);
        uart->state = DRV_UART_ST_OPEN;
        if (uart->cfg.autoBaudLC)
            DRV_CBufWrite(&uart->rxcb, "at", 2);
        else
            DRV_CBufWrite(&uart->rxcb, "AT", 2);
        goto unlock_exit;
    }

    if (status & UART_CHARACTER_MISCOMPARE)
    {
        uint32_t ctrl = uart->hwp->ctrl;
        uart->hwp->ctrl = (ctrl & ~UART_AUTO_ENABLE);
        hal_SysWaitMicrosecond(20);
        uart->hwp->ctrl = (ctrl | UART_AUTO_ENABLE);

        // maybe there are garbage inside
        hwp_sysIfc->std_ch[uart->rxIfcCh].control |= SYS_IFC_DISABLE;
        uart->hwp->CMD_Set = UART_RX_FIFO_RESET;
        DRV_CBufFlush(&uart->rxcb);
        DRV_UartStartRxIfc(uart);

        COS_StartFunctionTimer(UART_AUTO_BAUD_POLL_PERIOD, DRV_UartAutoBaudCheck, uart);
    }

unlock_exit:
    hal_SysExitCriticalSection(sc);
#endif
}

static void DRV_UartEnableUart(DRV_UART_T *uart)
{
#ifdef CHIP_DIE_8955
    switch (uart->id)
    {
    case 1:
        hwp_iomux->pad_GPIO_0_cfg = IOMUX_PAD_GPIO_0_SEL_FUN_UART1_RXD_SEL;
        hwp_iomux->pad_GPIO_1_cfg = IOMUX_PAD_GPIO_1_SEL_FUN_UART1_TXD_SEL;
        if (uart->cfg.ctsEnabled)
            hwp_iomux->pad_GPIO_2_cfg = IOMUX_PAD_GPIO_2_SEL_FUN_UART1_CTS_SEL;
        if (uart->cfg.rtsEnabled)
            hwp_iomux->pad_GPIO_3_cfg = IOMUX_PAD_GPIO_3_SEL_FUN_UART1_RTS_SEL;
        break;
    case 2:
        hwp_iomux->pad_GPIO_4_cfg = IOMUX_PAD_GPIO_4_SEL_FUN_UART2_RXD_SEL;
        hwp_iomux->pad_GPIO_5_cfg = IOMUX_PAD_GPIO_5_SEL_FUN_UART2_TXD_SEL;
        if (uart->cfg.ctsEnabled)
            hwp_iomux->pad_GPIO_6_cfg = IOMUX_PAD_GPIO_6_SEL_FUN_UART2_CTS_SEL;
        if (uart->cfg.rtsEnabled)
            hwp_iomux->pad_GPIO_7_cfg = IOMUX_PAD_GPIO_7_SEL_FUN_UART2_RTS_SEL;
        break;
    }
#endif
}

static bool DRV_UartStartRxIfc(DRV_UART_T *uart)
{
    uart->rxIfcCh = SYS_IFC_CH_TO_USE(hwp_sysIfc->get_ch);
    if (uart->rxIfcCh >= SYS_IFC_STD_CHAN_NB)
    {
        uart->rxIfcCh = HAL_UNKNOWN_CHANNEL;
        return false;
    }

    hwp_sysIfc->std_ch[uart->rxIfcCh].start_addr = KSEG01_PHY_ADDR(uart->cfg.rxDmaBuf);
    hwp_sysIfc->std_ch[uart->rxIfcCh].tc = uart->cfg.rxDmaSize;
    hwp_sysIfc->std_ch[uart->rxIfcCh].control = SYS_IFC_REQ_SRC(uart->rxIfcId) |
                                                HAL_IFC_SIZE_8_MODE_MANUAL |
                                                SYS_IFC_CH_RD_HW_EXCH |
                                                SYS_IFC_ENABLE;
    return true;
}

bool DRV_UartOpen(DRV_UART_T *uart)
{
    DRVLOGI("UART%d open, baud=%d", uart->id, uart->cfg.baud);

    DRV_UartEnableUart(uart);
    hal_UartAllowSleep(uart->id, false);

    // Disable it (in case it is already enabled)
    uint32_t oldctrl = uart->hwp->ctrl;
    oldctrl &= ~UART_ENABLE;
#if AUTO_BAUD_SUPPORTED
    oldctrl &= ~UART_AUTO_ENABLE;
#endif
    uart->hwp->ctrl = oldctrl;

    uart->hwp->CMD_Clr = UART_RI |
                         UART_DCD |
                         UART_DSR |
                         UART_TX_BREAK_CONTROL |
                         UART_TX_FINISH_N_WAIT |
                         UART_RTS;
    uart->hwp->CMD_Set = UART_RX_FIFO_RESET | UART_TX_FIFO_RESET;

    uint32_t ctrl = UART_ENABLE |
                    UART_DMA_MODE |
                    UART_RX_BREAK_LENGTH(13);
    ctrl |= (uart->cfg.dataBits == DRV_UART_DATA_BITS_7) ? UART_DATA_BITS_7_BITS : UART_DATA_BITS_8_BITS;
    ctrl |= (uart->cfg.stopBits == DRV_UART_STOP_BITS_1) ? UART_TX_STOP_BITS_1_BIT : UART_TX_STOP_BITS_2_BITS;

    switch (uart->cfg.parity)
    {
    case DRV_UART_ODD_PARITY:
        ctrl |= UART_PARITY_ENABLE_YES | UART_PARITY_SELECT_ODD;
        break;
    case DRV_UART_EVEN_PARITY:
        ctrl |= UART_PARITY_ENABLE_YES | UART_PARITY_SELECT_EVEN;
        break;
    case DRV_UART_SPACE_PARITY:
        ctrl |= UART_PARITY_ENABLE_YES | UART_PARITY_SELECT_SPACE;
        break;
    case DRV_UART_MARK_PARITY:
        ctrl |= UART_PARITY_ENABLE_YES | UART_PARITY_SELECT_MARK;
        break;
    case DRV_UART_NO_PARITY:
    default:
        ctrl |= UART_PARITY_ENABLE_NO;
        break;
    }

    uint32_t trigger = UART_RX_TRIGGER(0);
    if (uart->cfg.rtsEnabled && uart->cfg.autoFlowCtrlLevel != DRV_UART_DISABLE_AUTO_FLOW_CTRL)
    {
        ctrl |= UART_AUTO_FLOW_CONTROL_ENABLE;
    }
    else
    {
        ctrl |= UART_AUTO_FLOW_CONTROL_DISABLE;
        trigger |= UART_AFC_LEVEL(uart->cfg.autoFlowCtrlLevel);
    }
    uart->hwp->triggers = trigger;

    uart->state = DRV_UART_ST_OPEN;
    DRV_UartCalcBaudConfig(uart);
    hwp_sysCtrl->Cfg_Clk_Uart[uart->id] = uart->divider;
    ctrl |= uart->dividerMode;
    uart->irqMask = UART_RX_DMA_TIMEOUT |
                    UART_RX_DATA_AVAILABLE |
                    UART_RX_DMA_DONE |
                    UART_RX_LINE_ERR |
                    UART_TX_DMA_DONE;
    uart->hwp->irq_mask = 0; // IRQ will be enabled later
    hal_UartIrqSetHandler(uart->id, DRV_UartIrq, (uint32_t)uart);

    DRV_CBufFlush(&uart->rxcb);
    DRV_CBufFlush(&uart->txcb);
    uart->hwp->ctrl = ctrl;

#if AUTO_BAUD_SUPPORTED
    if (uart->cfg.baud == 0)
    {
        // sequence of (!AUTO_ENABLE -> AUTO_ENABLE) under UART_ENABLE is needed
        uart->state = DRV_UART_ST_BAUD_LOCKING;
        hal_SysWaitMicrosecond(20);
        ctrl |= UART_AUTO_ENABLE;
        if (uart->cfg.autoBaudLC)
            ctrl |= UART_SYNC_CHAR_SEL;
        uart->hwp->ctrl = ctrl;
    }
#endif
    uart->hwp->irq_mask = uart->irqMask;

    // TODO: error handling on RX IFC start failure
    DRV_UartStartRxIfc(uart);

    if (uart->cfg.rxPollPeriod != 0)
        COS_StartFunctionTimer(uart->cfg.rxPollPeriod, DRV_UartRxPoll, uart);
    if (uart->state == DRV_UART_ST_BAUD_LOCKING)
        COS_StartFunctionTimer(UART_AUTO_BAUD_POLL_PERIOD, DRV_UartAutoBaudCheck, uart);
    return true;
}

static void DRV_UartReleaseTxIfcLocked(DRV_UART_T *uart)
{
    if (uart->txIfcCh == HAL_UNKNOWN_CHANNEL)
        return;

    uint32_t control = hwp_sysIfc->std_ch[uart->txIfcCh].control;
    if (GET_BITFIELD(control, SYS_IFC_REQ_SRC) != uart->txIfcId)
        return;
    hwp_sysIfc->std_ch[uart->txIfcCh].control = control | SYS_IFC_DISABLE;
    uart->txIfcCh = HAL_UNKNOWN_CHANNEL;
}

static void DRV_UartCloseTxDoneLocked(DRV_UART_T *uart)
{
    DRVLOGI("UART%d close done", uart->id);

    uart->hwp->ctrl &= ~UART_ENABLE;
    uart->hwp->irq_mask = 0;
    uart->hwp->irq_cause = 0xffff;
    uart->hwp->CMD_Set = UART_RX_FIFO_RESET | UART_TX_FIFO_RESET;
    if (uart->rxIfcCh != HAL_UNKNOWN_CHANNEL)
        hwp_sysIfc->std_ch[uart->rxIfcCh].control |= SYS_IFC_DISABLE;
    DRV_UartReleaseTxIfcLocked(uart);
    uart->rxIfcCh = HAL_UNKNOWN_CHANNEL;
    uart->txIfcCh = HAL_UNKNOWN_CHANNEL;

    hal_UartAllowSleep(uart->id, true);
    DRV_CBufFlush(&uart->rxcb);
    COS_StopFunctionTimer(DRV_UartAutoBaudCheck, uart);
    COS_StopFunctionTimer(DRV_UartRxPoll, uart);
    COS_StopFunctionTimer(DRV_UartTxPoll, uart);
}

static bool DRV_UartTxIfcRunning(DRV_UART_T *uart)
{
    return (uart->txIfcCh != HAL_UNKNOWN_CHANNEL &&
            (GET_BITFIELD(hwp_sysIfc->std_ch[uart->txIfcCh].control, SYS_IFC_REQ_SRC) == uart->txIfcId) &&
            (hwp_sysIfc->std_ch[uart->txIfcCh].status & SYS_IFC_ENABLE) &&
            (hwp_sysIfc->std_ch[uart->txIfcCh].tc != 0));
}

bool DRV_UartWaitTxFinish(DRV_UART_T *uart, unsigned timeout)
{
    unsigned starttick = hal_TimGetUpTime();
    unsigned toticks = COS_Msec2Tick(timeout);
    for (;;)
    {
        uint32_t sc = hal_SysEnterCriticalSection();
        bool done = DRV_CBufIsEmpty(&uart->txcb) &&
                    !DRV_UartTxIfcRunning(uart) &&
                    (uart->hwp->status & UART_TX_ACTIVE) == 0;
        hal_SysExitCriticalSection(sc);

        if (done)
            return true;
        if ((hal_TimGetUpTime() - starttick) > toticks)
        {
            DRVLOGI("UART%d wait tx finish failed", uart->id);
            return false;
        }
        COS_Sleep(5);
    }
}

void DRV_UartClose(DRV_UART_T *uart, bool forcedly)
{
    DRVLOGI("UART%d close, forcedly=%d", uart->id, forcedly);

    uint32_t sc = hal_SysEnterCriticalSection();
    if (uart->state != DRV_UART_ST_CLOSE && uart->state != DRV_UART_ST_CLOSE_REQ)
    {
        if (DRV_CBufIsEmpty(&uart->txcb))
        {
            DRV_UartCloseTxDoneLocked(uart);
        }
        else if (forcedly)
        {
            DRV_UartReleaseTxIfcLocked(uart);
            uart->txPendingSize = 0;
            DRV_CBufFlush(&uart->txcb);
            DRV_UartCloseTxDoneLocked(uart);
        }
        else
        {
            uart->state = DRV_UART_ST_CLOSE_REQ;
        }
    }
    hal_SysExitCriticalSection(sc);
}

static void DRV_UartSleepTxDoneLocked(DRV_UART_T *uart)
{
    DRVLOGI("UART%d sleep done", uart->id);

    if (uart->wakeupEnabled)
        uart->hwp->ctrl &= ~UART_DMA_MODE;
    else
        uart->hwp->ctrl &= ~UART_ENABLE;

    uart->state = DRV_UART_ST_SLEEP;
    DRV_UartSetSleepClock(uart);
    hal_UartAllowSleep(uart->id, true);
}

void DRV_UartSleep(DRV_UART_T *uart, bool forcedly, bool wakeupEnabled)
{
    DRVLOGI("UART%d sleep forcedly=%d wakupEnabled=%d", uart->id, forcedly, wakeupEnabled);

    uint32_t sc = hal_SysEnterCriticalSection();
    if (uart->state == DRV_UART_ST_OPEN || uart->state == DRV_UART_ST_BAUD_LOCKING)
    {
        uart->wakeupEnabled = wakeupEnabled;
        if (DRV_CBufIsEmpty(&uart->txcb))
        {
            DRV_UartSleepTxDoneLocked(uart);
        }
        else if (forcedly)
        {
            DRV_UartReleaseTxIfcLocked(uart);
            uart->txPendingSize = 0;
            DRV_CBufFlush(&uart->txcb);
            DRV_UartSleepTxDoneLocked(uart);
        }
        else
        {
            uart->state = DRV_UART_ST_SLEEP_REQ;
        }
    }
    hal_SysExitCriticalSection(sc);
}

void DRV_UartWakeup(DRV_UART_T *uart)
{
    DRVLOGI("UART%d wakeup", uart->id);

    uint32_t sc = hal_SysEnterCriticalSection();
    if (uart->state == DRV_UART_ST_SLEEP || uart->state == DRV_UART_ST_SLEEP_REQ)
    {
        if (uart->state == DRV_UART_ST_SLEEP)
        {
            DRV_UartRestoreClock(uart);
            uart->hwp->CMD_Set = UART_RX_FIFO_RESET;
            uart->hwp->ctrl |= (UART_DMA_MODE | UART_ENABLE);
            hal_UartAllowSleep(uart->id, false);
        }
        uart->state = DRV_UART_ST_OPEN;
    }
    hal_SysExitCriticalSection(sc);
}

static void DRV_UartRxPoll(void *param)
{
    DRV_UART_T *uart = (DRV_UART_T *)param;
    if (uart->rxIfcCh == HAL_UNKNOWN_CHANNEL || uart->state == DRV_UART_ST_BAUD_LOCKING)
        return;

    uint32_t sc = hal_SysEnterCriticalSection();

    hwp_sysIfc->std_ch[uart->rxIfcCh].control |= SYS_IFC_FLUSH;
    while (!(hwp_sysIfc->std_ch[uart->rxIfcCh].status & SYS_IFC_FIFO_EMPTY))
        ;

    int size = uart->cfg.rxDmaSize - hwp_sysIfc->std_ch[uart->rxIfcCh].tc;
    int trans = 0;
    if (size > 0)
    {
        hwp_sysIfc->std_ch[uart->rxIfcCh].control |= SYS_IFC_DISABLE;
        trans = DRV_CBufWrite(&uart->rxcb, uart->cfg.rxDmaBuf, size);

        // It won't fail, due to a channel is just disabled.
        DRV_UartStartRxIfc(uart);
    }
    else
    {
        hwp_sysIfc->std_ch[uart->rxIfcCh].control &= ~SYS_IFC_FLUSH;
    }

    if (uart->cfg.rxPollPeriod != 0)
        COS_StartFunctionTimer(uart->cfg.rxPollPeriod, (COS_CALLBACK_FUNC_T)DRV_UartRxPoll, uart);

    hal_SysExitCriticalSection(sc);

    if (size > 0 && uart->cfg.callback != NULL)
    {
        uint32_t evt = DRV_UART_RX_ARRIVED;
        if (trans < size)
            evt |= DRV_UART_RX_OVERFLOW;
        uart->cfg.callback(uart, uart->cfg.callbackParam, evt);
    }
}

static void DRV_UartTxPoll(DRV_UART_T *uart)
{
    uint32_t sc = hal_SysEnterCriticalSection();

    if (DRV_CBufIsEmpty(&uart->txcb) || uart->state == DRV_UART_ST_BAUD_LOCKING)
        goto unlock_exit;

    if (uart->txIfcCh != HAL_UNKNOWN_CHANNEL)
    {
        if ((GET_BITFIELD(hwp_sysIfc->std_ch[uart->txIfcCh].control, SYS_IFC_REQ_SRC) == uart->txIfcId) &&
            (hwp_sysIfc->std_ch[uart->txIfcCh].status & SYS_IFC_ENABLE) &&
            (hwp_sysIfc->std_ch[uart->txIfcCh].tc != 0))
            goto unlock_exit;
    }

    if (uart->txPendingSize > 0)
        DRV_CBufSkipBytes(&uart->txcb, uart->txPendingSize);
    uart->txPendingSize = 0;

    unsigned txsize = 0;
    uint8_t *txbuf = DRV_CBufLinearRead(&uart->txcb, &txsize);
    if (txsize == 0)
        goto unlock_exit;

    uart->txIfcCh = SYS_IFC_CH_TO_USE(hwp_sysIfc->get_ch);
    if (uart->txIfcCh >= SYS_IFC_STD_CHAN_NB)
    {
        uart->txIfcCh = HAL_UNKNOWN_CHANNEL;
        COS_StartFunctionTimer(TX_IFC_UNAVAIL_POLL_PERIOD, (COS_CALLBACK_FUNC_T)DRV_UartTxPoll, uart);
    }
    else
    {
#ifdef CHIP_XTAL_CLK_IS_52M
        if (ADDRESS_IN_HWP(txbuf, hwp_cs1))
            hal_SysRequestDdrHighFreq(true);
#endif

        DRVLOGV("UART%d tx ch/%d tc/%d", uart->id, uart->txIfcCh, txsize);
        hwp_sysIfc->std_ch[uart->txIfcCh].start_addr = KSEG01_PHY_ADDR(txbuf);
        hwp_sysIfc->std_ch[uart->txIfcCh].tc = txsize;
        hwp_sysIfc->std_ch[uart->txIfcCh].control = SYS_IFC_REQ_SRC(uart->txIfcId) |
                                                    HAL_IFC_SIZE_8_MODE_AUTO |
                                                    SYS_IFC_CH_RD_HW_EXCH |
                                                    SYS_IFC_ENABLE;
        uart->txPendingSize = txsize;
    }

unlock_exit:
    hal_SysExitCriticalSection(sc);
}

static void DRV_UartIrq(HAL_UART_IRQ_STATUS_T status, HAL_UART_ERROR_STATUS_T error, UINT32 param)
{
    DRV_UART_T *uart = (DRV_UART_T *)param;
    uint32_t evt = 0;

    DRVLOGV("UART%d state=%d IRQ status=0x%x, error=0x%x",
            uart->id, uart->state, *(uint32_t *)&status, *(uint32_t *)&error);

    // Sign... hal_uart may change irq_mask
    uart->hwp->irq_mask = uart->irqMask;

    if (uart->state == DRV_UART_ST_CLOSE)
        return;

    if (uart->state == DRV_UART_ST_BAUD_LOCKING)
    {
        COS_StopFunctionTimer(DRV_UartAutoBaudCheck, uart);
        DRV_UartAutoBaudCheck(uart);
        if (uart->state == DRV_UART_ST_BAUD_LOCKING)
            return;
    }

    if (uart->state == DRV_UART_ST_SLEEP)
    {
        if (status.rxDataAvailable || status.rxTimeout || status.rxLineErr || error.rxBreakInt)
            DRV_UartWakeup(uart);

        evt |= DRV_UART_WAKE_UP;
        goto send_evt;
    }

    if (uart->state == DRV_UART_ST_SLEEP_REQ)
    {
        if (status.rxDmaDone || status.rxDmaTimeout)
        {
            uart->state = DRV_UART_ST_OPEN;
            evt |= DRV_UART_WAKE_UP;
            goto open_handler;
        }

        if (status.txDmaDone)
        {
            if (uart->txPendingSize > 0)
                DRV_CBufSkipBytes(&uart->txcb, uart->txPendingSize);

            if (DRV_CBufIsEmpty(&uart->txcb))
                DRV_UartSleepTxDoneLocked(uart);
            else
                DRV_UartTxPoll(uart);
        }
        return;
    }

    if (uart->state == DRV_UART_ST_CLOSE_REQ)
    {
        if (status.txDmaDone)
        {
            if (uart->txPendingSize > 0)
                DRV_CBufSkipBytes(&uart->txcb, uart->txPendingSize);

            if (DRV_CBufIsEmpty(&uart->txcb))
                DRV_UartCloseTxDoneLocked(uart);
            else
                DRV_UartTxPoll(uart);
        }
        return;
    }

open_handler:
    if (status.rxDmaDone || status.rxDmaTimeout)
    {
        DRV_UartRxPoll(uart);
    }

    if (status.txDmaDone)
    {
        if (uart->txPendingSize > 0)
            DRV_CBufSkipBytes(&uart->txcb, uart->txPendingSize);
        uart->txPendingSize = 0;

        if (DRV_CBufIsEmpty(&uart->txcb))
            evt |= DRV_UART_TX_DONE;
        else
            DRV_UartTxPoll(uart);
    }

    if (status.rxLineErr)
        evt |= DRV_UART_LINE_ERR;

    if (error.rxBreakInt)
        evt |= DRV_UART_BREAK;

send_evt:
    evt &= uart->cfg.evtMask;
    if (evt != 0 && uart->cfg.callback != NULL)
        uart->cfg.callback(uart, uart->cfg.callbackParam, evt);
}

int DRV_UartWrite(DRV_UART_T *uart, const uint8_t *data, unsigned size, unsigned timeout)
{
    if (uart == NULL || data == NULL || uart->state != DRV_UART_ST_OPEN)
        return -1;
    if (size == 0)
        return 0;

    unsigned starttick = -1U;
    unsigned toticks = COS_Msec2Tick(timeout);
    unsigned total = size;
    for (;;)
    {
        uint32_t sc = hal_SysEnterCriticalSection();
        unsigned trans = DRV_CBufWrite(&uart->txcb, data, size);
        DRV_UartTxPoll(uart);
        hal_SysExitCriticalSection(sc);

        size -= trans;
        data += trans;
        if (size == 0)
            break;

        if (trans != 0)
            starttick = -1U;
        else if (starttick == -1U)
            starttick = hal_TimGetUpTime();
        else if ((hal_TimGetUpTime() - starttick) > toticks)
            break;

        COS_Sleep(5);
    }

    DRVLOGV("UART%d write size=%d written=%d", uart->id, total, total - size);
    return total - size;
}

int DRV_UartRead(DRV_UART_T *uart, uint8_t *data, unsigned size)
{
    if (uart == NULL || data == NULL || uart->state != DRV_UART_ST_OPEN)
        return -1;
    if (size == 0)
        return 0;

    uint32_t sc = hal_SysEnterCriticalSection();
    unsigned trans = DRV_CBufRead(&uart->rxcb, data, size);
    hal_SysExitCriticalSection(sc);

    DRVLOGV("UART%d read size=%d read=%d", uart->id, size, trans);
    return trans;
}
