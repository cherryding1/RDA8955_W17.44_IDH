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

#include "sxr_mutx.h"
#define __SXR_MUTX_VAR__
#include "sxp_mutx.h"
#undef __SXR_MUTX_VAR__

#include "sxs_io.h"

#include "sxr_sbx.h"
#include "sxr_sbx.hp"
#include "sxp_debug.h"
#include "sxr_tksd.hp"


// =============================================================================
// sxr_MutexInit
// -----------------------------------------------------------------------------
/// Mutual exclusion data context initialization.
// =============================================================================
void  sxr_MutexInit (void)
{
    u32 i;

    sxr_Mutex.Nb        = 0;
    sxr_Mutex.IdxFree   = 0;

    for (i=0; i<SXR_NB_MAX_MUTEX; i++)
    {
        sxr_Mutex.Queue [i].SemaId  = SXR_NO_ID;
        sxr_Mutex.Queue [i].TaskId  = SXR_NO_TASK;
        sxr_Mutex.Queue [i].Count   = 0;
        sxr_Mutex.Queue [i].Next    = i+1;
    }

    sxr_Mutex.Queue [SXR_NB_MAX_MUTEX-1].Next = SXR_NO_MUTEX;
}


// =============================================================================
// sxr_NewMutex
// -----------------------------------------------------------------------------
/// provide a free mutex.
/// @return mutex Id.
// =============================================================================
u8    sxr_NewMutex (void)
{
    u32 Status = sxr_EnterSc ();

    // queue management
    u8 Id = sxr_Mutex.IdxFree;

    if (Id == SXR_NO_MUTEX)
    {
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No more free mutex\n",0x06bf0011)));
    }

    sxr_Mutex.Nb++;
    sxr_Mutex.IdxFree = sxr_Mutex.Queue [Id].Next;

    sxr_ExitSc (Status);

    sxr_Mutex.Queue [Id].Next = SXR_MUTEX_ALLOCATED;

    // create the underlying semaphore
    sxr_Mutex.Queue [Id].SemaId = sxr_NewSemaphore (1);

    return Id;
}


// =============================================================================
// sxr_FreeMutex
// -----------------------------------------------------------------------------
/// Free a previously allocated mutex.
/// @param Id mutex Id.
// =============================================================================
void  sxr_FreeMutex (u8 Id)
{
    // ensure mutex is allocated
    SX_MUTX_ASSERT(sxr_Mutex.Queue [Id].Next == SXR_MUTEX_ALLOCATED, "sxr_FreeMutex mutex is not allocated!");
    // ensure mutex is released
    SX_MUTX_ASSERT(sxr_Mutex.Queue [Id].Count == 0, "sxr_FreeMutex while still taken!");

    // free the semaphore
    sxr_FreeSemaphore(sxr_Mutex.Queue [Id].SemaId);

    u32 Status = sxr_EnterSc ();

    // queue management
    sxr_Mutex.Queue [Id].Next = sxr_Mutex.IdxFree;
    sxr_Mutex.IdxFree = Id;

    sxr_Mutex.Nb--;

    sxr_ExitSc (Status);
}

// =============================================================================
// sxr_ChkMutex
// -----------------------------------------------------------------------------
/// Display debug information about mutexes
// =============================================================================
void  sxr_CheckMutex (void)
{
    u32 i;

    for (i=0; i<SXR_NB_MAX_MUTEX; i++)
        sxr_ChkMutex (i);

}

// =============================================================================
// sxr_ChkMutex
// -----------------------------------------------------------------------------
/// Display debug information about mutex
/// @param Id mutex Id.
// =============================================================================
void  sxr_ChkMutex (u8 Id)
{
    if (sxr_Mutex.Queue [Id].Next == SXR_MUTEX_ALLOCATED)
    {
        SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(1),TSTR("Mutex %i:\n",0x06bf0012),
                   Id);
        SXS_TRACE (_SXR|TSTDOUT|TSMAP(2)|TDB|TNB_ARG(3),TSTR("Owned by Task :%2i: %s Priority %3i\n",0x06bf0013),
                   sxr_Mutex.Queue [Id].TaskId,
                   sxr_Task.Ctx [sxr_Mutex.Queue [Id].TaskId].Desc -> Name,
                   sxr_Task.Ctx [sxr_Mutex.Queue [Id].TaskId].Desc -> Priority);
        SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(1),TSTR("Count:%d\n",0x06bf0014),
                   sxr_Mutex.Queue [Id].Count);
        SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(1),TSTR("First Take Caller: 0x%lx\n",0x06bf0015),
                   sxr_Mutex.Queue [Id].CallerAdd);
        SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(0),TSTR("Semaphore info:",0x06bf0016));

        sxr_ChkSbx(sxr_Mutex.Queue [Id].SemaId);
    }
}

