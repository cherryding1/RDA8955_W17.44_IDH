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
#include "chip_id.h"

#include "global_macros.h"
#include "debug_uart.h"
#include "debug_host.h"
#include "sys_irq.h"
#include "sys_ifc.h"

#include "boot_usb_monitor.h"

#include "hal_trace.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "hal_host.h"
#include "hal_map.h"
#include "halp_sys_ifc.h"
#include "halp_sys.h"
#include "halp_debug.h"

#include "sxs_srl.h"
#include "sxs_rmt.h"
#include "sxs_rmc.h"
#include "sxs_idle.h"

#include "string.h"
#include "rfd_xcv.h"
#include "hal_trace_p.h"

#if defined(RAMRUN) || defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
#define HAL_TRACE_USE_NONE
#define HAL_TRACE_API_ALIAS(name) __attribute__((alias(STRINGIFY(hal_TraceNone##name))))
#elif defined(CHIP_XTAL_CLK_IS_52M)
#define HAL_TRACE_USE_DUALRAM
#define HAL_TRACE_API_ALIAS(name) __attribute__((alias(STRINGIFY(hal_TraceDual##name))))
#else
#define HAL_TRACE_USE_ONERAM
#define HAL_TRACE_API_ALIAS(name) __attribute__((alias(STRINGIFY(hal_TraceOne##name))))
#endif

HAL_TRACE_RX_CTX_T g_halTraceRxCtx;

#if defined(HAL_TRACE_USE_DUALRAM)
#define HAL_TRACE_PSRAM_BUFFER_SIZE (64 * 1024)
#define HAL_TRACE_ISRAM_BUFFER_SIZE ((2 * 1024) - 512)
static UINT32 HAL_UNCACHED_BSS g_halTracePsramBuffer[HAL_TRACE_PSRAM_BUFFER_SIZE / 4];
static UINT32 HAL_UNCACHED_BSS_INTERNAL g_halTraceIsramBuffer[HAL_TRACE_ISRAM_BUFFER_SIZE / 4];
HAL_TRACE_DUALRAM_CTX_T HAL_BSS_INTERNAL g_halTraceDualCtx;
#endif

#if defined(HAL_TRACE_USE_ONERAM)
#define HAL_TRACE_RAM_BUFFER_SIZE (64 * 1024)
static UINT32 HAL_UNCACHED_BSS g_halTraceRamBuffer[HAL_TRACE_RAM_BUFFER_SIZE / 4];
HAL_TRACE_ONERAM_CTX_T HAL_BSS_INTERNAL g_halTraceOneCtx;
#endif

VOLATILE UINT32 HAL_UNCACHED_DATA g_halTraceSwitchPortCmd = 0; // NOT USED NOW

// =============================================================================
// static functions
// =============================================================================
static BOOL hal_TraceProcessRx(VOID);
static BOOL hal_TraceChannelIsEmpty(HAL_TRACE_DATA_CHANNEL_T *p);
static void hal_TraceChannelStart(HAL_TRACE_DATA_CHANNEL_T *p);
static void hal_TraceChannelExtend(HAL_TRACE_DATA_CHANNEL_T *p, UINT32 size);
static VOID hal_TraceChannelUpdateIfc(HAL_TRACE_DATA_CHANNEL_T *p);
static void hal_TraceChannelReset(HAL_TRACE_DATA_CHANNEL_T *p);
static UINT32 *hal_TraceChannelRequest(HAL_TRACE_DATA_CHANNEL_T *p, UINT8 id, UINT16 size);
static VOID hal_TraceChannelRequestEnd(HAL_TRACE_DATA_CHANNEL_T *p);
static BOOL hal_TraceChannelInsertStamp(HAL_TRACE_DATA_CHANNEL_T *p, UINT32 *lost, UINT32 *stamp, UINT32 *stampExt);
static VOID hal_TraceOpenDebugUart(VOID);
static VOID hal_TraceCloseDebugUart(VOID);
static void hal_TraceDualChannelSwitch(HAL_TRACE_DATA_CHANNEL_T *pfrom, HAL_TRACE_DATA_CHANNEL_T *pto);
static void hal_TraceDualChannelMaintain(VOID);

// =============================================================================
VOID hal_hstTraceSetRxProc(HAL_TRACE_RX_PROC_T proc)
{
    g_halTraceRxCtx.proc = proc;
}

// =============================================================================
static BOOL hal_TraceProcessRx(VOID)
{
    UINT32 status = hwp_debugUart->status;
    UINT32 avail = GET_BITFIELD(status, DEBUG_UART_RX_FIFO_LEVEL);
    if (avail == 0)
        return FALSE;

    HAL_TRACE_RX_CTX_T *ctx = &g_halTraceRxCtx;
    for (int n = 0; n < avail; n++)
    {
        UINT8 ch = (UINT8)hwp_debugUart->rxtx_buffer;
        if (ctx->len == 0)
        {
            if (ch != 0xAD /*SXS_RMT_START_FRM*/)
                continue;
            ctx->buffer[ctx->len++] = ch;
        }
        else if (ctx->len == 1)
        {
            if (ch != 0)
            {
                ctx->len = 0;
                continue;
            }
            ctx->buffer[ctx->len++] = ch;
        }
        else if (ctx->len == 2)
        {
            ctx->frameLen = ch;
            if (ch + 4 > HAL_TRACE_RX_BUFFER_SIZE)
            {
                ctx->len = 0;
                continue;
            }
            ctx->crc = 0;
            ctx->buffer[ctx->len++] = ch;
        }
        else if (ctx->len + 1 < ctx->frameLen + 4)
        {
            ctx->crc ^= ch;
            ctx->buffer[ctx->len++] = ch;
        }
        else if (ctx->len + 1 == ctx->frameLen + 4)
        {
            if (ctx->crc == ch && ctx->proc != NULL)
                ctx->proc(ctx->buffer[3], ctx->buffer + 4, ctx->frameLen - 1);
            ctx->len = 0;
        }
    }
    return TRUE;
}

// =============================================================================
static BOOL hal_TraceChannelIsEmpty(HAL_TRACE_DATA_CHANNEL_T *p)
{
    if (p->numPendReq == 0 && p->ridx == p->widx)
    {
        // ASSERT(wdoneIdx == widx)
        return TRUE;
    }
    return FALSE;
}

// =============================================================================
static void hal_TraceChannelStart(HAL_TRACE_DATA_CHANNEL_T *p)
{
    hwp_sysIfc->std_ch[p->channel].start_addr = KSEG01_PHY_ADDR(p->buffer + p->ridx);
    hwp_sysIfc->std_ch[p->channel].tc = p->wdoneIdx - p->ridx;
    p->ifcEndIdx = p->wdoneIdx;
}

// =============================================================================
static void hal_TraceChannelExtend(HAL_TRACE_DATA_CHANNEL_T *p, UINT32 size)
{
    hwp_sysIfc->std_ch[p->channel].tc = size;
    p->ifcEndIdx += size;
}

// =============================================================================
static VOID hal_TraceChannelUpdateIfc(HAL_TRACE_DATA_CHANNEL_T *p)
{
    if (p->channel == HAL_UNKNOWN_CHANNEL)
        return;

    UINT32 tc = hwp_sysIfc->std_ch[p->channel].tc;
    INT32 transfered = (p->ifcEndIdx - tc) - p->ridx;

    p->rcount += transfered;
    p->ridx += transfered;
    if (p->ridx >= p->size)
    {
        // Reach end. ASSERT(tc == 0)
        p->ridx = 0;
        hal_TraceChannelStart(p);
    }
    else if (p->wdoneIdx > p->ifcEndIdx)
    {
        // More data in the same round
        if (p->ifcEndIdx == p->ridx)
        {
            hal_TraceChannelStart(p);
        }
        else
        {
            hal_TraceChannelExtend(p, p->wdoneIdx - p->ifcEndIdx);
        }
    }
    else if (p->wdoneIdx < p->ifcEndIdx)
    {
        // Write has come to next round
        if (p->wrapIdx > p->ifcEndIdx)
        {
            // There are remaining data in the same round
            hal_TraceChannelExtend(p, p->wrapIdx - p->ifcEndIdx);
        }
        else if (tc == 0)
        {
            // This round has finished.
            p->ridx = 0;
            hal_TraceChannelStart(p);
        }
    }
}

// =============================================================================
static void hal_TraceChannelReset(HAL_TRACE_DATA_CHANNEL_T *p)
{
    p->numPendReq = 0;
    p->ridx = 0;
    p->widx = 0;
    p->wdoneIdx = 0;
    p->ifcEndIdx = 0;
    p->rcount = 0;
    p->wcount = 0;
}

// =============================================================================
static UINT32 *hal_TraceChannelRequest(HAL_TRACE_DATA_CHANNEL_T *p, UINT8 id, UINT16 size)
{
    // packet format
    //      SYNC (0xAD)
    //      Frame Len MSB
    //      Frame Len LSB
    //      Flow Id
    //      DATA
    int framelen = size;
    int packetLen = framelen + 4;
    int requestLen = (packetLen + 3) & ~3;

    if (p->widx < p->ridx)
    {
        // |------W       R-----|
        if (p->widx + requestLen + 4 > p->ridx)
            return NULL;
    }
    else
    {
        // |      R-------W     |
        if (p->widx + requestLen + 4 < p->size)
        {
            ; // OK
        }
        else if (p->widx + requestLen + 4 == p->size)
        {
            if (p->ridx == 0)
                return NULL;
        }
        else
        {
            if (requestLen + 4 > p->ridx)
                return NULL;
            p->wrapIdx = p->widx;
            p->widx = 0;
        }
    }

    UINT8 *data = p->buffer + p->widx;
    p->numPendReq++;
    p->wcount += requestLen;
    p->widx += requestLen;
    if (p->widx >= p->size)
    {
        p->wrapIdx = p->size;
        p->widx = 0;
    }

    *((UINT32 *)data + requestLen / 4 - 1) = 0;
    data[0] = 0xAD; //SXS_RMT_START_FRM;
    data[1] = (framelen >> 8) & 0xff;
    data[2] = (framelen & 0xff);
    data[3] = id;

    return (UINT32 *)(data + 4);
}

// =============================================================================
static VOID hal_TraceChannelRequestEnd(HAL_TRACE_DATA_CHANNEL_T *p)
{
    p->numPendReq--;
    if (p->numPendReq == 0)
        p->wdoneIdx = p->widx;
}

// =============================================================================
static BOOL hal_TraceChannelInsertStamp(HAL_TRACE_DATA_CHANNEL_T *p, UINT32 *lost, UINT32 *stamp, UINT32 *stampExt)
{
    if (stampExt != NULL)
    {
        UINT32 fn = *stampExt & 0x00ffffff;
        if (*stamp == -1U) // drop the first one
        {
            *stamp = fn;
        }
        else if (*stamp != fn || *lost > 64)
        {
            UINT32 *data = hal_TraceChannelRequest(p, SXS_TIME_STAMP_RMC, 8);
            if (data == NULL)
                return FALSE;

            UINT8 nlost = (*lost > 255) ? 255 : *lost;
            data[0] = (nlost << 24) | fn;
            data[1] = hal_TimGetUpTime();
            hal_TraceChannelRequestEnd(p);

            *lost = 0;
            *stamp = fn;
        }
    }

    return TRUE;
}

// =============================================================================
static UINT16 hal_TraceChannelFetch(HAL_TRACE_DATA_CHANNEL_T *p, UINT8 *data, UINT16 maxlen)
{
    if (p->wdoneIdx < p->ridx)
    {
        // |------W       R-----|
        if (p->wrapIdx <= p->ridx) // error case
        {
            p->ridx = p->ifcEndIdx = 0;
            return 0;
        }

        UINT32 len = p->wrapIdx - p->ridx;
        if (len > maxlen)
            len = maxlen;
        memcpy(data, p->buffer + p->ridx, len);
        p->rcount += len;
        p->ridx += len;
        if (p->ridx >= p->wrapIdx)
            p->ridx = 0;
        p->ifcEndIdx = p->ridx; // update manually
        return (UINT16)len;
    }
    else
    {
        // |      R-------W     |
        if (p->wdoneIdx == p->ridx)
            return 0;

        UINT32 len = p->wdoneIdx - p->ridx;
        if (len > maxlen)
            len = maxlen;
        memcpy(data, p->buffer + p->ridx, len);
        p->rcount += len;
        p->ridx += len;
        p->ifcEndIdx = p->ridx; // update manually
        return (UINT16)len;
    }
}

// =============================================================================
static VOID hal_TraceOpenDebugUart(VOID)
{
    // Uart already open
    HAL_ASSERT(!((hwp_debugUart->ctrl) & DEBUG_UART_ENABLE), "Host trace already open");

    hwp_debugUart->triggers = DEBUG_UART_RX_TRIGGER(7) |
                              DEBUG_UART_TX_TRIGGER(12) |
                              DEBUG_UART_AFC_LEVEL(10);

    // Enable IRQs:It is only enabled in the IRQ module as a wake-up IRQ, and not handled otherwise
    hwp_debugUart->irq_mask = DEBUG_UART_RX_DATA_AVAILABLE | DEBUG_UART_RX_TIMEOUT;

#if (CHIP_HAS_DBG_UART_SW_FLOW_CONTROL == 1)
    // FIXME configure rationaly the remaining fields
    // By default use host hard coded config, but !
    hwp_debugUart->ctrl = DEBUG_UART_ENABLE |
                          DEBUG_UART_DATA_BITS_8_BITS |
                          DEBUG_UART_TX_STOP_BITS_1_BIT |
                          DEBUG_UART_PARITY_ENABLE_NO |
                          DEBUG_UART_SWRX_FLOW_CTRL(1) |
                          DEBUG_UART_SWTX_FLOW_CTRL(1) |
                          DEBUG_UART_BACKSLASH_EN |
                          DEBUG_UART_DMA_MODE;
#else
    hwp_debugUart->ctrl = DEBUG_UART_ENABLE |
                          DEBUG_UART_DMA_MODE |
                          DEBUG_UART_DATA_BITS_8_BITS |
                          DEBUG_UART_AUTO_FLOW_CONTROL_ENABLE;
#endif
    while (hwp_debugUart->status & DEBUG_UART_ENABLE_N_FINISHED)
        ;
}

// =============================================================================
static VOID hal_TraceCloseDebugUart(VOID)
{
    //  Turn off the Uart. Other config bits should be kept, or debug
    //  host might encounter trouble when communicating with tools.
    hwp_debugUart->ctrl &= ~DEBUG_UART_ENABLE;
    hwp_debugUart->ctrl |= DEBUG_UART_RX_FIFO_RESET |
                           DEBUG_UART_TX_FIFO_RESET;
}

#if defined(HAL_TRACE_USE_ONERAM)
// =============================================================================
VOID hal_TraceOneOpen(UINT16 setting, UINT8 portIdx)
{
    HAL_TRACE_ONERAM_CTX_T *ctx = &g_halTraceOneCtx;

    if (!ctx->inited)
    {
        ctx->inited = TRUE;
        ctx->channel = HAL_UNKNOWN_CHANNEL;
        ctx->txEnabled = TRUE;
        ctx->rxEnabled = TRUE;
        ctx->data.buffer = (UINT8 *)g_halTraceRamBuffer;
        ctx->data.size = HAL_TRACE_RAM_BUFFER_SIZE;
    }

    if (ctx->opened)
        return;

    ctx->opened = TRUE;
    ctx->lost = 0;
    ctx->stamp = -1U;

    ctx->data.channel = HAL_UNKNOWN_CHANNEL;
    hal_TraceChannelReset(&ctx->data);

    hal_TraceOpenDebugUart();

    ctx->channel = hal_IfcGetChannel(HAL_IFC_DEBUG_UART_TX, HAL_IFC_SIZE_8_MODE_MANUAL);
    ctx->data.channel = ctx->channel;
}

// =============================================================================
VOID hal_TraceOneClose(UINT8 portIdx)
{
    HAL_TRACE_ONERAM_CTX_T *ctx = &g_halTraceOneCtx;

    if (!ctx->opened)
        return;

    ctx->opened = FALSE;

    //  Disable the Tx IFC Channel.
    if (ctx->channel != HAL_UNKNOWN_CHANNEL)
    {
        hal_IfcChannelRelease(HAL_IFC_DEBUG_UART_TX, ctx->channel);
    }

    ctx->channel = HAL_UNKNOWN_CHANNEL;
    ctx->data.channel = HAL_UNKNOWN_CHANNEL;
}

// =============================================================================
VOID hal_TraceOneSetFnStamp(UINT32 *stamp)
{
    g_halTraceOneCtx.stampExt = stamp;
}

// =============================================================================
UINT32 *hal_TraceOneRequestData(UINT8 id, UINT16 size)
{
    UINT32 sc = hal_SysEnterCriticalSection();

    HAL_TRACE_ONERAM_CTX_T *ctx = &g_halTraceOneCtx;
    HAL_TRACE_DATA_CHANNEL_T *p = &ctx->data;

    hal_TraceChannelUpdateIfc(p);
    if (!hal_TraceChannelInsertStamp(p, &ctx->lost, &ctx->stamp, ctx->stampExt))
        goto failed;

    UINT32 *data = hal_TraceChannelRequest(p, id, size);
    if (data == NULL)
        goto failed;

    hal_SysExitCriticalSection(sc);
    return data;

failed:
    ctx->lost++;
    hal_SysExitCriticalSection(sc);
    return NULL;
}

// =============================================================================
VOID hal_TraceOneDataFilled(VOID *data)
{
    UINT32 sc = hal_SysEnterCriticalSection();

    HAL_TRACE_ONERAM_CTX_T *ctx = &g_halTraceOneCtx;
    HAL_TRACE_DATA_CHANNEL_T *p = &ctx->data;

    hal_TraceChannelRequestEnd(p);
    hal_TraceChannelUpdateIfc(p);

    hal_SysExitCriticalSection(sc);
}

// =============================================================================
BOOL hal_TraceOneOutput(UINT8 id, UINT8 *data, UINT16 size)
{
    UINT32 sc = hal_SysEnterCriticalSection();

    UINT32 *buffer = hal_TraceOneRequestData(id, size);
    if (buffer == NULL)
        goto failed;

    if (size != 0)
        memcpy(buffer, data, size);
    hal_TraceOneDataFilled(buffer);

    hal_SysExitCriticalSection(sc);
    return TRUE;

failed:
    hal_SysExitCriticalSection(sc);
    return FALSE;
}

// =============================================================================
UINT16 hal_TraceOneFetch(UINT8 *data, UINT16 maxlen)
{
    UINT32 sc = hal_SysEnterCriticalSection();

    HAL_TRACE_ONERAM_CTX_T *ctx = &g_halTraceOneCtx;
    HAL_TRACE_DATA_CHANNEL_T *p = &ctx->data;
    UINT16 len = 0;

    if (ctx->txEnabled) // can't fetch when TX is enabled
        goto finished;

    len = hal_TraceChannelFetch(p, data, maxlen);

finished:
    hal_SysExitCriticalSection(sc);
    return len;
}

// =============================================================================
BOOL hal_TraceOneSetDdrEnabled(BOOL enabled)
{
    return TRUE;
}

// =============================================================================
BOOL hal_TraceOneIdleCheck(VOID)
{
    HAL_TRACE_ONERAM_CTX_T *ctx = &g_halTraceOneCtx;
    if (ctx->rxEnabled)
        hal_TraceProcessRx();

    UINT32 sc = hal_SysEnterCriticalSection();

    HAL_TRACE_DATA_CHANNEL_T *p = &ctx->data;
    hal_TraceChannelUpdateIfc(p);

    BOOL active = (ctx->txEnabled && !hal_TraceChannelIsEmpty(p));

    hal_SysExitCriticalSection(sc);
    return active;
}

// =============================================================================
BOOL hal_TraceOneTxIsEnabled(VOID)
{
    return g_halTraceOneCtx.txEnabled;
}

// =============================================================================
BOOL hal_TraceOneRxIsEnabled(VOID)
{
    return g_halTraceOneCtx.rxEnabled;
}

// =============================================================================
VOID hal_TraceOneSetTxEnabled(BOOL enabled)
{
    UINT32 sc = hal_SysEnterCriticalSection();

    HAL_TRACE_ONERAM_CTX_T *ctx = &g_halTraceOneCtx;

    if (ctx->txEnabled == enabled)
        goto finished;

    if (enabled)
    {
        ctx->data.channel = ctx->channel;
    }
    else
    {
        ctx->data.channel = HAL_UNKNOWN_CHANNEL;
    }
    ctx->txEnabled = enabled;

finished:
    hal_SysExitCriticalSection(sc);
}

// =============================================================================
VOID hal_TraceOneSetRxEnabled(BOOL enabled)
{
    g_halTraceOneCtx.rxEnabled = enabled;
}

// =============================================================================
BOOL hal_TraceOneTxFlush(UINT32 ticks)
{
    if (!g_halTraceOneCtx.txEnabled)
        return FALSE;

    UINT32 startTick = hal_TimGetUpTime();
    HAL_TRACE_DATA_CHANNEL_T *p = &g_halTraceOneCtx.data;
    do
    {
        hal_TraceChannelUpdateIfc(p);
        if (hal_TraceChannelIsEmpty(p))
            return TRUE;
    } while ((hal_TimGetUpTime() - startTick) < ticks);

    return FALSE;
}

UINT32 *hal_TraceOneGetBufferHead()
{
    UINT32 sc = hal_SysEnterCriticalSection();

    HAL_TRACE_ONERAM_CTX_T *ctx = &g_halTraceOneCtx;
    HAL_TRACE_DATA_CHANNEL_T *p = &ctx->data;

    UINT32 *data = p->buffer;

    hal_SysExitCriticalSection(sc);

    return data;
}
#endif // HAL_TRACE_USE_ONERAM

#if defined(HAL_TRACE_USE_DUALRAM)
// =============================================================================
VOID hal_TraceDualOpen(UINT16 setting, UINT8 portIdx)
{
    HAL_TRACE_DUALRAM_CTX_T *ctx = &g_halTraceDualCtx;

    if (!ctx->inited)
    {
        ctx->inited = TRUE;
        ctx->channel = HAL_UNKNOWN_CHANNEL;
        ctx->txEnabled = TRUE;
        ctx->rxEnabled = TRUE;
        ctx->data[HAL_TRACE_DATA_PSRAM].buffer = (UINT8 *)g_halTracePsramBuffer;
        ctx->data[HAL_TRACE_DATA_PSRAM].size = HAL_TRACE_PSRAM_BUFFER_SIZE;
        ctx->data[HAL_TRACE_DATA_ISRAM].buffer = (UINT8 *)g_halTraceIsramBuffer;
        ctx->data[HAL_TRACE_DATA_ISRAM].size = HAL_TRACE_ISRAM_BUFFER_SIZE;
    }

    if (ctx->opened)
        return;

    ctx->opened = TRUE;
    ctx->prefData = HAL_TRACE_DATA_PSRAM;
    ctx->currData = HAL_TRACE_DATA_PSRAM;
    ctx->lost = 0;
    ctx->stamp = -1U;

    for (int n = 0; n < HAL_TRACE_DATA_COUNT; n++)
    {
        ctx->data[n].channel = HAL_UNKNOWN_CHANNEL;
        hal_TraceChannelReset(&ctx->data[n]);
    }

    hal_TraceOpenDebugUart();

    ctx->channel = hal_IfcGetChannel(HAL_IFC_DEBUG_UART_TX, HAL_IFC_SIZE_8_MODE_MANUAL);
    ctx->data[ctx->currData].channel = ctx->channel;
}

// =============================================================================
VOID hal_TraceDualClose(UINT8 portIdx)
{
    HAL_TRACE_DUALRAM_CTX_T *ctx = &g_halTraceDualCtx;

    if (!ctx->opened)
        return;

    ctx->opened = FALSE;
    hal_TraceCloseDebugUart();

    //  Disable the Tx IFC Channel.
    if (ctx->channel != HAL_UNKNOWN_CHANNEL)
    {
        hal_IfcChannelRelease(HAL_IFC_DEBUG_UART_TX, ctx->channel);
    }

    ctx->channel = HAL_UNKNOWN_CHANNEL;
    for (int n = 0; n < HAL_TRACE_DATA_COUNT; n++)
    {
        ctx->data[n].channel = HAL_UNKNOWN_CHANNEL;
    }
}

// =============================================================================
VOID hal_TraceDualSetFnStamp(UINT32 *stamp)
{
    g_halTraceDualCtx.stampExt = stamp;
}

// =============================================================================
static void hal_TraceDualChannelSwitch(HAL_TRACE_DATA_CHANNEL_T *pfrom, HAL_TRACE_DATA_CHANNEL_T *pto)
{
    // Caller sould be checked that FROM is empty
    pto->channel = pfrom->channel;

    pfrom->channel = HAL_UNKNOWN_CHANNEL;
    pfrom->ridx = pfrom->widx = pfrom->wdoneIdx = pfrom->ifcEndIdx = 0;
    hal_TraceChannelStart(pto);
}

// =============================================================================
static void hal_TraceDualChannelMaintain(VOID)
{
    HAL_TRACE_DUALRAM_CTX_T *ctx = &g_halTraceDualCtx;
    HAL_TRACE_DATA_CHANNEL_T *p = &ctx->data[ctx->currData];

    hal_TraceChannelUpdateIfc(p);
    if (ctx->prefData != ctx->currData)
    {
        if (hal_TraceChannelIsEmpty(p))
        {
            HAL_TRACE_DATA_CHANNEL_T *ppref = &ctx->data[ctx->prefData];
            ctx->currData = ctx->prefData;
            hal_TraceDualChannelSwitch(p, ppref);
        }
    }
}

// =============================================================================
UINT32 *hal_TraceDualRequestData(UINT8 id, UINT16 size)
{
    UINT32 sc = hal_SysEnterCriticalSection();

    HAL_TRACE_DUALRAM_CTX_T *ctx = &g_halTraceDualCtx;

    hal_TraceDualChannelMaintain();
    HAL_TRACE_DATA_CHANNEL_T *p = &ctx->data[ctx->prefData];

    if (!hal_TraceChannelInsertStamp(p, &ctx->lost, &ctx->stamp, ctx->stampExt))
        goto failed;

    UINT32 *data = hal_TraceChannelRequest(p, id, size);
    if (data == NULL)
        goto failed;

    hal_SysExitCriticalSection(sc);
    return data;

failed:
    ctx->lost++;
    hal_SysExitCriticalSection(sc);
    return NULL;
}

// =============================================================================
VOID hal_TraceDualDataFilled(VOID *data)
{
    UINT32 sc = hal_SysEnterCriticalSection();

    HAL_TRACE_DUALRAM_CTX_T *ctx = &g_halTraceDualCtx;
    HAL_TRACE_DATA_CHANNEL_T *p;

    if ((UINT8 *)data >= ctx->data[0].buffer &&
        (UINT8 *)data < ctx->data[0].buffer + ctx->data[0].size)
        p = &ctx->data[0];
    else
        p = &ctx->data[1];

    hal_TraceChannelRequestEnd(p);
    hal_TraceDualChannelMaintain();

    hal_SysExitCriticalSection(sc);
}

// =============================================================================
BOOL hal_TraceDualOutput(UINT8 id, UINT8 *data, UINT16 size)
{
    UINT32 sc = hal_SysEnterCriticalSection();

    UINT32 *buffer = hal_TraceDualRequestData(id, size);
    if (buffer == NULL)
        goto failed;

    if (size != 0)
        memcpy(buffer, data, size);
    hal_TraceDualDataFilled(buffer);

    hal_SysExitCriticalSection(sc);
    return TRUE;

failed:
    hal_SysExitCriticalSection(sc);
    return FALSE;
}

// =============================================================================
UINT16 hal_TraceDualFetch(UINT8 *data, UINT16 maxlen)
{
    UINT32 sc = hal_SysEnterCriticalSection();

    HAL_TRACE_DUALRAM_CTX_T *ctx = &g_halTraceDualCtx;
    HAL_TRACE_DATA_CHANNEL_T *p = &ctx->data[ctx->currData];
    UINT16 len = 0;

    if (ctx->txEnabled) // can't fetch when TX is enabled
        goto finished;

    len = hal_TraceChannelFetch(p, data, maxlen);
    hal_TraceDualChannelMaintain(); // for channel switch

finished:
    hal_SysExitCriticalSection(sc);
    return len;
}

// =============================================================================
BOOL hal_TraceDualSetDdrEnabled(BOOL ddren)
{
    UINT32 sc = hal_SysEnterCriticalSection();
    HAL_TRACE_DUALRAM_CTX_T *ctx = &g_halTraceDualCtx;

    // This condition is more than needed.
    // Anyway, it is safe and doesn't hurt too much.
    if (!ddren && hwp_sysIfc->std_ch[ctx->channel].tc != 0)
        goto failed;

    HAL_TRACE_DATA_CHANNEL_T *psram = &ctx->data[HAL_TRACE_DATA_PSRAM];
    HAL_TRACE_DATA_CHANNEL_T *isram = &ctx->data[HAL_TRACE_DATA_ISRAM];
    if (ctx->txEnabled)
    {
        if (ddren)
        {
            // Only in this case, curr and pref may be different
            // Also, we can't return FALSE in this case.
            ctx->prefData = HAL_TRACE_DATA_PSRAM;
        }
        else
        {
            if (ctx->currData == HAL_TRACE_DATA_ISRAM)
            {
                // Already on ISRAM. Just reset PSRAM in case pref is set to PSRAM
                hal_TraceChannelReset(psram);
                ctx->prefData = HAL_TRACE_DATA_ISRAM;
            }
            else
            {
                // On PSRAM, and it is needed to wait PSRAM empty
                if (!hal_TraceChannelIsEmpty(psram))
                    goto failed;

                // It is needed to switch to ISRAM before return TRUE
                // Due to psram is checked, and switch can finish
                hal_TraceDualChannelSwitch(psram, isram);
                ctx->currData = ctx->prefData = HAL_TRACE_DATA_ISRAM;
            }
        }
    }
    else
    {
        ctx->prefData = ddren ? HAL_TRACE_DATA_PSRAM : HAL_TRACE_DATA_ISRAM;
        if (ctx->currData != ctx->prefData)
        {
            // When TX is disabled, waiting is meaningless.
            // More trace data than needed wil be drop, but it doesn't hurt too much
            hal_TraceChannelReset(psram);
            hal_TraceChannelReset(isram);
            ctx->currData = ctx->prefData;
        }
    }

passed:
    hal_SysExitCriticalSection(sc);
    return TRUE;

failed:
    hal_SysExitCriticalSection(sc);
    return FALSE;
}

// =============================================================================
BOOL hal_TraceDualIdleCheck(VOID)
{
    HAL_TRACE_DUALRAM_CTX_T *ctx = &g_halTraceDualCtx;
    if (ctx->rxEnabled)
        hal_TraceProcessRx();

    UINT32 sc = hal_SysEnterCriticalSection();

    hal_TraceDualChannelMaintain();

    HAL_TRACE_DATA_CHANNEL_T *psram = &ctx->data[HAL_TRACE_DATA_PSRAM];
    HAL_TRACE_DATA_CHANNEL_T *isram = &ctx->data[HAL_TRACE_DATA_ISRAM];
    BOOL active = (ctx->txEnabled && !(hal_TraceChannelIsEmpty(psram) && hal_TraceChannelIsEmpty(isram)));

    hal_SysExitCriticalSection(sc);
    return active;
}

// =============================================================================
BOOL hal_TraceDualTxIsEnabled(VOID)
{
    return g_halTraceDualCtx.txEnabled;
}

// =============================================================================
BOOL hal_TraceDualRxIsEnabled(VOID)
{
    return g_halTraceDualCtx.rxEnabled;
}

// =============================================================================
VOID hal_TraceDualSetTxEnabled(BOOL txen)
{
    UINT32 sc = hal_SysEnterCriticalSection();

    HAL_TRACE_DUALRAM_CTX_T *ctx = &g_halTraceDualCtx;
    if (ctx->txEnabled == txen)
        goto finished;

    if (txen)
    {
        ctx->data[ctx->currData].channel = ctx->channel;
    }
    else
    {
        ctx->data[0].channel = ctx->data[1].channel = HAL_UNKNOWN_CHANNEL;
    }

    ctx->txEnabled = txen;
    hal_TraceDualChannelMaintain();

finished:
    hal_SysExitCriticalSection(sc);
}

// =============================================================================
VOID hal_TraceDualSetRxEnabled(BOOL enabled)
{
    g_halTraceDualCtx.rxEnabled = enabled;
}

// =============================================================================
BOOL hal_TraceDualTxFlush(UINT32 ticks)
{
    // Not to check the real status when TX is diabled
    if (!g_halTraceDualCtx.txEnabled)
        return FALSE;

    UINT32 startTick = hal_TimGetUpTime();
    HAL_TRACE_DATA_CHANNEL_T *psram = &g_halTraceDualCtx.data[HAL_TRACE_DATA_PSRAM];
    HAL_TRACE_DATA_CHANNEL_T *isram = &g_halTraceDualCtx.data[HAL_TRACE_DATA_ISRAM];

    for (;;)
    {
        UINT32 sc = hal_SysEnterCriticalSection();
        hal_TraceDualChannelMaintain();
        if (hal_TraceChannelIsEmpty(psram) && hal_TraceChannelIsEmpty(isram))
        {
            hal_SysExitCriticalSection(sc);
            return TRUE;
        }
        hal_SysExitCriticalSection(sc);

        if ((hal_TimGetUpTime() - startTick) > ticks)
            return FALSE;
    }

    return FALSE; // not reachable
}

UINT32 *hal_TraceDualGetBufferHead()
{
    UINT32 sc = hal_SysEnterCriticalSection();

    HAL_TRACE_DUALRAM_CTX_T *ctx = &g_halTraceDualCtx;
    HAL_TRACE_DATA_CHANNEL_T *p = &ctx->data[ctx->prefData];

    UINT32 *data = p->buffer;
    hal_SysExitCriticalSection(sc);

    return data;
}
#endif // HAL_TRACE_USE_DUALRAM

// =============================================================================
// hal_TraceNone...
// =============================================================================
VOID hal_TraceNoneOpen(UINT16 setting, UINT8 portIdx) {}
VOID hal_TraceNoneClose(UINT8 portIdx) {}
UINT32 *hal_TraceNoneRequestData(UINT8 id, UINT16 size) { return NULL; }
VOID hal_TraceNoneDataFilled(VOID *data) {}
BOOL hal_TraceNoneOutput(UINT8 id, UINT8 *data, UINT16 size) { return FALSE; }
UINT16 hal_TraceNoneFetch(UINT8 *data, UINT16 maxlen) { return 0; }
VOID hal_TraceNoneSetFnStamp(UINT32 *stamp) {}
VOID hal_TraceNoneSetTxEnabled(BOOL enabled) {}
BOOL hal_TraceNoneTxIsEnabled(VOID) { return FALSE; }
VOID hal_TraceNoneSetRxEnabled(BOOL enabled) {}
BOOL hal_TraceNoneRxIsEnabled(VOID) { return FALSE; }
BOOL hal_TraceNoneTxFlush(UINT32 ticks) { return TRUE; }
BOOL hal_TraceNoneIdleCheck(VOID) {}
BOOL hal_TraceNoneSetDdrEnabled(BOOL enabled) { return TRUE; }
UINT32 *hal_TraceNoneGetBufferHead() { return NULL; }

// =============================================================================
// Use function alias to provide stable API, and avoid too many functions
// with the same name (even embraced with macro)
// =============================================================================
VOID hal_HstTraceOpen(UINT16 setting, UINT8 portIdx) HAL_TRACE_API_ALIAS(Open);
VOID hal_HstTraceClose(UINT8 portIdx) HAL_TRACE_API_ALIAS(Close);
UINT32 *hal_HstTraceRequestData(UINT8 id, UINT16 size) HAL_TRACE_API_ALIAS(RequestData);
VOID hal_HstTraceDataFilled(VOID *data) HAL_TRACE_API_ALIAS(DataFilled);
BOOL hal_HstTraceOutput(UINT8 id, UINT8 *data, UINT16 size) HAL_TRACE_API_ALIAS(Output);
UINT16 hal_HstTraceFetch(UINT8 *data, UINT16 maxlen) HAL_TRACE_API_ALIAS(Fetch);
VOID hal_HstTraceSetFnStamp(UINT32 *stamp) HAL_TRACE_API_ALIAS(SetFnStamp);
VOID hal_HstTraceSetTxEnabled(BOOL enabled) HAL_TRACE_API_ALIAS(SetTxEnabled);
BOOL hal_HstTraceTxIsEnabled(VOID) HAL_TRACE_API_ALIAS(TxIsEnabled);
VOID hal_HstTraceSetRxEnabled(BOOL enabled) HAL_TRACE_API_ALIAS(SetRxEnabled);
BOOL hal_HstTraceRxIsEnabled(VOID) HAL_TRACE_API_ALIAS(RxIsEnabled);
BOOL hal_HstTraceTxFlush(UINT32 ticks) HAL_TRACE_API_ALIAS(TxFlush);
BOOL hal_HstTraceIdleCheck(VOID) HAL_TRACE_API_ALIAS(IdleCheck);
BOOL hal_HstTraceSetDdrEnabled(BOOL enabled) HAL_TRACE_API_ALIAS(SetDdrEnabled);
UINT32 *hal_HstTraceGetBufferHead() HAL_TRACE_API_ALIAS(GetBufferHead);
