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


#include "syscmds_m.h"

#include "syscmdsp_debug.h"
#include "syscmdsp_hdlr.h"
#include "syscmds_gen_cmd_id.h"
#include "hal_ap_comm.h"
#include "hal_timers.h"
#include "sxr_tksd.h"
#include "sxr_sbx.h"
#include "string.h"
#include "gpio_i2c.h"

#ifndef _TGT_MODEM_NO_TS_
extern VOID syscmds_StartTsTask(VOID);
#endif
extern VOID ap_SleepInit(VOID);

PRIVATE VOID syscmds_Task(VOID *param);

PRIVATE VOID syscmds_SendUnknownCmdRecvMsg(UINT32 recvReqId, UINT16 recvModId,
        UINT16 recvMsgId);

extern VOID sxr_Sleep (UINT32 Period);

// =============================================================================
// MACROS
// =============================================================================

#define SYS_CMD_HEADER_PREFIX     (0xA8B1)

#define SYS_CMD_HEADER_FIXED_SIZE (16)

#define SYS_CMD_RECV_FIFO_SIZE    (3)

#define SYS_CMD_SEND_FIFO_SIZE    (10)

// In number of words
#define SYS_CMD_TASK_STACK_SIZE   (512)

#define SYS_CMD_TASK_PRIORITY     (205)

#define SYS_CMD_VERSION_MAJOR     (1)

#define SYS_CMD_VERSION_MINOR     (1)

#define SYS_CMD_VERSION_NUMBER    ((SYS_CMD_VERSION_MAJOR<<16)|SYS_CMD_VERSION_MINOR)


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// SYSCMDS_DEC_STATE_T
// -----------------------------------------------------------------------------
/// The decoding state
// =============================================================================
typedef enum
{
    SYSCMDS_DEC_STATE_IDLE = 0,
    SYSCMDS_DEC_STATE_REQ_ID,
    SYSCMDS_DEC_STATE_RET_VAL,
    SYSCMDS_DEC_STATE_MSG_IDLEN,
    SYSCMDS_DEC_STATE_MSG_PARAM,
} SYSCMDS_DEC_STATE_T;

// =============================================================================
// SYSCMDS_DEC_VAR_T
// -----------------------------------------------------------------------------
/// The system command decoding variable structure
// =============================================================================
typedef struct
{
    SYSCMDS_DEC_STATE_T state;
    VOLATILE UINT32 rIdx;
    VOLATILE UINT32 wIdx;
    SYSCMDS_RECV_MSG_T recvFifo[SYS_CMD_RECV_FIFO_SIZE];
    UINT16 msgParamDec;
} SYSCMDS_DEC_VAR_T;

// =============================================================================
// SYSCMDS_SEND_MSG_T
// -----------------------------------------------------------------------------
/// The structure to save system command messages to be sent
// =============================================================================
typedef struct
{
    UINT16 modId;
    UINT16 msgId;
    UINT16 paramLen;
    UINT8 param[SYS_CMD_IRQ_SEND_PARAM_MAX_SIZE];
} SYSCMDS_SEND_MSG_T;

// =============================================================================
// SYSCMDS_SEND_VAR_T
// -----------------------------------------------------------------------------
/// The system command sending variable structure
// =============================================================================
typedef struct
{
    VOLATILE UINT32 rIdx;
    VOLATILE UINT32 wIdx;
    SYSCMDS_SEND_MSG_T sendFifo[SYS_CMD_SEND_FIFO_SIZE];
} SYSCMDS_SEND_VAR_T;


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

PRIVATE CONST sxr_TaskDesc_t gc_syscmdsTaskDesc =
{
    .TaskBody = &syscmds_Task,
    .TaskExit = NULL,
    .Name = "Syscmds Task",
    .StackSize = SYS_CMD_TASK_STACK_SIZE,
    .Priority = SYS_CMD_TASK_PRIORITY,
};

PRIVATE UINT8 g_syscmdsTaskId;

PRIVATE BOOL g_syscmdsRecvFifoFull = FALSE;

PRIVATE UINT8 g_syscmdsSendSema = 0xFF;

PRIVATE UINT8 g_syscmdsSendTaskId = 0xFF;

PRIVATE SYSCMDS_DEC_VAR_T g_syscmdsDecVar;

PRIVATE SYSCMDS_SEND_VAR_T g_syscmdsSendVar;
PUBLIC   BOOL g_syscmdsTaskSuspend =  FALSE;

PRIVATE UINT32 g_syscmdsSendBuffer[(SYS_CMD_HEADER_FIXED_SIZE+SYS_CMD_PARAM_MAX_SIZE+3)/4];


// =============================================================================
//  FUNCTIONS
// =============================================================================

PRIVATE VOID syscmds_WakeupTask(VOID)
{
    g_syscmdsTaskSuspend = FALSE;
    hal_APCommRequestFreq(FALSE);
    sxr_ResumeTask(g_syscmdsTaskId);
}

PRIVATE VOID syscmds_SendBufferFree(VOID)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();
    if (g_syscmdsSendTaskId != 0xFF)
    {
        sxr_ResumeTask(g_syscmdsSendTaskId);
    }
    hal_SysExitCriticalSection(scStatus);
}

PRIVATE UINT32 syscmds_Decode(CONST UINT8 *buf, UINT32 len)
{
    //SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0, "syscmds_Decode");

    UINT32 pos = 0;
    UINT32 rIdx, wIdx, copySize;
    UINT16 prefix, modId, msgId, paramLen;
    UINT32 reqId, retVal;

    SYSCMDS_ASSERT(((UINT32)buf&0x3) == 0, "Invalid syscmds buf addr: 0x%08x", buf);
    SYSCMDS_ASSERT(len >= 4, "Invalid syscmds buf len: %d", len);

    while (pos < len && !g_syscmdsRecvFifoFull)
    {
        switch (g_syscmdsDecVar.state)
        {
            default:
            case SYSCMDS_DEC_STATE_IDLE:
                rIdx = g_syscmdsDecVar.rIdx;
                wIdx = g_syscmdsDecVar.wIdx;

                SYSCMDS_ASSERT(wIdx-rIdx <= SYS_CMD_RECV_FIFO_SIZE,
                               "Invalid syscmds recv fifo indexes: wIdx=0x%x, rIdx=0x%x", wIdx, rIdx);

                if (wIdx - rIdx == SYS_CMD_RECV_FIFO_SIZE)
                {
                    g_syscmdsRecvFifoFull = TRUE;
                    break;
                }

                prefix = *(UINT16*)(buf + pos);
                SYSCMDS_ASSERT(prefix == SYS_CMD_HEADER_PREFIX,
                               "Invalid syscmds frame prefix: 0x%04x", prefix);

                pos += 2;
                modId = *(UINT16*)(buf + pos);
                SYSCMDS_ASSERT(modId < SYSCMDS_MOD_ID_END,
                               "Invalid syscmds module ID: %d", modId);

                pos += 2;
                // FIFO index
                wIdx %= SYS_CMD_RECV_FIFO_SIZE;
                g_syscmdsDecVar.recvFifo[wIdx].modId = modId;

                g_syscmdsDecVar.state = SYSCMDS_DEC_STATE_REQ_ID;
                break;
            case SYSCMDS_DEC_STATE_REQ_ID:
                SYSCMDS_ASSERT(len-pos >= 4,
                               "Invalid syscmds buf len: len=%d, pos=%d", len, pos);

                reqId = *(UINT32*)(buf + pos);
                pos += 4;
                // FIFO index
                wIdx = g_syscmdsDecVar.wIdx % SYS_CMD_RECV_FIFO_SIZE;
                g_syscmdsDecVar.recvFifo[wIdx].reqId = reqId;

                g_syscmdsDecVar.state = SYSCMDS_DEC_STATE_RET_VAL;
                break;
            case SYSCMDS_DEC_STATE_RET_VAL:
                SYSCMDS_ASSERT(len-pos >= 4,
                               "Invalid syscmds buf len: len=%d, pos=%d", len, pos);

                retVal = *(UINT32*)(buf + pos);
                pos += 4;
                // FIFO index
                wIdx = g_syscmdsDecVar.wIdx % SYS_CMD_RECV_FIFO_SIZE;
                g_syscmdsDecVar.recvFifo[wIdx].retVal = retVal;

                g_syscmdsDecVar.state = SYSCMDS_DEC_STATE_MSG_IDLEN;
                break;
            case SYSCMDS_DEC_STATE_MSG_IDLEN:
                SYSCMDS_ASSERT(len-pos >= 4,
                               "Invalid syscmds buf len: len=%d, pos=%d", len, pos);

                msgId = *(UINT16*)(buf + pos);
                pos += 2;
                paramLen = *(UINT16*)(buf + pos);
                SYSCMDS_ASSERT(paramLen <= SYS_CMD_PARAM_MAX_SIZE,
                               "Invalid syscmds param len: %d", paramLen);

                pos += 2;
                // FIFO index
                wIdx = g_syscmdsDecVar.wIdx % SYS_CMD_RECV_FIFO_SIZE;
                g_syscmdsDecVar.recvFifo[wIdx].msgId = msgId;
                g_syscmdsDecVar.recvFifo[wIdx].paramLen = paramLen;
                g_syscmdsDecVar.msgParamDec = 0;

                if (paramLen > 0)
                {
                    g_syscmdsDecVar.state = SYSCMDS_DEC_STATE_MSG_PARAM;
                }
                else
                {
                    // Update write index;
                    g_syscmdsDecVar.wIdx++;
                    // Wakeup syscmds task
                    syscmds_WakeupTask();
                    // Reset decoding state
                    g_syscmdsDecVar.state = SYSCMDS_DEC_STATE_IDLE;
                }
                break;
            case SYSCMDS_DEC_STATE_MSG_PARAM:
                // FIFO index
                wIdx = g_syscmdsDecVar.wIdx % SYS_CMD_RECV_FIFO_SIZE;
                copySize = g_syscmdsDecVar.recvFifo[wIdx].paramLen - g_syscmdsDecVar.msgParamDec;
                if (copySize > len - pos)
                {
                    copySize = len - pos;
                }
                memcpy(&g_syscmdsDecVar.recvFifo[wIdx].param[g_syscmdsDecVar.msgParamDec],
                       buf + pos, copySize);
                pos += copySize;
                g_syscmdsDecVar.msgParamDec += copySize;
                if (g_syscmdsDecVar.recvFifo[wIdx].paramLen == g_syscmdsDecVar.msgParamDec)
                {
                    // The whole msg has been saved
                    // Align in 4 bytes
                    pos = (pos + 3) & ~0x3;
                    // Update write index;
                    g_syscmdsDecVar.wIdx++;
                    // Wakeup syscmds task
                    syscmds_WakeupTask();
                    // Reset decoding state
                    g_syscmdsDecVar.state = SYSCMDS_DEC_STATE_IDLE;
                }
                break;
        }
    }

    if (g_syscmdsRecvFifoFull)
    {
        // Disable system command arrival irq
        hal_ApCommSetSysCmdRecvCallback(NULL);
    }

    return pos;
}

PRIVATE VOID syscmds_Task(VOID *param)
{
    UINT32 scStatus;
    UINT32 rIdx, wIdx;
    SYSCMDS_RECV_MSG_T *pRecvMsg;
    SYSCMDS_SEND_MSG_T *pSendMsg;
    BOOL result = FALSE;

#ifndef _TGT_MODEM_NO_TS_
    syscmds_StartTsTask();
    gpio_i2c_open();
#endif
    while (1)
    {
        scStatus = hal_SysEnterCriticalSection();
        while (g_syscmdsDecVar.rIdx == g_syscmdsDecVar.wIdx &&
                g_syscmdsSendVar.rIdx == g_syscmdsSendVar.wIdx)
        {
            g_syscmdsTaskSuspend = TRUE;

            hal_APCommRequestFreq(TRUE);
            sxr_SuspendTask(g_syscmdsTaskId);
        }
        hal_SysExitCriticalSection(scStatus);

        if (g_syscmdsDecVar.rIdx != g_syscmdsDecVar.wIdx)
        {
            rIdx = g_syscmdsDecVar.rIdx;
            wIdx = g_syscmdsDecVar.wIdx;

            SYSCMDS_ASSERT(wIdx-rIdx <= SYS_CMD_RECV_FIFO_SIZE,
                           "Invalid syscmds recv fifo indexes: wIdx=0x%x, rIdx=0x%x", wIdx, rIdx);

            // FIFO index
            rIdx %= SYS_CMD_RECV_FIFO_SIZE;
            pRecvMsg = &g_syscmdsDecVar.recvFifo[rIdx];
#if 0
            SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,
                          "Recv cmd: mod=0x%x, req=0x%x, id=0x%x, len=%d, data=0x%08x",
                          pRecvMsg->modId, pRecvMsg->reqId, pRecvMsg->msgId,
                          pRecvMsg->paramLen, *(UINT32 *)&pRecvMsg->param[0]);
#endif
            switch (pRecvMsg->modId)
            {
                case SYSCMDS_MOD_ID_GENERAL:
                    result = syscmds_HandleGeneralCmd(pRecvMsg);
                    break;
                case SYSCMDS_MOD_ID_CALIB:
                    result = syscmds_HandleCalibCmd(pRecvMsg);
                    break;
                case SYSCMDS_MOD_ID_PM:
                    result = syscmds_HandlePmCmd(pRecvMsg);
                    break;
                case SYSCMDS_MOD_ID_AUDIO:
                    result = syscmds_HandleAudioCmd(pRecvMsg);
                    break;
                case SYSCMDS_MOD_ID_TS:
                    result = syscmds_HandleTsCmd(pRecvMsg);
                    break;
                case SYSCMDS_MOD_ID_GPIO:
                    result = syscmds_HandleGpioCmd(pRecvMsg);
                    break;
                default:
                    break;
            }

            if (!result)
            {
                syscmds_SendUnknownCmdRecvMsg(pRecvMsg->reqId,
                                              pRecvMsg->modId,
                                              pRecvMsg->msgId);
            }

            scStatus = hal_SysEnterCriticalSection();
            // Update read index
            g_syscmdsDecVar.rIdx++;
            if (g_syscmdsRecvFifoFull)
            {
                g_syscmdsRecvFifoFull = FALSE;
                // Enable system command arrival irq
                hal_ApCommSetSysCmdRecvCallback(&syscmds_Decode);
            }
            hal_SysExitCriticalSection(scStatus);
        }

        if (g_syscmdsSendVar.rIdx != g_syscmdsSendVar.wIdx)
        {
            rIdx = g_syscmdsSendVar.rIdx;
            wIdx = g_syscmdsSendVar.wIdx;

            SYSCMDS_ASSERT(wIdx-rIdx <= SYS_CMD_SEND_FIFO_SIZE,
                           "Invalid syscmds send fifo indexes: wIdx=0x%x, rIdx=0x%x", wIdx, rIdx);

            // FIFO index
            rIdx %= SYS_CMD_SEND_FIFO_SIZE;
            pSendMsg = &g_syscmdsSendVar.sendFifo[rIdx];

            syscmds_Send(pSendMsg->modId, pSendMsg->msgId,
                         pSendMsg->param, pSendMsg->paramLen);
            // Update read index
            g_syscmdsSendVar.rIdx++;
        }
    }
}

PRIVATE VOID syscmds_SendSingleCmd(UINT16 modId, UINT32 reqId, UINT32 retVal,
                                   UINT16 msgId, VOID *param, UINT16 len)
{
    UINT32 sent;
    UINT32 scStatus;

    g_syscmdsSendBuffer[0] = SYS_CMD_HEADER_PREFIX | (modId << 16);
    g_syscmdsSendBuffer[1] = reqId;
    g_syscmdsSendBuffer[2] = retVal;
    g_syscmdsSendBuffer[3] = msgId | (len << 16);
    if (len > 0)
    {
        SYSCMDS_ASSERT(len <= SYS_CMD_PARAM_MAX_SIZE,
                       "Invalid syscmds len in single send: %d", len);
        memcpy(&g_syscmdsSendBuffer[4], param, len);
    }

    while (1)
    {
        // Clear buffer free irq
        hal_ApCommClearSysCmdBufFreeIrq();
        sent = hal_ApCommSendSysCmd((UINT8*)g_syscmdsSendBuffer,
                                    SYS_CMD_HEADER_FIXED_SIZE+len);
        if (sent == SYS_CMD_HEADER_FIXED_SIZE + len)
        {
            break;
        }
        // Send buffer full. Wait for buffer free irq

        scStatus = hal_SysEnterCriticalSection();
        // Enable buffer free irq
        hal_ApCommSetSysCmdBufFreeCallback(&syscmds_SendBufferFree);
        // Suspend myself
        g_syscmdsSendTaskId = sxr_GetCurrentTaskId();
        sxr_SuspendTask(g_syscmdsSendTaskId);
        g_syscmdsSendTaskId = 0xFF;
        // Disable buffer free irq
        hal_ApCommSetSysCmdBufFreeCallback(NULL);
        hal_SysExitCriticalSection(scStatus);
    }
}

PRIVATE VOID syscmds_SendUnknownCmdRecvMsg(UINT32 recvReqId, UINT16 recvModId,
        UINT16 recvMsgId)
{
    UINT32 unknownId = (recvMsgId<<16) | recvModId;

    sxr_TakeSemaphore(g_syscmdsSendSema);

    syscmds_SendSingleCmd(SYSCMDS_MOD_ID_GENERAL, recvReqId, unknownId,
                          MODEM_GENERAL_CMD_UNKNOWN_CMD_RECV,
                          NULL, 0);

    sxr_ReleaseSemaphore(g_syscmdsSendSema);
}

PROTECTED VOID syscmds_Send(UINT16 modId, UINT16 msgId, VOID *param, UINT16 len)
{
    sxr_TakeSemaphore(g_syscmdsSendSema);

    syscmds_SendSingleCmd(modId, 0, 0, msgId, param, len);

    sxr_ReleaseSemaphore(g_syscmdsSendSema);
}

PROTECTED VOID syscmds_IrqSend(UINT16 modId, UINT16 msgId, VOID *param, UINT16 len)
{
    UINT32 scStatus;
    UINT32 rIdx, wIdx;
    UINT32 k1;

    scStatus = hal_SysEnterCriticalSection();

    rIdx = g_syscmdsSendVar.rIdx;
    wIdx = g_syscmdsSendVar.wIdx;

    SYSCMDS_ASSERT(wIdx-rIdx <= SYS_CMD_SEND_FIFO_SIZE,
                   "Invalid syscmds send fifo indexes: wIdx=0x%x, rIdx=0x%x", wIdx, rIdx);
    SYSCMDS_ASSERT(len <= SYS_CMD_IRQ_SEND_PARAM_MAX_SIZE,
                   "Invalid syscmds len in irq send: %d", len);

    if (wIdx - rIdx == SYS_CMD_SEND_FIFO_SIZE)
    {
        hal_TracehalIRQInfo();
        HAL_SYS_GET_K1(k1);
        if (k1 != 0)//if send event from interuupt, don't need wait the trace flush, we can get info from back trace
        {
            // Interrupt or startup context
            hal_TimDelay(5);
        }
        else
        {
            // Task context
            sxr_Sleep(500);
        }
        SYSCMDS_ASSERT(FALSE, "Syscmds sending fifo full");
    }

    // FIFO index
    wIdx %= SYS_CMD_SEND_FIFO_SIZE;
    // Save the msg in FIFO
    g_syscmdsSendVar.sendFifo[wIdx].modId = modId;
    g_syscmdsSendVar.sendFifo[wIdx].msgId = msgId;
    if (len > 0)
    {
        memcpy(g_syscmdsSendVar.sendFifo[wIdx].param, param, len);
    }
    g_syscmdsSendVar.sendFifo[wIdx].paramLen = len;
    // Update write index;
    g_syscmdsSendVar.wIdx++;
    // Wakeup syscmds task
    syscmds_WakeupTask();

    hal_SysExitCriticalSection(scStatus);
}

PROTECTED VOID syscmds_Reply(UINT16 modId, UINT32 reqId, UINT32 retVal,
                             UINT16 msgId, VOID *param, UINT16 len)
{
    sxr_TakeSemaphore(g_syscmdsSendSema);

    syscmds_SendSingleCmd(modId, reqId, retVal,
                          msgId|SYS_CMD_REPLY_MSG_FLAG, param, len);

    sxr_ReleaseSemaphore(g_syscmdsSendSema);
}

PUBLIC VOID syscmds_Open(VOID)
{
    SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0, "syscmds_Open");

    hal_ApCommOpen(SYS_CMD_VERSION_NUMBER);

    g_syscmdsDecVar.state = SYSCMDS_DEC_STATE_IDLE;
    g_syscmdsDecVar.rIdx = 0;
    g_syscmdsDecVar.wIdx = 0;

    g_syscmdsSendVar.rIdx = 0;
    g_syscmdsSendVar.wIdx = 0;

    g_syscmdsSendSema = sxr_NewSemaphore(1);

    g_syscmdsTaskId = sxr_NewTask(&gc_syscmdsTaskDesc);
    sxr_StartTask(g_syscmdsTaskId, NULL);

    hal_ApCommSetSysCmdRecvCallback(&syscmds_Decode);

    hal_TimRtcAlarmIrqSetModemHandler(&syscmds_SendRtcTriggerMsg);

    ap_SleepInit();
}

