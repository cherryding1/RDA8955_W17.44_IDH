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

//For profiling
#include "vpp_efr_profile_codes.h"
#include "vpp_efr_debug.h"

INT16 Autocorr (
    INT16* x,         /* (i)    : Input signal                    */
    INT16 m,           /* (i)    : LPC order                       */
    INT16* r_h,       /* (o)    : Autocorrelations  (msb)         */
    INT16* r_l,       /* (o)    : Autocorrelations  (lsb)         */
    const INT16* wind       /* (i)    : window for LPC analysis         */
)
{
    INT16 i, j, norm;

    // even aligned
    INT32 ye[L_WINDOW/2];
    INT16 * ye16 = (INT16 *)ye;

    // odd aligned
    INT32 yo[L_WINDOW/2 + 1];
    INT16 * yo16 = (INT16 *)yo;

    INT32 sum =0;
    INT16 overfl, overfl_shft;

    register INT32 sum_hi=0;
    register UINT32 sum_lo=0;

    VPP_EFR_PROFILE_FUNCTION_ENTER (Autocorr);

    /* Windowing of signal */

    for (i = 0; i < L_WINDOW; i++)
    {
        //ye16[i] = MULT_R(x[i], wind[i]);
        VPP_MULT_R(sum_hi, sum_lo, x[i], wind[i]);
        ye16[i] = (INT16)L_SHR_D((INT32)sum_lo, 15);
    }

    /* Compute r[0] and test for overflow */

    overfl_shft = 0;

    do
    {
        overfl = 0;
        sum_lo =0;
        sum_hi = 0;

        for (i = 0; i < L_WINDOW/2; i++)
        {
            VPP_BIMLA ( sum_hi, sum_lo,  ye[i],  ye[i]);
        }

        sum = VPP_SCALE64_TO_16( sum_hi, sum_lo);

        /* If overflow divide y[] by 4 */

        if (L_SUB(sum, MAX_32) == 0L)
        {
            overfl_shft += 4;

            overfl = 1;                 /* Set the overflow flag */

            for (i = 0; i < L_WINDOW; i++)
            {
                ye16[i] >>= 2;
            }
        }
    }
    while (overfl != 0);

    // copy even-aligned to odd-aligned
    for (i = 0; i < L_WINDOW; i++)
    {
        yo16[i+1] = ye16[i];
    }

    /* Avoid the case of all zeros */
    sum += 1;

    /* Normalization of r[0] */
    norm = norm_l (sum);
    sum = L_SHL(sum, norm);

    L_Extract (sum, &r_h[0], &r_l[0]); /* Put in DPF format (see oper_32b) */

    /* r[1] to r[m] */

    for (i = 1; i <= m; i++)
    {
        INT32 * y_j;
        INT32 * y_i;

        //sum = 0;
        sum_hi =0;
        sum_lo =0;

        if (i & 1)
        {
            // odd aligned
            y_j = (INT32 *)&ye16[0];
            y_i = (INT32 *)&yo16[i+1];

            VPP_MLA16(sum_hi,sum_lo, ye16[L_WINDOW - 1 - i], ye16[L_WINDOW - 1]);
        }
        else
        {
            y_j = (INT32 *)&ye16[0];
            y_i = (INT32 *)&ye16[i];
        }

        for (j = 0; j < (L_WINDOW - i)/2; j++, y_j++, y_i++)
        {
            //sum = L_mac (sum, y[j], y[j + i]);
            //sum = L_MAC(sum, y[j], y[j + i]);
            VPP_BIMLA(sum_hi,sum_lo, (*y_j), (*y_i));
        }

        sum = VPP_SCALE64_TO_16(sum_hi, sum_lo);

        //sum = L_shl (sum, norm);
        sum = L_SHL(sum, norm);

        L_Extract (sum, &r_h[i], &r_l[i]);
    }

    //norm = sub (norm, overfl_shft);
    norm = SUB (norm, overfl_shft);

    VPP_EFR_PROFILE_FUNCTION_EXIT(Autocorr);
    return norm;
}










































