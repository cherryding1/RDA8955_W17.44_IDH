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

// #define LOCAL_LOG_LEVEL LOG_LEVEL_VERBOSE

#include "at_dispatch.h"
#include "at_device.h"
#include "at_utils.h"
#include "at_cmd_engine.h"
#include "at_data_engine.h"
#include "at_cmux_engine.h"
#include "at_module.h"
#include "hal_timers.h"
#include "cos.h"

#define FETCH_BUF_SIZE (32)       // Size for each read
#define TPLUS_LEADING_TICK (1000) // 1s
#define TPLUS_DURATION_TICK (500) // 0.5s
#define TPLUS_TRAILING_TICK (500) // 0.5s

#ifdef AT_CMUX_SUPPORT
#define CMUX_PARENT(th) th->cmuxParent
#else
#define CMUX_PARENT(th) NULL
#endif

enum DISPATCH_MODE
{
    DISPATCH_MODE_CMD,
    DISPATCH_MODE_DATA,
    DISPATCH_MODE_CMUX
};

struct AT_DISPATCH_T
{
    int channelId;
    enum DISPATCH_MODE mode;
    uint8_t *buff;
    int buffLen;
    int buffSize;
    AT_DEVICE_T *device;
    AT_CMD_ENGINE_T *cmdEngine;
    AT_DATA_ENGINE_T *dataEngine;
    AT_CMUX_ENGINE_T *cmuxEngine;

    unsigned prevRecvTick;
    unsigned tplusStartTick;
    int tplusCount;

#ifdef AT_CMUX_SUPPORT
    AT_CMUX_ENGINE_T *cmuxParent;
#endif
    int cmuxDlci;

    struct AT_DISPATCH_T *next;
};

static AT_DISPATCH_T *gAtDispatchHead = NULL;

// +++ (tplus) detection
//
// tplus is used to escape from data mode to command line mode. A valid tplus:
// * distance from previous input > 1.0s
// * distance among the three plus < 0.5s
// * distance from next input > 0.5s
//
// tplus detection should be located in dipatch rather than data engine,
// because dispatch can calculate time better. Due to the cache in dispatch,
// downstream is much harder to know the arrive time.

// channel ID
//
// Multiple dispatch support in ATS uses DLCI to distinguish channels. To
// integrate with existed ATS, each dispatch channel will be associated with
// an ID. And this ID is only used in ATS, and ATE shall use object pointer.

// =============================================================================
// Register AT dispatch, and bind to a channel ID.
// =============================================================================
static void at_DispatchRegisterChannel(AT_DISPATCH_T *dispatch)
{
    if (gAtDispatchHead == NULL)
    {
        gAtDispatchHead = dispatch;
        return;
    }

    AT_DISPATCH_T *node = gAtDispatchHead;
    while (node->next != NULL)
        node = node->next;

    node->next = dispatch;
}

// =============================================================================
// Unregister AT dispatch, and unbind to the channel ID.
// =============================================================================
static void at_DispatchUnregisterChannel(AT_DISPATCH_T *dispatch)
{
    if (dispatch == gAtDispatchHead)
    {
        gAtDispatchHead = gAtDispatchHead->next;
        return;
    }

    AT_DISPATCH_T *node = gAtDispatchHead;
    for (;;)
    {
        if (node == NULL)
        {
            ATDISPLOG(E, "AT dispatch unregister found fail");
            return;
        }
        if (node->next == dispatch)
        {
            node->next = dispatch->next;
            return;
        }
        node = node->next;
    }
}

// =============================================================================
// at_DispatchGetByChannel
// =============================================================================
AT_DISPATCH_T *at_DispatchGetByChannel(int channel)
{
    AT_DISPATCH_T *node = gAtDispatchHead;
    while (node != NULL)
    {
        if (node->channelId == channel)
            return node;
        node = node->next;
    }

    ATDISPLOG(E, "AT dispatch invalid channel id=%d", channel);
    return gAtDispatchHead;
}

// =============================================================================
// at_DispatchFindByChannel
// =============================================================================
AT_DISPATCH_T *at_DispatchFindByChannel(int channel)
{
    AT_DISPATCH_T *node = gAtDispatchHead;
    while (node != NULL)
    {
        if (node->channelId == channel)
            return node;
        node = node->next;
    }
    return NULL;
}

// =============================================================================
// at_DispatchGetChannel
// =============================================================================
int at_DispatchGetChannel(AT_DISPATCH_T *th)
{
    return th->channelId;
}

// =============================================================================
// at_DispatchPushData
// =============================================================================
int at_DispatchPushData(AT_DISPATCH_T *th, const uint8_t *data, unsigned length)
{
    enum DISPATCH_MODE mode = th->mode;

    if (mode == DISPATCH_MODE_CMD)
        return at_CmdPushData(th->cmdEngine, data, length);
    if (mode == DISPATCH_MODE_DATA)
        return at_DataPushData(th->dataEngine, data, length);
#ifdef AT_CMUX_SUPPORT
    if (mode == DISPATCH_MODE_CMUX)
        return at_CmuxPushData(th->cmuxEngine, data, length);
#endif
    return -1; // never reach
}

// =============================================================================
// Timer callback for tplus timer. When called, tplus trailing period is
// satisfied, and dispatch will be switched to cmd mode forcedly.
// =============================================================================
static void at_DispatchTplusTimeout(void *par1)
{
    AT_DISPATCH_T *th = (AT_DISPATCH_T *)par1;

    if (th->buffLen == 3 && th->tplusCount == 3 &&
        th->buff[0] == '+' && th->buff[1] == '+' &&
        th->buff[2] == '+' &&
        th->mode == DISPATCH_MODE_DATA)
    {
        ATDISPLOG(I, "AT dispatch%d detected +++", th->channelId);
        at_ModuleModeSwitch(AT_MODE_SWITCH_DATA_ESCAPE, th->channelId);
        th->buffLen = 0;
    }

    th->tplusCount = 0;
}

// =============================================================================
// tplus detection. If tplusCount != 0, data should be cached.
// =============================================================================
static void at_DispatchTplusDetect(AT_DISPATCH_T *th, const uint8_t *data, unsigned length)
{
    unsigned tick = hal_TimGetUpTime();
    th->prevRecvTick = tick;

    if (th->buffLen + length > 3)
        goto tplus_reset;

    if (th->buffLen != th->tplusCount)
        goto tplus_reset;

    for (unsigned n = 0; n < length; n++)
    {
        if (data[n] != '+')
            goto tplus_reset;
    }

    if (th->buffLen == 0)
    {
        th->tplusStartTick = tick;
        if ((unsigned)(tick - th->prevRecvTick) < TPLUS_LEADING_TICK)
            goto tplus_reset;
    }
    else
    {
        if ((unsigned)(tick - th->tplusStartTick) > TPLUS_DURATION_TICK)
            goto tplus_reset;
    }

    at_memcpy(th->buff + th->buffLen, data, length);
    th->buffLen += length;
    th->tplusCount += length;

    if (th->tplusCount == 3)
        at_StartCallbackTimer(TPLUS_TRAILING_TICK, at_DispatchTplusTimeout, th);

    return;

tplus_reset:
    at_memcpy(th->buff + th->buffLen, data, length);
    th->buffLen += length;
    if (th->tplusCount == 3)
        at_StopCallbackTimer(at_DispatchTplusTimeout, th);
    th->tplusCount = 0;
}

// =============================================================================
// at_DispatchRead
// =============================================================================
void at_DispatchRead(AT_DISPATCH_T *th)
{
    if (th->device == NULL)
        return;

    for (;;)
    {
        // Read if there are room
        if (th->buffLen != th->buffSize)
        {
            uint8_t *fetch = &th->buff[th->buffLen];
            int length = at_DeviceRead(th->device, fetch, th->buffSize - th->buffLen);
            if (length > 0)
                at_DispatchTplusDetect(th, fetch, length);
        }

        if (th->buffLen == 0)
            break;

        // Push existed buffer to engine, if any.
        int len = at_DispatchPushData(th, th->buff, th->buffLen);
        if (len == th->buffLen)
        {
            // Clear buffer, and continue to push new data
            th->buffLen = 0;
        }
        else
        {
            // Remove used data and exit.
            if (len > 0)
            {
                th->buffLen -= len;
                memmove(th->buff, th->buff + len, th->buffLen);
            }
            break;
        }
    }
}

// =============================================================================
// at_DispatchWrite
// -----------------------------------------------------------------------------
/// Write data to underlay device. There are no cache in dispatch, all data
/// will be written or dropped. Downstream modules should check whether the
/// data should be written based on dispatch mode.
// =============================================================================
void at_DispatchWrite(AT_DISPATCH_T *th, const uint8_t *data, unsigned length)
{
    if (CMUX_PARENT(th) != NULL)
    {
        at_CmuxDlcWrite(CMUX_PARENT(th), data, length, th->cmuxDlci);
        return;
    }
    if (th->device != NULL)
        at_DeviceWrite(th->device, data, length);
}

// =============================================================================
// at_DispatchSetCmdMode
// =============================================================================
void at_DispatchSetCmdMode(AT_DISPATCH_T *th)
{
    ATDISPLOG(I, "AT dispatch%d set CMD mode", th->channelId);
    th->mode = DISPATCH_MODE_CMD;
}

// =============================================================================
// at_DispatchSetDataMode
// =============================================================================
void at_DispatchSetDataMode(AT_DISPATCH_T *th)
{
    ATDISPLOG(I, "AT dispatch%d set DATA mode", th->channelId);
    if (th->dataEngine == NULL)
    {
        th->dataEngine = at_DataEngineCreate(th);
    }
    th->mode = DISPATCH_MODE_DATA;
}

// =============================================================================
// at_DispatchSetCmuxMode
// =============================================================================
void at_DispatchSetCmuxMode(AT_DISPATCH_T *th, AT_CMUX_CONFIG_T *cfg)
{
    ATDISPLOG(I, "AT dispatch%d set CMUX mode", th->channelId);
#ifdef AT_CMUX_SUPPORT
    if (th->cmuxEngine == NULL)
    {
        th->cmuxEngine = at_CmuxEngineCreate(th, cfg);
    }
    th->mode = DISPATCH_MODE_CMUX;
#endif
}

// =============================================================================
// at_DispatchEndCmuxMode
// =============================================================================
void at_DispatchEndCmuxMode(AT_DISPATCH_T *th)
{
    ATDISPLOG(I, "AT dispatch%d end CMUX mode", th->channelId);
#ifdef AT_CMUX_SUPPORT
    // assert(th->mode != DISPATCH_MODE_CMUX)
    at_CmuxEngineDestroy(th->cmuxEngine);
    th->cmuxEngine = NULL;
    th->mode = DISPATCH_MODE_CMD;
#endif
}

// =============================================================================
// at_DispatchEndDataMode
// =============================================================================
void at_DispatchEndDataMode(AT_DISPATCH_T *th)
{
    ATDISPLOG(I, "AT dispatch%d end DATA mode", th->channelId);
    // assert(th->mode != DISPATCH_MODE_DATA)
    at_DataEngineDestroy(th->dataEngine);
    th->dataEngine = NULL;
    th->mode = DISPATCH_MODE_CMD;
}

// =============================================================================
// at_DispatchIsCmdMode/at_DispatchIsDataMode/at_DispatchIsCmuxMode
// =============================================================================
BOOL at_DispatchIsCmdMode(AT_DISPATCH_T *th) { return th->mode == DISPATCH_MODE_CMD; }
BOOL at_DispatchIsDataMode(AT_DISPATCH_T *th) { return th->mode == DISPATCH_MODE_DATA; }
BOOL at_DispatchIsCmuxMode(AT_DISPATCH_T *th) { return th->mode == DISPATCH_MODE_CMUX; }
AT_DEVICE_T *at_DispatchGetDevice(AT_DISPATCH_T *th) { return CMUX_PARENT(th) != NULL ? at_CmuxGetDevice(CMUX_PARENT(th)) : th->device; }
AT_CMD_ENGINE_T *at_DispatchGetCmdEngine(AT_DISPATCH_T *th) { return th->cmdEngine; }
AT_DATA_ENGINE_T *at_DispatchGetDataEngine(AT_DISPATCH_T *th) { return th->dataEngine; }
AT_CMUX_ENGINE_T *at_DispatchGetParentCmux(AT_DISPATCH_T *th) { return CMUX_PARENT(th); }
int at_DispatchGetDlci(AT_DISPATCH_T *th) { return CMUX_PARENT(th) == NULL ? -1 : th->cmuxDlci; }

// =============================================================================
// at_DispatchCreate
// =============================================================================
AT_DISPATCH_T *at_DispatchCreate(AT_DEVICE_T *device, AT_CMUX_ENGINE_T *cmuxParent, int dlci)
{
    AT_DISPATCH_T *th = (AT_DISPATCH_T *)at_malloc(sizeof(*th));

    th->mode = DISPATCH_MODE_CMD;
#ifdef AT_CMUX_SUPPORT
    th->cmuxParent = cmuxParent;
#endif
    th->cmuxDlci = dlci;
    th->buffSize = (CMUX_PARENT(th) == NULL) ? FETCH_BUF_SIZE : at_CmuxMaxFrameSize(cmuxParent);
    th->buff = (uint8_t *)at_malloc(th->buffSize);
    th->buffLen = 0;
    th->device = device;
    th->cmdEngine = NULL;
    th->dataEngine = NULL;
    th->cmuxEngine = NULL;
    th->prevRecvTick = 0;
    th->tplusStartTick = 0;
    th->tplusCount = 0;
    if (CMUX_PARENT(th) == NULL)
        th->channelId = at_ChannelFromDevice(at_DeviceGetNumber(device));
    else
        th->channelId = at_ChannelFromDeviceDlci(at_DeviceGetNumber(at_CmuxGetDevice(cmuxParent)), dlci);
    th->next = NULL;
    at_DispatchRegisterChannel(th);

    th->cmdEngine = at_CmdEngineCreate(th);
    return th;
}

// =============================================================================
// at_DispatchDestroy
// =============================================================================
void at_DispatchDestroy(AT_DISPATCH_T *th)
{
    if (th == NULL)
        return;

    at_DispatchUnregisterChannel(th);
    at_CmdEngineDestroy(th->cmdEngine);
    at_DataEngineDestroy(th->dataEngine);
#ifdef AT_CMUX_SUPPORT
    at_CmuxEngineDestroy(th->cmuxEngine);
#endif

    at_free(th->buff);
    at_free(th);
}
