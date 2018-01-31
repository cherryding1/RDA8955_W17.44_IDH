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

#ifndef _HAL_TRACE_P_H_
#define _HAL_TRACE_P_H_

#include "cs_types.h"
#include "chip_id.h"

#include "global_macros.h"
#include "debug_uart.h"
#include "debug_host.h"
#include "sys_irq.h"
#include "sys_ifc.h"

#include "hal_trace.h"
#include "hal_host.h"
#include "hal_map.h"

#include "sxs_srl.h"
#include "sxs_rmt.h"
#include "sxs_rmc.h"
#include "sxs_idle.h"

// =============================================================================
// HAL_TRACE_DATA_CHANNEL_T
// =============================================================================
typedef struct
{
    UINT8 channel;    // IFC channel
    UINT8 numPendReq; // number of requested but not completed write
    UINT32 ridx;      // index of IFC finished (for check request end)
    UINT32 widx;      // index for next request start. It can't be full
    UINT32 wdoneIdx;  // index for request-filled index (for check IFC end)
    UINT32 wrapIdx;   // the end index (for early wrap around)
    UINT32 ifcEndIdx; // end index sent to IFC. It wil be used with tc to update ridx
    UINT32 rcount;    // total IFC finished count, just for debug
    UINT32 wcount;    // total requested count, just for debug
    UINT32 size;      // buffer size in byte
    UINT8 *buffer;    // THE buffer
} HAL_TRACE_DATA_CHANNEL_T;

// =============================================================================
// HAL_TRACE_ONERAM_CTX_T
// =============================================================================
typedef struct
{
    BOOL inited;
    BOOL opened;
    BOOL txEnabled;
    BOOL rxEnabled;
    UINT8 channel;
    UINT32 lost;
    UINT32 stamp;
    UINT32 *stampExt;
    HAL_TRACE_DATA_CHANNEL_T data;
} HAL_TRACE_ONERAM_CTX_T;

// =============================================================================
// HAL_TRACE_DUALRAM_CTX_T
// =============================================================================
enum
{
    HAL_TRACE_DATA_PSRAM,
    HAL_TRACE_DATA_ISRAM,
    HAL_TRACE_DATA_COUNT
};

typedef struct
{
    BOOL inited;      // avoid double initialization
    BOOL opened;      // avoid double open
    BOOL txEnabled;   // whether DebugHost TX is enabled
    BOOL rxEnabled;   // whether DebugHost RX is enabled
    UINT8 channel;    // IFC channel
    UINT8 prefData;   // prefered data. The following data will be written here.
    UINT8 currData;   // current data. IFC is working here.
    UINT32 lost;      // lost count
    UINT32 stamp;     // last stamp
    UINT32 *stampExt; // external pointer for stamp
    HAL_TRACE_DATA_CHANNEL_T data[HAL_TRACE_DATA_COUNT];
} HAL_TRACE_DUALRAM_CTX_T;

// =============================================================================
// HAL_TRACE_RX_CTX_T
// =============================================================================
#define HAL_TRACE_RX_BUFFER_SIZE (64)

typedef struct
{
    UINT8 buffer[HAL_TRACE_RX_BUFFER_SIZE];
    UINT8 len;
    UINT8 frameLen;
    UINT8 crc;
    HAL_TRACE_RX_PROC_T proc;
} HAL_TRACE_RX_CTX_T;

#endif
