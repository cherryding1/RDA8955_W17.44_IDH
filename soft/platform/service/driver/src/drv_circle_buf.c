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

#include "drv.h"
#include <string.h>

void DRV_CBufInit(DRV_CIRCLE_BUF_T *cb, uint8_t *buf, unsigned size)
{
    cb->buf = buf;
    cb->size = size;
    cb->rd = 0;
    cb->payload = 0;
}

void DRV_CBufFlush(DRV_CIRCLE_BUF_T *cb)
{
    cb->rd = 0;
    cb->payload = 0;
}

bool DRV_CBufIsEmpty(DRV_CIRCLE_BUF_T *cb)
{
    return (cb->payload == 0);
}

bool DRV_CBufIsFull(DRV_CIRCLE_BUF_T *cb)
{
    return (cb->payload >= cb->size);
}

unsigned DRV_CBufPayloadSize(DRV_CIRCLE_BUF_T *cb)
{
    return cb->payload;
}

unsigned DRV_CBufRemainSize(DRV_CIRCLE_BUF_T *cb)
{
    return cb->size - cb->payload;
}

static unsigned DRV_CBufWriteIndex(DRV_CIRCLE_BUF_T *cb)
{
    unsigned wr = cb->rd + cb->payload;
    if (wr >= cb->size)
        wr -= cb->size;
    return wr;
}

uint8_t *DRV_CBufLinearRead(DRV_CIRCLE_BUF_T *cb, unsigned *size)
{
    if (size != NULL)
    {
        *size = cb->payload;
        if (cb->rd + *size > cb->size)
            *size = cb->size - cb->rd;
    }
    return cb->buf + cb->rd;
}

void DRV_CBufSkipBytes(DRV_CIRCLE_BUF_T *cb, unsigned size)
{
    cb->payload -= size;
    cb->rd += size;
    if (cb->rd >= cb->size)
        cb->rd -= cb->size;
}

unsigned DRV_CBufWrite(DRV_CIRCLE_BUF_T *cb, const uint8_t *data, unsigned size)
{
    unsigned room = cb->size - cb->payload;
    if (size > room)
        size = room;
    if (size == 0)
        return 0;

    unsigned wr = DRV_CBufWriteIndex(cb);
    unsigned toend = cb->size - wr;
    if (size <= toend)
    {
        memcpy(cb->buf + wr, data, size);
    }
    else
    {
        memcpy(cb->buf + wr, data, toend);
        memcpy(cb->buf, data + toend, size - toend);
    }
    cb->payload += size;
    return size;
}

unsigned DRV_CBufRead(DRV_CIRCLE_BUF_T *cb, uint8_t *data, unsigned size)
{
    if (size > cb->payload)
        size = cb->payload;
    if (size == 0)
        return 0;

    unsigned toend = cb->size - cb->rd;
    if (size <= toend)
    {
        memcpy(data, cb->buf + cb->rd, size);
    }
    else
    {
        memcpy(data, cb->buf + cb->rd, toend);
        memcpy(data + toend, cb->buf, size - toend);
    }
    cb->payload -= size;
    cb->rd += size;
    if (cb->rd >= cb->size)
        cb->rd -= cb->size;
    return size;
}
