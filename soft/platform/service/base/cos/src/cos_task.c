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

#include "cos.h"
#include "sxr_sbx.h"
#include "sxr_sbx.hp"
#include "sxr_tksd.h"
#include "sxr_tksd.hp"
#include "sxr_tim.h"
#include "sxr_tls.h"
#include "sxs_cksum.h"
#include "sxr_mutx.h"
#include "event.h"
#include "csw_csp.h"
#include "base_prv.h"
#include "hal_host.h"
#include <stdio.h>

#define CSW_COS_MALLOC(size) CSW_Malloc(size)

struct _TASK_HANDLE
{
    sxr_TaskDesc_t desc; // It must be the first one. So, sxr_TaskDesc_t* and TASK_HANDLE* are the same
    u32 nErrCode;
    u8 nTaskId;
    u8 nMailBoxId;
};

#if (XCPU_CACHE_MODE_WRITE_BACK == 1)
extern VOID hal_DcacheFlushAll(BOOL invalid);
#endif

extern PVOID CSW_Malloc(UINT32 nSize);
extern void BAL_MmiTask(void *);
extern void BAL_ATTask(void *);

HANDLE g_hCosMmiTask = HNULL; // the default MMI task.
HANDLE g_hCosATTask = HNULL;  // the default MMI task.

HANDLE COS_GetDefaultMmiTaskHandle(UINT16 nUTI)
{
#ifdef ONLY_AT_SUPPORT
    COS_Assert(g_hCosATTask != HNULL, "ATTask can't be NULL");
    return g_hCosATTask;
#elif defined ONLY_MMI_SUPPORT
    COS_Assert(g_hCosMmiTask != HNULL, "MmiTask can't be NULL");
    return g_hCosMmiTask;
#else
    if (nUTI < CFW_AT_UTI_RANGE)
    {
        COS_Assert(g_hCosATTask != HNULL, "ATTask can't be NULL");
        return g_hCosATTask;
    }
    else
    {
        COS_Assert(g_hCosMmiTask != HNULL, "MmiTask can't be NULL");
        return g_hCosMmiTask;
    }
#endif
}

HANDLE COS_GetDefaultATTaskHandle(VOID)
{
    COS_Assert(g_hCosATTask != HNULL, "ATTask can't be NULL");
    return g_hCosATTask;
}

//
// Create task. Apply to upper layer MMI.
//
HANDLE COS_CreateTask(PTASK_ENTRY pTaskEntry,
                      PVOID pParameter,
                      PVOID pStackAddr,
                      UINT16 nStackSize,
                      UINT8 nPriority,
                      UINT16 nCreationFlags,
                      UINT16 nTimeSlice,
                      PCSTR pTaskName)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)CSW_COS_MALLOC(SIZEOF(TASK_HANDLE));
    if (pHTask == NULL)
        return NULL;

    pHTask->desc.Name = (pTaskName == NULL) ? (const ascii *)"noname" : (const ascii *)pTaskName;
    pHTask->desc.Priority = nPriority;
    pHTask->desc.StackSize = nStackSize;
    pHTask->desc.TaskBody = pTaskEntry;
    pHTask->desc.TaskExit = NULL;
    pHTask->nErrCode = 0;
    pHTask->nMailBoxId = sxr_NewMailBox();
    pHTask->nTaskId = sxr_NewTask(&pHTask->desc);

    if ((nCreationFlags & COS_CREATE_SUSPENDED) == 0)
        sxr_StartTask(pHTask->nTaskId, pParameter);

#ifndef ONLY_AT_SUPPORT
    if ((UINT32)pTaskEntry == (UINT32)BAL_MmiTask)
        g_hCosMmiTask = (HANDLE)pHTask;
#endif

#ifndef ONLY_MMI_SUPPORT
    if ((UINT32)pTaskEntry == (UINT32)BAL_ATTask)
        g_hCosATTask = (HANDLE)pHTask;
#endif

    return (HANDLE)pHTask;
}

//
// Start task. Start a suspended task(never start over).
//
VOID COS_StartTask(TASK_HANDLE *pHTask, PVOID pParameter)
{
    sxr_StartTask(pHTask->nTaskId, pParameter);
}

//
// Stop task.
//
VOID COS_StopTask(TASK_HANDLE *pHTask)
{
    sxr_StopTask(pHTask->nTaskId);
}

//
// Create task, only apply to CSW internal.
//
HANDLE COS_CreateTask_Prv(PTASK_ENTRY pTaskEntry,
                          PVOID pParameter,
                          PVOID pStackAddr,
                          UINT16 nStackSize,
                          UINT8 nPriority,
                          UINT16 nCreationFlags,
                          UINT16 nTimeSlice,
                          PCSTR pTaskName)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)CSW_COS_MALLOC(SIZEOF(TASK_HANDLE));
    if (pHTask == NULL)
        return NULL;

    pHTask->desc.Name = (pTaskName == NULL) ? (const ascii *)"noname" : (const ascii *)pTaskName;
    pHTask->desc.Priority = nPriority;
    pHTask->desc.StackSize = nStackSize;
    pHTask->desc.TaskBody = pTaskEntry;
    pHTask->desc.TaskExit = NULL;
    pHTask->nMailBoxId = sxr_NewMailBox();
    pHTask->nTaskId = sxr_NewTask(&pHTask->desc);

    if ((nCreationFlags & COS_CREATE_SUSPENDED) == 0)
        sxr_StartTask(pHTask->nTaskId, pParameter);

    return (HANDLE)pHTask;
}

//
// Get current task handle.
//
HANDLE COS_GetCurrentTaskHandle(void)
{
    return (HANDLE)sxr_GetCurrentTaskDesc();
}

//
// Delete Task. Release Mailbox, task, and the handle.
//
BOOL COS_DeleteTask(HANDLE hTask)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;

    sxr_FreeTask(pHTask->nTaskId);
    sxr_FreeMailBox(pHTask->nMailBoxId);
    return FALSE;
}

//
// Suspsend Task.
//
BOOL COS_SuspendTask(HANDLE hTask)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    sxr_SuspendTask(pHTask->nTaskId);
    return TRUE;
}

//
// Resume task.
//
BOOL COS_ResumeTask(HANDLE hTask)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    sxr_ResumeTask(pHTask->nTaskId);
    return TRUE;
}

void COS_Assert(BOOL expected, const char *format, ...)
{
    if (!expected)
    {
        char buf[128];
        va_list ap;

        va_start(ap, format);
        vsnprintf(buf, sizeof(buf) - 1, format, ap);
        va_end(ap);

        hal_SysEnterCriticalSection();
#if (XCPU_CACHE_MODE_WRITE_BACK == 1)
        hal_DcacheFlushAll(FALSE);
#endif
        hal_HstSendEvent(0xa55e47);
        hal_HstSendEvent((uint32_t)buf);

        hal_HstTraceTxFlush(HAL_TICK1S);
        asm("break 1");
    }
}

BOOL COS_WaitEvent(HANDLE hTask, COS_EVENT *pEvent, UINT32 nTimeOut)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;

    if (pHTask == NULL || pEvent == NULL)
        return FALSE;

    if (nTimeOut != COS_WAIT_FOREVER)
        return FALSE; // not implemented now

    for (;;)
    {
        UINT32 Evt[4] = {};
        sxr_Wait(Evt, pHTask->nMailBoxId);

        if (Evt[0] == EV_COS_CALLBACK || Evt[0] == EV_COS_TIMER_CALLBACK)
        {
            COS_CALLBACK_FUNC_T callback = (COS_CALLBACK_FUNC_T)(Evt[1]);
            if (callback != NULL)
                callback((void *)Evt[2]);
            continue;
        }

        if (Evt[0] == EV_COS_CALLBACK_SYNC)
        {
            COS_CALLBACK_FUNC_T callback = (COS_CALLBACK_FUNC_T)(Evt[1]);
            if (callback != NULL)
                callback((void *)Evt[2]);
            COS_SemaRelease((COS_SEMA *)Evt[3]);
            continue;
        }

        if (Evt[0])
        {
            if (Evt[0] >= HVY_TIMER_IN)
            {
                if (Evt[1] >> 31)
                    sxs_StartTimer(Evt[1] & 0x7FFFFFFF, Evt[0], (VOID *)Evt[1], FALSE, pHTask->nMailBoxId);

                pEvent->nEventId = EV_TIMER;
                pEvent->nParam1 = Evt[0] - HVY_TIMER_IN;
                // NOTE: HVY_TIMER_IN is abused by application, and Evt[2/3] may
                //       contain application user data.
            }
            else
            {
                pEvent->nEventId = Evt[0];
                pEvent->nParam1 = Evt[1];
            }
            pEvent->nParam2 = Evt[2];
            pEvent->nParam3 = Evt[3];
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    return FALSE; // never reach
}

UINT32 COS_TaskMailBoxId(HANDLE hTask)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;

    if (pHTask == NULL)
        return SXR_NO_ID;
    return pHTask->nMailBoxId;
}

UINT32 COS_TaskEventCount(HANDLE hTask)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;

    if (pHTask == NULL)
        return FALSE;

    sxr_SbxInfo_t sbxinfo = {};
    sxr_GetSbxInfo(pHTask->nMailBoxId, &sbxinfo);
    return sbxinfo.NbAvailMsg;
}

//
// Send message to the specified task.
//
BOOL COS_SendEvent(HANDLE hTask, COS_EVENT *pEvent, UINT32 nTimeOut, UINT16 nOption)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;

    if (pHTask == NULL || pEvent == NULL)
        return FALSE;

    if (nTimeOut == COS_WAIT_FOREVER)
    {
        UINT8 status = SXR_SEND_EVT;
        if (nOption & COS_EVENT_PRI_URGENT)
            status |= SXR_QUEUE_FIRST;

        sxr_Send(pEvent, pHTask->nMailBoxId, status);
        return TRUE;
    }

    return FALSE;
}

#define GET_SYSTASK_QUEUE_ID() (((TASK_HANDLE *)BAL_TH(BAL_SYS_TASK_PRIORITY))->nMailBoxId)

//
// Send message to SYS_Task.
//
BOOL COS_SendSysEvent(COS_EVENT *pEvent, UINT16 nOption)
{
    UINT8 status = SXR_SEND_EVT;

    if (nOption & COS_EVENT_PRI_URGENT)
        status |= SXR_QUEUE_FIRST;

    sxr_Send(pEvent, GET_SYSTASK_QUEUE_ID(), status);
    return TRUE;
}

BOOL COS_ResetEventQueue(HANDLE hTask)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;

    if (pHTask == NULL)
        return FALSE;

    sxr_RmvAllMsgFromMbx(pHTask->nMailBoxId);
    return TRUE;
}

BOOL COS_IsEventAvailable(HANDLE hTask)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;

    if (pHTask == NULL)
        return FALSE;

    return (sxr_SbxHot(pHTask->nMailBoxId)) ? TRUE : FALSE;
}

BOOL COS_FreeEnvCountAvail(UINT32 count)
{
    return sxr_FreeEnvCountAvail(count);
}

//
// Start a timer with time-out value.
//
BOOL COS_SetTimer(HANDLE hTask,
                  UINT8 nTimerId,
                  UINT8 nMode,
                  UINT32 nElapse)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    UINT8 nMailBoxId = 0;

    if (nElapse > 0x7FFFFFFF)
        return FALSE;

    if ((UINT32)pHTask == (HANDLE)0xFFFFFFFF)
    {
        nMailBoxId = 11;
    }
    else
    {
        nMailBoxId = pHTask->nMailBoxId;
    }

    if (pHTask)
    {
        if (nMailBoxId)
        {
            BOOL bMode = (nMode == COS_TIMER_MODE_SINGLE) ? FALSE : TRUE;
            //sxs_StartTimer ( nElapse*15, HVY_TIMER_IN+nTimerId, (VOID*)(bMode<<31|nElapse), FALSE, nMailBoxId );
            sxs_StartTimer(nElapse, HVY_TIMER_IN + nTimerId, (VOID *)(bMode << 31 | nElapse), FALSE, nMailBoxId);

            return TRUE;
        }
    }

    return FALSE;
}

BOOL COS_SetTimerEX(HANDLE hTask,
                    UINT16 nTimerId, //nTimerId should small than 0x1000,
                    UINT8 nMode,
                    UINT32 nElapse)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    UINT8 nMailBoxId = 0;
    if (nTimerId > 0x1000)
    {
        return FALSE;
    }

    if (nElapse > 0x7FFFFFFF)
        return FALSE;

    if ((UINT32)pHTask == (HANDLE)0xFFFFFFFF)
    {
        nMailBoxId = 11;
    }
    else
    {
        nMailBoxId = pHTask->nMailBoxId;
    }

    if (pHTask)
    {
        if (nMailBoxId)
        {
            BOOL bMode = (nMode == COS_TIMER_MODE_SINGLE) ? FALSE : TRUE;
            //sxs_StartTimer ( nElapse*15, HVY_TIMER_IN+nTimerId, (VOID*)(bMode<<31|nElapse), FALSE, nMailBoxId );
            sxs_StartTimer(nElapse, HVY_TIMER_IN + nTimerId, (VOID *)(bMode << 31 | nElapse), FALSE, nMailBoxId);
            return TRUE;
        }
    }

    return FALSE;
}

//
// Stop and Realse Timer
//
BOOL COS_KillTimer(HANDLE hTask, UINT8 nTimerId)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    UINT8 nMailBoxId = 0;

    if ((UINT32)pHTask == (HANDLE)0xFFFFFFFF)
    {
        nMailBoxId = 11;
    }
    else
    {
        nMailBoxId = pHTask->nMailBoxId;
    }

    if (pHTask)
    {
        if (nMailBoxId)
        {
            sxs_StopTimer(HVY_TIMER_IN + nTimerId, NULL, nMailBoxId);
            return TRUE;
        }
    }

    return FALSE;
}

BOOL COS_KillTimerEX(HANDLE hTask, UINT16 nTimerId)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    UINT8 nMailBoxId = 0;

    if (nTimerId > 0x1000)
    {
        return FALSE;
    }
    if ((UINT32)pHTask == (HANDLE)0xFFFFFFFF)
    {
        nMailBoxId = 11;
    }
    else
    {
        nMailBoxId = pHTask->nMailBoxId;
    }

    if (pHTask)
    {
        if (nMailBoxId)
        {
            sxs_StopTimer(HVY_TIMER_IN + nTimerId, NULL, nMailBoxId);
            return TRUE;
        }
    }

    return FALSE;
}

BOOL COS_ForceDeliverTimerEx(HANDLE hTask, UINT16 nTimerId)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    if (pHTask == NULL)
        return FALSE;

    sxs_StopTimer(HVY_TIMER_IN + nTimerId, NULL, pHTask->nMailBoxId);

    COS_EVENT event = {};
    event.nEventId = EV_TIMER;
    event.nParam1 = nTimerId;
    sxr_Send(&event, pHTask->nMailBoxId, SXR_SEND_EVT);
    return TRUE;
}

UINT32 COS_QueryTimer(HANDLE hTask, UINT16 nTimerId)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    UINT8 nMailBoxId = 0;
    sxr_Timer_t Timer = {0, 0xFFFFFFFF, 0, 0};
    Timer.Ctx = nTimerId;
    UINT32 TotalPeriod = 0;

    if ((UINT32)pHTask == (HANDLE)0xFFFFFFFF)
    {
        nMailBoxId = 11;
    }
    else
    {
        nMailBoxId = pHTask->nMailBoxId;
    }

    if (pHTask)
    {
        if (nMailBoxId)
        {
            TotalPeriod = sxr_TimerQuery(&Timer, SXR_REGULAR_TIMER);
            return TotalPeriod;
        }
    }

    return TotalPeriod;
}

UINT32 COS_QueryTimerEX(HANDLE hTask, UINT16 nTimerId)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    UINT8 nMailBoxId = 0;
    sxr_Timer_t Timer = {0, 0xFFFFFFFF, 0, 0};
    Timer.Ctx = nTimerId;
    UINT32 TotalPeriod = 0;

    if (nTimerId > 0x1000)
    {
        return TotalPeriod;
    }
    if ((UINT32)pHTask == (HANDLE)0xFFFFFFFF)
    {
        nMailBoxId = 11;
    }
    else
    {
        nMailBoxId = pHTask->nMailBoxId;
    }

    if (pHTask)
    {
        if (nMailBoxId)
        {
            TotalPeriod = sxr_TimerQuery(&Timer, SXR_REGULAR_TIMER);
            return TotalPeriod;
        }
    }

    return TotalPeriod;
}

// =============================================================================
// COS_TaskCallback
// =============================================================================
BOOL COS_TaskCallback(HANDLE hTask, COS_CALLBACK_FUNC_T callback, void *param)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    if (hTask == NULL || callback == NULL)
        return FALSE;

    UINT8 nMailBoxId = pHTask->nMailBoxId;
    if (nMailBoxId == 0)
        return FALSE;

    COS_EVENT event = {};
    event.nEventId = EV_COS_CALLBACK;
    event.nParam1 = (UINT32)callback;
    event.nParam2 = (UINT32)param;
    sxr_Send(&event, nMailBoxId, SXR_SEND_EVT);
    return TRUE;
}

// =============================================================================
// COS_TaskCallbackNotif
// =============================================================================
BOOL COS_TaskCallbackNotif(HANDLE hTask, COS_CALLBACK_FUNC_T callback, void *param)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    if (hTask == NULL || callback == NULL)
        return FALSE;

    UINT8 nMailBoxId = pHTask->nMailBoxId;
    if (nMailBoxId == 0)
        return FALSE;

    UINT32 sc = hal_SysEnterCriticalSection();
    COS_EVENT event = {};
    event.nEventId = EV_COS_CALLBACK;
    event.nParam1 = (UINT32)callback;
    event.nParam2 = (UINT32)param;
    if (!sxr_CheckMsgInMbx(&event, TRUE, nMailBoxId, 3))
        sxr_Send(&event, nMailBoxId, SXR_SEND_EVT);
    hal_SysExitCriticalSection(sc);
    return TRUE;
}

// =============================================================================
// COS_StopTaskCallback
// =============================================================================
BOOL SRVAPI COS_StopTaskCallback(HANDLE hTask, COS_CALLBACK_FUNC_T callback, void *param)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    if (hTask == NULL)
        return FALSE;

    UINT8 nMailBoxId = pHTask->nMailBoxId;
    if (nMailBoxId == 0)
        return FALSE;

    UINT32 sc = hal_SysEnterCriticalSection();
    COS_EVENT event = {};
    event.nEventId = EV_COS_CALLBACK;
    event.nParam1 = (UINT32)callback;
    event.nParam2 = (UINT32)param;
    sxr_RmvMsgFromMbx(&event, TRUE, nMailBoxId, 3);
    hal_SysExitCriticalSection(sc);
    return TRUE;
}

// =============================================================================
// COS_IsTaskCallbackSet
// =============================================================================
BOOL SRVAPI COS_IsTaskCallbackSet(HANDLE hTask, COS_CALLBACK_FUNC_T callback, void *param)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    if (hTask == NULL)
        return FALSE;

    UINT8 nMailBoxId = pHTask->nMailBoxId;
    if (nMailBoxId == 0)
        return FALSE;

    UINT32 sc = hal_SysEnterCriticalSection();
    COS_EVENT event = {};
    event.nEventId = EV_COS_CALLBACK;
    event.nParam1 = (UINT32)callback;
    event.nParam2 = (UINT32)param;
    BOOL existed = sxr_CheckMsgInMbx(&event, TRUE, nMailBoxId, 3);
    hal_SysExitCriticalSection(sc);
    return existed;
}

// =============================================================================
// COS_IsTaskCallbackSet
// =============================================================================
BOOL COS_TaskCallbackSync(HANDLE hTask, COS_CALLBACK_FUNC_T callback, void *param)
{
    if (hTask == COS_GetCurrentTaskHandle())
    {
        if (callback != NULL)
            callback(param);
        return TRUE;
    }

    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    if (hTask == NULL)
        return FALSE;

    UINT8 nMailBoxId = pHTask->nMailBoxId;
    if (nMailBoxId == 0)
        return FALSE;

    COS_SEMA sema;
    COS_SemaInit(&sema, 0);

    COS_EVENT event = {};
    event.nEventId = EV_COS_CALLBACK_SYNC;
    event.nParam1 = (UINT32)callback;
    event.nParam2 = (UINT32)param;
    event.nParam3 = (UINT32)&sema;
    sxr_Send(&event, nMailBoxId, SXR_SEND_EVT);

    COS_SemaTake(&sema);
    COS_SemaDestroy(&sema);
    return TRUE;
}

// =============================================================================
// COS_TaskWaitEventHandled
// =============================================================================
BOOL COS_TaskWaitEventHandled(HANDLE hTask)
{
    return COS_TaskCallbackSync(hTask, NULL, NULL);
}

// =============================================================================
// COS_StartTimerCallback
// =============================================================================
BOOL SRVAPI COS_StartCallbackTimer(HANDLE hTask, UINT32 ms, COS_CALLBACK_FUNC_T callback, void *param)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    if (hTask == NULL || callback == NULL)
        return FALSE;

    UINT8 nMailBoxId = pHTask->nMailBoxId;
    if (nMailBoxId == 0)
        return FALSE;

    UINT32 evt[SXR_TIMER_CTX_SIZE] = {};
    evt[0] = EV_COS_TIMER_CALLBACK;
    evt[1] = (UINT32)callback;
    evt[2] = (UINT32)param;
    UINT32 sc = hal_SysEnterCriticalSection();
    sxr_StopMbxTimer(evt, nMailBoxId, SXR_REGULAR_TIMER);
    sxr_StartMbxTimer(COS_Msec2Tick(ms), evt, nMailBoxId, SXR_REGULAR_TIMER);
    hal_ExitCriticalSection(sc);
    return TRUE;
}

// =============================================================================
// COS_StartCallbackTimerForcedly
// =============================================================================
BOOL SRVAPI COS_StartCallbackTimerForcedly(HANDLE hTask, UINT32 ms, COS_CALLBACK_FUNC_T callback, void *param)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    if (hTask == NULL || callback == NULL)
        return FALSE;

    UINT8 nMailBoxId = pHTask->nMailBoxId;
    if (nMailBoxId == 0)
        return FALSE;

    UINT32 evt[SXR_TIMER_CTX_SIZE] = {};
    evt[0] = EV_COS_TIMER_CALLBACK;
    evt[1] = (UINT32)callback;
    evt[2] = (UINT32)param;
    sxr_StartMbxTimer(COS_Msec2Tick(ms), evt, nMailBoxId, SXR_REGULAR_TIMER);
    return TRUE;
}

// =============================================================================
// COS_StartCallbackTimerCheck
// =============================================================================
BOOL SRVAPI COS_StartCallbackTimerCheck(HANDLE hTask, UINT32 ms, COS_CALLBACK_FUNC_T callback, void *param)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    if (hTask == NULL || callback == NULL)
        return FALSE;

    UINT8 nMailBoxId = pHTask->nMailBoxId;
    if (nMailBoxId == 0)
        return FALSE;

    UINT32 evt[SXR_TIMER_CTX_SIZE] = {};
    evt[0] = EV_COS_TIMER_CALLBACK;
    evt[1] = (UINT32)callback;
    evt[2] = (UINT32)param;
    UINT32 sc = hal_SysEnterCriticalSection();
    if (!COS_IsCallbackTimerSet(hTask, callback, param))
        sxr_StartMbxTimer(COS_Msec2Tick(ms), evt, nMailBoxId, SXR_REGULAR_TIMER);
    hal_ExitCriticalSection(sc);
    return TRUE;
}

// =============================================================================
// COS_StopCallbackTimer
// =============================================================================
BOOL SRVAPI COS_StopCallbackTimer(HANDLE hTask, COS_CALLBACK_FUNC_T callback, void *param)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    if (hTask == NULL)
        return FALSE;

    UINT8 nMailBoxId = pHTask->nMailBoxId;
    if (nMailBoxId == 0)
        return FALSE;

    UINT32 evt[SXR_TIMER_CTX_SIZE] = {};
    evt[0] = EV_COS_TIMER_CALLBACK;
    evt[1] = (UINT32)callback;
    evt[2] = (UINT32)param;
    sxr_StopMbxTimer(evt, nMailBoxId, SXR_REGULAR_TIMER);
    return TRUE;
}

// =============================================================================
// COS_QueryTimerCallback
// =============================================================================
UINT32 SRVAPI COS_QueryCallbackTimer(HANDLE hTask, COS_CALLBACK_FUNC_T callback, void *param)
{
    sxr_TimerEx_t Timer = {};
    Timer.Id = SXR_MBX_TIMER;
    Timer.IdMsk = SXR_MBX_TIMER;
    Timer.Ctx[0] = EV_COS_TIMER_CALLBACK;
    Timer.Ctx[1] = (UINT32)callback;
    Timer.Ctx[2] = (UINT32)param;
    Timer.Msk[0] = Timer.Msk[1] = Timer.Msk[2] = 0xFFFFFFFF;
    return sxr_TimerExQuery(&Timer, SXR_REGULAR_TIMER);
}

// =============================================================================
// COS_IsCallbackTimerSet
// =============================================================================
BOOL SRVAPI COS_IsCallbackTimerSet(HANDLE hTask, COS_CALLBACK_FUNC_T callback, void *param)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)hTask;
    if (hTask == NULL)
        return FALSE;

    UINT8 nMailBoxId = pHTask->nMailBoxId;
    if (nMailBoxId == 0)
        return FALSE;

    UINT32 sc = hal_SysEnterCriticalSection();
    sxr_TimerEx_t Timer = {};
    Timer.Id = SXR_MBX_TIMER;
    Timer.IdMsk = SXR_MBX_TIMER;
    Timer.Ctx[0] = EV_COS_TIMER_CALLBACK;
    Timer.Ctx[1] = (UINT32)callback;
    Timer.Ctx[2] = (UINT32)param;
    Timer.Msk[0] = Timer.Msk[1] = Timer.Msk[2] = 0xFFFFFFFF;
    BOOL existed = sxr_IsTimerExSet(&Timer, SXR_REGULAR_TIMER) ||
                   sxr_CheckMsgInMbx(&Timer.Ctx[0], TRUE, nMailBoxId, SXR_TIMER_CTX_SIZE);
    hal_SysExitCriticalSection(sc);
    return existed;
}

// =============================================================================
// COS_IsFunctionTimerSet
// =============================================================================
BOOL COS_IsFunctionTimerSet(COS_CALLBACK_FUNC_T callback, void *param)
{
    sxr_TimerEx_t Timer = {};
    Timer.Id = SXR_FUNCTION_TIMER;
    Timer.IdMsk = SXR_FUNCTION_TIMER;
    Timer.Ctx[0] = (UINT32)callback;
    Timer.Ctx[1] = (UINT32)param;
    Timer.Msk[0] = Timer.Msk[1] = 0xFFFFFFFF;
    return sxr_IsTimerExSet(&Timer, SXR_REGULAR_TIMER);
}

// =============================================================================
// COS_StartFunctionTimer
// =============================================================================
VOID COS_StartFunctionTimer(UINT32 ms, COS_CALLBACK_FUNC_T callback, void *param)
{
    UINT32 sc = hal_SysEnterCriticalSection();
    sxr_StopFunctionTimer2(callback, param);
    sxr_StartFunctionTimer(COS_Msec2Tick(ms), callback, param, 0);
    hal_SysExitCriticalSection(sc);
}

// =============================================================================
// COS_StartFunctionTimerForcedly
// =============================================================================
VOID COS_StartFunctionTimerForcedly(UINT32 ms, COS_CALLBACK_FUNC_T callback, void *param)
{
    sxr_StartFunctionTimer(COS_Msec2Tick(ms), callback, param, 0);
}

// =============================================================================
// COS_StartFunctionTimerCheck
// =============================================================================
VOID COS_StartFunctionTimerCheck(UINT32 ms, COS_CALLBACK_FUNC_T callback, void *param)
{
    UINT32 sc = hal_SysEnterCriticalSection();
    if (!COS_IsFunctionTimerSet(callback, param))
        sxr_StartFunctionTimer(COS_Msec2Tick(ms), callback, param, 0);
    hal_ExitCriticalSection(sc);
}

// =============================================================================
// COS_StopFunctionTimer
// =============================================================================
VOID COS_StopFunctionTimer(COS_CALLBACK_FUNC_T callback, void *param)
{
    sxr_StopFunctionTimer2(callback, param);
}

//
// Sleep current task in nMillisecondes
//
BOOL COS_Sleep(UINT32 nMillisecondes)
{
    sxr_Sleep(nMillisecondes * 16384 / 1000);
    return TRUE;
}

HANDLE COS_EnterCriticalSection(VOID)
{
    return (HANDLE)hal_EnterCriticalSection();
}

BOOL COS_ExitCriticalSection(HANDLE hSection)
{
    hal_ExitCriticalSection(hSection);
    return TRUE;
}

HANDLE SRVAPI COS_CreateSemaphore(UINT32 nInitCount) // Specify the initial count of the semaphore    sxr_NewSemaphore
{
    UINT8 id = sxr_NewSemaphore((UINT8)nInitCount);
    return (HANDLE)id;
}

BOOL SRVAPI COS_DeleteSemaphore(HANDLE hSem)
{
    if (hSem == 0)
        return FALSE;

    sxr_FreeSemaphore(hSem);
    return TRUE;
}

BOOL SRVAPI COS_WaitForSemaphore(HANDLE hSem,     // Specify the handle to a counting semaphore
                                 UINT32 nTimeOut) // the time-out value
{
    if (hSem == 0)
        return FALSE;

    sxr_TakeSemaphore(hSem);
    return TRUE;
}

BOOL SRVAPI COS_ReleaseSemaphore(HANDLE hSem) //Specify the counting semaphore
{
    if (hSem == 0)
        return FALSE;

    sxr_ReleaseSemaphore(hSem);
    return TRUE;
}

UINT32 COS_GetLastError(VOID)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)COS_GetCurrentTaskHandle();
    if (pHTask == NULL)
        return 0;

    return pHTask->nErrCode;
}

VOID COS_SetLastError(UINT32 nErrCode)
{
    TASK_HANDLE *pHTask = (TASK_HANDLE *)COS_GetCurrentTaskHandle();
    if (pHTask != NULL)
        pHTask->nErrCode = nErrCode;
}

HANDLE SRVAPI COS_CreateMutex(void)
{
    return (HANDLE)sxr_NewMutex();
}

void SRVAPI COS_DeleteMutex(HANDLE mutex)
{
    sxr_FreeMutex(mutex);
}

void SRVAPI COS_LockMutex(HANDLE mutex)
{
    sxr_TakeMutex(mutex);
}

void SRVAPI COS_UnlockMutex(HANDLE mutex)
{
    sxr_ReleaseMutex(mutex, sxr_GetCurrentTaskId());
}

UINT32 COS_cksum(UINT8 *addr, UINT32 length)
{
    return sxs_cksum(addr, length);
}

// =============================================================================
// COS_Msec2Tick (ms -> tick)
// =============================================================================
unsigned COS_Msec2Tick(unsigned ms)
{
    return (ms * HAL_TICK1S) / 1000;
}

// =============================================================================
// COS_Sec2Tick (second -> tick)
// =============================================================================
unsigned COS_Sec2Tick(unsigned sec)
{
    return sec * HAL_TICK1S;
}

// =============================================================================
// COS_Tick2Msec (tick -> ms)
// =============================================================================
unsigned COS_Tick2Msec(unsigned tick)
{
    return (tick * 1000) / HAL_TICK1S;
}

// =============================================================================
// COS_GetTickCount
// =============================================================================
unsigned COS_GetTickCount(void)
{
    return hal_TimGetUpTime();
}

// =============================================================================
// COS_Printf
// =============================================================================
void COS_Printf(UINT32 id, const char *fmt, ...)
{
    char buf[256];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
    va_end(ap);

    sxs_fprintf(id, buf);
}

// =============================================================================
// COS_SemaInited
// =============================================================================
BOOL COS_SemaInited(COS_SEMA *sema)
{
    return (sema->mbx == 0 || sema->mbx == 0xff) ? FALSE : TRUE;
}

// =============================================================================
// COS_SemaInit
// =============================================================================
VOID COS_SemaInit(COS_SEMA *sema, UINT32 init)
{
    COS_SemaDestroy(sema);

    sema->count = init;
    sema->mbx = sxr_NewMailBox();
}

// =============================================================================
// COS_SemaTake
// =============================================================================
VOID COS_SemaTake(COS_SEMA *sema)
{
    if (sema->mbx == 0 || sema->mbx == 0xff)
        return;

    UINT32 sc = hal_SysEnterCriticalSection();
    for (;;)
    {
        if (sema->count > 0)
        {
            sema->count--;
            hal_SysExitCriticalSection(sc);
            return;
        }
        sxr_TaskWaitToQueue(sema->mbx);
    }
    hal_SysExitCriticalSection(sc);
}

// =============================================================================
// COS_SemaTimeout (for function timer)
// =============================================================================
static void COS_SemaTimeout(void *param)
{
    UINT8 mbx = (UINT32)param & 0xff;
    UINT8 task = ((UINT32)param >> 8) & 0xff;
    sxr_WakeupTaskFromQueue(task, mbx);
}

// =============================================================================
// COS_SemaTryTake
// =============================================================================
BOOL COS_SemaTryTake(COS_SEMA *sema, UINT32 ms)
{
    if (sema->mbx == 0 || sema->mbx == 0xff)
        return FALSE;

    UINT32 sc = hal_SysEnterCriticalSection();
    if (sema->count > 0)
        goto passed;
    if (ms == 0)
        goto failed;

    UINT32 param = (sxr_GetCurrentTaskId() << 8) | sema->mbx;
    sxr_StartFunctionTimer((ms * HAL_TICK1S) / 1000, COS_SemaTimeout, (void *)param, 0);
    sxr_TaskWaitToQueue(sema->mbx);
    sxr_StopFunctionTimer2(COS_SemaTimeout, (void *)param);

    if (sema->count > 0)
        goto passed;

failed:
    hal_SysExitCriticalSection(sc);
    return FALSE;

passed:
    sema->count--;
    hal_SysExitCriticalSection(sc);
    return TRUE;
}

// =============================================================================
// COS_SemaRelease
// =============================================================================
VOID COS_SemaRelease(COS_SEMA *sema)
{
    if (sema->mbx == 0 || sema->mbx == 0xff)
        return;

    UINT32 sc = hal_SysEnterCriticalSection();
    sema->count++;
    sxr_WakeupFirstTaskFromQueue(sema->mbx);
    hal_SysExitCriticalSection(sc);
}

// =============================================================================
// COS_SemaDestroy
// =============================================================================
VOID COS_SemaDestroy(COS_SEMA *sema)
{
    if (sema->mbx == 0 || sema->mbx == 0xff)
        return;

    sxr_FreeMailBox(sema->mbx);
    sema->mbx = 0;
}

// =============================================================================
// COS_MutexInited
// =============================================================================
BOOL COS_MutexInited(COS_MUTEX *mutex)
{
    return (mutex->mbx == 0 || mutex->mbx == 0xff) ? FALSE : TRUE;
}

// =============================================================================
// COS_MutexInit
// =============================================================================
VOID COS_MutexInit(COS_MUTEX *mutex)
{
    COS_MutexDestroy(mutex);

    mutex->count = 0;
    mutex->task = SXR_NO_TASK;
    mutex->mbx = sxr_NewMailBox();
}

// =============================================================================
// COS_MutexLock
// =============================================================================
VOID COS_MutexLock(COS_MUTEX *mutex)
{
    if (mutex->mbx == 0 || mutex->mbx == 0xff)
        return;

    UINT32 sc = hal_SysEnterCriticalSection();
    for (;;)
    {
        if (mutex->count == 0 || mutex->task == sxr_GetCurrentTaskId())
        {
            mutex->count++;
            mutex->task = sxr_GetCurrentTaskId();
            hal_SysExitCriticalSection(sc);
            return;
        }
        sxr_TaskWaitToQueue(mutex->mbx);
    }
    hal_SysExitCriticalSection(sc);
}

// =============================================================================
// COS_MutexTimeout (for function timer)
// =============================================================================
static void COS_MutexTimeout(void *param)
{
    UINT8 mbx = (UINT32)param & 0xff;
    UINT8 task = ((UINT32)param >> 8) & 0xff;
    sxr_WakeupTaskFromQueue(task, mbx);
}

// =============================================================================
// COS_SemaTryTake
// =============================================================================
BOOL COS_MutexTryLock(COS_MUTEX *mutex, UINT32 ms)
{
    if (mutex->mbx == 0 || mutex->mbx == 0xff)
        return FALSE;

    UINT32 sc = hal_SysEnterCriticalSection();
    if (mutex->count == 0 || mutex->task == sxr_GetCurrentTaskId())
        goto passed;
    if (ms == 0)
        goto failed;

    UINT32 param = (sxr_GetCurrentTaskId() << 8) | mutex->mbx;
    sxr_StartFunctionTimer((ms * HAL_TICK1S) / 1000, COS_MutexTimeout, (void *)param, 0);
    sxr_TaskWaitToQueue(mutex->mbx);
    sxr_StopFunctionTimer2(COS_SemaTimeout, (void *)param);

    if (mutex->count == 0 || mutex->task == sxr_GetCurrentTaskId())
        goto passed;

failed:
    hal_SysExitCriticalSection(sc);
    return FALSE;

passed:
    mutex->count++;
    mutex->task = sxr_GetCurrentTaskId();
    hal_SysExitCriticalSection(sc);
    return TRUE;
}

// =============================================================================
// COS_MutexUnlock
// =============================================================================
VOID COS_MutexUnlock(COS_MUTEX *mutex)
{
    if (mutex->mbx == 0 || mutex->mbx == 0xff)
        return;

    UINT32 sc = hal_SysEnterCriticalSection();
    mutex->count--;
    if (mutex->count == 0)
        mutex->task = SXR_NO_TASK;
    sxr_WakeupFirstTaskFromQueue(mutex->mbx);
    hal_SysExitCriticalSection(sc);
}

// =============================================================================
// COS_MutexDestroy
// =============================================================================
VOID COS_MutexDestroy(COS_MUTEX *mutex)
{
    if (mutex->mbx == 0 || mutex->mbx == 0xff)
        return;

    sxr_FreeMailBox(mutex->mbx);
    mutex->mbx = 0;
}
