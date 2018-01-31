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


void CII_COD_AMR_122(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_push16(REG7,DEFAULT); // COD_AMR_ANAP_ADDR

    /* Autocorrelations */
    VoC_lw16i(REG6,COD_AMR_A_T_ADDRESS+11);

    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_P_WINDOW_12K2_ADDR,1);//p_window_mid address
    VoC_bez16_d(CII_lpc_M122_1,GLOBAL_EFR_FLAG_ADDR)        //   if (efr_flag == 0 )
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_P_WINDOW_12K2_ADDR+40,1);//p_window_mid address
CII_lpc_M122_1:

    VoC_lw16i_set_inc(REG1,TABLE_WINDOW_160_80_compressd_ADDR,1);//window_160_80 address

    VoC_push16(REG0,DEFAULT);
    VoC_sw16_d(REG6,PARA4_ADDR_ADDR);/*for calling Levinson*/

    VoC_jal(Coolsand_decompressed_windows);

    VoC_jal(CII_Autocorr);
    /* Lag windowing    */
    VoC_jal(CII_Lag_window);
    /* Levinson Durbin  */
    VoC_jal(CII_Levinson);

    /* Autocorrelations */
    VoC_lw16i(REG6,COD_AMR_A_T_ADDRESS+33);
    VoC_pop16(REG0,DEFAULT);

    VoC_lw16i_set_inc(REG1,TABLE_WINDOW_232_8_compressd_ADDR,1);//window_160_80 address
    VoC_sw16_d(REG6,PARA4_ADDR_ADDR);/*for calling Levinson*/

    VoC_lw32z(RL6,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);
    VoC_lw16i_set_inc(REG2,Windows_table_ADDR,1);
    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG4,16,DEFAULT);


    VoC_loop_i(0,232)
    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_endloop0

    VoC_loop_i_short(8,DEFAULT)
    VoC_startloop0
    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lbinc_p(REG1);
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG1,Windows_table_ADDR,1);

    VoC_jal(CII_Autocorr);
    /* Lag windowing    */
    VoC_jal(CII_Lag_window);
    /* Levinson Durbin  */
    VoC_jal(CII_Levinson);

    VoC_lw16i_short(REG0,1,DEFAULT);

    VoC_bez16_d(efr_encoder101,GLOBAL_EFR_FLAG_ADDR)
    VoC_sw16_d (REG0,GLOBAL_OUTPUT_Vad_ADDR);/*VAD_flag = 1;*/
    VoC_lw16i_short(REG5,3,DEFAULT ) ;           //   txdtx_ctrl = TX_VAD_FLAG | TX_SP_FLAG;
    VoC_bez16_d(efr_encoder102,GLOBAL_DTX_ADDR)     //   if (st->dtx)

    VoC_jal(CII_vad_computation);
    VoC_jal(CII_tx_dtx);

    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_and16_rr(REG0,REG5,DEFAULT);
efr_encoder102:
    VoC_sw16_d(REG5,GLOBAL_TXDTX_CTRL_ADDR);
    VoC_sw16_d(REG0,GLOBAL_OUTPUT_SP_ADDR);
efr_encoder101:

    VoC_jal(CII_lsp_122);











    /* From A(z) to lsp. LSP quantization and interpolation */
//   lsp(st->lspSt, mode, *usedMode, A_t, Aq_t, lsp_new, &ana);
    /* Buffer lsp's and energy */
    /*
    dtx_buffer(st->dtx_encSt,
          lsp_new,
          st->new_speech);
    */

//////////////////////////
    VoC_bnez16_d(cod_amr103,GLOBAL_EFR_FLAG_ADDR)
    VoC_jal(CII_dtx_buffer);
//  VoC_lw16_d(REG0,GLOBAL_ENC_USED_MODE_ADDR);
//  VoC_bne16_rd(cod_amr102_122,REG0,CONST_8_ADDR)

    VoC_lw16i_short(REG0,8,DEFAULT);
    VoC_bne16_rd(cod_amr102_122,REG0,GLOBAL_ENC_USED_MODE_ADDR)
    VoC_lw16_d(REG7,COD_AMR_COMPUTE_SID_FLAG_ADDRESS);
    VoC_jal(CII_dtx_enc);
    /*
           CII_dtx_enc2(st->dtx_encSt,
                  compute_sid_flag,
                  st->lspSt->qSt,
                  st->gainQuantSt->gc_predSt,
                  &ana);
    */
    VoC_jump(cod_amr103);
cod_amr102_122:
    /* check resonance in the filter */
//      lsp_flag = check_lsp(st->tonStabSt, st->lspSt->lsp_old);  move16 ();
    VoC_jal(CII_check_lsp);
cod_amr103:

    VoC_lw32z(REG67,DEFAULT);     // REG7 for i_subfr // REG6 for subfrnr

    VoC_lw16i_set_inc(REG0,COD_AMR_A_T_ADDRESS,1);
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_SPEECH_ADDR,1);
    VoC_lw16i_set_inc(REG2,STRUCT_COD_AMRSTATE_WSP_ADDR,1);
    VoC_bez16_d(cod_amr105, GLOBAL_EFR_FLAG_ADDR)
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR,1);
cod_amr105:

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_sw32_d(REG67,COD_AMR_SUBFRNR_ADDRESS);
    VoC_blt16_rd(cod_amr1041,REG6,CONST_4_ADDR)
    VoC_jump(cod_amr104);
cod_amr1041:/*
      pre_big(mode, gamma1, gamma1_12k2, gamma2, A_t, i_subfr, st->speech,
              st->mem_w, st->wsp);
*/
    VoC_lw16i_short(REG4,5,DEFAULT);
    VoC_push16(REG0,DEFAULT);
    VoC_push32(REG23,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,STATIC_CONST_F_GAMMA1_ADDR,1);
//PRE_BIG01:

    VoC_lw16i_set_inc(REG2,PRE_BIG_AP1_ADDR,1);
    VoC_jal(CII_Weight_Ai);
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16i_set_inc(REG1,STATIC_CONST_F_GAMMA2_ADDR,1);
    VoC_lw16i_set_inc(REG2,PRE_BIG_AP2_ADDR,1);
    VoC_jal(CII_Weight_Ai);
    VoC_lw16i_set_inc(REG0,PRE_BIG_AP1_ADDR,1);
    VoC_lw32_sd(REG23,0,DEFAULT);
    VoC_jal(CII_Residu_new);

    VoC_lw32_sd(REG23,0,DEFAULT);

    VoC_lw16i(REG5,PRE_BIG_AP2_ADDR);
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_MEM_W_ADDR,1);
    VoC_movreg16(REG3,REG2,DEFAULT);
    VoC_movreg16(REG7,REG2,IN_PARALLEL);
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

//////////////////////////////////////////
    VoC_lw16_d(REG6,COD_AMR_SUBFRNR_ADDRESS);
    VoC_be16_rd(cod_amr106,REG6,CONST_0_ADDR)
    VoC_be16_rd(cod_amr106,REG6,CONST_2_ADDR)
    /* Find open loop pitch lag for two subframes */
    VoC_jal(CII_Pitch_ol_M122);

    /*
             ol_ltp(st->pitchOLWghtSt, st->vadSt, mode, &st->wsp[i_subfr],
                    &T_op[subfrNr], st->old_lags, st->ol_gain_flg, subfrNr,
                    st->dtx);
    */
cod_amr106:
    VoC_lw16i(REG6,COD_AMR_T_OP_ADDRESS);
    VoC_lw16_d(REG7,COD_AMR_SUBFRNR_ADDRESS);
    VoC_shr16_ri(REG7,1,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_pop16(REG0,DEFAULT);
    VoC_pop32(REG23,IN_PARALLEL);
    VoC_sw16_p(REG4,REG6,DEFAULT);
    VoC_lw16i_short(REG4,11,DEFAULT);
    VoC_lw16i_short(REG5,40,IN_PARALLEL);
    VoC_add16_rr(REG2,REG2,REG5,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG5,IN_PARALLEL);
    VoC_lw32_d(REG67,COD_AMR_SUBFRNR_ADDRESS);
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);

    VoC_bne16_rd(cod_amr601,REG6,CONST_3_ADDR)
    VoC_add16_rd(REG7,REG7,CONST_80_ADDR);
cod_amr601:
    VoC_jump(cod_amr105);

cod_amr104:

    /* run VAD pitch detection */
    VoC_bez16_d(cod_amr110,GLOBAL_DTX_ADDR)
    VoC_bnez16_d(cod_amr801,GLOBAL_EFR_FLAG_ADDR)
    VoC_jal(CII_vad_pitch_detection);
    VoC_jump(cod_amr110);
cod_amr801:
    VoC_jal(CII_periodicity_update);
cod_amr110:

    VoC_lw16i_short(REG0,8,DEFAULT);
    VoC_bne16_rd(the_end1,REG0,GLOBAL_ENC_USED_MODE_ADDR)       // if (sub((Word16)*usedMode, (Word16)MRDTX) == 0)
    VoC_jump(the_end);
the_end1:
    /*------------------------------------------------------------------------*
    *          Loop for every subframe in the analysis frame                 *
    *------------------------------------------------------------------------*
    *  To find the pitch and innovation parameters. The subframe size is     *
    *  L_SUBFR and the loop is repeated L_FRAME/L_SUBFR times.               *
    *     - find the weighted LPC coefficients                               *
    *     - find the LPC residual signal res[]                               *
    *     - compute the target signal for pitch search                       *
    *     - compute impulse response of weighted synthesis filter (h1[])     *
    *     - find the closed-loop pitch parameters                            *
    *     - encode the pitch dealy                                           *
    *     - update the impulse response h1[] by including fixed-gain pitch   *
    *     - find target vector for codebook search                           *
    *     - codebook search                                                  *
    *     - encode codebook address                                          *
    *     - VQ of pitch and codebook gains                                   *
    *     - find synthesis speech                                            *
    *     - update states of weighting filter                                *
    *------------------------------------------------------------------------*/

    VoC_lw16i_short(REG7,0,DEFAULT);    // REG7 for loop i_subfr
    VoC_lw16i_short(REG6,1,DEFAULT);    // REG6 for loop evenSubfr
//  VoC_lw16i_short(REG5,0,IN_PARALLEL);    // REG5 for loop subfrNr
    VoC_lw16i(REG0,COD_AMR_A_T_ADDRESS);    // REG0 for A
    VoC_lw16i(REG1,COD_AMR_AQ_T_ADDRESS);   // REG1 for Aq
    VoC_push16(REG7,DEFAULT);
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG1,DEFAULT);
    VoC_push16(REG6,DEFAULT);
    VoC_push16(REG7,DEFAULT);
cod_amr112:


    VoC_blt16_rd(cod_amr1111,REG7,CONST_160_ADDR)
    /* Save states for the MR475 mode */
    VoC_jump(cod_amr111);
cod_amr1111:
    VoC_bez16_d(EFR_DTX_PART,GLOBAL_EFR_FLAG_ADDR)
    VoC_bnez16_d(EFR_DTX_PART,GLOBAL_OUTPUT_SP_ADDR)
    VoC_jal(CII_EFR_DTX_PART1);

    VoC_jump(EFR_DTX_PART1);
EFR_DTX_PART:
    /*-----------------------------------------------------------------*
       * - Preprocessing of subframe                                     *
       *-----------------------------------------------------------------*/
    /*         subframePreProc(*usedMode, gamma1, gamma1_12k2,
                             gamma2, A, Aq, &st->speech[i_subfr],
                             st->mem_err, st->mem_w0, st->zero,
                             st->ai_zero, &st->exc[i_subfr],
                             st->h1, xn, res, st->error);
    */
    VoC_lw16i(RL7_HI,STRUCT_COD_AMRSTATE_MEM_W0_ADDR);
    VoC_jal(CII_subframePreProc);
    /* copy the LP residual (res2 is modified in the CL LTP search)    */
    // Copy (res, res2, L_SUBFR);

    VoC_lw16_sd(REG7,0,DEFAULT);
    VoC_lw16i(REG5,STRUCT_COD_AMRSTATE_EXC_ADDR);
    VoC_add16_rr(REG7,REG7,REG5,DEFAULT);

    VoC_lw16i_short(REG5,0,DEFAULT);    // REG5 for delta_search=1;
    /*-----------------------------------------------------------------*
     * - Closed-loop LTP search                                        *
     *-----------------------------------------------------------------*/
    /*      cl_ltp(st->clLtpSt, st->tonStabSt, *usedMode, i_subfr, T_op, st->h1,
                 &st->exc[i_subfr], res2, xn, lsp_flag, xn2, y1,
                 &T0, &T0_frac, &gain_pit, gCoeff, &ana,
                 &gp_limit);*/

    VoC_sw16_d(REG7,COD_AMR_EXC_ADDR_ADDRESS);

    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//  begin of  CII_cl_ltp
    //*T0 = Pitch_fr(clSt->pitchSt,mode, T_op, exc, xn, h1, L_SUBFR, frameOffset,T0_frac, &resu3, &index);
    /*++++++++++++++++++++++++++++++++++++++++++++++++++*/
    //  begin of  CII_Pitch_fr

    VoC_lw16_sd(REG7,0,DEFAULT);        // REG7 for i_subfr
    VoC_lw16i_short(REG1,18,IN_PARALLEL);   // REG1 for pit_min

    VoC_lw16_d(REG0,COD_AMR_T_OP_ADDRESS);
//  VoC_lw32_d(REG23,STATIC_CONST_mode_dep_parm_MR122_ADDR+4);
    VoC_lw16i_short(REG2,3,DEFAULT);
    VoC_lw16i_short(REG3,6,IN_PARALLEL);


    VoC_bez16_r(AMR_PITCH_FR201,REG7)

    VoC_lw16_d(REG0,COD_AMR_T_OP_ADDRESS+1);

    VoC_be16_rd(AMR_PITCH_FR201,REG7,CONST_80_ADDR)
    VoC_lw16i_short(REG5,1,DEFAULT);    // REG5 for delta_search=0;
    VoC_lw16_d(REG0,STRUCT_PITCH_FRSTATE_T0_PREV_SUBFRAME_ADDR);
//  VoC_lw32_d(REG23,STATIC_CONST_mode_dep_parm_MR122_ADDR+6);

    VoC_lw16i_short(REG2,5,DEFAULT);
    VoC_lw16i_short(REG3,9,IN_PARALLEL);
AMR_PITCH_FR201:
    VoC_sub16_rr(REG4,REG0,REG2,DEFAULT);   //      *t0_min = sub(T0, delta_low);
    VoC_sw16_d(REG5,Pitch_delta_search_addr);// save delta_search
    /* REG0 for T0;REG23 for delta_frc_low,delta_frc_range; REG1 for pit_min; REG6 for PIT_MAX; REG45 for t0_min,t0_max */
    /*++++++++++++++++++++++++++++++++++++++++++++++++*/
    // begin of CII_getRange
    VoC_bgt16_rr(GET_RANGE1,REG4,REG1)      //      if (sub(*t0_min, pitmin) < 0) {
    VoC_movreg16(REG4,REG1,DEFAULT);    //      *t0_min = pitmin;                                  move16();
GET_RANGE1:
    VoC_add16_rr(REG5,REG4,REG3,DEFAULT);   //      *t0_max = add(*t0_min, delta_range);
    VoC_bngt16_rd(GET_RANGE2,REG5,CONST_143_ADDR)       //      if (sub(*t0_max, pitmax) > 0) {
    VoC_lw16i(REG5,143);                //      *t0_max = pitmax;                                  move16();
    VoC_sub16_rr(REG4,REG5,REG3,DEFAULT);   //      *t0_min = sub(*t0_max, delta_range);
GET_RANGE2:
    // end of CII_getRange
    /*++++++++++++++++++++++++++++++++++++++++++++++++*/
    /*-----------------------------------------------------------------------*
     *           Find interval to compute normalized correlation             *
     *-----------------------------------------------------------------------*/

    VoC_lw16i_set_inc(REG0,Pitch_fr6_corr_v_addr,2);
    VoC_sw32_d(REG45,Pitch_fr6_t0_min_addr);
    VoC_lw16i_short(REG2,4,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG2,DEFAULT);           //t_min = sub (t0_min, L_INTER_SRCH);
    VoC_add16_rr(REG5,REG5,REG2,IN_PARALLEL);       //t_max = add (t0_max, L_INTER_SRCH);

    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_H1_ADDR,-2); /*REG1=h[n]地址*/
    VoC_sub16_rr(REG2,REG0,REG4,DEFAULT);           //REG2 for address &corr_v[-t_min];
    VoC_push32(REG45,IN_PARALLEL);      // stack32 0 for t_min t_max
//    Norm_Corr (exc, xn, h, L_subfr, t_min, t_max, corr);
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    // begin of CII_Norm_Corr
    {

#if 0
        voc_short    PITCH_F6_EXCF_ADDR                        ,40,y
        voc_short    Pitch_fr6_max_loop_addr                   ,2 ,x

        voc_short   Pitch_fr6_corr_v_addr                     ,40,x            // can have negative offset

        voc_short    Pitch_fr6_t0_min_addr                      ,x
        voc_short    Pitch_fr6_t0_max_addr                      ,x
        voc_short    Pitch_delta_search_addr                    ,x

#endif

        VoC_sub16_dr(REG0,COD_AMR_EXC_ADDR_ADDRESS,REG4);   //k = -t_min;
        VoC_lw16i_set_inc(REG3,PITCH_F6_EXCF_ADDR,1);
        VoC_push16(REG2,DEFAULT);   // stack16 1 for corr_v[-t_min] address
        VoC_push16(REG0,DEFAULT);   // stack16 0 for exc[k] address
//    Convolve (&exc[k], h, excf, L_subfr);
        VoC_jal(CII_Convolve);
        VoC_lw32z(ACC0,DEFAULT);  //  s = 0;
        VoC_lw16i_set_inc(REG0,PITCH_F6_EXCF_ADDR,2);

        VoC_loop_i_short(20,DEFAULT)  //  for (j = 0; j < L_subfr; j++)
        VoC_startloop0
        VoC_bimac32inc_pp(REG0,REG0);   // s = L_mac (s, excf[j], excf[j]);
        VoC_endloop0

//  VoC_lw16i_short(REG2,0,DEFAULT);
//  VoC_lw16i(REG3,0x0400);


        VoC_lw16i_short(REG4,-3,DEFAULT);                               //-h_fac
        VoC_lw16i_short(REG5,0,IN_PARALLEL);        // scaling
        VoC_bngt32_rd(PITCH_F6_1001,ACC0,CONST_0x04000000_ADDR);

        /* scale "excf[]" to avoid overflow */
        VoC_lw16i_set_inc(REG0,PITCH_F6_EXCF_ADDR,2);           /* REG0 for excf[i] address */
        VoC_lw16i_set_inc(REG1,PITCH_F6_EXCF_ADDR,2);           /* REG1 for s_excf[i] address */
        VoC_lw16i_short(REG2,2,DEFAULT);
        VoC_lw16i_short(REG3,10,IN_PARALLEL);
        VoC_jal(CII_scale);

        VoC_lw16i_short(REG4,-1,DEFAULT);                           //-h_fac
        VoC_lw16i_short(REG5,2,DEFAULT);
PITCH_F6_1001:

//    for (j = 0; j < L_subfr; j++)
//        scaled_excf[j] = shr (excf[j], 2);
        VoC_lw16_sd(REG2,1,DEFAULT);        //REG2 for address &corr_v[-t_min];
        VoC_lw32_sd(REG67,0,DEFAULT);   // REG6 for t_min, REG7 for t_max
        VoC_add16_rr(REG3,REG2,REG7,DEFAULT);

        VoC_lw16i_short(INC2,1,DEFAULT);
        VoC_sw16_d(REG3,Pitch_fr6_max_loop_addr);       // for decide the last loop
        VoC_add16_rr(REG2,REG2,REG6,DEFAULT);       // REG2 for corr_norm[i] address
        VoC_lw16_sd(REG3,0,IN_PARALLEL);            // REG3 for exc[k] address
PITCH_F6_1002:  // begin loop
        VoC_lw32z(ACC0,DEFAULT);
        VoC_lw16i_set_inc(REG0,PITCH_F6_EXCF_ADDR,2);

        VoC_loop_i_short(20,DEFAULT)
        VoC_push32(REG45,IN_PARALLEL);  // -h_fac scaling
        VoC_startloop0
        VoC_bimac32inc_pp(REG0,REG0);
        VoC_endloop0
        VoC_push32(REG23,DEFAULT);      // save exc corr_v address
        VoC_movreg32(REG01,ACC0,DEFAULT);
        VoC_jal(CII_Inv_sqrt);
        VoC_movreg32(REG67,REG01,DEFAULT);

        /* Compute correlation between xn[] and excf[] */
        VoC_lw16i_set_inc(REG0,PITCH_F6_EXCF_ADDR,2);
        VoC_lw16i_set_inc(REG1,COD_AMR_XN_ADDRESS,2);
        VoC_loop_i_short(20,DEFAULT);
        VoC_lw32z(ACC0,IN_PARALLEL);
        VoC_startloop0
        VoC_bimac32inc_pp(REG0,REG1);
        VoC_endloop0

        VoC_shru16_ri(REG6,1,DEFAULT);     //nor_l

        VoC_movreg32(REG45,ACC0,DEFAULT);       //corr_h
        VoC_shru16_ri(REG4,1,DEFAULT);         //corr_l

//        s = Mpy_32 (corr_h, corr_l, norm_h, norm_l);
        VoC_multf32_rr(ACC0,REG7,REG5,DEFAULT);
        VoC_multf16_rr(REG0,REG7,REG4,IN_PARALLEL);

        VoC_multf16_rr(REG1,REG6,REG5,DEFAULT);
        VoC_lw16i_short(REG3,1,IN_PARALLEL);
        VoC_mac32_rr(REG0,REG3,DEFAULT);
        VoC_mac32_rr(REG1,REG3,DEFAULT);
        VoC_pop32(REG23,DEFAULT);       // REG3 for exc[k]
        VoC_shr32_ri(ACC0,-16,DEFAULT);

        VoC_lw16i_short(INC3,-1,DEFAULT);
//        corr_norm[i] = extract_h (L_shl (s, 16));
        VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);
        VoC_pop32(REG45,DEFAULT);   // -h_fac scaling

        VoC_bgt16_rd(PITCH_F6_1003,REG2,Pitch_fr6_max_loop_addr)
        VoC_inc_p(REG3,DEFAULT);      //      k--;
        VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_H1_ADDR+39,-1);
        VoC_lw16i_set_inc(REG1,PITCH_F6_EXCF_ADDR+38,1);
        VoC_lw16_p(REG6,REG3,DEFAULT);  // REG6 for exc[k]
        VoC_push32(REG23,DEFAULT);
        VoC_multf32inc_rp(REG23,REG6,REG0,DEFAULT);   //   s = L_mult (exc[k], h[j]);

        VoC_loop_i_short(39,DEFAULT)  //   for (j = L_subfr - 1; j > 0; j--)
        VoC_startloop0
        VoC_shr32_rr(REG23,REG4,DEFAULT);       //  s = L_shl (s, h_fac);
        VoC_add16inc_rp(REG3,REG3,REG1,DEFAULT);
        VoC_multf32inc_rp(REG23,REG6,REG0,DEFAULT);   //   s = L_mult (exc[k], h[j]);
        exit_on_warnings = OFF;
        VoC_sw16_p(REG3,REG1,DEFAULT);    //  s_excf[j] = add (extract_h (s), s_excf[j - 1]);
        exit_on_warnings = ON;
        VoC_sub16_rd(REG1,REG1,CONST_2_ADDR);
        VoC_endloop0

        VoC_shr16_rr(REG6,REG5,DEFAULT);
        VoC_pop32(REG23,DEFAULT);
        VoC_sw16_d(REG6,PITCH_F6_EXCF_ADDR);     //  s_excf[0] = shr (exc[k], scaling);
        VoC_jump(PITCH_F6_1002);
PITCH_F6_1003:
        // end of CII_Norm_Corr
        /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

        VoC_lw16i_short(INC0,1,DEFAULT);
        VoC_pop32(REG45,DEFAULT);
        VoC_pop16(REG2,IN_PARALLEL);
        /*-----------------------------------------------------------------------*
         *                           Find integer pitch                          *
         *-----------------------------------------------------------------------*/

        VoC_lw16_sd(REG0,0,DEFAULT);    //  corr[]

        VoC_lw32_d(REG45,Pitch_fr6_t0_min_addr);
        VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
        VoC_movreg16(REG1,REG4,DEFAULT);            //REG1 for lag   lag = t0_min;

        VoC_lw16inc_p(REG7,REG0,DEFAULT);           //REG2 for max
        VoC_sub16_rr(REG6,REG5,REG4,IN_PARALLEL);       //REG6 for loop number

        VoC_movreg16(REG2,REG7,DEFAULT);            //  max = corr[t0_min];
        VoC_lw16i_short(REG5,1,IN_PARALLEL);
        VoC_add16_rr(REG4,REG4,REG5,DEFAULT);

        VoC_bngtz16_r(AMR_PITCH_FR103,REG6)

        VoC_lw16inc_p(REG7,REG0,DEFAULT);
        VoC_loop_r_short(REG6,DEFAULT);     //  for (i = t0_min + 1; i <= t0_max; i++)
        VoC_startloop0
        VoC_sub16_rr(REG3,REG7,REG2,DEFAULT);
        VoC_bltz16_r(AMR_PITCH_FR104,REG3)          //   if (sub (corr[i], max) >= 0) {

        VoC_movreg16(REG2,REG7,DEFAULT);        // max = corr[i];
        VoC_movreg16(REG1,REG4,DEFAULT);    //     lag = i;
AMR_PITCH_FR104:
        VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
        VoC_lw16inc_p(REG7,REG0,DEFAULT);
        VoC_endloop0;
AMR_PITCH_FR103:
        /*-----------------------------------------------------------------------*
         *                        Find fractional pitch                          *
         *-----------------------------------------------------------------------*/
        VoC_lw16i_short(INC2,-1,DEFAULT);

        VoC_lw16i_short(RL6_LO,0,DEFAULT);  // frac = 0;        // RL6_LO for frac RL6_HI for last_frac
        VoC_lw16i_short(REG4,0x5e,IN_PARALLEL);

        VoC_bnez16_d(AMR_PITCH_FR105,Pitch_delta_search_addr)

        VoC_bgt16_rr(AMR_PITCH_FR106,REG1,REG4)     // if ((delta_search == 0) && (sub (lag, max_frac_lag) > 0))
AMR_PITCH_FR105:


        VoC_lw16i_short(REG5,0xfffd,DEFAULT);


//             searchFrac (&lag, &frac, last_frac, corr, flag3);
//          searchFrac (&lag, &frac, last_frac, corr, flag3);
        /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
        //   begin of CII_searchFrac
        VoC_lw16_sd(REG2,0,DEFAULT);

        VoC_add16_rr(REG2,REG1,REG2,DEFAULT);   //REG2 for corr[*lag] address    x1 = &x[0];
        VoC_push16(REG1,IN_PARALLEL);       //  REG1 ->lag

        VoC_lw16i_set_inc(REG3,STATIC_CONST_INTER_6_25_ADDR,1);     //&inter_6[0]

        VoC_lw16i(RL6_HI,0x8000);       // RL6_HI for max

        VoC_lw16i_short(INC0,6,DEFAULT);
        VoC_lw16i_short(INC1,6,IN_PARALLEL);

        VoC_lw16i_short(REG4,1,DEFAULT);

        VoC_loop_i(1,7)            //for (i = add (*frac); i <= last_frac; i++)

        VoC_movreg16(REG6,REG5,DEFAULT);
        VoC_push32(REG23,DEFAULT);
// corr_int = Interpol_3or6 (&corr[*lag], i, flag3);
//    begin of Interpol_3or6
        VoC_bnltz16_r(INTERPOL_3OR6_101,REG6)  //  if (frac < 0)
        VoC_add16_rd(REG6,REG6,CONST_6_ADDR);   //  frac = add (frac, UP_SAMP_MAX);
        VoC_sub16_rr(REG2,REG2,REG4,DEFAULT);   // x--;
INTERPOL_3OR6_101:
        VoC_add16_rr(REG0,REG3,REG6,DEFAULT);       // c1 = &inter_6[frac];
        VoC_sub16_rr(REG1,REG3,REG6,DEFAULT);
        VoC_add16_rd(REG1,REG1,CONST_6_ADDR);       // c2 = &inter_6[sub (UP_SAMP_MAX, frac)];
        VoC_add16_rr(REG3,REG2,REG4,DEFAULT);   // x2 = &x[1];
        VoC_loop_i_short(4,DEFAULT)
        VoC_lw32z(ACC0,IN_PARALLEL);
        VoC_startloop0
        VoC_mac32inc_pp(REG0,REG2,DEFAULT); //  s = L_mac (s, x1[-i], c1[k]);
        VoC_mac32inc_pp(REG1,REG3,DEFAULT); //  s = L_mac (s, x2[i], c2[k]);
        VoC_endloop0
//    end of Interpol_3or6
        VoC_movreg16(REG0,RL6_HI,DEFAULT);
        VoC_add32_rd(REG23,ACC0,CONST_0x00008000_ADDR);
        VoC_bngt16_rr(SEARCHFRAC101,REG3,REG0);        // if (sub (corr_int, max) > 0)
        VoC_movreg16(RL6_LO,REG5,DEFAULT);          // *frac = i;
        VoC_movreg16(RL6_HI,REG3,IN_PARALLEL);          // max = corr_int;
SEARCHFRAC101:
        VoC_pop32(REG23,DEFAULT);
        VoC_add16_rr(REG5,REG5,REG4,DEFAULT);       //i++
        VoC_endloop1


        VoC_pop16(REG1,DEFAULT);
        VoC_movreg16(REG0,RL6_LO,DEFAULT);  // laod frac

        VoC_bne16_rd(SEARCHFRAC104,REG0,(426+TABLE_DICO1_LSF_3_compressed_ADDR));    // CONST_neg3_ADDRif (sub (*frac, -3) == 0)
        VoC_lw16i_short(REG0,3,DEFAULT);    // *frac = 3;
        VoC_sub16_rr(REG1,REG1,REG4,IN_PARALLEL);// *lag = sub (*lag, 1);
SEARCHFRAC104:
        VoC_movreg16(RL6_LO,REG0,DEFAULT);
        //   end of CII_searchFrac
        /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
AMR_PITCH_FR106:
        /* REG1 for lag */
        VoC_pop16(REG2,DEFAULT);
//  VoC_jal(CII_Enc_lag6);
    }

    VoC_sub16_rd(REG3,REG1,Pitch_fr6_t0_min_addr);  // REG3 for i  i = sub (T0, T0_min);
    VoC_mult16_rd(REG3,REG3,CONST_6_ADDR);
    VoC_movreg16(REG7,RL6_LO,DEFAULT);      // REG7 for t0_frac
    VoC_add16_rd(REG3,REG3,CONST_3_ADDR);
    VoC_add16_rr(REG2,REG3,REG7,DEFAULT);           // index = add (add (add (i, i), 3), T0_frac);

    VoC_bnez16_d(ENC_LAG6_100,Pitch_delta_search_addr)

    VoC_add16_rd(REG2,REG1,CONST_368_ADDR);     // index = add (T0, 368);

    VoC_bgt16_rd(ENC_LAG6_100,REG1,(23+STATIC_CONST_SID_CODEWORD_BIT_IDX_ADDR))     // 94  if (sub (T0, 94) <= 0)
    VoC_add16_rr(REG3,REG1,REG1,DEFAULT);       // REG3 for i
    VoC_add16_rr(REG3,REG3,REG1,DEFAULT);       // i = add (add (T0, T0), T0);

    VoC_add16_rr(REG3,REG3,REG3,DEFAULT);
    VoC_sub16_rd(REG3,REG3,(33+STATIC_CONST_SID_CODEWORD_BIT_IDX_ADDR));   //105
    VoC_add16_rr(REG2,REG3,REG7,DEFAULT);       // index = add (sub (add (i, i), 105), T0_frac);


ENC_LAG6_100:

    VoC_lw16_sd(REG7,5,DEFAULT);//COD_AMR_ANAP_ADDR
    VoC_sw16_d(REG1,STRUCT_PITCH_FRSTATE_T0_PREV_SUBFRAME_ADDR);    //st->T0_prev_subframe = lag;
    VoC_sw16_p(REG2,REG7,DEFAULT);
    VoC_lw16i_short(REG2,1,DEFAULT);    //REG2 for flag3
    VoC_add16_rr(REG7,REG7,REG2,DEFAULT);

    VoC_sw16_d(REG1,COD_AMR_T0_ADDRESS);
    VoC_sw16_sd(REG7,5,DEFAULT);// COD_AMR_ANAP_ADDR

    VoC_sw16_d(RL6_LO,COD_AMR_T0_FRAC_ADDRESS);
//   *(*anap)++ = index;

    //  end of CII_Pitch_fr
    /*++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /*---------------------------------------------------------------------------
    Function:  void CII_Pred_lt_3or6(void)
    Word16 exc[],        in/out: REG5
    Word16 T0,           input : REG6
    Word16 frac,         input : REG7
    Word16 L_subfr,      input : subframe size
    Word16 flag3         input : REG2
    -----------------------------------------------------------------------------*/
    VoC_lw16_d(REG5,COD_AMR_EXC_ADDR_ADDRESS);
    VoC_lw32_d(REG67,COD_AMR_T0_ADDRESS);//get T0 addr REG6  get frac in REG7
    VoC_lw16i_short(REG2,0,DEFAULT);    // flag3 = 0
    VoC_jal(CII_Pred_lt_3or6);
    /* Convolve(exc, h1, y1, L_SUBFR); */
    VoC_lw16d_set_inc(REG0,COD_AMR_EXC_ADDR_ADDRESS,2);
    VoC_lw16i_set_inc(REG3,COD_AMR_Y1_ADDRESS,1);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_H1_ADDR,-2); /*REG7=h[n]地址*/
    VoC_jal(CII_Convolve);
    /* gain_pit is Q14 for all modes
    *gain_pit = G_pitch(mode, xn, y1, g_coeff, L_SUBFR); */
    VoC_jal(CII_G_pitch);
    //  REG2 for gain_pit
    /* check if the pitch gain should be limit due to resonance in LPC filter */
    VoC_lw16_d(REG0,COD_AMR_LSP_FLAG_ADDRESS);
    VoC_lw16i_short(REG7,0,DEFAULT);        // REG7 for  gpc_flag = 0;
    VoC_movreg16(RL7_LO,REG2,IN_PARALLEL);  // RL7_LO->gain_pit
    VoC_lw16i(RL7_HI,0x7FFF);       // RL7_HI->gp_limit                 // REG6 for *gp_limit = MAX_16;

    VoC_bnez16_d(cl_ltp1202,GLOBAL_EFR_FLAG_ADDR)
    VoC_bez16_d(cl_ltp101,COD_AMR_LSP_FLAG_ADDRESS)         //  if ((lsp_flag != 0)
    VoC_bngt16_rd(cl_ltp101,REG2,CONST_15565_ADDR);     // (sub(*gain_pit, GP_CLIP) > 0))

    /*       gpc_flag = check_gp_clipping(tonSt, *gain_pit);  move16 (); */
    VoC_shr16_ri(REG2,3,DEFAULT);       // sum = shr(g_pitch, 3);
    VoC_lw16i_set_inc(REG3,STRUCT_TONSTABSTATE_GP_ADDR,1);
    VoC_loop_i_short(7,DEFAULT)     // for (i = 0; i < N_FRAME; i++)
    VoC_startloop0
    VoC_add16inc_rp(REG2,REG2,REG3,DEFAULT);    //     sum = add(sum, st->gp[i]);
    VoC_endloop0

    VoC_bngt16_rd(cl_ltp101,REG2,CONST_15565_ADDR)              // if (sub(sum, GP_CLIP) > 0)
    VoC_lw16i_short(REG7,1,DEFAULT);// return 1;
cl_ltp101:
    /* REG7 for gpc_flag */
    /* special for the MR475, MR515 mode; limit the gain to 0.85 to */
    /* cope with bit errors in the decoder in a better way.         */

    VoC_bez16_r(cl_ltp202,REG7);                    // if (gpc_flag != 0)
    VoC_lw16i(RL7_LO,15565);    // // *gain_pit = GP_CLIP;
    VoC_movreg16(RL7_HI,RL7_LO,DEFAULT); // *gp_limit = GP_CLIP;
cl_ltp202:
    VoC_sw16_d(RL7_LO,COD_AMR_GAIN_PIT_ADDRESS);
cl_ltp1202:

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_sw16_d(RL7_HI,COD_AMR_GP_LIMIT_ADDRESS);


    VoC_lw16i_set_inc(REG0, STATIC_CONST_QUA_GAIN_PITCH_ADDR,1);
    VoC_lw16i_set_inc(REG2, 0, 1);
    VoC_lw16i(REG3, 0x7fff);

    VoC_loop_i_short(16, DEFAULT);   // for (i = 1; i < NB_QUA_PITCH; i++)   #define NB_QUA_PITCH  16

    VoC_startloop0
    VoC_lw16inc_p(REG1, REG0, DEFAULT);
    VoC_bgt16_rd(q_gain_pitch_110, REG1, COD_AMR_GP_LIMIT_ADDRESS)
    VoC_sub16_dr(REG1, COD_AMR_GAIN_PIT_ADDRESS, REG1);
    VoC_bgtz16_r(q_gain_pitch_111, REG1)
    VoC_sub16_dr(REG1, CONST_0_ADDR, REG1);
q_gain_pitch_111:
    //REG1:  err
    VoC_bngt16_rr(q_gain_pitch_110, REG3, REG1)
    VoC_movreg16(REG3, REG1, DEFAULT);
    VoC_movreg16(REG4, REG2, DEFAULT);
q_gain_pitch_110:
    VoC_inc_p(REG2, DEFAULT);
    //  VoC_lw16inc_p(REG1, REG0, IN_PARALLEL);
    VoC_endloop0

    //REG4  index
    VoC_lw16i_set_inc(REG0, STATIC_CONST_QUA_GAIN_PITCH_ADDR,1);
    VoC_add16_rr(REG1, REG0, REG4, DEFAULT);
    VoC_movreg16(REG3,REG4,  DEFAULT);
    //REG3   ii
    VoC_lw16inc_p(REG5, REG1, DEFAULT);

    VoC_and16_ri(REG5,0xFFFC);  // *gain = qua_gain_pitch[index] & 0xFFFC;
    exit_on_warnings = OFF;
    VoC_lw16_sd(REG7,5,DEFAULT);      //COD_AMR_ANAP_ADDR
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);
    VoC_sw16_p(REG4,REG7,DEFAULT);
    VoC_sw16_sd(REG7,5,DEFAULT);        //COD_AMR_ANAP_ADDR
    VoC_sw16_d(REG5, COD_AMR_GAIN_PIT_ADDRESS);
    exit_on_warnings = ON;

    /* update target vector und evaluate LTP residual */
    VoC_lw16_d(REG3,COD_AMR_GAIN_PIT_ADDRESS);
    VoC_lw16i_set_inc(REG0,COD_AMR_Y1_ADDRESS,2 );                  /* REG0 for y1[] first address */
    VoC_lw16i_set_inc(REG1,COD_AMR_XN_ADDRESS,2 );                  /* REG1 for xn[] first address */
    VoC_lw16i_set_inc(REG2,COD_AMR_XN2_ADDRESS,2 );             /* REG2 for xn2[] first address */

    VoC_loop_i(1,2)

    VoC_multf32inc_rp(REG45,REG3,REG0,DEFAULT);       // L_temp = L_mult(y1[i], *gain_pit);
    VoC_multf32inc_rp(REG67,REG3,REG0,IN_PARALLEL);

    VoC_loop_i_short(20,DEFAULT)                        /*for (i = 0; i < L_SUBFR; i++) */
    VoC_startloop0

    VoC_shr32_ri(REG45,-1,DEFAULT);                 // L_temp = L_shl(L_temp, 1);
    VoC_shr32_ri(REG67,-1,IN_PARALLEL);
    VoC_sub16inc_pr(REG4,REG1,REG5,DEFAULT);        // xn2[i] = sub(xn[i], extract_h(L_temp));
    VoC_sub16inc_pr(REG5,REG1,REG7,IN_PARALLEL);

    VoC_multf32inc_rp(REG45,REG3,REG0,DEFAULT);       // L_temp = L_mult(y1[i], *gain_pit);
    VoC_multf32inc_rp(REG67,REG3,REG0,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(REG45,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_lw16_d(REG0,COD_AMR_EXC_ADDR_ADDRESS);      /* REG0 for &exc[i_subfr] address */
    VoC_lw16i(REG1,COD_AMR_RES_ADDRESS);
    VoC_movreg16(REG2,REG1,DEFAULT);    /* REG1 and REG2 for res2[] first address */
    VoC_endloop1
//  end of CII_cl_ltp
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

    /* update LTP lag history
    if ((subfrNr == 0) && (st->ol_gain_flg[0] > 0))
    {
       st->old_lags[1] = T0;     move16 ();
    }
    */
    VoC_lw16_sd(REG6,4,DEFAULT);
    VoC_lw16i_set_inc(REG0, COD_AMR_XN2_ADDRESS,1);


    VoC_lw16_d(REG7,COD_AMR_T0_ADDRESS);
    VoC_bnez16_r(cod_amr116,REG6)
    VoC_bngtz16_d(cod_amr116,STRUCT_COD_AMRSTATE_OL_GAIN_FLG_ADDR)
    VoC_sw16_d(REG7,STRUCT_COD_AMRSTATE_OLD_LAGS_ADDR+1);
cod_amr116:

    /*  if ((sub(subfrNr, 3) == 0) && (st->ol_gain_flg[1] > 0))
          {
             st->old_lags[0] = T0;     move16 ();
          }
    */
    VoC_push16(REG0,DEFAULT);
    VoC_bne16_rd(cod_amr117,REG6,CONST_3_ADDR)
    VoC_bngtz16_d(cod_amr117,STRUCT_COD_AMRSTATE_OL_GAIN_FLG_ADDR+1)
    VoC_sw16_d(REG7,STRUCT_COD_AMRSTATE_OLD_LAGS_ADDR);
cod_amr117:
    /*-----------------------------------------------------------------*
     * - Inovative codebook search (find index and gain)               *
     *-----------------------------------------------------------------
    cbsearch(xn2, st->h1, T0, st->sharp, gain_pit, res2,
             code, y2, &ana, *usedMode, subfrNr);*/
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
// begin of function    CII_cbsearch);


    VoC_sw16_d(REG6,COD_AMR_SUBFRNR_ADDRESS);
    VoC_lw16i_set_inc(REG1, STRUCT_COD_AMRSTATE_H1_ADDR,1);
    VoC_lw16d_set_inc(REG2, STRUCT_COD_AMRSTATE_SHARP_ADDR,1);
    VoC_lw16d_set_inc(REG3, COD_AMR_GAIN_PIT_ADDRESS,1);
    VoC_lw16i(REG4, COD_AMR_RES_ADDRESS);
    VoC_lw16i(REG5, COD_AMR_CODE_ADDRESS);


    VoC_push16(REG1,DEFAULT);
    VoC_push16(REG2,DEFAULT);
    VoC_push16(REG3,DEFAULT);
    VoC_push16(REG4,DEFAULT);
    VoC_push16(REG5,DEFAULT);

    VoC_lw16i(REG0, COD_AMR_Y2_ADDRESS);
    VoC_lw16_d(REG1, COD_AMR_SUBFRNR_ADDRESS);

    VoC_lw16_sd(REG2,11,DEFAULT); //COD_AMR_ANAP_ADDR
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG1,DEFAULT);
    VoC_push16(REG2,DEFAULT);
    VoC_lw16_sd(REG0, 7, DEFAULT);          // st->h1
    VoC_lw16_sd(REG3,5, DEFAULT);         // gain_pit

    VoC_jal(CII_cbseach_subr1);
//   VoC_counter_stats(voc_counter);
    VoC_jal(CII_code_10i40_35bits);
//   VoC_counter_stats(voc_counter);

    VoC_lw16_sd(REG0, 3, DEFAULT);          // code
    VoC_lw16_sd(REG3,5, DEFAULT);         // st->sharp
    VoC_jal(CII_cbseach_subr1);
    VoC_jal(CII_gainQuant_M122);
//stack16:   mode,  xn2 ,st->h1, st->sharp,   gain_pit, res2,  code, y2, subfrNr, anap
    // if want to save anap, do here  (COD_AMR_ANAP_ADDR)

    VoC_bez16_d(EFR_1001, GLOBAL_EFR_FLAG_ADDR)
    VoC_lw16_d(REG3,COD_AMR_GAIN_CODE_ADDRESS);//REG3: new_gain_code
    VoC_jal(CII_update_gain_code_history_tx);
EFR_1001:
    VoC_loop_i_short(8,DEFAULT);
    VoC_startloop0
    VoC_pop16(REG0, DEFAULT);
    VoC_endloop0

    VoC_lw16_d(REG6,COD_AMR_GAIN_PIT_ADDRESS);

    VoC_sw16_sd(REG5,5,DEFAULT);

// end of function  CII_cbsearch);
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


    /* update gain history */
//      update_gp_clipping(st->tonStabSt, gain_pit);
//   Copy(&st->gp[1], &st->gp[0], N_FRAME-1);
//   st->gp[N_FRAME-1] = shr(g_pitch, 3);

    VoC_shr16_ri(REG6,3,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,STRUCT_TONSTABSTATE_GP_ADDR+1,1);
    VoC_lw16i_set_inc(REG0,STRUCT_TONSTABSTATE_GP_ADDR,1);

    VoC_loop_i_short(6,DEFAULT)
    VoC_lw16inc_p(REG7,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_sw16_p(REG6,REG0,DEFAULT);

EFR_DTX_PART1:
    /* Subframe Post Porcessing
         subframePostProc(st->speech, *usedMode, i_subfr, gain_pit,
                          gain_code, Aq, synth, xn, code, y1, y2, st->mem_syn,
                          st->mem_err, st->mem_w0, st->exc, &st->sharp);*/
    VoC_lw16i_short(REG0,0,DEFAULT);    // 0 for not sf
    VoC_lw16i(RL7_LO,STRUCT_COD_AMRSTATE_MEM_W0_ADDR);
    VoC_lw16i(RL7_HI,STRUCT_COD_AMRSTATE_MEM_SYN_ADDR);

    VoC_jal(CII_subframePostProc);

    VoC_lw16_sd(REG7,0,DEFAULT);    // i_subfr
    VoC_lw16i_short(REG0,40,IN_PARALLEL);
    VoC_add16_rr(REG7,REG7,REG0,DEFAULT);
    VoC_lw16_sd(REG5,2,IN_PARALLEL);    // A_Q
    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_lw16_sd(REG6,1,IN_PARALLEL);    // evensb
    VoC_sub16_rr(REG6,REG1,REG6,DEFAULT);// evenSubfr = sub(1, evenSubfr);
    VoC_sw16_sd(REG7,0,DEFAULT);    // i_subfr
    VoC_lw16i_short(REG2,11,IN_PARALLEL);
    VoC_sw16_sd(REG6,1,DEFAULT);    // evensb
    VoC_add16_rr(REG5,REG5,REG2,IN_PARALLEL);
    VoC_lw16_sd(REG4,3,DEFAULT);    // A
    VoC_sw16_sd(REG5,2,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG2,IN_PARALLEL);
    VoC_lw16_sd(REG3,4,DEFAULT);    // subNr     // subfrNr = add(subfrNr, 1);
    VoC_add16_rr(REG3,REG3,REG1,DEFAULT);
    VoC_sw16_sd(REG4,3,DEFAULT);
    VoC_sw16_sd(REG3,4,DEFAULT);

    VoC_jump(cod_amr112);
cod_amr111:
    VoC_loop_i_short(5,DEFAULT)
    VoC_startloop0
    VoC_pop16(REG7,DEFAULT);
    VoC_endloop0

//   Copy(&st->old_exc[L_FRAME], &st->old_exc[0], PIT_MAX + L_INTERPOL);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_OLD_EXC_ADDR+160,2);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_OLD_EXC_ADDR,2);
    exit_on_warnings = OFF;
    VoC_lw32inc_p(RL7,REG0,DEFAULT);
    VoC_loop_i(1,77)
    VoC_lw32inc_p(RL7,REG0,DEFAULT);
    VoC_sw32inc_p(RL7,REG1,DEFAULT);
    VoC_endloop1
    exit_on_warnings = ON;

the_end:
    VoC_bez16_d(EFR_END,GLOBAL_EFR_FLAG_ADDR)
    VoC_jal(CII_PRM2_BITS_POST_PROCESS_EFR);
    VoC_jump(EFR_END1);
EFR_END:
    VoC_jal(CII_PRM2_BITS_POST_PROCESS);
EFR_END1:
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;


}



/*************************************************************************
 *
 *  FUNCTION:  code_10i40_35bits()
 *
 *  PURPOSE:  Searches a 35 bit algebraic codebook containing 10 pulses
 *            in a frame of 40 samples.
 *
 *  DESCRIPTION:
 *    The code contains 10 nonzero pulses: i0...i9.
 *    All pulses can have two possible amplitudes: +1 or -1.
 *    The 40 positions in a subframe are divided into 5 tracks of
 *    interleaved positions. Each track contains two pulses.
 *    The pulses can have the following possible positions:
 *
 *       i0, i5 :  0, 5, 10, 15, 20, 25, 30, 35.
 *       i1, i6 :  1, 6, 11, 16, 21, 26, 31, 36.
 *       i2, i7 :  2, 7, 12, 17, 22, 27, 32, 37.
 *       i3, i8 :  3, 8, 13, 18, 23, 28, 33, 38.
 *       i4, i9 :  4, 9, 14, 19, 24, 29, 34, 39.
 *
 *    Each pair of pulses require 1 bit for their signs and 6 bits for their
 *    positions (3 bits + 3 bits). This results in a 35 bit codebook.
 *    The function determines the optimal pulse signs and positions, builds
 *    the codevector, and computes the filtered codevector.
 *
 *************************************************************************/

void CII_code_10i40_35bits (void)
{


#if 0

    voc_short   C1035PF_DN_ADDR                             ,40,y        //[40]shorts
    voc_short   C1035PF_RR_DIAG_ADDR                        ,40,y        //[40]shorts
    voc_short   C1035PF_RR_SIDE_ADDR                        ,780,y       //[780]shorts

#endif


    VoC_push16(RA,DEFAULT);
//stack16:   RA,  xn2 ,st->h1, st->sharp,   gain_pit, res2,  code, y2, subfrNr, anap,  RA

    VoC_lw16i_short(ACC1_LO,2, DEFAULT);              //ACC1_LO  : sf
    VoC_lw16i_short(REG4,5, DEFAULT);          // REG4  :nb_track & step
    VoC_lw16i_short(REG5,8, DEFAULT);                    //REG5  : 40/step        // incr0 and  incr1 =1
    VoC_jal(CII_cor_h_x);                                         //    cor_h_x (h, x, dn, 2);


    {
#if 0

        voc_short SET_SIGN_EN_ADDR            ,230,x
        voc_short  C417PF_DN2_ADDR            ,40,x
#endif
    }

//  VoC_jal(CII_set_sign12k2_122);                          //    set_sign12k2 (dn, cn, sign, pos_max, NB_TRACK, ipos, STEP);


//stack16:   RA,  xn2 ,st->h1, st->sharp,   gain_pit, res2,  code, y2, subfrNr, anap,  RA

    /* calculate energy for normalization of cn[] and dn[] */


    VoC_lw16_sd(REG0, 5, DEFAULT);                    // incr0 = 2           /* REG2 for cn[] first address */

    VoC_loop_i(1,2)

    VoC_lw32_d(ACC0,CONST_0x00000100_ADDR);     /* ACC0 for s  s = 256 */
    VoC_loop_i_short(20,DEFAULT);                       /*for (i = 0; i < L_CODE; i++)*/
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG0);               /*s = L_mac (s, cn[i], cn[i]);*/
    VoC_endloop0
    VoC_movreg16(REG4, REG1, DEFAULT);
    VoC_movreg32(REG01,ACC0,DEFAULT);
    VoC_jal(CII_Inv_sqrt);

    VoC_shr32_ri(REG01,-5,DEFAULT);             /* L_shl (s, 5) */
    VoC_lw16i_set_inc(REG0,C1035PF_DN_ADDR,2);

    VoC_endloop1

    VoC_movreg16(REG5, REG1, DEFAULT);
    VoC_movreg16(REG2, REG0, IN_PARALLEL);
    //REG2    C1035PF_DN_ADDR
    VoC_lw32_d(ACC1,CONST_0x80017fff_ADDR);

    VoC_lw16i_set_inc(REG0,C1035PF_SIGN_ADDR-1,1);                   /* REG0 for sign[-1]*/


    VoC_lw16_sd(REG3, 5, DEFAULT);               // RES2     incr3 =1    /* REG3 for cn[] first address */
    VoC_lw16i(REG1,SET_SIGN_EN_ADDR);               /* REG1 for en[] first address */

    //for 0 and -32767

    VoC_lw32_d(RL6,CONST_0x00008000_ADDR);

    VoC_lw16i_short(FORMAT32,-11,DEFAULT);

    VoC_loop_i_short(40,DEFAULT);                       /*    for (i = 0; i < L_CODE; i++)  */
    VoC_movreg32(ACC0, RL6,IN_PARALLEL);
    VoC_startloop0
    VoC_mac32inc_rp(REG4,REG3,DEFAULT);
    VoC_inc_p(REG0,DEFAULT);

    VoC_mac32_rp(REG5,REG2,DEFAULT);

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_sw16_p(ACC1_LO,REG0,IN_PARALLEL);

    VoC_movreg16(REG7, ACC0_HI,DEFAULT);
    VoC_movreg32(ACC0, RL6,IN_PARALLEL);

    VoC_bnltz16_r(SET_SIGN101,REG7)

    VoC_sub16_rr(REG7,REG6,REG7,DEFAULT);                   /*cor = negate (cor);*/
    /* modify dn[] according to the fixed sign */
    VoC_sub16_rp(REG6,REG6,REG2,IN_PARALLEL);               /*val = negate (val);*/
    VoC_sw16_p(ACC1_HI,REG0,DEFAULT);           /*sign[i] = -32767; */
    VoC_sw16_p(REG6,REG2,DEFAULT);
SET_SIGN101:
    VoC_sw16inc_p(REG7,REG1,DEFAULT);           /* en[i] = cor;*/
    VoC_inc_p(REG2,DEFAULT);
    VoC_endloop0


    VoC_lw16i_short(FORMAT32,-1,DEFAULT);

//RL7_LO :  nb_track   RL7_HI  :   40/step

    VoC_lw16i_short(REG0,0,DEFAULT);                     /* REG0 for i */
    VoC_lw16i_set_inc(REG1,C1035PF_POS_MAX_ADDR,1);               /* REG1:&pas_max[] */


    VoC_lw16i_short(INC2,5,DEFAULT);
    //  VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_lw16i_short(REG4,-1,DEFAULT);                                    /* REG4 for max_of_all*/
    //REG4   max_of_all

    VoC_loop_i(1,5)
    VoC_lw16i(REG2, SET_SIGN_EN_ADDR);
    VoC_add16_rr(REG2,REG2,REG0,DEFAULT);/* REG2 for &en[j]*/

    VoC_loop_i_short(8,DEFAULT);
    VoC_lw16i_short(REG5,-1,IN_PARALLEL);                                /* REG5 for max max = -1; */
    //REG5   max
    VoC_startloop0
    VoC_lw16_p(REG6,REG2,DEFAULT);           /* REG6 for cor   cor = en[j];  */
    VoC_bngt16_rr(SET_SIGN107,REG6,REG5)             /* if (val <= 0)*/
    /* max = cor; */
    VoC_lw16i(REG3, SET_SIGN_EN_ADDR);
    VoC_movreg16(REG5,REG6,DEFAULT);
    // REG3 for j
    VoC_sub16_rr(REG3,REG2,REG3,IN_PARALLEL);    //pos=j+5
SET_SIGN107:
    VoC_inc_p(REG2, DEFAULT);
    VoC_endloop0


    VoC_bngt16_rr(SET_SIGN104,REG5,REG4)              /* if (val <= 0)*/
    VoC_movreg16(REG4,REG5,DEFAULT);            /*max_of_all = max;  */
    /* starting position for i0 */
    /* ipos[0] = i;  */
    VoC_movreg16(REG7,REG0,IN_PARALLEL);
SET_SIGN104:

    /* store maximum correlation position */
    VoC_inc_p(REG0,DEFAULT);                    //i++
    VoC_sw16inc_p(REG3,REG1,DEFAULT);               /*pos_max[i] = pos;  */
    VoC_endloop1

    /*----------------------------------------------------------------*
     *     Set starting position of each pulse.                       *
     *----------------------------------------------------------------*/
    VoC_lw16i_set_inc(REG1,C1035PF_IPOS_ADDR,1);


    VoC_lw16i_short(REG4,5,DEFAULT);            //NB_TRACK   STEP
    /* ipos[5] = pos;   */

    /* REG1 for ipos[1] */
    VoC_loop_i_short(5,DEFAULT)                         /* for (i = 1; i < NB_TRACK; i++)*/
    VoC_add16_rr(REG3, REG1, REG4, IN_PARALLEL);
    VoC_startloop0

    VoC_sw16inc_p(REG7,REG1,DEFAULT);           /*ipos[i] = pos;   */
    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);               /* pos = add (pos, 1);*/
    /* if (sub (pos, NB_TRACK) >= 0) */
    VoC_bgt16_rr(SET_SIGN106,REG4,REG7)
    VoC_lw16i_short(REG7,0,DEFAULT);                            /*pos = 0;                  */
SET_SIGN106:
    VoC_NOP();
    VoC_endloop0




    VoC_jal(CII_cor_h);                           //    cor_h (h, sign, rr);
    VoC_jal(CII_search_M122);

    {

#if 0

        voc_short build_code_sign_ADDR            ,10,x

#endif
    }


    //   build_code (codvec, sign, cod, h, y, indx);
//  VoC_jal(CII_build_code_M122);

    //stack16:   RA,  xn2 ,st->h1, st->sharp,   gain_pit, res2,  code, y2, subfrNr, anap,  RA



    VoC_lw16_sd(REG0, 4, DEFAULT);              // code
    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw32z(ACC0,DEFAULT);

    VoC_loop_i_short(20,DEFAULT);
    VoC_lw16i_short(REG2,-1,IN_PARALLEL);
    VoC_startloop0
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i(ACC1_LO,C1035PF_SIGN_ADDR);

    VoC_lw16_sd(REG3, 1, DEFAULT);              // anap    incr1 =1
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_loop_i_short(5,DEFAULT);
    VoC_lw16_sd(ACC1_HI, 4, IN_PARALLEL);           // code
    VoC_startloop0
    VoC_sw16inc_p(REG2,REG3,DEFAULT);
    VoC_endloop0


    VoC_lw32_d(RL6, CONST_0x2000e000_ADDR);

    VoC_lw16i_set_inc(REG0,C1035PF_CODVEC_ADDR,1);
    VoC_lw16i_set_inc(REG1,build_code_sign_ADDR,1);

    VoC_loop_i(0,10)

    VoC_lw16inc_p(REG4, REG0,DEFAULT);      //REG4    i
    VoC_multf16_rd(REG6, REG4, CONST_0x199a_ADDR);  // 6554 ,REG6   index
    VoC_lw16i(REG7,4096);
    VoC_multf32_rd(REG23,REG6,CONST_5_ADDR);
    VoC_sw16_p(RL6_HI, REG1,DEFAULT);
    VoC_shr32_ri(REG23, 1, DEFAULT);
    VoC_sub16_rr(REG5,REG4, REG2, DEFAULT);     //REG5   track
    VoC_movreg32(REG23, ACC1, IN_PARALLEL);
    VoC_add16_rr(REG2, REG2,REG4, DEFAULT);         //REG2    cbsearch_dn_sign_ADDR
    VoC_add16_rr(REG3, REG3,REG4, DEFAULT);     //REG3     COD_AMR_CODE_ADDRESS


    VoC_lw16_p(REG2, REG2, DEFAULT);        //REG2       j

    VoC_add16_rp(REG7,REG7, REG3, DEFAULT);
    VoC_bgtz16_r(build_code_M122_100_end, REG2)
    VoC_sub16_rd(REG7, REG7, CONST_0x2000_ADDR);    // 2*4096
    VoC_sw16_p(RL6_LO, REG1,DEFAULT);
    VoC_add16_rd(REG6, REG6, CONST_8_ADDR);
build_code_M122_100_end:
    VoC_lw16_sd(REG2, 1, DEFAULT);              // anap
    VoC_inc_p(REG1,DEFAULT);
    VoC_sw16_p(REG7, REG3,DEFAULT);
    VoC_add16_rr(REG2, REG2, REG5,IN_PARALLEL);     //REG2->  indx[track]
    VoC_add16_rd(REG3, REG2, CONST_5_ADDR);     //REG3 ->  indx[track + 5]
    VoC_lw16_p(REG7, REG2, DEFAULT);
    VoC_bltz16_r(build_code_M122_200_end, REG7);
    VoC_movreg16(REG5, REG6, DEFAULT);
    VoC_xor16_rr(REG5, REG7, DEFAULT);
    VoC_sub16_rr(REG4, REG7, REG6, IN_PARALLEL);    //REG4  (sub (indx[track], index)
    VoC_and16_ri(REG5, 8);                      //REG5     (index ^ indx[track]) & 8)
    VoC_bnez16_r(build_code_M122_300, REG5);
    VoC_bgtz16_r(build_code_M122_400, REG4);
build_code_M122_500:
    VoC_movreg16(REG2, REG3,DEFAULT);
    VoC_jump(build_code_M122_200_end);
build_code_M122_300:

    VoC_lw16i_short(REG4,7,DEFAULT);
    VoC_lw16i_short(REG5,7,DEFAULT);
    VoC_and32_rr(REG45,REG67,DEFAULT);

    VoC_sub16_rr(REG4, REG5, REG4, DEFAULT);         //REG4    (sub ((indx[track] & 7), (index & 7))
    VoC_bgtz16_r(build_code_M122_500, REG4);
build_code_M122_400:
    VoC_sw16_p(REG7, REG3,DEFAULT);
build_code_M122_200_end:
    VoC_sw16_p(REG6, REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16_sd(REG7, 8, DEFAULT);           //st->h1
    VoC_lw16_sd(REG0, 3, DEFAULT);         // Y2
    VoC_lw16i_short(REG6,10,IN_PARALLEL);
    /**********************************************
      Function CII_build_code_com2
      input: REG6->NB_PULSE
         REG7->h[]
         REG0(incr=1)->y[]
      Note: This function can used in all mode
    **********************************************/
    VoC_jal(CII_build_code_com2);

    VoC_lw16i_set_inc(REG2,STATIC_CONST_GRAY_ADDR, 1);

    VoC_lw16_sd(REG0,1,DEFAULT);      //  ANAP
    VoC_lw16i_short(REG1, 0, DEFAULT);   //incr1=1

    VoC_loop_i_short(10, DEFAULT);
    VoC_lw16i_short(REG5,8, IN_PARALLEL);
    VoC_startloop0
    VoC_lw16_p(REG7, REG0, DEFAULT);
    VoC_lw16i_short(REG6, 7, IN_PARALLEL);

    VoC_and16_rr(REG6, REG7,DEFAULT);
    VoC_add16_rr(REG6, REG2, REG6, DEFAULT);

    VoC_and16_rr(REG7, REG5,DEFAULT);

    VoC_lw16_p(REG6, REG6, DEFAULT);
    VoC_or16_rr(REG7, REG6, DEFAULT);
    VoC_bnlt16_rd(q_p_100, REG1, CONST_5_ADDR)
    VoC_movreg16( REG6,REG7, DEFAULT);
q_p_100:
    VoC_inc_p(REG1,DEFAULT);
    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    VoC_endloop0

    VoC_pop16(RA,DEFAULT);
    VoC_sw16_sd(REG0,0,DEFAULT);         //  REstore ANAP
    VoC_return;
}




void CII_search_M122(void)  //CII_search_M102
{


#if 0

    voc_short SEARCH_10I40_RRV_ADDR                     ,40,x        //[40]shorts
    voc_short SEARCH_CODE_CP_RR_ADDR                    ,320,x       //[320]shorts
    voc_short SEARCH_CODE_PS0_TEMP_ADDR                 ,x       //1 short
    voc_short SEARCH_10I40_PS0_ADDR                     ,x       //1 short
    voc_short C1035PF_IPOS_ADDR                         ,10,x        //[10]shorts
    voc_short C1035PF_POS_MAX_ADDR                      ,10,x        //[10]shorts
    voc_short C1035PF_CODVEC_ADDR                       ,10,x        //[10]shorts
    voc_short C1035PF_SIGN_ADDR                         ,40,x        //[40]shorts

#endif


//  search_count++;
    /*my pointers*/
    VoC_push16(RA,DEFAULT);


    VoC_lw16i_set_inc(REG1,SEARCH_CODE_CP_RR_ADDR,1);

    VoC_lw16i(REG4,C1035PF_RR_SIDE_ADDR );
    VoC_lw16i(REG5,C1035PF_RR_DIAG_ADDR  );

    VoC_lw16i_set_inc(REG2,C1035PF_DN_ADDR,1);          //&rr[0] and &dn[0]

    VoC_sw32_d(REG45,C1035PF_RR_SIDE_ADDR_ADDR);
    VoC_movreg16(REG0, REG5, DEFAULT);


    VoC_lw16i_set_inc(REG3,SEARCH_10I40_RRV_ADDR,1);
    VoC_sw32_d(REG01,C1035PF_RR_DIAG_ADDR_2_ADDR);

    /*end of my pointers*/

    /* fix i0 on maximum of correlation position */

    VoC_lw16i(REG0,C1035PF_POS_MAX_ADDR);
    VoC_add16_rd(REG0,REG0,C1035PF_IPOS_ADDR);

    VoC_sw32_d(REG23,SEARCH_CODE_DN_ADDR_ADDR);             //&cp_rr[0] and &rrv[0]


    VoC_lw16_p(REG2,REG0,DEFAULT);          // REG2 value of pos_max[ipos[0]]

//  SEARCH_CODE_ALP0_TEMP_ADDR  SEARCH_CODE_PS0_TEMP_ADDR
    /*ps0_temp:dn[i0]; alp0:L_mult (rr[i0][i0], _1_16)+0x8000;*/
    VoC_add16_rd(REG0,REG2,SEARCH_CODE_DN_ADDR_ADDR);   //&dn[i0]

    VoC_sw16_d(REG2,SEARCH_10I40_I0_ADDR);      //i0 = pos_max[ipos[0]];

    VoC_add16_rd(REG1,REG2,C1035PF_RR_DIAG_ADDR_ADDR);  //&rr[i0][i0]

    VoC_lw16i(REG6,_1_16);
    VoC_multf32_rp(ACC0,REG6,REG1,DEFAULT);

    VoC_lw16_p(REG6,REG0,DEFAULT);                                          //dn[i0]


    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR,8);
    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000_ADDR);

    VoC_sw16_d(REG6,SEARCH_CODE_PS0_TEMP_ADDR);
    VoC_sw32_d(ACC0,SEARCH_CODE_ALP0_TEMP_ADDR);        //  rr[i0][i0] *  _1_16  + 0x00008000

    /*copy rr[i0] to cp_rr*/


    VoC_jal(CII_SEARCH_COPY);

    /*------------------------------------------------------------------*
     * i1 loop:                                                         *
     *------------------------------------------------------------------*/

    VoC_lw16i_short(REG4,-1,DEFAULT);
    VoC_lw16i_short(REG2,0,DEFAULT);

    VoC_lw16i_set_inc(REG1,C1035PF_CODVEC_ADDR,1); //REG1 addr of codvec[0]

    VoC_loop_i_short(10,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_startloop0
    VoC_sw16inc_p(REG2,REG1,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_endloop0

    VoC_lw16i_short(REG2,5,DEFAULT);

    VoC_sw32_d(REG45,SEARCH_10I40_PSK_ADDR);  //alpk = 1   //psk =-1


SEARCH_10I40_LOOP:

    VoC_lw16i(REG3,C1035PF_POS_MAX_ADDR); //REG2 addr of pos_max[0]
    VoC_add16_rd(REG3,REG3,C1035PF_IPOS_ADDR+1);//REG2 addr of pos_max[ipos[1]]

    /*copy rr[i1] to cp_rr*/

    VoC_push16(REG2,DEFAULT);

    VoC_lw16_p(REG2,REG3,DEFAULT);                      // REG2 value of pos_max[ipos[1]]
    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR+1,8);
    VoC_sw16_d(REG2,SEARCH_10I40_I1_ADDR);              //i1 = pos_max[ipos[1]];
    VoC_jal(CII_SEARCH_COPY);


    /*compute rrv*/
    VoC_lw32_d(REG67,CONST_D_1_4_ADDR);     //reg3:&double_1_4

    VoC_lw16i_short(REG4,1,DEFAULT);            //REG4:loop num
    VoC_lw16i_short(REG3,38,IN_PARALLEL);
    VoC_lw16i(REG5,_1_8);                   //reg5:_1_8
    VoC_lw16d_set_inc(REG0,(C1035PF_IPOS_ADDR+3),5);            //reg6:ipos[3]


    VoC_jal(CII_COMPUTE_RRV_M122);

    /*ps0 = add (dn[i0], dn[i1]);*/
    VoC_lw16d_set_inc(REG1,SEARCH_10I40_I1_ADDR,1);     //reg1:i1

    VoC_add16_rd(REG0,REG1,SEARCH_CODE_DN_ADDR_ADDR);   //REG0:&dn[i1]
    VoC_mult16_rd(REG1,REG1,CONST_8_ADDR);
    VoC_add16_pd(REG7,REG0,SEARCH_CODE_PS0_TEMP_ADDR);
    VoC_add16_rd(REG1,REG1,SEARCH_CODE_CP_RR_ADDR_ADDR);    //reg1:&cp_rr[i0][i1]
    VoC_lw32_d(ACC0,SEARCH_CODE_ALP0_TEMP_ADDR);
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);         // reg5=_1_8
    VoC_lw16inc_p(REG2,REG1,IN_PARALLEL);
    VoC_mac32_rd(REG2,CONST_D_1_16_ADDR);



    VoC_sw16_d(REG7,SEARCH_10I40_PS0_ADDR);

    /*compute alp1*/
    VoC_lw32_d(REG45,CONST_D_1_8_ADDR);     //reg3:&double_1_8

    VoC_lw16i_short(REG2,1,DEFAULT);            //reg2:loop num

    VoC_push32(ACC0,DEFAULT);               //alp0+0x8000
    VoC_lw16i_short(REG3,42,IN_PARALLEL);

    VoC_lw16_d(REG6,C1035PF_IPOS_ADDR+2);           //reg6:i2
    VoC_lw16i(REG7,_1_16);                  //reg7:_1_16

    VoC_jal(CII_COMPUTE_ALP1_M122);

    /* Default value */
//  VoC_lw16i(REG0,0x4000);
//  VoC_lw16i(REG1,_1_8);

    VoC_lw32_d(REG01,CONST_1_8_0x4000_ADDR);

    VoC_lw32_d(RL7,C1035PF_IPOS_ADDR+2);        //rl7:  ia and ib
    VoC_jal(CII_SEARCH_LOOP_M122);

    VoC_sw32_d(RL7,SEARCH_10I40_I2_ADDR);

    VoC_sw16_d(RL6_LO,SEARCH_10I40_PS0_ADDR);
    VoC_push32(ACC0,DEFAULT);               //alp0+0x8000


    /*copy rr[i2],rr[i3] to rr_cp*/
    VoC_movreg16(REG2, RL7_LO,IN_PARALLEL);
    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR+2,8); //reg2:dest addr
    VoC_jal(CII_SEARCH_COPY);

    VoC_movreg16(REG2, RL7_HI,DEFAULT);

    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR+3,8); //reg2:dest addr
    VoC_jal(CII_SEARCH_COPY);

    /*compute rrv*/
    VoC_lw32_d(REG67,CONST_D_1_4_ADDR);     //reg3:&double_1_4
    VoC_lw16i_short(REG4,2,DEFAULT);            //REG4:loop num
    VoC_lw16i_short(REG3,36,IN_PARALLEL);

    VoC_lw16i(REG5,_1_8);                   //reg5:_1_8
    VoC_lw16d_set_inc(REG0,(C1035PF_IPOS_ADDR+5),5);            //reg6:ipos[5]

    VoC_jal(CII_COMPUTE_RRV_M122);


    /*compute alp1*/

    VoC_lw32_d(REG45,CONST_D_1_16_ADDR);        //reg3:&double_1_16

    VoC_lw16i_short(REG2,2,DEFAULT);            //reg2:loop num
    VoC_lw16i_short(REG3,44,IN_PARALLEL);

    VoC_lw16_d(REG6,C1035PF_IPOS_ADDR+4);           //reg6:i4
    VoC_lw16i(REG7,_1_32);                  //reg7:_1_32
    VoC_jal(CII_COMPUTE_ALP1_M122);

    /* Default value */
//  VoC_lw16i(REG0,_1_4);
//  VoC_lw16i(REG1,_1_16);
    VoC_lw32_d(REG01,CONST_1_16_1_4_ADDR);
    VoC_lw32_d(RL7,C1035PF_IPOS_ADDR+4);            //rl7:      ia and ib
    VoC_jal(CII_SEARCH_LOOP_M122);

    VoC_sw32_d(RL7,SEARCH_M74_TRACK_ADDR);
    VoC_sw16_d(RL6_LO,SEARCH_10I40_PS0_ADDR);
    VoC_push32(ACC0,DEFAULT);               //alp0+0x8000

    /*copy rr[i4],rr[i5] to rr_cp*/

    VoC_movreg16(REG2, RL7_LO,IN_PARALLEL);
    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR+4,8); //reg2:dest addr
    VoC_jal(CII_SEARCH_COPY);

    VoC_movreg16(REG2, RL7_HI,DEFAULT);
    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR+5,8); //reg2:dest addr
    VoC_jal(CII_SEARCH_COPY);

    /*compute rrv*/
    VoC_lw32_d(REG67,CONST_D_1_8_ADDR);     //reg3:&double_1_8

    VoC_lw16i_short(REG4,3,DEFAULT);            //REG4:loop num
    VoC_lw16i_short(REG3,34,IN_PARALLEL);
    VoC_lw16i(REG5,_1_16);                  //reg5:_1_16
    VoC_lw16d_set_inc(REG0,(C1035PF_IPOS_ADDR+7),5);            //reg6:ipos[7]

    VoC_jal(CII_COMPUTE_RRV_M122);

    /*compute alp1*/

    VoC_lw32_d(REG45,CONST_D_1_32_ADDR);        //reg3:&double_1_32

    VoC_lw16i_short(REG2,3,DEFAULT);            //reg2:loop num
    VoC_lw16i_short(REG3,46,IN_PARALLEL);

    VoC_lw16_d(REG6,C1035PF_IPOS_ADDR+6);           //reg6:i6
    VoC_lw16i(REG7,_1_64);                  //reg7:_1_64
    VoC_jal(CII_COMPUTE_ALP1_M122);

    /* Default value */
//  VoC_lw16i(REG0,_1_4);
//  VoC_lw16i(REG1,_1_32);
    VoC_lw32_d(REG01,CONST_1_32_1_4_ADDR);
    VoC_lw32_d(RL7,C1035PF_IPOS_ADDR+6);            //rl7:      ia and ib
    VoC_jal(CII_SEARCH_LOOP_M122);

    VoC_sw32_d(RL7,SEARCH_10I40_I6_ADDR);
    /* now finished searching a set of 8 pulses */

    VoC_sw16_d(RL6_LO,SEARCH_10I40_PS0_ADDR);
    VoC_push32(ACC0,DEFAULT);               //alp0+0x8000

    /*copy rr[i6],rr[i7] to rr_cp*/
    VoC_movreg16(REG2, RL7_LO,IN_PARALLEL);
    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR+6,8); //reg2:dest addr
    VoC_jal(CII_SEARCH_COPY);

    VoC_movreg16(REG2, RL7_HI,DEFAULT);
    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR+7,8); //reg2:dest addr
    VoC_jal(CII_SEARCH_COPY);
    /*compute rrv*/
    VoC_lw32_d(REG67,CONST_D_1_8_ADDR);     //reg3:&double_1_8

    VoC_lw16i_short(REG4,4,DEFAULT);            //REG4:loop num
    VoC_lw16i_short(REG3,32,IN_PARALLEL);

    VoC_lw16i(REG5,_1_16);                  //reg5:_1_16
    VoC_lw16d_set_inc(REG0,(C1035PF_IPOS_ADDR+9),5);            //reg6:ipos[9]


    VoC_jal(CII_COMPUTE_RRV_M122);

    /*compute alp1*/

    VoC_lw32_d(REG45,CONST_D_1_64_ADDR);        //reg3:&double_1_64

    VoC_lw16i_short(REG2,4,DEFAULT);            //reg2:loop num
    VoC_lw16i_short(REG3,48,IN_PARALLEL);

    VoC_lw16_d(REG6,C1035PF_IPOS_ADDR+8);           //reg6:i8
    VoC_lw16i(REG7,_1_128);                 //reg7:_1_128
    VoC_jal(CII_COMPUTE_ALP1_M122);

    /* Default value */
//  VoC_lw16i(REG0,_1_8);
//  VoC_lw16i(REG1,_1_64);
    VoC_lw32_d(REG01,CONST_1_64_1_8_ADDR);
    VoC_lw32_d(RL7,C1035PF_IPOS_ADDR+8);            //rl7:      ia and ib
    VoC_jal(CII_SEARCH_LOOP_M122);

    /*----------------------------------------------------------------*
    * memorise codevector if this one is better than the last one.   *
    *----------------------------------------------------------------*/

    VoC_multf32_rd(ACC0,REG6,SEARCH_10I40_ALPK_ADDR);
    VoC_msu32_rd(REG7,SEARCH_10I40_PSK_ADDR);

    VoC_sw32_d(RL7,SEARCH_10I40_I8_ADDR);

    VoC_bngtz32_r(SEARCH_CODE_NOMEMORISE,ACC0)
    VoC_sw32_d(REG67,SEARCH_10I40_PSK_ADDR);    //PSK  ALPK
    VoC_lw16i_set_inc(REG0,SEARCH_10I40_I0_ADDR,2);
    VoC_lw16i_set_inc(REG1,C1035PF_CODVEC_ADDR,2);
    VoC_loop_i_short(5,DEFAULT)
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
SEARCH_CODE_NOMEMORISE:
    /*----------------------------------------------------------------*
     * Cyclic permutation of i1,i2,i3,i4,i5,i6,i7,(i8 and i9).          *
     *----------------------------------------------------------------*/
    VoC_lw16i_set_inc(REG1,C1035PF_IPOS_ADDR+1,1);
    VoC_lw16i_set_inc(REG0,C1035PF_IPOS_ADDR+2,1);

    VoC_lw16_p(REG6,REG1,DEFAULT);  //ipos[1]

    VoC_loop_i_short(8,DEFAULT);
    VoC_lw16inc_p(REG7,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_sw16_p(REG6,REG1,DEFAULT);
    VoC_pop16(REG2,DEFAULT);
    VoC_sub16_rd(REG2, REG2, CONST_1_ADDR);
    VoC_bngt16_rd(SEARCH_10I40_LOOP1,REG2,CONST_1_ADDR);
    VoC_jump(SEARCH_10I40_LOOP);
SEARCH_10I40_LOOP1:

    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_return;
}


/*need reg3,reg4,reg5,reg6,rl6_hi,*/
void CII_COMPUTE_RRV_M122(void) //COMPUTE_RRV_M102
{

    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_mult16_rd(REG1,REG0,CONST_8_ADDR);
    VoC_add16_rd(REG2,REG0,SEARCH_CODE_RRV_ADDR_ADDR);       //reg2:&rrv[i3]
    VoC_add32_rd(REG01,REG01,C1035PF_RR_DIAG_ADDR_2_ADDR);

//    VoC_lw16i_short(INC0,5,DEFAULT);
    //reg0:&rr[i3][i3],inc 205;reg1:&cp_rr[i0][i3]

    //at least 4 instructions in the loop
    VoC_lw32_d(ACC0,CONST_0x00008000_ADDR);

    VoC_mac32inc_rp(REG5,REG0,DEFAULT);
    VoC_lw16i_short(INC2,5,IN_PARALLEL);
    VoC_loop_i(1,8)

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_startloop0
    VoC_bimac32inc_rp(REG67,REG1);
    VoC_endloop0

    VoC_add16_rr(REG1,REG1,REG3,DEFAULT);

    VoC_lw32_d(ACC0,CONST_0x00008000_ADDR);
    exit_on_warnings=OFF;
    VoC_mac32inc_rp(REG5,REG0,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG2,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop1
    VoC_return
}



/*need reg2,reg3,reg5,reg6,reg7,RL6(stack32)*/
/*after popping alp1 a more pop32 is needed*/
void CII_COMPUTE_ALP1_M122(void)    //COMPUTE_ALP1_M102
{

    VoC_pop32(RL6,DEFAULT);                //rl6:alp0+0x8000
    VoC_lw16i_set_inc(REG0, C1035PF_RR_DIAG_ADDR+35, -5);
    VoC_add16_rr(REG0,REG6,REG0,DEFAULT);

    VoC_mult16_rd(REG6,REG6,CONST_8_ADDR);
    VoC_lw16i_set_inc(REG1, SEARCH_CODE_CP_RR_ADDR+35*8, 2);

    VoC_add16_rr(REG1,REG6,REG1,DEFAULT);

    VoC_loop_i(1,8)
    VoC_loop_r_short(REG2,DEFAULT);
    VoC_multf32inc_rp(ACC0,REG7,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_bimac32inc_rp(REG45,REG1);
    VoC_endloop0
    VoC_push32(RL7,DEFAULT);

    VoC_sub16_rr(REG1,REG1,REG3,DEFAULT);
    VoC_add32_rr(RL7,ACC0,RL6,IN_PARALLEL);
    VoC_endloop1
    VoC_NOP();
    VoC_push32(RL7,DEFAULT);
    VoC_return

}




//output:
//r4:ps;rl_7:ia and ib;acc0:new alp0
void CII_SEARCH_LOOP_M122(void)
{


#if 0

    voc_short   SEARCH_CODE_RRV_COE_ADDR                    ,y       //1 short
    voc_short   SEARCH_CODE_RR_COE_ADDR                     ,y       //1 short
    voc_short   SEARCH_10I40_I_ADDR                         ,2,y         //[2]shorts
    voc_short   SEARCH_10I40_PSK_ADDR                       ,y       //1 short
    voc_short   SEARCH_10I40_ALPK_ADDR                      ,y       //1 short
    voc_short   C1035PF_RR_DIAG_ADDR_2_ADDR                 ,y       //1 short
    voc_short   SEARCH_CODE_CP_RR_ADDR_ADDR                 ,y       //1 short
    voc_short   SEARCH_CODE_DN_ADDR_ADDR                    ,y       //1 short
    voc_short   SEARCH_CODE_RRV_ADDR_ADDR                   ,y       //1 short
    voc_short   C1035PF_RR_SIDE_ADDR_ADDR                   ,y       //1 short
    voc_short   C1035PF_RR_DIAG_ADDR_ADDR                   ,y       //1 short
    voc_short   SEARCH_CODE_CP_RR_7_ADDR_ADDR               ,y       //1 short
    voc_short   RR_SIDE_DIAG_TEMP_ADDR                      ,y       //1 short
    voc_short   SEARCH_10I40_I2_TEMP_ADDR                   ,y       //1 short
    voc_short   SEARCH_10I40_I2I3_TEMP_ADDR                 ,y       //1 short
    voc_short   SEARCH_CODE_DN_RRV_ADDR_ADDR                ,2,y         //[2]shorts
    voc_short   SEARCH_CODE_ALP0_TEMP_ADDR                  ,2,y         //[2]shorts
    voc_short   SEARCH_10I40_I0_ADDR                        ,y       //1 short
    voc_short   SEARCH_10I40_I1_ADDR                        ,y       //1 short
    voc_short   SEARCH_10I40_I2_ADDR                        ,y       //1 short
    voc_short   SEARCH_10I40_I3_ADDR                        ,y       //1 short
    voc_short   SEARCH_M74_TRACK_ADDR                       ,y       //1 short
    voc_short   SEARCH_M67_TRACK2_ADDR                      ,y       //1 short
    voc_short   SEARCH_10I40_I6_ADDR                        ,y       //1 short
    voc_short   SEARCH_10I40_I7_ADDR                        ,y       //1 short
    voc_short   SEARCH_10I40_I8_ADDR                        ,y       //1 short
    voc_short   SEARCH_10I40_I9_ADDR                        ,y       //1 short
    voc_short   linear_signs_ADDR                           ,4,y         //[4]shorts
    voc_short   linear_codewords_ADDR                       ,4,y         //[4]shorts
#endif



    VoC_movreg32(REG23, RL7,DEFAULT);
    VoC_movreg16(REG0,RL7_HI,DEFAULT);
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw32_d(REG01,SEARCH_CODE_RRV_COE_ADDR);
    exit_on_warnings=ON;

    VoC_movreg16(REG1,RL7_HI,DEFAULT);
    VoC_add32_rd(REG01,REG01,SEARCH_CODE_DN_ADDR_ADDR);


    VoC_lw16i_short(REG6,-1,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);        //reg67:    sq  and alp

    VoC_sw32_d(REG01,SEARCH_CODE_DN_RRV_ADDR_ADDR);     //reg01:&dn[i3] and &rrv[i3]

    VoC_loop_i(1,8)
    VoC_lw16i_short(INC0,5,DEFAULT);
    VoC_lw16i_set_inc(REG3, SEARCH_CODE_CP_RR_ADDR+7,8);
    VoC_add16_rd(REG5,REG2,SEARCH_CODE_DN_ADDR_ADDR);            // REG3  :  &dn[i2]

    VoC_sw16_d(REG2,SEARCH_10I40_I2_TEMP_ADDR);

//  copy the  rr[i2][]  to  cp_rr[][7] , and use  cp_rr[][7] to address  rr[i2][i3]


/////////////////////////////////////////////////////
// IN:
//     REG3:dest addr
//     REG2:i2
//     REG4:ipos[3]
//     INCR1 =-1
//     INCR3 =1
// OUT:
//     NONE
// REG USED:
//     REG0,REG1,REG2,REG3,REG4,REG5
// /////////////////////////////////////////////////
    // INCR0=5


    // REG0 for i2*i2
    VoC_mult16_rr(REG0, REG2, REG2,DEFAULT);
    // REG1 for i2
    VoC_movreg16(REG1, REG2,DEFAULT);

    // REG0 for i2*(i2-1)
    VoC_sub16_rr(REG0, REG0, REG2,DEFAULT);

    // REG0 for i2*(i2-1)/2
    VoC_shr16_ri(REG0,1,DEFAULT);
    VoC_push16(REG5,IN_PARALLEL);

    // REG0 for &rr_side[i2-1][0](rr[i2][0])
    // REG1 for &rr_diag[i2]
    VoC_add32_rd(REG01, REG01, C1035PF_RR_SIDE_ADDR_ADDR);

    // REG4 for &rr_side[i2-1][0]+i2
    VoC_add16_rr(REG4,REG0,REG2,DEFAULT);

    // REG0 for &rr_side[i2-1][ipos(3)]                 (rr[i2][ipos(3)]

    VoC_lw16i(REG5,SEARCH_10I40_RRV_ADDR);

    VoC_sub16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_lw16i_short(REG5,5,IN_PARALLEL);

    VoC_add16_rd(REG0,REG0,SEARCH_CODE_DN_RRV_ADDR_ADDR+1);   //  SEARCH_CODE_DN_RRV_ADDR_ADDR+1  is  ipos(3)

    // REG4 for &rr_side[i2-1][0]+i2+5
    // prepare for comparison because the REG0 will be pre-incremented 5
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    // REG5 for  &rr_diag[i2]
    VoC_movreg16(REG5,REG1,IN_PARALLEL);

    // REG2 loop count
    VoC_lw16i_short(REG1,8,DEFAULT);
    // REG4 for rr_side[i2-1][ipos(3)]
    // REG0 for &rr_side[i2-1][ipos(3)]+5
    VoC_lw16inc_p(REG4, REG0,IN_PARALLEL);

    exit_on_warnings=OFF;
    // save &rr_side[i2-1][0]+i2 to RR_SIDE_DIAG_TEMP_ADDR
    VoC_sw16_d(REG4,RR_SIDE_DIAG_TEMP_ADDR);
    exit_on_warnings=ON;

    // jump if i2<=i3 (&rr_side[i2-1][0]+i2<=&rr_side[i2-1][ipos(3)])

    VoC_bnlt16_rd(SEARCH_COPY_label1, REG0, RR_SIDE_DIAG_TEMP_ADDR)

    // loop 8 time, maybe break if i3>=i2

    VoC_loop_i_short(8,DEFAULT);
    VoC_startloop0;
    // jump if i2<=i3 (&rr_side[i2-1][0]+i2+5<=&rr_side[i2-1][ipos(3)]+5)

    VoC_bnlt16_rd(SEARCH_COPY_label1, REG0, RR_SIDE_DIAG_TEMP_ADDR)

    VoC_sw16inc_p(REG4, REG3,DEFAULT);
    // loop count - 1
    VoC_inc_p(REG1,DEFAULT);
    // REG0 is incremented 5(STEP) each time
    VoC_lw16inc_p(REG4, REG0,IN_PARALLEL);
    VoC_endloop0
SEARCH_COPY_label1:
    // REG2 is incremented by 25

    VoC_lw16i_short(INC2,25,DEFAULT);
    // REG4 for i3-i2
    VoC_sub16_rd(REG4,REG0,RR_SIDE_DIAG_TEMP_ADDR);
    // jump if i2!=i3 (&rr_side[i2-1][0]+i2+5<=&rr_side[i2-1][ipos(3)]+5)
    VoC_bne16_rd( SEARCH_COPY_label2,REG0,RR_SIDE_DIAG_TEMP_ADDR);
    // REG4 fo rr_diag[i2]
    VoC_lw16_p(REG5, REG5,DEFAULT);
    // loop count -1
    VoC_inc_p(REG1,DEFAULT);
    VoC_sw16inc_p(REG5, REG3,DEFAULT);


SEARCH_COPY_label2:


    // REG4 for i3
    VoC_add16_rr(REG4,REG4,REG2,DEFAULT);


    // REG0 for i3*i3
    VoC_mult16_rr(REG0,REG4,REG4,DEFAULT);
    VoC_lw16i_short(REG5,5,DEFAULT);
    // REG0 for i3*(i3-1)

    VoC_sub16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_lw16i_short(INC1,5,IN_PARALLEL);
    VoC_bez16_r(SEARCH_COPY_label_end, REG1)
    // REG0 for (i3*i3-1)/2
    VoC_shr16_ri(REG0,1,DEFAULT);
    // REG0 for & rr_side+(i3*i3-1)/2
    VoC_add16_rd(REG0, REG0, C1035PF_RR_SIDE_ADDR_ADDR);

    // REG0 for & rr_side+i3*(i3-1)/2+i2
    VoC_add16_rr(REG0,REG0,REG2,DEFAULT);
    // REG2 for i3*5
    VoC_mult16_rr(REG2,REG4,REG5,DEFAULT);

    // REG4 for rr_side[i2+5][i3] (rr[i2+5][i3])
    VoC_lw16_p(REG4, REG0,DEFAULT);
    // REG2 for i3*5+10
    VoC_add16_rd(REG2,REG2,CONST_10_ADDR);


    // REG0 for &rr_side[i2+10][i3]
    VoC_add16_rr(REG0, REG0, REG2,DEFAULT);
    // continue loop to 8 times

    VoC_loop_r_short(REG1,DEFAULT)
    // REG2 for 5*i3+10
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16_p(REG4, REG0,DEFAULT);
    VoC_add16_rr(REG0, REG0, REG2,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4, REG3,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop0;

SEARCH_COPY_label_end:

///copy the  rr[i2][]  to  cp_rr[][7] , and use  cp_rr[][7] to address  rr[i2][i3] end


    //alp1
    VoC_pop16(REG5,DEFAULT);

    VoC_lw32_d(REG01,SEARCH_CODE_DN_RRV_ADDR_ADDR); //reg01:&dn[i3] and &rrv[i3]

    VoC_add16_pd(REG3,REG5,SEARCH_10I40_PS0_ADDR);  //REG3:ps1

    VoC_lw16i_set_inc(REG2, SEARCH_CODE_CP_RR_ADDR+7,8);       //  &rr[i2][i3]   at   CP_RR



    VoC_loop_i_short(8,DEFAULT)
    VoC_lw32_sd(ACC0,0,IN_PARALLEL);
    VoC_startloop0

    VoC_mac32_pd(REG1,SEARCH_CODE_RRV_COE_ADDR);
    VoC_mac32inc_pd(REG2,SEARCH_CODE_RR_COE_ADDR);

    VoC_add16inc_rp(REG4,REG3,REG0,DEFAULT);
    VoC_movreg16(RL6_HI,REG4,DEFAULT);

    VoC_multf16_rr(REG4,REG4,REG4,DEFAULT);
    VoC_movreg16(REG5,ACC0_HI,DEFAULT);

    VoC_multf32_rr(ACC1,REG6,REG5,DEFAULT);
    VoC_multf32_rr(ACC0,REG7,REG4,IN_PARALLEL);
    VoC_NOP();

    VoC_bngt32_rr(SEARCH_CODE_I3_END,ACC0,ACC1)
    VoC_push16(REG1,DEFAULT);
    VoC_lw16i(REG7,SEARCH_10I40_RRV_ADDR);
    VoC_sub16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_movreg32(REG67,REG45,DEFAULT);
    VoC_movreg16(RL7_HI,REG1,IN_PARALLEL);
    VoC_pop16(REG1,DEFAULT);
    VoC_lw16_d(RL7_LO,SEARCH_10I40_I2_TEMP_ADDR);
    VoC_movreg16(RL6_LO,RL6_HI, DEFAULT);

SEARCH_CODE_I3_END:

    VoC_inc_p(REG1,DEFAULT);
    VoC_lw32_sd(ACC0,0,DEFAULT);
    VoC_endloop0

    VoC_pop32(ACC1,DEFAULT);

    VoC_lw16i_short(REG2,5,DEFAULT);
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);
    VoC_add16_rd(REG2, REG2,SEARCH_10I40_I2_TEMP_ADDR);

    VoC_endloop1

    VoC_multf32_rd(ACC0,REG7,CONST_0x00004000L_ADDR);
    VoC_pop32(REG01,DEFAULT);
    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000_ADDR);

    //RL6_LO:ps;rl_7:ia and ib;acc0:new alp0+0x8000
    VoC_return

}

/***************************************
  Function CII_lsp_M122
  input:
     &lsp_new_q[]/&st->lsp_old_q[]   : push32(n+3)
    &az[]/&azQ[]            : push32(n+2)
    &anap[]/&st->qSt->past_rq[] : push32(n+1)
    &lsp_new[]          : push16(n+2)
    &st->lsp_old[]          : push16(n+1)
  Optimized by Kenneth   09/20/2004
***************************************/
void CII_lsp_M122(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16i_set_inc(REG1,LSP_LSP_MID_ADDR,-1);
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_push16(REG1,DEFAULT);   // push16(n-1)  &lsp_mid[]
    VoC_jal(CII_Az_lsp);

    VoC_lw16_sd(REG0,0,DEFAULT);

    VoC_lw16_sd(REG1,3,DEFAULT);
    VoC_lw16i_short(REG6,22,IN_PARALLEL);

    VoC_jal(CII_Az_lsp);


    //Int_lpc_1and3_2 (st->lsp_old, lsp_mid, lsp_new, az);
    VoC_lw16_sd(REG2,0,DEFAULT);
    VoC_lw32_sd(REG01,1,DEFAULT);
    VoC_lw16_sd(REG3,2,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_lw16_sd(REG5,3,DEFAULT);
    VoC_movreg16(REG0,REG1,IN_PARALLEL);
    VoC_jal(CII_Int_lpc_1and3);

    VoC_lw16i_short(REG7,8,DEFAULT);

    VoC_be16_rd(lsp_M122_DTX_L1,REG7,GLOBAL_ENC_USED_MODE_ADDR);

    VoC_lw16_sd(REG0,0,DEFAULT);        // load &lsp_mid[]
    VoC_lw16i(REG6,LSP_LSP_MID_Q_ADDR);
    VoC_lw32_sd(REG23,2,DEFAULT);   // load reg3->&lsp_new_q[]
    VoC_push16(REG6,DEFAULT);       // push16(n-2) lsp_mid_q
    VoC_jal(CII_Q_plsf_5);

    VoC_lw32_sd(REG45,0,DEFAULT);
//  VoC_lw16i_short(REG7,5,IN_PARALLEL);  move to the end of CII_Q_plsf_5
    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);   //  (*anap) += add(0,5);

    //Int_lpc_1and3 (st->lsp_old_q, lsp_mid_q, lsp_new_q, azQ);
    VoC_lw32_sd(REG01,1,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_sw16_sd(REG4,6,DEFAULT);  //COD_AMR_ANAP_ADDR

    VoC_lw32_sd(REG45,2,DEFAULT);   // &lsp_new_q[]/&st->lsp_old_q[]
    VoC_pop16(REG2,DEFAULT);    // pop16(n-2)   lsp_mid_q
    VoC_movreg16(REG3,REG4,IN_PARALLEL);
    VoC_bez16_d(lsp_M122_DTX_L0,GLOBAL_EFR_FLAG_ADDR);
    VoC_bez16_d(lsp_M122_DTX_L1,GLOBAL_OUTPUT_SP_ADDR)
lsp_M122_DTX_L0:
    VoC_jal(CII_Int_lpc_1and3);
lsp_M122_DTX_L1:
    VoC_pop16(REG1,DEFAULT);    // pop16(n-1)  &lsp_mid[]
    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_return;
}

/*``````````````````````````````````````````````````````
PARAMETERS:

REG0:&corr_ptr[-lag_max]    INC 2
REG1:lag_max                INC -1
REG4:number of loops ;
REG5:L_frame/2

OUTPUT: REG5:cor_max
RETURN: REG4:p_max

````````````````````````````````````````````````````````*/

void CII_Lag_max_M122(void)
{

#if 0
    voc_short    PITCH_OL_M475_SCALED_SIGNAL_ADDR           ,310,y      //[310]shorts
    voc_short    PITCH_OL_M475_P_MAX1_ADDR                  ,y      //1 short
    voc_short    PITCH_OL_M475_MAX1_ADDR                    ,y      //1 short
    voc_short    PITCH_OL_M475_P_MAX2_ADDR                  ,y      //1 short
    voc_short    PITCH_OL_M475_MAX2_ADDR                    ,y      //1 short
    voc_short    PITCH_OL_M475_P_MAX3_ADDR                  ,y      //1 short
    voc_short    PITCH_OL_M475_MAX3_ADDR                    ,7,y        //[7]shorts
    voc_short    PITCH_OL_M475_CORR_ADDR                    ,300,y      //[300]shorts

    voc_short    PITCH_OL_M122_SCALED_SIGNAL2_ADDR          ,310,x

#endif


    VoC_push16(RA,DEFAULT);
    VoC_lw32_d(RL6,CONST_0x80000000_ADDR);      //RL6 for max
    VoC_lw32inc_p(RL7,REG0,DEFAULT);


    VoC_loop_r_short(REG4,DEFAULT)
    VoC_movreg16(REG4,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_bgt32_rr(LAG_MAX_M122_02,RL6,RL7)
    VoC_movreg32(RL6,RL7,DEFAULT);
    VoC_movreg16(REG4,REG1,IN_PARALLEL);//REG6:p_max;
LAG_MAX_M122_02:;
    VoC_lw32inc_p(RL7,REG0,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);                        //i--
    VoC_endloop0

    /*return p_max in REG4*/

    /* compute energy */
    VoC_lw16i_set_inc(REG0,(PITCH_OL_M475_SCALED_SIGNAL_ADDR+143),2);
    VoC_sub16_rr(REG0,REG0,REG4,DEFAULT);           //&scal_sig[-p_max]
    VoC_lw32z(ACC0,DEFAULT);

    VoC_aligninc_pp(REG0,REG0,DEFAULT);
    VoC_loop_r_short(REG5,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG0);
    VoC_endloop0

    /* 1/sqrt(energy) */
    VoC_bez16_d(LAG_MAX_M122__03,GLOBAL_DTX_ADDR)
    /* check tone */
    //RL6:max ACC0:t0
    VoC_add32_rd(REG67,ACC0,CONST_0x00008000_ADDR);
    VoC_bngtz16_r(LAG_MAX_M122__03,REG7)

    VoC_multf32_rd(REG67,REG7,CONST_21298_ADDR);

    VoC_lw16i(REG0,0x4000);
    VoC_or16_rd(REG0,STRUCT_VADSTATE1_TONE_ADDR);
    VoC_bngt32_rr(LAG_MAX_M122__03,RL6,REG67)

    VoC_sw16_d(REG0,STRUCT_VADSTATE1_TONE_ADDR);

LAG_MAX_M122__03:

    VoC_movreg32(REG01,ACC0,DEFAULT);               //L_x

    VoC_jal(CII_Inv_sqrt);
    VoC_shr32_ri(REG01,-1,DEFAULT);

    /* max = max/sqrt(energy)  */
    VoC_movreg32(REG23,RL6,DEFAULT);
    VoC_shru16_ri(REG2,1,DEFAULT);
    VoC_shru16_ri(REG0,1,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG3,REG1,DEFAULT);
    VoC_multf16_rr(REG6,REG3,REG0,IN_PARALLEL);

    VoC_multf16_rr(REG2,REG2,REG1,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);

    VoC_mac32_rr(REG6,REG7,DEFAULT);
    VoC_mac32_rr(REG2,REG7,DEFAULT);

    VoC_lw16_d(REG7,(PITCH_OL_M475_MAX3_ADDR+1));
    VoC_shr32_rr(ACC0,REG7,DEFAULT);
    VoC_shr32_ri(ACC0,-15,DEFAULT);

    VoC_pop16(RA,DEFAULT);
    VoC_movreg16(REG5,ACC0_HI,DEFAULT);
    VoC_return;

}

/*
RETURN:REG4:p_max1
optimize by stephen 1020
*/
void CII_Pitch_ol_M122(void)
{


    VoC_push16(RA,DEFAULT);

    VoC_lw16_d(REG4,STRUCT_VADSTATE1_TONE_ADDR);        //st->tone

    VoC_bez16_d(PITCH_OL_NOT_DTX,GLOBAL_DTX_ADDR)

    VoC_shr16_ri(REG4,1,DEFAULT);

PITCH_OL_NOT_DTX:

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_OLD_WSP_ADDR-1,2);
    VoC_sw32_d(ACC0,STRUCT_COD_AMRSTATE_OL_GAIN_FLG_ADDR);

    VoC_sw16_d(REG4,STRUCT_VADSTATE1_TONE_ADDR);    //store st->tone

    VoC_add16_rd(REG0,REG0,COD_AMR_I_SUBFR_ADDRESS);
    VoC_add16_rd(REG5,REG0,CONST_1_ADDR);                       //&(old_wsp)


    VoC_multf32inc_pp(ACC0,REG0,REG0,DEFAULT);
    VoC_loop_i(0,(PITCH_OL_M122_LOOP_NUM-1)/2)
    VoC_bimac32inc_pp(REG0,REG0);
    VoC_endloop0

    /*--------------------------------------------------------*
     * Scaling of input signal.                               *
     *                                                        *
     *   if Overflow        -> scal_sig[i] = signal[i]>>3     *
     *   else if t0 < 1^20  -> scal_sig[i] = signal[i]<<3     *
     *   else               -> scal_sig[i] = signal[i]        *
     *--------------------------------------------------------*/

    /*--------------------------------------------------------*
     *  Verification for risk of overflow.                    *
     *--------------------------------------------------------*/
    VoC_movreg16(REG0,REG5,DEFAULT);
    VoC_lw16i_short(REG3,PITCH_OL_M122_LOOP_NUM/4+1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,PITCH_OL_M475_SCALED_SIGNAL_ADDR,2);


    VoC_aligninc_pp(REG0,REG0,DEFAULT);
    VoC_lw16i_short(REG2,3,DEFAULT);
    VoC_lw16i_short(REG6,56,IN_PARALLEL);

    VoC_be32_rd(PITCH_OL_122_02,ACC0,CONST_0x7FFFFFFF_ADDR)
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_bgt32_rd(PITCH_OL_122_02,ACC0, CONST_0x100000_ADDR)
    VoC_lw16i_short(REG2,-3,DEFAULT);
PITCH_OL_122_02:


    VoC_jal(CII_scale);
    /* calculate all coreelations of scal_sig, from pit_min to pit_max */
    /*copy scaled_signal to RAM_X*/
    VoC_lw16i_set_inc(REG0,PITCH_OL_M475_SCALED_SIGNAL_ADDR,2);
    VoC_sw16_d(REG2,(PITCH_OL_M475_MAX3_ADDR+1));
    VoC_lw16i_set_inc(REG1,PITCH_OL_M122_SCALED_SIGNAL2_ADDR,2);

    VoC_jal(CII_copy_xy);

    VoC_lw16i_set_inc(REG0,PITCH_OL_M475_SCALED_SIGNAL_ADDR+143,2);
    VoC_lw16i_set_inc(REG1,PITCH_OL_M122_SCALED_SIGNAL2_ADDR,2);
    VoC_lw16i_set_inc(REG2,PITCH_OL_M475_CORR_ADDR-2,2);
// begin of CII_comp_corr
    VoC_movreg16(REG6,REG0,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);
    VoC_add16_rr(REG7,REG1,REG3,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_aligninc_pp(REG0,REG1,DEFAULT);
//  exit_on_odd_address = OFF;

    VoC_loop_i(1,126)
    VoC_loop_i_short(40,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG1);
    VoC_endloop0

    VoC_movreg32(REG01,REG67,DEFAULT);
    VoC_sw32inc_p(RL6,REG2,IN_PARALLEL);
    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_aligninc_pp(REG0,REG1,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG3,DEFAULT);
    VoC_endloop1
    VoC_sw32_p(RL6,REG2,DEFAULT);
//  exit_on_odd_address = ON;

// end of CII_comp_corr

    /*--------------------------------------------------------------------*
     *  The pitch lag search is divided in three sections.                *
     *  Each section cannot have a pitch multiple.                        *
     *  We find a maximum for each section.                               *
     *  We compare the maximum of each section by favoring small lags.    *
     *                                                                    *
     *  First section:  lag delay = pit_max     downto 4*pit_min          *
     *  Second section: lag delay = 4*pit_min-1 downto 2*pit_min          *
     *  Third section:  lag delay = 2*pit_min-1 downto pit_min            *
     *--------------------------------------------------------------------*/
    /*
        j = shl (pit_min, 2);
        p_max1 = Lag_max (vadSt, corr_ptr, scal_sig, scal_fac, scal_flag, L_frame,
                          pit_max, j, &max1, dtx);
    */
    VoC_lw16i_set_inc(REG0,PITCH_OL_M475_CORR_ADDR,2);
    VoC_lw16i_set_inc(REG1,143,-1);
    VoC_lw32_d(REG45,CONST_72_40_ADDR);

    VoC_jal(CII_Lag_max_M122);
    /*
        p_max2 = Lag_max (vadSt, corr_ptr, scal_sig, scal_fac, scal_flag, L_frame,
                          i, j, &max2, dtx);
        */
    VoC_lw16i_set_inc(REG0,PITCH_OL_M475_CORR_ADDR+144,2);
    VoC_sw32_d(REG45,PITCH_OL_M475_P_MAX1_ADDR);
    VoC_lw16i_set_inc(REG1,71,-1);

    VoC_lw32_d(REG45,CONST_40_36_ADDR);
    VoC_jal(CII_Lag_max_M122);

    /*
        p_max3 = Lag_max (vadSt, corr_ptr, scal_sig, scal_fac, scal_flag, L_frame,
                          i, pit_min, &max3, dtx);
        */

    VoC_lw16i_set_inc(REG0,PITCH_OL_M475_CORR_ADDR+216,2);
    VoC_sw32_d(REG45,PITCH_OL_M475_P_MAX2_ADDR);
    VoC_lw16i_set_inc(REG1,35,-1);
    VoC_lw16i_short(REG4,18,DEFAULT);
    VoC_lw16i_short(REG5,40,IN_PARALLEL);
    VoC_jal(CII_Lag_max_M122);
    VoC_sw32_d(REG45,PITCH_OL_M475_P_MAX3_ADDR);

    VoC_bez16_d(PITCH_OL_M122_04a,GLOBAL_DTX_ADDR)
    VoC_bnez16_d(PITCH_OL_M122_04b,COD_AMR_SUBFRNR_ADDRESS)
    /* calculate max high-passed filtered correlation of all lags */
PITCH_OL_M122_04a:
    VoC_jump(PITCH_OL_M122_04);
PITCH_OL_M122_04b:
    VoC_lw16i_set_inc(REG0,PITCH_OL_M475_CORR_ADDR+0,2);
    VoC_lw16i_set_inc(REG1,PITCH_OL_M475_CORR_ADDR+2,2);
    VoC_lw16i_set_inc(REG2,PITCH_OL_M475_CORR_ADDR+4,2);

// begin of CII_hp_max
    VoC_lw32_d(ACC0,CONST_0x80000000_ADDR); //max
    VoC_lw32inc_p(RL6,REG1,DEFAULT);

    VoC_loop_i(0,122)
    VoC_shr32_ri(RL6,-1,DEFAULT);
    VoC_sub32inc_rp(RL6,RL6,REG0,DEFAULT);
    VoC_sub32inc_rp(RL6,RL6,REG2,DEFAULT);
    VoC_bnltz32_r(HP_MAX_01,RL6)
    VoC_sub32_dr(RL6,CONST_0_ADDR,RL6);
HP_MAX_01:
    VoC_bgt32_rr(HP_MAX_02,ACC0,RL6)
    VoC_movreg32(ACC0,RL6,DEFAULT);
HP_MAX_02:
    VoC_lw32inc_p(RL6,REG1,DEFAULT);
    VoC_endloop0

    VoC_jal(CII_NORM_L_ACC0);
    VoC_sub16_rd(REG3,REG1,CONST_1_ADDR);   //shift1
    VoC_lw16i_set_inc(REG0,PITCH_OL_M475_SCALED_SIGNAL_ADDR+143,2);
    VoC_lw16i_set_inc(REG1,PITCH_OL_M122_SCALED_SIGNAL2_ADDR+143,2);
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_movreg16(REG4,ACC0_HI,DEFAULT);         //max16
    /* compute energy */

//  exit_on_odd_address = OFF;
    VoC_aligninc_pp(REG0,REG1,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_loop_i_short(40,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG1);
    VoC_endloop0

    VoC_lw16i(REG0,PITCH_OL_M475_SCALED_SIGNAL_ADDR+143);
    VoC_lw16i(REG1,PITCH_OL_M122_SCALED_SIGNAL2_ADDR+143-1);
    VoC_movreg32(RL6,ACC0,DEFAULT); //t0

    VoC_aligninc_pp(REG0,REG1,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_loop_i_short(40,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG1);
    VoC_endloop0
//  exit_on_odd_address = ON;

    /* high-pass filtering */
    VoC_shr32_ri(RL6,-1,DEFAULT);
    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_sub32_rr(ACC0,RL6,ACC0,DEFAULT);
    VoC_bnltz32_r(HP_MAX_03,ACC0)
    VoC_sub32_dr(ACC0,CONST_0_ADDR,ACC0);
HP_MAX_03:
    /* max/t0 */
    VoC_jal(CII_NORM_L_ACC0);
    VoC_sub16_rr(REG7,REG3,REG1,DEFAULT);   //shift
    VoC_movreg16(REG1,ACC0_HI,IN_PARALLEL); //t016
    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_lw16i_short(REG2,0,IN_PARALLEL);        //cor_max
    VoC_bez16_r(HP_MAX_04,REG1)
    VoC_jal(CII_DIV_S);
HP_MAX_04:
    VoC_shr16_rr(REG2,REG7,DEFAULT);
    VoC_NOP();
// end of CII_hp_max
    /* update complex background detector */
    VoC_sw16_d(REG2,STRUCT_VADSTATE1_BEST_CORR_HP_ADDR);

PITCH_OL_M122_04:
    /*--------------------------------------------------------------------*
     * Compare the 3 sections maximum, and favor small lag.               *
     *--------------------------------------------------------------------*/

    VoC_lw16i(REG6,27853);
    VoC_lw32_d(REG45,PITCH_OL_M475_P_MAX1_ADDR);
    VoC_multf16_rr( REG7,REG6,REG5,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_bnlt16_rd(PITCH_OL_M122_05,REG7,PITCH_OL_M475_MAX2_ADDR)
    VoC_lw32_d(REG45,PITCH_OL_M475_P_MAX2_ADDR);

PITCH_OL_M122_05:
    VoC_multf16_rr( REG7,REG6,REG5,DEFAULT);
    VoC_NOP();

    VoC_bnlt16_rd(PITCH_OL_M122_06,REG7,PITCH_OL_M475_MAX3_ADDR)
    VoC_lw16_d(REG4,PITCH_OL_M475_P_MAX3_ADDR);
PITCH_OL_M122_06:

    VoC_return


}

/*********************************************
 Function:  CII_Q_plsf_5
 Input:
       st->past_rq/*indice  :push32(n)
       *lsp1(&lsp_mid[])    :REG0
       *lsp2(&lsp_new[])    :push16(n+4)
       *lsp1_q(&lsp_mid_q[])    :push16(n+1)
       *lsp2_q(&lsp_new_q[])    :REG3
 Optimized by Kenneth 09/18/2004
**********************************************/
void CII_Q_plsf_5(void)
{
#if 0

    voc_short    Q_PLSF_5_LSF1_ADDR                      ,10,x          //[10]shorts
    voc_short    Q_PLSF_5_LSF2_ADDR                      ,10,x           //[10]shorts
    voc_short    Q_PLSF_5_WF1_ADDR                       ,10,x           //[10]shorts
    voc_short    Q_PLSF_5_WF2_ADDR                       ,10,x           //[10]shorts
    voc_short    Q_PLSF_5_LSF_R1_ADDR                    ,10,x           //[10]shorts
    voc_short    Q_PLSF_5_LSF_R2_ADDR                    ,10,x           //[10]shorts
    voc_short    Q_PLSF_5_LSF1_Q_ADDR                    ,10,x           //[10]shorts
    voc_short    Q_PLSF_5_LSF2_Q_ADDR                    ,10,x           //[10]shorts
    voc_short   Q_PLSF_5_LSF_AVER_ADDR                    ,30,x
    voc_short   Q_PLSF_5_DTX_AND_SP_ADDR                  ,x
    voc_short   Q_PLSF_5_DTX_AND_SID_ADDR                 ,x
    voc_short   Q_PLSF_5_DTX_AND_HAN_ADDR                 ,x
    voc_short   Q_PLSF_5_DTX_AND_PREV_HAN_ADDR            ,x


    voc_short    Q_PLSF_5_LSF_P_ADDR                     ,10,y
#endif


    VoC_push16(RA,DEFAULT );
    VoC_lw16i_set_inc(REG1,Q_PLSF_5_LSF1_ADDR,-1);
    VoC_push16(REG3,DEFAULT);       // push16(n-1) *lsp2_q(&lsp_new_q[])
    VoC_lw16i_short(INC0,-1,IN_PARALLEL);
    VoC_push16(REG1,DEFAULT);       // push16(n-2) &lsf1[]

    VoC_jal(CII_Lsp_lsf);           // Lsp_lsf (lsp1, lsf1, M);

    VoC_lw16i_set_inc(REG1,Q_PLSF_5_LSF2_ADDR,-1);
    VoC_lw16_sd(REG0,7,DEFAULT);        // load &lsp2[]
    VoC_push16(REG1,DEFAULT);       // push16(n-3) &lsf2[]
    VoC_jal(CII_Lsp_lsf);           // Lsp_lsf (lsp2, lsf2, M);

    VoC_bez16_d(Q_PLSF5_DTX1,GLOBAL_EFR_FLAG_ADDR)
    VoC_jal(CII_plsf5_dtx1);
Q_PLSF5_DTX1:

    VoC_lw16i_set_inc(REG0,Q_PLSF_5_LSF_R1_ADDR,1);
    VoC_lw16i_set_inc(REG1,Q_PLSF_5_WF1_ADDR,1);
    VoC_lw16i(REG4,Q_PLSF_5_LSF_R2_ADDR );
    VoC_lw16i(REG5,Q_PLSF_5_WF2_ADDR );
    VoC_push32(REG01,DEFAULT);      // push32(n-1)  wf1/lsf_r1
    VoC_push32(REG45,DEFAULT);      // push32(n-2)  wf2/lsf_r2
    VoC_bez16_d(Q_PLSF5_DTX2,GLOBAL_EFR_FLAG_ADDR)
    VoC_bez16_d(Q_PLSF5_DTX2,Q_PLSF_5_DTX_AND_SID_ADDR)
    VoC_jal(CII_plsf5_dtx2);

    VoC_jump(Q_PLSF_5_05);

Q_PLSF5_DTX2:
    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_jal(CII_Lsf_wt);            //Lsf_wt (lsf1, wf1);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_jal(CII_Lsf_wt);            //Lsf_wt (lsf2, wf2);
Q_PLSF_5_05:
    VoC_bez16_d(Q_PLSF5_DTX3,GLOBAL_EFR_FLAG_ADDR)
    VoC_bnez16_d(Q_PLSF5_DTX3,Q_PLSF_5_DTX_AND_SP_ADDR)
    VoC_jal(CII_plsf5_dtx3);
    VoC_jump(Q_PLSF_5_06);

Q_PLSF5_DTX3:
    VoC_lw32_sd(REG01,2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);   //REG1 addr of past_rq[i]
    VoC_lw16i_set_inc(REG0,TABLE_MEAN_LSF_ADDR,2);  //REG0 addr of mean_lsf[i]
    VoC_lw16i_set_inc(REG3, Q_PLSF_5_LSF_P_ADDR,2); //REG2 addr of lsp[i]
    VoC_lw16i_set_inc(REG2, 21299,2);               //REG3 for const PRED_FAC

    VoC_multf16inc_rp(REG6,REG2,REG1, DEFAULT);         // for (i = 0; i < M; i++)
    VoC_multf16inc_rp(REG7,REG2,REG1, IN_PARALLEL);         //    {
    VoC_loop_i_short((M/2),DEFAULT)                         //        lsf_p[i] = add (mean_lsf_5[i], mult (st->past_rq[i], LSP_PRED_FAC_MR122));
    VoC_startloop0                                          //        move16 ();
    VoC_add16inc_rp(REG4,REG6,REG0, DEFAULT);       //        lsf_r1[i] = sub (lsf1[i], lsf_p[i]);           move16 ();
    VoC_add16inc_rp(REG5,REG7,REG0, IN_PARALLEL);   //        lsf_r2[i] = sub (lsf2[i], lsf_p[i]);           move16 ();
    VoC_multf16inc_rp(REG6,REG2,REG1, DEFAULT);     //    }
    VoC_multf16inc_rp(REG7,REG2,REG1, IN_PARALLEL);
    VoC_sw32inc_p(REG45,REG3,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG3,Q_PLSF_5_LSF_P_ADDR,2);
    VoC_pop16(REG2,DEFAULT);    //pop16(n-3)     REG2:&lsf2[i]
    VoC_pop16(REG0,DEFAULT);    //pop16(n-2)     REG0:&lsf1[i]
    VoC_movreg16(REG1,REG3,IN_PARALLEL);                //REG1:&lsf_p
    VoC_lw16i(REG4, Q_PLSF_5_LSF_R1_ADDR);  //REG4 &lsf_r1[i]
    VoC_lw16i(REG5, Q_PLSF_5_LSF_R2_ADDR);  //REG4 &lsf_r1[i]

    VoC_lw16i_short(RL6_HI,2,DEFAULT);
    VoC_lw16i_short(RL6_LO,2,IN_PARALLEL);

    VoC_loop_i_short((M/2),DEFAULT);
    VoC_startloop0
    VoC_sub16inc_pp(REG6,REG0,REG1,DEFAULT);
    VoC_sub16inc_pp(REG7,REG0,REG1,IN_PARALLEL);

    VoC_sub16inc_pp(REG6,REG2,REG3,DEFAULT);
    VoC_sub16inc_pp(REG7,REG2,REG3,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw32_p(REG67,REG4,DEFAULT);
    exit_on_warnings=ON;
    VoC_sw32_p(REG67,REG5,DEFAULT);
    VoC_add32_rr(REG45,REG45,RL6,IN_PARALLEL);
    VoC_endloop0
Q_PLSF_5_06:
    VoC_lw32z(RL7,DEFAULT);

    VoC_lw32_sd(REG23,2,DEFAULT);
    VoC_lw16i_short(REG6,4,IN_PARALLEL);        // 128/32=4
    VoC_lw16i_set_inc(REG3,TABLE_DICO1_LSF_compressed_ADDR,1);
    VoC_push16(REG2,DEFAULT);           // push16(n-2) &indice[]

    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_jal(CII_Vq_subvec_lsf_compressed);              //indice[0] = Vq_subvec (&lsf_r1[0], &lsf_r2[0], dico1_lsf_5,&wf1[0], &wf2[0], DICO1_SIZE_5);
    VoC_lw16i_short(REG6,8,DEFAULT);       // 256/32=8
    VoC_lw16i_set_inc(REG3,TABLE_DICO2_LSF_compressed_ADDR,1);

    VoC_jal(CII_Vq_subvec_lsf_compressed);              //indice[1] = Vq_subvec (&lsf_r1[2], &lsf_r2[2], dico2_lsf_5,&wf1[2], &wf2[2], DICO2_SIZE_5);
    VoC_lw16i_short(REG6,8,DEFAULT);       // 256/32=8
    VoC_lw16i_short(RL7_LO,1,DEFAULT);
    VoC_lw16i_set_inc(REG3,TABLE_DICO3_LSF_compressed_ADDR,1);

    VoC_jal(CII_Vq_subvec_lsf_compressed);              //indice[2] = Vq_subvec_s (&lsf_r1[4], &lsf_r2[4], dico3_lsf_5,&wf1[4], &wf2[4], DICO3_SIZE_5);
    VoC_lw16i_short(REG6,8,DEFAULT);       //256/32=8
    VoC_lw32z(RL7,DEFAULT);
    VoC_lw16i_set_inc(REG3,TABLE_DICO4_LSF_compressed_ADDR,1);

    VoC_jal(CII_Vq_subvec_lsf_compressed);              //indice[3] = Vq_subvec (&lsf_r1[6], &lsf_r2[6], dico4_lsf_5,&wf1[6], &wf2[6], DICO4_SIZE_5);
    VoC_lw16i_short(REG6,2,DEFAULT);        // 64/32=2
    VoC_lw16i_set_inc(REG3,TABLE_DICO5_LSF_compressed_ADDR,1);

    VoC_jal(CII_Vq_subvec_lsf_compressed);              //indice[4] = Vq_subvec (&lsf_r1[8], &lsf_r2[8], dico5_lsf_5,&wf1[8], &wf2[8], DICO5_SIZE_5);

    VoC_bez16_d(Q_PLSF5_DTX4,GLOBAL_EFR_FLAG_ADDR)
    VoC_bnez16_d(Q_PLSF5_DTX4,Q_PLSF_5_DTX_AND_SP_ADDR)
    VoC_jal(CII_plsf5_dtx4);

    VoC_jump(Q_PLSF_5_07);

Q_PLSF5_DTX4:
    VoC_pop16(REG2,DEFAULT);            // pop16(n-2) &indice[]

    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1, Q_PLSF_5_LSF2_Q_ADDR,1);//REG1 addr of lsf2_q[i]
    VoC_lw16i_set_inc(REG0, Q_PLSF_5_LSF1_Q_ADDR,1);//REG0 addr of lsf1_q[i]
    VoC_push16(REG1,DEFAULT);           // push16(n-2)  lsf2_q[i]
    VoC_pop32(REG45,IN_PARALLEL);           // pop32(n-2)  wf2/lsf_r2 , but here REG5->&lsf_r1[]
    VoC_push16(REG0,DEFAULT);           // push16(n-3)  lsf1_q[i]
    VoC_pop32(REG23,IN_PARALLEL);           // pop32(n-1)  wf1/lsf_r1 , but here REG2->&lsf_r2[]
    VoC_lw16i(REG4,Q_PLSF_5_LSF_P_ADDR);
    VoC_lw32_sd(REG67,0,DEFAULT);       //

    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_loop_i_short(10,DEFAULT);
    VoC_movreg16(REG3,REG7,IN_PARALLEL);        //REG3: &st->past_rq[]
    // reg0:&lsf1[] reg2:&lsf_r2        reg4:&lsf_r1[]
    // reg1:&lsf2[] reg3:&st->past_rq[]     reg5:&lsf_p[]
    VoC_startloop0                  // for (i = 0; i < M; i++)
    VoC_add16_pp(REG6,REG4,REG5,DEFAULT);   //     {
    VoC_lw16inc_p(REG7,REG2,DEFAULT);           //         lsf1_q[i] = add (lsf_r1[i], lsf_p[i]);          move16 ();
    VoC_sw16inc_p(REG6,REG0,DEFAULT);           //         lsf2_q[i] = add (lsf_r2[i], lsf_p[i]);          move16 ();
    VoC_sw16inc_p(REG7,REG3,DEFAULT);           //         st->past_rq[i] = lsf_r2[i];                     move16 ();
    VoC_add16_rp(REG7,REG7,REG4,IN_PARALLEL);       // }
    VoC_add32_rd(REG45,REG45,CONST_0x00010001_ADDR);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0

    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_jal(CII_Reorder_lsf);           // Reorder_lsf (lsf1_q, LSF_GAP, M);
    VoC_lw16_sd(REG0,1,DEFAULT);                // Reorder_lsf (lsf2_q, LSF_GAP, M);
    VoC_jal(CII_Reorder_lsf);

    VoC_bez16_d(Q_PLSF_5_09,GLOBAL_EFR_FLAG_ADDR)
    VoC_bez16_d(Q_PLSF_5_09,Q_PLSF_5_DTX_AND_HAN_ADDR);
    VoC_lw16i(RL6_LO,Q_PLSF_5_LSF1_Q_ADDR);
    VoC_lw16i(RL6_HI,Q_PLSF_5_LSF2_Q_ADDR);
    VoC_lw16i(REG0,GLOBAL_LSF_OLD_TX_ADDR);
    VoC_jal(CII_update_lsf_history);

Q_PLSF_5_09:
    VoC_pop16(REG0,DEFAULT);            // pop16(n-3)  &lsf1_q[i]
    VoC_lw16_sd(REG1,3,DEFAULT);            // load &lsp1_q[]
    VoC_jal(CII_Lsf_lsp);                   // Lsf_lsp (lsf1_q, lsp1_q, M);

    VoC_pop16(REG0,DEFAULT);            // pop16(n-2) &lsf2_q[i]
    VoC_pop16(REG1,DEFAULT);            // pop16(n-1) &lsp2_q []
    VoC_jal(CII_Lsf_lsp);               // Lsf_lsp (lsf2_q, lsp2_q, M);

Q_PLSF_5_07:
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(REG7,5,DEFAULT);
    VoC_return;
}






/***********************************************
  Function CII_Int_lpc_1and3
  input: REG3->&lsp_old[]
         REG2->&lsp_mid[]
         REG5->&lsp_new[]
         REG0->&Az[]
         REG6->
         REG6=1 -->CII_Int_lpc_1and3
         REG6=0 -->CII_Int_lpc_1and3_2
         Optimised by Cui 2005.01.25
***********************************************/
void CII_Int_lpc_1and3(void)
{



#if 0

    voc_short   INT_LPC_LSP_ADDR                ,10     ,y
    voc_short   F1_ADDR                ,12  ,y
    voc_short   F2_ADDR                ,12  ,y

#endif



    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_set_inc(REG1,INT_LPC_LSP_ADDR,2);

Int_lpc_1and3_L0:

    VoC_bne16_rd(Int_lpc_1and3_L3,REG7,(0+STATIC_CONST_pred_MR122_ADDR));//44
    VoC_return;
Int_lpc_1and3_L3:

    VoC_push16(RA,DEFAULT);                // push16(n)

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
    VoC_lw16i_short(REG0,11,DEFAULT);
    VoC_add16_rr(REG5,REG7,REG0,DEFAULT);
    VoC_add16_rr(REG7,REG5,REG0,DEFAULT);
    //  VoC_NOP();
//  VoC_add16_rd(REG7,REG7,CONST_22_ADDR);
//  VoC_sub16_rd(REG5,REG7,CONST_11_ADDR);
    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_sw32_sd(REG67,1,DEFAULT);

    VoC_bez16_r(Int_lpc_1and3_L1,REG6);
    VoC_lw16_sd(REG1,0,DEFAULT);

    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);

    VoC_bne16_rd(Int_lpc_1and3_L2,REG5,(68+TABLE_MR475_ADDR)); //33
    VoC_lw16_sd(REG1,1,DEFAULT);
Int_lpc_1and3_L2:

    VoC_jal(CII_Lsp_Az);
Int_lpc_1and3_L1:
    VoC_pop16(REG2,DEFAULT);
    VoC_pop32(REG01,DEFAULT);
    VoC_pop16(REG3,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);

    VoC_pop16(RA,DEFAULT);
    VoC_movreg16(REG5, REG3,IN_PARALLEL);
    VoC_jump(Int_lpc_1and3_L0);

    VoC_return;
}



void  CII_gainQuant_M122(void)
{



#if 0

    voc_short    calc_filt_energies_Y2_ADDRESS              ,50,y       //[50]shorts
    voc_short    gainQuant_frac_coeff_ADDRESS               ,10,y       //[10]shorts
    voc_short    gainQuant_exp_coeff_ADDRESS                ,10,y       //[10]shorts
    voc_short    MR475_gain_quant_coeff_ADDRESS             ,10,y       //[10]shorts
    voc_short    MR475_gain_quant_coeff_lo_ADDRESS          ,10,y       //[10]shorts
    voc_short    MR475_gain_quant_exp_max_ADDRESS           ,10,y       //[10]shorts
    voc_short    MR795_gain_quant_frac_en_ADDRESS           ,10,y        //[10]shorts
    voc_short    MR795_gain_quant_exp_en_ADDRESS            ,10,y        //[10]shorts
    voc_short    MR795_gain_quant_g_pitch_cand_ADDRESS      ,10,y        //[10]shorts
    voc_short    MR795_gain_quant_g_pitch_cind_ADDRESS      ,10,y        //[10]shorts

#endif



    // stack16:   mode,  xn2 ,st->h1, st->sharp,   gain_pit, res2,  code, y2, subfrNr, anap,RA
    VoC_push16(RA,DEFAULT);

    VoC_lw16i_set_inc(REG0,COD_AMR_CODE_ADDRESS, 2);

    VoC_loop_i_short(20, DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_bimac32inc_pp(REG0, REG0);
    VoC_endloop0

    VoC_NOP();

    VoC_add32_rd(REG23, ACC0, CONST_0x00008000_ADDR);
    VoC_multf32_rd(ACC0, REG3, (3+STATIC_CONST_PDOWN_ADDR) );//CONST_26214_ADDR
    VoC_jal(CII_Log2);  //   //REG4  exp   REG5   frac


    VoC_lw32_d(ACC0, CONST_783741L_ADDR); //acc0 is     783741L    MEAN_ENER_MR122

    VoC_lw16i_set_inc(REG0, (STRUCT_GC_PREDST_ADDR+4),2); //REG0  :     st->past_qua_en_MR122
    VoC_lw16i_set_inc(REG1, STATIC_CONST_pred_MR122_ADDR, 2);

    VoC_loop_i_short(2, DEFAULT);
    VoC_startloop0;
    VoC_bimac32inc_pp(REG0, REG1);
    VoC_endloop0;

    VoC_movreg16(REG6, REG5, DEFAULT); //reg67 :ener_code
    VoC_shr32_ri(REG67, -1, DEFAULT);
    VoC_lw16i_short(REG2,30,IN_PARALLEL);

    VoC_sub16_rr(REG7, REG4, REG2,DEFAULT);
    VoC_sub32_rr(REG67, ACC0, REG67, DEFAULT);
    VoC_shr32_ri(REG67, 1, DEFAULT);
    VoC_shru16_ri(REG6, 1,DEFAULT);


    VoC_lw16i_set_inc(REG0, COD_AMR_XN2_ADDRESS, 1);  //INPUT:   //      REG0       xn2   incr=1
    VoC_lw16i_set_inc(REG1, COD_AMR_Y2_ADDRESS, 2);     //   REG1      y2    incr=1

    // inline CII_G_code
    // INPUT:
    // REG0    xn2   incr=1
    // REG1    y2    incr=1
    // return  REG2

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(ACC0_LO,1,DEFAULT);

    VoC_loop_i_short(20, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG23, REG1,DEFAULT);
    VoC_shr16_ri(REG2, 1, DEFAULT);
    VoC_shr16_ri(REG3, 1, IN_PARALLEL);
    VoC_mac32inc_rp(REG2, REG0, DEFAULT);
    VoC_mac32_rr(REG2, REG2, IN_PARALLEL);
    VoC_mac32inc_rp(REG3, REG0, DEFAULT);
    VoC_mac32_rr(REG3, REG3, IN_PARALLEL);
    VoC_endloop0

    VoC_jal(CII_NORM_L_ACC0);
    // REG1 exp_xy
    VoC_movreg32(REG23, ACC0, DEFAULT);
    //   REG3  xy
    VoC_lw16i_short(REG2, 0, DEFAULT);
    VoC_bngtz16_r(CII_G_code01, REG3)

    VoC_add16_rd(REG4,  REG1,CONST_5_ADDR);
    // REG4   :    i = add (exp_xy, 5);
    VoC_movreg32(ACC0, ACC1, DEFAULT);
    VoC_jal(CII_NORM_L_ACC0);
    //REG1   -exp_yy

    // ACC0_HI    yy
    VoC_shr16_ri(REG3, 1, DEFAULT);
    VoC_sub16_rr(REG4, REG4, REG1, DEFAULT);
    //REG4      i = sub (i, exp_yy);
    VoC_movreg16(REG0, REG3, DEFAULT);
    VoC_movreg16(REG1, ACC0_HI, IN_PARALLEL);
    VoC_jal(CII_DIV_S);
    //REG2 gain
    VoC_shr16_rr(REG2, REG4, DEFAULT);

    VoC_bnez16_d(CII_G_code01, GLOBAL_EFR_FLAG_ADDR)
    VoC_shr16_ri(REG2, -1, DEFAULT);
CII_G_code01:

    // end inline
    // return REG2
    VoC_movreg32(REG01, REG67, DEFAULT);    //INPUT:       REG1:     exp_gcode0       REG0:    frac_gcode0
    VoC_sw16_d(REG2, COD_AMR_GAIN_CODE_ADDRESS);

//stack16:   mode,  xn2 ,st->h1, st->sharp,   gain_pit, res2,  code, y2, subfrNr, anap,RA

    VoC_jal(CII_Pow2);      //REG2     gcode0 = extract_l (Pow2 (exp_gcode0, frac_gcode0));
    VoC_lw16_d(REG6, COD_AMR_GAIN_CODE_ADDRESS);
    VoC_bnez16_d(CII_q_gain_code01, GLOBAL_EFR_FLAG_ADDR)
    VoC_shr16_ri(REG6, 1, DEFAULT);     //REG6  g_q0
CII_q_gain_code01:
    VoC_shr16_ri(REG2,-4, DEFAULT); //   REG2     gcode0= shr (*gain, 1);

    VoC_lw16i_set_inc(REG0, STATIC_CONST_qua_gain_code_ADDR, 3);
    VoC_lw16i(REG5,0x7FFF);
    VoC_lw16i_set_inc(REG3,0, 1);

    VoC_lw32z(RL6,DEFAULT);
    VoC_multf16inc_rp(REG4, REG2, REG0, IN_PARALLEL);
    VoC_loop_i_short(32, DEFAULT);     //      for (i = 1; i < NB_QUA_CODE; i++)     #define NB_QUA_CODE 32
    VoC_startloop0
    VoC_sub16_rr(REG1 , REG6, REG4, DEFAULT); //REG1 :   err
    VoC_bnltz16_r(q_gain_code_L140, REG1)
    VoC_sub16_dr(REG1 , CONST_0_ADDR, REG1);   //abs
q_gain_code_L140:
    VoC_bngt16_rr(q_gain_code_L150, REG5, REG1)
    VoC_movreg16(REG5, REG1,DEFAULT);
    VoC_movreg16(RL6_LO, REG3,IN_PARALLEL);    //RL6_LO   index
q_gain_code_L150:
    VoC_multf16inc_rp(REG4, REG2, REG0, DEFAULT);
    VoC_inc_p(REG3,DEFAULT);
    VoC_endloop0

    VoC_movreg16(REG3,RL6_LO, DEFAULT);    //REG3   index

    VoC_mult16_rd(REG4, REG3, CONST_3_ADDR);
    VoC_lw16i_set_inc(REG0, STATIC_CONST_qua_gain_code_ADDR, 1);
    VoC_add16_rr(REG0, REG4,REG0,DEFAULT );
    VoC_lw16_sd(REG1, 1,DEFAULT);   //COD_AMR_ANAP_ADDR

    VoC_multf16inc_rp(REG4, REG2, REG0, DEFAULT);

    VoC_sw16inc_p(REG3, REG1, DEFAULT);
    VoC_sw16_sd(REG1, 1,DEFAULT);   //COD_AMR_ANAP_ADDR

    VoC_bnez16_d(CII_q_gain_code02, GLOBAL_EFR_FLAG_ADDR)
    VoC_shr16_ri(REG4,-1, DEFAULT);
CII_q_gain_code02:

    VoC_lw16inc_p(REG7, REG0, DEFAULT);//REG6  qua_ener
    VoC_lw16inc_p(REG6, REG0, DEFAULT);// REG7  qua_ener_MR122

    VoC_sw16_d(REG4, COD_AMR_GAIN_CODE_ADDRESS);

    //OUTPUT:     //REG6  qua_ener    REG7  qua_ener_MR122
    VoC_lw16i(REG5, STRUCT_GC_PREDST_ADDR);
    VoC_jal(CII_gc_pred_update);

    VoC_pop16(RA,DEFAULT);
    VoC_pop16(REG5, DEFAULT);
    VoC_return;
}


/***************************************
  Function CII_lsp
  Call it directly , has not any interface need to config
  Note:  CII_lsp includes CII_lsp_M475 and CII_lsp_M122
  Optimized by Kenneth  19/09/2004
***************************************/
void CII_lsp_122(void)
{

#if 0
    voc_short    LSP_LSP_NEW_Q_ADDR                   ,10,x
    voc_short    LSP_LSP_MID_ADDR                     ,10,x
    voc_short    LSP_LSP_MID_Q_ADDR                   ,10,x
#endif

    VoC_push16(RA,DEFAULT);

    VoC_lw16i(REG7,LSP_LSP_NEW_Q_ADDR);
    VoC_lw16i(REG6,STRUCT_LSPSTATE_LSP_OLD_Q_ADDR);
    VoC_lw16i_set_inc(REG1,COD_AMR_LSP_NEW_ADDRESS,-1);
    VoC_lw16i(REG4,COD_AMR_AQ_T_ADDRESS);
    VoC_lw16i(REG5,COD_AMR_A_T_ADDRESS);
    VoC_push16(REG1,DEFAULT);   // push16(n+2)   &lsp_new[]
    VoC_push32(REG67,IN_PARALLEL);  // push32(n+3)   &lsp_new_q[]/&st->lsp_old_q[]
    VoC_lw16i_set_inc(REG0,STRUCT_LSPSTATE_LSP_OLD_ADDR,1);
    VoC_lw16i(REG7,STRUCT_Q_PLSFSTATE_PAST_RQ_ADDR);
    VoC_lw16_sd(REG6,2,DEFAULT);
    VoC_push16(REG0,DEFAULT);   // push16(n+1)   &st->lsp_old[]
    VoC_push32(REG45,IN_PARALLEL);  // push32(n+2)   &az[]/&azQ[]
    VoC_push32(REG67,DEFAULT);  // push32(n+1)   &st->qSt->past_rq[]/&anap[]
    VoC_lw16i_short(REG3,7,IN_PARALLEL);

    VoC_jal(CII_lsp_M122);

    VoC_pop16(REG1,DEFAULT);
    VoC_pop32(RL6,DEFAULT);
    VoC_pop16(REG0,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);
    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_jal(CII_Copy_M);              // Copy (lsp_new, st->lsp_old, M);

    VoC_bez16_d(efr_encoder103,GLOBAL_EFR_FLAG_ADDR)
    VoC_bnez16_d(efr_encoder103,GLOBAL_OUTPUT_SP_ADDR)
    VoC_lw16i_set_inc(REG0,COD_AMR_A_T_ADDRESS,2);
    VoC_lw16i_set_inc(REG1,COD_AMR_AQ_T_ADDRESS,2);
    VoC_lw16i_short(REG6,11,DEFAULT);
    VoC_jal(CII_copy_xy);
efr_encoder103:
    VoC_pop32(REG01,DEFAULT);
    VoC_bez16_d(CII_lsp_122_1,GLOBAL_EFR_FLAG_ADDR)
    VoC_bnez16_d(CII_lsp_122_1,GLOBAL_OUTPUT_SP_ADDR)
    VoC_lw16i_set_inc(REG1,COD_AMR_LSP_NEW_ADDRESS,2);
CII_lsp_122_1:
    VoC_pop16(RA,DEFAULT);
    VoC_loop_i_short(5,DEFAULT);
    VoC_lw32inc_p(ACC0,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG0,DEFAULT); //Copy (lsp_new_q, st->lsp_old_q, M);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_return;
}

// EFR DTX END

// VoC_directive: FAKE_FUNCTION_ON
#ifdef voc_compile_only

void CII_amr_dec_475(void) {};
void CII_amr_dec_515(void) {};
void CII_amr_dec_59(void) {};
void CII_amr_dec_67(void) {};
void CII_amr_dec_74(void) {};
void CII_amr_dec_795(void) {};
void CII_amr_dec_102(void) {};
void CII_COD_AMR_475(void) {};
void CII_COD_AMR_515(void) {};
void CII_COD_AMR_59(void) {};
void CII_COD_AMR_67(void) {};
void CII_COD_AMR_74(void) {};
void CII_COD_AMR_795(void) {};
void CII_COD_AMR_102(void) {};
void CII_HR_Encode(void) {};
void CII_HR_Decode(void) {};
void CII_FR_Encode(void) {};
void CII_FR_Decode(void) {};


#endif
// VoC_directive: FAKE_FUNCTION_OFF
