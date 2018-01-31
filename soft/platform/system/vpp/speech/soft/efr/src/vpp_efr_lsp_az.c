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

#include "vpp_efr_debug.h"
#include "vpp_efr_profile_codes.h"

/* local function */

static void Get_lsp_pol (INT16 *lsp, INT32 *f);

void Lsp_Az (
    INT16 lsp[],        /* (i)  : line spectral frequencies            */
    INT16 a[]           /* (o)  : predictor coefficients (order = 10)  */
)
{
    INT16 i, j;
    INT32 f1[6], f2[6];
    INT32 t0;

    VPP_EFR_PROFILE_FUNCTION_ENTER(Lsp_Az);

    Get_lsp_pol (&lsp[0], f1);
    Get_lsp_pol (&lsp[1], f2);

    // for (i = 5; i > 0; i--)
    // {
    //f1[i] = L_add (f1[i], f1[i - 1]);
    // f1[i] = L_ADD (f1[i], f1[i - 1] );     /* f1[i] += f1[i-1]; */
    //f2[i] = L_sub (f2[i], f2[i - 1]);
    //f2[i] = L_SUB(f2[i], f2[i - 1]);     /* f2[i] -= f2[i-1]; */
    //}

    a[0] = 4096;
    for (i = 5, j = 6; i > 0; i--, j++)
    {
        f1[i] = L_ADD (f1[i], f1[i - 1] );     /* f1[i] += f1[i-1]; */
        //f2[i] = L_sub (f2[i], f2[i - 1]);
        f2[i] = L_SUB(f2[i], f2[i - 1]);
        //t0 = L_add (f1[i], f2[i]);           /* f1[i] + f2[i] */
        t0 = L_ADD(f1[i], f2[i]);
        //a[i] = extract_l (L_shr_r (t0, 13));
        a[i] = EXTRACT_L(L_shr_r (t0, 13));
        //t0 = L_sub (f1[i], f2[i]);           /* f1[i] - f2[i] */
        t0 = L_SUB(f1[i], f2[i]);
        //a[j] = extract_l(L_shr_r (t0, 13));
        a[j] = EXTRACT_L(L_shr_r (t0, 13));
    }
    VPP_EFR_PROFILE_FUNCTION_EXIT(Lsp_Az);
    return;
}

/*************************************************************************
 *
 *  FUNCTION:  Get_lsp_pol
 *
 *  PURPOSE:  Find the polynomial F1(z) or F2(z) from the LSPs.
 *            If the LSP vector is passed at address 0  F1(z) is computed
 *            and if it is passed at address 1  F2(z) is computed.
 *
 *  DESCRIPTION:
 *       This is performed by expanding the product polynomials:
 *
 *           F1(z) =   product   ( 1 - 2 lsp[i] z^-1 + z^-2 )
 *                   i=0,2,4,6,8
 *           F2(z) =   product   ( 1 - 2 lsp[i] z^-1 + z^-2 )
 *                   i=1,3,5,7,9
 *
 *       where lsp[] is the LSP vector in the cosine domain.
 *
 *       The expansion is performed using the following recursion:
 *
 *            f[0] = 1
 *            b = -2.0 * lsp[0]
 *            f[1] = b
 *            for i=2 to 5 do
 *               b = -2.0 * lsp[2*i-2];
 *               f[i] = b*f[i-1] + 2.0*f[i-2];
 *               for j=i-1 down to 2 do
 *                   f[j] = f[j] + b*f[j-1] + f[j-2];
 *               f[1] = f[1] + b;
 *
 *************************************************************************/

static void Get_lsp_pol (INT16 *lsp, INT32 *f)
{
    INT16 i, j, hi, lo;
    INT32 t0;
    register INT32 f_hi=0;
    register UINT32 f_lo=0;

    /* f[0] = 1.0;             */
    //*f = L_mult (4096, 2048);
    //*f = L_MULT(4096, 2048);
    *f = 0x1000000;
    f++;
    //*f = L_msu ((INT32) 0, *lsp, 512);    /* f[1] =  -2.0 * lsp[0];  */
    //*f = L_MSU((INT32) 0, *lsp, 512);
    //*f = -L_MULT(*lsp, 512);
    *f = -((*lsp)<< 10);
    f++;

    lsp += 2;                              /* Advance lsp pointer     */

    for (i = 2; i <= 5; i++)
    {
        *f = f[-2];

        for (j = 1; j < i; j++, f--)
        {
            L_Extract (f[-1], &hi, &lo);
            t0 = Mpy_32_16 (hi, lo, *lsp); /* t0 = f[-1] * lsp    */
            //t0 = L_shl (t0, 1);
            t0 = L_SHL_SAT(t0, 1);
            //*f = L_add (*f, f[-2]);
            *f = L_ADD(*f, f[-2]);         /* *f += f[-2]      */
            //*f = L_sub (*f, t0);
            *f = L_SUB(*f, t0);
            /* *f -= t0            */
        }
        //*f = L_msu (*f, *lsp, 512);         /* *f -= lsp<<9     */
        //*f = L_MSU(*f, *lsp, 512);
        f_lo = *f >> 1;
        VPP_MLA16(f_hi,f_lo, *lsp, -512);
        *f = VPP_SCALE64_TO_16(f_hi,f_lo);

        f += i;                            /* Advance f pointer   */
        lsp += 2;                          /* Advance lsp pointer */
    }

    return;
}





















