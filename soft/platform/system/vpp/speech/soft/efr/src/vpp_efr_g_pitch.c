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

INT16 G_pitch (        /* (o)   : Gain of pitch lag saturated to 1.2      */
    INT16 xn[],        /* (i)   : Pitch target.                           */
    INT16 y1[],        /* (i)   : Filtered adaptive codebook.             */
    INT16 L_subfr      /*       : Length of subframe.                     */
)
{
    INT16 i;
    INT16 xy, yy, exp_xy, exp_yy, gain;
    INT32 s;
    INT16 scaled_y1[80];       /* Usually dynamic allocation of (L_subfr) */

    register INT32 s_hi=0;
    register UINT32 s_lo=0;

    VPP_EFR_PROFILE_FUNCTION_ENTER(G_pitch);

    /* divide by 2 "y1[]" to avoid overflow */

    for (i = 0; i < L_subfr; i++)
    {
        //scaled_y1[i] = shr (y1[i], 2);
        scaled_y1[i] =SHR_D(y1[i], 2);

    }

    /* Compute scalar product <y1[],y1[]> */

    //s = 0L;                             /* Avoid case of all zeros */
    s_lo = 0;
    for (i = 0; i < L_subfr; i++)
    {
        //s = L_mac (s, y1[i], y1[i]);
        //s = L_MAC(s, y1[i], y1[i]);
        VPP_MLA16(s_hi, s_lo, y1[i], y1[i]);
    }
    s = VPP_SCALE64_TO_16(s_hi, s_lo);

    if (L_SUB(s, MAX_32)!= 0L)       /* Test for overflow */
    {
        //s = L_add (s, 1L);             /* Avoid case of all zeros */
        s = L_ADD(s, 1L);
        exp_yy = norm_l (s);
        //yy = ROUND(L_shl (s, exp_yy));
        yy = ROUND(L_SHL(s, exp_yy));
    }
    else
    {
        //s = 1L;                         /* Avoid case of all zeros */
        s_lo = 0; //(1>>1)
        for (i = 0; i < L_subfr; i++)
        {
            //s = L_mac (s, scaled_y1[i], scaled_y1[i]);
            //s = L_MAC(s, scaled_y1[i], scaled_y1[i]);
            VPP_MLA16(s_hi, s_lo, scaled_y1[i], scaled_y1[i]);
        }
        s = VPP_SCALE64_TO_16(s_hi, s_lo);

        exp_yy = norm_l (s);

        // yy = ROUND(L_shl (s, exp_yy));
        yy = ROUND(L_SHL(s, exp_yy));

        //exp_yy = sub (exp_yy, 4);
        exp_yy = SUB (exp_yy, 4);
    }

    /* Compute scalar product <xn[],y1[]> */

    Overflow = 0;
    s = 1L;                             /* Avoid case of all zeros */
    for (i = 0; i < L_subfr; i++)
    {
        Carry = 0;
        s = L_macNs (s, xn[i], y1[i]);


        if (Overflow != 0)
        {
            break;
        }
    }

    if (Overflow == 0)
    {
        exp_xy = norm_l (s);
        //xy  = ROUND(L_SHL (s, exp_xy));
        xy  = ROUND(L_SHL(s, exp_xy));

    }
    else
    {
        //s = 1L;                         /* Avoid case of all zeros */
        s_lo = 0;
        for (i = 0; i < L_subfr; i++)
        {
            //s = L_mac (s, xn[i], scaled_y1[i]);
            //s = L_MAC (s, xn[i], scaled_y1[i]);
            VPP_MLA16(s_hi, s_lo, xn[i], scaled_y1[i]);
        }

        s = VPP_SCALE64_TO_16(s_hi, s_lo);
        exp_xy = norm_l (s);

        // xy = round(L_shl (s, exp_xy));
        xy = ROUND(L_SHL(s, exp_xy));

        //exp_xy = sub (exp_xy, 2);
        exp_xy = SUB (exp_xy, 2);
    }

    /* If (xy < 4) gain = 0 */

    //i = sub (xy, 4);
    i = SUB (xy, 4);


    if (i < 0)
    {
        VPP_EFR_PROFILE_FUNCTION_EXIT(G_pitch);
        return ((INT16) 0);
    }

    /* compute gain = xy/yy */

    //xy = shr (xy, 1);                  /* Be sure xy < yy */

    xy = SHR_D(xy, 1);



    gain = div_s (xy, yy);

    //i = add (exp_xy, 3 - 1);           /* Denormalization of division */
    i = ADD (exp_xy, 3 - 1);

    //i = sub (i, exp_yy);
    i = SUB (i, exp_yy);

    //gain = shr (gain, i);
    gain = SHR_V(gain, i);

    /* if(gain >1.2) gain = 1.2 */


    //if (sub (gain, 4915) > 0)
    if (SUB (gain, 4915) > 0)
    {
        gain = 4915;
    }

    VPP_EFR_PROFILE_FUNCTION_EXIT(G_pitch);
    return (gain);
}










































