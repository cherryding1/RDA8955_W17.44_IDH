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

#include "at_data_engine.h"
#include "at_dispatch.h"
#include "at_device.h"
#include "at_utils.h"
#include <stdlib.h>

extern INT32 CFW_PppSendData(UINT8 *pData, UINT16 nDataSize, UINT8 MuxId); // aps_main.c

#define SYNC_FLAG 0x7e
#define MIN_FRAME_SIZE 5          // flag, address, control, protocol, ... , flag
#define DATA_BUFF_SIZE (3 * 1024) // at least 2 MTU

enum DATA_MODE
{
    DATA_MODE_PPP,
    DATA_MODE_BYPASS
};

struct AT_DATA_ENGINE_T
{
    AT_DISPATCH_T *dispatch; // upstream dispatch
    int channelId;           // AT channel ID

    enum DATA_MODE mode;
    uint8_t *buff; // buffer, shared by all mode
    int buffLen;   // existed buffer data byte count
    int buffSize;  // buffer size
    int dropCount; // drop byte count

    AT_DATA_BYPASS_CALLBACK_T bypassCallback;
    void *bypassParam;
};

// =============================================================================
// Tiny helper to push a byte to buffer
// =============================================================================
static void at_DataPushByte(AT_DATA_ENGINE_T *th, uint8_t c)
{
    // buffer overflow should be checked before call this.
    th->buff[th->buffLen++] = c;
}

// =============================================================================
// Requested fixed length data is in buffer, and should send to downstream
// =============================================================================
static void at_DataDataFinished(AT_DATA_ENGINE_T *th)
{
    if (th->buffLen == 0)
        return;
#ifdef CFW_PPP_SUPPORT
    ATDATALOG(I, "AT DATA%d packet: len=%d", th->channelId, th->buffLen);
    CFW_PppSendData(th->buff, th->buffLen, th->channelId);
#endif
    th->buffLen = 0;
}

// =============================================================================
// at_DataEngineCreate
// =============================================================================
AT_DATA_ENGINE_T *at_DataEngineCreate(AT_DISPATCH_T *dispatch)
{
    AT_DATA_ENGINE_T *th = (AT_DATA_ENGINE_T *)at_malloc(sizeof(*th));

    th->dispatch = dispatch;
    th->channelId = at_DispatchGetChannel(dispatch);
    th->mode = DATA_MODE_PPP;
    th->buffSize = DATA_BUFF_SIZE;
    th->buff = (uint8_t *)at_malloc(th->buffSize);
    th->buffLen = 0;
    th->dropCount = 0;
    th->bypassCallback = NULL;
    th->bypassParam = NULL;
    return th;
}

// =============================================================================
// at_DataEngineDestroy
// =============================================================================
void at_DataEngineDestroy(AT_DATA_ENGINE_T *th)
{
    if (th == NULL)
        return;

    at_free(th->buff);
    at_free(th);
}

// =============================================================================
// at_DataPushData
// =============================================================================
int at_DataPushData(AT_DATA_ENGINE_T *th, const uint8_t *data, unsigned length)
{
    if (th->mode == DATA_MODE_BYPASS)
        return th->bypassCallback(th->bypassParam, data, length);

    unsigned total = length;
    while (length > 0)
    {
        uint8_t c = *data++;
        length--;

        ATDATALOG(V, "AT DATA%d byte %02x", th->channelId, c);

        // Drop whole buffer at overflow, maybe caused by data error
        // or buffer too small
        if (th->buffLen >= th->buffSize)
        {
            th->dropCount += th->buffLen + 1;
            th->buffLen = 0;
        }

        if (c == SYNC_FLAG)
        {
            at_DataPushByte(th, c);
            if (th->buffLen == 1)
            {
                ; // that is fine
            }
            else if (th->buffLen >= MIN_FRAME_SIZE)
            {
                if (th->dropCount > 0)
                    ATDATALOG(W, "AT DATA%d dropped count=%d", th->channelId, th->dropCount);
                th->dropCount = 0;
                at_DataDataFinished(th);
                at_DeviceScheduleRead(at_DispatchGetDevice(th->dispatch));
                return total - length - 1; // leave the last SYNC_FLAG
            }
            else
            {
                th->buffLen = 0;
                at_DataPushByte(th, c);
            }
        }
        else if (th->buffLen > 0) // Only accept data when SYNC_FLAG is found
        {
            at_DataPushByte(th, c);
        }
        else
        {
            th->dropCount++;
        }
    }
    return total;
}

// =============================================================================
// at_DataWrite
// =============================================================================
void at_DataWrite(AT_DATA_ENGINE_T *th, const uint8_t *data, unsigned length)
{
    if (th->dispatch != NULL && at_DispatchIsDataMode(th->dispatch))
        at_DispatchWrite(th->dispatch, data, length);
}

// =============================================================================
// at_DataChannelWrite
// =============================================================================
void at_DataChannelWrite(const uint8_t *data, unsigned length, int channel)
{
    AT_DISPATCH_T *dispatch = at_DispatchGetByChannel(channel);
    if (dispatch != NULL && at_DispatchIsDataMode(dispatch))
        at_DispatchWrite(dispatch, data, length);
}

// =============================================================================
// at_DataSetPPPMode
// =============================================================================
void at_DataSetPPPMode(AT_DATA_ENGINE_T *th)
{
    ATDATALOG(I, "AT DATA%d set PPP mode", th->channelId);
    th->mode = DATA_MODE_PPP;
}

// =============================================================================
// at_DataSetBypassMode
// =============================================================================
void at_DataSetBypassMode(AT_DATA_ENGINE_T *th, AT_DATA_BYPASS_CALLBACK_T callback, void *param)
{
    ATDATALOG(I, "AT DATA%d set bypass mode, callback=0x%08x/0x%08x", th->channelId, callback, param);
    th->mode = DATA_MODE_BYPASS;
    th->bypassCallback = callback;
    th->bypassParam = param;
}

bool at_DataIsPPPMode(AT_DATA_ENGINE_T *th) { return th->mode == DATA_MODE_PPP; }
bool at_DataIsBypassMode(AT_DATA_ENGINE_T *th) { return th->mode == DATA_MODE_BYPASS; }
