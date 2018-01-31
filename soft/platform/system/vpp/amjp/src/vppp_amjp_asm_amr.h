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


#define AMR_Dec_ConstX_size 0
#define AMR_Dec_ConstY_size 2668+124

// VoC_directive: PARSER_OFF
/*****************************
 **    Common functions     **
 *****************************/

void vpp_AmjpCircBuffDmaRd(void);

void test();
void CII_MP4DEC_MAIN(void);
void CII_MRRING_Decode(void);
void CII_MAIN_ENCODER(void);
void CII_MAIN_DECODER(void);


void CII_RunTxAudioFilter(void);
void CII_RunAntiDistorsionFilter (void);
void CII_LoadSpFormatFilter (void);
void CII_encoder_homing_frame_test(void);
void clear_stall_pipe (void);
void CII_NORM_L_ACC0(void);
void CII_NORM_L_ACC0_HR(void);
void CII_reload_sideinfo(void);
void CII_reload_mode(void);
void CII_prepare_reload(void);
void CII_DigitalGain (void);
//void CII_DigitalRamp(void);
void CII_reload_section(void);

/*****************************
 **   AMR/EFR functions     **
 *****************************/

void CII_AMR_Decode(void);
void CII_AMR_Encode(void);
void CII_energy_new(void);
void CII_agc(void);
void CII_decode_2i40_11bits(void);
void CII_decode_3i40_14bits(void);
void CII_Reorder_lsf(void);
void CII_D_plsf_3(void);
void CII_D_plsf_5_sub(void);
void CII_Int_lsf(void);
void CII_ec_gain_code (void);
void CII_ec_gain_code_update (void);
void CII_ec_gain_pitch (void);
void CII_ec_gain_pitch_update (void);
void CII_dtx_com(void);
void CII_dtx_com_s2(void);
void CII_bits2prm_amr_efr (void);
void CII_Decoder_amr (void);
void CII_Speech_Decode_Frame(void);
void CII_dtx_dec(void);
void CII_reset_rx_dtx(void);
void CII_rx_dtx(void);
void CII_dec_reset(void);
void CII_Post_Filter_reset (void);
void CII_R02R3_p_R6(void);
void CII_Inv_sqrt(void);
void CII_Lsp_lsf (void);
void CII_Lsf_lsp(void);
void CII_Log2_norm(void);
void CII_Pred_lt_3or6 (void);
void CII_Lsp_Az(void);
void CII_build_code_com2(void);
void CII_lpc_M122(void);
void CII_Int_lpc_1and3(void);
void CII_Int_lpc_1to3(void);
void CII_dtx_buffer(void);
void CII_dtx_enc(void);
void CII_lsp_M122(void);
void CII_Q_plsf_3(void);
void CII_check_lsp(void);
void CII_Pitch_ol_M475_515(void);
void CII_Pitch_ol_M122(void);
void CII_Lag_max_M122(void);
void CII_Pitch_ol_wgh(void);
void CII_gmed_n(void);
void CII_Lag_max_M102(void);
void CII_SEARCH_COPY(void);
void CII_subframePreProc(void);
void CII_subframePostProc(void);
void CII_copy(void);
void CII_copy_xy(void);
void CII_scale(void);
void CII_scale_xy(void);
void CII_Autocorr(void);
void CII_Lag_window(void);
void CII_Levinson (void);
void CII_Az_lsp(void);
void CII_Q_plsf_5(void);
void CII_Lsf_wt (void);
void CII_Vq_subvec (void);
void CII_MR475_gain_quant_subr2_475(void);
void CII_MR475_gain_quant_subr2_102(void);
void CII_MR475_gain_quant_subr2_515(void);
void CII_MR475_gain_quant_subr2_59(void);
void CII_MR475_gain_quant_subr2_74(void);
void CII_Pre_Process(void);
void CII_Weight_Ai(void);
void CII_Residu_new(void);
void CII_Syn_filt(void);
void CII_Convolve(void);
void CII_G_pitch(void);
void CII_q_gain_code(void);
void CII_CN_encoding(void);
void CII_sid_codeword_encoding(void);
void CII_EFR_LEVINSON_PART (void);
void CII_Chebps (void);
void CII_cor_h(void);
void CII_cor_h_x (void);
void CII_set_sign_475 (void);
void CII_set_sign_515 (void);
void CII_set_sign_59 (void);
void CII_set_sign_67 (void);
void CII_set_sign_74 (void);
void CII_pseudonoise (void);
void CII_Int_lpc_only(void);
void CII_Int_lpc_only_2(void);
void CII_DIV_32(void);
void CII_update_gcode0_CN (void);
void CII_Pow2 (void);
void CII_Log2 (void);
void CII_code_2i40_9bits_475(void);
void CII_code_2i40_9bits_515(void);
void CII_sqrt_l_exp (void);
void CII_calc_target_energy(void);
void CII_MR795_gain_quant(void);
void CII_cbseach_subr1(void);
void CII_search_4i40(void);
void CII_COMPUTE_ALP1_M74(void);
void CII_COMPUTE_ALP1_M74_795(void);
void CII_cbseach_cala_track_475(void);



void CII_search_M475_475(void);
void CII_search_M475_515(void);
void CII_build_code_M475_475(void);
void CII_build_code_M475_515(void);
void CII_code_2i40_11bits(void);
void CII_search_M59(void);
void CII_build_code_MR59(void);
void CII_code_3i40_14bits(void);
void CII_search_3i40(void);
void CII_build_code_MR67(void);
void CII_code_8i40_31bits (void);
void CII_set_sign12k2_102 (void);
void CII_set_sign12k2_122 (void);
void CII_COMPUTE_RRV_M102(void);
void CII_COMPUTE_RRV_M122(void);
void CII_COMPUTE_ALP1_M102(void);
void CII_COMPUTE_ALP1_M122(void);
void CII_SEARCH_LOOP_M102(void);
void CII_SEARCH_LOOP_M122(void);
void CII_build_code_M102 (void);
void CII_code_10i40_35bits (void);
void CII_build_code_M122 (void);
void CII_search_M122(void);
void CII_gc_pred_update(void);
void CII_gc_pred(void);
void CII_gainQuant_M475(void);
void CII_gainQuant_M122(void);
void CII_calc_filt_energies_102(void);
void CII_calc_filt_energies_475(void);
void CII_calc_filt_energies_515(void);
void CII_calc_filt_energies_59(void);
void CII_calc_filt_energies_67(void);
void CII_calc_filt_energies_74(void);
void CII_L_shr_r(void);
void CII_Copy_M(void);
void CII_gainQuant_102(void);
void CII_gainQuant_515(void);
void CII_gainQuant_59(void);
void CII_gainQuant_67(void);
void CII_gainQuant_74(void);
void CII_gainQuant_795(void);
void CII_calc_filt_energies_subr_102(void);
void CII_calc_filt_energies_subr_475(void);
void CII_calc_filt_energies_subr_515(void);
void CII_calc_filt_energies_subr_59(void);
void CII_calc_filt_energies_subr_67(void);
void CII_calc_filt_energies_subr_74(void);
void CII_calc_filt_energies_subr_795(void);
void CII_Int_lpc_1and3_dec(void);
void CII_mr475_quant_sub(void);
void CII_MR475_quant_store_results(void);
void CII_code_4i40_17bits_74(void);
void CII_code_4i40_17bits_795(void);
void CII_cbseach_cala_cod_475(void);
void CII_cbseach_cala_cod_515(void);
void CII_cbseach_cala_cod_59(void);
void CII_cbseach_cala_cod_74(void);
void CII_cbseach_clear_CODE_475(void);
void CII_cbseach_clear_CODE_515(void);
void CII_cbseach_clear_CODE_59(void);
void CII_cbseach_clear_CODE_74(void);
void CII_cbseach_clear_CODE_102(void);
void CII_Pitch_ol_M795_59(void);
void CII_Pitch_ol_M795_67(void);
void CII_Pitch_ol_M795_74(void);
void CII_Pitch_ol_M475_515(void);
void CII_Pitch_ol_M475_475(void);
void CII_Lag_max_M475_475(void);
void CII_Lag_max_M475_515(void);
void CII_Lag_max_M475_59(void);
void CII_Lag_max_M475_67(void);
void CII_Lag_max_M475_74(void);
void CII_Lag_max_M475_795(void);
void CII_COMPUTE_ALP1_M67_475(void);
void CII_COMPUTE_ALP1_M67_515(void);
void CII_COMPUTE_ALP1_M67_59(void);
void CII_MR475_gain_quant_subr_475(void);
void CII_MR475_gain_quant_subr_515(void);
void CII_MR475_gain_quant_subr_59(void);
void CII_MR475_gain_quant_subr_67(void);
void CII_MR475_gain_quant_subr_74(void);
void CII_MR475_gain_quant_subr_102(void);
void CII_search_M102(void);
void CII_COD_AMR_475(void);
void CII_COD_AMR_515(void);
void CII_COD_AMR_59(void);
void CII_COD_AMR_67(void);
void CII_COD_AMR_74(void);
void CII_COD_AMR_795(void);
void CII_COD_AMR_102(void);
void CII_COD_AMR_122(void);
void CII_tx_dtx_handler(void);
void CII_encoder_reset (void);
void CII_vad1(void);
void CII_reset_rx_dtx(void);
void CII_update_lsf_p_CN(void);
void CII_interpolate_CN_lsf(void);
void CII_update_lsf_history(void);
void CII_build_CN_code(void);
void CII_interpolate_CN_param (void);
void CII_update_gain_code_history_rx (void);
void CII_rx_dtx_handler(void);
void CII_efr_dtx_func1(void);
void CII_efr_dtx_func2(void);
void CII_efr_dtx_func3(void);
void CII_efr_dtx_func4(void);
void CII_efr_dtx_func5(void);
void CII_efr_dtx_func6(void);
void CII_efr_dtx_func7(void);
void CII_efr_dtx_func8(void);
void CII_efr_dtx_func9(void);
void CII_efr_dtx_func11(void);
void CII_amr_dec_com_sub1(void);
void CII_amr_dec_com_sub3(void);
void CII_amr_dec_com_sub4(void);
void CII_amr_dec_com_sub5(void);
void CII_amr_dec_475(void);
void CII_amr_dec_515(void);
void CII_amr_dec_59(void);
void CII_amr_dec_67(void);
void CII_amr_dec_74(void);
void CII_amr_dec_795(void);
void CII_amr_dec_102(void);
void CII_amr_dec_122(void);
void CII_schur_recursion (void);
void CII_step_up (void);
void CII_compute_rav1 (void);
void CII_vad_computation (void);
void CII_tx_dtx(void);
void CII_aver_lsf_history(void);
void CII_update_lsf_p_CN(void);
void CII_periodicity_update(void);
void CII_compute_CN_excitation_gain(void);
void CII_update_gain_code_history_tx(void);
void CII_vad_pitch_detection(void);
void CII_PRM2_BITS_POST_PROCESS(void);
void CII_PRM2_BITS_POST_PROCESS_EFR(void);
void CII_EFR_DTX_PART1(void);
void CII_plsf5_dtx1(void);
void CII_prm2bits_amr_efr(void);
void CII_EFR_DTX_ENC_RESET(void);
void CII_AMR_subfunc0(void);
void CII_Dec_lag3_67(void);
void CII_Dec_lag3_59(void);
void CII_Dec_lag3_515(void) ;
void CII_gc_pred_M122_dec(void);
void CII_plsf5_dtx2(void);
void CII_plsf5_dtx3(void);
void CII_plsf5_dtx4(void);
void CII_decode_2i40_9bits_515(void);
void CII_lsp_795(void);
void CII_decode_4i40_17bits_795(void);
void CII_Enc_lag3_74(void);
void CII_lsp_74(void);
void CII_decode_4i40_17bits_74(void);
void CII_Enc_lag3_67(void);
void CII_Enc_lag3_59(void);
void CII_lsp_59(void);
void CII_Enc_lag3_515(void);
void CII_Enc_lag3_475(void);
void CII_lsp_475(void);
void CII_lsp_122(void);
void CII_lsp_67(void);
void CII_lsp_515(void);
void CII_Enc_lag3_102(void);
void CII_lsp_102(void);
void CII_dtx_efr_dec_reset(void);
void CII_SUBFRAME_EFR_PART(void);
void CII_RESET_AMR_CONSTANT(void);
void CII_Post_Process(void);

// VoC_directive: PARSER_ON



