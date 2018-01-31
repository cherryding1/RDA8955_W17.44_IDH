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

void Syn_filt (
    INT16 a[],     /* (i)     : a[m+1] prediction coefficients   (m=10)  */
    INT16 x[],     /* (i)     : input signal                             */
    INT16 y[],     /* (o)     : output signal                            */
    INT16 lg,      /* (i)     : size of filtering                        */
    INT16 mem[],   /* (i/o)   : memory associated with this filtering.   */
    INT16 update   /* (i)     : 0=no update, 1=update of memory.         */
)
{
    INT16 i;
    INT16* p_x = x;
    INT16*p_a;
    //INT32 s;
    INT16 tmp[80]= {0};  /* This is usually done by memory allocation (lg+m) */
    INT16 *yy;

    register INT32 s_hi=0;
    register UINT32 s_lo=0;

    VPP_EFR_PROFILE_FUNCTION_ENTER(Syn_filt);

    /* Copy mem[] to yy[] */

    yy = tmp;

    /*for (i = 0; i < m; i++)
    {
        *yy++ = mem[i];
    }*/
    *yy++ = mem[0];
    *yy++ = mem[1];
    *yy++ = mem[2];
    *yy++ = mem[3];
    *yy++ = mem[4];
    *yy++ = mem[5];
    *yy++ = mem[6];
    *yy++ = mem[7];
    *yy++ = mem[8];
    *yy++ = mem[9];

    /* Do the filtering. */

    for (i = 0; i < lg; i++)
    {
        //s = L_mult (x[i], a[0]);
        //s = L_MULT(x[i], a[0]);
        p_a = &a[0];
        VPP_MLX16(s_hi,s_lo,(*p_x++), (*p_a++));

        //for (j = 1; j <= m; j++)
        // {
        //s = L_msu (s, a[j], yy[-j]);
        //s = L_MSU (s, a[j], yy[-j]);

        VPP_MLA16(s_hi,s_lo,(*p_a++), -yy[-1]);
        VPP_MLA16(s_hi,s_lo,(*p_a++), -yy[-2]);
        VPP_MLA16(s_hi,s_lo,(*p_a++), -yy[-3]);
        VPP_MLA16(s_hi,s_lo,(*p_a++), -yy[-4]);
        VPP_MLA16(s_hi,s_lo,(*p_a++), -yy[-5]);
        VPP_MLA16(s_hi,s_lo,(*p_a++), -yy[-6]);
        VPP_MLA16(s_hi,s_lo,(*p_a++), -yy[-7]);
        VPP_MLA16(s_hi,s_lo,(*p_a++), -yy[-8]);
        VPP_MLA16(s_hi,s_lo,(*p_a++), -yy[-9]);
        VPP_MLA16(s_hi,s_lo,(*p_a), -yy[-10]);
        // }
        //s = L_shl (s, 3);
        //s = VPP_SCALE64_TO_16(s_hi,s_lo);
        //s = L_SHL_SAT(s, 3);
        //*yy++ = ROUND(s);
        *yy++ = (INT16)(L_SHR_D(L_ADD((INT32)s_lo,0x800),12));
    }

    for (i = 0; i < lg; i++)
    {
        y[i] = tmp[i + m];
    }

    /* Update of memory if update==1 */


    if (update != 0)
    {
        // for (i = 0; i < m; i++)
        // {
        mem[0] = y[lg - m ];
        mem[1] = y[lg - m + 1];
        mem[2] = y[lg - m + 2];
        mem[3] = y[lg - m + 3];
        mem[4] = y[lg - m + 4];
        mem[5] = y[lg - m + 5];
        mem[6] = y[lg - m + 6];
        mem[7] = y[lg - m + 7];
        mem[8] = y[lg - m + 8];
        mem[9] = y[lg - m + 9];

        // }
    }
    VPP_EFR_PROFILE_FUNCTION_EXIT(Syn_filt);
    return;
}





















