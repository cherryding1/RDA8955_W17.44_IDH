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

#include "vpp_efr_debug.h"
#include "vpp_efr_profile_codes.h"

#include "vpp_efr_q_plsf_5.tab"         /* Codebooks of LSF prediction residual */

#include "vpp_efr_cnst.h"
#include "vpp_efr_dtx.h"

/* M  ->order of linear prediction filter                      */
/* LSF_GAP  -> Minimum distance between LSF after quantization */
/*             50 Hz = 205                                     */
/* PRED_FAC -> Prediction factor = 0.65                        */
/* ALPHA    ->  0.9                                            */
/* ONE_ALPHA-> (1.0-ALPHA)                                     */

#define M         10
#define LSF_GAP   205
#define PRED_FAC  21299
#define ALPHA     31128
#define ONE_ALPHA 1639

/* Past quantized prediction error */

extern INT16 past_r2_q[M];

/* Past dequantized lsfs */

INT16 past_lsf_q[M];

/* Reference LSF parameter vector (comfort noise) */

INT16 lsf_p_CN[M];

/*  LSF memories for comfort noise interpolation */

INT16 lsf_old_CN[M], lsf_new_CN[M];

/* LSF parameter buffer */

extern INT16 lsf_old_rx[DTX_HANGOVER][M];

void D_plsf_5 (
    INT16 *indice,       /* input : quantization indices of 5 submatrices */
    INT16 *lsp1_q,       /* output: quantized 1st LSP vector              */
    INT16 *lsp2_q,       /* output: quantized 2nd LSP vector              */
    INT16 bfi,           /* input : bad frame indicator (set to 1 if a bad
                                     frame is received)                    */
    INT16 rxdtx_ctrl,    /* input : RX DTX control word                   */
    INT16 rx_dtx_state   /* input : state of the comfort noise insertion
                                     period                                */
)
{
    INT16 i;
    const INT16 *p_dico;
    INT16 temp, sign;
    INT16 lsf1_r[M], lsf2_r[M];
    INT16 lsf1_q[M], lsf2_q[M];

    VPP_EFR_PROFILE_FUNCTION_ENTER(D_plsf_5);

    /* Update comfort noise LSF quantizer memory */
    if ((rxdtx_ctrl & RX_UPD_SID_QUANT_MEM) != 0)
    {
        update_lsf_p_CN (lsf_old_rx, lsf_p_CN);
    }
    /* Handle cases of comfort noise LSF decoding in which past
    valid SID frames are repeated */

    if (((rxdtx_ctrl & RX_NO_TRANSMISSION) != 0)
            || ((rxdtx_ctrl & RX_INVALID_SID_FRAME) != 0)
            || ((rxdtx_ctrl & RX_LOST_SID_FRAME) != 0))
    {

        if ((rxdtx_ctrl & RX_NO_TRANSMISSION) != 0)
        {
            /* DTX active: no transmission. Interpolate LSF values in memory */
            interpolate_CN_lsf (lsf_old_CN, lsf_new_CN, lsf2_q, rx_dtx_state);
        }
        else
        {
            /* Invalid or lost SID frame: use LSFs
               from last good SID frame */
            for (i = 0; i < M; i++)
            {
                lsf_old_CN[i] = lsf_new_CN[i];
                lsf2_q[i] = lsf_new_CN[i];
                past_r2_q[i] = 0;
            }
        }

        for (i = 0; i < M; i++)
        {
            past_lsf_q[i] = lsf2_q[i];
        }

        /*  convert LSFs to the cosine domain */
        Lsf_lsp (lsf2_q, lsp2_q, M);
        VPP_EFR_PROFILE_FUNCTION_EXIT(D_plsf_5);
        return;
    }

    if (bfi != 0)                               /* if bad frame */
    {
        /* use the past LSFs slightly shifted towards their mean */

        for (i = 0; i < M; i++)
        {
            /* lsfi_q[i] = ALPHA*past_lsf_q[i] + ONE_ALPHA*mean_lsf[i]; */

            /*lsf1_q[i] = add (mult (past_lsf_q[i], ALPHA),
                             mult (mean_lsf[i], ONE_ALPHA));*/

            lsf1_q[i] = ADD (MULT (past_lsf_q[i], ALPHA),
                             MULT (mean_lsf[i], ONE_ALPHA));


            lsf2_q[i] = lsf1_q[i];
        }

        /* estimate past quantized residual to be used in next frame */

        for (i = 0; i < M; i++)
        {
            /* temp  = mean_lsf[i] +  past_r2_q[i] * PRED_FAC; */

            //temp = add (mean_lsf[i], mult (past_r2_q[i], PRED_FAC));
            temp = ADD (mean_lsf[i], MULT (past_r2_q[i], PRED_FAC));

            //past_r2_q[i] = sub (lsf2_q[i], temp);
            past_r2_q[i] = SUB (lsf2_q[i], temp);

        }
    }
    else
        /* if good LSFs received */
    {
        /* decode prediction residuals from 5 received indices */

        //p_dico = &dico1_lsf[shl (indice[0], 2)];
        p_dico = &dico1_lsf[SHL(indice[0], 2)];

        lsf1_r[0] = *p_dico++;
        lsf1_r[1] = *p_dico++;
        lsf2_r[0] = *p_dico++;
        lsf2_r[1] = *p_dico++;

        //p_dico = &dico2_lsf[shl (indice[1], 2)];
        p_dico = &dico2_lsf[SHL(indice[1], 2)];
        lsf1_r[2] = *p_dico++;
        lsf1_r[3] = *p_dico++;
        lsf2_r[2] = *p_dico++;
        lsf2_r[3] = *p_dico++;

        sign = indice[2] & 1;
        //i = shr (indice[2], 1);
        i = SHR_D(indice[2], 1);
        //p_dico = &dico3_lsf[shl (i, 2)];
        p_dico = &dico3_lsf[SHL(i, 2)];

        if (sign == 0)
        {
            lsf1_r[4] = *p_dico++;
            lsf1_r[5] = *p_dico++;
            lsf2_r[4] = *p_dico++;
            lsf2_r[5] = *p_dico++;
        }
        else
        {
            //lsf1_r[4] = negate(*p_dico++);
            lsf1_r[4] = NEGATE(*p_dico);
            *p_dico++;
            //lsf1_r[5] = negate(*p_dico++);
            lsf1_r[5] = NEGATE(*p_dico);
            *p_dico++;

            //lsf2_r[4] = negate(*p_dico++);
            lsf2_r[4] = NEGATE(*p_dico);

            *p_dico++;
            //lsf2_r[5] = negate(*p_dico++);
            lsf2_r[5] = NEGATE(*p_dico);
            *p_dico++;

        }

        //p_dico = &dico4_lsf[shl (indice[3], 2)];
        p_dico = &dico4_lsf[SHL(indice[3], 2)];

        lsf1_r[6] = *p_dico++;
        lsf1_r[7] = *p_dico++;
        lsf2_r[6] = *p_dico++;
        lsf2_r[7] = *p_dico++;

        //p_dico = &dico5_lsf[shl (indice[4], 2)];
        p_dico = &dico5_lsf[SHL(indice[4], 2)];

        lsf1_r[8] = *p_dico++;
        lsf1_r[9] = *p_dico++;
        lsf2_r[8] = *p_dico++;
        lsf2_r[9] = *p_dico++;

        /* Compute quantized LSFs and update the past quantized residual */
        /* Use lsf_p_CN as predicted LSF vector in case of no speech
           activity */

        if ((rxdtx_ctrl & RX_SP_FLAG) != 0)
        {
            for (i = 0; i < M; i++)
            {
                //temp = add (mean_lsf[i], mult (past_r2_q[i], PRED_FAC));
                temp = ADD (mean_lsf[i], MULT (past_r2_q[i], PRED_FAC));

                //lsf1_q[i] = add (lsf1_r[i], temp);
                lsf1_q[i] = ADD (lsf1_r[i], temp);

                //lsf2_q[i] = add (lsf2_r[i], temp);
                lsf2_q[i] = ADD (lsf2_r[i], temp);

                past_r2_q[i] = lsf2_r[i];
            }
        }
        else
        {
            /* Valid SID frame */
            for (i = 0; i < M; i++)
            {
                //lsf2_q[i] = add (lsf2_r[i], lsf_p_CN[i]);
                lsf2_q[i] = ADD (lsf2_r[i], lsf_p_CN[i]);


                /* Use the dequantized values of lsf2 also for lsf1 */
                lsf1_q[i] = lsf2_q[i];

                past_r2_q[i] = 0;
            }
        }
    }

    /* verification that LSFs have minimum distance of LSF_GAP Hz */

    Reorder_lsf (lsf1_q, LSF_GAP, M);
    Reorder_lsf (lsf2_q, LSF_GAP, M);

    if ((rxdtx_ctrl & RX_FIRST_SID_UPDATE) != 0)
    {
        for (i = 0; i < M; i++)
        {
            lsf_new_CN[i] = lsf2_q[i];
        }
    }

    if ((rxdtx_ctrl & RX_CONT_SID_UPDATE) != 0)
    {
        for (i = 0; i < M; i++)
        {
            lsf_old_CN[i] = lsf_new_CN[i];
            lsf_new_CN[i] = lsf2_q[i];
        }
    }

    if ((rxdtx_ctrl & RX_SP_FLAG) != 0)
    {
        /* Update lsf history with quantized LSFs
           when speech activity is present. If the current frame is
           a bad one, update with most recent good comfort noise LSFs */

        if (bfi==0)
        {
            update_lsf_history (lsf1_q, lsf2_q, lsf_old_rx);
        }
        else
        {
            update_lsf_history (lsf_new_CN, lsf_new_CN, lsf_old_rx);
        }

        for (i = 0; i < M; i++)
        {
            lsf_old_CN[i] = lsf2_q[i];
        }
    }
    else
    {
        interpolate_CN_lsf (lsf_old_CN, lsf_new_CN, lsf2_q, rx_dtx_state);
    }

    for (i = 0; i < M; i++)
    {
        past_lsf_q[i] = lsf2_q[i];
    }

    /*  convert LSFs to the cosine domain */

    Lsf_lsp (lsf1_q, lsp1_q, M);
    Lsf_lsp (lsf2_q, lsp2_q, M);

    VPP_EFR_PROFILE_FUNCTION_EXIT(D_plsf_5);
    return;
}





















