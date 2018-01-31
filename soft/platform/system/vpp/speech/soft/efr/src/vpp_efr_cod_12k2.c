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

#include "vpp_efr_window2.tab"

#include "vpp_efr_vad.h"
#include "vpp_efr_dtx.h"

#include "vpp_efr_debug.h"
#include "vpp_efr_profile_codes.h"

/*-----------------------------------------------------------*
 *    Coder constant parameters (defined in "cnst.h")        *
 *-----------------------------------------------------------*
 *   L_WINDOW    : LPC analysis window size                  *
 *   L_FRAME     : Frame size                                *
 *   L_FRAME_BY2 : Half the frame size                       *
 *   L_SUBFR     : Sub-frame size                            *
 *   M           : LPC order                                 *
 *   MP1         : LPC order+1                               *
 *   L_TOTAL     : Total size of speech buffer               *
 *   PIT_MIN     : Minimum pitch lag                         *
 *   PIT_MAX     : Maximum pitch lag                         *
 *   L_INTERPOL  : Length of filter for interpolation        *
 *-----------------------------------------------------------*/

/*--------------------------------------------------------*
 *         Static memory allocation.                      *
 *--------------------------------------------------------*/

/* Speech vector */

static INT16 old_speech[L_TOTAL];
static INT16 *speech, *p_window, *p_window_mid;
INT16 *new_speech;             /* Global variable */

/* Weight speech vector */
static INT32 old_wsp32[(L_FRAME + PIT_MAX + 1)/2];
static INT16 * old_wsp = ((INT16 *)old_wsp32) + 1;
static INT16 * wsp     = ((INT16 *)old_wsp32) + 1 + PIT_MAX;

// static INT16 old_wsp[L_FRAME + PIT_MAX];
// static INT16 *wsp;

/* Excitation vector */

static INT16 old_exc[L_FRAME + PIT_MAX + L_INTERPOL];
static INT16 *exc;

/* Zero vector */

static INT16 ai_zero[L_SUBFR + MP1];
static INT16 *zero;

/* Impulse response vector */

static INT16 *h1;
static INT16 hvec[L_SUBFR * 2];

/* Spectral expansion factors */

static const INT16 F_gamma1[M] =
{
    29491, 26542, 23888, 21499, 19349,
    17414, 15672, 14105, 12694, 11425
};
static const INT16 F_gamma2[M] =
{
    19661, 11797, 7078, 4247, 2548,
    1529, 917, 550, 330, 198
};

/* Lsp (Line spectral pairs) */

static INT16 lsp_old[M];
static INT16 lsp_old_q[M];

/* Filter's memory */

static INT16 mem_syn[M], mem_w0[M], mem_w[M];
static INT16 mem_err[M + L_SUBFR], *error;

/***************************************************************************
 *  FUNCTION:   Init_Coder_12k2
 *
 *  PURPOSE:   Initialization of variables for the coder section.
 *
 *  DESCRIPTION:
 *       - initilize pointers to speech buffer
 *       - initialize static  pointers
 *       - set static vectors to zero
 *
 ***************************************************************************/

void Init_Coder_12k2 (void)
{
    VPP_EFR_PROFILE_FUNCTION_ENTER(Init_Coder_12k2);

    /*--------------------------------------------------------------------------*
     *          Initialize pointers to speech vector.                           *
     *--------------------------------------------------------------------------*/

    new_speech = old_speech + L_TOTAL - L_FRAME;/* New speech     */
    speech = new_speech;                        /* Present frame  */
    p_window = old_speech + L_TOTAL - L_WINDOW; /* For LPC window */
    p_window_mid = p_window;                    /* For LPC window */

    /* Initialize static pointers */

    wsp = old_wsp + PIT_MAX;
    exc = old_exc + PIT_MAX + L_INTERPOL;
    zero = ai_zero + MP1;
    error = mem_err + M;
    h1 = &hvec[L_SUBFR];

    /* Static vectors to zero */

    //Set_zero (old_speech, L_TOTAL);
    //Set_zero (old_exc, PIT_MAX + L_INTERPOL);
    //Set_zero (old_wsp, PIT_MAX);
    //Set_zero (mem_syn, M);
    //Set_zero (mem_w, M);
    //Set_zero (mem_w0, M);
    //Set_zero (mem_err, M);
    //Set_zero (zero, L_SUBFR);
    //Set_zero (hvec, L_SUBFR);   /* set to zero "h1[-L_SUBFR..-1]" */

    memset((INT8*)old_speech, 0, L_TOTAL<<1);
    memset((INT8*)old_exc, 0, (PIT_MAX + L_INTERPOL)<<1);
    memset((INT8*)old_wsp, 0, PIT_MAX);
    memset((INT8*)mem_syn, 0, M<<1);
    memset((INT8*)mem_w, 0,M<<1);
    memset((INT8*)mem_w0, 0,M<<1);
    memset((INT8*)mem_err,0, M<<1);
    memset((INT8*)zero, 0,L_SUBFR<<1);
    memset((INT8*)hvec, 0,L_SUBFR<<1);

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

    /* Initialize lsp_old_q[] */
    Copy (lsp_old, lsp_old_q, M);

    VPP_EFR_PROFILE_FUNCTION_EXIT(Init_Coder_12k2);

    return;
}

/***************************************************************************
 *   FUNCTION:   Coder_12k2
 *
 *   PURPOSE:  Principle encoder routine.
 *
 *   DESCRIPTION: This function is called every 20 ms speech frame,
 *       operating on the newly read 160 speech samples. It performs the
 *       principle encoding functions to produce the set of encoded parameters
 *       which include the LSP, adaptive codebook, and fixed codebook
 *       quantization indices (addresses and gains).
 *
 *   INPUTS:
 *       No input arguments are passed to this function. However, before
 *       calling this function, 160 new speech data samples should be copied to
 *       the vector new_speech[]. This is a global pointer which is declared in
 *       this file (it points to the end of speech buffer minus 160).
 *
 *   OUTPUTS:
 *
 *       ana[]:     vector of analysis parameters.
 *       synth[]:   Local synthesis speech (for debugging purposes)
 *
 ***************************************************************************/

void Coder_12k2 (
    INT16 ana[],    /* output  : Analysis parameters */
    INT16 synth[]   /* output  : Local synthesis     */
)
{

    /* LPC coefficients */
    INT16 r_l[MP1], r_h[MP1];      /* Autocorrelations lo and hi           */
    INT16 A_t[(MP1) * 4];          /* A(z) unquantized for the 4 subframes */
    INT16 Aq_t[(MP1) * 4];         /* A(z)   quantized for the 4 subframes */
    INT16 Ap1[MP1];                /* A(z) with spectral expansion         */
    INT16 Ap2[MP1];                /* A(z) with spectral expansion         */
    INT16 *A, *Aq;                 /* Pointer on A_t and Aq_t              */
    INT16 lsp_new[M], lsp_new_q[M];/* LSPs at 4th subframe                 */
    INT16 lsp_mid[M], lsp_mid_q[M];/* LSPs at 2nd subframe                 */

    /* Other vectors */
    INT16 xn[L_SUBFR];            /* Target vector for pitch search        */
    INT16 xn2[L_SUBFR];           /* Target vector for codebook search     */
    INT16 res2[L_SUBFR];          /* Long term prediction residual         */
    INT16 code[L_SUBFR];          /* Fixed codebook excitation             */
    INT16 y1[L_SUBFR];            /* Filtered adaptive excitation          */
    INT16 y2[L_SUBFR];            /* Filtered fixed codebook excitation    */

    /* Scalars */
    INT16 i, j, k, i_subfr;
    INT16 T_op,T0_min, T0_max, T0_frac;
    INT16 gain_pit, gain_code, pit_flag;
    INT16 temp;
    INT32 L_temp;

    INT16 scal_acf, VAD_flag, lags[2], rc[4];
    INT16 T0 =0;
    INT16 pit_sharp=0;

    register INT32 temp_hi=0;
    register UINT32 temp_lo=0;


    extern INT16 ptch;
    extern INT16 txdtx_ctrl, CN_excitation_gain;
    extern INT32 L_pn_seed_tx;
    extern INT16 dtx_mode;

    VPP_EFR_PROFILE_FUNCTION_ENTER(Coder_12k2);

    /*----------------------------------------------------------------------*
     *  - Perform LPC analysis: (twice per frame)                           *
     *       * autocorrelation + lag windowing                              *
     *       * Levinson-Durbin algorithm to find a[]                        *
     *       * convert a[] to lsp[]                                         *
     *       * quantize and code the LSPs                                   *
     *       * find the interpolated LSPs and convert to a[] for all        *
     *         subframes (both quantized and unquantized)                   *
     *----------------------------------------------------------------------*/

    /* LP analysis centered at 2nd subframe */


    scal_acf = Autocorr (p_window_mid, M, r_h, r_l, window_160_80);
    /* Autocorrelations */

    Lag_window (M, r_h, r_l);   /* Lag windowing    */



    Levinson (r_h, r_l, &A_t[MP1], rc); /* Levinson-Durbin  */



    Az_lsp (&A_t[MP1], lsp_mid, lsp_old); /* From A(z) to lsp */



    /* LP analysis centered at 4th subframe */

    /* Autocorrelations */
    scal_acf = Autocorr (p_window, M, r_h, r_l, window_232_8);



    Lag_window (M, r_h, r_l);   /* Lag windowing    */


    Levinson (r_h, r_l, &A_t[MP1 * 3], rc); /* Levinson-Durbin  */



    Az_lsp (&A_t[MP1 * 3], lsp_new, lsp_mid); /* From A(z) to lsp */


    if (dtx_mode == 1)
    {
        /* DTX enabled, make voice activity decision */
        VAD_flag = vad_computation (r_h, r_l, scal_acf, rc, ptch);


        tx_dtx (VAD_flag, &txdtx_ctrl); /* TX DTX handler */
    }
    else
    {
        /* DTX disabled, active speech in every frame */
        VAD_flag = 1;
        txdtx_ctrl = TX_VAD_FLAG | TX_SP_FLAG;
    }

    /* LSP quantization (lsp_mid[] and lsp_new[] jointly quantized) */

    Q_plsf_5 (lsp_mid, lsp_new, lsp_mid_q, lsp_new_q, ana, txdtx_ctrl);


    ana += 5;
    /*--------------------------------------------------------------------*
     * Find interpolated LPC parameters in all subframes (both quantized  *
     * and unquantized).                                                  *
     * The interpolated parameters are in array A_t[] of size (M+1)*4     *
     * and the quantized interpolated parameters are in array Aq_t[]      *
     *--------------------------------------------------------------------*/

    Int_lpc2 (lsp_old, lsp_mid, lsp_new, A_t);

    if ((txdtx_ctrl & TX_SP_FLAG) != 0)
    {
        Int_lpc (lsp_old_q, lsp_mid_q, lsp_new_q, Aq_t);

        /* update the LSPs for the next frame */
        for (i = 0; i < M; i++)
        {
            lsp_old[i] = lsp_new[i];
            lsp_old_q[i] = lsp_new_q[i];
        }
    }
    else
    {
        /* Use unquantized LPC parameters in case of no speech activity */
        for (i = 0; i < MP1; i++)
        {
            Aq_t[i] = A_t[i];
            Aq_t[i + MP1] = A_t[i + MP1];
            Aq_t[i + MP1 * 2] = A_t[i + MP1 * 2];
            Aq_t[i + MP1 * 3] = A_t[i + MP1 * 3];
        }

        /* update the LSPs for the next frame */
        for (i = 0; i < M; i++)
        {
            lsp_old[i] = lsp_new[i];
            lsp_old_q[i] = lsp_new[i];
        }
    }


    /*----------------------------------------------------------------------*
     * - Find the weighted input speech wsp[] for the whole speech frame    *
     * - Find the open-loop pitch delay for first 2 subframes               *
     * - Set the range for searching closed-loop pitch in 1st subframe      *
     * - Find the open-loop pitch delay for last 2 subframes                *
     *----------------------------------------------------------------------*/

    A = A_t;
    for (i = 0; i < L_FRAME; i += L_SUBFR)
    {
        Weight_Ai (A, F_gamma1, Ap1);
        Weight_Ai (A, F_gamma2, Ap2);
        Residu (Ap1, &speech[i], &wsp[i], L_SUBFR);
        Syn_filt (Ap2, &wsp[i], &wsp[i], L_SUBFR, mem_w, 1);
        A += MP1;
    }

    /* Find open loop pitch lag for first two subframes */

    T_op = Pitch_ol (wsp, PIT_MIN, PIT_MAX, L_FRAME_BY2);
    lags[0] = T_op;

    if ((txdtx_ctrl & TX_SP_FLAG) != 0)
    {
        /* Range for closed loop pitch search in 1st subframe */

        //T0_min = sub (T_op, 3);
        T0_min = SUB (T_op, 3);

        //if (sub (T0_min, PIT_MIN) < 0)
        if (SUB (T0_min, PIT_MIN) < 0)
        {
            T0_min = PIT_MIN;
        }

        //T0_max = add (T0_min, 6);
        T0_max = ADD (T0_min, 6);

        if ((T0_max-PIT_MAX) > 0)
        {
            T0_max = PIT_MAX;
            //T0_min = sub (T0_max, 6);
            T0_min = SUB (T0_max, 6);
        }

    }
    /* Find open loop pitch lag for last two subframes */

    T_op = Pitch_ol (&wsp[L_FRAME_BY2], PIT_MIN, PIT_MAX, L_FRAME_BY2);


    if (dtx_mode == 1)
    {
        lags[1] = T_op;
        periodicity_update (lags, &ptch);
    }
    /*----------------------------------------------------------------------*
     *          Loop for every subframe in the analysis frame               *
     *----------------------------------------------------------------------*
     *  To find the pitch and innovation parameters. The subframe size is   *
     *  L_SUBFR and the loop is repeated L_FRAME/L_SUBFR times.             *
     *     - find the weighted LPC coefficients                             *
     *     - find the LPC residual signal res[]                             *
     *     - compute the target signal for pitch search                     *
     *     - compute impulse response of weighted synthesis filter (h1[])   *
     *     - find the closed-loop pitch parameters                          *
     *     - encode the pitch delay                                         *
     *     - update the impulse response h1[] by including pitch            *
     *     - find target vector for codebook search                         *
     *     - codebook search                                                *
     *     - encode codebook address                                        *
     *     - VQ of pitch and codebook gains                                 *
     *     - find synthesis speech                                          *
     *     - update states of weighting filter                              *
     *----------------------------------------------------------------------*/

    /* pointer to interpolated LPC parameters          */
    A = A_t;
    /* pointer to interpolated quantized LPC parameters */
    Aq = Aq_t;

    for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
    {

        if ((txdtx_ctrl & TX_SP_FLAG) != 0)
        {

            /*---------------------------------------------------------------*
             * Find the weighted LPC coefficients for the weighting filter.  *
             *---------------------------------------------------------------*/

            Weight_Ai (A, F_gamma1, Ap1);

            Weight_Ai (A, F_gamma2, Ap2);

            /*---------------------------------------------------------------*
             * Compute impulse response, h1[], of weighted synthesis filter  *
             *---------------------------------------------------------------*/

            for (i = 0; i <= M; i++)
            {
                ai_zero[i] = Ap1[i];
            }

            Syn_filt (Aq, ai_zero, h1, L_SUBFR, zero, 0);
            Syn_filt (Ap2, h1, h1, L_SUBFR, zero, 0);


        }
        /*---------------------------------------------------------------*
         *          Find the target vector for pitch search:             *
         *---------------------------------------------------------------*/

        Residu (Aq, &speech[i_subfr], res2, L_SUBFR);   /* LPC residual */

        if ((txdtx_ctrl & TX_SP_FLAG) == 0)
        {
            /* Compute comfort noise excitation gain based on
            LP residual energy */

            CN_excitation_gain = compute_CN_excitation_gain (res2);

        }
        else
        {
            Copy (res2, &exc[i_subfr], L_SUBFR);

            Syn_filt (Aq, &exc[i_subfr], error, L_SUBFR, mem_err, 0);

            Residu (Ap1, error, xn, L_SUBFR);

            Syn_filt (Ap2, xn, xn, L_SUBFR, mem_w0, 0); /* target signal xn[]*/


            /*--------------------------------------------------------------*
             *                 Closed-loop fractional pitch search          *
             *--------------------------------------------------------------*/

            /* flag for first and 3th subframe */
            pit_flag = i_subfr;
            /* set t0_min and t0_max for 3th subf.*/
            //if (sub (i_subfr, L_FRAME_BY2) == 0)
            if (SUB (i_subfr, L_FRAME_BY2) == 0)
            {
                //T0_min = sub (T_op, 3);
                T0_min = SUB (T_op, 3);

                if ((T0_min-PIT_MIN) < 0)
                {
                    T0_min = PIT_MIN;
                }

                //T0_max = add (T0_min, 6);
                T0_max = ADD (T0_min, 6);


                if ((T0_max - PIT_MAX) > 0)
                {
                    T0_max = PIT_MAX;
                    //T0_min = sub (T0_max, 6);
                    T0_min = SUB (T0_max, 6);
                }
                pit_flag = 0;
            }


            T0 = Pitch_fr6 (&exc[i_subfr], xn, h1, L_SUBFR, T0_min, T0_max,
                            pit_flag, &T0_frac);


            *ana = Enc_lag6 (T0, &T0_frac, &T0_min, &T0_max, PIT_MIN,
                             PIT_MAX, pit_flag);


        }
        ana++;
        /* Incrementation of ana is done here to work also
        when no speech activity is present */

        if ((txdtx_ctrl & TX_SP_FLAG) != 0)
        {

            /*---------------------------------------------------------------*
             * - find unity gain pitch excitation (adaptive codebook entry)  *
             *   with fractional interpolation.                              *
             * - find filtered pitch exc. y1[]=exc[] convolved with h1[]     *
             * - compute pitch gain and limit between 0 and 1.2              *
             * - update target vector for codebook search                    *
             * - find LTP residual.                                          *
             *---------------------------------------------------------------*/

            Pred_lt_6 (&exc[i_subfr], T0, T0_frac, L_SUBFR);

            Convolve (&exc[i_subfr], h1, y1, L_SUBFR);

            gain_pit = G_pitch (xn, y1, L_SUBFR);

            *ana = q_gain_pitch (&gain_pit);



        }
        else
        {
            gain_pit = 0;
        }

        ana++;                  /* Incrementation of ana is done here to work
                                   also when no speech activity is present */

        if ((txdtx_ctrl & TX_SP_FLAG) != 0)
        {
            /* xn2[i]   = xn[i] - y1[i] * gain_pit  */
            /* res2[i] -= exc[i+i_subfr] * gain_pit */

            for (i = 0; i < L_SUBFR; i++)
            {
                //L_temp = L_mult (y1[i], gain_pit);
                L_temp = L_MULT(y1[i], gain_pit);
                //L_temp = l_shl (L_temp, 3);
                L_temp = L_SHL_SAT(L_temp, 3);
                //xn2[i] = sub (xn[i], extract_h (L_temp));
                xn2[i] = SUB (xn[i], EXTRACT_H(L_temp));

                //L_temp = L_mult (exc[i + i_subfr], gain_pit);
                L_temp = L_MULT(exc[i + i_subfr], gain_pit);
                //L_temp = l_shl (L_temp, 3);
                L_temp = L_SHL_SAT(L_temp, 3);
                //res2[i] = sub (res2[i], extract_h (L_temp));
                res2[i] = SUB (res2[i], EXTRACT_H(L_temp));

            }


            /*-------------------------------------------------------------*
             * - include pitch contribution into impulse resp. h1[]        *
             *-------------------------------------------------------------*/

            /* pit_sharp = gain_pit;                   */
            /* if (pit_sharp > 1.0) pit_sharp = 1.0;   */

            //pit_sharp = shl (gain_pit, 3);
            pit_sharp = SHL_SAT16(gain_pit, 3);


            for (i = T0; i < L_SUBFR; i++)
            {
                //temp = mult (h1[i - T0], pit_sharp);
                temp = MULT(h1[i - T0], pit_sharp);

                //h1[i] = add (h1[i], temp);
                h1[i] = ADD (h1[i], temp);

            }



            /*--------------------------------------------------------------*
             * - Innovative codebook search (find index and gain)           *
             *--------------------------------------------------------------*/

            code_10i40_35bits (xn2, res2, h1, code, y2, ana);


        }
        else
        {
            build_CN_code (code, &L_pn_seed_tx);
        }
        ana += 10;

        if ((txdtx_ctrl & TX_SP_FLAG) != 0)
        {

            /*-------------------------------------------------------*
             * - Add the pitch contribution to code[].               *
             *-------------------------------------------------------*/

            for (i = T0; i < L_SUBFR; i++)
            {
                //temp = mult (code[i - T0], pit_sharp);
                temp = MULT(code[i - T0], pit_sharp);

                //code[i] = add (code[i], temp);
                code[i] = ADD (code[i], temp);

            }



            /*------------------------------------------------------*
             * - Quantization of fixed codebook gain.               *
             *------------------------------------------------------*/

            gain_code = G_code (xn2, y2);

        }
        *ana++ = q_gain_code (code, L_SUBFR, &gain_code, txdtx_ctrl, i_subfr);


        /*------------------------------------------------------*
         * - Find the total excitation                          *
         * - find synthesis speech corresponding to exc[]       *
         * - update filter memories for finding the target      *
         *   vector in the next subframe                        *
         *   (update mem_err[] and mem_w0[])                    *
         *------------------------------------------------------*/

        for (i = 0; i < L_SUBFR; i++)
        {
            /* exc[i] = gain_pit*exc[i] + gain_code*code[i]; */

            //L_temp = L_mult (exc[i + i_subfr], gain_pit);
            //L_temp = L_MULT(exc[i + i_subfr], gain_pit);
            VPP_MLX16(temp_hi, temp_lo, exc[i + i_subfr], gain_pit);
            //L_temp = L_mac (L_temp, code[i], gain_code);
            //L_temp = L_MAC(L_temp, code[i], gain_code);
            VPP_MLA16(temp_hi, temp_lo, code[i], gain_code);

            //L_temp = l_shl (L_temp, 3);
            L_temp = L_SHL_SAT(VPP_SCALE64_TO_16(temp_hi, temp_lo), 3);
            //exc[i + i_subfr] = round(L_temp);
            exc[i + i_subfr] = ROUND(L_temp);
        }


        Syn_filt (Aq, &exc[i_subfr], &synth[i_subfr], L_SUBFR, mem_syn, 1);


        if ((txdtx_ctrl & TX_SP_FLAG) != 0)
        {

            for (i = L_SUBFR - M, j = 0; i < L_SUBFR; i++, j++)
            {
                //mem_err[j] = sub (speech[i_subfr + i], synth[i_subfr + i]);
                mem_err[j] = SUB (speech[i_subfr + i], synth[i_subfr + i]);

                //temp = extract_h (L_shl (L_mult (y1[i], gain_pit), 3));
                temp = EXTRACT_H (L_SHL_SAT (L_MULT(y1[i], gain_pit), 3));
                //k = extract_h (L_shl (L_mult (y2[i], gain_code), 5));
                k = EXTRACT_H (L_SHL_SAT (L_MULT(y2[i], gain_code), 5));

                //mem_w0[j] = sub (xn[i], add (temp,k));
                mem_w0[j] = SUB (xn[i], ADD (temp,k));

            }
        }
        else
        {
            for (j = 0; j < M; j++)
            {
                mem_err[j] = 0;
                mem_w0[j] = 0;
            }
        }

        /* interpolated LPC parameters for next subframe */
        A += MP1;
        Aq += MP1;
    }

    /*--------------------------------------------------*
     * Update signal for next frame.                    *
     * -> shift to the left by L_FRAME:                 *
     *     speech[], wsp[] and  exc[]                   *
     *--------------------------------------------------*/

    Copy (&old_speech[L_FRAME], &old_speech[0], L_TOTAL - L_FRAME);
    Copy (&old_wsp[L_FRAME], &old_wsp[0], PIT_MAX);
    Copy (&old_exc[L_FRAME], &old_exc[0], PIT_MAX + L_INTERPOL);

    VPP_EFR_PROFILE_FUNCTION_EXIT(Coder_12k2);

    return;
}










































