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





#define __SXR_TIM_VAR__
#include "sxr_tim.hp"
#undef  __SXR_TIM_VAR__

#include "sxr_ops.h"
#include "sxs_io.h"
#include "sxs_lib.h"


/*
===============================================================================
   Function   : sxr_InitTimer
 -----------------------------------------------------------------------------
   Scope      : Timer initialization.
   Parameters : pointer onto delivery function to be called at timer due date,
                pointer onto next timer notification function (optional) to notify
                the period of the next timer to come.
                pointer onto a fuction which allows to get the physical value of
                the hardware timer when it exists (optional).
   Return     : none.
==============================================================================
*/
void sxr_InitTimer (void (*TimerDelivery)(u32 *),
                    void (*SetCntValue)(u32),
                    u32  (*GetCntValue)(void),
                    u8  I2d)
{
    static u8 InitDone = FALSE;

    if (I2d >= SXR_TIMER_NB_INSTANCE)
    {
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("Too many instances\n",0x06c20001)));
    }

    if (InitDone == FALSE)
    {
        u16 i;

        for (i=0; i<SXR_NB_MAX_TIMER_ENVELOPPE; i++)
        {
            sxr_TEnv.TimerEnv [i].PeriodToNext = 0;
            sxr_TEnv.TimerEnv [i].IdxNext   = i+1;
            sxr_TEnv.TimerEnv [i].Id        = 0;
            memset ((u8 *)sxr_TEnv.TimerEnv [i].Ctx, 0, 4 * SXR_TIMER_CTX_SIZE);
        }

        sxr_TEnv.TimerEnv [SXR_NB_MAX_TIMER_ENVELOPPE - 1].IdxNext = SXR_TIM_NIL_IDX;
        sxr_TEnv.IdxFree = 0;

        InitDone = TRUE;
    }

    sxr_TCtx [I2d].TimerDelivery = TimerDelivery;
    sxr_TCtx [I2d].SetCntValue   = SetCntValue;
    sxr_TCtx [I2d].GetCntValue = GetCntValue;

    if (sxr_TCtx [I2d].TimerDelivery == NIL)
    {
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("Timer Delivery function missing\n",0x06c20002)));
    }

    sxr_TCtx [I2d].IdxFirst = SXR_TIM_NIL_IDX;
    sxr_TCtx [I2d].IdxExpired = SXR_TIM_NIL_IDX;
    sxr_TCtx [I2d].NbTimer  = 0;
}

/*
===============================================================================
   Function   : sxr_CheckTimer
 -----------------------------------------------------------------------------
   Scope      : Print a status for all the existing timer.
   Parameters : none.
   Return     : none.
===============================================================================
*/
u8 sxr_CheckTimer (void)
{
    u16 Idx;
    u32 Period = 0;
    u32 NbTimer = 0;
    u32 I2d;

    for (I2d=0; I2d < SXR_TIMER_NB_INSTANCE; I2d++)
    {
        Idx = sxr_TCtx [I2d].IdxFirst;
        Period = 0;

        if (sxr_TCtx [I2d].TimerDelivery != NIL)
        {
            SXS_FPRINTF ((_SXR|TSTDOUT|TDB|TNB_ARG(1),TSTR("Instance %i\n",0x06c20003), I2d));

            while (Idx != SXR_TIM_NIL_IDX)
            {
                if (Idx != SXR_TIM_NIL_IDX)
                {
                    Period += sxr_TEnv.TimerEnv [Idx].PeriodToNext;
                }

                SXS_FPRINTF ((_SXR|TIDU|TSTDOUT|TDB|TNB_ARG(4),TSTR("Ctx 0x%lx 0x%lx Id %x period %i\n",0x06c20004), sxr_TEnv.TimerEnv [Idx].Ctx [0], sxr_TEnv.TimerEnv [Idx].Ctx [1], sxr_TEnv.TimerEnv [Idx].Id, Period));
                NbTimer++;

                Idx = sxr_TEnv.TimerEnv [Idx].IdxNext;
            }
        }
    }
    SXS_FPRINTF ((_SXR|TSTDOUT|TDB|TNB_ARG(2),TSTR("Nb timers %i / %i\n",0x06c20005), NbTimer, SXR_NB_MAX_TIMER_ENVELOPPE));

    return (u8)NbTimer;
}

/*
===============================================================================
   Function   : sxr_PrintTimer
 -----------------------------------------------------------------------------
   Scope      : Print status corresponding to identified timers.
   Parameters : Pointer onto sxr_Timer_t.
   Return     : none.
===============================================================================
*/
void sxr_PrintTimer (sxr_Timer_t *Timer, u8 I2d)
{
    u16 Idx = sxr_TCtx [I2d].IdxFirst;
    u32 Period = 0;

    while (Idx != SXR_TIM_NIL_IDX)
    {
        if (Idx != SXR_TIM_NIL_IDX)
        {
            Period += sxr_TEnv.TimerEnv [Idx].PeriodToNext;
        }

        if (((Timer -> Id != 0)
                &&  ((sxr_TEnv.TimerEnv [Idx].Id & Timer -> Msk) == Timer -> Id))
                || ((Timer -> Id == 0)
                    &&  ((sxr_TEnv.TimerEnv [Idx].Ctx [Timer -> Idx] & Timer -> Msk) == Timer -> Ctx)))
        {
            SXS_FPRINTF ((_SXR|TIDU|TSTDOUT|TDB|TNB_ARG(4),TSTR("Ctx %lx %lx Id %x period %i\n",0x06c20006), sxr_TEnv.TimerEnv [Idx].Ctx [0], sxr_TEnv.TimerEnv [Idx].Ctx [1], sxr_TEnv.TimerEnv [Idx].Id, Period));
        }
        Idx = sxr_TEnv.TimerEnv [Idx].IdxNext;
    }
}


