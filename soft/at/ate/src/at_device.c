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

#include "at_device.h"
#include "at_dispatch.h"
#include "at_utils.h"

void at_DeviceDestroy(AT_DEVICE_T *th)
{
    if (th != NULL && th->destroy != NULL)
        th->destroy(th);
}

bool at_DeviceOpen(AT_DEVICE_T *th)
{
    if (th->open != NULL)
        return th->open(th);
    return false;
}

void at_DeviceClose(AT_DEVICE_T *th)
{
    if (th->close != NULL)
        th->close(th);
}

int at_DeviceWrite(AT_DEVICE_T *th, const uint8_t *data, unsigned length)
{
    if (th->write != NULL)
        return th->write(th, data, length);
    return 0;
}

int at_DeviceRead(AT_DEVICE_T *th, uint8_t *data, unsigned size)
{
    if (th->read != NULL)
        return th->read(th, data, size);
    return 0;
}

void at_DeviceSleep(AT_DEVICE_T *th, bool wakeupEnabled)
{
    if (th->sleep != NULL)
        th->sleep(th, wakeupEnabled);
}

void at_DeviceWakeup(AT_DEVICE_T *th)
{
    if (th->wakeup != NULL)
        th->wakeup(th);
}

bool at_DeviceIsBaudSupported(AT_DEVICE_T *th, int baud)
{
    if (th->isBaudSupported != NULL)
        return th->isBaudSupported(th, baud);
    return true;
}

bool at_DeviceSetFlowCtrl(AT_DEVICE_T *th, uint8_t rxfc, uint8_t txfc)
{
    if (th->setFlowCtrl != NULL)
        return th->setFlowCtrl(th, rxfc, txfc);
    return false;
}

void at_DeviceSetFormat(AT_DEVICE_T *th, int baud, AT_DEVICE_FORMAT_T format, AT_DEVICE_PARITY_T parity)
{
    if (th->setFormat != NULL)
        th->setFormat(th, baud, format, parity);
}

AT_DEVICE_T *at_GetDeviceByChannel(int channel)
{
    return at_DispatchGetDevice(at_DispatchGetByChannel(channel));
}

void at_DeviceScheduleRead(AT_DEVICE_T *th)
{
    if (th->recv == NULL)
        return;

    at_TaskCallbackNotif((COS_CALLBACK_FUNC_T)at_DispatchRead, th->recv);
}
