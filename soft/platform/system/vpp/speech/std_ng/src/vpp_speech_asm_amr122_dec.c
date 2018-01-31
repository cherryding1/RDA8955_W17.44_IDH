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


void CII_amr_dec_122(void)
{




#if 0

    voc_short    blank_dec                       ,106 ,x
    voc_short    DEC_AMR_BUFA40_ADDRESS                        ,x
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
    voc_short    DEC_AMR_TEMP23_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP24_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP25_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP26_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP27_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP28_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP29_ADDRESS                        ,11,x

    voc_short    DEC_AMR_D2PF_POS_ADDRESS                      ,2,x
    voc_short    DEC_AMR_D2PF_POS2_ADDRESS                     ,2,x
    voc_short    DEC_AMR_D2PF_POS4_ADDRESS                     ,2,x
    voc_short    DEC_AMR_D2PF_POS6_ADDRESS                     ,14,x

    voc_short    DEC_AMR_TEMP30_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP31_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP32_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP33_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP34_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP35_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP36_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP37_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP38_ADDRESS                        ,x
    voc_short    DEC_AMR_TEMP39_ADDRESS                        ,x

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

//  EFR DTX BEGIN
    VoC_push16(REG6,DEFAULT);

    VoC_bez16_d(amr_dec_122_EFR1,GLOBAL_EFR_FLAG_ADDR)

    VoC_jal(CII_efr_dtx_func1);
    VoC_bez16_r(EFR_DTX_D_PLSF5_END,REG6);

amr_dec_122_EFR1:
    // RESTOR the values
    VoC_pop16(REG6,DEFAULT);

//  EFR DTX END

    VoC_lw16i_set_inc(REG2,TABLE_MEAN_LSF_ADDR,1);
    VoC_lw16i_set_inc(REG3,DEC_AMR_BUFA40_ADDRESS,1);
    VoC_push16(REG2,DEFAULT);               //stack[n-2]

    VoC_bez16_r(Dplsf5_L1,REG6)             //if (bfi != 0){
    VoC_lw16i(REG6,1639);
    VoC_lw16i(RL7_LO,31128);
    VoC_lw16i(RL7_HI,21299);
    VoC_movreg16(RL6_LO,REG6,DEFAULT);

    VoC_loop_i_short(10,DEFAULT);               //for (i = 0; i < M; i++)
    VoC_lw16i_short(REG7,9,IN_PARALLEL);
    VoC_startloop0                          //{

    VoC_multf16_rp(REG4,REG6,REG2,DEFAULT);
    VoC_movreg16(REG6,RL7_LO,IN_PARALLEL);
//      VoC_multf16inc_pd(REG5,REG0,CONST_31128_ADDR);  //    lsf1_q[i] = add (mult (st->past_lsf_q[i], ALPHA),mult (mean_lsf[i], ONE_ALPHA));
    VoC_multf16inc_rp(REG5,REG6, REG0,DEFAULT);

    VoC_add16_rr(REG1,REG0,REG7,DEFAULT);
    VoC_movreg16(REG6,RL7_HI,IN_PARALLEL);

    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);
    //  VoC_multf16_pd(REG4,REG1,CONST_21299_ADDR); //    temp = add (mean_lsf[i], mult (st->past_r_q[i],LSP_PRED_FAC_MR122));
    VoC_multf16_rp(REG4,REG6, REG1,DEFAULT);

    VoC_sw16inc_p(REG5,REG3,DEFAULT);       //    lsf2_q[i] = lsf1_q[i];
    VoC_add16inc_rp(REG4,REG4,REG2,DEFAULT);
    VoC_sub16_rr(REG4,REG5,REG4,DEFAULT);       //    st->past_r_q[i] = sub (lsf2_q[i], temp);

    VoC_add16_rr(REG4,REG3,REG7,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16_p(REG4,REG1,DEFAULT);      //}
    exit_on_warnings=ON;
    VoC_sw16_p(REG5,REG4,DEFAULT);
    VoC_movreg16(REG6,RL6_LO,IN_PARALLEL);

    VoC_endloop0
    VoC_jump(Dplsf5_LEnd);              //}
Dplsf5_L1:                          //else   {


    VoC_lw16i_short(INC3,2,DEFAULT);

    VoC_lw16i(REG7,TABLE_DICO1_LSF_compressed_ADDR);    //p_dico = &dico1_lsf[shl (indice[0], 2)];
    //lsf1_r[0] = *p_dico++;  lsf1_r[1] = *p_dico++;
    //lsf2_r[0] = *p_dico++;  lsf2_r[1] = *p_dico++;
    //p_dico = &dico2_lsf[shl (indice[1], 2)];
    //lsf1_r[2] = *p_dico++;   lsf1_r[3] = *p_dico++;

    VoC_loop_i(1, 2);
    VoC_jal(CII_D_plsf_5_sub);
    VoC_lw16i(REG7,TABLE_DICO2_LSF_compressed_ADDR);
    VoC_endloop1
    //lsf2_r[2] = *p_dico++;   lsf2_r[3] = *p_dico++;
    VoC_lw16i_short(REG2,1,DEFAULT);        //sign = indice[2] & 1;
    VoC_lw16inc_p(REG0,REG1,DEFAULT);
    VoC_and16_rr(REG2,REG0,DEFAULT);

    VoC_shr16_ri(REG0,1,DEFAULT);           //i = shr (indice[2], 1);

    VoC_lw16i(REG7,TABLE_DICO3_LSF_compressed_ADDR );

    VoC_jal(Coolsand_read_13bits);

    //{
//  VoC_lw32inc_p(REG45,REG0,DEFAULT);      //   lsf1_r[4] = *p_dico++;   lsf1_r[5] = *p_dico++;
//  VoC_lw32inc_p(REG67,REG0,DEFAULT);      //   lsf2_r[4] = *p_dico++;   lsf2_r[5] = *p_dico++;
    VoC_add16_rd(REG0,REG3,CONST_10_ADDR);

    VoC_bez16_r(D_plsf5_L2,REG2);       //if (sign == 0)

    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_sub16_rr(REG4,REG2,REG4,DEFAULT);       //}
    VoC_sub16_rr(REG5,REG2,REG5,IN_PARALLEL);   //else{
    VoC_sub16_rr(REG6,REG2,REG6,DEFAULT);   // lsf1_r[4] = negate (*p_dico++); lsf1_r[5] = negate (*p_dico++);
    VoC_sub16_rr(REG7,REG2,REG7,IN_PARALLEL);   // lsf2_r[4] = negate (*p_dico++); lsf2_r[5] = negate (*p_dico++);
D_plsf5_L2:




    VoC_sw32inc_p(REG45,REG3,DEFAULT);
    VoC_sw32_p(REG67,REG0,DEFAULT);         //}

    VoC_lw16i(REG7,TABLE_DICO4_LSF_compressed_ADDR);       //p_dico = &dico4_lsf[shl (indice[3], 2)];
    //lsf1_r[6] = *p_dico++;   lsf1_r[7] = *p_dico++;
    //lsf2_r[6] = *p_dico++;   lsf2_r[7] = *p_dico++;
    //p_dico = &dico5_lsf[shl (indice[4], 2)];
    //lsf1_r[8] = *p_dico++;    lsf1_r[9] = *p_dico++;
    VoC_loop_i(1, 2);
    VoC_jal(CII_D_plsf_5_sub);
    VoC_lw16i(REG7,TABLE_DICO5_LSF_compressed_ADDR);
    VoC_endloop1

    //lsf2_r[8] = *p_dico++;    lsf2_r[9] = *p_dico++;
    VoC_lw16_sd(REG1,1,DEFAULT);
    VoC_lw16i_short(REG7,10,IN_PARALLEL);
    VoC_lw16i_set_inc(REG3,DEC_AMR_BUFA40_ADDRESS,1);
    VoC_lw16_sd(REG2,0,DEFAULT);            // load mean_lsf
    VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

//  EFR DTX BEGIN
    VoC_bez16_d(Dplsf5_LEnd_EFR,GLOBAL_EFR_FLAG_ADDR)
    VoC_jal(CII_efr_dtx_func2);
    VoC_bez16_r(Dplsf5_LEnd,REG4);

Dplsf5_LEnd_EFR:
//  EFR DTX END

//  VoC_multf16_pd(REG4,REG1,CONST_21299_ADDR);
    VoC_lw16i(REG0,21299);
    VoC_multf16_rp(REG4,REG0,REG1,DEFAULT);

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
    //  VoC_multf16_pd(REG4,REG1,CONST_21299_ADDR);
    VoC_multf16_rp(REG4,REG0,REG1,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16_p(REG4,REG6,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop0                    //    }
Dplsf5_LEnd:                            //}

    VoC_lw16i_set_inc(REG0,DEC_AMR_BUFA40_ADDRESS,1);   //Reorder_lsf (lsf1_q, LSF_GAP, M);

    VoC_jal(CII_Reorder_lsf);

    VoC_jal(CII_Reorder_lsf);                   //Reorder_lsf (lsf2_q, LSF_GAP, M);  //Note: REG0->DEC_AMR_TEMP0_ADDRESS+10

//  EFR DTX BEGIN


    VoC_bez16_d(D_plsf_5_end_EFR,GLOBAL_EFR_FLAG_ADDR)

    VoC_jal(CII_efr_dtx_func3);

D_plsf_5_end_EFR:

//  EFR DTX END


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
    VoC_lw16i(REG0,DEC_AMR_BUFA40_ADDRESS);         //Lsf_lsp (lsf1_q, lsp1_q, M);
    VoC_lw16i_set_inc(REG1,DEC_AMR_LSP_MID_ADDRESS,1);
    VoC_jal(CII_Lsf_lsp);

    VoC_lw16i_set_inc(REG0,DEC_AMR_TEMP10_ADDRESS,1);   //Lsf_lsp (lsf2_q, lsp2_q, M);
    VoC_lw16i_set_inc(REG1,DEC_AMR_LSP_NEW_ADDRESS,1);
    VoC_jal(CII_Lsf_lsp);
EFR_DTX_D_PLSF5_END:
    VoC_pop16(REG0,DEFAULT);
//  end of D_plsf_5
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    VoC_lw16_sd(REG7,1,DEFAULT);            //   parm += 5;
    VoC_lw16i_short(REG0,5,IN_PARALLEL);
    VoC_add16_rr(REG7,REG7,REG0,DEFAULT);
    VoC_lw32_sd(REG23,0,IN_PARALLEL);       // load  &A_t[]/&lsp_mid[]
    VoC_lw32_sd(REG45,1,DEFAULT);           // load  &lsp_new[]/&lsp_old[]

    VoC_sw16_sd(REG7,1,DEFAULT);
//  EFR DTX BEGIN

    VoC_bez16_d(CII_Int_lpc_1and3_dec_EFR,GLOBAL_EFR_FLAG_ADDR)
    VoC_jal(CII_efr_dtx_func4);
    VoC_bez16_r(CII_Int_lpc_1and3_dec_EFR1,REG7);

CII_Int_lpc_1and3_dec_EFR:
//  EFR DTX END
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_movreg16(REG0,REG3,DEFAULT);
    VoC_movreg16(REG3,REG4,IN_PARALLEL);    //   Int_lpc_1and3 (st->lsp_old, lsp_mid, lsp_new, A_t);

    VoC_lw16i_set_inc(REG1,INT_LPC_LSP_ADDR,2);

Int_lpc_1and3_L0_dec:

    VoC_push16(REG5,DEFAULT);           // push16(n-1)  lsp_new
    VoC_push32(REG67,DEFAULT);          // push32(n)    N/REG6
    VoC_push16(REG2,DEFAULT);           // push16(n-2)  lsp_mid
    VoC_push32(REG01,IN_PARALLEL);          // push32(n-1)  lsp/Az

    VoC_jal(CII_Int_lpc_only);
    VoC_lw32_sd(REG67,1,DEFAULT);
    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_jal(CII_Lsp_Az);
    VoC_lw32_sd(REG67,1,DEFAULT);
    VoC_lw16i_short(REG0,11,DEFAULT);
    VoC_add16_rr(REG5,REG7,REG0,DEFAULT);

    VoC_add16_rr(REG7,REG5,REG0,DEFAULT);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_sw32_sd(REG67,1,DEFAULT);

    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG5,IN_PARALLEL);

    VoC_bne16_rd(Int_lpc_1and3_L2_dec,REG5,(68+TABLE_MR475_ADDR));//33
    VoC_lw16_sd(REG1,1,DEFAULT);
Int_lpc_1and3_L2_dec:

    VoC_jal(CII_Lsp_Az);

    VoC_pop16(REG2,DEFAULT);
    VoC_pop32(REG01,DEFAULT);
    VoC_pop16(REG3,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);
    VoC_movreg16(REG5, REG3,DEFAULT);

    VoC_bne16_rd(Int_lpc_1and3_L0_dec,REG7,(31+TABLE_MR475_ADDR));//44

CII_Int_lpc_1and3_dec_EFR1:


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


Dec_amr_LoopStart_122:

    VoC_lw16i_short(REG3,1,DEFAULT);        //{
    VoC_lw16i_short(INC0,1,DEFAULT);

    VoC_push16(REG5,DEFAULT);               //   i_subfr  push16 stack[n-3]
    VoC_add16_rd(REG6,REG3,DEC_AMR_SUBFRNR_ADDRESS);    //   subfrNr = add(subfrNr, 1);
    VoC_sub16_rd(REG4,REG3,DEC_AMR_EVENSUBFR_ADDRESS);  //evenSubfr = sub(1, evenSubfr);
    VoC_sw16_d(REG6,DEC_AMR_SUBFRNR_ADDRESS);
    VoC_sw16_d(REG4,DEC_AMR_EVENSUBFR_ADDRESS);     //   pit_flag = i_subfr;

    VoC_bne16_rd(Dec_amr_L14_122,REG5,CONST_80_ADDR);   //   if (sub (i_subfr, L_FRAME_BY2) == 0)
    //   { if (sub(mode, MR475) != 0 && sub(mode, MR515) != 0)
    VoC_lw16i_short(REG5,0,DEFAULT);            //      {  pit_flag = 0;  }
Dec_amr_L14_122:
    VoC_lw16_sd(REG0,2,DEFAULT);            // assure REG0 incr=1
    VoC_sw16_d(REG5,DEC_AMR_PIT_FLAG_ADDRESS);
    VoC_lw16inc_p(REG3,REG0,DEFAULT);           //index = *parm++;
    VoC_lw16_d(REG4,STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR);
    VoC_sw16_sd(REG0,2,DEFAULT);            // restore the new address of parm
    VoC_sw16_d(REG3,DEC_AMR_INDEX_ADDRESS);

//  EFR DTX BEGIN
    VoC_bez16_d(EFR_jump_label1,GLOBAL_EFR_FLAG_ADDR)
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(REG2,40,IN_PARALLEL);
    VoC_and16_rd(REG6,GLOBAL_RXDTX_CTRL_ADDR);
    VoC_bnez16_r(EFR_jump_label1,REG6);
    VoC_sw16_d(REG2,DEC_AMR_T0_ADDRESS);
    VoC_jump(EFR_jump_label2);
EFR_jump_label1:
//  EFR DTX END

    // begin of dec_amr_122_s2
    // {
    VoC_lw16i_short(REG2,5,DEFAULT);
    VoC_lw16_d(REG6,DEC_AMR_T0_ADDRESS);            //   Dec_lag6 (index, PIT_MIN_MR122, PIT_MAX, pit_flag, &T0, &T0_frac);
    //  begin of CII_Dec_lag6

    VoC_bnez16_d(Dec_lag6_L1,DEC_AMR_PIT_FLAG_ADDRESS);     //if (i_subfr == 0)

    VoC_lw16i_short(REG7,0,DEFAULT);    //   {    *T0_frac = 0;

    VoC_add16_rr(REG5,REG3,REG2,DEFAULT);       //   {
    VoC_multf16_rd(REG5,REG5,CONST_5462_ADDR);          //      *T0 = add (mult (add (index, 5), 5462), 17);
    VoC_sub16_rd(REG6,REG3,CONST_368_ADDR);  //            *T0 = sub (index, 368);

    VoC_bnlt16_rd(Dec_lag6_L2,REG3,(10+TABLE_DICO2_LSF_3_compressed_ADDR));     // {  if (sub (index, 463) < 0)

    VoC_add16_rd(REG6,REG5,STATIC_CONST_PRMNO_ADDR);//17
    VoC_mult16_rd(REG5,REG6,CONST_6_ADDR);      //      i = add (add (*T0, *T0), *T0);

    VoC_lw16i(REG7,105);
    VoC_sub16_rr(REG5,REG3,REG5,DEFAULT);       //      *T0_frac = add (sub (index, add (i, i)), 105);
    VoC_add16_rr(REG7,REG5,REG7,DEFAULT);           //   }

Dec_lag6_L2:                        //   else
    VoC_jump(Dec_lag6_end);                                 //   }}

Dec_lag6_L1:                        //else{


    VoC_sub16_rr(REG5,REG6,REG2,DEFAULT);       //   T0_min = sub (*T0, 5);

    VoC_bnlt16_rd(Dec_lag6_L3,REG5,(37+TABLE_MR475_ADDR));      // CONST_18_ADDR  if (sub (T0_min, pit_min) < 0)
    VoC_lw16i_short(REG5,18,DEFAULT);           //   {    T0_min = pit_min;
Dec_lag6_L3:                        //   }

    VoC_bngt16_rd(Dec_lag6_L4,REG5,  (210+TABLE_MR122_ADDR)  );     //  CONST_134_ADDR if (sub (T0_max, pit_max) > 0)
    VoC_lw16i(REG5,134);                    //   {   T0_max = pit_max;
    //      T0_min = sub (T0_max, 9);
Dec_lag6_L4:                        //   }


    VoC_add16_rr(REG2,REG3,REG2,DEFAULT);       //   i = sub (mult (add (index, 5), 5462), 1);
    VoC_multf16_rd(REG2,REG2,CONST_5462_ADDR);

    VoC_sub16_rd(REG7,REG3,CONST_3_ADDR);       //   *T0_frac = sub (sub (index, 3), add (i, i));
    VoC_sub16_rd(REG2,REG2,CONST_1_ADDR);

    VoC_mult16_rd(REG2,REG2,CONST_6_ADDR);          //   i = add (add (i, i), i);
    exit_on_warnings =OFF;
    VoC_add16_rr(REG6,REG2,REG5,DEFAULT);       //   *T0 = add (i, T0_min);
    exit_on_warnings =ON;
    VoC_sub16_rr(REG7,REG7,REG2,DEFAULT);       //}
    //  end of CII_Dec_lag6
Dec_lag6_end:

    VoC_lw16i_short(REG2,0,DEFAULT);            //   Pred_lt_3or6 (st->exc, T0, T0_frac, L_SUBFR, 0);
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

    // end of  dec_amr_122_s2
    VoC_lw16i(REG5,STRUCT_DECOD_AMRSTATE_EXC_ADDR);
    VoC_sw32_d(REG67,DEC_AMR_T0_ADDRESS);       // restore T0 &T0_frac

    VoC_jal(CII_Pred_lt_3or6);

EFR_jump_label2:
    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16i_set_inc(REG1,DEC_AMR_CODE_ADDRESS,1);
    VoC_lw16_sd(REG2,2,DEFAULT);            // parm in reg2

    VoC_push16(REG1,DEFAULT);           // code push16 stack[n-4]

    // begin of dec_amr_122_s3
    VoC_lw16inc_p(REG1,REG2,DEFAULT);            //    index = *parm++;
    VoC_lw16i_set_inc(REG0,STRUCT_EC_GAIN_PITCHSTATE_ADDR,1);
    VoC_lw16d_set_inc(REG3,STRUCT_DECOD_AMRSTATE_STATE_ADDR,1);
    VoC_sw16_sd(REG2,3,DEFAULT);

    VoC_push16(REG0,DEFAULT);

    VoC_lw16i(REG6,STATIC_CONST_QUA_GAIN_PITCH_ADDR);   // gain =  shr (qua_gain_pitch[index], 2)
    VoC_add16_rr(REG6,REG6,REG1,DEFAULT);

    VoC_bez16_d(Dec_amr_L29_122,DEC_AMR_BFI_ADDRESS);       //    if (bfi != 0)

    VoC_push16(REG3,DEFAULT);               //    {
    VoC_jal(CII_ec_gain_pitch);             //       ec_gain_pitch (st->ec_gain_p_st, st->state, &gain_pit);
    VoC_pop16(REG3,DEFAULT);
    VoC_jump(Dec_amr_L30_122);              //    }
Dec_amr_L29_122:                        //    else

    //    {  gain_pit = d_gain_pitch (mode, index);   }


//  EFR DTX BEGIN



    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_shr16_ri(REG6,2,DEFAULT);

    VoC_bez16_d(Dec_amr_L30_122_EFR,GLOBAL_EFR_FLAG_ADDR)

    VoC_and16_rd(REG3,GLOBAL_RXDTX_CTRL_ADDR);
    VoC_bnez16_r(Dec_amr_L30_122_EFR1,REG3);
    VoC_lw16i_short(REG6,0,DEFAULT);

Dec_amr_L30_122_EFR:
//  EFR DTX END
    VoC_shr16_ri(REG6,-2,DEFAULT);       //if (sub(mode, MR122) == 0)
Dec_amr_L30_122_EFR1:
    VoC_NOP();
    VoC_sw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);

Dec_amr_L30_122:


    VoC_jal(CII_ec_gain_pitch_update);          //    ec_gain_pitch_update (st->ec_gain_p_st, bfi, st->prev_bf,&gain_pit);

    VoC_pop16(REG0,DEFAULT);
    // REG0 for &cod[]
    VoC_lw16_sd(REG0,0,DEFAULT);

// EFR DTX BEGIN

    VoC_bez16_d(CII_dec_10i40_35bits_EFR,GLOBAL_EFR_FLAG_ADDR)
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_and16_rd(REG3,GLOBAL_RXDTX_CTRL_ADDR);
    VoC_bnez16_r(CII_dec_10i40_35bits_EFR,REG3);
    VoC_lw32_d(ACC0,GLOBAL_L_PN_SEED_RX_ADDR);

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_jal(CII_build_CN_code);
    VoC_lw16_sd(REG1,3,DEFAULT);
    VoC_add16_rd(REG2,REG1,CONST_10_ADDR);
    VoC_sw32_d(ACC0,GLOBAL_L_PN_SEED_RX_ADDR);

    VoC_sw16_sd(REG2,3,DEFAULT);            //    parm += 10;
    VoC_jump(CII_dec_10i40_35bits_end_EFR);
CII_dec_10i40_35bits_EFR:
// EFR DTX END


    VoC_lw16_sd(REG1,3,DEFAULT);
    VoC_add16_rd(REG2,REG1,CONST_10_ADDR);

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
    VoC_lw16i_short(REG6,7,DEFAULT);        //    i = tmp & 7;
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

    VoC_add16_rd(REG5,REG1,CONST_4_ADDR);       //    i = index[add (j, 5)] & 7;
    VoC_add16_rr(REG3,REG0,REG6,DEFAULT);
    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_and16_ri(REG5,7);
    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);       //    i = dgray[i];


    VoC_sw16_p(REG7,REG3,DEFAULT);          //    cod[pos1] = sign;
    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_multf32_rd(ACC0,REG5,CONST_5_ADDR);     //    i = extract_l (L_shr (L_mult (i, 5), 1));
    VoC_NOP();
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_movreg16(REG5,ACC0_LO,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG2,DEFAULT);       //    pos2 = add (i, j);
    VoC_bngt16_rr(DEC_10I40_35BITS_L2,REG6,REG5);   //    if (sub (pos2, pos1) < 0)
    VoC_sub16_dr(REG7,CONST_0_ADDR,REG7);       //    {  sign = negate (sign);     }
DEC_10I40_35BITS_L2:
    VoC_add16_rr(REG6,REG0,REG5,DEFAULT);       //    cod[pos2] = add (cod[pos2], sign);
    VoC_NOP();
    VoC_add16_rp(REG7,REG7,REG6,DEFAULT);
    VoC_inc_p(REG2,DEFAULT);
    VoC_sw16_p(REG7,REG6,DEFAULT);
    VoC_endloop0                    //}
// end of CII_dec_10i40_35bits
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
CII_dec_10i40_35bits_end_EFR:

    VoC_lw16_d(REG5,DEC_AMR_GAIN_PIT_ADDRESS);      //    pit_sharp = shl (gain_pit, 1);
    // }
    // end of dec_amr_122_s3

    VoC_jal(CII_amr_dec_com_sub1);


    // REG6 for index
    // parm++;
    VoC_sw16_d(REG6,DEC_AMR_INDEX_ADDRESS);

//  EFR DTX BEGIN

    VoC_bez16_d(gc_pred_M122_dec_EFR,GLOBAL_EFR_FLAG_ADDR)

    VoC_jal(CII_efr_dtx_func5);

    VoC_bez16_r(D_GAIN000,REG0);//return

gc_pred_M122_dec_EFR:
//  EFR DTX END



    // begin of dec_amr_122_s4
    VoC_lw16i_set_inc(REG0,STRUCT_EC_GAIN_CODESTATE_ADDR,1);
    VoC_lw16i_set_inc(REG1,STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR,1);
    VoC_push16(REG0,DEFAULT);               // push16 st->ec_gain_c_st
    VoC_push16(REG1,DEFAULT);               // push16 st->pred_state

    VoC_lw16d_set_inc(REG2,STRUCT_DECOD_AMRSTATE_STATE_ADDR,1);//      {

    VoC_bnez16_d(Dec_amr_L44_122,DEC_AMR_BFI_ADDRESS);      //      if (bfi == 0)

    //         d_gain_code (st->pred_state, mode, index, code, &gain_code);
// EFR DTX BEGIN

    VoC_mult16_rd(REG6,REG6,CONST_3_ADDR);
    VoC_lw16i(REG3,STATIC_CONST_qua_gain_code_ADDR);
    VoC_add16_rr(REG5,REG6,REG3,DEFAULT);               //  p->REG6



    VoC_lw16i_set_inc(REG0,COD_AMR_CODE_ADDRESS, 2);
    VoC_loop_i_short(20, DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_bimac32inc_pp(REG0, REG0);
    VoC_endloop0

    VoC_add16_rd(REG0, REG1, CONST_4_ADDR);   //REG0  :     st->past_qua_en_MR122
    VoC_add32_rd(REG23, ACC0, CONST_0x00008000_ADDR);
    VoC_multf32_rd(ACC0, REG3, (3+STATIC_CONST_PDOWN_ADDR)  );  //  CONST_26214_ADDR   1216


    VoC_bez16_d(d_gain_code_EFR,GLOBAL_EFR_FLAG_ADDR)

    VoC_jal(CII_efr_dtx_func6);
    VoC_bez16_r(d_gain_code_end_EFR,REG7);
d_gain_code_EFR:
// EFR DTX END

    VoC_push16(REG5,DEFAULT);               //  push16 index

    VoC_jal(CII_Log2);  //   //REG4  exp   REG5   frac

    VoC_lw32_d(ACC0, CONST_783741L_ADDR); //acc0 is     783741L    MEAN_ENER_MR122
    VoC_lw16i_set_inc(REG1, STATIC_CONST_pred_MR122_ADDR, 1);

    VoC_loop_i_short(4, DEFAULT);
    VoC_startloop0;
    VoC_mac32inc_pp(REG0, REG1,DEFAULT);
    VoC_endloop0;

    VoC_movreg16(REG6, REG5, DEFAULT); //reg67 :ener_code
    VoC_shr32_ri(REG67, -1, DEFAULT);
    VoC_lw16i_short(REG2,30,IN_PARALLEL);

    VoC_sub16_rr(REG7, REG4, REG2,DEFAULT);
    VoC_sub32_rr(REG01, ACC0, REG67, DEFAULT);
    VoC_shr32_ri(REG01, 1, DEFAULT);


    VoC_shru16_ri(REG0, 1,DEFAULT);


    VoC_jal(CII_Pow2);                  // {gcode0 = extract_l (Pow2 (exp, frac));
    //  return REG23
    VoC_pop16(REG0,DEFAULT);        //p = &qua_gain_code[add (add (index, index), index)];

    VoC_shr16_ri(REG2,-4,DEFAULT);
    VoC_multf16inc_rp(REG2,REG2,REG0,DEFAULT);      //    *gain_code = shl (mult (gcode0, *p++), 1);

    VoC_bnez16_d(d_gain_code_M122_L1,GLOBAL_EFR_FLAG_ADDR)
    VoC_shr16_ri(REG2,-1,DEFAULT);
d_gain_code_M122_L1:
    VoC_lw16inc_p(REG7,REG0,DEFAULT);           //qua_ener_MR122 = *p++;
    VoC_lw16inc_p(REG6,REG0,DEFAULT);           //qua_ener = *p++;

    VoC_sw16_d(REG2,DEC_AMR_GAIN_CODE_ADDRESS);
    VoC_lw16i(REG5,STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR);
    VoC_jal(CII_gc_pred_update);                //gc_pred_update(pred_state, qua_ener_MR122, qua_ener);

    VoC_lw16i_short(INC1,1,DEFAULT);
//*  Function    : CII_d_gain_code_M122  end
// EFR DTX BEGIN
    VoC_bez16_d(d_gain_code_end_EFR,GLOBAL_EFR_FLAG_ADDR)
    VoC_jal(CII_efr_dtx_func7);


d_gain_code_end_EFR:
// EFR DTX END
    VoC_jump(Dec_amr_L45_122);                  //      }


Dec_amr_L44_122:
    VoC_push16(REG2,DEFAULT);               // push16 st->state
    VoC_jal(CII_ec_gain_code);              //      ec_gain_code (st->ec_gain_c_st, st->pred_state, st->state,&gain_code);

// EFR_DTX_BEGIN
    VoC_bez16_d(Dec_amr_L45_122_EFR,GLOBAL_EFR_FLAG_ADDR)
    VoC_jal(CII_efr_dtx_func8);
Dec_amr_L45_122_EFR:
// EFR_DTX_END
    VoC_pop16(REG2,DEFAULT);                //      }



Dec_amr_L45_122:


    VoC_pop16(REG1,DEFAULT);                //  pop16   st->pred_state
    VoC_lw16_d(REG6,DEC_AMR_GAIN_CODE_ADDRESS);
    VoC_jal(CII_ec_gain_code_update);           //      ec_gain_code_update (st->ec_gain_c_st, bfi, st->prev_bf,&gain_code);
    VoC_pop16(REG0,DEFAULT);                //  pop16  st->ec_gain_c_st

D_GAIN000:
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_lw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);     //      pit_sharp = gain_pit;
    // end of dec_amr_122_s4



// EFR DTX BEGIN

    VoC_bez16_d(D_GAIN000_EFR,GLOBAL_EFR_FLAG_ADDR)
    VoC_shr16_ri(REG6,-2,DEFAULT );             //pit_sharp = shl (pit_sharp, 2); EFR
    VoC_lw16i_short(REG7,-1,IN_PARALLEL);
D_GAIN000_EFR:
    VoC_shr16_ri(REG6,-1,DEFAULT );             //pit_sharp = shl (pit_sharp, 1);

// EFR DTX END
    VoC_lw16_d(REG2,DEC_AMR_GAIN_PIT_ADDRESS);          // gain_pit in reg2
    VoC_sw16_d(REG6,DEC_AMR_PIT_SHARP_ADDRESS);
    VoC_lw16i_set_inc(REG0,STRUCT_DECOD_AMRSTATE_EXC_ADDR,1);   //{

    VoC_bngt16_rd(Dec_amr_L52_122,REG6,CONST_0x00004000L_ADDR);     //if (sub (pit_sharp, 16384) > 0)
    VoC_multf16inc_rp(REG5,REG6,REG0,DEFAULT);
    VoC_lw16i_set_inc(REG1,DEC_AMR_EXCP_ADDRESS,1);
    VoC_multf32_rr(ACC0,REG5,REG2,DEFAULT);     //       L_temp = L_mult (temp, gain_pit);

    VoC_loop_i_short(40,DEFAULT);                   //   for (i = 0; i < L_SUBFR; i++)

    VoC_startloop0                      //    {
    //       temp = mult (st->exc[i], pit_sharp);
    //       if (sub(mode, MR122)==0)
    VoC_shr32_rr(ACC0,REG7,DEFAULT);                //       {  L_temp = L_shr (L_temp, 1);}
    VoC_multf16inc_rp(REG5,REG6,REG0,DEFAULT);
    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000_ADDR);      //       excp[i] = round (L_temp);
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
    VoC_add16_rd(REG0,REG3,CONST_1_ADDR);
    VoC_lw16i_short(REG6,8,DEFAULT);                   //   for (i = 0; i < 8; i++)
    VoC_jal(CII_R02R3_p_R6);                //   {   st->ltpGainHistory[i] = st->ltpGainHistory[i+1]; }
    VoC_sw16_p(REG2,REG3,DEFAULT);          //   st->ltpGainHistory[8] = gain_pit;
Dec_amr_L56_122:
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG0,DEFAULT);
    VoC_sw16_d(REG7,STRUCT_DECOD_AMRSTATE_SHARP_ADDR);
    VoC_jal(CII_Int_lsf);               //Int_lsf(prev_lsf, st->lsfState->past_lsf_q, i_subfr, lsf_i);


    VoC_lw32_d(REG67,STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR);
    VoC_add32_rd(REG67,REG67,CONST_0x00010001_ADDR);
    VoC_bgt16_rd(CGAver_L5_122,REG5,CONST_5325_ADDR);       //if (sub(diff, 5325) > 0)
    VoC_lw16i_short(REG7,0,DEFAULT);            //{ st->hangVar = add(st->hangVar, 1);}
CGAver_L5_122:                      // else{st->hangVar = 0; }
    VoC_pop16(REG5,DEFAULT);            //bgMix = 8192; cbGainMix = gain_code;
    VoC_bngt16_rd(CGAver_L6_122,REG7,CONST_10_ADDR);    //if (sub(st->hangVar, 10) > 0)
    VoC_lw16i_short(REG6,1,DEFAULT);            //{  st->hangCount = 0;
CGAver_L6_122:

    VoC_pop16(REG7,DEFAULT);
    VoC_sw32_d(REG67,STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR);
    //return cbGainMix;  ->REG5
    //  end of Cb_gain_average



    //   gain_code_mix = gain_code;      // MR74, MR795, MR122
    //}
    VoC_sw16_d(REG5,DEC_AMR_GAIN_CODE_MIX_ADDRESS);

    VoC_lw16_d(REG6,DEC_AMR_GAIN_PIT_ADDRESS);
    VoC_lw16i_short(REG7,3,DEFAULT);        //   tmp_shift = 3

    VoC_bnez16_d(CGAver_L6_newL,GLOBAL_EFR_FLAG_ADDR)
    VoC_shr16_ri(REG6,1,DEFAULT);               //{  pitch_fac = shr (gain_pit, 1);
    VoC_lw16i_short(REG7,2,DEFAULT);        //   tmp_shift = 2; }
CGAver_L6_newL:

    VoC_jal(CII_amr_dec_com_sub3);
    VoC_jal(CII_amr_dec_com_sub4);
    VoC_pop32(ACC0,DEFAULT);        //   pop32 stack32[n]

    VoC_jal(CII_amr_dec_com_sub5);


    VoC_be16_rd(Dec_amr_LoopEnd_1221_exit,REG5,CONST_160_ADDR);     //for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)

    VoC_jump(Dec_amr_LoopStart_122);
Dec_amr_LoopEnd_1221_exit:

    VoC_pop16(REG5,DEFAULT);        // pop16 stack[n-2]
    VoC_pop16(REG4,DEFAULT);        // pop16 stack[n-1]
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_lw16_sd(REG3,1,DEFAULT);
    VoC_return;

}
void CII_D_plsf_5_sub(void)
{
    VoC_lw16inc_p(REG0,REG1,DEFAULT);
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_push16(RA,DEFAULT);
    VoC_add16_rd(REG2,REG3,CONST_10_ADDR);
    VoC_jal(Coolsand_read_13bits);

    VoC_pop16(RA,DEFAULT);
    VoC_sw32_p(REG67,REG2,DEFAULT);         //lsf2_r[n+1] = *p_dico++;
    VoC_sw32inc_p(REG45,REG3,DEFAULT);      //lsf2_r[n]   = *p_dico++;

    VoC_return;
}



void Coolsand_read_13bits(void)
{
    VoC_shr16_ri(REG0,-2,DEFAULT);
    VoC_lw16i_short(REG4,0xf,DEFAULT);

    VoC_mult16_rd(REG0,REG0,CONST_13_ADDR);

    VoC_lw16i_short(BITCACHE,0,DEFAULT);

    VoC_and16_rr(REG4,REG0,DEFAULT);
    VoC_shr16_ri(REG0,4,IN_PARALLEL);

    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_lw32z(RL6,DEFAULT);

    VoC_lbinc_p(REG0);
    VoC_rbinc_r(REG4,REG4,DEFAULT);

    VoC_loop_i_short(2,DEFAULT)
    VoC_startloop0
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG6,13,DEFAULT);    //lsf1_r[n]   = *p_dico++;
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG7,13,DEFAULT);   //lsf1_r[n+1] = *p_dico++;

    VoC_shru32_ri(REG67,(-3),DEFAULT);  // create sign bit

    VoC_shr16_ri (REG6,(3),DEFAULT);
    VoC_shr16_ri (REG7,(3),IN_PARALLEL);

    VoC_movreg32(ACC0,REG67,DEFAULT);
    VoC_movreg32(REG45,ACC0,IN_PARALLEL);
    VoC_endloop0
    VoC_return;
}














