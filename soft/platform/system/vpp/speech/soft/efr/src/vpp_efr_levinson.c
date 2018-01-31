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
#include "vpp_efr_basic_macro.h"

#include "vpp_efr_profile_codes.h"
#include "vpp_efr_debug.h"



/* Lpc order == 10 */

#define M 10

/* Last A(z) for case of unstable filter */

INT16 old_A[M + 1];

void Levinson (
    INT16 Rh[],    /* (i)     : Rh[m+1] Vector of autocorrelations (msb) */
    INT16 Rl[],    /* (i)     : Rl[m+1] Vector of autocorrelations (lsb) */
    INT16 A[],     /* (o)     : A[m]    LPC coefficients  (m = 10)       */
    INT16 rc[]     /* (o)     : rc[4]   First 4 reflection coefficients  */

)
{
    INT16 i, j;
    INT16 hi, lo;
    INT16 Kh, Kl;                /* reflexion coefficient; hi and lo      */
    INT16 alp_h, alp_l, alp_exp; /* Prediction gain; hi lo and exponent   */
    INT16 Ah[M + 1], Al[M + 1];  /* LPC coef. in double prec.             */
    INT16 Anh[M + 1], Anl[M + 1];/* LPC coef.for next iteration in double
                                     prec. */
    INT32 t0, t1, t2;            /* temporary variable                    */
    register INT32 t0_hi=0;
    register UINT32 t0_lo=0;
    /* K = A[1] = -R[1] / R[0] */

    VPP_EFR_PROFILE_FUNCTION_ENTER(Levinson);

    t1 = L_Comp (Rh[1], Rl[1]);
    //t2 = L_abs (t1);                    /* ABS_S R[1]         */
    t2 = L_ABS(t1);
    t0 = Div_32 (t2, Rh[0], Rl[0]);     /* R[1]/R[0]        */

    if (t1 > 0)
    {
        //t0 = L_negate (t0);           /* -R[1]/R[0]       */
        t0 = L_NEGATE(t0);
    }

    L_Extract (t0, &Kh, &Kl);           /* K in DPF         */

    //rc[0] = round (t0);
    rc[0] = ROUND(t0);


    //t0 = L_shr (t0, 4);                 /* A[1] in          */
    t0 = L_SHR_D(t0, 4);
    L_Extract (t0, &Ah[1], &Al[1]);     /* A[1] in DPF      */

    /*  Alpha = R[0] * (1-K**2) */

    t0 = Mpy_32 (Kh, Kl, Kh, Kl);       /* K*K             */
    //t0 = L_abs (t0);                    /* Some case <0 !! */
    t0 = L_ABS(t0);
    //t0 = L_sub ((INT32) 0x7fffffffL, t0); /* 1 - K*K        */
    t0 = L_SUB((INT32) 0x7fffffffL, t0);
    L_Extract (t0, &hi, &lo);           /* DPF format      */
    t0 = Mpy_32 (Rh[0], Rl[0], hi, lo); /* Alpha in        */

    /* Normalize Alpha */

    alp_exp = norm_l (t0);
    //t0 = L_SHL (t0, alp_exp);
    t0 = L_SHL (t0, alp_exp);
    L_Extract (t0, &alp_h, &alp_l);     /* DPF format    */

    /*--------------------------------------*
     * ITERATIONS  I=2 to M                 *
     *--------------------------------------*/

    for (i = 2; i <= M; i++)
    {
        /* t0 = SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i] */

        t0 = 0;
        for (j = 1; j < i; j++)
        {
            //t0 = L_add (t0, Mpy_32 (Rh[j], Rl[j], Ah[i - j], Al[i - j]));
            t0 = L_ADD(t0, Mpy_32 (Rh[j], Rl[j], Ah[i - j], Al[i - j]));
        }
        //t0 = L_shl (t0, 4);
        t0 = L_SHL_SAT(t0, 4);

        t1 = L_Comp (Rh[i], Rl[i]);
        //t0 = L_add (t0, t1);            /* add R[i]        */
        t0 = L_ADD(t0, t1);

        /* K = -t0 / Alpha */

        //t1 = L_abs (t0);
        t1 = L_ABS(t0);
        t2 = Div_32 (t1, alp_h, alp_l); /* ABS_S(t0)/Alpha              */

        if (t0 > 0)
        {
            //t2 = L_negate (t2);         /* K =-t0/Alpha                */
            t2 = L_NEGATE(t2);         /* K =-t0/Alpha                */
        }

        //t2 = L_shl (t2, alp_exp);       /* denormalize; compare to Alpha */
        t2 = L_SHL(t2, alp_exp);
        L_Extract (t2, &Kh, &Kl);       /* K in DPF                      */


        //if (sub (i, 5) < 0)
        if (SUB (i, 5) < 0)
        {
            //rc[i - 1] = round (t2);
            rc[i - 1] = ROUND(t2);

        }
        /* Test for unstable filter. If unstable keep old A(z) */


        //if (sub (abs_s (Kh), 32750) > 0)
        if (ABS_S(Kh)> 32750)
        {
            for (j = 0; j <= M; j++)
            {
                A[j] = old_A[j];
            }

            for (j = 0; j < 4; j++)
            {
                rc[j] = 0;
            }
            VPP_EFR_PROFILE_FUNCTION_EXIT(Levinson);
            return;
        }
        /*------------------------------------------*
         *  Compute new LPC coeff. -> An[i]         *
         *  An[j]= A[j] + K*A[i-j]     , j=1 to i-1 *
         *  An[i]= K                                *
         *------------------------------------------*/

        for (j = 1; j < i; j++)
        {
            //t0 = Mpy_32 (Kh, Kl, Ah[i - j], Al[i - j]);
            t0 = Mpy_32 (Kh, Kl, Ah[i - j], Al[i - j]);
            //t0 = L_mac (t0, Ah[j], 32767);
            //t0 = L_MAC (t0, Ah[j], 32767);
            t0_lo = t0>>1;
            VPP_MLA16(t0_hi, t0_lo, Ah[j], 32767);
            //t0 = L_mac (t0, Al[j], 1);
            //t0 = L_MAC (t0, Al[j], 1);
            VPP_MLA16(t0_hi, t0_lo, Al[j], 1);
            L_Extract (VPP_SCALE64_TO_16(t0_hi, t0_lo), &Anh[j], &Anl[j]);
        }
        //t2 = L_shr (t2, 4);
        t2 = L_SHR_D(t2, 4);

        L_Extract (t2, &Anh[i], &Anl[i]);

        /*  Alpha = Alpha * (1-K**2) */

        t0 = Mpy_32 (Kh, Kl, Kh, Kl);           /* K*K             */
        //t0 = L_abs (t0);                        /* Some case <0 !! */
        t0 = L_ABS(t0);
        //t0 = L_sub ((INT32) 0x7fffffffL, t0);  /* 1 - K*K        */
        t0 = L_SUB((INT32) 0x7fffffffL, t0);
        L_Extract (t0, &hi, &lo);               /* DPF format      */
        t0 = Mpy_32 (alp_h, alp_l, hi, lo);

        /* Normalize Alpha */

        j = norm_l (t0);
        //t0 = L_shl (t0, j);
        t0 = L_SHL(t0, j);
        L_Extract (t0, &alp_h, &alp_l);         /* DPF format    */

        //alp_exp = add (alp_exp, j);             /* Add normalization to alp_exp */
        alp_exp = ADD (alp_exp, j);

        /* A[j] = An[j] */

        for (j = 1; j <= i; j++)
        {
            Ah[j] = Anh[j];
            Al[j] = Anl[j];
        }
    }

    A[0] = 4096;
    for (i = 1; i <= M; i++)
    {
        t0 = L_Comp (Ah[i], Al[i]);
        //old_A[i] = A[i] = round (L_shl (t0, 1));
        old_A[i] = A[i] = ROUND(L_SHL_SAT(t0, 1));
    }
    VPP_EFR_PROFILE_FUNCTION_EXIT(Levinson);
    return;
}










































