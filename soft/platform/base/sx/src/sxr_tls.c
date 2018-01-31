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




#define __SXR_TLS_VAR__
#include "sxr_tls.h"
#undef __SXR_TLS_VAR__

#include "sxs_type.h"
#include "sxs_io.h"
#include "sxs_lib.h"
#include "sxr_csp.h"
#include "sxr_ops.h"
#include "sxr_tksd.hp"


static u16 Xn [SXR_RND_NB_MAX_USR];
static u16 RLimit[SXR_RND_NB_MAX_USR];
static u16 LastInterval[SXR_RND_NB_MAX_USR];



/*
==============================================================================
   Function   : sxr_RandomInit
 ----------------------------------------------------------------------------
   Scope      : Initialize random table pointer. Pointed table must give access
                to a 128 bytes array containing 7 bits random values.
   Parameters : Random Table address.
   Return     : none
==============================================================================
*/
void sxr_RandomInit (u8 *RandomTab)
{
    u32 i;

    for(i=0; i<SXR_RND_NB_MAX_USR; i++)
        Xn[i] = (u16)hal_TimGetUpTime();
}

#define PRBS_POLY 0xb400
#define PRPS_POLY_LEN 16
/*
==============================================================================
   Function   : sxr_GenPrbs
 ----------------------------------------------------------------------------
   Scope      : Generate a random value from prbs sequence.
                    The polynomial for the Pseudo Random Binary Sequence(PRBS) is
                    G(16) = X^16+X^14+X^13+X^11+1
   Parameters : user index.
   Return     : random value[1..65536]
==============================================================================
*/
u16 sxr_GenPrbs(u8 Usr)
{
    u16 poly = PRBS_POLY;
    u16 L = ((u32)1<<PRPS_POLY_LEN) - 1;
    u16 tmp,sum,registers;

    registers = Xn[Usr] & L;
    tmp = poly&registers;
    sum = ((tmp>>15)&1)^((tmp>>13)&1)^((tmp>>12)&1)^((tmp>>10)&1);
    registers <<= 1;
    registers |= sum;
    registers &= L;
    return registers;
}

/*
==============================================================================
   Function   : sxr_Random
 ----------------------------------------------------------------------------
   Scope      : Generate a random value in the required interval.
                    Each output value has exactly the same probability.
                    This is obtained by rejecting certain bit values so that the number
                    of possible bit values is divisible by the interval length
   Parameters : Maximum value in the interval.
   Return     : none
==============================================================================
*/
u16 _sxr_Random (u16 Max, u8 Usr)
{
    u16 interval = (Max + 1);
    u16 iran;
    u32 longran;
    u16 remainder;

    if (interval != LastInterval[Usr])
    {
        // Interval length has changed. Must calculate rejection limit
        // Reject when remainder > 2^16 / interval * interval
        // No rejection when interval is a power of 2.
        RLimit[Usr] = (u16)(((u32)1 << 16) / interval) * interval - 1;
        LastInterval[Usr] = interval;
    }

    do   // Rejection loop
    {
        Xn [Usr] = sxr_GenPrbs(Usr);
        Xn [Usr] = ((9477 * Xn [Usr]) + 39) & 0xFFFF;
        longran = Xn [Usr] * interval;
        iran =(u16)(longran >> 16);
        remainder = (u16)longran;
    }
    while (remainder > RLimit[Usr]);

    return (iran);
}



void sxr_TestRaise (void)
{
    sxs_Raise (_SXR|TABORT|TDB,TSTR("Raise...\n",0x06ac0001));
}


void sxr_DeadLock (void)
{
    while (1);
}

/*
================================================================================
  Function   : sxr_ComputeFcsM
--------------------------------------------------------------------------------
  Scope      : Compute the FCS (CRC 24 bits) for multiple blocks of data
  Parameters : Length over which to calculate the FCS, pointer to the data,
               the initial remainder, flag to tell if it is the final block
  Return     : The intermediate remainder, or the FCS if it is the final block
================================================================================
*/
u32 sxr_ComputeFcsM (u32 Len, const u8 *Frm, u32 R, bool Final)
{
    u32 i;
    u32 fcs;

    for (i = 0; i < Len; i++)
    {
        R = (R<<8) ^ sxr_FcsTab[(u8)((R>>16)^(sxr_MsbToLsb[*Frm++]))];
    }

    if (Final)
    {
        R = ~R;  // ones complement of the remainder

        fcs = (sxr_MsbToLsb[(u8)(R >> 16)])
              | (sxr_MsbToLsb[(u8)(R >> 8)] << 8)
              | (sxr_MsbToLsb[(u8)(R)] << 16);

        return fcs;
    }

    return R;
}

/*
================================================================================
  Function   : sxr_ComputeFcs
--------------------------------------------------------------------------------
  Scope      : Compute the FCS (CRC 24 bits)
  Parameters : Length over which to calculate the FCS, pointer to the data,
               pointer where to store the FCS calculated
  Return     : none
================================================================================
*/
void sxr_ComputeFcs (u32 Len, const u8 *Frm, u8 *Fcs)
{
    u32 fcs;

    SXS_TRACE(_SXR|TDB|TNB_ARG(1),TSTR("Compute FCS over %d bytes",0x06ac0002), Len);

    fcs = sxr_ComputeFcsM (Len, Frm, 0xFFFFFFFF, TRUE);

    Fcs[0] = fcs & 0xFF;
    Fcs[1] = (fcs >> 8) & 0xFF;
    Fcs[2] = (fcs >> 16) & 0xFF;

    SXS_TRACE(_SXR|TDB|TNB_ARG(3),TSTR("Calculated FCS : 0x%02X 0x%02X 0x%02X",0x06ac0003), Fcs[0], Fcs[1], Fcs[2]);
}

/*
================================================================================
  Function   : sxr_ComputeMedianWithIdxBuf
--------------------------------------------------------------------------------
  Scope      : Compute the median value for an array of data. To speed up
               the computation, an extra buffer to hold the data index should
               be provided.
  Parameters : Pointer to the data, pointer to the index buffer, and length
               over which to calculate the median.
  Return     : The median.
================================================================================
*/
#define SXR_COMPUTE_MEDIAN_WITH_IDX_BUF(DataType) \
DataType sxr_ComputeMedianWithIdxBuf_##DataType(DataType *Data, u8 *Buf, u8 Len) \
{ \
    u32 i, j, t, h; \
    DataType m; \
 \
    h = Len / 2; \
 \
    for (i = 0; i < Len; i++) \
    { \
        Buf[i] = i; \
    } \
 \
    for (i = 0; i < Len && i <= h; i++) \
    { \
        for (j = i + 1; j < Len; j++) \
        { \
            if (Data[Buf[i]] < Data[Buf[j]]) \
            { \
                t = Buf[i]; \
                Buf[i] = Buf[j]; \
                Buf[j] = t; \
            } \
        } \
    } \
 \
    if (Len & 1) \
    { \
        m = Data[Buf[h]]; \
    } \
    else \
    { \
        m = (Data[Buf[h - 1]] + Data[Buf[h]]) / 2; \
    } \
 \
    return m; \
} \

SXR_COMPUTE_MEDIAN_WITH_IDX_BUF(u16)

/*
================================================================================
  Function   : sxr_ComputeMedian
--------------------------------------------------------------------------------
  Scope      : Compute the median value for an array of data.
  Parameters : Pointer to the data, and length over which to calculate
               the median.
  Return     : The median.
================================================================================
*/
#define SXR_COMPUTE_MEDIAN(DataType) \
DataType sxr_ComputeMedian_##DataType(DataType *Data, u32 Len) \
{ \
    u32 i, j, h; \
    u32 total, cnt; \
    DataType pmax, cmax, m; \
 \
    h = Len / 2; \
    total = 0; \
    pmax = cmax = Data[0]; \
 \
    for (i = 0; i < Len; i++) \
    { \
        cnt = 0; \
        for (j = 0; j < Len; j++) \
        { \
            if (i == 0 || pmax > Data[j]) \
            { \
                if (cnt == 0) \
                { \
                    cmax = Data[j]; \
                    cnt = 1; \
                } \
                else if (cmax < Data[j]) \
                { \
                    cmax = Data[j]; \
                    cnt = 1; \
                } \
                else if (cmax == Data[j]) \
                { \
                    cnt++; \
                } \
            } \
        } \
 \
        if (total + cnt > h) \
        { \
            break; \
        } \
        pmax = cmax; \
        total += cnt; \
    } \
 \
    if ((Len & 1) || total < h) \
    { \
        m = cmax; \
    } \
    else \
    { \
        m = (pmax + cmax) / 2; \
    } \
 \
    return m; \
} \

SXR_COMPUTE_MEDIAN(u16)
SXR_COMPUTE_MEDIAN(s16)
SXR_COMPUTE_MEDIAN(u8)
SXR_COMPUTE_MEDIAN(s8)

/*
==============================================================================
   Function   : sxr_Sleep
 -----------------------------------------------------------------------------
   Scope      : Suspend the active task for the required period.
   Parameters : Period expressed in regular timer basic unit.
   Return     : None.
==============================================================================
*/
void sxr_Sleep (u32 Period)
{
    u32 Status;

    Status = sxr_EnterSc ();

#ifndef CT_NO_DEBUG
    u32 k1;
    HAL_SYS_GET_K1(k1);
    if (k1 != 0)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1), TSTR("sxr_Sleep() must be run in task context! (k1=0x%08x)\n",0x06ac0004),
                    k1));
    }
    if (sxr_Task.Active == sxr_Task.Idle)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(2), TSTR("sxr_Sleep() cannot be run in idle task! (active=%d, idle=%d)\n",0x06ac0005),
                    sxr_Task.Active, sxr_Task.Idle));
    }
    if (sxr_Task.ScheduleDisable != 0)
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1), TSTR("sxr_Sleep() cannot suspend current task if schedule disabled! (schDis=%d)\n",0x06ac0006),
                    sxr_Task.ScheduleDisable));
    }
#endif // ! CT_NO_DEBUG

    if (Period != 0)
    {
        // The parameter passed to sxr_ResummeTask() should is a u8. But it is passed
        // through the SXR function timer mechanism, which only passes a void* parameter
        // to the called function. This is why we need the double casting.
        sxr_StartFunctionTimer (Period, sxr_ResumeTask, (void*)(u32)sxr_Task.Active, _SXR);

        sxr_SuspendTask (sxr_Task.Active);
    }

    sxr_ExitSc (Status);
}


