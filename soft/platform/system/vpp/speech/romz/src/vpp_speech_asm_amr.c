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

    VoC_be16_rd(DtxDec_L8,REG7,CONST_7_ADDR);       //if ((sub(frame_type, RX_SID_FIRST) == 0)   ||
    VoC_bgt16_rd(DtxDec_L3,REG7,CONST_3_ADDR);      //    (sub(frame_type, RX_SID_UPDATE) == 0)  ||
DtxDec_L8:
    VoC_bez16_r(DtxDec_L5,REG3);                    //    (((sub(st->dtxGlobalState, DTX) == 0) ||
    VoC_bngt16_rd(DtxDec_L5,REG7,CONST_1_ADDR);     //     ((sub(frame_type, RX_NO_DATA) == 0) ||
DtxDec_L3:                                          //   {
    VoC_lw16i_short(REG4,1,DEFAULT);                //   newState = DTX;
    VoC_bne16_rd(DtxDec_L7,REG3,CONST_2_ADDR);      //   if ((sub(st->dtxGlobalState, DTX_MUTE) == 0) &&
    VoC_be16_rd(DtxDec_L7,REG7,CONST_3_ADDR);       //       ((sub(frame_type, RX_SID_BAD) == 0) ||
    VoC_be16_rd(DtxDec_L7,REG7,CONST_5_ADDR);       //        (sub(frame_type, RX_SID_FIRST) ==  0) ||
    //   {
    VoC_lw16i_short(REG4,2,DEFAULT);                //      newState = DTX_MUTE;   }
DtxDec_L7:

    VoC_lw16i_short(REG2,1,DEFAULT);                //   st->since_last_sid = add(st->since_last_sid, 1);
    VoC_add16_rd(REG2,REG2,STRUCT_DTX_DECSTATE_SINCE_LAST_SID_ADDR);

    VoC_be16_rd(DtxDec_L5,REG5,CONST_5_ADDR);       //   if((sub(frame_type, RX_SID_UPDATE) != 0) &&
    VoC_bngt16_rd(DtxDec_L5,REG2,CONST_50_ADDR);    //      (sub(st->since_last_sid, DTX_MAX_EMPTY_THRESH) > 0))
    VoC_lw16i_short(REG4,2,DEFAULT);                //   { newState = DTX_MUTE;   }
DtxDec_L5:
    VoC_sw16_d(REG2,STRUCT_DTX_DECSTATE_SINCE_LAST_SID_ADDR);

    VoC_lw16d_set_inc(REG3,STRUCT_DTX_DECSTATE_DECANAELAPSEDCOUNT_ADDR,1);

    VoC_bnez16_d(DtxDec_L9,STRUCT_DTX_DECSTATE_DATA_UPDATED_ADDR);//if ((st->data_updated == 0) &&
    VoC_bne16_rd(DtxDec_L9,REG7,CONST_5_ADDR);      //    (sub(frame_type, RX_SID_UPDATE) == 0))
    VoC_lw16i_short(REG3,0,DEFAULT);            //{ st->decAnaElapsedCount = 0;  }
DtxDec_L9:
    VoC_lw16i_short(REG1,0,DEFAULT);           //else   //{  encState = SPEECH; }
    VoC_add16_rd(REG3,REG3,CONST_1_ADDR);               //st->decAnaElapsedCount = add(st->decAnaElapsedCount, 1);

    VoC_be16_rd(DtxDec_L10,REG7,CONST_2_ADDR);      //if ((sub(frame_type, RX_SID_FIRST) == 0)  ||(sub(frame_type, RX_ONSET) == 0)      ||
    //    (sub(frame_type, RX_SID_UPDATE) == 0) ||
    VoC_blt16_rd(DtxDec_L11,REG7,CONST_4_ADDR);     //    (sub(frame_type, RX_SID_BAD) == 0)    ||(sub(frame_type, RX_NO_DATA) == 0))
DtxDec_L10:                         //{

    VoC_bne16_rd(DtxDec_L12,REG7,CONST_7_ADDR);     //   if((sub(frame_type, RX_NO_DATA) == 0) &&
    VoC_bez16_r(DtxDec_L11,REG4);           //      (sub(newState, SPEECH) == 0))
DtxDec_L12:
    VoC_lw16i_short(REG1,1,DEFAULT);            //   encState = DTX;
DtxDec_L11:

    VoC_lw16i_short(REG6,0,DEFAULT);            //st->dtxHangoverAdded = 0;
    VoC_lw16i_short(REG2,7,DEFAULT);            //{   st->dtxHangoverCount = DTX_HANG_CONST;

    VoC_bez16_r(DtxDec_L14,REG1);       //if (sub(encState, SPEECH) == 0)

    VoC_lw16i_short(REG6,1,DEFAULT);            //      st->dtxHangoverAdded = 1;

    VoC_bgt16_rd(DtxDec_L15,REG3,CONST_30_ADDR);    //   if (sub(st->decAnaElapsedCount, DTX_ELAPSED_FRAMES_THRESH) > 0){
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
    VoC_be16_rd(DtxDec_L22,REG7,CONST_4_ADDR);      //   if (sub(frame_type, RX_SID_FIRST) == 0)
    VoC_lw16i_short(REG3,1,DEFAULT);            //      st->valid_data = 1;
    VoC_be16_rd(DtxDec_L22,REG7,CONST_5_ADDR);      //   else if (sub(frame_type, RX_SID_UPDATE) == 0)
    VoC_lw32z(REG23,DEFAULT);          //{   st->sid_frame = 0;    //    st->valid_data = 0;
    VoC_bne16_rd(DtxDec_L22,REG7,CONST_6_ADDR);     //   else if (sub(frame_type, RX_SID_BAD) == 0)
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
void CII_dtx_dec(void)
{
#if 0
    voc_short    DTX_DEC_L_lsf_ADDR                      ,20,x          //[20]shorts
    voc_short    DTX_DEC_lsf_ADDR                        ,10,x          //[10]shorts
    voc_short    dtx_dec_LSP_INT_ADDR                    ,10,x          //[10]shorts
    voc_short    dtx_dec_LSF_INT_ADDR                    ,10,x          //[10]shorts
    voc_short    dtx_dec_lsf_int_variab_ADDR             ,10,x          //[10]shorts
    voc_short    dtx_dec_lsp_int_variab_ADDR             ,10,x          //[10]shorts
    voc_short    dtx_dec_acoeff_ADDR                     ,12,x          //[12]shorts
    voc_short    dtx_dec_acoeff_variab_ADDR              ,12,x          //[12]shorts
    voc_short    dtx_dec_refl_ADDR                       ,10,x          //[10]shorts
    voc_short    dtx_dec_ex_ADDR                         ,40,x           //[40]shorts
#endif


    VoC_push16(RA,DEFAULT);             //push16 stack[n]
    // Push the parameters used in the Function
    VoC_lw16i_set_inc(REG0,STRUCT_DTX_DECSTATE_SINCE_LAST_SID_ADDR,2);
    VoC_lw16i_set_inc(REG1,STATIC_CONST_dtx_log_en_adjust_ADDR,2);
    VoC_push16(REG0,DEFAULT);                    //push16 stack[n-1]  The beginning address of dtx_decState

    VoC_lw32inc_p(REG23,REG0,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_push16(REG2,DEFAULT);           //push16 stack[n-2]  The value of st->since_last_sid
    VoC_push16(REG3,DEFAULT);           //push16 stack[n-3]  The value of st->true_sid_period_inv
    VoC_push16(REG4,DEFAULT);           //push16 stack[n-4]  The value of st->log_en
    VoC_push32(REG67,IN_PARALLEL);          //push32 stack32[n]  The value of st->L_pn_seed_rx

    VoC_push16(REG5,DEFAULT);           //push16 stack[n-5]  The value of st->old_log_en
    VoC_lw16i_short(REG6,10,IN_PARALLEL);
    VoC_add16_rr(REG2,REG0,REG6,DEFAULT);
    VoC_add16_rr(REG3,REG2,REG6,DEFAULT);
    VoC_push16(REG0,DEFAULT);           //push16 stack[n-6]  The beginning address of st->lsp[]
    VoC_push16(REG2,DEFAULT);           //push16 stack[n-7]  The beginning address of st->lsp_old[]
    VoC_push16(REG3,DEFAULT);           //push16 stack[n-8]  The beginning address of st->lsf_hist[]
    VoC_add16_rd(REG0,REG0,CONST_102_ADDR);
    VoC_add16_rd(REG2,REG0,CONST_80_ADDR);
    VoC_push16(REG0,DEFAULT);           //push16 stack[n-9]  The beginning address of st->lsf_hist_mean[]
    VoC_lw16_sd(REG4,13,DEFAULT);

    VoC_push16(REG2,DEFAULT);           //push16 stack[n-10] The beginning address of st->log_en_hist[]
    VoC_add16_rr(REG1,REG4,REG1,IN_PARALLEL);
    VoC_lw16_d(REG3,STRUCT_DTX_DECSTATE_LOG_EN_ADJUST_ADDR);
    VoC_lw16_p(REG5,REG1,DEFAULT);
    VoC_push16(REG3,DEFAULT);           //push16 stack[n-11] The value of st->log_en_adjust
    VoC_push16(REG5,DEFAULT);           //push16 stack[n-12] The value of dtx_log_en_adjust[mode]

    /*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    VoC_bez16_d(DTX_DEC_L001a, STRUCT_DTX_DECSTATE_DTXHANGOVERADDED_ADDR);  //if ((st->dtxHangoverAdded != 0) &&
    VoC_bnez16_d(DTX_DEC_L001b, STRUCT_DTX_DECSTATE_SID_FRAME_ADDR);    //    (st->sid_frame != 0))
DTX_DEC_L001a:
    VoC_jump(DTX_DEC_L001);
DTX_DEC_L001b:

    VoC_lw16d_set_inc(REG3,STRUCT_DTX_DECSTATE_LSF_HIST_PTR_ADDR,1);
    VoC_sw16_sd(REG5,1,DEFAULT);                //   st->log_en_adjust = dtx_log_en_adjust[mode];
    VoC_add16_rr(REG4,REG3,REG6,DEFAULT);           //   ptr = add(st->lsf_hist_ptr, M);
    VoC_bne16_rd(DTX_DEC_L002,REG4,CONST_80_ADDR);  //   if (sub(ptr, 80) == 0)
    VoC_lw16i_short(REG4, 0, DEFAULT);          //   {       ptr = 0;
DTX_DEC_L002:                           //   }
    VoC_lw16_sd(REG2,4,DEFAULT);
    VoC_add16_rr(REG0,REG2,REG3,DEFAULT);
    VoC_add16_rr(REG1,REG2,REG4,DEFAULT);
    VoC_jal(CII_Copy_M);                    //   Copy( &st->lsf_hist[st->lsf_hist_ptr],&st->lsf_hist[ptr],M);
    VoC_lw16_d(REG6,STRUCT_DTX_DECSTATE_LOG_EN_HIST_PTR_ADDR);
    VoC_add16_rd(REG5,REG6,CONST_1_ADDR);       //   ptr = add(st->log_en_hist_ptr,1);
    VoC_bne16_rd(DTX_DEC_L003,REG5,CONST_8_ADDR);       //   if (sub(ptr, DTX_HIST_SIZE) == 0)
    VoC_lw16i_short(REG5, 0,DEFAULT);           //   {       ptr = 0;
DTX_DEC_L003:                           //   }
    VoC_lw16_sd(REG1,2,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG1,DEFAULT);
    VoC_lw16i_set_inc(REG0,DTX_DEC_L_lsf_ADDR,2);
    VoC_add16_rr(REG5,REG5,REG1,DEFAULT);
    VoC_lw16_p(REG6,REG6,IN_PARALLEL);          //   st->log_en_hist[ptr] = st->log_en_hist[st->log_en_hist_ptr];
    VoC_push16(REG0,DEFAULT);
    VoC_lw16_sd(REG3,5,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);
    VoC_sw16_p(REG6,REG5,DEFAULT);
    VoC_lw32z(REG67,IN_PARALLEL);

    VoC_loop_i_short(10,DEFAULT);               // push16 stack[n-13]  L_lsf
    //   for (i = 0; i < M; i++)
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_startloop0                  //   {
    VoC_sw32inc_p(REG67,REG0,DEFAULT);      //      L_lsf[i] = 0;
    VoC_endloop0                        //   }

    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_loop_i(1, 8);                   //   for (i = 0; i < DTX_HIST_SIZE; i++)

    VoC_shr16_ri(REG5,3,DEFAULT);       //   { st->log_en = add(st->log_en,shr(st->log_en_hist[i],3));
    VoC_lw16_sd(REG0,0,IN_PARALLEL);

    VoC_loop_i_short(10,DEFAULT);           //      for (j = 0; j < M; j++)
    VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);
    VoC_startloop0                  //      {
    VoC_lw16inc_p(REG6,REG3,DEFAULT);       //         L_lsf[j] = L_add(L_lsf[j], L_deposit_l(st->lsf_hist[i * M + j]));
    VoC_add32_rp(ACC0,REG67,REG0,DEFAULT);
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);       //      }
    VoC_endloop0
    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_endloop1                        //   }

    VoC_lw16_sd(REG7,1,DEFAULT);                //   st->log_en = sub(st->log_en, st->log_en_adjust);
    VoC_sub16_rr(REG4,REG4,REG7,DEFAULT);
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_sw16_sd(REG4,9,DEFAULT);
    VoC_movreg16(REG1,REG0,IN_PARALLEL);

    VoC_loop_i_short(10,DEFAULT);           //   for (j = 0; j < M; j++)
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);                          //  L_lsf[] and lsf[] share the same memory
    VoC_startloop0                  //   {
    VoC_shr32_ri(REG45,3,DEFAULT);      //      lsf[j] = extract_l(L_shr(L_lsf[j],3));
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0                        //   }

    VoC_pop16(REG0,DEFAULT);                //  pop16 stack[n-13]
    VoC_lw16_sd(REG1,6,DEFAULT);
    VoC_lw16i(REG4,1310);
    VoC_jal(CII_Lsf_lsp);               //   Lsf_lsp(lsf, st->lsp, M);

    VoC_lw16_sd(REG0,4,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_lw16_sd(REG1,3,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_lw16i_short(REG6,20,DEFAULT);
    VoC_lw16i_short(INC2,10,IN_PARALLEL);
    VoC_jal(CII_copy_xy);                   //   Copy(st->lsf_hist, st->lsf_hist_mean, 80);

    VoC_lw16i_set_inc(REG1,655,2);

    VoC_lw16i_set_inc(REG3,STATIC_CONST_lsf_hist_mean_scale_ADDR,1);
    VoC_lw16_sd(REG0,3,DEFAULT);


    VoC_loop_i(1,10);                   //   for (i = 0; i < M; i++)
    VoC_lw32z(ACC0,DEFAULT);            //   {
    VoC_movreg16(REG2,REG0,DEFAULT);        //   L_lsf_mean = 0;

    VoC_loop_i_short(8,DEFAULT);                //      for (j = 0; j < 8; j++)
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_startloop0                  //      {
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,REG67,DEFAULT); //         L_lsf_mean = L_add(L_lsf_mean,L_deposit_l(st->lsf_hist_mean[i+j*M]));
    VoC_endloop0                    //      }
    VoC_movreg16(REG2,REG0,DEFAULT);
    VoC_shr32_ri(ACC0,3,DEFAULT);
    VoC_movreg16(REG5,ACC0_LO,DEFAULT);             //      lsf_mean = extract_l(L_shr(L_lsf_mean, 3));

    VoC_loop_i_short(8,DEFAULT);            //      for (j = 0; j < 8; j++)
    VoC_startloop0                  //      {

    VoC_sub16_pr(REG6,REG2,REG5,DEFAULT);   //         st->lsf_hist_mean[i+j*M] =  sub(st->lsf_hist_mean[i+j*M], lsf_mean);
    VoC_multf16_rp(REG6,REG6,REG3,DEFAULT); //         st->lsf_hist_mean[i+j*M] =  mult(st->lsf_hist_mean[i+j*M], lsf_hist_mean_scale[i]);
    //    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_NOP();//change
    VoC_bnltz16_r(DTX_DEC_L004,REG6);       //         if (st->lsf_hist_mean[i+j*M] < 0)
    VoC_sub16_rr(REG6,REG7,REG6,DEFAULT );
    VoC_lw16i_short(REG7,1,DEFAULT);        //         {             negative = 1;
    //      VoC_jump(DTX_DEC_L005);         //         }
DTX_DEC_L004:                           //         else
    //         {        negative = 0;
//DTX_DEC_L005:                         //         }
//          VoC_bgtz16_r(DTX_DEC_Lab5,REG6);                //         st->lsf_hist_mean[i+j*M] = abs_s(st->lsf_hist_mean[i+j*M]);
//          VoC_sub16_dr(REG6,CONST_0_ADDR,REG6);
//DTX_DEC_Lab5:
    VoC_bngt16_rr(DTX_DEC_L006,REG6,REG1);  //         if (sub(st->lsf_hist_mean[i+j*M], 655) > 0)
    VoC_sub16_rr(REG6,REG6,REG1,DEFAULT);   //         {
    VoC_shr16_ri(REG6,2,DEFAULT);       //            st->lsf_hist_mean[i+j*M] =  add(655, shr(sub(st->lsf_hist_mean[i+j*M], 655), 2));
    VoC_add16_rr(REG6,REG6,REG1,DEFAULT);   //         }
DTX_DEC_L006:
    VoC_bngt16_rr(DTX_DEC_L007,REG6,REG4);  //         if (sub(st->lsf_hist_mean[i+j*M], 1310) > 0)
    VoC_movreg16(REG6,REG4,DEFAULT);        //         {     st->lsf_hist_mean[i+j*M] = 1310;   }
DTX_DEC_L007:
    VoC_bez16_r(DTX_DEC_L008,REG7);     //         if (negative != 0)
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_sub16_rr(REG6,REG7,REG6,DEFAULT );  //         {    st->lsf_hist_mean[i+j*M] = -st->lsf_hist_mean[i+j*M];   }
DTX_DEC_L008:

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_sw16inc_p(REG6,REG2,DEFAULT);
    VoC_endloop0;                   //      }
    VoC_inc_p(REG0,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_endloop1                     //   }
DTX_DEC_L001:                       //}
    /*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

    VoC_bnez16_d(DTX_DEC_L010a, STRUCT_DTX_DECSTATE_SID_FRAME_ADDR);    //if (st->sid_frame != 0 )
    VoC_jump(DTX_DEC_L010);
DTX_DEC_L010a:
    VoC_lw16_sd(REG0,6,DEFAULT);                //{
    VoC_lw16_sd(REG1,5,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_jal(CII_Copy_M);                    //   Copy(st->lsp, st->lsp_old, M);
    VoC_lw16_sd(REG7,8,DEFAULT);                //   st->old_log_en = st->log_en;
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_sw16_sd(REG7,7,DEFAULT);

    VoC_bnez16_d(DTX_DEC_L011a,STRUCT_DTX_DECSTATE_VALID_DATA_ADDR)//   if (st->valid_data != 0 )
    VoC_jump(DTX_DEC_L011);
DTX_DEC_L011a:
    VoC_lw16_sd(REG1,10,DEFAULT);           //   {    tmp_int_length = st->since_last_sid;
    VoC_sw16_sd(REG4,10,DEFAULT);           //      st->since_last_sid = 0;
    VoC_bngt16_rd(DTX_DEC_L012,REG1,CONST_32_ADDR); //      if (sub(tmp_int_length, 32) > 0)
    VoC_lw16i(REG1, 32);
    //      {  tmp_int_length = 32;   }
DTX_DEC_L012:
    VoC_shr16_ri(REG1,-10,DEFAULT);
    VoC_lw16i(REG2,0x4000);
    VoC_blt16_rd(DTX_DEC_L013,REG1,CONST_0x0800_ADDR);  //      if (sub(tmp_int_length, 2) >= 0)
    VoC_lw16i_set_inc(REG0,0x400,1);            //      {
    VoC_jal(CII_DIV_S);                 //         st->true_sid_period_inv = div_s(1 << 10, shl(tmp_int_length, 10));
DTX_DEC_L013:                           //      }
    VoC_lw16i_short(REG7,10,DEFAULT);                           //      else {  st->true_sid_period_inv = 1 << 14;     }
    VoC_lw16i_set_inc(REG1,DEC_AMR_PARM_ADDRESS,1);
    VoC_sw16_sd(REG2,9,DEFAULT);                // restore st->true_sid_period_inv

    VoC_lw16inc_p(REG6,REG1,DEFAULT);
    VoC_mult16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_push16(REG1,DEFAULT);               //  push16 stack[n-13]  parm[1]
    VoC_lw16i_set_inc(REG1,STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR,2);
    VoC_lw16i_set_inc(REG0,STATIC_past_rq_init_ADDR,2);
    VoC_push16(REG1,DEFAULT);                //  push16 stack[n-14]  lsfState->past_lsf_q
//  VoC_add16_rr(REG1,REG1,REG7, DEFAULT);
//  VoC_add16_rr(REG0,REG0,REG6,IN_PARALLEL);
    VoC_add32_rr( REG01,REG01,REG67,DEFAULT);
    VoC_jal(CII_Copy_M);                    //   Copy(&past_rq_init[parm[0] * M], lsfState->past_r_q, M);

    VoC_lw16_sd(REG3,8,DEFAULT);
    VoC_lw16i_short(REG7,8,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_jal(CII_D_plsf_3);                  //      D_plsf_3(lsfState, MRDTX, 0, &parm[1], st->lsp);

    VoC_pop16(REG0,DEFAULT);                //  pop16 stack[n-14]
    VoC_add16_rd(REG0,REG0,CONST_10_ADDR);      //      Set_zero(lsfState->past_r_q, M);
    VoC_loop_i_short(10,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16_d(REG7,DEC_AMR_PARM_ADDRESS+4);    //      log_en_index = parm[4];
    VoC_pop16(REG4,DEFAULT);                //  pop16 stack[n-13]
    VoC_shr16_ri(REG7,-9,IN_PARALLEL);      //      st->log_en = shl(log_en_index, (11 - 2));
    VoC_lw16i_set_inc(REG0,5120,2);
    VoC_sub16_rr(REG7,REG7,REG0,DEFAULT);       //      st->log_en = sub(st->log_en, (2560 * 2));

    VoC_bnez16_d(DTX_DEC_L015,(DEC_AMR_PARM_ADDRESS+4));        //      if (log_en_index == 0)
    VoC_lw16i(REG7,0x8000);         //      {   st->log_en = MIN_16;    }
DTX_DEC_L015:
    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_sw16_sd(REG7,8,DEFAULT);

    VoC_bez16_d(DTX_DEC_L016,STRUCT_DTX_DECSTATE_DATA_UPDATED_ADDR);    //      if ((st->data_updated == 0) ||
    VoC_bnez16_d(DTX_DEC_L011,STRUCT_DTX_DECSTATE_DTXGLOBALSTATE_ADDR); //          (sub(st->dtxGlobalState, SPEECH) == 0)  )
DTX_DEC_L016:                           //      {
    VoC_lw16_sd(REG0,6,DEFAULT);            //         Copy(st->lsp, st->lsp_old, M);
    VoC_lw16_sd(REG1,5,DEFAULT);
    VoC_jal(CII_Copy_M);
    VoC_sw16_sd(REG7,7,DEFAULT);            //         st->old_log_en = st->log_en;
    //      }
DTX_DEC_L011:                   //   }
    VoC_shr16_ri(REG7,1,DEFAULT);       //   ma_pred_init = sub(shr(st->log_en,1), 9000);
    VoC_lw16i_set_inc(REG0,STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR,1);
    VoC_jal(CII_dtx_com);                   //}

    /*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
DTX_DEC_L010:
    VoC_pop16(REG5,DEFAULT);            //  pop16 stack[n-12]
    VoC_lw16_sd(REG4,0,DEFAULT);
    //st->log_en_adjust = add(mult(st->log_en_adjust, 29491),
    VoC_multf16_rd(REG4,REG4,CONST_29491_ADDR); //     shr(mult(shl(dtx_log_en_adjust[mode],5),3277),5));
    VoC_multf16_rd(REG5,REG5,CONST_3277_ADDR);
    VoC_lw16_sd(REG3,9,DEFAULT);        //int_fac = shl(add(1,st->since_last_sid), 10);
    VoC_add16_rd(REG3,REG3,CONST_1_ADDR);
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_shr16_ri(REG3,-10,IN_PARALLEL);     // int_fac in reg3
    VoC_lw16_sd(REG5,8,DEFAULT);

    VoC_multf16_rr(REG3,REG3,REG5,DEFAULT); //int_fac = mult(int_fac, st->true_sid_period_inv);
    VoC_sw16_sd(REG4,0,DEFAULT);
    VoC_bngt16_rd(DTX_DEC_L020,REG3,CONST_0x400_ADDR);      //if (sub(int_fac, 1024) > 0)
    VoC_lw16i(REG3,1024);       //{  int_fac = 1024; }
DTX_DEC_L020:
    VoC_shr16_ri(REG3,-4,DEFAULT);      //int_fac = shl(int_fac, 4);
    VoC_sub16_dr(REG4,CONST_0x4000_ADDR,REG3);      //int_fac = sub(16384, int_fac);
    VoC_lw16_sd(REG6,7,DEFAULT);        //L_log_en_int = L_mult(int_fac, st->log_en);
    VoC_lw16_sd(REG5,6,DEFAULT);
    VoC_multf32_rr(ACC0,REG3,REG6,IN_PARALLEL);
    VoC_lw16i_set_inc(REG2,dtx_dec_LSP_INT_ADDR,1);
    VoC_mac32_rr(REG4,REG5,DEFAULT);        //L_log_en_int = L_mac(L_log_en_int, int_fac, st->old_log_en);
    VoC_lw16_sd(REG0,5,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_lw16_sd(REG1,4,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_push16(REG2,DEFAULT);           //  push16 stack[n-12]  lsp_int[]
    VoC_push32(ACC0,DEFAULT);           // push32 stack32[n-1]  L_log_en_int

    VoC_loop_i_short(10,DEFAULT);       //for(i = 0; i < M; i++)
    VoC_multf16inc_rp(REG7,REG3,REG0,IN_PARALLEL);
    VoC_startloop0              //{
    VoC_multf16inc_rp(REG6,REG4,REG1,DEFAULT);  //   lsp_int[i] = mult(int_fac, st->lsp[i]);
    VoC_NOP();//change
    VoC_add16_rr(REG6,REG6,REG7,DEFAULT);   //   lsp_int[i] = add(lsp_int[i], mult(int_fac, st->lsp_old[i]));
    VoC_shr16_ri(REG6,-1,DEFAULT);      //   lsp_int[i] = shl(lsp_int[i], 1);
    VoC_multf16inc_rp(REG7,REG3,REG0,DEFAULT);
    VoC_sw16inc_p(REG6,REG2,DEFAULT);
    VoC_endloop0                    //}

    VoC_lw16i(REG7,2457);
    VoC_sub16_dr(REG7,STRUCT_DTX_DECSTATE_LOG_PG_MEAN_ADDR,REG7);//lsf_variab_factor = sub(st->log_pg_mean,2457);
    VoC_multf16_rd(REG7,REG7,CONST_0x2666_ADDR);
    VoC_lw16i(REG6,4096);
    VoC_sub16_rr(REG7,REG6,REG7,DEFAULT);       //lsf_variab_factor = sub(4096, mult(lsf_variab_factor, 9830));

    VoC_bngt16_rr(DTX_DEC_L021,REG7,REG6);  //if (sub(lsf_variab_factor, 4096) > 0)
    VoC_movreg16(REG7,REG6,DEFAULT);                //{ lsf_variab_factor = 4096;}
DTX_DEC_L021:
    VoC_bnltz16_r(DTX_DEC_L022,REG7);       //if (lsf_variab_factor < 0)
    VoC_lw16i_short(REG7,0,DEFAULT);        //{ lsf_variab_factor = 0; }
DTX_DEC_L022:
    VoC_shr16_ri(REG7,-3,DEFAULT);          //lsf_variab_factor = shl(lsf_variab_factor, 3);
    VoC_lw16i_set_inc(REG1,dtx_dec_LSF_INT_ADDR,-1);
    VoC_lw32_sd(ACC0,1,DEFAULT);
    VoC_push16(REG1,DEFAULT);           // push16 stack[n-13]  lsf_int[]
    VoC_push16(REG7,DEFAULT);           //push16 stack[n-14]
    VoC_lw16i_short(REG0,3,IN_PARALLEL);
    VoC_jal(CII_pseudonoise);               //lsf_variab_index = pseudonoise(&st->L_pn_seed_rx, 3);
    VoC_sw32_sd(ACC0,1,DEFAULT);
    VoC_mult16_rd(REG7,REG7,CONST_10_ADDR);
    VoC_lw16_sd(REG0,2,DEFAULT);            // lsp_int[]
    VoC_lw16i_short(INC0,-1,IN_PARALLEL);
    VoC_lw16_sd(REG1,1,DEFAULT);            // lsf_int[]
    VoC_push16(REG7,DEFAULT);           //push16 stack[n-15]

    VoC_jal(CII_Lsp_lsf);           //Lsp_lsf(lsp_int, lsf_int, M);

    VoC_pop16(REG6,DEFAULT);            // pop16 stack[n-15]
    VoC_pop16(REG7,DEFAULT);            // pop16 stack[n-14]
    VoC_lw16_sd(REG2,4,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_lw16i_set_inc(REG0,dtx_dec_lsf_int_variab_ADDR,1);
    VoC_lw16_sd(REG1,0,DEFAULT);        //Copy(lsf_int, lsf_int_variab, M);

    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_push16(REG0,DEFAULT);           // push16 stack[n-14]   lsf_int_variab[]
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_multf16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_loop_i_short(10,DEFAULT);//for(i = 0; i < M; i++)
    //VoC_multf16inc_rp(REG5,REG7,REG2,IN_PARALLEL);
    VoC_startloop0      //{
    //   lsf_int_variab[i] = add(lsf_int_variab[i],
    VoC_add16inc_rp(REG5,REG5,REG1,DEFAULT);    //           mult(lsf_variab_factor,st->lsf_hist_mean[i+lsf_variab_index*M]));
    VoC_multf16inc_rp(REG5,REG7,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0        //}

    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_jal(CII_Reorder_lsf);       //Reorder_lsf(lsf_int, LSF_GAP, M);
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_jal(CII_Reorder_lsf);       //Reorder_lsf(lsf_int_variab, LSF_GAP, M);

    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_lw16i_short(REG6,10,IN_PARALLEL);
    VoC_lw16i_set_inc(REG3,STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR,1);
    VoC_jal(CII_R02R3_p_R6);            //Copy(lsf_int, lsfState->past_lsf_q, M);

    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_lw16_sd(REG1,2,DEFAULT);
    VoC_jal(CII_Lsf_lsp);       //Lsf_lsp(lsf_int, lsp_int, M);

    VoC_lw16i_set_inc(REG1,dtx_dec_lsp_int_variab_ADDR,1);
    VoC_pop16(REG0,DEFAULT);        // pop16 stack[n-14]  pop lsf_int_variab[]
    VoC_push16(REG1,DEFAULT);       // push16 stack[n-14]  lsp_int_variab[]
    VoC_jal(CII_Lsf_lsp);       //Lsf_lsp(lsf_int_variab, lsp_int_variab, M);

    VoC_lw16i_set_inc(REG1,dtx_dec_acoeff_variab_ADDR,2);
    VoC_lw16i_set_inc(REG0,dtx_dec_acoeff_ADDR,1);
    VoC_lw16_sd(REG1,2,DEFAULT);
    exit_on_warnings =OFF;
    //  VoC_NOP();
    VoC_push32(REG01,DEFAULT);      // push32 stack32[n-2]  acoeff_variab/acoeff
    exit_on_warnings =ON;
    VoC_jal(CII_Lsp_Az);            //Lsp_Az(lsp_int, acoeff);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_movreg16(REG0,REG1,DEFAULT);
    VoC_pop16(REG1,DEFAULT);        // pop16 stack[n-14]  lsp_int_variab[]
    VoC_pop16(REG2,DEFAULT);        // pop16 stack[n-13]  lsf_int[]
    VoC_pop16(REG2,DEFAULT);        // pop16 stack[n-12]  lsp_int[]
    VoC_jal(CII_Lsp_Az);            //Lsp_Az(lsp_int_variab, acoeff_variab);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_lw16i_short(REG7,11,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,DEC_AMR_A_T_ADDRESS,1);

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_add16_rr(REG2,REG1,REG7,DEFAULT);
    VoC_add16_rr(REG3,REG2,REG7,DEFAULT);
    VoC_loop_i_short(11,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_add16_rr(REG4,REG3,REG7,DEFAULT);
    VoC_sw16inc_p(REG6,REG1,DEFAULT);   //Copy(acoeff, &A_t[0],           M + 1);
    VoC_sw16inc_p(REG6,REG2,DEFAULT);   //Copy(acoeff, &A_t[M + 1],       M + 1);
    VoC_sw16inc_p(REG6,REG3,DEFAULT);   //Copy(acoeff, &A_t[2 * (M + 1)], M + 1);
    VoC_sw16inc_p(REG6,REG4,DEFAULT);   //Copy(acoeff, &A_t[3 * (M + 1)], M + 1);
    VoC_endloop0

    VoC_lw16i(REG2,dtx_dec_refl_ADDR);
    VoC_lw16i_set_inc(REG3,DEC_AMR_TEMP10_ADDRESS,1);
    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);

    //A_Refl(&acoeff[1], refl);
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
// begin of  A_Refl

    VoC_lw16i_set_inc(REG2,9,-1);
    VoC_push16(REG3,DEFAULT);                //  push16 stack[n-1]
    VoC_inc_p(REG0,IN_PARALLEL);

    VoC_loop_i_short(10,DEFAULT);            //for (i = 0; i < M; i++)
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
    VoC_startloop0;                  //{ aState[i] = a[i];}
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_loop_i(1,10);                    //for (i = M-1; i >= 0; i--)
    VoC_lw16_sd(REG3,0,DEFAULT);          //{
    VoC_add16_rr(REG6,REG3,REG2,DEFAULT);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_movreg16(REG7,REG6,DEFAULT);
    VoC_bgtz16_r(A_refl_L0,REG6);         //   if (sub(abs_s(aState[i]), 4096) >= 0)
    VoC_sub16_rr(REG6,REG0,REG6,DEFAULT);
A_refl_L0:
    VoC_blt16_rd(A_refl_L1,REG6,CONST_4096_ADDR);
    VoC_jump(ExitRefl);               //   {     goto ExitRefl;    }
A_refl_L1:
    VoC_lw16_sd(REG6,1,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG2,DEFAULT);
    VoC_shr16_ri(REG7,-3,IN_PARALLEL);        //   refl[i] = shl(aState[i], 3);

    VoC_multf32_rr(ACC0,REG7,REG7,DEFAULT);       //   L_temp = L_mult(refl[i], refl[i]);
    VoC_push16(REG2,DEFAULT);
    VoC_sw16_p(REG7,REG6,DEFAULT);
    VoC_sub32_dr(ACC0,CONST_0x7FFFFFFF_ADDR,ACC0);    //   L_acc = L_sub(MAX_32, L_temp);
    VoC_jal(CII_NORM_L_ACC0);             //   normShift = norm_l(L_acc); L_acc = L_shl(L_acc, normShift);
    VoC_sub16_dr(REG3,CONST_15_ADDR,REG1);        //   scale = sub(15, normShift);
    VoC_add32_rd(REG01,ACC0,CONST_0x00008000_ADDR);   //   normProd = round(L_acc);

    VoC_lw16i(REG0,16384);                //   mult = div_s(16384, normProd);
    VoC_jal(CII_DIV_S);               //  Return REG2,  REG4->mult
    VoC_movreg16(REG4,REG2,DEFAULT);
    VoC_pop16(REG2,DEFAULT);

    VoC_bngtz16_r(A_refl_L3,REG2);
    VoC_loop_r_short(REG2,DEFAULT);           //   for (j = 0; j < i; j++)
    VoC_lw16i_short(REG0,0,IN_PARALLEL);
    VoC_startloop0                  //   {
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_add16_rr(REG5,REG1,REG0,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG6,IN_PARALLEL);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG2,IN_PARALLEL);
    VoC_lw16_p(ACC0_HI,REG5,DEFAULT);           //      L_acc = L_deposit_h(aState[j]);
    VoC_sub16_rr(REG1,REG1,REG0,IN_PARALLEL);//      L_acc = L_msu(L_acc, refl[i], aState[i-j-1]);

    VoC_push32(REG23,DEFAULT);

    VoC_lw16_p(REG6,REG1,DEFAULT);
    VoC_msu32_rr(REG6,REG7,DEFAULT);

    VoC_sub16_rd(REG5,REG3,CONST_1_ADDR);
    VoC_add32_rd(REG23,ACC0,CONST_0x00008000_ADDR);//      temp = round(L_acc);
    VoC_multf32_rr(ACC0,REG4,REG3,DEFAULT);     //      L_temp = L_mult(mult, temp);
    // VoC_jal(CII_L_shr_r);                //      L_temp = L_shr_r(L_temp, scale);
    VoC_pop32(REG23,DEFAULT);
    VoC_shr32_rr(ACC0,REG5,DEFAULT);
    VoC_add32_rd(ACC0,ACC0,CONST_1_ADDR);
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_movreg16(REG5,ACC0_LO,DEFAULT);
    VoC_bgtz32_r(A_refl_Lab1,ACC0);     //      if (L_sub(L_abs(L_temp), 32767) > 0)
    VoC_sub32_dr(ACC0,CONST_0_ADDR,ACC0);
A_refl_Lab1:
    VoC_bgt32_rd(ExitRefl,ACC0,CONST_0x00007FFF_ADDR);  //      {       goto ExitRefl;       }

    VoC_lw16i(REG1,DEC_AMR_TEMP20_ADDRESS);     //      bState[j] = extract_l(L_temp);
    VoC_add16_rr(REG1,REG1,REG0,DEFAULT);
    VoC_inc_p(REG0,DEFAULT);
    VoC_sw16_p(REG5,REG1,DEFAULT);
    VoC_endloop0                        //   }

    VoC_lw16i_set_inc(REG1,DEC_AMR_TEMP20_ADDRESS,1);
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_loop_r_short(REG2,DEFAULT);               //   for (j = 0; j < i; j++)
    VoC_lw16inc_p(REG6,REG1,IN_PARALLEL);
    VoC_startloop0                       //   {
    VoC_lw16inc_p(REG6,REG1,DEFAULT);          //      aState[j] = bState[j];
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0                     //   }
A_refl_L3:
    VoC_sub16_rd(REG2,REG2,CONST_1_ADDR);
    VoC_endloop1                        //}
    VoC_jump(A_refl_End);                            //return;

ExitRefl:
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16_sd(REG1,1,DEFAULT);
    VoC_loop_i_short(10,DEFAULT);                    //for (i = 0; i < M; i++)
    VoC_startloop0                      //{
    VoC_sw16inc_p(REG0,REG1,DEFAULT);               //   refl[i] = 0;  move16 ();
    VoC_endloop0                        //}
A_refl_End:
    VoC_pop16(REG3,DEFAULT);
// end of A_Refl
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    VoC_lw16i(REG2,0x7fff);         //pred_err = MAX_16;
    VoC_pop16(REG0,DEFAULT);
    VoC_movreg16(REG5,REG2,IN_PARALLEL);
    VoC_lw16i_short(ACC0_HI,0,DEFAULT);

    VoC_multf16inc_pp(REG7,REG0,REG0,DEFAULT);  //   pred_err = mult(pred_err, sub(MAX_16, mult(refl[i], refl[i])));

    VoC_loop_i_short(10,DEFAULT);       //for (i = 0; i < M; i++)
    VoC_startloop0              //{
    VoC_sub16_rr(REG7,REG5,REG7,DEFAULT);
    VoC_multf16inc_pp(REG7,REG0,REG0,DEFAULT);  //   pred_err = mult(pred_err, sub(MAX_16, mult(refl[i], refl[i])));
    exit_on_warnings =OFF;
    VoC_multf16_rr(REG2,REG7,REG2,DEFAULT);
    exit_on_warnings =ON;
    VoC_endloop0                    //}
    VoC_NOP();
    VoC_movreg16(ACC0_LO,REG2,DEFAULT);
    VoC_jal(CII_Log2);              // Log2(L_deposit_l(pred_err), &log_pg_e, &log_pg_m);

    VoC_sub16_rd(REG4,REG4,CONST_15_ADDR);      // log_pg = shl(sub(log_pg_e,15), 12);
    VoC_shr16_ri(REG4,-12,DEFAULT);
    VoC_shr16_ri(REG5,3,IN_PARALLEL);       //log_pg = shr(sub(0,add(log_pg, shr(log_pg_m, 15-12))), 1);
    VoC_add16_rr(REG4,REG5,REG4,DEFAULT);
    VoC_sub16_dr(REG4,CONST_0_ADDR,REG4);
    VoC_shr16_ri(REG4,1,DEFAULT);

    VoC_lw16i(REG6,29491);
    VoC_multf16_rd(REG5,REG4,CONST_3277_ADDR);  //st->log_pg_mean = add(mult(29491,st->log_pg_mean), mult(3277, log_pg));
    VoC_multf16_rd(REG3,REG6,STRUCT_DTX_DECSTATE_LOG_PG_MEAN_ADDR);

    VoC_lw32_sd(ACC0,1,DEFAULT);
    VoC_add16_rr(REG3,REG5,REG3,DEFAULT);

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_short(REG2,0,IN_PARALLEL);

    VoC_sw16_d(REG3,STRUCT_DTX_DECSTATE_LOG_PG_MEAN_ADDR);

    VoC_shr32_ri(ACC0,10,DEFAULT);      //L_log_en_int = L_shr(L_log_en_int, 10);
    VoC_lw16i_short(REG3,4,IN_PARALLEL);
    VoC_add32_rr(ACC0,ACC0,REG23,DEFAULT);      //L_log_en_int = L_add(L_log_en_int, 4 * 65536L);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_bnltz16_r(DTX_DEC_LA021,REG4);
    VoC_lw16i_short(REG5,-1,DEFAULT);
DTX_DEC_LA021:
    VoC_shr32_ri(REG45,-4,DEFAULT);     //L_log_en_int = L_sub(L_log_en_int, L_shl(L_deposit_l(log_pg), 4));
    VoC_pop16(REG2,DEFAULT);        // pop16 stack[n-11] st->log_en_adjust
    VoC_sub32_rr(ACC0,ACC0,REG45,DEFAULT);

    VoC_sw16_d(REG2,STRUCT_DTX_DECSTATE_LOG_EN_ADJUST_ADDR);
    VoC_bnltz16_r(DTX_DEC_LA022,REG2);
    VoC_lw16i_short(REG3,-1,DEFAULT);
DTX_DEC_LA022:
    VoC_shr32_ri(REG23,-5,DEFAULT);     //L_log_en_int = L_add(L_log_en_int,L_shl(L_deposit_l(st->log_en_adjust), 5));
    VoC_add32_rr(ACC0,ACC0,REG23,DEFAULT);

    VoC_movreg16(REG1,ACC0_HI,DEFAULT);     //log_en_int_e = extract_h(L_log_en_int);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);
    VoC_sub32_rr(ACC0,ACC0,REG01,DEFAULT);  //log_en_int_m = extract_l(L_shr(L_sub(L_log_en_int, L_deposit_h(log_en_int_e)), 1));
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_movreg16(REG0,ACC0_LO,DEFAULT);
    VoC_jal(CII_Pow2);              //level = extract_l(Pow2(log_en_int_e, log_en_int_m));

    VoC_lw16i_set_inc(REG3,dtx_dec_ex_ADDR,1);
    VoC_push16(REG2,DEFAULT);           // push16  stack[n-11] level
    VoC_lw16i_short(REG0,0,IN_PARALLEL);
    VoC_push16(REG3,DEFAULT);           // push16  stack[n-12] ex[]
DTX_DEC_L023:
//   VoC_bnlt16_rd(DTX_DEC_L024,REG0,CONST_4_ADDR);//for (i = 0; i < 4; i++)
    VoC_push16(REG0,DEFAULT);           // push16  stack[n-13] i
    VoC_lw32_sd(ACC0,2,DEFAULT);        //{
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_jal(CII_build_CN_code);

    VoC_sw32_sd(ACC0,2,DEFAULT);

    VoC_lw16_sd(REG3,1,DEFAULT);

    VoC_lw16_sd(REG4,2,DEFAULT);
    VoC_movreg16(REG0,REG3,IN_PARALLEL);
    VoC_multf16inc_rp(REG5,REG4,REG3,DEFAULT);

    VoC_loop_i_short(40,DEFAULT);       //   for (j = 0; j < L_SUBFR; j++)

    VoC_startloop0              //   {
    VoC_multf16inc_rp(REG5,REG4,REG3,DEFAULT);  //      ex[j] = mult(level, ex[j]);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_NOP();
    VoC_endloop0                    //   }

    VoC_lw16i_set_inc(REG1,STRUCT_DECOD_AMRSTATE_MEM_SYN_ADDR,1);
    VoC_lw32_sd(REG45,0,DEFAULT);
    VoC_lw16_sd(REG3,1,IN_PARALLEL);
    VoC_lw16_sd(REG7,15,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
    VoC_lw16_sd(REG4,0,DEFAULT);

    VoC_mult16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG4,DEFAULT);
//   VoC_push32(REG01,DEFAULT);
    VoC_jal(CII_Syn_filt);          //   Syn_filt(acoeff_variab, ex, &synth[i * L_SUBFR], L_SUBFR, mem_syn, 1);
    VoC_pop16(REG0,DEFAULT);            // pop16  stack[n-13] i
//    VoC_pop32(RL6,IN_PARALLEL);
    VoC_add16_rd(REG0,REG0,CONST_1_ADDR);       //}
//   VoC_jump(DTX_DEC_L023);
    VoC_blt16_rd(DTX_DEC_L023,REG0,CONST_4_ADDR)
//DTX_DEC_L024:
    VoC_pop32(ACC0,DEFAULT);        // pop32 stack32[n-2]
    VoC_pop16(REG2,IN_PARALLEL);        // pop16  stack[n-12] ex[]
    VoC_pop16(REG3,DEFAULT);        // pop16  stack[n-11] level
    VoC_pop32(ACC0,IN_PARALLEL);        // pop32 stack32[n-1]

    VoC_lw16i_short(REG5,20,DEFAULT);       //averState->hangVar = 20;
    VoC_lw16i_short(REG4,0,IN_PARALLEL);    //averState->hangCount = 0;
    VoC_lw16i_short(REG3,2,DEFAULT);
    VoC_sw32_d(REG45,STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR);

    VoC_bne16_rd(DTX_DEC_L025,REG3,DEC_AMR_NEWDTXSTATE_ADDRESS);//if (sub(new_state, DTX_MUTE) == 0)
    VoC_lw16_sd(REG1,8,DEFAULT);                //{   tmp_int_length = st->since_last_sid;

    VoC_bngt16_rd(DTX_DEC_L026,REG1,CONST_32_ADDR);     //   if (sub(tmp_int_length, 32) > 0)
    VoC_lw16i(REG1, 32);
    //   {  tmp_int_length = 32;   }
DTX_DEC_L026:

    VoC_bgtz16_r(DTX_DEC_L027,REG1);                //   if(tmp_int_length <= 0) {
    VoC_lw16i_short(REG1,8,DEFAULT);                //      tmp_int_length = 8;    }
DTX_DEC_L027:

    VoC_shr16_ri(REG1,-10,DEFAULT);
    VoC_lw16i(REG0,0x0400);
    VoC_jal(CII_DIV_S);                     //   st->true_sid_period_inv = div_s(1 << 10, shl(tmp_int_length, 10));
    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_sw16_sd(REG2,7,DEFAULT);
    VoC_sw16_sd(REG1,8,DEFAULT);                //   st->since_last_sid = 0;

    VoC_lw16_sd(REG0,4,DEFAULT);
    VoC_lw16_sd(REG1,3,DEFAULT);

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_jal(CII_Copy_M);                    //   Copy(st->lsp, st->lsp_old, M);

    VoC_lw16_sd(REG4,6,DEFAULT);        //   st->old_log_en = st->log_en;
    VoC_sub16_rd(REG5,REG4,CONST_256_ADDR); //   st->log_en = sub(st->log_en, 256);
    VoC_sw16_sd(REG4,5,DEFAULT);
    VoC_sw16_sd(REG5,6,DEFAULT);        //}
DTX_DEC_L025:
    VoC_bez16_d(DTX_DEC_L029,STRUCT_DTX_DECSTATE_SID_FRAME_ADDR);   //if ((st->sid_frame != 0) &&
    VoC_bnez16_d(DTX_DEC_L028,STRUCT_DTX_DECSTATE_VALID_DATA_ADDR); //    ((st->valid_data != 0) ||
    VoC_bez16_d(DTX_DEC_L029,STRUCT_DTX_DECSTATE_DTXHANGOVERADDED_ADDR) //     ((st->valid_data == 0) &&  (st->dtxHangoverAdded) != 0)))
DTX_DEC_L028:                   //{
    VoC_lw16i_short(REG0,0,DEFAULT);        //   st->since_last_sid =  0;
    VoC_lw16i_short(REG1,1,DEFAULT);    //   st->data_updated = 1;
    VoC_sw16_sd(REG0,8,DEFAULT);
    VoC_sw16_d(REG1,STRUCT_DTX_DECSTATE_DATA_UPDATED_ADDR);
    //}
DTX_DEC_L029:
    VoC_loop_i_short(6,DEFAULT);
    VoC_startloop0
    VoC_pop16(REG5,DEFAULT);    //pop16 stack[n-10]~ stack[n-5]
    VoC_endloop0
    VoC_pop16(REG4,DEFAULT);        //pop16 stack[n-4]  The value of st->log_en
    VoC_pop32(ACC0,IN_PARALLEL);    // pop32 stack32[n]

    VoC_pop16(REG3,DEFAULT);            //pop16 stack[n-3]  The value of st->true_sid_period_inv
    VoC_pop16(REG2,DEFAULT);            //pop16 stack[n-2]  The value of st->since_last_sid
    VoC_pop16(REG0,DEFAULT);                    //pop16 stack[n-1]  The beginning address of dtx_decState
    VoC_sw32_d(ACC0,STRUCT_DTX_DECSTATE_L_PN_SEED_RX_ADDR);
    VoC_pop16(RA,DEFAULT);
    VoC_sw32inc_p(REG23,REG0,DEFAULT);
    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_return;
}

//*in0 in REG5
//*in1 in REG1
//data in REG2
void CII_filter3(void)
{

    VoC_multf16_rp(REG7, REG4, REG2, DEFAULT);
    VoC_inc_p(REG1,DEFAULT);

    VoC_sub16_pr(REG7, REG1, REG7, DEFAULT);//temp1 in REG7
    VoC_multf16_rr(REG6, REG4, REG7, DEFAULT);
    VoC_NOP();
    VoC_add16_rp(REG6, REG6, REG2, DEFAULT);//temp2 in REG6

    VoC_sw16inc_p(REG7, REG2, DEFAULT);
    VoC_sub16_pr(REG3, REG0, REG6, DEFAULT);
    VoC_add16_rp(REG5, REG6, REG0, IN_PARALLEL);
    VoC_shr16_ri(REG5, 1, DEFAULT);
    VoC_shr16_ri(REG3, 1, DEFAULT);
    VoC_sw16inc_p(REG5, REG0,DEFAULT);
    VoC_sw16_p(REG3, REG1,DEFAULT);
    VoC_movreg16(REG1, REG0,IN_PARALLEL);
    VoC_return;
}

void CII_vad1(void)
{

#if 0
    voc_short    VAD1_level_ADDR           ,11,y
    voc_short    VAD1_decission_noise_level_ADDR           ,y
    voc_short    VAD1_low_power_flag_ADDR           ,y


#endif


    VoC_push16(RA, DEFAULT);
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_set_inc(REG0, STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR-40, 2);

    VoC_loop_i(0, 80)
    VoC_bimac32inc_pp(REG0,REG0);
    VoC_endloop0
    VoC_lw16i(REG3, 0x3fff);
    VoC_lw16i(REG4, 0x3C00);
    VoC_lw16_d(REG6, STRUCT_VADSTATE1_PITCH_ADDR);//REG6  ,st->pitch   REG7   st->complex_low
    VoC_lw16_d(REG7, STRUCT_VADSTATE1_COMPLEX_LOW_ADDR);
    VoC_and16_rr(REG6, REG3,DEFAULT);
    VoC_lw16i_short(REG5, 0x5, IN_PARALLEL);
    VoC_and16_rr(REG7, REG3,DEFAULT);

    VoC_bngt32_rr(VAD_L2, REG45, ACC0);
    VoC_sw16_d(REG6, STRUCT_VADSTATE1_PITCH_ADDR);
VAD_L2:
    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_bnlt32_rd(VAD_M2, ACC0,CONST_15000_ADDR );
    VoC_sw16_d(REG7,STRUCT_VADSTATE1_COMPLEX_LOW_ADDR);
VAD_M2:

    VoC_push32(ACC0,DEFAULT);   // save pow_sum

//void CII_filter_bank(void)

///Function   CII_filter_bank  begin!
    {
#if 0

        voc_short FILTER_BANK_tmp_buf_ADDR            ,160,x

#endif

//    CII_first_filter_stage
        VoC_lw16i_set_inc(REG0, STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR, 2);
        VoC_lw16i_set_inc(REG1, FILTER_BANK_tmp_buf_ADDR, 2);
        VoC_lw32_d(REG67, STRUCT_VADSTATE1_A_DATA5_ADDR);//data0 in REG6 | data1 in REG7
        // REG6 for data0 and temp0 REG7 for data1 and temp3
        VoC_lw16i(RL7_HI,6390);
        VoC_lw16i(RL7_LO,21955);
        VoC_lw32inc_p(REG23, REG0, DEFAULT);//in[4*i+0] in REG2,in[4*i+1] in REG3
        VoC_movreg16(REG4,RL7_LO,IN_PARALLEL);  ////COEFF5_1 in  REG4

        VoC_loop_i(1, 80)


        VoC_multf16_rr(REG5, REG4, REG6, DEFAULT);  // mult(COEFF5_1, data0)
        VoC_shr16_ri(REG2, 2, DEFAULT);
        VoC_sub16_rr(REG5, REG2, REG5, DEFAULT);//temp0 in REG5

        VoC_multf16_rr(REG4, REG4, REG5, DEFAULT);  //mult(COEFF5_1, temp0) in REG5
        VoC_NOP();
        VoC_add16_rr(REG4, REG4, REG6, DEFAULT);//temp1 in REG4
        VoC_movreg16(REG6,REG5,DEFAULT);//temp0 in REG6

        VoC_movreg16(REG5, RL7_HI,DEFAULT);//COEFF5_2 in REG5

        VoC_multf16_rr(REG2, REG5, REG7, DEFAULT);  // mult(COEFF5_2, data1)
        VoC_shr16_ri(REG3, 2, DEFAULT);
        VoC_sub16_rr(REG2, REG3, REG2, DEFAULT);//temp3 in REG2
        VoC_multf16_rr(REG3, REG5, REG2, DEFAULT);  //mult(COEFF5_2, temp3) in REG3
        VoC_NOP();
        VoC_add16_rr(REG5, REG3, REG7, DEFAULT);//temp2 in REG5
        VoC_movreg16(REG7,REG2,IN_PARALLEL);//temp3 in REG7
        VoC_add16_rr(REG2,REG4,REG5,DEFAULT);
        VoC_sub16_rr(REG3,REG4,REG5,IN_PARALLEL);
        exit_on_warnings=OFF;
        VoC_lw32inc_p(REG23, REG0, DEFAULT);//in[4*i+0] in REG2,in[4*i+1] in REG3
        VoC_sw32inc_p(REG23, REG1, DEFAULT);
        VoC_movreg16(REG4,RL7_LO,IN_PARALLEL);  ////COEFF5_1 in  REG4
        exit_on_warnings=ON;
        VoC_endloop1
        VoC_sw32_d(REG67, STRUCT_VADSTATE1_A_DATA5_ADDR);

    }

///Function   CII_filter_bank  end!
//void CII_filter5(void)
    VoC_lw16i_set_inc(REG0, FILTER_BANK_tmp_buf_ADDR, 2);

FILTER5_LOOP_START:
    VoC_lw16i_set_inc(REG1, STRUCT_VADSTATE1_A_DATA5_ADDR+2, 1);
    VoC_loop_i(1, 2)
    VoC_movreg16(REG4,RL7_LO,DEFAULT);  ////COEFF5_1 in  REG4
    VoC_loop_i_short(2, DEFAULT)
    VoC_movreg16(REG2, REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_multf16_rp(REG3, REG4, REG1, DEFAULT);
    VoC_NOP();
    VoC_sub16inc_pr(REG3, REG0, REG3, DEFAULT);//temp0 in REG3  //temp3
    VoC_multf16_rr(REG5, REG4, REG3, DEFAULT);
    VoC_movreg16(REG4,RL7_HI,DEFAULT);  ////COEFF5_2 in  REG4
    VoC_add16_rp(REG5, REG5, REG1, DEFAULT);//temp1 in REG5  //temp2
    VoC_sw16inc_p(REG3, REG1, DEFAULT);
    VoC_movreg16(REG7, REG6,DEFAULT);
    VoC_movreg16(REG6, REG5,IN_PARALLEL);
    VoC_endloop0
    //REG7    temp1     REG6     temp2
    VoC_add16_rr(REG5, REG7, REG6, DEFAULT);
    VoC_sub16_rr(REG4, REG7, REG6, IN_PARALLEL);
    VoC_shr16_ri(REG5, 1, DEFAULT);
    VoC_shr16_ri(REG4, 1, IN_PARALLEL);
    VoC_sub16_rd(REG0, REG0, CONST_3_ADDR);
    VoC_sw16inc_p(REG5, REG2, DEFAULT);
    VoC_sw16inc_p(REG4, REG2, DEFAULT);
    VoC_endloop1
    VoC_lw16i(REG4,FILTER_BANK_tmp_buf_ADDR);
    VoC_sub16_rr(REG2, REG2, REG4, DEFAULT);
    VoC_add16_rd(REG0, REG0, CONST_2_ADDR);

    VoC_blt16_rd(FILTER5_LOOP_START, REG2, CONST_160_ADDR)


//  for (i = 0; i < FRAME_LEN/8; i++)
    VoC_lw16i_set_inc(REG0, FILTER_BANK_tmp_buf_ADDR, 2);

    VoC_lw16i(REG4, 13363);
    VoC_loop_i_short(20,DEFAULT)
    VoC_lw16i_short(INC1,4,IN_PARALLEL);
    VoC_startloop0

    VoC_lw16i_set_inc(REG2, STRUCT_VADSTATE1_A_DATA3_ADDR,1);
    VoC_movreg16(REG1, REG0,DEFAULT);

    VoC_jal(CII_filter3);
    VoC_jal(CII_filter3);
    VoC_sub32_rd(REG01, REG01, CONST_0x00010001_ADDR);
    VoC_add16_rd(REG2, REG2, CONST_2_ADDR);
    VoC_jal(CII_filter3);
    VoC_add16_rd(REG0, REG0, CONST_3_ADDR);
    VoC_endloop0

//  for (i = 0; i < FRAME_LEN/16; i++)
    VoC_lw16i_set_inc(REG0, FILTER_BANK_tmp_buf_ADDR, 4);

    VoC_loop_i_short(10,DEFAULT)
    VoC_lw16i_short(INC1,8,IN_PARALLEL);
    VoC_startloop0

    VoC_lw16i_set_inc(REG2, STRUCT_VADSTATE1_A_DATA3_ADDR+2, 1);
    VoC_movreg16(REG1, REG0,DEFAULT);

    VoC_jal(CII_filter3);
    VoC_jal(CII_filter3);
    VoC_add16_rd(REG0, REG0, CONST_8_ADDR);
    VoC_endloop0

    /**************************/
//void CII_level_calculation(void)
    /**************************/
// REG6 count1 REG7 count2-count1 REG3 data[ind_m*i+ind_a] address inc should be conform when transfer
// REG4 for scale REG1 for output address inc 1  REG2 for *sub_level address inc -1
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_set_inc(REG1, VAD1_level_ADDR+8,-1);
    VoC_lw16i_set_inc(REG2, STRUCT_VADSTATE1_SUB_LEVEL_ADDR+8, -1);
    VoC_lw16i_set_inc(REG0, PRAM4_TABLE_ADDR,1);//  ind_a address

    VoC_loop_i(1, 9)
    VoC_push16(REG1,DEFAULT);
    VoC_movreg16(RL6_HI,REG5,DEFAULT);
    VoC_lw16i_short(REG6,16,DEFAULT);   // count1 for LOOP 1,2,3,4
    VoC_lw16i_short(REG7,4,IN_PARALLEL);    // count2-count1 for LOOP 1,2,3,4
    VoC_lw16i_set_inc(REG3,FILTER_BANK_tmp_buf_ADDR,8);
//      VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_add16inc_rp(REG3,REG3,REG0,DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    // ind_m for LOOP 1,2,3,4
    VoC_lw16i_short(REG4,-16,DEFAULT);  // -scale for LOOP 1-8
    VoC_bnez16_r(level_calculation100,REG5)

    VoC_lw16i_short(REG7,8,DEFAULT);    // count2-count1 for LOOP 0
    VoC_lw16i_short(REG6,32,IN_PARALLEL);   // count1 for LOOP 0

    VoC_lw16i_short(INC3,4,DEFAULT);    // ind_m for LOOP 0
    VoC_lw16i_short(REG4,-15,IN_PARALLEL);  // -scale for LOOP 0
level_calculation100:
    VoC_blt16_rd(level_calculation101,REG5,CONST_5_ADDR)

    VoC_lw16i_short(REG6,8,DEFAULT);    // count1 for LOOP 5,6,7,8
    VoC_lw16i_short(REG7,2,IN_PARALLEL);    // count2-count1 for LOOP 5,6,7,8

    VoC_lw16i_short(INC3,16,DEFAULT);   // ind_m for LOOP 5,6,7,8
level_calculation101:
    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_lw32z(ACC0, IN_PARALLEL);

    VoC_loop_r_short(REG6,DEFAULT)
    VoC_lw16inc_p(REG6, REG3,IN_PARALLEL);
    VoC_startloop0
    VoC_bnltz16_r(level_calculation_abs1, REG6)
    VoC_sub16_dr(REG6,CONST_0_ADDR, REG6);
level_calculation_abs1:
    VoC_lw16inc_p(REG6, REG3,DEFAULT);
    VoC_mac32_rr(REG6, REG1,IN_PARALLEL);       //ACC1
    VoC_endloop0

    VoC_sub16_dr(REG5, CONST_fu16_ADDR, REG4);  //sub(16, scale)

    VoC_loop_r_short(REG7,DEFAULT)
    VoC_startloop0
    VoC_bnltz16_r(level_calculation_abs2, REG6)
    VoC_sub16_dr(REG6,CONST_0_ADDR, REG6);
level_calculation_abs2:
    VoC_mac32_rr(REG6, REG1,DEFAULT);       // ACC0
    VoC_lw16inc_p(REG6, REG3,IN_PARALLEL);
    VoC_endloop0

    VoC_lw16_p(REG6, REG2,DEFAULT);
    VoC_lw16i_short(REG7, 0, IN_PARALLEL);
    VoC_bnltz16_r(level_calculation_100, REG6)
    VoC_lw16i_short(REG7,0xffff,DEFAULT);
level_calculation_100:
    VoC_shr32_rr(REG67, REG5,DEFAULT);
    VoC_add32_rr(RL7, ACC0,REG67,DEFAULT);
    VoC_shr32_rr(ACC0, REG4,IN_PARALLEL);
    VoC_pop16(REG1,DEFAULT);
    VoC_add32_rr(REG67, ACC1,RL7,IN_PARALLEL);
    VoC_sw16inc_p(ACC0_HI, REG2, DEFAULT);
    VoC_shr32_rr(REG67, REG4,IN_PARALLEL);
    VoC_movreg16(REG5,RL6_HI,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_sw16inc_p(REG7, REG1, DEFAULT);
    VoC_add16_rr(REG5,REG5,REG6,IN_PARALLEL);
    VoC_endloop1
///Function   CII_filter_bank  end!

//  void  CII_vad_decision(void)  begin
// level -> REG1 inc=1
    VoC_lw16i_set_inc(REG1, VAD1_level_ADDR, 1);
    VoC_lw16i_set_inc(REG0,STRUCT_VADSTATE1_BCKR_EST_ADDR,1);
    VoC_lw32z(ACC1, DEFAULT);
    VoC_loop_i(1,9)
    VoC_lw16i_short(ACC0_LO, 0,DEFAULT);
    VoC_lw16inc_p(ACC0_HI, REG0, DEFAULT);//st->bckr_est[i] in REG3
    VoC_lw16inc_p(REG0, REG1, DEFAULT);
    exit_on_warnings=OFF;
    VoC_push32(REG01, DEFAULT);
    exit_on_warnings=ON;
    VoC_shr16_ri(REG0, 1, DEFAULT);
    VoC_jal(CII_NORM_L_ACC0);
    VoC_sub16_dr(REG6, CONST_5_ADDR, REG1);
    VoC_movreg16(REG1, ACC0_HI, DEFAULT);

    VoC_jal(CII_DIV_S);// temp in REG2
    VoC_shr16_rr(REG2, REG6, DEFAULT);//temp in REG2
    VoC_pop32(REG01, DEFAULT);
    VoC_mac32_rr(REG2, REG2, IN_PARALLEL);
    VoC_endloop1
    VoC_lw16i(REG4, 3641);//CONST_3641_ADDR is error
    VoC_shr32_ri(ACC1, -6, DEFAULT);
    VoC_movreg32(REG67, ACC1, DEFAULT);

    VoC_multf16_rr(REG7, REG7, REG4, DEFAULT);//snr_sum in reg7
    VoC_lw32z(REG45, IN_PARALLEL);
    VoC_lw16i_set_inc(REG0, STRUCT_VADSTATE1_BCKR_EST_ADDR, 1);

    VoC_loop_i_short(9, DEFAULT)
    VoC_lw32z(REG23, IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG2, REG0, DEFAULT);
    VoC_add32_rr(REG45, REG45, REG23, DEFAULT);
    VoC_endloop0
    VoC_shr32_ri(REG45,-13,DEFAULT);
    VoC_lw16i(REG6,0xf508);
//  VoC_multf16_rd(REG3, REG5, CONST_0xf508_ADDR);
    VoC_multf16_rr(REG3, REG5, REG6,DEFAULT);
    VoC_sw16_d(REG5, VAD1_decission_noise_level_ADDR);// noise_level in reg5
    VoC_add16_rd(REG5, REG3, CONST_1260_ADDR);//vad_thr in reg5
    VoC_lw16_d(REG6, STRUCT_VADSTATE1_VADREG_ADDR);
    VoC_bnlt16_rd(VAD_DECISION_GO, REG5, CONST_720_ADDR);
    VoC_lw16_d(REG5, CONST_720_ADDR);
VAD_DECISION_GO:

    VoC_shr16_ri(REG6, 1, DEFAULT);
    VoC_bngt16_rr(VAD_DECISION_OLY, REG7, REG5);
    VoC_or16_ri(REG6, 0x4000);
VAD_DECISION_OLY:
    VoC_pop32(RL6,DEFAULT);
    VoC_sw16_d(REG6,STRUCT_VADSTATE1_VADREG_ADDR);
    VoC_lw16i_short(REG6, 1, DEFAULT);   // low_power_flag in reg6
    VoC_blt32_rd(VAD_DECISION_RM, RL6, CONST_15000_ADDR);
    VoC_lw16i_short(REG6, 0, DEFAULT);
VAD_DECISION_RM:
    VoC_lw16i_short(ACC0_LO, 0, DEFAULT);
// complex_estimate_adapt start
    VoC_lw16_d(REG3, STRUCT_VADSTATE1_CORR_HP_FAST_ADDR);
//  VoC_lw16i(REG5, 19660);//CVAD_THRESH_ADAPT_HIGH
    VoC_lw16i(REG4, 2621);             //CVAD_ADAPT_FAST
    VoC_bngt16_rd(COMPLEX_ESTIMATE_110, REG3, CONST_0x4CCC_ADDR);
    VoC_lw16i(REG4, 6553);            //CVAD_ADAPT_REALLY_FAST
    VoC_bgt16_rd(COMPLEX_ESTIMATE_110, REG3, STRUCT_VADSTATE1_BEST_CORR_HP_ADDR)
    VoC_lw16i(REG4, 655);          //CVAD_ADAPT_SLOW
COMPLEX_ESTIMATE_110:

    VoC_movreg16(ACC0_HI, REG3,DEFAULT);
    VoC_msu32_rr(REG4, REG3, DEFAULT);
    VoC_mac32_rd(REG4, STRUCT_VADSTATE1_BEST_CORR_HP_ADDR);
    VoC_lw16i(REG7, 13106);
    VoC_add32_rd(REG23, ACC0, CONST_0x00008000L_ADDR);// st->corr_hp_fast in reg3
//begin of CII_complex_vad

    VoC_lw32_d(REG01, STRUCT_VADSTATE1_COMPLEX_HIGH_ADDR);

    VoC_bgt16_rr(COMPLEX_ESTIMATE_130,REG7, REG3);
    VoC_bez16_r(COMPLEX_ESTIMATE_140,REG6);
COMPLEX_ESTIMATE_130:
    VoC_movreg16(REG3, REG7,DEFAULT);
COMPLEX_ESTIMATE_140:
    // REG3 for st->corr_hp_fast
    // REG5 for 19660 to next function
// complex_estimate_adapt end
    VoC_shr16_ri(REG0, 1, DEFAULT);
    VoC_shr16_ri(REG1, 1, DEFAULT);

    VoC_sw16_d(REG3,STRUCT_VADSTATE1_CORR_HP_FAST_ADDR);


    VoC_bnez16_r(COMPLEX_VAD_L1, REG6);// low_power in reg6
    VoC_bngt16_rd(COMPLEX_VAD_LL1, REG3, CONST_0x4CCC_ADDR)
    VoC_or16_rd(REG0, CONST_0x4000_ADDR);
COMPLEX_VAD_LL1:
    VoC_bngt16_rd(COMPLEX_VAD_L1, REG3, CONST_0x3fff_ADDR);//CVAD_THRESH_ADAPT_LOW
    VoC_or16_rd(REG1, CONST_0x4000_ADDR);
COMPLEX_VAD_L1:
//  VoC_lw16i(REG5, 0x5999);
    VoC_lw16i_short(REG4, 1, DEFAULT);
    VoC_lw16i_short(REG2, 0, DEFAULT);
    VoC_bngt16_rd(COMPLEX_VAD_MM, REG3, CONST_0x5999_ADDR);
    VoC_add16_rd(REG2, REG4, STRUCT_VADSTATE1_COMPLEX_HANG_TIMER_ADDR);// st->complex_hang_timer = add(st->complex_hang_timer, 1);
COMPLEX_VAD_MM:
    VoC_sw32_d(REG01, STRUCT_VADSTATE1_COMPLEX_HIGH_ADDR);
    VoC_sw16_d(REG2, STRUCT_VADSTATE1_COMPLEX_HANG_TIMER_ADDR);
    VoC_lw16i(REG2,0x7f80);
//  VoC_and16_ri(REG0, 0x7f80);
    VoC_and16_rr(REG0, REG2,DEFAULT);
    VoC_and16_ri(REG1, 0x7FFF);
    VoC_be16_rr(COMPLEX_VAD_END, REG0, REG2);
//  VoC_be16_rd(COMPLEX_VAD_END, REG0, CONST_0x7f80_ADDR);
    VoC_be16_rd(COMPLEX_VAD_END, REG1, CONST_0x7FFF_ADDR);
    VoC_lw16i_short(REG4, 0, DEFAULT);
COMPLEX_VAD_END:

    VoC_sw16_d(REG6,VAD1_low_power_flag_ADDR);
    VoC_sw16_d(REG4, STRUCT_VADSTATE1_COMPLEX_WARNING_ADDR);
//end of CII_complex_vad

// void CII_noise_estimate_update(void)  begin
// void CII_update_cntrl (void)  begin

    VoC_bez16_d(UPDATE_CNTRL_JUMP_100, STRUCT_VADSTATE1_COMPLEX_WARNING_ADDR);
    VoC_lw16i_short(REG4, 5, DEFAULT);
    VoC_bngt16_rd(UPDATE_CNTRL_JUMP_100, REG4, STRUCT_VADSTATE1_STAT_COUNT_ADDR);
    VoC_sw16_d(REG4,STRUCT_VADSTATE1_STAT_COUNT_ADDR);
UPDATE_CNTRL_JUMP_100:
    VoC_lw16i_short(REG7, 20, DEFAULT);
    VoC_lw32_d(REG45, STRUCT_VADSTATE1_PITCH_ADDR);
    VoC_lw16i(REG1,0x7c00);
    VoC_and16_ri(REG4, 0x6000);
//  VoC_and16_ri(REG5,0x7c00);
    VoC_and16_rr(REG5,REG1,DEFAULT);
    VoC_lw16_d(REG6, STRUCT_VADSTATE1_VADREG_ADDR);

    VoC_movreg16(REG3, REG6,DEFAULT);
    VoC_and16_ri(REG6, 0x4000);    //REG6   st->vadreg & 0x4000
    VoC_and16_ri(REG3, 0x7f80); // st->vadreg & 0x7f80
    VoC_be16_rr(UPDATE_CNTRL_JUMP_200_go, REG5, REG1);
//  VoC_be16_rd(UPDATE_CNTRL_JUMP_200_go, REG5, CONST_0x7c00_ADDR);
    VoC_be16_rd(UPDATE_CNTRL_JUMP_200_go, REG4, CONST_0x6000_ADDR);

    VoC_bnez16_r(UPDATE_CNTRL_JUMP_200_no,REG3);
UPDATE_CNTRL_JUMP_200_go:
    VoC_jump(UPDATE_CNTRL_JUMP_200);
UPDATE_CNTRL_JUMP_200_no:

    VoC_lw16i_set_inc(REG1, VAD1_level_ADDR, 1);
    VoC_lw16i_set_inc(REG0, STRUCT_VADSTATE1_AVE_LEVEL_ADDR,1);
    VoC_lw16i_short(REG5, 0, DEFAULT);// stat_rat in reg5
    VoC_lw16i_short(ACC0_LO,0, IN_PARALLEL);

    VoC_loop_i(1,9);
    VoC_lw16inc_p(REG4, REG1, DEFAULT);//level[i] in REG4
    VoC_lw16inc_p(REG3, REG0, DEFAULT);// num in reg4 denom in reg3
    VoC_bgt16_rr(UPDATE_CNTRL_JUMP_230, REG4, REG3);
    VoC_movreg16(REG3,REG4,DEFAULT);
    VoC_movreg16(REG4,REG3,IN_PARALLEL);
UPDATE_CNTRL_JUMP_230:
    VoC_push32(REG01, DEFAULT);
    VoC_bnlt16_rd(UPDATE_CNTRL_JUMP_240, REG4, CONST_184_ADDR);
    VoC_lw16i(REG4,184);
UPDATE_CNTRL_JUMP_240:
    VoC_bnlt16_rd(UPDATE_CNTRL_JUMP_250, REG3, CONST_184_ADDR);
    VoC_lw16i(REG3,184);
UPDATE_CNTRL_JUMP_250:

    VoC_movreg16(ACC0_HI, REG3,DEFAULT);
    VoC_jal(CII_NORM_L_ACC0);
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_movreg16(REG0, REG4, DEFAULT);
    VoC_sub16_dr(REG4, CONST_8_ADDR, REG1);
    VoC_movreg16(REG1, ACC0_HI, DEFAULT);// denom in reg3
    VoC_jal(CII_DIV_S);
    VoC_pop32(REG01, DEFAULT);//temp in REG2
    VoC_shr16_rr(REG2, REG4, IN_PARALLEL);
    VoC_add16_rr(REG5, REG5, REG2,DEFAULT);
    VoC_endloop1



    VoC_bgt16_rd(UPDATE_CNTRL_JUMP_200, REG5, CONST_1000_ADDR);
    VoC_lw16_d(REG7, STRUCT_VADSTATE1_STAT_COUNT_ADDR);

    VoC_bez16_r(UPDATE_CNTRL_JUMP_200, REG6); //REG6   st->vadreg & 0x4000
    VoC_bez16_r(UPDATE_CNTRL_JUMP_200, REG7);
    VoC_sub16_rd(REG7, REG7, CONST_1_ADDR);

UPDATE_CNTRL_JUMP_200:
    VoC_lw16i_short(ACC0_HI, 2, DEFAULT);    // bckr_add in ACC0_HI
    VoC_lw16_d(REG5, CONST_0x7FFF_ADDR);
    VoC_sw16_d(REG7,STRUCT_VADSTATE1_STAT_COUNT_ADDR);
    VoC_be16_rd(UPDATE_CNTRL_JUMP_280, REG7, CONST_20_ADDR);
    VoC_lw16_d(REG5, CONST_ALPHA5_ADDR);
    VoC_bez16_r(UPDATE_CNTRL_JUMP_280,REG6);    //REG6   st->vadreg & 0x4000
    VoC_lw16_d(REG5, CONST_ALPHA4_ADDR);// alpha in reg5
UPDATE_CNTRL_JUMP_280:

    VoC_lw16i(REG6,0x8000);
    VoC_lw16i_set_inc(REG0, STRUCT_VADSTATE1_AVE_LEVEL_ADDR,1);
    VoC_movreg16(REG2, REG0,DEFAULT);
    VoC_lw16i_set_inc(REG1, VAD1_level_ADDR, 1);
    VoC_loop_i_short(9, DEFAULT)
    VoC_lw16inc_p(REG7, REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_sub16inc_pr(REG4, REG1, REG7, DEFAULT);
    VoC_multf32_rr(ACC1, REG5, REG4, DEFAULT);
    VoC_NOP();
    VoC_add32_rr(ACC1, REG67, ACC1,DEFAULT);
    VoC_lw16inc_p(REG7, REG0,DEFAULT);
    VoC_sw16inc_p(ACC1_HI, REG2, DEFAULT);
    VoC_endloop0

// void CII_update_cntrl (void)  end


    VoC_lw16i_short(ACC0_LO,0,DEFAULT);
    VoC_lw16_d(REG4, STRUCT_VADSTATE1_VADREG_ADDR);
    VoC_lw16_d(REG3, STRUCT_VADSTATE1_PITCH_ADDR);
    VoC_and16_ri(REG4, 0x7800);
    VoC_and16_ri(REG3, 0x7800);
    VoC_bnez16_r(NOISE_ESTIMATE_ZGL, REG4);
    VoC_bnez16_r(NOISE_ESTIMATE_ZGL, REG3);
    VoC_bnez16_d(NOISE_ESTIMATE_ZGL, STRUCT_VADSTATE1_COMPLEX_HANG_COUNT_ADDR);
//  VoC_lw16_d(REG6, CONST_ALPHA_UP1_ADDR);// alpha_up in reg6

    VoC_lw16i(REG6, 0x666);
//  VoC_lw16_d(REG7, CONST_ALPHA_DOWN1_ADDR);// alpha_down in reg7
    VoC_lw16i(REG7, 0x831);

    VoC_jump(NOISE_ESTIMATE_JZGL);

NOISE_ESTIMATE_ZGL:
    VoC_lw16i_short(REG6, 0, DEFAULT);
    VoC_lw16i_short(ACC0_HI, 0, IN_PARALLEL);

    VoC_lw16i(REG7, 0x666);

    VoC_bnez16_d(NOISE_ESTIMATE_JZGL,STRUCT_VADSTATE1_STAT_COUNT_ADDR);
    VoC_bnez16_d(NOISE_ESTIMATE_JZGL,STRUCT_VADSTATE1_COMPLEX_HANG_COUNT_ADDR);
    VoC_lw16_d(REG6, CONST_ALPHA_UP2_ADDR);
//  VoC_lw16_d(REG7, CONST_ALPHA_DOWN2_ADDR);
    VoC_lw16i(REG7, 0x74b);

    VoC_lw16i_short(ACC0_HI, 2, DEFAULT);// bckr_add in reg5

NOISE_ESTIMATE_JZGL:

    VoC_lw16i(REG2,0x8000);
    VoC_lw16i_set_inc(REG1, STRUCT_VADSTATE1_BCKR_EST_ADDR, 1);
    VoC_lw16i_set_inc(REG0, STRUCT_VADSTATE1_OLD_LEVEL_ADDR, 1);

    VoC_loop_i_short(9, DEFAULT)
    VoC_startloop0
    VoC_lw16_p(REG3, REG1,DEFAULT);
    VoC_sub16inc_pr(REG4,REG0,REG3,DEFAULT);    //REG3    temp
    VoC_bnltz16_r(NOISE_ESTIMATE_LX, REG4);//if
    VoC_multf32_rr(REG45, REG7, REG4,DEFAULT);
    VoC_NOP();
    VoC_add32_rr(REG45, REG23, REG45,DEFAULT);
    VoC_sub16_rd(REG5, REG5, CONST_2_ADDR);
    VoC_bnlt16_rd(NOISE_ESTIMATE_JLX, REG5, CONST_40_ADDR);
    VoC_lw16i(REG5,40);
    VoC_jump(NOISE_ESTIMATE_JLX);

NOISE_ESTIMATE_LX: //else
    VoC_multf32_rr(REG45, REG6, REG4, DEFAULT);
    VoC_NOP();
    VoC_add32_rr(REG45, REG23, REG45,DEFAULT);
    VoC_add32_rr(REG45, ACC0, REG45,DEFAULT);
    VoC_lw16i(REG4, 16000);
    VoC_bngt16_rr(NOISE_ESTIMATE_JLX, REG5, REG4);
    VoC_movreg16(REG5, REG4, DEFAULT);
    VoC_NOP();
NOISE_ESTIMATE_JLX:
    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG1, VAD1_level_ADDR, 1);
    VoC_lw16i_set_inc(REG0, STRUCT_VADSTATE1_OLD_LEVEL_ADDR, 1 );
    VoC_loop_i_short(9, DEFAULT)
    VoC_lw16inc_p(REG2, REG1, IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG2, REG1, DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG2, REG0, DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

// void CII_noise_estimate_update(void)  end !!

    //noise_level->REG0
//low_power -> REG5
//  begin of CII_hangover_addition(void)

    VoC_lw32z(REG45,DEFAULT);

    VoC_lw16i_short(REG6, 7, DEFAULT);
    VoC_lw16i_short(REG7, 4, IN_PARALLEL);//hang_len in reg6 burst_len in reg7
    VoC_lw16_d(REG3,STRUCT_VADSTATE1_COMPLEX_HANG_COUNT_ADDR);  // REG3 for complex_hang_count
    VoC_lw16i(REG2, 100);
    VoC_blt16_rd(HANDOVER_F1, REG2, VAD1_decission_noise_level_ADDR);
    VoC_lw16i_short(REG6, 4, DEFAULT);
    VoC_lw16i_short(REG7, 5, IN_PARALLEL);
HANDOVER_F1:
    VoC_bez16_d(HANDOVER_F2,VAD1_low_power_flag_ADDR);
    VoC_sw32_d(REG45, STRUCT_VADSTATE1_BURST_COUNT_ADDR);
    VoC_sw32_d(REG45, STRUCT_VADSTATE1_COMPLEX_HANG_COUNT_ADDR);
    VoC_jump(HANDOVER_END);

HANDOVER_F2:
    VoC_bnlt16_rd(HANDOVER_F3, REG2, STRUCT_VADSTATE1_COMPLEX_HANG_TIMER_ADDR);
    VoC_lw16i(REG2, 250);
    VoC_bngt16_rr(HANDOVER_F3, REG2, REG3);
    VoC_movreg16(REG3,REG2,DEFAULT);
HANDOVER_F3:
    VoC_lw16i_short(REG4, 1, DEFAULT);
    VoC_lw16_d(REG2,STRUCT_VADSTATE1_VADREG_ADDR);

    VoC_bez16_r(HANDOVER_SG, REG3)
    VoC_lw16i_short(REG5, 4, DEFAULT);
    VoC_sub16_rr(REG3,REG3, REG4,DEFAULT);
    VoC_sw16_d(REG5,STRUCT_VADSTATE1_BURST_COUNT_ADDR);
    VoC_jump(HANDOVER_END);

HANDOVER_SG:
    VoC_movreg16(REG5, REG2, DEFAULT);
    VoC_and16_ri(REG5,0x3ff0);
    VoC_bnez16_r(HANDOVER_F4, REG5);
    VoC_lw16i(REG5, 21299);
    VoC_bnlt16_rd(HANDOVER_F4, REG5, STRUCT_VADSTATE1_CORR_HP_FAST_ADDR);
    VoC_jump(HANDOVER_END);
HANDOVER_F4:

    VoC_and16_ri(REG2, 0x4000);
    VoC_bez16_r(HANDOVER_F5, REG2);
    VoC_add16_rd(REG5, REG4, STRUCT_VADSTATE1_BURST_COUNT_ADDR);  //REG4  :1
    VoC_bgt16_rr(HANDOVER_MAMAMI, REG7, REG5);
    VoC_sw16_d(REG6,STRUCT_VADSTATE1_HANG_COUNT_ADDR);
HANDOVER_MAMAMI:
    VoC_sw16_d(REG5,STRUCT_VADSTATE1_BURST_COUNT_ADDR);
    VoC_jump(HANDOVER_END);

HANDOVER_F5:
    VoC_lw16i_short(REG2, 0, DEFAULT);
    VoC_sub16_dr(REG5, STRUCT_VADSTATE1_HANG_COUNT_ADDR,REG4);
    VoC_sw16_d(REG2, STRUCT_VADSTATE1_BURST_COUNT_ADDR);
    VoC_bngtz16_d(HANDOVER_F6, STRUCT_VADSTATE1_HANG_COUNT_ADDR);
    VoC_sw16_d(REG5, STRUCT_VADSTATE1_HANG_COUNT_ADDR);
    VoC_jump(HANDOVER_END);
HANDOVER_F6:
    VoC_lw16i_short(REG4, 0, DEFAULT);
//  end of CII_hangover_addition(void)
//return in REG4 void CII_hangover_addition(void)
HANDOVER_END:
    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG3,STRUCT_VADSTATE1_COMPLEX_HANG_COUNT_ADDR);
    VoC_sw16_d(REG4, STRUCT_VADSTATE1_SPEECH_VAD_DECISION_ADDR);
    VoC_return;
}

// vad_flag in REG7
// return in REG5
void CII_tx_dtx_handler(void)
{
    VoC_lw16i_short(REG2,1, DEFAULT);
    VoC_add16_rd(REG6, REG2, STRUCT_DTX_ENCSTATE_DECANAELAPSEDCOUNT_ADDR);

    VoC_lw16i_short(REG5, 0, DEFAULT);//compute_new_sid_possible in REG5
    VoC_lw16i_short(REG4, 7, IN_PARALLEL);//st->dtxHangoverCount in REG4

    VoC_sw16_d(REG6, STRUCT_DTX_ENCSTATE_DECANAELAPSEDCOUNT_ADDR);

    VoC_bnez16_r(TX_DTX_HANDLER_JUMP, REG7);
    VoC_lw16_d(REG4, STRUCT_DTX_ENCSTATE_DTXHANGOVERCOUNT_ADDR);
    VoC_lw16i_short(REG6, 8, DEFAULT);  //GLOBAL_USED_MODE_ADDR in REG6

    VoC_bez16_r(TX_DTX_HANDLER_GOTO, REG4);// else
    VoC_sub16_rr(REG4, REG4, REG2,DEFAULT);//    STRUCT_DTX_ENCSTATE_DTXHANGOVERCOUNT_ADDR  - 1
    VoC_add16_rd(REG7, REG4, STRUCT_DTX_ENCSTATE_DECANAELAPSEDCOUNT_ADDR);
    VoC_bnlt16_rd(TX_DTX_HANDLER_JUMP, REG7, CONST_30_ADDR);
    VoC_jump(TX_DTX_HANDLER_DECTCD);
TX_DTX_HANDLER_GOTO:// if

    VoC_sw16_d(REG5, STRUCT_DTX_ENCSTATE_DECANAELAPSEDCOUNT_ADDR);    //REG5: 0
    VoC_lw16i_short(REG5, 1, DEFAULT);

TX_DTX_HANDLER_DECTCD:
    VoC_sw16_d(REG6, GLOBAL_ENC_USED_MODE_ADDR);

TX_DTX_HANDLER_JUMP:
    VoC_sw16_d(REG4, STRUCT_DTX_ENCSTATE_DTXHANGOVERCOUNT_ADDR);
    VoC_return;
}
/***********************************************
 Function : CII_dtx_buffer
 Optimized by Kenneth  23/09/2004
***********************************************/
void CII_dtx_buffer(void)
{
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_push16(RA,DEFAULT);
    VoC_add16_rd(REG6,REG6,STRUCT_DTX_ENCSTATE_HIST_PTR_ADDR);
    VoC_lw16i_set_inc(REG2,STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR,2);
    VoC_bne16_rd(NOT_FULL,REG6,CONST_8_ADDR)
    VoC_lw16i_short(REG6,0,DEFAULT);
NOT_FULL:
    VoC_lw16i_short(REG7,10,DEFAULT);

    VoC_mult16_rr(REG7,REG7,REG6,DEFAULT);
    VoC_lw16i_set_inc(REG0,COD_AMR_LSP_NEW_ADDRESS,2);
    VoC_lw16i_set_inc(REG1,STRUCT_DTX_ENCSTATE_LSP_HIST_ADDR,2);

    VoC_sw16_d(REG6,STRUCT_DTX_ENCSTATE_HIST_PTR_ADDR);
    VoC_jal(CII_dtx_com_s2);

    VoC_lw16i(REG6,STRUCT_DTX_ENCSTATE_LOG_EN_HIST_ADDR);
    VoC_add16_rd(REG6,REG6,STRUCT_DTX_ENCSTATE_HIST_PTR_ADDR);
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_sw16_p(REG4,REG6,DEFAULT);
    VoC_return;
}

/*
IN:
REG7:computeSidFlag



**/
void CII_dtx_enc(void)
{

#if 0
    voc_short    DTX_ENC_LSP_ADDR                        ,10,x
    voc_short    DTX_ENC_LSF_ADDR                        ,10,x
    voc_short    DTX_ENC_LSP_Q_ADDR                      ,10,x
#endif




    VoC_push16(RA,DEFAULT);

    VoC_bnez16_r(COMPUTE_PARA,REG7)
    VoC_bez16_d(COMPUTE_PARA,STRUCT_DTX_ENCSTATE_LOG_EN_INDEX_ADDR)
    VoC_jump(GET_PARA);
COMPUTE_PARA:
    /* compute new SID frame if safe i.e don't
    * compute immediately after a talk spurt  */

    /*  lsp */
    VoC_lw32z(REG67,DEFAULT);
    VoC_lw16i_set_inc(REG0,STRUCT_DTX_ENCSTATE_LSP_HIST_ADDR,10);
    VoC_lw16i_set_inc(REG1,DTX_ENC_LSP_ADDR,1);
    VoC_lw16i(REG3,89);

    VoC_lw32z(REG45,DEFAULT);

    VoC_loop_i(1,10)
    VoC_loop_i_short(9,DEFAULT);
    VoC_startloop0
    VoC_add32_rr(REG67,REG67,REG45,DEFAULT);
    VoC_lw16inc_p(REG5,REG0,IN_PARALLEL);
    VoC_shr16_ri(REG5,15,DEFAULT);
    VoC_movreg16(REG4,REG5,IN_PARALLEL);
    VoC_endloop0
    VoC_shr32_ri(REG67,3,DEFAULT);
    VoC_sub16_rr(REG0,REG0,REG3,IN_PARALLEL);
    VoC_lw32z(REG45,DEFAULT);
    VoC_lw32z(REG67,DEFAULT);
    VoC_sw16inc_p(REG6,REG1,IN_PARALLEL);
    VoC_endloop1

    /*average energy*/
    VoC_lw16i_set_inc(REG0,STRUCT_DTX_ENCSTATE_LOG_EN_HIST_ADDR,1);
    VoC_lw16i_short(REG7,0,DEFAULT);    //log_en

    VoC_loop_i_short(8,DEFAULT)
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_shr16_ri(REG6,2,DEFAULT);
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG6,IN_PARALLEL);
    VoC_endloop0

    VoC_shr16_ri(REG7,1,DEFAULT);
    VoC_lw16i_short(REG6,63,IN_PARALLEL);

    VoC_add16_rd(REG7,REG7,CONST_0xa80_ADDR);
    VoC_shr16_ri(REG7,8,DEFAULT);


    VoC_bngt16_rr(NGT63,REG7,REG6)
    VoC_movreg16(REG7,REG6,DEFAULT);
NGT63:
    VoC_bnltz16_r(NLT0,REG7)
    VoC_lw16i_short(REG7,0,DEFAULT);
NLT0:
//  VoC_lw16i(REG6,2560);
    //    VoC_lw16_d(REG6,CONST_a00_ADDR);
    VoC_shr16_ri(REG7,-8,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16_d(REG7,STRUCT_DTX_ENCSTATE_LOG_EN_INDEX_ADDR);
    exit_on_warnings = ON;
    VoC_sub16_rd(REG7,REG7,CONST_0xa00_ADDR);

    /****************************************************/
    VoC_lw16i_set_inc(REG0,STRUCT_GC_PREDST_ADDR,1);
    VoC_jal(CII_dtx_com);
    /****************************************************/
    /* make sure that LSP's are ordered */
    VoC_lw16i_set_inc(REG0,DTX_ENC_LSP_ADDR,-1);
    VoC_lw16i_set_inc(REG1,DTX_ENC_LSF_ADDR,-1);
    VoC_jal(CII_Lsp_lsf);

    VoC_lw16i_set_inc(REG0,DTX_ENC_LSF_ADDR,1);
    VoC_jal(CII_Reorder_lsf);

    VoC_lw16i_set_inc(REG0,DTX_ENC_LSF_ADDR,1);
    VoC_lw16i_set_inc(REG1,DTX_ENC_LSP_ADDR,1);
    VoC_jal(CII_Lsf_lsp);

    VoC_lw16i(REG7,DTX_ENC_LSP_Q_ADDR);
    VoC_lw16i_short(REG6,8,DEFAULT);
    VoC_lw16i(REG4,STRUCT_DTX_ENCSTATE_LSP_INDEX_ADDR);
    VoC_lw16i_set_inc(REG0,DTX_ENC_LSP_ADDR,-1);
    VoC_lw16i(REG5,STRUCT_Q_PLSFSTATE_PAST_RQ_ADDR);
    VoC_jal(CII_Q_plsf_3);  // Q_plsf_3(qSt, MRDTX, lsp, lsp_q, st->lsp_index, &st->init_lsf_vq_index);
    // REG0(incr=-1)    : &lsp1[]
    // REG4       : &indice[]
    // REG5       : &st->past_rq[]
    // REG6       : mode
    // REG7       : &lsp1_q[]

GET_PARA:


    VoC_lw16i_set_inc(REG0,COD_AMR_ANA_ADDR,1);
    VoC_lw16_d(REG3,STRUCT_DTX_ENCSTATE_INIT_LSF_VQ_INDEX_ADDR);
    VoC_lw32_d(REG45,STRUCT_DTX_ENCSTATE_LSP_INDEX_ADDR);
    VoC_lw16_d(REG6,STRUCT_DTX_ENCSTATE_LSP_INDEX_ADDR+2);
    VoC_lw16_d(REG7,STRUCT_DTX_ENCSTATE_LOG_EN_INDEX_ADDR);
    VoC_sw16inc_p(REG3,REG0,DEFAULT);
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    VoC_sw16inc_p(REG7,REG0,DEFAULT);

    // DEBUG correction: the following instruction is wrong

//  VoC_sw16_sd(REG0,0,DEFAULT);  //COD_AMR_ANAP_ADDR
    // Set_zero(st->old_exc,    PIT_MAX + L_INTERPOL);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_OLD_EXC_ADDR,2);

    VoC_lw32z(REG67,DEFAULT);
    VoC_loop_i(0,77)
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_endloop0
//      Set_zero(st->mem_w0,     M);
//      Set_zero(st->mem_err,    M);
//  for ( i = 0; i < M; i++)      state->past_rq[i] = 0;
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_MEM_W0_ADDR,2);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_MEM_ERROR_ADDR,2);
    VoC_lw16i_set_inc(REG2,STRUCT_Q_PLSFSTATE_PAST_RQ_ADDR,2);
    VoC_loop_i_short(5,DEFAULT)
    VoC_startloop0
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG1,DEFAULT);
    VoC_sw32inc_p(REG67,REG2,DEFAULT);
    VoC_endloop0
//      Set_zero(st->zero,       L_SUBFR);
//      Set_zero(st->hvec,       L_SUBFR);    /* set to zero "h1[-L_SUBFR..-1]" */
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_ZERO_ADDR,2);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_HVEC_ADDR,2);
    VoC_loop_i_short(20,DEFAULT)
    VoC_startloop0
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG1,IN_PARALLEL);
    VoC_endloop0
    /* Reset lsp states
    lsp_reset(st->lspSt);*/
//  Copy(lsp_init_data, &st->lsp_old[0], M);
//  Copy(st->lsp_old, st->lsp_old_q, M);

//      Copy(lsp_new, st->lspSt->lsp_old, M);
//      Copy(lsp_new, st->lspSt->lsp_old_q, M);
    VoC_lw16i_set_inc(REG0,COD_AMR_LSP_NEW_ADDRESS,2);
    VoC_lw16i_set_inc(REG1,STRUCT_LSPSTATE_LSP_OLD_ADDR,2);
    VoC_lw16i_set_inc(REG2,STRUCT_LSPSTATE_LSP_OLD_Q_ADDR,2);
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_loop_i_short(5,DEFAULT)
    VoC_startloop0
    VoC_sw32inc_p(REG45,REG1,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(REG45,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    /* Reset clLtp states */
//      cl_ltp_reset(st->clLtpSt);
//      st->sharp = SHARPMIN;       move16 ();
    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG6,STRUCT_COD_AMRSTATE_SHARP_ADDR);
    VoC_return

}
void CII_vad_pitch_detection(void)
{
    VoC_lw16i_short(REG5, 0, DEFAULT);//lagcount
    VoC_lw16i_set_inc(REG0, COD_AMR_T_OP_ADDRESS, 1);

    VoC_loop_i_short(2, DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG6, REG0, DEFAULT);
    VoC_sub16_dr(REG7,STRUCT_VADSTATE1_OLDLAG_ADDR, REG6);
    VoC_bnltz16_r(VAD_PITCH_DTC_01, REG7)
    VoC_sub16_dr(REG7,CONST_0_ADDR,REG7);
VAD_PITCH_DTC_01:
    VoC_bnlt16_rd(VAD_PITCH_DTC_02, REG7, CONST_4_ADDR)
    VoC_add16_rd(REG5, REG5, CONST_1_ADDR);
VAD_PITCH_DTC_02:
    /* Save the current LTP lag */
    VoC_sw16_d(REG6, STRUCT_VADSTATE1_OLDLAG_ADDR);
    VoC_endloop0
    /* Make pitch decision.
      Save flag of the pitch detection to the variable pitch.
      */
    VoC_lw16_d(REG6, STRUCT_VADSTATE1_PITCH_ADDR);  //st->pitch
    VoC_shr16_ri(REG6, 1, DEFAULT);
    VoC_add16_rd(REG7, REG5, STRUCT_VADSTATE1_OLDLAG_COUNT_ADDR);
    VoC_blt16_rd(VAD_PITCH_DTC_03, REG7, CONST_4_ADDR)
    VoC_or16_ri(REG6, 0x4000);
VAD_PITCH_DTC_03:
    /* Update oldlagcount */
    VoC_sw16_d(REG5, STRUCT_VADSTATE1_OLDLAG_COUNT_ADDR);
    VoC_sw16_d(REG6, STRUCT_VADSTATE1_PITCH_ADDR);
    VoC_return;
}

void CII_PRM2_BITS_POST_PROCESS(void)
{
    VoC_pop16(REG3,DEFAULT);
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
//   Copy(&st->old_speech[L_FRAME], &st->old_speech[0], L_TOTAL - L_FRAME);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_OLD_SPEECH_ADDR+160,2);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_OLD_SPEECH_ADDR,2);

    VoC_jal(CII_copy_xy);

//  VoC_lw16_d(REG5, STRUCT_COD_AMRSTATE_OLD_WSP_ADDR+160);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_OLD_WSP_ADDR+160,1);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_OLD_WSP_ADDR,1);
//  VoC_sw16_d(REG5, STRUCT_COD_AMRSTATE_OLD_WSP_ADDR);

    VoC_lw16inc_p(REG6,REG0,DEFAULT);

    exit_on_warnings = OFF;
    VoC_loop_i(0,143)
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_endloop0

    exit_on_warnings = ON;
    //begin of CII_prm2bits
    VoC_lw16_d(REG7,GLOBAL_ENC_USED_MODE_ADDR);
    VoC_lw16i(REG5,STATIC_CONST_PRMNO_ADDR);
    VoC_lw16i_set_inc(REG0,STATIC_CONST_BITNO_AMR_ADDR,1);
    VoC_add16_rr(REG5,REG7,REG5,DEFAULT);
    VoC_add16_rr(REG0,REG7,REG0,DEFAULT);
    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_lw16_p(REG0,REG0,DEFAULT);
    VoC_push16(REG7,DEFAULT);
    VoC_jal(CII_prm2bits_amr_efr);

    //end of CII_prm2bits
    //begin of CII_sid_sync
//  VoC_lw16_d(REG7,GLOBAL_USED_MODE_ADDR);
    VoC_pop16(REG7,DEFAULT);
    VoC_lw32_d(REG45,STRUCT_SID_SYNCSTATE_SID_UPDATE_RATE_ADDR);
    // REG4 for sid_update_rate REG5 for sid_update_counter
    VoC_lw32_d(REG23,STRUCT_SID_SYNCSTATE_SID_HANDOVER_DEBT_ADDR);
    // REG2 for sid_handover_debt REG3 FOR prev_ft
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_bne16_rd(CII_sid_sync100,REG7,CONST_8_ADDR);  //  if ( mode == MRDTX)
    VoC_sub16_rr(REG5,REG5,REG0,DEFAULT);    //   st->sid_update_counter--;
    VoC_bnez16_d(CII_sid_sync101,STRUCT_SID_SYNCSTATE_PREV_FT_ADDR) //  if (st->prev_ft == TX_SPEECH_GOOD)
    VoC_lw16i_short(REG6,1,DEFAULT);    // *tx_frame_type = TX_SID_FIRST;
    VoC_lw16i_short(REG5,3,IN_PARALLEL);    // st->sid_update_counter = 3;
    VoC_jump(CII_sid_sync_end);
CII_sid_sync101:
    /* TX_SID_UPDATE or TX_NO_DATA */
    VoC_bngt16_rd(CII_sid_sync102,REG5,CONST_2_ADDR)    // st->sid_update_counter > 2
    VoC_bngtz16_r(CII_sid_sync102,REG2) // (st->sid_handover_debt > 0)
    /* ensure extra updates are  properly delayed after
       a possible SID_FIRST */
    VoC_lw16i_short(REG6,2,DEFAULT);    //  *tx_frame_type = TX_SID_UPDATE;
    VoC_sub16_rr(REG2,REG2,REG0,IN_PARALLEL);   //  st->sid_handover_debt--;
    VoC_jump(CII_sid_sync_end);
CII_sid_sync102:
    VoC_lw16i_short(REG6,3,DEFAULT);   // *tx_frame_type = TX_NO_DATA;
    VoC_bnez16_r(CII_sid_sync_end,REG5)     // if (st->sid_update_counter == 0)
    VoC_lw16i_short(REG6,2,DEFAULT);    //  *tx_frame_type = TX_SID_UPDATE;
    VoC_movreg16(REG5,REG4,IN_PARALLEL);    //st->sid_update_counter = st->sid_update_rate;
    VoC_jump(CII_sid_sync_end);
CII_sid_sync100:
    VoC_lw16i_short(REG6,0,DEFAULT);    //  *tx_frame_type = TX_SPEECH_GOOD;
    VoC_movreg16(REG5,REG4,DEFAULT);    //st->sid_update_counter = st->sid_update_rate;
CII_sid_sync_end:
    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG6,STRUCT_SID_SYNCSTATE_PREV_FT_ADDR);     // st->prev_ft = *tx_frame_type;
    VoC_sw16_d(REG5,STRUCT_SID_SYNCSTATE_SID_UPDATE_COUNTER_ADDR);
    // REG6 for frame_type
    VoC_return;
}
/************************************************
  Function CII_Vq_subvec3
  Input: RL7       -->  use_half
         &lsf_r1[] -->  push32(n+2) LO
         &wf1[]    -->  push32(n+2) HI
         *dico     -->  REG3(incr=1)
         dico_size/32 -> REG6
         &indice[] -> REG2(incr=1)  push16(n)
  requirment:  REG0.incr=1,REG1.incr=1;
  Output:&lsf_r1[+3] -->  push32(n+2) LO
         &wf1[+3]    -->  push32(n+2) HI
  Optimized by Kenneth 09/10/2004
************************************************/
void CII_Vq_subvec3(void)
{
    VoC_push16(REG3,DEFAULT);
    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_bez32_r(Vq_subvec3_L1,RL7);
    VoC_lw16i_short(REG5,4,DEFAULT);
Vq_subvec3_L1:
    VoC_lw16i_short(REG2,-1,DEFAULT);
    VoC_lw32_d(RL6,CONST_0x7FFFFFFF_ADDR);
    VoC_sub16inc_pp(REG7,REG0,REG3,DEFAULT);
    VoC_loop_r(1,REG6);
    VoC_loop_i_short(32,DEFAULT);
    VoC_startloop0
    VoC_multf16inc_rp(REG7,REG7,REG1,DEFAULT);      //temp = sub(lsf_r1[0], *p_dico++);
    VoC_sub16inc_pp(REG6,REG0,REG3,DEFAULT);    //temp = mult(wf1[0], temp);
    VoC_multf32_rr(ACC0,REG7,REG7,DEFAULT);         //dist = L_mult(temp, temp);
    VoC_multf16inc_rp(REG6,REG6,REG1,IN_PARALLEL);  //temp = sub(lsf_r1[1], *p_dico++);
    VoC_sub16_pp(REG7,REG0,REG3,DEFAULT);           //temp = mult(wf1[1], temp);
    //dist = L_mac(dist, temp, temp);
    VoC_multf16_rp(REG7,REG7,REG1,DEFAULT);     //temp = sub(lsf_r1[2], *p_dico++);
    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);
    VoC_bimac32_rr(REG67,REG67);                    //temp = mult(wf1[2], temp);
    VoC_inc_p(REG2,DEFAULT);                            //dist = L_mac(dist, temp, temp);
    VoC_lw32_sd(REG01,2,IN_PARALLEL);
    VoC_bngt32_rr(CII_Vq_subvec_new_02,RL6,ACC0);   //if (L_sub(dist, dist_min) < (Word32) 0) {
    VoC_movreg32(RL6,ACC0,DEFAULT);         //   dist_min = dist;
    VoC_movreg16(REG4,REG2,IN_PARALLEL);    //   index = i;
CII_Vq_subvec_new_02:                                           //}
    VoC_sub16inc_pp(REG7,REG0,REG3,DEFAULT);
    VoC_endloop0
    VoC_lw16i_short(REG6,2,DEFAULT);
    VoC_endloop1

    VoC_pop16(REG3,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG5,DEFAULT);
    VoC_lw16_sd(REG2,0,IN_PARALLEL);
    VoC_mult16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_lw32_sd(REG01,2,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);
    VoC_sw16inc_p(REG4,REG2,IN_PARALLEL);
    VoC_add16_rd(REG1,REG1,CONST_3_ADDR);



    VoC_loop_i_short(3,DEFAULT);
    VoC_lw16inc_p(REG5,REG3,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG3,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_lw32z(RL7,DEFAULT);
    VoC_sw16_sd(REG2,0,DEFAULT);
    VoC_sw32_sd(REG01,2,IN_PARALLEL);
    VoC_return;
}

/*********************************************
 Function:  CII_Q_plsf_3
 Input:
       REG0(incr=-1)    : &lsp1[]
       REG4     : &indice[]
       REG5     : &st->past_rq[]
       REG6         : mode
       REG7     : &lsp1_q[]
 Optimized by Kenneth 09/16/2004
**********************************************/
void CII_Q_plsf_3(void)
{
#if 0
    voc_short    Q_PLSF_3_LSF1_ADDR                      ,10,x          //[10]shorts
    voc_short    Q_PLSF_3_LSF_R1_ADDR                    ,10,x           //[10]shorts
    voc_short    Q_PLSF_3_LSF1_Q_ADDR                    ,20,x           //[20]shorts
    voc_short    Q_PLSF_3_PRED_INIT_I_ADDR               ,x          //1 short
    voc_short    Q_PLSF_3_LOOP_I_ADDR                    ,9,x            //[9]shorts
    voc_short    Q_PLSF_3_TEMP_P_ADDR                    ,80,x           //[80]shorts
    voc_short    Q_PLSF_3_MEAN_LSF_COPY_ADDR             ,10,x           //[10]shorts

#endif


#if 0
    voc_short   Q_PLSF_3_LSF_P_ADDR                     ,10,y           //[10]shorts
    voc_short   Q_PLSF_3_WF1_ADDR                       ,10,y            //[10]shorts
    voc_short   Q_PLSF_3_TEMP_R1_ADDR                   ,80,y            //[80]shorts
#endif





    VoC_push16(RA,DEFAULT);

    VoC_lw16i_set_inc(REG1,Q_PLSF_3_LSF1_ADDR,-1);
    VoC_lw16i_set_inc(REG2,Q_PLSF_3_WF1_ADDR,1);
    VoC_push16(REG1,DEFAULT);       //  push16(n-1) &lsf1[]
    VoC_push32(REG67,IN_PARALLEL);      // push32(n)   &lsp1_q[]/ mode
    VoC_push16(REG2,DEFAULT);       //  push16(n-2) &wf1[]
    VoC_push32(REG45,IN_PARALLEL);      //  push32(n-1)  &st->past_rq[]/&indice[]

    VoC_jal(CII_Lsp_lsf);
    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_jal(CII_Lsf_wt);


    VoC_lw16i(REG5,Q_PLSF_3_LSF_R1_ADDR);
    VoC_lw16i_set_inc(REG2,STATIC_MEAN_LSF_3_ADDR,1);
    VoC_push16(REG5,DEFAULT);       // push16(n-3) &lsf_r1[]
    VoC_lw32_sd(REG01,1,IN_PARALLEL);

    VoC_be16_rd(Q_PLSF_3_DTX_MODE,REG0,CONST_8_ADDR);           //if (test(), sub(mode, MRDTX) != 0)
    VoC_lw16_sd(REG4,2,DEFAULT);        // load &lsf1[]
    VoC_lw32_sd(REG01,0,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0,STATIC_pred_fac_ADDR,1);
    VoC_lw16i_set_inc(REG3,Q_PLSF_3_LSF_P_ADDR,1);
    VoC_lw32_d(RL6,CONST_0x00010001_ADDR);
    VoC_loop_i_short(M,DEFAULT)             // for (i = 0; i < M; i++)
    VoC_startloop0                                      // {
    VoC_multf16inc_pp(REG6,REG0,REG1,DEFAULT);      //    lsf_p[i] = add(mean_lsf_3[i],
    VoC_NOP();
    VoC_add16inc_rp(REG6,REG6,REG2,DEFAULT);        //                   mult(st->past_rq[i],
    VoC_sub16_pr(REG7,REG4,REG6,DEFAULT);       //                        pred_fac[i]));
    VoC_sw16inc_p(REG6,REG3,DEFAULT);               //    lsf_r1[i] = sub(lsf1[i], lsf_p[i]);
    VoC_sw16_p(REG7,REG5,DEFAULT);                  //}
    VoC_add32_rr(REG45,REG45,RL6,IN_PARALLEL);
    VoC_endloop0
    VoC_jump(Q_PLSF_3_PRED_END)
Q_PLSF_3_DTX_MODE:

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_set_inc(REG1,Q_PLSF_3_MEAN_LSF_COPY_ADDR,2);
    VoC_movreg16(REG0,REG2,DEFAULT);

    VoC_push16(REG1,DEFAULT);               // push16(n-4)   Q_PLSF_3_MEAN_LSF_COPY_ADDR
    VoC_movreg16(REG7,REG1,IN_PARALLEL);
    VoC_jal(CII_Copy_M);

    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,STATIC_past_rq_init_ADDR,2);//for j
    VoC_lw16_sd(REG2,3,DEFAULT);
    VoC_lw16i_set_inc(REG3,Q_PLSF_3_TEMP_R1_ADDR,2);
    VoC_lw16i(REG4,Q_PLSF_3_TEMP_P_ADDR-2);
    VoC_push16(REG7,DEFAULT);       // push16(n-5)   pred_init_i
    VoC_lw16i_short(REG5,2,IN_PARALLEL);
    VoC_lw32_d(RL7,CONST_0x7FFFFFFF_ADDR);  //L_min_pred_init_err

    //   REG0:mean_lsf_copy[]       reg1:past_rq_init[]
    //   reg2:lsf1[]        reg3:temp_r1[]
    //   reg4:temp_p[]      reg5:2
    //   reg6:xx            reg7:& mean_lsf_copy[]
    VoC_loop_i(1,8)                         //for (j = 0; j < PAST_RQ_INIT_SIZE; j++)
    VoC_add16inc_pp(REG6,REG0,REG1,DEFAULT);                //{
    VoC_add16inc_pp(REG7,REG0,REG1,IN_PARALLEL);        //   L_pred_init_err = 0;
    VoC_loop_i_short(5,DEFAULT);                            //   for (i = 0; i < M; i++)
    VoC_lw32z(ACC0,IN_PARALLEL);                        //   {
    VoC_startloop0                                          //      temp_p[i] = add(mean_lsf_3[i], past_rq_init[j*M+i]);
    VoC_movreg32(RL6,REG67,DEFAULT);            //      temp_r1[i] = sub(lsf1[i],temp_p[i]);
    VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);       //      L_pred_init_err = L_mac(L_pred_init_err, temp_r1[i], temp_r1[i]);
    VoC_sub16inc_pr(REG6,REG2,REG6,DEFAULT);        //   }
    VoC_sub16inc_pr(REG7,REG2,REG7,IN_PARALLEL);
    VoC_bimac32_rr(REG67,REG67);
    VoC_sw32inc_p(REG67,REG3,DEFAULT);
    VoC_sw32_p(RL6,REG4,IN_PARALLEL);
    VoC_add16inc_pp(REG6,REG0,REG1,DEFAULT);
    VoC_add16inc_pp(REG7,REG0,REG1,IN_PARALLEL);
    VoC_endloop0

    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG5,IN_PARALLEL);
    VoC_bngt32_rr(Q_PLSF_3_INIT_ERR_ELSE,RL7,ACC0)          //   if (L_sub(L_pred_init_err, L_min_pred_init_err) < (Word32) 0)
    VoC_movreg32(RL7,ACC0,DEFAULT);                         //  {       L_min_pred_init_err = L_pred_init_err;
    VoC_sw16_sd(REG1,0,DEFAULT);                            //     Copy(temp_r1, lsf_r1, M);
Q_PLSF_3_INIT_ERR_ELSE:                                                 //     Copy(temp_p, lsf_p, M);
    VoC_lw16_sd(REG2,4,DEFAULT);                            //     Copy(&past_rq_init[j*M], st->past_rq, M);
    //     *pred_init_i = j;
    VoC_endloop1                                    //  }


    VoC_pop16(REG7,DEFAULT);        // pop16(n-5)   pred_init_i
    VoC_lw16i_short(REG6,10,IN_PARALLEL);
    VoC_pop16(REG4,DEFAULT);        // pop16(n-4)   Q_PLSF_3_MEAN_LSF_COPY_ADDR
    VoC_sub16_rr(REG7,REG7,REG6,IN_PARALLEL);
    VoC_lw16i(REG4,STATIC_past_rq_init_ADDR);
    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);

    VoC_lw16i_set_inc(REG0,Q_PLSF_3_TEMP_P_ADDR,2);
    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_lw16i_short(REG2,0,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,Q_PLSF_3_LSF_P_ADDR,2);
    VoC_jal(CII_Copy_M);

    VoC_lw16i(REG0,Q_PLSF_3_TEMP_R1_ADDR);
    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_lw16_sd(REG1,0,IN_PARALLEL);
    VoC_jal(CII_Copy_M);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0,STATIC_past_rq_init_ADDR,1);
    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_loop_i_short(10,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_blt16_rd(q_plsf_3_L,REG7,CONST_10_ADDR);
    VoC_sub16_rr(REG7,REG7,REG6,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
q_plsf_3_L:
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_endloop0

    VoC_sw16_d(REG2,STRUCT_DTX_ENCSTATE_INIT_LSF_VQ_INDEX_ADDR);

Q_PLSF_3_PRED_END:
    VoC_lw32_sd(REG23,0,DEFAULT);            // reg2->&indice[]
    VoC_pop16(REG0,DEFAULT);             // pop16(n-3) &lsf_r1[]
    VoC_pop16(REG1,DEFAULT);             // pop16(n-2) &WF1[]
    VoC_lw32_sd(REG45,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG3,TABLE_DICO1_LSF_3_ADDR,1);//       temp_3[0]=DICO1_SIZE_3;
    VoC_push32(REG01,DEFAULT);           // push32(n-2) &WF1/&lsf_r1[]
    VoC_push16(REG2,IN_PARALLEL);                    // push16(n-2) &indice[]
    //********************
    VoC_lw16i_short(RL6_LO,16,DEFAULT);
    VoC_lw16i_short(ACC1_LO,16,IN_PARALLEL);             //     temp_ptr_0=dico1_lsf_3;
    VoC_lw16i(ACC1_HI,TABLE_DICO2_LSF_3_ADDR);           //     temp_3[1]=DICO2_SIZE_3; temp_ptr_1=dico2_lsf_3;
    VoC_lw16i(RL6_HI,TABLE_DICO3_LSF_3_ADDR);            //     temp_3[2]=DICO3_SIZE_3; temp_ptr_2=dico3_lsf_3;
    VoC_lw16i_short(REG6,8,DEFAULT);                     //     i=0;
    VoC_lw32z(RL7,IN_PARALLEL);                          //     if (sub (mode, MR475) == 0 || sub (mode, MR515) == 0)
    VoC_bgt16_rd(Vq_subvec3_L1a,REG4,CONST_1_ADDR);      //      {   // MR475, MR515 /
    VoC_lw16i(RL6_HI,STATIC_mr515_3_lsf_ADDR);           //        temp_ptr_2=mr515_3_lsf;
    VoC_lw16i_short(RL6_LO,4,DEFAULT);                   //       temp_3[2]=MR515_3_SIZE;
    VoC_jump(Vq_subvec3_L2);                             //      }
Vq_subvec3_L1a:                                               //      else if (sub (mode, MR795) == 0)
    VoC_bne16_rd(Vq_subvec3_L2,REG4,CONST_5_ADDR);       //      {   // MR795
    VoC_lw16i_set_inc(REG3,STATIC_mr795_1_lsf_ADDR,1);   //        temp_ptr_0=mr795_1_lsf;
    VoC_lw16i_short(REG6,16,DEFAULT);                    //       temp_3[0]=MR795_1_SIZE;
Vq_subvec3_L2:                                               //      }
    VoC_push32(ACC1,DEFAULT);                            //      else
    VoC_push32(RL6,DEFAULT);                         //      {   // MR59, MR67, MR74, MR102 , MRDTX
    VoC_jal(CII_Vq_subvec3);                             //        ;
    VoC_lw32_sd(REG45,4,DEFAULT);                     //      }
    VoC_lw32_sd(REG23,1,DEFAULT);
    VoC_bgt16_rd(Vq_subvec3_L3,REG4,CONST_1_ADDR);
    VoC_lw16i_short(RL7_LO,1,DEFAULT);                   //       indice[0] = Vq_subvec3(&lsf_r1[0], temp_ptr_0, &wf1[0], temp_3[0], i);
    VoC_shr16_ri(REG2,1,IN_PARALLEL);                    //     if (sub (mode, MR475) == 0 || sub (mode, MR515) == 0)
Vq_subvec3_L3:                                               //      {   // MR475, MR515
    VoC_movreg16(REG6,REG2,DEFAULT);                 //        i=1;
    VoC_jal(CII_Vq_subvec3);                             //       temp_3[1]=temp_3[1]/2;
    VoC_pop32(REG23,DEFAULT);                            //      }
    //       indice[1] = Vq_subvec3(&lsf_r1[3], temp_ptr_1, &wf1[3], temp_3[1], i);
    VoC_pop32(RL6,DEFAULT);

    VoC_add32_rd(REG01,REG01,CONST_0x00020002_ADDR);
    VoC_movreg16(REG6,REG2,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_push32(REG01,DEFAULT);
    VoC_jal(CII_Vq_subvec);             //       indice[2] = Vq_subvec4(&lsf_r1[6], temp_ptr_2, &wf1[6], temp_3[2]);
    VoC_pop32(REG01,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_lw16i(REG0,Q_PLSF_3_LSF_R1_ADDR);           // load  reg0->&lsf_r1[]
    VoC_pop16(REG7,DEFAULT);                // pop16(n-2) &indice[]
    VoC_lw32_sd(REG23,1,IN_PARALLEL);           // load  reg3->&st->past_rq[]
    VoC_lw16i_set_inc(REG1,Q_PLSF_3_LSF1_Q_ADDR,2);
    VoC_lw16i_set_inc(REG2,Q_PLSF_3_LSF_P_ADDR,2);
    VoC_push16(REG1,DEFAULT);           // push16(n-2);  &lsf1_q[]
    VoC_loop_i_short(5,DEFAULT);            //   for (i = 0; i < M; i++)
    VoC_startloop0                  //      {
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_add16inc_rp(REG6,REG4,REG2,DEFAULT);//          lsf1_q[i] = add(lsf_r1[i], lsf_p[i]);
    VoC_add16inc_rp(REG7,REG5,REG2,IN_PARALLEL);
    VoC_sw32inc_p(REG45,REG3,DEFAULT);  //          st->past_rq[i] = lsf_r1[i];
    VoC_sw32inc_p(REG67,REG1,DEFAULT);
    VoC_endloop0    //      }

    VoC_lw16_sd(REG0,0,DEFAULT);    //  load &lsf1_q[]
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_jal(CII_Reorder_lsf);           //  Reorder_lsf(lsf1_q, LSF_GAP, M);


    VoC_loop_i_short(3,DEFAULT);
    VoC_startloop0
    VoC_pop32(REG01,DEFAULT);       //  pop32(n)   &lsp1_q[]/mode
    VoC_endloop0
    VoC_pop16(REG0,DEFAULT);        //  pop16(n-2);  &lsf1_q[]
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_jal(CII_Lsf_lsp);       //  Lsf_lsp(lsf1_q, lsp1_q, M);

    VoC_pop16(REG0,DEFAULT);        //  pop16(n-1);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
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
    VoC_mac32_rd(REG5,CONST_1_ADDR);


    VoC_lw16i(REG0,16678);
    VoC_bne16_rd(label_no_MR795, REG7, CONST_5_ADDR)    // ACC1_HI : frac_en
    VoC_sub16_dr(REG1, CONST_NEG11_ADDR, REG1);     //   REG1:         exp_en
    VoC_lw16i(REG0, 17062);
    VoC_jump(label_tes_end)
label_no_MR795:
    VoC_bne16_rd(label_no_MR74, REG7, CONST_4_ADDR)
    VoC_lw16i(REG2, 32);
    VoC_lw16i(REG0, 32588);
    VoC_jump(label_tes_end)  //ACC0   L_tmp
label_no_MR74:
    VoC_bne16_rd(label_tes_end, REG7, CONST_3_ADDR)
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

    VoC_bne16_rd(label_no_MR74_2, REG5, CONST_4_ADDR)
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

void CII_check_lsp(void)
{


    VoC_lw16i_short(REG5,2,DEFAULT);
    VoC_lw16i_set_inc(REG0,STRUCT_LSPSTATE_LSP_OLD_ADDR+1,1);
    VoC_loop_i(1, 2)
    VoC_lw16i(REG4,MAX_16);                                 //dist_min2
    VoC_loop_r_short(REG5,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    VoC_sub16_rp(REG5,REG5,REG0,DEFAULT);
    VoC_bngt16_rr(CHECK_LSP_02,REG4,REG5)
    VoC_movreg16(REG4,REG5,DEFAULT);
CHECK_LSP_02:
    VoC_lw16i_short(REG5,5,DEFAULT);
    VoC_endloop0
    VoC_movreg16(REG7, REG6, DEFAULT);
    VoC_movreg16(REG6, REG4, IN_PARALLEL);
//  VoC_lw16i(REG4,MAX_16);
    VoC_endloop1

//REG7   dist_min2    REG6    dist_min1

    VoC_lw16i(REG4,32000);
    VoC_lw16i(REG5,600);            //dist_th

    VoC_blt16_rd(CHECK_LSP_03,REG4,STRUCT_LSPSTATE_LSP_OLD_ADDR+1)
    VoC_lw16i(REG4,30500);
    VoC_lw16i(REG5,800);
    VoC_blt16_rd(CHECK_LSP_03,REG4,STRUCT_LSPSTATE_LSP_OLD_ADDR+1)
    VoC_lw16i(REG5,1100);
CHECK_LSP_03:
//REG5    dist_th

    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_lw16i_short(REG1,0,DEFAULT);//st->count
    VoC_lw16i(REG4,1500);
    VoC_bgt16_rr(CHECK_LSP_04,REG4,REG6)
    VoC_bngt16_rr(CHECK_LSP_05,REG5,REG7)
CHECK_LSP_04:
    VoC_add16_rd(REG1,REG0,STRUCT_TONSTABSTATE_COUNT_ADDR);
CHECK_LSP_05:

    /* Need 12 consecutive frames to set the flag */
    VoC_lw16i_short(REG7,12,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);    //return value

    VoC_bgt16_rr(CHECK_LSP_RET,REG7,REG1)
    VoC_movreg16(REG1,REG7,DEFAULT);
    VoC_lw16i_short(REG6,1,DEFAULT);

CHECK_LSP_RET:
    VoC_sw16_d(REG1,STRUCT_TONSTABSTATE_COUNT_ADDR);
    VoC_sw16_d(REG6,COD_AMR_LSP_FLAG_ADDRESS);

    VoC_return

}


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
    VoC_add16_rd(REG0,REG0,CONST_40_ADDR);
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
    VoC_bngt16_rd(Bgnscd_LA5,REG5,CONST_20_ADDR);            //if ( (sub(maxEnergy, LOWERNOISELIMIT) > 0) &&
    VoC_bngt16_rd(Bgnscd_LA5,REG7,CONST_20_ADDR);            //     (sub(currEnergy, FRAMEENERGYLIMIT) < 0) &&
    VoC_bgt16_rd(Bgnscd_LA5,REG7,CONST_17578_ADDR);              //     (sub(currEnergy, LOWERNOISELIMIT) > 0) &&                    <1953>
    VoC_bgt16_rr(Bgnscd_LA4,REG6,REG7);              //     ( (sub(currEnergy, noiseFloor) < 0) ||(sub(maxEnergyLastPart, UPPERNOISELIMIT) < 0)))
    VoC_bngt16_rr(Bgnscd_LA5,REG3,REG4);
Bgnscd_LA4:                     //{
    VoC_add16_rp(REG2,REG0,REG1,DEFAULT);
    VoC_bngt16_rd(Bgnscd_LA5,REG2,CONST_30_ADDR);            //   if (sub(add(st->bgHangover, 1), 30) > 0)
    VoC_lw16i(REG2,30);              //   {  st->bgHangover = 30;
    //   }
    //else
    //   {st->bgHangover = add(st->bgHangover, 1);  }
    //}
    //else
    //{ st->bgHangover = 0;      }
Bgnscd_LA5:
    VoC_lw16i(REG6, 59);

    VoC_bgt16_rd(Bgnscd_LAEnd,REG2,CONST_1_ADDR);        //if (sub(st->bgHangover,1) > 0)
    VoC_lw16i_short(REG0,0,DEFAULT);             //   inbgNoise = 1;
Bgnscd_LAEnd:
    VoC_lw16_sd(REG3,0,DEFAULT);
    VoC_sw16_p(REG2,REG1,DEFAULT);
    VoC_sw16_d(REG0,STRUCT_DECOD_AMRSTATE_INBACKGROUNDNOISE_ADDR);

    VoC_add16_rd(REG0,REG3,CONST_1_ADDR);            //for (i = 0; i < L_ENERGYHIST-1; i++)
    VoC_jal(CII_R02R3_p_R6);             //{ st->frameEnergyHist[i] = st->frameEnergyHist[i+1];
    //}
    VoC_lw16i(REG6,13926);                   //ltpLimit = 13926;
    VoC_sw16_p(REG7,REG3,DEFAULT);               //st->frameEnergyHist[L_ENERGYHIST-1] = currEnergy;

    VoC_bngt16_rd(Bgnscd_LA8,REG2,CONST_8_ADDR);         //if (sub(st->bgHangover, 8) > 0)
    VoC_lw16i(REG6,15565);                   //{   ltpLimit = 15565;
Bgnscd_LA8:
    VoC_bngt16_rd(Bgnscd_LA9,REG2,CONST_15_ADDR);            //if (sub(st->bgHangover, 15) > 0)
    VoC_lw16i(REG6,16383);                   //{   ltpLimit = 16383; }
Bgnscd_LA9:                             //  REG6->ltpLimit

    VoC_bgt16_rd(Bgnscd_Lab11,REG2,CONST_20_ADDR);//if (sub(st->bgHangover, 20) <= 0)
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
    VoC_bngt16_rd(Bgnscd_LA15,REG7,CONST_10_ADDR);   //if (sub(temp, 10) > 0)
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
    VoC_bne16_rd(DtxDec_L1,REG7,CONST_80_ADDR);     //if (sub(st->lsf_hist_ptr, 80) == 0)
    VoC_lw16i_short(REG7,0,DEFAULT);            //{   st->lsf_hist_ptr = 0;
DtxDec_L1:                          //}
    VoC_lw16i_set_inc(REG0,STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR,2);
    VoC_lw16i_set_inc(REG1,STRUCT_DTX_DECSTATE_LSF_HIST_ADDR,2);
    VoC_sw16_d(REG7,STRUCT_DTX_DECSTATE_LSF_HIST_PTR_ADDR);
    VoC_jal(CII_dtx_com_s2);
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_add16_rd(REG3,REG3,STRUCT_DTX_DECSTATE_LOG_EN_HIST_PTR_ADDR); //st->log_en_hist_ptr = add(st->log_en_hist_ptr, 1);
    VoC_bne16_rd(DtxDec_L2,REG3,CONST_8_ADDR);      //if (sub(st->log_en_hist_ptr, DTX_HIST_SIZE) == 0)
    VoC_lw16i_short(REG3,0,DEFAULT);            //{   st->log_en_hist_ptr = 0;
DtxDec_L2:                              //}
    VoC_lw16i_set_inc(REG1,STRUCT_DTX_DECSTATE_LOG_EN_HIST_ADDR,1);
    VoC_add16_rr(REG1,REG3,REG1,DEFAULT);

    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG3,STRUCT_DTX_DECSTATE_LOG_EN_HIST_PTR_ADDR);

    VoC_sw16_p(REG4,REG1,DEFAULT);          //st->log_en_hist[st->log_en_hist_ptr] = log_en;
    VoC_return;
}

/***********************************************
 Function : CII_dtx_com
 Input: reg0(incr=1)    :&predState->past_qua_en[]
        reg7        :log_en(ma_pred_init)
 Note: dtx_dec and dtx_enc share the same code
 Optimized by Kenneth  22/09/2004
***********************************************/
void CII_dtx_com(void)
{
    VoC_sub16_rd(REG7,REG7,CONST_9000_ADDR);        //  ma_pred_init = sub(ma_pred_init, 9000);
    VoC_lw16i_set_inc(REG1,-14436,1);               //  if (ma_pred_init > 0)
    VoC_bngtz16_r(dtx_com_L1,REG7)                  //  {  ma_pred_init = 0; }
    VoC_lw16i_short(REG7,0,DEFAULT);
dtx_com_L1:
    VoC_bngt16_rr(dtx_com_L2,REG1,REG7)             //  if (sub(ma_pred_init, -14436) < 0)
    VoC_movreg16(REG7,REG1,DEFAULT);                //  {  ma_pred_init = -14436; }
dtx_com_L2:
    VoC_lw16i_short(REG1,4,DEFAULT);
    VoC_multf16_rd(REG6,REG7,CONST_5443_ADDR);      //  predState->past_qua_en[1] = ma_pred_init;
    VoC_add16_rr(REG1,REG1,REG0,DEFAULT);       //  predState->past_qua_en[2] = ma_pred_init;
    //  predState->past_qua_en[3] = ma_pred_init;
    VoC_loop_i_short(4,DEFAULT);                    //  ma_pred_init = mult(5443, ma_pred_init);
    VoC_startloop0                                  //  predState->past_qua_en_MR122[0] = ma_pred_init;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);       //  predState->past_qua_en_MR122[1] = ma_pred_init;
    VoC_sw16inc_p(REG6,REG1,DEFAULT);       //  predState->past_qua_en_MR122[2] = ma_pred_init;
    VoC_endloop0                                    //  predState->past_qua_en_MR122[3] = ma_pred_init;
    VoC_return;
}
/********************************************************************************
  Function:  void CII_D_plsf_3(void)
   input: mode    ->REG7;
          bfi     ->REG6
          * indice->REG1 (incr 1)(push REG1)    stack[n+3]
      *st     ->REG0 (incr 1)(push REG0)    stack[n+2]
   output:
          *st
          * lsp1_q->REG3
  Created by Kenneth  07/07/2004
   optimized by Cui 10/11/2004
 ********************************************************************************/

void CII_D_plsf_3(void)
{
    VoC_push16(RA,DEFAULT);                 //stack[n+1]
    VoC_push16(REG3,DEFAULT);               //stack[n]
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG2,STATIC_MEAN_LSF_3_ADDR,1);
    VoC_lw16i_set_inc(REG1,DEC_AMR_BUFA40_ADDRESS,1);
    VoC_push16(REG2,DEFAULT);                   //stack[n-1]->REG2->STATIC_MEAN_LSF_3_ADDR
    VoC_push16(REG1,DEFAULT);                   //stack[n-2]->REG1->DEC_AMR_TEMP0_ADDRESS->lsf1_q[]

    VoC_bez16_r(Dplsf3_lab1,REG6);      //if (bfi != 0)   {
    VoC_lw16_sd(REG0,4,DEFAULT);
    VoC_lw16i(REG6,29491);
    VoC_lw16i(REG5,3277);
    VoC_multf16inc_rp(REG4,REG5,REG2,DEFAULT);          //lsf1_q[i] = add(mult(st->past_lsf_q[i], ALPHA), mult(mean_lsf[i], ONE_ALPHA));
    VoC_multf16inc_rp(REG3,REG6,REG0,DEFAULT);

    VoC_loop_i_short(10,DEFAULT);                   //for (i = 0; i < M; i++)
    VoC_startloop0                          //{

    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_multf16inc_rp(REG4,REG5,REG2,DEFAULT);          //lsf1_q[i] = add(mult(st->past_lsf_q[i], ALPHA), mult(mean_lsf[i], ONE_ALPHA));
    VoC_multf16inc_rp(REG3,REG6,REG0,DEFAULT);
    exit_on_warnings =OFF;
    VoC_sw16inc_p(REG3,REG1,DEFAULT);
    exit_on_warnings =ON;
    VoC_endloop0                            //}

    VoC_lw16_sd(REG0,4,DEFAULT);
    VoC_lw16i_short(REG3,10,IN_PARALLEL);
    VoC_lw16_sd(REG2,1,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG3,IN_PARALLEL);

    VoC_lw16i_set_inc(REG3,STATIC_pred_fac_ADDR,1);

    VoC_loop_i_short(10,DEFAULT);               // for (i = 0; i < M; i++) {
    VoC_lw16_sd(REG1,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16_p(REG6,REG0,DEFAULT);
    VoC_be16_rd(Dplsf3_lab2,REG7,CONST_8_ADDR);
    VoC_multf16inc_rp(REG6,REG6,REG3,DEFAULT);
    VoC_NOP();
Dplsf3_lab2:
    VoC_add16inc_rp(REG6,REG6,REG2,DEFAULT);
    VoC_sub16inc_pr(REG6,REG1,REG6,DEFAULT);         //st->past_r_q[i] = sub(lsf1_q[i], temp);
    VoC_NOP();
    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    VoC_endloop0
    VoC_jump(Dplsf3_End);                   //    }

Dplsf3_lab1:                            //else {
    VoC_lw16i(REG4,TABLE_DICO1_LSF_3_ADDR);
    VoC_lw16i(REG5,TABLE_DICO2_LSF_3_ADDR);
    VoC_lw16i(REG6,TABLE_DICO3_LSF_3_ADDR);
    VoC_lw16i_set_inc(REG1,DEC_AMR_TEMP20_ADDRESS,1);

    VoC_bgt16_rd(Dplsf3_lab3,REG7,CONST_1_ADDR);        //if (sub (mode, MR475) == 0 || sub (mode, MR515) == 0)
    VoC_lw16i(REG6,STATIC_mr515_3_lsf_ADDR);            //{   p_cb1 = dico1_lsf;
    //   p_cb2 = dico2_lsf;
    //   p_cb3 = mr515_3_lsf;
    VoC_jump(Dplsf3_lab4);                      //}
Dplsf3_lab3:                            //else if (sub (mode, MR795) == 0)


    VoC_bne16_rd(Dplsf3_lab4,REG7,CONST_5_ADDR);        //{
    VoC_lw16i(REG4,STATIC_mr795_1_lsf_ADDR);                //p_cb1 = mr795_1_lsf;
    //p_cb2 = dico2_lsf;
    //p_cb3 = dico3_lsf;  }
Dplsf3_lab4:                            //else  {

    VoC_lw16_sd(REG2,5,DEFAULT);                    //index = *indice++;

    VoC_sw16inc_p(REG4,REG1,DEFAULT);               //   p_cb1 = dico1_lsf;
    VoC_sw16inc_p(REG5,REG1,DEFAULT);               //   p_cb2 = dico2_lsf;
    VoC_sw16_p(REG6,REG1,DEFAULT);              //   p_cb3 = dico3_lsf;
    VoC_lw16i_short(REG6,3,IN_PARALLEL);
    //}
    VoC_lw16i_set_inc(REG1,DEC_AMR_TEMP20_ADDRESS,1);

    VoC_mult16inc_rp(REG4,REG6,REG2,DEFAULT);               //p_dico = &p_cb1[add(index, add(index, index))];               move16 ();
    VoC_lw16i_set_inc(REG3,DEC_AMR_TEMP10_ADDRESS,1);
    VoC_add16inc_rp(REG0,REG4,REG1,DEFAULT);                //lsf1_r[0] = *p_dico++;
    //lsf1_r[1] = *p_dico++;
    VoC_jal(CII_R02R3_p_R6);                                        //lsf1_r[2] = *p_dico++;

    VoC_lw16inc_p(REG4,REG2,DEFAULT);               //index = *indice++;
    VoC_bgt16_rd(Dplsf3_lab6,REG7,CONST_1_ADDR);            //if ((sub (mode, MR475) == 0) || (sub (mode, MR515) == 0))
    VoC_shr16_ri(REG4,-1,DEFAULT);                  //   { index = shl(index,1);}
Dplsf3_lab6:
    VoC_mult16_rr(REG4,REG4,REG6,DEFAULT);                  //p_dico = &p_cb2[add(index, add(index, index))];               move16 ();
    VoC_NOP();

    VoC_add16inc_rp(REG0,REG4,REG1,DEFAULT);                //lsf1_r[3] = *p_dico++;
    //lsf1_r[4] = *p_dico++;
    VoC_jal(CII_R02R3_p_R6);                                        //lsf1_r[5] = *p_dico++;
    VoC_lw16i_short(REG6,4,DEFAULT);                                    //index = *indice++;
    VoC_mult16inc_rp(REG4,REG6,REG2,DEFAULT);           //p_dico = &p_cb3[shl(index, 2)];
    VoC_NOP();
    VoC_add16inc_rp(REG0,REG4,REG1,DEFAULT);            //lsf1_r[6] = *p_dico++;
    //lsf1_r[7] = *p_dico++;
    VoC_jal(CII_R02R3_p_R6);                                        //lsf1_r[8] = *p_dico++;
    //lsf1_r[9] = *p_dico++;

    VoC_lw16_sd(REG0, 4,DEFAULT);
    VoC_lw16i_short(REG3,10,IN_PARALLEL);
    VoC_lw16_sd(REG2, 1,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG3,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,DEC_AMR_TEMP10_ADDRESS,1);
    VoC_lw16i_set_inc(REG3,STATIC_pred_fac_ADDR,1);

    VoC_loop_i_short(10,DEFAULT);               //for (i = 0; i < M; i++) {
    VoC_startloop0
    VoC_lw16_p(REG6,REG0,DEFAULT);
    VoC_sub16_rd(REG4,REG1,CONST_10_ADDR);

    VoC_be16_rd(Dplsf3_lab7,REG7,CONST_8_ADDR);             //if (sub(mode, MRDTX) != 0)
    VoC_multf16inc_rp(REG6,REG6,REG3,DEFAULT);          //temp = add(mean_lsf[i], mult(st->past_r_q[i], pred_fac[i]));
Dplsf3_lab7:
    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_add16inc_rp(REG6,REG6,REG2,DEFAULT);

    VoC_add16_rr(REG6,REG5,REG6,DEFAULT);           //   lsf1_q[i] = add(lsf1_r[i], temp);
    VoC_sw16inc_p(REG5,REG0,DEFAULT);           //   st->past_r_q[i] = lsf1_r[i];
    VoC_sw16_p(REG6,REG4,DEFAULT);
    VoC_endloop0

Dplsf3_End:
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_jal(CII_Reorder_lsf);                   //Reorder_lsf(lsf1_q, LSF_GAP, M);

    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16_sd(REG3,4,DEFAULT);                    //Copy (lsf1_q, st->past_lsf_q, M);
    VoC_lw16i_short(REG6,10,DEFAULT);
    VoC_jal(CII_R02R3_p_R6);

    VoC_pop16(REG0,DEFAULT);                    //pop stack[n-2]
    VoC_pop16(REG2,DEFAULT);                    //pop stack[n-1]
    VoC_pop16(REG1,DEFAULT);                    //pop stack[n]
    VoC_jal(CII_Lsf_lsp);                       //Lsf_lsp(lsf1_q, lsp1_q, M);

    VoC_pop16(RA,DEFAULT);                  //pop stack[n+1]
    VoC_lw32z(ACC0,DEFAULT);
    VoC_return;
}

void CII_RESET_AMR_CONSTANT(void)
{
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i_set_inc(REG0,PRAM4_TABLE_ADDR,2);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,7,IN_PARALLEL);
    VoC_lw16i_short(REG6,3,DEFAULT);
    VoC_lw16i_short(REG7,2,IN_PARALLEL);
    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_lw16i_short(REG4,6,DEFAULT);
    VoC_lw16i_short(REG5,4,IN_PARALLEL);
    VoC_lw16i_short(REG6,12,DEFAULT);
    VoC_lw16i_short(REG7,8,IN_PARALLEL);
    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw16_p(REG3,REG0,DEFAULT);
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
    VoC_mac32_rd(REG5,CONST_1_ADDR);    //    L_tmp = L_mac (L_tmp, mult (st->y1_lo, a[1]), 1);
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


    VoC_add16_rd(REG0,REG4,CONST_2_ADDR);

    VoC_shr32_ri(ACC0,-2,DEFAULT);      //    L_tmp = L_shl (L_tmp, 2);


    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_lw32_p(RL6,REG0,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw32_d(REG67,STRUCT_POST_PROCESSSTATE_Y2_LO_ADDR+4);
    exit_on_warnings=ON;

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);      //    signal[i] = round(L_shl(L_tmp, 1));

//          VoC_and16_ri(REG6,0x7fff);
    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000_ADDR);

    VoC_sw32_p(RL6,REG4,DEFAULT);       //    st->y2_hi = st->y1_hi;
    VoC_sw32_p(REG67,REG0,DEFAULT);         //    st->y2_lo = st->y1_lo;
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    //    L_Extract (L_tmp, &st->y1_hi, &st->y1_lo);
    VoC_endloop1        //}
    VoC_return;
}
#ifdef voc_compile_only

// VoC_directive: FAKE_FUNCTION_ON
void CII_PRM2_BITS_POST_PROCESS_EFR(void) {};
void CII_EFR_DTX_PART1(void) {};
void CII_plsf5_dtx1(void) {};
void CII_EFR_DTX_ENC_RESET(void) {};
void CII_plsf5_dtx2(void) {};
void CII_plsf5_dtx3(void) {};
void CII_plsf5_dtx4(void) {};
void CII_tx_dtx(void) {};
void CII_update_gain_code_history_tx(void) {};
void CII_efr_dtx_func1(void) {};
void CII_efr_dtx_func2(void) {};
void CII_efr_dtx_func3(void) {};
void CII_efr_dtx_func4(void) {};
void CII_efr_dtx_func5(void) {};
void CII_efr_dtx_func6(void) {};
void CII_efr_dtx_func7(void) {};
void CII_efr_dtx_func8(void) {};
void CII_efr_dtx_func9(void) {};
void CII_rx_dtx(void) {};
void CII_reset_rx_dtx(void) {};
void CII_sid_codeword_encoding(void) {};
void CII_EFR_LEVINSON_PART (void) {};
void CII_update_lsf_history(void) {};
void CII_vad_computation (void) {};
void CII_periodicity_update(void) {};
void CII_efr_dtx_func11(void) {};
void CII_dtx_efr_dec_reset(void) {};
void CII_SUBFRAME_EFR_PART(void) {};
// VoC_directive: FAKE_FUNCTION_OFF

#endif
