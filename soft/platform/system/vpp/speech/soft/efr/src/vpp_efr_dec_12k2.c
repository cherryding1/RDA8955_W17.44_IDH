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




#include "string.h"

#include "cs_types.h"
#include "vpp_efr_basic_op.h"
#include "vpp_efr_sig_proc.h"
#include "vpp_efr_codec.h"
#include "vpp_efr_cnst.h"
#include "vpp_efr_basic_macro.h"

#include "vpp_efr_debug.h"
#include "vpp_efr_profile_codes.h"

#include "vpp_efr_dtx.h"

extern INT16 dtx_mode;

/*---------------------------------------------------------------*
 *   Decoder constant parameters (defined in "cnst.h")           *
 *---------------------------------------------------------------*
 *   L_FRAME     : Frame size.                                   *
 *   L_FRAME_BY2 : Half the frame size.                          *
 *   L_SUBFR     : Sub-frame size.                               *
 *   M           : LPC order.                                    *
 *   MP1         : LPC order+1                                   *
 *   PIT_MIN     : Minimum pitch lag.                            *
 *   PIT_MAX     : Maximum pitch lag.                            *
 *   L_INTERPOL  : Length of filter for interpolation            *
 *   PRM_SIZE    : size of vector containing analysis parameters *
 *---------------------------------------------------------------*/

/*--------------------------------------------------------*
 *         Static memory allocation.                      *
 *--------------------------------------------------------*/

/* Excitation vector */

static INT16 old_exc[L_FRAME + PIT_MAX + L_INTERPOL];
static INT16 *exc;

/* Lsp (Line spectral pairs) */

static INT16 lsp_old[M];

/* Filter's memory */

static INT16 mem_syn[M];

/* Memories for bad frame handling */

static INT16 prev_bf;
static INT16 state;

/***************************************************************************
 *
 *   FUNCTION:  Init_Decoder_12k2
 *
 *   PURPOSE: Initialization of variables for the decoder section.
 *
 ***************************************************************************/

void Init_Decoder_12k2 (void)
{
    VPP_EFR_PROFILE_FUNCTION_ENTER(Init_Decoder_12k2);

    /* Initialize static pointer */

    exc = old_exc + PIT_MAX + L_INTERPOL;

    /* Static vectors to zero */

    //Set_zero (old_exc, PIT_MAX + L_INTERPOL);
    //Set_zero (mem_syn, M);
    memset ((INT8*)old_exc, 0, (PIT_MAX + L_INTERPOL)<<1);
    memset ((INT8*)mem_syn, 0,M<<1);

    /* Initialize lsp_old [] */

    lsp_old[0] = 30000;
    lsp_old[1] = 26000;
    lsp_old[2] = 21000;
    lsp_old[3] = 15000;
    lsp_old[4] = 8000;
    lsp_old[5] = 0;
    lsp_old[6] = -8000;
    lsp_old[7] = -15000;
    lsp_old[8] = -21000;
    lsp_old[9] = -26000;

    /* Initialize memories of bad frame handling */

    prev_bf = 0;
    state = 0;
    VPP_EFR_PROFILE_FUNCTION_EXIT(Init_Decoder_12k2);
    return;
}

/***************************************************************************
 *
 *   FUNCTION:  Decoder_12k2
 *
 *   PURPOSE:   Speech decoder routine.
 *
 ***************************************************************************/

void Decoder_12k2 (
    INT16 parm[], /* input : vector of synthesis parameters
                      parm[0] = bad frame indicator (bfi)       */
    INT16 synth[],/* output: synthesis speech                  */
    INT16 A_t[],  /* output: decoded LP filter in 4 subframes  */
    INT16 TAF,
    INT16 SID_flag
)
{

    /* LPC coefficients */

    INT16 *Az;                 /* Pointer on A_t */

    /* LSPs */

    INT16 lsp_new[M];
    INT16 lsp_mid[M];

    /* Algebraic codevector */

    INT16 code[L_SUBFR];

    /* excitation */

    INT16 excp[L_SUBFR];

    /* Scalars */

    INT16 i, i_subfr;
    INT16 T0, T0_frac, index;
    INT16 gain_pit, gain_code, bfi, pit_sharp;
    INT16 temp;
    INT32 L_temp;
    register INT32 tmp_hi=0;
    register UINT32 tmp_lo=0;

    extern INT16 rxdtx_ctrl, rx_dtx_state;
    extern INT32 L_pn_seed_rx;

    VPP_EFR_PROFILE_FUNCTION_ENTER(Decoder_12k2);

    /* Test bad frame indicator (bfi) */

    bfi = *parm++;

    /* Set state machine */
    if (bfi != 0)
    {
        //state = add (state, 1);
        state = ADD (state, 1);
    }
    //else if (sub (state, 6) == 0)
    else if (SUB (state, 6) == 0)
    {
        state = 5;
    }
    else
    {
        state = 0;
    }


    //if (sub (state, 6) > 0)
    if (SUB (state, 6) > 0)
    {
        state = 6;
    }
    rx_dtx (&rxdtx_ctrl, TAF, bfi, SID_flag);

    /* If this frame is the first speech frame after CNI period,     */
    /* set the BFH state machine to an appropriate state depending   */
    /* on whether there was DTX muting before start of speech or not */
    /* If there was DTX muting, the first speech frame is muted.     */
    /* If there was no DTX muting, the first speech frame is not     */
    /* muted. The BFH state machine starts from state 5, however, to */
    /* keep the audible noise resulting from a SID frame which is    */
    /* erroneously interpreted as a good speech frame as small as    */
    /* possible (the decoder output in this case is quickly muted)   */

    if ((rxdtx_ctrl & RX_FIRST_SP_FLAG) != 0)
    {

        if ((rxdtx_ctrl & RX_PREV_DTX_MUTING) != 0)
        {
            state = 5;
            prev_bf = 1;
        }
        else
        {
            state = 5;
            prev_bf = 0;
        }
    }


    D_plsf_5 (parm, lsp_mid, lsp_new, bfi, rxdtx_ctrl, rx_dtx_state);

    /* Advance synthesis parameters pointer */
    parm += 5;

    if ((rxdtx_ctrl & RX_SP_FLAG) != 0)
    {
        /* Interpolation of LPC for the 4 subframes */

        Int_lpc (lsp_old, lsp_mid, lsp_new, A_t);
    }
    else
    {
        /* Comfort noise: use the same parameters in each subframe */
        Lsp_Az (lsp_new, A_t);

        for (i = 0; i < MP1; i++)
        {
            A_t[i + MP1] = A_t[i];
            A_t[i + 2 * MP1] = A_t[i];
            A_t[i + 3 * MP1] = A_t[i];
        }
    }

    /* update the LSPs for the next frame */
    for (i = 0; i < M; i++)
    {
        lsp_old[i] = lsp_new[i];
    }

    /*---------------------------------------------------------------------*
     *       Loop for every subframe in the analysis frame                 *
     *---------------------------------------------------------------------*
     * The subframe size is L_SUBFR and the loop is repeated               *
     * L_FRAME/L_SUBFR times                                               *
     *     - decode the pitch delay                                        *
     *     - decode algebraic code                                         *
     *     - decode pitch and codebook gains                               *
     *     - find the excitation and compute synthesis speech              *
     *---------------------------------------------------------------------*/

    /* pointer to interpolated LPC parameters */
    Az = A_t;

    for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
    {

        index = *parm++;                /* pitch index */

        if ((rxdtx_ctrl & RX_SP_FLAG) != 0)
        {
            T0 = Dec_lag6 (index, PIT_MIN, PIT_MAX, i_subfr, L_FRAME_BY2,
                           &T0_frac, bfi);


            /*-------------------------------------------------*
             * - Find the adaptive codebook vector.            *
             *-------------------------------------------------*/

            Pred_lt_6 (&exc[i_subfr], T0, T0_frac, L_SUBFR);
        }
        else
        {
            T0 = L_SUBFR;
        }

        /*-------------------------------------------------------*
         * - Decode pitch gain.                                  *
         *-------------------------------------------------------*/

        index = *parm++;

        gain_pit = d_gain_pitch (index, bfi, state, prev_bf, rxdtx_ctrl);


        /*-------------------------------------------------------*
         * - Decode innovative codebook.                         *
         *-------------------------------------------------------*/

        if ((rxdtx_ctrl & RX_SP_FLAG) != 0)
        {
            dec_10i40_35bits (parm, code);
        }
        else
        {
            /* Use pseudo noise for excitation when SP_flag == 0 */
            build_CN_code (code, &L_pn_seed_rx);
        }

        parm += 10;

        /*-------------------------------------------------------*
         * - Add the pitch contribution to code[].               *
         *-------------------------------------------------------*/

        /* pit_sharp = gain_pit;                   */
        /* if (pit_sharp > 1.0) pit_sharp = 1.0;   */

        //pit_sharp = shl(gain_pit, 3);
        pit_sharp = SHL_SAT16(gain_pit, 3);

        /* This loop is not entered when SP_FLAG is 0 */
        for (i = T0; i < L_SUBFR; i++)
        {
            //temp = mult (code[i - T0], pit_sharp);
            temp = MULT(code[i - T0], pit_sharp);

            //code[i] = add(code[i], temp);
            code[i] = ADD(code[i], temp);

        }

        /* post processing of excitation elements */


        //if (sub (pit_sharp, 16384) > 0)
        if (SUB (pit_sharp, 16384) > 0)
        {
            for (i = 0; i < L_SUBFR; i++)
            {
                //temp = mult (exc[i + i_subfr], pit_sharp);
                temp = MULT(exc[i + i_subfr], pit_sharp);
                //L_temp = L_mult (temp, gain_pit);
                L_temp = L_MULT(temp, gain_pit);
                //L_temp = L_shl (L_temp, 1);
                L_temp = L_SHL_SAT(L_temp, 1);
                //excp[i] = round(L_temp);
                excp[i] = ROUND(L_temp);

            }
        }
        /*-------------------------------------------------*
         * - Decode codebook gain.                         *
         *-------------------------------------------------*/

        index = *parm++;                /* index of energy VQ */

        d_gain_code (index, code, L_SUBFR, &gain_code, bfi, state, prev_bf,
                     rxdtx_ctrl, i_subfr, rx_dtx_state);


        /*-------------------------------------------------------*
         * - Find the total excitation.                          *
         * - Find synthesis speech corresponding to exc[].       *
         *-------------------------------------------------------*/
        for (i = 0; i < L_SUBFR; i++)
        {
            /* exc[i] = gain_pit*exc[i] + gain_code*code[i]; */

            //L_temp = L_mult (exc[i + i_subfr], gain_pit);
            //L_temp = L_MULT(exc[i + i_subfr], gain_pit);
            VPP_MLX16(tmp_hi, tmp_lo, exc[i + i_subfr], gain_pit);
            //L_temp = L_mac (L_temp, code[i], gain_code);
            //L_temp = L_MAC(L_temp, code[i], gain_code);
            VPP_MLA16(tmp_hi, tmp_lo, code[i], gain_code);
            //L_temp = L_shl (L_temp, 3);
            L_temp = L_SHL_SAT(VPP_SCALE64_TO_16(tmp_hi, tmp_lo), 3);

            //exc[i + i_subfr] = round(L_temp);
            exc[i + i_subfr] = ROUND(L_temp);
        }

        //if (sub (pit_sharp, 16384) > 0)
        if (SUB (pit_sharp, 16384) > 0)
        {
            for (i = 0; i < L_SUBFR; i++)
            {
                //excp[i] = add(excp[i], exc[i + i_subfr]);
                excp[i] = ADD(excp[i], exc[i + i_subfr]);
            }
            agc2 (&exc[i_subfr], excp, L_SUBFR);
            Syn_filt (Az, excp, &synth[i_subfr], L_SUBFR, mem_syn, 1);
        }
        else
        {
            Syn_filt (Az, &exc[i_subfr], &synth[i_subfr], L_SUBFR, mem_syn, 1);
        }


        /* interpolated LPC parameters for next subframe */
        Az += MP1;
    }

    /*--------------------------------------------------*
     * Update signal for next frame.                    *
     * -> shift to the left by L_FRAME  exc[]           *
     *--------------------------------------------------*/

    Copy (&old_exc[L_FRAME], &old_exc[0], PIT_MAX + L_INTERPOL);

    prev_bf = bfi;
    VPP_EFR_PROFILE_FUNCTION_EXIT(Decoder_12k2);
    return;
}










































