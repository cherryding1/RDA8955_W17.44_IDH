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
#include "vpp_efr_basic_macro.h"
#include "vpp_efr_oper_32b.h"
#include "vpp_efr_cnst.h"

#include "vpp_efr_grid.tab"

//For profiling
#include "vpp_efr_debug.h"
#include "vpp_efr_profile_codes.h"

#include <stdio.h>

/* M = LPC order, NC = M/2 */

#define NC   M/2

/* local function */

static INT16 Chebps (INT16 x, INT16 f[], INT16 n);

void Az_lsp (
    INT16 a[],         /* (i)     : predictor coefficients                 */
    INT16 lsp[],       /* (o)     : line spectral pairs                    */
    INT16 old_lsp[]    /* (i)     : old lsp[] (in case not found 10 roots) */
)
{
    INT16 i, j, nf, ip;
    INT16 xlow, ylow, xhigh, yhigh, xmid, ymid, xint;
    INT16 x, y, sign, exp;
    INT16 *coef;
    INT16 f1[M / 2 + 1], f2[M / 2 + 1];
    INT32 t0=0;


    VPP_EFR_PROFILE_FUNCTION_ENTER(Az_lsp);

    /*-------------------------------------------------------------*
     *  find the sum and diff. pol. F1(z) and F2(z)                *
     *    F1(z) <--- F1(z)/(1+z**-1) & F2(z) <--- F2(z)/(1-z**-1)  *
     *                                                             *
     * f1[0] = 1.0;                                                *
     * f2[0] = 1.0;                                                *
     *                                                             *
     * for (i = 0; i< NC; i++)                                     *
     * {                                                           *
     *   f1[i+1] = a[i+1] + a[M-i] - f1[i] ;                       *
     *   f2[i+1] = a[i+1] - a[M-i] + f2[i] ;                       *
     * }                                                           *
     *-------------------------------------------------------------*/

    f1[0] = 1024;                  /* f1[0] = 1.0 */
    f2[0] = 1024;                  /* f2[0] = 1.0 */

    for (i = 0; i < NC; i++)
    {

        //VPP_MLX16 (t0_hi,t0_lo,a[i + 1], 8192);
        //VPP_MLA16 ( t0_hi, t0_lo,   a[M - i],  8192);
        //t0 = VPP_SCALE64_TO_16( t0_hi, t0_lo);
        //x = EXTRACT_H(t0);

        t0 = (INT32) a[i + 1] + (INT32)a[M - i];
        x = (INT16)(L_SHR_D(t0,2));


        /* f1[i+1] = a[i+1] + a[M-i] - f1[i] */
        f1[i + 1] = SUB (x, f1[i]);

        //VPP_MLX16(t0_hi, t0_lo, a[i + 1], 8192);
        //VPP_MLA16(t0_hi, t0_lo, a[M - i], -8192);
        //x = EXTRACT_H(VPP_SCALE64_TO_16(t0_hi, t0_lo));

        t0 = (INT32) a[i + 1] - (INT32)a[M - i];
        x = (INT16)(L_SHR_D(t0,2));

        //f2[i + 1] = add (x, f2[i]);
        f2[i + 1] = ADD(x, f2[i]);

    }

    /*-------------------------------------------------------------*
     * find the LSPs using the Chebychev pol. evaluation           *
     *-------------------------------------------------------------*/

    nf = 0;                         /* number of found frequencies */
    ip = 0;                         /* indicator for f1 or f2      */

    coef = f1;

    xlow = grid[0];
    ylow = Chebps (xlow, coef, NC);

    j = 0;

    /* while ( (nf < M) && (j < grid_points) ) */
    //while ((sub (nf, M) < 0) && (sub (j, grid_points) < 0))
    while ((SUB (nf, M) < 0) && (SUB (j, grid_points) < 0))
    {
        j++;
        xhigh = xlow;
        yhigh = ylow;
        xlow = grid[j];
        ylow = Chebps (xlow, coef, NC);

        //if (L_mult (ylow, yhigh) <= (INT32) 0L)
        if (L_MULT(ylow, yhigh) <= (INT32) 0L)
        {
            /* divide 4 times the interval */

            for (i = 0; i < 4; i++)
            {
                /* xmid = (xlow + xhigh)/2 */

                // xmid = add (shr (xlow, 1), shr (xhigh, 1));
                xmid = ADD ((SHR_D(xlow, 1)),(SHR_D(xhigh, 1)));

                ymid = Chebps (xmid, coef, NC);

                //if (L_mult (ylow, ymid) <= (INT32) 0L)
                if (L_MULT(ylow, ymid) <= (INT32) 0L)
                {
                    yhigh = ymid;
                    xhigh = xmid;
                }
                else
                {
                    ylow = ymid;
                    xlow = xmid;
                }
            }

            /*-------------------------------------------------------------*
             * Linear interpolation                                        *
             *    xint = xlow - ylow*(xhigh-xlow)/(yhigh-ylow);            *
             *-------------------------------------------------------------*/

            //x = sub (xhigh, xlow);
            x = SUB (xhigh, xlow);
            //y = sub (yhigh, ylow);
            y = SUB (yhigh, ylow);


            if (y == 0)
            {
                xint = xlow;
            }
            else
            {
                sign = y;

                //y = abs_s (y);
                y = ABS_S(y);

                exp = norm_s (y);

                //y = shl (y, exp);
                y = SHL(y, exp);

                y = div_s ((INT16) 16383, y);
                //t0 = L_mult (x, y);
                t0 = L_MULT(x, y);
                //t0 = L_shr (t0, sub (20, exp));
                t0 = L_SHR_V(t0, SUB (20, exp));

                //y = extract_l (t0);     /* y= (xhigh-xlow)/(yhigh-ylow) */
                y = EXTRACT_L(t0);

                if (sign < 0)
                {
                    //y = negate (y);
                    y = NEGATE(y);
                }


                //t0 = L_mult (ylow, y);
                t0 = L_MULT(ylow, y);
                //t0 = L_shr (t0, 11);
                t0 = L_SHR_D(t0, 11);
                //xint = sub (xlow, extract_l (t0)); /* xint = xlow - ylow*y */
                xint = SUB (xlow, EXTRACT_L(t0));
            }

            lsp[nf] = xint;
            xlow = xint;
            nf++;

            if (ip == 0)
            {
                ip = 1;
                coef = f2;
            }
            else
            {
                ip = 0;
                coef = f1;
            }
            ylow = Chebps (xlow, coef, NC);

        }
    }

    /* Check if M roots found */


    //if (sub (nf, M) < 0)
    if (SUB (nf, M) < 0)
    {
        for (i = 0; i < M; i++)
        {
            lsp[i] = old_lsp[i];
        }

    }

    VPP_EFR_PROFILE_FUNCTION_EXIT(Az_lsp);
    return;
}

/************************************************************************
 *
 *  FUNCTION:  Chebps
 *
 *  PURPOSE:   Evaluates the Chebyshev polynomial series
 *
 *  DESCRIPTION:
 *  - The polynomial order is   n = m/2 = 5
 *  - The polynomial F(z) (F1(z) or F2(z)) is given by
 *     F(w) = 2 exp(-j5w) C(x)
 *    where
 *      C(x) = T_n(x) + f(1)T_n-1(x) + ... +f(n-1)T_1(x) + f(n)/2
 *    and T_m(x) = cos(mw) is the mth order Chebyshev polynomial ( x=cos(w) )
 *  - The function returns the value of C(x) for the input x.
 *
 ***********************************************************************/
static INT16 Chebps (INT16 x, INT16 f[], INT16 n)
{
    INT16 cheb;
    INT16 b0_h, b0_l, b1_h, b1_l, b2_h = 256, b2_l =0;
    INT32 t0;


    t0 = f[1] <<4;
    t0 += x;
    t0  = L_SHL_SAT(t0, 10);

    L_Extract (t0, &b1_h, &b1_l);  /* b1 = 2*x + f[1]     */


    t0 = Mpy_32_16 (b1_h, b1_l, x);         /* t0 = 2.0*x*b1        */

    t0 -= (b2_h<<15);
    t0 -= (b2_l);
    t0 += (f[2]<<13);
    t0  = L_SHL_SAT(t0, 1);

    L_Extract (t0, &b0_h, &b0_l);           /* b0 = 2.0*x*b1 - b2 + f[i]*/

    b2_l = b1_l;                 /* b2 = b1; */
    b2_h = b1_h;
    b1_l = b0_l;                 /* b1 = b0; */
    b1_h = b0_h;

    t0 = Mpy_32_16 (b1_h, b1_l, x);         /* t0 = 2.0*x*b1        */

    t0 -= (b2_h<<15);
    t0 -= (b2_l);
    t0 += (f[3]<<13);
    t0  = L_SHL_SAT(t0, 1);

    L_Extract (t0, &b0_h, &b0_l);           /* b0 = 2.0*x*b1 - b2 + f[i]*/

    b2_l = b1_l;                 /* b2 = b1; */
    b2_h = b1_h;
    b1_l = b0_l;                 /* b1 = b0; */
    b1_h = b0_h;

    t0 = Mpy_32_16 (b1_h, b1_l, x);         /* t0 = 2.0*x*b1        */

    t0 -= (b2_h<<15);
    t0 -= (b2_l);
    t0 += (f[4]<<13);
    t0  = L_SHL_SAT(t0, 1);

    L_Extract (t0, &b0_h, &b0_l);           /* b0 = 2.0*x*b1 - b2 + f[i]*/

    b2_l = b1_l;                 /* b2 = b1; */
    b2_h = b1_h;
    b1_l = b0_l;                 /* b1 = b0; */
    b1_h = b0_h;

    t0 = Mpy_32_16 (b1_h, b1_l, x);             /* t0 = x*b1; */

    t0 -= (b2_h<<16);
    t0 -= (b2_l<<1);
    t0 += (f[5]<<13);

    t0  = L_SHL_SAT(t0, 6);

    cheb = EXTRACT_H(t0);

    return (cheb);
}



































