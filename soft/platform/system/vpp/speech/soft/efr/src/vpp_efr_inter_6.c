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

#define UP_SAMP      6
#define L_INTERPOL   4
#define FIR_SIZE     (UP_SAMP*L_INTERPOL+1)

/* 1/6 resolution interpolation filter  (-3 dB at 3600 Hz) */

static const INT16 inter_6[FIR_SIZE] =
{
    29519,
    28316, 24906, 19838, 13896, 7945, 2755,
    -1127, -3459, -4304, -3969, -2899, -1561,
    -336, 534, 970, 1023, 823, 516,
    220, 0, -131, -194, -215, 0
};

INT16 Interpol_6 (  /* (o)  : interpolated value  */
    INT16 *x,       /* (i)  : input vector        */
    INT16 frac      /* (i)  : fraction            */
)
{
    //INT16 i, k;
    INT16 *x1, *x2;
    const INT16 *c1, *c2;
    //INT32 s;
    register INT32 s_hi=0;
    register UINT32 s_lo=0;

    VPP_EFR_PROFILE_FUNCTION_ENTER(Interpol_6);

    if (frac < 0)
    {
        //frac = add (frac, UP_SAMP);
        frac = ADD (frac, UP_SAMP);
        x--;
    }
    x1 = &x[0];
    x2 = &x[1];
    c1 = &inter_6[frac];
    //c2 = &inter_6[sub (UP_SAMP, frac)];
    c2 = &inter_6[SUB (UP_SAMP, frac)];


    s_lo = 0;
    //for (i = 0, k = 0; i < L_INTERPOL; i++, k += UP_SAMP)
    //{
    //s = L_mac (s, x1[-i], c1[k]);
    //s = L_mac (s, x2[i], c2[k]);
    //s = L_MAC (s, x1[-i], c1[k]);
    //s = L_MAC (s, x2[i], c2[k]);
    VPP_MLA16(s_hi, s_lo, x1[0], c1[0]);
    VPP_MLA16(s_hi, s_lo, x1[-1],c1[6]);
    VPP_MLA16(s_hi, s_lo, x1[-2],c1[12]);
    VPP_MLA16(s_hi, s_lo, x1[-3],c1[18]);

    VPP_MLA16(s_hi, s_lo, x2[0], c2[0]);
    VPP_MLA16(s_hi, s_lo, x2[1], c2[6]);
    VPP_MLA16(s_hi, s_lo, x2[2], c2[12]);
    VPP_MLA16(s_hi, s_lo, x2[3], c2[18]);

    //return round(s);
    VPP_EFR_PROFILE_FUNCTION_EXIT(Interpol_6);

    return ROUND(VPP_SCALE64_TO_16(s_hi, s_lo));
}
