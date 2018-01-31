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

#include "vpp_efr_debug.h"
#include "vpp_efr_profile_codes.h"

void Convolve (
    INT16 x[],        /* (i)     : input vector                           */
    INT16 h[],        /* (i)     : impulse response                       */
    INT16 y[],        /* (o)     : output vector                          */
    INT16 L           /* (i)     : vector size                            */
)
{
    INT16 i, n;
    //INT32 s;
    register INT32 s_hi=0;
    register UINT32 s_lo=0;


    VPP_EFR_PROFILE_FUNCTION_ENTER(Convolve);

    for (n = 0; n < L; n++)
    {
        s_lo = 0;
        for (i = 0; i <= n; i++)
        {
            //s = L_mac (s, x[i], h[n - i]);
            VPP_MLA16(s_hi, s_lo, x[i], h[n - i]);
        }
        //s = L_shl (s, 3);
        //s = L_SHL_SAT(VPP_SCALE64_TO_16(s_hi, s_lo), 3);
        //y[n] = EXTRACT_H(s);

        y[n] = (INT16)(L_SHR_D(s_lo,12));

    }

    VPP_EFR_PROFILE_FUNCTION_EXIT(Convolve);

    return;
}





















