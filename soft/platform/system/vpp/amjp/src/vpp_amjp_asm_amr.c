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



#include "vppp_amjp_asm_map.h"
#include "vppp_amjp_asm_amr.h"

#include "voc2_library.h"
#include "voc2_define.h"


//////////////////////////////////////////////////////////////////////////
/*AMR RING DECODER*/
//////////////////////////////////////////////////////////////////////////

#if 0

voc_struct  VPP_AMJP_AMR_INOUT_STRUCT ,x

voc_short INPUT_BITS2_ADDR,8,x
voc_short INPUT_ACTUAL_ADDR,48,x
voc_short OUTPUT_SPEECH_BUFFER2_ADDR,160,x

voc_struct_end

#endif


#if 0
voc_struct AMR_TABLE_RAM_Y_STRUCT     , y

// short constants
voc_short CONST_AMR_0x1fff_ADDR ,y
voc_short CONST_AMR_5462_ADDR ,y
voc_short CONST_AMR_0x3fff_ADDR ,y
voc_short CONST_AMR_0x0800_ADDR,y
voc_short CONST_AMR_21299_ADDR,y
voc_short CONST_AMR_5443_ADDR,y
voc_short CONST_AMR_112_ADDR,y
voc_short CONST_AMR_105_ADDR,y
voc_short CONST_AMR_197_ADDR,y
voc_short CONST_AMR_31128_ADDR,y
voc_short CONST_AMR_134_ADDR,y
voc_short CONST_AMR_0x177_ADDR,y
voc_short CONST_AMR_102_ADDR,y
voc_short CONST_AMR_3277_ADDR,y
voc_short CONST_AMR_0x87_ADDR,y
voc_short CONST_AMR_NEG11_ADDR,y
voc_short CONST_AMR_31821_ADDR,y
voc_short CONST_AMR_29491_ADDR,y
voc_short CONST_AMR_9830_ADDR,y
voc_short CONST_AMR_14746_ADDR,y
voc_short CONST_AMR_17578_ADDR,y
voc_short CONST_AMR_16384_ADDR,y
voc_short CONST_AMR_256_ADDR,y
voc_short CONST_AMR_160_ADDR,y
voc_short CONST_AMR_143_ADDR,y
voc_short CONST_AMR_26214_ADDR,y
voc_short CONST_AMR_4096_ADDR,y
voc_short CONST_AMR_64_ADDR,y
voc_short CONST_AMR_128_ADDR,y
voc_short CONST_AMR_205_ADDR,y
voc_short CONST_AMR_50_ADDR,y
voc_short CONST_AMR_44_ADDR,y
voc_short CONST_AMR_2_ADDR,y
voc_short CONST_AMR_3_ADDR,y
voc_short CONST_AMR_4_ADDR,y
voc_short CONST_AMR_6_ADDR,y
voc_short CONST_AMR_7_ADDR,y
voc_short CONST_AMR_8_ADDR,y
voc_short CONST_AMR_9_ADDR,y
voc_short CONST_AMR_10_ADDR,y
voc_short CONST_AMR_12_ADDR,y
voc_short CONST_AMR_16_ADDR,y
voc_short CONST_AMR_17_ADDR,y
voc_short CONST_AMR_18_ADDR,y
voc_short CONST_AMR_20_ADDR,y
voc_short CONST_AMR_22_ADDR,y
voc_short CONST_AMR_25_ADDR,y
voc_short CONST_AMR_30_ADDR,y
voc_short CONST_AMR_31_ADDR,y
voc_short CONST_AMR_32_ADDR,y
voc_short CONST_AMR_33_ADDR,y
voc_short CONST_AMR_40_ADDR,y
voc_short CONST_AMR_41_ADDR,y
voc_short CONST_AMR_15_ADDR,y
voc_short CONST_AMR_11_ADDR,y
voc_short CONST_AMR_3276_ADDR,y
voc_short CONST_AMR_0x2000_ADDR,y
voc_short CONST_AMR_0x2666_ADDR,y
voc_short CONST_AMR_neg2_ADDR,y
voc_short CONST_AMR_80_ADDR,y
voc_short CONST_AMR_512_ADDR,y
voc_short CONST_AMR_1024_ADDR,y
voc_short CONST_AMR_0x199a_ADDR,y
voc_short CONST_AMR_120_ADDR,y
voc_short CONST_AMR_124_ADDR,y
voc_short CONST_AMR_1311_ADDR,y
voc_short CONST_AMR_368_ADDR,y
voc_short CONST_AMR_463_ADDR,y
voc_short CONST_AMR_9000_ADDR,y
voc_short CONST_AMR_5325_ADDR,y

// long constants
voc_word CONST_AMR_0_ADDR ,y
voc_word CONST_AMR_1_ADDR,y
voc_word CONST_AMR_0x40000000_ADDR,y
voc_word CONST_AMR_0x7FFFFFFF_ADDR,y
voc_word CONST_AMR_0x70816958_ADDR,y
voc_word CONST_AMR_0x00010001_ADDR,y
voc_word CONST_AMR_0x00020002_ADDR,y
voc_word CONST_AMR_0x00080008_ADDR,y
voc_word CONST_AMR_0xFFFFFFFF_ADDR,y
voc_word CONST_AMR_0x00004000L_ADDR,y
voc_word CONST_AMR_0x00007FFF_ADDR,y
voc_word CONST_AMR_0x00008000_ADDR,y
voc_word CONST_AMR_0x0000FFFF_ADDR,y
voc_word CONST_AMR_0x00000005L_ADDR,y

voc_alias CONST_AMR_0x40000000L_ADDR CONST_AMR_0x40000000_ADDR,y
voc_alias CONST_AMR_0x7fffffff_ADDR  CONST_AMR_0x7FFFFFFF_ADDR,y
voc_alias CONST_AMR_WORD32_0_ADDR    CONST_AMR_0_ADDR,y
voc_alias CONST_AMR_0x00000001_ADDR  CONST_AMR_1_ADDR,y
voc_alias CONST_AMR_0x7FFF_ADDR      CONST_AMR_0x00007FFF_ADDR,y
voc_alias CONST_AMR_0xFFFF_ADDR      CONST_AMR_0x0000FFFF_ADDR,y
voc_alias CONST_AMR_0x4000_ADDR      CONST_AMR_0x00004000L_ADDR  ,y
voc_alias CONST_AMR_5_ADDR           CONST_AMR_0x00000005L_ADDR,y
voc_alias CONST_AMR_0X80008_ADDR     CONST_AMR_0x00080008_ADDR,y
voc_alias CONST_AMR_0x00008000L_ADDR CONST_AMR_0x00008000_ADDR,y
voc_alias CONST_AMR_0x7fff_ADDR      CONST_AMR_0x7FFF_ADDR,y
voc_alias CONST_AMR_0x400_ADDR       CONST_AMR_1024_ADDR,y
voc_alias CONST_AMR_0x80008_ADDR     CONST_AMR_0x00080008_ADDR,y
voc_alias CONST_AMR_6554_ADDR        CONST_AMR_0x199a_ADDR,y

// EFR tables
voc_short TABLE_MEAN_LSF_ADDR,10,y
voc_short TABLE_SLOPE_ADDR,64,y
voc_short TABLE_LSP_LSF_ADDR,66,y
voc_short TABLE_LOG2_ADDR,34,y
voc_short TABLE_INV_SQRT_ADDR,50,y
voc_short TABLE_POW2_ADDR,34,y
voc_short STATIC_CONST_DHF_MASK_ADDR,58,y
voc_short STATIC_CONST_QUA_GAIN_PITCH_ADDR,16,y
voc_short STATIC_CONST_F_GAMMA4_ADDR,10,y
voc_short STATIC_CONST_F_GAMMA3_ADDR,10,y
voc_short STATIC_CONST_INTER_6_ADDR,62,y
voc_short STATIC_CONST_DGRAY_ADDR,8,y
voc_short TABLE_DICO1_LSF_ADDR,512,y
voc_short TABLE_DICO2_LSF_ADDR,1024,y
voc_short TABLE_DICO3_LSF_ADDR,1024,y
voc_short TABLE_DICO4_LSF_ADDR,1024,y
voc_short TABLE_DICO5_LSF_ADDR,256   ,y
voc_short STATIC_CONST_BITNO_ADDR,30,y

// AMR tables
voc_short STATIC_CONST_PRMNO_ADDR ,10    ,y
voc_short STATIC_CONST_PRMNOFSF_ADDR ,8,y
voc_short STATIC_CONST_bitno_MR475_ADDR,9 ,y
voc_short STATIC_CONST_bitno_MR515_ADDR,10,y
voc_short STATIC_CONST_bitno_MR59_ADDR,10,y
voc_short STATIC_CONST_bitno_MR67_ADDR,10,y
voc_short STATIC_CONST_bitno_MR74_ADDR,10,y
voc_short STATIC_CONST_bitno_MR795_ADDR,12,y
voc_short STATIC_CONST_bitno_MR102_ADDR,20,y
voc_short STATIC_CONST_bitno_MRDTX_ADDR,3,y
voc_short STATIC_CONST_BITNO_AMR_ADDR,10,y
voc_short STATIC_CONST_startPos_ADDR,16,y
voc_short STATIC_CONST_dhf_MR475_ADDR,18,y
voc_short STATIC_CONST_dhf_MR515_ADDR,20,y
voc_short STATIC_CONST_dhf_MR59_ADDR,20,y
voc_short STATIC_CONST_dhf_MR67_ADDR,20,y
voc_short STATIC_CONST_dhf_MR74_ADDR,20,y
voc_short STATIC_CONST_dhf_MR795_ADDR,24,y
voc_short STATIC_CONST_dhf_MR102_ADDR,40,y
voc_short STATIC_CONST_dhf_MR122_ADDR,58,y
voc_short STATIC_CONST_dhf_amr_ADDR,8,y
voc_short STATIC_CONST_qua_gain_code_ADDR,96,y
voc_short STATIC_CONST_lsp_init_data_ADDR,10,y
voc_short STATIC_past_rq_init_ADDR,80,y
voc_short TABLE_SQRT_L_ADDR,50,y
voc_short STATIC_CONST_WIND_200_40_ADDR,240,y
voc_short STATIC_trackTable_ADDR,20,y
voc_short STATIC_CONST_lsf_hist_mean_scale_ADDR,10,y
voc_short STATIC_CONST_dtx_log_en_adjust_ADDR,10,y
voc_short STATIC_CONST_pred_ADDR,4,y
voc_short STATIC_CONST_pred_MR122_ADDR,4,y
voc_short STATIC_CONST_b_60Hz_ADDR,4,y
voc_short STATIC_CONST_a_60Hz_ADDR,4,y
voc_short STATIC_CONST_gamma3_ADDR,10,y
voc_short AMR_RESET_VALUE_TABLE_ADDR,48,y

voc_struct_end
#endif


#if 0

voc_short STRUCT_POST_FILTERSTATE_RES2_ADDR                 ,40,x
voc_short STRUCT_POST_FILTERSTATE_MEM_SYN_PST_ADDR          ,10,x

voc_short STRUCT_PREEMPHASISSTATE_ADDR                      ,x
voc_alias STRUCT_PREEMPHASISSTATE_MEM_PRE_ADDR              STRUCT_PREEMPHASISSTATE_ADDR,x

voc_short STRUCT_AGCSTATE_ADDR                              ,x
voc_alias STRUCT_AGCSTATE_PAST_GAIN_ADDR                    STRUCT_AGCSTATE_ADDR,x

voc_short STRUCT_POST_FILTERSTATE_SYNTH_BUF_ADDR            ,170,x
voc_short STRUCT_POST_PROCESSSTATE_ADDR                     ,x
voc_short STRUCT_POST_PROCESSSTATE_Y2_HI_ADDR               ,x
voc_short STRUCT_POST_PROCESSSTATE_Y1_LO_ADDR               ,x
voc_short STRUCT_POST_PROCESSSTATE_Y1_HI_ADDR               ,x
voc_short STRUCT_POST_PROCESSSTATE_X0_ADDR                  ,x
voc_short STRUCT_POST_PROCESSSTATE_X1_ADDR                  ,x

voc_short STURCT_SPEECH_DECODE_FRAMESTATE_PREV_MODE_ADDR    ,8,x
voc_short TX_SP_FLAG_ADDR                                   ,2,x

voc_short  STRUCT_DECOD_AMRSTATE_OLD_EXC_ADDR               ,154 ,x     // reset 154
voc_short  STRUCT_DECOD_AMRSTATE_EXC_ADDR                   ,40 ,x
voc_short  STRUCT_DECOD_AMRSTATE_LSP_OLD_ADDR               ,10 ,x      // reset 10
voc_short  STRUCT_DECOD_AMRSTATE_MEM_SYN_ADDR               ,10 ,x      // reset 10
voc_short  STRUCT_DECOD_AMRSTATE_SHARP_ADDR                 ,x          // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR                ,x          // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR               ,x          // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_PREV_PDF_ADDR              ,x          // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_STATE_ADDR                 ,x          // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_T0_LAGBUFF_ADDR            ,x          // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_EXCENERGYHIST_ADDR         ,10,x       // reset 9
voc_short  STRUCT_DECOD_AMRSTATE_INBACKGROUNDNOISE_ADDR     ,x          // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_VOICEDHANGOVER_ADDR        ,x          // reset 1
voc_short  STRUCT_DECOD_AMRSTATE_LTPGAINHISTORY_ADDR        ,9,x        // reset 9
voc_short  STRUCT_DECOD_AMRSTATE_NODATASEED_ADDR            ,x          // reset 1

voc_short  STRUCT_BGN_SCDSTATE_ADDR                         ,60,x       // reset 60
voc_alias  STRUCT_BGN_SCDSTATE_FRAMEENERGYHIST_ADDR         STRUCT_BGN_SCDSTATE_ADDR,x
voc_short  STRUCT_BGN_SCDSTATE_BGHANGOVER_ADDR              ,2,x        // reset 1

voc_short  STRUCT_CB_GAIN_AVERAGESTATE_ADDR                 ,8,x        // reset 8
voc_alias  STRUCT_CB_GAIN_AVERAGESTATE_CBGAINHISTORY_ADDR   STRUCT_CB_GAIN_AVERAGESTATE_ADDR,x
voc_short  STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR       ,x          // reset 1
voc_short  STRUCT_CB_GAIN_AVERAGESTATE_HANGVAR_ADDR         ,x          // reset 1

voc_short  STRUCT_LSP_AVGSTATE_ADDR,10,x
voc_alias  STRUCT_LSP_AVGSTATE_LSP_MEANSAVE_ADDR            STRUCT_LSP_AVGSTATE_ADDR,x

voc_short  STRUCT_D_PLSFSTATE_ADDR,10,x
voc_alias  STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR               STRUCT_D_PLSFSTATE_ADDR,x
voc_short  STRUCT_D_PLSFSTATE_PAST_R_Q_ADDR,10,x

voc_short  STRUCT_EC_GAIN_PITCHSTATE_ADDR                   ,6,x        // reset 5
voc_alias  STRUCT_EC_GAIN_PITCHSTATE_PBUF_ADDR              STRUCT_EC_GAIN_PITCHSTATE_ADDR,x

voc_short  STRUCT_EC_GAIN_PITCHSTATE_PAST_GAIN_PIT_ADDR     ,x          // reset 1
voc_short  STRUCT_EC_GAIN_PITCHSTATE_PREV_GP_ADDR           ,x          // reset 1
voc_short  STRUCT_EC_GAIN_CODESTATE_ADDR                    ,6,x        // reset 6
voc_alias  STRUCT_EC_GAIN_CODESTATE_GBUF_ADDR               STRUCT_EC_GAIN_CODESTATE_ADDR,x
voc_short  STRUCT_EC_GAIN_CODESTATE_PAST_GAIN_CODE_ADDR     ,x          // reset 1
voc_short  STRUCT_EC_GAIN_CODESTATE_PREV_GC_ADDR            ,x          // reset 1

voc_short  STRUCT_GC_PREDSTATE_ADDR                         ,4,x
voc_alias  STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR             STRUCT_GC_PREDSTATE_ADDR,x
voc_short  STRUCT_GC_PREDSTATE_PAST_QUA_EN_MR122_ADDR       ,4,x        // reset 4

voc_short  STRUCT_PH_DISPSTATE_ADDR                         ,6,x        // reset 6
voc_alias  STRUCT_PH_DISPSTATE_GAINMEM_ADDR                 STRUCT_PH_DISPSTATE_ADDR,x
voc_short  STRUCT_PH_DISPSTATE_PREVSTATE_ADDR               ,x
voc_short  STRUCT_PH_DISPSTATE_PREVCBGAIN_ADDR              ,x
voc_short  STRUCT_PH_DISPSTATE_LOCKFULL_ADDR                ,x
voc_short  STRUCT_PH_DISPSTATE_ONSET_ADDR                   ,x

voc_short  STRUCT_DTX_DECSTATE_ADDR,x
voc_alias  STRUCT_DTX_DECSTATE_SINCE_LAST_SID_ADDR          STRUCT_DTX_DECSTATE_ADDR,x
voc_short  STRUCT_DTX_DECSTATE_TRUE_SID_PERIOD_INV_ADDR     ,x          // reset 1

voc_struct  STRUCT_AMR_DEC_INPUT_ADDR                       ,x

voc_short STRUCT_AMR_DEC_GLOBAL_MODE                    ,x
voc_short STRUCT_AMR_DEC_GLOBAL_RESET                   ,x
voc_short STRUCT_AMR_DEC_INBUF_ADDR                     ,2,x
voc_short STRUCT_AMR_DEC_OUTBUF_ADDR                    ,2,x

voc_struct_end

voc_short  STRUCT_DTX_DECSTATE_LSP_OLD_ADDR                 ,10,x       // reset 10
voc_short  STRUCT_DTX_DECSTATE_LSF_HIST_ADDR                ,80,x       // reset 80
voc_short  STRUCT_DTX_DECSTATE_LSF_HIST_PTR_ADDR            ,x          // reset 1
voc_short  STRUCT_DTX_DECSTATE_LOG_PG_MEAN_ADDR             ,x          // reset 1
voc_short  STRUCT_DTX_DECSTATE_LOG_EN_HIST_ADDR             ,8,x        // reset 8
voc_short  STRUCT_DTX_DECSTATE_LOG_EN_HIST_PTR_ADDR         ,x
voc_short  STRUCT_DTX_DECSTATE_LOG_EN_ADJUST_ADDR           ,x          // reset 1
voc_short  STRUCT_DTX_DECSTATE_DTXHANGOVERCOUNT_ADDR        ,x          // reset 1
voc_short  STRUCT_DTX_DECSTATE_DECANAELAPSEDCOUNT_ADDR      ,x          // reset 1
voc_short  STRUCT_DTX_DECSTATE_SID_FRAME_ADDR               ,x          // reset 1
voc_short  STRUCT_DTX_DECSTATE_VALID_DATA_ADDR              ,x          // reset 1
voc_short  STRUCT_DTX_DECSTATE_DTXHANGOVERADDED_ADDR        ,x          // reset 1
voc_short  STRUCT_DTX_DECSTATE_DTXGLOBALSTATE_ADDR          ,x          // reset 1
voc_short  STRUCT_DTX_DECSTATE_DATA_UPDATED_ADDR            ,2,x        // reset 1

#endif

#if 0

voc_short CONST_TAB244_ADDR,                    244,y
voc_short CONST_HEAD_RING,                        2,y

voc_short GLOBAL_RELOAD_MODE_ADDR,                  y
voc_short GLOBAL_RESET_FLAG_ADDR,                   y
voc_short GLOBAL_AMR_RESET_ADDR,                    y
voc_short GLOBAL_DEC_RESET_DONE_ADDR,               y
voc_short GLOBAL_RELOAD_RESET_FLAG_ADDR,            y
voc_short GLOBAL_BFI_ADDR,                          y
voc_short GLOBAL_UFI_ADDR,                          y
voc_short GLOBAL_SID_ADDR,                          y
voc_short GLOBAL_TAF_ADDR,                          y
voc_short GLOBAL_DEC_MODE_ADDR,                     y
voc_short GLOBAL_ENC_MODE_ADDR,                     y
voc_short GLOBAL_ENC_USED_MODE_ADDR,                y
voc_short DEC_RESET_FLAG_ADDR,                      y
voc_short DEC_MAIN_RESET_FLAG_OLD_ADDR,             y
voc_short DEC_AMR_FRAME_TYPE_ADDR,                2,y
voc_short DEC_INPUT_ADDR_ADDR,                      y
voc_short DEC_OUTPUT_ADDR_ADDR,y
voc_short DEC_INOUT_ADDR_BAK_ADDR,                2,y

voc_struct GLOBAL_OUTPUT_STRUCT_ADDR,               y
voc_short STRUCT_AMR_DEC_mode_ADDR,            y
voc_short STRUCT_AMR_DEC_CONSUME_ADDR,         y
voc_short STRUCT_AMR_DEC_length_ADDR,          y
voc_struct_end

#endif



void CII_MRRING_Decode(void)
{

    VoC_bez16_d(CII_MAIN_VOCODER,GLOBAL_RESET);     //if (reset_flag != 0)

    VoC_lw16i_set_inc(REG3,INPUT_BITS2_ADDR,2);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(REG1,0x10,DEFAULT);
    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,DEFAULT);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,DEFAULT);

    VoC_sw16_d(REG0,CFG_DMA_WRAP);

    VoC_lw16i(STATUS,STATUS_CLR);
    VoC_lw16i_short(BITCACHE,0,DEFAULT);
    VoC_sw16_d(REG0,CONST_HEAD_RING);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_short(REG5,0xffff,IN_PARALLEL);
    VoC_lw16i(REG4,0xf7);
    VoC_movreg16(ACC0_HI,REG4,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);

    VoC_sw16_d(ACC0_HI,GLOBAL_RESET);

    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_sw32inc_p(REG45,REG3,DEFAULT);

    VoC_sw16_d(REG7,GLOBAL_AMR_RESET_ADDR);

CII_MAIN_VOCODER:
    VoC_lw16i_set_inc(REG0,INPUT_BITS2_ADDR,2);
    VoC_add16_rd(REG0,REG0,CONST_AMR_2_ADDR);
    VoC_push16(RA,DEFAULT);

    // SAVE PARAMETERS
    VoC_lw32inc_p(REG67,REG0,DEFAULT);//mode

    VoC_lw32inc_p(REG23,REG0,DEFAULT);
    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    VoC_sw16_d(REG7,DEC_AMR_FRAME_TYPE_ADDR);

    VoC_sw16_d(REG2,GLOBAL_BFI_ADDR);
    VoC_sw16_d(REG3,GLOBAL_SID_ADDR);

    VoC_jal(CII_MAIN_DECODER);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_pop16(RA,IN_PARALLEL);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,12,IN_PARALLEL);
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i_short(REG6,80,IN_PARALLEL);//[int]

    VoC_lw16i(REG2,8000);

    VoC_sw16_d(ACC0_LO,GLOBAL_AMR_RESET_ADDR);

    VoC_sw16_d(REG4,GLOBAL_NB_CHAN);
    VoC_sw16_d(REG5,GLOBAL_BIT_RATE);
    VoC_sw32_d(REG23,GLOBAL_SAMPLE_RATE);

    VoC_return;
}






// VoC_directive: ALIGN
void CII_MAIN_DECODER(void)
{

    VoC_push16(RA,DEFAULT);

    // ****************
    // input :  REG2 : reset
    //          REG3 : coeffs_buffers, INC3 = 2
    // output : none
    // used : REG34, ACC0 1
    // not modified : REG2
    // ****************

    VoC_lw16_d(REG2,GLOBAL_AMR_RESET_ADDR);
    VoC_lw16i_short(FORMAT16,(15-16),DEFAULT);
    VoC_lw16i_short(FORMAT32,-1,DEFAULT);

    // bypass in calibration mode
    VoC_be16_rd(CII_MAIN_DECODER_end,REG7,CONST_AMR_0xFFFF_ADDR);

    // reload decoder
    VoC_jal(CII_reload_sideinfo);//VoC_jal(CII_reload_mode);
    VoC_jal(CII_AMR_Decode);


CII_MAIN_DECODER_end:

    VoC_pop16(RA,DEFAULT);

    // output parameters
    VoC_lw16_d(REG0,GLOBAL_DEC_RESET_DONE_ADDR); // REG0->RESET_Done
    VoC_lw16i_short(REG7,0,DEFAULT);             // decoder flag
    VoC_return;

}




/**************************/
// input in ACC0
// output in REG1  only used ACC0, REG1
/**************************/

void CII_NORM_L_ACC0(void)
{

    VoC_lw16i_set_inc(REG1,0,1);
    VoC_push32(ACC1,DEFAULT);
    VoC_movreg32(ACC1,ACC0,IN_PARALLEL);
    VoC_bez32_r(G_PITCH_NORM_L_3_EXIT,ACC0)
    VoC_bnltz32_r(G_PITCH_NORM_L_3_1,ACC0)
    VoC_not32_r(ACC0,DEFAULT);
G_PITCH_NORM_L_3_1:
    VoC_loop_i_short(31,DEFAULT)
    VoC_startloop0
    VoC_bnlt32_rd(G_PITCH_NORM_L_3_EXIT,ACC0,CONST_AMR_0x40000000L_ADDR)
    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);
    VoC_shr32_ri(ACC1,-1,DEFAULT);
    VoC_endloop0
G_PITCH_NORM_L_3_EXIT:
    VoC_movreg32(ACC0,ACC1,DEFAULT);
    VoC_pop32(ACC1,DEFAULT);
    VoC_return
}



/*********************/
// input in REG0, REG1
// output in  REG2
// used register RL6, RL7
// REG0/REG1
/*********************/

void CII_DIV_S(void)
{
    VoC_lw16i(REG2,0x7fff);
    VoC_be16_rr(SCHUR1_DIV_S_11,REG1,REG0)
    VoC_lw16i_set_inc(REG2,0,1);

    VoC_movreg16(RL6_LO, REG0, DEFAULT);
    VoC_movreg16(RL7_LO, REG1, IN_PARALLEL);
    VoC_lw16i_short(RL6_HI,0,DEFAULT);
    VoC_lw16i_short(RL7_HI,0, IN_PARALLEL);

    VoC_loop_i_short(14,DEFAULT)
    VoC_shr32_ri(RL6,-1,IN_PARALLEL);
    VoC_startloop0

    VoC_bgt32_rr(SCHUR1_DIV_S_1,RL7,RL6)
    VoC_sub32_rr(RL6,RL6,RL7,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
SCHUR1_DIV_S_1:
    VoC_shr16_ri(REG2,-1,DEFAULT);
    VoC_shr32_ri(RL6,-1,IN_PARALLEL);
    VoC_endloop0
    VoC_bgt32_rr(SCHUR1_DIV_S_11,RL7,RL6)
    VoC_inc_p(REG2,DEFAULT);
SCHUR1_DIV_S_11:

    VoC_return
}





// ****************************
// func: CII_reload_sideinfo
// input : REG7 <- mode flag
//         REG5     enc : 2 ,  dec : 1
// output: -
// used : REG3467
// ****************************

void CII_reload_sideinfo(void)
{

    // ******************
    // RESET
    // ******************

    // new & old reload mode
    VoC_movreg16(REG6,REG7,DEFAULT);
    VoC_lw16_d(REG4,GLOBAL_RELOAD_MODE_ADDR);

    // ignore AMR mode bits
    VoC_shr16_ri(REG6,7,DEFAULT);
    VoC_shr16_ri(REG4,7,IN_PARALLEL);

    // compute reset flag
    VoC_lw16i_short(REG3,0,DEFAULT);

    VoC_be16_rr(NO_SECTION_RESET,REG6,REG4);
    VoC_lw16i_short(REG3,1,DEFAULT);
NO_SECTION_RESET:

    // reset if vocoder reload is detected now or during the previous turn
    VoC_or16_rr(REG2,REG3,DEFAULT);
    VoC_or16_rd(REG2,GLOBAL_RELOAD_RESET_FLAG_ADDR);



    // keep reset flag & new mode for next turn
    VoC_sw16_d(REG3,GLOBAL_RELOAD_RESET_FLAG_ADDR);
    VoC_sw16_d(REG7,GLOBAL_RELOAD_MODE_ADDR);
    VoC_lw16i_set_inc(REG3,GLOBAL_DEC_MODE_ADDR,1);

    // ******************
    // AMR MODE FLAGS
    // ******************

    // mode flag for AMR modes
    VoC_loop_i_short(2,DEFAULT)

    VoC_startloop0

    VoC_lw16i_short(REG6,0xf,DEFAULT);
    VoC_and16_rr(REG6,REG7,DEFAULT);

    // check if a section was reloaded
    VoC_be16_rd(SECTION_4_DONE,REG6,CONST_AMR_8_ADDR)

    // mode flags
    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    //      VoC_sw16inc_p(REG6,REG3,DEFAULT);

SECTION_4_DONE:

    VoC_shr16_ri(REG7,4,DEFAULT);

    VoC_lw16i_set_inc(REG3,GLOBAL_ENC_MODE_ADDR,1);
    VoC_endloop0;

    VoC_sw16_d(REG6,GLOBAL_ENC_USED_MODE_ADDR);




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
    VoC_sub32_dr(REG45,CONST_AMR_WORD32_0_ADDR,REG45);   //    t1 = L_abs (t0);
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
    VoC_sub32_dr(REG67, CONST_AMR_0x7FFFFFFF_ADDR, ACC0);
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
    VoC_sub32_dr(REG67,CONST_AMR_WORD32_0_ADDR,REG67);  //    t2 = L_negate (t2);         /* K =-t0/Alpha                */
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
    VoC_bgt32_rd(INV_SQRT101,REG01,CONST_AMR_WORD32_0_ADDR)
    VoC_lw16i_short(REG0,(Word16)0xffff,DEFAULT);
    VoC_lw16i(REG1,0x3fff);

    VoC_return;
INV_SQRT101:
    /* REG2 for exp = norm_l (L_x);*/
    VoC_lw16i_set_inc(REG2,0,1);
    VoC_loop_i_short(31,DEFAULT)
    VoC_startloop0
    VoC_bnlt32_rd(NORM_L105,REG01,CONST_AMR_0x40000000_ADDR);
    VoC_inc_p(REG2,DEFAULT);
    VoC_shr32_ri(REG01,-1,IN_PARALLEL);
    VoC_endloop0

NORM_L105:;
    /*Normalization of L_x is done in norm_l*/

    VoC_sub16_dr(REG2,CONST_AMR_30_ADDR,REG2);

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
    VoC_sub16_rd(REG6,REG1,CONST_AMR_16_ADDR);
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
    VoC_be16_rd(Get_lsp_pol_L1,REG0,CONST_AMR_2_ADDR);
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
    //Change
    VoC_lw16_d(REG4,CONST_AMR_512_ADDR);
    VoC_msu32inc_rp(REG4,REG1,DEFAULT);
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
    VoC_add16_rd(REG1,REG1,CONST_AMR_1_ADDR);
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
    VoC_add32_rd(ACC0,ACC0,CONST_AMR_1_ADDR);
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_sub32_rr(ACC0,RL7,RL6,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(ACC0_LO,REG2,DEFAULT);
    exit_on_warnings = ON;
    // VoC_lw16i_short(REG5,12,IN_PARALLEL);
    // VoC_jal(CII_L_shr_r);
    VoC_shr32_ri(ACC0,12,DEFAULT);
    VoC_add32_rd(ACC0,ACC0,CONST_AMR_1_ADDR);
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
    VoC_loop_i_short(10,DEFAULT)
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
    VoC_loop_i_short(10,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_loop_i(1,64)
    VoC_sub16inc_rp(REG7,REG6,REG3,DEFAULT);
    VoC_bngtz16_r(LSP_LSF_02,REG7)
    VoC_inc_p(REG2,DEFAULT);
    VoC_sub16_rd(REG4, REG4,CONST_AMR_1_ADDR);
    VoC_endloop1
LSP_LSF_02:

    VoC_multf32_rp(REG67,REG7,REG4,DEFAULT);
    VoC_movreg16(REG5,REG2,DEFAULT); //ind
    VoC_shr32_ri(REG67,-3,DEFAULT);
    VoC_shr16_ri(REG5,-8,IN_PARALLEL);  //ind<<8
    VoC_add32_rd(REG67,REG67,CONST_AMR_0x00008000_ADDR);
    VoC_add16_rr(REG7,REG7,REG5,DEFAULT);
    VoC_add16_rd(REG3,REG3,CONST_AMR_1_ADDR);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0

    VoC_return
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
    VoC_sub16_dr(REG1,CONST_AMR_30_ADDR,REG1);//reg1:exponent
    VoC_shr32_ri(REG23,9,DEFAULT);

    VoC_sub16_rd(REG0,REG3,CONST_AMR_32_ADDR);      //reg0:i
    VoC_shr32_ri(REG23,1,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_and16_rd(REG2,CONST_AMR_0x7FFF_ADDR);    //reg2:a
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
    VoC_sub16_dr(REG6,CONST_AMR_16384_ADDR,REG6); /*wf[9] = sub (16384, lsf[8])*/
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
    VoC_sub16_rd(REG1,REG3,CONST_AMR_2_ADDR);   //REG1 for x[i-j]
    VoC_loop_i_short(10,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG0,DEFAULT);
    VoC_mac32inc_rp(REG7,REG1,DEFAULT);
    VoC_endloop0
    VoC_movreg16(REG0,RL7_LO,DEFAULT);
    VoC_shr32_ri(ACC0,-3,DEFAULT);
    VoC_add32_rd(REG45,ACC0,CONST_AMR_0x00008000_ADDR);
    VoC_NOP();
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    VoC_endloop1
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
    VoC_add32_rd(REG45,REG45,CONST_AMR_0x00008000_ADDR);

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
    VoC_loop_i_short(5,DEFAULT)
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
    VoC_multf32_rd(REG23,REG0,CONST_AMR_32_ADDR);
    VoC_lw16i_set_inc(REG0,TABLE_POW2_ADDR,1);
    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);   //REG0:addr of table[i]
    VoC_shr32_ri(REG23,1,IN_PARALLEL);
    VoC_and16_ri(REG2,0x7fff);          //reg2:a

    VoC_lw16inc_p(REG5,REG0,DEFAULT);       //REG45:L_x



    VoC_sub16_rp(REG3,REG5,REG0,DEFAULT);   //REG3:tmp

    VoC_multf32_rr(REG23,REG3,REG2,DEFAULT);
    VoC_sub16_dr(REG0,CONST_AMR_30_ADDR,REG1); //exp = sub (30, exponent);
    VoC_sub32_rr(REG23,REG45,REG23,DEFAULT);


//  VoC_lw16i_short(REG4,31,DEFAULT);//L_shr_r(r)
    VoC_bngt16_rd(POW2101,REG0,CONST_AMR_31_ADDR);
    VoC_lw32z(REG23,DEFAULT);
    VoC_return

POW2101:

    VoC_sub16_rd(REG0,REG0,CONST_AMR_1_ADDR);
    VoC_shr32_rr(REG23,REG0,DEFAULT); //L_shr_r = (L_shr(r-1) +1) >> 1
    VoC_bngt16_rd(POW2102,REG0,CONST_AMR_1_ADDR);
    VoC_add32_rd(REG23,REG23,CONST_AMR_0x00000001_ADDR);

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
    VoC_sub16_dr(REG0,CONST_AMR_0_ADDR,REG1);
    VoC_shr32_rr(REG67,REG0,DEFAULT);
    // REG1    exp
    VoC_shr32_ri(REG67,9, DEFAULT);
    VoC_lw16i_set_inc(REG0, TABLE_SQRT_L_ADDR, 1);
    VoC_add16_rr(REG0, REG0, REG7, DEFAULT);
    VoC_sub16_rd(REG0, REG0, CONST_AMR_16_ADDR);
    //REG0      table[i]
    VoC_shr32_ri(REG67,1, DEFAULT);
    VoC_and16_ri(REG6, 0x7fff);
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
    VoC_add16_rd(REG2,REG5,CONST_AMR_6_ADDR);
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
#if 0

    voc_short    MR475_gain_quant_coeff_ADDRESS             ,10,y       //[10]shorts
    voc_short    MR475_gain_quant_coeff_lo_ADDRESS          ,10,y       //[10]shorts

#endif

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
    VoC_sub16_dr(REG1,CONST_AMR_0_ADDR,REG7);// frac = negate (frac);

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
    VoC_lw32_d(ACC0,CONST_AMR_0x00008000_ADDR);
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
    VoC_add32_rd(ACC0,ACC0,CONST_AMR_0x00008000_ADDR);

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
    VoC_bne16_rd(SYN_FILT_01,REG6,CONST_AMR_5_ADDR);
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



    VoC_bnlt16_rd(Dec_amr_L32A,REG2,CONST_AMR_40_ADDR);
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
    VoC_add32_rd(ACC0,ACC0,CONST_AMR_0x00008000_ADDR);  //   st->exc[i] = round (L_temp);
    // put it here to replace a NOP
    VoC_lw16i_short(REG4,2,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    VoC_endloop0                //}

    VoC_lw16i_short(REG5,0,DEFAULT);        //   ph_disp_release(st->ph_disp_st);

    VoC_blt16_rd(Dec_amr_L64,REG4,GLOBAL_DEC_MODE_ADDR);    //if ( ((sub(mode, MR475) == 0) ||(sub(mode, MR515) == 0) ||(sub(mode, MR59) == 0))   &&
    VoC_bez16_d(Dec_amr_L64,DEC_AMR_BFI_ADDRESS);       //     sub(st->voicedHangover, 3) > 0 && st->inBackgroundNoise != 0 && bfi != 0 )
    VoC_lw32_sd(REG67,1,DEFAULT);
    VoC_bez16_r(Dec_amr_L64,REG6);
    VoC_bngt16_rd(Dec_amr_L64,REG7,CONST_AMR_3_ADDR);
    VoC_lw16i_short(REG5,1,DEFAULT);    //{  ph_disp_lock(st->ph_disp_st);}
Dec_amr_L64:
    VoC_lw16i_set_inc(REG3,STRUCT_PH_DISPSTATE_ADDR,1);
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
    VoC_bnlt16_rd(Phdisp_L1,REG7,CONST_AMR_14746_ADDR); //if (sub(ltpGain, PHDTHR2LTP) < 0) {
    VoC_lw16i_short(REG4,0,DEFAULT);            // } else
    VoC_bngt16_rd(Phdisp_L1,REG7,CONST_AMR_9830_ADDR);  //    if (sub(ltpGain, PHDTHR1LTP) > 0)
    VoC_lw16i_short(REG4,1,DEFAULT);            //    {  impNr = 1;

Phdisp_L1:                          //{impNr = 2; }  REG2->impNr
    //tmp1 = round(L_shl(L_mult(state->prevCbGain, ONFACTPLUS1), 2));
    VoC_multf32_rd(REG67,REG5,CONST_AMR_0x4000_ADDR);
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
    VoC_sub16inc_dp(REG7,CONST_AMR_9830_ADDR,REG2);
    VoC_bngtz16_r(Phdisp_L7,REG7);          //        if (sub(state->gainMem[i], PHDTHR1LTP) < 0)
    VoC_add16_rr(REG6,REG6,REG3,DEFAULT);       //        {i1 = add (i1, 1);}
Phdisp_L7:
    VoC_NOP();
    VoC_endloop0;                   //    }

    VoC_bngt16_rd(Phdisp_L6,REG6,CONST_AMR_2_ADDR);     //    if (sub(i1, 2) > 0)
    VoC_lw16i_short(REG4,0,DEFAULT);            //    {impNr = 0;    }
Phdisp_L6:                          //}
    VoC_bnez16_r(Phdisp_L8,REG1);           //if ((sub(impNr, add(state->prevState, 1)) > 0) && (state->onset == 0))
    VoC_sub16_rr(REG2,REG4,REG3,DEFAULT);
    VoC_bngt16_rd(Phdisp_L8,REG2,STRUCT_PH_DISPSTATE_PREVSTATE_ADDR);//{
    VoC_movreg16(REG4,REG2,DEFAULT);            //    impNr = sub (impNr, 1);}
Phdisp_L8:
    VoC_bngtz16_r(Phdisp_L9,REG1);
    VoC_bnlt16_rd(Phdisp_L9,REG4,CONST_AMR_2_ADDR);     //if((sub(impNr, 2) < 0) && (state->onset > 0))
    VoC_add16_rr(REG4,REG4,REG3,DEFAULT);       //{impNr = add (impNr, 1);}
Phdisp_L9:

    VoC_lw16i_short(INC3,2,DEFAULT);
    VoC_lw16_d(REG5,DEC_AMR_GAIN_CODE_MIX_ADDRESS);
    VoC_bnlt16_rd(Phdisp_L10,REG5,CONST_AMR_10_ADDR);   //if(sub(cbGain, 10) < 0)
    VoC_lw16i_short(REG4,2,DEFAULT);            //{impNr = 2;   }
Phdisp_L10:
    VoC_pop16(REG3,DEFAULT);
    VoC_bne16_rd(Phdisp_L11,REG0,CONST_AMR_1_ADDR);     //if(sub(state->lockFull, 1) == 0)
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
    VoC_sub16_dr(REG7,CONST_AMR_0_ADDR,REG7);

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

    VoC_bngt16_rd(Dec_amr_L69,REG4,CONST_AMR_0x4000_ADDR); //if (sub (pit_sharp, 16384) > 0)
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

    VoC_lw16i_short(REG0,0,DEFAULT);     //except EFR mode


    VoC_lw16i_set_inc(REG1,STRUCT_DECOD_AMRSTATE_MEM_SYN_ADDR,1);

    VoC_push32(REG67,DEFAULT);
    VoC_jal(CII_Syn_filt);              //   Syn_filt (Az, excp, &synth[i_subfr], L_SUBFR,st->mem_syn, 0);
    //}
    //else
    //{
    //   Overflow = 0;
    //   Syn_filt (Az, exc_enhanced, &synth[i_subfr], L_SUBFR,st->mem_syn, 0);
    //}

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
    VoC_add16_rd(REG0,REG7,CONST_AMR_30_ADDR);
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
    VoC_add16_rd(REG6,REG5,CONST_AMR_11_ADDR);
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


void CII_amr2cod(void)
{
    /****************************************************
    make the bit into AMR_122 RING
    done by gugogi
    *****************************************************/

    // extern FILE* file_serial;
    VoC_lw16i_set_inc(REG1,CONST_TAB244_ADDR,1);//temp_table[i]
    VoC_lw16i(REG2,AMR_DEC_BUFIN_ADDR);//short* serialp = &RAM_X[(BIT_SERIAL_ADDR_RAM)/2];
    // char  temp[32];
    VoC_lw16i_set_inc(REG0,LOCAL_ENC_SO_ADDR,2);//short Myring[256];

    VoC_add16inc_rp(REG6,REG2,REG1,DEFAULT);
    VoC_add16inc_rp(REG7,REG2,REG1,DEFAULT);
    VoC_loop_i(1,122);//for (i=0;i<244;i++)//serialp[temp_table[i]] = Myring[i] ;
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_add16inc_rp(REG6,REG2,REG1,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16_p(REG4,REG6,DEFAULT);
    VoC_add16inc_rp(REG7,REG2,REG1,DEFAULT);
    VoC_sw16_p(REG5,REG7,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop1

    VoC_lw16i_set_inc(REG2,AMR_DEC_BUFIN_ADDR+244,2);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_loop_i_short(6,DEFAULT);
    VoC_startloop0
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_endloop0
    /////////////////////////////////////////////////////////////
    VoC_lw16i_short(REG7,7,DEFAULT);
    VoC_lw16i_set_inc(REG3,DEC_AMR_PARM_ADDRESS,1);


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
    VoC_add16_rd(REG7,REG7,CONST_AMR_1_ADDR);
    // }
    //     bits += bitno[mode][i];      add(0,0);
    VoC_endloop1                 // }
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

    voc_alias LOCAL_ENC_SO_ADDR    DEC_AMR_LSP_NEW_ADDRESS,x     //160

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
    voc_short DEC_AMR_PARM_ADDRESS                      ,58,x        //[57]shorts
    voc_short COD_AMR_CODE_ADDRESS                      ,40 ,x           //[40]shorts
    voc_alias DEC_AMR_CODE_ADDRESS                COD_AMR_CODE_ADDRESS  ,x
#endif


#if 0

    voc_short    AMR_DEC_BUFOUT_ADDR     ,160,y

#endif


    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i_short(REG5,-1,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,INPUT_ACTUAL_ADDR,2);
    VoC_movreg16(REG6,REG0,DEFAULT);
    VoC_shr16_ri(REG6,1,DEFAULT);

    VoC_lw16i_short(REG7,8,DEFAULT);
    VoC_bez16_d(LABLE_NOT_1_FRAME,GLOBAL_AMR_RESET_ADDR);

    VoC_lw32_d(RL6,GLOBAL_STRM_BUF_IN_START);

    //first frame contains file header
    //VoC_lw16i_short(REG7,10,DEFAULT);
    VoC_NOP();

    VoC_sw32_d(RL6,GLOBAL_STRM_BUF_IN_PTR);
LABLE_NOT_1_FRAME:

    // ----------------------------
    // fct : vpp_AmjpCircBuffDmaRd
    // in :  REG4 -> local buffer wrap exponent
    //       REG5 -> local buffer wrap mask
    //       REG6 -> local buffer address
    //       REG7 -> load size (words)
    // out : REG5 -> new local buffer address
    // not modified : REG0
    // unused : REG012, ACC0, ACC1
    // ----------------------------

    // read data from external input circular buffer
    VoC_jal(vpp_AmjpCircBuffDmaRd);

    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_lw16_d(REG5,GLOBAL_AMR_RESET_ADDR);
    VoC_sw16_d(REG1,GLOBAL_BFI_ADDR);

    VoC_bez16_r(amr_dec_main_L117,REG5)    //if (reset_flag != 0)

    // update the old reset flag
    VoC_sw16_d(REG5,DEC_MAIN_RESET_FLAG_OLD_ADDR);

    // reset decoder
    VoC_lw32z(RL6,DEFAULT);
    VoC_jal(CII_dec_reset);
    VoC_jal(CII_Post_Filter_reset);

amr_dec_main_L117:

    VoC_lw16i_set_inc(REG0,INPUT_ACTUAL_ADDR,2);
    VoC_sw16_d(REG5,GLOBAL_DEC_RESET_DONE_ADDR);

    ////////// seria->bits  in  ram[LOCAL_ENC_SO_ADDR]/////////
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,LOCAL_ENC_SO_ADDR,1);
    VoC_sw16_d(REG5,DEC_AMR_FRAME_TYPE_ADDR);

    VoC_shr16_ri(REG6,8,DEFAULT);
    VoC_and16_ri(REG6,255);

    VoC_loop_i_short(8,DEFAULT);
    VoC_movreg16(REG4,REG7,IN_PARALLEL);
    VoC_startloop0
    VoC_movreg16(REG5,REG6,DEFAULT);
    VoC_and16_ri(REG5,128);

    VoC_movreg16(REG2,REG7,DEFAULT);
    VoC_shr16_ri(REG5,7,DEFAULT);
    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_endloop0

    VoC_loop_i(1,15);
ringtone_process_L4:
    VoC_and16_ri(REG4,255);
    VoC_loop_i_short(8,DEFAULT);
    VoC_shr16_ri(REG2,8,IN_PARALLEL);
    VoC_startloop0
    VoC_movreg16(REG5,REG4,DEFAULT);
    VoC_and16_ri(REG5,128);
    VoC_shr16_ri(REG5,7,DEFAULT);
    VoC_shr16_ri(REG4,-1,DEFAULT);

    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_endloop0


//      VoC_and16_rd(REG2,CONST_0x00ff_ADDR);
    VoC_and16_ri(REG2,255);

    VoC_loop_i_short(8,DEFAULT);
    VoC_startloop0
    VoC_movreg16(REG5,REG2,DEFAULT);
    VoC_and16_ri(REG5,128);
    VoC_shr16_ri(REG5,7,DEFAULT);
    VoC_shr16_ri(REG2,-1,DEFAULT);
    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_endloop0

    VoC_bez16_r(ringtone_process_L3,REG3);
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_movreg16(REG4,REG7,IN_PARALLEL);
    VoC_movreg16(REG2,REG7,DEFAULT);
    VoC_jump(ringtone_process_L4);
ringtone_process_L3:
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);
    VoC_movreg16(REG4,REG6,DEFAULT);
    VoC_movreg16(REG2,REG6,IN_PARALLEL);

    VoC_endloop1


    /////////////////////////////////////////////////////////////
    VoC_lw16_d(REG6,STURCT_SPEECH_DECODE_FRAMESTATE_PREV_MODE_ADDR);
    VoC_lw16_d(REG7,GLOBAL_DEC_MODE_ADDR);

    VoC_lw16i_short(REG5,7,DEFAULT);
    VoC_bne16_rd(amr_dec_main_L1,REG5,DEC_AMR_FRAME_TYPE_ADDR); //if (rx_type == 7)
    VoC_movreg16(REG7,REG6,DEFAULT);            //{ mode = prev_mode; }
    VoC_jump(amr_dec_main_L2);
amr_dec_main_L1:                        //else
    VoC_movreg16(REG6,REG7,DEFAULT);           //{prev_mode = mode;}

amr_dec_main_L2:

    VoC_push16(REG7,DEFAULT);               // push16 mode
    VoC_sw16_d(REG6,STURCT_SPEECH_DECODE_FRAMESTATE_PREV_MODE_ADDR);
    VoC_sw16_d(REG7,GLOBAL_DEC_MODE_ADDR);

    VoC_jal(CII_amr2cod);//VoC_jal(CII_bits2prm_amr_efr);//Bits2prm(mode, input_frame, param);
    VoC_lw32_d(REG23,DEC_RESET_FLAG_ADDR);

    VoC_lw16i(REG6,STATIC_CONST_PRMNO_ADDR);   //{decoder_homing_frame_test(&serial[1], mode);}
    VoC_bne16_rd(amr_dec_main_L3,REG3,CONST_AMR_1_ADDR);   //if (reset_flag_old == 1)
    VoC_lw16i(REG6,STATIC_CONST_PRMNOFSF_ADDR);    //{
amr_dec_main_L3:

// INLINE FUNCTION amr_dec_main_sr

    VoC_lw16_sd(REG7,0,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_lw16i_set_inc(REG2,STATIC_CONST_dhf_amr_ADDR,1);
    VoC_lw16_p(REG6,REG6,DEFAULT);// decoder_homing_frame_test(&serial[1], mode);}
    VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,DEC_AMR_PARM_ADDRESS,1);     //j = 0;
    VoC_lw16_p(REG2,REG2,DEFAULT);

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

    //  VoC_bne16_rd(amr_dec_main_L3B,REG7,CONST_1_ADDR);   //if (reset_flag_old == 1)
    VoC_bez16_r(amr_dec_main_L3B,REG7);
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
    VoC_lw32_d(ACC0,CONST_AMR_0x00080008_ADDR);
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

amr_dec_main_L7:
    VoC_lw16i_set_inc(REG0,AMR_DEC_BUFOUT_ADDR,2);
    VoC_lw16i_set_inc(REG1,OUTPUT_SPEECH_BUFFER2_ADDR,2);
    //VoC_lw16d_set_inc(REG1,DEC_OUTPUT_ADDR_ADDR,2);

    VoC_movreg16(REG3,REG1,DEFAULT);//START ADDR
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
    VoC_jal(CII_copy_xy);
    ////////DMAI WRITE DATA
    VoC_shru16_ri(REG3,1,DEFAULT);
    VoC_lw16i_short(REG2,80,IN_PARALLEL);

    VoC_lw32_d(RL6,GLOBAL_STRM_BUF_OUT_PTR);

    VoC_sw32_d(REG23,CFG_DMA_SIZE);
    VoC_sw32_d(RL6,CFG_DMA_EADDR);

//goto_AMR122RING_WRITE_DECODE:
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    ///////DMAI  WRITE END

    VoC_lw16_d(REG0,DEC_RESET_FLAG_ADDR); // reset_flag

    VoC_jump(ringtone_main_end);

ringtone_main_L8://if amr_rind header is wrong , set the bfi flag
    VoC_sw16_d(REG1,GLOBAL_BFI_ADDR);
ringtone_main_end:

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
    VoC_lw32_d(RL6,CONST_AMR_0x00008000L_ADDR);
    VoC_lw16_sd(REG7,1,DEFAULT);
    VoC_lw16_d(REG6,DEC_AMR_FRAME_TYPE_ADDR);
    VoC_push32(RL6,DEFAULT);            // push32  0x8000 in stack32
    VoC_be16_rd(SpDec_Fr_L1,REG6,CONST_AMR_5_ADDR); //if ((frame_type == RX_SID_BAD) ||
    VoC_bne16_rd(SpDec_Fr_L2,REG6,CONST_AMR_6_ADDR);//    (frame_type == RX_SID_UPDATE))
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

    VoC_bgt16_rd(Post_Filter_L4,REG6,CONST_AMR_5_ADDR);     //  if (sub(mode, MR122) == 0 || sub(mode, MR102) == 0)
    VoC_lw16i_set_inc(REG1,STATIC_CONST_gamma3_ADDR,1);        //      Weight_Ai (Az, gamma3, Ap3);
    // VoC_add16_rr(REG7,REG1,REG0,DEFAULT);            //      Weight_Ai (Az, gamma4, Ap4);  }
    VoC_lw16i(REG7,STATIC_CONST_F_GAMMA3_ADDR);
Post_Filter_L4:



    VoC_lw16i_set_inc(REG2,DEC_AMR_TEMP0_ADDRESS,1);    //AP3-> DEC_AMR_TEMP0_ADDRESS

    VoC_loop_i(1,2);
    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_jal(CII_Weight_Ai);
    //AP4-> DEC_AMR_TEMP11_ADDRESS
    VoC_movreg16(REG1,REG7,DEFAULT);
    VoC_endloop1;

    VoC_sub16_rd(REG0,REG0,CONST_AMR_1_ADDR);


    VoC_lw16_sd(REG6,0,DEFAULT);
    VoC_sw16_sd(REG0,1,DEFAULT);

    VoC_lw16i_set_inc(REG3,STRUCT_POST_FILTERSTATE_SYNTH_BUF_ADDR+10,1);
    VoC_lw16i_set_inc(REG2,STRUCT_POST_FILTERSTATE_RES2_ADDR,1);
    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);
    VoC_lw16i_set_inc(REG0,DEC_AMR_TEMP0_ADDRESS,1);
    VoC_push16(REG3,DEFAULT);               // push16 stack[n-2] ->&syn_work[i_subfr]
    VoC_jal(CII_Residu_new);                    //  Residu (Ap3, &syn_work[i_subfr], st->res2, L_SUBFR);

    // para for syn_filt
    VoC_lw16i_short(REG6,22,DEFAULT);

    VoC_lw16i_set_inc(REG3,DEC_AMR_TEMP0_ADDRESS,1);
    VoC_lw16i_set_inc(REG1,DEC_AMR_TEMP22_ADDRESS,1);
    VoC_movreg16(REG7,REG1,DEFAULT);

    VoC_add16_rd(REG2,REG1,CONST_AMR_11_ADDR);
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
    VoC_multf16_rd(REG0,REG0,CONST_AMR_26214_ADDR); //      temp2 = mult (temp2, MU);
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
    VoC_lw16i_set_inc(REG1,STRUCT_AGCSTATE_PAST_GAIN_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_push16(REG1,DEFAULT);               // push16 stack[n-2]
    VoC_lw32_sd(RL7,0,DEFAULT);
    VoC_jal(CII_agc);       //   agc (st->agc_state, &syn_work[i_subfr], &syn[i_subfr],AGC_FAC, L_SUBFR);
    VoC_pop16(REG1,DEFAULT);            // pop16 stack[n-2]

    VoC_pop16(REG6,DEFAULT);                // pop16 stack[n-1]
    VoC_add16_rd(REG6,REG6,CONST_AMR_40_ADDR);


    VoC_be16_rd(not_Post_Filter_L1,REG6,CONST_AMR_160_ADDR);    //for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
    VoC_jump(Post_Filter_L1);
not_Post_Filter_L1:

    VoC_lw16i_set_inc(REG1,STRUCT_POST_FILTERSTATE_SYNTH_BUF_ADDR,2);
    VoC_lw16i_set_inc(REG0,STRUCT_POST_FILTERSTATE_SYNTH_BUF_ADDR+160,2);
    VoC_jal(CII_Copy_M);    //Copy (&syn_work[L_FRAME - M], &syn_work[-M], M);

//  END of INLINE FUNCTION CII_Post_Filter

    VoC_pop16(REG2,DEFAULT);


    VoC_lw16i_set_inc(REG0,STRUCT_POST_PROCESSSTATE_ADDR,2);
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

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



    VoC_pop32(RL6,DEFAULT);

    VoC_pop16(REG0,DEFAULT);
    VoC_lw16i_short(REG2,0xfff8,IN_PARALLEL);


    VoC_movreg16(REG1, REG0,DEFAULT);

    VoC_lw16i_short(INC0,1,DEFAULT);    // EFR
    VoC_pop16(RA,DEFAULT);
    VoC_lw16inc_p(REG6,REG0,DEFAULT);            // EFR

    VoC_loop_i(0,160);                           // EFR
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




    //newDTXState = rx_dtx_handler(st->dtxDecoderState, frame_type);
    VoC_jal(CII_rx_dtx_handler);

    VoC_lw32z(REG23,DEFAULT);
    VoC_sw16_d(REG4,DEC_AMR_NEWDTXSTATE_ADDRESS);
    VoC_sw16_d(REG2,DEC_AMR_INDEX_MR475_ADDRESS);
    // sw BFI & PDFI
    VoC_sw32_d(REG23,DEC_AMR_BFI_ADDRESS);



    VoC_bne16_rd(Dec_amr_L3,REG7,CONST_AMR_1_ADDR);     //else if (sub(frame_type, RX_SPEECH_DEGRADED) == 0)
    VoC_sw16_d(REG7,DEC_AMR_PDFI_ADDRESS);
Dec_amr_L3:
    VoC_be16_rd(Dec_amr_L2,REG7,CONST_AMR_3_ADDR);      //if ((sub(frame_type, RX_SPEECH_BAD) == 0)
    VoC_be16_rd(Dec_amr_L2,REG7,CONST_AMR_7_ADDR);      //    || (sub(frame_type, RX_NO_DATA) == 0)
    VoC_be16_rd(Dec_amr_L2,REG7,CONST_AMR_2_ADDR);      //    ||(sub(frame_type, RX_ONSET) == 0))
    VoC_jump(Dec_amr_L4);
Dec_amr_L2:                         //{
    VoC_lw16i_short(REG6,1,DEFAULT);            //   bfi = 1;
    VoC_lw16_sd(REG5,3,DEFAULT);            // mode in reg5
    VoC_sw16_d(REG6,DEC_AMR_BFI_ADDRESS);

    //   if ((sub(frame_type, RX_NO_DATA) == 0) //||(sub(frame_type, RX_ONSET) == 0))
    VoC_bne16_rd(Dec_amr_L4_go,REG7,CONST_AMR_3_ADDR);
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
    VoC_multf32_rd(ACC0,REG4,CONST_AMR_31821_ADDR); //*seed = extract_l(L_add(L_shr(L_mult(*seed, 31821), 1), 13849L));
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
    VoC_add16_rd(REG3,REG4,CONST_AMR_1_ADDR);               //{    st->state = add (st->state, 1);
    VoC_bnez16_d(Dec_amr_L7,DEC_AMR_BFI_ADDRESS);   //if (bfi != 0)
    VoC_lw16i_short(REG3,5,DEFAULT);            //{     st->state = 5;
    VoC_be16_rd(Dec_amr_L7,REG4,CONST_AMR_6_ADDR);      //else if (sub (st->state, 6) == 0)
    //else
    VoC_lw16i_short(REG3,0,DEFAULT);          //{     st->state = 0;  }
Dec_amr_L7:

    VoC_bngt16_rd(Dec_amr_L9,REG3,CONST_AMR_6_ADDR);    //if (sub (st->state, 6) > 0)
    VoC_lw16i_short(REG3,6,DEFAULT);            //{    st->state = 6; }
Dec_amr_L9:





    VoC_lw16_d(REG4,STRUCT_DTX_DECSTATE_DTXGLOBALSTATE_ADDR);
    VoC_lw16_d(REG5,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);

    VoC_bne16_rd(Dec_amr_L11_1,REG4,CONST_AMR_1_ADDR);  //if (sub(st->dtxDecoderState->dtxGlobalState, DTX) == 0)
    //{  st->state = 5;
    VoC_lw16i_short(REG5,0,DEFAULT);        //   st->prev_bf = 0;
    VoC_lw16i_short(REG3,5,DEFAULT);
Dec_amr_L11_1:
    VoC_bne16_rd(Dec_amr_L11,REG4,CONST_AMR_2_ADDR);    //else if (sub(st->dtxDecoderState->dtxGlobalState, DTX_MUTE) == 0)

    VoC_lw16i_short(REG5,1,DEFAULT);        //    st->prev_bf = 1;
    VoC_lw16i_short(REG3,5,DEFAULT);                    //}
Dec_amr_L11:

    VoC_sw16_d(REG5,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);

    VoC_sw16_d(REG3,STRUCT_DECOD_AMRSTATE_STATE_ADDR);


    VoC_lw16i_set_inc(REG0,STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR,2);
    VoC_lw16i_set_inc(REG1,DEC_AMR_PREV_LSF_ADDRESS,2);
    VoC_jal(CII_Copy_M);                //Copy (st->lsfState->past_lsf_q, prev_lsf, M);

    VoC_lw16i_set_inc(REG0,STRUCT_D_PLSFSTATE_ADDR,1);
    VoC_lw16i_set_inc(REG1,DEC_AMR_PARM_ADDRESS,1);
    VoC_lw16_sd(REG7,3,DEFAULT);            // mode in reg7
    VoC_lw16_d(REG6,DEC_AMR_BFI_ADDRESS);
    VoC_lw16i(REG2,STRUCT_DECOD_AMRSTATE_LSP_OLD_ADDR);
    VoC_lw16i(REG3,DEC_AMR_LSP_NEW_ADDRESS);
    VoC_lw16i(REG4,DEC_AMR_LSP_MID_ADDRESS);
    VoC_lw16i(REG5,DEC_AMR_A_T_ADDRESS);
    VoC_push32(REG23,DEFAULT);              // push32 stack32[n]  &lsp_new[]/&lsp_old[]
    VoC_push32(REG45,DEFAULT);              // push32 stack32[n-1]  &A_t[]/&lsp_mid[]




    VoC_jal(CII_amr_dec_122);



//spy_on_addr = OFF;
    VoC_jal(CII_AMR_subfunc0);
//spy_on_addr = ON;


//   end  of  dtx_dec_activity_update
//*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    //st->prev_bf = bfi;
    //st->prev_pdf = pdfi;
    VoC_lw32_d(REG67,DEC_AMR_BFI_ADDRESS);
    VoC_NOP();
    VoC_sw32_d(REG67,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);

    VoC_pop16(RA,DEFAULT);              // pop16 stack[n]
    VoC_lw16i_set_inc(REG0,STRUCT_LSP_AVGSTATE_ADDR,1); //   lsp_avg(st->lsp_avg_st, st->lsfState->past_lsf_q);
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

    VoC_lw16i_set_inc(REG2,STRUCT_DECOD_AMRSTATE_OLD_EXC_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i_set_inc(REG3,STRUCT_DECOD_AMRSTATE_MEM_SYN_ADDR,1);
    VoC_loop_i(0,154);
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG2,STRUCT_DECOD_AMRSTATE_SHARP_ADDR,1);
    VoC_loop_i_short(10,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_endloop0

    VoC_sw16inc_p(REG4,REG2,DEFAULT);

    VoC_lw16i_set_inc(REG2,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR,1);
    VoC_loop_i_short(3,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG2,STRUCT_DECOD_AMRSTATE_INBACKGROUNDNOISE_ADDR,1);
    VoC_loop_i_short(2,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG2,STRUCT_DECOD_AMRSTATE_EXCENERGYHIST_ADDR,1);
    VoC_loop_i_short(9,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG2,STRUCT_DECOD_AMRSTATE_LTPGAINHISTORY_ADDR,1);
    VoC_loop_i_short(9,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG2,STRUCT_CB_GAIN_AVERAGESTATE_CBGAINHISTORY_ADDR,1);
    VoC_loop_i_short(10,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG2,STRUCT_D_PLSFSTATE_PAST_R_Q_ADDR,1);
    VoC_loop_i_short(10,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_endloop0


    VoC_lw16i_set_inc(REG2,STRUCT_BGN_SCDSTATE_FRAMEENERGYHIST_ADDR,1);
    VoC_loop_i_short(61,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG2,STRUCT_PH_DISPSTATE_ADDR,1);
    VoC_lw16i_set_inc(REG3,STRUCT_DTX_DECSTATE_SINCE_LAST_SID_ADDR,1);
    VoC_loop_i_short(5,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_endloop0

    VoC_sw16inc_p(REG4,REG3,DEFAULT);

    VoC_lw16i_set_inc(REG2,STRUCT_DTX_DECSTATE_LSF_HIST_PTR_ADDR,1);
    VoC_loop_i_short(3,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG2,STRUCT_DTX_DECSTATE_SID_FRAME_ADDR,1);
    VoC_lw16i_set_inc(REG3,STRUCT_DTX_DECSTATE_DATA_UPDATED_ADDR,1);
    VoC_loop_i_short(3,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_endloop0
    VoC_sw16inc_p(REG4,REG3,DEFAULT);

    VoC_lw32z(RL7,DEFAULT);



    VoC_lw16_d(REG5,CONST_AMR_40_ADDR);
    VoC_lw16i_set_inc(REG1,STRUCT_DECOD_AMRSTATE_LSP_OLD_ADDR,1);
    VoC_lw16i_set_inc(REG2,STATIC_CONST_lsp_init_data_ADDR,1);
    VoC_sw16_d(REG5,STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR);

    VoC_loop_i_short(10,DEFAULT);
    VoC_lw16inc_p(REG4,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_sw16_d(REG5,STRUCT_DECOD_AMRSTATE_T0_LAGBUFF_ADDR);

    VoC_lw16i_set_inc(REG0,TABLE_MEAN_LSF_ADDR,1);
    VoC_lw16i_set_inc(REG1,STRUCT_LSP_AVGSTATE_LSP_MEANSAVE_ADDR,1);
    VoC_loop_i_short(10,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_lw16i_set_inc(REG0,TABLE_MEAN_LSF_ADDR,1);
    VoC_lw16i_set_inc(REG1,STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR,1);
    VoC_loop_i_short(10,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_lw16i_set_inc(REG0,AMR_RESET_VALUE_TABLE_ADDR+6,1);
    VoC_lw16i_set_inc(REG1,STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR,1);
    VoC_loop_i_short(4,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_lw16i_set_inc(REG0,AMR_RESET_VALUE_TABLE_ADDR+10,1);
    VoC_lw16i_set_inc(REG1,STRUCT_GC_PREDSTATE_PAST_QUA_EN_MR122_ADDR,1);
    VoC_lw16_d(REG5,AMR_RESET_VALUE_TABLE_ADDR+34);
    VoC_loop_i_short(4,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_sw16_d(REG5,STRUCT_DECOD_AMRSTATE_NODATASEED_ADDR);

    VoC_lw16i_set_inc(REG0,TABLE_MEAN_LSF_ADDR,1);
    VoC_lw16i_set_inc(REG1,STRUCT_DTX_DECSTATE_LSF_HIST_ADDR,1);

    VoC_loop_i(1,8);

    VoC_loop_i_short(10,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_lw16i_set_inc(REG0,TABLE_MEAN_LSF_ADDR,1);
    VoC_NOP();
    VoC_endloop1


    VoC_lw16i_set_inc(REG0,AMR_RESET_VALUE_TABLE_ADDR+36,1);
    VoC_lw16i_set_inc(REG1,STRUCT_DTX_DECSTATE_LOG_EN_HIST_ADDR,1);
    VoC_loop_i_short(8,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_lw16i_set_inc(REG0,AMR_RESET_VALUE_TABLE_ADDR+44,1);
    VoC_lw16i_set_inc(REG1,STRUCT_DTX_DECSTATE_DTXHANGOVERCOUNT_ADDR,1);
    VoC_lw16_d(REG5,AMR_RESET_VALUE_TABLE_ADDR+46);
    VoC_loop_i_short(2,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_sw16_d(REG5,STRUCT_DTX_DECSTATE_DTXGLOBALSTATE_ADDR);



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
    VoC_sw32_d(ACC1,STRUCT_PREEMPHASISSTATE_MEM_PRE_ADDR);
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
    VoC_be32_rd(Energy_L0,ACC1,CONST_AMR_0x7FFFFFFF_ADDR);


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
    VoC_add32_rd(REG23,REG01,CONST_AMR_0x00008000L_ADDR);
AGC_LA5:
    VoC_movreg16(REG5,REG3,DEFAULT);
    VoC_bnez16_r(AGC_LA4,REG4);              //agc :g0 = mult (go, sub (32767, agc_fac));
    VoC_lw16_sd(REG7,2,DEFAULT);             //agc: gain = st->past_gain;
    VoC_multf16_rd(REG3,REG3,CONST_AMR_3276_ADDR);       //}
    VoC_lw16_p(REG5,REG7,DEFAULT);
AGC_LA4:
    VoC_lw16_sd(REG0,0, DEFAULT);
// opti: two branch are deleted here by loading default values
    VoC_loop_i_short(40,DEFAULT);            //for (i = 0; i < l_trm; i++)
    VoC_startloop0                   //{
    VoC_bnez16_r(AGC_LA6,REG4);
    VoC_multf16_rd(REG5,REG5,CONST_AMR_29491_ADDR); //agc: gain = mult (gain,agc_fac);
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
    VoC_be16_rd(word32_needed,REG7,CONST_AMR_7_ADDR)
    VoC_lw16i_short(REG5,7,DEFAULT);
    VoC_be16_rd(word32_needed,REG7,CONST_AMR_6_ADDR)
    VoC_lw16i_short(REG5,5,DEFAULT);
    VoC_bgt16_rd(word32_needed,REG7,CONST_AMR_2_ADDR)
    VoC_lw16i_short(REG5,4,DEFAULT);
    VoC_bgt16_rd(word32_needed,REG7,CONST_AMR_0_ADDR)
    VoC_lw16i_short(REG5,3,DEFAULT);

word32_needed:

    //VoC_lw16d_set_inc(REG0,DEC_INPUT_ADDR_ADDR,2);
    //VoC_add16_rd(REG0,REG0,CONST_8_ADDR);
    VoC_lw16i_set_inc(REG0,INPUT_ACTUAL_ADDR,2);
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
    VoC_bez16_r(BITN1_AMR_even,REG7)
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_shru16_ri(REG4,8,DEFAULT);

BITN1_AMR_even:

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_startloop0
    VoC_add16inc_rp(REG5,REG5,REG1,DEFAULT);
    VoC_shr16_ri(REG5,-1,DEFAULT);
    VoC_endloop0
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_add16_rd(REG7,REG7,CONST_AMR_1_ADDR);
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

    VoC_add16_rd(REG2,REG0,CONST_AMR_7_ADDR);

    VoC_bnez16_d(Ecgains_L10,DEC_AMR_BFI_ADDRESS);       //if (bfi == 0){



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
    VoC_add16_rd(REG2,REG0,CONST_AMR_7_ADDR);
    VoC_bnez16_d(Ecgains_L2,DEC_AMR_BFI_ADDRESS);            //if (bfi == 0)  {


    //    }
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

    VoC_lw16i(REG1,16384);

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

    VoC_be16_rd(Lspavg_lab1,REG7,CONST_AMR_80_ADDR);    //else if ( sub(i_subfr, 80) == 0 )
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
    VoC_be16_rd(Lspavg_lab2,REG7,CONST_AMR_120_ADDR);   //else if ( sub(i_subfr, 40) == 0 )

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
    VoC_sub16_dr(REG6,CONST_AMR_0_ADDR,REG6);
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
    VoC_add16inc_pd(REG5,REG0,CONST_AMR_205_ADDR);    //    lsf_min = add (lsf[i], min_dist);
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



//////////////////////////////////////////////////
//  REG0 for &cod[]
//  ACC0 for L_pn_seed_rx
//////////////////////////////////////////////////

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





/***********************************************************************
   Function: CII_rx_dtx_handler
   Input:    RXFrameType frame_type  ->REG7
   Output:   newState->REG4
   The parameters below need not transfer.
   Created by kenneth     07/20/2004
***********************************************************************/

void CII_rx_dtx_handler(void)
{
// start of rx_dtx_handler
    //else  {
    VoC_lw16i_short(REG4,0,DEFAULT);                //   newState = SPEECH;
    VoC_lw16i_short(REG2,0,DEFAULT);            //   st->since_last_sid = 0;
    //}

    VoC_lw16_d(REG3,STRUCT_DTX_DECSTATE_DTXGLOBALSTATE_ADDR);

    VoC_be16_rd(DtxDec_L8,REG7,CONST_AMR_7_ADDR);       //if ((sub(frame_type, RX_SID_FIRST) == 0)   ||
    VoC_bgt16_rd(DtxDec_L3,REG7,CONST_AMR_3_ADDR);      //    (sub(frame_type, RX_SID_UPDATE) == 0)  ||
DtxDec_L8:
    VoC_bez16_r(DtxDec_L5,REG3);                    //    (((sub(st->dtxGlobalState, DTX) == 0) ||
    VoC_bngt16_rd(DtxDec_L5,REG7,CONST_AMR_1_ADDR);     //     ((sub(frame_type, RX_NO_DATA) == 0) ||
DtxDec_L3:                                          //   {
    VoC_lw16i_short(REG4,1,DEFAULT);                //   newState = DTX;
    VoC_bne16_rd(DtxDec_L7,REG3,CONST_AMR_2_ADDR);      //   if ((sub(st->dtxGlobalState, DTX_MUTE) == 0) &&
    VoC_be16_rd(DtxDec_L7,REG7,CONST_AMR_3_ADDR);       //       ((sub(frame_type, RX_SID_BAD) == 0) ||
    VoC_be16_rd(DtxDec_L7,REG7,CONST_AMR_5_ADDR);       //        (sub(frame_type, RX_SID_FIRST) ==  0) ||
    //   {
    VoC_lw16i_short(REG4,2,DEFAULT);                //      newState = DTX_MUTE;   }
DtxDec_L7:

    VoC_lw16i_short(REG2,1,DEFAULT);                //   st->since_last_sid = add(st->since_last_sid, 1);
    VoC_add16_rd(REG2,REG2,STRUCT_DTX_DECSTATE_SINCE_LAST_SID_ADDR);

    VoC_be16_rd(DtxDec_L5,REG5,CONST_AMR_5_ADDR);       //   if((sub(frame_type, RX_SID_UPDATE) != 0) &&
    VoC_bngt16_rd(DtxDec_L5,REG2,CONST_AMR_50_ADDR);    //      (sub(st->since_last_sid, DTX_MAX_EMPTY_THRESH) > 0))
    VoC_lw16i_short(REG4,2,DEFAULT);                //   { newState = DTX_MUTE;   }
DtxDec_L5:
    VoC_sw16_d(REG2,STRUCT_DTX_DECSTATE_SINCE_LAST_SID_ADDR);

    VoC_lw16d_set_inc(REG3,STRUCT_DTX_DECSTATE_DECANAELAPSEDCOUNT_ADDR,1);

    VoC_bnez16_d(DtxDec_L9,STRUCT_DTX_DECSTATE_DATA_UPDATED_ADDR);//if ((st->data_updated == 0) &&
    VoC_bne16_rd(DtxDec_L9,REG7,CONST_AMR_5_ADDR);      //    (sub(frame_type, RX_SID_UPDATE) == 0))
    VoC_lw16i_short(REG3,0,DEFAULT);            //{ st->decAnaElapsedCount = 0;  }
DtxDec_L9:
    VoC_lw16i_short(REG1,0,DEFAULT);           //else   //{  encState = SPEECH; }
    VoC_add16_rd(REG3,REG3,CONST_AMR_1_ADDR);               //st->decAnaElapsedCount = add(st->decAnaElapsedCount, 1);

    VoC_be16_rd(DtxDec_L10,REG7,CONST_AMR_2_ADDR);      //if ((sub(frame_type, RX_SID_FIRST) == 0)  ||(sub(frame_type, RX_ONSET) == 0)      ||
    //    (sub(frame_type, RX_SID_UPDATE) == 0) ||
    VoC_blt16_rd(DtxDec_L11,REG7,CONST_AMR_4_ADDR);     //    (sub(frame_type, RX_SID_BAD) == 0)    ||(sub(frame_type, RX_NO_DATA) == 0))
DtxDec_L10:                         //{

    VoC_bne16_rd(DtxDec_L12,REG7,CONST_AMR_7_ADDR);     //   if((sub(frame_type, RX_NO_DATA) == 0) &&
    VoC_bez16_r(DtxDec_L11,REG4);           //      (sub(newState, SPEECH) == 0))
DtxDec_L12:
    VoC_lw16i_short(REG1,1,DEFAULT);            //   encState = DTX;
DtxDec_L11:

    VoC_lw16i_short(REG6,0,DEFAULT);            //st->dtxHangoverAdded = 0;
    VoC_lw16i_short(REG2,7,DEFAULT);            //{   st->dtxHangoverCount = DTX_HANG_CONST;

    VoC_bez16_r(DtxDec_L14,REG1);       //if (sub(encState, SPEECH) == 0)

    VoC_lw16i_short(REG6,1,DEFAULT);            //      st->dtxHangoverAdded = 1;

    VoC_bgt16_rd(DtxDec_L15,REG3,CONST_AMR_30_ADDR);    //   if (sub(st->decAnaElapsedCount, DTX_ELAPSED_FRAMES_THRESH) > 0){
    VoC_lw16i_short(REG6,0,DEFAULT);            //st->dtxHangoverAdded = 0;
    VoC_lw16i_short(REG2,1,IN_PARALLEL);
    VoC_sub16_dr(REG2,STRUCT_DTX_DECSTATE_DTXHANGOVERCOUNT_ADDR,REG2);      //   {     st->dtxHangoverCount = sub(st->dtxHangoverCount, 1); }
    VoC_bnez16_d(DtxDec_L14,STRUCT_DTX_DECSTATE_DTXHANGOVERCOUNT_ADDR);         //   else if (st->dtxHangoverCount == 0)
DtxDec_L15:
    VoC_lw32z(REG23,DEFAULT);           //     {  st->decAnaElapsedCount = 0;
    VoC_NOP();                          //}
DtxDec_L14:
    VoC_sw32_d(REG23,STRUCT_DTX_DECSTATE_DTXHANGOVERCOUNT_ADDR);

    VoC_bez16_r(DtxDec_L19,REG4);       //if (sub(newState, SPEECH) != 0)
    VoC_lw16i_short(REG2,1,DEFAULT);            //   {       st->sid_frame = 1;
    VoC_lw16i_short(REG3,0,IN_PARALLEL);            //    st->valid_data = 0;
    VoC_be16_rd(DtxDec_L22,REG7,CONST_AMR_4_ADDR);      //   if (sub(frame_type, RX_SID_FIRST) == 0)
    VoC_lw16i_short(REG3,1,DEFAULT);            //      st->valid_data = 1;
    VoC_be16_rd(DtxDec_L22,REG7,CONST_AMR_5_ADDR);      //   else if (sub(frame_type, RX_SID_UPDATE) == 0)
    VoC_lw32z(REG23,DEFAULT);          //{   st->sid_frame = 0;    //    st->valid_data = 0;
    VoC_bne16_rd(DtxDec_L22,REG7,CONST_AMR_6_ADDR);     //   else if (sub(frame_type, RX_SID_BAD) == 0)
    VoC_lw16i_short(REG2,1,DEFAULT);            //   {       st->sid_frame = 1;
    VoC_lw16i_short(REG6,0,DEFAULT);            //      st->dtxHangoverAdded = 0;
DtxDec_L22:                         //   }
    VoC_sw32_d(REG23,STRUCT_DTX_DECSTATE_SID_FRAME_ADDR);//}
DtxDec_L19:

    VoC_sw16_d(REG6,STRUCT_DTX_DECSTATE_DTXHANGOVERADDED_ADDR);   //return newState;  in REG4
// end of  rx_dtx_handler
    VoC_return;
}

/***************************************************************************
*    Function    : CII_dtx_dec
*
     Parameters:
     dtx_decState *st,
     Word16 mem_syn[],
     D_plsfState* lsfState,
     gc_predState* predState,
     Cb_gain_averageState* averState,
     enum DTXStateType new_state,
     enum Mode mode,                    push16 stack[n+4]
     Word16 parm[],
     Word16 synth[],                    push16 stack[n+2]
     Word16 A_t[]

*    Version 1.0  Create by Kenneth    07/30/2004     debug and optimize
     Version 1.1  Optimized by Kenneth 09/24/2004
**************************************************************************/


//*in0 in REG5
//*in1 in REG1
//data in REG2


// vad_flag in REG7
// return in REG5



/*************************************************************************
 *   Copyright 2004, CII Technologies Inc
 * FUNCTION:  gc_pred()
 *
 * PURPOSE: MA prediction of the innovation energy
 *          (in dB/(20*log10(2))) with mean  removed).

 *INPUT:    REG7:  mode
*
*              REG1 :  st

*  OUTPUT:
               REG7:     exp_gcode0
                REG6:    frac_gcode0
                ACC1_HI :   frac_en
                 REG1:         exp_en

 *
 * Version 1.0  Create by Cui   07/09/2004

 *************************************************************************/
void     CII_gc_pred(void)

{
    VoC_push16(RA, DEFAULT);
    VoC_lw32z(ACC0,DEFAULT);

    VoC_lw16i_set_inc(REG0,COD_AMR_CODE_ADDRESS, 2);
    VoC_loop_i_short(20, DEFAULT);
    VoC_push16(REG1, IN_PARALLEL);
    VoC_startloop0
    VoC_bimac32inc_pp(REG0, REG0);
    VoC_endloop0

    /*  VoC_jal(CII_NORM_L_ACC0);  // reg1   exp_code

    //  VoC_movreg32(ACC1,ACC0,DEFAULT); //acc1 :ener_code        // save val ACC0 in ACC1    ener_code
        VoC_push16(REG1, DEFAULT);

        VoC_jal(CII_Log2_norm);  //  REG4:EXP          REG5:FRAC
        VoC_pop16(REG1, DEFAULT);
    */  VoC_jal(CII_Log2);  //  REG4:EXP          REG5:FRAC

    VoC_lw16i(REG2, -24660);
    VoC_multf32_rr(ACC0, REG4, REG2,DEFAULT);

    VoC_multf16_rr(REG5, REG5,  REG2,DEFAULT);
    VoC_lw16i_set_inc(REG2, 64,1);  // MR59, MR515, MR475,MR102 guosz
    VoC_mac32_rd(REG5,CONST_AMR_1_ADDR);


    VoC_lw16i(REG0,16678);
    VoC_bne16_rd(label_no_MR795, REG7, CONST_AMR_5_ADDR)    // ACC1_HI : frac_en
    VoC_sub16_dr(REG1, CONST_AMR_NEG11_ADDR, REG1);     //   REG1:         exp_en
    VoC_lw16i(REG0, 17062);
    VoC_jump(label_tes_end)
label_no_MR795:
    VoC_bne16_rd(label_no_MR74, REG7, CONST_AMR_4_ADDR)
    VoC_lw16i(REG2, 32);
    VoC_lw16i(REG0, 32588);
    VoC_jump(label_tes_end)  //ACC0   L_tmp
label_no_MR74:
    VoC_bne16_rd(label_tes_end, REG7, CONST_AMR_3_ADDR)
    VoC_lw16i(REG0, 32268);
    VoC_lw16i(REG2, 32);
label_tes_end:
    VoC_mac32_rr(REG0, REG2,DEFAULT);
    //ACC0   L_tmp
    VoC_lw16i_set_inc(REG0, STATIC_CONST_pred_ADDR, 1);

    VoC_pop16(REG2, DEFAULT);

    //  REG2  :    st->past_qua_en
    VoC_loop_i_short(4, DEFAULT);
    VoC_shr32_ri(ACC0, -10, IN_PARALLEL);
    VoC_startloop0
    VoC_mac32inc_pp(REG0, REG2, DEFAULT);
    VoC_endloop0

    VoC_lw16i(REG2, 5443);
    VoC_movreg16(REG5, REG7,DEFAULT);
    VoC_movreg16(REG0, ACC0_HI,IN_PARALLEL);

    VoC_multf32_rr(REG67, REG0, REG2,DEFAULT);
    VoC_sub16_rr(REG2, REG2, REG5,IN_PARALLEL);    //   5439

    VoC_bne16_rd(label_no_MR74_2, REG5, CONST_AMR_4_ADDR)
    VoC_multf32_rr(REG67, REG0, REG2,DEFAULT);
label_no_MR74_2:
    VoC_pop16(RA, DEFAULT);
    VoC_shr32_ri(REG67, 8, DEFAULT);


    VoC_shru16_ri(REG6, 1,DEFAULT);
//  VoC_and16_ri(REG6,0x7fff);
    VoC_return;
}




/***************************************************************************
 *                                                                          *
 *  Function:  check_lsp()                                                  *
 *  Purpose:   Check the LSP's to detect resonances                         *
 *                                                                          *
 ****************************************************************************
 */




void CII_AMR_subfunc0(void)
{
    VoC_push16(RA,DEFAULT);
    /*******************************************************************************
      Function:  CII_Bgn_scd
      input:  Word16 speech[]        ->REG3 (INCR=2)
      Version 1.0 created by Kenneth  07/19/2004
      Version 1.1 Optimise by Kenneth 08/12/2004
    *******************************************************************************/
//   start of Bgn_scd
    //currEnergy = 0;
    VoC_lw32z(ACC0,DEFAULT);     //s = (Word32) 0;
    VoC_loop_i(0,80);            //for (i = 0; i < L_FRAME; i++)
    VoC_bimac32inc_pp(REG3,REG3);    //{ s = L_mac (s, speech[i], speech[i]);
    VoC_endloop0;            //}

    VoC_lw16i_set_inc(REG0,STRUCT_BGN_SCDSTATE_FRAMEENERGYHIST_ADDR,1);
    VoC_shr32_ri(ACC0,-2,DEFAULT);               //s = L_shl(s, 2);
    VoC_push16(REG0,DEFAULT);                    // push the address of frameEnergyHist[]

    VoC_movreg32(REG67,ACC0,IN_PARALLEL);                //currEnergy = extract_h (s);   REG7->currEnergy
    VoC_lw16i(REG6,0x7FFF);                  //frameEnergyMin = 32767;
    VoC_loop_i_short(60,DEFAULT);
    VoC_lw16inc_p(REG5,REG0,IN_PARALLEL);                //for (i = 0; i < L_ENERGYHIST; i++)
    VoC_startloop0;              //{
    VoC_bngt16_rr(Bgnscd_LA1,REG6,REG5);            //   if (sub(st->frameEnergyHist[i], frameEnergyMin) < 0)
    VoC_movreg16(REG6,REG5,DEFAULT);
Bgnscd_LA1:                             //      frameEnergyMin = st->frameEnergyHist[i];
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    VoC_endloop0;                        //}

    VoC_shr16_ri(REG6,-4,DEFAULT);               //noiseFloor = shl (frameEnergyMin, 4);  REG6->noiseFloor
    VoC_lw16_sd(REG0,0,IN_PARALLEL);             //maxEnergy = st->frameEnergyHist[0];
    VoC_lw16i_set_inc(REG1,STRUCT_BGN_SCDSTATE_BGHANGOVER_ADDR,1);
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    VoC_loop_i_short(55,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);               //for (i = 1; i < L_ENERGYHIST-4; i++)
    VoC_startloop0;              //{
    VoC_bngt16_rr(Bgnscd_LA2,REG4,REG5);         //   if ( sub (maxEnergy, st->frameEnergyHist[i]) < 0)
    VoC_movreg16(REG5,REG4,DEFAULT);             //   { maxEnergy = st->frameEnergyHist[i];
Bgnscd_LA2:                             //   }
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_endloop0;                        //}          REG5->maxEnergy

    VoC_lw16_sd(REG0,0,DEFAULT);                 //maxEnergyLastPart = st->frameEnergyHist[2*L_ENERGYHIST/3]; move16 ();
    VoC_add16_rd(REG0,REG0,CONST_AMR_40_ADDR);
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_loop_i_short(19,DEFAULT);
    VoC_lw16inc_p(REG3,REG0,IN_PARALLEL);             //for (i = 2*L_ENERGYHIST/3+1; i < L_ENERGYHIST; i++)
    VoC_startloop0;                      //{
    VoC_bngt16_rr(Bgnscd_LA3,REG3,REG4);         //   if ( sub (maxEnergyLastPart, st->frameEnergyHist[i] ) < 0)
    VoC_movreg16(REG4,REG3,DEFAULT);             //   {  maxEnergyLastPart = st->frameEnergyHist[i];
Bgnscd_LA3:                             //   }      REG4->maxEnergyLastPart
    VoC_lw16inc_p(REG3,REG0,DEFAULT);
    VoC_endloop0;

    VoC_lw16i_short(REG0,1,DEFAULT);                 //}
    VoC_lw16i_set_inc(REG3,1953,1);
    VoC_bngt16_rd(Bgnscd_LA5,REG5,CONST_AMR_20_ADDR);            //if ( (sub(maxEnergy, LOWERNOISELIMIT) > 0) &&
    VoC_bngt16_rd(Bgnscd_LA5,REG7,CONST_AMR_20_ADDR);            //     (sub(currEnergy, FRAMEENERGYLIMIT) < 0) &&
    VoC_bgt16_rd(Bgnscd_LA5,REG7,CONST_AMR_17578_ADDR);              //     (sub(currEnergy, LOWERNOISELIMIT) > 0) &&                    <1953>
    VoC_bgt16_rr(Bgnscd_LA4,REG6,REG7);              //     ( (sub(currEnergy, noiseFloor) < 0) ||(sub(maxEnergyLastPart, UPPERNOISELIMIT) < 0)))
    VoC_bngt16_rr(Bgnscd_LA5,REG3,REG4);
Bgnscd_LA4:                     //{
    VoC_add16_rp(REG2,REG0,REG1,DEFAULT);
    VoC_bngt16_rd(Bgnscd_LA5,REG2,CONST_AMR_30_ADDR);            //   if (sub(add(st->bgHangover, 1), 30) > 0)
    VoC_lw16i(REG2,30);              //   {  st->bgHangover = 30;
    //   }
    //else
    //   {st->bgHangover = add(st->bgHangover, 1);  }
    //}
    //else
    //{ st->bgHangover = 0;      }
Bgnscd_LA5:
    VoC_lw16i(REG6, 59);

    VoC_bgt16_rd(Bgnscd_LAEnd,REG2,CONST_AMR_1_ADDR);        //if (sub(st->bgHangover,1) > 0)
    VoC_lw16i_short(REG0,0,DEFAULT);             //   inbgNoise = 1;
Bgnscd_LAEnd:
    VoC_lw16_sd(REG3,0,DEFAULT);
    VoC_sw16_p(REG2,REG1,DEFAULT);
    VoC_sw16_d(REG0,STRUCT_DECOD_AMRSTATE_INBACKGROUNDNOISE_ADDR);

    VoC_add16_rd(REG0,REG3,CONST_AMR_1_ADDR);            //for (i = 0; i < L_ENERGYHIST-1; i++)
    VoC_jal(CII_R02R3_p_R6);             //{ st->frameEnergyHist[i] = st->frameEnergyHist[i+1];
    //}
    VoC_lw16i(REG6,13926);                   //ltpLimit = 13926;
    VoC_sw16_p(REG7,REG3,DEFAULT);               //st->frameEnergyHist[L_ENERGYHIST-1] = currEnergy;

    VoC_bngt16_rd(Bgnscd_LA8,REG2,CONST_AMR_8_ADDR);         //if (sub(st->bgHangover, 8) > 0)
    VoC_lw16i(REG6,15565);                   //{   ltpLimit = 15565;
Bgnscd_LA8:
    VoC_bngt16_rd(Bgnscd_LA9,REG2,CONST_AMR_15_ADDR);            //if (sub(st->bgHangover, 15) > 0)
    VoC_lw16i(REG6,16383);                   //{   ltpLimit = 16383; }
Bgnscd_LA9:                             //  REG6->ltpLimit

    VoC_bgt16_rd(Bgnscd_Lab11,REG2,CONST_AMR_20_ADDR);//if (sub(st->bgHangover, 20) <= 0)
    //prevVoiced = 0;
    VoC_lw16i_set_inc(REG0,STRUCT_DECOD_AMRSTATE_LTPGAINHISTORY_ADDR+4,1);//if (sub(gmed_n(&ltpGainHist[4], 5), ltpLimit) > 0)
    VoC_push16(REG6,DEFAULT);
    VoC_lw16i_short(REG5,5,IN_PARALLEL);
    VoC_jal(CII_gmed_n);                 // Return REG1
    VoC_pop16(REG6,DEFAULT);             //{
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_bgt16_rr(Bgnscd_LA15,REG1,REG6);     //   prevVoiced = 1;
    VoC_jump(Bgnscd_Lab15);
Bgnscd_Lab11:                       //}
    VoC_push16(REG6,DEFAULT);
    VoC_lw16i_short(REG5,9,DEFAULT);
    VoC_lw16i_set_inc(REG0,STRUCT_DECOD_AMRSTATE_LTPGAINHISTORY_ADDR,1);    //{*voicedHangover = temp; }
    VoC_jal(CII_gmed_n);         // if (sub(gmed_n(ltpGainHist, 9), ltpLimit) > 0)
    VoC_pop16(REG6,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_bgt16_rr(Bgnscd_LA15,REG1,REG6);
Bgnscd_Lab15:
    VoC_lw16i_short(REG5,1,DEFAULT);     // prevVoiced = 0;
    VoC_add16_rd(REG7,REG5,STRUCT_DECOD_AMRSTATE_VOICEDHANGOVER_ADDR);       //temp = add(*voicedHangover, 1);
    VoC_bngt16_rd(Bgnscd_LA15,REG7,CONST_AMR_10_ADDR);   //if (sub(temp, 10) > 0)
    VoC_lw16i_short(REG7,10,DEFAULT);            //{  *voicedHangover = 10;}
    //else
    // {
    //    prevVoiced = 1;  *voicedHangover = 0;
Bgnscd_LA15:        // }
    VoC_pop16(REG0,DEFAULT);
    VoC_sw16_d(REG7,STRUCT_DECOD_AMRSTATE_VOICEDHANGOVER_ADDR);
//   end of  Bgn_scd
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_lw16_sd(REG2,2,IN_PARALLEL);
    // dtx_dec_activity_update(st->dtxDecoderState, st->lsfState->past_lsf_q,synth);
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /***********************************************************************
       Function: CII_dtx_dec_activity_update
       Input: Word16 frame[]  ->REG2(incr 2)
       The parameters below need not transfer.
       parameter:dtx_decState *st->
                    st->lsf_hist_ptr   :STRUCT_DTX_DECSTATE_LSF_HIST_PTR_ADDR
                    st->lsf_hist       :STRUCT_DTX_DECSTATE_LSF_HIST_ADDR
                    st->log_en_hist_ptr:STRUCT_DTX_DECSTATE_LOG_EN_HIST_PTR_ADDR
                    st->log_en_hist    :STRUCT_DTX_DECSTATE_LOG_EN_HIST_ADDR
                 Word16 lsf[]: STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR
       Created by kenneth     07/20/2004
       Optimized by Kenneth   09/23/2004
    ***********************************************************************/
//   start of dtx_dec_activity_update
    VoC_lw16i_short(REG7,10,DEFAULT);
    VoC_add16_rd(REG7,REG7,STRUCT_DTX_DECSTATE_LSF_HIST_PTR_ADDR);      //st->lsf_hist_ptr = add(st->lsf_hist_ptr,M);
    VoC_bne16_rd(DtxDec_L1,REG7,CONST_AMR_80_ADDR);     //if (sub(st->lsf_hist_ptr, 80) == 0)
    VoC_lw16i_short(REG7,0,DEFAULT);            //{   st->lsf_hist_ptr = 0;
DtxDec_L1:                          //}
    VoC_lw16i_set_inc(REG0,STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR,2);
    VoC_lw16i_set_inc(REG1,STRUCT_DTX_DECSTATE_LSF_HIST_ADDR,2);
    VoC_sw16_d(REG7,STRUCT_DTX_DECSTATE_LSF_HIST_PTR_ADDR);
    VoC_jal(CII_dtx_com_s2);
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_add16_rd(REG3,REG3,STRUCT_DTX_DECSTATE_LOG_EN_HIST_PTR_ADDR); //st->log_en_hist_ptr = add(st->log_en_hist_ptr, 1);
    VoC_bne16_rd(DtxDec_L2,REG3,CONST_AMR_8_ADDR);      //if (sub(st->log_en_hist_ptr, DTX_HIST_SIZE) == 0)
    VoC_lw16i_short(REG3,0,DEFAULT);            //{   st->log_en_hist_ptr = 0;
DtxDec_L2:                              //}
    VoC_lw16i_set_inc(REG1,STRUCT_DTX_DECSTATE_LOG_EN_HIST_ADDR,1);
    VoC_add16_rr(REG1,REG3,REG1,DEFAULT);

    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG3,STRUCT_DTX_DECSTATE_LOG_EN_HIST_PTR_ADDR);

    VoC_sw16_p(REG4,REG1,DEFAULT);          //st->log_en_hist[st->log_en_hist_ptr] = log_en;
    VoC_return;
}




void CII_Post_Process(void)
{
    VoC_movreg16(REG4,REG0,DEFAULT);

    VoC_loop_i(1,160);              //for (i = 0; i < 160; i++)
    VoC_movreg16(REG0,REG4,DEFAULT);    //{

    VoC_lw16i_set_inc(REG2,STATIC_CONST_a_60Hz_ADDR+2,-1);
    VoC_lw16i_set_inc(REG3,STATIC_CONST_b_60Hz_ADDR,2);

    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_loop_i_short(2,DEFAULT)
    VoC_startloop0
    VoC_mac32_rp(REG7,REG2,DEFAULT);    //    L_tmp = L_mult (st->y1_hi, a[1]);
    VoC_multf16inc_rp(REG5,REG6,REG2,DEFAULT);
    VoC_NOP();
    VoC_mac32_rd(REG5,CONST_AMR_1_ADDR);    //    L_tmp = L_mac (L_tmp, mult (st->y1_lo, a[1]), 1);
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_endloop0

    VoC_movreg16(REG5,REG7,DEFAULT);

    VoC_lw16_p(REG6,REG1,DEFAULT);
    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    //    x2 = st->x1;
    //    st->x1 = st->x0;
    //    st->x0 = signal[i];
    VoC_bimac32inc_rp(REG67,REG3);  //    L_tmp = L_mac (L_tmp, st->x0, b[0]);

    VoC_mac32inc_rp(REG5,REG3,DEFAULT); //    L_tmp = L_mac (L_tmp, x2, b[2]);


    VoC_add16_rd(REG0,REG4,CONST_AMR_2_ADDR);

    VoC_shr32_ri(ACC0,-2,DEFAULT);      //    L_tmp = L_shl (L_tmp, 2);


    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_lw32_p(RL6,REG0,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw32_d(REG67,STRUCT_POST_PROCESSSTATE_ADDR+4);
    exit_on_warnings=ON;

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);      //    signal[i] = round(L_shl(L_tmp, 1));

//          VoC_and16_ri(REG6,0x7fff);
    VoC_add32_rd(ACC0,ACC0,CONST_AMR_0x00008000_ADDR);

    VoC_sw32_p(RL6,REG4,DEFAULT);       //    st->y2_hi = st->y1_hi;
    VoC_sw32_p(REG67,REG0,DEFAULT);         //    st->y2_lo = st->y1_lo;
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    //    L_Extract (L_tmp, &st->y1_hi, &st->y1_lo);
    VoC_endloop1        //}
    VoC_return;
}




void CII_amr_dec_122(void)
{

#if 0


    voc_short    DEC_AMR_TEMP0_ADDRESS                         ,x
    voc_short    DEC_AMR_TEMP1_ADDRESS                         ,x
    voc_short    DEC_AMR_TEMP2_ADDRESS                         ,x
    voc_short    DEC_AMR_TEMP3_ADDRESS                         ,x
    voc_short    DEC_AMR_TEMP4_ADDRESS                         ,x
    voc_short    DEC_AMR_TEMP5_ADDRESS                         ,x
    voc_short    DEC_AMR_TEMP6_ADDRESS                         ,x
    voc_short    DEC_AMR_TEMP7_ADDRESS                         ,x
    voc_short    DEC_AMR_TEMP8_ADDRESS                         ,x
    voc_short    DEC_AMR_TEMP9_ADDRESS                         ,x
    voc_short    DEC_AMR_TEMP10_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP11_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP12_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP13_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP14_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP15_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP16_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP17_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP18_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP19_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP20_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP21_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP22_ADDRESS                        ,x

#endif


    VoC_push16(RA,DEFAULT);

    VoC_push16(REG1,DEFAULT);           // the pointer for parm  push16 stack[n-1]
    VoC_push16(REG0,DEFAULT);           // push16 stack[n-2] st->lsfState

    //{    D_plsf_5 (st->lsfState, bfi, parm, lsp_mid, lsp_new);
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /********************************************************************************
     Function:  void CII_D_plsf_5(void)
      input: bfi     ->REG6
             * indice->REG1 (incr 1)(push REG1)    stack[n]
         *st     ->REG0 (incr 1)(push REG0)    stack[n-1]
      output:
             *st
             *lsp1_q->DEC_AMR_LSP_MID_ADDRESS(direct address access)
             *lsp2_q->DEC_AMR_LSP_NEW_ADDRESS(direct address access)
     Version 1.0 Created by Kenneth  07/09/2004
     Version 1.1 Optimized by Kenneth 08/12/2004
     Version 1.2 revised by Kenneth 09/27/2004
    ********************************************************************************/
//  start of D_plsf_5




    VoC_lw16i_set_inc(REG2,TABLE_MEAN_LSF_ADDR,1);
    VoC_lw16i_set_inc(REG3,DEC_AMR_TEMP0_ADDRESS,1);
    VoC_push16(REG2,DEFAULT);               //stack[n-2]

    VoC_bez16_r(Dplsf5_L1,REG6)             //if (bfi != 0){
    VoC_lw16i(REG6,1639);

    VoC_loop_i_short(10,DEFAULT);               //for (i = 0; i < M; i++)
    VoC_lw16i_short(REG7,9,IN_PARALLEL);
    VoC_startloop0                          //{

    VoC_multf16_rp(REG4,REG6,REG2,DEFAULT);
    VoC_multf16inc_pd(REG5,REG0,CONST_AMR_31128_ADDR);  //    lsf1_q[i] = add (mult (st->past_lsf_q[i], ALPHA),mult (mean_lsf[i], ONE_ALPHA));

    VoC_add16_rr(REG1,REG0,REG7,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);
    VoC_multf16_pd(REG4,REG1,CONST_AMR_21299_ADDR); //    temp = add (mean_lsf[i], mult (st->past_r_q[i],LSP_PRED_FAC_MR122));
    VoC_sw16inc_p(REG5,REG3,DEFAULT);       //    lsf2_q[i] = lsf1_q[i];
    VoC_add16inc_rp(REG4,REG4,REG2,DEFAULT);
    VoC_sub16_rr(REG4,REG5,REG4,DEFAULT);       //    st->past_r_q[i] = sub (lsf2_q[i], temp);

    VoC_add16_rr(REG4,REG3,REG7,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16_p(REG4,REG1,DEFAULT);      //}
    exit_on_warnings=ON;
    VoC_sw16_p(REG5,REG4,DEFAULT);

    VoC_endloop0
    VoC_jump(Dplsf5_LEnd);              //}
Dplsf5_L1:                          //else   {

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);


    VoC_lw16i(REG7,TABLE_DICO1_LSF_ADDR);   //p_dico = &dico1_lsf[shl (indice[0], 2)];
    //lsf1_r[0] = *p_dico++;  lsf1_r[1] = *p_dico++;
    //lsf2_r[0] = *p_dico++;  lsf2_r[1] = *p_dico++;
    //p_dico = &dico2_lsf[shl (indice[1], 2)];
    //lsf1_r[2] = *p_dico++;   lsf1_r[3] = *p_dico++;

    VoC_loop_i_short(2, DEFAULT);
    VoC_startloop0
    VoC_jal(CII_D_plsf_5_sub);
    VoC_lw16i(REG7,TABLE_DICO2_LSF_ADDR);
    VoC_endloop0
    //lsf2_r[2] = *p_dico++;   lsf2_r[3] = *p_dico++;
    VoC_lw16i_short(REG2,1,DEFAULT);        //sign = indice[2] & 1;
    VoC_lw16inc_p(REG0,REG1,IN_PARALLEL);
    VoC_and16_rr(REG2,REG0,DEFAULT);

    VoC_shr16_ri(REG0,1,DEFAULT);           //i = shr (indice[2], 1);
    VoC_shr16_ri(REG0,-2,DEFAULT);          //p_dico = &dico3_lsf[shl (i, 2)];

    VoC_lw16i(REG4,TABLE_DICO3_LSF_ADDR );
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);

    VoC_NOP();
    //{
    VoC_lw32inc_p(REG45,REG0,DEFAULT);      //   lsf1_r[4] = *p_dico++;   lsf1_r[5] = *p_dico++;
    VoC_lw32inc_p(REG67,REG0,DEFAULT);      //   lsf2_r[4] = *p_dico++;   lsf2_r[5] = *p_dico++;

    VoC_bez16_r(D_plsf5_L2,REG2);       //if (sign == 0)

    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_sub16_rr(REG4,REG2,REG4,DEFAULT);       //}
    VoC_sub16_rr(REG5,REG2,REG5,IN_PARALLEL);   //else{
    VoC_sub16_rr(REG6,REG2,REG6,DEFAULT);   // lsf1_r[4] = negate (*p_dico++); lsf1_r[5] = negate (*p_dico++);
    VoC_sub16_rr(REG7,REG2,REG7,IN_PARALLEL);   // lsf2_r[4] = negate (*p_dico++); lsf2_r[5] = negate (*p_dico++);
D_plsf5_L2:
    VoC_add16_rd(REG2,REG3,CONST_AMR_10_ADDR);
    VoC_sw32inc_p(REG45,REG3,DEFAULT);
    VoC_sw32_p(REG67,REG2,DEFAULT);         //}

    VoC_lw16i(REG7,TABLE_DICO4_LSF_ADDR);       //p_dico = &dico4_lsf[shl (indice[3], 2)];
    //lsf1_r[6] = *p_dico++;   lsf1_r[7] = *p_dico++;
    //lsf2_r[6] = *p_dico++;   lsf2_r[7] = *p_dico++;
    //p_dico = &dico5_lsf[shl (indice[4], 2)];
    //lsf1_r[8] = *p_dico++;    lsf1_r[9] = *p_dico++;
    VoC_loop_i_short(2, DEFAULT);
    VoC_startloop0
    VoC_jal(CII_D_plsf_5_sub);
    VoC_lw16i(REG7,TABLE_DICO5_LSF_ADDR);
    VoC_endloop0

    //lsf2_r[8] = *p_dico++;    lsf2_r[9] = *p_dico++;
    VoC_lw16_sd(REG1,1,DEFAULT);
    VoC_lw16i_short(REG7,10,IN_PARALLEL);
    VoC_lw16i_set_inc(REG3,DEC_AMR_TEMP0_ADDRESS,1);
    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_lw16_sd(REG2,0,DEFAULT);            // load mean_lsf



    VoC_multf16_pd(REG4,REG1,CONST_AMR_21299_ADDR);
    VoC_loop_i_short(10,DEFAULT);               //    for (i = 0; i < M; i++)
    VoC_startloop0                      //    {
//      VoC_multf16_pd(REG4,REG1,CONST_21299_ADDR);     //        temp = add (mean_lsf[i], mult (st->past_r_q[i],LSP_PRED_FAC_MR122));
    VoC_add16inc_rp(REG4,REG4,REG2,DEFAULT);
    VoC_add16_rp(REG5,REG4,REG3,DEFAULT);       //        lsf1_q[i] = add (lsf1_r[i], temp);
    VoC_add16_rr(REG6,REG3,REG7,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_lw16_p(REG5,REG6,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);           //        lsf2_q[i] = add (lsf2_r[i], temp);
    VoC_sw16inc_p(REG5,REG1,DEFAULT);       //        st->past_r_q[i] = lsf2_r[i];
    VoC_multf16_pd(REG4,REG1,CONST_AMR_21299_ADDR);
    exit_on_warnings = OFF;
    VoC_sw16_p(REG4,REG6,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop0                    //    }
Dplsf5_LEnd:                            //}

    VoC_lw16i_set_inc(REG0,DEC_AMR_TEMP0_ADDRESS,1);    //Reorder_lsf (lsf1_q, LSF_GAP, M);

    VoC_jal(CII_Reorder_lsf);

    VoC_jal(CII_Reorder_lsf);                   //Reorder_lsf (lsf2_q, LSF_GAP, M);  //Note: REG0->DEC_AMR_TEMP0_ADDRESS+10




    VoC_lw16i_set_inc(REG0,DEC_AMR_TEMP10_ADDRESS,1);   //Copy (lsf2_q, st->past_lsf_q, M);
    VoC_lw16_sd(REG3,1,DEFAULT);                // load  & st->past_lsf_q[]
    VoC_loop_i_short(10,DEFAULT);
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    // restore lsf1_q
    VoC_lw16i(REG0,DEC_AMR_TEMP0_ADDRESS);          //Lsf_lsp (lsf1_q, lsp1_q, M);
    VoC_lw16i_set_inc(REG1,DEC_AMR_LSP_MID_ADDRESS,1);
    VoC_jal(CII_Lsf_lsp);

    VoC_lw16i_set_inc(REG0,DEC_AMR_TEMP10_ADDRESS,1);   //Lsf_lsp (lsf2_q, lsp2_q, M);
    VoC_lw16i_set_inc(REG1,DEC_AMR_LSP_NEW_ADDRESS,1);
    VoC_jal(CII_Lsf_lsp);

    VoC_pop16(REG0,DEFAULT);
//  end of D_plsf_5
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    VoC_lw16_sd(REG7,1,DEFAULT);            //   parm += 5;
    VoC_lw16i_short(REG0,5,IN_PARALLEL);
    VoC_add16_rr(REG7,REG7,REG0,DEFAULT);
    VoC_lw32_sd(REG23,0,IN_PARALLEL);       // load  &A_t[]/&lsp_mid[]
    VoC_lw32_sd(REG45,1,DEFAULT);           // load  &lsp_new[]/&lsp_old[]
    //  VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_sw16_sd(REG7,1,DEFAULT);



    VoC_movreg16(REG0,REG3,DEFAULT);
    VoC_movreg16(REG3,REG4,IN_PARALLEL);
    VoC_jal(CII_Int_lpc_1and3_dec);             //   Int_lpc_1and3 (st->lsp_old, lsp_mid, lsp_new, A_t);



    VoC_pop32(RL6,DEFAULT);             // pop32 stack32[n-1]
    VoC_pop16(REG2,IN_PARALLEL);            // pop16 stack[n-2] st->lsfState
    VoC_pop32(REG45,DEFAULT);           // pop32 stack32[n]

    VoC_movreg16(REG0,REG5,DEFAULT);
    VoC_movreg16(REG1,REG4, IN_PARALLEL );      //for (i = 0; i < M; i++)
    VoC_jal(CII_Copy_M);                //{st->lsp_old[i] = lsp_new[i];//}

    VoC_lw16i_short(REG5,0,DEFAULT);        //evenSubfr = 0;
    VoC_lw16i_short(REG6,-1,IN_PARALLEL);       //subfrNr = -1;
    VoC_push16(RL6_HI,DEFAULT);         //Az = A_t;     Az in stack[n-2]
    VoC_sw16_d(REG5,DEC_AMR_EVENSUBFR_ADDRESS);
    VoC_sw16_d(REG6,DEC_AMR_SUBFRNR_ADDRESS);
Dec_amr_LoopStart_122:
    VoC_bne16_rd(Dec_amr_LoopEnd_1221,REG5,CONST_AMR_160_ADDR);     //for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
    VoC_jump(Dec_amr_LoopEnd_122);
Dec_amr_LoopEnd_1221:
    VoC_lw16i_short(REG3,1,DEFAULT);        //{

    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_push16(REG5,DEFAULT);               //   i_subfr  push16 stack[n-3]
    VoC_add16_rd(REG6,REG3,DEC_AMR_SUBFRNR_ADDRESS);    //   subfrNr = add(subfrNr, 1);
    VoC_sub16_rd(REG4,REG3,DEC_AMR_EVENSUBFR_ADDRESS);  //evenSubfr = sub(1, evenSubfr);
    VoC_sw16_d(REG6,DEC_AMR_SUBFRNR_ADDRESS);
    VoC_sw16_d(REG4,DEC_AMR_EVENSUBFR_ADDRESS);     //   pit_flag = i_subfr;

    VoC_bne16_rd(Dec_amr_L14_122,REG5,CONST_AMR_80_ADDR);   //   if (sub (i_subfr, L_FRAME_BY2) == 0)
    //   { if (sub(mode, MR475) != 0 && sub(mode, MR515) != 0)
    VoC_lw16i_short(REG5,0,DEFAULT);            //      {  pit_flag = 0;  }
Dec_amr_L14_122:
    VoC_lw16_sd(REG0,2,DEFAULT);            // assure REG0 incr=1
    VoC_sw16_d(REG5,DEC_AMR_PIT_FLAG_ADDRESS);
    VoC_lw16inc_p(REG3,REG0,DEFAULT);           //index = *parm++;
    VoC_lw16_d(REG4,STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR);
    VoC_sw16_sd(REG0,2,DEFAULT);            // restore the new address of parm
    VoC_sw16_d(REG3,DEC_AMR_INDEX_ADDRESS);



    // begin of dec_amr_122_s2
    // {
    VoC_lw16_d(REG6,DEC_AMR_T0_ADDRESS);            //   Dec_lag6 (index, PIT_MIN_MR122, PIT_MAX, pit_flag, &T0, &T0_frac);
    //  begin of CII_Dec_lag6
    VoC_lw16i_short(REG2,5,DEFAULT);
    VoC_bnez16_d(Dec_lag6_L1,DEC_AMR_PIT_FLAG_ADDRESS);     //if (i_subfr == 0)

    VoC_lw16i_short(REG7,0,DEFAULT);    //   {    *T0_frac = 0;
    VoC_sub16_rd(REG6,REG3,CONST_AMR_368_ADDR);  //            *T0 = sub (index, 368);
    VoC_bnlt16_rd(Dec_lag6_L2,REG3,CONST_AMR_463_ADDR);     // {  if (sub (index, 463) < 0)

    VoC_add16_rr(REG6,REG3,REG2,DEFAULT);       //   {
    VoC_multf16_rd(REG6,REG6,CONST_AMR_5462_ADDR);          //      *T0 = add (mult (add (index, 5), 5462), 17);
    VoC_NOP();
    VoC_add16_rd(REG6,REG6,CONST_AMR_17_ADDR);

    VoC_mult16_rd(REG5,REG6,CONST_AMR_6_ADDR);      //      i = add (add (*T0, *T0), *T0);
    VoC_NOP();
    VoC_sub16_rr(REG5,REG3,REG5,DEFAULT);       //      *T0_frac = add (sub (index, add (i, i)), 105);
    VoC_add16_rd(REG7,REG5,CONST_AMR_105_ADDR);         //   }

Dec_lag6_L2:                        //   else
    VoC_jump(Dec_lag6_end);                                 //   }}
Dec_lag6_L1:                        //else{

    VoC_sub16_rr(REG5,REG6,REG2,DEFAULT);       //   T0_min = sub (*T0, 5);
    VoC_bnlt16_rd(Dec_lag6_L3,REG5,CONST_AMR_18_ADDR);      //   if (sub (T0_min, pit_min) < 0)
    VoC_lw16i_short(REG5,18,DEFAULT);           //   {    T0_min = pit_min;
Dec_lag6_L3:                        //   }
    //   T0_max = add (T0_min, 9);
    VoC_bngt16_rd(Dec_lag6_L4,REG5,CONST_AMR_134_ADDR);     //   if (sub (T0_max, pit_max) > 0)
    VoC_lw16i(REG5,134);                    //   {   T0_max = pit_max;
    //      T0_min = sub (T0_max, 9);
Dec_lag6_L4:                        //   }
    VoC_add16_rr(REG2,REG3,REG2,DEFAULT);       //   i = sub (mult (add (index, 5), 5462), 1);
    VoC_multf16_rd(REG2,REG2,CONST_AMR_5462_ADDR);

    VoC_sub16_rd(REG7,REG3,CONST_AMR_3_ADDR);       //   *T0_frac = sub (sub (index, 3), add (i, i));
    VoC_sub16_rd(REG2,REG2,CONST_AMR_1_ADDR);

    VoC_mult16_rd(REG2,REG2,CONST_AMR_6_ADDR);          //   i = add (add (i, i), i);
    exit_on_warnings =OFF;
    VoC_add16_rr(REG6,REG2,REG5,DEFAULT);       //   *T0 = add (i, T0_min);
    exit_on_warnings =ON;
    VoC_sub16_rr(REG7,REG7,REG2,DEFAULT);       //}
    //  end of CII_Dec_lag6
Dec_lag6_end:
    VoC_lw16i(REG1,61);

    VoC_bnez16_d(Dec_amr_L21_122,DEC_AMR_BFI_ADDRESS);      //    if ( bfi == 0 && (pit_flag == 0 || sub (index, 61) < 0))
    VoC_bez16_d(Dec_amr_L22_122,DEC_AMR_PIT_FLAG_ADDRESS);  //   {
    VoC_bgt16_rd(Dec_amr_L22_122,REG1,DEC_AMR_INDEX_ADDRESS);//  }
Dec_amr_L21_122:                            //   else
    //   {
    VoC_sw16_d(REG6,STRUCT_DECOD_AMRSTATE_T0_LAGBUFF_ADDR);     //      st->T0_lagBuff = T0;
    VoC_movreg16(REG6,REG4,DEFAULT);            //      T0 = st->old_T0;
    VoC_lw16i_short(REG7,0,IN_PARALLEL);            //      T0_frac = 0;
Dec_amr_L22_122:                            //   }
    VoC_lw16i_short(REG2,0,DEFAULT);            //   Pred_lt_3or6 (st->exc, T0, T0_frac, L_SUBFR, 0);
    // end of  dec_amr_122_s2

    VoC_sw32_d(REG67,DEC_AMR_T0_ADDRESS);       // restore T0 &T0_frac
    VoC_lw16i(REG5,STRUCT_DECOD_AMRSTATE_EXC_ADDR);
    VoC_jal(CII_Pred_lt_3or6);



    VoC_lw16i_set_inc(REG0,DEC_AMR_CODE_ADDRESS,1);
    VoC_lw16_sd(REG2,2,DEFAULT);            // parm in reg2

    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_push16(REG0,DEFAULT);           // code push16 stack[n-4]

    // begin of dec_amr_122_s3
    VoC_lw16inc_p(REG1,REG2,DEFAULT);           //    index = *parm++;
    VoC_lw16i_set_inc(REG0,STRUCT_EC_GAIN_PITCHSTATE_ADDR,1);
    VoC_lw16d_set_inc(REG3,STRUCT_DECOD_AMRSTATE_STATE_ADDR,1);
    VoC_sw16_sd(REG2,3,DEFAULT);


    VoC_push16(REG0,DEFAULT);

    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_bez16_d(Dec_amr_L29_122,DEC_AMR_BFI_ADDRESS);       //    if (bfi != 0)

    VoC_push16(REG3,DEFAULT);               //    {
//      VoC_jal(CII_ec_gain_pitch);             //       ec_gain_pitch (st->ec_gain_p_st, st->state, &gain_pit);
    VoC_pop16(REG3,DEFAULT);
    VoC_jump(Dec_amr_L30_122);              //    }
Dec_amr_L29_122:                        //    else

    //    {  gain_pit = d_gain_pitch (mode, index);   }
    /*
    **************************************************************************
    *
    *  Function    : CII_d_gain_pitch_M122
       Input:      enum Mode mode->REG4
                   Word16 index  ->REG1
    *  Output:     gain->REG6(DEC_AMR_GAIN_PIT_ADDRESS)
    *  Created by Kenneth  07/15/2004
    **************************************************************************
    */
//  EFR DTX BEGIN

    VoC_lw16i(REG6,STATIC_CONST_QUA_GAIN_PITCH_ADDR);   // gain =  shr (qua_gain_pitch[index], 2)
    VoC_add16_rr(REG6,REG6,REG1,DEFAULT);
    VoC_NOP();

    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_shr16_ri(REG6,2,DEFAULT);



    VoC_shr16_ri(REG6,-2,DEFAULT);      //if (sub(mode, MR122) == 0)

    VoC_NOP();
    VoC_sw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);

Dec_amr_L30_122:
    VoC_jal(CII_ec_gain_pitch_update);          //    ec_gain_pitch_update (st->ec_gain_p_st, bfi, st->prev_bf,&gain_pit);


    VoC_pop16(REG0,DEFAULT);




    VoC_lw16_sd(REG1,3,DEFAULT);
    VoC_add16_rd(REG2,REG1,CONST_AMR_10_ADDR);

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_sw16_sd(REG2,3,DEFAULT);            //    parm += 10;

    //    dec_10i40_35bits (parm, code);
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /**************************************************************
     function void CII_dec_10i40_35bits(void)
     Input: Word16 index[],    i -> REG1(incr 1)
            Word16 cod[]       o -> REG0(incr 2) push REG0

      Version 1.0 Created by Kenneth       07/05/2004
      Version 1.1 Revised by Kenneth       08/17/2004
    **************************************************************/
//  begin of CII_dec_10i40_35bits
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16_sd(REG0,0,IN_PARALLEL);


    VoC_loop_i_short(20,DEFAULT);           //for (i = 0; i < L_CODE; i++)
    VoC_lw16i_short(REG2,0,IN_PARALLEL);
    VoC_startloop0;                 //{
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);       //    cod[i] = 0;
    VoC_endloop0;                   //}


    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16i(REG4,STATIC_CONST_DGRAY_ADDR);

    VoC_loop_i(0,5);            //for (j = 0; j < NB_TRACK; j++)
    //{
    VoC_lw16inc_p(REG3,REG1,DEFAULT);       //    tmp = index[j];
    VoC_lw16i_short(REG6,7,IN_PARALLEL);        //    i = tmp & 7;
    VoC_and16_rr(REG6,REG3,DEFAULT);        //    i = dgray[i];
    VoC_add16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_lw16i_short(REG5,5,DEFAULT);
    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_multf32_rr(REG67,REG6,REG5,DEFAULT);    //    i = extract_l (L_shr (L_mult (i, 5), 1));
    VoC_and16_ri(REG3,8);
    VoC_shr32_ri(REG67,1,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG2,DEFAULT);       //    pos1 = add (i, j);
    //    i = shr (tmp, 3) & 1;
    VoC_lw16i(REG7,4096);
    VoC_bez16_r(DEC_10I40_35BITS_L1,REG3);      //    if (i == 0)
    VoC_lw16i(REG7,-4096);              //    {        sign = 4096;        }
DEC_10I40_35BITS_L1:                    //    else {sign = -4096; }
    VoC_add16_rd(REG5,REG1,CONST_AMR_4_ADDR);       //    i = index[add (j, 5)] & 7;
    VoC_add16_rr(REG3,REG0,REG6,DEFAULT);
    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_and16_ri(REG5,7);
    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);       //    i = dgray[i];
    VoC_sw16_p(REG7,REG3,DEFAULT);          //    cod[pos1] = sign;
    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_multf32_rd(ACC0,REG5,CONST_AMR_5_ADDR);     //    i = extract_l (L_shr (L_mult (i, 5), 1));
    VoC_NOP();
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_movreg16(REG5,ACC0_LO,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG2,DEFAULT);       //    pos2 = add (i, j);
    VoC_bngt16_rr(DEC_10I40_35BITS_L2,REG6,REG5);   //    if (sub (pos2, pos1) < 0)
    VoC_sub16_dr(REG7,CONST_AMR_0_ADDR,REG7);       //    {  sign = negate (sign);     }
DEC_10I40_35BITS_L2:
    VoC_add16_rr(REG6,REG0,REG5,DEFAULT);       //    cod[pos2] = add (cod[pos2], sign);
    VoC_NOP();
    VoC_add16_rp(REG7,REG7,REG6,DEFAULT);
    VoC_inc_p(REG2,DEFAULT);
    VoC_sw16_p(REG7,REG6,DEFAULT);
    VoC_endloop0                    //}
// end of CII_dec_10i40_35bits
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


    VoC_lw16_d(REG5,DEC_AMR_GAIN_PIT_ADDRESS);      //    pit_sharp = shl (gain_pit, 1);
    // }
    // end of dec_amr_122_s3

    VoC_jal(CII_amr_dec_com_sub1);


    // REG6 for index
    // parm++;
    VoC_sw16_d(REG6,DEC_AMR_INDEX_ADDRESS);





    // begin of dec_amr_122_s4
    VoC_lw16i_set_inc(REG0,STRUCT_EC_GAIN_CODESTATE_ADDR,1);
    VoC_lw16i_set_inc(REG1,STRUCT_GC_PREDSTATE_ADDR,1);
    VoC_push16(REG0,DEFAULT);               // push16 st->ec_gain_c_st
    VoC_push16(REG1,DEFAULT);               // push16 st->pred_state

    VoC_bnez16_d(Dec_amr_L44_122,DEC_AMR_BFI_ADDRESS);      //      if (bfi == 0)


    /*
    **************************************************************************
    *
    *  Function    : CII_d_gain_code_M122
    *  Input: gc_predState *pred_state-> REG1() push16        stack[n]
              enum Mode mode,         -> GLOBAL_MODE_IN      //(direct address access, needn't pass the address as a parameter)
              Word16 index,           -> REG2 push16          stack[n-2]
              Word16 code[],          -> REG0(incr 1) push16  stack[n-1]
              Word16 *gain_code       ->DEC_AMR_GAIN_CODE_ADDRESS    //(direct address access, needn't pass the address as a parameter)
      Created by  Kenneth   07/14/2004
    **************************************************************************
    */
    VoC_push16(REG6,DEFAULT);               //  push16 index
    VoC_jal(CII_gc_pred_M122_dec);              //return exp->REG7 ;frac->REG6
    VoC_movreg32(REG01,REG67,DEFAULT);
    VoC_jal(CII_Pow2);                  // {gcode0 = extract_l (Pow2 (exp, frac));
    //  return REG23
    VoC_pop16(REG5,DEFAULT);        //p = &qua_gain_code[add (add (index, index), index)];


    VoC_mult16_rd(REG5,REG5,CONST_AMR_3_ADDR);
    VoC_lw16i(REG3,STATIC_CONST_qua_gain_code_ADDR);
    VoC_add16_rr(REG0,REG5,REG3,DEFAULT);               //  p->REG6
    //    gcode0 = shl (gcode0, 4);
    VoC_shr16_ri(REG2,-4,DEFAULT);
    VoC_multf16inc_rp(REG2,REG2,REG0,DEFAULT);      //    *gain_code = shl (mult (gcode0, *p++), 1);

    VoC_NOP();
    VoC_shr16_ri(REG2,-1,DEFAULT);

    VoC_lw16inc_p(REG7,REG0,DEFAULT);           //qua_ener_MR122 = *p++;
    VoC_lw16inc_p(REG6,REG0,DEFAULT);           //qua_ener = *p++;

    VoC_sw16_d(REG2,DEC_AMR_GAIN_CODE_ADDRESS);
    VoC_lw16i(REG5,STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR);
    VoC_jal(CII_gc_pred_update);                //gc_pred_update(pred_state, qua_ener_MR122, qua_ener);

    VoC_lw16i_short(INC1,1,DEFAULT);




    VoC_jump(Dec_amr_L45_122);                  //      }
Dec_amr_L44_122:                            //      else
    VoC_lw16d_set_inc(REG2,STRUCT_DECOD_AMRSTATE_STATE_ADDR,1);//      {
    VoC_NOP();
    VoC_push16(REG2,DEFAULT);               // push16 st->state
//      VoC_jal(CII_ec_gain_code);              //      ec_gain_code (st->ec_gain_c_st, st->pred_state, st->state,&gain_code);




    VoC_pop16(REG2,DEFAULT);                //      }
Dec_amr_L45_122:
    VoC_pop16(REG1,DEFAULT);                //  pop16   st->pred_state
    VoC_lw16_d(REG6,DEC_AMR_GAIN_CODE_ADDRESS);
    VoC_jal(CII_ec_gain_code_update);           //      ec_gain_code_update (st->ec_gain_c_st, bfi, st->prev_bf,&gain_code);
    VoC_pop16(REG0,DEFAULT);                //  pop16  st->ec_gain_c_st





    VoC_lw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);     //      pit_sharp = gain_pit;
    // end of dec_amr_122_s4

    VoC_shr16_ri(REG6,-1,DEFAULT );             //pit_sharp = shl (pit_sharp, 1);



// EFR DTX BEGIN

    VoC_lw16i_short(REG7,1,DEFAULT);


    VoC_lw16_d(REG2,DEC_AMR_GAIN_PIT_ADDRESS);          // gain_pit in reg2
    VoC_sw16_d(REG6,DEC_AMR_PIT_SHARP_ADDRESS);
    VoC_lw16i_set_inc(REG0,STRUCT_DECOD_AMRSTATE_EXC_ADDR,1);   //{
    VoC_bngt16_rd(Dec_amr_L52_122,REG6,CONST_AMR_0x4000_ADDR);  //if (sub (pit_sharp, 16384) > 0)
    VoC_multf16inc_rp(REG5,REG6,REG0,DEFAULT);
    VoC_lw16i_set_inc(REG1,DEC_AMR_EXCP_ADDRESS,1);
    VoC_multf32_rr(ACC0,REG5,REG2,DEFAULT);     //       L_temp = L_mult (temp, gain_pit);

    VoC_loop_i_short(40,DEFAULT);                   //   for (i = 0; i < L_SUBFR; i++)

    VoC_startloop0                      //    {
    //       temp = mult (st->exc[i], pit_sharp);
    //       if (sub(mode, MR122)==0)
    VoC_shr32_rr(ACC0,REG7,DEFAULT);                //       {  L_temp = L_shr (L_temp, 1);}
    VoC_multf16inc_rp(REG5,REG6,REG0,DEFAULT);
    VoC_add32_rd(ACC0,ACC0,CONST_AMR_0x00008000_ADDR);      //       excp[i] = round (L_temp);
    VoC_multf32_rr(ACC0,REG5,REG2,DEFAULT);     //       L_temp = L_mult (temp, gain_pit);
    exit_on_warnings =OFF ;
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    exit_on_warnings =ON ;
    VoC_endloop0                            //    }


Dec_amr_L52_122:                                //}
    VoC_lw16i_set_inc(REG0,13017,1);
    //if (sub(mode, MR475) != 0 || evenSubfr == 0)  {
    VoC_movreg16(REG7,REG2,DEFAULT);        //    st->sharp = gain_pit;
    VoC_bngt16_rr(Dec_amr_L54_122,REG7,REG0);           //    if (sub (st->sharp, SHARPMAX) > 0)
    VoC_movreg16(REG7,REG0,DEFAULT);        //    {  st->sharp = SHARPMAX;
Dec_amr_L54_122:                    //}

    VoC_bnez16_d(Dec_amr_L56_122,DEC_AMR_BFI_ADDRESS);      //if ( bfi == 0 )
    VoC_lw16i_set_inc(REG3,STRUCT_DECOD_AMRSTATE_LTPGAINHISTORY_ADDR,1);//{
    VoC_add16_rd(REG0,REG3,CONST_AMR_1_ADDR);
    VoC_lw16i_short(REG6,8,DEFAULT);                   //   for (i = 0; i < 8; i++)
    VoC_jal(CII_R02R3_p_R6);                //   {   st->ltpGainHistory[i] = st->ltpGainHistory[i+1]; }
    VoC_sw16_p(REG2,REG3,DEFAULT);          //   st->ltpGainHistory[8] = gain_pit;
Dec_amr_L56_122:
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG0,DEFAULT);
    VoC_sw16_d(REG7,STRUCT_DECOD_AMRSTATE_SHARP_ADDR);
    VoC_jal(CII_Int_lsf);               //Int_lsf(prev_lsf, st->lsfState->past_lsf_q, i_subfr, lsf_i);


    VoC_lw32_d(REG67,STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR);
    VoC_add32_rd(REG67,REG67,CONST_AMR_0x00010001_ADDR);
    VoC_bgt16_rd(CGAver_L5_122,REG5,CONST_AMR_5325_ADDR);       //if (sub(diff, 5325) > 0)
    VoC_lw16i_short(REG7,0,DEFAULT);            //{ st->hangVar = add(st->hangVar, 1);}
CGAver_L5_122:                      // else{st->hangVar = 0; }

    VoC_bngt16_rd(CGAver_L6_122,REG7,CONST_AMR_10_ADDR);    //if (sub(st->hangVar, 10) > 0)
    VoC_lw16i_short(REG6,1,DEFAULT);            //{  st->hangCount = 0;
CGAver_L6_122:
    VoC_pop16(REG5,DEFAULT);            //bgMix = 8192; cbGainMix = gain_code;
    VoC_sw32_d(REG67,STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR);
    //return cbGainMix;  ->REG5
    //  end of Cb_gain_average

    VoC_pop16(REG7,DEFAULT);

    //   gain_code_mix = gain_code;      // MR74, MR795, MR122
    //}
    VoC_sw16_d(REG5,DEC_AMR_GAIN_CODE_MIX_ADDRESS);

    VoC_lw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);
    VoC_lw16i_short(REG7,3,DEFAULT);        //   tmp_shift = 3


    VoC_shr16_ri(REG6,1,DEFAULT);               //{  pitch_fac = shr (gain_pit, 1);
    VoC_lw16i_short(REG7,2,DEFAULT);        //   tmp_shift = 2; }

    VoC_jal(CII_amr_dec_com_sub3);
    VoC_jal(CII_amr_dec_com_sub4);
    VoC_pop32(ACC0,DEFAULT);        //   pop32 stack32[n]

    VoC_jal(CII_amr_dec_com_sub5);
    VoC_jump(Dec_amr_LoopStart_122);    //  }
Dec_amr_LoopEnd_122:

    VoC_pop16(REG5,DEFAULT);        // pop16 stack[n-2]
    VoC_pop16(REG4,DEFAULT);        // pop16 stack[n-1]
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_lw16_sd(REG3,1,DEFAULT);
    VoC_return;

}

/*************************************************************************
 *   Copyright 2004, CII Technologies Inc
 * FUNCTION:  gc_pred()
 *
 * PURPOSE: MA prediction of the innovation energy
 *          (in dB/(20*log10(2))) with mean  removed).

 *INPUT:    REG7:  mode
*
*              REG1 :  st

*  OUTPUT:
               REG7:     exp_gcode0
                REG6:    frac_gcode0

 *
 * Version 1.0  Create by Cui   07/09/2004

 *************************************************************************/
void CII_gc_pred_M122_dec(void)

{
    VoC_push16(RA, DEFAULT);

    // inline CII_gc_pred_s1(void)
    VoC_lw16i_set_inc(REG0,COD_AMR_CODE_ADDRESS, 2);
    VoC_loop_i_short(20, DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_bimac32inc_pp(REG0, REG0);
    VoC_endloop0

    VoC_add16_rd(REG6, REG1, CONST_AMR_4_ADDR);
    VoC_add32_rd(REG23, ACC0, CONST_AMR_0x00008000_ADDR);
    VoC_multf32_rd(ACC0, REG3, CONST_AMR_26214_ADDR);   //1216
    VoC_jal(CII_Log2);  //   //REG4  exp   REG5   frac

//  VoC_lw32_d(ACC0, CONST_783741L_ADDR); //acc0 is     783741L    MEAN_ENER_MR122
    VoC_lw16i(ACC0_LO,0xf57d);
    VoC_movreg16(REG0, REG6,DEFAULT);          //REG0  :     st->past_qua_en_MR122
    VoC_lw16i_short(ACC0_HI,0xb,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1, STATIC_CONST_pred_MR122_ADDR, 1);

    VoC_loop_i_short(4, DEFAULT);
    VoC_startloop0;
    VoC_mac32inc_pp(REG0, REG1,DEFAULT);
    VoC_endloop0;

    VoC_movreg16(REG6, REG5, DEFAULT); //reg67 :ener_code
    VoC_shr32_ri(REG67, -1, DEFAULT);
    VoC_lw16i_short(REG2,30,IN_PARALLEL);

    VoC_sub16_rr(REG7, REG4, REG2,DEFAULT);
    VoC_sub32_rr(REG67, ACC0, REG67, DEFAULT);
    VoC_shr32_ri(REG67, 1, DEFAULT);

    VoC_pop16(RA, IN_PARALLEL);
    VoC_shru16_ri(REG6, 1,DEFAULT);
//  VoC_and16_ri(REG6,0x7fff);
    VoC_return;
}


/********************************************************************************
  Function:  void CII_D_plsf_5_sub(void)
   input: REG1(incr 1)
      REG0(incr 2)
          REG3(incr 2)
   Description: [REG0]->[REG3]
   Used registerS: ACC0,ACC1,REG0,REG3,REG4
   Version 1.0 Created by Kenneth  07/09/2004
   Version 1.1 Optimized by Kenneth 08/12/2004
 ********************************************************************************/

void CII_D_plsf_5_sub(void)
{
    VoC_lw16inc_p(REG0,REG1,DEFAULT);
    VoC_shr16_ri(REG0,-2,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_add16_rd(REG4,REG3,CONST_AMR_10_ADDR);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);       //lsf1_r[n]   = *p_dico++;
    VoC_lw32inc_p(ACC1,REG0,DEFAULT);       //lsf1_r[n+1] = *p_dico++;
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);       //lsf2_r[n]   = *p_dico++;
    VoC_sw32_p(ACC1,REG4,DEFAULT);          //lsf2_r[n+1] = *p_dico++;
    VoC_return;
}



/***********************************************
  Function CII_Int_lpc_1and3
  input: REG3->&lsp_old[]
         REG2->&lsp_mid[]
         REG5->&lsp_new[]
         REG0->&Az[]
         Optimised by Cui 2005.01.25
***********************************************/
void CII_Int_lpc_1and3_dec(void)
{

#if 0

    voc_short   INT_LPC_LSP_ADDR                ,10     ,y
    voc_short   F1_ADDR                ,12  ,y
    voc_short   F2_ADDR                ,12  ,y

#endif

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_set_inc(REG1,INT_LPC_LSP_ADDR,2);

Int_lpc_1and3_L0_dec:
    VoC_bne16_rd(Int_lpc_1and3_L3_dec,REG7,CONST_AMR_44_ADDR);
    VoC_return;
Int_lpc_1and3_L3_dec:

    VoC_push16(RA,DEFAULT);             // push16(n)

    VoC_push16(REG5,DEFAULT);           // push16(n-1)  lsp_new
    VoC_push32(REG67,IN_PARALLEL);          // push32(n)    N/REG6
    VoC_push16(REG2,DEFAULT);           // push16(n-2)  lsp_mid
    VoC_push32(REG01,IN_PARALLEL);          // push32(n-1)  lsp/Az

    VoC_jal(CII_Int_lpc_only);
    VoC_lw32_sd(REG67,1,DEFAULT);
    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_jal(CII_Lsp_Az);
    VoC_lw32_sd(REG67,1,DEFAULT);
    VoC_add16_rd(REG7,REG7,CONST_AMR_22_ADDR);
    VoC_sub16_rd(REG5,REG7,CONST_AMR_11_ADDR);
    VoC_sw32_sd(REG67,1,DEFAULT);
    //VoC_bez16_r(Int_lpc_1and3_L1_dec,REG6);
    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG5,IN_PARALLEL);
    VoC_bne16_rd(Int_lpc_1and3_L2_dec,REG5,CONST_AMR_33_ADDR);
    VoC_lw16_sd(REG1,1,DEFAULT);
Int_lpc_1and3_L2_dec:
    VoC_jal(CII_Lsp_Az);
//Int_lpc_1and3_L1_dec:
    VoC_pop16(REG2,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);
    VoC_pop16(REG3,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);

    VoC_pop16(RA,DEFAULT);
    VoC_movreg16(REG5, REG3,IN_PARALLEL);
    VoC_jump(Int_lpc_1and3_L0_dec);

    VoC_return;
}





