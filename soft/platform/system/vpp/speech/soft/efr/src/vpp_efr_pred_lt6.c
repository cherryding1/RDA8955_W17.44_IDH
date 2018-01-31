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

#include "vpp_efr_profile_codes.h"
#include "vpp_efr_debug.h"

#include <stdio.h>

#define UP_SAMP      6
#define L_INTERPOL   10
#define FIR_SIZE     (UP_SAMP*L_INTERPOL+1)

/* 1/6 resolution interpolation filter  (-3 dB at 3600 Hz) */

static const INT16 inter_6[FIR_SIZE] =
{
    29443,
    28346, 25207, 20449, 14701, 8693, 3143,
    -1352, -4402, -5865, -5850, -4673, -2783,
    -672, 1211, 2536, 3130, 2991, 2259,
    1170, 0, -1001, -1652, -1868, -1666,
    -1147, -464, 218, 756, 1060, 1099,
    904, 550, 135, -245, -514, -634,
    -602, -451, -231, 0, 191, 308,
    340, 296, 198, 78, -36, -120,
    -163, -165, -132, -79, -19, 34,
    73, 91, 89, 70, 38, 0
};

void Pred_lt_6 (
    INT16 exc[],     /* in/out: excitation buffer */
    INT16 T0,        /* input : integer pitch lag */
    INT16 frac,      /* input : fraction of lag   */
    INT16 L_subfr    /* input : subframe size     */
)
{
    INT16 i, j, k;
    INT16 *x0, *x1, *x2;
    const INT16 *c1, *c2;
    //INT32 s;
    register INT32 s_hi=0;
    register UINT32 s_lo=0;

    VPP_EFR_PROFILE_FUNCTION_ENTER(Pred_lt_6);

    x0 = &exc[-T0];
    frac = NEGATE(frac);

    if (frac < 0)
    {
        frac = ADD(frac, UP_SAMP);
        x0--;
    }
    for (j = 0; j < L_subfr; j++)
    {
        x1 = x0++;
        x2 = x0;
        c1 = &inter_6[frac];
        c2 = &inter_6[SUB (UP_SAMP, frac)];

        s_lo = 0;
        for (i = 0, k = 0; i < L_INTERPOL; i++, k += UP_SAMP)
        {
            //s = L_MAC(s, x1[-i], c1[k]);
            VPP_MLA16(s_hi, s_lo, x1[-i], c1[k]);
            //s = L_MAC(s, x2[i], c2[k]);
            VPP_MLA16(s_hi, s_lo, x2[i], c2[k]);
        }

        //exc[j] = round(s);
        //exc[j] = ROUND(VPP_SCALE64_TO_16(s_hi, s_lo));
        exc[j] = L_SHR_D(L_ADD((INT32)s_lo,0x4000),15);

    }
    VPP_EFR_PROFILE_FUNCTION_EXIT(Pred_lt_6);
    return;
}
