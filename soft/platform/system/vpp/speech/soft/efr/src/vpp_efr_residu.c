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


/* m = LPC order == 10 */
#define m 10

void Residu (
    INT16 a[], /* (i)     : prediction coefficients                      */
    INT16 x[], /* (i)     : speech signal                                */
    INT16 y[], /* (o)     : residual signal                              */
    INT16 lg   /* (i)     : size of filtering                            */
)
{
    INT16 i;
    //INT32 s;
    register INT32 s_hi=0;
    register UINT32 s_lo=0;

    VPP_EFR_PROFILE_FUNCTION_ENTER(Residu);

    for (i = 0; i < lg; i++)
    {
        //s = L_MULT(x[i], a[0]);
        VPP_MLX16(s_hi,s_lo,x[i], a[0]);

        //for (j = 1; j <= m; j++)
        //{
        //s = L_MAC(s, a[j], x[i - j]);
        VPP_MLA16(s_hi,s_lo, a[1], x[i - 1]);
        VPP_MLA16(s_hi,s_lo, a[2], x[i - 2]);
        VPP_MLA16(s_hi,s_lo, a[3], x[i - 3]);
        VPP_MLA16(s_hi,s_lo, a[4], x[i - 4]);
        VPP_MLA16(s_hi,s_lo, a[5], x[i - 5]);
        VPP_MLA16(s_hi,s_lo, a[6], x[i - 6]);
        VPP_MLA16(s_hi,s_lo, a[7], x[i - 7]);
        VPP_MLA16(s_hi,s_lo, a[8], x[i - 8]);
        VPP_MLA16(s_hi,s_lo, a[9], x[i - 9]);
        VPP_MLA16(s_hi,s_lo, a[10], x[i - 10]);
        //}
        //s = VPP_SCALE64_TO_16(s_hi,s_lo);
        //s = L_SHL_SAT(s, 3);
        //y[i] = ROUND(s);

        y[i] = (INT16)(L_SHR_D(L_ADD((INT32)s_lo,0x800),12));

    }
    VPP_EFR_PROFILE_FUNCTION_EXIT(Residu);
    return;
}





















