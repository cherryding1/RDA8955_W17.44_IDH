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
#include "sxs_io.h"
#include "sxr_queu.hp"
#include "sxr_ops.h"
#include "cs_types.h"


/// Number of envelop currently in use for the queues.
PRIVATE INT16 g_sxQueueEnvUsed = 0;

/// Queue overloading status.
PRIVATE BOOL g_sxQueueOverload = FALSE;


// =============================================================================
// sxr_QueueCheckOverload
// -----------------------------------------------------------------------------
/// Test the overloading of the queue envelope usage
///
/// @return \c TRUE if the queue mechanism is overloaded, and no new low priority
/// envelope should be attributed, \c FALSE otherwise.
// =============================================================================
PRIVATE BOOL sxr_QueueCheckOverload(VOID)
{
    // Release the overload condition test.
    if ((g_sxQueueOverload)
            && (g_sxQueueEnvUsed < (SXR_NB_QUEUE_ENVELOPPE - SXR_NB_FREE_QUEUE_ENVELOPPE_OVERLOAD_END)))
    {
        g_sxQueueOverload = FALSE;
    }

    // Overload condition test.
    if ((!g_sxQueueOverload)
            && (g_sxQueueEnvUsed > (SXR_NB_QUEUE_ENVELOPPE - SXR_NB_FREE_QUEUE_ENVELOPPE_OVERLOAD_START)))
    {
        g_sxQueueOverload = TRUE;
    }

    return g_sxQueueOverload;
}

/*
==============================================================================
   Function   : sxr_InitQueue
 ----------------------------------------------------------------------------
   Scope      : Queue initialization.
   Parameters : none
   Return     : none
==============================================================================
*/
void sxr_InitQueue (void)
{
    u32 i, j;

    for (i=0; i<SXR_NB_MAX_QUEUE; i++)
    {
        sxr_QueueDesc [i].First = (u16)i+1;
        sxr_QueueDesc [i].Last  = SXR_NO_ENV;
        sxr_QueueDesc [i].Load  = SXR_NO_ENV;

        for (j=0; j<SXR_NB_MAX_USR_QUEUE; j++)
        {
            sxr_QueueDesc [i].Cur [j] = SXR_NO_ENV;
            sxr_QueueDesc [i].Prev[j] = SXR_NO_ENV;
        }
    }

    sxr_QueueDesc [SXR_NB_MAX_QUEUE - 1].First = SXR_NO_QUEUE;

    for (i=0; i<SXR_NB_QUEUE_ENVELOPPE; i++)
    {
        sxr_QueueEnv [i].Next = (u16)i+1;
        sxr_QueueEnv [i].Idx  = (u16)i;
        sxr_QueueEnv [i].Data = NIL;
    }

    sxr_QueueEnv [SXR_NB_QUEUE_ENVELOPPE - 1].Next = SXR_NO_ENV;

    sxr_QueueFreeEnv = 0;
    sxr_QueueFreeDesc = 0;

    // Init queue overloading protection mechanism.
    g_sxQueueEnvUsed = 0;
    g_sxQueueOverload = FALSE;

}


/*
==============================================================================
   Function   : sxr_NewQueue
 ----------------------------------------------------------------------------
   Scope      : Allocate a new queue.
   Parameters : Pointer on queue name.
   Return     : none
==============================================================================
*/
#ifdef __SXR_QUEUE_DBG__
u8 _sxr_NewQueue (const ascii *Name, void (*Print)(void *))
#else
u8 __sxr_NewQueue (void)
#endif
{
    u32 Status = sxr_EnterSc ();

    u8 Queue = sxr_QueueFreeDesc;

#ifdef __SXR_QUEUE_DBG__
    if (Queue == SXR_NO_QUEUE)
    {
        sxr_CheckAllQueues ();
        SXS_RAISE ((_SXR|TABORT|TSMAP(1)|TNB_ARG(1)|TDB,TSTR("No queue available for %s\n",0x06a80001), Name));
    }
    sxr_QueueDesc [Queue].Name = Name;
    sxr_QueueDesc [Queue].Print = Print;
#else
    if (Queue == SXR_NO_QUEUE)
    {
        sxr_CheckAllQueues ();
        SXS_RAISE ((_SXR|TABORT|TNB_ARG(1)|TDB,TSTR("No queue available for %i\n",0x06a80002), Queue));
    }
#endif

    sxr_QueueFreeDesc = (u8)sxr_QueueDesc [Queue].First;

    sxr_ExitSc (Status);

    sxr_QueueDesc [Queue].First = SXR_NO_ENV;
    sxr_QueueDesc [Queue].Load  = 0;

    return Queue;
}


/*
==============================================================================
   Function   : sxr_FreeQueue
 ----------------------------------------------------------------------------
   Scope      : Free an allocated queue.
   Parameters : Queue number.
   Return     : none
==============================================================================
*/
void sxr_FreeQueue (u8 Queue)
{
    u32 j;

    if ((sxr_QueueDesc [Queue].First != SXR_NO_ENV)
            ||  (sxr_QueueDesc [Queue].Last  != SXR_NO_ENV)
            ||  (sxr_QueueDesc [Queue].Load  != 0))
    {
        sxr_CheckQueue (Queue);

#ifdef __SXR_QUEUE_DBG__
        SXS_RAISE ((_SXR|TABORT|TSMAP(1)|TNB_ARG(2)|TDB,TSTR("Free a non empty or inconsistent queue %s (%i)\n",0x06a80003), sxr_QueueDesc [Queue].Name, Queue));
#else
        SXS_RAISE ((_SXR|TABORT|TNB_ARG(1)|TDB,TSTR("Free a non empty or inconsistent queue (%i)\n",0x06a80004), Queue));
#endif
    }

    sxr_QueueDesc [Queue].Last  = SXR_NO_ENV;
    sxr_QueueDesc [Queue].Load  = SXR_NO_ENV;

#ifdef __SXR_QUEUE_DBG__
    sxr_QueueDesc [Queue].Name  = NIL;
#endif

    for (j=0; j<SXR_NB_MAX_USR_QUEUE; j++)
    {
        sxr_QueueDesc [Queue].Cur [j] = SXR_NO_ENV;
        sxr_QueueDesc [Queue].Prev[j] = SXR_NO_ENV;
    }

    u32 Status = sxr_EnterSc ();

    sxr_QueueDesc [Queue].First = sxr_QueueFreeDesc;
    sxr_QueueFreeDesc = Queue;

    sxr_ExitSc (Status);
}


// =============================================================================
// sxr_GetQueueEnv
// -----------------------------------------------------------------------------
/// Get a free queue enveloppe. If the usage is specified to be a low
/// priority one, the envelop is only got if, and only if, enough envelop
/// are available for the high priority usage.
///
/// This function MUST BE USED IN A CRITICAL SECTION.
///
/// @param  queueEnvPtr Pointer to a free envelop pointer
/// @param  highPrio    \c TRUE, if and only if, the usage is a high priority
/// one.
/// @return \c TRUE if the operation could be done,
///         \c FALSE if no envelop where available for a low priority use.
// =============================================================================
PRIVATE BOOL sxr_GetQueueEnv(sxr_QueueEnv_t** queueEnvPtr, BOOL highPrio)
{
    if (UNLIKELY(!highPrio))
    {
        // Low priority, check availability
        if (sxr_QueueCheckOverload())
        {
            // Not enough envelop, leave
            *queueEnvPtr = NULL;
            return FALSE;
        }
    }

    // Either High priority or enough envelop.
    u16 Env = sxr_QueueFreeEnv;

    if (Env == SXR_NO_ENV)
    {
        sxr_CheckAllQueues();
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No more queue enveloppes\n",0x06a80005)));
    }

    sxr_QueueFreeEnv = sxr_QueueEnv [Env].Next;

    *queueEnvPtr = &sxr_QueueEnv [Env];

    // One more envelop is used
    g_sxQueueEnvUsed++;

    return TRUE;
}

/*
==============================================================================
   Function   : sxr_FreeQueueEnv
 ----------------------------------------------------------------------------
   Scope      : Free an eveloppe.
   Parameters : Enveloppe index.
   Return     : none
==============================================================================
*/
void sxr_FreeQueueEnv (u16 Env)
{
    sxr_QueueEnv [Env].Next = sxr_QueueFreeEnv;
    sxr_QueueFreeEnv = Env;

    // An envelop is freed.
    g_sxQueueEnvUsed--;
}



// =============================================================================
// _sxr_QueueFirst
// -----------------------------------------------------------------------------
/// Insert an item first in queue. Current position (user 0) is set to first.
/// If the usage is specified to be a low priority one, the element is inserted
/// if, and only if, enough envelop are available for the high priority usage.
///
/// @param Data Pointer to the item to be inserted.
/// @param Queue Index.
/// @param highPrio \c TRUE, if and only if, the usage is a high priority
/// one.
/// @return \c TRUE if the operation could be done, and the data is inserted.
///         \c FALSE if no envelop where available for a low priority use, and
/// the data couldn't be inserted.
// =============================================================================
PUBLIC BOOL _sxr_QueueFirst (void *Data, u8 Queue, BOOL highPrio)
{
    BOOL available = FALSE;
    sxr_QueueEnv_t* QueueEnv = NULL;

    u32 Status = sxr_EnterSc ();

    // Get envelop, if available at this priority level.
    available = sxr_GetQueueEnv(&QueueEnv, highPrio);

    if (LIKELY(available))
    {
        QueueEnv -> Data = Data;
        QueueEnv -> Next = sxr_QueueDesc [Queue].First;
        sxr_QueueDesc [Queue].First = QueueEnv -> Idx;
        sxr_QueueDesc [Queue].Cur [0] = QueueEnv -> Idx;

        if (sxr_QueueDesc [Queue].Last == SXR_NO_ENV)
        {
            sxr_QueueDesc [Queue].Last = QueueEnv -> Idx;
        }

        sxr_QueueDesc [Queue].Prev [0] = SXR_NO_ENV;

        sxr_QueueDesc [Queue].Load++;
    }
    sxr_ExitSc (Status);

    // Return success or not of operation.
    return available;
}

// =============================================================================
// _sxr_QueueLast
// -----------------------------------------------------------------------------
/// Insert an item last in queue. Current position is set to last.
/// If the usage is specified to be a low priority one, the element is inserted
/// if, and only if, enough envelop are available for the high priority usage.
///
/// @param Data Pointer to the item to be inserted.
/// @param Queue Index.
/// @param highPrio \c TRUE, if and only if, the usage is a high priority
/// one.
/// @return \c TRUE if the operation could be done, and the data is inserted.
///         \c FALSE if no envelop where available for a low priority use, and
/// the data couldn't be inserted.
// =============================================================================
PUBLIC BOOL _sxr_QueueLast (void *Data, u8 Queue, BOOL highPrio)
{
    BOOL available = FALSE;
    u16 Cur;

    u32 Status = sxr_EnterSc ();

    Cur = sxr_QueueDesc [Queue].Cur [0];
    sxr_QueueDesc [Queue].Cur [0] = sxr_QueueDesc [Queue].Last;

    available = _sxr_QueueNext(Data, Queue, highPrio);

    // Restore the current position in case of failure
    if (!available)
    {
        sxr_QueueDesc [Queue].Cur [0] = Cur;
    }

    sxr_ExitSc (Status);

    // Return success or not of operation.
    return available;
}

// =============================================================================
// _sxr_QueueNext
// -----------------------------------------------------------------------------
/// Insert item after current position.
/// Current position is set to inserted item.
/// If the usage is specified to be a low priority one, the element is inserted
/// if, and only if, enough envelop are available for the high priority usage.
///
/// @param Data Pointer to the item to be inserted.
/// @param Queue Index.
/// @param highPrio \c TRUE, if and only if, the usage is a high priority
/// one.
/// @return \c TRUE if the operation could be done, and the data is inserted.
///         \c FALSE if no envelop where available for a low priority use, and
/// the data couldn't be inserted.
// =============================================================================
PUBLIC BOOL _sxr_QueueNext (void *Data, u8 Queue, BOOL highPrio)
{
    u32 Status;
    sxr_QueueEnv_t *QueueEnv = NULL;
    BOOL available = FALSE;
    u16 Cur;

    Status = sxr_EnterSc ();

    available = sxr_GetQueueEnv (&QueueEnv, highPrio);

    if (LIKELY(available))
    {
        QueueEnv -> Data = Data;
        Cur = sxr_QueueDesc [Queue].Cur [0];

        if (Cur == SXR_NO_ENV)
        {
            if (sxr_QueueDesc [Queue].First == SXR_NO_ENV)
            {
                QueueEnv -> Next = SXR_NO_ENV;
                sxr_QueueDesc [Queue].First = QueueEnv -> Idx;
            }
            else
            {
                Cur = sxr_QueueDesc [Queue].Last;

                if (Cur == SXR_NO_ENV)
                {
                    SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No current position for insertion.\n",0x06a80006)));
                }
            }
        }

        if (Cur != SXR_NO_ENV)
        {
            QueueEnv -> Next = sxr_QueueEnv [Cur].Next;
            sxr_QueueEnv [Cur].Next = QueueEnv -> Idx;
        }

        if (QueueEnv -> Next == SXR_NO_ENV)
        {
            sxr_QueueDesc [Queue].Last = QueueEnv -> Idx;
        }

        sxr_QueueDesc [Queue].Prev [0] = Cur;
        sxr_QueueDesc [Queue].Cur [0] = QueueEnv -> Idx;

        sxr_QueueDesc [Queue].Load++;
    }

    sxr_ExitSc (Status);

    // Return success or not of operation.
    return available;
}


// =============================================================================
// _sxr_QueueSorted
// -----------------------------------------------------------------------------
/// Insert item sorted according to Sort function criteria.
/// Current position is set to inserted item.
/// If the usage is specified to be a low priority one, the element is inserted
/// if, and only if, enough envelop are available for the high priority usage.
///
/// @param Data Pointer to the item to be inserted.
/// @param Sort Sort Function: this function takes as parameter 2 pointers
/// on 2 differents items. Returns \c TRUE when items order must
/// be inverted, \c FALSE otherwise.
/// @param Queue Index.
/// @param highPrio \c TRUE, if and only if, the usage is a high priority
/// one.
/// @return \c TRUE if the operation could be done, and the data is inserted.
///         \c FALSE if no envelop where available for a low priority use, and
/// the data couldn't be inserted.
// ==============================================================================
PUBLIC BOOL _sxr_QueueSorted (void *Data, u8 (*Sort)(void *Data1, void *Data2), u8 Queue, BOOL highPrio)
{
    u32 Status;
    sxr_QueueEnv_t *QueueEnv   = NULL;
    BOOL   available           = FALSE;
    u16 Prev = SXR_NO_ENV;

    Status = sxr_EnterSc ();

    u16 Env = sxr_QueueDesc [Queue].First;

    available = sxr_GetQueueEnv (&QueueEnv, highPrio);

    if (LIKELY(available))
    {
        QueueEnv -> Data = Data;

        while ((Env != SXR_NO_ENV)
                &&     (Sort (Data, sxr_QueueEnv [Env].Data)))
        {
            Prev = Env;
            Env = sxr_QueueEnv [Env].Next;
        }

        if (Prev == SXR_NO_ENV)
        {
            sxr_QueueDesc [Queue].First = QueueEnv -> Idx;
        }
        else
        {
            sxr_QueueEnv [Prev].Next = QueueEnv -> Idx;
        }

        QueueEnv -> Next = Env;
        sxr_QueueDesc [Queue].Cur [0] = QueueEnv -> Idx;
        sxr_QueueDesc [Queue].Prev [0] = Prev;

        if (QueueEnv -> Next == SXR_NO_ENV)
        {
            sxr_QueueDesc [Queue].Last = QueueEnv -> Idx;
        }

        sxr_QueueDesc [Queue].Load++;
    }

    sxr_ExitSc (Status);
    // Return success or not of operation.
    return available;
}

/*
==============================================================================
   Function   : sxr_QueueGetFirst
 ----------------------------------------------------------------------------
   Scope      : Get first item in queue.
                Current position is set to first.
   Parameters : Queue index
                User index.
   Return     : Pointer on the found item.
==============================================================================
*/
void *sxr_QueueGetFirst (u8 User, u8 Queue)
{
    void *Data;
    u32 Status = sxr_EnterSc ();

    sxr_QueueDesc [Queue].Cur [User] = sxr_QueueDesc [Queue].First;
    sxr_QueueDesc [Queue].Prev [User] = SXR_NO_ENV;

    if (sxr_QueueDesc [Queue].First == SXR_NO_ENV)
        Data = NIL;
    else
        Data = sxr_QueueEnv [sxr_QueueDesc [Queue].First].Data;

    sxr_ExitSc (Status);

    return Data;
}

/*
==============================================================================
   Function   : sxr_QueueGetLast
 ----------------------------------------------------------------------------
   Scope      : Get the lat item in queue.
                Current position is set to last.
   Parameters : Queue index
                User idenx.
   Return     : pointer on the found item.
==============================================================================
*/
void *sxr_QueueGetLast (u8 User, u8 Queue)
{
    void *Data;
    u32 Status = sxr_EnterSc ();

    sxr_QueueDesc [Queue].Cur [User] = sxr_QueueDesc [Queue].Last;

    if (sxr_QueueDesc [Queue].First == sxr_QueueDesc [Queue].Last)
        sxr_QueueDesc [Queue].Prev [User] = SXR_NO_ENV;
    else
    {
        u16 Env = sxr_QueueDesc [Queue].First;

        while (sxr_QueueEnv [Env].Next != sxr_QueueDesc [Queue].Last)
            Env = sxr_QueueEnv [Env].Next;

        sxr_QueueDesc [Queue].Prev [User] = Env;
    }

    if (sxr_QueueDesc [Queue].Last == SXR_NO_ENV)
        Data = NIL;
    else
        Data = sxr_QueueEnv [sxr_QueueDesc [Queue].Last].Data;

    sxr_ExitSc (Status);
    return Data;
}

/*
==============================================================================
   Function   : sxr_QueueGetNext
 ----------------------------------------------------------------------------
   Scope      : Get the item after the current position.
                Current position is set to the found item.
   Parameters : Queue index
                Loop TRUE if first item must be retreived when last position
                is reached, FALSE otherwise.
                User index.
   Return     : Pointer on the found item.
==============================================================================
*/
void *sxr_QueueGetNext (u8 Loop, u8 User, u8 Queue)
{
    u32 Status;
    void *Data = NIL;
    u16 Cur;

    Status = sxr_EnterSc ();

    Cur = sxr_QueueDesc [Queue].Cur [User];

    sxr_QueueDesc [Queue].Prev [User] = Cur;

    if (Cur != SXR_NO_ENV)
    {
        Cur = sxr_QueueDesc [Queue].Cur [User] =
                  sxr_QueueEnv [Cur].Next;
    }

    if (Cur == SXR_NO_ENV && Loop)
    {
        Cur = sxr_QueueDesc [Queue].Cur [User] =
                  sxr_QueueDesc [Queue].First;
    }

    if (Cur != SXR_NO_ENV)
    {
        Data = sxr_QueueEnv [Cur].Data;
    }

    sxr_ExitSc (Status);

    return Data;
}

/*
==============================================================================
   Function   : sxr_QueueGetCur
 ----------------------------------------------------------------------------
   Scope      : Get the item at the current position.
                Current position is set to the next item.
   Parameters : Queue index
                User index.
   Return     : Pointer on the found item.
==============================================================================
*/
void *sxr_QueueGetCur (u8 User, u8 Queue)
{
    u32 Status;
    void *Data = NIL;
    u16 Cur;

    Status = sxr_EnterSc ();

    Cur = sxr_QueueDesc [Queue].Cur [User];

    if (Cur != SXR_NO_ENV)
    {
        Data = sxr_QueueEnv [Cur].Data;

        sxr_QueueDesc [Queue].Prev [User] = Cur;
        sxr_QueueDesc [Queue].Cur [User] = sxr_QueueEnv [Cur].Next;
    }

    sxr_ExitSc (Status);

    return Data;
}

/*
==============================================================================
   Function   : sxr_QueueGet
 ----------------------------------------------------------------------------
   Scope      : Get item identified item in queue. Search starts at current
                position.
                Current position is left to item following found item or
                unchanged when no item is found.
   Parameters : UserData pointer. This pointer is provided to the Get function
                as parameter and may be used for the identification of the item.
                Get function: takes as parameter the provided UserData pointer
                and the current item. Return TRUE when the item corresponds
                to the one searched and FALSE otherwise.
                Queue index
                User index.
   Return     : Pointer on the found item.
==============================================================================
*/
void *sxr_QueueGet (void *UserData, u8 (*Get)(void *UserData, void *Data), u8 User, u8 Queue)
{
    void *Data;
    u32 Status = sxr_EnterSc ();
// u16 Prev = SXR_NO_ENV;
    u16 Env  = sxr_QueueDesc [Queue].Cur [User];

    while (Env != SXR_NO_ENV)
    {
        if (Get (UserData, sxr_QueueEnv [Env].Data))
        {
            sxr_QueueDesc [Queue].Cur [User] = sxr_QueueEnv [Env].Next;
            sxr_QueueDesc [Queue].Prev [User] = Env;
            Data = sxr_QueueEnv [Env].Data;
            sxr_ExitSc (Status);
            return Data;
        }
        else
        {
//   Prev = Env;
            Env = sxr_QueueEnv [Env].Next;
        }
    }

    sxr_ExitSc (Status);
    return NIL;
}

/*
==============================================================================
   Function   : sxr_QueueRmvFirst
 ----------------------------------------------------------------------------
   Scope      : Remove first item in queue.
                Current position is set to first after removal.
   Parameters : Queue index.
   Return     : Pointer on the found item.
==============================================================================
*/
void *sxr_QueueRmvFirst (u8 Queue)
{
    u16 Env;
    void *Data = NIL;
    u32 Status;

    Status = sxr_EnterSc ();

    if (sxr_QueueDesc [Queue].First != SXR_NO_ENV)
    {
        Env = sxr_QueueDesc [Queue].First;
        Data = sxr_QueueEnv [Env].Data;

        sxr_QueueDesc [Queue].First =
            sxr_QueueDesc [Queue].Cur [0] = sxr_QueueEnv [Env].Next;
        sxr_QueueDesc [Queue].Prev [0] = SXR_NO_ENV;

        if (sxr_QueueDesc [Queue].First == SXR_NO_ENV)
            sxr_QueueDesc [Queue].Last = SXR_NO_ENV;

        sxr_QueueDesc [Queue].Load--;

        sxr_FreeQueueEnv (Env);
    }

    sxr_ExitSc (Status);
    return Data;
}

/*
==============================================================================
   Function   : sxr_QueueRmvLast
 ----------------------------------------------------------------------------
   Scope      : Remove last item in queue.
                Current position is set to last after removal.
   Parameters : Queue index.
   Return     : Pointer on the found item.
==============================================================================
*/
void *sxr_QueueRmvLast (u8 Queue)
{
    u16 Env, Last;
    void *Data = NIL;
    u32 Status;

    Status = sxr_EnterSc ();

    if (sxr_QueueDesc [Queue].Last != SXR_NO_ENV)
    {
        if (sxr_QueueDesc [Queue].Cur [0] == SXR_NO_ENV)
            SXS_RAISE ((_SXR|TDB,TSTR("Broken queue\n.",0x06a80008)));

        Env = sxr_QueueDesc [Queue].Last;
        Data = sxr_QueueEnv [Env].Data;

        if (sxr_QueueDesc [Queue].First == sxr_QueueDesc [Queue].Last)
        {
            sxr_QueueDesc [Queue].First = SXR_NO_ENV;
            sxr_QueueDesc [Queue].Last = SXR_NO_ENV;
            sxr_QueueDesc [Queue].Prev [0] = SXR_NO_ENV;
            sxr_QueueDesc [Queue].Cur [0] = SXR_NO_ENV;
        }
        else
        {
            u16 Prev = SXR_NO_ENV;
            Last = sxr_QueueDesc [Queue].First;

            while (sxr_QueueEnv [Last].Next != Env)
            {
                Prev = Last;
                Last = sxr_QueueEnv [Last].Next;
            }

            sxr_QueueDesc [Queue].Prev [0] = Prev;
            sxr_QueueDesc [Queue].Last =
                sxr_QueueDesc [Queue].Cur [0] = Last;

            sxr_QueueEnv [Last].Next = SXR_NO_ENV;
        }

        sxr_FreeQueueEnv (Env);
        sxr_QueueDesc [Queue].Load--;
    }

    sxr_ExitSc (Status);
    return Data;
}

/*
==============================================================================
   Function   : sxr_QueueRmvCur
 ----------------------------------------------------------------------------
   Scope      : Remove item at current position in queue.
                Current position (user 0) is set to next.
   Parameters : Queue index.
   Return     : Pointer on the found item.
==============================================================================
*/
void *sxr_QueueRmvCur (u8 Queue)
{
    u16 Env;
    void *Data = NIL;
    u32 Status;

    Status = sxr_EnterSc ();

    if (sxr_QueueDesc [Queue].First != SXR_NO_ENV)
    {
        if (sxr_QueueDesc [Queue].Cur [0] == SXR_NO_ENV)
            SXS_RAISE ((_SXR|TDB,TSTR("Broken queue\n.",0x06a80009)));

        Env = sxr_QueueDesc [Queue].Cur [0];
        Data = sxr_QueueEnv [Env].Data;

        sxr_QueueDesc [Queue].Cur [0] = sxr_QueueEnv [Env].Next;

        if (Env == sxr_QueueDesc [Queue].First)
            sxr_QueueDesc [Queue].First = sxr_QueueEnv [Env].Next;
        else
            sxr_QueueEnv [sxr_QueueDesc [Queue].Prev [0]].Next = sxr_QueueEnv [Env].Next;

        if (Env == sxr_QueueDesc [Queue].Last)
            sxr_QueueDesc [Queue].Last = sxr_QueueDesc [Queue].Prev [0];

        sxr_FreeQueueEnv (Env);
        sxr_QueueDesc [Queue].Load--;
    }

    sxr_ExitSc (Status);

    return Data;
}

/*
==============================================================================
   Function   : sxr_QueueRmv
 ----------------------------------------------------------------------------
   Scope      : Remove identified item.
                Current position (user 0) is set to the position following the
                removed item.
   Parameters : Queue index.
                Pointer on item.
   Return     : pointer on the found item.
==============================================================================
*/
void *sxr_QueueRmv (void *Data, u8 Queue)
{
    u16 Prev = SXR_NO_ENV;
    void *RData;

    u32 Status = sxr_EnterSc ();
    u16 Env  = sxr_QueueDesc [Queue].First;

    while ((Env != SXR_NO_ENV)
            &&     (sxr_QueueEnv [Env].Data != Data))
    {
        Prev = Env;
        Env = sxr_QueueEnv [Env].Next;
    }

    if ((Env != SXR_NO_ENV)
            &&  (sxr_QueueEnv [Env].Data == Data))
    {
        sxr_QueueDesc [Queue].Cur [0] = Env;
        sxr_QueueDesc [Queue].Prev [0] = Prev;
        RData = sxr_QueueRmvCur (Queue);
    }
    else
        RData = NIL;

    sxr_ExitSc (Status);

    return RData;
}

/*
==============================================================================
   Function   : sxr_QueuePurge
 ----------------------------------------------------------------------------
   Scope      : Remove one or all the items in list identified by purge function.
                Search occurs on all the items of the queue.
                Current position (user 0) is set to the position following the
                removed item.
                This function is not protected against concurrent access in
                queue
   Parameters : UserData pointer. This pointer is provided to the Purge function
                as parameter and may be used for the identification of the items
                to be deleted.
                Purge function: takes as parameter the provided UserData pointer
                and the current item eligible for deletion. Return TRUE when
                the item must be removed, FALSE otherwise.
                All: when TRUE all the items identified by the purge function
                are removed, otherwise, purge stop as soon as one item has been
                removed.
   Return     : Number of items removed.
==============================================================================
*/
u16 sxr_QueuePurge (void *UserData, u8 (*Purge)(void *UserData, void *QueueData), bool All, u8 Queue)
{
    u16 Prev = SXR_NO_ENV;
    u16 Env  = sxr_QueueDesc [Queue].First;
    u16 NbPurge = 0;

    while (Env != SXR_NO_ENV)
    {
        if (Purge (UserData, sxr_QueueEnv [Env].Data))
        {
            sxr_QueueDesc [Queue].Cur [0] = Env;
            sxr_QueueDesc [Queue].Prev [0] = Prev;
            sxr_QueueRmvCur (Queue);
            Env = sxr_QueueDesc [Queue].Cur [0];
            Prev = sxr_QueueDesc [Queue].Prev [0];
            NbPurge++;

            if (!All)
                return NbPurge;
        }
        else
        {
            Prev = Env;
            Env = sxr_QueueEnv [Env].Next;
        }
    }

    return NbPurge;
}

/*
==============================================================================
   Function   : sxr_QueueSort
 ----------------------------------------------------------------------------
   Scope      : Sort items in queue according to sort function direction.
                Current position (user 0) is set to first.
                This function is not protected against concurrent access in
                queue
   Parameters : Queue index.
                Sort Function: this function takes as parameter 2 pointers
                on 2 differents items. Returns TRUE when items order must
                be inverted, FALSE otherwise.
   Return     : none
==============================================================================
*/
void sxr_QueueSort (u8 (*Sort)(void *Data1, void *Data2), u8 Queue)
{
    u16 Env1 = sxr_QueueDesc [Queue].First;
    u16 Env2 = sxr_QueueDesc [Queue].First;

    while (Env1 != SXR_NO_ENV)
    {
        Env2 = sxr_QueueEnv [Env1].Next;

        while (Env2 != SXR_NO_ENV)
        {
            if (Sort (sxr_QueueEnv [Env1].Data, sxr_QueueEnv [Env2].Data))
                SWAPT (sxr_QueueEnv [Env1].Data, sxr_QueueEnv [Env2].Data, void *);

            Env2 = sxr_QueueEnv [Env2].Next;
        }

        Env1 = sxr_QueueEnv [Env1].Next;
    }
    sxr_QueueDesc [Queue].Cur [0] = sxr_QueueDesc [Queue].First;
    sxr_QueueDesc [Queue].Prev [0] = SXR_NO_ENV;
}


/*
==============================================================================
   Function   : sxr_QueueGetCurPos
 ----------------------------------------------------------------------------
   Scope      : Retreive the range of the current position in queue.
   Parameters : Queue index.
                User index.
   Return     : Current position range.
==============================================================================
*/
u16 sxr_QueueGetCurPos (u8 User, u8 Queue)
{
    u16 Env = sxr_QueueDesc [Queue].First;
    u16 Pos = 0;

    while ((Env != SXR_NO_ENV)
            &&     (Env != sxr_QueueDesc [Queue].Cur [User]))
    {
        Pos++;
        Env = sxr_QueueEnv [Env].Next;
    }

    return Pos;
}

/*
==============================================================================
   Function   : sxr_QueueSetCurPos
 ----------------------------------------------------------------------------
   Scope      : Set the current position at the required range in queue.
   Parameters : Queue index.
                Required position.
                User index.
   Return     : none
==============================================================================
*/
void sxr_QueueSetCurPos (u16 Pos, u8 User, u8 Queue)
{
    u16 Prev = SXR_NO_ENV;
    u16 Cnt = 0;

    u32 Status = sxr_EnterSc ();
    u16 Env = sxr_QueueDesc [Queue].First;

    while ((Env != SXR_NO_ENV)
            &&     (Cnt++ < Pos))
    {
        Prev = Env;
        Env = sxr_QueueEnv [Env].Next;
    }

    sxr_QueueDesc [Queue].Cur [User] = Env;
    sxr_QueueDesc [Queue].Prev [User] = Prev;

    sxr_ExitSc (Status);
}

/*
==============================================================================
   Function   : sxr_QueueGetLoad
 ----------------------------------------------------------------------------
   Scope      : Get the number of items linked in the queue.
   Parameters : Queue index
   Return     : Number of items currently in queue.
==============================================================================
*/
u16 sxr_QueueGetLoad (u8 Queue)
{
    return sxr_QueueDesc [Queue].Load;
}

/*
==============================================================================
   Function   : sxr_QueueGetName
 ----------------------------------------------------------------------------
   Scope      : Get the name of the queue.
   Parameters : Queue index.
   Return     : Pointer to name of the queue.
==============================================================================
*/
const ascii *sxr_QueueGetName (u8 Queue)
{
#ifdef __SXR_QUEUE_DBG__
    return sxr_QueueDesc [Queue].Name;
#else
    return NIL;
#endif
}

/*
==============================================================================
   Function   : sxr_CheckQueue
 ----------------------------------------------------------------------------
   Scope      : Print queue content.
   Parameters : Queue index.
   Return     : none
==============================================================================
*/
void sxr_CheckQueue (u8 Queue)
{
    u16 Env = sxr_QueueDesc [Queue].First;

    if (sxr_QueueDesc [Queue].Load == SXR_NO_ENV)
        return;

#ifdef __SXR_QUEUE_DBG__
    SXS_TRACE (_SXR|TSTDOUT|TSMAP(1)|TNB_ARG(1)|TDB,TSTR("Queue %s\n",0x06a8000a), sxr_QueueDesc [Queue].Name);
#else
    SXS_TRACE (_SXR|TSTDOUT|TNB_ARG(1)|TDB,TSTR("Queue %i\n",0x06a8000b), Queue);
#endif

    SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(5),TSTR("%i items First 0x%x Last 0x%x Cur 0x%x Prev 0x%x\n",0x06a8000c),
               sxr_QueueDesc [Queue].Load,
               sxr_QueueDesc [Queue].First != SXR_NO_ENV ? (u32)sxr_QueueEnv [sxr_QueueDesc [Queue].First].Data : sxr_QueueDesc [Queue].First,
               sxr_QueueDesc [Queue].Last != SXR_NO_ENV ? (u32)sxr_QueueEnv [sxr_QueueDesc [Queue].Last].Data : sxr_QueueDesc [Queue].Last,
               sxr_QueueDesc [Queue].Cur [0] != SXR_NO_ENV ? (u32)sxr_QueueEnv [sxr_QueueDesc [Queue].Cur [0]].Data : sxr_QueueDesc [Queue].Cur [0],
               sxr_QueueDesc [Queue].Prev [0] != SXR_NO_ENV ? (u32)sxr_QueueEnv [sxr_QueueDesc [Queue].Prev [0]].Data : sxr_QueueDesc [Queue].Prev [0]);

    while (Env != SXR_NO_ENV)
    {
#ifdef __SXR_QUEUE_DBG__
        if (sxr_QueueDesc [Queue].Print != NIL)
            sxr_QueueDesc [Queue].Print (sxr_QueueEnv [Env].Data);
        else
            SXS_TRACE (_SXR|TSTDOUT|TIDU|TDB|TNB_ARG(1),TSTR("%lx -> ",0x06a8000d), sxr_QueueEnv [Env].Data);
#else
        SXS_TRACE (_SXR|TSTDOUT|TIDU|TDB|TNB_ARG(1),TSTR("%lx -> ",0x06a8000e), sxr_QueueEnv [Env].Data);
#endif
        Env = sxr_QueueEnv [Env].Next;
    }
    SXS_TRACE (_SXR|TSTDOUT|TIDU|TDB,TSTR("NIL.\n",0x06a8000f));
}

/*
==============================================================================
   Function   : sxr_CheckAllQueues
 ----------------------------------------------------------------------------
   Scope      : Print all queues contents.
   Parameters : none
   Return     : none
==============================================================================
*/
void sxr_CheckAllQueues (void)
{
    u32 i;

    for (i=0; i<SXR_NB_MAX_QUEUE; i++)
        sxr_CheckQueue ((u8)i);
}


