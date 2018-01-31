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

void Weight_Ai (
    INT16 a[],         /* (i)     : a[m+1]  LPC coefficients   (m=10)    */
    const INT16 fac[],       /* (i)     : Spectral expansion factors.          */
    INT16 a_exp[]      /* (o)     : Spectral expanded LPC coefficients   */
)
{
    INT16 i;
    register INT32 hi = 0;
    register UINT32 lo =0;

    VPP_EFR_PROFILE_FUNCTION_ENTER(Weight_Ai);

    a_exp[0] = a[0];
    for (i = 1; i <= m; i++)
    {
        //a_exp[i] = ROUND(L_MULT (a[i], fac[i - 1]));
        VPP_MLX16(hi,lo, a[i], fac[i - 1]);
        a_exp[i] =  L_SHR_D(L_ADD((INT32)lo, 0x4000), 15);
    }

    VPP_EFR_PROFILE_FUNCTION_EXIT(Weight_Ai);

    return;
}
