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

#include "vpp_efr_sig_proc.h"
#include "vpp_efr_basic_macro.h"
#include <stdio.h>

#include "vpp_efr_cnst.h"
#include "vpp_efr_dtx.h"

#include "vpp_efr_profile_codes.h"
#include "vpp_efr_debug.h"

/* Locals functions */

void Lsf_wt (
    INT16 *lsf,       /* input : LSF vector                    */
    INT16 *wf2       /* output: square of weighting factors   */
);

INT16 Vq_subvec (     /* output: return quantization index     */
    INT16 *lsf_r1,    /* input : 1st LSF residual vector       */
    INT16 *lsf_r2,    /* input : and LSF residual vector       */
    const INT16 *dico,/* input : quantization codebook         */
    INT16 *wf1,       /* input : 1st LSF weighting factors     */
    INT16 *wf2,       /* input : 2nd LSF weighting factors     */
    INT16 dico_size   /* input : size of quantization codebook */
);
INT16 Vq_subvec_s (   /* output: return quantization index     */
    INT16 *lsf_r1,    /* input : 1st LSF residual vector       */
    INT16 *lsf_r2,    /* input : and LSF residual vector       */
    const INT16 *dico,/* input : quantization codebook         */
    INT16 *wf1,       /* input : 1st LSF weighting factors     */
    INT16 *wf2,       /* input : 2nd LSF weighting factors     */
    INT16 dico_size   /* input : size of quantization codebook */
);
/* M  ->order of linear prediction filter                      */
/* LSF_GAP  -> Minimum distance between LSF after quantization */
/*             50 Hz = 205                                     */
/* PRED_FAC -> Predcition factor                               */

#define M         10
#define LSF_GAP   205
#define PRED_FAC  21299

#include "vpp_efr_q_plsf_5.tab"         /* Codebooks of LSF prediction residual */

/* Past quantized prediction error */

INT16 past_r2_q[M];

extern INT16 lsf_old_tx[DTX_HANGOVER][M];

void Q_plsf_5 (
    INT16 *lsp1,      /* input : 1st LSP vector                     */
    INT16 *lsp2,      /* input : 2nd LSP vector                     */
    INT16 *lsp1_q,    /* output: quantized 1st LSP vector           */
    INT16 *lsp2_q,    /* output: quantized 2nd LSP vector           */
    INT16 *indice,    /* output: quantization indices of 5 matrices */
    INT16 txdtx_ctrl  /* input : tx dtx control word                */
)
{
    INT16 i;
    INT16 lsf1[M], lsf2[M], wf1[M], wf2[M], lsf_p[M], lsf_r1[M], lsf_r2[M];
    INT16 lsf1_q[M], lsf2_q[M];
    INT16 lsf_aver[M];
    static INT16 lsf_p_CN[M];

    VPP_EFR_PROFILE_FUNCTION_ENTER(Q_plsf_5);
    /* convert LSFs to normalize frequency domain 0..16384  */

    Lsp_lsf (lsp1, lsf1, M);
    Lsp_lsf (lsp2, lsf2, M);

    /* Update LSF CN quantizer "memory" */


    if ((txdtx_ctrl & TX_SP_FLAG) == 0
            && (txdtx_ctrl & TX_PREV_HANGOVER_ACTIVE) != 0)
    {
        update_lsf_p_CN (lsf_old_tx, lsf_p_CN);
    }

    if ((txdtx_ctrl & TX_SID_UPDATE) != 0)
    {
        /* New SID frame is to be sent:
        Compute average of the current LSFs and the LSFs in the history */

        aver_lsf_history (lsf_old_tx, lsf1, lsf2, lsf_aver);
    }
    /* Update LSF history with unquantized LSFs when no speech activity
    is present */


    if ((txdtx_ctrl & TX_SP_FLAG) == 0)
    {
        update_lsf_history (lsf1, lsf2, lsf_old_tx);
    }

    if ((txdtx_ctrl & TX_SID_UPDATE) != 0)
    {
        /* Compute LSF weighting factors for lsf2, using averaged LSFs */
        /* Set LSF weighting factors for lsf1 to zero */
        /* Replace lsf1 and lsf2 by the averaged LSFs */

        Lsf_wt (lsf_aver, wf2);
        for (i = 0; i < M; i++)
        {
            wf1[i] = 0;
            lsf1[i] = lsf_aver[i];
            lsf2[i] = lsf_aver[i];
        }
    }
    else
    {
        /* Compute LSF weighting factors */

        Lsf_wt (lsf1, wf1);
        Lsf_wt (lsf2, wf2);
    }

    /* Compute predicted LSF and prediction error */


    if ((txdtx_ctrl & TX_SP_FLAG) != 0)
    {
        for (i = 0; i < M; i++)
        {
            //lsf_p[i] = add (mean_lsf[i], mult (past_r2_q[i], PRED_FAC));
            lsf_p[i] = ADD (mean_lsf[i], MULT(past_r2_q[i], PRED_FAC));

            //lsf_r1[i] = sub (lsf1[i], lsf_p[i]);
            lsf_r1[i] = SUB (lsf1[i], lsf_p[i]);

            //lsf_r2[i] = sub (lsf2[i], lsf_p[i]);
            lsf_r2[i] = SUB (lsf2[i], lsf_p[i]);

        }
    }
    else
    {
        for (i = 0; i < M; i++)
        {
            //lsf_r1[i] = sub (lsf1[i], lsf_p_CN[i]);
            lsf_r1[i] = SUB (lsf1[i], lsf_p_CN[i]);

            //lsf_r2[i] = sub (lsf2[i], lsf_p_CN[i]);
            lsf_r2[i] = SUB (lsf2[i], lsf_p_CN[i]);

        }
    }

    /*---- Split-VQ of prediction error ----*/

    indice[0] = Vq_subvec (&lsf_r1[0], &lsf_r2[0], dico1_lsf,
                           &wf1[0], &wf2[0], DICO1_SIZE);


    indice[1] = Vq_subvec (&lsf_r1[2], &lsf_r2[2], dico2_lsf,
                           &wf1[2], &wf2[2], DICO2_SIZE);


    indice[2] = Vq_subvec_s (&lsf_r1[4], &lsf_r2[4], dico3_lsf,
                             &wf1[4], &wf2[4], DICO3_SIZE);


    indice[3] = Vq_subvec (&lsf_r1[6], &lsf_r2[6], dico4_lsf,
                           &wf1[6], &wf2[6], DICO4_SIZE);


    indice[4] = Vq_subvec (&lsf_r1[8], &lsf_r2[8], dico5_lsf,
                           &wf1[8], &wf2[8], DICO5_SIZE);


    /* Compute quantized LSFs and update the past quantized residual */
    /* In case of no speech activity, skip computing the quantized LSFs,
       and set past_r2_q to zero (initial value) */


    if ((txdtx_ctrl & TX_SP_FLAG) != 0)
    {
        for (i = 0; i < M; i++)
        {
            //lsf1_q[i] = add (lsf_r1[i], lsf_p[i]);
            lsf1_q[i] = ADD (lsf_r1[i], lsf_p[i]);

            //lsf2_q[i] = add (lsf_r2[i], lsf_p[i]);
            lsf2_q[i] = ADD (lsf_r2[i], lsf_p[i]);

            past_r2_q[i] = lsf_r2[i];
        }

        /* verification that LSFs has minimum distance of LSF_GAP */

        Reorder_lsf (lsf1_q, LSF_GAP, M);
        Reorder_lsf (lsf2_q, LSF_GAP, M);

        /* Update LSF history with quantized LSFs
        when hangover period is active */


        if ((txdtx_ctrl & TX_HANGOVER_ACTIVE) != 0)
        {
            update_lsf_history (lsf1_q, lsf2_q, lsf_old_tx);
        }
        /*  convert LSFs to the cosine domain */

        Lsf_lsp (lsf1_q, lsp1_q, M);
        Lsf_lsp (lsf2_q, lsp2_q, M);
    }
    else
    {
        for (i = 0; i < M; i++)
        {
            past_r2_q[i] = 0;
        }
    }
    VPP_EFR_PROFILE_FUNCTION_EXIT(Q_plsf_5);
    return;
}

/* Quantization of a 4 dimensional subvector */

INT16 Vq_subvec (      /* output: return quantization index     */
    INT16 *lsf_r1,     /* input : 1st LSF residual vector       */
    INT16 *lsf_r2,     /* input : and LSF residual vector       */
    const INT16 *dico, /* input : quantization codebook         */
    INT16 *wf1,        /* input : 1st LSF weighting factors     */
    INT16 *wf2,        /* input : 2nd LSF weighting factors     */
    INT16 dico_size    /* input : size of quantization codebook */
)
{
    INT16 i, temp, index=0;
    const INT16 *p_dico;
    INT32 dist_min, dist;
    register INT32 dist_hi=0;
    register UINT32 dist_lo=0;

    dist_min = MAX_32;
    p_dico = dico;

    for (i = 0; i < dico_size; i++)
    {
        //temp = sub (lsf_r1[0], *p_dico++);
        temp = SUB (lsf_r1[0], *p_dico++);
        //temp = mult (wf1[0], temp);
        temp = MULT(wf1[0], temp);
        //dist = L_mult (temp, temp);
        //dist = L_MULT(temp, temp);
        VPP_MLX16(dist_hi, dist_lo, temp, temp);

        //temp = sub (lsf_r1[1], *p_dico++);
        temp = SUB (lsf_r1[1], *p_dico++);
        //temp = mult (wf1[1], temp);
        temp = MULT(wf1[1], temp);
        //dist = L_mac (dist, temp, temp);
        //dist = L_MAC(dist, temp, temp);
        VPP_MLA16(dist_hi, dist_lo, temp, temp);

        //temp = sub (lsf_r2[0], *p_dico++);
        temp = SUB (lsf_r2[0], *p_dico++);
        //temp = mult (wf2[0], temp);
        temp = MULT(wf2[0], temp);
        //dist = L_mac (dist, temp, temp);
        //dist = L_MAC(dist, temp, temp);
        VPP_MLA16(dist_hi, dist_lo, temp, temp);

        //temp = sub (lsf_r2[1], *p_dico++);
        temp = SUB (lsf_r2[1], *p_dico++);
        //temp = mult (wf2[1], temp);
        temp = MULT(wf2[1], temp);
        //dist = L_mac (dist, temp, temp);
        //dist = L_MAC(dist, temp, temp);
        VPP_MLA16(dist_hi, dist_lo, temp, temp);

        dist = VPP_SCALE64_TO_16(dist_hi, dist_lo);

        //if (L_sub (dist, dist_min) < (INT32) 0)
        if (dist< dist_min)
        {
            dist_min = dist;
            index = i;
        }
    }

    /* Reading the selected vector */

    //p_dico = &dico[shl (index, 2)];
    p_dico = &dico[SHL(index, 2)];
    lsf_r1[0] = *p_dico++;
    lsf_r1[1] = *p_dico++;
    lsf_r2[0] = *p_dico++;
    lsf_r2[1] = *p_dico++;

    return index;

}

/* Quantization of a 4 dimensional subvector with a signed codebook */

INT16 Vq_subvec_s (    /* output: return quantization index     */
    INT16 *lsf_r1,     /* input : 1st LSF residual vector       */
    INT16 *lsf_r2,     /* input : and LSF residual vector       */
    const INT16 *dico, /* input : quantization codebook         */
    INT16 *wf1,        /* input : 1st LSF weighting factors     */
    INT16 *wf2,        /* input : 2nd LSF weighting factors     */
    INT16 dico_size)   /* input : size of quantization codebook */
{
    INT16 i, temp, sign=0;
    INT16 index=0;
    const INT16 *p_dico;
    INT32 dist_min, dist;
    register INT32 dist_hi=0;
    register UINT32 dist_lo=0;

    dist_min = MAX_32;
    p_dico = dico;

    for (i = 0; i < dico_size; i++)
    {
        /* test positive */

        //temp = sub (lsf_r1[0], *p_dico++);
        temp = SUB (lsf_r1[0], *p_dico++);
        //temp = mult (wf1[0], temp);
        temp = MULT (wf1[0], temp);
        //dist = L_mult (temp, temp);
        //dist = L_MULT(temp, temp);
        VPP_MLX16(dist_hi, dist_lo, temp, temp);

        //temp = sub (lsf_r1[1], *p_dico++);
        temp = SUB (lsf_r1[1], *p_dico++);
        //temp = mult (wf1[1], temp);
        temp = MULT (wf1[1], temp);
        //dist = L_mac (dist, temp, temp);
        //dist = L_MAC(dist, temp, temp);
        VPP_MLA16(dist_hi, dist_lo, temp, temp);

        //temp = sub (lsf_r2[0], *p_dico++);
        temp = SUB (lsf_r2[0], *p_dico++);
        //temp = mult (wf2[0], temp);
        temp = MULT (wf2[0], temp);
        //dist = L_mac (dist, temp, temp);
        //dist = L_MAC(dist, temp, temp);
        VPP_MLA16(dist_hi, dist_lo, temp, temp);

        //temp = sub (lsf_r2[1], *p_dico++);
        temp = SUB (lsf_r2[1], *p_dico++);
        //temp = mult (wf2[1], temp);
        temp = MULT(wf2[1], temp);
        //dist = L_mac (dist, temp, temp);
        //dist = L_MAC(dist, temp, temp);
        VPP_MLA16(dist_hi, dist_lo, temp, temp);

        dist = VPP_SCALE64_TO_16(dist_hi, dist_lo);

        //if (L_sub (dist, dist_min) < (INT32) 0)
        if (L_SUB(dist, dist_min)< (INT32) 0)
        {
            dist_min = dist;
            index = i;
            sign = 0;
        }
        /* test negative */

        p_dico -= 4;
        //temp = add (lsf_r1[0], *p_dico++);
        temp = ADD (lsf_r1[0], *p_dico++);
        //temp = mult (wf1[0], temp);
        temp = MULT(wf1[0], temp);
        //dist = L_mult (temp, temp);
        //dist = L_MULT(temp, temp);
        VPP_MLX16(dist_hi, dist_lo, temp, temp);

        //temp = add (lsf_r1[1], *p_dico++);
        temp = ADD (lsf_r1[1], *p_dico++);
        //temp = mult (wf1[1], temp);
        temp = MULT(wf1[1], temp);
        //dist = L_mac (dist, temp, temp);
        //dist = L_MAC(dist, temp, temp);
        VPP_MLA16(dist_hi, dist_lo, temp, temp);

        //temp = add (lsf_r2[0], *p_dico++);
        temp = ADD (lsf_r2[0], *p_dico++);
        //temp = mult (wf2[0], temp);
        temp = MULT(wf2[0], temp);
        //dist = L_mac (dist, temp, temp);
        //dist = L_MAC(dist, temp, temp);
        VPP_MLA16(dist_hi, dist_lo, temp, temp);

        //temp = add (lsf_r2[1], *p_dico++);
        temp = ADD (lsf_r2[1], *p_dico++);
        //temp = mult (wf2[1], temp);
        temp = MULT(wf2[1], temp);
        //dist = L_mac (dist, temp, temp);
        //dist = L_MAC(dist, temp, temp);
        VPP_MLA16(dist_hi, dist_lo, temp, temp);

        dist = VPP_SCALE64_TO_16(dist_hi, dist_lo);

        //if (L_sub (dist, dist_min) < (INT32) 0)
        if (L_SUB(dist, dist_min) < (INT32) 0)
        {
            dist_min = dist;
            index = i;
            sign = 1;
        }
    }

    /* Reading the selected vector */

    //p_dico = &dico[shl(index, 2)];
    p_dico = &dico[SHL(index, 2)];

    if (sign == 0)
    {
        lsf_r1[0] = *p_dico++;
        lsf_r1[1] = *p_dico++;
        lsf_r2[0] = *p_dico++;
        lsf_r2[1] = *p_dico++;
    }
    else
    {

        //lsf_r1[0] = negate (*p_dico++);
        lsf_r1[0] = NEGATE(*p_dico);
        *p_dico++;

        //lsf_r1[1] = negate (*p_dico++);
        lsf_r1[1] = NEGATE(*p_dico);
        *p_dico++;

        //lsf_r2[0] = negate (*p_dico++);
        lsf_r2[0] = NEGATE(*p_dico);
        *p_dico++;

        //lsf_r2[1] = negate (*p_dico++);
        lsf_r2[1] = NEGATE(*p_dico);
        *p_dico++;
    }

    //index = shl (index, 1);
    index = SHL(index, 1);

    //index = add (index, sign);
    index = ADD (index, sign);

    return index;

}

/****************************************************
 * FUNCTION  Lsf_wt                                                         *
 *                                                                          *
 ****************************************************
 * Compute LSF weighting factors                                            *
 *                                                                          *
 *  d[i] = lsf[i+1] - lsf[i-1]                                              *
 *                                                                          *
 *  The weighting factors are approximated by two line segment.             *
 *                                                                          *
 *  First segment passes by the following 2 points:                         *
 *                                                                          *
 *     d[i] = 0Hz     wf[i] = 3.347                                         *
 *     d[i] = 450Hz   wf[i] = 1.8                                           *
 *                                                                          *
 *  Second segment passes by the following 2 points:                        *
 *                                                                          *
 *     d[i] = 450Hz   wf[i] = 1.8                                           *
 *     d[i] = 1500Hz  wf[i] = 1.0                                           *
 *                                                                          *
 *  if( d[i] < 450Hz )                                                      *
 *    wf[i] = 3.347 - ( (3.347-1.8) / (450-0)) *  d[i]                      *
 *  else                                                                    *
 *    wf[i] = 1.8 - ( (1.8-1.0) / (1500-450)) *  (d[i] - 450)               *
 *                                                                          *
 *                                                                          *
 *  if( d[i] < 1843)                                                        *
 *    wf[i] = 3427 - (28160*d[i])>>15                                       *
 *  else                                                                    *
 *    wf[i] = 1843 - (6242*(d[i]-1843))>>15                                 *
 *                                                                          *
 *--------------------------------------------------------------------------*/

void Lsf_wt (
    INT16 *lsf,         /* input : LSF vector                  */
    INT16 *wf)          /* output: square of weighting factors */
{
    INT16 temp;
    INT16 i;
    INT32 hi=0;
    UINT32 lo =0;

    /* wf[0] = lsf[1] - 0  */
    wf[0] = lsf[1];
    // for (i = 1; i < 9; i++)
    // {
    //wf[i] = sub (lsf[i + 1], lsf[i - 1]);
    wf[1] = SUB (lsf[2], lsf[0]);
    wf[2] = SUB (lsf[3], lsf[1]);
    wf[3] = SUB (lsf[4], lsf[2]);
    wf[4] = SUB (lsf[5], lsf[3]);
    wf[5] = SUB (lsf[6], lsf[4]);
    wf[6] = SUB (lsf[7], lsf[5]);
    wf[7] = SUB (lsf[8], lsf[6]);
    wf[8] = SUB (lsf[9], lsf[7]);


    //}
    /* wf[9] = 0.5 - lsf[8] */
    //wf[9] = sub (16384, lsf[8]);
    wf[9] = SUB (16384, lsf[8]);


    for (i = 0; i < 10; i++)
    {
        //temp = sub (wf[i], 1843);
        temp = SUB (wf[i], 1843);

        if (temp < 0)
        {
            //wf[i] = sub (3427, mult (wf[i], 28160));
            lo = 0x06B18000;
            VPP_MLA16 (hi, lo, wf[i], -28160);
            //wf[i] = SUB (3427, MULT(wf[i], 28160));


        }
        else
        {
            //wf[i] = sub (1843, mult (temp, 6242));
            //wf[i] = SUB (1843, MULT(temp, 6242));
            lo = 0x03998000;
            VPP_MLA16 (hi, lo,temp, -6242);

        }

        //wf[i] = shl (wf[i], 3);
        //wf[i] = SHL(wf[i], 3);
        wf[i]= ((INT16)(SHR_D((INT32)lo, 12)& 0xfffffff8)+0x8);
    }
    return;
}





















