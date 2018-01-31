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





#include "cs_types.h"
#include "vpp_efr_basic_op.h"
#include "vpp_efr_oper_32b.h"

#include "vpp_efr_sig_proc.h"
#include "vpp_efr_basic_macro.h"

#include "vpp_efr_profile_codes.h"
#include "vpp_efr_debug.h"

#define THRESHOLD 27853

/* local function */

static INT16 Lag_max (   /* output: lag found                              */
    INT16 scal_sig[],    /* input : scaled signal                          */
    INT16 scal_fac,      /* input : scaled signal factor                   */
    INT16 L_frame,       /* input : length of frame to compute pitch       */
    INT16 lag_max,       /* input : maximum lag                            */
    INT16 lag_min,       /* input : minimum lag                            */
    INT16 *cor_max);     /* output: normalized correlation of selected lag */

INT16 Pitch_ol (      /* output: open loop pitch lag                        */
    INT16 signal[],   /* input : signal used to compute the open loop pitch */
    /*     signal[-pit_max] to signal[-1] should be known */
    INT16 pit_min,    /* input : minimum pitch lag                          */
    INT16 pit_max,    /* input : maximum pitch lag                          */
    INT16 L_frame     /* input : length of frame to compute pitch           */
)
{
    INT16 i, j;
    INT16 max1, max2, max3;
    INT16 p_max1, p_max2, p_max3;
    INT32 t0;
    INT32 *pSignal;
    INT16  pit_max_aligned;

    /* Scaled signal                                                */
    /* Can be allocated with memory allocation of(pit_max+L_frame)  */

    INT16 scaled_signal[512];
    INT16 *scal_sig, scal_fac;

    register INT32 t0_hi=0;
    register UINT32 t0_lo=0;

    VPP_EFR_PROFILE_FUNCTION_ENTER(Pitch_ol);

    scal_sig = &scaled_signal[pit_max];

    //t0 = 0L;

    if (pit_max & 1)
    {
        VPP_MLA16 (t0_hi, t0_lo, signal[pit_max], signal[pit_max]);
        pit_max_aligned = (pit_max - 1)/2;
        pSignal = (INT32*)&signal[-pit_max + 1];

    }
    else
    {
        pit_max_aligned = pit_max;
        pSignal = (INT32*)&signal[-pit_max];
    }

    for (i = -pit_max_aligned; i < L_frame/2; i++, pSignal++)
    {
        //t0 = L_mac (t0, signal[i], signal[i]);
        //t0 = L_MAC(t0, signal[i], signal[i]);
        VPP_BIMLA (t0_hi, t0_lo, (*pSignal), (*pSignal));
    }

    t0 = VPP_SCALE64_TO_16(t0_hi, t0_lo);
    /*--------------------------------------------------------*
     * Scaling of input signal.                               *
     *                                                        *
     *   if Overflow        -> scal_sig[i] = signal[i]>>2     *
     *   else if t0 < 1^22  -> scal_sig[i] = signal[i]<<2     *
     *   else               -> scal_sig[i] = signal[i]        *
     *--------------------------------------------------------*/

    /*--------------------------------------------------------*
     *  Verification for risk of overflow.                    *
     *--------------------------------------------------------*/


    //if (L_sub (t0, MAX_32) == 0L)               /* Test for overflow */
    if ( L_SUB(t0, MAX_32) == 0L)
    {
        for (i = -pit_max; i < L_frame; i++)
        {
            //scal_sig[i] = shr(signal[i], 3);
            scal_sig[i] = SHR_D(signal[i], 3);

        }
        scal_fac = 3;
    }
    //else if (L_sub (t0, (INT32) 1048576L) < (INT32) 0)
    /* if (t0 < 2^20) */

    //else if (L_sub (t0, (INT32) 1048576L) < (INT32) 0)
    else
    {
        if(L_SUB (t0, (INT32) 1048576L) < (INT32) 0)
        {
            for (i = -pit_max; i < L_frame; i++)
            {
                //scal_sig[i] = shl (signal[i], 3);
                scal_sig[i] = SHL(signal[i], 3);
            }
            scal_fac = -3;
        }

        else
        {
            for (i = -pit_max; i < L_frame; i++)
            {
                scal_sig[i] = signal[i];
            }
            scal_fac = 0;
        }
    }

    /*--------------------------------------------------------------------*
     *  The pitch lag search is divided in three sections.                *
     *  Each section cannot have a pitch multiple.                        *
     *  We find a maximum for each section.                               *
     *  We compare the maximum of each section by favoring small lags.    *
     *                                                                    *
     *  First section:  lag delay = pit_max     downto 4*pit_min          *
     *  Second section: lag delay = 4*pit_min-1 downto 2*pit_min          *
     *  Third section:  lag delay = 2*pit_min-1 downto pit_min            *
     *-------------------------------------------------------------------*/

    //j = shl (pit_min, 2);
    j = SHL(pit_min, 2);
    p_max1 = Lag_max (scal_sig, scal_fac, L_frame, pit_max, j, &max1);

    //i = sub (j, 1);
    i = SUB (j, 1);
    //j = shl (pit_min, 1);
    j = SHL(pit_min, 1);
    p_max2 = Lag_max (scal_sig, scal_fac, L_frame, i, j, &max2);

    //i = sub (j, 1);
    i = SUB (j, 1);
    p_max3 = Lag_max (scal_sig, scal_fac, L_frame, i, pit_min, &max3);

    /*--------------------------------------------------------------------*
     * Compare the 3 sections maximum, and favor small lag.               *
     *-------------------------------------------------------------------*/


    //if (sub (mult (max1, THRESHOLD), max2) < 0)
    if (SUB (MULT (max1, THRESHOLD), max2) < 0)
    {
        max1 = max2;
        p_max1 = p_max2;
    }

    //if (sub (mult (max1, THRESHOLD), max3) < 0)
    if (SUB( MULT(max1, THRESHOLD), max3) < 0)
    {
        p_max1 = p_max3;
    }

    VPP_EFR_PROFILE_FUNCTION_EXIT(Pitch_ol);

    return (p_max1);
}

/*************************************************************************
 *
 *  FUNCTION:  Lag_max
 *
 *  PURPOSE: Find the lag that has maximum correlation of scal_sig[] in a
 *           given delay range.
 *
 *  DESCRIPTION:
 *      The correlation is given by
 *           cor[t] = <scal_sig[n],scal_sig[n-t]>,  t=lag_min,...,lag_max
 *      The functions outputs the maximum correlation after normalization
 *      and the corresponding lag.
 *
 *************************************************************************/

static INT16 Lag_max ( /* output: lag found                               */
    INT16 scal_sig[],  /* input : scaled signal.                          */
    INT16 scal_fac,    /* input : scaled signal factor.                   */
    INT16 L_frame,     /* input : length of frame to compute pitch        */
    INT16 lag_max,     /* input : maximum lag                             */
    INT16 lag_min,     /* input : minimum lag                             */
    INT16 *cor_max)    /* output: normalized correlation of selected lag  */
{
    INT16 i, j;
    INT16 *p, *p1;
    INT32 max, t0;
    INT16 max_h, max_l, ener_h, ener_l;
    register INT32 hi=0;
    register UINT32 lo=0;


    INT16 p_max=0;



    max = MIN_32;

    for (i = lag_max; i >= lag_min; i--)
    {
        p = scal_sig;
        p1 = &scal_sig[-i];
        lo=0;

        for (j = 0; j < L_frame - 3; j +=4)
        {
            //t0 = L_mac (t0, *p, *p1);
            //t0 = L_MAC(t0, *p, *p1);
            VPP_MLA16(hi,lo,p[j],p1[j]);
            VPP_MLA16(hi,lo,p[j+1],p1[j+1]);
            VPP_MLA16(hi,lo,p[j+2],p1[j+2]);
            VPP_MLA16(hi,lo,p[j+3],p1[j+3]);
        }
        t0 = VPP_SCALE64_TO_16(hi, lo);

        if (L_SUB(t0, max) >= 0)
        {
            max = t0;
            p_max = i;
        }
    }

    /* compute energy */
    lo=0;
    p = &scal_sig[-p_max];
    for (i = 0; i < L_frame-3; i+=4)
    {
        //t0 = L_mac (t0, *p, *p);
        //t0 = L_MAC (t0, p[i], p[i]);
        VPP_MLA16(hi,lo,p[i],p[i]);
        VPP_MLA16(hi,lo,p[i+1],p[i+1]);
        VPP_MLA16(hi,lo,p[i+2],p[i+2]);
        VPP_MLA16(hi,lo,p[i+3],p[i+3]);
    }

    t0 = VPP_SCALE64_TO_16(hi, lo);

    /* 1/sqrt(energy) */
    t0 = Inv_sqrt (t0);
    //t0 = L_shl (t0, 1);
    t0 = L_SHL_SAT(t0, 1);

    /* max = max/sqrt(energy)  */

    L_Extract (max, &max_h, &max_l);
    L_Extract (t0, &ener_h, &ener_l);

    t0 = Mpy_32 (max_h, max_l, ener_h, ener_l);
    //t0 = L_shr (t0, scal_fac);
    t0 = L_SHR_V(t0, scal_fac);

    //*cor_max = extract_h (L_shl (t0, 15));
    //*cor_max = EXTRACT_H(L_SHL_SAT(t0, 15));  /* divide by 2 */
    *cor_max = L_SHR_D(t0, 1);

    return (p_max);
}





















