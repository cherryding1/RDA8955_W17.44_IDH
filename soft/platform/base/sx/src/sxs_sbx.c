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

#define __SXR_SBX_VAR__
#include "sxr_sbx.hp"
#undef __SXR_SBX_VAR__
#include "sxr_tksd.hp"
#include "sxs_io.h"
#include "sxr_tim.h"
#include "sxp_debug.h"

// =============================================================================
// sxr_SbxInit
// -----------------------------------------------------------------------------
/// Synchronization boxes data context initialization.
// =============================================================================
void sxr_SbxInit (void)
{
    u32 i;

    sxr_Sbx.Nb           = 0;
    sxr_Sbx.EnvIdxFree   = 0;
    sxr_Sbx.IdxFree      = 0;

    for (i=0; i<SXR_NB_MAX_MBX_ENV; i++)
    {
        sxr_Sbx.Env [i].EvtTag = FALSE;
        sxr_Sbx.Env [i].Next   = i+1;
    }

    sxr_Sbx.Env [SXR_NB_MAX_MBX_ENV-1].Next = SXR_NO_ENV;

    for (i=0; i<SXR_NB_TOTAL_SBX; i++)
    {
        sxr_Sbx.Queue [i].Msg.First  = SXR_NO_ENV;
        sxr_Sbx.Queue [i].Msg.Last   = SXR_NO_ENV;
        sxr_Sbx.Queue [i].Task.First = SXR_NO_TASK;
        sxr_Sbx.Queue [i].Task.Next  = SXR_NO_TASK;
        sxr_Sbx.Queue [i].Id         = i+1;
        sxr_Sbx.Queue [i].TrcLev     = 0;
    }

    sxr_Sbx.Queue [SXR_NB_TOTAL_SBX - 1].Id = SXR_NO_ID;

    for (i=0; i<1 + (SXR_NB_TOTAL_SBX / 32); i++)
    {
        sxr_Sbx.RmtSbx [i] = 0;
    }

    sxr_Sbx.TxMsg = NIL;
    sxr_Sbx.RxMsg = NIL;
}

// =============================================================================
// sxr_NewMailBox
// -----------------------------------------------------------------------------
/// provide a free mail box Id.
/// @return Mail box Id.
// =============================================================================
u8 sxr_NewMailBox (void)
{
    u32 Status = sxr_EnterSc ();

    u8 Id = sxr_Sbx.IdxFree;

    if (Id == SXR_NO_ID)
    {
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No more free mail boxe\n",0x06bf0001)));
    }

    sxr_Sbx.Nb++;
    sxr_Sbx.IdxFree = sxr_Sbx.Queue [Id].Id;
    sxr_Sbx.Queue[Id].Id = SXR_SBX_ALLOCATED;

    sxr_ExitSc (Status);

    return Id;
}

// =============================================================================
// sxr_FreeMailBox
// -----------------------------------------------------------------------------
/// Free the previously allocated mail box.
/// @param Sbx  Mail box Id.
// =============================================================================
void sxr_FreeMailBox (u8 Sbx)
{
    u32 Evt;
    u32 Status = sxr_EnterSc ();

    while (sxr_SbxHot(Sbx))
    {
        sxr_Wait(&Evt, Sbx);
    }

    sxr_Sbx.Queue [Sbx].Id = sxr_Sbx.IdxFree;
    sxr_Sbx.IdxFree = Sbx;

    sxr_Sbx.Nb--;

    sxr_ExitSc (Status);
}

// =============================================================================
// sxr_NewSemaphore
// -----------------------------------------------------------------------------
/// Provide a free semaphore.
/// @param InitValue    Semaphore initialization value:
///                     - 0 Means semaphore is taken,
///                     - n Means semaphore is free for n users.
///                     .
/// @return             Semaphore Id.
// =============================================================================
u8 sxr_NewSemaphore (u8 InitValue)
{
    u8 Id = sxr_NewMailBox ();
    u32 i;

    for (i=0; i<InitValue; i++)
    {
        sxr_Send (NIL, Id, SXR_SEND_MSG | SXR_QUEUE_LAST);
    }

    for (i=0; i<SXR_NB_MAX_SEM; i++)
    {
        if (!sxr_SemaInfo[i].InUse)
        {
            sxr_SemaInfo[i].InUse = TRUE;
            sxr_SemaInfo[i].SemaId = Id;
            sxr_SemaInfo[i].CallerTask = SXR_NO_TASK;
            sxr_SemaInfo[i].CallerAddr = NIL;
            break;
        }
    }

    return Id;
}

// =============================================================================
// sxr_FreeSemaphore
// -----------------------------------------------------------------------------
/// Free a previously allocated semaphore.
/// @param Id   Semaphore Id.
// =============================================================================
void sxr_FreeSemaphore (u8 Id)
{
    while (sxr_SbxHot (Id))
    {
        sxr_Wait (NIL, Id);
    }

    for (u32 i=0; i<SXR_NB_MAX_SEM; i++)
    {
        if (sxr_SemaInfo[i].InUse && sxr_SemaInfo[i].SemaId == Id)
        {
            sxr_SemaInfo[i].InUse = FALSE;
            sxr_SemaInfo[i].SemaId = SXR_NO_ID;
            sxr_SemaInfo[i].CallerTask = SXR_NO_TASK;
            sxr_SemaInfo[i].CallerAddr = NIL;
            break;
        }
    }

    sxr_FreeMailBox (Id);
}


// =============================================================================
// sxr_SetRmtMbx
// -----------------------------------------------------------------------------
/// Set or unset a mail box in remote state.
/// @param Sbx  Sbx index.
/// @param Set  boolean Set.
// =============================================================================
void sxr_SetRmtMbx (u8 Sbx, u8 Set)
{
    if (Set)
    {
        sxr_Sbx.RmtSbx [Sbx >> 5] |= 1 << (Sbx & 31);
    }
    else
    {
        sxr_Sbx.RmtSbx [Sbx >> 5] &= ~(1 << (Sbx & 31));
    }
}

// =============================================================================
// sxr_IsRmtSbx
// -----------------------------------------------------------------------------
/// Get a mail box in remote state.
/// @param Sbx  Sbx index.
/// @return     remote state.
// =============================================================================
u8 sxr_IsRmtSbx (u8 Sbx)
{
    return (sxr_Sbx.RmtSbx [Sbx >> 5] & (1 << (Sbx & 31))) != 0;
}


// =============================================================================
// sxr_SetRxTxRmtMsg
// -----------------------------------------------------------------------------
/// Set the remote conversion function pointers.
/// @param TxMsg    Pointer on Tx conversion function.
/// @param RxMsg    Pointer on Rx conversion function.
// =============================================================================
void sxr_SetRxTxRmtMsg (void *(*TxMsg)(void *), void *(*RxMsg)(void *))
{
    sxr_Sbx.TxMsg = TxMsg;
    sxr_Sbx.RxMsg = RxMsg;
}


// =============================================================================
// sxr_SbxRxMsg
// -----------------------------------------------------------------------------
/// Call remote conversion function for received messages.
/// @param Msg  Pointer on received message.
/// @return     Pointer on converted message.
// =============================================================================
void *sxr_SbxRxMsg (void *Msg)
{
    if (sxr_Sbx.RxMsg != NIL)
    {
        return sxr_Sbx.RxMsg (Msg);
    }
    else
    {
        return Msg;
    }
}

// =============================================================================
// sxr_GetTaskWaitingForSbx
// -----------------------------------------------------------------------------
/// Return the Id of the task waiting on the Sbx.
/// @param Sbx  Sbx index.
/// @return     Id of the task.
// =============================================================================
u8 sxr_GetTaskWaitingForSbx (u8 Sbx)
{
    return sxr_Sbx.Queue [Sbx].Task.First;
}

// =============================================================================
// sxr_ChkSbx
// -----------------------------------------------------------------------------
/// Trace out a diagnostic information for sbx
/// @param Sbx  Sbx index.
// =============================================================================
void sxr_ChkSbx (u8 Sbx)
{
    {
        if (sxr_Sbx.Queue [Sbx].Id == SXR_SBX_ALLOCATED)
        {

            SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(1),TSTR("Sbx %i:\n",0x06bf0002), Sbx);

            if (sxr_Sbx.Queue [Sbx].Msg.First != SXR_NO_ENV)
            {
                u16 IdxCur = sxr_Sbx.Queue [Sbx].Msg.First;

                do
                {
                    SXS_TRACE (_SXR|TSTDOUT|TIDU|TDB|TNB_ARG(1),TSTR("0x%lx ->",0x06bf0003), sxr_Sbx.Env [IdxCur].Evt [0]);
                    IdxCur = sxr_Sbx.Env [IdxCur].Next;
                }
                while (IdxCur != SXR_NO_ENV);

                SXS_TRACE (_SXR|TSTDOUT|TIDU|TDB,TSTR(" -> NIL\n",0x06bf0004));
            }

            if (sxr_Sbx.Queue [Sbx].Task.First == SXR_NO_TASK)
            {
                SXS_TRACE (_SXR|TSTDOUT|TDB,TSTR("No waiting task.\n",0x06bf0005));
            }
            else
            {
                SXS_TRACE (_SXR|TSTDOUT|TSMAP(1)|TNB_ARG(1)|TDB,TSTR("Task %s is waiting.\n",0x06bf0006), sxr_Task.Ctx [sxr_Sbx.Queue [Sbx].Task.First].Desc -> Name);
            }
        }
    }
}

// =============================================================================
// sxr_CheckSbx
// -----------------------------------------------------------------------------
/// Trace out a diagnostic information for all sbx
// =============================================================================
void sxr_CheckSbx (void)
{
    u32 i;

    for (i=0; i<SXR_NB_TOTAL_SBX; i++)
    {
        sxr_ChkSbx (i);
    }
}

// =============================================================================
// sxr_TraceSbx
// -----------------------------------------------------------------------------
/// Set trace diagnostic for sbx action (very verbose)
/// @param Sbx      Sbx index.
/// @param Level    Verbosity level, currently 0: off, else on.
// =============================================================================
void sxr_TraceSbx (u8 Sbx, u8 Level)
{
    if (Sbx < SXR_NB_TOTAL_SBX)
    {
        sxr_Sbx.Queue [Sbx].TrcLev = Level;
    }
}


// =============================================================================
// sxr_GetSbxInfo
// -----------------------------------------------------------------------------
/// Get information about the identified synchronization box.
/// @param Sbx      Sbx index.
/// @param SbxInfo  Pointer to sxr_SbxInfo_t to fill.
// =============================================================================
void sxr_GetSbxInfo (u8 Sbx, sxr_SbxInfo_t *SbxInfo)
{
    SbxInfo -> NbAvailMsg    = 0;
    SbxInfo -> FirstMsg      = 0;
    SbxInfo -> NbWaitingTask = 0;

    if (sxr_Sbx.Queue [Sbx].Id == SXR_SBX_ALLOCATED)
    {
        if (sxr_Sbx.Queue [Sbx].Msg.First != SXR_NO_ENV)
        {
            u16 IdxCur = sxr_Sbx.Queue [Sbx].Msg.First;

            SbxInfo -> FirstMsg = sxr_Sbx.Env [IdxCur].Evt [0];

            do
            {
                SbxInfo -> NbAvailMsg++;
                IdxCur = sxr_Sbx.Env [IdxCur].Next;
            }
            while (IdxCur != SXR_NO_ENV);
        }

        if (sxr_Sbx.Queue [Sbx].Task.First != SXR_NO_TASK)
        {
            u8 NextTsk = sxr_Sbx.Queue [Sbx].Task.First;

            SbxInfo -> NbWaitingTask = 1;

            while ( sxr_Sbx.Queue [NextTsk].Task.Next != SXR_NO_TASK )
            {
                SbxInfo -> NbWaitingTask++;
                NextTsk = sxr_Sbx.Queue [NextTsk].Task.Next;
            }

            SbxInfo -> FirstTaskId = sxr_Sbx.Queue [Sbx].Task.First;
        }
    }
}

// =============================================================================
// sxs_FuncName
// -----------------------------------------------------------------------------
/// Scope
/// @param
/// @return
// =============================================================================

