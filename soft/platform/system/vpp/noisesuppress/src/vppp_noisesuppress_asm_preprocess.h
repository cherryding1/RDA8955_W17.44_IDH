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

#ifndef VPPP_NOISESUPPRESS_ASM_PREPROCESS_H
#define VPPP_NOISESUPPRESS_ASM_PREPROCESS_H

//speex_aec
#define SPEEX_NB_BANDS  24
#define SPEEX_NN    80


//#define SPEEX_NN  256
#define NOISE_SHIFT 7
#define NEG_NOISE_SHIFT (-NOISE_SHIFT)
#define NEG_NOISE_SHIFT_1 (1-NOISE_SHIFT)
#define SNR_SHIFT   8
#define NEG_SNR_SHIFT   (-SNR_SHIFT)
#define EXPIN_SHIFT 11
#define NEG_EXPIN_SHIFT (-EXPIN_SHIFT)
#define EXPIN_SNR_SHIFT (EXPIN_SHIFT-SNR_SHIFT)
#define SNR_EXPIN_SHIFT (SNR_SHIFT-EXPIN_SHIFT)
#define NEG_EXPIN_SNR_SHIFT (-EXPIN_SNR_SHIFT)

#define SPEEX_N_PLUS_M  (SPEEX_NN+SPEEX_NB_BANDS)

#define SPEECH_PROB_START_DEFAULT       11469
#define SPEECH_PROB_CONTINUE_DEFAULT    6554
#define NOISE_SUPPRESS_DEFAULT       -30
#define ECHO_SUPPRESS_DEFAULT        -60
#define ECHO_SUPPRESS_ACTIVE_DEFAULT -30

#define MAXFACTORS 32

//voc_short KFS_substate_nfft   ,y
//voc_short KFS_substate_inverse    ,y
//voc_short KFS_substate_factors    ,(2*MAXFACTORS),y
//voc_short KFS_substate_twiddles   ,2,y
//voc_short KFS_tmpbuf  ,(2*(SPEEX_NN)),y
//voc_short KFS_super_twiddles  ,(2*(SPEEX_NN)),y
#define SUBSTATE_NFFT_OFFSET    0
#define SUBSTATE_INVERSE_OFFSET (SUBSTATE_NFFT_OFFSET+1)
#define SUBSTATE_FACTORS_OFFSET (SUBSTATE_INVERSE_OFFSET+1)
#define SUBSTATE_TWIDDLES_OFFSET    (SUBSTATE_FACTORS_OFFSET+2*MAXFACTORS)
#define TMPBUF_OFFSET   (SUBSTATE_TWIDDLES_OFFSET+2)
#define SUPER_TWIDDLES_OFFSET   (TMPBUF_OFFSET+2*SPEEX_NN)


#define SPEEX_NOISESUPPRESS_CONSTX_SIZE             880 //(SPEEX_NN*18+MAXFACTORS*4+8+178)


// speex_aec functions
void speex_div32_16(void);
void spx_ilog2(void);
void spx_ilog4(void);
void spx_atan01(void);
void spx_atan(void);
void speex_toBARK(void);
void filterbank_new(void);
void _spx_cos_pi_2(void);
void spx_cos_norm(void);
void spx_sqrt(void);
void spx_fft(void);
void filterbank_compute_bank32(void);
void update_noise_prob(void);
void spx_DIV32_16_Q8(void);
void spx_DIV32_16_Q15(void);
void spx_fft_init(void);
void spx_kf_factor(void);
void spx_qcurve(void);
void compute_gain_floor(void);
void hypergeom_gain(void);
void spx_exp(void);
void spx_ifft(void);
void spx_kiss_fft(void);
void spx_kf_shuffle(void);
void spx_kf_bfly4(void);
void spx_kiss_fftri2(void);
void spx_kf_work(void);
void spx_kf_shuffle(void);
void spx_kf_bfly5(void);
void speex_preprocess_state_init(void);
void speex_preprocess_run(void);
void speex_div32_16_sign(void);

void preprocess_analysis(void);
void spx_mult16_32_q15(void);
void filterbank_compute_psd16(void);
void spx_mult16_32_p15(void);
void Rda_speex_preprocess_state_reset(void);


#endif

