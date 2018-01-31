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





#include "sxs_io.h"
#include "sxs_lib.h"
#include "sxs_rmt.h"
#include "sxs_rmc.h"
#include "sxr_ops.h"
#include "sxr_tksd.hp"
#include "sxr_sbx.hp"
#if (SXR_NB_MAX_TASK < 0x80)
#include "sxr_jbsd.h" // for debug only
#endif

#include "sxp_debug.h"

#ifdef SX_PROFILING
u32 sxr_ProfileCfg =
    SX_PRFOILE_ENV_ENABLE;
//SX_PROFILE_QUEUE_ENABLE;
//SX_PRFOILE_ENV_ENABLE|SX_PROFILE_QUEUE_ENABLE;
#endif // SX_PROFILING

SemaInfo_t sxr_SemaInfo[SXR_NB_MAX_SEM];

// =============================================================================
// sxr_Send
// -----------------------------------------------------------------------------
/// Send a message or an event to a synchronization box.
/// @param Msg          Pointer onto message or event.
/// @param Sbx          Box number.
/// @param MsgStatus    Message status.
// =============================================================================
void sxr_Send (void *Msg, u8 Sbx, u8 MsgStatus)
{
    u16 IdxCur;
    u8  TaskId;
    u32 K1, CallerAdd;

    // The following must be the first execution statement in the function
    SXS_GET_RA(CallerAdd);
    HAL_SYS_GET_K1(K1);

    if (sxr_Sbx.Queue [Sbx].TrcLev > 0)
    {
        SXS_TRACE (_SXR|TSMAP(1)|TSTDOUT|TNB_ARG(2)|TDB,TSTR("%s put a message into Sbx %i\n",0x06a90001), sxr_Task.Ctx [sxr_Task.Active].Desc -> Name, Sbx);
    }

    if (!(sxr_Sbx.RmtSbx [Sbx >> 5] & (1 << (Sbx & 31))))
    {
        u32 Status = sxr_EnterSc ();

        if (sxr_Sbx.EnvIdxFree == SXR_NO_ENV)
        {
            sxr_CheckSbx();
            SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No more envelope\n",0x06a90002)));
        }

        if ((Sbx > SXR_NB_TOTAL_SBX)
                || (sxr_Sbx.Queue [Sbx].Id != SXR_SBX_ALLOCATED))
        {

            SXS_RAISE ((_SXR|TABORT|TNB_ARG(1)|TDB,TSTR("Unknown synchronization box %i\n",0x06a90003), Sbx));
        }

        IdxCur = sxr_Sbx.EnvIdxFree;
        sxr_Sbx.EnvIdxFree = sxr_Sbx.Env [IdxCur].Next;

        SX_PROFILE_ENVELOP_USE_START(IdxCur);

        // Save sender's info
#if (SXR_NB_MAX_TASK < 0x80)
        // JOB_STACK_FLAG is copied from halp_irq_handler.h
#define JOB_STACK_FLAG  0x01000000
        sxr_Sbx.Env [IdxCur].SenderTaskId =
            (K1 & JOB_STACK_FLAG) ?
            (0x80 | sxr_GetActiveJob()) :
            ((K1) ? SXR_NO_TASK : sxr_Task.Active);
#else
        sxr_Sbx.Env [IdxCur].SenderTaskId = (K1) ? SXR_NO_TASK : sxr_Task.Active;
#endif
        sxr_Sbx.Env [IdxCur].SenderAddr = CallerAdd;

        if (!(MsgStatus & SXR_SEND_EVT))
        {
            sxr_Sbx.Env [IdxCur].EvtTag = FALSE;
            sxr_Sbx.Env [IdxCur].Evt [0] = (u32)Msg;
        }
        else
        {
            u8 i;

            sxr_Sbx.Env [IdxCur].EvtTag = TRUE;

            for (i=0; i<SXR_EVT_MBX_SIZE; i++)
            {
                sxr_Sbx.Env [IdxCur].Evt [i] = ((u32 *) Msg) [i];
            }
        }

        if (sxr_Sbx.Queue [Sbx].Msg.First == SXR_NO_ENV)
        {
            sxr_Sbx.Queue [Sbx].Msg.First = IdxCur;
            sxr_Sbx.Queue [Sbx].Msg.Last  = IdxCur;
            sxr_Sbx.Env [IdxCur].Next     = SXR_NO_ENV;
        }
        else if (MsgStatus & SXR_QUEUE_FIRST)
        {
            sxr_Sbx.Env [IdxCur].Next     = sxr_Sbx.Queue [Sbx].Msg.First;
            sxr_Sbx.Queue [Sbx].Msg.First = IdxCur;
        }
        else
        {
            sxr_Sbx.Env [sxr_Sbx.Queue [Sbx].Msg.Last].Next = IdxCur;
            sxr_Sbx.Queue [Sbx].Msg.Last = IdxCur;
            sxr_Sbx.Env [IdxCur].Next    = SXR_NO_ENV;
        }

        if (sxr_Sbx.Queue [Sbx].Task.First != SXR_NO_TASK )
        {
            TaskId = sxr_Sbx.Queue [Sbx].Task.First;
            sxr_Sbx.Queue [Sbx].Task.First = sxr_Sbx.Queue [TaskId].Task.Next;
            sxr_Sbx.Queue [TaskId].Task.Next = SXR_NO_TASK;
            sxr_Task.Ctx [TaskId].State &= ~SXR_WAIT_MSG_TSK;

            sxr_WakeUpTask (TaskId);
        }

        sxr_ExitSc (Status);
    }
    else
    {
        u16 Size;
        sxs_RmtMbx_t *RmtMbx;

        if (!(MsgStatus & SXR_SEND_EVT))
        {
            if (sxr_Sbx.TxMsg)
            {
                Msg = sxr_Sbx.TxMsg (Msg);
            }

            Size = sxr_GetMemSize (Msg);
        }
        else
        {
            Size = 4 * SXR_EVT_MBX_SIZE;
        }

#ifdef SXS_RMT_SPL_CPU_IDX
        RmtMbx = (sxs_RmtMbx_t *)_sxs_SendToRemote (SXS_MBX_RMC, NIL,(u16)(Size + sizeof (sxs_RmtMbx_t)), SXS_RMT_SPL_CPU_IDX); // - 4));

        if (RmtMbx != NIL)
        {
            RmtMbx -> Mbx = Sbx;
            RmtMbx -> MsgStatus = MsgStatus;
            memcpy ((u8*)RmtMbx -> Data, Msg, Size);
        }
#else
        RmtMbx = (sxs_RmtMbx_t *)pal_TraceRequestBuf (SXS_MBX_RMC, (u16)(Size + sizeof (sxs_RmtMbx_t))); // - 4));

        if (RmtMbx != NIL)
        {
            RmtMbx -> Mbx = Sbx;
            RmtMbx -> MsgStatus = MsgStatus;
            memcpy ((u8*)RmtMbx -> Data, Msg, Size);
            pal_TraceBufFilled (RmtMbx);
        }
#endif

        if (!(MsgStatus & SXR_SEND_EVT))
        {
            sxr_Free (Msg);
        }

#ifndef __EMBEDDED__
        sxs_RmtBufferFlush ();
#endif
#ifdef __SXS_RMT_PROCESS_IDLE__
        while ( sxs_SerialFlush( ) );
#endif
    }
}



// =============================================================================
// sxr_Wait
// -----------------------------------------------------------------------------
/// Wait for a message or an event on a synchronization box.
/// @param Evt  Pointer onto Event to be received.
/// @param Sbx  Synchronization box number.
/// @return     Pointer onto message or NIL for event.
// =============================================================================
void *sxr_Wait (u32 *Evt, u8 Sbx)
{
    return sxr_WaitEx(Evt, Sbx, NULL, NULL);
}

// =============================================================================
// sxr_WaitEx
// -----------------------------------------------------------------------------
/// Wait for a message or an event on a synchronization box.
/// @param Evt  Pointer onto Event to be received.
/// @param Sbx  Synchronization box number.
/// @param SenderTaskId Pointer onto the sender's task ID.
/// @param SenderAddr Pointer onto the sender's function address.
/// @return     Pointer onto message or NIL for event.
// =============================================================================
void *sxr_WaitEx (u32 *Evt, u8 Sbx, u8 *SenderTaskId, u32 *SenderAddr)
{
#ifndef CT_NO_DEBUG
    u32 k1;
    HAL_SYS_GET_K1(k1);
    if (k1 != 0
            // Dirty workaround: BAL_TaskInit() might take semaphore. At that time
            // SX has been init but scheduling is not started yet -- k1 is not zero.
            && sxr_Task.Ctx [sxr_Task.Active].State != SXR_PENDING_TSK
       )
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1), TSTR("sxr_Wait() must run in task context! (k1=0x%08x)\n",0x06a9000a),
                    k1));
    }
    if (sxr_Task.Active == sxr_Task.Idle)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(2), TSTR("sxr_Wait() cannot run in idle task! (active=%d, idle=%d)\n",0x06a9000b),
                    sxr_Task.Active, sxr_Task.Idle));
    }
#endif // ! CT_NO_DEBUG

    u8    i;
    u16   IdxCur;
    void *Msg = NIL;
    u32   Status = sxr_EnterSc ();

    SX_PROFILE_WAIT_QUEUE_START(Sbx);

    if ((Sbx > SXR_NB_TOTAL_SBX)
            || (sxr_Sbx.Queue [Sbx].Id != SXR_SBX_ALLOCATED))
    {
        SXS_RAISE ((_SXR|TABORT|TNB_ARG(1)|TDB,TSTR("Unknown synchronization box %i\n",0x06a90004), Sbx));
    }

    sxr_Sbx.Queue [sxr_Task.Active].Task.Next = SXR_NO_TASK;

    do
    {
        if (sxr_Sbx.Queue [Sbx].Msg.First == SXR_NO_ENV)
        {
            if (sxr_Task.Active == sxr_Task.Idle)
            {
                SXS_RAISE ((_SXR|TABORT|TDB,TSTR("Idle Task cannot wait for a message.\n",0x06a90005)));
            }

            if (sxr_Sbx.Queue [Sbx].Task.First == SXR_NO_TASK)
            {
                sxr_Sbx.Queue [Sbx].Task.First = sxr_Task.Active;
            }

#ifdef SX_SBX_QUEUE_BY_PRIORITY
            else
            {
                u8 Index = sxr_Sbx.Queue [Sbx].Task.First;
                u8 NextIndex = sxr_Sbx.Queue [Index].Task.Next;

                if((sxr_Task.Ctx [sxr_Task.Active].Desc -> Priority < sxr_Task.Ctx [Index].Desc -> Priority))
                {
                    sxr_Sbx.Queue [sxr_Task.Active].Task.Next = Index;
                    sxr_Sbx.Queue [Sbx].Task.First = sxr_Task.Active;
                }
                else
                {
                    while (( NextIndex != SXR_NO_TASK )
                            &&     (sxr_Task.Ctx [sxr_Task.Active].Desc -> Priority > sxr_Task.Ctx [NextIndex].Desc -> Priority))
                    {
                        Index = NextIndex;
                        NextIndex = sxr_Sbx.Queue [Index].Task.Next;
                    }

                    sxr_Sbx.Queue [Index].Task.Next = sxr_Task.Active;

                    sxr_Sbx.Queue [sxr_Task.Active].Task.Next = NextIndex;
                }
            }
#else
            else
            {
                u8 NextTsk = sxr_Sbx.Queue [Sbx].Task.First;

                while ( sxr_Sbx.Queue [NextTsk].Task.Next != SXR_NO_TASK )
                {
                    NextTsk = sxr_Sbx.Queue [NextTsk].Task.Next;
                }

                sxr_Sbx.Queue [NextTsk].Task.Next = sxr_Task.Active;
            }
#endif

            sxr_Task.Ctx [sxr_Task.Active].State |= SXR_WAIT_MSG_TSK;
            sxr_Task.Ctx [sxr_Task.Active].State &= ~SXR_ACTIVE_TSK;

            sxr_RunNextTask ();
        }

        //sxr_Sbx.Queue [Sbx].Task.First = sxr_Sbx.Queue [sxr_Task.Active].Task.Next;
        if ( sxr_Sbx.Queue [Sbx].Msg.First == SXR_NO_ENV )
        {
            SXS_FPRINTF ((_SXR|TDB,TSTR("Message lost !\n",0x06a90006)));
        }

    }
    while ( sxr_Sbx.Queue [Sbx].Msg.First == SXR_NO_ENV );

    IdxCur = sxr_Sbx.Queue [Sbx].Msg.First;
    sxr_Sbx.Queue [Sbx].Msg.First = sxr_Sbx.Env [IdxCur].Next;

    sxr_Sbx.Env [IdxCur].Next = sxr_Sbx.EnvIdxFree;
    sxr_Sbx.EnvIdxFree = IdxCur;

    SX_PROFILE_WAIT_QUEUE_END(Sbx);
    SX_PROFILE_ENVELOP_USE_END(IdxCur);

    if ( sxr_Sbx.Queue [Sbx].Msg.First == SXR_NO_ENV )
    {
        sxr_Sbx.Queue [Sbx].Msg.Last = SXR_NO_ENV;
    }

    // Get the sender's info
    if (SenderTaskId)
    {
        *SenderTaskId = sxr_Sbx.Env [IdxCur].SenderTaskId;
    }
    if (SenderAddr)
    {
        *SenderAddr = sxr_Sbx.Env [IdxCur].SenderAddr;
    }

    if (sxr_Sbx.Env [IdxCur].EvtTag)
    {

        if ( Evt == NIL )
        {
            SXS_RAISE ((_SXR|TABORT|TDB,TSTR("Unable to deliver event.\n",0x06a90007)));
        }

        for (i=0; i<SXR_EVT_MBX_SIZE; i++)
        {
            Evt [i] = sxr_Sbx.Env [IdxCur].Evt [i];
        }
    }
    else
    {
        Msg = (void *)sxr_Sbx.Env [IdxCur].Evt [0];
    }

    sxr_ExitSc (Status);

    if (sxr_Sbx.Queue [Sbx].TrcLev > 0)
    {
        if (sxr_Sbx.Env [IdxCur].EvtTag)
        {
            SXS_TRACE (_SXR|TSMAP(1)|TSTDOUT|TNB_ARG(4)|TDB,TSTR("%s get the Event 0x%08lx 0x%08lx from Sbx %i\n",0x06a90009), sxr_Task.Ctx [sxr_Task.Active].Desc -> Name, Evt [0], Evt [1], Sbx);
        }
        else
        {
            SXS_TRACE (_SXR|TSMAP(1)|TSTDOUT|TNB_ARG(2)|TDB,TSTR("%s get a message from Sbx %i\n",0x06a90008), sxr_Task.Ctx [sxr_Task.Active].Desc -> Name, Sbx);
            if (Msg != NIL)
                SXS_DUMP (_SXR|TSTDOUT, 0, Msg, sxr_GetMemSize (Msg));
        }
    }

    return Msg;
}

// =============================================================================
// sxr_SbxHot
// -----------------------------------------------------------------------------
/// Check if at least a message or an event is in the synchronisation box.
/// @param Sbx  Synchronization box number.
/// @return     TRUE when box is hot, FALSE otherwise.
// =============================================================================
u8 sxr_SbxHot (u8 Sbx)
{
    return (sxr_Sbx.Queue [Sbx].Msg.First != SXR_NO_ENV);
}

// =============================================================================
// sxr_RemoveCustMsgFromMbox
// -----------------------------------------------------------------------------
/// Remove a/all message  from a mail box.add by licheng
/// @param Sbx          Mail box number.
/// @param msgid      message wan't to remove
/// @param isAny      remove all message or only the customed msgid message
/// @return             TRUE when message or event has been found, FALSE otherwise.
// =============================================================================

u8 sxr_RemoveCustMsgFromMbox (u8 Sbx,u8 msgid,u8 isAny)
{
    u32 Status   = sxr_EnterSc ();
    u16 IdxCur   = sxr_Sbx.Queue [Sbx].Msg.First;
    u16 IdxPrev  = sxr_Sbx.Queue [Sbx].Msg.First;
    u8  MsgFound = FALSE;
    typedef enum
    {
        GCJ_MSG_COND_WAIT,
        GCJ_MSG_COND_WAIT_TIMEOUT,
        GCJ_MSG_PTHREAD_SIG,
        GCJ_MSG_MUTEX,
        GCJ_MSG_GCJ_KILL,
    } GCJ_BODY_MSG_T ;
    typedef GCJ_BODY_MSG_T MsgBody_t;
    typedef struct
    {
        u32      Id ;          // Identifier Unique for whole stack
        u32      SimId;
    } MsgHead_t;
    typedef struct
    {
        MsgHead_t   H;
        MsgBody_t   B;

    } Msg_t;
    while (IdxCur != SXR_NO_ENV)
    {
        MsgFound = FALSE;
        Msg_t * sig = (Msg_t *)sxr_Sbx.Env [IdxCur].Evt [0];
        //if ( sig->B  ==  msgid || isAny)
        if ( sig->H.Id  ==  msgid || isAny)     //cmni, mod, CreselPlmnScan, 2016.03.21
        {
            MsgFound = TRUE;
            sxr_Free(sig);
        }

        if (MsgFound)
        {
            if (sxr_Sbx.Queue [Sbx].Msg.First == IdxCur)
            {
                sxr_Sbx.Queue [Sbx].Msg.First = sxr_Sbx.Env [IdxCur].Next;

                if ( sxr_Sbx.Queue [Sbx].Msg.First == SXR_NO_ENV )
                {
                    sxr_Sbx.Queue [Sbx].Msg.Last = SXR_NO_ENV;
                }
            }
            else if (sxr_Sbx.Queue [Sbx].Msg.Last == IdxCur )
            {
                sxr_Sbx.Queue [Sbx].Msg.Last = IdxPrev;
                sxr_Sbx.Env [IdxPrev].Next  = SXR_NO_ENV;
            }
            else
            {
                sxr_Sbx.Env [IdxPrev].Next = sxr_Sbx.Env [IdxCur].Next;
            }

            sxr_Sbx.Env [IdxCur].Next = sxr_Sbx.EnvIdxFree;
            sxr_Sbx.EnvIdxFree = IdxCur;
            IdxCur     = sxr_Sbx.Queue [Sbx].Msg.First;
            IdxPrev  = sxr_Sbx.Queue [Sbx].Msg.First;
        }
        else
        {
            IdxPrev = IdxCur;
            IdxCur  = sxr_Sbx.Env [IdxCur].Next;
        }
    }

    sxr_ExitSc (Status);

    return MsgFound;
}


// =============================================================================
// sxr_RemoveCustMsgFromMbox
// -----------------------------------------------------------------------------
/// Remove one message  from a mail box.add by licheng
/// @param Sbx          Mail box number.
/// @param msgid      message wan't to remove
/// @param isAny      remove all message or only the customed msgid message
/// @return             TRUE when message or event has been found, FALSE otherwise.
///
//   cmni, add, CreselPlmnScan, 2016.03.21
// =============================================================================

u8 sxr_RemoveOneCustMsgFromMbox (u8 Sbx, u32 msgid, u8 isAny)
{
    u32 Status   = sxr_EnterSc ();
    u16 IdxCur   = sxr_Sbx.Queue [Sbx].Msg.First;
    u16 IdxPrev  = sxr_Sbx.Queue [Sbx].Msg.First;
    u8  MsgFound = FALSE;

    typedef enum
    {
        GCJ_MSG_COND_WAIT,
        GCJ_MSG_COND_WAIT_TIMEOUT,
        GCJ_MSG_PTHREAD_SIG,
        GCJ_MSG_MUTEX,
        GCJ_MSG_GCJ_KILL,
    } GCJ_BODY_MSG_T ;
    typedef GCJ_BODY_MSG_T MsgBody_t;
    typedef struct
    {
        u32      Id ;          // Identifier Unique for whole stack
        u32      SimId;      //never use this parameter!!!
    } MsgHead_t;
    typedef struct
    {
        MsgHead_t   H;
        MsgBody_t   B;

    } Msg_t;

    while (IdxCur != SXR_NO_ENV)
    {
        MsgFound = FALSE;
        Msg_t * sig = (Msg_t *)sxr_Sbx.Env [IdxCur].Evt [0];
        if (sxr_Sbx.Env [IdxCur].EvtTag == FALSE)
        {
            if ( sig->H.Id  ==  msgid || isAny)
            {
                MsgFound = TRUE;
                sxr_Free(sig);
            }
        }

        if (MsgFound)
        {
            if (sxr_Sbx.Queue [Sbx].Msg.First == IdxCur)
            {
                sxr_Sbx.Queue [Sbx].Msg.First = sxr_Sbx.Env [IdxCur].Next;

                if ( sxr_Sbx.Queue [Sbx].Msg.First == SXR_NO_ENV )
                {
                    sxr_Sbx.Queue [Sbx].Msg.Last = SXR_NO_ENV;
                }
            }
            else if (sxr_Sbx.Queue [Sbx].Msg.Last == IdxCur )
            {
                sxr_Sbx.Queue [Sbx].Msg.Last = IdxPrev;
                sxr_Sbx.Env [IdxPrev].Next  = SXR_NO_ENV;
            }
            else
            {
                sxr_Sbx.Env [IdxPrev].Next = sxr_Sbx.Env [IdxCur].Next;
            }

            sxr_Sbx.Env [IdxCur].Next = sxr_Sbx.EnvIdxFree;
            sxr_Sbx.EnvIdxFree = IdxCur;
            IdxCur     = sxr_Sbx.Queue [Sbx].Msg.First;
            IdxPrev  = sxr_Sbx.Queue [Sbx].Msg.First;

            break;      //only this is different with sxr_RemoveCustMsgFromMbox()
        }
        else
        {
            IdxPrev = IdxCur;
            IdxCur  = sxr_Sbx.Env [IdxCur].Next;
        }
    }

    sxr_ExitSc (Status);

    return MsgFound;
}

// =============================================================================
// sxr_RmvTaskFromSbx
// -----------------------------------------------------------------------------
/// Remove a task waiting on a SBX.
/// @param TaskId   Task Id.
/// @return         TRUE when task has been removed, FALSE otherwise.
// =============================================================================
u8 sxr_RmvTaskFromSbx (u8 TaskId)
{
    u8 Sbx;

    for (Sbx=0; Sbx<SXR_NB_TOTAL_SBX; Sbx++)
    {
        if (sxr_Sbx.Queue [Sbx].Id == SXR_SBX_ALLOCATED)
        {
            u8 TaskPrev = sxr_Sbx.Queue [Sbx].Task.First;

            if (TaskPrev == TaskId)
            {
                sxr_Sbx.Queue [Sbx].Task.First = sxr_Sbx.Queue [TaskPrev].Task.Next;
                return TRUE;
            }
            else if (TaskPrev != SXR_NO_TASK)
            {
                u8 TaskCur = sxr_Sbx.Queue [TaskPrev].Task.Next;

                while ((TaskCur != TaskPrev)
                        && (TaskCur != SXR_NO_TASK))
                {
                    TaskPrev = TaskCur;
                    TaskCur = sxr_Sbx.Queue [TaskPrev].Task.Next;
                }

                if (TaskCur != SXR_NO_TASK)
                {
                    sxr_Sbx.Queue [TaskPrev].Task.Next = sxr_Sbx.Queue [TaskCur].Task.Next;
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

// =============================================================================
// sxr_RmvMsgFromMbx
// -----------------------------------------------------------------------------
/// Remove a message or an event from a mail box.
/// @param Msg          Pointer onto a message.
/// @param Evt          Event description.
/// @param Sbx          Mail box number.
/// @param EvtCtxSize   Event size(0 for message).
/// @return             TRUE when message or event has been found, FALSE otherwise.
// =============================================================================
u8 sxr_RmvMsgFromMbx ( void *Msg, u8 Evt, u8 Sbx, u8 EvtCtxSize )
{
    u32 Status   = sxr_EnterSc ();
    u16 IdxCur   = sxr_Sbx.Queue [Sbx].Msg.First;
    u16 IdxPrev  = sxr_Sbx.Queue [Sbx].Msg.First;
    u8  MsgFound = FALSE;
    u32 i;

    while ((IdxCur != SXR_NO_ENV)
            && (!MsgFound))
    {
        if (Evt)
        {
            if ( sxr_Sbx.Env [IdxCur].EvtTag)
            {
                MsgFound = TRUE;

                for (i=0; i < EvtCtxSize; i++)
                {
                    if (sxr_Sbx.Env [IdxCur].Evt [i] != ((u32 *) Msg) [i])
                    {
                        MsgFound = FALSE;
                        break;
                    }
                }
            }
        }
        else if ((void *)sxr_Sbx.Env [IdxCur].Evt [0] == Msg)
        {
            MsgFound = TRUE;
        }

        if (MsgFound)
        {
            if (sxr_Sbx.Queue [Sbx].Msg.First == IdxCur)
            {
                sxr_Sbx.Queue [Sbx].Msg.First = sxr_Sbx.Env [IdxCur].Next;

                if ( sxr_Sbx.Queue [Sbx].Msg.First == SXR_NO_ENV )
                {
                    sxr_Sbx.Queue [Sbx].Msg.Last = SXR_NO_ENV;
                }
            }
            else if (sxr_Sbx.Queue [Sbx].Msg.Last == IdxCur )
            {
                sxr_Sbx.Queue [Sbx].Msg.Last = IdxPrev;
                sxr_Sbx.Env [IdxPrev].Next  = SXR_NO_ENV;
            }
            else
            {
                sxr_Sbx.Env [IdxPrev].Next = sxr_Sbx.Env [IdxCur].Next;
            }

            sxr_Sbx.Env [IdxCur].Next = sxr_Sbx.EnvIdxFree;
            sxr_Sbx.EnvIdxFree = IdxCur;
        }
        else
        {
            IdxPrev = IdxCur;
            IdxCur  = sxr_Sbx.Env [IdxCur].Next;
        }
    }

    sxr_ExitSc (Status);

    return MsgFound;
}

// =============================================================================
// sxr_RmvAllMsgFromMbx
// -----------------------------------------------------------------------------
/// Remove all event/msg already sent to the mail box.
/// @param Sbx  Mail box Id.
// =============================================================================
void sxr_RmvAllMsgFromMbx (u8 Sbx)
{
    u32 Status   = sxr_EnterSc ();

    u16 IdxLast  = sxr_Sbx.Queue [Sbx].Msg.Last;
    if (IdxLast != SXR_NO_ENV)
    {
        sxr_Sbx.Env [IdxLast].Next = sxr_Sbx.EnvIdxFree;
        sxr_Sbx.EnvIdxFree = IdxLast;
    }

    sxr_Sbx.Queue [Sbx].Msg.First = SXR_NO_ENV;
    sxr_Sbx.Queue [Sbx].Msg.Last = SXR_NO_ENV;

    sxr_ExitSc (Status);
}

// =============================================================================
// sxr_CheckMsgInMbx
// =============================================================================
u8 sxr_CheckMsgInMbx ( void *Msg, u8 Evt, u8 Sbx, u8 EvtCtxSize )
{
    u32 Status   = sxr_EnterSc ();
    u16 IdxCur   = sxr_Sbx.Queue [Sbx].Msg.First;
    u16 IdxPrev  = sxr_Sbx.Queue [Sbx].Msg.First;
    u32 i;

    while (IdxCur != SXR_NO_ENV)
    {
        if (Evt)
        {
            if ( sxr_Sbx.Env [IdxCur].EvtTag)
            {
                for (i=0; i < EvtCtxSize; i++)
                {
                    if (sxr_Sbx.Env [IdxCur].Evt [i] != ((u32 *) Msg) [i])
                        break;
                }

                if (i >= EvtCtxSize)
                    goto found;
            }
        }
        else if ((void *)sxr_Sbx.Env [IdxCur].Evt [0] == Msg)
        {
            goto found;
        }

        IdxPrev = IdxCur;
        IdxCur  = sxr_Sbx.Env [IdxCur].Next;
    }

    sxr_ExitSc (Status);
    return FALSE;

found:
    sxr_ExitSc (Status);
    return TRUE;
}

// =============================================================================
// sxr_SetSemaCallerAddr
// -----------------------------------------------------------------------------
/// Set the caller address for the semaphore. For debug only.
/// @param Id   Semaphore Id.
/// @param CallerAddr   Caller Address.
// =============================================================================
void sxr_SetSemaCallerAddr (u8 Id, u32 CallerAddr)
{
    for (u32 i=0; i<SXR_NB_MAX_SEM; i++)
    {
        if (sxr_SemaInfo[i].InUse && sxr_SemaInfo[i].SemaId == Id)
        {
            sxr_SemaInfo[i].CallerAddr = CallerAddr;
            break;
        }
    }
}

// =============================================================================
// sxr_TakeSemaphore
// -----------------------------------------------------------------------------
/// Take the semaphore.
/// @param Id   Semaphore Id.
// =============================================================================
void sxr_TakeSemaphore (u8 Id)
{
    sxr_Wait (NIL, Id);

    for (u32 i=0; i<SXR_NB_MAX_SEM; i++)
    {
        if (sxr_SemaInfo[i].InUse && sxr_SemaInfo[i].SemaId == Id)
        {
            sxr_SemaInfo[i].CallerTask = sxr_Task.Active;
            SXS_GET_RA(sxr_SemaInfo[i].CallerAddr);
            break;
        }
    }
}

// =============================================================================
// sxr_ReleaseSemamphore
// -----------------------------------------------------------------------------
/// Release a semaphore.
/// @param Id   Semaphore Id.
// =============================================================================
void sxr_ReleaseSemaphore (u8 Id)
{
    for (u32 i=0; i<SXR_NB_MAX_SEM; i++)
    {
        if (sxr_SemaInfo[i].InUse && sxr_SemaInfo[i].SemaId == Id)
        {
            sxr_SemaInfo[i].CallerTask = SXR_NO_TASK;
            sxr_SemaInfo[i].CallerAddr = NIL;
            break;
        }
    }

    sxr_Send (NIL, Id, SXR_SEND_MSG | SXR_QUEUE_LAST);
}

// =============================================================================
// sxr_SemaphoreHot
// -----------------------------------------------------------------------------
/// Check if a semaphore is free.
/// @param Id   Semaphore Id.
/// @return     TRUE when the semaphore is free, FALSE when it is hot.
// =============================================================================
u8 sxr_SemaphoreHot (u8 Id)
{
    return (sxr_Sbx.Queue [Id].Msg.First != SXR_NO_ENV);
}

// =============================================================================
// sxr_TaskWaitToQueue
// =============================================================================
void sxr_TaskWaitToQueue (u8 Sbx)
{
    u32   Status = sxr_EnterSc ();

    if (sxr_Sbx.Queue [Sbx].Task.First == SXR_NO_TASK)
    {
        sxr_Sbx.Queue [Sbx].Task.First = sxr_Task.Active;
    }
    else
    {
        u8 Index = sxr_Sbx.Queue [Sbx].Task.First;
        u8 NextIndex = sxr_Sbx.Queue [Index].Task.Next;

        if((sxr_Task.Ctx [sxr_Task.Active].Desc -> Priority < sxr_Task.Ctx [Index].Desc -> Priority))
        {
            sxr_Sbx.Queue [sxr_Task.Active].Task.Next = Index;
            sxr_Sbx.Queue [Sbx].Task.First = sxr_Task.Active;
        }
        else
        {
            while (( NextIndex != SXR_NO_TASK )
                    &&     (sxr_Task.Ctx [sxr_Task.Active].Desc -> Priority > sxr_Task.Ctx [NextIndex].Desc -> Priority))
            {
                Index = NextIndex;
                NextIndex = sxr_Sbx.Queue [Index].Task.Next;
            }

            sxr_Sbx.Queue [Index].Task.Next = sxr_Task.Active;
            sxr_Sbx.Queue [sxr_Task.Active].Task.Next = NextIndex;
        }
    }

    sxr_Task.Ctx [sxr_Task.Active].State |= SXR_WAIT_MSG_TSK;
    sxr_Task.Ctx [sxr_Task.Active].State &= ~SXR_ACTIVE_TSK;

    sxr_RunNextTask ();

    sxr_ExitSc (Status);
}

// =============================================================================
// sxr_WakeupFirstTaskFromQueue
// =============================================================================
void sxr_WakeupFirstTaskFromQueue (u8 Sbx)
{
    u32   Status = sxr_EnterSc ();

    if (sxr_Sbx.Queue [Sbx].Task.First != SXR_NO_TASK )
    {
        u8 TaskId = sxr_Sbx.Queue [Sbx].Task.First;
        sxr_Sbx.Queue [Sbx].Task.First = sxr_Sbx.Queue [TaskId].Task.Next;
        sxr_Sbx.Queue [TaskId].Task.Next = SXR_NO_TASK;
        sxr_Task.Ctx [TaskId].State &= ~SXR_WAIT_MSG_TSK;

        sxr_WakeUpTask (TaskId);
    }

    sxr_ExitSc (Status);
}

// =============================================================================
// sxr_WakeupTaskFromQueue
// =============================================================================
void sxr_WakeupTaskFromQueue (u8 Task, u8 Sbx)
{
    u32   Status = sxr_EnterSc ();

    u8 IdFirst = sxr_Sbx.Queue [Sbx].Task.First;
    if (IdFirst == SXR_NO_TASK)
    {
        ; // do nothing
    }
    else if (IdFirst == Task)
    {
        sxr_Sbx.Queue [Sbx].Task.First = sxr_Sbx.Queue [IdFirst].Task.Next;
        sxr_Sbx.Queue [IdFirst].Task.Next = SXR_NO_TASK;
        sxr_Task.Ctx [IdFirst].State &= ~SXR_WAIT_MSG_TSK;

        sxr_WakeUpTask (IdFirst);
    }
    else
    {
        u8 IdPrev = IdFirst;
        u8 IdCurr = sxr_Sbx.Queue [IdPrev].Task.Next;
        while (IdCurr != SXR_NO_TASK)
        {
            if (IdCurr == Task)
            {
                sxr_Sbx.Queue [IdPrev].Task.Next = sxr_Sbx.Queue [IdCurr].Task.Next;
                sxr_Sbx.Queue [IdCurr].Task.Next = SXR_NO_TASK;
                sxr_Task.Ctx [IdCurr].State &= ~SXR_WAIT_MSG_TSK;

                sxr_WakeUpTask (IdCurr);
                break;
            }
            IdPrev = IdCurr;
            IdCurr = sxr_Sbx.Queue [IdCurr].Task.Next;
        }
    }

    sxr_ExitSc (Status);
}

// =============================================================================
// sxr_FreeEnvCountAvail
// -----------------------------------------------------------------------------
/// Check whether the requested free env are aviable.
// =============================================================================
u8 sxr_FreeEnvCountAvail (u16 Cnt)
{
    u32 Status = sxr_EnterSc ();

    u16 IdxFree = sxr_Sbx.IdxFree;
    while (IdxFree != SXR_NO_ENV)
    {
        if (--Cnt == 0)
            break;

        IdxFree = sxr_Sbx.Env[IdxFree].Next;
    }
    sxr_ExitSc (Status);

    return (Cnt == 0)? 1 : 0;
}

// =============================================================================
// sxs_FuncName
// -----------------------------------------------------------------------------
/// Scope
/// @param
/// @return
// =============================================================================

