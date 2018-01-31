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





#include "vppp_speech_asm_noisesuppress.h"

#include "vppp_speech_common.h"

#include "voc2_library.h"
#include "voc2_define.h"

// VoC_directive: PARSER_OFF
#include <stdio.h>
// VoC_directive: PARSER_ON





#if 0

voc_short For_noisesuppress_align   ,816,x


//x part
//CONST IN X
// preprocess
// Basic info
voc_short SpxPpState_frame_size ,x
voc_short SpxPpState_ps_size    ,x
voc_short SpxPpState_sampling_rate  ,x
voc_short SpxPpState_nbands ,x

// Parameters
voc_short SpxPpState_denoise_enabled    ,x
voc_short SpxPpState_vad_enabled    ,x
voc_short SpxPpState_dereverb_enabled   ,x
voc_short SpxPpState_reverb_decay   ,x
voc_short SpxPpState_reverb_level   ,x
voc_short SpxPpState_speech_prob_start  ,x
voc_short SpxPpState_speech_prob_continue   ,x
voc_short SpxPpState_noise_suppress ,x
voc_short SpxPpState_echo_suppress  ,x
voc_short SpxPpState_echo_suppress_active   ,x
voc_short SpxPpState_echo_state_ADDR    ,x

voc_short SpxPpState_speech_prob    ,x
// DSP-related arrays
voc_short SpxPpState_frame  ,SPEEX_NN,x
voc_short SpxPpState_frame_N    ,SPEEX_NN,x
voc_short SpxPpState_ft ,2*SPEEX_NN,x

voc_short SpxPpState_gain   ,(SPEEX_NN),x
voc_short SpxPpState_gain_M ,(SPEEX_NB_BANDS),x
voc_short SpxPpState_prior  ,(SPEEX_NN),x
voc_short SpxPpState_prior_M    ,(SPEEX_NB_BANDS),x
voc_short SpxPpState_post   ,(SPEEX_NN),x
voc_short SpxPpState_post_M ,(SPEEX_NB_BANDS),x
voc_short SpxPpState_gain2  ,(SPEEX_NN),x
voc_short SpxPpState_gain2_M    ,(SPEEX_NB_BANDS),x
voc_short SpxPpState_gain_floor ,(SPEEX_NN),x
voc_short SpxPpState_gain_floor_M   ,(SPEEX_NB_BANDS),x
voc_short SpxPpState_zeta   ,(SPEEX_NN),x
voc_short SpxPpState_zeta_M ,(SPEEX_NB_BANDS),x

voc_short SpxPpState_inbuf  ,SPEEX_NN,x
voc_short SpxPpState_outbuf ,SPEEX_NN,x
voc_short SpxPpState_nb_adapt   ,x
voc_short SpxPpState_was_speech ,x
voc_short SpxPpState_min_count  ,x
voc_short SpxPpState_frame_shift    ,x


voc_word SpxPpState_noise   ,SPEEX_NN,x
voc_word SpxPpState_noise_M ,SPEEX_NB_BANDS,x
voc_word SpxPpState_reverb_estimate ,(SPEEX_NN+SPEEX_NB_BANDS),x
voc_word SpxPpState_old_ps  ,(SPEEX_NN),x
voc_word SpxPpState_old_ps_M    ,(SPEEX_NB_BANDS),x
voc_word SpxPpState_ps  ,(SPEEX_NN),x
voc_word SpxPpState_ps_M    ,(SPEEX_NB_BANDS),x

voc_word SpxPpState_S   ,SPEEX_NN,x
voc_word SpxPpState_Smin    ,SPEEX_NN,x
voc_word SpxPpState_Stmp    ,SPEEX_NN,x
voc_short SpxPpState_update_prob    ,SPEEX_NN,x

voc_word SpxPpState_echo_noise  ,(SPEEX_NN),x
voc_word SpxPpState_echo_noise_M    ,(SPEEX_NB_BANDS),x
voc_word SpxPpState_residual_echo   ,(SPEEX_NN+SPEEX_NB_BANDS),x

voc_short SpxPpState_fft_in_ADDR    ,x
voc_short SpxPpState_fft_out_ADDR   ,x
voc_short SpxPpState_ifft_in_ADDR   ,x
voc_short SpxPpState_ifft_out_ADDR  ,x

// echo state
voc_short SpxEchoState_y    ,2*SPEEX_NN,x
voc_short SpxEchoState_Y    ,2*SPEEX_NN,x
voc_short SpxEchoState_leak_estimate    ,x
voc_short SpxEchoState_reservered   ,x
voc_short SpxEchoState_last_y   ,SPEEX_NN,x
voc_short SpxEchoState_last_y_N ,SPEEX_NN,x

//voc_word SPEEX_VAR_RL7_ADDR   ,x
// ref_buf

voc_short SPEEX_REF_BUF ,SPEEX_NN,x
//voc_alias SPEEX_REF_BUF   SBC_DEC_FRAME_PCM_SAMPLE,x

//filterbank_new
//voc_short FILTERBANK_NEW_SAMPLING ,x
//voc_short FILTERBANK_NEW_TYPE ,x


//voc_word FILTERBANK_DF    ,x
//voc_word FILTERBANK_MAX_MEL   ,x
//voc_word FILTERBANK_MEL_INTERVAL  ,x

// conj_window
//voc_short SPEEX_ST_WINDOW ,2*SPEEX_NN,x
//voc_short CONJ_WINDOW_LEN ,x

// speex_preprocess
//voc_short SPEEX_PRE_RETURN    ,x

// compute_gain_floor
voc_word COMPUTE_GAIN_FLOOR_NOISE_ECHO_GAIN ,x

// spx_kf_bfly
voc_word SPX_KF_BFLY3_EPI3  ,x
voc_short SPX_KF_BFLY_SCRATCH       ,1,x
voc_alias SPX_KF_BFLY_SCRATCH_0     SPX_KF_BFLY_SCRATCH,x
voc_short SPX_KF_BFLY_SCRATCH_0i    ,1,x
voc_short SPX_KF_BFLY_SCRATCH_1     ,1,x
voc_short SPX_KF_BFLY_SCRATCH_1i    ,1,x
voc_short SPX_KF_BFLY_SCRATCH_2     ,1,x
voc_short SPX_KF_BFLY_SCRATCH_2i    ,1,x
voc_short SPX_KF_BFLY_SCRATCH_3     ,1,x
voc_short SPX_KF_BFLY_SCRATCH_3i    ,1,x
voc_short SPX_KF_BFLY_SCRATCH_4     ,1,x
voc_short SPX_KF_BFLY_SCRATCH_4i    ,1,x
voc_short SPX_KF_BFLY_SCRATCH_5     ,1,x
voc_short SPX_KF_BFLY_SCRATCH_5i    ,1,x
voc_short SPX_KF_BFLY_SCRATCH_6     ,1,x
voc_short SPX_KF_BFLY_SCRATCH_6i    ,1,x
voc_short SPX_KF_BFLY_SCRATCH_7     ,1,x
voc_short SPX_KF_BFLY_SCRATCH_7i    ,1,x
voc_short SPX_KF_BFLY_SCRATCH_8     ,1,x
voc_short SPX_KF_BFLY_SCRATCH_8i    ,1,x
voc_short SPX_KF_BFLY_SCRATCH_9     ,1,x
voc_short SPX_KF_BFLY_SCRATCH_9i    ,1,x
voc_short SPX_KF_BFLY_SCRATCH_10    ,1,x
voc_short SPX_KF_BFLY_SCRATCH_10i   ,1,x
voc_short SPX_KF_BFLY_SCRATCH_11    ,1,x
voc_short SPX_KF_BFLY_SCRATCH_11i   ,1,x
voc_short SPX_KF_BFLY_SCRATCH_12    ,1,x
voc_short SPX_KF_BFLY_SCRATCH_12i   ,1,x
voc_short SPX_KF_BFLY_YA    ,x
voc_short SPX_KF_BFLY_YAi   ,x
voc_short SPX_KF_BFLY_YB    ,x
voc_short SPX_KF_BFLY_YBi   ,x
voc_short SPX_KF_BFLY_FSTRIDE   ,x
voc_short SPX_KF_BFLY_M ,x
voc_short SPX_KF_BFLY_N ,x
voc_short SPX_KF_BFLY_MM    ,x
voc_short SPX_KF_BFLY_TMP   ,x
//voc_short SPEEX_PRE_RESET ,x
//voc_short SPEEX_PRE_FRAME_NUM ,x
voc_short SPEEX_RESERVR_01  ,1,x
voc_short SPEEX_SPEAKER_DELAY_BUFFER ,160,x
voc_short SPEEX_RESERVR_FOR_ALIGN   ,6,x


//CONST X
/********************************speex const begin*********************************/

voc_short SpxPpState_window ,2*SPEEX_NN,x

voc_short SpxPpState_bank_bank_left ,SPEEX_NN,x
//voc_alias SpxPpState_bank SpxPpState_bank_bank_left,x
voc_short SpxPpState_bank_bank_right    ,SPEEX_NN,x
voc_short SpxPpState_bank_filter_left   ,SPEEX_NN,x
voc_short SpxPpState_bank_filter_right  ,SPEEX_NN,x
voc_short SpxPpState_bank_nb_banks  ,x
voc_short SpxPpState_bank_len   ,x

voc_alias FILTERBANK_NB_BANKS SpxPpState_bank_nb_banks,x
voc_alias FILTERBANK_LEN    SpxPpState_bank_len,x


voc_short SpxPpState_fft_lookup_forward_substate_nfft   ,x
voc_alias SpxPpState_fft_lookup SpxPpState_fft_lookup_forward_substate_nfft,x
voc_alias SpxPpState_fft_lookup_forward_substate SpxPpState_fft_lookup_forward_substate_nfft,x
voc_short SpxPpState_fft_lookup_forward_substate_inverse    ,x
voc_short SpxPpState_fft_lookup_forward_substate_factors    ,(2*MAXFACTORS),x
voc_short SpxPpState_fft_lookup_forward_substate_twiddles   ,(2*SPEEX_NN),x
voc_short SpxPpState_fft_lookup_forward_tmpbuf  ,(2*SPEEX_NN),x
voc_short SpxPpState_fft_lookup_forward_super_twiddles  ,(2*SPEEX_NN),x

voc_short SpxPpState_fft_lookup_backward_substate_nfft  ,x
voc_alias SpxPpState_fft_lookup_backward_substate SpxPpState_fft_lookup_backward_substate_nfft,x
voc_short SpxPpState_fft_lookup_backward_substate_inverse   ,x
voc_short SpxPpState_fft_lookup_backward_substate_factors   ,(2*MAXFACTORS),x
voc_short SpxPpState_fft_lookup_backward_substate_twiddles  ,(2*SPEEX_NN),x
voc_short SpxPpState_fft_lookup_backward_tmpbuf ,(2*SPEEX_NN),x
voc_short SpxPpState_fft_lookup_backward_super_twiddles ,(2*SPEEX_NN),x

voc_short SpxPpState_fft_lookup_N   ,x
voc_short SpxPpState_fft_lookup_reserved,x

// echo_state
voc_short SpxEchoState_window   ,2*SPEEX_NN,x
voc_alias SpxEchoState_fft_table    SpxPpState_fft_lookup_forward_substate_nfft,x


voc_word SPEEX_ATAN01_M ,4,x
voc_word SPEEX_COS_PI_2_L   ,4,x
voc_short SPEEX_SQRT_C  ,4,x
voc_short SPEEX_UPDATE_NOISE_X  ,4,x

voc_short SPEEX_HYPERGEOM_GAIN_TABLE    ,22,x
voc_short SPEEX_EXP_D   ,4,x

voc_word SPEEX_CONST_0  ,x
voc_word SPEEX_CONST_1  ,x
voc_word SPEEX_CONST_2  ,x
voc_word SPEEX_CONST_3  ,x
voc_word SPEEX_CONST_4  ,x
voc_word SPEEX_CONST_5  ,x
voc_word SPEEX_CONST_6  ,x
voc_word SPEEX_CONST_7  ,x
voc_word SPEEX_CONST_8  ,x
voc_word SPEEX_CONST_14 ,x

voc_word SPEEX_CONST_16 ,x
voc_word SPEEX_CONST_17 ,x
voc_word SPEEX_CONST_19 ,x
voc_word SPEEX_CONST_20 ,x
voc_word SPEEX_CONST_29 ,x
voc_word SPEEX_CONST_64 ,x
voc_word SPEEX_CONST_97 ,x
voc_word SPEEX_CONST_100    ,x
voc_word SPEEX_CONST_128    ,x
voc_word SPEEX_CONST_236    ,x

voc_word SPEEX_CONST_256    ,x
voc_word SPEEX_CONST_472    ,x
voc_word SPEEX_CONST_512    ,x
voc_word SPEEX_CONST_983    ,x
voc_word SPEEX_CONST_1000   ,x
voc_word SPEEX_CONST_2458   ,x
voc_word SPEEX_CONST_3277   ,x
voc_word SPEEX_CONST_4915   ,x
voc_word SPEEX_CONST_6521   ,x
voc_word SPEEX_CONST_6711   ,x

voc_word SPEEX_CONST_8192   ,x
voc_word SPEEX_CONST_9830   ,x
voc_word SPEEX_CONST_10000  ,x
voc_word SPEEX_CONST_10912  ,x
voc_word SPEEX_CONST_16384  ,x
voc_word SPEEX_CONST_19458  ,x
voc_word SPEEX_CONST_19661  ,x
voc_word SPEEX_CONST_20000  ,x
voc_word SPEEX_CONST_20839  ,x
voc_word SPEEX_CONST_21290  ,x

voc_word SPEEX_CONST_21924  ,x
voc_word SPEEX_CONST_22938  ,x
voc_word SPEEX_CONST_23637  ,x
voc_word SPEEX_CONST_24576  ,x
voc_word SPEEX_CONST_25736  ,x
voc_word SPEEX_CONST_26214  ,x
voc_word SPEEX_CONST_29164  ,x
voc_word SPEEX_CONST_29458  ,x
voc_word SPEEX_CONST_32000  ,x
voc_word SPEEX_CONST_32766  ,x
voc_word SPEEX_CONST_32767  ,x

voc_word SPEEX_CONST_32768  ,x
voc_word SPEEX_CONST_65535  ,x
voc_word SPEEX_CONST_65536  ,x
voc_word SPEEX_CONST_131071 ,x
voc_word SPEEX_CONST_131072 ,x
voc_word SPEEX_CONST_0x80000    ,x
voc_word SPEEX_CONST_1087164    ,x
voc_word SPEEX_CONST_0x800000   ,x
voc_word SPEEX_CONST_0x7fffffff ,x
voc_word SPEEX_CONST_NEG21290   ,x

voc_word SPEEX_CONST_NEG15  ,x
voc_word SPEEX_CONST_NEG2   ,x
voc_word SPEEX_CONST_NEG32767   ,x

/********************************speex const end*********************************/


#endif


#if 1
////RAMY

//GLOBAL
//---------------//74 --INPUT ADDRESS OUTPUT ADRESS
//|
//|--------------////STRUCT FRAME  74+10
//|                         SIZE
//|--------------////
//|
//LOCAL
//---------------//sbc crc_HEADER 2560 ==0X1400
//|                          11*2 BYTE
//|--------------//
//|
//|--------------////
//|
//|--------------////INPUT STREAM BUFF 0x2000
//|                         2048 BYTE
//|--------------///OUTPUT STREAM BUFF  0x2400
//|                       2048 BYTE
//|--------------///BOTTOM 0x1400



////RAMX
//---------------//sbc crc_table 0
//|                          256 btye
//|--------------//
//|
//|--------------////
//|
//|--------------////
//|
//|--------------///
//|
//|--------------///BOTTOM 0x1400
/**************************************************************************************
 * Function:    Rda_NoiseSuppress
 *
 * Description: no
 *
 * Inputs:
 *
 * Outputs:
 *
 *
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       12/23/2013
 **************************************************************************************/
void Rda_NoiseSuppress(void)
{

    VoC_push16(RA, DEFAULT);

//  VoC_sw16_d(REG0,GLABAL_NOISESUPPRESS_INPUTADDR_ADDR);

    //load NOISE SUPPRESS CONSTX
    VoC_lw32_d(REG45,GLOBAL_NOISESUPPRESS_CONSTX_ADDR);
    VoC_lw16i(REG2,SpxPpState_window/2);
    VoC_lw16i(REG3,SPEEX_NOISESUPPRESS_CONSTX_SIZE);
    VoC_jal(Rda_RunDma);

    VoC_lw16d_set_inc(REG0,GLABAL_NOISESUPPRESS_INPUTADDR_ADDR,2);
    VoC_lw16i_set_inc(REG1,SPEEX_REF_BUF,2);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_loop_i(0,40)
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_endloop0

    VoC_lw16d_set_inc(REG0,GLABAL_NOISESUPPRESS_SPEAKERADDR_ADDR,2);
    VoC_lw16i_set_inc(REG1,SPEEX_SPEAKER_DELAY_BUFFER+154,2);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_loop_i(0,3)

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG1, SpxEchoState_last_y_N, 2);
    VoC_lw16i_set_inc(REG0, SpxEchoState_last_y, 2);

    VoC_lw32inc_p(ACC0, REG1, DEFAULT);
    VoC_loop_i(0, 40)

    VoC_lw32inc_p(ACC0, REG1, DEFAULT);
    VoC_sw32inc_p(ACC0, REG0, DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG0, SPEEX_SPEAKER_DELAY_BUFFER, 2);
    VoC_lw16i_set_inc(REG1, SpxEchoState_last_y_N, 2);

    VoC_lw32inc_p(ACC0, REG0, DEFAULT);
    VoC_loop_i(0, 40)

    VoC_lw32inc_p(ACC0, REG0, DEFAULT);
    VoC_sw32inc_p(ACC0, REG1, DEFAULT);
    VoC_endloop0


    //MIC NOISE SUPPRESS
    VoC_jal(speex_preprocess_run);

    VoC_lw16i_set_inc(REG0,SPEEX_REF_BUF,2);
    VoC_lw16d_set_inc(REG1,GLABAL_NOISESUPPRESS_INPUTADDR_ADDR,2);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);

    VoC_loop_i(0,40)
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_endloop0


    VoC_lw16i_set_inc(REG0,80,2);
    VoC_add16_rd(REG0,REG0,GLABAL_NOISESUPPRESS_INPUTADDR_ADDR);

    VoC_lw16i_set_inc(REG1,SPEEX_REF_BUF,2);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);

    VoC_loop_i(0,40)

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG1, SpxEchoState_last_y_N, 2);
    VoC_lw16i_set_inc(REG0, SpxEchoState_last_y, 2);

    VoC_lw32inc_p(ACC0, REG1, DEFAULT);
    VoC_loop_i(0, 40)

    VoC_lw32inc_p(ACC0, REG1, DEFAULT);
    VoC_sw32inc_p(ACC0, REG0, DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG0, SPEEX_SPEAKER_DELAY_BUFFER+80, 2);
    VoC_lw16i_set_inc(REG1, SpxEchoState_last_y_N, 2);

    VoC_lw32inc_p(ACC0, REG0, DEFAULT);
    VoC_loop_i(0, 40)

    VoC_lw32inc_p(ACC0, REG0, DEFAULT);
    VoC_sw32inc_p(ACC0, REG1, DEFAULT);
    VoC_endloop0


    //MIC NOISE SUPPRESS
    VoC_jal(speex_preprocess_run);

    VoC_lw16i_set_inc(REG0,SPEEX_REF_BUF,2);
    VoC_lw16d_set_inc(REG1,GLABAL_NOISESUPPRESS_INPUTADDR_ADDR,2);

    VoC_lw16i(REG2,80);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG2,IN_PARALLEL);

    VoC_loop_i(0,40)
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_endloop0


    VoC_lw16d_set_inc(REG0,GLABAL_NOISESUPPRESS_SPEAKERADDR_ADDR,2);
    VoC_lw16i(REG1, 6);
    VoC_add16_rr(REG0, REG0, REG1, DEFAULT);
    VoC_lw16i_set_inc(REG1,SPEEX_SPEAKER_DELAY_BUFFER,2);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_loop_i(0,77)
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_endloop0


    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_NOP();
    VoC_return;
}
/****************************
 * input :  st->echo_state->Y
            st->residual_echo
            N = 2 * st->frame_size
 * output : none
 ****************************/
// power_spectrum(st->Y, residual_echo, N);
void power_spectrum(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);

    VoC_lw16i_set_inc(REG0, SpxEchoState_Y, 1);
    VoC_lw16i_set_inc(REG1, SpxPpState_residual_echo, 2);

    // ps[0]=MULT16_16(X[0],X[0]);
    VoC_lw16inc_p(REG5, REG0, DEFAULT);
    VoC_multf32_rr(ACC0, REG5, REG5, DEFAULT);
    VoC_NOP();
    VoC_NOP();
    VoC_sw32inc_p(ACC0, REG1, DEFAULT);
    /*
    for (i=1,j=1;i<N-1;i+=2,j++)
    {
        ps[j] =  MULT16_16(X[i],X[i]) + MULT16_16(X[i+1],X[i+1]);
    }
    */
    VoC_lw16_d(REG4, SpxPpState_frame_size);
    //VoC_shr16_ri(REG4, -1, DEFAULT);
    VoC_sub16_rd(REG4, REG4, SPEEX_CONST_2);
    //VoC_NOP();

    VoC_loop_r_short(REG4, DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG5, REG0, DEFAULT);
    VoC_lw32z(ACC0, IN_PARALLEL);

    VoC_mac32_rr(REG5, REG5, DEFAULT);
    //VoC_NOP();

    VoC_lw16inc_p(REG5, REG0, DEFAULT);
    VoC_mac32_rr(REG5, REG5, DEFAULT);

    VoC_NOP();
    VoC_NOP();

    VoC_sw32inc_p(ACC0, REG1, DEFAULT);
    VoC_endloop0

    // ps[j]=MULT16_16(X[i],X[i]);
    VoC_lw16inc_p(REG5, REG0, DEFAULT);
    VoC_multf32_rr(ACC0, REG5, REG5, DEFAULT);

    VoC_NOP();
    VoC_NOP();
    VoC_sw32inc_p(ACC0, REG1, DEFAULT);

    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/*************************
 * input :  *st: st->echo_state
            *residual_echo
   output:  none
 *************************/

void speex_echo_get_residual(void)
{
    VoC_push16(RA, DEFAULT);

    VoC_lw16_d(REG4, SpxPpState_frame_size);
    VoC_shr16_ri(REG4, -1, DEFAULT);

    /* Apply hanning window (should pre-compute it)*/
    /*
    for (i=0;i<N;i++)
        st->y[i] = MULT16_16_Q15(st->window[i],st->last_y[i]);
    */
    VoC_lw16i_set_inc(REG0, SpxEchoState_window, 1);
    VoC_lw16i_set_inc(REG1, SpxEchoState_last_y, 1);
    VoC_lw16i_set_inc(REG2, SpxEchoState_y, 1);

    VoC_lw16i_short(FORMAT16, 0, DEFAULT);

    VoC_loop_r_short(REG4, DEFAULT);
    VoC_lw16i_short(FORMAT32, 15, IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG5, REG0, DEFAULT);
    VoC_multf32inc_rp(REG67, REG5, REG1, DEFAULT);

    VoC_NOP();
    VoC_mult16_rd(REG6,REG6,GLABAL_NOISESUPPRESS_SPEAKER_GAIN_ADDR);
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_bnlt16_rd(speex_echo_get_residual_L0,REG7,GLABAL_NOISESUPPRESS_SPEAKER_GAIN_ADDR);
    VoC_movreg16(REG7,REG6,DEFAULT);
    VoC_multf16_rr(REG7,REG7,REG7,DEFAULT);
    VoC_NOP();
    VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_NOP();
speex_echo_get_residual_L0:
    VoC_NOP();
    VoC_sw16inc_p(REG6, REG2, DEFAULT);
    VoC_endloop0

    /* Compute power spectrum of the echo */
    //spx_fft(st->fft_table, st->y, st->Y);
    VoC_lw16i(REG0, SpxEchoState_y);
    VoC_lw16i(REG1, SpxEchoState_Y);
    VoC_sw16_d(REG0, SpxPpState_fft_in_ADDR);
    VoC_sw16_d(REG1, SpxPpState_fft_out_ADDR);
    VoC_jal(spx_fft);

    //power_spectrum(st->Y, residual_echo, N);
    VoC_jal(power_spectrum);

    /*
    if (st->leak_estimate > 16383)
        leak2 = 32767;
    else
        leak2 = SHL16(st->leak_estimate, 1);
    */
    VoC_lw16_d(REG2, GLABAL_NOISESUPPRESS_LEAK_ESTIMATE_ADDR);
//  VoC_lw16_d(REG4, GLABAL_NOISESUPPRESS_LEAK_ESTIMATE_ADDR);
//  VoC_lw16i(REG5, 16383);
//  VoC_lw16i(REG2, 0x7fff);
//  VoC_bgt16_rr(SPX_ECHO_LEAK_ESTIMATE, REG4, REG5)
//      VoC_shr16_ri(REG4, -1, DEFAULT);
//      VoC_movreg16(REG2, REG4, DEFAULT);
//SPX_ECHO_LEAK_ESTIMATE:

    // leak2 in reg5
    /* Estimate residual echo */
    /*
    for (i=0;i<=st->frame_size;i++)
        residual_echo[i] = (spx_int32_t)MULT16_32_Q15(leak2,residual_echo[i]);
    */
    VoC_lw16i_set_inc(REG0, SpxPpState_residual_echo, 2);
    VoC_lw16i_set_inc(REG1, SpxPpState_residual_echo, 2);
    //VoC_lw16_d(REG4, SpxPpState_frame_size);
    VoC_lw16_d(REG6, SpxPpState_frame_size);

    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_lw16i_short(FORMATX, 15, IN_PARALLEL);
    /*
    VoC_loop_r_short(REG4, DEFAULT);
    VoC_startloop0
        VoC_lw32_p(REG45, REG0, DEFAULT);

        VoC_shr32_ri(REG45, -1, DEFAULT);
        VoC_shru16_ri(REG4, 1, DEFAULT);

        VoC_multf32_rr(ACC0, REG2, REG5, DEFAULT);
        VoC_macX_rr(REG2, REG4, DEFAULT);

        VoC_NOP();
        VoC_NOP();

        VoC_sw32inc_p(ACC0, REG0, DEFAULT);
    VoC_endloop0
    */
    VoC_lw32inc_p(REG45, REG1, DEFAULT);

    VoC_loop_r_short(REG6, DEFAULT);
    VoC_shr32_ri(REG45, -1, IN_PARALLEL);
    VoC_startloop0

    VoC_shru16_ri(REG4, 1, DEFAULT);
    VoC_multf32_rr(ACC0, REG2, REG5, IN_PARALLEL);

    VoC_macX_rr(REG2, REG4, DEFAULT);

    VoC_lw32inc_p(REG45, REG1, DEFAULT);

    VoC_shr32_ri(REG45, -1, DEFAULT);

    VoC_sw32inc_p(ACC0, REG0, DEFAULT);
    VoC_endloop0


//  VoC_lw16_d(REG2,GLABAL_NOISESUPPRESS_SPEAKER_GAIN_ADDR);

//    VoC_lw16i(REG2,2);
//
//  VoC_lw16i_set_inc(REG0, SpxPpState_residual_echo, 2);
//  VoC_lw16i_set_inc(REG1, SpxPpState_residual_echo, 2);
//
//  VoC_lw16_d(REG6, SpxPpState_frame_size);
//
//  VoC_lw16i_short(FORMAT32, -15, DEFAULT);
//  VoC_lw16i_short(FORMATX, 0, IN_PARALLEL);
//
//  VoC_lw32inc_p(REG45, REG1, DEFAULT);
//
//  VoC_loop_r_short(REG6, DEFAULT);
//  VoC_shr32_ri(REG45,-1,IN_PARALLEL);
//  VoC_startloop0
//
//      VoC_shru16_ri(REG4,1,DEFAULT);
//      VoC_multf32_rr(ACC0, REG2, REG5, IN_PARALLEL);
//
//      VoC_macX_rr(REG2, REG4, DEFAULT);
//
//      VoC_lw32inc_p(REG45, REG1, DEFAULT);
//
//      VoC_shr32_ri(REG45,-1,DEFAULT);
//
//      VoC_sw32inc_p(ACC0, REG0, DEFAULT);
//      VoC_endloop0



    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}


void speex_preprocess_run(void)
{
    VoC_push16(RA, DEFAULT);

    VoC_bez16_d(speex_preprocess_run_L0,GLABAL_NOISESUPPRESS_RESET_ADDR);
    VoC_jal(Rda_speex_preprocess_state_reset);
speex_preprocess_run_L0:
    /*
    st->nb_adapt++;
    if (st->nb_adapt>20000)
        st->nb_adapt = 20000;
    st->min_count++;
    */
    VoC_lw16_d(REG4, SpxPpState_nb_adapt);
    VoC_lw16_d(REG5, SpxPpState_min_count);
    VoC_add16_rd(REG4, REG4, SPEEX_CONST_1);
    VoC_add16_rd(REG5, REG5, SPEEX_CONST_1);

    VoC_bngt16_rd(SPEEX_PRE_NB_ADAPT_NGT_20000, REG4, SPEEX_CONST_20000)
    VoC_lw16i(REG4, 20000);
SPEEX_PRE_NB_ADAPT_NGT_20000:
    VoC_sw16_d(REG5, SpxPpState_min_count);
    VoC_sw16_d(REG4, SpxPpState_nb_adapt);

    VoC_lw16_d(REG4, SpxPpState_ps_size); // N in reg4
    VoC_lw16_d(REG5, SpxPpState_nbands); // M in reg5

    VoC_add16_rr(REG5, REG5, REG4, DEFAULT); // N+M in REG5
    //VoC_push16(REG5, DEFAULT);

    /*
    if (st->echo_state)
    {
      speex_echo_get_residual(st->echo_state, st->residual_echo, N);
      for (i=0;i<N;i++)
         st->echo_noise[i] = MAX32(MULT16_32_Q15(QCONST16(.6f,15),st->echo_noise[i]), st->residual_echo[i]);
      filterbank_compute_bank32(st->bank, st->echo_noise, st->echo_noise+N);
    } else {
      for (i=0;i<N+M;i++)
         st->echo_noise[i] = 0;
    }
    */
    VoC_bez16_d(SPEEX_PRE_ECHO_STATE_EZ, GLOBAL_NOISESUPPRESS_ECHO_STATE_ADDR)
    VoC_jal(speex_echo_get_residual);

    VoC_lw16i_set_inc(REG0, SpxPpState_echo_noise, 2);
    VoC_lw16i_set_inc(REG1, SpxPpState_residual_echo, 2);

    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_lw16i_short(FORMATX, 15, IN_PARALLEL);

    VoC_lw16_d(REG4, SpxPpState_frame_size);
    VoC_loop_r_short(REG4, DEFAULT);
    VoC_startloop0
    VoC_lw32_p(REG45, REG0, DEFAULT);
    VoC_lw16i(REG2, 19661);

    /*
    VoC_movreg32(REG67, REG45, DEFAULT);
    VoC_shr32_ri(REG45, 15, IN_PARALLEL); // high in reg4

    VoC_and32_rd(REG67, SPEEX_CONST_32767); // low in reg6
    */
    VoC_shr32_ri(REG45, -1, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);

    VoC_multf32_rr(ACC0, REG2, REG5, DEFAULT);

    VoC_macX_rr(REG2, REG4, DEFAULT);
    VoC_lw32inc_p(ACC1, REG1, IN_PARALLEL);

    VoC_NOP();
    VoC_bgt32_rr(SPEEX_PRE_ECHO_NOISE_MAX, ACC1, ACC0)
    VoC_movreg32(ACC1, ACC0, DEFAULT);
    VoC_NOP();
SPEEX_PRE_ECHO_NOISE_MAX:
    VoC_sw32inc_p(ACC1, REG0, DEFAULT);
    VoC_endloop0

    VoC_lw16i(REG4, SpxPpState_echo_noise);
    VoC_lw16i(REG5, SpxPpState_echo_noise_M);
    VoC_jal(filterbank_compute_bank32);

    VoC_jump(SPEEX_PRE_ECHO_STATE_END);
SPEEX_PRE_ECHO_STATE_EZ:
    VoC_lw16i_set_inc(REG1, SpxPpState_echo_noise, 2);
    VoC_lw32z(RL7, DEFAULT);
    VoC_loop_r_short(REG5, DEFAULT);
    VoC_startloop0
    VoC_sw32inc_p(RL7, REG1, DEFAULT);
    VoC_endloop0

SPEEX_PRE_ECHO_STATE_END:
    VoC_jal(preprocess_analysis);

    VoC_jal(update_noise_prob);

    /*
    beta = MAX16(983,DIV32_16(Q15_ONE,st->nb_adapt));
    beta_1 = Q15_ONE-beta;
    */
    VoC_lw32_d(RL6, SPEEX_CONST_32767);
    VoC_lw16_d(REG4, SpxPpState_nb_adapt);
    VoC_lw16i_short(REG5, 0, DEFAULT);
    VoC_movreg32(RL7, REG45, DEFAULT);
    VoC_jal(speex_div32_16); // result in REG01

    //VoC_bgt16_rd(SPEEX_PRE_GT_983, REG0, SPEEX_CONST_3277)
    VoC_bgt16_rd(SPEEX_PRE_GT_983, REG0, SPEEX_CONST_983)
    VoC_lw16i(REG0, 983);
SPEEX_PRE_GT_983:
    VoC_sub16_dr(REG7, SPEEX_CONST_32767, REG0); // beta1 in REG7, beta in reg0

    /*
    for (i=0;i<N;i++)
    {
      spx_word32_t tmp;
      tmp = PSHR32(st->noise[i], NOISE_SHIFT);
      if (!st->update_prob[i] || st->ps[i] < tmp)
      {
         spx_word32_t tmp2, tmp3;
         tmp2 = SHL32(st->ps[i],NOISE_SHIFT);
         tmp2 = MULT16_32_Q15(beta, tmp2);
         tmp3 = MULT16_32_Q15(beta_1,st->noise[i]);
         tmp3 = tmp3 + tmp2;
         st->noise[i] = MAX32(EXTEND32(0),tmp3);
      }
    }
    */
    VoC_lw16_d(REG4, SpxPpState_ps_size);
    VoC_lw16i_set_inc(REG1, SpxPpState_noise, 2);
    VoC_lw16i_set_inc(REG2, SpxPpState_update_prob, 1);
    VoC_lw16i_set_inc(REG3, SpxPpState_ps, 2);

    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_lw16i_short(FORMATX, 15, IN_PARALLEL);

    VoC_loop_r_short(REG4, DEFAULT);
    VoC_startloop0
    VoC_push16(REG7, DEFAULT);
    VoC_push16(REG0, DEFAULT);
    //if (REGS[1].reg == 0x19be)
    //  VoC_NOP();

    VoC_lw32_p(RL7, REG1, DEFAULT); // RL7 = st->noise[i]
    VoC_lw32_d(RL6, SPEEX_CONST_1);

    VoC_shr32_ri(RL6, NEG_NOISE_SHIFT_1, DEFAULT);
    VoC_add32_rr(RL7, RL7, RL6, DEFAULT);

    VoC_shr32_ri(RL7, NOISE_SHIFT, DEFAULT);

    VoC_lw16inc_p(REG5, REG2, DEFAULT);
    VoC_lw32inc_p(RL6, REG3, DEFAULT);

    VoC_bez16_r(SPEEX_PRE_UPDATE_PROB_EZ, REG5)
    VoC_bgt32_rr(SPEEX_PRE_UPDATE_PROB_EZ, RL7, RL6)
    VoC_lw32_p(ACC0, REG1, DEFAULT);
    VoC_jump(SPEEX_PRE_NOISE_EST_END);
SPEEX_PRE_UPDATE_PROB_EZ:
    // do something
    //if (reg_LOOP[0] == 0x0001)
    //  VoC_NOP();
    VoC_shr32_ri(RL6, NEG_NOISE_SHIFT, DEFAULT);
    VoC_movreg32(REG45, RL6, DEFAULT); // beta in reg0

    //VoC_jal(spx_mult16_32_q15);
    VoC_shr32_ri(REG45, -1, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);

    VoC_multf32_rr(ACC0, REG5, REG0, DEFAULT);
    VoC_macX_rr(REG4, REG0, DEFAULT);
    VoC_NOP();

    //if (REGL[0])
    //  VoC_NOP();
    //VoC_push32(ACC0, DEFAULT); // push result
    VoC_movreg32(RL6, ACC0, DEFAULT);

    VoC_lw32_p(REG45, REG1, DEFAULT);
    VoC_movreg16(REG0, REG7, IN_PARALLEL); // beta1 in reg7

    //VoC_jal(spx_mult16_32_q15);
    VoC_shr32_ri(REG45, -1, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);

    VoC_multf32_rr(ACC0, REG5, REG0, DEFAULT);
    VoC_macX_rr(REG4, REG0, DEFAULT);
    VoC_NOP();
    //if (REGL[0])
    //  VoC_NOP();

    //VoC_movreg32(ACC1, ACC0, DEFAULT);
    //VoC_pop32(ACC0, DEFAULT);
    VoC_add32_rr(ACC0, ACC0, RL6, DEFAULT);

    VoC_bgtz32_r(SPEEX_PRE_NOISE_EST_END, ACC0)
    VoC_lw32z(ACC0, DEFAULT);
    //VoC_NOP();
SPEEX_PRE_NOISE_EST_END:
    VoC_sw32inc_p(ACC0, REG1, DEFAULT);

    VoC_pop16(REG0, DEFAULT);
    VoC_pop16(REG7, DEFAULT);
    //VoC_NOP();
    VoC_endloop0

    VoC_lw16i(REG4, SpxPpState_noise);
    VoC_lw16i(REG5, SpxPpState_noise_M);
    VoC_jal(filterbank_compute_bank32);

    /*
    if (st->nb_adapt==1)
        for (i=0;i<N+M;i++)
            st->old_ps[i] = ps[i];
    */
    VoC_lw16_d(REG4, SpxPpState_nb_adapt);
    VoC_lw16i(REG5, SPEEX_N_PLUS_M);
    //VoC_pop16(REG5, DEFAULT);
    VoC_bne16_rd(SPEEX_PRE_NB_ADAPT_E_1, REG4, SPEEX_CONST_1)
    VoC_lw16i_set_inc(REG0, SpxPpState_ps, 2);
    VoC_lw16i_set_inc(REG1, SpxPpState_old_ps, 2);
    VoC_loop_r_short(REG5, DEFAULT);
    VoC_lw32inc_p(REG67, REG0, IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG67, REG0, DEFAULT);
    // sw before REG67 update
    VoC_sw32inc_p(REG67, REG1, DEFAULT);
    VoC_endloop0
SPEEX_PRE_NB_ADAPT_E_1:
    VoC_lw16i_set_inc(REG0, SpxPpState_noise, 2);
    VoC_lw16i_set_inc(REG1, SpxPpState_echo_noise, 2);
    VoC_lw16i_set_inc(REG2, SpxPpState_reverb_estimate, 2);
    VoC_lw16i_set_inc(REG3, SpxPpState_ps, 2);
    VoC_lw16i_short(REG4, 0, DEFAULT);
    VoC_loop_r(0, REG5)
    VoC_push16(REG4, DEFAULT);
    //VoC_lw32_d(RL6, SPEEX_CONST_1);
    VoC_lw16i_short(RL6_LO, 1, DEFAULT);
    VoC_lw16i_short(RL6_HI, 0, IN_PARALLEL);
    VoC_lw32inc_p(RL7, REG0, DEFAULT);

    // spx_word32_t tot_noise = ADD32(ADD32(ADD32(EXTEND32(1), PSHR32(st->noise[i],NOISE_SHIFT)) , st->echo_noise[i]) , st->reverb_estimate[i]);
    VoC_shr32_ri(RL6, NEG_NOISE_SHIFT_1, IN_PARALLEL);
    VoC_add32_rr(RL7, RL7, RL6, DEFAULT);
    VoC_shr32_ri(RL7, NOISE_SHIFT, DEFAULT);
    VoC_add32_rd(RL7, RL7, SPEEX_CONST_1);

    VoC_lw32inc_p(RL6, REG1, DEFAULT);
    VoC_lw32inc_p(REG67, REG2, DEFAULT);

    VoC_add32_rr(RL6, RL6, REG67, DEFAULT);
    VoC_add32_rr(RL7, RL7, RL6, DEFAULT); // rl7 = tot_noise
    VoC_push32(REG01, IN_PARALLEL);
    /*
    st->post[i] = SUB16(DIV32_16_Q8(ps[i],tot_noise), QCONST16(1.f,SNR_SHIFT));
    st->post[i] = MIN16(st->post[i], QCONST16(100.f,SNR_SHIFT));
    */
    VoC_lw32inc_p(RL6, REG3, DEFAULT);
    //VoC_push32(REG01, IN_PARALLEL);
    VoC_jal(spx_DIV32_16_Q8);

    //VoC_lw32_d(RL6, SPEEX_CONST_1);
    VoC_lw16i_short(RL6_LO, 1, DEFAULT);
    VoC_lw16i_short(RL6_HI, 0, IN_PARALLEL);
    VoC_shr32_ri(RL6, NEG_SNR_SHIFT, DEFAULT);
    VoC_sub32_rr(REG01, REG01, RL6, DEFAULT);

    VoC_lw16_d(REG7, SPEEX_CONST_100);
    VoC_shr16_ri(REG7, NEG_SNR_SHIFT, DEFAULT);

    VoC_lw16i(REG5, SpxPpState_post);
    VoC_add16_rr(REG5, REG5, REG4, DEFAULT); // r5 point to st->post[]
    VoC_bgt16_rr(SPEEX_PRE_POST, REG7, REG0)
    VoC_movreg16(REG0, REG7, DEFAULT);
    VoC_NOP();
SPEEX_PRE_POST:
    VoC_push16(REG0, DEFAULT);
    VoC_sw16_p(REG0, REG5, DEFAULT);

    //VoC_pop32(REG01, IN_PARALLEL);
    //VoC_push16(REG6, DEFAULT);

    VoC_lw16i(REG5, SpxPpState_old_ps);
    VoC_add16_rr(REG7, REG4, REG4, DEFAULT);
    //VoC_add16_rr(REG7, REG7, REG4, DEFAULT);

    VoC_add16_rr(REG5, REG5, REG7, DEFAULT); // r5 POINT to st->old_ps
    VoC_NOP();
    VoC_lw32_p(RL6, REG5, DEFAULT); // RL6 = st->old_ps[i]

    //VoC_push32(RL7, IN_PARALLEL);
    VoC_add32_rr(RL7, RL6, RL7, DEFAULT);
    VoC_jal(spx_DIV32_16_Q15);
    //VoC_pop32(RL7, DEFAULT);

    //VoC_lw32z(ACC0, DEFAULT);
    //VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_lw16i_short(FORMAT32, 15, DEFAULT);
    //VoC_lw16i_short(FORMATX, 0, IN_PARALLEL);

    //VoC_mac32_rr(REG0, REG0, DEFAULT);
    VoC_multf32_rr(REG45, REG0, REG0, DEFAULT);
    //VoC_NOP();
    //VoC_movreg32(REG45, ACC0, DEFAULT);

    //VoC_lw32z(ACC0, DEFAULT);
    //VoC_mac32_rd(REG4, SPEEX_CONST_29164);
    //VoC_NOP();
    //VoC_movreg32(REG45, ACC0, DEFAULT);
    VoC_NOP();
    VoC_multf32_rd(REG45, REG4, SPEEX_CONST_29164);
    VoC_NOP();

    VoC_add16_rd(REG4, REG4, SPEEX_CONST_3277); // now gamma in REG4

    /*
    tmp = MULT16_16(gamma,MAX16(0,st->post[i]));
    tmp2 = DIV32_16_Q8(st->old_ps[i],tot_noise);
    tmp2 = MULT16_16(Q15_ONE-gamma,tmp2);
    st->prior[i] = EXTRACT16(PSHR32(ADD32(tmp, tmp2), 15));
    st->prior[i]=MIN16(st->prior[i], QCONST16(100.f,SNR_SHIFT));
    */
    VoC_pop16(REG0, DEFAULT);
    VoC_bgtz16_r(SPEEX_PRE_POST_GTZ, REG0)
    VoC_lw16i_short(REG0, 0, DEFAULT);
SPEEX_PRE_POST_GTZ:
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    VoC_mac32_rr(REG4, REG0, DEFAULT); // tmp in ACC0

    //VoC_NOP();
    VoC_sub32_rr(RL7, RL7, RL6, IN_PARALLEL); // st->old_ps[i] in RL6 tot_noise in RL7
    VoC_jal(spx_DIV32_16_Q8); // tmp2 in reg01
    VoC_sub16_dr(REG5, SPEEX_CONST_32767, REG4);

    VoC_mac32_rr(REG5, REG0, DEFAULT); // now ACC0 = ADD32(tmp, tmp2)
    VoC_NOP();

    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT);

    VoC_movreg32(REG45, ACC0, DEFAULT); // st->prior[i] in reg4

    VoC_lw16i(REG5, 100);
    VoC_shr16_ri(REG5, NEG_SNR_SHIFT, DEFAULT);

    VoC_movreg16(REG6, REG4, IN_PARALLEL);
    VoC_bgt16_rr(SPEEX_PRE_PRIOR_END, REG5, REG4)
    //VoC_movreg16(REG4, REG5, DEFAULT);
    VoC_movreg16(REG6, REG5, DEFAULT);
SPEEX_PRE_PRIOR_END:
    //VoC_movreg16(REG6, REG4, DEFAULT);

    VoC_pop32(REG01, DEFAULT);
    VoC_pop16(REG4, IN_PARALLEL);

    VoC_lw16i(REG5, SpxPpState_prior);
    VoC_add16_rr(REG5, REG5, REG4, DEFAULT);

    VoC_add16_rd(REG4, REG4, SPEEX_CONST_1);
    VoC_sw16_p(REG6, REG5, DEFAULT);

    //printf("st->prior[%d]: 0x%x\t &st->prior[%d]: 0x%x\n", REGS[4].reg, REGS[6].reg, REGS[4].reg, REGS[5].reg);
    //if (REGS[5].reg == 0xc3d)
    //  VoC_NOP();
    VoC_endloop0

    /*
    st->zeta[0] = PSHR32(ADD32(MULT16_16(22938,st->zeta[0]), MULT16_16(9830,st->prior[0])),15);
    for (i=1;i<N-1;i++)
        st->zeta[i] = PSHR32(ADD32(ADD32(ADD32(MULT16_16(22938,st->zeta[i]), MULT16_16(4915,st->prior[i])),
                           MULT16_16(2458,st->prior[i-1])), MULT16_16(2458,st->prior[i+1])),15);
    for (i=N-1;i<N+M;i++)
        st->zeta[i] = PSHR32(ADD32(MULT16_16(22938,st->zeta[i]), MULT16_16(9830,st->prior[i])),15);
    */
    VoC_lw16i_set_inc(REG0, SpxPpState_prior, 1);
    VoC_lw16i_set_inc(REG1, SpxPpState_zeta, 1);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16_p(REG4,REG0,IN_PARALLEL);

    VoC_lw16_p(REG5,REG1,DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);

    VoC_mac32_rd(REG4, SPEEX_CONST_9830);
    //VoC_NOP();
    VoC_mac32_rd(REG5, SPEEX_CONST_22938);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_movreg32(REG45, ACC0, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG4, REG1, DEFAULT);

    VoC_lw16_d(REG6, SpxPpState_ps_size);
    VoC_sub16_rd(REG6, REG6, SPEEX_CONST_2);
    VoC_loop_r_short(REG6, DEFAULT);
    VoC_startloop0
    //printf("REG0: 0x%x\n", REGS[0].reg);
    VoC_lw32z(ACC0, DEFAULT);

    VoC_lw16_p(REG5,REG1,DEFAULT);

    VoC_mac32_rd(REG5, SPEEX_CONST_22938);

    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_mac32_rd(REG4, SPEEX_CONST_2458);

    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_mac32_rd(REG4, SPEEX_CONST_4915);

    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_mac32_rd(REG4, SPEEX_CONST_2458);
    VoC_NOP();

    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_movreg32(REG45, ACC0, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG4, REG1, DEFAULT);
    VoC_sub16_rd(REG0, REG0, SPEEX_CONST_2);
    VoC_endloop0

    VoC_lw16_d(REG7, SpxPpState_nbands);
    VoC_add16_rd(REG7, REG7, SPEEX_CONST_1);
    VoC_lw32z(ACC0, DEFAULT);
    VoC_loop_r_short(REG7, DEFAULT);
    VoC_inc_p(REG0, IN_PARALLEL);
    VoC_startloop0
    //printf("REG0: 0x%x\n", REGS[0].reg);
    //VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16_p(REG5,REG1,DEFAULT);


    VoC_mac32_rd(REG5, SPEEX_CONST_22938);

    VoC_lw16inc_p(REG4,REG0,DEFAULT);


    VoC_mac32_rd(REG4, SPEEX_CONST_9830);
    VoC_NOP();

    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_movreg32(REG45, ACC0, DEFAULT);
    //VoC_NOP();
    VoC_lw32z(ACC0, DEFAULT);
    VoC_sw16inc_p(REG4, REG1, DEFAULT);
    //VoC_NOP();
    VoC_endloop0

    /*
    Zframe = 0;
    for (i=N;i<N+M;i++)
        Zframe = ADD32(Zframe, EXTEND32(st->zeta[i]));
    Pframe = 3277+MULT16_16_Q15(29458,qcurve(DIV32_16(Zframe,st->nbands)));
    */
    VoC_lw16i_set_inc(REG1, SpxPpState_zeta_M, 1);
    VoC_lw16_d(REG7, SpxPpState_nbands);
    VoC_lw32z(RL7, DEFAULT);
    VoC_lw32z(RL6, IN_PARALLEL); // Zframe in RL6
    VoC_loop_r_short(REG7, DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG4, REG1, DEFAULT);
    VoC_lw16i_short(REG5, 0, IN_PARALLEL);
    //VoC_NOP();
    VoC_bnltz16_r(SPEEX_PRE_EXTEND_ZETA_M, REG4)
    VoC_lw16i(REG5, 0xffff);
SPEEX_PRE_EXTEND_ZETA_M:
    VoC_add32_rr(RL6, RL6, REG45, DEFAULT);
    VoC_endloop0

    VoC_lw16_d(RL7_LO, SpxPpState_nbands);
    VoC_jal(speex_div32_16);
    VoC_jal(spx_qcurve);

    //VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 15, DEFAULT);

    //VoC_mac32_rd(REG0, SPEEX_CONST_29458);
    VoC_multf32_rd(REG45, REG0, SPEEX_CONST_29458);
    VoC_NOP();
    //VoC_movreg32(REG45, ACC0, DEFAULT);
    VoC_add16_rd(REG4, REG4, SPEEX_CONST_3277);// now Pframe in reg4
    VoC_NOP();
    VoC_sw16_d(REG4, SpxPpState_speech_prob);

    // above works as intended
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    VoC_mac32_rd(REG4, SpxPpState_echo_suppress_active);

    VoC_sub16_dr(REG4, SPEEX_CONST_32767, REG4);
    VoC_mac32_rd(REG4, SpxPpState_echo_suppress);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_movreg32(REG45, ACC0, DEFAULT); // now effective_echo_suppress in reg4

    VoC_jal(compute_gain_floor);

    VoC_lw16_d(REG6, SpxPpState_ps_size);
    VoC_lw16_d(REG7, SpxPpState_nbands);
    VoC_lw16i_set_inc(REG0, SpxPpState_gain_M, 1);
    VoC_lw16i_set_inc(REG1, SpxPpState_old_ps_M, 2);
    VoC_lw16i_set_inc(REG2, SpxPpState_prior_M, 1);
    VoC_lw16i_set_inc(REG3, SpxPpState_post_M, 1);

    //VoC_lw32z(RL7, DEFAULT);
    //VoC_lw32z(RL6, IN_PARALLEL);
    VoC_lw16i_short(REG6, 0, DEFAULT);
    VoC_loop_r(0, REG7)
    //printf("REG0: 0x%x\tREG1: 0x%x\tREG2: 0x%x\tREG3: 0x%x\n", REGS[0].reg, REGS[1].reg, REGS[2].reg, REGS[3].reg);
    VoC_push16(REG6, DEFAULT);
    VoC_push32(REG01, IN_PARALLEL);

    //VoC_lw32_d(RL7, SPEEX_CONST_1);
    VoC_lw16i_short(RL7_LO, 1, DEFAULT);
    VoC_lw16i_short(RL7_HI, 0, IN_PARALLEL);
    VoC_shr32_ri(RL7, NEG_SNR_SHIFT, DEFAULT);

    VoC_lw16_p(REG4, REG2, DEFAULT);
    VoC_lw16i_short(REG5, 0, IN_PARALLEL);
    VoC_bnltz16_r(SPEEX_PRE_EXTEND_PRIOR_M, REG4)
    VoC_lw16i(REG5, 0xffff);
SPEEX_PRE_EXTEND_PRIOR_M:
    VoC_movreg32(RL6, REG45, DEFAULT);

    VoC_add32_rr(RL7, REG45, RL7, DEFAULT);
    VoC_shr32_ri(RL6, -15, IN_PARALLEL);

    VoC_movreg32(REG45, RL7, DEFAULT);
    VoC_shr32_ri(REG45, 1, DEFAULT);
    VoC_add32_rr(RL6, RL6, REG45, DEFAULT);
    VoC_jal(speex_div32_16); // now prior_ratio in REG0
    VoC_push16(REG0, DEFAULT);

    VoC_lw16inc_p(REG4, REG3, DEFAULT);
    VoC_lw16i_short(REG5, 0, IN_PARALLEL);
    VoC_bnltz16_r(SPEEX_PRE_EXTEND_POST_M, REG4)
    VoC_lw16i(REG5, 0xffff);
SPEEX_PRE_EXTEND_POST_M:
    VoC_shr32_ri(REG45, NEG_EXPIN_SNR_SHIFT, DEFAULT);
    //VoC_lw32_d(RL6, SPEEX_CONST_1);

    VoC_lw16i_short(RL6_LO, 1, DEFAULT);
    VoC_lw16i_short(RL6_HI, 0, IN_PARALLEL);
    VoC_shr32_ri(RL6, NEG_EXPIN_SHIFT, DEFAULT);
    VoC_add32_rr(REG45, REG45, RL6, DEFAULT);

    VoC_jal(spx_mult16_32_p15); // result in ACC0
    VoC_movreg32(RL6, ACC0, DEFAULT); // now theta in RL6
    VoC_jal(hypergeom_gain); // result in reg01

    VoC_movreg32(REG45, REG01, DEFAULT);
    VoC_pop16(REG0, DEFAULT);
    //VoC_jal(spx_mult16_32_q15); // result in ACC0

    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_lw16i_short(FORMATX, 15, IN_PARALLEL);

    VoC_shr32_ri(REG45, -1, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);

    VoC_multf32_rr(ACC0, REG5, REG0, DEFAULT);
    VoC_macX_rr(REG4, REG0, DEFAULT);
    VoC_NOP();

    VoC_blt32_rd(SPEEX_PRE_GAIN, ACC0, SPEEX_CONST_32767)
    VoC_lw32_d(ACC0, SPEEX_CONST_32767);
SPEEX_PRE_GAIN:
    VoC_movreg32(REG45, ACC0, DEFAULT);
    VoC_pop32(REG01, IN_PARALLEL);
    VoC_NOP();
    VoC_sw16inc_p(REG4, REG0, DEFAULT); // save st->gain[i]
    VoC_push32(REG01, DEFAULT);

    /*
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 15, IN_PARALLEL);
    VoC_mac32_rr(REG4, REG4, DEFAULT);
    VoC_NOP();
    VoC_movreg32(REG45, ACC0, DEFAULT);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    VoC_mac32_rd(REG4, SPEEX_CONST_26214);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    */
    VoC_lw16i_short(FORMAT32, 15, DEFAULT);
    VoC_multf32_rr(REG45, REG4, REG4, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_multf32_rd(REG01, REG4, SPEEX_CONST_26214);
    VoC_NOP();
    VoC_add32_rd(REG01, REG01, SPEEX_CONST_16384);
    VoC_shr32_ri(REG01, 15, DEFAULT);

    VoC_pop16(REG6, DEFAULT);
    //VoC_push32(REG01, DEFAULT);

    //VoC_movreg32(REG01, ACC0, DEFAULT);

    VoC_lw16i(REG1, SpxPpState_ps_M);
    VoC_add16_rr(REG4, REG6, REG6, DEFAULT);
    VoC_add16_rr(REG1, REG1, REG4, DEFAULT);
    VoC_NOP();
    VoC_lw32_p(REG45, REG1, DEFAULT);
    VoC_jal(spx_mult16_32_p15); // result in ACC0
    VoC_movreg32(ACC1, ACC0, DEFAULT);
    VoC_pop32(REG01, IN_PARALLEL);
    //VoC_NOP();

    VoC_push16(REG6, DEFAULT);
    VoC_push32(REG01, DEFAULT);

    VoC_lw16i(REG0, 6554);
    VoC_lw32_p(REG45, REG1, DEFAULT);
    VoC_push32(ACC1, DEFAULT);
    VoC_jal(spx_mult16_32_p15);
    VoC_pop32(ACC1, DEFAULT);

    VoC_add32_rr(ACC0, ACC0, ACC1, DEFAULT);
    VoC_pop32(REG01, IN_PARALLEL);
    VoC_NOP();
    VoC_sw32inc_p(ACC0, REG1, DEFAULT); // save st->old_ps[i]

    VoC_push32(REG01, DEFAULT);

    // P1 = 6521+MULT16_16_Q15(26214,qcurve (st->zeta[i]));
    VoC_lw16i(REG1, SpxPpState_zeta_M);
    VoC_add16_rr(REG1, REG1, REG6, DEFAULT);
    VoC_NOP();
    VoC_lw16_p(REG0, REG1, DEFAULT);
    VoC_jal(spx_qcurve); // result in reg0

    /*
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 15, IN_PARALLEL);
    VoC_mac32_rd(REG0, SPEEX_CONST_26214);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_6521);

    VoC_movreg32(REG01, ACC0, DEFAULT); // now P1 in REG01
    */
    VoC_lw16i_short(FORMAT32, 15, DEFAULT);
    VoC_multf32_rd(REG01, REG0, SPEEX_CONST_26214);
    VoC_NOP();
    VoC_add32_rd(REG01, REG01, SPEEX_CONST_6521);

    /*
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 15, IN_PARALLEL);
    VoC_mac32_rd(REG0, SpxPpState_speech_prob);
    VoC_NOP();
    VoC_movreg32(REG01, ACC0, DEFAULT);
    */
    VoC_multf32_rd(REG01, REG0, SpxPpState_speech_prob);
    VoC_NOP();

    VoC_sub16_dr(REG4, SPEEX_CONST_32767, REG0);// now q in REG4

    // theta = MIN32(theta, EXTEND32(32767));
    VoC_blt32_rd(SPEEX_PRE_THETA, RL6, SPEEX_CONST_32767) //theta in RL6
    VoC_lw32_d(RL6, SPEEX_CONST_32767);
SPEEX_PRE_THETA:
    VoC_movreg32(REG01, RL6, DEFAULT);

    VoC_sub16_dr(REG0, SPEEX_CONST_0, REG0);
    VoC_jal(spx_exp); // result in rl7

    VoC_shr32_ri(RL7, 1, DEFAULT);
    VoC_blt32_rd(SPEEX_PRE_TMP_MIN32,  RL7, SPEEX_CONST_32767)
    VoC_lw32_d(RL7, SPEEX_CONST_32767);
SPEEX_PRE_TMP_MIN32:
    // tmp = MULT16_16_Q15((SHL32(1,SNR_SHIFT)+st->prior[i]),EXTRACT16(MIN32(Q15ONE,SHR32(spx_exp(-EXTRACT16(theta)),1))));
    VoC_movreg32(REG01, RL7, DEFAULT);

    VoC_lw16i_short(REG1, 1, DEFAULT);
    VoC_shr16_ri(REG1, NEG_SNR_SHIFT, DEFAULT);
    //VoC_NOP();
    VoC_add16inc_rp(REG1, REG1, REG2, DEFAULT);
    /*
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 15, IN_PARALLEL);
    VoC_mac32_rr(REG0, REG1, DEFAULT);
    VoC_NOP();
    */
    VoC_lw16i_short(FORMAT32, 15, DEFAULT);
    VoC_multf32_rr(REG01, REG0, REG1, DEFAULT);
    VoC_NOP();

    // now tmp in ACC0
    // tmp = MIN16(QCONST16(3.,SNR_SHIFT), tmp);
    //VoC_movreg32(REG01, ACC0, DEFAULT);
    VoC_lw16i_short(REG1, 3, DEFAULT);
    VoC_shr16_ri(REG1, NEG_SNR_SHIFT, DEFAULT);
    VoC_bgt16_rr(SPEEX_PRE_TMP_MIN16, REG1, REG0)
    VoC_movreg16(REG0, REG1, DEFAULT);
    VoC_NOP();
SPEEX_PRE_TMP_MIN16:
    VoC_push16(REG0, DEFAULT); // now tmp in reg0

    // tmp = EXTRACT16(PSHR32(MULT16_16(PDIV32_16(SHL32(EXTEND32(q),8),(Q15_ONE-q)),tmp),8));
    // extend reg4 to RL6
    VoC_lw16i_short(RL6_HI, 0, DEFAULT);
    VoC_movreg16(RL6_LO, REG4, IN_PARALLEL);

    VoC_bnltz16_r(SPEEX_PRE_EXTEND_Q, REG4)
    VoC_lw16i(RL6_HI, 0xffff);
SPEEX_PRE_EXTEND_Q:
    VoC_movreg32(RL7, RL6, DEFAULT);

    VoC_shr32_ri(RL6, -8, DEFAULT); // SHL32(EXTEND32(q),8) rl6
    VoC_sub32_dr(RL7, SPEEX_CONST_32767, RL7); // (Q15_ONE-q) rl7
    VoC_movreg32(REG45, RL7, DEFAULT);
    VoC_shr32_ri(REG45, 1, DEFAULT);
    VoC_add32_rr(RL6, RL6, REG45, DEFAULT);
    VoC_jal(speex_div32_16);

    VoC_movreg32(REG45, REG01, DEFAULT);

    VoC_pop16(REG0, DEFAULT);
    /*
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    VoC_mac32_rr(REG0, REG4, DEFAULT); // MULT16_16(PDIV32_16(SHL32(EXTEND32(q),8),(Q15_ONE-q)),tmp)
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_128);
    VoC_shr32_ri(ACC0, 8, DEFAULT);
    VoC_movreg32(RL7, ACC0, DEFAULT); // tmp in RL7
    //VoC_lw32z(RL7, IN_PARALLEL);
    */
    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_multf32_rr(RL7, REG0, REG4, DEFAULT); // MULT16_16(PDIV32_16(SHL32(EXTEND32(q),8),(Q15_ONE-q)),tmp)
    VoC_NOP();
    VoC_add32_rd(RL7, RL7, SPEEX_CONST_128);
    VoC_shr32_ri(RL7, 8, DEFAULT); // tmp in RL7

    VoC_lw32_d(RL6, SPEEX_CONST_32767);
    VoC_shr32_ri(RL6, NEG_SNR_SHIFT, DEFAULT);
    VoC_add32_rd(RL7, RL7, SPEEX_CONST_256);
    //VoC_jal(speex_div32_16_sign);
    VoC_jal(speex_div32_16);

    VoC_lw16i(REG1, SpxPpState_gain2_M);
    VoC_add16_rr(REG1, REG1, REG6, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG0, REG1, DEFAULT);

    VoC_pop32(REG01, DEFAULT);
    VoC_pop16(REG6, DEFAULT);

    //VoC_pop16(REG6, DEFAULT);
    VoC_add16_rd(REG6, REG6, SPEEX_CONST_1);
    VoC_NOP();
    VoC_endloop0

    VoC_lw16i(REG4, SpxPpState_gain2_M);
    VoC_lw16i(REG5, SpxPpState_gain2);
    VoC_jal(filterbank_compute_psd16);

    VoC_lw16i(REG4, SpxPpState_gain_M);
    VoC_lw16i(REG5, SpxPpState_gain);
    VoC_jal(filterbank_compute_psd16);

    VoC_lw16i(REG4, SpxPpState_gain_floor_M);
    VoC_lw16i(REG5, SpxPpState_gain_floor);
    VoC_jal(filterbank_compute_psd16);

    // above working as intended
    VoC_lw16_d(REG4, SpxPpState_ps_size);
    VoC_lw16i_set_inc(REG0, SpxPpState_prior, 1);
    VoC_lw16i_set_inc(REG1, SpxPpState_gain_floor, 1);
    VoC_lw16i_set_inc(REG2, SpxPpState_gain2, 1);
    VoC_lw16i_set_inc(REG3, SpxPpState_gain, 1);
    //VoC_loop_r_short(REG4, DEFAULT);
    VoC_lw16i_short(REG7, 0, DEFAULT);
    VoC_loop_r(0, REG4)
    VoC_push16(REG7, DEFAULT);
    //printf("REG0: 0x%x\tREG1: 0x%x\tREG2: 0x%x\tREG3: 0x%x\n", REGS[0].reg, REGS[1].reg, REGS[2].reg, REGS[3].reg);
    // prior_ratio = PDIV32_16(SHL32(EXTEND32(st->prior[i]), 15), ADD16(st->prior[i], SHL32(1,SNR_SHIFT)));
    //VoC_lw16i_short(REG6, 1, DEFAULT);
    //VoC_lw16i_short(REG7, 0, IN_PARALLEL);

    //VoC_lw32z(RL6, DEFAULT);
    //VoC_lw32z(RL7, IN_PARALLEL);
    //VoC_lw32_d(RL7, SPEEX_CONST_1);
    VoC_lw16i_short(RL7_LO, 1, DEFAULT);
    VoC_lw16i_short(RL7_HI, 0, IN_PARALLEL);
    VoC_shr32_ri(RL7, NEG_SNR_SHIFT, DEFAULT);
    //VoC_lw16_p(RL6_LO, REG0, IN_PARALLEL);
    VoC_lw16inc_p(REG4, REG0, IN_PARALLEL);
    VoC_lw16i_short(REG5, 0, DEFAULT);
    VoC_bnltz16_r(SPEEX_PRE_EXTEND_PRIOR_N, REG4)
    VoC_lw16i(REG5, 0xffff);
SPEEX_PRE_EXTEND_PRIOR_N:
    VoC_movreg32(RL6, REG45, DEFAULT);
    VoC_add32_rr(RL7, REG45, RL7, DEFAULT);

    VoC_shr32_ri(RL6, -15, DEFAULT);
    //VoC_movreg16(RL7_LO, REG6, IN_PARALLEL);

    VoC_movreg32(REG45, RL7, DEFAULT);
    VoC_shr32_ri(REG45, 1, DEFAULT);
    VoC_add32_rr(RL6, RL6, REG45, DEFAULT);
    VoC_push32(REG01, IN_PARALLEL);
    VoC_jal(speex_div32_16);// prior_ratio in reg0, result in ACC0
    VoC_push16(REG0, DEFAULT);

    // theta = MULT16_32_P15(prior_ratio, QCONST32(1.f,EXPIN_SHIFT)+SHL32(EXTEND32(st->post[i]),EXPIN_SHIFT-SNR_SHIFT));
    //VoC_lw32z(REG45, DEFAULT);
    VoC_lw16i(REG4, SpxPpState_post);

    VoC_add16_rr(REG4, REG4, REG7, DEFAULT);
    VoC_NOP();
    VoC_lw16_p(REG4, REG4, DEFAULT);
    VoC_lw16i_short(REG5, 0, IN_PARALLEL);
    VoC_bnltz16_r(SPEEX_PRE_EXTEND_POST_N, REG4)
    VoC_lw16i(REG5, 0xffff);
SPEEX_PRE_EXTEND_POST_N:
    VoC_shr32_ri(REG45, SNR_EXPIN_SHIFT, DEFAULT);

    VoC_lw32_d(RL7, SPEEX_CONST_1);
    VoC_shr32_ri(RL7, NEG_EXPIN_SHIFT, DEFAULT);
    VoC_add32_rr(REG45, REG45, RL7, DEFAULT);
    VoC_jal(spx_mult16_32_p15); // now theta in ACC0

    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_jal(hypergeom_gain); // now MM in reg01

    // g = EXTRACT16(MIN32(Q15_ONE, MULT16_32_Q15(prior_ratio, MM)));
    VoC_movreg32(REG45, REG01, DEFAULT);

    VoC_pop16(REG0, DEFAULT);
    //VoC_jal(spx_mult16_32_q15); // result in ACC0
    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_lw16i_short(FORMATX, 15, IN_PARALLEL);

    VoC_shr32_ri(REG45, -1, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);

    VoC_multf32_rr(ACC0, REG5, REG0, DEFAULT);
    VoC_macX_rr(REG4, REG0, DEFAULT);
    VoC_NOP();
    VoC_NOP();

    VoC_movreg32(REG01, ACC0, DEFAULT);

    VoC_blt32_rd(SPEEX_PRE_G, REG01, SPEEX_CONST_32767)
    VoC_lw32_d(REG01, SPEEX_CONST_32767);
SPEEX_PRE_G: // now g in reg0
    /*
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 15, IN_PARALLEL);
    VoC_mac32_rd(REG0, SPEEX_CONST_10912);

    VoC_lw16_p(REG6, REG3, DEFAULT);
    VoC_movreg32(REG45, ACC0, DEFAULT);
    */
    VoC_lw16i_short(FORMAT32, 15, DEFAULT);
    VoC_multf32_rd(REG45, REG0, SPEEX_CONST_10912);
    VoC_lw16_p(REG6, REG3, DEFAULT);

    VoC_bngt16_rr(SPEEX_PRE_G2, REG4, REG6)
    VoC_mult16_rd(REG0, REG6, SPEEX_CONST_3);
    VoC_NOP();
    //VoC_movreg16(REG0, REG6, DEFAULT);
SPEEX_PRE_G2:
    VoC_movreg16(REG6, REG0, DEFAULT); // st->gain[i] in REG6

    /*
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_lw16i_short(FORMAT32, 15, DEFAULT);
    VoC_mac32_rr(REG6, REG6, IN_PARALLEL);
    VoC_NOP();
    VoC_movreg32(REG01, ACC1, DEFAULT);
    */
    VoC_multf32_rr(REG01, REG6, REG6, DEFAULT);
    VoC_NOP();

    /*
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_mac32_rd(REG0, SPEEX_CONST_26214);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_movreg32(REG01, ACC0, DEFAULT);
    */
    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_multf32_rd(REG01, REG0, SPEEX_CONST_26214);
    VoC_NOP();
    VoC_add32_rd(REG01, REG01, SPEEX_CONST_16384);
    VoC_shr32_ri(REG01, 15, DEFAULT);

    VoC_pop16(REG7, IN_PARALLEL);
    VoC_lw16i(REG4, SpxPpState_ps);
    VoC_add16_rr(REG5, REG7, REG7, DEFAULT);
    VoC_NOP();
    VoC_push16(REG5, DEFAULT);
    VoC_add16_rr(REG4, REG4, REG5, DEFAULT);
    VoC_NOP();
    VoC_lw32_p(REG45, REG4, DEFAULT);
    //VoC_push16(REG7, DEFAULT);

    VoC_jal(spx_mult16_32_p15); // result in ACC0
    //VoC_push32(ACC0, DEFAULT);
    VoC_movreg32(RL7, ACC0, DEFAULT);

    VoC_lw16i(REG0, 6554);
    VoC_lw16i(REG4, SpxPpState_old_ps);
    VoC_pop16(REG5, DEFAULT);

    VoC_add16_rr(REG4, REG4, REG5, DEFAULT);

    VoC_push16(REG7, DEFAULT);
    VoC_push16(REG4, DEFAULT);

    VoC_lw32_p(REG45, REG4, DEFAULT);


    VoC_jal(spx_mult16_32_p15);
    VoC_add32_rr(RL7, RL7, ACC0, DEFAULT);
    VoC_pop16(REG4, DEFAULT);
    //VoC_pop32(ACC0, DEFAULT);
    //VoC_push16(REG7, IN_PARALLEL);

    VoC_NOP();
    VoC_sw32_p(RL7, REG4, DEFAULT); // save st->old_ps
    VoC_pop32(REG01, DEFAULT);
    VoC_NOP();

    VoC_lw16inc_p(REG7, REG1, DEFAULT); // gain_floor[i] in REG7

    VoC_bgt16_rr(SPEEX_PRE_GAIN_GT_FLOOR, REG6, REG7)
    VoC_movreg16(REG6, REG7, DEFAULT);
SPEEX_PRE_GAIN_GT_FLOOR:
    VoC_NOP();
    VoC_sw16inc_p(REG6, REG3, DEFAULT); // save st->gain[i]
    VoC_push16(REG6, DEFAULT);

    VoC_lw16i_short(RL7_HI, 0, DEFAULT);
    VoC_movreg16(RL7_LO, REG7, IN_PARALLEL);
    VoC_push32(REG01, DEFAULT);
    VoC_shr32_ri(RL7, -15, DEFAULT);

    VoC_movreg32(REG01, RL7, DEFAULT);
    VoC_jal(spx_sqrt); // result in reg0

    // get p in reg7
    VoC_lw16_p(REG7, REG2, DEFAULT);

    VoC_sub16_dr(REG6, SPEEX_CONST_32767, REG7);

    /*
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMATX, 0, DEFAULT);
    VoC_macX_rr(REG0, REG6, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_movreg32(REG45, ACC0, DEFAULT);
    */
    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_multf32_rr(REG45, REG0, REG6, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(REG45, REG45, SPEEX_CONST_16384);
    VoC_shr32_ri(REG45, 15, DEFAULT);

    VoC_pop16(REG6, IN_PARALLEL);

    VoC_lw16i_short(RL7_HI, 0, DEFAULT);
    VoC_movreg16(RL7_LO, REG6, IN_PARALLEL);
    VoC_shr32_ri(RL7, -15, DEFAULT);

    VoC_movreg32(REG01, RL7, DEFAULT);
    VoC_jal(spx_sqrt);


    VoC_lw32z(ACC0, DEFAULT);
    VoC_macX_rr(REG0, REG7, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_add32_rr(REG45, REG45, ACC0, DEFAULT);

    /*
    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_multf32_rr(REG45, REG0, REG7, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(REG45, REG45, SPEEX_CONST_16384);
    VoC_shr32_ri(REG45, 15, DEFAULT);
    */


    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 15, IN_PARALLEL);
    VoC_mac32_rr(REG4, REG4, DEFAULT);
    VoC_NOP();
    VoC_movreg32(REG45, ACC0, DEFAULT);

    /*
    VoC_lw16i_short(FORMAT32, 15, DEFAULT);
    VoC_multf32_rr(REG45, REG4, REG4, DEFAULT);
    */
    VoC_NOP();
    VoC_sw16inc_p(REG4, REG2, DEFAULT);

    VoC_pop32(REG01, IN_PARALLEL);
    VoC_pop16(REG7, DEFAULT);
    VoC_add16_rd(REG7, REG7, SPEEX_CONST_1);
    VoC_NOP();
    VoC_endloop0

    /*
    if (!st->denoise_enabled)
    {
        for (i=0;i<N+M;i++)
            st->gain2[i]=Q15_ONE;
    }
    */
    VoC_bnez16_d(SPEEX_PRE_DENOISE, SpxPpState_denoise_enabled)
    VoC_lw16_d(REG4, SpxPpState_ps_size);
    VoC_lw16i(REG5, 32767);
    VoC_add16_rd(REG4, REG4, SpxPpState_nbands);
    VoC_lw16i_set_inc(REG0, SpxPpState_gain2, 1);
    VoC_loop_r_short(REG4, DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG5, REG0, DEFAULT);
    VoC_endloop0
SPEEX_PRE_DENOISE:
    /*
    st->ft[0] = MULT16_16_P15(st->gain2[0],st->ft[0]);
    for (i=1;i<N;i++)
    {
        st->ft[2*i-1] = MULT16_16_P15(st->gain2[i],st->ft[2*i-1]);
        st->ft[2*i] = MULT16_16_P15(st->gain2[i],st->ft[2*i]);
    }
    st->ft[2*N-1] = MULT16_16_P15(st->gain2[N-1],st->ft[2*N-1]);
    */
    VoC_lw16i_set_inc(REG0, SpxPpState_ft, 1);
    VoC_lw16i_set_inc(REG1, SpxPpState_gain2, 1);
    //VoC_lw32z(ACC0, DEFAULT)
    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_lw16_p(REG7, REG0, IN_PARALLEL);

    /*
    VoC_mac32_rp(REG7, REG1, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(REG45, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(REG45, 15, DEFAULT);
    */
    VoC_multf32_rp(REG45, REG7, REG1, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(REG45, REG45, SPEEX_CONST_16384);
    VoC_shr32_ri(REG45, 15, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG4, REG0, DEFAULT);

    VoC_inc_p(REG1, DEFAULT);

    VoC_lw16_d(REG6, SpxPpState_ps_size);
    VoC_sub16_rd(REG6, REG6, SPEEX_CONST_1);
    VoC_loop_r(1, REG6)
    /*
    VoC_loop_i_short(2, DEFAULT);
    VoC_startloop0
        VoC_lw16_p(REG7, REG0, DEFAULT);
        VoC_multf32_rp(REG45, REG7, REG1, DEFAULT);
        VoC_NOP();
        VoC_add32_rd(REG45, REG45, SPEEX_CONST_16384);
        VoC_shr32_ri(REG45, 15, DEFAULT);
        VoC_NOP();
        VoC_sw16inc_p(REG4, REG0, DEFAULT);
    VoC_endloop0

    VoC_inc_p(REG1, DEFAULT);
    */
    VoC_lw16_p(REG7, REG0, DEFAULT);
    VoC_multf32_rp(REG45, REG7, REG1, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(REG45, REG45, SPEEX_CONST_16384);
    VoC_shr32_ri(REG45, 15, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG4, REG0, DEFAULT);

    VoC_lw16_p(REG7, REG0, DEFAULT);
    VoC_multf32inc_rp(REG45, REG7, REG1, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(REG45, REG45, SPEEX_CONST_16384);
    VoC_shr32_ri(REG45, 15, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG4, REG0, DEFAULT);
    VoC_endloop1

    VoC_sub16_rd(REG1, REG1, SPEEX_CONST_1);
    /*
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16_p(REG7, REG0, DEFAULT);
    VoC_mac32_rp(REG7, REG1, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(REG45, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(REG45, 15, DEFAULT);
    */
    VoC_lw16_p(REG7, REG0, DEFAULT);
    VoC_multf32_rp(REG45, REG7, REG1, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(REG45, REG45, SPEEX_CONST_16384);
    VoC_shr32_ri(REG45, 15, DEFAULT);
    VoC_NOP();
    VoC_sw16_p(REG4, REG0, DEFAULT);

    //VoC_jal(spx_ifft);
    VoC_lw16i(REG0, SpxPpState_ft);
    VoC_lw16i(REG1, SpxPpState_frame);
    VoC_sw16_d(REG0, SpxPpState_ifft_in_ADDR);
    VoC_sw16_d(REG1, SpxPpState_ifft_out_ADDR);
    VoC_jal(spx_kiss_fftri2);

    /*
    for (i=0;i<2*N;i++)
    {
        st->frame[i] = PSHR16(st->frame[i], st->frame_shift);
        st->frame[i] = MULT16_16_Q15(st->frame[i], st->window[i]);
    }
    */
    // when calc PSHR16(0x7fff, 3) = 0x1000 or 0xfff?
    /*
    VoC_lw16i_set_inc(REG0, SpxPpState_frame, 1);
    VoC_lw16i_set_inc(REG1, SpxPpState_window, 1);
    VoC_lw16_d(REG2, SpxPpState_frame_shift);
    VoC_lw16_d(REG4, SpxPpState_ps_size);
    VoC_NOP();
    VoC_shr16_ri(REG4, -1, DEFAULT);
    VoC_lw16i_short(REG5, 1, DEFAULT);
    VoC_sub16_dr(REG3, SPEEX_CONST_1, REG2); // REG3 = 1-st->frame_shift
    VoC_NOP();
    VoC_shr16_rr(REG5, REG3, DEFAULT);

    VoC_lw16i_short(REG3, 0, DEFAULT);
    VoC_NOP();
    SPX_PRCOESS_RUN_FRAME_BEGIN:
    VoC_be16_rr(SPX_PRCOESS_RUN_FRAME_END, REG3, REG4)
        if (REGS[3].reg == 0x9d)
            VoC_NOP();
        VoC_add16_rp(REG6, REG5, REG0, DEFAULT);
        VoC_NOP();
        VoC_shr16_rr(REG6, REG2, DEFAULT);

        VoC_lw32z(ACC0, DEFAULT);
        VoC_lw16i_short(FORMAT32, 15, IN_PARALLEL);
        VoC_NOP();
        VoC_mac32inc_rp(REG6, REG1, DEFAULT);
        VoC_NOP();
        VoC_movreg32(REG67, ACC0, DEFAULT);
        VoC_NOP();
        VoC_sw16inc_p(REG6, REG0, DEFAULT);
        VoC_add16_rd(REG3, REG3, SPEEX_CONST_1);
        VoC_NOP();
        VoC_jump(SPX_PRCOESS_RUN_FRAME_BEGIN);
    SPX_PRCOESS_RUN_FRAME_END:
        VoC_NOP();
    */
    VoC_lw16i_set_inc(REG0, SpxPpState_frame, 1);
    VoC_lw16i_set_inc(REG1, SpxPpState_window, 1);
    VoC_lw16_d(REG2, SpxPpState_frame_shift);
    VoC_lw16_d(REG4, SpxPpState_ps_size);

    VoC_shr16_ri(REG4, -1, DEFAULT);
    //VoC_lw16i_short(REG5, 1, DEFAULT);
    //VoC_lw32_d(RL7, SPEEX_CONST_1);
    VoC_lw16i_short(RL7_LO, 1, DEFAULT);
    VoC_lw16i_short(RL7_HI, 0, IN_PARALLEL);
    VoC_sub16_dr(REG3, SPEEX_CONST_1, REG2); // REG3 = 1-st->frame_shift
    //VoC_shr16_rr(REG5, REG3, DEFAULT);
    VoC_shr32_rr(RL7, REG3, DEFAULT);

    VoC_loop_r_short(REG4, DEFAULT);
    VoC_lw16i_short(FORMAT32, 15, IN_PARALLEL);
    VoC_startloop0
    VoC_lw16_p(REG6, REG0, DEFAULT);
    VoC_lw16i_short(REG7, 0, IN_PARALLEL);

    VoC_bnltz16_r(SPEEX_PRE_EXTEND_FRAME, REG6)
    VoC_lw16i(REG7, 0xffff);
SPEEX_PRE_EXTEND_FRAME:
    VoC_add32_rr(REG67, REG67, RL7, DEFAULT);

    //VoC_shr16_rr(REG6, REG2, DEFAULT);
    VoC_shr32_rr(REG67, REG2, DEFAULT);


    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 15, IN_PARALLEL);
    VoC_mac32inc_rp(REG6, REG1, DEFAULT);
    VoC_NOP();
    VoC_movreg32(REG67, ACC0, DEFAULT);
    /*
    VoC_multf32inc_rp(REG67, REG6, REG1, DEFAULT);
    */
    VoC_NOP();
    VoC_sw16inc_p(REG6, REG0, DEFAULT);
    VoC_endloop0

    /*
    for (i=0;i<N;i++)
    {
        x[i] = st->outbuf[i] + st->frame[i];
        st->outbuf[i] = st->frame[N+i];
    }
    */
    VoC_lw16i_set_inc(REG0, SPEEX_REF_BUF, 1);
    VoC_lw16i_set_inc(REG1, SpxPpState_frame, 1);
    VoC_lw16i_set_inc(REG2, SpxPpState_frame_N, 1);
    VoC_lw16i_set_inc(REG3, SpxPpState_outbuf, 1);
    VoC_lw16_d(REG4, SpxPpState_ps_size);

    VoC_loop_r_short(REG4, DEFAULT);
    VoC_startloop0
    VoC_lw16_p(REG6, REG3, DEFAULT);
    VoC_lw16inc_p(REG5, REG2, DEFAULT);
    VoC_add16inc_rp(REG4, REG6, REG1, DEFAULT);
    VoC_sw16inc_p(REG5, REG3, DEFAULT);
    //VoC_NOP();
    VoC_sw16inc_p(REG4, REG0, DEFAULT);
    //VoC_lw16inc_p(REG5, REG2, DEFAULT);
    //VoC_NOP();
    //VoC_sw16inc_p(REG5, REG3, DEFAULT);
    VoC_endloop0

    /*
    for (i=0;i<N;i++)
    {
        x[i] = st->outbuf[i] + st->frame[i];
        st->outbuf[i] = st->frame[st->frame_size+i];
    }
    */

    // was_speech in r0
    // return result in r1
    /*
    VoC_bez16_d(SPEEX_PRE_VAD_ENABLE, SpxPpState_vad_enabled)
        VoC_lw16_d(REG4, SpxPpState_speech_prob);
        VoC_lw16i_short(REG0, 0, DEFAULT);
        VoC_bngt16_rd(SPEEX_PRE_VAD_IF2, REG4, SpxPpState_speech_prob_start)
            VoC_lw16i_short(REG0, 1, DEFAULT);
    SPEEX_PRE_VAD_IF2:
        VoC_sw16_d(REG0, SpxPpState_was_speech);
        VoC_sw16_d(REG0, SPEEX_PRE_RETURN);
        VoC_jump(SPEEX_PRE_VAD_END);
    SPEEX_PRE_VAD_ENABLE:
    VoC_lw16i_short(REG1, 1, DEFAULT);
    VoC_sw16_d(REG1, SPEEX_PRE_RETURN);
    SPEEX_PRE_VAD_END:
    */

    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

void speex_div32_16(void)
{

    /*
    VoC_lw16i_short(RL6_LO, 10, DEFAULT);
    VoC_lw16i_short(RL6_HI, 0, IN_PARALLEL);
    VoC_lw16i_short(RL7_LO, 2, DEFAULT);
    VoC_lw16i_short(RL7_HI, 0, IN_PARALLEL);
    */

    //VoC_push16(RA, DEFAULT);
    VoC_push32(ACC0, DEFAULT);

    VoC_push32(REG23, DEFAULT);
    VoC_lw16i_short(REG4, 1, IN_PARALLEL);
    //VoC_push32(REG45, DEFAULT);

    VoC_push32(REG67, DEFAULT);
    VoC_lw16i_short(REG5, 0, IN_PARALLEL);

    // just for test

    VoC_lw32z(REG01, DEFAULT);
    VoC_movreg32(ACC0, RL6, IN_PARALLEL); // r45 = tmp = dividend

    VoC_lw16i_short(REG6, 31, DEFAULT); //r6 = 31
    VoC_shr32_ri(ACC0, 31, IN_PARALLEL); // tmp = dividend >> 31

    //VoC_sw32_d(RL7, SPEEX_VAR_RL7_ADDR);
    // get RL6 bits
    VoC_push32(RL6, DEFAULT);
    VoC_lw16i_short(REG7, 0, IN_PARALLEL);
    VoC_blt32_rd(SPX_DIV_INT32_LT_65536, RL6, SPEEX_CONST_65536)
    VoC_shr32_ri(RL6, 16, DEFAULT);
    VoC_add16_rd(REG7, REG7, SPEEX_CONST_16);
SPX_DIV_INT32_LT_65536:
    VoC_blt32_rd(SPX_DIV_INT32_LT_256, RL6, SPEEX_CONST_256)
    VoC_shr32_ri(RL6, 8, DEFAULT);
    VoC_add16_rd(REG7, REG7, SPEEX_CONST_8);
SPX_DIV_INT32_LT_256:
    VoC_blt32_rd(SPX_DIV_INT32_LT_16, RL6, SPEEX_CONST_16)
    VoC_shr32_ri(RL6, 4, DEFAULT);
    VoC_add16_rd(REG7, REG7, SPEEX_CONST_4);
SPX_DIV_INT32_LT_16:
    VoC_blt32_rd(SPX_DIV_INT32_LT_4, RL6, SPEEX_CONST_4)
    VoC_shr32_ri(RL6, 2, DEFAULT);
    VoC_add16_rd(REG7, REG7, SPEEX_CONST_2);
SPX_DIV_INT32_LT_4:
    VoC_blt32_rd(SPX_DIV_INT32_LT_2, REG67, SPEEX_CONST_2)
    VoC_add16_rr(REG7, REG7, REG4, DEFAULT);
SPX_DIV_INT32_LT_2:

    VoC_add16_rr(REG7, REG7, REG4, DEFAULT);
    VoC_pop32(RL6, IN_PARALLEL);

    VoC_movreg16(REG6, REG7, DEFAULT);
    VoC_add16_rr(REG7, REG7, REG4, IN_PARALLEL);
    //VoC_loop_r_short(REG7, DEFAULT)

    VoC_loop_r(1, REG7)
    VoC_sub16_rr(REG6, REG6, REG4, DEFAULT);
    VoC_movreg32(REG23, RL6, IN_PARALLEL);

    VoC_shr32_rr(REG23, REG6, DEFAULT);

    VoC_and32_rr(REG23, REG45, DEFAULT); // reg23 = nextd = (dividend >> i) & 0x1;
    VoC_shr32_ri(REG01, -1, IN_PARALLEL); // quotient = quotient << 1;

    VoC_bgt32_rr(SPX_DIV32_TMP_LT_DIVISOR, RL7, ACC0);
    VoC_add32_rr(REG01, REG01, REG45, DEFAULT); // quotient += 1;
    VoC_sub32_rr(ACC0, ACC0, RL7, IN_PARALLEL);
SPX_DIV32_TMP_LT_DIVISOR:
    VoC_shr32_ri(ACC0, -1, DEFAULT);
    VoC_add32_rr(ACC0, ACC0, REG23, DEFAULT); // tmp = ((tmp - divisor) << 1) + nextd;

    VoC_endloop1

    VoC_shr32_ri(ACC0, 1, DEFAULT);
    VoC_pop32(REG67, IN_PARALLEL);

    VoC_movreg32(REG45, ACC0, DEFAULT);
    //VoC_pop32(REG45, DEFAULT);
    VoC_pop32(REG23, IN_PARALLEL);

    VoC_pop32(ACC0, DEFAULT);
    //VoC_pop16(RA, DEFAULT);
    //VoC_NOP();
    VoC_return;
}

#if 0
// RL6 < 0
//VoC_directive:PARSER_OFF
void speex_div32_16_sign(void)
{

    /*
    VoC_lw16i_short(RL6_LO, 10, DEFAULT);
    VoC_lw16i_short(RL6_HI, 0, IN_PARALLEL);
    VoC_lw16i_short(RL7_LO, 2, DEFAULT);
    VoC_lw16i_short(RL7_HI, 0, IN_PARALLEL);
    */

    //VoC_push16(RA, DEFAULT);

    VoC_push32(ACC0, DEFAULT);
    VoC_push32(RL6, DEFAULT);

    VoC_push32(REG23, DEFAULT);
    VoC_lw16i_short(REG4, 1, IN_PARALLEL);
    //VoC_push32(REG45, DEFAULT);

    VoC_push32(REG67, DEFAULT);
    VoC_lw16i_short(REG5, 0, IN_PARALLEL);

    VoC_lw16i_short(REG3, 0, DEFAULT);
    VoC_bnltz32_r(SPEEX_DIV_NEG_RL6, RL6)
    VoC_lw16i_short(REG3, 1, DEFAULT);
    VoC_sub32_dr(RL6, SPEEX_CONST_0, RL6);
SPEEX_DIV_NEG_RL6:
    // just for test
    VoC_push16(REG3, DEFAULT);

    VoC_lw32z(REG01, DEFAULT);
    VoC_movreg32(ACC0, RL6, IN_PARALLEL); // r45 = tmp = dividend

    VoC_lw16i_short(REG6, 31, DEFAULT); //r6 = 31
    VoC_shr32_ri(ACC0, 31, IN_PARALLEL); // tmp = dividend >> 31

    //VoC_sw32_d(RL7, SPEEX_VAR_RL7_ADDR);
    // get RL6 bits
    VoC_push32(RL6, DEFAULT);
    VoC_lw16i_short(REG7, 0, IN_PARALLEL);
    VoC_blt32_rd(SPX_DIV_INT32_LT_65536, RL6, SPEEX_CONST_65536)
    VoC_shr32_ri(RL6, 16, DEFAULT);
    VoC_add16_rd(REG7, REG7, SPEEX_CONST_16);
SPX_DIV_INT32_LT_65536:
    VoC_blt32_rd(SPX_DIV_INT32_LT_256, RL6, SPEEX_CONST_256)
    VoC_shr32_ri(RL6, 8, DEFAULT);
    VoC_add16_rd(REG7, REG7, SPEEX_CONST_8);
SPX_DIV_INT32_LT_256:
    VoC_blt32_rd(SPX_DIV_INT32_LT_16, RL6, SPEEX_CONST_16)
    VoC_shr32_ri(RL6, 4, DEFAULT);
    VoC_add16_rd(REG7, REG7, SPEEX_CONST_4);
SPX_DIV_INT32_LT_16:
    VoC_blt32_rd(SPX_DIV_INT32_LT_4, RL6, SPEEX_CONST_4)
    VoC_shr32_ri(RL6, 2, DEFAULT);
    VoC_add16_rd(REG7, REG7, SPEEX_CONST_2);
SPX_DIV_INT32_LT_4:
    VoC_blt32_rd(SPX_DIV_INT32_LT_2, REG67, SPEEX_CONST_2)
    VoC_add16_rr(REG7, REG7, REG4, DEFAULT);
SPX_DIV_INT32_LT_2:

    VoC_add16_rr(REG7, REG7, REG4, DEFAULT);
    VoC_pop32(RL6, IN_PARALLEL);

    VoC_movreg16(REG6, REG7, DEFAULT);
    VoC_add16_rr(REG7, REG7, REG4, IN_PARALLEL);
    //VoC_loop_r_short(REG7, DEFAULT)

    VoC_loop_r(1, REG7)
    VoC_sub16_rr(REG6, REG6, REG4, DEFAULT);
    VoC_movreg32(REG23, RL6, IN_PARALLEL);

    VoC_shr32_rr(REG23, REG6, DEFAULT);

    VoC_and32_rr(REG23, REG45, DEFAULT); // reg23 = nextd = (dividend >> i) & 0x1;
    VoC_shr32_ri(REG01, -1, IN_PARALLEL); // quotient = quotient << 1;

    VoC_bgt32_rr(SPX_DIV32_TMP_LT_DIVISOR, RL7, ACC0);
    VoC_add32_rr(REG01, REG01, REG45, DEFAULT); // quotient += 1;
    VoC_sub32_rr(ACC0, ACC0, RL7, IN_PARALLEL);
SPX_DIV32_TMP_LT_DIVISOR:
    VoC_shr32_ri(ACC0, -1, DEFAULT);
    VoC_add32_rr(ACC0, ACC0, REG23, DEFAULT); // tmp = ((tmp - divisor) << 1) + nextd;

    VoC_endloop1

    VoC_pop16(REG3, DEFAULT);

    VoC_shr32_ri(ACC0, 1, DEFAULT);
    VoC_pop32(REG67, IN_PARALLEL);

    VoC_bez16_r(SPEEX_DIV_REG3_TRUE, REG3)
    VoC_sub32_dr(REG01, SPEEX_CONST_0, REG01);
SPEEX_DIV_REG3_TRUE:

    VoC_movreg32(REG45, ACC0, DEFAULT);
    //VoC_pop32(REG45, DEFAULT);
    VoC_pop32(REG23, IN_PARALLEL);

    VoC_pop32(RL6, DEFAULT);
    VoC_pop32(ACC0, DEFAULT);
    //VoC_pop16(RA, DEFAULT);
    //VoC_NOP();
    VoC_return;
}
//VoC_directive:PARSER_ON
#endif

/***********************
 * x in REG67
 * result in REG1
 ***********************/
// now working as intended
void spx_ilog2(void)
{
    //VoC_push16(RA, DEFAULT);
    VoC_push32(REG67, DEFAULT);
    //just for test
    //VoC_lw16i(REG6, 0x7ae8);
    //VoC_lw16i(REG7, 0x0001);


    VoC_lw16i_short(REG1, 0, DEFAULT);
    VoC_blt32_rd(SPX_ILOG2_INT32_LT_65536, REG67, SPEEX_CONST_65536)
    VoC_shr32_ri(REG67, 16, DEFAULT);
    VoC_add16_rd(REG1, REG1, SPEEX_CONST_16);
SPX_ILOG2_INT32_LT_65536:
    VoC_blt32_rd(SPX_ILOG2_INT32_LT_256, REG67, SPEEX_CONST_256)
    VoC_shr32_ri(REG67, 8, DEFAULT);
    VoC_add16_rd(REG1, REG1, SPEEX_CONST_8);
SPX_ILOG2_INT32_LT_256:
    VoC_blt32_rd(SPX_ILOG2_INT32_LT_16, REG67, SPEEX_CONST_16)
    VoC_shr32_ri(REG67, 4, DEFAULT);
    VoC_add16_rd(REG1, REG1, SPEEX_CONST_4);
SPX_ILOG2_INT32_LT_16:
    VoC_blt32_rd(SPX_ILOG2_INT32_LT_4, REG67, SPEEX_CONST_4)
    VoC_shr32_ri(REG67, 2, DEFAULT);
    VoC_add16_rd(REG1, REG1, SPEEX_CONST_2);
SPX_ILOG2_INT32_LT_4:
    VoC_blt32_rd(SPX_ILOG2_INT32_LT_2, REG67, SPEEX_CONST_2)
    VoC_add16_rd(REG1, REG1, SPEEX_CONST_1);
SPX_ILOG2_INT32_LT_2:
    VoC_pop32(REG67, DEFAULT);
    //VoC_pop16(RA, DEFAULT);
    //VoC_NOP();
    VoC_return;
}

/***********************
 * x in REG67
 * result in REG2
 ***********************/
void spx_ilog4(void)
{
    //VoC_push16(RA, DEFAULT);
    VoC_push32(REG67, DEFAULT);
    //just for test
    //VoC_lw16i(REG6, 0x8000);
    //VoC_lw16i(REG7, 0x0101);

    VoC_lw16i_short(REG2, 0, DEFAULT);
    VoC_blt32_rd(SPX_ILOG4_INT32_LT_65536, REG67, SPEEX_CONST_65536)
    VoC_shr32_ri(REG67, 16, DEFAULT);
    VoC_add16_rd(REG2, REG2, SPEEX_CONST_8);
SPX_ILOG4_INT32_LT_65536:
    VoC_blt32_rd(SPX_ILOG4_INT32_LT_256, REG67, SPEEX_CONST_256)
    VoC_shr32_ri(REG67, 8, DEFAULT);
    VoC_add16_rd(REG2, REG2, SPEEX_CONST_4);
SPX_ILOG4_INT32_LT_256:
    VoC_blt32_rd(SPX_ILOG4_INT32_LT_16, REG67, SPEEX_CONST_16)
    VoC_shr32_ri(REG67, 4, DEFAULT);
    VoC_add16_rd(REG2, REG2, SPEEX_CONST_2);
SPX_ILOG4_INT32_LT_16:
    VoC_blt32_rd(SPX_ILOG4_INT32_LT_4, REG67, SPEEX_CONST_4)
    VoC_shr32_ri(REG67, 2, DEFAULT);
    VoC_add16_rd(REG2, REG2, SPEEX_CONST_1);
SPX_ILOG4_INT32_LT_4:
    VoC_pop32(REG67, DEFAULT);
    //VoC_pop16(RA, DEFAULT);
    //VoC_NOP();
    VoC_return;
}

/***********************
 * x in REG1
 * result in REG0
 ***********************/
// now working as intended
//VoC_directive:PARSER_OFF
#if 0
void spx_atan01(void)
{
    //VoC_push16(RA, DEFAULT);
    VoC_push32(REG23, DEFAULT);
    //VoC_push16(LOOP0, DEFAULT);
    //// just for test
    // VoC_lw16i(REG0, 16);
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);

    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_lw16i_set_inc(REG0, SPEEX_ATAN01_M, 2);
    VoC_NOP();

    // MULT16_16_P15(x, ADD32(M1, MULT16_16_P15(x, ADD32(M2, MULT16_16_P15(x, ADD32(M3, MULT16_16_P15(M4, x)))))));
    VoC_lw32inc_p(REG23, REG0, DEFAULT);
    VoC_loop_i(1, 3)
    VoC_mac32_rr(REG2, REG1, DEFAULT); //acc0 = MULT16_16_P15(M4, x)
    VoC_NOP();

    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT);

    VoC_movreg32(ACC1, ACC0, DEFAULT);
    VoC_lw32inc_p(REG23, REG0, IN_PARALLEL); // M3 in r23

    VoC_add32_rr(ACC1, ACC1, REG23, DEFAULT);
    VoC_movreg32(REG23, ACC1, DEFAULT);
    VoC_lw32z(ACC0, DEFAULT);
    VoC_endloop1
    VoC_mac32_rr(REG2, REG1, DEFAULT); //acc0 = MULT16_16_P15(M4, x)
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_movreg32(REG01, ACC0, DEFAULT);

    //VoC_pop16(LOOP0, DEFAULT);
    VoC_pop32(REG23, DEFAULT);
    //VoC_pop16(RA, DEFAULT);
    //VoC_NOP();
    VoC_return;
}
#endif
//VoC_directive:PARSER_ON

/***********************
 * x in REG67
 * result in REG0
 ***********************/
/*
static inline spx_word16_t spx_atan(spx_word32_t x)
{
   if (x <= 32767)
   {
      return SHR16(spx_atan01(x),1);
   } else {
      int e = spx_ilog2(x);
      if (e>=29)
         return 25736;
      x = DIV32_16(SHL32(EXTEND32(32767),29-e), EXTRACT16(SHR32(x, e-14)));
      return SUB16(25736, SHR16(spx_atan01(x),1));
   }
}
*/
// spx_atan(16) = 8
// now working as intended
//VoC_directive:PARSER_OFF
#if 0
void spx_atan(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push16(REG1, DEFAULT);
    VoC_push16(REG2, DEFAULT);
    VoC_push32(ACC0, DEFAULT);
    VoC_push32(ACC1, DEFAULT);

    // just for test
    //VoC_lw16i(REG6, 0x7ae8);
    //VoC_lw16i(REG7, 0x0001);

    VoC_bngt32_rd(SPX_ATAN_INT32_LET_Z, REG67, SPEEX_CONST_32767)
    VoC_jal(spx_ilog2);
    VoC_lw16i(REG0, 25736);
    VoC_bnlt16_rd(SPX_ATAN_END, REG1, SPEEX_CONST_29) // e in r1
    // calc EXTRACT16(SHR32(x, e-14))
    VoC_sub16_rd(REG2, REG1, SPEEX_CONST_14);
    VoC_shr32_rr(REG67, REG2, DEFAULT); // result in r6
    VoC_movreg32(RL7, REG67, DEFAULT); // result in rl7
    // calc SHL32(EXTEND32(32767),29-e)
    VoC_sub16_rd(REG2, REG1, SPEEX_CONST_29);
    VoC_lw32_d(RL6, SPEEX_CONST_32767);
    VoC_shr32_rr(RL6, REG2, DEFAULT); // result in rl6
    VoC_jal(speex_div32_16); // x in r01
    // return SUB16(25736, SHR16(spx_atan01(x),1));
    VoC_movreg16(REG1, REG0, DEFAULT);
    VoC_jal(spx_atan01);
    VoC_shr16_ri(REG0, 1, DEFAULT);
    VoC_sub16_dr(REG0, SPEEX_CONST_25736, REG0);
    VoC_jump(SPX_ATAN_END);
SPX_ATAN_INT32_LET_Z:
    VoC_movreg16(REG1, REG6, DEFAULT);
    VoC_jal(spx_atan01);
    VoC_shr16_ri(REG0, 1, DEFAULT);
SPX_ATAN_END:
    VoC_pop32(ACC1, DEFAULT);
    VoC_pop32(ACC0, DEFAULT);
    VoC_pop16(REG2, DEFAULT);
    VoC_pop16(REG1, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}
#endif
//VoC_directive:PARSER_ON

/***********************
 * input :  n in REG1
 *          result in ACC1
 * output : none
 ***********************/
// now working as intended
//VoC_directive:PARSER_OFF
#if 0
void speex_toBARK(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push16(REG0, DEFAULT);
    VoC_push32(REG23, DEFAULT);
    VoC_push32(REG45, DEFAULT);
    VoC_push32(REG67, DEFAULT);

    // just for test
    //VoC_lw16i(REG1, 4000);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);

    VoC_lw16i_short(FORMAT32, 2, DEFAULT);
    VoC_lw16i_short(FORMATX, 0, IN_PARALLEL);

    VoC_lw16i(REG0, 97);
    VoC_mac32_rr(REG1, REG0, DEFAULT); // ACC0 = SHR32(MULT16_16(97,n),2)
    VoC_NOP();
    VoC_movreg32(REG67, ACC0, DEFAULT);
    VoC_jal(spx_atan);  // r0 = spx_atan(SHR32(MULT16_16(97,n),2))
    VoC_lw16i(REG2, 26829);
    VoC_lw32z(ACC1, DEFAULT);
    VoC_NOP();
    VoC_macX_rr(REG2, REG0, IN_PARALLEL); // result in acc1

    //MULT16_32_Q15(a,b) = ADD32(MULT16_16((a),SHR((b),15)), SHR(MULT16_16((a),((b)&0x00007fff)),15))
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_mac32_rr(REG1, REG1, DEFAULT); // r23 =MULT16_16(n,n)
    VoC_NOP();
    VoC_movreg32(REG23, ACC0, DEFAULT);
    VoC_movreg32(REG45, ACC0, IN_PARALLEL);
    VoC_shr32_ri(REG45, 15, DEFAULT);
    VoC_lw16i_short(REG0, 20, IN_PARALLEL);
    VoC_lw32z(ACC0, DEFAULT);
    VoC_and32_rd(REG23, SPEEX_CONST_32767);
    VoC_mac32_rr(REG4, REG0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 15, DEFAULT);
    VoC_mac32_rr(REG2, REG0, DEFAULT);  // ACC0 = MULT16_32_Q15(20,MULT16_16(n,n))
    VoC_NOP();
    VoC_movreg32(REG67, ACC0, DEFAULT);
    VoC_jal(spx_atan); // r0 = spx_atan(MULT16_32_Q15(20,MULT16_16(n,n)))
    VoC_lw16i(REG3, 4588);
    VoC_NOP();
    VoC_macX_rr(REG0, REG3, IN_PARALLEL);
    // ACC1 = MULT16_16(26829,spx_atan(SHR32(MULT16_16(97,n),2))) + MULT16_16(4588,spx_atan(MULT16_32_Q15(20,MULT16_16(n,n))))
    VoC_lw16i(REG3, 3355); // why VoC_NOP is not needed
    VoC_NOP();
    VoC_macX_rr(REG1, REG3, IN_PARALLEL);

    VoC_pop32(REG67, DEFAULT);
    VoC_pop32(REG45, DEFAULT);
    VoC_pop32(REG23, DEFAULT);
    VoC_pop16(REG0, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}
#endif
//VoC_directive:PARSER_ON

/****************************
 * input :  int banks : SpxPpState_nb_bands
            spx_word32_t sampling : FILTERBANK_NEW_SAMPLING
            int len : SpxPpState_ps_size
            int type : FILTERBANK_NEW_TYPE
 ****************************/
// now working as intended
//VoC_directive:PARSER_OFF
#if 0
void filterbank_new(void)
{
    VoC_push16(RA, DEFAULT);
//  bank->nb_banks = banks;
//  bank->len = len;

//  df = DIV32(SHL32(sampling,14),len);
//  max_mel = toBARK(EXTRACT16(sampling/2));
//  mel_interval = PDIV32(max_mel,banks-1);
    VoC_lw16_d(RL6_LO, FILTERBANK_NEW_SAMPLING);
    VoC_lw16i_short(RL6_HI, 0, DEFAULT);
    VoC_shr32_ri(RL6, -14, DEFAULT);
    VoC_lw16i_short(RL7_HI, 0, IN_PARALLEL);
    VoC_lw16_d(RL7_LO, SpxPpState_bank_len);
    VoC_jal(speex_div32_16); // then df in r01
    VoC_shr32_ri(REG01, 15, DEFAULT); // then r0 = (df>>15)
    VoC_NOP();
    VoC_sw16_d(REG0, FILTERBANK_DF);

    VoC_lw16_d(REG1, FILTERBANK_NEW_SAMPLING);
    VoC_shr16_ri(REG1, 1, DEFAULT);
    VoC_jal(speex_toBARK); //max_mel in acc1
    VoC_sw32_d(ACC1, FILTERBANK_MAX_MEL);

    // PDIV32(a,b) (((spx_word32_t)(a)+((spx_word16_t)(b)>>1))/((spx_word32_t)(b)))
    VoC_lw32z(RL7, DEFAULT);
    VoC_lw16_d(REG2, SpxPpState_bank_nb_banks);
    VoC_sub16_rd(REG2, REG2, SPEEX_CONST_1);
    VoC_movreg16(RL7_LO, REG2, DEFAULT);
    VoC_lw16i_short(REG3, 0, IN_PARALLEL);
    VoC_shr16_ri(REG2, 1, DEFAULT);
    VoC_add32_rr(RL6, ACC1, REG23, DEFAULT);
    VoC_jal(speex_div32_16); // mel_intrval in r01
    VoC_sw32_d(REG01, FILTERBANK_MEL_INTERVAL);
    /*
    for (i=0;i<len;i++)
    {
      spx_word16_t curr_freq;
      spx_word32_t mel;
      spx_word16_t val;
      curr_freq = EXTRACT16(MULT16_32_P15(i,df));
      mel = toBARK(curr_freq);
      if (mel > max_mel)
         break;
      id1 = DIV32(mel,mel_interval);
      if (id1>banks-2)
      {
         id1 = banks-2;
         val = Q15_ONE;
      } else {
         val = DIV32_16(mel - id1*mel_interval,EXTRACT16(PSHR32(mel_interval,15)));
      }
      id2 = id1+1;
      bank->bank_left[i] = id1;
      bank->filter_left[i] = SUB16(Q15_ONE,val);
      bank->bank_right[i] = id2;
      bank->filter_right[i] = val;
    }
    */
    VoC_lw16i_set_inc(REG0, SpxPpState_bank_bank_left, 1);
    VoC_lw16i_set_inc(REG1, SpxPpState_bank_bank_right, 1);
    VoC_lw16i_set_inc(REG2, SpxPpState_bank_filter_left, 1);
    VoC_lw16i_set_inc(REG3, SpxPpState_bank_filter_right, 1);
    VoC_lw32z(REG67, DEFAULT);

    VoC_lw16_d(REG5, FILTERBANK_LEN);
    VoC_loop_r(0, REG5)
    VoC_push32(REG01, DEFAULT);
    VoC_push16(REG7, DEFAULT);

    VoC_movreg16(REG1, REG7, DEFAULT);
    VoC_jal(speex_toBARK); // mel now in acc1
    VoC_lw32_d(REG45, FILTERBANK_MAX_MEL);
    VoC_bgt32_rr(FILTERBANK_NEW_END, ACC1, REG45);
    VoC_movreg32(RL6, ACC1, DEFAULT); // mel now in rl6
    VoC_lw32_d(RL7, FILTERBANK_MEL_INTERVAL);
    VoC_jal(speex_div32_16); // id1(16bit) in r01
    VoC_lw16_d(REG6, FILTERBANK_NB_BANKS);
    VoC_movreg16(REG7, REG0, DEFAULT); // id1 in reg7
    VoC_sub16_rd(REG6, REG6, SPEEX_CONST_2);
    VoC_bngt16_rr(FILTERBANK_ID1_LET, REG7, REG6)
    VoC_movreg16(REG7, REG6, DEFAULT);
    VoC_lw16i(REG5, 0x7fff); //val = Q15_ONE
    VoC_jump(FILTERBANK_ID1_END);
FILTERBANK_ID1_LET:
    // else val = DIV32_16(mel - id1*mel_interval,PSHR32(mel_interval,15));
    VoC_lw32_d(RL7, FILTERBANK_MEL_INTERVAL);
    VoC_movreg32(REG45, RL7, DEFAULT);
    VoC_add32_rd(RL7, RL7, SPEEX_CONST_16384);
    VoC_shr32_ri(RL7, 15, DEFAULT); //PSHR32(mel_interval,15) in rl6

    //calc id1*mel_interval
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_lw16i_short(FORMATX, -16, DEFAULT);

    VoC_mac32_rr(REG4, REG7, DEFAULT);
    VoC_macX_rr(REG5, REG7, IN_PARALLEL);

    VoC_NOP();
    VoC_add32_rr(ACC0, ACC0, ACC1, DEFAULT);
    VoC_sub32_rr(RL6, RL6, ACC0, DEFAULT);

    VoC_jal(speex_div32_16); // now val in r01
FILTERBANK_ID1_END:
    VoC_sw16inc_p(REG0, REG3, DEFAULT);
    VoC_lw16i_short(REG6, 0, DEFAULT);
    VoC_sub16_dr(REG4, SPEEX_CONST_32767, REG0);
    VoC_NOP();
    VoC_pop32(REG01, DEFAULT);
    VoC_lw16i_short(INC0, 1, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG4, REG2, DEFAULT);
    VoC_sw16inc_p(REG7, REG0, DEFAULT);

    VoC_add16_rd(REG7, REG7, SPEEX_CONST_1);
    VoC_NOP();
    VoC_sw16inc_p(REG7, REG1, DEFAULT);
    VoC_pop16(REG7, DEFAULT);
    VoC_add16_rd(REG7, REG7, FILTERBANK_DF);
    // this cannot be commented
    VoC_endloop0
FILTERBANK_NEW_END:
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}
#endif
//VoC_directive:PARSER_ON

/*******************************
 * INPUT :  spx_word16_t *w: POINT st->window
            int len = 2 * frame_size
 * output:  none
 *******************************/
// working as intended
//VoC_directive:PARSER_OFF
#if 0
void conj_window(void)
{
    VoC_push16(RA, DEFAULT);

    VoC_lw32z(RL6, DEFAULT);
    VoC_lw32z(RL7, IN_PARALLEL);
    VoC_lw16i_set_inc(REG3, SpxPpState_window, 1);

    VoC_lw16_d(REG1, SpxPpState_frame_size);
    VoC_shr16_ri(REG1, -1, DEFAULT);

    VoC_lw16i_short(REG7, 0, DEFAULT);
    VoC_movreg16(RL7_LO, REG1, DEFAULT); // rl6 = tmp2 rl7 = len
    VoC_movreg16(REG6, REG1, IN_PARALLEL);
CONJ_WINDOW_LOOP_BEGIN:
    VoC_be16_rr(CONJ_WINDOW_LOOP_END, REG7, REG6)
    VoC_jal(speex_div32_16); // r01 = x
    VoC_lw16i_short(REG2, 0, DEFAULT);
    VoC_bnlt16_rd(CONJ_WINDOW_X_NLT_8192, REG0, SPEEX_CONST_8192);
    VoC_jump(CONJ_WINDOW_X_END);
CONJ_WINDOW_X_NLT_8192:
    VoC_bnlt16_rd(CONJ_WINDOW_X_NLT_16384, REG0, SPEEX_CONST_16384);
    VoC_sub16_dr(REG0, SPEEX_CONST_16384, REG0);
    VoC_lw16i_short(REG2, 1, DEFAULT);
    VoC_jump(CONJ_WINDOW_X_END);
CONJ_WINDOW_X_NLT_16384:
    VoC_bnlt16_rd(CONJ_WINDOW_X_NLT_24576, REG0, SPEEX_CONST_24576);
    VoC_sub16_rd(REG0, REG0, SPEEX_CONST_16384);
    VoC_lw16i_short(REG2, 1, DEFAULT);
    VoC_jump(CONJ_WINDOW_X_END);
CONJ_WINDOW_X_NLT_24576:
    VoC_sub32_dr(REG01, SPEEX_CONST_32768, REG01);

CONJ_WINDOW_X_END:
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 14, IN_PARALLEL);

    VoC_mac32_rd(REG0, SPEEX_CONST_20839);
    VoC_NOP();
    VoC_shr32_ri(ACC0, -2, DEFAULT);
    VoC_movreg32(REG01, ACC0, DEFAULT);
    VoC_jal(spx_cos_norm); // now tmp16 in reg0

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    VoC_mac32_rd(REG0, SPEEX_CONST_16384);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT); // now MULT16_16_P15(16384,tmp16) in ACC0
    VoC_movreg32(REG01, ACC0, DEFAULT); // now MULT16_16_P15(16384,tmp16) in r0
    VoC_sub16_dr(REG0, SPEEX_CONST_16384, REG0);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 15, IN_PARALLEL);

    VoC_mac32_rr(REG0, REG0, DEFAULT); // now tmp in acc1
    VoC_NOP();
    VoC_movreg32(REG01, ACC0, DEFAULT);
    VoC_bez16_r(CONJ_WINDOW_INV_EZ, REG2)
    VoC_sub16_dr(REG0, SPEEX_CONST_32767, REG0);
CONJ_WINDOW_INV_EZ:
    VoC_shr32_ri(REG01, -15, DEFAULT);
    VoC_jal(spx_sqrt);
    VoC_sw16inc_p(REG0, REG3, DEFAULT);
    VoC_add32_rd(RL6, RL6, SPEEX_CONST_32767);
    VoC_add16_rd(REG7, REG7, SPEEX_CONST_1);
    VoC_jump(CONJ_WINDOW_LOOP_BEGIN);
CONJ_WINDOW_LOOP_END:

    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}
#endif
//VoC_directive:PARSER_ON

/************
 * in in reg01
 * out in reg0
 ************/
// working as intended
void spx_sqrt(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push32(REG23, DEFAULT);
    VoC_push32(REG67, DEFAULT);

    //VoC_lw16i(REG1, 0x0101);
    //VoC_lw16i(REG0, 0x8000);


    VoC_movreg32(REG67, REG01, DEFAULT);
    VoC_jal(spx_ilog4); // now spx_ilog4(x) in reg2
    // k = spx_ilog4(x)-6; k <<= 1;
    VoC_sub16_rd(REG2, REG2, SPEEX_CONST_6);
    VoC_movreg16(REG3, REG2, DEFAULT); //k in r3
    VoC_shr16_ri(REG2, -1, DEFAULT); // tmp in r2
    // x = VSHR32(x, tmp);
    VoC_shr32_rr(REG01, REG2, DEFAULT); // x in reg0

    VoC_lw16i_set_inc(REG2, SPEEX_SQRT_C, 1);
    /*
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 14, IN_PARALLEL);
    VoC_mac32inc_rp(REG0, REG2, DEFAULT);
    VoC_NOP();
    VoC_movreg32(REG67, ACC0, DEFAULT);
    */
    VoC_lw16i_short(FORMAT32, 14, DEFAULT);
    VoC_multf32inc_rp(REG67, REG0, REG2, DEFAULT);
    VoC_NOP();
    VoC_add16inc_rp(REG6, REG6, REG2, DEFAULT);

    /*
    VoC_lw32z(ACC0, DEFAULT);
    VoC_mac32_rr(REG0, REG6, DEFAULT);
    VoC_NOP();
    VoC_movreg32(REG67, ACC0, DEFAULT);
    */
    VoC_multf32_rr(REG67, REG0, REG6, DEFAULT);
    VoC_NOP();
    VoC_add16inc_rp(REG6, REG6, REG2, DEFAULT);

    /*
    VoC_lw32z(ACC0, DEFAULT);
    VoC_mac32_rr(REG0, REG6, DEFAULT);
    VoC_NOP();
    VoC_movreg32(REG67, ACC0, DEFAULT);
    */
    VoC_multf32_rr(REG67, REG0, REG6, DEFAULT);
    VoC_NOP();
    VoC_add16inc_rp(REG0, REG6, REG2, DEFAULT); // now rt in REG01

    VoC_sub16_dr(REG3, SPEEX_CONST_7, REG3); // tmp in r3
    VoC_shr16_rr(REG0, REG3, DEFAULT);
    //VoC_movreg16(REG0, REG6, DEFAULT); // rt in reg0

    VoC_pop32(REG67, DEFAULT);
    VoC_pop32(REG23, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}


/******
 * in REG0
 * out REG0
 ******/
// _spx_cos_pi_2(0xe38) = 0x7e0f
// now working as intended
//VoC_directive:PARSER_OFF
#if 0
void _spx_cos_pi_2(void)
{
    //VoC_push16(RA, DEFAULT);
    VoC_push16(REG1, DEFAULT);
    VoC_push32(REG23,IN_PARALLEL);
    VoC_push32(REG45, DEFAULT);
    VoC_push32(REG67, DEFAULT);

    // just for test
    //VoC_lw16i(REG0, 0xe38);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    VoC_mac32_rr(REG0, REG0, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT); // x2 = MULT16_16_P15(x,x)
    VoC_movreg32(REG01, ACC0, DEFAULT);
    VoC_movreg32(REG45, ACC0, IN_PARALLEL);
    VoC_lw16i_set_inc(REG2, SPEEX_COS_PI_2_L, 2);

    // ADD32(L3, MULT16_16_P15(L4, x2)))
    VoC_lw32z(ACC0, DEFAULT);
    VoC_mac32inc_rp(REG4, REG2, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_add32inc_rp(ACC0, ACC0, REG2, DEFAULT);
    VoC_movreg32(REG01, ACC0, DEFAULT);

    // ADD32(L2, MULT16_16_P15(x2, ADD32(L3, MULT16_16_P15(L4, x2)))
    VoC_lw32z(ACC0, DEFAULT);
    VoC_mac32_rr(REG4, REG0, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_add32inc_rp(ACC0, ACC0, REG2, DEFAULT);
    VoC_movreg32(REG01, ACC0, DEFAULT);

    // ADD32(SUB16(L1,x2), MULT16_16_P15(x2, ADD32(L2, MULT16_16_P15(x2, ADD32(L3, MULT16_16_P15(L4, x2)))))
    VoC_lw32z(ACC0, DEFAULT);
    VoC_mac32_rr(REG4, REG0, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_sub16_pr(REG6, REG2, REG4, DEFAULT);
    VoC_lw16i_short(REG7, 0, IN_PARALLEL);
    VoC_add32_rr(ACC0, ACC0, REG67, DEFAULT);
    VoC_movreg32(REG01, ACC0, DEFAULT);

    VoC_blt16_rd(SPX_COS_PI_2_LT, REG0, SPEEX_CONST_32766)
    VoC_lw16i(REG0, 32766);
SPX_COS_PI_2_LT:
    VoC_add16_rd(REG0, REG0, SPEEX_CONST_1);

    VoC_pop32(REG67, DEFAULT);
    VoC_pop32(REG45, DEFAULT);
    VoC_pop32(REG23, DEFAULT);
    VoC_pop16(REG1, IN_PARALLEL);
    //VoC_pop16(RA, DEFAULT);
    //VoC_NOP();
    VoC_return;
}
#endif
//VoC_directive:PARSER_ON

/*****************
 * x in reg01
 * result in reg0
 *****************/
// spx_cos_norm(0x0e38)=0x1
// now working as intended
//VoC_directive:PARSER_OFF
#if 0
void spx_cos_norm(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push32(REG23, DEFAULT);
    VoC_push32(RL7, DEFAULT);

    //just for test
    //VoC_lw16i_short(REG1, 0, DEFAULT);
    //VoC_lw16i(REG0, 0xe38);


    VoC_and32_rd(REG01, SPEEX_CONST_131071);
    VoC_bngt32_rd(SPX_COS_NORM_NGT_65536, REG01, SPEEX_CONST_65536)
    VoC_sub32_dr(REG01, SPEEX_CONST_131072, REG01);
SPX_COS_NORM_NGT_65536:
    VoC_movreg32(RL7, REG01, DEFAULT);
    VoC_movreg32(REG23, REG01, DEFAULT);
    VoC_and32_rd(RL7, SPEEX_CONST_32767);
    VoC_bez32_r(SPX_COS_NORM_EZ, RL7)
    VoC_bnlt32_rd(SPX_COS_NORM_IN_IF, REG01, SPEEX_CONST_32768);
    VoC_jal(_spx_cos_pi_2);
    VoC_jump(SPX_COS_NORM_END);
SPX_COS_NORM_IN_IF:
    VoC_sub16_dr(REG0, SPEEX_CONST_65536, REG0);
    VoC_jal(_spx_cos_pi_2);
    VoC_sub16_dr(REG0, SPEEX_CONST_0, REG0);
    VoC_jump(SPX_COS_NORM_END);
SPX_COS_NORM_EZ:
    VoC_movreg32(REG01, REG23, DEFAULT);
    VoC_and32_rd(REG23, SPEEX_CONST_65535);
    VoC_bez32_r(SPX_COS_NORM_ELSE1, REG23)
    VoC_lw16i_short(REG0, 0, DEFAULT);
    VoC_jump(SPX_COS_NORM_END);
SPX_COS_NORM_ELSE1:
    VoC_and32_rd(REG01, SPEEX_CONST_131071);
    VoC_bez32_r(SPX_COS_NORM_ELSE2, REG01)
    VoC_lw16i(REG0, -32767);
    VoC_jump(SPX_COS_NORM_END);
SPX_COS_NORM_ELSE2:
    VoC_lw16i(REG0, 32767);
SPX_COS_NORM_END:
    VoC_pop32(RL7, DEFAULT);
    VoC_pop32(REG23, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}
#endif
//VoC_directive:PARSER_ON

/*************************
 * INPUT : int frame_size,
           int sampling_rate
 * output : none
 *************************/
//VoC_directive:PARSER_OFF
#if 0
void speex_preprocess_state_init(void)
{
    VoC_push16(RA, DEFAULT);

    VoC_lw16i_short(REG2, 0, DEFAULT);
    VoC_lw16i_short(REG3, 1, IN_PARALLEL);

    /*
    st->sampling_rate = sampling_rate;
    st->denoise_enabled = 1;
    st->vad_enabled = 0;
    st->dereverb_enabled = 0;
    st->reverb_decay = 0;
    st->reverb_level = 0;
    st->noise_suppress = NOISE_SUPPRESS_DEFAULT;
    st->echo_suppress = ECHO_SUPPRESS_DEFAULT;
    st->echo_suppress_active = ECHO_SUPPRESS_ACTIVE_DEFAULT;

    st->speech_prob_start = SPEECH_PROB_START_DEFAULT;
    st->speech_prob_continue = SPEECH_PROB_CONTINUE_DEFAULT;

    st->echo_state = NULL;

    st->nbands = NB_BANDS;
    */
    VoC_lw16i(REG0, SPEEX_NN);
    VoC_lw16i(REG1, 8000);
    VoC_sw16_d(REG0, SpxPpState_frame_size);
    VoC_sw16_d(REG0, SpxPpState_ps_size);
    VoC_sw16_d(REG1, SpxPpState_sampling_rate);
    VoC_sw16_d(REG3, SpxPpState_denoise_enabled);
    VoC_sw16_d(REG2, SpxPpState_vad_enabled);
    VoC_sw16_d(REG2, SpxPpState_dereverb_enabled);
    VoC_sw16_d(REG2, SpxPpState_reverb_decay);
    VoC_sw16_d(REG2, SpxPpState_reverb_level);
    VoC_sw16_d(REG2, SpxPpState_echo_state_ADDR);

    VoC_lw16_d(REG2, SPEEX_NOISE_SUPPRESS_DEFAULT);
    VoC_lw16i(REG3, ECHO_SUPPRESS_DEFAULT);
    VoC_sw16_d(REG2, SpxPpState_noise_suppress);
    VoC_sw16_d(REG3, SpxPpState_echo_suppress);

    VoC_lw16i(REG2, ECHO_SUPPRESS_ACTIVE_DEFAULT);
    VoC_lw16i(REG3, SPEECH_PROB_START_DEFAULT);
    VoC_sw16_d(REG2, SpxPpState_echo_suppress_active);
    VoC_sw16_d(REG3, SpxPpState_speech_prob_start);

    VoC_lw16i(REG2, SPEECH_PROB_CONTINUE_DEFAULT);
    VoC_lw16i_short(REG3, SPEEX_NB_BANDS, DEFAULT);
    VoC_sw16_d(REG2, SpxPpState_speech_prob_continue);
    VoC_sw16_d(REG3, SpxPpState_nbands);

    VoC_lw16i(REG0, SPEEX_NB_BANDS);
    VoC_lw16i(REG1, SPEEX_NN);
    VoC_lw16i(REG2, 8000);
    VoC_sw16_d(REG0, FILTERBANK_NB_BANKS);
    VoC_sw16_d(REG1, FILTERBANK_LEN);
    VoC_sw16_d(REG2, FILTERBANK_NEW_SAMPLING);
    VoC_jal(filterbank_new);

    VoC_jal(conj_window);

    VoC_lw16_d(REG4, SpxPpState_ps_size);

    VoC_lw16i_short(RL7_LO, 1, DEFAULT);
    VoC_lw16i_short(RL7_HI, 0, IN_PARALLEL);
    VoC_lw32z(RL6, DEFAULT);

    /*
    for (i=0;i<N;i++)
    {
      st->update_prob[i] = 1;
      st->inbuf[i]=0;
      st->outbuf[i]=0;
    }
    */
    VoC_lw16i_set_inc(REG0, SpxPpState_update_prob, 1);
    VoC_loop_r_short(REG4, DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(RL7_LO, REG0, DEFAULT);
    VoC_endloop0
    /*
    for (i=0;i<N+M;i++)
    {
      st->reverb_estimate[i]=0;
      st->old_ps[i]=1;
      st->post[i]=SHL16(1, SNR_SHIFT);
      st->prior[i]=SHL16(1, SNR_SHIFT);
    }
    */
    VoC_add16_rd(REG4, REG4, SpxPpState_nbands);
    VoC_lw16i_set_inc(REG0, SpxPpState_reverb_estimate, 2);
    VoC_lw16i_set_inc(REG1, SpxPpState_old_ps, 2);
    VoC_lw16i_set_inc(REG2, SpxPpState_post, 1);
    VoC_lw16i_set_inc(REG3, SpxPpState_prior, 1);
    VoC_lw16i_short(REG7, 1, DEFAULT);
    VoC_shr16_ri(REG7, NEG_SNR_SHIFT, DEFAULT); // REG7 = SHL16(1, SNR_SHIFT)
    VoC_loop_r_short(REG4, DEFAULT);
    VoC_startloop0
    VoC_sw32inc_p(RL6, REG0, DEFAULT);
    VoC_sw32inc_p(RL7, REG1, DEFAULT);
    VoC_sw16inc_p(REG7, REG2, DEFAULT);
    VoC_sw16inc_p(REG7, REG3, DEFAULT);
    VoC_endloop0

    /*
    for (i=0;i<N+M;i++)
    {
      st->gain[i]=Q15_ONE;
      st->noise[i]=QCONST32(1.f,NOISE_SHIFT);
    }
    */
    VoC_lw16i_set_inc(REG0, SpxPpState_gain, 1);
    VoC_lw16i_set_inc(REG1, SpxPpState_noise, 2);
    VoC_lw16i(REG5, 0x7fff);
    VoC_loop_r_short(REG4, DEFAULT);
    VoC_shr32_ri(RL7, NEG_NOISE_SHIFT, IN_PARALLEL);
    VoC_startloop0
    VoC_sw16inc_p(REG5, REG0, DEFAULT);
    VoC_sw32inc_p(RL7, REG1, DEFAULT);
    VoC_endloop0

    VoC_lw16i_short(REG2, 0, DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG2, SpxPpState_was_speech);
    VoC_sw16_d(REG2, SpxPpState_nb_adapt);
    VoC_sw16_d(REG2, SpxPpState_min_count);

    VoC_jal(spx_fft_init);

    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}
#endif
//VoC_directive:PARSER_ON

/********************
 * input : size : 2*SpxPpState_ps_size
 ********************/
/*********************
void *spx_fft_init(int size)
{
   struct kiss_config *table;
   table = (struct kiss_config*)speex_alloc(sizeof(struct kiss_config));
   table->forward = kiss_fftr_alloc(size,0,NULL,NULL);
   table->backward = kiss_fftr_alloc(size,1,NULL,NULL);
   table->N = size;
   return table;
}
*/
// now working as intended
//VoC_directive:PARSER_OFF
#if 0
void spx_fft_init(void)
{
    VoC_push16(RA, DEFAULT);

    VoC_lw16_d(REG4, SpxPpState_ps_size);
    VoC_shr16_ri(REG4, -1, DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG4, SpxPpState_fft_lookup_N);

    /*
    if (st2) {
        int i;
        st2->nfft=nfft;
        st2->inverse = inverse_fft;
        for (i=0;i<nfft;++i) {
            spx_word32_t phase = i;
            spx_word32_t tmp;
            if (!st2->inverse)
                phase = -phase;
            tmp = DIV32(SHL32(phase,17),nfft);
            //kf_cexp2(st2->twiddles+i, DIV32(SHL32(phase,17),nfft));
            //kf_cexp2(st2->twiddles+i, tmp);
            (st2->twiddles+i)->r = spx_cos_norm((tmp));
            //printf("tmp: %x\ttmp-32768: %x\tr: %x\t", tmp, (tmp-32768), (st2->twiddles+i)->r);
            (st2->twiddles+i)->i = spx_cos_norm((tmp-32768));
            //printf("i: %x\n", (st2->twiddles+i)->i);
        }
        kf_factor(nfft,st2->factors);
    }
    */
    VoC_lw16_d(REG4, SpxPpState_ps_size);
    /*
    VoC_lw16i_short(REG5, 0, DEFAULT);
    VoC_sw16_d(REG4, SpxPpState_fft_lookup_forward_substate_nfft);
    VoC_sw16_d(REG5, SpxPpState_fft_lookup_forward_substate_inverse);
    VoC_lw32z(REG67, DEFAULT);
    VoC_lw32z(RL7, IN_PARALLEL);
    VoC_lw16i_short(REG5, 0 ,DEFAULT);
    VoC_lw16i_set_inc(REG2, SpxPpState_fft_lookup_forward_substate_twiddles, 1);
    SPX_FFT_INIT_LOOP1_BEGIN:
    VoC_be16_rr(SPX_FFT_INIT_LOOP1_END, REG5, REG4)
        VoC_push32(REG67, DEFAULT);
        VoC_shr32_ri(REG67, -17, DEFAULT);
        VoC_movreg32(RL6, REG67, DEFAULT);
        VoC_movreg16(RL7_LO, REG4, IN_PARALLEL);
        //VoC_jal(speex_div32_16_sign);// result in reg01
        VoC_push32(REG45, DEFAULT);
        VoC_jal(speex_div32_16);// result in reg01
        VoC_pop32(REG45, DEFAULT);

        VoC_push32(REG01, DEFAULT);
        VoC_jal(spx_cos_norm); // result in reg0
        VoC_lw16i_short(INC2, 1, DEFAULT);
        VoC_NOP();
        VoC_sw16inc_p(REG0, REG2, DEFAULT);

        VoC_pop32(REG01, DEFAULT);
        VoC_NOP();
        VoC_sub32_rd(REG01, REG01, SPEEX_CONST_32768);
        VoC_jal(spx_cos_norm); // result in reg0
        VoC_lw16i_short(INC2, 1, DEFAULT);
        VoC_NOP();
        VoC_sw16inc_p(REG0, REG2, DEFAULT);

        VoC_pop32(REG67, DEFAULT);
        VoC_sub32_rd(REG67, REG67, SPEEX_CONST_1);
        VoC_add16_rd(REG5, REG5, SPEEX_CONST_1);
        VoC_jump(SPX_FFT_INIT_LOOP1_BEGIN);
    SPX_FFT_INIT_LOOP1_END:
    */
    VoC_lw16i_short(REG5, 0, DEFAULT);
    VoC_sw16_d(REG4, SpxPpState_fft_lookup_forward_substate_nfft);
    VoC_sw16_d(REG5, SpxPpState_fft_lookup_forward_substate_inverse);
    VoC_lw32z(REG67, DEFAULT);
    VoC_lw32z(RL7, IN_PARALLEL);
    VoC_lw16i_short(REG5, 0 ,DEFAULT);
    VoC_lw16i_set_inc(REG2, SpxPpState_fft_lookup_forward_substate_twiddles, 1);
SPX_FFT_INIT_LOOP1_BEGIN:
    VoC_be16_rr(SPX_FFT_INIT_LOOP1_END, REG5, REG4)
    VoC_push32(REG67, DEFAULT);
    VoC_shr32_ri(REG67, -17, DEFAULT);
    VoC_movreg32(RL6, REG67, DEFAULT);
    VoC_movreg16(RL7_LO, REG4, IN_PARALLEL);
    //VoC_jal(speex_div32_16_sign);// result in reg01
    VoC_push32(REG45, DEFAULT);
    VoC_jal(speex_div32_16_sign);
    VoC_pop32(REG45, DEFAULT)

    VoC_push32(REG01, DEFAULT);
    VoC_jal(spx_cos_norm); // result in reg0
    VoC_lw16i_short(INC2, 1, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG0, REG2, DEFAULT);

    VoC_pop32(REG01, DEFAULT);
    VoC_sub32_rd(REG01, REG01, SPEEX_CONST_32768);
    VoC_jal(spx_cos_norm); // result in reg0
    VoC_lw16i_short(INC2, 1, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG0, REG2, DEFAULT);

    VoC_pop32(REG67, DEFAULT);
    VoC_sub32_rd(REG67, REG67, SPEEX_CONST_1);
    VoC_add16_rd(REG5, REG5, SPEEX_CONST_1);
    VoC_jump(SPX_FFT_INIT_LOOP1_BEGIN);
SPX_FFT_INIT_LOOP1_END:

    VoC_lw16i_set_inc(REG2, SpxPpState_fft_lookup_forward_substate_factors, 1);
    VoC_movreg16(RL6_LO, REG4, DEFAULT);
    VoC_lw16i_short(RL6_HI, 0, IN_PARALLEL);
    VoC_jal(spx_kf_factor);

    /*
    for (i=0;i<nfft;++i) {
       spx_word32_t phase = i+(nfft>>1);
       spx_word32_t tmp;
       if (!inverse_fft)
          phase = -phase;
       tmp = DIV32(SHL32(phase,16),nfft);
       //kf_cexp2(st->super_twiddles+i, tmp);
       (st->super_twiddles+i)->r = spx_cos_norm((tmp));
       (st->super_twiddles+i)->i = spx_cos_norm((tmp-32768));
    }
    */
    /*
    VoC_lw32z(REG67, DEFAULT);
    VoC_lw32z(RL7, IN_PARALLEL);
    VoC_lw16i_short(REG5, 0 ,DEFAULT);
    VoC_movreg16(REG6, REG4, IN_PARALLEL);
    VoC_sub32_dr(REG67, SPEEX_CONST_0, REG67);
    VoC_shr32_ri(REG67, 1, DEFAULT);
    VoC_lw16i_set_inc(REG2, SpxPpState_fft_lookup_forward_super_twiddles, 1);
    SPX_FFT_INIT_LOOP3_BEGIN:
    VoC_be16_rr(SPX_FFT_INIT_LOOP3_END, REG5, REG4)
        VoC_push32(REG67, DEFAULT);
        VoC_shr32_ri(REG67, -16, DEFAULT);
        VoC_movreg32(RL6, REG67, DEFAULT);
        VoC_movreg16(RL7_LO, REG4, IN_PARALLEL);
        //VoC_jal(speex_div32_16_sign);// result in reg01
        VoC_push32(REG45, DEFAULT);
        VoC_jal(speex_div32_16);
        VoC_pop32(REG45, DEFAULT);

        VoC_push32(REG01, DEFAULT);
        VoC_jal(spx_cos_norm); // result in reg0
        VoC_lw16i_short(INC2, 1, DEFAULT);
        VoC_NOP();
        VoC_sw16inc_p(REG0, REG2, DEFAULT);

        VoC_pop32(REG01, DEFAULT);
        VoC_sub32_rd(REG01, REG01, SPEEX_CONST_32768);
        VoC_jal(spx_cos_norm); // result in reg0
        VoC_lw16i_short(INC2, 1, DEFAULT);
        VoC_NOP();
        VoC_sw16inc_p(REG0, REG2, DEFAULT);

        VoC_pop32(REG67, DEFAULT);
        VoC_sub32_rd(REG67, REG67, SPEEX_CONST_1);
        VoC_add16_rd(REG5, REG5, SPEEX_CONST_1);
        VoC_jump(SPX_FFT_INIT_LOOP3_BEGIN);
    SPX_FFT_INIT_LOOP3_END:
    */
    VoC_lw32z(REG67, DEFAULT);
    VoC_lw32z(RL7, IN_PARALLEL);
    VoC_lw16i_short(REG5, 0 ,DEFAULT);
    VoC_movreg16(REG6, REG4, IN_PARALLEL);
    VoC_shr32_ri(REG67, 1, DEFAULT);
    VoC_sub32_dr(REG67, SPEEX_CONST_0, REG67);
    VoC_lw16i_set_inc(REG2, SpxPpState_fft_lookup_forward_super_twiddles, 1);
SPX_FFT_INIT_LOOP3_BEGIN:
    VoC_be16_rr(SPX_FFT_INIT_LOOP3_END, REG5, REG4)
    VoC_push32(REG67, DEFAULT);
    VoC_shr32_ri(REG67, -16, DEFAULT);
    VoC_movreg32(RL6, REG67, DEFAULT);
    VoC_movreg16(RL7_LO, REG4, IN_PARALLEL);
    //VoC_jal(speex_div32_16_sign);// result in reg01
    VoC_push32(REG45, DEFAULT);
    VoC_jal(speex_div32_16_sign);
    VoC_pop32(REG45, DEFAULT);

    VoC_push32(REG01, DEFAULT);
    VoC_jal(spx_cos_norm); // result in reg0
    VoC_lw16i_short(INC2, 1, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG0, REG2, DEFAULT);

    VoC_pop32(REG01, DEFAULT);
    VoC_sub32_rd(REG01, REG01, SPEEX_CONST_32768);
    VoC_jal(spx_cos_norm); // result in reg0
    VoC_lw16i_short(INC2, 1, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG0, REG2, DEFAULT);

    VoC_pop32(REG67, DEFAULT);
    VoC_sub32_rd(REG67, REG67, SPEEX_CONST_1);
    VoC_add16_rd(REG5, REG5, SPEEX_CONST_1);
    VoC_jump(SPX_FFT_INIT_LOOP3_BEGIN);
SPX_FFT_INIT_LOOP3_END:

    /*
    if (st2) {
        int i;
        st2->nfft=nfft;
        st2->inverse = inverse_fft;
        for (i=0;i<nfft;++i) {
            spx_word32_t phase = i;
            spx_word32_t tmp;
            if (!st2->inverse)
                phase = -phase;
            tmp = DIV32(SHL32(phase,17),nfft);
            //kf_cexp2(st2->twiddles+i, DIV32(SHL32(phase,17),nfft));
            //kf_cexp2(st2->twiddles+i, tmp);
            (st2->twiddles+i)->r = spx_cos_norm((tmp));
            //printf("tmp: %x\ttmp-32768: %x\tr: %x\t", tmp, (tmp-32768), (st2->twiddles+i)->r);
            (st2->twiddles+i)->i = spx_cos_norm((tmp-32768));
            //printf("i: %x\n", (st2->twiddles+i)->i);
        }
        kf_factor(nfft,st2->factors);
    }
    */
    VoC_lw16i_short(REG5, 1, DEFAULT);
    VoC_sw16_d(REG4, SpxPpState_fft_lookup_backward_substate_nfft);
    VoC_sw16_d(REG5, SpxPpState_fft_lookup_backward_substate_inverse);
    VoC_lw32z(REG67, DEFAULT);
    VoC_lw32z(RL7, IN_PARALLEL);
    VoC_lw16i_short(REG5, 0 ,DEFAULT);
    VoC_lw16i_set_inc(REG2, SpxPpState_fft_lookup_backward_substate_twiddles, 1);
SPX_FFT_INIT_LOOP2_BEGIN:
    VoC_be16_rr(SPX_FFT_INIT_LOOP2_END, REG5, REG4)
    VoC_push32(REG67, DEFAULT);
    VoC_shr32_ri(REG67, -17, DEFAULT);
    VoC_movreg32(RL6, REG67, DEFAULT);
    VoC_movreg16(RL7_LO, REG4, IN_PARALLEL);
    //VoC_jal(speex_div32_16_sign);// result in reg01
    VoC_push32(REG45, DEFAULT);
    VoC_jal(speex_div32_16_sign);
    VoC_pop32(REG45, DEFAULT)

    VoC_push32(REG01, DEFAULT);
    VoC_jal(spx_cos_norm); // result in reg0
    VoC_lw16i_short(INC2, 1, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG0, REG2, DEFAULT);

    VoC_pop32(REG01, DEFAULT);
    VoC_sub32_rd(REG01, REG01, SPEEX_CONST_32768);
    VoC_jal(spx_cos_norm); // result in reg0
    VoC_lw16i_short(INC2, 1, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG0, REG2, DEFAULT);

    VoC_pop32(REG67, DEFAULT);
    VoC_add32_rd(REG67, REG67, SPEEX_CONST_1);
    VoC_add16_rd(REG5, REG5, SPEEX_CONST_1);
    VoC_jump(SPX_FFT_INIT_LOOP2_BEGIN);
SPX_FFT_INIT_LOOP2_END:
    VoC_lw16i_set_inc(REG2, SpxPpState_fft_lookup_backward_substate_factors, 1);
    VoC_movreg16(RL6_LO, REG4, DEFAULT);
    VoC_lw16i_short(RL6_HI, 0, IN_PARALLEL);
    VoC_jal(spx_kf_factor);

    /*
    for (i=0;i<nfft;++i) {
       spx_word32_t phase = i+(nfft>>1);
       spx_word32_t tmp;
       if (!inverse_fft)
          phase = -phase;
       tmp = DIV32(SHL32(phase,16),nfft);
       //kf_cexp2(st->super_twiddles+i, tmp);
       (st->super_twiddles+i)->r = spx_cos_norm((tmp));
       (st->super_twiddles+i)->i = spx_cos_norm((tmp-32768));
    }
    */
    VoC_lw32z(REG67, DEFAULT);
    VoC_lw32z(RL7, IN_PARALLEL);
    VoC_lw16i_short(REG5, 0 ,DEFAULT);
    VoC_movreg16(REG6, REG4, IN_PARALLEL);
    VoC_shr32_ri(REG67, 1, DEFAULT);
    VoC_lw16i_set_inc(REG2, SpxPpState_fft_lookup_backward_super_twiddles, 1);
SPX_FFT_INIT_LOOP4_BEGIN:
    VoC_be16_rr(SPX_FFT_INIT_LOOP4_END, REG5, REG4)
    VoC_push32(REG67, DEFAULT);
    VoC_shr32_ri(REG67, -16, DEFAULT);
    VoC_movreg32(RL6, REG67, DEFAULT);
    VoC_movreg16(RL7_LO, REG4, IN_PARALLEL);
    //VoC_jal(speex_div32_16_sign);// result in reg01
    VoC_push32(REG45, DEFAULT);
    VoC_jal(speex_div32_16_sign);
    VoC_pop32(REG45, DEFAULT);

    VoC_push32(REG01, DEFAULT);
    VoC_jal(spx_cos_norm); // result in reg0
    VoC_lw16i_short(INC2, 1, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG0, REG2, DEFAULT);

    VoC_pop32(REG01, DEFAULT);
    VoC_sub32_rd(REG01, REG01, SPEEX_CONST_32768);
    VoC_jal(spx_cos_norm); // result in reg0
    VoC_lw16i_short(INC2, 1, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG0, REG2, DEFAULT);

    VoC_pop32(REG67, DEFAULT);
    VoC_add32_rd(REG67, REG67, SPEEX_CONST_1);
    VoC_add16_rd(REG5, REG5, SPEEX_CONST_1);
    VoC_jump(SPX_FFT_INIT_LOOP4_BEGIN);
SPX_FFT_INIT_LOOP4_END:

    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}
#endif
//VoC_directive:PARSER_ON

/*****************
 * input :  SpeexPreprocessState *st : SpxPpState
            spx_int16_t *x : SPEEX_REF_BUF
 *****************/

void preprocess_analysis(void)
{
    VoC_push16(RA, DEFAULT);

    VoC_lw16_d(REG0, SpxPpState_ps_size); // r0 = N, should not be changed

    /*
    for (i=0;i<N;i++)
        st->frame[i]=st->inbuf[i];
    for (i=0;i<N;i++)
    {
        st->frame[N+i]=x[i];
        st->inbuf[i]=x[i];
    }
    */
    /*
    VoC_lw16i_set_inc(REG1, SpxPpState_inbuf, 1);
    VoC_lw16i_set_inc(REG2, SpxPpState_frame, 1); // r2 point to st->frame[]
    VoC_lw16i_set_inc(REG3, SPEEX_REF_BUF, 1);  // r3 point to x[i]
    VoC_loop_r_short(REG0, DEFAULT);
    VoC_movreg16(REG5, REG0, IN_PARALLEL); // save r0 in r5 as a backup
    VoC_startloop0
        VoC_lw16inc_p(REG4, REG1, DEFAULT);
        VoC_NOP();
        VoC_sw16inc_p(REG4, REG2, DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG1, SpxPpState_inbuf, 1);
    VoC_loop_r_short(REG0, DEFAULT);
    VoC_push16(REG0, IN_PARALLEL); // push r0
    VoC_startloop0
        VoC_lw16inc_p(REG4, REG3, DEFAULT);
        VoC_NOP();
        VoC_sw16inc_p(REG4, REG2, DEFAULT);
        VoC_sw16inc_p(REG4, REG1, DEFAULT);
    VoC_endloop0
    */

    /*
    for (i=0;i<N;i+=2)
    {
        st->frame[i]=st->inbuf[i];
        st->frame[i+1]=st->inbuf[i+1];
    }
    for (i=0;i<N;i+=2)
    {
        st->frame[N+i]=x[i];
        st->frame[N+i+1]=x[i+1];
        st->inbuf[i]=x[i];
        st->inbuf[i+1]=x[i+1];
    }
    */
    VoC_lw16i_set_inc(REG1, SpxPpState_inbuf, 2);
    VoC_lw16i_set_inc(REG2, SpxPpState_frame, 2); // r2 point to st->frame[]
    VoC_lw16i_set_inc(REG3, SPEEX_REF_BUF, 2);  // r3 point to x[i]
    VoC_movreg16(REG4, REG0, DEFAULT);
    VoC_movreg16(REG5, REG0, IN_PARALLEL); // save r0 in r5 as a backup
    VoC_shr16_ri(REG4, 1, DEFAULT);
    VoC_loop_r_short(REG4, DEFAULT);
    VoC_lw32inc_p(RL7, REG1, IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(RL7, REG1, DEFAULT);
    //VoC_NOP();
    //Exit_on_warnings=OFF;
    // sw value before the last instruction update
    VoC_sw32inc_p(RL7, REG2, DEFAULT);
    //Exit_on_warnings=ON;
    VoC_endloop0

    VoC_lw16i_set_inc(REG1, SpxPpState_inbuf, 2);
    VoC_loop_r_short(REG4, DEFAULT);
    //VoC_lw32inc_p(RL7, REG3, IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(RL7, REG3, DEFAULT);
    VoC_NOP();
    VoC_sw32inc_p(RL7, REG2, DEFAULT);
    VoC_sw32inc_p(RL7, REG1, DEFAULT);
    VoC_endloop0

    /*
    for (i=0;i<2*N;i++)
    {
        st->frame[i] = MULT16_16_Q15(st->frame[i], st->window[i]);
        max_val = MAX16(max_val, ABS16(st->frame[i]));
    }
    */
    VoC_lw16i_set_inc(REG2, SpxPpState_frame, 1);
    VoC_lw16i_set_inc(REG3, SpxPpState_window, 1);
    VoC_lw16i_short(REG4, 0, DEFAULT);
    VoC_shr16_ri(REG5, -1, IN_PARALLEL);
    VoC_lw16i_short(FORMAT32, 15, DEFAULT);
    //VoC_lw16i_short(FORMAT16, 15, DEFAULT);
    VoC_lw32z(REG67, IN_PARALLEL); // max_val in reg6
    VoC_loop_r_short(REG5, DEFAULT);
    VoC_startloop0
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16inc_p(REG4, REG3, IN_PARALLEL);
    VoC_mac32_rp(REG4, REG2, DEFAULT);
    //VoC_multf32_rp(REG01, REG4, REG2, DEFAULT);
    //printf("format[%d]: %x\twindow[%d]: %x\n", REGS[4].reg, REGS[2].reg, REGS[4].reg, REGS[3].reg);
    VoC_NOP();
    VoC_movreg32(REG01, ACC0, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG0, REG2, DEFAULT);
    //printf("format[%d]: %x\t", REGS[4].reg, REGS[0].reg);
    VoC_bgtz16_r(Pp_analysis_Frame_gtz, REG0)
    VoC_sub16_dr(REG0, SPEEX_CONST_0, REG0);
Pp_analysis_Frame_gtz:
    VoC_bgt16_rr(Pp_analysis_max_val, REG6, REG0)
    VoC_movreg16(REG6, REG0, DEFAULT);
Pp_analysis_max_val:
    VoC_NOP();
    //printf("maxval: %x\n", REGS[6].reg);
    VoC_endloop0

    VoC_bnltz16_r(SPEEX_PRE_EXTEND_MAX_VAL, REG6)
    VoC_lw16i(REG7, 0xffff);
SPEEX_PRE_EXTEND_MAX_VAL:
    //st->frame_shift = 14-spx_ilog2(EXTEND32(max_val));
    VoC_jal(spx_ilog2); // spx_ilog2(max_val) in reg1
    /*
    VoC_sub16_rd(REG1, REG1, SPEEX_CONST_14);
    VoC_sub16_dr(REG2, SPEEX_CONST_0, REG1);
    VoC_NOP();
    */
    VoC_sub16_dr(REG2, SPEEX_CONST_14, REG1);
    VoC_sub16_rd(REG1, REG1, SPEEX_CONST_14);
    VoC_sw16_d(REG2, SpxPpState_frame_shift);

    //for (i=0;i<2*N;i++)
    //  st->frame[i] = SHL16(st->frame[i], st->frame_shift);
    // VoC_loop_r_short(REG5, DEFAULT);
    VoC_lw16_d(REG0, SpxPpState_ps_size);
    VoC_lw16i_set_inc(REG2, SpxPpState_frame, 2);
    VoC_lw16i_set_inc(REG3, SpxPpState_frame, 2);
    VoC_loop_r_short(REG0, DEFAULT);
    VoC_lw32inc_p(REG67, REG2, IN_PARALLEL);
    VoC_startloop0
    VoC_shr16_rr(REG6, REG1, DEFAULT);
    VoC_shr16_rr(REG7, REG1, IN_PARALLEL);
    VoC_lw32inc_p(REG67, REG2, DEFAULT);
    exit_on_warnings=OFF;
    // SW before REG45 update
    VoC_sw32inc_p(REG67, REG3, DEFAULT);
    exit_on_warnings=ON;
    //printf("frame[%d]: 0x%x\n", REGS[7].reg, REGS[6].reg);
    VoC_endloop0

    VoC_lw16i(REG0, SpxPpState_frame);
    VoC_lw16i(REG1, SpxPpState_ft);
    VoC_sw16_d(REG0, SpxPpState_fft_in_ADDR);
    VoC_sw16_d(REG1, SpxPpState_fft_out_ADDR);
    //VoC_jal(spx_fft);
    VoC_jal(spx_kiss_fftr2);

    /*
    // Power spectrum
    st->ps[0] = MULT16_16(st->ft[0],st->ft[0]);
    st->ps[0] = PSHR32(st->ps[0], 2*st->frame_shift);
    for (i=1;i<N;i++)
    {
        st->ps[i]=MULT16_16(st->ft[2*i-1],st->ft[2*i-1]) + MULT16_16(st->ft[2*i],st->ft[2*i]);
        st->ps[i] = PSHR32(st->ps[i], 2*st->frame_shift);
    }
    */
    //VoC_lw32z(ACC0, DEFAULT);
    //VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);

    VoC_lw16_d(REG1, SpxPpState_frame_shift);

    VoC_shr16_ri(REG1, -1,  DEFAULT); // r1 = 2*st->frame_shift
    VoC_lw16i_short(REG4, 1, IN_PARALLEL); // r45 = 1;
    VoC_lw16i_short(REG5, 0, DEFAULT);
    VoC_movreg16(REG2, REG1, IN_PARALLEL);

    //VoC_sub16_dr(REG2, SPEEX_CONST_1, REG2); // r2 = 1 - 2*st->frame_shift
    VoC_sub16_rr(REG2, REG4, REG2, DEFAULT);

    VoC_lw16i_set_inc(REG0, SpxPpState_ft, 1); // r0 POINT to ft
    VoC_lw16i_set_inc(REG3, SpxPpState_ps, 2); // r3 POINT to ps

    VoC_lw16inc_p(REG6, REG0, DEFAULT);
    VoC_shr32_rr(REG45, REG2, IN_PARALLEL); //r45 = 1 << (2*st->frame_shift-1)

    //VoC_lw32z(ACC0, DEFAULT);
    VoC_movreg32(ACC0, REG45, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    VoC_mac32_rr(REG6, REG6, DEFAULT);
    VoC_NOP();
    //VoC_add32_rr(ACC0, ACC0, REG45, DEFAULT);
    VoC_shr32_rr(ACC0, REG1, DEFAULT);
    VoC_NOP();
    VoC_sw32inc_p(ACC0, REG3, DEFAULT);

    VoC_lw16_d(REG6, SpxPpState_ps_size);
    VoC_sub16_rd(REG6, REG6, SPEEX_CONST_1);
    VoC_loop_r_short(REG6, DEFAULT);
    VoC_startloop0
    //VoC_lw32z(ACC0, DEFAULT);
    VoC_movreg32(ACC0, REG45, DEFAULT);
    //VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    //VoC_lw32z(ACC1, IN_PARALLEL);
    //if (REGS[0].reg == 0x0a45)
    //  VoC_NOP();
    VoC_mac32inc_pp(REG0, REG0, DEFAULT);
    //VoC_NOP();
    VoC_mac32inc_pp(REG0, REG0, DEFAULT);
    VoC_NOP();
    //VoC_add32_rr(ACC0, ACC0, REG45, DEFAULT);
    VoC_shr32_rr(ACC0, REG1, DEFAULT);
    VoC_NOP();
    VoC_sw32inc_p(ACC0, REG3, DEFAULT);
    VoC_endloop0

    VoC_lw16i(REG4, SpxPpState_ps);
    VoC_lw16i(REG5, SpxPpState_ps_M);
    VoC_jal(filterbank_compute_bank32);

    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/*************************
 * INPUT :  kiss_fftr_cfg st, SpxPpState_fft_lookup_forward
            const kiss_fft_scalar *timedata, REG3
            kiss_fft_scalar *freqdata, REG2
 *************************/
void spx_kiss_fftr2(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push32(REG01, DEFAULT);
    VoC_push16(REG5, DEFAULT);

    //VoC_movreg16(REG2, REG0, DEFAULT);
    VoC_lw16i_set_inc(REG0, SpxPpState_fft_lookup_forward_substate, 1);
    VoC_lw16i_set_inc(REG1, SpxPpState_fft_lookup_forward_substate_factors, 2);
    VoC_lw16i_set_inc(REG2, SpxPpState_fft_lookup_forward_tmpbuf, 2);
    //VoC_lw16i_set_inc(REG3, SpxPpState_frame, 2);
    VoC_lw16d_set_inc(REG3, SpxPpState_fft_in_ADDR, 2);
    VoC_lw16i_short(REG4, 1, DEFAULT);
    VoC_lw16i_short(REG7, 2, IN_PARALLEL);
    VoC_jal(spx_kf_shuffle);

    VoC_lw16i_short(REG6, 1, DEFAULT);
    VoC_lw16i_short(RL7_LO, 1, DEFAULT);
    VoC_lw16i_short(RL7_HI, 1, IN_PARALLEL);
    VoC_jal(spx_kf_work);

    /*
    tdc.r = st->tmpbuf[0].r;
    tdc.i = st->tmpbuf[0].i;
    //C_FIXDIV(tdc,2);
    tdc.r = (((tdc.r*16383) + (1<<14))>>15);
    tdc.i = (((tdc.i*16383) + (1<<14))>>15);
    CHECK_OVERFLOW_OP(tdc.r ,+, tdc.i);
    CHECK_OVERFLOW_OP(tdc.r ,-, tdc.i);
    freqdata[0] = tdc.r + tdc.i;
    freqdata[2*ncfft-1] = tdc.r - tdc.i;
    */
    // REG2 POINT to st->tmpbuf

    VoC_lw16_d(REG0, SpxPpState_fft_lookup_forward_substate_nfft);
    //VoC_lw16i_set_inc(REG3, SpxPpState_ft, 1);
    VoC_lw16d_set_inc(REG3, SpxPpState_fft_out_ADDR, 1);
    VoC_lw16i_set_inc(REG2, SpxPpState_fft_lookup_forward_tmpbuf, 2);

    VoC_shr16_ri(REG0, -1, DEFAULT);
    //VoC_sub16_rd(REG0, REG0, SPEEX_CONST_1);

    VoC_add16_rr(REG1, REG0, REG3, DEFAULT);
    VoC_add16_rr(REG0, REG0, REG2, IN_PARALLEL); // reg0 point to tmpbuf[ncfft-1].r+2
    VoC_sub16_rd(REG1, REG1, SPEEX_CONST_1); // REG1 POINT to freqdata[2*ncfft-1]
    VoC_lw16i_short(INC0, -2, DEFAULT);
    //VoC_lw16i_short(INC2, 2, IN_PARALLEL);
    VoC_lw16i_short(INC1, -1, DEFAULT);
    //VoC_lw16i_short(INC3, 1, IN_PARALLEL);


    VoC_lw32inc_p(REG45, REG2, DEFAULT);
    VoC_inc_p(REG0, IN_PARALLEL);
    VoC_lw16i(REG6, 16383);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_lw16i_short(FORMAT32, 0, DEFAULT);

    VoC_mac32_rr(REG4, REG6, DEFAULT);
    VoC_mac32_rr(REG5, REG6, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);

    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_movreg32(RL7, ACC1, IN_PARALLEL);
    VoC_movreg16(REG4, RL6_LO, DEFAULT);
    VoC_movreg16(REG5, RL7_LO, IN_PARALLEL); // tdc in reg45

    VoC_add16_rr(REG6, REG4, REG5, DEFAULT);
    VoC_sub16_rr(REG7, REG4, REG5, IN_PARALLEL);
    VoC_NOP();
    VoC_sw16inc_p(REG6, REG3, DEFAULT);
    VoC_sw16inc_p(REG7, REG1, DEFAULT);

    // reg1 -> freqdata[2*ncfft-1], REG3 -> freqdata[0]
    // reg0 -> tmpbuf[ncfft-k], REG2 -> tmpbuf[0]
    VoC_lw16_d(REG6, SpxPpState_fft_lookup_forward_substate_nfft);

    VoC_shr16_ri(REG6, 1, DEFAULT);
    VoC_lw16i(REG7, SpxPpState_fft_lookup_forward_super_twiddles);

    VoC_add16_rd(REG7, REG7, SPEEX_CONST_2);
    VoC_lw16i_short(INC0, 1, DEFAULT);
    VoC_lw16i_short(INC2, 1, DEFAULT);
    VoC_loop_r(0, REG6)
    // twr = SHR32(SUB32(MULT16_16(f2k.r,st->super_twiddles[k].r),MULT16_16(f2k.i,st->super_twiddles[k].i)), 1);
    // twi = SHR32(ADD32(MULT16_16(f2k.i,st->super_twiddles[k].r),MULT16_16(f2k.r,st->super_twiddles[k].i)), 1);
    //if (REGS[3].reg == 0x0a43)
    //  VoC_NOP();
    VoC_lw32_p(REG45, REG7, DEFAULT);

    //VoC_add16_rd(REG7, REG7, SPEEX_CONST_1);
    VoC_push16(REG7, DEFAULT); // st->super_twiddles[k] in REG45
    VoC_push32(REG45, DEFAULT);

    VoC_lw32z(RL6, DEFAULT);
    VoC_lw32z(RL7, IN_PARALLEL);
    VoC_lw16inc_p(REG4, REG2, DEFAULT);
    VoC_lw16inc_p(REG5, REG0, DEFAULT);

    VoC_sub16_rr(REG6, REG4, REG5, DEFAULT);
    VoC_add16_rr(REG7, REG4, REG5, IN_PARALLEL);

    //VoC_push16(REG6, DEFAULT);
    //VoC_push16(REG7, DEFAULT);
    VoC_shr16_ri(REG6, 1, DEFAULT);
    VoC_movreg16(RL6_LO, REG7, IN_PARALLEL);
    VoC_bnltz16_r(SPX_KISS_FFTR2_NEG_EXTEND1, REG7)
    VoC_lw16i(RL6_HI, 0xffff);
SPX_KISS_FFTR2_NEG_EXTEND1:

    VoC_shr32_ri(RL6, -13, DEFAULT); // f2k.r in REG6, f1kr in RL6
    VoC_push16(REG6, DEFAULT);

    //VoC_add16_rd(REG2, REG2, SPEEX_CONST_1);
    //VoC_add16_rd(REG0, REG0, SPEEX_CONST_1);
    //VoC_pop16(REG7, DEFAULT);
    //VoC_pop16(REG6, DEFAULT);
    VoC_lw16inc_p(REG4, REG2, DEFAULT);
    VoC_lw16inc_p(REG5, REG0, DEFAULT);

    VoC_add16_rr(REG7, REG4, REG5, DEFAULT);
    VoC_sub16_rr(REG6, REG4, REG5, IN_PARALLEL);

    VoC_add16_rd(REG7, REG7, SPEEX_CONST_1);
    VoC_movreg16(RL7_LO, REG6, DEFAULT);
    VoC_bnltz16_r(SPX_KISS_FFTR2_NEG_EXTEND2, REG6)
    VoC_lw16i(RL7_HI, 0xffff);
SPX_KISS_FFTR2_NEG_EXTEND2:

    VoC_shr16_ri(REG7, 1, DEFAULT);
    VoC_shr32_ri(RL7, -13, IN_PARALLEL); // f2k.i in REG7, f1ki in RL7
    VoC_pop16(REG6, DEFAULT);

    //VoC_add16_rd(REG2, REG2, SPEEX_CONST_1);
    //VoC_add16_rd(REG0, REG0, SPEEX_CONST_1);
    VoC_pop32(REG45, DEFAULT);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG5, REG7, DEFAULT);
    VoC_mac32_rr(REG4, REG7, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);

    VoC_mac32_rr(REG4, REG6, DEFAULT);
    VoC_mac32_rr(REG5, REG6, IN_PARALLEL);
    VoC_NOP();
    VoC_shr32_ri(ACC0, 1, DEFAULT);
    VoC_shr32_ri(ACC1, 1, IN_PARALLEL); // twr in ACC0, twi in ACC1

    /*
    freqdata[2*k-1] = SATURATE16(PSHR32(f1kr + twr, 15),32767);
    freqdata[2*k] = SATURATE16(PSHR32(f1ki + twi, 15),32767);
    freqdata[2*(ncfft-k)-1] = SATURATE16(PSHR32(f1kr - twr, 15),32767);
    freqdata[2*(ncfft-k)] = SATURATE16(PSHR32(twi - f1ki, 15),32767);
    */

    VoC_add32_rr(REG45, RL6, ACC0, DEFAULT);
    VoC_sub32_rr(REG67, ACC1, RL7, IN_PARALLEL);

    VoC_add32_rd(REG45, REG45, SPEEX_CONST_16384);
    VoC_add32_rd(REG67, REG67, SPEEX_CONST_16384);

    VoC_shr32_ri(REG45, 15, DEFAULT);
    VoC_shr32_ri(REG67, 15, IN_PARALLEL);
    VoC_NOP();
    VoC_sw16inc_p(REG4, REG3, DEFAULT);
    VoC_sw16inc_p(REG6, REG1, DEFAULT);

    //if (REGS[4].reg == 0xbff3 || REGS[6].reg == 0xbff3)
    //  VoC_NOP();

    VoC_add32_rr(REG45, RL7, ACC1, DEFAULT);
    VoC_sub32_rr(REG67, RL6, ACC0, IN_PARALLEL);

    VoC_add32_rd(REG45, REG45, SPEEX_CONST_16384);
    VoC_add32_rd(REG67, REG67, SPEEX_CONST_16384);

    VoC_shr32_ri(REG45, 15, DEFAULT);
    VoC_shr32_ri(REG67, 15, IN_PARALLEL);
    VoC_NOP();
    VoC_sw16inc_p(REG4, REG3, DEFAULT);
    VoC_sw16inc_p(REG6, REG1, DEFAULT);

    //if (REGS[4].reg == 0xbff3 || REGS[6].reg == 0xbff3)
    //  VoC_NOP();

    VoC_pop16(REG7, DEFAULT);

    VoC_add16_rd(REG7, REG7, SPEEX_CONST_2);
    VoC_sub16_rd(REG0, REG0, SPEEX_CONST_4);
    VoC_endloop0

    VoC_pop16(REG5, DEFAULT);
    VoC_pop32(REG01, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/************************
 * INPUT :  void *table, st->fft_lookup
            spx_word16_t *in, REG0
            spx_word16_t *out, REG1
 * output : none
 ************************/
// now working as intended
void spx_fft(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push32(REG01, DEFAULT);

    VoC_lw16d_set_inc(REG0, SpxPpState_fft_in_ADDR, 1);
    VoC_lw16d_set_inc(REG1, SpxPpState_fft_out_ADDR, 1);
    /*
    for (i=0;i<len;i++)
    max_val = MAX16(max_val, ABS16(in[i]));
    */
    VoC_lw16_d(REG4, SpxPpState_fft_lookup_N);
    VoC_lw32z(REG67, DEFAULT);
    VoC_push16(REG0, IN_PARALLEL);
    VoC_loop_r_short(REG4, DEFAULT);
    //VoC_lw16i_short(INC0, 1, IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG5, REG0, DEFAULT);
    //printf("in[%d]: 0x%x\n", REGS[7].reg, REGS[5].reg);
    VoC_bgtz16_r(SPX_FFT_IN_GTZ, REG5)
    VoC_sub16_dr(REG5, SPEEX_CONST_0, REG5);
SPX_FFT_IN_GTZ:
    VoC_bgt16_rr(SPX_FFT_MAX_VAL_MAX16, REG6, REG5)
    VoC_movreg16(REG6, REG5, DEFAULT);
SPX_FFT_MAX_VAL_MAX16:
    VoC_NOP();
    VoC_endloop0  // max_val in REG6
    VoC_pop16(REG0, DEFAULT);

    /*
    while (max_val <= half_bound && max_val != 0)
    {
        max_val <<= 1;
        shift++;
    }
    */
    VoC_lw16i(REG4, 16000);
    VoC_lw16i_short(REG5, 0, DEFAULT); // shift in REG5
SPX_FFT_SHIFT_WHILE:
    VoC_bgt16_rr(SPX_FFT_SHIFT, REG6, REG4)
    VoC_bez16_r(SPX_FFT_SHIFT, REG6)
    VoC_shr16_ri(REG6, -1, DEFAULT);
    VoC_add16_rd(REG5, REG5, SPEEX_CONST_1);
    VoC_jump(SPX_FFT_SHIFT_WHILE);
SPX_FFT_SHIFT:

    /*
    for (i=0;i<len;i++)
    {
        in[i] = SHL16(in[i], shift);
    }
    */
    //VoC_lw16i_set_inc(REG0, SpxPpState_frame, 2);
    //VoC_lw16i_set_inc(REG2, SpxPpState_frame, 2);
    VoC_lw16d_set_inc(REG0, SpxPpState_fft_in_ADDR, 2);
    VoC_lw16d_set_inc(REG2, SpxPpState_fft_in_ADDR, 2);
    VoC_lw16_d(REG4, SpxPpState_fft_lookup_N);
    VoC_push16(REG5, DEFAULT);
    VoC_shr16_ri(REG4, 1, IN_PARALLEL);
    VoC_sub16_dr(REG5, SPEEX_CONST_0, REG5);
    VoC_loop_r_short(REG4, DEFAULT);
    VoC_lw32inc_p(REG67, REG2, IN_PARALLEL)
    VoC_startloop0
    VoC_shr16_rr(REG6, REG5, DEFAULT);
    VoC_shr16_rr(REG7, REG5, IN_PARALLEL);
    VoC_lw32inc_p(REG67, REG2, DEFAULT);
    //exit_on_warnings=OFF;
    // sw before REG67 update
    VoC_sw32inc_p(REG67, REG0, DEFAULT);
    //exit_on_warnings=ON;
    VoC_endloop0
    VoC_pop16(REG5, DEFAULT);

    // push REG01, reg45
    VoC_jal(spx_kiss_fftr2);

    /*
    for (i=0; i < t->N; i++)
    {
        in[i] = PSHR16(in[i], shift);
        out[i] = PSHR16(out[i], shift);
    }
    */
    VoC_lw16_d(REG4, SpxPpState_fft_lookup_N);
    //VoC_sub16_rd(REG2, REG5, SPEEX_CONST_1);
    VoC_sub16_dr(REG2, SPEEX_CONST_1, REG5);
    VoC_lw16i_short(REG7, 0, DEFAULT);
    VoC_lw16i_short(REG6, 1, IN_PARALLEL);

    VoC_lw16d_set_inc(REG0, SpxPpState_fft_in_ADDR, 1);
    VoC_lw16d_set_inc(REG1, SpxPpState_fft_out_ADDR, 1);
    //VoC_lw16i_short(INC0, 1, DEFAULT);
    //VoC_lw16i_short(INC1, 1, IN_PARALLEL);
    VoC_loop_r_short(REG4, DEFAULT);
    VoC_shr16_rr(REG6, REG2, IN_PARALLEL); // (1<<(shift-1)) in REG6
    VoC_startloop0
    VoC_lw16_p(REG2, REG0, DEFAULT);
    VoC_lw16_p(REG3, REG1, DEFAULT);

    VoC_add16_rr(REG2, REG2, REG6, DEFAULT);
    VoC_add16_rr(REG3, REG3, REG6, IN_PARALLEL);

    VoC_shr16_rr(REG2, REG5, DEFAULT);
    VoC_shr16_rr(REG3, REG5, IN_PARALLEL);
    VoC_NOP();
    VoC_sw16inc_p(REG2, REG0, DEFAULT);
    VoC_sw16inc_p(REG3, REG1, DEFAULT);
    VoC_endloop0

    VoC_pop32(REG01, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/************************************************************************/
/* filterbank_compute_bank32
 * input bank in st->bank
 * input ps in r4
 * input mel in r5                                                      */
/************************************************************************/
void filterbank_compute_bank32(void)
{
    //VoC_push16(RA, DEFAULT);
    VoC_push32(REG01, DEFAULT);
    VoC_push32(REG23, DEFAULT);

    VoC_push32(REG45, DEFAULT);
    VoC_lw16i_short(INC0, 2, IN_PARALLEL);

    VoC_push32(REG67, DEFAULT);
    VoC_lw32z(RL7, IN_PARALLEL);

    VoC_movreg16(REG0, REG5, DEFAULT);
    VoC_lw16_d(REG6, SpxPpState_bank_nb_banks);
    VoC_NOP();
    //VoC_push16(REG5, DEFAULT);

    VoC_loop_r_short(REG6, DEFAULT);
    //VoC_sw32inc_p(RL7, REG0, IN_PARALLEL);
    VoC_startloop0
    //VoC_sw32_p(RL7, REG5, DEFAULT);
    //VoC_add16_rd(REG5, REG5, SPEEX_CONST_2);
    //VoC_NOP();
    VoC_sw32inc_p(RL7, REG0, DEFAULT);
    VoC_NOP();
    VoC_endloop0
    //VoC_pop16(REG5, DEFAULT);

    VoC_lw16_d(REG6, FILTERBANK_LEN);
    VoC_lw16i_set_inc(REG0, SpxPpState_bank_bank_left, 1);
    VoC_lw16i_set_inc(REG1, SpxPpState_bank_bank_right, 1);
    VoC_lw16i_set_inc(REG2, SpxPpState_bank_filter_left, 1);
    VoC_lw16i_set_inc(REG3, SpxPpState_bank_filter_right, 1);
    VoC_lw16i_short(REG7, 0, DEFAULT);
    /*
    for (i=0;i<bank->len;i++)
    {
        int id;
        id = bank->bank_left[i];
        mel[id] += MULT16_32_P15(bank->filter_left[i],ps[i]);
        id = bank->bank_right[i];
        mel[id] += MULT16_32_P15(bank->filter_right[i],ps[i]);
    }
    */
    VoC_loop_r_short(REG6, DEFAULT);
    VoC_startloop0
    VoC_push16(REG7, DEFAULT);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    //MULT16_32_P15(bank->filter_left[i],ps[i])
    VoC_lw32_p(REG67, REG4, DEFAULT);

    VoC_movreg32(RL7, REG67, DEFAULT);
    VoC_shr32_ri(RL7, 15, DEFAULT);
    VoC_and32_rd(REG67, SPEEX_CONST_32767);
    VoC_movreg16(REG7, RL7_LO, DEFAULT);

    VoC_mac32_rp(REG7, REG2, DEFAULT);
    VoC_mac32inc_rp(REG6, REG2, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC1, 15, DEFAULT);

    VoC_add32_rr(ACC0, ACC0, ACC1, DEFAULT);
    VoC_add16inc_pp(REG6, REG0, REG0, IN_PARALLEL);

    // mel[id] += MULT16_32_P15(bank->filter_left[i],ps[i]);
    VoC_add16_rr(REG6, REG5, REG6, DEFAULT);
    VoC_NOP();
    VoC_add32_rp(ACC0, ACC0, REG6, DEFAULT);
    VoC_NOP();
    VoC_sw32_p(ACC0, REG6, DEFAULT);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    // MULT16_32_P15(bank->filter_right[i],ps[i])
    VoC_lw32_p(REG67, REG4, DEFAULT);

    VoC_movreg32(RL7, REG67, DEFAULT);
    VoC_shr32_ri(RL7, 15, DEFAULT);
    VoC_and32_rd(REG67, SPEEX_CONST_32767);
    VoC_movreg16(REG7, RL7_LO, DEFAULT);

    VoC_mac32_rp(REG7, REG3, DEFAULT);
    VoC_mac32inc_rp(REG6, REG3, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC1, 15, DEFAULT);

    VoC_add32_rr(ACC0, ACC0, ACC1, DEFAULT);
    VoC_add16inc_pp(REG6, REG1, REG1, IN_PARALLEL);

    // mel[id] += MULT16_32_P15(bank->filter_right[i],ps[i])
    VoC_add16_rr(REG6, REG5, REG6, DEFAULT);
    VoC_NOP();
    VoC_add32_rp(ACC0, ACC0, REG6, DEFAULT);
    VoC_NOP();
    VoC_sw32_p(ACC0, REG6, DEFAULT);

    VoC_pop16(REG7, DEFAULT);
    VoC_add16_rd(REG4, REG4, SPEEX_CONST_2);
    VoC_add16_rd(REG7, REG7, SPEEX_CONST_1);
    VoC_endloop0

    VoC_pop32(REG67, DEFAULT);
    VoC_pop32(REG45, DEFAULT);
    VoC_pop32(REG23, DEFAULT);
    VoC_pop32(REG01, DEFAULT);
    //VoC_pop16(RA, DEFAULT);
    //VoC_NOP();
    VoC_return;
}

/*****************
 * int16 in REG0
 * int 32 in REG45
 * result in ACC0
 *****************/
#if 0
void spx_mult16_32_q15(void)
{
    //VoC_push16(RA, DEFAULT);
    /*
    VoC_lw16i(REG0, 0x03ff);
    VoC_lw16i(REG4, 0x3480);
    VoC_lw16i(REG5, 0x86e7);
    */
    VoC_push32(REG45, DEFAULT);
// VoC_directive: PARSER_OFF
    if (REGS[5].reg >= 0x4000)
        VoC_NOP();
// VoC_directive: PARSER_ON
    /*
    VoC_movreg32(RL7, REG45, IN_PARALLEL);

    VoC_shr32_ri(RL7, 15, DEFAULT);

    VoC_and32_rd(REG45, SPEEX_CONST_32767);

    VoC_movreg16(REG5, RL7_LO, DEFAULT);
    */
    VoC_shr32_ri(REG45, -1, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);

    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_lw16i_short(FORMATX, 15, IN_PARALLEL);

    VoC_multf32_rr(ACC0, REG5, REG0, DEFAULT);
    VoC_macX_rr(REG4, REG0, DEFAULT);

    VoC_pop32(REG45, DEFAULT);

    //VoC_pop16(RA, DEFAULT);
    //VoC_NOP();
    VoC_return;
}
#endif

/*****************
 * int16 in REG0
 * int 32 in REG45
 * result in ACC0
 *****************/
void spx_mult16_32_p15(void)
{
    //VoC_push16(RA, DEFAULT);
    VoC_push16(REG0, DEFAULT);

    VoC_push32(REG45, DEFAULT);
    /*
    VoC_movreg32(RL7, REG45, IN_PARALLEL);

    VoC_shr32_ri(RL7, 15, DEFAULT);
    VoC_and32_rd(REG45, SPEEX_CONST_32767);

    VoC_movreg16(REG5, RL7_LO, DEFAULT);
    */
    VoC_shr32_ri(REG45, -1, DEFAULT);

    //VoC_lw32z(ACC0, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);

    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_lw16i_short(FORMATX, 0, IN_PARALLEL); // outer use this

    //VoC_mac32_rr(REG5, REG0, DEFAULT);
    VoC_multf32_rr(ACC0, REG5, REG0, DEFAULT);
    VoC_mac32_rr(REG4, REG0, IN_PARALLEL);


    VoC_pop32(REG45, DEFAULT);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC1, 15, DEFAULT);


    VoC_add32_rr(ACC0, ACC0, ACC1, DEFAULT);
    VoC_pop16(REG0, IN_PARALLEL);
    //VoC_pop16(RA, DEFAULT);
    //VoC_NOP();
    VoC_return;
}


void update_noise_prob(void)
{
    VoC_push16(RA, DEFAULT);

    VoC_lw16_d(REG7, SpxPpState_ps_size); // N in REG0
    VoC_lw16i_set_inc(REG1, SpxPpState_S, 2); // r1 point to st->S
    VoC_lw16i_set_inc(REG2, SpxPpState_ps, 2); // r2 point to st->ps
    VoC_lw16i_set_inc(REG3, SPEEX_UPDATE_NOISE_X, 1);

    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_lw16i_short(FORMATX, 15, IN_PARALLEL);
    // st->S[0] =  MULT16_32_Q15(26214,st->S[0]) + MULT16_32_Q15(6554,st->ps[0]);
    VoC_lw16i(REG0, 26214);
    VoC_lw32_p(REG45, REG1, DEFAULT);
    //VoC_jal(spx_mult16_32_q15);
    //VoC_movreg32(RL7, ACC0, DEFAULT);
    VoC_shr32_ri(REG45, -1, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);

    VoC_multf32_rr(ACC0, REG5, REG0, DEFAULT);
    VoC_macX_rr(REG4, REG0, DEFAULT);

    VoC_NOP();
    VoC_lw16i(REG0, 6554);
    VoC_lw32_p(REG45, REG2, DEFAULT);
    //VoC_jal(spx_mult16_32_q15);
    //VoC_add32_rr(ACC0, ACC0, RL7, DEFAULT);
    VoC_shr32_ri(REG45, -1, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);

    VoC_mac32_rr(REG5, REG0, DEFAULT);
    VoC_macX_rr(REG4, REG0, DEFAULT);

    VoC_NOP();
    VoC_NOP();
    VoC_sw32inc_p(ACC0, REG1, DEFAULT);

    /*
    for (i=1;i<N-1;i++)
      st->S[i] =  MULT16_32_Q15(26214,st->S[i]) + MULT16_32_Q15(1638,st->ps[i-1])
                      + MULT16_32_Q15(3277,st->ps[i]) + MULT16_32_Q15(1638,st->ps[i+1]);
    */
    VoC_sub16_rd(REG6, REG7, SPEEX_CONST_2);
    //VoC_NOP();
    VoC_loop_r_short(REG6, DEFAULT);
    VoC_startloop0
    VoC_lw16i(REG0, 26214);
    VoC_lw32_p(REG45, REG1, DEFAULT);
    //VoC_jal(spx_mult16_32_q15);
    //VoC_movreg32(RL7, ACC0, DEFAULT);

    VoC_shr32_ri(REG45, -1, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);

    VoC_multf32_rr(ACC0, REG5, REG0, DEFAULT);
    VoC_macX_rr(REG4, REG0, DEFAULT);

    VoC_loop_i(1, 3)
    VoC_NOP();
    VoC_lw16inc_p(REG0, REG3, DEFAULT);
    VoC_lw32inc_p(REG45, REG2, DEFAULT);
    //VoC_jal(spx_mult16_32_q15);
    //VoC_add32_rr(RL7, RL7, ACC0, DEFAULT);

    VoC_shr32_ri(REG45, -1, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);

    VoC_mac32_rr(REG5, REG0, DEFAULT);
    VoC_macX_rr(REG4, REG0, DEFAULT);
    VoC_endloop1

    VoC_sub16_rd(REG2, REG2, SPEEX_CONST_4);
    VoC_sub16_rd(REG3, REG3, SPEEX_CONST_3);
    //VoC_sw32inc_p(RL7, REG1, DEFAULT);
    VoC_sw32inc_p(ACC0, REG1, DEFAULT);
    VoC_endloop0

    VoC_add16_rd(REG2, REG2, SPEEX_CONST_2);
    // st->S[N-1] =  MULT16_32_Q15(26214,st->S[N-1]) + MULT16_32_Q15(6554,st->ps[N-1]);
    VoC_lw16i(REG0, 26214);
    VoC_lw32_p(REG45, REG1, DEFAULT);
    //VoC_jal(spx_mult16_32_q15);
    //VoC_movreg32(RL7, ACC0, DEFAULT);
    VoC_shr32_ri(REG45, -1, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);

    VoC_multf32_rr(ACC0, REG5, REG0, DEFAULT);
    VoC_macX_rr(REG4, REG0, DEFAULT);

    VoC_lw16i(REG0, 6554);
    VoC_lw32inc_p(REG45, REG2, DEFAULT);
    //VoC_jal(spx_mult16_32_q15);
    //VoC_add32_rr(ACC0, ACC0, RL7, DEFAULT);
    VoC_shr32_ri(REG45, -1, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);

    VoC_mac32_rr(REG5, REG0, DEFAULT);
    VoC_macX_rr(REG4, REG0, DEFAULT);

    VoC_NOP();
    VoC_NOP();
    VoC_sw32inc_p(ACC0, REG1, DEFAULT);

    VoC_lw16_d(REG1, SpxPpState_nb_adapt);
    VoC_bne16_rd(UPDATE_NOISE_ADAPT_NE1, REG1, SPEEX_CONST_1)
    VoC_lw32z(RL7, DEFAULT);
    VoC_lw16_d(REG7, SpxPpState_ps_size);
    VoC_lw16i_set_inc(REG0, SpxPpState_Stmp, 2);
    VoC_lw16i_set_inc(REG2, SpxPpState_Smin, 2);
    VoC_loop_r_short(REG7, DEFAULT);
    VoC_startloop0
    VoC_sw32inc_p(RL7, REG0, DEFAULT);
    VoC_sw32inc_p(RL7, REG2, DEFAULT);
    VoC_endloop0
UPDATE_NOISE_ADAPT_NE1:
    /*
    if (st->nb_adapt < 100)
        min_range = 15;
    else if (st->nb_adapt < 1000)
        min_range = 50;
    else if (st->nb_adapt < 10000)
        min_range = 150;
    else
        min_range = 300;
    */
    // min_range in reg4
    /*
        VoC_bnlt16_rd(UPDATE_NOISE_NLT_100, REG1, SPEEX_CONST_100)
            VoC_lw16i_short(REG4, 15, DEFAULT);
            VoC_jump(UPDATE_NOISE_MIN_RANGE_END);
    UPDATE_NOISE_NLT_100:
        VoC_bnlt16_rd(UPDATE_NOISE_NLT_1000, REG1, SPEEX_CONST_1000)
            VoC_lw16i(REG4, 50);
            VoC_jump(UPDATE_NOISE_MIN_RANGE_END);
    UPDATE_NOISE_NLT_1000:
        VoC_bnlt16_rd(UPDATE_NOISE_NLT_10000, REG1, SPEEX_CONST_10000)
            VoC_lw16i(REG4, 150);
            VoC_jump(UPDATE_NOISE_MIN_RANGE_END);
    UPDATE_NOISE_NLT_10000:
        VoC_lw16i(REG4, 300);
    UPDATE_NOISE_MIN_RANGE_END:
    */
    VoC_bnlt16_rd(UPDATE_NOISE_NLT_100, REG1, SPEEX_CONST_100)
    //VoC_lw16i_short(REG4, 30, DEFAULT);
    VoC_lw16_d(REG4, SPEEX_NOISE_PROB_MIN_RANGE1);
    VoC_jump(UPDATE_NOISE_MIN_RANGE_END);
UPDATE_NOISE_NLT_100:
    VoC_bnlt16_rd(UPDATE_NOISE_NLT_1000, REG1, SPEEX_CONST_1000)
    //VoC_lw16i(REG4, 30);
    VoC_lw16_d(REG4, SPEEX_NOISE_PROB_MIN_RANGE2);
    VoC_jump(UPDATE_NOISE_MIN_RANGE_END);
UPDATE_NOISE_NLT_1000:
    VoC_bnlt16_rd(UPDATE_NOISE_NLT_10000, REG1, SPEEX_CONST_10000)
    //VoC_lw16i(REG4, 30);
    VoC_lw16_d(REG4, SPEEX_NOISE_PROB_MIN_RANGE3);
    VoC_jump(UPDATE_NOISE_MIN_RANGE_END);
UPDATE_NOISE_NLT_10000:
    //VoC_lw16i(REG4, 30);
    VoC_lw16_d(REG4, SPEEX_NOISE_PROB_MIN_RANGE4);
UPDATE_NOISE_MIN_RANGE_END:
    /*
       if (st->min_count > min_range)
    {
      st->min_count = 0;
      for (i=0;i<N;i++)
      {
         st->Smin[i] = MIN32(st->Stmp[i], st->S[i]);
         st->Stmp[i] = st->S[i];
      }
    } else {
      for (i=0;i<N;i++)
      {
         st->Smin[i] = MIN32(st->Smin[i], st->S[i]);
         st->Stmp[i] = MIN32(st->Stmp[i], st->S[i]);
      }
    }
    */
    VoC_lw16_d(REG6, SpxPpState_min_count);
    VoC_lw16i_set_inc(REG0, SpxPpState_Stmp, 2);
    VoC_lw16i_set_inc(REG1, SpxPpState_S, 2);
    VoC_lw16i_set_inc(REG2, SpxPpState_Smin, 2);
    VoC_lw16_d(REG7, SpxPpState_ps_size);
    VoC_bngt16_rr(UPDATE_NOISE_COUNT_NGT_RANGE, REG6, REG4);
    VoC_lw16i_short(REG6, 0, DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG6, SpxPpState_min_count);
    VoC_loop_r_short(REG7, DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p(RL7, REG1, DEFAULT);
    VoC_lw32_p(REG45, REG0, DEFAULT);
    VoC_sw32inc_p(RL7, REG0, DEFAULT);
    VoC_bgt32_rr(UPDATE_NOISE_Stmp_GT_S, REG45, RL7)
    VoC_movreg32(RL7, REG45, DEFAULT);
    VoC_NOP();
UPDATE_NOISE_Stmp_GT_S:
    VoC_sw32inc_p(RL7, REG2, DEFAULT);
    VoC_endloop0
    VoC_jump(UPDATE_NOISE_COUNT_END);
UPDATE_NOISE_COUNT_NGT_RANGE:
    VoC_loop_r_short(REG7, DEFAULT);
    VoC_startloop0
    VoC_lw32_p(REG45, REG2, DEFAULT);
    VoC_lw32inc_p(RL7, REG1, DEFAULT);
    VoC_bgt32_rr(UPDATE_NOISE_Smin_GT_S, RL7, REG45)
    VoC_movreg32(REG45, RL7, DEFAULT);
    VoC_NOP();
UPDATE_NOISE_Smin_GT_S:
    VoC_sw32inc_p(REG45, REG2, DEFAULT);
    VoC_lw32_p(REG45, REG0, DEFAULT);
    VoC_bgt32_rr(UPDATE_NOISE_Stmp_GT_S2, RL7, REG45)
    VoC_movreg32(REG45, RL7, DEFAULT);
    VoC_NOP();
UPDATE_NOISE_Stmp_GT_S2:
    VoC_sw32inc_p(REG45, REG0, DEFAULT);
    VoC_endloop0
UPDATE_NOISE_COUNT_END:
    /*
    for (i=0;i<N;i++)
    {
        if (MULT16_32_Q15(13107,st->S[i]) > st->Smin[i])
            st->update_prob[i] = 1;
        else
            st->update_prob[i] = 0;
    }
    */
    VoC_lw16i_set_inc(REG3, SpxPpState_S, 2);
    VoC_lw16i_set_inc(REG1, SpxPpState_Smin, 2);
    VoC_lw16i_set_inc(REG2, SpxPpState_update_prob, 1);
    VoC_loop_r_short(REG7, DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p(REG45, REG3, DEFAULT);
    VoC_lw16i(REG0, 13107);
    //VoC_jal(spx_mult16_32_q15);
    VoC_shr32_ri(REG45, -1, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);

    VoC_multf32_rr(ACC0, REG5, REG0, DEFAULT);
    VoC_macX_rr(REG4, REG0, DEFAULT);

    VoC_lw32inc_p(RL7, REG1, DEFAULT);
    VoC_lw16i_short(REG6, 1, DEFAULT);
    VoC_bgt32_rr(UPDATE_NOISE_UPDATE_PROB, ACC0, RL7)
    VoC_lw16i_short(REG6, 0, DEFAULT);
    VoC_NOP();
UPDATE_NOISE_UPDATE_PROB:
    VoC_sw16inc_p(REG6, REG2, DEFAULT);
    VoC_endloop0

    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/******************
 * mel in reg4
 * ps in reg5
 * bank in st->bank
 ******************/
/*
    for (i=0;i<bank->len;i++)
   {
      spx_word32_t tmp;
      int id1, id2;
      id1 = bank->bank_left[i];
      id2 = bank->bank_right[i];
      tmp = MULT16_16(mel[id1],bank->filter_left[i]);
      tmp += MULT16_16(mel[id2],bank->filter_right[i]);
      ps[i] = EXTRACT16(PSHR32(tmp,15));
   }
*/
void filterbank_compute_psd16(void)
{
    //VoC_push16(RA, DEFAULT);
    VoC_push32(REG01, DEFAULT);
    VoC_push32(REG23, DEFAULT);
    VoC_push32(REG67, DEFAULT);

    VoC_lw16_d(REG6, SpxPpState_bank_len);
    VoC_lw16i_set_inc(REG0, SpxPpState_bank_bank_left, 1);
    VoC_lw16i_set_inc(REG1, SpxPpState_bank_filter_left, 1);
    VoC_lw16i_set_inc(REG2, SpxPpState_bank_bank_right, 1);
    VoC_lw16i_set_inc(REG3, SpxPpState_bank_filter_right, 1);

    VoC_loop_r_short(REG6, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    VoC_startloop0
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16inc_p(REG6, REG0, IN_PARALLEL);

    VoC_add16_rr(REG7, REG4, REG6, DEFAULT);
    VoC_NOP();
    VoC_lw16_p(REG7, REG7, DEFAULT);
    VoC_mac32inc_rp(REG7, REG1, DEFAULT);
    VoC_lw16inc_p(REG6, REG2, DEFAULT);

    VoC_add16_rr(REG7, REG4, REG6, DEFAULT);
    VoC_NOP();
    VoC_lw16_p(REG7, REG7, DEFAULT);
    VoC_mac32inc_rp(REG7, REG3, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(REG67, ACC0, SPEEX_CONST_16384);

    VoC_shr32_ri(REG67, 15, DEFAULT);
    // sw REG6 before REG5 update
    VoC_add16_rd(REG5, REG5, SPEEX_CONST_1);
    VoC_sw16_p(REG6, REG5, DEFAULT);
    VoC_endloop0

    VoC_pop32(REG67, DEFAULT);
    VoC_pop32(REG23, DEFAULT);
    VoC_pop32(REG01, DEFAULT);
    //VoC_pop16(RA, DEFAULT);
    //VoC_NOP();
    VoC_return;
}

/***************************
 * a in RL6 0x1
 * b in RL7 0x3
 * a/b in reg01 0x2aaa
 ***************************/
void spx_DIV32_16_Q15(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push32(REG67, IN_PARALLEL);

    VoC_push32(RL6, DEFAULT);
    VoC_movreg32(REG67, RL6, IN_PARALLEL);

    VoC_push32(RL7, DEFAULT);
    VoC_shr32_ri(REG67, 15, IN_PARALLEL);
    //just for test
    /*
    VoC_lw32z(RL6, DEFAULT);
    VoC_lw32z(RL7, IN_PARALLEL);
    VoC_lw16i_short(RL6_LO, 1, DEFAULT);
    VoC_lw16i_short(RL7_LO, 3, DEFAULT);
    */

    VoC_bgt32_rr(DIV32_16_Q15_LT_B, RL7, REG67)
    VoC_lw16i(REG0, 0x7fff);
    VoC_jump(DIV32_16_Q15_END);
DIV32_16_Q15_LT_B:
    VoC_blt32_rd(DIV32_16_Q15_IF1, RL7, SPEEX_CONST_0x800000)
    VoC_shr32_ri(RL6, 8, DEFAULT);
    VoC_shr32_ri(RL7, 8, IN_PARALLEL);
DIV32_16_Q15_IF1:
    VoC_blt32_rd(DIV32_16_Q15_IF2, RL7, SPEEX_CONST_0x80000)
    VoC_shr32_ri(RL6, 4, DEFAULT);
    VoC_shr32_ri(RL7, 4, IN_PARALLEL);
DIV32_16_Q15_IF2:
    VoC_blt32_rd(DIV32_16_Q15_IF3, RL7, SPEEX_CONST_32768)
    VoC_shr32_ri(RL6, 4, DEFAULT);
    VoC_shr32_ri(RL7, 4, IN_PARALLEL);
DIV32_16_Q15_IF3:
    VoC_movreg32(REG67, RL6, DEFAULT);
    VoC_shr32_ri(REG67, -15, DEFAULT);

    VoC_sub32_rr(RL6, REG67, RL6, DEFAULT);
    VoC_push32(REG45, IN_PARALLEL);
    VoC_jal(speex_div32_16);
    VoC_pop32(REG45, DEFAULT);

DIV32_16_Q15_END:
    VoC_pop32(RL7, DEFAULT);
    VoC_pop32(RL6, DEFAULT);
    VoC_pop32(REG67, DEFAULT);
    VoC_pop16(RA, IN_PARALLEL);
    VoC_NOP();
    VoC_return;
}

/***************************
 * a in RL6 0x1
 * b in RL7 0x2
 * a/b in reg01 0x80
 ***************************/
void spx_DIV32_16_Q8(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push32(REG67, IN_PARALLEL);

    VoC_push32(RL6, DEFAULT);
    VoC_movreg32(REG67, RL6, IN_PARALLEL);

    VoC_push32(RL7, DEFAULT);
    VoC_shr32_ri(REG67, 7, IN_PARALLEL);

    //just for test
    /*
    VoC_lw32z(RL6, DEFAULT);
    VoC_lw32z(RL7, IN_PARALLEL);
    VoC_NOP();
    VoC_lw16i(RL6_LO, 0x80e8);
    VoC_lw16i(RL7_LO, 0x1a1);
    VoC_NOP();
    */

    VoC_bgt32_rr(DIV32_16_Q8_LT_B, RL7, REG67)
    VoC_lw16i(REG0, 0x7fff);
    VoC_jump(DIV32_16_Q8_END);
DIV32_16_Q8_LT_B:
    VoC_blt32_rd(DIV32_16_Q8_IF1, RL7, SPEEX_CONST_0x800000)
    VoC_shr32_ri(RL6, 8, DEFAULT);
    VoC_shr32_ri(RL7, 8, IN_PARALLEL);
DIV32_16_Q8_IF1:
    VoC_blt32_rd(DIV32_16_Q8_IF2, RL7, SPEEX_CONST_0x80000)
    VoC_shr32_ri(RL6, 4, DEFAULT);
    VoC_shr32_ri(RL7, 4, IN_PARALLEL);
DIV32_16_Q8_IF2:
    VoC_blt32_rd(DIV32_16_Q8_IF3, RL7, SPEEX_CONST_32768)
    VoC_shr32_ri(RL6, 4, DEFAULT);
    VoC_shr32_ri(RL7, 4, IN_PARALLEL);
DIV32_16_Q8_IF3:
    VoC_shr32_ri(RL6, -8, DEFAULT);
    VoC_movreg32(REG67, RL7, IN_PARALLEL);

    VoC_shr32_ri(REG67, 1, DEFAULT);
    VoC_push32(REG45, IN_PARALLEL);

    VoC_add32_rr(RL6, RL6, REG67, DEFAULT);

    VoC_jal(speex_div32_16);
    VoC_pop32(REG45, DEFAULT);

DIV32_16_Q8_END:
    VoC_pop32(RL7, DEFAULT);
    VoC_pop32(RL6, DEFAULT);
    VoC_pop32(REG67, DEFAULT);
    VoC_pop16(RA, IN_PARALLEL);
    VoC_NOP();
    VoC_return;
}

/************************
 * input :  int noise_suppress: st->noise_suppress
            int effective_echo_suppress: effective_echo_suppress in reg4
            spx_word32_t *noise: st->noise+N
            spx_word32_t *echo: st->echo_noise+N
            spx_word16_t *gain_floor: st->gain_floor+N
            int len: M
 ***********************/
void compute_gain_floor(void)
{
    VoC_push16(RA, DEFAULT);

    VoC_lw16_d(REG5, SpxPpState_noise_suppress);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    //VoC_bngt16_rr(COMPUTE_GAIN_FLOOR_IF1, REG5, REG4)
    VoC_mac32_rd(REG5, SPEEX_CONST_512);
    VoC_NOP();
    VoC_movreg32(REG01, ACC0, DEFAULT);


    VoC_jal(spx_exp); // result in RL7
    VoC_shr32_ri(RL7, 1, DEFAULT);

    VoC_blt32_rd(COMPUTE_GAIN_FLOOR_Q15ONE1, RL7, SPEEX_CONST_32767)
    VoC_lw32_d(RL7, SPEEX_CONST_32767);
    VoC_NOP();
COMPUTE_GAIN_FLOOR_Q15ONE1:
    VoC_sw32_d(RL7, COMPUTE_GAIN_FLOOR_NOISE_ECHO_GAIN); // now noise_gain stored

    VoC_sub16_rr(REG4, REG4, REG5, DEFAULT);
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    VoC_mac32_rd(REG4, SPEEX_CONST_472);
    VoC_NOP();
    VoC_movreg32(REG01, ACC0, DEFAULT);

    VoC_jal(spx_exp);
    VoC_shr32_ri(RL7, 1, DEFAULT);

    VoC_blt32_rd(COMPUTE_GAIN_FLOOR_Q15ONE2, RL7, SPEEX_CONST_32767)
    VoC_lw32_d(RL7, SPEEX_CONST_32767);

COMPUTE_GAIN_FLOOR_Q15ONE2:
    VoC_movreg32(REG45, RL7, DEFAULT); // now gain_ratio in reg4

    VoC_lw16_d(REG3, SpxPpState_nbands);
    VoC_lw16i_set_inc(REG0, SpxPpState_noise_M, 2);
    VoC_lw16i_set_inc(REG1, SpxPpState_echo_noise_M, 2);
    VoC_lw16i_set_inc(REG2, SpxPpState_gain_floor_M, 1);

    //VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    //VoC_lw16i_short(FORMATX, 15, IN_PARALLEL);

    VoC_loop_r_short(REG3, DEFAULT);
    VoC_startloop0
    VoC_push16(REG4, DEFAULT);
    VoC_lw32inc_p(RL6, REG0, DEFAULT);
    VoC_lw32_d(RL7, SPEEX_CONST_1);

    VoC_shr32_ri(RL7, NEG_NOISE_SHIFT_1, DEFAULT);

    VoC_add32_rr(RL6, RL6, RL7, DEFAULT);

    VoC_shr32_ri(RL6, NOISE_SHIFT, DEFAULT); // tmp in rl6

    VoC_push32(REG01, DEFAULT);

    VoC_movreg16(REG0, REG4, DEFAULT);
    VoC_lw32inc_p(REG45, REG1, DEFAULT);

    //VoC_jal(spx_mult16_32_q15); //result in ACC0

    VoC_NOP();
    VoC_push32(REG45, DEFAULT);

    VoC_shr32_ri(REG45, -1, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);

    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_lw16i_short(FORMATX, 15, IN_PARALLEL);

    VoC_multf32_rr(ACC0, REG5, REG0, DEFAULT);
    VoC_macX_rr(REG4, REG0, DEFAULT);

    VoC_pop32(REG45, DEFAULT);
    VoC_add32_rr(RL7, RL6, ACC0, DEFAULT);

    VoC_NOP();
    VoC_push32(RL7, DEFAULT);

    VoC_add32_rr(RL7, RL6, REG45, DEFAULT);

    VoC_add32_rd(RL7, RL7, SPEEX_CONST_1);

    VoC_pop32(RL6, DEFAULT); // this way RL6 = pushed(RL7)

    VoC_jal(spx_DIV32_16_Q15); // result in reg01

    VoC_shr32_ri(REG01, -15, DEFAULT);
    VoC_jal(spx_sqrt); // result in reg0

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 15, IN_PARALLEL);
    VoC_mac32_rd(REG0, COMPUTE_GAIN_FLOOR_NOISE_ECHO_GAIN);
    VoC_NOP();
    VoC_movreg32(REG01, ACC0, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG0, REG2, DEFAULT);

    VoC_pop32(REG01, DEFAULT);
    VoC_pop16(REG4, DEFAULT);
    VoC_add16_rd(REG1, REG1, SPEEX_CONST_2);

    VoC_endloop0

    VoC_jump(COMPUTE_GAIN_FLOOR_END);
COMPUTE_GAIN_FLOOR_IF1:
    VoC_mac32_rd(REG4, SPEEX_CONST_1000);
    VoC_NOP();
    VoC_movreg32(REG01, ACC0, DEFAULT);


    VoC_jal(spx_exp); // result in RL7
    VoC_shr32_ri(RL7, 1, DEFAULT);

    VoC_blt32_rd(COMPUTE_GAIN_FLOOR_Q15ONE3, RL7, SPEEX_CONST_32767)
    VoC_lw32_d(RL7, SPEEX_CONST_32767);
    VoC_NOP();
COMPUTE_GAIN_FLOOR_Q15ONE3:
    VoC_sw32_d(RL7, COMPUTE_GAIN_FLOOR_NOISE_ECHO_GAIN); // now echo_gain in COMPUTE_GAIN_FLOOR_NOISE_ECHO_GAIN

    VoC_sub16_rr(REG4, REG5, REG4, DEFAULT);
    VoC_lw32z(ACC0, IN_PARALLEL);
    VoC_mac32_rd(REG4, SPEEX_CONST_472);
    VoC_NOP();
    VoC_movreg32(REG01, ACC0, DEFAULT);


    VoC_jal(spx_exp);
    VoC_shr32_ri(RL7, 1, DEFAULT);

    VoC_blt32_rd(COMPUTE_GAIN_FLOOR_Q15ONE4, RL7, SPEEX_CONST_32767)
    VoC_lw32_d(RL7, SPEEX_CONST_32767);

COMPUTE_GAIN_FLOOR_Q15ONE4:
    VoC_movreg32(REG45, RL7, DEFAULT); // now gain_ratio in reg4

    VoC_lw16_d(REG3, SpxPpState_nbands);
    VoC_lw16i_set_inc(REG0, SpxPpState_noise_M, 2);
    VoC_lw16i_set_inc(REG1, SpxPpState_echo_noise_M, 2);
    VoC_lw16i_set_inc(REG2, SpxPpState_gain_floor_M, 1);

    //VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    //VoC_lw16i_short(FORMATX, 15, IN_PARALLEL);

    VoC_loop_r_short(REG3, DEFAULT);
    VoC_startloop0
    VoC_push16(REG4, DEFAULT);
    VoC_lw32inc_p(RL6, REG0, DEFAULT);
    VoC_lw32_d(RL7, SPEEX_CONST_1);

    VoC_shr32_ri(RL7, NEG_NOISE_SHIFT_1, DEFAULT);

    VoC_add32_rr(RL6, RL6, RL7, DEFAULT);

    VoC_shr32_ri(RL6, NOISE_SHIFT, DEFAULT); // tmp in rl6

    //VoC_movreg32(REG67, RL6, DEFAULT);

    VoC_push32(REG01, DEFAULT);

    VoC_movreg16(REG0, REG4, DEFAULT);
    VoC_movreg32(REG45, RL6, DEFAULT);
    //VoC_jal(spx_mult16_32_q15);

    VoC_shr32_ri(REG45, -1, DEFAULT);
    VoC_shru16_ri(REG4, 1, DEFAULT);

    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_lw16i_short(FORMATX, 15, IN_PARALLEL);

    VoC_multf32_rr(ACC0, REG5, REG0, DEFAULT);
    VoC_macX_rr(REG4, REG0, DEFAULT);

    VoC_lw32_p(REG45, REG1, DEFAULT);

    VoC_add32_rr(RL7, REG45, ACC0, DEFAULT);
    VoC_NOP();
    VoC_push32(RL7, DEFAULT);

    VoC_add32_rr(RL7, RL6, REG45, DEFAULT);

    VoC_add32_rd(RL7, RL7, SPEEX_CONST_1);
    VoC_pop32(RL6, DEFAULT);

    VoC_jal(spx_DIV32_16_Q15); // result in reg01

    VoC_shr32_ri(REG01, -15, DEFAULT);
    VoC_jal(spx_sqrt);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 15, IN_PARALLEL);
    VoC_mac32_rd(REG0, COMPUTE_GAIN_FLOOR_NOISE_ECHO_GAIN);
    VoC_NOP();
    VoC_movreg32(REG01, ACC0, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG0, REG2, DEFAULT);

    VoC_pop32(REG01, DEFAULT);
    VoC_pop16(REG4, DEFAULT);
    VoC_add16_rd(REG1, REG1, SPEEX_CONST_2);

    VoC_endloop0
COMPUTE_GAIN_FLOOR_END:

    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/****************
 * xx in rl6
 * result in r01
 ****************/
void hypergeom_gain(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push32(REG23, DEFAULT);
    VoC_push32(REG45, DEFAULT);
    VoC_push32(REG67, DEFAULT);
    VoC_push32(RL6, DEFAULT);
    VoC_push32(RL7, DEFAULT);

    VoC_movreg32(REG67, RL6, DEFAULT);
    VoC_movreg32(REG45, RL6, IN_PARALLEL);
    VoC_shr32_ri(REG67, 10, DEFAULT); // now ind in reg6

    //VoC_push32(REG67, DEFAULT);
    VoC_bnltz16_r(HYPERGEOM_GAIN_IND_NLT_0, REG6)
    VoC_lw16i(REG0, 0x7fff);
    VoC_lw16i_short(REG1, 0, DEFAULT);
    VoC_jump(HYPERGEOM_GAIN_END);
HYPERGEOM_GAIN_IND_NLT_0:
    VoC_bngt16_rd(HYPERGEOM_GAIN_IND_NGT_19, REG6, SPEEX_CONST_19)
    // ADD32(32767,EXTEND32(DIV32_16(1087164, SHR32(xx,EXPIN_SHIFT-SNR_SHIFT))));
    VoC_shr32_ri(REG45, EXPIN_SNR_SHIFT, DEFAULT);
    VoC_push32(RL6, IN_PARALLEL);
    VoC_lw32_d(RL6, SPEEX_CONST_1087164);
    VoC_movreg32(RL7, REG45, DEFAULT);
    VoC_jal(speex_div32_16);
    VoC_pop32(RL6, DEFAULT);
    VoC_add32_rd(REG01, REG01, SPEEX_CONST_32767);
    VoC_jump(HYPERGEOM_GAIN_END);
HYPERGEOM_GAIN_IND_NGT_19:
    // frac = SHL32(xx-SHL32(ind,10),5);
    VoC_push32(REG67, DEFAULT);

    VoC_shr32_ri(REG67, -10, DEFAULT);
    VoC_sub32_rr(REG23, RL6, REG67, DEFAULT);
    VoC_shr32_ri(REG23, -5, DEFAULT);
    VoC_pop32(REG67, IN_PARALLEL);

    VoC_lw16i_set_inc(REG0, SPEEX_HYPERGEOM_GAIN_TABLE, 1);
    VoC_add16_rr(REG0, REG0, REG6, DEFAULT);
    VoC_sub16_dr(REG3, SPEEX_CONST_32767, REG2);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    VoC_mac32inc_rp(REG3, REG0, DEFAULT);
    VoC_mac32inc_rp(REG2, REG0, DEFAULT);
    VoC_NOP(); // now tmp+tmp2 in ACC0

    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_64);
    VoC_shr32_ri(ACC0, 7, DEFAULT); // tmp4 in ACC0
    VoC_shr32_ri(RL6, -15, DEFAULT);

    VoC_add32_rd(REG01, RL6, SPEEX_CONST_6711);
    VoC_push32(ACC0, DEFAULT);
    VoC_jal(spx_sqrt); // tmp3 in reg0
    VoC_pop32(ACC0, DEFAULT);

    VoC_movreg16(RL7_LO, REG0, DEFAULT);
    VoC_lw16i_short(RL7_HI, 0, IN_PARALLEL);
    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_jal(speex_div32_16); //tmp5 in reg01

    VoC_shr32_ri(REG01, -7, DEFAULT); // result in reg01
HYPERGEOM_GAIN_END:
    VoC_pop32(RL7, DEFAULT);
    VoC_pop32(RL6, DEFAULT);
    VoC_pop32(REG67, DEFAULT);
    VoC_pop32(REG45, DEFAULT);
    VoC_pop32(REG23, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/*************
 * x in REG0
 * result in REG0
 *************/
void spx_qcurve(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push32(RL6, DEFAULT);
    VoC_push32(RL7, DEFAULT);
    VoC_push32(REG45, DEFAULT);

    // x = MAX16(x, 1)
    VoC_bgt16_rd(SPX_QCURVE_X_GT_1, REG0, SPEEX_CONST_1)
    VoC_lw16i_short(REG0, 1, DEFAULT);
SPX_QCURVE_X_GT_1:
    VoC_lw32_d(RL6, SPEEX_CONST_32767);

    VoC_movreg16(RL7_LO, REG0, DEFAULT);
    VoC_bnltz16_r(SPX_QCURVE_EXTEND_X, REG0)
    VoC_lw16i_short(RL7_HI, 0, DEFAULT);
SPX_QCURVE_EXTEND_X:
    VoC_jal(speex_div32_16); // tmp2 in reg0

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 15, IN_PARALLEL);
    VoC_mac32_rd(REG0, SPEEX_CONST_19661);
    VoC_NOP(); // tmp3 in ACC0
    VoC_add32_rd(RL7, ACC0, SPEEX_CONST_512);

    VoC_lw16i(RL6_LO, 0xfe00);
    VoC_lw16i(RL6_HI, 0x00ff);
    //VoC_movreg32(RL7, ACC0, DEFAULT);
    VoC_jal(speex_div32_16); // result in reg0

    VoC_pop32(REG45, DEFAULT);
    VoC_pop32(RL7, DEFAULT);
    VoC_pop32(RL6, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/*  facbuf is populated by p1,m1,p2,m2, ...
    where
    p[i] * m[i] = m[i-1]
    m0 = n                  */
/******************
 * n in RL6
 * facbuf in REG2
 ******************/
// now works as intended
//VoC_directive:PARSER_OFF
#if 0
void spx_kf_factor(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push16(REG2, DEFAULT);
    VoC_push32(REG45, DEFAULT);

    // just for test
    //VoC_lw16i_set_inc(REG2, SPEEX_REF_BUF, 2);
    //VoC_lw16i(RL6_LO, 0xa0);
    //VoC_lw16i_short(RL6_HI, 0, DEFAULT);

    VoC_lw16i_short(RL7_LO, 4, DEFAULT); // now p in RL7_LO
    VoC_lw16i_short(RL7_HI, 0, IN_PARALLEL);
KF_FACTOR_WHILE2_BEGINS:
KF_FACTOR_WHILE1_BEGINS:
    VoC_jal(speex_div32_16);
    VoC_bez32_r(KF_FACTOR_N_DIV_P, REG45)
    VoC_bne32_rd(KF_FACTOR_P_E_4, RL7, SPEEX_CONST_4)
    VoC_lw16i_short(RL7_LO, 2, DEFAULT);
    VoC_lw16i_short(RL7_HI, 0, IN_PARALLEL);
    VoC_jump(KF_FACTOR_SWITCH_END);
KF_FACTOR_P_E_4:
    VoC_bne32_rd(KF_FACTOR_P_E_2, RL7, SPEEX_CONST_2)
    VoC_lw16i_short(RL7_LO, 3, DEFAULT);
    VoC_lw16i_short(RL7_HI, 0, IN_PARALLEL);
    VoC_jump(KF_FACTOR_SWITCH_END);
KF_FACTOR_P_E_2:
    VoC_add32_rd(RL7, RL7, SPEEX_CONST_2);
    VoC_NOP();
KF_FACTOR_SWITCH_END:
    VoC_bgt32_rd(KF_FACTOR_P_NO_MORE, RL7, SPEEX_CONST_32000)
    VoC_movreg16(REG5, RL7_LO, DEFAULT);

    VoC_mult16_rr(REG4, REG5, REG5, DEFAULT);

    VoC_lw16i_short(REG5, 0, DEFAULT);

    VoC_bgt32_rr(KF_FACTOR_P_NO_MORE, REG45, RL6)
    VoC_jump(KF_FACTOR_WHILE1_BEGINS);
KF_FACTOR_P_NO_MORE:
    VoC_movreg32(RL7, RL6, DEFAULT);
    VoC_jump(KF_FACTOR_WHILE1_BEGINS);
KF_FACTOR_N_DIV_P:
    VoC_jal(speex_div32_16);
    VoC_movreg32(RL6, REG01, DEFAULT);
    VoC_lw16i_short(REG4, 1, IN_PARALLEL);
    VoC_sw16inc_p(RL7_LO, REG2, DEFAULT);
    VoC_lw16i_short(REG5, 0, IN_PARALLEL);
    VoC_sw16inc_p(RL6_LO, REG2, DEFAULT);
    VoC_bgt32_rr(KF_FACTOR_WHILE2_BEGINS, RL6, REG45);

    VoC_pop32(REG45, DEFAULT);
    VoC_pop16(REG2, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}
#endif
//VoC_directive:PARSER_ON

/**************
 * INPUT in REG0
 * output in RL7
 **************/
void spx_exp(void)
{
//  VoC_push16(RA, DEFAULT);
    VoC_push32(REG45, DEFAULT);
    VoC_push32(REG67, DEFAULT);
    VoC_push32(RL6, DEFAULT);

    //VoC_lw16i_short(REG0, 0, DEFAULT);


    VoC_bngt16_rd(SPX_EXP_X_NGT_21290, REG0, SPEEX_CONST_21290)
    VoC_lw32_d(RL7, SPEEX_CONST_0x7fffffff);
    VoC_jump(SPX_EXP_END);
SPX_EXP_X_NGT_21290:
    VoC_bgt16_rd(SPX_EXP_X_NLT_NEG21290, REG0, SPEEX_CONST_NEG21290)
    VoC_lw32z(RL7, DEFAULT);
    VoC_jump(SPX_EXP_END);
SPX_EXP_X_NLT_NEG21290:
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    VoC_mac32_rd(REG0, SPEEX_CONST_23637);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_8192);

    VoC_shr32_ri(ACC0, 14, DEFAULT);

    VoC_movreg32(REG67, ACC0, DEFAULT);

    VoC_shr32_ri(REG67, 11, DEFAULT); // integer in REG67

    //VoC_push32(REG67, DEFAULT);
    //VoC_movreg32(RL6, REG67, DEFAULT);
    VoC_bngt32_rd(SPX_EXP_X_NGT_14, REG67, SPEEX_CONST_14)
    VoC_lw32_d(RL7, SPEEX_CONST_0x7fffffff);
    VoC_jump(SPX_EXP_END);
SPX_EXP_X_NGT_14:
    VoC_bnlt32_rd(SPX_EXP_X_NLT_NEG15, REG67, SPEEX_CONST_NEG15)
    VoC_lw32z(RL7, DEFAULT);
    VoC_jump(SPX_EXP_END);
SPX_EXP_X_NLT_NEG15:
    VoC_push32(REG67, DEFAULT);

    VoC_shr32_ri(REG67, -11, DEFAULT);

    VoC_sub32_rr(RL7, ACC0, REG67, DEFAULT);

    VoC_shr32_ri(RL7, -3, DEFAULT); // frac now in RL7


    VoC_movreg32(REG67, RL7, DEFAULT);
    VoC_lw16i_short(FORMAT32, 14, IN_PARALLEL);
    VoC_lw16i_set_inc(REG0, SPEEX_EXP_D, 1);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_mac32inc_rp(REG6, REG0, DEFAULT);
    VoC_NOP();
    VoC_movreg32(REG45, ACC0, DEFAULT);

    VoC_add16inc_rp(REG4, REG4, REG0, DEFAULT);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_mac32_rr(REG6, REG4, DEFAULT);
    VoC_NOP();
    VoC_movreg32(REG45, ACC0, DEFAULT);

    VoC_add16inc_rp(REG4, REG4, REG0, DEFAULT);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_mac32_rr(REG6, REG4, DEFAULT);
    VoC_NOP();
    VoC_movreg32(REG45, ACC0, DEFAULT);

    VoC_add16inc_rp(REG4, REG4, REG0, DEFAULT); // now frac in reg4

    VoC_bnltz16_r(SPX_EXP_EXTEND_FRAC, REG4)
    VoC_lw16i(REG5, 0xffff);
SPX_EXP_EXTEND_FRAC:

    VoC_pop32(REG67, DEFAULT);
    //VoC_movreg32(REG67, RL6, DEFAULT);


    //VoC_sub16_dr(REG6, SPEEX_CONST_NEG2, REG6);
    // VSHR32(EXTEND32(frac), -integer-2);
    VoC_add16_rd(REG6, REG6, SPEEX_CONST_2);

    VoC_sub16_dr(REG6, SPEEX_CONST_0, REG6);

    VoC_shr32_rr(REG45, REG6, DEFAULT);

    VoC_movreg32(RL7, REG45, DEFAULT);
SPX_EXP_END:
    VoC_pop32(RL6, DEFAULT);
    VoC_pop32(REG67, DEFAULT);
    VoC_pop32(REG45, DEFAULT);
//  VoC_pop16(RA, DEFAULT);
//  VoC_NOP();
    VoC_return;
}

/*********************************
 * Function: spx_kiss_fftri2
 *
 * INPUT : kiss_fftr_cfg st
 *         const kiss_fft_scalar *freqdata: POINT to SpxPpState_ft
 *         kiss_fft_scalar *timedata: SpxPpState_frame
 * Output: no
 *********************************/
void spx_kiss_fftri2(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push16(REG0, DEFAULT);

    //VoC_lw16i_set_inc(REG2, SpxPpState_ft, 1);
    VoC_lw16d_set_inc(REG2, SpxPpState_ifft_in_ADDR, 1);
    VoC_lw16i_set_inc(REG0, SpxPpState_fft_lookup_backward_tmpbuf, 2);

    VoC_lw16_d(REG4, SpxPpState_fft_lookup_backward_substate_nfft);

    //VoC_push16(REG4, DEFAULT); // ncfft in REG4

    VoC_add16_rr(REG5, REG4, REG4, DEFAULT);

    VoC_add16_rr(REG1, REG0, REG5, DEFAULT);
    VoC_add16_rr(REG3, REG2, REG5, IN_PARALLEL);

    VoC_lw16i_short(INC3, -1, DEFAULT);
    VoC_lw16i_short(INC1, -2, IN_PARALLEL);
    VoC_NOP();
    VoC_inc_p(REG3, DEFAULT); // r3 POINT to freqdata[2*ncfft-1] r2 POINT to freqdata[0]
    VoC_inc_p(REG1, IN_PARALLEL); // r1 point to st->tmpbuf[ncfft-1], r0 POINT to st->tmpbuf[0]

    // st->tmpbuf[0].r = freqdata[0] + freqdata[2*ncfft-1];
    // st->tmpbuf[0].i = freqdata[0] - freqdata[2*ncfft-1];
    VoC_lw16inc_p(REG5, REG2, DEFAULT);

    VoC_add16_rp(REG6, REG5, REG3, DEFAULT);
    VoC_sub16_rp(REG7, REG5, REG3, IN_PARALLEL);
    VoC_inc_p(REG3, DEFAULT);
    VoC_sw32inc_p(REG67, REG0, DEFAULT);

    VoC_lw16i(REG5, SpxPpState_fft_lookup_backward_super_twiddles);

    VoC_add16_rd(REG5, REG5, SPEEX_CONST_2); // REG5 POINT to st->super_twiddles[1]
    VoC_shr16_ri(REG4, 1, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    VoC_loop_r_short(REG4, DEFAULT);
    VoC_startloop0
    //printf("REG0: 0x%x\tREG1: 0x%x\tREG2: 0x%x\tREG3: 0x%x\n", REGS[0].reg, REGS[1].reg, REGS[2].reg, REGS[3].reg);
    VoC_push32(REG01, DEFAULT);
    VoC_push16(REG5, DEFAULT);
    VoC_lw16inc_p(REG4, REG2, DEFAULT); // REG4 = fk.r
    VoC_lw16inc_p(REG5, REG2, DEFAULT); // REG5 = fk.i
    VoC_lw16inc_p(REG1, REG3, DEFAULT); // REG2 = -fnkc.i
    VoC_lw16inc_p(REG0, REG3, DEFAULT); // REG0 = fnkc.r

    // C_ADD (fek, fk, fnkc);
    // C_SUB (tmp, fk, fnkc);
    // C_MUL (fok, tmp, st->super_twiddles[k]);
    VoC_add16_rr(REG6, REG4, REG0, DEFAULT);
    VoC_sub16_rr(REG7, REG5, REG1, IN_PARALLEL);
    VoC_movreg32(RL7, REG67, DEFAULT); // fek stored in RL7

    VoC_sub16_rr(REG6, REG4, REG0, DEFAULT);
    VoC_add16_rr(REG7, REG5, REG1, IN_PARALLEL); // tmp in REG67

    VoC_pop16(REG5, DEFAULT);
    VoC_NOP();
    VoC_lw32_p(REG01, REG5, DEFAULT); //st->super_twiddles[k] in REG01

    VoC_push16(REG5, DEFAULT);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG7, REG1, DEFAULT);
    VoC_mac32_rr(REG7, REG0, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);
    VoC_mac32_rr(REG6, REG0, DEFAULT);
    VoC_mac32_rr(REG6, REG1, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);

    VoC_movreg32(REG45, ACC0, DEFAULT);
    VoC_movreg32(REG67, ACC1, IN_PARALLEL);
    VoC_movreg16(REG5, REG6, DEFAULT); // fok now in reg45

    VoC_pop32(REG01, DEFAULT);
    VoC_movreg32(RL6, REG23, DEFAULT);
    VoC_movreg32(REG67, RL7, IN_PARALLEL); // fek now in reg67

    // C_ADD (st->tmpbuf[k],     fek, fok);
    VoC_add16_rr(REG2, REG6, REG4, DEFAULT);
    VoC_add16_rr(REG3, REG7, REG5, IN_PARALLEL);
    VoC_NOP();
    VoC_sw32inc_p(REG23, REG0, DEFAULT);
    VoC_NOP();
    // C_SUB (st->tmpbuf[ncfft - k], fek, fok);
    // st->tmpbuf[ncfft - k].i *= -1;
    VoC_sub16_rr(REG2, REG6, REG4, DEFAULT);
    VoC_sub16_rr(REG3, REG5, REG7, IN_PARALLEL);
    VoC_NOP();
    VoC_sw32inc_p(REG23, REG1, DEFAULT);

    VoC_movreg32(REG23, RL6, DEFAULT);
    VoC_pop16(REG5, DEFAULT);
    VoC_add16_rd(REG5, REG5, SPEEX_CONST_2);
    VoC_endloop0

    //VoC_lw16i_set_inc(REG2, SpxPpState_frame, 2);
    VoC_lw16d_set_inc(REG2, SpxPpState_ifft_out_ADDR, 2);
    VoC_lw16i_set_inc(REG3, SpxPpState_fft_lookup_backward_tmpbuf, 2);
    VoC_lw16i_short(REG7, 2, DEFAULT);
    VoC_lw16i_set_inc(REG0, SpxPpState_fft_lookup_backward_substate, 1);
    VoC_lw16i_set_inc(REG1, SpxPpState_fft_lookup_backward_substate_factors, 2);
    VoC_lw16i_short(REG4, 1, DEFAULT);
    VoC_jal(spx_kf_shuffle);

    VoC_lw16i_short(REG6, 1, DEFAULT);
    VoC_lw16i_short(RL7_LO, 1, DEFAULT);
    VoC_lw16i_short(RL7_HI, 1, DEFAULT);
    VoC_jal(spx_kf_work);

    VoC_pop16(REG0, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/****************************
 * inputs : REG2: kiss_fft_cpx * Fout = (kiss_fft_cpx *) st->tmpbuf,
            REG3: const kiss_fft_cpx * f = timedata,
            REG7: const size_t fstride,
            int in_stride = 1,
            REG0: const kiss_fft_cfg st = st->substate;
            REG1: int * factors = st->substate->factors,
 * output : none
 ****************************/
void spx_kf_shuffle(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push32(REG01, DEFAULT);
    VoC_push32(REG23, DEFAULT);
    VoC_push32(REG45, DEFAULT);
    VoC_push32(REG67, DEFAULT);

    VoC_mult16_rr(REG7, REG7, REG4, DEFAULT);
    //VoC_shr16_ri(REG7, -1, DEFAULT);
    VoC_lw32inc_p(REG45, REG1, DEFAULT); // p in reg4, m in reg5
    VoC_shr16_ri(REG5, -1, DEFAULT);
    VoC_bne16_rd(SPX_KF_SHUFFLE_IF, REG5, SPEEX_CONST_2)
    VoC_lw16i_short(REG6, 0, DEFAULT);
SPX_KF_SHUFFLE_FOR1_BEGIN:
    VoC_be16_rr(SPX_KF_SHUFFLE_FOR1_END, REG6, REG4)
    VoC_lw32_p(RL7, REG3, DEFAULT);
    VoC_NOP();
    VoC_sw32inc_p(RL7, REG2, DEFAULT);
    VoC_add16_rr(REG3, REG3, REG7, IN_PARALLEL);
    VoC_add16_rd(REG6, REG6, SPEEX_CONST_1);
    //VoC_NOP();
    //printf("j: 0x%x\tm: 0x%x\tp: 0x%x\tFout: 0x%x\tf: 0x%x\tfstride: 0x%x\n",
    //  REGS[6].reg, REGS[5].reg/2, REGS[4].reg, REGS[2].reg, REGS[3].reg, REGS[7].reg/2);
    VoC_jump(SPX_KF_SHUFFLE_FOR1_BEGIN);
SPX_KF_SHUFFLE_FOR1_END:

    VoC_jump(SPX_KF_SHUFFLE_END);
SPX_KF_SHUFFLE_IF:
    VoC_lw16i_short(REG6, 0, DEFAULT);
SPX_KF_SHUFFLE_FOR2_BEGIN:
    VoC_be16_rr(SPX_KF_SHUFFLE_FOR2_END, REG6, REG4)
    //VoC_mult16_rr(REG7, REG7, REG4, DEFAULT);
    VoC_jal(spx_kf_shuffle);

    VoC_add16_rr(REG3, REG3, REG7, DEFAULT);
    VoC_add16_rr(REG2, REG2, REG5, IN_PARALLEL);
    VoC_add16_rd(REG6, REG6, SPEEX_CONST_1);
    VoC_NOP();
    //printf("j: 0x%x\tm: 0x%x\tp: 0x%x\tFout: 0x%x\tf: 0x%x\tfstride: 0x%x\n",
    //  REGS[6].reg, REGS[5].reg/2, REGS[4].reg, REGS[2].reg, REGS[3].reg, REGS[7].reg/2);
    VoC_jump(SPX_KF_SHUFFLE_FOR2_BEGIN);
SPX_KF_SHUFFLE_FOR2_END:
SPX_KF_SHUFFLE_END:

    VoC_pop32(REG67, DEFAULT);
    VoC_pop32(REG45, DEFAULT);
    VoC_pop32(REG23, DEFAULT);
    VoC_pop32(REG01, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/***********************
 * INPUT :  REG2: kiss_fft_cpx * Fout,
            REG7: const size_t fstride,
            REG0: const kiss_fft_cfg st = st->substate,
            REG5: int m,
            REG6: int N,
            RL7_HI(REG4): int mm
 * output : none
 **********************/
// now working as intended
void spx_kf_bfly2(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push32(REG01, DEFAULT);
    VoC_push32(REG23, DEFAULT);
    VoC_push32(REG45, DEFAULT);
    VoC_push32(REG67, DEFAULT);
    VoC_push32(RL6, DEFAULT);
    VoC_push32(RL7, DEFAULT);

    VoC_movreg16(REG4, RL7_HI, DEFAULT);
    //VoC_shr16_ri(REG5, -1, IN_PARALLEL);
    VoC_lw16i(REG3, SUBSTATE_INVERSE_OFFSET);

    VoC_add16_rr(REG1, REG0, REG3, DEFAULT);
    VoC_NOP();
    //VoC_bnez16_d(SPX_KF_BFLY2_IF, KFS_substate_inverse)
    VoC_lw16_p(REG1, REG1, DEFAULT);
    VoC_lw16i_short(INC0, 2, IN_PARALLEL);
    VoC_lw16i_short(INC1, 1, DEFAULT);
    VoC_lw16i_short(INC2, 1, IN_PARALLEL);
    VoC_bnez16_r(SPX_KF_BFLY2_IF, REG1)
    VoC_loop_r(1, REG6)
    VoC_push16(REG0, DEFAULT);
    VoC_push16(REG2, DEFAULT);
    //VoC_add16_rr(REG2, REG2, REG4, DEFAULT); // Fout in REG2
    VoC_add16_rr(REG1, REG2, REG5, DEFAULT); // Fout2 in REG1
    VoC_add16_rr(REG1, REG1, REG5, DEFAULT);
    //VoC_lw16i_set_inc(REG0, KFS_substate_twiddles, 1);
    VoC_lw16i(REG3, SUBSTATE_TWIDDLES_OFFSET);

    VoC_add16_rr(REG0, REG0, REG3, DEFAULT); // REG0 POINT to st->twiddles
    VoC_push32(REG45, IN_PARALLEL);
    VoC_loop_r(0, REG5)
    VoC_push32(REG67, DEFAULT)
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_lw32_p(REG45, REG1, DEFAULT);
    VoC_lw32_p(REG67, REG0, DEFAULT);

    VoC_mac32_rr(REG5, REG7, DEFAULT);
    VoC_mac32_rr(REG4, REG7, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);
    VoC_mac32_rr(REG4, REG6, DEFAULT);
    VoC_mac32_rr(REG5, REG6, IN_PARALLEL);
    VoC_NOP();
    VoC_shr32_ri(ACC0, 1, DEFAULT);
    VoC_shr32_ri(ACC1, 1, IN_PARALLEL); // now tr in ACC0, ti in ACC1

    //VoC_pop16(REG1, DEFAULT);
    VoC_lw16i_short(REG7, 0 ,DEFAULT);
    // Fout2->r = PSHR32(SUB32(SHL32(EXTEND32(Fout->r), 14), tr), 15);
    // Fout->r = PSHR32(ADD32(SHL32(EXTEND32(Fout->r), 14), tr), 15);
    VoC_lw16_p(REG6, REG2, DEFAULT);

    VoC_bnltz16_r(SPX_KF_BFLY2_EXTEND32, REG6)
    VoC_lw16i(REG7, 0xffff);
SPX_KF_BFLY2_EXTEND32:

    VoC_shr32_ri(REG67, -14, DEFAULT);

    VoC_sub32_rr(RL7, REG67, ACC0, DEFAULT);
    VoC_add32_rr(RL6, REG67, ACC0, IN_PARALLEL);

    VoC_add32_rd(RL6, RL6, SPEEX_CONST_16384);
    VoC_add32_rd(RL7, RL7, SPEEX_CONST_16384);

    VoC_shr32_ri(RL6, 15, DEFAULT);
    VoC_shr32_ri(RL7, 15, IN_PARALLEL);
    VoC_NOP();
    VoC_sw16inc_p(RL6_LO, REG2, DEFAULT);
    VoC_sw16inc_p(RL7_LO, REG1, DEFAULT);

    // Fout2->i = PSHR32(SUB32(SHL32(EXTEND32(Fout->i), 14), ti), 15);
    // Fout->i = PSHR32(ADD32(SHL32(EXTEND32(Fout->i), 14), ti), 15);
    VoC_lw16_p(REG6, REG2, DEFAULT);
    VoC_lw16i_short(REG7, 0, DEFAULT);
    VoC_bnltz16_r(SPX_KF_BFLY2_EXTEND32_2, REG6)
    VoC_lw16i(REG7, 0xffff);
SPX_KF_BFLY2_EXTEND32_2:

    VoC_shr32_ri(REG67, -14, DEFAULT);

    VoC_sub32_rr(RL7, REG67, ACC1, DEFAULT);
    VoC_add32_rr(RL6, REG67, ACC1, DEFAULT);

    VoC_add32_rd(RL6, RL6, SPEEX_CONST_16384);
    VoC_add32_rd(RL7, RL7, SPEEX_CONST_16384);

    VoC_shr32_ri(RL6, 15, DEFAULT);
    VoC_shr32_ri(RL7, 15, IN_PARALLEL);
    VoC_NOP();
    VoC_sw16inc_p(RL6_LO, REG2, DEFAULT);
    VoC_sw16inc_p(RL7_LO, REG1, DEFAULT);

    // tw1 += fstride;
    VoC_pop32(REG67, DEFAULT);

    VoC_add16_rr(REG0, REG0, REG7, DEFAULT);
    //VoC_add16_rd(REG2, REG2, SPEEX_CONST_2);
    VoC_endloop0
    VoC_pop32(REG45, DEFAULT);
    VoC_pop16(REG2, DEFAULT);
    VoC_pop16(REG0, DEFAULT);
    VoC_add16_rr(REG2, REG2, REG4, DEFAULT); // Fout in REG2
    VoC_add16_rr(REG2, REG2, REG4, DEFAULT);
    VoC_endloop1
    VoC_jump(SPX_KF_BFLY2_END);
SPX_KF_BFLY2_IF:
    VoC_lw16i_short(INC1, 2, DEFAULT);
    VoC_lw16i_short(INC2, 2, IN_PARALLEL);
    VoC_loop_r(1, REG6)
    VoC_push16(REG0, DEFAULT);
    VoC_push16(REG2, DEFAULT);
    VoC_add16_rr(REG1, REG2, REG5, DEFAULT); // Fout2 in REG1
    VoC_add16_rr(REG1, REG1, REG5, DEFAULT);
    //VoC_lw16i_set_inc(REG0, KFS_substate_twiddles, 1);
    VoC_lw16i(REG3, SUBSTATE_TWIDDLES_OFFSET);

    VoC_add16_rr(REG0, REG0, REG3, DEFAULT); // REG0 POINT to st->twiddles
    VoC_push32(REG45, IN_PARALLEL);
    VoC_loop_r(0, REG5)
    VoC_push32(REG67, DEFAULT)
    //VoC_push16(REG1, IN_PARALLEL);
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_lw32_p(REG45, REG1, DEFAULT);
    VoC_lw32_p(REG67, REG0, DEFAULT);

    VoC_mac32_rr(REG5, REG7, DEFAULT);
    VoC_mac32_rr(REG4, REG7, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);

    VoC_mac32_rr(REG4, REG6, DEFAULT);
    VoC_mac32_rr(REG5, REG6, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL); // now tr in ACC0, ti in ACC1

    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_movreg32(RL7, ACC1, IN_PARALLEL);

    VoC_movreg16(REG6, RL6_LO, DEFAULT);
    VoC_movreg16(REG7, RL7_LO, IN_PARALLEL); // now t in reg67

    VoC_push32(REG01, DEFAULT);
    VoC_lw32_p(REG45, REG2, DEFAULT);

    VoC_sub16_rr(REG0, REG4, REG6, DEFAULT);
    VoC_sub16_rr(REG1, REG5, REG7, IN_PARALLEL);

    VoC_movreg32(RL7, REG01, DEFAULT);
    // now *Fout2 in RL7
    VoC_pop32(REG01, DEFAULT);
    VoC_NOP();
    VoC_sw32inc_p(RL7, REG1, DEFAULT); // store *Fout2

    VoC_add16_rr(REG4, REG4, REG6, DEFAULT);
    VoC_add16_rr(REG5, REG5, REG7, IN_PARALLEL);
    VoC_NOP();
    VoC_sw32inc_p(REG45, REG2, DEFAULT);

    // tw1 += fstride;
    VoC_pop32(REG67, DEFAULT);

    VoC_add16_rr(REG0, REG0, REG7, DEFAULT);
    VoC_endloop0
    VoC_pop32(REG45, DEFAULT);
    VoC_pop16(REG2, DEFAULT);
    VoC_pop16(REG0, DEFAULT);
    VoC_add16_rr(REG2, REG2, REG4, DEFAULT);
    VoC_add16_rr(REG2, REG2, REG4, DEFAULT);

    VoC_endloop1
SPX_KF_BFLY2_END:
    VoC_pop32(RL7, DEFAULT);
    VoC_pop32(RL6, DEFAULT);
    VoC_pop32(REG67, DEFAULT);
    VoC_pop32(REG45, DEFAULT);
    VoC_pop32(REG23, DEFAULT);
    VoC_pop32(REG01, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/***********************
 * INPUT :  REG2: kiss_fft_cpx * Fout,
            REG7: const size_t fstride: SPX_KF_BFLY_FSTRIDE
            const kiss_fft_cfg st,
            REG5: size_t m: SPX_KF_BFLY_M
 * output : none
 ***********************/
void spx_kf_bfly3(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push32(REG01, DEFAULT);
    VoC_push32(REG23, DEFAULT);
    VoC_push32(REG45, DEFAULT);
    VoC_push32(REG67, DEFAULT);
    VoC_push32(RL6, DEFAULT);
    VoC_push32(RL7, DEFAULT);

    // epi3 = st->twiddles[fstride*m];
    VoC_lw16_d(REG5, SPX_KF_BFLY_M);

    VoC_mult16_rd(REG4, REG5, SPX_KF_BFLY_FSTRIDE);

    VoC_lw16i(REG3, SUBSTATE_TWIDDLES_OFFSET);

    VoC_add16_rr(REG3, REG0, REG3, DEFAULT);
    //VoC_lw16i_set_inc(REG3, KFS_substate_twiddles, 2);
    VoC_add16_rr(REG4, REG4, REG3, DEFAULT);
    VoC_NOP();
    VoC_lw32_p(REG67, REG4, DEFAULT);
    VoC_NOP();
    VoC_sw32_d(REG67, SPX_KF_BFLY3_EPI3);

    VoC_movreg16(REG4, REG5, DEFAULT);

    VoC_shr16_ri(REG4, -1, DEFAULT);

    VoC_sw16_d(REG4, SPX_KF_BFLY_M);

    //VoC_lw16i_short(REG7, 0, DEFAULT);

    //VoC_sw16_d(REG7, SPX_KF_BFLY_TMP);

    VoC_loop_r(1, REG5)
    VoC_push16(REG7, DEFAULT);
    VoC_lw16i(REG7, SUBSTATE_INVERSE_OFFSET);

    VoC_add16_rr(REG7, REG7, REG0, DEFAULT);
    VoC_lw16_p(REG7, REG7, DEFAULT);
    VoC_lw16i_short(REG6, 0, DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG6, SPX_KF_BFLY_TMP);
    //VoC_bnez16_d(SPX_KF_BFLY3_IF, KFS_substate_inverse)
    VoC_bnez16_r(SPX_KF_BFLY3_IF, REG7)
    //VoC_push32(REG67, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, DEFAULT);
    VoC_loop_i_short(6, DEFAULT);
    VoC_startloop0
    VoC_lw32_p(REG45, REG2, DEFAULT);
    VoC_lw16i(REG7, 0x2aaa);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG4, REG7, DEFAULT);
    VoC_mac32_rr(REG5, REG7, IN_PARALLEL);
    VoC_NOP();
    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_movreg32(RL7, ACC1, IN_PARALLEL);

    VoC_add32_rd(RL6, RL6, SPEEX_CONST_16384);
    VoC_add32_rd(RL7, RL7, SPEEX_CONST_16384);

    VoC_shr32_ri(RL6, 15, DEFAULT);
    VoC_shr32_ri(RL7, 15, IN_PARALLEL);

    VoC_movreg16(REG4, RL6_LO, DEFAULT);
    VoC_movreg16(REG5, RL7_LO, IN_PARALLEL);

    VoC_sw32_p(REG45, REG2, DEFAULT);
    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_M);
    VoC_endloop0
    //VoC_pop32(REG67, DEFAULT);
SPX_KF_BFLY3_IF:
    // C_MUL(scratch[1],Fout[m] , *tw1);
    VoC_push16(REG2, DEFAULT);

    VoC_add16_rd(REG6, REG2, SPX_KF_BFLY_M);
    VoC_add16_rd(REG7, REG3, SPX_KF_BFLY_TMP);

    VoC_lw32_p(REG01, REG6, DEFAULT);
    VoC_lw32_p(REG45, REG7, DEFAULT);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG1, REG5, DEFAULT);
    VoC_mac32_rr(REG0, REG5, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);

    VoC_mac32_rr(REG0, REG4, DEFAULT);
    VoC_mac32_rr(REG1, REG4, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);

    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_movreg32(RL7, ACC1, IN_PARALLEL);
    VoC_movreg16(REG0, RL6_LO, DEFAULT);
    VoC_movreg16(REG1, RL7_LO, IN_PARALLEL);
    VoC_NOP();
    VoC_sw32_d(REG01, SPX_KF_BFLY_SCRATCH_1);

    // C_MUL(scratch[2],Fout[m2] , *tw2);
    VoC_add16_rd(REG6, REG6, SPX_KF_BFLY_M);
    VoC_add16_rd(REG7, REG7, SPX_KF_BFLY_TMP);
    VoC_lw32_p(REG01, REG6, DEFAULT);
    VoC_lw32_p(REG45, REG7, DEFAULT);

    VoC_mac32_rr(REG1, REG5, DEFAULT);
    VoC_mac32_rr(REG0, REG5, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);

    VoC_mac32_rr(REG0, REG4, DEFAULT);
    VoC_mac32_rr(REG1, REG4, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);

    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_movreg32(RL7, ACC1, IN_PARALLEL);
    VoC_movreg16(REG0, RL6_LO, DEFAULT);
    VoC_movreg16(REG1, RL7_LO, IN_PARALLEL);
    VoC_NOP();
    VoC_sw32_d(REG01, SPX_KF_BFLY_SCRATCH_2);

    // C_ADD(scratch[3],scratch[1],scratch[2]);
    // C_SUB(scratch[0],scratch[1],scratch[2]);
    VoC_sub16_dr(REG6, SPX_KF_BFLY_SCRATCH_1, REG0);
    VoC_sub16_dr(REG7, SPX_KF_BFLY_SCRATCH_1i, REG1);
    VoC_NOP();
    VoC_sw32_d(REG67, SPX_KF_BFLY_SCRATCH);
    VoC_add16_rd(REG0, REG0, SPX_KF_BFLY_SCRATCH_1);
    VoC_add16_rd(REG1, REG1, SPX_KF_BFLY_SCRATCH_1i);
    VoC_NOP();
    VoC_sw32_d(REG01, SPX_KF_BFLY_SCRATCH_3);

    // Fout[m].r = Fout->r - HALF_OF(scratch[3].r);
    // Fout[m].i = Fout->i - HALF_OF(scratch[3].i);
    VoC_pop16(REG2, DEFAULT);
    VoC_lw32_p(REG67, REG2, DEFAULT);
    VoC_push16(REG2, IN_PARALLEL);

    VoC_shr16_ri(REG0, 1, DEFAULT);
    VoC_shr16_ri(REG1, 1, IN_PARALLEL);
    VoC_sub16_rr(REG0, REG6, REG0, DEFAULT);
    VoC_sub16_rr(REG1, REG7, REG1, IN_PARALLEL);
    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_M);
    VoC_NOP();
    VoC_sw32_p(REG01, REG2, DEFAULT);

    // C_ADDTO(*Fout,scratch[3]);
    VoC_pop16(REG2, DEFAULT);
    VoC_add16_rd(REG6, REG6, SPX_KF_BFLY_SCRATCH_3);
    VoC_add16_rd(REG7, REG7, SPX_KF_BFLY_SCRATCH_3i);
    VoC_push16(REG2, DEFAULT);
    VoC_sw32_p(REG67, REG2, DEFAULT);

    // C_MULBYSCALAR( scratch[0] , epi3.i );
    VoC_lw32_d(REG01, SPX_KF_BFLY_SCRATCH_0);
    VoC_lw32_d(REG67, SPX_KF_BFLY3_EPI3);
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG0, REG7, DEFAULT);
    VoC_mac32_rr(REG1, REG7, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);

    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_movreg32(RL7, ACC1, IN_PARALLEL);
    VoC_movreg16(REG6, RL6_LO, DEFAULT);
    VoC_movreg16(REG7, RL7_LO, IN_PARALLEL);
    VoC_NOP();
    VoC_sw32_d(REG67, SPX_KF_BFLY_SCRATCH_0);

    // Fout[m2].r = Fout[m].r + scratch[0].i;
    // Fout[m2].i = Fout[m].i - scratch[0].r;
    // Fout[m].r -= scratch[0].i;
    // Fout[m].i += scratch[0].r;
    VoC_pop16(REG2, DEFAULT);
    VoC_push16(REG2, DEFAULT);
    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_M);

    VoC_lw32_p(REG01, REG2, DEFAULT);

    VoC_add16_rr(REG2, REG0, REG6, DEFAULT);
    VoC_sub16_rr(REG3, REG1, REG7, IN_PARALLEL);
    VoC_sub16_rr(REG6, REG0, REG6, DEFAULT);
    VoC_add16_rr(REG7, REG1, REG7, IN_PARALLEL);
    VoC_NOP();
    VoC_sw32_p(REG67, REG2, DEFAULT);
    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_M);
    VoC_NOP();
    VoC_sw32_p(REG23, REG2, DEFAULT);

    VoC_pop16(REG2, DEFAULT);
    VoC_pop16(REG7, DEFAULT);
    VoC_NOP();
    VoC_add16_rd(REG7, REG7, SPX_KF_BFLY_FSTRIDE);
    VoC_NOP();
    VoC_sw16_d(REG7, SPX_KF_BFLY_TMP);
    VoC_NOP();
    VoC_endloop1

    VoC_pop32(RL7, DEFAULT);
    VoC_pop32(RL6, DEFAULT);
    VoC_pop32(REG67, DEFAULT);
    VoC_pop32(REG45, DEFAULT);
    VoC_pop32(REG23, DEFAULT);
    VoC_pop32(REG01, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/***********************
 * INPUT :  kiss_fft_cpx * Fout,
            const size_t fstride, : SPX_KF_BFLY_FSTRIDE
            int m, : SPX_KF_BFLY_M
            int N, : SPX_KF_BFLY_N
            int mm: SPX_KF_BFLY_MM
 * output : none
 **********************/
// now working as intended
void spx_kf_bfly4(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push32(REG01, DEFAULT);
    VoC_push32(REG23, DEFAULT);
    VoC_push32(REG45, DEFAULT);
    VoC_push32(REG67, DEFAULT);
    VoC_push32(RL6, DEFAULT);
    VoC_push32(RL7, DEFAULT);


    //VoC_lw16i_short(REG6, 0, DEFAULT);

    //VoC_sw16_d(REG6, SPX_KF_BFLY_TMP);

    VoC_lw16_d(REG5, SPX_KF_BFLY_MM);

    VoC_shr16_ri(REG5, -1, DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG5, SPX_KF_BFLY_MM);
    VoC_lw16_d(REG6, SPX_KF_BFLY_N); // N in REG6
    VoC_lw16_d(REG7, SPX_KF_BFLY_M); // M in reg7

    VoC_movreg16(REG5, REG7, DEFAULT);
    VoC_shr16_ri(REG5, -1, DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG5, SPX_KF_BFLY_M); // SPX_KF_BFLY_M now stores 2*M

    VoC_lw16i(REG5, SUBSTATE_INVERSE_OFFSET);

    VoC_add16_rr(REG5, REG0, REG5, DEFAULT); // reg5 = st->inverse
    VoC_NOP();
    VoC_lw16_p(REG5, REG5, DEFAULT);
    //VoC_lw16_d(REG5, KFS_substate_inverse);
    VoC_lw16i_short(FORMAT32, 0, DEFAULT);

    VoC_bez16_r(SPX_KF_BFLY4_IF, REG5)
    VoC_loop_r(1, REG6)
    VoC_push16(REG0, DEFAULT);
    VoC_push16(REG2, DEFAULT);
    //VoC_lw16i_short(REG6, 0, DEFAULT);
    //VoC_lw16i_set_inc(REG3, KFS_substate_twiddles, 2);
    VoC_lw16i(REG3, SUBSTATE_TWIDDLES_OFFSET);

    VoC_add16_rr(REG3, REG0, REG3, DEFAULT); // r3 POINT to st->twiddles
    //VoC_lw16i_short(INC3, 2, DEFAULT);
    VoC_lw16_d(REG7, SPX_KF_BFLY_M);

    VoC_shr16_ri(REG7, 1, DEFAULT);
    VoC_lw16i_short(REG6, 0, DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG6, SPX_KF_BFLY_TMP);
    VoC_loop_r(0, REG7)
    // C_MUL(scratch[0],Fout[m] , *(st->twiddles+tmp) );
    //printf("&Fout: 0x%x\ttmp: 0x%x\n", REGS[2].reg, REGS[7].reg);
    VoC_push16(REG3, DEFAULT);
    VoC_push16(REG2, DEFAULT);
    VoC_add16_rd(REG6, REG2, SPX_KF_BFLY_M);
    VoC_add16_rd(REG7, REG3, SPX_KF_BFLY_TMP);
    VoC_NOP();
    VoC_lw32_p(REG01, REG6, DEFAULT);
    VoC_lw32_p(REG45, REG7, DEFAULT); // load Fout[m], tw1

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG1, REG5, DEFAULT);
    VoC_mac32_rr(REG0, REG5, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);

    VoC_mac32_rr(REG0, REG4, DEFAULT);
    VoC_mac32_rr(REG1, REG4, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);

    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_movreg32(RL7, ACC1, IN_PARALLEL);
    VoC_movreg16(REG0, RL6_LO, DEFAULT);
    VoC_movreg16(REG1, RL7_LO, IN_PARALLEL);
    VoC_NOP();
    VoC_sw32_d(REG01, SPX_KF_BFLY_SCRATCH_0);

    // C_MUL(scratch[1],Fout[m2] , *(st->twiddles+2*tmp) );
    VoC_add16_rd(REG6, REG6, SPX_KF_BFLY_M);
    VoC_add16_rd(REG7, REG7, SPX_KF_BFLY_TMP);
    VoC_NOP();
    VoC_lw32_p(REG01, REG6, DEFAULT);
    VoC_lw32_p(REG45, REG7, DEFAULT);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG1, REG5, DEFAULT);
    VoC_mac32_rr(REG0, REG5, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);

    VoC_mac32_rr(REG0, REG4, DEFAULT);
    VoC_mac32_rr(REG1, REG4, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);

    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_movreg32(RL7, ACC1, IN_PARALLEL);
    VoC_movreg16(REG0, RL6_LO, DEFAULT);
    VoC_movreg16(REG1, RL7_LO, IN_PARALLEL);
    VoC_NOP();
    VoC_sw32_d(REG01, SPX_KF_BFLY_SCRATCH_1);

    // C_MUL(scratch[2],Fout[m3] , *(st->twiddles+3*tmp) );
    VoC_add16_rd(REG6, REG6, SPX_KF_BFLY_M);
    VoC_add16_rd(REG7, REG7, SPX_KF_BFLY_TMP);
    VoC_NOP();
    VoC_lw32_p(REG01, REG6, DEFAULT);
    VoC_lw32_p(REG45, REG7, DEFAULT);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG1, REG5, DEFAULT);
    VoC_mac32_rr(REG0, REG5, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);

    VoC_mac32_rr(REG0, REG4, DEFAULT);
    VoC_mac32_rr(REG1, REG4, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);

    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_movreg32(RL7, ACC1, IN_PARALLEL);
    VoC_movreg16(REG0, RL6_LO, DEFAULT);
    VoC_movreg16(REG1, RL7_LO, IN_PARALLEL);
    VoC_NOP();
    VoC_sw32_d(REG01, SPX_KF_BFLY_SCRATCH_2);

    // C_ADD( scratch[3] , scratch[0] , scratch[2] );
    // C_SUB( scratch[4] , scratch[0] , scratch[2] );
    VoC_add16_rd(REG4, REG0, SPX_KF_BFLY_SCRATCH_0);
    VoC_add16_rd(REG5, REG1, SPX_KF_BFLY_SCRATCH_0i);
    VoC_NOP();
    VoC_sw32_d(REG45, SPX_KF_BFLY_SCRATCH_3);

    VoC_sub16_dr(REG4, SPX_KF_BFLY_SCRATCH, REG0);
    VoC_sub16_dr(REG5, SPX_KF_BFLY_SCRATCH_0i, REG1);
    VoC_NOP();
    VoC_sw32_d(REG45, SPX_KF_BFLY_SCRATCH_4);

    // C_SUB( scratch[5] , *Fout, scratch[1] );
    // C_ADDTO(*Fout, scratch[1]);
    // C_SUB( Fout[m2], *Fout, scratch[3] );
    VoC_pop16(REG2, DEFAULT); // now reg7 remains in stack
    VoC_NOP();

    VoC_lw16_p(REG6,REG2,DEFAULT);

    VoC_sub16_rd(REG0, REG6, SPX_KF_BFLY_SCRATCH_1);
    VoC_add16_rd(REG6, REG6, SPX_KF_BFLY_SCRATCH_1);

    VoC_push16(REG2, DEFAULT);

    VoC_sub16_rd(REG4, REG6, SPX_KF_BFLY_SCRATCH_3);

    VoC_add16_rd(REG2, REG2, SPEEX_CONST_1);
    VoC_NOP();



    VoC_lw16_p(REG7,REG2,DEFAULT);

    VoC_sub16_rd(REG1, REG7, SPX_KF_BFLY_SCRATCH_1i);
    VoC_add16_rd(REG7, REG7, SPX_KF_BFLY_SCRATCH_1i);
    VoC_NOP();
    VoC_sub16_rd(REG5, REG7, SPX_KF_BFLY_SCRATCH_3i); // REG01 = scratch[5] reg67 = *Fout REG45 = Fout[m2]

    VoC_sw32_d(REG01, SPX_KF_BFLY_SCRATCH_5);
    VoC_sub16_rd(REG2, REG2, SPEEX_CONST_1);
    VoC_NOP();
    VoC_sw32_p(REG67, REG2, DEFAULT);

    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_M);

    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_M);
    VoC_NOP();
    VoC_sw32_p(REG45, REG2, DEFAULT);

    VoC_pop16(REG2, DEFAULT);
    VoC_NOP();

    // C_ADDTO( *Fout , scratch[3] );
    VoC_push16(REG2, DEFAULT);
    VoC_lw16i_short(INC2, 1, IN_PARALLEL);
    VoC_NOP();

    VoC_lw16inc_p(REG0,REG2,DEFAULT);

    VoC_add16_rd(REG0, REG0, SPX_KF_BFLY_SCRATCH_3);

    VoC_lw16_p(REG1,REG2,DEFAULT);

    VoC_add16_rd(REG1, REG1, SPX_KF_BFLY_SCRATCH_3i);

    VoC_pop16(REG2, DEFAULT);
    VoC_NOP();
    VoC_sw32_p(REG01, REG2, DEFAULT);

    // tempr= scratch[5].r - scratch[4].i;
    // tempi= scratch[5].i + scratch[4].r;
    VoC_push16(REG2, DEFAULT);

    VoC_lw32_d(REG67, SPX_KF_BFLY_SCRATCH_5);
    VoC_lw32_d(REG45, SPX_KF_BFLY_SCRATCH_4);

    VoC_sub16_rr(REG0, REG6, REG5, DEFAULT);
    VoC_add16_rr(REG1, REG7, REG4, IN_PARALLEL);
    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_M);

    /*
    VoC_bnlt16_rd(SPX_KF_BFLY4_TEMPR, REG0, SPEEX_CONST_NEG32767)
        VoC_lw16_d(REG0, SPEEX_CONST_NEG32767);
    SPX_KF_BFLY4_TEMPR:
    VoC_bnlt16_rd(SPX_KF_BFLY4_TEMPI, REG1, SPEEX_CONST_NEG32767)
        VoC_lw16_d(REG1, SPEEX_CONST_NEG32767);
    SPX_KF_BFLY4_TEMPI:
    */
    VoC_NOP();
    VoC_sw32_p(REG01, REG2, DEFAULT);

    // tempr = scratch[5].r + scratch[4].i;
    // tempi = scratch[5].i - scratch[4].r;
    VoC_add16_rr(REG0, REG6, REG5, DEFAULT);
    VoC_sub16_rr(REG1, REG7, REG4, IN_PARALLEL);

    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_M);

    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_M);

    /*
    VoC_bnlt16_rd(SPX_KF_BFLY4_TEMPR2, REG0, SPEEX_CONST_NEG32767)
        VoC_lw16_d(REG0, SPEEX_CONST_NEG32767);
    SPX_KF_BFLY4_TEMPR2:
    VoC_bnlt16_rd(SPX_KF_BFLY4_TEMPI2, REG1, SPEEX_CONST_NEG32767)
        VoC_lw16_d(REG1, SPEEX_CONST_NEG32767);
    SPX_KF_BFLY4_TEMPI2:
    */
    VoC_NOP();
    VoC_sw32_p(REG01, REG2, DEFAULT);

    VoC_pop16(REG2, DEFAULT);
    VoC_pop16(REG3, DEFAULT);

    VoC_lw16_d(REG7, SPX_KF_BFLY_TMP);
    VoC_add16_rd(REG2, REG2, SPEEX_CONST_2);
    VoC_add16_rd(REG7, REG7, SPX_KF_BFLY_FSTRIDE);
    VoC_NOP();
    VoC_sw16_d(REG7, SPX_KF_BFLY_TMP);

    VoC_endloop0
    //VoC_pop16(REG7, DEFAULT);
    VoC_pop16(REG2, DEFAULT);
    VoC_pop16(REG0, DEFAULT);
    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_MM);

    VoC_endloop1
    VoC_jump(SPX_KF_BFLY4_END);
SPX_KF_BFLY4_IF:
    VoC_loop_r(1, REG6)
    VoC_push16(REG0, DEFAULT);
    VoC_push16(REG2, DEFAULT);
    //VoC_lw16i_short(REG6, 0, DEFAULT);
    //VoC_lw16i_set_inc(REG3, KFS_substate_twiddles, 2);
    VoC_lw16i(REG3, SUBSTATE_TWIDDLES_OFFSET);

    VoC_add16_rr(REG3, REG0, REG3, DEFAULT);
    VoC_lw16_d(REG7, SPX_KF_BFLY_M);

    VoC_shr16_ri(REG7, 1, DEFAULT);
    VoC_lw16i_short(REG6, 0, DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG6, SPX_KF_BFLY_TMP);
    //VoC_lw16i_short(INC3, 2, DEFAULT);
    VoC_loop_r(0, REG7)
    // C_MUL(scratch[0],Fout[m] , *(st->twiddles+tmp) );
    VoC_push16(REG3, DEFAULT);
    VoC_push16(REG2, DEFAULT);
    //printf("&Fout: 0x%x\tst->twiddles: 0x%x\n", REGS[2].reg, REGS[3].reg);
    VoC_add16_rd(REG6, REG2, SPX_KF_BFLY_M);
    VoC_add16_rd(REG7, REG3, SPX_KF_BFLY_TMP);
    VoC_NOP();
    VoC_lw32_p(REG01, REG6, DEFAULT);
    VoC_lw32_p(REG45, REG7, DEFAULT);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG1, REG5, DEFAULT);
    VoC_mac32_rr(REG0, REG5, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);

    VoC_mac32_rr(REG0, REG4, DEFAULT);
    VoC_mac32_rr(REG1, REG4, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_65536);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_65536);

    VoC_shr32_ri(ACC0, 17, DEFAULT);
    VoC_shr32_ri(ACC1, 17, IN_PARALLEL);

    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_movreg32(RL7, ACC1, IN_PARALLEL);
    VoC_movreg16(REG0, RL6_LO, DEFAULT);
    VoC_movreg16(REG1, RL7_LO, IN_PARALLEL);
    VoC_NOP();
    VoC_sw32_d(REG01, SPX_KF_BFLY_SCRATCH_0);

    // C_MUL(scratch[1],Fout[m2] , *(st->twiddles+2*tmp) );
    VoC_add16_rd(REG6, REG6, SPX_KF_BFLY_M);
    VoC_add16_rd(REG7, REG7, SPX_KF_BFLY_TMP);
    VoC_NOP();
    VoC_lw32_p(REG01, REG6, DEFAULT);
    VoC_lw32_p(REG45, REG7, DEFAULT);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG1, REG5, DEFAULT);
    VoC_mac32_rr(REG0, REG5, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);

    VoC_mac32_rr(REG0, REG4, DEFAULT);
    VoC_mac32_rr(REG1, REG4, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_65536);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_65536);

    VoC_shr32_ri(ACC0, 17, DEFAULT);
    VoC_shr32_ri(ACC1, 17, IN_PARALLEL);

    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_movreg32(RL7, ACC1, IN_PARALLEL);
    VoC_movreg16(REG0, RL6_LO, DEFAULT);
    VoC_movreg16(REG1, RL7_LO, IN_PARALLEL);
    VoC_NOP();
    VoC_sw32_d(REG01, SPX_KF_BFLY_SCRATCH_1);

    // C_MUL(scratch[2],Fout[m3] , *(st->twiddles+3*tmp) );
    VoC_add16_rd(REG6, REG6, SPX_KF_BFLY_M);
    VoC_add16_rd(REG7, REG7, SPX_KF_BFLY_TMP);
    VoC_NOP();
    VoC_lw32_p(REG01, REG6, DEFAULT);
    VoC_lw32_p(REG45, REG7, DEFAULT);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG1, REG5, DEFAULT);
    VoC_mac32_rr(REG0, REG5, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);

    VoC_mac32_rr(REG0, REG4, DEFAULT);
    VoC_mac32_rr(REG1, REG4, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_65536);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_65536);

    VoC_shr32_ri(ACC0, 17, DEFAULT);
    VoC_shr32_ri(ACC1, 17, IN_PARALLEL);

    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_movreg32(RL7, ACC1, IN_PARALLEL);
    VoC_movreg16(REG0, RL6_LO, DEFAULT);
    VoC_movreg16(REG1, RL7_LO, IN_PARALLEL);
    VoC_NOP();
    VoC_sw32_d(REG01, SPX_KF_BFLY_SCRATCH_2);

    // C_ADD( scratch[3] , scratch[0] , scratch[2] );
    // C_SUB( scratch[4] , scratch[0] , scratch[2] );
    VoC_add16_rd(REG4, REG0, SPX_KF_BFLY_SCRATCH_0);
    VoC_add16_rd(REG5, REG1, SPX_KF_BFLY_SCRATCH_0i);
    VoC_NOP();
    VoC_sw32_d(REG45, SPX_KF_BFLY_SCRATCH_3);

    VoC_sub16_dr(REG4, SPX_KF_BFLY_SCRATCH_0, REG0);
    VoC_sub16_dr(REG5, SPX_KF_BFLY_SCRATCH_0i, REG1);
    VoC_NOP();
    VoC_sw32_d(REG45, SPX_KF_BFLY_SCRATCH_4);

    // Fout->r = PSHR16(Fout->r, 2);
    // Fout->i = PSHR16(Fout->i, 2);
    // C_SUB( scratch[5] , *Fout, scratch[1] );
    // C_ADDTO(*Fout, scratch[1]);
    VoC_lw32_p(REG01, REG2, DEFAULT);

    VoC_add16_rd(REG0, REG0, SPEEX_CONST_2);
    VoC_add16_rd(REG1, REG1, SPEEX_CONST_2);

    VoC_shr16_ri(REG0, 2, DEFAULT);
    VoC_shr16_ri(REG1, 2, IN_PARALLEL);
    VoC_sub16_rd(REG6, REG0, SPX_KF_BFLY_SCRATCH_1);
    VoC_sub16_rd(REG7, REG1, SPX_KF_BFLY_SCRATCH_1i);
    VoC_NOP();
    VoC_sw32_d(REG67, SPX_KF_BFLY_SCRATCH_5);
    VoC_add16_rd(REG0, REG0, SPX_KF_BFLY_SCRATCH_1);
    VoC_add16_rd(REG1, REG1, SPX_KF_BFLY_SCRATCH_1i);
    VoC_NOP();
    VoC_sw32_p(REG01, REG2, DEFAULT);

    // Fout[m2].r = PSHR16(Fout[m2].r, 2);
    // Fout[m2].i = PSHR16(Fout[m2].i, 2);
    //VoC_push16(REG2, DEFAULT);
    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_M);
    VoC_sub16_rd(REG6, REG0, SPX_KF_BFLY_SCRATCH_3);
    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_M);
    VoC_sub16_rd(REG7, REG1, SPX_KF_BFLY_SCRATCH_3i);
    VoC_NOP();
    VoC_sw32_p(REG67, REG2, DEFAULT);

    // // C_ADDTO( *Fout , scratch[3] );
    VoC_pop16(REG2, DEFAULT);

    VoC_add16_rd(REG0, REG0, SPX_KF_BFLY_SCRATCH_3);
    VoC_add16_rd(REG1, REG1, SPX_KF_BFLY_SCRATCH_3i);
    VoC_NOP();
    VoC_sw32_p(REG01, REG2, DEFAULT);

    // tempr= scratch[5].r + scratch[4].i;
    // tempi= scratch[5].i - scratch[4].r;
    VoC_push16(REG2, DEFAULT);
    VoC_lw32_d(REG67, SPX_KF_BFLY_SCRATCH_5);
    VoC_lw32_d(REG45, SPX_KF_BFLY_SCRATCH_4);
    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_M);
    VoC_add16_rr(REG0, REG6, REG5, DEFAULT);
    VoC_sub16_rr(REG1, REG7, REG4, IN_PARALLEL);

    /*
    VoC_bnlt16_rd(SPX_KF_BFLY4_LEAST_1, REG0, SPEEX_CONST_NEG32767)
        VoC_lw16_d(REG0, SPEEX_CONST_NEG32767);
    SPX_KF_BFLY4_LEAST_1:
    VoC_bnlt16_rd(SPX_KF_BFLY4_LEAST_2, REG1, SPEEX_CONST_NEG32767)
        VoC_lw16_d(REG1, SPEEX_CONST_NEG32767);
    SPX_KF_BFLY4_LEAST_2:
    */
    VoC_NOP();
    VoC_sw32_p(REG01, REG2, DEFAULT);

    // tempr = scratch[5].r - scratch[4].i;
    // tempi = scratch[5].i + scratch[4].r;
    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_M);
    VoC_sub16_rr(REG0, REG6, REG5, DEFAULT);
    VoC_add16_rr(REG1, REG7, REG4, IN_PARALLEL);
    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_M);

    /*
    VoC_bnlt16_rd(SPX_KF_BFLY4_LEAST_3, REG0, SPEEX_CONST_NEG32767)
        VoC_lw16_d(REG0, SPEEX_CONST_NEG32767);
    SPX_KF_BFLY4_LEAST_3:
    VoC_bnlt16_rd(SPX_KF_BFLY4_LEAST_4, REG1, SPEEX_CONST_NEG32767)
        VoC_lw16_d(REG1, SPEEX_CONST_NEG32767);
    SPX_KF_BFLY4_LEAST_4:
    */
    VoC_NOP();
    VoC_sw32_p(REG01, REG2, DEFAULT);

    VoC_pop16(REG2, DEFAULT);
    VoC_pop16(REG3, DEFAULT);

    VoC_lw16_d(REG7, SPX_KF_BFLY_TMP);
    VoC_add16_rd(REG2, REG2, SPEEX_CONST_2);
    VoC_add16_rd(REG7, REG7, SPX_KF_BFLY_FSTRIDE);
    VoC_NOP();
    VoC_sw16_d(REG7, SPX_KF_BFLY_TMP);

    VoC_endloop0
    VoC_NOP();
    //VoC_pop32(REG7, DEFAULT);
    VoC_pop16(REG2, DEFAULT);
    VoC_pop16(REG0, DEFAULT);
    VoC_add16_rd(REG2, REG2, SPX_KF_BFLY_MM);

    VoC_endloop1
SPX_KF_BFLY4_END:

    VoC_pop32(RL7, DEFAULT);
    VoC_pop32(RL6, DEFAULT);
    VoC_pop32(REG67, DEFAULT);
    VoC_pop32(REG45, DEFAULT);
    VoC_pop32(REG23, DEFAULT);
    VoC_pop32(REG01, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/***********************
 * INPUT :  REG2: kiss_fft_cpx * Fout,
            REG7: const size_t fstride: SPX_KF_BFLY_FSTRIDE,
            const kiss_fft_cfg st,
            REG5: size_t m: SPX_KF_BFLY_M
 * output : none
 ***********************/
// now working as intended
void spx_kf_bfly5(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push32(REG01, DEFAULT);
    VoC_push32(REG23, DEFAULT);
    VoC_push32(REG45, DEFAULT);
    VoC_push32(REG67, DEFAULT);
    VoC_push32(RL6, DEFAULT);
    VoC_push32(RL7, DEFAULT);

    VoC_lw16_d(REG5, SPX_KF_BFLY_M);
    VoC_NOP();
    VoC_mult16_rr(REG4, REG7, REG5, DEFAULT);

    //VoC_lw16i(REG6, KFS_substate_twiddles);
    VoC_lw16i(REG6, SUBSTATE_TWIDDLES_OFFSET);

    VoC_add16_rr(REG3, REG0, REG6, DEFAULT);

    VoC_add16_rr(REG6, REG3, REG4, DEFAULT);
    VoC_NOP();
    VoC_lw32_p(RL7, REG6, DEFAULT);
    VoC_NOP();
    VoC_sw32_d(RL7, SPX_KF_BFLY_YA);

    VoC_add16_rr(REG6, REG6, REG4, DEFAULT);
    VoC_NOP();
    VoC_lw32_p(RL7, REG6, DEFAULT);
    VoC_NOP();
    VoC_sw32_d(RL7, SPX_KF_BFLY_YB);
    // calc ya,yb and store

    VoC_lw16i_short(INC2, 1, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    VoC_loop_r(1, REG5)
    VoC_lw16i(REG6, SUBSTATE_INVERSE_OFFSET);

    VoC_add16_rr(REG0, REG0, REG6, DEFAULT);
    VoC_NOP();
    VoC_lw16_p(REG0, REG0, DEFAULT);
    VoC_push16(REG2, DEFAULT);
    VoC_bnez16_r(SPX_KF_BFLY5_IF, REG0)
    VoC_loop_i_short(10, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);
    VoC_startloop0
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16_p(REG0, REG2, DEFAULT);

    VoC_lw16i(REG6, 6553);

    VoC_mac32_rr(REG0, REG6, DEFAULT);
    VoC_NOP();
    VoC_movreg32(REG01, ACC0, DEFAULT);

    VoC_add32_rd(REG01, REG01, SPEEX_CONST_16384);

    VoC_shr32_ri(REG01, 15, DEFAULT);
    VoC_NOP();
    VoC_sw16_p(REG0, REG2, DEFAULT);

    VoC_add16_rr(REG2, REG2, REG5, DEFAULT);
    VoC_endloop0
SPX_KF_BFLY5_IF: // tmp stored in reg4
    VoC_pop16(REG2, DEFAULT);
    VoC_shr16_ri(REG5, -1, DEFAULT);
    VoC_lw16i_short(REG4, 0, DEFAULT);
    VoC_push32(REG23, DEFAULT);
    VoC_lw32_p(RL7, REG2, DEFAULT);
    VoC_push32(REG45, DEFAULT);
    VoC_sw32_d(RL7, SPX_KF_BFLY_SCRATCH_0);

    VoC_push32(REG23, DEFAULT);

    // C_MUL(scratch[1] ,*Fout1, tw[tmp]);
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_add16_rr(REG2, REG2, REG5, DEFAULT);
    VoC_NOP();
    VoC_lw32_p(REG01, REG2, DEFAULT);
    VoC_lw32_p(REG67, REG3, DEFAULT);

    VoC_mac32_rr(REG1, REG7, DEFAULT);
    VoC_mac32_rr(REG0, REG7, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);

    VoC_mac32_rr(REG0, REG6, DEFAULT);
    VoC_mac32_rr(REG1, REG6, IN_PARALLEL);
    VoC_NOP();
    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_movreg32(RL7, ACC1, IN_PARALLEL);
    VoC_add32_rd(RL6, RL6, SPEEX_CONST_16384);
    VoC_add32_rd(RL7, RL7, SPEEX_CONST_16384);

    VoC_shr32_ri(RL6, 15, DEFAULT);
    VoC_shr32_ri(RL7, 15, IN_PARALLEL);

    VoC_sw16_d(RL6_LO, SPX_KF_BFLY_SCRATCH_1);
    VoC_sw16_d(RL7_LO, SPX_KF_BFLY_SCRATCH_1i);

    VoC_pop32(REG23, DEFAULT);
    // C_MUL(scratch[4] ,*Fout4, tw[4*tmp]);
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_shr16_ri(REG5, -2, DEFAULT);
    VoC_shr16_ri(REG4, -2, IN_PARALLEL);
    //VoC_push32(REG23, DEFAULT);

    VoC_add16_rr(REG2, REG2, REG5, DEFAULT);
    VoC_add16_rr(REG3, REG3, REG4, IN_PARALLEL);
    VoC_NOP();
    //VoC_add16_rr(REG2, REG2, REG5, DEFAULT);
    VoC_lw32_p(REG01, REG2, DEFAULT);
    VoC_lw32_p(REG67, REG3, DEFAULT);

    VoC_mac32_rr(REG1, REG7, DEFAULT);
    VoC_mac32_rr(REG0, REG7, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);

    VoC_mac32_rr(REG0, REG6, DEFAULT);
    VoC_mac32_rr(REG1, REG6, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, DEFAULT); // scratch[4].r in acc0, scratch[4].i in acc1

    // C_ADD( scratch[7],scratch[1],scratch[4]);
    // C_SUB( scratch[10],scratch[1],scratch[4]);


    VoC_add32_rr(REG01, RL6, ACC0, DEFAULT);
    VoC_add32_rr(REG67, RL7, ACC1, IN_PARALLEL);

    VoC_movreg16(REG1, REG6, DEFAULT);
    VoC_NOP();
    VoC_sw32_d(REG01, SPX_KF_BFLY_SCRATCH_7);

    VoC_sub32_rr(REG01, RL6, ACC0, DEFAULT);
    VoC_sub32_rr(REG67, RL7, ACC1, IN_PARALLEL);

    VoC_movreg16(REG1, REG6, DEFAULT);
    VoC_NOP();
    VoC_sw32_d(REG01, SPX_KF_BFLY_SCRATCH_10);

    // C_MUL(scratch[3] ,*Fout3, tw[3*tmp]);
    //VoC_pop32(REG23, DEFAULT);
    VoC_pop32(REG45, DEFAULT);
    //VoC_push32(REG, DEFAULT);
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_sub16_rr(REG2, REG2, REG5, DEFAULT);
    VoC_sub16_rr(REG3, REG3, REG4, IN_PARALLEL);
    VoC_NOP();
    VoC_lw32_p(REG01, REG2, DEFAULT);
    VoC_lw32_p(REG67, REG3, DEFAULT);

    VoC_mac32_rr(REG1, REG7, DEFAULT);
    VoC_mac32_rr(REG0, REG7, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);

    VoC_mac32_rr(REG0, REG6, DEFAULT);
    VoC_mac32_rr(REG1, REG6, IN_PARALLEL);
    VoC_NOP();
    VoC_movreg32(RL6, ACC0, DEFAULT);
    VoC_movreg32(RL7, ACC1, IN_PARALLEL);
    VoC_add32_rd(RL6, RL6, SPEEX_CONST_16384);
    VoC_add32_rd(RL7, RL7, SPEEX_CONST_16384);

    VoC_shr32_ri(RL6, 15, DEFAULT);
    VoC_shr32_ri(RL7, 15, IN_PARALLEL);

    // C_MUL(scratch[2] ,*Fout2, tw2*tmp]);
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_sub16_rr(REG2, REG2, REG5, DEFAULT);
    VoC_sub16_rr(REG3, REG3, REG4, IN_PARALLEL);
    VoC_NOP();
    VoC_lw32_p(REG01, REG2, DEFAULT);
    VoC_lw32_p(REG67, REG3, DEFAULT);

    VoC_mac32_rr(REG1, REG7, DEFAULT);
    VoC_mac32_rr(REG0, REG7, IN_PARALLEL);
    VoC_NOP();
    VoC_sub32_dr(ACC0, SPEEX_CONST_0, ACC0);

    VoC_mac32_rr(REG0, REG6, DEFAULT);
    VoC_mac32_rr(REG1, REG6, IN_PARALLEL); // scratch[3] in RL67, scratch[2] in ACC01
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, DEFAULT); // scratch[2].r in acc0, scratch[2].i in acc1

    // C_ADD( scratch[8],scratch[2],scratch[3]);
    // C_SUB( scratch[9],scratch[2],scratch[3]);

    VoC_add32_rr(REG01, ACC0, RL6, DEFAULT);
    VoC_add32_rr(REG67, ACC1, RL7, IN_PARALLEL);

    VoC_movreg16(REG1, REG6, DEFAULT);
    VoC_NOP();
    VoC_sw32_d(REG01, SPX_KF_BFLY_SCRATCH_8);

    VoC_sub32_rr(REG01, ACC0, RL6, DEFAULT);
    VoC_sub32_rr(REG67, ACC1, RL7, IN_PARALLEL);

    VoC_movreg16(REG1, REG6, DEFAULT);
    VoC_NOP();
    VoC_sw32_d(REG01, SPX_KF_BFLY_SCRATCH_9);

    // C_ADDTO(*Fout0,scratch[7]);
    // C_ADDTO(*Fout0,scratch[8]);
    VoC_pop32(REG23, DEFAULT);
    VoC_lw16_d(REG6,SPX_KF_BFLY_SCRATCH_7);

    VoC_add16_rp(REG6, REG6, REG2,DEFAULT);

    VoC_add16_rd(REG6, REG6, SPX_KF_BFLY_SCRATCH_8);
    VoC_NOP();
    VoC_sw16_p(REG6, REG2, DEFAULT);
    VoC_add16_rd(REG2, REG2, SPEEX_CONST_1);

    VoC_lw16_d(REG7,SPX_KF_BFLY_SCRATCH_7i);
    VoC_add16_rp(REG7, REG7, REG2,DEFAULT);

    VoC_add16_rd(REG7, REG7, SPX_KF_BFLY_SCRATCH_8i);
    VoC_NOP();
    VoC_sw16_p(REG7, REG2, DEFAULT);

    VoC_push32(REG45, DEFAULT);
    VoC_sub16_rd(REG2, REG2, SPEEX_CONST_1);
    VoC_NOP();
    VoC_push32(REG23, DEFAULT);

    // scratch[5].r = scratch[0].r + S_MUL(scratch[7].r,ya.r) + S_MUL(scratch[8].r,yb.r);
    // scratch[5].i = scratch[0].i + S_MUL(scratch[7].i,ya.r) + S_MUL(scratch[8].i,yb.r);
    VoC_lw32_d(REG01, SPX_KF_BFLY_YA);
    VoC_lw32_d(REG23, SPX_KF_BFLY_YB);
    VoC_lw32_d(REG45, SPX_KF_BFLY_SCRATCH_7);
    VoC_lw32_d(REG67, SPX_KF_BFLY_SCRATCH_8);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG4, REG0, DEFAULT);
    VoC_mac32_rr(REG6, REG2, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);
    VoC_push32(REG67, DEFAULT);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);
    VoC_add32_rr(REG67, ACC0, ACC1, DEFAULT);
    VoC_add16_rd(REG6, REG6, SPX_KF_BFLY_SCRATCH_0);
    VoC_NOP();
    VoC_sw16_d(REG6, SPX_KF_BFLY_SCRATCH_5);
    VoC_pop32(REG67, DEFAULT);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG5, REG0, DEFAULT);
    VoC_mac32_rr(REG7, REG2, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);
    VoC_add32_rr(REG67, ACC0, ACC1, DEFAULT);
    VoC_add16_rd(REG6, REG6, SPX_KF_BFLY_SCRATCH_0i);
    VoC_NOP();
    VoC_sw16_d(REG6, SPX_KF_BFLY_SCRATCH_5i);

    // scratch[6].r =  S_MUL(scratch[10].i,ya.i) + S_MUL(scratch[9].i,yb.i);
    VoC_lw32_d(REG45, SPX_KF_BFLY_SCRATCH_9);
    VoC_lw32_d(REG67, SPX_KF_BFLY_SCRATCH_10);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG7, REG1, DEFAULT);
    VoC_mac32_rr(REG5, REG3, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);
    VoC_push32(REG67, DEFAULT);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);
    VoC_add32_rr(REG67, ACC0, ACC1, DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG6, SPX_KF_BFLY_SCRATCH_6);
    VoC_pop32(REG67, DEFAULT);

    // scratch[6].i = -S_MUL(scratch[10].r,ya.i) - S_MUL(scratch[9].r,yb.i);
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG6, REG1, DEFAULT);
    VoC_mac32_rr(REG4, REG3, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);

    VoC_add32_rr(REG67, ACC0, ACC1, DEFAULT);

    VoC_sub32_dr(REG67, SPEEX_CONST_0, REG67);
    VoC_NOP();
    VoC_sw16_d(REG6, SPX_KF_BFLY_SCRATCH_6i);

    // above working now
    // C_SUB(*Fout1,scratch[5],scratch[6]);
    // C_ADD(*Fout4,scratch[5],scratch[6]);
    VoC_pop32(REG23, DEFAULT);
    VoC_lw32_d(REG45, SPX_KF_BFLY_SCRATCH_5);
    VoC_lw32_d(REG67, SPX_KF_BFLY_SCRATCH_6);
    VoC_sub16_rr(REG0, REG4, REG6, DEFAULT);
    VoC_sub16_rr(REG1, REG5, REG7, IN_PARALLEL);
    VoC_add16_rr(REG6, REG4, REG6, DEFAULT);
    VoC_add16_rr(REG7, REG5, REG7, IN_PARALLEL);
    VoC_pop32(REG45, DEFAULT);
    VoC_NOP();
    VoC_push32(REG45, DEFAULT);
    VoC_push32(REG23, DEFAULT);
    VoC_add16_rr(REG2, REG2, REG5, DEFAULT);
    VoC_NOP();
    VoC_sw32_p(REG01, REG2, DEFAULT);
    VoC_add16_rr(REG2, REG2, REG5, DEFAULT);

    VoC_add16_rr(REG2, REG2, REG5, DEFAULT);

    VoC_add16_rr(REG2, REG2, REG5, DEFAULT);
    VoC_NOP();
    VoC_sw32_p(REG67, REG2, DEFAULT);

    // scratch[11].r = scratch[0].r + S_MUL(scratch[7].r,yb.r) + S_MUL(scratch[8].r,ya.r);
    // scratch[11].i = scratch[0].i + S_MUL(scratch[7].i,yb.r) + S_MUL(scratch[8].i,ya.r);
    VoC_lw32_d(REG01, SPX_KF_BFLY_YA);
    VoC_lw32_d(REG23, SPX_KF_BFLY_YB);
    VoC_lw32_d(REG45, SPX_KF_BFLY_SCRATCH_7);
    VoC_lw32_d(REG67, SPX_KF_BFLY_SCRATCH_8);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG4, REG2, DEFAULT);
    VoC_mac32_rr(REG6, REG0, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);
    VoC_push32(REG67, DEFAULT);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);
    VoC_add32_rr(REG67, ACC0, ACC1, DEFAULT);
    VoC_add16_rd(REG6, REG6, SPX_KF_BFLY_SCRATCH_0);
    VoC_NOP();
    VoC_sw16_d(REG6, SPX_KF_BFLY_SCRATCH_11);
    VoC_pop32(REG67, DEFAULT);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG5, REG2, DEFAULT);
    VoC_mac32_rr(REG7, REG0, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);
    VoC_add32_rr(REG67, ACC0, ACC1, DEFAULT);
    VoC_add16_rd(REG6, REG6, SPX_KF_BFLY_SCRATCH_0i);
    VoC_NOP();
    VoC_sw16_d(REG6, SPX_KF_BFLY_SCRATCH_11i);

    // scratch[12].r = - S_MUL(scratch[10].i,yb.i) + S_MUL(scratch[9].i,ya.i);
    VoC_lw32_d(REG45, SPX_KF_BFLY_SCRATCH_9);
    VoC_lw32_d(REG67, SPX_KF_BFLY_SCRATCH_10);

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG7, REG3, DEFAULT);
    VoC_mac32_rr(REG5, REG1, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);
    VoC_push32(REG67, DEFAULT);
    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);
    VoC_sub32_rr(REG67, ACC1, ACC0, DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG6, SPX_KF_BFLY_SCRATCH_12);
    VoC_pop32(REG67, DEFAULT);


    // scratch[12].i = S_MUL(scratch[10].r,yb.i) - S_MUL(scratch[9].r,ya.i);
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_mac32_rr(REG6, REG3, DEFAULT);
    VoC_mac32_rr(REG4, REG1, IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, SPEEX_CONST_16384);
    VoC_add32_rd(ACC1, ACC1, SPEEX_CONST_16384);

    VoC_shr32_ri(ACC0, 15, DEFAULT);
    VoC_shr32_ri(ACC1, 15, IN_PARALLEL);

    VoC_sub32_rr(REG67, ACC0, ACC1, DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG6, SPX_KF_BFLY_SCRATCH_12i);

    // above works
    // C_SUB(*Fout3,scratch[11],scratch[12]);
    // C_ADD(*Fout2,scratch[11],scratch[12]);
    VoC_pop32(REG23, DEFAULT);
    VoC_lw32_d(REG45, SPX_KF_BFLY_SCRATCH_11);
    VoC_lw32_d(REG67, SPX_KF_BFLY_SCRATCH_12);
    VoC_sub16_rr(REG0, REG4, REG6, DEFAULT);
    VoC_sub16_rr(REG1, REG5, REG7, IN_PARALLEL);
    VoC_add16_rr(REG6, REG4, REG6, DEFAULT);
    VoC_add16_rr(REG7, REG5, REG7, IN_PARALLEL);
    VoC_pop32(REG45, DEFAULT);
    VoC_push32(REG45, DEFAULT);
    VoC_push32(REG23, DEFAULT);
    VoC_add16_rr(REG2, REG2, REG5, DEFAULT);
    VoC_add16_rr(REG2, REG2, REG5, DEFAULT);
    VoC_NOP();
    VoC_sw32_p(REG67, REG2, DEFAULT);
    VoC_add16_rr(REG2, REG2, REG5, DEFAULT);
    VoC_NOP();
    VoC_sw32_p(REG01, REG2, DEFAULT);

    VoC_pop32(REG23, DEFAULT);
    VoC_pop32(REG45, DEFAULT);

    VoC_add16_rd(REG4, REG4, SPX_KF_BFLY_FSTRIDE);
    //VoC_add16_rr(REG2, REG2, REG5, DEFAULT);
    VoC_add16_rd(REG2, REG2, SPEEX_CONST_2);
    VoC_add16_rd(REG4, REG4, SPX_KF_BFLY_FSTRIDE);

    VoC_endloop1

    VoC_pop32(RL7, DEFAULT);
    VoC_pop32(RL6, DEFAULT);
    VoC_pop32(REG67, DEFAULT);
    VoC_pop32(REG45, DEFAULT);
    VoC_pop32(REG23, DEFAULT);
    VoC_pop32(REG01, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/***********************
 * this function does not needed as it changes nothing in memory
 * INPUT :  REG2: kiss_fft_cpx * Fout,
            REG7: const size_t fstride,
            const kiss_fft_cfg st,
            REG5: int m,
            REG4: int p
 * output : none
 **********************/
void spx_kf_bfly_generic(void)
{
    VoC_push16(RA, DEFAULT);

    /*
    VoC_bngt16_rd(SPX_KF_BFLY_GEN_P, REG4, SPEEX_CONST_17)
        VoC_NOP();
    SPX_KF_BFLY_GEN_P:
    VoC_lw16i_short(REG6, 0, DEFAULT);
    SPX_KF_BFLY_BEGINS:
    VoC_be16_rr(SPX_KF_BFLY_END, REG6, REG5)
        VoC_lw16i_set_inc(REG0, SPX_KF_BFLY_SCRATCHBUF, 2);
        VoC_loop_r_short(REG4, DEFAULT);
        VoC_movreg16(REG3, REG6, IN_PARALLEL);
        VoC_startloop0
            VoC_lw32_p(REG67, REG3, DEFAULT);
            VoC_bnez16_d(SPX_KF_BFLY_INVERSE, KFS_substate_inverse)
                VoC_mac32_rd(REG6, SPEEX_CONST_32767);
                VoC_NOP();
                VoC_movreg32(RL6, ACC0, DEFAULT);
                VoC_movreg16(RL7_LO, REG4, DEFAULT);
                VoC_push32(REG01, DEFAULT);
                VoC_jal(speex_div32_16);

                VoC_movreg16(REG6, REG0, DEFAULT);
                VoC_pop32(REG01, DEFAULT);

                VoC_mac32_rd(REG7, SPEEX_CONST_32767);
                VoC_NOP();
                VoC_movreg32(RL6, ACC0, DEFAULT);
                VoC_movreg16(RL7_LO, REG4, DEFAULT);
                VoC_push32(REG01, DEFAULT);
                VoC_jal(speex_div32_16);

                VoC_movreg16(REG7, REG0, DEFAULT);
                VoC_pop32(REG01, DEFAULT);
            VoC_NOP();
            VoC_sw32inc_p(REG67, REG0, DEFAULT);
            VoC_add16_rr(REG3, REG3, REG5, IN_PARALLEL);
        VoC_endloop0

        VoC_loop_r(1, REG4)
            VoC_lw16i_short(REG)
            VoC_loop_r_short(REG4, DEFAULT);
            VoC_startloop0

            VoC_endloop0
            VoC_add16_rr(REG3, REG3, REG5, IN_PARALLEL);
        VoC_endloop1


    VoC_jump(SPX_KF_BFLY_BEGINS);
    SPX_KF_BFLY_END:





    */

    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/****************************
 * inputs : REG2: kiss_fft_cpx * Fout = (kiss_fft_cpx *) st->tmpbuf,
            REG3: const kiss_fft_cpx * f = timedata,
            REG7: const size_t fstride,
            int in_stride = 1,
            REG1: int * factors = st->substate->factors,
            REG0: const kiss_fft_cfg st = st->substate,
            REG6: int N,
            RL7_LO: int s2,
            RL7_HI: int m2
 * output : none
 ***************************/
void spx_kf_work(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push32(REG01, DEFAULT);
    VoC_push32(REG23, DEFAULT);
    VoC_push32(REG45, DEFAULT);
    VoC_push32(REG67, DEFAULT);
    VoC_push32(RL7, DEFAULT);

    // in the first fun, REG4 is set to 1
    VoC_movreg16(RL7_LO, REG7, DEFAULT);
    VoC_movreg16(RL7_HI, REG5, IN_PARALLEL);
    VoC_mult16_rr(REG7, REG7, REG4, DEFAULT);
    VoC_mult16_rr(REG6, REG6, REG4, IN_PARALLEL);

    VoC_lw32inc_p(REG45, REG1, DEFAULT); // p in reg4, m in reg5


    //printf("p: 0x%x\tm: 0x%x\nFout: 0x%x\tf: 0x%x\tfstride: 0x%x\tN: 0x%x\tfstride*in_stride: 0x%x\n",
    //      REGS[4].reg, REGS[5].reg, REGS[2].reg, REGS[3].reg, REGS[7].reg, REGS[6].reg, REGL[3]);
    VoC_be16_rd(SPX_KF_WORK_IF, REG5, SPEEX_CONST_1)
    //VoC_mult16_rr(REG7, REG7, REG4, DEFAULT);
    //VoC_mult16_rr(REG6, REG6, REG4, IN_PARALLEL);
    //VoC_movreg16(RL7_LO, REG7, DEFAULT);
    //VoC_movreg16(RL7_HI, REG5, IN_PARALLEL);
    VoC_jal(spx_kf_work);
SPX_KF_WORK_IF:

    VoC_sw16_d(REG7, SPX_KF_BFLY_FSTRIDE);
    VoC_sw16_d(REG5, SPX_KF_BFLY_M);
    VoC_sw16_d(REG6, SPX_KF_BFLY_N);
    VoC_sw16_d(RL7_HI, SPX_KF_BFLY_MM);
    VoC_bne16_rd(SPX_KF_SWITCH_P2, REG4, SPEEX_CONST_2)
    VoC_jal(spx_kf_bfly2);
    VoC_jump(SPX_KF_SWITCH_END);
SPX_KF_SWITCH_P2:
    VoC_bne16_rd(SPX_KF_SWITCH_P3, REG4, SPEEX_CONST_3)
    VoC_push32(REG45, DEFAULT);
    VoC_lw16i_short(REG5, 0, DEFAULT);
SPX_KF_SWITCH_P2_LOOP_BEGIN:
    VoC_be16_rr(SPX_KF_SWITCH_P2_LOOP_END, REG5, REG6)
    VoC_jal(spx_kf_bfly3);
    VoC_movreg16(REG4, RL7_HI, DEFAULT);

    VoC_add16_rr(REG2, REG2, REG4, DEFAULT);
    VoC_add16_rd(REG5, REG5, SPEEX_CONST_1);
    VoC_jump(SPX_KF_SWITCH_P2_LOOP_BEGIN);
SPX_KF_SWITCH_P2_LOOP_END:
    VoC_pop32(REG45, DEFAULT);
    VoC_jump(SPX_KF_SWITCH_END);
SPX_KF_SWITCH_P3:
    VoC_bne16_rd(SPX_KF_SWITCH_P4, REG4, SPEEX_CONST_4)
    //printf("fstride: 0x%x\tm: 0x%x\tN: 0x%x\tm2: 0x%x\n", REGS[7].reg, REGS[5].reg, REGS[6].reg, REGL[3]);
    VoC_jal(spx_kf_bfly4);
    VoC_jump(SPX_KF_SWITCH_END);
SPX_KF_SWITCH_P4:
    VoC_bne16_rd(SPX_KF_SWITCH_P5, REG4, SPEEX_CONST_5)
    VoC_push32(REG45, DEFAULT);

    VoC_lw16i_short(REG5, 0, DEFAULT);
SPX_KF_SWITCH_P4_LOOP_BEGIN:
    VoC_be16_rr(SPX_KF_SWITCH_P4_LOOP_END, REG5, REG6)
    VoC_jal(spx_kf_bfly5);
    VoC_movreg16(REG4, RL7_HI, DEFAULT);

    VoC_shr16_ri(REG4, -1, DEFAULT);

    VoC_add16_rr(REG2, REG2, REG4, DEFAULT);
    VoC_add16_rd(REG5, REG5, SPEEX_CONST_1);
    VoC_jump(SPX_KF_SWITCH_P4_LOOP_BEGIN);
SPX_KF_SWITCH_P4_LOOP_END:
    VoC_pop32(REG45, DEFAULT);
    VoC_jump(SPX_KF_SWITCH_END);
SPX_KF_SWITCH_P5:
    VoC_push32(REG45, DEFAULT);
    VoC_lw16i_short(REG5, 0, DEFAULT);
SPX_KF_SWITCH_DEFAULT_LOOP_BEGIN:
    VoC_be16_rr(SPX_KF_SWITCH_DEFAULT_LOOP_END, REG5, REG6)
    VoC_jal(spx_kf_bfly_generic);
    VoC_movreg16(REG4, RL7_HI, DEFAULT);

    VoC_add16_rr(REG2, REG2, REG4, DEFAULT);
    VoC_add16_rd(REG5, REG5, SPEEX_CONST_1);

    VoC_jump(SPX_KF_SWITCH_DEFAULT_LOOP_BEGIN);
SPX_KF_SWITCH_DEFAULT_LOOP_END:
    VoC_pop32(REG45, DEFAULT);
SPX_KF_SWITCH_END:

    VoC_pop32(RL7, DEFAULT);
    VoC_pop32(REG67, DEFAULT);
    VoC_pop32(REG45, DEFAULT);
    VoC_pop32(REG23, DEFAULT);
    VoC_pop32(REG01, DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

#endif
/**************************************************************************************
 * Function:    Rda_speex_preprocess_state_reset
 *
 * Description: no
 *
 * Inputs:
 *
 * Outputs:
 *
 *
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       12/23/2013
 **************************************************************************************/
void Rda_speex_preprocess_state_reset(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);

    //clear wrap regs
    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,DEFAULT);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,DEFAULT);

    //clear DMA reg
    VoC_sw16_d(REG0,CFG_DMA_WRAP);

    //clear reset flag
    VoC_sw16_d(REG0,GLABAL_NOISESUPPRESS_RESET_ADDR);

    //clear
    VoC_sw16_d(REG0,SpxPpState_frame_shift);

    //clear status reg
    VoC_lw16i(STATUS,STATUS_CLR);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_set_inc(REG0,SpxPpState_frame_size,2)
    VoC_lw16i(REG6,SPEEX_NN);
    VoC_lw16i(REG7,8000);

    //VoC_sw16_d(ACC0_LO,SPEEX_PRE_FRAME_NUM);

    // 将SpxPpState相关内存清零，
    VoC_loop_i(1,8)
    VoC_loop_i(0,203)
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

    VoC_sw16_d(REG6,SpxPpState_frame_size);
    VoC_sw16_d(REG6,SpxPpState_ps_size);
    VoC_sw16_d(REG7,SpxPpState_sampling_rate);

    VoC_lw16i(REG1,1);
    //VoC_lw16i(REG2,NOISE_SUPPRESS_DEFAULT);
    VoC_lw16_d(REG2, SPEEX_NOISE_SUPPRESS_DEFAULT);
    VoC_lw16i(REG3,ECHO_SUPPRESS_DEFAULT);
    VoC_lw16i(REG4,ECHO_SUPPRESS_ACTIVE_DEFAULT);
    VoC_lw16i(REG5,SPEECH_PROB_START_DEFAULT);
    VoC_lw16i(REG6,SPEECH_PROB_CONTINUE_DEFAULT);
    VoC_lw16i(REG7,SPEEX_NB_BANDS);


    VoC_sw16_d(REG1,SpxPpState_denoise_enabled);
    VoC_sw16_d(REG2,SpxPpState_noise_suppress);
    VoC_sw16_d(REG3,SpxPpState_echo_suppress);
    VoC_sw16_d(REG4,SpxPpState_echo_suppress_active);
    VoC_sw16_d(REG5,SpxPpState_speech_prob_start);
    VoC_sw16_d(REG6,SpxPpState_speech_prob_continue);
    VoC_sw16_d(REG7,SpxPpState_nbands);

    VoC_lw16i(ACC1_LO,1<<NOISE_SHIFT);
    VoC_lw16i(ACC1_HI,0);

    VoC_lw16i_set_inc(REG0,SpxPpState_noise,2)

    VoC_loop_i(0, SPEEX_NN+SPEEX_NB_BANDS)
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i(ACC1_LO,1);
    VoC_lw16i(ACC1_HI,0);

    VoC_lw16i_set_inc(REG0,SpxPpState_old_ps,2)

    VoC_loop_i(0,SPEEX_NN+SPEEX_NB_BANDS)
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i(ACC1_LO,32767);

    VoC_lw16i_set_inc(REG0,SpxPpState_gain,1)

    VoC_loop_i(0,SPEEX_NN+SPEEX_NB_BANDS)
    VoC_sw16inc_p(ACC1_LO,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i(ACC1_LO,(1<<SNR_SHIFT));

    VoC_lw16i_set_inc(REG0,SpxPpState_post,1)

    VoC_loop_i(0,SPEEX_NN+SPEEX_NB_BANDS)
    VoC_sw16inc_p(ACC1_LO,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i(ACC1_LO,(1<<SNR_SHIFT));


    VoC_lw16i_set_inc(REG0,SpxPpState_prior,1)

    VoC_loop_i(0,SPEEX_NN+SPEEX_NB_BANDS)
    VoC_sw16inc_p(ACC1_LO,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i(ACC1_LO,1);

    VoC_lw16i_set_inc(REG0,SpxPpState_update_prob,1)

    VoC_loop_i(0,SPEEX_NN)
    VoC_sw16inc_p(ACC1_LO,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG0, SpxEchoState_last_y, 1)

    VoC_loop_i(0, SPEEX_NN)
    VoC_sw16inc_p(ACC1_LO, REG0, DEFAULT);
    VoC_endloop0
    // echo_state
//  VoC_lw16i(ACC1_LO, 8192);
    // leak_estimate
//  VoC_sw16_d(ACC1_LO, SpxEchoState_leak_estimate);

    //VoC_lw16i(ACC0_LO, 0);
    //VoC_NOP();
    //VoC_sw16_d(ACC0_LO,SPEEX_PRE_RESET);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

