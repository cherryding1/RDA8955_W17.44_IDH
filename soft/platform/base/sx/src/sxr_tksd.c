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
#include "sxr_ops.h"
#include "sxr_csp.h"
#include "sxr_tksd.hp"

u32 __attribute__((section(".sramucdata"))) sxr_active_Task;

// =============================================================================
// sxr_StartTask
// -----------------------------------------------------------------------------
/// Initialize dynamic context and wake up the task.
/// @param Id       Task Id.
/// @param DataCtx  Pointer onto data context.
// =============================================================================
void sxr_StartTask (u8 Id, void *DataCtx)
{
    sxr_CheckTaskId (Id);

    if (sxr_Task.Ctx [Id].Id != Id)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(2),TSTR("Task Id unconsistency %i/%i",0x06ab0001), sxr_Task.Ctx [Id].Id, Id));
    }

    if (!(sxr_Task.Ctx [Id].State & (SXR_ALLOCATED_TSK | SXR_STOPPED_TSK)))
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(2),TSTR("Bad state for to start a task 0x%x (%i)",0x06ab0002), sxr_Task.Ctx [Id].State, Id));
    }

    sxr_Task.Ctx [Id].Sp = (u32 *)sxr_Task.Ctx [Id].StackTop + (SXR_SET_STACK(sxr_Task.Ctx [Id].Desc -> StackSize)) - 1;

#ifdef __SXR_STACK_CHECK__
    memset ((u8 *)sxr_Task.Ctx [Id].StackTop, SXR_MEM_PATTERN, (SXR_SET_STACK(sxr_Task.Ctx [Id].Desc -> StackSize)) << 2);
#else
    ((u32 *)sxr_Task.Ctx [Id].StackTop) [0] = SXR_DW_MEM_PATTERN;
#endif

    sxr_Task.Ctx [Id].Pc      = (void *)sxr_Task.Ctx [Id].Desc -> TaskBody;
    sxr_Task.Ctx [Id].DataCtx = DataCtx;

    sxr_TaskSetUp (&sxr_Task.Ctx [Id], DataCtx);

    sxr_WakeUpTask (Id);
}



// =============================================================================
// sxr_StopTask
// -----------------------------------------------------------------------------
/// Stop a task.
/// @param Id Task Id.
// =============================================================================
void sxr_StopTask (u8 Id)
{
    sxr_CheckTaskId (Id);

    u32 Status = sxr_EnterSc ();

    if (sxs_IoCtx.TraceBitMap [TGET_ID(_SXR)] & (1 << 3))
    {
        SXS_FPRINTF ((_SXR|TSTDOUT|TDB|TNB_ARG(0), TSTR("Stopping task:\n",0x06ab000a)));
        sxr_ChkTask (Id);
    }

    if (sxr_Task.Ctx [Id].State == SXR_PENDING_TSK)
    {
        u8 Index     = sxr_Task.Active;
        u8 NextIndex = sxr_Task.Ctx [Index].Next;

        while ((NextIndex != SXR_NO_TASK)
                &&     (NextIndex != Id))
        {
            Index     = NextIndex;
            NextIndex = sxr_Task.Ctx [Index].Next;
        }

        if (NextIndex != Id)
        {
            if (Id == sxr_Task.Active && sxr_Task.ScheduleDisable == 1)
            {
                // Active task can be stopped if scheduling has not been started
                sxr_Task.Ctx [Id].State = SXR_STOPPED_TSK;
                sxr_Task.Active = sxr_Task.Ctx [Id].Next;
                if (sxr_Task.Active == SXR_NO_TASK)
                {
                    SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1), TSTR("No pending task except active task %i\n",0x06ab000b), Id));
                }

                sxr_ExitSc (Status);
                return;
            }
            else
            {
                SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Task %i not Pending\n",0x06ab0003), Id));
            }
        }

        sxr_Task.Ctx [Index].Next = sxr_Task.Ctx [NextIndex].Next;
        sxr_Task.Ctx [NextIndex].Next = SXR_NO_TASK;
    }
    else if ((sxr_Task.Ctx [Id].State & ~SXR_SUSPENDED_TSK) == SXR_WAIT_MSG_TSK)
    {
        if (!sxr_RmvTaskFromSbx (Id))
        {
            SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Task not found in any SBX %i.\n",0x06ab0004), sxr_Task.Ctx [Id].State));
        }
    }
    else if (sxr_Task.Ctx [Id].State & SXR_SUSPENDED_TSK)
    {
        sxr_RmvFromSuspendedQueue (Id);
    }
    else if (sxr_Task.Ctx [Id].State != SXR_ACTIVE_TSK)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Unable to stop task %i.\n",0x06ab0005), sxr_Task.Ctx [Id].State));
    }

    if (sxr_Task.Ctx [Id].Desc -> TaskExit)
    {
        sxr_Task.Ctx [Id].Desc -> TaskExit (sxr_Task.Ctx [Id].DataCtx);
    }

    sxr_Task.Ctx [Id].State = SXR_STOPPED_TSK;

    if (Id == sxr_Task.Active)
    {
        sxr_RunNextTask ();
    }

    sxr_ExitSc (Status);
}



// =============================================================================
// sxr_WakeUpTask
// -----------------------------------------------------------------------------
/// Link task in the pending queue or activate the task if
/// start up is over and priority task is higher.
/// @param Id Task Id.
// =============================================================================
void sxr_WakeUpTask (u8 Id)
{
    sxr_CheckTaskId (Id);

    u32 Status = sxr_EnterSc ();
    if ((sxr_Task.Ctx [Id].State & SXR_SUSPENDED_TSK)
            &&  (!(sxr_Task.Ctx [Id].State & SXR_STOPPED_TSK)))
    {
        sxr_Task.Ctx [Id].State &= ~(SXR_ACTIVE_TSK | SXR_WAIT_MSG_TSK);
        sxr_Task.Ctx [Id].Next  = sxr_Task.Suspended;
        sxr_Task.Suspended = Id;
    }
    else
    {
        u8 CurPriority;

        if (sxr_Task.Active == SXR_NO_TASK)
        {
            sxr_Task.Active = Id;
            sxr_Task.Ctx [Id].Next = SXR_NO_TASK;
            sxr_Task.Ctx [Id].State = SXR_PENDING_TSK; // Me ...
//            sxr_ExitSc (Status); TBC
            return;
        }

        CurPriority = sxr_Task.Ctx [sxr_Task.Active].Desc -> Priority;

        sxr_Task.Ctx [Id].State = SXR_PENDING_TSK;

        if ((!sxr_Task.ScheduleDisable)
                &&  (sxr_Task.Ctx [Id].Desc -> Priority < CurPriority))
        {
            sxr_Task.Ctx [Id].Next  = sxr_Task.Active;
            sxr_Task.Ctx [Id].State = SXR_ACTIVE_TSK;
            sxr_Task.Ctx [sxr_Task.Active].State = SXR_PENDING_TSK;

            sxr_Task.Active = Id;
            sxr_active_Task = sxr_Task.Active;

            pal_SetTaskSpy (Id);
            sxr_TaskSwap (&sxr_Task.Ctx [sxr_Task.Ctx [Id].Next],
                          &sxr_Task.Ctx [sxr_Task.Active]);
        }
        else
        {
            u8 Index;
            u8 NextIndex;

            if ((sxr_Task.ScheduleDisable)
                    &&  (sxr_Task.Ctx [Id].Desc -> Priority < CurPriority))
            {
                if (sxr_Task.Ctx [sxr_Task.Active].State != SXR_ACTIVE_TSK)
                {
                    sxr_Task.Ctx [Id].Next = sxr_Task.Active;
                    sxr_Task.Active = Id;
                    sxr_ExitSc (Status);
                    return;
                }
            }

            Index     = sxr_Task.Active;
            NextIndex = sxr_Task.Ctx [Index].Next;

            while ((NextIndex != SXR_NO_TASK)
                    &&     (sxr_Task.Ctx [Id].Desc -> Priority > sxr_Task.Ctx [NextIndex].Desc -> Priority))
            {
                Index     = NextIndex;
                NextIndex = sxr_Task.Ctx [Index].Next;
            }

            sxr_Task.Ctx [Index].Next = Id;
            sxr_Task.Ctx [Id].Next    = NextIndex;
        }
    }

    sxr_ExitSc (Status);
}



// =============================================================================
// sxr_EnterScSchedule
// -----------------------------------------------------------------------------
/// Disable tasks scheduling.
// =============================================================================
void sxr_EnterScSchedule (void)
{
    u32 Status = sxr_EnterSc ();

    sxr_Task.ScheduleDisable++;

    sxr_ExitSc (Status);
}



// =============================================================================
// sxr_ExitScSchedule
// -----------------------------------------------------------------------------
/// Enable tasks scheduling.
// =============================================================================
void sxr_ExitScSchedule (void)
{
    u32 Status = sxr_EnterSc ();


    if (sxr_Task.ScheduleDisable == 1)
    {
        u8  FirstPending = sxr_Task.Ctx [sxr_Task.Active].Next;

        sxr_Task.ScheduleDisable = 0;

        if (sxr_Task.IdleReq)
        {
            sxr_Task.IdleReq = FALSE;
            sxr_ScheduleIdle ();
        }
        else if (((FirstPending != SXR_NO_TASK)
                  &&   (sxr_Task.Ctx [FirstPending].Desc -> Priority < sxr_Task.Ctx [sxr_Task.Active].Desc -> Priority))
                 ||   (sxr_Task.Ctx [sxr_Task.Active].State & SXR_SUSPENDED_TSK))
        {
            sxr_RunNextTask ();
        }
    }
    else
    {
        sxr_Task.ScheduleDisable--;
    }
    sxr_ExitSc (Status);
}



// =============================================================================
// sxr_ScheduleIdle
// -----------------------------------------------------------------------------
/// Force Idle task scheduling and disable the scheduling.
// =============================================================================
void sxr_ScheduleIdle (void)
{
    u32 Status   = sxr_EnterSc ();
    u8 Index     = sxr_Task.Active;
    u8 NextIndex = sxr_Task.Ctx [Index].Next;

    if (sxr_Task.ScheduleDisable)
    {
        sxr_Task.IdleReq = TRUE;
        sxr_ExitSc (Status);
        return;
    }

    sxr_Task.ScheduleDisable++;

    while (NextIndex != sxr_Task.Idle)
    {
        Index     = NextIndex;
        NextIndex = sxr_Task.Ctx [Index].Next;
    }

    sxr_Task.Ctx [Index].Next = sxr_Task.Ctx [sxr_Task.Idle].Next;

    sxr_Task.Ctx [sxr_Task.Idle].Next = sxr_Task.Active;

    sxr_Task.Active = sxr_Task.Idle;
    sxr_active_Task = sxr_Task.Active;
    pal_SetTaskSpy (sxr_Task.Active);

    sxr_Task.Ctx [sxr_Task.Idle].State = SXR_ACTIVE_TSK;

    sxr_TaskSwap (&sxr_Task.Ctx [sxr_Task.Ctx [sxr_Task.Idle].Next],
                  &sxr_Task.Ctx [sxr_Task.Active]);

    sxr_ExitSc (Status);
}




// =============================================================================
// sxr_RunNextTask
// -----------------------------------------------------------------------------
/// Run the first pending task. Current active task is supposed
/// to be link in the queue corresponding to its state, except
/// in case of suspension.
// =============================================================================
void sxr_RunNextTask (void)
{
    u32 Status = sxr_EnterSc ();
    u8 NewPendingTask = sxr_Task.Active;

#ifdef __SXR_MEM_CHECK__
    sxr_CheckUpdateCluster (NIL, 0);
#endif

    sxr_Task.Active = sxr_Task.Ctx [sxr_Task.Active].Next;

    if (sxr_Task.Active == SXR_NO_TASK)
    {
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No pending task\n",0x06ab0006)));
    }

    if (sxr_Task.ScheduleDisable)
    {
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("Scheduling disabled.\n",0x06ab0007)));
    }

    if (*((u32 *)sxr_Task.Ctx [NewPendingTask].StackTop) != SXR_DW_MEM_PATTERN)
    {
        SXS_RAISE ((_SXR|TABORT|TSMAP(2)|TDB|TNB_ARG(2),TSTR("Stack overflow %lx %s",0x06ab0008), sxr_Task.Ctx [NewPendingTask].Sp,
                    sxr_Task.Ctx [NewPendingTask].Desc -> Name));
    }
    if ((sxr_Task.Ctx [NewPendingTask].State & SXR_SUSPENDED_TSK)
            &&  (!(sxr_Task.Ctx [NewPendingTask].State & SXR_STOPPED_TSK)))
    {
        sxr_Task.Ctx [NewPendingTask].Next = sxr_Task.Suspended;
        sxr_Task.Suspended = NewPendingTask;
    }
    else if (sxr_Task.Ctx [NewPendingTask].State == SXR_ACTIVE_TSK)
    {
        u8 Index     = sxr_Task.Active;
        u8 NextIndex = sxr_Task.Ctx [Index].Next;

        while ((NextIndex != SXR_NO_TASK)
                &&     (sxr_Task.Ctx [NewPendingTask].Desc -> Priority > sxr_Task.Ctx [NextIndex].Desc -> Priority))
        {
            Index     = NextIndex;
            NextIndex = sxr_Task.Ctx [Index].Next;
        }

        sxr_Task.Ctx [Index].Next = NewPendingTask;
        sxr_Task.Ctx [NewPendingTask].Next = NextIndex;
    }
    // Clear the ACTIVE Bit
    // When the task is "only" ACTIVE we can not just clear the statuys bit
    // or the state becomes 0 which is invalid
    // We set it to the pending state
    if (sxr_Task.Ctx [NewPendingTask].State == SXR_ACTIVE_TSK)
    {
        sxr_Task.Ctx [NewPendingTask].State = SXR_PENDING_TSK;
    }
    else // End added code
    {
        // Keep other status bits as they were
        sxr_Task.Ctx [NewPendingTask].State &= ~SXR_ACTIVE_TSK;
    }
    sxr_Task.Ctx [sxr_Task.Active].State = SXR_ACTIVE_TSK;
    sxr_active_Task = sxr_Task.Active;

    pal_SetTaskSpy (sxr_Task.Active);
    sxr_TaskSwap (&sxr_Task.Ctx [NewPendingTask],
                  &sxr_Task.Ctx [sxr_Task.Active]);
    sxr_ExitSc (Status);
}



// =============================================================================
// sxr_SuspendTask
// -----------------------------------------------------------------------------
/// Suspend a task.
/// @param Id Task Id.
/// @return
// =============================================================================
void sxr_SuspendTask (u8 Id)
{
    sxr_CheckTaskId (Id);

    u32 Status = sxr_EnterSc ();

    if (sxr_Task.Ctx [Id].State & SXR_PENDING_TSK)
    {
        u8 Index     = sxr_Task.Active;
        u8 NextIndex = sxr_Task.Ctx [Index].Next;

        while (NextIndex != Id)
        {
            Index     = NextIndex;
            NextIndex = sxr_Task.Ctx [Index].Next;
        }

        sxr_Task.Ctx [Index].Next = sxr_Task.Ctx [Id].Next;

        sxr_Task.Ctx [Id].Next = sxr_Task.Suspended;
        sxr_Task.Suspended = Id;
        sxr_Task.Ctx [Id].State = SXR_SUSPENDED_TSK;
    }
    else
    {
        sxr_Task.Ctx [Id].State |= SXR_SUSPENDED_TSK;
    }

    if ((Id == sxr_Task.Active) && (!sxr_Task.ScheduleDisable))
    {
        sxr_RunNextTask ();
    }

    sxr_ExitSc (Status);
}






// =============================================================================
// sxr_RmvFromSuspendedQueue
// -----------------------------------------------------------------------------
/// Remove a task from the suspended queue.
/// @param Id Task Id.
// =============================================================================
void sxr_RmvFromSuspendedQueue (u8 Id)
{
    if (sxr_Task.Suspended != Id)
    {
        // The searched task is not the first element of the list
        u8 Index     = sxr_Task.Suspended;
        u8 NextIndex = sxr_Task.Ctx [Index].Next;

        // Go through the list
        // Look if the element pointed by Next is the Task "Id"
        while ((NextIndex != Id)
                &&     (NextIndex != SXR_NO_TASK))
        {
            Index     = NextIndex;
            NextIndex = sxr_Task.Ctx [Index].Next;
        }

        // Remove the element from the list
        if (NextIndex == SXR_NO_TASK)
        {
            SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Not a suspended task %i\n",0x06ab0009), Id));
        }
        else
        {
            sxr_Task.Ctx [Index].Next = sxr_Task.Ctx [Id].Next;
        }
    }
    else
    {
        // Remove the first element of the list
        // The head of the list becomes the second element
        sxr_Task.Suspended = sxr_Task.Ctx [Id].Next;
    }
}



// =============================================================================
// sxr_ResumeTask
// -----------------------------------------------------------------------------
/// Resume a task.
/// @param Id Task Id.
// =============================================================================
void sxr_ResumeTask (u8 Id)
{
    sxr_CheckTaskId (Id);

    u32 Status = sxr_EnterSc ();

    sxr_Task.Ctx [Id].State &= ~SXR_SUSPENDED_TSK;

    if (sxr_Task.Ctx [Id].State == 0)
    {
        sxr_RmvFromSuspendedQueue (Id);
        sxr_WakeUpTask (Id);
    }
    sxr_ExitSc (Status);
}



// =============================================================================
// sxr_IsSuspendedTask
// -----------------------------------------------------------------------------
/// Check if a task is suspended.
/// @param Id Task Id.
/// @return   TRUE if the task is suspended, FALSE otherwise
// =============================================================================
u8 sxr_IsSuspendedTask (u8 Id)
{
    return (sxr_Task.Ctx [Id].State & SXR_SUSPENDED_TSK) == SXR_SUSPENDED_TSK;
}

// =============================================================================
// sxs_FuncName
// -----------------------------------------------------------------------------
/// Scope
/// @param
/// @return
// =============================================================================

