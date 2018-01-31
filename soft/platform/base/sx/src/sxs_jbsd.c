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





#define __SXR_JBSD_VAR__
#include "sxr_jbsd.hp"
#undef __SXR_JBSD_VAR__

#include "sxs_type.h"
#include "sxr_ops.h"
#include "sxs_io.h"
#include "sxs_lib.h"

#include "sxr_jbsd.h"
#include "sxp_debug.h"


// =============================================================================
// sxr_JbSdInit
// -----------------------------------------------------------------------------
/// Jobs scheduler data context initialization.
// =============================================================================
void sxr_JbSdInit (void)
{
    u32 i;

    sxr_Job = (sxr_Job_t *) _sxr_HMalloc (sizeof (sxr_Job_t), SXR_JB_CTX_HEAP);

    memset (sxr_Job -> Ctx, 0, sizeof (sxr_JbCtx_t) * SXR_NB_MAX_JOB);
    memset (sxr_Job -> EvtStatus, 0, 4 * sizeof (u16));

    for (i=0; i<SXR_NB_MAX_JOB; i++)
    {
        sxr_Job -> Ctx [i].Free    = (u8) i + 1;
        sxr_Job -> Ctx [i].IdxSort = SXR_NIL_JOB;
        sxr_Job -> Ctx [i].State   = SXR_FREE_JOB;
        sxr_Job -> Sorted [i]      = SXR_NIL_JOB;
    }

    sxr_Job -> Ctx [i-1].Free = SXR_NIL_JOB;

    for (i=0; i<SXR_NB_MAX_JOB_EVT; i++)
    {
        sxr_Job -> EvtQueue [i] = SXR_NIL_JOB;
    }

    sxr_Job -> Load        = 0;
    sxr_Job -> IdxFree     = 0;
    sxr_Job -> Active      = SXR_NIL_JOB;
    sxr_Job -> FirstReady  = SXR_NIL_JOB;
    sxr_Job -> BitMapReady = 0;
    sxr_Job -> InitCnt     = 0;

    // DEBUG ?
    sxr_Job -> LastActive = SXR_NIL_JOB; //?
    // DEBUG.END
    sxr_InitTimer (sxr_TimerDelivery, NIL, NIL, SXR_FRAMED_TIMER);

#ifdef JOB_DEBUG
    // Protect the full stack ?
    hal_DbgPageProtectDisable(HAL_DBG_PAGE_UD0);
    hal_DbgPageProtectSetup(HAL_DBG_PAGE_10,HAL_DBG_PAGE_WRITE_TRIGGER,(UINT32)&sxr_Job, (UINT32)&sxr_Job + 3);
    hal_DbgPageProtectEnable(HAL_DBG_PAGE_10);
#endif //JOB_DEBUG

}


// =============================================================================
// sxr_NewJob
// -----------------------------------------------------------------------------
/// Allocate a free job context.
/// @param  Desc    Pointer onto sxr_JbDesc_t
/// @return         Job number.
// =============================================================================
u8 sxr_NewJob (sxr_JbDesc_t const *Desc)
{
    u32 Status = sxr_EnterSc ();
    u8  JobNb = sxr_Job -> IdxFree;
    u32 IdxSort = 0;
    u8  MemJobNb = JobNb;
// u32 BitMapReady = sxr_Job -> BitMapReady;
    u32 i;

// sxr_Job -> BitMapReady = 0;

    sxr_Job -> IdxFree = sxr_Job -> Ctx [sxr_Job -> IdxFree].Free;

    if (JobNb == SXR_NIL_JOB)
    {
        sxr_CheckJob ();
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No more free job.\n",0x06bb0001)));
    }

    while ((IdxSort < sxr_Job -> Load)
            &&     (Desc -> Priority > sxr_Job -> Ctx [sxr_Job -> Sorted [IdxSort]].Desc -> Priority))
    {
        IdxSort++;
    }

    for (i=IdxSort; i<=sxr_Job -> Load; i++)
    {
        sxr_Job -> Ctx [MemJobNb].IdxSort = (u8)i;
        SWAPT (MemJobNb, sxr_Job -> Sorted [i], u8);

//  if (BitMapReady & (1 << i))
        {
            //BitMapReady &= ~(1 << i);
//   sxr_Job -> BitMapReady |=  (1 << (i+1));
        }
    }
    //sxr_Job -> BitMapReady = BitMapReady;

    sxr_Job -> BitMapReady = (sxr_Job -> BitMapReady & ~(0xFFFFFFFF << IdxSort))
                             | ((sxr_Job -> BitMapReady & (0xFFFFFFFF << IdxSort)) << 1);

    sxr_Job -> Load++;

    sxr_ExitSc (Status);

    if (Desc -> Id == 0)
    {
        SXS_RAISE ((_SXR | TABORT|TDB,TSTR("Job Id may not be 0.\n",0x06bb0002)));
    }

    sxr_Job -> Ctx [JobNb].Desc     = Desc;
    sxr_Job -> Ctx [JobNb].State    = SXR_ALLOCATED_JOB;

    sxr_Job -> Ctx [JobNb].StackTop = (u32 *)_sxr_HMalloc ((u16)(SXR_SET_STACK(sxr_Job -> Ctx [JobNb].Desc -> StackSize) << 2), SXR_JB_STCK_HEAP);
    sxr_Job->Ctx[JobNb].StackBottom = sxr_Job -> Ctx [JobNb].StackTop
                                      + SXR_SET_STACK(sxr_Job -> Ctx [JobNb].Desc -> StackSize);

    if (sxr_Job -> Ctx [JobNb].StackTop == (u32 *) NIL)
    {
        SXS_RAISE ((_SXR | TABORT|TDB,TSTR("Out of memory for new Job\n",0x06bb0003)));
    }


    return JobNb;
}


// =============================================================================
// sxr_FreeJob
// -----------------------------------------------------------------------------
/// Free a job context.
/// @param  JobNb   Job number.
// =============================================================================
void sxr_FreeJob (u8 JobNb)
{
    sxr_CheckJobNb(JobNb);

    u32 Status = sxr_EnterSc ();
//    u32 BitMapReady = sxr_Job -> BitMapReady;
    u32 i;
    u32 IdxSort = sxr_Job -> Ctx [JobNb].IdxSort;

    if (!(sxr_Job -> Ctx [JobNb].State & (SXR_ALLOCATED_JOB | SXR_STOPPED_JOB)))
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(2),TSTR("Bad state for release 0x%x (%i)\n",0x06bb0004),sxr_Job -> Ctx [JobNb].State, JobNb));
    }

    sxr_Job -> Ctx [JobNb].Free = sxr_Job -> IdxFree;
    sxr_Job -> IdxFree = JobNb;

    for (i=IdxSort; i<sxr_Job -> Load; i++)
    {
//        if (sxr_Job -> BitMapReady & ((u32)1 << sxr_Job -> Ctx [sxr_Job -> Sorted [i+1]].IdxSort))
//        {
//            BitMapReady &= ~(1 << (i + 1)); // ~((u32)1 << sxr_Job -> Ctx [sxr_Job -> Sorted [i+1]].IdxSort);
//            BitMapReady |=  (1 << i);
//        }
        if(sxr_Job -> Sorted [i+1] != SXR_NIL_JOB)
        {
            sxr_Job -> Ctx [sxr_Job -> Sorted [i+1]].IdxSort = (u8)i;
        }
        sxr_Job -> Sorted [i] = sxr_Job -> Sorted [i+1];
    }

//     sxr_Job -> BitMapReady = BitMapReady;

    sxr_Job -> BitMapReady = (sxr_Job -> BitMapReady & ~(0xFFFFFFFF << IdxSort))
                             | ((sxr_Job -> BitMapReady &  (0xFFFFFFFF << (IdxSort + 1))) >> 1);

    sxr_Job -> FirstReady = 0;

    sxr_Job -> Load--;

    sxr_HFree (sxr_Job -> Ctx [JobNb].StackTop);
    sxr_Job -> Ctx [JobNb].IdxSort = SXR_NIL_JOB;
    sxr_Job -> Ctx [JobNb].StackTop = NIL;
    sxr_Job -> Ctx [JobNb].State    = SXR_FREE_JOB;

    sxr_ExitSc (Status);
}




// =============================================================================
// sxr_StartJob
// -----------------------------------------------------------------------------
/// Start identified job with the data context given in parameter.
/// @param  JobNb   job number.
/// @param  DataCtx pointer onto data context.
// =============================================================================
void sxr_StartJob (u8 JobNb, void *DataCtx)
{
    sxr_CheckJobNb(JobNb);

    u32 Status = sxr_EnterSc ();
//    sxr_EnterScSchedule ();

    if (!(sxr_Job -> Ctx [JobNb].State & (SXR_ALLOCATED_JOB | SXR_STOPPED_JOB)))
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(2),TSTR("Bad state to start job 0x%x %i\n",0x06bb0005), sxr_Job -> Ctx [JobNb].State, JobNb, sxr_CheckJob ()));
    }

    sxr_Job -> Ctx [JobNb].State = SXR_INIT_JOB;
    sxr_Job -> InitCnt++;
    sxr_Job -> Ctx [JobNb].Data  = DataCtx;

    sxr_Job -> Ctx [JobNb].Sp = sxr_Job -> Ctx [JobNb].StackTop + SXR_SET_STACK(sxr_Job -> Ctx [JobNb].Desc -> StackSize) - 1;

#ifdef JOB_DEBUG
    hal_DbgPageProtectDisable(HAL_DBG_PAGE_UD0);
#endif

#ifdef __SXR_STACK_CHECK__
    memset (sxr_Job -> Ctx [JobNb].StackTop, SXR_MEM_PATTERN, SXR_SET_STACK(sxr_Job -> Ctx [JobNb].Desc -> StackSize) << 2);
#else
    sxr_Job -> Ctx [JobNb].StackTop [0] = SXR_DW_MEM_PATTERN;
#endif

    SX_PROFILE_JOB_START_ENTER(JobNb);
    sxr_JobSetUp (JobNb, &sxr_Job -> Ctx [JobNb]);
    SX_PROFILE_JOB_START_EXIT(JobNb);

    sxr_JobSwap (&sxr_Job -> Ctx [JobNb].StackSwp, &sxr_Job -> Ctx [JobNb].Sp);

//    sxr_ExitScSchedule ();
    sxr_ExitSc (Status);
}



// =============================================================================
// sxr_GetJobId
// -----------------------------------------------------------------------------
/// Retreived the user Id of the job.
/// @param  JobNb   Job number.
/// @return         User Id.
// =============================================================================
u16 sxr_GetJobId (u16 JobNb)
{
    return sxr_Job -> Ctx [JobNb].Desc -> Id;
}



// =============================================================================
// sxr_GetActiveJob
// -----------------------------------------------------------------------------
/// Retrieve the current active job number.
/// @return Active job number.
// =============================================================================
u8 sxr_GetActiveJob (void)
{
    return sxr_Job -> Active;
}



// =============================================================================
// sxr_GetJobState
// -----------------------------------------------------------------------------
/// Get the state of the identified job.
/// @param  JobNb   Job number.
/// @return         State.
// =============================================================================
u16 sxr_GetJobState (u8 JobNb)
{
    return sxr_Job -> Ctx [JobNb].State;
}


// =============================================================================
// sxr_NextFrameClear
// -----------------------------------------------------------------------------
/// Check whether the next frame is clear.
/// @return TRUE if next frame is clear
// =============================================================================
u8 sxr_NextFrameClear (void)
{
    return (!(sxr_Job -> EvtStatus [SXR_EVT_FRAME_EXPECTED] & ~(1 << SXR_FRAME_END_EVT ) & ~(1 << SXR_CLEAR_EVT)));
}



// =============================================================================
// sxr_GetJobParam
// -----------------------------------------------------------------------------
/// Get the job data context.
/// parameter \p DataCtx of sxr_StartJob().
/// @param  JobNb   job number.
/// @return         pointer onto data context.
// =============================================================================
void *sxr_GetJobParam (u8 JobNb)
{
    if (sxr_Job -> Ctx [JobNb].State & (SXR_ALLOCATED_JOB | SXR_FREE_JOB))
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(2),TSTR("Bad state to get job parameter 0x%x %i\n",0x06bb0006), sxr_Job -> Ctx [JobNb].State, JobNb));
    }

    return sxr_Job -> Ctx [JobNb].Data;
}


// =============================================================================
// sxr_GetJobName
// -----------------------------------------------------------------------------
/// Get the active job name.
/// @return the active job name or "No Active Job"
// =============================================================================
const ascii *sxr_GetJobName (void)
{
    const ascii *NoJobName = "No Active Job";

    if (sxr_Job -> Active != SXR_NIL_JOB)
    {
        return sxr_Job -> Ctx [sxr_Job -> Active].Desc -> Name;
    }
    else
    {
        return NoJobName;
    }
}

// =============================================================================
// sxr_GetLastJobName
// -----------------------------------------------------------------------------
/// Get the last active job name.
/// @return the last active job name or "No Active Job"
// =============================================================================
const ascii *sxr_GetLastJobName (void)
{
    const ascii *NoJobName = "No Active Job";

    if (sxr_Job -> LastActive != SXR_NIL_JOB)
    {
        return sxr_Job -> Ctx [sxr_Job -> LastActive].Desc -> Name;
    }
    else
    {
        return NoJobName;
    }
}



// =============================================================================
// sxr_ChkJob
// -----------------------------------------------------------------------------
/// Print status for a job.
/// @param  JbNb    job number.
// =============================================================================
void sxr_ChkJob (u8 JbNb)
{
    sxr_CheckJobNb(JbNb);

    if (sxr_Job -> Ctx [JbNb].State != SXR_FREE_JOB)
    {
        SXS_FPRINTF ((_SXR|TSTDOUT|TSMAP(1)|TDB|TNB_ARG(3),TSTR("Job %s (%i, 0x%x) ",0x06bb0007), sxr_Job -> Ctx [JbNb].Desc -> Name, JbNb, sxr_Job -> Ctx [JbNb].Desc -> Id));

        switch (sxr_Job -> Ctx [JbNb].State & ~(u16)0x0F)
        {
            case SXR_ALLOCATED_JOB :
                SXS_FPRINTF ((_SXR|TIDU|TSTDOUT|TDB,TSTR("is allocated.\n",0x06bb0008)));
                break;

            case SXR_INIT_JOB :
                SXS_FPRINTF ((_SXR|TIDU|TSTDOUT|TDB,TSTR("is starting.\n",0x06bb0009)));
                break;

            case SXR_ACTIVE_JOB :
                SXS_FPRINTF ((_SXR|TIDU|TSTDOUT|TDB,TSTR("is active.\n",0x06bb000a)));
                break;

            case SXR_WAIT_DUR_JOB :
                SXS_FPRINTF ((_SXR|TIDU|TSTDOUT|TDB|TNB_ARG(1),TSTR("wait %i frames.\n",0x06bb000b), sxr_GetJobWaitingPeriod ((u16)JbNb, SXR_JOB_NB_IDX)));
                break;

            case SXR_WAIT_EVT_JOB(0) :
                SXS_FPRINTF ((_SXR|TIDU|TSTDOUT|TDB|TNB_ARG(1),TSTR("wait event %i.\n",0x06bb000c), SXR_GET_EVT(sxr_Job -> Ctx [JbNb].State)));
                break;

            case SXR_STOPPED_JOB :
                SXS_FPRINTF ((_SXR|TIDU|TSTDOUT|TDB,TSTR("is stopped.\n",0x06bb000d)));
                break;

            default :
                SXS_FPRINTF ((_SXR|TIDU|TSTDOUT|TDB,TSTR("is broken...\n",0x06bb000e)));
        }

#ifdef __SXR_STACK_CHECK__
        {
            u32 j = 0;

            while (sxr_Job -> Ctx [JbNb].StackTop [j++] == SXR_DW_MEM_PATTERN);

            SXS_FPRINTF ((_SXR|TIDU|TSTDOUT|TDB|TNB_ARG(5),TSTR("Stack: %lu/%lu Top 0x%lx <- 0x%lx Data 0x%lx\n",0x06bb000f),
                          ((SXR_SET_STACK(sxr_Job -> Ctx [JbNb].Desc -> StackSize) << 2) - (j << 2)),
                          SXR_SET_STACK(sxr_Job -> Ctx [JbNb].Desc -> StackSize) << 2,
                          sxr_Job -> Ctx [JbNb].StackTop,
                          sxr_Job -> Ctx [JbNb].StackTop + SXR_SET_STACK(sxr_Job -> Ctx [JbNb].Desc -> StackSize),
                          sxr_Job -> Ctx [JbNb].Data));
        }
#endif
    }
}


// =============================================================================
// sxr_CheckJob
// -----------------------------------------------------------------------------
/// Print status for all allocated jobs.
/// @return number of jobs.
// =============================================================================
u8 sxr_CheckJob (void)
{
    u32 i;
    // Before the OS is started, sxr_Job doesn't exist,
    // but this check function can be called by the Exception
    // handling (in GDB) would an exception occurs even before
    // the OS is started
    if (sxr_Job != NULL)
    {
        SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(1),TSTR("BitMapReady 0x%x",0x06bb0010), sxr_Job -> BitMapReady);
        for (i=0; i < SXR_NB_MAX_JOB; i++)
        {
            sxr_ChkJob (i);
        }

        return sxr_Job -> Load;
    }
    else
    {
        return 0;
    }
}


// =============================================================================
// sxs_CheckStkJb
// -----------------------------------------------------------------------------
/// Find a job from a stack pointer and Print status for this job.
/// @param  Sp  Stack Pointer
/// @return True if job was found.
// =============================================================================
u8 sxs_CheckStkJb (u32 *Sp)
{
    u32 i;

    // Before the OS is started, sxr_Job doesn't exist,
    // but this check function can be called by the Exception
    // handling (in GDB) would an exception occurs even before
    // the OS is started
    if (sxr_Job != NULL)
    {
        for (i=0; i < SXR_NB_MAX_JOB; i++)
        {
            if (sxr_Job -> Ctx [i].State != SXR_FREE_JOB)
            {
                if ((Sp >= sxr_Job -> Ctx [i].StackTop)
                        &&  (Sp < (sxr_Job -> Ctx [i].StackTop + SXR_SET_STACK(sxr_Job -> Ctx [i].Desc -> StackSize))))
                {
                    SXS_TRACE (_SXR|TSTDOUT|TDB,TSTR("Stack found for Job\n",0x06bb0011));
                    sxr_ChkJob (i);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}


// =============================================================================
// sxs_GetJobSpById
// -----------------------------------------------------------------------------
/// Get the stack pointer from the job context list.
/// @param Id Job Id.
/// @return The stack pointer.
// =============================================================================
u32 * sxr_GetJobSpById(u16 Id)
{
    UINT32 jobNum;
    for (jobNum=0; jobNum<SXR_NB_MAX_JOB; jobNum++)
    {
        if (sxr_Job->Ctx [jobNum].State == SXR_FREE_JOB)
        {
            continue;
        }
        if  ( sxr_Job->Ctx[jobNum].Desc != NULL && sxr_Job->Ctx[jobNum].Desc->Id == Id)
        {
            break;
        }
    }
    if (jobNum == SXR_NB_MAX_JOB)
    {
        return NULL;
    }

    if ((sxr_Job->Ctx [jobNum].State & (SXR_INIT_JOB|
                                        SXR_ACTIVE_JOB|
                                        SXR_WAIT_DUR_JOB|
                                        SXR_WAIT_EVT_JOB(0))) == 0)
    {
        return NULL;
    }
    return sxr_Job->Ctx [jobNum].Sp;
}


#if (!defined(__TARGET__)) || defined (__SIMU__)
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <assert.h>

jmp_buf sxr_JobCtxJmp [2 * SXR_NB_MAX_JOB];


// =============================================================================
// sxr_JobSetUp
// -----------------------------------------------------------------------------
/// Set up a job context
/// @param  JobNb   Job number
/// @param  JobCtx  job context to set up.
// =============================================================================
void sxr_JobSetUp (u8 NoJob, sxr_JbCtx_t *JobCtx)
{
    u32 *CtxJump;
    u32 *Sp;

    CtxJump = (u32 *)&sxr_JobCtxJmp [NoJob];
    Sp      = JobCtx -> Sp;

    JobCtx -> Sp       = (u32 *)NoJob;
    JobCtx -> StackSwp = (u32 *)(NoJob + SXR_NB_MAX_JOB);

    setjmp (sxr_JobCtxJmp [NoJob]);

    Sp -= 2;
    *Sp = (u32) JobCtx -> Data;

#if defined (__BORLANDC__) && defined (__WIN32__)
    CtxJump [4] = (u32) Sp;
    CtxJump [5] = (u32) JobCtx -> Desc -> JbBody;
    Sp += 1;
#elif defined (linux)
    Sp -= 1;
    CtxJump [4] = (u32) Sp;
    CtxJump [5] = (u32) JobCtx -> Desc -> JbBody;
#elif defined (__MSDOS__)
    {
        u16 *ZobCtx = (u16 *)CtxJump;
        u32 Zob;
        u16 *ZZob = (u16 *)&Zob;

        Sp -= 1;
        Zob = (u32) Sp;
        ZobCtx [0] = ZZob [0]; // Sp
        ZobCtx [1] = ZZob [1]; // Ss

        Zob = (u32) JobCtx -> Desc -> JbBody;
        ZobCtx [3] = ZZob [1]; // Cs
        ZobCtx [4] = ZZob [0]; // Ip
    }
#else
    Sp -= 2;
    CtxJump [4] = (u32) Sp;
    CtxJump [5] = (u32) JobCtx -> Desc -> JbBody;
#endif
    *Sp = NoJob;
}


// =============================================================================
// sxr_JobSwap
// -----------------------------------------------------------------------------
/// Save current (old) job and restore new job
/// @param  OldJob  place to store old stack pointer
/// @param  NewJob  place to get new stack pointer to restore
// =============================================================================
void sxr_JobSwap (u32 **OldJob, u32 **NewJob)
{
    static u8 MemJob [SXR_NB_MAX_JOB];
// u32 *NewJobSp = (u32 *)((u32 *)(&sxr_JobCtxJmp [(u8)*NewJob])) [4];
// u8 OldActive = sxr_Job -> Active;
// u8 NewActive = *NewJobSp;



    if (setjmp (sxr_JobCtxJmp [(u8)*OldJob]) == 0)
    {
        //    u32 *OldJobSp = (u32 *)((u32 *)(&sxr_JobCtxJmp [(u8)*OldJob])) [4];

        //    *OldJobSp = sxr_Job -> Active;
//        sxr_Job -> Active = NewActive;

        if ((u8) *OldJob >= SXR_NB_MAX_JOB)
        {
            MemJob [(u8)*OldJob - SXR_NB_MAX_JOB] = sxr_Job -> Active;
        }

        if ((u8) *NewJob >= SXR_NB_MAX_JOB)
        {
            sxr_Job -> Active = MemJob [(u8)*NewJob - SXR_NB_MAX_JOB];
        }
        else
        {
            sxr_Job -> Active = (u8) *NewJob;
        }

        longjmp (sxr_JobCtxJmp [(u8)*NewJob], TRUE);
    }
    else
    {
//        *NewJobSp = sxr_Job -> Active;
//        sxr_Job -> Active = OldActive;
    }

//    else
//    {
//        *OldJobSp = sxr_Job -> Active;
//        sxr_Job -> Active = *NewJobSp;
//    }
}

#endif


// =============================================================================
// JobSwap_SpyBefore
// -----------------------------------------------------------------------------
/// Executed before the job swap
/// @param  OldJob  place to store old stack pointer
/// @param  NewJob  place to get new stack pointer to restore
// =============================================================================
VOID JobSwap_SpyBefore(UINT32 ** OldJobSp, UINT32 ** NewJobSp)
{
    SX_PROFILE_JOB_EXIT(sxr_Job->Active);

#ifdef JOB_DEBUG

// Page spy 4 is used for whole stack or before part

    hal_DbgPageProtectDisable(HAL_DBG_PAGE_UD0);

#endif
}




// =============================================================================
// JobSwap_SpyAfter
// -----------------------------------------------------------------------------
/// Executed after job swap
/// @param  OldJob  place to store old stack pointer
/// @param  NewJob  place to get new stack pointer to restore
// =============================================================================
VOID JobSwap_SpyAfter (UINT32 ** OldJobSp, UINT32 ** NewJobSp)
{
    UINT8 jobNum;

    jobNum = sxr_Job->Active;

    // PROFILING
    // If the reasoning is correct, sxr_Job->Active is now the job we're in!
    SX_PROFILE_JOB_ENTER(jobNum);


    // The good old assert ...

    if (jobNum >= SXR_NB_MAX_JOB && jobNum != SXR_NIL_JOB)
    {
        hal_DbgAssert(
            "\n##############################################\n"
            "# FATAL assert \n"
            "#    --> Active Job = %d\n"
            "#    --> Old Job = 0x%08x\n"
            "#    --> New Job = 0x%08x\n"
            "##############################################",
            sxr_Job->Active, OldJobSp, NewJobSp);
    }


#ifdef JOB_DEBUG
    hal_DbgPageProtectSetup(HAL_DBG_PAGE_UD0, HAL_DBG_PAGE_WRITE_TRIGGER,
                            (UINT32)(*OldJobSp), (UINT32) (*OldJobSp)+24);
    hal_DbgPageProtectEnable(HAL_DBG_PAGE_UD0);
#endif
}


// =============================================================================
// sxs_FuncName
// -----------------------------------------------------------------------------
/// Scope
/// @param
/// @return
// =============================================================================


