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
#include "string.h"
#include "vpp_efr_basic_op.h"
#include "vpp_efr_sig_proc.h"

#include "vpp_efr_codec.h"
#include "vpp_efr_cnst.h"
#include "vpp_efr_basic_macro.h"

#include "vpp_efr_profile_codes.h"
#include "vpp_efr_debug.h"

/*---------------------------------------------------------------*
 *    Postfilter constant parameters (defined in "cnst.h")       *
 *---------------------------------------------------------------*
 *   L_FRAME     : Frame size.                                   *
 *   L_SUBFR     : Sub-frame size.                               *
 *   M           : LPC order.                                    *
 *   MP1         : LPC order+1                                   *
 *   MU          : Factor for tilt compensation filter           *
 *   AGC_FAC     : Factor for automatic gain control             *
 *---------------------------------------------------------------*/

#define L_H 22  /* size of truncated impulse response of A(z/g1)/A(z/g2) */

/*------------------------------------------------------------*
 *   static vectors                                           *
 *------------------------------------------------------------*/

/* inverse filtered synthesis */

static INT16 res2[L_SUBFR];

/* memory of filter 1/A(z/0.75) */

static INT16 mem_syn_pst[M];

/* Spectral expansion factors */

const INT16 F_gamma3[M] =
{
    22938, 16057, 11240, 7868, 5508,
    3856, 2699, 1889, 1322, 925
};
const INT16 F_gamma4[M] =
{
    24576, 18432, 13824, 10368, 7776,
    5832, 4374, 3281, 2461, 1846
};

/*************************************************************************
 *
 *  FUNCTION:   Init_Post_Filter
 *
 *  PURPOSE: Initializes the postfilter parameters.
 *
 *************************************************************************/

void Init_Post_Filter (void)
{
    //Set_zero (mem_syn_pst, M);
    memset ((INT8*)mem_syn_pst, 0, M<<1);
    //Set_zero (res2, L_SUBFR);
    memset ((INT8*)res2, 0, L_SUBFR<<1);
    return;
}

/*************************************************************************
 *  FUNCTION:  Post_Filter()
 *
 *  PURPOSE:  postfiltering of synthesis speech.
 *
 *  DESCRIPTION:
 *      The postfiltering process is described as follows:
 *
 *          - inverse filtering of syn[] through A(z/0.7) to get res2[]
 *          - tilt compensation filtering; 1 - MU*k*z^-1
 *          - synthesis filtering through 1/A(z/0.75)
 *          - adaptive gain control
 *
 *************************************************************************/

void Post_Filter (
    INT16 *syn,    /* in/out: synthesis speech (postfiltered is output)    */
    INT16 *Az_4    /* input: interpolated LPC parameters in all subframes  */
)
{
    /*-------------------------------------------------------------------*
     *           Declaration of parameters                               *
     *-------------------------------------------------------------------*/

    INT16 syn_pst[L_FRAME];    /* post filtered synthesis speech   */
    INT16 Ap3[MP1], Ap4[MP1];  /* bandwidth expanded LP parameters */
    INT16 *Az;                 /* pointer to Az_4:                 */
    /*  LPC parameters in each subframe */
    INT16 i_subfr;             /* index for beginning of subframe  */
    INT16 h[L_H];

    INT16 i;
    INT16 temp1, temp2;
    //INT32 L_tmp;
    register INT32 tmp_hi=0;
    register UINT32 tmp_lo=0;

    VPP_EFR_PROFILE_FUNCTION_ENTER(Post_Filter);


    /*-----------------------------------------------------*
     * Post filtering                                      *
     *-----------------------------------------------------*/

    Az = Az_4;

    for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
    {
        /* Find weighted filter coefficients Ap3[] and ap[4] */

        Weight_Ai (Az, F_gamma3, Ap3);
        Weight_Ai (Az, F_gamma4, Ap4);

        /* filtering of synthesis speech by A(z/0.7) to find res2[] */

        Residu (Ap3, &syn[i_subfr], res2, L_SUBFR);

        /* tilt compensation filter */

        /* impulse response of A(z/0.7)/A(z/0.75) */

        Copy (Ap3, h, M + 1);

        //Set_zero (&h[M + 1], L_H - M - 1);
        memset ((INT8*)&h[M + 1], 0, (L_H - M - 1)<<1);

        Syn_filt (Ap4, h, h, L_H, &h[M + 1], 0);

        /* 1st correlation of h[] */


        //L_tmp = L_MULT(h[0], h[0]);
        VPP_MLX16(tmp_hi,tmp_lo, h[0], h[0]);

        for (i = 1; i < L_H; i++)
        {

            //L_tmp = L_MAC(L_tmp, h[i], h[i]);
            VPP_MLA16(tmp_hi,tmp_lo, h[i], h[i]);

        }
        //temp1 = extract_h (L_tmp);
        //temp1 = EXTRACT_H(VPP_SCALE64_TO_16(tmp_hi,tmp_lo));
        temp1 = L_SHR_D((INT32)tmp_lo, 15);


        //L_tmp = L_MULT(h[0], h[1]);
        VPP_MLX16(tmp_hi,tmp_lo, h[0], h[1]);
        for (i = 1; i < L_H - 1; i++)
        {

            //L_tmp = L_MAC(L_tmp, h[i], h[i + 1]);
            VPP_MLA16(tmp_hi,tmp_lo, h[i], h[i + 1]);
        }
        //temp2 = extract_h (L_tmp);
        //temp2 = EXTRACT_H(VPP_SCALE64_TO_16(tmp_hi,tmp_lo));
        temp2 = L_SHR_D((INT32)tmp_lo, 15);


        if (temp2 <= 0)
        {
            temp2 = 0;
        }
        else
        {
            //temp2 = mult (temp2, MU);
            temp2 = MULT(temp2, MU);
            temp2 = div_s (temp2, temp1);
        }

        preemphasis (res2, temp2, L_SUBFR);

        /* filtering through  1/A(z/0.75) */

        Syn_filt (Ap4, res2, &syn_pst[i_subfr], L_SUBFR, mem_syn_pst, 1);

        /* scale output to input */

        agc (&syn[i_subfr], &syn_pst[i_subfr], AGC_FAC, L_SUBFR);

        Az += MP1;
    }

    /* update syn[] buffer */

    Copy (&syn[L_FRAME - M], &syn[-M], M);
    /* overwrite synthesis speech by postfiltered synthesis speech */

    Copy (syn_pst, syn, L_FRAME);

    VPP_EFR_PROFILE_FUNCTION_EXIT(Post_Filter);
    return;
}
