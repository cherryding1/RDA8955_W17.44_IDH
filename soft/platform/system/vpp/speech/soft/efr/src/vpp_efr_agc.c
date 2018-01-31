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
#include "vpp_efr_sig_proc.h"
#include "vpp_efr_cnst.h"

//For Profiling
#include "vpp_efr_debug.h"
#include "vpp_efr_profile_codes.h"

INT16 past_gain;               /* initial value of past_gain = 1.0  */

void agc (
    INT16 *sig_in,             /* (i)     : postfilter input signal  */
    INT16 *sig_out,            /* (i/o)   : postfilter output signal */
    INT16 agc_fac,             /* (i)     : AGC factor               */
    INT16 l_trm                /* (i)     : subframe size            */
)
{
    INT16 i, exp;
    INT16 gain_in, gain_out, g0, gain;
    INT32 s;
    INT16 temp;
    register INT32 s_hi=0;
    register UINT32 s_lo=0;


    VPP_EFR_PROFILE_FUNCTION_ENTER(agc);

    /* calculate gain_out with exponent */

    //temp = shr (sig_out[0], 2);
    temp = SHR_D(sig_out[0], 2);

    //s = L_MULT (temp, temp);
    VPP_MLX16(s_hi, s_lo, temp, temp);

    for (i = 1; i < l_trm; i++)
    {
        //temp = shr (sig_out[i], 2);
        temp = SHR_D(sig_out[i], 2);
        //s = L_mac (s, temp, temp);
        //s = L_MAC(s, temp, temp);
        VPP_MLA16 (s_hi, s_lo, temp, temp);
    }

    s = VPP_SCALE64_TO_16(s_hi, s_lo);

    if (s == 0)
    {
        past_gain = 0;

        VPP_EFR_PROFILE_FUNCTION_EXIT(agc);
        return;
    }

    //exp = sub (norm_l (s), 1);
    exp = SUB (norm_l (s), 1);

    //gain_out= round(l_shl (s, exp));
    gain_out= ROUND(L_SHL(s, exp));

    /* calculate gain_in with exponent */

    //temp = shr (sig_in[0], 2);
    temp = SHR_D (sig_in[0], 2);

    //s = L_mult (temp, temp);
    VPP_MLX16(s_hi, s_lo, temp, temp);

    for (i = 1; i < l_trm; i++)
    {
        //temp = shr (sig_in[i], 2);
        temp = SHR_D(sig_in[i], 2);
        //s = L_mac (s, temp, temp);
        //s = L_MAC(s, temp, temp);
        VPP_MLA16 (s_hi, s_lo, temp, temp);
    }

    s = VPP_SCALE64_TO_16(s_hi, s_lo);

    if (s == 0)
    {
        g0 = 0;
    }
    else
    {

        i = norm_l (s);

        //gain_in = round (l_shl (s, i));
        gain_in = ROUND(L_SHL(s, i));

        //exp = sub (exp, i);
        exp = SUB (exp, i);

        /*---------------------------------------------------*
         *  g0 = (1-agc_fac) * sqrt(gain_in/gain_out);       *
         *---------------------------------------------------*/

        //s = L_deposit_l (div_s (gain_out, gain_in));
        s = L_DEPOSIT_L(div_s (gain_out, gain_in));

        //s = L_shl (s, 7);       /* s = gain_out / gain_in */
        s = L_SHL_SAT(s, 7);

        //s = L_shr (s, exp);     /* add exponent */
        s = L_SHR_V(s, exp);

        s = Inv_sqrt (s);

        //i = round (L_shl (s, 9));
        i = ROUND(L_SHL_SAT(s, 9));


        /* g0 = i * (1-agc_fac) */
        //g0 = mult (i, sub (32767, agc_fac));
        g0 = MULT(i, SUB (32767, agc_fac));

    }

    /* compute gain[n] = agc_fac * gain[n-1]
                        + (1-agc_fac) * sqrt(gain_in/gain_out) */
    /* sig_out[n] = gain[n] * sig_out[n]                        */

    gain = past_gain;

    for (i = 0; i < l_trm; i++)
    {
        //gain = mult (gain, agc_fac);
        gain = MULT(gain, agc_fac);

        //gain = add (gain, g0);
        gain = ADD(gain, g0);

        //sig_out[i] = extract_h (L_shl (L_mult (sig_out[i], gain), 3));
        sig_out[i] = EXTRACT_H (L_SHL_SAT(L_MULT (sig_out[i], gain), 3));

    }

    past_gain = gain;

    VPP_EFR_PROFILE_FUNCTION_EXIT(agc);

    return;
}

void agc2 (
    INT16 *sig_in,        /* (i)     : postfilter input signal  */
    INT16 *sig_out,       /* (i/o)   : postfilter output signal */
    INT16 l_trm           /* (i)     : subframe size            */
)
{
    INT16 i, exp;
    INT16 gain_in, gain_out, g0;
    INT32 s;
    INT16 temp;
    register INT32 s_hi=0;
    register UINT32 s_lo=0;

    VPP_EFR_PROFILE_FUNCTION_ENTER(agc2);
    /* calculate gain_out with exponent */

    //temp = shr (sig_out[0], 2);
    temp = SHR_D(sig_out[0], 2);
    //s = L_mult (temp, temp);
    //s = L_MULT(temp, temp);
    VPP_MLX16(s_hi, s_lo, temp, temp);

    for (i = 1; i < l_trm; i++)
    {
        //temp = shr (sig_out[i], 2);
        temp = SHR_D(sig_out[i], 2);
        //s = L_mac (s, temp, temp);
        //   s = L_MAC(s, temp, temp);
        VPP_MLA16 (s_hi, s_lo, temp, temp);
    }

    s = VPP_SCALE64_TO_16(s_hi, s_lo);

    if (s == 0)
    {
        VPP_EFR_PROFILE_FUNCTION_EXIT(agc2);
        return;
    }

    //exp = sub (norm_l (s), 1);
    exp = SUB (norm_l (s), 1);

    //gain_out = round (L_shl (s, exp));
    gain_out = ROUND (L_SHL(s, exp));

    /* calculate gain_in with exponent */

    //temp = shr (sig_in[0], 2);
    temp = SHR_D(sig_in[0], 2);
    //s = L_mult (temp, temp);
    VPP_MLX16(s_hi, s_lo, temp, temp);

    for (i = 1; i < l_trm; i++)
    {
        //temp = shr (sig_in[i], 2);
        temp = SHR_D(sig_in[i], 2);
        //s = L_mac (s, temp, temp);
        //s = L_MAC(s, temp, temp);
        VPP_MLA16 (s_hi, s_lo, temp, temp);
    }

    s = VPP_SCALE64_TO_16(s_hi, s_lo);

    if (s == 0)
    {
        g0 = 0;
    }
    else
    {
        i = norm_l (s);
        //gain_in = round (L_shl (s, i));
        gain_in = ROUND(L_SHL(s, i));

        //exp = sub (exp, i);
        exp = SUB (exp, i);

        /*---------------------------------------------------*
         *  g0 = sqrt(gain_in/gain_out);                     *
         *---------------------------------------------------*/

        //s = L_deposit_l (div_s (gain_out, gain_in));
        s = L_DEPOSIT_L(div_s (gain_out, gain_in));

        //s = L_shl (s, 7);       /* s = gain_out / gain_in */
        s = L_SHL_SAT(s, 7);

        //s = L_shr (s, exp);     /* add exponent */
        s = L_SHR_V(s, exp);

        s = Inv_sqrt (s);
        //g0 = round (L_shl (s, 9));
        g0 = ROUND(L_SHL_SAT(s, 9));
    }

    /* sig_out(n) = gain(n) sig_out(n) */

    for (i = 0; i < l_trm; i++)
    {
        //sig_out[i] = extract_h (L_shl (L_mult (sig_out[i], g0), 3));
        sig_out[i] = EXTRACT_H(L_SHL_SAT(L_MULT(sig_out[i], g0), 3));
    }

    VPP_EFR_PROFILE_FUNCTION_EXIT(agc2);

    return;
}































































