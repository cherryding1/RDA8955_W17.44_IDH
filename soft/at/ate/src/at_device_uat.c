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

#ifdef AT_USE_USB_TRAS_DATA

// #define LOCAL_LOG_LEVEL LOG_LEVEL_VERBOSE

#include "at_device_uat.h"
#include "at_dispatch.h"
#include "at_utils.h"
#include "cos.h"
#include "drv.h"
#include <stdlib.h>

#define UAT_RX_BUF_SIZE (4 * 1024)
#define UAT_TX_BUF_SIZE (4 * 1024)

typedef struct
{
    AT_DEVICE_T ops;     // API
    DRV_UAT_T drv;
} AT_DEVICE_UAT_T;

// =============================================================================
// at_UatDestroy
// =============================================================================
static void at_UatDestroy(AT_DEVICE_T *th)
{
    AT_DEVICE_UAT_T *uat = (AT_DEVICE_UAT_T *)th;

    if (uat == NULL)
        return;

    ATUATLOG(I, "AT UAT destroy");
    DRV_UatClose(&uat->drv);
    at_free(uat);
}

// =============================================================================
// at_UatWrite
// =============================================================================
static int at_UatWrite(AT_DEVICE_T *th, const uint8_t *data, unsigned length)
{
    AT_DEVICE_UAT_T *uat = (AT_DEVICE_UAT_T *)th;

    if (data == NULL || length == 0)
        return 0;

    ATUATLOG(I, "AT UAT write: len=%d", length);
    int trans = DRV_UatWrite(&uat->drv, data, length);
    if (trans < length)
    {
        ATUATLOG(E, "AT UAT output overflow, drop %d bytes", length - trans);
    }
    return trans;
}

// =============================================================================
// at_UatCallback
// =============================================================================
static void at_UatCallback(DRV_UAT_T *drv, void *param, DRV_UAT_EVENT_T evt)
{
    AT_DEVICE_UAT_T *uat = (AT_DEVICE_UAT_T *)param;

    ATUARTLOG(D, "AT UAT event=0x%x", evt);
    if (evt & DRV_UAT_RX_ARRIVED)
        at_DeviceScheduleRead((AT_DEVICE_T *)uat);
}

// =============================================================================
// at_UatRead
// =============================================================================
static int at_UatRead(AT_DEVICE_T *th, uint8_t *data, unsigned size)
{
    AT_DEVICE_UAT_T *uat = (AT_DEVICE_UAT_T *)th;
    if (data == NULL || size == 0)
        return 0;

    return DRV_UatRead(&uat->drv, data, size);
}

// =============================================================================
// at_UatOpen
// =============================================================================
static bool at_UatOpen(AT_DEVICE_T *th)
{
    AT_DEVICE_UAT_T *uat = (AT_DEVICE_UAT_T *)th;

    DRV_UatOpen(&uat->drv);
    return true;
}

// =============================================================================
// at_UatClose
// =============================================================================
static void at_UatClose(AT_DEVICE_T *th)
{
    AT_DEVICE_UAT_T *uat = (AT_DEVICE_UAT_T *)th;

    DRV_UatClose(&uat->drv);
}

// =============================================================================
// at_UatCreate
// =============================================================================
AT_DEVICE_T *at_UatCreate(uint8_t devno)
{
    ATUATLOG(I, "AT UAT create");

    // assert(cfg != NULL)
    unsigned memsize = sizeof(AT_DEVICE_UAT_T) + UAT_RX_BUF_SIZE + UAT_TX_BUF_SIZE;
    AT_DEVICE_UAT_T *uat = (AT_DEVICE_UAT_T *)at_malloc(memsize);
    memset(uat, 0, sizeof(*uat));

    uint32_t rxBuf = (uint32_t)uat + sizeof(*uat);
    uint32_t txBuf = rxBuf + UAT_RX_BUF_SIZE;

    DRV_UAT_CFG_T drvcfg;

    drvcfg.rxBuf = (uint8_t *)rxBuf;
    drvcfg.txBuf = (uint8_t *)txBuf;
    drvcfg.rxBufSize = UAT_RX_BUF_SIZE;
    drvcfg.txBufSize = UAT_TX_BUF_SIZE;
    drvcfg.evtMask = DRV_UAT_RX_ARRIVED;
    drvcfg.callback = at_UatCallback;
    drvcfg.callbackParam = uat;
    DRV_UatInit(&uat->drv, &drvcfg);

    uat->ops.devno = devno;
    uat->ops.destroy = at_UatDestroy;
    uat->ops.open = at_UatOpen;
    uat->ops.close = at_UatClose;
    uat->ops.write = at_UatWrite;
    uat->ops.read = at_UatRead;

    return (AT_DEVICE_T *)uat;
}

#endif // AT_USE_USB_TRAS_DATA
