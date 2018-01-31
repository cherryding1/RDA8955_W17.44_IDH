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

#include "vpp_efr_profile_codes.h"
#include "vpp_efr_debug.h"

/* past quantized energies.                               */
/* initialized to -14.0/constant, constant = 20*Log10(2)   */
INT16 past_qua_en[4];

/* MA prediction coeff */
INT16 pred[4];

extern INT16 CN_excitation_gain, gain_code_old_tx[4 * DTX_HANGOVER];

INT16 q_gain_pitch (   /* Return index of quantization */
    INT16 *gain        /* (i)  :  Pitch gain to quantize  */
)
{
    INT16 i, index, gain_q14, err, err_min;

    VPP_EFR_PROFILE_FUNCTION_ENTER(q_gain_pitch);

    //gain_q14 = shl (*gain, 2);
    gain_q14 = SHL(*gain, 2);

    //err_min = abs_s (sub (gain_q14, qua_gain_pitch[0]));
    err_min = ABS_S(SUB (gain_q14, qua_gain_pitch[0]));
    index = 0;

    for (i = 1; i < NB_QUA_PITCH; i++)
    {
        //err = abs_s (sub (gain_q14, qua_gain_pitch[i]));
        err = ABS_S(SUB (gain_q14, qua_gain_pitch[i]));


        //if (sub (err, err_min) < 0)
        if (SUB (err, err_min) < 0)
        {
            err_min = err;
            index = i;
        }
    }

    //*gain = shr (qua_gain_pitch[index], 2);
    *gain = SHR_D(qua_gain_pitch[index], 2);

    VPP_EFR_PROFILE_FUNCTION_EXIT(q_gain_pitch);

    return index;
}

/* average innovation energy.                             */
/* MEAN_ENER  = 36.0/constant, constant = 20*Log10(2)     */

#define MEAN_ENER  783741L      /* 36/(20*log10(2))       */

INT16 q_gain_code (    /* Return quantization index                  */
    INT16 code[],      /* (i)      : fixed codebook excitation       */
    INT16 lcode,       /* (i)      : codevector size                 */
    INT16 *gain,       /* (i/o)    : quantized fixed codebook gain   */
    INT16 txdtx_ctrl,
    INT16 i_subfr
)
{
    INT16 i, index=0;
    INT16 gcode0, err, err_min, exp, frac;
    INT32 ener, ener_code;
    register INT32 ener_code_hi=0;
    register UINT32 ener_code_lo=0;
    INT16 aver_gain;
    static INT16 gcode0_CN;

    VPP_EFR_PROFILE_FUNCTION_ENTER(q_gain_code);



    if ((txdtx_ctrl & TX_SP_FLAG) != 0)
    {

        /*-------------------------------------------------------------------*
         *  energy of code:                                                   *
         *  ~~~~~~~~~~~~~~~                                                   *
         *  ener_code(Q17) = 10 * Log10(energy/lcode) / constant              *
         *                 = 1/2 * Log2(energy/lcode)                         *
         *                                           constant = 20*Log10(2)   *
         *-------------------------------------------------------------------*/

        /* ener_code = log10(ener_code/lcode) / (20*log10(2))       */
        //ener_code = 0;
        ener_code_lo = 0;
        for (i = 0; i < lcode; i++)
        {
            //ener_code = L_mac (ener_code, code[i], code[i]);
            //ener_code = L_MAC(ener_code, code[i], code[i]);
            VPP_MLA16(ener_code_hi,ener_code_lo,code[i], code[i]);

        }
        /* ener_code = ener_code / lcode */
        ener_code = VPP_SCALE64_TO_16(ener_code_hi,ener_code_lo);

        //ener_code = L_mult (round(ener_code), 26214);
        ener_code = L_MULT(ROUND(ener_code), 26214);

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
            VPP_MLA16(ener_code_hi,ener_code_lo, past_qua_en[i], pred[i]);
        }

        ener = VPP_SCALE64_TO_16(ener_code_hi,ener_code_lo);

        /*-------------------------------------------------------------------*
         *  predicted codebook gain                                           *
         *  ~~~~~~~~~~~~~~~~~~~~~~~                                           *
         *  gcode0(Qx) = Pow10( (ener*constant - ener_code*constant) / 20 )   *
         *             = Pow2(ener-ener_code)                                 *
         *                                           constant = 20*Log10(2)   *
         *-------------------------------------------------------------------*/

        //ener = L_shr (L_SUB(ener, ener_code), 1);
        ener = L_SHR_D(L_SUB(ener, ener_code), 1);


        L_Extract (ener, &exp, &frac);

        //gcode0 = extract_l (Pow2 (exp, frac));  /* predicted gain */
        gcode0 = EXTRACT_L(Pow2 (exp, frac));

        //gcode0 = shl (gcode0, 4);
        gcode0 = SHL(gcode0, 4);

        /*-------------------------------------------------------------------*
         *                   Search for best quantizer                        *
         *-------------------------------------------------------------------*/

        //err_min = abs_s (sub (*gain, mult (gcode0, qua_gain_code[0])));
        err_min = ABS_S(SUB (*gain, MULT(gcode0, qua_gain_code[0])));
        index = 0;

        for (i = 1; i < NB_QUA_CODE; i++)
        {
            //err = abs_s (sub (*gain, mult (gcode0, qua_gain_code[i])));
            err = ABS_S(SUB (*gain, MULT (gcode0, qua_gain_code[i])));


            //if (sub (err, err_min) < 0)
            if (SUB (err, err_min) < 0)
            {
                err_min = err;
                index = i;
            }
        }

        //*gain = mult (gcode0, qua_gain_code[index]);
        *gain = MULT(gcode0, qua_gain_code[index]);


        /*------------------------------------------------------------------*
         *  update table of past quantized energies                         *
         *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                         *
         *  past_qua_en(Q12) = 20 * Log10(qua_gain_code) / constant         *
         *                   = Log2(qua_gain_code)                          *
         *                                           constant = 20*Log10(2) *
         *------------------------------------------------------------------*/

        for (i = 3; i > 0; i--)
        {
            past_qua_en[i] = past_qua_en[i - 1];
        }
        //Log2 (L_deposit_l (qua_gain_code[index]), &exp, &frac);
        Log2 (L_DEPOSIT_L(qua_gain_code[index]), &exp, &frac);

        //past_qua_en[0] = shr (frac, 5);
        past_qua_en[0] = SHR_D(frac, 5);

        //past_qua_en[0] = add (past_qua_en[0], shl (sub (exp, 11), 10));
        past_qua_en[0] = ADD (past_qua_en[0], SHL(SUB (exp, 11), 10));


        update_gain_code_history_tx (*gain, gain_code_old_tx);
    }
    else
    {

        if ((txdtx_ctrl & TX_PREV_HANGOVER_ACTIVE) != 0 && (i_subfr == 0))
        {
            gcode0_CN = update_gcode0_CN (gain_code_old_tx);
            //gcode0_CN = shl (gcode0_CN, 4);
            gcode0_CN = SHL(gcode0_CN, 4);
        }
        *gain = CN_excitation_gain;


        if ((txdtx_ctrl & TX_SID_UPDATE) != 0)
        {
            aver_gain = aver_gain_code_history (CN_excitation_gain,
                                                gain_code_old_tx);

            /*---------------------------------------------------------------*
             *                   Search for best quantizer                    *
             *---------------------------------------------------------------*/

            //err_min = abs_s (sub (aver_gain, mult (gcode0_CN, qua_gain_code[0])));
            err_min = ABS_S(SUB (aver_gain, MULT(gcode0_CN, qua_gain_code[0])));

            index = 0;

            for (i = 1; i < NB_QUA_CODE; i++)
            {
                //err = abs_s (sub (aver_gain,mult (gcode0_CN, qua_gain_code[i])));
                err = ABS_S(SUB (aver_gain, MULT(gcode0_CN, qua_gain_code[i])));


                //if (sub (err, err_min) < 0)
                if (SUB (err, err_min) < 0)
                {
                    err_min = err;
                    index = i;
                }
            }
        }
        update_gain_code_history_tx (*gain, gain_code_old_tx);

        /*-------------------------------------------------------------------*
         *  reset table of past quantized energies                            *
         *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                            *
         *-------------------------------------------------------------------*/

        for (i = 0; i < 4; i++)
        {
            past_qua_en[i] = -2381;
        }
    }
    VPP_EFR_PROFILE_FUNCTION_EXIT(q_gain_code);
    return index;
}










































