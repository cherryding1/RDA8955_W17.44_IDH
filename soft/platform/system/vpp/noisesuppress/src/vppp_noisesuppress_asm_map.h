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

#ifndef MAP_ADDR_H
#define MAP_ADDR_H

/* This file defines all the variables as memory addresses.
   It's created by voc_map automatically.*/

/********************
 **  SECTION_SIZE  **
 ********************/

#define COMMON_GLOBAL_X_SIZE                                    0
#define COMMON_GLOBAL_Y_SIZE                                    174
#define PREPROCESS_GLOBAL_X_SIZE                                5802
#define PREPROCESS_GLOBAL_Y_SIZE                                0
#define PREPROCESS_LOCAL_X_SIZE                                 0
#define PREPROCESS_LOCAL_Y_SIZE                                 0
#define COMMON_LOCAL_X_SIZE                                     0
#define COMMON_LOCAL_Y_SIZE                                     0

/**************************
 **  SECTION_BEGIN_ADDR  **
 **************************/

#define COMMON_GLOBAL_X_BEGIN_ADDR                              (0 + RAM_X_BEGIN_ADDR)
#define COMMON_GLOBAL_Y_BEGIN_ADDR                              (0 + RAM_Y_BEGIN_ADDR)
#define PREPROCESS_GLOBAL_X_BEGIN_ADDR                          (0 + RAM_X_BEGIN_ADDR)
#define PREPROCESS_GLOBAL_Y_BEGIN_ADDR                          (174 + RAM_Y_BEGIN_ADDR)
#define PREPROCESS_LOCAL_X_BEGIN_ADDR                           (5802 + RAM_X_BEGIN_ADDR)
#define PREPROCESS_LOCAL_Y_BEGIN_ADDR                           (174 + RAM_Y_BEGIN_ADDR)
#define COMMON_LOCAL_X_BEGIN_ADDR                               (5802 + RAM_X_BEGIN_ADDR)
#define COMMON_LOCAL_Y_BEGIN_ADDR                               (174 + RAM_Y_BEGIN_ADDR)

/*********************
 **  COMMON_GLOBAL  **
 *********************/

//GLOBAL_VARS
//VPP_NOISESUPPRESS_CFG_STRUCT
#define VPP_NOISESUPPRESS_CFG_STRUCT                        (0 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_NOISESUPPRESS_CONSTX_ADDR                    (0 + VPP_NOISESUPPRESS_CFG_STRUCT) //1 word
#define SPEEX_NS_Code_ExternalAddr_addr                     (2 + VPP_NOISESUPPRESS_CFG_STRUCT) //1 word
#define GLABAL_OUT_PARA                                     (4 + VPP_NOISESUPPRESS_CFG_STRUCT) //1 word
#define GLOBAL_MIC_NOISE_SUPPRESS_RESET_ADDR                (6 + VPP_NOISESUPPRESS_CFG_STRUCT) //1 short
#define GLOBAL_RECEIVER_NOISE_SUPPRESS_RESET_ADDR           (7 + VPP_NOISESUPPRESS_CFG_STRUCT) //1 short

//VPP_NOISESUPPRESS_STATUS_STRUCT
#define VPP_NOISESUPPRESS_STATUS_STRUCT                     (8 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_USED_MODE                                    (0 + VPP_NOISESUPPRESS_STATUS_STRUCT) //1 short
#define GLOBAL_NOISESUPPRESS_STATUS                         (1 + VPP_NOISESUPPRESS_STATUS_STRUCT) //1 short
#define GLOBAL_OUTSIZE                                      (2 + VPP_NOISESUPPRESS_STATUS_STRUCT) //1 word

#define GLABAL_NOISESUPPRESS_RESET_ADDR                     (12 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLABAL_NOISESUPPRESS_INPUTADDR_ADDR                 (13 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define ENC_INPUT_ADDR_ADDR                                 (14 + COMMON_GLOBAL_Y_BEGIN_ADDR) //160 short

/*************************
 **  PREPROCESS_GLOBAL  **
 *************************/

//GLOBAL_VARS
#define For_noisesuppress_align                             (0 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //816 short
#define SpxPpState_frame_size                               (816 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_ps_size                                  (817 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_sampling_rate                            (818 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_nbands                                   (819 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_denoise_enabled                          (820 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_vad_enabled                              (821 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_dereverb_enabled                         (822 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_reverb_decay                             (823 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_reverb_level                             (824 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_speech_prob_start                        (825 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_speech_prob_continue                     (826 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_noise_suppress                           (827 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_echo_suppress                            (828 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_echo_suppress_active                     (829 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_echo_state_ADDR                          (830 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_speech_prob                              (831 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_frame                                    (832 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_frame_N                                  (912 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_ft                                       (992 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_gain                                     (1152 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_gain_M                                   (1232 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //24 short
#define SpxPpState_prior                                    (1256 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_prior_M                                  (1336 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //24 short
#define SpxPpState_post                                     (1360 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_post_M                                   (1440 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //24 short
#define SpxPpState_gain2                                    (1464 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_gain2_M                                  (1544 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //24 short
#define SpxPpState_gain_floor                               (1568 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_gain_floor_M                             (1648 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //24 short
#define SpxPpState_zeta                                     (1672 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_zeta_M                                   (1752 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //24 short
#define SpxPpState_inbuf                                    (1776 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_outbuf                                   (1856 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_nb_adapt                                 (1936 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_was_speech                               (1937 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_min_count                                (1938 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_frame_shift                              (1939 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_noise                                    (1940 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 word
#define SpxPpState_noise_M                                  (2100 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //24 word
#define SpxPpState_reverb_estimate                          (2148 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //104 word
#define SpxPpState_old_ps                                   (2356 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 word
#define SpxPpState_old_ps_M                                 (2516 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //24 word
#define SpxPpState_ps                                       (2564 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 word
#define SpxPpState_ps_M                                     (2724 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //24 word
#define SpxPpState_S                                        (2772 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 word
#define SpxPpState_Smin                                     (2932 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 word
#define SpxPpState_Stmp                                     (3092 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 word
#define SpxPpState_update_prob                              (3252 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_echo_noise                               (3332 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 word
#define SpxPpState_echo_noise_M                             (3492 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //24 word
#define SpxPpState_residual_echo                            (3540 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //104 word
#define SPEEX_VAR_RL7_ADDR                                  (3748 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_REF_BUF                                       (3750 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define FILTERBANK_NEW_SAMPLING                             (3830 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define FILTERBANK_NEW_TYPE                                 (3831 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define FILTERBANK_DF                                       (3832 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define FILTERBANK_MAX_MEL                                  (3834 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define FILTERBANK_MEL_INTERVAL                             (3836 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_ST_WINDOW                                     (3838 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define CONJ_WINDOW_LEN                                     (3998 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPEEX_PRE_RETURN                                    (3999 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define COMPUTE_GAIN_FLOOR_NOISE_ECHO_GAIN                  (4000 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPX_KF_BFLY3_EPI3                                   (4002 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPX_KF_BFLY_SCRATCH                                 (4004 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_0                               (SPX_KF_BFLY_SCRATCH) //alias
#define SPX_KF_BFLY_SCRATCH_0i                              (4005 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_1                               (4006 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_1i                              (4007 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_2                               (4008 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_2i                              (4009 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_3                               (4010 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_3i                              (4011 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_4                               (4012 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_4i                              (4013 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_5                               (4014 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_5i                              (4015 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_6                               (4016 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_6i                              (4017 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_7                               (4018 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_7i                              (4019 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_8                               (4020 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_8i                              (4021 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_9                               (4022 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_9i                              (4023 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_10                              (4024 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_10i                             (4025 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_11                              (4026 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_11i                             (4027 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_12                              (4028 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_12i                             (4029 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_YA                                      (4030 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_YAi                                     (4031 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_YB                                      (4032 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_YBi                                     (4033 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_FSTRIDE                                 (4034 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_M                                       (4035 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_N                                       (4036 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_MM                                      (4037 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_TMP                                     (4038 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPEEX_PRE_RESET                                     (4039 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPEEX_PRE_FRAME_NUM                                 (4040 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPEEX_RESERVR_FOR_ALIGN                             (4041 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //7 short
#define SpxPpState_window                                   (4048 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_bank_bank_left                           (4208 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_bank_bank_right                          (4288 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_bank_filter_left                         (4368 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_bank_filter_right                        (4448 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_bank_nb_banks                            (4528 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_bank_len                                 (4529 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define FILTERBANK_NB_BANKS                                 (SpxPpState_bank_nb_banks) //alias
#define FILTERBANK_LEN                                      (SpxPpState_bank_len) //alias
#define SpxPpState_fft_lookup_forward_substate_nfft         (4530 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_fft_lookup                               (SpxPpState_fft_lookup_forward_substate_nfft) //alias
#define SpxPpState_fft_lookup_forward_substate              (SpxPpState_fft_lookup_forward_substate_nfft) //alias
#define SpxPpState_fft_lookup_forward_substate_inverse          (4531 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_fft_lookup_forward_substate_factors          (4532 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //64 short
#define SpxPpState_fft_lookup_forward_substate_twiddles         (4596 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_fft_lookup_forward_tmpbuf                (4756 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_fft_lookup_forward_super_twiddles            (4916 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_fft_lookup_backward_substate_nfft            (5076 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_fft_lookup_backward_substate             (SpxPpState_fft_lookup_backward_substate_nfft) //alias
#define SpxPpState_fft_lookup_backward_substate_inverse         (5077 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_fft_lookup_backward_substate_factors         (5078 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //64 short
#define SpxPpState_fft_lookup_backward_substate_twiddles            (5142 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_fft_lookup_backward_tmpbuf               (5302 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_fft_lookup_backward_super_twiddles           (5462 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_fft_lookup_N                             (5622 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_fft_lookup_reserved                      (5623 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPEEX_ATAN01_M                                      (5624 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //4 word
#define SPEEX_COS_PI_2_L                                    (5632 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //4 word
#define SPEEX_SQRT_C                                        (5640 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //4 short
#define SPEEX_UPDATE_NOISE_X                                (5644 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //4 short
#define SPEEX_HYPERGEOM_GAIN_TABLE                          (5648 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //22 short
#define SPEEX_EXP_D                                         (5670 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //4 short
#define SPEEX_CONST_0                                       (5674 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_1                                       (5676 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_2                                       (5678 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_3                                       (5680 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_4                                       (5682 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_5                                       (5684 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_6                                       (5686 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_7                                       (5688 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_8                                       (5690 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_14                                      (5692 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_16                                      (5694 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_17                                      (5696 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_19                                      (5698 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_20                                      (5700 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_29                                      (5702 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_64                                      (5704 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_97                                      (5706 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_100                                     (5708 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_128                                     (5710 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_236                                     (5712 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_256                                     (5714 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_472                                     (5716 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_512                                     (5718 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_983                                     (5720 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_1000                                    (5722 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_2458                                    (5724 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_3277                                    (5726 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_4915                                    (5728 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_6521                                    (5730 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_6711                                    (5732 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_8192                                    (5734 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_9830                                    (5736 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_10000                                   (5738 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_10912                                   (5740 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_16384                                   (5742 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_19458                                   (5744 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_19661                                   (5746 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_20000                                   (5748 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_20839                                   (5750 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_21290                                   (5752 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_21924                                   (5754 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_22938                                   (5756 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_23637                                   (5758 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_24576                                   (5760 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_25736                                   (5762 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_26214                                   (5764 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_29164                                   (5766 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_29458                                   (5768 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_32000                                   (5770 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_32766                                   (5772 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_32767                                   (5774 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_32768                                   (5776 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_65535                                   (5778 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_65536                                   (5780 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_131071                                  (5782 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_131072                                  (5784 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_0x80000                                 (5786 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_1087164                                 (5788 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_0x800000                                (5790 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_0x7fffffff                              (5792 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_NEG21290                                (5794 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_NEG15                                   (5796 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_NEG2                                    (5798 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_NEG32767                                (5800 + PREPROCESS_GLOBAL_X_BEGIN_ADDR) //1 word

/************************
 **  PREPROCESS_LOCAL  **
 ************************/

/********************
 **  COMMON_LOCAL  **
 ********************/

//RAM_X: size 0x2800, used 0x16aa   RAM_Y: size 0x2800, used 0x00ae

#endif
