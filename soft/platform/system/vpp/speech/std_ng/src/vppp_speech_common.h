#include "voc2_library.h"

#include "vppp_speech_asm_map.h"
#include "vppp_speech_const_map.h"
#include "vppp_speech_sections.h"


// VoC_directive: PARSER_ON

/* some const used in project */
#define MAX_32		 		 		 		 		 		 0x7fffffffL
#define MIN_32		 		 		 		 		 		 0x80000000L
#define MAX_16		 		 		 		 		 		 0x7fff
#define MIN_16		 		 		 		 		 		 0x8000
#define _1_2		 		 		 		 		 		 (32768L/2)
#define _1_4		 		 		 		 		 		 (32768L/4)
#define _1_8		 		 		 		 		 		 (32768L/8)
#define _1_16		 		 		 		 		 		 (32768L/16)
#define _1_32		 		 		 		 		 		 (32768L/32)
#define _1_64		 		 		 		 		 		 (32768L/64)
#define _1_128		 		 		 		 		 		 (32768L/128)
#define PIT_MIN		 		 		 		 		 		 20        /* Minimum pitch lag (all other modes)      */
#define PIT_MAX		 		 		 		 		 		 143       /* Maximum pitch lag                        */
#define MAX_PRM_SIZE		 		 		 				 57        /* max. num. of params                      */
#define MAX_SERIAL_SIZE		 		 		 				 244       /* max. num. of serial bits                 */
#define L_WINDOW		 		 		 		 			 240       /* Window size in LP analysis               */
#define L_FRAME		 		 		 		 		 		 160       /* Frame size                               */
#define L_FRAME_BY2		 		 		 		 			 80        /* Frame size divided by 2                  */
#define L_SUBFR		 		 		 		 		 		 40        /* Subframe size                            */
#define L_CODE		 		 		 		 		 		 40        /* codevector length                        */
#define NB_TRACK		 		 		 		 			 5         /* number of tracks                         */
#define STEP		 		 		 		 		 		 5         /* codebook step size                       */
#define NB_TRACK_MR102		 		 		 				 4         /* number of tracks mode mr102              */
#define STEP_MR102		 		 		 		 		     4         /* codebook step size mode mr102            */
#define M		 		 		 		 		 		 	 10        /* Order of LP filter                       */
#define MP1		 		 		 		 		 		 	 (M+1)     /* Order of LP filter + 1                   */
#define LSF_GAP		 		 		 		 		 		 205       /* Minimum distance between LSF after quan-tization; 50 Hz = 205                    */
#define NC		 		 		 		 		 		 	 M/2


/* Constants of VAD hangover addition */
#define HANGCONST		 		 		 		 			 10
#define BURSTCONST		 		 		 		 			 3

/* Constant of spectral comparison */
#define STAT_THRESH		 		 		 		 			 3670L       /* 0.056 */

/* Constants of periodicity detection */
#define LTHRESH		 		 		 		 		 		 2
#define NTHRESH		 		 		 		 		 		 4

/* Pseudo floating point representations of constants for threshold adaptation */
#define M_PTH		 		 		 		 		 		 32500          /*** 130000.0 ***/
#define E_PTH		 		 		 		 		 		 17
#define M_PLEV		 		 		 		 		 		 21667          /*** 346666.7 ***/
#define E_PLEV		 		 		 		 		 		 19
#define M_MARGIN		 		 		 		 			 16927          /*** 69333340 ***/
#define E_MARGIN		 		 		 		 			 27

#define FAC		 		 		 		 		 		 	 17203		 		 		 /* 2.1 */
#define FREQTH		 		 		 		 		 		 3189
#define PREDTH		 		 		 		 		 		 1464
#define PN_INITIAL_SEED		 		 		 				 0x70816958L   /* Pseudo noise generator seed value  */

/* Comfort noise interpolation period (nbr of frames between successive SID updates in the decoder) */
#define CN_INT_PERIOD		 		 		 				 24

/* Period when SP=1 although VAD=0.Used for comfort noise averaging */
#define DTX_HANGOVER		 		 		 				 7
#define INV_DTX_HANGOVER		 		 					 (0x7fff / DTX_HANGOVER)
#define INV_DTX_HANGOVER_P1		 		 					 (0x7fff / (DTX_HANGOVER+1))

/* Number of pulses in fixed codebook excitation */
#define NB_PULSE		 		 		 			 		 10

/* SID frame classification thresholds */
#define VALID_SID_THRESH		 		 					 2
#define INVALID_SID_THRESH		 		 					 16

/* Constant DTX_ELAPSED_THRESHOLD is used as threshold for allowing
   SID frame updating without hangover period in case when elapsed
   time measured from previous SID update is below 24 */
#define DTX_ELAPSED_THRESHOLD		 						 (24 + DTX_HANGOVER - 1)
#define DTX_HANGOVER_1		 		 				 		 (4  * DTX_HANGOVER - 1)
#define DTX_ELAPSED_THRESHOLD		 						 (24 + DTX_HANGOVER - 1)
#define DTX_HANGOVER_1_MPY10		 						 (DTX_HANGOVER-1)*M

/* Frame classification constants */
#define VALID_SID_FRAME		 		 				 		 1
#define INVALID_SID_FRAME		 					 		 2
#define GOOD_SPEECH_FRAME		 					 		 3
#define UNUSABLE_FRAME		 						 		 4

/* Encoder DTX control flags */
#define TX_SP_FLAG		 		 		 		 		 0x0001
#define TX_VAD_FLAG		 		 		 		 		 0x0002
#define TX_HANGOVER_ACTIVE		 				 		 0x0004
#define TX_PREV_HANGOVER_ACTIVE							 0x0008
#define TX_SID_UPDATE		 					 		 0x0010
#define TX_USE_OLD_SID		 					 		 0x0020

#define RX_SP_FLAG		 		 		 		 		 0x0001
#define RX_UPD_SID_QUANT_MEM							 0x0002
#define RX_FIRST_SID_UPDATE		 				 		 0x0004
#define RX_CONT_SID_UPDATE		 				 		 0x0008
#define RX_LOST_SID_FRAME		 				 		 0x0010
#define RX_INVALID_SID_FRAME							 0x0020
#define RX_NO_TRANSMISSION		 				 		 0x0040
#define RX_DTX_MUTING		 					 		 0x0080
#define RX_PREV_DTX_MUTING		 				 		 0x0100
#define RX_CNI_BFI		 		 		 		 		 0x0200
#define RX_FIRST_SP_FLAG		 				 		 0x0400

#define PITCH_OL_M475_LOOP_NUM		 		 (L_FRAME+PIT_MAX)
#define PITCH_OL_M795_LOOP_NUM		 		 (L_FRAME_BY2+PIT_MAX)
#define PITCH_OL_WGH_LOOP_NUM		 		 (L_FRAME_BY2+PIT_MAX)
#define PITCH_OL_M122_LOOP_NUM		 		 (L_FRAME_BY2+PIT_MAX)

#define DICO1_SIZE		 		 		 		 		 128
#define DICO2_SIZE		 		 		 		 		 256
#define DICO3_SIZE		 		 		 		 		 256
#define DICO4_SIZE		 		 		 		 		 256
#define DICO5_SIZE		 		 		 		 		 64



/************for ESR*****************/


#define FRAMESIZE (160) //short
#define FRACBIT 13
// --------------------
// ALGORITHM PARAMETERS
// --------------------

#define VADTAIL     (480)
#define VADHEAD     (480)


#define LOCDTHRLOG  (19)
#define DISDTHRLOG  (20)
// ---------------------
// SIMULATION PARAMETERS
// ---------------------

#define VADWINDOW   (40)
#define DELTAPRED   (VADHEAD/VADWINDOW)
#define DELTAPOST   (VADTAIL/VADWINDOW)

#define LOCDTHR      (2 << (LOCDTHRLOG + 2*(FRACBIT - 15)))
#define DISDTHR      (2 << (DISDTHRLOG + 2*(FRACBIT - 15)))

#define FILTTAPS    (41)
#define DELAY       (2*160)
#define FILESIZE    (1000*160)
#define ITERATIONS  ((FILESIZE - DELAY)/VADWINDOW)


/*****************************
 **    Common functions     **
 *****************************/

void CII_VOCODER(void);
void CII_MAIN_ENCODER(void);
void CII_MAIN_DECODER(void);
void CII_reoad_constants(void);

void CII_RunTxAudioFilter(void);
void CII_RunAntiDistorsionFilter (void);
void CII_LoadSpFormatFilter (void);
void CII_encoder_homing_frame_test(void);

void CII_NORM_L_ACC0(void);
void CII_NORM_L_ACC0_HR(void);
void CII_reload_sideinfo(void);
void CII_reload_mode(void);
void CII_DMA_READ(void);
void CII_prepare_reload(void);
void CII_DigitalGain (void);
void CII_reload_section(void);
void CII_ECHO_SUPPRESSION(void);
void CII_ECHO_CANCELLATION(void);
void CII_ECHO_MAIN(void);
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
void Coolsand_read_13bits(void);


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
void COOSAND_lsf_compressed_read(void);
void CII_Vq_subvec_lsf_compressed(void);
void Coolsand_decompressed_windows(void);
void Coolsand_If1format2amr(void);
void Coolsand_amr2If1format(void);
void Coolsand_If1_table_selet(void);
/*****************************
 **      HR functions       **
 *****************************/


void CII_HR_Encode(void);
void CII_HR_Decode(void);
void CII_speechEncoder(void);
void CII_speechDecoder_opt(void);
void CII_L_mpy_ll_opt(void);
void CII_r0Quant_opt(void);
void CII_L_mpy_ls_opt(void);
void CII_DIV_S(void);
void CII_lpcCorrQntz_opt(void);
void CII_rcToCorrDpL_opt(void);
void CII_setupPreQ(void);
void CII_getNextVec_opt(void);
void CII_flat_opt(void);
void CII_aflat_opt(void);
void CII_vad_algorithm(void);
void CII_getPnBits_opt(void);
void CII_getSfrmLpcTx_opt(void);
void CII_reset_set_values(void);
void CII_reset_set_zeros(void);
void CII_resetHREnc(void);
void CII_level_calc_opt(void);
void CII_level_estimator_opt(void);
void CII_decoderHomingFrameTest(void);
void CII_agcGain_opt(void);
void CII_lookupVq_opt(void);
void CII_resetDec(void);
void CII_res_eng_opt(void);
void CII_lpcFir_opt(void);
void CII_aToRc_opt2(void);
void CII_rcToADp_opt_new(void);
void CII_aFlatRcDp_opt( void);
void CII_b_con_opt(void);
void CII_v_con_opt(void);
void CII_scaleExcite_opt(void);
void CII_g_corr1_opt_paris(void);
void CII_rs_rrNs_opt(void);
void CII_fp_ex_opt(void);
void CII_lpcIir_opt_paris(void);
void CII_hnwFilt_opt(void);
void CII_lpcZsIir_opt(void);
void CII_g_corr2_opt(void);
void CII_decorr_opt(void);
void CII_v_srch_opt(void);
void CII_norm_s_opt(void);
void CII_sqroot_opt(void);
void CII_get_ipjj_opt_paris(void);
void CII_quantLag_opt_paris(void);
void CII_maxCCOverGWithSign_opt_paris(void);
void CII_iir_d_opt(void);
void CII_compute_s2_s3_new(void);
void CII_fnBest_CG_new(void);
void CII_r0BasedEnergyShft_new(void);
void CII_g_corr1s_new(void);
void CII_findPeak_new(void);
void CII_CGInterp_new(void);
void CII_CGInterpValid_new(void);
void CII_bestDelta_new(void);
void CII_siCPeakIndex_save_new(void);
void CII_findBestInQuantList_opt(void);
void CII_linInterpSidLoop_opt(void);
void CII_linInterpSidShort_opt(void);
void CII_lpcZsFir_opt(void);
void CII_avgGsHistQntz_gsQuant_opt(void);
void CII_aflatRecursionLoop_opt(void);
void CII_flat_recursion_opt(void);
void CII_sub_rs_rrNs(void);
void CII_step_up_opt(void);
void  CII_extra_HR_bfi_filter(void);
void CII_pitchLags_new(void);
void CII_openLoopLagSearch_new(void);
void CII_sfrmAnalysis_opt(void);



/*****************************
 **      FR functions       **
 *****************************/


void CII_encoder_reset_fr(void);
void CII_gsm_norm(void);
void CII_copy(void);
void CII_periodicity_update_fr(void);
void CII_tone_detection_fr(void);
void CII_vad_init(void);
void CII_prm2bits_fr(void);
void CII_bits2prm_fr(void);
void CII_vad_computation_fr(void);
void CII_Decoding_of_the_coded_Log_Area_Ratios_sub(void);
void CII_Decoding_of_the_coded_Log_Area_Ratios (void);
void CII_Coefficients_0_12 (void);
void CII_Coefficients_13_26 (void);
void CII_Coefficients_27_39 (void);
void CII_Coefficients_40_159 (void);
void CII_LARp_to_rp (void) ;
void CII_Short_term_analysis_filtering (void);
void CII_Short_term_synthesis_filtering ( void);
void CII_Gsm_Short_Term_Analysis_Filter (void);
void CII_Gsm_Short_Term_Synthesis_Filter (void);
void CII_Weighting_filter (void );
void CII_RPE_grid_selection (void);
void CII_APCM_quantization_xmaxc_to_exp_mant (void) ;
void CII_APCM_quantization (void);
void CII_APCM_inverse_quantization (void);
void CII_RPE_grid_positioning (void);
void CII_Gsm_RPE_Encoding (void);
void CII_Gsm_RPE_Decoding (void);
void CII_Gsm_Preprocess (void);
void CII_step_t(void);
void CII_Gsm_LPC_Analysis (void);
void CII_Gsm_Long_Term_Predictor (void);
void CII_Gsm_Long_Term_Synthesis_Filtering (void);
void CII_Gsm_Coder (void);
void CII_Gsm_Decoder (void);
void CII_FR_Decode (void);
void CII_FR_Encode(void);
void CII_tx_dtx_fr (void);
void CII_decoder_homing_frame_test (void);
void CII_rx_dtx_fr(void);
void CII_interpolate_CN_param (void);
void CII_pseudonoise_fr (void);
void CII_silence_frame_paramer(void );
void CII_lost_frame_process(void);
void CII_CN_paramer( void );
void CII_APCM_quantization_sub(void);
void CII_dtx_dec_init(void);
void reset_fr_table(void);
void CII_extra_bfi_filter(void);


void Coolsand_EsrModelMain(void);
void Coolsand_EsrFilt(void);



//aec
void Coolsand_AnalysisProcessing(void);
void Coolsand_GlobalReset(void);
void Coolsand_BackCopyVec(void);
void Coolsand_FlipVec(void);
void Coolsand_MulVec(void);
void Coolsand_ClearVec(void);
void Coolsand_AddVec(void);


void Coolsand_IFFT16(void);
void Coolsand_ReverseIndex(void);


void Coolsand_SynthesisProcessing(void);
void Coolsand_CopyVec(void);

void Coolsand_EchoCancelProcessing(void);


void Coolsand_AdaptiveFilteringAmp(void);
void Coolsand_UpdateSpkPower(void);


void Coolsand_MSBPos(void);
void Coolsand_CmplxAppxNorm_int(void);

void Coolsand_FilterUpdateCotrol_Int(void);
void Coolsand_UpdateFilterAmp(void);
void Coolsand_AEC(void);
void Coolsand_AECReset(void);
void Rda_RunDma(void);
void Coolsand_NonLinearTransform(void);

#define GLOBAL_AEC_RAMX_BACKUP_SIZE 1740
#define GLOBAL_AEC_CODE_BACKUP_SIZE 2400

#define GLOBAL_MORPH_RAMX_BACKUP_SIZE 600
#define GLOBAL_MORPH_CODE_BACKUP_SIZE 400

#define GLOBAL_NOISESUPPRESS_RAMX_BACKUP_SIZE (2000)
#define GLOBAL_NOISESUPPRESS_CODE_BACKUP_SIZE 2200

#define GLOBAL_NOISESUPPRESS_TOTAL_RAMX_BACKUP_SIZE (2000+960)

#define GLOBAL_AGC_RAMX_BACKUP_SIZE 250
#define GLOBAL_AGC_CODE_BACKUP_SIZE 1200

#define GLOBAL_AGC_TOTAL_RAMX_BACKUP_SIZE (250+96)

#define GLOBAL_AEC_DELAY_SAMPLE  0 //1~159



void Coolsand_CmplxAppxNorm(void);
void Coolsand_FirstOrderIIRSmooth2(void);

void Coolsand_NoisePSDEst(void);
void Coolsand_ShrinkBit(void);
void Coolsand_ComputeNoiseWeight_DeciDirectCTRL(void);
void Coolsand_Generate(void);
void Coolsand_PostFiltering(void);
void Coolsand_Agc(void);
void Coolsand_Lin2Log(void);



#define PF_CL_CHNLNUM 2

void Coolsand_SumInstantPower(void);
void Coolsand_InstantPower(void);
void Coolsand_ControlLogicMergeChnlEnhancedDTD(void);
void Coolsand_ControlLogicSingleChnlEnhanced(void);
void Coolsand_ControlLogicChnlDecisionEnhanced(void);
void Coolsand_ControlLogicCalVarSub(void);
void Coolsand_SmoothPeakPower(void);
void Coolsand_InstantSNR(void);
void Coolsand_OverallNoiseEst(void);

void Coolsand_OverallNoiseEstwState(void);
void Cooland_InstantPower(void);

void Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL(void);

void Coolsand_EchoPSDEst_MFCOHFFix(void);



void Coolsand_ControlLogicDTD(void);
void Coolsand_CmplxMul(void);
void Coolsand_Double_FirstOrderIIRSmooth2(void);




#define GLOBAL_AEC_CONSTX_SIZE (64+64+4+4+16)

#define PROTOTYPE_FILTER_LEN 128
#define SUBBAND_CHANNEL_NUM  16
#define SUBSAMPLING_RATE  (SUBBAND_CHANNEL_NUM/2)

#define TOTAL_FILTER_LEN  128
#define FILTER_LEN  (TOTAL_FILTER_LEN/SUBBAND_CHANNEL_NUM)
#define SUBBANDFILTER_NUM  (SUBBAND_CHANNEL_NUM/2+1)


#define DEF_DATA_BLOCK_SIZE (SUBBAND_CHANNEL_NUM/2)

#define CNGTapLen 4
#define CNGRegLen 32


#define g_PE_PPFilterLen 5


#define GLOBAL_MORPHVOICE_CONST_SIZE 81
void CS_intsmbFft(void);
void CII_smbPitchShift(void);
void Coolsand_MorphVoiceReset(void);




void Coolsand_Morph(void);
void Coolsand_Aec(void);
void Coolsand_NoiseSuppresser(void);
void Coolsand_HighPassFilter(void);
void Coolsand_NotchFilter(void);

void Coolsand_NonClipDigitalGain(void);

void Rda_ProcessMic(void);
void Rda_NoiseSuppress(void);
void Rda_SpeexMicNoiseSuppresser(void);
void Rda_SpeexReceiverNoiseSuppresser(void);


void RDA_DIV32_S(void);
void RDA_Pitch_Filter(void);

#define IIR_DATA_SHIFT  13
void Rda_ProcessIIR(void);

void Rda_MicAgc(void);
void Rda_ReceiverAgc(void);
void Agc1_Main(void);
void Agc_ProcessDigital(void);
void Rda_ProcessWebRtcAgc(void);


