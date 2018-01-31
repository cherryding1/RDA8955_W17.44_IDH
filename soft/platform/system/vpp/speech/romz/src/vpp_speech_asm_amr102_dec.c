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



void CII_amr_dec_102(void)
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
Dec_amr_LoopStart_102:
    VoC_bne16_rd(Dec_amr_LoopEnd_1021,REG5,CONST_160_ADDR);     //for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
    VoC_jump(Dec_amr_LoopEnd_102);
Dec_amr_LoopEnd_1021:
    VoC_lw16i_short(REG3,1,DEFAULT);        //{

    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_push16(REG5,DEFAULT);               //   i_subfr  push16 stack[n-3]
    VoC_add16_rd(REG6,REG3,DEC_AMR_SUBFRNR_ADDRESS);    //   subfrNr = add(subfrNr, 1);
    VoC_sub16_rd(REG4,REG3,DEC_AMR_EVENSUBFR_ADDRESS);  //evenSubfr = sub(1, evenSubfr);
    VoC_sw16_d(REG6,DEC_AMR_SUBFRNR_ADDRESS);
    VoC_sw16_d(REG4,DEC_AMR_EVENSUBFR_ADDRESS);     //   pit_flag = i_subfr;

    VoC_bne16_rd(Dec_amr_L14_102,REG5,CONST_80_ADDR);   //   if (sub (i_subfr, L_FRAME_BY2) == 0)
    //   { if (sub(mode, MR475) != 0 && sub(mode, MR515) != 0)
    VoC_lw16i_short(REG5,0,DEFAULT);            //      {  pit_flag = 0;  }
Dec_amr_L14_102:
    VoC_lw16_sd(REG0,2,DEFAULT);            // assure REG0 incr=1
    VoC_sw16_d(REG5,DEC_AMR_PIT_FLAG_ADDRESS);
    VoC_lw16inc_p(REG3,REG0,DEFAULT);           //index = *parm++;
    VoC_lw16_d(REG4,STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR);

    VoC_sw16_d(REG3,DEC_AMR_INDEX_ADDRESS);
    VoC_sw16_sd(REG0,2,DEFAULT);    // restore the new address of parm
    // begin of dec_amr_not122_s1_102

    VoC_lw16i_short(REG2,5,IN_PARALLEL);        //     delta_frc_low = 5;
    VoC_lw16i_short(REG1,9,DEFAULT);        //     delta_frc_range = 9;
    VoC_sub16_rr(REG6,REG4,REG2,IN_PARALLEL);           //     t0_min = sub(st->old_T0, delta_frc_low);
    VoC_bnlt16_rd(Dec_amr_L18_102,REG6,CONST_20_ADDR);      //     if (sub(t0_min, PIT_MIN) < 0)
    VoC_lw16i_short(REG6,20,DEFAULT);               //     {           t0_min = PIT_MIN;
Dec_amr_L18_102:                            //     }
    VoC_add16_rr(REG7,REG6,REG1,DEFAULT);           //     t0_max = add(t0_min, delta_frc_range);
    VoC_bngt16_rd(Dec_amr_L19_102,REG7,CONST_143_ADDR);     //     if (sub(t0_max, PIT_MAX) > 0)
    VoC_lw16i(REG7,143);                    //     {        t0_max = PIT_MAX;
    VoC_sub16_rr(REG6,REG7,REG1,DEFAULT);           //        t0_min = sub(t0_max, delta_frc_range);
Dec_amr_L19_102:                            //     }

    // begin of CII_Dec_lag3_102                //     Dec_lag3 (index, t0_min, t0_max, pit_flag, st->old_T0,&T0, &T0_frac, flag4);
    /*************************************
    Function  void CII_Dec_lag3_102(void)
    input: index      ->REG3  //unchanged in REG3
          flag4      ->REG5
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
    VoC_bnez16_d(Dec_lag3_L1_102,DEC_AMR_PIT_FLAG_ADDRESS); //if (i_subfr == 0) {
    VoC_sub16_rd(REG6,REG3,CONST_112_ADDR);     //      *T0 = sub(index, 112);
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_bnlt16_rd(Dec_lag3_L2_102,REG3,CONST_197_ADDR); //     if (sub(index, 197) < 0) {
    VoC_add16_rr(REG2,REG3,REG0,DEFAULT);       //      *T0 = add(mult(add(index, 2), 10923), 19);

    VoC_multf16_rr(REG6,REG2,REG1,DEFAULT); //      i = add(add(*T0, *T0), *T0);
    VoC_lw16i_short(REG5,19,DEFAULT);//gugogi
    VoC_add16_rr(REG6,REG6,REG5,DEFAULT);       //      *T0_frac = add(sub(index, i), 58);
    VoC_lw16i_short(REG0,-58,IN_PARALLEL);
    VoC_movreg16(REG2,REG6,DEFAULT);
    VoC_jump(Dec_lag3_LA_102);          //   } else {
    //      *T0_frac = 0;       }
Dec_lag3_L1_102:                        //} else {
    //if (flag4 == 0) {
    VoC_add16_rr(REG2,REG3,REG0,DEFAULT);       //      i = sub(mult(add(index, 2), 10923), 1);
    VoC_multf16_rr(REG2,REG2,REG1,DEFAULT);
    VoC_NOP();//change
    VoC_sub16_rd(REG2,REG2,CONST_1_ADDR);
    VoC_add16_rr(REG6,REG2,REG6,DEFAULT);       //      *T0 = add(i, t0_min);
Dec_lag3_LA_102:
    VoC_add16_rr(REG7,REG2,REG2,DEFAULT);       //      i = add(add(i, i), i);
    VoC_add16_rr(REG7,REG7,REG2,DEFAULT);
    VoC_sub16_rr(REG7,REG3,REG7,DEFAULT);       //      *T0_frac = sub(sub(index, 2), i);
    VoC_sub16_rr(REG7,REG7,REG0,DEFAULT);
Dec_lag3_L2_102:
    // end of CII_Dec_lag3_102
    VoC_sw16_d(REG6,STRUCT_DECOD_AMRSTATE_T0_LAGBUFF_ADDR);     //     st->T0_lagBuff = T0;

    VoC_bez16_d(Dec_amr_L20_102,DEC_AMR_BFI_ADDRESS);       //  if (bfi != 0)
    VoC_bnlt16_rd(Dec_amr_L20_102A,REG4,CONST_143_ADDR);    //  {      if (sub (st->old_T0, PIT_MAX) < 0)
    VoC_add16_rd(REG4,REG4,CONST_1_ADDR);           //  {   st->old_T0 = add(st->old_T0, 1);
Dec_amr_L20_102A:                       //  }
    VoC_lw16i_short(REG7,0,DEFAULT );           //     T0 = st->old_T0;
    VoC_movreg16(REG6,REG4,IN_PARALLEL);            //     T0_frac = 0;
    VoC_sw16_d(REG4,STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR);

Dec_amr_L20_102:                        //  }
    VoC_lw16i_short(REG2,1,DEFAULT);                //  Pred_lt_3or6 (st->exc, T0, T0_frac, L_SUBFR, 1);
    // end of dec_amr_not122_s1_102

    VoC_sw32_d(REG67,DEC_AMR_T0_ADDRESS);       // restore T0 &T0_frac
    VoC_lw16i(REG5,STRUCT_DECOD_AMRSTATE_EXC_ADDR);
    VoC_jal(CII_Pred_lt_3or6);

    VoC_lw16i_set_inc(REG0,DEC_AMR_CODE_ADDRESS,1);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16_sd(REG2,2,DEFAULT);            // parm in reg2
    VoC_push16(REG0,DEFAULT);           // code push16 stack[n-4]
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_add16_rd(REG4,REG2,CONST_7_ADDR);           // {  /* MR102 */
    VoC_movreg16(REG1,REG2,DEFAULT);            //    dec_8i40_31bits (parm, code);

    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_sw16_sd(REG4,3,DEFAULT);            //    parm += 7;
    //    pit_sharp = shl (st->sharp, 1);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    //  begin of CII_dec_8i40_31bits            // }
    /**************************************************************
    function void CII_dec_8i40_31bits(void)
    Input: Word16 index[],    i -> REG1(incr 1)
        Word16 cod[]       o -> REG0(incr 2)  push16 stack[n+1]

    Version 1.0 Created by Kenneth       07/02/2004
    Version 1.1 Optimized by Kenneth     08/10/2004
    **************************************************************/

    //for (i = 0; i < 40; i++)
    VoC_lw16i_set_inc(REG2,DEC_AMR_BUFA40_ADDRESS,1);
    VoC_push16(REG1,DEFAULT);          // push16 stack[n-1]    index[]
    VoC_loop_i_short(20,DEFAULT);          //{
    VoC_push16(REG2,IN_PARALLEL);          //push16 stack[n-2]  linear_codewords
    VoC_startloop0                 //    cod[i] = 0;
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);       //}
    VoC_endloop0
    //decompress_code (index, linear_signs, linear_codewords);
    //for (i = 0; i < 4; i++) { linear_signs[i] = index[i]; }
    // linear_codewords[7:0] stored in TEMP0~TEMP7
    /*****************************************************************
     Function: CII_decompress10
     Input: MSBs      ->REG4          LSBs      ->REG6
            index1    ->REG3          index2    ->ACC0_LO
            index3    ->ACC0_HI       pos_indx[]->REG2
    ******************************************************************/

    VoC_lw16_sd(REG2,1,DEFAULT);  ////MSBs = shr(index[4], 3);
    VoC_lw16i_short(REG0,4,IN_PARALLEL);        //LSBs = index[4] & 7;

    VoC_add16_rr(REG2,REG2,REG0,DEFAULT);
    VoC_lw16_sd(REG3,0,IN_PARALLEL);       //pos_indx

    VoC_lw16i_short(ACC0_LO,4,DEFAULT);
    VoC_lw16i_short(ACC0_HI,1,IN_PARALLEL);
    VoC_lw16i_short(INC3,2,DEFAULT);
    VoC_loop_i(0,2);

    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_lw16i_short(REG6,7,IN_PARALLEL);
    VoC_and16_rr(REG6,REG4,DEFAULT);
    VoC_shr16_ri(REG4,3,DEFAULT);

    VoC_bngt16_rd(decompress10_L1,REG4,CONST_124_ADDR);     //if (sub(MSBs, 124) > 0) //{
    VoC_lw16i(REG4,124);     //   MSBs = 124;
decompress10_L1:                //}


    VoC_multf16_rd(REG5,REG4,CONST_1311_ADDR);  //ia = mult(MSBs, 1311);        REG5
    VoC_NOP();//change
    VoC_multf32_rd(REG01,REG5,CONST_25_ADDR);   //ia = sub(MSBs, extract_l(L_shr(L_mult(ia, 25), 1)));
    VoC_NOP();//change
    VoC_shr32_ri(REG01,1,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG0,DEFAULT);        //REG4->ia

    //ib = shl(sub(ia, extract_l(L_shr(L_mult(mult(ia, 6554), 5), 1))), 1);
    VoC_multf16_rd(REG7,REG4,CONST_6554_ADDR);      //REG7->mult(ia, 6554)
    VoC_NOP();//change
    VoC_multf32_rd(REG01,REG7,CONST_5_ADDR);
    VoC_shr16_ri(REG7,-1,DEFAULT);//gugogi VoC_NOP();//change
    VoC_shr32_ri(REG01,1,DEFAULT);
    VoC_sub16_rr(REG0,REG4,REG0,DEFAULT);       //REG0->ib

    VoC_shr16_ri(REG0,-1,DEFAULT);
    VoC_lw16i_short(REG1,0x3,IN_PARALLEL);          //ic = shl(shr(LSBs, 2), 2);
    VoC_and16_rr(REG1,REG6,DEFAULT);             //ic = sub(LSBs, ic);
    //pos_indx[index1] = add(ib, (ic & 1));

    VoC_lw16i_short(REG4,1,IN_PARALLEL);
    VoC_and16_rr(REG4,REG1,DEFAULT);
    VoC_add16_rr(REG4,REG0,REG4,DEFAULT);

    //VoC_shr16_ri(REG7,-1,DEFAULT);        //ib = shl(mult(ia, 6554), 1);
    VoC_shr16_ri(REG1,1,DEFAULT);//gugogi       //pos_indx[index2] = add(ib, shr(ic, 1));

    VoC_add16_rr(REG7,REG7,REG1,DEFAULT);

    VoC_movreg32(REG01,ACC0,IN_PARALLEL);
    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG3,IN_PARALLEL);

    VoC_sw16inc_p(REG4,REG3,DEFAULT);

    VoC_shr16_ri(REG5,-1,DEFAULT);
    VoC_shr16_ri(REG6,2,IN_PARALLEL);
    VoC_add16_rr(REG5,REG5,REG6,DEFAULT);
    VoC_sw16_p(REG7,REG0,DEFAULT);      //pos_indx[index3] = add(shl(mult(MSBs, 1311), 1), shr(LSBs, 2));
    VoC_sw16_p(REG5,REG1,DEFAULT);

    VoC_lw16i_short(ACC0_HI,3,IN_PARALLEL);

    VoC_endloop0

    /*****************************************************************
     Function: CII_decompress10 end
    ******************************************************************/

    VoC_lw16i_short(REG6,3,DEFAULT);
    VoC_lw16inc_p(REG4,REG2,IN_PARALLEL);  //MSBs = shr(index[6], 2);

    VoC_and16_rr(REG6,REG4,DEFAULT);        //LSBs = index[6] & 3;
    VoC_shr16_ri(REG4,2,IN_PARALLEL);//gugogi

    VoC_multf32_rd(REG45,REG4,CONST_25_ADDR);   //MSBs0_24 = shr(add(extract_l(L_shr(L_mult(MSBs, 25), 1)), 12), 5);

    VoC_pop16(REG2,DEFAULT);            // pop16 stack[n-2]  linear_codewords
    VoC_shr32_ri(REG45,1,DEFAULT);
    VoC_add16_rd(REG4,REG4,CONST_12_ADDR);
    VoC_shr16_ri(REG4,5,DEFAULT);

    VoC_multf16_rd(REG5,REG4,CONST_6554_ADDR);  //ia = mult(MSBs0_24, 6554) & 1;
    VoC_lw16i_short(REG3,1,DEFAULT);    //REG3->ia

    VoC_multf32_rd(REG01,REG5,CONST_5_ADDR);    //ib = sub(MSBs0_24, extract_l(L_shr(L_mult(mult(MSBs0_24, 6554), 5), 1)));
    VoC_and16_rr(REG3,REG5,DEFAULT);//change2
    VoC_shr16_ri(REG0,1,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG0,DEFAULT);   //REG4->ib

    VoC_bne16_rd(d8_31pf_L1,REG3,CONST_1_ADDR); //if (sub(ia, 1) == 0)
    VoC_sub16_dr(REG4,CONST_4_ADDR,REG4);       //{ ib = sub(4, ib);
d8_31pf_L1:                     //}

    VoC_lw16i_short(REG0,1,DEFAULT);        //linear_codewords[3] = add(shl(ib, 1), (LSBs & 1));
    VoC_add16_rd(REG1,REG2,CONST_3_ADDR);

    VoC_and16_rr(REG0,REG6,DEFAULT);
    VoC_shr16_ri(REG4,-1,IN_PARALLEL);
    VoC_add16_rr(REG4,REG4,REG0,DEFAULT);

    VoC_shr16_ri(REG5,-1,DEFAULT);      //ia = shl(mult(MSBs0_24, 6554), 1);
    VoC_shr16_ri(REG6,1,IN_PARALLEL);       //linear_codewords[7] = add(ia, shr(LSBs, 1));
    VoC_add16_rr(REG5,REG5,REG6,DEFAULT);
    VoC_add16_rd(REG0,REG2,CONST_7_ADDR);
    VoC_sw16_p(REG4,REG1,DEFAULT);
    VoC_sw16_p(REG5,REG0,DEFAULT);


    VoC_pop16(REG1,DEFAULT);        // pop16 stack[n-1]
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16i_set_inc(REG3,0,1);

    VoC_loop_i(0,4);        //for (j = 0; j < 4; j++)


    VoC_lw16inc_p(REG4,REG2,DEFAULT);        //   i = linear_codewords[j];
    VoC_multf32_rd(REG45,REG4,CONST_2_ADDR);    //   i = extract_l (L_shr (L_mult (i, 4), 1));

    VoC_add16_rr(REG6,REG1,REG3,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG3,DEFAULT);   //   pos1 = add (i, j);

    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_lw16i(REG5,8191);
    VoC_bez16_r(d8_31pf_L2,REG6);       //   if (linear_signs[j] == 0)
    VoC_lw16i(REG5,-8191);          //   {sign = 8191;      }
d8_31pf_L2:                     //   else
    //   {   sign = -8191;   }
    //VoC_add16_rr(REG6,REG4,REG0,DEFAULT);     //   cod[pos1] = sign;
    VoC_add16_rd(REG7,REG2,CONST_3_ADDR);
    //VoC_sw16_p(REG5,REG6,DEFAULT);
    VoC_add16_rr(REG6,REG4,REG0,DEFAULT);

    VoC_lw16_p(REG7,REG7,DEFAULT);      //   i = linear_codewords[add (j, 4)];

    VoC_multf32_rd(ACC0,REG7,CONST_2_ADDR); //   i = extract_l (L_shr (L_mult (i, 4), 1));
    VoC_sw16_p(REG5,REG6,DEFAULT);//VoC_NOP();//change
    VoC_movreg16(REG7,ACC0_LO,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG3,DEFAULT);   //   pos2 = add (i, j);
    VoC_bngt16_rr(d8_31pf_L3,REG4,REG7);    //   if (sub (pos2, pos1) < 0)
    VoC_sub16_dr(REG5,CONST_0_ADDR,REG5);   //   {  sign = negate (sign);
d8_31pf_L3:                     //   }
    VoC_add16_rr(REG7,REG7,REG0,DEFAULT);   //   cod[pos2] = add (cod[pos2], sign);
    VoC_NOP();
    VoC_add16_rp(REG5,REG5,REG7,DEFAULT);
    VoC_inc_p(REG3,DEFAULT);
    VoC_sw16_p(REG5,REG7,DEFAULT);      //}
    VoC_endloop0


    //  begin of CII_dec_8i40_31bits

    VoC_lw16_d(REG5,STRUCT_DECOD_AMRSTATE_SHARP_ADDR);
    VoC_jal(CII_amr_dec_com_sub1);

    // begin of  dec_amr_not795122_s1_102               // M475 ,515 ,59 ,67 ,74 ,102
    VoC_bez16_d(Dec_amr_L35_102A,DEC_AMR_BFI_ADDRESS);      //   if (bfi == 0)
    VoC_jump(Dec_amr_L35_102);
Dec_amr_L35_102A:
    VoC_lw16i_set_inc(REG0,STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR,1); //   {
    VoC_lw16i_set_inc(REG1,DEC_AMR_CODE_ADDRESS,1);
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG1,DEFAULT);
    //      Dec_gain(st->pred_state, mode, index_mr475, code,evenSubfr, &gain_pit, &gain_code);
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /*************************************************************************
     *   FUNCTION:  CII_Dec_gain()
     * Input: gc_predState *pred_state,  ->REG0  push16 stack[n+1]
              enum Mode mode,            ->REG4
              Word16 index,              ->REG6
              Word16 code[],             ->REG1  push16  stack[n]
              Word16 evenSubfr,          DEC_AMR_EVENSUBFR_ADDRESS //(direct address access, needn't pass the address as a parameter)
              Word16 * gain_pit,         DEC_AMR_GAIN_PIT_ADDRESS  //(direct address access, needn't pass the address as a parameter)
              Word16 * gain_cod          DEC_AMR_GAIN_CODE_ADDRESS //(direct address access, needn't pass the address as a parameter)
     * Version 1.0  Created by Kenneth  07/15/2004
       Version 1.1  Optimized by Kenneth 08/13/2004
     ************************************************************************/
// begin of dec_gain
    VoC_shr16_ri(REG6,-2,IN_PARALLEL);          //index = shl (index, 2);
    VoC_lw16i_set_inc(REG1,STATIC_CONST_table_gain_highrates_ADDR,2); //    p = &table_gain_highrates[index];
    VoC_add16_rr(REG1,REG1,REG6,DEFAULT);           // MR67/74/102
    //        p = &table_gain_lowrates[index];
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw32inc_p(REG67, REG1,DEFAULT);             //        *gain_pit = *p++;
    //        g_code = *p++;
    VoC_lw16inc_p(REG3,REG1,DEFAULT);           //        qua_ener_MR122 = *p++;
    VoC_lw16inc_p(REG2,REG1,IN_PARALLEL);               //        qua_ener = *p;
    //    } }

    VoC_sw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);
    VoC_push32(REG23,DEFAULT);              // push32 stack[n]
    VoC_push16(REG7,IN_PARALLEL);           // push16 stack[n-1]

    VoC_lw16_sd(REG1,2,DEFAULT);
    VoC_lw16i_short(REG7,6,IN_PARALLEL);        // mode
    VoC_jal(CII_gc_pred);               //gc_pred(pred_state, mode, code, &exp, &frac, NULL, NULL);


    VoC_movreg16(REG0,REG6,DEFAULT);            //gcode0 = extract_l(Pow2(14, frac));
    VoC_lw16i_short(REG1,14,IN_PARALLEL);
    VoC_jal(CII_Pow2);                  //  REG2->gcode0

    VoC_pop16(REG3,DEFAULT);                //L_tmp = L_mult(g_code, gcode0);
    //VoC_sub16_dr(REG7,CONST_10_ADDR,REG7);        //L_tmp = L_shr(L_tmp, sub(10, exp));
    VoC_multf32_rr(REG23,REG3,REG2,DEFAULT);
    VoC_sub16_dr(REG7,CONST_10_ADDR,REG7);//change2 VoC_NOP();//change
    VoC_shr32_rr(REG23,REG7,DEFAULT);
    VoC_pop32(REG67,DEFAULT);
    VoC_sw16_d(REG3,DEC_AMR_GAIN_CODE_ADDRESS);         //*gain_cod = extract_h(L_tmp);

    VoC_lw16i(REG5,STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR);
    VoC_jal(CII_gc_pred_update);                //gc_pred_update(pred_state, qua_ener_MR122, qua_ener);


    VoC_lw16i_short(INC1,1,DEFAULT);
//  end of dec_gain
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    VoC_pop16(REG1,IN_PARALLEL);
    VoC_pop16(REG0,DEFAULT);
    VoC_jump(Dec_amr_L36_102);                  //   }
Dec_amr_L35_102:                            //   else

    VoC_lw16d_set_inc(REG1,STRUCT_DECOD_AMRSTATE_STATE_ADDR,1);
    VoC_lw16i_set_inc(REG0,STRUCT_EC_GAIN_PITCHSTATE_ADDR,1);   //   {

    //VoC_push16(REG0,DEFAULT);
    VoC_push16(REG1,DEFAULT);
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
Dec_amr_L36_102:
    VoC_lw16i_set_inc(REG1,STRUCT_EC_GAIN_CODESTATE_ADDR,1);
    VoC_lw16i_set_inc(REG0,STRUCT_EC_GAIN_PITCHSTATE_ADDR,1);
    VoC_push16(REG1,DEFAULT);
    VoC_push16(REG0,DEFAULT);
    VoC_lw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);
    VoC_jal(CII_ec_gain_pitch_update);          //   ec_gain_pitch_update (st->ec_gain_p_st, bfi, st->prev_bf,&gain_pit);
    VoC_pop16(REG0,DEFAULT);
    VoC_lw16_d(REG6,DEC_AMR_GAIN_CODE_ADDRESS);
    VoC_jal(CII_ec_gain_code_update);           //   ec_gain_code_update (st->ec_gain_c_st, bfi, st->prev_bf,&gain_code);
    VoC_lw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);      //   pit_sharp = gain_pit;
    VoC_pop16(REG1,DEFAULT);
    VoC_lw16i(REG5,13017);
    VoC_bngt16_rr(Dec_amr_L37_102,REG6,REG5);           //   if (sub (pit_sharp, SHARPMAX) > 0)
    VoC_movreg16(REG6,REG5,DEFAULT);            //   {       pit_sharp = SHARPMAX;    }
Dec_amr_L37_102:                        //}
    VoC_lw16i_short(REG4,6,DEFAULT);        //    if (sub (mode, MR102) == 0)
    VoC_lw16i_short(REG2,45,IN_PARALLEL);       //    {
    VoC_lw16d_set_inc(REG3,STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR,1);
    VoC_bngt16_rr(Dec_amr_L39_102,REG3,REG2);       //       if (sub (st->old_T0, add(L_SUBFR, 5)) > 0)
    VoC_shr16_ri(REG6,2,DEFAULT);               //       {   pit_sharp = shr(pit_sharp, 2);   }
Dec_amr_L39_102:                                //    }
    // end of  dec_amr_not795122_s1_102

    VoC_shr16_ri(REG6,-1,DEFAULT );             //pit_sharp = shl (pit_sharp, 1);
    VoC_lw16_d(REG2,DEC_AMR_GAIN_PIT_ADDRESS);      // gain_pit in reg2
    VoC_sw16_d(REG6,DEC_AMR_PIT_SHARP_ADDRESS);


    VoC_lw16i_set_inc(REG0,STRUCT_DECOD_AMRSTATE_EXC_ADDR,1);   //{
    VoC_bngt16_rd(Dec_amr_L52_102,REG6,CONST_0x4000_ADDR);  //if (sub (pit_sharp, 16384) > 0)
    VoC_multf16inc_rp(REG5,REG6,REG0,DEFAULT);//change
    VoC_lw16i_set_inc(REG1,DEC_AMR_EXCP_ADDRESS,1);
    VoC_multf32_rr(ACC0,REG5,REG2,DEFAULT);     //       L_temp = L_mult (temp, gain_pit);



    VoC_loop_i_short(40,DEFAULT);                   //   for (i = 0; i < L_SUBFR; i++)
    //VoC_multf16inc_rp(REG5,REG6,REG0,IN_PARALLEL);
    VoC_startloop0                      //    {
    //       temp = mult (st->exc[i], pit_sharp);
    VoC_multf16inc_rp(REG5,REG6,REG0,DEFAULT);
    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000_ADDR);      //       excp[i] = round (L_temp);
    VoC_multf32_rr(ACC0,REG5,REG2,DEFAULT);     //       L_temp = L_mult (temp, gain_pit);
    exit_on_warnings =OFF;
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    exit_on_warnings =ON;
    VoC_endloop0                            //    }
Dec_amr_L52_102:                                //}


    VoC_lw16i_set_inc(REG0,13017,1);

    VoC_movreg16(REG7,REG2,DEFAULT);        //    st->sharp = gain_pit;
    VoC_bngt16_rr(Dec_amr_L54_102,REG7,REG0);           //    if (sub (st->sharp, SHARPMAX) > 0)
    VoC_movreg16(REG7,REG0,DEFAULT);        //    {  st->sharp = SHARPMAX;
Dec_amr_L54_102:                        // }
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_sw16_d(REG7,STRUCT_DECOD_AMRSTATE_SHARP_ADDR);  //}

    VoC_bnez16_d(Dec_amr_L56_102,DEC_AMR_BFI_ADDRESS);      //if ( bfi == 0 )
    VoC_lw16i_set_inc(REG3,STRUCT_DECOD_AMRSTATE_LTPGAINHISTORY_ADDR,1);//{
    VoC_add16_rr(REG0,REG3,REG6,DEFAULT);
    VoC_lw16i_short(REG6,8,DEFAULT);                   //   for (i = 0; i < 8; i++)
    VoC_jal(CII_R02R3_p_R6);                //   {   st->ltpGainHistory[i] = st->ltpGainHistory[i+1]; }
    VoC_sw16_p(REG2,REG3,DEFAULT);          //   st->ltpGainHistory[8] = gain_pit;
    //}
Dec_amr_L56_102:
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG0,DEFAULT);
    VoC_jal(CII_Int_lsf);               //Int_lsf(prev_lsf, st->lsfState->past_lsf_q, i_subfr, lsf_i);


    VoC_lw32_d(REG67,STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR);
    VoC_add32_rd(REG67,REG67,CONST_0x00010001_ADDR);
    VoC_bgt16_rd(CGAver_L5_102,REG5,CONST_5325_ADDR);       //if (sub(diff, 5325) > 0)
    VoC_lw16i_short(REG7,0,DEFAULT);            //{ st->hangVar = add(st->hangVar, 1);}
CGAver_L5_102:                      // else{st->hangVar = 0; }

    VoC_bngt16_rd(CGAver_L6_102,REG7,CONST_10_ADDR);    //if (sub(st->hangVar, 10) > 0)
    VoC_lw16i_short(REG6,1,DEFAULT);            //{  st->hangCount = 0;
CGAver_L6_102:
    VoC_lw16_sd(ACC1_HI,0,DEFAULT);         //bgMix = 8192; cbGainMix = gain_code;

    VoC_lw16i(REG4,3277);
    //  begin of dec_amr_not74795122_s1_102
    //   if (sub(2048, tmp1) < 0)
    VoC_sub16_rr(REG3,REG5,REG4,DEFAULT);               //   { bgMix = 8192; }
    VoC_bgtz16_r(CGAver_L11_102,REG3);              //   else   {bgMix = shl(tmp1, 2);             } }
    VoC_lw16i_short(REG3,0,DEFAULT);            //else {
CGAver_L11_102:                                 //   tmp_diff = sub(diff, 3277);
    VoC_shr16_ri(REG3,-2,DEFAULT);                          //   if (tmp_diff > 0)
    VoC_bgt16_rd(CGAver_L17_102,REG3,CONST_0x2000_ADDR);    //   {tmp1 = tmp_diff;  }
    VoC_blt16_rd(CGAver_L17_102,REG6,CONST_41_ADDR);        //if ((sub(st->hangCount, 40) < 0) || (sub(diff, 5325) > 0))
    VoC_bngt16_rd(CGAver_L10_102,REG5,CONST_5325_ADDR);
CGAver_L17_102:
    VoC_lw16i(REG3,8192);                   //   else {tmp1 = 0; }
CGAver_L10_102:                                         //   if (sub(2048, tmp1) < 0)
    //   {bgMix = 8192;  }
    //   else  {bgMix = shl(tmp1, 2); }  }


    VoC_lw32_sd(ACC0,1,DEFAULT);            // should check the value if equal 0x00008000
    VoC_lw16_sd(REG2,1,DEFAULT);
    VoC_lw32_sd(REG01,0,IN_PARALLEL);

    VoC_lw16i(REG4,6554);
    VoC_add16_rd(REG2,REG2,CONST_2_ADDR);

    VoC_loop_i_short(5,DEFAULT);            //L_sum = L_mult(6554, st->cbGainHistory[2]);
    VoC_startloop0                          //for (i = 3; i < L_CBGAINHIST; i++)
    VoC_mac32inc_rp(REG4,REG2,DEFAULT);             //{ L_sum = L_mac(L_sum, 6554, st->cbGainHistory[i]);}
    VoC_endloop0

    VoC_pop16(REG5,DEFAULT);                //  cbGainMix
    VoC_lw32_sd(ACC1,1,IN_PARALLEL);            // should check the value if equal 0x00008000
    VoC_movreg16(REG1,ACC0_HI,DEFAULT);
    VoC_multf32_rr(ACC0,REG3,REG5,IN_PARALLEL);     //   L_sum = L_mult(bgMix, cbGainMix);
    VoC_mac32_rd(REG1,CONST_0x2000_ADDR);       //   L_sum = L_mac(L_sum, 8192, cbGainMean);
    VoC_msu32_rr(REG3,REG1,DEFAULT);            //   L_sum = L_msu(L_sum, bgMix,cbGainMean);

    VoC_sw32_d(REG67,STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR);
    VoC_shr32_ri(ACC0,-2,DEFAULT);              //   cbGainMix = round(L_shl(L_sum, 2));
    VoC_pop16(REG7,IN_PARALLEL);
    VoC_add32_rr(ACC1,ACC0,ACC1,DEFAULT);   //}

    //  end of dec_amr_not74795122_s1_102

    //return cbGainMix;  ->ACC1_HI

    VoC_lw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);
    VoC_sw16_d(ACC1_HI,DEC_AMR_GAIN_CODE_MIX_ADDRESS);

    VoC_lw16i_short(REG7,1,DEFAULT);        //   tmp_shift = 1;

    VoC_jal(CII_amr_dec_com_sub3);

    VoC_jal(CII_amr_dec_com_sub4);
    VoC_pop32(ACC0,DEFAULT);
    VoC_jal(CII_amr_dec_com_sub5);
    VoC_jump(Dec_amr_LoopStart_102);    //  }
Dec_amr_LoopEnd_102:

    VoC_pop16(REG5,DEFAULT);        // pop16 stack[n-2]
    VoC_pop16(REG4,DEFAULT);        // pop16 stack[n-1]
    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_lw16_sd(REG3,1,DEFAULT);
    VoC_return;

}

/**/
#ifdef voc_compile_only
// VoC_directive: FAKE_FUNCTION_ON
void CII_amr_dec_475(void) {};
void CII_amr_dec_515(void) {};
void CII_amr_dec_59(void) {};
void CII_amr_dec_67(void) {};
void CII_amr_dec_74(void) {};
void CII_amr_dec_795(void) {};
void CII_amr_dec_122(void) {};
void CII_COD_AMR_475(void) {};
void CII_COD_AMR_515(void) {};
void CII_COD_AMR_59(void) {};
void CII_COD_AMR_67(void) {};
void CII_COD_AMR_74(void) {};
void CII_COD_AMR_795(void) {};
void CII_COD_AMR_122(void) {};
void CII_HR_Encode(void) {};
void CII_HR_Decode(void) {};
void CII_FR_Encode(void) {};
void CII_FR_Decode(void) {};

// VoC_directive: FAKE_FUNCTION_OFF
#endif

