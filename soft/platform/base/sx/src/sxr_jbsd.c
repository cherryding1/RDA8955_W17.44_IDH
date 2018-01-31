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





#include "sxs_type.h"
#include "sxr_ops.h"
#include "sxs_io.h"
#include "sxs_lib.h"

#include "sxr_jbsd.hp"
#include "sxr_jbsd.h"
#include "sxp_debug.h"


// =============================================================================
// sxr_TimerDelivery
// -----------------------------------------------------------------------------
/// Notify that waited duration required by a job has expired.
/// The job remains in Wait state but is marked as ready in a bit map.
/// @param  Ctx Pointer onto context which contains job number.
// =============================================================================
VOID sxr_TimerDelivery (u32 *Ctx)
{
    u8 JobNb = (u8)Ctx [SXR_JOB_NB_IDX];

    if (sxr_Job -> Ctx [JobNb].IdxSort < sxr_Job -> FirstReady)
    {
        sxr_Job -> FirstReady = sxr_Job -> Ctx [JobNb].IdxSort;
    }

    sxr_Job -> BitMapReady |= ((u32)1 << sxr_Job -> Ctx [JobNb].IdxSort);
}


// =============================================================================
// sxr_InitJobEventStatus
// -----------------------------------------------------------------------------
/// Init job event status at the beginning of a new frame period.
// =============================================================================
VOID sxr_InitJobEventStatus (VOID)
{
    if (sxr_Job -> EvtStatus [SXR_EVT_FRAME_EXPECTED])
    {
        SXS_RAISE ((_SXR|TDB|TNB_ARG(1),TSTR("Evt %x didn't occur\n",0x06a60001), sxr_Job -> EvtStatus [SXR_EVT_FRAME_EXPECTED], sxr_CheckJob()));
    }

    sxr_Job -> EvtStatus [SXR_EVT_FRAME_EXPECTED]     = sxr_Job -> EvtStatus [SXR_EVT_NXT_FRAME_EXPECTED];
    sxr_Job -> EvtStatus [SXR_EVT_NXT_FRAME_EXPECTED] =
        sxr_Job -> EvtStatus [SXR_EVT_FRAME_PENDING]      = 0;
}


// =============================================================================
// sxr_FrameJobScheduling
// -----------------------------------------------------------------------------
/// Called on frame interrupt. Indicate the beginning of a new frame period.
/// @param  NbFrames    Number of frames occured since last frame interrupt.
// =============================================================================
VOID sxr_FrameJobScheduling (u16 NbFrames)
{
    u8 JobNb;

    sxr_Tampax (NbFrames, SXR_FRAMED_TIMER);

    if (sxr_Job -> EvtStatus [SXR_EVT_FRAME_EXPECTED] & (1 << SXR_NEXT_FRAME_EVT ))
    {
        sxr_Job -> EvtStatus [SXR_EVT_FRAME_EXPECTED] &= ~(1 << SXR_NEXT_FRAME_EVT);

        JobNb = sxr_Job -> EvtQueue [SXR_NEXT_FRAME_EVT];
        sxr_Job -> EvtQueue [SXR_NEXT_FRAME_EVT] = SXR_NIL_JOB;

        if (sxr_Job -> Ctx [JobNb].State == SXR_INIT_JOB)
        {
            sxr_Job -> InitCnt--;
        }

        sxr_Job -> Ctx [JobNb].State = SXR_ACTIVE_JOB;
        sxr_JobSwap (&sxr_Job -> Ctx [JobNb].StackSwp, &sxr_Job -> Ctx [JobNb].Sp);

#ifdef __SXR_MEM_CHECK__
        sxr_CheckUpdateCluster (NIL, 0);
#endif
    }

    if (sxr_Job -> FirstReady != SXR_NIL_JOB)
    {
//  u32 IdxSort = sxr_Job -> Ctx [sxr_Job -> FirstReady].IdxSort;
        u8 IdxSort = sxr_Job -> FirstReady;

        while (sxr_Job -> BitMapReady)
        {
            if (sxr_Job -> BitMapReady & ((u32)1 << IdxSort))
            {
                sxr_Job -> BitMapReady &= ~((u32)1 << IdxSort);
                JobNb = sxr_Job -> Sorted [IdxSort];

                if (sxr_Job -> Ctx [JobNb].State == SXR_INIT_JOB)
                {
                    sxr_Job -> InitCnt--;
                }

                sxr_Job -> Ctx [JobNb].State = SXR_ACTIVE_JOB;
                sxr_JobSwap (&sxr_Job -> Ctx [JobNb].StackSwp, &sxr_Job -> Ctx [JobNb].Sp);

#ifdef __SXR_MEM_CHECK__
                sxr_Job -> LastActive = JobNb;
                sxr_CheckUpdateCluster (NIL, 0);
                sxr_Job -> LastActive = SXR_NIL_JOB;
#endif
            }
            if (sxr_Job -> BitMapReady & ~(0xFFFFFFFF << IdxSort))
            {
                IdxSort = 0;
            }
            else
            {
                IdxSort++;
            }
        }
        sxr_Job -> FirstReady = SXR_NIL_JOB;
    }

}


// =============================================================================
// sxr_WaitJDuration
// -----------------------------------------------------------------------------
/// Wait for the required number of frames periods.
/// @param  NbFrames    Number of frames to wait.
// =============================================================================
VOID sxr_WaitJDuration (u16 NbFrames)
{
    if (NbFrames == 0)
    {
        return;
    }

    if (sxr_Job -> Active == SXR_NIL_JOB || sxr_Job->Active >= SXR_NB_MAX_JOB)
    {
        SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(1),TSTR("active jobId = %d\n",0x06a60012), sxr_Job -> Active);
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Wait is reserved for job (%x)\n",0x06a60002)));
    }


    if (sxr_Job -> Ctx [sxr_Job -> Active].StackTop [0] != SXR_DW_MEM_PATTERN)
    {
        SXS_RAISE ((_SXR|TABORT| TSMAP(1)|TDB|TNB_ARG(1),TSTR("Job %s has a broken stack\n",0x06a60003), sxr_Job -> Ctx [sxr_Job -> Active].Desc -> Name));
    }

    sxr_EnterSc ();

    u32 Ctx [SXR_TIMER_CTX_SIZE] = {};
    Ctx [SXR_JOB_NB_IDX] = sxr_Job -> Active | SXR_JOB_ID;
    Ctx [SXR_JOB_ID_IDX] = sxr_Job -> Ctx [sxr_Job -> Active].Desc -> Id | SXR_JOB_ID;

    sxr_SetTimer (NbFrames, Ctx, 0, SXR_FRAMED_TIMER);

    if (sxr_Job -> Ctx [sxr_Job -> Active].State == SXR_INIT_JOB)
    {
        sxr_Job -> InitCnt--;
    }

    sxr_Job -> Ctx [sxr_Job -> Active].State = SXR_WAIT_DUR_JOB;

    sxr_JobSwap (&sxr_Job -> Ctx [sxr_Job -> Active].Sp, &sxr_Job -> Ctx [sxr_Job -> Active].StackSwp);

#ifdef __SXR_MEM_CHECK__
    sxr_Job -> LastActive = sxr_Job -> Active;
    sxr_CheckUpdateCluster (NIL, 0);
    sxr_Job -> LastActive = SXR_NIL_JOB;
#endif
}


// =============================================================================
// sxr_GetJobWaitingPeriod
// -----------------------------------------------------------------------------
/// Return the number of frame period to be waited of a job in wait state.
/// @param  Id      job number or job user Id.
/// @param  NbVsId  boolean indicating if parameter Id corresponds to the
///                 job number or to the user Id.
/// @return         Duration in frame period or SXR_NO_DUE_DATE when
///                 job is not found in wait state.
// =============================================================================
u32 sxr_GetJobWaitingPeriod (u16 Id, u8 NbVsId)
{
    sxr_Timer_t Timer;

    Timer.Ctx = Id | SXR_JOB_ID;
    Timer.Msk = 0xFFFFFFFF;
    Timer.Id  = 0;
    Timer.Idx = NbVsId;

    return sxr_TimerQuery (&Timer, SXR_FRAMED_TIMER);
}


// =============================================================================
// sxr_FrameBoundary
// -----------------------------------------------------------------------------
/// Look for the virtual frame end boundary. Must be invoked in
/// each interruption that is likely to raise a short lived event.
/// Events clear and last, when expected, are raised when the
/// virtual end of the frame is detected.
/// @return TRUE when the end frame boundary has been reached FALSE otherwise.
// =============================================================================
u8 sxr_FrameBoundary (VOID)
{
    if (!(sxr_Job -> EvtStatus [SXR_EVT_FRAME_EXPECTED] & ~(1 << SXR_FRAME_END_EVT ) & ~(1 << SXR_CLEAR_EVT)))
    {
        if  ((sxr_Job -> EvtStatus [SXR_EVT_NXT_FRAME_EXPECTED] == 0)
                &&   (sxr_Job -> EvtQueue [SXR_CLEAR_EVT] != SXR_NIL_JOB))
        {
            sxr_RaiseJEvent (SXR_CLEAR_EVT, 0);
        }

//  if (sxr_Job -> EvtStatus [SXR_EVT_FRAME_EXPECTED] & (1 << SXR_FRAME_END_EVT ))

        if ((sxr_Job -> EvtQueue [SXR_FRAME_END_EVT] != SXR_NIL_JOB)
                &&  (sxr_Job -> SkipFrameEndEvt == FALSE))
        {
            sxr_RaiseJEvent (SXR_FRAME_END_EVT, 0);
        }

        sxr_Job -> SkipFrameEndEvt = FALSE;

        return TRUE;
    }

    return FALSE;
}


// =============================================================================
// sxr_RaiseJEvent
// -----------------------------------------------------------------------------
/// Raise a job event. A job waiting for the raised event
/// is scheduled immediately.
/// @param  Evt     Event number.
/// @param  Data    Event data.
// =============================================================================
VOID sxr_RaiseJEvent (u8 Evt, VOID *Data)
{
    u8  JobNb;
    u32 Status = sxr_EnterSc ();

    sxr_Job -> EvtData [Evt] = Data;

    if (sxr_Job -> EvtQueue [Evt] != SXR_NIL_JOB)
    {
        JobNb = sxr_Job -> EvtQueue [Evt];
        sxr_Job -> EvtQueue [Evt] = SXR_NIL_JOB;

        if (sxr_Job -> EvtStatus [SXR_EVT_NXT_FRAME_EXPECTED] & (1 << Evt))
        {
            SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Event %i expected in next frame already available.\n",0x06a60004), Evt));
        }

        sxr_Job -> EvtStatus [SXR_EVT_PENDING]        &= ~(1 << Evt);
        sxr_Job -> EvtStatus [SXR_EVT_FRAME_PENDING]  &= ~(1 << Evt);
        sxr_Job -> EvtStatus [SXR_EVT_FRAME_EXPECTED] &= ~(1 << Evt);

        if (sxr_Job -> Ctx [JobNb].State == SXR_INIT_JOB)
        {
            sxr_Job -> InitCnt--;
        }

        sxr_Job -> Ctx [JobNb].State = SXR_ACTIVE_JOB;

        sxr_JobSwap (&sxr_Job -> Ctx [JobNb].StackSwp, &sxr_Job -> Ctx [JobNb].Sp);

#ifdef __SXR_MEM_CHECK__
        sxr_Job -> LastActive = JobNb;
        sxr_CheckUpdateCluster (NIL, 0);
        sxr_Job -> LastActive = SXR_NIL_JOB;
#endif
    }

    sxr_ExitSc (Status);
}


// =============================================================================
// sxr_RaiseLongLivedJEvent
// -----------------------------------------------------------------------------
/// Raise a long lived job event. A job waiting for the
/// raised event is scheduled immediately. Event is held until
/// cleared or consumed.
/// @param  Evt     Event number.
/// @param  Data    Event data.
// =============================================================================
VOID sxr_RaiseLongLivedJEvent (u8 Evt, VOID *Data)
{
    sxr_Job -> EvtStatus [SXR_EVT_PENDING] |= (1 << Evt);
    sxr_RaiseJEvent (Evt, Data);
}


// =============================================================================
// sxr_RaiseFrameJEvent
// -----------------------------------------------------------------------------
/// Raise a frame lived job event. A job waiting for the
/// raised event is scheduled immediately. Event is held for the
/// duration of the frame only.
/// @param  Evt     Event number.
/// @param  Data    Event data.
// =============================================================================
VOID sxr_RaiseFrameJEvent (u8 Evt, VOID *Data)
{
    sxr_Job -> EvtStatus [SXR_EVT_FRAME_PENDING] |= (1 << Evt);
    sxr_RaiseJEvent (Evt, Data);
}



// =============================================================================
// sxr_ClearJEvent
// -----------------------------------------------------------------------------
/// Clear an event.
/// @param  Evt Event number.
/// @return
// =============================================================================
VOID sxr_ClearJEvent (u8 Evt)
{
    sxr_Job -> EvtStatus [SXR_EVT_PENDING]       &= ~(1 << Evt);
    sxr_Job -> EvtStatus [SXR_EVT_FRAME_PENDING] &= ~(1 << Evt);
}



// =============================================================================
// sxr_WaitJEvent
// -----------------------------------------------------------------------------
/// Wait for a job event.
/// @param  Evt     Event number.
/// @param  When    specifiy if the expected event is a frame lived
///                 (SXR_THIS_FRAME or SXR_NEXT_FRAME) or a long lived event
///                 (SXR_ANY_FRAME).
// =============================================================================
VOID *sxr_WaitJEvent (u8 Evt, u8 When)
{
    u32 Status;

    if (sxr_Job -> Active == SXR_NIL_JOB)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Wait is reserved for job (%x)\n",0x06a60005), Evt));
    }

    if (sxr_Job -> EvtQueue [Evt] != SXR_NIL_JOB)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(3),TSTR("Event %i already expected (%i/%i)\n",0x06a60006), Evt, sxr_Job -> EvtQueue [Evt], sxr_Job -> Active, sxr_CheckJob ()));
    }

    if ((Evt == SXR_NEXT_FRAME_EVT)
            &&  (When == SXR_CUR_FRAME ))
    {
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("Fint'll occur next frame !\n",0x06a60007)));
    }

    if (sxr_Job -> Ctx [sxr_Job -> Active].StackTop [0] != SXR_DW_MEM_PATTERN)
    {
        SXS_RAISE ((_SXR|TABORT|TSMAP(1)|TDB|TNB_ARG(1),TSTR("Job %s has a broken stack\n",0x06a60008), sxr_Job -> Ctx [sxr_Job -> Active].Desc -> Name));
    }

    Status = sxr_EnterSc ();

    if ((sxr_Job -> EvtStatus [SXR_EVT_PENDING] |
            sxr_Job -> EvtStatus [SXR_EVT_FRAME_PENDING]) & (1 << Evt))
    {
        if (When == SXR_NEXT_FRAME)
        {
            SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Event %i expected in next frame already available.\n",0x06a60009), Evt));
        }

        sxr_Job -> EvtStatus [SXR_EVT_PENDING]       &= ~(1 << Evt);
        sxr_Job -> EvtStatus [SXR_EVT_FRAME_PENDING] &= ~(1 << Evt);
    }
    else
    {
        sxr_Job -> EvtQueue [Evt] = sxr_Job -> Active;

        if (When == SXR_CUR_FRAME)
        {
            sxr_Job -> EvtStatus [SXR_EVT_FRAME_EXPECTED] |= 1 << Evt;
        }
        else if (When == SXR_NEXT_FRAME)
        {
            sxr_Job -> EvtStatus [SXR_EVT_NXT_FRAME_EXPECTED] |= 1 << Evt;
        }

        if (sxr_Job -> Ctx [sxr_Job -> Active].State == SXR_INIT_JOB)
        {
            sxr_Job -> InitCnt--;
        }

        sxr_Job -> Ctx [sxr_Job -> Active].State = SXR_WAIT_EVT_JOB(Evt);
        sxr_JobSwap (&sxr_Job -> Ctx [sxr_Job -> Active].Sp, &sxr_Job -> Ctx [sxr_Job -> Active].StackSwp);

#ifdef __SXR_MEM_CHECK__
        sxr_Job -> LastActive = sxr_Job -> Active;
        sxr_CheckUpdateCluster (NIL, 0);
        sxr_Job -> LastActive = SXR_NIL_JOB;
#endif
    }

    sxr_ExitSc (Status);

    return sxr_Job -> EvtData [Evt];
}




// =============================================================================
// sxr_AutoStop
// -----------------------------------------------------------------------------
/// Job suspend itself and switch into STOPPED state.
// =============================================================================
VOID sxr_AutoStop (VOID)
{
    sxr_EnterSc ();

    SXS_TRACE (_SXR|TDB|TNB_ARG(1),TSTR("AutoStop Job %i\n",0x06a6000a), sxr_Job -> Active);

    if (sxr_Job -> Ctx [sxr_Job -> Active].Desc -> JbExit)
    {
        sxr_Job -> Ctx [sxr_Job -> Active].Desc -> JbExit (sxr_Job -> Ctx [sxr_Job -> Active].Data, sxr_Job -> Active, FALSE);
    }

    if (sxr_Job -> Ctx [sxr_Job -> Active].State == SXR_INIT_JOB)
    {
        sxr_Job -> InitCnt--;
    }

    sxr_Job -> Ctx [sxr_Job -> Active].State = SXR_STOPPED_JOB;
    sxr_JobSwap (&sxr_Job -> Ctx [sxr_Job -> Active].Sp, &sxr_Job -> Ctx [sxr_Job -> Active].StackSwp);

#ifdef __SXR_MEM_CHECK__
    sxr_Job -> LastActive = sxr_Job -> Active;
    sxr_CheckUpdateCluster (NIL, 0);
    sxr_Job -> LastActive = SXR_NIL_JOB;
#endif
}



// =============================================================================
// sxr_StopJob
// -----------------------------------------------------------------------------
/// Suspend or delete one or several jobs.
/// When Id given in parameter is 0, job to be considered is
/// identified by its number. Otherwise, all job such that
/// JobId & Msk = parameter Id are considered for suspension/deletion.
/// @param  JobNb       Job number.
/// @param  Id
/// @param  Msk
/// @param  Deletion    when TRUE, job is deleted, othewise, job is suspended.
/// @return
// =============================================================================
VOID sxr_StopJob (u8 JobNb, u16 Id, u16 Msk, u8 Deletion)
{
    u8 i = 0;
    u8 NbJob, StartJob;

    sxr_CheckJobNb(JobNb);

    SXS_TRACE (_SXR|TDB|TSMAP(2)|TNB_ARG(4),TSTR("Delete job Nb %i %s (%i) BitMapReady 0x%x\n",0x06a6000b), JobNb, sxr_Job -> Ctx [JobNb].Desc -> Name, Deletion, sxr_Job -> BitMapReady);

    if (sxs_IoCtx.TraceBitMap [TGET_ID(_SXR)] & (1 << 4))
    {
        sxr_ChkJob (JobNb);
    }

    if (Id == 0)
    {
        StartJob = sxr_Job -> Ctx [JobNb].IdxSort;
        NbJob    = 1;

        sxr_CheckJobNb(JobNb);
    }
    else
    {
        StartJob = 0;
        NbJob    = sxr_Job -> Load;
    }

    for (JobNb=sxr_Job -> Sorted [StartJob]; i++<NbJob; JobNb=sxr_Job -> Sorted [++StartJob])
    {
        if (!(sxr_Job -> Ctx [JobNb].State & ( /* SXR_ALLOCATED_JOB| */ SXR_FREE_JOB))
                &&  ((sxr_Job -> Ctx [JobNb].Desc -> Id & Msk) == Id))
        {
            u32 Status = sxr_EnterSc();

            if ((sxr_Job -> Active == JobNb)
                    &&  (JobNb != SXR_NIL_JOB))
            {
                SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Active job may not be deleted %i\n",0x06a6000d), sxr_Job -> Active, sxr_CheckJob ()));
            }
            sxr_CheckJobNb(JobNb);
            SX_PROFILE_JOB_STOP_ENTER(JobNb);

            if (!(sxr_Job -> Ctx [JobNb].State & (SXR_INIT_JOB | SXR_ACTIVE_JOB)))
            {
                if ((sxr_Job -> Ctx [JobNb].State & SXR_WAIT_EVT_JOB(0)))
                {
                    u8 Evt = SXR_GET_EVT(sxr_Job -> Ctx [JobNb].State);

                    sxr_Job -> EvtQueue  [Evt]                         = SXR_NIL_JOB;
                    sxr_Job -> EvtStatus [SXR_EVT_FRAME_EXPECTED]     &= ~(1 << Evt);
                    sxr_Job -> EvtStatus [SXR_EVT_NXT_FRAME_EXPECTED] &= ~(1 << Evt);
                }
                else if (sxr_Job -> Ctx [JobNb].State == SXR_WAIT_DUR_JOB)
                {
                    sxr_Timer_t Timer = {0, 0xFFFFFFFF, 0, SXR_JOB_NB_IDX};

                    Timer.Ctx = JobNb | SXR_JOB_ID;

                    if (sxr_DeleteTimer (&Timer, NIL, TRUE, SXR_FRAMED_TIMER) != 1)
                    {
                        if (sxr_Job -> BitMapReady & ((u32)1 << sxr_Job -> Ctx [JobNb].IdxSort))
                        {
                            sxr_Job -> BitMapReady &= ~((u32)1 << sxr_Job -> Ctx [JobNb].IdxSort);
                        }
                        else
                        {
#if 0
                            /* May be the concerned job is in the pipe for activation. */
                            u32 IdxSort     = sxr_Job -> FirstReady;
                            u32 BitMapReady = sxr_Job -> BitMapReady;
                            u8  Found       = FALSE;

                            while ((BitMapReady) && (!Found))
                            {
                                if (BitMapReady & ((u32)1 << IdxSort))
                                {
                                    BitMapReady &= ~((u32)1 << IdxSort);

                                    if (JobNb == sxr_Job -> Sorted [IdxSort])
                                    {
                                        sxr_Job -> BitMapReady &= ~((u32)1 << IdxSort);
                                        Found = TRUE;
                                    }
                                }
                                IdxSort++;
                            }

                            if (!Found)
                            {
#endif
                                static u32 StatSc = 0;
                                StatSc = sxr_EnterSc();
                                sxr_ExitSc (StatSc);

                                sxr_CheckJob ();
                                sxr_CheckTimer ();

                                SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Job %i not found for deletion in state Wait\n",0x06a6000e), JobNb));
#if 0
                            }
#endif
                        }
                    }
                }
                else if (sxr_Job -> Ctx [JobNb].State == SXR_FREE_JOB)
                {
                    SXS_RAISE ((_SXR|TABORT|TNB_ARG(1)|TDB,TSTR("Unable to delete a free job %i",0x06a6000f), JobNb));
                }

                if (sxr_Job -> Ctx [JobNb].Desc -> JbExit)
//              &&  (!(sxr_Job -> Ctx [JobNb].State & (SXR_STOPPED_JOB | SXR_ALLOCATED_JOB))))
                {
                    sxr_Job -> Ctx [JobNb].Desc -> JbExit (sxr_Job -> Ctx [JobNb].Data, JobNb, // Deletion);
                                                           (u8)(Deletion ?
                                                                   sxr_Job -> Ctx [JobNb].State & (SXR_STOPPED_JOB | SXR_ALLOCATED_JOB) ?
                                                                   SXR_JOB_IS_STOPPED | SXR_JOB_IS_BEING_DELETED :
                                                                   SXR_JOB_IS_BEING_DELETED :
                                                                   FALSE));
                }

                if (sxr_Job -> Ctx [JobNb].State == SXR_INIT_JOB)
                {
                    sxr_Job -> InitCnt--;
                }

                sxr_Job -> Ctx [JobNb].State = SXR_STOPPED_JOB;

#ifdef JOB_DEBUG
                hal_DbgPageProtectDisable(HAL_DBG_PAGE_UD0);
#endif

                if (Deletion)
                {
                    sxr_FreeJob (JobNb);  // TBC
                    NbJob--;
                    StartJob--;
                }
            }
            else
            {
                sxr_CheckJob ();
                SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(2),TSTR("Job %i in state 0x%x may not be deleted.\n",0x06a60010), JobNb, sxr_Job -> Ctx [JobNb].State));
            }
            SX_PROFILE_JOB_STOP_EXIT(JobNb);
            sxr_ExitSc (Status);
        }
    }

}


// =============================================================================
// sxr_ResumeJob
// -----------------------------------------------------------------------------
/// Resume one or several jobs.
/// When Id given in parameter is 0, job to be considered is
/// identified but its number. Otherwise, all job such that
/// JobId & Msk = parameter Id are resumed.
/// @param  JobNb   Job number.
/// @param  Id
/// @param  Msk
// =============================================================================
VOID sxr_ResumeJob (u8 JobNb, u16 Id, u16 Msk)
{
    u32 i;
    u32 NbJob, StartJob;

    sxr_CheckJobNb(JobNb);

    if (Id == 0)
    {
        StartJob = sxr_Job -> Ctx [JobNb].IdxSort;
        NbJob    = 1;

        sxr_CheckJobNb(JobNb);
    }
    else
    {
        StartJob = 0;
        NbJob    = sxr_Job -> Load;
    }

    for (i=0; i<NbJob; i++)
    {
        if ((sxr_Job -> Ctx [sxr_Job -> Sorted [StartJob + i]].State & SXR_STOPPED_JOB)
                &&  ((sxr_Job -> Ctx [sxr_Job -> Sorted [StartJob + i]].Desc -> Id & Msk) == Id))
        {
            sxr_StartJob (sxr_Job -> Sorted [StartJob + i], sxr_Job -> Ctx [sxr_Job -> Sorted [StartJob + i]].Data);
        }
    }
}


// =============================================================================
// sxr_ResetJob
// -----------------------------------------------------------------------------
/// Reset one or several jobs.
/// When Id given in parameter is 0, job to be considered is
/// identified but its number. Otherwise, all job such that
/// JobId & Msk = parameter Id are reset.
/// @param  JobNb   Job number.
/// @param  Id
/// @param  Msk
// =============================================================================
VOID sxr_ResetJob (u8 JobNb, u16 Id, u16 Msk)
{
    u32 Status;

    sxr_CheckJobNb(JobNb);

    sxr_StopJob (JobNb, Id, Msk, FALSE);

    Status = sxr_EnterSc ();

    if (sxr_Job -> Ctx [JobNb].State == SXR_STOPPED_JOB)
    {
        sxr_ResumeJob (JobNb, Id, Msk);
    }

    sxr_ExitSc (Status);
}



// =============================================================================
// sxr_SearchJob
// -----------------------------------------------------------------------------
/// Search a job such that JobId & Msk = Id Parameter.
/// only the first found job is retrieved.
/// @param  Id
/// @param  Msk
/// @return     Job number or SXR_NIL_JOB.
// =============================================================================
u8 sxr_SearchJob (u16 Id, u16 Msk)
{
    u32 i;

    for (i=0; i<sxr_Job -> Load; i++)
    {
        if ((sxr_Job -> Ctx [sxr_Job -> Sorted [i]].State & (SXR_ALLOCATED_JOB | SXR_FREE_JOB))
                &&  ((sxr_Job -> Ctx [sxr_Job -> Sorted [i]].Desc -> Id & Msk) == Id))
        {
            return (u8)sxr_Job -> Sorted [i];
        }
    }
    return SXR_NIL_JOB;
}


// =============================================================================
// sxr_SkipFrameEndEvt
// -----------------------------------------------------------------------------
/// Tels sxr_FrameBoundary() to skip the #SXR_FRAME_END_EVT event
// =============================================================================
VOID sxr_SkipFrameEndEvt (VOID)
{
    sxr_Job -> SkipFrameEndEvt = TRUE;
}



// =============================================================================
// sxr_IsJobInitializing
// -----------------------------------------------------------------------------
/// Return the number of Jobs in the INIT state.
/// @return the number of Jobs in the INIT state.
// =============================================================================
u8 sxr_IsJobInitializing (VOID)
{
    return sxr_Job -> InitCnt;
}


// =============================================================================
// sxs_FuncName
// -----------------------------------------------------------------------------
/// Scope
/// @param
/// @return
// =============================================================================

