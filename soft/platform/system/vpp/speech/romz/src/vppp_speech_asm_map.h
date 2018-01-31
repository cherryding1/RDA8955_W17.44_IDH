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

#define COMMON_GLOBAL_X_SIZE                                    816
#define COMMON_GLOBAL_Y_SIZE                                    772
#define FR_GLOBAL_X_SIZE                                        1078
#define FR_GLOBAL_Y_SIZE                                        390
#define FR_LOCAL_X_SIZE                                         790
#define FR_LOCAL_Y_SIZE                                         468
#define HR_GLOBAL_X_SIZE                                        2372
#define HR_GLOBAL_Y_SIZE                                        124
#define HR_LOCAL_X_SIZE                                         804
#define HR_LOCAL_Y_SIZE                                         1020
#define AMR_EFR_GLOBAL_X_SIZE                                   2366
#define AMR_EFR_GLOBAL_Y_SIZE                                   230
#define AMR_EFR_LOCAL_X_SIZE                                    820
#define AMR_EFR_LOCAL_Y_SIZE                                    1022
#define COMMON_LOCAL_X_SIZE                                     0
#define COMMON_LOCAL_Y_SIZE                                     0

/**************************
 **  SECTION_BEGIN_ADDR  **
 **************************/

#define COMMON_GLOBAL_X_BEGIN_ADDR                              (0 + RAM_X_BEGIN_ADDR)
#define COMMON_GLOBAL_Y_BEGIN_ADDR                              (0 + RAM_Y_BEGIN_ADDR)
#define FR_GLOBAL_X_BEGIN_ADDR                                  (816 + RAM_X_BEGIN_ADDR)
#define FR_GLOBAL_Y_BEGIN_ADDR                                  (772 + RAM_Y_BEGIN_ADDR)
#define FR_LOCAL_X_BEGIN_ADDR                                   (1894 + RAM_X_BEGIN_ADDR)
#define FR_LOCAL_Y_BEGIN_ADDR                                   (1162 + RAM_Y_BEGIN_ADDR)
#define HR_GLOBAL_X_BEGIN_ADDR                                  (816 + RAM_X_BEGIN_ADDR)
#define HR_GLOBAL_Y_BEGIN_ADDR                                  (772 + RAM_Y_BEGIN_ADDR)
#define HR_LOCAL_X_BEGIN_ADDR                                   (3188 + RAM_X_BEGIN_ADDR)
#define HR_LOCAL_Y_BEGIN_ADDR                                   (896 + RAM_Y_BEGIN_ADDR)
#define AMR_EFR_GLOBAL_X_BEGIN_ADDR                             (816 + RAM_X_BEGIN_ADDR)
#define AMR_EFR_GLOBAL_Y_BEGIN_ADDR                             (772 + RAM_Y_BEGIN_ADDR)
#define AMR_EFR_LOCAL_X_BEGIN_ADDR                              (3182 + RAM_X_BEGIN_ADDR)
#define AMR_EFR_LOCAL_Y_BEGIN_ADDR                              (1002 + RAM_Y_BEGIN_ADDR)
#define COMMON_LOCAL_X_BEGIN_ADDR                               (3188 + RAM_X_BEGIN_ADDR)
#define COMMON_LOCAL_Y_BEGIN_ADDR                               (1162 + RAM_Y_BEGIN_ADDR)

/*********************
 **  COMMON_GLOBAL  **
 *********************/

//GLOBAL_VARS
//VPP_SPEECH_DEC_IN_STRUCT
#define VPP_SPEECH_DEC_IN_STRUCT                            (0 + COMMON_GLOBAL_X_BEGIN_ADDR)
#define INPUT_BITS2_ADDR                                    (0 + VPP_SPEECH_DEC_IN_STRUCT) //26 short

//VPP_SPEECH_ENC_OUT_STRUCT
#define VPP_SPEECH_ENC_OUT_STRUCT                           (26 + COMMON_GLOBAL_X_BEGIN_ADDR)
#define OUTPUT_BITS2_ADDR                                   (0 + VPP_SPEECH_ENC_OUT_STRUCT) //22 short

//VPP_SPEECH_ENC_IN_STRUCT
#define VPP_SPEECH_ENC_IN_STRUCT                            (48 + COMMON_GLOBAL_X_BEGIN_ADDR)
#define INPUT_SPEECH_BUFFER2_ADDR                           (0 + VPP_SPEECH_ENC_IN_STRUCT) //160 short
#define INPUT_SPEECH_BUFFER1_ADDR                           (160 + VPP_SPEECH_ENC_IN_STRUCT) //160 short

//VPP_SPEECH_DEC_OUT_STRUCT
#define VPP_SPEECH_DEC_OUT_STRUCT                           (368 + COMMON_GLOBAL_X_BEGIN_ADDR)
#define OUTPUT_SPEECH_BUFFER2_ADDR                          (0 + VPP_SPEECH_DEC_OUT_STRUCT) //160 short
#define OUTPUT_SPEECH_BUFFER1_ADDR                          (160 + VPP_SPEECH_DEC_OUT_STRUCT) //160 short

#define GLOBAL_SDF_SAMPLS_ADDR                              (688 + COMMON_GLOBAL_X_BEGIN_ADDR) //64 short
#define GLOBAL_MDF_SAMPLS_ADDR                              (752 + COMMON_GLOBAL_X_BEGIN_ADDR) //64 short

//GLOBAL_VARS
//VPP_SPEECH_INIT_STRUCT
#define VPP_SPEECH_INIT_STRUCT                              (0 + COMMON_GLOBAL_Y_BEGIN_ADDR)
#define GLOBAL_LOADED_VOCODER_ADDR                          (0 + VPP_SPEECH_INIT_STRUCT) //1 short
#define GLOBAL_LOADED_AMR_EFR_ADDR                          (1 + VPP_SPEECH_INIT_STRUCT) //1 short
#define GLOBAL_LOADED_AMR_ENC_MODE_ADDR                     (2 + VPP_SPEECH_INIT_STRUCT) //1 short
#define GLOBAL_LOADED_AMR_DEC_MODE_ADDR                     (3 + VPP_SPEECH_INIT_STRUCT) //1 short
#define GLOBAL_VOCODER_SOURCE_CODE_ADDR_ADDR                (4 + VPP_SPEECH_INIT_STRUCT) //6 short
#define GLOBAL_AMR_EFR_SOURCE_CODE_ADDR_ADDR                (10 + VPP_SPEECH_INIT_STRUCT) //4 short
#define GLOBAL_AMR_ENC_MODE_SOURCE_CODE_ADDR_ADDR           (14 + VPP_SPEECH_INIT_STRUCT) //16 short
#define GLOBAL_AMR_DEC_MODE_SOURCE_CODE_ADDR_ADDR           (30 + VPP_SPEECH_INIT_STRUCT) //16 short
#define GLOBAL_RESET_ADDR                                   (46 + VPP_SPEECH_INIT_STRUCT) //1 short
#define GLOBAL_IRQGEN_ADDR                                  (47 + VPP_SPEECH_INIT_STRUCT) //1 short

//VPP_SPEECH_AUDIO_CFG_STRUCT
#define VPP_SPEECH_AUDIO_CFG_STRUCT                         (48 + COMMON_GLOBAL_Y_BEGIN_ADDR)
#define GLOBAL_ECHO_CANCEL_ON                               (0 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_ECHO_CANCEL_EXP_MU                           (1 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_ECHO_CANCEL_EXP_REL_MIN                      (2 + VPP_SPEECH_AUDIO_CFG_STRUCT) //2 short
#define GLOBAL_EchoRstThr_ADDR                              (4 + VPP_SPEECH_AUDIO_CFG_STRUCT) //2 short
#define GLOBAL_ENC_MICGAIN_ADDR                             (6 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_DEC_SPKGAIN_ADDR                             (7 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_SDF_ADDR                                     (8 + VPP_SPEECH_AUDIO_CFG_STRUCT) //2 short
#define GLOBAL_MDF_ADDR                                     (10 + VPP_SPEECH_AUDIO_CFG_STRUCT) //2 short

#define GLOBAL_RELOAD_MODE_ADDR                             (60 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_AMR_EFR_RESET_ADDR                           (61 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_OLD_MICGAIN_ADDR                             (62 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_OLD_SPKGAIN_ADDR                             (63 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_SDF_COEFFS_ADDR                              (64 + COMMON_GLOBAL_Y_BEGIN_ADDR) //64 short
#define GLOBAL_BFI_ADDR                                     (128 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_UFI_ADDR                                     (129 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_SID_ADDR                                     (130 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_TAF_ADDR                                     (131 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DEC_MODE_ADDR                                (132 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ENC_MODE_ADDR                                (133 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ENC_USED_MODE_ADDR                           (134 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_EFR_FLAG_ADDR                                (135 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_RESET_FLAG_ADDR                                 (136 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_MAIN_RESET_FLAG_OLD_ADDR                        (137 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_MDF_COEFFS_ADDR                              (138 + COMMON_GLOBAL_Y_BEGIN_ADDR) //64 short
#define GLOBAL_ECHO_CANCEL_COR_PWR                          (202 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_ECHO_CANCEL_ENC_PWR                          (204 + COMMON_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_ECHO_CANCEL_DEC_PWR                          (208 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_ECHO_CANCEL_SAMPLS_ADDR                      (210 + COMMON_GLOBAL_Y_BEGIN_ADDR) //296 short
#define GLOBAL_ECHO_CANCEL_COEFFS_ADDR                      (506 + COMMON_GLOBAL_Y_BEGIN_ADDR) //256 short
#define GLOBAL_VOCODER_RESET_ADDR                           (762 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_OUTPUT_Vad_ADDR                              (763 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_OUTPUT_SP_ADDR                               (764 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_AMR_FRAME_TYPE_ADDR                             (765 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define ENC_INPUT_ADDR_ADDR                                 (766 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_OUTPUT_ADDR_ADDR                                (767 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define ENC_INPUT_ADDR_BAK_ADDR                             (768 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_OUTPUT_ADDR_BAK_ADDR                            (769 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DTX_ADDR                                     (770 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Reserved_ADDR                                       (771 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short

/*****************
 **  FR_GLOBAL  **
 *****************/

//GLOBAL_VARS
#define STATIC_ENC_GSM_STATE_ADDR                           (0 + FR_GLOBAL_X_BEGIN_ADDR) //280 short
#define STATIC_ENC_GSM_STATE_Z1_ADDR                        (280 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_ENC_GSM_STATE_L_Z2_ADDR                      (282 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_ENC_GSM_STATE_MP_ADDR                        (284 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_ENC_GSM_STATE_U_ADDR                         (286 + FR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STATIC_ENC_GSM_STATE_LARPP_ADDR                     (294 + FR_GLOBAL_X_BEGIN_ADDR) //16 short
#define STATIC_ENC_GSM_STATE_J_ADDR                         (310 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_ENC_GSM_STATE_NRP_ADDR                       (311 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_ENC_GSM_STATE_V_ADDR                         (312 + FR_GLOBAL_X_BEGIN_ADDR) //9 short
#define STATIC_ENC_GSM_STATE_MSR_ADDR                       (321 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_ENC_GSM_STATE_VERBOSE_ADDR                   (322 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_ENC_GSM_STATE_FAST_ADDR                      (323 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_ENC_LARC_ADDR                                (324 + FR_GLOBAL_X_BEGIN_ADDR) //8 short
#define GLOBAL_ENC_NC_ADDR                                  (332 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_ENC_MC_ADDR                                  (336 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_ENC_BC_ADDR                                  (340 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_ENC_XMAXC_ADDR                               (344 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_ENC_XMC_ADDR                                 (348 + FR_GLOBAL_X_BEGIN_ADDR) //52 short
#define STATIC_DEC_GSM_STATE_DP0_ADDR                       (400 + FR_GLOBAL_X_BEGIN_ADDR) //280 short
#define STATIC_DEC_GSM_STATE_Z1_ADDR                        (680 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_DEC_GSM_STATE_L_Z2_ADDR                      (682 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_DEC_GSM_STATE_MP_ADDR                        (684 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_DEC_GSM_STATE_U_ADDR                         (686 + FR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STATIC_DEC_GSM_STATE_LARPP_ADDR                     (694 + FR_GLOBAL_X_BEGIN_ADDR) //16 short
#define STATIC_DEC_GSM_STATE_J_ADDR                         (710 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DEC_GSM_STATE_NRP_ADDR                       (711 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DEC_GSM_STATE_V_ADDR                         (712 + FR_GLOBAL_X_BEGIN_ADDR) //9 short
#define STATIC_DEC_GSM_STATE_MSR_ADDR                       (721 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DEC_GSM_STATE_VERBOSE_ADDR                   (722 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DEC_GSM_STATE_FAST_ADDR                      (723 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DEC_LARC_ADDR                                (724 + FR_GLOBAL_X_BEGIN_ADDR) //8 short
#define GLOBAL_DEC_NC_ADDR                                  (732 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DEC_MC_ADDR                                  (736 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DEC_BC_ADDR                                  (740 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DEC_XMAXC_ADDR                               (744 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DEC_XMC_ADDR                                 (748 + FR_GLOBAL_X_BEGIN_ADDR) //52 short
#define STATIC_VAD_pswRvad_ADDR                             (800 + FR_GLOBAL_X_BEGIN_ADDR) //9 short
#define STATIC_VAD_tone_ADDR                                (809 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swNormRvad_ADDR                          (810 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swPt_sacf_ADDR                           (811 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swPt_sav0_ADDR                           (812 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swE_thvad_ADDR                           (813 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swM_thvad_ADDR                           (814 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swAdaptCount_ADDR                        (815 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swBurstCount_ADDR                        (816 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swHangCount_ADDR                         (817 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swOldLagCount_ADDR                       (818 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swVeryOldLagCount_ADDR                   (819 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swOldLag_ADDR                            (820 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_VAD_pL_sacf_ADDR                             (822 + FR_GLOBAL_X_BEGIN_ADDR) //54 short
#define STATIC_VAD_pL_sav0_ADDR                             (876 + FR_GLOBAL_X_BEGIN_ADDR) //72 short
#define STATIC_VAD_L_lastdm_ADDR                            (948 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_VAD_SCALVAD_ADDR                             (950 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STATIC_CONST_GSM_DLB_ADDR                           (954 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STATIC_CONST_GSM_QLB_ADDR                           (958 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STATIC_CONST_GSM_NRFAC_ADDR                         (962 + FR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STATIC_CONST_GSM_FAC_ADDR                           (970 + FR_GLOBAL_X_BEGIN_ADDR) //8 short
#define DEC_HOMING_LPC_PARM_ADDR                            (978 + FR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STATIC_CONST_GSM_H_ADDR                             (986 + FR_GLOBAL_X_BEGIN_ADDR) //12 short
#define STATIC_TABLE_HANN_ADDR                              (998 + FR_GLOBAL_X_BEGIN_ADDR) //80 short

//GLOBAL_VARS
#define STATIC_ENC_E_ADDR                                   (0 + FR_GLOBAL_Y_BEGIN_ADDR) //50 short
#define GLOBAL_dtx_begin_BLANK                              (50 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_subfrm_ADDR                                  (51 + FR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_txdtx_ctrl_ADDR                              (53 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_txdtx_hangover_ADDR                          (54 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_txdtx_N_elapsed_ADDR                         (55 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_lost_speech_frames_ADDR                      (56 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_rxdtx_aver_period_ADDR                       (57 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_rxdtx_N_elapsed_ADDR                         (58 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_rxdtx_ctrl_ADDR                              (59 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_rx_dtx_state_ADDR                            (60 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_prev_SID_frames_lost_ADDR                    (61 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_L_pn_seed_rx_ADDR                            (62 + FR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_LAR_SID_tx_ADDR                              (70 + FR_GLOBAL_Y_BEGIN_ADDR) //32 short
#define GLOBAL_xmax_SID_tx_ADDR                             (102 + FR_GLOBAL_Y_BEGIN_ADDR) //16 short
#define GLOBAL_LARc_old_tx_ADDR                             (118 + FR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_xmaxc_old_tx_ADDR                            (126 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_LARc_old_rx_ADDR                             (130 + FR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_Nc_old_rx_ADDR                               (138 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_Mc_old_rx_ADDR                               (142 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_bc_old_rx_ADDR                               (146 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_xmaxc_old_rx_ADDR                            (150 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_xmc_old_rx_ADDR                              (154 + FR_GLOBAL_Y_BEGIN_ADDR) //52 short
#define GLOBAL_LARc_old_2_rx_ADDR                           (206 + FR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_xmaxc_old_2_rx_ADDR                          (214 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_LARc_old_SID_rx_ADDR                         (218 + FR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_LARc_old_2_SID_rx_ADDR                       (226 + FR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_xmaxc_old_SID_rx_ADDR                        (234 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_xmaxc_old_2_SID_rx_ADDR                      (238 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define VAD_ALGORITHM_swStat_ADDR                           (242 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define VAD_ALGORITHM_swNormRav1_ADDR                       (243 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define VAD_ALGORITHM_pswRav1_ADDR                          (244 + FR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define VAD_ALGORITHM_pL_av0_ADDR                           (254 + FR_GLOBAL_Y_BEGIN_ADDR) //18 short
#define VAD_ALGORITHM_pL_av1_ADDR                           (272 + FR_GLOBAL_Y_BEGIN_ADDR) //18 short
#define VAD_ALGORITHM_swM_pvad_ADDR                         (290 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define VAD_ALGORITHM_swE_pvad_ADDR                         (291 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define VAD_ALGORITHM_swM_acf0_ADDR                         (292 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define VAD_ALGORITHM_swE_acf0_ADDR                         (293 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define VAD_PREDICTOR_VALUES_pswAav1_ADDR                   (294 + FR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define VAD_PREDICTOR_VALUES_pswVpar_ADDR                   (304 + FR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define VAD_STEP_UP_pL_coef_ADDR                            (312 + FR_GLOBAL_Y_BEGIN_ADDR) //18 short
#define VAD_STEP_UP_pL_work_ADDR                            (330 + FR_GLOBAL_Y_BEGIN_ADDR) //18 short
#define VAD_COMPUTE_RAV1_pL_work_ADDR                       (348 + FR_GLOBAL_Y_BEGIN_ADDR) //18 short
#define VAD_TONE_DETECTION_pswA_ADDR                        (366 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define SCHUR_RECURSION_pswPp_ADDR                          (370 + FR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define SCHUR_RECURSION_pswKk_ADDR                          (380 + FR_GLOBAL_Y_BEGIN_ADDR) //10 short

/****************
 **  FR_LOCAL  **
 ****************/

//CII_Gsm_Coder
#define CII_Gsm_Coder_X_BEGIN_ADDR                          (0 + FR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_ENC_SO_ADDR                                   (0 + CII_Gsm_Coder_X_BEGIN_ADDR) //160 short
#define LOCAL_ENC_L_ACF_ADDR                                (160 + CII_Gsm_Coder_X_BEGIN_ADDR) //18 short
#define LOCAL_ENC_X_ADDR                                    (178 + CII_Gsm_Coder_X_BEGIN_ADDR) //40 short
#define LOCAL_ENC_XM_ADDR                                   (218 + CII_Gsm_Coder_X_BEGIN_ADDR) //14 short
#define LOCAL_ENC_XMP_ADDR                                  (232 + CII_Gsm_Coder_X_BEGIN_ADDR) //14 short
#define LOCAL_DEC_XMP_ADDR                                  (246 + CII_Gsm_Coder_X_BEGIN_ADDR) //14 short
#define LOCAL_ENC_ACF_ADDR                                  (260 + CII_Gsm_Coder_X_BEGIN_ADDR) //10 short
#define LOCAL_ENC_P_ADDR                                    (270 + CII_Gsm_Coder_X_BEGIN_ADDR) //10 short
#define LOCAL_ENC_K_ADDR                                    (280 + CII_Gsm_Coder_X_BEGIN_ADDR) //10 short

//CII_Gsm_Coder
#define CII_Gsm_Coder_Y_BEGIN_ADDR                          (0 + FR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_ENC_WT_ADDR                                   (0 + CII_Gsm_Coder_Y_BEGIN_ADDR) //40 short
#define LOCAL_ENC_LARP_ADDR                                 (40 + CII_Gsm_Coder_Y_BEGIN_ADDR) //100 short

//CII_vad_computation_fr
#define CII_vad_computation_fr_X_BEGIN_ADDR                 (290 + FR_LOCAL_X_BEGIN_ADDR)
#define BIT_PARA_TEMP_ADDR                                  (0 + CII_vad_computation_fr_X_BEGIN_ADDR) //500 short

//CII_Gsm_Decoder
#define CII_Gsm_Decoder_X_BEGIN_ADDR                        (0 + FR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_DEC_WT_ADDR                                   (0 + CII_Gsm_Decoder_X_BEGIN_ADDR) //160 short
#define LOCAL_DEC_ERP_ADDR                                  (160 + CII_Gsm_Decoder_X_BEGIN_ADDR) //40 short
#define LOCAL_VAD_L_ACFH_ADDR                               (200 + CII_Gsm_Decoder_X_BEGIN_ADDR) //10 short
#define LOCAL_VAD_SAV0_ADDR                                 (210 + CII_Gsm_Decoder_X_BEGIN_ADDR) //100 short

//CII_Gsm_Decoder
#define CII_Gsm_Decoder_Y_BEGIN_ADDR                        (0 + FR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_DEC_LARP_ADDR                                 (0 + CII_Gsm_Decoder_Y_BEGIN_ADDR) //196 short
#define LOCAL_SOF_Y_ADDR                                    (196 + CII_Gsm_Decoder_Y_BEGIN_ADDR) //160 short
#define LOCAL_VAD_SACF_Y_ADDR                               (356 + CII_Gsm_Decoder_Y_BEGIN_ADDR) //6 short
#define LOCAL_VAD_K_Y_ADDR                                  (362 + CII_Gsm_Decoder_Y_BEGIN_ADDR) //6 short
#define LOCAL_VAD_RC_Y_ADDR                                 (368 + CII_Gsm_Decoder_Y_BEGIN_ADDR) //100 short

/*****************
 **  HR_GLOBAL  **
 *****************/

//GLOBAL_VARS
#define HR_RESERVED1                                        (0 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_GLOBAL_BEGIN                                     (HR_RESERVED1)
#define GLOBAL_DTX_swVadFrmCnt_ADDR                         (1 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DTX_siUpdPointer_ADDR                        (2 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DTX_swNElapsed_ADDR                          (3 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DTX_pL_GsHist_ADDR                           (4 + HR_GLOBAL_X_BEGIN_ADDR) //56 short
#define STATIC_DTX_pL_R0Hist_ADDR                           (60 + HR_GLOBAL_X_BEGIN_ADDR) //16 short
#define STATIC_DTX_ppL_CorrHist_ADDR                        (76 + HR_GLOBAL_X_BEGIN_ADDR) //176 short
#define STATIC_DTX_swQntRefR0_ADDR                          (252 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DTX_swRefGsIndex_ADDR                        (253 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DTX_piRefVqCodewds_ADDR                      (254 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STATIC_DTX_swShortBurst_ADDR                        (258 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define DECODER_resetflag_ADDR                              (259 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DTX_L_TxPNSeed_ADDR                          (260 + HR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_DTX_swR0OldCN_ADDR                           (262 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_RESERVED2                                        (263 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DTX_pL_OldCorrSeq_ADDR                       (264 + HR_GLOBAL_X_BEGIN_ADDR) //22 short
#define STATIC_DTX_pL_NewCorrSeq_ADDR                       (286 + HR_GLOBAL_X_BEGIN_ADDR) //22 short
#define STATIC_DTX_pL_CorrSeq_ADDR                          (308 + HR_GLOBAL_X_BEGIN_ADDR) //22 short
#define GLOBAL_ERR_plSubfrEnergyMem_ADDR                    (330 + HR_GLOBAL_X_BEGIN_ADDR) //8 short
#define GLOBAL_ERR_swLevelMem_ADDR                          (338 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_ERR_lastR0_ADDR                              (342 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define DECODER_resetflag_old_ADDR                          (343 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_ERR_pswLastGood_ADDR                         (344 + HR_GLOBAL_X_BEGIN_ADDR) //18 short
#define GLOBAL_ERR_swState_ADDR                             (362 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_ERR_swLastFlag_ADDR                          (363 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_RESERVED5                                        (364 + HR_GLOBAL_X_BEGIN_ADDR) //3 short
#define GLOBAL_DE_giSfrmCnt_ADDR                            (367 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DE_pswSpeechPara_ADDR                        (368 + HR_GLOBAL_X_BEGIN_ADDR) //22 short
#define STATIC_DE_pswDecodedSpeechFrame_ADDR                (390 + HR_GLOBAL_X_BEGIN_ADDR) //160 short
#define GLOBAL_swPreBfi_ADDR                                (550 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_swOldR0_ADDR                                 (551 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_EN_pswSpeechPara_ADDR                        (552 + HR_GLOBAL_X_BEGIN_ADDR) //20 short
#define STATIC_EN_pswSpeechBuff_ADDR                        (572 + HR_GLOBAL_X_BEGIN_ADDR) //160 short
#define GLOBAL_DE_gswPostFiltAgcGain_ADDR                   (732 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_sfrmAnalysis_siPrevLagCode_ADDR               (733 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_gpswPostFiltStateNum_ADDR                 (734 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_DE_gpswPostFiltStateDenom_ADDR               (744 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_DE_swPostEmphasisState_ADDR                  (754 + HR_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_DE_pswSynthFiltState_ADDR                    (756 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_DE_pswOldFrmPFDenom_ADDR                     (766 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_DE_pswOldFrmKsDec_ADDR                       (776 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_DE_pswOldFrmAsDec_ADDR                       (786 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_DE_pswOldFrmPFNum_ADDR                       (796 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_DE_swOldR0Dec_ADDR                           (806 + HR_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_DE_pswLtpStateBaseDec_ADDR                   (808 + HR_GLOBAL_X_BEGIN_ADDR) //188 short
#define GLOBAL_DE_pswPPreState_ADDR                         (996 + HR_GLOBAL_X_BEGIN_ADDR) //188 short
#define GLOBAL_DE_swMuteFlagOld_ADDR                        (1184 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DTX_DE_swRxDTXState_ADDR                     (1185 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_swDecoMode_ADDR                           (1186 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_swDtxMuting_ADDR                          (1187 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_swDtxBfiCnt_ADDR                          (1188 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_swOldR0IndexDec_ADDR                      (1189 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_swRxGsHistPtr_ADDR                        (1190 + HR_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_DE_pL_RxGsHist_ADDR                          (1192 + HR_GLOBAL_X_BEGIN_ADDR) //56 short
#define GLOBAL_DE_SP_swR0Dec_ADDR                           (1248 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_SP_swVoicingMode_ADDR                     (1249 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_SP_pswVq_ADDR                             (1250 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DE_SP_swSi_ADDR                              (1254 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_SP_swEngyRShift_ADDR                      (1255 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_SP_swR0NewCN_ADDR                         (1256 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DE_SP_swLastLag_ADDR                         (1257 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_EN_swOldR0_ADDR                              (1258 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_EN_swOldR0Index_ADDR                         (1259 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_EN_psnsWSfrmEngSpace_ADDR                    (1260 + HR_GLOBAL_X_BEGIN_ADDR) //16 short
#define GLOBAL_EN_pswHPFXState_ADDR                         (1276 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_EN_pswHPFYState_ADDR                         (1280 + HR_GLOBAL_X_BEGIN_ADDR) //8 short
#define GLOBAL_EN_pswOldFrmKs_ADDR                          (1288 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_EN_pswOldFrmAs_ADDR                          (1298 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_EN_pswOldFrmSNWCoefs_ADDR                    (1308 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_EN_pswWgtSpeechSpace_ADDR                    (1318 + HR_GLOBAL_X_BEGIN_ADDR) //306 short
#define GLOBAL_EN_pswSpeech_ADDR                            (1624 + HR_GLOBAL_X_BEGIN_ADDR) //196 short
#define GLOBAL_EN_swPtch_ADDR                               (1820 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DTX_TX_swTxGsHistPtr_ADDR                    (1821 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DTX_TX_pswCNVSCode1_ADDR                     (1822 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DTX_TX_pswCNVSCode2_ADDR                     (1826 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DTX_TX_pswCNGsp0Code_ADDR                    (1830 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DTX_TX_pswCNLpc_ADDR                         (1834 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DTX_TX_swCNR0_ADDR                           (1838 + HR_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_EN_SP_FRM_pswAnalysisState_ADDR              (1840 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_EN_SP_FRM_pswWStateNum_ADDR                  (1850 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_EN_SP_FRM_pswWStateDenom_ADDR                (1860 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STATIC_EN_SP_FRM_psrTable_ADDR_ADDR                 (1870 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_RESERVED3                                        (1871 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_EN_SP_FRM_iLow_ADDR                          (1872 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_EN_SP_FRM_iThree_ADDR                        (1873 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_EN_SP_FRM_iWordHalfPtr_ADDR                  (1874 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_EN_SP_FRM_iWordPtr_ADDR                      (1875 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_pswRvad_ADDR                          (1876 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define HR_STATIC_VAD_swNormRvad_ADDR                       (1886 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swPt_sacf_ADDR                        (1887 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swPt_sav0_ADDR                        (1888 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swE_thvad_ADDR                        (1889 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swM_thvad_ADDR                        (1890 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swAdaptCount_ADDR                     (1891 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swBurstCount_ADDR                     (1892 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swHangCount_ADDR                      (1893 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swOldLagCount_ADDR                    (1894 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swVeryOldLagCount_ADDR                (1895 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swOldLag_ADDR                         (1896 + HR_GLOBAL_X_BEGIN_ADDR) //2 short
#define HR_STATIC_VAD_pL_sacf_ADDR                          (1898 + HR_GLOBAL_X_BEGIN_ADDR) //54 short
#define HR_STATIC_VAD_pL_sav0_ADDR                          (1952 + HR_GLOBAL_X_BEGIN_ADDR) //72 short
#define HR_STATIC_VAD_L_lastdm_ADDR                         (2024 + HR_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_SFRM_pswLtpStateBase_ADDR                    (2026 + HR_GLOBAL_X_BEGIN_ADDR) //188 short
#define GLOBAL_SFRM_pswHState_ADDR                          (2214 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_SFRM_pswHNWState_ADDR                        (2224 + HR_GLOBAL_X_BEGIN_ADDR) //145 short
#define HR_RESERVED4                                        (2369 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define BITS_SHORT_ADDR                                     (2370 + HR_GLOBAL_X_BEGIN_ADDR) //1 short

//GLOBAL_VARS
#define DEC_pswSythAsSpace_ADDR                             (0 + HR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define DEC_pswPFNumAsSpace_ADDR                            (40 + HR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define DEC_pswPFDenomAsSpace_ADDR                          (80 + HR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define DEC_L_RxPNSeed_ADDR                                 (120 + HR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define DEC_swRxDtxGsIndex_ADDR                             (122 + HR_GLOBAL_Y_BEGIN_ADDR) //2 short

/****************
 **  HR_LOCAL  **
 ****************/

//CII_speechEncoder
#define CII_speechEncoder_X_BEGIN_ADDR                      (0 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_EN_SPEECHENCODER_swR0_ADDR                    (0 + CII_speechEncoder_X_BEGIN_ADDR) //1 short
#define MIC_FILT_ADDR                                       (LOCAL_EN_SPEECHENCODER_swR0_ADDR)
#define LOCAL_ECHO_CANCEL_SAMPLS_ADDR                       (MIC_FILT_ADDR)
#define LOCAL_EN_SPEECHENCODER_iR0_ADDR                     (1 + CII_speechEncoder_X_BEGIN_ADDR) //1 short
#define LOCAL_EN_SPEECHENCODER_swVadFlag_ADDR               (2 + CII_speechEncoder_X_BEGIN_ADDR) //1 short
#define LOCAL_EN_SPEECHENCODER_swSP_ADDR                    (3 + CII_speechEncoder_X_BEGIN_ADDR) //1 short
#define LOCAL_EN_SPEECHENCODER_pswFrmKs_ADDR                (4 + CII_speechEncoder_X_BEGIN_ADDR) //10 short
#define LOCAL_EN_SPEECHENCODER_pswVadLags_ADDR              (14 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_pswLagList_ADDR              (18 + CII_speechEncoder_X_BEGIN_ADDR) //12 short
#define LOCAL_EN_SPEECHENCODER_piVq_ADDR                    (30 + CII_speechEncoder_X_BEGIN_ADDR) //3 short
#define LOCAL_EN_SPEECHENCODER_siUVCode_ADDR                (33 + CII_speechEncoder_X_BEGIN_ADDR) //1 short
#define LOCAL_EN_SPEECHENCODER_piVSCode1_ADDR               (34 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_piVSCode2_ADDR               (38 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_piGsp0Code_ADDR              (42 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_piLagCode_ADDR               (46 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_pswNumLagList_ADDR           (50 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_pswPitchBuf_ADDR             (54 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_pswHNWCoefBuf_ADDR           (58 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_pswFrmSNWCoefs_ADDR          (62 + CII_speechEncoder_X_BEGIN_ADDR) //10 short
#define LOCAL_EN_SPEECHENCODER_ppswSNWCoefAs_ADDR           (72 + CII_speechEncoder_X_BEGIN_ADDR) //40 short
#define LOCAL_EN_SPEECHENCODER_iSi_ADDR                     (112 + CII_speechEncoder_X_BEGIN_ADDR) //1 short
#define LOCAL_EN_SPEECHENCODER_iVoicing_ADDR                (113 + CII_speechEncoder_X_BEGIN_ADDR) //1 short
#define LOCAL_EN_SPEECHENCODER_psnsSqrtRs_ADDR              (114 + CII_speechEncoder_X_BEGIN_ADDR) //8 short
#define LOCAL_EN_SPEECHENCODER_pswLagListPtr_ADDR_ADDR          (122 + CII_speechEncoder_X_BEGIN_ADDR) //2 short

//CII_speechEncoder
#define CII_speechEncoder_Y_BEGIN_ADDR                      (0 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_EN_SPEECHENCODER_pswFrmAs_ADDR                (0 + CII_speechEncoder_Y_BEGIN_ADDR) //10 short
#define LOCAL_EN_SPEECHENCODER_ppswSynthAs_ADDR             (10 + CII_speechEncoder_Y_BEGIN_ADDR) //40 short

//CII_iir_d_opt
#define CII_iir_d_opt_X_BEGIN_ADDR                          (124 + HR_LOCAL_X_BEGIN_ADDR)
#define CII_iir_d_temp                                      (0 + CII_iir_d_opt_X_BEGIN_ADDR) //10 short

//CII_aflat_opt
#define CII_aflat_opt_X_BEGIN_ADDR                          (124 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_AFLAT_pL_VadAcf_ADDR                          (0 + CII_aflat_opt_X_BEGIN_ADDR) //18 short
#define LOCAL_AFLAT_pswFlatsRc_ADDR                         (18 + CII_aflat_opt_X_BEGIN_ADDR) //10 short
#define LOCAL_AFLAT_pL_CorrelSeq_ADDR                       (28 + CII_aflat_opt_X_BEGIN_ADDR) //22 short
#define LOCAL_AFLAT_pswVadRc_ADDR                           (50 + CII_aflat_opt_X_BEGIN_ADDR) //6 short
#define LOCAL_AFLAT_swVadScalAuto_ADDR                      (56 + CII_aflat_opt_X_BEGIN_ADDR) //10 short

//CII_speechEncoder_inner_0
#define CII_speechEncoder_inner_0_X_BEGIN_ADDR              (124 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_EN_SPEECHENCODER_pswCoefTmp2_ADDR             (0 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //10 short
#define LOCAL_EN_SPEECHENCODER_pswCoefTmp3_ADDR             (10 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //10 short
#define LOCAL_EN_SPEECHENCODER_pswVecTmp_ADDR               (20 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //40 short
#define LOCAL_EN_SPEECHENCODER_pswVecTmp2_ADDR              (60 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //40 short
#define LOCAL_EN_SPEECHENCODER_L_Temp_ADDR                  (100 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //2 short
#define LOCAL_EN_SPEECHENCODER_pL_AutoCorTmp_ADDR           (102 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //22 short
#define LOCAL_EN_SPEECHENCODER_pswTempRc_ADDR               (124 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //10 short
#define LOCAL_EN_SPEECHENCODER_pL_A_ADDR                    (134 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //20 short
#define LOCAL_EN_SPEECHENCODER_pL_tmp_ADDR                  (154 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //20 short

//CII_aFlatRcDp_opt
#define CII_aFlatRcDp_opt_Y_BEGIN_ADDR                      (324 + HR_LOCAL_Y_BEGIN_ADDR)
#define AFLATRCDP_pL_pjNewSpace_ADDR                        (0 + CII_aFlatRcDp_opt_Y_BEGIN_ADDR) //20 short
#define AFLATRCDP_pL_pjOldSpace_ADDR                        (20 + CII_aFlatRcDp_opt_Y_BEGIN_ADDR) //20 short
#define AFLATRCDP_pL_vjNewSpace_ADDR                        (40 + CII_aFlatRcDp_opt_Y_BEGIN_ADDR) //38 short
#define AFLATRCDP_pL_vjOldSpace_ADDR                        (78 + CII_aFlatRcDp_opt_Y_BEGIN_ADDR) //38 short

//CII_getSfrmLpcTx_opt
#define CII_getSfrmLpcTx_opt_X_BEGIN_ADDR                   (124 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_EN_SPEECHENCODER_interpolateCheck_pswRcTemp_ADDR          (0 + CII_getSfrmLpcTx_opt_X_BEGIN_ADDR) //10 short
#define LOCAL_compResidEnergy_pswResidual_ADDR              (10 + CII_getSfrmLpcTx_opt_X_BEGIN_ADDR) //40 short

//CII_getSfrmLpcTx_opt
#define CII_getSfrmLpcTx_opt_Y_BEGIN_ADDR                   (282 + HR_LOCAL_Y_BEGIN_ADDR)
#define cov32_copy_ADDR                                     (0 + CII_getSfrmLpcTx_opt_Y_BEGIN_ADDR) //10 short

//CII_speechEncoder_inner_1
#define CII_speechEncoder_inner_1_X_BEGIN_ADDR              (124 + HR_LOCAL_X_BEGIN_ADDR)
#define weightSpeechFrame_pswScratch0_ADDR                  (0 + CII_speechEncoder_inner_1_X_BEGIN_ADDR) //305 short

//CII_lpcIir_opt_paris
#define CII_lpcIir_opt_paris_Y_BEGIN_ADDR                   (282 + HR_LOCAL_Y_BEGIN_ADDR)
#define TEMP_COPY_ADDR_Y                                    (0 + CII_lpcIir_opt_paris_Y_BEGIN_ADDR) //10 short

//CII_openLoopLagSearch_new
#define CII_openLoopLagSearch_new_X_BEGIN_ADDR              (124 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_OPENLOOPSEARCH_pswGFrame_ADDR                 (0 + CII_openLoopLagSearch_new_X_BEGIN_ADDR) //248 short
#define LOCAL_OPENLOOPSEARCH_pswScaledWSpeechBuffer_ADDR            (248 + CII_openLoopLagSearch_new_X_BEGIN_ADDR) //306 short
#define LOCAL_OPENLOOPSEARCH_pswLPeaks_ADDR                 (554 + CII_openLoopLagSearch_new_X_BEGIN_ADDR) //14 short
#define LOCAL_OPENLOOPSEARCH_pswCPeaks_ADDR                 (568 + CII_openLoopLagSearch_new_X_BEGIN_ADDR) //14 short
#define LOCAL_OPENLOOPSEARCH_pswGPeaks_ADDR                 (582 + CII_openLoopLagSearch_new_X_BEGIN_ADDR) //14 short
#define LOCAL_OPENLOOPSEARCH_pswLArray_ADDR                 (596 + CII_openLoopLagSearch_new_X_BEGIN_ADDR) //16 short

//CII_openLoopLagSearch_new
#define CII_openLoopLagSearch_new_Y_BEGIN_ADDR              (50 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_OPENLOOPSEARCH_pswCFrame_ADDR                 (0 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //512 short
#define LOCAL_OPENLOOPSEARCH_pswSfrmEng_ADDR                (512 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //4 short
#define LOCAL_OPENLOOPSEARCH_pswLIntBuf_ADDR                (516 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //4 short
#define LOCAL_OPENLOOPSEARCH_pswCCThresh_ADDR               (520 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //4 short
#define OPENLOOP_SP32_SAVE_ADDR                             (524 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //1 short
#define OPENLOOP_SP16_SAVE_ADDR                             (525 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //1 short
#define LOCAL_OPENLOOPSEARCH_ppswTrajLBuf_ADDR              (526 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //32 short
#define LOCAL_OPENLOOPSEARCH_ppswTrajCCBuf_ADDR             (558 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //32 short
#define LOCAL_OPENLOOPSEARCH_ppswTrajGBuf_ADDR              (590 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //32 short
#define LOCAL_OPENLOOPSEARCH_pswScaledWSpeechBufferCopy_ADDR            (622 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //306 short

//CII_sfrmAnalysis_opt
#define CII_sfrmAnalysis_opt_X_BEGIN_ADDR                   (124 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_sfrmAnalysis_pswPVec_ADDR                     (0 + CII_sfrmAnalysis_opt_X_BEGIN_ADDR) //40 short
#define LOCAL_sfrmAnalysis_pswWPVec_ADDR                    (40 + CII_sfrmAnalysis_opt_X_BEGIN_ADDR) //40 short
#define LOCAL_sfrmAnalysis_pswWBasisVecs_ADDR               (80 + CII_sfrmAnalysis_opt_X_BEGIN_ADDR) //360 short
#define LOCAL_sfrmAnalysis_ppswWVselpEx_ADDR                (440 + CII_sfrmAnalysis_opt_X_BEGIN_ADDR) //80 short

//CII_sfrmAnalysis_opt
#define CII_sfrmAnalysis_opt_Y_BEGIN_ADDR                   (50 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_sfrmAnalysis_pswTempVec_ADDR                  (0 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //40 short
#define LOCAL_sfrmAnalysis_pswBitArray_ADDR                 (40 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //10 short
#define LOCAL_sfrmAnalysis_pswHNWCoefs_ADDR                 (50 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //6 short
#define LOCAL_sfrmAnalysis_pswWSVec_ADDR                    (56 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //40 short
#define sfrmAnalysis_psiVSCode1_ADDR_ADDR                   (96 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define sfrmAnalysis_psiVSCode2_ADDR_ADDR                   (97 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define sfrmAnalysis_psiGsp0Code_ADDR_ADDR                  (98 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define sfrmAnalysis_psiLagCode_ADDR_ADDR                   (99 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define LOCAL_sfrmAnalysis_swLag_ADDR                       (100 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define LOCAL_sfrmAnalysis_swLtpShift_ADDR                  (101 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define LOCAL_sfrmAnalysis_snsRs00_ADDR                     (102 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //2 short
#define LOCAL_sfrmAnalysis_snsRs11_ADDR                     (104 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //2 short
#define LOCAL_sfrmAnalysis_snsRs22_ADDR                     (106 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //2 short
#define sfrmAnalysis_SP16_SAVE0_ADDR                        (108 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define sfrmAnalysis_SP16_SAVE1_ADDR                        (109 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define LOCAL_sfrmAnalysis_ppswVselpEx_ADDR                 (110 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //80 short

//CII_flat_opt
#define CII_flat_opt_X_BEGIN_ADDR                           (190 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_FLAT_pppL_B_ADDR                              (0 + CII_flat_opt_X_BEGIN_ADDR) //400 short
#define LOCAL_pswInScale_ADDR                               (400 + CII_flat_opt_X_BEGIN_ADDR) //170 short
#define LOCAL_pL_Phi_ADDR                                   (570 + CII_flat_opt_X_BEGIN_ADDR) //22 short

//CII_flat_opt
#define CII_flat_opt_Y_BEGIN_ADDR                           (50 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_FLAT_pppL_C_ADDR                              (0 + CII_flat_opt_Y_BEGIN_ADDR) //400 short
#define LOCAL_FLAT_pppL_F_ADDR                              (400 + CII_flat_opt_Y_BEGIN_ADDR) //400 short

//CII_rcToCorrDpL_opt
#define CII_rcToCorrDpL_opt_X_BEGIN_ADDR                    (190 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_rcToCorrDpL_pL_tmpSpace_ADDR                  (0 + CII_rcToCorrDpL_opt_X_BEGIN_ADDR) //20 short
#define LOCAL_rcToCorrDpL_pL_ASpace_ADDR                    (20 + CII_rcToCorrDpL_opt_X_BEGIN_ADDR) //20 short

//CII_vad_algorithm
#define CII_vad_algorithm_Y_BEGIN_ADDR                      (50 + HR_LOCAL_Y_BEGIN_ADDR)
#define HR_VAD_ALGORITHM_swStat_ADDR                        (0 + CII_vad_algorithm_Y_BEGIN_ADDR) //1 short
#define HR_VAD_ALGORITHM_swNormRav1_ADDR                    (1 + CII_vad_algorithm_Y_BEGIN_ADDR) //1 short
#define HR_VAD_ALGORITHM_pswRav1_ADDR                       (2 + CII_vad_algorithm_Y_BEGIN_ADDR) //10 short
#define HR_VAD_ALGORITHM_pL_av0_ADDR                        (12 + CII_vad_algorithm_Y_BEGIN_ADDR) //18 short
#define HR_VAD_ALGORITHM_pL_av1_ADDR                        (30 + CII_vad_algorithm_Y_BEGIN_ADDR) //18 short
#define HR_VAD_ALGORITHM_swM_pvad_ADDR                      (48 + CII_vad_algorithm_Y_BEGIN_ADDR) //1 short
#define HR_VAD_ALGORITHM_swE_pvad_ADDR                      (49 + CII_vad_algorithm_Y_BEGIN_ADDR) //1 short
#define HR_VAD_ALGORITHM_swM_acf0_ADDR                      (50 + CII_vad_algorithm_Y_BEGIN_ADDR) //1 short
#define HR_VAD_ALGORITHM_swE_acf0_ADDR                      (51 + CII_vad_algorithm_Y_BEGIN_ADDR) //1 short
#define HR_VAD_PREDICTOR_VALUES_pswAav1_ADDR                (52 + CII_vad_algorithm_Y_BEGIN_ADDR) //10 short
#define HR_VAD_PREDICTOR_VALUES_pswVpar_ADDR                (62 + CII_vad_algorithm_Y_BEGIN_ADDR) //8 short
#define HR_VAD_STEP_UP_pL_coef_ADDR                         (70 + CII_vad_algorithm_Y_BEGIN_ADDR) //18 short
#define HR_VAD_STEP_UP_pL_work_ADDR                         (88 + CII_vad_algorithm_Y_BEGIN_ADDR) //18 short
#define HR_VAD_COMPUTE_RAV1_pL_work_ADDR                    (106 + CII_vad_algorithm_Y_BEGIN_ADDR) //18 short
#define HR_VAD_TONE_DETECTION_pswA_ADDR                     (124 + CII_vad_algorithm_Y_BEGIN_ADDR) //4 short
#define HR_SCHUR_RECURSION_pswPp_ADDR                       (128 + CII_vad_algorithm_Y_BEGIN_ADDR) //10 short
#define HR_SCHUR_RECURSION_pswKk_ADDR                       (138 + CII_vad_algorithm_Y_BEGIN_ADDR) //10 short

//CII_aflat_opt_inner_0
#define CII_aflat_opt_inner_0_X_BEGIN_ADDR                  (190 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_SWCOMFORTNOISE_pswFinalRc_ADDR                (0 + CII_aflat_opt_inner_0_X_BEGIN_ADDR) //10 short
#define LOCAL_SWCOMFORTNOISE_pL_RefCorr_ADDR                (10 + CII_aflat_opt_inner_0_X_BEGIN_ADDR) //22 short

//CII_lpcCorrQntz_opt
#define CII_lpcCorrQntz_opt_X_BEGIN_ADDR                    (222 + HR_LOCAL_X_BEGIN_ADDR)
#define LPCCORRQNTZ_pswPOldSpace_ADDR                       (0 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //4 short
#define LPCCORRQNTZ_pswVOldSpace_ADDR                       (4 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //7 short
#define LPCCORRQNTZ_pswPNewSpace_ADDR                       (11 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //4 short
#define LPCCORRQNTZ_pswVNewSpace_ADDR                       (15 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //7 short
#define LPCCORRQNTZ_pswPBar_ADDR                            (22 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //4 short
#define LPCCORRQNTZ_pswVBarSpace_ADDR                       (26 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //8 short
#define LPCCORRQNTZ_pswRc_ADDR                              (34 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //12 short
#define LPCCORRQNTZ_pL_PBarFull_ADDR                        (46 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //20 short
#define LPCCORRQNTZ_pL_VBarFullSpace_ADDR                   (66 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //38 short

//CII_lpcCorrQntz_opt
#define CII_lpcCorrQntz_opt_Y_BEGIN_ADDR                    (50 + HR_LOCAL_Y_BEGIN_ADDR)
#define LPCCORRQNTZ_quantList_ADDR                          (0 + CII_lpcCorrQntz_opt_Y_BEGIN_ADDR) //66 short
#define LPCCORRQNTZ_bestPql_ADDR                            (66 + CII_lpcCorrQntz_opt_Y_BEGIN_ADDR) //264 short
#define LPCCORRQNTZ_bestQl_ADDR                             (330 + CII_lpcCorrQntz_opt_Y_BEGIN_ADDR) //264 short

//CII_aToRc_opt2
#define CII_aToRc_opt2_X_BEGIN_ADDR                         (174 + HR_LOCAL_X_BEGIN_ADDR)
#define aToRc_pswASpace_ADDR                                (0 + CII_aToRc_opt2_X_BEGIN_ADDR) //10 short

//CII_aToRc_opt2
#define CII_aToRc_opt2_Y_BEGIN_ADDR                         (324 + HR_LOCAL_Y_BEGIN_ADDR)
#define aToRc_pswTmp_ADDR                                   (0 + CII_aToRc_opt2_Y_BEGIN_ADDR) //10 short

//CII_pitchLags_new
#define CII_pitchLags_new_X_BEGIN_ADDR                      (736 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_PITCHLAGS_pswLBuf_ADDR                        (0 + CII_pitchLags_new_X_BEGIN_ADDR) //12 short
#define LOCAL_PITCHLAGS_pswCBuf_ADDR                        (12 + CII_pitchLags_new_X_BEGIN_ADDR) //12 short
#define LOCAL_PITCHLAGS_pswLPeaks_ADDR                      (24 + CII_pitchLags_new_X_BEGIN_ADDR) //14 short
#define LOCAL_PITCHLAGS_pswCPeaks_ADDR                      (38 + CII_pitchLags_new_X_BEGIN_ADDR) //14 short

//CII_pitchLags_new
#define CII_pitchLags_new_Y_BEGIN_ADDR                      (978 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_PITCHLAGS_pswGBuf_ADDR                        (0 + CII_pitchLags_new_Y_BEGIN_ADDR) //12 short
#define LOCAL_PITCHLAGS_pswGPeaks_ADDR                      (12 + CII_pitchLags_new_Y_BEGIN_ADDR) //14 short
#define PITCHLAG_SP32_SAVE_ADDR                             (26 + CII_pitchLags_new_Y_BEGIN_ADDR) //1 short
#define PITCHLAG_SP16_SAVE_ADDR                             (27 + CII_pitchLags_new_Y_BEGIN_ADDR) //1 short

//CII_bestDelta_new
#define CII_bestDelta_new_X_BEGIN_ADDR                      (736 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_BESTDELTA_pswCBuf_ADDR                        (0 + CII_bestDelta_new_X_BEGIN_ADDR) //24 short
#define LOCAL_BESTDELTA_pswCInterp_ADDR                     (24 + CII_bestDelta_new_X_BEGIN_ADDR) //18 short

//CII_bestDelta_new
#define CII_bestDelta_new_Y_BEGIN_ADDR                      (978 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_BESTDELTA_pswGBuf_ADDR                        (0 + CII_bestDelta_new_Y_BEGIN_ADDR) //24 short
#define LOCAL_BESTDELTA_pswGInterp_ADDR                     (24 + CII_bestDelta_new_Y_BEGIN_ADDR) //18 short

//CII_sfrmAnalysis_opt_inner_0
#define CII_sfrmAnalysis_opt_inner_0_X_BEGIN_ADDR           (644 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_closedLoopLagSearch_ppswTVect_ADDR            (0 + CII_sfrmAnalysis_opt_inner_0_X_BEGIN_ADDR) //160 short

//CII_sfrmAnalysis_opt_inner_0
#define CII_sfrmAnalysis_opt_inner_0_Y_BEGIN_ADDR           (240 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_closedLoopLagSearch_pL_CCBuf_ADDR             (0 + CII_sfrmAnalysis_opt_inner_0_Y_BEGIN_ADDR) //12 short
#define LOCAL_closedLoopLagSearch_pL_CGBuf_ADDR             (12 + CII_sfrmAnalysis_opt_inner_0_Y_BEGIN_ADDR) //12 short
#define LOCAL_closedLoopLagSearch_pswCCBuf_ADDR             (24 + CII_sfrmAnalysis_opt_inner_0_Y_BEGIN_ADDR) //6 short
#define LOCAL_closedLoopLagSearch_pswCGBuf_ADDR             (30 + CII_sfrmAnalysis_opt_inner_0_Y_BEGIN_ADDR) //6 short

//CII_hnwFilt_opt
#define CII_hnwFilt_opt_X_BEGIN_ADDR                        (644 + HR_LOCAL_X_BEGIN_ADDR)
#define hnwFilt_swZeroState_ADDR                            (0 + CII_hnwFilt_opt_X_BEGIN_ADDR) //1 short

//CII_lpcZsIir_opt
#define CII_lpcZsIir_opt_Y_BEGIN_ADDR                       (276 + HR_LOCAL_Y_BEGIN_ADDR)
#define lpcZsIir_copy_ADDR                                  (0 + CII_lpcZsIir_opt_Y_BEGIN_ADDR) //80 short

//CII_decorr_opt
#define CII_decorr_opt_X_BEGIN_ADDR                         (644 + HR_LOCAL_X_BEGIN_ADDR)
#define decorr_pswGivenVect_ADDR_ADDR                       (0 + CII_decorr_opt_X_BEGIN_ADDR) //1 short

//CII_v_srch_opt
#define CII_v_srch_opt_X_BEGIN_ADDR                         (644 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_v_srch_pswCGUpdates_ADDR                      (0 + CII_v_srch_opt_X_BEGIN_ADDR) //144 short

//CII_v_srch_opt
#define CII_v_srch_opt_Y_BEGIN_ADDR                         (240 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_v_srch_pswBits_ADDR                           (0 + CII_v_srch_opt_Y_BEGIN_ADDR) //10 short
#define LOCAL_v_srch_pswWBasisVecs_copy_ADDR                (10 + CII_v_srch_opt_Y_BEGIN_ADDR) //360 short
#define LOCAL_v_srch_pswDSpace_ADDR                         (370 + CII_v_srch_opt_Y_BEGIN_ADDR) //144 short
#define LOCAL_v_srch_pL_R_ADDR                              (514 + CII_v_srch_opt_Y_BEGIN_ADDR) //18 short
#define LOCAL_v_srch_siBest_ADDR                            (532 + CII_v_srch_opt_Y_BEGIN_ADDR) //1 short
#define LOCAL_v_srch_pswModNextBit_ADDR_ADDR                (533 + CII_v_srch_opt_Y_BEGIN_ADDR) //1 short
#define LOCAL_v_srch_pswBits_ADDR_ADDR                      (534 + CII_v_srch_opt_Y_BEGIN_ADDR) //1 short

//CII_sfrmAnalysis_opt_inner_1
#define CII_sfrmAnalysis_opt_inner_1_X_BEGIN_ADDR           (644 + HR_LOCAL_X_BEGIN_ADDR)
#define g_quant_vl_swWIShift_ADDR                           (0 + CII_sfrmAnalysis_opt_inner_1_X_BEGIN_ADDR) //1 short
#define LOCAL_g_quant_vl_siNormMin_ADDR                     (1 + CII_sfrmAnalysis_opt_inner_1_X_BEGIN_ADDR) //1 short
#define LOCAL_g_quant_vl_ErrorTerm_ADDR                     (2 + CII_sfrmAnalysis_opt_inner_1_X_BEGIN_ADDR) //12 short
#define LOCAL_gainTweak_siNorm_ADDR                         (14 + CII_sfrmAnalysis_opt_inner_1_X_BEGIN_ADDR) //2 short
#define LOCAL_gainTweak_terms_ADDR                          (16 + CII_sfrmAnalysis_opt_inner_1_X_BEGIN_ADDR) //10 short

//CII_lpcCorrQntz_opt_inner_0
#define CII_lpcCorrQntz_opt_inner_0_Y_BEGIN_ADDR            (644 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_AFLATNEWBARRECURSION_pL_POldSpace_ADDR            (0 + CII_lpcCorrQntz_opt_inner_0_Y_BEGIN_ADDR) //20 short
#define LOCAL_AFLATNEWBARRECURSION_pL_VOldSpace_ADDR            (20 + CII_lpcCorrQntz_opt_inner_0_Y_BEGIN_ADDR) //38 short
#define LOCAL_AFLATNEWBARRECURSION_pL_PNewSpace_ADDR            (58 + CII_lpcCorrQntz_opt_inner_0_Y_BEGIN_ADDR) //20 short
#define LOCAL_AFLATNEWBARRECURSION_pL_VNewSpace_ADDR            (78 + CII_lpcCorrQntz_opt_inner_0_Y_BEGIN_ADDR) //38 short

//CII_aflatRecursionLoop_opt_inner_0
#define CII_aflatRecursionLoop_opt_inner_0_X_BEGIN_ADDR         (326 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_AFLATRECURSION_pswQntRcSqd_ADDR               (0 + CII_aflatRecursionLoop_opt_inner_0_X_BEGIN_ADDR) //4 short

//CII_speechDecoder_opt
#define CII_speechDecoder_opt_X_BEGIN_ADDR                  (0 + HR_LOCAL_X_BEGIN_ADDR)
#define DEC_pswSynthFiltOut_ADDR                            (0 + CII_speechDecoder_opt_X_BEGIN_ADDR) //40 short

//CII_speechDecoder_opt
#define CII_speechDecoder_opt_Y_BEGIN_ADDR                  (0 + HR_LOCAL_Y_BEGIN_ADDR)
#define DEC_pswFrmPFDenom_ADDR                              (0 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //10 short
#define DEC_pswFrmKs_ADDR                                   (10 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //10 short
#define DEC_pswFrmAs_ADDR                                   (20 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //10 short
#define DEC_pswFrmPFNum_ADDR                                (30 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //20 short
#define DEC_siGsp0Code_ADDR                                 (50 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //1 short
#define FLAG_GETSFRMLPC_TX_ADDR                             (51 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //1 short
#define DEC_psiVselpCw_ADDR                                 (52 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //2 short
#define DEC_pswPVec_ADDR                                    (54 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //40 short
#define DEC_ppswVselpEx_ADDR                                (94 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //80 short
#define DEC_pswExcite_ADDR                                  (174 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //40 short
#define DEC_pswPPFExcit_ADDR                                (214 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //40 short
#define DEC_pswBitArray_ADDR                                (254 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //10 short
#define DEC_psnsSqrtRs_ADDR                                 (264 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //8 short
#define DEC_snsRs00_ADDR                                    (272 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //2 short
#define DEC_snsRs11_ADDR                                    (274 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //2 short
#define DEC_snsRs22_ADDR                                    (276 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //2 short
#define DEC_swMutePermit_ADDR                               (278 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //1 short
#define DEC_swLevelMean_ADDR                                (279 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //1 short
#define DEC_swLevelMax_ADDR                                 (280 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //1 short
#define DEC_swMuteFlag_ADDR                                 (281 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //1 short

//CII_a_sst_opt
#define CII_a_sst_opt_Y_BEGIN_ADDR                          (282 + HR_LOCAL_Y_BEGIN_ADDR)
#define A_SST_pL_CorrTemp_ADDR                              (0 + CII_a_sst_opt_Y_BEGIN_ADDR) //22 short
#define A_SST_pswRCNum_ADDR                                 (22 + CII_a_sst_opt_Y_BEGIN_ADDR) //10 short
#define A_SST_pswRCDenom_ADDR                               (32 + CII_a_sst_opt_Y_BEGIN_ADDR) //10 short

//CII_speechDecoder_opt_inner_0
#define CII_speechDecoder_opt_inner_0_Y_BEGIN_ADDR          (282 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_pitchPreFilt_pswInterpCoefs_ADDR              (0 + CII_speechDecoder_opt_inner_0_Y_BEGIN_ADDR) //10 short

//CII_speechDecoder_opt_inner_1
#define CII_speechDecoder_opt_inner_1_X_BEGIN_ADDR          (40 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_signal_conceal_sub_pswStateTmp_ADDR           (0 + CII_speechDecoder_opt_inner_1_X_BEGIN_ADDR) //10 short
#define LOCAL_signal_conceal_sub_swOutTmp_ADDR              (10 + CII_speechDecoder_opt_inner_1_X_BEGIN_ADDR) //40 short

/**********************
 **  AMR_EFR_GLOBAL  **
 **********************/

//GLOBAL_VARS
#define STRUCT_POST_FILTERSTATE_RES2_ADDR                   (0 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_POST_FILTERSTATE_MEM_SYN_PST_ADDR            (40 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_PREEMPHASISSTATE_ADDR                        (50 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_AGCSTATE_ADDR                                (51 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_FILTERSTATE_SYNTH_BUF_ADDR              (52 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //170 short
#define STRUCT_POST_PROCESSSTATE_Y2_LO_ADDR                 (222 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_Y2_HI_ADDR                 (223 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_Y1_LO_ADDR                 (224 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_Y1_HI_ADDR                 (225 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_X0_ADDR                    (226 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_X1_ADDR                    (227 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_RXDTX_CTRL_ADDR                              (STRUCT_POST_PROCESSSTATE_Y2_LO_ADDR)
#define STATIC_PREV_SID_FRAMES_LOST_ADDR                    (STRUCT_POST_PROCESSSTATE_Y2_HI_ADDR)
#define GLOBAL_RX_DTX_STATE_ADDR                            (STRUCT_POST_PROCESSSTATE_Y1_LO_ADDR)
#define STATIC_RXDTX_N_ELAPSED_ADDR                         (STRUCT_POST_PROCESSSTATE_Y1_HI_ADDR)
#define STATIC_RXDTX_AVER_PERIOD_ADDR                       (STRUCT_POST_PROCESSSTATE_X0_ADDR)
#define STATIC_BUF_P_RX_ADDR                                (STRUCT_POST_PROCESSSTATE_X1_ADDR)
#define STURCT_SPEECH_DECODE_FRAMESTATE_PREV_MODE_ADDR          (228 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STATIC_L_SACF_ADDR                                  (236 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //54 short
#define GLOBAL_CN_EXCITATION_GAIN_ADDR                      (290 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_GCODE0_CN_ADDR                               (291 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define SCAL_ACF_ADDR_P                                     (292 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define VAD_FLAG_ADDR_P                                     (293 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define TX_SP_FLAG_ADDR                                     (294 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_TXDTX_CTRL_ADDR                              (295 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define PRAM4_TABLE_ADDR                                    (296 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_PRE_PROCESSSTATE_Y2_LO_ADDR                  (306 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PRE_PROCESSSTATE_Y2_HI_ADDR                  (307 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PRE_PROCESSSTATE_Y1_LO_ADDR                  (308 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PRE_PROCESSSTATE_Y1_HI_ADDR                  (309 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PRE_PROCESSSTATE_X0_ADDR                     (310 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PRE_PROCESSSTATE_X1_ADDR                     (311 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_COD_AMRSTATE_OLD_SPEECH_ADDR                 (312 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_P_WINDOW_12K2_ADDR              (352 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_P_WINDOW_ADDR                   (392 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_SPEECH_ADDR                     (432 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR                 (472 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //161 short
#define STRUCT_COD_AMRSTATE_OLD_WSP_ADDR                    (633 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //143 short
#define STRUCT_COD_AMRSTATE_WSP_ADDR                        (776 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //160 short
#define STRUCT_COD_AMRSTATE_OLD_LAGS_ADDR                   (936 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_COD_AMRSTATE_OL_GAIN_FLG_ADDR                (942 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_COD_AMRSTATE_OLD_EXC_ADDR                    (944 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //154 short
#define STRUCT_COD_AMRSTATE_EXC_ADDR                        (1098 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //161 short
#define STRUCT_COD_AMRSTATE_AI_ZERO_ADDR                    (1259 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //11 short
#define STRUCT_COD_AMRSTATE_ZERO_ADDR                       (1270 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //41 short
#define STRUCT_LEVINSONSTATE_OLD_A_ADDR                     (1311 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //11 short
#define STRUCT_LSPSTATE_LSP_OLD_ADDR                        (1322 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_LSPSTATE_LSP_OLD_Q_ADDR                      (1332 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_Q_PLSFSTATE_PAST_RQ_ADDR                     (1342 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_PITCH_FRSTATE_T0_PREV_SUBFRAME_ADDR          (1352 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_GAINQUANTSTATE_SF0_FRAC_GCODE0_ADDR          (1354 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GAINQUANTSTATE_SF0_EXP_GCODE0_ADDR           (1355 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GAINQUANTSTATE_SF0_EXP_TARGET_EN_ADDR            (1356 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GAINQUANTSTATE_SF0_FRAC_TARGET_EN_ADDR           (1357 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GAINQUANTSTATE_SF0_EXP_COEFF_ADDR            (1358 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_GAINQUANTSTATE_SF0_FRAC_COEFF_ADDR           (1364 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //5 short
#define STRUCT_GAINQUANTSTATE_GAIN_IDX_PTR_ADDR             (1369 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GC_PREDST_ADDR                               (1370 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_GC_PREDST_PAST_QUA_EN_MR122_ADDR             (1374 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STURCT_GC_PREDUNQST_ADDR                            (1378 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_GC_PREDUNQST_PAST_QUA_EN_MR122_ADDR          (1382 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_GAINADAPTSTATE_ONSET_ADDR                    (1386 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GAINADAPTSTATE_PREV_ALPHA_ADDR               (1387 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GAINADAPTSTATE_PREV_GC_ADDR                  (1388 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_GAINADAPTSTATE_LTPG_MEM_ADDR                 (1390 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_PITCHOLWGHTSTATE_OLD_T0_MED_ADDR             (1396 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PITCHOLWGHTSTATE_ADA_W_ADDR                  (1397 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PITCHOLWGHTSTATE_WGHT_FLG_ADDR               (1398 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_TONSTABSTATE_GP_ADDR                         (1400 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //7 short
#define STRUCT_TONSTABSTATE_COUNT_ADDR                      (1407 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_BCKR_EST_ADDR                      (1408 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_VADSTATE1_AVE_LEVEL_ADDR                     (1418 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_VADSTATE1_OLD_LEVEL_ADDR                     (1428 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_VADSTATE1_SUB_LEVEL_ADDR                     (1438 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_VADSTATE1_A_DATA5_ADDR                       (1448 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_VADSTATE1_A_DATA3_ADDR                       (1454 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_VADSTATE1_BURST_COUNT_ADDR                   (1460 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_HANG_COUNT_ADDR                    (1461 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_STAT_COUNT_ADDR                    (1462 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_VADREG_ADDR                        (1463 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_PITCH_ADDR                         (1464 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_TONE_ADDR                          (1465 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_COMPLEX_HIGH_ADDR                  (1466 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_COMPLEX_LOW_ADDR                   (1467 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_OLDLAG_COUNT_ADDR                  (1468 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_OLDLAG_ADDR                        (1469 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_COMPLEX_HANG_COUNT_ADDR            (1470 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_COMPLEX_HANG_TIMER_ADDR            (1471 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_BEST_CORR_HP_ADDR                  (1472 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_SPEECH_VAD_DECISION_ADDR           (1473 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_COMPLEX_WARNING_ADDR               (1474 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_SP_BURST_COUNT_ADDR                (1475 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_CORR_HP_FAST_ADDR                  (1476 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_COD_AMRSTATE_DTX_ADDR                        (1477 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_ENCSTATE_LSP_HIST_ADDR                   (1478 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //80 short
#define STRUCT_DTX_ENCSTATE_LOG_EN_HIST_ADDR                (1558 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STRUCT_DTX_ENCSTATE_HIST_PTR_ADDR                   (1566 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_ENCSTATE_LOG_EN_INDEX_ADDR               (1567 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_ENCSTATE_INIT_LSF_VQ_INDEX_ADDR          (1568 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_DTX_ENCSTATE_LSP_INDEX_ADDR                  (1570 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_DTX_ENCSTATE_DTXHANGOVERCOUNT_ADDR           (1574 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_ENCSTATE_DECANAELAPSEDCOUNT_ADDR         (1575 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_COD_AMRSTATE_MEM_SYN_ADDR                    (1576 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_MEM_W0_ADDR                     (1586 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_MEM_W_ADDR                      (1596 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_MEM_ERROR_ADDR                  (1606 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_ERROR_ADDR                      (1616 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_SHARP_ADDR                      (1656 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_SPEECH_ENCODE_FRAMESTATE_DTX_ADDR            (1657 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_SID_SYNCSTATE_SID_UPDATE_RATE_ADDR           (1658 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_SID_SYNCSTATE_SID_UPDATE_COUNTER_ADDR            (1659 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_SID_SYNCSTATE_SID_HANDOVER_DEBT_ADDR         (1660 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_SID_SYNCSTATE_PREV_FT_ADDR                   (1661 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_OLD_EXC_ADDR                  (1662 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //154 short
#define STRUCT_DECOD_AMRSTATE_EXC_ADDR                      (1816 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_DECOD_AMRSTATE_LSP_OLD_ADDR                  (1856 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DECOD_AMRSTATE_MEM_SYN_ADDR                  (1866 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DECOD_AMRSTATE_SHARP_ADDR                    (1876 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR                   (1877 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR                  (1878 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_PREV_PDF_ADDR                 (1879 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_STATE_ADDR                    (1880 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_T0_LAGBUFF_ADDR               (1881 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_EXCENERGYHIST_ADDR            (1882 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DECOD_AMRSTATE_INBACKGROUNDNOISE_ADDR            (1892 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_VOICEDHANGOVER_ADDR           (1893 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_LTPGAINHISTORY_ADDR           (1894 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //9 short
#define STRUCT_DECOD_AMRSTATE_NODATASEED_ADDR               (1903 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_BGN_SCDSTATE_FRAMEENERGYHIST_ADDR            (1904 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //60 short
#define STRUCT_BGN_SCDSTATE_BGHANGOVER_ADDR                 (1964 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_CB_GAIN_AVERAGESTATE_ADDR                    (1966 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR          (1974 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_CB_GAIN_AVERAGESTATE_HANGVAR_ADDR            (1975 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_LSP_AVGSTATE_LSP_MEANSAVE_ADDR               (1976 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR                  (1986 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_D_PLSFSTATE_PAST_R_Q_ADDR                    (1996 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_EC_GAIN_PITCHSTATE_ADDR                      (2006 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_EC_GAIN_PITCHSTATE_PAST_GAIN_PIT_ADDR            (2012 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_EC_GAIN_PITCHSTATE_PREV_GP_ADDR              (2013 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_EC_GAIN_CODESTATE_ADDR                       (2014 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_EC_GAIN_CODESTATE_PAST_GAIN_CODE_ADDR            (2020 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_EC_GAIN_CODESTATE_PREV_GC_ADDR               (2021 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR                (2022 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_GC_PREDSTATE_PAST_QUA_EN_MR122_ADDR          (2026 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_PH_DISPSTATE_GAINMEM_ADDR                    (2030 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_PH_DISPSTATE_PREVSTATE_ADDR                  (2036 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PH_DISPSTATE_PREVCBGAIN_ADDR                 (2037 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PH_DISPSTATE_LOCKFULL_ADDR                   (2038 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PH_DISPSTATE_ONSET_ADDR                      (2039 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_SINCE_LAST_SID_ADDR             (2040 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_TRUE_SID_PERIOD_INV_ADDR            (2041 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_LOG_EN_ADDR                     (2042 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_OLD_LOG_EN_ADDR                 (2043 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_L_PN_SEED_RX_ADDR               (2044 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_DTX_DECSTATE_LSP_ADDR                        (2046 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DTX_DECSTATE_LSP_OLD_ADDR                    (2056 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DTX_DECSTATE_LSF_HIST_ADDR                   (2066 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //80 short
#define STRUCT_DTX_DECSTATE_LSF_HIST_PTR_ADDR               (2146 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_LOG_PG_MEAN_ADDR                (2147 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define DEC_STATIC_LSF_P_CN_ADDR                            (2148 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_LSF_OLD_RX_ADDR                              (2158 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //70 short
#define STRUCT_DTX_DECSTATE_LOG_EN_HIST_ADDR                (2228 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STRUCT_DTX_DECSTATE_LOG_EN_HIST_PTR_ADDR            (2236 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_LOG_EN_ADJUST_ADDR              (2237 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DTXHANGOVERCOUNT_ADDR           (2238 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DECANAELAPSEDCOUNT_ADDR         (2239 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_SID_FRAME_ADDR                  (2240 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_VALID_DATA_ADDR                 (2241 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DTXHANGOVERADDED_ADDR           (2242 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DTXGLOBALSTATE_ADDR             (2243 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DATA_UPDATED_ADDR               (2244 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_L_SAV0_ADDR                                  (2246 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //72 short
#define GLOBAL_LSF_NEW_CN_DEC_ADDR                          (2318 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_LSF_OLD_CN_DEC_ADDR                          (2328 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_GAIN_CODE_OLD_RX_ADDR                        (2338 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //28 short

//GLOBAL_VARS
#define STRUCT_COD_AMRSTATE_HVEC_ADDR                       (0 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_H1_ADDR                         (40 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define STATIC_SCAL_RVAD_ADDR                               (80 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define COD_AMR_CODE_SF0_ADDRESS                            (STATIC_SCAL_RVAD_ADDR)
#define STATIC_RVAD_ADDR                                    (81 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //9 short
#define STATIC_BURSTCOUNT_ADDR                              (90 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_HANGCOUNT_ADDR                               (91 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_PT_SACF_ADDR                                 (92 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_PT_SAV0_ADDR                                 (93 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_L_LASTDM_ADDR                                (94 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define STATIC_ADAPTCOUNT_ADDR                              (96 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_PTCH_ADDR                                    (97 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_THVAD_ADDR                                   (98 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define STATIC_LSF_P_CN_ADDR                                (102 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define GLOBAL_LSF_OLD_TX_ADDR                              (112 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define COD_AMR_XN_SF0_ADDRESS                              (122 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define COD_AMR_Y2_SF0_ADDRESS                              (162 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //20 short
#define STATIC_TXDTX_HANGOVER_ADDR                          (182 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_TXDTX_N_ELAPSED_ADDR                         (183 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_L_PN_SEED_TX_ADDR                            (184 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define STATIC_VERYOLDLAGCOUNT_ADDR                         (186 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_BUF_P_TX_ADDR                                (187 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_OLD_CN_MEM_TX_ADDR                           (188 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_GAIN_CODE_OLD_TX_ADDR                        (196 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //6 short
#define COD_AMR_T0_SF0_ADDRESS                              (202 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define COD_AMR_T0_FRAC_SF0_ADDRESS                         (203 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define COD_AMR_I_SUBFR_SF0_ADDRESS                         (204 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define COD_AMR_GAIN_PIT_SF0_ADDRESS                        (206 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define COD_AMR_GAIN_CODE_SF0_ADDRESS                       (207 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Blankkk                                             (208 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //16 short
#define GLOBAL_L_PN_SEED_RX_ADDR                            (224 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_GCODE0_CN_DEC_ADDR                           (226 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_GAIN_CODE_OLD_CN_DEC_ADDR                    (227 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_GAIN_CODE_NEW_CN_DEC_ADDR                    (228 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_GAIN_CODE_MUTING_CN_DEC_ADDR                 (229 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short

/*********************
 **  AMR_EFR_LOCAL  **
 *********************/

//CII_AMR_Encode
#define CII_AMR_Encode_X_BEGIN_ADDR                         (0 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define COD_AMR_XN_ADDRESS                                  (0 + CII_AMR_Encode_X_BEGIN_ADDR) //40 short
#define COD_AMR_XN2_ADDRESS                                 (40 + CII_AMR_Encode_X_BEGIN_ADDR) //40 short
#define COD_AMR_Y1_ADDRESS                                  (80 + CII_AMR_Encode_X_BEGIN_ADDR) //40 short
#define COD_AMR_Y2_ADDRESS                                  (120 + CII_AMR_Encode_X_BEGIN_ADDR) //40 short
#define COD_AMR_RES_ADDRESS                                 (160 + CII_AMR_Encode_X_BEGIN_ADDR) //40 short
#define COD_AMR_MEM_SYN_SAVE_ADDRESS                        (200 + CII_AMR_Encode_X_BEGIN_ADDR) //10 short
#define COD_AMR_MEM_W0_SAVE_ADDRESS                         (210 + CII_AMR_Encode_X_BEGIN_ADDR) //10 short
#define COD_AMR_MEM_ERR_SAVE_ADDRESS                        (220 + CII_AMR_Encode_X_BEGIN_ADDR) //10 short
#define COD_AMR_GCOEFF_ADDRESS                              (230 + CII_AMR_Encode_X_BEGIN_ADDR) //4 short
#define COD_AMR_SUBFRNR_ADDRESS                             (234 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_I_SUBFR_ADDRESS                             (235 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_T_OP_ADDRESS                                (236 + CII_AMR_Encode_X_BEGIN_ADDR) //2 short
#define COD_AMR_T0_ADDRESS                                  (238 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_T0_FRAC_ADDRESS                             (239 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_GAIN_PIT_ADDRESS                            (240 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_GAIN_CODE_ADDRESS                           (241 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_EXC_ADDR_ADDRESS                            (242 + CII_AMR_Encode_X_BEGIN_ADDR) //2 short
#define COD_AMR_LSP_FLAG_ADDRESS                            (244 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_GP_LIMIT_ADDRESS                            (245 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_VAD_FLAG_ADDRESS                            (246 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_COMPUTE_SID_FLAG_ADDRESS                    (247 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_A_T_ADDRESS                                 (248 + CII_AMR_Encode_X_BEGIN_ADDR) //44 short
#define COD_AMR_AQ_T_ADDRESS                                (292 + CII_AMR_Encode_X_BEGIN_ADDR) //44 short
#define COD_AMR_LSP_NEW_ADDRESS                             (336 + CII_AMR_Encode_X_BEGIN_ADDR) //10 short
#define COD_AMR_SHARP_SAVE_ADDRESS                          (346 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_EVENSUBFR_ADDRESS                           (347 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_CODE_ADDRESS                                (348 + CII_AMR_Encode_X_BEGIN_ADDR) //40 short
#define DEC_AMR_CODE_ADDRESS                                (COD_AMR_CODE_ADDRESS)

//CII_AMR_Encode
#define CII_AMR_Encode_Y_BEGIN_ADDR                         (0 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define COD_AMR_ANA_ADDR                                    (0 + CII_AMR_Encode_Y_BEGIN_ADDR) //60 short
#define COD_AMR_RES3_ADDRESS                                (60 + CII_AMR_Encode_Y_BEGIN_ADDR) //40 short
#define BLANK                                               (100 + CII_AMR_Encode_Y_BEGIN_ADDR) //2 short

//CII_vad1
#define CII_vad1_Y_BEGIN_ADDR                               (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define VAD1_level_ADDR                                     (0 + CII_vad1_Y_BEGIN_ADDR) //11 short
#define VAD1_decission_noise_level_ADDR                     (11 + CII_vad1_Y_BEGIN_ADDR) //1 short
#define VAD1_low_power_flag_ADDR                            (12 + CII_vad1_Y_BEGIN_ADDR) //1 short

//CII_Levinson
#define CII_Levinson_X_BEGIN_ADDR                           (388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define LEVINSON_ANL_ADDR                                   (0 + CII_Levinson_X_BEGIN_ADDR) //11 short

//CII_Levinson
#define CII_Levinson_Y_BEGIN_ADDR                           (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define Blank_LEVINSON                                      (0 + CII_Levinson_Y_BEGIN_ADDR) //3 short
#define PARA4_ADDR_ADDR                                     (3 + CII_Levinson_Y_BEGIN_ADDR) //1 short
#define LEVINSON_AL_ADDR                                    (4 + CII_Levinson_Y_BEGIN_ADDR) //22 short

//CII_subframePostProc
#define CII_subframePostProc_Y_BEGIN_ADDR                   (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define COD_AMR_SYNTH_ADDR                                  (0 + CII_subframePostProc_Y_BEGIN_ADDR) //160 short

//CII_Syn_filt
#define CII_Syn_filt_Y_BEGIN_ADDR                           (262 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define SYN_FILT_TMP_ADDR                                   (0 + CII_Syn_filt_Y_BEGIN_ADDR) //80 short

//CII_Autocorr
#define CII_Autocorr_X_BEGIN_ADDR                           (388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define AUTOCORR_Y_ADDR                                     (0 + CII_Autocorr_X_BEGIN_ADDR) //240 short
#define LPC_RLOW_ADDR                                       (240 + CII_Autocorr_X_BEGIN_ADDR) //12 short
#define LPC_RHIGH_ADDR                                      (252 + CII_Autocorr_X_BEGIN_ADDR) //18 short
#define LPC_RC_ADDR                                         (270 + CII_Autocorr_X_BEGIN_ADDR) //4 short

//CII_Autocorr
#define CII_Autocorr_Y_BEGIN_ADDR                           (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define Blamk_AUTOCORR_Y_ADDR                               (0 + CII_Autocorr_Y_BEGIN_ADDR) //4 short
#define AUTOCORR_Y_COPY_ADDR                                (4 + CII_Autocorr_Y_BEGIN_ADDR) //240 short

//CII_Az_lsp
#define CII_Az_lsp_X_BEGIN_ADDR                             (418 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define AZ_LSP_IP_ADDR                                      (0 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_COEF_ADDR                                    (1 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_GRID_J_ADDR_ADDR                             (2 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_LOOP_CTRL_ADDR                               (3 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_XLOW_ADDR                                    (4 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_YLOW_ADDR                                    (5 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_XHIGH_ADDR                                   (6 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_YHIGH_ADDR                                   (7 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_XMID_ADDR                                    (8 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_YMID_ADDR                                    (9 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_F1_ADDR                                      (10 + CII_Az_lsp_X_BEGIN_ADDR) //10 short
#define AZ_LSP_F2_ADDR                                      (20 + CII_Az_lsp_X_BEGIN_ADDR) //10 short

//CII_Chebps
#define CII_Chebps_Y_BEGIN_ADDR                             (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define CHEBPS_B2_ADDR                                      (0 + CII_Chebps_Y_BEGIN_ADDR) //2 short

//CII_cor_h_x
#define CII_cor_h_x_X_BEGIN_ADDR                            (388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define COR_H_X_Y32_ADDR                                    (0 + CII_cor_h_x_X_BEGIN_ADDR) //40 short

//CII_cor_h
#define CII_cor_h_X_BEGIN_ADDR                              (388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define COR_H_H2_ADDR                                       (0 + CII_cor_h_X_BEGIN_ADDR) //40 short

//CII_cor_h
#define CII_cor_h_Y_BEGIN_ADDR                              (962 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define COR_H_H2_COPY_ADDR                                  (0 + CII_cor_h_Y_BEGIN_ADDR) //40 short
#define COPY_C1035PF_SIGN_ADDR                              (40 + CII_cor_h_Y_BEGIN_ADDR) //20 short

//CII_G_pitch
#define CII_G_pitch_Y_BEGIN_ADDR                            (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define G_PITCH_SCALED_Y1_ADDR                              (0 + CII_G_pitch_Y_BEGIN_ADDR) //40 short
#define G_PITCH_Y1_ADDR                                     (40 + CII_G_pitch_Y_BEGIN_ADDR) //40 short

//CII_subframePreProc
#define CII_subframePreProc_X_BEGIN_ADDR                    (388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define PRE_BIG_AP1_ADDR                                    (0 + CII_subframePreProc_X_BEGIN_ADDR) //20 short
#define PRE_BIG_AP2_ADDR                                    (20 + CII_subframePreProc_X_BEGIN_ADDR) //20 short

//CII_AMR_Decode
#define CII_AMR_Decode_X_BEGIN_ADDR                         (0 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define DEC_AMR_LSP_NEW_ADDRESS                             (0 + CII_AMR_Decode_X_BEGIN_ADDR) //10 short
#define DEC_AMR_LSP_MID_ADDRESS                             (10 + CII_AMR_Decode_X_BEGIN_ADDR) //10 short
#define DEC_AMR_PREV_LSF_ADDRESS                            (20 + CII_AMR_Decode_X_BEGIN_ADDR) //10 short
#define DEC_AMR_LSF_I_ADDRESS                               (30 + CII_AMR_Decode_X_BEGIN_ADDR) //50 short
#define DEC_AMR_EXCP_ADDRESS                                (80 + CII_AMR_Decode_X_BEGIN_ADDR) //40 short
#define DEC_AMR_EXC_ENHANCED_ADDRESS                        (120 + CII_AMR_Decode_X_BEGIN_ADDR) //44 short
#define DEC_AMR_T0_ADDRESS                                  (164 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_T0_FRAC_ADDRESS                             (165 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_INDEX_ADDRESS                               (166 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_INDEX_MR475_ADDRESS                         (167 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_GAIN_PIT_ADDRESS                            (168 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_GAIN_CODE_ADDRESS                           (169 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_GAIN_CODE_MIX_ADDRESS                       (170 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_PIT_SHARP_ADDRESS                           (171 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_PIT_FLAG_ADDRESS                            (172 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_PIT_FAC_ADDRESS                             (173 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_T0_MIN_ADDRESS                              (174 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_T0_MAX_ADDRESS                              (175 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_DELTA_FRC_LOW_ADDRESS                       (176 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_DELTA_FRC_RANGE_ADDRESS                     (177 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP_SHIFT_ADDRESS                          (178 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP_ADDRESS                                (179 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_L_TEMP_ADDRESS                              (180 + CII_AMR_Decode_X_BEGIN_ADDR) //2 short
#define DEC_AMR_FLAG4_ADDRESS                               (182 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_CAREFULFLAG_ADDRESS                         (183 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_EXCENERGY_ADDRESS                           (184 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_SUBFRNR_ADDRESS                             (185 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_EVENSUBFR_ADDRESS                           (186 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_NEWDTXSTATE_ADDRESS                         (187 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_BFI_ADDRESS                                 (188 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_PDFI_ADDRESS                                (189 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_A_T_ADDRESS                                 (190 + CII_AMR_Decode_X_BEGIN_ADDR) //44 short
#define DEC_AMR_PARM_ADDRESS                                (234 + CII_AMR_Decode_X_BEGIN_ADDR) //57 short

//CII_AMR_Decode
#define CII_AMR_Decode_Y_BEGIN_ADDR                         (0 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define AMR_DEC_BUFOUT_ADDR                                 (0 + CII_AMR_Decode_Y_BEGIN_ADDR) //160 short

//CII_bits2prm_amr_efr
#define CII_bits2prm_amr_efr_X_BEGIN_ADDR                   (292 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define AMR_DEC_BUFIN_ADDR                                  (0 + CII_bits2prm_amr_efr_X_BEGIN_ADDR) //246 short

//CII_dtx_dec
#define CII_dtx_dec_X_BEGIN_ADDR                            (292 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define DTX_DEC_L_lsf_ADDR                                  (0 + CII_dtx_dec_X_BEGIN_ADDR) //20 short
#define DTX_DEC_lsf_ADDR                                    (20 + CII_dtx_dec_X_BEGIN_ADDR) //10 short
#define dtx_dec_LSP_INT_ADDR                                (30 + CII_dtx_dec_X_BEGIN_ADDR) //10 short
#define dtx_dec_LSF_INT_ADDR                                (40 + CII_dtx_dec_X_BEGIN_ADDR) //10 short
#define dtx_dec_lsf_int_variab_ADDR                         (50 + CII_dtx_dec_X_BEGIN_ADDR) //10 short
#define dtx_dec_lsp_int_variab_ADDR                         (60 + CII_dtx_dec_X_BEGIN_ADDR) //10 short
#define dtx_dec_acoeff_ADDR                                 (70 + CII_dtx_dec_X_BEGIN_ADDR) //12 short
#define dtx_dec_acoeff_variab_ADDR                          (82 + CII_dtx_dec_X_BEGIN_ADDR) //12 short
#define dtx_dec_refl_ADDR                                   (94 + CII_dtx_dec_X_BEGIN_ADDR) //10 short
#define dtx_dec_ex_ADDR                                     (104 + CII_dtx_dec_X_BEGIN_ADDR) //40 short

//CII_amr_dec_122
#define CII_amr_dec_122_X_BEGIN_ADDR                        (292 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define blank_dec                                           (0 + CII_amr_dec_122_X_BEGIN_ADDR) //106 short
#define DEC_AMR_BUFA40_ADDRESS                              (106 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP1_ADDRESS                               (107 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP2_ADDRESS                               (108 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP3_ADDRESS                               (109 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP4_ADDRESS                               (110 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP5_ADDRESS                               (111 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP6_ADDRESS                               (112 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP7_ADDRESS                               (113 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP8_ADDRESS                               (114 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP9_ADDRESS                               (115 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP10_ADDRESS                              (116 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP11_ADDRESS                              (117 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP12_ADDRESS                              (118 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP13_ADDRESS                              (119 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP14_ADDRESS                              (120 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP15_ADDRESS                              (121 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP16_ADDRESS                              (122 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP17_ADDRESS                              (123 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP18_ADDRESS                              (124 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP19_ADDRESS                              (125 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP20_ADDRESS                              (126 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP21_ADDRESS                              (127 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP22_ADDRESS                              (128 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP23_ADDRESS                              (129 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP24_ADDRESS                              (130 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP25_ADDRESS                              (131 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP26_ADDRESS                              (132 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP27_ADDRESS                              (133 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP28_ADDRESS                              (134 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP29_ADDRESS                              (135 + CII_amr_dec_122_X_BEGIN_ADDR) //11 short
#define DEC_AMR_D2PF_POS_ADDRESS                            (146 + CII_amr_dec_122_X_BEGIN_ADDR) //2 short
#define DEC_AMR_D2PF_POS2_ADDRESS                           (148 + CII_amr_dec_122_X_BEGIN_ADDR) //2 short
#define DEC_AMR_D2PF_POS4_ADDRESS                           (150 + CII_amr_dec_122_X_BEGIN_ADDR) //2 short
#define DEC_AMR_D2PF_POS6_ADDRESS                           (152 + CII_amr_dec_122_X_BEGIN_ADDR) //14 short
#define DEC_AMR_TEMP30_ADDRESS                              (166 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP31_ADDRESS                              (167 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP32_ADDRESS                              (168 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP33_ADDRESS                              (169 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP34_ADDRESS                              (170 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP35_ADDRESS                              (171 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP36_ADDRESS                              (172 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP37_ADDRESS                              (173 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP38_ADDRESS                              (174 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP39_ADDRESS                              (175 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short

//CII_prm2bits_amr_efr
#define CII_prm2bits_amr_efr_X_BEGIN_ADDR                   (388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define BIT_SERIAL_ADDR_RAM                                 (0 + CII_prm2bits_amr_efr_X_BEGIN_ADDR) //40 short

//CII_schur_recursion
#define CII_schur_recursion_Y_BEGIN_ADDR                    (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define VAD_SCHUR_RECUR_KK_ADDR_ADDR                        (0 + CII_schur_recursion_Y_BEGIN_ADDR) //9 short

//CII_compute_rav1
#define CII_compute_rav1_Y_BEGIN_ADDR                       (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define VAD_COMP_RAV1_L_WORK_ADDR                           (0 + CII_compute_rav1_Y_BEGIN_ADDR) //100 short
#define VAD_COMP_RAV1_AAV1_ADDR_ADDR                        (100 + CII_compute_rav1_Y_BEGIN_ADDR) //20 short

//CII_spectral_comparison
#define CII_spectral_comparison_Y_BEGIN_ADDR                (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define SPECTRAL_COMPARISON_SAV0_ADDR                       (0 + CII_spectral_comparison_Y_BEGIN_ADDR) //9 short

//CII_vad_computation
#define CII_vad_computation_X_BEGIN_ADDR                    (388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define VAD_COMPUTATION_ACF0_ADDR                           (0 + CII_vad_computation_X_BEGIN_ADDR) //2 short
#define VAD_COMPUTATION_PVAD_ADDR                           (2 + CII_vad_computation_X_BEGIN_ADDR) //2 short
#define VAD_COMPUTATION_L_AV0_ADDR                          (4 + CII_vad_computation_X_BEGIN_ADDR) //18 short
#define VAD_COMPUTATION_L_AV1_ADDR                          (22 + CII_vad_computation_X_BEGIN_ADDR) //18 short
#define VAD_COMPUTATION_RAV1_ADDR                           (40 + CII_vad_computation_X_BEGIN_ADDR) //9 short
#define VAD_COMPUTATION_SCAL_RAV1_ADDR                      (49 + CII_vad_computation_X_BEGIN_ADDR) //1 short
#define VAD_COMPUTATION_STAT_ADDR                           (50 + CII_vad_computation_X_BEGIN_ADDR) //1 short
#define VAD_COMPUTATION_TONE_ADDR                           (51 + CII_vad_computation_X_BEGIN_ADDR) //1 short
#define VAD_COMPUTATION_VVAD_ADDR                           (52 + CII_vad_computation_X_BEGIN_ADDR) //1 short
#define VAD_COMPUTATION_VAD_ADDR                            (53 + CII_vad_computation_X_BEGIN_ADDR) //7 short
#define VAD_PRED_VAL_VPAR_ADDR                              (60 + CII_vad_computation_X_BEGIN_ADDR) //10 short
#define VAD_PRED_VAL_AAV1_ADDR                              (70 + CII_vad_computation_X_BEGIN_ADDR) //20 short
#define VAD_COMPUTATION_A_ADDR                              (90 + CII_vad_computation_X_BEGIN_ADDR) //10 short
#define VAD_STEP_UP_L_COEF_ADDR_ADDR                        (100 + CII_vad_computation_X_BEGIN_ADDR) //9 short
#define VAD_COMPUTATION_A_ADDR_END                          (109 + CII_vad_computation_X_BEGIN_ADDR) //1 short
#define VAD_SCHUR_RECUR_PP_ADDR_ADDR                        (110 + CII_vad_computation_X_BEGIN_ADDR) //10 short
#define VAD_STEP_UP_L_WORK_ADDR_ADDR                        (120 + CII_vad_computation_X_BEGIN_ADDR) //80 short

//CII_vad1_inner_0
#define CII_vad1_inner_0_X_BEGIN_ADDR                       (388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define FILTER_BANK_tmp_buf_ADDR                            (0 + CII_vad1_inner_0_X_BEGIN_ADDR) //160 short

//CII_dtx_enc
#define CII_dtx_enc_X_BEGIN_ADDR                            (388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define DTX_ENC_LSP_ADDR                                    (0 + CII_dtx_enc_X_BEGIN_ADDR) //10 short
#define DTX_ENC_LSF_ADDR                                    (10 + CII_dtx_enc_X_BEGIN_ADDR) //10 short
#define DTX_ENC_LSP_Q_ADDR                                  (20 + CII_dtx_enc_X_BEGIN_ADDR) //10 short

//CII_Q_plsf_3
#define CII_Q_plsf_3_X_BEGIN_ADDR                           (418 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define Q_PLSF_3_LSF1_ADDR                                  (0 + CII_Q_plsf_3_X_BEGIN_ADDR) //10 short
#define Q_PLSF_3_LSF_R1_ADDR                                (10 + CII_Q_plsf_3_X_BEGIN_ADDR) //10 short
#define Q_PLSF_3_LSF1_Q_ADDR                                (20 + CII_Q_plsf_3_X_BEGIN_ADDR) //20 short
#define Q_PLSF_3_PRED_INIT_I_ADDR                           (40 + CII_Q_plsf_3_X_BEGIN_ADDR) //1 short
#define Q_PLSF_3_LOOP_I_ADDR                                (41 + CII_Q_plsf_3_X_BEGIN_ADDR) //9 short
#define Q_PLSF_3_TEMP_P_ADDR                                (50 + CII_Q_plsf_3_X_BEGIN_ADDR) //80 short
#define Q_PLSF_3_MEAN_LSF_COPY_ADDR                         (130 + CII_Q_plsf_3_X_BEGIN_ADDR) //10 short

//CII_Q_plsf_3
#define CII_Q_plsf_3_Y_BEGIN_ADDR                           (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define Q_PLSF_3_LSF_P_ADDR                                 (0 + CII_Q_plsf_3_Y_BEGIN_ADDR) //10 short
#define Q_PLSF_3_WF1_ADDR                                   (10 + CII_Q_plsf_3_Y_BEGIN_ADDR) //10 short
#define Q_PLSF_3_TEMP_R1_ADDR                               (20 + CII_Q_plsf_3_Y_BEGIN_ADDR) //80 short

//CII_lsp_122
#define CII_lsp_122_X_BEGIN_ADDR                            (388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define LSP_LSP_NEW_Q_ADDR                                  (0 + CII_lsp_122_X_BEGIN_ADDR) //10 short
#define LSP_LSP_MID_ADDR                                    (10 + CII_lsp_122_X_BEGIN_ADDR) //10 short
#define LSP_LSP_MID_Q_ADDR                                  (20 + CII_lsp_122_X_BEGIN_ADDR) //10 short

//CII_COD_AMR_122_inner_0
#define CII_COD_AMR_122_inner_0_X_BEGIN_ADDR                (388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define Pitch_fr6_max_loop_addr                             (0 + CII_COD_AMR_122_inner_0_X_BEGIN_ADDR) //2 short
#define Pitch_fr6_corr_v_addr                               (2 + CII_COD_AMR_122_inner_0_X_BEGIN_ADDR) //40 short
#define Pitch_fr6_t0_min_addr                               (42 + CII_COD_AMR_122_inner_0_X_BEGIN_ADDR) //1 short
#define Pitch_fr6_t0_max_addr                               (43 + CII_COD_AMR_122_inner_0_X_BEGIN_ADDR) //1 short
#define Pitch_delta_search_addr                             (44 + CII_COD_AMR_122_inner_0_X_BEGIN_ADDR) //1 short

//CII_COD_AMR_122_inner_0
#define CII_COD_AMR_122_inner_0_Y_BEGIN_ADDR                (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define PITCH_F6_EXCF_ADDR                                  (0 + CII_COD_AMR_122_inner_0_Y_BEGIN_ADDR) //40 short

//CII_code_10i40_35bits
#define CII_code_10i40_35bits_Y_BEGIN_ADDR                  (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define C1035PF_DN_ADDR                                     (0 + CII_code_10i40_35bits_Y_BEGIN_ADDR) //40 short
#define C1035PF_RR_DIAG_ADDR                                (40 + CII_code_10i40_35bits_Y_BEGIN_ADDR) //40 short
#define C1035PF_RR_SIDE_ADDR                                (80 + CII_code_10i40_35bits_Y_BEGIN_ADDR) //780 short

//CII_gainQuant_M122
#define CII_gainQuant_M122_Y_BEGIN_ADDR                     (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define calc_filt_energies_Y2_ADDRESS                       (0 + CII_gainQuant_M122_Y_BEGIN_ADDR) //50 short
#define gainQuant_frac_coeff_ADDRESS                        (50 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define gainQuant_exp_coeff_ADDRESS                         (60 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define MR475_gain_quant_coeff_ADDRESS                      (70 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define MR475_gain_quant_coeff_lo_ADDRESS                   (80 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define MR475_gain_quant_exp_max_ADDRESS                    (90 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define MR795_gain_quant_frac_en_ADDRESS                    (100 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define MR795_gain_quant_exp_en_ADDRESS                     (110 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define MR795_gain_quant_g_pitch_cand_ADDRESS               (120 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define MR795_gain_quant_g_pitch_cind_ADDRESS               (130 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short

//CII_Lag_max_M122
#define CII_Lag_max_M122_X_BEGIN_ADDR                       (388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define PITCH_OL_M122_SCALED_SIGNAL2_ADDR                   (0 + CII_Lag_max_M122_X_BEGIN_ADDR) //310 short

//CII_Lag_max_M122
#define CII_Lag_max_M122_Y_BEGIN_ADDR                       (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define PITCH_OL_M475_SCALED_SIGNAL_ADDR                    (0 + CII_Lag_max_M122_Y_BEGIN_ADDR) //310 short
#define PITCH_OL_M475_P_MAX1_ADDR                           (310 + CII_Lag_max_M122_Y_BEGIN_ADDR) //1 short
#define PITCH_OL_M475_MAX1_ADDR                             (311 + CII_Lag_max_M122_Y_BEGIN_ADDR) //1 short
#define PITCH_OL_M475_P_MAX2_ADDR                           (312 + CII_Lag_max_M122_Y_BEGIN_ADDR) //1 short
#define PITCH_OL_M475_MAX2_ADDR                             (313 + CII_Lag_max_M122_Y_BEGIN_ADDR) //1 short
#define PITCH_OL_M475_P_MAX3_ADDR                           (314 + CII_Lag_max_M122_Y_BEGIN_ADDR) //1 short
#define PITCH_OL_M475_MAX3_ADDR                             (315 + CII_Lag_max_M122_Y_BEGIN_ADDR) //7 short
#define PITCH_OL_M475_CORR_ADDR                             (322 + CII_Lag_max_M122_Y_BEGIN_ADDR) //300 short

//CII_set_sign12k2_122
#define CII_set_sign12k2_122_X_BEGIN_ADDR                   (388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define SET_SIGN_EN_ADDR                                    (0 + CII_set_sign12k2_122_X_BEGIN_ADDR) //230 short
#define C417PF_DN2_ADDR                                     (230 + CII_set_sign12k2_122_X_BEGIN_ADDR) //40 short

//CII_search_M122
#define CII_search_M122_X_BEGIN_ADDR                        (388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define SEARCH_10I40_RRV_ADDR                               (0 + CII_search_M122_X_BEGIN_ADDR) //40 short
#define SEARCH_CODE_CP_RR_ADDR                              (40 + CII_search_M122_X_BEGIN_ADDR) //320 short
#define SEARCH_CODE_PS0_TEMP_ADDR                           (360 + CII_search_M122_X_BEGIN_ADDR) //1 short
#define SEARCH_10I40_PS0_ADDR                               (361 + CII_search_M122_X_BEGIN_ADDR) //1 short
#define C1035PF_IPOS_ADDR                                   (362 + CII_search_M122_X_BEGIN_ADDR) //10 short
#define C1035PF_POS_MAX_ADDR                                (372 + CII_search_M122_X_BEGIN_ADDR) //10 short
#define C1035PF_CODVEC_ADDR                                 (382 + CII_search_M122_X_BEGIN_ADDR) //10 short
#define C1035PF_SIGN_ADDR                                   (392 + CII_search_M122_X_BEGIN_ADDR) //40 short

//CII_build_code_M122
#define CII_build_code_M122_X_BEGIN_ADDR                    (388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define build_code_sign_ADDR                                (0 + CII_build_code_M122_X_BEGIN_ADDR) //10 short

//CII_SEARCH_LOOP_M122
#define CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR                   (962 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define SEARCH_CODE_RRV_COE_ADDR                            (0 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_CODE_RR_COE_ADDR                             (1 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I_ADDR                                 (2 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //2 short
#define SEARCH_10I40_PSK_ADDR                               (4 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_ALPK_ADDR                              (5 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define C1035PF_RR_DIAG_ADDR_2_ADDR                         (6 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_CODE_CP_RR_ADDR_ADDR                         (7 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_CODE_DN_ADDR_ADDR                            (8 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_CODE_RRV_ADDR_ADDR                           (9 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define C1035PF_RR_SIDE_ADDR_ADDR                           (10 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define C1035PF_RR_DIAG_ADDR_ADDR                           (11 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_CODE_CP_RR_7_ADDR_ADDR                       (12 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define RR_SIDE_DIAG_TEMP_ADDR                              (13 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I2_TEMP_ADDR                           (14 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I2I3_TEMP_ADDR                         (15 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_CODE_DN_RRV_ADDR_ADDR                        (16 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //2 short
#define SEARCH_CODE_ALP0_TEMP_ADDR                          (18 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //2 short
#define SEARCH_10I40_I0_ADDR                                (20 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I1_ADDR                                (21 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I2_ADDR                                (22 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I3_ADDR                                (23 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_M74_TRACK_ADDR                               (24 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_M67_TRACK2_ADDR                              (25 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I6_ADDR                                (26 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I7_ADDR                                (27 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I8_ADDR                                (28 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I9_ADDR                                (29 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define linear_signs_ADDR                                   (30 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //4 short
#define linear_codewords_ADDR                               (34 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //4 short

//CII_build_code_com2
#define CII_build_code_com2_Y_BEGIN_ADDR                    (962 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define BUILD_CODE_POINT_ADDR_ADDR                          (0 + CII_build_code_com2_Y_BEGIN_ADDR) //10 short

//CII_Int_lpc_1and3
#define CII_Int_lpc_1and3_Y_BEGIN_ADDR                      (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define INT_LPC_LSP_ADDR                                    (0 + CII_Int_lpc_1and3_Y_BEGIN_ADDR) //10 short
#define F1_ADDR                                             (10 + CII_Int_lpc_1and3_Y_BEGIN_ADDR) //12 short
#define F2_ADDR                                             (22 + CII_Int_lpc_1and3_Y_BEGIN_ADDR) //12 short

//CII_Q_plsf_5
#define CII_Q_plsf_5_X_BEGIN_ADDR                           (418 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define Q_PLSF_5_LSF1_ADDR                                  (0 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_LSF2_ADDR                                  (10 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_WF1_ADDR                                   (20 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_WF2_ADDR                                   (30 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_LSF_R1_ADDR                                (40 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_LSF_R2_ADDR                                (50 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_LSF1_Q_ADDR                                (60 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_LSF2_Q_ADDR                                (70 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_LSF_AVER_ADDR                              (80 + CII_Q_plsf_5_X_BEGIN_ADDR) //30 short
#define Q_PLSF_5_DTX_AND_SP_ADDR                            (110 + CII_Q_plsf_5_X_BEGIN_ADDR) //1 short
#define Q_PLSF_5_DTX_AND_SID_ADDR                           (111 + CII_Q_plsf_5_X_BEGIN_ADDR) //1 short
#define Q_PLSF_5_DTX_AND_HAN_ADDR                           (112 + CII_Q_plsf_5_X_BEGIN_ADDR) //1 short
#define Q_PLSF_5_DTX_AND_PREV_HAN_ADDR                      (113 + CII_Q_plsf_5_X_BEGIN_ADDR) //1 short

//CII_Q_plsf_5
#define CII_Q_plsf_5_Y_BEGIN_ADDR                           (102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define Q_PLSF_5_LSF_P_ADDR                                 (0 + CII_Q_plsf_5_Y_BEGIN_ADDR) //10 short

/********************
 **  COMMON_LOCAL  **
 ********************/

//RAM_X: size 0x1000, used 0x0fa2   RAM_Y: size 0x0800, used 0x07e8

#endif
