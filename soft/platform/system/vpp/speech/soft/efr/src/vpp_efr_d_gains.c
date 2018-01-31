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

#include "vpp_efr_gains_tb.h"

#include "vpp_efr_cnst.h"
#include "vpp_efr_dtx.h"

#include "vpp_efr_debug.h"
#include "vpp_efr_profile_codes.h"

extern INT16 gain_code_old_rx[4 * DTX_HANGOVER];

/* Static variables for bad frame handling */

/* Variables used by d_gain_pitch: */
INT16 pbuf[5], past_gain_pit, prev_gp;

/* Variables used by d_gain_code: */
INT16 gbuf[5], past_gain_code, prev_gc;

/* Static variables for CNI (used by d_gain_code) */

INT16 gcode0_CN, gain_code_old_CN, gain_code_new_CN, gain_code_muting_CN;

/* Memories of gain dequantization: */

/* past quantized energies.      */
/* initialized to -14.0/constant, constant = 20*Log10(2) */

extern INT16 past_qua_en[4];

/* MA prediction coeff   */
extern INT16 pred[4];

/*************************************************************************
 *
 *  FUNCTION:   gmed5
 *
 *  PURPOSE:    calculates 5-point median.
 *
 *  DESCRIPTION:
 *
 *************************************************************************/

INT16 gmed5 (        /* out      : index of the median value (0...4) */
    INT16 ind[]      /* in       : Past gain values                  */
)
{
    INT16 i, j, ix = 0, tmp[5];
    INT16 max, tmp2[5];

    for (i = 0; i < 5; i++)
    {
        tmp2[i] = ind[i];
    }

    for (i = 0; i < 5; i++)
    {
        max = -8192;
        for (j = 0; j < 5; j++)
        {
            //if (sub (tmp2[j], max) >= 0)
            if (SUB (tmp2[j], max) >= 0)

            {
                max = tmp2[j];
                ix = j;
            }
        }
        tmp2[ix] = -16384;
        tmp[i] = ix;
    }

    return (ind[tmp[2]]);
}

/*************************************************************************
 *
 *  FUNCTION:   d_gain_pitch
 *
 *  PURPOSE:  decodes the pitch gain using the received index.
 *
 *  DESCRIPTION:
 *       In case of no frame erasure, the gain is obtained from the
 *       quantization table at the given index; otherwise, a downscaled
 *       past gain is used.
 *
 *************************************************************************/

INT16 d_gain_pitch ( /* out      : quantized pitch gain           */
    INT16 index,     /* in       : index of quantization          */
    INT16 bfi,       /* in       : bad frame indicator (good = 0) */
    INT16 state,
    INT16 prev_bf,
    INT16 rxdtx_ctrl
)
{
    static const INT16 pdown[7] =
    {
        32767, 32112, 32112, 26214,
        9830, 6553, 6553
    };

    INT16 gain, tmp, i;

    VPP_EFR_PROFILE_FUNCTION_ENTER(d_gain_pitch);

    if (bfi == 0)
    {

        if ((rxdtx_ctrl & RX_SP_FLAG) != 0)
        {
            //gain = shr (qua_gain_pitch[index], 2);
            gain = SHR_D(qua_gain_pitch[index], 2);


            if (prev_bf != 0)
            {

                //if (sub (gain, prev_gp) > 0)
                if (SUB (gain, prev_gp) > 0)
                {
                    gain = prev_gp;
                }
            }
        }
        else
        {
            gain = 0;
        }
        prev_gp = gain;
    }
    else
    {
        if ((rxdtx_ctrl & RX_SP_FLAG) != 0)
        {
            tmp = gmed5 (pbuf);


            //if (sub (tmp, past_gain_pit) < 0)
            if (SUB (tmp, past_gain_pit) < 0)
            {
                past_gain_pit = tmp;
            }
            //gain = mult (pdown[state], past_gain_pit);
            gain = MULT(pdown[state], past_gain_pit);

        }
        else
        {
            gain = 0;
        }
    }

    past_gain_pit = gain;


    //if (sub (past_gain_pit, 4096) > 0)
    if (SUB (past_gain_pit, 4096) > 0)   /* if (past_gain_pit > 1.0) */
    {
        past_gain_pit = 4096;
    }
    for (i = 1; i < 5; i++)
    {
        pbuf[i - 1] = pbuf[i];
    }

    pbuf[4] = past_gain_pit;

    VPP_EFR_PROFILE_FUNCTION_EXIT(d_gain_pitch);
    return gain;
}

/*************************************************************************
 *
 *  FUNCTION:  d_gain_code
 *
 *  PURPOSE:  decode the fixed codebook gain using the received index.
 *
 *  DESCRIPTION:
 *      The received index gives the gain correction factor gamma.
 *      The quantized gain is given by   g_q = g0 * gamma
 *      where g0 is the predicted gain.
 *      To find g0, 4th order MA prediction is applied to the mean-removed
 *      innovation energy in dB.
 *      In case of frame erasure, downscaled past gain is used.
 *
 *************************************************************************/

/* average innovation energy.                             */
/* MEAN_ENER = 36.0/constant, constant = 20*Log10(2)      */
#define MEAN_ENER  783741L      /* 36/(20*log10(2))       */

void d_gain_code (
    INT16 index,      /* input : received quantization index */
    INT16 code[],     /* input : innovation codevector       */
    INT16 lcode,      /* input : codevector length           */
    INT16 *gain_code, /* output: decoded innovation gain     */
    INT16 bfi,        /* input : bad frame indicator         */
    INT16 state,
    INT16 prev_bf,
    INT16 rxdtx_ctrl,
    INT16 i_subfr,
    INT16 rx_dtx_state
)
{
    static const INT16 cdown[7] =
    {
        32767, 32112, 32112, 32112,
        32112, 32112, 22937
    };

    INT16 i, tmp;
    INT16 gcode0, exp, frac, av_pred_en;
    INT32 ener, ener_code;

    register INT32 ener_code_hi=0;
    register UINT32 ener_code_lo=0;


    VPP_EFR_PROFILE_FUNCTION_ENTER(d_gain_code);

    if (((rxdtx_ctrl & RX_UPD_SID_QUANT_MEM) != 0) && (i_subfr == 0))
    {
        gcode0_CN = update_gcode0_CN (gain_code_old_rx);
        //gcode0_CN = shl (gcode0_CN, 4);
        gcode0_CN = SHL(gcode0_CN, 4);
    }

    /* Handle cases of comfort noise fixed codebook gain decoding in
       which past valid SID frames are repeated */


    if (((rxdtx_ctrl & RX_NO_TRANSMISSION) != 0)
            || ((rxdtx_ctrl & RX_INVALID_SID_FRAME) != 0)
            || ((rxdtx_ctrl & RX_LOST_SID_FRAME) != 0))
    {

        if ((rxdtx_ctrl & RX_NO_TRANSMISSION) != 0)
        {
            /* DTX active: no transmission. Interpolate gain values
            in memory */

            if (i_subfr == 0)
            {
                *gain_code = interpolate_CN_param (gain_code_old_CN,
                                                   gain_code_new_CN, rx_dtx_state);

            }
            else
            {
                *gain_code = prev_gc;
            }
        }
        else
        {
            /* Invalid or lost SID frame:
            use gain values from last good SID frame */
            gain_code_old_CN = gain_code_new_CN;
            *gain_code = gain_code_new_CN;

            /* reset table of past quantized energies */
            for (i = 0; i < 4; i++)
            {
                past_qua_en[i] = -2381;
            }
        }

        if ((rxdtx_ctrl & RX_DTX_MUTING) != 0)
        {
            /* attenuate the gain value by 0.75 dB in each subframe */
            /* (total of 3 dB per frame) */
            // gain_code_muting_CN = mult(gain_code_muting_CN, 30057);
            gain_code_muting_CN = MULT(gain_code_muting_CN, 30057);
            *gain_code = gain_code_muting_CN;
        }
        else
        {
            /* Prepare for DTX muting by storing last good gain value */
            gain_code_muting_CN = gain_code_new_CN;
        }

        past_gain_code = *gain_code;

        for (i = 1; i < 5; i++)
        {
            gbuf[i - 1] = gbuf[i];
        }

        gbuf[4] = past_gain_code;
        prev_gc = past_gain_code;

        VPP_EFR_PROFILE_FUNCTION_EXIT(d_gain_code);
        return;
    }

    /*----------------- Test erasure ---------------*/
    if (bfi != 0)
    {
        tmp = gmed5 (gbuf);

        //if (sub (tmp, past_gain_code) < 0)
        if (SUB (tmp, past_gain_code) < 0)
        {
            past_gain_code = tmp;
        }
        //past_gain_code = mult (past_gain_code, cdown[state]);
        past_gain_code = MULT(past_gain_code, cdown[state]);

        *gain_code = past_gain_code;

        av_pred_en = 0;
        for (i = 0; i < 4; i++)
        {
            //av_pred_en = add (av_pred_en, past_qua_en[i]);
            av_pred_en = ADD (av_pred_en, past_qua_en[i]);
        }

        /* av_pred_en = 0.25*av_pred_en - 4/(20Log10(2)) */
        // av_pred_en = mult (av_pred_en, 8192);   /*  *= 0.25  */
        av_pred_en = MULT(av_pred_en, 8192);

        /* if (av_pred_en < -14/(20Log10(2))) av_pred_en = .. */

        //if (sub (av_pred_en, -2381) < 0)
        if (SUB (av_pred_en, -2381) < 0)
        {
            av_pred_en = -2381;
        }
        for (i = 3; i > 0; i--)
        {
            past_qua_en[i] = past_qua_en[i - 1];
        }
        past_qua_en[0] = av_pred_en;
        for (i = 1; i < 5; i++)
        {
            gbuf[i - 1] = gbuf[i];
        }
        gbuf[4] = past_gain_code;

        /* Use the most recent comfort noise fixed codebook gain value
           for updating the fixed codebook gain history */
        if (gain_code_new_CN == 0)
        {
            tmp = prev_gc;
        }
        else
        {
            tmp = gain_code_new_CN;
        }

        update_gain_code_history_rx (tmp, gain_code_old_rx);

        //if (sub (i_subfr, (3 * L_SUBFR)) == 0)
        if (SUB (i_subfr, (3 * L_SUBFR)) == 0)
        {
            gain_code_old_CN = *gain_code;
        }
        VPP_EFR_PROFILE_FUNCTION_EXIT(d_gain_code);
        return;
    }

    if ((rxdtx_ctrl & RX_SP_FLAG) != 0)
    {

        /*-------------- Decode codebook gain ---------------*/

        /*-------------------------------------------------------------------*
         *  energy of code:                                                   *
         *  ~~~~~~~~~~~~~~~                                                   *
         *  ener_code = 10 * Log10(energy/lcode) / constant                   *
         *            = 1/2 * Log2(energy/lcode)                              *
         *                                           constant = 20*Log10(2)   *
         *-------------------------------------------------------------------*/

        /* ener_code = log10(ener_code/lcode) / (20*log10(2)) */
        //ener_code = 0;
        ener_code_lo = 0;
        for (i = 0; i < lcode; i++)
        {
            //ener_code = L_mac (ener_code, code[i], code[i]);
            //ener_code = L_MAC(ener_code, code[i], code[i]);
            VPP_MLA16(ener_code_hi, ener_code_lo,code[i], code[i]);
        }
        ener_code = VPP_SCALE64_TO_16(ener_code_hi, ener_code_lo);

        /* ener_code = ener_code / lcode */
        //ener_code = L_mult (round(ener_code), 26214);
        ener_code = L_MULT (ROUND(ener_code), 26214);

        /* ener_code = 1/2 * Log2(ener_code) */
        Log2 (ener_code, &exp, &frac);
        //ener_code = L_Comp (sub (exp, 30), frac);
        ener_code = L_Comp (SUB (exp, 30), frac);

        /* predicted energy */
        //ener = MEAN_ENER;
        ener_code_lo = MEAN_ENER>>1;
        ener_code_hi =0;

        for (i = 0; i < 4; i++)
        {
            //ener = L_mac (ener, past_qua_en[i], pred[i]);
            //ener = L_MAC(ener, past_qua_en[i], pred[i]);
            VPP_MLA16(ener_code_hi, ener_code_lo,past_qua_en[i], pred[i]);
        }
        ener = VPP_SCALE64_TO_16(ener_code_hi, ener_code_lo);

        /*-------------------------------------------------------------------*
         *  predicted codebook gain                                           *
         *  ~~~~~~~~~~~~~~~~~~~~~~~                                           *
         *  gcode0     = Pow10( (ener*constant - ener_code*constant) / 20 )   *
         *             = Pow2(ener-ener_code)                                 *
         *                                           constant = 20*Log10(2)   *
         *-------------------------------------------------------------------*/

        //ener = L_shr (L_sub (ener, ener_code), 1);
        ener = L_SHR_D(L_SUB(ener, ener_code), 1);

        L_Extract (ener, &exp, &frac);

        //gcode0 = extract_l(Pow2 (exp, frac));
        gcode0 = EXTRACT_L(Pow2 (exp, frac));  /* predicted gain */

        //gcode0 = shl (gcode0, 4);
        gcode0 = SHL(gcode0, 4);

        //*gain_code = mult (qua_gain_code[index], gcode0);
        *gain_code = MULT(qua_gain_code[index], gcode0);

        if (prev_bf != 0)
        {
            //if (sub (*gain_code, prev_gc) > 0)
            if (SUB (*gain_code, prev_gc) > 0)
            {
                *gain_code = prev_gc;
            }
        }
        /*-------------------------------------------------------------------*
         *  update table of past quantized energies                           *
         *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                           *
         *  past_qua_en      = 20 * Log10(qua_gain_code) / constant           *
         *                   = Log2(qua_gain_code)                            *
         *                                           constant = 20*Log10(2)   *
         *-------------------------------------------------------------------*/

        for (i = 3; i > 0; i--)
        {
            past_qua_en[i] = past_qua_en[i - 1];
        }
        //Log2 (L_deposit_l (qua_gain_code[index]), &exp, &frac);
        Log2 (L_DEPOSIT_L(qua_gain_code[index]), &exp, &frac);

        //past_qua_en[0] = shr (frac, 5);
        past_qua_en[0] = SHR_D(frac, 5);

        //past_qua_en[0] = add(past_qua_en[0], shl (sub (exp, 11), 10));
        past_qua_en[0] = ADD(past_qua_en[0], SHL(SUB (exp, 11), 10));



        update_gain_code_history_rx (*gain_code, gain_code_old_rx);

        //if (sub (i_subfr, (3 * L_SUBFR)) == 0)
        if (SUB (i_subfr, (3 * L_SUBFR)) == 0)
        {
            gain_code_old_CN = *gain_code;
        }
    }
    else
    {

        if (((rxdtx_ctrl & RX_FIRST_SID_UPDATE) != 0) && (i_subfr == 0))
        {
            //gain_code_new_CN = mult (gcode0_CN, qua_gain_code[index]);
            gain_code_new_CN = MULT(gcode0_CN, qua_gain_code[index]);

            /*---------------------------------------------------------------*
             *  reset table of past quantized energies                        *
             *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                        *
             *---------------------------------------------------------------*/

            for (i = 0; i < 4; i++)
            {
                past_qua_en[i] = -2381;
            }
        }
        if (((rxdtx_ctrl & RX_CONT_SID_UPDATE) != 0) && (i_subfr == 0))
        {
            gain_code_old_CN = gain_code_new_CN;
            //gain_code_new_CN = mult (gcode0_CN, qua_gain_code[index]);
            gain_code_new_CN = MULT(gcode0_CN, qua_gain_code[index]);

        }

        if (i_subfr == 0)
        {
            *gain_code = interpolate_CN_param (gain_code_old_CN,
                                               gain_code_new_CN,
                                               rx_dtx_state);
        }
        else
        {
            *gain_code = prev_gc;
        }
    }

    past_gain_code = *gain_code;

    for (i = 1; i < 5; i++)
    {
        gbuf[i - 1] = gbuf[i];
    }
    gbuf[4] = past_gain_code;
    prev_gc = past_gain_code;

    VPP_EFR_PROFILE_FUNCTION_EXIT(d_gain_code);

    return;
}





















