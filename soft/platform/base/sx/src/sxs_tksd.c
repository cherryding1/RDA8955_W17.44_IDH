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

#define __SXR_TKSD_VAR__
#include "sxr_tksd.hp"
#undef __SXR_TKSD_VAR__
#include "sxs_cksum.h"

// =============================================================================
// sxr_TkSdInit
// -----------------------------------------------------------------------------
/// Nucleus initialization.
// =============================================================================
void sxr_TkSdInit (void)
{
    u8 i;

    for (i=0; i<SXR_NB_MAX_TASK; i++)
    {
        sxr_Task.Ctx [i].Sp       = NIL;
        sxr_Task.Ctx [i].Pc       = NIL;
        sxr_Task.Ctx [i].StackTop = NIL;
        sxr_Task.Ctx [i].State    = SXR_FREE_TSK;
        sxr_Task.Ctx [i].Id       = SXR_NO_TASK;
        sxr_Task.Ctx [i].Next     = SXR_NO_TASK;
        sxr_Task.Ctx [i].Free     = i + 1;
    }

    sxr_Task.Ctx [SXR_NB_MAX_TASK - 1].Free = SXR_NO_TASK;
    sxr_Task.IdxFree         = 0;
    sxr_Task.Load            = 0;
    sxr_Task.Active          = SXR_NO_TASK;
    sxr_Task.Idle            = SXR_NO_TASK;
    sxr_Task.Suspended       = SXR_NO_TASK;
    sxr_Task.ScheduleDisable = 1;
    sxr_Task.IdleReq         = FALSE;
}

// =============================================================================
// sxr_NewTask
// -----------------------------------------------------------------------------
/// Allocate a new task.
/// @param Desc Pointer onto the task context static descriptor.
/// @return     Task Id.
// =============================================================================
u8 sxr_NewTask (sxr_TaskDesc_t const *Desc)
{
    u32 Status = sxr_EnterSc ();

    u8 Id = sxr_Task.IdxFree;

    if (Id == SXR_NO_TASK)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Too many tasks %i\n",0x06c30001), sxr_Task.Load));
    }

    sxr_Task.IdxFree = sxr_Task.Ctx [Id].Free;

    sxr_Task.Load++;

    sxr_ExitSc (Status);

    sxr_Task.Ctx [Id].Id    = Id;
    sxr_Task.Ctx [Id].State = SXR_ALLOCATED_TSK;
    sxr_Task.Ctx [Id].Next  = SXR_NO_TASK;

    sxr_Task.Ctx [Id].StackTop = (u32 *)_sxr_HMalloc ((u16)(SXR_SET_STACK(Desc -> StackSize) << 2), SXR_TK_STCK_HEAP);

    if (sxr_Task.Ctx [Id].StackTop == NIL)
    {
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No more memory\n",0x06c30002)));
    }

    sxr_Task.Ctx [Id].Desc = Desc;

    if ((sxr_Task.Idle == SXR_NO_TASK)
            ||  (sxr_Task.Ctx [sxr_Task.Idle].Desc -> Priority < sxr_Task.Ctx [Id].Desc -> Priority))
    {
        sxr_Task.Idle = Id;
    }

    return Id;
}

// =============================================================================
// sxr_NewTask
// -----------------------------------------------------------------------------
/// Allocate a new task.
/// @param Desc Pointer onto the task context static descriptor.
/// @return     Task Id.
// =============================================================================
u8 sxr_NewTaskExt(sxr_TaskDesc_t const *Desc, u32 *pStack)
{
    u32 Status = sxr_EnterSc ();

    u8 Id = sxr_Task.IdxFree;

    if (Id == SXR_NO_TASK)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Too many tasks %i\n",0x06c30001), sxr_Task.Load));
    }

    sxr_Task.IdxFree = sxr_Task.Ctx [Id].Free;

    sxr_Task.Load++;

    sxr_ExitSc (Status);

    sxr_Task.Ctx [Id].Id    = Id;
    sxr_Task.Ctx [Id].State = SXR_ALLOCATED_TSK;
    sxr_Task.Ctx [Id].Next  = SXR_NO_TASK;

    sxr_Task.Ctx [Id].StackTop = pStack;

    if (sxr_Task.Ctx [Id].StackTop == NIL)
    {
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No more memory\n",0x06c30002)));
    }

    sxr_Task.Ctx [Id].Desc = Desc;

    if ((sxr_Task.Idle == SXR_NO_TASK)
            ||  (sxr_Task.Ctx [sxr_Task.Idle].Desc -> Priority < sxr_Task.Ctx [Id].Desc -> Priority))
    {
        sxr_Task.Idle = Id;
    }

    return Id;
}
u8 sxr_GetNewTaskId ()
{
    u32 Status = sxr_EnterSc ();
    u8 Id = sxr_Task.IdxFree;
    sxr_ExitSc (Status);
    return Id;
}

char * sxr_GetStatckTop(u8 id)
{
    return (u8*)sxr_Task.Ctx [id].StackTop;
}

char * sxr_GetStatckBottom(u8 id)
{
    return (u8*)(sxr_Task.Ctx [id].StackTop) + (SXR_SET_STACK(sxr_Task.Ctx [id].Desc -> StackSize) << 2);
}

u8 sxr_GetCurrentTaskId (void)
{
    if (sxr_Task.Active !=  SXR_NO_TASK)
    {
        return sxr_Task.Ctx [sxr_Task.Active].Id;
    }
    else
    {
        return 0xff;
    }
}

void *sxr_GetCurrentTaskDesc (void)
{
    if (sxr_Task.Active ==  SXR_NO_TASK)
        return NULL;

    return sxr_Task.Ctx [sxr_Task.Active].Desc;
}

// =============================================================================
// sxr_FreeTask
// -----------------------------------------------------------------------------
/// Free a task.
/// @param Id Task Id.
// =============================================================================
void sxr_FreeTask (u8 Id)
{
    sxr_CheckTaskId (Id);

    if (!(sxr_Task.Ctx [Id].State & (SXR_ALLOCATED_TSK | SXR_STOPPED_TSK)))
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(2),TSTR("Bad state for task release 0x%x (%i)\n",0x06c30003), sxr_Task.Ctx [Id].State, Id));
    }

    sxr_Task.Ctx [Id].Id = SXR_NO_TASK;
    sxr_Task.Ctx [Id].State = SXR_FREE_TSK;
    sxr_HFree (sxr_Task.Ctx [Id].StackTop);
    sxr_Task.Ctx [Id].StackTop = NIL;

    u32 Status = sxr_EnterSc ();

    sxr_Task.Ctx [Id].Free = sxr_Task.IdxFree;
    sxr_Task.IdxFree = Id;

    sxr_Task.Load--;

    sxr_ExitSc (Status);
}


// =============================================================================
// sxr_StartScheduling
// -----------------------------------------------------------------------------
/// Activation of the first eligible task.
// =============================================================================
void sxr_StartScheduling (void)
{
    u32 Status = sxr_EnterSc ();

    if (sxr_Task.Active == SXR_NO_TASK)
    {
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No elligible task\n",0x06c30004)));
    }
    sxr_Task.ScheduleDisable = 0;

    sxr_Task.Ctx [sxr_Task.Active].State = SXR_ACTIVE_TSK;

    sxr_TaskFirst (&sxr_Task.Ctx [sxr_Task.Active]);
    sxr_ExitSc (Status);
}


// =============================================================================
// sxr_SchedulingStarted
// -----------------------------------------------------------------------------
/// Tell if the scheduling has been started.
// =============================================================================
bool sxr_SchedulingStarted (void)
{
    // Snack: If sxr_Task.Active has never been initialized, it will equal 0,
    // and sxr_Task.Ctx[] will be filled with 0 too, so the following condition still works.
    if (sxr_Task.Active != SXR_NO_TASK)
    {
        if (sxr_Task.Ctx [sxr_Task.Active].State == SXR_ACTIVE_TSK)
        {
            return TRUE;
        }
    }

    return FALSE;
}


// =============================================================================
// sxr_GetTaskPriority
// -----------------------------------------------------------------------------
/// Get the active task priority.
/// @return Active task priority.
// =============================================================================
u8 sxr_GetTaskPriority (void)
{
    return sxr_Task.Ctx [sxr_Task.Active].Desc -> Priority;
}


// =============================================================================
// sxr_GetTaskName
// -----------------------------------------------------------------------------
/// Get the active task Name.
/// @return Pointer onto active task Name.
// =============================================================================
const ascii *sxr_GetTaskName (void)
{
    const ascii *NoTaskName = "No Active Task";

    if (sxr_Task.Active !=  SXR_NO_TASK)
    {
        return sxr_Task.Ctx [sxr_Task.Active].Desc -> Name;
    }
    else
    {
        return NoTaskName;
    }
}


// =============================================================================
// sxr_GetTaskNameById
// -----------------------------------------------------------------------------
/// Get the task name by task ID.
/// @param Id Task Id.
/// @return Pointer onto the task Name.
// =============================================================================
const ascii *sxr_GetTaskNameById (u8 Id)
{
    const ascii *NoTaskName = "No Active Task or Free Task";

    if (Id !=  SXR_NO_TASK && sxr_Task.Ctx[Id].State>1)
    {
        return sxr_Task.Ctx [Id].Desc -> Name;
    }
    else
    {
        return NoTaskName;
    }
}

__attribute__((deprecated)) const ascii *sxr_GetTaskNameId (u8 Id)
{
    return sxr_GetTaskNameById(Id);
}

// =============================================================================
// sxr_GetTaskParam
// -----------------------------------------------------------------------------
/// Get the dynamic context of a task as defined by sxr_StartTask().
/// @param Id Task Id.
/// @return   user defined data context.
// =============================================================================
void *sxr_GetTaskParam (u8 Id)
{
    return sxr_Task.Ctx [Id].DataCtx;
}


// =============================================================================
// sxr_GetTaskState
// -----------------------------------------------------------------------------
/// Get task current state.
/// @param Id Task Id.
/// @return   the task's state.
// =============================================================================
u8 sxr_GetTaskState (u8 Id)
{
    return sxr_Task.Ctx [Id].State;
}


// =============================================================================
// sxr_GetStackMaxUsage
// -----------------------------------------------------------------------------
/// Calculate the max used bytes in stack for a task.
/// @param TkNb Task Id.
// =============================================================================
u32 sxr_GetStackMaxUsage(u8 TkNb)
{
#ifdef __SXR_STACK_CHECK__

    u32 i=0;
    u32 *stackMem = (u32 *)sxr_Task.Ctx [TkNb].StackTop;
    while (stackMem[i++] == SXR_DW_MEM_PATTERN );

    return (SXR_SET_STACK(sxr_Task.Ctx [TkNb].Desc -> StackSize) << 2) - (i << 2);

#else

    return 0;

#endif
}


// =============================================================================
// sxr_ChkTask
// -----------------------------------------------------------------------------
/// Trace out a diagnostic information for a task.
/// @param TkNb Task Id.
// =============================================================================
void sxr_ChkTask (u8 TkNb)
{
    sxr_CheckTaskId (TkNb);

    if ( sxr_Task.Ctx [TkNb].Id != SXR_NO_TASK)
    {
        SXS_FPRINTF ((_SXR|TSTDOUT|TSMAP(2)|TDB|TNB_ARG(3),TSTR("Task %2i: %s Priority %3i ",0x06c30005),
                      TkNb,
                      sxr_Task.Ctx [TkNb].Desc -> Name,
                      sxr_Task.Ctx [TkNb].Desc -> Priority));

        switch (sxr_Task.Ctx [TkNb].State)
        {
            case SXR_ALLOCATED_TSK :
                SXS_FPRINTF ((_SXR|TSTDOUT|TIDU|TDB,TSTR("is allocated.\n",0x06c30006)));
                break;

            case SXR_PENDING_TSK :
                SXS_FPRINTF ((_SXR|TSTDOUT|TIDU|TDB,TSTR("is pending.\n",0x06c30007)));
                break;

            case SXR_ACTIVE_TSK :
                SXS_FPRINTF ((_SXR|TSTDOUT|TIDU|TDB,TSTR("is active.\n",0x06c30008)));
                break;

            case SXR_SUSPENDED_TSK :
                SXS_FPRINTF ((_SXR|TSTDOUT|TIDU|TDB,TSTR("is suspended.\n",0x06c30009)));
                break;

            case SXR_WAIT_MSG_TSK :
                SXS_FPRINTF ((_SXR|TSTDOUT|TIDU|TDB,TSTR("wait for a message.\n",0x06c3000a)));
                break;

            default :
                SXS_FPRINTF ((_SXR|TSTDOUT|TIDU|TDB,TSTR("is broken...\n",0x06c3000b)));
        }

#ifdef __SXR_STACK_CHECK__
        {
            SXS_FPRINTF ((_SXR|TSTDOUT|TIDU|TDB|TNB_ARG(4),TSTR("Stack %3i/%3i Top 0x%lx <- 0x%lx\n",0x06c3000c),
                          sxr_GetStackMaxUsage(TkNb),
                          SXR_SET_STACK(sxr_Task.Ctx [TkNb].Desc -> StackSize) << 2,
                          sxr_Task.Ctx [TkNb].StackTop,
                          sxr_Task.Ctx [TkNb].StackTop + SXR_SET_STACK(sxr_Task.Ctx [TkNb].Desc -> StackSize)));
        }
#endif
    }
}


// =============================================================================
// sxr_CheckTask
// -----------------------------------------------------------------------------
/// Trace out a diagnostic information for all the existing task.
// =============================================================================
void sxr_CheckTask (void)
{
    u32 i;

    SXS_FPRINTF ((_SXR|TSTDOUT|TDB|TNB_ARG(2),TSTR("Allocated Tasks %i Schedule Disable %i\n",0x06c3000d),
                  sxr_Task.Load, sxr_Task.ScheduleDisable));
    for (i=0; i<SXR_NB_MAX_TASK; i++)
    {
        sxr_ChkTask ((u8)i);
    }
}


// =============================================================================
// sxs_CheckStkTk
// -----------------------------------------------------------------------------
/// Lookup a task from a stack pointer.
/// Trace out a diagnostic information about the task found..
/// @param Sp Stack pointer.
/// @return TRUE if task was found.
// =============================================================================
u8 sxs_CheckStkTk (u32 *Sp)
{
    u32 i;

    for (i=0; i<SXR_NB_MAX_TASK; i++)
    {
        if (sxr_Task.Ctx [i].State != SXR_FREE_TSK)
        {
            if ((Sp >= sxr_Task.Ctx [i].StackTop)
                    &&  (Sp < (sxr_Task.Ctx [i].StackTop + SXR_SET_STACK(sxr_Task.Ctx [i].Desc -> StackSize))))
            {
                SXS_TRACE (_SXR|TSTDOUT|TDB,TSTR("Stack found for Task\n",0x06c3000e));
                sxr_ChkTask (i);
                return TRUE;
            }
        }
    }
    return FALSE;
}

// =============================================================================
// sxs_CheckPendingTk
// -----------------------------------------------------------------------------
/// Trace out a diagnostic information for all the pending or active task.
// =============================================================================
void sxs_CheckPendingTk (void)
{
    u32 i;

    for (i=0; i<SXR_NB_MAX_TASK; i++)
    {
        if ((sxr_Task.Ctx [i].State == SXR_PENDING_TSK)
                ||  (sxr_Task.Ctx [i].State == SXR_ACTIVE_TSK))
        {
            if (sxr_Task.Ctx [i].State == SXR_PENDING_TSK)
            {
                SXS_TRACE (_SXR|TSMAP(1)|TSTDOUT|TNB_ARG(1)|TDB,TSTR("Task %s is Pending\n",0x06c3000f), sxr_Task.Ctx [i].Desc -> Name);
            }
            else
            {
                SXS_TRACE (_SXR|TSMAP(1)|TSTDOUT|TNB_ARG(1)|TDB,TSTR("Task %s is Active\n",0x06c30010), sxr_Task.Ctx [i].Desc -> Name);
            }

            sxs_BackTrace (sxr_GetTkPcInStack (sxr_Task.Ctx [i].Sp), sxr_Task.Ctx [i].Sp);
        }
    }
}



// =============================================================================
// sxr_GetTkPcInStack
// -----------------------------------------------------------------------------
/// Get the PC from the context stored on a task's stack.
/// @param Sp Stack pointer of a task.
/// @return   PC value.
// =============================================================================
u32 sxr_GetTkPcInStack (u32 *Sp)
{
    return *(Sp+1);
}


// =============================================================================
// sxs_GetTaskSp
// -----------------------------------------------------------------------------
/// Get the stack pointer from the task context list.
/// @param Id Task Id.
/// @return The stack pointer.
// =============================================================================
u32 * sxr_GetTaskSp(u8 Id)
{
    if ((sxr_Task.Ctx [Id].State & (SXR_PENDING_TSK|
                                    SXR_SUSPENDED_TSK|
                                    SXR_ACTIVE_TSK|
                                    SXR_WAIT_MSG_TSK)) == 0)
    {
        return NULL;
    }
    return sxr_Task.Ctx [Id].Sp;
}

#if (!defined(__TARGET__)) || defined (__SIMU__)
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <assert.h>

jmp_buf sxr_TskCtxJmp  [SXR_NB_MAX_TASK];

/*
==============================================================================
   Function   : sxr_Disable
 ----------------------------------------------------------------------------
   Scope      : Stub for interrupts disable function.
   Parameters : none
   Return     : 0
==============================================================================
*/

// =============================================================================
// sxs_FuncName
// -----------------------------------------------------------------------------
/// Scope
/// @param
/// @return
// =============================================================================
void sxr_Disable (void)
{
}

// =============================================================================
// sxr_EnterSc
// -----------------------------------------------------------------------------
/// Stub for interrupts disable function
/// @return 0
// =============================================================================
u32 sxr_EnterSc (void)
{
    return 0xabcd;
}


// =============================================================================
// sxr_ExitSc
// -----------------------------------------------------------------------------
/// Stub for interrupts enable function.
/// @param Status Interrupt status.
// =============================================================================
void sxr_ExitSc  (u32 Status)
{
    if (Status != 0xabcd)
    {
        SXS_RAISE ((_SXR|TDB,TSTR("Unknown status\n",0x06c30011)));
    }
}


// =============================================================================
// sxr_TaskSetUp
// -----------------------------------------------------------------------------
/// Set up task stack.
/// @param TaskCtx   Pointer onto task context.
/// @param TaskParam Pointer onto task param.
// =============================================================================
void sxr_TaskSetUp (sxr_TaskCtx_t *TaskCtx, void *TaskParam)
{
    u32 *CtxJump;
    u32 *Sp;

    CtxJump = (u32 *)&sxr_TskCtxJmp [TaskCtx -> Id];
    Sp      = (u32 *) TaskCtx -> Sp;

    setjmp (sxr_TskCtxJmp [TaskCtx -> Id]);

    Sp -= 2;
    *Sp = (u32) TaskParam;

#if defined (__BORLANDC__) && defined (__WIN32__)
    CtxJump [4] = (u32) Sp;
    CtxJump [5] = (u32) TaskCtx -> Pc;
#elif defined (__BORLANDC__) || defined (linux)
    CtxJump [4] = (u32) (Sp-1);
    CtxJump [5] = (u32) TaskCtx -> Pc;
#elif defined (__MSDOS__)
    {
        u16 *ZobCtx = (u16 *)CtxJump;
        u32 Zob;
        u16 *ZZob = (u16 *)&Zob;

        Zob = (u32) (Sp-1);
        ZobCtx [0] = ZZob [0]; // Sp
        ZobCtx [1] = ZZob [1]; // Ss

        Zob = (u32) TaskCtx -> Pc;
        ZobCtx [3] = ZZob [1]; // Cs
        ZobCtx [4] = ZZob [0]; // Ip
    }
#else
    CtxJump [4] = (u32) (Sp-2);
    CtxJump [5] = (u32) TaskCtx -> Pc;
#endif

}


// =============================================================================
// sxr_TaskFirst
// -----------------------------------------------------------------------------
/// Activation of the first task.
/// @param TaskCtx Pointer onto the task context.
// =============================================================================
void sxr_TaskFirst (sxr_TaskCtx_t *TaskCtx)
{
    longjmp (sxr_TskCtxJmp [TaskCtx -> Id], FALSE);
}


// =============================================================================
// sxr_TaskSwap
// -----------------------------------------------------------------------------
/// Save Old task context and activate new task by restoring its context.
/// @param OldTaskCtx Pointer onto old task context.
/// @param NewTaskCtx Pointer onto new task context.
// =============================================================================
void sxr_TaskSwap (sxr_TaskCtx_t *OldTaskCtx, sxr_TaskCtx_t *NewTaskCtx)
{
    if (setjmp (sxr_TskCtxJmp [OldTaskCtx -> Id]) == 0)
    {
        longjmp (sxr_TskCtxJmp [NewTaskCtx -> Id], TRUE);
    }
}



#endif

