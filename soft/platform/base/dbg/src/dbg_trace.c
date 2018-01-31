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

#include "stdarg.h"
#include "string.h"
#include "stdio.h"

#include "sxs_io.h"
#include "sxs_srl.h"

#include "dbg.h"

#define DBG_TRACE_MAX_LEVEL  16

PRIVATE UINT8 g_dbgTraceStart = 0;

// =============================================================================
// dbg_TraceOpen
// -----------------------------------------------------------------------------
/// Open Trace Module
// =============================================================================
DBG_ERR_T dbg_TraceOpen(BOOL clearLevels)
{
    g_dbgTraceStart       = 1;
    if (clearLevels)
    {
        sxs_IoCtx.TraceBitMap [TGET_ID(_MMI)] = 0;
    }
    return DBG_ERR_NO;
}

// =============================================================================
// dbg_TraceClose
// -----------------------------------------------------------------------------
// Close Trace Module
// =============================================================================
DBG_ERR_T dbg_TraceClose()
{
    g_dbgTraceStart       = 0;
    return DBG_ERR_NO;
}

// =============================================================================
// dbg_TraceSetOutputMask
// -----------------------------------------------------------------------------
// Set enabled trace level
// =============================================================================
DBG_ERR_T dbg_TraceSetOutputMask(UINT16 nIndex, BOOL bMode)
{
    if(nIndex >= DBG_TRACE_MAX_LEVEL)
        return DBG_ERR_UNSUPPORTED;

    if (bMode)
    {
        sxs_IoCtx.TraceBitMap [TGET_ID(_MMI)] |= (1<<nIndex);
    }
    else
    {
        sxs_IoCtx.TraceBitMap [TGET_ID(_MMI)] &= ~(1<<nIndex);
    }

    return DBG_ERR_NO;
}


// =============================================================================
// dbg_TraceVariadicOutputText
// -----------------------------------------------------------------------------
// Output a trace, variadic format.
// =============================================================================
PUBLIC DBG_ERR_T dbg_TraceVariadicOutputText(UINT16 nIndex, PCSTR fmt, va_list ap)
{
#ifndef SXS_NO_PRINTF
    if(g_dbgTraceStart == 0)
    {
        return DBG_ERR_UNSUPPORTED;
    }

    if(nIndex >= DBG_TRACE_MAX_LEVEL)
    {
        return DBG_ERR_UNSUPPORTED;
    }

    CHAR uartBuf[256];

    vsnprintf(uartBuf, sizeof(uartBuf), fmt, ap);

    if (nIndex == 0x00)
    {
        // Forced trace
        SXS_TRACE(_MMI | TSTDOUT, uartBuf);
    }
    else
    {
        // Regular trace
        SXS_TRACE(_MMI | TLEVEL(nIndex), uartBuf);
    }
#endif
    return DBG_ERR_NO;
}

// =============================================================================
// dbg_TraceOutputText
// -----------------------------------------------------------------------------
// Output a trace
// =============================================================================
#ifndef BT_NO_PRINTF
PUBLIC DBG_ERR_T BT_TraceOutputText(UINT16 nIndex, PCSTR fmt, ...)
{
    DBG_ERR_T returnStatus;
    va_list ap;

    va_start(ap, fmt);
    returnStatus = dbg_TraceVariadicOutputText(nIndex, fmt, ap);
    va_end(ap);

    return returnStatus;
}
#endif

PUBLIC DBG_ERR_T dbg_TraceOutputText(UINT16 nIndex, PCSTR fmt, ...)
{
    DBG_ERR_T returnStatus;
#ifndef SXS_NO_PRINTF
    va_list ap;

    va_start(ap, fmt);
    returnStatus = dbg_TraceVariadicOutputText(nIndex, fmt, ap);
    va_end(ap);
#endif
    return returnStatus;
}
