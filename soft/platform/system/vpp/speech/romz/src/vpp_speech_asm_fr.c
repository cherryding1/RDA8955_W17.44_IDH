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




#include "vppp_speech_common.h"

#if 0
voc_short STATIC_ENC_GSM_STATE_ADDR,280,x
voc_short STATIC_ENC_GSM_STATE_Z1_ADDR,2,x
voc_short STATIC_ENC_GSM_STATE_L_Z2_ADDR,2,x
voc_short STATIC_ENC_GSM_STATE_MP_ADDR,2,x
voc_short STATIC_ENC_GSM_STATE_U_ADDR,8,x
voc_short STATIC_ENC_GSM_STATE_LARPP_ADDR,16,x
voc_short STATIC_ENC_GSM_STATE_J_ADDR,1,x
voc_short STATIC_ENC_GSM_STATE_NRP_ADDR,1,x
voc_short STATIC_ENC_GSM_STATE_V_ADDR,9,x
voc_short STATIC_ENC_GSM_STATE_MSR_ADDR,x
voc_short STATIC_ENC_GSM_STATE_VERBOSE_ADDR,x
voc_short STATIC_ENC_GSM_STATE_FAST_ADDR,x
voc_short GLOBAL_ENC_LARC_ADDR,8,x
voc_short GLOBAL_ENC_NC_ADDR,4,x
voc_short GLOBAL_ENC_MC_ADDR,4,x
voc_short GLOBAL_ENC_BC_ADDR,4,x
voc_short GLOBAL_ENC_XMAXC_ADDR,4,x
voc_short GLOBAL_ENC_XMC_ADDR,52,x
#endif

#if 0
voc_short STATIC_ENC_E_ADDR,50,y

#endif

#if 0
voc_short STATIC_DEC_GSM_STATE_DP0_ADDR,280,x
voc_short STATIC_DEC_GSM_STATE_Z1_ADDR,2,x
voc_short STATIC_DEC_GSM_STATE_L_Z2_ADDR,2,x
voc_short STATIC_DEC_GSM_STATE_MP_ADDR,2,x
voc_short STATIC_DEC_GSM_STATE_U_ADDR,8,x
voc_short STATIC_DEC_GSM_STATE_LARPP_ADDR,16,x
voc_short STATIC_DEC_GSM_STATE_J_ADDR,x
voc_short STATIC_DEC_GSM_STATE_NRP_ADDR,x
voc_short STATIC_DEC_GSM_STATE_V_ADDR,9,x
voc_short STATIC_DEC_GSM_STATE_MSR_ADDR,x
voc_short STATIC_DEC_GSM_STATE_VERBOSE_ADDR,x
voc_short STATIC_DEC_GSM_STATE_FAST_ADDR,x
voc_short GLOBAL_DEC_LARC_ADDR,8,x
voc_short GLOBAL_DEC_NC_ADDR,4,x
voc_short GLOBAL_DEC_MC_ADDR,4,x
voc_short GLOBAL_DEC_BC_ADDR,4,x
voc_short GLOBAL_DEC_XMAXC_ADDR,4,x
voc_short GLOBAL_DEC_XMC_ADDR,52,x
#endif
#if 0
voc_short STATIC_VAD_pswRvad_ADDR,9,x
voc_short STATIC_VAD_tone_ADDR,x
voc_short STATIC_VAD_swNormRvad_ADDR,x
voc_short STATIC_VAD_swPt_sacf_ADDR,x
voc_short STATIC_VAD_swPt_sav0_ADDR,x
voc_short STATIC_VAD_swE_thvad_ADDR,x
voc_short STATIC_VAD_swM_thvad_ADDR,x
voc_short STATIC_VAD_swAdaptCount_ADDR,x
voc_short STATIC_VAD_swBurstCount_ADDR,x
voc_short STATIC_VAD_swHangCount_ADDR,x
voc_short STATIC_VAD_swOldLagCount_ADDR,x
voc_short STATIC_VAD_swVeryOldLagCount_ADDR,x
voc_short STATIC_VAD_swOldLag_ADDR,2,x
voc_short STATIC_VAD_pL_sacf_ADDR,54,x
voc_short STATIC_VAD_pL_sav0_ADDR,72,x
voc_short STATIC_VAD_L_lastdm_ADDR,2,x
voc_short STATIC_VAD_SCALVAD_ADDR,4,x
#endif

#if 0
voc_short STATIC_CONST_GSM_DLB_ADDR,4,x
voc_short STATIC_CONST_GSM_QLB_ADDR,4,x
voc_short STATIC_CONST_GSM_NRFAC_ADDR,8,x
voc_short STATIC_CONST_GSM_FAC_ADDR,8,x
voc_short DEC_HOMING_LPC_PARM_ADDR,8,x
voc_short STATIC_CONST_GSM_H_ADDR,12,x
voc_short STATIC_TABLE_HANN_ADDR,80,x
#endif

#if 0
voc_short GLOBAL_dtx_begin_BLANK,y
voc_short GLOBAL_subfrm_ADDR,2,y
voc_short GLOBAL_txdtx_ctrl_ADDR,y
voc_short GLOBAL_txdtx_hangover_ADDR,y
voc_short GLOBAL_txdtx_N_elapsed_ADDR,y
voc_short GLOBAL_lost_speech_frames_ADDR,y
voc_short GLOBAL_rxdtx_aver_period_ADDR,y
voc_short GLOBAL_rxdtx_N_elapsed_ADDR,y
voc_short GLOBAL_rxdtx_ctrl_ADDR,y
voc_short GLOBAL_rx_dtx_state_ADDR,y
voc_short GLOBAL_prev_SID_frames_lost_ADDR,y
voc_short GLOBAL_L_pn_seed_rx_ADDR,8,y
voc_short GLOBAL_LAR_SID_tx_ADDR,32,y
voc_short GLOBAL_xmax_SID_tx_ADDR,16,y
voc_short GLOBAL_LARc_old_tx_ADDR,8,y
voc_short GLOBAL_xmaxc_old_tx_ADDR,4,y
voc_short GLOBAL_LARc_old_rx_ADDR,8,y
voc_short GLOBAL_Nc_old_rx_ADDR,4,y
voc_short GLOBAL_Mc_old_rx_ADDR,4,y
voc_short GLOBAL_bc_old_rx_ADDR,4,y
voc_short GLOBAL_xmaxc_old_rx_ADDR,4,y
voc_short GLOBAL_xmc_old_rx_ADDR,52,y
voc_short GLOBAL_LARc_old_2_rx_ADDR,8,y
voc_short GLOBAL_xmaxc_old_2_rx_ADDR,4,y
voc_short GLOBAL_LARc_old_SID_rx_ADDR,8,y
voc_short GLOBAL_LARc_old_2_SID_rx_ADDR,8,y
voc_short GLOBAL_xmaxc_old_SID_rx_ADDR,4,y
voc_short GLOBAL_xmaxc_old_2_SID_rx_ADDR,4,y
#endif

#if 0
voc_short VAD_ALGORITHM_swStat_ADDR,y
voc_short VAD_ALGORITHM_swNormRav1_ADDR,y
voc_short VAD_ALGORITHM_pswRav1_ADDR,10,y
voc_short VAD_ALGORITHM_pL_av0_ADDR,18,y
voc_short VAD_ALGORITHM_pL_av1_ADDR,18,y
voc_short VAD_ALGORITHM_swM_pvad_ADDR,y
voc_short VAD_ALGORITHM_swE_pvad_ADDR,y
voc_short VAD_ALGORITHM_swM_acf0_ADDR,y
voc_short VAD_ALGORITHM_swE_acf0_ADDR,y
voc_short VAD_PREDICTOR_VALUES_pswAav1_ADDR,10,y
voc_short VAD_PREDICTOR_VALUES_pswVpar_ADDR,8,y
voc_short VAD_STEP_UP_pL_coef_ADDR,18,y
voc_short VAD_STEP_UP_pL_work_ADDR,18,y
voc_short VAD_COMPUTE_RAV1_pL_work_ADDR,18,y
voc_short VAD_TONE_DETECTION_pswA_ADDR,4,y
voc_short SCHUR_RECURSION_pswPp_ADDR,10,y
voc_short SCHUR_RECURSION_pswKk_ADDR,10,y
#endif
void CII_FR_Encode (void)
{
    VoC_lw32_d(REG01,CONST_0x00008000_ADDR);

    VoC_push16(RA,DEFAULT);
    VoC_push32(REG01,DEFAULT);

    VoC_bnez16_r(gsm_encode_reset,REG2)

    //CII_encoder_homing_frame_test
    VoC_lw16d_set_inc(REG0,ENC_INPUT_ADDR_ADDR,2);
    VoC_jal(CII_encoder_homing_frame_test);

    VoC_bez16_r(FR_label_100,REG4)

gsm_encode_reset:

    VoC_jal(CII_encoder_reset_fr);
    VoC_jal(CII_vad_init);

    // inline  void CII_dtx_init(void)
    // only set non zero value  other all have set  as 0 send
    VoC_lw16i_short(REG0,4,DEFAULT);               // DTX_HANGOVER
    VoC_lw16i_short(REG3,3,IN_PARALLEL);           // TX_SP_FLAG | TX_VAD_FLAG
    VoC_lw16i(REG1,0x7fff);
    VoC_sw16_d(REG3,GLOBAL_txdtx_ctrl_ADDR);
    VoC_sw32_d(REG01,GLOBAL_txdtx_hangover_ADDR);  // GLOBAL_txdtx_hangover_ADDR  and  GLOBAL_txdtx_N_elapsed_ADDR


FR_label_100:

    // ****************
    // INPUT PARMS:
    // REG1 : enc_in,  INC1 = 1
    // REG4 : echo_on (0 -> off, 1 -> on, 2 -> on & reset)
    // REG6 : DoubleTalk
    // ****************


    VoC_jal(CII_Gsm_Coder);     //Gsm_Coder(s, source, LARc, Nc, bc, Mc, xmaxc, xmc);

    VoC_lw16i_set_inc(REG0, GLOBAL_ENC_LARC_ADDR,2);
    VoC_lw16i_set_inc(REG1, GLOBAL_LARc_old_tx_ADDR,2);
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_lw16_d(REG4, GLOBAL_txdtx_ctrl_ADDR);
    VoC_movreg16(REG5, REG4,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_sw16_d(REG3,GLOBAL_OUTPUT_SP_ADDR);     // sp_flag=1;
    VoC_and16_rr(REG4, REG3,DEFAULT);
    VoC_bnez16_r(gsm_enc_label_100, REG4)      //   if ((txdtx_ctrl & TX_SP_FLAG) == 0)
    VoC_lw16i_set_inc(REG3, GLOBAL_ENC_XMC_ADDR,2);
    VoC_sw16_d(ACC1_HI,GLOBAL_OUTPUT_SP_ADDR);      // sp_flag=0;
    VoC_loop_i_short(26,DEFAULT)
    VoC_startloop0
    VoC_sw32inc_p(ACC1, REG3,DEFAULT);
    VoC_endloop0

    VoC_and16_ri(REG5, 0x0010);   //    //if ((txdtx_ctrl & TX_SID_UPDATE) != 0)
    VoC_bez16_r(gsm_enc_label_200, REG5)

    VoC_lw16_d(REG4, GLOBAL_ENC_XMAXC_ADDR);
    VoC_loop_i_short(4,DEFAULT);
    VoC_lw32inc_p(ACC0, REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0, REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0, REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_sw16_d(REG4, GLOBAL_xmaxc_old_tx_ADDR);
    VoC_jump(gsm_enc_label_100);
gsm_enc_label_200:
    VoC_lw32inc_p(ACC0, REG1,DEFAULT);

    VoC_lw16_d(REG4, GLOBAL_xmaxc_old_tx_ADDR);
    VoC_lw16i_set_inc(REG2, GLOBAL_ENC_XMAXC_ADDR,1);

    exit_on_warnings=OFF;
    VoC_loop_i_short(4,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(ACC0, REG1,DEFAULT);
    VoC_sw32inc_p(ACC0, REG0,DEFAULT);
    VoC_sw16inc_p(REG4, REG2,DEFAULT);
    VoC_endloop0
    exit_on_warnings=ON;
gsm_enc_label_100:
    VoC_jal(CII_prm2bits_fr);
    VoC_pop16(RA,DEFAULT);
    VoC_pop32(ACC1,DEFAULT);

    VoC_return;

}


void CII_prm2bits_fr(void)
{
    /*used to add magic bit
    VoC_lw32_d(REG01,CONST_1_ADDR);
    VoC_lw16i_set_inc(REG3,BIT_PARA_TEMP_ADDR-4,1);
    VoC_NOP();
    VoC_sw16inc_p(REG0,REG3,DEFAULT);
    VoC_sw16inc_p(REG1,REG3,DEFAULT);
    VoC_sw16inc_p(REG0,REG3,DEFAULT);
    VoC_sw16inc_p(REG0,REG3,DEFAULT);
    end */


    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_lw16i_set_inc(REG0,GLOBAL_ENC_LARC_ADDR,1);
    VoC_lw16i_set_inc(REG1,BIT_PARA_TEMP_ADDR,1);
    VoC_lw16i_set_inc(REG2,6,-1);

PRM2BIT_L1:
    VoC_blt16_rd(PRM2BIT_L2,REG2,CONST_3_ADDR);
    VoC_loop_i(1,2);
    VoC_lw16inc_p(REG4,REG0,DEFAULT);

    VoC_loop_r_short(REG2,DEFAULT)
    VoC_movreg16(REG5,REG4,IN_PARALLEL);
    VoC_startloop0
    VoC_and16_rr(REG5,REG7,DEFAULT);
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);
    VoC_endloop0;
    VoC_NOP();
    VoC_endloop1;
    VoC_inc_p(REG2,DEFAULT);
    VoC_jump(PRM2BIT_L1);
PRM2BIT_L2:

    VoC_lw16i_set_inc(REG0,GLOBAL_ENC_XMC_ADDR,1);
    VoC_lw16i_set_inc(REG2,GLOBAL_ENC_NC_ADDR,1);
    VoC_lw16i_set_inc(REG3,GLOBAL_ENC_BC_ADDR,1);
    VoC_lw16i(REG4,GLOBAL_ENC_MC_ADDR);
    VoC_lw16i(REG5,GLOBAL_ENC_XMAXC_ADDR);
    VoC_movreg32(RL6,REG45,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    VoC_loop_i(1,4)
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    VoC_loop_i_short(7,DEFAULT)
    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    VoC_startloop0
    VoC_and16_rr(REG7,REG5,DEFAULT);
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    VoC_endloop0;

    VoC_lw16inc_p(REG6,REG3,DEFAULT);
    VoC_movreg32(REG45,RL6,IN_PARALLEL);

    VoC_loop_i_short(2,DEFAULT)
    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    VoC_startloop0
    VoC_and16_rd(REG7,CONST_1_ADDR);
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    VoC_endloop0;

    VoC_lw16_p(REG6,REG4,DEFAULT);
    VoC_loop_i_short(2,DEFAULT)
    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    VoC_startloop0
    VoC_and16_rd(REG7,CONST_1_ADDR);
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    VoC_endloop0;

    VoC_lw16_p(REG6,REG5,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    VoC_loop_i_short(6,DEFAULT)
    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    VoC_startloop0
    VoC_and16_rr(REG7,REG5,DEFAULT);
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    VoC_endloop0;

    VoC_loop_i_short(13,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_movreg16(REG7,REG6,DEFAULT);
    VoC_and16_rr(REG7,REG5,DEFAULT);
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);

    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    VoC_and16_rr(REG7,REG5,DEFAULT);
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);

    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    VoC_and16_rr(REG7,REG5,DEFAULT);
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0

    VoC_add32_rd(RL6,RL6,CONST_0x00010001_ADDR);
    VoC_endloop1

    VoC_lw16i_set_inc(REG0,OUTPUT_BITS2_ADDR,1);
    VoC_add16_rd(REG0,REG0,CONST_4_ADDR);
    VoC_lw16i_set_inc(REG1,BIT_PARA_TEMP_ADDR,1);       // if no magic bit the address should be BIT_PARA_TEMP_ADDR
    VoC_lw16i_set_inc(REG3,1,1);

    VoC_lw16i_short(INC2,-1,DEFAULT);

    VoC_loop_i(1,17)
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_loop_i_short(16,DEFAULT)
    VoC_lw16i_short(REG2,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_shr32_rr(REG45,REG2,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_or16_rr(REG7,REG4,DEFAULT);
    VoC_bngt16_rd(PRM2BIT_001,REG3,CONST_260_ADDR)
    VoC_jump(PRM2BIT_002);
PRM2BIT_001:
    VoC_inc_p(REG2,DEFAULT);
    VoC_endloop0
PRM2BIT_002:
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    VoC_endloop1

    VoC_return;
}



// ACC1= 0x00008000
void CII_Gsm_Coder (void)
{
#if 0
    voc_short LOCAL_ENC_SO_ADDR,160,x
    voc_short LOCAL_ENC_L_ACF_ADDR,18,x
    voc_short LOCAL_ENC_X_ADDR,40,x
    voc_short LOCAL_ENC_XM_ADDR,14,x
    voc_short LOCAL_ENC_XMP_ADDR,14,x
    voc_short LOCAL_DEC_XMP_ADDR,14,x
    voc_short LOCAL_ENC_ACF_ADDR,10,x
    voc_short LOCAL_ENC_P_ADDR,10,x
    voc_short LOCAL_ENC_K_ADDR,10,x
#endif

#if 0
    voc_short LOCAL_ENC_WT_ADDR,40,y
    voc_short LOCAL_ENC_LARP_ADDR,100,y
#endif

    VoC_push16(RA,DEFAULT);

    VoC_lw16d_set_inc(REG0,ENC_INPUT_ADDR_ADDR,1);
    VoC_lw16i_set_inc(REG1,LOCAL_ENC_SO_ADDR,1);
    VoC_lw16i_set_inc(REG2,GLOBAL_ENC_LARC_ADDR,1);
    VoC_push16(REG7,DEFAULT);       // loop_k
    VoC_push16(REG0,DEFAULT);       // source
    VoC_push16(REG1,DEFAULT);       // so
    VoC_push16(REG2,DEFAULT);       // LARc

// stack16 ra loop_k source s0 LARc
// input : reg0->s   reg1->so  ACC1= 0x00008000
    VoC_jal(CII_Gsm_Preprocess);            //Gsm_Preprocess(S, s, so);

// stack16 loop_k ra source s0 LARc
// stack32 0x00008000
    VoC_jal(CII_Gsm_LPC_Analysis);          //Gsm_LPC_Analysis(S, so, LARc);

    VoC_jal(CII_Gsm_Short_Term_Analysis_Filter);    //Gsm_Short_Term_Analysis_Filter(S, LARc, so);

    VoC_lw16i_set_inc(REG0,GLOBAL_ENC_XMC_ADDR,1);
    VoC_lw16i_set_inc(REG1,STATIC_ENC_GSM_STATE_ADDR+120,1);
    VoC_lw16i_set_inc(REG2,GLOBAL_ENC_NC_ADDR,1);
    VoC_lw16i_set_inc(REG3,GLOBAL_ENC_BC_ADDR,1);
    VoC_sw16_sd(REG0,0,DEFAULT);    // xMc
    VoC_push16(REG1,DEFAULT);   // dp
    VoC_push16(REG1,DEFAULT);   // dpp
    VoC_push16(REG2,DEFAULT);   // Nc
    VoC_push16(REG3,DEFAULT);   // bc
    VoC_lw16i_set_inc(REG2,GLOBAL_ENC_XMAXC_ADDR,1);
    VoC_lw16i_set_inc(REG3,GLOBAL_ENC_MC_ADDR,1);
    VoC_push16(REG2,DEFAULT);   // xmaxc
    VoC_push16(REG3,DEFAULT);   // Mc
    VoC_lw16i_short(REG7,-1,IN_PARALLEL);

//stack16  ra loop_k source so xMc dp dpp &Nc[k] &bc[k] &xmaxc[k] &Mc[k]
    //VoC_lw16_sd(REG7,9,DEFAULT);
Gsm_Coder_Loop_start:
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);           //loop_k
    VoC_bngt16_rd(Gsm_Coder_Loop_start2,REG7,CONST_3_ADDR); //for (k = 0; k <= 3; k++ )
    VoC_jump(Gsm_Coder_Loop_End);
Gsm_Coder_Loop_start2:
    VoC_sw16_d(REG7, GLOBAL_subfrm_ADDR);
    VoC_mult16_rd(REG6,REG7,CONST_40_ADDR);         //{
    VoC_lw16_sd(REG0,7,DEFAULT);            // so[]
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);
    VoC_sw16_sd(REG7,9,DEFAULT);            // loop_k
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
//  Input:  REG0(incr=1) ->d
//stack16  ra loop_k source so xMc dp dpp &Nc[k] &bc[k] &xmaxc[k] &Mc[k]
    VoC_jal(CII_Gsm_Long_Term_Predictor);       //  Gsm_Long_Term_Predictor( S,so+k*40,dp,e + 5,dpp, Nc++, bc++);
    VoC_jal(CII_Gsm_RPE_Encoding);          //  Gsm_RPE_Encoding( S,e + 5,xmaxc++, Mc++, xMc );
    VoC_lw16_sd(REG4,6,DEFAULT);            //  xMc += 13;
    VoC_lw16i_short(REG5,13,IN_PARALLEL);
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_lw16_sd(REG1,4,IN_PARALLEL);        // dpp
    VoC_lw16i_set_inc(REG2,STATIC_ENC_E_ADDR+5,1);
    VoC_sw16_sd(REG4,6,DEFAULT);            // xMc
    VoC_lw16i_short(REG5,40,IN_PARALLEL);
    VoC_lw16_sd(REG0,5,DEFAULT);            // dp
    VoC_add16_rr(REG4,REG1,REG5,IN_PARALLEL);
    VoC_add16_rr(REG5,REG0,REG5,DEFAULT);

    VoC_loop_i_short(40,DEFAULT);           //    for (i = 0; i <= 39; i++)
    VoC_add16inc_pp(REG6,REG2,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_add16inc_pp(REG6,REG2,REG1,DEFAULT);//      dp[ i ] = (word) (GSM_ADD( e[5 + i], dpp[i] ));
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_lw16_sd(REG0,0,DEFAULT);            // &Mc[k]
    VoC_lw16_sd(REG1,1,DEFAULT);                    // &xmaxc[k]
    VoC_lw16_sd(REG2,2,DEFAULT);                    // &bc[k]
    VoC_lw16_sd(REG3,3,DEFAULT);                    // &Nc[k]
    VoC_add32_rd(REG01,REG01,CONST_0x00010001_ADDR);
    VoC_add32_rd(REG23,REG23,CONST_0x00010001_ADDR);
    VoC_sw16_sd(REG5,5,DEFAULT);            //  dp+= 40;
    VoC_sw16_sd(REG4,4,DEFAULT);            //  dpp += 40;

    VoC_sw16_sd(REG0,0,DEFAULT);            // &Mc[k]
    VoC_sw16_sd(REG1,1,DEFAULT);                    // &xmaxc[k]
    VoC_sw16_sd(REG2,2,DEFAULT);                    // &bc[k]
    VoC_sw16_sd(REG3,3,DEFAULT);                    // &Nc[k]

    VoC_lw16_sd(REG7,9,DEFAULT);
    VoC_jump(Gsm_Coder_Loop_start);         //}
Gsm_Coder_Loop_End:


/////addd for dtx
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_and16_rd(REG3, GLOBAL_txdtx_ctrl_ADDR);     //  if ((txdtx_ctrl & TX_SP_FLAG) == 0)//SID frame
    VoC_bnez16_r(coder_label_100, REG3)
    VoC_lw16i_short(REG6,8,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0, GLOBAL_xmax_SID_tx_ADDR, 1);
    VoC_loop_i_short(16,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG4, REG0,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_bnltz16_r(DTX_MODE_label_LBC,REG4);
    VoC_lw16i_short(REG5,-1,DEFAULT);
DTX_MODE_label_LBC:
    VoC_add32_rr(REG67, REG67, REG45,DEFAULT);
    VoC_endloop0
    VoC_shr32_ri(REG67,4,DEFAULT);  //   L_Temp = L_Temp >> 4;      xmax_aver = (word)(L_Temp);
    //REG6  :   xmax_aver

    /*  Qantizing and coding of xmax_aver to get xmaxc.
     */
    VoC_movreg16(REG7, REG6,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);

    VoC_jal(CII_APCM_quantization_sub);

    VoC_lw16i_set_inc(REG0, GLOBAL_ENC_XMAXC_ADDR, 1);

    //         for(i=0;i<4;i++)xmaxc[i]  = temp;
    VoC_loop_i_short(4,DEFAULT)
    VoC_startloop0
    VoC_sw16inc_p(REG7, REG0,DEFAULT);
    VoC_endloop0
coder_label_100:

/////addd for dtx end

    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_bne16_rd(coder_label_200, REG3, GLOBAL_DTX_ADDR)

    VoC_jal(CII_periodicity_update_fr);
    VoC_jal(CII_tone_detection_fr);

coder_label_200:

//stack16  ra loop_k source so xMc dp dpp &Nc[k] &bc[k] &xmaxc[k] &Mc[k]
    VoC_lw16i_set_inc(REG0,STATIC_ENC_GSM_STATE_ADDR+160,2);
    VoC_lw16i_set_inc(REG1,STATIC_ENC_GSM_STATE_ADDR,2);
    VoC_loop_i_short(10,DEFAULT);
    VoC_lw16i_short(REG6,30,IN_PARALLEL);       //120/4
    VoC_startloop0
    VoC_pop16(REG7,DEFAULT);
    VoC_endloop0

    VoC_jal(CII_copy);  //memcpy( (char *)S->dp0, (char *)(S->dp0 + 160), 120 * sizeof(*S->dp0) );

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}



// input : reg0->s   reg1->so
void CII_Gsm_Preprocess (void)
{



    VoC_lw16i_set_inc(REG2,LOCAL_SOF_Y_ADDR,1);
    VoC_lw32_d(RL6,STATIC_ENC_GSM_STATE_L_Z2_ADDR);
    VoC_lw16_d(REG6,STATIC_ENC_GSM_STATE_Z1_ADDR);
    VoC_lw16_d(REG7,STATIC_ENC_GSM_STATE_MP_ADDR);
    VoC_lw32_d(ACC1,CONST_0x00008000_ADDR);

    VoC_lw16i(RL7_HI,32735);
    VoC_lw16i(RL7_LO,-28180);
    VoC_push16(REG2,DEFAULT);

    VoC_loop_i(1,160);              //while (k--) {
    VoC_lw16inc_p(REG5,REG0,DEFAULT);   //SO = ( (*s)>> 3 ) << 2;
    VoC_movreg32(REG23,RL6,IN_PARALLEL);
    VoC_shr16_ri(REG5,3,DEFAULT);       //s++;
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_shr16_ri(REG5,-2,DEFAULT);
    VoC_shr32_ri(RL6,15,IN_PARALLEL);   //msp = (word) (( (L_z2)>> 15 ));
    //assert (SO >= -0x4000);
    //assert (SO <=  0x3FFC);
    VoC_sub16_rr(REG4,REG5,REG6,DEFAULT);       //s1 = SO - z1;
    VoC_movreg16(REG6,REG5,IN_PARALLEL);        //z1 = SO;
    //assert(s1 != MIN_WORD);
    VoC_bnltz16_r(Gsm_Preprocess_L1,REG4);
    VoC_lw16i_short(ACC0_HI,-1,DEFAULT);
Gsm_Preprocess_L1:                      //L_s2 = s1;
    VoC_movreg16(ACC0_LO,REG4,DEFAULT);
    VoC_movreg16(REG4,RL6_LO,IN_PARALLEL);
    VoC_and32_rd(REG23,CONST_0x00007FFF_ADDR);  //lsp = (word) (L_z2-((longword)msp<<15));
    VoC_movreg16(REG3,RL7_HI,DEFAULT);              //rl7_hi=32735
    VoC_multf32_rr(RL6,REG2,REG3,DEFAULT);      //L_s2  += GSM_MULT_R( lsp, 32735 );
    VoC_shr32_ri(ACC0,-15,DEFAULT);     //L_s2 <<= 15;
    VoC_add32_rr(RL6,RL6,ACC1,DEFAULT);
    VoC_shr32_ri(RL6,16,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_multf32_rr(RL6,REG4,REG3,IN_PARALLEL);      //L_temp = (longword)msp * 32735;
    VoC_movreg16(REG2,RL7_LO,DEFAULT);      // RL7_LO=-28180
    VoC_shr32_ri(RL6,1,DEFAULT);
    VoC_add32_rr(RL6,RL6,ACC0,DEFAULT);     //L_z2   = GSM_L_ADD( L_temp, L_s2 );
    VoC_multf32_rr(REG45,REG2,REG7,IN_PARALLEL);    //msp   = (word) (GSM_MULT_R( mp, -28180 ));
    VoC_add32_rd(REG23,RL6,CONST_0x00004000L_ADDR); //L_temp = GSM_L_ADD( L_z2, 16384 );
    VoC_add32_rr(REG45,REG45,ACC1,DEFAULT);
    VoC_shr32_ri(REG23,15,IN_PARALLEL);     //mp      = (word) (( L_temp>>15 ));
    VoC_add16_rr(REG5,REG5,REG2,DEFAULT);       //*so++ = (word) (GSM_ADD( mp, msp ));
    VoC_lw16_sd(REG4,0,IN_PARALLEL);
    VoC_movreg16(REG7,REG2,DEFAULT);
    VoC_add16_rd(REG3,REG4,CONST_1_ADDR);
    VoC_sw16_p(REG2,REG4,DEFAULT);
    VoC_sw16inc_p(REG5,REG1,IN_PARALLEL);
    VoC_sw16_sd(REG3,0,DEFAULT);
    VoC_endloop1        //}

    VoC_sw16_d(REG6,STATIC_ENC_GSM_STATE_Z1_ADDR);          //S->z1 = z1;S->mp  = mp;
    VoC_sw32_d(RL6,STATIC_ENC_GSM_STATE_L_Z2_ADDR);         //S->L_z2 = L_z2;
    VoC_movreg16(ACC0_LO,REG7,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,DEFAULT);
    VoC_bnltz16_r(Gsm_Preprocess_L2,REG7);
    VoC_lw16i_short(ACC0_HI,-1,DEFAULT);
Gsm_Preprocess_L2:
    VoC_pop16(REG2,DEFAULT);
    VoC_sw32_d(ACC0,STATIC_ENC_GSM_STATE_MP_ADDR);

    VoC_return;

}



/************************************************
 input: REG4,REG5,REG6,REG7
 input: reg0->&LAR[]
*************************************************/
void CII_step_t(void)
{
    VoC_multf16_rp(REG3,REG4,REG0,DEFAULT); //temp = (word) GSM_MULT( x,   *LAR );
    VoC_add16_rr(REG1,REG2,REG5,DEFAULT);   //temp = (word) GSM_ADD(  temp,   y );
    VoC_add16_rr(REG3,REG3,REG1,DEFAULT);   //temp = (word) GSM_ADD(  temp, 256 );
    VoC_shr16_ri(REG3,9,DEFAULT);       //temp = temp>>9;

    VoC_sub16_rr(REG1,REG6,REG7,DEFAULT);   //VoC_sub16_rr(REG1,REG3,REG6,DEFAULT);
    VoC_bgt16_rr(step_t_L2,REG3,REG6);  //*LAR  =  temp>z ? z - v : (temp<v ? 0 : temp - v);

    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_bgt16_rr(step_t_L2,REG7,REG3);
    VoC_sub16_rr(REG1,REG3,REG7,DEFAULT);
step_t_L2:
    VoC_NOP();
    VoC_sw16inc_p(REG1,REG0,DEFAULT);//LAR++;
    VoC_return;
}



// stack16 ra source s0 LARc
// stack32 0x00008000
void CII_Gsm_LPC_Analysis (void)
{

    // begin of Autocorrelation
    VoC_push16(RA,DEFAULT);

    VoC_lw16_sd(REG0,2,DEFAULT);            // reg0->so[]
    VoC_lw16i_short(REG3,4,IN_PARALLEL);

    VoC_lw32z(REG67,DEFAULT);           //smax = 0;
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    VoC_sw16_d(REG7,STATIC_VAD_SCALVAD_ADDR);

    VoC_loop_i(0,160);              //for (k = 0; k <= 159; k++) {
    VoC_bnltz16_r(Autocor_L1,REG5);
    VoC_sub16_rr(REG5,REG6,REG5,DEFAULT);
Autocor_L1:
    VoC_bngt16_rr(Autocor_L2,REG5,REG7);
    VoC_movreg16(REG7,REG5,DEFAULT);    //      if (temp > smax) smax = temp;
Autocor_L2:
    VoC_lw16inc_p(REG5,REG0,DEFAULT);   //      temp = GSM_ABS( s[k] );
    VoC_endloop0                    //}

    VoC_bez16_r(Autocor_L3,REG7);           //if (smax == 0) scalauto = 0;
    //else {   assert(smax > 0)
    VoC_jal(CII_gsm_norm);              //  input reg67,output reg6
    VoC_sub16_rr(REG6,REG3,REG6,DEFAULT);       //      scalauto = 4 - gsm_norm( (longword)smax << 16 );
    //}
Autocor_L3:// reg6->scalauto
    VoC_bngtz16_r(Autocor_L4,REG6);         //if (scalauto > 0) {
    VoC_lw16i(REG7,16384);              //      switch (scalauto) {
    VoC_be16_rd(Autocor_L5,REG6,CONST_1_ADDR);  //       case 1: for (k = 0; k <= 159; k++)  s[k] = GSM_MULT_R( s[k], 16384   ); break;
    VoC_shr16_ri(REG7,1,DEFAULT);
    VoC_be16_rd(Autocor_L5,REG6,CONST_2_ADDR);  //       case 2: for (k = 0; k <= 159; k++)  s[k] = GSM_MULT_R( s[k], 16384 >> (1) ); break;
    VoC_shr16_ri(REG7,1,DEFAULT);
    VoC_be16_rd(Autocor_L5,REG6,CONST_3_ADDR);  //       case 3: for (k = 0; k <= 159; k++)  s[k] = GSM_MULT_R( s[k], 16384 >> (2) ); break;
    VoC_shr16_ri(REG7,1,DEFAULT);           //       case 4: for (k = 0; k <= 159; k++)  s[k] = GSM_MULT_R( s[k], 16384 >> (3) ); break;
Autocor_L5:
    VoC_lw32_sd(ACC1,0,DEFAULT);            //acc1=0x00008000
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_lw16_sd(REG1,2,DEFAULT);            // reg1->&s[]
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_movreg16(REG0,REG1,DEFAULT);

    VoC_multf32inc_rp(RL6,REG7,REG1,DEFAULT);
    VoC_multf32inc_rp(RL7,REG7,REG1,IN_PARALLEL);

    VoC_loop_i(0,80);
    VoC_add32_rr(RL6,RL6,ACC1,DEFAULT); // acc1=0x00008000
    VoC_add32_rr(RL7,RL7,ACC1,IN_PARALLEL);
    VoC_movreg16(RL7_LO,RL6_HI,DEFAULT);
    VoC_multf32inc_rp(RL6,REG7,REG1,DEFAULT);
    VoC_multf32inc_rp(RL7,REG7,REG1,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(RL7,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

Autocor_L4:     //}}
    VoC_lw16_sd(REG0,2,DEFAULT);            // reg0->&s[]
    VoC_lw16i_short(INC0,1,IN_PARALLEL);   //word  * sp = s;
    VoC_lw16i_set_inc(REG2,LOCAL_ENC_L_ACF_ADDR,2); //word  sl = *sp;
    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);
    VoC_movreg16(RL7_HI,REG2,DEFAULT);
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);
    VoC_multf32_pp(ACC0,REG1,REG1,DEFAULT); //L_ACF[0] += (longword)((longword) sl * sp[ 0 ]);
    VoC_inc_p(REG0,IN_PARALLEL);

    VoC_loop_i_short(9,DEFAULT);    //for (k = 9; k--; L_ACF[k] = 0) ;
    VoC_movreg16(REG4,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_sw32inc_p(RL6,REG2,DEFAULT);
    VoC_endloop0

    VoC_mac32_pp(REG4,REG4,DEFAULT);    //L_ACF[0] += (longword)((longword) sl * sp[ 0 ]);
    VoC_movreg16(REG2,RL7_HI,IN_PARALLEL);
    VoC_multf32_pp(ACC1,REG0,REG1,DEFAULT); //sl = *++sp;
    VoC_movreg16(RL7_LO,RL7_HI,IN_PARALLEL);

    VoC_lw16i_short(REG5,3,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);


    VoC_loop_i(1,6);
    VoC_movreg32(REG23,RL7,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);                //sl = *++sp;
    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_lw16_p(REG4,REG0,IN_PARALLEL);

    VoC_loop_r_short(REG5,DEFAULT);             //for(j=0;j<4;j++)
    VoC_lw32inc_p(ACC0,REG3,IN_PARALLEL);
    VoC_startloop0                      //{
    VoC_mac32inc_rp(REG4,REG1,DEFAULT); //  L_ACF[j] += (longword)((longword) sl * sp[ -j ]);
    VoC_NOP();
    VoC_lw32inc_p(ACC0,REG3,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_add16_rd(REG5,REG5,CONST_1_ADDR);                   //}
    VoC_endloop1

    VoC_movreg32(REG23,RL7,DEFAULT);                //sl = *++sp;
    VoC_inc_p(REG0,IN_PARALLEL);

    VoC_loop_i(1,152);                      //for (i = 8; i <= 159; i++){
    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_lw16_p(REG4,REG0,IN_PARALLEL);
    VoC_loop_i_short(9,DEFAULT);                //for(j=0;j<4;j++)
    VoC_lw32inc_p(ACC0,REG3,IN_PARALLEL);
    VoC_startloop0                      //{
    VoC_mac32inc_rp(REG4,REG1,DEFAULT); //  L_ACF[j] += (longword)((longword) sl * sp[ -j ]);
    VoC_NOP();
    VoC_lw32inc_p(ACC0,REG3,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0                        //}
    VoC_movreg32(REG23,RL7,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);                //sl = *++sp;
    VoC_endloop1                            // }

    VoC_lw16_sd(REG0,2,DEFAULT);        // reg0->&s[]
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    //for (k = 9; k--; L_ACF[k] <<= 1) ;


    VoC_movreg16(REG3,REG0,DEFAULT);
    VoC_movreg16(REG2,REG0,IN_PARALLEL);
    VoC_bngtz16_r(Autocor_L6,REG6); //if (scalauto > 0) {
    //      assert(scalauto <= 4);
    VoC_sw16_d(REG6,STATIC_VAD_SCALVAD_ADDR);
    VoC_sub16_dr(REG6,CONST_0_ADDR,REG6);
    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_loop_i(0,80);   //      for (k = 160; k--; *s++ <<= scalauto) ;
    VoC_shr16_rr(REG4,REG6,DEFAULT);
    VoC_shr16_rr(REG5,REG6,IN_PARALLEL);
    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(REG45,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0    //}
Autocor_L6:



//add for vad & dtx

    VoC_lw16i_short(REG6,1,DEFAULT);         //     VAD_flag = 1;
    VoC_lw16i_short(REG7,3,DEFAULT);      //    txdtx_ctrl = TX_VAD_FLAG | TX_SP_FLAG;
    VoC_bne16_rd(NO_DTX_MODE_label_100, REG6, GLOBAL_DTX_ADDR)
    VoC_jal(CII_vad_computation_fr);
    VoC_lw16_d(REG6, GLOBAL_OUTPUT_Vad_ADDR);
    //input  REG6 ,  VAD_flag
    VoC_jal(CII_tx_dtx_fr);
NO_DTX_MODE_label_100:
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_sw16_d(REG6,GLOBAL_OUTPUT_Vad_ADDR);
    VoC_sw16_d(REG7,GLOBAL_txdtx_ctrl_ADDR);

//add for vad & dtx end

    // begin of Reflection_coefficients
    VoC_lw16i_set_inc(REG2,LOCAL_ENC_L_ACF_ADDR,2);
    VoC_bnez16_d(Ref_coeff_L1,LOCAL_ENC_L_ACF_ADDR);        //if (L_ACF[0] == 0)
    VoC_lw16_sd(REG2,1,DEFAULT);        //{ reg2-> &LARc[]
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_loop_i_short(4,DEFAULT);        //      for (i = 8; i--; *r++ = 0) ;
    VoC_startloop0
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_endloop0
    VoC_jump(Ref_coeff_LEnd);       //      return;
    //}
Ref_coeff_L1:
    //VoC_movreg16(REG2,RL7_HI,DEFAULT);        //assert( L_ACF[0] != 0 );
    VoC_lw32_d(REG67,LOCAL_ENC_L_ACF_ADDR);     //temp = gsm_norm( L_ACF[0] );
    VoC_jal(CII_gsm_norm);              //  input reg67,output reg6
    //assert(temp >= 0 && temp < 32);

    VoC_lw16i_set_inc(REG0,LOCAL_ENC_P_ADDR,1);
    VoC_lw16i_set_inc(REG1,LOCAL_ENC_ACF_ADDR,1);
    VoC_sub16_dr(REG6,CONST_16_ADDR,REG6);
    VoC_add32_rd(RL6,REG01,CONST_0x00010001_ADDR);

    VoC_loop_i_short(9,DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_shr32_rr(REG45,REG6,DEFAULT);
    VoC_lw16i_set_inc(REG3,LOCAL_ENC_K_ADDR+1,1);
    VoC_sw16inc_p(REG4,REG0,DEFAULT);   //for (i = 0; i <= 8; i++) ACF[i] = ((word) ((( L_ACF[i] << temp)>> 16 )));
    VoC_sw16inc_p(REG4,REG1,DEFAULT);   //for (i = 0; i <= 8; i++) P[ i ] = ACF[ i ];
    VoC_endloop0

    VoC_movreg16(REG1,RL6_HI,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_movreg16(RL6_HI,REG3,DEFAULT);
    VoC_lw16_sd(REG2,1,IN_PARALLEL);    //reg2-> &LARc[]

    VoC_loop_i_short(7,DEFAULT);    //for (i = 1; i <= 7; i++) K[ i ] = ACF[ i ];
    VoC_lw16inc_p(REG4,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_lw32_sd(RL7,0,IN_PARALLEL);         //RL7=0x00008000
    VoC_loop_i(1,8);                //for (n = 1; n <= 8; n++, r++)
    VoC_lw32_d(REG67,LOCAL_ENC_P_ADDR); //{   temp = P[1];
    VoC_movreg16(REG4,REG7,DEFAULT);
    VoC_movreg16(REG3,REG6,IN_PARALLEL);
    VoC_bnltz16_r(Ref_coeff_L2,REG7);   //    temp = GSM_ABS(temp);
    VoC_sub16_dr(REG7,CONST_0_ADDR,REG7);
Ref_coeff_L2:
    VoC_bngt16_rr(Ref_coeff_L3,REG7,REG6);  //    if (P[0] < temp)
    VoC_sub16_dr(REG6,CONST_9_ADDR,REG5);   //    {
    //  VoC_lw16_sd(REG0,1,DEFAULT);        //  reg0-> &LARc[]
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_loop_r_short(REG6,DEFAULT);     //      for (i = n; i <= 8; i++) *r++ = 0;
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_endloop0
    VoC_jump(Ref_coeff_LEnd);       //          return;  }
Ref_coeff_L3:





    /*********************/
// input in REG0, REG1
// output in  REG2
// used register RL6, RL7
// REG0/REG1
    /*********************/
//void CII_DIV_S(void)

    VoC_movreg16(REG0,REG7,DEFAULT);
    VoC_movreg16(REG1,REG6,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);

    VoC_push32(RL6,DEFAULT);

    VoC_jal(CII_DIV_S);                          //    *r = gsm_div( temp, P[0] );
    VoC_movreg16(REG0,REG2,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);
    VoC_pop16(REG2,DEFAULT);
    //    assert(*r >= 0);
    VoC_bngtz16_r(Ref_coeff_L4,REG4);   //    if (P[1] > 0) *r = -*r;
    VoC_sub16_dr(REG0,CONST_0_ADDR,REG0);
Ref_coeff_L4:
    //    assert (*r != MIN_WORD);
    VoC_multf32_rr(REG67,REG4,REG0,DEFAULT);    //    temp = GSM_MULT_R( P[1], *r );
    VoC_lw32_sd(RL7,0,DEFAULT);         //RL7=0x00008000
    VoC_sw16inc_p(REG0,REG2,DEFAULT);
    VoC_add32_rr(REG67,REG67,RL7,IN_PARALLEL);
    VoC_be16_rd(Ref_coeff_LEnd,REG5,CONST_8_ADDR);  //    if (n == 8) return;
    VoC_add16_rr(REG7,REG3,REG7,DEFAULT);       //    P[0] = (word) GSM_ADD( P[0], temp );
    VoC_movreg16(REG4,REG0,IN_PARALLEL);        // *r->reg0

    VoC_movreg32(REG01,RL6,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);
    VoC_sub16_dr(REG6,CONST_8_ADDR,REG5);
    VoC_sw16_d(REG7,LOCAL_ENC_P_ADDR);
    VoC_add16_rr(REG5,REG5,REG3,DEFAULT);       // reg5=n++;

    VoC_loop_r_short(REG6,DEFAULT);             //    for (m = 1; m <= 8 - n; m++)
    VoC_add16_rr(REG3,REG0,REG3,IN_PARALLEL);       // P[ m+1 ]
    VoC_startloop0                      //    {
    VoC_multf32_rp(REG67,REG4,REG1,DEFAULT);    //      temp     = GSM_MULT_R( K[m],*r );
    VoC_NOP();
    VoC_add32_rr(REG67,REG67,RL7,DEFAULT);
    VoC_add16_rp(REG7,REG7,REG3,DEFAULT);       //      P[m]     = (word) GSM_ADD(P[ m+1 ], temp );
    VoC_multf32inc_rp(ACC0,REG4,REG3,DEFAULT);  //      temp     = GSM_MULT_R( P[ m+1 ],*r );
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
//          VoC_add32_rr(REG67,ACC0,RL7,IN_PARALLEL);
    VoC_add32_rr(REG67,ACC0,RL7,DEFAULT);
    VoC_add16_rp(REG7,REG7,REG1,DEFAULT);   //      K[m]     = (word) GSM_ADD(K[m], temp );
    VoC_NOP();
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0    //    }
    VoC_NOP();
    VoC_endloop1        //}
Ref_coeff_LEnd:

    // begin of Transformation_to_Log_Area_Ratios

    VoC_lw16_sd(REG0,1,DEFAULT);        // reg0-> &LARc

    VoC_loop_i_short(8,DEFAULT);            // for (i = 1; i <= 8; i++, r++)
    VoC_startloop0                  //{
    VoC_lw16_p(REG6,REG0,DEFAULT);      //  temp = *r;
    VoC_movreg16(REG5,REG6,DEFAULT);
    VoC_bnltz16_r(Trans_to_Log_L1,REG6);    //  temp = GSM_ABS(temp);
    VoC_sub16_dr(REG6,CONST_0_ADDR,REG6);
Trans_to_Log_L1:                    //  assert(temp >= 0);


    VoC_movreg16(REG7,REG6,DEFAULT);

    VoC_shr16_ri(REG6,1,DEFAULT);      //  {temp >>= 1;}
    VoC_lw16i(REG3,0x5666);
    VoC_bngt16_rr(Trans_to_Log_L4,REG7,REG3);
    //   VoC_bngt16_rd(Trans_to_Log_L4,REG7,CONST_0x5666_ADDR);    //  if (temp < 22118)

    VoC_sub16_rd(REG6,REG7,CONST_0x2b33_ADDR);    //  {   assert( temp >= 11059 );
    VoC_lw16i(REG3,0x799a);
    VoC_bngt16_rr(Trans_to_Log_L4,REG7,REG3);
    //   VoC_bngt16_rd(Trans_to_Log_L4,REG7,CONST_0x799a_ADDR); //  else if (temp < 31130)


    VoC_lw16i(REG6,0x6600);
    //  VoC_sub16_rd(REG6,REG7,CONST_0x6600_ADDR);  //      temp -= 26112;
    VoC_sub16_rr(REG6,REG7,REG6,DEFAULT);
    VoC_shr16_ri(REG6,-2,DEFAULT);      //      temp <<= 2;

Trans_to_Log_L4:            //  }


    VoC_bgtz16_r(Trans_to_Log_L5,REG5); //  *r = *r < 0 ? -temp : temp;
    VoC_sub16_dr(REG6,CONST_0_ADDR,REG6);   //  assert( *r != MIN_WORD );
    VoC_NOP();
Trans_to_Log_L5:

    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    VoC_endloop0        //}


    VoC_lw16i_short(REG3,3,DEFAULT);
    /*
           txdtx_ctrl  :   10    TX_SID_UPDATE;
                             5     TX_HANGOVER_ACTIVE  | TX_SP_FLAG;
                            25    TX_USE_OLD_SID|TX_HANGOVER_ACTIVE   | TX_SP_FLAG
                             3    TX_SP_FLAG | TX_VAD_FLAG;
      */
    VoC_be16_rd(DTX_MODE_label_200, REG3, GLOBAL_txdtx_ctrl_ADDR)
    VoC_lw16i_set_inc(REG0, GLOBAL_LAR_SID_tx_ADDR+30,-2);
    VoC_lw16i_set_inc(REG1, GLOBAL_LAR_SID_tx_ADDR+22,-2);

    VoC_lw16i_set_inc(REG2, GLOBAL_ENC_LARC_ADDR,2);

    VoC_loop_i_short(12,DEFAULT);
    VoC_lw32inc_p(ACC0, REG1,IN_PARALLEL);
    VoC_startloop0
    exit_on_warnings=OFF;
    VoC_lw32inc_p(ACC0, REG1,DEFAULT);
    VoC_sw32inc_p(ACC0, REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_lw16i_set_inc(REG1, GLOBAL_LAR_SID_tx_ADDR,2);
    VoC_loop_i_short(4,DEFAULT);
    VoC_lw32inc_p(ACC0, REG2,IN_PARALLEL);
    VoC_startloop0
    exit_on_warnings=OFF;
    VoC_lw32inc_p(ACC0, REG2,DEFAULT);
    VoC_sw32inc_p(ACC0, REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

DTX_MODE_label_200:

    VoC_lw16i_short(REG3,0x10,DEFAULT);

    VoC_bne16_rd(DTX_MODE_label_300, REG3, GLOBAL_txdtx_ctrl_ADDR)

    VoC_lw16i_set_inc(REG2, GLOBAL_ENC_LARC_ADDR,1);
    VoC_lw16i_set_inc(REG0, GLOBAL_LAR_SID_tx_ADDR,1);

    VoC_lw16i_short(INC1,8,DEFAULT);

    VoC_lw16i_short(RL6_HI,0,DEFAULT);
    VoC_lw16i_short(RL6_LO,2,IN_PARALLEL);

    VoC_loop_i(1,8)
    VoC_movreg16(REG1, REG0,DEFAULT);
    VoC_movreg32(ACC1,RL6,IN_PARALLEL);

    VoC_loop_i_short(4,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG6, REG1,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_bnltz16_r(DTX_MODE_label_LAB,REG6);
    VoC_lw16i_short(REG7,-1,DEFAULT);
DTX_MODE_label_LAB:
    VoC_add32_rr(ACC1,ACC1,REG67,DEFAULT);
    VoC_endloop0

    VoC_shr32_ri(ACC1,2,DEFAULT);
    VoC_inc_p(REG0,DEFAULT);
    VoC_sw16inc_p(ACC1_LO, REG2,DEFAULT);
    VoC_endloop1
DTX_MODE_label_300:


    VoC_lw16i_short(INC0,1,DEFAULT);

    // begin of Quantization_and_coding

    VoC_lw16_sd(REG0,1,DEFAULT);            // REG0->&LARc
    VoC_lw16i_short(REG7,-32,IN_PARALLEL);
    VoC_lw16i(REG2,256);
    VoC_lw16i(REG4,20480);
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_short(REG6,31,IN_PARALLEL);

    VoC_jal(CII_step_t);        //STEP_t(  20480,  0,  31, -32 );
    VoC_jal(CII_step_t);        //STEP_t(  20480,  0,  31, -32 );

    VoC_lw16i(REG5,2048);
    VoC_lw16i_short(REG6,15,DEFAULT);
    VoC_lw16i_short(REG7,-16,IN_PARALLEL);
    VoC_jal(CII_step_t);        //STEP_t(  20480,  2048,  15, -16 );

    VoC_lw16i(REG5,-2560);
    VoC_jal(CII_step_t);        //STEP_t(  20480, -2560,  15, -16 );

    VoC_lw16i(REG4,13964);
    VoC_lw16i(REG5,94);
    VoC_lw16i_short(REG6,7,DEFAULT);
    VoC_lw16i_short(REG7,-8,IN_PARALLEL);
    VoC_jal(CII_step_t);        //STEP_t(  13964,  94,   7,  -8 );

    VoC_lw16i(REG4,15360);
    VoC_lw16i(REG5,-1792);
    VoC_jal(CII_step_t);        //STEP_t(  15360, -1792,  7,  -8 );

    VoC_lw16i(REG4,8534);
    VoC_lw16i(REG5,-341);
    VoC_lw16i_short(REG6,3,DEFAULT);
    VoC_lw16i_short(REG7,-4,IN_PARALLEL);
    VoC_jal(CII_step_t);        //STEP_t(  8534,  -341,   3,  -4 );

    VoC_lw16i(REG4,9036);
    VoC_lw16i(REG5,-1144);
    VoC_jal(CII_step_t);        //STEP_t(  9036, -1144,   3,  -4 );

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;

}



//input  REG6   VAD_flag
//return    REG6   VAD_flag,   REG7   txdtx_ctrl
void  CII_tx_dtx_fr (  void )

{

    /* N_elapsed (frames since last SID update) is incremented. If SID
       is updated N_elapsed is cleared later in this function */

    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_add16_rd(REG5, REG3, GLOBAL_txdtx_N_elapsed_ADDR);
    VoC_lw16i_short(REG7,3,DEFAULT);              // *txdtx_ctrl = TX_SP_FLAG | TX_VAD_FLAG;
    VoC_lw16i_short(REG4,4,IN_PARALLEL);             //       txdtx_hangover = DTX_HANGOVER;

    VoC_be16_rr(tx_dtx_end, REG6,REG3)
    VoC_lw16_d(REG4, GLOBAL_txdtx_hangover_ADDR);
    VoC_bnez16_r(tx_dtx_100, REG4)
    VoC_lw16i_short(REG5,0,DEFAULT);                            //      txdtx_N_elapsed = 0;
    VoC_lw16i_short(REG7,   0x0010,IN_PARALLEL);     //        *txdtx_ctrl = TX_SID_UPDATE;
    VoC_jump(tx_dtx_end);
tx_dtx_100:

    VoC_lw16i_short(REG7,5,DEFAULT);             //      *txdtx_ctrl = TX_HANGOVER_ACTIVE  | TX_SP_FLAG;
    VoC_sub16_rd(REG4, REG4,CONST_1_ADDR);
    VoC_add16_rr(REG2, REG5, REG4,DEFAULT);

    VoC_bnlt16_rd(tx_dtx_end, REG2 , CONST_27_ADDR  )
    VoC_lw16i(REG7,0x24);                //      *txdtx_ctrl = TX_USE_OLD_SID|TX_HANGOVER_ACTIVE   | TX_SP_FLAG ;     // Changed by Cui   2004.11.25
tx_dtx_end:
    VoC_sw32_d(REG45, GLOBAL_txdtx_hangover_ADDR);
    VoC_return;

}


/*******************************************************
  function: CII_Gsm_Long_Term_Predictor
  Input:  REG0(incr=1) ->d
//stack16  source so xMc dp dpp &Nc[k] &bc[k] &xmaxc[k] &Mc[k]
*******************************************************/
void CII_Gsm_Long_Term_Predictor (void)
{
    //assert( d  ); assert( dp ); assert( e  );
    //assert( dpp); assert( Nc ); assert( bc );
    VoC_push16(RA,DEFAULT);

    VoC_push16(REG0,DEFAULT);
    VoC_lw32z(REG67,IN_PARALLEL);               //dmax = 0;
//stack16  source so xMc dp dpp &Nc[k] &bc[k] &xmaxc[k] &Mc[k] ra d

// begin of Calculation_of_the_LTP_parameters( d, dp, bc, Nc );

    VoC_movreg16(RL7_HI,REG0,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_loop_i_short(40,DEFAULT);               //for (k = 0; k <= 39; k++)
    VoC_lw16inc_p(REG5,REG0,IN_PARALLEL);
    VoC_startloop0                      //{
    //  temp = d[k];
    VoC_bnltz16_r(Cal_of_the_LTP_p_L1,REG5);    //  temp = GSM_ABS( temp );
    VoC_sub16_rr(REG5,REG6,REG5,DEFAULT);
Cal_of_the_LTP_p_L1:
    VoC_bngt16_rr(Cal_of_the_LTP_p_L2,REG5,REG7);
    VoC_movreg16(REG7,REG5,DEFAULT);        //  if (temp > dmax) dmax = temp;
Cal_of_the_LTP_p_L2:
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    VoC_endloop0                        //}

    //temp = 0;
    VoC_bez16_r(Cal_of_the_LTP_p_L3,REG7);          //if (dmax == 0) scal = 0;
    //else
    //{     assert(dmax > 0);
    VoC_jal(CII_gsm_norm);                  //  temp = gsm_norm( ((longword) dmax) << 16 );
    //}
Cal_of_the_LTP_p_L3:
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_movreg16(REG2,RL7_HI,IN_PARALLEL);
    VoC_bgt16_rd(Cal_of_the_LTP_p_L4,REG6,CONST_6_ADDR);    //if (temp > 6) scal = 0;
    VoC_sub16_dr(REG5,CONST_6_ADDR,REG6);           //else scal = 6 - temp;
Cal_of_the_LTP_p_L4:
    //assert(scal >= 0);
    VoC_lw16i_set_inc(REG3,LOCAL_ENC_WT_ADDR,2);
    VoC_movreg16(RL7_LO,REG3,DEFAULT);
    VoC_lw16_sd(RL7_HI,7,IN_PARALLEL);      //RL7_HI->dp[]
    VoC_loop_i_short(20,DEFAULT);       //for (k = 0; k <= 39; k++) wt[k] = d[k]>>scal;
    VoC_lw32inc_p(REG67,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_shr16_rr(REG6,REG5,DEFAULT);
    VoC_shr16_rr(REG7,REG5,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_lw32inc_p(REG67,REG2,DEFAULT);
    VoC_sw32inc_p(REG67,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0


    VoC_movreg32(REG01,RL7,DEFAULT);
    VoC_lw16i_short(REG3,40,IN_PARALLEL);
    VoC_lw32z(RL6,DEFAULT);         //L_max = 0;
    VoC_lw16i_short(REG7,40,IN_PARALLEL);       //Nc= 40;


    VoC_sub16_rr(REG1,REG1,REG3,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_loop_i(1,81);                   //for (lambda = 40; lambda <= 120; lambda++)
    //{
    VoC_loop_i_short(40,DEFAULT);           //for(k=0;k<40;k++)
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0                  //{
    VoC_mac32inc_pp(REG0,REG1,DEFAULT);//   L_result+=wt[k] * (longword) dp[k - lambda];
    VoC_endloop0                    //}
    VoC_movreg32(REG01,RL7,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG6,IN_PARALLEL);
    VoC_bngt32_rr(Cal_of_the_LTP_p_L5,ACC0,RL6);    //  if (L_result > L_max)
    VoC_movreg16(REG7,REG3,DEFAULT);        //  {      Nc  = (word) lambda;
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);     //      L_max = L_result; }
Cal_of_the_LTP_p_L5:
    VoC_sub16_rr(REG1,REG1,REG3,DEFAULT);
    VoC_endloop1                        //}


//stack16  source so xMc dp dpp &Nc[k] &bc[k] &xmaxc[k] &Mc[k] ra d
    VoC_be16_rd(Cal_of_the_LTP_p_L5t,REG7,CONST_40_ADDR);
    VoC_sub16_rr(REG7,REG7,REG6,DEFAULT);
Cal_of_the_LTP_p_L5t:
    VoC_lw16_sd(REG0,5,DEFAULT);        // reg7->Nc_out
    VoC_movreg16(REG1,RL7_HI,DEFAULT);          // reg1->*dp
    VoC_sub16_dr(REG5,CONST_6_ADDR,REG5);
    VoC_sub16_rr(REG1,REG1,REG7,DEFAULT);   //dp[k - Nc]
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_sw16_p(REG7,REG0,DEFAULT);          //*Nc_out = Nc;   L_max <<= 1;
    //assert(scal <= 100 && scal >=  -100);
    VoC_shr32_rr(RL6,REG5,IN_PARALLEL);     //L_max = L_max >> (6 - scal);
    VoC_push16(REG1,DEFAULT);               // push16 dp[k - Nc]
    //assert( Nc <= 120 && Nc >= 40);


    //L_power = 0;
    VoC_loop_i_short(40,DEFAULT);           //for (k = 0; k <= 39; k++)
    VoC_lw16inc_p(REG6,REG1,IN_PARALLEL);
    VoC_startloop0                  //{
    VoC_shr16_ri(REG6,3,DEFAULT);       //  register longword L_temp;
    VoC_mac32_rr(REG6,REG6,DEFAULT);    //  L_temp   = dp[k - Nc]>>3 ;
    VoC_lw16inc_p(REG6,REG1,IN_PARALLEL);   //  L_power += L_temp * L_temp;
    VoC_endloop0                    //}
    //L_power <<= 1;


    VoC_lw16i_short(REG0,0,DEFAULT);        //{     *bc_out = 0;
    VoC_bngtz32_r(Cal_of_the_LTP_p_LEnd,RL6);       //if (L_max <= 0)   //  return;
    VoC_lw16i_short(REG0,3,DEFAULT);        //  *bc_out = 3;
    VoC_bngt32_rr(Cal_of_the_LTP_p_LEnd,ACC0,RL6);  //if (L_max >= L_power) {       //  return;


    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_jal(CII_gsm_norm);              //temp = gsm_norm( L_power );
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_sub16_rr(REG6,REG0,REG6,DEFAULT);       //R = (word) ((( L_max   << temp) >> 16 ));
    VoC_shr32_rr(ACC0,REG6,DEFAULT);
    VoC_shr32_rr(RL6,REG6,IN_PARALLEL);     //S = (word) ((( L_power << temp) >> 16 ));
    VoC_movreg16(REG4,RL6_HI,DEFAULT);
    VoC_movreg16(REG5,ACC0_HI,IN_PARALLEL);
    VoC_lw16i_set_inc(REG2,STATIC_CONST_GSM_DLB_ADDR,1);

    VoC_loop_i_short(3,DEFAULT);            //for (bc = 0; bc <= 2; bc++)
    VoC_startloop0;
    VoC_multf32inc_rp(REG67,REG5,REG2,DEFAULT);
    VoC_NOP();
    VoC_bngt16_rr(Cal_of_the_LTP_p_LEnd,REG4,REG7); //if (R <= gsm_mult(S, gsm_DLB[bc])) break;
    VoC_inc_p(REG0,DEFAULT);
    VoC_endloop0;
    //*bc_out = bc;
Cal_of_the_LTP_p_LEnd:
    VoC_lw16_sd(REG5,5,DEFAULT);        // bc[]

    VoC_pop16(REG2,DEFAULT);        //dp[k-Nc]
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_pop16(REG1,DEFAULT);        // d
    VoC_sw16_p(REG0,REG5,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
//stack16  source so xMc dp dpp &Nc[k] &bc[k] &xmaxc[k] &Mc[k] ra d

// begin of Long_term_analysis_filtering( *bc, *Nc, dp, d, dpp, e );


    //switch (bc)
    VoC_lw16i(REG4,3277);   //case 0: BP= 3277  ; break;
    VoC_bez16_r(Long_term_analysis_filt_L4, REG0)
    VoC_lw16i(REG4,11469);                  //case 1: BP= 11469 ; break;
    VoC_be16_rd(Long_term_analysis_filt_L4, REG0, CONST_1_ADDR)
    VoC_lw16i(REG4,21299);                  //case 2: BP= 21299 ; break;
    VoC_be16_rd(Long_term_analysis_filt_L4, REG0, CONST_2_ADDR)
    VoC_lw16i(REG4,32767);                   //case 3: BP= 32767 ; break;

Long_term_analysis_filt_L4:                     //}
    VoC_lw16_sd(REG0,5,DEFAULT);        // dpp
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG3,STATIC_ENC_E_ADDR+5,1);
    VoC_loop_i_short(40,DEFAULT);               //for (k = 0; k <= 39; k++)
    VoC_pop16(RA,IN_PARALLEL);
    VoC_startloop0                      //{
    VoC_multf32inc_rp(REG67,REG4,REG2,DEFAULT); //  dpp[k]  = (word) (GSM_MULT_R( BP, dp[k - Nc]));
    VoC_NOP();
    VoC_add32_rr(REG67,REG67,ACC1,DEFAULT);
    VoC_sub16inc_pr(REG6,REG1,REG7,DEFAULT);    //  e[k]    = (word) (GSM_SUB( d[k], dpp[k]));
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    VoC_endloop0                        //}

    VoC_return;
}



/******************************************************

 input: 2*B->reg4
        MIC->reg5
        INVA->reg6

******************************************************/
void CII_Decoding_of_the_coded_Log_Area_Ratios_sub(void)
{
    VoC_add16inc_rp(REG2,REG5,REG0,DEFAULT);        //temp1    = (word) (GSM_ADD( (long) *LARc++, (long) MIC ) << 10);

    VoC_shr32_ri(REG23,-10,DEFAULT);
    VoC_sub16_rr(REG2,REG2,REG4,DEFAULT);           //temp1    = (word) GSM_SUB( (long) temp1, (long) B << 1 );
    VoC_multf32_rr(REG23,REG2,REG6,DEFAULT);        //temp1    = (word) GSM_MULT_R( (long) INVA, (long) temp1 );
    VoC_NOP();
    VoC_add32_rr(REG23,REG23,RL7,DEFAULT);      //rl7=ox000080000
    VoC_shr16_ri(REG3,-1,DEFAULT);
    VoC_lw16i_short(REG3,0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG3,REG1,DEFAULT);       //*LARpp++ = (word) GSM_ADD( (long) temp1, (long) temp1 );
    exit_on_warnings=ON;
    VoC_return;

}

// stack16 source s0 LARc RA LARpp_j LARpp_j_1
// LARc     reg0(incr=1)
// LARpp_j  reg1(incr=1)
void CII_Decoding_of_the_coded_Log_Area_Ratios (void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw32_sd(RL7,0,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i_short(REG5,-32,IN_PARALLEL);
    VoC_lw16i(REG6,13107);
    VoC_jal(CII_Decoding_of_the_coded_Log_Area_Ratios_sub); //STEP(    0,  -32,  13107 );
    VoC_jal(CII_Decoding_of_the_coded_Log_Area_Ratios_sub); //STEP(    0,  -32,  13107 );

    VoC_lw16i(REG4,(2048*2));
    VoC_lw16i_short(REG5,-16,DEFAULT);
    VoC_jal(CII_Decoding_of_the_coded_Log_Area_Ratios_sub); //STEP( 2048,  -16,  13107 );

    VoC_lw16i(REG4,((-2560)*2));
    VoC_jal(CII_Decoding_of_the_coded_Log_Area_Ratios_sub); //STEP(  -2560,  -16,  13107 );

    VoC_lw16i(REG4,(94*2));
    VoC_lw16i_short(REG5,-8,DEFAULT);
    VoC_lw16i(REG6,19223);
    VoC_jal(CII_Decoding_of_the_coded_Log_Area_Ratios_sub); //STEP(   94,   -8,  19223 );

    VoC_lw16i(REG4,((-1792)*2));
    VoC_lw16i(REG6,17476);
    VoC_jal(CII_Decoding_of_the_coded_Log_Area_Ratios_sub); //STEP(  -1792, -8,  17476 );

    VoC_lw16i(REG4,((-341)*2));
    VoC_lw16i_short(REG5,-4,DEFAULT);
    VoC_lw16i(REG6,31454);
    VoC_jal(CII_Decoding_of_the_coded_Log_Area_Ratios_sub); //STEP( -341,   -4,  31454 );

    VoC_lw16i(REG4,((-1144)*2));
    VoC_lw16i(REG6,29708);
    VoC_jal(CII_Decoding_of_the_coded_Log_Area_Ratios_sub); //STEP(  -1144, -4,  29708 );

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

// stack16 source s0 LARc RA LARpp_j LARpp_j_1
// input:  LARp->reg0(incr=1)
void CII_Coefficients_0_12 (void)
{
    VoC_lw16_sd(REG2,1,DEFAULT);

    VoC_lw16_sd(REG1,2,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
//VoC_push16(REG0,DEFAULT);
    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_loop_i_short(8,DEFAULT);            //for (i = 1; i <= 8; i++)
    VoC_lw16inc_p(REG6,REG2,IN_PARALLEL);
    VoC_startloop0                  //{

    VoC_lw16inc_p(REG5,REG1,DEFAULT);   //  temp = (word) GSM_ADD(( (*LARpp_j_1)>> 2 ),  ( (*LARpp_j++)>> 2 ));
    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    VoC_shr16_ri(REG5,2,DEFAULT);
    VoC_shr16_ri(REG6,2,IN_PARALLEL);
    VoC_shr16_ri(REG7,1,DEFAULT);
    VoC_add16_rr(REG5,REG6,REG5,IN_PARALLEL);
    VoC_add16_rr(REG7,REG7,REG5,DEFAULT);   //  *LARp++ = (word) GSM_ADD( temp,  ( (*LARpp_j_1++)>> 1));
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    VoC_endloop0                    //}
    VoC_return;
}

// stack16 source s0 LARc RA LARpp_j LARpp_j_1 LARp
void CII_Coefficients_13_26 (void)
{
    VoC_lw16_sd(REG2,1,DEFAULT);
    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_lw16_sd(REG1,2,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_lw16_sd(REG0,0,DEFAULT);

    VoC_loop_i_short(8,DEFAULT);    //for (i = 1; i <= 8; i++)
    VoC_lw16inc_p(REG7,REG2,IN_PARALLEL);
    VoC_startloop0          //{
    //  *LARp++ = (word) GSM_ADD( ( (*LARpp_j_1++)>> 1), ( (*LARpp_j++)>> 1 ));
    VoC_lw16inc_p(REG6,REG1,DEFAULT);
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_shr16_ri(REG7,1,IN_PARALLEL);
    VoC_add16_rr(REG7,REG7,REG6,DEFAULT);
    VoC_lw16inc_p(REG7,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0            //}
    VoC_return;
}

// stack16 source s0 LARc RA LARpp_j LARpp_j_1 LARp
void CII_Coefficients_27_39 (void)
{

    VoC_lw16_sd(REG1,1,DEFAULT);

    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_lw16_sd(REG2,2,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_loop_i_short(8,DEFAULT);        //for (i = 1; i <= 8; i++)
    VoC_lw16_sd(REG0,0,IN_PARALLEL);    //{
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG2,DEFAULT);// *LARp = (word) GSM_ADD( ( (*LARpp_j_1++)>> 2L ), ( (*LARpp_j)>> 2L ));
    VoC_shr16_ri(REG4,2,IN_PARALLEL);
    VoC_movreg16(REG6,REG5,DEFAULT);
    VoC_shr16_ri(REG5,2,IN_PARALLEL);
    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);
    VoC_shr16_ri(REG6,1,IN_PARALLEL);// *LARp++ = (word) GSM_ADD( *LARp, ( (*LARpp_j++)>> 1L ));
    VoC_add16_rr(REG6,REG6,REG5,DEFAULT);
    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    VoC_endloop0                //}
    VoC_return;
}

// stack16 source s0 LARc RA LARpp_j LARpp_j_1 LARp
void CII_Coefficients_40_159 (void)
{
    VoC_lw16_sd(REG0,2,DEFAULT);
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_loop_i_short(8,DEFAULT);        //for (i = 1; i <= 8; i++,)
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);// *LARp++ = *LARpp_j++;
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_return;
}

// stack16 source s0 LARc RA LARpp_j LARpp_j_1 LARp
void CII_LARp_to_rp (void)
{
    VoC_lw16_sd(REG0,0,DEFAULT);


    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_lw32z(REG67,IN_PARALLEL);

    VoC_loop_i_short(8,DEFAULT);            //for (i = 1; i <= 8; i++, LARp++)
    VoC_lw16inc_p(REG5,REG0,IN_PARALLEL);
    VoC_startloop0                  //{


    VoC_bnltz16_r(LARp_to_rp_L1,REG5);  //  if (*LARp < 0)
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_movreg16(REG4, REG5,IN_PARALLEL);

    VoC_sub16_rr(REG5,REG6,REG5,DEFAULT);   //  }
    VoC_bne16_rd(LARp_to_rp_L1,REG4,CONST_0x00008000_ADDR); //  {
    VoC_lw16i(REG5,0x7FFF);         //      temp = *LARp == MIN_WORD ? MAX_WORD : -(*LARp);

LARp_to_rp_L1:                      //  else

    VoC_movreg16(REG4, REG5,DEFAULT);
    VoC_shr16_ri(REG5,-1,DEFAULT);        //      *LARp =    ((temp < 11059) ? temp << 1
    VoC_blt16_rd(LARp_to_rp_L5, REG4, CONST_0x2b33_ADDR)     //   {   temp  = *LARp; }

    VoC_add16_rd(REG5,REG4,CONST_0x2b33_ADDR);
    VoC_lw16i(REG3,0x4e66);
    VoC_bgt16_rr(LARp_to_rp_L5,REG3, REG4)
//        VoC_blt16_rd(LARp_to_rp_L5, REG4, CONST_0x4e66_ADDR)         //      : ((temp < 20070) ? temp + 11059
    VoC_shr16_ri(REG4,2,DEFAULT);
//            VoC_add16_rd(REG5,REG4,CONST_0x6600_ADDR);    //      :  (word) GSM_ADD( temp >> 2, 26112L )));
    VoC_lw16i(REG3,0x6600);
    VoC_add16_rr(REG5,REG4,REG3,DEFAULT);
LARp_to_rp_L5:

    VoC_bez16_r(LARp_to_rp_L6,REG7);
    VoC_sub16_rr(REG5,REG6,REG5,DEFAULT);
LARp_to_rp_L6:
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop0                    //}

    VoC_return;

}


// stack16 source s0 LARc RA LARpp_j LARpp_j_1 LARp
// input k_n->reg7
//   *s->reg0
void CII_Short_term_analysis_filtering (void)
{

    VoC_lw16_sd(REG2,0,DEFAULT);
    VoC_lw16i_set_inc(REG3,STATIC_ENC_GSM_STATE_U_ADDR,1);

    VoC_movreg32(RL7,REG23,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_lw32_d(ACC1,CONST_0x00008000_ADDR);

    VoC_loop_r(1,REG7);             //for (; k_n--; s++)
    VoC_lw16_p(REG6,REG0,DEFAULT);      //{     di = sav = *s;
    VoC_movreg16(RL6_LO,REG0,IN_PARALLEL);

    VoC_loop_i_short(8,DEFAULT);                //  for (i = 0; i < 8; i++)
    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    VoC_startloop0                      //   {
    VoC_multf32_rp(REG01,REG7,REG2,DEFAULT);
    VoC_lw16_p(REG5,REG3,DEFAULT);          //      ui  = u[i];    rpi = rp[i];
    VoC_add32_rr(REG01,REG01,ACC1,DEFAULT);
    VoC_sw16inc_p(REG6,REG3,IN_PARALLEL);       //      u[i]= sav;
    VoC_add16_rr(REG6,REG5,REG1,DEFAULT);       //      sav = (word) GSM_ADD(ui,zzz);
    VoC_multf32inc_rp(REG01,REG5,REG2,IN_PARALLEL); //      zzz = (word) GSM_MULT_R(rpi, ui);
    VoC_NOP();
    VoC_add32_rr(REG01,REG01,ACC1,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG1,DEFAULT);       //      di  = (word) GSM_ADD(di,zzz);
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);
    VoC_endloop0                        //  }
    VoC_movreg32(REG23,RL7,DEFAULT);
    VoC_sw16inc_p(REG7,REG0,DEFAULT);           //  *s = di;
    VoC_endloop1                            //}

    VoC_return;
}



// stack16 ra source s0 LARc
// stack32 0x00008000
void CII_Gsm_Short_Term_Analysis_Filter (void)
{
    // * LARpp_j    = S->LARpp[ S->j      ];
    // * LARpp_j_1  = S->LARpp[ S->j ^= 1 ];
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG6,1,DEFAULT);

    VoC_lw16i_set_inc(REG1,STATIC_ENC_GSM_STATE_LARPP_ADDR,1);
    VoC_lw16_d(REG5,STATIC_ENC_GSM_STATE_J_ADDR);
    VoC_xor16_rr(REG6,REG5,DEFAULT);
    VoC_lw16i_short(REG7,8,IN_PARALLEL);
    VoC_mult16_rr(REG4,REG7,REG6,DEFAULT);
    VoC_mult16_rr(REG5,REG7,REG5,IN_PARALLEL);
    VoC_sw16_d(REG6,STATIC_ENC_GSM_STATE_J_ADDR);
    VoC_add16_rr(REG2,REG1,REG4,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG5,IN_PARALLEL);
    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_push16(REG1,DEFAULT);
//  VoC_sw16_d(REG6,STATIC_ENC_GSM_STATE_J_ADDR);
    VoC_push16(REG2,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
// stack16 source s0 LARc RA LARpp_j LARpp_j_1
// LARc     reg0(incr=1)
// LARpp_j  reg1(incr=1)

    VoC_jal(CII_Decoding_of_the_coded_Log_Area_Ratios);

    VoC_lw16i_set_inc(REG0,LOCAL_ENC_LARP_ADDR,1);
    VoC_NOP();
    VoC_push16(REG0,DEFAULT);
// stack16 source s0 LARc RA LARpp_j LARpp_j_1
// input LARp->reg0(incr=1)
    VoC_jal(CII_Coefficients_0_12);

// stack16 source s0 LARc RA LARpp_j LARpp_j_1 LARp
    VoC_jal(CII_LARp_to_rp);

    VoC_lw16_sd(REG0,5,DEFAULT);        //reg0->*s
    VoC_lw16i_short(REG7,13,IN_PARALLEL);
// stack16 source s0 LARc RA LARpp_j LARpp_j_1 LARp
// input k_n->reg7
//   *s->reg0
    VoC_jal(CII_Short_term_analysis_filtering);

    VoC_jal(CII_Coefficients_13_26);

    VoC_jal(CII_LARp_to_rp);

    VoC_lw16_sd(REG0,5,DEFAULT);        //reg0->*s
    VoC_lw16i_short(REG7,14,IN_PARALLEL);
    VoC_add16_rd(REG0,REG0,CONST_13_ADDR);
// stack16 source s0 LARc RA LARpp_j LARpp_j_1 LARp
// input k_n->reg7
//   *s->reg0
    VoC_jal(CII_Short_term_analysis_filtering);

    VoC_jal(CII_Coefficients_27_39);
    VoC_jal(CII_LARp_to_rp);
    VoC_lw16_sd(REG0,5,DEFAULT);        //reg0->*s
    VoC_lw16i_short(REG7,13,IN_PARALLEL);
    VoC_add16_rd(REG0,REG0,CONST_27_ADDR);
// stack16 source s0 LARc RA LARpp_j LARpp_j_1 LARp
// input k_n->reg7
//   *s->reg0
    VoC_jal(CII_Short_term_analysis_filtering); // Short_term_analysis_filtering( S, LARp, 13L, s + 27);

    VoC_jal(CII_Coefficients_40_159);
    VoC_jal(CII_LARp_to_rp);
    VoC_lw16_sd(REG0,5,DEFAULT);        //reg0->*s
    VoC_lw16i(REG7,120);
    VoC_add16_rd(REG0,REG0,CONST_40_ADDR);
// stack16 source s0 LARc RA LARpp_j LARpp_j_1 LARp
// input k_n->reg7
//   *s->reg0
    VoC_jal(CII_Short_term_analysis_filtering); //Short_term_analysis_filtering( S, LARp, 120L, s + 40);

    VoC_pop16(REG4,DEFAULT);
    VoC_pop16(REG4,DEFAULT);
    VoC_pop16(REG4,DEFAULT);
    VoC_pop16(RA,DEFAULT);
// stack16 source s0 LARc
    VoC_NOP();
    VoC_return;

}




// stack16 source so LARc LARpp_j  LARpp_j_1  LARp
// reg2->*wt        reg3->*sr      k->reg7
void CII_Short_term_synthesis_filtering ( void)
{



    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16i_set_inc(REG1,STATIC_DEC_GSM_STATE_V_ADDR+7,-1);
    VoC_lw16i_set_inc(REG0,LOCAL_DEC_LARP_ADDR+7,-1);       //LARp+7
    VoC_movreg32(RL7,REG01,DEFAULT);
    VoC_lw32_sd(ACC1,0,IN_PARALLEL);            //acc1=0x8000
    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_loop_r(1,REG7);                 //while (k--)

    VoC_lw16inc_p(REG4,REG0,DEFAULT);   //  tmp1 = rrp[i];
    VoC_lw16_p(REG6,REG1,IN_PARALLEL);  //  tmp2 = v[i];
    //  sri = *wt++;
    VoC_loop_i_short(8,DEFAULT);            //  for (i = 8; i--;)
    VoC_lw16inc_p(REG5,REG2,IN_PARALLEL);
    VoC_startloop0                  //  {
    VoC_bne16_rd(Short_term_syn_filt_L1,REG4,CONST_0x00008000_ADDR);//  tmp2 =  ( tmp1 == MIN_WORD && tmp2 == MIN_WORD

    VoC_lw16i(REG7,0x7FFF);         //      ? MAX_WORD
    VoC_be16_rd(Short_term_syn_filt_L2,REG6,CONST_0x00008000_ADDR);
Short_term_syn_filt_L1:
    VoC_multf32_rr(REG67,REG6,REG4,DEFAULT);//     : 0x0FFFF & ((word) (( (longword)tmp1 * (longword)tmp2 + 16384) >> 15))) ;
    VoC_NOP();
    VoC_add32_rr(REG67,REG67,ACC1,DEFAULT);
Short_term_syn_filt_L2:

    VoC_sub16_rr(REG5,REG5,REG7,DEFAULT);   //  sri  = (word) GSM_SUB( sri, tmp2 );
    VoC_bne16_rd(Short_term_syn_filt_L3,REG4,CONST_0x00008000_ADDR);//  tmp1  = ( tmp1 == MIN_WORD && sri == MIN_WORD
    VoC_bne16_rd(Short_term_syn_filt_L4,REG5,CONST_0x00008000_ADDR);
    VoC_lw16i(REG5,0x7FFF);         //      ? MAX_WORD
    VoC_jump(Short_term_syn_filt_L4);
Short_term_syn_filt_L3:
    VoC_multf32_rr(REG67,REG4,REG5,DEFAULT);//      : 0x0FFFF & ((word) (( (longword)tmp1 * (longword)sri+ 16384) >> 15))) ;
    VoC_NOP();
    VoC_add32_rr(REG67,REG67,ACC1,DEFAULT);
Short_term_syn_filt_L4:


    VoC_add16_rd(REG6,REG1,CONST_1_ADDR);
    VoC_add16inc_rp(REG7,REG7,REG1,DEFAULT);//  v[i+1] = (word) GSM_ADD( v[i], tmp1);
    VoC_lw16inc_p(REG4,REG0,DEFAULT);   //  tmp1 = rrp[i];
    VoC_lw16_p(REG6,REG1,IN_PARALLEL);  //  tmp2 = v[i];
    exit_on_warnings=OFF;
    VoC_sw16_p(REG7,REG6,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0                //  }

    VoC_sw16inc_p(REG5,REG3,DEFAULT);   //  *sr++ = v[0] = sri;
    VoC_movreg32(REG01,RL7,IN_PARALLEL);
    VoC_sw16_d(REG5,STATIC_DEC_GSM_STATE_V_ADDR);
    VoC_endloop1                //}
    VoC_return;
}


//  stack16  source so xMc dp dpp &Nc[k] &bc[k] &xmaxc[k] &Mc[k] ra
void CII_Weighting_filter (void )
{

    VoC_lw16i_set_inc(REG0,STATIC_ENC_E_ADDR,1);    //e -= 5;
    VoC_lw16i_set_inc(REG1,STATIC_CONST_GSM_H_ADDR,1);
    VoC_lw16i_set_inc(REG2,0,1);
    VoC_lw16i_set_inc(REG3,LOCAL_ENC_X_ADDR,1);

    VoC_movreg32(RL7,REG01,DEFAULT);
    VoC_lw16i_short(ACC1_HI,-1,IN_PARALLEL);
    VoC_lw16i(ACC1_LO,0x8000);

//  VoC_lw32_d(ACC0,CONST_0x00002000_ADDR);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i(ACC0_LO,0x2000);
    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_loop_i(1,40);               //for (k = 0; k <= 39; k++)
    //{     L_result = 8192 >> 1;
    VoC_loop_i_short(11,DEFAULT)        //  for(j=0;j<=10;j++)
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_startloop0              //  {
    VoC_mac32inc_pp(REG0,REG1,DEFAULT);//   L_result += e[k + j] * ((long) TABLE_WEIGHTING_FILTER(j));
    VoC_endloop0                //  }   //可能会溢出
    VoC_movreg32(REG01,RL7,DEFAULT);
    VoC_shr32_ri(ACC0,14,DEFAULT);      //  L_result =  ( L_result>>13 );
    VoC_add16_rr(REG0,REG0,REG2,IN_PARALLEL);

    VoC_bngt32_rr(Weighting_filter_L1,ACC1,ACC0)

//      VoC_bnlt32_rd(Weighting_filter_L1,ACC0,CONST_0xFFFF8000_ADDR)   //  x[k] =  (word) (  L_result < MIN_WORD ? MIN_WORD
    VoC_lw32_d(ACC0,CONST_0x00008000_ADDR);
    VoC_jump(Weighting_filter_L2);
Weighting_filter_L1:

    VoC_bngt32_rd(Weighting_filter_L2,ACC0,CONST_0x00007FFF_ADDR);  //      : (L_result > MAX_WORD ? MAX_WORD : L_result ));
    VoC_lw32_d(ACC0,CONST_0x00007FFF_ADDR);
Weighting_filter_L2:

    VoC_movreg32(ACC0,RL6,DEFAULT);
//      VoC_lw32_d(ACC0,CONST_0x00002000_ADDR);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(ACC0_LO,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop1        //}
    VoC_return;
}


//  stack16  source so xMc dp dpp &Nc[k] &bc[k] &xmaxc[k] &Mc[k] ra
void CII_RPE_grid_selection (void)
{
    VoC_lw16i_short(REG5,0,DEFAULT);      //EM = 0; Mc = 0;
    VoC_lw16i_set_inc(REG0,LOCAL_ENC_X_ADDR,3);
    VoC_movreg16(REG3,REG0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);            //L_result = 0;

    VoC_inc_p(REG0,DEFAULT);

    VoC_loop_i_short(12,DEFAULT)            //for(i=1;i<=12;i++)
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_startloop0                  //{
    VoC_shr16_ri(REG4,2,DEFAULT);       //  L_temp =  ( x[0 + 3 * i]>> 2 );
    VoC_mac32_rr(REG4,REG4,DEFAULT);    //  L_result += L_temp * L_temp;
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_endloop0                    //}
    VoC_lw16inc_p(REG4,REG3,DEFAULT);

    VoC_shr16_ri(REG4,2,DEFAULT);
    VoC_movreg32(RL7,ACC0,IN_PARALLEL);     //L_common_0_3 = L_result;  RL7=2*L_common_0_3

    VoC_mac32_rr(REG4,REG4,DEFAULT);        //STEP( 0, 0 );
    VoC_movreg16(REG0,REG3,DEFAULT);
    //L_result <<= 1;
    VoC_movreg32(RL6,ACC0,DEFAULT);         //EM = L_result;
    VoC_lw32z(ACC0,IN_PARALLEL);            //L_result = 0;

    VoC_loop_i_short(13,DEFAULT)            //for(i=0;i<=12;i++)
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_startloop0                  //{
    VoC_shr16_ri(REG4,2,DEFAULT);       //  L_temp =  ( x[1 + 3 * i]>> 2 );
    VoC_mac32_rr(REG4,REG4,DEFAULT);    //  L_result += L_temp * L_temp;
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_endloop0                    //}

    VoC_inc_p(REG3,DEFAULT);            //L_result <<= 1;
    VoC_movreg16(REG0,REG3,DEFAULT);
//  VoC_lw16i_short(REG6,37,IN_PARALLEL);       //2*12+1

    VoC_bngt32_rr(RPE_grid_sel_L1,ACC0,RL6);    //if (L_result > EM)
    VoC_lw16i_short(REG5,1,DEFAULT);        //{     Mc = 1;
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);     //  EM = L_result;
RPE_grid_sel_L1:                    //}

    VoC_lw32z(ACC0,DEFAULT);            //L_result = 0;
//  VoC_add16_rr(REG3,REG3,REG6,IN_PARALLEL);

    VoC_loop_i_short(13,DEFAULT)            //for(i=0;i<=12;i++)
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_startloop0                  //{
    VoC_shr16_ri(REG4,2,DEFAULT);       //  L_temp =  ( x[2 + 3 * i]>> 2 );
    VoC_mac32_rr(REG4,REG4,DEFAULT);    //  L_result += L_temp * L_temp;
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_endloop0                    //}

//  VoC_lw16_p(REG6,REG3,DEFAULT);                          //L_result <<= 1;
    VoC_lw16_d(REG6,LOCAL_ENC_X_ADDR+39);
    VoC_shr16_ri(REG6,2,DEFAULT);

    VoC_bngt32_rr(RPE_grid_sel_L2,ACC0,RL6);    //if (L_result > EM)
    VoC_lw16i_short(REG5,2,DEFAULT);        //{     Mc = 2;
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);     //  EM = L_result;
RPE_grid_sel_L2:                    //}
    VoC_multf32_rr(ACC0,REG6,REG6,DEFAULT);
    VoC_NOP();
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);        //L_result = L_common_0_3;
    //STEP( 3, 12 );  L_result <<= 1;
    VoC_bngt32_rr(RPE_grid_sel_L3,ACC0,RL6);    //if (L_result > EM)
    VoC_lw16i_short(REG5,3,DEFAULT);        //{     Mc = 3;
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);     //  EM = L_result;
RPE_grid_sel_L3:                    //}
//  stack16  source so xMc dp dpp &Nc[k] &bc[k] &xmaxc[k] &Mc[k] ra

    VoC_lw16i_set_inc(REG3,LOCAL_ENC_X_ADDR,3);
    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);
    VoC_lw16_sd(REG2,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0,LOCAL_ENC_XM_ADDR,1);
    VoC_movreg16(REG1,REG0,DEFAULT);

    VoC_loop_i_short(13,DEFAULT);       //for (i = 0; i <= 12; i ++) xM[i] = x[Mc + 3*i];
    VoC_lw16inc_p(REG7,REG3,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG3,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_sw16_p(REG5,REG2,DEFAULT);      //*Mc_out = Mc;

    VoC_return;
}


// input  :  xmaxc->reg7;
// output :  mant->reg5 ; exp->reg6
void CII_APCM_quantization_xmaxc_to_exp_mant (void)
{
    VoC_lw16i_short(REG6,0,DEFAULT);                //exp = 0;
    VoC_movreg16(REG3,REG7,DEFAULT);

    VoC_shr16_ri(REG3,3,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);
    VoC_bngt16_rd(APCM_quant_xmaxc_to_exp_mant_L1,REG7,CONST_15_ADDR);  //if (xmaxc > 15) exp =  (xmaxc>>3) - 1;
    VoC_sub16_rr(REG6,REG3,REG2,DEFAULT);
APCM_quant_xmaxc_to_exp_mant_L1:
    VoC_movreg16(REG3,REG6,DEFAULT);                //mant = xmaxc - (exp << 3);
    VoC_shr16_ri(REG3,-3,DEFAULT);
    VoC_sub16_rr(REG5,REG7,REG3,DEFAULT);

    VoC_bgtz16_r(APCM_quant_xmaxc_to_exp_mant_L2,REG5);     //if (mant == 0)
    VoC_lw16i_short(REG6,-4,DEFAULT);               //{ exp  = -4;
    VoC_lw16i_short(REG5,7,IN_PARALLEL);                //  mant = 7;
    VoC_jump(APCM_quant_xmaxc_to_exp_mant_L3);          //}
APCM_quant_xmaxc_to_exp_mant_L2:                    //else
    //{
    VoC_bgt16_rd(APCM_quant_xmaxc_to_exp_mant_L4,REG5,CONST_7_ADDR);        //  while (mant <= 7)
    VoC_shr16_ri(REG5,-1,DEFAULT);                  //  {
    VoC_sub16_rr(REG6,REG6,REG2,IN_PARALLEL);           //      exp--;
    VoC_or16_rr(REG5,REG2,DEFAULT);                 //      mant = mant << 1 | 1;
    VoC_jump(APCM_quant_xmaxc_to_exp_mant_L2);          //  }
APCM_quant_xmaxc_to_exp_mant_L4:
    VoC_sub16_rd(REG5,REG5,CONST_8_ADDR);               //  mant -= 8;
    //}
APCM_quant_xmaxc_to_exp_mant_L3:

    //assert( exp  >= -4 && exp <= 6 );
    //assert( mant >= 0 && mant <= 7 );
    //*exp_out  = exp;
    //*mant_out = mant;
    VoC_NOP();
    VoC_return;
}


//  stack16  source so xMc dp dpp &Nc[k] &bc[k] &xmaxc[k] &Mc[k] ra
//  input : reg1->xM[]
//  output :  mant->reg5 ; exp->reg6
void CII_APCM_quantization (void)
{

    VoC_push16(RA,DEFAULT);

    VoC_push16(REG1,DEFAULT);       // push16 xM[]
    VoC_lw32z(REG67,IN_PARALLEL);       //xmax = 0;


    VoC_loop_i_short(13,DEFAULT);       //for (i = 0; i <= 12; i++)
    VoC_lw16inc_p(REG5,REG1,IN_PARALLEL);
    VoC_startloop0              //{
    //  temp = xM[i];
    VoC_bnltz16_r(APCM_quant_L1,REG5);  //  temp = GSM_ABS(temp);
    VoC_sub16_rr(REG5,REG6,REG5,DEFAULT);
APCM_quant_L1:
    VoC_bngt16_rr(APCM_quant_L2,REG5,REG7); //  if (temp > xmax) xmax = temp;
    VoC_movreg16(REG7,REG5,DEFAULT);
APCM_quant_L2:
    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_endloop0                    //}


//

    VoC_lw16i_short(REG3,3,DEFAULT);
    /*
                       txdtx_ctrl  :   10    TX_SID_UPDATE;
                                                     5     TX_HANGOVER_ACTIVE  | TX_SP_FLAG;
                                                    25    TX_USE_OLD_SID|TX_HANGOVER_ACTIVE   | TX_SP_FLAG
                                                     3    TX_SP_FLAG | TX_VAD_FLAG;
      */
    //if (((txdtx_ctrl & TX_HANGOVER_ACTIVE) != 0 ) ||
    //  ((txdtx_ctrl & TX_SID_UPDATE) != 0 ))
    VoC_be16_rd(RPE_label_100, REG3, GLOBAL_txdtx_ctrl_ADDR)

    VoC_lw16i_set_inc(REG0, GLOBAL_xmax_SID_tx_ADDR+12, -4);
    VoC_lw16i_set_inc(REG1, GLOBAL_xmax_SID_tx_ADDR+8, -4);
    VoC_add16_rd(REG1, REG1, GLOBAL_subfrm_ADDR);
    VoC_add16_rd(REG0, REG0, GLOBAL_subfrm_ADDR);


    VoC_loop_i_short(3,DEFAULT)
    VoC_lw16inc_p(REG5, REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG5, REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5, REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_sw16inc_p(REG7, REG0,DEFAULT) ;


RPE_label_100:


//////////add fts end for dtx


    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC1,1,DEFAULT);



    VoC_jal(CII_APCM_quantization_sub);

    // input  :  xmaxc->reg7;
    // output :  mant->reg5 ; exp->reg6
    VoC_jal(CII_APCM_quantization_xmaxc_to_exp_mant);//APCM_quantization_xmaxc_to_exp_mant( xmaxc, &exp, &mant );

    //assert( exp <= 4096 && exp >= -4096);
    //assert( mant >= 0 && mant <= 7 );

    //*mant_out  = mant;
    //*exp_out   = exp;

//  stack16  source so xMc dp dpp &Nc[k] &bc[k] &xmaxc[k] &Mc[k] ra ra xM
    VoC_lw16_sd(REG0,4,DEFAULT);            //&xmaxc[k]
    VoC_sub16_rd(REG4,REG6,CONST_6_ADDR);       //temp1 = 6 - exp;
    VoC_sw16_p(REG7,REG0,DEFAULT);          //*xmaxc_out = xmaxc;
    VoC_lw16i(REG2,STATIC_CONST_GSM_NRFAC_ADDR);
    VoC_add16_rr(REG2,REG2,REG5,DEFAULT);
    VoC_lw16_sd(REG3,9,IN_PARALLEL);        //xMc[] in reg3

    VoC_pop16(REG0,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_pop16(RA,DEFAULT);

    VoC_lw16_p(REG7,REG2,DEFAULT);      //temp2 = gsm_NRFAC[ mant ];
    VoC_loop_i_short(13,DEFAULT);           //for (i = 0; i <= 12; i++)
    VoC_lw16inc_p(REG1,REG0,IN_PARALLEL);
    VoC_startloop0                  //{
    //  assert(temp1 >= 0 && temp1 < 16);
    VoC_shr16_rr(REG1,REG4,DEFAULT);    //  temp = xM[i] << temp1;
    VoC_multf16_rr(REG1,REG1,REG7,DEFAULT); //  temp = (word) (GSM_MULT( temp, temp2 ));
    VoC_NOP();
    VoC_shr16_ri(REG1,12,DEFAULT);      //  temp = (temp>>12);
    VoC_add16_rd(REG1,REG1,CONST_4_ADDR);   //  xMc[i] = temp + 4;
    VoC_lw16inc_p(REG1,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG1,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0    //}

//  VoC_NOP();
    VoC_return;
}


// input :  mant->reg5 ; exp->reg6  xMp->reg2  xMc->reg3  rl6_hi->Mc  rl6_lo->e
void CII_APCM_inverse_quantization (void)
{

    //assert( mant >= 0 && mant <= 7 );
    VoC_lw32_sd(ACC1,0,DEFAULT);        // 0x00008000
    VoC_lw16i(REG0,STATIC_CONST_GSM_FAC_ADDR);
    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);
    VoC_sub16_dr(REG6,CONST_6_ADDR,REG6);       //temp2 = gsm_sub( 6, exp );
    VoC_lw16_p(REG7,REG0,DEFAULT);          //temp1 = gsm_FAC[ mant ];
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_sub16_rr(REG0,REG4,REG6,DEFAULT);       //temp3 = gsm_asl( 1, (int) (gsm_sub(temp2, 1)));

    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_shr32_rr(REG45,REG0,DEFAULT);       // temp3->reg4
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_push16(REG2,DEFAULT);

    VoC_loop_i_short(13,DEFAULT);           //for (i = 13; i--;)
    VoC_lw16inc_p(REG5,REG3,IN_PARALLEL);
    VoC_startloop0                  //{
    //  assert( *xMc <= 7 && *xMc >= 0 );
    VoC_shr16_ri(REG5,-1,DEFAULT);      //  temp = (*xMc++ << 1) - 7;
    VoC_sub16_rd(REG5,REG5,CONST_7_ADDR);   //  assert( temp <= 7 && temp >= -7 );

    VoC_shr16_ri(REG5,-12,DEFAULT);     //  temp <<= 12;
    VoC_multf32_rr(REG01,REG7,REG5,DEFAULT);//  temp = (word) (GSM_MULT_R( temp1, temp ));
    VoC_NOP();
    VoC_add32_rr(REG01,REG01,ACC1,DEFAULT);

    VoC_add16_rr(REG0,REG1,REG4,DEFAULT);   //  temp = (word) (GSM_ADD( temp, temp3 ));
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    VoC_bgtz16_r(APCM_inverse_quant_L1,REG0);
    VoC_lw16i_short(REG1,-1,DEFAULT);
APCM_inverse_quant_L1:
    VoC_shr32_rr(REG01,REG6,DEFAULT);   //  *xMp++ = gsm_asr( temp, temp2 );
    VoC_lw16inc_p(REG5,REG3,DEFAULT);
    VoC_sw16inc_p(REG0,REG2,DEFAULT);
    VoC_endloop0                    //}

    VoC_return;
}

// input :  rl6_hi->Mc  rl6_lo->e  push16 xMp[]
void CII_RPE_grid_positioning (void)
{
    VoC_movreg32(REG01,RL6,DEFAULT);    //int   i = 13;

    VoC_lw32z(REG45,DEFAULT);
    VoC_pop16(REG2,IN_PARALLEL);

    VoC_lw16_p(REG1,REG1,DEFAULT);
    VoC_lw16i_short(REG7,13,IN_PARALLEL);       //assert(0 <= Mc && Mc <= 3);

    VoC_be16_rd(RPE_grid_positioning_L4,REG1,CONST_0_ADDR); //switch (Mc)
    VoC_be16_rd(RPE_grid_positioning_L3,REG1,CONST_1_ADDR);
    VoC_be16_rd(RPE_grid_positioning_L2,REG1,CONST_2_ADDR);
    //{

    VoC_sw16inc_p(REG4,REG0,DEFAULT);           //  case 3: *ep++ = 0;
RPE_grid_positioning_L2:                    //  case 2:  do {
    VoC_sw16inc_p(REG4,REG0,DEFAULT);           //          *ep++ = 0;
RPE_grid_positioning_L3:
    VoC_sw16inc_p(REG4,REG0,DEFAULT);           //  case 1:     *ep++ = 0;
RPE_grid_positioning_L4:
    VoC_lw16inc_p(REG5,REG2,DEFAULT);           //  case 0:     *ep++ = *xMp++;
    VoC_sub16_rd(REG7,REG7,CONST_1_ADDR);
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    VoC_bgtz16_r(RPE_grid_positioning_L2,REG7);     //          } while (--i);}

RPE_grid_positioning_L5:
    VoC_add16_rd(REG1,REG1,CONST_1_ADDR);       //while (++Mc < 4) *ep++ = 0;
    VoC_bgt16_rd(RPE_grid_positioning_L6,REG1,CONST_3_ADDR);
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    VoC_jump(RPE_grid_positioning_L5);
RPE_grid_positioning_L6:
    VoC_NOP();
    VoC_return;
}



//stack16  ra loop_k source so xMc dp dpp &Nc[k] &bc[k] &xmaxc[k] &Mc[k]
void CII_Gsm_RPE_Encoding (void)
{

    VoC_push16(RA,DEFAULT);

    VoC_jal(CII_Weighting_filter);      //Weighting_filter(e, x);

    VoC_jal(CII_RPE_grid_selection);    //RPE_grid_selection(x, xM, Mc);

    VoC_lw16i_set_inc(REG1,LOCAL_ENC_XM_ADDR,1);
//  stack16  source so xMc dp dpp &Nc[k] &bc[k] &xmaxc[k] &Mc[k] ra
//  input : reg1->xM[]
//  output :  mant->reg5 ; exp->reg6
    VoC_jal(CII_APCM_quantization);     //APCM_quantization(xM, xMc, &mant, &exp, xmaxc);

    VoC_lw16_sd(REG3,7,DEFAULT);            // xMc->reg3
    VoC_lw16_sd(RL6_HI,1,DEFAULT);          // rl6_hi->Mc
    VoC_lw16i(RL6_LO,STATIC_ENC_E_ADDR+5);      // rl6_lo->e
    VoC_lw16i_set_inc(REG2,LOCAL_ENC_XMP_ADDR,1);   // xMp->reg2
// input :  mant->reg5 ; exp->reg6  xMp->reg2  xMc->reg3  rl6_hi->Mc  rl6_lo->e
    VoC_jal(CII_APCM_inverse_quantization); //APCM_inverse_quantization(    xMc,  mant,  exp, xMp);
    VoC_jal(CII_RPE_grid_positioning);  //RPE_grid_positioning( *Mc, xMp, e );

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


//stack16  ra loop_k dst wt xMcr drp erp &Ncr[k] &bcr[k] &xmaxcr[k] &Mcr[k]
void CII_Gsm_RPE_Decoding (void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16_sd(REG7,2,DEFAULT);    // reg7->&xmaxcr[k]
    VoC_NOP();
    VoC_lw16_p(REG7,REG7,DEFAULT);  // reg7->xmaxc

// input  :  xmaxc->reg7;
// output :  mant->reg5 ; exp->reg6
    VoC_jal(CII_APCM_quantization_xmaxc_to_exp_mant);   //APCM_quantization_xmaxc_to_exp_mant( xmaxcr, &exp, &mant );

    VoC_lw16i_set_inc(REG2,LOCAL_DEC_XMP_ADDR,1);
    VoC_lw16_sd(REG3,7,DEFAULT);        // reg3->&xMcr[k]
    VoC_lw16_sd(RL6_HI,1,DEFAULT);      // rl6_hi->Mcr
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_lw16_sd(RL6_LO,5,DEFAULT);      // rl6_lo->erp
// input :  mant->reg5 ; exp->reg6  xMp->reg2  xMc->reg3  rl6_hi->Mc  rl6_lo->e
    VoC_jal(CII_APCM_inverse_quantization);     //APCM_inverse_quantization( xMcr, mant, exp, xMp );
    VoC_jal(CII_RPE_grid_positioning);      //RPE_grid_positioning( Mcr, xMp, erp );

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}




void CII_encoder_reset_fr(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);

    /*initial encodet gsm_state*/
    VoC_lw16i_set_inc(REG0,STATIC_ENC_GSM_STATE_ADDR,2);
    VoC_loop_i(0,161);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop0
    VoC_sw16_d(REG6,STATIC_ENC_GSM_STATE_NRP_ADDR); //r->nrp = 40;

    VoC_lw16i_set_inc(REG0,STATIC_ENC_E_ADDR,2);
    VoC_loop_i_short(25,DEFAULT);           //e [50] = {0};
    VoC_startloop0
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop0
    VoC_jal(reset_fr_table);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

void CII_decoder_reset(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);

    /*initial encodet gsm_state*/
    VoC_lw16i_set_inc(REG0,STATIC_DEC_GSM_STATE_DP0_ADDR,2);
    VoC_loop_i(0,161);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop0
    VoC_sw16_d(REG6,STATIC_DEC_GSM_STATE_NRP_ADDR); //r->nrp = 40;
    VoC_jal(reset_fr_table);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

/******************************************************************
 Function name: CII_bitoff
 input:  REG6 <- L_var1

 output  REG6 -> var_out
 Version 1.0  created by wangbin   2006/06/12

*******************************************************************/

void CII_bitoff(void)
{
    VoC_lw16i_set_inc(REG1,8,-1);
    VoC_bnez16_r(bitoff_L1,REG6);
    VoC_jump(bitoff_end);
bitoff_L1:
    VoC_bgtz16_r(bitoff_L2,REG6);
    VoC_jump(bitoff_end);
bitoff_L2:
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);
    VoC_jump(bitoff_L1);
bitoff_end:
    VoC_movreg16(REG6,REG1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_return
}
/********************************************
 function: CII_gsm_norm
 input:  REG67
 output: reg6
 used registers: reg67,reg0,acc0
*********************************************/
void CII_gsm_norm(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_push32(ACC0,DEFAULT );
    VoC_lw16i_short(REG0,0,IN_PARALLEL);
    VoC_bnltz32_r(gsm_norm_L1,REG67);           //if (a < 0)
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i(ACC0_HI,0xc000);

    VoC_bgt32_rr(gsm_norm_L2,REG67,ACC0);    //{     if (a <= -1073741824) return 0;

//  VoC_bgt32_rd(gsm_norm_L2,REG67,CONST_0xc0000000L_ADDR); //{     if (a <= -1073741824) return 0;
    VoC_lw16i(REG6,CONST_0_ADDR);
    VoC_jump(gsm_norm_Lend);
gsm_norm_L2:
    VoC_sub32_dr(REG67,CONST_0_ADDR,REG67);     //  a = ~a;
gsm_norm_L1:                        //}

    VoC_movreg32(ACC0,REG67,DEFAULT);
    VoC_and32_rd(ACC0,CONST_0xffff0000_ADDR);   //return      a & 0xffff0000
    VoC_bez32_r(gsm_norm_L3,ACC0);

//  VoC_movreg32(ACC0,REG67,DEFAULT);
//  VoC_and32_rd(ACC0,CONST_0xFF000000_ADDR);   // ? ( a & 0xff000000
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i(ACC0_HI,0xff00);
    VoC_and32_rr(ACC0,REG67,DEFAULT);



    VoC_bez32_r(gsm_norm_L4,ACC0);
    VoC_shr32_ri(REG67,24,DEFAULT);         //    ?  -1 + bitoff[ 0xFF & (a >> 24) ]
    VoC_lw16i_short(REG0,-1,IN_PARALLEL);
    VoC_jump(gsm_norm_L6);
gsm_norm_L4:
    VoC_shr32_ri(REG67,16,DEFAULT);         //        :   7 + bitoff[ 0xFF & (a >> 16) ] )
    VoC_lw16i_short(REG0,7,IN_PARALLEL);
    VoC_jump(gsm_norm_L6);
gsm_norm_L3:

//  VoC_movreg32(ACC0,REG67,DEFAULT);
//  VoC_and32_rd(ACC0,CONST_0x0000FF00_ADDR);   //      : ( a & 0xff00
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i(ACC0_LO,0xff00);
    VoC_and32_rr(ACC0,REG67,DEFAULT);

    VoC_bez32_r(gsm_norm_L5,ACC0);
    VoC_shr32_ri(REG67,8,DEFAULT);          //        ?  15 + bitoff[ 0xFF & (a >> 8) ]
    VoC_lw16i_short(REG0,15,IN_PARALLEL);
    VoC_jump(gsm_norm_L6);
gsm_norm_L5:
    VoC_lw16i_short(REG0,23,DEFAULT);       //        :  23 + bitoff[ 0xFF & a ] );
gsm_norm_L6:
    VoC_and16_rd(REG6,CONST_0x00ff_ADDR);
//  VoC_jal(CII_bitoff);
//  VoC_add16_rd(REG6,REG6,CONST_ARRAY_BITOFF_ADDR_ADDR);
gsm_norm_Lend:
    VoC_pop32(ACC0,DEFAULT);
    VoC_jal(CII_bitoff);
//  VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG0,DEFAULT);
    VoC_pop16(RA,IN_PARALLEL);
    VoC_NOP();
    VoC_return;
}

/**********************************************
INPUT:
REG0: source address INC 2
REG1: dest   address INC 2
REG6: (number of Long Words copyed)/2 or (number of Word16 copyed)/4

USED:   ACC0,ACC1
*********************************************/
void CII_copy(void)
{
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32inc_p(ACC1,REG0,DEFAULT);

    VoC_loop_r(0,REG6);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC1,REG1,DEFAULT);
    VoC_lw32inc_p(ACC1,REG0,DEFAULT);
    VoC_endloop0

    VoC_return;

}

void CII_APCM_quantization_sub(void)
{
    VoC_movreg16(REG3,REG7,DEFAULT);        //exp   = 0;    reg6=exp

    VoC_shr16_ri(REG3,9,DEFAULT);           //temp  =  ( xmax>>9 );
    VoC_lw32z(REG45,IN_PARALLEL);           //itest = 0;

    VoC_loop_i_short(6,DEFAULT);            //for (i = 0; i <= 5; i++)
    VoC_startloop0                  //{
    VoC_bgtz16_r(APCM_quant_L3s,REG3);  //  itest |= (temp <= 0);
    VoC_lw16i_short(REG5,1,DEFAULT);
APCM_quant_L3s:
    VoC_or16_rr(REG4,REG5,DEFAULT);
    VoC_shr16_ri(REG3,1,IN_PARALLEL);   //  temp =  ( temp>>1 );
    //  assert(exp <= 5);
    VoC_bnez16_r(APCM_quant_L4s,REG4);  //  if (itest == 0) exp++;
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);
APCM_quant_L4s:
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_endloop0                    //}

    //assert(exp <= 6 && exp >= 0);
    VoC_add16_rd(REG3,REG6,CONST_5_ADDR);       //temp = exp + 5;
    //assert(temp <= 11 && temp >= 0);
    VoC_shr16_rr(REG7,REG3,DEFAULT);        //xmaxc = gsm_add( (word)  (xmax>>temp), (word) (exp << 3) );
    VoC_shr16_ri(REG6,-3,IN_PARALLEL);
    VoC_add16_rr(REG7,REG7,REG6,DEFAULT);       // xmaxc->reg7

    VoC_return;
}


void CII_vad_init(void)
{
    VoC_lw16i_set_inc(REG0,STATIC_VAD_pswRvad_ADDR,2);
    VoC_lw16i(REG6,24576);
    VoC_lw16i(REG7,-16384);
    VoC_lw16i(REG4,4096);
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,IN_PARALLEL);      // rvad[0] = 24576;rvad[1] = -16384;
    VoC_lw16i(REG3,31250);
    VoC_sw32inc_p(REG45,REG0,DEFAULT);      // rvad[2] = 4096;
    VoC_lw32z(ACC0,IN_PARALLEL);


    VoC_lw16i_short(REG4,7,DEFAULT);
    VoC_lw16i_short(REG6,20,IN_PARALLEL);
    VoC_lw16i_short(REG5,-1,DEFAULT);
    VoC_lw16i_short(REG7,40,IN_PARALLEL);

    VoC_loop_i(0,73);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop0

    VoC_sw16_d(REG4,STATIC_VAD_swNormRvad_ADDR);        //normrvad = 7;
    VoC_sw16_d(REG6,STATIC_VAD_swE_thvad_ADDR);     //thvad.e = 20;
    VoC_sw16_d(REG3,STATIC_VAD_swM_thvad_ADDR);     //thvad.m = 31250;
    VoC_sw16_d(REG5,STATIC_VAD_swHangCount_ADDR);       //hangcount = -1;
    VoC_sw16_d(REG7,STATIC_VAD_swOldLag_ADDR);      //oldlag = 40;

    VoC_return;
}


/***************************************************
 Function: void CII_step_up(void)

 IN:    REG5     swNp
        REG2     &pswVpar       INCR2     1
                                INCR3     -2
        RL6_HI   &pswAav1

 OUT:   REG7     pswAav1[swNp]

 USE:   ALL REGS
 ****************************************************/
void CII_step_up_opt_fr(void)
{
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_set_inc(REG1, VAD_STEP_UP_pL_coef_ADDR, 2);       //pL_coef[m]  in REG1
    VoC_lw16i(REG7,0x2000);                 // pL_coef[0] = L_shl(0x4000L, 15);
    VoC_push16(REG1,DEFAULT);

    VoC_sw32inc_p(REG67,REG1,DEFAULT);
    VoC_lw16i_set_inc(REG0, VAD_STEP_UP_pL_work_ADDR+2, 2);
    // REG0 for VAD_STEP_UP_pL_work_ADDR+2
    // REG1 for VAD_STEP_UP_pL_coef_ADDR+2
    VoC_movreg32(ACC1,REG01,DEFAULT);

    // REG4 for m-1 (m=1 for initilization)
    VoC_lw16i_short(REG4, 0,IN_PARALLEL);

    VoC_loop_r(1, REG5);
    // pswVpar[m - 1] in REG6
    VoC_lw16inc_p(REG6, REG2, DEFAULT);
    // REG3 for pL_coef[m - i] (init i=1)
    VoC_movreg16(REG3, REG1, IN_PARALLEL);

    VoC_bez16_r(VAD_STEP_UP_LABEL1,REG4);
    VoC_lw32inc_p(RL7, REG3,DEFAULT);
    VoC_movreg32(REG01,ACC1,IN_PARALLEL);

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_movreg16(REG7, RL7_HI, IN_PARALLEL);    //swTemp in REG7
    VoC_startloop0              //for (i = 1; i < m; i++)
    VoC_multf32_rr(ACC0,REG6, REG7, DEFAULT);   // swTemp = extract_h(pL_coef[m - i]);
    VoC_NOP();
    VoC_add32inc_rp(ACC0,ACC0,REG1,DEFAULT);
    VoC_lw32inc_p(RL7, REG3,DEFAULT);   //pL_work[i] = L_mac(pL_coef[i], pswVpar[m - 1], swTemp);
    VoC_sw32inc_p(ACC0, REG0, DEFAULT);
    VoC_movreg16(REG7, RL7_HI, IN_PARALLEL);    //swTemp in REG7
    VoC_endloop0                //}

    VoC_movreg32(REG01,ACC1,DEFAULT);       // for (i = 1; i < m; i++)

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p(RL7, REG0, DEFAULT);  // pL_coef[i] = pL_work[i];
    VoC_NOP();
    VoC_sw32inc_p(RL7, REG1,DEFAULT);
    VoC_endloop0

VAD_STEP_UP_LABEL1:

    //pL_coef[m] = L_shl(L_deposit_l(pswVpar[m - 1]), 14);

    // in REG1 now is &pL_coef[m]
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_bnltz16_r(STEP_UP_01,REG6)
    VoC_lw16i_short(REG7,-1,DEFAULT);
STEP_UP_01:
    VoC_lw16i_short(REG0,1,DEFAULT);

    VoC_shr32_ri(REG67, -14, DEFAULT);
    VoC_add16_rr(REG4, REG4, REG0,DEFAULT);
    // REG1 for &pL_coef[m-1] in the following loop
    VoC_sw32_p(REG67, REG1, DEFAULT);
    VoC_endloop1

    // REG5 for loop counter(swNp+1)
    VoC_add16_rr(REG5,REG5,REG0,DEFAULT);
    VoC_pop16(REG0,IN_PARALLEL);

    VoC_movreg16(REG2, RL6_HI,DEFAULT);

    VoC_loop_r_short(REG5,DEFAULT);                 //for (i = 0; i <= swNp; i++)
    VoC_lw32inc_p(REG67, REG0, IN_PARALLEL);//pswAav1[i] = extract_h(L_shr(pL_coef[i], 3));
    VoC_startloop0
    VoC_shr32_ri(REG67, 3, DEFAULT);
    VoC_lw32inc_p(REG67, REG0, DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7, REG2, DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_return;
}

/**********************************
Function: void CII_tone_detection(void)
OUT:    REG3 pswTone
*********************************/
void CII_tone_detection_fr(void)
{


    VoC_push16(RA,DEFAULT);


    VoC_lw16i_set_inc(REG0,LOCAL_SOF_Y_ADDR,1);
    VoC_lw16i_set_inc(REG1,LOCAL_SOF_Y_ADDR+159,-1);
    VoC_push16(REG0,DEFAULT);
    VoC_lw32_sd(ACC1,0,IN_PARALLEL);    // acc1= 0x00008000
    VoC_lw16i_set_inc(REG2,STATIC_TABLE_HANN_ADDR,1);   // hann[] in RAM_X or RAM_Z

    VoC_loop_i(0,80);   //for (i = 0; i < 80; i++ )
    //{
    VoC_multf32_pp(ACC0,REG0,REG2,DEFAULT);//   sofh[i]     = gsm_mult_r (sof[i], hann[i]);
    VoC_NOP();
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_multf32_pp(RL6,REG1,REG2,DEFAULT);//    sofh[159-i] = gsm_mult_r (sof[159-i], hann[i]);
    VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);
//      VoC_add32_rr(RL6,RL6,ACC1,IN_PARALLEL);
    VoC_add32_rr(RL6,RL6,ACC1,DEFAULT);
    VoC_inc_p(REG2,DEFAULT);
    VoC_sw16inc_p(RL6_HI,REG1,DEFAULT);
    VoC_endloop0    //}


    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16i_short(REG6,0,DEFAULT);    // smax->reg6

    VoC_lw16inc_p(REG7,REG0,DEFAULT);
    VoC_loop_i(0,160);                  //for( k = 0; k<160; k++)
    VoC_bgtz16_r(TONE_DETECTION_L1,REG7);           //{
    VoC_sub16_dr(REG7,CONST_0_ADDR,REG7);           //  temp = gsm_abs( sofh[k] );
TONE_DETECTION_L1:                                              //  if ( temp > smax ) smax = temp;
    VoC_bngt16_rr(TONE_DETECTION_L2,REG7,REG6);     //}
    VoC_movreg16(REG6,REG7,DEFAULT);
TONE_DETECTION_L2:
    VoC_lw16inc_p(REG7,REG0,DEFAULT);
    VoC_endloop0

    VoC_bez16_r(TONE_DETECTION_L4,REG6);
    VoC_movreg16(ACC0_HI,REG6,DEFAULT);
    VoC_lw16i_short(ACC0_LO,0,IN_PARALLEL);
    VoC_jal(CII_NORM_L_ACC0);               //scalauto = gsm_sub( 4, gsm_norm( (longword)smax << 16));
    VoC_sub16_dr(REG5,CONST_3_ADDR,REG1);       // scalauto->reg5

    VoC_bngt16_rd(TONE_DETECTION_L4,REG5,CONST_neg1_ADDR);
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16i(REG4,16384);
    VoC_shr16_rr(REG4,REG5,DEFAULT);
    VoC_movreg16(REG2,REG0,IN_PARALLEL);        // reg0->sofh[]
    VoC_multf32inc_rp(ACC0,REG4,REG0,DEFAULT);

    VoC_loop_i(0,160);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_multf32inc_rp(ACC0,REG4,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
TONE_DETECTION_L4:
    VoC_lw16i_set_inc(REG3,160,-1);
    VoC_lw16i_set_inc(REG2,LOCAL_VAD_L_ACFH_ADDR,2);
    VoC_pop16(RL7_HI,DEFAULT);

    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_movreg16(RL7_LO,RL7_HI,DEFAULT);
    VoC_loop_i(1,5);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_movreg32(REG01,RL7,DEFAULT);
    VoC_loop_r(0,REG3);
    VoC_lw16inc_p(REG7,REG0,DEFAULT);
    VoC_mac32inc_rp(REG7,REG1,DEFAULT);
    VoC_endloop0
    VoC_add32_rd(RL7,RL7,CONST_1_ADDR);
    VoC_inc_p(REG3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_endloop1
    /*********** Computation of the reflection coefficients  **********/
    VoC_lw16i_set_inc(REG2,LOCAL_VAD_L_ACFH_ADDR,2);
    VoC_lw16i_set_inc(REG3,LOCAL_VAD_RC_Y_ADDR,1);
    VoC_lw32_p(ACC0,REG2,DEFAULT);
    VoC_lw32z(REG45,IN_PARALLEL);

    VoC_bnez32_r(TONE_DETECTION_L5,ACC0);
    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_endloop0
    VoC_jump(TONE_DETECTION_L6);
TONE_DETECTION_L5:
    VoC_jal(CII_NORM_L_ACC0);
    VoC_sub16_dr(REG4,CONST_16_ADDR,REG1);
    VoC_lw16i_set_inc(REG0,LOCAL_VAD_SACF_Y_ADDR,1);    // sacf[] & p[] share the same space    // 5
    VoC_lw16i_set_inc(REG1,LOCAL_VAD_K_Y_ADDR+4,-1);
    VoC_loop_i_short(5,DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);
    VoC_shr32_rr(ACC0,REG4,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(ACC0_LO,REG0,DEFAULT);
    VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
    VoC_endloop0

    VoC_lw32_sd(ACC1,0,DEFAULT);    // acc1= 0x00008000
    VoC_lw16i(RL6_LO,LOCAL_VAD_SACF_Y_ADDR+1);
    VoC_lw16i(RL6_HI,LOCAL_VAD_K_Y_ADDR+3);

    VoC_loop_i(1,4);
    VoC_lw32_d(REG67,LOCAL_VAD_SACF_Y_ADDR);
    VoC_movreg16(REG4,REG7,DEFAULT);
    VoC_bgtz16_r(TONE_DETECTION_L7,REG4);
    VoC_sub16_dr(REG4,CONST_0_ADDR,REG4);
TONE_DETECTION_L7:
    VoC_bngt16_rr(TONE_DETECTION_L8,REG4,REG6);
    VoC_sub16_dr(REG6,CONST_4_ADDR,REG5);
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_loop_r_short(REG6,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_endloop0
    VoC_jump(TONE_DETECTION_L6);
TONE_DETECTION_L8:
    VoC_push32(RL6,DEFAULT);
    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_movreg16(REG1,REG6,IN_PARALLEL);
    VoC_jal(CII_DIV_S);
    VoC_pop32(RL6,DEFAULT);
    VoC_movreg16(REG4,REG2,IN_PARALLEL);
    VoC_bngtz16_r(TONE_DETECTION_L9,REG7);
    VoC_sub16_dr(REG4,CONST_0_ADDR,REG4);
TONE_DETECTION_L9:
    VoC_multf32_rr(ACC0,REG7,REG4,DEFAULT);
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
//      VoC_add32_rr(ACC0,ACC0,ACC1,IN_PARALLEL);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_be16_rd(TONE_DETECTION_L6,REG5,CONST_3_ADDR);
    VoC_movreg16(REG7,ACC0_HI,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_movreg32(REG01,RL6,IN_PARALLEL);

    VoC_sub16_dr(REG7,CONST_3_ADDR,REG5);
    VoC_add16_rd(REG2,REG0,CONST_1_ADDR);
    VoC_sw16_d(REG6,LOCAL_VAD_SACF_Y_ADDR);
    VoC_loop_r_short(REG7,DEFAULT);
    VoC_startloop0
    VoC_multf32_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_NOP();
    VoC_add32_rr(REG67,ACC0,ACC1,DEFAULT);
    VoC_add16_rp(REG7,REG7,REG2,DEFAULT);
    VoC_multf32inc_rp(ACC0,REG4,REG2,DEFAULT);
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
//          VoC_add32_rr(REG67,ACC0,ACC1,IN_PARALLEL);
    VoC_add32_rr(REG67,ACC0,ACC1,DEFAULT);
    VoC_add16_rp(REG7,REG7,REG1,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0
    VoC_add16_rd(REG5,REG5,CONST_1_ADDR);
    VoC_endloop1
TONE_DETECTION_L6:
    VoC_lw32_d(REG67,LOCAL_VAD_RC_Y_ADDR);
    VoC_shr16_ri(REG6,2,DEFAULT);
    VoC_multf32_rr(ACC0,REG6,REG7,DEFAULT);
    VoC_lw32z(RL7,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
//  VoC_lw32z(RL7,IN_PARALLEL);
    VoC_shr16_ri(REG7,2,DEFAULT);       //a[2] = rc[2] >> 2;
    VoC_movreg16(REG5,ACC0_HI,IN_PARALLEL); //a[1] = gsm_add( temp, gsm_mult_r( rc[2], temp ) );
    VoC_add16_rr(REG6,REG6,REG5,DEFAULT);
    VoC_multf32_rr(RL6,REG6,REG6,DEFAULT);  //L_den  = gsm_L_mult (a[1], a[1]);
//  VoC_movreg16(RL7_HI,REG7,IN_PARALLEL);  //L_temp = gsm_L_asl ((longword)(a[2]), 16);
    VoC_movreg16(RL7_HI,REG7,DEFAULT);
    VoC_sub32_rr(RL7,RL7,RL6,DEFAULT);  //L_num  = gsm_L_sub (L_temp, L_den);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_bez32_r(TONE_DETECTION_End,RL7);
    VoC_bltz32_r(TONE_DETECTION_End,RL7);
    VoC_bnltz16_r(TONE_DETECTION_L10,REG6);
    VoC_movreg16(REG6,RL6_HI,DEFAULT);
    //FREQTH 3189
    VoC_lw16i(REG7,3189);
    VoC_multf32_rr(RL6,REG6,REG7,DEFAULT);
//  VoC_multf32_rd(RL6,REG6,CONST_3189_ADDR);   //L_den = gsm_L_mult (temp, FREQTH);
    VoC_NOP();
    VoC_bgt32_rr(TONE_DETECTION_End,RL6,RL7);//L_temp = gsm_L_sub (L_num, L_den);
TONE_DETECTION_L10:
    VoC_lw16i(REG7,0x7fff);
    VoC_lw16i_set_inc(REG1,LOCAL_VAD_RC_Y_ADDR,1);
    VoC_loop_i_short(4,DEFAULT);
    VoC_movreg16(REG5,REG7,IN_PARALLEL);
    VoC_startloop0
    VoC_multf16inc_pp(REG6,REG1,REG1,DEFAULT);// temp = gsm_mult (rc[i], rc[i]);
    VoC_NOP();
    VoC_sub16_rr(REG6,REG5,REG6,DEFAULT);   //temp = gsm_sub (0x7fff, temp);
    VoC_multf16_rr(REG7,REG7,REG6,DEFAULT); //prederr = gsm_mult (prederr, temp);
    VoC_endloop0
    //PREDTH 1464
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_bnlt16_rd(TONE_DETECTION_End,REG7,CONST_1464_ADDR); //temp = gsm_sub (prederr, PREDTH);
    VoC_lw16i_short(REG3,1,DEFAULT);
TONE_DETECTION_End:
    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG3,STATIC_VAD_tone_ADDR);
    VoC_return;
}

/***************************************************
  Function: CII_vad_computation
  input   :
  Output  :
  Description: All the input and output are finished inside

***************************************************/
void CII_vad_computation_fr(void)
{
#if 0

    voc_short BIT_PARA_TEMP_ADDR,500,x

#endif
    VoC_push16(RA, DEFAULT);


    VoC_lw16i_set_inc(REG0, LOCAL_ENC_L_ACF_ADDR, 2);
    VoC_lw16d_set_inc(REG3, STATIC_VAD_SCALVAD_ADDR, 1);
    VoC_lw16i_set_inc(REG2, STATIC_VAD_pswRvad_ADDR, 1);
    VoC_lw16_d(RL6_LO,STATIC_VAD_swNormRvad_ADDR);

    /*******************************************
     *Function inline: void CII_energy_computation_opt(void)

     *IN:  REG0    &pL_acf      INCR0=2
     *     REG3    swScaleAcf
     *     REG2    &pswRvad     INCR2=1
     *     RL6_LO  swNormRvad

     *OUT: REG45   pswE_pvad & pswM_pvad
     *     RL7     pswE_acf0 & pswM_acf0
    /*******************************************/

    VoC_lw32_d(REG45, CONST_0x80000000_ADDR);
    // ACC0 for pL_acf[0]
    VoC_lw32_p(ACC0, REG0,DEFAULT);

    VoC_movreg32(RL7, REG45, DEFAULT);
    VoC_bez32_r(ENERGY_COMPUTATION_EXIT, ACC0);

    //swNormAcf in REG1
    VoC_jal(CII_NORM_L_ACC0);


    //shl(swScaleAcf, 1)
    VoC_shr16_ri(REG3, -1, DEFAULT);
    //REG4=32
    VoC_lw16i_short(REG4,32,IN_PARALLEL);

    //*pswE_acf0 = add(32, shl(swScaleAcf, 1));
    VoC_add16_rr(REG3, REG3, REG4,DEFAULT);
    //*pswE_acf0 = sub(*pswE_acf0, swNormAcf);
    VoC_sub16_rr(REG5, REG3, REG1,DEFAULT);

    //swShift in REG1
    VoC_sub16_dr(REG1, CONST_19_ADDR,REG1);

    VoC_loop_i_short(9,DEFAULT);
    VoC_lw32z (ACC0,IN_PARALLEL);   //L_temp=0
    VoC_startloop0
    //pswSacf[i] = extract_h(L_shl(pL_acf[i], swShift));
    VoC_lw32inc_p(REG67, REG0,DEFAULT);
    VoC_shr32_rr(REG67, REG1,DEFAULT);
    VoC_mac32inc_rp (REG6, REG2, DEFAULT);
    VoC_bne16_rd(ENERGY_COMPUTATION_First,REG4,CONST_32_ADDR);      //L_temp = L_mac(L_temp, pswSacf[i], pswRvad[i]);
    VoC_shr16_ri(REG6,-3,DEFAULT);
    VoC_movreg16(REG4,REG6,DEFAULT);
    VoC_shr32_ri(ACC0, 1, DEFAULT);
    VoC_movreg32(RL7, REG45, IN_PARALLEL);
ENERGY_COMPUTATION_First:
    VoC_lw16i_short(REG4,14,DEFAULT);
    VoC_endloop0;

    //*pswE_pvad = add(*pswE_acf0, 14);
    VoC_add16_rr(REG5, REG5, REG4, DEFAULT);
    // REG4 for swNormRvad
    VoC_movreg16(REG4,RL6_LO,IN_PARALLEL);

    //*pswE_pvad = sub(*pswE_pvad, swNormRvad);
    VoC_sub16_rr(REG5, REG5, REG4, DEFAULT);

    VoC_bgtz32_r(ENERGY_COMPUTATION_100, ACC0);//L_temp in ACC0
    VoC_lw32_d(ACC0, CONST_1_ADDR);
ENERGY_COMPUTATION_100:

    // ACC0 is surely >0, so after following instrution
    // ACC0 = L_shl(L_temp, swNormProd)
    VoC_jal(CII_NORM_L_ACC0);

    //REG5 for *pswE_pvad
    VoC_sub16_rr(REG5, REG5, REG1,DEFAULT);
    //*pswM_pvad = extract_h(L_shl(L_temp, swNormProd));
    VoC_movreg16(REG4, ACC0_HI, IN_PARALLEL);

ENERGY_COMPUTATION_EXIT:

    /*******************************************
     *Function end: void CII_energy_computation_opt(void)

    /*******************************************/

    VoC_sw32_d(RL7,VAD_ALGORITHM_swM_acf0_ADDR);
    VoC_sw32_d(REG45, VAD_ALGORITHM_swM_pvad_ADDR);




    VoC_lw16d_set_inc(REG1, STATIC_VAD_SCALVAD_ADDR,2); //swScale = sub(10, shl(swScaleAcf, 1));
    VoC_lw16i_set_inc(REG0, LOCAL_ENC_L_ACF_ADDR, 2);
    VoC_lw16i_set_inc(REG2, VAD_ALGORITHM_pL_av0_ADDR, 2);
    VoC_lw16i_set_inc(REG3, VAD_ALGORITHM_pL_av1_ADDR, 2);

    /*****************************************************/
// inline function: CII_average_acf_opt

// REG0 addr of PL_ACF[0],INC0 = 2
// REG2 addr of L_AV0[0], INC2 = 2
// REG3 addr of L_AV1[0], INC3 = 2
// REG1 ScalAcf
    /****************************************************/

    VoC_shr16_ri(REG1,-1,DEFAULT);
    VoC_lw16i_short(REG7,34,IN_PARALLEL);
    VoC_sub16_dr (REG5,CONST_10_ADDR,REG1);             //scale

//    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_push32(REG23,DEFAULT);  //&L_av0[i]

    VoC_lw16i_short(REG1,2,DEFAULT);
    VoC_lw16i_set_inc(REG3,STATIC_VAD_pL_sacf_ADDR,18);         //&L_sacf[i]
    VoC_mult16_rd (REG1,REG1,STATIC_VAD_swPt_sacf_ADDR);
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_loop_i_short(9,DEFAULT)
    VoC_add16_rr (REG1, REG3, REG1,IN_PARALLEL);    //&L_sacf[pt_sacf+i]
    VoC_startloop0
    VoC_lw32inc_p(RL7,REG0,DEFAULT);
    VoC_shr32_rr(RL7,REG5,DEFAULT);         //L_temp
    VoC_add32inc_rp(RL7,RL7,REG3,DEFAULT);
    VoC_movreg32(RL6,RL7,IN_PARALLEL);
    VoC_add32inc_rp(RL7,RL7,REG3,DEFAULT);
    /*RAM_ACCESS*/
    VoC_add32_rp(RL7,RL7,REG3,DEFAULT);

    exit_on_warnings=OFF;
    VoC_sw32inc_p(RL6,REG1,DEFAULT);
    exit_on_warnings=ON;

    VoC_sw32inc_p(RL7,REG2,DEFAULT);
    VoC_sub32_rr(REG23,REG23,REG67,IN_PARALLEL);
    VoC_endloop0

//  VoC_lw16i_set_inc(REG3,STATIC_VAD_pL_sav0_ADDR,2);
    VoC_lw16i_short(REG6,2,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_mult16_rd (REG0, REG6,STATIC_VAD_swPt_sav0_ADDR);
    VoC_lw16i_set_inc(REG3,STATIC_VAD_pL_sav0_ADDR,2);
    VoC_add16_rr (REG0, REG0, REG3, DEFAULT);    // reg3 address of the L_SAV0[pt_sav0]
    VoC_pop32(REG23,IN_PARALLEL);

    /*ADDR CHANGE*/
    VoC_loop_i_short(9,DEFAULT)
    VoC_startloop0
    VoC_lw32_p(REG67,REG0,DEFAULT);
    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_sw32inc_p(REG67,REG3,DEFAULT);
    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_endloop0


    /* Update the array pointers */
    VoC_lw16i_short(REG4,18,DEFAULT);
    VoC_lw16i_short(REG5,9,IN_PARALLEL);

    VoC_add16_rd(REG6,REG5,STATIC_VAD_swPt_sacf_ADDR);  //REG6:pt_sacf

    VoC_bne16_rd(ACF_AVERAGING101,REG4,STATIC_VAD_swPt_sacf_ADDR);
    VoC_lw16i_short(REG6,0,DEFAULT);
ACF_AVERAGING101:
    VoC_lw16i_short(REG4,27,DEFAULT);
    VoC_add16_rd(REG7,REG5,STATIC_VAD_swPt_sav0_ADDR);//REG7:pt_sav0

    VoC_bne16_rd(ACF_AVERAGING103,REG4,STATIC_VAD_swPt_sav0_ADDR)
    VoC_lw16i_short(REG7,0,DEFAULT);
ACF_AVERAGING103:
    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_sw16_d(REG6,STATIC_VAD_swPt_sacf_ADDR);
    VoC_sw16_d(REG7,STATIC_VAD_swPt_sav0_ADDR);

    /*****************************************************/
// end inline function: CII_average_acf_opt

    /****************************************************/


    /************************************************
      inline function: void predictor_values()

    ************************************************/



    // load CII_schur_recursion parameters
    VoC_lw16i_set_inc(REG2, VAD_ALGORITHM_pL_av1_ADDR,2);
    VoC_lw16i(REG0, VAD_PREDICTOR_VALUES_pswVpar_ADDR);  //      pswVpar[i] = 0;

    /**********************************************
      inline Function: void CII_schur_recursion(void)
    IN:
        REG2 &pL_av1    INCR2=2
        REG0 &pswVpar
                        INCR3=1
    **********************************************/

    VoC_lw32_p(ACC0,REG2,DEFAULT);
    VoC_movreg16(ACC1_HI,REG0,DEFAULT);    // save &pswVpar in ACC1_HI
    VoC_lw16i_short(ACC1_LO,0,IN_PARALLEL);  // n in ACC1_LO
    VoC_bez32_r(schur_recursion_End1, ACC0);
    VoC_jal(CII_NORM_L_ACC0);                       //swTemp = norm_l(pL_av1[0]);

    VoC_sub16_dr(REG1,CONST_16_ADDR,REG1);
    VoC_lw16i_set_inc(REG0, SCHUR_RECURSION_pswKk_ADDR+9, -1);
    VoC_lw16i_set_inc(REG3, SCHUR_RECURSION_pswPp_ADDR, 1);

    VoC_loop_i_short(9, DEFAULT);                   //  for (i = 0; i <= 8; i++)
    VoC_startloop0                          //{
    VoC_lw32inc_p(REG67, REG2, DEFAULT);            //  pswAcf[i] = extract_h(L_shl(pL_av1[i], swTemp));
    VoC_shr32_rr(REG67, REG1, DEFAULT);             //  if ((i>0)&&(i<8))
    VoC_NOP();
    // also put values in pswKk[0,1,9] which will not be used
    // pswKk should have a space of 10 instead of 9
    VoC_sw16inc_p(REG6,REG0,DEFAULT);               //  }
    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    VoC_endloop0

    VoC_lw32_d(REG45, SCHUR_RECURSION_pswPp_ADDR);
    VoC_loop_i(1, 8);                           //if (pswPp[0] < abs_s(pswPp[1]))
    //{
    VoC_sub16_dr(REG7,CONST_0_ADDR,REG5);
    VoC_bltz16_r(abs_end,REG5);
    VoC_movreg16(REG7,REG5,DEFAULT);
abs_end:
    VoC_movreg32(REG23,ACC1,DEFAULT);
    VoC_bngt16_rr(SCHUR_RECURSION_130, REG7,REG4);          //}

schur_recursion_End1:
    VoC_movreg32(REG23,ACC1,DEFAULT);
    VoC_sub16_dr(REG2, CONST_8_ADDR, REG2);

    VoC_lw16i_short(REG7, 0,DEFAULT);
    VoC_bngtz16_r(SCHUR_RECURSION_NEWADD,REG2);
    VoC_loop_r_short(REG2,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG7, REG3, DEFAULT);
    VoC_endloop0
SCHUR_RECURSION_NEWADD:
    VoC_jump(schur_recursion_End);

SCHUR_RECURSION_130:
    VoC_movreg16(REG0,REG7,DEFAULT);            //    pswVpar[n] = divide_s(abs_s(pswPp[1]), pswPp[0]);
    VoC_movreg16(REG1,REG4,DEFAULT);

    /*********************/
// input in REG0, REG1
// output in  REG2
// used register RL6, RL7
// REG0/REG1
    /*********************/
    VoC_jal(CII_DIV_S);

    VoC_bngtz16_r(SCHUR_RECURSION_210, REG5);       //  if (pswPp[1] > 0)
    VoC_sub16_dr(REG2, CONST_0_ADDR, REG2);             //  pswVpar[n] = negate(pswVpar[n]);
SCHUR_RECURSION_210:

    VoC_multf32_rr(ACC0, REG5, REG2,DEFAULT);

    VoC_lw32_d(RL6,CONST_0x00008000L_ADDR);
    VoC_sw16inc_p(REG2, REG3, DEFAULT);


    VoC_movreg16(REG2,ACC1_LO,DEFAULT);
    //pswVpar[n]  in REG5
    VoC_movreg16(REG5,REG2,IN_PARALLEL);


    VoC_be16_rd(schur_recursion_End, REG2, CONST_7_ADDR);  //if (n == 7)     return;

    VoC_sub16_dr(REG6,CONST_7_ADDR,REG2);
    VoC_add32_rr(REG01, ACC0, RL6,DEFAULT);         //  pswPp[0] = add(pswPp[0], mult_r(pswPp[1], pswVpar[n]));
    VoC_inc_p(REG2, IN_PARALLEL);
    // in REG4 pswPp[0]
    VoC_add16_rr(REG7, REG1, REG4,DEFAULT);
    VoC_movreg32(ACC1,REG23,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1, SCHUR_RECURSION_pswPp_ADDR, 1);
    VoC_lw16i_set_inc(REG0, SCHUR_RECURSION_pswKk_ADDR+8, -1);  // pswKk[9 - m] in REG0

    VoC_add16_rd(REG2,REG1,CONST_2_ADDR);               // pswPp[1 + m] in REG2

    VoC_loop_r_short(REG6,DEFAULT)
    VoC_sw16inc_p(REG7, REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_multf32_rp(ACC0, REG5, REG0, DEFAULT);
    VoC_NOP();
    VoC_add32_rr(REG67, ACC0, RL6,DEFAULT);     //for (m = 1; m <= (7 - n); m++)
    VoC_add16_rp(REG4, REG7, REG2, DEFAULT);               //{
    VoC_multf32inc_rp(ACC0, REG5, REG2, DEFAULT);          //  pswPp[m] = add(pswPp[1 + m], mult_r(pswKk[9 - m], pswVpar[n]));
    VoC_NOP();
    VoC_add32_rr(REG67, ACC0, RL6,DEFAULT);     // }
    VoC_add16_rp(REG7, REG7, REG0, DEFAULT);
    VoC_sw16inc_p(REG4, REG1, DEFAULT);                    //  pswKk[9 - m] = add(pswKk[9 - m], mult_r(pswPp[1 + m], pswVpar[n]));
    VoC_sw16inc_p(REG7, REG0, DEFAULT);
    VoC_endloop0
    VoC_lw32_d(REG45, SCHUR_RECURSION_pswPp_ADDR);
    VoC_endloop1

schur_recursion_End:


    /********************************************************
      end inline Function: void CII_schur_recursion(void)

    **********************************************************/


    VoC_lw16i_short(REG5, 8,DEFAULT);
    VoC_lw16i_set_inc(REG3, VAD_PREDICTOR_VALUES_pswAav1_ADDR,-2);
    VoC_lw16i_set_inc(REG2, VAD_PREDICTOR_VALUES_pswVpar_ADDR,1);
    VoC_movreg16(RL6_HI,REG3,DEFAULT);

    VoC_jal(CII_step_up_opt_fr);



    // load parameters for CII_compute_rav1

    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_lw16i_set_inc(REG2, VAD_PREDICTOR_VALUES_pswAav1_ADDR, 1);  // pL_work[i] = 0;  ;REG2->pswAav1[k]
    VoC_lw16i_set_inc(REG3, VAD_ALGORITHM_pswRav1_ADDR, 1);
    /********************************************************************
      function: void compute_rav1_opt(void)

      IN:  REG2  pswAav1  INCR2=1
           REG3  pswRav1  INCR3=1
                          INCR1=1
      OUT: REG2  *pswNormRav1

    *******************************************************************/

    VoC_lw16i_set_inc(REG0, VAD_COMPUTE_RAV1_pL_work_ADDR, 2);
    VoC_lw16i_short(REG6,9, DEFAULT);
    VoC_push16(REG0,DEFAULT);
    VoC_movreg16(REG1,REG2,IN_PARALLEL);

    VoC_loop_i(1, 9);

    VoC_loop_r_short(REG6,DEFAULT); // REG6 for 9-i
    VoC_lw32z(ACC0, IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG5, REG1, DEFAULT);
    VoC_mac32inc_rp(REG5, REG2, DEFAULT);
    VoC_endloop0


    VoC_sub16_rr(REG2, REG2, REG6, DEFAULT);// REG2 for pswAav1[k]
    VoC_sub16_rd(REG6, REG6, CONST_1_ADDR);// REG6 (9-i)--
    VoC_sw32inc_p(ACC0, REG0, DEFAULT);
    VoC_sub16_rr(REG1, REG1, REG6, IN_PARALLEL);// REG1 for pswAav1[i+1]
    VoC_endloop1

    VoC_pop16(REG0,DEFAULT);
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw32_p(ACC0, REG0, DEFAULT);

    //  the condition of if(pL_work[0]==0) is not necessary
    //  because in norm_l, if the input is 0, the output is 0

    //  *pswNormRav1 = norm_l(pL_work[0]);
    /************************************************************
     *Function: CII_NORM_L_ACC0
     *input: ACC0;
     *return:REG1;
     ***********************************************************/
    VoC_jal(CII_NORM_L_ACC0);
    VoC_sub16_dr(REG2,CONST_16_ADDR,REG1);

    VoC_loop_i_short(9, DEFAULT);
    VoC_lw32inc_p(REG45, REG0, IN_PARALLEL);
    VoC_startloop0
    VoC_shr32_rr(REG45, REG2, DEFAULT);
    VoC_lw32inc_p(REG45, REG0, DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4, REG3, DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    /********************************************************************
      end function: void compute_rav1_opt(void)

    /********************************************************************/
    // save parameters for CII_compute_rav1
    VoC_sw16_d(REG1, VAD_ALGORITHM_swNormRav1_ADDR);


    /************************************************
      end inline function: void predictor_values()

    ************************************************/

    // load parameters of spectral_comparison_opt
    VoC_lw16i_set_inc(REG0, VAD_ALGORITHM_pL_av0_ADDR, 2);
    VoC_push16(REG1,DEFAULT);
    VoC_lw16i_set_inc(REG2,VAD_ALGORITHM_pswRav1_ADDR+1, 1);

    /***************************************************************************
     function spectral_comparison_opt

     IN:     REG0        &pL_av0  INCR0=2
             STACK[0]    *swNormRav1
             REG2        pswRav1[0] INCR2=1

     OUT:    REG3        *pswStat
     ****************************************************************************/
    VoC_lw32_p(ACC0, REG0,DEFAULT);
    VoC_lw16i_set_inc(REG3,LOCAL_VAD_SAV0_ADDR,1);

    VoC_bnez32_r(spectral_comp_L1,ACC0);//if (L_av0[0] == 0L)
    VoC_lw16i(REG7,0xfff);          //    {
    VoC_loop_i_short(9,DEFAULT);        //        for (i = 0; i <= 8; i++)
    VoC_startloop0
    VoC_sw16inc_p(REG7,REG3,DEFAULT);   //        { sav0[i] = 0x0fff;
    VoC_endloop0                //        }
    VoC_jump(spectral_comp_L2); //    }
spectral_comp_L1:           //    else
    VoC_jal(CII_NORM_L_ACC0);       //    {
    VoC_sub16_dr(REG1,CONST_19_ADDR,REG1);//        shift = gsm_sub  (gsm_norm (L_av0[0]), 3);
    VoC_loop_i_short(9,DEFAULT) //        for (i = 0; i <= 8; i++)
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);
    VoC_startloop0          //        {
    //            sav0[i] = (word) (gsm_L_asl (L_av0[i], shift ) >> 16);
    VoC_shr32_rr(ACC0,REG1,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(ACC0_LO,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0            //        }
spectral_comp_L2:   //    }

    VoC_lw32z(ACC0,DEFAULT);    //    L_sump = 0L;
    VoC_lw16i_set_inc(REG3,LOCAL_VAD_SAV0_ADDR+1,1);

    VoC_loop_i_short(8,DEFAULT);            //    for (i = 1; i <= 8; i++)
    VoC_startloop0                  //    {
    VoC_mac32inc_pp(REG2,REG3,DEFAULT); //  L_sump = gsm_L_add( L_sump, gsm_L_mult( rav1[i], sav0[i] ) );
    VoC_endloop0

    VoC_lw32z(REG67,DEFAULT);     //REG67:L_dm
    VoC_lw16i_short(REG1,0,IN_PARALLEL );       //REG1:swshift
    /*** Compute the division of the partial sum by sav0[0] ***/

    //ACC1:L_sump
    VoC_movreg32(ACC1,ACC0,DEFAULT);
    //ACC0:L_temp
    VoC_bnltz32_r(SPECTRAL_COMPARISON104, ACC0);
    VoC_sub32_dr(ACC0,CONST_0_ADDR,ACC0);

SPECTRAL_COMPARISON104:
    VoC_bez32_r(SPECTRAL_COMPARISON106, ACC0)//else
    VoC_lw16_d(REG5,LOCAL_VAD_SAV0_ADDR);
    VoC_shr16_ri(REG5,-3,DEFAULT);//pswSav0[0] in REG5
    /************************************************************
     *Function: CII_NORM_L_ACC0
     *input: ACC0;
     *return:REG1;
     ***********************************************************/
    VoC_jal(CII_NORM_L_ACC0);
    //push swShift (in REG1)
    VoC_push32(REG01, DEFAULT);
    //L_temp is sure to be >0, so ACC0 is L_shl(L_tmep,swShift)

    //swTemp in REG0
    VoC_movreg16(REG0, ACC0_HI, DEFAULT);

    //no need to save swDivShift here, because it is only used
    //as a flag of condition to set proper L_dm values
    VoC_bngt16_rr(SPECTRAL_COMPARISON109,REG0,REG5)
    //swTemp in REG0
    VoC_sub16_rr(REG0,REG0,REG5, DEFAULT);
    VoC_lw32_d(REG67,CONST_0x00008000L_ADDR);
SPECTRAL_COMPARISON109:

    VoC_movreg16(REG1, REG5, DEFAULT);
    /*********************/
// input in REG0, REG1
// output in  REG2
// used register RL6, RL7
// REG0/REG1
    /*********************/
    VoC_jal(CII_DIV_S);
    //swTemp in REG2
    VoC_pop32(REG01,DEFAULT);

    // L_deposit_l(swTemp)
    // REG2 is always >0, no need to contidion here
    VoC_lw16i_short(REG3,0,IN_PARALLEL );

    // REG67 L_dm
    VoC_add32_rr (REG67,REG67,REG23, DEFAULT);
    VoC_shr32_ri (REG67, -1, DEFAULT);
    VoC_bnltz32_r(SPECTRAL_COMPARISON106, ACC1);/*if (ACC1 < 0L)*/
    VoC_sub32_dr(REG67,CONST_0_ADDR,REG67);
SPECTRAL_COMPARISON106:

    /*** Re-normalization and final computation of L_dm ***/
    VoC_shr32_ri (REG67, -14, DEFAULT);
    VoC_shr32_rr (REG67, REG1, DEFAULT);/* reg67 = L_shr (reg67, reg1);*/
    VoC_lw16i_short(REG5,0,IN_PARALLEL );
    VoC_lw16_d(REG4,VAD_ALGORITHM_pswRav1_ADDR);
    // REG4 here is surely >0, so no need to condition
    // (confer to pswRav1[i] in func compute_rav1

    VoC_bnltz16_r(SPECTRAL_COMPARISON_LA1,REG4);
    VoC_lw16i_short(REG5,-1,DEFAULT );
SPECTRAL_COMPARISON_LA1:
    VoC_shr32_ri (REG45, -11, DEFAULT);/*reg45=L_shl (reg45, 11)*/
    VoC_add32_rr (REG67, REG67, REG45, DEFAULT);/*reg23=L_add(reg23,reg45)*/
    //REG4 for swNormRav1
    VoC_pop16(REG4, IN_PARALLEL);

    VoC_shr32_rr (REG67, REG4, DEFAULT);/* reg23 = L_shr (reg23, reg4);*/
    /*** Compute the difference and save L_dm ***/
    VoC_sub32_rd (REG45, REG67, STATIC_VAD_L_lastdm_ADDR);
    VoC_sw32_d(REG67, STATIC_VAD_L_lastdm_ADDR);
    VoC_bnltz32_r(SPECTRAL_COMPARISON113,REG45)
    VoC_sub32_dr(REG45,CONST_0_ADDR,REG45);//L_temp in REG45
SPECTRAL_COMPARISON113:
    /*** Evaluation of the stat flag ***/
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i(REG2,3277);   // L_temp = gsm_L_sub (L_temp, STAT_THRESH);

    VoC_bngt32_rr(SPECTRAL_COMPARISON115, REG23, REG45);/*if (reg4 < 0L)*/
    VoC_lw16i_short(REG3,1,DEFAULT);
SPECTRAL_COMPARISON115:

    /************************************************
      end inline function: void spectral_comparison()
       output :REG3->swStat
    ************************************************/

    //temp = gsm_add( oldlagcount, veryoldlagcount );
    //if ( temp >= 4 )  ptch = 1;
    //else ptch = 0;

    VoC_lw16_d(REG0, STATIC_VAD_swOldLagCount_ADDR);
    VoC_add16_rd(REG0, REG0, STATIC_VAD_swVeryOldLagCount_ADDR);
    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_blt16_rd(threshold_adaptation_100,REG0, CONST_4_ADDR);
    VoC_lw16i_short(REG1,1,DEFAULT);
threshold_adaptation_100:


    // load parameters for CII_threshold_adaptation
    VoC_movreg16(REG0,REG3,DEFAULT);        //VAD_ALGORITHM_swStat_ADDR
    VoC_lw16_d(REG3,STATIC_VAD_tone_ADDR );
    VoC_lw32_d(REG45,VAD_ALGORITHM_swM_acf0_ADDR);
    VoC_lw32_d(RL6,VAD_ALGORITHM_swM_pvad_ADDR);

    // could not put use lw32_d because the address of swM_thvad is odd
    // could not change address here because they are used in RESET functions
    VoC_lw16_d(REG6,STATIC_VAD_swM_thvad_ADDR);
    VoC_lw16_d(REG7,STATIC_VAD_swE_thvad_ADDR);

    /***************************************************************
      Function  threshold_adaptation
       *     INPUTS:    REG0              swStat
       *                REG1              swPtch
       *                REG3              swTone
       *                pswRav1[0..8]     VAD_ALGORITHM_pswRav1_ADDR
       *                swNormRav1        VAD_ALGORITHM_swNormRav1_ADDR
       *                swM_pvad          VAD_ALGORITHM_swM_pvad_ADDR
       *                swE_pvad          VAD_ALGORITHM_swE_pvad_ADDR
       *                REG4              swM_acf0
       *                REG5              swE_acf0
       *                REG6              *pswM_thvad
       *                REG7              *pswE_thvad
       *                RL6_LO            *pswM_pvad
       *                RL6_HI            *pswE_pvad
       *
       *     OUTPUTS:   pswRvad[0..8]     STATIC_VAD_pswRvad_ADDR
       *                pswNormRvad       STATIC_VAD_swNormRvad_ADDR
       *                RL7_LO            *pswM_thvad
       *                RL7_HI            *pswE_thvad

     * Version 1.1 revised by Kenneth 06/21/2004
     * Version 1.0 Create  by Lang
    ****************************************************************/

    // E_PLEV = 20
    VoC_lw16i_short(RL7_HI,20, DEFAULT);
    // E_PTH = 19
    VoC_lw16i_short(ACC0_HI,19,IN_PARALLEL);
    // M_PLEV = 25000
    VoC_lw16i(RL7_LO,25000);
    // M_PTH = 18750
    VoC_lw16i(ACC0_LO,18750);

    VoC_bgt32_rr(THRESHOLD_ADAPTATION_End,ACC0,REG45);


    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);
    // swStat & swPtch are flags, either 0 or 1
    // so swPtch==1 is equal to swPtch!=0
    // swStat==0 is equal to swStat!=1

    // REG0 for swStat
    // REG1 for swPtch
    VoC_bez16_r(THRESHOLD_ADAPTATION107,REG0);
    VoC_be16_rd(THRESHOLD_ADAPTATION107,REG1, CONST_1_ADDR);
    // REG3 for swTone
    VoC_bne16_rd(THRESHOLD_ADAPTATION10A,REG3,CONST_1_ADDR);
THRESHOLD_ADAPTATION107:
    VoC_movreg32(RL7,REG67,DEFAULT);
    VoC_sw16_d(REG5, STATIC_VAD_swAdaptCount_ADDR);
    VoC_jump(THRESHOLD_ADAPTATION_End);

THRESHOLD_ADAPTATION10A:
    VoC_add16_rd (REG5,REG4,STATIC_VAD_swAdaptCount_ADDR);      //swAdaptCount = add(swAdaptCount, 1);
    VoC_bgt16_rd(THRESHOLD_ADAPTATION108, REG5,CONST_8_ADDR);       // if (swAdaptCount <= 8) return;
    VoC_movreg32(RL7,REG67,DEFAULT);
    VoC_sw16_d(REG5, STATIC_VAD_swAdaptCount_ADDR);
    VoC_jump(THRESHOLD_ADAPTATION_End);

THRESHOLD_ADAPTATION108:
    VoC_movreg16(REG5,REG6,DEFAULT);
    VoC_shr16_ri (REG6,5,DEFAULT);
    //pswM_thvad in REG6
    VoC_sub16_rr (REG6,REG5,REG6,DEFAULT);

    //pswE_thvad in REG7
    VoC_bnlt16_rd(THRESHOLD_ADAPTATION109,REG6,CONST_0x4000_ADDR)
    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_sub16_rd (REG7, REG7,CONST_1_ADDR);
THRESHOLD_ADAPTATION109:
    /*****************************************/
    // notice here: not the same with HR

    //RL6 for swM_pvad & swE_pvad
    VoC_movreg16(REG0,RL6_LO,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    VoC_add32_rr(REG45,REG01,REG01,DEFAULT);    //L_temp = gsm_L_add( pvad.m, pvad.m );
    VoC_add32_rr(REG45,REG45,REG01,DEFAULT);    //L_temp = gsm_L_add( L_temp, pvad.m );
    VoC_lw16i_short(REG0,1,IN_PARALLEL);
    VoC_shr32_ri (REG45,1,DEFAULT);
    VoC_movreg16(REG1,RL6_HI,IN_PARALLEL);

    VoC_add16_rr(REG1, REG1, REG0, DEFAULT);    //swE_temp in REG1
    /*****************************************/

    VoC_bngt32_rd(THRESHOLD_ADAPTATION110,REG45,CONST_0x00007fffL_ADDR) //if (L_temp > 0x7fffL)

    //L_temp = L_shr(L_temp, 1);
    VoC_shr32_ri (REG45, 1, DEFAULT);
    //swE_temp = add(swE_temp, 1);
    VoC_add16_rr (REG1, REG1,REG0, IN_PARALLEL);
THRESHOLD_ADAPTATION110:
    //swM_temp in REG0
    VoC_movreg16(REG0,REG4,DEFAULT);

    // if thvad<pavd*fac
    VoC_bngt32_rr(THRESHOLD_ADAPTATION117,REG01,REG67);

    VoC_lw32z(ACC0,DEFAULT);
    //REG2:thvad.e temprarily
    VoC_movreg16(REG2,REG6,IN_PARALLEL);
    VoC_shr16_ri(REG2,4,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_movreg16(ACC0_LO,REG6,DEFAULT);

    VoC_add32_rr(REG23,ACC0,REG23,DEFAULT);     //L_temp in REG23

    VoC_bngt32_rd(THRESHOLD_ADAPTATION113,REG23, CONST_0x00007fffL_ADDR)//if
    VoC_shr32_ri (REG23,1,DEFAULT);
    VoC_add16_rd(REG7, REG7, CONST_1_ADDR);

THRESHOLD_ADAPTATION113://else
    VoC_movreg16(REG6, REG2,DEFAULT);

    VoC_bngt32_rr(THRESHOLD_ADAPTATION117, REG67, REG01) ;
    VoC_movreg32(REG67,REG01,DEFAULT);
THRESHOLD_ADAPTATION117:


    // E_MARGIN=27
    VoC_lw16i_short(REG3,27, DEFAULT);
    // M_MARGIN=19531
    VoC_lw16i(REG2,19531);

    // save thvad
    VoC_movreg32(RL7,REG67,DEFAULT);

    // RL6 for swM_pvad & swE_pvad
    // REG4:swM_pvad; REG5:swE_pvad
    VoC_movreg32(REG45, RL6, DEFAULT);


    //  put a small value in ACC1 if swE_pvad==E_Margin
    //  put 0x7FFF in ACC1 if swE_pvad != E_MARGIN
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_be16_rr(THRESHOLD_ADAPTATION120,REG3,REG5);
    VoC_lw16i(ACC1_LO,0x7fff);

    VoC_bgt16_rr(THRESHOLD_ADAPTATION120,REG5,REG3)     //if
    VoC_movreg32(REG23, REG45, DEFAULT);
    VoC_movreg32(REG45, REG23, IN_PARALLEL);
THRESHOLD_ADAPTATION120:
    VoC_sub16_rr(REG0,REG5,REG3,DEFAULT);           //swTemp in REG0

    VoC_movreg16(REG6,REG5,DEFAULT);
    VoC_shr16_rr(REG2,REG0,IN_PARALLEL);            //swTemp in REG2

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL );

    VoC_add32_rr(REG45,REG45,REG23,DEFAULT);
    // if swE_pvad==E_MARGIN ACC1=0, so branch will never do
    // if swE_pvad!=E_MARGIN ACC1=0x7FFF
    VoC_bngt32_rr(THRESHOLD_ADAPTATION121,REG45,ACC1)
    VoC_shr32_ri(REG45,1,DEFAULT);
    VoC_add16_rd(REG5, REG6, CONST_1_ADDR);
    VoC_jump(THRESHOLD_ADAPTATION124);

THRESHOLD_ADAPTATION121:                    //else
    VoC_movreg16(REG5,REG6,DEFAULT);

THRESHOLD_ADAPTATION124:
    VoC_bngt32_rr(THRESHOLD_ADAPTATION127,RL7,REG45);
    VoC_movreg32(RL7,REG45,DEFAULT);

THRESHOLD_ADAPTATION127:

    VoC_lw16_d(REG1,VAD_ALGORITHM_swNormRav1_ADDR);
    VoC_lw16i_set_inc(REG0,STATIC_VAD_pswRvad_ADDR, 1);
    VoC_sw16_d(REG1, STATIC_VAD_swNormRvad_ADDR);

    VoC_lw16i_set_inc(REG2,VAD_ALGORITHM_pswRav1_ADDR, 1);

    VoC_loop_i_short(9, DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p (REG3,REG2, DEFAULT);
    VoC_lw16i_short(REG5,9,DEFAULT);
    VoC_sw16inc_p (REG3,REG0, DEFAULT);
    VoC_endloop0

    VoC_sw16_d(REG5, STATIC_VAD_swAdaptCount_ADDR);
THRESHOLD_ADAPTATION_End:


    /************************************************
      end of inline function: void CII_threshold_adaptation(void)

    ************************************************/

    // save parameters for CII_threshold_adaptation_opt

    // could not put use sw32_d because the address of swM_thvad is odd
    // could not change address here because they are used in RESET functions

    VoC_sw16_d(RL7_LO,STATIC_VAD_swM_thvad_ADDR);
    VoC_sw16_d(RL7_HI,STATIC_VAD_swE_thvad_ADDR);
    VoC_lw32_d(RL6,VAD_ALGORITHM_swM_pvad_ADDR);

    /*******************************************************************
     *  inline Function:  void CII_vad_decision(void)
     *     INPUTS:    swM_pvad       RL6_LO
     *                swE_pvad       RL6_HI
     *                swM_thvad      RL7_LO
     *                swE_thvad      RL7_HI
     *
     *     OUTPUTS:   pswVvad        REG0
     * Version 1.1 revised by Lang 06/18/2004
     * Version 1.0 Create  by Lang
     ***************************************************************************/

    VoC_lw16i_short(REG0, 0,DEFAULT);
    VoC_bngt32_rr(DECISION_PENG_101, RL6, RL7);
    VoC_lw16i_short(REG0, 1,DEFAULT);
DECISION_PENG_101:

    /************************************************
      end inline function: void CII_vad_decision(void)

    ************************************************/


    /***********************************************************************
    inline Function: void CII_vad_hangover(void)
    IN:    REG0   swVvad
    OUT:   pswVadFlag   LOCAL_EN_SPEECHENCODER_swVadFlag_ADDR

    ***************************************************************************/

    VoC_lw32_d(REG67, STATIC_VAD_swBurstCount_ADDR);
    //REG6:burstcount;REG7:hangcount
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);
    VoC_be16_rd(VAD_HANDOVER101,REG0,CONST_1_ADDR);
    VoC_lw16i_short(REG6,0,DEFAULT);
VAD_HANDOVER101:
    VoC_blt16_rd(VAD_HANDOVER103,REG6,CONST_3_ADDR)
    VoC_lw16i_short(REG7,5,DEFAULT);
    VoC_lw16i_short(REG6,3,IN_PARALLEL);
VAD_HANDOVER103:
    VoC_bltz16_r(VAD_HANDOVER104,REG7);
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_sub16_rr (REG7,REG7,REG0, DEFAULT);
VAD_HANDOVER104:
    VoC_sw16_d(REG0, GLOBAL_OUTPUT_Vad_ADDR);
    VoC_pop16(RA, DEFAULT);
    VoC_sw32_d(REG67,STATIC_VAD_swBurstCount_ADDR);

    /***********************************************************************
    end inline Function: void CII_vad_hangover(void)

    ***************************************************************************/

    VoC_return;
}


void CII_periodicity_update_fr(void)
{
    //load parameters for periodicity_update
    VoC_lw16i_set_inc(REG0, GLOBAL_ENC_NC_ADDR, 1);

    VoC_lw16_d(REG6, STATIC_VAD_swOldLag_ADDR);
    //swLagCount in REG7
    VoC_lw16i_short(REG7, 0,DEFAULT);

    VoC_loop_i(1, 4)
    VoC_lw16_p(REG5, REG0, DEFAULT);
    //swMinLag in REG6 , swMaxLag in REG5
    VoC_bngt16_rr(PERIODICITYUPDATE101, REG6,REG5);//else
    VoC_movreg16(ACC0_LO, REG5, DEFAULT);
    VoC_movreg16(REG5, REG6,DEFAULT );
    VoC_movreg16(REG6,ACC0_LO, IN_PARALLEL );

PERIODICITYUPDATE101:
    //swSmallLag in REG5
    VoC_loop_i_short(3,DEFAULT)
    VoC_startloop0
    VoC_bgt16_rr(PERIODICITYUPDATE102, REG6, REG5);
    VoC_sub16_rr(REG5, REG5, REG6, DEFAULT);
PERIODICITYUPDATE102:
    //REG4 for *pswPtch
    //put it here to reduce a NOP
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_endloop0

    //swTemp in REG3
    VoC_sub16_rr(REG3, REG6, REG5, DEFAULT);
    /*
           if (swTemp < swSmallLag)
                swSmallLag = swTemp;
           if (swSmallLag < 2)

    is equal to
           if (swTemp <2 || swSmallLag<2)
    */
    VoC_bngt16_rr(PERIODICITYUPDATE103A, REG5,REG3);
    VoC_movreg16(REG5,REG3,DEFAULT);
PERIODICITYUPDATE103A:
    VoC_bnlt16_rd(PERIODICITYUPDATE104, REG5, CONST_2_ADDR);
    VoC_add16_rr(REG7, REG7, REG4, DEFAULT);
PERIODICITYUPDATE104:

    VoC_lw16inc_p(REG6, REG0, DEFAULT);
    VoC_endloop1

    VoC_lw16_d(REG5, STATIC_VAD_swOldLagCount_ADDR);
    VoC_sw16_d(REG6, STATIC_VAD_swOldLag_ADDR);
    VoC_sw16_d(REG7, STATIC_VAD_swOldLagCount_ADDR);
    VoC_sw16_d(REG5, STATIC_VAD_swVeryOldLagCount_ADDR);

    VoC_return;
}


/************************************************************************
  Version 1.0  2004-12-22  The first version pass all the test sequency
  Version 2.0  2004-12-28  Optimised and revise several bugs in the first version
*************************************************************************/


// VoC_directive: ALIGN

void CII_FR_Decode (void)
{

    /****************************
              FR GLOBAL
     ****************************/
    VoC_lw32_d(ACC0,CONST_0x00008000_ADDR);
    VoC_push16(RA,DEFAULT);

    //  word    LARc[8], Nc[4], Mc[4], bc[4], xmaxc[4], xmc[13*4];

    VoC_push32(ACC0,DEFAULT);

    VoC_bez16_r(gsm_decode_L1,REG2)

    VoC_jal(CII_decoder_reset);
    VoC_jal(CII_dtx_dec_init);

gsm_decode_L1:


    VoC_jal(CII_bits2prm_fr);

    // extra BFI filtering
    VoC_jal(CII_extra_bfi_filter);
    // add for dtx
    VoC_jal(CII_decoder_homing_frame_test);
    VoC_bez16_r(gsm_decode_100, REG1)

    VoC_lw16d_set_inc(REG0, DEC_OUTPUT_ADDR_ADDR, 2);
    VoC_lw16i_short(REG2,0x0008, DEFAULT);
    VoC_lw16i_short(REG3,0x0008, IN_PARALLEL);    //    for(i=0;i<160;i++)

    VoC_loop_i(0,80)
    VoC_sw32inc_p(REG23, REG0,DEFAULT);                   //        target[i] = 0x0008;
    VoC_endloop0

    VoC_jal(CII_dtx_dec_init);
    VoC_jump(gsm_decode_LEnd);
gsm_decode_100:
    VoC_jal(CII_rx_dtx_fr);

    //store nowly parameter of good speech frame
    VoC_bnez16_d(gsm_decode_120, GLOBAL_BFI_ADDR)
    VoC_bnez16_d(gsm_decode_120, GLOBAL_SID_ADDR)           //  if((bfi==0) && (SID_flag == 0))
    VoC_lw16i_set_inc(REG0,   GLOBAL_DEC_LARC_ADDR  , 2);
    VoC_lw16i_set_inc(REG1,   GLOBAL_LARc_old_rx_ADDR  , 2);

    VoC_loop_i_short(38,DEFAULT)
    VoC_lw32inc_p(ACC0, REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0, REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0, REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
gsm_decode_120:






    //no transmission period

    VoC_lw16_d(REG7, GLOBAL_rxdtx_ctrl_ADDR);
    VoC_movreg16(REG3, REG7,DEFAULT);
    VoC_and16_ri(REG3, 0x0040);  //RX_NO_TRANSMISSION
    VoC_and16_ri(REG7, 0x0020);  //RX_INVALID_SID_FRAME

    VoC_bnez16_r(gsm_decode_130, REG3)         //       if (((rxdtx_ctrl & RX_NO_TRANSMISSION) != 0)||((rxdtx_ctrl & RX_INVALID_SID_FRAME) != 0))
    VoC_bez16_r(gsm_decode_140, REG7)
gsm_decode_130:
    VoC_lw16i_set_inc(REG0,  GLOBAL_LARc_old_2_SID_rx_ADDR  , 1);
    VoC_lw16i_set_inc(REG1,  GLOBAL_LARc_old_SID_rx_ADDR  , 1);
    VoC_lw16i(RL6_LO,  GLOBAL_xmaxc_old_2_SID_rx_ADDR);
    VoC_lw16i(RL6_HI,  GLOBAL_xmaxc_old_SID_rx_ADDR);

    VoC_lw16i_set_inc(REG3,STATIC_CONST_INTERP_FACTOR_ADDR,1);//reg2 addr of interp_factor
    VoC_add16_rd(REG3,REG3,GLOBAL_rx_dtx_state_ADDR);

    VoC_lw16i_set_inc(REG2,  GLOBAL_DEC_LARC_ADDR  , 1);

    VoC_lw16_p(REG4,REG3,DEFAULT);//reg4 interp_factor[rx_dtx_state]
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_sub16_dr(REG5,CONST_0x7FFF_ADDR,REG4);//reg5 sub (0x7fff, interp_factor[rx_dtx_state]);
    VoC_add16_rd(REG5,REG5,CONST_1_ADDR);//reg5 add (temp, 1)

    VoC_lw16i(RL7_LO,  GLOBAL_DEC_XMAXC_ADDR);

    VoC_loop_i_short(12,DEFAULT);                                                      //   for(i=0;i<8;i++)
    VoC_lw32_sd(ACC0, 0,IN_PARALLEL);
    VoC_startloop0
    // LARc[i] = interpolate_CN_param (LARc_old_2_rx[i], LARc_old_rx[i], rx_dtx_state);
    VoC_mac32inc_rp( REG4, REG1,DEFAULT);
    VoC_movreg16(REG6,REG2,IN_PARALLEL);
    VoC_mac32inc_rp(REG5, REG0,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_bne16_rd(gsm_decode_1ABC,REG3,CONST_7_ADDR);
    VoC_movreg16(REG2,RL7_LO,DEFAULT);
    VoC_movreg32(REG01,RL6,IN_PARALLEL);
gsm_decode_1ABC:
    VoC_lw32_sd(ACC0, 0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16_p(ACC0_HI, REG6,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop0
    //  for(i=0;i<4;i++)
    //  xmaxc[i] = interpolate_CN_param (xmaxc_old_2_rx[i], xmaxc_old_rx[i], rx_dtx_state);
gsm_decode_140:
    VoC_lw16i_short(REG7, 0x10,DEFAULT);
    VoC_and16_rd(REG7, GLOBAL_rxdtx_ctrl_ADDR);  //RX_LOST_SID_FRAME
    VoC_bnez16_r(gsm_decode_150, REG7);
    VoC_bez16_d(gsm_decode_160, GLOBAL_lost_speech_frames_ADDR)
gsm_decode_150:
    VoC_jal(CII_lost_frame_process);

    VoC_lw16i_set_inc(REG0,GLOBAL_DEC_XMAXC_ADDR,1);
    VoC_lw16i(REG7,64);

    VoC_loop_i_short(4,DEFAULT);
    VoC_lw16inc_p(REG6, REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_bngt16_rr(gsm_decode_170,REG7, REG6)
    VoC_movreg16(REG7, REG6,DEFAULT);
gsm_decode_170:
    VoC_lw16inc_p(REG6, REG0,DEFAULT);
    VoC_endloop0
    VoC_blt16_rd(gsm_decode_180, REG7,CONST_3_ADDR)


    VoC_lw16i_short(REG6,6,DEFAULT);
    VoC_bnlt16_rd(gsm_decode_160, REG6, GLOBAL_lost_speech_frames_ADDR)
gsm_decode_180:
    VoC_jal(CII_silence_frame_paramer);
gsm_decode_160:



    VoC_lw16i_short(REG0,0x0008,DEFAULT);//RX_CONT_SID_UPDATE
    VoC_lw16_d(REG7, GLOBAL_rxdtx_ctrl_ADDR);
    VoC_and16_rr(REG0,REG7,DEFAULT);

    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_and16_ri(REG7, 0x0004);  //RX_FIRST_SID_UPDATE
    VoC_bnez16_r(gsm_decode_200, REG0)
    VoC_bez16_r(gsm_decode_200_end, REG7)
gsm_decode_200:

    /*
       VoC_lw16i_set_inc(REG2, GLOBAL_DEC_XMAXC_ADDR,1);
        VoC_lw16i_set_inc(REG1, GLOBAL_xmaxc_old_SID_rx_ADDR,1);

        VoC_lw16i_short(REG0,0,DEFAULT);
         VoC_lw16i_short(REG3,0,DEFAULT);

            VoC_loop_i_short(4,DEFAULT);
           VoC_startloop0
           VoC_add16inc_rp(REG0,REG0,REG2,DEFAULT);
           VoC_add16inc_rp(REG3,REG3,REG1,DEFAULT);
           VoC_endloop0

          VoC_sub16_rr(REG0,REG0,REG3,DEFAULT);
          VoC_bgtz16_r(gsm_decode_200_abs,REG0)
          VoC_sub16_dr(REG0,CONST_0_ADDR,REG0);
    gsm_decode_200_abs:

        VoC_bgt16_rd(gsm_decode_20000,REG0,CONST_50_ADDR) //should choose a suitable value!!
    */

    VoC_lw16i_set_inc(REG0, GLOBAL_LARc_old_2_SID_rx_ADDR,2);
    VoC_lw16i_set_inc(REG1, GLOBAL_LARc_old_SID_rx_ADDR,2);
    VoC_lw16i_set_inc(REG2, GLOBAL_DEC_LARC_ADDR,2);
    VoC_lw16i(RL6_LO,  GLOBAL_xmaxc_old_2_SID_rx_ADDR );
    VoC_lw16i(RL6_HI,  GLOBAL_xmaxc_old_SID_rx_ADDR);
    VoC_lw16i(RL7_LO,  GLOBAL_DEC_XMAXC_ADDR);

    VoC_loop_i_short(6,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32_p(ACC0, REG1,DEFAULT);
    VoC_lw32inc_p(ACC1, REG2,DEFAULT);
    VoC_sw32inc_p(ACC0, REG0,DEFAULT);
    VoC_sw32inc_p(ACC1, REG1,DEFAULT);
    VoC_bne16_rd(gsm_decode_200ABC,REG3,CONST_3_ADDR);
    VoC_movreg16(REG2,RL7_LO,DEFAULT);
    VoC_movreg32(REG01,RL6,IN_PARALLEL);
gsm_decode_200ABC:
    VoC_inc_p(REG3,DEFAULT);
    VoC_endloop0
    /*
         VoC_jump(gsm_decode_20000_end)
    gsm_decode_20000:

         VoC_lw16i_set_inc(REG0, GLOBAL_xmaxc_old_SID_rx_ADDR,2);
         VoC_lw16i_set_inc(REG2, GLOBAL_DEC_XMAXC_ADDR,2);
         VoC_lw32inc_p(ACC0,REG0,DEFAULT);
         VoC_lw32inc_p(ACC1,REG0,DEFAULT);
         VoC_sw32inc_p(ACC0,REG2,DEFAULT);
         VoC_sw32inc_p(ACC1,REG2,DEFAULT);

    gsm_decode_20000_end:
    */

    VoC_jal(CII_CN_paramer);
gsm_decode_200_end:


    VoC_jal(CII_Gsm_Decoder);   //Gsm_Decoder(s, LARc, Nc, bc, Mc, xmaxc, xmc, target);


gsm_decode_LEnd:
    VoC_pop16(RA,DEFAULT);
    VoC_pop32(ACC0,DEFAULT);

    VoC_return;
}




void CII_dtx_dec_init(void)
{
    //only set non zero value  other all have set  as 0

//send

    VoC_lw16i_short(REG1,4,DEFAULT);    // DTX_HANGOVER
    VoC_lw16i_short(REG6,1,DEFAULT);    // RX_SP_FLAG
    VoC_lw16i(REG2,0x7fff);
    VoC_lw32_d(ACC0,CONST_0x70816958_ADDR);
    VoC_sw16_d(REG1,GLOBAL_rxdtx_aver_period_ADDR);
    VoC_sw16_d(REG2,GLOBAL_rxdtx_N_elapsed_ADDR);
    VoC_sw16_d(REG6,GLOBAL_rxdtx_ctrl_ADDR);
    VoC_sw32_d(ACC0,GLOBAL_L_pn_seed_rx_ADDR);

    VoC_lw16i_short(REG1,23,DEFAULT);    // CN_INT_PERIOD - 1
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,GLOBAL_LARc_old_rx_ADDR,2);
    VoC_lw16i_set_inc(REG2,GLOBAL_Nc_old_rx_ADDR,1);
    VoC_loop_i_short(56,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
    VoC_startloop0
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop0

    VoC_sw16_d(REG1,GLOBAL_rx_dtx_state_ADDR);
    VoC_sw16_d(ACC0_LO,GLOBAL_prev_SID_frames_lost_ADDR);
    VoC_sw16_d(ACC0_LO,GLOBAL_lost_speech_frames_ADDR);

    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG6,REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_short(RL7_HI, 10,DEFAULT);
    VoC_lw16i_short(RL6_LO, 42,IN_PARALLEL);

    VoC_lw16i_short(RL7_LO, 21,DEFAULT);
    VoC_lw16i_short(RL6_HI, 39,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,GLOBAL_LARc_old_SID_rx_ADDR,2);
    VoC_lw16i_set_inc(REG2,GLOBAL_LARc_old_2_SID_rx_ADDR,2);

    VoC_sw32inc_p(RL6, REG0,DEFAULT);
    VoC_sw32inc_p(RL6, REG2,DEFAULT);

    VoC_lw16i_short(RL6_HI, 4,DEFAULT);
    VoC_lw16i_short(RL6_LO, 9,IN_PARALLEL);

    VoC_sw32inc_p(RL7, REG0,DEFAULT);
    VoC_sw32inc_p(RL7, REG2,DEFAULT);

    VoC_lw16i_short(RL7_HI, 2,IN_PARALLEL);
    VoC_lw16i_short(RL7_LO, 3,DEFAULT);

    VoC_sw32inc_p(RL6, REG0,DEFAULT);
    VoC_sw32inc_p(RL6, REG2,DEFAULT);

    VoC_sw32inc_p(RL7, REG0,DEFAULT);
    VoC_sw32inc_p(RL7, REG2,DEFAULT);
    VoC_return;

}




void CII_CN_paramer(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_set_inc(REG1, GLOBAL_DEC_BC_ADDR, 1);
    VoC_lw16i_set_inc(REG2, GLOBAL_DEC_MC_ADDR, 1);

    VoC_lw32_d(ACC0, GLOBAL_L_pn_seed_rx_ADDR  );
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_loop_i(1,4);
    VoC_sw16inc_p(REG3, REG1,DEFAULT);
    VoC_lw16i_short(REG0,2,IN_PARALLEL);
    VoC_jal(CII_pseudonoise_fr);
    VoC_sw16inc_p(REG7, REG2,DEFAULT);
    VoC_endloop1

    VoC_lw16i(RL6_LO, 40);
    VoC_lw16i(RL6_HI,120);

    VoC_lw16i_set_inc(REG1, GLOBAL_DEC_XMC_ADDR, 1);
    VoC_sw32_d(RL6, GLOBAL_DEC_NC_ADDR);
    VoC_sw32_d(RL6, GLOBAL_DEC_NC_ADDR+2);

    VoC_loop_i(1, 52)
CN_paramer_100:
    VoC_lw16i_short(REG0,3,DEFAULT);
    VoC_jal(CII_pseudonoise_fr);

    VoC_bez16_r(CN_paramer_100, REG7)
    VoC_be16_rd(CN_paramer_100, REG7,CONST_7_ADDR)
    VoC_sw16inc_p(REG7, REG1,DEFAULT);
    VoC_endloop1

    VoC_pop16(RA,DEFAULT);
    VoC_sw32_d(ACC0,GLOBAL_L_pn_seed_rx_ADDR );
    VoC_return;

}



void CII_silence_frame_paramer(void )
{


    VoC_lw16i_short(RL7_HI, 10,DEFAULT);
    VoC_lw16i_short(RL6_LO, 42,IN_PARALLEL);

    VoC_lw16i_short(RL7_LO, 21,DEFAULT);
    VoC_lw16i_short(RL6_HI, 39,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0, GLOBAL_DEC_LARC_ADDR, 2);


    VoC_lw16i_short(RL6_HI, 4,DEFAULT);
    VoC_lw16i_short(RL6_LO, 9,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_sw32inc_p(RL6, REG0,DEFAULT);
    VoC_lw16i_short(RL7_HI, 2,IN_PARALLEL);

    VoC_sw32inc_p(RL7, REG0,DEFAULT);
    VoC_lw16i_short(RL7_LO, 3,IN_PARALLEL);
    exit_on_warnings = ON;

    VoC_sw32inc_p(RL6, REG0,DEFAULT);
    VoC_lw16i_short(REG5, 40,IN_PARALLEL);
    VoC_sw32inc_p(RL7, REG0,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG3, GLOBAL_DEC_NC_ADDR, 1);
    VoC_lw16i_set_inc(REG1, GLOBAL_DEC_BC_ADDR, 1);
    VoC_lw16i_set_inc(REG2, GLOBAL_DEC_MC_ADDR, 1);
    VoC_lw16i_set_inc(REG0, GLOBAL_DEC_XMAXC_ADDR, 1);


    VoC_loop_i_short(4,DEFAULT)
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_startloop0
    VoC_sw16inc_p(REG4, REG0,DEFAULT);
    VoC_sw16inc_p(REG4, REG1,DEFAULT);
    VoC_sw16inc_p(REG6, REG2,DEFAULT);
    VoC_lw16i_short(ACC0_LO,3,IN_PARALLEL);
    VoC_sw16inc_p(REG5, REG3,DEFAULT);
    VoC_lw16i_short(ACC0_HI,3,IN_PARALLEL);
    VoC_endloop0


    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_loop_i_short(26,DEFAULT);
    VoC_lw16i_short(REG5, 4,IN_PARALLEL);
    VoC_startloop0
    VoC_sw32inc_p(ACC0, REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG0, GLOBAL_DEC_XMC_ADDR+1, 13);
    VoC_lw16i_set_inc(REG1, GLOBAL_DEC_XMC_ADDR+3, 13);
    VoC_lw16i_set_inc(REG2, GLOBAL_DEC_XMC_ADDR+4, 13);

    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG5, REG0,DEFAULT);
    VoC_sw16inc_p(REG5, REG1,DEFAULT);
    VoC_sw16inc_p(REG5, REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG0, GLOBAL_DEC_XMC_ADDR+9, 13);
    VoC_lw16i_set_inc(REG1, GLOBAL_DEC_XMC_ADDR+10, 13);

    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG5, REG0,DEFAULT);
    VoC_sw16inc_p(REG5, REG1,DEFAULT);
    VoC_endloop0

    VoC_return;

}


void CII_lost_frame_process(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16i_set_inc(REG0,   GLOBAL_LARc_old_rx_ADDR  , 2);
    VoC_lw16i_set_inc(REG1,   GLOBAL_DEC_LARC_ADDR  , 2);

    VoC_loop_i_short(38,DEFAULT)
    VoC_lw32inc_p(ACC0, REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0, REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0, REG1,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_lw16_d(REG7, GLOBAL_rxdtx_ctrl_ADDR);
    VoC_movreg16(REG1,REG7,DEFAULT);
    VoC_and16_ri(REG7, 0x0080);           //RX_DTX_MUTING

    VoC_and16_ri(REG1, RX_SP_FLAG);   //rxdtx_ctrl & RX_SP_FLAG
    VoC_lw32_d(ACC0, GLOBAL_L_pn_seed_rx_ADDR);
    VoC_lw16i_set_inc(REG2, GLOBAL_xmaxc_old_rx_ADDR, 1);
    VoC_be16_rr(lost_frame_process_300, REG1,REG6) // == 1 ?
    VoC_bez16_r(lost_frame_process_200, REG7)
    VoC_lw16i_set_inc(REG2,   GLOBAL_LARc_old_SID_rx_ADDR  , 2);
    VoC_lw16i_set_inc(REG3,   GLOBAL_DEC_LARC_ADDR  , 2);
    VoC_loop_i_short(4,DEFAULT)
    VoC_lw32inc_p(ACC0, REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0, REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0, REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_lw32_d(ACC0, GLOBAL_L_pn_seed_rx_ADDR);
    VoC_lw16i_set_inc(REG2, GLOBAL_xmaxc_old_SID_rx_ADDR, 1);
    VoC_jump(lost_frame_process_400);
lost_frame_process_300:
    VoC_be16_rd(lost_frame_process_200, REG6, GLOBAL_lost_speech_frames_ADDR  )

lost_frame_process_400:

    VoC_lw16i_set_inc(REG1, GLOBAL_DEC_XMAXC_ADDR, 1);
    VoC_lw16i_set_inc(REG3, GLOBAL_DEC_MC_ADDR, 1);

    VoC_loop_i(1,4);
    VoC_lw16_p(REG6, REG1,DEFAULT);
    VoC_blt16_rd(lost_frame_process_10,REG6,CONST_64_ADDR)
    VoC_lw16i(REG6,64);
lost_frame_process_10:
//      VoC_sub16_rd(REG6, REG6,CONST_6_ADDR);
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_bnltz16_r(lost_frame_process_100,REG6);
    VoC_lw16i_short(REG6,0,DEFAULT);
lost_frame_process_100:
    VoC_lw16i_short(REG0, 2,DEFAULT);
    VoC_sw16inc_p(REG6, REG1,DEFAULT);
    VoC_sw16inc_p(REG6, REG2,DEFAULT);
    VoC_jal(CII_pseudonoise_fr);
    VoC_sw16inc_p(REG7, REG3,DEFAULT);
    VoC_endloop1

lost_frame_process_200:
    VoC_pop16(RA,DEFAULT);
    VoC_sw32_d(ACC0, GLOBAL_L_pn_seed_rx_ADDR);
    VoC_return;
}



/*-------------------------------------------------------------------------------
  Function: void  CII_pseudonoise (void)
  Input: REG0-> no_bits
         ACC0->*shift_reg
  Output:ACC0->*shift_reg
         REG7->noise_bits
  Version 1.0  Create by Guosuozhi
  Version 1.1  Revised by Kenneth  08/02/2004
---------------------------------------------------------------------------------*/
void CII_pseudonoise_fr (void)
{
    // noise_bits=0
    VoC_lw16i_short(REG7,0,DEFAULT);   //REG7:value of noise_bits
    VoC_loop_r_short(REG0,DEFAULT)
    VoC_movreg32(ACC1,ACC0,IN_PARALLEL);
    VoC_startloop0
    //if_else_1: if ((*shift_reg & 0x00000001) != 0) {Sn = 1; }    else {Sn = 0; }
    VoC_and32_rd(ACC1,CONST_0x00000001_ADDR);
    VoC_lw16i_short(REG0,0x0001,DEFAULT);     //sn=1
    VoC_bnez32_r(PSEUDONOISE102,ACC1);
    VoC_lw16i_short(REG0,0x0000,DEFAULT);     //sn=0  REG0:value of Sn
PSEUDONOISE102:

    //  VoC_movreg32(ACC1,ACC0,DEFAULT);
    VoC_lw32z(ACC1,DEFAULT);

    VoC_lw16i(ACC1_HI,0x1000);
    VoC_and32_rr(ACC1,ACC0,DEFAULT);
    VoC_lw16i_short(REG4, 0,IN_PARALLEL);
//      VoC_and32_rd(ACC1,CONST_0x10000000_ADDR);


    VoC_bez32_r(PSEUDONOISE103,ACC1);
    VoC_lw16i_short(REG4, 1,DEFAULT);
PSEUDONOISE103:
    VoC_xor16_rr (REG0,REG4,DEFAULT);
    VoC_shr16_ri(REG7,-1,IN_PARALLEL);

    VoC_movreg32(REG45,ACC0,DEFAULT);
    VoC_and32_rd(REG45,CONST_0x00000001_ADDR);
    VoC_or16_rr (REG7,REG4,DEFAULT);

    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_and16_rr(REG4,REG0,DEFAULT);

    VoC_bez16_r(PSEUDONOISE105,REG4);
    VoC_or32_rd (ACC0,CONST_0x40000000_ADDR);
PSEUDONOISE105:;
    VoC_movreg32(ACC1,ACC0,DEFAULT);
    VoC_endloop0

    VoC_return;
}


void CII_rx_dtx_fr(void)//new
{

    VoC_lw16i_short(REG3,1,DEFAULT);// REG3 = frame_type   VALID_SID_FRAME
    VoC_lw16_d(REG2,GLOBAL_BFI_ADDR);
    VoC_lw16_d(REG1, GLOBAL_SID_ADDR);   //reg1 value of SID_flag
    VoC_bnez16_r(RX_DTX102, REG2);  //if bfi != 0
    VoC_be16_rd(RX_DTX107, REG1, CONST_2_ADDR);  //if SID_flag != 2

    VoC_bnez16_r(RX_DTX103, REG1);  //if SID_flag != 0
    VoC_lw16i_short(REG3,3,DEFAULT);//GOOD_SPEECH_FRAME
    VoC_jump(RX_DTX107);
RX_DTX102:
    VoC_lw16i_short(REG3,4,DEFAULT);//UNUSABLE_FRAME
    VoC_bez16_r(RX_DTX107, REG1);  //if SID_flag != 0
RX_DTX103:
    VoC_lw16i_short(REG3,2,DEFAULT);//INVALID_SID_FRAME

RX_DTX107: /* Update of decoder state */
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_lw16_d(REG7,GLOBAL_rxdtx_ctrl_ADDR);   //reg7 value of *rxdtx_ctrl

    VoC_and16_rr(REG0, REG7,DEFAULT);         //RX_SP_FLAG
    VoC_movreg16(REG5,REG7,IN_PARALLEL);
    VoC_lw16_d(REG6,GLOBAL_lost_speech_frames_ADDR);

    VoC_bez16_r(RX_DTX120, REG0);   //if ((*rxdtx_ctrl & RX_SP_FLAG) != 0)

    //  VALID_SID_FRAME=1
    VoC_lw16i_short(REG7,4,DEFAULT);//*rxdtx_ctrl = RX_FIRST_SID_UPDATE;
    VoC_be16_rd(RX_DTX130a, REG3,CONST_1_ADDR);

    VoC_lw16i(REG7,(4|0x20));   //  *rxdtx_ctrl = RX_FIRST_SID_UPDATE    | RX_INVALID_SID_FRAME;
    VoC_be16_rd(RX_DTX130, REG3,CONST_2_ADDR);

    VoC_lw16i_short(REG7,1,DEFAULT); // RX_SP_FLAG
    VoC_bne16_rd(RX_DTX110, REG3,CONST_4_ADDR);
    VoC_add16_rr(REG6, REG6, REG7,DEFAULT);
RX_DTX130a:
    VoC_jump(RX_DTX130);
RX_DTX110:
    VoC_movreg16(REG7,REG5,DEFAULT);
    VoC_bne16_rd(RX_DTX130,REG3,CONST_3_ADDR);
    VoC_lw16i_short(REG7,1,DEFAULT);  // RX_SP_FLAG
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_jump(RX_DTX130);
RX_DTX120: // else
    VoC_lw16i_short(REG7,0x0008,DEFAULT);//*rxdtx_ctrl = RX_CONT_SID_UPDATE;
    VoC_be16_rd(RX_DTX130, REG3,CONST_1_ADDR);

    VoC_lw16i(REG7,(0x0008|0x0020));//*rxdtx_ctrl = RX_CONT_SID_UPDATE   | RX_INVALID_SID_FRAME;
    VoC_be16_rd(RX_DTX130, REG3,CONST_2_ADDR);

    VoC_lw16i(REG7,0x0200);//RX_CNI_BFI
    VoC_be16_rd(RX_DTX130, REG3,CONST_4_ADDR);

    VoC_movreg16(REG7,REG5,DEFAULT);
    VoC_bne16_rd(RX_DTX130, REG3,CONST_3_ADDR);
    VoC_lw16i_short(REG7,1,DEFAULT );//RX_SP_FLAG
RX_DTX130:
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_sw16_d(REG6,GLOBAL_lost_speech_frames_ADDR);
    VoC_lw16_d(REG4,GLOBAL_prev_SID_frames_lost_ADDR);  //reg4:prev_sid_frames_lost
    VoC_lw16_d(REG2,GLOBAL_rx_dtx_state_ADDR);          //reg2:rxdtx_state

    VoC_and16_rr(REG0, REG7,DEFAULT);         //RX_SP_FLAG

    VoC_bez16_r(RX_DTX140, REG0); //((*rxdtx_ctrl & RX_SP_FLAG) != 0)
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i_short(REG2,(24-1),IN_PARALLEL);  //rx_dtx_state = CN_INT_PERIOD - 1;


    VoC_jump(RX_DTX160);
RX_DTX140: /* First SID frame */
    VoC_lw16i_short(REG0,4,DEFAULT);
    VoC_and16_rr(REG0, REG7,DEFAULT); //RX_FIRST_SID_UPDATE
    VoC_bez16_r(RX_DTX141, REG0); //((*rxdtx_ctrl & RX_SP_FLAG) != 0)

    VoC_lw16i_short(REG4,0,DEFAULT);                       //prev_SID_frames_lost = 0;
    VoC_lw16i_short(REG2,24-1,DEFAULT); //rx_dtx_state = CN_INT_PERIOD - 1;
    VoC_sw16_d(REG4,GLOBAL_lost_speech_frames_ADDR);     // lost_speech_frames = 0;

RX_DTX141:/* SID frame detected, but not the first SID */
    VoC_lw16i_short(REG0,8,DEFAULT);
    VoC_and16_rr(REG0, REG7,DEFAULT); // RX_CONT_SID_UPDATE

    VoC_bez16_r(RX_DTX150, REG0);//if ((*rxdtx_ctrl & RX_CONT_SID_UPDATE) != 0)
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_bne16_rd(RX_DTX142,REG3,CONST_1_ADDR);
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_jump(RX_DTX150);
RX_DTX142:
    VoC_bne16_rd(RX_DTX150, REG3,CONST_2_ADDR);
    VoC_lw16i_short(REG1,24 - 1,DEFAULT);
    VoC_bngt16_rr(RX_DTX150, REG1,REG2);

    VoC_add16_rd(REG2,REG2,CONST_1_ADDR);
    //rx_dtx_state = add(rx_dtx_state, 1);

RX_DTX150:  /* Bad frame received in CNI mode */
    VoC_movreg16(REG0,REG7,DEFAULT);
    VoC_and16_ri(REG0,0x0200);       //RX_CNI_BFI

    VoC_bez16_r(RX_DTX160,REG0);
    VoC_lw16i_short(REG1,24-1,DEFAULT);
    VoC_bngt16_rr(RX_DTX151,REG1,REG2);
    VoC_add16_rd(REG2,REG2,CONST_1_ADDR); //rx_dtx_state = add(rx_dtx_state, 1);
RX_DTX151:
    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_bne16_rd(RX_DTX152,REG1,GLOBAL_TAF_ADDR);
    VoC_or16_ri(REG7,0x0010);  //RX_LOST_SID_FRAME
    VoC_add16_rr(REG4,REG4,REG1,DEFAULT); //prev_SID_frames_lost = add (prev_SID_frames_lost, 1);
    VoC_jump(RX_DTX153);
RX_DTX152:
    VoC_or16_ri(REG7,0x0040);  //RX_NO_TRANSMISSION
RX_DTX153:
    VoC_bngt16_rr(RX_DTX160,REG4,REG1);
    VoC_or16_ri(REG7,0x0080);   //RX_DTX_MUTING
RX_DTX160://rxdtx_N_elapsed = add (rxdtx_N_elapsed, 1);
    VoC_sw16_d(REG4,GLOBAL_prev_SID_frames_lost_ADDR);  //reg4:prev_sid_frames_lost
    VoC_sw16_d(REG2,GLOBAL_rx_dtx_state_ADDR);      //reg2:rxdtx_state
    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_lw16_d(REG2,GLOBAL_rxdtx_N_elapsed_ADDR);//reg2:rxdtx_N_elapsed
    VoC_and16_rr(REG1,REG7,DEFAULT);    // RX_SP_FLAG
    VoC_add16_rr(REG2,REG2,REG1,IN_PARALLEL);        //rxdtx_N_elapsed = add (rxdtx_N_elapsed, 1);

    VoC_lw16_d(REG4,GLOBAL_rxdtx_aver_period_ADDR);//reg4:rxdtx_aver_period
    VoC_bez16_r(RX_DTX161,REG1);
    //rxdtx_aver_period = DTX_HANGOVER;
    VoC_lw16i_short(REG4,4,DEFAULT); //DTX_HANGOVER
    VoC_jump(RX_DTX170);
RX_DTX161:

    VoC_bngt16_rd(RX_DTX162,REG2,CONST_27_ADDR)  //if (sub (rxdtx_N_elapsed, DTX_ELAPSED_THRESHOLD) > 0)
    VoC_or16_ri(REG7,0x0002);//*rxdtx_ctrl |= RX_UPD_SID_QUANT_MEM;

    VoC_lw16i(REG0,0x6958);
    VoC_lw16i(REG1,0x7081); //rxdtx_N_elapsed = 0;
    VoC_lw16i_short(REG2,0,DEFAULT);    //rxdtx_aver_period = 0;
    VoC_lw16i_short(REG4,0,IN_PARALLEL);    //L_pn_seed_rx = 0x70816958L

    VoC_sw32_d(REG01,GLOBAL_L_pn_seed_rx_ADDR);
    VoC_jump(RX_DTX170);
RX_DTX162:
    VoC_bnez16_r(RX_DTX163,REG4);
    VoC_lw16i_short(REG2,0,DEFAULT);//rxdtx_N_elapsed = 0;
    VoC_jump(RX_DTX170);

RX_DTX163:
    VoC_sub16_rd(REG4,REG4,CONST_1_ADDR);
RX_DTX170:
    VoC_sw16_d(REG2,GLOBAL_rxdtx_N_elapsed_ADDR);//reg2:rxdtx_N_elapsed
    VoC_sw16_d(REG4,GLOBAL_rxdtx_aver_period_ADDR);//reg4:rxdtx_aver_period
    VoC_sw16_d(REG7,GLOBAL_rxdtx_ctrl_ADDR);   //reg7 value of *rxdtx_ctrl
    VoC_return;
}


void CII_decoder_homing_frame_test (void)
{
    //return REG1

    VoC_lw16i_set_inc(REG2, GLOBAL_DEC_LARC_ADDR,1);
    VoC_lw16i_set_inc(REG0, DEC_HOMING_LPC_PARM_ADDR,1);
    VoC_loop_i_short(8,DEFAULT);
    VoC_lw16inc_p(REG1,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG3,REG0,DEFAULT);
    VoC_xor16_rr(REG1,REG3,DEFAULT);
    VoC_bnez16_r(DEC_HOMING_LABLE,REG1);
    VoC_lw16inc_p(REG1,REG2,DEFAULT);
    VoC_endloop0;

    VoC_loop_i_short(4,DEFAULT);        // NC
    VoC_startloop0
    VoC_xor16_ri(REG1,0x28);
    VoC_bnez16_r(DEC_HOMING_LABLE,REG1);
    VoC_lw16inc_p(REG1,REG2,DEFAULT);
    VoC_endloop0;

    VoC_loop_i_short(12,DEFAULT)        // MC/BC/XMAXC
    VoC_startloop0
    VoC_xor16_ri(REG1,0x00);
    VoC_bnez16_r(DEC_HOMING_LABLE,REG1);
    VoC_lw16inc_p(REG1,REG2,DEFAULT);
    VoC_endloop0;

    //          if(i==43) j = xMcr[i] ^ 0x0003;
    //          else j = xMcr[i] ^ 0x0004;
    // 0 42  43
    //43      1
    //44 51   8

    VoC_loop_i_short(43,DEFAULT);   //GLOBAL_DEC_XMC_ADDR
    VoC_lw16i_short(REG5,0x4,IN_PARALLEL);
    VoC_startloop0
    VoC_xor16_rr(REG1,REG5,DEFAULT);
    VoC_bnez16_r(DEC_HOMING_LABLE,REG1);
    VoC_lw16inc_p(REG1,REG2,DEFAULT);
    VoC_endloop0;

    //if(i==43) j = xMcr[i] ^ 0x0003;
    VoC_xor16_ri(REG1,0x03);
    VoC_bnez16_r(DEC_HOMING_LABLE,REG1);

    VoC_loop_i_short((52-44),DEFAULT);
    VoC_lw16inc_p(REG1,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_xor16_rr(REG1,REG5,DEFAULT);
    VoC_bnez16_r(DEC_HOMING_LABLE,REG1);
    VoC_lw16inc_p(REG1,REG2,DEFAULT);
    VoC_endloop0;

    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_return;

DEC_HOMING_LABLE:
    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_return;
}



void CII_bits2prm_fr(void)
{
    VoC_lw16i_short(REG5,1,DEFAULT);

    VoC_lw16i_set_inc(REG0, INPUT_BITS2_ADDR,2);
    VoC_add16_rd(REG0,REG0,CONST_8_ADDR);


    VoC_lw16i_set_inc(REG1,BIT_PARA_TEMP_ADDR,1);

    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_lw16i_short(INC2,-1,IN_PARALLEL);

    VoC_loop_i(1,8);
    VoC_loop_i_short(32,DEFAULT);
    VoC_movreg16(REG4,REG6,IN_PARALLEL);
    VoC_startloop0
    VoC_and16_rr(REG4,REG5,DEFAULT);
    VoC_shr32_ri(REG67,1,IN_PARALLEL);
    VoC_movreg16(REG4,REG6,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_endloop1


    VoC_loop_i_short(8,DEFAULT);
    VoC_movreg16(REG4,REG6,IN_PARALLEL);
    VoC_startloop0
    VoC_and16_rr(REG4,REG5,DEFAULT);
    VoC_shr32_ri(REG67,1,IN_PARALLEL);
    VoC_movreg16(REG4,REG6,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_lw16i_set_inc(REG0,GLOBAL_DEC_LARC_ADDR,1);
    VoC_lw16i_set_inc(REG1,BIT_PARA_TEMP_ADDR,1);
    VoC_lw16i_set_inc(REG3,6,-1);


    VoC_lw16i_short(REG2,0,DEFAULT);

BITS2PRM_L1:
    VoC_blt16_rd(BITS2PRM_L2,REG3,CONST_3_ADDR);
    VoC_loop_i(1,2);
    VoC_loop_r_short(REG3,DEFAULT)
    VoC_lw16i_short(REG4,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_shr16_rr(REG5,REG2,DEFAULT);
    VoC_or16_rr(REG4,REG5,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_endloop0;
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    VoC_endloop1
    VoC_inc_p(REG3,DEFAULT);
    VoC_jump(BITS2PRM_L1);
BITS2PRM_L2:

    VoC_lw16i_set_inc(REG0,GLOBAL_DEC_XMC_ADDR,1);
    VoC_lw16i(REG6,GLOBAL_DEC_NC_ADDR);
    VoC_lw16i(REG7,GLOBAL_DEC_BC_ADDR);
    VoC_lw16i(REG4,GLOBAL_DEC_MC_ADDR);
    VoC_lw16i(REG5,GLOBAL_DEC_XMAXC_ADDR);
    VoC_movreg32(RL6,REG45,DEFAULT);
    VoC_movreg32(RL7,REG67,IN_PARALLEL);
    VoC_lw16i_short(REG3,0,DEFAULT);

    VoC_loop_i(1,4);
    VoC_loop_i_short(7,DEFAULT)
    VoC_lw16i_short(REG2,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_shr16_rr(REG5,REG2,DEFAULT);
    VoC_or16_rr(REG3,REG5,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_endloop0

    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_movreg32(REG45,RL6,IN_PARALLEL);
    VoC_sw16_p(REG3,REG6,DEFAULT);

    VoC_loop_i_short(2,DEFAULT)
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG1,DEFAULT);
    VoC_shr16_rr(REG6,REG2,DEFAULT);
    VoC_or16_rr(REG3,REG6,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_endloop0

    VoC_add32_rd(RL7,RL7,CONST_0x00010001_ADDR);
    VoC_sw16_p(REG3,REG7,DEFAULT);
    VoC_lw16i_short(REG2,0,IN_PARALLEL);


    VoC_loop_i_short(2,DEFAULT)
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG1,DEFAULT);
    VoC_shr16_rr(REG6,REG2,DEFAULT);
    VoC_or16_rr(REG3,REG6,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_endloop0

    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_sw16inc_p(REG3,REG4,DEFAULT);

    VoC_loop_i_short(6,DEFAULT)
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG1,DEFAULT);
    VoC_shr16_rr(REG6,REG2,DEFAULT);
    VoC_or16_rr(REG3,REG6,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_endloop0

    VoC_add32_rd(RL6,RL6,CONST_0x00010001_ADDR);
    VoC_sw16_p(REG3,REG5,DEFAULT);
    VoC_lw16i_short(REG2,-1,IN_PARALLEL);

    VoC_loop_i_short(13,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG1,DEFAULT);
    VoC_or16_rr(REG3,REG6,DEFAULT);
    VoC_lw16inc_p(REG6,REG1,DEFAULT);
    VoC_shr16_rr(REG6,REG2,DEFAULT);
    VoC_or16_rr(REG3,REG6,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_lw16inc_p(REG6,REG1,DEFAULT);
    VoC_shr16_rr(REG6,REG2,DEFAULT);
    VoC_or16_rr(REG3,REG6,DEFAULT);
    VoC_lw16i_short(REG2,-1,IN_PARALLEL);
    VoC_lw16i_short(REG3,0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG3,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_endloop1
    VoC_return;
}



void CII_Gsm_Decoder (void)
{
#if 0
    voc_short LOCAL_DEC_WT_ADDR,160,x
    voc_short LOCAL_DEC_ERP_ADDR,40,x
    voc_short LOCAL_VAD_L_ACFH_ADDR,10,x
    voc_short LOCAL_VAD_SAV0_ADDR,100,x
#endif

#if 0
    voc_short LOCAL_DEC_LARP_ADDR,196,y
    voc_short LOCAL_SOF_Y_ADDR,160,y
    voc_short LOCAL_VAD_SACF_Y_ADDR,6,y
    voc_short LOCAL_VAD_K_Y_ADDR,6,y
    voc_short LOCAL_VAD_RC_Y_ADDR,100,y
#endif

    // word            * drp = S->dp0 + 120;
    VoC_push16(RA,DEFAULT);
    VoC_lw16d_set_inc(REG0,DEC_OUTPUT_ADDR_ADDR,1);
    VoC_lw16i_set_inc(REG1,LOCAL_DEC_WT_ADDR,1);
    VoC_lw16i_set_inc(REG2,GLOBAL_DEC_XMC_ADDR,1);
    VoC_push16(REG7,DEFAULT);       // loop_k
    VoC_push16(REG0,DEFAULT);       // dst
    VoC_push16(REG1,DEFAULT);       // wt
    VoC_push16(REG2,DEFAULT);       // xMcr
    VoC_lw16i_set_inc(REG0,STATIC_DEC_GSM_STATE_DP0_ADDR+120,1);
    VoC_lw16i_set_inc(REG1,LOCAL_DEC_ERP_ADDR,1);
    VoC_lw16i_set_inc(REG2,GLOBAL_DEC_NC_ADDR,1);
    VoC_push16(REG0,DEFAULT);       // drp
    VoC_push16(REG1,DEFAULT);       // erp
    VoC_push16(REG2,DEFAULT);       // &Ncr[k]
    VoC_lw16i_set_inc(REG0,GLOBAL_DEC_BC_ADDR,1);
    VoC_lw16i_set_inc(REG1,GLOBAL_DEC_XMAXC_ADDR,1);
    VoC_lw16i_set_inc(REG2,GLOBAL_DEC_MC_ADDR,1);
    VoC_push16(REG0,DEFAULT);       // &bcr[k]
    VoC_push16(REG1,DEFAULT);       // &xmaxcr[k]
    VoC_push16(REG2,DEFAULT);       // &Mcr[k]
    VoC_lw16i_short(REG7,-1,IN_PARALLEL);

//stack16  ra loop_k dst wt xMcr drp erp &Ncr[k] &bcr[k] &xmaxcr[k] &Mcr[k]
    //VoC_lw16_sd(REG7,9,DEFAULT);      // loop_k
Gsm_Decoder_Loop_start:
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);           //loop_k
    VoC_bngt16_rd(Gsm_Decoder_Loop_start2,REG7,CONST_3_ADDR);   //for (j=0; j <= 3; j++)
    VoC_jump(Gsm_Decoder_Loop_End);
Gsm_Decoder_Loop_start2:

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_sw16_sd(REG7,9,DEFAULT);
    VoC_jal(CII_Gsm_RPE_Decoding);              //Gsm_RPE_Decoding( S, *xmaxcr, *Mcr, xMcr, erp );
    VoC_jal(CII_Gsm_Long_Term_Synthesis_Filtering);     //Gsm_Long_Term_Synthesis_Filtering( S, *Ncr, *bcr, erp, drp );

    VoC_lw16_sd(REG4,6,DEFAULT);            //xMcr += 13;
    VoC_lw16_sd(REG0,1,DEFAULT);            //xmaxcr++;
    VoC_lw16_sd(REG1,2,DEFAULT);            //bcr++;
    VoC_lw16_sd(REG2,3,DEFAULT);            //Ncr++;
    VoC_lw16i_short(REG5,13,IN_PARALLEL);
    VoC_lw16_sd(REG3,0,DEFAULT);            //Mcr++;
    VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);
    VoC_add32_rd(REG01,REG01,CONST_0x00010001_ADDR);
    VoC_add32_rd(REG23,REG23,CONST_0x00010001_ADDR);

    VoC_sw16_sd(REG4,6,DEFAULT);            //xMcr
    VoC_sw16_sd(REG0,1,DEFAULT);            //xmaxcr
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_sw16_sd(REG1,2,DEFAULT);            //bcr
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_sw16_sd(REG2,3,DEFAULT);            //Ncr
    VoC_lw16i_short(REG6,10,IN_PARALLEL);
    VoC_sw16_sd(REG3,0,DEFAULT);            //Mcr


    VoC_lw16_sd(REG7,9,DEFAULT);            // loop_k
    VoC_lw16i_short(REG5,40,IN_PARALLEL);
//  VoC_lw16_sd(REG1,7,DEFAULT);            // wt[]
    VoC_mult16_rr(REG5,REG5,REG7,DEFAULT);
    VoC_lw16_sd(REG1,7,DEFAULT);
    VoC_lw16_sd(REG0,5,DEFAULT);            //drp[]
    VoC_add16_rr(REG1,REG1,REG5,IN_PARALLEL);

    VoC_jal(CII_copy);              //for (k = 0; k <= 39; k++) wt[ j * 40 + k ] =  drp[ k ];
    VoC_jump(Gsm_Decoder_Loop_start);
Gsm_Decoder_Loop_End:                   //}
//stack16  ra loop_k dst wt xMcr drp erp &Ncr[k] &bcr[k] &xmaxcr[k] &Mcr[k]
    VoC_lw16i_set_inc(REG0,GLOBAL_DEC_LARC_ADDR,1);  // LARcr
    VoC_loop_i_short(6,DEFAULT);
    VoC_startloop0
    VoC_pop16(REG4,DEFAULT);
    VoC_endloop0

    VoC_sw16_sd(REG0,0,DEFAULT);
//stack16  ra loop_k dst wt LARcr
    VoC_jal(CII_Gsm_Short_Term_Synthesis_Filter);       //Gsm_Short_Term_Synthesis_Filter( S, LARcr, wt, s );
    VoC_pop16(REG0,DEFAULT);                //  REG0->LARcr
    VoC_pop16(REG7,DEFAULT);
    VoC_pop16(REG1,DEFAULT);                // reg1->dst

    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_pop16(REG6,DEFAULT);

    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_pop16(RA,DEFAULT);
    VoC_lw32_sd(ACC1,0,IN_PARALLEL);

    // Begin of Postprocessing(S, s);
    VoC_lw16_d(REG5,STATIC_DEC_GSM_STATE_MSR_ADDR);     //register word         msr = S->msr;
    VoC_lw16i(REG4,28180);
    VoC_multf32_rr(REG67,REG5,REG4,DEFAULT);

    VoC_loop_i(1,160);                  //for (k = 160; k--; s++) {
    //  tmp = (word) GSM_MULT_R( msr, 28180 );
    VoC_add32_rr(REG67,REG67,ACC1,DEFAULT);
    VoC_add16_rp(REG5,REG7,REG1,DEFAULT);       //  msr = (word) GSM_ADD(*s, tmp);
    VoC_add16_rr(REG3,REG5,REG5,DEFAULT);       //  *s  = (word) (GSM_ADD(msr, msr) & 0xFFF8);
    VoC_and16_rd(REG3,CONST_0xfff8_ADDR);
    VoC_multf32_rr(REG67,REG5,REG4,DEFAULT);
    VoC_sw16inc_p(REG3,REG1,DEFAULT);
    VoC_endloop1                        //}

    VoC_sw16_d(REG5,STATIC_DEC_GSM_STATE_MSR_ADDR);     //S->msr = msr;

    VoC_return;
}



//stack16  ra loop_k dst wt xMcr drp erp &Ncr[k] &bcr[k] &xmaxcr[k] &Mcr[k]
void CII_Gsm_Long_Term_Synthesis_Filtering (void)
{

    VoC_lw16_sd(REG6,3,DEFAULT);                // &Ncr[k]
    VoC_lw16_d(REG7,STATIC_DEC_GSM_STATE_NRP_ADDR);
    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_lw32_sd(ACC1,0,DEFAULT);            // acc1=0x8000

    VoC_blt16_rd(Gsm_Long_Term_Syn_Filt_L1,REG6,CONST_40_ADDR);//Nr = Ncr < 40 || Ncr > 120 ? S->nrp : Ncr;
    VoC_bgt16_rd(Gsm_Long_Term_Syn_Filt_L1,REG6,CONST_120_ADDR);
    VoC_movreg16(REG7,REG6,DEFAULT);
Gsm_Long_Term_Syn_Filt_L1:
    VoC_lw16_sd(REG4,2,DEFAULT);                // &bcr[k]
    VoC_lw16i(REG5,STATIC_CONST_GSM_QLB_ADDR);
    VoC_lw16_p(REG4,REG4,DEFAULT);
    VoC_sw16_d(REG7,STATIC_DEC_GSM_STATE_NRP_ADDR);     //S->nrp = Nr;
    //assert(Nr >= 40 && Nr <= 120);
    VoC_bgt16_rd(Gsm_Long_Term_Syn_Filt_LAB1,REG4,CONST_3_ADDR);
    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);
Gsm_Long_Term_Syn_Filt_LAB1:
    VoC_lw16_sd(REG0,5,DEFAULT);                // drp[]

    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_sub16_rr(REG1,REG0,REG7,DEFAULT);
    VoC_lw16_sd(REG2,4,IN_PARALLEL);            // erp[]
    VoC_lw16_p(REG5,REG5,DEFAULT);              //brp = gsm_QLB[ bcr ];
    VoC_movreg16(REG3,REG0,IN_PARALLEL);
    //assert(brp != MIN_WORD);
    VoC_multf32inc_rp(REG67,REG5,REG1,DEFAULT);
    VoC_loop_i_short(40,DEFAULT);               //for (k = 0; k <= 39; k++)
    VoC_startloop0                      //{
    //  drpp   = (word) (GSM_MULT_R( brp, drp[ k - Nr ]));
    VoC_add32_rr(REG67,REG67,ACC1,DEFAULT);
    VoC_add16inc_rp(REG4,REG7,REG2,DEFAULT);    //  drp[k] = (word) (GSM_ADD( erp[k], drpp));
    VoC_multf32inc_rp(REG67,REG5,REG1,DEFAULT);
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    VoC_endloop0                    //}


    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_sub16_rd(REG2,REG3,CONST_120_ADDR);
    VoC_sub16_rd(REG3,REG3,CONST_80_ADDR);

    VoC_loop_i_short(30,DEFAULT);   //for (k = 0; k <= 119; k++) drp[ -120 + k ] = drp[ -80 + k ];
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG3,DEFAULT);
    VoC_lw32inc_p(ACC1,REG3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);
    VoC_endloop0

    VoC_return;
}





//stack16  ra loop_k dst wt LARc
void CII_Gsm_Short_Term_Synthesis_Filter (void)
{
    //  word    * LARpp_j   = S->LARpp[ S->j ];
    //  word    * LARpp_j_1     = S->LARpp[ S->j ^=1 ];
    //  word    LARp[8];

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,STATIC_DEC_GSM_STATE_LARPP_ADDR,1);
    VoC_lw16_d(REG5,STATIC_DEC_GSM_STATE_J_ADDR);
    VoC_xor16_rr(REG6,REG5,DEFAULT);
    VoC_lw16i_short(REG7,8,IN_PARALLEL);
    VoC_mult16_rr(REG4,REG7,REG6,DEFAULT);
    VoC_mult16_rr(REG5,REG7,REG5,IN_PARALLEL);
    VoC_sw16_d(REG6,STATIC_DEC_GSM_STATE_J_ADDR);
    VoC_add16_rr(REG2,REG1,REG4,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG5,IN_PARALLEL);
    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_push16(REG1,DEFAULT);
//  VoC_sw16_d(REG6,STATIC_DEC_GSM_STATE_J_ADDR);
    VoC_push16(REG2,DEFAULT);

    VoC_lw16i_short(INC0,1,IN_PARALLEL);
// stack16 dst wt LARc RA LARpp_j LARpp_j_1
// LARc     reg0(incr=1)
// LARpp_j  reg1(incr=1)
    VoC_jal(CII_Decoding_of_the_coded_Log_Area_Ratios); //Decoding_of_the_coded_Log_Area_Ratios( LARcr, LARpp_j );




    VoC_lw16i_set_inc(REG0,LOCAL_DEC_LARP_ADDR,1);
    VoC_lw16_d(REG2, GLOBAL_rxdtx_ctrl_ADDR);
    VoC_push16(REG0,DEFAULT);           // push LARp
    VoC_and16_ri(REG2, 1);
    VoC_bez16_r(CII_Gsm_Short_Term_Synthesis_Filter_100, REG2)


// stack16 dst wt LARc RA LARpp_j LARpp_j_1
// input:  LARp->reg0(incr=1)
    VoC_jal(CII_Coefficients_0_12);             //Coefficients_0_12( LARpp_j_1, LARpp_j, LARp );
    VoC_jal(CII_LARp_to_rp);                //LARp_to_rp( LARp );

    VoC_lw16_sd(REG2,5,DEFAULT);                // reg2->*wt
    VoC_lw16_sd(REG3,6,DEFAULT);                // reg3->*sr
    VoC_lw16i_short(REG7,13,IN_PARALLEL);
// stack16 dst wt LARc ra LARpp_j  LARpp_j_1 LARp
// reg2->*wt        reg3->*sr      k->reg7
    VoC_jal(CII_Short_term_synthesis_filtering);        //Short_term_synthesis_filtering( S, LARp, 13L, wt, s );

    VoC_jal(CII_Coefficients_13_26);            //Coefficients_13_26( LARpp_j_1, LARpp_j, LARp);
    VoC_jal(CII_LARp_to_rp);                //LARp_to_rp( LARp );

    VoC_lw16_sd(REG2,5,DEFAULT);                // reg2->*wt
    VoC_lw16i_short(REG6,13,IN_PARALLEL);
    VoC_lw16_sd(REG3,6,DEFAULT);                // reg3->*sr
    VoC_add16_rr(REG2,REG2,REG6,IN_PARALLEL);
    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);
    VoC_lw16i_short(REG7,14,IN_PARALLEL);
// stack16 dst wt LARc ra LARpp_j  LARpp_j_1 LARp
// reg2->*wt        reg3->*sr      k->reg7
    VoC_jal(CII_Short_term_synthesis_filtering);        //Short_term_synthesis_filtering( S, LARp, 14L, wt + 13, s + 13 );

    VoC_jal(CII_Coefficients_27_39);            //Coefficients_27_39( LARpp_j_1, LARpp_j, LARp);
    VoC_jal(CII_LARp_to_rp);                //LARp_to_rp( LARp );

    VoC_lw16_sd(REG2,5,DEFAULT);                // reg2->*wt
    VoC_lw16i_short(REG6,27,IN_PARALLEL);
    VoC_lw16_sd(REG3,6,DEFAULT);                // reg3->*sr
    VoC_add16_rr(REG2,REG2,REG6,IN_PARALLEL);
    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);
    VoC_lw16i_short(REG7,13,IN_PARALLEL);
// stack16 dst wt LARc ra LARpp_j  LARpp_j_1 LARp
// reg2->*wt        reg3->*sr      k->reg7
    VoC_jal(CII_Short_term_synthesis_filtering);        //Short_term_synthesis_filtering( S, LARp, 13L, wt + 27, s + 27 );

    VoC_jal(CII_Coefficients_40_159);           //Coefficients_40_159( LARpp_j, LARp );
    VoC_jal(CII_LARp_to_rp);                //LARp_to_rp( LARp );

    VoC_lw16_sd(REG2,5,DEFAULT);                // reg2->*wt
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
    VoC_lw16_sd(REG3,6,DEFAULT);                // reg3->*sr
    VoC_add16_rr(REG2,REG2,REG6,IN_PARALLEL);
    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);
    VoC_lw16i(REG7,120);
// stack16 dst wt LARc ra LARpp_j  LARpp_j_1 LARp
// reg2->*wt        reg3->*sr      k->reg7
    VoC_jal(CII_Short_term_synthesis_filtering);        //Short_term_synthesis_filtering(S, LARp, 120L, wt + 40, s + 40);

    VoC_jump(CII_Gsm_Short_Term_Synthesis_Filter_100_end);
CII_Gsm_Short_Term_Synthesis_Filter_100:


    VoC_jal(CII_Coefficients_40_159);           //Coefficients_40_159( LARpp_j, LARp );
    VoC_jal(CII_LARp_to_rp);                //LARp_to_rp( LARp );

    VoC_lw16_sd(REG2,5,DEFAULT);                // reg2->*wt
    VoC_lw16_sd(REG3,6,DEFAULT);                // reg3->*sr

    VoC_lw16i(REG7,160);
// stack16 dst wt LARc ra LARpp_j  LARpp_j_1 LARp
// reg2->*wt        reg3->*sr      k->reg7
    VoC_jal(CII_Short_term_synthesis_filtering);        //  FILTER(S, LARp, 160L, wt, s );

CII_Gsm_Short_Term_Synthesis_Filter_100_end:

    VoC_pop16(REG4,DEFAULT);
    VoC_pop16(REG4,DEFAULT);
    VoC_pop16(REG4,DEFAULT);
    VoC_pop16(RA,DEFAULT);
// stack16 dst wt LARc
    VoC_NOP();
    VoC_return;
}

void reset_fr_table(void)
{

#define  gsm_DLB0    6554
#define  gsm_DLB1    16384
#define  gsm_DLB2    26214
#define  gsm_DLB3    32767
    VoC_lw16i(REG6,gsm_DLB0);
    VoC_lw16i(REG7,gsm_DLB1);
    VoC_lw16i_set_inc(REG0,STATIC_CONST_GSM_DLB_ADDR,2);
    VoC_lw16i(ACC0_LO,gsm_DLB2);
    VoC_lw16i(ACC0_HI,gsm_DLB3);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

#define  gsm_QLB0    3277
#define  gsm_QLB1    11469
#define  gsm_QLB2    21299
#define  gsm_QLB3    32767

    VoC_lw16i(REG6,gsm_QLB0);
    VoC_lw16i(REG7,gsm_QLB1);
    VoC_lw16i(ACC0_LO,gsm_QLB2);
    VoC_lw16i(ACC0_HI,gsm_QLB3);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

#define  gsm_NRFAC0    29128
#define  gsm_NRFAC1    26215
#define  gsm_NRFAC2    23832
#define  gsm_NRFAC3    21846

#define  gsm_NRFAC4    20165
#define  gsm_NRFAC5    18725
#define  gsm_NRFAC6    17476
#define  gsm_NRFAC7    16384

    VoC_lw16i(REG6,gsm_NRFAC0);
    VoC_lw16i(REG7,gsm_NRFAC1);
    VoC_lw16i(ACC0_LO,gsm_NRFAC2);
    VoC_lw16i(ACC0_HI,gsm_NRFAC3);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

    VoC_lw16i(REG6,gsm_NRFAC4);
    VoC_lw16i(REG7,gsm_NRFAC5);
    VoC_lw16i(ACC0_LO,gsm_NRFAC6);
    VoC_lw16i(ACC0_HI,gsm_NRFAC7);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

#define  gsm_FAC0    18431
#define  gsm_FAC1    20479
#define  gsm_FAC2    22527
#define  gsm_FAC3    24575

#define  gsm_FAC4    26623
#define  gsm_FAC5    28671
#define  gsm_FAC6    30719
#define  gsm_FAC7    32767

    VoC_lw16i(REG6,gsm_FAC0);
    VoC_lw16i(REG7,gsm_FAC1);
    VoC_lw16i(ACC0_LO,gsm_FAC2);
    VoC_lw16i(ACC0_HI,gsm_FAC3);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

    VoC_lw16i(REG6,gsm_FAC4);
    VoC_lw16i(REG7,gsm_FAC5);
    VoC_lw16i(ACC0_LO,gsm_FAC6);
    VoC_lw16i(ACC0_HI,gsm_FAC7);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

#define  DEC_homing_LPC0    0x0009
#define  DEC_homing_LPC1    0x0017
#define  DEC_homing_LPC2    0x000f
#define  DEC_homing_LPC3    0x0008

#define  DEC_homing_LPC4    0x0007
#define  DEC_homing_LPC5    0x0003
#define  DEC_homing_LPC6    0x0003
#define  DEC_homing_LPC7    0x0002

    VoC_lw16i(REG6,DEC_homing_LPC0);
    VoC_lw16i(REG7,DEC_homing_LPC1);
    VoC_lw16i(ACC0_LO,DEC_homing_LPC2);
    VoC_lw16i(ACC0_HI,DEC_homing_LPC3);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,DEC_homing_LPC4);
    VoC_lw16i(REG7,DEC_homing_LPC5);
    VoC_lw16i(ACC0_LO,DEC_homing_LPC6);
    VoC_lw16i(ACC0_HI,DEC_homing_LPC7);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

#define  gsm_H0    -134
#define  gsm_H1     -374
#define  gsm_H2    0
#define  gsm_H3    2054

#define  gsm_H4    5741
#define  gsm_H5    8192
#define  gsm_H6    5741
#define  gsm_H7    2054

#define  gsm_H8    0
#define  gsm_H9    -374
#define  gsm_H10    -134
#define  gsm_H11    0


    VoC_lw16i(REG6,gsm_H0);
    VoC_lw16i(REG7,gsm_H1);
    VoC_lw16i(ACC0_LO,gsm_H2);
    VoC_lw16i(ACC0_HI,gsm_H3);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

    VoC_lw16i(REG6,gsm_H4);
    VoC_lw16i(REG7,gsm_H5);
    VoC_lw16i(ACC0_LO,gsm_H6);
    VoC_lw16i(ACC0_HI,gsm_H7);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

    VoC_lw16i(REG6,gsm_H8);
    VoC_lw16i(REG7,gsm_H9);
    VoC_lw16i(ACC0_LO,gsm_H10);
    VoC_lw16i(ACC0_HI,gsm_H11);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

#define  hann1_table0    0
#define  hann1_table1    12
#define  hann1_table2    51
#define  hann1_table3    114


#define  hann1_table4    204
#define  hann1_table5    318
#define  hann1_table6    458
#define  hann1_table7    622

#define  hann1_table8    811
#define  hann1_table9    1025
#define  hann1_table10   1262
#define  hann1_table11   1523

#define  hann1_table12    1807
#define  hann1_table13    2114
#define  hann1_table14    2444
#define  hann1_table15    2795

#define  hann1_table16    3167
#define  hann1_table17    3560
#define  hann1_table18    3972
#define  hann1_table19    4405

#define  hann1_table20    4856
#define  hann1_table21    5325
#define  hann1_table22    5811
#define  hann1_table23    6314


#define  hann1_table24    6832
#define  hann1_table25    7365
#define  hann1_table26    7913
#define  hann1_table27    8473

#define  hann1_table28    9046
#define  hann1_table29    9631


#define  hann1_table30    10226
#define  hann1_table31    10831


#define  hann1_table32    11444
#define  hann1_table33    12065
#define  hann1_table34    12693
#define  hann1_table35    13326

#define  hann1_table36    13964
#define  hann1_table37    14607
#define  hann1_table38    15251
#define  hann1_table39    15898


#define  hann1_table40    16545
#define  hann1_table41    17192
#define  hann1_table42    17838
#define  hann1_table43    18482

#define  hann1_table44    19122
#define  hann1_table45    19758
#define  hann1_table46    20389
#define  hann1_table47    21014


#define  hann1_table48    21631
#define  hann1_table49    22240
#define  hann1_table50    22840
#define  hann1_table51    23430

#define  hann1_table52    24009
#define  hann1_table53    24575
#define  hann1_table54    25130
#define  hann1_table55    25670

#define  hann1_table56    26196
#define  hann1_table57    26707
#define  hann1_table58    27201
#define  hann1_table59    27679


#define  hann1_table60    28139
#define  hann1_table61    28581
#define  hann1_table62    29003
#define  hann1_table63    29406

#define  hann1_table64    29789
#define  hann1_table65    30151
#define  hann1_table66    30491
#define  hann1_table67    30809



#define  hann1_table68    31105
#define  hann1_table69    31377
#define  hann1_table70    31626
#define  hann1_table71    31852

#define  hann1_table72    32053
#define  hann1_table73    32230
#define  hann1_table74    32382
#define  hann1_table75    32509

#define  hann1_table76    32611
#define  hann1_table77    32688
#define  hann1_table78    32739
#define  hann1_table79    32764


    VoC_lw16i(REG6,hann1_table0);
    VoC_lw16i(REG7,hann1_table1);
    VoC_lw16i(ACC0_LO,hann1_table2);
    VoC_lw16i(ACC0_HI,hann1_table3);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);





    VoC_lw16i(REG6,hann1_table4);
    VoC_lw16i(REG7,hann1_table5);
    VoC_lw16i(ACC0_LO,hann1_table6);
    VoC_lw16i(ACC0_HI,hann1_table7);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table8);
    VoC_lw16i(REG7,hann1_table9);
    VoC_lw16i(ACC0_LO,hann1_table10);
    VoC_lw16i(ACC0_HI,hann1_table11);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table12);
    VoC_lw16i(REG7,hann1_table13);
    VoC_lw16i(ACC0_LO,hann1_table14);
    VoC_lw16i(ACC0_HI,hann1_table15);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table16);
    VoC_lw16i(REG7,hann1_table17);
    VoC_lw16i(ACC0_LO,hann1_table18);
    VoC_lw16i(ACC0_HI,hann1_table19);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table20);
    VoC_lw16i(REG7,hann1_table21);
    VoC_lw16i(ACC0_LO,hann1_table22);
    VoC_lw16i(ACC0_HI,hann1_table23);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table24);
    VoC_lw16i(REG7,hann1_table25);
    VoC_lw16i(ACC0_LO,hann1_table26);
    VoC_lw16i(ACC0_HI,hann1_table27);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table28);
    VoC_lw16i(REG7,hann1_table29);
    VoC_lw16i(ACC0_LO,hann1_table30);
    VoC_lw16i(ACC0_HI,hann1_table31);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table32);
    VoC_lw16i(REG7,hann1_table33);
    VoC_lw16i(ACC0_LO,hann1_table34);
    VoC_lw16i(ACC0_HI,hann1_table35);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table36);
    VoC_lw16i(REG7,hann1_table37);
    VoC_lw16i(ACC0_LO,hann1_table38);
    VoC_lw16i(ACC0_HI,hann1_table39);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table40);
    VoC_lw16i(REG7,hann1_table41);
    VoC_lw16i(ACC0_LO,hann1_table42);
    VoC_lw16i(ACC0_HI,hann1_table43);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table44);
    VoC_lw16i(REG7,hann1_table45);
    VoC_lw16i(ACC0_LO,hann1_table46);
    VoC_lw16i(ACC0_HI,hann1_table47);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table48);
    VoC_lw16i(REG7,hann1_table49);
    VoC_lw16i(ACC0_LO,hann1_table50);
    VoC_lw16i(ACC0_HI,hann1_table51);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table52);
    VoC_lw16i(REG7,hann1_table53);
    VoC_lw16i(ACC0_LO,hann1_table54);
    VoC_lw16i(ACC0_HI,hann1_table55);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table56);
    VoC_lw16i(REG7,hann1_table57);
    VoC_lw16i(ACC0_LO,hann1_table58);
    VoC_lw16i(ACC0_HI,hann1_table59);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table60);
    VoC_lw16i(REG7,hann1_table61);
    VoC_lw16i(ACC0_LO,hann1_table62);
    VoC_lw16i(ACC0_HI,hann1_table63);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table64);
    VoC_lw16i(REG7,hann1_table65);
    VoC_lw16i(ACC0_LO,hann1_table66);
    VoC_lw16i(ACC0_HI,hann1_table67);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);


    VoC_lw16i(REG6,hann1_table68);
    VoC_lw16i(REG7,hann1_table69);
    VoC_lw16i(ACC0_LO,hann1_table70);
    VoC_lw16i(ACC0_HI,hann1_table71);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);



    VoC_lw16i(REG6,hann1_table72);
    VoC_lw16i(REG7,hann1_table73);
    VoC_lw16i(ACC0_LO,hann1_table74);
    VoC_lw16i(ACC0_HI,hann1_table75);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

    VoC_lw16i(REG6,hann1_table76);
    VoC_lw16i(REG7,hann1_table77);
    VoC_lw16i(ACC0_LO,hann1_table78);
    VoC_lw16i(ACC0_HI,hann1_table79);

    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_return;
}

void CII_extra_bfi_filter(void)
{

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(REG4,0x28,IN_PARALLEL); // lower LTP delay limit
    VoC_lw16i(REG5,0x78);                   // upper LTP delay limit

    VoC_lw16i_set_inc(REG1,GLOBAL_DEC_NC_ADDR,1);
    VoC_bnez16_d(bfi_filter_exit,GLOBAL_SID_ADDR);
    VoC_loop_i_short(4,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG1,DEFAULT);
    VoC_bgt16_rr(update_bfi,REG4,REG7);
    VoC_bgt16_rr(update_bfi,REG7,REG5);
    VoC_jump(no_update_bfi);
update_bfi:
    VoC_sw16_d(REG6,GLOBAL_BFI_ADDR);
no_update_bfi:

    VoC_NOP();

    VoC_endloop0
bfi_filter_exit:
    VoC_return;

}



#ifdef voc_compile_only
// VoC_directive: FAKE_FUNCTION_ON
void CII_AMR_Encode(void) {};
void CII_AMR_Decode(void) {};
void CII_HR_Encode(void) {};
void CII_HR_Decode(void) {};

// VoC_directive: FAKE_FUNCTION_OFF
#endif

