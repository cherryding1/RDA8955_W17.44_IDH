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

/****************************
          HR GLOBAL
 ****************************/
#if 0
voc_short HR_RESERVED1,x

voc_alias  HR_GLOBAL_BEGIN  HR_RESERVED1,x

voc_short GLOBAL_DTX_swVadFrmCnt_ADDR,x
voc_short GLOBAL_DTX_siUpdPointer_ADDR,x
voc_short GLOBAL_DTX_swNElapsed_ADDR,x
voc_short GLOBAL_DTX_pL_GsHist_ADDR,56,x
voc_short STATIC_DTX_pL_R0Hist_ADDR,16,x
voc_short STATIC_DTX_ppL_CorrHist_ADDR,176,x
voc_short STATIC_DTX_swQntRefR0_ADDR,x
voc_short STATIC_DTX_swRefGsIndex_ADDR,x
voc_short STATIC_DTX_piRefVqCodewds_ADDR,4,x
voc_short STATIC_DTX_swShortBurst_ADDR,x
voc_short DECODER_resetflag_ADDR,x
voc_short STATIC_DTX_L_TxPNSeed_ADDR,2,x
voc_short STATIC_DTX_swR0OldCN_ADDR,x

voc_short HR_RESERVED2,x              //should removed

voc_short STATIC_DTX_pL_OldCorrSeq_ADDR,22,x
voc_short STATIC_DTX_pL_NewCorrSeq_ADDR,22,x
voc_short STATIC_DTX_pL_CorrSeq_ADDR,22,x
voc_short GLOBAL_ERR_plSubfrEnergyMem_ADDR,8,x
voc_short GLOBAL_ERR_swLevelMem_ADDR,4,x
voc_short GLOBAL_ERR_lastR0_ADDR,x
voc_short DECODER_resetflag_old_ADDR,x
voc_short GLOBAL_ERR_pswLastGood_ADDR,18,x
voc_short GLOBAL_ERR_swState_ADDR,x
voc_short GLOBAL_ERR_swLastFlag_ADDR,x
voc_short HR_RESERVED5,3,x             // should removed
voc_short GLOBAL_DE_giSfrmCnt_ADDR,1,x
voc_short STATIC_DE_pswSpeechPara_ADDR,22,x
voc_short STATIC_DE_pswDecodedSpeechFrame_ADDR,160,x

voc_short GLOBAL_swPreBfi_ADDR,x
voc_short GLOBAL_swOldR0_ADDR,x

voc_short STATIC_EN_pswSpeechPara_ADDR,20,x
voc_short STATIC_EN_pswSpeechBuff_ADDR,160,x

voc_short GLOBAL_DE_gswPostFiltAgcGain_ADDR,x
voc_short LOCAL_sfrmAnalysis_siPrevLagCode_ADDR,x
voc_short GLOBAL_DE_gpswPostFiltStateNum_ADDR,10,x
voc_short GLOBAL_DE_gpswPostFiltStateDenom_ADDR,10,x
voc_short GLOBAL_DE_swPostEmphasisState_ADDR,2,x
voc_short GLOBAL_DE_pswSynthFiltState_ADDR,10,x
voc_short GLOBAL_DE_pswOldFrmPFDenom_ADDR,10,x
voc_short GLOBAL_DE_pswOldFrmKsDec_ADDR,10,x
voc_short GLOBAL_DE_pswOldFrmAsDec_ADDR,10,x
voc_short GLOBAL_DE_pswOldFrmPFNum_ADDR,10,x


voc_short GLOBAL_DE_swOldR0Dec_ADDR,2,x
voc_short GLOBAL_DE_pswLtpStateBaseDec_ADDR,188,x
voc_short GLOBAL_DE_pswPPreState_ADDR,188,x
voc_short GLOBAL_DE_swMuteFlagOld_ADDR,x
voc_short GLOBAL_DTX_DE_swRxDTXState_ADDR,x
voc_short GLOBAL_DE_swDecoMode_ADDR,x
voc_short GLOBAL_DE_swDtxMuting_ADDR,x
voc_short GLOBAL_DE_swDtxBfiCnt_ADDR,x
voc_short GLOBAL_DE_swOldR0IndexDec_ADDR,x
voc_short GLOBAL_DE_swRxGsHistPtr_ADDR,2,x
voc_short GLOBAL_DE_pL_RxGsHist_ADDR,56,x
voc_short GLOBAL_DE_SP_swR0Dec_ADDR,x
voc_short GLOBAL_DE_SP_swVoicingMode_ADDR,x
voc_short GLOBAL_DE_SP_pswVq_ADDR,4,x
voc_short GLOBAL_DE_SP_swSi_ADDR,x
voc_short GLOBAL_DE_SP_swEngyRShift_ADDR,x
voc_short GLOBAL_DE_SP_swR0NewCN_ADDR,x
voc_short STATIC_DE_SP_swLastLag_ADDR,x

voc_short GLOBAL_EN_swOldR0_ADDR,x
voc_short GLOBAL_EN_swOldR0Index_ADDR,x
voc_short GLOBAL_EN_psnsWSfrmEngSpace_ADDR,16,x
voc_short GLOBAL_EN_pswHPFXState_ADDR,4,x
voc_short GLOBAL_EN_pswHPFYState_ADDR,8,x
voc_short GLOBAL_EN_pswOldFrmKs_ADDR,10,x
voc_short GLOBAL_EN_pswOldFrmAs_ADDR,10,x
voc_short GLOBAL_EN_pswOldFrmSNWCoefs_ADDR,10,x
voc_short GLOBAL_EN_pswWgtSpeechSpace_ADDR,306,x
voc_short GLOBAL_EN_pswSpeech_ADDR,196,x
voc_short GLOBAL_EN_swPtch_ADDR,x
voc_short GLOBAL_DTX_TX_swTxGsHistPtr_ADDR,x
voc_short GLOBAL_DTX_TX_pswCNVSCode1_ADDR,4,x
voc_short GLOBAL_DTX_TX_pswCNVSCode2_ADDR,4,x
voc_short GLOBAL_DTX_TX_pswCNGsp0Code_ADDR,4,x
voc_short GLOBAL_DTX_TX_pswCNLpc_ADDR,4,x
voc_short GLOBAL_DTX_TX_swCNR0_ADDR,2,x
voc_short GLOBAL_EN_SP_FRM_pswAnalysisState_ADDR,10,x
voc_short GLOBAL_EN_SP_FRM_pswWStateNum_ADDR,10,x
voc_short GLOBAL_EN_SP_FRM_pswWStateDenom_ADDR,10,x
voc_short STATIC_EN_SP_FRM_psrTable_ADDR_ADDR,x

voc_short HR_RESERVED3,x               //should removed

voc_short STATIC_EN_SP_FRM_iLow_ADDR,x
voc_short STATIC_EN_SP_FRM_iThree_ADDR,x
voc_short STATIC_EN_SP_FRM_iWordHalfPtr_ADDR,x
voc_short STATIC_EN_SP_FRM_iWordPtr_ADDR,x

voc_short HR_STATIC_VAD_pswRvad_ADDR,10,x
voc_short HR_STATIC_VAD_swNormRvad_ADDR,x
voc_short HR_STATIC_VAD_swPt_sacf_ADDR,x
voc_short HR_STATIC_VAD_swPt_sav0_ADDR,x
voc_short HR_STATIC_VAD_swE_thvad_ADDR,x
voc_short HR_STATIC_VAD_swM_thvad_ADDR,x
voc_short HR_STATIC_VAD_swAdaptCount_ADDR,x
voc_short HR_STATIC_VAD_swBurstCount_ADDR,x
voc_short HR_STATIC_VAD_swHangCount_ADDR,x
voc_short HR_STATIC_VAD_swOldLagCount_ADDR,x
voc_short HR_STATIC_VAD_swVeryOldLagCount_ADDR,x
voc_short HR_STATIC_VAD_swOldLag_ADDR,2,x
voc_short HR_STATIC_VAD_pL_sacf_ADDR,54,x
voc_short HR_STATIC_VAD_pL_sav0_ADDR,72,x
voc_short HR_STATIC_VAD_L_lastdm_ADDR,2,x
voc_short GLOBAL_SFRM_pswLtpStateBase_ADDR,188,x
voc_short GLOBAL_SFRM_pswHState_ADDR,10,x
voc_short GLOBAL_SFRM_pswHNWState_ADDR,145,x

voc_short HR_RESERVED4,x                      ////should removed



#endif

#if 0
voc_short DEC_pswSythAsSpace_ADDR,40,y
voc_short DEC_pswPFNumAsSpace_ADDR,40,y
voc_short DEC_pswPFDenomAsSpace_ADDR,40,y
voc_short DEC_L_RxPNSeed_ADDR,2,y
voc_short DEC_swRxDtxGsIndex_ADDR,2,y
#endif

#if 0
voc_short BITS_SHORT_ADDR,x
#endif

void CII_HR_Encode(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_bez16_r(Encoder_101, REG2);
    // reset
    VoC_jal(CII_resetHREnc);

Encoder_101:
    VoC_lw16i_short(REG6,0XFFF8,DEFAULT);
    VoC_lw16i_set_inc(REG1, STATIC_EN_pswSpeechBuff_ADDR, 2);

    VoC_movreg16(REG7,REG6,DEFAULT);
    VoC_lw16d_set_inc(REG0,ENC_INPUT_ADDR_ADDR,2);

    VoC_loop_i(0,80)

    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_and32_rr(RL6,REG67,DEFAULT);
    VoC_NOP();
    VoC_sw32inc_p(RL6,REG1,DEFAULT);
    VoC_endloop0

/////////////////////////////////////////////////////////////////////////
//  function in homing.c
/////////////////////////////////////////////////////////////////////////
// inline void CII_encoderHomingFrameTest(void)

    VoC_lw16i_set_inc(REG0, STATIC_EN_pswSpeechBuff_ADDR, 2);
    VoC_jal(CII_encoder_homing_frame_test);

    VoC_push16(REG4,DEFAULT);

    // encode
    VoC_jal(CII_speechEncoder);
    VoC_pop16(REG0,DEFAULT);
    // reset if homing frame
    VoC_bez16_r(Encoder_homing, REG0);
    VoC_jal(CII_resetHREnc);
Encoder_homing:

    VoC_lw16i_set_inc(REG0,TABLE_HOMING_n_ADDR,1);
    // save swVadFlag & swSP
    VoC_bez16_d(Prm2bits_MODE0,STATIC_EN_pswSpeechPara_ADDR+5);
    VoC_lw16i_set_inc(REG0,TABLE_HOMING_n1_ADDR,1);
Prm2bits_MODE0:

    VoC_lw16i_set_inc(REG1,BITS_SHORT_ADDR,1);                  // REG1:&bits_short
    VoC_lw16i_set_inc(REG2,STATIC_EN_pswSpeechPara_ADDR,1);     // REG2:&ana

    VoC_loop_i(1,18)

    VoC_lw16inc_p(REG6,REG0,DEFAULT);           // REG6: no_of_bits
    VoC_lw16inc_p(REG7,REG2,IN_PARALLEL);       // REG7: value

    VoC_loop_r_short(REG6,DEFAULT)
    VoC_startloop0
    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_and16_rr(REG5,REG7,DEFAULT);
    VoC_shr16_ri(REG7,1,DEFAULT);
    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_endloop0

    // for speed & code size optimization
    VoC_lw16i_set_inc(REG3,BITS_SHORT_ADDR+15,-1);

    VoC_endloop1

    VoC_lw16i_set_inc(REG1,OUTPUT_BITS2_ADDR,1);
    VoC_add16_rd(REG1,REG1,CONST_4_ADDR);
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(REG0,32,IN_PARALLEL);


    VoC_loop_i(1,7)
    VoC_lw32z(REG45,DEFAULT);
    VoC_loop_i_short(16,DEFAULT)

    VoC_startloop0
    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_lw16inc_p(REG6,REG3,DEFAULT);   // reg6: bits_short
    VoC_or16_rr(REG4,REG6,DEFAULT);
    VoC_endloop0

    VoC_add16_rr(REG3,REG3,REG0,DEFAULT);
    VoC_sw16inc_p(REG4,REG1,DEFAULT);

    VoC_endloop1

    VoC_return;

}





// ******************************************
// CII_avgGsHistQntz_gsQuant_opt
// input  REG0 : pL_GsHistory, INC0 = 2
//         -   : swVoicingMode always 0
// output REG2 : return
// used reg : REG01245,ACC0,ACC1,RL6,RL7
// ******************************************


void CII_avgGsHistQntz_gsQuant_opt(void)
{

    VoC_lw16i_short(ACC1_LO,1,DEFAULT);
    VoC_push16(RA,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);


    VoC_loop_i_short(28,DEFAULT);
    VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32_p(RL6,REG0,DEFAULT);
    VoC_lw32inc_p(RL7,REG0,IN_PARALLEL);
    VoC_shr32_ri(RL6,4,DEFAULT);
    VoC_shr32_ri(RL7,5,IN_PARALLEL);
    VoC_and32_rr(RL6,ACC1,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL7,IN_PARALLEL);
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_endloop0;

    /****************************/
    // CII_L_mpy_ls_opt
    // input ACC0, REG4
    // output ACC0
    // other used register REG5
    // not modified : REG4
    /****************************/

    VoC_movreg32(ACC1,ACC0,DEFAULT);
    VoC_lw16i(REG4,0x1249);
    VoC_jal(CII_L_mpy_ls_opt);

    // &ppLr_gsTable[swVoicingMode=0]
    VoC_lw16i_set_inc(REG0,TABLE_DTX_ppLr_gsTable_ADDR,2);

    VoC_add32_rr(ACC0,ACC1,ACC0,DEFAULT);

    // L_min in ACC1
    VoC_lw32_d(ACC1,CONST_0x7fffffff_ADDR);

    // swGsIndex in REG1
    VoC_lw16i_set_inc(REG1,0,1);
    VoC_pop16(RA,DEFAULT);
    VoC_loop_i_short(32,DEFAULT);

    VoC_startloop0;
    // L_diff in RL6
    VoC_sub32inc_rp(RL6,ACC0,REG0,DEFAULT);
    VoC_bnltz32_r(CII_gsQuant_opt_abs_pos,RL6)
    // L_diff = - L_diff
    VoC_sub32_dr(RL6,CONST_0_ADDR,RL6);
CII_gsQuant_opt_abs_pos:
    // if (L_sub(L_diff, L_min) < 0)
    VoC_bngt32_rr(CII_gsQuant_opt_no_new_min,ACC1,RL6);
    // new minimum
    VoC_movreg16(REG2,REG1,DEFAULT);
    VoC_movreg32(ACC1,RL6,IN_PARALLEL);
CII_gsQuant_opt_no_new_min:
    VoC_inc_p(REG1,DEFAULT);
    VoC_endloop0;
    VoC_return;
}




// ***********************************
// CII_getPnBits_opt
// input :   ACC0 -> L_RxPNSeed
//                   iBits always 7
// output :  REG2 -> return
//           ACC0 -> L_RxPNSeed
// used register : REG2, ACC0, RL6, RL7
// ************************************


void CII_getPnBits_opt(void)
{


    // for (i = 0; i < iBits = 7; i++)
    VoC_loop_i_short(7,DEFAULT);
    // swPnBits = 0;
    VoC_lw16i_short(REG2,0,IN_PARALLEL);
    VoC_startloop0

    VoC_movreg32(RL6,ACC0,DEFAULT);

    // *pL_PNSeed = L_shr(*pL_PNSeed, 1)
    VoC_shr32_ri(ACC0,1,IN_PARALLEL);

    VoC_and32_rd(RL6,CONST_0x00000005L_ADDR);
    VoC_shr32_ri(RL6,1,DEFAULT);
    VoC_movreg32(RL7,RL6,IN_PARALLEL);
GTPNB_opt_while_begin:
    // while (L_Taps) ...
    VoC_bez32_r(GTPNB_opt_while_end,RL6)
    // L_FeedBack = L_FeedBack ^ L_Taps;
    VoC_xor32_rr(RL7,RL6,DEFAULT);
    // L_Taps = L_shr(L_Taps, 1);
    VoC_shr32_ri(RL6,1,IN_PARALLEL);
    VoC_jump(GTPNB_opt_while_begin);
GTPNB_opt_while_end:

    // swPnBits = shl(swPnBits, 1);
    VoC_shr16_ri(REG2,-1,DEFAULT);

    // if (L_FeedBack & 1)
    VoC_and32_rd(RL7,CONST_1_ADDR);
    VoC_bez32_r(GTPNB_opt_FeedBack,RL7)
    // *pL_PNSeed = *pL_PNSeed | PN_XOR_ADD
    VoC_or32_rd(ACC0,CONST_0x40000000L_ADDR);
GTPNB_opt_FeedBack:

    // swPnBits = swPnBits | (extract_l(*pL_PNSeed) & 0x0001);
    VoC_movreg16(REG4,ACC0_LO,DEFAULT);
    VoC_and16_rd(REG4,CONST_1_ADDR);
    VoC_or16_rr(REG2,REG4,DEFAULT);
    VoC_endloop0

    VoC_return;
}




void CII_lpcZsFir_opt(void)
{

    VoC_lw16inc_p(REG4,REG0,DEFAULT);

    // REG7 for sismp
    VoC_lw16i_short(REG7,1,DEFAULT);

    // REG6 pswInput[-1]
    VoC_sub16_rr(REG6,REG0,REG7,DEFAULT);
    VoC_sw16inc_p(REG4,REG2,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG6,REG7,DEFAULT);

    VoC_loop_i(1,39)
    VoC_lw16i_short(REG4,10,DEFAULT);       // for (siStage = 0; ((0 < (siSmp - siStage)) && siStage < NP); siStage++)
    VoC_lw32_d(ACC0,CONST_0x00000800L_ADDR);//    L_Sum = LPC_ROUND
    VoC_bgt16_rd(CII_lpcZsFir_100,REG7,CONST_10_ADDR);
    VoC_movreg16(REG4,REG7,DEFAULT);
CII_lpcZsFir_100:
    // REG3 pswInput[siSmp-1]
    VoC_add16_rr(REG3,REG7,REG6,DEFAULT);

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0
    // reg1 pswCoef[siStage]
    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_mac32inc_rp(REG5,REG3,DEFAULT); // L_Sum = L_msu(L_Sum, pswFiltOut[siSmp - siStage - 1],pswCoef[siStage]);
    VoC_endloop0
    /* scale output */
    /* ------------ */
    VoC_sub16_rr(REG1,REG1,REG4,DEFAULT);

    VoC_multf32inc_pd(RL7,REG0,CONST_0x8000_ADDR);  //L_Sum = L_msu(L_Sum, pswInput[siSmp], 0x8000);
    VoC_shr32_ri(ACC0,-4,DEFAULT);  // L_Sum = L_shl(L_Sum, ASHIFT);
    VoC_sub32_rr(ACC0,ACC0,RL7,DEFAULT);
    /* save current output sample */
    /* -------------------------- */
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);
    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);    // pswFiltOut[siSmp] = extract_h(L_Sum);
    VoC_endloop1;
    VoC_return;
}



void  CII_speechEncoder(void)
{


#if 0
    voc_short LOCAL_EN_SPEECHENCODER_pswFrmAs_ADDR,10,y
    voc_short LOCAL_EN_SPEECHENCODER_ppswSynthAs_ADDR,40,y
#endif

#if 0
    voc_short LOCAL_EN_SPEECHENCODER_swR0_ADDR,x

    voc_alias  MIC_FILT_ADDR    LOCAL_EN_SPEECHENCODER_swR0_ADDR ,x
    voc_alias  LOCAL_ECHO_CANCEL_SAMPLS_ADDR    MIC_FILT_ADDR   ,x




    voc_short LOCAL_EN_SPEECHENCODER_iR0_ADDR,x
    voc_short LOCAL_EN_SPEECHENCODER_swVadFlag_ADDR,x
    voc_short LOCAL_EN_SPEECHENCODER_swSP_ADDR,x
    voc_short LOCAL_EN_SPEECHENCODER_pswFrmKs_ADDR,10,x
    voc_short LOCAL_EN_SPEECHENCODER_pswVadLags_ADDR,4,x
    voc_short LOCAL_EN_SPEECHENCODER_pswLagList_ADDR,12,x
    voc_short LOCAL_EN_SPEECHENCODER_piVq_ADDR,3,x
    voc_short LOCAL_EN_SPEECHENCODER_siUVCode_ADDR,x
    voc_short LOCAL_EN_SPEECHENCODER_piVSCode1_ADDR,4,x
    voc_short LOCAL_EN_SPEECHENCODER_piVSCode2_ADDR,4,x
    voc_short LOCAL_EN_SPEECHENCODER_piGsp0Code_ADDR,4,x
    voc_short LOCAL_EN_SPEECHENCODER_piLagCode_ADDR,4,x
    voc_short LOCAL_EN_SPEECHENCODER_pswNumLagList_ADDR,4,x
    voc_short LOCAL_EN_SPEECHENCODER_pswPitchBuf_ADDR,4,x
    voc_short LOCAL_EN_SPEECHENCODER_pswHNWCoefBuf_ADDR,4,x
    voc_short LOCAL_EN_SPEECHENCODER_pswFrmSNWCoefs_ADDR,10,x
    voc_short LOCAL_EN_SPEECHENCODER_ppswSNWCoefAs_ADDR     ,40,x
    voc_short LOCAL_EN_SPEECHENCODER_iSi_ADDR             ,x
    voc_short LOCAL_EN_SPEECHENCODER_iVoicing_ADDR          ,x
    voc_short LOCAL_EN_SPEECHENCODER_psnsSqrtRs_ADDR         ,8,x     // [4]long
    voc_short LOCAL_EN_SPEECHENCODER_pswLagListPtr_ADDR_ADDR ,2,x

#endif






    VoC_push16(RA, DEFAULT);

////////////////////////////////////////////////////////////
//INLINE FUNC void CII_filt4_2nd(void)
///////////////////////////////////////////////////////////

    VoC_lw16i_set_inc(REG0, TABLE_SP_ROM_psrHPFCoefs_ADDR, 1);
    VoC_lw16i_set_inc(REG1, STATIC_EN_pswSpeechBuff_ADDR, 1);  //REG1 for pswIn
    VoC_lw16i_set_inc(REG2, GLOBAL_EN_pswHPFXState_ADDR, 2);
    VoC_lw16i_set_inc(REG3, GLOBAL_EN_pswHPFYState_ADDR, 1);

    VoC_push16(REG1,DEFAULT);

//              STACK16
//------------------------------------------------
//   &pswSpeechIn(STATIC_EN_pswSpeechBuff_ADDR)

    VoC_lw16i_short(REG4, 1, DEFAULT);    //swPreFirDownSh
    VoC_lw16i_short(REG5, 0, IN_PARALLEL);//swFinalUpShift

    VoC_jal(CII_iir_d_opt);
    VoC_lw16_sd(REG1, 0, DEFAULT);  //REG1 for pswIn
    VoC_lw16i_set_inc(REG0, TABLE_SP_ROM_psrHPFCoefs_ADDR+5, 1);


    VoC_lw16i_short(REG4, 0, DEFAULT);//swPreFirDownSh
    VoC_lw16i_short(REG5, -1, IN_PARALLEL);//-swFinalUpShift
    VoC_jal(CII_iir_d_opt);

////////////////////////////////////////////////////////////
//end INLINE FUNC void CII_filt4_2nd(void)
///////////////////////////////////////////////////////////

    VoC_pop16(REG1,DEFAULT);
    VoC_lw16i_set_inc(REG0,GLOBAL_EN_pswSpeech_ADDR+35, 1);


    VoC_lw16inc_p(REG2, REG1, DEFAULT);
    VoC_loop_i(0, 160)
    VoC_lw16inc_p(REG2, REG1, DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG2, REG0, DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_lw16i_set_inc(REG1, LOCAL_AFLAT_pswFlatsRc_ADDR, 1);
    VoC_lw16i_set_inc(REG0, GLOBAL_EN_pswSpeech_ADDR+25, 1);  // pswIn in REG0

    VoC_jal(CII_aflat_opt);


    VoC_lw16_d(REG1, LOCAL_EN_SPEECHENCODER_iR0_ADDR);
    VoC_lw16i(REG0, TABLE_SP_ROM_psrR0DecTbl_ADDR);
    VoC_add16_rr(REG1,REG1, REG1,DEFAULT);
    VoC_add16_rr(REG0,REG1, REG0,DEFAULT);
    VoC_lw16i_set_inc(REG1,LOCAL_EN_SPEECHENCODER_pswFrmAs_ADDR,-2);
    VoC_lw16_p(REG0, REG0,DEFAULT);
    VoC_lw16i_set_inc(REG0,LOCAL_EN_SPEECHENCODER_pswFrmKs_ADDR,1);
    exit_on_warnings=OFF;
    VoC_sw16_d(REG0, LOCAL_EN_SPEECHENCODER_swR0_ADDR);
    exit_on_warnings=ON;


    VoC_jal(CII_rcToADp_opt_new);

    VoC_lw16i_short(REG4,0,DEFAULT);

    VoC_bez16_r(EN_CODE_100, REG7)

    VoC_lw16i_set_inc(REG0, GLOBAL_EN_pswOldFrmSNWCoefs_ADDR, 1);
    VoC_lw16i_set_inc(REG1, LOCAL_EN_SPEECHENCODER_pswFrmSNWCoefs_ADDR, 1);
    VoC_loop_i_short(10, DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG4, REG0, DEFAULT);
    VoC_lw16i_set_inc(REG2, GLOBAL_EN_pswOldFrmAs_ADDR, 1);
    VoC_sw16inc_p(REG4, REG1, DEFAULT);
    VoC_endloop0


    VoC_lw16i_set_inc(REG0, GLOBAL_EN_pswOldFrmKs_ADDR, 1);
    VoC_lw16i_set_inc(REG1, LOCAL_EN_SPEECHENCODER_pswFrmKs_ADDR, 1);
    VoC_lw16i_set_inc(REG3, LOCAL_EN_SPEECHENCODER_pswFrmAs_ADDR, 1);

    VoC_loop_i_short(10, DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG4, REG0, DEFAULT);
    VoC_lw16inc_p(REG5, REG2, DEFAULT);
    VoC_sw16inc_p(REG4, REG1, DEFAULT);
    VoC_sw16inc_p(REG5, REG3, DEFAULT);
    VoC_endloop0

    VoC_jump(EN_CODE_100_end)


    /******************************************************/
// inline function: void   CII_getNWCoefs_new(void)

// IN:
// REG0 &pswACoef  INCR0=1
//                 INCR2=1
    /*******************************************************/
EN_CODE_100:;


    VoC_lw16i_short(INC2,1,DEFAULT);
    {
#if 0

        voc_short    LOCAL_EN_SPEECHENCODER_pswCoefTmp2_ADDR        ,10,x   //[10]shorts
        voc_short    LOCAL_EN_SPEECHENCODER_pswCoefTmp3_ADDR        ,10,x   //[10]shorts
        voc_short    LOCAL_EN_SPEECHENCODER_pswVecTmp_ADDR          ,40,x   //[40]shorts
        voc_short    LOCAL_EN_SPEECHENCODER_pswVecTmp2_ADDR         ,40,x   //[40]shorts
        voc_short    LOCAL_EN_SPEECHENCODER_L_Temp_ADDR             ,2,x     //[2]shorts
        voc_short    LOCAL_EN_SPEECHENCODER_pL_AutoCorTmp_ADDR      ,22,x    //[22]shorts
        voc_short    LOCAL_EN_SPEECHENCODER_pswTempRc_ADDR          ,10,x    //[10]shorts
        voc_short    LOCAL_EN_SPEECHENCODER_pL_A_ADDR               ,20,x    //[20]shorts
        voc_short    LOCAL_EN_SPEECHENCODER_pL_tmp_ADDR             ,20,x    //[20]shorts

#endif
    }


    VoC_lw16i_set_inc(REG0,LOCAL_EN_SPEECHENCODER_pswFrmAs_ADDR,1);
    VoC_lw16i_set_inc(REG1,TABLE_SP_ROM_psrNWCoefs_ADDR,1);
    VoC_lw32_d(RL6,CONST_0x00008000L_ADDR);

    VoC_lw16i_set_inc(REG3,LOCAL_EN_SPEECHENCODER_pswCoefTmp2_ADDR,1);

    VoC_lw16i_short(REG5,10,DEFAULT);


    VoC_push16(REG3,DEFAULT);
    // pswCoefTmp3 is in contigent with pswCoefTmp2
    // so after the first inner loop, REG3 is automatically pointed to pswCoefTmp3
    VoC_loop_i(1,2);
    VoC_loop_i_short(10,DEFAULT);
    VoC_startloop0
    VoC_multf32inc_pp(REG67,REG0,REG1,DEFAULT);
    VoC_be16_rd(getNWCoefs_LABEL1,REG4,CONST_0x400_ADDR);
    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);
    VoC_jump(getNWCoefs_LABEL2);
getNWCoefs_LABEL1:
    VoC_sub32_rr(REG67,RL6,REG67,DEFAULT);
    VoC_lw16i_set_inc(REG2,LOCAL_EN_SPEECHENCODER_pswVecTmp2_ADDR,1);
getNWCoefs_LABEL2:
    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    VoC_endloop0;

    VoC_sub16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_lw16i(REG4,0x400);

    VoC_endloop1;


    /*******************************************************/

//    INLINE FUNC: LpcIrZsIir(pswACoefs, pswVecTmp2);

//    IN:
//        REG0: pswACoefs   INCR0=1
//        REG2: pswVecTmp2  INCR2=1

//    pswACoefs & pswVecTemp2 should in different RAM

    /*******************************************************/



    // REG1 for siSmp
    VoC_lw16i_short(REG1,1,DEFAULT);
    // for pswFiltOut[siSmp - siStage - 2] first address
    VoC_sub16_rr(REG5,REG2,REG1,DEFAULT);

    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);
    VoC_loop_i(1,39)

    // for (siStage = 0; ((0 < (siSmp - siStage)) && siStage < NP); siStage++)
    VoC_lw16i_short(REG4,10,DEFAULT);
    // L_Sum = LPC_ROUND
    VoC_lw32_d(ACC0,CONST_0x00000800L_ADDR);

    VoC_bgt16_rd(CII_lpcIrZsIir_100,REG1,CONST_10_ADDR);
    VoC_movreg16(REG4,REG1,DEFAULT);
CII_lpcIrZsIir_100:
    VoC_add16_rr(REG3,REG5,REG1,DEFAULT);
    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0
    VoC_msu32inc_pp(REG0,REG3,DEFAULT); // L_Sum = L_msu(L_Sum, pswFiltOut[siSmp - siStage - 1],pswCoef[siStage]);
    VoC_endloop0
    /* scale output */
    /* ------------ */
    VoC_sub16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_shr32_ri(ACC0,-4,DEFAULT);  // L_Sum = L_shl(L_Sum, ASHIFT);

    /* save current output sample */
    /* -------------------------- */
    VoC_inc_p(REG1,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);    // pswFiltOut[siSmp] = extract_h(L_Sum);


    VoC_endloop1
// end of CII_lpcIrZsIir


    VoC_lw16i_set_inc(REG0,LOCAL_EN_SPEECHENCODER_pswVecTmp2_ADDR,1);       // pswInput address
    VoC_lw16i_set_inc(REG2,LOCAL_EN_SPEECHENCODER_pswVecTmp_ADDR,1);

    // pswInput address
    VoC_pop16(REG1,DEFAULT);

//         STACK16
//---------------------------
//           RA
//      pswVecTmp_ADDR
    VoC_push16(REG2,DEFAULT);
    /*******************************************************/
//  lpcZsFir(pswVecTmp2, pswCoefTmp2, pswVecTmp);

//    IN:
//      REG0  pswInput       INCR0=1
//      REG1  pswCoef        INCR1=1
//      REG2  pswFiltOut     INCR2=1
//                           INCR3=-1
//
//    pswCoef, pswInput should in different RAM

    /********************************************************/

    VoC_jal(CII_lpcZsFir_opt);

//  lpcZsIirP(pswVecTmp, pswCoefTmp3);

//  here lpcZsIirP is replaced by lpcZsFir
//  because we find they are the same format if we map pswCommonIO
//  to pswInput & pswFiltOut in lpcZsFir

    VoC_lw16_sd(REG0,0,DEFAULT);        // pswCommonIO
    VoC_lw16i(REG1,LOCAL_EN_SPEECHENCODER_pswCoefTmp3_ADDR);    // pswCoef
    VoC_movreg16(REG2,REG0,DEFAULT);
    VoC_jal(CII_lpcZsFir_opt);


// end of CII_lpcZsIirP
    /****************************************************************/
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_jal(CII_g_corr1_opt_paris);

    VoC_lw16i_short(INC1,1,DEFAULT);
    // REG1 for swNormShift
    VoC_lw16i_set_inc(REG0,LOCAL_EN_SPEECHENCODER_pL_AutoCorTmp_ADDR,2);
    VoC_sub16_dr(REG7,CONST_0_ADDR,REG1);           // REG1 for -swNormShift

    VoC_movreg16(RL6_LO,REG0,DEFAULT);

    VoC_lw16i_short(REG3,40,IN_PARALLEL);//REG2 for inner loop number
    VoC_lw16i(REG6,LOCAL_EN_SPEECHENCODER_pswVecTmp_ADDR+40);

    VoC_loop_i(1,11)

    VoC_sub16_rr(REG1,REG6,REG3,DEFAULT);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16_sd(REG2,0,IN_PARALLEL);



    VoC_loop_r_short(REG3,DEFAULT);
    VoC_startloop0;
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_mac32inc_rp(REG4,REG1,DEFAULT);
    VoC_endloop0;
    VoC_inc_p(REG3,DEFAULT);
    VoC_shr32_rr(ACC0,REG7,DEFAULT);
    // load RL6_HI here to replace a NOP
    VoC_lw16_sd(RL6_HI,0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop1;
    /*************************************************************************

      Function:  void   CII_aFlatRcDp( void)
      Input : RL6_LO      Longword  &pL_R
              RL6_HI      Shortword &pswRc

      Output:  NONE
      Return:s

    *************************************************************************/

    //use pswVecTemp as pswTempRc
    VoC_jal(CII_aFlatRcDp_opt);
    /*************************************************/
// CII_rcToADp_opt_new
// IN:
//    REG1:   pswA          INCR1=-2
//    REG0:   &pswRc        INCR0=1
//    0x800   swAscale(is a const)
//OUT:
//    REG7
//USE:
//    ALL REGs
    /************************************************/
    //use pswVecTemp as pswTempRc
    VoC_pop16(REG0,DEFAULT);
    VoC_lw16i_set_inc(REG1,LOCAL_EN_SPEECHENCODER_pswFrmSNWCoefs_ADDR,-2);


    VoC_jal(CII_rcToADp_opt_new);


    /******************************************************/
// end inline function: void   CII_getNWCoefs_new(void)

///////////////////////////////////////////////////////





EN_CODE_100_end:

    /* Interpolate, or otherwise get sfrm reflection coefs */
    /* --------------------------------------------------- */
    VoC_lw16i_short(REG5,1,DEFAULT);

    VoC_lw16i_set_inc(REG0,LOCAL_EN_SPEECHENCODER_pswFrmAs_ADDR, 1);
    VoC_sw16_d(REG5,FLAG_GETSFRMLPC_TX_ADDR);
    VoC_lw16i_set_inc(REG2,LOCAL_EN_SPEECHENCODER_ppswSynthAs_ADDR, 1);
    VoC_lw16i_set_inc(REG3, GLOBAL_EN_pswOldFrmAs_ADDR, 1);
    VoC_lw16i(REG4, GLOBAL_EN_pswOldFrmKs_ADDR);
    VoC_lw16i(RL6_LO,LOCAL_EN_SPEECHENCODER_pswFrmSNWCoefs_ADDR);
    VoC_lw16i(RL6_HI,LOCAL_EN_SPEECHENCODER_ppswSNWCoefAs_ADDR);
    VoC_lw16i_set_inc(REG1,LOCAL_EN_SPEECHENCODER_psnsSqrtRs_ADDR,1);
    VoC_lw16i(REG6, LOCAL_EN_SPEECHENCODER_pswFrmKs_ADDR);
    VoC_lw16i(RL7_HI,TABLE_SP_ROM_psrOldCont_ADDR);
    VoC_lw16i(RL7_LO,TABLE_SP_ROM_psrNewCont_ADDR);
    VoC_lw16i(ACC0_LO,GLOBAL_EN_pswOldFrmSNWCoefs_ADDR);
    VoC_lw16_d(REG5, GLOBAL_EN_swOldR0_ADDR);

    VoC_movreg32(ACC1,RL6,DEFAULT);
    VoC_movreg16(ACC0_HI,ACC0_LO,IN_PARALLEL);

    VoC_lw16_d(REG7, LOCAL_EN_SPEECHENCODER_swR0_ADDR);
    VoC_jal(CII_getSfrmLpcTx_opt);

    VoC_lw16i_short(REG5,0,DEFAULT);


    /* Weight the entire speech frame */
    /* ------------------------------ */
////////////////////////////////////////////////////////////
//  INLINE FUNC void   weightSpeechFrame()
////////////////////////////////////////////////////////////
    {
#if 0
        voc_short weightSpeechFrame_pswScratch0_ADDR ,305,x
#endif
    }

    //  this func is not a critical on speed (which will only be done once each frame)
    //  so optimise on code size not the speed
    VoC_lw16i_set_inc(REG1,GLOBAL_EN_pswWgtSpeechSpace_ADDR+160,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_EN_pswWgtSpeechSpace_ADDR,1);

    VoC_lw16inc_p(ACC0_HI,REG1,DEFAULT);

    VoC_loop_i(0, 145);
    VoC_lw16inc_p(ACC0_HI,REG1,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop0;

    // in the out loop, it will only be done 4 times each frame
    // so try to reduce size instead of speed
WEIGHTSPEECHFRAME_LOOP_START:
    VoC_push16(REG5, DEFAULT);

    VoC_lw16i_set_inc(REG0,GLOBAL_EN_pswSpeech_ADDR, -1);
    VoC_lw16i_set_inc(REG3,LOCAL_EN_SPEECHENCODER_ppswSynthAs_ADDR, 1);
    VoC_lw16i_set_inc(REG1,weightSpeechFrame_pswScratch0_ADDR, 1);
    VoC_lw16i_set_inc(REG2,GLOBAL_EN_SP_FRM_pswWStateNum_ADDR,1);
    VoC_lw16i_short(REG6, 10, DEFAULT);
    VoC_lw16i(REG7, 40);

    VoC_mult16_rr(REG7,REG5,REG7,DEFAULT);
    VoC_mult16_rr(REG6,REG5,REG6,DEFAULT);

    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);
    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);

    VoC_push32(REG67,DEFAULT);
    VoC_jal(CII_lpcFir_opt);
    VoC_pop32(REG67,DEFAULT);
    VoC_lw16i_set_inc(REG0,weightSpeechFrame_pswScratch0_ADDR,1);
    VoC_lw16i_set_inc(REG3,LOCAL_EN_SPEECHENCODER_ppswSNWCoefAs_ADDR,1);
    VoC_lw16i_set_inc(REG1,GLOBAL_EN_pswWgtSpeechSpace_ADDR+145, -1);
    VoC_lw16i_set_inc(REG2,GLOBAL_EN_SP_FRM_pswWStateDenom_ADDR,1);


    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);
    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);

    VoC_jal(CII_lpcIir_opt_paris);
    VoC_pop16(REG5, DEFAULT);
    VoC_add16_rd(REG5, REG5, CONST_1_ADDR);

    VoC_be16_rd(WEIGHTSPEECHFRAME_go, REG5, CONST_4_ADDR);
    VoC_jump(WEIGHTSPEECHFRAME_LOOP_START);
WEIGHTSPEECHFRAME_go:  ;
////////////////////////////////////////////////////////////
//  end INLINE FUNC void   CII_weightSpeechFrame()
////////////////////////////////////////////////////////////


//}

    /* Perform open-loop lag search, get harmonic-noise-weighting parameters */
    /* --------------------------------------------------------------------- */


    // REG INPUT:
    // ---------
    // REG0 : swPrevR0Index
    // REG1 : swCurrR0Index
    // REG2 : pswLagList
    // REG3 : pswNumLagList
    // REG4 : pswPitchBuf
    // REG5 : pswHNWCoefBuf
    // REG6 : pswVadLags
    // REG7 : swSP
    //
    // GLOBAL INPUT:
    // ------------
    // GLOBAL_EN_pswWgtSpeechSpace_ADDR : &pswWSpeech[-145]
    // GLOBAL_EN_psnsWSfrmEngSpace_ADDR : &psnsWSfrmEng[-4]
    //
    // OUTPUT:
    // ------
    // REG0 : *psiUVCode


    VoC_lw16_d(REG0,GLOBAL_EN_swOldR0Index_ADDR);


    VoC_lw16_d(REG1,LOCAL_EN_SPEECHENCODER_iR0_ADDR);
    VoC_lw16i(REG2,LOCAL_EN_SPEECHENCODER_pswLagList_ADDR);
    VoC_lw16i(REG3,LOCAL_EN_SPEECHENCODER_pswNumLagList_ADDR);
    VoC_lw16i(REG4,LOCAL_EN_SPEECHENCODER_pswPitchBuf_ADDR);
    VoC_lw16i(REG5,LOCAL_EN_SPEECHENCODER_pswHNWCoefBuf_ADDR);
    VoC_lw16i(REG6,LOCAL_EN_SPEECHENCODER_pswVadLags_ADDR);
    VoC_lw16_d(REG7,GLOBAL_OUTPUT_SP_ADDR);


    VoC_jal(CII_openLoopLagSearch_new);

    //swLagCount in REG7
    VoC_lw16i_short(REG7, 0,DEFAULT);

    VoC_sw16_d(REG0, LOCAL_EN_SPEECHENCODER_siUVCode_ADDR);

    /******************************************************************************/

    VoC_sw16_d(REG0, LOCAL_EN_SPEECHENCODER_iVoicing_ADDR);

    //load parameters for periodicity_update
    VoC_lw16i_set_inc(REG0, LOCAL_EN_SPEECHENCODER_pswVadLags_ADDR, 1);
    /***************************************************
      Function: void CII_periodicity_update(void)

    IN:
         REG0 &pswLags INCR0=1
    OUT:
         REG4 *pswPtch

    ***************************************************/
    VoC_lw16_d(REG6, HR_STATIC_VAD_swOldLag_ADDR);


    VoC_loop_i(1, 4)
    VoC_lw16_p(REG5, REG0, DEFAULT);
    //swMinLag in REG6 , swMaxLag in REG5
    VoC_bngt16_rr(PERIODICITYUPDATE101, REG6,REG5);//else
    VoC_movreg16(REG6, REG5, DEFAULT);
    VoC_movreg16(REG5, REG6, IN_PARALLEL);
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
    VoC_blt16_rd(PERIODICITYUPDATE103, REG3,CONST_2_ADDR);
    VoC_bnlt16_rd(PERIODICITYUPDATE104, REG5, CONST_2_ADDR);
PERIODICITYUPDATE103:
    VoC_add16_rr(REG7, REG7, REG4, DEFAULT);
PERIODICITYUPDATE104:

    VoC_lw16inc_p(REG6, REG0, DEFAULT);
    VoC_endloop1

    VoC_lw16_d(REG5, HR_STATIC_VAD_swOldLagCount_ADDR);

    VoC_sw16_d(REG6, HR_STATIC_VAD_swOldLag_ADDR);
//   VoC_sw16_d(REG5, HR_STATIC_VAD_swVeryOldLagCount_ADDR);

    VoC_add16_rr(REG6, REG5, REG7, DEFAULT);

    VoC_bnlt16_rd(PERIODICITYUPDATE105, REG6, CONST_7_ADDR);
    VoC_lw16i_short(REG4, 0,DEFAULT);
PERIODICITYUPDATE105:

    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_sw16_d(REG7, HR_STATIC_VAD_swOldLagCount_ADDR);

    /***************************************************
    end Function: void CII_periodicity_update(void)

    ***************************************************/

    //save parameters of periodicity_update
    VoC_sw16_d(REG4, GLOBAL_EN_swPtch_ADDR);
    VoC_lw16i(REG0, GLOBAL_DTX_TX_pswCNVSCode1_ADDR);
    // put directly piVsCode1 instead of pswVsCode1
    // no need to move it lator
    VoC_lw16i(REG3, LOCAL_EN_SPEECHENCODER_piVSCode1_ADDR);

    // here require piVSCode1,piVsCode2,piGsp0Code to stock continuously
    // also requires pswCNVSCode1,pswCNVSCode2,pswCNGsp0Code to stock continuously
    // so we could treat them together in a loop
    VoC_loop_i_short(12,DEFAULT);
    VoC_startloop0;
    VoC_lw16inc_p(REG1,REG0,DEFAULT);
    VoC_lw16i_short(INC1,-1,DEFAULT);
    VoC_sw16inc_p(REG1,REG3,DEFAULT);
    VoC_endloop0;

    VoC_lw16i(REG6, LOCAL_EN_SPEECHENCODER_pswLagList_ADDR);
    // REG7 for giSfrmCnt
    VoC_lw16i_short(REG7, 0, DEFAULT);


EN_LOOP_START:
// sfrmAnalysis loop start

    VoC_sw16_d(REG6, LOCAL_EN_SPEECHENCODER_pswLagListPtr_ADDR_ADDR);
    // put directly piVsCode1 instead of pswVsCode1
    // no need to move it lator

    VoC_lw16i_set_inc(REG2,LOCAL_EN_SPEECHENCODER_piVSCode1_ADDR,4);

    // REG2 for &pswVSCode1[giSfrmCnt]
    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);

    VoC_lw16i(REG3,sfrmAnalysis_psiVSCode1_ADDR_ADDR);

    // here also requires piVSCode1,piVSCode2,piGsp0Code,siNumLags,swPitch,swHNWCoef to stock continuously
    // &piVSCode1,&piVSCode2,&piGsp0Code,&piLagCode to stock continuously
    // so we could treat them together
    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0;

    // save &pswVSCode1[giSfrmCnt]
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG2, REG3,DEFAULT);
    exit_on_warnings=ON;
    // REG2 for &pswVSCode1[giSfrmCnt]
    VoC_inc_p(REG2, DEFAULT);

    VoC_endloop0;

    //output



    // REG0 = 10*giSfrmCnt
    VoC_mult16_rd(REG0, REG7, CONST_10_ADDR);
    VoC_lw16i(REG3, LOCAL_EN_SPEECHENCODER_ppswSNWCoefAs_ADDR);
    // REG3 for  pswHCoefs
    VoC_add16_rr(REG3, REG3, REG0, DEFAULT);
    // REG0 = 40*giSfrmCnt
    VoC_shr16_ri(REG0, -2,IN_PARALLEL);

    VoC_lw16i(REG1, GLOBAL_EN_pswWgtSpeechSpace_ADDR+145);
    // REG1 for pswWSpeech
    VoC_add16_rr(REG0, REG1, REG0, DEFAULT);





////////////////////////////////////////////////////////////////////
// INPUT:
// REG0 : &pswWSpeech,  INC0 = 1
// REG1 : siNumLags,    INC0 = -1
// REG2 : &pswLagList,  INC2 = 1
// REG3 : &pswHCoefs,   INC3 = 1
// REG4 : swPitch
// REG5 : &snsSqrtRs
// REG6 : swHNWCoef
// REG7 : swSP
/////////////////////////////////////////////////////////////////////
    // REG1 for siNumLags
    VoC_lw16inc_p(REG1, REG2, DEFAULT);
    // REG4 for swPitch
    VoC_lw16inc_p(REG4, REG2, DEFAULT);
    // REG6 for swHNWCoef
    VoC_lw16inc_p(REG6, REG2, DEFAULT);
    // REG2 for pswLagList
    VoC_movreg16(REG2, REG6, IN_PARALLEL);

    VoC_lw16i_short(INC2,1,DEFAULT);
    // push pswNumLagList[giSfrmCnt]
    VoC_push16(REG1,IN_PARALLEL);

    VoC_lw16i(REG5, LOCAL_EN_SPEECHENCODER_psnsSqrtRs_ADDR);

    VoC_add16_rr(REG5, REG5, REG7, DEFAULT);
    VoC_add16_rr(REG5, REG5, REG7, DEFAULT);

    // push giSfrmCnt
    VoC_push16(REG7, IN_PARALLEL);

//         STACK16
//--------------------------------------------------------------------------------------------------
//        giSfrmCnt  must be in the top of stack (will be used in sfrmAnalyse as VoC_lw16_sd(25); )

    VoC_lw16_d(REG7, GLOBAL_OUTPUT_SP_ADDR);//end


//        if(giFrmCnt==51)
//          {
//              fprintf(stderr,"\n%d\n",voc_counter.VoC_cycle);
//              exit(-1);
//          }

    VoC_jal(CII_sfrmAnalysis_opt);

    // pop giSfrmCnt
    VoC_pop16(REG7, DEFAULT);

    // pop pswNumLagList[giSfrmCnt]
    VoC_pop16(REG2,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);

    // new pswLagListPtr
    VoC_add16_rd(REG6, REG2, LOCAL_EN_SPEECHENCODER_pswLagListPtr_ADDR_ADDR);
    // new giSfrmCnt
    VoC_add16_rr(REG7, REG7, REG3, DEFAULT);

    VoC_be16_rd(EN_LOOP_END,REG7,CONST_4_ADDR);
    VoC_jump(EN_LOOP_START);
EN_LOOP_END:

//  sfrmAnalysis loop ended
    VoC_lw16_d(REG4, LOCAL_EN_SPEECHENCODER_siUVCode_ADDR);

    VoC_bnez16_d(EN_CODE_200, GLOBAL_OUTPUT_SP_ADDR)//if
    VoC_lw16_d(REG0, GLOBAL_DTX_TX_swCNR0_ADDR);
    VoC_lw16i_set_inc(REG1, GLOBAL_DTX_TX_pswCNLpc_ADDR, 1);
    VoC_sw16_d(REG0, LOCAL_EN_SPEECHENCODER_iR0_ADDR);
    VoC_lw16i_set_inc(REG2, LOCAL_EN_SPEECHENCODER_piVq_ADDR, 1);
    VoC_loop_i_short(3,DEFAULT)
    VoC_lw16inc_p(REG3, REG1, IN_PARALLEL);
    VoC_startloop0;
    VoC_lw16inc_p(REG3, REG1, DEFAULT);
    //       VoC_NOP();
    VoC_sw16inc_p(REG3, REG2, DEFAULT);
    VoC_endloop0

    VoC_jump(EN_CODE_200_END)
EN_CODE_200://else
    VoC_shr16_ri(REG4, -6,DEFAULT);
    VoC_lw16i(REG2, GLOBAL_DTX_pL_GsHist_ADDR);
    VoC_lw16i_set_inc(REG1, LOCAL_EN_SPEECHENCODER_piGsp0Code_ADDR,1);
    VoC_lw16d_set_inc(REG0, GLOBAL_DTX_TX_swTxGsHistPtr_ADDR, 1);

    VoC_lw16i(REG3, TABLE_DTX_ppLr_gsTable_ADDR);

    VoC_add16_rr(REG3, REG3, REG4, DEFAULT);
    //VoC_loop_i(0, 4)

    VoC_loop_i_short(4,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG6, REG1, DEFAULT);
    VoC_shr16_ri(REG6, -1, DEFAULT);
    VoC_add16_rr(REG5, REG3, REG6, DEFAULT);
    VoC_mult16_rd(REG6, REG0, CONST_2_ADDR);
    VoC_lw32_p(ACC0, REG5, DEFAULT);
    VoC_add16_rr(REG7,REG2,REG6, DEFAULT);

    VoC_inc_p(REG0, DEFAULT);
    VoC_bngt16_rd(EN_LOOP1_100, REG0, CONST_27_ADDR);
    VoC_lw16i_short(REG0,0,DEFAULT);
EN_LOOP1_100:
    VoC_sw32_p(ACC0, REG7, DEFAULT);
    VoC_endloop0
    VoC_sw16_d(REG0, GLOBAL_DTX_TX_swTxGsHistPtr_ADDR);

EN_CODE_200_END:

    /* End of frame processing, update frame based parameters */
    /* ------------------------------------------------------ */




    VoC_lw32_d(REG45, LOCAL_EN_SPEECHENCODER_swR0_ADDR);

    VoC_lw16i(REG0,LOCAL_EN_SPEECHENCODER_pswFrmKs_ADDR);
    VoC_sw32_d(REG45, GLOBAL_EN_swOldR0_ADDR);

    VoC_lw16i(REG1,GLOBAL_EN_pswOldFrmKs_ADDR);
    VoC_lw16i(REG2,LOCAL_EN_SPEECHENCODER_pswFrmAs_ADDR);
    VoC_lw16i(REG3,GLOBAL_EN_pswOldFrmAs_ADDR);
    VoC_loop_i_short(10, DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG4, REG0, DEFAULT);
    VoC_lw16inc_p(REG5, REG2, DEFAULT);
    VoC_sw16inc_p(REG4, REG1, DEFAULT);
    VoC_sw16inc_p(REG5, REG3, DEFAULT);
    VoC_endloop0


    VoC_lw16i(REG0,LOCAL_EN_SPEECHENCODER_pswFrmSNWCoefs_ADDR);
    VoC_lw16i(REG1,GLOBAL_EN_pswOldFrmSNWCoefs_ADDR);

    VoC_lw16inc_p(REG4,REG0,DEFAULT);

    VoC_loop_i_short(10,DEFAULT);

    VoC_startloop0
    VoC_lw16inc_p(REG4, REG0, DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4, REG1, DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_bnez16_d(EN_CODE_300, GLOBAL_OUTPUT_SP_ADDR)

    VoC_lw16i_short(REG7, 3, DEFAULT);
    VoC_lw16i_short(REG6, 1, DEFAULT);
    VoC_sw16_d(REG7, LOCAL_EN_SPEECHENCODER_iVoicing_ADDR);
    VoC_sw16_d(REG6, LOCAL_EN_SPEECHENCODER_iSi_ADDR);

    VoC_lw16i(REG7,0x01ff);
    VoC_lw16i_short(REG6,0x001f,DEFAULT);

    VoC_lw16i_set_inc(REG0,LOCAL_EN_SPEECHENCODER_piVSCode1_ADDR, 1);
    VoC_lw16i_set_inc(REG1,LOCAL_EN_SPEECHENCODER_piGsp0Code_ADDR, 1);
    VoC_loop_i_short(4, DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG7, REG0, DEFAULT);
    VoC_sw16inc_p(REG6, REG1, DEFAULT);
    VoC_endloop0

    VoC_lw16i(REG0,LOCAL_EN_SPEECHENCODER_piLagCode_ADDR);
    //     VoC_lw16i(REG7,0x00ff);
    //    VoC_lw16i_short(REG6,0x000f,DEFAULT);


    VoC_shr16_ri(REG7,1,DEFAULT); // VoC_lw16i(REG7,0x00ff);
    VoC_shr16_ri(REG6,1,DEFAULT); // VoC_lw16i_short(REG6,0x000f,DEFAULT);


    VoC_sw16inc_p(REG7, REG0, DEFAULT);
    VoC_loop_i_short(3, DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG6, REG0, DEFAULT);
    VoC_endloop0
EN_CODE_300:
/////////////////////////////////////////////////////////////////
//  INLINE FUNC void   CII_fillBitAlloc(void)
////////////////////////////////////////////////////////////////
    VoC_lw16i_set_inc(REG0, STATIC_EN_pswSpeechPara_ADDR, 1);
    VoC_lw16_d(REG6, LOCAL_EN_SPEECHENCODER_iR0_ADDR);
    VoC_lw16i_set_inc(REG1, LOCAL_EN_SPEECHENCODER_piVq_ADDR, 1);
    VoC_loop_i_short(3,DEFAULT)
    VoC_startloop0;
    VoC_lw16inc_p(REG6,REG1, DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG0, DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    // REG2 for isi & REG3 for iVoicing
    VoC_lw32_d(REG45, LOCAL_EN_SPEECHENCODER_iSi_ADDR);


    VoC_lw16i_set_inc(REG1, LOCAL_EN_SPEECHENCODER_piVSCode1_ADDR, 1);
    VoC_lw16i_set_inc(REG2, LOCAL_EN_SPEECHENCODER_piVSCode2_ADDR, 1);
    VoC_lw16i_set_inc(REG3, LOCAL_EN_SPEECHENCODER_piGsp0Code_ADDR, 1);

    VoC_bngtz16_d(CII_fillBitAlloc_100, LOCAL_EN_SPEECHENCODER_iVoicing_ADDR)

    VoC_movreg16(REG2,REG1,DEFAULT);
    VoC_lw16i(REG1, LOCAL_EN_SPEECHENCODER_piLagCode_ADDR);
CII_fillBitAlloc_100:

    VoC_loop_i_short(5,DEFAULT)
    VoC_startloop0;
    VoC_sw16inc_p(REG6,REG0, DEFAULT);
    VoC_sw16inc_p(REG4,REG0, DEFAULT);
    VoC_sw16inc_p(REG5,REG0, DEFAULT);
    VoC_lw16inc_p(REG6,REG1, DEFAULT);
    VoC_lw16inc_p(REG4,REG2, DEFAULT);
    VoC_lw16inc_p(REG5,REG3, DEFAULT);
    VoC_endloop0

    //REG2 for swVadFlag, REG3 for swSP
//       VoC_lw32_d(REG23, GLOBAL_OUTPUT_Vad_ADDR);
    VoC_pop16(RA, DEFAULT);
//     VoC_sw32_p(REG23,REG0, DEFAULT);     // VAD & SP needn't translate in parameters, they are in registers. Stephen 060710
//////////////////////////////////////////////////////////
// end INLINE FUNC void   CII_fillBitAlloc(void)
/////////////////////////////////////////////////////////
    /* delay the input speech by 1 frame */
    /*-----------------------------------*/

    VoC_lw16i_set_inc(REG0,GLOBAL_EN_pswSpeech_ADDR,1);
    VoC_add16_rd(REG1, REG0, CONST_160_ADDR);
    VoC_loop_i_short(35, DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG4, REG1, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG4, REG0, DEFAULT);
    VoC_endloop0
    VoC_return;
}




void CII_aflat_opt(void)
{

#if 0

    voc_short   LOCAL_AFLAT_pL_VadAcf_ADDR                  ,18,x        //[18]shorts
    voc_short   LOCAL_AFLAT_pswFlatsRc_ADDR                 ,10,x        //[10]shorts
    voc_short   LOCAL_AFLAT_pL_CorrelSeq_ADDR               ,22,x        //[22]shorts
    voc_short   LOCAL_AFLAT_pswVadRc_ADDR                   ,6,x         //[6]shorts
    voc_short   LOCAL_AFLAT_swVadScalAuto_ADDR              ,10,x        //[10]shorts

#endif


    VoC_push16(RA,DEFAULT);
    /***************************************************************
      CII_flat_opt

      INPUT :
        REG0 : pswSpeechIn  (param0), INC0 = 1
        REG1 : pswRc        (param1), INC1 = 1

        pL_VadAcf (param3) not passed, directly used
        in cov32 : LOCAL_AFLAT_pL_VadAcf_ADDR

      OUTPUT :
        ACC0 : return
        REG6 : *piR0Inx
        REG7 : *pswVadScalAuto

     **************************************************************/

    VoC_push16(REG1,DEFAULT);

    VoC_jal(CII_flat_opt);

    VoC_lw16_sd(REG4,0,DEFAULT);
    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_lw16i_set_inc(REG1, LOCAL_AFLAT_pswVadRc_ADDR,2);


    VoC_loop_i_short(2,DEFAULT);
    VoC_push32(ACC0, IN_PARALLEL);
    VoC_startloop0;
    VoC_lw32inc_p(RL6, REG0, DEFAULT);
//      VoC_NOP();
    VoC_sw16_d(REG7, LOCAL_AFLAT_swVadScalAuto_ADDR);       // Stephen 060711
    VoC_sw32inc_p(RL6, REG1, DEFAULT);
    VoC_endloop0;

    VoC_sw16_d(REG6,LOCAL_EN_SPEECHENCODER_iR0_ADDR);
    VoC_lw16i(REG1,LOCAL_AFLAT_pL_CorrelSeq_ADDR);


// ********************************************
// CII_rcToCorrDpL_opt
//
//
// note :  1st two parameters are not passed, because
//         they are always the same constants :
//         swAshift = 4, swAscale = 0x800
//
// input:  REG4 : pswRc (param2), INC0 = 2
//         REG1 : pL_R  (param3), INC1 = 2
//
// output: none
//
// ********************************************

    VoC_jal(CII_rcToCorrDpL_opt);


    VoC_jal(CII_vad_algorithm);
    VoC_movreg16(REG4,REG0,DEFAULT);

    VoC_bnez16_d(AFLAT100,GLOBAL_DTX_ADDR);
    VoC_lw16i_short(REG4,1,DEFAULT);
AFLAT100:


// *******************************
// CII_swComfortNoise_opt
// input :  REG0 -> pL_UnqntzdCorr, INC0 = 2
//          REG4 -> swVadFlag
//          ACC0 -> L_UnqntzdR0
// output : REG7 -> return
// used regs : all
// *******************************


    VoC_pop32(ACC0,DEFAULT);
    VoC_sw16_d(REG4, GLOBAL_OUTPUT_Vad_ADDR);
    VoC_lw16i_set_inc(REG0,LOCAL_AFLAT_pL_CorrelSeq_ADDR,2);


// *******************************
// INLINE CII_swComfortNoise_opt
// input :  REG0 -> pL_UnqntzdCorr, INC0 = 2
//          REG4 -> swVadFlag
//          ACC0 -> L_UnqntzdR0
// output : REG7 -> return
// used regs : all
// *******************************


    {

#if 0

        voc_short   LOCAL_SWCOMFORTNOISE_pswFinalRc_ADDR        ,10,x       //[10]shorts
        voc_short   LOCAL_SWCOMFORTNOISE_pL_RefCorr_ADDR        ,22,x       //[22]shorts

#endif




        // swSP in REG7
        VoC_lw16i_short(REG7,1,DEFAULT);
        // swVadFrmCnt in REG6
        VoC_lw16i_short(REG6,0,IN_PARALLEL);

        VoC_bnez16_r(SWCOMFORTNOISE_opt_100,REG4);
        VoC_lw16_d(REG6,GLOBAL_DTX_swVadFrmCnt_ADDR);
        VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
SWCOMFORTNOISE_opt_100:

        // swNElapsed = add(swNElapsed, 1);
        VoC_add16_rd(REG5,REG7,GLOBAL_DTX_swNElapsed_ADDR);

        VoC_bez16_r(SWCOMFORTNOISE_opt_RETURN1,REG6);

        // ACC0 : L_UnqntzdR0
        // REG0 : pL_UnqntzdCorr
        // REG5 : swNElapsed
        // REG6 : swVadFrmCnt
        // REG7 : swSP = 1

        // Update history, with this frames data
        // updateCNHist inline added

        // swShortBurst is calculated after

        VoC_lw16_d(REG4,GLOBAL_DTX_siUpdPointer_ADDR);
        VoC_mult16_rd(REG3,REG4,CONST_11_ADDR);

        VoC_lw16i_set_inc(REG2,STATIC_DTX_ppL_CorrHist_ADDR,2);
        // &ppL_CorrHistory[siUpdPointer][0]
        VoC_add16_rr(REG2,REG2,REG3,DEFAULT);

        VoC_lw16i_set_inc(REG3,STATIC_DTX_pL_R0Hist_ADDR,2);

        // &pL_R0History[siUpdPointer]
        VoC_add16_rr(REG3,REG3,REG4,DEFAULT);



        VoC_loop_i_short(11,DEFAULT);
        VoC_lw32inc_p(RL7,REG0,IN_PARALLEL);
        VoC_startloop0;
        // ppL_CorrHistory[siUpdPointer][i] = pL_UnqntzdCorr[i];
        VoC_lw32inc_p(RL7,REG0,DEFAULT);
        exit_on_warnings=OFF;
        VoC_sw32inc_p(RL7,REG2,DEFAULT);
        exit_on_warnings=ON;
        VoC_endloop0;

        // siUpdPointer ++
        VoC_add16_rr(REG4,REG4,REG7,DEFAULT);
        VoC_add16_rr(REG4,REG4,REG7,DEFAULT);

        // siUpdPointer % OVERHANG
        VoC_bne16_rd(updateCNHist_no_modulo,REG4,CONST_16_ADDR);
        VoC_lw16i_short(REG4,0,DEFAULT);
updateCNHist_no_modulo:

        // pL_R0History[siUpdPointer] = L_UnqntzdR0
        VoC_sw32_p(ACC0,REG3,DEFAULT);

        // store siUpdPointer
        VoC_sw16_d(REG4, GLOBAL_DTX_siUpdPointer_ADDR);

        // END of inline updateCNHist


        // REG5 : swNElapsed
        // REG6 : swVadFrmCnt
        // REG7 : swSP = 1

        VoC_lw16_d(REG4, STATIC_DTX_swShortBurst_ADDR);
        VoC_bne16_rd(SWCOMFORTNOISE_opt_101,REG6,CONST_1_ADDR);
        VoC_lw16i_short(REG4,0,DEFAULT);
        VoC_bnlt16_rd(SWCOMFORTNOISE_opt_101,REG5,CONST_24_ADDR);
        VoC_lw16i_short(REG4,1,DEFAULT);

SWCOMFORTNOISE_opt_101:


        // REG4 : swShortBurst
        // REG5 : swNElapsed
        // REG6 : swVadFrmCnt
        // REG7 : swSP = 1

        // init value for random generator (PN_INIT_SEED)
        //      VoC_lw32_d(RL7,CONST_0x1091988L_ADDR);
        VoC_lw16i(RL7_HI,0x109);
        VoC_lw16i(RL7_LO,0x1988);


        // -----------------------
        // if first_SID_frame_true
        // -----------------------

        VoC_bnez16_r(SWCOMFORTNOISE_opt_Lbrst,REG4);
        // (swShortBurst == 0)
        VoC_bne16_rd(SWCOMFORTNOISE_opt_no_1st_SID,REG6,CONST_8_ADDR);
        // (swVadFrmCnt == OVERHANG)
        VoC_jump(SWCOMFORTNOISE_opt_1st_SID);
SWCOMFORTNOISE_opt_Lbrst:
        // (swShortBurst == 1)
        VoC_bne16_rd(SWCOMFORTNOISE_opt_no_1st_SID,REG6,CONST_1_ADDR);
        // (swVadFrmCnt == 1)
SWCOMFORTNOISE_opt_1st_SID:


        // L_TxPNSeed = PN_INIT_SEED
        VoC_sw32_d(RL7, STATIC_DTX_L_TxPNSeed_ADDR);

        // ******************************************
        // CII_avgGsHistQntz_gsQuant_opt
        // input  REG0 : pL_GsHistory, INC0 = 2
        //         -   : swVoicingMode always 0
        // output REG2 : return
        // used reg : REG01245,ACC0,ACC1,RL6,RL7
        // ******************************************

        VoC_push32(REG45,DEFAULT);
        VoC_lw16i_set_inc(REG0, GLOBAL_DTX_pL_GsHist_ADDR,2);
        VoC_jal(CII_avgGsHistQntz_gsQuant_opt);
        VoC_pop32(REG45,DEFAULT);
        VoC_sw16_d(REG2,STATIC_DTX_swRefGsIndex_ADDR);


SWCOMFORTNOISE_opt_no_1st_SID:

        // REG4 : swShortBurst
        // REG5 : swNElapsed
        // REG6 : swVadFrmCnt
        // REG7 : swSP = 1


        VoC_bnlt16_rd(SWCOMFORTNOISE_opt_106,REG6,CONST_8_ADDR);
        // (swVadFrmCnt < OVERHANG)
        VoC_bez16_r(SWCOMFORTNOISE_opt_RETURN,REG4);
        // (swShortBurst == 1)

        // swSP = 0
        // VoC_lw16i_short(REG7,0,DEFAULT);

        //  L_RefR0 in REG67, so push vars
        VoC_push16(REG6,DEFAULT);
        VoC_push32(REG45,IN_PARALLEL);

        // jump_lg
        VoC_jump(SWCOMFORTNOISE_opt_swSP_0);

SWCOMFORTNOISE_opt_106:

        // (swVadFrmCnt >= OVERHANG)

        // swNElapsed = 0;
        VoC_lw16i_short(REG5,0,DEFAULT);

        // swSP = 0
        // VoC_lw16i_short(REG7,0,DEFAULT);

        //  L_RefR0 in REG67, so push vars
        VoC_push16(REG6,DEFAULT);
        VoC_push32(REG45,DEFAULT);
        VoC_lw32z(REG67,IN_PARALLEL);
        // average R0 and correlation values
        // avgCNHist inline
        VoC_lw16i_set_inc(REG0,LOCAL_SWCOMFORTNOISE_pL_RefCorr_ADDR, 2);
        VoC_lw16i_set_inc(REG2,STATIC_DTX_ppL_CorrHist_ADDR,22);
        VoC_lw16i_set_inc(REG3,STATIC_DTX_pL_R0Hist_ADDR,2);

        // L_RefR0 in REG67
        VoC_loop_i_short(8, DEFAULT);

        VoC_startloop0;
        VoC_lw32inc_p(RL6,REG3,DEFAULT);
        VoC_shr32_ri(RL6,3,DEFAULT);
        VoC_add32_rr(REG67,REG67,RL6,DEFAULT);
        VoC_endloop0;

        // 1 in REG4
        VoC_lw16i_short(REG4,1,DEFAULT);

        // 174 in REG3
        // VoC_lw16i(REG3,174);

        // change for code rearrangement optimization
        // STATIC_DTX_ppL_CorrHist_ADDR in REG3
        VoC_movreg16(REG3,REG2,IN_PARALLEL);

        VoC_loop_i(1,11)

        VoC_loop_i_short(8,DEFAULT);
        VoC_lw32z(RL6,IN_PARALLEL);
        VoC_startloop0;
        VoC_lw32_p(ACC0,REG2,DEFAULT);
        VoC_lw32inc_p(ACC1,REG2,IN_PARALLEL);
        VoC_shr32_ri(ACC0,2,DEFAULT);
        VoC_shr32_ri(ACC1,3,IN_PARALLEL);
        VoC_and32_rr(ACC0,REG45,DEFAULT);
        VoC_add32_rr(RL6,RL6,ACC1,IN_PARALLEL);
        VoC_add32_rr(RL6,RL6,ACC0,DEFAULT);
        VoC_endloop0;

        // STATIC_DTX_ppL_CorrHist_ADDR +=2
        VoC_inc_p(REG3,DEFAULT);
        VoC_movreg16(REG2,REG3,IN_PARALLEL);

        // VoC_sub16_rr(REG2,REG2,REG3,DEFAULT);
        VoC_sw32inc_p(RL6,REG0,DEFAULT);

        VoC_endloop1;


        // END avgCNHist inline

        // --------------------
        // CII_r0Quant_opt
        // input in REG67
        // output in REG6
        // used register 0167, inc0 = 2
        // --------------------


        // L_RefR0 in REG67
        VoC_jal(CII_r0Quant_opt);

        // swQntRefR0
        VoC_sw16_d(REG6,STATIC_DTX_swQntRefR0_ADDR);


        // ********************************
        // CII_lpcCorrQntz_opt
        // Input:
        //      REG0 -> pL_CorrelSeq, INC0 = 2
        //      REG4 -> pswFinalRc
        //      REG5 -> piVQCodewds
        //  Output : none
        //  Used regs : all
        // ********************************


        VoC_lw16i_set_inc(REG0,LOCAL_SWCOMFORTNOISE_pL_RefCorr_ADDR, 2);
        VoC_lw16i(REG4,LOCAL_SWCOMFORTNOISE_pswFinalRc_ADDR);
        VoC_lw16i(REG5,STATIC_DTX_piRefVqCodewds_ADDR);

        VoC_jal(CII_lpcCorrQntz_opt);

SWCOMFORTNOISE_opt_swSP_0:
// ********************************
// CII_swComfortNoiseSaveParams_opt
// input : none
// output : none
// used regs : REG013
// ********************************

//      begin of CII_swComfortNoiseSaveParams_opt
        VoC_lw16_d(REG3,STATIC_DTX_swQntRefR0_ADDR);
        VoC_lw16i_set_inc(REG0,STATIC_DTX_piRefVqCodewds_ADDR,1);
        VoC_lw16i_set_inc(REG1,GLOBAL_DTX_TX_pswCNGsp0Code_ADDR,1);
        VoC_sw16_d(REG3,GLOBAL_DTX_TX_swCNR0_ADDR);
        VoC_lw16_d(REG3,STATIC_DTX_swRefGsIndex_ADDR);

        // for (i = 0; i < N_SUB; i++) pswCNGsp0Code[i] = swRefGsIndex;
        VoC_loop_i_short(4,DEFAULT);
        VoC_startloop0
        VoC_sw16inc_p(REG3,REG1,DEFAULT);
        VoC_endloop0;

        // for (i = 0; i < 3;     i++) pswCNLpc[i] = piRefVqCodewds[i];
        VoC_loop_i_short(3,DEFAULT);
        VoC_lw16inc_p(REG3,REG0,IN_PARALLEL);
        VoC_startloop0
        VoC_lw16inc_p(REG3,REG0,DEFAULT);
        exit_on_warnings=OFF;
        VoC_sw16inc_p(REG3,REG1,DEFAULT);
        exit_on_warnings=ON;
        VoC_endloop0;
// end of CII_swComfortNoiseSaveParams_opt

        // unpile stack ...
        VoC_pop16(REG6,DEFAULT);
        VoC_pop32(REG45,DEFAULT);

        // swSP = 0
        VoC_lw16i_short(REG7,0,DEFAULT);

        // (swSP == 0)
        VoC_lw16i_set_inc(REG0,GLOBAL_DTX_TX_pswCNVSCode1_ADDR,1);
        VoC_lw16i_set_inc(REG1,GLOBAL_DTX_TX_pswCNVSCode2_ADDR,1);
        VoC_lw32_d(ACC0,STATIC_DTX_L_TxPNSeed_ADDR);

        // ***********************************
        // CII_getPnBits_opt
        // input :   ACC0 -> L_RxPNSeed
        //                   iBits always 7
        // output :  REG2 -> return
        //           ACC0 -> L_RxPNSeed
        // used register : REG2, ACC0, RL6, RL7
        // ************************************

        VoC_push32(REG45,DEFAULT);

        VoC_loop_i(1,4)
        VoC_jal(CII_getPnBits_opt);
        VoC_sw16inc_p(REG2,REG0,DEFAULT);
        VoC_jal(CII_getPnBits_opt);
        VoC_sw16inc_p(REG2,REG1,DEFAULT);
        VoC_endloop1

        VoC_pop32(REG45,DEFAULT);

        // store back L_TxPNSeed
        VoC_sw32_d(ACC0,STATIC_DTX_L_TxPNSeed_ADDR);

SWCOMFORTNOISE_opt_RETURN:


        VoC_lw16i_short(INC0,2,DEFAULT);
        // store back global vars
        VoC_sw16_d(REG4,STATIC_DTX_swShortBurst_ADDR);

SWCOMFORTNOISE_opt_RETURN1:

        // INC0 = 2 if coming from branch
        VoC_sw16_d(REG5,GLOBAL_DTX_swNElapsed_ADDR);
        VoC_sw16_d(REG6,GLOBAL_DTX_swVadFrmCnt_ADDR);

    }
// *******************************
// END INLINE CII_swComfortNoise_opt

// *******************************

    VoC_pop16(REG0, DEFAULT);
    VoC_sw16_d(REG7, GLOBAL_OUTPUT_SP_ADDR);
    // save swSP


    VoC_lw16i_set_inc(REG1, LOCAL_EN_SPEECHENCODER_pswFrmKs_ADDR, 2);

    VoC_bnez16_r(AFLAT101, REG7);//if
    VoC_lw32inc_p(RL7, REG0, DEFAULT);
    VoC_loop_i_short(5, DEFAULT);

    VoC_startloop0
    VoC_lw32inc_p(RL7, REG0, DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(RL7, REG1, DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_jump(AFLATRETURNN);
AFLAT101: // else
    VoC_movreg16(REG4, REG1,DEFAULT);
    VoC_lw16i(REG0, LOCAL_AFLAT_pL_CorrelSeq_ADDR);
    VoC_lw16i(REG5, LOCAL_EN_SPEECHENCODER_piVq_ADDR);

    VoC_jal(CII_lpcCorrQntz_opt);

AFLATRETURNN:

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    /****************************************************
    end Inline Function    void CII_aflat_opt(void);
    *****************************************************/
    VoC_return;
}




// *****************************************************
// CII_aflatRecursionLoop_opt
//
// Description :
//
//  setupPreQ_Quant(iSeg, quantList.iRCIndex);
//
//  for (iCnt = 0; iCnt < quantList.iNum; iCnt++)
//  {
//      getNextVec(pswRc);
//      iLimit = 0;
//
//      quantList.pswPredErr[iCnt] =
//              aflatRecursion(&pswRc[psvqIndex[iSeg - 1].l],
//                             pswPBar, pswVBar,
//                             ppswPAddrs, ppswVAddrs,
//                             psvqIndex[iSeg - 1].len);
//
//      if (iLimit)
//        quantList.pswPredErr[iCnt] = 0x7fff;
//  }
//
//
// Input :
//      REG1 : bestPql[iVec].iRCIndex_or_0
//      REG2 : iSeg, INC2 = 2
//      REG5 : &psrPreQ_QuantSz[-1]
//      REG6 : TABLE_SP_ROM_psrPreQ_Quant1
//      RL7  : TABLE_SP_ROM_psrPreQ_Quant23

// STACK32 : (+2) pswPBar & pswVBar
//           (+1) ppswPAddrs[0] & ppswVAddrs[0]
//           (+0) ppswPAddrs[1] & ppswVAddrs[1]
//
// Used regs : all
//
// ******************************************************



void CII_aflatRecursionLoop_opt(void)
{


    // STACK16                           STACK32
    // --------------------------------------------------
    //                                   pswPBar & pswVBar
    //                                   ppswPAddrs[0] & ppswVAddrs[0]
    //                                   ppswPAddrs[1] & ppswVAddrs[1]


    // get the following vars, depending on iSeg:
    // ------------------------------------------
    // psrPreQSz[iSeg - 1] or psrQuantSz[iSeg - 1]
    // &pswRc[psvqIndex[iSeg - 1].l]
    // psvqIndex[iSeg - 1].len


    // &psrPreQ_QuantSz[iSeg-1]
    VoC_add16_rr(REG5,REG5,REG2,DEFAULT);

    // quantList
    VoC_lw16i_set_inc(REG0,LPCCORRQNTZ_quantList_ADDR,1);

    // quantList.iRCIndex = bestPql[iVec].iRCIndex_or_0 * psrPreQ_QuantSz[iSeg-1]
    VoC_mult16_rp(REG3,REG1,REG5,DEFAULT);

    // quantList.iNum
    VoC_lw16_p(REG1,REG5,IN_PARALLEL);

    // push RA
    VoC_push16(RA,DEFAULT);

    // store struct values
    // quantList.pswPredErr in REG0
    VoC_sw16inc_p(REG1,REG0,DEFAULT);
    VoC_sw16inc_p(REG3,REG0,DEFAULT);

    // qntLst.pswPrdErr & qntLst.iNum
    VoC_push32(REG01,DEFAULT);


    // STACK16                           STACK32
    // --------------------------------------------------
    // RA                                pswPBar & pswVBar
    //                                   ppswPAddrs[0] & ppswVAddrs[0]
    //                                   ppswPAddrs[1] & ppswVAddrs[1]
    //                                   qntLst.pswPrdErr & qntLst.iNum


    // ***************************************
    // Function: CII_setupPreQ
    // Input:  REG2->iSeg
    //         REG3->iVector
    //         REG6<-psrPreQ1;
    //         RL7_LO<-psrPreQ2;
    //         RL7_HI<-psrPreQ3;
    //  unused : RL6, ACC0, ACC1
    //  unchanged : REG2
    // ***************************************


    // quantList.iRCIndex in REG3
    // iSeg in REG2
    VoC_jal(CII_setupPreQ);

    // iSeg in REG2
    // iSeq * 4
    VoC_shr16_ri(REG2,-2,DEFAULT);
    VoC_lw16i(REG3,TABLE_SP_ROM_psvqIndex_ADDR_MOIN_4_ADDR);
    // &psvqIndex[iSeg - 1].l
    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);

    // pswRc
    VoC_lw16i_set_inc(REG3,LPCCORRQNTZ_pswRc_ADDR,1);

    // &pswRc[psvqIndex[iSeg - 1].l]
    VoC_add16inc_rp(REG0,REG3,REG2,DEFAULT);

    // psvqIndex[iSeg - 1].len
    VoC_lw16_p(REG1,REG2,DEFAULT);


    // REG0 : &pswRc[psvqIndex[iSeg - 1].l] : pswQntRc
    // REG1 : psvqIndex[iSeg - 1].len       : swSegmentOrder
    // REG3 : pswRc


    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG1,DEFAULT);


    // STACK16                           STACK32
    // --------------------------------------------------
    // RA                                pswPBar & pswVBar
    // pswQntRc                          ppswPAddrs[0] & ppswVAddrs[0]
    // swSegmentOrder                    ppswPAddrs[1] & ppswVAddrs[1]
    // pswRc                             qntLst.pswPrdErr & iCnt = qntLst.iNum



    VoC_lw16i_short(INC2,1,IN_PARALLEL);


    // for (iCnt = quantList.iNum; iCnt > 0; iCnt--)

CII_aflatRecurLoop_begin:


    // INC0 = 1
    // INC2 = 1
    // INC3 = 1

    VoC_push16(REG3,DEFAULT);


    // ****************************
    // input : REG3 : pswRC, INC3 = 1
    // used : all 16-bit
    // ****************************


    VoC_jal(CII_getNextVec_opt);



    // ****************************
    // inline aflatRecursion
    //
    // params correspondence :
    //    &pswRc[psvqIndex[iSeg - 1].l]  : pswQntRc
    //    pswPBar                        : pswPBar
    //    pswVBar                        : pswVBar
    //    ppswPAddrs                     : ppswPAddrs
    //    ppswVAddrs                     : ppswVAddrs
    //    psvqIndex[iSeg - 1].len        : swSegmentOrder
    //
    // remarks :
    //    all params will be described in
    //    the inline function with their
    //    local names
    // ****************************



    // STACK16                           STACK32
    // --------------------------------------------------
    // RA                                pswPBar & pswVBar
    // pswQntRc                          ppswPAddrs[0] & ppswVAddrs[0]
    // swSegmentOrder                    ppswPAddrs[1] & ppswVAddrs[1]
    // pswRc                             qntLst.pswPrdErr & iCnt


    {

#if 0
        voc_short    LOCAL_AFLATRECURSION_pswQntRcSqd_ADDR           ,4,x         // [4]
#endif

        // pswQntRc in REG0, swSegmentOrder in REG4
        VoC_lw16_sd(REG0,2,DEFAULT);
        // init New, iLimit = 0, prepare swap
        VoC_lw32_sd(ACC1,2,DEFAULT);

        // 0x8000 for round, pswQntRc in REG1
        VoC_lw32_d(ACC0,CONST_0x00008000L_ADDR);

        VoC_multf32inc_pp(REG67,REG0,REG0,DEFAULT);
        VoC_lw16i_short(REG5,0,IN_PARALLEL);

        VoC_lw16i_set_inc(REG1,LOCAL_AFLATRECURSION_pswQntRcSqd_ADDR,1);
        VoC_lw16_sd(REG4,1,DEFAULT);
        // calculate pswQuntRc[j]
        // mult_r, 0x8000 in ACC0
        VoC_add32_rr(REG67,REG67,ACC0,IN_PARALLEL);

        // for (j = 0; j < swSegmentOrder; j++)
        VoC_loop_r_short(REG4,DEFAULT);
        VoC_startloop0
        // REG67 updated after VoC_sw16
        VoC_multf32inc_pp(REG67,REG0,REG0,DEFAULT);
        exit_on_warnings=OFF;
        // save pswQuntRc[j]
        VoC_sw16inc_p(REG7,REG1,DEFAULT);
        exit_on_warnings=ON;
        // mult_r, 0x8000 in ACC0
        VoC_add32_rr(REG67,REG67,ACC0,DEFAULT);
        VoC_endloop0


        VoC_lw32_sd(REG23,1,DEFAULT);
        // pswQntRcSqd in REG1
        VoC_sub16_rr(REG1,REG1,REG4,IN_PARALLEL);

        // create swap space on the stack, get pointers, ilimit=0 on the stack
        VoC_lw16_sd(REG0,2,DEFAULT);
        VoC_push32(ACC1,IN_PARALLEL);

        VoC_push16(REG5,DEFAULT);
        VoC_push32(REG23,IN_PARALLEL);



        // pswQntRc[0] & pswQntRcSqd[0]
        VoC_lw16inc_p(REG6,REG0,DEFAULT);
        VoC_lw16i_short(INC3,-1,IN_PARALLEL);
        VoC_lw16inc_p(REG7,REG1,DEFAULT);
        // push 0x8000
        VoC_push32(ACC0,IN_PARALLEL);


        VoC_lw16i_short(REG5,1,DEFAULT);
        // push pswQntRc & pswQntRcSqd
        VoC_push32(REG01,IN_PARALLEL);


        // STACK16                           STACK32
        // --------------------------------------------------
        // RA                                pswPBar & pswVBar
        // pswQntRc                          ppswPAddrs[0] & ppswVAddrs[0]
        // swSegmentOrder                    ppswPAddrs[1] & ppswVAddrs[1]
        // pswRc                             qntLst.pswPrdErr & iCnt
        // iLimit = 0                        ppswPVAddrs[j%2] (Old)
        //                                   ppswPVAddrs[(j+1)%2] (New)
        //                                   0x8000

        // INC0 = 1
        // INC1 = 1
        // INC2 = 1
        // INC3 = 1

        // init Old (with pswPBar & pswVBar)
        VoC_lw32_sd(REG01,7,DEFAULT);
        // swSegmentOrder - 1 in REG4, bound+1 in REG4
        VoC_sub16_rr(REG4,REG4,REG5,IN_PARALLEL);

        // STACK16                           STACK32
        // --------------------------------------------------
        // RA                                pswPBar & pswVBar
        // pswQntRc                          ppswPAddrs[0] & ppswVAddrs[0]
        // swSegmentOrder                    ppswPAddrs[1] & ppswVAddrs[1]
        // pswRc                             qntLst.pswPrdErr & iCnt
        // iLimit = 0                        ppswPVAddrs[j%2] (Old)
        //                                   ppswPVAddrs[(j+1)%2] (New)
        //                                   0x8000
        //                                   pswQntRc[1] & pswQntRcSqd[1]


        // REG0 : pswPOld, INC0 =  1
        // REG1 : pswVOld, INC1 =  1
        // REG2 : pswPNew, INC2 =  1
        // REG3 : pswVNew, INC3 = -1
        // REG4 : swSegmentOrder - 1
        // REG5 : 1
        // REG6 : pswQntRc[0]
        // REG7 : pswQntRcSqd[0]

// critical loop
        VoC_loop_r(1,REG4)


        // STACK16                 STACK32
        // ---------------------------------------------
        // RA                      pswPBar & pswVBar
        // pswQntRc                ppswPAddrs[0] & ppswVAddrs[0]
        // swSegmentOrder          ppswPAddrs[1] & ppswVAddrs[1]
        // pswRc                   qntLst.pswPrdErr & iCnt
        // iLimit                  ppswPVAddrs[j%2] (Old)
        //                         ppswPVAddrs[(j+1)%2] (New)
        //                         0x8000
        //                         pswQntRc[j+1] & pswQntRcSqd[j+1]

        // REG0 : pswPOld, INC0 =  1
        // REG1 : pswVOld, INC1 =  1
        // REG2 : pswPNew, INC2 =  1
        // REG3 : pswVNew, INC3 = -1
        // REG4 : bound+1
        // REG5 : 1
        // REG6 : pswQntRc[j]
        // REG7 : pswQntRcSqd[j]



        // OPTIMIZED FOR SPEED !!!!

        // put bound in REG4
        VoC_sub16_rr(REG4,REG4,REG5,DEFAULT);
        // pswVOld in REG3
        VoC_movreg16(REG3,REG1,DEFAULT);


        //  L_mac(L_ROUND, pswVOld[0], pswQntRc[j])
        VoC_mac32inc_rp(REG6,REG1,DEFAULT);
        //  ACC0 <- L_ROUND before accumulation
        VoC_lw32_sd(ACC0,1,IN_PARALLEL);
        //  L_mac(L_sum, pswVOld[0], pswQntRc[j]);
        VoC_mac32inc_rp(REG6,REG3,DEFAULT);
        // push bound & 1
        exit_on_warnings=OFF;
        VoC_movreg16(REG5,ACC0_LO,IN_PARALLEL);
        exit_on_warnings=ON;
        //  L_sum = L_msu(L_sum, pswPOld[0], SW_MIN);
        VoC_msu32_rp(REG5,REG0,DEFAULT);
        exit_on_warnings=OFF;
        VoC_push32(REG45,IN_PARALLEL);
        exit_on_warnings=ON;

        //  L_sum = L_mac(L_sum, pswPOld[0], pswQntRcSqd[j]);
        VoC_mac32inc_rp(REG7,REG0,DEFAULT);
        //  load 0x8000 in REG5


        // STACK16                 STACK32
        // ---------------------------------------------
        // RA                      pswPBar & pswVBar
        // pswQntRc                ppswPAddrs[0] & ppswVAddrs[0]
        // swSegmentOrder          ppswPAddrs[1] & ppswVAddrs[1]
        // pswRc                   qntLst.pswPrdErr & iCnt
        // iLimit                  ppswPVAddrs[j%2] (Old)
        //                         ppswPVAddrs[(j+1)%2] (New)
        //                         0x8000
        //                         pswQntRc[j+1] & pswQntRcSqd[j+1]
        //                         bound & 1


        // if bound == 0
        VoC_bez16_r(CII_aflatRecurLoop_skipL1,REG4);


        // for (i = 1; i <= bound; i++)
        VoC_loop_r_short(REG4,DEFAULT)

        VoC_startloop0


        // load pswVOld[-i-1]
        VoC_lw16inc_p(REG4,REG3,DEFAULT);
        //  L_mac(L_ROUND, pswVOld[i+1], pswQntRc[j])
        VoC_mac32inc_rp(REG6,REG1,DEFAULT);
        //  ACC0 <- L_ROUND before accumulation
        VoC_lw32_sd(ACC0,2,IN_PARALLEL);
        //  L_mac(L_sum, pswVOld[-i-1], pswQntRc[j]);
        VoC_mac32_rr(REG6,REG4,DEFAULT);
        exit_on_warnings=OFF;
        //  pswPNew[i] = extract_h(L_sum);
        VoC_sw16inc_p(ACC0_HI,REG2,IN_PARALLEL);
        exit_on_warnings=ON;
        //  L_sum = L_mac(L_sum, pswPOld[i+1], pswQntRcSqd[j]);
        VoC_mac32_rp(REG7,REG0,DEFAULT);
        //  L_sum = L_msu(L_sum, pswPOld[i+1], SW_MIN);
        VoC_msu32inc_rp(REG5,REG0,DEFAULT);

        VoC_endloop0



CII_aflatRecurLoop_skipL1:


        // last write still pending ....
        VoC_pop32(REG45,DEFAULT);
        VoC_lw16i_short(INC0,-1,DEFAULT);

        // STACK16                 STACK32
        // ---------------------------------------------
        // RA                      pswPBar & pswVBar
        // pswQntRc                ppswPAddrs[0] & ppswVAddrs[0]
        // swSegmentOrder          ppswPAddrs[1] & ppswVAddrs[1]
        // pswRc                   qntLst.pswPrdErr & iCnt
        // iLimit                  ppswPVAddrs[j%2] (Old)
        //                         ppswPVAddrs[(j+1)%2] (New)
        //                         0x8000
        //                         pswQntRc[j+1] & pswQntRcSqd[j+1]


        VoC_lw16i_short(INC3,1,DEFAULT);
        // compute pswPNew[0]
        VoC_sub16_rr(REG2,REG2,REG4,IN_PARALLEL);

        exit_on_warnings=OFF;
        //  pswPNew[bound] = extract_h(L_sum);
        VoC_sw16_p(ACC0_HI,REG2,DEFAULT);
        exit_on_warnings=ON;
        // last write still pending ....

        VoC_lw16i_short(INC1,-1,IN_PARALLEL);
        // ppsPVNew in ACC1
        VoC_lw32_sd(ACC1,2,DEFAULT);

        // if (sub(pswPNew[0], 0x4000) >= 0) iLimit = 1
        VoC_lw16_p(REG2,REG2,IN_PARALLEL);
        VoC_blt16_rd(CII_aflatRecurLoop_PNew,REG2,CONST_0x4000_ADDR);
        VoC_sw16_sd(REG5,0,DEFAULT);
CII_aflatRecurLoop_PNew:


        // STACK16                 STACK32
        // ---------------------------------------------
        // RA                      pswPBar & pswVBar
        // pswQntRc                ppswPAddrs[0] & ppswVAddrs[0]
        // swSegmentOrder          ppswPAddrs[1] & ppswVAddrs[1]
        // pswRc                   qntLst.pswPrdErr & iCnt
        // iLimit                  ppswPVAddrs[j%2] (Old)
        //                         ppswPVAddrs[(j+1)%2] (New)
        //                         0x8000
        //                         pswQntRc[j+1] & pswQntRcSqd[j+1]



        // REG0 : &pswPOld[ bound+1], INC0 = -1
        // REG1 : &pswVOld[ bound+1], INC1 = -1
        // REG2 : &pswPNew[ bound+1], INC2 =  1
        // REG3 : &pswVOld[-bound-1], INC3 =  1
        // REG4 : bound
        // REG5 : 1
        // REG6 : pswQntRc[j]
        // REG7 : pswQntRcSqd[j]


        // initiate pswVNew, compute 2*bound
        VoC_movreg16(REG2,ACC1_HI,DEFAULT);
        VoC_lw16i_short(INC2,-1,DEFAULT);


        // for (i = bound; i >= 0; i--)
        // {
        //   L_sum = L_msu(L_ROUND, pswVOld[i + 1], SW_MIN);
        //   L_sum = L_mac(L_sum, pswQntRcSqd[j], pswVOld[-i - 1]);
        //   L_sum = 2*L_mult(pswQntRc[j], pswPOld[ i + 1]) + L_sum;
        //   pswVNew[i] = extract_h(L_sum);
        // }


        // REG0 : &pswPOld[ bound+1], INC0 = -1
        // REG1 : &pswVOld[ bound+1], INC1 = -1
        // REG2 : &pswVNew[ bound  ], INC2 = -1
        // REG3 : &pswVOld[-bound-1], INC3 =  1
        // REG4 : bound
        // REG5 : 0x8000
        // REG6 : pswQntRc[j]
        // REG7 : pswQntRcSqd[j]


        // OPTIMIZED FOR SPEED !!!!


        // L_mult = L_mult(pswQntRc[j], pswPOld[ i + 1]
        VoC_multf32inc_rp(ACC1,REG6,REG0,DEFAULT);
        //  ACC0 <- L_ROUND before accumulation
        VoC_lw32_sd(ACC0,1,IN_PARALLEL);

        VoC_add16_rr(REG2,REG2,REG4,DEFAULT);
        VoC_movreg16(REG5,ACC0_LO,IN_PARALLEL);

        // L_sum = L_msu(L_ROUND, pswVOld[i + 1], SW_MIN);
        VoC_msu32inc_rp(REG5,REG1,DEFAULT);

        // L_sum = L_mac(L_sum, pswQntRcSqd[j], pswVOld[-i - 1]);
        VoC_mac32inc_rp(REG7,REG3,DEFAULT);
        // L_mult = 2*L_mult
        VoC_shr32_ri(ACC1,-1,IN_PARALLEL);



        // if bound == 0, skip both loops
        VoC_bez16_r(CII_aflatRecurLoop_skipL2,REG4);


        // (j < swSegmentOrder - 2) <=> (bound > 0),
        // so we can swap the vectors in the stack
        VoC_lw32_sd(RL6,3,DEFAULT);
        VoC_lw32_sd(RL7,2,DEFAULT);
        VoC_sw32_sd(RL6,2,DEFAULT);

        // for (i = bound-1; i >= 0; i--)
        VoC_loop_r_short(REG4,DEFAULT)

        VoC_startloop0
        // L_mult[i] = L_mult(pswQntRc[j], pswPOld[ i + 1]
        VoC_multf32inc_rp(ACC1,REG6,REG0,DEFAULT);
        // L_sum = L_mult[i+1] + L_sum;
        VoC_add32_rr(RL6,ACC0,ACC1,IN_PARALLEL);
        // L_sum = L_msu(L_ROUND, pswVOld[i + 1], SW_MIN);
        VoC_msu32inc_rp(REG5,REG1,DEFAULT);
        //  ACC0 <- L_ROUND before accumulation
        VoC_lw32_sd(ACC0,1,IN_PARALLEL);
        // L_sum = L_mac(L_sum, pswQntRcSqd[j], pswVOld[-i - 1]);
        VoC_mac32inc_rp(REG7,REG3,DEFAULT);
        // L_mult[i] = 2*L_mult[1]
        VoC_shr32_ri(ACC1,-1,IN_PARALLEL);

        exit_on_warnings=OFF;
        // pswVNew[i] = extract_h(L_sum)
        VoC_sw16inc_p(RL6_HI,REG2,DEFAULT);
        exit_on_warnings=ON;

        VoC_endloop0;


        // for (i = -1; i >= -bound; i--)
        // {
        //   L_sum = L_msu(L_ROUND, pswVOld[i + 1], SW_MIN);
        //   L_sum = L_mac(L_sum, pswQntRcSqd[j], pswVOld[-i - 1]);
        //   L_sum = 2*L_mult(pswQntRc[j], pswPOld[-i - 1]) + L_sum;
        //   pswVNew[i] = extract_h(L_sum);
        // }

        // REG0 : &pswPOld[ 0], INC0 =  1
        // REG1 : &pswVOld[ 0], INC1 = -1
        // REG2 : &pswVNew[-1], INC2 = -1
        // REG3 : &pswVOld[ 0], INC3 =  1
        // REG4 : bound
        // REG5 : 1
        // REG6 : pswQntRc[j]
        // REG7 : pswQntRcSqd[j]

        VoC_lw16i_short(INC0,1,DEFAULT);
        VoC_sw32_sd(RL7,3,IN_PARALLEL);

        // for (i = -1; i >= -bound; i--)
        VoC_loop_r_short(REG4,DEFAULT)
        VoC_startloop0
        // L_mult[i] = L_mult(pswQntRc[j], pswPOld[-i - 1]
        VoC_multf32inc_rp(ACC1,REG6,REG0,DEFAULT);
        // L_sum = L_mult[i+1] + L_sum;
        VoC_add32_rr(RL6,ACC0,ACC1,IN_PARALLEL);
        // L_sum = L_msu(L_ROUND, pswVOld[i + 1], SW_MIN);
        VoC_msu32inc_rp(REG5,REG1,DEFAULT);
        //  ACC0 <- L_ROUND before accumulation
        VoC_lw32_sd(ACC0,1,IN_PARALLEL);
        // L_sum = L_mac(L_sum, pswQntRcSqd[j], pswVOld[-i - 1]);
        VoC_mac32inc_rp(REG7,REG3,DEFAULT);
        // L_mult[i] = 2*L_mult[1]
        VoC_shr32_ri(ACC1,-1,IN_PARALLEL);

        exit_on_warnings=OFF;
        // pswVNew[i] = extract_h(L_sum)
        VoC_sw16inc_p(RL6_HI,REG2,DEFAULT);
        exit_on_warnings=ON;


        VoC_endloop0;


CII_aflatRecurLoop_skipL2:


        // last write still pending ....
        // L_sum = L_mult[-bound] + L_sum;
        VoC_add32_rr(RL6,ACC0,ACC1,DEFAULT);
        VoC_lw16i_short(INC1,1,DEFAULT);
        // last write still pending ....

        VoC_lw16i_short(INC2,1,DEFAULT);
        // prepare new vals
        VoC_lw32_sd(REG01,0,IN_PARALLEL);


        // pswVNew[-bound] = extract_h(L_sum)
        VoC_sw16inc_p(RL6_HI,REG2,DEFAULT);
        VoC_lw16i_short(INC0,1,IN_PARALLEL);

        // STACK16                 STACK32
        // ---------------------------------------------
        // RA                      pswPBar & pswVBar
        // pswQntRc                ppswPAddrs[0] & ppswVAddrs[0]
        // swSegmentOrder          ppswPAddrs[1] & ppswVAddrs[1]
        // pswRc                   qntLst.pswPrdErr & iCnt
        // iLimit                  ppswPVAddrs[j%2] (Old)
        //                         ppswPVAddrs[(j+1)%2] (New)
        //                         0x8000
        //                         pswQntRc[j+1] & pswQntRcSqd[j+1]


        VoC_lw32_sd(REG23,2,DEFAULT);
        VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);

        VoC_lw16inc_p(REG7,REG1,DEFAULT);
        VoC_lw16i_short(INC3,-1,IN_PARALLEL);
        // REG0 : &pswQntRc[j+2],    INC0 =  1
        // REG1 : &pswQntRcSqd[j+2], INC1 =  1
        // REG2 : pswPNew(j+1),      INC2 =  1
        // REG3 : pswVNew(j+1),      INC3 = -1
        // REG4 : (bound-1)+1
        // REG5 : 1
        // REG6 : pswQntRc[j+1]
        // REG7 : pswQntRcSqd[j+1]

        // swap pswQntRc[j+2] & pswQntRcSqd[j+2] with Old
        VoC_lw32_sd(REG01,3,DEFAULT);
        VoC_lw16i_short(REG5,1,IN_PARALLEL);

        exit_on_warnings=OFF;
        VoC_sw32_sd(REG01,0,DEFAULT);
        exit_on_warnings=ON;


        // STACK16                      STACK32
        // ---------------------------------------------
        // RA                           pswPBar & pswVBar
        // quantList.pswPredErr         ppswPAddrs[0] & ppswVAddrs[0]
        // iCnt                         ppswPAddrs[1] & ppswVAddrs[1]
        // pswQntRc                     ppswPVAddrs[j%2] (Old)
        // swSegmentOrder               ppswPVAddrs[(j+1)%2] (New)
        // pswRc                        0x8000
        // iLimit                       pswQntRc[j+1] & pswQntRcSqd[j+1]


        // REG0 : pswPOld(j+1), INC0 =  1
        // REG1 : pswVOld(j+1), INC1 =  1
        // REG2 : pswPNew(j+1), INC2 =  1
        // REG3 : pswVNew(j+1), INC3 = -1
        // REG4 : (bound-1)+1
        // REG5 : 1
        // REG6 : pswQntRc[j+1]
        // REG7 : pswQntRcSqd[j+1]


        VoC_endloop1


        // STACK16                 STACK32
        // ---------------------------------------------
        // RA                      pswPBar & pswVBar
        // pswQntRc                ppswPAddrs[0] & ppswVAddrs[0]
        // swSegmentOrder          ppswPAddrs[1] & ppswVAddrs[1]
        // pswRc                   qntLst.pswPrdErr & iCnt
        // iLimit                  ppswPVAddrs[(swSgOrd-3)%2] (Old)
        //                         ppswPVAddrs[(swSgOrd-2)%2] (New)
        //                         0x8000
        //                         pswQntRc[swSgOrd] & pswQntRcSqd[swSgOrd]


        // REG0 : pswPOld, INC0 =  1
        // REG1 : pswVOld, INC1 =  1
        // REG2 : pswPNew, INC2 =  1
        // REG3 : pswVNew, INC3 = -1
        // REG4 : 0
        // REG5 : 1
        // REG6 : pswQntRc[swSgOrd-1]
        // REG7 : pswQntRcSqd[swSgOrd-1]


        // unpile the stack (iLimit in REG4) ...
        VoC_pop16(REG4,DEFAULT);
        VoC_pop32(ACC0,IN_PARALLEL);


        // STACK16                 STACK32
        // ---------------------------------------------
        // RA                      pswPBar & pswVBar
        // pswQntRc                ppswPAddrs[0] & ppswVAddrs[0]
        // swSegmentOrder          ppswPAddrs[1] & ppswVAddrs[1]
        // pswRc                   qntLst.pswPrdErr & iCnt
        //                         ppswPVAddrs[(swSgOrd-3)%2] (Old)
        //                         ppswPVAddrs[(swSgOrd-2)%2] (New)
        //                         0x8000



        // L_mac(L_ROUND, pswVNew[0],pswQntRc[j])
        VoC_mac32_rp(REG6,REG3,DEFAULT);
        // ACC0 <- L_ROUND before accumulation
        VoC_pop32(ACC0,IN_PARALLEL);
        // L_sum = L_mac(L_sum, pswVNew[0], pswQntRc[j]);
        VoC_mac32_rp(REG6,REG3,DEFAULT);
        // unpile the stack...
        VoC_pop32(ACC1,IN_PARALLEL);
        // L_sum = L_mac(L_sum, pswPNew[0], pswQntRcSqd[j]);
        VoC_mac32_rp(REG7,REG2,DEFAULT);
        // unpile the stack...
        VoC_pop32(ACC1,IN_PARALLEL);
        // L_sum = L_msu(L_sum, pswPNew[0], SW_MIN);
        VoC_msu32_pd(REG2,CONST_0x00008000L_ADDR);


        // STACK16                 STACK32
        // ---------------------------------------------
        // RA                      pswPBar & pswVBar
        // pswQntRc                ppswPAddrs[0] & ppswVAddrs[0]
        // swSegmentOrder          ppswPAddrs[1] & ppswVAddrs[1]
        // pswRc                   qntLst.pswPrdErr & iCnt

        // quantList.pswPredErr & iCnt
        VoC_pop32(REG01,DEFAULT);
        // pswRc
        VoC_pop16(REG3,IN_PARALLEL);
    }
    // if iLimit == 1, result is SW_MAX
    VoC_bez16_r(CII_aflatRecurLoop_no_max,REG4);
    VoC_lw16i(ACC0_HI,0x7fff);

CII_aflatRecurLoop_no_max:


    // iCnt--
    VoC_sub16_rr(REG1,REG1,REG5,DEFAULT);



    VoC_lw16i_short(INC3,1,DEFAULT);
    // *quantList.pswPredErr++ <= result
    VoC_sw16inc_p(ACC0_HI,REG0,IN_PARALLEL);

    // quantList.pswPredErr++
    VoC_push32(REG01,DEFAULT);


    // while(ICnt) ...
    VoC_bnez16_r(CII_aflatRecurLoop_begin,REG1);


    // STACK16                           STACK32
    // --------------------------------------------------
    // RA                                pswPBar & pswVBar
    // pswQntRc                          ppswPAddrs[0] & ppswVAddrs[0]
    // swSegmentOrder                    ppswPAddrs[1] & ppswVAddrs[1]
    // pswRc                             qntLst.pswPrdErr & iCnt = qntLst.iNum


    // unpile stack
    VoC_pop16(REG0,DEFAULT);
    VoC_pop16(REG0,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_pop32(REG01,DEFAULT);



    // STACK16                           STACK32
    // --------------------------------------------------
    //                                   pswPBar & pswVBar
    //                                   ppswPAddrs[0] & ppswVAddrs[0]
    //                                   ppswPAddrs[1] & ppswVAddrs[1]


    VoC_return;

}






// ********************************
// CII_lpcCorrQntz_opt
// Input:
//      REG0 -> pL_CorrelSeq, INC0 = 2
//      REG4 -> pswFinalRc
//      REG5 -> piVQCodewds
//  Output : none
//  Used regs : all
// ********************************


void CII_lpcCorrQntz_opt(void)
{

#if 0
    voc_short   LPCCORRQNTZ_pswPOldSpace_ADDR               ,4,x        //[4]shorts
    voc_short   LPCCORRQNTZ_pswVOldSpace_ADDR               ,7,x        //[7]shorts
    voc_short   LPCCORRQNTZ_pswPNewSpace_ADDR               ,4,x         //[4]shorts
    voc_short   LPCCORRQNTZ_pswVNewSpace_ADDR               ,7,x         //[7]shorts
    voc_short   LPCCORRQNTZ_pswPBar_ADDR                    ,4,x         //[4]shorts
    voc_short   LPCCORRQNTZ_pswVBarSpace_ADDR               ,8,x         //[8]shorts
    voc_short   LPCCORRQNTZ_pswRc_ADDR                      ,12,x        //[12]shorts
    voc_short   LPCCORRQNTZ_pL_PBarFull_ADDR                ,20,x        //[20]shorts
    voc_short   LPCCORRQNTZ_pL_VBarFullSpace_ADDR           ,38,x        //[38]shorts

#endif

#if 0

    voc_short   LPCCORRQNTZ_quantList_ADDR                  ,66,y       //[66]shorts
    voc_short   LPCCORRQNTZ_bestPql_ADDR                    ,264,y       //[264]shorts
    voc_short   LPCCORRQNTZ_bestQl_ADDR                     ,264,y       //[264]shorts

#endif


//spy_max_on = OFF;

    VoC_push16(RA,DEFAULT);
    VoC_push32(REG45,DEFAULT);


    // STACK16                           STACK32
    // --------------------------------------------------
    // RA                                pswFinalRc & piVQCodewds


    // *********************************
    // inline CII_initPBarFullVBarFullL
    // INPUTS:
    //        REG0(inc  2)->pL_CorrelSeq
    //        REG1(inc  2)->pL_PBarFull
    //        REG2(inc  2)->&pL_VBarFull[-2]
    //        REG3(inc -2)->&pL_VBarFull[-2]
    // OTHER USED REGS:
    //        RL6, RL7
    // *********************************


    VoC_lw16i_set_inc(REG1,LPCCORRQNTZ_pL_PBarFull_ADDR,2);
    VoC_lw16i_set_inc(REG2,LPCCORRQNTZ_pL_VBarFullSpace_ADDR+16, 2);
    VoC_lw16i_set_inc(REG3,LPCCORRQNTZ_pL_VBarFullSpace_ADDR+16,-2);
    // push pL_PBarFull
    VoC_push16(REG1,DEFAULT);
    VoC_loop_i_short(9,DEFAULT);

    VoC_startloop0;
    // L_shr(pL_CorrelSeq[i], RSHIFT);
    VoC_lw32inc_p(RL7,REG0,DEFAULT);
    VoC_shr32_ri(RL7,2,DEFAULT);
//      VoC_NOP();
    // some constants
    VoC_lw16i_short(REG6,7,DEFAULT);        // stephen 060711
    VoC_lw16i_short(REG7,11,IN_PARALLEL);
    // pL_PBarFull[i] = L_shr(pL_CorrelSeq[i], RSHIFT);
    // pL_VBarFull[+i-1] = L_shr(pL_CorrelSeq[i], RSHIFT);
    // pL_VBarFull[-i-1] = L_shr(pL_CorrelSeq[i], RSHIFT);
    VoC_sw32inc_p(RL7,REG1,DEFAULT);
    VoC_sw32inc_p(RL7,REG2,DEFAULT);
    VoC_sw32inc_p(RL7,REG3,DEFAULT);
    VoC_endloop0;

    VoC_lw32inc_p(RL7,REG0,DEFAULT);
    VoC_shr32_ri(RL7,2,DEFAULT);
    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_shr32_ri(RL6,2,DEFAULT);

    // pL_PBarFull[9] = L_shr(pL_CorrelSeq[9], RSHIFT);
    // pL_VBarFull[8] = L_shr(pL_CorrelSeq[9], RSHIFT);
    // pL_VBarFull[9] = L_shr(pL_CorrelSeq[10], RSHIFT);
    VoC_sw32inc_p(RL7,REG1,DEFAULT);
    VoC_sw32inc_p(RL7,REG2,DEFAULT);
    VoC_sw32inc_p(RL6,REG2,DEFAULT);



    // Compute pointers :

    // ppswPAddrs[0] = pswPOldSpace;
    // ppswPAddrs[1] = pswPNewSpace;
    // ppswVAddrs[0] = pswVOldSpace + NP_AFLAT - 1;
    // ppswVAddrs[1] = pswVNewSpace + NP_AFLAT - 1;
    // pswPBar
    // pswVBar = pswVBarSpace + NP_AFLAT - 1;
    // NP_AFLAT = 4

    // LPCCORRQNTZ_pswPOldSpace_ADDR        -> LPCCORRQNTZ_ADDR+0
    // LPCCORRQNTZ_pswVOldSpace_ADDR        -> LPCCORRQNTZ_ADDR+4
    // LPCCORRQNTZ_pswPNewSpace_ADDR        -> LPCCORRQNTZ_ADDR+11
    // LPCCORRQNTZ_pswVNewSpace_ADDR        -> LPCCORRQNTZ_ADDR+15
    // LPCCORRQNTZ_pswPBar_ADDR             -> LPCCORRQNTZ_ADDR+22
    // LPCCORRQNTZ_pswVBarSpace_ADDR        -> LPCCORRQNTZ_ADDR+26



    // pswVBar (+29)
    VoC_lw16i_set_inc(REG1,LPCCORRQNTZ_pswPOldSpace_ADDR+29,1);
    // pswPBar (+22)
    VoC_sub16_rr(REG0,REG1,REG6,DEFAULT);
    // ppswVAddrs[1] (+18)
    VoC_sub16_rr(REG3,REG1,REG7,IN_PARALLEL);
    // ppswPAddrs[1] (+11)
    VoC_sub16_rr(REG2,REG3,REG6,DEFAULT);
    // ppswVAddrs[0] (+7)
    VoC_sub16_rr(REG4,REG3,REG7,IN_PARALLEL);
    // ppswPAddrs[0] (+0)
    VoC_sub16_rr(REG5,REG4,REG6,DEFAULT);

    // push pswPBar & pswVBar
    VoC_push32(REG01,IN_PARALLEL);

    // &pswVBar[-2]
    VoC_lw16i_set_inc(REG1,LPCCORRQNTZ_pswPOldSpace_ADDR+27,1);

    // pop pL_PBarFull
    VoC_pop16(REG2,DEFAULT);

    // push ppswPAddrs[0] & ppswVAddrs[0]
    VoC_push32(REG45,IN_PARALLEL);

    exit_on_warnings = OFF;

    // push ppswPAddrs[1] & ppswVAddrs[1]
    VoC_push32(REG23,DEFAULT);

    exit_on_warnings = ON;

    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    // STACK16                   STACK32
    // -----------------------------------------
    // RA                        pswFinalRc & piVQCodewds
    //                           pswPBar & pswVBar
    //                           ppswPAddrs[0] & ppswVAddrs[0]
    //                           ppswPAddrs[1] & ppswVAddrs[1]


    // *********************************
    // inline CII_initPBarVBarL_opt
    // INPUTS:
    //        REG0(inc  1)->pswPBar
    //        REG1(inc  1)->&pswVBar[-2]
    //        REG2(inc  2)->pL_PBarFull
    // OTHER USED REGS:
    //        RL6, RL7, ACC0, ACC1
    // *********************************


    // bound = 2
    // for (i = 0; i <= bound; i++) round(pL_PBarFull[i]);
    VoC_lw32_d(RL7,CONST_0x00008000L_ADDR);

    VoC_add32inc_rp(RL6 ,RL7,REG2,DEFAULT);
    VoC_add32inc_rp(ACC0,RL7,REG2,DEFAULT);
    VoC_add32inc_rp(ACC1,RL7,REG2,DEFAULT);

    // for (i = 0; i <= bound; i++) pswPBar[i] = round(pL_PBarFull[i]);
    VoC_sw16inc_p(RL6_HI ,REG0,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);
    VoC_sw16inc_p(ACC1_HI,REG0,DEFAULT);

    // for (i = -bound; i < 0; i++) pswVBar[i] = pswPBar[-i - 1];
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    VoC_sw16inc_p(RL6_HI ,REG1,DEFAULT);

    //  round(pL_PBarFull[bound + 1]);
    VoC_add32inc_rp(RL6,RL7,REG2,DEFAULT);

    // for (i = 0; i < bound; i++)  pswVBar[i] = pswPBar[i + 1];
    // pswVBar[bound] = round(pL_PBarFull[bound + 1]);
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    VoC_sw16inc_p(ACC1_HI,REG1,DEFAULT);
    // iSeg in REG2
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    VoC_sw16inc_p(RL6_HI ,REG1,DEFAULT);




    // INC0 =  1
    // INC1 =  1
    // INC2 =  2
    // INC3 = -2


CII_lpcCorrQntz_opt_iSeg_loop:


    VoC_push16(REG2,DEFAULT);


    // STACK16                   STACK32
    // -----------------------------------------
    // RA                        pswFinalRc & piVQCodewds
    // iSeg                      pswPBar & pswVBar
    //                           ppswPAddrs[0] & ppswVAddrs[0]
    //                           ppswPAddrs[1] & ppswVAddrs[1]


    // *****************************************************
    // CII_aflatRecursionLoop_opt
    //
    // Input :
    //      REG1 : 0
    //      REG2 : iSeg, INC2 = 2
    //      REG5 : &psrPreQSz[-1]
    //      REG6 : TABLE_SP_ROM_psrPreQ1
    //      RL7  : TABLE_SP_ROM_psrPreQ23
    //
    // STACK32 : (+2) pswPBar & pswVBar
    //           (+1) ppswPAddrs[0] & ppswVAddrs[0]
    //           (+0) ppswPAddrs[1] & ppswVAddrs[1]
    //
    // Used regs : all
    //
    // INC output : INC0 = 1, INC1 = 1, INC2 = 1, INC3 = 1
    //
    // ****************************************************


    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_lw16i(REG5,  TABLE_SP_ROM_psrPreQSz_ADDR-1);
    VoC_lw16i(REG6,  TABLE_SP_ROM_psrPreQ1_ADDR);
    VoC_lw16i(RL7_LO,TABLE_SP_ROM_psrPreQ2_ADDR);
    VoC_lw16i(RL7_HI,TABLE_SP_ROM_psrPreQ3_ADDR);

    VoC_jal(CII_aflatRecursionLoop_opt);


    // ***********************************
    // CII_findBestInQuantList_opt
    // input:
    //    REG0  : psqlOutList   inc0 = 1
    //    REG2  :               inc2 = 1
    //    REG3  : psqlInList    inc3 = 1
    //    REG4  : iNumVectOut
    // ***********************************


    VoC_lw16i_short(REG4,4,DEFAULT);
    VoC_lw16i(REG0, LPCCORRQNTZ_bestPql_ADDR);
    VoC_lw16i(REG3, LPCCORRQNTZ_quantList_ADDR);

    VoC_jal(CII_findBestInQuantList_opt);


    // iNumVectOut = 4 is unchanged
    // for (invVec = 4; invVec > 0; invVec--)
    // invVec in REG4 (invVec = 4-iVec)


CII_lpcCorrQntz_opt_iVec_loop:


    VoC_push16(REG4,DEFAULT);

    // STACK16                   STACK32
    // -----------------------------------------
    // RA                        pswFinalRc & piVQCodewds
    // iSeg                      pswPBar & pswVBar
    // invVec                    ppswPAddrs[0] & ppswVAddrs[0]
    //                           ppswPAddrs[1] & ppswVAddrs[1]


    // *****************************************************
    // CII_aflatRecursionLoop_opt
    //
    // Input :
    //      REG1 : bestPql[iVec].iRCIndex
    //      REG2 : iSeg, INC2 = 2
    //      REG5 : &psrQuantSz[-1]
    //      REG6 : TABLE_SP_ROM_psrQuant1
    //      RL7  : TABLE_SP_ROM_psrQuant23
    //
    // STACK32 : (+2) pswPBar & pswVBar
    //           (+1) ppswPAddrs[0] & ppswVAddrs[0]
    //           (+0) ppswPAddrs[1] & ppswVAddrs[1]
    //
    // Used regs : all
    //
    // INC output : INC0 = 1, INC1 = 1, INC2 = 1, INC3 = 1
    //
    // ****************************************************


    // &bestPql[4-invVec].iRCIndex
    VoC_mult16_rd(REG4,REG4,CONST_66_ADDR);
    // &bestPql[4].iRCIndex
    VoC_lw16i_set_inc(REG2,LPCCORRQNTZ_bestPql_ADDR+1+4*66,2);

    VoC_sub16_rr(REG4,REG2,REG4,DEFAULT);

    // iSeq
    VoC_lw16_sd(REG2,1,DEFAULT);

    // bestPql[4-invVec].iRCIndex
    VoC_lw16_p(REG1,REG4,DEFAULT);

    // constant tables
    VoC_lw16i(REG5,  TABLE_SP_ROM_psrQuantSz_ADDR-1);
    VoC_lw16i(REG6,  TABLE_SP_ROM_psrQuant1_ADDR);
    VoC_lw16i(RL7_LO,TABLE_SP_ROM_psrQuant2_ADDR);
    VoC_lw16i(RL7_HI,TABLE_SP_ROM_psrQuant3_ADDR);

    VoC_jal(CII_aflatRecursionLoop_opt);


    // ***********************************
    // CII_findBestInQuantList_opt
    // input:
    //    REG0  : psqlOutList   inc0 = 1
    //    REG2  :               inc2 = 1
    //    REG3  : psqlInList    inc3 = 1
    //    REG4  : iNumVectOut
    // ***********************************


    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i(REG0, LPCCORRQNTZ_bestQl_ADDR);
    VoC_lw16i(REG3, LPCCORRQNTZ_quantList_ADDR);

    VoC_jal(CII_findBestInQuantList_opt);


    // STACK16                   STACK32
    // -----------------------------------------
    // RA                        pswFinalRc & piVQCodewds
    // iSeg                      pswPBar & pswVBar
    // invVec                    ppswPAddrs[0] & ppswVAddrs[0]
    //                           ppswPAddrs[1] & ppswVAddrs[1]


    // iSeq
    VoC_lw16_sd(REG2,1,DEFAULT);
    VoC_lw16i_short(REG3,2,IN_PARALLEL);


    // &bestQl[iSeq]
    VoC_mult16_rd(REG6,REG2,CONST_66_ADDR);
    // &bestQl[0]
    VoC_lw16i_set_inc(REG0,LPCCORRQNTZ_bestQl_ADDR,2);

    VoC_add16_rr(REG1,REG6,REG0,DEFAULT);

    // keep a copy of &bestQl[iSeq]
    VoC_add16_rr(REG6,REG6,REG0,IN_PARALLEL);

    // &bestQl[0].pswPredErr[0]
    // &bestQl[iSeg].pswPredErr[0]
    VoC_add16_rr(REG4,REG0,REG3,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_add16_rr(REG5,REG1,REG3,DEFAULT);

    // bestQl[0].pswPredErr[0]
    // bestQl[iSeg].pswPredErr[0]
    VoC_lw16_p(REG4,REG4,DEFAULT);
    VoC_lw16_p(REG5,REG5,DEFAULT);

    // invVec
    VoC_pop16(REG3,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);


    // STACK16                   STACK32
    // -----------------------------------------
    // RA                        pswFinalRc & piVQCodewds
    // iSeg                      pswPBar & pswVBar
    //                           ppswPAddrs[0] & ppswVAddrs[0]
    //                           ppswPAddrs[1] & ppswVAddrs[1]


    // if invVec != 4
    // if bestQl[iSeg].pswPredErr[0] <= bestQl[0].pswPredErr[0]
    VoC_be16_rd(CII_lpcCorrQntz_opt_copy,REG3,CONST_4_ADDR);
    VoC_bngt16_rr(CII_lpcCorrQntz_opt_no_copy,REG5,REG4);

CII_lpcCorrQntz_opt_copy:
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_loop_i_short(33,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0


CII_lpcCorrQntz_opt_no_copy:

    // invVec in REG3
    VoC_sub16_rr(REG4,REG3,REG7,DEFAULT);

    // while (invVec > 0) ...
    VoC_bnez16_r(CII_lpcCorrQntz_opt_iVec_loop,REG4);


    // iSeq in REG2
    // &bestQl[iSeq] in REG6
    // 1 in REG7
    // INC0 = 2, INC1 = 2, INC2 = 1, INC3 = 1


    // STACK16                   STACK32
    // -----------------------------------------
    // RA                        pswFinalRc & piVQCodewds
    // iSeg                      pswPBar & pswVBar
    //                           ppswPAddrs[0] & ppswVAddrs[0]
    //                           ppswPAddrs[1] & ppswVAddrs[1]


    // ******************************
    //  Input:     REG2->iSeg
    //             REG3->iVector
    //  Function: CII_setupQuant
    //             REG6<-psrQuant1;
    //             RL7_LO<-psrQuant2;
    //             RL7_HI<-psrQuant3;
    // ********************************


    // &bestQl[iSeg].iRCIndex
    VoC_add16_rr(REG3,REG6,REG7,DEFAULT);

    VoC_lw16i(REG6,  TABLE_SP_ROM_psrQuant1_ADDR);
    VoC_lw16i(RL7_LO,TABLE_SP_ROM_psrQuant2_ADDR);
    VoC_lw16i(RL7_HI,TABLE_SP_ROM_psrQuant3_ADDR);

    // bestQl[iSeg].iRCIndex
    VoC_lw16_p(REG3,REG3,DEFAULT);

    VoC_jal(CII_setupPreQ);


    // ******************************************
    // CII_getNextVec_opt
    // IN: REG3  &pswRC, incr3 1
    // ******************************************


    // pswFinalRc & piVQCodewds
    VoC_lw32_sd(REG23,3,DEFAULT);
    VoC_sub16_rd(REG3,REG2,CONST_1_ADDR);


    VoC_jal(CII_getNextVec_opt);

    // iSeg in REG7
    VoC_lw16_sd(REG7,0,DEFAULT);


    // *************************************
    // aflatNewBarRecursionL_opt
    // INPUT :
    //  REG0 (inc 2):  pL_PBar  (param2)
    //  REG1 (inc 2):  pL_VBar  (param3)
    //  REG2        :  pswQntRc (param0)
    //  REG4        :  pswPBar  (param4)
    //  REG5        :  pswVBar  (param5)
    //  REG7        :  iSegment (param1)
    // OUTPUT : none
    // USED REGS : all
    // *************************************



    VoC_blt16_rd(CII_lpcCorrQntz_opt_no_NewRecur1,REG7,CONST_3_ADDR);
    VoC_jump(CII_lpcCorrQntz_opt_no_NewRecur);
CII_lpcCorrQntz_opt_no_NewRecur1:
    // pL_PBar & pL_VBar
    VoC_lw16i_set_inc(REG0,LPCCORRQNTZ_pL_PBarFull_ADDR,2);
    VoC_lw16i_set_inc(REG1,LPCCORRQNTZ_pL_VBarFullSpace_ADDR+18,2);



    // &psvqIndex[iSeg - 1].l
    VoC_mult16_rd(REG6,REG7,CONST_4_ADDR);
    // &psvqIndex[-1].l
    VoC_lw16i(REG3,TABLE_SP_ROM_psvqIndex_ADDR-4);
    VoC_add16_rr(REG6,REG6,REG3,DEFAULT);

    // pswFinalRc & piVQCodewds
    VoC_lw32_sd(REG23,3,IN_PARALLEL);

    // pswPBar & pswVBar
    VoC_lw32_sd(REG45,2,DEFAULT);

    // &pswFinalRc[psvqIndex[iSeg - 1].l-1]
    VoC_add16_rp(REG2,REG2,REG6,DEFAULT);
    VoC_sub16_rd(REG2,REG2,CONST_1_ADDR);


//      begin of CII_aflatNewBarRecursionL_opt
    {


#if 0

        voc_short   LOCAL_AFLATNEWBARRECURSION_pL_POldSpace_ADDR        ,20,y   //[20]shorts
        voc_short   LOCAL_AFLATNEWBARRECURSION_pL_VOldSpace_ADDR        ,38,y    //[38]shorts
        voc_short   LOCAL_AFLATNEWBARRECURSION_pL_PNewSpace_ADDR        ,20,y    //[20]shorts
        voc_short   LOCAL_AFLATNEWBARRECURSION_pL_VNewSpace_ADDR        ,38,y    //[38]shorts

#endif


        VoC_push32(REG45,DEFAULT);

        VoC_lw16i(REG4,LOCAL_AFLATNEWBARRECURSION_pL_POldSpace_ADDR);
        VoC_lw16i(REG5,LOCAL_AFLATNEWBARRECURSION_pL_VOldSpace_ADDR+18);

        VoC_push16(REG2,DEFAULT);
        VoC_push32(REG01,IN_PARALLEL);


        VoC_push32(REG45,DEFAULT);


        // STACK16                 STACK32
        // ------------------------------------------
        // RA                     pswPBar & pswVBar
        // pswQntRc               pL_PBar & pL_VBar
        //                        ppL_Addrs_Old


        // iSegment * 4
        VoC_shr16_ri(REG7,-2,IN_PARALLEL);

        // &psvqIndex[-1].l
        VoC_lw16i_set_inc(REG3,TABLE_SP_ROM_psvqIndex_ADDR-4,2);

        // &psvqIndex[iSegment-1].l
        VoC_add16_rr(REG3,REG3,REG7,DEFAULT);
        VoC_lw16i_short(REG4,10,IN_PARALLEL);

        // pL_PNewSpace
        VoC_lw16i_set_inc(REG2,LOCAL_AFLATNEWBARRECURSION_pL_PNewSpace_ADDR,2);

        // psvqIndex[iSegment-1].l in REG5
        // psvqIndex[iSegment-1].len in REG6
        // psvqIndex[iSegment].len in REG7
        VoC_lw16inc_p(REG5,REG3,DEFAULT);
        VoC_lw16inc_p(REG6,REG3,DEFAULT);
        VoC_lw16inc_p(REG7,REG3,DEFAULT);
        VoC_lw16inc_p(REG7,REG3,DEFAULT);

        // pL_VNewSpace
        VoC_lw16i_set_inc(REG3,LOCAL_AFLATNEWBARRECURSION_pL_VNewSpace_ADDR+18,-2);

        // NP - psvqIndex[iSegment - 1].l - 1 + 1 (bound+1)
        VoC_sub16_rr(REG5,REG4,REG5,DEFAULT);
        VoC_lw16i_short(REG4,0,IN_PARALLEL);

        VoC_push32(REG23,DEFAULT);
        VoC_push16(REG3,IN_PARALLEL);
        VoC_push32(REG67,DEFAULT);
        VoC_push16(REG5,IN_PARALLEL);

        VoC_push16(REG4,DEFAULT);
        VoC_push16(REG4,DEFAULT);


        // STACK16                 STACK32
        // ------------------------------------------
        // RA                     pswPBar & pswVBar
        // pswQntRc               pL_PBar & pL_VBar
        // pL_VNew                ppL_Addrs_Old
        // bound+1                ppL_Addrs_New
        // j = 0                  psvqIdx[iSeg-1].len & psvqIdx[iSeg].len
        // swNShift = 0


        // pL_POld in REG0, INC0 =  2
        // pL_VOld in REG1, INC1 =  2
        // pL_PNew in REG2, INC2 =  2
        // pL_VNew in REG3, INC3 = -2
        // bound in REG5


CII_afNBR_loop_start:

        // for (j = 0; j < psvqIndex[iSegment - 1].len; j++)

        // pswQntRc in REG4
        VoC_lw16_sd(REG4,4,DEFAULT);

        // move pL_VOld
        VoC_movreg16(REG3,REG1,DEFAULT);

        // swQntRcSq REG7
        VoC_multf32_pp(REG67,REG4,REG4,DEFAULT);
        // swNShift computation flag
        VoC_lw16i_short(RL7_LO,1,DEFAULT);

        VoC_add32_rd(REG67,REG67,CONST_0x00008000L_ADDR);

        // pswQntRc[0]
        VoC_lw16_p(REG6,REG4,DEFAULT);


        // the subsequent inner loops
        // can be modified to :
        //
        //  for (i = 0; i <= bound; i++)
        //  {
        //      L_temp = L_mpy_ls(pL_VOld[i], pswQntRc[j]);
        //      L_sum = L_add(L_temp, pL_POld[i]);
        //      L_temp = L_mpy_ls(pL_POld[i], swQntRcSq);
        //      L_sum = L_add(L_temp, L_sum);
        //      L_temp = L_mpy_ls(pL_VOld[-i], pswQntRc[j]);
        //      L_temp = L_add(L_sum, L_temp);
        //      if (i == 0)
        //          swNShift = sub(norm_s(extract_h(L_temp)), RSHIFT);
        //      pL_PNew[i] = L_shl(L_temp, swNShift);
        //  }
        //
        //      /* Calculate VNew(i) */
        //  /*-------------------*/
        //
        //  for (i = bound; i >= -bound; i--)
        //  {
        //      L_temp = L_mpy_ls(pL_VOld[-i - 1], swQntRcSq);
        //      L_sum = L_add(L_temp, pL_VOld[i + 1]);
        //      if  (i < 0)
        //          L_temp = L_mpy_ls(pL_POld[-i - 1], pswQntRc[j]);
        //      else
        //          L_temp = L_mpy_ls(pL_POld[i + 1] , pswQntRc[j]);
        //      L_temp = L_shl(L_temp, 1);
        //      L_temp = L_add(L_temp, L_sum);
        //      pL_VNew[i] = L_shl(L_temp, swNShift);
        //  }




        // for (i = 0; i <= bound; i++)
        VoC_loop_r(1,REG5)


        /****************************/
        // CII_L_mpy_ls_opt
        // input ACC0, REG4
        // output ACC0
        // other used regs : REG5
        // unchanged regs  : REG4
        /****************************/


        // pL_POld in REG0, INC0 =  2
        // pL_VOld in REG1, INC1 =  2
        // pL_PNew in REG2, INC2 =  2
        // pL_VOld in REG3, INC3 = -2


        // L_temp = L_mpy_ls(pL_VOld[i], pswQntRc[j]);
        VoC_lw32inc_p(ACC0,REG1,DEFAULT);
        VoC_movreg16(REG4,REG6,IN_PARALLEL);
        VoC_jal(CII_L_mpy_ls_opt);


        // L_sum = L_add(L_temp, pL_POld[i]);
        VoC_add32_rp(ACC1,ACC0,REG0,DEFAULT);


        // L_temp = L_mpy_ls(pL_POld[i], swQntRcSq);
        VoC_lw32inc_p(ACC0,REG0,DEFAULT);
        VoC_movreg16(REG4,REG7,IN_PARALLEL);

        VoC_jal(CII_L_mpy_ls_opt);

        // L_sum = L_add(L_temp, L_sum);
        VoC_add32_rr(ACC1,ACC0,ACC1,DEFAULT);

        // L_temp = L_mpy_ls(pL_VOld[-i], pswQntRc[j]);
        VoC_lw32inc_p(ACC0,REG3,DEFAULT);
        VoC_movreg16(REG4,REG6,IN_PARALLEL);

        VoC_jal(CII_L_mpy_ls_opt);

        // L_sum = L_add(L_temp, L_sum);
        VoC_add32_rr(ACC1,ACC0,ACC1,DEFAULT);

        // swNShift = sub(norm_s(extract_h(L_temp)), RSHIFT) (if i==0)
        VoC_bez32_r(CII_afNBR_swNShift_cmpt,RL7)

        VoC_push16(REG0,DEFAULT);
        VoC_push16(REG2,DEFAULT);
        VoC_movreg16(REG2,ACC1_HI,IN_PARALLEL);
        VoC_jal(CII_norm_s_opt);
        VoC_pop16(REG2,DEFAULT);
        VoC_add16_rd(REG4,REG0,CONST_2_ADDR);
        VoC_pop16(REG0,DEFAULT);
        VoC_lw16i_short(INC0,2,IN_PARALLEL);
        VoC_lw32z(RL7,DEFAULT);
        VoC_sw16_sd(REG4,0,DEFAULT);

CII_afNBR_swNShift_cmpt:

        // STACK16                 STACK32
        // ------------------------------------------
        // RA                     pswPBar & pswVBar
        // pswQntRc               pL_PBar & pL_VBar
        // pL_VNew                ppL_Addrs_Old
        // bound+1                ppL_Addrs_New
        // j                      psvqIdx[iSeg-1].len & psvqIdx[iSeg].len
        // swNShift


        // swNShift
        VoC_lw16_sd(REG4,0,DEFAULT);
        VoC_shr32_rr(ACC1,REG4,DEFAULT);

        // pL_PNew[i] = L_shl(L_temp, swNShift);
        VoC_NOP();
        VoC_sw32inc_p(ACC1,REG2,DEFAULT);

        VoC_endloop1

        // pswQntRc & swQntRcSq
        VoC_movreg32(RL7,REG67,DEFAULT);


        // bound+1
        VoC_lw16_sd(REG4,2,DEFAULT);
        VoC_lw16i_short(REG6,1,IN_PARALLEL);

        // pL_VNew
        VoC_lw16_sd(REG2,3,DEFAULT);
        // (bound+1) --
        VoC_sub16_rr(REG4,REG4,REG6,IN_PARALLEL);

        // (bound+1-1) << 1
        VoC_shr16_ri(REG4,-1,DEFAULT);

        // keep bound value in REG7
        VoC_movreg16(REG7,REG4,IN_PARALLEL);

        // store back new (bound+1) before shift
        exit_on_warnings=OFF;
        VoC_sw16_sd(REG4,2,DEFAULT);
        exit_on_warnings=ON;

        // &pL_VNew[bound]
        VoC_add16_rr(REG2,REG2,REG4,IN_PARALLEL);

        // 2*bound + 1
        VoC_add16_rr(REG4,REG4,REG6,DEFAULT);

        // change increments

        VoC_lw16i_short(INC0,-2,IN_PARALLEL);

        VoC_lw16i_short(INC1,-2,DEFAULT);
        VoC_lw16i_short(INC3,2,IN_PARALLEL);


        // &pL_POld[bound+1]  in REG0, INC0 = -2
        // &pL_VOld[bound+1]  in REG1, INC1 = -2
        // &pL_VNew[-bound]   in REG2, INC2 = -2
        // &pL_VOld[-bound-1] in REG3, INC3 =  2
        // 1 in REG6
        // i in REG7
        // pswQntRc[j] in RL7_LO
        // swQntRcSq in RL7_HI



        //      /* Calculate VNew(i) */
        //  /*-------------------*/
        //
        //  for (i = bound; i >= -bound; i--)
        //  {
        //      L_temp = L_mpy_ls(pL_VOld[-i - 1], swQntRcSq);
        //      L_sum = L_add(L_temp, pL_VOld[i + 1]);
        //      if  (i < 0)
        //          L_temp = L_mpy_ls(pL_POld[-i - 1], pswQntRc[j]);
        //      else
        //          L_temp = L_mpy_ls(pL_POld[i + 1] , pswQntRc[j]);
        //      L_temp = L_shl(L_temp, 1);
        //      L_temp = L_add(L_temp, L_sum);
        //      pL_VNew[i] = L_shl(L_temp, swNShift);
        //  }


        VoC_loop_r_short(REG4,DEFAULT)
        VoC_lw16i_short(INC2,-2,IN_PARALLEL);
        VoC_startloop0

        // L_temp = L_mpy_ls(pL_VOld[-i - 1], swQntRcSq);
        VoC_lw32inc_p(ACC0,REG3,DEFAULT);
        VoC_movreg16(REG4,RL7_HI,IN_PARALLEL);
        VoC_jal(CII_L_mpy_ls_opt);


        // L_sum = L_add(L_temp, pL_VOld[i + 1]);
        VoC_add32inc_rp(ACC1,ACC0,REG1,DEFAULT);

        // L_temp = L_mpy_ls(pL_POld[(-/+i) - 1], pswQntRc[j]);
        VoC_lw32inc_p(ACC0,REG0,DEFAULT);
        VoC_movreg16(REG4,RL7_LO,IN_PARALLEL);

        VoC_jal(CII_L_mpy_ls_opt);

        // L_temp = L_shl(L_temp, 1); L_temp = L_add(L_temp, L_sum);
        VoC_shr32_ri(ACC0,-1,DEFAULT);
        VoC_add32_rr(ACC1,ACC0,ACC1,DEFAULT);

        // change increment if i == 0
        VoC_bnez16_r(CII_afNBR_incr_invert,REG7);

        VoC_lw16i_short(INC0,2,DEFAULT);
CII_afNBR_incr_invert:
        // swNShift
        VoC_lw16_sd(REG4,0,DEFAULT);
        VoC_shr32_rr(ACC1,REG4,DEFAULT);

        // i--
        VoC_sub16_rr(REG7,REG7,REG6,DEFAULT);

        // pL_VNew[i] = L_shl(L_temp, swNShift);
        VoC_sw32inc_p(ACC1,REG2,DEFAULT);

        VoC_endloop0

        // INC0 =  2
        // INC1 = -2
        // INC2 =  2
        // INC3 =  2


        // STACK16                 STACK32
        // ------------------------------------------
        // RA                     pswPBar & pswVBar
        // pswQntRc               pL_PBar & pL_VBar
        // pL_VNew                ppL_Addrs_Old
        // bound+1                ppL_Addrs_New
        // j                      psvqIdx[iSeg-1].len & psvqIdx[iSeg].len
        // swNShift


        // get j & psvqIndex[iSegment - 1].len & Old & New

        VoC_lw16_sd(REG4,1,DEFAULT);
        VoC_lw32_sd(REG67,0,IN_PARALLEL);

        VoC_lw16i_short(REG7,1,DEFAULT);
        VoC_lw32_sd(REG23,1,IN_PARALLEL);

        VoC_lw16_sd(REG5,1,DEFAULT);
        VoC_lw32_sd(REG01,2,IN_PARALLEL);

        // j & 1
        VoC_and16_rr(REG5,REG7,DEFAULT);

        // psvqIndex[iSegment - 1].len -2
        VoC_sub16_rd(REG6,REG6,CONST_2_ADDR);


        // pL_POldSpace         in REG0, INC0 =  2
        // pL_VOldSpace         in REG1, INC1 = -2
        // pL_PNewSpace         in REG2, INC2 =  2
        // pL_VNewSpace         in REG3, INC3 =  2
        // j                in REG4
        // j & 1            in REG5
        // psvqIdx[iSeg-1].len - 2  in REG6
        // 1                in REG7


        // if (j < psvqIndex[iSegment - 1].len - 2)
        VoC_bngt16_rr(CII_afNBR_swap1,REG6,REG4);

        // if j is even swap, if j is odd, do not swap
        VoC_bnez16_r(CII_afNBR_swap1,REG5);

        // swap
        VoC_movreg16(REG0,REG2,DEFAULT);
        VoC_movreg16(REG2,REG0,IN_PARALLEL);
        VoC_movreg16(REG1,REG3,DEFAULT);
        VoC_movreg16(REG3,REG1,IN_PARALLEL);

CII_afNBR_swap1:
        VoC_lw16i_short(INC1,2,DEFAULT);
        // if (j == psvqIndex[iSegment - 1].len - 2)
        VoC_bne16_rr(CII_afNBR_swap2,REG4,REG6);

        // if j is even swap, if j is odd, do not swap
        VoC_bnez16_r(CII_afNBR_swap3,REG5);
        // swap
        VoC_movreg16(REG0,REG2,DEFAULT);
        VoC_movreg16(REG1,REG3,IN_PARALLEL);
CII_afNBR_swap3:
        // load New
        VoC_lw32_sd(REG23,3,DEFAULT);
CII_afNBR_swap2:


        // pswQntRc in REG4
        VoC_lw16_sd(REG5,4,DEFAULT);
        VoC_add16_rr(REG5,REG5,REG7,DEFAULT);


        // j++ & psvqIndex[iSegment - 1].len in REG6
        VoC_add16_rr(REG4,REG4,REG7,DEFAULT);
        VoC_lw32_sd(REG67,0,IN_PARALLEL);

        // STACK16                 STACK32
        // ------------------------------------------
        // RA                     pswPBar & pswVBar
        // pswQntRc               pL_PBar & pL_VBar
        // pL_VNew                ppL_Addrs_Old
        // bound+1                ppL_Addrs_New
        // j                      psvqIdx[iSeg-1].len & psvqIdx[iSeg].len
        // swNShift


        // store back local vars
        VoC_sw16_sd(REG5,4,DEFAULT);

        VoC_sw16_sd(REG4,1,DEFAULT);
        VoC_lw16i_short(INC3,-2,IN_PARALLEL);


        VoC_lw16_sd(REG5,2,DEFAULT);

        VoC_sw16_sd(REG3,3,DEFAULT);
        VoC_lw16i_short(INC2,2,IN_PARALLEL);
        // pL_POld      in REG0, INC0 =  2
        // pL_VOld      in REG1, INC1 =  2
        // pL_PNew      in REG2, INC2 =  2
        // pL_VNew      in REG3, INC3 = -2
        // j++          in REG4
        // pswQntRc++       in REG5
        // psvqIdx[iSeg-1].len  in REG6
        // psvqIdx[iSeg].len    in REG7


        // for (j = 0; j < psvqIndex[iSegment - 1].len; j++)
        VoC_bne16_rr(CII_afNBR_loop_start,REG4,REG6);


        // STACK16                 STACK32
        // ------------------------------------------
        // RA                     pswPBar & pswVBar
        // pswQntRc               pL_PBar & pL_VBar
        // pL_VNew                ppL_Addrs_Old
        // bound+1                ppL_Addrs_New
        // j                      psvqIdx[iSeg-1].len & psvqIdx[iSeg].len
        // swNShift

        // here bound+1=psvqIdx[iSeg-1].len in REG7

        // unpile stacks
        /*  VoC_pop16(REG4,DEFAULT);
            VoC_pop32(REG01,IN_PARALLEL);
            VoC_pop16(REG4,DEFAULT);
            VoC_pop32(REG01,IN_PARALLEL);
            VoC_pop16(REG4,DEFAULT);
            VoC_pop32(REG01,IN_PARALLEL);


            // pL_PBar & pL_VBar
            VoC_pop16(REG4,DEFAULT);
            VoC_pop32(REG23,IN_PARALLEL);
            // pswPBar & pswVBar
            VoC_pop16(REG4,DEFAULT);
            VoC_pop32(REG01,IN_PARALLEL);
        */
        VoC_loop_i_short(5,DEFAULT)
        VoC_startloop0
        VoC_movreg32(REG23,REG01,DEFAULT);
        VoC_pop16(REG4,DEFAULT);
        VoC_pop32(REG01,IN_PARALLEL);
        VoC_endloop0;

        VoC_lw16i_short(INC0,1,DEFAULT);
        VoC_lw16i_short(INC1,1,IN_PARALLEL);
        VoC_lw32_d(ACC0,CONST_0x00008000L_ADDR);

        // pswPBar      in REG0, INC0 =  1
        // pswVBar      in REG1, INC1 =  1
        // pL_PBar      in REG2, INC2 =  2
        // pL_VBar      in REG3, INC3 = -2
        // bound        in REG7
        // 0x00008000       in ACC0


        // for (i = 0; i <= bound; i++)
        // {
        //   pswPBar[ i] = round(pL_PBar[ i]);
        // }
        // for (i = 0; i <= bound; i++)
        // {
        //   pswVBar[ i] = round(pL_VBar[ i]);
        //   pswVBar[-i] = round(pL_VBar[-i]);
        // }

        VoC_loop_r_short(REG7,DEFAULT);
        VoC_add32inc_rp(ACC1,ACC0,REG2,IN_PARALLEL);
        VoC_startloop0
        VoC_add32inc_rp(ACC1,ACC0,REG2,DEFAULT);
        exit_on_warnings=OFF;
        VoC_sw16inc_p(ACC1_HI,REG0,DEFAULT);
        exit_on_warnings=ON;
        VoC_endloop0


        // pswPBar      in REG0, INC0 =  1
        // pswVBar      in REG1, INC1 =  1
        // pL_PBar      in REG2, INC2 =  2
        // pL_VBar      in REG3, INC3 = -2
        // bound        in REG7
        // 0x00008000       in ACC0

        VoC_movreg16(REG2,REG3,DEFAULT);
        VoC_movreg16(REG0,REG1,IN_PARALLEL);


        // pswVBar      in REG0, INC0 =  1
        // pswVBar      in REG1, INC1 = -1
        // pL_VBar      in REG2, INC2 =  2
        // pL_VBar      in REG3, INC3 = -2
        // bound        in REG7
        // 0x00008000       in ACC0


        VoC_loop_r_short(REG7,DEFAULT);
        VoC_lw16i_short(INC1,-1,IN_PARALLEL);
        VoC_startloop0
        VoC_add32inc_rp(RL6,ACC0,REG2,DEFAULT);
        VoC_add32inc_rp(RL7,ACC0,REG3,DEFAULT);
        VoC_sw16inc_p(RL6_HI,REG0,DEFAULT);
        VoC_sw16inc_p(RL7_HI,REG1,DEFAULT);
        VoC_endloop0

//  end of CII_aflatNewBarRecursionL_opt        stephen 060711

    }





CII_lpcCorrQntz_opt_no_NewRecur:

    // iSeg in REG2
    VoC_pop16(REG2,DEFAULT);
    VoC_lw16i_short(INC2,2,DEFAULT);

    // STACK16                   STACK32
    // -----------------------------------------
    // RA                        pswFinalRc & piVQCodewds
    //                           pswPBar & pswVBar
    //                           ppswPAddrs[0] & ppswVAddrs[0]
    //                           ppswPAddrs[1] & ppswVAddrs[1]

    // iSeg--
    VoC_add16_rd(REG2,REG2,CONST_1_ADDR);
    // 4-iSeg
    VoC_sub16_dr(REG3,CONST_4_ADDR,REG2);

    // while (iSeg < 4) ...
    VoC_bnez16_r(CII_lpcCorrQntz_opt_iSeg_loop,REG3);


    // STACK16                   STACK32
    // -----------------------------------------
    // RA                        pswFinalRc & piVQCodewds
    //                           pswPBar & pswVBar
    //                           ppswPAddrs[0] & ppswVAddrs[0]
    //                           ppswPAddrs[1] & ppswVAddrs[1]



    VoC_pop16(RA,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);
    VoC_lw16i_short(INC1,1,DEFAULT);

    VoC_pop32(REG01,IN_PARALLEL);

    VoC_pop32(REG01, DEFAULT);



    // STACK16                   STACK32
    // -----------------------------------------


    VoC_lw16i_set_inc(REG0,LPCCORRQNTZ_bestQl_ADDR+67,66);

    VoC_pop32(REG23, DEFAULT);
    VoC_lw16inc_p(REG7, REG0, DEFAULT);
    VoC_loop_i_short(3, DEFAULT);

    VoC_startloop0
    VoC_lw16inc_p(REG7, REG0, DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7, REG3, DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

//spy_max_on = ON;


    VoC_return;
}







/************************************
 * IN:
    REG0  &psqlOutList   inc0=1
                         inc2=1
    REG3  &psqlInList    inc3=1
    REG4  iNumVectOut

************************************/

void CII_findBestInQuantList_opt(void)
{

    //   STACK16
    //-----------------------------
    //   &psqlOutList

    VoC_push16(REG0,DEFAULT);

    VoC_lw16_d(REG1, CONST_0x7FFF_ADDR);
    VoC_lw16i_short(REG5, 1,DEFAULT);
    //   load REG6 psqlInList.iNum;
    //   load REG2 psqlInList.iRCIndex;
    VoC_lw16inc_p(REG6, REG3, IN_PARALLEL);
    VoC_lw16inc_p(REG2, REG3, DEFAULT);


    //   reg4 iNumVectOut
    VoC_loop_r_short(REG4,DEFAULT);
    VoC_startloop0
    //   psqlBestOutList[bstIndex].iNum = 1;
    VoC_sw16inc_p(REG5, REG0, DEFAULT);
    //   psqlBestOutList[bstIndex].iRCIndex = psqlInList.iRCIndex;
    VoC_sw16inc_p(REG2, REG0, DEFAULT);
    //   psqlBestOutList[bstIndex++], update after next instruction
    VoC_add16_rd(REG0,REG0,CONST_64_ADDR);
    //   psqlBestOutList[bstIndex].pswPredErr[0] = 0x7fff;
    exit_on_warnings=OFF;
    VoC_sw16_p(REG1, REG0, DEFAULT);
    exit_on_warnings=ON;

    VoC_endloop0


    VoC_pop16(REG0,DEFAULT);

    VoC_lw16i_short(INC0,66,IN_PARALLEL);
    //   REG0 psqlBestOutList[bstIndex].pswPredErr[0]
    VoC_add16_rd(REG0,REG0,CONST_2_ADDR);

    //   REG1 psqlBestOutList[bstIndex].iRCIndex
    // REG5 == 1
    VoC_sub16_rr(REG1,REG0,REG5,DEFAULT);
    VoC_lw16i_short(INC1,66,IN_PARALLEL);
    VoC_NOP();
    VoC_push32(REG01,DEFAULT);


    VoC_loop_r(1,REG6)

    //   REG3 &psqlInList.pswPredErr[quantIndex]
    //   REG2 psqlInList.iRCIndex + quantIndex
    VoC_lw16inc_p(REG5,REG3,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    //   REG4 iNumVectOut(not change in the loop)
    VoC_loop_r_short(REG4,DEFAULT);
    VoC_lw32_sd(REG01,0,IN_PARALLEL);
    VoC_startloop0;
    //   REG6 is a flag, for while finished
    VoC_bnez16_r(findBestInQuantList_LABEL3,REG6);
    //   if while is not finished
    VoC_lw16_p(REG7,REG0,DEFAULT);
    //   compare psqlInList.pswPredErr[quantIndex] & psqlBestOutList[bstIndex].pswPredErr[0]
    VoC_bgt16_rr(findBestInQuantList_LABEL1,REG7,REG5)
    //   REG1 point to REG1 psqlBestOutList[bstIndex+1].iRCIndex
    VoC_inc_p(REG1,DEFAULT);
    //   &psqlBestOutList[bstIndex++].pswPredErr[0]
    VoC_inc_p(REG0,IN_PARALLEL);
    //   should continue while
    VoC_jump(findBestInQuantList_LABEL2);
findBestInQuantList_LABEL1:
    //   when finished while, put the current psqInList value to psqlBestOutList
    //   this part will only be done once

    //   first set the while finished flag
    VoC_lw16i_short(REG6,1,DEFAULT);
    //   load REG7 current psqlBestOutList[bstIndex].iRCIndex
    VoC_lw16_p(REG7,REG1,IN_PARALLEL);
    //   psqlBestOutList[bstIndex].iRCIndex = psqlInList.iRCIndex + quantIndex;
    VoC_sw16inc_p(REG2,REG1,DEFAULT);
    //   continue the for loop
    VoC_jump(findBestInQuantList_LABEL4);


findBestInQuantList_LABEL3:
    //   load REG7 current psqlBestOutList[i].iRCIndex
    VoC_lw16_p(REG7,REG1,DEFAULT);
    //   psqlBestOutList[i].iRCIndex = psqlBestOutList[i-1].iRCIndex
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    exit_on_warnings=ON;


findBestInQuantList_LABEL4:
    //   load REG5 current psqlBestOutList[bstIndex].pswPredErr[0]
    VoC_lw16_p(REG5,REG0,DEFAULT);
    //   psqlBestOutList[bstIndex].pswPredErr[0] = psqlInList.pswPredErr[quantIndex];
    //   or
    //   psqlBestOutList[i].pswPredErr[0] = psqlBestOutList[i-1].pswPredErr[0]
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    exit_on_warnings=ON;


findBestInQuantList_LABEL2:
    VoC_NOP();

    VoC_endloop0;

    //   psqlInList.iRCIndex + quantIndex;
    VoC_inc_p(REG2,DEFAULT);

    VoC_endloop1;

    VoC_pop32(REG01,DEFAULT);
    VoC_return;

}




/*******************************************
IN: REG3  &pswRC
    incr3 1

********************************************/
void CII_getNextVec_opt(void)
{

    VoC_lw16d_set_inc(REG1,STATIC_EN_SP_FRM_iWordPtr_ADDR,1);  //iWordPtr in REG1

    //  update after the next instruction
    VoC_add16_rd(REG1,REG1,STATIC_EN_SP_FRM_psrTable_ADDR_ADDR);
    VoC_lw16i(REG0,TABLE_SP_ROM_psrSQuant_ADDR);        //psrSQuant in REG0
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16inc_p(REG4,REG1,IN_PARALLEL);

    // REG6 for temp3=psrSQuant[high(psrTable[iWordPtr])];
    VoC_movreg16(REG6,REG4,DEFAULT);
    VoC_shr32_ri(REG45,-8,IN_PARALLEL);
    VoC_and16_ri(REG6,0xff);
    VoC_add16_rr(REG7,REG6,REG0,DEFAULT);

    // REG6 for temp3=psrSQuant[high(psrTable[iWordPtr])];
    VoC_lw16inc_p(REG4,REG1,IN_PARALLEL);
    VoC_add16_rd(REG3,REG3,STATIC_EN_SP_FRM_iLow_ADDR);

    //REG7 for temp4=psrSQuant[low(psrTable[iWordPtr])];
    VoC_add16_rr(REG6,REG5,REG0,DEFAULT);
    VoC_lw16_p(REG7,REG7,IN_PARALLEL);
    VoC_sub16_rd(REG1,REG1,STATIC_EN_SP_FRM_psrTable_ADDR_ADDR);
    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    //  first  time in the loop, save a random value
    //  second time in the loop, save psrSQuant[high(psrTable[iWordPtr])] & psrSQuant[low(psrTable[iWordPtr]
    VoC_movreg16(REG7,REG4,DEFAULT);
    exit_on_warnings = OFF;
    VoC_push32(REG67,DEFAULT);
    exit_on_warnings = ON;

    VoC_and16_ri(REG7, 0xff);
    VoC_shr32_ri(REG45,-8, DEFAULT);
    VoC_add16_rr(REG7,REG7,REG0,IN_PARALLEL);

    // REG7 for temp4=psrSQuant[low(psrTable[iWordPtr])];
    VoC_add16_rr(REG6,REG5,REG0,DEFAULT);
    VoC_lw16i_short(REG5,3,IN_PARALLEL);

    VoC_lw16_p(REG7,REG7,DEFAULT);
    VoC_add16_rr(REG2,REG3,REG5,IN_PARALLEL);           //i++;

    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);

    VoC_bez16_d(N_iThree_L2,STATIC_EN_SP_FRM_iThree_ADDR);              //if ((iThree) && (iWordHalfPtr == LOW))
    VoC_bnez16_d(N_iThree_L1,STATIC_EN_SP_FRM_iWordHalfPtr_ADDR);
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_movreg16(REG4,REG5,IN_PARALLEL);
    VoC_movreg16(REG5,REG6,DEFAULT);
    VoC_movreg16(REG6,REG7,IN_PARALLEL);
    VoC_jump(N_iThree_LE);
N_iThree_L1:
    VoC_lw16i_short(REG0,0,DEFAULT);            //iWordHalfPtr = LOW;
    VoC_sub16_rd(REG1,REG1,CONST_1_ADDR);       // iWordPtr--;
    VoC_jump(N_iThree_LE);
N_iThree_L2:                                            //esle
    VoC_sw16_p(REG7,REG2,DEFAULT);              //pswRc[iLow+3] = temp4;
N_iThree_LE:

    VoC_sw16inc_p(REG4,REG3,DEFAULT);               //pswRc[iLow+0]
    VoC_sw16inc_p(REG5,REG3,DEFAULT);                           //pswRc[iLow+1]
    VoC_sw16_p(REG6,REG3,DEFAULT);                              //pswRc[iLow+2]

    VoC_sw32_d(REG01,STATIC_EN_SP_FRM_iWordHalfPtr_ADDR);
    VoC_return;
}




/***************************************
  Input:    REG2->iSeg
            REG3->iVector
  Function: CII_setupPreQ
            REG6<-psrPreQ1;
            RL7_LO<-psrPreQ2;
            RL7_HI<-psrPreQ3;
  Function: CII_setupQuant
            REG6<-psrQuant1;
            RL7_LO<-psrQuant2;
            RL7_HI<-psrQuant3;
****************************************/
void CII_setupPreQ(void)
{
    // reg5 for iThree
    VoC_lw16i_short(REG7, 1,DEFAULT);

    VoC_sub16_rr(REG1, REG2, REG7,DEFAULT);       //iLow = psvqIndex[iSeg - 1].l;
    VoC_shr16_ri(REG1, -2,DEFAULT);           //iThree = ((psvqIndex[iSeg - 1].h - iLow) == 2);

    VoC_lw16i(REG0, TABLE_SP_ROM_psvqIndex_ADDR);     //  if((iSeg==1)|(iSeg==2))
    VoC_add16_rr(REG0, REG1, REG0, DEFAULT);          //  {
    VoC_add16_rr(REG4, REG0, REG7,DEFAULT);           //  if(iSeg==1)  psrTable = psr_temp1;
    VoC_lw16_p(REG5, REG0, DEFAULT);                  //  if(iSeg==2)  psrTable = psr_temp2;
    VoC_sub16_pr(REG0, REG4, REG5, DEFAULT);          //   else
    VoC_sw16_d(REG5, STATIC_EN_SP_FRM_iLow_ADDR);     //   if (odd(iVector))
    //     iWordHalfPtr = LOW;

    //     iWordHalfPtr = HIGH;
    VoC_be16_rd(SETUPPREQ_100, REG0, CONST_2_ADDR);  //   }
    VoC_lw16i_short(REG7, 0,DEFAULT);                 //else if(iSeg==3)
SETUPPREQ_100:

    VoC_lw16i_short(REG4, 1,DEFAULT);
    VoC_movreg16(REG5,RL7_LO,IN_PARALLEL);
    VoC_be16_rd(SETUPPREQ_2, REG2, CONST_2_ADDR);     //   iWordPtr = iVector * 2;
    VoC_be16_rd(SETUPPREQ_3, REG2, CONST_3_ADDR);     //   iWordHalfPtr = HIGH;
    // case 1:
    VoC_movreg16(REG5,REG6,DEFAULT);

SETUPPREQ_2://case2

    VoC_movreg16(REG0, REG3, DEFAULT);

    VoC_mult16_rd(REG3, REG3, CONST_3_ADDR);
    // here REG4 == 1
    VoC_and16_rr(REG0,REG4,DEFAULT);

    VoC_shr16_ri(REG3, 1, DEFAULT);


    VoC_bez16_r(SETUPPREQ_end, REG0);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jump(SETUPPREQ_end);

SETUPPREQ_3:// case 3
    VoC_movreg16(REG5, RL7_HI,DEFAULT);
    VoC_shr16_ri(REG3, -1, IN_PARALLEL);

SETUPPREQ_end:
    VoC_sw16_d(REG7, STATIC_EN_SP_FRM_iThree_ADDR);
    VoC_sw16_d(REG5, STATIC_EN_SP_FRM_psrTable_ADDR_ADDR);
    VoC_sw16_d(REG3, STATIC_EN_SP_FRM_iWordPtr_ADDR);
    VoC_sw16_d(REG4, STATIC_EN_SP_FRM_iWordHalfPtr_ADDR);

    VoC_return;
}


/****************************/
// CII_flat_recursion_opt
//
//   description:
//
//   This function is used only in the CII_flat_opt function.
//   The stack is dependent on that of CII_flat_opt.
//   It implements the following C code:
//
//   L_Cik = L_shl(pppL_C[i][k++][j_1], swShift);
//   L_Cki = L_shl(pppL_C[k++][i][j_1], swShift);
//   L_Bik = L_shl(pppL_B[i][k++][j_1], swShift);
//   L_Fik = L_shl(pppL_F[i][k++][j_1], swShift);
//   L_TmpA = L_add(L_Cik, L_Cki);
//   L_TmpA = L_mpy_ls(L_TmpA, pswRc[j]);
//   L_TmpB = L_add(L_Bik, L_Fik);
//   L_TmpB = L_mpy_ls(L_TmpB, pswRc[j]);
//
//   input :
//  REG0 : pppL_C(i,k), INC0 = 4
//  REG1 : pppL_C(k,i), INC1 = 40
//  REG2 : pppL_B(i,k), INC2 = 4
//  REG3 : pppL_F(i,k), INC3 = 4
//  REG4 : pswRc[j]
//  REG6 : swShift
//  output:
//      RL6  : L_TmpB
//      ACC1 : L_TmpA
//  stack32 :
//      +3 : L_Cik
//      +2 : L_Cki
//      +1 : L_Bik
//      +0 : L_Fik
//  other used :
//  ACC1, RL7
//  unused :
//      REG7
//  not modified :
//      REG4, REG6
/****************************/


void CII_flat_recursion_opt(void)
{

    VoC_push16(RA,DEFAULT);

    // STACK16                 STACK32
    // --------------------------------------
    // RA                      pppL_B & pppL_F
    // &pswRc[j]               L_R0
    // pppL_C                  L_Cik
    // *pswVadScalAuto         L_Cki
    // swShift                 L_Bik
    // j                       L_Fik
    // *piR0Inx                pppL_C & pppL_C j_1
    // 2*j_1                   pppL_B & pppL_F j_1
    // pswRc[j]                pppL_C & pppL_C j_0
    // swRcSq                  pppL_B & pppL_F j_0
    // NP - j - 1
    // 2 - 4*j_1
    // RA


    // swap pointers in REG0 to REG3
    VoC_sw32_sd(REG01,1,DEFAULT);
    VoC_sw32_sd(REG23,0,DEFAULT);
    VoC_lw32_sd(REG01,3,DEFAULT);
    VoC_lw32_sd(REG23,2,DEFAULT);

    //   L_Cik = L_shl(pppL_C[i][k++][j_1], swShift);
    //   L_Cki = L_shl(pppL_C[k++][i][j_1], swShift);
    //   L_Bik = L_shl(pppL_B[i][k++][j_1], swShift);
    //   L_Fik = L_shl(pppL_F[i][k++][j_1], swShift);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32inc_p(ACC1,REG1,DEFAULT);
    VoC_shr32_rr(ACC0,REG6,IN_PARALLEL);
    VoC_lw32inc_p(RL6,REG2,DEFAULT);
    VoC_shr32_rr(ACC1,REG6,IN_PARALLEL);
    VoC_lw32inc_p(RL7,REG3,DEFAULT);
    VoC_shr32_rr(RL6,REG6,IN_PARALLEL);
    VoC_sw32_sd(ACC0,7,DEFAULT);
    VoC_shr32_rr(RL7,REG6,IN_PARALLEL);
    VoC_sw32_sd(ACC1,6,DEFAULT);

    // L_TmpA = L_add(L_Cik, L_Cki);
    VoC_add32_rr(ACC0,ACC0,ACC1,IN_PARALLEL);


    /****************************/
    // CII_L_mpy_ls_opt
    // input ACC0, REG4
    // output ACC0
    // other used register REG5
    // not modified : REG4
    /****************************/


    // L_TmpA = L_mpy_ls(L_TmpA, pswRc[j]);
    VoC_jal(CII_L_mpy_ls_opt);


    VoC_sw32_sd(RL6,5,DEFAULT);
    VoC_movreg32(ACC1,ACC0,DEFAULT);
    VoC_sw32_sd(RL7,4,DEFAULT);

    // L_TmpB = L_add(L_Bik, L_Fik);
    VoC_add32_rr(ACC0,RL7,RL6,IN_PARALLEL);

    // L_TmpB = L_mpy_ls(L_TmpB, pswRc[j]);
    VoC_jal(CII_L_mpy_ls_opt);


    VoC_pop16(RA,DEFAULT);
    VoC_movreg32(RL6,ACC0,DEFAULT);


    // swap pointers in REG0 to REG3
    VoC_sw32_sd(REG01,3,DEFAULT);
    VoC_sw32_sd(REG23,2,DEFAULT);
    VoC_lw32_sd(REG01,1,DEFAULT);
    VoC_lw32_sd(REG23,0,DEFAULT);


    VoC_return;
}


/***************************************************************
  CII_flat_opt

  INPUT :
    REG0 : pswSpeechIn  (param0), INC0 = 1
    REG1 : pswRc        (param1), INC1 = 1

    pL_VadAcf (param3) not passed, directly used
    in cov32 : LOCAL_AFLAT_pL_VadAcf_ADDR

  OUTPUT :
    ACC0 : return
    REG6 : *piR0Inx
    REG7 : *pswVadScalAuto

 **************************************************************/


void CII_flat_opt(void)
{

#if 0

    voc_short   LOCAL_FLAT_pppL_B_ADDR                  ,400,x           //[400]shorts
    voc_short   LOCAL_pswInScale_ADDR                   ,170,x           //[170]shorts
    voc_short   LOCAL_pL_Phi_ADDR                       ,22,x            //[22]shorts


#endif

#if 0
    voc_short   LOCAL_FLAT_pppL_C_ADDR                  ,400,y                 // long [200]
    voc_short   LOCAL_FLAT_pppL_F_ADDR                  ,400,y                 // long [200]
#endif




    VoC_push16(RA,DEFAULT);

    VoC_lw16i(REG2, LOCAL_FLAT_pppL_B_ADDR);
    VoC_lw16i(REG3, LOCAL_FLAT_pppL_F_ADDR);
    VoC_lw16i(REG4, LOCAL_FLAT_pppL_C_ADDR);

    VoC_push16(REG1,DEFAULT);

    VoC_push16(REG4,DEFAULT);
    VoC_push32(REG23,IN_PARALLEL);



    // STACK16                 STACK32
    // --------------------------------------
    // RA                     pppL_B & pppL_F
    // pswRc
    // pppL_C


    /**************************************/
    //
    // CII_cov32_opt
    //
    // INPUT:
    //  REG0 pswIn              INC=1
    //  REG1            INC=1
    //  REG2 pppL_B
    //  REG3 pppL_F
    //  REG4 pppL_C
    //
    //  pL_VadAcf (param3) not passed, directly
    //  used : LOCAL_AFLAT_pL_VadAcf_ADDR
    //
    // OUTPUT:
    //  RL7  *pL_R0
    //  REG6 *pswVadScalAuto
    //  REG5 swNormPwr (return)
    //
    /*************************************/


    // begin of CII_cov32_opt
    VoC_push16(REG0,DEFAULT);// STK16:&pswIn,RA
    VoC_push16(REG1,DEFAULT);// STK16:&pswVadScalAuto,&pswIn,RA
    VoC_push16(REG2,DEFAULT);// STK16:&pppL_B,&pswVadScalAuto,&pswIn,RA
    VoC_push16(REG3,DEFAULT);// STK16:&pppL_F,&pppL_B,&pswVadScalAuto,&pswIn,RA
    VoC_push16(REG4,DEFAULT);// STK16:&pppL_C,&pppL_F,&pppL_B,&pswVadScalAuto,&pswIn,RA


    VoC_lw16i_short(ACC0_HI,0x11,DEFAULT);// L_Pwr in ACC0
    VoC_lw16i(ACC0_LO,0x7f0b);
    VoC_loop_i(0,160);
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_shr16_ri(REG6,3,DEFAULT);
    VoC_mac32_rr(REG6,REG6,DEFAULT);
    VoC_endloop0

    VoC_lw16_sd(REG0,4,DEFAULT);// REG0 to pswIn
    VoC_add16_rd(REG1,REG0,CONST_00A0_ADDR);//pswIn[F_LEN+i-1] in REG1
    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_loop_i_short(10,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_shr16_ri(REG6,3,DEFAULT);
    VoC_shr16_ri(REG4,3,IN_PARALLEL);
    VoC_msu32_rr(REG6,REG6,DEFAULT);
    VoC_mac32_rr(REG4,REG4,DEFAULT);
    VoC_NOP();
    VoC_or32_rr(RL6,ACC0,DEFAULT);
    VoC_endloop0  // Lmax in RL6

    VoC_movreg32(ACC0,RL6,DEFAULT);
    VoC_jal(CII_NORM_L_ACC0);

    VoC_lw16i_short(REG7, 0,DEFAULT);
//  VoC_add16_rd(REG1,REG1,CONST_6_ADDR);
    VoC_sub16_dr(REG1,CONST_6_ADDR,REG1);
    VoC_bltz16_r(COV32a, REG1);
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_add16_rr(REG7, REG1, REG6,DEFAULT);
    VoC_shr16_ri(REG7, 1, DEFAULT);
    VoC_add16_rr(REG7, REG7, REG6,DEFAULT);//swNormSig in REG7
COV32a:

    VoC_movreg16(REG6,REG7,DEFAULT);
    VoC_shr16_ri(REG6, -1, DEFAULT);//swNormPwr in REG6

    //  reg7 swNormSig
    //  reg6 swNormPwr
    VoC_lw16_sd(REG0, 4, DEFAULT);// pswIn in REG0
    VoC_push16(REG6,DEFAULT); // swNormPwr on top of the stack
    VoC_lw16i_set_inc(REG1, LOCAL_pswInScale_ADDR, 1);// pswInScale in REG1
    VoC_lw16inc_p(REG4, REG0, DEFAULT);

    VoC_loop_i(0, 170); // A_LEN
    VoC_shr16_rr(REG4, REG7, DEFAULT);
    VoC_lw16inc_p(REG4, REG0, DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4, REG1, DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    /****************************************************************/
    VoC_lw16i_short(REG5,-1,DEFAULT);
// reg2 for pswInScale
// reg5 for k
// RL6  for L_max
    VoC_loop_i(1, 10);

    VoC_lw32z(ACC0, DEFAULT);
    // k=0 for first loop, and everytime k++
    VoC_add16_rd(REG5,REG5,CONST_1_ADDR);
    VoC_lw16i_set_inc(REG3, LOCAL_pswInScale_ADDR+10, 2);// &pswInScale[NP+n] in REG3

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_sub16_rr(REG0,REG3,REG5,DEFAULT); // reg0 &pswInscale[NP+n-k]
    VoC_push16(REG7,DEFAULT);

    VoC_aligninc_pp(REG0,REG0,DEFAULT);
    VoC_loop_i(0, 80);
    VoC_lw32inc_p(REG67, REG0, DEFAULT);
    VoC_bimac32inc_rp(REG67, REG3);
    VoC_endloop0

    VoC_pop16(REG7,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_bnez16_r(LAP1,REG5);
    VoC_push16(REG7, DEFAULT);
    VoC_movreg32(RL6,ACC0,DEFAULT);   // L_max in RL6
LAP1:


    VoC_movreg32(ACC1, ACC0, DEFAULT);//L_Pwr in ACC1
    VoC_lw16i(REG1, LOCAL_AFLAT_pL_VadAcf_ADDR);
    VoC_add16_rr(REG1, REG1, REG5, DEFAULT);

    VoC_add16_rr(REG1, REG1, REG5, DEFAULT);


    VoC_bnlt16_rd(cov32kk, REG5, CONST_9_ADDR);

    VoC_sw32_p(ACC0, REG1, DEFAULT);
    VoC_bez16_r(LAP2,REG5);
    VoC_lw16i_set_inc(REG3, LOCAL_pswInScale_ADDR+10, 1);// pswInScale[NP+kk] in REG3
    VoC_sub16_rr(REG0,REG3,REG5,DEFAULT); // reg0 &pswInScale[NP+kk-k]

    VoC_lw16inc_p(REG6, REG3, DEFAULT);
    VoC_msu32inc_rp(REG6, REG0, DEFAULT);

    VoC_loop_r_short(REG5,DEFAULT);
    VoC_startloop0;
    VoC_lw16inc_p(REG6, REG0, DEFAULT);
    VoC_msu32inc_rp(REG6, REG3, DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32_p(ACC0, REG1, DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

cov32kk:
    VoC_movreg32(ACC0, ACC1, DEFAULT);
    VoC_lw16_sd(REG7,0,DEFAULT);
    VoC_shr32_rr(ACC0, REG7, IN_PARALLEL);//L_temp in ACC0
    VoC_lw16i(REG7, TABLE_SP_ROM_pL_rFlatSstCoefs_ADDR-2);
    VoC_add16_rr(REG7, REG7, REG5, DEFAULT);
    VoC_add16_rr(REG7, REG7, REG5, DEFAULT);
    VoC_push32(REG45, DEFAULT);
    VoC_lw32_p(REG67, REG7, DEFAULT);//pL_rFlatSstCoefs[k-1] in RL7
    VoC_movreg32(REG45,ACC0,DEFAULT);

    VoC_push32(REG67,DEFAULT);
    /****************/
// input in ACC0(1) and RL7(2)
// output in ACC0
// used register REG67, REG45
    /****************/
    VoC_jal(CII_L_mpy_ll_opt);
    VoC_pop32(RL7,DEFAULT);


    VoC_movreg32(ACC1,ACC0,DEFAULT);
    VoC_movreg32(ACC0,ACC1,IN_PARALLEL);

    VoC_pop32(REG45, DEFAULT);

    /******************************************************/
    /*   for (kk = k + 1, i = 1; kk <= NP - 1; kk++, i++)
         {
          L_Pwr = L_msu(L_Pwr, pswInScale[NP + F_LEN - i],
                        pswInScale[NP + F_LEN - kk]);
          L_Pwr = L_mac(L_Pwr, pswInScale[NP - i], pswInScale[NP - kk]);
          PL_phi[i] = L_Pwr;
         }
    */

//L_temp in ACC0
//pL_phi[0] in ACC1
// reg0 for pswInScale[NP+F_LEN-i]
// reg1 for pswInScale[NP+F_LEN-kk]
// reg2 for pswInScale[NP-i]
// reg3 for pswInScale[NP-kk]
// reg4 for 10-k
// reg5 for k
// reg6 for &pL_phi[i]
// reg7 for temp

LAP2:
    // reg1 & pswInScale[NP + F_LEN - kk]
    VoC_lw16i_short(INC1,-1,DEFAULT);
    VoC_sub16_dr(REG4, CONST_10_ADDR, REG5);
    VoC_lw16i_set_inc(REG0, LOCAL_pswInScale_ADDR+169, -1); // reg0 & pswInScale[NP + F_LEN - i]
    VoC_sub16_rr(REG1, REG0, REG5, DEFAULT);
    VoC_lw16i_set_inc(REG2, LOCAL_pswInScale_ADDR+9, -1);// reg2 & pswInScale[NP - i]
    VoC_lw16i(REG6,LOCAL_pL_Phi_ADDR);
    VoC_add16_rr(REG6,REG5,REG6,DEFAULT);
    VoC_add16_rr(REG6,REG5,REG6,DEFAULT);
// reg3 pswInScale[NP - kk]
    VoC_lw16i_short(INC3,-1,DEFAULT);
    VoC_sub16_rr(REG3, REG2, REG5, DEFAULT);
    VoC_bnez16_r(LAP8,REG5);
    VoC_sw32_p(RL6,REG6,DEFAULT);     // save pL_phi[0]
LAP8:

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_startloop0;
    VoC_lw16inc_p(REG7, REG0, DEFAULT);
    VoC_msu32inc_rp(REG7, REG1, DEFAULT);
    VoC_lw16inc_p(REG7, REG2, DEFAULT);
    VoC_mac32inc_rp(REG7, REG3, DEFAULT);
    VoC_add16_rd(REG6,REG6,CONST_2_ADDR);
    VoC_or32_rr(RL6,ACC0,DEFAULT);
    VoC_sw32_p(ACC0,REG6,DEFAULT);
    VoC_endloop0;

    /****************************************************************/
    //reg1 for swPhiNorm
    //reg6 for swNormPwr
    //reg7 for -swTemp
    //ACC1 PL_phi[0]
    //ACC0 PL_phi[NP]
    //RL7  PL_phi[NP]
    //RL6  *PL_R0

    VoC_bnez16_r(LAP3,REG5);
    VoC_push32(RL6,DEFAULT);
    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_pop32(ACC0,IN_PARALLEL);
    VoC_jal(CII_NORM_L_ACC0);  //swPhiNorm in REG1
    VoC_lw16_sd(REG6,1,DEFAULT);
    //  VoC_add16_rr(REG6, REG6, REG1,DEFAULT);//swNormPwr in REG6
    VoC_sub16_rr(REG6, REG6, REG1,DEFAULT);//swNormPwr in REG6
    // VoC_add16_rd(REG7, REG1,CONST_1_ADDR);//-swTemp in REG7
    VoC_sub16_dr(REG7,CONST_1_ADDR, REG1);//-swTemp in REG7
    VoC_shr32_rr(RL6, REG7, DEFAULT);
    VoC_shr32_rr(ACC1, REG7, IN_PARALLEL);
    VoC_add32_rr(RL6, RL6, ACC1, DEFAULT);

    VoC_movreg32(ACC1,RL6,DEFAULT);
    VoC_and32_rd(ACC1, CONST_0x40000000L_ADDR);
    VoC_bnez32_r(cov32and, ACC1);
    VoC_shr32_ri(RL6, -1, DEFAULT);
    VoC_sub16_rd(REG6, REG6, CONST_1_ADDR);//swNormPwr in REG6
cov32and:
    VoC_push32(RL6, DEFAULT);
    VoC_sw16_sd(REG6, 1, DEFAULT);
    //  VoC_add16_rd(REG7, REG1, CONST_2_ADDR);//-swNorm in REG4
    VoC_sub16_dr(REG7, CONST_2_ADDR, REG1);//-swNorm in REG4

    VoC_lw32_d(ACC1,LOCAL_pL_Phi_ADDR);
    VoC_shr32_rr(ACC1, REG7, DEFAULT);//L_temp in RL7
    VoC_push16(REG7, DEFAULT);
    VoC_movreg32(ACC0,ACC1,DEFAULT);

LAP3:

    /****************************************************************/
    /*   for (kk = k + 1, i = 1; kk <= NP - 1; kk++, i++)
         {
          L_pwr = pL_phi[i];
          L_temp = L_shl(L_Pwr, swNorm);
          L_temp = L_mpy_ll(L_temp, pL_rFlatSstCoefs[k - 1]);

          pppL_F[i][kk][0] = L_temp;
          pppL_B[i - 1][kk - 1][0] = L_temp;
          pppL_C[i][kk - 1][0] = L_temp;
          pppL_C[kk][i - 1][0] = L_temp;
         }
    */

    //reg0  pppL_F[i][kk][0]
    //reg1  pppL_B[i - 1][kk - 1][0]
    //reg2  pppL_C[kk][i-1][0]
    //reg3  pppL_C[i][kk-1][0]
    //reg4  10-k
    //reg5  k
    //reg6  &PL_Phi[i]
    //reg7  temp
    //ACC0  L_temp
    //ACC1  init L_temp

    VoC_lw16_sd(REG2,3,DEFAULT);


    VoC_lw16i(REG6,LOCAL_pL_Phi_ADDR+2);

    VoC_lw16_sd(REG0,4,DEFAULT);
    VoC_lw16i_short(INC0,44,IN_PARALLEL);

    VoC_add16_rd(REG3,REG2,CONST_40_ADDR); // reg3 &pppL_C[1][0][0]

    VoC_mult16_rd(REG7,REG5,CONST_4_ADDR);

    VoC_lw16_sd(REG1,5,DEFAULT);
    VoC_lw16i_short(INC3,44,IN_PARALLEL);
    VoC_add16_rr(REG0,REG0,REG7,DEFAULT); // reg0 &pppL_F[0][k][0]
    VoC_lw16i_short(INC2,44,IN_PARALLEL);

    VoC_add16_rr(REG1,REG1,REG7,DEFAULT); // reg1 &pppL_B[0][k][0]
    VoC_add16_rr(REG2,REG2,REG7,DEFAULT); // reg2 &pppL_C[0][k-1][0]

    VoC_mult16_rd(REG7,REG5,CONST_40_ADDR);
    VoC_sub16_rd(REG2,REG2,CONST_4_ADDR);

    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);  // reg3 &pppL_C[k+1][0][0]
    VoC_add16_rr(REG6,REG5,REG6,DEFAULT);
    VoC_add16_rr(REG6,REG5,REG6,DEFAULT);

    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_bez16_r(LAP10,REG5);
    VoC_sw32_p(ACC1,REG2,DEFAULT);
LAP10:
    VoC_inc_p(REG2,DEFAULT);
    VoC_lw16_sd(REG7, 0, DEFAULT);

    // reg4 is one time more than in the C code

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_lw16i_short(INC1,44,IN_PARALLEL);
    VoC_startloop0;

    VoC_lw32_p(ACC0,REG6,DEFAULT);
    VoC_add16_rd(REG6,REG6,CONST_2_ADDR);

    VoC_shr32_rr(ACC0, REG7, DEFAULT);

    VoC_bez16_r(LAP7,REG5);


    VoC_push32(REG67, DEFAULT);
    VoC_movreg32(REG67,RL7,IN_PARALLEL);

    VoC_push32(REG45, DEFAULT);
    VoC_movreg32(REG45,ACC0,IN_PARALLEL);


    /****************/
    // input in ACC0(1) and RL7(2)
    // output in ACC0
    // used register REG67, REG45
    /****************/

    VoC_jal(CII_L_mpy_ll_opt);


    VoC_pop32(REG45,DEFAULT);
    VoC_pop32(REG67, DEFAULT);//L_temp in ACC0
LAP7:

    VoC_be16_rd(LAP5,REG4,CONST_1_ADDR);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_jump(LAP6);
LAP5:

    VoC_bez16_r(LAP6,REG5);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
LAP6:

    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sub16_rd(REG4,REG4,CONST_1_ADDR);

    VoC_endloop0;

    VoC_lw32z(ACC0,DEFAULT);

    VoC_endloop1;


    VoC_lw16i_set_inc(REG0,LOCAL_pswInScale_ADDR,2);
    VoC_lw16i_set_inc(REG1,LOCAL_pswInScale_ADDR+10,2);

    VoC_loop_i(1, 80);
    VoC_lw32inc_p(REG67, REG0, DEFAULT);
    VoC_bimac32inc_rp(REG67, REG1);
    VoC_endloop1

    VoC_lw32_d(REG67, TABLE_SP_ROM_pL_rFlatSstCoefs_ADDR+18);

    VoC_lw16_sd(REG3,0,DEFAULT);
    VoC_movreg32(REG45,ACC0,IN_PARALLEL);

    VoC_shr32_rr(REG45, REG3, DEFAULT);
    VoC_jal(CII_L_mpy_ll_opt);

    VoC_pop16(REG7,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);  //swNorm in REG7

    VoC_pop16(REG6,DEFAULT);  // *pswVadScalAuto in reg6
    VoC_pop16(REG5,DEFAULT);  // *swNormPwr in reg5

    VoC_pop16(REG7,DEFAULT);  // &pppL_c in reg7
    VoC_lw16i(REG4,36);

    VoC_add16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0;
    VoC_pop16(REG4,DEFAULT);
    VoC_endloop0;

    VoC_sw32_p(ACC0, REG7,DEFAULT);
    // end of CII_cov32_opt

    // *pswVadScalAuto in REG6
    // L_R0 in RL7
    // swRShifts in REG5

    VoC_push16(REG6,DEFAULT);

    // swRShifts = sub(S_SH + 2, swRShifts);
    VoC_sub16_dr(REG4, CONST_8_ADDR, REG5);


    // STACK16                 STACK32
    // --------------------------------------
    // RA                     pppL_B & pppL_F
    // pswRc
    // pppL_C
    // *pswVadScalAuto


    // -------------------------------
    // Function: void CII_sqroot(void)
    // input in RL7
    // output in REG7
    // used register ACC0, REG6, REG5
    // -------------------------------


    // swSqrtOut = 0 in REG7
    VoC_lw16i_short(REG7,0,DEFAULT);
    // f (L_R0 != 0)
    VoC_bez32_r(EN_FLAT_opt_100, RL7);
    VoC_jal(CII_sqroot_opt);
EN_FLAT_opt_100:

    // lo of REG67 = 0
    VoC_lw16i_short(REG6,0,DEFAULT);

    // swRShifts & 1
    VoC_movreg16(REG5,REG4,DEFAULT);
    VoC_and16_rd(REG5,CONST_1_ADDR);

    // preload swShift = 0; j = 0;
    VoC_push16(REG6,DEFAULT);
    VoC_push16(REG6,DEFAULT);

    // STACK16                 STACK32
    // --------------------------------------
    // RA                     pppL_B & pppL_F
    // pswRc
    // pppL_C
    // *pswVadScalAuto
    // swShift = 0
    // j = 0

    // L_temp = L_mult(swSqrtOut, 0x5a82);
    VoC_bez16_r(EN_FLAT_opt_101, REG5);
    VoC_multf32_rd(REG67,REG7,CONST_0x5a82_ADDR);
EN_FLAT_opt_101:

    // swRShifts = shr(swRShifts, 1);
    VoC_shr16_ri(REG4,1,DEFAULT);

    // if (swRShifts >= 0)
    VoC_bltz16_r(EN_FLAT_opt_103,REG4);
    VoC_shr32_rr(REG67,REG4,DEFAULT);
    VoC_jump(EN_FLAT_opt_104);
EN_FLAT_opt_103:
    // if (swRShifts < 0)
    VoC_lw32z(REG67,DEFAULT);
EN_FLAT_opt_104:


    // L_R0 = L_temp
    VoC_push32(REG67,DEFAULT);


    // --------------------
    // CII_r0Quant_opt
    // input in REG67
    // output in REG6
    // used register 0167, inc0 = 2
    // --------------------

    VoC_jal(CII_r0Quant_opt);

    // push *piR0Inx
    VoC_push16(REG6,DEFAULT);


    // STACK16                 STACK32
    // --------------------------------------
    // RA                      pppL_B & pppL_F
    // pswRc                   L_R0
    // pppL_C
    // *pswVadScalAuto
    // swShift = 0
    // j = 0
    // *piR0Inx


    // j = 0 in REG6
    // swShift = 0 in REG7
    VoC_lw32z(REG67,IN_PARALLEL);


EN_FLAT_opt_LOOP_START:

    // INC3 = 1

    VoC_lw16i_short(INC3,1,DEFAULT);
    // compute (NP - j - 1) * 44 in REG3
    VoC_mult16_rd(REG3,REG6,CONST_44_ADDR);
    // general purpose 1
    VoC_lw16i_short(REG4,1,DEFAULT);

    VoC_sub16_dr(REG3,CONST_396_ADDR,REG3);


    // j_1 = j % 2; in REG6
    VoC_and16_rr(REG6,REG4,DEFAULT);
    // 2*j_1
    VoC_shr16_ri(REG6,-1,DEFAULT);
    // pppL_C
    VoC_lw16_sd(REG5,4,DEFAULT);
    // put on stack 2*j_1 (j_0 = 1 -j_1 is not saved)
    VoC_push16(REG6,DEFAULT);


    // &pppL_C[0][0][j_1]
    VoC_add16_rr(REG5,REG5,REG6,DEFAULT);

    // &pppL_C[NP - j - 1][NP - j - 1][j_1]
    VoC_add16_rr(REG4,REG5,REG3,IN_PARALLEL);
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);


    // STACK16                 STACK32
    // --------------------------------------
    // RA                      pppL_B & pppL_F
    // &pswRc[j]               L_R0
    // pppL_C
    // *pswVadScalAuto
    // swShift
    // j
    // *piR0Inx
    // 2*j_1


    VoC_lw32_p(RL7,REG5,DEFAULT);
    VoC_lw32_p(RL6,REG4,DEFAULT);

    VoC_shr32_rr(RL6,REG7,DEFAULT);
    VoC_shr32_rr(RL7,REG7,IN_PARALLEL);

    // L_Num in ACC1
    VoC_add32_rr(ACC1,RL7,RL6,DEFAULT);

    // pppL_B & pppL_F
    VoC_lw32_sd(REG45,1,IN_PARALLEL);

    // &pppL_F[0][0][j_1] in REG5
    // &pppL_B[0][0][j_1] in REG4
    VoC_add16_rr(REG5,REG5,REG6,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);

    // pppL_F[NP - j - 1][NP - j - 1][j_1] un REG5
    // pppL_B[NP - j - 1][NP - j - 1][j_1] in REG6
    VoC_add16_rr(REG5,REG5,REG3,DEFAULT);
    VoC_add16_rr(REG6,REG4,REG3,IN_PARALLEL);

    // read before REG5 update
    exit_on_warnings=OFF;
    VoC_lw32_p(RL7,REG5,DEFAULT);
    exit_on_warnings=ON;
    VoC_lw32_p(RL6,REG4,DEFAULT);

    VoC_shr32_rr(RL6,REG7,DEFAULT);
    VoC_shr32_rr(RL7,REG7,IN_PARALLEL);

    // L_temp in ACC0
    VoC_add32_rr(ACC0,RL7,RL6,DEFAULT);

    // read
    VoC_lw32_p(RL7,REG5,IN_PARALLEL);
    VoC_lw32_p(RL6,REG6,DEFAULT);

    VoC_shr32_rr(RL7,REG7,IN_PARALLEL);
    VoC_shr32_rr(RL6,REG7,DEFAULT);

    // L_tmpA in RL7, L_sum in ACC0
    VoC_add32_rr(RL7,RL7,RL6,DEFAULT);
    VoC_add32_rr(ACC0,RL7,ACC0,DEFAULT);
    VoC_shr32_ri(ACC0,1,DEFAULT);

    // extract_h(L_sum)
    VoC_movreg16(REG2,ACC0_HI,DEFAULT);

    /**********************
    * norm_s_opt
    * input: REG2 (y);
    * return: REG0 (exp) in shr!!!
    * other used regs : none
    ***********************/

    // extract_h(L_sum) in REG2
    VoC_jal(CII_norm_s_opt);
    // swShift1 in REG0

    // general purpose constant in REG45
    VoC_lw32_d(REG45,CONST_0x00008000L_ADDR);

    // L_sum in ACC0 & L_Num in ACC1
    VoC_shr32_rr(ACC0,REG0,DEFAULT);
    VoC_shr32_rr(ACC1,REG0,IN_PARALLEL);

    // round : swNum in REG1, swDen in REG3
    VoC_add32_rr(REG01,REG45,ACC1,DEFAULT);
    VoC_add32_rr(REG23,REG45,ACC0,IN_PARALLEL);

    // round : swNum in REG0, swDen in REG2
    VoC_movreg16(REG2,REG3,DEFAULT);
    VoC_movreg16(REG0,REG1,IN_PARALLEL);

    // abs_s(swNum) in REG0
    VoC_bnltz16_r(EN_FLAT_opt_ABS_SWNUM,REG0);
    // negate swNum
    VoC_sub16_rr(REG0,REG5,REG0,DEFAULT);
    // swNum_is_negative in REG5
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
EN_FLAT_opt_ABS_SWNUM:

    // pswRc[j] in REG3
    VoC_lw16_sd(REG3,6,DEFAULT);
    // j in REG7
    VoC_lw16_sd(REG7,2,DEFAULT);

    // if (swDen <= 0) or if (abs_s(swNum) >= swDen)
    // but test 2 is stronger than test 1, because if 1 is respected, then 2 is also
    // so it can be replaced by simply
    // if (abs_s(swNum) >= swDen)
    // VoC_bngtz16_r(EN_FLAT_opt_NULL_DIV,REG2);
    // VoC_bngt16_rr(EN_FLAT_opt_NULL_DIV,REG2,REG0);

    VoC_bgt16_rr(EN_FLAT_opt_NOT_NULL_DIV,REG2,REG0);

    VoC_sub16_dr(REG7,CONST_10_ADDR,REG7);
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_loop_r_short(REG7,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    VoC_endloop0;

    VoC_jump(EN_FLAT_opt_END);

EN_FLAT_opt_NOT_NULL_DIV:

    // else make division

    // INPUT :
    // REG0 : Nominator (param0)
    // REG1 : Denominator (param1)
    // OUTPUT
    // REG2 : return
    //
    // USED REGS : RL6, RL7

    // Den in REG1
    VoC_movreg16(REG1,REG2,DEFAULT);
    VoC_jal(CII_DIV_S);
    // return in REG2

    // if swNum_is_negative == 0, negate result
    VoC_bnez16_r(EN_FLAT_opt_NEG_DIV,REG5);
    VoC_sub16_rr(REG2,REG5,REG2,DEFAULT);
EN_FLAT_opt_NEG_DIV:

//          VoC_NOP();
    // if (j != NP - 1)
    VoC_lw16i_short(REG5,9,DEFAULT);    // stephen 060711
    // pswRc[j] <= result
    VoC_sw16inc_p(REG2,REG3,DEFAULT);

    // &pswRc[++j] on the stack
    VoC_sw16_sd(REG3,6,DEFAULT);


    // STACK16                 STACK32
    // --------------------------------------
    // RA                      pppL_B & pppL_F
    // &pswRc[j]               L_R0
    // pppL_C
    // *pswVadScalAuto
    // swShift
    // j
    // *piR0Inx
    // 2*j_1


    // VoC_jump(EN_FLAT_opt_DONE_DIV);

// EN_FLAT_opt_DONE_DIV:


    VoC_be16_rr(EN_FLAT_opt_END,REG7,REG5);

    /* The code is reorganized in the following way :

    // funct CII_flat_recursion_opt() :

    L_Cik = L_shl(pppL_C[i][k++][j_1], swShift);
    L_Cki = L_shl(pppL_C[k++][i][j_1], swShift);
    L_Bik = L_shl(pppL_B[i][k++][j_1], swShift);
    L_Fik = L_shl(pppL_F[i][k++][j_1], swShift);
    L_TmpB = L_add(L_Bik, L_Fik);
    L_TmpB = L_mpy_ls(L_TmpB, pswRc[j]);
    L_TmpA = L_add(L_Cik, L_Cki);
    L_TmpA = L_mpy_ls(L_TmpA, pswRc[j]);

    // recursion :

    CII_flat_recursion_opt();

    for (k = 0; k <= NP - j - 2; k++)
    {
      L_temp = L_mpy_ls(L_Bik, swRcSq);
      L_temp = L_add(L_temp, L_Fik);
      pppL_F[0][k][j_0] = L_add(L_temp, L_TmpA);

      CII_flat_recursion_opt();

      L_temp = L_mpy_ls(L_Cki, swRcSq);
      L_temp = L_add(L_temp, L_Cik);
      pppL_C[0][k][j_0] = L_add(L_temp, L_TmpB);
    }

        for (i = 1; i <= NP - j - 2; i++)
    {

          for (k = i; k <= NP - j - 2; k++)
          {
        CII_flat_recursion_opt();

            L_temp = L_mpy_ls(L_Bik, swRcSq);
            L_temp = L_add(L_temp, L_Fik);
            pppL_F[i][k][j_0] = L_add(L_temp, L_TmpA);

            L_temp = L_mpy_ls(L_Cik, swRcSq);
            L_temp = L_add(L_temp, L_Cki);
            pppL_C[k][i - 1][j_0] = L_add(L_temp, L_TmpB);

            L_temp = L_mpy_ls(L_Fik, swRcSq);
            L_temp = L_add(L_temp, L_Bik);
            pppL_B[i - 1][k - 1][j_0] = L_add(L_temp, L_TmpA);

            L_temp = L_mpy_ls(L_Cki, swRcSq);
            L_temp = L_add(L_temp, L_Cik);
            pppL_C[i][k - 1][j_0] = L_add(L_temp, L_TmpB);
          }

          CII_flat_recursion_opt();

          L_temp = L_mpy_ls(L_Fik, swRcSq);
          L_temp = L_add(L_temp, L_Bik);
          pppL_B[i - 1][NP - j - 2][j_0] = L_add(L_temp, L_TmpA);

          L_temp = L_mpy_ls(L_Cki, swRcSq);
          L_temp = L_add(L_temp, L_Cik);
          pppL_C[i][NP - j - 2][j_0] = L_add(L_temp, L_TmpB);

    }

        CII_flat_recursion_opt();

        L_temp = L_mpy_ls(L_Fik, swRcSq);
        L_temp = L_add(L_temp, L_Bik);
        pppL_B[NP - j - 2][NP - j - 2][j_0] = L_add(L_temp, L_TmpA); */



    // REG2 : pswRc[j]
    // REG7 : j

    // make room for L_Cik, L_Cki, L_Bik, L_Fik
    // push anything on the stack32;
    // prepare increments

    VoC_push32(ACC0,DEFAULT);
    VoC_lw16i_short(INC0,4,IN_PARALLEL);
    VoC_push32(ACC0,DEFAULT);
    VoC_lw16i_short(INC1,40,IN_PARALLEL);
    VoC_push32(ACC0,DEFAULT);
    VoC_lw16i_short(INC2,4,IN_PARALLEL);
    VoC_push32(ACC0,DEFAULT);
    VoC_lw16i_short(INC3,4,IN_PARALLEL);

    // NP - j - 1 in REG5 (NP - 1 is in REG5)
    // pswRc[j] in REG4
    VoC_sub16_rr(REG5,REG5,REG7,DEFAULT);



    // swRcSq = mult_r(pswRc[j], pswRc[j]) in REG7
    VoC_multf32_rr(REG67,REG2,REG2,DEFAULT);
    VoC_movreg16(REG4,REG2,DEFAULT);

    VoC_add32_rd(REG67,REG67,CONST_0x00008000L_ADDR);


    // prepare pointers with j_1 = 1 - j_0
    VoC_lw16_sd(REG6,0,DEFAULT);
    VoC_lw32_sd(REG23,5,IN_PARALLEL);

    // push pswRc[j] & swRcSq
    VoC_push16(REG4,DEFAULT);
    VoC_push16(REG7,DEFAULT);


    // STACK16                 STACK32
    // --------------------------------------
    // RA                      pppL_B & pppL_F
    // &pswRc[j]               L_R0
    // pppL_C                  L_Cik
    // *pswVadScalAuto         L_Cki
    // swShift                 L_Bik
    // j                       L_Fik
    // *piR0Inx
    // 2*j_1
    // pswRc[j]
    // swRcSq


    VoC_lw16_sd(REG0,7,DEFAULT);



    VoC_add16_rr(REG2,REG2,REG6,IN_PARALLEL);
    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);

    VoC_add16_rr(REG1,REG0,REG6,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG6,IN_PARALLEL);

    // push NP - j - 1
    VoC_push16(REG5,DEFAULT);

    // REG5 2-4*j_1
    VoC_lw16i_short(REG5,2,DEFAULT);
    VoC_bez16_r(EN_FLAT_opt_j_1_is_ZERO,REG6)
    VoC_lw16i_short(REG5,-2,DEFAULT);

EN_FLAT_opt_j_1_is_ZERO:


    // push pppL_C & pppL_C j_1
    VoC_push32(REG01,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);

    // push pppL_B & pppL_F j_1
    VoC_push32(REG23,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG5,IN_PARALLEL);

    VoC_add16_rr(REG2,REG2,REG5,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG5,IN_PARALLEL);

    // push pppL_C & pppL_C j_0
    VoC_push32(REG01,DEFAULT);

    // push pppL_B & pppL_F j_0
    VoC_push32(REG23,DEFAULT);

    // push 2 - 4*j_1
    VoC_push16(REG5,IN_PARALLEL);

    // STACK16                 STACK32
    // --------------------------------------
    // RA                      pppL_B & pppL_F
    // &pswRc[j]               L_R0
    // pppL_C                  L_Cik
    // *pswVadScalAuto         L_Cki
    // swShift                 L_Bik
    // j                       L_Fik
    // *piR0Inx                pppL_C & pppL_C j_1
    // 2*j_1                   pppL_B & pppL_F j_1
    // pswRc[j]                pppL_C & pppL_C j_0
    // swRcSq                  pppL_B & pppL_F j_0
    // NP - j - 1
    // 2 - 4*j_1


    /****************************/
    // CII_flat_recursion_opt
    // input :
    //  REG0 : pppL_C(i,k), INC0 = 4
    //  REG1 : pppL_C(k,i), INC1 = 40
    //  REG2 : pppL_B(i,k), INC2 = 4
    //  REG3 : pppL_F(i,k), INC3 = 4
    //  REG4 : pswRc[j]
    //  REG6 : swShift
    //  output:
    //      ACC1 : L_TmpA
    //      RL6 :  L_TmpB
    //  stack32 :
    //      +3 : L_Cik
    //      +2 : L_Cki
    //      +1 : L_Bik
    //      +0 : L_Fik
    //  other used :
    //  ACC0, RL7
    //  unused :
    //      REG7
    //  not modified :
    //      REG4, REG6
    /****************************/

    /****************************/
    // CII_L_mpy_ls_opt
    // input ACC0, REG4
    // output ACC0
    // other used register REG5
    // not modified : REG4
    /****************************/


    VoC_lw16_sd(REG6,7,DEFAULT);
    VoC_jal(CII_flat_recursion_opt);


    VoC_lw16_sd(REG5,1,DEFAULT);
    VoC_loop_r_short(REG5,DEFAULT);
    VoC_startloop0

    // L_temp = L_mpy_ls(L_Bik, swRcSq);
    VoC_lw32_sd(ACC0,5,DEFAULT);
    VoC_lw16_sd(REG4,2,DEFAULT);
    VoC_jal(CII_L_mpy_ls_opt);

    // L_temp = L_add(L_temp, L_Fik);
    VoC_lw32_sd(RL7,4,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);

    // pppL_F[0][k][j_0] = L_add(L_temp, L_TmpA);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw16_sd(REG4,3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);

    VoC_jal(CII_flat_recursion_opt);

    // L_temp = L_mpy_ls(L_Cki, swRcSq);
    VoC_lw32_sd(ACC0,6,DEFAULT);
    VoC_lw16_sd(REG4,2,DEFAULT);
    VoC_jal(CII_L_mpy_ls_opt);

    // L_temp = L_add(L_temp, L_Cik);
    VoC_lw32_sd(RL7,7,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);

    // pppL_C[0][k][j_0] = L_add(L_temp, L_TmpB);
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_NOP();
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

    VoC_endloop0;

    // STACK16                 STACK32
    // --------------------------------------
    // RA                      pppL_B & pppL_F
    // &pswRc[j]               L_R0
    // pppL_C                  L_Cik
    // *pswVadScalAuto         L_Cki
    // swShift                 L_Bik
    // j                       L_Fik
    // *piR0Inx                pppL_C & pppL_C j_1
    // 2*j_1                   pppL_B & pppL_F j_1
    // pswRc[j]                pppL_C & pppL_C j_0
    // swRcSq                  pppL_B & pppL_F j_0
    // NP - j - 1
    // 2 - 4*j_1



    // NP - j - 2, i = 1 in REG7
    VoC_lw16_sd(REG5,1,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);
    VoC_sub16_rr(REG5,REG5,REG7,DEFAULT);

    VoC_bngtz16_r(LOOP_END_LABEL_FLAT,REG5);

    // for (i = 1; i <= NP - j - 2; i++)
    VoC_loop_r(1,REG5)


    // at this stage, pointers should be :
    //
    // &pppL_C(i,k)[         i][       i-1][j_0]
    // &pppL_C(k,i)[         i][       i-1][j_0]
    // &pppL_B(i,k)[       i-1][       i-1][j_0]
    // &pppL_F(i,k)[         i][         i][j_0]
    //
    // &pppL_C(i,k)[         i][         i][j_1]
    // &pppL_C(k,i)[         i][         i][j_1]
    // &pppL_B(i,k)[         i][         i][j_1]
    // &pppL_F(i,k)[         i][         i][j_1]


    // prepare the following constants:
    // 44*i + 2*j_1                   -> 44*i + 2*j_1
    // 44*i + 2*j_1 + 2 - 4*j_1       -> 2 - 4*j_1
    // 44*i + 2*j_1 + 2 - 4*j_1 - 4   -> 2 - 4*j_1 - 4
    // 44*i + 2*j_1 + 2 - 4*j_1 - 44  -> 2 - 4*j_1 - 44


    // i in REG7, 44 in REG6,
    // j_1 in REG5, rel addr in REG4

    VoC_lw16_sd(REG5,4,DEFAULT);
    VoC_lw16i(REG6,44);
    // get pointers & rel address
    VoC_lw16_sd(REG0,9,DEFAULT);
    VoC_mult16_rr(REG4,REG6,REG7,IN_PARALLEL);
    VoC_lw32_sd(REG23,9,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);


    // pointers [j_1]
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_add16_rr(REG1,REG0,REG4,IN_PARALLEL);
    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG4,IN_PARALLEL);

    VoC_sw32_sd(REG01,3,DEFAULT);
    VoC_lw16_sd(REG4,0,IN_PARALLEL);

    VoC_sw32_sd(REG23,2,DEFAULT);
    VoC_lw16i_short(REG5,4,IN_PARALLEL);

    //  REG4 : 2 - 4*j_1
    //  REG5 : 2 - 4*j_1 - 4
    //  REG6 : 2 - 4*j_1 - 44
    VoC_sub16_rr(REG5,REG4,REG5,DEFAULT);
    VoC_sub16_rr(REG6,REG4,REG6,IN_PARALLEL);

    // pointers [j_0]
    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_add16_rr(REG1,REG0,REG5,IN_PARALLEL);
    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG4,IN_PARALLEL);

    VoC_sw32_sd(REG01,1,DEFAULT);
    VoC_lw16_sd(REG6,1,IN_PARALLEL);
    VoC_sw32_sd(REG23,0,DEFAULT);

    // NP - j - 1 - i
    VoC_sub16_rr(REG6,REG6,REG7,IN_PARALLEL);


    // STACK16                 STACK32
    // --------------------------------------
    // RA                      pppL_B & pppL_F
    // &pswRc[j]               L_R0
    // pppL_C                  L_Cik
    // *pswVadScalAuto         L_Cki
    // swShift                 L_Bik
    // j                       L_Fik
    // *piR0Inx                pppL_C & pppL_C j_1
    // 2*j_1                   pppL_B & pppL_F j_1
    // pswRc[j]                pppL_C & pppL_C j_0
    // swRcSq                  pppL_B & pppL_F j_0
    // NP - j - 1
    // 2 - 4*j_1


    // pswRc[j]
    VoC_lw16_sd(REG4,3,DEFAULT);

    // for (k = i; k <= NP - j - 2; k++)
    VoC_loop_r_short(REG6,DEFAULT);
    // swShift
    VoC_lw16_sd(REG6,7,IN_PARALLEL);
    VoC_startloop0

    VoC_jal(CII_flat_recursion_opt);

    // L_temp = L_mpy_ls(L_Bik, swRcSq);
    VoC_lw32_sd(ACC0,5,DEFAULT);
    VoC_lw16_sd(REG4,2,IN_PARALLEL);
    VoC_jal(CII_L_mpy_ls_opt);

    // L_temp = L_add(L_temp, L_Fik);
    VoC_lw32_sd(RL7,4,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);

    // pppL_F[i][k][j_0] = L_add(L_temp, L_TmpA);
    // L_Cik in ACC0
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw32_sd(ACC0,7,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    exit_on_warnings=ON;

    // L_temp = L_mpy_ls(L_Cik, swRcSq);
    VoC_jal(CII_L_mpy_ls_opt);

    // L_temp = L_add(L_temp, L_Cki);
    VoC_lw32_sd(RL7,6,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);

    // pppL_C[k][i - 1][j_0] = L_add(L_temp, L_TmpB);
    // L_Fik in ACC0
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_lw32_sd(ACC0,4,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    exit_on_warnings=ON;

    // L_temp = L_mpy_ls(L_Fik, swRcSq);
    VoC_jal(CII_L_mpy_ls_opt);

    // L_temp = L_add(L_temp, L_Bik);
    VoC_lw32_sd(RL7,5,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);

    // pppL_B[i-1][k-1][j_0] = L_add(L_temp, L_TmpA);
    // L_Cki in ACC0
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw32_sd(ACC0,6,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    exit_on_warnings=ON;

    // L_temp = L_mpy_ls(L_Cki, swRcSq);
    VoC_jal(CII_L_mpy_ls_opt);

    // L_temp = L_add(L_temp, L_Cik);
    VoC_lw32_sd(RL7,7,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);

    // pppL_C[i][k - 1][j_0] = L_add(L_temp, L_TmpB);
    // pswRc[j]
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_lw16_sd(REG4,3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

    VoC_endloop0;


    VoC_jal(CII_flat_recursion_opt);

    // L_temp = L_mpy_ls(L_Fik, swRcSq);
    VoC_lw32_sd(ACC0,4,DEFAULT);
    VoC_lw16_sd(REG4,2,IN_PARALLEL);
    VoC_jal(CII_L_mpy_ls_opt);

    // L_temp = L_add(L_temp, L_Bik);
    VoC_lw32_sd(RL7,5,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);

    // pppL_B[i-1][NP-j-2][j_0] = L_add(L_temp, L_TmpA);
    // L_Cki in ACC0
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw32_sd(ACC0,6,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    exit_on_warnings=ON;

    // L_temp = L_mpy_ls(L_Cki, swRcSq);
    VoC_jal(CII_L_mpy_ls_opt);

    // L_temp = L_add(L_temp, L_Cik);
    VoC_lw32_sd(RL7,7,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);

    // pppL_C[i][NP-j-2][j_0] = L_add(L_temp, L_TmpB);
    // pswRc[j]
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_lw16_sd(REG4,3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

    // i++
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);

    VoC_endloop1;

    VoC_jump(LOOP_END_LABEL_FLAT1);
// added when debugging

LOOP_END_LABEL_FLAT:
    // pswRc[j]
    VoC_lw16_sd(REG4,3,DEFAULT);
    VoC_lw16i(REG3,36);
    VoC_sub16_rr(REG2,REG2,REG3,DEFAULT);





LOOP_END_LABEL_FLAT1:

    VoC_lw32_sd(REG01,3,DEFAULT);
    VoC_lw16i(REG3,36);
    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG3,IN_PARALLEL);

    VoC_lw32_sd(REG01,2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32_sd(REG01,3,DEFAULT);
    exit_on_warnings=ON;

    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG3,IN_PARALLEL);

    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);

    VoC_sw32_sd(REG01,2,DEFAULT);



    VoC_jal(CII_flat_recursion_opt);


    // STACK16                 STACK32
    // --------------------------------------
    // RA                      pppL_B & pppL_F
    // &pswRc[j]               L_R0
    // pppL_C                  L_Cik
    // *pswVadScalAuto         L_Cki
    // swShift                 L_Bik
    // j                       L_Fik
    // *piR0Inx                pppL_C & pppL_C j_1
    // 2*j_1                   pppL_B & pppL_F j_1
    // pswRc[j]                pppL_C & pppL_C j_0
    // swRcSq                  pppL_B & pppL_F j_0
    // NP - j - 1
    // 2 - 4*j_1

    // finish recursion and clean stacks

    // clean ...
    VoC_pop16(REG7,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);

    // NP - j - 1 in REG7, clean ...
    VoC_pop16(REG7,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);

    // clean ...
    VoC_pop32(RL7,DEFAULT);
    VoC_pop32(RL7,DEFAULT);

    // STACK16                 STACK32
    // --------------------------------------
    // RA                      pppL_B & pppL_F
    // &pswRc[j]               L_R0
    // pppL_C                  L_Cik
    // *pswVadScalAuto         L_Cki
    // swShift                 L_Bik
    // j                       L_Fik
    // *piR0Inx
    // 2*j_1
    // pswRc[j]
    // swRcSq



    // L_temp = L_mpy_ls(L_Fik, swRcSq);
    VoC_pop32(ACC0,DEFAULT);
    VoC_pop16(REG4,IN_PARALLEL);
    VoC_jal(CII_L_mpy_ls_opt);

    // L_temp = L_add(L_temp, L_Bik);
    VoC_pop32(RL7,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);

    // pppL_B[i-1][NP-j-2][j_0] = L_add(L_temp, L_TmpA);
    // NP - j - 1 in REG7
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);

    // clean ...
    VoC_pop32(RL7,IN_PARALLEL);
    VoC_pop16(REG6,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG2,DEFAULT);


    // STACK16                 STACK32
    // --------------------------------------
    // RA                      pppL_B & pppL_F
    // &pswRc[j]               L_R0
    // pppL_C
    // *pswVadScalAuto
    // swShift
    // j
    // *piR0Inx
    // 2*j_1


    // OUF!!! End of recursion

    // 2*j_1 & pppL_B & pppL_F
    VoC_pop16(REG4,DEFAULT);
    VoC_lw32_sd(REG23,1,IN_PARALLEL);


    // STACK16                 STACK32
    // --------------------------------------
    // RA                      pppL_B & pppL_F
    // &pswRc[j]               L_R0
    // pppL_C
    // *pswVadScalAuto
    // swShift
    // j
    // *piR0Inx


    // 2 - 2*j_1, INC2 = INC3 = 44
    VoC_lw16i_short(REG5,2,DEFAULT);
    VoC_lw16i_short(INC2,44,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG5,REG4,DEFAULT);
    VoC_lw16i_short(INC3,44,IN_PARALLEL);
    // pppL_B[0][0][j_0]
    // pppL_F[0][0][j_0]
    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG4,IN_PARALLEL);

    VoC_lw32z(ACC0,DEFAULT);

    // L_TmpA = 0;
    VoC_loop_r_short(REG7,DEFAULT);
    VoC_lw32inc_p(RL6,REG2,IN_PARALLEL);
    VoC_startloop0

    // L_TmpA |= pppL_F[l][l][j_0];
    // L_TmpA |= pppL_B[l][l][j_0];
    VoC_or32_rr(ACC0,RL6,DEFAULT);
    VoC_lw32inc_p(RL7,REG3,IN_PARALLEL);

    VoC_or32_rr(ACC0,RL7,DEFAULT);
    VoC_lw32inc_p(RL6,REG2,IN_PARALLEL);

    VoC_endloop0

    // swShift = 0; j in REG6
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16_sd(REG6,1,IN_PARALLEL);

    // j++
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);

    VoC_bngtz32_r(EN_FLAT_opt_L_TmpA_NPOS,ACC0);

    /******************************
     *Function: CII_NORM_L_ACC0
     *input: ACC0;
     *return:REG1;
     ******************************/

    VoC_jal(CII_NORM_L_ACC0);
    VoC_sub16_dr(REG7,CONST_2_ADDR,REG1);


EN_FLAT_opt_L_TmpA_NPOS:

    // store back updated j & swShift
    VoC_sw16_sd(REG6,1,DEFAULT);
    VoC_sw16_sd(REG7,2,DEFAULT);

    VoC_lw16i_short(REG1,10,DEFAULT);



    // loop for (j = 0; j < NP; j++)
    VoC_bne16_rr(EN_FLAT_opt_LOOP_START,REG6,REG1);


    VoC_jump(EN_FLAT_opt_END_bis);

EN_FLAT_opt_END:


    // unpile stacks, load outputs

    // STACK16                 STACK32
    // --------------------------------------
    // RA                      pppL_B & pppL_F
    // pswRc                   L_R0
    // pppL_C
    // *pswVadScalAuto
    // swShift
    // j
    // *piR0Inx
    // 2*j_1

    VoC_pop16(REG0,DEFAULT);


EN_FLAT_opt_END_bis:


    // unpile stacks, load outputs

    // STACK16                 STACK32
    // --------------------------------------
    // RA                      pppL_B & pppL_F
    // pswRc                   L_R0
    // pppL_C
    // *pswVadScalAuto
    // swShift
    // j
    // *piR0Inx


    // ACC0 : L_R0
    // REG6 : *piR0Inx
    // REG7 : *pswVadScalAuto


    VoC_pop16(REG6,DEFAULT);

    // VoC_pop16(REG0,DEFAULT);
    // VoC_pop16(REG0,DEFAULT);
    // VoC_pop16(REG7,DEFAULT);

    VoC_loop_i_short(3,DEFAULT);
    VoC_startloop0
    VoC_pop16(REG7,DEFAULT);
    VoC_endloop0;

    VoC_pop16(REG0,DEFAULT);
    VoC_pop16(REG0,DEFAULT);
    VoC_pop32(ACC0,IN_PARALLEL);

    VoC_pop16(RA,DEFAULT);
    VoC_pop32(ACC1,DEFAULT);

    VoC_return;

}




/***********************************************
 Function: void CII_r0Quant_opt(void)
 input in REG67
 output in REG6
 used register 0167
 ***********************************************/

void CII_r0Quant_opt(void)
{
    //swUnqntzdR0 = round(L_UnqntzdR0);
    VoC_add32_rd(REG67,REG67,CONST_0x00008000L_ADDR);
    VoC_lw16i_set_inc(REG0,TABLE_SP_ROM_psrR0DecTbl_ADDR+1,2);

    //swR0Index in REG6
    //if (sub(swUnqntzdR0, psrR0DecTbl[2 * swR0Index + 1]) < 0)
    VoC_loop_i_short(31,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG1,REG0,DEFAULT);
    VoC_bgt16_rr(CII_r0Quant_opt_100,REG1,REG7);
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);
    VoC_endloop0
CII_r0Quant_opt_100:

    VoC_return;
}





/**************************************************************************
//argument pass

//  REG0  pswIncoef_Addr              inc0=-1
//  REG1  pswState                    inc1=1
//  REG2  hnwFilt_pswInSample_ADDR    inc2=1
//  REG3  hnwFilt_pswOutSample_ADDR   inc3=-1
//  REG4  hnwFilt_iStateOffset
//  REG5  swZeroState
//  REG6  hnwFilt_iNumSamples
**************************************************************************/

void CII_hnwFilt_opt(void)
{

    //  int i;
    //  FILE *fp=fopen("hnwFilt_s","w");

    // REGS[REG0].reg = 0x300;  REGS[REG0].incr_val=-1;
    // REGS[REG1].reg = 0x500;  REGS[REG1].incr_val=1;
    // REGS[REG2].reg = 0x100;  REGS[REG2].incr_val=1;
    // REGS[REG3].reg = 0x700;  REGS[REG3].incr_val=-1;

    // REGS[4].reg = 0xff8c;
    // REGS[5].reg = 0;
    // REGS[6].reg = 40;

    // register map

    //  REG4 i
    //  REG5 j
    //  REG6 iStatIndx

    //  most of the if and else part have the same code,
    //  branch is done only when there are difference,

#if 0

    voc_short   hnwFilt_swZeroState_ADDR                  ,x

#endif


    VoC_sw16_d(REG5,hnwFilt_swZeroState_ADDR);


    VoC_movreg16(REG7,REG6,DEFAULT);
    VoC_lw16i(REG5,39);
    VoC_add16_rr(REG6,REG4,REG5,DEFAULT); //  reg6 iStatIndx
    VoC_push16(REG0,IN_PARALLEL);

    VoC_add16_rr(REG3,REG3,REG5,DEFAULT); // reg3, pswoutsample_addr
    VoC_lw16i_short(REG4,0,IN_PARALLEL);  // i in reg4

    VoC_inc_p(REG1,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    VoC_bltz16_r(END_LOOP_ADDR_LABEL_ADDED0,REG7);
    VoC_loop_r(1,REG7)

    VoC_push16(REG3,DEFAULT);

    // L_temp = L_mac((long) 16384, pswInSample[S_LEN - i - 1], 0x4000);
    VoC_add16_rd(REG7,REG2,CONST_39_ADDR);

    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_lw16i_short(REG3,5,IN_PARALLEL);//j in reg5

    VoC_lw32_d(ACC0,CONST_0x00004000L_ADDR);
    VoC_mac32_pd(REG7,CONST_0x4000_ADDR);//L_temp in ACC0

    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_sub16_rr(REG7,REG6,REG4,IN_PARALLEL);

    VoC_add16_rr(REG7,REG7,REG3,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG3,IN_PARALLEL);

    // iStatIndx - i + j in reg7
    // for (j = 5; (j >= 0) && (iStatIndx - i + j >= 0); j--)
    //      is equivalent to:
    // for (j=5; j>=0; j--) if (iStatIndx - i + j <= 0) goto OUT_LOOP;
    VoC_add16_rr(REG3,REG2,REG7,DEFAULT);

    //  optimised for speed, put branch outside the loop
    VoC_bnez16_d(hnwFilt06,hnwFilt_swZeroState_ADDR);

    VoC_loop_i_short(6,DEFAULT);
    // VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_startloop0;
    //  reg6 is pre-assigned to pswState[iStatIndx1 - i + j]
    //  if iStatIndx1 - i + j>=0,
    //      REG7 will be assigned to  pswInSample[iStatIndx - i + j]
    VoC_bltz16_r(hnwFilt06_bis,REG7);

    //  common part
    VoC_lw16_p(REG3,REG3,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG5,IN_PARALLEL);

    VoC_mac32inc_rp(REG3,REG0,DEFAULT);
    VoC_add16_rr(REG3,REG2,REG7,IN_PARALLEL);

    VoC_endloop0;
    VoC_jump(hnwFilt06_bis);

hnwFilt06:

    VoC_loop_i_short(6,DEFAULT);
    // VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_startloop0;
    //  REG6 is pre-assigned to pswState[iStatIndx1 - i + j]
    //  if iStatIndx1 - i + j>=0,
    //  REG7 will be assigned to  pswInSample[iStatIndx - i + j]
    VoC_bnltz16_r(hnwFilt05,REG7);
    //  if swZeroState==1, the loop will continue to 6
    VoC_add16_rr(REG3,REG7,REG1,DEFAULT);
hnwFilt05:
    //  common part
    VoC_sub16_rr(REG7,REG7,REG5,DEFAULT);
    VoC_lw16_p(REG3,REG3,DEFAULT);

    VoC_mac32inc_rp(REG3,REG0,DEFAULT);
    VoC_add16_rr(REG3,REG2,REG7,IN_PARALLEL);

    VoC_endloop0;

hnwFilt06_bis:

    VoC_pop16(REG3,DEFAULT);
    //  the following part would be done only when swZeroState_ADDR==1
    VoC_shr32_ri(ACC0,-1,DEFAULT);

    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG3,DEFAULT);
    VoC_endloop1

END_LOOP_ADDR_LABEL_ADDED0:

    VoC_add16_rr(REG1,REG1,REG5,DEFAULT);
    VoC_pop16(REG0,DEFAULT);

    VoC_return;
}



/**********************************************
 CII_L_mpy_ll_opt
 input  : REG45 (1), REG67 (2)
 output : ACC0
 other used : RL7
*********************************************/


void CII_L_mpy_ll_opt(void)
{
    VoC_shru16_ri(REG6, 1, DEFAULT);
    VoC_shru16_ri(REG4, 1, IN_PARALLEL);

//  VoC_and16_rd(REG6, CONST_0x7FFF_ADDR);
//  VoC_and16_rd(REG4, CONST_0x7FFF_ADDR);

    VoC_multf32_rr(RL6, REG4, REG6, DEFAULT);
    VoC_multf32_rr(ACC0, REG7, REG4, DEFAULT);

    VoC_shr32_ri(RL6, 16, DEFAULT);
    VoC_shr32_ri(ACC0, 1, DEFAULT);

    VoC_multf32_rr(RL6, REG5, REG6, DEFAULT);
    exit_on_warnings = OFF;
    VoC_add32_rr(ACC0, ACC0, RL6, DEFAULT);
    exit_on_warnings = ON;
    VoC_shr32_ri(RL6, 1, DEFAULT);
    VoC_add32_rr(ACC0, ACC0, RL6, DEFAULT);

    VoC_mac32_rr(REG5, REG7, DEFAULT);
    exit_on_warnings = OFF;
    VoC_shr32_ri(ACC0, 14, IN_PARALLEL);
    exit_on_warnings = ON;

    VoC_return;
}



/****************************/
// input ACC0, REG4
// output ACC0
// other used register REG5
// not modified : REG4
/****************************/

void CII_L_mpy_ls_opt(void)
{
    VoC_movreg16(REG5,ACC0_LO,DEFAULT);
    VoC_shru16_ri(REG5,1,DEFAULT);
//  VoC_and16_rd(REG5,CONST_0x7fff_ADDR);

    VoC_multf32_rr(ACC0,REG5,REG4,DEFAULT);
    exit_on_warnings=OFF;
    VoC_movreg16(REG5,ACC0_HI,DEFAULT);
    exit_on_warnings=ON;
    // shift occurs before accumulate
    VoC_mac32_rr(REG5,REG4,DEFAULT);
    exit_on_warnings=OFF;
    VoC_shr32_ri(ACC0,15,IN_PARALLEL);
    exit_on_warnings=ON;

    VoC_return;
}




void CII_scaleExcite_opt(void)
{

    // IN:
    // REG7 scaleExcite_snsRS_ADDR
    // REG4 scaleExcite_swErrTerm
    // REG1 scaleExcite_pswScldVect_ADDR inc1=1
    // REG0 scaleExcite_pswVect_ADDR  inc0=1;
    // OUT:
    // REG3


    VoC_multf32_rp(RL6,REG4,REG7,DEFAULT);
    VoC_push16(RA,DEFAULT);
    VoC_movreg16(REG2,RL6_HI,DEFAULT);
    VoC_push16(REG0,IN_PARALLEL);

    VoC_jal(CII_norm_s_opt);

    VoC_shr32_rr(RL6,REG0,DEFAULT);//L_GainUs in RL6
    VoC_lw16i_short(REG3,1,IN_PARALLEL);

    VoC_add16_rr(REG7,REG7,REG3,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_lw32_d(RL7,CONST_0x00008000L_ADDR);

    VoC_sub16_pr(REG2,REG7,REG0,DEFAULT);//swGainShift = add(swGainUsShft, snsRS.sh);
    VoC_pop16(REG0,IN_PARALLEL);

    VoC_sub16_rr(REG2,REG2,REG3,DEFAULT);//swGainShift = sub(swGainShift, GSP0_SCALE);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);


    //  reg2 swGainShift
    //  reg3 swGain
    //  reg4 if (swGainshift<0) -swGainshift
    //       else               0

    //  if (swGainShift < 0) swGain = 0x7fff

    VoC_bnltz16_r(scaleExcite01,REG2);
    VoC_sub16_rr(REG4,REG4,REG2,DEFAULT);
    // VoC_lw16_d(REG3,CONST_0x7fff_ADDR);
    VoC_shr16_ri(REG3,-16,DEFAULT); // 0x7fff = sat(1 << 16)
    VoC_jump(scaleExcite_loop);

    //  if (swGainShift>=0)
    //      L_GainUs = L_shr(L_GainUs, swGainShift);
    //  the swGainShift==0 case is concluded as L_shr(L_GainUs,0)
    //  would not change the value of L_GainUs

scaleExcite01:
    VoC_shr32_rr(RL6,REG2,DEFAULT);
    VoC_add32_rr(REG23,RL6,RL7,DEFAULT);
    VoC_lw16i_short(REG2,0,DEFAULT);

    //  the left part is the same between if and else, if we put 0 in
    //  swGainshift when swGainshift>0

scaleExcite_loop:

    VoC_pop16(RA,DEFAULT);

    // REG4 = 0 for the case REG2 <= 0
    VoC_add32_rr(REG67,RL6,RL7,DEFAULT);
    VoC_shr32_rr(RL7,REG4,IN_PARALLEL);//L_Round_off = L_shl((long) 32768, swGainShift);

    VoC_multf32inc_rp(ACC0,REG7,REG0,DEFAULT);
    VoC_loop_i_short(40,DEFAULT);
    VoC_startloop0;
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);
    VoC_shr32_rr(ACC0,REG2,DEFAULT);
    VoC_multf32inc_rp(ACC0,REG7,REG0,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    exit_on_warnings = ON;

    VoC_endloop0;

    VoC_return;
}



/*****************************************
// INPUT
// REG6 : swRawLag
// OUTPUT
// REG2 : *pswCode
// REG3 : return
*******************************************/
void CII_quantLag_opt_paris(void)
{

    // REG0 : siOffset
    // REG1 : swIndex1
    // REG2 : swIndex2
    // REG3 : psrLagTbl[swIndex2]
    // REG4 : &psrLagTbl[0]
    // REG5 : &psrLagTbl[i]

    /*
       REGS[REG6].reg = 0x2a6;
       REGS[REG7].reg = 0;
    */


    VoC_lw16_d(REG2, CONST_128_ADDR);
    VoC_lw16i_short(REG1, 0, DEFAULT);
    // possible because of preshift
    VoC_movreg16(REG0, REG2, IN_PARALLEL);

    VoC_lw16i(REG4, TABLE_SP_ROM_psrLagTbl_ADDR);
    VoC_add16_rr(REG5, REG4, REG2, DEFAULT);

QUANTLAG103:
    // can replace while{} by do{}while
    // because the first condition is always true

    // can be moved here (preshift)
    VoC_shr16_ri(REG0, 1, DEFAULT);
    VoC_lw16_p(REG3, REG5, DEFAULT);
    VoC_bgt16_rr(QUANTLAG102, REG3, REG6);
    VoC_movreg16(REG1, REG2, DEFAULT);
QUANTLAG102:
    VoC_add16_rr(REG2, REG1, REG0, DEFAULT);
    VoC_add16_rr(REG5, REG4, REG2, DEFAULT);

    VoC_bne16_rr(QUANTLAG103, REG1, REG2);

    // return psrLagTbl[swIndex2] in REG3
    VoC_lw16_p(REG3, REG5, DEFAULT);
    VoC_return;
}


void CII_get_ipjj_opt_paris(void)
{

    // swRunningLag in REG4
    // return swTempJj in REG0
    // return swTempIp in REG1


    VoC_multf32_rd(REG01, REG4, CONST_0x1555_ADDR);
    VoC_NOP();
    VoC_shru16_ri(REG0, 1, DEFAULT);
//     VoC_and16_rd(REG0, CONST_0x7fff_ADDR);
    VoC_multf32_rd(ACC0, REG0, CONST_6_ADDR);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, CONST_0x00008000L_ADDR);
    VoC_movreg16(REG0, ACC0_HI, DEFAULT);

    VoC_bne16_rd(GETIPJJ100, REG0, CONST_6_ADDR);
    VoC_lw16i_short(REG0, 0, DEFAULT);
    VoC_add16_rd(REG1, REG1, CONST_1_ADDR);
GETIPJJ100:
    VoC_return;
}
/********************************************
// pswLTPState in REG5
// swOrigLagIn in REG6
*********************************************/


void CII_fp_ex_opt(void)
{

    // other registers:
    // REG7 : siSampsSoFar
    // REG6 :           siSampsThisPass
    // REG4 : swRunningLag
    // REG3 : -         &pswLTPState[siSampsSoFar]
    // REG2 :           &ppsrPVecIntFilt[0][swRemain]
    // REG1 : swIntLag      &pswLTPState[siSampsSoFar - swIntLag]
    // REG0 : swRemain      temp


    VoC_lw16i_short(REG7,0,DEFAULT); //siSampsSoFar
    VoC_movreg16(REG4,REG6,DEFAULT); // swRunningLag

fp_ex0:

    VoC_blt16_rd(fp_ex03_go,REG7,CONST_40_ADDR);
//  VoC_jump(fp_ex03);
    VoC_return
fp_ex03_go:
    VoC_push16(RA,DEFAULT);
    // swRunningLag in REG4
    // return swTempJj in REG0 (swRemain)
    // return swTempIp in REG1 (swIntLag)
    VoC_jal(CII_get_ipjj_opt_paris);

    VoC_push32(REG67,DEFAULT);
    VoC_pop16(RA,DEFAULT);


    // siSampsThisPass in REG6
    VoC_sub16_dr(REG6,CONST_40_ADDR,REG7);

    VoC_bnlt16_rd(fp_ex00,REG1,CONST_40_ADDR);
    VoC_sub16_rr(REG6,REG1,REG7,DEFAULT);
fp_ex00:

    // &pswLTPState[siSampsSoFar]
    VoC_add16_rr(REG3,REG7,REG5,DEFAULT);
    VoC_lw16i_short(INC3,1,DEFAULT);
    // &pswLTPState[siSampsSoFar - swIntLag]
    VoC_sub16_rr(REG1,REG3,REG1,DEFAULT);

    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_push16(REG6,DEFAULT);
    // swRemain no longer needed
    VoC_bngtz16_r(fp_ex_else,REG6);
    VoC_bnez16_r(fp_ex01,REG0);

    VoC_loop_r_short(REG6,DEFAULT)
    VoC_lw16inc_p(REG0,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG0,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG0,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;

    VoC_jump(fp_ex_else);

fp_ex01:
    VoC_lw16i_short(REG7,5,DEFAULT);
    // &ppsrPVecIntFilt[0][swRemain]
    // this works because ptr rd occurs before storing immediate value
    // and register rd occurs after storing immeditate value
    VoC_lw16i_set_inc(REG2,TABLE_SP_ROM_ppsrPVecIntFilt_ADDR,6);

    VoC_add16_rr(REG2,REG2,REG0,DEFAULT);


    VoC_loop_r(1,REG6)

    // &pswLTPState[siSampsSoFar - swIntLag - 5 + i]
    VoC_sub16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_push16(REG2,DEFAULT);

    VoC_lw32_d(ACC0,CONST_0x00008000L_ADDR);
    VoC_loop_i_short(10,DEFAULT)
    VoC_startloop0
    VoC_mac32inc_pp(REG1,REG2,DEFAULT);
    VoC_endloop0;

    // &pswLTPState[siSampsSoFar - swIntLag + i]
    VoC_sub16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_pop16(REG2,DEFAULT);

    VoC_inc_p(REG1,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG3,IN_PARALLEL);

    VoC_endloop1

fp_ex_else:

    VoC_pop16(REG1,DEFAULT);
    VoC_pop32(REG67,DEFAULT);

    VoC_add16_rr(REG7,REG7,REG1,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);

    VoC_bgtz16_r(fp_ex0,REG1);

    // VoC_jump(fp_ex0);
//fp_ex03:


    VoC_return

}


/**********************************************************
IN:
// REG2 : &pswCIn[0]             INC2 = 1;
// REG3 : &pswGIn[0],            INC3 = 1;
// REG6 : swGMax
// REG7 : swNum
OUT:
// REG1 : maxCCGIndex
// REG6 : pswGMax
// REG7 : pswCCMax
***********************************************************/


void CII_maxCCOverGWithSign_opt_paris(void)
{
    /*
        REGS[REG7].reg = 0x000c;
        REGS[REG2].reg = 0x0;         REGS[REG2].incr_val=1;
        REGS[REG3].reg = 0x10;        REGS[REG3].incr_val=1;
        REGS[REG4].reg = 0x21;
        REGS[REG5].reg = 0x20;
    */

    //i in REG0
    VoC_lw16i_set_inc(REG0,0,1);

    // we can put case i == 0 in the main loop
    // after this instr the input param in REG7 is no longer needed
    VoC_loop_r_short(REG7,DEFAULT)
    // parameters in REG4 & 5 are needed only at the end
    // we can push them and use REG45 in the function
    VoC_startloop0
    VoC_lw16inc_p(REG4, REG2, DEFAULT);
    VoC_lw16inc_p(REG5, REG3, DEFAULT);
    VoC_push32(REG23,DEFAULT);
    VoC_bez16_r(MAXCCOVERGVI103,REG0);
    VoC_bngtz16_r(MAXCCOVERGVI101, REG5);
MAXCCOVERGVI103:
    // icr already = 1
    VoC_multf32_rr(REG23, REG4, REG4, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(REG23, REG23, CONST_0x00008000L_ADDR);
    VoC_bnltz16_r(MAXCCOVERGVI100, REG4);
    VoC_sub16_dr(REG3,CONST_0_ADDR,REG3);
MAXCCOVERGVI100:

    VoC_multf32_rr(ACC0,REG3,REG6,DEFAULT);
    VoC_msu32_rr(REG5,REG7,DEFAULT);

    // if (i == 0) or (L_Temp > 0)
    VoC_bez16_r (MAXCCOVERGVI102,REG0);
    VoC_bgtz32_r (MAXCCOVERGVI102,ACC0);
    VoC_jump(MAXCCOVERGVI101);
MAXCCOVERGVI102:
    VoC_movreg16(REG7,REG3,DEFAULT);       //swCCMax
    VoC_movreg16(REG6,REG5,DEFAULT);       //swGmax
    VoC_movreg16(REG1,REG0,IN_PARALLEL);   //i

MAXCCOVERGVI101:
    VoC_pop32(REG23,DEFAULT);
    VoC_inc_p(REG0,DEFAULT);
    VoC_endloop0;



    // return maxCCGIndex in REG1
    VoC_return;
}





void   CII_v_con_opt(void)
{

    // INPUT :
    // REG0 : pswBVects,        INC0 = 40
    // REG1 : pswOutVect,       INC1 = 2
    // REG2 : pswBitArray,      INC2 = 1
    // REG4 : siNumBVctrs

    // OTHER:
    // REG3 : pswBitArray_bis,  INC3 = 1
    // REG5 : pswBVects_bis
    // REG6 : 2
    // RL6_LO : pswBitArray_start
    // RL6_HI : pswBitArray_start


    // RESTRICTIONS:
    // !!! pswBVects & pswBitArray need to be placed
    // in different memories. If necessary, one of the arrays
    // need to be copied, to accelerate the mac instruction

    // !!! pswBVects need to be aligned on even address
    // !!! pswOutVect need to be aligned on even address


    // SPEED : for siNumBVctrs = 9 :
    // The current version takes 306 cycles
    // The initial version takes 1165 cycles (3.8 times slower)


    // pswBitArray_bis in REG3
    VoC_movreg16(REG3,REG2,DEFAULT);
    VoC_lw16i_short(INC3,1,DEFAULT);
    // pswBitArray_start in RL6
    VoC_movreg32(RL6,REG23,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    // pswBVects_bis in REG5
    // 2 in REG6
    VoC_movreg16(REG5,REG0,DEFAULT);
    VoC_lw16i_short(REG6,2,IN_PARALLEL);

    // we will use the double-mac technique, which is possible on
    // the outer loop counter, because pswBitArray[siCVectCnt]
    // does not depend on siSampleCnt and
    // pswBVects[siCVectCnt * S_LEN + siSampleCnt] and
    // pswBVects[siCVectCnt * S_LEN + siSampleCnt + 1]
    // are consecutive

    VoC_loop_i(1,20)

    // set to 0
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw32z(ACC1,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG6,IN_PARALLEL);

    VoC_loop_r_short(REG4,DEFAULT)
    // increment pointer for next turn

    VoC_startloop0

    // we need to use two different pointers with
    // the same address for pswBitArray, because
    // the pointer is incremented between the two mac.
    // The _p(...REG2,DEFAULT) _p(...REG2,IN_PARALLEL)
    // sequence will be "understood" as a bi-fetch,
    // that is : *REG2 in DEFAULT and *REG2+1 in PARALLEL

    // REGS[REG0].incr_val = 1;
    // this mac operates on ACC0
    VoC_mac32inc_pp(REG2,REG0,DEFAULT);

    // REGS[REG0].incr_val = 39;
    // this mac operates on ACC1
    VoC_mac32inc_pp(REG3,REG0,IN_PARALLEL);

    VoC_endloop0

    // get to initial pointers
    VoC_movreg32(REG23,RL6,DEFAULT);

    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_shr32_ri(ACC1,-1,IN_PARALLEL);

    // get to initial pointers
    VoC_movreg16(REG0,REG5,DEFAULT);

    // pack together the two consecutive values
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    VoC_sw16inc_p(ACC1_HI,REG1,DEFAULT);

    VoC_endloop1

    VoC_return;
}

//////////////////////////////////////////////////////////////////
//IN:     reg0 &pswIn  INCR0=2
//INOUT:
//        ACC0 *pL_out
//OUT:    reg1
///////////////////////////////////////////////////////////////////

void  CII_g_corr1_opt_paris(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw32z(ACC0,DEFAULT);

    VoC_loop_i_short(20,DEFAULT);
    VoC_startloop0;
    VoC_bimac32inc_pp(REG0,REG0);
    VoC_endloop0;

//  L_sum in the NORM_L function is certainly >=0
    VoC_jal(CII_NORM_L_ACC0);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
//  VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
    VoC_return;
}

void CII_b_con_opt(void)
{
    // INPUT :
    // REG0 : pswVectOut, INC = 1
    // REG4 : siNumBits
    // REG5 : swCodeWord

    // REG2 : 0xc000
    // REG3 : 0x4000

    VoC_lw32_d(REG23,CONST_0x40000000L_ADDR);

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0

    // if (swCodeWord & 1)
    // pswVectOut[siLoopCnt] = 0x4000
    // else pswVectOut[siLoopCnt] = 0xc000

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_and16_rr(REG6,REG5,DEFAULT);

    VoC_movreg16(REG7,REG3,IN_PARALLEL);
    VoC_bnez16_r(b_con01,REG6);
    VoC_sub16_rr(REG7,REG2,REG3,DEFAULT);
b_con01:

    VoC_shr16_ri(REG5,1,DEFAULT);
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    VoC_endloop0

    VoC_return;

}

/************************************************************
 * norm_s
 *paras: REG2 (y);
 *return: REG0 (exp) in shr!!!
 ************************************************************/

void CII_norm_s_opt(void)
{

    VoC_lw16i_set_inc(REG0,0,-1);
    VoC_bez16_r(NORM_S_RETURN,REG2);
    VoC_loop_i_short(15,DEFAULT)
    VoC_startloop0
    VoC_bnlt16_rd(NORM_S_RETURN,REG2,CONST_0x4000_ADDR)
    VoC_shr16_ri(REG2,-1,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);
    VoC_endloop0
NORM_S_RETURN:
    VoC_return;
}





/********************************************
 Function: void CII_sqroot(void)
 input in RL7
 output in REG7
 used register ACC0, REG6, REG5
*******************************************/
void CII_sqroot_opt(void)
{

    VoC_shr32_ri(RL7, 1, DEFAULT);
    // L_shr(L_SqrtIn, 1);
    VoC_sub32_rd(REG67, RL7, CONST_0x40000000L_ADDR);
    // swTemp in REG7
    VoC_add32_rd(RL7, RL7, CONST_0x40000000L_ADDR);
    // L_temp1 in RL7


    // swTemp in REG6
    VoC_loop_i_short(2,DEFAULT);
    VoC_movreg16(REG6,REG7,IN_PARALLEL);
    VoC_startloop0;
    VoC_lw32z(ACC0, DEFAULT);
    VoC_msu32_rr(REG7, REG6, DEFAULT);
    VoC_movreg16(REG5,REG6,DEFAULT);
    //reg5:first  loop  swTemp
    //     second loop swTemp2
    //L_Temp0 in ACC0
    VoC_movreg16(REG6, ACC0_HI, DEFAULT);
    //reg6:first  loop swTemp2
    //     second loop swTemp3
    VoC_shr32_ri(ACC0, 1, IN_PARALLEL);
    //ACC0:first  loop L_Temp0
    //     second loop L_Temp1
    VoC_add32_rr(RL7, ACC0, RL7, DEFAULT);
    //RL7: first  loop L_Temp0
    //     second loop L_Temp1
    VoC_endloop0;


    VoC_multf32_rr(ACC0, REG6, REG7, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(ACC0, ACC0, CONST_0x00008000L_ADDR);
    VoC_msu32_rr(REG7,REG6,DEFAULT);
    // ACC0 = 0x00008000L after 2 cycles
    exit_on_warnings = OFF;
    VoC_movreg16(REG7, ACC0_HI, DEFAULT);//swTemp4 in REG7
    exit_on_warnings = ON;

    VoC_msu32_rr(REG6, REG5, DEFAULT);
    VoC_mac32_rr(REG6, REG5, DEFAULT);
    exit_on_warnings = OFF;
    // ACC0 = 0x00008000L after 2 cycles
    VoC_movreg16(REG6, ACC0_HI, DEFAULT);//swTemp2 in REG6
    exit_on_warnings = ON;
// L_Temp0 = L_msu(0L, TERM5_MULTIPLER, swTemp4);
// L_Temp1 = L_add(L_Temp0, L_Temp1);
// swSqrtOut = mac_r(L_Temp1, TERM6_MULTIPLER, swTemp2);
    VoC_add32_rr(ACC0, RL7, ACC0,DEFAULT);
    VoC_msu32_rd(REG7, CONST_0x5000_ADDR);//L_Temp0 in ACC0
    VoC_multf32_rd(RL7, REG6, CONST_0x7000_ADDR);
    VoC_NOP();
    VoC_add32_rr(REG67, RL7, ACC0, DEFAULT);

    VoC_return;
}

void CII_sub_rs_rrNs(void)
{
    VoC_push16(RA,DEFAULT);
    // it is important to have an even short
    // number of instructions between two jal,
    // otherwise the compiler will add a NOP

    VoC_movreg16(REG0,REG1,DEFAULT);
    VoC_and16_rd(REG0,CONST_1_ADDR);

    VoC_movreg32(RL7,ACC0,DEFAULT);
    VoC_movreg32(ACC1,REG01,IN_PARALLEL);

    // input ACC1 (RL7 & ACC0 unused)
    VoC_jal(CII_sqroot_opt);
    // swTemp in reg7

    // swNormShift IN REG5
    // swNormShift & 1 in REG4
    VoC_movreg32(REG45,ACC1,DEFAULT);

    // else L_Temp = L_deposit_h(swTemp);
    VoC_lw16i_short(REG6,0,DEFAULT);

    // if (swNormShift & 1)
    VoC_bez16_r(rs_rrNs00,REG4);
    VoC_sub16_rd(REG5,REG5,CONST_1_ADDR);
    VoC_multf32_rd(REG67,REG7,CONST_0x5a82_ADDR);
rs_rrNs00:

    // pop it here because regs are not used
    // in next fct and to ensure even short
    // number of instructions between two jal
    VoC_pop32(REG23,DEFAULT);
    VoC_movreg32(ACC0,REG67,DEFAULT);
    // input ACC0
    VoC_jal(CII_NORM_L_ACC0);
    // swNormShift in reg5
    VoC_shr16_ri(REG5,1,DEFAULT);
    // output REG1
    VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);


    VoC_sub16_rr(REG5,REG5,REG1,DEFAULT);

    // L_Temp = L_shl(L_Temp, swShift);
    VoC_shr32_rr(REG67,REG1,IN_PARALLEL);

    // round(L_Temp) in REG1*
    VoC_add32_rd(REG01,REG67,CONST_0x00008000L_ADDR);

    // shr(snsSqrtRs.man,1) in reg0
    VoC_lw16inc_p(REG0,REG2,DEFAULT);
    VoC_shr16_ri(REG0,1,DEFAULT);

    // even short number of instructions
    // between two jal and free REG2
    // snsSqrtRs.sh in reg6
    VoC_lw16_p(REG6,REG2,DEFAULT);

    /*********************/
    // input in REG0, REG1
    // output in  REG2
    // used register RL6, RL7
    /*********************/

    VoC_jal(CII_DIV_S);
    // output in REG2

    /**********************
    * norm_s
    * input: REG2 (y);
    * return: REG0 (exp) in shr!!!
    * other used regs : none
    ***********************/

    // swTemp2 in REG2
    VoC_jal(CII_norm_s_opt);
    // output in REG0

    // swNormShift = sub(snsSqrtRs.sh, swNormShift);
    VoC_sub16_rr(REG5,REG6,REG5,DEFAULT);

    // sub(swNormShift, 1);
    VoC_sub16_rd(REG5,REG5,CONST_1_ADDR);

    VoC_sub16_rr(REG5,REG5,REG0,DEFAULT);
    VoC_pop16(RA,IN_PARALLEL);

    // shl(swTemp2, swShift) is not necessary
    // because it is already done by CII_norm_s
    // VoC_shr16_rr(REG2,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG2,REG3,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    exit_on_warnings = OFF;
    VoC_return;
}


//////////////////////////////////////////////////////////////////
// INPUT :
// REG0 : pswExcitation,    INC0 = 2
// REG2 : &snsSqrtRs,       INC2 = 1
// REG3 : &snsSqrtRsRr;     INC3 = 1
//////////////////////////////////////////////////////////////////

void CII_rs_rrNs_opt(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_push32(REG23,IN_PARALLEL);

    // input REG0, INC0 = 2
    VoC_jal(CII_g_corr1_opt_paris);
    // return swNormShift in reg1
    // return L_temp in ACC0

    VoC_jal(CII_sub_rs_rrNs);

    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_return;
}



/***************************************************
INPUT:
      REG3 : pswFiltOut
      REG5 : pswInput
      REG6 : pswCoef
OUTPUT:
      REG3 : pswFiltOut
***************************************************/

void CII_lpcZsIir_opt(void)
{


#if 0

    voc_short   lpcZsIir_copy_ADDR                  ,80,y

#endif




    VoC_movreg16(REG2,REG6,DEFAULT);
    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_lw16i_set_inc(REG1,lpcZsIir_copy_ADDR,2);

    VoC_push32(RL7,DEFAULT);

    VoC_loop_i_short(20,DEFAULT);
    VoC_lw32inc_p(RL7,REG2,IN_PARALLEL);
    VoC_startloop0;
    VoC_lw32inc_p(RL7,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(RL7,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;



    VoC_push16(REG7,DEFAULT);
    // &pswInput[0] in REG0
    VoC_movreg16(REG0,REG5,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    // &pswFiltOut[0] in REG1
    VoC_movreg16(REG1,REG3,DEFAULT);
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);
    // pswInput[0]
    VoC_lw16inc_p(REG5,REG0,DEFAULT);

    // &_pswCoef[0] in REG2

    VoC_lw16i_set_inc(REG2,lpcZsIir_copy_ADDR,1);

    // -> pswFiltOut[0]
    VoC_sw16_p(REG5,REG1,DEFAULT);

    // init siSmp
    VoC_lw16i_short(REG6,1,IN_PARALLEL);


    VoC_lw32_d(RL7,CONST_0x00000800L_ADDR);
    VoC_movreg32(ACC0,RL7,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);

    // only the lo (8000) will be loaded)
    VoC_lw16i(REG7,0x8000);

    VoC_loop_i(1,10)

    // Sistage is < min(siSmp,10)
    // compute MIN in REG5


    VoC_loop_r_short(REG6,DEFAULT);
    VoC_startloop0
    VoC_msu32inc_pp(REG2,REG1,DEFAULT);
    VoC_endloop0

    VoC_sub16_rr(REG2,REG2,REG6,DEFAULT);
    // compute &pswFiltOut[siSmp]
    VoC_movreg16(REG1,REG3,IN_PARALLEL);


    // added: restitute reg2 pswCoef[0]
    VoC_msu32inc_rp(REG7,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_shr32_ri(ACC0,-4,IN_PARALLEL);
    exit_on_warnings=ON;


    VoC_be16_rd(lpcZsIir_NP_IS_MIN,REG6,CONST_10_ADDR);

    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);

    // sw and msu should be separated by 2 cycles
    // here take advantage of branch instruction which take 1 cycle when condtion is false
    // and 2 cycle when condition is true
    VoC_sw16inc_p(ACC0_HI,REG3,DEFAULT);
    VoC_movreg32(ACC0,RL7,IN_PARALLEL);

    VoC_endloop1

lpcZsIir_NP_IS_MIN:

    VoC_lw16i_short(REG6,9,DEFAULT);


    VoC_lw16i_short(INC2,-2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_sub16_rr(REG1,REG1,REG6,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG6,IN_PARALLEL);

    VoC_loop_i(1,29)

    // Sistage is < min(siSmp,10)
    // compute MIN in REG5

    VoC_push16(REG2,DEFAULT);

    VoC_aligninc_pp(REG1,REG2,DEFAULT);
    // sw and msu should be separated by 2 cycles
    // here take advantage of branch instruction which take 1 cycle when condtion is false
    // and 2 cycle when condition is true
    VoC_movreg32(ACC0,RL7,IN_PARALLEL);

    VoC_loop_i_short(5,DEFAULT)
    exit_on_warnings = OFF;
    VoC_sw16inc_p(ACC0_HI,REG3,IN_PARALLEL);
    exit_on_warnings = ON;
    VoC_startloop0
    VoC_bimsu32inc_pp(REG2,REG1);
    VoC_endloop0;


    VoC_pop16(REG2,DEFAULT);
    // compute &pswFiltOut[siSmp]
    VoC_sub16_rr(REG1,REG3,REG6,IN_PARALLEL);

    // added: restitute reg2 pswCoef[0]
    VoC_msu32inc_rp(REG7,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_shr32_ri(ACC0,-4,IN_PARALLEL);
    exit_on_warnings=ON;


    VoC_endloop1


    VoC_pop16(REG7,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);


    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    // sw and msu should be separated by 2 cycles
    // here take advantage of branch instruction which take 1 cycle when condtion is false
    // and 2 cycle when condition is true
    VoC_sw16inc_p(ACC0_HI,REG3,DEFAULT);



    VoC_return;

}


/****************************************************************
// CII_v_srch:
// INPUT:
// REG4 : siNumBasis
// REG0 : pswWBasisVecs, INC0 = 1
// REG1 : pswWInput,     INC1 = 1
// OUTPUT :
// REG0 : sw1
// USED:
// all register
*****************************************************************/

void CII_v_srch_opt(void)//return in reg2
{

#if 0

    voc_short   LOCAL_v_srch_pswCGUpdates_ADDR                ,144,x        // 144
    voc_short   LOCAL_v_srch_pswBits_ADDR                     ,10,y        // 8
    voc_short   LOCAL_v_srch_pswWBasisVecs_copy_ADDR          ,360,y        // 360
    voc_short   LOCAL_v_srch_pswDSpace_ADDR                   ,144,y        // 144
    voc_short   LOCAL_v_srch_pL_R_ADDR                        ,18,y        // 9 long
    voc_short   LOCAL_v_srch_siBest_ADDR                      ,y
    voc_short   LOCAL_v_srch_pswModNextBit_ADDR_ADDR          ,y
    voc_short   LOCAL_v_srch_pswBits_ADDR_ADDR                ,y

#endif



    VoC_push16(RA,DEFAULT);
    // initialize variables based on voicing mode ...

    // In fact we do not need to decide for the vocoder
    // variables here, because they depend only on the
    // input parameter siNumBasis, and are used only in
    // section 4. siNumBasis is stored in REG4 and is not
    // erased until the end of the program. This will
    // prevent us from storing these variables into memory.

    // start with 32-bit to ensure function starts on even address
    VoC_sub16_rd(REG5,REG4,CONST_1_ADDR);

    // during code writing, we can see that all these pointers of
    // different types point to the same memory space, defined by
    // pswDSpace, and in any case, we will directly use
    // &pswDSpace[i] for any of the accesses, so this initialisation
    // is useless, and we remove it

    //VoC_lw16i_set_inc(REG0,LOCAL_v_srch_pppswDubD_ADDR,1);
    //VoC_lw16i_set_inc(REG1,LOCAL_v_srch_ppswD_ADDR,1);
    //VoC_lw16i_set_inc(REG2,LOCAL_v_srch_ppswDPSpace_ADDR,1);
    //VoC_lw16i_set_inc(REG3,LOCAL_v_srch_pswDSpace_ADDR,2);

    //VoC_loop_r_short(REG5,DEFAULT)
    //VoC_movreg16(REG6,REG3,IN_PARALLEL);
    //VoC_startloop0

    //address of ppswDPSpace[siI * siNumBasis] in REG7
    //VoC_sw16inc_p(REG2,REG0,DEFAULT);

    //VoC_loop_r(1, REG4)
    //VoC_sw16inc_p(REG3,REG2,DEFAULT);
    //VoC_inc_p(REG3,IN_PARALLEL);
    //VoC_endloop1;

    //VoC_sw16inc_p(REG6,REG1,DEFAULT);
    //VoC_add16_rd(REG6,REG6,REG4,IN_PARALLEL);

    //VoC_endloop0;


// 1

    // save RA and &pswWBasisVecs[0]

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_push16(REG0,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_lw16i_set_inc(REG2,LOCAL_v_srch_pL_R_ADDR,2);

    // L_MaxC in rl6

    VoC_lw16i(REG3,40);
    VoC_loop_r_short(REG4,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);
    VoC_startloop0

    VoC_lw32z(ACC0,DEFAULT);

    VoC_loop_i(1,20)
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_bimac32inc_rp(REG67,REG1);
    VoC_endloop1;

    // restitute &pswWInput[0]
    VoC_sub16_rr(REG1,REG1,REG3,DEFAULT);

    // ABS()
    VoC_bnltz32_r(v_srch02,ACC0)
    VoC_sub32_dr(ACC0,CONST_0_ADDR,ACC0);
v_srch02:

    exit_on_warnings = OFF;
    // pL_R[siI] = L_R
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    exit_on_warnings = ON;

    VoC_add32_rr(RL6,RL6,ACC0,DEFAULT);


    VoC_endloop0;



    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    // input param in REG1 no longer needed
    VoC_movreg32(ACC0,RL6,DEFAULT);
    VoC_jal(CII_NORM_L_ACC0);//return in reg1
    VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);

    VoC_lw32z(REG67,DEFAULT);//L_c0

    VoC_lw16i_set_inc(REG2,LOCAL_v_srch_pL_R_ADDR,2);

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0
    VoC_lw32_p(RL7,REG2,DEFAULT);
    VoC_shr32_rr(RL7,REG1,DEFAULT);
    VoC_shr32_ri(RL7,1,DEFAULT);
    exit_on_warnings=OFF;
    // SW will take effect before >> 1
    VoC_sw32inc_p(RL7,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_sub32_rr(REG67,REG67,RL7,DEFAULT);
    VoC_endloop0
    // reg7 swC0



    // &pswWBasisVecs[0] (because INC2 = 2)
    VoC_pop16(REG2,DEFAULT);

    // &pswWBasisVecs_copy[0]
    VoC_lw16i_set_inc(REG3,LOCAL_v_srch_pswWBasisVecs_copy_ADDR,2);

    // make copy of pswWBasisVecs to Y
    // nb to copy
    VoC_mult16_rd(REG0,REG4,CONST_20_ADDR);

    VoC_push32(REG23,DEFAULT);

    VoC_loop_r_short(REG0,DEFAULT)
    VoC_lw32inc_p(ACC0,REG2,IN_PARALLEL);
    VoC_startloop0
    // preload first sample
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);
    // ACCO stored before previous LW
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    // from the definition of pppswDubD:
    // pppswDubD[siI] = &ppswDPSpace[siI * siNumBasis]
    // pppswDubD[siI][siJ] = ppswDPSpace[siI * siNumBasis + siJ]
    // pppswDubD[siI][siJ] = &pswDSpace[(siI * siNumBasis * 2) + (siJ * 2)]
    // pppswDubD[siI][siJ] = &pswDSpace[(siI * siNumBasis + siJ) * 2]
    // so we load &pswDSpace[0] in REG1

    VoC_lw16i_set_inc(REG1,LOCAL_v_srch_pswDSpace_ADDR,2);

    // siI
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_pop32(REG23,IN_PARALLEL);

    //&pswWBasisVecs_copy[S_LEN]
    VoC_add16_rd(REG3,REG3,CONST_40_ADDR);

    // REG0 : siI, INC0 = 1
    // REG1 : pppswDubD[0][0] = &pswDSpace[0], INC2 = 2
    // REG2 : &pswWBasisVecs[0], INC0 = 2
    // REG3 : &pswWBasisVecs_copy[S_LEN], INC1 = 2
    // REG4 : siNumBasis
    // REG5 : siNumBasis - 1
    // REG6 : siNumBasis - 1 - siI
    // REG7 : swC0


    // save &pswWBasisVecs[0]
    VoC_push16(REG2,DEFAULT);


v_srch_loop0:
    // siNumBasis - 1 - siI
    VoC_sub16_rr(REG6,REG5,REG0,DEFAULT);

    // save pswWBasisVecs_copy[(siI + 1) * S_LEN]
    VoC_push16(REG3,DEFAULT);

    // very first write is at address:
    // &pswDSpace[0 + 1 * 2] = &pswDSpace[2]
    // new val (after iteration) should be :
    // &pswDSpace[(siI + 1) * siNumBasis * 2 + (siI + 1 + 1) * 2 ]
    // or we have :
    // &pswDSpace[(siI + 1) * siNumBasis * 2 + (siI + 1 ) * 2 ]
    // so in both cses we do REG1 += 2

    VoC_inc_p(REG1,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);

    VoC_bngtz16_r(COMPILED_NEW_ADDR6,REG6);
    VoC_loop_r(1,REG6)

    // save pswWBasisVecs[siI * S_LEN]


    VoC_loop_i_short(20,DEFAULT)
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_bimac32inc_pp(REG2,REG3);
    VoC_endloop0

    // get back pswWBasisVecs[siI * S_LEN]
    VoC_lw16_sd(REG2,0,DEFAULT);


    // store result (we swap hilo)
    // pppswDubD[siI][siJ][0] = extract_l(L_D);
    // pppswDubD[siI][siJ][1] = extract_h(L_D);

    // when debugging, I have swaped back hilo
    // because in section 3, some values in this table will be rewritten
    // if the hilo is reversed, extra effort will be done to treat
    // the saving address
    VoC_movreg16(ACC1_HI,ACC0_LO,DEFAULT);
    VoC_movreg16(ACC1_LO,ACC0_HI,IN_PARALLEL);

    VoC_lw16i(REG6,40);


    VoC_sw32inc_p(ACC1,REG1,DEFAULT);


    VoC_endloop1
COMPILED_NEW_ADDR6:
//LABEL_V_srch1:
    // here (after last write) :
    // REG1 = &pswDSpace[(siI * siNumBasis + siNumBasis) * 2]
    // REG1 = &pswDSpace[(siI + 1) * siNumBasis * 2]
    // first address of next iteration is
    // &pswDSpace[(siI + 1) * siNumBasis * 2 + (siI + 1 + 1) * 2 ]
    // REG1 = REG1 + 2 * new_siI + 2

    // siI++
    VoC_inc_p(REG0,DEFAULT);
    VoC_pop16(REG2,DEFAULT);

    VoC_movreg16(REG3,REG0,DEFAULT);

    // REG1 = REG1 + 2 * new_siI
    VoC_shr16_ri(REG3,-1,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG3,DEFAULT);

    // save pswWBasisVecs_copy[(siI + 1) * S_LEN]
    VoC_pop16(REG3,DEFAULT);

    // increment += 40 pswWBasisVecs & pswWBasisVecs_copy
    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG6,IN_PARALLEL);


    VoC_bngt16_rr(v_srch_loop0,REG0,REG5);


//2

    VoC_lw16i_short(REG6,20,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    // REG6 = 40
    VoC_mult16_rr(REG6,REG4,REG6,DEFAULT);

    // get &pswWBasisVecs[0], INC2 = 2
    VoC_pop16(REG2,DEFAULT);

    VoC_loop_r_short(REG6,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_pp(REG2,REG2);
    VoC_endloop0;

//    VoC_NOP();
    // Note :
    // ppswD[siI][siJ] = pswDSpace[siI * siNumBasis + siJ];
    // pppswDubD[siI][siJ][0] = pswDSpace[(siI * siNumBasis + siJ) * 2]

    VoC_lw16i_set_inc(REG3,LOCAL_v_srch_pswDSpace_ADDR,1);  // stephen 060711

    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_jal(CII_NORM_L_ACC0);

    //siShiftCnt in reg1
    VoC_sub16_dr(REG1,CONST_4_ADDR,REG1);

    //L_G0 in RL6
    VoC_shr32_rr(RL6,REG1,DEFAULT);

    // siShiftCnt++
    VoC_sub16_rd(REG1,REG1,CONST_1_ADDR);


    VoC_movreg16(REG2,REG3,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);

    // save swC0
    VoC_push16(REG7,DEFAULT);

    // REG0 : siI, INC0 = 1
    // REG1 : - siShiftCnt - 1
    // REG2 : &pswDSpace[0], INC2 = 2
    // REG3 : &pswDSpace[0], INC3 = 1
    // REG4 : siNumBasis
    // REG5 : siNumBasis - 1
    // REG6 : siNumBasis - 1 - siI
    // REG7 :
    // RL6  : L_G0 (swG0)

    VoC_loop_r(1,REG5)

    // siNumBasis - 1 - siI
    VoC_sub16_rr(REG6,REG5,REG0,DEFAULT);
    // to get pppswDubD[siI][siI+1][0]
    VoC_inc_p(REG2,DEFAULT);

    // to get ppswD[siI][siI+1]
    VoC_inc_p(REG3,DEFAULT);
    VoC_lw32inc_p(ACC0,REG2,IN_PARALLEL);

    VoC_movreg16(REG7,ACC0_LO,DEFAULT);

    VoC_loop_r_short(REG6,DEFAULT)
    VoC_movreg16(REG6,ACC0_HI,IN_PARALLEL);
    VoC_startloop0
    // because of previous swap, lohi are correctly arranged
    // IN_DEbugging, I have changed back the swap of hilo
    // in order to avoid the extra expense on the saving address
    // in reg3, otherwise, there should be a reverse operation
    // of this address.

    VoC_shr32_rr(REG67,REG1,DEFAULT);
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);

    VoC_add32_rr(RL6,REG67,RL6,DEFAULT);
    VoC_shr32_ri(REG67,-1,IN_PARALLEL);

    VoC_add32_rd(REG67,REG67,CONST_0x00008000L_ADDR);

    VoC_movreg16(REG7,ACC0_LO,DEFAULT);
    VoC_movreg16(REG6,ACC0_HI,IN_PARALLEL);

    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    exit_on_warnings=ON;


    VoC_endloop0

    VoC_sub16_rd(REG2,REG2,CONST_2_ADDR);
    // here (after last write) :
    // REG3 = &pswDSpace[(siI * siNumBasis + siNumBasis)]
    // REG3 = &pswDSpace[(siI + 1) * siNumBasis]
    // first address of next iteration is
    // &pswDSpace[(siI + 1) * siNumBasis + (siI + 1 + 1)] =
    // target : REG3 <= REG3 + new_siI + 1

    // here (after last read) :
    // REG2 = &pswDSpace[(siI * siNumBasis + siNumBasis) * 2]
    // REG2 = &pswDSpace[(siI + 1) * siNumBasis * 2]
    // first address of next iteration is
    // &pswDSpace[(siI + 1) * siNumBasis + (siI + 1 + 1) * 2] =
    // target : REG2 <= REG2 + 2 * new_siI + 2

    // siI++
    VoC_inc_p(REG0,DEFAULT);
    VoC_movreg16(REG6,REG0,DEFAULT);

    // REG2 = REG2 + 2 * new_siI
    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);

    // here REG3 = REG3 + new_siI
    VoC_add16_rr(REG3,REG3,REG0,IN_PARALLEL);

    VoC_endloop1


// 3
    // REG0 : psw0, INC0 = siEBits + 1
    // REG1 : pswCGUpdates, INC1 = 1
    // REG2 : psw2, INC2 = siEBits
    // REG3 : pL_R, INC2 = 2
    // REG4 : siNumBasis
    // REG5 : siEBits
    // REG6 :
    // REG7 :
    // RL6  : L_G0 (swG0 in hi)


    // for the moment only siEBits is needed
    // siEBits in REG5
    VoC_lw16i_short(REG5,8,DEFAULT);
    VoC_lw16i_short(INC0,9,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,LOCAL_v_srch_pswCGUpdates_ADDR,1);
    VoC_be16_rd(v_srch_siEBits,REG4,CONST_9_ADDR);
    VoC_lw16i_short(REG5,6,DEFAULT);
    VoC_lw16i_short(INC0,7,IN_PARALLEL);
v_srch_siEBits:
    VoC_movreg16(REG0,REG1,DEFAULT);

    VoC_lw16i_set_inc(REG3,LOCAL_v_srch_pL_R_ADDR,2);



    // &pswCGUpdates[2*siEBits]
    VoC_add16_rr(REG2,REG5,REG0,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG5,DEFAULT);

    VoC_movreg16(INC2,REG5,IN_PARALLEL);
    VoC_lw32_d(RL7,CONST_0x00008000L_ADDR);

    // here we make one extra add, but SW occurs before
    // add result, so everything is correct (last add not used)

    VoC_loop_r_short(REG5,DEFAULT)
    VoC_add32inc_rp(REG67,RL7,REG3,IN_PARALLEL);
    VoC_startloop0
    VoC_add32inc_rp(REG67,RL7,REG3,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;



    // REG0 : psw0,     INC0 = siEBits + 1
    // REG1 : psw1,     INC1 = 1
    // REG2 : psw2,     INC2 = siEBits
    // REG3 : pswDSpace,    INC3 = 1
    // REG4 : siNumBasis
    // REG5 : siEBits
    // REG6 : temp
    // REG7 : siEBits - 1 - siI
    // RL6  : L_G0 (swG0 in hi)


    // &pswCGUpdates[siEBits]
    VoC_add16_rr(REG0,REG1,REG5,DEFAULT);

    // siEBits - 1
    VoC_sub16_rd(REG7,REG5,CONST_1_ADDR);

    // &pswCGUpdates[siEBits - (siEBits - 1)]
    VoC_sub16_rr(REG1,REG0,REG7,DEFAULT);

    // &ppswD[0][1] = pswDSpace+1
    VoC_lw16i_set_inc(REG3,LOCAL_v_srch_pswDSpace_ADDR+1,1);

    VoC_loop_r(1,REG5)

    VoC_bngtz16_r(COMPILED_NEW_LABEL7,REG7);

    VoC_loop_r_short(REG7,DEFAULT)
    VoC_startloop0

    // in fact here a 16-bit operation is sufficient,
    // because we are only using the hi for deposit &
    // extract. Obviously bit 0 of the hi will be 0.
    VoC_lw16inc_p(REG6,REG3,DEFAULT);
    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_sw16inc_p(REG6,REG2,DEFAULT);

    VoC_endloop0;
COMPILED_NEW_LABEL7:

    VoC_lw16inc_p(REG6,REG3,DEFAULT);
    VoC_sub16_dr(REG6,CONST_0_ADDR,REG6);

    // write occurs before
    // REG6 = siNumBasis - (siEBits - 1 - siI)
    VoC_sub16_rr(REG6,REG4,REG7,DEFAULT);


    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings=ON;


    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);
    // Compute NEXT ppswD:
    //
    // &ppswD[siI][siEBits+1] =
    // &pswDSpace[siI * siNumBasis + siEBits + 1] =
    // &pswDSpace[newsiI * siNumBasis + siEBits + 1 - siNumBasis] =
    // first value of next iteration is:
    // &ppswD[newsiI][newsiI+1] =
    // &pswDSpace[newsiI * siNumBasis + newsiI + 1]
    // so siNumBasis - (siEBits - 1 - siI) need to be added


    VoC_lw16i_short(REG6,1,IN_PARALLEL);



    // siEBits - 1 - newsiI = (siEBits - 1 - siI)--
    VoC_sub16_rr(REG7,REG7,REG6,DEFAULT);


    // Compute NEXT PSW1:
    //
    // psw1 [siEBits - 1 + (siI * siEBits)] =
    // psw1 [-1 + (newsiI * siEBits)]
    // first value of next iteration is:
    // psw1 [newsiI + 1 - 1 + (newsiI * siEBits)] =
    // psw1 [newsiI + (newsiI * siEBits)]
    // so newsiI + 1 need to be added to psw1
    // newsiI+1=(siEBits)-(siEBits-1-newsiI) = reg5-new reg7

    VoC_add16_rr(REG1,REG5,REG1,DEFAULT);



    // Compute NEXT PSW2:
    //
    // psw2 [siI + (siEBits * (siEBits - 1))] =
    // psw2 [-1 + newsiI + (siEBits * (siEBits - 1))]
    // first value of next iteration is:
    // psw2 [newsiI + (siEBits * (newsiI + 1 - 1))] =
    // psw2 [newsiI + (siEBits * newsiI)]
    // so siEBits * (siEBits - 1 - newsiI) - 1
    // need to be sub from psw2

    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_mult16_rr(REG6,REG5,REG7,IN_PARALLEL);

    VoC_sub16_rr(REG1,REG1,REG7,DEFAULT);

    VoC_sub16_rr(REG2,REG2,REG6,DEFAULT);


    VoC_endloop1;




//4

    // REG0 : 0             INC0 = siEBits + 1
    // REG1 : psw0,         INC1 = 1
    // REG2 : siEBits*(siEBits+1),  INC2 = siEBits
    // REG3 : pswCGUpdates,     INC3 = 1
    // REG4 : siNumBasis
    // REG5 : siEBits
    // REG6 :
    // REG7 :
    // RL6  : L_G0 (swG0 in hi)


    VoC_mult16_rr(REG2,REG5,REG5,DEFAULT);
    VoC_lw16i_short(REG7,0,DEFAULT);

    VoC_lw16i_set_inc(REG3,LOCAL_v_srch_pswCGUpdates_ADDR,1);

    // add occurs after inc
    VoC_inc_p(REG2,DEFAULT);
    VoC_add16_rr(REG1,REG3,REG2,IN_PARALLEL);

    VoC_lw16inc_p(REG6,REG3,DEFAULT);

    VoC_loop_r_short(REG2,DEFAULT)
    VoC_startloop0
    VoC_sub16_rr(REG0,REG7,REG6,DEFAULT);
    VoC_lw16inc_p(REG6,REG3,DEFAULT);
    VoC_sw16inc_p(REG0,REG1,DEFAULT);
    VoC_endloop0;


    VoC_lw16i_set_inc(REG1,LOCAL_v_srch_pswBits_ADDR,1);
//  VoC_lw16_d(REG0,CONST_0xc000_ADDR);
    VoC_lw16i(REG0,0xc000);

    VoC_loop_r_short(REG5,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG0,REG1,DEFAULT);
    VoC_endloop0;


    // REG0 : (1 << siEBits) - 1, siI, 0x8000, swG      INC0 = 1
    // REG1 : pswUIndex, siBitIndex, temp,  swCC        INC1 = 1
    // REG2 : pswBIndex,                    INC2 = 1
    // REG3 : pswUpdatePtr                  INC3 = 1
    // REG4 : swGMax
    // REG5 : swCCMax
    // REG6 : pswModNextBit, pswBits, temp
    // REG7 : siEBits - 1
    // RL6  : 0x00008000, temp
    // RL7  : L_C
    // ACC0 : L_G
    // ACC1 : temp

    // get swC0
    VoC_pop16(REG5,DEFAULT);

    // get &pswBits[0] to use in inner critical loop
    // VoC_lw16i(REG7,LOCAL_v_srch_pswBits_ADDR);

    // store siNumBasis (occurs before = 0)
    VoC_push16(REG4,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    // store &pswBits[0] to use in inner critical loop
    // VoC_sw16_d(REG7,LOCAL_v_srch_pswBits_ADDR_ADDR);

    // we need to decide for the vocoder variables here
    VoC_lw16i_set_inc(REG0,0x00ff,1);

    // store siBest = 0 (REG7 == 0)
    // VoC_sw16_d(REG1,LOCAL_v_srch_siBest_ADDR);
    VoC_sw16_d(REG7,LOCAL_v_srch_siBest_ADDR);

    VoC_lw16i_set_inc(REG1,TABLE_SFRM_pswUpdateIndexV_pswBitIndexV_ADDR,1);
//  VoC_lw16i_set_inc(REG2,TABLE_SFRM_pswBitIndexV_ADDR,1);
    VoC_lw16i(REG6,TABLE_SFRM_pswModNextBitV_ADDR);
    VoC_lw16i_short(REG7,7,DEFAULT);

    VoC_be16_rd(v_srch_psw_other,REG4,CONST_9_ADDR);

    VoC_shr16_ri(REG0,2,DEFAULT); //0x003f
    VoC_lw16i_short(REG7,5,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,TABLE_SFRM_pswUpdateIndexUn_pswBitIndexUn_ADDR,1);

//      VoC_lw16i_set_inc(REG2,TABLE_SFRM_pswBitIndexUn_ADDR,1);
    VoC_lw16i(REG6,TABLE_SFRM_pswModNextBitUV_ADDR);

v_srch_psw_other:

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_movreg16(REG2,REG1,IN_PARALLEL);
    // move L_G to ACC0
    VoC_lw16i_short(RL6_LO,0,DEFAULT);

    // L_C in ACC1
    VoC_movreg32(ACC1,REG45,IN_PARALLEL);

    // store &pswModNextBit[0] to use in inner critical loop


    VoC_movreg32(ACC0,RL6,DEFAULT);

    // compute swCCMax in REG5
    VoC_multf32_rr(REG45,REG5,REG5,IN_PARALLEL);


    VoC_lw32_d(RL6,CONST_0x00008000L_ADDR);
    VoC_add32_rr(REG45,REG45,RL6,DEFAULT);

    // get swGMax in REG4
    VoC_movreg16(REG4,ACC0_HI,DEFAULT);
    VoC_lw16i_set_inc(REG3,LOCAL_v_srch_pswCGUpdates_ADDR,1);


    // VoC_lw16_d(REG4,LOCAL_v_srch_pswBits_ADDR_ADDR);
    // get &pswBits[0] to use in inner critical loop
    VoC_lw16i(REG4,LOCAL_v_srch_pswBits_ADDR);

    exit_on_warnings=OFF;
    VoC_push32(REG45,DEFAULT);
    exit_on_warnings=ON;

    // VoC_lw16_d(REG5,LOCAL_v_srch_pswModNextBit_ADDR_ADDR);
    // &pswModNextBit[0] from REG6 to REG5
    VoC_movreg16(REG5,REG6,IN_PARALLEL);

    // siI+1 in REG0
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_movreg16(REG6,REG0,IN_PARALLEL);

    VoC_push32(REG45,DEFAULT);
    VoC_push16(REG3,IN_PARALLEL);

    VoC_loop_r(1,REG6);
    VoC_lw16inc_p(REG6,REG1,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);

    VoC_and16_ri(REG6,0x00ff);
    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);
    VoC_lw16inc_p(REG6,REG2,IN_PARALLEL);

    VoC_shru16_ri(REG6,8,DEFAULT);

    // compute &pswBits[siBitIndex]
    VoC_add16_rr(REG6,REG4,REG6,DEFAULT);
    // compute &pswModNextBit[siBitIndex]
    VoC_add16_rr(REG1,REG5,REG6,IN_PARALLEL);


    exit_on_warnings=OFF;
    // free two regs (0x8000 is still not written)
    VoC_push32(REG01,DEFAULT);
    exit_on_warnings=ON;
    // get 0x8000 in REG0
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);


    VoC_loop_r_short(REG7,DEFAULT);
    // do not use msu for L_C to avoid move
    VoC_msu32inc_rp(REG0,REG3,IN_PARALLEL);
    VoC_startloop0
    exit_on_warnings =OFF;
    // compute &pswBits[siBitIndex]
    VoC_add16_rp(REG6,REG4,REG1,DEFAULT);
    // compute &pswModNextBit[siBitIndex]
    VoC_add16_rp(REG1,REG5,REG1,IN_PARALLEL);
    // REG6 is not incremental, so won't be incremented
    exit_on_warnings =ON;
    exit_on_warnings =OFF;
    // here REG6 = &pswBits[siBitIndex]
    VoC_mac32inc_pp(REG6,REG3,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    exit_on_warnings =ON;

    VoC_endloop0;



    VoC_msu32_rp(REG0,REG3,DEFAULT);
    // negate(pswBits[siBitIndex])
    VoC_sub16_rp(REG5,REG1,REG6,IN_PARALLEL);

    // extract_h(L_C)
    VoC_movreg16(REG1,ACC1_HI,DEFAULT);
    VoC_lw32_sd(REG45,2,IN_PARALLEL);

    // pswBits[siBitIndex] = negate(pswBits[siBitIndex])
    // SW before REG1 is overwritten
    exit_on_warnings=OFF;
    VoC_sw16_p(REG5,REG6,DEFAULT);
    exit_on_warnings=ON;
    // swCC = mult_r(sw1, sw1)
    // swG = extract_h(L_G);
    VoC_multf32_rr(REG01,REG1,REG1,IN_PARALLEL);

    VoC_movreg16(REG6,ACC0_HI,DEFAULT);
    VoC_add32_rr(REG01,REG01,RL6,DEFAULT);

    // L_mult(swG, swCCMax);
    VoC_multf32_rr(RL7,REG6,REG5,DEFAULT);
    // L_msu(L_1, swGMax, swCC);
    VoC_multf32_rr(REG45,REG1,REG4,IN_PARALLEL);

    VoC_NOP();
    VoC_bngt32_rr(v_srch_no_update,REG45,RL7);

    VoC_movreg16(REG0,REG6,DEFAULT);
    VoC_lw32_sd(REG01,0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32_sd(REG01,2,DEFAULT);
    exit_on_warnings=ON;
    VoC_sw16_d(REG0,LOCAL_v_srch_siBest_ADDR);

v_srch_no_update:
    // restitute siI & pswUIndex[siI]
    VoC_pop32(REG01,DEFAULT);

    // pswUpdatePtr = &pswCGUpdates[pswUIndex[siI]];
    VoC_lw32_sd(REG45,0,DEFAULT);
    VoC_lw16_sd(REG3,0,IN_PARALLEL);

    VoC_endloop1;


//5

    // REG0 : siBest_copy, sw1      INC0 = 1
    // REG1 : pL_R              INC1 = 2
    // REG2 : temp              INC2 = 1
    // REG3 :               INC3 = 1
    // REG4 : siNumBasis
    // REG5 : siMask
    // REG6 : siBest
    // REG7 : siMask_copy
    // RL6  : L_1
    // RL7  :
    // ACC0 :
    // ACC1 :
    VoC_pop16(REG3,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);

    // restitute siNumBasis
    VoC_pop16(REG4,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);

    // siBest
    VoC_lw16_d(REG6,LOCAL_v_srch_siBest_ADDR);

    // siBest_copy
    VoC_movreg16(REG0,REG6,DEFAULT);

    //L_1 in rl6
    VoC_lw32z(RL6,IN_PARALLEL);

    // shr(siBest, 1)
    VoC_shr16_ri(REG0,1,DEFAULT);

    // siMask in reg5
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    // sw1 = siBest ^ (shr(siBest, 1))
    VoC_xor16_rr(REG0,REG6,DEFAULT);

    // temp for shl(1, siNumBasis)
    VoC_lw16i_short(REG2,0,IN_PARALLEL);

    // pL_R
    VoC_lw16i_set_inc(REG1,LOCAL_v_srch_pL_R_ADDR,2);

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0
    // temp<<1 for shl(1, siNumBasis)
    VoC_shr16_ri(REG2,-1,DEFAULT);

    VoC_movreg16(REG7,REG5,DEFAULT);
    VoC_shr16_ri(REG5,-1,IN_PARALLEL);

    VoC_and16_rr(REG7,REG0,DEFAULT);

    // temp++ for shl(1, siNumBasis)
    VoC_inc_p(REG2,IN_PARALLEL);

    VoC_bnez16_r(v_srch111,REG7);
    VoC_sub32_rp(RL6,RL6,REG1,DEFAULT);
    VoC_sub32_rp(RL6,RL6,REG1,DEFAULT);
v_srch111:
    VoC_add32inc_rp(RL6,RL6,REG1,DEFAULT);
    VoC_endloop0;

    VoC_pop16(RA,DEFAULT);

    VoC_bnltz32_r(v_srch333,RL6);
    VoC_xor16_rr(REG0,REG2,DEFAULT);
v_srch333:

    // return sw1 in REG0
    VoC_return;

//6

}


void CII_sfrmAnalysis_opt(void)
{

#if 0

    voc_short   LOCAL_sfrmAnalysis_pswPVec_ADDR             ,40,x       //[40]shorts
    voc_short   LOCAL_sfrmAnalysis_pswWPVec_ADDR            ,40,x       //[40]shorts
    voc_short   LOCAL_sfrmAnalysis_pswWBasisVecs_ADDR       ,360,x      //[360]shorts
    voc_short   LOCAL_sfrmAnalysis_ppswWVselpEx_ADDR        ,80,x        //[80]shorts

#endif

#if 0

    voc_short   LOCAL_sfrmAnalysis_pswTempVec_ADDR          ,40,y       //[40]shorts
    voc_short   LOCAL_sfrmAnalysis_pswBitArray_ADDR         ,10,y       //[10]shorts
    voc_short   LOCAL_sfrmAnalysis_pswHNWCoefs_ADDR         ,6,y        //[6]shorts
    voc_short   LOCAL_sfrmAnalysis_pswWSVec_ADDR            ,40,y       //[40]shorts
    voc_short   sfrmAnalysis_psiVSCode1_ADDR_ADDR           ,y      //1 short
    voc_short   sfrmAnalysis_psiVSCode2_ADDR_ADDR           ,y      //1 short
    voc_short   sfrmAnalysis_psiGsp0Code_ADDR_ADDR          ,y      //1 short
    voc_short   sfrmAnalysis_psiLagCode_ADDR_ADDR           ,y      //1 short
    voc_short   LOCAL_sfrmAnalysis_swLag_ADDR               ,y       //1 short
    voc_short   LOCAL_sfrmAnalysis_swLtpShift_ADDR          ,y       //1 short
    voc_short   LOCAL_sfrmAnalysis_snsRs00_ADDR             ,2,y         //[2]shorts
    voc_short   LOCAL_sfrmAnalysis_snsRs11_ADDR             ,2,y         //[2]shorts
    voc_short   LOCAL_sfrmAnalysis_snsRs22_ADDR             ,2,y         //[2]shorts
    voc_short   sfrmAnalysis_SP16_SAVE0_ADDR                ,y       //1 short
    voc_short   sfrmAnalysis_SP16_SAVE1_ADDR                ,y       //1 short
    voc_short   LOCAL_sfrmAnalysis_ppswVselpEx_ADDR         ,80,y        //[80]shorts
#endif


    // We try to pass as many parameters as possible in REGs:
    //
    // REG0 : *pswWSpeech,  INC0 = 1
    // REG1 : siNumLags,    INC0 = -1
    // REG2 : *pswLagList,  INC2 = 1
    // REG3 : *pswHCoefs,   INC3 = 1
    // REG4 : swPitch
    // REG5 : snsSqrtRs
    // REG6 : swHNWCoef
    // REG7 : swSP
    //
    // the rest are stored in memory:
    // psiLagCode : sfrmAnalysis_psiLagCode_ADDR_ADDR

    // We will use a new coding technique, based on
    // the new sw/lw mode _sd that has been introduced in VoC.
    // Using the SP we will define a local space that can
    // be easily accessed with _sd. Controlling the local var
    // space is done as before with pop/push. The new feature _sd
    // will allow accessing for rw any value of the local space
    // without poping or pushing the rest of the values in the stack.


    // RA
    VoC_push16(RA,DEFAULT);

    // An extention of sw16_d & lw16_d allows saving the SP16 & SP32
    // pointers in memory. Thus, if a space is defined, no need to pop all
    // values back to get back the inital SP16 & SP32 value.

    // saving the SP16
    VoC_sw16_d(SP16,sfrmAnalysis_SP16_SAVE0_ADDR);

    // saving local pointers & parameters
    // needed throughout the function


    // snsRs22
    VoC_lw16i(ACC0_LO,LOCAL_sfrmAnalysis_snsRs22_ADDR);
    // snsRs11
    VoC_lw16i(ACC1_LO,LOCAL_sfrmAnalysis_snsRs11_ADDR);
    // snsRs00
    VoC_lw16i(RL6_LO,LOCAL_sfrmAnalysis_snsRs00_ADDR);

    // push snsRs
    VoC_push16(ACC0_LO,DEFAULT);
    VoC_push16(ACC1_LO,DEFAULT);
    VoC_push16(RL6_LO,DEFAULT);

    // push pswHCoef & snsSqrtRs
    VoC_push16(REG3,DEFAULT);
    VoC_push16(REG5,DEFAULT);

    // swVoicingMode & psiGsp0Code
    VoC_lw16_d(REG5,sfrmAnalysis_psiGsp0Code_ADDR_ADDR);
    VoC_lw16_d(REG3,LOCAL_EN_SPEECHENCODER_siUVCode_ADDR);
    VoC_lw16_p(REG5,REG5,DEFAULT);

    VoC_push16(REG3,DEFAULT);
    VoC_push16(REG5,DEFAULT);

    // psiVSCode1 & psiVSCode2
    VoC_lw16_d(REG3,sfrmAnalysis_psiVSCode1_ADDR_ADDR);
    VoC_lw16_d(REG5,sfrmAnalysis_psiVSCode2_ADDR_ADDR);
    VoC_lw16_p(REG3,REG3,DEFAULT);
    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_push16(REG3,DEFAULT);
    VoC_push16(REG5,DEFAULT);

    // pswTempVec (need to be placed in RAM_Y) & pswHState
    VoC_lw16i(REG3,LOCAL_sfrmAnalysis_pswTempVec_ADDR);
    VoC_lw16i(REG5,GLOBAL_SFRM_pswHState_ADDR);
    VoC_push16(REG3,DEFAULT);
    VoC_push16(REG5,DEFAULT);

    // pswPVec & ppswVselpEx
    VoC_lw16i(REG3,LOCAL_sfrmAnalysis_pswPVec_ADDR);
    VoC_lw16i(REG5,LOCAL_sfrmAnalysis_ppswVselpEx_ADDR);
    VoC_push16(REG3,DEFAULT);
    VoC_push16(REG5,DEFAULT);

    // pswLTPStateOut & pswBitArray
    VoC_lw16i(REG3,GLOBAL_SFRM_pswLtpStateBase_ADDR);
    VoC_add16_rd(REG3,REG3,CONST_147_ADDR);
    VoC_lw16i(REG5,LOCAL_sfrmAnalysis_pswBitArray_ADDR);
    VoC_push16(REG3,DEFAULT);
    VoC_push16(REG5,DEFAULT);




    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *

    // saving the SP16
    VoC_sw16_d(SP16,sfrmAnalysis_SP16_SAVE1_ADDR);


    // swSp is 0 or 1, using _rd mode will limit the jump
    // range, so probably an error will be generated if used
    // because current jump is very big. So we use bez instead
    VoC_bez16_r(sfrmAnalysis_mid,REG7);


    // saving local pointers needed for swSP = 1 only
    // pswWSpeech
    VoC_push16(REG0,DEFAULT);
    // siNumLags
    VoC_push16(REG1,DEFAULT);
    // pswLagList
    VoC_push16(REG2,DEFAULT);
    // swPitch
    VoC_push16(REG4,DEFAULT);
    // swHNWCoef
    VoC_push16(REG6,DEFAULT);

    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // pswWSpeech       *
    // siNumLags        *
    // pswLagList       *
    // swPitch      *
    // swHNWCoef        *


    // REG1 : pswTempVec[S_LEN] need to be placed in RAM_Y
    VoC_lw16_sd(REG1,10,DEFAULT);
    // REG2 : pswHState
    VoC_lw16_sd(REG2,9,DEFAULT);
    // REG3 : pswCoef
    VoC_lw16_sd(REG3,16,DEFAULT);

    // INPUT parameters :
    // REG3 : pswCoef , INC3 = 1
    // REG2 : pswState, INC2 = 1
    // REG1 : swFiltOut, INC1 = -1
    // other used regs: REG4, REG5, ACC0, RL7

    //      begin of CII_lpcZiIir_opt

    VoC_push16(REG7,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_lw32_d(RL7,CONST_0x00000800L_ADDR);

    // in order to achieve better speed,
    // the loop is split in two
    // here REG5 is in [0,9]
    VoC_loop_i(1,10)

    VoC_lw16i_short(REG4,10,DEFAULT);
    VoC_movreg32(ACC0,RL7,DEFAULT);

    VoC_bez16_r(lpcZiIir_skip_1st,REG5);
    VoC_loop_r_short(REG5,DEFAULT)
    VoC_startloop0
    VoC_msu32inc_pp(REG1,REG3,DEFAULT);
    VoC_endloop0;

    // if NP > siSmp, REG4 = siSmp, newREG4 = NP - siSmp
    // else REG4 = NP, newREG4 = NP - NP = 0 !>0
    // so second loop will be skipped
    VoC_add16_rr(REG1,REG1,REG5,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG5,IN_PARALLEL);

    // get &pswFiltOut[siSmp]
    VoC_add16_rd(REG1,REG1,CONST_1_ADDR);

lpcZiIir_skip_1st:

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG2,DEFAULT);
    VoC_msu32inc_rp(REG7,REG3,DEFAULT);
    VoC_endloop0;

    // siSmp++
    VoC_add16_rd(REG5,REG5,CONST_1_ADDR);

    // restitute &pswState[0]
    VoC_sub16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_shr32_ri(ACC0,-4,IN_PARALLEL);

    // restitute &pswCoef[0]
    VoC_sub16_rd(REG3,REG3,CONST_10_ADDR);

    VoC_sw16_p(ACC0_HI,REG1,DEFAULT);

    VoC_endloop1;


    VoC_lw16i_short(INC1,-2,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    // here counter >= 10
    // we do not increment REG5 anymore, REG5 == 10
    // using bimac saves 4 cycles per iteration
    VoC_loop_i(1,30)

    // REG3 is always even, REG1 is in RAM_Y

    VoC_aligninc_pp(REG1,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_push16(REG1,IN_PARALLEL);
    exit_on_warnings=ON;

    VoC_loop_i_short(5,DEFAULT)
    VoC_movreg32(ACC0,RL7,IN_PARALLEL);
    VoC_startloop0
    VoC_bimsu32inc_pp(REG1,REG3);
    VoC_endloop0;

    VoC_pop16(REG1,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    VoC_shr32_ri(ACC0,-4,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG2,IN_PARALLEL);

    // restitute &pswCoef[0]
    VoC_sub16_rd(REG3,REG3,CONST_10_ADDR);
    VoC_sw16_p(ACC0_HI,REG1,DEFAULT);

    VoC_endloop1;

    VoC_pop16(REG7,DEFAULT);
// end of CII_lpcZiIir_opt

    // pswTempVec need to be placed in RAM_Y
    VoC_lw16_sd(REG2,10,DEFAULT);
    // pswWSVec need to be aligned on even address
    VoC_lw16i_set_inc(REG1,LOCAL_sfrmAnalysis_pswWSVec_ADDR,2);
    // REG0 : pswWSpeech already set

    VoC_sub16inc_pp(REG4,REG0,REG2,DEFAULT);
    // pswWSVec on stack
    VoC_push16(REG1,DEFAULT);


    VoC_loop_i_short(20,DEFAULT)

    VoC_startloop0
    VoC_sub16inc_pp(REG5,REG0,REG2,DEFAULT);
    VoC_sub16inc_pp(REG4,REG0,REG2,DEFAULT);
    exit_on_warnings=OFF;
    // SW before REG4 update
    VoC_sw32inc_p(REG45,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;

    VoC_lw16i_short(ACC0_HI,0x001d,DEFAULT);
    VoC_lw16i(ACC0_LO,0xff4c);
    //  VoC_lw32_d(ACC0,CONST_0x001dff4cL_ADDR);

    // pswWSVec
    VoC_lw16_sd(REG2,0,DEFAULT);

    VoC_loop_i_short(40,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_shr16_ri(REG4,2,DEFAULT);
    VoC_mac32_rr(REG4,REG4,DEFAULT);
    VoC_endloop0;

    // compute swPnShift
    VoC_lw16i_short(REG6, 0, DEFAULT);
    // swPnEnergy in REG5
    VoC_add32_rd(REG45,ACC0,CONST_0x00008000L_ADDR);


    VoC_bngt16_rd(sfrmAnalysis_000, REG5, CONST_0x7ff_ADDR);
    VoC_lw16i_short(REG6, 1, DEFAULT);
    VoC_bngt16_rd(sfrmAnalysis_000, REG5, CONST_0x1fff_ADDR);
    VoC_lw16i_short(REG6, 2, DEFAULT);
sfrmAnalysis_000:


    // pswWSVec
    VoC_lw16_sd(REG1,0,DEFAULT);

    // push swPnShift
    VoC_push16(REG6,DEFAULT);

    // pswWSVec
    VoC_movreg16(REG2,REG1,IN_PARALLEL);

    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_lw32inc_p(REG45,REG1,IN_PARALLEL);
    VoC_loop_i_short(20,DEFAULT)

    VoC_startloop0
    VoC_shr16_rr(REG4,REG6,DEFAULT);
    VoC_shr16_rr(REG5,REG6,DEFAULT);
    VoC_lw32inc_p(REG45,REG1,DEFAULT);
    exit_on_warnings=OFF;
    // SW before REG45 update
    VoC_sw32inc_p(REG45,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;


    // preload case (swVoicingMode == 0)
    // which is overwritten if (swVoicingMode > 0)


    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16_d(REG3,sfrmAnalysis_psiLagCode_ADDR_ADDR);

    // swLag = 0;
    VoC_sw16_d(REG0,LOCAL_sfrmAnalysis_swLag_ADDR);
    // *psiLagCode = 0
    VoC_sw16_p(REG0,REG3,DEFAULT);
    // siHnwNum
    VoC_push16(REG0,DEFAULT);


    // pswWPVec
    VoC_lw16i(REG3,LOCAL_sfrmAnalysis_pswWPVec_ADDR);

    // swVoicingMode
    VoC_lw16_sd(REG0,17,DEFAULT);

    // push pswWPVec
    VoC_push16(REG3,DEFAULT);


    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // pswWSpeech       *
    // siNumLags        *
    // pswLagList       *
    // swPitch      *
    // swHNWCoef        *
    // pswWSVec     *
    // swPnShift        *
    // siHnwNum     *
    // pswWPVec     *


    // (swVoicingMode !> 0)
    VoC_bngtz16_r(sfrmAnalysis_011,REG0);


    //  function closedLoopLagSearch is called only once
    //  so we can add it inline to save a lot of parameter
    //  passing overhead


    // INPUT
    // pswLagList (REG2 from input params),
    // siNumLags (REG1 from input params),
    // pswLtpStateBase (GLOBAL_SFRM_pswLtpStateBase_ADDR),
    // pswHCoefs (REG3 from input parameters),
    // pswWSVec (LOCAL_sfrmAnalysis_pswWSVec_ADDR),
    // &swLag (LOCAL_sfrmAnalysis_swLag_ADDR),
    // &swLtpShift (LOCAL_sfrmAnalysis_swLtpShift_ADDR),


    /**************************************************
    * INPUT :
    * REG0 : pswLagList
    * REG1 : pswLtpState
    * REG2 : pswHCoefs
    * REG3 : pswPVect           INCR3=1
    * REG5 : *pswLtpShift
    * REG6 : *pswLag
    * REG7 : iNumLagsACC0
    * OUTPUT :
    * REG2 : siLagCode
    * REG5 : *pswLtpShift
    * REG6 : *pswLag
    **************************************************/

    {

#if 0
        voc_short   LOCAL_closedLoopLagSearch_ppswTVect_ADDR         ,160,x      // 160
        voc_short   LOCAL_closedLoopLagSearch_pL_CCBuf_ADDR          ,12,y       // [6] long
        voc_short   LOCAL_closedLoopLagSearch_pL_CGBuf_ADDR          ,12,y       // [6] long
        voc_short   LOCAL_closedLoopLagSearch_pswCCBuf_ADDR          ,6,y       // [6]
        voc_short   LOCAL_closedLoopLagSearch_pswCGBuf_ADDR          ,6,y       // [6]
#endif





        // pswLagList in REG0, INC = 2
        VoC_lw16_sd(REG0,6,DEFAULT);

        // pswHCoefs  in REG2, INC = -1
        VoC_lw16_sd(REG2,20,DEFAULT);

        // siNumLags
        VoC_lw16_sd(REG7,7,DEFAULT);

        // pswWSVec
        VoC_lw16_sd(REG3,3,DEFAULT);

        // pswLtpState
        VoC_lw16i(REG1,GLOBAL_SFRM_pswLtpStateBase_ADDR);

        /**************************************************************************/



        VoC_lw16i_short(REG4,0,DEFAULT);

closedLoopLagSearch_loopstart:
        VoC_movreg16(REG5, REG1,DEFAULT);
        VoC_push32(REG01,DEFAULT);
        VoC_add16_rr(REG0,REG4,REG0,DEFAULT);
        VoC_add16_rd(REG5, REG5, CONST_147_ADDR);
        VoC_lw16_p(REG6, REG0, DEFAULT);
        //VoC_sw16_d(REG5, fp_ex_pswLTPState_ADDR_ADDR);
        //VoC_sw16_d(REG6, fp_ex_swOrigLagIn_ADDR);
        VoC_push32(REG45,DEFAULT);
        /********************************************
        // pswLTPState in REG5
        // swOrigLagIn in REG6
        *********************************************/
        VoC_jal(CII_fp_ex_opt);
        VoC_pop32(REG45,DEFAULT);

        VoC_mult16_rd(REG2,REG4,CONST_40_ADDR);
        VoC_lw16i(REG3, LOCAL_closedLoopLagSearch_ppswTVect_ADDR);
        VoC_add16_rr(REG3,REG3,REG2,DEFAULT);
        VoC_lw16_sd(REG6, 20, DEFAULT);
        VoC_push16(REG4,DEFAULT);
        /***************************************************
        INPUT:
              REG3 : pswFiltOut
              REG5 : pswInput
              REG6 : pswCoef
        OUTPUT:
              REG3 : pswFiltOut
        ***************************************************/
        VoC_jal(CII_lpcZsIir_opt);
        VoC_pop16(REG4,DEFAULT);
        VoC_lw16_sd(REG7, 7, DEFAULT);
        VoC_add16_rd(REG4, REG4, CONST_1_ADDR);
        VoC_pop32(REG01,DEFAULT);
        VoC_bgt16_rr(closedLoopLagSearch_loopstart,REG7,REG4);

        VoC_lw16_sd(REG2,20,DEFAULT);

//1

        VoC_lw16i_set_inc(REG0, LOCAL_closedLoopLagSearch_ppswTVect_ADDR, 1);
//     VoC_lw32_d(ACC0,CONST_0x001dff4cL_ADDR);

        VoC_lw16i_short(ACC0_HI,0x001d,DEFAULT);
        VoC_lw16i(ACC0_LO,0xff4c);
        // the problem here is for example (11*11)>>2=10 while (11>1)*(11>1)=1;

        VoC_loop_i_short(40,DEFAULT)
        VoC_startloop0
        VoC_lw16inc_p(REG5,REG0,DEFAULT);
        VoC_shr16_ri(REG5,2,DEFAULT);
        VoC_mac32_rr(REG5, REG5,DEFAULT);
        VoC_endloop0

        VoC_sub16_rd(REG6, REG7, CONST_1_ADDR);
        VoC_lw16i_short(REG5,0,DEFAULT); //It is useless,just make next instruction to in parallel!!
        VoC_lw16i_short(INC0,40,IN_PARALLEL);


        VoC_bltz16_r(jump_labellele0,REG6);
        VoC_loop_r_short(REG6,DEFAULT)
        VoC_startloop0
        VoC_lw16inc_p(REG5,REG0,DEFAULT);
        VoC_shr16_ri(REG5,2,DEFAULT);
        VoC_mac32_rr(REG5, REG5, DEFAULT);
        VoC_endloop0;

jump_labellele0:
        // compute shift value

//  in STK16, &pVect, RA
        VoC_lw16i_set_inc(REG0, LOCAL_closedLoopLagSearch_pL_CCBuf_ADDR, 2);
        VoC_lw16i_set_inc(REG1, LOCAL_closedLoopLagSearch_pL_CGBuf_ADDR, 2);
        //swLTPEnergy in REG5
        VoC_add32_rd(REG45, ACC0, CONST_0x00008000L_ADDR);

        VoC_lw16i_short(REG4, 0,DEFAULT);
        VoC_bngt16_rd(CLOSEDLOOPLAG100, REG5, CONST_0x7ff_ADDR);
        VoC_lw16i_short(REG4, 1, DEFAULT);
        VoC_bngt16_rd(CLOSEDLOOPLAG100, REG5, CONST_0x1fff_ADDR);
        VoC_lw16i_short(REG4, 2, DEFAULT);
CLOSEDLOOPLAG100:

        // store initial values
        VoC_movreg32(RL6, REG01, DEFAULT);
        VoC_lw16i_set_inc(REG2, LOCAL_closedLoopLagSearch_ppswTVect_ADDR, 1);
//  in STK16, pswLtpShift,&pVect,RA);
        VoC_push16(REG4,DEFAULT);


        VoC_loop_r(1, REG7)

        VoC_lw16_sd(REG3, 4, DEFAULT);
        VoC_lw32z(ACC0,DEFAULT);
        VoC_lw32z(ACC1,IN_PARALLEL);

        VoC_loop_i_short(40,DEFAULT)
        VoC_startloop0
        VoC_lw16_p(REG5, REG2, DEFAULT);
        VoC_shr16_rr(REG5, REG4, DEFAULT);
        // Compute energy on ACC0
        VoC_mac32_rr(REG5, REG5, DEFAULT);
        // Compute Cross correlation on ACC1
        VoC_mac32inc_rp(REG5, REG3, IN_PARALLEL);
        // store shifted value for future use
        VoC_sw16inc_p(REG5, REG2, DEFAULT);

        VoC_endloop0

        VoC_NOP();
        VoC_sw32inc_p(ACC0, REG1, DEFAULT);
        VoC_sw32inc_p(ACC1, REG0, DEFAULT);

        VoC_endloop1;

//2
        // restitute initial values
        VoC_movreg32(REG01, RL6, DEFAULT);
        VoC_lw32z(REG23, DEFAULT);

        VoC_lw32z(ACC0,DEFAULT);
        VoC_lw32z(ACC1,IN_PARALLEL);

        VoC_loop_r_short(REG7, DEFAULT)
        VoC_startloop0
        VoC_lw32inc_p(REG45, REG0, DEFAULT);
        VoC_lw32inc_p(RL7, REG1, DEFAULT);
        VoC_bnltz32_r(CLOSEDLOOPLAG901, REG45);
        VoC_sub32_rr(REG45, REG23, REG45, DEFAULT);
CLOSEDLOOPLAG901:
        VoC_or32_rr(ACC0, REG45, DEFAULT);
        VoC_or32_rr(ACC1, RL7, DEFAULT);
        VoC_endloop0;

        /************************************************************
         *Function: CII_NORM_L_ACC0
         *input: ACC0;
         *return:REG1;
         ***********************************************************/
        VoC_jal(CII_NORM_L_ACC0);
        VoC_movreg32(ACC0, ACC1, DEFAULT);
        VoC_sub16_dr(REG5,CONST_0_ADDR,REG1);
//  VoC_movreg16(REG5,REG1,DEFAULT);//swCCShiftCnt in REG5

        VoC_jal(CII_NORM_L_ACC0);
        //   VoC_movreg16(REG4,REG1,DEFAULT);//swCGShiftCnt in REG4
        VoC_sub16_dr(REG4,CONST_0_ADDR,REG1);
        // restitute initial values
        VoC_movreg32(REG01, RL6, DEFAULT);

        VoC_lw16i_set_inc(REG2, LOCAL_closedLoopLagSearch_pswCCBuf_ADDR, 1);
        VoC_lw16i_set_inc(REG3, LOCAL_closedLoopLagSearch_pswCGBuf_ADDR, 1);
        VoC_lw32_d(ACC0,CONST_0x00008000L_ADDR);


        VoC_lw16i_short(INC1,2,DEFAULT);
        // save pointers
        VoC_push32(REG23,DEFAULT);

        VoC_loop_r_short(REG7, DEFAULT)
        VoC_startloop0
        VoC_lw32inc_p(RL7, REG0, DEFAULT);
        VoC_lw32inc_p(RL6, REG1, DEFAULT);
        VoC_shr32_rr(RL7, REG5, DEFAULT);
        VoC_shr32_rr(RL6, REG4, IN_PARALLEL);
        VoC_add32_rr(RL7, RL7, ACC0, DEFAULT);
        VoC_add32_rr(RL6, RL6, ACC0, DEFAULT);
        // no conflict on REG7 due to pipe
        VoC_sw16inc_p(RL7_HI, REG2, DEFAULT);
        VoC_sw16inc_p(RL6_HI, REG3, DEFAULT);
        VoC_endloop0

        // restitute pointers
        VoC_pop32(REG23,DEFAULT);

//      VoC_lw16i(REG5, LOCAL_closedLoopLagSearch_swCCMax_ADDR);
//      VoC_lw16i(REG4, LOCAL_closedLoopLagSearch_swGMax_ADDR);


        /**********************************************************
        // REG0 : i
        // REG1 : maxCCGIndex
        // REG2 : &pswCIn[0]             INC2 = 1;
        // REG3 : &pswGIn[0],   swCC     INC3 = 1;
        // REG6 : swGMax
        // REG7 : swNum,    swCCMax
        ***********************************************************/

        VoC_jal(CII_maxCCOverGWithSign_opt_paris);

        //siLagOffset in REG1
        //VoC_lw16_d(REG7, LOCAL_MAXCCOVERGWITHSIGN_maxCCGIndex_ADDR);
        VoC_lw16_sd(REG2,7,DEFAULT);
        VoC_add16_rr(REG6, REG1, REG2,DEFAULT);

        VoC_NOP();
        // VoC_lw16i(REG7, LOCAL_closedLoopLagSearch_siLagCode_ADDR);
        VoC_lw16_p(REG6, REG6, DEFAULT);

        //VoC_sw16_d(REG6, QUANTLAG_swRawLag_ADDR);
        //VoC_sw16_d(REG7, QUANTLAG_pswCode_ADDR_ADDR);

        /*****************************************
        // INPUT
        // REG6 : swRawLag
        // OUTPUT
        // REG2 : *pswCode
        // REG3 : return
        *******************************************/
        VoC_jal(CII_quantLag_opt_paris);


        VoC_pop16(REG5,DEFAULT);


        /****************************************************************************/
        VoC_sw16_d(REG6,LOCAL_sfrmAnalysis_swLag_ADDR);
        VoC_sw16_d(REG5,LOCAL_sfrmAnalysis_swLtpShift_ADDR);
    }
//3

    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // pswWSpeech       *
    // siNumLags
    // pswLagList
    // swPitch      *
    // swHNWCoef        *
    // pswWSVec     *
    // swPnShift        *
    // siHnwNum     *
    // pswWPVec     *


    // no necessary to check if (swVoicingMode > 0)
    // because we are already in a case (swVoicingMode > 0)

// A

    VoC_lw16_sd(REG1,25,DEFAULT);
    VoC_lw16_d(REG3,sfrmAnalysis_psiLagCode_ADDR_ADDR);

    VoC_lw16_d(REG4,LOCAL_sfrmAnalysis_siPrevLagCode_ADDR);
    // siPrevLagCode = siCode
    VoC_sw16_d(REG2,LOCAL_sfrmAnalysis_siPrevLagCode_ADDR);

    VoC_bez16_r(sfrmAnalysis_003,REG1);
    // sub(siCode, siPrevLagCode)
    VoC_sub16_rr(REG2,REG2,REG4,DEFAULT);
    // add(res, DELTA_LEVELS / 2);
    VoC_add16_rd(REG2,REG2,CONST_8_ADDR);

sfrmAnalysis_003:

    // swRunningLag in REG4
    // return swTempJj in REG0
    // return swTempIp in REG1
    // other used regs : ACC0

    // swPitch in REG4
    VoC_lw16_sd(REG4,5,DEFAULT);

    // *psiLagCode = siCode;  OR
    // *psiLagCode = add(sub(siCode, siPrevLagCode), DELTA_LEVELS / 2);
    VoC_sw16_p(REG2,REG3,DEFAULT);

    VoC_jal(CII_get_ipjj_opt_paris);
    // siIntPitch in REG1
    // siRemainder in REG0


    // ppsrCGIntFilt[0][siRemainder]
    VoC_lw16i_set_inc(REG2,TABLE_SP_ROM_ppsrCGIntFilt_ADDR,6);
    VoC_add16_rr(REG2,REG2,REG0,DEFAULT);
    // preload constant
    VoC_lw16i(REG5,42);

    // pswHNWCoefs
    VoC_lw16i_set_inc(REG0,LOCAL_sfrmAnalysis_pswHNWCoefs_ADDR,1);

    // swHNWCoef
    VoC_lw16_sd(REG3,4,DEFAULT);

    // store pswHNWCoefs (swPitch no longer needed)
    VoC_sw16_sd(REG0,5,DEFAULT);


    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // pswWSpeech       *
    // siNumLags
    // pswLagList
    // pswHNWCoefs      *
    // swHNWCoef        *
    // pswWSVec     *
    // swPnShift        *
    // siHnwNum     *
    // pswWPVec     *

    VoC_lw16inc_p(REG7,REG2,DEFAULT);

    VoC_loop_i_short(REG6,DEFAULT);
    VoC_lw16i_short(REG4,0xfffd,IN_PARALLEL);
    VoC_startloop0

    VoC_sub16_dr(REG6,CONST_0_ADDR,REG7);
    VoC_multf32_rr(REG67,REG6,REG3,DEFAULT);
    VoC_NOP();
    VoC_add32_rd(REG67,REG67,CONST_0x00008000L_ADDR);
    VoC_lw16inc_p(REG7,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;

    // siHnwOffset
    VoC_sub16_rr(REG4,REG4,REG1,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG1,DEFAULT);


// B
    // siHnwOffset
    VoC_sw16_sd(REG4,6,DEFAULT);
    // siHnwNum
    VoC_sw16_sd(REG5,1,DEFAULT);


    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // pswWSpeech       *
    // siNumLags
    // siHnwOffset      *
    // pswHNWCoefs      *
    // swHNWCoef        *
    // pswWSVec     *
    // swPnShift        *
    // siHnwNum     *
    // pswWPVec     *


    // REG0 :           INC0 = 1
    // REG1 :           INC2 = 1
    // REG2 :           INC2 = 6
    // REG3 :           INC3 = -1
    // REG4 : siHnwOffset
    // REG5 :
    // REG6 :
    // REG7 :

    // pswWSpeech
    VoC_lw16_sd(REG2,8,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    // pswWSVec
    VoC_lw16_sd(REG3,3,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    //&pswWSpeech[-1]
    VoC_sub16_rr(REG1,REG2,REG5,DEFAULT);
    VoC_lw16i(REG6,40);

    // pswHNWCoefs
    VoC_lw16_sd(REG0,5,DEFAULT);

    VoC_lw16i_short(INC0,-1,DEFAULT);
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);
    VoC_lw16i_short(INC1,1,DEFAULT);
    // REG0 : pswInCoef,    INC0 = 1  (parameter 3) : pswHNWCoefs
    // REG1 : pswState, INC1 = 1  (parameter 2) : &pswWSpeech[-1]
    // REG2 : pswInSample,  INC2 = 1  (parameter 0) : pswWSpeech
    // REG3 : pswOutSample, INC3 = -1 (parameter 1) : pswWSVec
    // REG4 : iStateOffset        (parameter 4) : siHnwOffset
    // REG5 : swZeroState         (parameter 5) : 1
    // REG6 : iNumSamples         (parameter 6) : 40

    // siHnwOffset & 1
    VoC_push32(REG45,IN_PARALLEL);

    VoC_jal(CII_hnwFilt_opt);
    // INC values are not modified within the function

    // siHnwOffset & 1
    VoC_pop32(REG45,DEFAULT);

    // pswTempVec
    VoC_lw16_sd(REG2,14,DEFAULT);
    VoC_lw16i(REG6,40);
    // pswHNWCoefs
    VoC_lw16_sd(REG0,5,DEFAULT);

    // pswTempVec
    VoC_movreg16(REG3,REG2,IN_PARALLEL);

    // pswHNWState
    VoC_lw16i_set_inc(REG1,GLOBAL_SFRM_pswHNWState_ADDR,1);
    VoC_add16_rd(REG1,REG1,CONST_144_ADDR);

    // REG0 : pswInCoef,    INC0 = -1  (parameter 3) : pswHNWCoefs
    // REG1 : pswState, INC1 = 1  (parameter 2) : &pswHNWState[144]
    // REG2 : pswInSample,  INC2 = 1  (parameter 0) : pswTempVec
    // REG3 : pswOutSample, INC3 = -1 (parameter 1) : pswTempVec
    // REG4 : iStateOffset        (parameter 4) : siHnwOffset
    // REG5 : swZeroState         (parameter 5) : 1
    // REG6 : iNumSamples         (parameter 6) : 40



    // INC values are not modified within the function
    VoC_jal(CII_hnwFilt_opt);

    VoC_lw16_sd(REG2,14,DEFAULT);
    VoC_lw16_sd(REG0,2,DEFAULT);
    VoC_lw16_sd(REG1,3,DEFAULT);


    // REG0 : swPnShift ,   INC0 = -1
    // REG1 : pswWSVec  ,   INC1 = 1
    // REG2 : pswTempVec    ,   INC2 = 1
    // REG3 :       ,   INC3 = -1
    // REG4 :
    // REG5 :
    // REG6 :
    // REG7 :
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    VoC_loop_i_short(40,DEFAULT)
    VoC_startloop0

    VoC_sub16_pr(REG3,REG1,REG6,DEFAULT);
    VoC_shr16_rr(REG3,REG0,DEFAULT);
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG3,REG1,DEFAULT);
    exit_on_warnings=ON;

    VoC_endloop0;

// C

    // swOrigLagIn in REG6
    VoC_lw16_d(REG6,LOCAL_sfrmAnalysis_swLag_ADDR);
    // pswLtpStateOut
    VoC_lw16_sd(REG5,10,DEFAULT);

    VoC_jal(CII_fp_ex_opt);

    // pswLtpStateOut
    VoC_lw16_sd(REG1,10,DEFAULT);
    // pswPVec
    VoC_lw16_sd(REG0,12,DEFAULT);


    // REG0 :   ,   INC0 = 1
    // REG1 :   ,   INC1 = 1
    // REG2 :   ,   INC2 = 1
    // REG3 :   ,   INC3 = 1


    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16inc_p(REG2,REG1,DEFAULT);
    VoC_loop_i_short(40,DEFAULT)

    VoC_startloop0
    VoC_lw16inc_p(REG2,REG1,DEFAULT);
    exit_on_warnings=OFF;
    // sw before RE2 update
    VoC_sw16inc_p(REG2,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;


    // REG3 : pswFiltOut    (pswWPVec : param 2)
    VoC_lw16_sd(REG3,0,DEFAULT);
    // REG5 : pswInput  (pswPVec :  param 0)
    VoC_lw16_sd(REG5,12,DEFAULT);
    // REG6 : pswCoef   (pswHCoefs: param 1)
    VoC_lw16_sd(REG6,20,DEFAULT);


    VoC_jal(CII_lpcZsIir_opt);

    // siHnwOffset
    VoC_lw16_sd(REG4,6,DEFAULT);
    // pswWPVec
    VoC_lw16i_set_inc(REG3,LOCAL_sfrmAnalysis_pswWPVec_ADDR,-1);
    // pswWPVec
    VoC_movreg16(REG2,REG3,DEFAULT);
    // siHnwNum
    VoC_lw16_sd(REG6,1,IN_PARALLEL);
    // 0
    VoC_lw16i_short(REG5,0,DEFAULT);
    // pswHNWCoefs
    VoC_lw16_sd(REG0,5,IN_PARALLEL);
    // NULL
    VoC_lw16i_short(REG1,0,DEFAULT);



    VoC_lw16i_short(INC0,-1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    // REG0 : pswInCoef,    INC0 = -1  (parameter 3) : pswHNWCoefs
    // REG1 : pswState, INC1 = 1  (parameter 2) : NULL
    // REG2 : pswInSample,  INC2 = 1  (parameter 0) : pswWPVec
    // REG3 : pswOutSample, INC3 = -1 (parameter 1) : pswWPVec
    // REG4 : iStateOffset        (parameter 4) : siHnwOffset
    // REG5 : swZeroState         (parameter 5) : 0
    // REG6 : iNumSamples         (parameter 6) : siHnwNum


    VoC_bngtz16_r(sfrmAnalysis_004,REG6);
    VoC_jal(CII_hnwFilt_opt);
sfrmAnalysis_004:

    // pswPVec
    VoC_lw16_sd(REG0,12,DEFAULT);
    // swLtpShift
    VoC_lw16_d(REG4,LOCAL_sfrmAnalysis_swLtpShift_ADDR);
    // pswWPVec
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_loop_i_short(40,DEFAULT)
    VoC_startloop0
    VoC_lw16_p(REG2,REG0,DEFAULT);
    VoC_lw16_p(REG3,REG1,DEFAULT);
    VoC_shr16_rr(REG2,REG4,DEFAULT);
    VoC_shr16_rr(REG3,REG4,DEFAULT);
    VoC_sw16inc_p(REG2,REG0,DEFAULT);
    VoC_sw16inc_p(REG3,REG1,DEFAULT);
    VoC_endloop0;

sfrmAnalysis_011:


// D
    // REG6 : siNumBasisVecs
    // REG0 : i

    VoC_lw16i_short(REG6,7,DEFAULT);
    VoC_lw16i(REG5,TABLE_SP_ROM_pppsrUvCodeVec_ADDR);
    VoC_bngtz16_d(sfrmAnalysis_012,LOCAL_EN_SPEECHENCODER_siUVCode_ADDR);
    VoC_lw16i_short(REG6,9,DEFAULT);
    VoC_lw16i(REG5,TABLE_SP_ROM_pppsrVcdCodeVec_ADDR);
sfrmAnalysis_012:

    // i
    VoC_lw16i_short(REG0,0,DEFAULT);

    // push siNumBasisVecs
    VoC_push16(REG6,DEFAULT);
    VoC_push16(REG5,DEFAULT);



sfrmAnalysis_loop0:

    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // pswWSpeech       *
    //
    // siHnwOffset      *
    // pswHNWCoefs      *
    // swHNWCoef        *
    // pswWSVec     *
    // swPnShift        *
    // siHnwNum     *
    // pswWPVec     *
    // siNumBasisVecs   *
    // Vcd_or_Uv        *


    // all parameters are the same except
    // pppsrVcdCodeVec & pppsrUvCodeVec

    // REG3 : param 2
    // REG6 : param 1 (pswHCoefs)
    VoC_lw16_sd(REG6,22,DEFAULT);
    VoC_mult16_rd(REG1,REG0,CONST_40_ADDR);
    VoC_lw16i_set_inc(REG3,LOCAL_sfrmAnalysis_pswWBasisVecs_ADDR,1);
    VoC_add16_rr(REG3,REG3,REG1,DEFAULT);



    // REG5 : param 0 (Vcd_or_Uv)
    VoC_lw16_sd(REG5,0,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG1,DEFAULT);

    // push i
    VoC_push16(REG0,IN_PARALLEL);

    // push &pswWBasisVecs[i * S_LEN]
    VoC_push16(REG3,DEFAULT);


    VoC_jal(CII_lpcZsIir_opt);

    // pop &pswWBasisVecs[i * S_LEN]
    VoC_pop16(REG3,DEFAULT);
    VoC_lw16i_short(INC3,-1,DEFAULT);
    VoC_movreg16(REG2,REG3,DEFAULT);

    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // pswWSpeech       *
    //
    // siHnwOffset      *
    // pswHNWCoefs      *
    // swHNWCoef        *
    // pswWSVec     *
    // swPnShift        *
    // siHnwNum     *
    // pswWPVec     *
    // siNumBasisVecs   *
    // Vcd_or_Uv
    // i

    // siHnwNum
    VoC_lw16_sd(REG6,4,IN_PARALLEL);

    VoC_bngtz16_r(sfrmAnalysis_009,REG6);

    // pswHNWCoefs
    VoC_lw16_sd(REG0,8,DEFAULT);
    // NULL
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    // siHnwOffset
    VoC_lw16_sd(REG4,9,DEFAULT);
    // 0
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    // REG0 : INC0 = -1     (par 3) : pswHNWCoefs
    // REG1 : INC1 = 1      (par 2) : NULL
    // REG2 : INC2 = 1      (par 0) : &pswWBasisVecs[i * S_LEN]
    // REG3 : INC3 = -1     (par 1) : &pswWBasisVecs[i * S_LEN]
    // REG4 :       (par 4) : siHnwOffset
    // REG5 :       (par 5) : 0
    // REG6 :       (par 6) : siHnwNum

    VoC_lw16i_short(INC0,-1,DEFAULT);
    VoC_jal(CII_hnwFilt_opt);

sfrmAnalysis_009:

    VoC_lw16i_short(INC0,1,DEFAULT);

    // pop i
    VoC_pop16(REG0,DEFAULT);
    // siNumBasisVecs
    VoC_lw16_sd(REG1,1,DEFAULT);
    // i++
    VoC_inc_p(REG0,DEFAULT);
    // loop while i < siNumBasisVecs)
    VoC_bgt16_rr(sfrmAnalysis_loop0,REG1,REG0);


// E
    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // pswWSpeech       *
    //
    // siHnwOffset      *
    // pswHNWCoefs      *
    // swHNWCoef        *
    // pswWSVec     *
    // swPnShift        *
    // siHnwNum     *
    // pswWPVec     *
    // siNumBasisVecs   *
    // Vcd_or_Uv        *


    /* If voiced, make the H(z)C(z)-weighted basis vectors
     * orthogonal to the H(z)C(z)-weighted adaptive codebook vector */

    // pswWPVec
    VoC_lw16i_set_inc(REG3,LOCAL_sfrmAnalysis_pswWBasisVecs_ADDR,1);

    // pswWBasisVecs
    VoC_lw16i_set_inc(REG0,LOCAL_sfrmAnalysis_pswWPVec_ADDR,1);


    // push pswWBasisVecs
    VoC_push16(REG3,DEFAULT);



    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // pswWSpeech       *
    //
    // siHnwOffset      *
    // pswHNWCoefs      *
    // swHNWCoef        *
    // pswWSVec     *
    // swPnShift        *
    // siHnwNum     *
    // pswWPVec     *
    // siNumBasisVecs   *
    // Vcd_or_Uv        *
    // pswWBasisVecs    *


    VoC_bngtz16_d(CII_sfrmAnalysis_100, LOCAL_EN_SPEECHENCODER_siUVCode_ADDR)

    // REG5 : param 0
    // REG3 : param 2, INC3 = 1
    // REG0 : param 1

    // siNumBasisVecs
    VoC_lw16_sd(REG5,2,DEFAULT);
    VoC_jal(CII_decorr_opt);

CII_sfrmAnalysis_100:

    /* Do the vector-sum codebook search on the H(z)C(z)-weighted, */
    /* orthogonalized basis vectors                                */

    // siNumBasisVecs
    VoC_lw16_sd(REG4,2,DEFAULT);
    // pswWBasisVecs
    VoC_lw16_sd(REG0,0,DEFAULT);
    // pswWSVec
    VoC_lw16_sd(REG1,6,DEFAULT);


    /****************************************************************
    // INPUT_PARAMETERS:
    // REG4 : siNumBasis
    // REG0 : pswWBasisVecs, INC0 = 1
    // REG1 : pswWInput,     INC1 = 1
    // OUTPUT :
    // REG0 : sw1
    *****************************************************************/

    // INPUT_PARAMETERS:
    // REG4 : siNumBasis
    // REG0 : pswWBasisVecs, INC0 = 1
    // REG1 : pswWInput,     INC1 = 1
    VoC_jal(CII_v_srch_opt);


    // OUTPUT :
    // REG0 : sw1

    // *psiVSCode1 in stack
    VoC_sw16_sd(REG0,19,DEFAULT);

    // siNumBasisVecs
    VoC_lw16_sd(REG4,2,DEFAULT);


    /* Construct the chosen vector-sum codebook vector
     * from basis vectors */

    // *psiVSCode1
    VoC_movreg16(REG5,REG0,IN_PARALLEL);

    // pswBitArray
    VoC_lw16_sd(REG0,12,DEFAULT);

    // INPUT :
    // REG0 : pswVectOut, INC = 1       (param2)
    // REG4 : siNumBits         (param1)
    // REG5 : swCodeWord            (param0)

    VoC_jal(CII_b_con_opt);

    // Vcd_or_Uv
    VoC_lw16_sd(REG0,1,DEFAULT);

    // pswBitArray
    VoC_lw16_sd(REG2,12,DEFAULT);
    VoC_lw16i_short(INC0,40,IN_PARALLEL);
    // siNumBasisVecs
    VoC_lw16_sd(REG4,2,DEFAULT);

    // ppswVselpEx
    VoC_lw16i_set_inc(REG1,LOCAL_sfrmAnalysis_ppswVselpEx_ADDR,2);


    // INPUT :
    // REG0 : pswBVects,        INC0 = 40   (param0)
    // REG1 : pswOutVect,       INC1 = 2    (param1)
    // REG2 : pswBitArray,      INC2 = 1    (param2)
    // REG4 : siNumBVctrs               (param3)

    VoC_jal(CII_v_con_opt);

    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // pswWSpeech       *
    //
    // siHnwOffset      *
    // pswHNWCoefs      *
    // swHNWCoef        *
    // pswWSVec     *
    // swPnShift        *
    // siHnwNum     *
    // pswWPVec     *
    // siNumBasisVecs   *
    // Vcd_or_Uv
    // pswWBasisVecs    *


    // default (swVoicingMode != 0) *psiVSCode2 = 0
    VoC_lw16i_short(REG0,0,DEFAULT);

    // ppswWVselpEx
    VoC_lw16i(REG1, LOCAL_sfrmAnalysis_ppswVselpEx_ADDR);

    // *psiVSCode2 = 0
    VoC_sw16_sd(REG0,18,DEFAULT);

    // save ppswVselpEx
    VoC_sw16_sd(REG1,1,DEFAULT);



    VoC_bnez16_d(CII_sfrmAnalysis_300_END,LOCAL_EN_SPEECHENCODER_siUVCode_ADDR)

    /* Construct the H(z)-weighted 1st-codebook vector */
    /*-------------------------------------------------*/


    // pswWBasisVecs
    VoC_lw16_sd(REG0,0,DEFAULT);

    VoC_lw16i(REG1,LOCAL_sfrmAnalysis_ppswWVselpEx_ADDR);

    // pswBitArray
    VoC_lw16_sd(REG2,12,DEFAULT);

    // siNumBasisVecs
    VoC_lw16_sd(REG4,2,DEFAULT);

    // INPUT :
    // REG0 : pswBVects,        INC0 = 40   (param0)
    // REG1 : pswOutVect,       INC1 = 2    (param1)
    // REG2 : pswBitArray,      INC2 = 1    (param2)
    // REG4 : siNumBVctrs               (param3)

    VoC_jal(CII_v_con_opt);

    // siNumBasisVecs
    VoC_lw16_sd(REG7,2,DEFAULT);
    // pppsrUvCodeVec[1][0]
    VoC_lw16i(REG0,TABLE_SP_ROM_pppsrUvCodeVec_ADDR+40*7);

    VoC_lw16i_short(REG4,0,DEFAULT);

    // save pppsrUvCodeVec[1][0]
    VoC_push16(REG0,DEFAULT);


    /* Filter the 2nd basis vector set through H(z) with zero state */
    /*--------------------------------------------------------------*/

    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // pswWSpeech       *
    //
    // siHnwOffset      *
    // pswHNWCoefs      *
    // swHNWCoef        *
    // pswWSVec     *
    // swPnShift        *
    // siHnwNum     *
    // pswWPVec     *
    // siNumBasisVecs   *
    // ppswVselpEx      *
    // pswWBasisVecs    *
    // pppsrUvCodeVec[1][0] *



sfrmAnalysis_LOOP_100_START:

    VoC_mult16_rd(REG1,REG4,CONST_40_ADDR);
    // REG3 : param 2 (&pswWBasisVecs[i * S_LEN])
    VoC_lw16_sd(REG3,1,DEFAULT);

    VoC_add16_rr(REG3,REG3,REG1,DEFAULT);

    // REG6 : param 1 (pswHCoefs)
    VoC_lw16_sd(REG6,24,IN_PARALLEL);

    // REG5 : param 0 (pppsrUvCodeVec[1][i])
    VoC_lw16_sd(REG5,0,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG1,DEFAULT);


    // REG7 unused in function
    VoC_jal(CII_lpcZsIir_opt);


    // (siNumBasisVecs - i) --
    VoC_add16_rd(REG4, REG4, CONST_1_ADDR);


    VoC_bne16_rr(sfrmAnalysis_LOOP_100_START,REG7,REG4);



    /* Make the 2nd set of H(z)-weighted basis vectors
     * orthogonal to the H(z)-weighted 1st-codebook vector  */

    // INC0 = 40
    // INC1 = 2
    // INC2 = 1
    // INC3 = 1

    // INPUT
    // REG5 : param 0
    // REG3 : param 1, INC3 = 1
    // REG0 : param 2

    // siNumBasisVecs
    VoC_lw16_sd(REG5,3,DEFAULT);

    // ppswWVselpEx (INC3 = 1 is not modified)
    VoC_lw16i(REG0,LOCAL_sfrmAnalysis_ppswWVselpEx_ADDR);

    // pswWBasisVecs
    VoC_lw16_sd(REG3,1,DEFAULT);

    /************************************************************
    *
    * INPUT:
    *       REG0  decorr_pswGivenVect_ADDR
    *       REG3  decorr_pswVects_ADDR         INC3=1
    *       REG5  decorr_iNumVects
    *
    * NO OUTPUT
    ************************************************************/

    VoC_jal(CII_decorr_opt);



    /*  Do the vector-sum codebook search on the H(z)-weighted, */
    /* orthogonalized, 2nd basis vector set                    */

    // siNumBasisVecs
    VoC_lw16_sd(REG4,3,DEFAULT);

    // pswWBasisVecs
    VoC_lw16i_set_inc(REG0, LOCAL_sfrmAnalysis_pswWBasisVecs_ADDR,1);

    // pswWSVec
    VoC_lw16i_set_inc(REG1, LOCAL_sfrmAnalysis_pswWSVec_ADDR,1);


    // INPUT_PARAMETERS:
    // REG4 : siNumBasis
    // REG0 : pswWBasisVecs, INC0 = 1
    // REG1 : pswWInput,     INC1 = 1
    VoC_jal(CII_v_srch_opt);

    // OUTPUT :
    // REG0 : sw1

    /* Construct the chosen vector-sum codebook vector
     * from the 2nd set of basis vectors */

    // *psiVSCode2 in stack = return(v_srch)
    VoC_sw16_sd(REG0,19,DEFAULT);

    // siNumBasisVecs
    VoC_lw16_sd(REG4,3,DEFAULT);



    // *psiVSCode2
    VoC_movreg16(REG5,REG0,IN_PARALLEL);

    // pswBitArray
    VoC_lw16_sd(REG0,13,DEFAULT);


    // INPUT :
    // REG0 : pswVectOut, INC = 1       (param2)
    // REG4 : siNumBits         (param1)
    // REG5 : swCodeWord            (param0)

    VoC_jal(CII_b_con_opt);


    // pppsrUvCodeVec[1][0]
    VoC_pop16(REG0,DEFAULT);

    // ppswVselpEx[1]
    VoC_lw16i_set_inc(REG1,LOCAL_sfrmAnalysis_ppswVselpEx_ADDR+40,2);

    // pswBitArray
    VoC_lw16_sd(REG2,12,DEFAULT);
    VoC_lw16i_short(INC0,40,IN_PARALLEL);
    // siNumBasisVecs
    VoC_lw16_sd(REG4,2,DEFAULT);

    // INPUT :
    // REG0 : pswBVects,        INC0 = 40   (param0)
    // REG1 : pswOutVect,       INC1 = 2    (param1)
    // REG2 : pswBitArray,      INC2 = 1    (param2)
    // REG4 : siNumBVctrs               (param3)

    VoC_jal(CII_v_con_opt);


CII_sfrmAnalysis_300_END:




    /* Filter the 2nd basis vector set
     * through H(z) with zero state */


    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // pswWSpeech       *
    //
    // siHnwOffset      *
    // pswHNWCoefs      *
    // swHNWCoef        *
    // pswWSVec     *
    // swPnShift        *
    // siHnwNum     *
    // pswWPVec     *
    // siNumBasisVecs   *
    // ppswVselpEx      *
    // pswWBasisVecs    *


// F

    /***************************************************
    INPUT:
          REG3 : pswFiltOut
          REG5 : pswInput
          REG6 : pswCoef
    OUTPUT:
          REG3 : pswFiltOut
    ***************************************************/

    // ppswVselpEx
    VoC_lw16_sd(REG5,14,DEFAULT);
    /* Filter the 1st-codebook vector through H(z)
     * (also through C(z) if appropriate) */
    VoC_lw16i(REG3,LOCAL_sfrmAnalysis_ppswWVselpEx_ADDR);


    // pswHCoefs
    VoC_lw16_sd(REG6,23,DEFAULT);

    // ppswWVselpEx
    VoC_sw16_sd(REG3,1,DEFAULT);

    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // pswWSpeech       *
    //
    // siHnwOffset      *
    // pswHNWCoefs      *
    // swHNWCoef        *
    // pswWSVec     *
    // swPnShift        *
    // siHnwNum     *
    // pswWPVec     *
    // siNumBasisVecs   *
    // ppswVselpEx      *
    // pswWBasisVecs    *


    // REG3 : param 2
    // REG6 : param 1
    // REG5 : param 0
    VoC_jal(CII_lpcZsIir_opt);

    // siHnwNum
    VoC_lw16_sd(REG6,4,DEFAULT);


    VoC_lw16i_short(INC0,-1,DEFAULT);
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);
    VoC_lw16i_short(INC1,1,DEFAULT);


    VoC_bngtz16_r(CII_sfrmAnalysis_400,REG6)

    // pswHNWCoefs
    VoC_lw16_sd(REG0,8,DEFAULT);
    // ppswWVselpEx[0]
    VoC_lw16_sd(REG2,1,DEFAULT);
    // 0
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    // ppswWVselpEx[0]
    VoC_lw16_sd(REG3,1,DEFAULT);
    // siHnwOffset
    VoC_lw16_sd(REG4,9,DEFAULT);


    // REG0 : INC0 = -1     (par 3) : pswHNWCoefs
    // REG1 : INC1 = 1      (par 2) : NULL
    // REG2 : INC2 = 1      (par 0) : ppswWVselpEx[0]
    // REG3 : INC3 = -1     (par 1) : ppswWVselpEx[0]
    // REG4 :       (par 4) : siHnwOffset
    // REG5 :       (par 5) : 0
    // REG6 :       (par 6) : siHnwNum
    VoC_jal(CII_hnwFilt_opt);
CII_sfrmAnalysis_400:


    VoC_lw16i_short(INC3,1,DEFAULT);
    /* Filter the 2nd-codebook vector through H(z) */
    VoC_bnez16_d(CII_sfrmAnalysis_500, LOCAL_EN_SPEECHENCODER_siUVCode_ADDR)


    // ppswWVselpEx
    VoC_lw16_sd(REG3,1,DEFAULT);
    VoC_lw16i(REG7,40);

    // ppswVselpEx
    VoC_lw16_sd(REG5,14,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);

    // pswHCoefs
    VoC_lw16_sd(REG6,23,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG7,IN_PARALLEL);

    // REG3 : param 2
    // REG6 : param 1
    // REG5 : param 0
    /***************************************************
    INPUT:
    REG3 : pswFiltOut
    REG5 : pswInput
    REG6 : pswCoef
    OUTPUT:
    REG3 : pswFiltOut
    ***************************************************/

    VoC_jal(CII_lpcZsIir_opt);

CII_sfrmAnalysis_500:

    /* Get the square-root of the ratio of residual
    * energy to excitation vector energy for each of
    * the excitation sources */


    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_bngtz16_d(CII_sfrmAnalysis_600, LOCAL_EN_SPEECHENCODER_siUVCode_ADDR)

    // pswPVec
    VoC_lw16_sd(REG0,15,DEFAULT);
    // snsSqrtRs
    VoC_lw16_sd(REG2,22,DEFAULT);
    // snsRs00
    VoC_lw16_sd(REG3,24,DEFAULT);

    //////////////////////////////////////
    // INPUT :
    // REG0 : pswExcitation,    INC0 = 2
    // REG2 : &snsSqrtRs,       INC2 = 1
    // REG3 : &snsSqrtRsRr;     INC3 = 1
    //////////////////////////////////////

    VoC_jal(CII_rs_rrNs_opt);

CII_sfrmAnalysis_600:


    // ppswVselpEx
    VoC_lw16_sd(REG0,14,DEFAULT);

    // snsSqrtRs
    VoC_lw16_sd(REG2,22,DEFAULT);

    // snsRs11
    VoC_lw16_sd(REG3,25,DEFAULT);

    ///////////////////////////////////////
    // INPUT :
    // REG0 : pswExcitation,    INC0 = 2
    // REG2 : &snsSqrtRs,       INC2 = 1
    // REG3 : &snsSqrtRsRr;     INC3 = 1
    ///////////////////////////////////////


    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_jal(CII_rs_rrNs_opt);

    VoC_bnez16_d(CII_sfrmAnalysis_700, LOCAL_EN_SPEECHENCODER_siUVCode_ADDR)

    // ppswVselpEx
    VoC_lw16_sd(REG0,14,DEFAULT);
    VoC_lw16i(REG7,40);
    // snsSqrtRs
    VoC_lw16_sd(REG2,22,DEFAULT);

    // ppswVselpEx[1]
    VoC_add16_rr(REG0,REG0,REG7,IN_PARALLEL);

    // snsRs22
    VoC_lw16_sd(REG3,26,DEFAULT);

    // INPUT :
    // REG0 : param0,   INC0 = 2
    // REG2 : param1,   INC2 = 1
    // REG3 : param2,   INC3 = 1

    VoC_jal(CII_rs_rrNs_opt);

CII_sfrmAnalysis_700:

    /* Determine the vector-quantized gains
     * for each of the excitations */

    /************************************************************/
// the parameters below are passed by registers
// REG7 swUVCode
// REG0 pswWInput_ADDR   // INC0 = 1;
// REG1 pswWVSVec1_ADDR  // INC1 = 1;
// REG2 pswWVSVec2_ADDR  // INC2 = 1;
// REG3 pswWLTPVec_ADDR  // INC3 = 1;
// REG4 snsRs22_ADDR
// REG5 snsRs11_ADDR
// REG6 snsRs00_ADDR

// return in REG5
    /************************************************************/

    {


#if 0

        voc_short   g_quant_vl_swWIShift_ADDR                   ,x      //1 short
        voc_short   LOCAL_g_quant_vl_siNormMin_ADDR             ,x      //1 short
        voc_short   LOCAL_g_quant_vl_ErrorTerm_ADDR             ,12,x       //[12]shorts
        voc_short   LOCAL_gainTweak_siNorm_ADDR                 ,2,x         //[2]shorts
        voc_short   LOCAL_gainTweak_terms_ADDR                  ,10,x        //[10]shorts
#endif








        // pswWSVec
        VoC_lw16_sd(REG0,6,DEFAULT);

        // ppswWVselpEx[0]
        VoC_lw16_sd(REG1,1,DEFAULT);
        VoC_lw16i(REG7,40);

        // ppswWVselpEx[1]
        VoC_add16_rr(REG2,REG1,REG7,DEFAULT);

        // pswWPVec
        VoC_lw16_sd(REG3,3,DEFAULT);

        // snsRs22
        VoC_lw16_sd(REG4,26,DEFAULT);

        // snsRs11
        VoC_lw16_sd(REG5,25,DEFAULT);

        // snsRs00
        VoC_lw16_sd(REG6,24,DEFAULT);

        // swPnShift
        VoC_lw16_sd(REG7,5,DEFAULT);

        // swVoicingMode
        VoC_lw16_sd(REG7,21,DEFAULT);

        exit_on_warnings=OFF;
        // before next REG7 update
        VoC_sw16_d(REG7, g_quant_vl_swWIShift_ADDR);

        exit_on_warnings=ON;


        VoC_lw16i_short(INC0,1,DEFAULT);
        VoC_lw16i_short(INC1,1,IN_PARALLEL);
        /************************************************************/
// the parameters below are passed by registers
// REG7 swUVCode
// REG0 pswWInput_ADDR   // INC0 = 1;
// REG1 pswWVSVec1_ADDR  // INC1 = 1;
// REG2 pswWVSVec2_ADDR  // INC2 = 1;
// REG3 pswWLTPVec_ADDR  // INC3 = 1;
// REG4 snsRs22_ADDR
// REG5 snsRs11_ADDR
// REG6 snsRs00_ADDR

// return in REG5
//remove a bug by Cui  Mar 29 ,2005
        /************************************************************/
        //      begin of CII_g_quant_vl_opt_paris
        VoC_push16(REG7,DEFAULT);

        VoC_push16(REG6,DEFAULT);
        VoC_push32(REG45,IN_PARALLEL);


// we have noticed that when swUVCode==0 && swUVCode==1,the only
// difference is only pswWLTPVec replace pswWVSVec1, pswWVSVec1
// replace pswWVSVec2, so we do this operation at the beginning.
// then the treatment should be the same, which reduce one half
// of the code


        VoC_bez16_r(CII_g_quant_vl_Lable0,REG7);
        VoC_movreg16(REG2,REG1,DEFAULT);
        VoC_movreg16(REG1,REG3,IN_PARALLEL);
CII_g_quant_vl_Lable0:

//  Again, we have notice that the treatment of each ErrorTerm[i]
//  is very similar, the only difference is the two arguments of
//  g_corr2, and one arguments of g_corr1. The same method could
//  be used as before.

        VoC_lw16i(REG3,LOCAL_g_quant_vl_ErrorTerm_ADDR);
        VoC_lw16i_short(REG6,0,DEFAULT);
//  reg6 is the loop counter
        VoC_loop_i(1,6);

        VoC_push32(REG01,DEFAULT);
        VoC_push16(REG2,DEFAULT);


        VoC_lw16i_short(INC1,1,DEFAULT);
//  we always use reg1 to store the first arg of CII_corr2 and
//  reg2 to store the second arg of CII_corr2. Their original
//  value est pushed and later poped for restituer.

//  when 1er loop, the REG6==0, reg0 and reg1 has already store
//  the proper args, no move is needed.
        VoC_bne16_rd(CII_g_quant_vl_Lable1,REG6,CONST_1_ADDR);
//  this correspond to the second loop, so the first arg is
//  pswWInput which is stocked in reg0, no mov is need, but
//  the second arg now is stocked in reg2, we mov it to reg1
        VoC_movreg16(REG1,REG2,DEFAULT);
CII_g_quant_vl_Lable1:
        VoC_be16_rd(CII_g_quant_vl_Lable2,REG6,CONST_2_ADDR);
//  From 3 to 5 loop, we only need one arg in reg0, if it is not
//  stocked there, we done a mov
        VoC_bne16_rd(CII_g_quant_vl_Lable3,REG6,CONST_3_ADDR);
        VoC_movreg16(REG0,REG1,DEFAULT);
CII_g_quant_vl_Lable3:
        VoC_bne16_rd(CII_g_quant_vl_Lable4,REG6,CONST_4_ADDR);
CII_g_quant_vl_Lable2:
        VoC_movreg16(REG0,REG2,DEFAULT);
CII_g_quant_vl_Lable4:

//  We judge if the loop counter is greater than 2,which will use
//  g_corr1, otherwise, g_corr2 is used.
        VoC_bgt16_rd(CII_g_quant_vl_Lable10,REG6,CONST_2_ADDR);
        VoC_jal(CII_g_corr2_opt);//return in reg1; output in acc1
        VoC_jump(CII_g_quant_vl_Lable11);
CII_g_quant_vl_Lable10:

        VoC_lw16i_short(INC0,2,DEFAULT);
        VoC_jal(CII_g_corr1_opt_paris);

        VoC_lw16i_short(INC0,1,DEFAULT);
        VoC_movreg32(ACC1,ACC0,DEFAULT);
CII_g_quant_vl_Lable11:

//  after that, it is the common part of round and save
        VoC_add32_rd(REG45,ACC1,CONST_0x00008000L_ADDR);
        VoC_pop16(REG2,DEFAULT);
        VoC_sw16inc_p(REG5,REG3,DEFAULT);
        VoC_sw16inc_p(REG1,REG3,DEFAULT);
        VoC_pop32(REG01,DEFAULT);
        VoC_add16_rd(REG6,REG6,CONST_1_ADDR);
        VoC_endloop1;

        VoC_sub16_rd(REG7,REG3,CONST_12_ADDR);
        exit_on_warnings=OFF;
        VoC_push16(REG7,DEFAULT);
        exit_on_warnings=ON;

        VoC_lw16i(REG5,0x7fff);
        VoC_lw16i(REG3,LOCAL_gainTweak_terms_ADDR);
        VoC_sw16_d(REG5,LOCAL_gainTweak_siNorm_ADDR);
//  put MAX16 to siNorm, then we could make same treatment of loop
//  0 & loop1
        VoC_lw16i_short(REG5,0,DEFAULT);

// gainTweak_psErrorTerm_ADDR
        VoC_push16(REG7,DEFAULT);


LOOP_START0:

//  we notice that the treatment of five blocks could be the same
//  if we map three different arguments to reg0,reg1,reg2

        VoC_push32(REG67,DEFAULT);
        VoC_push16(REG5,DEFAULT);

        VoC_lw16i_short(REG6,2,DEFAULT);

        VoC_lw16i_short(INC1,1,IN_PARALLEL);

        VoC_add16_rd(REG0,REG7,CONST_6_ADDR);
        VoC_add16_rr(REG2,REG0,REG6,DEFAULT);
        VoC_add16_rr(REG1,REG2,REG6,DEFAULT);

//  if loop_counter == 0
//  reg0: &psErrorTerm[3].man
//  reg1: &psErrorTerm[5].man
//  reg2: &psErrorTerm[4].man


        VoC_bne16_rd(LOOP_ADDR2,REG5,CONST_1_ADDR);
        VoC_add16_rd(REG0,REG7,CONST_4_ADDR);
        VoC_movreg16(REG1,REG0,DEFAULT);
        VoC_add16_rd(REG2,REG7,CONST_10_ADDR);
//  if loop_counter == 1
//  reg0: &psErrorTerm[2].man
//  reg1: &psErrorTerm[2].man
//  reg2: &psErrorTerm[5].man
LOOP_ADDR2:
        VoC_bne16_rd(LOOP_ADDR3,REG5,CONST_2_ADDR);
        VoC_add16_rr(REG0,REG7,REG6,DEFAULT);
        VoC_movreg16(REG1,REG0,DEFAULT);
        VoC_add16_rd(REG2,REG7,CONST_6_ADDR);
//  if loop_counter == 2
//  reg0: &psErrorTerm[1].man
//  reg1: &psErrorTerm[1].man
//  reg2: &psErrorTerm[3].man
LOOP_ADDR3:
        VoC_bne16_rd(LOOP_ADDR4,REG5,CONST_3_ADDR);
        VoC_movreg16(REG0,REG7,DEFAULT);
        VoC_add16_rr(REG1,REG7,REG6,IN_PARALLEL);
        VoC_add16_rr(REG2,REG1,REG6,DEFAULT);
//  if loop_counter == 3
//  reg0: &psErrorTerm[0].man
//  reg1: &psErrorTerm[1].man
//  reg2: &psErrorTerm[2].man
LOOP_ADDR4:

        VoC_bne16_rd(LOOP_ADDR5,REG5,CONST_4_ADDR);
        VoC_movreg16(REG0,REG7,DEFAULT);
        VoC_movreg16(REG2,REG7,IN_PARALLEL);
        VoC_add16_rd(REG1,REG7,CONST_8_ADDR);

//  if loop_counter == 4
//  reg0: &psErrorTerm[0].man
//  reg1: &psErrorTerm[4].man
//  reg2: &psErrorTerm[0].man
LOOP_ADDR5:
//  reg4 for temp_val
//  reg5 for term.sh
//  reg6 for term.man
//  reg7 for swshift

//  common treatment for all five blocks

//  save the values in REG5 & REG67 inorder to use them after

        VoC_lw16inc_p(REG4,REG0,DEFAULT); // [3].man in reg4
        VoC_lw16i_short(REG7,0,DEFAULT);

        VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT); // L_temp in ACC0

        VoC_push16(REG1,DEFAULT);  // reg1 will be use in NORM_L_ACC0
        VoC_lw16_p(REG5,REG0,DEFAULT);


        VoC_loop_i(1,2);
        VoC_movreg32(ACC1,ACC0,DEFAULT);  // L_temp in ACC1
        VoC_movreg16(ACC0_LO,REG6,DEFAULT); // ACC0_LO set to 0 for the norm_s of ACC0_hi
        VoC_jal(CII_NORM_L_ACC0);        // swShift in reg1
        VoC_movreg32(ACC0,ACC1,DEFAULT); // L_temp in ACC0
        VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);

        VoC_sub16_rr(REG5,REG5,REG1,DEFAULT); // [0].sh in reg5
        VoC_shr32_rr(ACC0,REG1,DEFAULT);
        VoC_add32_rd(REG67,ACC0,CONST_0x00008000L_ADDR);
        VoC_multf32_rp(ACC0,REG7,REG2,DEFAULT); // ACC0 L_temp
        VoC_lw16i_short(REG6,0,DEFAULT);
        VoC_endloop1;


        VoC_inc_p(REG2,DEFAULT);
        VoC_pop16(REG1,IN_PARALLEL);  // &[5].sh in reg1

        VoC_add16_rp(REG5,REG5,REG2,DEFAULT); // [0].sh in reg5

        VoC_add16_rp(REG5,REG5,REG1,DEFAULT);  // [0].sh in reg5
        VoC_lw16_sd(REG6,0,DEFAULT);
        VoC_bne16_rd(LOOP_END1,REG6,CONST_3_ADDR);
        VoC_sub16_rd(REG5,REG5,CONST_1_ADDR);
LOOP_END1:

        VoC_sw16inc_p(REG7,REG3,DEFAULT);
        VoC_sw16inc_p(REG5,REG3,DEFAULT);
        VoC_bgt16_rd(LOOP_END0,REG5,LOCAL_gainTweak_siNorm_ADDR);
        VoC_sw16_d(REG5,LOCAL_gainTweak_siNorm_ADDR);
LOOP_END0:
        VoC_pop32(REG67,DEFAULT);
        VoC_pop16(REG5,DEFAULT);
//  reg5 loop_counter
        VoC_add16_rd(REG5,REG5,CONST_1_ADDR);
        VoC_be16_rd(LOOP_START0_new,REG5,CONST_5_ADDR);

        VoC_jump(LOOP_START0);
LOOP_START0_new:
        VoC_lw16i_short(REG6,0,DEFAULT);
        VoC_lw16_d(REG4,LOCAL_gainTweak_siNorm_ADDR);

        VoC_lw16i_set_inc(REG0,LOCAL_gainTweak_terms_ADDR,2);
        VoC_lw16i_set_inc(REG1,LOCAL_gainTweak_terms_ADDR+1,2);
        VoC_loop_i_short(5,DEFAULT)

        VoC_startloop0;
        VoC_lw16_p(REG7,REG0,DEFAULT);//L_Temp in reg67
        VoC_sub16inc_pr(REG3,REG1,REG4,DEFAULT);//siNormShift in reg3
        VoC_bngtz16_r(gainTweak55,REG3);
        VoC_shr32_rr(REG67,REG3,DEFAULT);
gainTweak55:

        VoC_add32_rd(REG67,REG67,CONST_0x00008000L_ADDR);
        VoC_lw16i_short(REG6,0,DEFAULT);
        VoC_sw16inc_p(REG7,REG0,DEFAULT);
        VoC_endloop0


        VoC_lw16_d(REG5,LOCAL_gainTweak_terms_ADDR);
//  swNum = sub(terms[0].man, terms[1].man);
        VoC_sub16_rd(REG5,REG5,LOCAL_gainTweak_terms_ADDR+2);
        VoC_bngtz16_r(gainTweak_end,REG5);

        VoC_lw16_d(REG6,LOCAL_gainTweak_terms_ADDR+4);
        VoC_sub16_rd(REG6,REG6,LOCAL_gainTweak_terms_ADDR+6);
        VoC_add16_rd(REG6,REG6,LOCAL_gainTweak_terms_ADDR+8);//swDenom in reg5
        VoC_bngtz16_r(gainTweak_end,REG6);

        VoC_bngt16_rr(gainTweak_end,REG5,REG6);

        VoC_lw32z(ACC0,DEFAULT);
        VoC_movreg16(ACC0_HI,REG5,DEFAULT);
        VoC_jal(CII_NORM_L_ACC0);//swShift in reg1
        VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
        VoC_sub16_dr(REG2,CONST_0xFFFF_ADDR,REG1);//siNormShift in reg2
        VoC_shr16_rr(REG5,REG1,DEFAULT);
        VoC_shr16_ri(REG5,1,DEFAULT);//siNum in reg5

        VoC_lw32z(ACC0,DEFAULT);
        VoC_movreg16(ACC0_HI,REG6,DEFAULT);
        VoC_jal(CII_NORM_L_ACC0);//swShift in reg1
        VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
        VoC_add16_rr(REG7,REG2,REG1,DEFAULT);//siNormShift in reg7
        VoC_shr16_rr(REG6,REG1,DEFAULT);//swDenom in reg6
        VoC_movreg16(REG0,REG5,DEFAULT);
        VoC_movreg16(REG1,REG6,IN_PARALLEL);
        VoC_jal(CII_DIV_S);//swTemp in reg2

        VoC_lw32z(ACC0,DEFAULT);
        VoC_movreg16(ACC0_HI,REG2,DEFAULT);
        VoC_movreg32(RL7,ACC0,DEFAULT);
        VoC_jal(CII_NORM_L_ACC0);//swShift in reg1

        VoC_lw32z(RL6,DEFAULT);//RL6 will not be destroyed in CII_sqroot_opt
        VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
        VoC_sub16_rr(REG2,REG7,REG1,DEFAULT);//siNormShift in reg2
        VoC_shr32_rr(RL7,REG1,DEFAULT);
        VoC_jal(CII_sqroot_opt);//swTemp in reg7


        VoC_movreg16(RL6_HI,REG7,DEFAULT);
//  L_Temp = L_deposit_h(swTemp)
        VoC_movreg16(REG0,REG2,DEFAULT);
        VoC_and16_ri(REG0,0x1);
        VoC_bez16_r(gainTweak99,REG0);
//  if (siNormShift & 1), do the process below
//  else L_Temp not change
        VoC_multf32_rd(RL6,REG7,CONST_0x5a82_ADDR);
        VoC_sub16_rd(REG2,REG2,CONST_1_ADDR);
gainTweak99:

        VoC_lw32z(ACC0,DEFAULT);
        VoC_movreg16(ACC0_HI,RL6_HI,DEFAULT);

        VoC_jal(CII_NORM_L_ACC0);

        VoC_shr16_ri(REG2,1,DEFAULT);
        VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
        VoC_add16_rr(REG2,REG2,REG1,DEFAULT);//swNormShift in reg2

        VoC_shr32_rr(RL6,REG1,IN_PARALLEL);
        VoC_add32_rd(REG45,RL6,CONST_0x00008000L_ADDR);//swGainTweak in reg5


        VoC_lw16i(REG4,0x5a82);
//  siNormShift>-1
        VoC_bgt16_rd(gainTweak_end,REG2,CONST_0xFFFF_ADDR);

        VoC_bgt16_rd(gainTweak003,REG5,CONST_0x5a82_ADDR);
        VoC_blt16_rd(gainTweak003,REG2,CONST_0xFFFF_ADDR);
        VoC_movreg16(REG4,REG5,DEFAULT);
gainTweak003:

        VoC_lw16_sd(REG0,0,DEFAULT);

        VoC_lw16i_short(INC0,1,DEFAULT);
        VoC_loop_i(1,2);
//  reg0 gainTweak_psErrorTerm_ADDR
        VoC_movreg16(REG3,REG0,DEFAULT);
        VoC_multf32inc_rp(REG67,REG4,REG0,DEFAULT);
        VoC_lw32z(ACC0,DEFAULT);
        VoC_movreg16(ACC0_HI,REG7,DEFAULT);
        VoC_jal(CII_NORM_L_ACC0);//-swShift in reg1
        VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
        VoC_sub16_pr(REG2,REG0,REG1,DEFAULT);

        VoC_sub16_rd(REG2,REG2,CONST_1_ADDR);
        VoC_shr32_rr(REG67,REG1,DEFAULT);
        VoC_add32_rd(REG67,REG67,CONST_0x00008000L_ADDR);

        VoC_sw16inc_p(REG2,REG0,DEFAULT);
        VoC_sw16_p(REG7,REG3,DEFAULT);

        VoC_endloop1;

gainTweak_end:

//////////////////////////////////////////////////////////////////////////////




//  for the later part, it is the same principle as befor, the
//  only difference between swUVCode==0 && swUVCode==1 is the
//  premier use snsRs11 and snsRs22, while in the second, they
//  are replaced by snsRs00 and snsRs11. So this replacement is
//  done first, and the left treatment are same.
        VoC_pop16(REG3,DEFAULT);
        VoC_pop16(REG7,DEFAULT);
        VoC_pop16(REG6,DEFAULT);
        VoC_pop32(REG45,IN_PARALLEL);

        VoC_bez16_r(CII_g_quant_vl_Lable20,REG7);
        VoC_movreg16(REG4,REG5,DEFAULT);
        VoC_movreg16(REG5,REG6,IN_PARALLEL);
CII_g_quant_vl_Lable20:

//  the treatment of CoeffA and CoeffB is the same if we predefine
//  siNormMin to MAX16. And we also notice that the treatment of
//  A & B could be comprised in the treatment of C and D, only on
//  reducing some instructions
//  So we can integrate the five blocks to
//  one, only need to preload different args into same regs.

        VoC_lw16i(REG0,0x7fff);
        VoC_lw16i_short(REG6,0,DEFAULT);

//  reg6 is the loop counter
        VoC_sw16_d(REG0,LOCAL_g_quant_vl_siNormMin_ADDR);


        VoC_loop_i(1,5);
        VoC_push32(REG45,DEFAULT);
        VoC_push32(REG67,DEFAULT);

//  in this loop, regs are allocated as following:

//  reg4 map first  snsRS addr
//  reg5 map second snsRs addr
//  reg3 address of errorTerm[n].man
//  reg2 address of errorTerm[n].sh
//  reg1 swshift
//  reg6 temp
//  reg0 errorTerm[n].sh
//  reg7 errorTerm[n].man
//  RL6  L_TEMP

//  only the 1er,3th,4th loop need to move the args from reg to reg.
        VoC_be16_rd(CII_g_quant_vl_Lable21,REG6,CONST_0_ADDR);
        VoC_bne16_rd(CII_g_quant_vl_Lable23,REG6,CONST_3_ADDR);
CII_g_quant_vl_Lable21:
        VoC_movreg16(REG4,REG5,DEFAULT);
CII_g_quant_vl_Lable23:
        VoC_bne16_rd(CII_g_quant_vl_Lable24,REG6,CONST_4_ADDR);
        VoC_movreg16(REG5,REG4,DEFAULT);
CII_g_quant_vl_Lable24:

        VoC_lw32z(ACC0,DEFAULT);
//  the following part is not done by the first and second loop,
        VoC_bngt16_rd(CII_g_quant_vl_Lable28,REG6,CONST_1_ADDR);
        VoC_lw16_p(REG1,REG5,DEFAULT);
        VoC_multf32_rp(RL6,REG1,REG3,DEFAULT);
        VoC_add16_rd(REG5,REG5,CONST_1_ADDR);
        VoC_movreg16(ACC0_HI,RL6_HI,DEFAULT);//L_Temp in rl6

        VoC_jal(CII_NORM_L_ACC0);//swShift in reg1
        VoC_lw32z(ACC0,DEFAULT);
        VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);

        VoC_add16_rd(REG2,REG3,CONST_1_ADDR);

        VoC_shr32_rr(RL6,REG1,DEFAULT);
        VoC_sub16_pr(REG0,REG2,REG1,DEFAULT);
        VoC_lw16i_short(INC1,1,IN_PARALLEL);
        VoC_add32_rd(RL6,RL6,CONST_0x00008000L_ADDR);
        VoC_movreg16(REG7,RL6_HI,DEFAULT);

        VoC_add16_rp(REG0,REG0,REG5,DEFAULT);
        VoC_add16_rd(REG1,REG3,CONST_1_ADDR);
        VoC_sw16_p(REG7,REG3,DEFAULT);
        VoC_sw16_p(REG0,REG1,DEFAULT);
CII_g_quant_vl_Lable28:

//  the following is the common parts for five
        VoC_lw16_p(REG1,REG4,DEFAULT);

        VoC_multf32_rp(RL6,REG1,REG3,DEFAULT);
        VoC_add16_rd(REG4,REG4,CONST_1_ADDR);

        VoC_movreg16(ACC0_HI,RL6_HI,DEFAULT);//L_Temp in RL6
        VoC_jal(CII_NORM_L_ACC0);//swShift in reg1
        VoC_lw16i_short(INC1,1,DEFAULT);
        VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
        VoC_add16_rd(REG2,REG3,CONST_1_ADDR);
        VoC_shr32_rr(RL6,REG1,DEFAULT);


        VoC_sub16_pr(REG0,REG2,REG1,DEFAULT);

//  ErrorTerm[2].sh = add(ErrorTerm[2].sh, swShift);

        VoC_add32_rd(RL6,RL6,CONST_0x00008000L_ADDR);//ErrorTerm[1].man in reg7
        VoC_movreg16(REG7,RL6_HI,DEFAULT);
//  ErrorTerm[2].man = round(L_shl(L_Temp, swShift));

        VoC_add16_rp(REG0,REG0,REG4,DEFAULT);
//  the following part is not done by the first and second loop,
        VoC_bngt16_rd(CII_g_quant_vl_Lable29,REG6,CONST_1_ADDR);

        VoC_add16_rd(REG0,REG0,g_quant_vl_swWIShift_ADDR);
CII_g_quant_vl_Lable29:

        VoC_sw16inc_p(REG7,REG3,DEFAULT);
        VoC_sw16inc_p(REG0,REG3,DEFAULT);

        VoC_bgt16_rd(CII_g_quant_vl_Lable30,REG0,LOCAL_g_quant_vl_siNormMin_ADDR);
        VoC_sw16_d(REG0,LOCAL_g_quant_vl_siNormMin_ADDR);
CII_g_quant_vl_Lable30:

        VoC_pop32(REG67,DEFAULT);
        VoC_pop32(REG45,DEFAULT);
        VoC_add16_rd(REG6,REG6,CONST_1_ADDR);
        VoC_endloop1;

        VoC_sub16_rd(REG3,REG3,CONST_10_ADDR);
        VoC_lw16_d(REG4,LOCAL_g_quant_vl_siNormMin_ADDR);

        VoC_loop_i_short(5,DEFAULT)
        VoC_startloop0;
//  reg0  addr of ErrorTerm[i].man
//  reg2  SiNormShift
//  reg3  addr of ErrorTerm[i].man,ErrorTerm[i].sh
//  reg67 L_temp
        VoC_movreg16(REG0,REG3,DEFAULT);
        VoC_lw32z(REG67,DEFAULT);

        VoC_lw16inc_p(REG7,REG3,DEFAULT);
        VoC_sub16inc_pr(REG2,REG3,REG4,DEFAULT);
        VoC_bngtz16_r(g_quant_vl_111,REG2);
        VoC_shr32_rr(REG67,REG2,DEFAULT);
g_quant_vl_111:

        VoC_add32_rd(REG67,REG67,CONST_0x00008000L_ADDR);
        VoC_lw32_d(RL7,CONST_0x80000000_ADDR);
        VoC_sw16_p(REG7,REG0,DEFAULT);
        VoC_endloop0;

        VoC_pop16(REG7,DEFAULT);
        VoC_mult16_rd(REG2,REG7,CONST_160_ADDR);
        VoC_lw16i_set_inc(REG0,TABLE_SP_ROM_pppsrGsp0_ADDR,1);
        VoC_add16_rr(REG0,REG0,REG2,DEFAULT);

        VoC_lw16i_short(INC3,2,IN_PARALLEL);
        VoC_sub16_rd(REG3,REG3,CONST_10_ADDR);

        VoC_lw16i_short(REG1,0,DEFAULT);

        VoC_loop_i(1,32)
//  reg0 addr of pppsrGsp0[][i][j]
//  reg3 addr of ErrorTerm[j].man
//  reg4 i
//  reg5 siCode
        VoC_lw32z(ACC0,DEFAULT);
        VoC_loop_i_short(5,DEFAULT);

        VoC_startloop0;
        VoC_lw16inc_p(REG6,REG3,DEFAULT);
        VoC_mac32inc_rp(REG6,REG0,DEFAULT);
        VoC_endloop0;
        VoC_sub16_rd(REG3,REG3,CONST_10_ADDR);
        VoC_bngt32_rr(g_quant_vl_222,ACC0,RL7);
        VoC_movreg32(RL7,ACC0,DEFAULT);
        VoC_movreg16(REG5,REG1,IN_PARALLEL);
g_quant_vl_222:
        VoC_inc_p(REG1,DEFAULT);
        VoC_endloop1;
//  end of CII_g_quant_vl_opt_paris
        // OUTPUT in REG5

    }


    // psiGsp0Code
    VoC_sw16_sd(REG5,20,DEFAULT);

    // Need to have the same stack state
    // independently from the branch taken

    // restoring intermediary SP16
    VoC_lw16_d(SP16,sfrmAnalysis_SP16_SAVE1_ADDR);

    VoC_jump(sfrmAnalysis_midend);

// G

sfrmAnalysis_mid:

    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *


    /* swSP == 0, currently in comfort noise insertion mode */
    /* DTX mode */

    // siNumBasisVecs
    VoC_lw16i_short(REG4,7,DEFAULT); // C_BITS_UV

    // pswBitArray
    VoC_lw16_sd(REG0,0,DEFAULT);

    // psiVSCode1
    VoC_lw16_sd(REG5,7,DEFAULT);

    VoC_push16(REG4,DEFAULT);

    // INPUT :
    // REG0 : pswVectOut, INC = 1       (param2)
    // REG4 : siNumBits         (param1)
    // REG5 : swCodeWord            (param0)

    VoC_jal(CII_b_con_opt);

    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // siNumBasisVecs   *

    // ppswVselpEx[0]
    VoC_lw16_sd(REG1,3,DEFAULT);
    // pppsrUvCodeVec[0][0]
    VoC_lw16i_set_inc(REG0,TABLE_SP_ROM_pppsrUvCodeVec_ADDR,40);

    // pswBitArray
    VoC_lw16_sd(REG2,1,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    // siNumBasisVecs
    VoC_lw16_sd(REG4,0,DEFAULT);

    // INPUT :
    // REG0 : pswBVects,        INC0 = 40   (param0)
    // REG1 : pswOutVect,       INC1 = 2    (param1)
    // REG2 : pswBitArray,      INC2 = 1    (param2)
    // REG4 : siNumBVctrs               (param3)

    VoC_jal(CII_v_con_opt);

    // siNumBasisVecs
    VoC_lw16_sd(REG4,0,DEFAULT);

    // pswBitArray
    VoC_lw16_sd(REG0,1,DEFAULT);

    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    // psiVSCode2
    VoC_lw16_sd(REG5,7,DEFAULT);

    // INPUT :
    // REG0 : pswVectOut, INC = 1       (param2)
    // REG4 : siNumBits         (param1)
    // REG5 : swCodeWord            (param0)

    VoC_jal(CII_b_con_opt);

    // pswBitArray
    VoC_lw16_sd(REG2,1,DEFAULT);
    // pppsrUvCodeVec[1][0]
    VoC_lw16i_set_inc(REG0,TABLE_SP_ROM_pppsrUvCodeVec_ADDR+7*40,40);

    // ppswVselpEx[1]
    VoC_lw16i_set_inc(REG1,LOCAL_sfrmAnalysis_ppswVselpEx_ADDR+40,2);


    // siNumBasisVecs
    VoC_pop16(REG4,DEFAULT);

    // INPUT :
    // REG0 : pswBVects,        INC0 = 40   (param0)
    // REG1 : pswOutVect,       INC1 = 2    (param1)
    // REG2 : pswBitArray,      INC2 = 1    (param2)
    // REG4 : siNumBVctrs               (param3)

    VoC_jal(CII_v_con_opt);

    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *

    // snsSqrtRs
    VoC_lw16_sd(REG2,10,DEFAULT);
    // ppswVselpEx[0]
    VoC_lw16i_set_inc(REG0,LOCAL_sfrmAnalysis_ppswVselpEx_ADDR,2);

    // snsRs11
    VoC_lw16_sd(REG3,13,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    // INPUT :
    // REG0 : param0,   INC0 = 2
    // REG2 : param1,   INC2 = 1
    // REG3 : param2,   INC3 = 1

    VoC_jal(CII_rs_rrNs_opt);

    // snsSqrtRs
    VoC_lw16_sd(REG2,10,DEFAULT);

    // snsRs22
    VoC_lw16_sd(REG3,14,DEFAULT);

    // ppswVselpEx[1]
    VoC_lw16i_set_inc(REG0,LOCAL_sfrmAnalysis_ppswVselpEx_ADDR+40,2);

    // INPUT :
    // REG0 : param0,   INC0 = 2
    // REG2 : param1,   INC2 = 1
    // REG3 : param2,   INC3 = 1

    VoC_jal(CII_rs_rrNs_opt);

sfrmAnalysis_midend:

// H

    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *

    // swVoicingMode
    VoC_lw16_sd(REG6,9,DEFAULT);

    // psiGsp0Code
    VoC_lw16_sd(REG1,8,DEFAULT);
    VoC_mult16_rd(REG0,REG6,CONST_160_ADDR);

    // *psiGsp0Code
    VoC_mult16_rd(REG1,REG1,CONST_5_ADDR);

    // & pppsrGsp0[swVoicingMode][*psiGsp0Code][0]
    VoC_lw16i(REG4,TABLE_SP_ROM_pppsrGsp0_ADDR);
    VoC_add16_rr(REG4,REG4,REG0,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG1,DEFAULT);

    // snsRs11
    VoC_lw16_sd(REG7,13,DEFAULT);

    // push pppsrGsp0[swVoicingMode][*psiGsp0Code][0]
    VoC_push16(REG4,DEFAULT);

    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // &pppsrGsp0[swVoicingMode][*psiGsp0Code][0]


    // pppsrGsp0[swVoicingMode][*psiGsp0Code][0]
    VoC_lw16_p(REG4,REG4,DEFAULT);

    // ppswVselpEx[0]
    VoC_lw16_sd(REG0,3,DEFAULT);
    VoC_lw16_sd(REG1,3,IN_PARALLEL);


    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_bnez16_r(sfrmAnalysis_0000,REG6)

    // INPUT :
    // REG7 : snsRs             (param2)
    // REG4 : swErrTerm         (param1)
    // REG1 : pswScldVect,  INC1 = 1    (param0)
    // REG0 : pswVect,  INC0 = 1    (param3)

    VoC_jal(CII_scaleExcite_opt);


    // pppsrGsp0[swVoicingMode][*psiGsp0Code][1]
    VoC_lw16_sd(REG4,0,DEFAULT);
    VoC_add16_rd(REG4,REG4,CONST_1_ADDR);

    // ppswVselpEx[1]
    VoC_lw16i(REG1,LOCAL_sfrmAnalysis_ppswVselpEx_ADDR+40);

    VoC_lw16_p(REG4,REG4,DEFAULT);

    // snsRs22
    VoC_lw16_sd(REG7,15,DEFAULT);

    // ppswVselpEx[1]
    VoC_movreg16(REG0,REG1,IN_PARALLEL);

    // INPUT :
    // REG7 : snsRs             (param2)
    // REG4 : swErrTerm         (param1)
    // REG1 : pswScldVect,  INC1 = 1    (param0)
    // REG0 : pswVect,  INC0 = 1    (param3)

    VoC_jal(CII_scaleExcite_opt);

    // ppswVselpEx[0]
    VoC_lw16_sd(REG0,3,DEFAULT);

    // pswTempVec
    VoC_lw16_sd(REG1,6,DEFAULT);

    // ppswVselpEx[1]
    VoC_lw16i_set_inc(REG2,LOCAL_sfrmAnalysis_ppswVselpEx_ADDR+40,1);

    VoC_jump(sfrmAnalysis_0001);

sfrmAnalysis_0000:


    // snsRs00
    VoC_lw16_sd(REG7,13,DEFAULT);

    // pswPVec
    VoC_lw16_sd(REG0,4,DEFAULT);
    VoC_lw16_sd(REG1,4,IN_PARALLEL);

    // INPUT :
    // REG7 : snsRs             (param2)
    // REG4 : swErrTerm         (param1)
    // REG1 : pswScldVect,  INC1 = 1    (param0)
    // REG0 : pswVect,  INC0 = 1    (param3)

    VoC_jal(CII_scaleExcite_opt);

    // &pppsrGsp0[swVoicingMode][*psiGsp0Code][1]
    VoC_lw16_sd(REG4,0,DEFAULT);
    VoC_add16_rd(REG4,REG4,CONST_1_ADDR);

    // ppswVselpEx[0]
    VoC_lw16_sd(REG1,3,DEFAULT);
    VoC_lw16_sd(REG0,3,IN_PARALLEL);

    // snsRs11
    VoC_lw16_sd(REG7,14,DEFAULT);

    VoC_lw16_p(REG4,REG4,DEFAULT);

    // INPUT :
    // REG7 : snsRs             (param2)
    // REG4 : swErrTerm         (param1)
    // REG1 : pswScldVect,  INC1 = 1    (param0)
    // REG0 : pswVect,  INC0 = 1    (param3)

    VoC_jal(CII_scaleExcite_opt);

    // pswPVec
    VoC_lw16_sd(REG0,4,DEFAULT);
    // ppswVselpEx[0]
    VoC_lw16_sd(REG2,3,DEFAULT);
    // pswTempVec
    VoC_lw16_sd(REG1,6,DEFAULT);

sfrmAnalysis_0001:

    // for (i = 0; i < S_LEN; i++)
    //  pswTempVec[i] = add(...
    VoC_lw16inc_p(REG3,REG0,DEFAULT);
    VoC_loop_i_short(40,DEFAULT)

    VoC_startloop0
    VoC_add16inc_rp(REG4,REG3,REG2,DEFAULT);
    VoC_lw16inc_p(REG3,REG0,DEFAULT);
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_endloop0;

    // pswLtpStateOut[-LTP_LEN+S_LEN]
    VoC_lw16i_set_inc(REG1,GLOBAL_SFRM_pswLtpStateBase_ADDR+40,1);
    // pswLtpStateOut[-LTP_LEN]
    VoC_lw16i_set_inc(REG0,GLOBAL_SFRM_pswLtpStateBase_ADDR,1);


    VoC_lw16inc_p(REG2,REG1,DEFAULT);
    VoC_loop_i(0,107)
    VoC_lw16inc_p(REG2,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG2,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;


    // pswTempVec
    VoC_lw16_sd(REG1,6,DEFAULT);
    // REG0 = pswLtpStateOut[-S_LEN] at the end of the loop

    VoC_loop_i_short(40,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG2,REG1,DEFAULT);
//      VoC_NOP();
    VoC_lw16i_set_inc(REG3,GLOBAL_SFRM_pswHNWState_ADDR+40,1);      // stephen 060711
    VoC_sw16inc_p(REG2,REG0,DEFAULT);
    VoC_endloop0;


    VoC_lw16i_set_inc(REG2,GLOBAL_SFRM_pswHNWState_ADDR,1);

    VoC_lw16inc_p(REG1,REG3,DEFAULT);
    VoC_loop_i(0,105)
    VoC_lw16inc_p(REG1,REG3,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG1,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;



    // pswTempVec
    VoC_lw16_sd(REG0,6,DEFAULT);

    // &pswHNWState[HNW_BUFF_LEN - S_LEN]
    VoC_lw16i_set_inc(REG1,GLOBAL_SFRM_pswHNWState_ADDR+105,-1);


    // INPUT
    // REG0 : pswInput,     INC0 = 1    (param0)
    // REG1 : pswFiltout,   INC1 = -1   (param3)
    // REG2 : pswState,     INC2 = 1    (param2)
    // REG3 : pswCoeffs,    INC3 = 1    (param1)

    // pswHState
    VoC_lw16_sd(REG2,5,DEFAULT);

    // pswHCoefs
    VoC_lw16_sd(REG3,12,DEFAULT);

    // STACK16      still       STACK32     still
    //          needed              needed
    // -----------------------------------------------------------
    // RA           *
    // snsRs22      *
    // snsRs11      *
    // snsRs00      *
    // pswHCoef     *
    // snsSqrtRs        *
    // swVoicingMode    *
    // psiGsp0Code      *
    // psiVSCode1       *
    // psiVSCode2       *
    // pswTempVec       *
    // pswHState        *
    // pswPVec      *
    // ppswVselpEx      *
    // pswLTPStateOut   *
    // pswBitArray      *
    // &pppsrGsp0[swVoicingMode][*psiGsp0Code][0]

    VoC_lw16_sd(REG5,9,DEFAULT);
    VoC_lw16_d(REG4,sfrmAnalysis_psiGsp0Code_ADDR_ADDR);
    VoC_lw16_sd(REG6,8,DEFAULT);
    VoC_sw16_p(REG5,REG4,DEFAULT);
    VoC_lw16_d(REG4,sfrmAnalysis_psiVSCode1_ADDR_ADDR);
    VoC_lw16_sd(REG7,7,DEFAULT);
    VoC_sw16_p(REG6,REG4,DEFAULT);
    VoC_lw16_d(REG4,sfrmAnalysis_psiVSCode2_ADDR_ADDR);


    // Need to have the same stack state
    // independently from the branch taken
    // restoring initial SP16
    VoC_lw16_d(SP16,sfrmAnalysis_SP16_SAVE0_ADDR);

    VoC_sw16_p(REG7,REG4,DEFAULT);


    VoC_jal(CII_lpcIir_opt_paris);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;

// I

}

void   CII_lpcIir_opt_paris(void)
{
    // INPUT parameters :
    // REG3 : pswCoef , INC3 = 1
    // REG2 : pswState, INC2 = 1
    // REG1 : swFiltOut, INC1 = -1
    // REG0 : pswInput, INC0 = 1
    // other vars:
    // REG4 : maxsiStage1, maxsiStage2, extract_h(L_Sum)
    // REG5 : siSmp

#if 0
    voc_short TEMP_COPY_ADDR_Y,10,y

#endif


    VoC_lw16i(REG2,TEMP_COPY_ADDR_Y);
    exit_on_warnings=OFF;
    VoC_push32(REG23,DEFAULT);
    exit_on_warnings=ON;
    VoC_lw16inc_p(ACC0_LO,REG3,DEFAULT);
    VoC_loop_i_short(10,DEFAULT);

    VoC_startloop0;
    VoC_lw16inc_p(ACC0_LO,REG3,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(ACC0_LO,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;

    VoC_pop32(REG23,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL); // &pswFiltOut[-1]

    VoC_lw32_d(ACC0,CONST_0x00000800L_ADDR);
    VoC_lw16i(REG3,TEMP_COPY_ADDR_Y);

    VoC_push16(REG7,DEFAULT);
    VoC_push32(ACC0,IN_PARALLEL);

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_short(REG4,10,IN_PARALLEL);

    VoC_lw16i(REG7,0x8000);

    VoC_loop_i(1,10)

    // if NP > siSmp, REG4 = siSmp, newREG4 = NP - siSmp
    // else REG4 = NP, newREG4 = NP - NP = 0 ! > 0
    // so second loop will be skipped
    VoC_sub16_rr(REG4,REG4,REG5,DEFAULT);

    VoC_bez16_r(lpcIir_skip_1st,REG5);

    VoC_loop_r_short(REG5,DEFAULT)
    VoC_lw32_sd(ACC0,0,IN_PARALLEL);
    VoC_startloop0
    VoC_msu32inc_pp(REG1,REG3,DEFAULT);
    VoC_endloop0;
lpcIir_skip_1st:

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0
    VoC_msu32inc_pp(REG2,REG3,DEFAULT);
    VoC_endloop0;

    // siSmp++
    VoC_add16_rd(REG5,REG5,CONST_1_ADDR);

    VoC_msu32inc_rp(REG7,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_shr32_ri(ACC0,-4,IN_PARALLEL);
    exit_on_warnings=ON;

    // restitute &pswState[0]
    VoC_sub16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG5,IN_PARALLEL);
    VoC_lw16i_short(REG4,10,DEFAULT);
    VoC_sw16_p(ACC0_HI,REG1,DEFAULT);

    // restitute &pswCoef[0]
    VoC_sub16_rr(REG3,REG3,REG4,IN_PARALLEL);

    VoC_endloop1;


    VoC_push16(REG2,DEFAULT);
    VoC_push32(RL7,IN_PARALLEL);

    VoC_lw16i_short(REG5,9,DEFAULT);
    VoC_movreg16(REG2,REG1,IN_PARALLEL);

    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_lw16i_short(INC3,-2,IN_PARALLEL);
    VoC_sub16_rr(REG1,REG2,REG5,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG5,IN_PARALLEL);

    VoC_lw32_sd(RL7,1,DEFAULT);

    VoC_loop_i(1,30)

    VoC_push16(REG3,DEFAULT);

    VoC_aligninc_pp(REG1,REG3,DEFAULT);
    VoC_movreg32(ACC0,RL7,IN_PARALLEL);

    VoC_loop_i_short(5,DEFAULT)
    exit_on_warnings=OFF;
    VoC_sw16inc_p(ACC0_HI,REG2,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_startloop0
    VoC_bimsu32inc_pp(REG1,REG3);
    VoC_endloop0;

    VoC_sub16_rr(REG1,REG2,REG5,DEFAULT);
    // restitute &pswCoef[0]
    VoC_pop16(REG3,IN_PARALLEL);


    VoC_msu32inc_rp(REG7,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_shr32_ri(ACC0,-4,IN_PARALLEL);
    exit_on_warnings=ON;

    VoC_endloop1;


    VoC_add16_rr(REG1,REG1,REG5,DEFAULT);

    VoC_lw16i_short(INC1,-1,IN_PARALLEL);
    VoC_pop16(REG2,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw16_p(ACC0_HI,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_pop32(RL7,IN_PARALLEL);

    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_loop_i_short(10,DEFAULT);

    VoC_startloop0;
    VoC_lw16inc_p(REG5,REG1,DEFAULT);

    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    exit_on_warnings=ON;

    VoC_endloop0;

    VoC_pop16(REG7,DEFAULT);
    VoC_pop32(ACC0,IN_PARALLEL);


    VoC_return;
}

/************************************************************
*
* INPUT:
*       REG0  decorr_pswGivenVect_ADDR
*       REG3  decorr_pswVects_ADDR         INC3=1
*       REG5  decorr_iNumVects
*
* NO OUTPUT
************************************************************/


void CII_decorr_opt(void)
{


#if 0

    voc_short   decorr_pswGivenVect_ADDR_ADDR                  ,x

#endif



    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(INC0,2,DEFAULT);

    VoC_sw16_d(REG0,decorr_pswGivenVect_ADDR_ADDR);
    VoC_lw16i_short(REG4,0,DEFAULT);
//  VoC_lw16_d(REG0,decorr_pswGivenVect_ADDR_ADDR);
    VoC_jal(CII_g_corr1_opt_paris);//return swEShift in reg1,L_Energy in acc1
    VoC_movreg16(REG6,ACC0_HI,DEFAULT);
    VoC_add16_rd(REG7,REG1,CONST_1_ADDR);
    VoC_bez16_r(decorr_end,REG6);

//  VoC_lw16_d(REG5,decorr_iNumVects_ADDR);
//  VoC_lw16d_set_inc(REG3,decorr_pswVects_ADDR_ADDR,1);//address of pswVects[] in reg3

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC1,1,DEFAULT);

    VoC_loop_r(1,REG5);
    VoC_movreg16(REG0,REG3,DEFAULT);
    VoC_lw32_d(RL7,CONST_0x00008000L_ADDR);
    VoC_lw16_d(REG1,decorr_pswGivenVect_ADDR_ADDR);

    VoC_jal(CII_g_corr2_opt);//return swCShift in reg1,L_Temp1 in acc1
    VoC_sub16_rr(REG5,REG1,REG7,DEFAULT);//swShiftSum in reg0
    VoC_bnltz32_r(decorr01,ACC1);
    VoC_sub32_dr(ACC0,CONST_0_ADDR,ACC1);
decorr01:
    VoC_shr32_ri(ACC0,1,DEFAULT);


    VoC_add32_rr(REG01,ACC0,RL7,DEFAULT);
    VoC_movreg16(REG0,REG1,DEFAULT);
    VoC_movreg16(REG1,REG6,IN_PARALLEL);
    VoC_jal(CII_DIV_S);//swTemp in reg2

    VoC_bngtz32_r(decorr02,ACC1);
    VoC_sub16_rr(REG2,REG4,REG2,DEFAULT);
decorr02:
    VoC_lw32z(ACC0,DEFAULT);
    VoC_movreg16(ACC0_HI,REG2,DEFAULT);
    VoC_jal(CII_NORM_L_ACC0);

    VoC_add16_rr(REG0,REG5,REG1,DEFAULT);
    VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
    // CII_NORM_L_ACC0 put -swQShift instead of swQShift in reg1, 'cause norm_l is always followed by shl
    VoC_shr16_rr(REG2,REG1,DEFAULT);

//////////////////////////////////////////////////////////////////
    VoC_movreg16(REG5,REG2,DEFAULT);
// SHIRT_R
    VoC_bngtz16_r(decorr04,REG0);
    VoC_shr32_rr(REG45,REG0,DEFAULT);
    VoC_add32_rd(REG45,REG45,CONST_0x00008000L_ADDR);
    VoC_bngt16_rd(decorr03,REG0,CONST_15_ADDR);
    VoC_lw16i_short(REG5,0,DEFAULT);//swout=0;
decorr03:

////////////////////////////////////////////////////////////////
    VoC_lw16i_short(REG0,0,DEFAULT);

decorr04:
    VoC_push32(REG67,DEFAULT);
    VoC_lw16d_set_inc(REG1,decorr_pswGivenVect_ADDR_ADDR,1);


    VoC_lw32_d(REG67,CONST_0x00008000L_ADDR);
    VoC_movreg32(ACC0,REG67,DEFAULT);
    VoC_lw16inc_p(REG4,REG1,IN_PARALLEL);

    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_shr16_rr(REG4,REG0,IN_PARALLEL);

    VoC_mac32_rr(REG5,REG4,DEFAULT);

    VoC_loop_i_short(40,DEFAULT);
    VoC_startloop0;
    VoC_msu32_rp(REG6,REG3,DEFAULT);

    VoC_lw16inc_p(REG4,REG1,DEFAULT);

    VoC_movreg32(ACC0,REG67,DEFAULT);
    VoC_shr16_rr(REG4,REG0,IN_PARALLEL);

    VoC_mac32_rr(REG5,REG4,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(ACC0_HI,REG3,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop0;

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);
    VoC_endloop1;
decorr_end:
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}


void CII_g_corr2_opt(void)//return in reg1;output in acc1,ues reg0,1,2;acc0,1
{
//  inc0=1;
//  inc1=1;

    VoC_push16(RA,DEFAULT);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_loop_i_short(40,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG2,REG0,DEFAULT);
    VoC_mac32inc_rp(REG2,REG1,DEFAULT);
    VoC_endloop0
    VoC_NOP();
    VoC_movreg32(ACC1,ACC0,DEFAULT);
    VoC_jal(CII_NORM_L_ACC0);
    VoC_pop16(RA,DEFAULT);
    VoC_sub16_dr(REG2,CONST_0_ADDR,REG1);
    VoC_shr32_rr(ACC1,REG2,DEFAULT);
//  VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
    VoC_return;
}



/**************************************************************/
//IN:
//REG0 pswCoeff       INC0 2
//pswCoeff aligned on even address
//REG1 pswIn          INC1 1
//REG2 pswXstate
//REG3 pswYstate      INC3 1
//REG4 swPreFirDownSh
//REG5 swFinalUpShift


/**************************************************************/


void CII_iir_d_opt(void)
{

    /*    int i;
        FILE* fp;
        fp=fopen("iir_d_s","w");


        REGS[REG0].reg = 0x100;
        REGS[REG1].reg = 0;
        REGS[REG2].reg = 0x110;
        REGS[REG3].reg = 0x120;
        REGS[REG4].reg = 0;
        REGS[REG5].reg = 1;


    */

#if 0
    voc_short  CII_iir_d_temp              , 10,x                 // [10]
#endif




    VoC_lw16inc_p(RL6_LO,REG0,DEFAULT);
    VoC_lw16inc_p(RL6_HI,REG0,DEFAULT);
    VoC_lw16inc_p(RL7_LO,REG0,DEFAULT);
    VoC_lw16inc_p(RL7_HI,REG0,DEFAULT);
    VoC_lw16_p(REG7,REG0,DEFAULT);
    VoC_movreg16(REG6,RL7_HI,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0,CII_iir_d_temp,2);       // reorder pswCoeff

    VoC_push32(REG23, DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_movreg16(REG6,RL7_LO,DEFAULT);
    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_sw16_p(REG6,REG0,DEFAULT);

    VoC_lw16inc_p(REG6, REG3, DEFAULT);
    VoC_lw16inc_p(RL6_LO, REG3, DEFAULT);
    VoC_lw16inc_p(REG7, REG3, DEFAULT);
    VoC_lw16inc_p(RL6_HI, REG3, DEFAULT);

    VoC_lw32_p(RL7, REG2, DEFAULT);

//pswYstate[0] in REG6
//pswYstate[1] in RL6_LO
//pswYstate[2] in REG7
//pswYstate[3] in RL6_HI
//pswXstate[0] in RL7_LO
//pswXstate[1] in RL7_HI
    VoC_lw16i(REG0,CII_iir_d_temp);
    VoC_lw32_p(REG23, REG1, DEFAULT);

    VoC_push16(REG0,DEFAULT);
    VoC_lw32z(ACC0, IN_PARALLEL);

    VoC_loop_i(1, 80)


    VoC_shr16_rr(REG3, REG4, DEFAULT);  // swx1 = shr(pswIn[loop_cnt + 1], swPreFirDownSh);
    VoC_shr16_rr(REG2, REG4, DEFAULT);      // swx0 = shr(pswIn[loop_cnt], swPreFirDownSh);

//  reg2 swx0
//  reg3 swx1


    VoC_loop_i_short(2,DEFAULT);
    VoC_startloop0;
    VoC_push32(REG45,DEFAULT);
    VoC_movreg32(REG45,RL6,DEFAULT);

    VoC_bimac32inc_rp(REG45, REG0);
    VoC_movreg32(REG45,RL7,DEFAULT);
    VoC_movreg16(RL7_LO,RL7_HI,IN_PARALLEL);

    VoC_shr32_ri(ACC0, 14, DEFAULT);
    VoC_movreg16(RL7_HI,REG2,IN_PARALLEL);

    VoC_bimac32inc_rp(REG67, REG0);
    VoC_bimac32inc_rp(REG45, REG0);

    VoC_mac32_rp(REG2, REG0, DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);

    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_movreg16(REG6, REG7, IN_PARALLEL);

    VoC_shr32_ri(ACC0, -1, DEFAULT);
    VoC_movreg16(RL6_LO,RL6_HI,IN_PARALLEL);

    VoC_movreg16(REG7, ACC0_HI, DEFAULT);
    VoC_movreg16(REG2, ACC0_LO, IN_PARALLEL);//swtemp

    VoC_shr16_ri(REG2, 2, DEFAULT);
    VoC_shr32_rr(ACC0, REG5, IN_PARALLEL);

    VoC_and16_rd(REG2, CONST_0x3fff_ADDR);

//    reverse the place
//          pswYstate_3<->pswYstate_1
//          pswYstate_2<->pswYstate_0
//          pswXstate_1<->pswXstate_0
//          swx1 -> swx0

    VoC_add32_rd(ACC0, ACC0, CONST_0x00008000L_ADDR);

    VoC_movreg16(RL6_HI, REG2, DEFAULT);
    VoC_movreg16(REG2, REG3, IN_PARALLEL);

    VoC_sw16inc_p(ACC0_HI, REG1, DEFAULT);
    VoC_lw32z(ACC0, IN_PARALLEL);

    VoC_endloop0;
    VoC_lw32_p(REG23, REG1, DEFAULT);

    VoC_endloop1;

    VoC_pop32(REG23, DEFAULT);// move 23 to 01
    VoC_pop16(REG0,DEFAULT);



    VoC_sw16inc_p(REG6, REG3, DEFAULT);
    VoC_sw16inc_p(RL6_LO, REG3, DEFAULT);
    VoC_sw16inc_p(REG7, REG3, DEFAULT);
    VoC_sw16inc_p(RL6_HI, REG3, DEFAULT);

    VoC_sw32inc_p(RL7, REG2, DEFAULT);


    VoC_return;
}


// ********************************************
// CII_rcToCorrDpL_opt
//
//
// note :  1st two parameters are not passed, because
//         they are always the same constants :
//         swAshift = 4, swAscale = 0x800
//
// input:  REG4 : pswRc (param2), INC0 = 2
//         REG1 : pL_R  (param3), INC1 = 2
//
// output: none
//
// ********************************************


void CII_rcToCorrDpL_opt(void)
{

#if 0
    voc_short LOCAL_rcToCorrDpL_pL_tmpSpace_ADDR    ,20,x   // long[10]
    voc_short LOCAL_rcToCorrDpL_pL_ASpace_ADDR      ,20,x   // long[10]

#endif
    //  pL_R[0] = LW_MAX; push RA
    VoC_lw32_d(RL7,CONST_0x7fffffff_ADDR);
    VoC_push16(RA,DEFAULT);

    VoC_sw32inc_p(RL7,REG1,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    // get pL_tmpSpace
    VoC_lw16i_set_inc(REG2,LOCAL_rcToCorrDpL_pL_tmpSpace_ADDR,2);
    VoC_movreg16(REG3,REG2,DEFAULT);

    // move pL_tmpSpace in REG0; push ++pswRc
    VoC_movreg16(REG0,REG2,DEFAULT);

    // push ptmpSpace & ptmpSpace
    VoC_push32(REG23,DEFAULT);

    // get pL_ASpace
    VoC_lw16i_set_inc(REG3,LOCAL_rcToCorrDpL_pL_ASpace_ADDR,-2);
    VoC_movreg16(REG2,REG3,DEFAULT);

    // swAscale in REG6
    VoC_lw16_d(REG6,CONST_0x0800_ADDR);

    // push pL_ASpace & pL_ASpace
    VoC_push32(REG23,DEFAULT);

    // i = 0 in REG7
    VoC_lw16i_short(REG7,0,IN_PARALLEL);



    // STACK16                 STACK32
    // --------------------------------------
    // RA                      ptmpSpace & ptmpSpace
    //                         pL_ASpace & pL_ASpace


    // REG0 : ptmpSpace,     INC0 =  2
    // REG1 : &pL_R[1],      INC1 =  2
    // REG2 : pL_ASpace,     INC2 =  2
    // REG3 : pL_ASpace,     INC3 = -2
    // REG4 :
    // REG6 : swAscale
    // REG7 : i = 0


    VoC_loop_i(1, 10);



    // STACK16                 STACK32
    // --------------------------------------
    // RA                      ptmp & ptmp
    //                         pL_A & pL_A

    // REG0 : ptmp,          INC0 =  2
    // REG1 : &pL_R[i+1],    INC1 =  2
    // REG2 : pL_A,          INC2 =  2
    // REG3 : pL_A,          INC3 = -2
    // REG4 :
    // REG6 : swAscale
    // REG7 : i


    // general purpose 1
    VoC_lw16i_short(REG5,1,DEFAULT);
    // pswRc[i]
    VoC_lw16_p(REG4,REG4,DEFAULT);
    // &pswRc[i++]
    VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);


    // &pL_A[i - 1]
    VoC_inc_p(REG3,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);
    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);


    // push &pswRc[i+1]
    VoC_push16(REG5,IN_PARALLEL);


    // STACK16                 STACK32
    // --------------------------------------
    // RA                      ptmp & ptmp
    // &pswRc[i+1]             pL_A & pL_A


    // REG0 : ptmp,          INC0 =  2
    // REG1 : &pL_R[i+1],    INC1 =  2
    // REG2 : pL_A,          INC2 =  2
    // REG3 : &pL_A[i-1],    INC3 = -2
    // REG4 : pswRc[i],
    // REG6 : swAscale
    // REG7 : i


    VoC_bez16_r(rcToCorrDpL_opt_First_Loop,REG7);

    // for (j = 0; j <= i - 1; j++)
    VoC_loop_r_short(REG7,DEFAULT);
    VoC_lw32inc_p(ACC0,REG3,IN_PARALLEL);
    VoC_startloop0

    /****************************/
    // CII_L_mpy_ls_opt
    // input ACC0, REG4
    // output ACC0
    // other used register REG5
    // not modified : REG4
    /****************************/

    VoC_jal(CII_L_mpy_ls_opt);
    VoC_add32inc_rp(ACC0,ACC0,REG2,DEFAULT);
    VoC_lw32inc_p(ACC0,REG3,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

rcToCorrDpL_opt_First_Loop:

    // L_mult(swAscale, pswRc[i]);
    VoC_multf32_rr(ACC0,REG6,REG4,DEFAULT);

    // L_sum = 0 in ACC1
    VoC_lw32z(ACC1,DEFAULT);
    // i++
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);
    // pL_tmp[i] = L_mult(swAscale, pswRc[i]);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);



    // Swap pL_A and pL_tmp buffers
    // put new pL_A's in REG23
    VoC_lw32_sd(REG23,1,DEFAULT);
    VoC_lw32_sd(ACC0,0,DEFAULT);
    VoC_sw32_sd(REG23,0,DEFAULT);

    // &pL_R[i+1] in REG3
    VoC_movreg16(REG3,REG1,IN_PARALLEL);

    // end of swap
    VoC_sw32_sd(ACC0,1,DEFAULT);

    // &pL_R[i] in REG3
    VoC_inc_p(REG3,DEFAULT);


    // STACK16                 STACK32
    // --------------------------------------
    // RA                      ptmp & ptmp
    // &pswRc[i+1]             pL_A & pL_A


    // REG0 : &pL_A[i+1],    INC0 =  2
    // REG1 : &pL_R[i+1],    INC1 =  2
    // REG2 : &pL_A[0],      INC2 =  2
    // REG3 : &pL_R[i],      INC3 = -2
    // REG4 : pswRc[i],
    // REG6 : swAscale
    // REG7 : i+1



    VoC_loop_r_short(REG7,DEFAULT);
    VoC_push32(REG67,IN_PARALLEL);
    VoC_startloop0

    // ******************************
    //  CII_L_mpy_ll_opt
    //  input  : REG45 (1), REG67 (2)
    //  output : ACC0
    //  other used : RL7
    // *****************************

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_lw32inc_p(REG67,REG3,DEFAULT);
    VoC_jal(CII_L_mpy_ll_opt);
    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);

    VoC_endloop0

    // L_shl(L_sum, swAshift);
    VoC_shr32_ri(ACC1,-4,DEFAULT);

    // pop context
    VoC_pop16(REG4,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);

    // pL_R[i + 1] = L_shl(L_sum, swAshift);
    VoC_sw32inc_p(ACC1,REG1,DEFAULT);

    // get ptmp & pL_A
    VoC_lw32_sd(REG23,1,DEFAULT);
    VoC_lw32_sd(REG23,0,DEFAULT);
    VoC_movreg16(REG0,REG2,IN_PARALLEL);


    // STACK16                 STACK32
    // --------------------------------------
    // RA                      ptmp & ptmp
    //                         pL_A & pL_A

    // REG0 : ptmp,          INC0 =  2
    // REG1 : &pL_R[i+2],    INC1 =  2
    // REG2 : pL_A,          INC2 =  2
    // REG3 : pL_A,          INC3 = -2
    // REG4 :
    // REG6 : swAscale
    // REG7 : i+1

    VoC_endloop1

    // unpile stacks
    VoC_pop16(RA,DEFAULT);
    VoC_pop32(REG23,IN_PARALLEL);
    VoC_pop32(REG23,DEFAULT);

    VoC_return;

}


void CII_resetDec(void)
{

    VoC_push16(RA, DEFAULT);
    VoC_sw16_d(REG2,DECODER_resetflag_old_ADDR);
    VoC_lw16i_set_inc(REG2,DE_RESET_ZERO_ADDR,2);
    VoC_lw16i_short(REG4,12,DEFAULT);
    VoC_jal(CII_reset_set_zeros);
    VoC_lw16i_set_inc(REG2,DE_RESET_VALUE_ADDR,1);
    VoC_lw16i_short(REG4,8,DEFAULT);
    VoC_jal(CII_reset_set_values);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_sw32_d(ACC0, GLOBAL_swPreBfi_ADDR);

    VoC_return;
}

void CII_resetHREnc(void)
{

    VoC_push16(RA, DEFAULT);
    VoC_lw16i_set_inc(REG2,EN_RESET_ZERO_ADDR,2);
    VoC_lw16i_short(REG4,30,DEFAULT);
    VoC_jal(CII_reset_set_zeros);
    VoC_lw16i_short(REG4,8,DEFAULT);
    VoC_lw16i_set_inc(REG2,EN_RESET_VALUE_ADDR,1);
    VoC_jal(CII_reset_set_values);
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/*************************************************************************/
// by zhang
/*reset functions**/
/*
parameters:
REG4:num of blocks to be set
REG2:&(pointer table) inc 2// put to ROM the array
*/
void CII_reset_set_zeros(void)
{
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_loop_r(1,REG4)
    VoC_lw32inc_p(REG01,REG2,DEFAULT);
    VoC_lw16i(REG4, HR_GLOBAL_BEGIN);

    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_loop_r_short(REG1,DEFAULT)
    VoC_startloop0
    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1
    VoC_return
}

/*
parameters:
REG4:num of blocks to be set
REG2:&(pointer table) inc 1
*/
void CII_reset_set_values(void)
{

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_loop_r(1,REG4)
    VoC_lw16inc_p(  REG1,REG2,DEFAULT); //ptr_w
    VoC_lw16i(REG4, HR_GLOBAL_BEGIN);

    VoC_lw16inc_p(  REG5,REG2,DEFAULT); //lng
    VoC_lw16inc_p(  REG0,REG2,DEFAULT); //ptr_r
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);
    VoC_lw16inc_p(  REG6,REG0,DEFAULT);
    VoC_loop_r_short(REG5,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_lw32z(ACC0,DEFAULT);
    VoC_endloop1

    VoC_return
}

//////////////////////////////////////////////////////////////////
//  funcions in vad.c
//////////////////////////////////////////////////////////////////

/***************************************************
 Function: void CII_step_up(void)

 IN:    REG5     swNp
        REG2     &pswVpar       INCR2     1
                                INCR3     -2
        RL6_HI   &pswAav1

 OUT:   REG7     pswAav1[swNp]

 USE:   ALL REGS
 ****************************************************/
void CII_step_up_opt(void)
{
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_set_inc(REG1, HR_VAD_STEP_UP_pL_coef_ADDR, 2);        //pL_coef[m]  in REG1


    VoC_lw16i(REG7,0x2000);                     // pL_coef[0] = L_shl(0x4000L, 15);


    VoC_push16(REG1,DEFAULT);

    VoC_sw32inc_p(REG67,REG1,DEFAULT);
    VoC_lw16i_set_inc(REG0, HR_VAD_STEP_UP_pL_work_ADDR+2, 2);
    // REG0 for VAD_STEP_UP_pL_work_ADDR+2
    // REG1 for VAD_STEP_UP_pL_coef_ADDR+2
    VoC_movreg32(ACC1,REG01,DEFAULT);

    // REG4 for m-1 (m=1 for initilization)
    VoC_lw16i_short(REG4, 0,DEFAULT);

    VoC_loop_r(1, REG5);
    // pswVpar[m - 1] in REG6
    VoC_lw16inc_p(REG6, REG2, DEFAULT);
    // REG3 for pL_coef[m - i] (init i=1)
    VoC_movreg16(REG3, REG1, DEFAULT);

    VoC_bez16_r(VAD_STEP_UP_LABEL1,REG4);

    VoC_movreg32(REG01,ACC1,DEFAULT);

    VoC_lw32inc_p(RL7, REG3,DEFAULT);

    VoC_movreg16(REG7, RL7_HI, DEFAULT);    //swTemp in REG7
    VoC_loop_r_short(REG4,DEFAULT);
    VoC_startloop0              //for (i = 1; i < m; i++)

    VoC_multf32_rr(ACC0,REG6, REG7, DEFAULT);   // swTemp = extract_h(pL_coef[m - i]);
    VoC_lw32inc_p(RL7, REG3,DEFAULT);   //pL_work[i] = L_mac(pL_coef[i], pswVpar[m - 1], swTemp);

    VoC_add32inc_rp(ACC0,ACC0,REG1,DEFAULT);
    VoC_movreg16(REG7, RL7_HI, DEFAULT);    //swTemp in REG7
    VoC_sw32inc_p(ACC0, REG0, DEFAULT);
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
    VoC_startloop0
    VoC_lw32inc_p(REG67, REG0, DEFAULT);            //pswAav1[i] = extract_h(L_shr(pL_coef[i], 3));
    VoC_shr32_ri(REG67, 3, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG7, REG2, DEFAULT);
    VoC_endloop0

    VoC_return;
}





/***************************************************
  Function: void CII_vad_algorithm(void)
  input   :
  Output  :
  Description: All the input and output are finished inside
 * Version 1.0 Create  by Lang
***************************************************/
void CII_vad_algorithm(void)
{

#if 0

    voc_short   HR_VAD_ALGORITHM_swStat_ADDR                ,y      //1 short
    voc_short   HR_VAD_ALGORITHM_swNormRav1_ADDR            ,y      //1 short
    voc_short   HR_VAD_ALGORITHM_pswRav1_ADDR               ,10,y       //[10]shorts
    voc_short   HR_VAD_ALGORITHM_pL_av0_ADDR                ,18,y       //[18]shorts
    voc_short   HR_VAD_ALGORITHM_pL_av1_ADDR                ,18,y       //[18]shorts
    voc_short   HR_VAD_ALGORITHM_swM_pvad_ADDR              ,y      //1 short
    voc_short   HR_VAD_ALGORITHM_swE_pvad_ADDR              ,y      //1 short
    voc_short   HR_VAD_ALGORITHM_swM_acf0_ADDR              ,y      //1 short
    voc_short   HR_VAD_ALGORITHM_swE_acf0_ADDR              ,y      //1 short
    voc_short   HR_VAD_PREDICTOR_VALUES_pswAav1_ADDR        ,10,y       //[10]shorts
    voc_short   HR_VAD_PREDICTOR_VALUES_pswVpar_ADDR        ,8,y        //[8]shorts
    voc_short   HR_VAD_STEP_UP_pL_coef_ADDR                 ,18,y       //[18]shorts
    voc_short   HR_VAD_STEP_UP_pL_work_ADDR                 ,18,y       //[18]shorts
    voc_short   HR_VAD_COMPUTE_RAV1_pL_work_ADDR            ,18,y        //[18]shorts
    voc_short   HR_VAD_TONE_DETECTION_pswA_ADDR             ,4,y         //[4]shorts
    voc_short   HR_SCHUR_RECURSION_pswPp_ADDR               ,10,y        //[10]shorts
    voc_short   HR_SCHUR_RECURSION_pswKk_ADDR               ,10,y        //[10]shorts


#endif






    VoC_push16(RA, DEFAULT);


    VoC_lw16i_set_inc(REG0, LOCAL_AFLAT_pL_VadAcf_ADDR, 2);
    VoC_lw16d_set_inc(REG3, LOCAL_AFLAT_swVadScalAuto_ADDR, 1);
    VoC_lw16i_set_inc(REG2, HR_STATIC_VAD_pswRvad_ADDR, 1);
    VoC_lw16_d(RL6_LO,HR_STATIC_VAD_swNormRvad_ADDR);

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
    VoC_lw16i(REG4,32);


    //*pswE_acf0 = add(32, shl(swScaleAcf, 1));
    VoC_add16_rr(REG3, REG3, REG4,DEFAULT);
    //*pswE_acf0 = sub(*pswE_acf0, swNormAcf);
//  VoC_add16_rr(REG5, REG3, REG1,DEFAULT);
    VoC_sub16_rr(REG5, REG3, REG1,DEFAULT);
    //swShift in REG1
    //VoC_add16_rd(REG1, REG1,CONST_3_ADDR);
    VoC_sub16_dr(REG1,CONST_3_ADDR, REG1);
    VoC_loop_i_short(9,DEFAULT);
    //L_temp=0
    VoC_lw32z (ACC0,IN_PARALLEL);
    VoC_startloop0
    //pswSacf[i] = extract_h(L_shl(pL_acf[i], swShift));
    VoC_lw32inc_p(REG67, REG0,DEFAULT);
    VoC_shr32_rr(REG67, REG1,DEFAULT);
    VoC_mac32inc_rp (REG7, REG2, DEFAULT);
    VoC_bne16_rd(ENERGY_COMPUTATION_First,REG4,CONST_32_ADDR);      //L_temp = L_mac(L_temp, pswSacf[i], pswRvad[i]);
    VoC_shr16_ri(REG7,-3,DEFAULT);
    VoC_movreg16(REG4,REG7,DEFAULT);
    VoC_shr32_ri(ACC0, 1, DEFAULT);
    VoC_movreg32(RL7, REG45, DEFAULT);
ENERGY_COMPUTATION_First:
    VoC_lw16i_short(REG4,14,DEFAULT);
    VoC_endloop0;

    //*pswE_pvad = add(*pswE_acf0, 14);
    VoC_add16_rr(REG5, REG5, REG4, DEFAULT);
    // REG4 for swNormRvad
    VoC_movreg16(REG4,RL6_LO,IN_PARALLEL);

    //*pswE_pvad = sub(*pswE_pvad, swNormRvad);
    VoC_sub16_rr(REG5, REG5, REG4, DEFAULT);


    //L_temp in ACC0
    VoC_bgtz32_r(ENERGY_COMPUTATION_100, ACC0);
    VoC_lw32_d(ACC0, CONST_1_ADDR);
ENERGY_COMPUTATION_100:

    // ACC0 is surely >0, so after following instrution
    // ACC0 = L_shl(L_temp, swNormProd)
    VoC_jal(CII_NORM_L_ACC0);

    //REG5 for *pswE_pvad
    //    VoC_add16_rr(REG5, REG5, REG1,DEFAULT);
    VoC_sub16_rr(REG5, REG5, REG1,DEFAULT);
    //*pswM_pvad = extract_h(L_shl(L_temp, swNormProd));
    VoC_movreg16(REG4, ACC0_HI, IN_PARALLEL);

ENERGY_COMPUTATION_EXIT:

    /*******************************************
     *Function end: void CII_energy_computation_opt(void)

    /*******************************************/

    VoC_sw32_d(RL7,HR_VAD_ALGORITHM_swM_acf0_ADDR);
    VoC_sw32_d(REG45, HR_VAD_ALGORITHM_swM_pvad_ADDR);




    VoC_lw16_d(REG1, LOCAL_AFLAT_swVadScalAuto_ADDR);   //swScale = sub(10, shl(swScaleAcf, 1));
    VoC_lw16i_set_inc(REG0, LOCAL_AFLAT_pL_VadAcf_ADDR, 2);
    VoC_lw16i_set_inc(REG2, HR_VAD_ALGORITHM_pL_av0_ADDR, 2);
    VoC_lw16i_set_inc(REG3, HR_VAD_ALGORITHM_pL_av1_ADDR, 2);

    /*****************************************************/
// inline function: CII_average_acf_opt

// REG0 addr of PL_ACF[0],INC0 = 2
// REG2 addr of L_AV0[0], INC2 = 2
// REG3 addr of L_AV1[0], INC3 = 2
// REG1 ScalAcf
    /****************************************************/

    VoC_shr16_ri(REG1,-1,DEFAULT);
    VoC_sub16_dr (REG5,CONST_10_ADDR,REG1);             //scale

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i(REG7,34);

    VoC_push32(REG23,DEFAULT);  //&L_av0[i]




//  VoC_lw16i(REG4,STATIC_VAD_swPt_sacf_ADDR);
//  VoC_mult16_pd (REG4,REG4,CONST_2_ADDR);
    VoC_lw16i_short(REG4,2,DEFAULT);
    VoC_mult16_rd (REG4,REG4,HR_STATIC_VAD_swPt_sacf_ADDR);

    VoC_lw16i_set_inc(REG3,HR_STATIC_VAD_pL_sacf_ADDR,18);          //&L_sacf[i]

    VoC_add16_rr (REG4, REG3, REG4, DEFAULT);               //&L_sacf[pt_sacf+i]

//  VoC_lw32_d(REG01,1,PARA1_ADDR_ADDR);                    //&r_h[i] and &r_l[i]

    VoC_loop_i_short(9,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(RL7,REG0,DEFAULT);
    VoC_shr32_rr(RL7,REG5,DEFAULT);         //L_temp
    VoC_movreg32(RL6,RL7,DEFAULT);
    VoC_add32inc_rp(RL7,RL7,REG3,DEFAULT);

    VoC_add32inc_rp(RL7,RL7,REG3,DEFAULT);
    /*RAM_ACCESS*/
    VoC_add32_rp(RL7,RL7,REG3,DEFAULT);

    exit_on_warnings=OFF;
    VoC_sw32_p(RL6,REG4,DEFAULT);
    exit_on_warnings=ON;

    VoC_add16_rd(REG4,REG4,CONST_2_ADDR);
    VoC_sw32inc_p(RL7,REG2,DEFAULT);
    VoC_sub32_rr(REG23,REG23,REG67,IN_PARALLEL);


    VoC_endloop0


    VoC_lw16i_short(REG6,2,DEFAULT);
    VoC_mult16_rd (REG0, REG6,HR_STATIC_VAD_swPt_sav0_ADDR);
    VoC_lw16i_set_inc(REG3,HR_STATIC_VAD_pL_sav0_ADDR,2);
    VoC_add16_rr (REG0, REG0, REG3, DEFAULT);    // reg3 address of the L_SAV0[pt_sav0]

    VoC_pop32(REG23,DEFAULT);


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

    VoC_add16_rd(REG6,REG5,HR_STATIC_VAD_swPt_sacf_ADDR);   //REG6:pt_sacf

    VoC_bne16_rd(ACF_AVERAGING101,REG4,HR_STATIC_VAD_swPt_sacf_ADDR);
//      VoC_lw16z(REG6,DEFAULT);
    VoC_lw16i_short(REG6,0,DEFAULT);

ACF_AVERAGING101:

    VoC_lw16i_short(REG4,27,DEFAULT);

    VoC_add16_rd(REG7,REG5,HR_STATIC_VAD_swPt_sav0_ADDR);//REG7:pt_sav0

    VoC_bne16_rd(ACF_AVERAGING103,REG4,HR_STATIC_VAD_swPt_sav0_ADDR)
//      VoC_lw16z(REG6,DEFAULT);
    VoC_lw16i_short(REG7,0,DEFAULT);
ACF_AVERAGING103:


    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_sw16_d(REG6,HR_STATIC_VAD_swPt_sacf_ADDR);
    VoC_sw16_d(REG7,HR_STATIC_VAD_swPt_sav0_ADDR);

    /*****************************************************/
// end inline function: CII_average_acf_opt

    /****************************************************/


    /************************************************
      inline function: void predictor_values()

    ************************************************/



    // load CII_schur_recursion parameters
    VoC_lw16i_set_inc(REG2, HR_VAD_ALGORITHM_pL_av1_ADDR,2);
    VoC_lw16i(REG0, HR_VAD_PREDICTOR_VALUES_pswVpar_ADDR);  //      pswVpar[i] = 0;

    /**********************************************
      inline Function: void CII_schur_recursion(void)
    IN:
        REG2 &pL_av1    INCR2=2
        REG0 &pswVpar
                        INCR3=1
    **********************************************/

    VoC_lw32_p(ACC0,REG2,DEFAULT);

    // save &pswVpar in ACC0_HI
    VoC_movreg16(ACC1_HI,REG0,DEFAULT);
    // n in ACC1_LO
    VoC_lw16i_short(ACC1_LO,0,DEFAULT);

    VoC_bez32_r(schur_recursion_End1, ACC0);


    VoC_jal(CII_NORM_L_ACC0);                       //swTemp = norm_l(pL_av1[0]);

    VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
    VoC_lw16i_set_inc(REG0, HR_SCHUR_RECURSION_pswKk_ADDR+9, -1);
    VoC_lw16i_set_inc(REG3, HR_SCHUR_RECURSION_pswPp_ADDR, 1);

    VoC_loop_i_short(9, DEFAULT);                   //  for (i = 0; i <= 8; i++)
    VoC_startloop0                          //{
    VoC_lw32inc_p(REG67, REG2, DEFAULT);            //  pswAcf[i] = extract_h(L_shl(pL_av1[i], swTemp));
    VoC_shr32_rr(REG67, REG1, DEFAULT);             //  if ((i>0)&&(i<8))
    VoC_NOP();
    // also put values in pswKk[0,1,9] which will not be used
    // pswKk should have a space of 10 instead of 9
    VoC_sw16inc_p(REG7,REG0,DEFAULT);               //  }
    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    VoC_endloop0


    VoC_loop_i(1, 8);
    //if (pswPp[0] < abs_s(pswPp[1]))
    VoC_lw32_d(REG45, HR_SCHUR_RECURSION_pswPp_ADDR);         //{

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
    VoC_loop_r_short(REG2,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG7, REG3, DEFAULT);
    VoC_endloop0
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
    VoC_lw16i_set_inc(REG1, HR_SCHUR_RECURSION_pswPp_ADDR, 1);
    VoC_lw16i_set_inc(REG0, HR_SCHUR_RECURSION_pswKk_ADDR+8, -1);   // pswKk[9 - m] in REG0

    VoC_add16_rd(REG2,REG1,CONST_2_ADDR);               // pswPp[1 + m] in REG2

    VoC_sw16inc_p(REG7, REG1,DEFAULT);

    VoC_loop_r_short(REG6,DEFAULT)
    VoC_startloop0

    VoC_multf32_rp(ACC0, REG5, REG0, DEFAULT);
    exit_on_warnings = OFF;
    VoC_multf32_rp(ACC0, REG5, REG2, DEFAULT);          //  pswPp[m] = add(pswPp[1 + m], mult_r(pswKk[9 - m], pswVpar[n]));
    exit_on_warnings = ON;
    VoC_add32_rr(REG67, ACC0, RL6,DEFAULT);     //for (m = 1; m <= (7 - n); m++)
    VoC_add16inc_rp(REG4, REG7, REG2, DEFAULT);               //{

    VoC_add32_rr(REG67, ACC0, RL6,DEFAULT);     // }
    VoC_add16_rp(REG7, REG7, REG0, DEFAULT);
    VoC_sw16inc_p(REG4, REG1, DEFAULT);                    //  pswKk[9 - m] = add(pswKk[9 - m], mult_r(pswPp[1 + m], pswVpar[n]));
    VoC_sw16inc_p(REG7, REG0, DEFAULT);

    VoC_endloop0
    VoC_NOP();
    VoC_endloop1

schur_recursion_End:


    /********************************************************
      end inline Function: void CII_schur_recursion(void)

    **********************************************************/


    VoC_lw16i_short(REG5, 8,DEFAULT);
    VoC_lw16i_set_inc(REG3, HR_VAD_PREDICTOR_VALUES_pswAav1_ADDR,-2);
    VoC_lw16i_set_inc(REG2, HR_VAD_PREDICTOR_VALUES_pswVpar_ADDR,1);
    VoC_movreg16(RL6_HI,REG3,DEFAULT);

    VoC_jal(CII_step_up_opt);



    // load parameters for CII_compute_rav1

    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_lw16i_set_inc(REG2, HR_VAD_PREDICTOR_VALUES_pswAav1_ADDR, 1);   // pL_work[i] = 0;  ;REG2->pswAav1[k]
    VoC_lw16i_set_inc(REG3, HR_VAD_ALGORITHM_pswRav1_ADDR, 1);
    /********************************************************************
      function: void compute_rav1_opt(void)

      IN:  REG2  pswAav1  INCR2=1
           REG3  pswRav1  INCR3=1
                          INCR1=1
      OUT: REG2  *pswNormRav1

    *******************************************************************/

    VoC_lw16i_set_inc(REG0, HR_VAD_STEP_UP_pL_work_ADDR, 2);
    VoC_lw16i_short(REG6,9, DEFAULT);
    VoC_push16(REG0,DEFAULT);

    VoC_movreg16(REG1,REG2,DEFAULT);

    VoC_loop_i(1, 9);

    VoC_lw32z(ACC0, DEFAULT);
    // REG6 for 9-i
    VoC_loop_r_short(REG6,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG5, REG1, DEFAULT);
    VoC_mac32inc_rp(REG5, REG2, DEFAULT);
    VoC_endloop0

    // REG2 for pswAav1[k]
    VoC_sub16_rr(REG2, REG2, REG6, DEFAULT);
    // REG6 (9-i)--
    VoC_sub16_rd(REG6, REG6, CONST_1_ADDR);
    // REG1 for pswAav1[i+1]
    VoC_sub16_rr(REG1, REG1, REG6, DEFAULT);

    VoC_sw32inc_p(ACC0, REG0, DEFAULT);
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
//  VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
    VoC_sub16_rr(REG1,REG6,REG1,DEFAULT); //REG6 is 0
    VoC_loop_i_short(9, DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p(REG45, REG0, DEFAULT);
    VoC_shr32_rr(REG45, REG1, DEFAULT);
    VoC_sub16_rr(REG2,REG6,REG1, DEFAULT);
    VoC_sw16inc_p(REG5, REG3, DEFAULT);
    VoC_endloop0

    /********************************************************************
      end function: void compute_rav1_opt(void)

    /********************************************************************/
    // save parameters for CII_compute_rav1
    VoC_sw16_d(REG2, VAD_ALGORITHM_swNormRav1_ADDR);


    /************************************************
      end inline function: void predictor_values()

    ************************************************/

    // load parameters of spectral_comparison_opt
    VoC_lw16i_set_inc(REG0, HR_VAD_ALGORITHM_pL_av0_ADDR, 2);
    VoC_push16(REG2,DEFAULT);
    VoC_lw16i_set_inc(REG2,HR_VAD_ALGORITHM_pswRav1_ADDR+1, 1);

    /***************************************************************************
     function spectral_comparison_opt

     IN:     REG0        &pL_av0  INCR0=2
             STACK[0]    *swNormRav1
             REG2        pswRav1[0] INCR2=1

     OUT:    REG3        *pswStat
     ****************************************************************************/

    VoC_lw32inc_p(ACC0, REG0,DEFAULT);


    VoC_movreg32(REG45, ACC0, DEFAULT);
    /************************************************************
     *Function: CII_NORM_L_ACC0
     *input: ACC0;
     *return:REG1;
     ***********************************************************/
    VoC_jal(CII_NORM_L_ACC0);
    //  VoC_add16_rd(REG3, REG1, CONST_3_ADDR);
    VoC_sub16_dr(REG3, CONST_3_ADDR, REG1);
    //REG5   pswSav0[0]
    VoC_shr32_rr(REG45, REG3, DEFAULT);
    //L_sump in ACC0
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_loop_i_short(8,DEFAULT);
    VoC_movreg32(ACC1,REG45,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG67,REG0, DEFAULT);/*reg67=L_av0[i]*/
    VoC_shr32_rr(REG67, REG3, DEFAULT);/*reg67=L_shl (L_av0[i], shift)*/
    // pL_av0[0]!=0 is equal to L_shl(pL_av0[i],(norm(pL_av0[0])-3))
    VoC_bnez32_r(SPECTRAL_COMPARISON1020, ACC1);
    VoC_lw16i(REG7, 0x0fff);
    VoC_movreg16(REG5,REG7,DEFAULT);
SPECTRAL_COMPARISON1020:
    VoC_mac32inc_rp(REG7, REG2, DEFAULT);  // L_sump
    VoC_endloop0


    VoC_lw32z(REG67,DEFAULT);     //REG67:L_dm
    VoC_lw16i_short(REG1,0,IN_PARALLEL );       //REG1:swshift
    /*** Compute the division of the partial sum by sav0[0] ***/

    //ACC1:L_sump
    VoC_movreg32(ACC1,ACC0,DEFAULT);
    //ACC0:L_temp
    VoC_bnltz32_r(SPECTRAL_COMPARISON104, ACC0);
//         VoC_sub32_dr(ACC0,CONST_0_ADDR,ACC0);
    VoC_sub32_rr(ACC0,REG67,ACC0,DEFAULT);
SPECTRAL_COMPARISON104:

    VoC_bez32_r(SPECTRAL_COMPARISON106, ACC0)//else
    VoC_shr16_ri(REG5,-3,DEFAULT);//pswSav0[0] in REG5
    /************************************************************
     *Function: CII_NORM_L_ACC0
     *input: ACC0;
     *return:REG1;
     ***********************************************************/
    VoC_jal(CII_NORM_L_ACC0);

    VoC_sub16_rr(REG1,REG7,REG1,DEFAULT);
//      VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);

    //L_temp is sure to be >0, so ACC0 is L_shl(L_tmep,swShift)

    //swTemp in REG0
    VoC_movreg16(REG0, ACC0_HI, DEFAULT);
    exit_on_warnings=OFF;
    //push swShift (in REG1)
    VoC_push32(REG01, DEFAULT);
    exit_on_warnings=ON;
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
    VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
    VoC_shr32_rr (REG67, REG1, DEFAULT);/* reg67 = L_shr (reg67, reg1);*/

    VoC_lw16_d(REG4,HR_VAD_ALGORITHM_pswRav1_ADDR);
    // REG4 here is surely >0, so no need to condition
    // (confer to pswRav1[i] in func compute_rav1
    VoC_lw16i_short(REG5,0,DEFAULT );

    VoC_shr32_ri (REG45, -11, DEFAULT);/*reg45=L_shl (reg45, 11)*/
    VoC_add32_rr (REG67, REG67, REG45, DEFAULT);/*reg23=L_add(reg23,reg45)*/
    //REG4 for swNormRav1
    VoC_pop16(REG4, DEFAULT);

    VoC_shr32_rr (REG67, REG4, DEFAULT);/* reg23 = L_shr (reg23, reg4);*/
    /*** Compute the difference and save L_dm ***/
    VoC_sub32_rd (REG45, REG67, HR_STATIC_VAD_L_lastdm_ADDR);
    VoC_sw32_d(REG67, HR_STATIC_VAD_L_lastdm_ADDR);
    VoC_bnltz32_r(SPECTRAL_COMPARISON113,REG45)
    VoC_sub32_dr(REG45,CONST_0_ADDR,REG45);//L_temp in REG45
SPECTRAL_COMPARISON113:
    /*** Evaluation of the stat flag ***/
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i(REG2,4456);



    VoC_bngt32_rr(SPECTRAL_COMPARISON115, REG23, REG45);/*if (reg4 < 0L)*/
    VoC_lw16i_short(REG3,1,DEFAULT);
SPECTRAL_COMPARISON115:

    /************************************************
      end inline function: void spectral_comparison()

    ************************************************/
    VoC_lw16i_short(REG5, 2,DEFAULT);
    // load CII_tonc_detection parameters
    VoC_lw16i_set_inc(REG2, LOCAL_AFLAT_pswVadRc_ADDR,1);


    // save parameters of spectral_comparison
    VoC_sw16_d(REG3, HR_VAD_ALGORITHM_swStat_ADDR);


    /**********************************
    Function: void CII_tone_detection_opt(void)

    IN:
        REG2 &pswRc   INCR2=1
    OUT:
        REG3 pswTone
    *********************************/

    VoC_lw16i_set_inc(REG3, HR_VAD_TONE_DETECTION_pswA_ADDR,-2);


    VoC_movreg16(RL6_HI,REG3,DEFAULT);
    /***************************************************
     Function: void CII_step_up(void)

     IN:    REG5     swNp
            REG2     &pswVpar       INCR2     1
                                    INCR3     -2
            RL6_HI   &pswAav1

     OUT:   REG7     pswAav1[swNp]

     USE:   ALL REGS
    ****************************************************/
    VoC_jal(CII_step_up_opt);
    //running to here, the INCR2=1
    //REG7 = pswA[2]
    //L_deposit_h(pswA[2]);
    VoC_lw16i_short(REG6, 0,DEFAULT);           //L_num = L_sub(L_temp, L_den);
    //swTemp = shl(pswA[1], 3);
    VoC_lw16_d(REG0, HR_VAD_TONE_DETECTION_pswA_ADDR+1);
    VoC_shr16_ri(REG0, -3, DEFAULT);
    //L_den in REG45
    VoC_multf32_rr(REG45, REG0, REG0,DEFAULT);


    VoC_shr32_ri(REG67, -3, DEFAULT);
    VoC_sub32_rr(ACC0, REG67, REG45, DEFAULT);      //L_num in ACC0

    VoC_bngtz32_r(TONE_DETECTION_End, ACC0);        //if (L_num <= 0) return;
    VoC_lw16i(REG3, 3189);
    // condition pswA[1]<0 is equal to (pswA[1]<<3)<0
    VoC_bnltz16_r(TONE_DETECTION_101, REG0);        //if (pswA[1] < 0)
    //  VoC_multf32_rd(ACC1,REG5, CONST_3189_ADDR);         //{ swTemp = extract_h(L_den);
    VoC_multf32_rr(ACC1,REG5, REG3,DEFAULT);
    VoC_NOP();
    VoC_bgt32_rr(TONE_DETECTION_End, ACC1,ACC0);            //if (L_temp < 0)  return;
TONE_DETECTION_101:                     //}

    VoC_lw16_d(REG7, CONST_0x7fff_ADDR);             //swPredErr = 0x7fff;   ;swPredErr in REG7
    VoC_lw16i_set_inc(REG0, LOCAL_AFLAT_pswVadRc_ADDR, 1);   //
    VoC_loop_i_short(4, DEFAULT);                // for (i = 0; i < 4; i++)
    VoC_startloop0                       //{
    VoC_multf16inc_pp(REG5, REG0,REG0,DEFAULT);     // swTemp = sub(32767, swTemp);
    VoC_NOP();
    VoC_sub16_dr(REG5, CONST_0x7fff_ADDR, REG5);        //swPredErr = mult(swPredErr, swTemp);
    VoC_multf16_rr(REG7, REG7, REG5, DEFAULT);
    VoC_endloop0                         //}

    VoC_lw16i_short(REG3, 1,DEFAULT);            //*pswTone = 1;
    VoC_blt16_rd(TONE_DETECTION_120, REG7, CONST_1464_ADDR);
TONE_DETECTION_End:
    VoC_lw16i_short(REG3, 0,DEFAULT);
TONE_DETECTION_120:

    /************************************************
      end inline function: void tone_detection(void)

    ************************************************/


    // load parameters for CII_threshold_adaptation
    VoC_lw16_d(REG0, HR_VAD_ALGORITHM_swStat_ADDR);
    VoC_lw16_d(REG1, GLOBAL_EN_swPtch_ADDR);
    VoC_lw32_d(REG45,HR_VAD_ALGORITHM_swM_acf0_ADDR);
    VoC_lw32_d(RL6,HR_VAD_ALGORITHM_swM_pvad_ADDR);

    // could not put use lw32_d because the address of swM_thvad is odd
    // could not change address here because they are used in RESET functions
    VoC_lw16_d(REG6,HR_STATIC_VAD_swM_thvad_ADDR);
    VoC_lw16_d(REG7,HR_STATIC_VAD_swE_thvad_ADDR);

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

    // swE_thvad = 20
    VoC_lw16i_short(RL7_HI,20, DEFAULT);
    // swM_thvad = 17500
    VoC_lw16i(RL7_LO,17500);
    // E_PTH = 18
    VoC_lw16i_short(ACC0_HI,18,DEFAULT);
    // M_PTH = 26250
    VoC_lw16i(ACC0_LO,26250);

    VoC_bgt32_rr(THRESHOLD_ADAPTATION_End,ACC0,REG45);


    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_short(REG4,1,DEFAULT);
    // swStat & swPtch are flags, either 0 or 1
    // so swPtch==1 is equal to swPtch!=0
    // swStat==0 is equal to swStat!=1

    // REG0 for swStat
    // REG1 for swPtch
    VoC_bne32_rd(THRESHOLD_ADAPTATION107,REG01, CONST_1_ADDR);
    // REG3 for swTone
    VoC_be16_rd(THRESHOLD_ADAPTATION107,REG3,CONST_1_ADDR);

    VoC_add16_rd (REG5,REG4,HR_STATIC_VAD_swAdaptCount_ADDR);       //swAdaptCount = add(swAdaptCount, 1);
    VoC_bgt16_rd(THRESHOLD_ADAPTATION108, REG5,CONST_8_ADDR);       // if (swAdaptCount <= 8) return;
THRESHOLD_ADAPTATION107:
    VoC_movreg32(RL7,REG67,DEFAULT);
    VoC_sw16_d(REG5, HR_STATIC_VAD_swAdaptCount_ADDR);
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


    //RL6 for swM_pvad & swE_pvad
    VoC_movreg32(REG01,RL6,DEFAULT);
    VoC_lw16i(REG3,20889);
    VoC_multf32_rr (REG45, REG0, REG3,DEFAULT);
//  VoC_multf32_rd (REG45, REG0, CONST_20889_ADDR);

    VoC_lw16i_short(REG0,1,DEFAULT);
    // L_temp in REG45
    VoC_shr32_ri (REG45,15,DEFAULT);
    //swE_temp in REG1
    VoC_add16_rr(REG1, REG1, REG0, IN_PARALLEL);

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
    VoC_movreg16(REG2,REG6,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_shr16_ri(REG2,4,DEFAULT);
    VoC_movreg16(ACC0_LO,REG6,DEFAULT);
    //L_temp in REG23
    VoC_add32_rr(REG23,ACC0,REG23,DEFAULT);

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
    // M_MARGIN=27343
    VoC_lw16i(REG2,27343);

    // save thvad
    VoC_movreg32(RL7,REG67,DEFAULT);

    // RL6 for swM_pvad & swE_pvad
    // REG4:swM_pvad; REG5:swE_pvad
    VoC_movreg32(REG45, RL6, DEFAULT);


    //  put a small value in ACC1 if swE_pvad==E_Margin
    //  put 0x7FFF in ACC1 if swE_pvad != E_MARGIN
    VoC_lw32z(ACC1,DEFAULT);
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

THRESHOLD_ADAPTATION121:
    //else
    VoC_movreg16(REG5,REG6,DEFAULT);
THRESHOLD_ADAPTATION124:

    VoC_lw16_d(REG1,VAD_ALGORITHM_swNormRav1_ADDR);
    VoC_lw16i_set_inc(REG0,HR_STATIC_VAD_pswRvad_ADDR, 1);
    VoC_sw16_d(REG1, HR_STATIC_VAD_swNormRvad_ADDR);
    VoC_lw16i_set_inc(REG2,HR_VAD_ALGORITHM_pswRav1_ADDR, 1);

    VoC_bngt32_rr(THRESHOLD_ADAPTATION127,RL7,REG45);
    VoC_movreg32(RL7,REG45,DEFAULT);
THRESHOLD_ADAPTATION127:


    VoC_loop_i_short(9, DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p (REG3,REG2, DEFAULT);
    VoC_lw16i_short(REG5,9,DEFAULT);
    VoC_sw16inc_p (REG3,REG0, DEFAULT);
    VoC_endloop0

    VoC_sw16_d(REG5, HR_STATIC_VAD_swAdaptCount_ADDR);
THRESHOLD_ADAPTATION_End:


    /************************************************
      end of inline function: void CII_threshold_adaptation(void)

    ************************************************/

    // save parameters for CII_threshold_adaptation_opt

    // could not put use sw32_d because the address of swM_thvad is odd
    // could not change address here because they are used in RESET functions
    VoC_lw16i_short(REG0, 0,DEFAULT);
    VoC_sw16_d(RL7_LO,HR_STATIC_VAD_swM_thvad_ADDR);
    VoC_sw16_d(RL7_HI,HR_STATIC_VAD_swE_thvad_ADDR);


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


    VoC_bngt32_rr(DECISION_PENG_101, RL6, RL7);
    VoC_lw16i_short(REG0, 1,DEFAULT);
DECISION_PENG_101:

    /************************************************
      end inline function: void CII_vad_decision(void)

    ************************************************/


    /***********************************************************************
    inline Function: void CII_vad_hangover(void)
    IN:    REG0   swVvad
    OUT:   pswVadFlag   GLOBAL_EN_SPEECHENCODER_swVadFlag_ADDR

    ***************************************************************************/

    VoC_lw32_d(REG67, HR_STATIC_VAD_swBurstCount_ADDR);
    //REG6:burstcount;REG7:hangcount
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);
    VoC_be16_rd(VAD_HANDOVER101,REG0,CONST_1_ADDR);
    VoC_lw16i_short(REG6,0,DEFAULT);
VAD_HANDOVER101:
    VoC_pop16(RA, DEFAULT);
    VoC_blt16_rd(VAD_HANDOVER103,REG6,CONST_3_ADDR)
    VoC_lw16i_short(REG7,5,DEFAULT);
    VoC_lw16i_short(REG6,3,IN_PARALLEL);
VAD_HANDOVER103:

    VoC_bltz16_r(VAD_HANDOVER104,REG7);
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_sub16_rr (REG7,REG7,REG0, DEFAULT);
VAD_HANDOVER104:
    VoC_sw16_d(REG0, GLOBAL_OUTPUT_Vad_ADDR);

    VoC_sw32_d(REG67,HR_STATIC_VAD_swBurstCount_ADDR);

    /***********************************************************************
    end inline Function: void CII_vad_hangover(void)

    ***************************************************************************/

    VoC_return;
}






/*************************************************************************

  Function:  void   CII_aFlatRcDp( void)
  Input : RL6_LO      Longword  &pL_R
          RL6_HI      Shortword &pswRc

  Output:  NONE
  Return:s

*************************************************************************/

void CII_aFlatRcDp_opt(void)
{

#if 0

    voc_short   AFLATRCDP_pL_pjNewSpace_ADDR                ,20,y       //[20]shorts
    voc_short   AFLATRCDP_pL_pjOldSpace_ADDR                ,20,y        //[20]shorts
    voc_short   AFLATRCDP_pL_vjNewSpace_ADDR                ,38,y        //[38]shorts
    voc_short   AFLATRCDP_pL_vjOldSpace_ADDR                ,38,y        //[38]shorts

#endif



    VoC_push16(RA,DEFAULT);

    //  pL_pjOld = pL_pjOldSpace;
    //  pL_pjNew = pL_pjNewSpace;
    //  pL_vjOld = pL_vjOldSpace + NP - 1;
    //  pL_vjNew = pL_vjNewSpace + NP - 1;

    VoC_lw16i(RL7_LO,AFLATRCDP_pL_pjOldSpace_ADDR);     // pL_pjOld
    VoC_lw16i(RL7_HI,AFLATRCDP_pL_vjOldSpace_ADDR+18);  // pL_vjOld
    VoC_lw16i(ACC1_LO,AFLATRCDP_pL_pjNewSpace_ADDR);    // pL_pjNew
    VoC_lw16i(ACC1_HI,AFLATRCDP_pL_vjNewSpace_ADDR+18); // pL_vjNew


    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_movreg32(REG01,RL6,IN_PARALLEL);

    VoC_lw16i_short(REG6,0,DEFAULT);        // REG6 for loop number
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);   // REG23 for pL_R[0] swTemp1 = round(pL_R[0]);
    VoC_lw32inc_p(REG45,REG0,DEFAULT);      // REG45 for pL_R[1] swTemp2 = round(pL_R[1]);

    VoC_loop_i(1,10)

    VoC_bez16_r(CII_aFlatRcDp201,REG6);
    /* Swap pjNew and pjOld buffers
        pL_swap = pL_pjNew;
        pL_pjNew = pL_pjOld;
        pL_pjOld = pL_swap;

        pL_swap = pL_vjNew;
        pL_vjNew = pL_vjOld;
        pL_vjOld = pL_swap;
    */

    VoC_movreg32(REG23,ACC1,DEFAULT);
    VoC_movreg32(ACC1,RL7,IN_PARALLEL);
    VoC_movreg32(RL7,REG23,DEFAULT);

    // swTemp = norm_l(pL_pjOld[0]);
    VoC_lw32_p(ACC0,REG2,DEFAULT);

    /**************************/
    // input in ACC0
    // output in REG1  only used ACC0, REG1
    /**************************/

    VoC_jal(CII_NORM_L_ACC0);
    VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
    //    swTemp1 = round(L_shl(pL_vjOld[0], swTemp));        /* normalize num.  */
    //    swTemp2 = round(L_shl(pL_pjOld[0], swTemp));        /* normalize den.  */

    VoC_lw32_p(REG45,REG3,DEFAULT);
    VoC_shr32_rr(REG45,REG1,DEFAULT);

CII_aFlatRcDp201:


    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_add32_rd(REG45,REG45,CONST_0x00008000L_ADDR);
    VoC_add32_rd(REG23,ACC0,CONST_0x00008000L_ADDR);

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_movreg16(REG4,REG5,IN_PARALLEL);

    // swAbsTemp1 = abs_s(swTemp1);
    VoC_bgtz16_r(CII_aFlatRcDp100,REG5)
    VoC_sub16_rr(REG5,REG7,REG5,DEFAULT);
CII_aFlatRcDp100:

    /*  if (swTemp2 <= 0 || sub(swAbsTemp1, swTemp2) >= 0)
        {
            j = 0;
            for (i = j; i < NP; i++)
        {
            pswRc[i] = 0;
        }
        return;
    */

    // the first condition is
    VoC_bgt16_rr(CII_aFlatRcDp101,REG3,REG5)

    VoC_loop_i_short(10,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0

    VoC_jump(aFlatRcDp_end);

CII_aFlatRcDp101:

    /*********************/
    // input in REG0, REG1
    // output in  REG2
    // used register RL6, RL7
    /*********************/

    VoC_movreg16(REG1,REG3,DEFAULT);
    VoC_push32(RL7,IN_PARALLEL);

    VoC_push32(RL6,DEFAULT);
    VoC_movreg16(REG0,REG5,IN_PARALLEL);

    VoC_jal(CII_DIV_S); // REG2 for swRc

    // RL6 for input address
    VoC_pop32(RL6,DEFAULT);
    // REG4 for swTemp1

    VoC_movreg32(REG01,RL6,DEFAULT);
    VoC_add16_rr(REG1,REG6,REG1,DEFAULT);
    VoC_bne16_rr(CII_aFlatRcDp102,REG4,REG5)
    VoC_sub16_rr(REG2, REG7, REG2, DEFAULT);
CII_aFlatRcDp102:

    VoC_push16(REG6,DEFAULT);
    VoC_sw16_p(REG2,REG1,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);


    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_lw16i_short(INC3,-2,IN_PARALLEL);
    VoC_movreg16(REG4,REG2,DEFAULT);
    VoC_bnez16_r(CII_aFlatRcDp202,REG6)

    /* copy into the output Rc array
        pswRc[0] = swRc;
        for (i = 0; i <= NP; i++)
        {
            pL_R[i] = L_shr(pL_R[i], AFSHIFT);
        }
    */

    VoC_loop_i_short(11,DEFAULT)
    VoC_startloop0
    VoC_lw32_p(REG67,REG0,DEFAULT);
    VoC_shr32_ri(REG67,2,DEFAULT);
    // REG2: pL_pjOld[i] addre
    // REG3: pL_vjOld[i] address
    // put the following instr in the loop to replace NOP
    VoC_lw32_sd(REG23,0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_endloop0

    /* Initialize the pjOld and vjOld recursion arrays */
    /*
      for (i = 0; i < NP; i++)
      {
        pL_pjOld[i] = pL_R[i];
        pL_vjOld[i] = pL_R[i + 1];
      }
      for (i = -1; i > -NP; i--)
        pL_vjOld[i] = pL_R[-(i + 1)];
    */

    // REG1: pL_vjOld[i] address
    VoC_movreg16(REG1,REG3,DEFAULT);
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);                          // pL_R[i] address

    VoC_loop_i_short(10,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    // in first time, save any value in pL_vjOld[0], which will be rewritten in the next instruction
    // else, it will save pL_R[-(i+1)] to pL_vjOld[i]
    exit_on_warnings=OFF;
    VoC_sw32inc_p(REG67,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_lw32_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);

    VoC_endloop0

CII_aFlatRcDp202:

    /* Compute the square of the j=0 reflection coefficient */
    /*------------------------------------------------------*/
    //  swRcSq = mult_r(swRc, swRc);

    // REG4 for swRc
    VoC_multf32_rr(REG67,REG4,REG4,DEFAULT);


    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_movreg32(REG23,ACC1,IN_PARALLEL);   // REG2 for pL_pjNew REG3 for pL_vjOld -2

    VoC_add32_rd(REG67,REG67,CONST_0x00008000L_ADDR);   // REG7 for swRcSq
    // VoC_pop16(REG6,DEFAULT);

    // REG6 for j
    VoC_lw16_sd(REG6,0,DEFAULT);
    VoC_lw32_sd(REG01,0,IN_PARALLEL);   // REG0 for pL_pjOld REG1 for pL_vjOld

    VoC_movreg16(REG3,REG1,DEFAULT);
    VoC_push16(REG4,IN_PARALLEL);

    //  for (i = 0; i <= NP -j - 2; i++)
    //  {
    //    L_temp = L_mpy_ls(pL_vjOld[i], swRc);
    //    L_sum = L_add(L_temp, pL_pjOld[i]);
    //    L_temp = L_mpy_ls(pL_pjOld[i], swRcSq);
    //    L_sum = L_add(L_temp, L_sum);
    //    L_temp = L_mpy_ls(pL_vjOld[-i], swRc);
    //    pL_pjNew[i] = L_add(L_sum, L_temp);
    //  }

    VoC_sub16_dr(REG5,CONST_9_ADDR,REG6);

    VoC_bngtz16_r(CII_aFlatRcDp301,REG5)
    VoC_loop_r_short(REG5,DEFAULT)
    VoC_lw32inc_p(ACC0,REG1,IN_PARALLEL);
    VoC_startloop0

    /****************************/
    // input ACC0, REG4
    // output ACC0
    // used register REG5
    // unchanged : REG4
    /****************************/

    // L_temp = L_mpy_ls(pL_vjOld[i], swRc);
    VoC_jal(CII_L_mpy_ls_opt);

    // L_sum = L_add(L_temp, pL_pjOld[i]);
    VoC_add32_rp(RL7,ACC0,REG0,DEFAULT);

    // L_temp = L_mpy_ls(pL_pjOld[i], swRcSq);
    VoC_movreg16(REG4,REG7,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);
    VoC_jal(CII_L_mpy_ls_opt);

    // L_sum = L_add(L_temp, L_sum);
    VoC_add32_rr(RL7,RL7,ACC0,DEFAULT);

    //   L_temp = L_mpy_ls(pL_vjOld[-i], swRc);
    VoC_lw32inc_p(ACC0,REG3,DEFAULT);
    VoC_lw16_sd(REG4,0,IN_PARALLEL);
    VoC_jal(CII_L_mpy_ls_opt);

    // pL_pjNew[i] = L_add(L_sum, L_temp);
    VoC_add32_rr(RL7,RL7,ACC0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sw32inc_p(RL7,REG2,DEFAULT);

    VoC_endloop0

CII_aFlatRcDp301:


    // REG0 for pL_pjOld
    // REG1 for pL_vjOld
    VoC_lw32_sd(REG01,0,DEFAULT);
    /// VoC_push16(REG6,DEFAULT);

    // REG6: j*2
    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_lw16i_short(INC0,-2,IN_PARALLEL);
    // REG5: 14-j*2
    VoC_sub16_dr(REG5,CONST_14_ADDR,REG6);

    // REG6 for pL_pjOld
    // REG2 for pL_vjNew
    VoC_movreg16(REG6,REG0,DEFAULT);
    VoC_movreg16(REG2,ACC1_HI,IN_PARALLEL);

    // REG3 for pL_vjOld
    VoC_add16_rr(REG3,REG1,REG5,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG5,IN_PARALLEL);

    // REG0 for pL_pjOld[7-j]
    // REG0 initial value of pL_pjOld[-(i+1)](i=-NP+2+j)
    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_sub16_rr(REG2,REG2,REG5,IN_PARALLEL);
    VoC_sub16_rd(REG2,REG2,CONST_2_ADDR);

    // REG5 for 17-j*2 (loop counter) = 14-j*2 + 3
    VoC_add16_rd(REG5,REG5,CONST_3_ADDR);

    //  for (i = -NP + j + 2; i <= NP - j - 2; i++)
    // {
    //   L_temp = L_mpy_ls(pL_vjOld[-i - 1], swRcSq);
    //   L_sum = L_add(L_temp, pL_vjOld[i + 1]);
    //   L_temp = L_mpy_ls(pL_pjOld[(((i + 1) >= 0) ? i + 1 : -(i + 1))], swRc);
    //   L_temp = L_shl(L_temp, 1);
    //   pL_vjNew[i] = L_add(L_temp, L_sum);
    //  }

    VoC_bngtz16_r(CII_aFlatRcDp302,REG5)
    //  if 2*j==16
    VoC_bne16_rd(CII_aFlatRcDp105,REG5,CONST_1_ADDR)
    VoC_add16_rd(REG0,REG6,CONST_2_ADDR);

CII_aFlatRcDp105:

    // VoC_push16(REG4,DEFAULT);
    VoC_loop_r_short(REG5,DEFAULT)
    VoC_movreg16(REG4,REG7,IN_PARALLEL);
    VoC_startloop0

    /****************************/
    // input ACC0, REG4
    // output ACC0
    // used register REG5
    // unchanged : REG4
    /****************************/

    VoC_lw32inc_p(ACC0,REG3,DEFAULT);
    VoC_bne16_rr(CII_aFlatRcDp103,REG0,REG6)

    VoC_lw16i_short(INC0,2,DEFAULT);
CII_aFlatRcDp103:

    VoC_jal(CII_L_mpy_ls_opt);

    VoC_add32inc_rp(RL7,ACC0,REG1,DEFAULT);
    VoC_lw16_sd(REG4,0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_jal(CII_L_mpy_ls_opt);

    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_add32_rr(RL7,RL7,ACC0,DEFAULT);
    VoC_movreg16(REG4,REG7,DEFAULT);

    exit_on_warnings=OFF;
    VoC_sw32inc_p(RL7, REG2, DEFAULT);
    exit_on_warnings=ON;

    VoC_endloop0


CII_aFlatRcDp302:

    VoC_pop16(REG4,DEFAULT);
    VoC_pop32(RL7,DEFAULT);
    VoC_pop16(REG6,IN_PARALLEL);

    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);

    VoC_endloop1

    /* Compute reflection coefficients Rc[1],...,Rc[9] */
    /*-------------------------------------------------*/

aFlatRcDp_end:

    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_return;

}


/*************************************************/
// CII_rcToADp_opt_new
// IN:
//    REG1:   pswA          INCR1=-2
//    REG0:   &pswRc        INCR0=1
//    0x800   swAscale(is a const)
//OUT:
//    REG7
//USE:
//    ALL REGs
/************************************************/

void CII_rcToADp_opt_new(void)
{
#if 0

//  voc_short LOCAL_rcToCorrDpL_pL_ASpace_ADDR      ,20,x   // long[10]


#endif












    // siLimit in reg7
    VoC_lw16i_short(REG7,0,DEFAULT);

    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    // RL6_HI is also used as tmpSpace
    VoC_lw16i(RL6_LO,LOCAL_rcToCorrDpL_pL_ASpace_ADDR);

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_movreg16(RL6_HI,REG1,IN_PARALLEL);

    VoC_loop_i(1,10)

    // REG2 is initialized to pL_A[0]
    // REG3 is initialized to pL_tmp[0]
    VoC_movreg32(REG23,RL6,DEFAULT);

    VoC_bngtz16_r(rcToADp100,REG6)

    VoC_loop_r_short(REG6,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);
    VoC_startloop0

    /****************************/
    // input ACC0, REG4
    // output ACC0
    // used register REG5
    /****************************/

    VoC_lw32inc_p(ACC0,REG1,DEFAULT);
    VoC_jal(CII_L_mpy_ls_opt);

    // REG2 PL_A[j]
    VoC_add32inc_rp(ACC0,ACC0,REG2,DEFAULT);

    VoC_be32_rd(rcToADp_sat,ACC0,CONST_0x7fffffff_ADDR)
    VoC_bne32_rd(rcToADp_nosat,ACC0,CONST_0x80000000_ADDR)
rcToADp_sat:
    VoC_lw16i_short(REG7,1,DEFAULT);
rcToADp_nosat:
    // REG3 PL_tmp[j]
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_endloop0

    VoC_pop16(RA,DEFAULT);

rcToADp100:


    // REG0 &pswRc[i]
    VoC_multf32inc_pd(ACC1,REG0,CONST_0x0800_ADDR);

    // swap PL_tmp & PL_A
    VoC_movreg16(RL6_LO,RL6_HI,DEFAULT);
    VoC_movreg16(RL6_HI,RL6_LO,IN_PARALLEL);

    VoC_lw16_p(REG4,REG0,DEFAULT);
    // pL_tmp[i]
    // which in the next loop is PL_A[i-1]
    VoC_sw32_p(ACC1,REG3,DEFAULT);
    // REG1 is initialized to pL_A[i-1]
    // REG1 will not be used in the first time in loop, so it could be initilize here
    VoC_movreg16(REG1,REG3,IN_PARALLEL);



    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);

    VoC_endloop1

    // compared to C, the swap is done once more here
    // in RL6_HI is &pswA (also used as pL_tmp)
    // in RL6_LO is pL_A (correspond to pL_tmp in C)
    VoC_movreg32(REG23,RL6,DEFAULT);

    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_loop_i_short(10,DEFAULT)
    VoC_startloop0
    VoC_add32inc_pd(REG45,REG2,CONST_0x00008000L_ADDR);

    VoC_be16_rd(isSwLimit2,REG5,CONST_0x8000_ADDR);
    VoC_bne16_rd(isSwLimit1,REG5,CONST_0x7fff_ADDR);
isSwLimit2:
    VoC_lw16i_short(REG7,1,DEFAULT);
isSwLimit1:

    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_endloop0

    VoC_return;
}



void CII_lpcFir_opt(void)
{
    // INPUT parameters :
    // REG3 : pswCoef , INC3 = 1
    // REG2 : pswState, INC2 = 1
    // REG1 : swFiltOut,INC1 = 1
    // REG0 : pswInput, INC0 = -1
    // other vars:
    // REG4 : maxsiStage1, maxsiStage2, extract_h(L_Sum)
    // REG5 : siSmp

    VoC_lw16i_short(REG5, 0, DEFAULT);
    VoC_lw16i_short(REG4,10,DEFAULT);

    VoC_lw32_d(ACC0,CONST_0x00000800L_ADDR);
    VoC_lw16i(REG7,0x8000);

    exit_on_warnings=OFF;
    VoC_push16(REG7,DEFAULT);
    exit_on_warnings=ON;
    VoC_push32(ACC0,DEFAULT);



    VoC_loop_i(1,10)

    VoC_bez16_r(lpcFir_skip_1st,REG5);

    VoC_loop_r_short(REG5,DEFAULT)
    VoC_lw32_sd(ACC0,0,IN_PARALLEL);
    VoC_startloop0
    VoC_mac32inc_pp(REG0,REG3,DEFAULT);
    VoC_endloop0;

    // if NP > siSmp, REG4 = siSmp, newREG4 = NP - siSmp
    // else REG4 = NP, newREG4 = NP - NP = 0 ! > 0
    // so second loop will be skipped
    VoC_sub16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG5,IN_PARALLEL);
    VoC_add16_rd(REG0,REG0,CONST_1_ADDR);

lpcFir_skip_1st:

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0
    VoC_mac32inc_pp(REG2,REG3,DEFAULT);
    VoC_endloop0;

    // restitute &pswState[0]
    VoC_sub16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_lw16i_short(REG4,10,IN_PARALLEL);

    VoC_msu32_rp(REG7,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_shr32_ri(ACC0,-4,IN_PARALLEL);
    exit_on_warnings=ON;

    // siSmp++
    VoC_add16_rd(REG5,REG5,CONST_1_ADDR);

    // restitute &pswCoef[0]
    VoC_sub16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);

    VoC_endloop1;



    VoC_push16(REG2,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_add16_rr(REG2,REG0,REG4,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG4,IN_PARALLEL);

    VoC_lw16i_short(INC0,-2,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_loop_i(1,30)

    VoC_push16(REG3,DEFAULT);

    // REG3 is always even, REG0 points to RAM_X

    VoC_aligninc_pp(REG0,REG0,DEFAULT);
    VoC_lw32_sd(ACC0,0,IN_PARALLEL);

    VoC_loop_i_short(5, DEFAULT)
    exit_on_warnings=OFF;
    VoC_sw16inc_p(ACC0_HI,REG1,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG3);
    VoC_endloop0;

    VoC_pop16(REG3,DEFAULT);
    VoC_movreg16(REG0,REG2,IN_PARALLEL);

    VoC_msu32inc_rp(REG7,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_shr32_ri(ACC0,-4,IN_PARALLEL);
    exit_on_warnings=ON;

    VoC_endloop1;

    VoC_lw16i_short(INC0,-1,DEFAULT);

    VoC_pop16(REG2,DEFAULT);

    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);

    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    VoC_loop_i_short(10,DEFAULT);

    VoC_startloop0;
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;

    VoC_pop16(REG7,DEFAULT);
    VoC_pop32(ACC0,IN_PARALLEL);

    VoC_return;
}



/*************************************************************
IN:
reg0:&pswReflecCoefIn[0] INC0=1;
reg5:swRq
reg23:
OUT:
reg2:psnsSqrtRsOut->man
reg3:psnsSqrtRsOut->sh
USE:
all regs  void   CII_res_eng()
*************************************************************/


void CII_res_eng_opt(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_lw32_d(RL6,CONST_0x00008000L_ADDR);
    VoC_movreg16(REG3,RL6_LO,DEFAULT);
    VoC_lw16i_short(REG4,-6,IN_PARALLEL);//swPartialProductShift in reg4

    VoC_loop_i(1,10);

    VoC_multf32inc_pp(ACC0,REG0,REG0,DEFAULT);
    VoC_NOP();
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_add32_rd(REG67,ACC0,CONST_0x80000000_ADDR);

    VoC_multf32_rr(ACC1,REG3,REG7,DEFAULT);//L_Product in ACC1
    VoC_lw16i_short(ACC0_LO,0,DEFAULT);
    VoC_movreg16(ACC0_HI,ACC1_HI,DEFAULT);


    VoC_jal(CII_NORM_L_ACC0);                  //swShift in REG1
    VoC_add16_rr(REG4,REG4,REG1,DEFAULT);  //reg4 swPartialProductShift
    VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
    VoC_shr32_rr(ACC1,REG1,DEFAULT);

    VoC_add32_rr(REG23,ACC1,RL6,DEFAULT);


    VoC_endloop1;

    VoC_lw16i_short(REG2,0,DEFAULT);              //L_shift in REG23
    VoC_bgtz16_r(CII_abs_s_new,REG3);
    VoC_sub16_rr(REG3,REG2,REG3,DEFAULT);
CII_abs_s_new:

    VoC_push16(REG5,DEFAULT);
    VoC_movreg32(RL7,REG23,DEFAULT);

    VoC_jal(CII_sqroot_opt);     //swSqrtPP in REG7
    VoC_pop16(REG5,DEFAULT);
    // result in reg3

    VoC_movreg16(REG3,REG4,DEFAULT);
    VoC_shr32_ri(REG23,1,DEFAULT);
    VoC_bez16_r(res_eng00,REG2);
    VoC_multf32_rd(REG67,REG7,CONST_0x5a82_ADDR);
    VoC_NOP();
    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);
res_eng00:

    VoC_multf32_rr(ACC0,REG7,REG5,DEFAULT);
    VoC_jal(CII_NORM_L_ACC0);
    // result in reg3
    VoC_add32_rr(ACC1,ACC0,RL6,DEFAULT);
    VoC_movreg16(REG2,ACC1_HI,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG1,DEFAULT);
    VoC_return;
}




// ***********************************************
// CII_aToRc_opt2
// input :  REG0 -> pswAin (param1),     INC0 = 1
//          REG1 -> &pswRc[9] (param2),  INC1 = -1
// note   : param0 is always 4
// output : REG7 <- return
// ***********************************************


void CII_aToRc_opt2(void)

{
#if 0
    voc_short aToRc_pswTmp_ADDR                     ,10,y                 // [10]
    voc_short aToRc_pswASpace_ADDR                  ,10,x                // [10]
#endif


    // push RA, i+1 = 10
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG4,10,DEFAULT);

    VoC_lw16i_set_inc(REG2,aToRc_pswASpace_ADDR,1);
    VoC_lw16i_set_inc(REG3,aToRc_pswTmp_ADDR-1,-1);

    // siUnstableFlt
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);

    VoC_loop_i_short(10,DEFAULT);
    VoC_lw16inc_p(REG5,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;


    // REG0 :               INC0 =  1
    // REG1 : pswRc,        INC1 = -1
    // REG2 : &pswA[10],    INC2 =  1
    // REG3 : &pswTmp[-1],  INC3 = -1
    // REG4 : i+1
    // REG5 :
    // REG6 : swAshift
    // REG7 : siUnstableFlt


    // for (i = NP - 1; i >= 1; i--)
    VoC_loop_i(1,9);


    // STACK16           STACK32
    // ---------------------------------
    // &pswA[0]

    // REG0 :              INC0 =  1
    // REG1 : pswRc,       INC1 = -1
    // REG2 : &pswA[i+1],  INC2 =  1
    // REG3 : &pswTmp[-1], INC3 = -1
    // REG4 : i+1
    // REG5 :
    // REG6 :
    // REG7 : siUnstableFlt

    VoC_lw16i_short(REG5,1,DEFAULT);

    // swap positions & increment
    VoC_sub16_rr(REG3,REG2,REG5,DEFAULT);
    VoC_add16_rr(REG2,REG3,REG5,IN_PARALLEL);

    // (i+1)--
    VoC_sub16_rr(REG4,REG4,REG5,DEFAULT);


    // REG0 :             INC0 =  1
    // REG1 : pswRc,      INC1 = -1
    // REG2 : &pswTmp[0], INC2 =  1
    // REG3 : &pswA[i],   INC3 = -1
    // REG4 : i
    // REG5 : 1
    // REG6 :
    // REG7 : siUnstableFlt

    // STACK16           STACK32
    // ---------------------------------
    // &pswA[0]


    // shl(pswA[i], 4)
    VoC_lw16inc_p(REG5,REG3,DEFAULT);
    VoC_shr16_ri(REG5,-4,DEFAULT);

    // CII_isSwLimit inline...

    // if not equal limits, do nothing
    VoC_be16_rd(isSwLimit_opt1a,REG5,CONST_0x8000_ADDR);
    VoC_bne16_rd(isSwLimit_opt1b,REG5,CONST_0x7fff_ADDR);
isSwLimit_opt1a:
    VoC_lw16i_short(REG7,1,DEFAULT);
isSwLimit_opt1b:


    // REG0 :             INC0 =  1
    // REG1 : pswRc,      INC1 = -1
    // REG2 : pswTmp,     INC2 =  1
    // REG3 : &pswA[i-1], INC3 = -1
    // REG4 : pswRc[i]
    // REG5 : i
    // REG6 : swAshift
    // REG7 : siUnstableFlt


    // store pswRc[i]
    VoC_sw16inc_p(REG5,REG1,DEFAULT);

    // free some regs
    VoC_push16(REG1,DEFAULT);
    VoC_push32(REG23,IN_PARALLEL);
    VoC_push16(REG4,DEFAULT);


    // STACK16           STACK32
    // ---------------------------------
    // &pswA[0]          &pswTmp[0], &pswA[i-1]
    // &pswRc[i-1]
    // i

    // L_temp = L_msu(LW_MAX, pswRc[i], pswRc[i]);
    VoC_lw32_d(ACC0,CONST_0x7fffffff_ADDR);

    // if (pswRc[i] == SW_MIN)
    VoC_bne16_rd(aToRc_opt0,REG0,CONST_0x8000_ADDR);

    // siUnstableFlt = 1
    VoC_lw16i_short(REG7,1,DEFAULT);
    // swRcOverE = 0
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    // swDiv = 0
    VoC_lw16i_short(REG2,1,DEFAULT);
    // -swActShift = -2
    VoC_lw16i_short(REG6,-2,IN_PARALLEL);

    VoC_jump(aToRc_opt1);

aToRc_opt0:

    // else


    VoC_msu32_rr(REG5,REG5,DEFAULT);

    // *****************************
    // CII_NORM_L_ACC0
    // input : ACC0
    // output : REG1
    // note : ACC0 <- (ACC0 >> REG1)
    // *****************************

    // swNormShift = norm_l(L_temp);
    // L_temp = L_shl(L_temp, swNormShift);
    VoC_jal(CII_NORM_L_ACC0);
    VoC_lw16i_short(INC1,-1,DEFAULT);
    VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);


    //    VoC_sub32_dr(ACC0,CONST_0_ADDR,ACC0);
    // reg1 -swNormShift
    // -swActShift = add(-2, -swNormShift);
    VoC_add16_rd(REG6,REG1,CONST_neg2_ADDR);


    // ************************************
    // CII_DIV_S
    // input :  REG0 -> Nominator (param0)
    //          REG1 -> Denominator (param1)
    // output : REG2 -> return
    // used regs : REG012, RL67
    // ************************************

    VoC_lw16i(REG0,0x2000);
    VoC_movreg16(REG1,ACC0_HI,DEFAULT);
    VoC_jal(CII_DIV_S);
    // swDiv in REG2

    // swRcOverE = mult_r(pswRc[i], swDiv);
    VoC_multf32_rr(REG45,REG2,REG5,DEFAULT);
    VoC_NOP();
    VoC_add32_rd(REG45,REG45,CONST_0x00008000L_ADDR);
aToRc_opt1:

    // CII_isSwLimit inline...

    // if not equal limits, do nothing
    VoC_be16_rd(isSwLimit_opt2a,REG5,CONST_0x8000_ADDR);
    VoC_bne16_rd(isSwLimit_opt2b,REG5,CONST_0x7fff_ADDR);
isSwLimit_opt2a:
    VoC_lw16i_short(REG7,1,DEFAULT);
isSwLimit_opt2b:

    // i in REG4
    VoC_pop16(REG4,DEFAULT);
    // swDiv in REG1
    VoC_movreg16(REG1,REG2,DEFAULT);
    // &pswTmp[0], &pswA[i-1]
    VoC_pop32(REG23,IN_PARALLEL);
    // &pswA[0]
    VoC_lw16_sd(REG0,1,DEFAULT);


    // REG0 : &pswA[0]     INC0 =  1
    // REG1 : swDiv        INC1 = -1
    // REG2 : &pswTmp[0]   INC2 =  1
    // REG3 : &pswA[i-1]   INC3 = -1
    // REG4 : i
    // REG5 : swRcOverE
    // REG6 : -swActShift
    // REG7 : siUnstableFlt


    // STACK16           STACK32
    // ---------------------------------
    // &pswTmp[0]
    // &pswRc[i-1]

    // swap pswTmp & pswA
    VoC_sw16_sd(REG2,1,DEFAULT);
    VoC_loop_r_short(REG4,DEFAULT);

    VoC_startloop0

    VoC_lw32z(ACC0,DEFAULT);
    // L_temp = L_mult(pswA[j], swDiv);
    VoC_mac32inc_rp(REG1,REG0,DEFAULT);


    // L_temp = L_msu(L_temp, pswA[i - j - 1], swRcOverE);
    VoC_msu32inc_rp(REG5,REG3,DEFAULT);
    // &pswA[j+1] & swDiv
    VoC_push32(REG01,DEFAULT);

    // round(L_temp = L_shl(L_temp, swActShift));
    VoC_shr32_rr(ACC0,REG6,DEFAULT);
    VoC_add32_rd(REG01,ACC0,CONST_0x00008000L_ADDR);

    // CII_isSwLimit inline...
    // if not equal limits, do nothing
    VoC_be16_rd(isSwLimit_opt3a,REG1,CONST_0x8000_ADDR);
    VoC_bne16_rd(isSwLimit_opt3b,REG1,CONST_0x7fff_ADDR);
isSwLimit_opt3a:
    VoC_lw16i_short(REG7,1,DEFAULT);
isSwLimit_opt3b:
    // &pswA[j+1] & swDiv
    VoC_pop32(REG01,DEFAULT);
    // store pswTmp[j]
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG1,REG2,DEFAULT);
    exit_on_warnings=ON;

    VoC_endloop0;



    // REG0 : &pswA[i]     INC0 =  1
    // REG1 : swDiv        INC1 = -1
    // REG2 : &pswTmp[i]   INC2 =  1
    // REG3 : &pswA[-1]    INC3 = -1
    // REG4 : i
    // REG5 : swRcOverE
    // REG6 : -swActShift
    // REG7 : siUnstableFlt


    // STACK16           STACK32
    // ---------------------------------
    // &pswTmp[0]
    // &pswRc[i-1]


    // pop
    VoC_pop16(REG1,DEFAULT);


    // REG0 :                  INC0 =  1
    // REG1 : &pswRc[new_i]    INC1 = -1
    // REG2 : &pswTmp[new_i+1] INC2 =  1
    // REG3 : &pswA[-1]        INC3 = -1
    // REG4 : new_i+1
    // REG5 :
    // REG6 :
    // REG7 : siUnstableFlt

    // STACK16           STACK32
    // ---------------------------------
    // &pswTmp[0]



    // -----------------
    //       SWAP
    // -----------------


    // REG0 :                  INC0 =  1
    // REG1 : &pswRc[new_i]    INC1 = -1
    // REG2 : &pswA[new_i+1]   INC2 =  1
    // REG3 : &pswTmp[-1]      INC3 = -1
    // REG4 : new_i+1
    // REG5 :
    // REG6 :
    // REG7 : siUnstableFlt

    // STACK16           STACK32
    // ---------------------------------
    // &pswA[0]


    VoC_endloop1;



    // REG0 :                  INC0 =  1
    // REG1 : &pswRc[0]        INC1 = -1
    // REG2 : &pswA[1]         INC2 =  1
    // REG3 : &pswTmp[-1]      INC3 = -1
    // REG4 : 1
    // REG5 :
    // REG6 :
    // REG7 : siUnstableFlt


    // STACK16           STACK32
    // ---------------------------------
    // &pswA[0]


    VoC_pop16(REG2,DEFAULT);

    // REG0 :                  INC0 =  1
    // REG1 : &pswRc[0]        INC1 = -1
    // REG2 : &pswA[0]         INC2 =  1
    // REG3 : &pswTmp[-1]      INC3 = -1
    // REG4 : 1
    // REG5 :
    // REG6 : swAshift
    // REG7 : siUnstableFlt

    VoC_pop16(RA,DEFAULT);

    // pswRc[0] = shl(pswA[0], swAshift);
    VoC_lw16_p(REG4,REG2,DEFAULT);
    VoC_shr16_ri(REG4,-4,DEFAULT);


    // CII_isSwLimit inline...
    // if not equal limits, do nothing
    VoC_be16_rd(isSwLimit_opt4a,REG1,CONST_0x8000_ADDR);
    VoC_bne16_rd(isSwLimit_opt4b,REG1,CONST_0x7fff_ADDR);
isSwLimit_opt4a:
    VoC_lw16i_short(REG7,1,DEFAULT);
isSwLimit_opt4b:

    VoC_sw16_p(REG4,REG1,DEFAULT);
    // return in REG7
    VoC_return;

}














/*********************/
// input :
//
// REG2     Shortword swRxDTXState,
// REG3     Shortword swDecoMode
// REG4     Shortword swFrameType
//
// output   NONE
/*********************/

void CII_HR_Decode(void)
{

    VoC_push16(RA,DEFAULT);

    // if force_reset
    VoC_bez16_r(Decoder_101,REG2);

    // reset decoder
    VoC_jal(CII_resetDec);
Decoder_101:


    VoC_lw16i_set_inc(REG0, INPUT_BITS2_ADDR,2);
    VoC_add16_rd(REG0,REG0,CONST_8_ADDR);
//  BEGIN OF CII_bits2prm_hr

//  VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_set_inc(REG1,BITS_SHORT_ADDR,1);

    VoC_loop_i(1,4)
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_loop_i_short(32,DEFAULT)
    VoC_startloop0
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_and16_rr(REG4,REG6,DEFAULT);
    VoC_shr32_ri(REG67,1,DEFAULT);
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_endloop0
    VoC_lw16i(REG6,TABLE_HOMING_n_ADDR+6);
    VoC_endloop1


    VoC_lw16i_short(INC0,-1,DEFAULT);
    VoC_lw16i_set_inc(REG1,BITS_SHORT_ADDR,1);
    VoC_lw16i_set_inc(REG3,STATIC_DE_pswSpeechPara_ADDR,1);
    VoC_lw16i_set_inc(REG2,TABLE_HOMING_n_ADDR,1);

    VoC_loop_i(1,18)

    VoC_lw16inc_p(REG4,REG2,DEFAULT);   // REG4 for loop number
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_add16_rr(REG0,REG1,REG4,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);
    VoC_inc_p(REG0,DEFAULT);

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0
    VoC_shr16_ri(REG5,-1,DEFAULT);
    VoC_add16inc_rp(REG5,REG5,REG0,DEFAULT);
    VoC_endloop0

    // change pointer only if 6 loops passed and REG5 == 0
    VoC_bne16_rr(Bits2prm_100,REG2,REG6);
    VoC_bez16_r(Bits2prm_100,REG5);
    VoC_lw16i_set_inc(REG2,TABLE_HOMING_n1_ADDR+6,1);
Bits2prm_100:

    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_endloop1
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_loop_i_short(4,DEFAULT)
    VoC_startloop0
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_endloop0
//      END OF CII_bits2prm_hr

    //in decoder
    VoC_lw16_d(REG0, DECODER_resetflag_old_ADDR);
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_bez16_r(Decoder_120, REG0);
    VoC_lw16i_short(REG0,9,DEFAULT);
    VoC_jal(CII_decoderHomingFrameTest);
Decoder_120:
    VoC_sw16_d(REG6, DECODER_resetflag_ADDR);

    VoC_bez16_d(Decoder_220, DECODER_resetflag_old_ADDR);
    VoC_bez16_d(Decoder_220, DECODER_resetflag_ADDR);
    VoC_lw16i_set_inc(REG0, STATIC_DE_pswDecodedSpeechFrame_ADDR, 2);
    VoC_lw16i_short(REG2,0x0008,DEFAULT);
    VoC_lw16i_short(REG3,0x0008,IN_PARALLEL);
    VoC_loop_i(0, 80)
    VoC_sw32inc_p(REG23, REG0, DEFAULT);
    VoC_endloop0
    VoC_jump(Decoder_330);
Decoder_220:
    VoC_lw16i_set_inc(REG0, STATIC_DE_pswDecodedSpeechFrame_ADDR, 2);
    VoC_jal(CII_speechDecoder_opt);
Decoder_330:
    VoC_lw16d_set_inc(REG1,DEC_OUTPUT_ADDR_ADDR,1);
//    VoC_jal(CII_speechDecoderHostInterface);
    VoC_lw16i_set_inc(REG0,STATIC_DE_pswDecodedSpeechFrame_ADDR,1);
    VoC_loop_i(0,160)
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_and16_ri(REG6,0xfff8);
    VoC_NOP();
    VoC_sw16inc_p(REG6, REG1, DEFAULT);
    VoC_endloop0

    VoC_bnez16_d(Decoder_440,DECODER_resetflag_old_ADDR)
    VoC_lw16i_short(REG0,18,DEFAULT);
    VoC_jal(CII_decoderHomingFrameTest);
    VoC_sw16_d(REG6,DECODER_resetflag_ADDR);

Decoder_440:
    VoC_lw16_d(REG6,DECODER_resetflag_ADDR);
    VoC_bez16_r(Decoder_550, REG6);
    VoC_jal(CII_resetDec);

Decoder_550:
    VoC_lw16_d(REG7, DECODER_resetflag_ADDR);
    VoC_pop16(RA, DEFAULT);
    VoC_sw16_d(REG7, DECODER_resetflag_old_ADDR);
    VoC_lw16i_short(REG7,0,DEFAULT);

    VoC_return;
}



// ************************************************
// CII_level_estimator_opt
// input  : REG7 -> update
//          REG0 -> pswDecodedSpeechFrame, INC0 = 2
// output : none
// used regs : REG0167, ACC01
// ************************************************


void CII_level_estimator_opt(void)
{

    VoC_push16(RA,DEFAULT);

    // if (update == 0)
    VoC_bnez16_r(level_estimator_opt0,REG7)

    VoC_lw16i_set_inc(REG1,GLOBAL_ERR_plSubfrEnergyMem_ADDR,2);

    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_lw32inc_p(ACC0,REG1,DEFAULT);
    VoC_loop_i_short(3,DEFAULT);

    VoC_startloop0
    VoC_add32inc_rp(ACC0,ACC0,REG1,DEFAULT);
    VoC_endloop0

    // *********************************
    // CII_level_calc_opt
    // input  : ACC0 -> *pl_en
    //          REG0 -> swInd
    // output : ACC1 <- *pl_en
    //          REG7 <- return
    // used : REG0167, ACC01
    // *********************************

    // *pswLevelMean = level_calc(1, &L_sum);
    VoC_jal(CII_level_calc_opt);

    // *pswLevelMax = -72;
    VoC_lw16i(REG6,-72);
    VoC_sw16_d(REG7,DEC_swLevelMean_ADDR);


    // swLevelMem
    VoC_lw16i_set_inc(REG0,GLOBAL_ERR_swLevelMem_ADDR,1);
    VoC_sw16_d(REG6,DEC_swLevelMax_ADDR);

    VoC_loop_i_short(4,DEFAULT);
    VoC_lw16inc_p(REG1,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_bngt16_rr(level_estimator_opt1,REG1,REG6)
    VoC_sw16_d(REG1,DEC_swLevelMax_ADDR);
    VoC_movreg16(REG6,REG1,DEFAULT);
level_estimator_opt1:
    VoC_lw16inc_p(REG1,REG0,DEFAULT);
    VoC_endloop0

    VoC_jump(level_estimator_opt_end);

level_estimator_opt0:

    // else (update != 0)

    // general purpose 1
    VoC_lw16i_short(REG1,1,DEFAULT);

    // set format to (lo >> 3)

    VoC_lw16i_short(FORMAT16,(3-16),DEFAULT);
    VoC_multf16inc_rp(REG6,REG1,REG0,DEFAULT);
    VoC_multf16inc_rp(REG7,REG1,REG0,IN_PARALLEL);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_loop_i_short(20,DEFAULT);

    VoC_startloop0
    VoC_multf16inc_rp(REG6,REG1,REG0,DEFAULT);
    VoC_multf16inc_rp(REG7,REG1,REG0,IN_PARALLEL);
    exit_on_warnings =OFF;
    VoC_bimac32_rr(REG67,REG67);
    exit_on_warnings =ON;
    VoC_endloop0


    // *********************************
    // CII_level_calc_opt
    // input  : ACC0 -> *pl_en
    //          REG0 -> swInd
    // output : ACC1 <- *pl_en
    //          REG7 <- return
    // used : REG0167, ACC01
    // *********************************

    // swLevelSub = level_calc(0, &L_sum);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_jal(CII_level_calc_opt);
    // set back format to (lo << 1)

    VoC_lw16i_short(FORMAT16,(15-16),DEFAULT);
    VoC_lw16i_set_inc(REG0,GLOBAL_ERR_plSubfrEnergyMem_ADDR+6,-2);
    VoC_lw16i_set_inc(REG1,GLOBAL_ERR_swLevelMem_ADDR+3,-1);

    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0

    VoC_lw32_p(ACC1,REG0,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    exit_on_warnings = ON;
    VoC_lw16_p(REG7,REG1,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    exit_on_warnings = ON;

    VoC_endloop0

level_estimator_opt_end:

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


// **************************************
// CII_para_conceal_speech_decoder_opt
// input : none
// output : none
// used regs : all
// **************************************

void CII_para_conceal_speech_decoder_opt(void)
{

    // pswMutePermit in REG7
    VoC_lw16i_short(REG7,0,DEFAULT);

    // pswSpeechPara
    VoC_lw16i_set_inc(REG0,STATIC_DE_pswSpeechPara_ADDR,1);

    // psrR0MuteThreshold
    VoC_lw16i_set_inc(REG1,TABLE_ERR_psrR0RepeatThreshold_ADDR,1);

    // lastR0 in REG6
    VoC_lw16_d(REG6,GLOBAL_ERR_lastR0_ADDR);

    // r0_diff = sub(pswSpeechPara[0], lastR0);
    VoC_sub16_pr(REG6,REG0,REG6,DEFAULT);

    // &psrR0MuteThreshold[lastR0]
    VoC_add16_rr(REG1,REG1,REG6,IN_PARALLEL);

    // pswErrorFlag[0]  ->REG4, pswErrorFlag[1] -> REG5
    VoC_lw32_d(REG45,GLOBAL_BFI_ADDR);

    // REG0 : pswSpeechPara
    // REG1 : &psrR0MuteThreshold[lastR0]
    // REG4 : pswErrorFlag[0]
    // REG5 : pswErrorFlag[1]
    // REG6 : r0_diff
    // REG7 : swMutePermit


    // sub(r0_diff,psrR0RepeatThreshold[lastR0])
    VoC_sub16_rp(REG2,REG6,REG1,DEFAULT);

    // if ((pswErrorFlag[0] == 0) && (pswErrorFlag[1] > 0))
    VoC_bnez16_r(PARA_CONC_opt_100, REG4)
    VoC_bngtz16_r(PARA_CONC_opt_100, REG5)

    // if (sub(r0_diff, psrR0MuteThreshold[lastR0]) >= 0)
    VoC_bltz16_r(PARA_CONC_opt_101,REG2)
    // pswErrorFlag[0] = 1
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jump(PARA_CONC_opt_100);
PARA_CONC_opt_101:

    // if (sub(pswSpeechPara[0], 30) >= 0)
    VoC_lw16_p(REG3,REG0,DEFAULT);
    VoC_bnlt16_rd(PARA_CONC_opt_100,REG3,CONST_30_ADDR)
    // swMutePermit = 1
//          VoC_lw16i_short(REG7,1,DEFAULT);

    // if ((pswErrorFlag[1] > 0)  &&
    //     (pswErrorFlag[0] == 0) &&
    //     (r0_diff >= psrR0MuteThreshold[lastR0]))

    VoC_bngtz16_r(PARA_CONC1_opt_100,REG5)
    VoC_bnez16_r(PARA_CONC1_opt_100,REG4)
    VoC_bltz16_r(PARA_CONC1_opt_100,REG2)
PARA_CONC_opt_100:
    VoC_lw16i_short(REG7,1,DEFAULT);

PARA_CONC1_opt_100:

    // REG0 : pswSpeechPara
    // REG1 : &psrR0MuteThreshold[lastR0]
    // REG4 : pswErrorFlag[0]
    // REG5 : pswErrorFlag[1]
    // REG7 : swMutePermit

    VoC_lw16_d(REG1,GLOBAL_ERR_swState_ADDR);
    exit_on_warnings = OFF;
    VoC_sw16_d(REG7,DEC_swMutePermit_ADDR);
    exit_on_warnings = ON;

    // REG0 : pswSpeechPara, INC0 = 1
    // REG1 : swState,       INC1 = 1
    // REG4 : pswErrorFlag[0]
    // REG5 : pswErrorFlag[1]

    VoC_bgtz16_r(PARA_CONC2_opt_else,REG4)

    // if (pswErrorFlag[0] <= 0)

    // if (sub(swState, 6) < 0) || (swLastFlag == 0)
    VoC_blt16_rd(PARA_CONC2_opt_104,REG1,CONST_6_ADDR)
    VoC_bnez16_d(PARA_CONC2_opt_end,GLOBAL_ERR_swLastFlag_ADDR)
    // swState = 0
PARA_CONC2_opt_104:
    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_jump(PARA_CONC2_opt_end)
PARA_CONC2_opt_else:

    // if (pswErrorFlag[0] > 0)

    // swState++
    VoC_inc_p(REG1,DEFAULT);
    // if (sub(swState, 6) > 0)
    VoC_bngt16_rd(PARA_CONC2_opt_end,REG1,CONST_6_ADDR)
    // swState = 6
    VoC_lw16i_short(REG1,6,DEFAULT);

PARA_CONC2_opt_end:


    // swLastFlag = pswErrorFlag[0]
    VoC_sw16_d(REG4,GLOBAL_ERR_swLastFlag_ADDR);

    // pswLastGood
    VoC_lw16i_set_inc(REG2,GLOBAL_ERR_pswLastGood_ADDR,1);

    // pswErrorFlags
    VoC_sw32_d(REG45,GLOBAL_BFI_ADDR);

    // save back swState
    VoC_sw16_d(REG1,GLOBAL_ERR_swState_ADDR);


    // REG0 : pswSpeechPara, INC0 = 1
    // REG1 : swState,       INC1 = 1
    // REG2 : pswLastGood,   INC2 = 1


    // if (swState == 0)
    VoC_bnez16_r(PARA_CONC3_opt_100,REG1)

    // pswSpeechPara[0]
    VoC_lw16_p(REG7,REG0,DEFAULT);
    VoC_lw16inc_p(REG3,REG0,DEFAULT);
    VoC_loop_i_short(18,DEFAULT);

    VoC_startloop0
    VoC_lw16inc_p(REG3,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG3,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0


    VoC_jump(PARA_CONC3_opt_end)


PARA_CONC3_opt_100:

    // (swState != 0)

    // pswLastGood[0] in REG4
    VoC_lw16_p(REG4,REG2,DEFAULT);

    VoC_blt16_rd(PARA_CONC3_opt_101,REG1,CONST_3_ADDR)
    VoC_bgt16_rd(PARA_CONC3_opt_101,REG1,CONST_5_ADDR)

    // REG0 : pswSpeechPara, INC0 = 1
    // REG1 : swState,       INC1 = 1
    // REG2 : pswLastGood,   INC2 = 1

    // swR0 = sub(pswLastGood[0], 2);
    VoC_sub16_pd(REG4,REG2,CONST_2_ADDR);
    VoC_bltz16_r(PARA_CONC3_opt_101,REG4)
    //      VoC_lw16i_short(REG4,0,DEFAULT);

    VoC_blt16_rd(PARA_CONC3_opt_103,REG1,CONST_6_ADDR)
PARA_CONC3_opt_101:
    // pswLastGood[0] = 0
    VoC_lw16i_short(REG4,0,DEFAULT);
PARA_CONC3_opt_103:


    // REG0 : pswSpeechPara, INC0 = 1
    // REG1 :                INC1 = 1
    // REG2 : pswLastGood,   INC2 = 1
    // REG4 : pswLastGood[0]

    VoC_lw16_p(REG7,REG0,DEFAULT);
    VoC_lw16_d(REG6,GLOBAL_ERR_pswLastGood_ADDR+17);
    VoC_lw16i_set_inc(REG3,STATIC_DE_pswSpeechPara_ADDR+8,3);
    VoC_sw16_p(REG4,REG2,DEFAULT);


    // REG0 : pswSpeechPara,     INC0 = 1
    // REG1 :                    INC1 = 1
    // REG2 : pswLastGood,       INC2 = 1
    // REG3 : &pswSpeechPara[8], INC3 = 3
    // REG6 : pswLastGood[17]
    // REG7 : pswSpeechPara[0]


    // if (pswLastGood[5] == 0)
    VoC_bnez16_d(PARA_CONC3_opt_104,GLOBAL_ERR_pswLastGood_ADDR+5);

    // if (pswSpeechPara[5] == 0)
    VoC_bnez16_d(PARA_CONC3_opt_105,STATIC_DE_pswSpeechPara_ADDR+5)

    // (i=0;i<5;i++) pswSpeechPara[i] = pswLastGood[i];
    VoC_loop_i_short(5,DEFAULT);
    VoC_lw16inc_p(REG5,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    // (i=0;i<4;i++) pswSpeechPara[3*i+8] = pswLastGood[17]
    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    VoC_endloop0

    VoC_jump(PARA_CONC3_opt_end)

PARA_CONC3_opt_105:
    // if (pswSpeechPara[5] != 0)

    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    // (i=0;i<18;i++) pswSpeechPara[i] = pswLastGood[i];
    VoC_loop_i_short(18,DEFAULT);

    VoC_startloop0
    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    // (i=0;i<3;i++) pswSpeechPara[3*i+8] = pswLastGood[17]
    VoC_loop_i_short(3,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    VoC_endloop0

    VoC_jump(PARA_CONC3_opt_end)

PARA_CONC3_opt_104:


    // if (pswLastGood[5] != 0)

    VoC_lw16i_short(REG4,8,DEFAULT);
    // swLastLag = pswLastGood[6]
    VoC_lw16_d(REG5,GLOBAL_ERR_pswLastGood_ADDR+6);

    // &pswLastGood[9]
    VoC_lw16i_set_inc(REG1,GLOBAL_ERR_pswLastGood_ADDR+9,3);


    // REG0 : pswSpeechPara,     INC0 = 1
    // REG1 : &pswLastGood[9],   INC1 = 3
    // REG2 : pswLastGood,       INC2 = 1
    // REG3 : &pswSpeechPara[8], INC3 = 3
    // REG5 : swLastLag
    // REG6 : pswLastGood[17]
    // REG7 : pswSpeechPara[0]


    VoC_loop_i_short(3,DEFAULT);

    VoC_startloop0

    // swLag = sub(pswLastGood[3 * i + 9], 0x8)
    VoC_sub16inc_pr(REG4,REG1,REG4,DEFAULT);
    // swLag = add(swLag, swLastLag)
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    // default : swLastLag = swLag
    VoC_add16_rr(REG5,REG4,REG5,IN_PARALLEL);

    // if (sub(swLag, 0x00ff) > 0)
    VoC_bngt16_rd(PARA_CONC3_opt_LOOP1,REG4,CONST_0x00ff_ADDR)
    // swLastLag = 0x00ff
    VoC_lw16i(REG5,0x00ff);

    VoC_jump(PARA_CONC3_opt_LOOP2)

PARA_CONC3_opt_LOOP1:

    // if (swLag < 0)
    VoC_bnltz16_r(PARA_CONC3_opt_LOOP2, REG4)
    // swLastLag = 0
    VoC_lw16i_short(REG5,0,DEFAULT);
PARA_CONC3_opt_LOOP2:
    VoC_lw16i_short(REG4,8,DEFAULT);

    VoC_endloop0


    // REG0 : pswSpeechPara,     INC0 = 1
    // REG1 : &pswLastGood[18],  INC1 = 3
    // REG2 : pswLastGood,       INC2 = 1
    // REG3 : &pswSpeechPara[8], INC3 = 3
    // REG4 : 8
    // REG5 : swLastLag
    // REG6 : pswLastGood[17]
    // REG7 : pswSpeechPara[0]


    // &pswLastGood[6]
    VoC_lw16i_set_inc(REG1,GLOBAL_ERR_pswLastGood_ADDR+15,-3);

    VoC_loop_i_short(3,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_endloop0

    // pswLastGood[6] = swLastLag
    VoC_sw16_p(REG5,REG1,DEFAULT);
    VoC_lw16i_short(INC1,3,IN_PARALLEL);

    // REG0 : pswSpeechPara,     INC0 = 1
    // REG1 : &pswLastGood[6],   INC1 = 3
    // REG2 : pswLastGood,       INC2 = 1
    // REG3 : &pswSpeechPara[8], INC3 = 3
    // REG6 : pswLastGood[17]
    // REG7 : pswSpeechPara[0]


    // if (pswSpeechPara[5] != 0)
    VoC_bez16_d(PARA_CONC3_opt_106,STATIC_DE_pswSpeechPara_ADDR+5)


    // REG0 : pswSpeechPara,     INC0 = 1
    // REG1 : &pswLastGood[6],   INC1 = 3
    // REG2 : pswLastGood,       INC2 = 1
    // REG3 : &pswSpeechPara[8], INC3 = 3
    // REG6 : pswLastGood[17]
    // REG7 : pswSpeechPara[0]

    // (i=0;i<6;i++) pswSpeechPara[i] = pswLastGood[i];
    VoC_loop_i_short(6,DEFAULT);
    VoC_lw16inc_p(REG4,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0


    VoC_lw16i_short(INC0,3,DEFAULT);
    // REG0 : &pswSpeechPara[6], INC0 = 3
    // REG1 : &pswLastGood[6],   INC1 = 3
    // REG2 : &pswLastGood[7],   INC2 = 1
    // REG3 : &pswSpeechPara[8], INC3 = 3
    // REG6 : pswLastGood[17]
    // REG7 : pswSpeechPara[0]

    // (i=0;i<4;i++) pswSpeechPara[3*i+6] = pswLastGood[3*i+6];
    VoC_loop_i_short(4,DEFAULT);
    VoC_lw16inc_p(REG4,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0


    // (i=0;i<4;i++) pswSpeechPara[3*i+8] = pswLastGood[17]
    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    VoC_endloop0



    VoC_jump(PARA_CONC3_opt_end)



    // if (pswSpeechPara[5] == 0)

PARA_CONC3_opt_106:

    // (i=0;i<18;i++) pswSpeechPara[i] = pswLastGood[i];
    VoC_loop_i_short(18,DEFAULT);
    VoC_lw16inc_p(REG4,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    // (i=0;i<3;i++) pswSpeechPara[3*i+8] = pswLastGood[17]
    VoC_loop_i_short(3,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    VoC_endloop0


PARA_CONC3_opt_end:


    // REG7 : pswSpeechPara[0]
    // lastR0 = pswSpeechPara[0]
    VoC_sw16_d(REG7,GLOBAL_ERR_lastR0_ADDR);

    VoC_return;
}

// *********************************
// CII_level_calc_opt
// input  : ACC0 -> *pl_en
//          REG0 -> swInd
// output : ACC1 <- *pl_en
//          REG7 <- return
// used : REG0167, ACC01
// *********************************


void CII_level_calc_opt(void)
{

    VoC_push16(RA,DEFAULT);

    // swPos = 0, swLevel = 0
    VoC_lw32z(REG67,DEFAULT);

    // *pl_en in ACC1
    VoC_movreg32(ACC1,ACC0,IN_PARALLEL);

    // if (*pl_en != 0)
    VoC_bez32_r(level_calc_opt0,ACC0);

    // *****************************
    // CII_NORM_L_ACC0
    // input : ACC0
    // output : REG1
    // note : ACC0 <- (ACC0 >> REG1)
    // *****************************

    // -swPos = sub((Shortword) -29, -norm_l(*pl_en));
    VoC_jal(CII_NORM_L_ACC0);
    VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
    VoC_lw16i_short(REG6,-29,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG1,DEFAULT);

    // swLevel in REG7
    VoC_sub16_rr(REG7,REG1,REG6,IN_PARALLEL);
    VoC_shr16_ri(REG7,-2,DEFAULT);

level_calc_opt0:

    // L_tmp = L_shl((Longword) 1, swPos)
    // (*pl_en & L_tmp)
    VoC_lw32_d(ACC0,CONST_1_ADDR);
    VoC_shr32_rr(ACC0,REG6,DEFAULT);
    VoC_and32_rr(ACC0,ACC1,DEFAULT);

    // if (-swPos <= 0) && (*pl_en & L_tmp) != 0)
    VoC_bgtz16_r(level_calc_opt1,REG6);
    VoC_bez32_r(level_calc_opt1,ACC0);
    VoC_add16_rd(REG7,REG7,CONST_2_ADDR);
level_calc_opt1:

    // L_tmp = L_shl((Longword) 1, --swPos) =
    VoC_lw32_d(ACC0,CONST_1_ADDR);
    VoC_shr32_rr(ACC0,REG6,DEFAULT);
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_and32_rr(ACC0,ACC1,DEFAULT);

    // if (-swPos <= -1) && (*pl_en & L_tmp) != 0)
    VoC_bgt16_rd(level_calc_opt2,REG6,CONST_neg1_ADDR);
    VoC_bez32_r(level_calc_opt2,ACC0);
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);
level_calc_opt2:

    // pop RA
    VoC_pop16(RA,DEFAULT);

    // swLevel += shl(swLevel, 1);
    VoC_movreg16(REG6,REG7,DEFAULT);
    VoC_shr16_ri(REG7,-1,IN_PARALLEL);
    VoC_add16_rr(REG7,REG7,REG6,DEFAULT);


    // swLevel -= (swInd == 0) ? 353 : 377;
    // L_temp   = (swInd == 0) ? 80 : 320;
    VoC_lw16i(REG6,353);
    VoC_lw16i(REG1,80);
    VoC_bez16_r(level_calc_opt4,REG0)
    VoC_lw16i(REG6,377);
    VoC_lw16i(REG1,320);
level_calc_opt4:
    VoC_sub16_rr(REG7,REG7,REG6,DEFAULT);

    // swLevel = mult_r(swLevel, 0X2000);
    VoC_multf32_rd(REG67,REG7,CONST_0x2000_ADDR);
    VoC_NOP();
    // DEBUG: add when debugging
    VoC_add32_rd(REG67,REG67,CONST_0x00008000L_ADDR);
    VoC_bgt16_rd(level_calc_opt5,REG7,CONST_neg72_ADDR);
    VoC_lw16i(REG7,-72);
    VoC_lw16i_short(ACC1_HI,0,DEFAULT);
    VoC_movreg16(ACC1_LO,REG1,IN_PARALLEL);

level_calc_opt5:

    VoC_return;
}


// input : REG0 <- iLastPara
// output: REG6
// used:   REG01234567

void CII_decoderHomingFrameTest(void)
{

    VoC_lw16i_set_inc(REG1, STATIC_DE_pswSpeechPara_ADDR,1);
    VoC_lw16i_set_inc(REG2, TABLE_HOMING_n_ADDR,1);
    VoC_lw16i_set_inc(REG3, TABLE_HOMING_dhf_mask_ADDR,1);

    VoC_loop_r_short(REG0,DEFAULT)
    VoC_lw16i_short(REG0,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16i_short(REG5,-1,DEFAULT);
    VoC_sub16inc_rp(REG4,REG0,REG2,IN_PARALLEL);

    VoC_shr16_rr(REG5,REG4,DEFAULT);
    VoC_lw16inc_p(REG6,REG1,IN_PARALLEL);

    VoC_not16_r(REG5,DEFAULT);
    VoC_lw16inc_p(REG7,REG3,IN_PARALLEL);

    VoC_and16_rr(REG6,REG5,DEFAULT);
    VoC_xor16_rr(REG6,REG7,DEFAULT);

    VoC_bgtz16_r(CII_decoderHomingFrameTest_100,REG6)
    VoC_endloop0

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_return;

CII_decoderHomingFrameTest_100:

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_return;
}








// ***************************************
// CII_lookupVq_opt
// input : none
// output : none
// used regs : all 16_bit, RL7
// ***************************************



void CII_lookupVq_opt(void)
{

    // !!! the following define is to be added in the header
    // #define CONST_0x2L_ADDR  2*0xa43
    // siSeg in RL7
    VoC_lw32z(RL7,DEFAULT);

    VoC_lw16i_set_inc(REG0,GLOBAL_DE_SP_pswVq_ADDR,1);
    VoC_lw16i_set_inc(REG2,TABLE_SP_ROM_psvqIndex_ADDR,4);


    // large loop, so loop_short does no compile
    VoC_loop_i(0,3)

    // REG0 : &pswVqCodeWds[siSeg]
    // REG2 : &psvqIndex[siSeg]
    // RL7 :  siSeg

    // if (siSeg == 2)
    // mult_factor = 4
    VoC_bne32_rd(LOOKUP_opt_100,RL7,CONST_0x2L_ADDR)
    VoC_lw16i(REG6,TABLE_SP_ROM_psrQuant3_ADDR);
    VoC_lw16i_short(REG1,4,DEFAULT);
    VoC_jump(LOOKUP_opt_101);
LOOKUP_opt_100:
    // if (siSeg != 2)
    // mult_factor = 3
    VoC_lw16i_short(REG1,3,DEFAULT);
    VoC_lw16i(REG6,TABLE_SP_ROM_psrQuant1_ADDR);
    VoC_bez32_r(LOOKUP_opt_101, RL7)
    VoC_lw16i(REG6,TABLE_SP_ROM_psrQuant2_ADDR);
LOOKUP_opt_101:
    // &psrQTable[siWordPtr]
    VoC_mult16_rp(REG1,REG1,REG0,DEFAULT);
    VoC_NOP();
    VoC_shr16_ri(REG1,1,DEFAULT);
    VoC_add16_rr(REG7,REG6,REG1,DEFAULT);

    VoC_lw16i(REG1,0xff);

    // REG0 : &pswVqCodeWds[siSeg]
    // REG1 : 0xff
    // REG2 : &psvqIndex[siSeg]
    // REG7 : &psrQTable[siWordPtr]
    // RL7 :  siSeg

    // DEBUG: could not use 32-bit load because they are aligned
    // to odd address
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);
    exit_on_warnings=OFF;
    VoC_lw16_p(REG4,REG7,DEFAULT);
    exit_on_warnings=ON;
    VoC_lw16_p(REG5,REG7,DEFAULT);
    VoC_movreg32(REG67,REG45,DEFAULT);

    // REG0 : &pswVqCodeWds[siSeg]
    // REG1 : 0xff
    // REG2 : &psvqIndex[siSeg]
    // REG4 : siVector1
    // REG5 : siVector2
    // REG6 : siVector1
    // REG7 : siVector2
    // RL7 :  siSeg

    VoC_and16_rr(REG4,REG1,DEFAULT);
    VoC_shr16_ri(REG6,8,IN_PARALLEL);
    VoC_and16_rr(REG5,REG1,DEFAULT);
    VoC_shr16_ri(REG7,8,IN_PARALLEL);
    VoC_and16_rr(REG6,REG1,DEFAULT);
    VoC_and16_rr(REG7,REG1,DEFAULT);

    VoC_lw16i_set_inc(REG1,TABLE_SP_ROM_psrSQuant_ADDR,1);

    // REG0 : &pswVqCodeWds[siSeg]
    // REG1 : psrSQuant
    // REG2 : &psvqIndex[siSeg]
    // REG4 : siVector1 & LSP_MASK
    // REG5 : siVector2 & LSP_MASK
    // REG6 : (siVector1 >> 8) & LSP_MASK
    // REG7 : (siVector2 >> 8) & LSP_MASK
    // RL7 :  siSeg

    VoC_add16_rr(REG4,REG4,REG1,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG1,IN_PARALLEL);
    VoC_add16_rr(REG6,REG6,REG1,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG1,DEFAULT);

    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_lw16_p(REG4,REG4,DEFAULT);
    VoC_lw16_p(REG7,REG7,DEFAULT);
    VoC_lw16_p(REG5,REG5,DEFAULT);

    // REG0 : &pswVqCodeWds[siSeg]
    // REG1 : psrSQuant
    // REG2 : &psvqIndex[siSeg]
    // REG4 : psrSQuant[siVector1 & LSP_MASK]
    // REG5 : psrSQuant[siVector2 & LSP_MASK]
    // REG6 : psrSQuant[(siVector1 >> 8) & LSP_MASK]
    // REG7 : psrSQuant[(siVector2 >> 8) & LSP_MASK]
    // RL7 :  siSeg

    // pswRCOut[siIndex - 1]
    VoC_lw16i_set_inc(REG3,DEC_pswFrmKs_ADDR-1,1);
    VoC_add16inc_rp(REG3,REG3,REG2,DEFAULT);

    VoC_lw16inc_p(REG1,REG0,DEFAULT);
    VoC_and16_ri(REG1,0x0001);

    // REG0 : &pswVqCodeWds[siSeg]
    // REG1 : siVector & 1
    // REG2 : &psvqIndex[siSeg]
    // REG3 : &pswRCOut[siIndex-1]
    // REG4 : psrSQuant[siVector1 & LSP_MASK]
    // REG5 : psrSQuant[siVector2 & LSP_MASK]
    // REG6 : psrSQuant[(siVector1 >> 8) & LSP_MASK]
    // REG7 : psrSQuant[(siVector2 >> 8) & LSP_MASK]
    // RL7 :  siSeg

    VoC_sw16_p(REG6,REG3,DEFAULT);

    VoC_be32_rd(LOOKUP_opt_103,RL7,CONST_0x2L_ADDR)
    VoC_bnez16_r(LOOKUP_opt_102,REG1)
LOOKUP_opt_103:
    VoC_inc_p(REG3,DEFAULT);

LOOKUP_opt_102:
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_sw16inc_p(REG7,REG3,DEFAULT);

    VoC_be32_rd(LOOKUP_opt_END1,RL7,CONST_0x2L_ADDR)
    VoC_bez16_r(LOOKUP_opt_END,REG1)
LOOKUP_opt_END1:
    VoC_sw16_p(REG5,REG3,DEFAULT);
LOOKUP_opt_END:

    VoC_add32_rd(RL7,RL7,CONST_1_ADDR);
    VoC_endloop0

    VoC_return;
}



// ******************************************************
// CII_a_sst_opt
// input : REG0 -> pswDirectFormCoefIn (param2), INC0 = 1
//         REG4 -> pswDirectFormCoefOut (param3)
// note  : swAshift = 4, swAscale = 0x800 always
// output : none
// used regs : all
// ******************************************************


void CII_a_sst_opt(void)
{

#if  0

    voc_short A_SST_pL_CorrTemp_ADDR      ,22,y                            // [NP + 1] LONG
    voc_short A_SST_pswRCNum_ADDR         ,10,y                            // [NP]
    voc_short A_SST_pswRCDenom_ADDR       ,10,y                            // [NP]

#endif

    VoC_push16(RA,DEFAULT);
    VoC_push16(REG4,DEFAULT);


    // ***********************************************
    // CII_aToRc_opt2
    // input  : REG0 -> pswAin (param1),     INC0 = 1
    //          REG1 -> &pswRc[9] (param2),  INC1 = -1
    // note   : param0 is always 4
    // output : REG7 <- return
    // ***********************************************

    VoC_lw16i_set_inc(REG1,A_SST_pswRCDenom_ADDR+9,-1);
    VoC_jal(CII_aToRc_opt2);

    // **************************************************
    // CII_rcToCorrDpL_opt
    // note :  1st two parameters are not passed, because
    //         they are always the same constants :
    //         swAshift = 4, swAscale = 0x800
    // input:  REG4 : pswRc (param2), INC0 = 2
    //         REG1 : pL_R  (param3), INC1 = 2
    // output: none
    // **************************************************

    VoC_lw16i_set_inc(REG1,A_SST_pL_CorrTemp_ADDR,2);
    VoC_lw16i(REG4,A_SST_pswRCDenom_ADDR);
    VoC_jal(CII_rcToCorrDpL_opt);

    VoC_lw16i_set_inc(REG1,TABLE_DE_A_SST_psrSST_ADDR+1,1);
    VoC_lw16i_set_inc(REG2,A_SST_pL_CorrTemp_ADDR+2,2);

    VoC_loop_i_short(10,DEFAULT)
    VoC_lw16inc_p(REG4,REG1,IN_PARALLEL);
    VoC_startloop0

    // ***************************
    // CII_L_mpy_ls_opt
    // input ACC0, REG4
    // output ACC0
    // other used regs : REG5
    // unchanged regs  : REG4
    // ****************************

    VoC_lw32_p(ACC0,REG2,DEFAULT);
    VoC_jal(CII_L_mpy_ls_opt);
    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_endloop0

    // *******************************
    // CII_aFlatRcDp_opt
    // input : RL6_LO -> pL_R
    //         RL6_HI -> pswRc
    // output : none
    // used regs : all
    // *******************************

    VoC_lw16i(RL6_LO,A_SST_pL_CorrTemp_ADDR);
    VoC_lw16i(RL6_HI,A_SST_pswRCNum_ADDR);
    VoC_jal(CII_aFlatRcDp_opt);

    // ***********************************
    // CII_rcToADp_opt_new
    // input  : REG0 -> pswRc,   INC0 =  1
    //          REG1 -> pswA,    INC1 = -2
    //          0x800  swAscale is a const
    // output : REG7 <- return
    // ***********************************

    VoC_pop16(REG1,DEFAULT);

    VoC_lw16i_short(INC1,-2,DEFAULT);
    VoC_lw16i(REG0,A_SST_pswRCNum_ADDR);
    VoC_jal(CII_rcToADp_opt_new);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}







// *******************************
// CII_linInterpSidLoop_opt
// input :  none
// output : none
// used register : REG012345,ACC01
// *******************************


void CII_linInterpSidLoop_opt(void)
{

    VoC_lw16i_set_inc(REG1,STATIC_DTX_pL_NewCorrSeq_ADDR,2);
    VoC_lw16i_set_inc(REG2,STATIC_DTX_pL_OldCorrSeq_ADDR,2);
    VoC_lw16i_set_inc(REG3,STATIC_DTX_pL_CorrSeq_ADDR,2);

    // *******************************
    // inline CII_linInterpSid_opt
    // input :  ACC0 -> L_New
    //          ACC1 -> L_Old
    //          REG0 -> swDtxState
    // output : ACC0 -> return
    // used register : REG045,ACC0,ACC1
    // *******************************

    //for (i = 0; i < NP + 1; i++)
    VoC_loop_i_short(11,DEFAULT);
    VoC_startloop0

    // L_New
    VoC_lw32inc_p(ACC0,REG1,DEFAULT);

    // &psrCNNewFactor[swDtxState] in REG0
    VoC_lw16i(REG4,TABLE_DTX_psrCNNewFactor_ADDR);
    VoC_add16_rd(REG0,REG4,GLOBAL_DTX_DE_swRxDTXState_ADDR);

    // push RA
    VoC_push16(RA,DEFAULT);

    // psrCNNewFactor[swDtxState]
    VoC_lw16_p(REG4,REG0,DEFAULT);

    // *************************
    // CII_L_mpy_ls_opt
    // input ACC0, REG4
    // output ACC0
    // other used regs : REG5
    // unchanged regs  : REG4
    // *************************

    // L_New = L_mpy_ls(L_New, psrCNNewFactor[swDtxState])
    VoC_jal(CII_L_mpy_ls_opt);

    // swap old & new
    VoC_movreg32(ACC1,ACC0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG2,IN_PARALLEL);

    VoC_lw16_d(REG4,CONST_0x7fff_ADDR);
    // swOldFactor in REG4
    VoC_sub16_rp(REG4,REG4,REG0,DEFAULT);
    VoC_add16_rd(REG4,REG4,CONST_1_ADDR);

    // *************************
    // CII_L_mpy_ls_opt
    // input ACC0, REG4
    // output ACC0
    // other used regs : REG5
    // unchanged regs  : REG4
    // *************************

    // L_Old = L_mpy_ls(L_Old, swOldFactor);
    VoC_jal(CII_L_mpy_ls_opt);

    // return (L_add(L_New, L_Old));
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);

    VoC_pop16(RA,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);

    VoC_endloop0

    VoC_return;
}









// *************************
// CII_linInterpSidShort_opt
// input :  REG3 -> swNew
//          REG4 -> swOld
//          REG0 -> swDtxState
// output : ACC0_HI -> return
// used register : REG0134,ACC0
// ********************


void CII_linInterpSidShort_opt(void)
{

    // psrCNNewFactor[swDtxState] in REG0
    VoC_lw16i(REG1,TABLE_DTX_psrCNNewFactor_ADDR);
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);

    // DEBUG: VoC_sub16_dp(REG1,CONST_0x7fff_ADDR,REG0); is not possible
    // as they are in the same ROM
    VoC_lw16i(REG1,0x7fff);
    // swOldFactor
    VoC_sub16_rp(REG1,REG1,REG0,DEFAULT);

    // returnL += L_mult(swNew, psrCNNewFactor[swDtxState]);
    VoC_lw32_d(ACC0,CONST_0x00008000L_ADDR);
    VoC_mac32_rp(REG3,REG0,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);

    // returnL += L_mult(swOld, swOldFactor);
    VoC_mac32_rr(REG4,REG1,DEFAULT);

    VoC_return;
}



void  CII_extra_HR_bfi_filter(void)
{
    //start of extra_HR_bfi_filter
    VoC_lw16_d(ACC0_LO,GLOBAL_BFI_ADDR);
    VoC_bez16_d(CII_extra_HR_bfi_filter_exit,GLOBAL_UFI_ADDR)

    VoC_lw16_d(REG2,STATIC_DE_pswSpeechPara_ADDR);
    VoC_sub16_rd(REG3,REG2, GLOBAL_swOldR0_ADDR );
    VoC_bnlt16_rd(CII_extra_HR_bfi_filter_100,REG3,CONST_25_ADDR)
    VoC_sw16_d(REG2,GLOBAL_swOldR0_ADDR);

    VoC_bne16_rd(CII_extra_HR_bfi_filter_exit,REG1,CONST_33_ADDR)  //branch if no speech
    VoC_bez16_d(CII_extra_HR_bfi_filter_exit,GLOBAL_swPreBfi_ADDR)
    VoC_lw16_d(REG4,(STATIC_DE_pswSpeechPara_ADDR+5));
    VoC_blt16_rd(CII_extra_HR_bfi_filter_exit,REG4,CONST_3_ADDR)

CII_extra_HR_bfi_filter_100:
    VoC_lw16i(REG1,44);    // swFrameType = UNUSABLE;

CII_extra_HR_bfi_filter_exit:
    VoC_sw16_d(ACC0_LO,GLOBAL_swPreBfi_ADDR);

    //end of extra_HR_bfi_filter
    VoC_return


}






// ***********************************************
// CII_speechDecoder_opt
// input : REG0 -> &pswDecodedSpeechFrame[giSfrmCnt*S_LEN], INC0 = 2
// output : none
// used : all
// ***********************************************

void CII_speechDecoder_opt(void)
{

#if 0

    voc_short DEC_pswFrmPFDenom_ADDR         , 10  ,y                        // [NP]
    voc_short DEC_pswFrmKs_ADDR              , 10  ,y                        // [NP]=10
    voc_short DEC_pswFrmAs_ADDR              , 10  ,y                        // [NP]
    voc_short DEC_pswFrmPFNum_ADDR           , 20  ,y                        // [2*NP]
    voc_short DEC_siGsp0Code_ADDR                 ,y
    voc_short FLAG_GETSFRMLPC_TX_ADDR             ,y
    voc_short DEC_psiVselpCw_ADDR            , 2  ,y                         // [2]
    voc_short DEC_pswPVec_ADDR               , 40  ,y                        // [S_LEN]=40
    voc_short DEC_ppswVselpEx_ADDR           , 80  ,y                        // [2][S_LEN]
    voc_short DEC_pswExcite_ADDR             , 40  ,y                        // [S_LEN]
    voc_short DEC_pswPPFExcit_ADDR           , 40  ,y                        // [S_LEN]
    voc_short DEC_pswBitArray_ADDR           , 10  ,y                        // [MAXBITS]=9
    voc_short DEC_psnsSqrtRs_ADDR            , 8  ,y                         // [N_SUB]*2=8
    voc_short DEC_snsRs00_ADDR               , 2  ,y                         // 2
    voc_short DEC_snsRs11_ADDR               , 2  ,y                         // 2
    voc_short DEC_snsRs22_ADDR               , 2  ,y                         // 2
    voc_short DEC_swMutePermit_ADDR               ,y
    voc_short DEC_swLevelMean_ADDR                ,y
    voc_short DEC_swLevelMax_ADDR                 ,y
    voc_short DEC_swMuteFlag_ADDR                 ,y

#endif


#if 0
    voc_short DEC_pswSynthFiltOut_ADDR,40,x
#endif


    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG2,0,DEFAULT);

    // swBfiDtx_L : No need to load & compute the BFI ->
    // swBfiDtx = pswParameters[18] | pswParameters[19]
    // because only two states are considered : 0 and !0.
    // We can directly check if the 32-bit word formed by
    // pswParameters[18] and pswParameters[19] is 0 or not.

    // swSID & swTAF
    VoC_lw32_d(REG45,GLOBAL_SID_ADDR);

    // swMuteFlag = 0;
    // swMutePermit = 0;
    VoC_sw16_d(REG2,DEC_swMuteFlag_ADDR);
    VoC_sw16_d(REG2,DEC_swMutePermit_ADDR);

    // REG0 : pswDcdSpchFrm
    // REG2 : 0
    // REG4 : swSID
    // REG5 : swTAF

    // default : INVALIDSID
    VoC_lw16i_short(REG1,22,DEFAULT);
    // if ((swSID == 0)
    VoC_bnez16_r(DEC_FRAME_CLASS_100,REG4);
    // swFrameType = GOODSPEECH;
    VoC_lw16i(REG1,33);


    // if (swBfiDtx_L != 0)
    VoC_bez16_d(DEC_FRAME_CLASS_100,GLOBAL_BFI_ADDR);
    // swFrameType = UNUSABLE;
    VoC_lw16i(REG1,44);
    VoC_jump(DEC_FRAME_CLASS_101);

DEC_FRAME_CLASS_100:

    // if (swSID == 2)
    VoC_bne16_rd(DEC_FRAME_CLASS_101,REG4,CONST_2_ADDR);
    // if (swBfiDtx_L == 0)
    VoC_bnez16_d(DEC_FRAME_CLASS_101,GLOBAL_BFI_ADDR);  // BFI
    // swFrameType = VALIDSID;
    VoC_lw16i_short(REG1,11,DEFAULT);

DEC_FRAME_CLASS_101:

    VoC_jal(CII_extra_HR_bfi_filter);

    VoC_lw16i_short(REG2,1,DEFAULT);

    // REG0 : pswDcdSpchFrm
    // REG1 : swFrameType
    // REG2 : 1
    // REG4 : swSID
    // REG5 : swTAF


    // if (swDecoMode == SPEECH)
    VoC_bne16_rd(DEC_UPDATE_STATE_100,REG2,GLOBAL_DE_swDecoMode_ADDR)

    // case list is exhaustive, can put a default value
    // default swDecoMode = SPEECH;
    VoC_lw16i_short(REG3,1,DEFAULT);
    // jump if (swFrameType == UNUSABLE)
    // jump if (swFrameType == GOODSPEECH)
    VoC_bgt16_rd(DEC_UPDATE_STATE_END,REG1,CONST_22_ADDR)
    // else (swFrameType == VALIDSID) || (swFrameType == INVALIDSID)
    // swDecoMode = CNIFIRSTSID
    VoC_lw16i_short(REG3,2,DEFAULT);
    VoC_jump(DEC_UPDATE_STATE_END);

DEC_UPDATE_STATE_100:

    // else

    // case list is exhaustive, can put a default value
    // default swDecoMode = SPEECH;
    VoC_lw16i_short(REG3,1,DEFAULT);
    // jump if (swFrameType == GOODSPEECH)
    VoC_be16_rd(DEC_UPDATE_STATE_END,REG1,CONST_33_ADDR)
    // else swDecoMode = CNIBFI
    VoC_lw16i_short(REG3,4,DEFAULT);
    // jump if (swFrameType == UNUSABLE)
    VoC_be16_rd(DEC_UPDATE_STATE_END,REG1,CONST_44_ADDR)
    // else (swFrameType == VALIDSID) || (swFrameType == INVALIDSID)
    // swDecoMode = CNICONT;
    VoC_lw16i_short(REG3,3,DEFAULT);

DEC_UPDATE_STATE_END:


    // REG0 : pswDcdSpchFrm
    // REG1 : swFrameType
    // REG2 : 1
    // REG3 : swDecoMode
    // REG4 : swSID
    // REG5 : swTAF

    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG1,DEFAULT);
    // make place for swR0Index
    // push anything
    VoC_push16(REG0,DEFAULT);


    // STACK16              STACK32
    // ------------------------------------------
    // RA
    // pswDcdSpchFrm
    // swFrameType
    // swR0Index


    VoC_sw16_d(REG3,GLOBAL_DE_swDecoMode_ADDR);


    // if (swDecoMode == SPEECH)
    VoC_bne16_rr(DEC_NOT_SPEECH_MODE100,REG2,REG3)


    // **************************************
    // CII_para_conceal_speech_decoder_opt
    // input : none
    // output : none
    // used regs : all
    // **************************************

    VoC_jal(CII_para_conceal_speech_decoder_opt);

    // copy the frame rate parameters
    VoC_lw16i_set_inc(REG0,STATIC_DE_pswSpeechPara_ADDR,2);
    VoC_lw16i_set_inc(REG1,GLOBAL_DE_SP_pswVq_ADDR,1);

    // load pswParameters[0:5]
    VoC_lw32inc_p(REG23,REG0,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_lw32inc_p(REG67,REG0,DEFAULT);

    // store pswVq[0]
    VoC_sw16inc_p(REG3,REG1,DEFAULT);

    // store swR0Index
    VoC_sw16_sd(REG2,0,DEFAULT);

    // &psrR0DecTbl[swR0Index * 2]
    VoC_shr16_ri(REG2,-1,IN_PARALLEL);
    VoC_lw16i(REG3,TABLE_SP_ROM_psrR0DecTbl_ADDR);
    VoC_add16_rr(REG3,REG3,REG2,DEFAULT);

    // store pswVq[1:2]
    VoC_sw16inc_p(REG4,REG1,IN_PARALLEL);
    VoC_sw16inc_p(REG5,REG1,DEFAULT);

    // swR0Dec = psrR0DecTbl[swR0Index * 2];
    VoC_lw16_p(REG3,REG3,DEFAULT);

    // store global vars
    VoC_sw16_d(REG6,GLOBAL_DE_SP_swSi_ADDR);
    VoC_sw16_d(REG7,GLOBAL_DE_SP_swVoicingMode_ADDR);
    VoC_sw16_d(REG3,GLOBAL_DE_SP_swR0Dec_ADDR);

    // ***************************************
    // CII_lookupVq_opt
    // input : none
    // output : none
    // used regs : all 16_bit, RL7
    // ***************************************

    VoC_jal(CII_lookupVq_opt);

    // &ppLr_gsTable[swVoicingMode][0]
    VoC_lw16i_set_inc(REG3,STATIC_DE_pswSpeechPara_ADDR+8,3);
    VoC_lw16i_set_inc(REG1,TABLE_DTX_ppLr_gsTable_ADDR,2);
    VoC_lw16_d(REG4,GLOBAL_DE_SP_swVoicingMode_ADDR);
    VoC_shr16_ri(REG4,-6,DEFAULT);
    VoC_add16_rr(REG4,REG1,REG4,DEFAULT);

    // CNINTPER - 1
    VoC_lw16i_short(REG6,11,IN_PARALLEL);

    // &ppLr_gsTable[swVoicingMode][pswParameters[8]]
    VoC_add16_rp(REG5,REG4,REG3,DEFAULT);

    // swDtxBfiCnt = 0
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    // &ppLr_gsTable[swVoicingMode][pswParameters[8]]
    // DEBUG: add when debugging
    VoC_add16inc_rp(REG5,REG5,REG3,DEFAULT);

    // &pL_RxGsHist[0]
    VoC_lw16i_set_inc(REG0,GLOBAL_DE_pL_RxGsHist_ADDR,2);
    // &pL_RxGsHist[swRxGsHistPtr]
    VoC_add16_rd(REG1,REG0,GLOBAL_DE_swRxGsHistPtr_ADDR);
    // &pL_RxGsHist[28]
    VoC_lw16i_set_inc(REG2,GLOBAL_DE_pL_RxGsHist_ADDR+56,2);


    // REG0 : &pL_RxGsHist[0],             INC0 = 2
    // REG1 : &pL_RxGsHist[swRxGsHistPtr], INC1 = 2
    // REG2 : &pL_RxGsHist[27],            INC2 = 2
    // REG3 : &pswParameters[(i * 3) + 8], INC3 = 3
    // REG4 : &ppLr_gsTable[swVoicingMode][0]
    // REG5 : &ppLr_gsTable[swVoicingMode][pswParameters[(i*3)+8]
    // REG6 : CNINTPER - 1
    // REG7 : 0


    VoC_loop_i_short(4,DEFAULT);
    // ACC0 <- ppLr_gsTable[swVoicingMode][pswParameters[(i * 3) + 8]
    VoC_lw32_p(ACC0,REG5,IN_PARALLEL);
    VoC_startloop0

    // &ppLr_gsTable[swVoicingMode][pswParameters[((i+1)*3)+8]]
    VoC_add16_rp(REG5,REG4,REG3,DEFAULT);
    // &ppLr_gsTable[swVoicingMode][pswParameters[8]]
    // DEBUG: add when debugging
    VoC_add16inc_rp(REG5,REG5,REG3,DEFAULT);

    // pL_RxGsHist[swRxGsHistPtr] <- ACC0
    // swRxGsHistPtr++;
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);

    // if (swRxGsHistPtr > ((OVERHANG - 1) * N_SUB) - 1), wrap
    // DEBUG: modified from VoC_be to VoC_bne
    VoC_bne16_rr(DEC_WRAP_GSHIST,REG1,REG2);
    VoC_movreg16(REG1,REG0,DEFAULT);
DEC_WRAP_GSHIST:
    VoC_lw32_p(ACC0,REG5,DEFAULT);

    VoC_endloop0

    // swDtxMuting = 0
    // swDtxBfiCnt = 0
    // swRxDTXState = CNINTPER - 1

    // calculate swRxGsHistPtr*2
    // DEBUG: add when debugging
    VoC_sub16_rr(REG1,REG1,REG0,DEFAULT);
    // DEBUG: change from REG4 to REG7
    VoC_sw16_d(REG7,GLOBAL_DE_swDtxBfiCnt_ADDR);
    VoC_sw16_d(REG7,GLOBAL_DE_swDtxMuting_ADDR);
    VoC_sw16_d(REG6,GLOBAL_DTX_DE_swRxDTXState_ADDR);
    // save swRxGsHistPtr*2
    // DEBUG: add when debugging
    VoC_sw16_d(REG1,GLOBAL_DE_swRxGsHistPtr_ADDR);
    VoC_jump(DEC_NOT_SPEECH_MODE_END)


DEC_NOT_SPEECH_MODE100:

    // if (swDecoMode != SPEECH)

    // REG1 : swFrameType
    // REG2 : 1
    // REG3 : swDecoMode
    // REG5 : swTAF


    // STACK16              STACK32
    // ------------------------------------------
    // RA
    // pswDcdSpchFrm
    // swFrameType
    // swR0Index


    VoC_lw16i_set_inc(REG0,GLOBAL_DE_SP_pswVq_ADDR,1);

    // load pswParameters[0:3]
    VoC_lw32_d(REG67,STATIC_DE_pswSpeechPara_ADDR);
    VoC_lw32_d(ACC0,STATIC_DE_pswSpeechPara_ADDR+2);

    // store pswVq[0:2]
    // DEBUG: change from REG1 to REG0
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    // DEBUG: change from REG1 to REG0
    VoC_sw16inc_p(ACC0_LO,REG0,DEFAULT);
    // DEBUG: change from REG1 to REG0
    VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);

    // store swR0Index
    VoC_sw16_sd(REG6,0,DEFAULT);

    // &psrR0DecTbl[swR0Index * 2]
    VoC_shr16_ri(REG6,-1,IN_PARALLEL);
    VoC_lw16i(REG0,TABLE_SP_ROM_psrR0DecTbl_ADDR);
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);

    // 0
    VoC_lw16i_short(REG4,0,IN_PARALLEL);

    // REG0 : &psrR0DecTbl[swR0Index * 2]
    // REG1 : swFrameType
    // REG2 : 1
    // REG3 : swDecoMode
    // REG4 : 0
    // REG5 : swTAF
    // REG6 : swR0Index

    VoC_sw16_d(REG2,GLOBAL_DE_SP_swSi_ADDR);
    VoC_sw16_d(REG4,GLOBAL_DE_SP_swVoicingMode_ADDR);
    VoC_lw16_p(REG0,REG0,DEFAULT);


    // REG0 : psrR0DecTbl[swR0Index * 2]
    // REG1 : swFrameType
    // REG2 : 1
    // REG3 : swDecoMode
    // REG4 : 0
    // REG5 : swTAF
    // REG6 : swR0Index


    // if (swDecoMode == CNIFIRSTSID)
    VoC_be16_rd(DEC_NO_CNIFIRSTSID_100_10,REG3,CONST_2_ADDR)
    VoC_jump(DEC_NO_CNIFIRSTSID_100)
DEC_NO_CNIFIRSTSID_100_10:
    VoC_lw16i_short(REG2,11,DEFAULT);
    VoC_sw16_d(REG4,GLOBAL_DE_swDtxBfiCnt_ADDR);
    VoC_sw16_d(REG4,GLOBAL_DE_swDtxMuting_ADDR);
    VoC_sw16_d(REG2,GLOBAL_DTX_DE_swRxDTXState_ADDR);


    // REG0 : psrR0DecTbl[swR0Index * 2]
    // REG1 : swFrameType
    // REG2 : 11
    // REG3 : swDecoMode
    // REG4 : 0
    // REG5 : swTAF
    // REG6 : swR0Index


    // if (swFrameType == VALIDSID)
    VoC_bne16_rd(DEC_NO_CNIFIRSTSID_101,REG1,CONST_11_ADDR)


    // swR0NewCN = psrR0DecTbl[swR0Index * 2];
    VoC_sw16_d(REG0,GLOBAL_DE_SP_swR0NewCN_ADDR);

    // ***************************************
    // CII_lookupVq_opt
    // input : none
    // output : none
    // used regs : all 16_bit, RL7
    // ***************************************

    VoC_jal(CII_lookupVq_opt);
    VoC_jump(DEC_NO_CNIFIRSTSID_END)

DEC_NO_CNIFIRSTSID_101:

    // Note : only two cases are possible
    // for (swDecoMode == CNIFIRSTSID)
    // so the <else if> is replaced by <else>

    // swR0Index = swOldR0IndexDec
    // &psrR0DecTbl[swOldR0IndexDec * 2]
    VoC_lw16_d(REG6,GLOBAL_DE_swOldR0IndexDec_ADDR);
    VoC_lw16i(REG0,TABLE_SP_ROM_psrR0DecTbl_ADDR);
    VoC_sw16_sd(REG6,0,DEFAULT);
    VoC_shr16_ri(REG6,-1,IN_PARALLEL);
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);

    // swR0NewCN = psrR0DecTbl[swOldR0IndexDec * 2];
    VoC_lw16i_set_inc(REG2,DEC_pswFrmKs_ADDR,2);
    VoC_lw16_p(REG7,REG0,DEFAULT);
    VoC_lw16i_set_inc(REG0,GLOBAL_DE_pswOldFrmKsDec_ADDR,2);
    VoC_sw16_d(REG7,GLOBAL_DE_SP_swR0NewCN_ADDR);

    VoC_loop_i_short(5,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0, REG2, DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_jump(DEC_NO_CNIFIRSTSID_END)

DEC_NO_CNIFIRSTSID_100:

    // REG0 : psrR0DecTbl[swR0Index * 2]
    // REG1 : swFrameType
    // REG2 : 1
    // REG3 : swDecoMode
    // REG4 : 0
    // REG5 : swTAF
    // REG6 : swR0Index

    // else if (swDecoMode == CNICONT)
    VoC_bne16_rd(DEC_NO_CNICONT_200,REG3,CONST_3_ADDR)

    VoC_sw16_d(REG4,GLOBAL_DE_swDtxBfiCnt_ADDR);
    VoC_sw16_d(REG4,GLOBAL_DE_swDtxMuting_ADDR);

    // if (swFrameType == VALIDSID)
    VoC_bne16_rd(DEC_NO_CNIFIRSTSID_201,REG1,CONST_11_ADDR)

    // swRxDTXState = 0;
    // DEBUG: change REG2 to REG4
    VoC_sw16_d(REG4,GLOBAL_DTX_DE_swRxDTXState_ADDR);

    // swR0NewCN = psrR0DecTbl[swR0Index * 2];
    VoC_sw16_d(REG0,GLOBAL_DE_SP_swR0NewCN_ADDR);

    // ***************************************
    // CII_lookupVq_opt
    // input : none
    // output : none
    // used regs : all 16_bit, RL7
    // ***************************************

    VoC_jal(CII_lookupVq_opt);
    VoC_jump(DEC_NO_CNIFIRSTSID_END);
DEC_NO_CNIFIRSTSID_201:
DEC_NO_CNICONT_200:

    // Note1 : case (swDecoMode == CNIBFI) and
    // case (swFrameType == INVALIDSID), (swDecoMode == CNICONT)
    // have the same treatment...

    // Note2 : only two cases are possible
    // for (swDecoMode == CNIFIRSTSID)
    // so the <else if> is replaced by <else>

    VoC_lw16i_short(REG7,11,DEFAULT);

    // swR0Index = swOldR0IndexDec;
    VoC_lw16_d(REG6,GLOBAL_DE_swOldR0IndexDec_ADDR);
    VoC_lw16_d(REG0,GLOBAL_DTX_DE_swRxDTXState_ADDR);

    // if (swRxDTXState < (CNINTPER - 1))
    VoC_bngt16_rr(DEC_NO_CNICONT_203,REG7,REG0);
    // swRxDTXState = add(swRxDTXState, 1);
    // DEBUG: change from VoC_add16_rd to VoC_add16_rr
    VoC_add16_rr(REG0,REG0,REG2,DEFAULT);
DEC_NO_CNICONT_203:
    VoC_sw16_sd(REG6,0,DEFAULT);
    VoC_sw16_d(REG0,GLOBAL_DTX_DE_swRxDTXState_ADDR);


    // else  if (swDecoMode == CNIBFI), continue...
    VoC_be16_rd(DEC_NO_CNICONT_204,REG3,CONST_4_ADDR)
    VoC_jump(DEC_NO_CNICONT_END)
DEC_NO_CNICONT_204:


    // REG0 : swRxDTXState
    // REG1 : swFrameType
    // REG2 : 1
    // REG3 : swDecoMode
    // REG4 : 0
    // REG5 : swTAF
    // REG6 : swR0Index = swOldR0IndexDec
    // REG7 : 11

    // if (swDtxMuting == 1)
    VoC_bne16_rd(DEC_NO_CNIFIRSTSID_302,REG2,GLOBAL_DE_swDtxMuting_ADDR)
    // swOldR0IndexDec = sub(swOldR0IndexDec, 2);
    VoC_sub16_rd(REG6,REG6,CONST_2_ADDR);
    // if (swOldR0IndexDec < 0) swOldR0IndexDec = 0;
    VoC_bnltz16_r(DEC_NO_CNIFIRSTSID_303, REG6)
    VoC_lw16i_short(REG6,0,DEFAULT);
DEC_NO_CNIFIRSTSID_303:
    // &psrR0DecTbl[swOldR0IndexDec * 2]
    VoC_add16_rr(REG7,REG6,REG6,DEFAULT);
    VoC_lw16i(REG0,TABLE_SP_ROM_psrR0DecTbl_ADDR);
    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);

    // swR0Index = swOldR0IndexDec
    VoC_sw16_d(REG6,GLOBAL_DE_swOldR0IndexDec_ADDR);

    // psrR0DecTbl[swOldR0IndexDec * 2]
    VoC_lw16_p(REG0,REG0,DEFAULT);

    // store swR0Index
    VoC_sw16_sd(REG6,0,DEFAULT);

    // swR0NewCN = psrR0DecTbl[swOldR0IndexDec * 2];
    VoC_sw16_d(REG0,GLOBAL_DE_SP_swR0NewCN_ADDR);

DEC_NO_CNIFIRSTSID_302:

    // swDtxBfiCnt = add(swDtxBfiCnt, 1);
    VoC_add16_rd(REG0,REG2,GLOBAL_DE_swDtxBfiCnt_ADDR);
    // if ((swTAF == 1) && (swDtxBfiCnt >= (2 * CNINTPER + 1)))
    VoC_bne16_rr(DEC_NO_CNIFIRSTSID_304,REG5,REG2)
    VoC_blt16_rd(DEC_NO_CNIFIRSTSID_304,REG0,CONST_25_ADDR)
    // swDtxMuting = 1
    VoC_sw16_d(REG2,GLOBAL_DE_swDtxMuting_ADDR);
DEC_NO_CNIFIRSTSID_304:

    VoC_sw16_d(REG0, GLOBAL_DE_swDtxBfiCnt_ADDR);

    // L_RxPNSeed
    VoC_lw32_d(ACC0,DEC_L_RxPNSeed_ADDR);

    // DEBUG: add when debugging
    VoC_jump(DEC_NO_FIRST_SID);

DEC_NO_CNIFIRSTSID_END:

DEC_NO_CNICONT_END:

    // STACK16              STACK32
    // ------------------------------------------
    // RA
    // pswDcdSpchFrm
    // swFrameType
    // swR0Index


    // swDecoMode
    VoC_lw16_d(REG6,GLOBAL_DE_swDecoMode_ADDR);
    // L_RxPNSeed
    VoC_lw32_d(ACC0,DEC_L_RxPNSeed_ADDR);

    // if (swDecoMode == CNIFIRSTSID)
    VoC_bne16_rd(DEC_NO_FIRST_SID,REG6,CONST_2_ADDR)

    VoC_lw16i_set_inc(REG0,GLOBAL_DE_pL_RxGsHist_ADDR,2);

    // ******************************************
    // CII_avgGsHistQntz_gsQuant_opt
    // input  REG0 : pL_GsHistory, INC0 = 2
    //         -   : swVoicingMode always 0
    // output REG2 : return
    // used reg : REG01245,ACC0,ACC1,RL6,RL7
    // ******************************************

    VoC_jal(CII_avgGsHistQntz_gsQuant_opt);

    // swRxDtxGsIndex <- return
    VoC_sw16_d(REG2,DEC_swRxDtxGsIndex_ADDR);

    // L_RxPNSeed = PN_INIT_SEED
//          VoC_lw32_d(ACC0,CONST_0x1091988L_ADDR);
    VoC_lw16i(ACC0_HI,0x109);
    VoC_lw16i(ACC0_LO,0x1988);


DEC_NO_FIRST_SID:

    // REG6 : swDecoMode
    // ACC0 : L_RxPNSeed
    VoC_lw16_d(REG6,GLOBAL_DE_swDecoMode_ADDR);
    // swRxDtxGsIndex
    VoC_lw16_d(REG7,DEC_swRxDtxGsIndex_ADDR);


    // &pswParameters[6]
    VoC_lw16i_set_inc(REG1,STATIC_DE_pswSpeechPara_ADDR+6,1);
    VoC_loop_i(1, 4)

    // ***********************************
    // CII_getPnBits_opt
    // input :   ACC0 -> L_RxPNSeed
    //                   iBits always 7
    // output :  REG2 -> return
    //           ACC0 -> L_RxPNSeed
    // used register : REG2, ACC0, RL6, RL7
    // ************************************

    VoC_jal(CII_getPnBits_opt);
    VoC_sw16inc_p(REG2,REG1,DEFAULT);
    VoC_jal(CII_getPnBits_opt);
    VoC_sw16inc_p(REG2,REG1,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);

    VoC_endloop1

    // store L_RxPNSeed
    VoC_sw32_d(ACC0,DEC_L_RxPNSeed_ADDR);

    // REG6 : swDecoMode
    // REG7 : swFrameType
    VoC_lw16_sd(REG7,1,DEFAULT);

    // pswOldKs
    VoC_lw16i(REG2,GLOBAL_DE_pswOldFrmKsDec_ADDR);

    // pswNewKs
    VoC_lw16i(RL6_HI,DEC_pswFrmKs_ADDR);

    // ***********************************
    // CII_rxInterpR0Lpc_opt
    // input : REG2 -> pswOldKs
    //         REG6 -> swDecoMode
    //         REG7 -> swFrameType
    //         RL6_HI -> pswNewKs
    // output : none
    // used regs : all
    // ***********************************

//      VoC_jal(CII_rxInterpR0Lpc_opt);

    // preload parameters
    // pL_CorrSeq
    VoC_lw16i(RL6_LO,STATIC_DTX_pL_CorrSeq_ADDR);

    // swR0NewCN
    VoC_lw16_d(REG3,GLOBAL_DE_SP_swR0NewCN_ADDR);

    VoC_push32(RL6,DEFAULT);
//  VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    // swOldR0Dec
    VoC_lw16_d(REG4,GLOBAL_DE_swOldR0Dec_ADDR);
    // swRxDTXState
    VoC_lw16_d(REG0,GLOBAL_DTX_DE_swRxDTXState_ADDR);


    // REG0 : swRxDTXState
    // REG2 : pswOldKs
    // REG3 : swR0NewCN
    // REG4 : swOldR0Dec
    // REG6 : swDecoMode
    // REG7 : swFrameType
    // RL6  : pL_CorrSeq & pswFrmKs


    // if (swDecoMode == CNIFIRSTSID)
    VoC_bne16_rd(RxInterpR0Lpc_opt_100,REG6,CONST_2_ADDR)


    // REG0 : swRxDTXState
    // REG2 : pswOldKs
    // REG3 : swR0NewCN
    // REG4 : swOldR0Dec
    // RL6  : pL_CorrSeq & pswFrmKs


    // swR0OldCN = swOldR0Dec
    VoC_sw16_d(REG4,STATIC_DTX_swR0OldCN_ADDR);


    // *************************
    // CII_linInterpSidShort_opt
    // input :  REG3 -> swNew
    //          REG4 -> swOld
    //          REG0 -> swDtxState
    //          INCR1=1
    // output : ACC0_HI -> return
    // used register : REG0134,ACC0
    // ********************

    VoC_jal(CII_linInterpSidShort_opt);

    // ********************************************
    // CII_rcToCorrDpL_opt
    //
    //
    // note :  1st two parameters are not passed, because
    //         they are always the same constants :
    //         swAshift = 4, swAscale = 0x800
    //
    // input:  REG4 : pswRc (param2), INC0 = 2
    //         REG1 : pL_R  (param3), INC1 = 2
    //
    // output: none
    //
    // ********************************************

    // pswOldKs
    VoC_movreg16(REG4,REG2,DEFAULT);
    VoC_lw16i_set_inc(REG1,STATIC_DTX_pL_OldCorrSeq_ADDR,2);

    // store swR0Dec
    VoC_sw16_d(ACC0_HI,GLOBAL_DE_SP_swR0Dec_ADDR);
    VoC_jal(CII_rcToCorrDpL_opt);
    VoC_jump(RxInterpR0Lpc_opt_same);

RxInterpR0Lpc_opt_100:

    // else if ((swDecoMode == CNICONT) && (swFrameType == VALIDSID))
    VoC_bne16_rd(RxInterpR0Lpc_opt_else,REG6,CONST_3_ADDR)
    VoC_bne16_rd(RxInterpR0Lpc_opt_else,REG7,CONST_11_ADDR)


    // REG0 : swRxDTXState
    // REG2 : pswOldKs
    // REG3 : swR0NewCN
    // REG4 : swOldR0Dec
    // RL6  : pL_CorrSeq & pswFrmKs

    // swR0OldCN = swOldR0Dec
    VoC_sw16_d(REG4,STATIC_DTX_swR0OldCN_ADDR);

    // ****************************
    // CII_linInterpSidShort_opt
    // input :  REG3 -> swNew
    //          REG4 -> swOld
    //          REG0 -> swDtxState
    // output : ACC0_HI -> return
    // used register : REG0134,ACC0
    // ****************************

    VoC_jal(CII_linInterpSidShort_opt);

    // REG2 : pswOldKs
    // REG5 : swR0OldCN
    // RL6  : pL_CorrSeq & pswFrmKs

    VoC_lw16i_set_inc(REG0,STATIC_DTX_pL_CorrSeq_ADDR,2);
    VoC_lw16i_set_inc(REG1,STATIC_DTX_pL_OldCorrSeq_ADDR,2);

    // swR0Dec
    VoC_sw16_d(ACC0_HI ,GLOBAL_DE_SP_swR0Dec_ADDR);

    VoC_loop_i_short(11,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0


RxInterpR0Lpc_opt_same:

    // pswNewKs
    VoC_lw32_sd(RL6,0,DEFAULT);
    VoC_movreg16(REG4,RL6_HI,DEFAULT);
    VoC_lw16i_set_inc(REG1,STATIC_DTX_pL_NewCorrSeq_ADDR,2);
    VoC_jal(CII_rcToCorrDpL_opt);

    // *******************************
    // CII_linInterpSidLoop_opt
    // input :  none
    // output : none
    // used register : REG012345,ACC01
    // *******************************

    VoC_jal(CII_linInterpSidLoop_opt);

    // *******************************
    // CII_aFlatRcDp_opt
    // input : RL6_LO -> pL_R
    //         RL6_HI -> pswRc
    // output : none
    // used regs : all
    // *******************************
    VoC_lw32_sd(RL6,0,DEFAULT);
    VoC_jal(CII_aFlatRcDp_opt);
    VoC_jump(RxInterpR0Lpc_opt_END);


RxInterpR0Lpc_opt_else:

    // else


    // ****************************
    // CII_linInterpSidShort_opt
    // input :  REG3 -> swNew
    //          REG4 -> swOld
    //          REG0 -> swDtxState
    // output : ACC0_HI -> return
    // used register : REG0134,ACC0
    // ****************************

    // swR0OldCN
    VoC_lw16_d(REG4,STATIC_DTX_swR0OldCN_ADDR);
    VoC_jal(CII_linInterpSidShort_opt);

    VoC_NOP();
    VoC_sw16_d(ACC0_HI,GLOBAL_DE_SP_swR0Dec_ADDR);

    // *******************************
    // CII_linInterpSidLoop_opt
    // input :  none
    // output : none
    // used register : REG012345,ACC01
    // *******************************

    VoC_jal(CII_linInterpSidLoop_opt);

    // *******************************
    // CII_aFlatRcDp_opt
    // input : RL6_LO -> pL_R
    //         RL6_HI -> pswRc
    // output : none
    // used regs : all
    // *******************************
    VoC_jal(CII_aFlatRcDp_opt);

RxInterpR0Lpc_opt_END:

//  VoC_pop16(RA, DEFAULT);
    VoC_pop32(RL6,DEFAULT);

DEC_NOT_SPEECH_MODE_END:

    // ------------------- //
    // do frame processing //
    // ------------------- //

    // ***********************************
    // CII_rcToADp_opt_new
    // input  : REG0 -> pswRc,   INC0 =  1
    //          REG1 -> pswA,    INC1 = -2
    //          0x800  swAscale is a const
    // output : REG7 <- return
    // ***********************************

    VoC_lw16i_set_inc(REG0,DEC_pswFrmKs_ADDR,1);
    VoC_lw16i_set_inc(REG1,DEC_pswFrmAs_ADDR,-2);
    VoC_jal(CII_rcToADp_opt_new);


    // if (!rcToADp(ASCALE, pswFrmKs, pswFrmAs))
    // DEBUG: change from VoC_bez to VoC_bnez
    VoC_bnez16_r(DEC_PROCESS_100,REG7)

    VoC_lw16i_set_inc(REG1,DEC_pswFrmAs_ADDR,1);
    VoC_lw16i_set_inc(REG2,TABLE_DE_SP_psrSPFDenomWidenCf_ADDR,1);
    VoC_lw16i_set_inc(REG3,DEC_pswFrmPFDenom_ADDR,1);

    VoC_movreg16(REG0,REG3,DEFAULT);
    VoC_multf32inc_pp(ACC0,REG1,REG2,IN_PARALLEL);

    VoC_lw32_d(ACC1,CONST_0x00008000L_ADDR);

    VoC_loop_i_short(10,DEFAULT);
    VoC_add32_rr(REG67,ACC0,ACC1,IN_PARALLEL);
    VoC_startloop0
    // DEBUG: change from REG0 to REG2
    VoC_multf32inc_pp(ACC0,REG2,REG1,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    exit_on_warnings = ON;
    VoC_add32_rr(REG67,ACC0,ACC1,DEFAULT);
    VoC_endloop0

    // ******************************************************
    // CII_a_sst_opt
    // input : REG0 -> pswDirectFormCoefIn (param2), INC0 = 1
    //         REG4 -> pswDirectFormCoefOut (param3)
    // note  : swAshift = 4, swAscale = 0x800 always
    // output : none
    // used regs : all
    // ******************************************************

    VoC_lw16i(REG4,DEC_pswFrmPFNum_ADDR);
    VoC_jal(CII_a_sst_opt);
    VoC_jump(DEC_PROCESS_100_END)

DEC_PROCESS_100:

    //   for (i = 0; i < NP; i++)
    //   {
    //      pswFrmKs[i] = pswOldFrmKsDec[i];
    //      pswFrmAs[i] = pswOldFrmAsDec[i];
    //      pswFrmPFDenom[i] = pswOldFrmPFDenom[i];
    //      pswFrmPFNum[i] = pswOldFrmPFNum[i];
    //   }

    // We use the following properties:
    // 1) Old are in RAM_X, New are in RAM_Y
    // 2) Ks, As, Num & Denom are consecutive
    // 3) copy 40 16-bit values <=> 20 32-bit values

    VoC_lw16i_set_inc(REG0,GLOBAL_DE_pswOldFrmKsDec_ADDR,2);
    VoC_lw16i_set_inc(REG1,DEC_pswFrmKs_ADDR,2);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_loop_i_short(10,DEFAULT);
    VoC_lw32inc_p(ACC1,REG0,IN_PARALLEL);
    VoC_startloop0

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,IN_PARALLEL);
    VoC_lw32inc_p(ACC1,REG0,DEFAULT);
    VoC_sw32inc_p(ACC1,REG1,IN_PARALLEL);

    VoC_endloop0


DEC_PROCESS_100_END:


    // **********************************************************
    //  CII_getSfrmLpcTx_opt
    //  IN:
    //      REG0    pswNewFrmAs     INC0=1
    //      REG1    psnsSqrtRs      INC1=1
    //      REG2    ppswSynthAs     INC2=1
    //      REG3    pswPrevFrmAs    INC3=1
    //      REG4    pswPrevFrmKs
    //      REG5    swPrevR0
    //      REG6    pswNewFrmKs
    //      REG7    swNewR0
    //      RL6_LO  pswNewFrmSNWCoef (pswNewFrmPFNum)
    //      RL6_HI  ppswSNWCoefAs(ppswPFNumAs)
    //      RL7_HI  psrOldCont
    //      RL7_LO  psrNewCont
    //      ACC0_LO pswPrevFrmSNWCoef (pswPrevFrmPFNum)
    //      ACC0_HI pswPrevFrmPFDenom
    //      ACC1_LO pswNewFrmPFDenom
    //      ACC1_HI ppswPFDenomAs
    //  FLAG_GETSFRMLPC_TX_ADDR :
    //          1 : CII_getSfrmLpcTx
    //          0 : CII_getSfrmLpc
    // **********************************************************

    // decoder flag
    VoC_lw16i_short(REG5,0,DEFAULT);

    // prepare pointers :
    VoC_lw16i_set_inc(REG0,DEC_pswFrmAs_ADDR,1);
    VoC_lw16i_set_inc(REG1,DEC_psnsSqrtRs_ADDR,1);
    VoC_lw16i_set_inc(REG2,DEC_pswSythAsSpace_ADDR,1);
    VoC_lw16i_set_inc(REG3,GLOBAL_DE_pswOldFrmAsDec_ADDR,1);

    // GETSFRMLPC_flag = 0  (decoder)
    VoC_sw16_d(REG5,FLAG_GETSFRMLPC_TX_ADDR);

    VoC_lw16i(REG4,GLOBAL_DE_pswOldFrmKsDec_ADDR);
    VoC_lw16_d(REG5,GLOBAL_DE_swOldR0Dec_ADDR);
    VoC_lw16i(REG6,DEC_pswFrmKs_ADDR);
    VoC_lw16_d(REG7,GLOBAL_DE_SP_swR0Dec_ADDR);

    VoC_lw16i(ACC0_LO,GLOBAL_DE_pswOldFrmPFNum_ADDR);
    VoC_lw16i(ACC0_HI,GLOBAL_DE_pswOldFrmPFDenom_ADDR);
    VoC_lw16i(ACC1_HI,DEC_pswPFDenomAsSpace_ADDR);
    VoC_lw16i(ACC1_LO,DEC_pswFrmPFDenom_ADDR);

    VoC_lw16i(RL6_LO,DEC_pswFrmPFNum_ADDR);
    VoC_lw16i(RL6_HI,DEC_pswPFNumAsSpace_ADDR);
    VoC_lw16i(RL7_LO,TABLE_SP_ROM_psrNewCont_ADDR);
    VoC_lw16i(RL7_HI,TABLE_SP_ROM_psrOldCont_ADDR);

    VoC_jal(CII_getSfrmLpcTx_opt);


    // swR0Index
    VoC_lw16_sd(REG4,0,DEFAULT);

    // STACK16              STACK32
    // ------------------------------------------
    // RA
    // pswDcdSpchFrm
    // swFrameType
    // swR0Index

    // *******************************
    // CII_r0BasedEnergyShft_new
    // input  REG4 : swR0Index
    // output REG5 : swShiftDownSignal
    // used register : REG45
    // *******************************

    VoC_jal(CII_r0BasedEnergyShft_new);

    // giSfrmCnt = 0
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_short(REG6,3,DEFAULT);
    VoC_sw16_d(REG5,GLOBAL_DE_SP_swEngyRShift_ADDR);

    // for (giSfrmCnt = 0; giSfrmCnt < 4; giSfrmCnt++)

DEC_giSfrmCnt_LOOP_BEGIN:

    VoC_bgt16_rr(DEC_giSfrmCnt_LOOP,REG7,REG6)

    // &pswParameters[(giSfrmCnt * 3) + 6]

    VoC_mult16_rr(REG5,REG7,REG6,DEFAULT);
    VoC_lw16i_set_inc(REG2,STATIC_DE_pswSpeechPara_ADDR+6,1);
    VoC_add16_rr(REG2,REG5,REG2,DEFAULT);

    // store global variable
    VoC_sw16_d(REG7,GLOBAL_DE_giSfrmCnt_ADDR);

    // load parameters
    VoC_lw16inc_p(REG0,REG2,DEFAULT);
    VoC_lw16inc_p(REG1,REG2,DEFAULT);
    VoC_lw16inc_p(REG3,REG2,DEFAULT);

    // unvoiced
    VoC_bnez16_d(DEC_PROCESS_200,GLOBAL_DE_SP_swVoicingMode_ADDR);

    // psiVselpCw[0:1] = pswParameters[(giSfrmCnt * 3) + 6:7]
    VoC_sw32_d(REG01,DEC_psiVselpCw_ADDR);
    VoC_jump(DEC_PROCESS_200_END);
DEC_PROCESS_200:

    // voiced

    // psiVselpCw[0] = pswParameters[(giSfrmCnt * 3) + 7];
    VoC_sw16_d(REG1,DEC_psiVselpCw_ADDR);

DEC_PROCESS_200_END:

    // unvoiced : swLag = 0
    VoC_lw16i_short(REG5,0,DEFAULT);

    // siGsp0Code = pswParameters[(giSfrmCnt * 3) + 8]
    VoC_sw16_d(REG3,DEC_siGsp0Code_ADDR);

    // push swLag
    VoC_push16(REG5,DEFAULT);

    // STACK16              STACK32
    // ------------------------------------------
    // RA
    // pswDcdSpchFrm
    // swFrameType
    // swR0Index
    // swLag

    VoC_bez16_d(DEC_PROCESS_300,GLOBAL_DE_SP_swVoicingMode_ADDR)

    // voiced

    // ***************************
    // inline CII_lagDecode_opt
    // input  : REG0 -> siLagCode
    // output : REG5 -> swLag
    // used register : REG01
    // ***************************

    // REG0 : siLagCode (swDeltaLag)
    // if (giSfrmCnt == 0), swLastLag in REG0
    VoC_bez16_d(LagDecode_opt_END,GLOBAL_DE_giSfrmCnt_ADDR)

    // else
    // swLag = sub(swDeltaLag, DELTA_LEVELS_D2)
    VoC_sub16_rd(REG0,REG0,CONST_8_ADDR);
    // swLag = add(swLag, swLastLag);
    VoC_add16_rd(REG0,REG0,STATIC_DE_SP_swLastLag_ADDR);

    // if (sub(swLag, MAX_LAG) > 0)
    VoC_bngt16_rd(LagDecode_opt_200,REG0,CONST_0x00ff_ADDR)
    // swLastLag = MAX_LAG
    VoC_lw16i(REG0,0x00ff);
LagDecode_opt_200:

    // else if (sub(swLag, MIN_LAG) < 0)
    VoC_bnltz16_r(LagDecode_opt_201,REG1);
    //  swLastLag = MIN_LAG;
    VoC_lw16i_short(REG0,0,DEFAULT);
LagDecode_opt_201:
    // else swLastLag = swLag;
LagDecode_opt_END:
    // else swLastLag = swDeltaLag

    // &psrLagTbl[swLastLag]
    VoC_lw16i(REG1,TABLE_SP_ROM_psrLagTbl_ADDR);
    VoC_add16_rr(REG1,REG1,REG0,DEFAULT);

    // store static swLastLag
    VoC_sw16_d(REG0,STATIC_DE_SP_swLastLag_ADDR);

    // swLag = psrLagTbl[swLastLag]
    // DEBUG: inverse REG5 & REG6
    VoC_lw16_p(REG6,REG1,DEFAULT);
    VoC_lw16i(REG5,GLOBAL_DE_pswLtpStateBaseDec_ADDR+147);

    // **********************
    // CII_fp_ex_opt
    // pswLTPState in REG5
    // swOrigLagIn in REG6
    // used : all 16-bit
    // unchanged : REG5
    // **********************

    VoC_jal(CII_fp_ex_opt);

    VoC_lw16i_set_inc(REG0,GLOBAL_DE_pswLtpStateBaseDec_ADDR+147,2);
    VoC_lw16i_set_inc(REG1,DEC_pswPVec_ADDR,2);

    VoC_aligninc_pp(REG0,REG0,DEFAULT);
    VoC_loop_i_short(20,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    // save swLag
    // DEBUG: change REG5 to REG6, change VoC_lw16 to VoC_sw16
    VoC_sw16_sd(REG6,0,DEFAULT);


    // REG5 : swLag
    // Continue with voiced...

    // siNumBits = C_BITS_V;
    VoC_lw16i_short(REG4,9,DEFAULT);
    VoC_lw16_d(REG5,DEC_psiVselpCw_ADDR);
    VoC_lw16i_set_inc(REG0,DEC_pswBitArray_ADDR,1);

    // ***********************************
    // CII_b_con_opt
    // input : REG0 -> pswVectOut, INC = 1
    //         REG4 -> siNumBits
    //         REG5 -> swCodeWord
    // output : none
    // used : all 16-bit except REG1
    // unchanged : REG4
    // ***********************************
    // DEBUG: add following instruction
    VoC_jal(CII_b_con_opt);

    VoC_lw16i_set_inc(REG0,TABLE_SP_ROM_pppsrVcdCodeVec_ADDR,40);
    VoC_lw16i_set_inc(REG1, DEC_ppswVselpEx_ADDR,2);
    VoC_lw16i_set_inc(REG2, DEC_pswBitArray_ADDR,1);

    // ***********************************
    // CII_v_con_opt
    // input : REG0 -> pswBVects,   INC0 = 40
    //         REG1 -> pswOutVect,  INC1 = 2
    //         REG2 -> pswBitArray, INC2 = 1
    //         REG4 -> siNumBVctrs
    // output : none
    // used : all except REG7 & RL7
    // unchanged : REG4
    // ***********************************

    VoC_jal(CII_v_con_opt);
    VoC_jump(DEC_PROCESS_400)

DEC_PROCESS_300:

    // unvoiced .....

    // unfold loop... saves 22 byte

    // siCodeBook == 0

    // siNumBits = C_BITS_UV;
    VoC_lw16i_short(REG4,7,DEFAULT);
    VoC_lw16_d(REG5,DEC_psiVselpCw_ADDR);
    VoC_lw16i_set_inc(REG0,DEC_pswBitArray_ADDR,1);

    // ***********************************
    // CII_b_con_opt
    // input : REG0 -> pswVectOut, INC = 1
    //         REG4 -> siNumBits
    //         REG5 -> swCodeWord
    // output : none
    // used : all 16-bit except REG1
    // unchanged : REG4
    // ***********************************
    // DEBUG: add following instruction
    VoC_jal(CII_b_con_opt);

    VoC_lw16i_set_inc(REG0,TABLE_SP_ROM_pppsrUvCodeVec_ADDR,40);
    VoC_lw16i_set_inc(REG1,DEC_ppswVselpEx_ADDR,2);
    VoC_lw16i_set_inc(REG2,DEC_pswBitArray_ADDR,1);

    // ***********************************
    // CII_v_con_opt
    // input : REG0 -> pswBVects,   INC0 = 40
    //         REG1 -> pswOutVect,  INC1 = 2
    //         REG2 -> pswBitArray, INC2 = 1
    //         REG4 -> siNumBVctrs
    // output : none
    // used : all except REG7 & RL7
    // unchanged : REG4
    // ***********************************

    VoC_jal(CII_v_con_opt);

    // siCodeBook == 1

    // siNumBits = C_BITS_UV;
    VoC_lw16_d(REG5,DEC_psiVselpCw_ADDR+1);
    VoC_lw16i_set_inc(REG0,DEC_pswBitArray_ADDR,1);

    // ***********************************
    // CII_b_con_opt
    // input : REG0 -> pswVectOut, INC = 1
    //         REG4 -> siNumBits
    //         REG5 -> swCodeWord
    // output : none
    // used : all 16-bit except REG1
    // unchanged : REG4
    // ***********************************
    // DEBUG: add following instruction
    VoC_jal(CII_b_con_opt);

    VoC_lw16i_set_inc(REG0,TABLE_SP_ROM_pppsrUvCodeVec_ADDR+7*40,40);
    VoC_lw16i_set_inc(REG1,DEC_ppswVselpEx_ADDR+40,2);
    VoC_lw16i_set_inc(REG2,DEC_pswBitArray_ADDR,1);

    // ***********************************
    // CII_v_con_opt
    // input : REG0 -> pswBVects,   INC0 = 40
    //         REG1 -> pswOutVect,  INC1 = 2
    //         REG2 -> pswBitArray, INC2 = 1
    //         REG4 -> siNumBVctrs
    // output : none
    // used : all except REG7 & RL7
    // unchanged : REG4
    // ***********************************

    VoC_jal(CII_v_con_opt);

DEC_PROCESS_400:

    // swLag
    VoC_lw16_sd(REG7,0,DEFAULT);

    // preload swSemiBeta = 0
    VoC_lw16i_short(REG5,0,DEFAULT);

    // psnsSqrtRs[giSfrmCnt]
    VoC_lw16i_set_inc(REG2,DEC_psnsSqrtRs_ADDR,1);
    VoC_add16_rd(REG2,REG2,GLOBAL_DE_giSfrmCnt_ADDR);
    VoC_add16_rd(REG2,REG2,GLOBAL_DE_giSfrmCnt_ADDR);

    VoC_lw16i_set_inc(REG0,DEC_pswPVec_ADDR,2);
    VoC_lw16i_set_inc(REG3,DEC_snsRs00_ADDR,1);

    // push psnsSqrtRs[giSfrmCnt] & swSemiBeta
    VoC_push16(REG5,DEFAULT);
    VoC_push16(REG2,DEFAULT);

    // STACK16              STACK32
    // ------------------------------------------
    // RA
    // pswDcdSpchFrm
    // swFrameType
    // swR0Index
    // swLag
    // swSemiBeta
    // psnsSqrtRs[giSfrmCnt]

    // if (swLag)
    VoC_bez16_r(DEC_PROCESS_500,REG7)

    // ******************************************
    // CII_rs_rr_opt2
    // input  :  REG0 -> pswExcitation, INC0 = 2
    //           REG2 -> psnsSqrtRsRr,  INC2 = 1
    //           REG3 -> &snsSqrtRs,    INC3 = 1
    // output :  none
    // used register : all
    // ******************************************

//      BEGIN OF CII_rs_rr_opt
    VoC_push32(REG23,DEFAULT);

    VoC_lw32_p(REG23,REG2,DEFAULT);
    VoC_shr16_ri(REG3,-1,DEFAULT);

    VoC_sub16_dr(REG5,CONST_3_ADDR,REG3);
    //  if (-swEnergy > 0)
    VoC_bngtz16_r(RS_RR_opt_100,REG5)
    // swNormShift = g_corr1s(pswExcitation, negate(swEnergy), &L_Temp);

    // *************************************
    // CII_g_corr1s_new
    // input:   REG0 -> pswIn,  INC0 = 2
    //          REG5 -> swEngyRShft
    // output : REG7 -> swEngyLShft
    //          ACC0 -> *pL_out
    // used : all except REG23,ACC1,RL6,RL7
    // *************************************

    VoC_jal(CII_g_corr1s_new);
    VoC_movreg16(REG1,REG7,DEFAULT);
    VoC_jump(RS_RR_opt_100_END)

RS_RR_opt_100:

    // else

    // swNormShift = g_corr1(pswExcitation, &L_Temp);

    // *************************************
    // CII_g_corr1_opt_paris
    // input:   REG0 -> pswIn,  INC0 = 2
    // output : REG1 -> swEngyLShft
    //          ACC0 -> *pL_out
    // *************************************

    VoC_jal(CII_g_corr1_opt_paris);

RS_RR_opt_100_END:

    // return swNormShift in reg1
    // return L_temp in ACC0

    VoC_jal(CII_sub_rs_rrNs);
// END OF CII_rs_rr_opt

DEC_PROCESS_500:

    // &pppsrGsp0[swVoicingMode][siGsp0Code][0:1]
    VoC_lw16i_short(REG3,5,DEFAULT);

    VoC_lw16_d(REG4,GLOBAL_DE_SP_swVoicingMode_ADDR);
    VoC_lw16i_set_inc(REG1,TABLE_SP_ROM_pppsrGsp0_ADDR,1);
    VoC_mult16_rd(REG3,REG3,DEC_siGsp0Code_ADDR);
    VoC_mult16_rd(REG5,REG4,CONST_160_ADDR);
    VoC_add16_rr(REG1,REG1,REG3,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG5,DEFAULT);

    // ******************************************
    // CII_rs_rrNs_opt
    // input  :  REG0 -> pswExcitation, INC0 = 2
    //           REG2 -> psnsSqrtRsRr,  INC2 = 1
    //           REG3 -> &snsSqrtRs,    INC3 = 1
    // output :  none
    // used register : all
    // ******************************************

    // ppswVselpEx
    VoC_lw16i_set_inc(REG0,DEC_ppswVselpEx_ADDR,2);

    // pppsrGsp0[swVoicingMode][siGsp0Code][0:1]
    VoC_lw16inc_p(ACC0_LO,REG1,DEFAULT);
    // DEBUG: change from VoC_lw32 to VoC_lw16 (REG1 will be odd)
    VoC_lw16inc_p(ACC0_HI,REG1,DEFAULT);

    // &snsRs11
    VoC_lw16i_set_inc(REG3,DEC_snsRs11_ADDR,1);

    // psnsSqrtRs[giSfrmCnt]
    VoC_lw16_sd(REG2,0,DEFAULT);

    // push pppsrGsp0[swVoicingMode][siGsp0Code][0:1]
    VoC_push16(ACC0_HI,DEFAULT);
    VoC_push16(ACC0_LO,DEFAULT);


    // STACK16              STACK32
    // ------------------------------------------
    // RA
    // pswDcdSpchFrm
    // swFrameType
    // swR0Index
    // swLag
    // swSemiBeta
    // psnsSqrtRs[giSfrmCnt]
    // pppsrGsp0[swVoicingMode][siGsp0Code][1]
    // pppsrGsp0[swVoicingMode][siGsp0Code][0]


    VoC_jal(CII_rs_rrNs_opt);

    // DEBUG: add instruction ( REG4 will change in CII_rs_rrNs)
    VoC_lw16_d(REG4,GLOBAL_DE_SP_swVoicingMode_ADDR);
    // if (!swVoicingMode)
    VoC_bnez16_r(DEC_PROCESS_600,REG4)

    // ******************************************
    // CII_rs_rrNs_opt
    // input  :  REG0 -> pswExcitation, INC0 = 2
    //           REG2 -> psnsSqrtRsRr,  INC2 = 1
    //           REG3 -> &snsSqrtRs,    INC3 = 1
    // output :  none
    // used register : all
    // ******************************************

    // psnsSqrtRs[giSfrmCnt]
    VoC_lw16_sd(REG2,2,DEFAULT);

    VoC_lw16i_set_inc(REG0,DEC_ppswVselpEx_ADDR+40,2);
    VoC_lw16i_set_inc(REG3,DEC_snsRs22_ADDR,1);
    VoC_jal(CII_rs_rrNs_opt);

    // continue with unvoiced ...
    // preloaded swSemiBeta = 0 on stack

    // ***********************************************
    // CII_scaleExcite_opt
    // input  : REG0 -> pswVect (param0),     INC0 = 1
    //          REG1 -> pswScldVect (param3), INC1 = 1
    //          REG4 -> swErrTerm (param1)
    //          REG7 -> &snsRS (param2)
    // output : REG3 <- return
    // used regs: all
    // ***********************************************

    VoC_pop16(REG4,DEFAULT);
    // DEBUG: add when debuggig

    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_lw16i_set_inc(REG0,DEC_ppswVselpEx_ADDR,1);
    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_lw16i(REG7,DEC_snsRs11_ADDR);
    VoC_jal(CII_scaleExcite_opt);

    VoC_pop16(REG4,DEFAULT);
    VoC_lw16i_set_inc(REG0,DEC_ppswVselpEx_ADDR+40,1);
    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_lw16i(REG7,DEC_snsRs22_ADDR);
    VoC_jal(CII_scaleExcite_opt);

    // prepare pointers for pswExcite computation
    VoC_lw16i_set_inc(REG0,DEC_ppswVselpEx_ADDR+40,2);
    VoC_lw16i_set_inc(REG1,DEC_ppswVselpEx_ADDR,2);

    VoC_jump(DEC_PROCESS_700);

DEC_PROCESS_600:

    // if (swVoicingMode)

    // ***********************************************
    // CII_scaleExcite_opt
    // input  : REG0 -> pswVect (param0),     INC0 = 1
    //          REG1 -> pswScldVect (param3), INC1 = 1
    //          REG4 -> swErrTerm (param1)
    //          REG7 -> &snsRS (param2)
    // output : REG3 <- return
    // used regs: all
    // ***********************************************

    VoC_pop16(REG4,DEFAULT);
    // DEBUG: add when debuggig

    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_lw16i_set_inc(REG0,DEC_pswPVec_ADDR,1);
    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_lw16i(REG7,DEC_snsRs00_ADDR);
    VoC_jal(CII_scaleExcite_opt);

    VoC_pop16(REG4,DEFAULT);
    // DEBUG: add when debugging
    VoC_sw16_sd(REG3,1,DEFAULT);
    VoC_lw16i_set_inc(REG0,DEC_ppswVselpEx_ADDR,1);
    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_lw16i(REG7,DEC_snsRs11_ADDR);
    VoC_jal(CII_scaleExcite_opt);

    // prepare pointers for pswExcite computation
    VoC_lw16i_set_inc(REG0,DEC_pswPVec_ADDR,2);
    VoC_lw16i_set_inc(REG1,DEC_ppswVselpEx_ADDR,2);



DEC_PROCESS_700:

    // STACK16              STACK32
    // ------------------------------------------
    // RA
    // pswDcdSpchFrm
    // swFrameType
    // swR0Index
    // swLag
    // swSemiBeta
    // psnsSqrtRs[giSfrmCnt]


    // pswExcite computation
    VoC_lw16i_set_inc(REG2,DEC_pswExcite_ADDR,2);

    //VoC_lw16inc_p(REG4,REG0,DEFAULT);
    //VoC_lw16inc_p(REG5,REG0,IN_PARALLEL);

    VoC_loop_i_short(20,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);
    VoC_startloop0

    VoC_add16inc_rp(REG4,REG4,REG1,DEFAULT);
    // DEBUG: change from REG4 to REG5
    VoC_add16inc_rp(REG5,REG5,REG1,IN_PARALLEL);

    // VoC_lw16inc_p(REG4,REG0,DEFAULT);
    // VoC_lw16inc_p(REG5,REG0,IN_PARALLEL);
    VoC_lw32inc_p(REG45,REG0,DEFAULT);

    exit_on_warnings=OFF;
    VoC_sw32inc_p(REG45,REG2,DEFAULT);
    exit_on_warnings=ON;

    VoC_endloop0


    // shift pswLtpStateBaseDec
    VoC_lw16i_set_inc(REG0,GLOBAL_DE_pswLtpStateBaseDec_ADDR+40,2);
    VoC_lw16i_set_inc(REG1,GLOBAL_DE_pswLtpStateBaseDec_ADDR,2);

    // only LTP_LEN - S_LEN values need to be copied
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_loop_i_short(53,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop0

    // REG1 : pswLtpStateOut - S_LEN - 1

    VoC_lw16i_short(INC1,1,DEFAULT);
    // add the current sub-frames data to the state
    VoC_lw16i_set_inc(REG0,DEC_pswExcite_ADDR,2);

    // last write ...
    VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);

    VoC_loop_i_short(20,DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(ACC0_LO,REG1,IN_PARALLEL);
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop0


    // STACK16              STACK32
    // ------------------------------------------
    // RA
    // pswDcdSpchFrm
    // swFrameType
    // swR0Index
    // swLag
    // swSemiBeta
    // psnsSqrtRs[giSfrmCnt]



    // ***********************************************
    // inline CII_pitchPreFilt_opt
    // input : REG0 -> pswExcite (param0),    INC0 = 1
    //         REG1 -> pswExciteOut (param6), INC1 = 1
    //         REG2 -> &pswPPreState[0],      INC2 = 1
    //         REG3 -> &pswPPreState[40],     INC3 = 1
    //         REG7 -> snsSqrtRs.sh (param5)
    //         ACC1_LO -> swSemiBeta (param4)
    //         ACC1_HI -> swRxLag (param2)
    // not passed or in stack:
    //         swRxGsp0 (param1)
    //     swUvCode (param3)
    // output : none
    // used regs : all
    // note : local buffer pswInterpCoefs
    //        should be placed in RAM_X !!!
    // ***********************************************

    {
#if 0

        voc_short LOCAL_pitchPreFilt_pswInterpCoefs_ADDR         ,10,y         //10

#endif


        // psnsSqrtRs[giSfrmCnt]
        VoC_pop16(REG7,DEFAULT);
        VoC_pop16(ACC1_LO,DEFAULT);
        VoC_pop16(ACC1_HI,DEFAULT);

        // DEBUG: add following instruction psnsSqrtRs[giSfrmCnt].sh
        VoC_add16_rd(REG7,REG7,CONST_1_ADDR);
        // &pswPPreState[0]
        VoC_lw16i_set_inc(REG2,GLOBAL_DE_pswPPreState_ADDR,1);
        // &pswPPreState[40]
        VoC_lw16i_set_inc(REG3,GLOBAL_DE_pswPPreState_ADDR+40,1);
        // snsSqrtRs.sh
        VoC_lw16_p(REG7,REG7,DEFAULT);
        // pswPPFExcit (pswExciteOut)
        VoC_lw16i_set_inc(REG1,DEC_pswPPFExcit_ADDR,1);
        // pswExcite
        VoC_lw16i_set_inc(REG0,DEC_pswExcite_ADDR,1);


        // STACK16              STACK32
        // ------------------------------------------
        // RA
        // pswDcdSpchFrm
        // swFrameType
        // swR0Index


        // if (swUvCode == 0)
        VoC_bnez16_d(pitchPreFilt_opt_00,GLOBAL_DE_SP_swVoicingMode_ADDR);


        // REG0 : pswExcite,         INC0 = 1
        // REG1 : pswExciteOut,      INC1 = 1
        // REG2 : &pswPPreState[0],  INC2 = 1
        // REG3 : &pswPPreState[40], INC3 = 1
        // ACC1 : swSemiBeta & swLag


        // pswPPreState[i] = pswPPreState[i + S_LEN];
        VoC_lw16inc_p(REG6,REG3,DEFAULT);
        VoC_loop_i(0,107)
        VoC_lw16inc_p(REG6,REG3,DEFAULT);
        exit_on_warnings=OFF;
        VoC_sw16inc_p(REG6,REG2,DEFAULT);
        exit_on_warnings=ON;
        VoC_endloop0

        // if (pswExciteOut != pswExcite)
        // DEBUG: delete the following instruction
        // DEBUG: the first VoC_sw16inc_p in loop should not be branched
        // VoC_be16_rr(pitchPreFilt_opt_end,REG0,REG1);

        VoC_lw16inc_p(REG6,REG0,DEFAULT);
        VoC_loop_i_short(40,DEFAULT);
        VoC_startloop0
        VoC_lw16inc_p(REG6,REG0,DEFAULT);
        VoC_movreg16(REG5,REG6,IN_PARALLEL);
        exit_on_warnings=OFF;
        VoC_sw16inc_p(REG6,REG2,DEFAULT);
        exit_on_warnings=ON;

        VoC_sw16inc_p(REG5,REG1,DEFAULT);
        VoC_endloop0

        VoC_jump(pitchPreFilt_opt_end);

pitchPreFilt_opt_00:

        // else

        // REG0 : pswExcite,         INC0 = 1
        // REG1 : pswExciteOut,      INC1 = 1
        // REG2 : &pswPPreState[0],  INC2 = 1
        // REG3 : &pswPPreState[40], INC3 = 1
        // REG7 : snsSqrtRs.sh
        // ACC1 : swSemiBeta & swLag

        // swEnergy = add(negate(shl(snsSqrtRs.sh, 1)), 3);
        VoC_shr16_ri(REG7,-1,DEFAULT);
        VoC_sub16_dr(REG5,CONST_3_ADDR,REG7);


        VoC_lw16i_short(INC0,2,DEFAULT);
        VoC_push32(REG01,IN_PARALLEL);


        // REG0 : pswExcite,         INC0 = 2
        // REG1 : pswExciteOut,      INC1 = 1
        // REG2 : &pswPPreState[0],  INC2 = 1
        // REG3 : &pswPPreState[40], INC3 = 1
        // REG5 : swEnergy
        // ACC1 : swSemiBeta & swLag


        VoC_bngtz16_r(pitchPreFilt_opt_01,REG5);

        // *************************************
        // CII_g_corr1s_new
        // input:   REG0 -> pswIn,  INC0 = 2
        //          REG5 -> swEngyRShft
        // output : REG7 -> swEngyLShft
        //          ACC0 -> *pL_out
        // used : all except REG23,ACC1,RL6,RL7
        // *************************************

        // snsOrigEnergy.sh in REG7
        VoC_jal(CII_g_corr1s_new);
        VoC_jump(pitchPreFilt_opt_02);


pitchPreFilt_opt_01:

        // else

        VoC_lw16i_short(REG5,0,DEFAULT);

        // REG0 : pswExcite,         INC0 = 2
        // REG1 : pswExciteOut,      INC1 = 1
        // REG2 : &pswPPreState[0],  INC2 = 1
        // REG3 : &pswPPreState[40], INC3 = 1
        // REG5 : swEnergy = 0
        // ACC1 : swSemiBeta & swLag


        VoC_aligninc_pp(REG0,REG0,DEFAULT);
        VoC_loop_i_short(20,DEFAULT);
        VoC_lw32z(ACC0,IN_PARALLEL);
        VoC_startloop0
        VoC_bimac32inc_pp(REG0,REG0);
        VoC_endloop0

        // *****************************
        // CII_NORM_L_ACC0
        // input : ACC0
        // output : REG1
        // note : ACC0 <- (ACC0 >> REG1)
        // *****************************

        // snsOrigEnergy.sh = norm_l(L_OrigEnergy)
        // L_shl(L_OrigEnergy, snsOrigEnergy.sh)
        VoC_jal(CII_NORM_L_ACC0);
        VoC_movreg16(REG7,REG1,DEFAULT);
        VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
        // DEBUG: change from VoC_movreg16
        //      VoC_sub16_dr(REG7,CONST_0_ADDR,REG1);

pitchPreFilt_opt_02:

        // 0x8000
        VoC_lw32_d(RL6,CONST_0x00008000L_ADDR);

        // &ppsrSqrtP0[swUvCode-1][swRxGsp0]
        VoC_lw16i_set_inc(REG0,TABLE_SP_ROM_ppsrSqrtP0_ADDR-32,1);
        VoC_add16_rd(REG0,REG0,DEC_siGsp0Code_ADDR);
        VoC_lw16_d(REG6,GLOBAL_DE_SP_swVoicingMode_ADDR);
        VoC_shr16_ri(REG6,-5,DEFAULT);
        VoC_add16_rr(REG6,REG0,REG6,DEFAULT);

        // snsOrigEnergy.man in REG1
        VoC_add32_rr(REG01,ACC0,RL6,DEFAULT);

        // swSqrtP0
        VoC_lw16_p(REG0,REG6,DEFAULT);

        // snsOrigEnergy.man in REG6
        VoC_movreg16(REG6,REG1,IN_PARALLEL);

        // REG0 : swSqrtP0,          INC0 = 1
        // REG1 :                    INC1 = 1
        // REG2 : &pswPPreState[0],  INC2 = 1
        // REG3 : &pswPPreState[40], INC3 = 1
        // REG5 : swEnergy
        // REG6 : snsOrigEnergy.man
        // REG7 : snsOrigEnergy.sh
        // ACC1 : swSemiBeta & swLag
        // RL6 : 0x8000

        // swSemiBeta
        VoC_movreg16(REG1,ACC1_LO,DEFAULT);
        VoC_bngt16_rr(pitchPreFilt_opt_03,REG0,REG1);
        // swScale = swSemiBeta;
        VoC_movreg16(REG0,REG1,DEFAULT);
pitchPreFilt_opt_03:


        // REG0 : swScale,           INC0 = 1
        // REG1 :                    INC1 = 1
        // REG2 : &pswPPreState[0],  INC2 = 1
        // REG3 : &pswPPreState[40], INC3 = 1
        // REG5 : swEnergy
        // REG6 : snsOrigEnergy.man
        // REG7 : snsOrigEnergy.sh
        // RL6 : 0x8000

        // save snsOrigEnergy in RL7
        VoC_movreg32(RL7,REG67,DEFAULT);

        VoC_multf32_rd(REG67,REG0,CONST_0x2666_ADDR);
        // swLag
        VoC_movreg16(REG4,ACC1_HI,DEFAULT);

        VoC_add32_rr(REG67,REG67,RL6,DEFAULT);

        // REG0 :                    INC0 = 1
        // REG1 :                    INC1 = 1
        // REG2 : &pswPPreState[0],  INC2 = 1
        // REG3 : &pswPPreState[40], INC3 = 1
        // REG5 : swEnergy
        // REG6 :
        // REG7 : swScale
        // ACC1 : swSemiBeta & swLag
        // RL6 : 0x8000

        // *****************************
        // CII_get_ipjj_opt_paris
        // input  : REG4 -> swRunningLag
        // output : REG0 -> TempJJ (remain)
        //          REG1?> TempIp (IntLag)
        // other used : ACCO
        // *****************************


        VoC_jal(CII_get_ipjj_opt_paris);


        VoC_lw16i_set_inc(REG3,TABLE_SP_ROM_ppsrPVecIntFilt_ADDR,6);
        VoC_add16_rr(REG3,REG3,REG0,DEFAULT);
        VoC_lw16i_set_inc(REG0,LOCAL_pitchPreFilt_pswInterpCoefs_ADDR,1);


        // REG0 : &ppsrPVecIntFilt[0][swRemain], INC0 = 1
        // REG1 : swIntLag                       INC1 = 1
        // REG2 : &pswPPreState[0],              INC2 = 1
        // REG3 : &pswInterpCoefs[0],            INC3 = 6
        // REG5 : swEnergy
        // REG7 : swScale
        // RL6 : 0x8000

        // DEBUG: change from RL6 to ACC1
        // DEBUG: change from REG0 to REG3
        VoC_multf32inc_rp(ACC1,REG7,REG3,DEFAULT);

        // DEBUG: change REG4=2 to REG4=1
        VoC_lw16i_short(REG4,1,DEFAULT);
        VoC_add32_rr(ACC1,ACC1,RL6,DEFAULT);
        VoC_loop_i_short(10,DEFAULT);

        VoC_startloop0
        // DEBUG: change from REG0 to REG3
        // DEBUG: change from RL6 to ACC1
        VoC_multf32inc_rp(ACC1,REG7,REG3,DEFAULT);
        // DEBUG: change from REG3 to REG0
        exit_on_warnings=OFF;
        VoC_sw16inc_p(ACC1_HI,REG0,DEFAULT);
        exit_on_warnings=ON;
        VoC_add32_rr(ACC1,ACC1,RL6,DEFAULT);

        VoC_endloop0;


        // &pswPPreCurr[- swIntLag - P_INT_MACS / 2]
        // DEBUG: change INCR3=1 to INCR3=2
        VoC_lw16i_set_inc(REG3,GLOBAL_DE_pswPPreState_ADDR+142,2);
        // DEBUG: change from VoC_add to VoC_sub
        VoC_sub16_rr(REG3,REG3,REG1,DEFAULT);

        // pswExcite in REG0
        VoC_lw32_sd(REG01,0,IN_PARALLEL);

        // &pswInterpCoefs[0]
        VoC_lw16i_set_inc(REG2,LOCAL_pitchPreFilt_pswInterpCoefs_ADDR,2);

        // &pswPPreCurr[0]
        // DEBUG: change INCR1=2 to INCR1=1
        VoC_lw16i_set_inc(REG1,GLOBAL_DE_pswPPreState_ADDR+147,1);


        // REG0 : &pswExcite[0],                        INC0 = 1
        // REG1 : &pswPPreCurr[0],                      INC1 = 1
        // REG2 : &pswInterpCoefs[0],                   INC2 = 2
        // REG3 : &pswPPreCurr[-swIntLag-P_INT_MACS/2], INC3 = 2
        // REG4 : 2
        // REG5 : swEnergy
        // REG6 :
        // REG7 :
        // RL6 : 0x8000


        VoC_lw16inc_p(ACC0_HI,REG0,DEFAULT);
        VoC_loop_i(1,40)


        // change from VoC_movreg16 to VoC_movreg32
        VoC_movreg32(REG67,REG23,DEFAULT);
        // DEBUG: could not make the instruction before as IN_PARALLEL
        //        a recognized bug in library
        VoC_aligninc_pp(REG2,REG3,DEFAULT);
        // inject 0x8000 (round) at start
        VoC_lw16i_short(ACC0_LO,0,IN_PARALLEL);
        VoC_loop_i_short(5,DEFAULT);

        //VoC_movreg16(ACC0_LO,RL6_LO,IN_PARALLEL);
        VoC_startloop0
        VoC_bimac32inc_pp(REG3,REG2);
        VoC_endloop0

        VoC_movreg32(REG23,REG67,DEFAULT);
        VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
        // DEBUG: change from IN_PARALLEL to DEFAULT
        VoC_lw16inc_p(ACC0_HI,REG0,DEFAULT);
        // &pswPPreCurr[i+1-swIntLag-P_INT_MACS/2]
        VoC_add16_rr(REG3,REG3,REG4,IN_PARALLEL);

        exit_on_warnings= OFF;
        VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
        exit_on_warnings= ON;

        VoC_endloop1



        /* !!! if critical loop above is not correct, use this one:

        VoC_lw16inc_p(ACC0_HI,REG0,DEFAULT);
        VoC_loop_i(1,40)

            VoC_aligninc_pp(REG2,REG3,DEFAULT);
            VoC_movreg32(REG67,REG23,IN_PARALLEL);

            VoC_loop_i_short(5,DEFAULT);
            // inject 0x8000 (round) at start
            VoC_movreg16(ACC0_LO,RL6_HI,IN_PARALLEL);
            VoC_startloop0
                VoC_bimac32inc_pp(REG3,REG2,DEFAULT);
            VoC_endloop0

            VoC_movreg16(REG23,REG67,DEFAULT);
            VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
            VoC_lw16inc_p(ACC0_HI,REG0,DEFAULT);

        exit_on_warnings= OFF;
            VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
        exit_on_warnings= ON;

            // &pswPPreCurr[i+1-swIntLag-P_INT_MACS/2]
            VoC_add16_rr(REG3,REG3,REG4,IN_PARALLEL);

        VoC_endloop1 */


        // REG4 : 2
        // REG5 : swEnergy
        // REG6 :
        // REG7 :
        // RL6 : 0x8000


        // *************************************
        // CII_agcGain_opt
        // input  : REG0 -> pswStateCurr (param0), INC0 = 2
        //          REG5 -> swEngyRShft (param2)
        //          RL7  -> snsInSigEnergy
        // output : REG7 -> return
        // used regs : REG014567,ACC0
        // *************************************

        // DEBUG: inverse RL7_HI & RL7_LO
        VoC_movreg16(RL7_HI,RL7_LO,DEFAULT);
        VoC_movreg16(RL7_LO,RL7_HI,IN_PARALLEL);

        // &pswPPreCurr[0]
        VoC_lw16i_set_inc(REG0,GLOBAL_DE_pswPPreState_ADDR+147,2);
        VoC_jal(CII_agcGain_opt);

        // pop pswExcite & pswExciteOut
        VoC_pop32(REG01,DEFAULT);

        // pswPPreCurr
        VoC_lw16i_set_inc(REG0,GLOBAL_DE_pswPPreState_ADDR+147,1);


        // REG0 : &pswPPreCurr[0],    INC0 = 1
        // REG1 : &pswExciteOut[0],   INC1 = 1
        // REG4 :
        // REG5 :
        // REG7 : swScale
        // RL6 : 0x8000

        // to integrate <<1

        VoC_lw16i_short(FORMAT32,-2,DEFAULT);
        // DEBUG: add following instruction

        VoC_lw16i_short(INC1,1,DEFAULT);
        // DEBUG: change REG5 to REG7
        VoC_multf32inc_rp(ACC0,REG7,REG0,DEFAULT);

        // DEBUG: add following instruction
        VoC_lw32_d(RL6,CONST_0x00008000L_ADDR);
        VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
        VoC_loop_i_short(40,DEFAULT);

        VoC_startloop0
        // DEBUG: change REG5 to REG7
        VoC_multf32inc_rp(ACC0,REG7,REG0,DEFAULT);

        exit_on_warnings = OFF;
        // DEBUG: change REG0 to REG1
        VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
        exit_on_warnings = ON;

        VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);

        VoC_endloop0

        // go back to normal format

        VoC_lw16i_short(FORMAT32,-1,DEFAULT);
        // GLOBAL_DE_pswPPreState_ADDR is even
        VoC_lw16i_set_inc(REG1,GLOBAL_DE_pswPPreState_ADDR+40,2);
        VoC_lw16i_set_inc(REG0,GLOBAL_DE_pswPPreState_ADDR,2);

        VoC_lw32inc_p(ACC0,REG1,DEFAULT);
        VoC_loop_i(0,73)
        VoC_lw32inc_p(ACC0,REG1,DEFAULT);
        exit_on_warnings=OFF;
        VoC_sw32inc_p(ACC0,REG0,DEFAULT);
        exit_on_warnings=ON;
        VoC_endloop0

        // store last ...
        VoC_sw16_p(ACC0_LO,REG0,DEFAULT);

pitchPreFilt_opt_end:;



    }
    // STACK16              STACK32
    // ------------------------------------------
    // RA
    // pswDcdSpchFrm
    // swFrameType
    // swR0Index


    // ************************************************
    // CII_level_estimator_opt
    // input  : REG7 -> update
    //          REG0 -> pswDecodedSpeechFrame, INC0 = 2
    // output : none
    // used regs : REG0167, ACC01
    // ************************************************


    // &pswDecodedSpeechFrame[giSfrmCnt * S_LEN]
    VoC_lw16_sd(REG0,2,DEFAULT);

    // update
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_jal(CII_level_estimator_opt);


    // compute 10*giSfrmCnt
    VoC_lw16i_short(REG0,10,DEFAULT);
    VoC_mult16_rd(REG0,REG0,GLOBAL_DE_giSfrmCnt_ADDR);

    // ppswSynthAs[giSfrmCnt]
    VoC_lw16i_set_inc(REG3,DEC_pswSythAsSpace_ADDR,1);
    VoC_add16_rr(REG3,REG3,REG0,DEFAULT);

    // push ppswSynthAs[giSfrmCnt] & 10*giSfrmCnt
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG3,DEFAULT);

    // STACK16              STACK32
    // ------------------------------------------
    // RA
    // pswDcdSpchFrm
    // swFrameType
    // swR0Index
    // 10*giSfrmCnt
    // ppswSynthAs[giSfrmCnt]

//          BEGIN OF CII_signal_conceal_sub_opt
    {



#if 0

        voc_short LOCAL_signal_conceal_sub_pswStateTmp_ADDR,10,x
        voc_short LOCAL_signal_conceal_sub_swOutTmp_ADDR ,40,x

#endif


        VoC_lw16i_set_inc(REG0,GLOBAL_DE_pswSynthFiltState_ADDR,2);
        VoC_lw16i_set_inc(REG2,LOCAL_signal_conceal_sub_pswStateTmp_ADDR,2);

        // for (i = 0; i < 10; ++i) pswStateTmp[i] = pswSynthFiltState[i];
        VoC_loop_i_short(5,DEFAULT);
        VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);
        VoC_startloop0
        VoC_lw32inc_p(ACC0,REG0,DEFAULT);
        exit_on_warnings=OFF;
        VoC_sw32inc_p(ACC0,REG2,DEFAULT);
        exit_on_warnings=ON;
        VoC_endloop0

        // *************************************
        // CII_lpcIir_opt_paris
        // input  : REG3 -> pswCoef ,  INC3 =  1
        //          REG2 -> pswState,  INC2 =  1
        //          REG1 -> swFiltOut, INC1 = -1
        //          REG0 -> pswInput,  INC0 =  1
        // output : none
        // used   : all
        // *************************************

        VoC_lw16i_set_inc(REG0,DEC_pswPPFExcit_ADDR,1);
        VoC_lw16i_set_inc(REG1,LOCAL_signal_conceal_sub_swOutTmp_ADDR,-1);
        VoC_lw16i_set_inc(REG2,LOCAL_signal_conceal_sub_pswStateTmp_ADDR,1);
        VoC_jal(CII_lpcIir_opt_paris);

        // general purpose 1
        VoC_lw16i_short(REG3,1,DEFAULT);

        // Determine level in db of synthesized signal
        VoC_lw16i_set_inc(REG2,LOCAL_signal_conceal_sub_swOutTmp_ADDR,2);

        // set format to (lo >> 2)

        VoC_lw16i_short(FORMAT16,(2-16),DEFAULT);
        VoC_multf16inc_rp(REG0,REG3,REG2,DEFAULT);
        VoC_multf16inc_rp(REG1,REG3,REG2,IN_PARALLEL);

        VoC_loop_i_short(20,DEFAULT);
        VoC_lw32z(ACC0,IN_PARALLEL);
        VoC_startloop0
        VoC_multf16inc_rp(REG0,REG3,REG2,DEFAULT);
        VoC_multf16inc_rp(REG1,REG3,REG2,IN_PARALLEL);
        exit_on_warnings =OFF;
        VoC_bimac32_rr(REG01,REG01);
        exit_on_warnings = ON;
        VoC_endloop0

        // set back format to (lo << 1)

        VoC_lw16i_short(FORMAT16,(15-16),DEFAULT);
        // *********************************
        // CII_level_calc_opt
        // input  : ACC0 -> *pl_en
        //          REG0 -> swInd
        // output : ACC1 <- *pl_en
        //          REG7 <- return
        // used : REG0167, ACC01
        // *********************************

        VoC_lw16i_short(REG0,0,DEFAULT);
        VoC_jal(CII_level_calc_opt);

        // swIndex = mult(negate(swLevelMean), 1638);
        VoC_lw16i(REG1,-1638);
        VoC_multf16_rd(REG0,REG1,DEC_swLevelMean_ADDR);

        // swPermitMuteSub
        VoC_lw16_d(REG1,DEC_swMutePermit_ADDR);

        // if (sub(swIndex, 15) > 0) swIndex = 15;
        VoC_bngt16_rd(signal_conceal_opt00,REG0,CONST_15_ADDR);
        VoC_lw16i_short(REG0,15,DEFAULT);
signal_conceal_opt00:

        // if (swMuteFlagOld > 0) swPermitMuteSub = 1;
        VoC_bngtz16_d(signal_conceal_opt01,GLOBAL_DE_swMuteFlagOld_ADDR);
        VoC_lw16i_short(REG1,1,DEFAULT);
signal_conceal_opt01:

        // REG0 : swIndex
        // REG1 : swPermitMuteSub


        // if (swPermitMuteSub > 0)
        VoC_bgtz16_r(signal_conceal_opt_go,REG1);
signal_conceal_opt_exit2:
        VoC_jump(signal_conceal_opt_exit);
signal_conceal_opt_go:

        // &psrLevelMaxIncrease[swIndex]
        VoC_lw16i(REG2,TABLE_ERR_psrLevelMaxIncrease_ADDR);
        VoC_add16_rr(REG2,REG2,REG0,DEFAULT);

        // if (sub(swLevelSub, MIN_MUTE_LEVEL) <= 0)
        // swPermitMuteSub = 0 <=> exit
        VoC_bngt16_rd(signal_conceal_opt_exit2,REG2,CONST_neg45_ADDR);

        // swMute = sub(swLevelSub, add(swLevelMax, psrLMaxIncr[swIndex]));
        VoC_add16_pd(REG2,REG2,DEC_swLevelMax_ADDR);
        VoC_sub16_rr(REG2,REG7,REG2,DEFAULT);

        // if (swMute <= 0)
        // swPermitMuteSub = 0 <=> exit
        VoC_bngtz16_r(signal_conceal_opt_exit,REG2);

        // at this point, (swPermitMuteSub > 0)
        // if (sub(swMute, (Shortword) 15) > 0)
        VoC_bngt16_rd(signal_conceal_opt04,REG2,CONST_15_ADDR);
        // swMute = 15;
        VoC_lw16i_short(REG2,15,DEFAULT);
signal_conceal_opt04:

        VoC_lw16i_short(REG7,1,DEFAULT);

        // &psrConceal[-1]
        VoC_lw16i_set_inc(REG0,TABLE_ERR_psrConceal_ADDR-1,2);

        // if (swUFI > 0)
        VoC_bngtz16_d(signal_conceal_opt05,GLOBAL_UFI_ADDR);
        // *pswMuteFlag = 1;
        VoC_sw16_d(REG7,DEC_swMuteFlag_ADDR);
signal_conceal_opt05:

        // &psrConceal[swMute - 1]
        VoC_add16_rr(REG0,REG0,REG2,DEFAULT);

        // 0x8000
        VoC_lw32_d(RL6,CONST_0x00008000L_ADDR);

        // pswSynthFiltState
        VoC_lw16i_set_inc(REG1,GLOBAL_DE_pswSynthFiltState_ADDR,2);

        // psrConceal[swMute - 1]
        VoC_lw16_p(REG3,REG0,DEFAULT);

        // pswSynthFiltState
        VoC_movreg16(REG0,REG1,IN_PARALLEL);

        // optimized for speed
        VoC_multf32inc_rp(ACC0,REG3,REG1,DEFAULT);
        VoC_multf32inc_rp(ACC1,REG3,REG1,IN_PARALLEL);
        VoC_NOP();
        VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
        VoC_add32_rr(RL7,ACC1,RL6,IN_PARALLEL);

        VoC_loop_i_short(5,DEFAULT);
        VoC_startloop0

        VoC_multf32inc_rp(ACC0,REG3,REG1,DEFAULT);
        VoC_multf32inc_rp(ACC1,REG3,REG1,IN_PARALLEL);
        VoC_movreg16(RL7_LO,ACC0_HI,DEFAULT);
        VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
        VoC_add32_rr(RL7,ACC1,RL6,IN_PARALLEL);
        exit_on_warnings=OFF;
        VoC_sw32inc_p(RL7,REG0,DEFAULT);
        exit_on_warnings=ON;

        VoC_endloop0


        // optimized for speed (pointers to different RAMs)
        VoC_lw16i_set_inc(REG0,GLOBAL_DE_pswPPreState_ADDR+147-40,1);
        VoC_lw16i_set_inc(REG1,DEC_pswPPFExcit_ADDR,1);
        VoC_lw16i_set_inc(REG2,GLOBAL_DE_pswLtpStateBaseDec_ADDR+147-40,1);


        VoC_loop_i_short(40,DEFAULT);
        VoC_startloop0

        VoC_multf32_rp(ACC0,REG3,REG0,DEFAULT);
        VoC_multf32_rp(ACC1,REG3,REG1,IN_PARALLEL);

        VoC_multf32inc_rp(RL7,REG3,REG2,DEFAULT);

        VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
        VoC_add32_rr(ACC1,ACC1,RL6,IN_PARALLEL);

        VoC_add32_rr(RL7,RL7,RL6,DEFAULT);

        VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);
        VoC_sw16inc_p(ACC1_HI,REG1,DEFAULT);
        VoC_sw16inc_p(RL7_HI,REG2,IN_PARALLEL);

        VoC_endloop0

signal_conceal_opt_exit:;

        // END OF CII_signal_conceal_sub_opt


    }




    // *********************************************
    // CII_lpcIir_opt_paris
    // input : REG0 -> pswInput (param0),  INC0 =  1
    //         REG1 -> swFiltOut (param3), INC1 = -1
    //         REG2 -> pswState (param2),  INC2 =  1
    //         REG3 -> pswCoef (param1),   INC3 =  1
    // output : none:
    // used : REG012345, ACC0
    // *********************************************

    // ppswSynthAs[giSfrmCnt] (param1)
    VoC_pop16(REG3,DEFAULT);

    // STACK16              STACK32
    // ------------------------------------------
    // RA
    // pswDcdSpchFrm
    // swFrameType
    // 10*giSfrmCnt

    // pswPPFExcit (param0)
    VoC_lw16i_set_inc(REG0,DEC_pswPPFExcit_ADDR, 1);

    // pswState
    VoC_lw16i_set_inc(REG2,GLOBAL_DE_pswSynthFiltState_ADDR, 1);

    // swFiltOut
    VoC_lw16i_set_inc(REG1,DEC_pswSynthFiltOut_ADDR, -1);
    VoC_jal(CII_lpcIir_opt_paris);


    // ************************************************
    // CII_spectralPostFilter_opt
    // input  : REG0 -> pswSPFIn (param0),     INC0 = 2
    //          REG1 -> pswSPFOut (param3),    INC1 = 1
    //          REG2 -> pswDenomCoef (param2), INC2 = 1
    //          REG3 -> pswNumCoef (param1),   INC3 = 1
    // output : none
    // used : all
    // ************************************************


    // 10*giSfrmCnt
    VoC_pop16(REG4,DEFAULT);

    // STACK16              STACK32
    // ------------------------------------------
    // RA
    // pswDcdSpchFrm
    // swFrameType
    // swR0Index


    VoC_lw16i_set_inc(REG0,DEC_pswSynthFiltOut_ADDR,2);
    VoC_lw16i_set_inc(REG2,DEC_pswPFDenomAsSpace_ADDR,1);
    VoC_lw16i_set_inc(REG3,DEC_pswPFNumAsSpace_ADDR,1);

    VoC_add16_rr(REG2,REG4,REG2,DEFAULT);
    VoC_add16_rr(REG3,REG4,REG3,IN_PARALLEL);

    // &pswDecodedSpeechFrame[giSfrmCnt * S_LEN]
    VoC_lw16_sd(REG1,2,DEFAULT);
    VoC_lw16i_short(INC1,1,DEFAULT);
//      VoC_jal(CII_spectralPostFilter_opt);



//  VoC_push16(RA,DEFAULT);
    VoC_push16(REG1,DEFAULT);
    VoC_push16(REG2,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);

    // STACK16               STACK32
    // ------------------------------------
    // RA                    pswSPFIn & pswSPFOut
    // pswSPFOut
    // pswDenomCoef

    // *********************************************
    // CII_g_corr1s_new
    // input:   REG0 -> pswIn,         INC0 =  2
    //          REG5 -> swEngyRShft
    // output : REG7 -> swEngyLShft
    //          ACC0 -> *pL_out
    // used : REG014567, ACC0
    // *********************************************

    VoC_lw16_d(REG5,GLOBAL_DE_SP_swEngyRShift_ADDR);
    VoC_jal(CII_g_corr1s_new);

    // snsOrigEnergy
    VoC_add32_rd(RL7,ACC0,CONST_0x00008000L_ADDR);

    VoC_movreg16(RL7_LO,REG7,DEFAULT);

    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    // *********************************************
    // CII_lpcFir_opt
    // input : REG0 -> pswInput (param0),  INC0 = -1
    //         REG1 -> swFiltOut (param3), INC1 =  1
    //         REG2 -> pswState (param2),  INC2 =  1
    //         REG3 -> pswCoef (param1),   INC3 =  1
    // output : none:
    // used : REG012345, ACC0
    // *********************************************


    VoC_lw16i_short(INC0,-1,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);



    // STACK16               STACK32
    // ------------------------------------
    // RA
    // pswSPFOut
    // pswDenomCoef

    VoC_lw16i_set_inc(REG2,GLOBAL_DE_gpswPostFiltStateNum_ADDR,1);
    VoC_jal(CII_lpcFir_opt);

    // *********************************************
    // CII_lpcIir_opt_paris
    // input : REG0 -> pswInput (param0),  INC0 =  1
    //         REG1 -> swFiltOut (param3), INC1 = -1
    //         REG2 -> pswState (param2),  INC2 =  1
    //         REG3 -> pswCoef (param1),   INC3 =  1
    // output : none:
    // used : REG012345, ACC0
    // *********************************************

    VoC_pop16(REG3,DEFAULT);

    // STACK16               STACK32
    // ------------------------------------
    // RA
    // pswSPFOut


    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16_sd(REG1,0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,GLOBAL_DE_gpswPostFiltStateDenom_ADDR,1);
    VoC_jal(CII_lpcIir_opt_paris);

    // postemphasis section of postfilter
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16_sd(REG2,0,DEFAULT);
    VoC_lw16i(REG1,0x8000);
    VoC_lw16i(REG3,0x199a);

    // REG0 : pswSPFOut, INC0 =  1
    // REG1 : 0x8000,    INC1 = -1
    // REG2 : pswSPFOut, INC2 =  1
    // REG3 : 0x199a,    INC3 =  1

    // we use the fact that :
    // L_deposit_h(pswSPFOut[i]) + 0x00008000L =
    // L_deposit_h(pswSPFOut[i]) + L_deposit_l(0x8000)


    // L_deposit_h(pswSPFOut[0])
    VoC_lw16inc_p(ACC0_HI,REG0,DEFAULT);
    // L_deposit_l(0x8000)
    VoC_movreg16(ACC0_LO,REG1,IN_PARALLEL);
    // += swPostEmphasisState*POST_EMPHASIS
    VoC_msu32_rd(REG3,GLOBAL_DE_swPostEmphasisState_ADDR);

    VoC_loop_i_short(39,DEFAULT);
    VoC_startloop0

    // L_deposit_h(pswSPFOut[i+1])
    VoC_lw16inc_p(ACC0_HI,REG0,DEFAULT);
    // move previous result
    VoC_movreg16(REG4,ACC0_HI,IN_PARALLEL);
    // pswSPFOut[i]*POST_EMPHASIS
    VoC_msu32_rp(REG3,REG2,DEFAULT);
    // L_deposit_l(0x8000)
    VoC_movreg16(ACC0_LO,REG1,IN_PARALLEL);

    exit_on_warnings = OFF;
    // pswSPFOut[i] = swTemp
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    exit_on_warnings = ON;

    VoC_endloop0


    // tmp = pswSPFOut[39]
    VoC_lw16_p(REG1,REG2,DEFAULT);
    // pswSPFOut[39] = swTemp
    VoC_sw16_p(ACC0_HI,REG2,DEFAULT);
    // swPostEmphasisState = tmp
    VoC_sw16_d(REG1,GLOBAL_DE_swPostEmphasisState_ADDR);


    // *************************************
    // CII_agcGain_opt
    // input  : REG0 -> pswStateCurr (param0), INC0 = 2
    //          REG5 -> swEngyRShft (param2)
    //          RL7  -> snsInSigEnergy
    // output : REG7 -> return
    // used regs : REG014567,ACC0
    // *************************************

    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_lw16_d(REG5,GLOBAL_DE_SP_swEngyRShift_ADDR);
    VoC_jal(CII_agcGain_opt);

    // swRunningGain = gswPostFiltAgcGain;
    // L_runningGain(0) = L_deposit_h(gswPostFiltAgcGain)
    VoC_lw16i_short(ACC0_LO,0,DEFAULT);
    VoC_lw16_d(REG6,GLOBAL_DE_gswPostFiltAgcGain_ADDR);
    VoC_movreg16(ACC0_HI,REG6,DEFAULT);

    // -AGC_COEF in REG4
    // AGC_COEF in REG5
    VoC_lw16i(REG4,-0x19a);
    VoC_lw16i(REG5,0x19a);

    // REG4 : -0x19a
    // REG5 : 0x19a
    // REG6 : swRunningGain
    // REG7 : swAgcGain


    // NOTE  : extract_h(L_shl(L_mult(),2) <=>
    //         format16(mult32()), format16() = (<< 2+1)


    VoC_lw16i_short(FORMAT16,(13-16),DEFAULT);
    // L_runningGain(0) = L_mac(gswPostFiltAgcGain, -AGC_COEF);
    // L_runningGain(0) = L_mac(swAgcGain, AGC_COEF);
    VoC_bimac32_rr(REG67,REG45);

    // pswSPFOut
    VoC_pop16(REG2,DEFAULT);

    // STACK16               STACK32
    // ------------------------------------
    // RA

    // swRunningGain(0) = extract_h(L_runningGain(0));
    VoC_movreg16(REG6,ACC0_HI,DEFAULT);

    // L_Output(i+1) = L_mult(swRunningGain(i+1), pswSPFOut[i+1]);
    VoC_multf16_rp(REG3,REG6,REG2,DEFAULT);


    VoC_loop_i_short(39,DEFAULT);
    VoC_startloop0

    // L_runningGain(i+1) = L_mac(swRunningGain(i), -AGC_COEF);
    // L_runningGain(i+1) = L_mac(swAgcGain, AGC_COEF);
    VoC_bimac32_rr(REG67,REG45);

    exit_on_warnings = OFF;
    // pswSPFOut[i] = extract_h(L_Output(i));
    VoC_sw16inc_p(REG3,REG2,DEFAULT);
    exit_on_warnings = ON;

    // swRunningGain(i+1) = extract_h(L_runningGain);
    VoC_movreg16(REG6,ACC0_HI,DEFAULT);

    // L_Output(i+1) = L_mult(swRunningGain(i+1), pswSPFOut[i+1]);
    VoC_multf16_rp(REG3,REG6,REG2,DEFAULT);
    VoC_NOP();

    VoC_endloop0

//  VoC_pop16(RA,DEFAULT);

    VoC_lw16i_short(FORMAT16,(15-16),DEFAULT);

    // pswSPFOut[39] = extract_h(L_Output(39));
    VoC_sw16inc_p(REG3,REG2,DEFAULT);
    VoC_sw16_d(REG6,GLOBAL_DE_gswPostFiltAgcGain_ADDR);


    // ************************************************
    // CII_level_estimator_opt
    // input  : REG7 -> update
    //          REG0 -> pswDecodedSpeechFrame, INC0 = 2
    // output : none
    // used regs : REG0167, ACC01
    // ************************************************

    // &pswDecodedSpeechFrame[giSfrmCnt * S_LEN]
    VoC_lw16_sd(REG0,2,DEFAULT);

    // &pswDecodedSpeechFrame[new_giSfrmCnt * S_LEN]
    VoC_add16_rd(REG1,REG0,CONST_40_ADDR);

    // update
    VoC_lw16i_short(REG7,1,DEFAULT);

    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    // save &pswDecodedSpeechFrame[new_giSfrmCnt * S_LEN]
    VoC_sw16_sd(REG1,2,DEFAULT);
    VoC_jal(CII_level_estimator_opt);

    // giSfrmCnt++
    VoC_lw16i_short(REG6,3,DEFAULT);
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_add16_rd(REG7,REG7,GLOBAL_DE_giSfrmCnt_ADDR);
    VoC_jump(DEC_giSfrmCnt_LOOP_BEGIN)

DEC_giSfrmCnt_LOOP:


    // STACK16              STACK32
    // ------------------------------------------
    // RA
    // pswDcdSpchFrm
    // swFrameType
    // swR0Index


    // swMuteFlagOld = swMuteFlag;
    // swOldR0Dec = swR0Dec;
    // swOldR0IndexDec = swR0Index;

    VoC_pop16(REG0,DEFAULT);
    VoC_lw16_d(REG1,DEC_swMuteFlag_ADDR);
    VoC_lw16_d(REG2,GLOBAL_DE_SP_swR0Dec_ADDR);
    VoC_sw16_d(REG0,GLOBAL_DE_swOldR0IndexDec_ADDR);
    VoC_sw16_d(REG1,GLOBAL_DE_swMuteFlagOld_ADDR);
    VoC_sw16_d(REG2,GLOBAL_DE_swOldR0Dec_ADDR);


    // STACK16              STACK32
    // ------------------------------------------
    // RA
    // pswDcdSpchFrm
    // swFrameType


    // unpile stack ...
    VoC_pop16(REG0,DEFAULT);
    VoC_pop16(REG0,DEFAULT);
    VoC_pop16(RA,DEFAULT);


    // We use the following properties:
    // 1) Old are in RAM_X, New are in RAM_Y
    // 2) Ks, As, Num & Denom are consecutive
    // 3) copy 40 16-bit values <=> 20 32-bit values

    VoC_lw16i_set_inc(REG0,DEC_pswFrmPFDenom_ADDR,2);
    VoC_lw16i_set_inc(REG1,GLOBAL_DE_pswOldFrmPFDenom_ADDR,2);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_loop_i_short(10,DEFAULT);
    VoC_lw32inc_p(ACC1,REG0,IN_PARALLEL);
    VoC_startloop0

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,IN_PARALLEL);
    VoC_lw32inc_p(ACC1,REG0,DEFAULT);
    VoC_sw32inc_p(ACC1,REG1,IN_PARALLEL);

    VoC_endloop0

    VoC_return;
}



// *************************************
// CII_agcGain_opt
// input  : REG0 -> pswStateCurr (param0), INC0 =2
//          REG5 -> swEngyRShft (param2)
//          RL7  -> snsInSigEnergy
// output : REG7 -> return
// used regs : REG014567,ACC0
// *************************************

void CII_agcGain_opt(void)
{

    VoC_push16(RA,DEFAULT);

    // *************************************
    // CII_g_corr1s_new
    // input:   REG0 -> pswIn,  INC0 = 2
    //          REG5 -> swEngyRShft
    // output : REG7 -> swEngyLShft
    //          ACC0 -> *pL_out
    // used : all except REG23,ACC1,RL6,RL7
    // *************************************

    VoC_jal(CII_g_corr1s_new);

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_bez32_r(agcGain_opt_END,ACC0);

    // snsOutEnergy.sh = add(snsOutEnergy.sh, 1);
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);

    // snsOutEnergy.man = round(L_OutEnergy)
    // snsInSigEnergy.man = shr(snsInSigEnergy.man, 1);
    VoC_add32_rd(REG01,ACC0,CONST_0x00008000L_ADDR);


    // ************************************
    // CII_DIV_S
    // input :  REG0 -> Nominator (param0)
    //          REG1 -> Denominator (param1)
    // output : REG2 -> return
    // used regs : REG012, RL67
    // ************************************

    VoC_movreg16(REG0,RL7_HI,DEFAULT);
    VoC_movreg16(REG5,RL7_LO,DEFAULT);
    VoC_shr16_ri(REG0,1,DEFAULT);
    VoC_jal(CII_DIV_S);

    // *******************************
    // norm_s_opt
    // input:   REG2 -> swTemp
    // output : REG0 <- return
    //          REG2 <- (REG2 >> REG0)
    // other used regs : none
    // *******************************

    // swAgcShftCnt = norm_s(snsAgc.man);
    // snsAgc.man = shl(snsAgc.man, swAgcShftCnt);
    VoC_jal(CII_norm_s_opt);

    // REG0 : -swAgcShftCnt
    // REG2 : snsAgc.man
    // REG7 : snsOutEnergy.sh

    // sub(sub(snsInSigEnergy.sh, snsOutEnergy.sh),-swAgcShftCnt);
    VoC_sub16_rr(REG1,REG5,REG7,DEFAULT);
    VoC_sub16_rr(REG0,REG1,REG0,DEFAULT);

    // L_deposit_h(snsAgc.man)
    VoC_movreg16(RL7_HI,REG2,DEFAULT);
    VoC_lw16i_short(RL7_LO,0,IN_PARALLEL);


    // *************************
    // CII_sqroot_opt
    // input : RL7
    // output : REG7
    // used regs : ACC0, REG567
    // *************************

    VoC_jal(CII_sqroot_opt);

    // REG0 : snsAgc.sh
    // REG7 : snsAgc.man
    VoC_movreg16(REG6,REG7,DEFAULT);
    VoC_lw16i(REG7,0x7fff);
    VoC_blt16_rd(agcGain_opt_END,REG0,CONST_neg2_ADDR);

    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_movreg16(REG1,REG0,IN_PARALLEL);
    VoC_and16_rr(REG1,REG5,DEFAULT);

    // REG0 : snsAgc.sh
    // REG1 : snsAgc.sh & 1
    // REG7 : snsAgc.man

    // if (0x1 & snsAgc.sh)
    VoC_bez16_r(agcGain_opt_new_man1,REG1);
    VoC_multf16_rd(REG6,REG6,CONST_0x5a82_ADDR);
agcGain_opt_new_man1:

    VoC_shr16_ri(REG0,1,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);

    // if (snsAgc.sh > 0)
    VoC_bngtz16_r(agcGain_opt_new_man2,REG0);
    VoC_shr16_rr(REG6,REG0,DEFAULT);
agcGain_opt_new_man2:
    // swAgcOut = snsAgc.man;
    VoC_movreg16(REG7,REG6,DEFAULT);

agcGain_opt_END:

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}



void CII_compute_s2_s3_new(void)
{

    // INPUT (si1) : REG2
    // INPUT (flag) : REG7
    // OUTPUT (siNumDelta): REG7

    // load in some constants
    VoC_lw16i_short(REG1,6,DEFAULT);

    // REG4 for si2+1
    // REG5 for si3
    VoC_add16_rr(REG4,REG2,REG1,DEFAULT);
    VoC_sub16_rr(REG5,REG2,REG1,IN_PARALLEL);

    // case flag is 1, add 1 to both
    // case flag is 0, add 0 to both
    // equivalent : add flag to both
    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG7,IN_PARALLEL);

    // if (sub(si2 + 1, (1 << L_BITS) - 1 + 1) > 0)
    VoC_bngt16_rd(COMPUTES2S3_S2LIMIT,REG4,CONST_256_ADDR)
    // si2 + 1 = (1 << L_BITS) - 1 + 1;
    VoC_lw16i(REG4,256);
COMPUTES2S3_S2LIMIT:

    // if (si3 < 0)
    VoC_bgtz16_r(OPENLOOP415,REG5)
    // si3 = 0;
    VoC_lw16i_short(REG5,0,DEFAULT);
OPENLOOP415:

    // siNumDelta = sub(si2 + 1, si3);
    VoC_sub16_rr(REG7,REG4,REG5,DEFAULT);

    // &psrLagTbl[si3]
    VoC_lw16i_set_inc(REG3,TABLE_SP_ROM_psrLagTbl_ADDR,1);
    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);

    // pswLArray
    VoC_lw16i_set_inc(REG2,LOCAL_OPENLOOPSEARCH_pswLArray_ADDR,1);

    VoC_loop_r_short(REG7,DEFAULT)
    VoC_lw16inc_p(REG4,REG3,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG3,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_return;
}



void CII_fnBest_CG_new(void)
{

    // INPUT
    // REG0 : pswCframe, INC0 = 1   (param0)
    // REG1 : pswGframe, INC1 = 1   (param1)
    // REG4 : swCmaxSqr     (param2)
    // REG5 : swGmax        (param3)
    // REG7 : siNumPairs        (param4)
    //
    // OUTPUT
    // REG3 : return
    // REG4 : swCmaxSqr     (through param2)
    // REG5 : swGmax        (through param3)
    //
    // used all regs except ACC1, RL6, RL7


    VoC_lw16i_short(REG3,-1,DEFAULT);
    VoC_push32(ACC1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG2,0,1);


    VoC_loop_r_short(REG7,DEFAULT)
    // REG6 for pswCframe[siLoopCnt]
    VoC_lw16inc_p(REG6, REG0, IN_PARALLEL);
    VoC_startloop0


    // REG7 for pswGframe[siLoopCnt]
    VoC_lw16inc_p(REG7,REG1,DEFAULT);

    VoC_bngtz16_r(FNBEST100, REG6)
    VoC_bngtz16_r(FNBEST100, REG7)

    // swTemp = mult_r(pswCframe[siLoopCnt], pswCframe[siLoopCnt]);
    VoC_multf32_rr(ACC0,REG6,REG6, DEFAULT);
    VoC_NOP();
    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000L_ADDR);
    VoC_movreg16(REG6,ACC0_HI,DEFAULT);

    // L_Temp2 = L_mult(swTemp, swGmax);
    // L_Temp2 = L_msu(L_Temp2, swCmaxSqr, pswGframe[siLoopCnt]);
    VoC_multf32_rr(ACC0, REG6, REG5, DEFAULT);
    VoC_multf32_rr(ACC1,REG7, REG4, IN_PARALLEL);
    VoC_NOP();
    VoC_bngt32_rr(FNBEST100, ACC0, ACC1);

    VoC_movreg32(REG45, REG67, DEFAULT);
    VoC_movreg16(REG3, REG2, IN_PARALLEL);

FNBEST100:
    VoC_inc_p(REG2, DEFAULT);
    // REG6 for pswCframe[siLoopCnt]
    VoC_lw16inc_p(REG6, REG0, DEFAULT);
    VoC_endloop0;

    VoC_pop32(ACC1,DEFAULT);
    VoC_return;
}



void CII_r0BasedEnergyShft_new(void)
{


    // input  REG4 : swR0Index
    // output REG5 : swShiftDownSignal
    // used register   REG5 REG4

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_bngt16_rd(R0Based_END_CASE, REG4, CONST_18_ADDR)
    VoC_lw16i_short(REG5,2,DEFAULT);
    VoC_bngt16_rd(R0Based_END_CASE, REG4, CONST_21_ADDR)
    VoC_lw16i_short(REG5,3,DEFAULT);
    VoC_bngt16_rd(R0Based_END_CASE, REG4, CONST_23_ADDR)
    VoC_lw16i_short(REG5,4,DEFAULT);
    VoC_bngt16_rd(R0Based_END_CASE, REG4, CONST_24_ADDR)
    VoC_lw16i_short(REG5,5,DEFAULT);
    VoC_bngt16_rd(R0Based_END_CASE, REG4, CONST_26_ADDR)
    VoC_lw16i_short(REG5,6,DEFAULT);
    VoC_bngt16_rd(R0Based_END_CASE, REG4, CONST_28_ADDR)
    VoC_lw16i_short(REG5,7,DEFAULT);
    VoC_bngt16_rd(R0Based_END_CASE, REG4, CONST_29_ADDR)
    VoC_lw16i_short(REG5,8,DEFAULT);
R0Based_END_CASE:

    VoC_return;
}


void CII_g_corr1s_new(void)
{

    // INPUT :
    // REG0 : pswIn,    INC0 = 2
    // REG5 : swEngyRShft
    //
    // OUTPUT
    // REG7 : swEngyLShft
    // ACC0 : *pL_out
    //
    // USED : all except REG23,ACC1,RL6,RL7

    VoC_push32(REG23,DEFAULT);

    VoC_aligninc_pp(REG0,REG0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_bgt16_rd(CII_g_corr1s1,REG5,CONST_1_ADDR)

    VoC_loop_i_short(20,DEFAULT)
    VoC_startloop0
    VoC_lw32_p(REG67,REG0,DEFAULT);
    VoC_lw32inc_p(REG23,REG0,IN_PARALLEL);
    VoC_shr16_rr(REG6,REG5,DEFAULT);
    VoC_shr16_rr(REG7,REG5,IN_PARALLEL);
    VoC_bimac32_rr(REG67,REG23);
    VoC_endloop0

    VoC_jump(CII_g_corr1s2);
CII_g_corr1s1:

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_and16_rr(REG4,REG5,DEFAULT);
    VoC_shr16_ri(REG5,1,IN_PARALLEL);

    VoC_loop_i_short(20,DEFAULT)
    VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_shr16_rr(REG6,REG5,DEFAULT);
    VoC_shr16_rr(REG7,REG5,IN_PARALLEL);
    VoC_bimac32_rr(REG67,REG67);
    VoC_endloop0
    VoC_shr16_ri(REG5,-1,DEFAULT);
CII_g_corr1s2:

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_bez32_r(CII_g_corr1s3,ACC0);
    VoC_push16(RA,DEFAULT);
    VoC_jal(CII_NORM_L_ACC0);
    VoC_add16_rr(REG1,REG7,REG1,DEFAULT);
    VoC_sub16_rr(REG7,REG1,REG5,DEFAULT);
    VoC_pop16(RA,IN_PARALLEL);

CII_g_corr1s3:

    VoC_pop32(REG23,DEFAULT);
    VoC_return;

}


void CII_findPeak_new(void)
{


    // ------------------------------------
    // INPUT :
    // REG0 : pswCIn, INC0 = 1  (param1)
    // REG1 : pswGIn, INC1 = 1  (param2)
    // REG7 : swSingleResLag    (param0)
    // ------------------------------------
    // OUTPUT in REG6
    // ------------------------------------
    // used regs : all except ACC1, RL6, RL7
    // ------------------------------------


    // PEAK_VICINITY + 1
    VoC_lw16i_short(REG2,4,DEFAULT);

    // siUpperBound = add(swSingleResLag, PEAK_VICINITY + 1);
    // siLowerBound = sub(swSingleResLag, PEAK_VICINITY + 1);
    VoC_add16_rr(REG4,REG7,REG2,DEFAULT);
    VoC_sub16_rr(REG5,REG7,REG2,IN_PARALLEL);

    VoC_bngt16_rd(FINDPEAK100,REG4,CONST_143_ADDR);
    VoC_lw16i(REG4,143);
FINDPEAK100:

    VoC_bnlt16_rd(FINDPEAK101,REG5,CONST_20_ADDR);
    VoC_lw16i_short(REG5,20,DEFAULT);
FINDPEAK101:

    // INFO : swSingleResLag no longer needed
    // siRange
    VoC_sub16_rr(REG7,REG4,REG5,DEFAULT);
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);

    // saving siLowerBound & siUpperBound
    VoC_push32(REG45,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);

    VoC_sub16_rd(REG6,REG5,CONST_18_ADDR);

    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG6,IN_PARALLEL);
    VoC_lw16i_short(REG4, 0,DEFAULT);

    VoC_lw16i(REG5, 63);

    // ------------------------------------
    // INPUT
    // REG0 : pswCframe, INC0 = 1   (param0)
    // REG1 : pswGframe, INC1 = 1   (param1)
    // REG4 : swCmaxSqr     (param2)
    // REG5 : swGmax        (param3)
    // REG7 : siNumPairs        (param4)
    // ------------------------------------
    // OUTPUT
    // REG3 : return
    // REG4 : swCmaxSqr     (through param2)
    // REG5 : swGmax        (through param3)
    // ------------------------------------
    // used regs : all except ACC1, RL6, RL7
    // ------------------------------------



    VoC_jal(CII_fnBest_CG_new);

    // restituting all
    VoC_pop32(REG45,DEFAULT);
    VoC_pop16(RA,DEFAULT);

    //swFullResPeak
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_be16_rd(FINDPEAKRETURN,REG3,CONST_neg1_ADDR);

    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);

    VoC_be16_rr(FINDPEAKRETURN,REG3,REG4);
    VoC_be16_rr(FINDPEAKRETURN,REG3,REG5);

    VoC_multf32_rd(REG67,REG3,CONST_6_ADDR);
    VoC_NOP();
    VoC_shr16_ri(REG6,1,DEFAULT);

FINDPEAKRETURN:

    VoC_return;
}



void CII_CGInterp_new(void)
{


    // NOTE : pswCIn & pswGIn need to be placed
    //    in different memories !!!
    //
    // INPUT
    // REG0 : pswLIn, INC0 = 1  (param0)
    // REG1 : siNum,  INC1 = 1  (param1)
    // REG2 : pswCIn, INC2 = 6  (param2)
    // REG3 : pswGIn,       (param3)
    // REG5 : siLoIntLag        (param4)
    // REG6 : pswCOut       (param5)
    // REG7 : pswGOut       (param6)

    // save pswCOut & pswGOut
    VoC_push32(REG67,DEFAULT);

    // save pswCIn & pswGIn
    VoC_push32(REG23,DEFAULT);

    // swLoIntLag update
    VoC_add16_rd(REG5,REG5,CONST_2_ADDR);

    VoC_loop_r(1,REG1)

    // swLagInt in REG7
    // swTempRem in REG6
    VoC_multf32inc_pd(REG67,REG0,CONST_0x1555_ADDR);

    // preload 32768 in ACC0 & ACC1
    VoC_lw32_d(ACC0,CONST_0x00008000L_ADDR);

    // swLagInt - swLoIntLag
    VoC_sub16_rr(REG4,REG7,REG5,DEFAULT);

    // swTempRem = shr(swTempRem, 1);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);


    VoC_movreg32(ACC1,ACC0,DEFAULT);

    // save pswLIn
    VoC_push16(REG0,IN_PARALLEL);

    // swLagRem
    VoC_lw16i_short(REG0,6,DEFAULT);
//      VoC_and16_rd(REG6,CONST_0x7fff_ADDR);
    VoC_multf32_rr(REG67,REG6,REG0,DEFAULT);
    // ppsrCGIntFilt[0][swLagRem]
    VoC_lw16i_set_inc(REG3,TABLE_SP_ROM_ppsrCGIntFilt_ADDR,6);

    VoC_add32_rr(REG67,REG67,ACC0,DEFAULT);
    VoC_sub16_rr(REG7,REG0,REG7,DEFAULT);


    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);

    // ppsrCGIntFilt[0][swLagRem] bis
    VoC_movreg16(REG2,REG3,DEFAULT);

    // get in pointers
    VoC_lw32_sd(REG01,0,IN_PARALLEL);

    // pswCIn[swLagInt - swLoIntLag]
    // pswGIn[swLagInt - swLoIntLag]
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG4,IN_PARALLEL);

    VoC_loop_i_short(6,DEFAULT)
    VoC_startloop0
    // no pb here because each pointer
    // is pointing to a different memory
    // using REG23 with same address to
    // prevent from bi-fetch to occur
    VoC_mac32inc_pp(REG2,REG0,DEFAULT);
    VoC_mac32inc_pp(REG3,REG1,IN_PARALLEL);
    VoC_endloop0

    // get out pointers
    VoC_lw32_sd(REG01,1,DEFAULT);
    VoC_NOP();

    // save result
    VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);
    VoC_sw16inc_p(ACC1_HI,REG1,DEFAULT);

    // restitute pswLIn
    VoC_pop16(REG0,DEFAULT);

    exit_on_warnings=OFF;
    // save out pointers
    VoC_sw32_sd(REG01,1,DEFAULT);
    exit_on_warnings=ON;

    VoC_endloop1

    // restitute stack
    VoC_pop32(REG23,DEFAULT);
    VoC_pop32(REG67,DEFAULT);

    VoC_return;

}


void CII_CGInterpValid_new(void)
{

    // NOTE : pswCIn & pswGIn need to be placed
    //    in different memories !!!
    //
    // INPUT
    // REG0 : pswLout, INC0 = 1 (param3)
    // REG2 : pswCIn,  INC2 = 6 (param1)
    // REG3 : pswGIn,       (param2)
    // REG4 : pswCOut       (param4)
    // REG5 : pswGOut       (param5)
    // REG7 : swFullResLag      (param0)

    // OUTPUT
    // REG7 : return


    VoC_push16(REG0,DEFAULT);
    VoC_push32(REG45,DEFAULT);
    VoC_push16(RA,DEFAULT);
    VoC_push32(REG23,IN_PARALLEL);

    VoC_sub16_rd(REG6,REG7,CONST_6_ADDR);

    // INPUT
    // REG6 : swRawLag
    // OUTPUT
    // REG2 : *pswCode
    // REG3 : return

    // unused : REG7

    VoC_jal(CII_quantLag_opt_paris);

    // swLag in REG3
    // siLowerBound in REG2

    VoC_be16_rr(CGINTERPVALID100, REG3, REG7)
    VoC_add16_rd(REG2,REG2,CONST_1_ADDR);
CGINTERPVALID100:

    VoC_add16_rd(REG6,REG7,CONST_6_ADDR);
    VoC_push16(REG2,DEFAULT);

    // INPUT
    // REG6 : swRawLag
    // OUTPUT
    // REG2 : *pswCode
    // REG3 : return

    // unused : REG7

    VoC_jal(CII_quantLag_opt_paris);

    // swLag in REG3
    // siUpperBound in REG2

    VoC_pop16(REG6,DEFAULT);

    VoC_be16_rr(CGINTERPVALID101, REG3, REG7)
    VoC_sub16_rd(REG2,REG2,CONST_1_ADDR);
CGINTERPVALID101:

    // REG67 : siLowerBound & siUpperBound
    VoC_sub16_rr(REG1,REG2,REG6,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_1_ADDR);

    // psrLagTbl[siLowerBound]
    VoC_lw16i_set_inc(REG3,TABLE_SP_ROM_psrLagTbl_ADDR,1);
    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);

    // pswLOut
    VoC_lw16_sd(REG0,1,DEFAULT);

    VoC_loop_r_short(REG1,DEFAULT)
    VoC_lw16inc_p(REG6,REG3,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6, REG3, DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6, REG0, DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    // pswLOut
    // VoC_pop16(REG0,DEFAULT);
    VoC_lw16_sd(REG0,1,DEFAULT);

    // siNum in REG1

    VoC_lw16i_short(INC1,1,DEFAULT);
    // pswCIn & pswGIn
    VoC_pop32(REG23,DEFAULT);

    // save siNum
    // VoC_push16(REG1,IN_PARALLEL);
    VoC_sw16_sd(REG1,1,IN_PARALLEL);

    // pswCOut & pswGOut
    VoC_pop32(REG67,DEFAULT);

    // LSMIN
    VoC_lw16i_short(REG5,18,IN_PARALLEL);


    // NOTE : pswCIn & pswGIn need to be placed
    //    in different memories !!!
    //
    // INPUT
    // REG0 : pswLIn, INC0 = 1  (param0)
    // REG1 : siNum,  INC1 = 1  (param1)
    // REG2 : pswCIn, INC2 = 6  (param2)
    // REG3 : pswGIn,       (param3)
    // REG5 : siLoIntLag        (param4)
    // REG6 : pswCOut       (param5)
    // REG7 : pswGOut       (param6)


    VoC_jal(CII_CGInterp_new);

    VoC_pop16(RA,DEFAULT);
    VoC_pop16(REG7,DEFAULT);


    VoC_return;

}



void CII_bestDelta_new(void)
{

    // INPUT
    // REG0 : pswCSfrm,         (param1)
    // REG1 : pswGSfrm,         (param2)
    // REG2 : pswCCTraj,  INC2 = 1  (param6)
    // REG3 : pswGTraj,   INC3 = 1  (param7)
    // REG4 : pswLagList        (param0)
    // REG5 : pswLTraj      (param5)
    // REG6 : siSfrmIndex       (param4)
    // REG7 : siNumLags     (param3)

    // CAUTION !!!
    // pswcBuf & pswGBuf
    // pswCInterp & pswGInterp
    // need to be placed in different memories

#if 0

    voc_short   LOCAL_BESTDELTA_pswCBuf_ADDR                ,24,x       //[24]shorts
    voc_short   LOCAL_BESTDELTA_pswCInterp_ADDR             ,18,x       //[18]shorts

#endif

#if 0

    voc_short   LOCAL_BESTDELTA_pswGBuf_ADDR                ,24,y       //[24]shorts
    voc_short   LOCAL_BESTDELTA_pswGInterp_ADDR             ,18,y        //[18]shorts

#endif



    VoC_push16(RA,DEFAULT);

    // siSfrmIndex is needed only to compute
    // pTraj[siSfrmIndex], and all 4 are used only once
    // so we can precompute and free REG6

    VoC_add16_rr(REG5,REG5,REG6,IN_PARALLEL);
    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG6,IN_PARALLEL);

    // &pswLTraj[siSfrmIndex]
    VoC_push16(REG5,DEFAULT);

    // general purpose constant = 1
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    // pswLagList
    VoC_push16(REG4,DEFAULT);

    // &pswCCTraj[siSfrmIndex] and &pswGTraj[siSfrmIndex]
    VoC_push32(REG23,IN_PARALLEL);

    // siNumLags
    VoC_push16(REG7,DEFAULT);

    // pswCSfrm & pswGSfrm
    VoC_push32(REG01,IN_PARALLEL);

    // INPUT
    // REG4 : swRunningLag
    // OUTPUT
    // REG0 : TempJJ
    // REG1? TempIp
    //
    // other used : ACCO
    VoC_lw16_p(REG4,REG4,DEFAULT);


    VoC_jal(CII_get_ipjj_opt_paris);
    // siIPLo in REG1
    // siRemLo in REG0

    // pswLagList
    VoC_lw16_sd(REG4,1,DEFAULT);

    // REG7 is not overwritten so = siNumLags-1
    VoC_sub16_rr(REG7,REG7,REG6,DEFAULT);

    // &pswLagList[siNumLags - 1]
    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);

    // siIPLo in REG3, siRemLo in REG2
    VoC_movreg32(REG23,REG01,DEFAULT);

    VoC_lw16_p(REG4,REG4,DEFAULT);


    VoC_jal(CII_get_ipjj_opt_paris);
    // siIPHi in REG1
    // siRemHi in REG0

    // general purpose constant = 2
    VoC_lw16i_short(REG7,2,DEFAULT);

    // siLoLag = sub(siIPLo, CG_INT_MACS / 2 - 1);
    // siHiLag = add(siIPHi, CG_INT_MACS / 2 - 1);
    VoC_sub16_rr(REG5,REG3,REG7,DEFAULT);
    VoC_add16_rr(REG4,REG1,REG7,IN_PARALLEL);

    VoC_bez16_r(BESTDELTA100, REG0)
    // siRemHi != 0
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
BESTDELTA100:

    VoC_lw16i_set_inc(REG0, LOCAL_BESTDELTA_pswCBuf_ADDR, 1);
    VoC_lw16i_set_inc(REG1, LOCAL_BESTDELTA_pswGBuf_ADDR, 1);

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_short(REG7,18,IN_PARALLEL);


    VoC_bnez16_r(BESTDELTA101,REG2)

    // if (siRemLo == 0)
    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    VoC_sw16inc_p(REG6,REG1,IN_PARALLEL);

BESTDELTA101:


    // siHiLag - siLoLag
    // siLoLag - 18
    VoC_sub16_rr(REG6,REG4,REG5,DEFAULT);
    VoC_sub16_rr(REG7,REG5,REG7,IN_PARALLEL);

    VoC_bnez16_r(BESTDELTA102,REG2)
    // if (siRemLo == 0)
    VoC_sub16_rd(REG5,REG5,CONST_1_ADDR);

BESTDELTA102:

    // pswCSfrm & pswGSfrm
    VoC_pop32(REG23,DEFAULT);

    // &pswCSfrm[siLoLag - 18]
    // &pswGSfrm[siLoLag - 18]
    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);

    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);


    // preload
    VoC_lw16inc_p(REG7,REG3,DEFAULT);


    VoC_loop_r_short(REG6,DEFAULT)
    VoC_lw16inc_p(REG6,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    VoC_lw16inc_p(REG7,REG3,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop0





    // NOTE : pswCIn & pswGIn need to be placed
    //    in different memories !!!
    //
    // INPUT
    // REG0 : pswLIn, INC0 = 1  (param0)
    // REG1 : siNum,  INC1 = 1  (param1)
    // REG2 : pswCIn, INC2 = 6  (param2)
    // REG3 : pswGIn,       (param3)
    // REG5 : siLoIntLag        (param4)
    // REG6 : pswCOut       (param5)
    // REG7 : pswGOut       (param6)


    // pswCBuf & pswGBuf
    VoC_lw16i_set_inc(REG2,LOCAL_BESTDELTA_pswCBuf_ADDR,6);
    VoC_lw16i_set_inc(REG3,LOCAL_BESTDELTA_pswGBuf_ADDR,1);

    // siLoLag already in REG5

    // pswCInterp & pswGInterp
    VoC_lw16i(REG6,LOCAL_BESTDELTA_pswCInterp_ADDR);
    VoC_lw16i(REG7,LOCAL_BESTDELTA_pswGInterp_ADDR);

    // pswLagList & siNumLags
    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_lw16_sd(REG1,0,DEFAULT);

    // push pswCInterp & pswGInterp
    VoC_push32(REG67,IN_PARALLEL);

    VoC_jal(CII_CGInterp_new);

    // pop pswCInterp & pswGInterp
    VoC_pop32(REG23,DEFAULT);

    VoC_lw16i_short(INC3,1,DEFAULT);
    // INC2 = 1

    VoC_lw16i_short(INC2,1,DEFAULT);
    // siNumLags
    VoC_pop16(REG7,IN_PARALLEL);

    // INPUT:
    // REG7 : swNum
    // REG2 : &pswCIn[0]     INC2 = 1;
    // REG3 : &pswGIn[0]     INC3 = 1;
    // OUTPUT:
    // REG1 : Peak
    // REG6 : GMax
    // REG7 : CCMax

    VoC_jal(CII_maxCCOverGWithSign_opt_paris);
    // swCmaxSqr in REG7
    // swGmax in REG6
    // swPeak in REG1

    // pop pswLagList
    VoC_pop16(REG0,DEFAULT);

    // pop &pswCCTraj[siSfrmIndex] and &pswGTraj[siSfrmIndex]
    VoC_pop32(REG23,DEFAULT);

    // pop &pswLTraj[siSfrmIndex]
    VoC_pop16(REG4,DEFAULT);

    // &pswLagList[swPeak]
    VoC_add16_rr(REG0,REG0,REG1,IN_PARALLEL);

    // pop RA
    VoC_pop16(RA,DEFAULT);

    // pswLagList[swPeak]
    VoC_lw16_p(REG1,REG0,DEFAULT);


    // sw swGmax
    VoC_sw16_p(REG6,REG3,DEFAULT);

    // sw swCmaxSqr
    VoC_sw16_p(REG7,REG2,DEFAULT);

    // sw pswLagList[swPeak]
    VoC_sw16_p(REG1,REG4,DEFAULT);

    VoC_return;
}


void CII_siCPeakIndex_save_new(void)
{

    // INPUT
    // REG0 : pswCPeaks[siCPeakIndex], INC0 = 1
    // REG1 : pswGPeaks[siGPeakIndex], INC1 = 1
    // REG2 : pswLPeaks[siGPeakIndex], INC2 = 1
    // REG3 : siPeakIndex
    // REG4 : pswCBuf
    // REG5 : pswGBuf
    // REG6 : pswLBuf
    //
    // OUTPUT :
    // REG0 : pswCPeaks[siCPeakIndex+1]
    // REG1 : pswGPeaks[siGPeakIndex+1]
    // REG2 : pswLPeaks[siLPeakIndex+1]


    // &p[siPeakIndex]
    VoC_add16_rr(REG4,REG4,REG3,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG3,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG3,DEFAULT);

    VoC_lw16_p(REG4,REG4,DEFAULT);
    VoC_lw16_p(REG5,REG5,IN_PARALLEL);
    VoC_lw16_p(REG6,REG6,DEFAULT);

    // pswCPeaks[siCPeakIndex] = pswCBuf[siPeakIndex]
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    // pswGPeaks[siGPeakIndex] = pswGBuf[siPeakIndex]
    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    // pswLPeaks[siLPeakIndex] = pswLBuf[siPeakIndex]
    VoC_sw16inc_p(REG6,REG2,DEFAULT);

    VoC_return;
}


void CII_pitchLags_new(void)
{

    // INPUT:
    // REG0 : pswCPeaksSorted,  INC0 = 1    (param 5)
    // REG1 : pswGPeaksSorted,  INC1 = 1    (param 6)
    // REG2 : pswIntCs,     INC2 = 6    (param 1)
    // REG3 : pswIntGs,     INC3 = 1    (param 2)
    // REG4 : swBestIntLag              (param 0)
    // REG5 : swCCThreshold             (param 3)
    // REG6 : pswLPeaksSorted           (param 4)
    // OUTPUT:
    // REG5 : psiNumSorted
    // REG6 : pswPitch
    // REG7 : pswHNWCoef

    // NOTE : all C & G buffers need to be placed
    //    in different memories !!!

#if 0

    voc_short   LOCAL_PITCHLAGS_pswLBuf_ADDR                ,12,x       //[12]shorts
    voc_short   LOCAL_PITCHLAGS_pswCBuf_ADDR                ,12,x        //[12]shorts
    voc_short   LOCAL_PITCHLAGS_pswLPeaks_ADDR              ,14,x        //[14]shorts
    voc_short   LOCAL_PITCHLAGS_pswCPeaks_ADDR              ,14,x        //[14]shorts

#endif


#if 0

    voc_short   LOCAL_PITCHLAGS_pswGBuf_ADDR                ,12,y       //[12]shorts
    voc_short   LOCAL_PITCHLAGS_pswGPeaks_ADDR              ,14,y        //[14]shorts
    voc_short   PITCHLAG_SP32_SAVE_ADDR                     ,y       //1 short
    voc_short   PITCHLAG_SP16_SAVE_ADDR                     ,y       //1 short

#endif



    VoC_push16(RA,DEFAULT);

    // save SP16 & 32
    VoC_sw16_d(SP32,PITCHLAG_SP32_SAVE_ADDR);
    VoC_sw16_d(SP16,PITCHLAG_SP16_SAVE_ADDR);

    // pswLPeaksSorted
    VoC_push16(REG6,DEFAULT);

    // pswCPeaksSorted & pswGPeaksSorted
    VoC_push32(REG01,IN_PARALLEL);

    // swCCThreshold
    VoC_push16(REG5,DEFAULT);

    // pswIntCs & pswIntGs
    VoC_push32(REG23,IN_PARALLEL);

    // sw1 = shr(extract_l(L_mult(swBestIntLag, OS_FCTR)), 1);
    VoC_multf32_rd(REG45,REG4,CONST_6_ADDR);
    // pswLBuf
    VoC_lw16i_set_inc(REG0, LOCAL_PITCHLAGS_pswLBuf_ADDR,1);

    VoC_shr16_ri(REG4,1,DEFAULT);


    // NOTE : pswCIn & pswGIn need to be placed
    //    in different memories !!!
    //
    // INPUT
    // REG0 : pswLout, INC0 = 1 (param3)
    // REG2 : pswCIn,  INC2 = 6 (param1)
    // REG3 : pswGIn,       (param2)
    // REG4 : pswCOut       (param4)
    // REG5 : pswGOut       (param5)
    // REG7 : swFullResLag      (param0)

    // OUTPUT
    // REG7 : return



    // sw1 in REG7
    VoC_movreg16(REG7,REG4,DEFAULT);

    // pswCBuf & pswGBuf
    VoC_lw16i(REG4, LOCAL_PITCHLAGS_pswCBuf_ADDR);
    VoC_lw16i(REG5, LOCAL_PITCHLAGS_pswGBuf_ADDR);


    // push pswCBuf & pswGBuf & pswLBuf
    // pswIntCs & pswIntGs already set
    VoC_push16(REG0,DEFAULT);
    VoC_push32(REG45,DEFAULT);


    VoC_jal(CII_CGInterpValid_new);
    // siNum in REG7


    // INPUT
    // REG0 : pswCframe, INC0 = 1   (param0)
    // REG1 : pswGframe, INC1 = 1   (param1)
    // REG4 : swCmaxSqr     (param2)
    // REG5 : swGmax        (param3)
    // REG7 : siNumPairs        (param4)
    //
    // OUTPUT
    // REG3 : return
    // REG4 : swCmaxSqr
    // REG5 : swGmax
    //
    // used all regs except ACC1, RL6, RL7


    // pswCBuf & pswGBuf
    VoC_lw32_sd(REG01,0,DEFAULT);

    // sw1 & sw2
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i(REG5,0x3f);

    VoC_jal(CII_fnBest_CG_new);
    // siPeakIndex in REG3


    // STACK16                       STACK32
    // --------------------------------------------------
    // RA                            pswCPeaksSorted & pswGPeaksSorted
    // pswLPeaksSorted               pswIntCs & pswIntGs
    // swCCThreshold                 pswCBuf & pswGBuf
    // pswLBuf


    // *psiNumSorted = 0
    VoC_lw16i_short(REG6,0,DEFAULT);

    // if (siPeakIndex == -1) exit
    VoC_bltz16_r(PITCHLAGEXIT,REG3);

//          VoC_jump(PITCHLAGEXIT);



// PITCHLAGOK:

    VoC_lw16i_set_inc(REG0,LOCAL_PITCHLAGS_pswCPeaks_ADDR,1);
    VoC_lw16i_set_inc(REG1,LOCAL_PITCHLAGS_pswGPeaks_ADDR,1);
    VoC_lw16i_set_inc(REG2,LOCAL_PITCHLAGS_pswLPeaks_ADDR,1);

    // pswCBuf & pswGBuf & pswLBuf
    VoC_lw16_sd(REG6,0,DEFAULT);
    VoC_lw32_sd(REG45,0,IN_PARALLEL);

    // push pswCPeaks & pswGPeaks & pswLPeaks
    VoC_push16(REG2,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);

    VoC_jal(CII_siCPeakIndex_save_new);

    // push pswCPeaks++ & pswGPeaks++ & pswLPeaks++
    VoC_push16(REG2,DEFAULT);



    // STACK16                       STACK32
    // --------------------------------------------------
    // RA                            pswCPeaksSorted & pswGPeaksSorted
    // pswLPeaksSorted               pswIntCs & pswIntGs
    // swCCThreshold                 pswCBuf & pswGBuf
    // pswLBuf                       pswCPeaks & pswGPeaks
    // pswLPeaks                     pswCPeaks++ & pswGPeaks++
    // pswLPeaks++


    // pswLPeaks[0] is in REG6
    VoC_multf32_rd(REG67,REG6,TABLE_SP_FRM_psrSubMultFactor_ADDR);
    VoC_push32(REG01,DEFAULT);
    // swSubMult in REG7
    VoC_add32_rd(REG67, REG67, CONST_0x00008000L_ADDR);

    // REG6 for siSMFIndex
    VoC_lw16i_short(REG6,1,DEFAULT);


    // while begin
PITCHLAGS300:

    VoC_blt16_rd(PITCHLAGS200_go, REG7,CONST_21_ADDR);
    VoC_bngt16_rd(PITCHLAGS200_no, REG6, CONST_5_ADDR);
PITCHLAGS200_go:
    VoC_jump(PITCHLAGS200);
PITCHLAGS200_no:

    // INPUT :
    // REG0 : pswCIn, INC0 = 1  (param1)
    // REG1 : pswGIn, INC1 = 1  (param2)
    // REG7 : swSingleResLag    (param0)
    // OUTPUT :
    // REG6 : return
    //
    // USED : all except ACC1, RL6, RL7


    // swSubMult already set
    // pswCBuf & pswGBuf
    VoC_lw32_sd(REG01,3,DEFAULT);

    // siSMFIndex & swSubMult
    VoC_push32(REG67,DEFAULT);

    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswCPeaksSorted & pswGPeaksSorted
    // pswLPeaksSorted         pswIntCs & pswIntGs
    // swCCThreshold           pswCBuf & pswGBuf
    // pswLBuf                 pswCPeaks & pswGPeaks
    // pswLPeaks               pswCPeaks++ & pswGPeaks++
    // pswLPeaks++             siSMFIndex & swSubMult


    VoC_jal(CII_findPeak_new);
    // swFullResPeak in REG6

    VoC_bez16_r(PITCHLAGS400, REG6)


    // NOTE : pswCIn & pswGIn need to be placed
    //    in different memories !!!
    //
    // INPUT
    // REG0 : pswLout, INC0 = 1 (param3)
    // REG2 : pswCIn,  INC2 = 6 (param1)
    // REG3 : pswGIn,       (param2)
    // REG4 : pswCOut       (param4)
    // REG5 : pswGOut       (param5)
    // REG7 : swFullResLag      (param0)

    // OUTPUT
    // REG7 : return


    // swFullResPeak
    VoC_movreg16(REG7,REG6,DEFAULT);

    // pswIntCs & pswIntGs
    VoC_lw32_sd(REG23,4,IN_PARALLEL);

    // pswCBuf & pswGBuf & pswLBuf
    VoC_lw16_sd(REG0,2,DEFAULT);
    VoC_lw32_sd(REG45,3,IN_PARALLEL);

    // set INC2

    VoC_lw16i_short(INC2,6,DEFAULT);

    VoC_jal(CII_CGInterpValid_new);
    // siNum in REG7


    // INPUT
    // REG0 : pswCframe, INC0 = 1   (param0)
    // REG1 : pswGframe, INC1 = 1   (param1)
    // REG4 : swCmaxSqr     (param2)
    // REG5 : swGmax        (param3)
    // REG7 : siNumPairs        (param4)
    //
    // OUTPUT
    // REG3 : return
    // REG4 : swCmaxSqr
    // REG5 : swGmax
    //
    // used all regs except ACC1, RL6, RL7


    // pswCBuf & pswGBuf
    VoC_lw32_sd(REG01,3,DEFAULT);

    // set INC2

    VoC_lw16i_short(INC2,1,DEFAULT);
    // sw1 & sw2
    VoC_lw16_sd(REG4,3,IN_PARALLEL);
    VoC_lw16i(REG5,0x4000);

    VoC_jal(CII_fnBest_CG_new);
    // siPeakIndex in REG3


    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswCPeaksSorted & pswGPeaksSorted
    // pswLPeaksSorted         pswIntCs & pswIntGs
    // swCCThreshold           pswCBuf & pswGBuf
    // pswLBuf                 pswCPeaks & pswGPeaks
    // pswLPeaks               pswCPeaks++ & pswGPeaks++
    // pswLPeaks++             siSMFIndex & swSubMult


    // if (siPeakIndex != -1)
    VoC_bltz16_r(PITCHLAGS400,REG3)


    // pswCBuf & pswGBuf & pswLBuf
    VoC_lw16_sd(REG6,2,DEFAULT);
    VoC_lw32_sd(REG45,3,IN_PARALLEL);

    //  pswCPeaks++ & pswGPeaks++ & pswLPeaks++
    VoC_lw16_sd(REG2,0,DEFAULT);
    VoC_lw32_sd(REG01,1,IN_PARALLEL);

    VoC_jal(CII_siCPeakIndex_save_new);

    //  pswCPeaks++ & pswGPeaks++ & pswLPeaks++
    VoC_sw16_sd(REG2,0,DEFAULT);
    VoC_sw32_sd(REG01,1,IN_PARALLEL);

PITCHLAGS400:


    // siSMFIndex & swSubMult
    VoC_pop32(REG23,DEFAULT);

    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswCPeaksSorted & pswGPeaksSorted
    // pswLPeaksSorted         pswIntCs & pswIntGs
    // swCCThreshold           pswCBuf & pswGBuf
    // pswLBuf                 pswCPeaks & pswGPeaks
    // pswLPeaks               pswCPeaks++ & pswGPeaks++
    // pswLPeaks++

    // &psrSubMultFactor[siSMFIndex]
    VoC_lw16i(REG5,TABLE_SP_FRM_psrSubMultFactor_ADDR);
    VoC_add16_rr(REG5,REG5,REG2,DEFAULT);

    VoC_bnlt16_rd(PITCHLAGS500,REG2,CONST_5_ADDR);

    //swSubMult in REG7
    VoC_multf32_pd(REG67,REG5,LOCAL_PITCHLAGS_pswLPeaks_ADDR);
    VoC_NOP();
    VoC_add32_rd(REG67,REG67,CONST_0x00008000L_ADDR);

PITCHLAGS500:

    // the only case when REG7 does not contain swSubMult
    // is when siSMFIndex == 5, but in this case
    // swSubMult is no longer needed

    VoC_add16_rd(REG6,REG2,CONST_1_ADDR);
    VoC_jump(PITCHLAGS300);


//while end
PITCHLAGS200:


    // &pswLPeaks[siLPeakIndex-1]
    VoC_lw16_sd(REG2,0,DEFAULT);
    VoC_lw16i_short(REG5,5,DEFAULT);
    VoC_sub16_rd(REG2,REG2,CONST_1_ADDR);

    // LMAX_FR & LMIN_FR
    VoC_lw16i(REG4,126);
    VoC_lw16i(REG3,852);

    // siUpperBound = add(pswLPeaks[siLPeakIndex - 1], OS_FCTR)
    VoC_add16_rp(REG6,REG5,REG2,DEFAULT);

    // save *pswPitch default value
    // *pswPitch = LMIN_FR
    VoC_push16(REG4,DEFAULT);


    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswCPeaksSorted & pswGPeaksSorted
    // pswLPeaksSorted         pswIntCs & pswIntGs
    // swCCThreshold           pswCBuf & pswGBuf
    // pswLBuf                 pswCPeaks & pswGPeaks
    // pswLPeaks               pswCPeaks++ & pswGPeaks++
    // pswLPeaks++
    // *pswPitch = LMIN_FR

    VoC_bngt16_rr(PITCHLAGS700,REG6,REG3);
    // siUpperBound = LMAX_FR
    VoC_movreg16(REG6,REG3,DEFAULT);
PITCHLAGS700:

    // siLowerBound = sub(pswLPeaks[siLPeakIndex - 1], OS_FCTR);
    VoC_sub16_pr(REG7,REG2,REG5,DEFAULT);

    VoC_bngt16_rr(PITCHLAGS800,REG4,REG7);
    // siLowerBound = LMIN_FR
    VoC_movreg16(REG7,REG4,DEFAULT);
PITCHLAGS800:


    // siNum = siUpperBound - siLowerBound
    VoC_sub16_rr(REG1,REG6,REG7,DEFAULT);

    // i = siLowerBound
    VoC_movreg16(REG2,REG7,DEFAULT);

    // pswLBuf
    VoC_lw16_sd(REG0,3,IN_PARALLEL);

    // siNum = add(siNum, 1);
    VoC_inc_p(REG1,DEFAULT);

    VoC_loop_r_short(REG1,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG2,REG0,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_endloop0;


    // NOTE : pswCIn & pswGIn need to be placed
    //    in different memories !!!
    //
    // INPUT
    // REG0 : pswLIn, INC0 = 1  (param0)
    // REG1 : siNum,  INC1 = 1  (param1)
    // REG2 : pswCIn, INC2 = 6  (param2)
    // REG3 : pswGIn,       (param3)
    // REG5 : siLoIntLag        (param4)
    // REG6 : pswCOut       (param5)
    // REG7 : pswGOut       (param6)



    // set INC2

    VoC_lw16i_short(INC2,6,DEFAULT);
    // pswLBuf
    VoC_lw16_sd(REG0,3,DEFAULT);
    // pswIntCs & pswIntGs
    VoC_lw32_sd(REG23,3,IN_PARALLEL);
    // LSMIN
    VoC_lw16i_short(REG5,18,DEFAULT);
    // pswCBuf & pswGBuf
    VoC_lw32_sd(REG67,2,IN_PARALLEL);
    // push siNum
    VoC_push16(REG1,DEFAULT);

    VoC_jal(CII_CGInterp_new);

    // INPUT
    // REG0 : pswCframe, INC0 = 1   (param0)
    // REG1 : pswGframe, INC1 = 1   (param1)
    // REG4 : swCmaxSqr     (param2)
    // REG5 : swGmax        (param3)
    // REG7 : siNumPairs        (param4)
    //
    // OUTPUT
    // REG3 : return
    // REG4 : swCmaxSqr
    // REG5 : swGmax
    //
    // used all regs except ACC1, RL6, RL7


    // siNum
    VoC_pop16(REG7,DEFAULT);

    // pswCBuf & pswGBuf
    VoC_lw32_sd(REG01,2,IN_PARALLEL);

    // sw1 & sw2
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i(REG5,0x3f);

    VoC_jal(CII_fnBest_CG_new);
    // siPeakIndex in REG3

    // swCPitch & swGPitch
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i(REG5,0x3f);


    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswCPeaksSorted & pswGPeaksSorted
    // pswLPeaksSorted         pswIntCs & pswIntGs
    // swCCThreshold           pswCBuf & pswGBuf
    // pswLBuf                 pswCPeaks & pswGPeaks
    // pswLPeaks               pswCPeaks++ & pswGPeaks++
    // pswLPeaks++
    // *pswPitch = LMIN_FR


    VoC_bltz16_r(PITCHLAGSYOU,REG3);

    // pswCBuf & pswGBuf & pswLBuf
    VoC_lw16_sd(REG6,3,DEFAULT);
    VoC_lw32_sd(REG45,2,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG3,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG3,IN_PARALLEL);
    VoC_add16_rr(REG6,REG6,REG3,DEFAULT);

    VoC_lw16_p(REG4,REG4,DEFAULT);
    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_lw16_p(REG5,REG5,DEFAULT);


    // *pswPitch = pswLBuf[siPeakIndex]
    VoC_sw16_sd(REG6,0,DEFAULT);

PITCHLAGSYOU:

    // swMult
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_shr16_ri(REG0,-1,DEFAULT);


    // swMultInt in REG7
    VoC_multf32_rd(REG67,REG0,CONST_0x1555_ADDR);
    // push swCPitch & swGPitch
    VoC_push32(REG45,DEFAULT);

    VoC_add32_rd(REG67,REG67,CONST_0x00008000L_ADDR);




    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswCPeaksSorted & pswGPeaksSorted
    // pswLPeaksSorted         pswIntCs & pswIntGs
    // swCCThreshold           pswCBuf & pswGBuf
    // pswLBuf                 pswCPeaks & pswGPeaks
    // pswLPeaks               pswCPeaks++ & pswGPeaks++
    // pswLPeaks++             swCPitch & swGPitch
    // *pswPitch


PITCHLAGSSTART: //while start

    VoC_bngt16_rd(PITCHLAGSYIYI_go,REG7,CONST_142_ADDR)
    VoC_jump(PITCHLAGSYIYI)
PITCHLAGSYIYI_go:

    // INPUT :
    // REG0 : pswCIn, INC0 = 1  (param1)
    // REG1 : pswGIn, INC1 = 1  (param2)
    // REG7 : swSingleResLag    (param0)
    // OUTPUT :
    // REG6 : return
    //
    // USED : all except ACC1, RL6, RL7


    // pswIntCs & pswIntGs
    VoC_lw32_sd(REG01,4,DEFAULT);

    // push swMult
    VoC_push16(REG0,IN_PARALLEL);


    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswCPeaksSorted & pswGPeaksSorted
    // pswLPeaksSorted         pswIntCs & pswIntGs
    // swCCThreshold           pswCBuf & pswGBuf
    // pswLBuf                 pswCPeaks & pswGPeaks
    // pswLPeaks               pswCPeaks++ & pswGPeaks++
    // pswLPeaks++             swCPitch & swGPitch
    // *pswPitch
    // swMult


    VoC_jal(CII_findPeak_new);
    // swFullResPeak in REG6

    VoC_bez16_r(PITCHLAGSDA400,REG6)

    // NOTE : pswCIn & pswGIn need to be placed
    //    in different memories !!!
    //
    // INPUT
    // REG0 : pswLout, INC0 = 1 (param3)
    // REG2 : pswCIn,  INC2 = 6 (param1)
    // REG3 : pswGIn,       (param2)
    // REG4 : pswCOut       (param4)
    // REG5 : pswGOut       (param5)
    // REG7 : swFullResLag      (param0)

    // OUTPUT
    // REG7 : return

    VoC_movreg16(REG7,REG6,DEFAULT);

    // pswIntCs & pswIntGs
    VoC_lw32_sd(REG23,4,IN_PARALLEL);

    // pswCBuf & pswGBuf & pswLBuf
    VoC_lw16_sd(REG0,4,DEFAULT);
    VoC_lw32_sd(REG45,3,IN_PARALLEL);

    // set INC2

    VoC_lw16i_short(INC2,6,DEFAULT);

    VoC_jal(CII_CGInterpValid_new);
    // siNum in REG7

    // REG0 : pswCframe, INC0 = 1   (param0)
    // REG1 : pswGframe, INC1 = 1   (param1)
    // REG4 : swCmaxSqr     (param2)
    // REG5 : swGmax        (param3)
    // REG7 : siNumPairs        (param4)
    //
    // OUTPUT
    // REG3 : return
    // REG4 : swCmaxSqr
    // REG5 : swGmax
    //
    // used all regs except ACC1, RL6, RL7


    // pswCBuf & pswGBuf
    VoC_lw32_sd(REG01,3,DEFAULT);

    // set INC2

    VoC_lw16i_short(INC2,1,DEFAULT);
    // sw1 & sw2
    VoC_lw16_sd(REG4,5,IN_PARALLEL);
    VoC_lw16i(REG5,0x4000);

    VoC_jal(CII_fnBest_CG_new);
    // siPeakIndex in REG3


    // if (siPeakIndex != -1)
    VoC_bltz16_r(PITCHLAGSDA400,REG3)


    // pswCBuf & pswGBuf & pswLBuf
    VoC_lw16_sd(REG6,4,DEFAULT);
    VoC_lw32_sd(REG45,3,IN_PARALLEL);

    //  pswCPeaks++ & pswGPeaks++ & pswLPeaks++
    VoC_lw16_sd(REG2,2,DEFAULT);
    VoC_lw32_sd(REG01,1,IN_PARALLEL);

    VoC_jal(CII_siCPeakIndex_save_new);

    //  pswCPeaks++ & pswGPeaks++ & pswLPeaks++
    VoC_sw16_sd(REG2,2,DEFAULT);
    VoC_sw32_sd(REG01,1,IN_PARALLEL);

PITCHLAGSDA400:

    // swMult
    VoC_pop16(REG0,DEFAULT);

    //*pswPitch
    VoC_lw16_sd(REG1,0,DEFAULT);

    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);
    VoC_multf32_rd(REG67,REG0,CONST_0x1555_ADDR);
    VoC_NOP();
    VoC_add32_rd(REG67,REG67,CONST_0x00008000L_ADDR);


    VoC_jump(PITCHLAGSSTART);

PITCHLAGSYIYI: //WHILEEND


    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswCPeaksSorted & pswGPeaksSorted
    // pswLPeaksSorted         pswIntCs & pswIntGs
    // swCCThreshold           pswCBuf & pswGBuf
    // pswLBuf                 pswCPeaks & pswGPeaks
    // pswLPeaks               pswCPeaks++ & pswGPeaks++
    // pswLPeaks++             swCPitch & swGPitch
    // *pswPitch


    VoC_pop32(REG01,DEFAULT);


    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswCPeaksSorted & pswGPeaksSorted
    // pswLPeaksSorted         pswIntCs & pswIntGs
    // swCCThreshold           pswCBuf & pswGBuf
    // pswLBuf                 pswCPeaks & pswGPeaks
    // pswLPeaks               pswCPeaks++ & pswGPeaks++
    // pswLPeaks++
    // *pswPitch


    // paras:  REG2 (y);
    // return: REG0 (exp);
    // used :  REG0 & REG2

    VoC_movreg16(REG2,REG0,DEFAULT);

    // -si2 in REG0
    // sw1 in REG2
    VoC_jal(CII_norm_s_opt);

    // -si2 in REG3,
    VoC_movreg16(REG3,REG0,DEFAULT);

    // L_1 = L_mult(sw1, PW_FRAC);
    VoC_multf32_rd(ACC0,REG2,CONST_0x3333_ADDR);//L_1 in ACC0

    // swGPitch in REG2
    VoC_movreg16(REG2,REG1,DEFAULT);

    // -si1 in REG0
    // sw1 in REG2
    VoC_jal(CII_norm_s_opt);

    // sw2 = round(L_shr(L_1, 1));
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_add32_rd(REG45,ACC0,CONST_0x00008000L_ADDR);

    // -sub(si1, si2)
    VoC_sub16_rr(REG3,REG0,REG3,DEFAULT);

    // sw2 = divide_s(sw2, sw1);
    VoC_movreg16(REG0,REG5,DEFAULT);
    VoC_movreg16(REG1,REG2,IN_PARALLEL);


    // INPUT :
    // REG0 : Nominator (param0)
    // REG1 : Denominator (param1)
    // OUTPUT
    // REG2 : return
    //
    // USED REGS : RL6, RL7


    VoC_jal(CII_DIV_S);
    // sw2 in REG2

    // *psiNumSorted = 0
    // siLPeaksSortedInit is always = 0
    // siSortedIndex is always = *psiNumSorted
    VoC_lw16i_short(REG6,0,DEFAULT);


    VoC_bnltz16_r(PITCHLAGSSUN,REG3);
    // shl(sw2, sub(si1, si2)
    VoC_shr16_rr(REG2,REG3,DEFAULT);
PITCHLAGSSUN:


    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_bngtz16_r(PITCHLAGSDAO,REG3);

    // swRnd = shl(var1, var2 + 1) & 0x1;
    // sw2 = add(shl(var1, var2), swRnd);
    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_movreg16(REG1,REG2,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG3,REG5,DEFAULT);

    VoC_shr16_rr(REG1,REG4,DEFAULT);
    VoC_shr16_rr(REG2,REG3,IN_PARALLEL);

    VoC_and16_rr(REG1,REG5,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG1,DEFAULT);

PITCHLAGSDAO:

    // siNumPeaks
    VoC_lw16_sd(REG1,1,DEFAULT);
    VoC_lw16_sd(REG0,2,DEFAULT);
    VoC_sub16_rr(REG7,REG1,REG0,DEFAULT);

    // *pswHNWCoef
    VoC_push16(REG2,DEFAULT);

    // siNumPeaks & *psiNumSorted
    VoC_push32(REG67,DEFAULT);

    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswCPeaksSorted & pswGPeaksSorted
    // pswLPeaksSorted         pswIntCs & pswIntGs
    // swCCThreshold           pswCBuf & pswGBuf
    // pswLBuf                 pswCPeaks & pswGPeaks
    // pswLPeaks               pswCPeaks++ & pswGPeaks++
    // pswLPeaks++             *psiNumSorted & siNumPeaks
    // *pswPitch
    // *pswHNWCoef


    VoC_loop_r(1,REG7)


    // INPUT
    // REG0 : pswCframe, INC0 = 1   (param0)
    // REG1 : pswGframe, INC1 = 1   (param1)
    // REG4 : swCmaxSqr     (param2)
    // REG5 : swGmax        (param3)
    // REG7 : siNumPairs        (param4)
    //
    // OUTPUT
    // REG3 : return
    // REG4 : swCmaxSqr
    // REG5 : swGmax
    //
    // used all regs except ACC1, RL6, RL7


    // pswCPeaks & pswGPeaks
    VoC_lw32_sd(REG01,2,DEFAULT);

    // sw1 & sw2
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i(REG5,0x3f);

    VoC_jal(CII_fnBest_CG_new);
    // siPeakIndex in REG3

    // & pswLPeaks[siPeakIndex]
    VoC_lw16_sd(REG0,3,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);

    // siRepeat = 0;
    VoC_lw16i_short(REG4,0,DEFAULT);

    // swWorkingLag = pswLPeaks[siPeakIndex];
    VoC_lw16_p(REG5,REG0,DEFAULT);

    // *psiNumSorted & siNumPeaks
    VoC_lw32_sd(REG67,0,IN_PARALLEL);

    // & pswCPeaks[siPeakIndex]
    // & pswGPeaks[siPeakIndex]
    VoC_lw32_sd(REG01,2,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG3,IN_PARALLEL);

    // pswLPeaksSorted
    VoC_lw16_sd(REG0,6,DEFAULT);

    exit_on_warnings=OFF;
    VoC_push32(REG01,DEFAULT);
    exit_on_warnings=ON;

    // siRepeat in REG3
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_bngtz16_r(PITCHLAGSKIPLOOP,REG6)

    VoC_loop_r_short(REG6,DEFAULT)
    VoC_lw16inc_p(REG1,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_bne16_rr(PITCHLAG_FLAG_TO_1,REG1,REG5);
    VoC_lw16i_short(REG3,1,DEFAULT);
PITCHLAG_FLAG_TO_1:
    VoC_lw16inc_p(REG1, REG0, DEFAULT);
    VoC_endloop0

PITCHLAGSKIPLOOP:


    VoC_pop32(REG01,DEFAULT);


    // if (!siRepeat)
    VoC_bnez16_r(PITCHLAGNIU,REG3)

    // & pswLPeaksSorted[siSortedIndex]
    VoC_lw16_sd(REG4,6,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);

    // & pswCPeaksSorted[siSortedIndex]
    // & pswGPeaksSorted[siSortedIndex]
    VoC_lw32_sd(REG23,5,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG6,IN_PARALLEL);

    // pswLPeaksSorted = swWorkingLag
    VoC_sw16_p(REG5,REG4,DEFAULT);

    // pswCPeaks[siPeakIndex]
    // pswGPeaks[siPeakIndex]
    VoC_lw16_p(REG4,REG0,DEFAULT);
    VoC_lw16_p(REG5,REG1,DEFAULT);

    // pswCPeaksSorted[siSortedIndex] = pswCPeaks[siPeakIndex];
    // pswGPeaksSorted[siSortedIndex] = pswGPeaks[siPeakIndex];
    VoC_sw16_p(REG4,REG2,DEFAULT);
    VoC_sw16_p(REG5,REG3,DEFAULT);

    // *psiNumSorted = add(*psiNumSorted, 1), here REG7 == 1
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR); //REG7,IN_PARALLEL);

PITCHLAGNIU:

    VoC_lw16i_short(REG4,0,DEFAULT);

    // save *psiNumSorted
    VoC_sw32_sd(REG67,0,DEFAULT);

    // pswCPeaks[siPeakIndex] = 0x0
    VoC_sw16_p(REG4,REG0,DEFAULT);

    VoC_endloop1


    // *psiNumSorted in REG7
    VoC_pop16(REG7,DEFAULT);
    // *pswHNWCoef in REG5
    VoC_pop32(REG45,IN_PARALLEL);
    // *pswPitch in REG6
    VoC_pop16(REG6,DEFAULT);


PITCHLAGEXIT:

    VoC_lw16_d(SP16,PITCHLAG_SP16_SAVE_ADDR);
    VoC_lw16_d(SP32,PITCHLAG_SP32_SAVE_ADDR);

    VoC_pop16(RA,DEFAULT);
    VoC_movreg16(REG5,REG4,DEFAULT);

    VoC_return;
}



/*******************************************/

//openLoopLagSearch
//pswWSpeech---   GLOBAL_EN_pswWgtSpeechSpace_ADDR  //&pswWgtSpeechSpace[LSMAX]
//swPrevR0Index---GLOBAL_EN_swOldR0Index_ADDR
//swCurrR0Index---LOCAL_EN_SPEECHENCODER_iR0_ADDR
//psiUVCode---    LOCAL_EN_SPEECHENCODER_siUVCode_ADDR
//pswLagList---   LOCAL_EN_SPEECHENCODER_pswLagList_ADDR
//pswNumLagList---LOCAL_EN_SPEECHENCODER_pswNumLagList_ADDR
//pswPitchBuf---  LOCAL_EN_SPEECHENCODER_pswPitchBuf_ADDR
//pswHNWCoefBuf---LOCAL_EN_SPEECHENCODER_pswHNWCoefBuf_ADDR
//psnsWSfrmEng--- GLOBAL_EN_psnsWSfrmEngSpace_ADDR     &psnsWSfrmEngSpace[N_SUB];
//pswVadLags---   LOCAL_EN_SPEECHENCODER_pswVadLags_ADDR
//swSP---         GLOBAL_OUTPUT_SP_ADDR


void CII_openLoopLagSearch_new(void)
{

    // REG INPUT:
    // ---------
    // REG0 : swPrevR0Index
    // REG1 : swCurrR0Index
    // REG2 : pswLagList
    // REG3 : pswNumLagList
    // REG4 : pswPitchBuf
    // REG5 : pswHNWCoefBuf
    // REG6 : pswVadLags
    // REG7 : swSP
    //
    // GLOBAL INPUT:
    // ------------
    // GLOBAL_EN_pswWgtSpeechSpace_ADDR : &pswWSpeech[-145]
    // GLOBAL_EN_psnsWSfrmEngSpace_ADDR : &psnsWSfrmEng[-4]
    //
    // OUTPUT:
    // ------
    // REG0 : *psiUVCode

#if 0

    voc_short   LOCAL_OPENLOOPSEARCH_pswGFrame_ADDR             ,248,x     //[248]shorts
    voc_short   LOCAL_OPENLOOPSEARCH_pswScaledWSpeechBuffer_ADDR    ,306,x     //[306]shorts
    voc_short   LOCAL_OPENLOOPSEARCH_pswLPeaks_ADDR             ,14,x      //[14]shorts
    voc_short   LOCAL_OPENLOOPSEARCH_pswCPeaks_ADDR             ,14,x      //[14]shorts
    voc_short   LOCAL_OPENLOOPSEARCH_pswGPeaks_ADDR             ,14,x      //[14]shorts
    voc_short   LOCAL_OPENLOOPSEARCH_pswLArray_ADDR             ,16,x      //[16]shorts
#endif


#if 0
    voc_short   LOCAL_OPENLOOPSEARCH_pswCFrame_ADDR             ,512,y      //[512]shorts
    voc_short   LOCAL_OPENLOOPSEARCH_pswSfrmEng_ADDR            ,4,y         //[4]shorts
    voc_short   LOCAL_OPENLOOPSEARCH_pswLIntBuf_ADDR            ,4,y         //[4]shorts
    voc_short   LOCAL_OPENLOOPSEARCH_pswCCThresh_ADDR           ,4,y         //[4]shorts
    voc_short   OPENLOOP_SP32_SAVE_ADDR                         ,y       //1 short
    voc_short   OPENLOOP_SP16_SAVE_ADDR                         ,y       //1 short
    voc_short   LOCAL_OPENLOOPSEARCH_ppswTrajLBuf_ADDR          ,32,y        //[32]shorts
    voc_short   LOCAL_OPENLOOPSEARCH_ppswTrajCCBuf_ADDR         ,32,y        //[32]shorts
    voc_short   LOCAL_OPENLOOPSEARCH_ppswTrajGBuf_ADDR          ,32,y        //[32]shorts

    voc_short   LOCAL_OPENLOOPSEARCH_pswScaledWSpeechBufferCopy_ADDR        ,306,y
#endif



    VoC_push16(RA, DEFAULT);

    VoC_sw16_d(SP32,OPENLOOP_SP32_SAVE_ADDR);
    VoC_sw16_d(SP16,OPENLOOP_SP16_SAVE_ADDR);

    // we will group :
    // pswLagList & pswNumLagList
    // pswPitchBuf & pswHNWCoefBuf
    // because they are always used by pairs in the functions

    VoC_push16(REG6,DEFAULT);
    VoC_push32(REG23,IN_PARALLEL);
    VoC_push16(REG7,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);


    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // pswVadLags              pswPitchBuf & pswHNWCoefBuf
    // swSP


    // CII_r0BasedEnergyShft
    // ---------------------
    // input  REG4 : swR0Index
    // output REG5 : swShiftDownSignal
    // used register   REG5 REG4


    // r0BasedEnergyShft(swCurrR0Index)
    VoC_movreg16(REG4,REG1,DEFAULT);

    VoC_jal(CII_r0BasedEnergyShft_new);
    // return in REG5

    // r0BasedEnergyShft(swPrevR0Index)
    VoC_movreg16(REG4,REG0,DEFAULT);
    VoC_push16(REG5,DEFAULT);

    VoC_jal(CII_r0BasedEnergyShft_new);
    // return in REG5


    // CII_g_corr1s
    // ------------
    // INPUT :
    // REG0 : pswIn,    INC0 = 2
    // REG5 : swEngyRShft
    //
    // OUTPUT
    // REG7 : swEngyLShft
    // ACC0 : *pL_out
    //
    // USED : all except REG23, ACC1, RL6, RL7

    // &pswWSpeech[0]
    VoC_lw16i_set_inc(REG0,GLOBAL_EN_pswWgtSpeechSpace_ADDR+145,2);

    // &psnsWSfrmEng[0].man
    VoC_lw16i_set_inc(REG2,GLOBAL_EN_psnsWSfrmEngSpace_ADDR+8,1);

    // &psnsWSfrmEng[-4].man
    VoC_lw16i_set_inc(REG3,GLOBAL_EN_psnsWSfrmEngSpace_ADDR,2);




    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // pswVadLags              pswPitchBuf & pswHNWCoefBuf
    // swSP
    // r0BasedEnergyShft(Curr)


    VoC_loop_i(1,4)


    VoC_jal(CII_g_corr1s_new);
    // L_WSfrmEng in ACC0
    // return in REG7
    // &pswWSpeech[i*40] in REG0

    // added_after_debug
    // after the CII_g_corr1s_new, the reg0 is augmented by 42 instead of 40
    // because we have done a VoC_aligninc_p
    VoC_sub16_rd(REG0,REG0,CONST_2_ADDR);

    // round
    VoC_add32_rd(REG45,ACC0,CONST_0x00008000L_ADDR);

    // p.man(old)
    VoC_lw16_p(REG4,REG2,DEFAULT);

    // p.man = round
    VoC_sw16inc_p(REG5,REG2,DEFAULT);

    // p.sh(old)
    VoC_lw16_p(REG5,REG2,DEFAULT);

    // p.sh = swEngyLShft
    VoC_sw16inc_p(REG7,REG2,DEFAULT);

    // p(old)
    // psnsWSfrmEng[i - N_SUB] = psnsWSfrmEng[i];
    VoC_sw32inc_p(REG45,REG3,DEFAULT);

    // r0BasedEnergyShft(swCurrR0Index)
    VoC_lw16_sd(REG5,0,DEFAULT);


    VoC_endloop1

    // &psnsWSfrmEng[-4].man
    VoC_lw16i_set_inc(REG0,GLOBAL_EN_psnsWSfrmEngSpace_ADDR,2);

    // snsMax in REG67
    VoC_lw32z(REG67,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,DEFAULT);

//  VoC_loop_i_short(8,DEFAULT);
//  VoC_startloop0
//      // if (psnsWSfrmEng[i].man > 0)
//      VoC_bngtz16_r(OPENLOOP100,REG4);
//        VoC_bnez16_r(OPENLOOP200,REG6);
//          VoC_movreg32(REG67,REG45,DEFAULT);
//OPENLOOP200:
//      VoC_bngt16_rr(OPENLOOP300,REG7,REG5);
//          VoC_movreg32(REG67,REG45,DEFAULT);
//OPENLOOP300:
//        VoC_bne16_rr(OPENLOOP100,REG7,REG5);
//      VoC_bngt16_rr(OPENLOOP100,REG4,REG6);
//          VoC_movreg32(REG67,REG45,DEFAULT);
//OPENLOOP100:
//      VoC_lw32inc_p(REG45,REG0,DEFAULT);
//  VoC_endloop0


    // L_G in ACC0
    // code rearrangement for NAD reduction

    VoC_loop_i_short(8,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0

    // if (psnsWSfrmEng[i].man > 0)
    VoC_bngtz16_r(OPENLOOP_LOOP,REG4);

    // if (REG6 == 0)
    VoC_bez16_r(OPENLOOP_MOVE,REG6);

    // else if (REG7 > REG5)
    VoC_bgt16_rr(OPENLOOP_MOVE,REG7,REG5);

    // else if ((REG7 == REG5) && (REG4 > REG6))
    VoC_bne16_rr(OPENLOOP_LOOP,REG7,REG5);
    VoC_bngt16_rr(OPENLOOP_LOOP,REG4,REG6);
OPENLOOP_MOVE:
    VoC_movreg32(REG67,REG45,DEFAULT);
OPENLOOP_LOOP:

    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_endloop0


    // siShift = sub(shr(snsMax.sh, 1), 1);
    VoC_shr16_ri(REG7,1, DEFAULT);

    // -siShift in REG7
    VoC_sub16_dr(REG7,CONST_1_ADDR,REG7);

    // &pswScaledWSpeech[-145] = pswScaledWSpeechBuffer
    VoC_lw16i_set_inc(REG1,LOCAL_OPENLOOPSEARCH_pswScaledWSpeechBuffer_ADDR,2);

    // &pswWSpeech[-145]
    VoC_lw16i_set_inc(REG0,GLOBAL_EN_pswWgtSpeechSpace_ADDR,2);

    // push pswScaledWSpeechBuffer
    VoC_sw16_sd(REG1,0,DEFAULT);

    // preload first speech sample
    VoC_lw32inc_p(REG45,REG0,DEFAULT);

    // code rearrangement for NAD reduction
    VoC_lw32z(ACC1,IN_PARALLEL);

    // short not possible
    VoC_loop_i(0,153)
    VoC_shr16_rr(REG4,REG7,DEFAULT);
    VoC_shr16_rr(REG5,REG7,IN_PARALLEL);
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    // write is before reg update
    exit_on_warnings=OFF;
    VoC_sw32inc_p(REG45,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0


    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // pswVadLags              pswPitchBuf & pswHNWCoefBuf
    // swSP
    // pswScaledWSpeechBuffer


    // &pswScaledWSpeech[-145] = pswScaledWSpeechBuffer
    VoC_lw16_sd(REG0,0,DEFAULT);

    VoC_loop_i_short(20,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG0);
    VoC_endloop0

    // &pswGFrame[G_FRAME_LEN - 1]
    VoC_lw16i_set_inc(REG2,LOCAL_OPENLOOPSEARCH_pswGFrame_ADDR+247,-1);

    // &pswScaledWSpeech[-145+40]
    VoC_lw16i_set_inc(REG1,LOCAL_OPENLOOPSEARCH_pswScaledWSpeechBuffer_ADDR+40,2);

    // &pswScaledWSpeech[-145] = pswScaledWSpeechBuffer
    VoC_lw16_sd(REG0,0,DEFAULT);

    // We will use here a two parallel mac technique.
    // For this we will use both accumulators, computing on ACC1 an
    // advance of 1 on the addresses compared to ACC0. No read
    // conflict will appear because the initial read pointers are even

    // short not possible
    VoC_loop_i(0,124)

    VoC_msu32inc_pp(REG0,REG0,DEFAULT);
    VoC_msu32inc_pp(REG0,REG0,IN_PARALLEL);

    VoC_mac32inc_pp(REG1,REG1,DEFAULT);
    VoC_mac32inc_pp(REG1,REG1,IN_PARALLEL);

    // because of the pipe, ACCO is still
    // not affected by the mac/msu sequence
    // pswGFrame[G_FRAME_LEN - 1 - 2*i] = extract_h(L_G)
    exit_on_warnings=OFF;
    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);
    exit_on_warnings=ON;
    // at this point:
    // ACCO : pswGFrame[G_FRAME_LEN - LSMAX - 2 - 2*i]
    // ACC1 : pswGFrame[G_FRAME_LEN - LSMAX - 2 - 2*i - 1] -
    //        pswGFrame[G_FRAME_LEN - LSMAX - 2 - 2*i]

    VoC_lw32z(ACC1,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,IN_PARALLEL);

    // because of the pipe, ACC0 is still not
    // affected by the += ACC1
    // pswGFrame[G_FRAME_LEN - 2 - 2*i] = extract_h(L_G)
    exit_on_warnings=OFF;
    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);
    exit_on_warnings=ON;

    VoC_endloop0


    // We won't use the following pointer table,
    // but directly construct the equivalent addresses
    // ppswGSfrm[0] = pswGFrame + 3 * S_LEN;
    // ppswGSfrm[1] = pswGFrame + 2 * S_LEN;
    // ppswGSfrm[2] = pswGFrame + S_LEN;
    // ppswGSfrm[3] = pswGFrame;


    VoC_lw16i_set_inc(REG0,LOCAL_OPENLOOPSEARCH_pswGFrame_ADDR+102,-40);
    VoC_lw16i_set_inc(REG1,LOCAL_OPENLOOPSEARCH_pswSfrmEng_ADDR,1);
    VoC_lw16i_set_inc(REG2,LOCAL_OPENLOOPSEARCH_pswScaledWSpeechBuffer_ADDR+265,2);

    // save &pswSfrmEng[0]
    VoC_sw16_sd(REG1,0,DEFAULT);

    // save &pswSfrmEng[0]
    VoC_push16(REG1,DEFAULT);


    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // pswVadLags              pswPitchBuf & pswHNWCoefBuf
    // swSP
    // &pswSfrmEng
    // &pswSfrmEng


    VoC_loop_i_short(3,DEFAULT)
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0


    VoC_aligninc_pp(REG2,REG2,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_loop_i_short(20,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_pp(REG2,REG2);
    VoC_endloop0



    // Here we have a critical loop with convolution.
    // If we do not use the VoC specific align and copy features
    // the loop cycles are about 4*128*40*2 = 40960 cycles
    // If we use copy + align, the loop cycles are about
    // 4*128*20*1 = 10240, that is 4 times less

    // 1 : copy pswScaledWSpeech in Y
    VoC_lw16i_set_inc(REG2,LOCAL_OPENLOOPSEARCH_pswScaledWSpeechBuffer_ADDR,2);
    VoC_lw16i_set_inc(REG3,LOCAL_OPENLOOPSEARCH_pswScaledWSpeechBufferCopy_ADDR,2);

    VoC_lw32inc_p(ACC1,REG2,DEFAULT);
    VoC_loop_i(0,153)
    VoC_lw32inc_p(ACC1,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC1,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    // pswCFrame
    VoC_lw16i_set_inc(REG2,LOCAL_OPENLOOPSEARCH_pswCFrame_ADDR,1);
    // 1 in REG6
    VoC_lw16i_short(REG6,1,DEFAULT);
    // 40 in REG
    VoC_lw16i(REG3,40);



    // pswSfrmEng[3] = extract_h(L_WSfrmEng)
    VoC_sw16_p(ACC0_HI,REG1,DEFAULT);

    // &pswScaledWSpeech[0]
    VoC_lw16i_set_inc(REG0,LOCAL_OPENLOOPSEARCH_pswScaledWSpeechBuffer_ADDR+145,2);

    // &pswScaledWSpeech[-LSMIN]
    VoC_lw16i_set_inc(REG1,LOCAL_OPENLOOPSEARCH_pswScaledWSpeechBufferCopy_ADDR+127,2);

    // i in REG7
    VoC_lw16i_short(REG7,0,DEFAULT);

OPENLOOP302:
    // save pswScaledWSpeech & pcopy for future use
    VoC_push32(REG01,DEFAULT);

    VoC_loop_i(1,128)

    // save pswScaledWSpeech & --pcopy for future use
    VoC_sub16_rr(REG5,REG1,REG6,DEFAULT);
    VoC_movreg16(REG4,REG0,IN_PARALLEL);

    // align pointers
    VoC_aligninc_pp(REG0,REG1,DEFAULT);

    VoC_loop_i_short(20,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG1);
    VoC_endloop0

    VoC_NOP();
    // restitute pointers
    VoC_movreg32(REG01,REG45,DEFAULT);
    // *pswCFrame++ = extract_h(L_C)
    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);

    VoC_endloop1

    // restitute pointers
    VoC_pop32(REG01,DEFAULT);

    // increment i
    VoC_add16_rr(REG7,REG7,REG6,IN_PARALLEL);

    // add 40 to both
    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG3,IN_PARALLEL);

    VoC_bngt16_rd(OPENLOOP302,REG7,CONST_3_ADDR);


    // We won't use the following pointer table,
    // but directly construct the equivalent addresses
    // ppswCSfrm[0] = pswCFrame;
    // ppswCSfrm[1] = pswCFrame + CG_TERMS;
    // ppswCSfrm[2] = pswCFrame + 2 * CG_TERMS;
    // ppswCSfrm[3] = pswCFrame + 3 * CG_TERMS;



    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // pswVadLags              pswPitchBuf & pswHNWCoefBuf
    // swSP
    // pswSfrmEng
    // pswSfrmEng


    // pswLIntBuf
    VoC_lw16i_set_inc(REG2,LOCAL_OPENLOOPSEARCH_pswLIntBuf_ADDR,1);
    // pswCCThresh
    VoC_lw16i_set_inc(REG3,LOCAL_OPENLOOPSEARCH_pswCCThresh_ADDR,1);
    // &ppswCSfrm[0][LMIN - LSMIN]
    VoC_lw16i_set_inc(REG0,LOCAL_OPENLOOPSEARCH_pswCFrame_ADDR+3,1);
    // &ppswGSfrm[0][LMIN - LSMIN]
    VoC_lw16i_set_inc(REG1,LOCAL_OPENLOOPSEARCH_pswGFrame_ADDR+120+3,1);

    // push pswCCThresh
    VoC_push16(REG3,DEFAULT);

    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // pswVadLags              pswPitchBuf & pswHNWCoefBuf
    // swSP
    // pswSfrmEng
    // pswSfrmEng
    // pswCCThresh

    // push again pswLIntBuf & pswCCThresh
    VoC_push32(REG23,IN_PARALLEL);

    // swBestPG = 0
    VoC_lw16i_short(REG7,0,DEFAULT);

    // pswVadLags
    VoC_lw16_sd(REG3,4,IN_PARALLEL);



    // push pointer values on the stack
    VoC_push32(REG01,DEFAULT);

    VoC_push32(REG23,DEFAULT);
    VoC_push16(REG7,IN_PARALLEL);

    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // pswVadLags              pswPitchBuf & pswHNWCoefBuf
    // swSP                    pswLIntBuf & pswCCThresh
    // pswSfrmEng              &ppC[0][LMIN - LSMIN] & &ppG[0][LMIN - LSMIN]
    // pswSfrmEng              pswLIntBuf & pswVadLags
    // pswCCThresh
    // swBestPG


    VoC_loop_i(1,4)

    // INPUT
    // REG0 : pswCframe, INC0 = 1   (param0)
    // REG1 : pswGframe, INC1 = 1   (param1)
    // REG4 : swCmaxSqr     (param2)
    // REG5 : swGmax        (param3)
    // REG7 : siNumPairs        (param4)
    //
    // OUTPUT
    // REG3 : return
    // REG4 : swCmaxSqr
    // REG5 : swGmax
    //
    // used all regs except ACC1, RL6, RL7

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i(REG5,0X3f);
    VoC_lw16i(REG7,122);

    // pointers in REG01 set once before the loop start
    // and then at the end of the loop

    VoC_jal(CII_fnBest_CG_new);
    // siIndex in REG3
    // swCCMax in REG4
    // swGmax in REG5

    // pswLIntBuf & pswVadLags
    VoC_pop32(REG01,DEFAULT);

    // value for pswLIntBuf[i]
    VoC_lw16i_short(REG6,0,DEFAULT);
    // value for pswVadLags[i]
    VoC_lw16i_short(REG7,21,IN_PARALLEL);

    VoC_bltz16_r(OPENLOOP303,REG3)

    VoC_add16_rr(REG6,REG3,REG7,DEFAULT);
    VoC_add16_rr(REG7,REG3,REG7,DEFAULT);
OPENLOOP303:
    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);

    // because of pipe structure
    // ++ operation is done before store
    VoC_push32(REG01,DEFAULT);

    // preload value for pswCCThresh[i] with 0
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    // pswLIntBuf[i] = 0 if siIndex == -1
    // else is positive (at least 21)

    VoC_bez16_r(OPENLOOP305_bis,REG6)

    // swCCDivG
    VoC_lw16i(REG2,0X7fff);

    VoC_bngt16_rr(OPENLOOP304,REG5,REG4);

    // INPUT :
    // REG0 : Nominator (param0)
    // REG1 : Denominator (param1)
    // OUTPUT
    // REG2 : return
    //
    // USED REGS : RL6, RL7

    // swCCMax, swGMax
    VoC_movreg32(REG01,REG45,DEFAULT);

    // old swBestPG
    VoC_lw16_sd(REG3,0,IN_PARALLEL);

    VoC_jal(CII_DIV_S);
    // swCCDivG in REG2

OPENLOOP304:
    // new swBestPG
    VoC_add16_rr(REG3,REG3,REG2,DEFAULT);

    // function getCCThreshold is inline

    // &swRp0 = &pswSfrmEng[i] in REG0
    // swCC = swCCMax in REG4
    // swG = swGMax in REG5

    // &pswSfrmEng[i]
    VoC_lw16_sd(REG3,2,DEFAULT);

    // store swBestPG before it is overwritten
    exit_on_warnings=OFF;
    VoC_sw16_sd(REG3,0,DEFAULT);
    exit_on_warnings=ON;

    // L_deposit_h(swCC)
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_movreg16(REG7,REG4,DEFAULT);

    // sw1 = extract_h(L_mult(swRp0, swG)) in REG1
    VoC_multf32_rp(REG01,REG5,REG3,IN_PARALLEL);

    // new L_1 in ACC0, preload with L_deposit_h(swPGainClamp);
    VoC_lw16i(ACC0_HI,0x21);
    // L_1 = L_sub(L_1, L_deposit_h(swCC));
    VoC_sub32_rr(REG67,REG01,REG67,DEFAULT);


    VoC_bngtz32_r(GETCCTHRESHOLD101,REG67);

    // INPUT :
    // REG0 : Nominator (param0)
    // REG1 : Denominator (param1)
    // OUTPUT
    // REG2 : return
    //
    // USED REGS : RL6, RL7

    // extract_h(L_1) in REG0
    // sw1 already in REG1
    VoC_movreg16(REG0,REG7,DEFAULT);

    VoC_jal(CII_DIV_S);
    // sw1 in REG2

    // if (sub(sw1, swPGainClamp) <= 0 keep default values
    VoC_bngt16_rd(GETCCTHRESHOLD101,REG2,CONST_0x0021_ADDR);

    // L_1 = L_deposit_h(sw1);
    VoC_movreg16(ACC0_HI,REG2,DEFAULT);

GETCCTHRESHOLD101:

    VoC_lw16i_short(ACC0_LO,0,DEFAULT);

    // function fnLog2 is inline
    // input in ACC0
    // output in REG1

    // siShiftCnt = norm_l(L_Input);
    // L_Input = L_shl(L_Input, siShiftCnt);

    VoC_jal(CII_NORM_L_ACC0);
    // siShiftCnt in REG1
    // L_Input in ACC0

    // swInSqrd = mult_r(swIn, swIn);
    VoC_movreg16(REG2,ACC0_HI,DEFAULT);
    VoC_multf32_rr(REG67,REG2,REG2,DEFAULT);


    // siShiftCnt = negate(add(siShiftCnt, 1)) << 10;
    VoC_sub16_dr(REG1,CONST_0xFFFF_ADDR,REG1);

    VoC_add32_rd(REG67,REG67,CONST_0x00008000L_ADDR);
    VoC_shr16_ri(REG1,-10,DEFAULT);

    // L_Input = L_mult(swInSqrd, swC0);
    // L_Input = L_mac(L_Input, swIn, swC1);
    // L_Input = L_add(L_Input, L_deposit_h(swC2))
    VoC_lw16i_short(RL7_LO,0,IN_PARALLEL);
    VoC_lw16i(RL7_HI,-0x54d0);
    VoC_lw16i(REG0,-0x2b2a);
    //  VoC_multf32_rd(ACC0,REG7,CONST_neg0x2b2a_ADDR);
    VoC_multf32_rr(ACC0,REG7,REG0,DEFAULT);
    VoC_mac32_rd(REG2,CONST_0x7fc5_ADDR);

    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);

    // L_Input = L_shr(L_Input, 3) & 0x03ffffff;
    // L_Input = round(L_add(L_Input, L_deposit_h(siShiftCnt)));
    VoC_shr32_ri(ACC0,3,DEFAULT);
    VoC_lw16i_short(RL7_LO,-1,IN_PARALLEL);
    VoC_lw16i(RL7_HI,0x03ff);
    VoC_and32_rr(ACC0, RL7,DEFAULT);


    VoC_add32_rr(ACC0,ACC0,REG01,DEFAULT);
    VoC_add32_rd(REG01, ACC0, CONST_0x00008000L_ADDR);


    // sw1 in REG1
    // L_1 = L_mult(sw1, swPGainScale);
    VoC_multf32_rd(REG01,REG1,CONST_0x6000_ADDR);

    VoC_NOP();
    // function fnExp2 is inline
    // input in REG01
    // output in REG1

    // swTemp2 in REG1
    // L_Input in REG01
    VoC_multf32_rd(REG01,REG1,CONST_32_ADDR);

    // swTemp3
    VoC_lw16i_short(REG3,-1,DEFAULT);

    // swTemp4 in REG3
    VoC_sub16_rr(REG3,REG3,REG1,DEFAULT);

    // L_Input = L_Input & LSP_MASK
    // L_Input = L_add(L_Input, L_deposit_h(swTemp3));
    // this is in fact replacing the ih with swTemp3
    VoC_lw16i_short(REG1,-1,IN_PARALLEL);

    // L_shr(L_Input, 1) :
    // L_Input in REG01
    // swTemp1 in REG0
    VoC_shr32_ri(REG01,1,DEFAULT);

    // swTemp2 = mult_r(swTemp1, swTemp1);
    // L_Input = L_mult(swTemp2, pswPCoefE[0]);
    // swTemp2 in REG5
    VoC_multf32_rr(REG45,REG0,REG0,DEFAULT);
    VoC_lw16i_set_inc(REG2,TABLE_SP_FRM_pswPCoefE_ADDR,1);
    VoC_add32_rd(REG45,REG45,CONST_0x00008000L_ADDR);

    // L_Input
    VoC_multf32inc_rp(ACC0,REG5,REG2,DEFAULT);
    VoC_mac32inc_rp(REG0,REG2,DEFAULT);
    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);
    VoC_add32_rr(REG01,ACC0,REG45,DEFAULT);

    // &pswSfrmEng[i]
    VoC_lw16_sd(REG2,2,DEFAULT);

    // if (swTemp4 > 0)
    VoC_bngtz16_r(FNEXP100,REG3);
    VoC_shr32_rr(REG01,REG3,DEFAULT);
FNEXP100:
    // sw1 in REG1
    // swRp0 in REG5, REG4 = 0 already
    VoC_lw16inc_p(REG5,REG2,DEFAULT);

    // L_1 = L_mult(swRp0, sw1)
    // L_1 = L_deposit_h(swRp0) - L_1
    VoC_multf32_rr(REG01,REG5,REG1,DEFAULT);

    // store back &pswSfrmEng[++i]
    VoC_sw16_sd(REG2,2,DEFAULT);

    VoC_sub32_rr(REG01,REG45,REG01,DEFAULT);
    VoC_shr32_ri(REG01,1,DEFAULT);
    VoC_movreg16(REG3,REG1,DEFAULT);

    // value for pswCCThresh[i] in REG3
    VoC_jump(OPENLOOP305);


OPENLOOP305_bis:
    VoC_lw16_sd(REG1,2,DEFAULT);
    VoC_NOP();
    VoC_inc_p(REG1,DEFAULT);
    VoC_sw16_sd(REG1,2,DEFAULT);

OPENLOOP305:

    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // pswVadLags              pswPitchBuf & pswHNWCoefBuf
    // swSP                    pswLIntBuf & pswCCThresh
    // pswSfrmEng              &ppC[i][LMIN-LSMIN], &ppG[i][LMIN-LSMIN]
    // &pswSfrmEng[i]          &pswLIntBuf[i], &pswVadLags[i]
    // &pswCCThresh[i]
    // swBestPG


    // load pointers
    VoC_lw16_sd(REG2,1,DEFAULT);
    VoC_lw32_sd(REG01,1,DEFAULT);

    // update pointers
    VoC_add16_rd(REG0,REG0,CONST_128_ADDR);
    VoC_sub16_rd(REG1,REG1,CONST_40_ADDR);

    // save pswCCThresh[i]
    VoC_sw16inc_p(REG3,REG2,DEFAULT);

    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    // save back pointers
    VoC_sw16_sd(REG2,1,DEFAULT);
    VoC_sw32_sd(REG01,1,IN_PARALLEL);
    VoC_endloop1


    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // pswVadLags              pswPitchBuf & pswHNWCoefBuf
    // swSP                    pswLIntBuf & pswCCThresh
    // pswSfrmEng              &ppC[4][LMIN-LSMIN], &ppG[4][LMIN-LSMIN]
    // &pswSfrmEng[4]          &pswLIntBuf[4], &pswVadLags[4]
    // &pswCCThresh[4]
    // swBestPG


    // pswSfrmEng
    // L_Voicing = 0
    VoC_lw16_sd(REG1,3,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_lw16i(REG6,-0x2976);
    // load loop & swSP
    VoC_loop_i_short(4,DEFAULT)
    VoC_startloop0
//      VoC_mac32inc_pd(REG1, CONST_neg0x2976_ADDR);
    VoC_mac32inc_rp(REG6,REG1, DEFAULT);
    VoC_endloop0


    // L_Voicing = L_add(L_Voicing, L_deposit_h(swBestPG));
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_pop16(REG7,IN_PARALLEL);

    VoC_add32_rr(ACC0,REG67,ACC0,DEFAULT);

    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // pswVadLags              pswPitchBuf & pswHNWCoefBuf
    // swSP                    pswLIntBuf & pswCCThresh
    // pswSfrmEng              &ppC[4][LMIN-LSMIN], &ppG[4][LMIN-LSMIN]
    // &pswSfrmEng[4]          &pswLIntBuf[4], &pswVadLags[4]
    // &pswCCThresh[4]


    // preload siAnchorIndex with 0
    // preload siTrajIndex with 0
    // preload siPeakIndex with 0
    // preload siBestTrajIndex with 0
    VoC_sw16_sd(REG6,1,DEFAULT);
    VoC_sw16_sd(REG6,4,DEFAULT);
    VoC_push16(REG6,DEFAULT);
    VoC_push16(REG6,DEFAULT);


    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // siBestTrajIndex         pswPitchBuf & pswHNWCoefBuf
    // swSP                    pswLIntBuf & pswCCThresh
    // pswSfrmEng              &ppC[4][LMIN-LSMIN], &ppG[4][LMIN-LSMIN]
    // siAnchorIndex           &pswLIntBuf[4], &pswVadLags[4]
    // &pswCCThresh[4]
    // siTrajIndex
    // siPeakIndex


    // pointers to tables
    VoC_lw16i(REG6,LOCAL_OPENLOOPSEARCH_pswLPeaks_ADDR);
    VoC_lw16i_set_inc(REG0,LOCAL_OPENLOOPSEARCH_pswCPeaks_ADDR,1);
    VoC_lw16i_set_inc(REG1,LOCAL_OPENLOOPSEARCH_pswGPeaks_ADDR,1);
    VoC_lw16i_set_inc(REG2,LOCAL_OPENLOOPSEARCH_pswCFrame_ADDR,6);
    VoC_lw16i_set_inc(REG3,LOCAL_OPENLOOPSEARCH_pswGFrame_ADDR+120,1);

    // swBestPG = SW_MIN
    VoC_lw16i(REG7,0x8000);

    // save pswXPeaks
    VoC_sw16_sd(REG6,2,DEFAULT);
    VoC_sw32_sd(REG01,0,IN_PARALLEL);

    // pswXFrame
    VoC_sw32_sd(REG23,1,DEFAULT);

    // swSP
    VoC_lw16_sd(REG4,5,IN_PARALLEL);

    // store swBestPG
    VoC_sw16_sd(REG7,5,DEFAULT);


    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // siBestTrajIndex         pswPitchBuf & pswHNWCoefBuf
    // swBestPG                pswLIntBuf & pswCCThresh
    // pswSfrmEng              &ppG[0][0], &ppC[0][0]
    // siAnchorIndex           pswGPeaks & pswCPeaks
    // pswLPeaks
    // siTrajIndex
    // siPeakIndex


    VoC_bngtz32_r(OPENLOOPUNVOICED,ACC0)
    VoC_bez16_r(OPENLOOPUNVOICED,REG4)

    // if !((L_Voicing <= 0) || (swSP == 0))
    // for (siAnchorIndex = 0; siAnchorIndex < N_SUB; siAnchorIndex++)

OPENLOOPPITCHLAGLOOPSTART:


    // INPUT:
    // REG0 : pswCPeaksSorted,  INC0 = 1    (param 5)
    // REG1 : pswGPeaksSorted,  INC1 = 1    (param 6)
    // REG2 : pswIntCs,     INC2 = 6    (param 1)
    // REG3 : pswIntGs,     INC3 = 1    (param 2)
    // REG4 : swBestIntLag              (param 0)
    // REG5 : swCCThreshold             (param 3)
    // REG6 : pswLPeaksSorted           (param 4)
    // OUTPUT:
    // REG5 : siNumSorted               (param 7)
    // REG6 : swPitch               (param 8)
    // REG7 : swHNWCoef             (param 9)

    // NOTE : all C & G buffers need to be placed
    //    in different memories !!!


    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // siBestTrajIndex         pswPitchBuf & pswHNWCoefBuf
    // swBestPG                &pswLIntBuf[Anchor], &pswCCThresh[Anchor]
    // pswSfrmEng              &ppG[Anchor][0], &ppC[Anchor][0]
    // siAnchorIndex           pswGPeaks & pswCPeaks
    // pswLPeaks
    // siTrajIndex
    // siPeakIndex
    VoC_lw32_sd(REG45,2,DEFAULT);


    VoC_lw16i_short(INC2,6,DEFAULT);
    // swBestIntLag = pswLIntBuf[siAnchorIndex] in REG4
    VoC_lw16_p(REG4,REG4,DEFAULT);
    // swCCThreshold = pswCCThresh[siAnchorIndex] in REG5
    VoC_lw16_p(REG5,REG5,DEFAULT);

    // if (pswLIntBuf[siAnchorIndex] != 0)
    VoC_bez16_r(OPENLOOP403,REG4)

    VoC_jal(CII_pitchLags_new);
    // REG5 : siNumSorted   (param 7)
    // REG6 : swPitch   (param 8)
    // REG7 : swHNWCoef (param 9)

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jump(OPENLOOP403BIS);

OPENLOOP403:
    // not continue flag (same as siNumPeaks == 0)
    VoC_lw16i_short(REG5,0,DEFAULT);

OPENLOOP403BIS:

    // pswLIntBuf[siAnchorIndex]=0 on the stack
    VoC_sw16_sd(REG4,0,DEFAULT);

    // pswPitchBuf & pswHNWCoefBuf
    VoC_lw32_sd(REG01,3,DEFAULT);

    // load siAnchorIndex
    VoC_lw16_sd(REG4,3,IN_PARALLEL);

    // &p[Anchor]
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG4,IN_PARALLEL);

    VoC_bnez16_r(OPENLOOP404,REG5)

    // default swPitch & swHNWCoef
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i(REG6,126);
OPENLOOP404:

    // pswHNWCoefBuf[siAnchorIndex]
    VoC_sw16_p(REG7,REG1,DEFAULT);
    // pswPitchBuf[siAnchorIndex]
    VoC_sw16_p(REG6,REG0,DEFAULT);

    // continue
    VoC_bez16_r(OPENLOOPCONTINUE,REG5)



    // siNumPeaks & siNumTrajToDo in REG5
    VoC_bngt16_rd(OPENLOOPWHILESTART,REG5,CONST_2_ADDR)
    VoC_lw16i_short(REG5,2,DEFAULT);

OPENLOOPWHILESTART:

    // while (siNumTrajToDo)
    VoC_bez16_r(OPENLOOPWHILEEND,REG5)

    // REG4 : siAnchorIndex
    // REG5 : siNumTrajToDo

    // no need of si1, we directly jump if si1 = 1 to
    // the sequence to execute and skip otherwise
    // we load here a general purpose 1
    VoC_lw16i_short(REG1,1,DEFAULT);

    // STACK16           STACK32
    // --------------------------------------------
    // RA                pswLagList & pswNumLagList
    // siBestTrajIndex   pswPitchBuf & pswHNWCoefBuf
    // swBestPG          &pswLIntBuf[Anchor],&pswCCThresh[Anchor]
    // pswSfrmEng        ppGfrm[Anchor], ppCfrm[Anchor]
    // siAnchorIndex     pswGPeaks & pswCPeaks
    // pswLPeaks
    // siTrajIndex
    // siPeakIndex


    // pswLPeaks
    VoC_lw16_sd(REG7,2,DEFAULT);

    // &ppswTrajLBuf[0][0]
    VoC_lw16i_set_inc(REG2,LOCAL_OPENLOOPSEARCH_ppswTrajLBuf_ADDR,4);

    // &ppswTrajLBuf[0][siAnchorIndex]
    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);

    // siPeakIndex
    VoC_lw16_sd(REG6,0,IN_PARALLEL);

    // &pswLPeaks[siPeakIndex]
    VoC_add16_rr(REG7,REG6,REG7,DEFAULT);

    // siTrajIndex
    VoC_lw16_sd(REG3,1,IN_PARALLEL);


    // case siTrajIndex == 0, then si1 == 0,
    // skip {if (si1)} sequence
    VoC_bngtz16_r(OPENLOOPNOTSI1,REG3)

    VoC_loop_r_short(REG3,DEFAULT)
    VoC_startloop0
    VoC_lw16_p(REG1,REG7,DEFAULT);
    VoC_sub16inc_rp(REG0,REG1,REG2,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL);
    // case si1 == 1; goto {if (si1)} sequence
    VoC_bez16_r(OPENLOOPSI1,REG0);
    VoC_endloop0

    // set inc2 to 1

    VoC_lw16i_short(INC2,1,DEFAULT);

    // case si1 == 0, skip {if (si1)} sequence
    VoC_jump(OPENLOOPNOTSI1);

OPENLOOPSI1:
    // if (si1)
    // siNumTrajToDo--;
    VoC_sub16_rr(REG5,REG5,REG1,DEFAULT);

    // else siPeakIndex++; continue;
    VoC_add16_rr(REG6,REG6,REG1,DEFAULT);

    // if !(siNumTrajToDo) break;
    VoC_bez16_r(OPENLOOPWHILEEND,REG5)

    // save siPeakIndex++
    VoC_sw16_sd(REG6,0,DEFAULT);
    VoC_jump(OPENLOOPWHILESTART);

OPENLOOPNOTSI1:
    // if !(si1)

    // STACK16            STACK32
    // -------------------------------------------
    // RA                 pswLagList & pswNumLagList
    // siBestTrajIndex    pswPitchBuf & pswHNWCoefBuf
    // swBestPG           &pswLIntBuf[Anchor],&pswCCThresh[Anchor]
    // pswSfrmEng         &ppC[Anchor][0], &ppG[Anchor][0]
    // siAnchorIndex      pswGPeaks & pswCPeaks
    // pswLPeaks
    // siTrajIndex
    // siPeakIndex

    // REG2 : &ppswTrajLBuf[siTrajIndex][siAnchorIndex]
    // REG7 : &pswLPeaks[siPeakIndex]
    // REG3 : siTrajIndex
    // REG4 : siAnchorIndex
    // REG5 : siNumTrajToDo
    // REG6 : siPeakIndex



    // pswLPeaks[siPeakIndex]
    VoC_lw16_p(REG7,REG7,DEFAULT);

    // compute &ppswTrajLBuf[siTrajIndex][0] for future use
    VoC_sub16_rr(REG1,REG2,REG4,IN_PARALLEL);

    // load pswGPeaks & pswCPeaks
    VoC_lw32_sd(REG01,0,DEFAULT);

    // save &ppswTrajLBuf[siTrajIndex][0]
    exit_on_warnings=OFF;
    VoC_push16(REG1,DEFAULT);
    exit_on_warnings=ON;

    // siTrajIndex*4
    VoC_shr16_ri(REG3,-2,DEFAULT);


    // STACK16            STACK32
    // --------------------------------------------
    // RA                 pswLagList & pswNumLagList
    // siBestTrajIndex    pswPitchBuf & pswHNWCoefBuf
    // swBestPG           &pswLIntBuf[Anchor],&pswCCThresh[Anchor]
    // pswSfrmEng         &ppC[Anchor][0], &ppG[Anchor][0]
    // siAnchorIndex      pswCPeaks & pswGPeaks
    // pswLPeaks
    // siTrajIndex
    // siPeakIndex
    // ppTrjLBf[TrjIdx]

    // compute &p[siPeakIndex]
    VoC_add16_rr(REG0,REG0,REG6,IN_PARALLEL);
    VoC_add16_rr(REG1,REG1,REG6,DEFAULT);

    // store pswLPeaks[siPeakIndex]
    VoC_sw16_p(REG7,REG2,IN_PARALLEL);

    // ppswTrajGBuf & ppswTrajCCBuf
    VoC_lw16i(REG6,LOCAL_OPENLOOPSEARCH_ppswTrajCCBuf_ADDR);
    VoC_lw16i(REG7,LOCAL_OPENLOOPSEARCH_ppswTrajGBuf_ADDR);

    // pswGPeaks[siPeakIndex]
    VoC_lw16_p(REG1,REG1,DEFAULT);

    // &ppTrjCCBf[TrjIdx][0]
    VoC_add16_rr(REG6,REG6,REG3,IN_PARALLEL);

    // mult(pswCPeaks[siPeakIndex], pswCPeaks[siPeakIndex])
    VoC_multf32_pp(ACC1,REG0,REG0,DEFAULT);

    // &ppTrjGBf[TrjIdx][0]
    VoC_add16_rr(REG7,REG7,REG3,IN_PARALLEL);


    // &ppTrjCCBf[TrjIdx][siAnchorIndex]
    // &ppTrjGBf[TrjIdx][siAnchorIndex]
    VoC_add16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG4,IN_PARALLEL);


    // push &ppTrjGBf[TrjIdx][0], &ppTrjCCBf[TrjIdx][0]
    // push siNumTrajToDo
    exit_on_warnings=OFF;
    VoC_push32(REG67,DEFAULT);
    exit_on_warnings=ON;
    VoC_push16(REG5,DEFAULT);


    // mult_r(pswCPeaks[siPeakIndex], pswCPeaks[siPeakIndex]);
    VoC_add32_rd(ACC1,ACC1,CONST_0x00008000L_ADDR);


    // STACK16           STACK32
    // ---------------------------------------------
    // RA                pswLagList & pswNumLagList
    // siBestTrajIndex   pswPitchBuf & pswHNWCoefBuf
    // swBestPG          &pswLIntBuf[Anchor],&pswCCThresh[Anchor]
    // pswSfrmEng        ppGfrm[Anchor], ppCfrm[Anchor]
    // siAnchorIndex     pswGPeaks & pswCPeaks
    // pswLPeaks         ppTrjGBf[TrjIdx],ppTrjCCBf[TrjIdx]
    // siTrajIndex
    // siPeakIndex
    // ppTrjLBf[TrjIdx]
    // siNumTrajToDo

    // get a copy of ppGfrm[Anchor], ppCfrm[Anchor]
    VoC_lw32_sd(REG01,2,DEFAULT);

    // write before REG0 update
    exit_on_warnings=OFF;
    VoC_sw16_p(REG1,REG7,DEFAULT);
    exit_on_warnings=ON;
    VoC_sw16_p(ACC1_HI,REG6,DEFAULT);

    // save copy of ppGfrm[Anchor], ppCfrm[Anchor]
    // and &ppTrjLBf[TrjIdx][Anchor]
    VoC_push16(REG2,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);


    // STACK16            STACK32
    // -------------------------------------------
    // RA                 pswLagList & pswNumLagList
    // siBestTrajIndex    pswPitchBuf & pswHNWCoefBuf
    // swBestPG           &pswLIntBuf[Anchor],&pswCCThresh[Anchor]
    // pswSfrmEng         ppGfrm[Anchor], ppCfrm[Anchor]
    // siAnchorIndex      pswGPeaks & pswCPeaks
    // pswLPeaks          ppTrjCCBf[TrjIdx],ppTrjGBf[TrjIdx]
    // siTrajIndex        ppGfrm[Anchor], ppCfrm[Anchor]
    // siPeakIndex
    // ppTrjLBf[TrjIdx]
    // siNumTrajToDo
    // &ppTrjLBf[TrjIdx][Anchor]


    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_jal(CII_compute_s2_s3_new);

    VoC_lw16_sd(REG2,0,DEFAULT);
    VoC_lw16_sd(REG4,6,DEFAULT);

    // for (siFIndex = siAnchorIndex + 1; siFIndex < N_SUB; siFIndex++)
    // test if 0 loops
    VoC_bnlt16_rd(OPENLOOP412,REG4,CONST_3_ADDR)


OPENLOOP413:

    // ppswTrajLBuf[siTrajIndex][siFIndex - 1]
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    // because of pipe structure ++
    // is taken into account before sw
    VoC_push16(REG2,DEFAULT);

    // push siFIndex-1
    VoC_push16(REG4,DEFAULT);



    // STACK16         STACK32
    // ------------------------------------------------
    // RA              pswLagList & pswNumLagList
    // siBestTrajIndex pswPitchBuf & pswHNWCoefBuf
    // swBestPG        &pswLIntBuf[Anchor],&pswCCThresh[Anchor]
    // pswSfrmEng      ppGfrm[Anchor], ppCfrm[Anchor]
    // siAnchorIndex   pswGPeaks & pswCPeaks
    // pswLPeaks       ppTrjCCBf[TrjIdx],ppTrjGBf[TrjIdx]
    // siTrajIndex     ppGfrm[siFIndex-1],ppCfrm[siFIndex-1]
    // siPeakIndex
    // ppTrjLBf[TrjIdx]
    // siNumTrajToDo
    // &ppTrjLBf[TrjIdx][Anchor]
    // &ppTrjLBf[TrjIdx][siFIndex]
    // siFIndex-1


    // INPUT
    // REG6 : swRawLag
    // OUTPUT
    // REG2 : *pswCode
    // REG3 : return

    // unused : REG7

    VoC_jal(CII_quantLag_opt_paris);
    // si1 (siLowerBound) in REG2
    // compute_s2_s3 flag = 0
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_jal(CII_compute_s2_s3_new);
    // siNumDelta in REG7
    // INC2 = INC3 = 1


    // INPUT
    // REG0 : pswCSfrm,         (param1)
    // REG1 : pswGSfrm,         (param2)
    // REG2 : pswCCTraj,  INC2 = 1  (param6)
    // REG3 : pswGTraj,   INC3 = 1  (param7)
    // REG4 : pswLagList        (param0)
    // REG5 : pswLTraj      (param5)
    // REG6 : siSfrmIndex       (param4)
    // REG7 : siNumLags     (param3)

    // CAUTION !!!
    // pswcBuf & pswGBuf
    // pswCInterp & pswGInterp
    // need to be placed in different memories

    // siNumDelta already in REG7

    // siFIndex-1
    VoC_lw16_sd(REG6,0,DEFAULT);

    // ppswTrajCCBuf[siTrajIndex], ppswTrajGBuf[siTrajIndex]
    VoC_lw32_sd(REG23,1,DEFAULT);

    // ppswTrajLBuf[siTrajIndex]
    VoC_lw16_sd(REG5,4,DEFAULT);

    // ppswGSfrm[siFIndex-1], ppswCSfrm[siFIndex-1]
    VoC_lw32_sd(REG01,0,IN_PARALLEL);

    // ppswGSfrm[siFIndex], ppswCSfrm[siFIndex]
    VoC_add16_rd(REG0,REG0,CONST_128_ADDR);
    VoC_sub16_rd(REG1,REG1,CONST_40_ADDR);

    // (siFIndex-1)++
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);

    // pswLArray
    VoC_lw16i(REG4,LOCAL_OPENLOOPSEARCH_pswLArray_ADDR);

    // save back new siFIndex
    VoC_sw16_sd(REG6,0,DEFAULT);

    // save back ppswGSfrm[siFIndex], ppswCSfrm[siFIndex]
    VoC_sw32_sd(REG01,0,IN_PARALLEL);


    VoC_jal(CII_bestDelta_new);

    // pop new siFIndex - 1
    // and new ppswTrajLBuf[siTrajIndex][siFIndex-1]
    VoC_pop16(REG4,DEFAULT);
    VoC_pop16(REG2,DEFAULT);


    // for (siFIndex = siAnchorIndex + 1; siFIndex < N_SUB; siFIndex++)
    VoC_blt16_rd(OPENLOOP413,REG4,CONST_3_ADDR)

    // VoC_jump(OPENLOOP413);

OPENLOOP412:

    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // siBestTrajIndex         pswPitchBuf & pswHNWCoefBuf
    // swBestPG                &pswLIntBuf[Anchor],&pswCCThresh[Anchor]
    // pswSfrmEng              ppCfrm[Anchor], ppGfrm[Anchor]
    // siAnchorIndex           pswCPeaks & pswGPeaks
    // pswLPeaks               ppTrjCCBf[TrjIdx],ppTrjGBf[TrjIdx]
    // siTrajIndex             ppCfrm[N_SUB-1], ppGfrm[N_SUB-1]
    // siPeakIndex
    // ppTrjLBf[TrjIdx]
    // siNumTrajToDo
    // &ppTrjLBf[TrjIdx][Anchor]


    // get a copy of ppCfrm[Anchor], ppGfrm[Anchor]
    VoC_lw32_sd(REG01,3,DEFAULT);

    // REG2 for ppswTrajLBuf[siTrajIndex][siBIndex + 1] address
    VoC_pop16(REG2,DEFAULT);

    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // siBestTrajIndex         pswPitchBuf & pswHNWCoefBuf
    // swBestPG                &pswLIntBuf[Anchor],&pswCCThresh[Anchor]
    // pswSfrmEng              ppCfrm[Anchor], ppGfrm[Anchor]
    // siAnchorIndex           pswCPeaks & pswGPeaks
    // pswLPeaks               ppTrjCCBf[TrjIdx],ppTrjGBf[TrjIdx]
    // siTrajIndex             ppCfrm[N_SUB-1], ppGfrm[N_SUB-1]
    // siPeakIndex
    // ppTrjLBf[TrjIdx]
    // siNumTrajToDo

    // REG4 for siBIndex+1
    VoC_lw16_sd(REG4,5,DEFAULT);

    // save a copy of ppCfrm[Anchor], ppGfrm[Anchor]
    VoC_sw32_sd(REG01,0,IN_PARALLEL);


    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // siBestTrajIndex         pswPitchBuf & pswHNWCoefBuf
    // swBestPG                &pswLIntBuf[Anchor],&pswCCThresh[Anchor]
    // pswSfrmEng              ppCfrm[Anchor], ppGfrm[Anchor]
    // siAnchorIndex           pswCPeaks & pswGPeaks
    // pswLPeaks               ppTrjCCBf[TrjIdx],ppTrjGBf[TrjIdx]
    // siTrajIndex             ppCfrm[siBIndex+1], ppGfrm[siBIndex+1]
    // siPeakIndex
    // ppTrjLBf[TrjIdx]
    // siNumTrajToDo



    // for (siBIndex = siAnchorIndex - 1; siBIndex >= 0; siBIndex--)
    // test if 0 loops
    VoC_bngtz16_r(OPENLOOP417,REG4)

OPENLOOP418:

    // INC2 = -1

    VoC_lw16i_short(INC2,-1,DEFAULT);
    // compute_s2_s3 flag = 1
    VoC_lw16i_short(REG7,1,DEFAULT);

    // ppswTrajLBuf[siTrajIndex][siBIndex+1]
    VoC_lw16inc_p(REG6,REG2,DEFAULT);

    // because of pipe structure --
    // is taken into account before sw
    VoC_push16(REG2,DEFAULT);

    // push siBIndex+1
    VoC_push16(REG4,DEFAULT);


    // STACK16         STACK32
    // -----------------------------------------------
    // RA              pswLagList & pswNumLagList
    // siBestTrajIndex pswPitchBuf & pswHNWCoefBuf
    // swBestPG        &pswLIntBuf[Anchor],&pswCCThresh[Anchor]
    // pswSfrmEng      ppCfrm[Anchor], ppGfrm[Anchor]
    // siAnchorIndex   pswCPeaks & pswGPeaks
    // pswLPeaks       ppTrjCCBf[TrjIdx],ppTrjGBf[TrjIdx]
    // siTrajIndex     ppCfrm[siBIndex+1], ppGfrm[siBIndex+1]
    // siPeakIndex
    // ppTrjLBf[TrjIdx]
    // siNumTrajToDo
    // ppswTrajLBuf[siTrajIndex][siBIndex+1]
    // siBIndex+1


    // INPUT
    // REG6 : swRawLag
    // OUTPUT
    // REG2 : *pswCode
    // REG3 : return

    // unused : REG7

    VoC_jal(CII_quantLag_opt_paris);
    // si1 (siLowerBound) in REG2
    // flag in REG7
    VoC_jal(CII_compute_s2_s3_new);
    // siNumDelta in REG7
    // INC2 = INC3 = 1

    // INPUT
    // REG0 : pswCSfrm,         (param1)
    // REG1 : pswGSfrm,         (param2)
    // REG2 : pswCCTraj,  INC2 = 1  (param6)
    // REG3 : pswGTraj,   INC3 = 1  (param7)
    // REG4 : pswLagList        (param0)
    // REG5 : pswLTraj      (param5)
    // REG6 : siSfrmIndex       (param4)
    // REG7 : siNumLags     (param3)

    // CAUTION !!!
    // pswcBuf & pswGBuf
    // pswCInterp & pswGInterp
    // need to be placed in different memories

    // siNumDelta already in REG7

    // siBIndex+1
    VoC_lw16_sd(REG6,0,DEFAULT);

    // ppswTrajCCBuf[siTrajIndex], ppswTrajGBuf[siTrajIndex]
    VoC_lw32_sd(REG23,1,IN_PARALLEL);

    // ppswTrajLBuf[siTrajIndex]
    VoC_lw16_sd(REG5,3,DEFAULT);

    // ppswCSfrm[siBIndex+1], ppswGSfrm[siBIndex+1]
    VoC_lw32_sd(REG01,0,IN_PARALLEL);

    // ppswCSfrm[siBIndex], ppswGSfrm[siBIndex]
    VoC_sub16_rd(REG0,REG0,CONST_128_ADDR);
    VoC_add16_rd(REG1,REG1,CONST_40_ADDR);

    // (siBIndex+1)--
    VoC_sub16_rd(REG6,REG6,CONST_1_ADDR);

    // pswLArray
    VoC_lw16i(REG4,LOCAL_OPENLOOPSEARCH_pswLArray_ADDR);

    // save back new siBIndex
    VoC_sw16_sd(REG6,0,DEFAULT);

    // save back ppswCSfrm[siBIndex], ppswGSfrm[siBIndex]
    VoC_sw32_sd(REG01,0,IN_PARALLEL);

    VoC_jal(CII_bestDelta_new);

    // pop new siBIndex+1
    // and new ppswTrajLBuf[siTrajIndex][siBIndex+1]
    VoC_pop16(REG4,DEFAULT);
    VoC_pop16(REG2,DEFAULT);

    // VoC_jump(OPENLOOP418);
    VoC_bgtz16_r(OPENLOOP418,REG4)


OPENLOOP417:


    // STACK16           STACK32
    // --------------------------------------------
    // RA                pswLagList & pswNumLagList
    // siBestTrajIndex   pswPitchBuf & pswHNWCoefBuf
    // swBestPG          &pswLIntBuf[Anchor],&pswCCThresh[Anchor]
    // pswSfrmEng        ppCfrm[Anchor], ppGfrm[Anchor]
    // siAnchorIndex     pswCPeaks & pswGPeaks
    // pswLPeaks         ppTrjCCBf[TrjIdx],ppTrjGBf[TrjIdx]
    // siTrajIndex       ppCfrm[1], ppGfrm[1]
    // siPeakIndex
    // ppTrjLBf[TrjIdx]
    // siNumTrajToDo

    // REG7 for swTotalCCDivG = 0;
    VoC_lw16i_short(REG7,0,DEFAULT);

    // ppTrjCCBf[TrjIdx],ppTrjGBf[TrjIdx]
    VoC_lw32_sd(REG23,1,IN_PARALLEL);

    // for (i = 0; i < N_SUB; i++)
    VoC_loop_i(1,4)

    // swCC = ppswTrajCCBuf[siTrajIndex][i];
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    // swG = ppswTrajGBuf[siTrajIndex][i];
    VoC_lw16inc_p(REG5,REG3,DEFAULT);

    // &ppswTrajCCBuf[siTrajIndex][++i]
    // &ppswTrajGBuf[siTrajIndex][++i]
    VoC_push32(REG23,DEFAULT);

    VoC_movreg16(REG3,REG4,IN_PARALLEL);

    // if (swG <= 0)
    VoC_bgtz16_r(OPENLOOP2001,REG5)
    // swCCDivG = 0;
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_jump(OPENLOOP2002ADD);
OPENLOOP2001:
    // abs_s(swCC)
    VoC_bgtz16_r(OPENLOOP2003, REG4)
    VoC_sub16_dr(REG4,CONST_0_ADDR,REG4);
OPENLOOP2003:

    // if (sub(abs_s(swCC), swG) > 0)
    VoC_bngt16_rr(OPENLOOP2005,REG4,REG5)

    // swCCDivG = SW_MAX;
    VoC_lw16i(REG2,0X7FFF);
    // if (swCC > 0)
    VoC_bgtz16_r(OPENLOOP2004,REG4)
    // swCCDivG = SW_MIN;
    VoC_lw16i(REG2,0X8000);
OPENLOOP2004:
    VoC_jump(OPENLOOP2002ADD);

OPENLOOP2005:

    // INPUT :
    // REG0 : Nominator (param0)
    // REG1 : Denominator (param1)
    // OUTPUT
    // REG2 : return
    //
    // USED REGS : RL6, RL7

    // swCCMax, swGMax
    VoC_movreg32(REG01,REG45,DEFAULT);


    VoC_jal(CII_DIV_S);
    // swCCDivG in REG2

    // if (swCC < 0) negate(swCCDivG)
    VoC_bgtz16_r(OPENLOOP2002ADD,REG3)
    VoC_sub16_dr(REG2,CONST_0_ADDR,REG2);


OPENLOOP2002ADD:

    // swTotalCCDivG = add(swTotalCCDivG, swCCDivG);
    VoC_add16_rr(REG7,REG7,REG2,DEFAULT);
    // &ppswTrajCCBuf[siTrajIndex][i]
    // &ppswTrajGBuf[siTrajIndex][i]
    VoC_pop32(REG23,DEFAULT);

    VoC_NOP();

    VoC_endloop1


    // STACK16           STACK32
    // --------------------------------------------
    // RA                pswLagList & pswNumLagList
    // siBestTrajIndex   pswPitchBuf & pswHNWCoefBuf
    // swBestPG          &pswLIntBuf[Anchor],&pswCCThresh[Anchor]
    // pswSfrmEng        ppCfrm[Anchor], ppGfrm[Anchor]
    // siAnchorIndex     pswCPeaks & pswGPeaks
    // pswLPeaks         ppTrjCCBf[TrjIdx],ppTrjGBf[TrjIdx]
    // siTrajIndex       ppCfrm[1], ppGfrm[1]
    // siPeakIndex
    // ppTrjLBf[TrjIdx]
    // siNumTrajToDo

    // swBestPG
    VoC_lw16_sd(REG6,7,DEFAULT);

    // general purpose cnst = 1
    VoC_lw16i_short(REG1,1,IN_PARALLEL);

    // siPeakIndex
    VoC_lw16_sd(REG3,2,DEFAULT);

    // siTrajIndex
    VoC_lw16_sd(REG5,3,DEFAULT);

    // siPeakIndex++;
    VoC_add16_rr(REG3,REG3,REG1,IN_PARALLEL);


    // if (sub(swTotalCCDivG, swBestPG) > 0)
    VoC_bngt16_rr(OPENLOOP421,REG7,REG6)

    // swBestPG = swTotalCCDivG;
    VoC_sw16_sd(REG7,7,DEFAULT);

    // siBestTrajIndex = siTrajIndex
    VoC_sw16_sd(REG5,8,DEFAULT);

OPENLOOP421:

    // siTrajIndex++
    VoC_add16_rr(REG5,REG5,REG1,DEFAULT);

    // store siPeakIndex
    VoC_sw16_sd(REG3,2,DEFAULT);

    // store siTrajIndex
    VoC_sw16_sd(REG5,3,DEFAULT);

    // siNumTrajToDo
    VoC_pop16(REG5,DEFAULT);

    // siNumTrajToDo--
    VoC_sub16_rr(REG5,REG5,REG1,DEFAULT);

    // clear some no more used pointers from stack
    VoC_pop32(RL7,IN_PARALLEL);
    VoC_pop16(REG4,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);


    // STACK16           STACK32
    // --------------------------------------------
    // RA                pswLagList & pswNumLagList
    // siBestTrajIndex   pswPitchBuf & pswHNWCoefBuf
    // swBestPG          &pswLIntBuf[Anchor],&pswCCThresh[Anchor]
    // pswSfrmEng        ppCfrm[Anchor], ppGfrm[Anchor]
    // siAnchorIndex     pswCPeaks & pswGPeaks
    // pswLPeaks
    // siTrajIndex
    // siPeakIndex


    // siAnchorIndex
    VoC_lw16_sd(REG4,3,DEFAULT);


    // REG4 : siAnchorIndex
    // REG5 : siNumTrajToDo

    VoC_jump(OPENLOOPWHILESTART);

OPENLOOPWHILEEND:



    // STACK16           STACK32
    // --------------------------------------------
    // RA                pswLagList & pswNumLagList
    // siBestTrajIndex   pswPitchBuf & pswHNWCoefBuf
    // swBestPG          &pswLIntBuf[Anchor],&pswCCThresh[Anchor]
    // pswSfrmEng        ppCfrm[Anchor], ppGfrm[Anchor]
    // siAnchorIndex     pswCPeaks & pswGPeaks
    // pswLPeaks
    // siTrajIndex
    // siPeakIndex

OPENLOOPCONTINUE:

    // REG1 : 1
    // REG4 : siAnchorIndex

    VoC_lw16i_short(REG1,1,DEFAULT);

    // siAnchorIndex++
    VoC_add16_rr(REG7,REG4,REG1,DEFAULT);

    // &pswLIntBuf[Anchor],&pswCCThresh[Anchor]
    VoC_lw32_sd(REG45,2,IN_PARALLEL);

    // ppCfrm[Anchor], ppGfrm[Anchor]
    VoC_lw32_sd(REG23,1,DEFAULT);

    // &pswLIntBuf[Anchor++],&pswCCThresh[Anchor++]
    VoC_add16_rr(REG4,REG4,REG1,IN_PARALLEL);
    VoC_add16_rr(REG5,REG5,REG1,DEFAULT);

    // pswCPeaks & pswGPeaks
    VoC_lw32_sd(REG01,0,IN_PARALLEL);

    // ppswCSfrm[Anchor++], ppswGSfrm[Anchor++]
    VoC_add16_rd(REG2,REG2,CONST_128_ADDR);
    VoC_sub16_rd(REG3,REG3,CONST_40_ADDR);

    // pswLPeaks
    VoC_lw16_sd(REG6,2,DEFAULT);

    // store &pswLIntBuf[Anchor++],&pswCCThresh[Anchor++]
    VoC_sw32_sd(REG45,2,IN_PARALLEL);

    // store siAnchorIndex++
    VoC_sw16_sd(REG7,3,DEFAULT);

    // store ppswCSfrm[Anchor++], ppswGSfrm[Anchor++]
    VoC_sw32_sd(REG23,1,IN_PARALLEL);


    // if (siAnchorIndex == 4)
    VoC_be16_rd(OPENLOOPPITCHLAGLOOPEND,REG7,CONST_4_ADDR);
    // else
    VoC_jump(OPENLOOPPITCHLAGLOOPSTART)

OPENLOOPPITCHLAGLOOPEND:

    // END for (siAnchorIndex = 0; siAnchorIndex < N_SUB; siAnchorIndex++)


    // STACK16           STACK32
    // --------------------------------------------
    // RA                pswLagList & pswNumLagList
    // siBestTrajIndex   pswPitchBuf & pswHNWCoefBuf
    // swBestPG          &pswLIntBuf[4],&pswCCThresh[4]
    // pswSfrmEng        ppCfrm[4], ppGfrm[4]
    // 4                 pswCPeaks & pswGPeaks
    // pswLPeaks
    // siTrajIndex
    // siPeakIndex


    // siTrajIndex
    VoC_lw16_sd(REG7,1,DEFAULT);
    // if (siTrajIndex == 0)
    VoC_bez16_r(OPENLOOPUNVOICED,REG7)

    // siBestTrajIndex
    VoC_lw16_sd(REG7,6,DEFAULT);

    // RL6_LO for *psiUVCode
    VoC_lw16i_short(RL6_LO,3,IN_PARALLEL);

    // siBestTrajIndex * 4
    VoC_shr16_ri(REG7,-2,DEFAULT);

    // RL6_HI for siLowestSoFar
    VoC_lw16i_short(RL6_HI,2,IN_PARALLEL);

    // ppswTrajCCBuf[siBestTrajIndex]
    // ppswTrajGBuf[siBestTrajIndex]
    VoC_lw16i_set_inc(REG0,LOCAL_OPENLOOPSEARCH_ppswTrajCCBuf_ADDR,1);
    VoC_lw16i_set_inc(REG1,LOCAL_OPENLOOPSEARCH_ppswTrajGBuf_ADDR,1);
    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

    // pswSfrmEng (INC2 = 1), ACC1 = 0
    VoC_lw16_sd(REG2,4,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_loop_i_short(4,DEFAULT)
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_startloop0

    // swG = ppswTrajGBuf[siBestTrajIndex][i];
    VoC_lw16inc_p(REG3,REG1,DEFAULT);

    // swRG = mult_r(swG, pswSfrmEng[i]);
    VoC_multf32inc_rp(REG45,REG3,REG2,DEFAULT);

    // swCC = ppswTrajCCBuf[siBestTrajIndex][i];
    // L_Voicing = L_deposit_h(swCC);
    VoC_lw16inc_p(ACC1_HI,REG0,DEFAULT);

    VoC_add32_rd(REG45,REG45,CONST_0x00008000L_ADDR);

    // L_Voicing = L_mac(L_Voicing, swRG, UV_SCALE1);
    VoC_multf32_rd(ACC0,REG5,CONST_neg0x6676_ADDR);

    //  VoC_NOP();
    VoC_lw16i(REG3, -0x46d3);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);

    VoC_bnltz32_r(OPENLOOP431,ACC0)
    // L_Voicing = L_mac(L_Voicing, swRG, UV_SCALE1);
    //      VoC_multf32_rd(ACC0,REG5,CONST_neg0x46d3_ADDR);
    VoC_multf32_rr(ACC0,REG5,REG3,DEFAULT);
    VoC_NOP();
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);

    VoC_bnltz32_r(OPENLOOP432, ACC0)
    VoC_lw16i_short(RL6_HI,1,DEFAULT);
OPENLOOP432:
    // *psiUVCode = siLowestSoFar;
    VoC_movreg16(RL6_LO,RL6_HI,DEFAULT);
OPENLOOP431:
    VoC_NOP();
    VoC_endloop0


    // STACK16           STACK32
    // --------------------------------------------
    // RA                pswLagList & pswNumLagList
    // siBestTrajIndex   pswPitchBuf & pswHNWCoefBuf
    // swBestPG          &pswLIntBuf[4],&pswCCThresh[4]
    // pswSfrmEng        ppCfrm[4], ppGfrm[4]
    // 4                 pswCPeaks & pswGPeaks
    // pswLPeaks
    // siTrajIndex
    // siPeakIndex


    // REG7 : siBestTrajIndex * 4
    // ppswTrajLBuf[siBestTrajIndex]
    VoC_lw16i_set_inc(REG0,LOCAL_OPENLOOPSEARCH_ppswTrajLBuf_ADDR,1);
    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);

    // pswLagList & pswNumLagList
    VoC_lw32_sd(REG23,4,IN_PARALLEL);

    VoC_lw16i_short(INC3,1,DEFAULT);
    // siLagsSoFar is not necessary


    // STACK16           STACK32
    // --------------------------------------------
    // RA                pswLagList & pswNumLagList
    // siBestTrajIndex   pswPitchBuf & pswHNWCoefBuf
    // swBestPG          &pswLIntBuf[4],&pswCCThresh[4]
    // pswSfrmEng        ppCfrm[4], ppGfrm[4]
    // *psiUVCode        pswCPeaks & pswGPeaks
    // pswLPeaks
    // siTrajIndex
    // siPeakIndex


    VoC_loop_i(1,4)

    VoC_lw16inc_p(REG6,REG0,DEFAULT);

    // INPUT
    // REG6 : swRawLag
    // OUTPUT
    // REG2 : *pswCode
    // REG3 : return

    // unused : REG7

    VoC_push32(REG23,DEFAULT);
    VoC_push16(REG0,IN_PARALLEL);

    VoC_jal(CII_quantLag_opt_paris);
    // si1 in REG2

    // CNST = 1
    VoC_lw16i_short(REG3,1,DEFAULT);

    VoC_add16_rr(REG4,REG2,REG3,DEFAULT);
    VoC_sub16_rr(REG5,REG2,REG3,IN_PARALLEL);

    // if (sub(si2, (1 << L_BITS) - 1) > 0)
    VoC_bngt16_rd(OPENLOOP441,REG4,CONST_0x00ff_ADDR)
    // si2 = (1 << L_BITS) - 1;
    VoC_lw16i(REG4,0xff);
OPENLOOP441:
    // if (si3 < 0)
    VoC_bnltz16_r(OPENLOOP442,REG5)
    //   si3 = 0;
    VoC_lw16i_short(REG5,0,DEFAULT);
OPENLOOP442:

    // sub(si2, si3);
    VoC_sub16_rr(REG6,REG4,REG5,DEFAULT);

    // &ppswTrajLBuf[siBestTrajIndex][i]
    VoC_pop16(REG0,DEFAULT);

    // psrLagTbl[si3]
    VoC_lw16i_set_inc(REG1,TABLE_SP_ROM_psrLagTbl_ADDR,1);
    VoC_add16_rr(REG1,REG1,REG5,DEFAULT);

    // add(sub(si2, si3), 1)
    VoC_add16_rr(REG6,REG6,REG3,IN_PARALLEL);

    // pswLagList[siLagsSoFar] & pswNumLagList[i]
    VoC_pop32(REG23,DEFAULT);

    VoC_bngtz16_r(OPENLOOP443,REG6)
    VoC_loop_r_short(REG6,DEFAULT)
    VoC_lw16inc_p(REG5,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

OPENLOOP443:
    // pswNumLagList[i] = add(sub(si2, si3), 1);
    VoC_sw16inc_p(REG6,REG3,DEFAULT);

    VoC_endloop1


    // STACK16           STACK32
    // --------------------------------------------
    // RA                pswLagList & pswNumLagList
    // siBestTrajIndex   pswPitchBuf & pswHNWCoefBuf
    // swBestPG          &pswLIntBuf[4],&pswCCThresh[4]
    // pswSfrmEng        ppCfrm[4], ppGfrm[4]
    // *psiUVCode        pswCPeaks & pswGPeaks
    // pswLPeaks
    // siTrajIndex
    // siPeakIndex

    VoC_jump(OPENLOOPFINISH);



OPENLOOPUNVOICED:

    // STACK16                 STACK32
    // ------------------------------------------------
    // RA                      pswLagList & pswNumLagList
    // siBestTrajIndex         pswPitchBuf & pswHNWCoefBuf
    // swBestPG                pswLIntBuf & pswCCThresh
    // pswSfrmEng              &ppC[0][0], &ppG[0][0]
    // siAnchorIndex           pswCPeaks & pswGPeaks
    // pswLPeaks
    // siTrajIndex
    // siPeakIndex

    // OR

    // STACK16           STACK32
    // --------------------------------------------
    // RA                pswLagList & pswNumLagList
    // siBestTrajIndex   pswPitchBuf & pswHNWCoefBuf
    // swBestPG          &pswLIntBuf[4],&pswCCThresh[4]
    // pswSfrmEng        ppCfrm[4], ppGfrm[4]
    // 4                 pswCPeaks & pswGPeaks
    // pswLPeaks
    // siTrajIndex
    // siPeakIndex


    // pswLagList & pswNumLagList
    VoC_lw32_sd(REG01,4,DEFAULT);

    // *psiUVCode = 0;
    VoC_lw32z(RL6,IN_PARALLEL);

    // pswPitchBuf & pswHNWCoefBuf

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw32_sd(REG23,3,IN_PARALLEL);

    // load loop
    VoC_loop_i_short(4,DEFAULT)
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_startloop0
    VoC_sw16inc_p(RL6_LO,REG0,DEFAULT);   //   pswNumLagList[i] = 0;
    VoC_sw16inc_p(RL6_LO,REG1,DEFAULT);   //   pswLagList[i] = 0;
    VoC_sw16inc_p(RL6_LO,REG2,DEFAULT);   //   pswPitchBuf[i] = 0;
    VoC_sw16inc_p(RL6_LO,REG3,DEFAULT);   //   pswHNWCoefBuf[i] = 0;
    VoC_endloop0


OPENLOOPFINISH:

    // restitute stack
    VoC_lw16_d(SP16,OPENLOOP_SP16_SAVE_ADDR);
    VoC_lw16_d(SP32,OPENLOOP_SP32_SAVE_ADDR);

    VoC_pop16(RA,DEFAULT);

    // return *psiUVCode in REG0
    VoC_movreg16(REG0,RL6_LO,DEFAULT);

    VoC_return;
}

void CII_getSfrmLpcTx_opt(void)
//////////////////////////////////////////////////////////////////////
//  FUNC void CII_getSfrmLpcTx_opt(void)
//  IN:
//      REG0    &pswNewFrmAs     INCR0=1
//      REG1    &psnsSqrtRs      INCR1=1
//      REG2    &ppswSynthAs     INCR2=1
//      REG3    &pswPrevFrmAs    INCR3=1
//      REG4    &pswPrevFrmKs
//      REG5    swPrevR0
//      REG6    &pswNewFrmKs
//      REG7    swNewR0
//      RL6_LO  &pswNewFrmSNWCoef(&pswNewFrmPFNum)
//      RL6_HI  &ppswSNWCoefAs(&ppswPFNumAs)
//      RL7_HI  &psrOldCont
//      RL7_LO  &psrNewCont
//      ACC0_LO &pswPrevFrmSNWCoef(&pswPrevFrmPFNum)
//      ACC0_HI &pswPrevFrmPFDenom
//      ACC1_LO &pswNewFrmPFDenom
//      ACC1_HI &ppswPFDenomAs

//  FLAG_GETSFRMLPC_TX_ADDR   CII_getSfrmLpcTx=1,CII_getSfrmLpc=0
//
/////////////////////////////////////////////////////////////////////
{

#if 0

    voc_short    LOCAL_EN_SPEECHENCODER_interpolateCheck_pswRcTemp_ADDR    ,10,x
    voc_short    LOCAL_compResidEnergy_pswResidual_ADDR                    ,40,x

#endif


#if 0

    voc_short    cov32_copy_ADDR    ,10,Y
#endif



    // push variables in the STACK
    VoC_push16(RA,DEFAULT);

    VoC_sw16_d(SP16,sfrmAnalysis_SP16_SAVE0_ADDR);


    // &pswPrevFrmKs
    VoC_push16(REG4,DEFAULT);

    // push &pswNewFrmAs
    VoC_push16(REG0,DEFAULT);
    // push &ppswPFDenomAs
    VoC_push16(ACC1_HI,DEFAULT);
    // push &pswNewFrmPFDenom
    VoC_push16(ACC1_LO,DEFAULT);

    // push &pswPrevFrmPFDenom
    VoC_push16(ACC0_HI,DEFAULT);
    // push &pswPrevFrmSNWCoef
    VoC_push16(ACC0_LO,DEFAULT);

    // push &ppswSNWCoefAs
    VoC_push16(RL6_HI,DEFAULT);
    // push &pswNewFrmSNWCoef
    VoC_push16(RL6_LO,DEFAULT);

    // push swPrevR0
    VoC_push16(REG5,DEFAULT);
    // push &ppswSynthAs
    VoC_push16(REG2,DEFAULT);
    // push &pswNewFrmKs
    VoC_push16(REG6,DEFAULT);

    // push &psnsSqrtRs
    VoC_push16(REG1,DEFAULT);

    // push &pswPrevFrmAs
    VoC_push16(REG3,DEFAULT);
    // push &psrNewCont AND &psrOldCont
    VoC_push32(RL7,IN_PARALLEL);

    // push swNewR0
    VoC_push16(REG7,DEFAULT);



//     STACK16                          STACK32
//---------------------------------------------------------
//  &pswPrevFrmKs              &psrNewCont AND &psrOldCont
//  &pswNewFrmAs
//  &ppswPFDenomAs(&ppswSNWCoefAs)
//  &pswNewFrmPFDenom(&pswNewFrmSNWCoef)
//  &pswPrevFrmPFDenom(&pswPrevFrmSNWCoef)
//  &pswPrevFrmSNWCoef
//  &ppswSNWCoefAs
//  &pswNewFrmSNWCoef
//    swPrevR0
//  &ppswSynthAs
//  &pswNewFrmKs
//  &psnsSqrtRs
//  &pswPrevFrmAs
//     swNewR0

    // in function GetSfrmLpcTx, judge if (siSoftInterpolation)
    VoC_bnez16_d(GETSFRMLPC_IEF2,FLAG_GETSFRMLPC_TX_ADDR);
    // if swSI==0 will jump
    VoC_bnez16_d(GETSFRMLPC_IEF2,GLOBAL_DE_SP_swSi_ADDR);

    // else jump
    VoC_jump(GETSFRMLPC_IEF0);


GETSFRMLPC_IEF2:


///////////////////////////////////////////////
//  void IEF_SUBFUNC(void)
//  IN:
//      REG3     &pswPrevFrmAs
//      REG4     &pswPrevFrmKs
//      REG5     swPrevR0
//      REG2     &ppswSynthAs
//      ACC0_LO  &pswPrevFrmPFDenom (&pswPrevSNWCoefAs)
//      RL6_LO   &pswNewFrmPFDenom  (&pswNewSNWCoefAs)
//      RL6_HI   &ppswPFDenomAs     (&ppswSNWCoefAs)
//////////////////////////////////////////////
// begin of CII_IEF_SUBFUNC

//     STACK16
//-----------------------------------
//  &pswPrevFrmKs
//  &pswNewFrmAs
//  &ppswPFDenomAs(&ppswSNWCoefAs)
//  &pswNewFrmPFDenom(&pswNewFrmSNWCoef)
//  &pswPrevFrmPFDenom(&pswPrevFrmSNWCoef)
//  &pswPrevFrmSNWCoef
//  &ppswSNWCoefAs
//  &pswNewFrmSNWCoef
//    swPrevR0
//  &ppswSynthAs
//  &pswNewFrmKs
//  &psnsSqrtRs
//  &pswPrevFrmAs
//     swNewR0

//STACK32
//----------------------
//&psrNewCont AND &psrOldCont
//&psrNewCont AND &psrOldCont

    // push &ppswPFDenomAs(&ppswSNWCoefAs) AND &pswNewFrmPFDenom(&pswNewSNWCoefAs)
    VoC_push32(RL6,DEFAULT);
    // push &pswPrevFrmPFDenom
    VoC_push16(ACC0_LO,DEFAULT);
    // REG6 for counter
    VoC_lw16i_short(REG6, 0, DEFAULT);

//    VoC_push16(REG0,IN_PARALLEL);


GETSFRMLPCTX_LOOP_START:
    // push ppswSynthAs
    VoC_push16(REG2,DEFAULT);
    // push &pswNewFrmAs
    VoC_push16(REG0,DEFAULT);
    // REG01 for &psrNewCont AND &psrOldCont
    VoC_lw32_sd(REG01,1,IN_PARALLEL);
    // push counter
    VoC_push16(REG6,DEFAULT);

//     STACK16
//-----------------------------------
//  &pswPrevFrmKs
//  &pswNewFrmAs
//  &ppswPFDenomAs(&ppswSNWCoefAs)
//  &pswNewFrmPFDenom(&pswNewFrmSNWCoef)
//  &pswPrevFrmPFDenom(&pswPrevFrmSNWCoef)
//  &pswPrevFrmSNWCoef
//  &ppswSNWCoefAs
//  &pswNewFrmSNWCoef
//    swPrevR0
//  &ppswSynthAs
//  &pswNewFrmKs
//  &psnsSqrtRs
//  &pswPrevFrmAs
//     swNewR0
//  &pswPrevFrmPFDenom  (&pswPrevSNWCoefAs)
//       RA
//  &ppswSynthAs
//  &pswNewFrmAs
//  counter


//      STACK32
//-------------------------------------------
//  &psrNewCont AND &psrOldCont
//  &psrNewCont AND &psrOldCont
//  &ppswPFDenomAs(&ppswSNWCoefAs) AND &pswNewFrmPFDenom(&pswNewSNWCoefAs)

    // REG67 for psrNewCont AND psrOldCont
    VoC_lw16inc_p(REG6, REG0, DEFAULT);
    VoC_lw16inc_p(REG7, REG1, DEFAULT);

    // save new &psrNewCont AND &psrOldCont
    VoC_sw32_sd(REG01,1,DEFAULT);
    // REG1 for &pswPrevFrmAs
    VoC_lw16_sd(REG1,5, IN_PARALLEL);
    // REG0 for &pswNewFrmAs
    VoC_lw16_sd(REG0,1,DEFAULT);
    // push value of psrNewCont AND psrOldCont
    VoC_push32(REG67,IN_PARALLEL);

//      STACK32
//-------------------------------------------
//  &psrNewCont AND &psrOldCont
//  &psrNewCont AND &psrOldCont
//  &ppswPFDenomAs(&ppswSNWCoefAs) AND &pswNewFrmPFDenom(&pswNewSNWCoefAs)
//  psrNewCont AND psrOldCont

    /*************************************************************
    IN: REG0  &pswNewCoefsA[0] INC0 1
        REG1  &pswOldCoefsA[0] INC1 1
        REG2  &pswCoefOutA[0]  INC2 1
        REG3  &pswRefCoefsA[0] INC3 1
        REG4  &psnRefKs
        REG5  swRq
        REG6  swNewPer
        REG7  swOldPer
    OUT:REG7  SiInterp_flg
        REG23 psnsSqrtRsOut
    **************************************************************/
//      begin of CII_interpolateCheck_opt

    VoC_push32(REG23,DEFAULT);

    // STACK16 RA
    // STACK32 &pswCoefOutA[0],&pswRefCoefsA[0],&psnRefKs,swRq

    VoC_loop_i_short(10,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);
    VoC_startloop0;
    VoC_lw32_d(ACC0,CONST_0x00008000L_ADDR);
    VoC_mac32inc_rp(REG6,REG0,DEFAULT);
    VoC_mac32inc_rp(REG7,REG1,DEFAULT);
    VoC_NOP();//0428
    VoC_lw16i_short(REG3,10,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);
    VoC_endloop0;
    VoC_lw16i_set_inc(REG1,LOCAL_EN_SPEECHENCODER_interpolateCheck_pswRcTemp_ADDR+9,-1);

    VoC_sub16_rr(REG2,REG2,REG3,DEFAULT);
    VoC_movreg16(REG0,REG2,DEFAULT);

    /**************************************************
    *INPUT: REG0 &pswAin[0] INC0=1
    *       REG1 &pswRc[9]  INC1=-1
    *       REG6 swActShift
    *OUTPUT:REG7 siUnstableFlt
    *USE:   all regs
    **************************************************/

    VoC_jal(CII_aToRc_opt2);
    VoC_pop32(REG45,DEFAULT);

    VoC_bnez16_r(interpolateCheck0,REG7);

    VoC_lw16i_set_inc(REG0,LOCAL_EN_SPEECHENCODER_interpolateCheck_pswRcTemp_ADDR,1);
    VoC_jal(CII_res_eng_opt);
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_pop32(REG45, DEFAULT);
    VoC_jump(interpolateCheck1);

interpolateCheck0:

    VoC_movreg16(REG0,REG4,DEFAULT);

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_jal(CII_res_eng_opt);
    VoC_pop32(REG01,DEFAULT);

    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_loop_i_short(10,DEFAULT);
    VoC_startloop0;
    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    VoC_endloop0;

interpolateCheck1:


    VoC_lw16i_short(INC3,1,DEFAULT);
// end of CII_interpolateCheck_opt
    // pop counter
    VoC_pop16(REG6, DEFAULT);
    // REG4 for &psnsSqrtRs
    VoC_lw16_sd(REG4,5,DEFAULT);
//     STACK16
//-----------------------------------
//  &pswPrevFrmKs
//  &pswNewFrmAs
//  &ppswPFDenomAs(&ppswSNWCoefAs)
//  &pswNewFrmPFDenom(&pswNewFrmSNWCoef)
//  &pswPrevFrmPFDenom(&pswPrevFrmSNWCoef)
//  &pswPrevFrmSNWCoef
//  &ppswSNWCoefAs
//  &pswNewFrmSNWCoef
//    swPrevR0
//  &ppswSynthAs
//  &pswNewFrmKs
//  &psnsSqrtRs
//  &pswPrevFrmAs
//     swNewR0
//  &pswPrevFrmPFDenom(&pswPrevSNWCoefAs)
//       RA
//  &ppswSynthAs
//  &pswNewFrmAs

    // calculate &psnsSqrtRs[siSfrm]
    VoC_add16_rr(REG4, REG4, REG6, DEFAULT);
    VoC_add16_rr(REG4, REG4, REG6, DEFAULT);

    // REG01 &ppswPFDenomAs(&ppswSNWCoefAs) AND &pswNewFrmPFDenom(&pswNewSNWCoefAs)
    VoC_lw32_sd(REG01,1, DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    // save &psnsSqrtRs[siSfrm]
    VoC_sw32_p(REG23, REG4, DEFAULT);



    // REG45 psrOldCont[siSfrm] & psrNewCont[siSfrm]
    VoC_pop32(REG45,DEFAULT);

    // REG3 for &pswPrevFrmPFDenom(&pswPrevSNWCoefAs)
    VoC_lw16_sd(REG3,2,IN_PARALLEL);

    VoC_loop_i(1,2);
    // if it is the first time in the loop
    // use &pswPrevFrmPFDenom(&pswPrevSNWCoefAs)
    VoC_movreg16(REG2,REG3,DEFAULT);
    VoC_bez16_r(getSfrmLpcTx0_JUMP, REG6);
    // else use &pswNewFrmPFDenom(&pswNewSNWCoefAs)
    VoC_movreg16(REG2, REG0, DEFAULT);
getSfrmLpcTx0_JUMP:
    /*for (i = 0; i < NP; i++)
    {
      L_Temp1 = L_mult(pswNewFrmPFNum[i], psrNewCont[siSfrm]);
      ppswPFNumAs[siSfrm][i] = mac_r(L_Temp1, pswPrevFrmPFNum[i],
                                     psrOldCont[siSfrm]);
      L_Temp2 = L_mult(pswNewFrmPFDenom[i], psrNewCont[siSfrm]);
      ppswPFDenomAs[siSfrm][i] = mac_r(L_Temp2, pswPrevFrmPFDenom[i],
                                       psrOldCont[siSfrm]);
    }*/
    VoC_multf32inc_rp(ACC0,REG4,REG0,DEFAULT);
    VoC_loop_i_short(10, DEFAULT);

    VoC_startloop0

    VoC_mac32inc_rp(REG5,REG3,DEFAULT);
    VoC_lw16inc_p(ACC1_HI,REG2,DEFAULT);
    // judge if siStable
    VoC_bez16_r(getSfrmLpcTx0, REG7);
    VoC_add32_rd(ACC1,ACC0,CONST_0x00008000L_ADDR);
getSfrmLpcTx0:
    VoC_multf32inc_rp(ACC0,REG4,REG0,DEFAULT);
    VoC_sw16inc_p(ACC1_HI,REG1,DEFAULT);
    VoC_endloop0

    VoC_bnez16_d(GETSFRMLPC_IEF3,FLAG_GETSFRMLPC_TX_ADDR);
    // the following is only for FUNC lpc


    // REG3 for &pswPrevFrmPFDenom
    VoC_lw16_sd(REG3,12,DEFAULT);
    // REG0 for &pswNewFrmPFDenom
    VoC_lw16_sd(REG0,13,DEFAULT);
    // REG1 for &ppswPFDenomAs
    VoC_lw16_sd(REG1,14,DEFAULT);



    VoC_endloop1

    exit_on_warnings=OFF;
    VoC_sw16_sd(REG1,14,DEFAULT);
    exit_on_warnings=ON;

GETSFRMLPC_IEF3:


    // REG01 for &psrNewCont AND &psrOldCont
    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_lw16i_short(REG7,10,DEFAULT);

    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);

    VoC_add16_rd(REG6, REG6, CONST_1_ADDR);
    // save new &psrNewCont AND &psrOldCont
    VoC_sw32_sd(REG01,0,DEFAULT);

    VoC_lw16_sd(REG4, 6,IN_PARALLEL);




    // REG0 &pswNewFrmAs
    VoC_pop16(REG0, DEFAULT);
    // REG3 &pswNewFrmAs
    VoC_movreg16(REG3,REG0,DEFAULT);
    // REG2 &ppswSNWCoefAs
    VoC_pop16(REG2,IN_PARALLEL);
    // REG5 swNewR0
    VoC_lw16_sd(REG5,1,DEFAULT);

    VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);

    VoC_be16_rd(GETSFRMLPCTX_LOOP_END, REG6, CONST_3_ADDR);
    VoC_jump(GETSFRMLPCTX_LOOP_START);
GETSFRMLPCTX_LOOP_END:
//     STACK16                          STACK32
//---------------------------------------------------------
//  &pswPrevFrmKs              &psrNewCont AND &psrOldCont
//  &pswNewFrmAs
//  &ppswPFDenomAs(&ppswSNWCoefAs)
//  &pswNewFrmPFDenom(&pswNewFrmSNWCoef)
//  &pswPrevFrmPFDenom(&pswPrevFrmSNWCoef)
//  &pswPrevFrmSNWCoef
//  &ppswSNWCoefAs
//  &pswNewFrmSNWCoef
//    swPrevR0
//  &ppswSynthAs
//  &pswNewFrmKs
//  &psnsSqrtRs
//  &pswPrevFrmAs
//     swNewR0

    /****************three*****************************/
//    VoC_pop16(REG7,DEFAULT);      // RA STEPHEN
    VoC_pop32(RL6,DEFAULT);
    VoC_pop16(REG7,DEFAULT);

    VoC_push16(REG0,DEFAULT);
    /*
        siSfrm = 3;
        for (i = 0; i < NP; i++)
        {
          ppswPFNumAs[siSfrm][i] = pswNewFrmPFNum[i];
          ppswPFDenomAs[siSfrm][i] = pswNewFrmPFDenom[i];
          ppswSynthAs[siSfrm][i] = pswNewFrmAs[i];
        }
    */
    VoC_movreg16(REG0,RL6_LO,DEFAULT);
    VoC_loop_i_short(10, DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(ACC0_LO,REG0,DEFAULT);
    VoC_lw16inc_p(ACC1_LO,REG3,DEFAULT);
    VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
    VoC_sw16inc_p(ACC1_LO,REG2,DEFAULT);
    VoC_endloop0

    VoC_bnez16_d(GETSFRMLPC_IEF3_bis,FLAG_GETSFRMLPC_TX_ADDR);
    VoC_lw16_sd(REG0,11,DEFAULT);
    VoC_lw16_sd(REG1,12,DEFAULT);
    VoC_loop_i_short(10, DEFAULT);
    VoC_lw16inc_p(ACC0_LO,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(ACC0_LO,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

GETSFRMLPC_IEF3_bis:

    VoC_pop16(REG0,DEFAULT);

// end of CII_IEF_SUBFUNC

    /*************************************************************
    IN:
    reg0:&pswReflecCoefIn[0] INC0=1;
    reg5:swRq
    reg23:
    OUT:
    reg2:psnsSqrtRsOut->man
    reg3:psnsSqrtRsOut->sh
    USE:
    all regs  void   CII_res_eng()
    *************************************************************/

//     STACK16                          STACK32
//---------------------------------------------------------
//  &pswPrevFrmKs              &psrNewCont AND &psrOldCont
//  &pswNewFrmAs
//  &ppswPFDenomAs(&ppswSNWCoefAs)
//  &pswNewFrmPFDenom(&pswNewFrmSNWCoef)
//  &pswPrevFrmPFDenom(&pswPrevFrmSNWCoef)
//  &pswPrevFrmSNWCoef
//  &ppswSNWCoefAs
//  &pswNewFrmSNWCoef
//    swPrevR0
//  &ppswSynthAs
//  &pswNewFrmKs
//  &psnsSqrtRs
//  &pswPrevFrmAs
//     swNewR0

    // REG5 for swNewR0
    VoC_lw16_sd(REG5,0,DEFAULT);
    // REG0 for &pswNewFrmKs
    VoC_lw16_sd(REG0,3,DEFAULT);

    VoC_jal(CII_res_eng_opt);

    // REG0 for &psnsSqrtRs
    VoC_lw16_sd(REG0,2,DEFAULT);
    // REG0 for &psnsSqrtRs[3]
    VoC_add16_rd(REG0,REG0,CONST_6_ADDR);
    VoC_NOP();
    // save the result
    VoC_sw32_p(REG23, REG0,DEFAULT);

    // if is CII_getSfrmLpc, will return directly here
    VoC_bnez16_d(GETSFRMLPC_IEF11111,FLAG_GETSFRMLPC_TX_ADDR)

    VoC_jump(GETSFRMLPC_IEF1);

GETSFRMLPC_IEF11111:
    /**************************************************
    INLINE FUNC CII_compResidEnergy

    * INPUT parameters
    * REG0 &psnsSqrtRs       INC0=2
    * REG1 &ppswInterpCoef
    * REG2 &pswPreviousCoef
    * REG3 &pswCurrentCoef
    * REG7 &pswSpeech
    *
    * Other vars:
    * reg0  &psnsSqrtRs[i]
    * reg45 temp
    * reg6  swMinShift
    * reg7  siOverfolowPossible
    * RL6   L_ResidualEng
    * return in REG4
    *
    * !!!!  State and Speech should in differnet RAM with Coef
    *
    *void  CII_compResidEnerg

    In the function, the tables of speech[] & state[] should be in different ram
    with table Coef[];So in the following places in the function:

    #define GLOBAL_pswAnalysisState_ADDR addr1
    #define LOCAL_compResidEnergy_pswTempState_ADDR addr2

    passing parameters:
    REGS[REG7].reg = addr3
    REGS[REG1].reg = addr4
    REGS[REG2].reg = addr5
    REGS[REG3].reg = addr6

    addr1,addr2,addr3 should be in a same RAM; addr4,addr5,addr6 should be in
    another.
    **************************************************/

    // so the following part is only for CII_getSfrmLpcTx
    VoC_lw16i(REG7, GLOBAL_EN_pswSpeech_ADDR);
    // REG1 for &ppswSynthAs
    VoC_lw16_sd(REG1, 4, DEFAULT);
    // REG2 for &pswPrevFrmAs
    VoC_lw16_sd(REG2, 1, DEFAULT);
    // REG3 for &pswFramAs
    VoC_lw16i(REG3, LOCAL_EN_SPEECHENCODER_pswFrmAs_ADDR);
    // REG0 for &psnsSqrtRs
    VoC_lw16_sd(REG0,2,DEFAULT);

    VoC_lw16i_short(INC0,2,IN_PARALLEL);
//  VoC_jal(CII_comRpesidEnergy);



    /**************************************************
    INLINE FUNC CII_compResidEnergy

    * INPUT parameters
    * REG0 &psnsSqrtRs       INC0=2
    * REG1 &ppswInterpCoef
    * REG2 &pswPreviousCoef
    * REG3 &pswCurrentCoef
    * REG7 &pswSpeech
    *
    * Other vars:
    * reg0  &psnsSqrtRs[i]
    * reg45 temp
    * reg6  swMinShift
    * reg7  siOverfolowPossible
    * RL6   L_ResidualEng
    * return in REG4
    *
    * !!!!  State and Speech should in differnet RAM with Coef
    *
    *void  CII_compResidEnerg

    In the function, the tables of speech[] & state[] should be in different ram
    with table Coef[];So in the following places in the function:



    passing parameters:
    REGS[REG7].reg = addr3
    REGS[REG1].reg = addr4
    REGS[REG2].reg = addr5
    REGS[REG3].reg = addr6

    addr1,addr2,addr3 should be in a same RAM; addr4,addr5,addr6 should be in
    another.
    **************************************************/



//     STACK16                          STACK32
//---------------------------------------------------------
//  &pswPrevFrmKs              &psrNewCont AND &psrOldCont
//  &pswNewFrmAs
//  &ppswPFDenomAs(&ppswSNWCoefAs)
//  &pswNewFrmPFDenom(&pswNewFrmSNWCoef)
//  &pswPrevFrmPFDenom(&pswPrevFrmSNWCoef)
//  &pswPrevFrmSNWCoef
//  &ppswSNWCoefAs
//  &pswNewFrmSNWCoef
//    swPrevR0
//  &ppswSynthAs
//  &pswNewFrmKs
//  &psnsSqrtRs
//  &pswPrevFrmAs
//     swNewR0

    //ra
//   VoC_push16(RA,DEFAULT);
    VoC_push16(REG2,DEFAULT);         //preCoef,ra
    VoC_push16(REG3,DEFAULT);         //currentCoef,preCoef,ra
    VoC_push16(REG7,DEFAULT);         //speech,currentCoef,preCoef,ra
    VoC_push16(REG1,DEFAULT);         //InterpCoef,speech,currentCoef,preCoef,ra

    VoC_lw16i(REG6, 0x7fff);// swMinShift in reg6
    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0;
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_bngt16_rd(compResidEnergy0,REG4,CONST_0_ADDR);
    VoC_bngt16_rr(compResidEnergy0,REG6,REG5);
    VoC_movreg16(REG6,REG5,DEFAULT);
compResidEnergy0:
    VoC_lw16i_short(REG7,0,DEFAULT); //siOverflowPossible in reg7
    // VoC_lw16i_short(REG7,1,DEFAULT); //siOverflowPossible in reg7
    VoC_endloop0;


    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_push16(REG7,DEFAULT);      //  counter,InterpCoef,speech,currentCoef,preCoef,ra in Stack

    // if REG6 >= 1 then REG5 = 0x1000, REG7 = 0
    VoC_bnlt16_rd(compResidEnergy3,REG6,CONST_1_ADDR);

    // elsif REG6 < -1 then REG5 = 0x1000, REG7 = 1
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_blt16_rd(compResidEnergy3,REG6,CONST_0xFFFF_ADDR);

    // elsif REG6 == -1 then REG5 = 0x2000, REG7 = 1
    VoC_shr16_ri(REG5,-1,DEFAULT);
    VoC_be16_rd(compResidEnergy3,REG6,CONST_0xFFFF_ADDR);

    // else REG6 == 0 and then REG5 = 0x4000, REG7 = 1
    VoC_shr16_ri(REG5,-1,DEFAULT);

compResidEnergy3:

    VoC_shr16_ri(REG5,-12,DEFAULT);

    // VoC_lw16i_short(REG0,0,DEFAULT);// reg5 for flag of first round or second round
    // VoC_push16(REG0,DEFAULT);       // counter,InterpCoef,speech,currentCoef,preCoef,ra in Stack

    VoC_lw16i_set_inc(REG1,GLOBAL_EN_SP_FRM_pswAnalysisState_ADDR,2);
    VoC_lw16i_set_inc(REG0,LOCAL_EN_SPEECHENCODER_interpolateCheck_pswRcTemp_ADDR,2);

    VoC_loop_i_short(5,DEFAULT);
    VoC_startloop0;
    VoC_lw32inc_p(RL6,REG1,DEFAULT);
    VoC_lw32z(RL6,DEFAULT);//L_ResidualEng in RL6
    exit_on_warnings=OFF;
    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;

LAP12:
    VoC_lw16i_short(REG6,0,DEFAULT);  // REG6 for i
LAP13:

    VoC_lw16i_short(INC3,1,DEFAULT);
    // in second round, reg1 &preCoef if i!=0
    VoC_lw16_sd(REG3,4,DEFAULT);//
    VoC_lw16_sd(REG0,0,DEFAULT);
    // else (also in second round) reg1 &CurrentCoef
    VoC_bez16_r(LAP10_new,REG6);
    VoC_lw16_sd(REG3,3,DEFAULT);   //
LAP10_new:

    VoC_lw16i_short(INC0,-1,DEFAULT);
    // reg2 &Globle_analysisState in the second round
    VoC_lw16i_set_inc(REG2,GLOBAL_EN_SP_FRM_pswAnalysisState_ADDR,1);
    VoC_bnez16_r(LAP11,REG0);
    // if in the first round
    VoC_lw16_sd(REG3,1,DEFAULT);
    VoC_mult16_rd(REG4,REG6,CONST_10_ADDR);
    VoC_lw16i_set_inc(REG2,LOCAL_EN_SPEECHENCODER_interpolateCheck_pswRcTemp_ADDR,1);
    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
    // reg1 &InterpCoef
    // reg2 &pswTempState

LAP11:
    // either in first round or in second round
    // reg0 &speech[i*S_LEN]
    VoC_lw16_sd(REG0,2,DEFAULT);
    VoC_mult16_rd(REG4,REG6,CONST_40_ADDR);


    // reg1 &pswResidual
    VoC_lw16i_set_inc(REG1,LOCAL_compResidEnergy_pswResidual_ADDR,1);
    VoC_add16_rr(REG0,REG4,REG0,DEFAULT);


    VoC_push16(REG7, DEFAULT);
    VoC_push16(REG5, DEFAULT);

    VoC_push32(REG01,DEFAULT);
    VoC_movreg16(REG1,REG3,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,cov32_copy_ADDR,1);
    VoC_loop_i_short(10,DEFAULT);
    VoC_startloop0;
    VoC_lw16inc_p(REG7,REG1,DEFAULT);
    VoC_lw16i(REG3,cov32_copy_ADDR);
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    VoC_endloop0;

    VoC_pop32(REG01,DEFAULT);
    VoC_lw16i_short(INC0,-1,IN_PARALLEL);
    /**************************************************
    * INPUT parameters :
    * REG0 : pswInput, INC0 = -1
    * REG3 : pswCoef , INC3 = 1
    * REG2 : pswState, INC2 = 1
    * REG1 : swFiltOut,INC1 = 1
    * other vars:
    * REG4 : maxsiStage1, maxsiStage2, extract_h(L_Sum)
    * REG5 : siSmp
    * ACC0
    **************************************************/


    VoC_jal(CII_lpcFir_opt);

    VoC_pop16(REG5,DEFAULT);
    VoC_pop16(REG7,DEFAULT);

    VoC_lw16i_set_inc(REG0,LOCAL_compResidEnergy_pswResidual_ADDR,1);

    VoC_movreg32(ACC0,RL6,DEFAULT);

    VoC_loop_i_short(40,DEFAULT);
    VoC_startloop0;
    VoC_bez16_r(compResidEnergy40,REG7);
    VoC_multf32inc_rp(REG23,REG5,REG0,DEFAULT);
    VoC_NOP();
    VoC_add32_rd(REG23,REG23,CONST_0x00008000L_ADDR);
    VoC_mac32_rr(REG3, REG3, DEFAULT);
    VoC_jump(compResidEnergy790);
compResidEnergy40:
    VoC_mac32inc_pp(REG0,REG0,DEFAULT);
compResidEnergy790:
    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_endloop0;

    // the first 4 times of loop is mac,
    // they are done by firstly 4 msu, then reverse the result

    VoC_add16_rr(REG6,REG6,REG1,DEFAULT);
    VoC_movreg32(RL6,ACC0,DEFAULT);

    VoC_be16_rd(LAP13_end,REG6,CONST_4_ADDR);
    VoC_jump(LAP13);
LAP13_end:


    VoC_pop16(REG0,DEFAULT);  // pop swShiftFactor
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);
    VoC_sub32_dr(RL6,CONST_0_ADDR,RL6);
    VoC_push16(REG0,DEFAULT);

    // VoC_bne16_rd(LAP12_bis,REG0,CONST_1_ADDR);
    // VoC_jump(LAP12);
    // here REG1=1
    VoC_be16_rr(LAP12,REG0,REG1);

// LAP12_bis:




//  VoC_pop16(RA,DEFAULT);   // pop counter

    VoC_bltz32_r(LAP20,RL6);
    VoC_lw16i_short(REG1,0,DEFAULT);
LAP20:

    // pop all parameters stocked in the stack
    // the last one put in the RA
    VoC_loop_i_short(5,DEFAULT);
    VoC_startloop0;
    VoC_pop16(REG7,DEFAULT);   // pop counter
    VoC_endloop0;


//  VoC_return;

///////////////////////////////////////////////////////////////
// end inline function CII_compResideEnefrty
//////////////////////////////////////////////////////////////



    // save result of CII_compResidEnergy
    VoC_sw16_d(REG1, LOCAL_EN_SPEECHENCODER_iSi_ADDR);

//     STACK16                          STACK32
//---------------------------------------------------------
//  &pswPrevFrmKs              &psrNewCont AND &psrOldCont
//  &pswNewFrmAs
//  &ppswPFDenomAs(&ppswSNWCoefAs)
//  &pswNewFrmPFDenom(&pswNewFrmSNWCoef)
//  &pswPrevFrmPFDenom(&pswPrevFrmSNWCoef)
//  &pswPrevFrmSNWCoef
//  &ppswSNWCoefAs
//  &pswNewFrmSNWCoef
//    swPrevR0
//  &ppswSynthAs
//  &pswNewFrmKs
//  &psnsSqrtRs
//  &pswPrevFrmAs
//     swNewR0

    VoC_bnez16_r(GETSFRMLPC_IEF1,REG1);

GETSFRMLPC_IEF0:
    // REG5 for swPrevR0
    VoC_lw16_sd(REG5,5,DEFAULT);
    // REG0 for &pswPrevFrmKs
    VoC_lw16_sd(REG0,13,DEFAULT);

    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    /*************************************************************
    IN:
    reg0:&pswReflecCoefIn[0] INC0=1;
    reg5:swRq
    reg23:
    OUT:
    reg2:psnsSqrtRsOut->man
    reg3:psnsSqrtRsOut->sh
    USE:
    all regs  void   CII_res_eng()
    *************************************************************/
    VoC_jal(CII_res_eng_opt);

    // push the result of res_eng_opt for the moment
    VoC_push32(REG23,DEFAULT);
    // REG5 for swPrevR0
    VoC_lw16_sd(REG5,0,DEFAULT);
    // REG0 for &pswNewFrmKs
    VoC_lw16_sd(REG0,3,DEFAULT);

    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_jal(CII_res_eng_opt);

//     STACK16                          STACK32
//---------------------------------------------------------
//  &pswPrevFrmKs              &psrNewCont AND &psrOldCont
//  &pswNewFrmAs               psnsSqrtRs[0]
//  &ppswPFDenomAs(&ppswSNWCoefAs)
//  &pswNewFrmPFDenom(&pswNewFrmSNWCoef)
//  &pswPrevFrmPFDenom(&pswPrevFrmSNWCoef)
//  &pswPrevFrmSNWCoef
//  &ppswSNWCoefAs
//  &pswNewFrmSNWCoef
//    swPrevR0
//  &ppswSynthAs
//  &pswNewFrmKs
//  &psnsSqrtRs
//  &pswPrevFrmAs
//     swNewR0

    // REG0 for &psnsSqrtRs
    VoC_lw16_sd(REG0, 2, DEFAULT);

    VoC_lw16i_short(INC0,2,DEFAULT);
    // pop &psnsSqrtRs[0]
    VoC_pop32(ACC0,DEFAULT);

    // save &psnsSqrtRs[0] to &psnsSqrtRs[3]
    VoC_loop_i_short(4,DEFAULT);
    VoC_movreg32(ACC0,REG23,IN_PARALLEL);
    VoC_startloop0;
    exit_on_warnings=OFF;
    // the first one is &psnsSqrtRs[0]
    // others are &psnsSqrtRs[1]
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;

    // REG0 for &ppswSNWCoefAs
    VoC_lw16_sd(REG0,7,DEFAULT);
    // REG1 for &pswPrevFrmSNWCoef
    VoC_lw16_sd(REG1,8,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    VoC_lw16i_short(INC2,2,DEFAULT);
    // REG2 for &ppswSynthAs
    VoC_lw16_sd(REG2,4,IN_PARALLEL);
    // REG3 for &pswPrevFrmAs
    VoC_lw16_sd(REG3,1,DEFAULT);

    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    // the first time, copy previous group (pswPrevFrmPFNum[i] etc.)
    VoC_loop_i(1,4);

    VoC_loop_i_short(5, DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG1,DEFAULT);
    VoC_lw32inc_p(ACC1,REG3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);
    VoC_endloop0
    // in the 3 other time, copy new group (pswNewFrmPFNum[i] etc.)
    VoC_lw16_sd(REG1,6,DEFAULT);

//     STACK16                          STACK32
//---------------------------------------------------------
//  &pswPrevFrmKs              &psrNewCont AND &psrOldCont
//  &pswNewFrmAs
//  &ppswPFDenomAs(&ppswSNWCoefAs)
//  &pswNewFrmPFDenom(&pswNewFrmSNWCoef)
//  &pswPrevFrmPFDenom(&pswPrevFrmSNWCoef)
//  &pswPrevFrmSNWCoef
//  &ppswSNWCoefAs
//  &pswNewFrmSNWCoef
//    swPrevR0
//  &ppswSynthAs
//  &pswNewFrmKs
//  &psnsSqrtRs
//  &pswPrevFrmAs
//     swNewR0

    VoC_lw16_sd(REG3,12,DEFAULT);
    VoC_endloop1;

    //  treat Denom group(in case of Tx, the following will be SNWCoef)
    VoC_lw16_sd(REG0,11,DEFAULT);
    VoC_lw16_sd(REG1,9,DEFAULT);
    VoC_loop_i(1,4);
    VoC_lw32inc_p(ACC0,REG1,DEFAULT);
    VoC_loop_i_short(5,DEFAULT);

    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_lw16_sd(REG1,10,DEFAULT);
    VoC_NOP();
    VoC_endloop1;

//     STACK16                          STACK32
//---------------------------------------------------------
//  &pswPrevFrmKs              &psrNewCont AND &psrOldCont
//  &pswNewFrmAs
//  &ppswPFDenomAs(&ppswSNWCoefAs)
//  &pswNewFrmPFDenom(&pswNewFrmSNWCoef)
//  &pswPrevFrmPFDenom(&pswPrevFrmSNWCoef)
//  &pswPrevFrmSNWCoef
//  &ppswSNWCoefAs
//  &pswNewFrmSNWCoef
//    swPrevR0
//  &ppswSynthAs
//  &pswNewFrmKs
//  &psnsSqrtRs
//  &pswPrevFrmAs
//     swNewR0


GETSFRMLPC_IEF1:

    VoC_lw16_d(SP16,sfrmAnalysis_SP16_SAVE0_ADDR);
    VoC_NOP();
    VoC_pop16(RA,DEFAULT);
    VoC_pop32(RL7,DEFAULT);
    VoC_return;

/////////////////////////////////////////////////////////////
// end inline FUNC CII_getSfrmLpcTx_opt
////////////////////////////////////////////////////////////
}


#ifdef voc_compile_only
// VoC_directive: FAKE_FUNCTION_ON
void CII_AMR_Encode(void) {};
void CII_AMR_Decode(void) {};
void CII_FR_Encode(void) {};
void CII_FR_Decode(void) {};
// VoC_directive: FAKE_FUNCTION_OFF
#endif





