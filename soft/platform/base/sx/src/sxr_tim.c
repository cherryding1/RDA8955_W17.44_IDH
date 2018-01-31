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

#include "sxr_tim.hp"
#include "sxr_ops.h"
#include "sxs_io.h"
#include "sxs_lib.h"
#include "sxr_cpu.h"

// Framed timer check might be violated when L1 jobs enter sync domain while leaving async
// domain (task context) -- Some jobs might be started or reset via L1 events (not always via messages).
// So the framed timer strict check is disabled here.

//#define SXR_FRAMED_TIMER_STRICT_CHECK

#define SXR_TIMER_MBX_POS       0
#define SXR_TIMER_MBX_MSK       ((u16)0x3FFFL << SXR_TIMER_MBX_POS)

#define SXR_GEN_MBX_TIMER_ID(Mbx)                          \
        (u16)( SXR_MBX_TIMER                               \
        | ((Mbx << SXR_TIMER_MBX_POS) & SXR_TIMER_MBX_MSK))

#define SXR_GET_TIMER_MBX(Ctx) (u8)((Ctx & SXR_TIMER_MBX_MSK) >> SXR_TIMER_MBX_POS)

#define SXR_GEN_FUNCTION_TIMER_CTX(Ctx, Function, Param)  \
        Ctx [SXR_TIMER_PARAM_IDX]    = (u32)Param;        \
        Ctx [SXR_TIMER_FUNCTION_IDX] = (u32)Function;

/*
==============================================================================
   Function   : sxs_StartTimer
 -----------------------------------------------------------------------------
   Scope      : Start a timer. At timer due date, an event built with parameters
                Id and Data is send in the required mail box.
   Parameters : - Timer Period,
                - Id, user data,
                - Boolean restart is set to TRUE when timer must be stopped first.
                - mail box number.
   Return     : none
==============================================================================
*/
void sxs_StartTimer (u32 Period, u32 Id, void *Data, u8 ReStart, u8 Mbx)
{
    if ((Mbx & (SXR_TIMER_MBX_MSK >> SXR_TIMER_MBX_POS)) != Mbx)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1), TSTR("Mail box number overflow: %d\n",0x06aa0006), Mbx));
    }

    if (ReStart)
    {
        sxs_StopTimer (Id, Data, Mbx);
    }

    u32 Ctx [SXR_TIMER_CTX_SIZE] = { 0, };
    Ctx [0] = Id;
    Ctx [1] = (u32)Data;
    sxr_SetTimer (Period, Ctx, (u16)SXR_GEN_MBX_TIMER_ID (Mbx), SXR_REGULAR_TIMER);
}

/*
==============================================================================
   Function   : sxs_StopTimer
 -----------------------------------------------------------------------------
   Scope      : Stop the identified timer.
   Parameters : Id, user Data, mail box number.
   Return     : TRUE when timer found, FALSE otherwise.
==============================================================================
*/
u8 sxs_StopTimer (u32 Id, void *Data, u8 Mbx)
{
    if ((Mbx & (SXR_TIMER_MBX_MSK >> SXR_TIMER_MBX_POS)) != Mbx)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1), TSTR("Mail box number overflow: %d\n",0x06aa0006), Mbx));
    }

    sxr_Timer_t Timer = { 0, };
    Timer.Idx = 0;
    Timer.Ctx = Id;
    Timer.Msk = 0xFFFFFFFF;

    if (!sxr_DeleteTimer (&Timer, NIL, FALSE, SXR_REGULAR_TIMER))
    {
        u32 Ctx [SXR_TIMER_CTX_SIZE] = { 0, };
        Ctx [0] = Id;
        Ctx [1] = (u32)Data;
        return sxr_RmvMsgFromMbx (Ctx, TRUE, Mbx, SXR_TIMER_CTX_SIZE);
    }
    else
    {
        return TRUE;
    }
}

/*
==============================================================================
   Function   : sxs_StopAllTimer
 -----------------------------------------------------------------------------
   Scope      : Stop all instances of identified timer.
   Parameters : Id, user Data, mail box number.
   Return     : TRUE when timer found, FALSE otherwise.
==============================================================================
*/
u8 sxs_StopAllTimer (u32 Id, void *Data, u8 Mbx)
{
    if ((Mbx & (SXR_TIMER_MBX_MSK >> SXR_TIMER_MBX_POS)) != Mbx)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1), TSTR("Mail box number overflow: %d\n",0x06aa0006), Mbx));
    }

    sxr_Timer_t Timer = { 0, };
    Timer.Idx = 0;
    Timer.Ctx = Id;
    Timer.Msk = 0xFFFFFFFF;

    if (!sxr_DeleteTimer (&Timer, NIL, TRUE, SXR_REGULAR_TIMER))
    {
        u32 Ctx [SXR_TIMER_CTX_SIZE];
        Ctx [0] = Id;
        Ctx [1] = (u32)Data;
        return sxr_RmvMsgFromMbx (Ctx, TRUE, Mbx,SXR_TIMER_CTX_SIZE);
    }
    else
    {
        return TRUE;
    }
}

/*
==============================================================================
   Function   : sxs_ExistTimer
 -----------------------------------------------------------------------------
   Scope      : Check if the timer identified thanks to its Id exists and if
                so returns its period or SXR_NO_DUE_DATE when no timer found.
   Parameters : Timer Id.
   Return     : Period or SXR_NO_DUE_DATE.
==============================================================================
*/
u32 sxs_ExistTimer (u32 Id)
{
    sxr_Timer_t Timer = { 0, 0xFFFFFFFF, 0, 0 };
    Timer.Ctx = Id;

    return sxr_TimerQuery (&Timer, SXR_REGULAR_TIMER);
}

/*
==============================================================================
   Function   : _sxr_StartFunctionTimer
 -----------------------------------------------------------------------------
   Scope      : Start a timer such that after required period, the function
                given in parameter is called with the parameter given.
   Parameters : Period to wait,
                Function to be called
                Parameter to be given to the function called.
                Timer Id,
                Timer instance.
   return     : none.
==============================================================================
*/
void _sxr_StartFunctionTimer (u32 Period, void (*Function)(void *), void *Param, u16 Id, u8 I2d)
{
    if (Id & SXR_MBX_TIMER)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Bit %x reserved.\n",0x06aa0001), SXR_MBX_TIMER));
    }

    u32 Ctx [SXR_TIMER_CTX_SIZE] = { 0, };
    SXR_GEN_FUNCTION_TIMER_CTX (Ctx, Function, Param);

    sxr_SetTimer (Period, Ctx, (u16)(Id | SXR_FUNCTION_TIMER), I2d);
}

/*
==============================================================================
   Function   : _sxr_StopFunctionTimer
 -----------------------------------------------------------------------------
   Scope      : stop a function timer
   Parameters : Function to be stoped
                Timer instance.
   return     : none.
==============================================================================
*/
u32 _sxr_StopFunctionTimer (void (*Function)(void *), u8 I2d)
{
    sxr_Timer_t Timer = { 0, };
    Timer.Idx = SXR_TIMER_FUNCTION_IDX;
    Timer.Ctx = (u32)Function;
    Timer.Msk = 0xFFFFFFFF;
    return sxr_DeleteTimer (&Timer, NULL, TRUE, I2d);
}

/*
==============================================================================
   Function   : _sxr_StopFunctionTimer2
 -----------------------------------------------------------------------------
   Scope      : stop a function timer which related to param
   Parameters : Function to be stoped
                Parameter is related to function
                Timer instance.
   return     : none.
==============================================================================
*/
u32 _sxr_StopFunctionTimer2 (void (*Function)(void *),void *Param, u8 I2d)
{
    sxr_TimerEx_t Timer = { 0, };
    Timer.Id = SXR_FUNCTION_TIMER;
    Timer.IdMsk = SXR_FUNCTION_TIMER;
    Timer.Ctx[SXR_TIMER_FUNCTION_IDX] = (u32)Function;
    Timer.Msk[SXR_TIMER_FUNCTION_IDX] = 0xFFFFFFFF;
    Timer.Ctx[SXR_TIMER_PARAM_IDX] = (u32)Param;
    Timer.Msk[SXR_TIMER_PARAM_IDX] = 0xFFFFFFFF;
    return sxr_DeleteTimerEx (&Timer, NULL, TRUE, NULL, I2d);
}

// =============================================================================
// sxr_StartMbxTimer
// -----------------------------------------------------------------------------
/// Start a timer such that after required period, the event
/// given in parameter is sent in the required mail box.
///
/// @param period   Period to wait,
/// @param evt      Event to be sent.
/// @param mbx      Mail box number
/// @param i2d      Timer instance. (SXR_REGULAR_TIMER or SXR_FRAMED_TIMER)
// =============================================================================
void sxr_StartMbxTimer (u32 period, u32 *evt, u8 mbx, u8 i2d)
{
    if ((mbx & (SXR_TIMER_MBX_MSK >> SXR_TIMER_MBX_POS)) != mbx)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1), TSTR("Mail box number overflow: %d\n",0x06aa0006), mbx));
    }

    sxr_SetTimer (period, evt, (u16)SXR_GEN_MBX_TIMER_ID(mbx), i2d);
}


// =============================================================================
// sxr_StopMbxTimer
// -----------------------------------------------------------------------------
/// Stop the identified timer, identified by the mailbox id and the timer id.
///
/// @param evt      Event to be sent.
/// @param mbx      Mail box number
/// @param i2d      Timer instance. (SXR_REGULAR_TIMER or SXR_FRAMED_TIMER)
/// @return         \c TRUE when timer found, \c FALSE otherwise.
// =============================================================================
u8 sxr_StopMbxTimer (u32 *evt, u8 mbx, u8 I2d)
{
    if ((mbx & (SXR_TIMER_MBX_MSK >> SXR_TIMER_MBX_POS)) != mbx)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1), TSTR("Mail box number overflow: %d\n",0x06aa0006), mbx));
    }

    sxr_TimerEx_t Timer = { 0, };
    Timer.Id = SXR_GEN_MBX_TIMER_ID(mbx);
    Timer.IdMsk = 0xFFFF;
    for (int i = 0; i < SXR_TIMER_CTX_SIZE; i++)
    {
        Timer.Ctx[i] = evt[i];
        Timer.Msk[i] = 0xFFFFFFFF;
    }

    if (!sxr_DeleteTimerEx (&Timer, NULL, TRUE, NULL, I2d))
    {
        return sxr_RmvMsgFromMbx (evt, TRUE, mbx, SXR_TIMER_CTX_SIZE);
    }
    else
    {
        return TRUE;
    }
}

/*
==============================================================================
   Function   : sxr_SetTimer
 -----------------------------------------------------------------------------
   Scope      : Set a timer.
   Parameters : relative Period, Ctx[SXR_TIMER_CTX_SIZE], Id and instance number.
   Return     : none
==============================================================================
*/
void sxr_SetTimer (u32 PeriodParam, u32 *Ctx, u16 Id, u8 I2d)
{
    u16 IdxFree;
    s32 Period = (s32)PeriodParam;
    if (Period <= 0)
    {
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("Period must be > 0\n",0x06aa0003)));
    }

    u32 Status = sxr_EnterSc();

    if (sxr_TEnv.IdxFree == SXR_TIM_NIL_IDX)
    {
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No more timer enveloppe\n",0x06aa0004), sxr_CheckTimer ()));
    }

    u16 IdxNew = sxr_TEnv.IdxFree;
    u16 IdxFirst = sxr_TCtx [I2d].IdxFirst;
    IdxFree = sxr_TEnv.TimerEnv [IdxNew].IdxNext;

    if (IdxFirst != SXR_TIM_NIL_IDX)
    {
        u16 IdxCur  = IdxFirst;
        u16 IdxPrev = SXR_TIM_NIL_IDX;
        u32 Elapsed = 0;
        u32 TotalPeriod;

        if (I2d == SXR_REGULAR_TIMER)
            Elapsed = hal_TimGetElapsedTime();

        Period += Elapsed;
        TotalPeriod = sxr_TEnv.TimerEnv [IdxFirst].PeriodToNext;

        while ((sxr_TEnv.TimerEnv [IdxCur].IdxNext != SXR_TIM_NIL_IDX)
                &&     (TotalPeriod < Period))
        {
            IdxPrev = IdxCur;
            IdxCur  = sxr_TEnv.TimerEnv [IdxCur].IdxNext;

            TotalPeriod += sxr_TEnv.TimerEnv [IdxCur].PeriodToNext;
        }

        if ((IdxPrev == SXR_TIM_NIL_IDX) &&  (TotalPeriod >= Period))
        {
            // Insert the new timer to the list head
            sxr_TEnv.TimerEnv [IdxNew].PeriodToNext = Period;
            sxr_TEnv.TimerEnv [IdxNew].IdxNext = IdxFirst;

            sxr_TEnv.TimerEnv [IdxFirst].PeriodToNext = TotalPeriod - Period;
            sxr_TCtx [I2d].IdxFirst = IdxNew;

            if (I2d == SXR_REGULAR_TIMER)
                hal_TimChangeExpirationTime(0, Period - Elapsed);
        }
        else if ((sxr_TEnv.TimerEnv [IdxCur].IdxNext == SXR_TIM_NIL_IDX)
                 &&  (TotalPeriod < Period))
        {
            // Append the new timer to the list tail
            sxr_TEnv.TimerEnv [IdxCur].IdxNext = IdxNew;
            sxr_TEnv.TimerEnv [IdxNew].PeriodToNext = Period - TotalPeriod;
            sxr_TEnv.TimerEnv [IdxNew].IdxNext = SXR_TIM_NIL_IDX;
        }
        else
        {
            // Insert the new timer to the middle of the list, just before
            // the current timer

            // TotalPeriod will always >= sxr_TEnv.TimerEnv [IdxCur].PeriodToNext
            // TotalPeriod will always >= Period
            u32 NewPeriod = sxr_TEnv.TimerEnv [IdxCur].PeriodToNext - (TotalPeriod - Period);
            sxr_TEnv.TimerEnv [IdxNew].PeriodToNext = NewPeriod;
            sxr_TEnv.TimerEnv [IdxCur].PeriodToNext -= NewPeriod;

            sxr_TEnv.TimerEnv [IdxPrev].IdxNext = IdxNew;
            sxr_TEnv.TimerEnv [IdxNew].IdxNext = IdxCur;
        }
    }
    else
    {
        if (I2d == SXR_REGULAR_TIMER)
            hal_TimSetExpirationTime(Period);

        sxr_TCtx [I2d].IdxFirst = IdxNew;

        sxr_TEnv.TimerEnv [IdxNew].IdxNext = SXR_TIM_NIL_IDX;
        sxr_TEnv.TimerEnv [IdxNew].PeriodToNext = Period;
    }

    sxr_TEnv.TimerEnv [IdxNew].Id = Id;
    memcpy ((u8* )sxr_TEnv.TimerEnv [IdxNew].Ctx, Ctx, 4 * SXR_TIMER_CTX_SIZE);

    sxr_TEnv.IdxFree = IdxFree;

    sxr_TCtx [I2d].NbTimer++;

    sxr_ExitSc (Status);
}

/*
==============================================================================
   Function   : sxr_DeleteTimer
 -----------------------------------------------------------------------------
   Scope      : When Id = 0, delete all timer / Ctx  = Ctx [Idx] & Msk.
                When Id != 0 delete all timer / Id = Id & Msk.
   Parameters : - Pointer onto sxr_Timer_t.
                - Pointer on Ctx. When not null, filled with last deleted timer
                context.
                - DelAll : When TRUE, delete all the timer that match criteria.
                - Timer instance.
   Return     : Number of deleted timers.
==============================================================================
*/
u32 sxr_DeleteTimer (sxr_Timer_t *Timer, u32 *Ctx, u8 DelAll, u8 I2d)
{
    return sxr_DeleteComplexTimer (Timer, Ctx, DelAll, NIL, I2d);
}

/*
==============================================================================
   Function   : sxr_TimerMatched
 -----------------------------------------------------------------------------
   Scope      : Check whether timer envelop can match
   Parameters : - Pointer onto sxr_Timer_t.
                - Pointer on timer envelop
   Return     : TRUE if can match.
==============================================================================
*/
static u8 sxr_TimerMatched(sxr_Timer_t *Timer, Timer_t *TEnv)
{
    if (TEnv->Ctx[0] == SXR_DELETED_TIMER)
        return FALSE;

    if (Timer->Id != 0)
    {
        return (((TEnv->Id & Timer->Msk) == Timer->Id) && (TEnv->Id != 0));
    }
    else
    {
        return ((TEnv->Ctx[Timer->Idx] & Timer->Msk) == Timer->Ctx);
    }
}

/*
==============================================================================
   Function   : sxr_DeleteComplexTimer
 -----------------------------------------------------------------------------
   Scope      : When Id = 0, delete all timer / Ctx  = Ctx [Idx] & Msk.
                When Id != 0 delete all timer / Id = Id & Msk.
   Parameters : - Pointer onto sxr_Timer_t.
                - Pointer on Ctx. When not null, filled with last deleted timer
                context.
                - DelAll : When TRUE, delete all the timer that match criteria.
                - Free : Callback before the timer to be deleted.
                - Timer instance.
   Return     : Number of deleted timers.
==============================================================================
*/
u32 sxr_DeleteComplexTimer (sxr_Timer_t *Timer, u32 *Ctx, u8 DelAll, void (*Free)(u16, u32 *), u8 I2d)
{
    u16 IdxPrev;
    u16 IdxCur;
    u16 IdxMem = SXR_TIM_NIL_IDX;
    u8  NbDeletion = 0;
    u16 i;
    u32 Status = sxr_EnterSc ();

    for (i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            IdxPrev = sxr_TCtx [I2d].IdxExpired;
            if (IdxPrev == SXR_TIM_NIL_IDX)
                continue;
            IdxCur = sxr_TEnv.TimerEnv [IdxPrev].IdxNext;
        }
        else
        {
            IdxPrev = SXR_TIM_NIL_IDX;
            IdxCur = sxr_TCtx [I2d].IdxFirst;
        }

        while (IdxCur != SXR_TIM_NIL_IDX)
        {
            if (sxr_TimerMatched(Timer, &(sxr_TEnv.TimerEnv [IdxCur])))
            {
                IdxMem = IdxCur;
                IdxCur = sxr_TEnv.TimerEnv [IdxCur].IdxNext;

                if (Free != NIL)
                    Free (sxr_TEnv.TimerEnv [IdxMem].Id, sxr_TEnv.TimerEnv [IdxMem].Ctx);

                if (i == 0)
                {
                    sxr_TEnv.TimerEnv [IdxMem].Ctx [0] = SXR_DELETED_TIMER;
                }
                else
                {
                    // The head of "IdxExpired" is skipped. So, head check is not needed
                    //
                    // Idealy, SetCntValue can be called when the deleted timer is the first
                    // timer to reduce timer interrupt. But the saving is negelectable.
                    if (IdxMem == sxr_TCtx [I2d].IdxFirst)
                        sxr_TCtx [I2d].IdxFirst = IdxCur;
                    else
                        sxr_TEnv.TimerEnv [IdxPrev].IdxNext = IdxCur;

                    sxr_TEnv.TimerEnv [IdxMem].IdxNext = sxr_TEnv.IdxFree;
                    sxr_TEnv.IdxFree = IdxMem;

                    if (IdxCur != SXR_TIM_NIL_IDX)
                        sxr_TEnv.TimerEnv [IdxCur].PeriodToNext += sxr_TEnv.TimerEnv [IdxMem].PeriodToNext;

                    sxr_TCtx [I2d].NbTimer--;
                }

                NbDeletion++;
                if (!DelAll)
                    break;
            }
            else
            {
                IdxPrev = IdxCur;
                IdxCur  = sxr_TEnv.TimerEnv [IdxCur].IdxNext;
            }
        }

        if (NbDeletion && !DelAll)
            break;
    }

    if (Ctx && NbDeletion)
        memcpy ((u8* )Ctx, (u8 *)sxr_TEnv.TimerEnv [IdxMem].Ctx, 4 * SXR_TIMER_CTX_SIZE);

    sxr_ExitSc (Status);
    return NbDeletion;
}

/*
==============================================================================
   Function   : sxr_TimerExMatched
 -----------------------------------------------------------------------------
   Scope      : Check whether timer envelop can match
   Parameters : - Pointer onto sxr_TimerEx_t.
                - Pointer on timer envelop
   Return     : TRUE if can match.
==============================================================================
*/
static u8 sxr_TimerExMatched(sxr_TimerEx_t *Timer, Timer_t *TEnv)
{
    if (TEnv->Ctx[0] == SXR_DELETED_TIMER)
        return FALSE;

    if ((TEnv->Id & Timer->IdMsk) != Timer->Id)
        return FALSE;

    for (int i = 0; i < SXR_TIMER_CTX_SIZE; i++)
    {
        if ((TEnv->Ctx[i] & Timer->Msk[i]) != Timer->Ctx[i])
            return FALSE;
    }
    return TRUE;
}

// ==============================================================================
//    Function   : sxr_DeleteTimerEx
//  -----------------------------------------------------------------------------
//    Scope      : Delete timer(s) with specified criteria
//    Parameters : - Pointer onto sxr_TimerEx_t.
//                 - Pointer on Ctx. When not null, filled with last deleted timer
//                   context.
//                 - DelAll : When TRUE, delete all the timer that match criteria.
//                 - Free : Callback before the timer to be deleted.
//                 - Timer instance.
//    Return     : Number of deleted timers.
// ==============================================================================
u32 sxr_DeleteTimerEx (sxr_TimerEx_t *Timer, u32 *Ctx, u8 DelAll, void (*Free)(u16, u32 *), u8 I2d)
{
    u16 IdxPrev;
    u16 IdxCur;
    u16 IdxMem = SXR_TIM_NIL_IDX;
    u8  NbDeletion = 0;
    u16 i;
    u32 Status = sxr_EnterSc ();

    for (i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            IdxPrev = sxr_TCtx [I2d].IdxExpired;
            if (IdxPrev == SXR_TIM_NIL_IDX)
                continue;
            IdxCur = sxr_TEnv.TimerEnv [IdxPrev].IdxNext;
        }
        else
        {
            IdxPrev = SXR_TIM_NIL_IDX;
            IdxCur = sxr_TCtx [I2d].IdxFirst;
        }

        while (IdxCur != SXR_TIM_NIL_IDX)
        {
            if (sxr_TimerExMatched(Timer, &(sxr_TEnv.TimerEnv [IdxCur])))
            {
                IdxMem = IdxCur;
                IdxCur = sxr_TEnv.TimerEnv [IdxCur].IdxNext;

                if (Free != NIL)
                    Free (sxr_TEnv.TimerEnv [IdxMem].Id, sxr_TEnv.TimerEnv [IdxMem].Ctx);

                if (i == 0)
                {
                    sxr_TEnv.TimerEnv [IdxMem].Ctx [0] = SXR_DELETED_TIMER;
                }
                else
                {
                    // The head of "IdxExpired" is skipped. So, head check is not needed
                    //
                    // Idealy, SetCntValue can be called when the deleted timer is the first
                    // timer to reduce timer interrupt. But the saving is negelectable.
                    if (IdxMem == sxr_TCtx [I2d].IdxFirst)
                        sxr_TCtx [I2d].IdxFirst = IdxCur;
                    else
                        sxr_TEnv.TimerEnv [IdxPrev].IdxNext = IdxCur;

                    sxr_TEnv.TimerEnv [IdxMem].IdxNext = sxr_TEnv.IdxFree;
                    sxr_TEnv.IdxFree = IdxMem;

                    if (IdxCur != SXR_TIM_NIL_IDX)
                        sxr_TEnv.TimerEnv [IdxCur].PeriodToNext += sxr_TEnv.TimerEnv [IdxMem].PeriodToNext;

                    sxr_TCtx [I2d].NbTimer--;
                }

                NbDeletion++;
                if (!DelAll)
                    break;
            }
            else
            {
                IdxPrev = IdxCur;
                IdxCur  = sxr_TEnv.TimerEnv [IdxCur].IdxNext;
            }
        }

        if (NbDeletion && !DelAll)
            break;
    }

    if (Ctx && NbDeletion)
        memcpy ((u8* )Ctx, (u8 *)sxr_TEnv.TimerEnv [IdxMem].Ctx, 4 * SXR_TIMER_CTX_SIZE);

    sxr_ExitSc (Status);
    return NbDeletion;
}

/*
==============================================================================
   Function   : sxr_Tampax
 -----------------------------------------------------------------------------
   Scope      : Take into account the number of time period ellapsed for
                scheduling. Timer at due date delivery.
   Parameters : Number of time period elapsed since last call.
   Return     : none
==============================================================================
*/
void sxr_Tampax (u32 Period, u8 I2d)
{
    u16 Idx, IdxPrev = SXR_TIM_NIL_IDX;
    u16 IdxExpired;
    u32 Status;

    Status = sxr_EnterSc ();

    Idx = sxr_TCtx [I2d].IdxFirst;

    if (Idx == SXR_TIM_NIL_IDX)
    {
        goto _exit;
    }

    if (I2d == SXR_REGULAR_TIMER)
        Period = hal_TimGetElapsedTime();

    if (Period < sxr_TEnv.TimerEnv [Idx].PeriodToNext)
    {
        sxr_TEnv.TimerEnv [Idx].PeriodToNext -= Period;

        if (I2d == SXR_REGULAR_TIMER)
            hal_TimChangeExpirationTime(Period, sxr_TEnv.TimerEnv [Idx].PeriodToNext);
    }
    else
    {
        u32 BasePeriod = 0;

        while ((Idx != SXR_TIM_NIL_IDX)
                &&     ((sxr_TEnv.TimerEnv [Idx].PeriodToNext + BasePeriod) <= Period))
        {
            BasePeriod += sxr_TEnv.TimerEnv [Idx].PeriodToNext;

            if (I2d == SXR_FRAMED_TIMER && BasePeriod < Period)
            {
#ifdef SXR_FRAMED_TIMER_STRICT_CHECK
                SXS_RAISE ((_SXR|TNB_ARG(2)|TDB|TABORT,TSTR("Period %i > BasePeriod %i forbidden for Framed timer\n",0x06aa0005), Period, BasePeriod));
#else
                SXS_TRACE (_SXR|TNB_ARG(2)|TDB,TSTR("Period %i > BasePeriod %i forbidden for Framed timer\n",0x06aa0005), Period, BasePeriod);
#endif
            }

            IdxPrev = Idx;
            Idx = sxr_TEnv.TimerEnv [Idx].IdxNext;
        }

        // BasePeriod will always <= Period here
        if (BasePeriod < Period)
        {
            if (UNLIKELY(I2d == SXR_FRAMED_TIMER))
            {
#ifdef SXR_FRAMED_TIMER_STRICT_CHECK
                SXS_RAISE ((_SXR|TNB_ARG(2)|TDB|TABORT,TSTR("Period %i > BasePeriod %i forbidden for Framed timer\n",0x06aa0005), Period, BasePeriod));
#else
                SXS_TRACE (_SXR|TNB_ARG(2)|TDB,TSTR("Period %i > BasePeriod %i forbidden for Framed timer\n",0x06aa0005), Period, BasePeriod);
#endif
            }

            if (Idx != SXR_TIM_NIL_IDX)
            {
                sxr_TEnv.TimerEnv [Idx].PeriodToNext -= (Period - BasePeriod);
            }
        }

        // IdxPrev will never be SXR_TIM_NIL_IDX
        sxr_TEnv.TimerEnv [IdxPrev].IdxNext = SXR_TIM_NIL_IDX;

        IdxExpired = sxr_TCtx [I2d].IdxFirst;
        sxr_TCtx [I2d].IdxFirst = Idx;

        if (I2d == SXR_REGULAR_TIMER && Idx != SXR_TIM_NIL_IDX)
            hal_TimChangeExpirationTime(Period, sxr_TEnv.TimerEnv [Idx].PeriodToNext);

        while (IdxExpired != SXR_TIM_NIL_IDX)
        {
            // Save current expired timer list
            sxr_TCtx [I2d].IdxExpired = IdxExpired;

            sxr_ExitSc (Status);

            if (sxr_TEnv.TimerEnv [IdxExpired].Ctx [0] != SXR_DELETED_TIMER)
            {
                if (sxr_TEnv.TimerEnv [IdxExpired].Id & SXR_FUNCTION_TIMER)
                {
                    ((void (*)(void*, u16))sxr_TEnv.TimerEnv [IdxExpired].Ctx [SXR_TIMER_FUNCTION_IDX])
                    ((void *) sxr_TEnv.TimerEnv [IdxExpired].Ctx [SXR_TIMER_PARAM_IDX],
                     sxr_TEnv.TimerEnv [IdxExpired].Id);
                }
                else if (sxr_TEnv.TimerEnv [IdxExpired].Id & SXR_MBX_TIMER)
                {
                    sxr_Send (sxr_TEnv.TimerEnv [IdxExpired].Ctx,
                              SXR_GET_TIMER_MBX(sxr_TEnv.TimerEnv [IdxExpired].Id),
                              SXR_SEND_EVT);
                }
                else
                {
                    (*sxr_TCtx [I2d].TimerDelivery) (sxr_TEnv.TimerEnv [IdxExpired].Ctx);
                }
            }

            Status = sxr_EnterSc();

            Idx = sxr_TEnv.IdxFree;
            sxr_TEnv.IdxFree = IdxExpired;
            IdxExpired = sxr_TEnv.TimerEnv [IdxExpired].IdxNext;
            sxr_TEnv.TimerEnv [sxr_TEnv.IdxFree].IdxNext = Idx;

            sxr_TCtx [I2d].NbTimer--;
        }

        // All expired timers are processed
        sxr_TCtx [I2d].IdxExpired = SXR_TIM_NIL_IDX;
    }

_exit:
    sxr_ExitSc (Status);
}

/*
==============================================================================
   Function   : sxr_IsTimerSet
 -----------------------------------------------------------------------------
   Scope      : Check if a Timer Unit is up and running
   Parameters : Pointer onto sxr_Timer_t.
   Return     : boolean
==============================================================================
*/
u8 sxr_IsTimerSet (sxr_Timer_t *Timer, u8 I2d)
{
    u32 Status = sxr_EnterSc ();
    u16 IdxCur = sxr_TCtx [I2d].IdxFirst;

    while (IdxCur != SXR_TIM_NIL_IDX)
    {
        if ((sxr_TEnv.TimerEnv [IdxCur].Ctx [Timer -> Idx] & Timer -> Msk) == Timer -> Ctx)
        {
            sxr_ExitSc (Status);
            return TRUE;
        }

        IdxCur = sxr_TEnv.TimerEnv [IdxCur].IdxNext;
    }

    sxr_ExitSc (Status);
    return FALSE;
}

/*
==============================================================================
   Function   : sxr_IsTimerExSet
 -----------------------------------------------------------------------------
   Scope      : Check if a Timer Unit is up and running
   Parameters : Pointer onto sxr_TimerEx_t.
   Return     : boolean
==============================================================================
*/
u8 sxr_IsTimerExSet (sxr_TimerEx_t *Timer, u8 I2d)
{
    u32 Status = sxr_EnterSc ();
    u16 IdxCur = sxr_TCtx [I2d].IdxFirst;

    while (IdxCur != SXR_TIM_NIL_IDX)
    {
        if (sxr_TimerExMatched(Timer, &(sxr_TEnv.TimerEnv [IdxCur])))
        {
            sxr_ExitSc (Status);
            return TRUE;
        }

        IdxCur = sxr_TEnv.TimerEnv [IdxCur].IdxNext;
    }

    sxr_ExitSc (Status);
    return FALSE;
}

/*
==============================================================================
   Function   : sxr_TimerList
 -----------------------------------------------------------------------------
   Scope      : Retreive a list of active timers.
   Parameters : Pointer onto TimerList_t, instance number.
   Return     : Number of timer found.
==============================================================================
*/
u8 sxr_TimerList (sxr_Timer_t *Timer, u32 *Tab, u8 Max, u8 I2d)
{
    u32 Status  = sxr_EnterSc ();
    u16 IdxCur  = sxr_TCtx [I2d].IdxFirst;
    u8  NbTimer = 0;

    while (IdxCur != SXR_TIM_NIL_IDX)
    {
        if (sxr_TimerMatched(Timer, &(sxr_TEnv.TimerEnv [IdxCur])))
        {
            Tab [NbTimer] = sxr_TEnv.TimerEnv [IdxCur].Ctx [Timer -> Idx];

            if (++NbTimer >= Max)
            {
                break;
            }
        }
        IdxCur = sxr_TEnv.TimerEnv [IdxCur].IdxNext;
    }
    sxr_ExitSc (Status);

    return NbTimer;
}

/*
==============================================================================
   Function   : sxr_TimerListEmpty
 -----------------------------------------------------------------------------
   Scope      : Check whether the timer list is empty.
   Parameters : instance number.
   Return     : TRUE if empty, and FALSE otherwise.
==============================================================================
*/
bool sxr_TimerListEmpty (u8 I2d)
{
    return (sxr_TCtx [I2d].IdxFirst == SXR_TIM_NIL_IDX);
}

/*
==============================================================================
   Function   : sxr_NextTimerQuery
 -----------------------------------------------------------------------------
   Scope      : Return the next timer remaining period for selected timer type.
   Parameters : Mask to determine the kind of timer to be considered.
                Instance number.
   Return     : Period or SXR_NO_DUE_DATE when no timer found.
==============================================================================
*/
u32 sxr_NextTimerQuery (u16 Mask, u8 I2d)
{
    u32 Status = sxr_EnterSc ();
    s32 Period;
    u16 IdxCur = sxr_TCtx [I2d].IdxFirst;

    if (IdxCur == SXR_TIM_NIL_IDX)
    {
        sxr_ExitSc (Status);
        return SXR_NO_DUE_DATE;
    }

    Period = sxr_TCtx [I2d].GetCntValue != NIL ?
             (s32)(*sxr_TCtx [I2d].GetCntValue) () :
             (s32)sxr_TEnv.TimerEnv [IdxCur].PeriodToNext;

    while ((sxr_TEnv.TimerEnv [IdxCur].Id & Mask) != sxr_TEnv.TimerEnv [IdxCur].Id)
    {
        IdxCur = sxr_TEnv.TimerEnv [IdxCur].IdxNext;

        if (IdxCur != SXR_TIM_NIL_IDX)
        {
            Period += sxr_TEnv.TimerEnv [IdxCur].PeriodToNext;
        }
        else
        {
            break;
        }
    }

    if (Period < 0)
    {
        Period = 0;
    }

    if (IdxCur == SXR_TIM_NIL_IDX)
    {
        Period = SXR_NO_DUE_DATE;
    }

    sxr_ExitSc (Status);
    return (u32)Period;
}

/*
==============================================================================
   Function   : sxr_TimerQuery
 -----------------------------------------------------------------------------
   Scope      : Return remaining period of the identified timer.
   Parameters : Pointer onto TimerQuery_t, instance number.
   Return     : Period or SXR_NO_DUE_DATE when no timer found.
==============================================================================
*/
u32 sxr_TimerQuery (sxr_Timer_t *Timer, u8 I2d)
{
    u32 Status = sxr_EnterSc ();
    s32 TotalPeriod;
    u16 IdxCur = sxr_TCtx [I2d].IdxFirst;

    if (IdxCur != SXR_TIM_NIL_IDX)
    {
        if (sxr_TCtx [I2d].GetCntValue != NIL)
        {
            TotalPeriod = (s32)(*sxr_TCtx [I2d].GetCntValue) ();
        }
        else
        {
            TotalPeriod = (s32)sxr_TEnv.TimerEnv [IdxCur].PeriodToNext;
        }

        while (IdxCur != SXR_TIM_NIL_IDX)
        {
            if (sxr_TimerMatched(Timer, &(sxr_TEnv.TimerEnv [IdxCur])))
            {
                sxr_ExitSc (Status);
                return (TotalPeriod<0) ? 0 : (u32)TotalPeriod;
            }

            IdxCur = sxr_TEnv.TimerEnv [IdxCur].IdxNext;

            if (IdxCur != SXR_TIM_NIL_IDX)
            {
                TotalPeriod += sxr_TEnv.TimerEnv [IdxCur].PeriodToNext;
            }
        }
    }

    sxr_ExitSc (Status);
    return SXR_NO_DUE_DATE;
}

/*
==============================================================================
   Function   : sxr_TimerExQuery
 -----------------------------------------------------------------------------
   Scope      : Return remaining period of the identified timer.
   Parameters : Pointer onto TimerQuery_t, instance number.
   Return     : Period or SXR_NO_DUE_DATE when no timer found.
==============================================================================
*/
u32 sxr_TimerExQuery (sxr_TimerEx_t *Timer, u8 I2d)
{
    u32 Status = sxr_EnterSc ();
    s32 TotalPeriod;
    u16 IdxCur = sxr_TCtx [I2d].IdxFirst;

    if (IdxCur != SXR_TIM_NIL_IDX)
    {
        if (sxr_TCtx [I2d].GetCntValue != NIL)
        {
            TotalPeriod = (s32)(*sxr_TCtx [I2d].GetCntValue) ();
        }
        else
        {
            TotalPeriod = (s32)sxr_TEnv.TimerEnv [IdxCur].PeriodToNext;
        }

        while (IdxCur != SXR_TIM_NIL_IDX)
        {
            if (sxr_TimerExMatched(Timer, &(sxr_TEnv.TimerEnv [IdxCur])))
            {
                sxr_ExitSc (Status);
                return (TotalPeriod<0) ? 0 : (u32)TotalPeriod;
            }

            IdxCur = sxr_TEnv.TimerEnv [IdxCur].IdxNext;

            if (IdxCur != SXR_TIM_NIL_IDX)
            {
                TotalPeriod += sxr_TEnv.TimerEnv [IdxCur].PeriodToNext;
            }
        }
    }

    sxr_ExitSc (Status);
    return SXR_NO_DUE_DATE;
}

/*
==============================================================================
   Function   : sxr_PeriodToTimer
 -----------------------------------------------------------------------------
   Scope      : Return identifier corresponding to the specified Period
                and generic Ctx or Id.
   Parameters : Pointer onto sxr_Timer_t, Period, instance number.
   return     : Updated sxr_Timer_t (Ctx and Id) or NIL.
==============================================================================
*/
sxr_Timer_t *sxr_PeriodToTimer (sxr_Timer_t *Timer, u32 Period, u8 I2d)
{
    u32 Status = sxr_EnterSc ();
    s32 TotalPeriod;
    u16 IdxCur = sxr_TCtx [I2d].IdxFirst;

    if (sxr_TCtx [I2d].GetCntValue != NIL)
    {
        TotalPeriod = (s32)(*sxr_TCtx [I2d].GetCntValue) ();
    }
    else
    {
        TotalPeriod = (s32)sxr_TEnv.TimerEnv [sxr_TCtx [I2d].IdxFirst].PeriodToNext;
    }

    while ((IdxCur != SXR_TIM_NIL_IDX) && (TotalPeriod < Period))
    {
        IdxCur     = sxr_TEnv.TimerEnv [IdxCur].IdxNext;
        TotalPeriod += sxr_TEnv.TimerEnv [IdxCur].PeriodToNext;
    }

    if (TotalPeriod < 0)
    {
        TotalPeriod = 0;
    }

    if (TotalPeriod == Period)
    {
        if (sxr_TimerMatched(Timer, &(sxr_TEnv.TimerEnv [IdxCur])))
        {
            Timer -> Ctx = sxr_TEnv.TimerEnv [IdxCur].Ctx [Timer -> Idx];
            Timer -> Id = sxr_TEnv.TimerEnv [IdxCur].Id;
            sxr_ExitSc (Status);
            return Timer;
        }
    }

    sxr_ExitSc (Status);
    return NIL;
}

#ifdef SXR_FUNCTION_TIMER_ACCURACY_TEST

// SXR timer verification (can be embedded into real environment)
// --------------------------------------------------------------
// The followings can be used to verify SXR timer mechanism. For a "right" timer:
// * The timer should never come earlier than expected
// * The maximum delay should be predictable
//
// For 2G system, the maximum delay shouldn't larger than a GSM frame. So, we set
// the condition a little larger than GSM frame period.
//
// Function timer is used to avoid affected by task priority scheduling delay.

struct FunctionTimerTestParam
{
    u32 period;
    u32 expectedTick;
};

static u32 g_sxrTestTimerMaxDelay = 0;
static struct FunctionTimerTestParam g_sxrTestTimerParam1;
static struct FunctionTimerTestParam g_sxrTestTimerParam2;
static struct FunctionTimerTestParam g_sxrTestTimerParam3;
static struct FunctionTimerTestParam g_sxrTestTimerParam4;

static void sxr_TestFunctionTimer(void *param)
{
    struct FunctionTimerTestParam *p = (struct FunctionTimerTestParam *)param;
    u32 tick = hal_TimGetUpTime();
    s32 delta = tick - p->expectedTick;
    if (delta < 0 || delta > 5*16)
        asm("break 1");

    if (delta > g_sxrTestTimerMaxDelay)
        g_sxrTestTimerMaxDelay = delta;

    u32 period = p->period + (rand() & 0xf);
    p->expectedTick = tick + period;
    sxr_StartFunctionTimer(period, sxr_TestFunctionTimer, p, 0);
}

void sxr_TimerTest(void)
{
    u32 sc = sxr_EnterSc();
    u32 tick = hal_TimGetUpTime();
    g_sxrTestTimerParam1.period = 5*16;
    g_sxrTestTimerParam1.expectedTick = tick;
    g_sxrTestTimerParam2.period = 50*16;
    g_sxrTestTimerParam2.expectedTick = tick;
    g_sxrTestTimerParam3.period = 500*16;
    g_sxrTestTimerParam3.expectedTick = tick;
    g_sxrTestTimerParam4.period = 5000*16;
    g_sxrTestTimerParam4.expectedTick = tick;
    sxr_TestFunctionTimer(&g_sxrTestTimerParam1);
    sxr_TestFunctionTimer(&g_sxrTestTimerParam2);
    sxr_TestFunctionTimer(&g_sxrTestTimerParam3);
    sxr_TestFunctionTimer(&g_sxrTestTimerParam4);
    sxr_ExitSc(sc);
}

#endif
