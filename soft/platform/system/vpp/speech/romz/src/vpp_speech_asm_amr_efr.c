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

/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
*****************************************************************************
*/

#if 0

voc_short STRUCT_POST_FILTERSTATE_RES2_ADDR                 ,40,x       //40
voc_short STRUCT_POST_FILTERSTATE_MEM_SYN_PST_ADDR          ,10,x       //10
voc_short STRUCT_PREEMPHASISSTATE_ADDR                        ,x
voc_short STRUCT_AGCSTATE_ADDR                                ,x
voc_short STRUCT_POST_FILTERSTATE_SYNTH_BUF_ADDR                 ,170,x   //170


voc_short STRUCT_POST_PROCESSSTATE_Y2_LO_ADDR                     ,x     //1
voc_short STRUCT_POST_PROCESSSTATE_Y2_HI_ADDR                     ,x    //1
voc_short STRUCT_POST_PROCESSSTATE_Y1_LO_ADDR                     ,x     //1
voc_short STRUCT_POST_PROCESSSTATE_Y1_HI_ADDR                     ,x    //1
voc_short STRUCT_POST_PROCESSSTATE_X0_ADDR                        ,x     //1
voc_short STRUCT_POST_PROCESSSTATE_X1_ADDR                        ,x     //1


voc_alias GLOBAL_RXDTX_CTRL_ADDR                                STRUCT_POST_PROCESSSTATE_Y2_LO_ADDR    ,X
voc_alias STATIC_PREV_SID_FRAMES_LOST_ADDR                      STRUCT_POST_PROCESSSTATE_Y2_HI_ADDR    ,X
voc_alias GLOBAL_RX_DTX_STATE_ADDR                              STRUCT_POST_PROCESSSTATE_Y1_LO_ADDR    ,x
voc_alias STATIC_RXDTX_N_ELAPSED_ADDR                           STRUCT_POST_PROCESSSTATE_Y1_HI_ADDR    ,x
voc_alias STATIC_RXDTX_AVER_PERIOD_ADDR                         STRUCT_POST_PROCESSSTATE_X0_ADDR       ,x
voc_alias STATIC_BUF_P_RX_ADDR                                  STRUCT_POST_PROCESSSTATE_X1_ADDR       ,x

voc_short STURCT_SPEECH_DECODE_FRAMESTATE_PREV_MODE_ADDR          ,8,x     //8
voc_short STATIC_L_SACF_ADDR                                      ,54,x     //54
voc_short GLOBAL_CN_EXCITATION_GAIN_ADDR                          ,x      //1
voc_short GLOBAL_GCODE0_CN_ADDR                                   ,x    //1
voc_short SCAL_ACF_ADDR_P                                         ,x     // 1
voc_short VAD_FLAG_ADDR_P                                         ,x      // 1
voc_short TX_SP_FLAG_ADDR                                         ,x     // 1
voc_short GLOBAL_TXDTX_CTRL_ADDR                                  ,x      // 1
voc_short PRAM4_TABLE_ADDR                                        ,10,x     //  10
voc_short STRUCT_PRE_PROCESSSTATE_Y2_LO_ADDR                      ,x     //   1
voc_short STRUCT_PRE_PROCESSSTATE_Y2_HI_ADDR                      ,x     //   1
voc_short STRUCT_PRE_PROCESSSTATE_Y1_LO_ADDR                      ,x     //   1
voc_short STRUCT_PRE_PROCESSSTATE_Y1_HI_ADDR                      ,x    //   1
voc_short STRUCT_PRE_PROCESSSTATE_X0_ADDR                         ,x     //   1
voc_short STRUCT_PRE_PROCESSSTATE_X1_ADDR                         ,x    //   1
voc_short STRUCT_COD_AMRSTATE_OLD_SPEECH_ADDR                     ,40,x   // 320
voc_short STRUCT_COD_AMRSTATE_P_WINDOW_12K2_ADDR                   ,40,x
voc_short STRUCT_COD_AMRSTATE_P_WINDOW_ADDR                        ,40,x
voc_short STRUCT_COD_AMRSTATE_SPEECH_ADDR                          ,40,x
voc_short STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR                    ,161,x
voc_short STRUCT_COD_AMRSTATE_OLD_WSP_ADDR                        ,143,x
voc_short STRUCT_COD_AMRSTATE_WSP_ADDR                            ,160,x
voc_short STRUCT_COD_AMRSTATE_OLD_LAGS_ADDR                       ,6,x
voc_short STRUCT_COD_AMRSTATE_OL_GAIN_FLG_ADDR                    ,2,x
voc_short STRUCT_COD_AMRSTATE_OLD_EXC_ADDR                        ,154,x
voc_short STRUCT_COD_AMRSTATE_EXC_ADDR                           ,161,x
voc_short STRUCT_COD_AMRSTATE_AI_ZERO_ADDR                      ,11,x
voc_short STRUCT_COD_AMRSTATE_ZERO_ADDR                         ,41,x
voc_short STRUCT_LEVINSONSTATE_OLD_A_ADDR                       ,11,x
voc_short STRUCT_LSPSTATE_LSP_OLD_ADDR                         ,10,x     //  10
voc_short STRUCT_LSPSTATE_LSP_OLD_Q_ADDR                        ,10,x   //  10
voc_short STRUCT_Q_PLSFSTATE_PAST_RQ_ADDR                       ,10,x    //  10
voc_short STRUCT_PITCH_FRSTATE_T0_PREV_SUBFRAME_ADDR             ,2,x
voc_short STRUCT_GAINQUANTSTATE_SF0_FRAC_GCODE0_ADDR              ,x
voc_short STRUCT_GAINQUANTSTATE_SF0_EXP_GCODE0_ADDR               ,x
voc_short STRUCT_GAINQUANTSTATE_SF0_EXP_TARGET_EN_ADDR            ,x
voc_short STRUCT_GAINQUANTSTATE_SF0_FRAC_TARGET_EN_ADDR           ,x
voc_short STRUCT_GAINQUANTSTATE_SF0_EXP_COEFF_ADDR               ,6 ,x       // 6
voc_short STRUCT_GAINQUANTSTATE_SF0_FRAC_COEFF_ADDR               ,5,x         // 5
voc_short STRUCT_GAINQUANTSTATE_GAIN_IDX_PTR_ADDR                 ,x
voc_short STRUCT_GC_PREDST_ADDR                                   ,4,x          // ~23
voc_short STRUCT_GC_PREDST_PAST_QUA_EN_MR122_ADDR                 ,4,x        // ~7
voc_short STURCT_GC_PREDUNQST_ADDR                               ,4,x           //~31
voc_short STRUCT_GC_PREDUNQST_PAST_QUA_EN_MR122_ADDR             ,4,x         //~7
voc_short STRUCT_GAINADAPTSTATE_ONSET_ADDR                       ,x
voc_short STRUCT_GAINADAPTSTATE_PREV_ALPHA_ADDR                  ,x
voc_short STRUCT_GAINADAPTSTATE_PREV_GC_ADDR                     ,2,x
voc_short STRUCT_GAINADAPTSTATE_LTPG_MEM_ADDR                    ,6,x
voc_short STRUCT_PITCHOLWGHTSTATE_OLD_T0_MED_ADDR                ,x
voc_short STRUCT_PITCHOLWGHTSTATE_ADA_W_ADDR                     ,x
voc_short STRUCT_PITCHOLWGHTSTATE_WGHT_FLG_ADDR                  ,2,x
voc_short STRUCT_TONSTABSTATE_GP_ADDR                            ,7,x
voc_short STRUCT_TONSTABSTATE_COUNT_ADDR                          ,x
voc_short STRUCT_VADSTATE1_BCKR_EST_ADDR                         ,10,x
voc_short STRUCT_VADSTATE1_AVE_LEVEL_ADDR                        ,10 ,x
voc_short STRUCT_VADSTATE1_OLD_LEVEL_ADDR                        ,10 ,x
voc_short STRUCT_VADSTATE1_SUB_LEVEL_ADDR                        ,10 ,x
voc_short STRUCT_VADSTATE1_A_DATA5_ADDR                          ,6 ,x
voc_short STRUCT_VADSTATE1_A_DATA3_ADDR                          ,6 ,x
voc_short STRUCT_VADSTATE1_BURST_COUNT_ADDR                      ,x
voc_short STRUCT_VADSTATE1_HANG_COUNT_ADDR                       ,x
voc_short STRUCT_VADSTATE1_STAT_COUNT_ADDR                       ,x
voc_short STRUCT_VADSTATE1_VADREG_ADDR                           ,x
voc_short STRUCT_VADSTATE1_PITCH_ADDR                            ,x
voc_short STRUCT_VADSTATE1_TONE_ADDR                             ,x
voc_short STRUCT_VADSTATE1_COMPLEX_HIGH_ADDR                     ,x
voc_short STRUCT_VADSTATE1_COMPLEX_LOW_ADDR                      ,x
voc_short STRUCT_VADSTATE1_OLDLAG_COUNT_ADDR                     ,x
voc_short STRUCT_VADSTATE1_OLDLAG_ADDR                           ,x
voc_short STRUCT_VADSTATE1_COMPLEX_HANG_COUNT_ADDR               ,x
voc_short STRUCT_VADSTATE1_COMPLEX_HANG_TIMER_ADDR               ,x
voc_short STRUCT_VADSTATE1_BEST_CORR_HP_ADDR                     ,x
voc_short STRUCT_VADSTATE1_SPEECH_VAD_DECISION_ADDR              ,x
voc_short STRUCT_VADSTATE1_COMPLEX_WARNING_ADDR                  ,x
voc_short STRUCT_VADSTATE1_SP_BURST_COUNT_ADDR                   ,x
voc_short STRUCT_VADSTATE1_CORR_HP_FAST_ADDR                     ,x
voc_short STRUCT_COD_AMRSTATE_DTX_ADDR                           ,x
voc_short STRUCT_DTX_ENCSTATE_LSP_HIST_ADDR                      ,80 ,x
voc_short STRUCT_DTX_ENCSTATE_LOG_EN_HIST_ADDR                   ,8 ,x
voc_short STRUCT_DTX_ENCSTATE_HIST_PTR_ADDR                      ,x
voc_short STRUCT_DTX_ENCSTATE_LOG_EN_INDEX_ADDR                  ,x
voc_short STRUCT_DTX_ENCSTATE_INIT_LSF_VQ_INDEX_ADDR             ,2,x
voc_short STRUCT_DTX_ENCSTATE_LSP_INDEX_ADDR                     ,4,x
voc_short STRUCT_DTX_ENCSTATE_DTXHANGOVERCOUNT_ADDR              ,x
voc_short STRUCT_DTX_ENCSTATE_DECANAELAPSEDCOUNT_ADDR            ,x
voc_short STRUCT_COD_AMRSTATE_MEM_SYN_ADDR                       ,10,x
voc_short STRUCT_COD_AMRSTATE_MEM_W0_ADDR                        ,10,x
voc_short STRUCT_COD_AMRSTATE_MEM_W_ADDR                         ,10,x
voc_short STRUCT_COD_AMRSTATE_MEM_ERROR_ADDR                     ,10,x
voc_short STRUCT_COD_AMRSTATE_ERROR_ADDR                         ,40,x
voc_short STRUCT_COD_AMRSTATE_SHARP_ADDR                         ,x
voc_short STRUCT_SPEECH_ENCODE_FRAMESTATE_DTX_ADDR               ,x
voc_short STRUCT_SID_SYNCSTATE_SID_UPDATE_RATE_ADDR              ,x
voc_short STRUCT_SID_SYNCSTATE_SID_UPDATE_COUNTER_ADDR           ,x
voc_short STRUCT_SID_SYNCSTATE_SID_HANDOVER_DEBT_ADDR            ,x
voc_short STRUCT_SID_SYNCSTATE_PREV_FT_ADDR                      ,x

#endif

#if 0

voc_short   STRUCT_COD_AMRSTATE_HVEC_ADDR                     ,40,y
voc_short   STRUCT_COD_AMRSTATE_H1_ADDR                       ,40,y


voc_short   STATIC_SCAL_RVAD_ADDR                             ,y
voc_alias   COD_AMR_CODE_SF0_ADDRESS     STATIC_SCAL_RVAD_ADDR  ,y

voc_short   STATIC_RVAD_ADDR                                  ,9,y
voc_short   STATIC_BURSTCOUNT_ADDR                            ,y
voc_short   STATIC_HANGCOUNT_ADDR                             ,y
voc_short   STATIC_PT_SACF_ADDR                               ,y
voc_short   STATIC_PT_SAV0_ADDR                               ,y
voc_short   STATIC_L_LASTDM_ADDR                              ,2,y
voc_short   STATIC_ADAPTCOUNT_ADDR                            ,y
voc_short   GLOBAL_PTCH_ADDR                                  ,y
voc_short   STATIC_THVAD_ADDR                                 ,4,y
voc_short   STATIC_LSF_P_CN_ADDR                              ,10,y
voc_short   GLOBAL_LSF_OLD_TX_ADDR                            ,10,y    //70

voc_short  COD_AMR_XN_SF0_ADDRESS                             ,40,y
voc_short  COD_AMR_Y2_SF0_ADDRESS                             ,20,y


voc_short   STATIC_TXDTX_HANGOVER_ADDR                        ,y
voc_short   STATIC_TXDTX_N_ELAPSED_ADDR                       ,y
voc_short   GLOBAL_L_PN_SEED_TX_ADDR                          ,2,y
voc_short   STATIC_VERYOLDLAGCOUNT_ADDR                       ,y
voc_short   STATIC_BUF_P_TX_ADDR                              ,y
voc_short   STATIC_OLD_CN_MEM_TX_ADDR                         ,8,y
voc_short   GLOBAL_GAIN_CODE_OLD_TX_ADDR                      ,6,y//,28,y


voc_short   COD_AMR_T0_SF0_ADDRESS                       ,y
voc_short   COD_AMR_T0_FRAC_SF0_ADDRESS                  ,y
voc_short   COD_AMR_I_SUBFR_SF0_ADDRESS                  ,2,y
voc_short   COD_AMR_GAIN_PIT_SF0_ADDRESS                 ,y
voc_short   COD_AMR_GAIN_CODE_SF0_ADDRESS                ,y
voc_short   Blankkk                                        ,16 ,y

voc_short   GLOBAL_L_PN_SEED_RX_ADDR                          ,2,y
voc_short   GLOBAL_GCODE0_CN_DEC_ADDR                         ,y
voc_short   GLOBAL_GAIN_CODE_OLD_CN_DEC_ADDR                  ,y
voc_short   GLOBAL_GAIN_CODE_NEW_CN_DEC_ADDR                  ,y
voc_short   GLOBAL_GAIN_CODE_MUTING_CN_DEC_ADDR               ,y


#endif




#if 0

voc_short  STRUCT_DECOD_AMRSTATE_OLD_EXC_ADDR                 ,154 ,x     // reset 154
voc_short  STRUCT_DECOD_AMRSTATE_EXC_ADDR                     ,40 ,x
voc_short  STRUCT_DECOD_AMRSTATE_LSP_OLD_ADDR                 ,10 ,x       // reset 10
voc_short  STRUCT_DECOD_AMRSTATE_MEM_SYN_ADDR                 ,10 ,x       // reset 10
voc_short  STRUCT_DECOD_AMRSTATE_SHARP_ADDR                   ,x           // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR                  ,x           // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR                 ,x           // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_PREV_PDF_ADDR                ,x           // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_STATE_ADDR                   ,x           // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_T0_LAGBUFF_ADDR              ,x           // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_EXCENERGYHIST_ADDR           ,10,x        // reset 9
voc_short  STRUCT_DECOD_AMRSTATE_INBACKGROUNDNOISE_ADDR       ,x          // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_VOICEDHANGOVER_ADDR          ,x          // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_LTPGAINHISTORY_ADDR          ,9,x        // reset 9
voc_short  STRUCT_DECOD_AMRSTATE_NODATASEED_ADDR              ,x          // reset 1
voc_short  STRUCT_BGN_SCDSTATE_FRAMEENERGYHIST_ADDR           ,60,x      // reset 60
voc_short  STRUCT_BGN_SCDSTATE_BGHANGOVER_ADDR                ,2,x       // reset 1
voc_short  STRUCT_CB_GAIN_AVERAGESTATE_ADDR                   ,8,x       // alias  // reset 8
voc_short  STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR         ,x          // reset 1
voc_short  STRUCT_CB_GAIN_AVERAGESTATE_HANGVAR_ADDR           ,x          // reset 1
voc_short  STRUCT_LSP_AVGSTATE_LSP_MEANSAVE_ADDR              ,10,x     // reset 10
voc_short  STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR                ,10,x        // reset 10
voc_short  STRUCT_D_PLSFSTATE_PAST_R_Q_ADDR                  ,10,x        // reset 10
voc_short  STRUCT_EC_GAIN_PITCHSTATE_ADDR                    ,6,x         // alias  // reset 5
voc_short  STRUCT_EC_GAIN_PITCHSTATE_PAST_GAIN_PIT_ADDR      ,x          // reset 1
voc_short  STRUCT_EC_GAIN_PITCHSTATE_PREV_GP_ADDR            ,x          // reset 1
voc_short  STRUCT_EC_GAIN_CODESTATE_ADDR                     ,6,x        // alias    // reset 6
voc_short  STRUCT_EC_GAIN_CODESTATE_PAST_GAIN_CODE_ADDR      ,x         // reset 1
voc_short  STRUCT_EC_GAIN_CODESTATE_PREV_GC_ADDR             ,x         // reset 1
voc_short  STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR             ,4,x       // reset 4
voc_short  STRUCT_GC_PREDSTATE_PAST_QUA_EN_MR122_ADDR       ,4,x       // reset 4
voc_short  STRUCT_PH_DISPSTATE_GAINMEM_ADDR                 ,6,x     // reset 6
voc_short  STRUCT_PH_DISPSTATE_PREVSTATE_ADDR               ,x
voc_short  STRUCT_PH_DISPSTATE_PREVCBGAIN_ADDR               ,x
voc_short  STRUCT_PH_DISPSTATE_LOCKFULL_ADDR                ,x
voc_short  STRUCT_PH_DISPSTATE_ONSET_ADDR                   ,x
voc_short  STRUCT_DTX_DECSTATE_SINCE_LAST_SID_ADDR         ,x          // reset 1
voc_short  STRUCT_DTX_DECSTATE_TRUE_SID_PERIOD_INV_ADDR     ,x         // reset 1
voc_short  STRUCT_DTX_DECSTATE_LOG_EN_ADDR                  ,x         // reset 1
voc_short  STRUCT_DTX_DECSTATE_OLD_LOG_EN_ADDR              ,x         // reset 1
voc_short  STRUCT_DTX_DECSTATE_L_PN_SEED_RX_ADDR            ,2,x       // reset 2
voc_short  STRUCT_DTX_DECSTATE_LSP_ADDR                     ,10,x       // reset 10
voc_short  STRUCT_DTX_DECSTATE_LSP_OLD_ADDR                 ,10,x       // reset 10
voc_short  STRUCT_DTX_DECSTATE_LSF_HIST_ADDR                ,80,x       // reset 80
voc_short  STRUCT_DTX_DECSTATE_LSF_HIST_PTR_ADDR            ,x      // reset 1
voc_short  STRUCT_DTX_DECSTATE_LOG_PG_MEAN_ADDR             ,x      // reset 1


voc_short  DEC_STATIC_LSF_P_CN_ADDR             ,10,x      // reset 80
voc_short  GLOBAL_LSF_OLD_RX_ADDR           ,70,x

voc_short  STRUCT_DTX_DECSTATE_LOG_EN_HIST_ADDR             ,8,x      // reset 8


voc_short  STRUCT_DTX_DECSTATE_LOG_EN_HIST_PTR_ADDR         ,x
voc_short  STRUCT_DTX_DECSTATE_LOG_EN_ADJUST_ADDR           ,x      // reset 1
voc_short  STRUCT_DTX_DECSTATE_DTXHANGOVERCOUNT_ADDR        ,x      // reset 1
voc_short  STRUCT_DTX_DECSTATE_DECANAELAPSEDCOUNT_ADDR      ,x      // reset 1
voc_short  STRUCT_DTX_DECSTATE_SID_FRAME_ADDR               ,x      // reset 1
voc_short  STRUCT_DTX_DECSTATE_VALID_DATA_ADDR              ,x      // reset 1
voc_short  STRUCT_DTX_DECSTATE_DTXHANGOVERADDED_ADDR        ,x      // reset 1
voc_short  STRUCT_DTX_DECSTATE_DTXGLOBALSTATE_ADDR          ,x      // reset 1
voc_short  STRUCT_DTX_DECSTATE_DATA_UPDATED_ADDR            ,2,x       // reset 1
voc_short  STATIC_L_SAV0_ADDR                               ,72,x       //72
voc_short  GLOBAL_LSF_NEW_CN_DEC_ADDR                       ,10,x    // 10
voc_short  GLOBAL_LSF_OLD_CN_DEC_ADDR                       ,10,x    // 10
voc_short  GLOBAL_GAIN_CODE_OLD_RX_ADDR                     ,28,x    // 28

#endif






void CII_AMR_Encode(void)
{
#if 0
    voc_short      COD_AMR_XN_ADDRESS                       ,40,x            //[40]shorts
    voc_short      COD_AMR_XN2_ADDRESS                      ,40,x            //[40]shorts
    voc_short      COD_AMR_Y1_ADDRESS                       ,40,x            //[40]shorts
    voc_short      COD_AMR_Y2_ADDRESS                       ,40,x            //[40]shorts
    voc_short      COD_AMR_RES_ADDRESS                      ,40,x            //[40]shorts
    voc_short      COD_AMR_MEM_SYN_SAVE_ADDRESS             ,10,x            //[10]shorts
    voc_short      COD_AMR_MEM_W0_SAVE_ADDRESS              ,10,x            //[10]shorts
    voc_short      COD_AMR_MEM_ERR_SAVE_ADDRESS             ,10,x            //[10]shorts
    voc_short      COD_AMR_GCOEFF_ADDRESS                   ,4 ,x            //[4]shorts
    voc_short      COD_AMR_SUBFRNR_ADDRESS                     ,x            //1 short
    voc_short      COD_AMR_I_SUBFR_ADDRESS                     ,x                //1 short
    voc_short      COD_AMR_T_OP_ADDRESS                     ,2 ,x            //[2]shorts
    voc_short      COD_AMR_T0_ADDRESS                          ,x                //1 short
    voc_short      COD_AMR_T0_FRAC_ADDRESS                     ,x                //1 short
    voc_short      COD_AMR_GAIN_PIT_ADDRESS                    ,x            //1 short
    voc_short      COD_AMR_GAIN_CODE_ADDRESS                   ,x            //1 short
    voc_short      COD_AMR_EXC_ADDR_ADDRESS                 ,2 ,x            //[2]shorts
    voc_short      COD_AMR_LSP_FLAG_ADDRESS                    ,x            //1 short
    voc_short      COD_AMR_GP_LIMIT_ADDRESS                    ,x                //1 short
    voc_short      COD_AMR_VAD_FLAG_ADDRESS                    ,x                //1 short
    voc_short      COD_AMR_COMPUTE_SID_FLAG_ADDRESS            ,x                //1 short
    voc_short      COD_AMR_A_T_ADDRESS                      ,44,x                //[44]shorts
    voc_short      COD_AMR_AQ_T_ADDRESS                     ,44,x                //[44]shorts
    voc_short      COD_AMR_LSP_NEW_ADDRESS                  ,10,x                //[10]shorts
    voc_short      COD_AMR_SHARP_SAVE_ADDRESS                  ,x                //1 short
    voc_short      COD_AMR_EVENSUBFR_ADDRESS                   ,x                    //1 short
    voc_short      COD_AMR_CODE_ADDRESS                    ,40 ,x            //[40]shorts
    voc_alias       DEC_AMR_CODE_ADDRESS                COD_AMR_CODE_ADDRESS  ,X
#endif

#if 0
    voc_short    COD_AMR_ANA_ADDR                      ,60 ,y
    voc_short    COD_AMR_RES3_ADDRESS                      ,40 ,y
    voc_short    BLANK                      ,2 ,y
#endif

    VoC_push16( RA,DEFAULT);

    VoC_or16_rd(REG2,GLOBAL_AMR_EFR_RESET_ADDR);    //reset_flag

    VoC_bez16_r(NO_RESET_ENC,REG2)
    //reset_enc
    VoC_jal(CII_encoder_reset);

    /*get new speech from in buffer*/

NO_RESET_ENC:
    VoC_lw16i_short(REG6,0XFFF8,DEFAULT);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR,2);

    VoC_movreg16(REG7,REG6,DEFAULT);
    VoC_lw16d_set_inc(REG0,ENC_INPUT_ADDR_ADDR,2);

    VoC_loop_i(0,80)

    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_and32_rr(RL6,REG67,DEFAULT);
    VoC_NOP();
    VoC_sw32inc_p(RL6,REG1,DEFAULT);
    VoC_endloop0

    /* Check whether this frame is an encoder homing frame */

    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR,2);
    VoC_jal(CII_encoder_homing_frame_test);
    VoC_sw16_d(REG4,GLOBAL_AMR_EFR_RESET_ADDR);

    // preprocess filter, copy(new_speech, st->new_speech, L_FRAME);
    VoC_jal(CII_Pre_Process);

    // if DTX or echo cancel and not EFR, do vad
    VoC_bnez16_d(no_vad_detection_needed,GLOBAL_EFR_FLAG_ADDR)
    VoC_bez16_d(no_vad_detection_needed,GLOBAL_DTX_ADDR)

    // vad_flag = vad1(st->vadSt, st->new_speech);
    VoC_jal(CII_vad1);
    // vad_flag in REG4
    // return in REG5

no_vad_detection_needed:

    /* DTX processing */
    VoC_bnez16_d(cod_amr301,GLOBAL_EFR_FLAG_ADDR)                 // if (efr)
    VoC_bez16_d(cod_amr301,GLOBAL_DTX_ADDR)   // if (st->dtx)

    /* NB! usedMode may change here */
    // compute_sid_flag = tx_dtx_handler(st->dtx_encSt, vad_flag, usedMode);
    VoC_lw16_d(REG7, STRUCT_VADSTATE1_SPEECH_VAD_DECISION_ADDR);
    VoC_jal(CII_tx_dtx_handler);
    VoC_sw16_d(REG5,COD_AMR_COMPUTE_SID_FLAG_ADDRESS);

cod_amr301:


    /*------------------------------------------------------------------------*
     *  - Perform LPC analysis:                                               *
     *       * autocorrelation + lag windowing                                *
     *       * Levinson-durbin algorithm to find a[]                          *
     *       * convert a[] to lsp[]                                           *
     *       * quantize and code the LSPs                                     *
     *       * find the interpolated LSPs and convert to a[] for all          *
     *         subframes (both quantized and unquantized)                     *
     *------------------------------------------------------------------------*/

    /* LP analysis */


    VoC_lw16i(REG7,COD_AMR_ANA_ADDR);

// VoC_directive: PARSER_OFF

    VoC_lw16_d(REG0,GLOBAL_ENC_MODE_ADDR);
    VoC_bnez16_r(cod_amr475,REG0)
    VoC_jal(CII_COD_AMR_475);
    VoC_jump(cod_amr_end);
cod_amr475:
    VoC_bne16_rd(cod_amr515,REG0,CONST_1_ADDR)
    VoC_jal(CII_COD_AMR_515);
    VoC_jump(cod_amr_end);
cod_amr515:
    VoC_bne16_rd(cod_amr59,REG0,CONST_2_ADDR)
    VoC_jal(CII_COD_AMR_59);
    VoC_jump(cod_amr_end);
cod_amr59:
    VoC_bne16_rd(cod_amr67,REG0,CONST_3_ADDR)
    VoC_jal(CII_COD_AMR_67);
    VoC_jump(cod_amr_end);
cod_amr67:
    VoC_bne16_rd(cod_amr74,REG0,CONST_4_ADDR)
    VoC_jal(CII_COD_AMR_74);
    VoC_jump(cod_amr_end);
cod_amr74:
    VoC_bne16_rd(cod_amr795,REG0,CONST_5_ADDR)
    VoC_jal(CII_COD_AMR_795);
    VoC_jump(cod_amr_end);
cod_amr795:
    VoC_bne16_rd(cod_amr102,REG0,CONST_6_ADDR)
    VoC_jal(CII_COD_AMR_102);
    VoC_jump(cod_amr_end);
cod_amr102:

// VoC_directive: PARSER_ON

    VoC_jal(CII_COD_AMR_122);

cod_amr_end:;

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


/*****************************************************************************
 * Function CII_Copy_M()        REG0->REG1                                          *
  Input  registers: REG0(incr 2),REG1(incr 2)
  Used   registers:  REG0,REG1,ACC0                            *
 ******************************************************************************/

void CII_Copy_M(void)
{
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_loop_i_short(5,DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_return;
}

/*
INPUT:
REG0 and REG1 :sourec and dest address  INC 2
REG2   :scale value (shr)
REG3   :(number of Word16s to be scaled)/4

USERD   :REG45,REG67
*/
void CII_scale(void)
{
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_shr16_ri(REG3,-1,DEFAULT);
    VoC_loop_r_short(REG3,DEFAULT)
    VoC_startloop0
    VoC_shr16_rr(REG4,REG2,DEFAULT);
    VoC_shr16_rr(REG5,REG2,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(REG45,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_return
}



void CII_copy_xy(void)
{
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_shr16_ri(REG6,-1,IN_PARALLEL);
    VoC_loop_r_short(REG6,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_return

}

/*
INPUT:
REG0: source address INC 2
REG1: dest   address INC 2
REG6: (number of Long Words copyed)/2 or (number of Word16 copyed)/4

USED:   ACC0,ACC1
*/


/*
need
r3:dest addr
r2:i0
    */
//used regigister  REG0 ,1,2,3 ,4,

void CII_SEARCH_COPY(void)
{
// compute first address i0*(i0-1)/2 +i0
    VoC_mult16_rr(REG0, REG2, REG2,DEFAULT);

    VoC_lw16i_short(INC2,1,DEFAULT);

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_movreg16(REG1, REG2,IN_PARALLEL);

    VoC_sub16_rr(REG0, REG0, REG2,DEFAULT);
    VoC_shr16_ri(REG0,1,DEFAULT);

    VoC_add32_rd(REG01, REG01, C1035PF_RR_SIDE_ADDR_ADDR);

    // copy abscissa number loop number is i0
    VoC_bez16_r(SEARCH_COPY_label_100, REG2)
    VoC_lw16inc_p(REG4, REG0,DEFAULT);
    VoC_loop_r_short(REG2,DEFAULT);
    VoC_lw16inc_p(REG4, REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4, REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4, REG3,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_sub16_rd(REG0, REG0,CONST_2_ADDR);  // read a useless word
SEARCH_COPY_label_100:

    // copy ordinate number loop number is 39-i0
    VoC_lw16_p(REG1, REG1,DEFAULT);
    VoC_add16_rr(REG0, REG0, REG2,IN_PARALLEL);
    VoC_sub16_dr(REG1, CONST_39_ADDR, REG2);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG1, REG3,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_bez16_r(SEARCH_COPY_label2_200, REG1)
    VoC_lw16_p(REG4, REG0,DEFAULT);
    VoC_add16_rr(REG0, REG0, REG2,IN_PARALLEL);
    VoC_loop_r_short(REG1,DEFAULT)
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16_p(REG4, REG0,DEFAULT);
    VoC_add16_rr(REG0, REG0, REG2,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4, REG3,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop0;
SEARCH_COPY_label2_200:
    VoC_return
}



void CII_encoder_reset (void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw32z(RL7,DEFAULT);
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_lw32z(ACC1,DEFAULT);

    VoC_lw16i_set_inc(REG0,STATIC_L_SACF_ADDR,2);
    VoC_loop_i(1,179)
    VoC_sw32inc_p(RL7,REG0,DEFAULT);
    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_endloop1;
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_HVEC_ADDR,2);
    VoC_loop_i(1,112)
    VoC_sw32inc_p(RL7,REG0,DEFAULT);
    VoC_endloop1;

    VoC_bez16_d(RESET_EFR,GLOBAL_EFR_FLAG_ADDR)
// EFR VAD & DTX part
    VoC_jal(CII_EFR_DTX_ENC_RESET);
    VoC_jump(RESET_AMR);
RESET_EFR:
    VoC_jal(CII_RESET_AMR_CONSTANT);
RESET_AMR:
    VoC_lw16i(REG6,AMR_EFR_RESET_Offset_X );


    VoC_lw16i_set_inc(REG0,AMR_ENC_RESET_TABLE_ADDR,3); // reset address
    VoC_lw16i_set_inc(REG1,AMR_ENC_RESET_TABLE_ADDR+1,3);// reset long
    VoC_lw16i(REG4,AMR_ENC_RESET_TABLE_ADDR+2);// reset value

    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_loop_i(1,24)
    VoC_lw16inc_p(REG2,REG0,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_lw16_p(REG3,REG4,DEFAULT);
    VoC_add16_rd(REG4,REG4,CONST_3_ADDR);
    VoC_lw16inc_p(REG7,REG3,DEFAULT);
    VoC_loop_r_short(REG5,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG3,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_NOP();
    VoC_endloop1

    VoC_lw16i_short(REG4,8,DEFAULT);
    VoC_lw16i_short(REG6,3,DEFAULT);
    VoC_sw16_d(REG4,STRUCT_SID_SYNCSTATE_SID_UPDATE_RATE_ADDR);
    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG6,STRUCT_SID_SYNCSTATE_SID_UPDATE_COUNTER_ADDR);

    VoC_return;
}


/***********************************************
 Function : CII_dtx_com_s2
 Input: REG0(incr=2)    :&lsf[](&lsp[])
    REG1(incr=2)    :&st->lsf_hist[](&st->lsp_hist[])
        REG7        :st->lsf_hist_ptr(st->hist_ptr*8)
    REG2(incr=2)    :&frame[](&speech[])
 Output:REG4        :log_en
 Note: dtx_dec_activity_update and dtx_buffer share the same code
  Optimized by Kenneth  22/09/2004
***********************************************/

void CII_dtx_com_s2(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_jal(CII_Copy_M);                //Copy(lsf, &st->lsf_hist[st->lsf_hist_ptr], M);

    VoC_lw32z(ACC0,DEFAULT);            //L_frame_en = 0;
    VoC_loop_i(0,80);               //for (i=0; i < L_FRAME; i++)
    VoC_bimac32inc_pp(REG2,REG2);       //{   L_frame_en = L_mac(L_frame_en, frame[i], frame[i]);
    VoC_endloop0                    //}
    VoC_lw16i(REG6,8521);
//      VoC_movreg32(REG23,ACC0,DEFAULT);
    VoC_jal(CII_Log2);              //Log2(L_frame_en, &log_en_e, &log_en_m);
    VoC_shr16_ri(REG4,-10,DEFAULT);         //log_en = shl(log_en_e, 10);
    VoC_shr16_ri(REG5,5,DEFAULT);       //log_en = add(log_en, shr(log_en_m, 15-10));
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG6,DEFAULT);       // log_en = sub(log_en, 8521);
    VoC_return;
}


/****************************************************************************
 *Div_32
 *INPUT: REG45: L_nom;  REG7 :denom_hi; REG6:denom_lo, REG3 = 1
 *output:ACC0:  division result
 *       ACC1:  L_nom
 *used  :all
 *unchanged : REG3
 ***************************************************************************/

void CII_DIV_32(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_movreg32(ACC1,REG45,DEFAULT);

    VoC_bnltz32_r(DIV_32_L_ABS_IN,REG45)
    VoC_sub32_dr(REG45,CONST_WORD32_0_ADDR,REG45);   //    t1 = L_abs (t0);
DIV_32_L_ABS_IN:
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_lw16i(REG0,0x3fff);
    VoC_movreg16(REG1, REG7,DEFAULT);


    /****************************/
    // input in REG0, REG1
    // output in  REG2
    // used register RL67, REG012
    /****************************/

    VoC_jal(CII_DIV_S);
    // REG2 approx
    VoC_multf16_rr(REG0, REG6, REG2,DEFAULT);
    VoC_multf32_rr(ACC0, REG7, REG2,DEFAULT);

    VoC_mac32_rr(REG0, REG3,DEFAULT);
    VoC_shru16_ri(REG4, 1,DEFAULT);
    VoC_sub32_dr(REG67, CONST_0x7FFFFFFF_ADDR, ACC0);
    VoC_shru16_ri(REG6, 1,DEFAULT);

    VoC_multf16_rr(REG0, REG6, REG2,DEFAULT);
    VoC_multf32_rr(ACC0, REG7, REG2,DEFAULT);
    VoC_mac32_rr(REG0, REG3,DEFAULT);
    VoC_NOP();
    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_shru16_ri(REG6, 1,DEFAULT);

    VoC_multf32_rr(ACC0, REG5, REG7,DEFAULT);
    VoC_multf16_rr(REG0, REG5, REG6,IN_PARALLEL);

    VoC_multf16_rr(REG6, REG4, REG7,DEFAULT);
    VoC_mac32_rr(REG0, REG3,DEFAULT);


    VoC_mac32_rr(REG6, REG3,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_shr32_ri(ACC0,-2,DEFAULT);
    VoC_movreg32(REG67,ACC0,DEFAULT);

    VoC_bngtz32_r(DIV_32_L_ABS_OUT,ACC1)                    //   if (t0 > 0)
    VoC_sub32_dr(REG67,CONST_WORD32_0_ADDR,REG67);  //    t2 = L_negate (t2);         /* K =-t0/Alpha                */
DIV_32_L_ABS_OUT:

    VoC_return
}



/****************************************************************
    *1/sqrt
    *INPUT:         REG01
    *OUTPUT:        REG01
    *USED:          REG2,REG6,ACC0,LOOP0
    ******************************************************************/

void CII_Inv_sqrt(void)
{
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_bgt32_rd(INV_SQRT101,REG01,CONST_WORD32_0_ADDR)
    VoC_lw16i_short(REG0,(Word16)0xffff,DEFAULT);
    VoC_lw16i(REG1,0x3fff);

    VoC_return;
INV_SQRT101:
    /* REG2 for exp = norm_l (L_x);*/
    VoC_lw16i_set_inc(REG2,0,1);
    VoC_loop_i_short(31,DEFAULT)
    VoC_startloop0
    VoC_bnlt32_rd(NORM_L105,REG01,CONST_0x40000000_ADDR);
    VoC_inc_p(REG2,DEFAULT);
    VoC_shr32_ri(REG01,-1,IN_PARALLEL);
    VoC_endloop0

NORM_L105:;
    /*Normalization of L_x is done in norm_l*/

    VoC_sub16_dr(REG2,CONST_30_ADDR,REG2);

    /*If (exponent=30-exponent) is even then shift right once. */

    VoC_and16_rr(REG6,REG2,DEFAULT);
    VoC_shr16_ri(REG2,1,IN_PARALLEL);
    VoC_bnez16_r(INV_SQRT102,REG6)
    VoC_shr32_ri(REG01,1,DEFAULT);
INV_SQRT102:

    /*exponent = exponent/2  +1*/

    /* i = bit25-b31 of L_x; */
    VoC_shr32_ri(REG01,9,DEFAULT);          //REG1 for i

    /*i -=16*/
    VoC_sub16_rd(REG6,REG1,CONST_16_ADDR);
    /*a = bit10-b24*/
    VoC_inc_p(REG2,DEFAULT);
    VoC_shr32_ri(REG01,1,IN_PARALLEL);


    /*L_y = table[i]<<16 - (table[i] - table[i+1]) * a * 2*/
    VoC_lw16i_set_inc(REG1,TABLE_INV_SQRT_ADDR,1);
    VoC_add16_rr(REG1,REG6,REG1,DEFAULT);
    VoC_lw16i_short(ACC0_LO,0,IN_PARALLEL);
    VoC_and16_ri(REG0,0X7FFF);              //REG0 for a

    VoC_lw16inc_p(REG6,REG1,DEFAULT);
    VoC_movreg16(ACC0_HI,REG6,DEFAULT);         //ACC0 for L_y
    VoC_sub16_rp(REG1,REG6,REG1,IN_PARALLEL);
    VoC_msu32_rr(REG1,REG0,DEFAULT);
    VoC_NOP();
    /*L_y >>= exponent*/
    VoC_movreg32(REG01,ACC0,DEFAULT);
    VoC_shr32_rr(REG01,REG2,DEFAULT);

    //--------------------------------------------------------------------
    VoC_return
}


void CII_Lag_window (void)
{



    VoC_lw16i_set_inc(REG0,LPC_RLOW_ADDR+1,1);          //&r_l[1]
    VoC_lw16i_set_inc(REG1,LPC_RHIGH_ADDR+1,1);         //&r_h[1]
    VoC_lw16i_set_inc(REG2,TABLE_LAG_L_ADDR,1);             //&lag_l
    VoC_lw16i_set_inc(REG3,TABLE_LAG_H_ADDR,1);             //&lag_h

    VoC_lw32_d(REG45,CONST_0x00010001_ADDR);

    VoC_loop_i_short(10,DEFAULT)
    VoC_startloop0
    VoC_multf16_pp(REG6,REG1,REG2,DEFAULT);
    VoC_multf16_pp(REG7,REG0,REG3,DEFAULT);
    VoC_multf32_pp(ACC0,REG1,REG3,DEFAULT);
    VoC_bimac32_rr(REG45,REG67);
    VoC_add32_rr(REG23,REG23,REG45,DEFAULT);
    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_shru16_ri(REG6,1,DEFAULT);

    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_sw16inc_p(REG6,REG0,DEFAULT);

    VoC_endloop0

    VoC_return;

}



void CII_Levinson (void)
{

#if 0

    voc_short    Blank_LEVINSON                  ,3,y
    voc_short    PARA4_ADDR_ADDR                   ,y
    voc_short    LEVINSON_AL_ADDR                  ,22,y                        // 0~(M+1)*2 for both Al and Ah

    voc_short    LEVINSON_ANL_ADDR                 ,11,x                        //&Anl[0]

#endif





    VoC_push16(RA,DEFAULT);

    VoC_lw16i_set_inc(REG1,LPC_RHIGH_ADDR,1);
    VoC_lw16i_set_inc(REG0,LPC_RLOW_ADDR,1);

    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_push32(REG01,DEFAULT);

    // count=0;
    VoC_lw16inc_p(REG7,REG1,DEFAULT);      // Rh[0]
    VoC_lw16inc_p(REG6,REG0,DEFAULT);      // Rl[0]

    VoC_multf32_rp(REG45,REG3,REG0,DEFAULT);    // L_deposit_lo(Rl[1]) << 1
    VoC_push32(REG67,DEFAULT);
    VoC_lw16_p(REG5,REG1,DEFAULT);              // L_deposit_hi(Rh[1])
    /*************************
     *FUNC  :     Div_32
     *INPUT :     REG45 <- L_nom;
     *            REG67 <- L_denom;
     *OUTPUT:     REG67 <- L_res
     *            ACC1  <- L_nom
     *            REG3  <- 1
     *USED      : all
     *************************/

    VoC_jal(CII_DIV_32);
    VoC_movreg32(REG45,REG67,DEFAULT);
    VoC_lw32_d(REG23,CONST_0x7FFFFFFF_ADDR);
    VoC_add32_rd(ACC0,REG67,CONST_0x00008000L_ADDR);


    VoC_shru16_ri(REG6,1,DEFAULT);

//  VoC_and16_rr(REG6,REG3,DEFAULT);             //   L_Extract (t0, &Kh, &Kl);           /* K in DPF         */
    VoC_shr32_ri(REG45,4,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
//  VoC_and16_rr(REG4,REG3,DEFAULT);

    VoC_multf32_rr(ACC0,REG7,REG7,DEFAULT);
    VoC_multf16_rr(REG0,REG7,REG6,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw16_d(ACC0_HI,LPC_RC_ADDR);
    exit_on_warnings=ON;
    VoC_mac32_rd(REG0,CONST_1_ADDR);              //  REG3      1
    VoC_mac32_rd(REG0,CONST_1_ADDR);           // t0 = Mpy_32 (Kh, Kl, Kh, Kl);       /* K*K             */

    VoC_sw32_d(REG45,LEVINSON_AL_ADDR+2);    //  L_Extract (t0, &Ah[1], &Al[1]);     /* A[1] in DPF      */

    VoC_bnltz32_r(LEVINSON_L_ABS_2,ACC0)
    VoC_sub32_dr(ACC0,CONST_WORD32_0_ADDR,ACC0);
LEVINSON_L_ABS_2:

    VoC_sub32_rr(REG45,REG23,ACC0,DEFAULT); // REG23 = 0X7FFFFFFF
    VoC_shru16_ri(REG4,1,DEFAULT);
//  VoC_and16_rr(REG4,REG3,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);

    // REG2 = -1

    VoC_multf32_rr(ACC0,REG7,REG5,DEFAULT);
    VoC_multf16_rr(REG0,REG7,REG4,IN_PARALLEL);
    VoC_multf16_rr(REG0,REG6,REG5,DEFAULT);
    exit_on_warnings = OFF;
    VoC_msu32_rr(REG0,REG2,DEFAULT);
    exit_on_warnings = ON;
    VoC_msu32_rr(REG0,REG2,DEFAULT);

    /**************************/
    // func  : CII_NORM_L_ACC0
    // input : ACC0
    // output: REG1
    // used  : ACC0, REG1
    /**************************/

    VoC_jal(CII_NORM_L_ACC0);

    VoC_movreg32(REG45,ACC0,DEFAULT);
    VoC_shru16_ri(REG4,1,DEFAULT);
//  VoC_and16_rr(REG4,REG3,DEFAULT);    //REG3 = 0x7FFF

    VoC_push16(REG1,IN_PARALLEL);           // alp_exp

    VoC_lw16i_short(REG6,2,DEFAULT);    //i = 2
    VoC_push32(REG45,DEFAULT);
    VoC_push16(REG6,DEFAULT);

    //stack16  RA    alp_exp    i
    //stack32  Rh[0]&Rl[0],     alp_h&alp_l

    VoC_loop_i(1,9)

    VoC_lw32_sd(REG01,1,DEFAULT);   // &Rh[0], &Rl[0],
    VoC_lw16i_short(REG5,1,DEFAULT);

    VoC_lw16i_set_inc(REG2,LEVINSON_AL_ADDR-2+1,-1); // point to hi

    VoC_lw16_sd(REG6,0,DEFAULT);    // i
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_sub16_rr(REG7,REG6,REG4,DEFAULT);//number of loops
    VoC_shr16_ri(REG6,-1,IN_PARALLEL);  // 2*i

    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);      ////&Ah[i-1]

    // VoC_add16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_add32_rr(REG01,REG01,REG45,IN_PARALLEL);//  Rh[1], Rl[1],

    VoC_loop_r_short(REG7,DEFAULT)
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_mac32_pp(REG1,REG2,DEFAULT);
    VoC_multf16inc_pp(REG6,REG0,REG2,DEFAULT);
    VoC_multf16inc_pp(REG7,REG1,REG2,DEFAULT);
    VoC_NOP();
    VoC_bimac32_rr(REG67,REG45);
    VoC_endloop0

    VoC_lw32_sd(REG01,1,DEFAULT);       // Rh[0], Rl[0]
    VoC_lw16_sd(REG6,0,IN_PARALLEL);    // i

    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG6,IN_PARALLEL);       //Rh[i], Rl[i],

    VoC_shr32_ri(ACC0,-4,DEFAULT);

    VoC_multf32_rp(REG45,REG4,REG0,DEFAULT); // REG4 = 1
    VoC_lw32_sd(REG67,0,DEFAULT);
    VoC_lw16_p(REG5,REG1,DEFAULT);//  t1 = L_Comp (Rh[i], Rl[i]);

    VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);  //   t0 = L_add (t0, t1);            /* add R[i]        */


    /*************************
     *FUNC  :     Div_32
     *INPUT :     REG45 <- L_nom;
     *            REG67 <- L_denom;
     *OUTPUT:     REG67 <- L_res
     *            ACC1  <- L_nom
     *            REG3  <- 1
     *USED      : all
     *************************/
    VoC_jal(CII_DIV_32);

    VoC_lw16_sd(REG2,1,DEFAULT); // alp_exp
    VoC_sub16_dr(REG2,CONST_0_ADDR,REG2);

    VoC_shr32_rr(REG67,REG2,DEFAULT);
    VoC_add32_rd(ACC0,REG67,CONST_0x00008000L_ADDR);

    VoC_movreg32(ACC1, REG67,DEFAULT);   //t2
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    //  VoC_and16_ri(REG6,0X7FFF);    //Kh   Kl

    VoC_lw16i(REG0,LPC_RC_ADDR-1);
    VoC_lw16_sd(REG4,0,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);

    VoC_bnlt16_rd(LEVINSON_03,REG4,CONST_5_ADDR)
    VoC_sw16_p(ACC0_HI,REG0,DEFAULT);
LEVINSON_03:

    VoC_movreg16(REG5,REG7,DEFAULT);
    VoC_bnltz16_r(LEVINSON_ABS_S_1,REG5)
    VoC_sub16_dr(REG5,CONST_0_ADDR,REG5);
LEVINSON_ABS_S_1:

    VoC_lw16i(REG4,32750);

    VoC_bngt16_rr(LEVINSON_04,REG5,REG4)

    VoC_lw16i(REG0,STRUCT_LEVINSONSTATE_OLD_A_ADDR);
    VoC_lw16_d(REG1,PARA4_ADDR_ADDR);

    VoC_lw32z(RL6,DEFAULT);
    VoC_loop_i_short(11,DEFAULT)
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_sw32_d(RL6,LPC_RC_ADDR);
    VoC_sw32_d(RL6,LPC_RC_ADDR+2);

    VoC_jump(LEVINSON_EXIT);

LEVINSON_04:

    /*------------------------------------------*
     *  Compute new LPC coeff. -> An[i]         *
     *  An[j]= A[j] + K*A[i-j]     , j=1 to i-1 *
     *  An[i]= K                                *
     *------------------------------------------*/

    VoC_lw16_sd(REG4,0,DEFAULT);    // i
    VoC_lw16i_set_inc(REG2,LEVINSON_AL_ADDR-2+1,-1);//point to hi
    VoC_bez16_d(LEVINSON_100,GLOBAL_EFR_FLAG_ADDR)

    /* for (j = 1; j < i; j++)
    {
        t0 = Mpy_32 (Kh, Kl, Ah[i - j], Al[i - j]);
        t0 = L_mac (t0, Ah[j], 32767);
        t0 = L_mac (t0, Al[j], 1);
        L_Extract (t0, &Anh[j], &Anl[j]);
    } */

    VoC_jal(CII_EFR_LEVINSON_PART);
    // REG1 = CONST_0X00010001_ADDR

    VoC_jump(LEVINSON_101);

LEVINSON_100:

    /*
      t0 = Mpy_32 (Kh, Kl, Ah[i - j], Al[i - j]);
      t0 = L_add(t0, L_Comp(Ah[j], Al[j]));
      L_Extract (t0, &Anh[j], &Anl[j]);
    */
    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_lw16i_set_inc(REG3,LEVINSON_ANL_ADDR,2);

    VoC_lw16i_set_inc(REG1,CONST_0x00010001_ADDR,0);//&0X00010001
    VoC_lw16i_set_inc(REG0,LEVINSON_AL_ADDR+2,1);


    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_sub16_rp(REG4,REG4,REG1,IN_PARALLEL); //--

    VoC_multf32inc_pp(RL6,REG0,REG1,DEFAULT);
    VoC_loop_r_short(REG4,DEFAULT)                   //   for (j = 1; j < i; j++)

    VoC_startloop0
    VoC_multf32_rp(ACC0,REG7,REG2,DEFAULT);
    VoC_multf16inc_rp(REG5,REG6,REG2,IN_PARALLEL);
    VoC_multf16inc_rp(REG4,REG7,REG2,DEFAULT);
    VoC_NOP();
    VoC_bimac32inc_rp(REG45,REG1);                //    t0 = Mpy_32 (Kh, Kl, Ah[i - j], Al[i - j])
    VoC_lw16inc_p(RL6_HI,REG0,DEFAULT);
    VoC_add32_rr(REG45,RL6,ACC0,DEFAULT);  //   t0 = L_add(t0, L_Comp(Ah[j], Al[j]));
    VoC_shru16_ri(REG4,1,DEFAULT);    //  L_Extract (t0, &Anh[j], &Anl[j]);
    VoC_multf32inc_pp(RL6,REG0,REG1,DEFAULT);
    VoC_sw32inc_p(REG45,REG3,DEFAULT);
    VoC_endloop0

LEVINSON_101:

    // REG1 = CONST_0X00010001_ADDR, independent from branch

    VoC_multf32_rr(ACC0,REG7,REG7,DEFAULT);
    VoC_multf16_rr(REG0,REG7,REG6,IN_PARALLEL);

//      VoC_lw32_d(REG67,CONST_0x7FFFFFFF_ADDR);

    VoC_movreg32(REG45,ACC1,DEFAULT);
    VoC_shr32_ri(REG45,4,DEFAULT);            // t2 = L_shr (t2, 4);
    VoC_shru16_ri(REG4,1,DEFAULT);
    //  VoC_and16_rr(REG4,REG7,DEFAULT);                //   L_Extract (t2, &Anh[i], &Anl[i]);

    VoC_mac32_rp(REG0,REG1,DEFAULT);
    VoC_mac32_rp(REG0,REG1,DEFAULT);    //  t0 = Mpy_32 (Kh, Kl, Kh, Kl);           /* K*K             */
    VoC_sw32inc_p(REG45,REG3,DEFAULT);

    VoC_bnltz32_r(LEVINSON_L_ABS_4,ACC0)
    VoC_sub32_dr(ACC0,CONST_WORD32_0_ADDR,ACC0);//  t0 = L_abs (t0);                        /* Some case <0 !! */
LEVINSON_L_ABS_4:

    //  VoC_sub32_rr(REG23,REG67,ACC0,DEFAULT);    // t0 = L_sub ((Word32) 0x7fffffffL, t0);  /* 1 - K*K        */
    VoC_sub32_dr(REG23,CONST_0x7FFFFFFF_ADDR,ACC0);
    /* DPF format */
    VoC_shru16_ri(REG2,1,DEFAULT);
//      VoC_and16_rr(REG2,REG7,DEFAULT);         //    L_Extract (t0, &hi, &lo);               /* DPF format      */
    VoC_lw32_sd(REG45,0,IN_PARALLEL);


    VoC_multf16_rr(REG2,REG5,REG2,DEFAULT);
    VoC_multf16_rr(REG3,REG4,REG3,DEFAULT);
    exit_on_warnings = OFF;
    VoC_multf32_rr(ACC0,REG5,REG3,DEFAULT);
    exit_on_warnings = ON;
    VoC_bimac32inc_rp(REG23,REG1);      //   t0 = Mpy_32 (alp_h, alp_l, hi, lo);

    /**************************/
    // func  : CII_NORM_L_ACC0
    // input : ACC0
    // output: REG1
    // used  : ACC0, REG1
    /**************************/

    VoC_jal(CII_NORM_L_ACC0);
    VoC_movreg32(REG45,ACC0,DEFAULT);                   //t0

    /* DPF format    */
    VoC_shru16_ri(REG4,1,DEFAULT);
    //  VoC_and16_rr(REG4,REG7,DEFAULT);
    VoC_lw16_sd(REG3,1,IN_PARALLEL);   //alp_exp
    VoC_add16_rr(REG3,REG3,REG1,DEFAULT);
    VoC_sw32_sd(REG45,0,DEFAULT);
    VoC_sw16_sd(REG3,1,DEFAULT);         //  alp_exp = add (alp_exp, j);             /* Add normalization to

    VoC_lw16i_set_inc(REG2,LEVINSON_ANL_ADDR,2);
    VoC_lw16i_set_inc(REG3,LEVINSON_AL_ADDR+2,2);

    VoC_lw16_sd(REG6,0,DEFAULT);   //i
    VoC_add16_rd(REG5,REG6,CONST_1_ADDR);

    VoC_loop_r_short(REG6,DEFAULT)
    VoC_lw32inc_p(ACC0,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_sw16_sd(REG5,0,DEFAULT);   //i++
    VoC_endloop1

    VoC_lw16i(REG4,4096);

    VoC_lw16d_set_inc(REG0,PARA4_ADDR_ADDR,1);
    VoC_lw16i_set_inc(REG1,STRUCT_LEVINSONSTATE_OLD_A_ADDR+1,1);//&old_A[1]
    VoC_lw16i_set_inc(REG2,LEVINSON_AL_ADDR+2,1);   //&AL[1]

    VoC_sw16inc_p(REG4,REG0,DEFAULT);                   //A[0]=4096
    VoC_multf32inc_pd(REG45,REG2,CONST_1_ADDR);
    VoC_loop_i_short(10,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_add32_rd(REG67,REG45,CONST_0x00008000_ADDR);
    VoC_multf32inc_pd(REG45,REG2,CONST_1_ADDR);
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0

LEVINSON_EXIT:

    VoC_loop_i_short(2,DEFAULT)
    VoC_startloop0
    VoC_pop16(REG6,DEFAULT);
    VoC_pop32(ACC0,DEFAULT);
    VoC_endloop0

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}




/******************************************
  Function: CII_Get_lsp_pol
  Input:  &lsp[] -> REG1(incr=2)
      &f[]   -> REG2(incr=2)

   Optimized by Kenneth  09/13/2004
******************************************/

void CII_Get_lsp_pol(void)
{
    VoC_lw16i_short(REG5,2,DEFAULT);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i(ACC0_HI,0x100);
//  VoC_lw32_d(ACC0,CONST_0x1000000_ADDR);
    VoC_lw16i_short(REG0,2,DEFAULT);
    VoC_lw16i_short(REG7,4,IN_PARALLEL);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_loop_i(1,5);
    VoC_be16_rd(Get_lsp_pol_L1,REG0,CONST_2_ADDR);
    VoC_lw32_p(ACC0,REG4,DEFAULT);
    VoC_sub16_rr(REG6,REG0,REG5,IN_PARALLEL);
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_loop_r_short(REG6,DEFAULT);
    VoC_sw32_p(ACC0,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_sub16_rr(REG6,REG2,REG5,DEFAULT);
    VoC_sub16_rr(REG4,REG2,REG7,DEFAULT);   // here reg7=4
    VoC_lw32_p(ACC0,REG2,IN_PARALLEL);
    VoC_lw32_p(REG67,REG6,DEFAULT);
    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_multf32_rp(ACC1,REG7,REG1,IN_PARALLEL);
    VoC_multf16_rp(REG6,REG6,REG1,DEFAULT);
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_add32_rp(ACC0,ACC0,REG4,DEFAULT);
    VoC_mac32_rr(REG6,REG7,IN_PARALLEL);    // here reg7=1
    VoC_movreg16(REG6,REG2,DEFAULT);
    VoC_shr32_ri(ACC1,-1,DEFAULT);
    VoC_sub32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw16i_short(REG7,4,IN_PARALLEL);
    VoC_sub16_rr(REG2,REG2,REG5,DEFAULT);
    VoC_sw32_p(ACC0,REG6,DEFAULT);
    VoC_endloop0
    VoC_lw32_p(ACC0,REG2,DEFAULT);
Get_lsp_pol_L1:
    VoC_msu32inc_pd(REG1,CONST_512_ADDR);
    VoC_movreg16(INC2,REG0,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sub16_rr(REG4,REG2,REG7,DEFAULT);   // here reg7=4
    VoC_endloop1

    VoC_return;
}


/******************************************
  Function: CII_Lsp_Az
  Input:  &lsp[] -> REG1
     &Az[]  -> REG0

   Optimized by Kenneth  09/13/2004
******************************************/

void CII_Lsp_Az(void)
{





    VoC_push16(RA,DEFAULT);
    VoC_push32(REG01,DEFAULT);
    VoC_lw16i_set_inc(REG2,F1_ADDR,2);
    VoC_jal(CII_Get_lsp_pol);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_lw16i_set_inc(REG2,F2_ADDR,2);
    VoC_add16_rd(REG1,REG1,CONST_1_ADDR);
    VoC_jal(CII_Get_lsp_pol);

    VoC_lw16i_set_inc(REG0,F1_ADDR+6*2-2,-2);/*REG0 is a point to f1[i]*/
    VoC_lw16i_set_inc(REG2,F1_ADDR+5*2-2,-2);/*REG2 is a point to f1[i-1]*/
    VoC_lw16i_set_inc(REG1,F2_ADDR+6*2-2,-2);/*REG1 is a point to f2[i]*/
    VoC_lw16i_set_inc(REG3,F2_ADDR+5*2-2,-2);/*REG3 is a point to f2[i-1]*/
    VoC_loop_i_short(5,DEFAULT)
    VoC_lw16i_short(REG7,10,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32_p(ACC0,REG0,DEFAULT);//f1[i] = L_add (f1[i], f1[i - 1])
    VoC_add32inc_rp(ACC0,ACC0,REG2,DEFAULT);
    VoC_lw32_p(ACC1,REG1,DEFAULT); //CHANGE
    VoC_sub32inc_rp(ACC1,ACC1,REG3,DEFAULT);//f2[i] = L_sub (f2[i], f2[i - 1])
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC1,REG1,DEFAULT);
    VoC_endloop0
    VoC_lw16i(REG4,4096);

    VoC_pop32(REG23,DEFAULT);

    VoC_lw16i_short(INC3,-1,IN_PARALLEL);
    VoC_add16_rr(REG3,REG2,REG7,DEFAULT);  // reg7=10
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0,F1_ADDR+2,2);
    VoC_lw16i_set_inc(REG1,F2_ADDR+2,2);

    VoC_loop_i_short(5,DEFAULT)
    VoC_sw16inc_p(REG4,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(RL7,REG0,DEFAULT);
    VoC_lw32inc_p(RL6,REG1,DEFAULT);
    VoC_add32_rr(ACC0,RL7,RL6,DEFAULT);
    // VoC_lw16i_short(REG5,12,IN_PARALLEL);
    // VoC_jal(CII_L_shr_r);
    VoC_shr32_ri(ACC0,12,DEFAULT);
    VoC_add32_rd(ACC0,ACC0,CONST_1_ADDR);
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_sub32_rr(ACC0,RL7,RL6,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(ACC0_LO,REG2,DEFAULT);
    exit_on_warnings = ON;
    // VoC_lw16i_short(REG5,12,IN_PARALLEL);
    // VoC_jal(CII_L_shr_r);
    VoC_shr32_ri(ACC0,12,DEFAULT);
    VoC_add32_rd(ACC0,ACC0,CONST_1_ADDR);
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(ACC0_LO,REG3,DEFAULT);
    VoC_endloop0

    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_return;
}

/**
PARAMETERS:
R0: &lsf inc 1
R1: &lsp inc 1

*****/

void CII_Lsf_lsp(void)
{
    /*&lsf and &lsp*/
    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG2,TABLE_LSP_LSF_ADDR,1);       //&table[0]


    VoC_lw16inc_p(REG3,REG0,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_loop_i_short(M,DEFAULT)
    VoC_startloop0
    VoC_movreg16(REG7,REG3,DEFAULT);

    VoC_shr16_ri(REG3,8,DEFAULT);       //ind

    VoC_add16_rr(REG3,REG3,REG2,DEFAULT);   //&table[ind]
    VoC_and16_ri(REG7,0XFF);            //offset

    VoC_lw16inc_p(REG5,REG3,DEFAULT);       //&table[ind+1]
    VoC_sub16_pr(REG6,REG3,REG5,DEFAULT);
    VoC_multf32_rr(REG67,REG6,REG7,DEFAULT);
    VoC_NOP();
    VoC_shr32_ri(REG67,9,DEFAULT);
    VoC_add16_rr(REG6,REG5,REG6,DEFAULT);
    VoC_lw16inc_p(REG3,REG0,DEFAULT);
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_endloop0

    VoC_return
}


/**
PARAMETERS:
R0: &lsp inc -1;
R1: &lsf inc -1;
Optimised by cui 2005.01.19
***/

void CII_Lsp_lsf (void)
{
    VoC_lw16i_short(REG6,9,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG6,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,63,-1);       //for ind
    VoC_lw16i_set_inc(REG3,(TABLE_LSP_LSF_ADDR+63),-1);

    VoC_lw16i(REG4,TABLE_SLOPE_ADDR+63);   //&slope[0]
    VoC_loop_i_short(M,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_loop_i(1,64)
    VoC_sub16inc_rp(REG7,REG6,REG3,DEFAULT);
    VoC_bngtz16_r(LSP_LSF_02,REG7)
    VoC_inc_p(REG2,DEFAULT);
    VoC_sub16_rd(REG4, REG4,CONST_1_ADDR);
    VoC_endloop1
LSP_LSF_02:

    VoC_multf32_rp(REG67,REG7,REG4,DEFAULT);
    VoC_movreg16(REG5,REG2,DEFAULT); //ind
    VoC_shr32_ri(REG67,-3,DEFAULT);
    VoC_shr16_ri(REG5,-8,IN_PARALLEL);  //ind<<8
    VoC_add32_rd(REG67,REG67,CONST_0x00008000_ADDR);
    VoC_add16_rr(REG7,REG7,REG5,DEFAULT);
    VoC_add16_rd(REG3,REG3,CONST_1_ADDR);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0

    VoC_return
}

// REG0 FOR XN[], REG1 FOR H[] REG3 FOR Y[]
void CII_Convolve(void)
{


    VoC_movreg32(REG67,REG01,DEFAULT);
    VoC_lw16i_set_inc(REG2,0,1);                //REG2 for n/2
    VoC_sub16_rd(REG3,REG3,CONST_1_ADDR);


    /*RL6:constant 0X00010000*/
    VoC_lw16i_short(RL6_HI,1,DEFAULT);
    VoC_lw16i_short(RL6_LO,0,IN_PARALLEL);
    /*RL7:constant:0xffff0001*/
    VoC_lw16i_short(RL7_HI,-1,DEFAULT);
    VoC_lw16i_short(RL7_LO,1,IN_PARALLEL);

    VoC_multf32_pp(ACC0,REG0,REG1,DEFAULT);

//  exit_on_odd_address = OFF;
    VoC_loop_i(1,20);

    VoC_bngtz16_r(Convolve100,REG2)
    VoC_loop_r_short(REG2,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_pp(REG0, REG1);
    VoC_endloop0
Convolve100:
    VoC_add32_rr(REG01,REG67,RL6,DEFAULT);  //&h[n]++  //REG7++;REG6
    VoC_sw16inc_p(REG5,REG3,IN_PARALLEL);

    VoC_movreg32(REG45,ACC0,DEFAULT);
    VoC_add32_rr(REG67,REG67,RL6,IN_PARALLEL);  //&h[n]++  //REG7++;REG6

    VoC_aligninc_pp(REG0,REG1,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);                //n/2++

    VoC_shr32_ri(REG45,-3,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);                //s=0

    VoC_loop_r_short(REG2,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG1);
    VoC_endloop0

    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);  //&h[n]++
    VoC_sw16inc_p(REG5,REG3,IN_PARALLEL);   //save result of previous loop

    VoC_movreg32(REG45,ACC0,DEFAULT);
    VoC_add32_rr(REG01,REG67,RL7,IN_PARALLEL);  //&x[1] and &h[n+1]

    VoC_shr32_ri(REG45,-3,DEFAULT);
    VoC_aligninc_pp(REG0,REG1,DEFAULT);
    VoC_multf32_pp(ACC0,REG6,REG7,DEFAULT);


    VoC_endloop1
//  exit_on_odd_address = ON;
    VoC_sw16inc_p(REG5,REG3,DEFAULT);

    VoC_return;
}
/*
INPUT:   REG23
OUTPUT:   REG4:EXP;REG5:FRAC

USED:   loop_reg[0]
used:   R01,R23,R45;
*/

void CII_Log2(void)                        /////////////////////revesed by cui 2005.01.18
{

    VoC_lw32z(REG45,DEFAULT);
    VoC_push16(RA,DEFAULT);
    VoC_jal(CII_NORM_L_ACC0);
    VoC_push32(REG01,DEFAULT);
    VoC_movreg32(REG23, ACC0, DEFAULT);
    VoC_bgtz32_r(LOG1001,ACC0);    /* if (L_x <= (Word32) 0) goto */
    VoC_lw32z(REG45,DEFAULT);
    VoC_jump(LOG1002);
LOG1001:
    VoC_movreg32(ACC1,ACC0,DEFAULT); //acc1 :ener_code        // save val ACC0 in ACC1    ener_code
    VoC_sub16_dr(REG1,CONST_30_ADDR,REG1);//reg1:exponent
    VoC_shr32_ri(REG23,9,DEFAULT);

    VoC_sub16_rd(REG0,REG3,CONST_32_ADDR);      //reg0:i
    VoC_shr32_ri(REG23,1,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_and16_rd(REG2,CONST_0x7FFF_ADDR);    //reg2:a
    VoC_lw16i(REG4,TABLE_LOG2_ADDR);
    VoC_add16_rr(REG0,REG4,REG0,DEFAULT);           //reg0:&table[i]

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16inc_p(REG5,REG0,DEFAULT);               //REG45:L_y

    VoC_sub16_rp(REG3,REG5,REG0,DEFAULT);           //REG3:tmp
    VoC_multf32_rr(REG23,REG3,REG2,DEFAULT);        /* L_y = L_msu (L_y, tmp, a);  */
    VoC_NOP();
    VoC_sub32_rr(REG45,REG45,REG23,DEFAULT);
    VoC_movreg16(REG4,REG1,DEFAULT);        //RETURN:REG45
LOG1002:
    VoC_pop16(RA,DEFAULT);
    VoC_pop32(REG01,DEFAULT);
    VoC_return;
}


void CII_Lsf_wt(void)
{
    /*&lsf and &wf*/
    //REG0   *lsf     REG1    *wf
    VoC_movreg32(REG23,REG01,DEFAULT);

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_inc_p(REG0,DEFAULT);       // &lsf[1]
    VoC_lw16i_short(REG6, 0,IN_PARALLEL);
    VoC_loop_i_short(9,DEFAULT)
    VoC_startloop0
    VoC_sub16inc_pr(REG7, REG0, REG6,DEFAULT);
    VoC_lw16inc_p(REG6, REG2,DEFAULT);
    VoC_sw16inc_p(REG7,REG1, DEFAULT);
    VoC_endloop0
    VoC_sub16_dr(REG6,CONST_16384_ADDR,REG6); /*wf[9] = sub (16384, lsf[8])*/
    VoC_lw16i(REG4,1843);
    VoC_sw16_p(REG6,REG1, DEFAULT);

    VoC_lw16i(REG5,3427);
    VoC_lw16i(REG6,28160);
    VoC_lw16i(REG7,6242);
    VoC_loop_i_short(10,DEFAULT);
    VoC_startloop0

    VoC_multf16_rp(REG0,REG6,REG3,DEFAULT);
    VoC_sub16_pr(REG1,REG3,REG4,DEFAULT);   //REG1: temp
    VoC_sub16_rr(REG0,REG5,REG0,DEFAULT);//REG0 for wf[i]
    VoC_bltz16_r(Lsf_wt101,REG1);
    VoC_multf16_rr(REG0,REG1,REG7, DEFAULT);
    VoC_NOP();
    VoC_sub16_rr(REG0,REG4,REG0,DEFAULT);
Lsf_wt101:
    VoC_shr16_ri(REG0,-3,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG0,REG3, DEFAULT);
    VoC_endloop0
    VoC_return;
}




/*
PARAS:
R0:     &a  INC1
REG3:   &x  INC1
REG2:   &y  INC1

*/
void CII_Residu_new (void)
{
    VoC_lw16inc_p(REG6,REG0,DEFAULT);   // REG6 for a[0]
    VoC_movreg16(RL7_LO,REG0,DEFAULT);
    // REG0 for a[j]
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);
    VoC_loop_i(1,40)
    VoC_multf32inc_rp(ACC0,REG6,REG3,DEFAULT);
    VoC_sub16_rd(REG1,REG3,CONST_2_ADDR);   //REG1 for x[i-j]
    VoC_loop_i_short(10,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG0,DEFAULT);
    VoC_mac32inc_rp(REG7,REG1,DEFAULT);
    VoC_endloop0
    VoC_movreg16(REG0,RL7_LO,DEFAULT);
    VoC_shr32_ri(ACC0,-3,DEFAULT);
    VoC_add32_rd(REG45,ACC0,CONST_0x00008000_ADDR);
    VoC_NOP();
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    VoC_endloop1
    VoC_return;
}

// RL7 for  *mem_syn,*mem_w0,

void CII_subframePostProc(void)
{
#if 0
    voc_short  COD_AMR_SYNTH_ADDR     ,160,y

#endif


    VoC_push16(RA,DEFAULT);

    VoC_lw32_d(REG67,COD_AMR_GAIN_PIT_ADDRESS); //pitch_fac = gain_pit;
    VoC_lw16i(REG1,STRUCT_COD_AMRSTATE_SHARP_ADDR);
    VoC_bez16_r(subframePostProc103,REG0)
    VoC_lw16i(REG1,COD_AMR_SHARP_SAVE_ADDRESS);
    VoC_lw32_d(REG67,COD_AMR_GAIN_PIT_SF0_ADDRESS); //pitch_fac = gain_pit;
subframePostProc103:

    VoC_movreg16(REG3,REG6,DEFAULT);    // *sharp = gain_pit;
    VoC_push32(REG67,DEFAULT);/* REG6 for gain_pit REG7 for gain_code */
    VoC_lw16i_set_inc(REG2,13017,2);
    VoC_bngt16_rr(subframePostProc102,REG3,REG2)    // if (sub(*sharp, SHARPMAX) > 0)
    VoC_movreg16(REG3,REG2,DEFAULT);            // *sharp = SHARPMAX;
subframePostProc102:


    // tempShift = 1;
    VoC_lw16i_short(FORMAT32,-2,DEFAULT);
    VoC_lw16i_short(REG5,-2,DEFAULT);   // kShift = 2;

    VoC_sw16_p(REG3,REG1,DEFAULT);
    VoC_lw16i_short(REG1,7,IN_PARALLEL);

    VoC_bne16_rd(subframePostProc101,REG1,GLOBAL_ENC_USED_MODE_ADDR )       // if (sub((Word16)mode, MR122) == 0)

    // tempShift = 2;
    VoC_lw16i_short(FORMAT32,-3,DEFAULT);
    VoC_shr16_ri(REG6,1,DEFAULT);           // pitch_fac = shr (gain_pit, 1);
    VoC_lw16i_short(REG5,-4,IN_PARALLEL);  // kShift = 4;
subframePostProc101:

    VoC_push16(REG0,DEFAULT);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_EXC_ADDR,1);
    VoC_push16(REG5,DEFAULT);   //save kShift
    VoC_lw32_d(RL6,CONST_0x00008000_ADDR);

    VoC_lw16_sd(REG2,3,DEFAULT);        // REG2 for i_subfr
    VoC_movreg32(ACC0, RL6,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,COD_AMR_CODE_ADDRESS,1);                 /* REG1 for code[] address */

    VoC_lw16_sd(REG5,1,DEFAULT);   // REG5 for sf or not
    VoC_movreg32(ACC1, RL6,IN_PARALLEL);

    VoC_bez16_r(subframePostProc105,REG5)
    VoC_lw16_d(REG2,COD_AMR_I_SUBFR_SF0_ADDRESS);
    VoC_lw16i_set_inc(REG1,COD_AMR_CODE_SF0_ADDRESS,1);                 /* REG1 for code[] address */
subframePostProc105:

    VoC_add16_rr(REG0,REG0,REG2,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);

    VoC_bez16_d(subframePostProc01, GLOBAL_EFR_FLAG_ADDR)

    VoC_lw16i_short(FORMAT32,-4,DEFAULT);
    VoC_shr16_ri(REG6,1,DEFAULT);
subframePostProc01:

    VoC_loop_i_short(40,DEFAULT)
    VoC_startloop0
    VoC_mac32_rp(REG6,REG0,DEFAULT);                  /* L_temp = L_mult (exc[i + i_subfr], gain_pit); */
    VoC_multf32inc_rp(REG45,REG7,REG1,DEFAULT);
    VoC_NOP();
    VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);
    VoC_movreg32(ACC0,RL6,DEFAULT);
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
//   Syn_filt(Aq, &exc[i_subfr], &synth[i_subfr], L_SUBFR, mem_syn, 1);
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_EXC_ADDR,1);
    VoC_lw16_sd(REG2,4,DEFAULT);        // REG2 for i_subfr
    VoC_lw16_sd(REG5,6,DEFAULT);    // REG5 for Aq
    VoC_lw16_sd(REG0,2,DEFAULT);    // REG0 for sf or not

    VoC_bez16_r(subframePostProc106,REG0)
    VoC_sub16_rd(REG5,REG5,CONST_11_ADDR);
    VoC_lw16_d(REG2,COD_AMR_I_SUBFR_SF0_ADDRESS);
subframePostProc106:

    VoC_add16_rr(REG3,REG3,REG2,DEFAULT);
    VoC_lw16i(REG7,COD_AMR_SYNTH_ADDR);
    VoC_add16_rr(REG7,REG7,REG2,DEFAULT);
    VoC_lw16i_short(REG0,1,DEFAULT);            /* 1 address */
    VoC_lw16i_short(REG6,40,IN_PARALLEL);           /* L_SUBFR address */
    VoC_movreg16(REG1,RL7_HI,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    /*__________________________________________________________________
      Function:  CII_Syn_filt
     prameters:
       &a[0]  ->reg5
       &x[0]  -> reg3(inc 1 )
       &y     ->REG7
       lg     ->REG6
       &mem[0]->REG1 (incr=1) push32:
       update ->reg0

     Version 1.0  Create
     Version 1.1 optimized by Kenneth 09/01/2004
    ____________________________________________________________________*/
    VoC_jal(CII_Syn_filt);

    VoC_pop16(REG1,DEFAULT);        // REG1 for i_subfr
    VoC_lw16i_set_inc(REG2,COD_AMR_SYNTH_ADDR,1);
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_SPEECH_ADDR,1);

    VoC_bez16_d(subframePostProc121, GLOBAL_EFR_FLAG_ADDR)
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR,1);
subframePostProc121:


    VoC_bez16_d(subframePostProc301,GLOBAL_EFR_FLAG_ADDR)
    VoC_bnez16_d(subframePostProc301,GLOBAL_OUTPUT_SP_ADDR)
    VoC_jal(CII_SUBFRAME_EFR_PART);
    VoC_jump(subframePostProc302);
subframePostProc301:
    VoC_add16_rd(REG0,REG1,CONST_30_ADDR);
    VoC_add16_rr(REG2,REG2,REG0,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG0,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_MEM_ERROR_ADDR,1);

    VoC_loop_i_short(10,DEFAULT)          /* for (j = 0; j < M; j++) */
    VoC_lw16inc_p(REG4,REG3,IN_PARALLEL);       //mem_err[j] = sub(speech[i_subfr + i], synth[i_subfr + i]);
    VoC_startloop0
    VoC_sub16inc_rp(REG4,REG4,REG2,DEFAULT);
//      VoC_sub16inc_rp(REG5,REG5,REG2,IN_PARALLEL);

    VoC_lw16inc_p(REG4,REG3,DEFAULT);       //mem_err[j] = sub(speech[i_subfr + i], synth[i_subfr + i]);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_pop16(RL6_LO,DEFAULT);      // RL6_LO for kShift
    VoC_pop16(REG1,DEFAULT);
    VoC_lw16i_set_inc(REG0,COD_AMR_XN_ADDRESS+30,1);                    /* REG0 for xn[] address */
    VoC_lw16i_set_inc(REG2,COD_AMR_Y2_ADDRESS+30,1);                    /* REG2 for y2[] address */
    VoC_bez16_r(subframePostProc107,REG1)
    VoC_lw16i_set_inc(REG0,COD_AMR_XN_SF0_ADDRESS+30,1);                    /* REG0 for xn[] address */
    VoC_lw16i_set_inc(REG2,COD_AMR_Y2_SF0_ADDRESS+30,1);                    /* REG2 for y2[] address */
subframePostProc107:
    VoC_movreg16(REG3,RL7_LO,DEFAULT);
    VoC_lw16i_set_inc(REG1,COD_AMR_Y1_ADDRESS+30,1);                    /* REG1 for y1[] address */


    /* REG3 for mem_w0 address */
    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_loop_i_short(10,DEFAULT)          /* for (i = L_SUBFR - M, j = 0; i < L_SUBFR; i++, j++) */
    VoC_lw32_sd(REG67,0,IN_PARALLEL);
    VoC_startloop0

    VoC_multf32inc_rp(REG45,REG6,REG1,DEFAULT);
    VoC_multf32inc_rp(REG67,REG7,REG2,DEFAULT);         /* k = extract_h (L_shl (L_mult (y2[i], gain_code), 5)); */

    VoC_shr32_ri(REG45,-1,DEFAULT);         /* REG5 for temp temp = extract_h(L_shl(L_mult(y1[i], gain_pit), 1)); */
    VoC_movreg16(REG4, RL6_LO, DEFAULT);      // REG4 for kShift
    VoC_bez16_d(subframePostProc02,GLOBAL_EFR_FLAG_ADDR)
    VoC_sub16_rd(REG4,REG4,CONST_1_ADDR);
subframePostProc02:
    VoC_shr32_rr(REG67,REG4,DEFAULT);               /* REG7 for k */
    VoC_add16_rr(REG7,REG5,REG7,DEFAULT);

    VoC_sub16inc_pr(REG4,REG0,REG7,DEFAULT);    /*  mem_w0[j] = sub (xn[i], add (temp, k));  */
    VoC_lw32_sd(REG67,0,DEFAULT);
    VoC_sw16inc_p(REG4,REG3,DEFAULT);       /* save mem_w0 */
    VoC_endloop0

subframePostProc302:
    VoC_pop16(RA,DEFAULT);
    VoC_pop32(REG67,DEFAULT);
    /*    {
          temp = extract_h(L_shl(L_mult(y1[i], gain_pit), 1));
          k = extract_h(L_shl(L_mult(y2[i], gain_code), kShift));
          mem_w0[j] = sub(xn[i], add(temp, k));
       }
    */
    VoC_return;
}




/*
PARAS:
R0: &a      INC1
R1: &fac    INC1
R2: &a_exp  INC1
*/


void CII_Weight_Ai (void)
{


    VoC_lw16inc_p(REG5,REG0,DEFAULT);           //&a[1]

    VoC_loop_i_short(11,DEFAULT)
    VoC_startloop0


    VoC_multf32inc_pp(REG45,REG0,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_add32_rd(REG45,REG45,CONST_0x00008000_ADDR);

    VoC_endloop0

    VoC_return;

}


/***********************************************
  Function CII_Int_lpc_only
  input: REG2->lsp_mid
         REG3->lsp_old
         REG1(incr=2)->&lsp[] push(n)
  Optimized by Kenneth  18/09/2004
***********************************************/
void CII_Int_lpc_only(void)
{

    VoC_lw16i_short(INC3,2,DEFAULT);
    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_lw32inc_p(REG67,REG3,DEFAULT);      //lsp_mid
    VoC_lw32inc_p(REG45,REG2,DEFAULT);      //lsp_old
    VoC_loop_i_short((M/2),DEFAULT)
    VoC_startloop0
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_shr16_ri(REG5,1,DEFAULT);
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_shr16_ri(REG7,1,IN_PARALLEL);
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG7,IN_PARALLEL);
    VoC_lw32inc_p(REG67,REG3,DEFAULT);          //lsp_mid
    VoC_sw32inc_p(REG45,REG1,DEFAULT);
    VoC_lw32inc_p(REG45,REG2,DEFAULT);          //lsp_old
    VoC_endloop0
    VoC_return;

}




// REG0:  fraction
// REG1:  exponent
// REG23: return
//used:REG0,1,2,3,4,5
void CII_Pow2 (void)
{

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_multf32_rd(REG23,REG0,CONST_32_ADDR);
    VoC_lw16i_set_inc(REG0,TABLE_POW2_ADDR,1);
    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);   //REG0:addr of table[i]
    VoC_shru16_ri(REG2,1,DEFAULT);
//  VoC_shr32_ri(REG23,1,IN_PARALLEL);
//  VoC_and16_ri(REG2,0x7fff);          //reg2:a

    VoC_lw16inc_p(REG5,REG0,DEFAULT);       //REG45:L_x

    VoC_sub16_rp(REG3,REG5,REG0,DEFAULT);   //REG3:tmp

    VoC_multf32_rr(REG23,REG3,REG2,DEFAULT);
    VoC_sub16_dr(REG0,CONST_30_ADDR,REG1); //exp = sub (30, exponent);
    VoC_sub32_rr(REG23,REG45,REG23,DEFAULT);


//  VoC_lw16i_short(REG4,31,DEFAULT);//L_shr_r(r)
    VoC_bngt16_rd(POW2101,REG0,CONST_31_ADDR);
    VoC_lw32z(REG23,DEFAULT);
    VoC_return

POW2101:

    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);
    VoC_shr32_rr(REG23,REG0,DEFAULT); //L_shr_r = (L_shr(r-1) +1) >> 1
    VoC_bngt16_rd(POW2102,REG0,CONST_1_ADDR);
    VoC_add32_rd(REG23,REG23,CONST_0x00000001_ADDR);

POW2102:
    VoC_shr32_ri(REG23,1,DEFAULT);//L_shr_r(r) = (L_shr(r-1) +1) >> 1

    VoC_return;
}

//INPUT:
// acc0  L_x
//OUTPUT:
//  REG1    exp
//RETURN:   ACC0
//REGISTER USED:    ACC0,REG0,REG1,REG67

void CII_sqrt_l_exp (void)
{
    VoC_lw16i_short(REG1,0, DEFAULT);
    VoC_bgtz32_r(sqrt_l_exp_go, ACC0)
    VoC_lw32z(ACC0,DEFAULT);
    VoC_return;
sqrt_l_exp_go:
    VoC_push16(RA,DEFAULT);
    VoC_movreg32(REG67, ACC0, IN_PARALLEL);
    VoC_jal(CII_NORM_L_ACC0);
    //     REG1     e
    VoC_and16_ri(REG1,0xFFFE);      // e = norm_l (L_x) & 0xFFFE;  ;
    VoC_sub16_dr(REG0,CONST_0_ADDR,REG1);
    VoC_shr32_rr(REG67,REG0,DEFAULT);
    // REG1    exp
    VoC_shr32_ri(REG67,9, DEFAULT);
    VoC_lw16i_set_inc(REG0, TABLE_SQRT_L_ADDR, 1);
    VoC_add16_rr(REG0, REG0, REG7, DEFAULT);
    VoC_sub16_rd(REG0, REG0, CONST_16_ADDR);
    //REG0      table[i]
//  VoC_shr32_ri(REG67,1, DEFAULT);
//  VoC_and16_ri(REG6, 0x7fff);

    VoC_shru16_ri(REG6,1, DEFAULT);

    //REG6    a
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16inc_p(REG7, REG0, IN_PARALLEL);
    VoC_movreg16(ACC0_HI, REG7,DEFAULT);
    VoC_sub16_rp(REG7,REG7, REG0, IN_PARALLEL);
    VoC_pop16(RA,DEFAULT);
    VoC_msu32_rr(REG6, REG7, DEFAULT);
    VoC_return;
}


/************************************************************************* * *
FUNCTION:  CII_gc_pred_update() * Input:  *st->          REG5
           Word16 *ener_avg_MR122,   REG7                   Word16 *ener_avg          REG6

           Notise:  REG0,REG1,REG2,REG3 increment stage changed to -1
           Created by  Kenneth   07/14/2004
           *************************************************************************/
void CII_gc_pred_update(void)
{
    VoC_lw16i_short(REG3,2,DEFAULT);
    VoC_add16_rr(REG0,REG5,REG3,DEFAULT);
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_add16_rd(REG2,REG5,CONST_6_ADDR);
    VoC_add16_rr(REG1,REG0,REG3,DEFAULT);
    VoC_add16_rr(REG3,REG2,REG3,DEFAULT);
    //addr of past_qua_en[2]
    VoC_lw16i_short(INC0,-1,DEFAULT);
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);//addr of past_qua_en[3]


    VoC_lw16i_short(INC2,-1,DEFAULT);   //addr of qua_ener_MR122[2]
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);//addr of qua_ener_MR122[3]
    VoC_loop_i_short(3,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_endloop0;
    VoC_sw16_p(REG7,REG3,DEFAULT);
    VoC_sw16_p(REG6,REG1,DEFAULT);
    VoC_return;
}




/*
parameters:
R0: &ind  INC 1
R5: n

return: R1
*/
void CII_gmed_n(void) //new  //return REG1
{

    VoC_lw16i_set_inc(REG1,MR475_gain_quant_coeff_ADDRESS,1);//reg1 addr of tmp2[i]
    VoC_movreg16(REG4,REG1,DEFAULT);//reg4 addr of tmp2[i]

    VoC_loop_r_short(REG5,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG2,REG0,DEFAULT);
    VoC_lw16i_set_inc(REG3,MR475_gain_quant_coeff_lo_ADDRESS,1);//reg3 addr of tmp[i]
    VoC_sw16inc_p(REG2,REG1,DEFAULT);
    VoC_endloop0;


    VoC_loop_r(1,REG5);
    VoC_lw16i(REG2,-32768); //max = -32767;

    VoC_movreg16(REG1,REG4,DEFAULT);//addr tmp2[j]

    VoC_loop_r_short(REG5,DEFAULT);
    VoC_startloop0

    VoC_lw16_p(REG7,REG1,DEFAULT);
    VoC_bgt16_rr(GMED001,REG2,REG7); //if max > tmp2[j]  goto GMED001
    VoC_movreg16(REG2,REG7,DEFAULT);//max = tmp2[j];
    VoC_movreg16(REG6,REG1,IN_PARALLEL); //ix = j;
GMED001:
    VoC_inc_p(REG1,DEFAULT);//j++
    VoC_endloop0;

    VoC_sub16_rr(REG2,REG6,REG4,DEFAULT);//&tmp2[ix]

    VoC_lw16i(REG7,-16384);
    VoC_sw16inc_p(REG2,REG3,DEFAULT);   //tmp[i] = ix;

    VoC_sw16_p(REG7,REG6,DEFAULT); //tmp2[ix] = -16384;

    VoC_endloop1;

    VoC_lw16i(REG3,(MR475_gain_quant_coeff_lo_ADDRESS)*2);//reg3 addr of tmp[0]
    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);//tmp[2]
    VoC_shru16_ri(REG3,1,DEFAULT);
    VoC_sub16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_add16_rp(REG0,REG0,REG3,DEFAULT);// reg4 addr of ind[tmp[2]]
    VoC_NOP();
    VoC_lw16_p(REG1,REG0,DEFAULT);//reg1 ind[tmp[2]]return

    VoC_return;
}


void CII_Autocorr(void)
{

#if 0
    voc_short    AUTOCORR_Y_ADDR                       ,240,x                          //~241
    voc_short    Blamk_AUTOCORR_Y_ADDR                ,4,y
    voc_short    AUTOCORR_Y_COPY_ADDR                  ,240,y
#endif


#if 0
    voc_short    LPC_RLOW_ADDR                      ,12,x
    voc_short    LPC_RHIGH_ADDR                     ,18,x
    voc_short    LPC_RC_ADDR                        ,4,x

#endif


    /* Windowing of signal */
    VoC_push16(RA,DEFAULT);

    VoC_lw16i_set_inc(REG2,AUTOCORR_Y_ADDR-2,1);                    //&y
    VoC_lw32_d(RL6,CONST_0x00008000_ADDR);

    VoC_loop_i(0,(L_WINDOW/2)+1)
    VoC_add32_rr(REG45,ACC0,RL6,DEFAULT);
    VoC_multf32inc_pp(ACC0,REG0,REG1,IN_PARALLEL);
    VoC_add32_rr(REG67,ACC1,RL6,DEFAULT);
    VoC_multf32inc_pp(ACC1,REG0,REG1,IN_PARALLEL);
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,DEFAULT);
    VoC_endloop0


    /* Compute r[0] and test for overflow */
    VoC_lw16i_short(RL6_LO,0,DEFAULT);      //RL6_LO for overfl_shft
    VoC_lw16i_short(RL7_LO,4,IN_PARALLEL);


AUTOCORR_01:

    VoC_lw32z(ACC0,DEFAULT);                            //s=0
    VoC_lw16i_set_inc(REG1,AUTOCORR_Y_ADDR,2);                          //&y

    VoC_loop_i(0,L_WINDOW/2)
    VoC_bimac32inc_pp(REG1, REG1);
    VoC_endloop0

    VoC_NOP();
    /* If overflow divide y[] by 4 */
    VoC_bne32_rd(AUTOCORR_01_END,ACC0,CONST_0x7FFFFFFF_ADDR);   //? sum==MAX_32

    VoC_add32_rr(RL6,RL6,RL7,DEFAULT);

    VoC_lw16i_set_inc(REG0,AUTOCORR_Y_ADDR,2);                          //&y
    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_lw16i_short(REG2,2,DEFAULT);
    VoC_lw16i_short(REG3,60,IN_PARALLEL);
    VoC_jal(CII_scale);

    VoC_jump(AUTOCORR_01)


AUTOCORR_01_END:


    VoC_add32_rd (ACC0, ACC0,CONST_0x00000001_ADDR);    //s+=lL

    /* Normalization of r[0] */
    VoC_jal(CII_NORM_L_ACC0);


    //REG67 for sum,NORMLIZATION IS DONE IN NORM_L
    VoC_movreg16(REG5,RL6_LO,DEFAULT);          //over_shft
    VoC_movreg32(REG67,ACC0,IN_PARALLEL);

    VoC_sub16_rr(REG5,REG1,REG5,DEFAULT);       //norm-over_shft

    VoC_shru16_ri(REG6,1,IN_PARALLEL);
//  VoC_and16_ri(REG6,0X7FFF);
    VoC_sw16_d(REG7,LPC_RHIGH_ADDR);
    VoC_sw16_d(REG6,LPC_RLOW_ADDR);
    VoC_sw16_d(REG5,SCAL_ACF_ADDR_P);

    VoC_sub16_dr(REG4,CONST_0_ADDR,REG1);       //REG4:norm=-norm

    /*copy y(in RAM_X) to y_copy(in RAM_Y)*/
    VoC_lw16i_set_inc(REG0,AUTOCORR_Y_ADDR,2);                          //&y
    VoC_lw16i_set_inc(REG1,AUTOCORR_Y_COPY_ADDR,2);                 //&y_copy
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw16i_short(REG6,60,IN_PARALLEL);
    VoC_jal(CII_copy_xy);
    VoC_sw32_p(RL6,REG1,DEFAULT);


    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    /* r[1] to r[m] */
    VoC_lw16i_set_inc(REG0,LPC_RLOW_ADDR+1,1);      //&r_l[]
    VoC_lw16i_set_inc(REG1,LPC_RHIGH_ADDR+1,1);     //&r_h[]

    // here REG4 for norm
    VoC_lw16i(RL6_HI, AUTOCORR_Y_COPY_ADDR+1);                  //&y_copy[1]
    VoC_lw16i(RL6_LO,240);//L_WINDOW

    VoC_lw32_d(RL7,CONST_0x0000FFFF_ADDR);      //0X0000FFFF

    VoC_pop16(RA,DEFAULT);


    VoC_lw32z(ACC0,DEFAULT);
    VoC_movreg16(REG3,RL6_HI,IN_PARALLEL);          //REG3 addr of y_copy[j+i]
    VoC_lw16i(REG2,AUTOCORR_Y_ADDR);                        //REG2 addr of y[j]
    VoC_movreg16(REG5,RL6_LO,DEFAULT);
    VoC_aligninc_pp(REG2,REG3,DEFAULT);
//  exit_on_odd_address = OFF;

    VoC_shr16_ri(REG5,1,IN_PARALLEL);


    VoC_loop_i(1,10);

    VoC_loop_r_short(REG5,DEFAULT);
    VoC_startloop0
    VoC_bimac32inc_pp(REG2,REG3);
    VoC_endloop0;

    VoC_add32_rr(RL6,RL6,RL7,DEFAULT);
    VoC_lw16i(REG2,AUTOCORR_Y_ADDR);
    //REG2 addr of y[j]
    VoC_shr32_rr(ACC0,REG4,DEFAULT);
    VoC_movreg16(REG3,RL6_HI,IN_PARALLEL);//REG3 addr of y_copy[j+i]
    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_aligninc_pp(REG2,REG3,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

//      VoC_and16_ri(REG6,0X7FFF);
    VoC_sw16inc_p(REG7, REG1, DEFAULT);
    VoC_movreg16(REG5,RL6_LO,IN_PARALLEL);

    VoC_sw16inc_p(REG6, REG0, DEFAULT);
    VoC_shr16_ri(REG5,1,IN_PARALLEL);

    VoC_endloop1;
//  exit_on_odd_address = ON;

    VoC_return;
}


/*******************************
 Function: CII_Chebps
 Input:  REG2->&f[0]
         REG3->x
 Output: REG7->cheb
         REG0<-0x7fff
         REG1<-0x2000
 Used  : all except RL67
 unchanged : REG3
 Optimized by Yordan 08/12/2005
********************************/


void CII_Chebps (void)
{

#if 0
    voc_short  CHEBPS_B2_ADDR                      ,2,y
#endif

    VoC_lw32z(ACC1,DEFAULT);
//  VoC_lw16i(REG0,0x7fff);
    VoC_lw16_d(REG1,CONST_8192_ADDR_Y);
    VoC_lw16i(REG6,CHEBPS_B2_ADDR);
    VoC_lw16i(REG7,CONST_0x8000FFFF_ADDR);
//  VoC_lw32_d(ACC1,CONST_0x1000000_ADDR);


    VoC_lw16i(ACC1_HI,0x100);
    // b1 = 2*x + f[1]
    VoC_multf32_rd(REG45,REG3,CONST_512_ADDR);
    VoC_multf32inc_rp(ACC0,REG1,REG2,DEFAULT);
    VoC_sw32_p(ACC1,REG6,DEFAULT);
    VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);


    VoC_shru16_ri(REG4,1,DEFAULT);
//  VoC_and16_rr(REG4,REG0,DEFAULT);

    VoC_loop_i_short(3,DEFAULT)
    VoC_startloop0
    VoC_multf32_rr(ACC1,REG4,REG3,DEFAULT); //b1_l*x
    // t0 = 2.0*x*b1
    VoC_multf32inc_rp(ACC0,REG1,REG2,DEFAULT);  //+f[i];

    VoC_shr32_ri(ACC1,16,DEFAULT);
    VoC_shr32_ri(ACC1,-1,DEFAULT);
    exit_on_warnings = OFF;
    VoC_mac32_rr(REG5,REG3,IN_PARALLEL);    //b1_h*x+b1_l*1<<1
    exit_on_warnings = ON;

    VoC_bimac32inc_pp(REG6,REG7);           // -b2
    VoC_sw32_p(REG45,REG6,DEFAULT);         //B2=B1;REG45: free
    VoC_shr32_ri(ACC1,-1,IN_PARALLEL);      //b1*x*2
    exit_on_warnings = OFF;
    VoC_add32_rr(REG45,ACC1,ACC0,DEFAULT);      //-b2+f[i]+2*b1*x
    exit_on_warnings = ON;
    VoC_shru16_ri(REG4,1,DEFAULT);
//      VoC_and16_rr(REG4,REG0,DEFAULT);

    VoC_endloop0

    // t0 = x*b1;
    VoC_multf32_rr(ACC0,REG4,REG3,DEFAULT); //b1_l*x
    VoC_NOP();
    VoC_shr32_ri(ACC0,15,DEFAULT);          //b1_h*x+b1_l*x<<1
    VoC_bimac32inc_pp(REG6,REG7);           //-b2

    VoC_mac32_pd(REG2,CONST_4096_ADDR);     //+f[i]/2
    VoC_multf32_rr(REG67,REG5,REG3,DEFAULT);//b1_h*x
    VoC_NOP();
    VoC_add32_rr(REG67,REG67,ACC0,DEFAULT);

    VoC_shr32_ri(REG67,-6,DEFAULT);
    VoC_return;
}


/*******************************************************
 Function: CII_Az_lsp
 Input:  REG6->x         ---az[MP1+x]
         REG1(incr=-1)->lsp[]
         REG0(incr=1)->old_lsp[]

 Optimized by Kenneth 09/06/2004
********************************************************/


void CII_Az_lsp (void)
{
#if 0
    voc_short    AZ_LSP_IP_ADDR                             ,x       //1 short
    voc_short    AZ_LSP_COEF_ADDR                           ,x       //1 short
    voc_short    AZ_LSP_GRID_J_ADDR_ADDR                    ,x       //1 short
    voc_short    AZ_LSP_LOOP_CTRL_ADDR                      ,x       //1 short
    voc_short    AZ_LSP_XLOW_ADDR                           ,x       //1 short
    voc_short    AZ_LSP_YLOW_ADDR                           ,x       //1 short
    voc_short    AZ_LSP_XHIGH_ADDR                          ,x       //1 short
    voc_short    AZ_LSP_YHIGH_ADDR                          ,x       //1 short
    voc_short    AZ_LSP_XMID_ADDR                           ,x       //1 short
    voc_short    AZ_LSP_YMID_ADDR                           ,x       //1 short



    voc_short   AZ_LSP_F1_ADDR                              ,10,x
    voc_short   AZ_LSP_F2_ADDR                              ,10,x

#endif


    VoC_push32(REG01,DEFAULT);      // push32(n)  &lsp[]/&old_lsp[]
    VoC_lw16i_set_inc(REG0,COD_AMR_A_T_ADDRESS+12,1);
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);

    VoC_lw16i_short(REG5,9,IN_PARALLEL);
    VoC_add16_rr(REG1,REG0,REG5,DEFAULT);

    VoC_push16(RA,IN_PARALLEL);         // push16(n)

    VoC_lw16i(REG6,1024);
    VoC_lw16i_set_inc(REG2,AZ_LSP_F1_ADDR,1);                   //&f1
    VoC_lw16i_set_inc(REG3,AZ_LSP_F2_ADDR,1);                   //&f2

    VoC_sw16_p(REG6,REG2,DEFAULT);
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);
    VoC_sw16_p(REG6,REG3,DEFAULT);
    VoC_lw16i(REG4,8192);

    VoC_loop_i_short(5,DEFAULT)
    VoC_startloop0
    VoC_multf32inc_rp(ACC0,REG4,REG0,DEFAULT);
    VoC_multf32inc_rp(REG67,REG4,REG1,DEFAULT);
    VoC_NOP();
//exit_on_warnings=OFF;
    VoC_sub32_rr(REG67,ACC0,REG67,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,REG67,IN_PARALLEL);
//exit_on_warnings=ON;
    VoC_movreg16(REG5,ACC0_HI,DEFAULT);
    VoC_add16inc_rp(REG7,REG7,REG3,IN_PARALLEL);
    VoC_sub16inc_rp(REG5,REG5,REG2,DEFAULT);
    VoC_sw16_p(REG7,REG3,DEFAULT);
    VoC_sw16_p(REG5,REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i(REG2, (1+AZ_LSP_F1_ADDR));
    VoC_sw16_d(REG4,AZ_LSP_IP_ADDR);

    VoC_lw16i(REG6,TABLE_GRID_ADDR);
    VoC_lw16i(REG7,(TABLE_GRID_ADDR+61));

    VoC_lw16_p(REG3,REG6,DEFAULT);

    VoC_push16(REG4,DEFAULT);       // push16(n-1)  ylow
    VoC_push32(REG67,IN_PARALLEL);  // push32(n-1)  &grid[61](REG7)/&grid[i](REG6)

    VoC_push16(REG4,DEFAULT);       // push16(n-2)  nf
    VoC_push32(REG23,IN_PARALLEL);  // push32(n-2)  xlow(REG3)/coef(REG2)

    /*****************************
     Function: CII_Chebps
     Input:  REG2->&f[0]
             REG3->x
     Output: REG7->cheb
             REG0<-0x7fff
             REG1<-0x2000
     Used  : all except RL67
     unchanged : REG3
    ******************************/

    VoC_jal(CII_Chebps);
    VoC_sw16_sd(REG7,1,DEFAULT);    // restore  ylow

AZ_LSP_LOOP:

    VoC_lw32_sd(REG23,0,DEFAULT);           // xlow(REG3)/coef(REG2)
    VoC_lw16i_short(REG0,10,IN_PARALLEL);

    VoC_lw32_sd(REG67,1,DEFAULT);           // &grid[61](REG7)/&grid[i](REG6)
    VoC_lw16_sd(REG4,0,IN_PARALLEL);        // nf

    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);   //j++

    VoC_bngt16_rr(AZ_LSP_LOOP_END,REG7,REG6)
    VoC_bngt16_rr(AZ_LSP_LOOP_END,REG0,REG4)

    VoC_lw16_sd(REG5,1,DEFAULT);
    VoC_movreg16(REG4,REG3,IN_PARALLEL);

    VoC_lw16_p(REG3,REG6,DEFAULT);
    VoC_sw32_sd(REG67,1,DEFAULT);
    VoC_sw32_sd(REG23,0,DEFAULT);           // store xlow
    VoC_push32(REG45,DEFAULT);          // push32(n-3) yhigh(high)/xhigh(low)

    /*****************************
     Function: CII_Chebps
     Input:  REG2->&f[0]
             REG3->x
     Output: REG7->cheb
             REG0<-0x7fff
             REG1<-0x2000
     Used  : all except RL67
     unchanged : REG3
    ******************************/

    VoC_jal(CII_Chebps);

    VoC_sw16_sd(REG7,1,DEFAULT);            // restore  ylow
    VoC_lw32_sd(REG45,0,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG7,REG5,DEFAULT);     //  L_mult (ylow, yhigh)
    VoC_NOP();
    VoC_bgtz32_r(AZ_LSP_NOLOOP1,ACC0)

    VoC_loop_i(1,4)

    // xmid = (xlow + xhigh)/2
    VoC_lw32_sd(REG23,1,DEFAULT);   // load xlow/coef
    VoC_shr16_ri(REG3,1,DEFAULT);
    VoC_shr16_ri(REG4,1,IN_PARALLEL);
    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);

    /*****************************
     Function: CII_Chebps
     Input:  REG2->&f[0]
             REG3->x
     Output: REG7->cheb
             REG0<-0x7fff
             REG1<-0x2000
     Used  : all except RL67
     unchanged : REG3
    ******************************/

    VoC_jal(CII_Chebps);

    VoC_lw16_sd(REG4,1,DEFAULT);    // load ylow
    VoC_multf32_rr(ACC0,REG7,REG4,DEFAULT);
    VoC_movreg16(REG6,REG3,DEFAULT);

    VoC_bgtz32_r(AZ_LSP_03_01_ELSE,ACC0)
    VoC_sw32_sd(REG67,0,DEFAULT);   // restore  yhigh/xhigh
    VoC_jump(AZ_LSP_03_01_END)
AZ_LSP_03_01_ELSE:
    VoC_lw32_sd(REG23,1,DEFAULT);
    VoC_movreg16(REG3,REG6,DEFAULT);
    VoC_sw16_sd(REG7,1,DEFAULT);    // restore ylow
    VoC_sw32_sd(REG23,1,DEFAULT);   // restore xlow/coef
AZ_LSP_03_01_END:
    VoC_lw32_sd(REG45,0,DEFAULT);   // load yhigh/xhigh

    VoC_endloop1

    // xint = xlow - ylow*(xhigh-xlow)/(yhigh-ylow);
    VoC_lw32_sd(REG23,1,DEFAULT);           // load  xlow/coef
    VoC_lw16_sd(REG7,1,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG4,REG3,DEFAULT);       //x
    VoC_sub16_rr(REG7,REG5,REG7,IN_PARALLEL);   //y;width 2

    VoC_bez16_r(AZ_LSP_03_02_END,REG7)             //exit with REG3:xlow;

    VoC_push32(REG67,DEFAULT);          // push32(n-4)     sign(reg7)/x(reg6);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);

    VoC_bnltz16_r(AZ_LSP_ABS_S_1,REG7)
    VoC_sub16_rr(REG7,REG6,REG7,DEFAULT);
AZ_LSP_ABS_S_1:

    VoC_movreg32(ACC0,REG67,DEFAULT);

    /**************************/
    // input in ACC0
    // output in REG1
    // used ACC0, REG1
    /**************************/

    VoC_jal(CII_NORM_L_ACC0);

    VoC_sub16_dr(REG1,CONST_20_ADDR,REG1);
    VoC_lw16i(REG0,0x3fff);

    VoC_push16(REG1,DEFAULT);           // push16(n-3)  20-exp
    VoC_movreg16(REG1,ACC0_HI,IN_PARALLEL);

    /****************************/
    // input in REG0, REG1
    // output in  REG2
    // used register RL67, REG012
    /****************************/

    VoC_jal(CII_DIV_S);

    VoC_pop32(REG67,DEFAULT);           //   pop32(n-4)     sign(reg7)/x(reg6);

    VoC_multf32_rr(ACC0,REG6,REG2,DEFAULT);     //   t0 = L_mult (x, y);
    VoC_pop16(REG5,DEFAULT);            // pop16(n-3)  20-exp
    VoC_shr32_rr(ACC0,REG5,DEFAULT);                //  t0 = L_shr (t0, sub (20, exp));
    VoC_movreg16(REG6,ACC0_LO,DEFAULT);     //y

    VoC_bnltz16_r(AZ_LSP_04_01_ELSE,REG7);
    VoC_sub16_dr(REG6,CONST_0_ADDR,REG6);
AZ_LSP_04_01_ELSE:

    VoC_lw16_sd(REG7,1,DEFAULT);            // load ylow
    VoC_multf32_rr(REG67,REG6,REG7,DEFAULT);
    VoC_lw32_sd(REG23,1,DEFAULT);           // load  xlow/coef
    VoC_shr32_ri(REG67,11,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG6,DEFAULT);       //exit with REG3:xlow;

AZ_LSP_03_02_END:

    VoC_lw32_sd(REG67,3,DEFAULT);           // load &lsp[]/&lsp_old[]
    VoC_lw16_sd(REG0,0,DEFAULT);            //  load nf

    VoC_add16_rr(REG7,REG0,REG7,DEFAULT);
    VoC_inc_p(REG0,DEFAULT);            // nf++
    VoC_sw16_p(REG3,REG7,DEFAULT);
    VoC_sw16_sd(REG0,0,DEFAULT);

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i(REG2, (1+AZ_LSP_F2_ADDR));

    VoC_bez16_d(AZ_LSP_03_03_ELSE,AZ_LSP_IP_ADDR)
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i(REG2, (1+AZ_LSP_F1_ADDR));
AZ_LSP_03_03_ELSE:

    VoC_sw16_d(REG6,AZ_LSP_IP_ADDR);
    VoC_sw32_sd(REG23,1,DEFAULT);

    /*****************************
     Function: CII_Chebps
     Input:  REG2->&f[0]
             REG3->x
     Output: REG7->cheb
             REG0<-0x7fff
             REG1<-0x2000
     Used  : all except RL67
     unchanged : REG3
    ******************************/

    VoC_jal(CII_Chebps);
    VoC_sw16_sd(REG7,1,DEFAULT);        // restore ylow

AZ_LSP_NOLOOP1:

    VoC_pop32(REG45,DEFAULT);       // pop32(n-3) yhigh(high)/xhigh(low)

    VoC_jump(AZ_LSP_LOOP)

AZ_LSP_LOOP_END:

    VoC_pop16(REG4,DEFAULT);        // pop16(n-2)  nf
    VoC_pop32(REG23,DEFAULT);       // pop32(n-2)  xlow(REG3)/coef(REG2)
    VoC_pop16(REG5,DEFAULT);        // pop16(n-1)  ylow
    VoC_pop32(REG67,IN_PARALLEL);       // pop32(n-1)  &grid[61](REG7)/&grid[i](REG6)

    VoC_lw32_sd(REG01,0,DEFAULT);

    VoC_bnlt16_rd(AZ_LSP_01_02,REG4,CONST_10_ADDR)

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_jal(CII_Copy_M);
AZ_LSP_01_02:

    VoC_pop16(RA,DEFAULT);
    VoC_pop32(REG01,DEFAULT);
    VoC_return;
}




void CII_cbseach_subr1(void)
{
    VoC_shr16_ri(REG3,-1,DEFAULT);

    VoC_lw16_d(REG4,COD_AMR_T0_ADDRESS);
    VoC_sub16_dr(REG5,CONST_40_ADDR,REG4);

    VoC_add16_rr(REG1,REG0,REG4,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_bngtz16_r(cbsearch_10000,REG5)
    VoC_multf16inc_rp(REG4,REG3,REG0,DEFAULT);
    VoC_loop_r_short(REG5,DEFAULT);
    VoC_startloop0
    VoC_add16_rp(REG4,REG4,REG1,DEFAULT);
    VoC_multf16inc_rp(REG4,REG3,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
cbsearch_10000:

    VoC_return;
}



/*************************************************************************
 *
 *  FUNCTION:  cor_h_x()
 *
 *  PURPOSE:  Computes correlation between target signal "x[]" and
 *            impulse response"h[]".
 *
 *  DESCRIPTION:
 *    The correlation is given by:
 *       d[n] = sum_{i=n}^{L-1} x[i] h[i-n]      n=0,...,L-1
 *
 *    d[n] is normalized such that the sum of 5 maxima of d[n] corresponding
 *    to each position track does not saturate.
 *
 *************************************************************************/


//ACC1_lo  : sf
//REG4  : nb_track & step
//REG5  : 40/step
//incr0 and  incr1 =1

void CII_cor_h_x(void)
{
    /*********  SPEED OPTIMIZATION *********************
    // total gain through using bimac is about 0.1 Mcps
    ****************************************************/

#if 0
    voc_short    COR_H_X_Y32_ADDR             ,40,x
#endif

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG0,40,IN_PARALLEL);

    // stack16: RA, xn2, st->h1, st->sharp, gain_pit, res2, code, y2, subfrNr, anap,  RA, RA

    VoC_lw16_sd(ACC1_HI,9,DEFAULT) ;    //    st->h1
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw16_sd(REG6,10,DEFAULT);       // xn2
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_lw32z(RL7,DEFAULT);
    VoC_lw32z(REG23,IN_PARALLEL); // REG3 : k


    /*********  SPEED OPTIMIZATION *********************
    // ptr to first line after buffer
    VoC_add16_rd(REG7,REG6,CONST_40_ADDR);
    VoC_set_incr_i(REG0,2,DEFAULT);
    VoC_set_incr_i(REG1,2,IN_PARALLEL);
    // replace line with 0, and keep erased val on stack
    VoC_lw32_p(RL6,REG7,DEFAULT);
    VoC_sw32_p(RL7,REG7,DEFAULT); //0
    VoC_push32(RL6,DEFAULT);
    ****************************************************/


    VoC_lw16i_short(RL7_LO,5,DEFAULT); //   RL7     tot
    VoC_movreg16(INC2,REG4,IN_PARALLEL);
cor_h_x_loop_begin:

    VoC_lw32z(RL6,DEFAULT);         // max = 0;
    VoC_sub16_rr(REG7,REG0,REG2,IN_PARALLEL);

    VoC_loop_r(1,REG5)

    VoC_add16_rr(REG0,REG2,REG6,DEFAULT);   // REG0 x[j]
    VoC_movreg16(REG1,ACC1_HI,IN_PARALLEL); // REG1 h[0]

    /*********  SPEED OPTIMIZATION *********************
    // execute one more bimac in case of odd loop nb
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);
    VoC_aligninc_p(REG0,DEFAULT);
    VoC_shr16_ri(REG7,1,IN_PARALLEL);
    ****************************************************/

    VoC_loop_r_short(REG7,DEFAULT);
    VoC_add16_rr(REG7,REG2,REG2,IN_PARALLEL);    //  REG3 -> Y32     and fist address of  Y32  must be 0
    VoC_startloop0
//              VoC_bimac32inc_pp(REG0,REG1);
    VoC_mac32inc_pp(REG0,REG1,DEFAULT);
    VoC_endloop0

    VoC_lw16i(REG0,COR_H_X_Y32_ADDR);
    VoC_inc_p(REG2,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG0,IN_PARALLEL);

    VoC_lw16i(REG0,40);
    VoC_sw32_p(ACC0,REG7,DEFAULT);


    VoC_bnltz32_r(cor_h_x_250,ACC0)
    VoC_sub32_dr(ACC0,CONST_WORD32_0_ADDR,ACC0);
cor_h_x_250:

    VoC_sub16_rr(REG7,REG0,REG2,DEFAULT);

    VoC_bngt32_rr(cor_h_x_200,ACC0,RL6)
    VoC_movreg32(RL6,ACC0,DEFAULT);
cor_h_x_200:

    VoC_lw32z(ACC0,DEFAULT);
    VoC_endloop1

    VoC_shr32_ri(RL6,1,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_add32_rr(RL7,RL6,RL7,DEFAULT);
    VoC_movreg16(REG2,REG3,IN_PARALLEL); //REG2 : i

    VoC_bgt16_rr(cor_h_x_loop_begin,REG4,REG3)

    /*********  SPEED OPTIMIZATION *********************
    // get erased value
    VoC_pop32(RL6,DEFAULT);
    VoC_add16_rr(REG7,REG6,REG0,IN_PARALLEL);
    VoC_set_incr_i(REG0,1,DEFAULT);
    VoC_set_incr_i(REG1,1,IN_PARALLEL);
    // restitute erased value
    VoC_sw32_p(RL6,REG7,DEFAULT);
    ****************************************************/

    VoC_movreg16(REG7,ACC1_LO,DEFAULT);
    VoC_movreg32(ACC0,RL7,IN_PARALLEL);

    VoC_jal(CII_NORM_L_ACC0);

    VoC_pop16(RA,DEFAULT);
    VoC_sub16_rr(REG2,REG7,REG1,DEFAULT);

    //// _sd mode
    VoC_lw16i_set_inc(REG0,COR_H_X_Y32_ADDR,2);             //&y32
    VoC_lw16i_set_inc(REG1,C1035PF_DN_ADDR,1);              //&dn
    VoC_lw32_d(ACC1,CONST_0x00008000_ADDR);     //&0x8000

    VoC_lw32inc_p(REG45,REG0,DEFAULT);

    VoC_loop_i_short(40,DEFAULT)                /*    for (i = 0; i < L_CODE; i++)  */
    VoC_shr32_rr(REG45,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_add32_rr(ACC0,REG45,ACC1,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);
    VoC_shr32_rr(REG45,REG2,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    VoC_endloop0

    VoC_return;
}


/*************************************************************************
 *
 *  FUNCTION:  cor_h()
 *
 *  PURPOSE:  Computes correlations of h[] needed for the codebook search;
 *            and includes the sign information into the correlations.
 *
 *  DESCRIPTION: The correlations are given by
 *         rr[i][j] = sum_{n=i}^{L-1} h[n-i] h[n-j];   i>=j; i,j=0,...,L-1
 *
 *  and the sign information is included by
 *         rr[i][j] = rr[i][j]*sign[i]*sign[j]
 *
 *************************************************************************/


void CII_cor_h( void)
{

#if 0
    voc_short    COR_H_H2_ADDR                            ,40,x                // 40
    voc_short    COR_H_H2_COPY_ADDR                       ,40,y                // 40
    voc_short    COPY_C1035PF_SIGN_ADDR                   ,20,y                // 20
#endif



    VoC_push16(RA,DEFAULT);
    // stack16: RA,  xn2 ,st->h1, st->sharp,   gain_pit, res2,  code, y2, subfrNr, anap,  RA,RA

    VoC_lw16i_short(ACC0_LO,2,DEFAULT);

    VoC_lw16_sd(REG2,9,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_loop_i_short(20,DEFAULT);                       /*     for (i = 0; i < L_CODE; i++)*/
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);
    VoC_startloop0
    VoC_bimac32inc_pp(REG2,REG2);               /*s = L_mac (s, h[i], h[i]); */
    VoC_endloop0

    VoC_lw32_d(RL6,CONST_0x00008000_ADDR);
    VoC_movreg32(REG01,ACC0,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_bne16_rd(COR_H101,REG1,CONST_0x7FFF_ADDR)                   /* if (j == 0) */

    VoC_lw16_sd(REG0, 9, DEFAULT);                                                    /* REG0 for h[] address */
    VoC_lw16i_set_inc(REG1,COR_H_H2_ADDR,2);                            /* REG1 for h2[] address */
    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16i_short(REG3,10,IN_PARALLEL);
    VoC_jal(CII_scale);
    VoC_jump(COR_H102);

COR_H101:

    VoC_shr32_ri(REG01,1,DEFAULT);              /* s = L_shr (s, 1); */
    VoC_jal(CII_Inv_sqrt);

    VoC_shr32_ri(REG01,-7,DEFAULT);             /* k = extract_h (L_shl (Inv_sqrt (s), 7)); */
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i(REG0,32440);
    VoC_multf16_rr(REG5,REG1,REG0,DEFAULT);     /*REG5 for k k = mult (k, 32440); k = 0.99*k */
    /* REG0 for h[] address */
    VoC_lw16_sd(REG0,9,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,COR_H_H2_ADDR,2);

    VoC_lw16i_short(FORMAT32,-10,DEFAULT);
    VoC_multf32inc_rp(ACC0,REG5,REG0,DEFAULT);  /* L_mult (h[i], k) */
    VoC_multf32inc_rp(ACC1,REG5,REG0,DEFAULT);  /* L_mult (h[i], k) */
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_loop_i_short(20,DEFAULT)                        /* for (i = 0; i < L_CODE; i++) */
    VoC_add32_rr(ACC1,ACC1,RL6,IN_PARALLEL);
    VoC_startloop0

    VoC_multf32inc_rp(ACC0,REG5,REG0,DEFAULT);  /* L_mult (h[i], k) */
    VoC_movreg16(ACC1_LO,ACC0_HI,IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rr(ACC0, ACC0,RL6,DEFAULT);
    VoC_multf32inc_rp(ACC1,REG5,REG0,IN_PARALLEL);  /* L_mult (h[i], k) */
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC1,REG1,DEFAULT);
    VoC_add32_rr(ACC1, ACC1,RL6,DEFAULT);
    exit_on_warnings=ON;

    VoC_endloop0

    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
COR_H102:

    /*compute correlations
    VoC_lw16i_set_inc(REG0,C1035PF_SIGN_ADDR,2);
    VoC_lw16i_set_inc(REG1,COPY_C1035PF_SIGN_ADDR,2);
    VoC_jal(CII_copy_xy);
    */
    /*copy h2 to RAM_Y*/
    VoC_lw16i_short(REG6,10,DEFAULT);
    VoC_lw16i_set_inc(REG0,COR_H_H2_ADDR,2);
    VoC_lw16i_set_inc(REG1,COR_H_H2_COPY_ADDR,2);

    VoC_jal(CII_copy_xy);

    VoC_lw16i_set_inc(REG0,COR_H_H2_ADDR,1);
    VoC_lw16i_set_inc(REG1,COR_H_H2_COPY_ADDR,1);
    VoC_lw16i_set_inc(REG2,(C1035PF_RR_DIAG_ADDR+39) ,-1 );

    VoC_multf32inc_pp(ACC0,REG0,REG1,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_add32_rr(REG67,ACC0,RL6,DEFAULT);

    VoC_mac32inc_pp(REG0,REG1,DEFAULT);

    VoC_loop_i_short(20,DEFAULT);
    VoC_startloop0

    VoC_add32_rr(REG45,ACC0,RL6,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,IN_PARALLEL);
    VoC_mac32inc_pp(REG0,REG1,DEFAULT);
    VoC_mac32inc_pp(REG0,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_add32_rr(REG67,ACC0,RL6,DEFAULT);
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    exit_on_warnings=ON;

    VoC_endloop0

    //_sd mode
    VoC_lw16i(ACC1_LO,COR_H_H2_ADDR);//&h2[0] and &h2_copy[0]
    VoC_lw16i(ACC1_HI,COR_H_H2_COPY_ADDR);

    VoC_lw16i_set_inc(REG3,(C1035PF_RR_SIDE_ADDR+779-38) ,-1  );

    VoC_lw16i_short(RL7_HI,1,DEFAULT);                          //dec=1
    VoC_lw16i_short(RL7_LO,38,IN_PARALLEL);                         //i=38

    VoC_push32(ACC1,DEFAULT);
    VoC_push16(REG3,IN_PARALLEL);

    VoC_lw16i(ACC1_LO,C1035PF_SIGN_ADDR+39);       //sign[j]
    VoC_lw16i(ACC1_HI,C1035PF_SIGN_ADDR);

    VoC_loop_i(1,39)

    VoC_movreg32(REG45,RL7,DEFAULT);
    VoC_lw32_sd(REG01,0,IN_PARALLEL);        //REG0:&h2[0]

    VoC_add16_rr(REG1,REG1,REG5,DEFAULT);       //REG1:&h2_copy[dec]
    VoC_movreg32(REG23,ACC1,IN_PARALLEL);

    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_lw16_sd(REG6,0,IN_PARALLEL);

    VoC_sub16_dr(REG7,CONST_40_ADDR,REG5);  //REG7:40-dec:loop number

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);   //REG2:&rr[39][i]
    VoC_lw16i_short(REG5,39,IN_PARALLEL);

    VoC_mac32inc_pp(REG0,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_movreg32(ACC0,RL6,IN_PARALLEL);
    exit_on_warnings=ON;

    VoC_lw16i_short(RL6_HI,1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_push32(RL6,IN_PARALLEL);
    exit_on_warnings=ON;

    VoC_loop_r_short(REG7,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG2,DEFAULT);
    VoC_multf16inc_rp(REG6,REG7,REG3,DEFAULT);
    VoC_movreg16(RL6_LO,REG5,IN_PARALLEL);

    VoC_movreg16(REG7,ACC0_HI,DEFAULT);
    VoC_multf16_rr(REG7,REG7,REG6,DEFAULT);
    VoC_mac32inc_pp(REG0,REG1,DEFAULT);

    VoC_sub32_rr(REG45,REG45,RL6,DEFAULT);

    exit_on_warnings=OFF;
    VoC_sw16_p(REG7,REG4,DEFAULT);
    exit_on_warnings=ON;


    VoC_endloop0

    VoC_pop32(RL6,DEFAULT);
    VoC_add32_rd(RL7,RL7,CONST_0x0000ffffL_ADDR);//dec++;i--

    VoC_endloop1

    VoC_pop32(ACC1,DEFAULT);
    VoC_pop16(REG3,IN_PARALLEL);

    VoC_return;
}



void CII_G_pitch(void)
{
    /* divide by 2 "y1[]" to avoid overflow */
#if 0
    voc_short    G_PITCH_SCALED_Y1_ADDR                 ,40,y
    voc_short    G_PITCH_Y1_ADDR                        ,40,y
#endif


    /*start of program*/
    VoC_push16(RA,DEFAULT);

    VoC_lw16i_set_inc(REG0,G_PITCH_Y1_ADDR,2);                  //REG0 addr of y1
    VoC_lw16i_set_inc(REG1,G_PITCH_SCALED_Y1_ADDR,2);               //REG1 addr of scaled_y1
    VoC_lw16i_set_inc(REG2,COD_AMR_Y1_ADDRESS,2);

    VoC_lw32_d(ACC1,CONST_1_ADDR);

    VoC_movreg16(RL6_HI,REG0,DEFAULT);
    VoC_movreg16(RL7_HI,REG1,IN_PARALLEL);

    VoC_movreg16(RL6_LO,REG0,DEFAULT);
    VoC_movreg16(RL7_LO,REG1,IN_PARALLEL);

    VoC_loop_i_short(20,DEFAULT);
    VoC_lw32inc_p(REG45, REG2, IN_PARALLEL);
    VoC_startloop0;
    VoC_shr16_ri(REG4,2,DEFAULT);
    VoC_shr16_ri(REG5,2,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(REG45, REG0, DEFAULT);
    VoC_sw32inc_p(REG45, REG1, DEFAULT);
    VoC_lw32inc_p(REG45, REG2, IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop0;

    VoC_movreg32(REG01,RL6,DEFAULT);
    VoC_lw16i_short(REG7,4,IN_PARALLEL);

    VoC_loop_i(1,2)

//      VoC_set_status(STATUS_OVF0,0);              //overflow[0]=0
    VoC_lw16i(STATUS,OVF0_CLR);

    VoC_loop_i_short(20,DEFAULT);
    VoC_movreg32(ACC0,ACC1,IN_PARALLEL);
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG1);
    VoC_endloop0

    VoC_NOP();

    VoC_bnoe0z16_r(G_PITCH_01_ELSE) //if(overflow == 0
    VoC_jal(CII_NORM_L_ACC0);
    VoC_jump(G_PITCH_01_END);
G_PITCH_01_ELSE:

    VoC_movreg32(REG01,RL7,DEFAULT);
    VoC_movreg32(ACC0,ACC1,DEFAULT);

    VoC_loop_i_short(20,DEFAULT);
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG1);
    VoC_endloop0

    VoC_jal(CII_NORM_L_ACC0);

    VoC_sub16_rr(REG1,REG1,REG7,DEFAULT);   //REG1 for exp_yy-=4

G_PITCH_01_END:

    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000_ADDR);

    VoC_lw16i_short(REG7,2,DEFAULT);
    VoC_movreg16(ACC0_LO,REG1,IN_PARALLEL);

    VoC_movreg32(REG45,REG23,DEFAULT);
    VoC_movreg32(REG23,ACC0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,COD_AMR_XN_ADDRESS,2);

    VoC_movreg16(REG0,RL6_HI,DEFAULT);
    VoC_movreg16(RL7_HI,REG1,IN_PARALLEL);

    VoC_endloop1

    // REG45    exp_yy,  yy
    // REG23    exp_xy,  xy

    VoC_lw16i_short(REG1,15,DEFAULT);
    VoC_lw16i_set_inc(REG0,COD_AMR_GCOEFF_ADDRESS, 1);

    VoC_sub16_rr(REG6,REG1,REG4,DEFAULT);

    VoC_sw16inc_p(REG5, REG0,DEFAULT);
    VoC_sub16_rr(REG7,REG1,REG2,IN_PARALLEL);

    VoC_sw16inc_p(REG6, REG0,DEFAULT);
    VoC_sub16_rr(REG6,REG2,REG4,IN_PARALLEL);                       //    i = sub (exp_xy, exp_yy);

    VoC_sw16inc_p(REG3, REG0,DEFAULT);
    VoC_lw16i_short(REG2,0,IN_PARALLEL);                                //REG2 for 0(gain)

    VoC_sub16_rd(REG1,REG3,CONST_4_ADDR);   //i=xy-4

    VoC_sw16inc_p(REG7, REG0,DEFAULT);
    VoC_shr16_ri(REG3,1,IN_PARALLEL);                   // xy=shr(xy,1)

    VoC_bltz16_r(G_PITCH_03_END1,REG1);         //i<0 ?

    VoC_movreg16(REG0,REG3,DEFAULT);
    VoC_movreg16(REG1,REG5,IN_PARALLEL);

    /****************************/
    // input in REG0, REG1
    // output in  REG2
    // used register RL67, REG012
    /****************************/

    VoC_jal(CII_DIV_S);
    //REG2 for gain

    VoC_shr16_rr(REG2,REG6,DEFAULT);    //    gain = shr (gain, i);
    VoC_lw16i_short(REG3,7,IN_PARALLEL);

    VoC_lw16i(REG1,19661);                              //REG0 for 19661
    VoC_bngt16_rr(G_PITCH_03_END,REG2,REG1);            //?gain>19661
    VoC_movreg16(REG2,REG1,DEFAULT);
G_PITCH_03_END:

    VoC_bne16_rd(G_PITCH_03_END1,REG3,GLOBAL_ENC_USED_MODE_ADDR )           // if (sub(mode, MR122) == 0)
    /* clear 2 LSBits */
    VoC_and16_ri(REG2,0XFFFC);              // gain = gain & 0xfffC;
G_PITCH_03_END1:

    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG2,COD_AMR_GAIN_PIT_ADDRESS);          //return gain
    VoC_return;

}




void CII_Pre_Process(void)
{
    VoC_lw16i_set_inc (REG3,STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR,1);//REG3指向signal的初始地址
    VoC_lw32_d(REG01,STRUCT_PRE_PROCESSSTATE_X0_ADDR);/* REG0 for x0 REG1 for x1 */
    VoC_lw16i_set_inc (REG2,STRUCT_PRE_PROCESSSTATE_Y1_LO_ADDR,1);//REG2指向y1_lo地址
    VoC_lw32_d(REG67,STRUCT_PRE_PROCESSSTATE_Y2_LO_ADDR);/* REG6 for y2_lo REG7 for y2_hi */
    VoC_lw32_p(REG45,REG2,DEFAULT);/* REG45 for y1_lo and y1_hi */
    VoC_loop_i(0,160)// loop begin
    /* Mpy_32_16 (y2_hi, y2_lo, a[2]) */

//  VoC_multf32_rd(ACC0,REG7,CONST_0xf16b_ADDR);
//  VoC_multf16_rd(REG6,REG6,CONST_0xf16b_ADDR);

    VoC_lw16i(REG2,0xf16b);
    VoC_multf32_rr(ACC0,REG7,REG2,DEFAULT);
    VoC_multf16_rr(REG6,REG6,REG2,DEFAULT);

    VoC_movreg16(REG2,REG1,DEFAULT);    /* x2 = x1 */
    VoC_mac32_rd(REG6,CONST_1_ADDR);

    VoC_lw16i(REG1,0x1e7f);
    VoC_mac32_rr(REG5,REG1,DEFAULT);
    VoC_multf16_rr(REG6,REG4,REG1,DEFAULT);

//  VoC_mac32_rd(REG5,CONST_0x1e7f_ADDR);
//  VoC_multf16_rd(REG6,REG4,CONST_0x1e7f_ADDR);


    VoC_movreg16(REG1,REG0,DEFAULT);/* x1 = x0 */
    VoC_lw16_p(REG0,REG3,IN_PARALLEL);  /* x0 = signal[i] */
    VoC_mac32_rd(REG6,CONST_1_ADDR);
    VoC_lw16i(REG6,0x076b);
    VoC_lw16i(REG7,0xf12a);

    VoC_mac32_rr(REG0,REG6,DEFAULT);
    VoC_mac32_rr(REG1,REG7,DEFAULT);
    VoC_mac32_rr(REG2,REG6,DEFAULT);


//  VoC_mac32_rd(REG0,CONST_0x076b_ADDR);
//  VoC_mac32_rd(REG1,CONST_0xf12a_ADDR);
//  VoC_mac32_rd(REG2,CONST_0x076b_ADDR);


    VoC_movreg32(REG67,REG45,DEFAULT);
    VoC_shr32_ri(ACC0,-3,DEFAULT);
    VoC_add32_rd(REG45,ACC0,CONST_0x00008000_ADDR);
    VoC_movreg16(REG4,ACC0_LO,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_movreg16(REG5,ACC0_HI,DEFAULT);
//  VoC_and16_ri(REG4,0X7FFF);

    VoC_endloop0
    VoC_sw32_d(REG01,STRUCT_PRE_PROCESSSTATE_X0_ADDR);/* REG0 for x0 REG1 for x1 */
    VoC_sw32_d(REG67,STRUCT_PRE_PROCESSSTATE_Y2_LO_ADDR);/* REG6 for y2_lo REG7 for y2_hi */
    VoC_sw32_d(REG45,STRUCT_PRE_PROCESSSTATE_Y1_LO_ADDR);   /* save y1_lo and y1_hi */

    VoC_return;


}


void CII_subframePreProc(void)
{
#if 0
    voc_short  PRE_BIG_AP1_ADDR                ,20,x                           //~20
    voc_short  PRE_BIG_AP2_ADDR                ,20,x                            //~40
#endif





    VoC_push16(RA,DEFAULT);
    VoC_lw16_d(REG6,GLOBAL_ENC_MODE_ADDR);

    VoC_push16(RL7_HI,DEFAULT);
    // A address
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,STATIC_CONST_gamma1_12K2_ADDR,1);      //  g1 = gamma1_12k2;
    VoC_bgt16_rd(cod_amr114,REG6,CONST_5_ADDR)          // if ( sub((Word16)mode, MR122) == 0 || sub((Word16)mode, MR102) == 0 )
    VoC_lw16i_set_inc(REG1,STATIC_CONST_gamma1_ADDR,1);      //  g1 = gamma1;
cod_amr114:
    /*---------------------------------------------------------------*
     * Find the weighted LPC coefficients for the weighting filter.  *
     *---------------------------------------------------------------*/
    /*
    PARAS:
    R0: &a      INC1
    R1: &fac    INC1
    R2: &a_exp  INC1
    */
    VoC_lw16i_set_inc(REG2,PRE_BIG_AP1_ADDR,1);   //   Weight_Ai(A, g1, Ap1);
    VoC_loop_i(1,2)
    VoC_lw16_sd(REG0,5,DEFAULT);
    VoC_jal(CII_Weight_Ai);
    VoC_lw16i_set_inc(REG1,STATIC_CONST_gamma2_ADDR,1);  //   Weight_Ai(A, gamma2, Ap2);
    VoC_lw16i_set_inc(REG2,PRE_BIG_AP2_ADDR,1);
    VoC_endloop1
    /*   for (i = 0; i <= M; i++)
    {
       ai_zero[i] = Ap1[i];        move16 ();
    }*/
    VoC_lw16i_set_inc(REG0,PRE_BIG_AP1_ADDR,1);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_AI_ZERO_ADDR,1);

    VoC_loop_i_short(11,DEFAULT)
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0


//  Syn_filt(Aq, ai_zero, h1, L_SUBFR, zero, 0);

    VoC_lw16_sd(REG5,4,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);

    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_AI_ZERO_ADDR,1);
    VoC_lw16i(REG7,STRUCT_COD_AMRSTATE_H1_ADDR);
    VoC_lw16i_short(REG0,0,DEFAULT);

    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_ZERO_ADDR,1);
    VoC_jal(CII_Syn_filt);
//   Syn_filt(Ap2, h1, h1, L_SUBFR, zero, 0);
    VoC_lw16i(REG5,PRE_BIG_AP2_ADDR);
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_H1_ADDR,1);
    VoC_movreg16(REG7,REG3,DEFAULT);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_ZERO_ADDR,1);
    VoC_jal(CII_Syn_filt);

    /*__________________________________________________________________
      Function:  CII_Syn_filt
     prameters:
       &a[0]  ->reg5
       &x[0]  -> reg3(inc 1 )
       &y     ->REG7
       lg     ->REG6
       &mem[0]->REG1 (incr=1) push32:
       update ->reg0

     Version 1.0  Create
     Version 1.1 optimized by Kenneth 09/01/2004
    ____________________________________________________________________*/


//   Residu(Aq, speech, res2, L_SUBFR);
    /*
    PARAS:
    R0:     &a  INC1
    REG3:   &x  INC1
    REG2:   &y  INC1

    */
    VoC_lw16_sd(REG0,4,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG2,COD_AMR_RES_ADDRESS,1);
    VoC_lw16_sd(REG6,2,DEFAULT);    // i_subfr
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_SPEECH_ADDR,1);
    VoC_bez16_d(subframePreProc500, GLOBAL_EFR_FLAG_ADDR)
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR,1);
subframePreProc500:
    VoC_add16_rr(REG3,REG6,REG3,DEFAULT);
    VoC_jal(CII_Residu_new);
//   Copy(res2, exc, L_SUBFR);
    /*
    INPUT:
    REG0: source address INC 2
    REG1: dest   address INC 2
    REG6: (number of Long Words copyed)/2 or (number of Word16 copyed)/4

    USED:   ACC0,ACC1
    */
    VoC_lw16i_set_inc(REG0,COD_AMR_RES_ADDRESS,2);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_EXC_ADDR,2);
    VoC_lw16_sd(REG6,2,DEFAULT);    // i_subfr
    VoC_add16_rr(REG1,REG6,REG1,DEFAULT);
    VoC_lw16i_short(REG6,10,DEFAULT);
    VoC_jal(CII_copy_xy);

//   Syn_filt(Aq, exc, error, L_SUBFR, mem_err, 0);
    VoC_lw16_sd(REG5,4,DEFAULT);
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_EXC_ADDR,1);
    VoC_lw16_sd(REG6,2,DEFAULT);    // i_subfr
    VoC_add16_rr(REG3,REG6,REG3,DEFAULT);
    VoC_lw16i(REG7,STRUCT_COD_AMRSTATE_ERROR_ADDR);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_MEM_ERROR_ADDR,1);
    /*__________________________________________________________________
      Function:  CII_Syn_filt
     prameters:
       &a[0]  ->reg5
       &x[0]  -> reg3(inc 1 )
       &y     ->REG7
       lg     ->REG6
       &mem[0]->REG1 (incr=1) push32:
       update ->reg0

     Version 1.0  Create
     Version 1.1 optimized by Kenneth 09/01/2004
    ____________________________________________________________________*/
    VoC_jal(CII_Syn_filt);

//   Residu(Ap1, error, xn, L_SUBFR);
    /*
    PARAS:
    R0:     &a  INC1
    REG3:   &x  INC1
    REG2:   &y  INC1

    */
    VoC_lw16i_set_inc(REG0,PRE_BIG_AP1_ADDR,1);
    VoC_lw16i_set_inc(REG2,COD_AMR_XN_ADDRESS,1);
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_ERROR_ADDR,1);
    VoC_jal(CII_Residu_new);

//   Syn_filt(Ap2, xn, xn, L_SUBFR, mem_w0, 0);
    VoC_lw16i(REG5,PRE_BIG_AP2_ADDR);
    VoC_lw16i_set_inc(REG3,COD_AMR_XN_ADDRESS,1);
    VoC_movreg16(REG7,REG3,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_pop16(REG1,IN_PARALLEL);
    VoC_lw16i_short(INC1,1,DEFAULT);
    /*__________________________________________________________________
      Function:  CII_Syn_filt
     prameters:
       &a[0]  ->reg5
       &x[0]  -> reg3(inc 1 )
       &y     ->REG7
       lg     ->REG6
       &mem[0]->REG1 (incr=1) push32:
       update ->reg0

     Version 1.0  Create
     Version 1.1 optimized by Kenneth 09/01/2004
    ____________________________________________________________________*/
    VoC_jal(CII_Syn_filt);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return
    // end of subframePreProc
}

/***********************************************
  Function CII_Int_lpc_only_2
  input: REG2->F0
         REG3->F1
         REG1(incr=2)->&lsp[] push(n)
  Description:  1/4 F0 + 3/4 F1
  Optimized by Kenneth  22/09/2004
***********************************************/
void CII_Int_lpc_only_2(void)
{
    VoC_lw16i_short(INC2,2,DEFAULT);


    VoC_lw16i_short(INC3,2,DEFAULT);
    VoC_lw32inc_p(REG45,REG2,DEFAULT);      //F0
    VoC_lw32_p(REG67,REG3,DEFAULT);         //F1
    VoC_loop_i_short((M/2),DEFAULT)
    VoC_startloop0
    VoC_shr16_ri(REG4,2,DEFAULT);
    VoC_shr16_ri(REG5,2,IN_PARALLEL);
    VoC_shr16_ri(REG6,2,DEFAULT);
    VoC_shr16_ri(REG7,2,IN_PARALLEL);
    VoC_sub16inc_pr(REG6,REG3,REG6,DEFAULT);
    VoC_sub16inc_pr(REG7,REG3,REG7,IN_PARALLEL);
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG7,IN_PARALLEL);
    VoC_lw32_p(REG67,REG3,DEFAULT);
    VoC_sw32inc_p(REG45,REG1,DEFAULT);
    VoC_lw32inc_p(REG45,REG2,DEFAULT);          //lsp_old
    VoC_endloop0

    VoC_return;
}
/***********************************************
  Function CII_Int_lpc_1to3
  input: REG2->&lsp_old[]
         REG3->&lsp_new[]
         REG0->&Az[]
         REG6->
         REG6=1 -->CII_Int_lpc_1to3
         REG6=0 -->CII_Int_lpc_1to3_2
  Optimized by Kenneth  22/09/2004
***********************************************/
void CII_Int_lpc_1to3(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_movreg16(REG4,REG3,DEFAULT);
    VoC_lw16i_set_inc(REG1,INT_LPC_LSP_ADDR,2);
    VoC_push16(REG6,DEFAULT);
    VoC_push32(REG23,IN_PARALLEL);
    VoC_push32(REG01,DEFAULT);
    VoC_movreg16(REG5,REG2,IN_PARALLEL);

    VoC_movreg32(REG23,REG45,DEFAULT);
    VoC_jal(CII_Int_lpc_only_2);
    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_jal(CII_Lsp_Az);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_lw32_sd(REG23,1,DEFAULT);
    VoC_jal(CII_Int_lpc_only);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_11_ADDR);
    VoC_jal(CII_Lsp_Az);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_lw32_sd(REG23,1,DEFAULT);
    VoC_jal(CII_Int_lpc_only_2);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_22_ADDR);
    VoC_jal(CII_Lsp_Az);

    VoC_pop16(REG6,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);

    VoC_bez16_r(Int_lpc_1to3_L,REG6);
    VoC_lw32_sd(REG23,0,DEFAULT);
    VoC_lw16i_short(REG7,33,IN_PARALLEL);
    VoC_movreg16(REG1,REG3,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG7,IN_PARALLEL);
    VoC_jal(CII_Lsp_Az);
Int_lpc_1to3_L:
    VoC_pop16(RA,DEFAULT);
    VoC_pop32(REG23,DEFAULT);
    VoC_return;
}




/**********************************************
  Function CII_build_code_com2
  input: REG6->NB_PULSE
     REG7->h[]
     REG1(incr=1)->codvec[]
     REG0(incr=1)->y[]
     REG4->_sign[]
  Note: This function can used in all mode
  Create by Kenneth  09/29/2004
**********************************************/

void CII_build_code_com2(void)
{
#if 0
    voc_short    BUILD_CODE_POINT_ADDR_ADDR           ,10,y
#endif


    VoC_lw16i_set_inc(REG1,C1035PF_CODVEC_ADDR,1);
    VoC_lw16i_set_inc(REG3,BUILD_CODE_POINT_ADDR_ADDR,1);
    VoC_lw16i_set_inc(REG2,build_code_sign_ADDR,1);
    VoC_loop_r_short(REG6,DEFAULT);
    VoC_movreg32(RL6,REG23,IN_PARALLEL);
    VoC_startloop0
    VoC_sub16inc_rp(REG5,REG7,REG1,DEFAULT);
    VoC_lw32_d(ACC0,CONST_0x00008000_ADDR);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_endloop0

    VoC_movreg32(RL7,ACC0,DEFAULT);
    VoC_movreg32(REG23,RL6,IN_PARALLEL);

    VoC_loop_i(1,40);

    VoC_loop_r_short(REG6,DEFAULT);
    VoC_startloop0
    VoC_lw16_p(REG1,REG3,DEFAULT);
    VoC_lw16inc_p(REG5,REG2,DEFAULT);

    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_sw16inc_p(REG1,REG3,DEFAULT);
    VoC_endloop0
    VoC_movreg32(ACC0,RL7,DEFAULT);
    VoC_movreg32(REG23,RL6,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop1
    VoC_return;
}




/******************************************************************************
*      File             : amr_dec_main.c

******************************************************************************/

/*---------------------------------------------------------------------------
    Function:  void CII_Pred_lt_3or6(void)
    Word16 exc[],        in/out: REG5
    Word16 T0,           input : REG6
    Word16 frac,         input : REG7
    Word16 L_subfr,      input : subframe size
    Word16 flag3         input : REG2
    Version 1.0
    Version 1.1   revised by Kenneth 08/04/2004
-----------------------------------------------------------------------------*/
void CII_Pred_lt_3or6(void)
{

    VoC_sub16_rr(REG6, REG5,REG6,DEFAULT);//x0 = &exc[-T0];

    VoC_lw16i_short(INC0,-1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_lw16i_short(INC2,6,DEFAULT);
    VoC_lw16i_short(INC3,6,IN_PARALLEL);
    VoC_sub16_dr(REG1,CONST_0_ADDR,REG7);// frac = negate (frac);

    VoC_bez16_r(PRED_IT_3OR6_001,REG2)
    VoC_shr16_ri(REG1,-1,DEFAULT);
PRED_IT_3OR6_001:
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG3,6,IN_PARALLEL);

    VoC_bnltz16_r(PRED_IT6_003,REG1)//if (frac < 0)   REG4   frac
    VoC_add16_rr(REG1,REG1,REG3,DEFAULT);//frac = add (frac, UP_SAMP);
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//x0--;
PRED_IT6_003:

    VoC_lw16i(REG7,STATIC_CONST_INTER_6_ADDR);//REG7   intere_6 addr
    VoC_add16_rr(REG2,REG7,REG1,DEFAULT);//c1 = &inter_6[frac]
    VoC_sub16_rr(REG3,REG3,REG1,IN_PARALLEL);   // sub (UP_SAMP_MAX, frac)
    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);//&inter_6[sub (UP_SAMP, frac)];
    VoC_lw32_d(ACC0,CONST_0x00008000_ADDR);
    VoC_movreg32(RL7, REG23,DEFAULT);
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);

    VoC_loop_i(1,40);

    VoC_movreg16(REG0,REG6,DEFAULT);    //  x1 = x0++;
    VoC_add16_rr(REG6,REG6,REG4,IN_PARALLEL);

    VoC_loop_i_short(10,DEFAULT);
    VoC_movreg16(REG1,REG6,IN_PARALLEL); //   x2 = x0;
    VoC_startloop0
    VoC_mac32inc_pp(REG0,REG2,DEFAULT);
    VoC_mac32inc_pp(REG1,REG3,DEFAULT);
    VoC_endloop0
    VoC_movreg32(REG23,RL7, DEFAULT);
    exit_on_warnings=OFF;
    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);//exc++
    VoC_sw16_p(ACC0_HI,REG5,DEFAULT);//exc[j] = round (s);
    VoC_movreg32(ACC0,RL6,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop1
    VoC_return;
}


/*_________________________________________________________________
  Function:  CII_Syn_filt
 prameters:
   &a[0]  ->reg5
   &x[0]  -> reg3(inc 1 )
   &y     ->REG7
   lg     ->REG6
   &mem[0]->REG1 (incr=1) push32:
   update ->reg0

 Version 1.0  Create
 Version 1.1 optimized by Kenneth 09/01/2004
____________________________________________________________________*/
void CII_Syn_filt (void)
{



#if 0
    voc_short   SYN_FILT_TMP_ADDR                ,80    ,y
#endif


    // Copy mem[] to yy[]
    VoC_push32(REG01,DEFAULT);

    VoC_lw16i_set_inc(REG2,SYN_FILT_TMP_ADDR,1);
    VoC_loop_i_short(10,DEFAULT);
    VoC_lw16inc_p(REG4,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw16i_short(REG0,1,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG2,DEFAULT);           //reg2:yy
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop0

//do the filtering
    VoC_lw16_p(REG4,REG5,DEFAULT);          //reg4:a[0]
    VoC_add16_rr(REG5,REG5,REG0,IN_PARALLEL);       //reg5:&a[1]
    VoC_movreg16(REG0,REG5,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);

//prepare for the loop

    VoC_lw16i_set_inc(REG1,SYN_FILT_TMP_ADDR+9,-2);//&yy[-1]

    VoC_loop_r(1,REG6)
    VoC_aligninc_pp(REG0,REG1,DEFAULT);
//      exit_on_odd_address = OFF;
    VoC_loop_i_short(5,DEFAULT)
    VoC_multf32inc_rp(ACC0,REG4,REG3,IN_PARALLEL);
    VoC_startloop0
    VoC_bimsu32inc_pp(REG0,REG1);
    VoC_endloop0
    VoC_movreg16(REG0,REG5,DEFAULT);        //&a[1]

    VoC_shr32_ri(ACC0,-3,DEFAULT);
    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000_ADDR);

    VoC_movreg16(REG1,REG2,DEFAULT);    //&yy[-1]
    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);
//  exit_on_odd_address = ON;
    VoC_endloop1

    VoC_lw16i_set_inc(REG0,SYN_FILT_TMP_ADDR+10,2); // REG0 for tmp[m] address

    VoC_movreg16(REG5,REG6,DEFAULT);        //lg
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_shr16_ri(REG6,2,DEFAULT);           //lg/4
    VoC_movreg16(REG1,REG7,IN_PARALLEL);        //&y[0]
    VoC_jal(CII_copy_xy);
    VoC_bne16_rd(SYN_FILT_01,REG6,CONST_5_ADDR);
    VoC_sw32_p(ACC0,REG1,DEFAULT);
SYN_FILT_01:
    // Update of memory if update==1
    VoC_lw32_sd(REG01,0,DEFAULT);           //REG1:&mem[0]
    VoC_bez16_r(Syn_filt103,REG0)
    VoC_lw16i(REG0,SYN_FILT_TMP_ADDR);
    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);       //&y[lg-m]
    VoC_jal(CII_Copy_M);
Syn_filt103:
    VoC_pop16(RA,DEFAULT);
    VoC_pop32(REG01,DEFAULT);
    VoC_return;
}


void CII_amr_dec_com_sub1(void)
{
    VoC_shr16_ri(REG5,-1,DEFAULT);

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_pop16(REG0,DEFAULT);                // pop16 stack[n-4]
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
    VoC_lw16d_set_inc(REG2,DEC_AMR_T0_ADDRESS,1);

    VoC_bez16_d(amr_dec_com_sub1_L1,GLOBAL_EFR_FLAG_ADDR)
    VoC_shr16_ri(REG5,-2,DEFAULT);
amr_dec_com_sub1_L1:

    VoC_bnlt16_rd(Dec_amr_L32A,REG2,CONST_40_ADDR);
    VoC_sub16_rr(REG4,REG6,REG2,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG0,IN_PARALLEL);
    VoC_multf16inc_rp(REG6,REG5,REG0,DEFAULT);
    VoC_loop_r_short(REG4,DEFAULT);             //for (i = T0; i < L_SUBFR; i++)
    //   temp = mult (code[i - T0], pit_sharp);
    VoC_startloop0                      //{
    //   temp = mult (code[i - T0], pit_sharp);
    VoC_add16_rp(REG6,REG6,REG2,DEFAULT);        //   code[i] = add (code[i], temp);
    VoC_multf16inc_rp(REG6,REG5,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0                        //}
Dec_amr_L32A:
    VoC_lw16_sd(REG2,2,DEFAULT);            // parm in reg2
    VoC_sw16_d(REG5,DEC_AMR_PIT_SHARP_ADDRESS);
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    VoC_sw16_sd(REG2,2,DEFAULT);
    VoC_return;
}



void CII_amr_dec_com_sub3(void)
{
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_push32(REG67,DEFAULT);              //  push32 stack32[n]  pitch_fac/tmp_shift
    VoC_lw16i_set_inc(REG0,DEC_AMR_EXC_ENHANCED_ADDRESS,1);
    VoC_lw16i_set_inc(REG1,STRUCT_DECOD_AMRSTATE_EXC_ADDR,1);
    VoC_lw16i_set_inc(REG2,DEC_AMR_CODE_ADDRESS,1);
    VoC_loop_i_short(40,DEFAULT);           //for (i = 0; i < L_SUBFR; i++)
    VoC_sub16_rr(REG7,REG4,REG7,IN_PARALLEL);
    VoC_startloop0                  //{
    VoC_lw16_p(REG3,REG1,DEFAULT);      //   exc_enhanced[i] = st->exc[i];
    VoC_multf32_rr(ACC0,REG3,REG6,DEFAULT); //   L_temp = L_mult (st->exc[i], pitch_fac);
    VoC_mac32inc_rp(REG5,REG2,DEFAULT); //   L_temp = L_mac (L_temp, code[i], gain_code);
    VoC_sw16inc_p(REG3,REG0,DEFAULT);
    VoC_shr32_rr(ACC0,REG7,DEFAULT);    //   L_temp = L_shl (L_temp, tmp_shift);
    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000_ADDR);  //   st->exc[i] = round (L_temp);
    // put it here to replace a NOP
    VoC_lw16i_short(REG4,2,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    VoC_endloop0                //}

    VoC_lw16i_short(REG5,0,DEFAULT);        //   ph_disp_release(st->ph_disp_st);

    VoC_blt16_rd(Dec_amr_L64,REG4,GLOBAL_DEC_MODE_ADDR);    //if ( ((sub(mode, MR475) == 0) ||(sub(mode, MR515) == 0) ||(sub(mode, MR59) == 0))   &&
    VoC_bez16_d(Dec_amr_L64,DEC_AMR_BFI_ADDRESS);       //     sub(st->voicedHangover, 3) > 0 && st->inBackgroundNoise != 0 && bfi != 0 )
    VoC_lw32_sd(REG67,1,DEFAULT);
    VoC_bez16_r(Dec_amr_L64,REG6);
    VoC_bngt16_rd(Dec_amr_L64,REG7,CONST_3_ADDR);
    VoC_lw16i_short(REG5,1,DEFAULT);    //{  ph_disp_lock(st->ph_disp_st);}
Dec_amr_L64:
    VoC_lw16i_set_inc(REG3,STRUCT_PH_DISPSTATE_GAINMEM_ADDR,1);
    VoC_sw16_d(REG5,STRUCT_PH_DISPSTATE_LOCKFULL_ADDR);

    VoC_lw16i_set_inc(REG2,DEC_AMR_EXC_ENHANCED_ADDRESS,1);
    VoC_lw16_d(REG7,DEC_AMR_GAIN_PIT_ADDRESS);
    VoC_lw16i_set_inc(REG0,DEC_AMR_CODE_ADDRESS,1);
    VoC_push16(REG3,DEFAULT);           //push16 stack[n-4]   st->ph_disp_st
    VoC_push16(REG2,DEFAULT);           //push16 stack[n-5]   exc_enhanced[]
    VoC_push16(REG0,DEFAULT);
    //ph_disp(st->ph_disp_st, mode,exc_enhanced, gain_code_mix, gain_pit, code, pitch_fac, tmp_shift);
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /**************************************************************************
     Function:    CII_ph_disp
     Input: st->ph_disp_st  ->REG3(incr 1) push stack[n]
            mode        ->
            exc_enhanced[]  ->REG2(incr 1) push stack[n-1]
            gain_code_mix   ->DEC_AMR_GAIN_CODE_MIX_ADDRESS //(direct address access, needn't pass the address as a parameter)
            gain_pit        ->REG7
            code[]      ->REG0(incr 1) push stack[n-2]
            pitch_fac       ->ACC0_LO  push32
            tmp_shift       ->ACC0_HI  push32
     Created by Kenneth    07/12/2004
     Optimized by Kenneth  09/02/2004
    **************************************************************************/
//  begin of ph_disp
    VoC_lw32_sd(RL6,2,DEFAULT);
    VoC_lw16i_set_inc(REG2,STRUCT_PH_DISPSTATE_GAINMEM_ADDR+6,2);
    VoC_lw16i_set_inc(REG3,STRUCT_PH_DISPSTATE_GAINMEM_ADDR+3,-1);
    VoC_push16(REG2,DEFAULT);
    VoC_lw32inc_p(REG45,REG2,DEFAULT);          //PREVSTATE/PREVCBGAIN
    VoC_lw32_p(REG01,REG2,DEFAULT);         //LOCKFULL/ONSET
    VoC_lw16i_short(REG4,2,IN_PARALLEL);
    VoC_lw16i_set_inc(REG2,STRUCT_PH_DISPSTATE_GAINMEM_ADDR+4,-1);  //for (i = PHDGAINMEMSIZE-1; i > 0; i--)
    VoC_loop_i_short(4,DEFAULT);            //{
    VoC_lw16inc_p(REG6,REG3,IN_PARALLEL);
    VoC_startloop0;                 //    state->gainMem[i] = state->gainMem[i-1];     }
    VoC_lw16inc_p(REG6,REG3,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_sw16_p(REG7,REG2,DEFAULT);          //state->gainMem[0] = ltpGain;
    VoC_lw16i_short(REG3,1,IN_PARALLEL);
    VoC_bnlt16_rd(Phdisp_L1,REG7,CONST_14746_ADDR); //if (sub(ltpGain, PHDTHR2LTP) < 0) {
    VoC_lw16i_short(REG4,0,DEFAULT);            // } else
    VoC_bngt16_rd(Phdisp_L1,REG7,CONST_9830_ADDR);  //    if (sub(ltpGain, PHDTHR1LTP) > 0)
    VoC_lw16i_short(REG4,1,DEFAULT);            //    {  impNr = 1;

Phdisp_L1:                          //{impNr = 2; }  REG2->impNr
    //tmp1 = round(L_shl(L_mult(state->prevCbGain, ONFACTPLUS1), 2));
    VoC_multf32_rd(REG67,REG5,CONST_0x4000_ADDR);
    VoC_NOP();
    VoC_shr32_ri(REG67,-2,DEFAULT);
    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);

    VoC_bnlt16_rd(Phdisp_L4,REG7,DEC_AMR_GAIN_CODE_MIX_ADDRESS);    //if (sub(cbGain, tmp1) > 0)
    VoC_lw16i_short(REG1,2,DEFAULT);            //{state->onset = ONLENGTH;
    VoC_jump(Phdisp_L5);                    //}
Phdisp_L4:                          //else
    VoC_bngtz16_r(Phdisp_L5,REG1);          //{  if (state->onset > 0)
    VoC_sub16_rr(REG1,REG1,REG3,DEFAULT);       //  {  state->onset = sub (state->onset, 1);    }
Phdisp_L5:                          //}

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_bnez16_r(Phdisp_L6,REG1);           //if (state->onset == 0)
    //{   i1 = 0;
    VoC_loop_i_short(5,DEFAULT);                //    for (i = 0; i < PHDGAINMEMSIZE; i++)
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_startloop0                  //    {
    VoC_sub16inc_dp(REG7,CONST_9830_ADDR,REG2);
    VoC_bngtz16_r(Phdisp_L7,REG7);          //        if (sub(state->gainMem[i], PHDTHR1LTP) < 0)
    VoC_add16_rr(REG6,REG6,REG3,DEFAULT);       //        {i1 = add (i1, 1);}
Phdisp_L7:
    VoC_NOP();
    VoC_endloop0;                   //    }

    VoC_bngt16_rd(Phdisp_L6,REG6,CONST_2_ADDR);     //    if (sub(i1, 2) > 0)
    VoC_lw16i_short(REG4,0,DEFAULT);            //    {impNr = 0;    }
Phdisp_L6:                          //}
    VoC_bnez16_r(Phdisp_L8,REG1);           //if ((sub(impNr, add(state->prevState, 1)) > 0) && (state->onset == 0))
    VoC_sub16_rr(REG2,REG4,REG3,DEFAULT);
    VoC_bngt16_rd(Phdisp_L8,REG2,STRUCT_PH_DISPSTATE_PREVSTATE_ADDR);//{
    VoC_movreg16(REG4,REG2,DEFAULT);            //    impNr = sub (impNr, 1);}
Phdisp_L8:
    VoC_bngtz16_r(Phdisp_L9,REG1);
    VoC_bnlt16_rd(Phdisp_L9,REG4,CONST_2_ADDR);     //if((sub(impNr, 2) < 0) && (state->onset > 0))
    VoC_add16_rr(REG4,REG4,REG3,DEFAULT);       //{impNr = add (impNr, 1);}
Phdisp_L9:

    VoC_lw16i_short(INC3,2,DEFAULT);
    VoC_lw16_d(REG5,DEC_AMR_GAIN_CODE_MIX_ADDRESS);
    VoC_bnlt16_rd(Phdisp_L10,REG5,CONST_10_ADDR);   //if(sub(cbGain, 10) < 0)
    VoC_lw16i_short(REG4,2,DEFAULT);            //{impNr = 2;   }
Phdisp_L10:
    VoC_pop16(REG3,DEFAULT);
    VoC_bne16_rd(Phdisp_L11,REG0,CONST_1_ADDR);     //if(sub(state->lockFull, 1) == 0)
    VoC_lw16i_short(REG4,0,DEFAULT);            //{impNr = 0;}
Phdisp_L11:
    VoC_lw16_sd(REG2,0,DEFAULT);
    VoC_sw32inc_p(REG45,REG3,DEFAULT);          //state->prevState = impNr;
    VoC_sw32_p(REG01,REG3,DEFAULT);         //state->prevCbGain = cbGain;

    VoC_return;
}



void CII_amr_dec_com_sub4(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16_sd(REG1,2,DEFAULT);
    VoC_lw32_sd(REG67,0,IN_PARALLEL);
    VoC_sub16_dr(REG7,CONST_0_ADDR,REG7);

    VoC_loop_i_short(40,DEFAULT);           //for (i = 0; i < L_SUBFR; i++)
    VoC_lw16_sd(REG2,1,IN_PARALLEL);
    VoC_startloop0                  //{
    VoC_multf32_rp(ACC0,REG6,REG1,DEFAULT);     //L_temp = L_mult (x[i],pitch_fac);
    VoC_mac32inc_rp(REG5,REG2,DEFAULT);     //   L_temp = L_mac  (L_temp,inno[i],cbGain);

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_shr32_rr(ACC0,REG7,DEFAULT);            //   L_temp = L_shl  (L_temp, tmp_shift);
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);    //   x[i] = round (L_temp);

    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    VoC_endloop0            //}
//  end of ph_disp

    VoC_pop32(ACC1,DEFAULT);            //  pop32 stack32[n]  pitch_fac/tmp_shift
    VoC_lw32z(ACC0,IN_PARALLEL);        //L_temp = 0;
    VoC_lw16_sd(REG2,2,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_loop_i_short(20,DEFAULT);          //for (i = 0; i < L_SUBFR; i++)
    VoC_startloop0                 //{
    VoC_bimac32inc_pp(REG2,REG2);  //    L_temp = L_mac (L_temp, exc_enhanced[i], exc_enhanced[i] );
    VoC_endloop0                   //}
    VoC_lw16i_short(REG5,17,DEFAULT);
    VoC_shr32_ri(ACC0,1,DEFAULT);          //L_temp = L_shr (L_temp, 1);

    VoC_jal(CII_sqrt_l_exp);           //L_temp = sqrt_l_exp(L_temp, &temp);

    VoC_shr16_ri(REG1,1,DEFAULT);          //L_temp = L_shr(L_temp, add( shr(temp, 1), 15));
    VoC_pop16(RA,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG5,IN_PARALLEL);
    VoC_lw32_sd(REG67,0,DEFAULT);
    VoC_shr32_rr(ACC0,REG1,IN_PARALLEL);       //L_temp = L_shr(L_temp, 2);
    VoC_pop16(REG0,DEFAULT);
    VoC_movreg16(REG5,ACC0_LO,IN_PARALLEL);    //excEnergy = extract_l(L_temp);

    VoC_return;
}

void CII_amr_dec_com_sub5(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(REG3,4,DEFAULT);
    VoC_lw16i_set_inc(REG0,STRUCT_DECOD_AMRSTATE_EXCENERGYHIST_ADDR+8,-1);// {

    VoC_bez16_d(Dec_amr_L67A,STRUCT_DECOD_AMRSTATE_INBACKGROUNDNOISE_ADDR);//if ( st->inBackgroundNoise != 0 && ( bfi != 0 || st->prev_bf != 0 ) && sub(st->state, 4) < 0 )
    VoC_bngt16_rd(Dec_amr_L67A,REG3,STRUCT_DECOD_AMRSTATE_STATE_ADDR)   //{
    VoC_bnez16_d(Dec_amr_L68,DEC_AMR_BFI_ADDRESS);              //   ;
    VoC_bnez16_d(Dec_amr_L68,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);       //}
Dec_amr_L67A:                                   //else
    //   for (i = 0; i < 8; i++)

    //   st->excEnergyHist[8] = excEnergy;

    VoC_loop_i_short(9,DEFAULT);   //   {
    VoC_lw16_p(REG5,REG0,IN_PARALLEL);
    VoC_startloop0
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG0,DEFAULT);//      st->excEnergyHist[i] = st->excEnergyHist[i+1]; }
    exit_on_warnings=ON;
    VoC_lw16_p(REG5,REG0,DEFAULT);
    VoC_endloop0

Dec_amr_L68:                    //}
    VoC_lw16_sd(REG1,1,DEFAULT);

    VoC_lw16i_set_inc(REG0,DEC_AMR_EXCP_ADDRESS,1);
    VoC_lw16_d(REG4,DEC_AMR_PIT_SHARP_ADDRESS);

    VoC_movreg32(REG23,REG01,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_bngt16_rd(Dec_amr_L69,REG4,CONST_0x4000_ADDR); //if (sub (pit_sharp, 16384) > 0)
    VoC_push32(REG01,DEFAULT);              // push32 stack32[n]  exc_enhanced/excp
    VoC_movreg16(REG3,REG0,DEFAULT);        //{
    VoC_loop_i_short(40,DEFAULT);           //   for (i = 0; i < L_SUBFR; i++)
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
    VoC_startloop0                  //   {
    VoC_add16inc_rp(REG6,REG6,REG1,DEFAULT);    //      excp[i] = add (excp[i], exc_enhanced[i]);
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0                //   }
    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);
    VoC_lw16_sd(REG2,1,DEFAULT);
    VoC_lw32_sd(RL7,1,IN_PARALLEL);
    VoC_jal(CII_agc);               //   agc2 (exc_enhanced, excp, L_SUBFR);
    VoC_pop32(REG23,DEFAULT);           //  pop32 stack32[n]  exc_enhanced/excp
    VoC_movreg16(REG3,REG2,DEFAULT);
Dec_amr_L69:

//  VoC_set_status(STATUS_OVF0,0);      //   Overflow = 0;
    VoC_lw16i(STATUS,OVF0_CLR);

    VoC_lw16_sd(REG5,4,DEFAULT);        // point to &a[1]???  Az in reg5
    VoC_lw16_sd(REG4,3,DEFAULT);        // i_subfr in reg4
    VoC_lw16_sd(REG7,8,DEFAULT);        // synth[0] in reg7
    VoC_add16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
    VoC_bnez16_d(Dec_amr_L69new,GLOBAL_EFR_FLAG_ADDR)
    VoC_lw16i_short(REG0,0,DEFAULT);     //except EFR mode
Dec_amr_L69new:

    VoC_lw16i_set_inc(REG1,STRUCT_DECOD_AMRSTATE_MEM_SYN_ADDR,1);

    VoC_push32(REG67,DEFAULT);
    VoC_jal(CII_Syn_filt);              //   Syn_filt (Az, excp, &synth[i_subfr], L_SUBFR,st->mem_syn, 0);
    //}
    //else
    //{
    //   Overflow = 0;
    //   Syn_filt (Az, exc_enhanced, &synth[i_subfr], L_SUBFR,st->mem_syn, 0);
    //}
    VoC_bnez16_d(Dec_amr_L76,GLOBAL_EFR_FLAG_ADDR)
    VoC_boe0z16_r(Dec_amr_L75);         //if (Overflow != 0)
    VoC_lw16i_set_inc(REG0,STRUCT_DECOD_AMRSTATE_OLD_EXC_ADDR,2);           //{
    VoC_lw16i(REG5,97);

    VoC_loop_i_short(2,DEFAULT);
    VoC_startloop0;
    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_loop_r(1,REG5);             //   for (i = 0; i < PIT_MAX + L_INTERPOL + L_SUBFR; i++)
    //   {
    VoC_shr16_ri(REG6,2,DEFAULT);       //      st->old_exc[i] = shr(st->old_exc[i], 2);
    VoC_shr16_ri(REG7,2,IN_PARALLEL);
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(REG67,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop1                //   }

    VoC_lw16i_set_inc(REG0,DEC_AMR_EXC_ENHANCED_ADDRESS,2);
    VoC_movreg16(REG3,REG0,DEFAULT);
    VoC_lw16i_short(REG5,20,IN_PARALLEL);

    VoC_endloop0;


    VoC_lw16_sd(REG5,4,DEFAULT);        // 原来接口指向&a[1]???  Az in reg5
    VoC_lw32_sd(REG67,0,DEFAULT);
    VoC_lw16i_set_inc(REG1,STRUCT_DECOD_AMRSTATE_MEM_SYN_ADDR,1);
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_jal(CII_Syn_filt);              //   Syn_filt(Az, exc_enhanced, &synth[i_subfr], L_SUBFR, st->mem_syn, 1);
    VoC_jump(Dec_amr_L76);              //}
Dec_amr_L75:                        //else{
    VoC_lw16i_set_inc(REG1,STRUCT_DECOD_AMRSTATE_MEM_SYN_ADDR,2);
    VoC_add16_rd(REG0,REG7,CONST_30_ADDR);
    VoC_jal(CII_Copy_M);            //   Copy(&synth[i_subfr+L_SUBFR-M], st->mem_syn, M);
Dec_amr_L76:                        //}
    VoC_pop32(RL6,DEFAULT);
    VoC_pop16(RA,DEFAULT);


    VoC_lw16i_set_inc(REG0,STRUCT_DECOD_AMRSTATE_OLD_EXC_ADDR+40,2);
    VoC_lw16i_set_inc(REG3,STRUCT_DECOD_AMRSTATE_OLD_EXC_ADDR,2);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_loop_i(0,77);
    //Copy (&st->old_exc[L_SUBFR], &st->old_exc[0], PIT_MAX + L_INTERPOL);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_lw16_d(REG4,DEC_AMR_T0_ADDRESS);        //st->old_T0 = T0;


    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16_sd(REG5,3,DEFAULT);            //Az += MP1;
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_add16_rd(REG6,REG5,CONST_11_ADDR);
    VoC_sw16_d(REG4,STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR);

    VoC_loop_i_short(3,DEFAULT);            //  pop16 stack[n-5]   exc_enhanced[]
    VoC_lw16i_short(REG0,40,IN_PARALLEL);
    VoC_startloop0
    VoC_pop16(REG5,DEFAULT);            // pop16 stack[n-3]
    VoC_endloop0

    VoC_sw16_sd(REG6,0,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG0,IN_PARALLEL);

    VoC_return;
}





/**************************************************************************
*  Function:   amr_dec_main
*  Create by Kenneth   09/01/2004
*  Revised by Kenneth  09/07/2004
**************************************************************************/



void CII_AMR_Decode(void)
{

#if 0
    voc_short DEC_AMR_LSP_NEW_ADDRESS                   ,10,x       //[10]shorts
    voc_short DEC_AMR_LSP_MID_ADDRESS                   ,10,x       //[10]shorts
    voc_short DEC_AMR_PREV_LSF_ADDRESS                  ,10,x       //[10]shorts
    voc_short DEC_AMR_LSF_I_ADDRESS                     ,50,x       //[50]shorts
    voc_short DEC_AMR_EXCP_ADDRESS                      ,40,x       //[40]shorts
    voc_short DEC_AMR_EXC_ENHANCED_ADDRESS              ,44,x        //[44]shorts
    voc_short DEC_AMR_T0_ADDRESS                           ,x        //1 short
    voc_short DEC_AMR_T0_FRAC_ADDRESS                   ,x       //1 short
    voc_short DEC_AMR_INDEX_ADDRESS                     ,x       //1 short
    voc_short DEC_AMR_INDEX_MR475_ADDRESS               ,x       //1 short
    voc_short DEC_AMR_GAIN_PIT_ADDRESS                  ,x       //1 short
    voc_short DEC_AMR_GAIN_CODE_ADDRESS                 ,x       //1 short
    voc_short DEC_AMR_GAIN_CODE_MIX_ADDRESS             ,x       //1 short
    voc_short DEC_AMR_PIT_SHARP_ADDRESS                 ,x       //1 short
    voc_short DEC_AMR_PIT_FLAG_ADDRESS                  ,x       //1 short
    voc_short DEC_AMR_PIT_FAC_ADDRESS                   ,x       //1 short
    voc_short DEC_AMR_T0_MIN_ADDRESS                    ,x       //1 short
    voc_short DEC_AMR_T0_MAX_ADDRESS                    ,x       //1 short
    voc_short DEC_AMR_DELTA_FRC_LOW_ADDRESS             ,x       //1 short
    voc_short DEC_AMR_DELTA_FRC_RANGE_ADDRESS           ,x       //1 short
    voc_short DEC_AMR_TEMP_SHIFT_ADDRESS                ,x       //1 short
    voc_short DEC_AMR_TEMP_ADDRESS                      ,x       //1 short
    voc_short DEC_AMR_L_TEMP_ADDRESS                    ,2,x         //[2]shorts
    voc_short DEC_AMR_FLAG4_ADDRESS                     ,x       //1 short
    voc_short DEC_AMR_CAREFULFLAG_ADDRESS               ,x       //1 short
    voc_short DEC_AMR_EXCENERGY_ADDRESS                 ,x       //1 short
    voc_short DEC_AMR_SUBFRNR_ADDRESS                   ,x       //1 short
    voc_short DEC_AMR_EVENSUBFR_ADDRESS                 ,x       //1 short
    voc_short DEC_AMR_NEWDTXSTATE_ADDRESS               ,x       //1 short
    voc_short DEC_AMR_BFI_ADDRESS                       ,x       //1 short
    voc_short DEC_AMR_PDFI_ADDRESS                      ,x       //1 short
    voc_short DEC_AMR_A_T_ADDRESS                       ,44,x        //[44]shorts
    voc_short DEC_AMR_PARM_ADDRESS                      ,57,x        //[57]shorts
#endif

#if 0

    voc_short    AMR_DEC_BUFOUT_ADDR     ,160,y

#endif

    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_blt16_rd(DEC_MODE_IS_AMR,REG7,CONST_0x177_ADDR);
    // clear tx_frame_type in case of EFR
    VoC_sw16_d(REG5,DEC_AMR_FRAME_TYPE_ADDR);
DEC_MODE_IS_AMR:


    VoC_bez16_r(amr_dec_main_L117,REG2)    //if (reset_flag != 0)

    VoC_sw16_d(REG2,DEC_MAIN_RESET_FLAG_OLD_ADDR);

    // reset decoder
    VoC_lw32z(RL6,DEFAULT);
    VoC_jal(CII_dec_reset);
    VoC_jal(CII_Post_Filter_reset);

    VoC_bez16_d(amr_dec_main_L117,GLOBAL_EFR_FLAG_ADDR);
    VoC_jal(CII_dtx_efr_dec_reset);
    VoC_jal(CII_reset_rx_dtx);

amr_dec_main_L117:

    VoC_lw16_d(REG6,STURCT_SPEECH_DECODE_FRAMESTATE_PREV_MODE_ADDR);

    VoC_lw16_d(REG7,GLOBAL_DEC_MODE_ADDR);

    VoC_lw16i_short(REG5,7,DEFAULT);
    VoC_bne16_rd(amr_dec_main_L1,REG5,DEC_AMR_FRAME_TYPE_ADDR); //if (rx_type == 7)
    VoC_movreg16(REG7,REG6,DEFAULT);            //{ mode = prev_mode; }
    VoC_jump(amr_dec_main_L2);
amr_dec_main_L1:                        //else
    VoC_movreg16(REG6,REG7,DEFAULT);           //{prev_mode = mode;}

amr_dec_main_L2:

    VoC_push16(REG7,DEFAULT);              // push16 mode
    VoC_sw16_d(REG6,STURCT_SPEECH_DECODE_FRAMESTATE_PREV_MODE_ADDR);
    VoC_sw16_d(REG7,GLOBAL_DEC_MODE_ADDR);

    VoC_jal(CII_bits2prm_amr_efr);//Bits2prm(mode, input_frame, param);

    VoC_lw16_d(REG3,DEC_MAIN_RESET_FLAG_OLD_ADDR);


    VoC_lw16i(REG6,STATIC_CONST_PRMNO_ADDR);   //{decoder_homing_frame_test(&serial[1], mode);}
    VoC_bne16_rd(amr_dec_main_L3,REG3,CONST_1_ADDR);   //if (reset_flag_old == 1)
    VoC_lw16i(REG6,STATIC_CONST_PRMNOFSF_ADDR); //{
amr_dec_main_L3:

// INLINE FUNCTION amr_dec_main_sr

    VoC_lw16_sd(REG7,0,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_lw16i_set_inc(REG2,STATIC_CONST_dhf_amr_ADDR,1);
    VoC_lw16_p(REG6,REG6,DEFAULT);// decoder_homing_frame_test(&serial[1], mode);}
    VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,DEC_AMR_PARM_ADDRESS,1);     //j = 0;
    VoC_lw16_p(REG2,REG2,DEFAULT);


    VoC_bez16_d(efr_label6,GLOBAL_EFR_FLAG_ADDR);            // EFR
    VoC_lw16i(REG2,STATIC_CONST_DHF_MASK_ADDR);         // EFR
efr_label6:                                                 // EFR


    VoC_loop_r_short(REG6,DEFAULT);         //for (i = 0; i < nparms; i++)
    VoC_startloop0              //{
    VoC_lw16inc_p(REG6,REG1,DEFAULT);   //    j = param[i] ^ dhf[mode][i];
    VoC_lw16inc_p(REG3,REG2,IN_PARALLEL);
    VoC_xor16_rr(REG6,REG3,DEFAULT);
    VoC_bnez16_r(Dhf_test_L2,REG6); //    if (j)
    VoC_endloop0            //}
Dhf_test_L2:

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_bnez16_r(Dhf_test_L3,REG6);
    VoC_lw16i_short(REG5,1,DEFAULT);
Dhf_test_L3:
    VoC_lw32_d(REG67,DEC_RESET_FLAG_ADDR);

// END OF INLINE FUNCTION amr_dec_main_sr


    VoC_bez16_r(amr_dec_main_L3B,REG7);     //if (reset_flag_old == 1)
    VoC_movreg16(REG6,REG5,DEFAULT);
    VoC_jump(amr_dec_main_L3A);
amr_dec_main_L3B:
    VoC_sw16_d(REG5,DEC_MAIN_RESET_FLAG_OLD_ADDR);
amr_dec_main_L3A:

//  VoC_sw32_d(REG67,DEC_RESET_FLAG_ADDR);
    VoC_push32(REG67,DEFAULT);//"RESET_FLAG"
    VoC_bez16_r(amr_dec_main_L4,REG6);      //if ((reset_flag != 0) && (reset_flag_old != 0))
    VoC_bez16_r(amr_dec_main_L4,REG7);      //{
    VoC_lw16i_set_inc(REG0,AMR_DEC_BUFOUT_ADDR,2);
    VoC_lw32_d(ACC0,CONST_0x00080008_ADDR);
    VoC_loop_i(0,80);               //    for (i = 0; i < 80; i++)
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);   //    {   RAM_X[((AMR_DEC_BUFOUT_ADDR-RAM_X_BEGIN_ADDR)/2)+i] = 0x00080008;
    VoC_endloop0                //    }
    VoC_jump(amr_dec_main_L5);          //}
amr_dec_main_L4:                    //else{

    VoC_jal(CII_Speech_Decode_Frame);       //Speech_Decode_Frame(speech_decoder_state, mode, &serial[1], rx_type, synth);}

amr_dec_main_L5:
    VoC_pop32(REG23,DEFAULT);
    VoC_bnez16_r(amr_dec_main_L6,REG3); //if (reset_flag_old == 0)
    VoC_lw16_d(REG2,DEC_MAIN_RESET_FLAG_OLD_ADDR);//{ decoder_homing_frame_test(&serial[1], mode);}
amr_dec_main_L6:

    VoC_pop16(REG7,DEFAULT);
    VoC_movreg16(REG3,REG2,IN_PARALLEL);    //  reset_flag_old = reset_flag;
    VoC_lw32z(RL6,DEFAULT);
    VoC_bnez16_d(amr_dec_main_L7,DEC_AMR_BFI_ADDRESS);
    VoC_sw32_d(REG23,DEC_RESET_FLAG_ADDR);
    VoC_bez16_r(amr_dec_main_L7,REG2);      //if (reset_flag != 0)
    VoC_jal(CII_dec_reset);
    VoC_jal(CII_Post_Filter_reset);
    // EFR DTX BEGIN
    VoC_bez16_d(amr_dec_main_L7,GLOBAL_EFR_FLAG_ADDR);
    VoC_jal(CII_reset_rx_dtx);
    // EFR DTX END
amr_dec_main_L7:
    VoC_lw16i_set_inc(REG0,AMR_DEC_BUFOUT_ADDR,2);
    VoC_lw16d_set_inc(REG1,DEC_OUTPUT_ADDR_ADDR,2);
    VoC_lw16i(REG6,40);
    VoC_jal(CII_copy_xy);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;

}

/*************************************************************************
*
*  Function:   CII_Speech_Decode_Frame
   Input:  enum Mode mode -> push16 stack[n+1]
           Word16 *serial -> push16 stack[n+2]
            frame_type    //
           Word16 *synth  //
*  Version 1.0 Create by Kenneth  07/27/2004
   Version 1.1 Optimized by Kenneth 09/01/2004
**************************************************************************/
void CII_Speech_Decode_Frame (void)
{


    VoC_push16(RA,DEFAULT);
    VoC_lw32_d(RL6,CONST_0x00008000L_ADDR);
    VoC_lw16_sd(REG7,1,DEFAULT);
    VoC_lw16_d(REG6,DEC_AMR_FRAME_TYPE_ADDR);
    VoC_push32(RL6,DEFAULT);            // push32  0x8000 in stack32
    VoC_be16_rd(SpDec_Fr_L1,REG6,CONST_5_ADDR); //if ((frame_type == RX_SID_BAD) ||
    VoC_bne16_rd(SpDec_Fr_L2,REG6,CONST_6_ADDR);//    (frame_type == RX_SID_UPDATE))
SpDec_Fr_L1:                    //     {
    VoC_lw16i_short(REG7,8,DEFAULT);        //  Bits2prm (MRDTX, serial, parm);

    VoC_jal(CII_bits2prm_amr_efr);
SpDec_Fr_L2:                    //} else { Bits2prm (mode, serial, parm);}


    VoC_lw16i_set_inc(REG0,AMR_DEC_BUFOUT_ADDR,1);
    VoC_movreg16(REG7,REG6,DEFAULT);
    VoC_push16(REG0,DEFAULT);

    VoC_jal(CII_Decoder_amr);       //Decoder_amr(st->decoder_amrState, mode, parm, frame_type,synth, Az_dec);

    VoC_lw16i_set_inc(REG2,DEC_AMR_A_T_ADDRESS,1);
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
    VoC_push16(REG2,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    /*
    **************************************************************************
    *  INLINE Function:  CII_Post_Filter
    *  Input   Post_FilterState *st //
               enum Mode mode      ->stack[n+4]       //
               Word16 *syn         ->REG0(incr=2)  push16 stack[n+2]
               Word16 *Az_4        ->REG2(incr=2)  push16 stack[n+1]
    *  Create by Kenneth 07/26/2004
       Optimized by Kenneth 09/01/2004
       reOptimized by Cui 10/11/2004
    ***************************************************************************/

    // *syn_work = &st->synth_buf[M];


    VoC_lw16i_set_inc(REG1,STRUCT_POST_FILTERSTATE_SYNTH_BUF_ADDR+10,2);
    VoC_jal(CII_copy_xy);//Copy (syn, syn_work , L_FRAME);

    VoC_lw16i_short(REG6,0,DEFAULT);    //Az = Az_4;


Post_Filter_L1:

    VoC_lw16_sd(REG6,3,DEFAULT);        //{
    exit_on_warnings=OFF;
    VoC_push16(REG6,DEFAULT);               // push16 stack[n-1]  i_subfr
    exit_on_warnings=ON;


    VoC_lw16i_short(REG0,10,DEFAULT);

    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,STATIC_CONST_F_GAMMA3_ADDR,1);   //      Weight_Ai (Az, gamma3_MR122, Ap3);
    VoC_sub16_rr(REG7,REG1,REG0,DEFAULT);       //      Weight_Ai (Az, gamma4_MR122, Ap4);

    VoC_bgt16_rd(Post_Filter_L4,REG6,CONST_5_ADDR);     //  if (sub(mode, MR122) == 0 || sub(mode, MR102) == 0)
    VoC_lw16i_set_inc(REG1,STATIC_CONST_gamma3_ADDR,1);        //      Weight_Ai (Az, gamma3, Ap3);
    // VoC_add16_rr(REG7,REG1,REG0,DEFAULT);            //      Weight_Ai (Az, gamma4, Ap4);  }
    VoC_lw16i(REG7,STATIC_CONST_F_GAMMA3_ADDR);
Post_Filter_L4:



    VoC_lw16i_set_inc(REG2,DEC_AMR_BUFA40_ADDRESS,1);    //AP3-> DEC_AMR_TEMP0_ADDRESS

    VoC_loop_i(1,2);
    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_jal(CII_Weight_Ai);
    //AP4-> DEC_AMR_TEMP11_ADDRESS
    VoC_movreg16(REG1,REG7,DEFAULT);
    VoC_endloop1;

    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);


    VoC_lw16_sd(REG6,0,DEFAULT);
    VoC_sw16_sd(REG0,1,DEFAULT);

    VoC_lw16i_set_inc(REG3,STRUCT_POST_FILTERSTATE_SYNTH_BUF_ADDR+10,1);
    VoC_lw16i_set_inc(REG2,STRUCT_POST_FILTERSTATE_RES2_ADDR,1);
    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);
    VoC_lw16i_set_inc(REG0,DEC_AMR_BUFA40_ADDRESS,1);
    VoC_push16(REG3,DEFAULT);               // push16 stack[n-2] ->&syn_work[i_subfr]
    VoC_jal(CII_Residu_new);                    //  Residu (Ap3, &syn_work[i_subfr], st->res2, L_SUBFR);

    // para for syn_filt
    VoC_lw16i_short(REG6,22,DEFAULT);

    VoC_lw16i_set_inc(REG3,DEC_AMR_BUFA40_ADDRESS,1);
    VoC_lw16i_set_inc(REG1,DEC_AMR_TEMP22_ADDRESS,1);
    VoC_movreg16(REG7,REG1,DEFAULT);

    VoC_add16_rd(REG2,REG1,CONST_11_ADDR);
    VoC_loop_i_short(11,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG3,DEFAULT);   //   Copy (Ap3, h, M + 1);
    VoC_sw16inc_p(REG0,REG2,DEFAULT);
    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_endloop0

    //   Set_zero (&h[M + 1], L_H - M - 1);
    VoC_movreg16(REG3,REG7,DEFAULT);
    VoC_movreg16(REG5,REG3,IN_PARALLEL);

    VoC_jal(CII_Syn_filt);          //   Syn_filt (Ap4, h, h, L_H, &h[M + 1], 0);



    VoC_movreg16(REG3,REG7,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_loop_i_short(22,DEFAULT);       //   for (i = 1; i < L_H; i++)
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG3,DEFAULT);       //   {
//      L_tmp0 = L_mac (L_tmp, h[i], h[i]);
    VoC_mac32_rr(REG6,REG6,DEFAULT);
//      L_tmp1 = L_mac (L_tmp, h[i], h[i]);
    VoC_mac32_rp(REG6,REG3,IN_PARALLEL);
    VoC_endloop0                //   }

//  save before The last mac of ACC1
    exit_on_warnings=OFF;
    VoC_movreg16(REG0,ACC1_HI,DEFAULT);     //   temp2 = extract_h (L_tmp);
    exit_on_warnings=ON;

    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_movreg16(REG1,ACC0_HI,IN_PARALLEL);


    VoC_bngtz16_r(Post_Filter_L6,REG0); //   if (temp2 <= 0)
    //   else
    //   {
    VoC_multf16_rd(REG0,REG0,CONST_26214_ADDR); //      temp2 = mult (temp2, MU);
    VoC_jal(CII_DIV_S);             //      temp2 = div_s (temp2, temp1);
    //   }
Post_Filter_L6:
    VoC_lw16i_set_inc(REG0,STRUCT_PREEMPHASISSTATE_ADDR,1);

    //   preemphasis (st->preemph_state, st->res2, temp2, L_SUBFR);
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /*
    *  Function:  CII_preemphasis
    *  Input:     preemphasisState *st  ->REG0(incr 1)
                  Word16 g          ->REG2
    *  Created by Kenneth   07/19/2004
    */
//  begin of CII_preemphasis
    VoC_lw16i_set_inc(REG1,STRUCT_POST_FILTERSTATE_RES2_ADDR+39,-1);    //p1 = signal + L - 1;
    VoC_lw16i_set_inc(REG3,STRUCT_POST_FILTERSTATE_RES2_ADDR+38,-1);    //p2 = p1 - 1;

    VoC_lw16_p(REG6,REG1,DEFAULT);      //temp = *p1;
    VoC_multf16inc_rp(REG5,REG2,REG3,DEFAULT);
    VoC_loop_i_short(39,DEFAULT);       //for (i = 0; i <= L - 2; i++)
    //    *p1 = sub (*p1, mult (g, *p2--));
    VoC_startloop0              //{
    //    *p1 = sub (*p1, mult (g, *p2--));
    VoC_sub16_pr(REG5,REG1,REG5,DEFAULT);   //    p1--;
    VoC_multf16inc_rp(REG5,REG2,REG3,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG1,DEFAULT);   //}
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_multf16_rp(REG5,REG2,REG0,DEFAULT); //*p1 = sub (*p1, mult (g, st->mem_pre));
    VoC_lw16_sd(REG7,3,DEFAULT);
    VoC_sub16_pr(REG5,REG1,REG5,DEFAULT);
    VoC_sw16_p(REG6,REG0,DEFAULT);      //st->mem_pre = temp;
    VoC_sw16_p(REG5,REG1,DEFAULT);
// end of CII_preemphasis
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    VoC_lw16_sd(REG0,1,DEFAULT);

    VoC_lw16i_short(REG6,40,IN_PARALLEL);
    VoC_lw16i(REG5,DEC_AMR_TEMP11_ADDRESS);
    VoC_lw16i_set_inc(REG3,STRUCT_POST_FILTERSTATE_RES2_ADDR,1);
    //REG7 for &syn[i_subfr]

    VoC_add16_rr(REG7,REG7,REG0,DEFAULT);
    VoC_lw16i_set_inc(REG1,STRUCT_POST_FILTERSTATE_MEM_SYN_PST_ADDR,1);
    VoC_lw16i_short(REG0,1,DEFAULT);


    VoC_jal(CII_Syn_filt);      //   Syn_filt (Ap4, st->res2, &syn[i_subfr], L_SUBFR, st->mem_syn_pst, 1);
    //REG0 for &syn[i_subfr]
    VoC_movreg16(REG0,REG7,DEFAULT);                // pop16 stack[n-3]
    VoC_pop16(REG2,DEFAULT);                // pop16 stack[n-2]
    VoC_lw16i_set_inc(REG1,STRUCT_AGCSTATE_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_push16(REG1,DEFAULT);               // push16 stack[n-2]
    VoC_lw32_sd(RL7,0,DEFAULT);
    VoC_jal(CII_agc);       //   agc (st->agc_state, &syn_work[i_subfr], &syn[i_subfr],AGC_FAC, L_SUBFR);
    VoC_pop16(REG1,DEFAULT);            // pop16 stack[n-2]

    VoC_pop16(REG6,DEFAULT);                // pop16 stack[n-1]
    VoC_add16_rd(REG6,REG6,CONST_40_ADDR);


    VoC_be16_rd(not_Post_Filter_L1,REG6,CONST_160_ADDR);    //for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
    VoC_jump(Post_Filter_L1);
not_Post_Filter_L1:

    VoC_lw16i_set_inc(REG1,STRUCT_POST_FILTERSTATE_SYNTH_BUF_ADDR,2);
    VoC_lw16i_set_inc(REG0,STRUCT_POST_FILTERSTATE_SYNTH_BUF_ADDR+160,2);
    VoC_jal(CII_Copy_M);    //Copy (&syn_work[L_FRAME - M], &syn_work[-M], M);

//  END of INLINE FUNCTION CII_Post_Filter

    VoC_pop16(REG2,DEFAULT);


    VoC_lw16i_set_inc(REG0,STRUCT_POST_PROCESSSTATE_Y2_LO_ADDR,2);
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    /************************************************************/
    VoC_bnez16_d(efr_label1,GLOBAL_EFR_FLAG_ADDR);   // EFR
    /************************************************************/
    /*************************************************************************
     *
     *  INLINE FUNCTION:  CII_Post_Process()
        Input:  Post_ProcessState *st-> REG0(incr 2)
                Word16 signal[]      -> REG1(incr 1)

     * Algorithm:
     *
     *  y[i] = b[0]*x[i]*2 + b[1]*x[i-1]*2 + b[2]*x[i-2]*2
     *                     + a[1]*y[i-1]   + a[2]*y[i-2];
     *
       Create by Kenneth 07/26/2004
         Optimized by Cui  10/11/2004
     *************************************************************************/
    VoC_jal(CII_Post_Process);

//  END INLINE FUNCTION CII_Post_Process

efr_label1:

    VoC_pop32(RL6,DEFAULT);

    VoC_pop16(REG0,DEFAULT);
    VoC_lw16i_short(REG2,0xfff8,IN_PARALLEL);


    VoC_movreg16(REG1, REG0,DEFAULT);

    VoC_lw16i_short(INC0,1,DEFAULT);    // EFR
    VoC_pop16(RA,DEFAULT);
    VoC_lw16inc_p(REG6,REG0,DEFAULT);            // EFR

    VoC_loop_i(0,160);                           // EFR

    VoC_bez16_d(efr_label2,GLOBAL_EFR_FLAG_ADDR);   // EFR
    VoC_shr16_ri(REG6,-1,DEFAULT);       // EFR
efr_label2:
    VoC_and16_rr(REG6,REG2,DEFAULT);
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    exit_on_warnings=ON;

    VoC_endloop0        //}

    VoC_return;
}


/****************************************************************************
*  Function    : CII_Decoder_amr
*  Input:  Decoder_amrState *st //    (direct address access, needn't pass the address as a parameter)
           enum Mode mode       ->push16 stack[n+3]
           Word16 parm[]    // DEC_AMR_PARM_ADDRESS  (direct address access, needn't pass the address as a parameter)
           frame_type       ->REG7
           Word16 synth[]       ->push16 stack[n+1]
           Word16 A_t[]     // DEC_AMR_A_T_ADDRESS  (direct address access, needn't pass the address as a parameter)

*  Output:
   Created by Kenneth  07/20/2004
Optimized by Kenneth  09/28/2004
reOptimized by Cui  10/14/2004
***************************************************************************/
void CII_Decoder_amr (void)
{
    VoC_push16(RA,DEFAULT);         // RA in stack[n]

    VoC_bez16_d(Label_rx_dtx_neglate,GLOBAL_EFR_FLAG_ADDR);
    VoC_jal(CII_rx_dtx);
    VoC_jump(Dec_amr_L4);
Label_rx_dtx_neglate:

//  EFR DTX END

    //newDTXState = rx_dtx_handler(st->dtxDecoderState, frame_type);
    VoC_jal(CII_rx_dtx_handler);

    VoC_lw32z(REG23,DEFAULT);
    VoC_sw16_d(REG4,DEC_AMR_NEWDTXSTATE_ADDRESS);
    VoC_sw16_d(REG2,DEC_AMR_INDEX_MR475_ADDRESS);
    // sw BFI & PDFI
    VoC_sw32_d(REG23,DEC_AMR_BFI_ADDRESS);

    VoC_bez16_r(Dec_amr_L1,REG4);       //if (sub(newDTXState, SPEECH) != 0 )
    VoC_lw16i_short(RL6_LO,1,DEFAULT);      //{
    VoC_jal(CII_dec_reset);             //   Decoder_amr_reset (st, MRDTX);
    VoC_jal(CII_dtx_dec);           //   dtx_dec(st->dtxDecoderState, st->mem_syn, st->lsfState, st->pred_state,st->Cb_gain_averState,
    //           newDTXState,mode, parm, synth, A_t);
    VoC_lw16i_set_inc(REG0,STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR,1);
    VoC_lw16i_set_inc(REG1,STRUCT_DECOD_AMRSTATE_LSP_OLD_ADDR,1);
    VoC_jal(CII_Lsf_lsp);               //   Lsf_lsp(st->lsfState->past_lsf_q, st->lsp_old, M);


    VoC_jump(Dec_amr_LEnd);             //   goto the_end;
    //}
Dec_amr_L1:


    VoC_bne16_rd(Dec_amr_L3,REG7,CONST_1_ADDR);     //else if (sub(frame_type, RX_SPEECH_DEGRADED) == 0)
    VoC_sw16_d(REG7,DEC_AMR_PDFI_ADDRESS);
Dec_amr_L3:
    VoC_be16_rd(Dec_amr_L2,REG7,CONST_3_ADDR);      //if ((sub(frame_type, RX_SPEECH_BAD) == 0)
    VoC_be16_rd(Dec_amr_L2,REG7,CONST_7_ADDR);      //    || (sub(frame_type, RX_NO_DATA) == 0)
    VoC_be16_rd(Dec_amr_L2,REG7,CONST_2_ADDR);      //    ||(sub(frame_type, RX_ONSET) == 0))
    VoC_jump(Dec_amr_L4);
Dec_amr_L2:                         //{
    VoC_lw16i_short(REG6,1,DEFAULT);            //   bfi = 1;
    VoC_lw16_sd(REG5,3,DEFAULT);            // mode in reg5
    VoC_sw16_d(REG6,DEC_AMR_BFI_ADDRESS);

    //   if ((sub(frame_type, RX_NO_DATA) == 0) //||(sub(frame_type, RX_ONSET) == 0))
    VoC_bne16_rd(Dec_amr_L4_go,REG7,CONST_3_ADDR);
    VoC_jump(Dec_amr_L4);
Dec_amr_L4_go:
    //   {
    VoC_lw16_d(REG4,STRUCT_DECOD_AMRSTATE_NODATASEED_ADDR);
    VoC_lw16i_set_inc(REG0,STATIC_CONST_PRMNO_ADDR,1);
    VoC_lw16i_set_inc(REG1,STATIC_CONST_BITNO_AMR_ADDR,1); // ?
    VoC_add16_rr(REG7,REG0,REG5,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG5,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0,DEC_AMR_PARM_ADDRESS,1); //    build_CN_param(&st->nodataSeed, prmno[mode], bitno[mode], parm);
    VoC_lw16_p(REG7,REG7,DEFAULT);
    VoC_lw16_p(REG1,REG1,DEFAULT);          //   }
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /*-------------------------------------------------------------------------------
      Function: void CII_build_CN_param (void)
      Input: REG0(incr 1)->parm[]
             REG4->*seed
             REG7-> n_param
             REG1(incr 1)-> param_size_table[]
      Output:REG0->parm[]
             STRUCT_DECOD_AMRSTATE_NODATASEED_ADDR->*seed
    ---------------------------------------------------------------------------------*/
    //  begin of build_CN_param
    VoC_multf32_rd(ACC0,REG4,CONST_31821_ADDR); //*seed = extract_l(L_add(L_shr(L_mult(*seed, 31821), 1), 13849L));
    VoC_lw16i(REG2,13849);
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_add32_rr(REG45,ACC0,REG23,DEFAULT);
    VoC_lw16i(REG6,0x7F);
    VoC_sw16_d(REG4,STRUCT_DECOD_AMRSTATE_NODATASEED_ADDR);
    VoC_lw16i_set_inc(REG3,STATIC_CONST_WIND_200_40_ADDR,1);
    VoC_and16_rr(REG6,REG4,DEFAULT);                 //p = &window_200_40[*seed & 0x7F];
    VoC_lw16i_short(REG2,0,IN_PARALLEL);
    VoC_add16_rr(REG3,REG6,REG3,DEFAULT);
    VoC_loop_r_short(REG7,DEFAULT)
    VoC_lw16i_short(REG5,-1,IN_PARALLEL);
    VoC_startloop0                   //for(i=0; i< n_param;i++){
    VoC_lw16inc_p(REG7,REG3,DEFAULT);
    VoC_sub16inc_rp(REG6,REG2,REG1,DEFAULT);
    VoC_shr16_rr(REG5,REG6,DEFAULT);
    VoC_not16_r(REG5,DEFAULT);
    VoC_and16_rr(REG7,REG5,DEFAULT);         //  parm[i] = *p++ & ~(0xFFFF<<param_size_table[i]);
    VoC_lw16i_short(REG5,-1,DEFAULT);
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    VoC_endloop0             //}
    // end of build_CN_param
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    //}
    //{    pdfi = 1;     }
Dec_amr_L4:



    VoC_lw16_d(REG4,STRUCT_DECOD_AMRSTATE_STATE_ADDR);
    VoC_add16_rd(REG3,REG4,CONST_1_ADDR);               //{    st->state = add (st->state, 1);
    VoC_bnez16_d(Dec_amr_L7,DEC_AMR_BFI_ADDRESS);   //if (bfi != 0)
    VoC_lw16i_short(REG3,5,DEFAULT);            //{     st->state = 5;
    VoC_be16_rd(Dec_amr_L7,REG4,CONST_6_ADDR);      //else if (sub (st->state, 6) == 0)
    //else
    VoC_lw16i_short(REG3,0,DEFAULT);          //{     st->state = 0;  }
Dec_amr_L7:

    VoC_bngt16_rd(Dec_amr_L9,REG3,CONST_6_ADDR);    //if (sub (st->state, 6) > 0)
    VoC_lw16i_short(REG3,6,DEFAULT);            //{    st->state = 6; }
Dec_amr_L9:




//  EFR DTX BEGIN
    VoC_bez16_d(Dec_amr_L9_EFR, GLOBAL_EFR_FLAG_ADDR);
    VoC_jal(CII_efr_dtx_func9);
    VoC_jump(Dec_amr_L11_EFR);
Dec_amr_L9_EFR:
//  EFR DTX END



    VoC_lw16_d(REG4,STRUCT_DTX_DECSTATE_DTXGLOBALSTATE_ADDR);
    VoC_lw16_d(REG5,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);

    VoC_bne16_rd(Dec_amr_L11_1,REG4,CONST_1_ADDR);  //if (sub(st->dtxDecoderState->dtxGlobalState, DTX) == 0)
    //{  st->state = 5;
    VoC_lw16i_short(REG5,0,DEFAULT);        //   st->prev_bf = 0;
    VoC_lw16i_short(REG3,5,DEFAULT);
Dec_amr_L11_1:
    VoC_bne16_rd(Dec_amr_L11,REG4,CONST_2_ADDR);    //else if (sub(st->dtxDecoderState->dtxGlobalState, DTX_MUTE) == 0)

    VoC_lw16i_short(REG5,1,DEFAULT);        //    st->prev_bf = 1;
    VoC_lw16i_short(REG3,5,DEFAULT);                    //}
Dec_amr_L11:

    VoC_sw16_d(REG5,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);
Dec_amr_L11_EFR:
    VoC_sw16_d(REG3,STRUCT_DECOD_AMRSTATE_STATE_ADDR);


    VoC_lw16i_set_inc(REG0,STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR,2);
    VoC_lw16i_set_inc(REG1,DEC_AMR_PREV_LSF_ADDRESS,2);
    VoC_jal(CII_Copy_M);                //Copy (st->lsfState->past_lsf_q, prev_lsf, M);

    VoC_lw16i_set_inc(REG0,STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR,1);
    VoC_lw16i_set_inc(REG1,DEC_AMR_PARM_ADDRESS,1);
    VoC_lw16_sd(REG7,3,DEFAULT);            // mode in reg7
    VoC_lw16_d(REG6,DEC_AMR_BFI_ADDRESS);
    VoC_lw16i(REG2,STRUCT_DECOD_AMRSTATE_LSP_OLD_ADDR);
    VoC_lw16i(REG3,DEC_AMR_LSP_NEW_ADDRESS);
    VoC_lw16i(REG4,DEC_AMR_LSP_MID_ADDRESS);
    VoC_lw16i(REG5,DEC_AMR_A_T_ADDRESS);
    VoC_push32(REG23,DEFAULT);              // push32 stack32[n]  &lsp_new[]/&lsp_old[]
    VoC_push32(REG45,DEFAULT);              // push32 stack32[n-1]  &A_t[]/&lsp_mid[]

// VoC_directive: PARSER_OFF

    VoC_bnez16_r(Dec_amr_La,REG7);
    VoC_jal(CII_amr_dec_475);
    VoC_jump(Dec_amr_end);
Dec_amr_La:
    VoC_bne16_rd(Dec_amr_Lb,REG7,CONST_1_ADDR);
    VoC_jal(CII_amr_dec_515);
    VoC_jump(Dec_amr_end);
Dec_amr_Lb:
    VoC_bne16_rd(Dec_amr_Lc,REG7,CONST_2_ADDR);
    VoC_jal(CII_amr_dec_59);
    VoC_jump(Dec_amr_end);
Dec_amr_Lc:
    VoC_bne16_rd(Dec_amr_Ld,REG7,CONST_3_ADDR);
    VoC_jal(CII_amr_dec_67);
    VoC_jump(Dec_amr_end);
Dec_amr_Ld:
    VoC_bne16_rd(Dec_amr_Le,REG7,CONST_4_ADDR);
    VoC_jal(CII_amr_dec_74);
    VoC_jump(Dec_amr_end);
Dec_amr_Le:
    VoC_bne16_rd(Dec_amr_Lf,REG7,CONST_5_ADDR);
    VoC_jal(CII_amr_dec_795);
    VoC_jump(Dec_amr_end);
Dec_amr_Lf:
    VoC_bne16_rd(Dec_amr_Lg,REG7,CONST_6_ADDR);
    VoC_jal(CII_amr_dec_102);
    VoC_jump(Dec_amr_end);
Dec_amr_Lg:

// VoC_directive: PARSER_ON

    VoC_jal(CII_amr_dec_122);
Dec_amr_end:

    VoC_bnez16_d(Dec_amr_LEnd_fake,GLOBAL_EFR_FLAG_ADDR);
//spy_on_addr = OFF;
    VoC_jal(CII_AMR_subfunc0);
//spy_on_addr = ON;

Dec_amr_LEnd_fake:

//   end  of  dtx_dec_activity_update
//*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    //st->prev_bf = bfi;
    //st->prev_pdf = pdfi;
    VoC_lw32_d(REG67,DEC_AMR_BFI_ADDRESS);
    VoC_NOP();
    VoC_sw32_d(REG67,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);
Dec_amr_LEnd:
    VoC_pop16(RA,DEFAULT);              // pop16 stack[n]
    VoC_lw16i_set_inc(REG0,STRUCT_LSP_AVGSTATE_LSP_MEANSAVE_ADDR,1); //   lsp_avg(st->lsp_avg_st, st->lsfState->past_lsf_q);
    VoC_lw16i_set_inc(REG1,STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR,1);

    // begin of CII_lsp_avg
    /*******************************************
    input: *st    ->REG0 (incr 1)
          *lsp    ->REG1 (incr 1)
    *************************************/
    VoC_lw16i(REG5,5243);
    VoC_loop_i_short(10,DEFAULT);           //for (i = 0; i < M; i++) {
    VoC_lw32_sd(ACC0,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16_p(ACC0_HI,REG0,DEFAULT);       //   L_tmp = L_deposit_h(st->lsp_meanSave[i]);
    //   L_tmp = L_msu(L_tmp, EXPCONST, st->lsp_meanSave[i]);
    VoC_msu32_rp(REG5,REG0,DEFAULT);
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);         //   L_tmp = L_mac(L_tmp, EXPCONST, lsp[i]);
    VoC_lw16_d(REG4,DEC_AMR_NEWDTXSTATE_ADDRESS);   //st->dtxDecoderState->dtxGlobalState = newDTXState;
    VoC_lw32_sd(ACC0,0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);    //   st->lsp_meanSave[i] = round(L_tmp);
    exit_on_warnings=ON;
    VoC_endloop0                    //}
    // end of CII_lsp_avg

    VoC_sw16_d(REG4,STRUCT_DTX_DECSTATE_DTXGLOBALSTATE_ADDR);
    VoC_return;
}


/***************************************************************************
*  Function    : dec_reset
*  Input: mode != MRDTX->RL6 =0
          mode == MRDTX->RL6!=0
*  Created by Kenneth    08/30/2004
**************************************************************************/
void CII_dec_reset(void)
{

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16i_short(REG7,19,DEFAULT);
    VoC_lw16i_short(REG5,27,IN_PARALLEL);
    VoC_bez32_r(dec_reset_L0,RL6);
    VoC_lw16i_short(REG7,11,DEFAULT);
    VoC_lw16i_short(REG5,7,IN_PARALLEL);
dec_reset_L0:
    VoC_lw16i_set_inc(REG1,0,1);
    VoC_lw16i(REG6,AMR_EFR_RESET_Offset_X);
    VoC_lw16i_set_inc(REG0,AMR_DEC_RESET0_TABLE_ADDR,2);
    VoC_loop_r(1,REG7)
    VoC_lw32inc_p(REG23,REG0,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);
    VoC_NOP();
    VoC_loop_r_short(REG3,DEFAULT)
//spy_on_addr=OFF;
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_endloop0
//spy_on_addr=ON;
    VoC_bez32_r(dec_reset_L2,RL6);
    VoC_bnez16_r(dec_reset_L1,REG1);
    VoC_inc_p(REG0,DEFAULT);
dec_reset_L1:
    VoC_bne16_rd(dec_reset_L2,REG1,CONST_3_ADDR);
    VoC_inc_p(REG0,DEFAULT);
dec_reset_L2:
    VoC_inc_p(REG1,DEFAULT);
    VoC_endloop1



    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_set_inc(REG0,AMR_DEC_RESET_TABLE_ADDR,3); // reset address
    VoC_lw16i_set_inc(REG1,AMR_DEC_RESET_TABLE_ADDR+1,3);// reset long
    VoC_lw16i(REG4,AMR_DEC_RESET_TABLE_ADDR+2);// reset value
    VoC_lw32z(RL7,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_loop_r(1,REG5)
    VoC_lw16i(REG5,AMR_EFR_RESET_Offset_X);
    VoC_lw16inc_p(REG2,REG0,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG5,DEFAULT);
    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_lw16_p(REG3,REG4,DEFAULT);
    VoC_add16_rd(REG4,REG4,CONST_3_ADDR);
    VoC_lw16inc_p(REG7,REG3,DEFAULT);
    VoC_loop_r_short(REG5,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG3,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_bez32_r(dec_reset_L3,RL6);
    VoC_bgt16_rd(dec_reset_L3,REG6,CONST_1_ADDR);
    VoC_inc_p(REG0,DEFAULT);
    VoC_inc_p(REG1,DEFAULT);
    VoC_add16_rd(REG4,REG4,CONST_3_ADDR);
dec_reset_L3:
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);
    VoC_endloop1
    VoC_bnez32_r(dec_reset_L4,RL6);
    VoC_sw32_d(RL7,STRUCT_DTX_DECSTATE_LOG_EN_HIST_PTR_ADDR);
dec_reset_L4:
    VoC_lw16i(REG6,21845);
    VoC_sw32_d(RL7,STRUCT_PH_DISPSTATE_PREVSTATE_ADDR);  //state->prevState = 0;state->prevCbGain = 0;
    VoC_sw32_d(RL7,STRUCT_PH_DISPSTATE_LOCKFULL_ADDR);   //state->lockFull = 0;state->onset = 0;
    VoC_sw16_d(REG6,STRUCT_DECOD_AMRSTATE_NODATASEED_ADDR);
    VoC_return;
}


/*************************************************************************
*  Function:   CII_Post_Filter_reset
*  Create by Kenneth  07/26/2004
**************************************************************************/

void CII_Post_Filter_reset (void)
{
    VoC_lw32z(ACC1,DEFAULT);
    VoC_lw16i_set_inc(REG0,STRUCT_POST_FILTERSTATE_RES2_ADDR,2);
    VoC_loop_i(0,114);                      //Set_zero (state->res2, L_SUBFR);
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);   //Set_zero (state->mem_syn_pst, M);
    VoC_endloop0                            //Set_zero (state->synth_buf, L_FRAME + M);

    VoC_lw16i(ACC1_HI,4096);    //agc_reset(state->agc_state);  ->( state->past_gain = 4096; )
    VoC_sw16_d(ACC1_LO,STURCT_SPEECH_DECODE_FRAMESTATE_PREV_MODE_ADDR);
    VoC_sw32_d(ACC1,STRUCT_PREEMPHASISSTATE_ADDR);
    VoC_return;
}

void CII_energy_new(void)
/*    input:  REG0(incr 1)->Word16 in[],
      output: ACC0
      used registers: REG0,REG7,ACC0                */

{
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw32z(ACC1,DEFAULT);
    VoC_loop_i_short(40,DEFAULT);
    VoC_lw16_p(REG7,REG0,IN_PARALLEL);
    VoC_startloop0;
    VoC_shr16_ri(REG7,2,DEFAULT);
    VoC_mac32inc_pp(REG0,REG0,IN_PARALLEL);
    VoC_mac32_rr(REG7,REG7,DEFAULT);
    VoC_lw16_p(REG7,REG0,IN_PARALLEL);
    VoC_endloop0;
    VoC_be32_rd(Energy_L0,ACC1,CONST_0x7FFFFFFF_ADDR);

    VoC_bnez16_d(Energy_L0,GLOBAL_EFR_FLAG_ADDR);       // EFR

    VoC_shr32_ri(ACC1,4,DEFAULT);
    VoC_movreg32(ACC0,ACC1,DEFAULT);
Energy_L0:
    VoC_return;
}

/***************************************************************************
*  Function    : CII_agc
*  Input: REG4:  agc->0;
                 agc2->1;
      push the address of st->past_gain before call this function
      Word16 *sig_in,         i   : REG2(incr 1)->
      Word16 *sig_out,        i/o : REG0(incr 1)->
      Word16 l_trm            i   : L_SUBFR
      RL7-> 0x8000
*  Created by Kenneth    06/20/2004
   Version 1.1 optimized by Fei
**************************************************************************/
void CII_agc(void)
{
    VoC_push16(RA,DEFAULT);              //s = energy_new(sig_out, l_trm);
    VoC_push16(REG0,DEFAULT);
    VoC_jal(CII_energy_new);
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_bez32_r(AGC_LEnd,ACC0);
    VoC_jal(CII_NORM_L_ACC0);
    VoC_movreg16(REG5,REG1,DEFAULT);
    VoC_shr32_ri(ACC0,1,IN_PARALLEL);
    VoC_add32_rr(RL6,ACC0,RL7,DEFAULT);
    VoC_movreg16(REG0,REG2,IN_PARALLEL);
    VoC_jal(CII_energy_new);
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_bez32_r(AGC_LA5, ACC0);
    VoC_jal(CII_NORM_L_ACC0);
    VoC_add32_rr(REG01,ACC0,RL7,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG1,IN_PARALLEL);
    VoC_movreg16(REG0,RL6_HI,DEFAULT);
    VoC_jal(CII_DIV_S);
    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_movreg16(REG0,REG2,IN_PARALLEL);
    VoC_shr32_ri(REG01, -8, DEFAULT);            //    s = L_shl (s, 7);
    VoC_shr32_rr(REG01, REG5,DEFAULT);           //    s = L_shr (s, exp);
    VoC_jal(CII_Inv_sqrt);               //    s = Inv_sqrt (s);
    VoC_shr32_ri(REG01,-9,DEFAULT);          //    g0 = round (L_shl (s, 9));
    VoC_add32_rd(REG23,REG01,CONST_0x00008000L_ADDR);
AGC_LA5:
    VoC_movreg16(REG5,REG3,DEFAULT);
    VoC_bnez16_r(AGC_LA4,REG4);              //agc :g0 = mult (go, sub (32767, agc_fac));
    VoC_lw16_sd(REG7,2,DEFAULT);             //agc: gain = st->past_gain;
    VoC_multf16_rd(REG3,REG3,CONST_3276_ADDR);       //}
    VoC_lw16_p(REG5,REG7,DEFAULT);
AGC_LA4:
    VoC_lw16_sd(REG0,0, DEFAULT);
// opti: two branch are deleted here by loading default values
    VoC_loop_i_short(40,DEFAULT);            //for (i = 0; i < l_trm; i++)
    VoC_startloop0                   //{
    VoC_bnez16_r(AGC_LA6,REG4);
    VoC_multf16_rd(REG5,REG5,CONST_29491_ADDR); //agc: gain = mult (gain,agc_fac);
    VoC_NOP();
    VoC_add16_rr(REG5,REG5,REG3,DEFAULT);       //agc: gain = add (gain, g0);
AGC_LA6:
    VoC_multf32_rp(ACC0, REG5, REG0, DEFAULT);  // sig_out[i] = extract_h(L_shl (L_mult (sig_out[i], g0), 3));
    VoC_NOP();
    VoC_shr32_ri(ACC0, (-3), DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(ACC0_HI, REG0, DEFAULT);

    VoC_endloop0;                    //}
AGC_LEnd:                       //agc: st->past_gain = gain;
    VoC_pop16(REG0,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_lw16_sd(REG6,0,DEFAULT);
    VoC_bnez16_r(AGC_LA7,REG4);
    VoC_sw16_p(REG5,REG6,DEFAULT);
AGC_LA7:
    VoC_return;
}

/***************************************************************************
*
*  Function    : CII_Bits2prm
*  Input: enum Mode mode ->REG7
      Word16 bits[]  ->REG0(incr 1)
      Word16 prm[]   ->REG1(incr 1)
    Optimized by Cui  10/13/2004
***************************************************************************/
void CII_bits2prm_amr_efr (void)
{



#if 0
    voc_short   AMR_DEC_BUFIN_ADDR                ,246  ,x
#endif


// number of word32 depending on mode
    VoC_lw16i_short(REG5,8,DEFAULT);
    VoC_be16_rd(word32_needed,REG7,CONST_7_ADDR)
    VoC_lw16i_short(REG5,7,DEFAULT);
    VoC_be16_rd(word32_needed,REG7,CONST_6_ADDR)
    VoC_lw16i_short(REG5,5,DEFAULT);
    VoC_bgt16_rd(word32_needed,REG7,CONST_2_ADDR)
    VoC_lw16i_short(REG5,4,DEFAULT);
    VoC_bgt16_rd(word32_needed,REG7,CONST_0_ADDR)
    VoC_lw16i_short(REG5,3,DEFAULT);

word32_needed:



    VoC_lw16i_set_inc(REG0, INPUT_BITS2_ADDR,2);
    VoC_add16_rd(REG0,REG0,CONST_8_ADDR);


    VoC_lw16i_set_inc(REG1,AMR_DEC_BUFIN_ADDR,1);
    VoC_loop_r(1,REG5)
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_loop_i_short(32,DEFAULT)

    VoC_startloop0

    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_and16_rr(REG2,REG4,DEFAULT);

    VoC_shr32_ri(REG45,1,DEFAULT);
    VoC_sw16inc_p(REG2,REG1,DEFAULT);
    VoC_endloop0
    VoC_lw16i_set_inc(REG3,DEC_AMR_PARM_ADDRESS,1);
    VoC_endloop1

    VoC_lw16i(REG5,STATIC_CONST_PRMNO_ADDR);
    VoC_lw16i_set_inc(REG2,STATIC_CONST_BITNO_AMR_ADDR,1);

    VoC_add16_rr(REG5,REG7,REG5,DEFAULT);
    VoC_add16_rr(REG2,REG7,REG2,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,AMR_DEC_BUFIN_ADDR,1);

    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_lw16_p(REG2,REG2,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);


    VoC_loop_r(1,REG5)
    VoC_lw16_p(REG4,REG2,DEFAULT);  // REG4 for loop number
    VoC_and16_ri(REG4,0xff);
    VoC_and16_ri(REG7,0x1);
    VoC_bez16_r(BITNO_AMR_even,REG7)
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_shru16_ri(REG4,8,DEFAULT);

BITNO_AMR_even:

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_startloop0
    VoC_add16inc_rp(REG5,REG5,REG1,DEFAULT);
    VoC_shr16_ri(REG5,-1,DEFAULT);
    VoC_endloop0
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);
    // }
    //     bits += bitno[mode][i];      add(0,0);
    VoC_endloop1                 // }
    VoC_return;
}


/********************************************************************************
  Function:  void CII_R02R3_p_R6(void)
   input: REG6;
   Description: write the value of the memory that REG0 points t0 the memory that REG3 points,the number is in REG6
   Used register: REG5,REG6,REG0,REG3
  Created by Kenneth  07/7/2004
 ********************************************************************************/
void CII_R02R3_p_R6(void)
{
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    VoC_loop_r_short(REG6,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_return;
}

/*************************************************************************
  Function: CII_ec_gain_code
  Input: ec_gain_codeState *st,    i/o : REG0(incr 1) push16 stack[n]
         gc_predState *pred_state, i/o : REG1(incr 1) push16 stack[n-1]
         Word16 state,             i   : push16 stack[n-2]
         Word16 *gain_code         o   : DEC_AMR_GAIN_CODE_ADDRESS //(direct address access, needn't pass the address as a parameter)
  Created by Kenneth  07/13/2004
*************************************************************************/
void CII_ec_gain_code (void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG5,5,DEFAULT);
    VoC_jal(CII_gmed_n);                 //tmp = gmed_n (st->gbuf,5);
    VoC_lw16_sd(REG4,3,DEFAULT);
    VoC_add16_rd(REG4,REG4,CONST_6_ADDR);
    VoC_lw16i(REG5,STATIC_CONST_CDOWN_ADDR);
    VoC_lw16_p(REG4,REG4,DEFAULT);
    VoC_bngt16_rr(Ecgains_L12,REG1,REG4);            //if (sub (tmp, st->past_gain_code) > 0)
    VoC_movreg16(REG1,REG4,DEFAULT);          //{ tmp = st->past_gain_code;  }
Ecgains_L12:
    VoC_lw16_sd(REG4,1,DEFAULT);             //tmp = mult (tmp, cdown[state]);
    VoC_lw16_sd(REG0,2,DEFAULT);
    VoC_add16_rr(REG4,REG5,REG4,DEFAULT);

    VoC_multf16_rp(REG1,REG1,REG4,DEFAULT);
    VoC_lw32z(REG67,DEFAULT);                //av_pred_en = 0;
    VoC_add16_rd(REG1,REG0,CONST_4_ADDR);
    exit_on_warnings = OFF;
    VoC_sw16_d(REG1,DEC_AMR_GAIN_CODE_ADDRESS);      //*gain_code = tmp;
    exit_on_warnings = ON;
//   CII_gc_pred_average_limited        //gc_pred_average_limited(pred_state, &qua_ener_MR122, &qua_ener);

    /*************************************************************************
     *
     * FUNCTION:  CII_gc_pred_average_limited()
       Input:  gc_predState *st,    REG0(incr 1) pushed
       Output: *ener_avg_MR122,     REG7
               *ener_avg            REG6
       REQUIRED: REG1 inc 1
       Created by  Kenneth   07/14/2004
    begin of    CII_gc_pred_average_limited

     *************************************************************************/

    VoC_loop_i_short(4,DEFAULT);            //for (i = 0; i < NPRED; i++)
    VoC_startloop0                  //{
    VoC_add16inc_rp(REG7,REG7,REG1,DEFAULT);    //    av_pred_en = add (av_pred_en, st->past_qua_en_MR122[i]);
    VoC_add16inc_rp(REG6,REG6,REG0,DEFAULT);
    VoC_endloop0                    //}

    VoC_shr16_ri(REG7,2,DEFAULT);                //av_pred_en = mult (av_pred_en, 8192);
    VoC_shr16_ri(REG6,2,IN_PARALLEL);      //av_pred_en = mult (av_pred_en, 8192);

    VoC_lw16i(REG5,-2381);
    VoC_bngt16_rr(Gcpred_L1,REG5,REG7);         //if (sub (av_pred_en, MIN_ENERGY_MR122) < 0)
    VoC_movreg16(REG7,REG5,DEFAULT);            //{  av_pred_en = MIN_ENERGY_MR122; }
Gcpred_L1:                          //av_pred_en = 0;

// EFR DTX BEGIN

    VoC_bnez16_d(Gcpred_L2,GLOBAL_EFR_FLAG_ADDR);

// EFR DTX END

    //for (i = 0; i < NPRED; i++)
    //{ av_pred_en = add (av_pred_en, st->past_qua_en[i]);}
    VoC_lw16i(REG5,-14336);
    VoC_bngt16_rr(Gcpred_L2,REG5,REG6);         //if (sub (av_pred_en, MIN_ENERGY) < 0)
    VoC_movreg16(REG6,REG5,DEFAULT);            //{av_pred_en = MIN_ENERGY;
Gcpred_L2:                          //}         //*ener_avg = av_pred_en;

    /* end of    CII_gc_pred_average_limited
     *************************************************************************/

    VoC_lw16i(REG5,STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR);
    VoC_jal(CII_gc_pred_update);             //gc_pred_update(pred_state, qua_ener_MR122, qua_ener);

    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_return;
}

/*************************************************************************
*  Function    : CII_ec_gain_code_update
*  Input:  ec_gain_codeState *st ->  REG0(incr 1) push16 stack[n]
           Word16 bfi,           ->  DEC_AMR_BFI_ADDRESS                //(direct address access,
           Word16 prev_bf,       ->  STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR //(direct address access,
           Word16 *gain_code     ->  REG6(DEC_AMR_GAIN_CODE_ADDRESS)
   Output: *gain_code ->REG6(DEC_AMR_GAIN_CODE_ADDRESS)
   REQUIRED: REG1 incr =1
*  Created by Kenneth  07/13/2004
**************************************************************************/
void CII_ec_gain_code_update (void)
{
    VoC_lw16_sd(REG0,0,DEFAULT);

    VoC_add16_rd(REG2,REG0,CONST_7_ADDR);

    VoC_bnez16_d(Ecgains_L10,DEC_AMR_BFI_ADDRESS);       //if (bfi == 0){

// EFR DTX BEGIN
    VoC_bnez16_d(Ecgains_L11,GLOBAL_EFR_FLAG_ADDR);
// EFR DTX END


    VoC_bez16_d(Ecgains_L11,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR); // if (prev_bf != 0){
    VoC_lw16_p(REG7,REG2,DEFAULT);
    VoC_bngt16_rr(Ecgains_L11,REG6,REG7);    // if (sub (*gain_code, st->prev_gc) > 0)
    VoC_movreg16(REG6,REG7,DEFAULT);         //  {  *gain_code = st->prev_gc;
    VoC_sw16_d(REG7,DEC_AMR_GAIN_CODE_ADDRESS);  //  }
Ecgains_L11:                    //   }


    VoC_sw16_p(REG6,REG2,DEFAULT);       // st->prev_gc = *gain_code;
Ecgains_L10:                                //}

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_sub16_rr(REG2, REG2, REG7,DEFAULT);
    VoC_add16_rr(REG1,REG0, REG7,DEFAULT);

    VoC_sw16_p(REG6,REG2,DEFAULT);       // st->past_gain_code = *gain_code;
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_loop_i_short(4,DEFAULT);         //for (i = 1; i < 5; i++)
    VoC_lw16inc_p(REG7,REG1,IN_PARALLEL);
    VoC_startloop0               //{
    VoC_lw16inc_p(REG7,REG1,DEFAULT);   //    st->gbuf[i - 1] = st->gbuf[i];
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0                 //}
    VoC_sw16_p(REG6,REG0,DEFAULT);               //st->gbuf[4] = *gain_code;
    VoC_return;
}

/*************************************************************************
*  Function    : CII_ec_gain_pitch
*  Input:  ec_gain_pitchState *st i/o : REG0(incr 1) push16();
           Word16 state,          i   :              push16()
           Word16 *gain_pitch     o   : REG6(DEC_AMR_GAIN_PIT_ADDRESS)
   REQUIRED: REG1 incr =1
*  Created by Kenneth  07/13/2004
**************************************************************************/
void CII_ec_gain_pitch (void)
{
// EFR DTX BEGIN
    VoC_bez16_d(ec_gain_pitch_EFR,GLOBAL_EFR_FLAG_ADDR);
    //VoC_lw16_d(REG5,GLOBAL_RXDTX_CTRL_ADDR);
    //VoC_and16_ri(REG5,1);
    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_and16_rd(REG5,GLOBAL_RXDTX_CTRL_ADDR);
    VoC_bnez16_r(ec_gain_pitch_EFR,REG5);
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_jump(ec_gain_pitch_EFR1);
ec_gain_pitch_EFR:
// EFR DTX END;
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG5,5,DEFAULT);         //tmp = gmed_n (st->pbuf, 5);
    VoC_jal(CII_gmed_n);

    VoC_lw16_sd(REG6,1,DEFAULT);
    VoC_lw16i(REG5,STATIC_CONST_PDOWN_ADDR);
    VoC_add16_rr(REG6,REG5,REG6,DEFAULT);

    VoC_bngt16_rd(Ecgains_L1,REG1,STRUCT_EC_GAIN_PITCHSTATE_PAST_GAIN_PIT_ADDR); //if (sub (tmp, st->past_gain_pit) > 0)
    VoC_lw16_d(REG1,STRUCT_EC_GAIN_PITCHSTATE_PAST_GAIN_PIT_ADDR);       //{   tmp = st->past_gain_pit;}
Ecgains_L1:
    VoC_multf16_rp(REG6,REG1,REG6,DEFAULT);  //*gain_pitch = mult (tmp, pdown[state]);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
ec_gain_pitch_EFR1:
    VoC_sw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);
    VoC_return;
}

/*************************************************************************
*
*  Function    : CII_ec_gain_pitch_update
   Input:  ec_gain_pitchState *st i/o : REG0     push16
           Word16 bfi,            i   : DEC_AMR_BFI_ADDRESS                 // direct address access
           Word16 prev_bf,        i   : STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR  // direct address access
           Word16 *gain_pitch     i/o : REG6(DEC_AMR_GAIN_PIT_ADDRESS)      // direct address access
* Created by Kenneth  07/13/2004
***************************************************************************/
void CII_ec_gain_pitch_update (void)
{
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_add16_rd(REG2,REG0,CONST_7_ADDR);
    VoC_bnez16_d(Ecgains_L2,DEC_AMR_BFI_ADDRESS);            //if (bfi == 0)  {

// EFR DTX BEGIN
    VoC_bez16_d(Ecgains_L3,GLOBAL_EFR_FLAG_ADDR);
    VoC_push16(RA,DEFAULT);
    VoC_jal(CII_efr_dtx_func11);
    VoC_pop16(RA,DEFAULT);
    VoC_jump(Ecgains_L33);
// EFR DTX END
Ecgains_L3:                             //    }
    VoC_bez16_d(Ecgains_L33,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);//    if (prev_bf != 0)  {
    VoC_lw16_p(REG7,REG2,DEFAULT);
    VoC_bngt16_rr(Ecgains_L33,REG6,REG7);               //        if (sub (*gain_pitch, st->prev_gp) > 0)
    VoC_movreg16(REG6,REG7,DEFAULT);                //        {  *gain_pitch = st->prev_gp;
    VoC_sw16_d(REG7,DEC_AMR_GAIN_PIT_ADDRESS);          //}
Ecgains_L33:

    VoC_sw16_p(REG6,REG2,DEFAULT);                   //    st->prev_gp = *gain_pitch;
Ecgains_L2:                             //}
    VoC_movreg16(REG7,REG6,DEFAULT);             //st->past_gain_pit = *gain_pitch;
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_sub16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_lw16i(REG1,4096);
    VoC_bnez16_d(Ecgains_Lnew2,GLOBAL_EFR_FLAG_ADDR)
    VoC_lw16i(REG1,16384);
Ecgains_Lnew2:
    VoC_bngt16_rr(Ecgains_L4,REG7,REG1);     //if (sub (st->past_gain_pit, 16384) > 0)
    VoC_movreg16(REG7,REG1,DEFAULT);                 //{  st->past_gain_pit = 16384;
Ecgains_L4:                             //}
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_add16_rr(REG1,REG0,REG6,DEFAULT);
    VoC_sw16_p(REG7,REG2,DEFAULT);

    VoC_loop_i_short(4,DEFAULT);                 //for (i = 1; i < 5; i++)
    VoC_lw16inc_p(REG6,REG1,IN_PARALLEL);
    VoC_startloop0                       //{
    VoC_lw16inc_p(REG6,REG1,DEFAULT);            //    st->pbuf[i - 1] = st->pbuf[i];
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0                         //}
    VoC_sw16_p(REG7,REG0,DEFAULT);               //st->pbuf[4] = st->past_gain_pit;
    VoC_return;
}


/********************************************************************************
  Function:  void CII_Int_lsf(void)
   input: lsf_old[] ->REG1 (incr 1)
          lsf_new[] ->REG2 (incr 1)
          i_subfr   ->REG7
          lsf_out[] ->REG0(incr 1)
  Created by Kenneth  07/09/2004
  Optimized by Kenneth 09/02/2004
    reOptimized by Cui 10/11/2004
    Optimized by Kenneth 11/11/2004
 ********************************************************************************/

void CII_Int_lsf(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16_sd(REG7,3,DEFAULT);    // i_subfr in reg7
    VoC_lw16i_set_inc(REG0,DEC_AMR_LSF_I_ADDRESS,1);

    VoC_lw16i_set_inc(REG1,DEC_AMR_PREV_LSF_ADDRESS,1);
    VoC_lw16i_set_inc(REG2,STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR,1);

    VoC_be16_rd(Lspavg_lab1,REG7,CONST_80_ADDR);    //else if ( sub(i_subfr, 80) == 0 )
    VoC_bnez16_r(Lspavg_lab3,REG7);         //if ( i_subfr == 0 )

    VoC_movreg16(REG2,REG1,DEFAULT);//{
    VoC_movreg16(REG1,REG2,IN_PARALLEL);//   for (i = 0; i < M; i++) {
Lspavg_lab1:                                 //   lsf_out[i] = add(sub(lsf_old[i], shr(lsf_old[i], 2)), shr(lsf_new[i], 2));
    VoC_lw16i(REG6,0x2000);             // 1/4
    VoC_loop_i_short(10,DEFAULT);               //{
    VoC_multf16inc_rp(REG4,REG6,REG1,IN_PARALLEL);
    VoC_startloop0                      //   for (i = 0; i < M; i++) {
    VoC_multf16_rp(REG5,REG6,REG2,DEFAULT); // lsf_out[i] = add(shr(lsf_old[i], 2), sub(lsf_new[i], shr(lsf_new[i], 2)));
    VoC_NOP();
    VoC_sub16inc_pr(REG5,REG2,REG5,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);
    VoC_multf16inc_rp(REG4,REG6,REG1,DEFAULT);
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    VoC_endloop0                    //   }
    VoC_jump(Lspavg_lab4);          //}

Lspavg_lab3:
    VoC_NOP();
    VoC_lw16inc_p(REG5,REG2,DEFAULT);                                                                                       //else if ( sub(i_subfr, 120) == 0 ){

    VoC_loop_i_short(10,DEFAULT);           //

    VoC_startloop0
    //   for (i = 0; i < M; i++) {
    VoC_be16_rd(Lspavg_lab2,REG7,CONST_120_ADDR);   //else if ( sub(i_subfr, 40) == 0 )

    VoC_shr16_ri(REG5, 1,DEFAULT);
    VoC_lw16inc_p(REG4,REG1,IN_PARALLEL);
    VoC_shr16_ri(REG4, 1,DEFAULT);
    VoC_add16_rr(REG5, REG4, REG5,DEFAULT);
Lspavg_lab2:
    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0                //   }  }


Lspavg_lab4:



    VoC_lw16i_set_inc(REG1,STRUCT_CB_GAIN_AVERAGESTATE_ADDR,1);
    VoC_lw16_d(REG7,DEC_AMR_GAIN_CODE_ADDRESS);

    VoC_lw16i_set_inc(REG2,STRUCT_LSP_AVGSTATE_LSP_MEANSAVE_ADDR,1);
    VoC_lw32_d(ACC1,STRUCT_DECOD_AMRSTATE_INBACKGROUNDNOISE_ADDR);
    VoC_sw16_sd(REG1,2,DEFAULT);
    VoC_sw16_sd(REG7,1,DEFAULT);
    VoC_push32(ACC1,IN_PARALLEL);       //   push32 stack32[n]
    //  gain_code_mix = Cb_gain_average(st->Cb_gain_averState, mode, gain_code, lsf_i, st->lsp_avg_st->lsp_meanSave, bfi,
    // st->prev_bf, pdfi, st->prev_pdf,st->inBackgroundNoise, st->voicedHangover);
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /**************************************************************************
     Function:    CII_Cb_gain_average
     Input:  *st       ->REG1(incr 1) push stack[n+4]
            gain_code  ->REG7         push stack[n+3]
            lsp[]      ->REG0(incr 1) push stack[n+2]
            lspAver[]  ->REG2(incr 1) push stack[n+1]
            bfi        ->DEC_AMR_BFI_ADDRESS                 //(direct address access, needn't pass the address as a parameter)
            prev_bf    ->STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR  //(direct address access, needn't pass the address as a parameter)
            pdfi       ->DEC_AMR_PDFI_ADDRESS                //(direct address access, needn't pass the address as a parameter)
            prev_pdf   ->STRUCT_DECOD_AMRSTATE_PREV_PDF_ADDR //(direct address access, needn't pass the address as a parameter)
            inBackgroundNoise ->  ACC1_HI
            voicedHangover    ->  ACC1_LO  push32 ACC1
     Output:cbGainMix         ->  ACC1_HI
     Version 1.0 Created by Kenneth    07/09/2004
     Version 1.1 Optimized by Kenneth  08/07/2004
    **************************************************************************/
//  start of Cb_gain_average
    VoC_lw16i_set_inc(REG0,STRUCT_CB_GAIN_AVERAGESTATE_ADDR+1,1);   //for (i = 0; i < (L_CBGAINHIST-1); i++)
    VoC_lw16i_set_inc(REG3,DEC_AMR_TEMP10_ADDRESS,1);
    VoC_loop_i_short(6,DEFAULT);        //{
    VoC_lw16inc_p(REG5,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0                //   st->cbGainHistory[i] = st->cbGainHistory[i+1]; //}
    VoC_sw16_p(REG7,REG1,DEFAULT);      //st->cbGainHistory[L_CBGAINHIST-1] = gain_code;

    VoC_lw16i_set_inc(REG0,DEC_AMR_LSF_I_ADDRESS,1);// lsp[]
    VoC_movreg16(REG1,REG2,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_loop_i(1,10);               //for (i = 0; i < M; i++) {
    VoC_lw16inc_p(REG4,REG1,DEFAULT);   //   tmp1 = abs_s(sub(lspAver[i], lsp[i]));
    VoC_sub16inc_rp(REG6,REG4,REG0,DEFAULT);
    VoC_push32(REG01,DEFAULT);
    VoC_lw16i_short(ACC0_LO,0,IN_PARALLEL);
    VoC_bnltz16_r(CII_Cb_gain_La2,REG6);
    VoC_sub16_dr(REG6,CONST_0_ADDR,REG6);
CII_Cb_gain_La2:
    VoC_movreg16(ACC0_HI,REG6,DEFAULT);
    VoC_jal(CII_NORM_L_ACC0);           //   shift1 = sub(norm_s(tmp1), 1);   REG5-> -shift1
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_movreg16(REG5,REG1,IN_PARALLEL);     //   tmp1 = shl(tmp1, shift1);
    VoC_movreg16(REG0,ACC0_HI,DEFAULT);
    VoC_movreg16(ACC0_HI,REG4,IN_PARALLEL);
    VoC_jal(CII_NORM_L_ACC0);           //   shift2 = norm_s(lspAver[i]);     REG7-> -shift2
    VoC_movreg16(REG1,ACC0_HI,DEFAULT); //   tmp2 = shl(lspAver[i], shift2);
    VoC_sub16_rr(REG5,REG5,REG1,IN_PARALLEL);//   shift = sub(add(2, shift1), shift2);
    VoC_jal(CII_DIV_S);                 // tmp[i] = div_s(tmp1, tmp2);
    VoC_shr16_ri(REG2,1,DEFAULT);   //   if (shift >= 0)
    VoC_shr16_rr(REG2,REG5,DEFAULT);        //   {tmp[i] = shr(tmp[i], shift);  }
    VoC_pop32(REG01,DEFAULT);       //   else  {    tmp[i] = shl(tmp[i], negate(shift));   }
    VoC_sw16inc_p(REG2,REG3,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG2,IN_PARALLEL);//diff = add(diff, tmp[i]);
    VoC_endloop1                    //}
    VoC_pop16(RA,DEFAULT);
    VoC_movreg16(REG5,REG7,DEFAULT);

    VoC_return;

}

/*************************************************************************
    FUNCTION:  CII_Reorder_lsf()
    Input  REG0(incr 1) ->Word16 *lsf: vector of LSFs   (range: 0<=val<=0.5)  ,
    Output REG0     ->Word16 *lsf: vector of LSFs
    used registers: REG0,REG5,REG4
 *************************************************************************/
void CII_Reorder_lsf (void)

{
    VoC_lw16i(REG5,205);                     //lsf_min = min_dist;
    VoC_loop_i_short(10,DEFAULT);            //for (i = 0; i < n; i++)
    VoC_startloop0                           //{
    VoC_lw16_p(REG4,REG0,DEFAULT);
    VoC_bngt16_rr(Reord_L,REG5,REG4);     //    if (sub (lsf[i], lsf_min) < 0)
    VoC_sw16_p(REG5,REG0,DEFAULT);        //    {     lsf[i] = lsf_min;   move16 (); }
Reord_L:
    VoC_add16inc_pd(REG5,REG0,CONST_205_ADDR);    //    lsf_min = add (lsf[i], min_dist);
    VoC_endloop0
    VoC_return;                              //}
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

void CII_pseudonoise (void)
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




//////////////////////////////////////////////////
//  REG0 for &cod[]
//  ACC0 for L_pn_seed_rx
//////////////////////////////////////////////////
void CII_build_CN_code(void)
{
    VoC_push16(REG0,DEFAULT);
    VoC_push16(RA,DEFAULT);

    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_loop_i_short( 20,DEFAULT);       // for (i = 0; i < L_SUBFR; i++)
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    VoC_startloop0
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);    //{ cod[i] = 0;}
    VoC_endloop0

    // for (k = 0; k < NB_PULSE; k++)
    VoC_loop_i(1,10);
    VoC_lw16i_short(REG0,2,DEFAULT);    // i= pseudonoise (seed, 2); seed is a pointer
    VoC_jal(CII_pseudonoise);       // REG7 for i
    VoC_multf32_rd (REG45,REG7,CONST_10_ADDR);// i= shr (extract_l (L_mult (i, 10)), 1);
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_shr16_ri(REG4,1,DEFAULT);                  //REG0:value of i
    VoC_add16_rr(REG6,REG1,REG4,DEFAULT); //i= add (i, k);

    VoC_jal(CII_pseudonoise);       //j = pseudonoise (seed, 1);

    VoC_lw16_sd(REG2,1,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG6, DEFAULT);        //REG2:addr of cod[i]
    VoC_lw16i_set_inc(REG0,4096,1);
    VoC_bgtz16_r(build_CN_code_101,REG7);//if (j > 0){cod[i] = 4096;}  else{ cod[i] = -4096; }
    VoC_lw16i_set_inc(REG0,-4096,1);
build_CN_code_101:
    VoC_add16_rd(REG1,REG1,CONST_1_ADDR);    //k++
    VoC_sw16_p(REG0,REG2,DEFAULT);
    VoC_endloop1


    VoC_pop16(RA,DEFAULT);
    VoC_pop16(REG0,DEFAULT);

    VoC_return;
}
/************************************************
  Function CII_Vq_subvec
  Input: RL7 == 0  -->  CII_Vq_subvec,CII_Vq_subvec4
             != 0  -->  CII_Vq_subvec_s
         &lsf_r1[] -->  push32(n+1) LO
         &wf1[]    -->  push32(n+1) HI
         &lsf_r2[] -->  push32(n) LO
         &wf2[]    -->  push32(n) HI
         *dico     -->  REG3(incr=2)
         dico_size/32 -> REG6
         &indice[] -> REG2(incr=1)  push16(n+1)
  requirment:  REG0.incr=2,REG1.incr=2;
  Output:&lsf_r1[+2] -->  push32(n+1) LO
         &wf1[+2]    -->  push32(n+1) HI
         &lsf_r2[+2] -->  push32(n) LO
         &wf2[+2]    -->  push32(n) HI

  Note: CII_Vq_subvec4,CII_Vq_subvec_s,CII_Vq_subvec can use this function only needs to
  confige the interface
  Optimized by Kenneth  20/09/2004
************************************************/

void CII_Vq_subvec(void)
{
    VoC_lw32z(REG45,DEFAULT);
    VoC_lw32_d(RL6,CONST_0x7FFFFFFF_ADDR);
    VoC_push16(REG3,DEFAULT);           //push16(n)  &dico[]
    VoC_lw32_sd(REG01,1,IN_PARALLEL);
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_loop_r(1,REG6);
    VoC_loop_i(0,32);           //in loop REG5 for sign
    VoC_sub16inc_pp(REG6,REG0,REG3,DEFAULT);    //  temp = sub (lsf_r1[0], *p_dico++);
    VoC_sub16inc_pp(REG7,REG0,REG3,IN_PARALLEL);    //  temp = mult (wf1[0], temp);

    VoC_multf16inc_rp(REG6,REG6,REG1,DEFAULT);  //  dist = L_mult (temp, temp);
    VoC_multf16inc_rp(REG7,REG7,REG1,IN_PARALLEL);  //  temp = sub (lsf_r1[1], *p_dico++);
    VoC_lw32_sd(REG01,0,DEFAULT);                   //  temp = mult (wf1[1], temp);
    VoC_bimac32_rr(REG67,REG67);                    //  dist = L_mac (dist, temp, temp);
    //  temp = sub (lsf_r2[0], *p_dico++);
    VoC_sub16inc_pp(REG6,REG0,REG3,DEFAULT);    //  temp = mult (wf2[0], temp);
    VoC_sub16inc_pp(REG7,REG0,REG3,IN_PARALLEL);    //  dist = L_mac (dist, temp, temp);

    VoC_multf16inc_rp(REG6,REG6,REG1,DEFAULT);  //  temp = sub (lsf_r2[1], *p_dico++);
    VoC_multf16inc_rp(REG7,REG7,REG1,IN_PARALLEL);  //  temp = mult (wf2[1], temp);
    VoC_NOP();

    VoC_bimac32_rr(REG67,REG67);                    //  dist = L_mac (dist, temp, temp);
    VoC_lw32_sd(REG01,1,DEFAULT);                   //   if (L_sub (dist, dist_min) < (Word32) 0)
    VoC_bngt32_rr(Vq_subvec_s_new_01,RL6,ACC0);     //   {
    VoC_lw16i_short(REG5,0,DEFAULT);        //       dist_min = dist;
    VoC_movreg32(RL6,ACC0,DEFAULT);         //       index = i;
    VoC_movreg16(REG4,REG2,IN_PARALLEL);    //       sign = 0;
Vq_subvec_s_new_01:                                             //    }
    VoC_bez32_r(Vq_subvec_s_new_02,RL7);
    /* test negative */
    VoC_sub16_rd(REG3,REG3,CONST_4_ADDR);            // p_dico -= 4;
    VoC_lw32z(ACC0,DEFAULT);                         // temp = add (lsf_r1[0], *p_dico++);
    VoC_add16inc_pp(REG6,REG0,REG3,DEFAULT);     // temp = mult (wf1[0], temp);
    VoC_add16inc_pp(REG7,REG0,REG3,IN_PARALLEL);     // dist = L_mult (temp, temp);
    VoC_multf16inc_rp(REG6,REG6,REG1,DEFAULT);       // temp = add (lsf_r1[1], *p_dico++);
    VoC_multf16inc_rp(REG7,REG7,REG1,IN_PARALLEL);   // temp = mult (wf1[1], temp);
    VoC_lw32_sd(REG01,0,DEFAULT);                    // dist = L_mac (dist, temp, temp);
    VoC_bimac32_rr(REG67,REG67);                     // temp = add (lsf_r2[0], *p_dico++);
    // temp = mult (wf2[0], temp);
    VoC_add16inc_pp(REG6,REG0,REG3,DEFAULT);     // dist = L_mac (dist, temp, temp);
    VoC_add16inc_pp(REG7,REG0,REG3,IN_PARALLEL);     // temp = add (lsf_r2[1], *p_dico++);
    VoC_multf16inc_rp(REG6,REG6,REG1,DEFAULT);       // temp = mult (wf2[1], temp);
    VoC_multf16inc_rp(REG7,REG7,REG1,IN_PARALLEL);   // dist = L_mac (dist, temp, temp);
    VoC_NOP();
    VoC_bimac32_rr(REG67,REG67);                     // if (L_sub (dist, dist_min) < (Word32) 0
    VoC_lw32_sd(REG01,1,DEFAULT);                    // {
    //     dist_min = dist;
    VoC_bngt32_rr(Vq_subvec_s_new_02,RL6,ACC0);      //     index = i;
    VoC_lw16i_short(REG5,1,DEFAULT);         //     sign = 1;
    VoC_movreg32(RL6,ACC0,DEFAULT);          // }                                      )
    VoC_movreg16(REG4,REG2,IN_PARALLEL);
Vq_subvec_s_new_02:
    VoC_lw32z(ACC0,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1


    VoC_pop16(REG3,DEFAULT);
    VoC_movreg16(REG6,REG4,IN_PARALLEL);
    VoC_lw16_sd(REG2,0,DEFAULT);
    VoC_shr16_ri(REG4,-2,IN_PARALLEL);

    VoC_bez32_r(Vq_subvec_LEnd,RL7);
    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG5,DEFAULT);
Vq_subvec_LEnd:
    VoC_lw32_sd(ACC0,1,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_sw16inc_p(REG6,REG2,IN_PARALLEL);

    VoC_lw32_d(RL6,CONST_0x00020002_ADDR);
    VoC_lw32inc_p(REG67,REG3,DEFAULT);
    VoC_lw32_sd(ACC1,0,IN_PARALLEL);
    VoC_movreg16(REG0,ACC0_LO,DEFAULT);
    VoC_movreg16(REG1,ACC1_LO,IN_PARALLEL);
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,RL6,IN_PARALLEL);
    VoC_sw16_sd(REG2,0,DEFAULT);
    VoC_sw32_sd(ACC0,1,DEFAULT);
    VoC_lw16i_short(REG2,0,IN_PARALLEL);
    VoC_sw32_sd(ACC1,0,DEFAULT);

    VoC_bnez16_r(Vq_subvec_s_new_05,REG5);      //if (sign == 0)
    VoC_lw32inc_p(REG45,REG3,DEFAULT);      //{
    VoC_sw32_p(REG67,REG0,DEFAULT);                 //lsf_r1[0] = *p_dico++;lsf_r1[1] = *p_dico++;
    VoC_sw32_p(REG45,REG1,DEFAULT);                 //lsf_r2[0] = *p_dico++;lsf_r2[1] = *p_dico++;
    VoC_return ;                                    //}
Vq_subvec_s_new_05:
    VoC_sub16_rr(REG6,REG2,REG6,DEFAULT);       //else{
    VoC_lw32inc_p(REG45,REG3,DEFAULT);              //lsf_r1[0] = negate (*p_dico++);
    VoC_sub16_rr(REG7,REG2,REG7,IN_PARALLEL);       //lsf_r1[1] = negate (*p_dico++);
    VoC_sub16_rr(REG4,REG2,REG4,DEFAULT);           //lsf_r2[0] = negate (*p_dico++);
    VoC_sub16_rr(REG5,REG2,REG5,IN_PARALLEL);       //lsf_r2[1] = negate (*p_dico++);
    VoC_sw32_p(REG67,REG0,DEFAULT);                 //}
    VoC_sw32_p(REG45,REG1,DEFAULT);
    VoC_return;
}


void CII_prm2bits_amr_efr(void)
{
#if 0
    voc_short    BIT_SERIAL_ADDR_RAM    ,40,x
#endif

    VoC_lw16i(REG1,BIT_SERIAL_ADDR_RAM);            //REG1:&bits_short
    VoC_lw16i_set_inc(REG2,COD_AMR_ANA_ADDR,1);         //reg2:&ana

    VoC_lw16i_short(INC3,-1,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);

    VoC_loop_r(1,REG5)
    VoC_lw16_p(REG6,REG0,DEFAULT);          //reg6:no_of_bits
    VoC_and16_ri(REG6,0xff);
    VoC_and16_ri(REG4,1);
    VoC_bez16_r(prm2bits_amr_efr_even,REG4)
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_shru16_ri(REG6,8,DEFAULT);
prm2bits_amr_efr_even:

    VoC_lw16inc_p(REG7,REG2,DEFAULT);       //REG7:value
    VoC_add16_rr(REG1,REG1,REG6,DEFAULT);       //bits += bitno[i];
    VoC_sub16_rd(REG3,REG1,CONST_1_ADDR);       //reg3:&bitstream[no_of_bits-1]

    VoC_loop_r_short(REG6,DEFAULT)
    VoC_startloop0
    // added by Yordan 20/09/2006
    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_and16_rr(REG5,REG7,DEFAULT);
    VoC_shr16_ri(REG7,1,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_endloop0
    VoC_add16_rd(REG4,REG4,CONST_1_ADDR);
    VoC_endloop1

    VoC_bez16_d(COD12D2_NO_SID_ENCODING,GLOBAL_EFR_FLAG_ADDR)
    VoC_bnez16_d(COD12D2_NO_SID_ENCODING,GLOBAL_OUTPUT_SP_ADDR)
    /* Insert SID codeword into the serial parameter frame */
    VoC_push16(RA,DEFAULT);
    VoC_jal(CII_sid_codeword_encoding);
    VoC_pop16(RA,DEFAULT);

COD12D2_NO_SID_ENCODING:

    VoC_lw16i_set_inc(REG0,BIT_SERIAL_ADDR_RAM,1);
    VoC_lw16i_set_inc(REG1,OUTPUT_BITS2_ADDR,1);
    VoC_add16_rd(REG1,REG1,CONST_4_ADDR);
    VoC_lw16i_short(REG2,15,DEFAULT);

    VoC_loop_i(1,16)
    VoC_lw32z(REG45,DEFAULT);
    VoC_add16_rr(REG3,REG0,REG2,IN_PARALLEL);   //REG3:&bits_short[i+15]

    VoC_loop_i_short(16,DEFAULT)
    VoC_startloop0
    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_lw16inc_p(REG6,REG3,IN_PARALLEL);   //reg6: bits_short
    VoC_and16_ri(REG6,1);
    VoC_or16_rr(REG4,REG6,DEFAULT);

    VoC_endloop0
    VoC_add16_rd(REG0,REG0,CONST_16_ADDR);
    VoC_sw16inc_p(REG4,REG1,DEFAULT);

    VoC_endloop1
    VoC_return;
}






