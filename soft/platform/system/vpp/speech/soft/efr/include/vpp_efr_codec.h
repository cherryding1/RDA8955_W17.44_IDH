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
#include "hal_speech.h"

#define BIT_0     0
#define BIT_1     1
#define PRM_NO    57

static const INT16 bitno[PRM_NO] =
{
    7, 8, 9, 8, 6,                          /* LSP VQ          */
    9, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 5,  /* first subframe  */
    6, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 5,  /* second subframe */
    9, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 5,  /* third subframe  */
    6, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 5   /* fourth subframe */
};


void vpp_efr_encode (HAL_SPEECH_PCM_HALF_BUF_T EncIn, HAL_SPEECH_ENC_OUT_T* EncOut);
void vpp_efr_decode (HAL_SPEECH_DEC_IN_T DecIn, HAL_SPEECH_PCM_HALF_BUF_T DecOut);

void Init_Coder_12k2 (void);

void Coder_12k2 (
    INT16 ana[],      /* output  : Analysis parameters */
    INT16 synth[]     /* output  : Local synthesis     */
);



void Init_Decoder_12k2 (void);

void Decoder_12k2 (
    INT16 parm[],     /* input : vector of synthesis parameters
                                  parm[0] = bad frame indicator (bfi) */
    INT16 synth[],    /* output: synthesis speech                    */
    INT16 A_t[],      /* output: decoded LP filter in 4 subframes    */
    INT16 TAF,
    INT16 SID_flag
);

void Init_Post_Filter (void);

void Post_Filter (
    INT16 *syn,       /* in/out: synthesis speech (postfiltered is output) */
    INT16 *Az_4       /* input : interpolated LPC parameters in all subfr. */
);

void code_10i40_35bits (
    INT16 x[],        /* (i)   : target vector                             */
    INT16 cn[],       /* (i)   : residual after long term prediction       */
    INT16 h[],        /* (i)   : impulse response of weighted synthesis
                                  filter                                    */
    INT16 cod[],      /* (o)   : algebraic (fixed) codebook excitation     */
    INT16 y[],        /* (o)   : filtered fixed codebook excitation        */
    INT16 indx[]      /* (o)   : index of 10 pulses (sign + position)      */
);
void dec_10i40_35bits (
    INT16 index[],    /* (i)   : index of 10 pulses (sign+position)        */
    INT16 cod[]       /* (o)   : algebraic (fixed) codebook excitation     */
);
INT16 Dec_lag6 (      /* output: return integer pitch lag                  */
    INT16 index,      /* input : received pitch index                      */
    INT16 pit_min,    /* input : minimum pitch lag                         */
    INT16 pit_max,    /* input : maximum pitch lag                         */
    INT16 i_subfr,    /* input : subframe flag                             */
    INT16 L_frame_by2,/* input : speech frame size divided by 2            */
    INT16 *T0_frac,   /* output: fractional part of pitch lag              */
    INT16 bfi         /* input : bad frame indicator                       */
);
INT16 d_gain_pitch (  /* out      : quantized pitch gain                   */
    INT16 index,      /* in       : index of quantization                  */
    INT16 bfi,        /* in       : bad frame indicator (good = 0)         */
    INT16 state,      /* in       : state of the state machine             */
    INT16 prev_bf,    /* Previous bf                                       */
    INT16 rxdtx_ctrl

);
void d_gain_code (
    INT16 index,      /* input : received quantization index               */
    INT16 code[],     /* input : innovation codevector                     */
    INT16 lcode,      /* input : codevector length                         */
    INT16 *gain_code, /* output: decoded innovation gain                   */
    INT16 bfi,        /* input : bad frame indicator                       */
    INT16 state,      /* in    : state of the state machine                */
    INT16 prev_bf,    /* Previous bf                                       */
    INT16 rxdtx_ctrl,
    INT16 i_subfr,
    INT16 rx_dtx_state

);
void D_plsf_5 (
    INT16 *indice,    /* input : quantization indices of 5 submatrices     */
    INT16 *lsp1_q,    /* output: quantized 1st LSP vector                  */
    INT16 *lsp2_q,    /* output: quantized 2nd LSP vector                  */
    INT16 bfi,        /* input : bad frame indicator (set to 1 if a bad
                                  frame is received)                        */
    INT16 rxdtx_ctrl,
    INT16 rx_dtx_state
);
INT16 Enc_lag6 (      /* output: Return index of encoding                  */
    INT16 T0,         /* input : Pitch delay                               */
    INT16 *T0_frac,   /* in/out: Fractional pitch delay                    */
    INT16 *T0_min,    /* in/out: Minimum search delay                      */
    INT16 *T0_max,    /* in/out: Maximum search delay                      */
    INT16 pit_min,    /* input : Minimum pitch delay                       */
    INT16 pit_max,    /* input : Maximum pitch delay                       */
    INT16 pit_flag    /* input : Flag for 1st or 3rd subframe              */
);

INT16 q_gain_pitch (  /* Return index of quantization                      */
    INT16 *gain       /* (i)    :  Pitch gain to quantize                  */
);

INT16 q_gain_code (   /* Return quantization index                         */
    INT16 code[],     /* (i)      : fixed codebook excitation              */
    INT16 lcode,      /* (i)      : codevector size                        */
    INT16 *gain,      /* (i/o)    : quantized fixed codebook gain          */
    INT16 txdtx_ctrl,
    INT16 i_subfr
);

INT16 G_pitch (       /* (o)     : Gain of pitch lag saturated to 1.2      */
    INT16 xn[],       /* (i)     : Pitch target.                           */
    INT16 y1[],       /* (i)     : Filtered adaptive codebook.             */
    INT16 L_subfr     /*         : Length of subframe.                     */
);
INT16 G_code (        /* out      : Gain of innovation code.               */
    INT16 xn[],       /* in       : target vector                          */
    INT16 y2[]        /* in       : filtered inovation vector              */
);

INT16 Interpol_6 (    /* (o)  : interpolated value                         */
    INT16 *x,         /* (i)  : input vector                               */
    INT16 frac        /* (i)  : fraction                                   */
);
void Int_lpc (
    INT16 lsp_old[],  /* input: LSP vector at the 4th subfr. of past frame */
    INT16 lsp_mid[],  /* input: LSP vector at the 2nd subfr. of
                          present frame                                     */
    INT16 lsp_new[],  /* input: LSP vector at the 4th subfr. of
                          present frame                                     */
    INT16 Az[]        /* output: interpolated LP parameters in all subfr.  */
);
void Int_lpc2 (
    INT16 lsp_old[],  /* input: LSP vector at the 4th subfr. of past frame */
    INT16 lsp_mid[],  /* input: LSP vector at the 2nd subframe of
                          present frame                                     */
    INT16 lsp_new[],  /* input: LSP vector at the 4th subframe of
                          present frame                                     */
    INT16 Az[]        /* output:interpolated LP parameters
                          in subframes 1 and 3                              */
);
INT16 Pitch_fr6 (     /* (o)     : pitch period.                           */
    INT16 exc[],      /* (i)     : excitation buffer                       */
    INT16 xn[],       /* (i)     : target vector                           */
    INT16 h[],        /* (i)     : impulse response of synthesis and
                                    weighting filters                       */
    INT16 L_subfr,    /* (i)     : Length of subframe                      */
    INT16 t0_min,     /* (i)     : minimum value in the searched range.    */
    INT16 t0_max,     /* (i)     : maximum value in the searched range.    */
    INT16 i_subfr,    /* (i)     : indicator for first subframe.           */
    INT16 *pit_frac   /* (o)     : chosen fraction.                        */
);
INT16 Pitch_ol (      /* output: open loop pitch lag                       */
    INT16 signal[],   /* input: signal used to compute the open loop pitch */
    /* signal[-pit_max] to signal[-1] should be known    */
    INT16 pit_min,    /* input : minimum pitch lag                         */
    INT16 pit_max,    /* input : maximum pitch lag                         */
    INT16 L_frame     /* input : length of frame to compute pitch          */
);
void Pred_lt_6 (
    INT16 exc[],      /* in/out: excitation buffer                         */
    INT16 T0,         /* input : integer pitch lag                         */
    INT16 frac,       /* input : fraction of lag                           */
    INT16 L_subfr     /* input : subframe size                             */
);
void Q_plsf_5 (
    INT16 *lsp1,      /* input : 1st LSP vector                            */
    INT16 *lsp2,      /* input : 2nd LSP vector                            */
    INT16 *lsp1_q,    /* output: quantized 1st LSP vector                  */
    INT16 *lsp2_q,    /* output: quantized 2nd LSP vector                  */
    INT16 *indice,    /* output: quantization indices of 5 matrices        */
    INT16 txdtx_ctrl  /* input : tx dtx control word                       */
);
void Bits2prm_12k2 (
    INT16 bits[],     /* input : serial bits                               */
    INT16 prm[]       /* output: analysis parameters                       */
);
void Prm2bits_12k2 (
    INT16 prm[],      /* input : analysis parameters                       */
    INT16 bits[]      /* output: serial bits                               */
);

/*INT16 Bin2int (      // Reconstructed parameter
    INT16 no_of_bits,  // input : number of bits associated with value
    INT16 *bitstream   // output: address where bits are written
);*/


