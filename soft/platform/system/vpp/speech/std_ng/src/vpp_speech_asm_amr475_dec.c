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


void CII_amr_dec_475(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_push16(REG1,DEFAULT);           // the pointer for parm  push16 stack[n-1]
    VoC_push16(REG0,DEFAULT);           // push16 stack[n-2] st->lsfState
    VoC_jal(CII_D_plsf_3);              //    D_plsf_3(st->lsfState, mode, bfi, parm, lsp_new);
    VoC_lw16i_short(REG6,3,DEFAULT);
    VoC_lw16_sd(REG7,1,DEFAULT);        //   parm += 3;
    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG6,IN_PARALLEL);
    VoC_movreg16(REG0,REG1,DEFAULT);
    VoC_lw32_sd(REG23,1,IN_PARALLEL);
    VoC_sw16_sd(REG7,1,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_jal(CII_Int_lpc_1to3);          //   Int_lpc_1to3(st->lsp_old, lsp_new, A_t);

    VoC_pop32(RL6,DEFAULT);             // pop32 stack32[n-1]
    VoC_pop16(REG2,DEFAULT);            // pop16 stack[n-2] st->lsfState
    VoC_pop32(REG45,DEFAULT);           // pop32 stack32[n]

    VoC_movreg16(REG0,REG5,DEFAULT);
    VoC_movreg16(REG1,REG4, IN_PARALLEL );      //for (i = 0; i < M; i++)
    VoC_jal(CII_Copy_M);                //{st->lsp_old[i] = lsp_new[i];//}

    VoC_lw16i_short(REG5,0,DEFAULT);        //evenSubfr = 0;
    VoC_lw16i_short(REG6,-1,DEFAULT);       //subfrNr = -1;
    VoC_push16(RL6_HI,DEFAULT);         //Az = A_t;     Az in stack[n-2]
    VoC_sw16_d(REG5,DEC_AMR_EVENSUBFR_ADDRESS);
    VoC_sw16_d(REG6,DEC_AMR_SUBFRNR_ADDRESS);

Dec_amr_LoopStart_475:
    VoC_bne16_rd(Dec_amr_LoopEnd_4751,REG5,CONST_160_ADDR);     //for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
    VoC_jump(Dec_amr_LoopEnd_475);
Dec_amr_LoopEnd_4751:
    VoC_lw16i_short(REG3,1,DEFAULT);        //{
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_push16(REG5,DEFAULT);               //   i_subfr  push16 stack[n-3]
    VoC_add16_rd(REG6,REG3,DEC_AMR_SUBFRNR_ADDRESS);    //   subfrNr = add(subfrNr, 1);
    VoC_sub16_rd(REG4,REG3,DEC_AMR_EVENSUBFR_ADDRESS);  //evenSubfr = sub(1, evenSubfr);
    VoC_sw16_d(REG6,DEC_AMR_SUBFRNR_ADDRESS);
    VoC_sw16_d(REG4,DEC_AMR_EVENSUBFR_ADDRESS);     //   pit_flag = i_subfr;


    VoC_lw16_sd(REG0,2,DEFAULT);            // assure REG0 incr=1
    VoC_sw16_d(REG5,DEC_AMR_PIT_FLAG_ADDRESS);
    VoC_lw16inc_p(REG3,REG0,DEFAULT);           //index = *parm++;
    VoC_lw16_d(REG4,STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR);

    VoC_sw16_d(REG3,DEC_AMR_INDEX_ADDRESS);
    VoC_sw16_sd(REG0,2,DEFAULT);            // restore the new address of parm
    // begin of dec_amr_not122_s1_475
    //        flag4 = 1;
    VoC_lw16i_short(REG2,5,IN_PARALLEL);        //     delta_frc_low = 5;
    VoC_lw16i_short(REG1,9,DEFAULT);        //     delta_frc_range = 9;
    VoC_sub16_rr(REG6,REG4,REG2,IN_PARALLEL);           //     t0_min = sub(st->old_T0, delta_frc_low);
    VoC_bnlt16_rd(Dec_amr_L18_475,REG6,CONST_20_ADDR);      //     if (sub(t0_min, PIT_MIN) < 0)
    VoC_lw16i_short(REG6,20,DEFAULT);               //     {           t0_min = PIT_MIN;
Dec_amr_L18_475:                            //     }
    VoC_add16_rr(REG7,REG6,REG1,DEFAULT);           //     t0_max = add(t0_min, delta_frc_range);
    VoC_bngt16_rd(Dec_amr_L19_475,REG7,CONST_143_ADDR);     //     if (sub(t0_max, PIT_MAX) > 0)
    VoC_lw16i(REG7,143);                  //     {        t0_max = PIT_MAX;
    VoC_sub16_rr(REG6,REG7,REG1,DEFAULT);              //        t0_min = sub(t0_max, delta_frc_range);

Dec_amr_L19_475:                            //     }

//     VoC_jal(CII_Dec_lag3_475);                  //     Dec_lag3 (index, t0_min, t0_max, pit_flag, st->old_T0,&T0, &T0_frac, flag4);
    /*************************************
     Function  void CII_Dec_lag3_475(void)
       input: index      ->REG3  //unchanged in REG3
              flag4      ->REG5
              t0_min     ->REG6
              t0_max     ->REG7
              i_subfr    ->DEC_AMR_PIT_FLAG_ADDRESS
              st->old_T0 ->REG4  //unchanged in REG4
       Output:
              * T0       ->REG6
              * T0_frac  ->REG7
       Note:ACC01,RL67 is not used in this function
       Create by Kenneth  06/29/2004
       Optimized by Kenneth 09/03/2004
      ************************************/
///begin of CII_Dec_lag3_475
    VoC_lw16i_short(REG0,2,DEFAULT);
    VoC_lw16i(REG1,10923);
    VoC_bnez16_d(Dec_lag3_L3_475,DEC_AMR_PIT_FLAG_ADDRESS); //if (i_subfr == 0) {

//  VoC_lw16i(REG6,112);
    VoC_sub16_rd(REG6,REG3,(50+TABLE_MR67_ADDR));       //      *T0 = sub(index, 112);
    VoC_lw16i_short(REG7,0,DEFAULT);



    VoC_bnlt16_rd(Dec_lag3_L2_475,REG3,CONST_197_ADDR); //     if (sub(index, 197) < 0) {
    VoC_add16_rr(REG2,REG3,REG0,DEFAULT);       //      *T0 = add(mult(add(index, 2), 10923), 19);

    VoC_multf16_rr(REG6,REG2,REG1,DEFAULT);     //      i = add(add(*T0, *T0), *T0);

    VoC_lw16i_short(REG5,19,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG5,DEFAULT);       //      *T0_frac = add(sub(index, i), 58);
    VoC_lw16i_short(REG0,58,IN_PARALLEL);       //   } else {  *T0_frac = 0;       }

    VoC_add16_rr(REG7,REG6,REG6,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG6,DEFAULT);
    VoC_sub16_rr(REG7,REG3,REG7,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG0,DEFAULT);
Dec_lag3_L2_475:
//  VoC_NOP();      // TO prevent from a compiler bug
    VoC_jump(Dec_lag3_LEnd_475);            //} else {
Dec_lag3_L3_475:                        // if (flag4 == 1)  {
    VoC_lw16i_short(REG0,5,DEFAULT);
    VoC_movreg16(REG5,REG4,DEFAULT);            //      tmp_lag = T0_prev;
    VoC_add16_rr(REG2,REG6,REG0,IN_PARALLEL);       //      if ( sub( sub(tmp_lag, t0_min), 5) > 0)
    VoC_bngt16_rr(Dec_lag3_L4_475,REG5,REG2);
    VoC_movreg16(REG5,REG2,DEFAULT);            //         tmp_lag = add (t0_min, 5);
Dec_lag3_L4_475:
    VoC_sub16_rd(REG7,REG7,CONST_4_ADDR);       //      if ( sub( sub(t0_max, tmp_lag), 4) > 0)
    VoC_bngt16_rr(Dec_lag3_L5_475,REG7,REG5);       //         tmp_lag = sub (t0_max, 4);
    VoC_movreg16(REG5,REG7,DEFAULT);
Dec_lag3_L5_475:
    VoC_blt16_rd(Dec_lag3_L8_475,REG3,CONST_4_ADDR);    //      if (sub(index, 4) < 0)
    //      { i = sub(tmp_lag, 5);
    //         *T0 = add(i, index);
    //         *T0_frac = 0;
    //      }
    //      else
    VoC_sub16_rr(REG2,REG3,REG0,DEFAULT);       //         {
    VoC_multf16_rr(REG2,REG2,REG1,DEFAULT);     //  i = sub(mult(sub(index, 5), 10923), 1);
    VoC_lw16i_short(REG0,11,DEFAULT);
    VoC_bnlt16_rd(Dec_lag3_L8_475,REG3,CONST_12_ADDR);  //      {if (sub(index, 12) < 0)

//  VoC_NOP();
    VoC_sub16_rd(REG2,REG2,CONST_1_ADDR);
    VoC_add16_rr(REG6,REG2,REG5,DEFAULT);       //            *T0 = add(i, tmp_lag);
    VoC_mult16_rd(REG2,REG2,CONST_3_ADDR);      //            i = add(add(i, i), i);
    VoC_sub16_rd(REG7,REG3,CONST_9_ADDR);       //            *T0_frac = sub(sub(index, 9), i);
    VoC_sub16_rr(REG7,REG7,REG2,DEFAULT);
    VoC_jump(Dec_lag3_LEnd_475);                        //         }
//Dec_lag3_L7_475:                      //         else

Dec_lag3_L8_475:


    VoC_sub16_rr(REG5,REG5,REG0,DEFAULT);       //         {
    VoC_add16_rr(REG6,REG5,REG3,DEFAULT);       //          i = add( sub (index, 12), tmp_lag);
    VoC_lw16i_short(REG7,0,DEFAULT);        //            *T0 = add (i, 1);
Dec_lag3_LEnd_475:


///end  of CII_Dec_lag3_475

    VoC_sw16_d(REG6,STRUCT_DECOD_AMRSTATE_T0_LAGBUFF_ADDR);     //     st->T0_lagBuff = T0;

    VoC_bez16_d(Dec_amr_L20_475,DEC_AMR_BFI_ADDRESS);       //  if (bfi != 0)
    VoC_bnlt16_rd(Dec_amr_L20_475A,REG4,CONST_143_ADDR);    //  {      if (sub (st->old_T0, PIT_MAX) < 0)
    VoC_add16_rd(REG4,REG4,CONST_1_ADDR);           //  {   st->old_T0 = add(st->old_T0, 1);
Dec_amr_L20_475A:                       //  }
    VoC_lw16i_short(REG7,0,DEFAULT );           //     T0 = st->old_T0;
    VoC_movreg16(REG6,REG4,DEFAULT);            //     T0_frac = 0;
    VoC_sw16_d(REG4,STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR);

    VoC_lw16i_short(REG1,4,DEFAULT);    //       if ( st->inBackgroundNoise != 0 &&  sub(st->voicedHangover, 4) > 0 &&
    VoC_bez16_d(Dec_amr_L20_475,STRUCT_DECOD_AMRSTATE_INBACKGROUNDNOISE_ADDR);
    VoC_bnlt16_rd(Dec_amr_L20_475,REG1,STRUCT_DECOD_AMRSTATE_VOICEDHANGOVER_ADDR);
    //          ((sub(mode, MR475) == 0 ) ||(sub(mode, MR515) == 0 ) ||(sub(mode, MR59) == 0) ) )
    VoC_lw16_d(REG6,STRUCT_DECOD_AMRSTATE_T0_LAGBUFF_ADDR);//     {T0 = st->T0_lagBuff;}
Dec_amr_L20_475:                        //  }
    VoC_lw16i_short(REG2,1,DEFAULT);                //  Pred_lt_3or6 (st->exc, T0, T0_frac, L_SUBFR, 1);
    // end of dec_amr_not122_s1_475

    VoC_sw32_d(REG67,DEC_AMR_T0_ADDRESS);       // restore T0 &T0_frac
    VoC_lw16i(REG5,STRUCT_DECOD_AMRSTATE_EXC_ADDR);
    VoC_jal(CII_Pred_lt_3or6);

    VoC_lw16i_set_inc(REG0,DEC_AMR_CODE_ADDRESS,1);
    VoC_lw16_sd(REG2,2,DEFAULT);            // parm in reg2
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_push16(REG0,DEFAULT);           // code push16 stack[n-4]

    VoC_lw16inc_p(REG1,REG2,DEFAULT);       // restore parm
    VoC_lw16inc_p(REG6,REG2,DEFAULT);       // {   /* MR475, MR515 */
    VoC_sw16_sd(REG2,3,DEFAULT);            //    index = *parm++;

    VoC_lw16_d(REG7,DEC_AMR_SUBFRNR_ADDRESS);       //    i = *parm++;
//  VoC_jal(CII_decode_2i40_9bits_475);         //    decode_2i40_9bits (subfrNr, i, index, code);


////begin of    CII_decode_2i40_9bits_475

    VoC_movreg16(REG3,REG1,DEFAULT);       //j = shr((index & 64),6);
    VoC_lw16i_short(REG2,8,DEFAULT);
    VoC_shr16_ri(REG3,3,IN_PARALLEL);              // REG5->j
    VoC_and16_rr(REG3,REG2,DEFAULT);              //k = startPos[add(shl(j, 3), shl(subNr, 1))];
    VoC_shr16_ri(REG7,-1,IN_PARALLEL);
    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);

    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_lw16i(REG7,STATIC_CONST_startPos_ADDR);
    VoC_lw16i_set_inc(REG2,DEC_AMR_D2PF_POS_ADDRESS,1);
    VoC_loop_i_short(2,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16i_short(REG4,7,DEFAULT);
    VoC_and16_rr(REG4,REG1,DEFAULT);              //i = index & 7;     REG4->i
    VoC_lw16i_short(REG5,5,IN_PARALLEL);          //i = add(i, shl(i, 2));
    VoC_mult16_rr(REG4,REG4,REG5,DEFAULT);        //pos[0] = add(i, k);
    VoC_NOP();
    VoC_add16inc_rp(REG4,REG4,REG3,DEFAULT);
    VoC_shr16_ri(REG1,3,DEFAULT);                 //index = shr(index, 3);
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_endloop0
    //i = index & 7;       REG1 and REG4 free use here
    //k = startPos[add(add(shl(j, 3), shl(subNr, 1)), 1)];
    //i = add(i, shl(i, 2));
    //pos[1] = add(i, k);


    VoC_lw16i_short(REG1,2,DEFAULT);

    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16_sd(REG0,0,IN_PARALLEL);
    VoC_loop_i_short(40,DEFAULT);             //for (i = 0; i < L_SUBFR; i++) {
    VoC_sub16_rr(REG2,REG2,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_sw16inc_p(REG3,REG0,DEFAULT);         //    cod[i] = 0;
    VoC_endloop0                                  //}

    VoC_loop_r_short(REG1,DEFAULT);               //for (j = 0; j < NB_PULSE; j++) {
    VoC_lw16_sd(REG0,0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16i_short(REG1,1,DEFAULT);          //    i = sign & 1;
    VoC_and16_rr(REG1,REG6,DEFAULT);
    VoC_add16inc_rp(REG3,REG0,REG2,IN_PARALLEL);
    VoC_lw16i(REG4,-8192);
    VoC_bez16_r(d_pf_sub_L11_475,REG1);           //    if (i != 0) {
    VoC_lw16i(REG4,8191);                     // cod[pos[j]] = 8191;
d_pf_sub_L11_475:                                     //    } else { cod[pos[j]] = -8192;
    VoC_shr16_ri(REG6,1,DEFAULT);             //    sign = shr(sign, 1);
    VoC_sw16_p(REG4,REG3,DEFAULT);
    VoC_endloop0                  //    }  }

/////end of  CII_decode_2i40_9bits_475


    //    pit_sharp = shl (st->sharp, 1);  }
    VoC_lw16_d(REG5,STRUCT_DECOD_AMRSTATE_SHARP_ADDR);
    VoC_jal(CII_amr_dec_com_sub1);

    // begin of  dec_amr_not795122_s1_475               // M475 ,515 ,59 ,67 ,74 ,102
    //if (sub (mode, MR475) == 0)
    VoC_sub16_rd(REG2,REG2,CONST_1_ADDR);

    VoC_bez16_d(Dec_amr_L34_475,DEC_AMR_EVENSUBFR_ADDRESS); //{if (evenSubfr != 0)
    VoC_sw16_d(REG6,DEC_AMR_INDEX_MR475_ADDRESS);       //   { index_mr475 = *parm++;
    VoC_jump(Dec_amr_L33_475);
Dec_amr_L34_475:                            //

    VoC_sw16_sd(REG2,2,DEFAULT);        //   }
    VoC_lw16_d(REG6,DEC_AMR_INDEX_MR475_ADDRESS);

Dec_amr_L33_475:


    VoC_lw16d_set_inc(REG1,STRUCT_DECOD_AMRSTATE_STATE_ADDR,1);
    VoC_bez16_d(Dec_amr_L35_475A,DEC_AMR_BFI_ADDRESS);      //   if (bfi == 0)
    VoC_jump(Dec_amr_L35_475);
Dec_amr_L35_475A:

    //      Dec_gain(st->pred_state, mode, index_mr475, code,evenSubfr, &gain_pit, &gain_code);
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /*************************************************************************
     *   FUNCTION:  CII_Dec_gain()
     * Input: gc_predState *pred_state,  ->
              enum Mode mode,            ->REG4
              Word16 index,              ->REG6
              Word16 code[],             ->REG1
              Word16 evenSubfr,          DEC_AMR_EVENSUBFR_ADDRESS //(direct address access, needn't pass the address as a parameter)
              Word16 * gain_pit,         DEC_AMR_GAIN_PIT_ADDRESS  //(direct address access, needn't pass the address as a parameter)
              Word16 * gain_cod          DEC_AMR_GAIN_CODE_ADDRESS //(direct address access, needn't pass the address as a parameter)
     * Version 1.0  Created by Kenneth  07/15/2004
       Version 1.1  Optimized by Kenneth 08/13/2004
     ************************************************************************/
// begin of dec_gain
    VoC_lw16i_short(REG2,1,DEFAULT);    //    if (sub (mode, MR475) == 0)
    VoC_lw16i_set_inc(REG1,DEC_AMR_CODE_ADDRESS,1);
    VoC_sub16_rd(REG2,REG2,DEC_AMR_EVENSUBFR_ADDRESS);
    VoC_push16(REG1,DEFAULT);
    VoC_shr16_ri(REG6,-2,IN_PARALLEL);          //index = shl (index, 2);
    //        p = &table_gain_lowrates[index];
    // begin of dec_gain_475_s1         // M475
    //   {      index = add (index, shl(sub(1, evenSubfr), 1));
    VoC_lw16i_set_inc(REG1,STATIC_CONST_table_gain_MR475_ADDR,2);
    VoC_shr16_ri(REG2,-1,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG2,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG6,DEFAULT);           //        p = &table_gain_MR475[index];
    VoC_lw16i_short(ACC0_HI,0,DEFAULT);                      //        *gain_pit = *p++;

    VoC_lw32inc_p(ACC1, REG1,DEFAULT);              //        g_code = *p++;

    VoC_movreg16(ACC0_LO,ACC1_HI,DEFAULT);          //        Log2 (L_deposit_l (g_code), &exp, &frac);

    VoC_sw16_d(ACC1_LO,DEC_AMR_GAIN_PIT_ADDRESS);
    VoC_push16(ACC1_HI,DEFAULT);            // push16 stack[n-1]
    VoC_jal(CII_Log2);



    VoC_sub16_rd(REG7,REG4,CONST_12_ADDR);      //        exp = sub(exp, 12);

    VoC_lw16i(REG3,24660);
    VoC_lw16i_short(REG2,1,DEFAULT);                //        L_tmp = Mpy_32_16(exp, frac, 24660);
    VoC_multf32_rr(ACC0,REG7,REG3,DEFAULT);     //    **L_32 = L_mult (hi, n);
    VoC_multf16_rr(REG0,REG3,REG5,IN_PARALLEL);     //    **L_32 = L_mac (L_32, mult (lo, n), 1);
    VoC_NOP();
    VoC_mac32_rr(REG0,REG2,DEFAULT);
    VoC_lw32_sd(RL6,0,DEFAULT);             // CONST_0x00008000_ADDR
    VoC_shr32_ri(ACC0,-13,DEFAULT);         //        qua_ener = round (L_shl (L_tmp, 13));
    VoC_add32_rr(REG23,ACC0,RL6,DEFAULT);

    VoC_movreg16(REG2,REG3,DEFAULT);
    //        qua_ener_MR122 = add (shr_r (frac, 5), shl (exp, 10));


    VoC_shr32_ri(REG45,5,DEFAULT);
    VoC_shr16_ri(REG7,-10,IN_PARALLEL);

    VoC_add32_rd(REG45,REG45,CONST_0x00008000_ADDR);


    VoC_add16_rr(REG3,REG7,REG5,DEFAULT);

    // end of dec_gain_475_s1
    //}

    VoC_lw16i_set_inc(REG1,STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR,1);
    VoC_push32(REG23,DEFAULT);              // push32 stack[n]


    VoC_lw16i_short(REG7,0,DEFAULT);        // mode
    VoC_jal(CII_gc_pred);               //gc_pred(pred_state, mode, code, &exp, &frac, NULL, NULL);


    VoC_movreg16(REG0,REG6,DEFAULT);            //gcode0 = extract_l(Pow2(14, frac));
    VoC_lw16i_short(REG1,14,IN_PARALLEL);
    VoC_jal(CII_Pow2);                  //  REG2->gcode0

    VoC_pop16(REG3,DEFAULT);                //L_tmp = L_mult(g_code, gcode0);

    VoC_multf32_rr(REG23,REG3,REG2,DEFAULT);
//  VoC_NOP();
    VoC_sub16_dr(REG7,CONST_10_ADDR,REG7);      //L_tmp = L_shr(L_tmp, sub(10, exp));
    VoC_shr32_rr(REG23,REG7,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);
    VoC_lw16i(REG5,STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR);
    VoC_sw16_d(REG3,DEC_AMR_GAIN_CODE_ADDRESS);         //*gain_cod = extract_h(L_tmp);

    VoC_jal(CII_gc_pred_update);                //gc_pred_update(pred_state, qua_ener_MR122, qua_ener);

    VoC_lw16i_short(INC1,1,DEFAULT);
//  end of dec_gain
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    VoC_pop16(REG1,IN_PARALLEL);
    VoC_jump(Dec_amr_L36_475);                  //   }
Dec_amr_L35_475:                            //   else




    VoC_push16(REG1,DEFAULT);
    VoC_lw16i_set_inc(REG0,STRUCT_EC_GAIN_PITCHSTATE_ADDR,1);   //   {


    VoC_jal(CII_ec_gain_pitch);             //      ec_gain_pitch (st->ec_gain_p_st, st->state, &gain_pit);
    VoC_pop16(REG2,DEFAULT);

    VoC_lw16i_set_inc(REG0,STRUCT_EC_GAIN_CODESTATE_ADDR,1);
    VoC_lw16i_set_inc(REG1,STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR,1);
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG1,DEFAULT);
    VoC_push16(REG2,DEFAULT);
    VoC_jal(CII_ec_gain_code);              //      ec_gain_code (st->ec_gain_c_st, st->pred_state, st->state,&gain_code);

    VoC_loop_i_short(3,DEFAULT)
    VoC_startloop0
    VoC_pop16(REG0,DEFAULT);
    VoC_endloop0


Dec_amr_L36_475:


    VoC_lw16i_set_inc(REG1,STRUCT_EC_GAIN_CODESTATE_ADDR,1);
    VoC_lw16i_set_inc(REG0,STRUCT_EC_GAIN_PITCHSTATE_ADDR,1);
    VoC_push16(REG1,DEFAULT);
    VoC_push16(REG0,DEFAULT);
    VoC_lw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);
    VoC_jal(CII_ec_gain_pitch_update);          //   ec_gain_pitch_update (st->ec_gain_p_st, bfi, st->prev_bf,&gain_pit);
    VoC_pop16(REG0,DEFAULT);
    VoC_lw16_d(REG6,DEC_AMR_GAIN_CODE_ADDRESS);
    VoC_jal(CII_ec_gain_code_update);           //   ec_gain_code_update (st->ec_gain_c_st, bfi, st->prev_bf,&gain_code);
    VoC_pop16(REG1,DEFAULT);
    VoC_lw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);      //   pit_sharp = gain_pit;

    VoC_movreg16(REG2,REG6,DEFAULT);


    VoC_lw16i(REG5,13017);
    VoC_bngt16_rr(Dec_amr_L37_475,REG6,REG5);           //   if (sub (pit_sharp, SHARPMAX) > 0)
    VoC_movreg16(REG6,REG5,DEFAULT);            //   {       pit_sharp = SHARPMAX;    }
Dec_amr_L37_475:                        //}


    // end of  dec_amr_not795122_s1_475

    VoC_shr16_ri(REG6,-1,DEFAULT );             //pit_sharp = shl (pit_sharp, 1);
    // gain_pit in reg2

    VoC_bngt16_rd(Dec_amr_L52_475,REG6,CONST_0x4000_ADDR);  //if (sub (pit_sharp, 16384) > 0)

    VoC_lw16i_set_inc(REG0,STRUCT_DECOD_AMRSTATE_EXC_ADDR,1);   //{
    VoC_lw16i_set_inc(REG1,DEC_AMR_EXCP_ADDRESS,1);

    VoC_multf16inc_rp(REG5,REG6,REG0,DEFAULT);      //       temp = mult (st->exc[i], pit_sharp);
    VoC_loop_i_short(40,DEFAULT);                   //   for (i = 0; i < L_SUBFR; i++)
    VoC_startloop0                      //    {
    VoC_lw32_d(ACC0,CONST_0x00008000_ADDR);  //       excp[i] = round (L_temp);
    VoC_mac32_rr(REG5,REG2,DEFAULT);        //       L_temp = L_mult (temp, gain_pit);
    VoC_NOP();

    VoC_multf16inc_rp(REG5,REG6,REG0,DEFAULT);      //       temp = mult (st->exc[i], pit_sharp);
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    VoC_endloop0                            //    }

Dec_amr_L52_475:                                //}
    VoC_lw16i_set_inc(REG0,13017,1);
    VoC_sw16_d(REG6,DEC_AMR_PIT_SHARP_ADDRESS);


    VoC_bnez16_d(Dec_amr_L55_475,DEC_AMR_EVENSUBFR_ADDRESS); //if (sub(mode, MR475) != 0 || evenSubfr == 0)  {
    VoC_movreg16(REG7,REG2,DEFAULT);        //    st->sharp = gain_pit;
    VoC_bngt16_rr(Dec_amr_L54_475,REG7,REG0);           //    if (sub (st->sharp, SHARPMAX) > 0)
    VoC_movreg16(REG7,REG0,DEFAULT);        //    {  st->sharp = SHARPMAX;
    VoC_NOP();
Dec_amr_L54_475:
    // }
    VoC_sw16_d(REG7,STRUCT_DECOD_AMRSTATE_SHARP_ADDR);  //}
Dec_amr_L55_475:
    VoC_bnez16_d(Dec_amr_L56_475,DEC_AMR_BFI_ADDRESS);      //if ( bfi == 0 )
    VoC_lw16i_set_inc(REG3,STRUCT_DECOD_AMRSTATE_LTPGAINHISTORY_ADDR,1);//{
    VoC_add16_rd(REG0,REG3,CONST_1_ADDR);
    VoC_lw16i_short(REG6,8,DEFAULT);                   //   for (i = 0; i < 8; i++)
    VoC_jal(CII_R02R3_p_R6);                //   {   st->ltpGainHistory[i] = st->ltpGainHistory[i+1]; }
    VoC_sw16_p(REG2,REG3,DEFAULT);          //   st->ltpGainHistory[8] = gain_pit;
    //}
Dec_amr_L56_475:
    // if ( (st->prev_bf != 0 || bfi != 0)


    VoC_bez16_d(Dec_amr_L60_475,STRUCT_DECOD_AMRSTATE_INBACKGROUNDNOISE_ADDR);// && st->inBackgroundNoise != 0 &&
    //     ((sub(mode, MR475) == 0) ||(sub(mode, MR515) == 0) ||(sub(mode, MR59) == 0)) )
    VoC_bnez16_d(Dec_amr_L57_475,DEC_AMR_BFI_ADDRESS);
    VoC_bez16_d(Dec_amr_L60_475,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);
Dec_amr_L57_475:                            //{

    VoC_bngt16_rd(Dec_amr_L58_475,REG2,(8+STATIC_CONST_INTERP_FACTOR_ADDR));        //   if ( sub (gain_pit, 12288) > 0)
    //      gain_pit = add( shr( sub(gain_pit, 12288), 1 ), 12288 );

    VoC_add16_rd(REG2,REG2,(8+STATIC_CONST_INTERP_FACTOR_ADDR));
    VoC_shr16_ri(REG2,1,DEFAULT);

Dec_amr_L58_475:


    VoC_lw16i(REG0,14745);
    VoC_bngt16_rr(Dec_amr_L60_475,REG2,REG0);       //   if ( sub (gain_pit, 14745) > 0)
    VoC_movreg16(REG2,REG0,DEFAULT);        //   {  gain_pit = 14745;
Dec_amr_L60_475:                            //   }
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG0,DEFAULT);
    VoC_sw16_d(REG2,DEC_AMR_GAIN_PIT_ADDRESS);          //}

    VoC_jal(CII_Int_lsf);               //Int_lsf(prev_lsf, st->lsfState->past_lsf_q, i_subfr, lsf_i);
    VoC_lw16_sd(ACC1_HI,0,DEFAULT);         //bgMix = 8192; cbGainMix = gain_code;

    VoC_lw32_d(REG67,STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR);
    VoC_add32_rd(REG67,REG67,CONST_0x00010001_ADDR);

    VoC_bgt16_rd(CGAver_L5_475,REG5,CONST_5325_ADDR);       //if (sub(diff, 5325) > 0)
    VoC_lw16i_short(REG7,0,DEFAULT);            //{ st->hangVar = add(st->hangVar, 1);}
CGAver_L5_475:                      // else{st->hangVar = 0; }


    VoC_lw32_sd(REG23,0,DEFAULT);
    VoC_bngt16_rd(CGAver_L6_475,REG7,CONST_10_ADDR);    //if (sub(st->hangVar, 10) > 0)
    VoC_lw16i_short(REG6,1,DEFAULT);            //{  st->hangCount = 0;
CGAver_L6_475:




    VoC_lw16i(REG4,3277);
    VoC_bez16_r(CGAver_L8_475,REG2);                //if ((((pdfi != 0) && (prev_pdf != 0)) || (bfi != 0) || (prev_bf != 0)) &&
    VoC_bngt16_rd(CGAver_L8_475,REG3,CONST_1_ADDR);                 //   (sub(voicedHangover, 1) > 0) && (inBackgroundNoise != 0) &&
    VoC_bnez16_d(CGAver_L9_475,DEC_AMR_BFI_ADDRESS);                //   ((sub(mode, MR475) == 0) ||(sub(mode, MR515) == 0) ||  (sub(mode, MR59) == 0)) )
    VoC_bnez16_d(CGAver_L9_475,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);
    VoC_bez16_d(CGAver_L8_475,DEC_AMR_PDFI_ADDRESS);
    VoC_bez16_d(CGAver_L8_475,STRUCT_DECOD_AMRSTATE_PREV_PDF_ADDR); //{       tmp_diff = sub(diff, 4506);
CGAver_L9_475:                                  //tmp1->reg3
    VoC_lw16i(REG4,4506);                                           //   if (tmp_diff > 0)
    //   { tmp1 = tmp_diff;    }
CGAver_L8_475:                                                      //   else{ tmp1 = 0; }

    //   else{ tmp1 = 0; }
    //  begin of dec_amr_not74795122_s1_475
    //   if (sub(2048, tmp1) < 0)
    VoC_sub16_rr(REG3,REG5,REG4,DEFAULT);               //   { bgMix = 8192; }
    VoC_bgtz16_r(CGAver_L11_475,REG3);              //   else   {bgMix = shl(tmp1, 2);             } }
    VoC_lw16i_short(REG3,0,DEFAULT);            //else {
CGAver_L11_475:                                 //   tmp_diff = sub(diff, 3277);
    VoC_shr16_ri(REG3,-2,DEFAULT);                          //   if (tmp_diff > 0)

    VoC_lw32_sd(ACC0,1,DEFAULT);                // should check the value if equal 0x00008000

    VoC_bgt16_rd(CGAver_L17_475,REG3,CONST_0x2000_ADDR);    //   {tmp1 = tmp_diff;  }
    VoC_blt16_rd(CGAver_L17_475,REG6,CONST_41_ADDR);
    VoC_bngt16_rd(CGAver_L10_475,REG5,CONST_5325_ADDR);
CGAver_L17_475:
    VoC_lw16i(REG3,8192);                   //   else {tmp1 = 0; }
CGAver_L10_475:                                         //   if (sub(2048, tmp1) < 0)
    //   {bgMix = 8192;  }
    //   else  {bgMix = shl(tmp1, 2); }  }
    //if ((sub(st->hangCount, 40) < 0) || (sub(diff, 5325) > 0))
    //  { bgMix = 8192;  }

    VoC_lw16i_short(REG5,7,DEFAULT);
    VoC_lw16_sd(REG2,1,DEFAULT);                //cbGainMean = round(L_sum);
    VoC_lw32_sd(REG01,0,IN_PARALLEL);           //if (((bfi != 0) || (prev_bf != 0)) && (inBackgroundNoise != 0) &&
    VoC_lw16i(REG4,4681);

    VoC_bez16_r(CGAver_L19_475,REG0);               //    ((sub(mode, MR475) == 0) ||(sub(mode, MR515) == 0) ||(sub(mode, MR59) == 0)) )
    VoC_bnez16_d(CGAver_L20_475,DEC_AMR_BFI_ADDRESS);   //{   L_sum = L_mult(4681, st->cbGainHistory[0]);
    VoC_bnez16_d(CGAver_L20_475,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);    //   for (i = 1; i < L_CBGAINHIST; i++)
CGAver_L19_475:                                             //   {
    VoC_lw16i_short(REG5,5,DEFAULT);            //      L_sum = L_mac(L_sum, 4681, st->cbGainHistory[i]);}
    VoC_lw16i(REG4,6554);                               //   cbGainMean = round(L_sum);
    VoC_add16_rd(REG2,REG2,CONST_2_ADDR);           //}
CGAver_L20_475:
    VoC_loop_r_short(REG5,DEFAULT);             //L_sum = L_mult(6554, st->cbGainHistory[2]);
    VoC_startloop0                      //for (i = 3; i < L_CBGAINHIST; i++)
    VoC_mac32inc_rp(REG4,REG2,DEFAULT);         //{ L_sum = L_mac(L_sum, 6554, st->cbGainHistory[i]);}
    VoC_endloop0

    VoC_pop16(REG5,DEFAULT);                //  cbGainMix=gain_code in REG5
    VoC_lw32_sd(ACC1,1,IN_PARALLEL);            // should check the value if equal 0x00008000
    VoC_movreg16(REG1,ACC0_HI,DEFAULT);
    VoC_multf32_rr(ACC0,REG3,REG5,IN_PARALLEL);     //   L_sum = L_mult(bgMix, cbGainMix);
    VoC_mac32_rd(REG1,CONST_0x2000_ADDR);           //   L_sum = L_mac(L_sum, 8192, cbGainMean);
    VoC_msu32_rr(REG3,REG1,DEFAULT);            //   L_sum = L_msu(L_sum, bgMix,cbGainMean);

    //st->hangCount = add(st->hangCount, 1);
    VoC_sw32_d(REG67,STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR);
    VoC_shr32_ri(ACC0,-2,DEFAULT);              //   cbGainMix = round(L_shl(L_sum, 2));
    VoC_pop16(REG7,IN_PARALLEL);
    VoC_add32_rr(ACC1,ACC0,ACC1,DEFAULT);   //}

    //  end of dec_amr_not74795122_s1_475

    //return cbGainMix;  ->ACC1_HI

    VoC_lw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);
    VoC_sw16_d(ACC1_HI,DEC_AMR_GAIN_CODE_MIX_ADDRESS);

    VoC_lw16i_short(REG7,1,DEFAULT);        //   tmp_shift = 1;

    VoC_jal(CII_amr_dec_com_sub3);

    VoC_blt16_rd(Phdisp_L16_475_no,REG4,CONST_2_ADDR);      //   sub(mode, MR74) != 0 && sub(impNr, 2) < 0)
    VoC_jump(Phdisp_L16_475);
Phdisp_L16_475_no:

    // begin of dec_Phdisp_not74102122_s1_475
    VoC_lw16i_set_inc(REG1,DEC_AMR_BUFA40_ADDRESS,1);   //{   nze = 0;
    // inno[] in REG2
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i_short(REG0,40,IN_PARALLEL);
    VoC_add16_rr(REG0,REG1,REG0,DEFAULT);

    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_lw32_d(ACC0,CONST_1_ADDR);
    VoC_loop_i_short(40,DEFAULT);           //    for (i = 0; i < L_SUBFR; i++)
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_startloop0                  //    {
    VoC_lw16_p(REG6,REG2,DEFAULT);          //        if (inno[i] != 0)
    VoC_bez16_r(Phdisp_L12_475,REG6);
    VoC_sw16inc_p(REG3,REG1,DEFAULT);       //        { ps_poss[nze] = i;
    VoC_add32_rr(ACC1,ACC1,ACC0,IN_PARALLEL);       //            nze = add (nze, 1);}
Phdisp_L12_475:
    VoC_sw16inc_p(REG6,REG0,DEFAULT);       //        inno_sav[i] = inno[i];
    VoC_inc_p(REG3,DEFAULT);
    VoC_sw16inc_p(ACC1_HI,REG2,DEFAULT);        //        inno[i] = 0;
    VoC_endloop0                        //    }

    VoC_lw16i_set_inc(REG3,STATIC_CONST_ph_imp_mid_ADDR,1);
    VoC_bnez16_r(Phdisp_L14_475,REG4);          //        if (impNr == 0)
    VoC_lw16i_set_inc(REG3,STATIC_CONST_ph_imp_low_ADDR,1); //        { ph_imp = ph_imp_low;    move16 ();
Phdisp_L14_475:                     //        }
    //        else      { ph_imp = ph_imp_mid;    move16 ();
    //        }
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

    VoC_bez16_r(Phdisp_L20_475,REG6);
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
Phdisp_L20_475:
    VoC_movreg32(REG23,ACC0,DEFAULT);
    VoC_endloop1                        // }
    // end of dec_Phdisp_not74102122_s1_475

Phdisp_L16_475:
    VoC_jal(CII_amr_dec_com_sub4);

    // begin of dec_amr_47551559_s1_475
    VoC_bez16_r(Dec_amr_L66S1c_475,REG6);               //&& sub(st->voicedHangover, 5) > 0 && st->inBackgroundNoise != 0 && sub(st->state, 4) < 0 &&
    VoC_bngt16_rd(Dec_amr_L66S1c_475,REG7,CONST_5_ADDR)     //     ( (pdfi != 0 && st->prev_pdf != 0) ||bfi != 0 ||st->prev_bf != 0) )
//Dec_amr_L66S1a_475:
//  VoC_jump(Dec_amr_L66S1_475);


//Dec_amr_L66S1b_475:


    VoC_lw16i_short(REG6,4,DEFAULT);
    VoC_bngt16_rd(Dec_amr_L66S1c_475,REG6,STRUCT_DECOD_AMRSTATE_STATE_ADDR);
    VoC_bnez16_d(Dec_amr_L65_475,DEC_AMR_BFI_ADDRESS);
    VoC_bnez16_d(Dec_amr_L65_475,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);
    VoC_bez16_d(Dec_amr_L66S1c_475,DEC_AMR_PDFI_ADDRESS);
    VoC_bnez16_d(Dec_amr_L65_475,STRUCT_DECOD_AMRSTATE_PREV_PDF_ADDR);
Dec_amr_L66S1c_475:
    VoC_jump(Dec_amr_L66S1_475);
Dec_amr_L65_475:                            //{
    VoC_lw16i_short(REG7,0,DEFAULT);        //   carefulFlag = 0;
    VoC_bez16_d(Dec_amr_L67_475,DEC_AMR_PDFI_ADDRESS);      //   if ( pdfi != 0 && bfi == 0 )
    VoC_bnez16_d(Dec_amr_L67_475,DEC_AMR_BFI_ADDRESS);      //   {
    VoC_lw16i_short(REG7,1,DEFAULT);            //      carefulFlag = 1;     }
Dec_amr_L67_475:
    VoC_lw16i_set_inc(REG0,STRUCT_DECOD_AMRSTATE_EXCENERGYHIST_ADDR,1);
    VoC_push16(REG5,DEFAULT);       //excEnergy
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG7,DEFAULT);
    //   Ex_ctrl(exc_enhanced,excEnergy, st->excEnergyHist, st->voicedHangover,st->prev_bf,carefulFlag);
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /***************************************************************************
       Function:void CII_Ex_ctrl (void)
       Input:  excitation[],        REG2(incr 1)         stack[n+4] //have not pop yet
               excEnergy,           REG5         push16  stack[n+3]
               exEnergyHist[],      REG0(incr 1) push16  stack[n+2]
               voicedHangover       REG3->    push32  stack_32[n]
               prevBFI,             STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR //
               carefulFlag                       push16  stack[n+1]
       Version 1.0 Created by Kenneth       07/14/2004
       Version 1.1 revised by Kenneth               08/14/2004
    ****************************************************************************/
//  begin of ex_ctrl
    VoC_lw16i_short(REG5,9,IN_PARALLEL);            //avgEnergy = gmed_n(exEnergyHist, 9);
    VoC_jal(CII_gmed_n);                    //  REG1->avgEnergy

    VoC_lw16_d(REG7,STRUCT_DECOD_AMRSTATE_EXCENERGYHIST_ADDR+8);    //  REG7->exEnergyHist[8]
    VoC_add16_rd(REG6,REG7,STRUCT_DECOD_AMRSTATE_EXCENERGYHIST_ADDR+7); //prevEnergy = shr( add (exEnergyHist[7], exEnergyHist[8]) ,1);
    VoC_shr16_ri(REG6,1,DEFAULT);           //  REG6->prevEnergy
    VoC_bngt16_rr(Exctrl_Lab1_475,REG6,REG7);           //if ( sub (exEnergyHist[8], prevEnergy) < 0)
    VoC_movreg16(REG6,REG7,DEFAULT);            //{  prevEnergy = exEnergyHist[8];}
Exctrl_Lab1_475:
    VoC_lw16_sd(REG5,2,DEFAULT);
    VoC_bngt16_rr(Exctrl_LaEnd_475_go,REG1,REG5);       //if ( sub (excEnergy, avgEnergy) < 0 && sub (excEnergy, 5) > 0)
    VoC_bgt16_rd(Exctrl_LaEnd_475_no,REG5,CONST_5_ADDR);
Exctrl_LaEnd_475_go:
    VoC_jump(Exctrl_LaEnd_475);
Exctrl_LaEnd_475_no:

    VoC_movreg16(REG4,REG6,DEFAULT);            //{  testEnergy = shl(prevEnergy, 2);
    VoC_lw32_sd(REG23,0,IN_PARALLEL);
    VoC_shr16_ri(REG4,-2,DEFAULT);
    VoC_movreg16(REG7,REG1,IN_PARALLEL);            //  reg7->avgEnergy
    VoC_blt16_rd(Exctrl_Lab2_475,REG3,CONST_7_ADDR);        //   if ( sub (voicedHangover, 7) < 0 || prevBFI != 0 )
    VoC_bez16_d(Exctrl_Lab3_475,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);
Exctrl_Lab2_475:                        //   {
    VoC_sub16_rr(REG4,REG4,REG6,DEFAULT);       // testEnergy = sub (testEnergy, prevEnergy);   }
Exctrl_Lab3_475:
    VoC_movreg16(ACC0_HI,REG5,DEFAULT);         //   exp = norm_s (excEnergy);
    VoC_bngt16_rr(Exctrl_Lab4_475,REG7,REG4);       //   if ( sub (avgEnergy, testEnergy) > 0)
    VoC_movreg16(REG7,REG4,DEFAULT);            //   { avgEnergy = testEnergy;        }
Exctrl_Lab4_475:


    VoC_lw16i_short(ACC0_LO,0,DEFAULT);
    VoC_jal(CII_NORM_L_ACC0);               //   excEnergy = shl (excEnergy, exp);
    VoC_movreg16(REG1,ACC0_HI,DEFAULT);
    VoC_movreg16(REG5,REG1,IN_PARALLEL);
    VoC_lw16i(REG0,16383);
    VoC_jal(CII_DIV_S);                     //   excEnergy = div_s ((Word16) 16383, excEnergy);
    VoC_multf32_rr(REG67,REG7,REG2,DEFAULT);        //   t0 = L_mult (avgEnergy, excEnergy);
    VoC_sub16_dr(REG5,CONST_20_ADDR,REG5);      //   t0 = L_shr (t0, sub (20, exp));
    VoC_shr32_rr(REG67,REG5,DEFAULT);
    VoC_bngt32_rd(Exctrl_Lab5_475,REG67,CONST_0x00007FFF_ADDR);//   if ( L_sub(t0, 32767) > 0 )
    VoC_lw32_d(REG67,CONST_0x00007FFF_ADDR);        //   { t0 = 32767;  }
Exctrl_Lab5_475:
    VoC_lw16_sd(REG7,0,DEFAULT);                //   scaleFactor = extract_l (t0);REG6->scaleFactor

    VoC_lw16i(REG4,3072);               //   if ( carefulFlag != 0 && sub(scaleFactor, 3072) > 0 )
    VoC_bez16_r(Exctrl_Lab6_475,REG7);
    VoC_bngt16_rr(Exctrl_Lab6_475,REG6,REG4);
    VoC_movreg16(REG6,REG4,DEFAULT);            //   {  scaleFactor = 3072;  }
Exctrl_Lab6_475:
    VoC_lw16_sd(REG2,3,DEFAULT);            // load excitation[]

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_movreg16(REG0,REG2,DEFAULT);

    VoC_lw16i_short(FORMAT32,10,DEFAULT);
    VoC_loop_i_short(40,DEFAULT);           //   for (i = 0; i < L_SUBFR; i++)
    VoC_multf32inc_rp(ACC0,REG6,REG0,IN_PARALLEL);
    VoC_startloop0                  //   {
    //      t0 = L_mult (scaleFactor, excitation[i]);
    //   VoC_shr32_ri(ACC0,11,DEFAULT);          //      t0 = L_shr (t0, 11);
    VoC_multf32inc_rp(ACC0,REG6,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(ACC0_LO,REG2,DEFAULT);        //      excitation[i] = extract_l (t0);
    exit_on_warnings=ON;
    VoC_endloop0                    //   } }


    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
Exctrl_LaEnd_475:
//  end of ex_ctrl
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    VoC_pop16(REG7,DEFAULT);
    VoC_pop16(REG0,DEFAULT);
    VoC_pop16(REG5,DEFAULT);                //}

Dec_amr_L66S1_475:
    // end of dec_amr_47551559_s1_475
    VoC_pop32(REG67,DEFAULT);       //"inBackgroundNoise&voicedHangover"

    VoC_jal(CII_amr_dec_com_sub5);
    VoC_jump(Dec_amr_LoopStart_475);    //  }
Dec_amr_LoopEnd_475:

    VoC_pop16(REG5,DEFAULT);        // pop16 stack[n-2]
    VoC_pop16(REG4,DEFAULT);        // pop16 stack[n-1]
    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_lw16_sd(REG3,1,DEFAULT);
    VoC_return;
}





// VoC_directive: FAKE_FUNCTION_ON
#ifdef voc_compile_only

void CII_amr_dec_515(void) {};
void CII_amr_dec_59(void) {};
void CII_amr_dec_67(void) {};
void CII_amr_dec_74(void) {};
void CII_amr_dec_795(void) {};
void CII_amr_dec_102(void) {};
void CII_amr_dec_122(void) {};
void CII_COD_AMR_515(void) {};
void CII_COD_AMR_59(void) {};
void CII_COD_AMR_67(void) {};
void CII_COD_AMR_74(void) {};
void CII_COD_AMR_795(void) {};
void CII_COD_AMR_102(void) {};
void CII_COD_AMR_122(void) {};
void CII_HR_Encode(void) {};
void CII_HR_Decode(void) {};
void CII_FR_Encode(void) {};
void CII_FR_Decode(void) {};


#endif

// VoC_directive: FAKE_FUNCTION_OFF
