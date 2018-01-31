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
#include "vpp_efr_codec.h"
#include "vpp_efr_basic_macro.h"

#include "vpp_efr_profile_codes.h"
#include "vpp_efr_debug.h"


/* L_inter = Length for fractional interpolation = nb.coeff/2 */

#define L_inter 4

/* Local functions */

void Norm_Corr (INT16 exc[], INT16 xn[], INT16 h[], INT16 L_subfr,
                INT16 t_min, INT16 t_max, INT16 corr_norm[]);

INT16 Pitch_fr6 (    /* (o)     : pitch period.                          */
    INT16 exc[],     /* (i)     : excitation buffer                      */
    INT16 xn[],      /* (i)     : target vector                          */
    INT16 h[],       /* (i)     : impulse response of synthesis and
                                    weighting filters                     */
    INT16 L_subfr,   /* (i)     : Length of subframe                     */
    INT16 t0_min,    /* (i)     : minimum value in the searched range.   */
    INT16 t0_max,    /* (i)     : maximum value in the searched range.   */
    INT16 i_subfr,   /* (i)     : indicator for first subframe.          */
    INT16 *pit_frac  /* (o)     : chosen fraction.                       */
)
{
    INT16 i;
    INT16 t_min, t_max;
    INT16 max, lag, frac;
    INT16 *corr;
    INT16 corr_int;
    INT16 corr_v[40];          /* Total length = t0_max-t0_min+1+2*L_inter */

    VPP_EFR_PROFILE_FUNCTION_ENTER(Pitch_fr6);

    /* Find interval to compute normalized correlation */

    //t_min = sub (t0_min, L_inter);
    t_min = SUB (t0_min, L_inter);
    //t_max = add (t0_max, L_inter);
    t_max = ADD (t0_max, L_inter);

    corr = &corr_v[-t_min];

    /* Compute normalized correlation between target and filtered excitation */
    Norm_Corr (exc, xn, h, L_subfr, t_min, t_max, corr);

    /* Find integer pitch */

    max = corr[t0_min];
    lag = t0_min;

    for (i = t0_min + 1; i <= t0_max; i++)
    {

        //if (sub (corr[i], max) >= 0)
        if (corr[i]>= max)
        {
            max = corr[i];
            lag = i;
        }
    }

    /* If first subframe and lag > 94 do not search fractional pitch */


    //if ((i_subfr == 0) && (sub (lag, 94) > 0))
    if ((i_subfr == 0) && (SUB (lag, 94) > 0))
    {
        *pit_frac = 0;
        VPP_EFR_PROFILE_FUNCTION_EXIT(Pitch_fr6);
        return (lag);
    }
    /* Test the fractions around T0 and choose the one which maximizes   */
    /* the interpolated normalized correlation.                          */

    max = Interpol_6 (&corr[lag], -3);
    frac = -3;

    for (i = -2; i <= 3; i++)
    {
        corr_int = Interpol_6 (&corr[lag], i);

        //if (sub (corr_int, max) > 0)
        if (corr_int > max)
        {
            max = corr_int;
            frac = i;
        }
    }

    /* Limit the fraction value in the interval [-2,-1,0,1,2,3] */


    //if (sub (frac, -3) == 0)
    if (frac == -3)
    {
        frac = 3;
        //lag = sub (lag, 1);
        lag --;
    }
    *pit_frac = frac;
    VPP_EFR_PROFILE_FUNCTION_EXIT(Pitch_fr6);
    return (lag);
}

/*************************************************************************
 *
 *  FUNCTION:   Norm_Corr()
 *
 *  PURPOSE: Find the normalized correlation between the target vector
 *           and the filtered past excitation.
 *
 *  DESCRIPTION:
 *     The normalized correlation is given by the correlation between the
 *     target and filtered past excitation divided by the square root of
 *     the energy of filtered excitation.
 *                   corr[k] = <x[], y_k[]>/sqrt(y_k[],y_k[])
 *     where x[] is the target vector and y_k[] is the filtered past
 *     excitation at delay k.
 *
 *************************************************************************/

void
Norm_Corr (INT16 exc[], INT16 xn[], INT16 h[], INT16 L_subfr,
           INT16 t_min, INT16 t_max, INT16 corr_norm[])
{
    INT16 i, j, k;
    INT16 corr_h, corr_l, norm_h, norm_lo;
    INT32 s;
    register INT32 s_hi=0, s2_hi=0;
    register UINT32 s_lo=0, s2_lo =0;

    /* Usally dynamic allocation of (L_subfr) */
    INT16 excf[80];
    INT16 *s_excf, scaled_excf[80];
    INT16 scaling = 0, h_fac=3;//15-12

    k = -t_min;

    /* compute the filtered excitation for the first delay t_min */

    Convolve (&exc[k], h, excf, L_subfr);

    /* scale "excf[]" to avoid overflow */

    // for (j = 0; j < L_subfr; j++)
    //{
    //scaled_excf[j] =shr(excf[j], 2);
    //scaled_excf[j] = SHR_D(excf[j], 2);

    // }

    /* Compute 1/sqrt(energy of excf[]) */

    //s = 0;
    for (j = L_subfr-1; j>=0; j--)
    {
        //s = L_MAC(s, excf[j], excf[j]);
        VPP_MLA16(s_hi, s_lo, excf[j], excf[j]);
        scaled_excf[j] = SHR_D(excf[j], 2);
    }

    //s = VPP_SCALE64_TO_16(s_hi, s_lo);
    //if (L_SUB(s, 0x4000000L)<= 0)    /* if (s <= 2^26) */
    if(((INT32)(s_lo)) <= 0x2000000L)
    {
        s_excf = excf;
    }
    else
    {
        /* "excf[]" is divided by 2 */
        s_excf = scaled_excf;
        h_fac = 1; //15 - 12 - 2;
        scaling = 2;
    }

    /* loop for every possible period */

    for (i = t_min; i <= t_max; i++)
    {
        /* Compute 1/sqrt(energy of excf[]) */

        //s = 0;
        s_lo = 0;
        s2_lo =0;
        for (j = 0; j < L_subfr - 3; j += 4)
        {
            //s = L_mac (s, s_excf[j], s_excf[j]);
            //s = L_MAC(s, s_excf[j], s_excf[j]);
            VPP_MLA16(s_hi, s_lo, s_excf[j], s_excf[j]);
            VPP_MLA16(s2_hi, s2_lo, xn[j], s_excf[j]);
            VPP_MLA16(s_hi, s_lo, s_excf[j+1], s_excf[j+1]);
            VPP_MLA16(s2_hi, s2_lo, xn[j+1], s_excf[j+1]);
            VPP_MLA16(s_hi, s_lo, s_excf[j+2], s_excf[j+2]);
            VPP_MLA16(s2_hi, s2_lo, xn[j+2], s_excf[j+2]);
            VPP_MLA16(s_hi, s_lo, s_excf[j+3], s_excf[j+3]);
            VPP_MLA16(s2_hi, s2_lo, xn[j+3], s_excf[j+3]);
        }

        s = VPP_SCALE64_TO_16(s_hi, s_lo);
        s = Inv_sqrt (s);
        L_Extract (s, &norm_h, &norm_lo);

        s = VPP_SCALE64_TO_16(s2_hi, s2_lo);
        L_Extract (s, &corr_h, &corr_l);


        /* Compute correlation between xn[] and excf[] */

        /* s_lo = 0;
        for (j = 0; j < L_subfr; j++)
        {
            //s = L_mac (s, xn[j], s_excf[j]);
            //s = L_MAC(s, xn[j], s_excf[j]);
            VPP_MLA16(s_hi, s_lo, xn[j], s_excf[j]);
        }*/


        /* Normalize correlation = correlation * (1/sqrt(energy)) */

        corr_norm[i] = (INT16)Mpy_32 (corr_h, corr_l, norm_h, norm_lo);

        //corr_norm[i] = extract_h (L_shl (s, 16));
        //corr_norm[i] = (INT16)s;

        /* modify the filtered excitation excf[] for the next iteration */
        //if (sub (i, t_max) != 0)
        if (i != t_max)
        {
            k--;
            for (j = L_subfr - 1; j > 0; j--)
            {
                //s = L_mult (exc[k], h[j]);
                //s = L_MULT(exc[k], h[j]);
                VPP_MLX16(s_hi, s_lo, exc[k], h[j]);
                //s = L_shl (s, h_fac);
                s = L_SHL_SAT((INT32)s_lo, h_fac);

                //s_excf[j] = add (extract_h (s), s_excf[j - 1]);
                s_excf[j] = ADD ((INT16)L_SHR_D(s, 15), s_excf[j - 1]);

            }
            //s_excf[0] = shr (exc[k], scaling);
            s_excf[0] = SHR_D(exc[k], scaling);
        }
    }
    return;
}





















