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
#include "uat_m.h"

static DRV_UAT_T *gDrvUat; // it is a pity that lower level can't support multi-instance

static void DRV_UatSendData(DRV_UAT_T *uat)
{
    if (uat == NULL)
        return;

    uint32_t sc = hal_SysEnterCriticalSection();
    if (!usb_at_is_ready() || !uat_get_send_state())
        goto unlock_exit;

    uint32_t payload = DRV_CBufPayloadSize(&uat->txcb);
    if (payload == 0)
        goto unlock_exit;

    if (payload > UAT_TX_EP_BUF_SIZE)
        payload = UAT_TX_EP_BUF_SIZE;
    DRV_CBufRead(&uat->txcb, uat->usbSendBuf, payload);
    uat_set_send_state(false);
    uat_send_data(payload, 0);

unlock_exit:
    hal_SysExitCriticalSection(sc);
}

static void DRV_UatCallback(uint8_t direction, uint16_t size)
{
    DRV_UAT_T *uat = gDrvUat;
    if (uat == NULL)
        return;

    DRVLOGD("UAT callback direction=%d size=%d", direction, size);

    uint32_t evt = 0;
    if (direction == UAT_DATA_DIR_IN)
    {
        uint32_t sc = hal_SysEnterCriticalSection();
        unsigned trans = 0;
        if (size > 0)
        {
            trans = DRV_CBufWrite(&uat->rxcb, uat->usbRecvBuf, size);
            if (trans < size)
            {
                DRVLOGE("UAT input overflow, input=%d saved=%d", size, trans);
                evt |= DRV_UAT_RX_OVERFLOW;
            }
            evt |= DRV_UAT_RX_ARRIVED;
        }
        hal_SysExitCriticalSection(sc);

        uat_recv_data(UAT_RX_EP_BUF_SIZE, 0);
    }
    else if (direction == UAT_DATA_DIR_OUT)
    {
        DRV_UatSendData(uat);
    }

    evt &= uat->cfg.evtMask;
    if (evt != 0 && uat->cfg.callback != NULL)
        uat->cfg.callback(uat, uat->cfg.callbackParam, evt);
}

void DRV_UatInit(DRV_UAT_T *uat, DRV_UAT_CFG_T *cfg)
{
    uat->cfg = *cfg;
    DRV_CBufInit(&uat->rxcb, cfg->rxBuf, cfg->rxBufSize);
    DRV_CBufInit(&uat->txcb, cfg->txBuf, cfg->txBufSize);
}

bool DRV_UatOpen(DRV_UAT_T *uat)
{
    if (gDrvUat != NULL)
        return false;
    uat->usbRecvBuf = (uint8_t *)uat_transGetBufferAdd(1);
    uat->usbSendBuf = (uint8_t *)uat_transGetBufferAdd(0);
    uat_transSetCallback(DRV_UatCallback);
    gDrvUat = uat;
    return true;
}

void DRV_UatClose(DRV_UAT_T *uat)
{
    gDrvUat = NULL;
    uat_transSetCallback(NULL);
}

int DRV_UatWrite(DRV_UAT_T *uat, const uint8_t *data, unsigned size)
{
    if (uat == NULL || data == NULL)
        return -1;
    if (size == 0)
        return 0;

    uint32_t sc = hal_SysEnterCriticalSection();
    unsigned trans = DRV_CBufWrite(&uat->txcb, data, size);
    hal_SysExitCriticalSection(sc);

    DRV_UatSendData(uat);
    DRVLOGV("UAT write size=%d written=%d", size, trans);
    return trans;
}

int DRV_UatRead(DRV_UAT_T *uat, uint8_t *data, unsigned size)
{
    if (uat == NULL || data == NULL)
        return -1;
    if (size == 0)
        return 0;

    uint32_t sc = hal_SysEnterCriticalSection();
    unsigned trans = DRV_CBufRead(&uat->rxcb, data, size);
    hal_SysExitCriticalSection(sc);

    DRVLOGV("UAT read size=%d read=%d", size, trans);
    return trans;
}
