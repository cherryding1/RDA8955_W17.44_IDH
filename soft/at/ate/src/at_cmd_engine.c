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

#include "at_cmd_engine.h"
#include "at_dispatch.h"
#include "at_utils.h"
#include "at_common.h"
#include "at_module.h"
#include "at_parse.h"
#include "at_device.h"
#include "at_cfg.h"
#include <stdbool.h>

#define CMD_LINE_BUFF_SIZE AT_CMD_LINE_BUFF_LEN

#define LF_TIMER_PERIOD 20 // wait 20ms after CR for LF
#define CHAR_CTRL_Z 0x1a
#define CHAR_ESC 0x1b
#define CHAR_CR gATCurrentnS3
#define CHAR_LF gATCurrentnS4
#define CHAR_BACKSPACE gATCurrentnS5

enum CMD_PARSER_MODE
{
    CMD_PARSE_MODE_LINE,
    CMD_PARSE_MODE_PROMPT,
    CMD_PARSE_MODE_FIXED,
    CMD_PARSE_MODE_BYPASS
};

struct AT_CMD_ENGINE_T
{
    AT_DISPATCH_T *dispatch;
    int channelId;

    enum CMD_PARSER_MODE mode; // current mode
    bool errSync;              // flags for recovering from buffer overflow
    uint8_t *buff;             // buffer, shared by all mode
    int buffLen;               // existed buffer data byte count
    int buffSize;              // buffer size
    int fixedLen;              // requested length in fixed length mode

    AT_CMD_BYPASS_CALLBACK_T bypassCallback;
    void *bypassParam;

    bool setFormatNeeded;
    AT_ModuleInfo module; // Legacy data structure
};

// =============================================================================
// at_CmdEngineCreate
// =============================================================================
AT_CMD_ENGINE_T *at_CmdEngineCreate(AT_DISPATCH_T *dispatch)
{
    AT_CMD_ENGINE_T *th = (AT_CMD_ENGINE_T *)at_malloc(sizeof(*th));

    th->mode = CMD_PARSE_MODE_LINE;
    th->dispatch = dispatch;
    th->channelId = at_DispatchGetChannel(dispatch);
    th->errSync = false;
    th->buff = (uint8_t *)at_malloc(CMD_LINE_BUFF_SIZE);
    th->buffLen = 0;
    th->buffSize = CMD_LINE_BUFF_SIZE;
    th->fixedLen = 0;
    th->bypassCallback = NULL;
    th->bypassParam = NULL;
    th->setFormatNeeded = false;

    th->module.cmds = NULL;
    th->module.cmd_buf.DataLen = 0;
    th->module.cmd_buf_bak.DataLen = 0;
    th->module.pIndList = NULL;
    th->module.cmssNum = 0;
    return th;
}

// =============================================================================
// at_CmdEngineDestroy
// =============================================================================
void at_CmdEngineDestroy(AT_CMD_ENGINE_T *th)
{
    if (th == NULL)
        return;

    at_free(th->buff);
    at_free(th);
}

// =============================================================================
// at_CmdWrite
// =============================================================================
void at_CmdWrite(AT_CMD_ENGINE_T *th, const uint8_t *data, unsigned length)
{
    if (th->dispatch != NULL && at_DispatchIsCmdMode(th->dispatch))
        at_DispatchWrite(th->dispatch, data, length);
}

// =============================================================================
// at_CmdChannelWrite
// =============================================================================
void at_CmdChannelWrite(const uint8_t *data, unsigned length, int channel)
{
    AT_DISPATCH_T *dispatch = at_DispatchGetByChannel(channel);
    if (dispatch != NULL && at_DispatchIsCmdMode(dispatch))
        at_DispatchWrite(dispatch, data, length);
}

// =============================================================================
// at_CmdDirectWrite
// =============================================================================
void at_CmdDirectWrite(const uint8_t *data, unsigned length)
{
    // HACK to get default dispatch
    AT_DISPATCH_T *dispatch = at_DispatchGetByChannel(-1);
    if (dispatch != NULL && at_DispatchIsCmdMode(dispatch))
        at_DispatchWrite(dispatch, data, length);
}

// =============================================================================
// Tiny helper to push a byte to buffer
// =============================================================================
static void at_CmdCachePushByte(AT_CMD_ENGINE_T *th, uint8_t c)
{
    // buffer overflow should be checked before call this.
    th->buff[th->buffLen++] = c;
}

// =============================================================================
// Append data to buffer. Buffer overflow should be considered here. When using
// fixed size buffer, we should drop data to avoid deadlock
// =============================================================================
static void at_CmdAppendToCache(AT_CMD_ENGINE_T *th, const uint8_t *data, unsigned length)
{
    const uint8_t *p = data;
    if (th->buffLen + length > th->buffSize) // overflow
    {
        th->buffLen = 0; // drop existed buffer
        if (length > th->buffLen)
        {
            p += length - th->buffLen;
            length = th->buffLen;
        }
    }
    at_memcpy(th->buff + th->buffLen, p, length);
    th->buffLen += length;
}

static void at_CmdBackupCmdBuff(AT_CMD_ENGINE_T *th)
{
    if (th->module.cmd_buf.DataLen != 0)
        at_memcpy(&th->module.cmd_buf_bak, &th->module.cmd_buf, sizeof(th->module.cmd_buf));
}

// =============================================================================
// Run the command line, which is already copied to cmd_buf
// =============================================================================
static void at_CmdRunCommandBuff(AT_CMD_ENGINE_T *th)
{
    AT_CMD_BUF *pCmdBuf = &th->module.cmd_buf;
    ATCMDLOGS(I, TSM(1), "AT CMD%d line: %s", th->channelId, pCmdBuf->DataBuf);
    int result = at_ModuleProcessLine(pCmdBuf->DataBuf, pCmdBuf->DataLen, th);

    if (result != 0) // parser error
    {
        th->module.cmd_buf_bak.DataLen = 0;
        at_CmdRespCmeError(th, ERR_AT_CME_INVALID_COMMAND_LINE);
        at_CmdlistFreeAll(&th->module.cmds);
        return;
    }

    th->setFormatNeeded = false;
    at_CmdBackupCmdBuff(th);
    at_ModuleRunCommand(th);
}

// =============================================================================
// Run command line specified by parameters
// =============================================================================
static void at_CmdRunCommandLine(AT_CMD_ENGINE_T *th, const uint8_t *data, unsigned length)
{
    if (length == 0)
        return;

    at_memcpy(th->module.cmd_buf.DataBuf, data, length);
    th->module.cmd_buf.DataBuf[length] = '\0';
    th->module.cmd_buf.DataLen = length;
    at_CmdRunCommandBuff(th);
}

// =============================================================================
// A complete command line is in buffer, and should send to downstream
// =============================================================================
static void at_CmdLineFinished(AT_CMD_ENGINE_T *th)
{
    // LF at the end is just for echo
    if (th->buff[th->buffLen - 1] == CHAR_LF)
        th->buffLen--;
    // parser need the last char to be '\r'
    if (th->buff[th->buffLen - 1] == CHAR_CR)
        th->buff[th->buffLen - 1] = '\r';
    at_CmdRunCommandLine(th, th->buff, th->buffLen);
}

// =============================================================================
// "A/" will repeat the previous command line.
// =============================================================================
static void at_CmdLineRepeat(AT_CMD_ENGINE_T *th)
{
    ATCMDLOG(I, "AT CMD%d line repeat", th->channelId);
    if (th->module.cmd_buf_bak.DataLen != 0)
    {
        at_memcpy(&th->module.cmd_buf, &th->module.cmd_buf_bak, sizeof(th->module.cmd_buf));
        at_CmdRunCommandBuff(th);
    }
}

// =============================================================================
// A complete prompt is in buffer, and should send to downstream
// =============================================================================
static void at_CmdPromptFinished(AT_CMD_ENGINE_T *th)
{
    ATCMDLOG(I, "AT CMD%d prompt end, len=%d, change back to line mode", th->channelId, th->buffLen);
    th->mode = CMD_PARSE_MODE_LINE;
    AT_KillAsyncTimerMux(th);
    at_ModuleRunCommandExtra(th, th->buff, th->buffLen);
}

// =============================================================================
// Requested fixed length data is in buffer, and should send to downstream
// =============================================================================
static void at_CmdRawFinished(AT_CMD_ENGINE_T *th)
{
    ATCMDLOG(I, "AT CMD%d raw end, len=%d, change back to line mode", th->channelId, th->buffLen);
    th->mode = CMD_PARSE_MODE_LINE;
    AT_KillAsyncTimerMux(th);
    at_ModuleRunCommandExtra(th, th->buff, th->buffLen);
}

// =============================================================================
// Timeout for waiting LF after CR
// =============================================================================
static void at_CmdLFTimeout(void *param)
{
    AT_CMD_ENGINE_T *th = (AT_CMD_ENGINE_T *)param;
    if (th->mode == CMD_PARSE_MODE_LINE && th->buffLen > 0 &&
        th->buff[th->buffLen - 1] == CHAR_CR)
    {
        ATCMDLOG(I, "AT CMD%d wait LF timeout", th->channelId);
        at_CmdLineFinished(th);
        th->buffLen = 0;
    }
}

// =============================================================================
// Feed data for line mode
// =============================================================================
static int at_CmdPushInLineMode(AT_CMD_ENGINE_T *th, const uint8_t *data, unsigned length)
{
    if (th->module.cmds != NULL)
        return -1;

    if (length > 0)
        at_StopCallbackTimer(at_CmdLFTimeout, th);

    unsigned total = length;
    while (length > 0)
    {
        uint8_t c = *data++;
        length--;

        // Remove previous byte for BACKSPACE
        if (c == CHAR_BACKSPACE && CHAR_BACKSPACE != CHAR_CR)
        {
            if (th->buffLen > 0)
                --th->buffLen;
            continue;
        }
        // Drop whole buffer at overflow, maybe caused by data error
        // or buffer too small. Reserve one space for '\0'
        if (th->buffLen >= th->buffSize - 1)
        {
            ATCMDLOG(E, "AT CMD%d line mode overflow, drop all", th->channelId);
            th->errSync = true;
            th->buffLen = 0;
        }
        // Error recovered at CR (this CR will be dropped also)
        if (th->errSync)
        {
            if (c == CHAR_CR)
                th->errSync = false;
            continue;
        }

        switch (th->buffLen)
        {
        case 0:
            if (c == 'a' || c == 'A')
                at_CmdCachePushByte(th, c);
            break;
        case 1:
            if (c == '/')
            {
                at_CmdCachePushByte(th, c);
                if (gATCurrenteCommandEchoMode)
                    at_CmdWrite(th, data - (total - length), total - length);

                at_CmdLineRepeat(th);
                th->buffLen = 0;
                return total - length;
            }
            else if (c == 't' || c == 'T')
            {
                at_CmdCachePushByte(th, c);
            }
            else
            {
                th->buffLen = 0;
            }
            break;
        default:
            if (th->buffLen > 0 && th->buff[th->buffLen - 1] == CHAR_CR)
            {
                if (c == CHAR_LF)
                    at_CmdCachePushByte(th, c);
                else
                    length++;

                if (gATCurrenteCommandEchoMode)
                    at_CmdWrite(th, data - (total - length), total - length);
                at_CmdLineFinished(th);
                th->buffLen = 0;
                return total - length;
            }

            at_CmdCachePushByte(th, c);
            if (c == CHAR_CR)
            {
                if (length == 0)
                {
                    at_StartCallbackTimer(LF_TIMER_PERIOD, at_CmdLFTimeout, th);
                    break;
                }
                else
                {
                    uint8_t next = *data;
                    if (next == CHAR_LF)
                    {
                        at_CmdCachePushByte(th, next);
                        data++;
                        length--;
                    }
                    if (gATCurrenteCommandEchoMode)
                        at_CmdWrite(th, data - (total - length), total - length);
                    at_CmdLineFinished(th);
                    th->buffLen = 0;
                    return total - length;
                }
            }
            break;
        }
    }
    if (gATCurrenteCommandEchoMode)
        at_CmdWrite(th, data - total, total);
    return total;
}

// =============================================================================
// Feed data for prompt mode
// =============================================================================
static int at_CmdPushInPromptMode(AT_CMD_ENGINE_T *th, const uint8_t *data, unsigned length)
{
    unsigned total = length;

    while (length > 0)
    {
        uint8_t c = *data++;
        length--;

        // Remove previous byte for BACKSPACE
        if (c == CHAR_BACKSPACE && CHAR_BACKSPACE != CHAR_CTRL_Z && CHAR_BACKSPACE != CHAR_ESC)
        {
            if (th->buffLen > 0)
                --th->buffLen;
            continue;
        }
        // Drop whole buffer at overflow, maybe caused by data error
        // or buffer too small
        if (th->buffLen >= th->buffSize)
        {
            ATCMDLOG(E, "AT CMD%d prompt mode overflow, drop all", th->channelId);
            th->errSync = true;
            th->buffLen = 0;
        }
        if (c == CHAR_CTRL_Z || c == CHAR_ESC)
        {
            th->errSync = false;
            at_CmdCachePushByte(th, c);
            if (gATCurrenteCommandEchoMode)
                at_CmdWrite(th, data - (total - length), total - length);

            at_CmdPromptFinished(th);
            th->buffLen = 0;
            return total - length;
        }
        // In error recovery mode, all other than CTRL-Z/ESC will be dropped
        if (!th->errSync)
            at_CmdCachePushByte(th, c);
    }
    if (gATCurrenteCommandEchoMode)
        at_CmdWrite(th, data - total, total);
    return total;
}

// =============================================================================
// Feed data for fixed length mode
// =============================================================================
static int at_CmdPushInRawMode(AT_CMD_ENGINE_T *th, const uint8_t *data, unsigned length)
{
    int needed = th->fixedLen - th->buffLen;

    if (needed == 0)
        return 0;

    if (gATCurrenteCommandEchoMode)
            at_CmdWrite(th, data, length);
    if (length >= needed)
    {
        at_CmdAppendToCache(th, data, needed);
        at_CmdRawFinished(th);
        return length;
    }

    at_CmdAppendToCache(th, data, length);
    return length;
}

// =============================================================================
// at_CmdPushData
// =============================================================================
int at_CmdPushData(AT_CMD_ENGINE_T *th, const uint8_t *data, unsigned length)
{
    // mode may be changed at feeding data
    enum CMD_PARSER_MODE mode = th->mode;

    if (mode == CMD_PARSE_MODE_LINE)
        return at_CmdPushInLineMode(th, data, length);
    if (mode == CMD_PARSE_MODE_PROMPT)
        return at_CmdPushInPromptMode(th, data, length);
    if (mode == CMD_PARSE_MODE_FIXED)
        return at_CmdPushInRawMode(th, data, length);
    if (mode == CMD_PARSE_MODE_BYPASS)
        return th->bypassCallback(th->bypassParam, data, length);
    return -1; // never reached
}

// =============================================================================
// at_CmdSetLineMode
// =============================================================================
void at_CmdSetLineMode(AT_CMD_ENGINE_T *th)
{
    ATCMDLOG(I, "AT CMD%d set line mode", th->channelId);
    th->mode = CMD_PARSE_MODE_LINE;
    th->buffLen = 0;
}

// =============================================================================
// at_CmdSetPromptMode
// =============================================================================
void at_CmdSetPromptMode(AT_CMD_ENGINE_T *th)
{
    ATCMDLOG(I, "AT CMD%d set prompt mode", th->channelId);
    th->mode = CMD_PARSE_MODE_PROMPT;
    th->buffLen = 0;
}

// =============================================================================
// at_CmdSetRawMode
// =============================================================================
bool at_CmdSetRawMode(AT_CMD_ENGINE_T *th, unsigned length)
{
    ATCMDLOG(I, "AT CMD%d set raw mode: len=%d", th->channelId, length);
    if (length > th->buffSize)
    {
        ATCMDLOG(E, "AT CMD%d raw mode length is too large", th->channelId);
        return false;
    }
    th->fixedLen = length;
    th->mode = CMD_PARSE_MODE_FIXED;
    th->buffLen = 0;
    return true;
}

// =============================================================================
// at_CmdSetBypassMode
// =============================================================================
void at_CmdSetBypassMode(AT_CMD_ENGINE_T *th, AT_CMD_BYPASS_CALLBACK_T callback, void *param)
{
    ATCMDLOG(I, "AT CMD%d set bypass mode, callback=0x%08x/0x%08x", th->channelId, callback, param);
    th->mode = CMD_PARSE_MODE_BYPASS;
    th->bypassCallback = callback;
    th->bypassParam = param;
}

// =============================================================================
// at_CmdDeviceSetFormatNeeded
// =============================================================================
void at_CmdDeviceSetFormatNeeded(AT_CMD_ENGINE_T *th)
{
    th->setFormatNeeded = true;
}

// =============================================================================
// at_CmdCommandFinished
// =============================================================================
void at_CmdCommandFinished(AT_CMD_ENGINE_T *th)
{
    if (th->module.cmds == NULL)
    {
        if (th->setFormatNeeded)
        {
            AT_DEVICE_T *device = at_DispatchGetDevice(th->dispatch);
            at_DeviceSetFormat(device, gATCurrentu32nBaudRate, gATCurrentIcfFormat, gATCurrentIcfParity);
            th->setFormatNeeded = false;
        }

        at_DeviceScheduleRead(at_DispatchGetDevice(th->dispatch));
    }
}

AT_CMD_ENGINE_T *at_CmdGetByChannel(int channel) { return at_DispatchGetCmdEngine(at_DispatchGetByChannel(channel)); }
AT_ModuleInfo *at_CmdGetModule(AT_CMD_ENGINE_T *th) { return &(th->module); }
int at_CmdGetChannel(AT_CMD_ENGINE_T *th) { return th->channelId; }
bool at_CmdIsLineMode(AT_CMD_ENGINE_T *th) { return th->mode == CMD_PARSE_MODE_LINE; }
bool at_CmdIsPromptMode(AT_CMD_ENGINE_T *th) { return th->mode == CMD_PARSE_MODE_PROMPT; }
bool at_CmdIsRawMode(AT_CMD_ENGINE_T *th) { return th->mode == CMD_PARSE_MODE_FIXED; }
bool at_CmdIsBypassMode(AT_CMD_ENGINE_T *th) { return th->mode == CMD_PARSE_MODE_BYPASS; }
