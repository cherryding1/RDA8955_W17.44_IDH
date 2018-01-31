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



void CII_amr_dec_795(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_push16(REG1,DEFAULT);           // the pointer for parm  push16 stack[n-1]
    VoC_push16(REG0,DEFAULT);           // push16 stack[n-2] st->lsfState

    VoC_jal(CII_D_plsf_3);              //    D_plsf_3(st->lsfState, mode, bfi, parm, lsp_new);
    VoC_lw16i_short(REG6,3,DEFAULT);
    VoC_lw16_sd(REG7,1,DEFAULT);            //   parm += 3;
    VoC_lw32_sd(REG23,1,IN_PARALLEL);
    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG6,IN_PARALLEL);
    VoC_movreg16(REG0,REG1,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_sw16_sd(REG7,1,DEFAULT);
    VoC_jal(CII_Int_lpc_1to3);          //   Int_lpc_1to3(st->lsp_old, lsp_new, A_t);

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
Dec_amr_LoopStart_795:
    VoC_bne16_rd(Dec_amr_LoopEnd_7951,REG5,CONST_160_ADDR);     //for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
    VoC_jump(Dec_amr_LoopEnd_795);
Dec_amr_LoopEnd_7951:
    VoC_lw16i_short(REG3,1,DEFAULT);        //{
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_push16(REG5,DEFAULT);               //   i_subfr  push16 stack[n-3]
    VoC_add16_rd(REG6,REG3,DEC_AMR_SUBFRNR_ADDRESS);    //   subfrNr = add(subfrNr, 1);
    VoC_sub16_rd(REG4,REG3,DEC_AMR_EVENSUBFR_ADDRESS);  //evenSubfr = sub(1, evenSubfr);
    VoC_sw16_d(REG6,DEC_AMR_SUBFRNR_ADDRESS);
    VoC_sw16_d(REG4,DEC_AMR_EVENSUBFR_ADDRESS);     //   pit_flag = i_subfr;

    VoC_bne16_rd(Dec_amr_L14_795,REG5,CONST_80_ADDR);   //   if (sub (i_subfr, L_FRAME_BY2) == 0)
    //   { if (sub(mode, MR475) != 0 && sub(mode, MR515) != 0)
    VoC_lw16i_short(REG5,0,DEFAULT);            //      {  pit_flag = 0;  }
Dec_amr_L14_795:
    VoC_lw16_sd(REG0,2,DEFAULT);            // assure REG0 incr=1
    VoC_sw16_d(REG5,DEC_AMR_PIT_FLAG_ADDRESS);
    VoC_lw16inc_p(REG3,REG0,DEFAULT);           //index = *parm++;
    VoC_lw16_d(REG4,STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR);
    VoC_sw16_d(REG3,DEC_AMR_INDEX_ADDRESS);
    VoC_sw16_sd(REG0,2,DEFAULT);            // restore the new address of parm


    // begin of dec_amr_not122_s1_795

    VoC_lw16i_short(REG2,10,IN_PARALLEL);           //     delta_frc_low = 5;
    VoC_lw16i_short(REG1,19,DEFAULT);       //     delta_frc_range = 9;
    VoC_sub16_rr(REG6,REG4,REG2,IN_PARALLEL);           //     t0_min = sub(st->old_T0, delta_frc_low);
    VoC_bnlt16_rd(Dec_amr_L18_795,REG6,CONST_20_ADDR);      //     if (sub(t0_min, PIT_MIN) < 0)
    VoC_lw16i_short(REG6,20,DEFAULT);               //     {           t0_min = PIT_MIN;
Dec_amr_L18_795:                            //     }
    VoC_add16_rr(REG7,REG6,REG1,DEFAULT);           //     t0_max = add(t0_min, delta_frc_range);
    VoC_bngt16_rd(Dec_amr_L19_795,REG7,CONST_143_ADDR);     //     if (sub(t0_max, PIT_MAX) > 0)
    VoC_lw16i(REG7,143);                    //     {        t0_max = PIT_MAX;
    VoC_sub16_rr(REG6,REG7,REG1,DEFAULT);           //        t0_min = sub(t0_max, delta_frc_range);
Dec_amr_L19_795:                            //     }

    //  begin of CII_Dec_lag3_795                   //     Dec_lag3 (index, t0_min, t0_max, pit_flag, st->old_T0,&T0, &T0_frac, flag4);
    /*************************************
    Function  void CII_Dec_lag3_795(void)
    input: index      ->REG3  //unchanged in REG3
          flag4      =0
          t0_min     ->REG6
          t0_max     ->REG7
          i_subfr    ->DEC_AMR_PIT_FLAG_ADDRESS
          st->old_T0 ->REG4  //unchanged in REG4
    Output:
          * T0       ->REG6
          * T0_frac  ->REG7
    Note:REG0,ACC01,RL67 is not used in this function
    Create by Kenneth  06/29/2004
    Optimized by Kenneth 09/03/2004
    ************************************/
    VoC_lw16i_short(REG0,2,DEFAULT);
    VoC_lw16i(REG1,10923);
    VoC_bnez16_d(Dec_lag3_L1_795,DEC_AMR_PIT_FLAG_ADDRESS); //if (i_subfr == 0) {
//  VoC_sub16_rd(REG6,REG3,CONST_112_ADDR);     //      *T0 = sub(index, 112);
//  VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i(REG6,112);
    VoC_sub16_rr(REG6,REG3,REG6,DEFAULT);       //      *T0 = sub(index, 112);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);


    VoC_bnlt16_rd(Dec_lag3_L2_795,REG3,CONST_197_ADDR); //     if (sub(index, 197) < 0) {
    VoC_add16_rr(REG2,REG3,REG0,DEFAULT);       //      *T0 = add(mult(add(index, 2), 10923), 19);

    VoC_multf16_rr(REG6,REG2,REG1,DEFAULT); //      i = add(add(*T0, *T0), *T0);
    VoC_lw16i_short(REG5,19,DEFAULT);
    VoC_lw16i_short(REG0,-58,IN_PARALLEL);
    VoC_add16_rr(REG6,REG6,REG5,DEFAULT);       //      *T0_frac = add(sub(index, i), 58);

    VoC_movreg16(REG2,REG6,DEFAULT);
    VoC_jump(Dec_lag3_LA_795);          //   } else {
    //      *T0_frac = 0;       }
Dec_lag3_L1_795:                        //} else {
    //if (flag4 == 0) {
    VoC_add16_rr(REG2,REG3,REG0,DEFAULT);       //      i = sub(mult(add(index, 2), 10923), 1);
    VoC_multf16_rr(REG2,REG2,REG1,DEFAULT);
    VoC_NOP();
    VoC_sub16_rd(REG2,REG2,CONST_1_ADDR);
    VoC_add16_rr(REG6,REG2,REG6,DEFAULT);       //      *T0 = add(i, t0_min);
Dec_lag3_LA_795:
    VoC_add16_rr(REG7,REG2,REG2,DEFAULT);       //      i = add(add(i, i), i);
    VoC_add16_rr(REG7,REG7,REG2,DEFAULT);
    VoC_sub16_rr(REG7,REG3,REG7,DEFAULT);       //      *T0_frac = sub(sub(index, 2), i);
    VoC_sub16_rr(REG7,REG7,REG0,DEFAULT);
Dec_lag3_L2_795:

    // end of CII_Dec_lag3_795
    VoC_sw16_d(REG6,STRUCT_DECOD_AMRSTATE_T0_LAGBUFF_ADDR);     //     st->T0_lagBuff = T0;

    VoC_bez16_d(Dec_amr_L20_795,DEC_AMR_BFI_ADDRESS);       //  if (bfi != 0)
    VoC_bnlt16_rd(Dec_amr_L20_795A,REG4,CONST_143_ADDR);    //  {      if (sub (st->old_T0, PIT_MAX) < 0)
    VoC_add16_rd(REG4,REG4,CONST_1_ADDR);           //  {   st->old_T0 = add(st->old_T0, 1);
Dec_amr_L20_795A:                       //  }
    VoC_lw16i_short(REG7,0,DEFAULT );           //     T0 = st->old_T0;
    VoC_movreg16(REG6,REG4,IN_PARALLEL);            //     T0_frac = 0;
    VoC_sw16_d(REG4,STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR);

Dec_amr_L20_795:                        //  }
    VoC_lw16i_short(REG2,1,DEFAULT);                //  Pred_lt_3or6 (st->exc, T0, T0_frac, L_SUBFR, 1);
    // end of dec_amr_not122_s1_795

    VoC_sw32_d(REG67,DEC_AMR_T0_ADDRESS);       // restore T0 &T0_frac
    VoC_lw16i(REG5,STRUCT_DECOD_AMRSTATE_EXC_ADDR);
    VoC_jal(CII_Pred_lt_3or6);

    VoC_lw16i_set_inc(REG0,DEC_AMR_CODE_ADDRESS,1);
    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16_sd(REG2,2,DEFAULT);            // parm in reg2
    VoC_push16(REG0,DEFAULT);               // code push16 stack[n-4]

    VoC_lw16inc_p(REG1,REG2,DEFAULT);       // restore parm
    VoC_lw16inc_p(REG6,REG2,DEFAULT);       // {   MR74/795
    VoC_sw16_sd(REG2,3,DEFAULT);            //    index = *parm++;
    //    i = *parm++;
    VoC_jal(CII_decode_4i40_17bits_795);        //    decode_4i40_17bits (i, index, code);
    //    pit_sharp = shl (st->sharp, 1);  }
    VoC_lw16_d(REG5,STRUCT_DECOD_AMRSTATE_SHARP_ADDR);
    VoC_jal(CII_amr_dec_com_sub1);


    // begin of dec_amr_795122_s1_795
    VoC_lw16i_short(REG4,5,DEFAULT);
    VoC_bez16_d(Dec_amr_L41_795,DEC_AMR_BFI_ADDRESS);   //   {   if (bfi != 0)
    VoC_lw16i_set_inc(REG0,STRUCT_EC_GAIN_PITCHSTATE_ADDR,1);   //   {
    VoC_lw16d_set_inc(REG1,STRUCT_DECOD_AMRSTATE_STATE_ADDR,1);
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG1,DEFAULT);
    VoC_jal(CII_ec_gain_pitch);             //      ec_gain_pitch (st->ec_gain_p_st, st->state, &gain_pit);
    VoC_pop16(REG1,DEFAULT);
    VoC_pop16(REG0,DEFAULT);
    VoC_jump(Dec_amr_L42_795);                  //      }
Dec_amr_L41_795:                            //      else
    //   gain_pit = d_gain_pitch (mode, index);   }
    /*
    **************************************************************************
    *
    *  Function    : CII_d_gain_pitch
       Input:      enum Mode mode->REG4
                   Word16 index  ->REG1
    *  Output:     gain->REG6(DEC_AMR_GAIN_PIT_ADDRESS)
    *  Created by Kenneth  07/15/2004
    **************************************************************************
    */
    VoC_lw16i(REG1,STATIC_CONST_QUA_GAIN_PITCH_ADDR);
    VoC_add16_rr(REG6,REG6,REG1,DEFAULT);
    VoC_NOP();
    VoC_lw16_p(REG6,REG6,DEFAULT);      // gain = qua_gain_pitch[index];
    VoC_NOP();
    VoC_sw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);
//*  Function    : CII_d_gain_pitch end
Dec_amr_L42_795:
    VoC_lw16i_set_inc(REG0,STRUCT_EC_GAIN_PITCHSTATE_ADDR,1);
    VoC_NOP();
    VoC_push16(REG0,DEFAULT);               // push16 stack[nn]  st->ec_gain_p_st
    VoC_jal(CII_ec_gain_pitch_update);          //      ec_gain_pitch_update (st->ec_gain_p_st, bfi, st->prev_bf,&gain_pit);
    VoC_pop16(REG0,DEFAULT);
    VoC_lw16_sd(REG2,2,DEFAULT);            // parm in reg2

    VoC_lw16i_set_inc(REG0,STRUCT_EC_GAIN_CODESTATE_ADDR,1);
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    VoC_sw16_sd(REG2,2,DEFAULT);            //      index = *parm++;

    VoC_lw16i_set_inc(REG1,STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR,1);
    VoC_push16(REG0,DEFAULT);               // push16 st->ec_gain_c_st
    VoC_push16(REG1,DEFAULT);               // push16 st->pred_state

    VoC_bnez16_d(Dec_amr_L44_795,DEC_AMR_BFI_ADDRESS);      //      if (bfi == 0)
    VoC_push16(REG6,DEFAULT);               //  push16 index
    //         d_gain_code (st->pred_state, mode, index, code, &gain_code);
    /*
    **************************************************************************
    *
    *  Function    : CII_d_gain_code
    *  Input: gc_predState *pred_state-> REG1() push16        stack[n]
              enum Mode mode,         -> GLOBAL_MODE_ADDR        //(direct address access, needn't pass the address as a parameter)
              Word16 index,           -> REG2 push16          stack[n-2]
              Word16 code[],          -> REG0(incr 1) push16  stack[n-1]
              Word16 *gain_code       ->DEC_AMR_GAIN_CODE_ADDRESS    //(direct address access, needn't pass the address as a parameter)
      Created by  Kenneth   07/14/2004
    **************************************************************************
    */
    VoC_lw16i_short(REG7,5,IN_PARALLEL);
    VoC_jal(CII_gc_pred);               //return exp->REG7 ;frac->REG6

    VoC_pop16(REG5,DEFAULT);   //p = &qua_gain_code[add (add (index, index), index)];

    VoC_mult16_rd(REG5,REG5,CONST_3_ADDR);
    VoC_lw16i(REG3,STATIC_CONST_qua_gain_code_ADDR);
    VoC_movreg32(REG01,REG67,DEFAULT);
    VoC_add16_rr(REG6,REG5,REG3,IN_PARALLEL);               //  p->REG6

    VoC_sub16_dr(REG7,CONST_9_ADDR,REG7);
    VoC_lw16i_short(REG1,14,DEFAULT);           //     gcode0 = extract_l (Pow2 (14, frac));
    VoC_jal(CII_Pow2);
    VoC_multf32_rp(REG23,REG2,REG6,DEFAULT);        //    L_tmp = L_mult(*p++, gcode0);
    VoC_lw16i(REG5,STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR);
    VoC_shr32_rr(REG23,REG7,DEFAULT);           //    L_tmp = L_shr(L_tmp, sub(9, exp));
    VoC_movreg16(REG2,REG3,DEFAULT);            //    *gain_code = extract_h(L_tmp);
    VoC_movreg16(REG0,REG6,DEFAULT);
    VoC_sw16_d(REG2,DEC_AMR_GAIN_CODE_ADDRESS);
    VoC_inc_p(REG0,DEFAULT);
    VoC_lw16inc_p(REG7,REG0,DEFAULT);           //qua_ener_MR122 = *p++;
    VoC_lw16inc_p(REG6,REG0,DEFAULT);           //qua_ener = *p++;

    VoC_jal(CII_gc_pred_update);                //gc_pred_update(pred_state, qua_ener_MR122, qua_ener);

    VoC_lw16i_short(INC1,1,DEFAULT);
//*  Function    : CII_d_gain_code end

    VoC_jump(Dec_amr_L45_795);                  //      }
Dec_amr_L44_795:                            //      else
    VoC_lw16d_set_inc(REG2,STRUCT_DECOD_AMRSTATE_STATE_ADDR,1);//      {
    VoC_NOP();
    VoC_push16(REG2,DEFAULT);               // push16 st->state
    VoC_jal(CII_ec_gain_code);              //      ec_gain_code (st->ec_gain_c_st, st->pred_state, st->state,&gain_code);
    VoC_pop16(REG2,DEFAULT);                //      }
Dec_amr_L45_795:
    VoC_pop16(REG1,DEFAULT);                //  pop16   st->pred_state
    VoC_lw16_d(REG6,DEC_AMR_GAIN_CODE_ADDRESS);
    VoC_jal(CII_ec_gain_code_update);           //      ec_gain_code_update (st->ec_gain_c_st, bfi, st->prev_bf,&gain_code);
    VoC_pop16(REG0,DEFAULT);                //  pop16  st->ec_gain_c_st

    VoC_lw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);      //      pit_sharp = gain_pit;

    VoC_lw16i(REG5,13017);
    VoC_bngt16_rr(Dec_amr_L46_795,REG6,REG5);       //      if (sub (pit_sharp, SHARPMAX) > 0)
    VoC_movreg16(REG6,REG5,DEFAULT);            //      {  pit_sharp = SHARPMAX;  }}
Dec_amr_L46_795:
    // end of dec_amr_795122_s1_795


    VoC_shr16_ri(REG6,-1,DEFAULT );             //pit_sharp = shl (pit_sharp, 1);
    VoC_lw16_d(REG2,DEC_AMR_GAIN_PIT_ADDRESS);      // gain_pit in reg2
    VoC_sw16_d(REG6,DEC_AMR_PIT_SHARP_ADDRESS);

    VoC_bngt16_rd(Dec_amr_L52_795,REG6,CONST_0x00004000L_ADDR);     //if (sub (pit_sharp, 16384) > 0)
    VoC_lw16i_set_inc(REG0,STRUCT_DECOD_AMRSTATE_EXC_ADDR,1);   //{
    VoC_lw16i_set_inc(REG1,DEC_AMR_EXCP_ADDRESS,1);
    VoC_multf16inc_rp(REG5,REG6,REG0,DEFAULT);      //       temp = mult (st->exc[i], pit_sharp);
    VoC_loop_i_short(40,DEFAULT);                   //   for (i = 0; i < L_SUBFR; i++)
    VoC_startloop0                      //    {
    VoC_multf32_rr(ACC0,REG5,REG2,DEFAULT);     //       L_temp = L_mult (temp, gain_pit);
    VoC_NOP();
    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000_ADDR);      //       excp[i] = round (L_temp);
    VoC_multf16inc_rp(REG5,REG6,REG0,DEFAULT);      //       temp = mult (st->exc[i], pit_sharp);
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    VoC_endloop0                            //    }
Dec_amr_L52_795:                                //}
    VoC_lw16i_set_inc(REG0,13017,1);

    VoC_movreg16(REG7,REG2,DEFAULT);        //    st->sharp = gain_pit;
    VoC_bngt16_rr(Dec_amr_L54_795,REG7,REG0);           //    if (sub (st->sharp, SHARPMAX) > 0)
    VoC_movreg16(REG7,REG0,DEFAULT);        //    {  st->sharp = SHARPMAX;
Dec_amr_L54_795:                        // }
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_sw16_d(REG7,STRUCT_DECOD_AMRSTATE_SHARP_ADDR);  //}

    VoC_bnez16_d(Dec_amr_L56_795,DEC_AMR_BFI_ADDRESS);      //if ( bfi == 0 )
    VoC_lw16i_set_inc(REG3,STRUCT_DECOD_AMRSTATE_LTPGAINHISTORY_ADDR,1);//{
    VoC_add16_rr(REG0,REG3,REG6,DEFAULT);
    VoC_lw16i_short(REG6,8,DEFAULT);                   //   for (i = 0; i < 8; i++)
    VoC_jal(CII_R02R3_p_R6);                //   {   st->ltpGainHistory[i] = st->ltpGainHistory[i+1]; }
    VoC_sw16_p(REG2,REG3,DEFAULT);          //   st->ltpGainHistory[8] = gain_pit;
    //}
Dec_amr_L56_795:
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG0,DEFAULT);
    VoC_jal(CII_Int_lsf);               //Int_lsf(prev_lsf, st->lsfState->past_lsf_q, i_subfr, lsf_i);

    VoC_lw32_d(REG67,STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR);
    VoC_add32_rd(REG67,REG67,CONST_0x00010001_ADDR);
    VoC_bgt16_rd(CGAver_L5_795,REG5,CONST_5325_ADDR);       //if (sub(diff, 5325) > 0)
    VoC_lw16i_short(REG7,0,DEFAULT);            //{ st->hangVar = add(st->hangVar, 1);}
CGAver_L5_795:                      // else{st->hangVar = 0; }

    VoC_bngt16_rd(CGAver_L6_795,REG7,CONST_10_ADDR);    //if (sub(st->hangVar, 10) > 0)
    VoC_lw16i_short(REG6,1,DEFAULT);            //{  st->hangCount = 0;
CGAver_L6_795:
    //bgMix = 8192; cbGainMix = gain_code;
    //st->hangCount = add(st->hangCount, 1);
    VoC_pop16(REG5,DEFAULT);
    VoC_sw32_d(REG67,STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR);
    //return cbGainMix;  ->ACC1_HI
    //  end of Cb_gain_average

    VoC_pop16(REG7,DEFAULT);
    //VoC_pop32(ACC0,IN_PARALLEL);      //   pop32 stack32[n]

    //   gain_code_mix = gain_code;      // MR74, MR795, MR122
    //}

    VoC_sw16_d(REG5,DEC_AMR_GAIN_CODE_MIX_ADDRESS); // gain_code in REG5

    VoC_lw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);
    VoC_lw16i_short(REG7,1,DEFAULT);        //   tmp_shift = 1;

    VoC_jal(CII_amr_dec_com_sub3);

    VoC_blt16_rd(Phdisp_L16_795_go,REG4,CONST_2_ADDR);      //   sub(mode, MR74) != 0 && sub(impNr, 2) < 0)
    VoC_jump(Phdisp_L16_795);
Phdisp_L16_795_go:

    // begin of dec_Phdisp_not74102122_s1_795
    VoC_lw16i_set_inc(REG1,DEC_AMR_BUFA40_ADDRESS,1);   //{   nze = 0;
    // inno[] in REG2
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i_short(REG0,40,IN_PARALLEL);
    VoC_add16_rr(REG0,REG1,REG0,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_loop_i_short(40,DEFAULT);           //    for (i = 0; i < L_SUBFR; i++)
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_startloop0                  //    {
    VoC_lw16_p(REG6,REG2,DEFAULT);          //        if (inno[i] != 0)
    VoC_bez16_r(Phdisp_L12_795,REG6);
    VoC_sw16inc_p(REG3,REG1,DEFAULT);       //        { ps_poss[nze] = i;
    VoC_add32_rd(ACC1,ACC1,CONST_1_ADDR);       //            nze = add (nze, 1);}
Phdisp_L12_795:
    VoC_sw16inc_p(REG6,REG0,DEFAULT);       //        inno_sav[i] = inno[i];
    VoC_inc_p(REG3,DEFAULT);
    VoC_sw16inc_p(ACC1_HI,REG2,DEFAULT);        //        inno[i] = 0;
    VoC_endloop0                        //    }

    //    if (sub (mode, MR795) == 0)
    VoC_lw16i_set_inc(REG3,STATIC_CONST_ph_imp_mid_ADDR,1); //        { ph_imp = ph_imp_mid_MR795;   }
    VoC_bnez16_r(Phdisp_L15_795,REG4);              //    { if (impNr == 0)
    VoC_lw16i_set_inc(REG3,STATIC_CONST_ph_imp_low_MR795_ADDR,1);   //        { ph_imp = ph_imp_low_MR795;   }
Phdisp_L15_795:


    VoC_lw16i_set_inc(REG1,DEC_AMR_BUFA40_ADDRESS,1);
    VoC_movreg16(REG7,ACC1_LO,DEFAULT);
    VoC_add16_rd(REG2,REG1,CONST_40_ADDR);
    VoC_movreg32(ACC0,REG23,DEFAULT);

    VoC_loop_r(1,REG7);                 //    for (nPulse = 0; nPulse < nze; nPulse++)    {
    VoC_lw16inc_p(REG6,REG1,DEFAULT);       //        ppos = ps_poss[nPulse];
    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_sub16_dr(REG4,CONST_40_ADDR,REG6);      //        j = 0;
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16_p(REG2,REG2,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG6,IN_PARALLEL);

    VoC_multf16inc_rp(REG7,REG2,REG3,DEFAULT);
    VoC_loop_r_short(REG4,DEFAULT);         //        for (i = ppos; i < L_SUBFR; i++)
    VoC_startloop0                  //        {
    //   tmp1 = mult(inno_sav[ppos], ph_imp[j++]);
    VoC_add16_rp(REG7,REG7,REG0,DEFAULT);      //            inno[i] = add(inno[i], tmp1);
    VoC_multf16inc_rp(REG7,REG2,REG3,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0                       //        }

    VoC_bez16_r(Phdisp_L20_795,REG6);
    VoC_sub16_rd(REG3,REG3,CONST_1_ADDR);
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_multf16inc_rp(REG7,REG2,REG3,DEFAULT);
    VoC_loop_r_short(REG6,DEFAULT);            //        for (i = 0; i < ppos; i++)
    VoC_startloop0                     //        {
    //    tmp1 = mult(inno_sav[ppos], ph_imp[j++]);
    VoC_add16_rp(REG7,REG7,REG0,DEFAULT);      //            inno[i] = add(inno[i], tmp1);
    VoC_multf16inc_rp(REG7,REG2,REG3,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0                    //        }
Phdisp_L20_795:
    VoC_movreg32(REG23,ACC0,DEFAULT);
    VoC_endloop1                        // }
    // end of dec_Phdisp_not74102122_s1_795

Phdisp_L16_795:

    VoC_jal(CII_amr_dec_com_sub4);
    VoC_pop32(ACC0,DEFAULT);        //   pop32 stack32[n]

    VoC_jal(CII_amr_dec_com_sub5);
    VoC_jump(Dec_amr_LoopStart_795);    //  }
Dec_amr_LoopEnd_795:

    VoC_pop16(REG5,DEFAULT);        // pop16 stack[n-2]
    VoC_pop16(REG4,DEFAULT);        // pop16 stack[n-1]
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_lw16_sd(REG3,1,DEFAULT);
    VoC_return;

}


/**************************************************************
 function void CII_decode_4i40_17bits_795(void)
 Input: Word16 sign,       i -> REG6
        Word16 index,      i -> REG1
        Word16 cod[]       o <- REG0  push REG0

  Version 1.0 Created by Kenneth       07/02/2004
  Version 1.1 Optimised by Kenneth     08/16/2004
**************************************************************/
void CII_decode_4i40_17bits_795(void)
{

    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_lw16i_set_inc(REG2,DEC_AMR_D2PF_POS_ADDRESS,1);
    VoC_lw16i_set_inc(REG3,0,1);
    VoC_push16(REG2,DEFAULT);
    VoC_lw16i(REG4,STATIC_CONST_DGRAY_ADDR);        //i = index & 7;
    VoC_loop_i_short(4,DEFAULT);            //i = dgray[i];
    VoC_lw16i_short(REG7,5,IN_PARALLEL);        //pos[0] = add(i, shl(i, 2));
    VoC_startloop0                  //index = shr(index, 3);
    VoC_lw16i_short(REG5,7,DEFAULT);         //i = index & 7;
    VoC_and16_rr(REG5,REG1,DEFAULT);         //i = dgray[i];
    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);        //i = add(i, shl(i, 2)); pos[1] = add(i, 1);
    VoC_shr16_ri(REG1,3,DEFAULT);            //index = shr(index, 3);
    VoC_mult16_rp(REG5,REG7,REG5,DEFAULT);       //i = index & 7;
    VoC_NOP();
    VoC_add16_rr(REG5,REG5,REG3,DEFAULT);            //i = dgray[i];
    VoC_bne16_rd(D4_17_Lab1_795,REG3,CONST_2_ADDR);  //i = add(i, shl(i, 2)); pos[2] = add(i, 2);
    VoC_and16_rr(REG0,REG1,DEFAULT);         //index = shr(index, 3);
    VoC_shr16_ri(REG1,1,DEFAULT);            //j = index & 1;
    VoC_add16_rr(REG3,REG3,REG0,IN_PARALLEL);    //index = shr(index, 1);
D4_17_Lab1_795:                         //i = index & 7;
    VoC_sw16inc_p(REG5,REG2,DEFAULT);        //i = dgray[i];
    VoC_inc_p(REG3,DEFAULT);             //i = add(i, shl(i, 2));
    VoC_endloop0                    //i = add(i, 3);        pos[3] = add(i, j);

    VoC_pop16(REG2,DEFAULT);
    //
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16_sd(REG0,0,IN_PARALLEL);
    VoC_loop_i_short(40,DEFAULT);             //for (i = 0; i < L_SUBFR; i++) {
    VoC_startloop0
    VoC_sw16inc_p(REG3,REG0,DEFAULT);         //    cod[i] = 0;
    VoC_endloop0                                  //}

    VoC_loop_i_short(4,DEFAULT);               //for (j = 0; j < NB_PULSE; j++) {
    VoC_lw16_sd(REG0,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16i_short(REG1,1,DEFAULT);          //    i = sign & 1;
    VoC_and16_rr(REG1,REG6,DEFAULT);
    VoC_add16inc_rp(REG3,REG0,REG2,IN_PARALLEL);
    VoC_lw16i(REG4,-8192);
    VoC_bez16_r(d_pf_sub_L11_795,REG1);           //    if (i != 0) {
    VoC_lw16i(REG4,8191);                     // cod[pos[j]] = 8191;
d_pf_sub_L11_795:                                     //    } else { cod[pos[j]] = -8192;
    VoC_shr16_ri(REG6,1,DEFAULT);             //    sign = shr(sign, 1);
    VoC_sw16_p(REG4,REG3,DEFAULT);
    VoC_endloop0                  //    }  }
    //

    VoC_return;
}

#ifdef voc_compile_only
// VoC_directive: FAKE_FUNCTION_ON
void CII_amr_dec_475(void) {};
void CII_amr_dec_515(void) {};
void CII_amr_dec_59(void) {};
void CII_amr_dec_67(void) {};
void CII_amr_dec_74(void) {};
void CII_amr_dec_102(void) {};
void CII_amr_dec_122(void) {};
void CII_COD_AMR_475(void) {};
void CII_COD_AMR_515(void) {};
void CII_COD_AMR_59(void) {};
void CII_COD_AMR_67(void) {};
void CII_COD_AMR_74(void) {};
void CII_COD_AMR_102(void) {};
void CII_COD_AMR_122(void) {};
void CII_HR_Encode(void) {};
void CII_HR_Decode(void) {};
void CII_FR_Encode(void) {};
void CII_FR_Decode(void) {};

// VoC_directive: FAKE_FUNCTION_OFF
#endif




