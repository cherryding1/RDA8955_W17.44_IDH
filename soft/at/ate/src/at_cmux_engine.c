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

#include "at_cmux_engine.h"
#include "at_dispatch.h"
#include "at_device.h"
#include "at_utils.h"
#include <stdbool.h>

#ifdef AT_CMUX_SUPPORT

#define BASIC_FLAG 0xf9

#define ADV_FLAG 0x7e
#define ADV_ESCAPE_CTRL 0x7d
#define ADV_ESCAPE_MASK 0x20

#define CTRL_PF 0x10
#define FRAME_TYPE_SABM 0x2f
#define FRAME_TYPE_UA 0x63
#define FRAME_TYPE_DM 0x0f
#define FRAME_TYPE_DISC 0x43
#define FRAME_TYPE_UIH 0xef
#define FRAME_TYPE_UI 0x03

#define CTRL_MSG_PN 0x20
#define CTRL_MSG_PSC 0x10
#define CTRL_MSG_CLD 0x30
#define CTRL_MSG_TEST 0x08
#define CTRL_MSG_FCON 0x28
#define CTRL_MSG_FCOFF 0x18
#define CTRL_MSG_MSC 0x38
#define CTRL_MSG_NSC 0x04
#define CTRL_MSG_RPN 0x24
#define CTRL_MSG_RLS 0x14
#define CTRL_MSG_SNC 0x34

#define MSC_FC (1 << 1)
#define MSC_DSR_DTR (1 << 2)
#define MSC_CTS_RFR (1 << 3)
#define MSC_RI (1 << 6)
#define MSC_DCD (1 << 7)

// FOR UE
#define CR_COMMAND 0
#define CR_RESPONSE 1

#define CTRL_COMMAND 1
#define CTRL_RESPONSE 0

#define MAX_DLC_NUM 63
#define CTRL_DLCI 0

#define ADV_MIN_FRAME_SIZE 5   // FLAG, ADDRESS, CONTROL, INFO(0), FCS, FLAG
#define BASIC_MIN_FRAME_SIZE 6 // FLAG, ADDRESS, CONTROL, LENGTH(1), INFO(0), FCS, FLAG
#define MAX_TEST_SIZE 32       // implementation limitation

#define LENGTH_ONE_MAX 0x7f
#define LENGTH_TWO_MAX 0x7fff

#define SET_BITS(v, s, l) (((v) & ((1 << (l)) - 1)) << (s))
#define SET_BITS26(v) SET_BITS(v, 2, 6) // [7:2]
#define SET_BITS17(v) SET_BITS(v, 1, 7) // [7:1]
#define SET_CR(v) SET_BITS(v, 1, 1)
#define SET_EA SET_BITS(1, 0, 1)

#define GET_BITS(v, s, l) (((v) >> (s)) & ((1 << (l)) - 1))
#define GET_BITS26(v) GET_BITS(v, 2, 6) // [7:2]
#define GET_BITS17(v) GET_BITS(v, 1, 7) // [7:1]
#define GET_BIT1(v) GET_BITS(v, 1, 1)
#define GET_EA(v) GET_BITS(v, 0, 1)

#define CMUX_IN_BUFF_SIZE (4096)
#define CMUX_OUT_BUFF_SIZE (4096)

enum AT_CMUX_OPTION
{
    BASIC_OPTION,
    ADV_OPTION
};

enum AT_CMUX_CHANNEL_STATE
{
    CHANNEL_DISCONNECT,
    CHANNEL_CONNECT,
    CHANNEL_FLOW_STOPPED
};

typedef struct
{
    uint8_t frameType;
    uint8_t convergence;
    uint8_t priority;
    uint8_t ackTimer;
    uint16_t frameSize;
    uint8_t maxRetrans;
    uint8_t windowSize;
} AT_CMUX_PARAM_T;

typedef struct
{
    AT_DISPATCH_T *dispatch;
    enum AT_CMUX_CHANNEL_STATE state;
    AT_CMUX_PARAM_T param;
} AT_CMUX_CHANNEL_T;

typedef struct
{
    uint8_t dlci;
    uint8_t ctrl;
    int infoLen;
    uint8_t *info;
} AT_CMUX_FRAME_T;

struct AT_CMUX_ENGINE_T
{
    enum AT_CMUX_OPTION mode;
    AT_DISPATCH_T *dispatch; // upstream dispatch

    AT_CMUX_CONFIG_T cfg;
    int maxFrameSize;
    uint8_t buff[CMUX_IN_BUFF_SIZE];
    int buffLen;
    int buffSize;
    bool escaped;

    uint8_t outBuff[CMUX_OUT_BUFF_SIZE];
    int outLen;
    bool outOverflow;
    AT_CMUX_CHANNEL_T channels[MAX_DLC_NUM];
};

// =============================================================================
// Tiny helper to push a byte to buffer
// =============================================================================
static void at_CmuxPushByte(AT_CMUX_ENGINE_T *th, uint8_t c)
{
    // buffer overflow should be checked before call this.
    th->buff[th->buffLen++] = c;
}

// =============================================================================
// Check valid DLCI
// =============================================================================
static bool at_CmuxIsDlciValid(AT_CMUX_ENGINE_T *th, uint8_t dlci)
{
    // 27010 Clause 5.6
    return ((th->mode == BASIC_OPTION && dlci <= 61) ||
            (th->mode == ADV_OPTION && dlci <= 62));
}

// =============================================================================
// Default priority of DLC
// =============================================================================
static uint8_t at_CmuxDefaultPriority(uint8_t dlci)
{
    // 27010 Clause 5.6
    return dlci == 0 ? 0 : dlci > 55 ? 61 : (dlci | 7);
}

// =============================================================================
// Start output, put the FLAG to output buffer
// =============================================================================
static void at_CmuxOutStart(AT_CMUX_ENGINE_T *th)
{
    th->outBuff[0] = (th->mode == BASIC_OPTION) ? BASIC_FLAG : ADV_FLAG;
    th->outLen = 1;
    th->outOverflow = false;
}

// =============================================================================
// Finish output, put the FLAG to output buffer and send.
// On output overflow, output buffer is dropped silently.
// =============================================================================
static void at_CmuxOutEnd(AT_CMUX_ENGINE_T *th)
{
    if (!th->outOverflow)
    {
        th->outBuff[th->outLen++] = (th->mode == BASIC_OPTION) ? BASIC_FLAG : ADV_FLAG;
        if (th->dispatch != NULL)
            at_DispatchWrite(th->dispatch, th->outBuff, th->outLen);
    }
    th->outLen = 0;
}

// =============================================================================
// Push data to output buffer, and escape in advanced option is handled.
// Return false at output buffer overflow.
// =============================================================================
static bool at_CmuxOutPush(AT_CMUX_ENGINE_T *th, const uint8_t *data, unsigned length)
{
    if (th->outOverflow)
        return false;

    if (th->mode == BASIC_OPTION)
    {
        if (th->buffLen + length >= CMUX_OUT_BUFF_SIZE - 1)
        {
            th->outOverflow = true;
            return false;
        }
        at_memcpy(th->outBuff + th->outLen, data, length);
        th->outLen += length;
    }
    else
    {
        while (length--)
        {
            uint8_t c = *data++;
            if (c == ADV_ESCAPE_CTRL || c == ADV_FLAG)
            {
                th->outBuff[th->outLen++] = ADV_ESCAPE_CTRL;
                th->outBuff[th->outLen++] = c ^ ADV_ESCAPE_MASK;
            }
            else
            {
                th->outBuff[th->outLen++] = c;
            }
        }
    }
    return true;
}

// =============================================================================
// Send DM frame
// =============================================================================
static void at_CmuxSendDM(AT_CMUX_ENGINE_T *th, uint8_t dlci)
{
    ATLOGI("CMUX send DM, dlci/%d", dlci);

    at_CmuxOutStart(th);
    if (th->mode == BASIC_OPTION)
    {
        uint8_t data[4];
        data[0] = SET_BITS26(dlci) | SET_CR(CR_RESPONSE) | SET_EA;
        data[1] = FRAME_TYPE_DM | CTRL_PF;
        data[2] = SET_BITS17(0) | SET_EA;
        data[3] = at_CmuxCalcFcs(&data[0], 3);
        at_CmuxOutPush(th, data, 4);
    }
    else
    {
        uint8_t data[3];
        data[0] = SET_BITS26(dlci) | SET_CR(CR_RESPONSE) | SET_EA;
        data[1] = FRAME_TYPE_DM | CTRL_PF;
        data[2] = at_CmuxCalcFcs(&data[0], 2);
        at_CmuxOutPush(th, data, 3);
    }
    at_CmuxOutEnd(th);
}

// =============================================================================
// Send UA frame
// =============================================================================
static void at_CmuxSendUA(AT_CMUX_ENGINE_T *th, uint8_t dlci)
{
    ATLOGI("CMUX send UA, dlci/%d", dlci);

    at_CmuxOutStart(th);
    if (th->mode == BASIC_OPTION)
    {
        uint8_t data[4];
        data[0] = SET_BITS26(dlci) | SET_CR(CR_RESPONSE) | SET_EA;
        data[1] = FRAME_TYPE_UA | CTRL_PF;
        data[2] = SET_BITS17(0) | SET_EA;
        data[3] = at_CmuxCalcFcs(&data[0], 3);
        at_CmuxOutPush(th, data, 4);
    }
    else
    {
        uint8_t data[3];
        data[0] = SET_BITS26(dlci) | SET_CR(CR_RESPONSE) | SET_EA;
        data[1] = FRAME_TYPE_UA | CTRL_PF;
        data[2] = at_CmuxCalcFcs(&data[0], 2);
        at_CmuxOutPush(th, data, 3);
    }
    at_CmuxOutEnd(th);
}

// =============================================================================
// Send UIH frame
// =============================================================================
static void at_CmuxSendUIH(AT_CMUX_ENGINE_T *th, const uint8_t *indata, unsigned length, uint8_t dlci)
{
    ATLOGD("CMUX send UIH, dlci/%d, length/%d", dlci, length);

    if (length == 0)
        return;

    at_CmuxOutStart(th);
    if (th->mode == BASIC_OPTION)
    {
        uint8_t data[5];
        uint8_t fcs;

        data[0] = SET_BITS26(dlci) | SET_CR(CR_COMMAND) | SET_EA;
        data[1] = FRAME_TYPE_UIH;
        if (length <= LENGTH_ONE_MAX)
        {
            data[2] = SET_BITS17(length) | SET_EA;
            fcs = at_CmuxCalcFcs(data, 3);
            at_CmuxOutPush(th, data, 3);
        }
        else
        {
            data[2] = SET_BITS17(length) | SET_EA;
            data[3] = (length >> 7);
            fcs = at_CmuxCalcFcs(data, 4);
            at_CmuxOutPush(th, data, 4);
        }

        at_CmuxOutPush(th, indata, length);
        at_CmuxOutPush(th, &fcs, 1);
    }
    else
    {
        uint8_t data[2];
        data[0] = SET_BITS26(dlci) | SET_CR(CR_COMMAND) | SET_EA;
        data[1] = FRAME_TYPE_UIH;
        uint8_t fcs = at_CmuxCalcFcs(data, 2);
        at_CmuxOutPush(th, data, 2);
        at_CmuxOutPush(th, indata, length);
        at_CmuxOutPush(th, &fcs, 1);
    }
    at_CmuxOutEnd(th);
}

// =============================================================================
// Parse PN data to parameter
// =============================================================================
static void at_CmuxPNDataGet(const uint8_t *data, AT_CMUX_PARAM_T *param)
{
    param->frameType = GET_BITS(data[1], 0, 4);
    param->convergence = GET_BITS(data[1], 4, 4);
    param->priority = GET_BITS(data[2], 2, 6);
    param->ackTimer = data[3];
    param->frameSize = data[4] | (data[5] << 8);
    param->maxRetrans = data[6];
    param->windowSize = GET_BITS(data[7], 5, 3);
}

// =============================================================================
// Construct PN data from parameter
// =============================================================================
static void at_CmuxPNDataSet(uint8_t *data, AT_CMUX_PARAM_T *param)
{
    data[1] = SET_BITS(param->frameType, 0, 4) | SET_BITS(param->convergence, 4, 4);
    data[2] = SET_BITS(param->priority, 2, 6);
    data[3] = param->ackTimer;
    data[4] = param->frameSize & 0xff;
    data[5] = (param->frameSize >> 8) & 0xff;
    data[6] = param->maxRetrans;
    data[7] = SET_BITS(param->windowSize, 5, 3);
}

// =============================================================================
// Send CTRL msg NSC
// =============================================================================
static void at_CmuxSendCtrlNSC(AT_CMUX_ENGINE_T *th, uint8_t cr, uint8_t intype)
{
    ATLOGI("CMUX send ctrl NSC");
    uint8_t data[3];
    data[0] = SET_BITS26(CTRL_MSG_NSC) | SET_CR(cr) | SET_EA;
    data[1] = SET_BITS17(1) | SET_EA;
    data[2] = intype;
    at_CmuxSendUIH(th, &data[0], 3, CTRL_DLCI);
}

// =============================================================================
// Send CTRL msg PN
// =============================================================================
static void at_CmuxSendCtrlPN(AT_CMUX_ENGINE_T *th, AT_CMUX_PARAM_T *param, uint8_t cr, uint8_t dlci)
{
    ATLOGI("CMUX send ctrl PN");
    uint8_t data[10];
    data[0] = SET_BITS26(CTRL_MSG_PN) | SET_CR(cr) | SET_EA;
    data[1] = SET_BITS17(8) | SET_EA;
    at_CmuxPNDataSet(&data[2], param); // 8 bytes
    data[2] = SET_BITS26(dlci);
    at_CmuxSendUIH(th, data, 10, CTRL_DLCI);
}

// =============================================================================
// Send CTRL msg PSC
// =============================================================================
static void at_CmuxSendCtrlPSC(AT_CMUX_ENGINE_T *th, uint8_t cr)
{
    ATLOGI("CMUX send ctrl PSC");
    uint8_t data[2];
    data[0] = SET_BITS26(CTRL_MSG_PSC) | SET_CR(cr) | SET_EA;
    data[1] = SET_BITS17(0) | SET_EA;
    at_CmuxSendUIH(th, data, 2, CTRL_DLCI);
}

// =============================================================================
// Send CTRL msg CLD
// =============================================================================
static void at_CmuxSendCtrlCLD(AT_CMUX_ENGINE_T *th, uint8_t cr)
{
    ATLOGI("CMUX send ctrl CLD");
    uint8_t data[2];
    data[0] = SET_BITS26(CTRL_MSG_CLD) | SET_CR(cr) | SET_EA;
    data[1] = SET_BITS17(0) | SET_EA;
    at_CmuxSendUIH(th, data, 2, CTRL_DLCI);
}

// =============================================================================
// Send CTRL msg TEST
// =============================================================================
static void at_CmuxSendCtrlTEST(AT_CMUX_ENGINE_T *th, uint8_t cr, uint8_t *indata, unsigned length)
{
    if (length > MAX_TEST_SIZE)
    {
        ATLOGW("CMUX ctrl test info too long, drop it");
        return;
    }
    ATLOGI("CMUX send ctrl TEST");
    uint8_t data[2 + MAX_TEST_SIZE];
    data[0] = SET_BITS26(CTRL_MSG_TEST) | SET_CR(cr) | SET_EA;
    data[1] = SET_BITS17(length) | SET_EA;
    at_memcpy(&data[2], indata, length);
    at_CmuxSendUIH(th, data, 2 + length, CTRL_DLCI);
}

// =============================================================================
// Send CTRL msg FCON
// =============================================================================
static void at_CmuxSendCtrlFCON(AT_CMUX_ENGINE_T *th, uint8_t cr)
{
    ATLOGI("CMUX send ctrl FCON");
    uint8_t data[2];
    data[0] = SET_BITS26(CTRL_MSG_FCON) | SET_CR(cr) | SET_EA;
    data[1] = SET_BITS17(0) | SET_EA;
    at_CmuxSendUIH(th, data, 2, CTRL_DLCI);
}

// =============================================================================
// Send CTRL msg FCOFF
// =============================================================================
static void at_CmuxSendCtrlFCOFF(AT_CMUX_ENGINE_T *th, uint8_t cr)
{
    ATLOGI("CMUX send ctrl FCOFF");
    uint8_t data[2];
    data[0] = SET_BITS26(CTRL_MSG_FCOFF) | SET_CR(cr) | SET_EA;
    data[1] = SET_BITS17(0) | SET_EA;
    at_CmuxSendUIH(th, data, 2, CTRL_DLCI);
}

// =============================================================================
// Send CTRL msg MSC
// =============================================================================
static void at_CmuxSendCtrlMSC(AT_CMUX_ENGINE_T *th, uint8_t cr, uint8_t dlci, uint8_t v24, uint8_t br)
{
    ATLOGI("CMUX send ctrl MSC");
    if (br != 0)
    {
        uint8_t data[5];
        data[0] = SET_BITS26(CTRL_MSG_MSC) | SET_CR(cr) | SET_EA;
        data[1] = SET_BITS17(3) | SET_EA;
        data[2] = SET_BITS26(dlci) | SET_CR(1) | SET_EA;
        data[3] = v24 | SET_EA;
        data[4] = br | SET_EA;
        at_CmuxSendUIH(th, data, 5, CTRL_DLCI);
    }
    else
    {
        uint8_t data[4];
        data[0] = SET_BITS26(CTRL_MSG_MSC) | SET_CR(cr) | SET_EA;
        data[1] = SET_BITS17(2) | SET_EA;
        data[2] = SET_BITS26(dlci) | SET_CR(1) | SET_EA;
        data[3] = v24 | SET_EA;
        at_CmuxSendUIH(th, data, 4, CTRL_DLCI);
    }
}

// =============================================================================
// Send CTRL msg SNC
// =============================================================================
static void at_CmuxSendCtrlSNC(AT_CMUX_ENGINE_T *th, uint8_t cr, uint8_t dlci, uint8_t srv, uint8_t codec)
{
    ATLOGI("CMUX send ctrl SNC");
    uint8_t data[5];
    data[0] = SET_BITS26(CTRL_MSG_SNC) | SET_CR(cr) | SET_EA;
    data[1] = SET_BITS17(3) | SET_EA;
    data[2] = SET_BITS26(dlci) | SET_CR(1) | SET_EA;
    data[3] = SET_BITS17(srv) | SET_EA;
    data[4] = SET_BITS17(codec) | SET_EA;
    at_CmuxSendUIH(th, data, 4, CTRL_DLCI);
}

// =============================================================================
// Parse and handle CTRL msg
// =============================================================================
static void at_CmuxParseCtrl(AT_CMUX_ENGINE_T *th, AT_CMUX_FRAME_T *frame)
{
    if (GET_BIT1(frame->info[0]) != CTRL_COMMAND)
    {
        ATLOGW("CMUX unexpected ctrl msg response, drop it");
        return;
    }

    uint8_t type = GET_BITS26(frame->info[0]);
    int valLen = GET_BITS17(frame->info[1]);
    uint8_t *val = &frame->info[2];
    if (GET_EA(frame->info[1]) == 0) // 2 bytes length
    {
        valLen |= (frame->info[2] << 7);
        val++;
    }

    switch (type)
    {
    case CTRL_MSG_PN:
    {
        ATLOGI("CMUX recv parameter negotiation msg");
        if (valLen != 8)
        {
            ATLOGW("CMUX invalid PN value length %d, drop it", valLen);
            return;
        }

        AT_CMUX_PARAM_T param;
        at_CmuxPNDataGet(val, &param);

        uint8_t dlci = GET_BITS26(val[0]);
        AT_CMUX_CHANNEL_T *ch = &th->channels[dlci];

        // update frame size if peer needs smaller
        if (param.frameSize < ch->param.frameSize)
            ch->param.frameSize = param.frameSize;
        at_CmuxSendCtrlPN(th, &ch->param, CTRL_RESPONSE, dlci);
        break;
    }
    case CTRL_MSG_PSC:
    {
        ATLOGI("CMUX recv power saving msg");
        at_CmuxSendCtrlPSC(th, CTRL_RESPONSE);
        break;
    }
    case CTRL_MSG_CLD:
    {
        ATLOGI("CMUX recv close down msg");
        at_CmuxSendCtrlCLD(th, CTRL_RESPONSE);
        at_TaskCallback((COS_CALLBACK_FUNC_T)at_DispatchEndCmuxMode, th->dispatch);
        break;
    }
    case CTRL_MSG_TEST:
    {
        ATLOGI("CMUX recv test msg");
        at_CmuxSendCtrlTEST(th, CTRL_RESPONSE, val, valLen);
        break;
    }
    case CTRL_MSG_FCON:
    {
        ATLOGI("CMUX recv flow ctrl on msg");
        for (int n = 1; n < MAX_DLC_NUM; n++)
        {
            AT_CMUX_CHANNEL_T *ch = &th->channels[n];
            if (ch->state == CHANNEL_FLOW_STOPPED)
                ch->state = CHANNEL_CONNECT;
        }
        at_CmuxSendCtrlFCON(th, CTRL_RESPONSE);
        break;
    }
    case CTRL_MSG_FCOFF:
    {
        ATLOGI("CMUX recv flow ctrl off msg");
        for (int n = 1; n < MAX_DLC_NUM; n++)
        {
            AT_CMUX_CHANNEL_T *ch = &th->channels[n];
            if (ch->state == CHANNEL_CONNECT)
                ch->state = CHANNEL_FLOW_STOPPED;
        }
        at_CmuxSendCtrlFCOFF(th, CTRL_RESPONSE);
        break;
    }
    case CTRL_MSG_MSC:
    {
        uint8_t dlci = GET_BITS26(val[0]);
        ATLOGI("CMUX recv modem status msg dlci=%d", dlci);

        AT_CMUX_CHANNEL_T *ch = &th->channels[dlci];
        if (val[1] & MSC_FC)
        {
            if (ch->state == CHANNEL_CONNECT)
                ch->state = CHANNEL_FLOW_STOPPED;
        }
        else
        {
            if (ch->state == CHANNEL_FLOW_STOPPED)
                ch->state = CHANNEL_CONNECT;
        }
        uint8_t v24 = (ch->state == CHANNEL_CONNECT || CHANNEL_FLOW_STOPPED) ? (MSC_DSR_DTR | MSC_CTS_RFR) : 0;
        at_CmuxSendCtrlMSC(th, CTRL_RESPONSE, dlci, v24, 0);
        break;
    }
    case CTRL_MSG_SNC:
    {
        ATLOGI("CMUX recv service negotiation msg");
        uint8_t dlci = GET_BITS26(val[0]);
        uint8_t srv;
        uint8_t codec;
        // only data is supported
        if (valLen == 1)
        {
            srv = SET_CR(1);
            codec = 0;
        }
        else
        {
            srv = val[1] & SET_CR(1);
            codec = 0;
        }
        at_CmuxSendCtrlSNC(th, CTRL_RESPONSE, dlci, srv, codec);
        break;
    }

    case CTRL_MSG_NSC:
    case CTRL_MSG_RPN:
    case CTRL_MSG_RLS:
    default:
        ATLOGW("CMUX unsupported msg %d", type);
        at_CmuxSendCtrlNSC(th, CR_RESPONSE, frame->info[0]);
        break;
    }
}

// =============================================================================
// Parse and handle UIH frame
// =============================================================================
static void at_CmuxHandleUIH(AT_CMUX_ENGINE_T *th, AT_CMUX_FRAME_T *frame)
{
    if (!at_CmuxIsDlciValid(th, frame->dlci))
    {
        ATLOGW("CMUX UIH invalid dlci=%d", frame->dlci);
        at_CmuxSendDM(th, frame->dlci);
        return;
    }

    AT_CMUX_CHANNEL_T *ch = &th->channels[frame->dlci];
    if (ch->state != CHANNEL_CONNECT && ch->state != CHANNEL_FLOW_STOPPED)
    {
        ATLOGW("CMUX UIH dlci=%d not connected", frame->dlci);
        at_CmuxSendDM(th, frame->dlci);
        return;
    }

    if (frame->dlci == CTRL_DLCI)
    {
        at_CmuxParseCtrl(th, frame);
    }
    else
    {
        at_DispatchPushData(ch->dispatch, frame->info, frame->infoLen);
    }
}

// =============================================================================
// Parse and handle frame
// =============================================================================
static void at_CmuxParseFrame(AT_CMUX_ENGINE_T *th, AT_CMUX_FRAME_T *frame)
{
    ATLOGD("CMUX frame dlci=%d ctrl=0x%02x len=%d", frame->dlci, frame->ctrl, frame->infoLen);

    switch (frame->ctrl & ~CTRL_PF)
    {
    case FRAME_TYPE_SABM:
    {
        ATLOGI("CMUX SAMB received");

        if (!at_CmuxIsDlciValid(th, frame->dlci))
        {
            ATLOGW("CMUX invalid dlci");
            at_CmuxSendDM(th, frame->dlci);
            return;
        }

        AT_CMUX_CHANNEL_T *ch = &th->channels[frame->dlci];
        if (frame->dlci == CTRL_DLCI)
        {
            ch->state = CHANNEL_CONNECT;
        }
        else
        {
            if (ch->dispatch != NULL)
            {
                // already estabilished
                at_CmuxSendUA(th, frame->dlci);
                return;
            }

            ch->dispatch = at_DispatchCreate(NULL, th, frame->dlci);
            if (ch->dispatch != NULL)
            {
                ch->state = CHANNEL_CONNECT;
                at_CmuxSendUA(th, frame->dlci);
            }
            else
            {
                // create dispatch failed
                at_CmuxSendDM(th, frame->dlci);
            }
        }
        break;
    }
    case FRAME_TYPE_UA:
    {
        ATLOGW("CMUX unexpected UA response and dropped, dlci/%d", frame->dlci);
        break;
    }
    case FRAME_TYPE_DM:
    {
        ATLOGW("CMUX unexpected DM response and dropped, dlci/%d", frame->dlci);
        break;
    }
    case FRAME_TYPE_DISC:
    {
        if (!at_CmuxIsDlciValid(th, frame->dlci))
        {
            ATLOGW("CMUX invalid dlci=%d", frame->dlci);
            at_CmuxSendDM(th, frame->dlci);
            return;
        }

        ATLOGI("CMUX DISC received");
        AT_CMUX_CHANNEL_T *ch = &th->channels[frame->dlci];
        if (ch->state == CHANNEL_DISCONNECT)
        {
            at_CmuxSendDM(th, frame->dlci);
        }
        else
        {
            at_CmuxSendUA(th, frame->dlci);
            if (frame->dlci == CTRL_DLCI)
            {
                at_DispatchEndCmuxMode(th->dispatch);
            }
            else
            {
                ch->state = CHANNEL_DISCONNECT;
                at_DispatchDestroy(ch->dispatch);
                ch->dispatch = NULL;

                // TODO: (27010 Clause 5.8.2) When all DLC expcet 0 are closed,
                // UE should send CLD to TE. This isn't implemented. And we will
                // wait TE to send CLD or DISC/0.
            }
        }
        break;
    }
    case FRAME_TYPE_UIH:
        at_CmuxHandleUIH(th, frame);
        break;
    default:
        ATLOGW("CMUX invalid frame type 0x%02x in dlci/%d", frame->ctrl, frame->dlci);
        break;
    }
}

// =============================================================================
// Check frame FCS
// =============================================================================
static bool at_CmuxCheckFrame(AT_CMUX_ENGINE_T *th, AT_CMUX_FRAME_T *frame)
{
    uint8_t *data = th->buff;
    unsigned length = th->buffLen;

    frame->dlci = GET_BITS26(data[1]);
    frame->ctrl = data[2];

    int fcslen = (th->mode == ADV_OPTION) ? 2 : GET_EA(data[3]) ? 3 : 4;
    if (!at_CmuxCheckFcs(data + 1, fcslen, data[length - 2]))
    {
        ATLOGW("CMUX frame FCS error, drop len=%d", th->buffLen);
        if (LOGD_EN)
            ATDUMP(th->buff, th->buffLen);
        return false;
    }
    frame->infoLen = th->buffLen - (fcslen + 3); // SYNC ... FCS SYNC
    frame->info = &data[fcslen + 1];
    return true;
}

// =============================================================================
// Handle input data in BASIC option
// =============================================================================
static int at_CmuxBasicPushData(AT_CMUX_ENGINE_T *th, const uint8_t *data, unsigned length)
{
    ATLOGD("CMUX basic push data len=%d", length);
    unsigned total = length;

    while (length > 0)
    {
        uint8_t c = *data++;
        length--;

        // Drop whole buffer at overflow, maybe caused by data error
        // or buffer too small
        if (th->buffLen >= th->buffSize)
            th->buffLen = 0;

        if (th->buffLen == 0 && c != BASIC_FLAG)
            continue;
        if (th->buffLen == 1 && c == BASIC_FLAG)
            continue;

        if (th->buffLen < BASIC_MIN_FRAME_SIZE - 1)
        {
            at_CmuxPushByte(th, c);
            continue;
        }

        unsigned lenexp = GET_BITS17(th->buff[3]);
        if (GET_EA(th->buff[3]) == 0)
            lenexp = (lenexp | (th->buff[4] << 7)) + 7;
        else
            lenexp += 6;

        at_CmuxPushByte(th, c);
        if (th->buffLen >= lenexp)
        {
            if (c != BASIC_FLAG)
            {
                ATLOGW("CMUX missing end flag drop len=%d", th->buffLen);
                th->buffLen = 0;
            }
            else
            {
                AT_CMUX_FRAME_T frame = {};
                if (at_CmuxCheckFrame(th, &frame))
                {
                    at_CmuxParseFrame(th, &frame);
                    th->buffLen = 0; // drop handled data
                    at_CmuxPushByte(th, c);
                    return total - length;
                }
                th->buffLen = 0;
                at_CmuxPushByte(th, c);
            }
        }
    }
    return total - length;
}

// =============================================================================
// Handle input data in ADVANCED option
// =============================================================================
static int at_CmuxAdvancedPushData(AT_CMUX_ENGINE_T *th, const uint8_t *data, unsigned length)
{
    ATLOGD("CMUX adv push data len=%d", length);
    unsigned total = length;

    while (length > 0)
    {
        uint8_t c = *data++;
        length--;

        // Drop whole buffer at overflow, maybe caused by data error
        // or buffer too small
        if (th->buffLen >= th->buffSize)
        {
            th->escaped = false;
            th->buffLen = 0;
        }

        if (th->buffLen == 0 && c != ADV_FLAG)
        {
            th->escaped = false;
            continue;
        }

        if (c == ADV_FLAG)
        {
            th->escaped = false;
            at_CmuxPushByte(th, c);
            if (th->buffLen == 1)
            {
                ; // that is fine
            }
            else if (th->buffLen >= ADV_MIN_FRAME_SIZE)
            {
                AT_CMUX_FRAME_T frame = {};
                if (at_CmuxCheckFrame(th, &frame))
                {
                    at_CmuxParseFrame(th, &frame);
                    th->buffLen = 0; // drop handled data
                    at_CmuxPushByte(th, c);
                    return total - length;
                }
                th->buffLen = 0;
                at_CmuxPushByte(th, c);
            }
            else
            {
                th->buffLen = 0;
                at_CmuxPushByte(th, c);
            }
        }
        else
        {
            if (th->escaped)
            {
                at_CmuxPushByte(th, c ^ ADV_ESCAPE_MASK);
                th->escaped = false;
            }
            else if (c == ADV_ESCAPE_CTRL)
            {
                th->escaped = true;
            }
            else
            {
                at_CmuxPushByte(th, c);
            }
        }
    }
    return total - length;
}

// =============================================================================
// at_CmuxPushData
// =============================================================================
int at_CmuxPushData(AT_CMUX_ENGINE_T *th, const uint8_t *data, unsigned length)
{
    if (LOGD_EN)
        ATDUMP(data, length);
    if (th->mode == BASIC_OPTION)
        return at_CmuxBasicPushData(th, data, length);
    if (th->mode == ADV_OPTION)
        return at_CmuxAdvancedPushData(th, data, length);
    return -1; // not reachable
}

// =============================================================================
// at_CmuxDlcWrite
// =============================================================================
void at_CmuxDlcWrite(AT_CMUX_ENGINE_T *th, const uint8_t *data, unsigned length, int dlci)
{
    if (dlci <= 0 || dlci > MAX_DLC_NUM)
    {
        ATLOGE("CMUX invalid dlci %d for write, drop it", dlci);
        return;
    }

    AT_CMUX_CHANNEL_T *ch = &th->channels[dlci];
    if (ch->state == CHANNEL_CONNECT)
        at_CmuxSendUIH(th, data, length, dlci);
    // TODO: CHANNEL_FLOW_STOPPED
}

// =============================================================================
// at_CmuxEngineCreate
// =============================================================================
AT_CMUX_ENGINE_T *at_CmuxEngineCreate(AT_DISPATCH_T *dispatch, AT_CMUX_CONFIG_T *cfg)
{
    ATLOGI("CMUX engine create");

    AT_CMUX_ENGINE_T *th = (AT_CMUX_ENGINE_T *)at_malloc(sizeof(*th));
    th->cfg = *cfg;
    th->mode = (cfg->transparency == 0) ? BASIC_OPTION : ADV_OPTION;
    th->dispatch = dispatch;
    th->buffLen = 0;
    th->buffSize = CMUX_IN_BUFF_SIZE;
    th->escaped = false;
    th->outLen = 0;
    th->maxFrameSize = cfg->maxFrameSize;
    for (int n = 0; n < MAX_DLC_NUM; n++)
    {
        th->channels[n].dispatch = NULL;
        th->channels[n].state = CHANNEL_DISCONNECT;
        th->channels[n].param.frameType = 0;   // UIH
        th->channels[n].param.convergence = 0; // Type 1
        th->channels[n].param.priority = at_CmuxDefaultPriority(n);
        th->channels[n].param.ackTimer = cfg->ackTimer;
        th->channels[n].param.frameSize = CMUX_IN_BUFF_SIZE / 2;
        th->channels[n].param.maxRetrans = cfg->maxRetransCount;
        th->channels[n].param.windowSize = cfg->windowSize;
    }
    return th;
}

// =============================================================================
// at_CmuxEngineDestroy
// =============================================================================
void at_CmuxEngineDestroy(AT_CMUX_ENGINE_T *th)
{
    if (th == NULL)
        return;

    ATLOGI("CMUX engine destroy");
    for (int n = 0; n < MAX_DLC_NUM; n++)
        at_DispatchDestroy(th->channels[n].dispatch);
    at_free(th);
}

AT_CMUX_CONFIG_T at_CmuxGetConfig(AT_CMUX_ENGINE_T *th) { return th->cfg; }
int at_CmuxMaxFrameSize(AT_CMUX_ENGINE_T *th) { return th->maxFrameSize; }
AT_DEVICE_T *at_CmuxGetDevice(AT_CMUX_ENGINE_T *th) { return at_DispatchGetDevice(th->dispatch); }

#endif

AT_CMUX_CONFIG_T gAtCmuxDefaultCfg = {
    .transparency = 0,
    .subset = 0,
    .portSpeed = 5,
    .maxFrameSize = 127,
    .ackTimer = 10,
    .maxRetransCount = 3,
    .respTimer = 30,
    .wakeupRespTimer = 10,
    .windowSize = 2,
};
