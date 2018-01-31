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

#include <bl_uart.h>
#include <bl_platform.h>

#include <global_macros.h>
#include <iomux.h>
#include <sys_ctrl.h>
#include <sys_ifc.h>
#include <hal_sys.h>
#include <hal_sys_ifc.h>
#include <uart.h>

#if defined(CHIP_DIE_8955)
#define BL_UART_QTY 2
#elif defined(CHIP_DIE_8909)
#define BL_UART_QTY 4
#endif

#undef BL_UART_UNCACHED_ADDR
#define BL_UART_UNCACHED_ADDR(addr) KSEG1((uint32_t)addr)

typedef struct
{
    uint8_t *buf;
    uint32_t size;
    uint32_t rd;
    uint32_t payload;
} BL_RING_BUF_T;

static inline void
bl_ring_buf_init(BL_RING_BUF_T *ring, uint8_t *buf, uint32_t size)
{
    ring->buf = buf;
    ring->size = size;
    ring->rd = 0;
    ring->payload = 0;
}

static inline void
bl_ring_buf_purge(BL_RING_BUF_T *ring)
{
    ring->rd = 0;
    ring->payload = 0;
}

static inline bool
bl_ring_buf_is_empty(BL_RING_BUF_T *ring)
{
    return (ring->payload == 0);
}

static inline bool
bl_ring_buf_is_full(BL_RING_BUF_T *ring)
{
    return (ring->payload == ring->size);
}

static inline uint32_t
bl_ring_buf_payload_size(BL_RING_BUF_T *ring)
{
    return ring->payload;
}

static inline uint32_t
bl_ring_buf_remain_size(BL_RING_BUF_T *ring)
{
    return ring->size - ring->payload;
}

static uint32_t
bl_ring_buf_write(BL_RING_BUF_T *ring, const void *buf, uint32_t size)
{
    uint32_t room = ring->size - ring->payload;
    if (size > room)
        size = room;
    if (size == 0)
        return 0;

    uint32_t start = ring->rd + ring->payload;
    if (start > ring->size)
        start = start - ring->size;

    int toend = ring->size - start;
    if (size <= toend)
    {
        memcpy(ring->buf + start, buf, size);
    }
    else
    {
        memcpy(ring->buf + start, buf, toend);
        memcpy(ring->buf, buf + toend, size - toend);
    }
    ring->payload += size;
    return size;
}

static uint32_t
bl_ring_buf_read(BL_RING_BUF_T *ring, void *buf, uint32_t size)
{
    if (size > ring->payload)
        size = ring->payload;
    if (size == 0)
        return 0;

    uint32_t toend = ring->size - ring->rd;
    if (size <= toend)
    {
        memcpy(buf, ring->buf + ring->rd, size);
    }
    else
    {
        memcpy(buf, ring->buf + ring->rd, toend);
        memcpy(buf + toend, ring->buf, size - toend);
    }
    ring->payload -= size;
    ring->rd += size;
    if (ring->rd > ring->size)
        ring->rd -= ring->size;

    return size;
}

struct BL_UART_DEV
{
    uint32_t id;
    uint32_t irqMask;
    HWP_UART_T *hwp;
    BL_UART_CFG_T cfg;
    BL_RING_BUF_T rxRingBuf;
    uint8_t rxIfcCh;
    HAL_IFC_REQUEST_ID_T rxIfcId;
};

uint32_t bl_uart_struct_size()
{
    return sizeof(struct BL_UART_DEV);
}

static inline uint32_t
bl_uart_write_fifo(BL_UART_DEV_T *uart, const void *data_, uint32_t size)
{
    HWP_UART_T *hwp = uart->hwp;
    const uint8_t *data = (const uint8_t *)data_;
    uint32_t fifo_space = ((hwp->status & UART_TX_FIFO_SPACE_MASK) >> UART_TX_FIFO_SPACE_SHIFT);
    uint32_t idx = 0;
    for (; idx < fifo_space && idx < size; ++idx)
    {
        hwp->rxtx_buffer = data[idx];
    }
    return idx;
}

static inline bool
bl_uart_tx_fifo_empty(BL_UART_DEV_T *uart)
{
    return !(uart->hwp->status & UART_TX_ACTIVE);
}

static inline HWP_UART_T *const bl_uart_id2hw(uint32_t id)
{
#ifdef CHIP_DIE_8955
    HWP_UART_T *const hwp_uarts[] = { hwp_uart, hwp_uart2 };
#elif defined(CHIP_DIE_8909)
    HWP_UART_T *const hwp_uarts[] = { hwp_uart1, hwp_uart2, hwp_uart3, hwp_uart4 };
#endif
    return hwp_uarts[id - 1];
}

static inline void bl_uart_assign_request_id(BL_UART_DEV_T *uart)
{
#ifdef CHIP_DIE_8955
    const HAL_IFC_REQUEST_ID_T rx_ids[] = { HAL_IFC_UART1_RX, HAL_IFC_UART2_RX };
#elif defined(CHIP_DIE_8909)
    const HAL_IFC_REQUEST_ID_T rx_ids[] =
    {
        HAL_IFC_UART1_RX, HAL_IFC_UART2_RX, HAL_IFC_UART3_RX, HAL_IFC_UART4_RX
    };
#endif
    uart->rxIfcId = rx_ids[uart->id - 1];
}

bool bl_uart_init(BL_UART_DEV_T *uart, uint32_t uartid, const BL_UART_CFG_T *cfg)
{
    if (uartid < 1 || uartid > BL_UART_QTY)
        return false;

    if (uart == NULL || cfg == NULL)
        return false;

    uart->id = uartid;
    uart->cfg = *cfg;
    uart->hwp = bl_uart_id2hw(uart->id);
    if (uart->hwp == NULL)
        return false;
    bl_uart_assign_request_id(uart);
    bl_ring_buf_init(&uart->rxRingBuf, cfg->rxBuf, cfg->rxBufSize);

    return true;
}

void bl_uart_update_config(BL_UART_DEV_T *uart, const BL_UART_CFG_T *cfg)
{
    uart->cfg = *cfg;
}

static void bl_uart_enable(BL_UART_DEV_T *uart)
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

static uint32_t bl_uart_calc_baud_config(BL_UART_DEV_T *uart)
{
#ifdef CHIP_DIE_8909
#define SYS_CTRL_CFG_UART_NUM SYS_CTRL_UART_DIV_NUM
#define SYS_CTRL_CFG_UART_DENOM SYS_CTRL_UART_DIV_DENOM
#endif
    uint32_t divider;
    if (uart->cfg.baudRate == 0)
    {
        divider = SYS_CTRL_CFG_UART_NUM(1) | SYS_CTRL_CFG_UART_DENOM(1);
    }
    else
    {
        switch (uart->cfg.baudRate)
        {
            case 1200:
                divider = SYS_CTRL_CFG_UART_NUM(1) | SYS_CTRL_CFG_UART_DENOM(10833);
                break;
            case 2400:
                divider = SYS_CTRL_CFG_UART_NUM(3) | SYS_CTRL_CFG_UART_DENOM(16250);
                break;
            default:
                // It works from 4800 to 921600: 52M/4/8125 = 1600
                divider = SYS_CTRL_CFG_UART_NUM(uart->cfg.baudRate / 1600)
                    | SYS_CTRL_CFG_UART_DENOM(8125);
                break;
        }
    }
    return divider;
}

static bool bl_uart_start_rx_ifc(BL_UART_DEV_T *uart)
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

bool bl_uart_open(BL_UART_DEV_T *uart)
{
    bl_uart_enable(uart);
    uint32_t oldctrl = uart->hwp->ctrl;
    oldctrl &= ~UART_ENABLE;

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
    ctrl |= (uart->cfg.dataBits == DRV_UART_DATA_BITS_7) ?
        UART_DATA_BITS_7_BITS : UART_DATA_BITS_8_BITS;
    ctrl |= (uart->cfg.stopBits == DRV_UART_STOP_BITS_1) ?
        UART_TX_STOP_BITS_1_BIT : UART_TX_STOP_BITS_2_BITS;
    ctrl |= UART_PARITY_ENABLE_NO;

    uint32_t trigger = UART_RX_TRIGGER(0);
    ctrl |= UART_AUTO_FLOW_CONTROL_DISABLE;
    trigger |= UART_AFC_LEVEL(0xff /*DRV_UART_DISABLE_AUTO_FLOW_CTRL*/);
    uart->hwp->triggers = trigger;

    uint32_t divider = bl_uart_calc_baud_config(uart);
#ifdef CHIP_DIE_8909
    hwp_sysCtrl->cfg_clk_uart[uart->id] = divider;
#else
    hwp_sysCtrl->Cfg_Clk_Uart[uart->id] = divider;
#endif
    uart->hwp->ctrl = ctrl;

    if (!bl_uart_start_rx_ifc(uart))
        return false;

    return true;
}

void bl_uart_close(BL_UART_DEV_T *uart)
{
    HWP_UART_T *hwp = uart->hwp;
    if (hwp == NULL)
        return;

    hwp->ctrl &= ~UART_ENABLE;
    hwp->CMD_Set = UART_RX_FIFO_RESET | UART_TX_FIFO_RESET;
    if (uart->rxIfcCh != HAL_UNKNOWN_CHANNEL)
        hwp_sysIfc->std_ch[uart->rxIfcCh].control |= SYS_IFC_DISABLE;
    uart->rxIfcCh = HAL_UNKNOWN_CHANNEL;
    return;
}

bool bl_uart_wait_tx_finished(BL_UART_DEV_T *uart, int timeout)
{
    bool indefinite = (timeout == -1);
    for (uint32_t slp = 0; slp < timeout || indefinite; bl_delay_us(1000), slp+=1)
    {
        if (bl_uart_tx_fifo_empty(uart))
            return true;
    }
    return false;
}

int bl_uart_write(BL_UART_DEV_T *uart, const void *data, uint32_t size)
{
    uint32_t total = 0;
    while (total < size)
    {
        uint32_t len = bl_uart_write_fifo(uart, data + total, size - total);
        total += len;
    }

    return total;
}

static bool bl_uart_rx_poll(BL_UART_DEV_T *uart)
{
    if (uart->rxIfcCh == HAL_UNKNOWN_CHANNEL)
        return false;

    // fifo empty
    if (hwp_sysIfc->std_ch[uart->rxIfcCh].status & SYS_IFC_FIFO_EMPTY)
        return true;

    uint32_t last_tc = hwp_sysIfc->std_ch[uart->rxIfcCh].tc;
    uint32_t filled = uart->cfg.rxDmaSize - last_tc;
    if (filled != 0)
    {
        bl_ring_buf_write(&uart->rxRingBuf, (const void *)BL_UART_UNCACHED_ADDR(uart->cfg.rxDmaBuf), filled);
    }

    hwp_sysIfc->std_ch[uart->rxIfcCh].control |= SYS_IFC_FLUSH;
    while (!(hwp_sysIfc->std_ch[uart->rxIfcCh].status & SYS_IFC_FIFO_EMPTY)) {}
    uint32_t tc = hwp_sysIfc->std_ch[uart->rxIfcCh].tc;
    hwp_sysIfc->std_ch[uart->rxIfcCh].control |= SYS_IFC_DISABLE;
    if (last_tc != tc)
    {
        uint32_t remain = last_tc - tc;
        bl_ring_buf_write(&uart->rxRingBuf, (const void *)BL_UART_UNCACHED_ADDR(uart->cfg.rxDmaBuf + filled), remain);
    }
    bl_uart_start_rx_ifc(uart);

    return true;
}

const BL_UART_CFG_T *bl_uart_get_cfg(BL_UART_DEV_T *uart)
{
    return &uart->cfg;
}

void bl_uart_rx_purge(BL_UART_DEV_T *uart)
{
    bl_uart_rx_poll(uart);
    bl_ring_buf_purge(&uart->rxRingBuf);
}

int bl_uart_read(BL_UART_DEV_T *uart, void *data, uint32_t size)
{
    if (!bl_uart_rx_poll(uart))
        return -1;

    int len = (int)bl_ring_buf_read(&uart->rxRingBuf, data, size);
    return len;
}

uint32_t bl_uart_rxbuf_count(BL_UART_DEV_T *uart)
{
    if (!bl_uart_rx_poll(uart))
        return 0;

    return bl_ring_buf_payload_size(&uart->rxRingBuf);
}
