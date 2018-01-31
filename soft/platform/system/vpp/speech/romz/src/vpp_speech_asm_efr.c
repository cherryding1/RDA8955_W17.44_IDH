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


// EFR DTX BEGIN
void CII_dtx_efr_dec_reset(void)
{

    VoC_lw16i_set_inc(REG0,TABLE_MEAN_LSF_ADDR,1);
    VoC_lw16i_set_inc(REG1,GLOBAL_LSF_OLD_CN_DEC_ADDR,1);
    VoC_lw16i_set_inc(REG2,GLOBAL_LSF_NEW_CN_DEC_ADDR,1);

    VoC_loop_i_short(10,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_startloop0;
    VoC_lw16inc_p(REG3,REG0,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG3,REG1,DEFAULT);
    VoC_sw16inc_p(REG3,REG2,DEFAULT);
    VoC_endloop0;

    VoC_sw16_d(ACC1_LO,GLOBAL_GAIN_CODE_OLD_CN_DEC_ADDR);
    VoC_sw16_d(ACC1_LO,GLOBAL_GAIN_CODE_NEW_CN_DEC_ADDR);

    VoC_return;


}

//REG2 transfer  output parameter lsf2_q or lsf_interp_CN
void CII_interpolate_CN_lsf(void) //fts optimised 1114
{
    VoC_lw16i_set_inc(REG0,GLOBAL_LSF_OLD_CN_DEC_ADDR,1);
    VoC_lw16i_set_inc(REG1,GLOBAL_LSF_NEW_CN_DEC_ADDR,1);
//      VoC_lw16i(REG3,GLOBAL_RX_DTX_STATE_ADDR);

    VoC_lw16i(REG4,STATIC_CONST_INTERP_FACTOR_ADDR);

    VoC_add16_rd(REG4,REG4,GLOBAL_RX_DTX_STATE_ADDR);//REG4:addr of interp_factor[rx_dtx_state]

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16_p(REG5,REG4,DEFAULT); //REG5:value of interp_factor[rx_dtx_state]

    VoC_sub16_dr(REG6,CONST_0x7FFF_ADDR,REG5);//reg6 sub (0x7fff, interp_factor[rx_dtx_state]);
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);//reg6 add (temp, 1)

    VoC_loop_i_short(M,DEFAULT);//M
    VoC_startloop0

    VoC_multf32inc_rp(ACC0,REG5,REG1,DEFAULT);
    VoC_mac32inc_rp(REG6,REG0,DEFAULT);
    VoC_NOP();
    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000_ADDR);
    VoC_NOP();
    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);

    VoC_endloop0;

    VoC_return;

}




void CII_interpolate_CN_param (void)
{

    VoC_lw16i(REG2,STATIC_CONST_INTERP_FACTOR_ADDR);//reg2 addr of interp_factor
    VoC_add16_rd(REG3,REG2,GLOBAL_RX_DTX_STATE_ADDR);
    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16_p(REG4,REG3,DEFAULT);//reg4 interp_factor[rx_dtx_state]

    VoC_multf32_rd(REG67,REG4,GLOBAL_GAIN_CODE_NEW_CN_DEC_ADDR);

    VoC_sub16_dr(REG5,CONST_0x7FFF_ADDR,REG4);//reg5 sub (0x7fff, interp_factor[rx_dtx_state]);
    VoC_add16_rr(REG5,REG5,REG2,DEFAULT);//reg5 add (temp, 1)

    VoC_multf32_rd(REG23,REG5,GLOBAL_GAIN_CODE_OLD_CN_DEC_ADDR);//reg23 L_mult(temp,old_param)
    VoC_NOP();
    VoC_add32_rr(REG23,REG23,REG67,DEFAULT);// L_temp = L_mac (L_temp, temp, old_param)
    //here if use VoC_mac instruction, more one instruction

    VoC_add32_rd(REG23,REG23,CONST_0x00008000_ADDR);    // temp = round (L_temp)                                     //reg1 temp;

    VoC_return; // REG3 return value

}



void CII_update_gain_code_history_rx (void)//REG0 CAN'T USE
//REG5 input new_gain_code
{

    VoC_lw16i(REG2,GLOBAL_GAIN_CODE_OLD_RX_ADDR);//reg2 addr of gain_code_old_rx
    VoC_lw16i(REG3,STATIC_BUF_P_RX_ADDR);//reg3 addr of buf_p_rx
    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_add16_rp(REG1,REG2,REG3,DEFAULT);//reg2 addr of gain_code_old_rx[buf_p_rx]

    VoC_add16_rp(REG2,REG1,REG3,IN_PARALLEL);       //reg2:buf_p_rx+=1

    VoC_bne16_rd(UP_RX001,REG2,CONST_28_ADDR);
    VoC_lw16i_short(REG2,0,DEFAULT);

UP_RX001:

    VoC_sw16_p(REG5,REG1,DEFAULT);//gain_code_old_rx[buf_p_rx] = new_gain_code;
    VoC_sw16_p(REG2,REG3,DEFAULT);// SAVE buf_p_rx


    VoC_return;
}


// EFR DTX BEGIN
void CII_rx_dtx(void)//new
{

    VoC_lw16i_short(REG3,INVALID_SID_FRAME,DEFAULT);

    VoC_lw16_d(REG1, GLOBAL_SID_ADDR);    //reg1 value of SID_flag

    VoC_bne16_rd(RX_DTX101, REG1, CONST_2_ADDR);  //if SID_flag != 2
    VoC_bnez16_d(RX_DTX101, GLOBAL_BFI_ADDR);  //if bfi != 0
    VoC_lw16i_short(REG3,VALID_SID_FRAME,DEFAULT);// REG3 = frame_type

RX_DTX101:

    VoC_bnez16_r(RX_DTX107, REG1);  //if SID_flag != 0
    //if bfi == 0
    VoC_lw16i_short(REG3,UNUSABLE_FRAME,DEFAULT);
    VoC_bnez16_d(RX_DTX107, GLOBAL_BFI_ADDR);  //if bfi != 0
    VoC_lw16i_short(REG3,GOOD_SPEECH_FRAME,DEFAULT);


RX_DTX107: /* Update of decoder state */

    //VoC_lw16i(REG0, VALID_SID_FRAME); VALID_SID_FRAME=1
    VoC_lw16i_short(REG7,RX_SP_FLAG,DEFAULT);

    VoC_lw16_d(REG4,GLOBAL_RXDTX_CTRL_ADDR);   //reg7 value of *rxdtx_ctrl

    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_and16_ri(REG0, (RX_SP_FLAG));


    VoC_bne16_rd(RX_DTX108, REG3,CONST_1_ADDR);
    VoC_lw16i_short(REG7,RX_FIRST_SID_UPDATE,DEFAULT);//*rxdtx_ctrl = RX_FIRST_SID_UPDATE;

RX_DTX108:
    VoC_bne16_rd(RX_DTX109, REG3,CONST_2_ADDR);
    VoC_lw16i(REG7,RX_FIRST_SID_UPDATE|RX_INVALID_SID_FRAME);

RX_DTX109:
    VoC_bne16_rd(RX_DTX110, REG3,CONST_4_ADDR);
    VoC_lw16i_short(REG7,RX_SP_FLAG,DEFAULT);

RX_DTX110:
    VoC_bnez16_r(RX_DTX130, REG0);   //if ((*rxdtx_ctrl & RX_SP_FLAG) != 0)

//////

    // else

    //VoC_lw16i_short(REG0, VALID_SID_FRAME=1,DEFAULT);
    VoC_lw16i_short(REG7,RX_CONT_SID_UPDATE,DEFAULT);//*rxdtx_ctrl = RX_FIRST_SID_UPDATE;

    VoC_bne16_rd(RX_DTX122, REG3,CONST_2_ADDR);
    VoC_lw16i(REG7,RX_CONT_SID_UPDATE|RX_INVALID_SID_FRAME);

RX_DTX122:
    //VoC_lw16i_short(REG0, UNUSABLE_FRAME=4,DEFAULT);
    VoC_bne16_rd(RX_DTX123, REG3,CONST_4_ADDR);
    VoC_lw16i(REG7,RX_CNI_BFI);

RX_DTX123:
    //VoC_lw16i_short(REG0, GOOD_SPEECH_FRAME=3,DEFAULT);
    VoC_bne16_rd(RX_DTX130, REG3,CONST_3_ADDR);
    //VoC_lw16i_short(REG0,RX_SP_FLAG,DEFAULT);
    //VoC_or16_ri(REG1,REG0,RX_FIRST_SP_FLAG);
    VoC_lw16i(REG7,RX_SP_FLAG | RX_FIRST_SP_FLAG);

    VoC_and16_ri(REG4,RX_DTX_MUTING);
    VoC_bez16_r(RX_DTX130, REG4);
    //VoC_or16_ri(REG7,REG1,RX_PREV_DTX_MUTING);
    VoC_lw16i(REG7, RX_SP_FLAG | RX_FIRST_SP_FLAG| RX_PREV_DTX_MUTING);


//////
//    if ((*rxdtx_ctrl & RX_SP_FLAG) != 0)
RX_DTX130:
//      VoC_lw16i(REG4, STATIC_PREV_SID_FRAMES_LOST_ADDR);  //reg3 address of prev_SID_frames_lost
//      VoC_lw16i(REG2, GLOBAL_RX_DTX_STATE_ADDR);
    VoC_lw16_d(REG4,STATIC_PREV_SID_FRAMES_LOST_ADDR);  //reg4:prev_sid_frames_lost
    VoC_lw16_d(REG2,GLOBAL_RX_DTX_STATE_ADDR);          //reg2:rxdtx_state
    VoC_movreg16(REG0,REG7,DEFAULT);
    VoC_and16_ri(REG0, (RX_SP_FLAG));
    VoC_bez16_r(RX_DTX140, REG0); //((*rxdtx_ctrl & RX_SP_FLAG) != 0)
    VoC_lw16i_short(REG4,0,DEFAULT);
    //VoC_sw16i_p(CN_INT_PERIOD -1,REG2);//rx_dtx_state = CN_INT_PERIOD - 1;
    VoC_lw16i_short(REG2,CN_INT_PERIOD-1,IN_PARALLEL);
    VoC_jump(RX_DTX160);

//////
RX_DTX140: /* First SID frame */

    VoC_movreg16(REG0,REG7,DEFAULT);
    VoC_and16_ri(REG0, (RX_FIRST_SID_UPDATE));
    VoC_bez16_r(RX_DTX141, REG0); //((*rxdtx_ctrl & RX_SP_FLAG) != 0)
    //VoC_sw16i_p(0,REG4);//prev_SID_frames_lost = 0;
    //VoC_sw16i_p(CN_INT_PERIOD -1,REG2);//rx_dtx_state = CN_INT_PERIOD - 1;
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i_short(REG2,CN_INT_PERIOD-1,IN_PARALLEL);

RX_DTX141:/* SID frame detected, but not the first SID */
    VoC_movreg16(REG0,REG7,DEFAULT);
    VoC_and16_ri(REG0,RX_CONT_SID_UPDATE);
    VoC_bez16_r(RX_DTX150, REG0);//if ((*rxdtx_ctrl & RX_CONT_SID_UPDATE) != 0)
    //VoC_sw16i_p(0,REG4);//prev_SID_frames_lost = 0;
    VoC_lw16i_short(REG4,0,DEFAULT);

    //VoC_lw16i(REG1,VALID_SID_FRAME=1);
    VoC_bne16_rd(RX_DTX142,REG3,CONST_1_ADDR);
    //VoC_sw16i_p(0,REG2);//rx_dtx_state = 0
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_jump(RX_DTX150);

RX_DTX142:
    //VoC_lw16i_short(REG1,INVALID_SID_FRAME=2,DEFAULT);
    VoC_bne16_rd(RX_DTX150, REG3,CONST_2_ADDR);
    VoC_lw16i_short(REG1,CN_INT_PERIOD - 1,DEFAULT);
    VoC_bngt16_rr(RX_DTX150, REG1,REG2);
    //VoC_lw16_p(REG1,REG2,DEFAULT);
    VoC_add16_rd(REG2,REG2,CONST_1_ADDR);
    //VoC_sw16_p(REG1,REG2,DEFAULT);//rx_dtx_state = add(rx_dtx_state, 1);

//////
RX_DTX150:  /* Bad frame received in CNI mode */
    VoC_movreg16(REG0,REG7,DEFAULT);
    VoC_and16_ri(REG0,RX_CNI_BFI);
    VoC_bez16_r(RX_DTX160,REG0);
    VoC_lw16i_short(REG1,CN_INT_PERIOD - 1,DEFAULT);
    VoC_bngt16_rr(RX_DTX151,REG1,REG2);
    //VoC_lw16_p(REG0,REG2,DEFAULT);
    VoC_add16_rd(REG2,REG2,CONST_1_ADDR);
    //VoC_sw16_p(REG0,REG2,DEFAULT);//rx_dtx_state = add(rx_dtx_state, 1);
RX_DTX151:
    VoC_lw16i_short(REG1,1,DEFAULT);
    //VoC_lw16_p(REG0,REG0,DEFAULT);
    //VoC_sub16_rd(REG0,REG0,PAGE_REG_2,CONST_1_ADDR);
    VoC_bne16_rd(RX_DTX152,REG1,GLOBAL_TAF_ADDR);
    VoC_or16_ri(REG7,RX_LOST_SID_FRAME);
    //VoC_lw16_p(REG0,REG4,DEFAULT);
    VoC_add16_rd(REG4,REG4,CONST_1_ADDR);
    //VoC_sw16_p(REG0,REG4,DEFAULT);//prev_SID_frames_lost = add (prev_SID_frames_lost, 1);
    VoC_jump(RX_DTX153);

RX_DTX152:
    VoC_or16_ri(REG7,RX_NO_TRANSMISSION);

RX_DTX153:
    //VoC_lw16_p(REG0,REG4,DEFAULT);
    //VoC_sub16_rd(REG0,REG0,PAGE_REG_2,CONST_1_ADDR);
    VoC_bngt16_rr(RX_DTX160,REG4,REG1);
    VoC_or16_ri(REG7,RX_DTX_MUTING);

RX_DTX160://rxdtx_N_elapsed = add (rxdtx_N_elapsed, 1);


    VoC_sw16_d(REG4,STATIC_PREV_SID_FRAMES_LOST_ADDR);  //reg4:prev_sid_frames_lost
    VoC_sw16_d(REG2,GLOBAL_RX_DTX_STATE_ADDR);          //reg2:rxdtx_state


    //VoC_lw16i(REG2, STATIC_RXDTX_N_ELAPSED_ADDR );
    VoC_lw16_d(REG2,STATIC_RXDTX_N_ELAPSED_ADDR);//reg2:rxdtx_N_elapsed
    VoC_add16_rd(REG2,REG2,CONST_1_ADDR);
//          VoC_sw16_p(REG0,REG2,DEFAULT); //rxdtx_N_elapsed = add (rxdtx_N_elapsed, 1);

//          VoC_lw16i(REG4, STATIC_RXDTX_AVER_PERIOD_ADDR);
    VoC_lw16_d(REG4,STATIC_RXDTX_AVER_PERIOD_ADDR);//reg4:rxdtx_aver_period
    VoC_movreg16(REG1,REG7,DEFAULT);
    VoC_and16_ri(REG1,RX_SP_FLAG);
    VoC_bez16_r(RX_DTX161,REG1);
    //VoC_sw16i_p(DTX_HANGOVER,REG4);//rxdtx_aver_period = DTX_HANGOVER;
    VoC_lw16i_short(REG4,DTX_HANGOVER,DEFAULT);
    VoC_jump(RX_DTX170);
RX_DTX161:
    //VoC_lw16i(REG1,DTX_ELAPSED_THRESHOLD=30);
    VoC_bngt16_rd(RX_DTX162,REG2,CONST_30_ADDR)  //if (sub (rxdtx_N_elapsed, DTX_ELAPSED_THRESHOLD) > 0)
    VoC_or16_ri(REG7,RX_UPD_SID_QUANT_MEM);//*rxdtx_ctrl |= RX_UPD_SID_QUANT_MEM;

    //L_pn_seed_rx = 0x70816958L
    VoC_lw16i(REG0,0x6958);
    VoC_lw16i(REG1,0x7081);
    //rxdtx_N_elapsed = 0;
    VoC_lw16i_short(REG2,0,DEFAULT);
    //rxdtx_aver_period = 0;
    VoC_lw16i_short(REG4,0,IN_PARALLEL);

    VoC_sw32_d(REG01,GLOBAL_L_PN_SEED_RX_ADDR);
    VoC_jump(RX_DTX170);
RX_DTX162:
    VoC_bnez16_r(RX_DTX163,REG4);
    //VoC_sw16i_p(0,REG2);//rxdtx_N_elapsed = 0;
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_jump(RX_DTX170);

RX_DTX163:
    //VoC_lw16_p(REG0,REG4,DEFAULT);
    VoC_sub16_rd(REG4,REG4,CONST_1_ADDR);
    //VoC_sw16_p(REG0,REG4,DEFAULT);

RX_DTX170:

    //VoC_sw16_p(REG7,REG6,DEFAULT);//reserve rxdtx_ctrl
    VoC_sw16_d(REG2,STATIC_RXDTX_N_ELAPSED_ADDR);//reg2:rxdtx_N_elapsed
    VoC_sw16_d(REG4,STATIC_RXDTX_AVER_PERIOD_ADDR);//reg4:rxdtx_aver_period

    VoC_lw16_d(REG2,GLOBAL_BFI_ADDR);
    VoC_sw16_d(REG7,GLOBAL_RXDTX_CTRL_ADDR);   //reg7 value of *rxdtx_ctrl

    VoC_sw16_d(REG2,DEC_AMR_BFI_ADDRESS);

    VoC_return;
}


void CII_reset_rx_dtx(void)
{
    VoC_push32(REG67,DEFAULT);
    VoC_push16(REG2,DEFAULT);
    VoC_push16(REG1,DEFAULT);

    VoC_lw32z(REG67,DEFAULT);
    VoC_lw16i_set_inc(REG2,GLOBAL_GAIN_CODE_OLD_RX_ADDR,2);
//  for (i = 0; i < 4 * DTX_HANGOVER; i++)
//  {
//      gain_code_old_rx[i] = 0;
//  }
    VoC_loop_i_short(14,DEFAULT);
    VoC_startloop0;
    VoC_sw32inc_p(REG67,REG2,DEFAULT);
    VoC_endloop0;
//  prev_SID_frames_lost = 0;
    VoC_sw16_d(REG7, STATIC_PREV_SID_FRAMES_LOST_ADDR);
    VoC_lw16i(REG6,0x7fff);
//  buf_p_rx = 0;
    VoC_sw16_d(REG7, STATIC_BUF_P_RX_ADDR);
    VoC_lw16i_short(REG7,7,DEFAULT);
//  rxdtx_N_elapsed = 0x7fff;
    VoC_sw16_d(REG6,STATIC_RXDTX_N_ELAPSED_ADDR);
    VoC_lw16i_short(REG6,1,DEFAULT);
//  rxdtx_aver_period = DTX_HANGOVER;
    VoC_sw16_d(REG7,STATIC_RXDTX_AVER_PERIOD_ADDR);
    VoC_lw32_d(REG67,CONST_0x70816958_ADDR);
//  rxdtx_ctrl = RX_SP_FLAG;
    exit_on_warnings=OFF;
    VoC_sw16_d(REG6,GLOBAL_RXDTX_CTRL_ADDR);
    exit_on_warnings=ON;
    VoC_lw16i_short(REG6,23,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32_d(REG67,GLOBAL_L_PN_SEED_RX_ADDR);
    exit_on_warnings=ON;
    VoC_sw16_d(REG6,GLOBAL_RX_DTX_STATE_ADDR);

//    for (i = 0; i < DTX_HANGOVER; i++)
//    {
//        lsf_old_rx[i][0] = 1384;
//        lsf_old_rx[i][1] = 2077;
//        lsf_old_rx[i][2] = 3420;
//        lsf_old_rx[i][3] = 5108;
//        lsf_old_rx[i][4] = 6742;
//        lsf_old_rx[i][5] = 8122;
//        lsf_old_rx[i][6] = 9863;
//        lsf_old_rx[i][7] = 11092;
//        lsf_old_rx[i][8] = 12714;
//        lsf_old_rx[i][9] = 13701;
//    }

    VoC_lw16i_set_inc(REG2,TABLE_MEAN_LSF_ADDR,1);
    VoC_lw16i_set_inc(REG1,GLOBAL_LSF_OLD_RX_ADDR,1);
    VoC_loop_i(1,7);
    VoC_loop_i_short(10,DEFAULT);
    VoC_lw16inc_p(REG6,REG2,IN_PARALLEL);
    VoC_startloop0;
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;
    VoC_lw16i(REG2,TABLE_MEAN_LSF_ADDR);
    VoC_NOP();
    VoC_endloop1;

    VoC_pop16(REG1,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);

    VoC_pop16(REG2,DEFAULT);


    VoC_return;


}


void CII_efr_dtx_func1(void)
{








    VoC_push16(RA,DEFAULT);

    VoC_lw16_d(REG2,GLOBAL_RXDTX_CTRL_ADDR);
    VoC_and16_ri(REG2,RX_UPD_SID_QUANT_MEM);
    VoC_bez16_r(D_PLSF100,REG2);                    //if ((rxdtx_ctrl & RX_UPD_SID_QUANT_MEM) != 0)

    VoC_lw16i_set_inc(REG0,GLOBAL_LSF_OLD_RX_ADDR,10);
    VoC_lw16i_set_inc(REG1, DEC_STATIC_LSF_P_CN_ADDR,1);
    VoC_jal(CII_update_lsf_p_CN);


D_PLSF100:
    /* Handle cases of comfort noise LSF decoding in which past
    valid SID frames are repeated */
    VoC_lw16_d(REG1,GLOBAL_RXDTX_CTRL_ADDR);
    VoC_movreg16(REG2,REG1,DEFAULT);
    VoC_and16_ri(REG2,RX_NO_TRANSMISSION);
    VoC_bnez16_r(D_PLSF1001_1,REG2);        //if (((rxdtx_ctrl & RX_NO_TRANSMISSION) != 0)
    VoC_movreg16(REG3,REG1,DEFAULT);
    VoC_and16_ri(REG3,RX_INVALID_SID_FRAME);
    VoC_bnez16_r(D_PLSF1001_1,REG3);        //((rxdtx_ctrl & RX_INVALID_SID_FRAME) != 0)
    VoC_movreg16(REG3,REG1,DEFAULT);
    VoC_and16_ri(REG3,RX_LOST_SID_FRAME);
    VoC_bez16_r(amr_dec_122_EFR10,REG3);     //((rxdtx_ctrl & RX_LOST_SID_FRAME) != 0))

D_PLSF1001_1:
    VoC_bez16_r(D_PLSF1001,REG2);       //if ((rxdtx_ctrl & RX_NO_TRANSMISSION) != 0)

    VoC_lw16i(REG2, DEC_AMR_TEMP10_ADDRESS);
    //  REG2 for lsf2_q
    VoC_jal(CII_interpolate_CN_lsf);
    VoC_jump(D_PLSF1002);

D_PLSF1001:

    VoC_lw32z(RL6,DEFAULT);
    VoC_lw16i_set_inc(REG0, GLOBAL_LSF_OLD_CN_DEC_ADDR,2);
    VoC_lw16i_set_inc(REG1, GLOBAL_LSF_NEW_CN_DEC_ADDR,2);
    //  REG2 for lsf2_q
    VoC_lw16i_set_inc(REG2, DEC_AMR_TEMP10_ADDRESS,2);
    //  REG3 for st
    VoC_lw16_sd(REG3,2,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    //  REG3 for st->past_r_q[i]
    VoC_add16_rd(REG3,REG3,CONST_10_ADDR);
    VoC_loop_i_short(5,DEFAULT);
    VoC_startloop0;
    VoC_lw32inc_p(REG67,REG1,DEFAULT);
//              VoC_NOP();
    VoC_sw32inc_p(RL6,REG3,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG2,DEFAULT);
    VoC_endloop0;

D_PLSF1002:
    // REG2 for lsf2_q[i]
    VoC_lw16i_set_inc(REG2, DEC_AMR_TEMP10_ADDRESS,2);
    // REG3 for st->past_lsf_q[i]
    VoC_lw16_sd(REG3,2,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    //  for (i = 0; i < M; i++)
    //  {
    //    st->past_lsf_q[i] = lsf2_q[i];          move16 ();
    //  }
    VoC_loop_i_short(5,DEFAULT);
    VoC_lw32inc_p(REG67,REG2,IN_PARALLEL);
    VoC_startloop0;
    VoC_lw32inc_p(REG67,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(REG67,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;

    // REG0 for lsp2_q
    VoC_lw16i_set_inc(REG0,DEC_AMR_TEMP10_ADDRESS,1);   //Lsf_lsp (lsf2_q, lsp2_q, M);
    // REG1 for lsp2_q
    VoC_lw16i_set_inc(REG1,DEC_AMR_LSP_NEW_ADDRESS,1);
    VoC_jal(CII_Lsf_lsp);




    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_jump(amr_dec_122_EFR11);

amr_dec_122_EFR10:
    VoC_lw16i_short(REG6,1,DEFAULT);

amr_dec_122_EFR11:

    VoC_pop16(RA,DEFAULT);
    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_lw16_sd(REG1,2,DEFAULT);
    VoC_return;
}



void CII_efr_dtx_func2(void)
{
    VoC_lw16_d(REG4,GLOBAL_RXDTX_CTRL_ADDR);
    VoC_and16_ri(REG4,1);
    VoC_bnez16_r(Dplsf5_LEnd_EFR10,REG4);
    // REG1 for &st->past_r_q[i]
    // REG3 for &lsf1_r[i] & &lsf1_q[i],notice they share the same address

    VoC_lw16i_short(INC0,1,DEFAULT);
    // REG0 for &lsf2_r[i] & &lsf2_q[i],notice they share the same address
    VoC_add16_rd(REG0,REG3,CONST_10_ADDR);
    // REG2 for &lsf_p_CN[i]
    VoC_lw16i_set_inc(REG2,DEC_STATIC_LSF_P_CN_ADDR,1);
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_loop_i_short(10,DEFAULT);               //    for (i = 0; i < M; i++)
    VoC_startloop0
    // REG4 for lsf_p_CN[i]
    VoC_lw16inc_p(REG4,REG2,DEFAULT);   //        temp = add (mean_lsf[i], mult (st->past_r_q[i],LSP_PRED_FAC_MR122));
    // REG4 for add16(lsf_p_CN[i],lsf_2_r[i])
    VoC_add16_rp(REG4,REG4,REG0,DEFAULT);
    // save st->past_r_q[i] = 0
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    // save lsf2_q[i]
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    // save lsf1_q[i]
    VoC_sw16inc_p(REG4,REG3,DEFAULT);

    VoC_endloop0

    VoC_lw16i_short(REG4,0,DEFAULT);

Dplsf5_LEnd_EFR10:

    VoC_return;
}



void CII_efr_dtx_func3 (void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16_d(REG6,GLOBAL_RXDTX_CTRL_ADDR);
    VoC_movreg16(REG2,REG6,DEFAULT);
    VoC_and16_ri(REG2,RX_FIRST_SID_UPDATE);//(rxdtx_ctrl & RX_FIRST_SID_UPDATE)

    VoC_lw16i_set_inc(REG1 ,GLOBAL_LSF_NEW_CN_DEC_ADDR,2); //REG0 : addr of lsf_new_CN
    VoC_lw16i_set_inc(REG0 ,DEC_AMR_TEMP10_ADDRESS,2); //REG1 : addr of lsf2_q

    VoC_bez16_r(D_PLSF200,REG2);

    VoC_jal(CII_Copy_M);

D_PLSF200:

    VoC_movreg16(REG2,REG6,DEFAULT);
    VoC_and16_ri(REG2,RX_CONT_SID_UPDATE);
    VoC_bez16_r(D_PLSF201,REG2);

    VoC_lw16i_set_inc(REG2 ,GLOBAL_LSF_OLD_CN_DEC_ADDR,2); //REG2 : addr of lsf_old_CN

    VoC_loop_i_short(M/2,DEFAULT); //change to 2 loop only short 2 cycles
    VoC_startloop0
    VoC_lw32_p(REG45,REG1,DEFAULT); //REG45 : value of lsf_new_CN
    VoC_lw32inc_p(RL7,REG0,DEFAULT); //REG4 : value of lsf2_q  INCR[REG1]
    VoC_sw32inc_p(REG45,REG2,DEFAULT);//lsf_old_CN[i] = lsf_new_CN[i];

    VoC_sw32inc_p(RL7,REG1,DEFAULT);//lsf_new_CN[i] = lsf2_q[i];
    VoC_endloop0;


D_PLSF201: //if ((rxdtx_ctrl & RX_SP_FLAG) != 0)

    VoC_movreg16(REG2,REG6,DEFAULT);
    VoC_and16_ri(REG2,RX_SP_FLAG);
    VoC_bez16_r(D_PLSF202,REG2);

    VoC_lw16i_set_inc(REG1,DEC_AMR_TEMP10_ADDRESS,-10);

    VoC_lw16i_set_inc(REG0,DEC_AMR_BUFA40_ADDRESS,-10);
    //if (bfi==0)
    VoC_bez16_d(D_PLSF2011,GLOBAL_BFI_ADDR);


    VoC_lw16i_set_inc(REG0,GLOBAL_LSF_NEW_CN_DEC_ADDR,-10);
    VoC_movreg16(REG1,REG0,DEFAULT);

D_PLSF2011:

    VoC_movreg32(RL6,REG01,DEFAULT);
    VoC_lw16i(REG0,GLOBAL_LSF_OLD_RX_ADDR);


    VoC_jal(CII_update_lsf_history);
    VoC_lw16i_set_inc(REG1 ,GLOBAL_LSF_OLD_CN_DEC_ADDR,2); //REG0 : addr of lsf_old_CN
    VoC_lw16i_set_inc(REG0 ,DEC_AMR_TEMP10_ADDRESS,2); //REG1 : addr of lsf2_q

    VoC_jal(CII_Copy_M);


    VoC_jump(D_PLSF203);

D_PLSF202:


    VoC_lw16i(REG2,DEC_AMR_TEMP10_ADDRESS);
    VoC_jal(CII_interpolate_CN_lsf);

D_PLSF203:

    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_return;
}


void CII_efr_dtx_func4(void)
{

    VoC_lw16_d(REG7,GLOBAL_RXDTX_CTRL_ADDR);
    VoC_and16_ri(REG7,1);
    VoC_bnez16_r(CII_Int_lpc_1and3_dec_EFR10,REG7);
    // REG0 for A_t
    VoC_movreg16(REG0,REG3,DEFAULT);
    // REG1 for lsp_new
    VoC_movreg16(REG1,REG5,IN_PARALLEL);


    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_push16(RA,DEFAULT);

    VoC_jal(CII_Lsp_Az);

    VoC_pop16(RA,DEFAULT);
    // REG3 for &A_t
    VoC_lw32_sd(REG23,0,DEFAULT);
    VoC_lw16i_short(REG4,11,IN_PARALLEL);
    // REG0 for &A_t[MP1]
    VoC_add16_rr(REG0,REG3,REG4,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    // REG1 for &A_t[2*MP1]
    VoC_add16_rr(REG1,REG0,REG4,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    // REG2 for &A_t[3*MP1]
    VoC_add16_rr(REG2,REG1,REG4,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_lw16inc_p(REG7,REG3,DEFAULT);

    VoC_loop_i_short(11,DEFAULT);
    VoC_startloop0;
    //A_t[i + MP1] = A_t[i];
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    //A_t[i + 2 * MP1] = A_t[i];
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_lw16inc_p(REG7,REG3,DEFAULT);
    exit_on_warnings=OFF;
    //A_t[i + 3 * MP1] = A_t[i];
    VoC_sw16inc_p(REG7,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0;
    VoC_lw16i_short(INC0,2,DEFAULT);

    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_lw16i_short(REG7,0,DEFAULT);

CII_Int_lpc_1and3_dec_EFR10:
//   REG7=1 if it is jumped here

    VoC_return;
}

void CII_efr_dtx_func5(void)
{

    VoC_push16(RA,DEFAULT);

    VoC_lw16_d(REG7,GLOBAL_RXDTX_CTRL_ADDR);            //REG7:rxdtx_ctrl

    VoC_and16_rd(REG7,CONST_2_ADDR);//reg1 value of (rxdtx_ctrl & RX_UPD_SID_QUANT_MEM)
    VoC_bez16_r(D_GAIN200,REG7);
    // REG7 for i_subfr
    VoC_lw16_sd(REG7,1,DEFAULT);
    VoC_bnez16_r(D_GAIN200,REG7);//if i_subfr != 0 jump 200


    VoC_lw16i_set_inc(REG0,GLOBAL_GAIN_CODE_OLD_RX_ADDR,2);

// inline FUNC CII_update_gcode0_CN BEGIN

    VoC_jal(CII_update_gcode0_CN);

// inline FUNC CII_update_gcode0_CN END

    VoC_shr16_ri(REG5,-4,DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG5,GLOBAL_GCODE0_CN_DEC_ADDR);

D_GAIN200:

    /* Handle cases of comfort noise fixed codebook gain decoding in
    which past valid SID frames are repeated */

    VoC_lw16_d(REG7,GLOBAL_RXDTX_CTRL_ADDR);            //REG7:rxdtx_ctrl
    VoC_movreg16(REG1,REG7,DEFAULT);
    VoC_and16_rd(REG1,CONST_RX_NO_TRANSMISSION_ADDR);
    VoC_bnez16_r(D_GAIN202,REG1);//reg1:rxdtx_ctrl&RX_NO_TRANSMISSION

    VoC_movreg16(REG2,REG7,DEFAULT);
    VoC_and16_rd(REG2,CONST_RX_INVALID_SID_FRAME_ADDR);
    VoC_bnez16_r(D_GAIN202,REG2);//reg2:rxdtx_ctrl&RX_INVALID_SID_FRAME

    VoC_movreg16(REG3,REG7,DEFAULT);
    VoC_and16_rd(REG3,CONST_RX_LOST_SID_FRAME_ADDR);
    VoC_bez16_r(gc_pred_M122_dec_EFR10,REG3);//reg3:rxdtx_ctrl&RX_LOST_SID_FRAME

D_GAIN202:
    VoC_bez16_r(D_GAIN203,REG1)

    /* DTX active: no transmission. Interpolate gain values
    in memory */

    VoC_lw16_d(REG3,STRUCT_EC_GAIN_CODESTATE_PREV_GC_ADDR);     //REG3:*gain_code
    // REG7 for i_subfr
    VoC_lw16_sd(REG1,1,DEFAULT);

    VoC_bnez16_r(D_GAIN210,REG1);


    VoC_jal(CII_interpolate_CN_param);// REG3 return value


    VoC_jump(D_GAIN210);

D_GAIN203:  //else

    /* Invalid or lost SID frame:
    use gain values from last good SID frame */

    VoC_lw16_d(REG3,GLOBAL_GAIN_CODE_NEW_CN_DEC_ADDR); //reg3:*gain_code
    VoC_lw16i_set_inc(REG1,STRUCT_GC_PREDSTATE_PAST_QUA_EN_MR122_ADDR,1);   //CHANGE
    VoC_sw16_d(REG3,GLOBAL_GAIN_CODE_OLD_CN_DEC_ADDR); // gain_code_old_CN = gain_code_new_CN;

    /* reset table of past quantized energies */

    VoC_lw16i(REG2,-2381);

    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG2,REG1,DEFAULT);               //past_qua_en[i] = -2381;
    VoC_endloop0;


D_GAIN210:

    VoC_lw16_d(REG1,GLOBAL_RXDTX_CTRL_ADDR);    //REG7:rxdtx_ctrl
    VoC_and16_rd(REG1,CONST_RX_DTX_MUTING_ADDR);//reg1 value of (rxdtx_ctrl & RX_DTX_MUTING)

    VoC_lw16_d(REG4,GLOBAL_GAIN_CODE_NEW_CN_DEC_ADDR);//reg4:gain_code_muting_CN
    VoC_bez16_r(D_GAIN212,REG1);                        //if reg2.reg=0,jump 212
    VoC_lw16i(REG2,30057);//reg2 30057
    VoC_multf16_rd(REG4,REG2,GLOBAL_GAIN_CODE_MUTING_CN_DEC_ADDR);//reg3:*gain_code
    VoC_NOP();
    VoC_movreg16(REG3,REG4,DEFAULT);

D_GAIN212:
    VoC_sw16_d(REG4,GLOBAL_GAIN_CODE_MUTING_CN_DEC_ADDR);
    VoC_sw16_d(REG3,STRUCT_EC_GAIN_CODESTATE_PAST_GAIN_CODE_ADDR);//past_gain_code = *gain_code;
    VoC_sw16_d(REG3,DEC_AMR_GAIN_CODE_ADDRESS);//save *gain_code

    VoC_lw16i_set_inc(REG1,STRUCT_EC_GAIN_CODESTATE_ADDR,1);//reg1 addr of gbuf[0]
    VoC_lw16i_set_inc(REG2,STRUCT_EC_GAIN_CODESTATE_ADDR+1,1);//reg2 addr of gbuf[1]


    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG2,DEFAULT);
    VoC_sw16_d(REG3,STRUCT_EC_GAIN_CODESTATE_PREV_GC_ADDR);//prev_gc = past_gain_code
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0;

    VoC_sw16_p(REG3,REG1,DEFAULT);//gbuf[4] = past_gain_code

    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_return;

gc_pred_M122_dec_EFR10:
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(REG0,1,DEFAULT);

    VoC_return;

}

void CII_efr_dtx_func6(void)
{


    VoC_lw16_d(REG7,GLOBAL_RXDTX_CTRL_ADDR);
    VoC_and16_ri(REG7,1);
    VoC_bnez16_r(d_gain_code_EFR10,REG7);

    VoC_lw16_d(REG1,GLOBAL_RXDTX_CTRL_ADDR);            //REG7:rxdtx_ctrl

    // REG6 for index
    VoC_lw16i_short(REG6,3,DEFAULT);
    // REG6 for 3*index
    VoC_mult16_rd(REG6,REG6,DEC_AMR_INDEX_ADDRESS);

    VoC_lw16i(REG7, STATIC_CONST_qua_gain_code_ADDR);
    // REG6 for &qua_gain_code[3*index]
    VoC_add16_rr(REG6,REG7,REG6,DEFAULT);

    VoC_and16_rd(REG1,CONST_4_ADDR);//reg1 value of (rxdtx_ctrl & RX_FIRST_SID_UPDATE)

    // RL6_LO for qua_gain_code[3*index]
    VoC_lw16_p(RL6_LO,REG6,DEFAULT);

    VoC_bez16_r(D_GAIN510,REG1);//if reg1.reg=0,jump 510

    // REG1 for i_subfr
    VoC_lw16_sd(REG1,2,DEFAULT);


    VoC_bnez16_r(D_GAIN510,REG1);//if (i_subfr != 0),jump D_GAIN510

    VoC_movreg16(REG6,RL6_LO,DEFAULT);      //qua_gainc_code[index]
    VoC_multf16_rd(REG6,REG6,GLOBAL_GCODE0_CN_DEC_ADDR);//reg7 mult (gcode0_CN, qua_gain_code[index])
    VoC_lw16i_set_inc(REG1,STRUCT_GC_PREDSTATE_PAST_QUA_EN_MR122_ADDR,1);//reg1 addr of past_qua_en[0]
    VoC_sw16_d(REG6,GLOBAL_GAIN_CODE_NEW_CN_DEC_ADDR);

    VoC_movreg16(REG6,RL6_LO,DEFAULT);      //qua_gainc_code[index]
    VoC_multf16_rd(REG6,REG6,GLOBAL_GCODE0_CN_DEC_ADDR);//reg7 mult (gcode0_CN, qua_gain_code[index])

    VoC_lw16i(REG7,-2381);
    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG7,REG1,DEFAULT);//past_qua_en[i] = -2381;
    VoC_endloop0;

    VoC_sw16_d(REG6,GLOBAL_GAIN_CODE_NEW_CN_DEC_ADDR);


D_GAIN510:

    VoC_lw16_d(REG1,GLOBAL_RXDTX_CTRL_ADDR);
    VoC_and16_rd(REG1,CONST_8_ADDR);//reg1 value of (rxdtx_ctrl & RX_CONT_SID_UPDATE)
    VoC_bez16_r(D_GAIN520,REG1);//if reg1.reg=0,jump 520
    // REG1 for i_subfr
    VoC_lw16_sd(REG1,2,DEFAULT);
    VoC_movreg16(REG3,RL6_LO,DEFAULT);      //qua_gainc_code[index]
    VoC_multf16_rd(REG3,REG3,GLOBAL_GCODE0_CN_DEC_ADDR);//reg7 mult (gcode0_CN, qua_gain_code[index])

    VoC_bnez16_r(D_GAIN520,REG1);//

    VoC_lw16_d(REG4,GLOBAL_GAIN_CODE_NEW_CN_DEC_ADDR);//reg4 gain_code_new_cn
    VoC_sw16_d(REG3,GLOBAL_GAIN_CODE_NEW_CN_DEC_ADDR);
    VoC_sw16_d(REG4,GLOBAL_GAIN_CODE_OLD_CN_DEC_ADDR);//gain_code_old_CN = gain_code_new_CN;



D_GAIN520:

    VoC_lw16_d(REG3,STRUCT_EC_GAIN_CODESTATE_PREV_GC_ADDR);//reg3:*gain_code
    // REG7 for i_subfr
    VoC_lw16_sd(REG7,2,DEFAULT);
    VoC_bnez16_r(D_GAIN522,REG7);//if (i_subfr != 0),jump 522

    VoC_push16(RA,DEFAULT);
    VoC_jal(CII_interpolate_CN_param);// REG3 return value
    VoC_pop16(RA,DEFAULT);

D_GAIN522:

    VoC_sw16_d(REG3,DEC_AMR_GAIN_CODE_ADDRESS);


    VoC_lw16i_short(REG7,0,DEFAULT);
d_gain_code_EFR10:
    // REG7 =1 if it is jumped here
    VoC_return;
}

void CII_efr_dtx_func7(void)
{
    VoC_push16(RA,DEFAULT);
    // REG7 for gain_code
    VoC_lw16_d(REG5,DEC_AMR_GAIN_CODE_ADDRESS);
    // gain_code=shr(gain_code,1)
    //VoC_shr16_ri(REG5,1,DEFAULT);

    //if (prev_bf != 0)
    VoC_bez16_d(D_GAIN403,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);
    //if (sub (gain_code, st->ec_gain_c_st->prev_gc) > 0)
    VoC_bngt16_rd(D_GAIN403,REG5,STRUCT_EC_GAIN_CODESTATE_PREV_GC_ADDR)
    // gain_code = st->ec_gain_c_st->prev_gc;
    VoC_lw16_d(REG5,STRUCT_EC_GAIN_CODESTATE_PREV_GC_ADDR);
D_GAIN403:

    VoC_jal(CII_update_gain_code_history_rx);//REG5 input new_gain_code
    // REG1 for i_subfr
    VoC_lw16_sd(REG1,3,DEFAULT);
    // if (sub (i_subfr, (3 * L_SUBFR)) == 0)
    VoC_bne16_rd(d_gain_code_end_EFR1,REG1,CONST_120_ADDR);
    // gain_code_old_CN = gain_code
    VoC_sw16_d(REG5,GLOBAL_GAIN_CODE_OLD_CN_DEC_ADDR);
d_gain_code_end_EFR1:
    VoC_pop16(RA,DEFAULT);
    // save gain code
    VoC_sw16_d(REG5,DEC_AMR_GAIN_CODE_ADDRESS);
    VoC_return;
}

void CII_efr_dtx_func8(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16_d(REG5,GLOBAL_GAIN_CODE_NEW_CN_DEC_ADDR);  //reg5:(tmp)gain_code_new_cn

    VoC_bnez16_r(D_GAIN304,REG5);// if (gain_code_new_CN != 0)
    VoC_lw16_d(REG5,STRUCT_EC_GAIN_CODESTATE_PREV_GC_ADDR);

D_GAIN304:


    //D_GAIN305:
    VoC_jal(CII_update_gain_code_history_rx);//REG5 input new_gain_code

    VoC_pop16(RA,DEFAULT);
    // REG5 for i_subfr
    VoC_lw16_sd(REG5, 3, DEFAULT);
    VoC_lw16_d(REG3,DEC_AMR_GAIN_CODE_ADDRESS);//reg3 gain_code

    VoC_bne16_rd(D_GAIN306,REG5,CONST_120_ADDR);//if (sub (i_subfr, (3 * L_SUBFR)) != 0)

    VoC_sw16_d(REG3,GLOBAL_GAIN_CODE_OLD_CN_DEC_ADDR);//gain_code_old_CN = *gain_code;

D_GAIN306:

    VoC_return;
}


void CII_efr_dtx_func9(void)
{
    // REG4 for rxdtx_ctrl
    VoC_lw16_d(REG4,GLOBAL_RXDTX_CTRL_ADDR);
    // REG5 for rxdtx_ctrl
    VoC_movreg16(REG5,REG4,DEFAULT);
    VoC_and16_ri(REG4,0x400);
    VoC_bez16_r(Dec_amr_L112_EFR,REG4);                  // if ((rxdtx_ctrl & RX_FIRST_SP_FLAG) != 0)
    VoC_and16_ri(REG5,0x100);                       // if ((rxdtx_ctrl & RX_PREV_DTX_MUTING) != 0)
    VoC_lw16i_short(REG3,5,DEFAULT);            //{  st->state = 5;
    VoC_bez16_r(Dec_amr_L10_EFR,REG5);
    VoC_lw16i_short(REG5,1,DEFAULT);        //   st->prev_bf = 1;
Dec_amr_L10_EFR:                                        // else
    VoC_NOP();
    VoC_sw16_d(REG5,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);
Dec_amr_L112_EFR:
    VoC_return;
}



void CII_energy_computation (void)
{
//ene_com++;

    /* Computation of acf0 (exponent and mantissa) */


    VoC_lw16i(REG1,32);
    VoC_sub16_rd(REG3,REG1,SCAL_ACF_ADDR_P);    //reg3:acf0->e
    VoC_lw16_d(REG2,LPC_RHIGH_ADDR);
    VoC_and16_ri (REG2, 0x7ff8);                //reg2:acf0->m
    /* Computation of pvad (exponent and mantissa) */
    VoC_lw16i_short(REG0,14,DEFAULT);
    exit_on_warnings =OFF;
    VoC_sw32_d(REG23,VAD_COMPUTATION_ACF0_ADDR);
    exit_on_warnings =ON;
    VoC_add16_rr (REG3, REG3, REG0, DEFAULT);

    VoC_sub16_rd (REG3, REG3,STATIC_SCAL_RVAD_ADDR);

    VoC_lw32z (ACC0,DEFAULT);
    VoC_sw16_d(REG3,VAD_COMPUTATION_PVAD_ADDR);


    VoC_lw16i_set_inc (REG2,LPC_RHIGH_ADDR+1,1);
    VoC_lw16i_set_inc(REG3,STATIC_RVAD_ADDR+1,1);

    VoC_loop_i_short( 8,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p (REG1,REG2,DEFAULT) ;
    VoC_shr16_ri (REG1, 3,DEFAULT);
    VoC_mac32inc_rp (REG1, REG3, DEFAULT);
    VoC_endloop0


    VoC_lw16_d (REG1,LPC_RHIGH_ADDR) ;
    VoC_shr16_ri (REG1, 3,DEFAULT);
    VoC_multf32_rd (REG45, REG1,STATIC_RVAD_ADDR);

    VoC_NOP();
    VoC_shr32_ri (REG45, 1, DEFAULT);
    VoC_add32_rr (REG45, REG45, ACC0, DEFAULT);

    VoC_bgtz32_r(ENERGY_COMPUTATION101,REG45);
    VoC_lw16i_short(REG4,1, DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

ENERGY_COMPUTATION101:
    VoC_NOP();
    /*****************************************************************************
     * norm_l(REG0,REG45)
     * INPUT:           REG45
     * OUTPUT:          REG0,(REG45 is normlized)
     * REQUIREMENTS:    REG0 inc val=1;PAGE[2]=2;
     * USED:            REG45,loop_reg[0]
     *****************************************************************************/
//      VoC_lw16z(REG2,DEFAULT);
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_loop_i_short(31,DEFAULT)
    VoC_startloop0
    VoC_bnlt32_rd(VAD_NORM_L_1_EXIT,REG45,CONST_0x40000000_ADDR)
    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_endloop0

VAD_NORM_L_1_EXIT:


    VoC_sub16_dr(REG4,VAD_COMPUTATION_PVAD_ADDR,REG2);


    VoC_return;
}


/****************************************************************************
 *
 *     FUNCTION:  acf_averaging
 *
 *     PURPOSE:   Computes the arrays L_av0[0..8] and L_av1[0..8].
 *
 *     INPUTS:    r_h[0..8]     autocorrelation of input signal frame (msb)
 *                r_l[0..8]     autocorrelation of input signal frame (lsb)
 *                scal_acf      scaling factor for the autocorrelations
 *
 *     OUTPUTS:   L_av0[0..8]   ACF averaged over last four frames
 *                L_av1[0..8]   ACF averaged over previous four frames
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/

void CII_acf_averaging (void)
{

    VoC_lw16_d(REG1,SCAL_ACF_ADDR_P);
    VoC_add16_rd (REG5,REG1,CONST_9_ADDR);              //scale


    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_short(REG7,34,IN_PARALLEL);
    VoC_movreg32(RL7,REG67,DEFAULT);                        //for 0 and 34

    VoC_lw16i_set_inc(REG3,STATIC_L_SACF_ADDR,18);          //&L_sacf[i]
    VoC_lw16i_set_inc(REG2,VAD_COMPUTATION_L_AV0_ADDR,2);   //&L_av0[i]

    VoC_lw16i_short(REG4,2,DEFAULT);
    VoC_mult16_rd (REG4,REG4,STATIC_PT_SACF_ADDR);

    VoC_lw16i_set_inc(REG0,LPC_RHIGH_ADDR,1);
    VoC_add16_rr (REG4, REG3, REG4, DEFAULT);               //&L_sacf[pt_sacf+i]

    VoC_lw16i_set_inc(REG1,LPC_RLOW_ADDR,1);
    VoC_loop_i_short(9,DEFAULT)
    VoC_startloop0

    VoC_multf32inc_pd(ACC0,REG1,CONST_1_ADDR);
    VoC_lw16inc_p(REG7,REG0,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_add32_rr(ACC0,REG67,ACC0,DEFAULT);

    VoC_shr32_rr(ACC0,REG5,DEFAULT);            //L_temp

    VoC_add32inc_rp(REG67,ACC0,REG3,DEFAULT);
    VoC_add32inc_rp(REG67,REG67,REG3,DEFAULT);
    VoC_add32_rp(REG67,REG67,REG3,DEFAULT);
    VoC_sw32_p(ACC0,REG4,DEFAULT);
    VoC_add16_rd(REG4,REG4,CONST_2_ADDR);
    VoC_sw32inc_p(REG67,REG2,DEFAULT);
    VoC_sub32_rr(REG23,REG23,RL7,IN_PARALLEL);

    VoC_endloop0


    VoC_lw16i_short(REG6,2,DEFAULT);
    VoC_mult16_rd (REG0, REG6,STATIC_PT_SAV0_ADDR);
    VoC_lw16i_set_inc(REG3,STATIC_L_SAV0_ADDR,2);
    VoC_add16_rr (REG3, REG0, REG3, DEFAULT);    // reg3 address of the L_SAV0[pt_sav0]
    VoC_movreg16(REG2,REG3,DEFAULT);            //

    VoC_lw16i_set_inc(REG0,VAD_COMPUTATION_L_AV0_ADDR,2);
    VoC_lw16i_set_inc(REG1,VAD_COMPUTATION_L_AV1_ADDR,2);

    VoC_lw32inc_p(REG67,REG2,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    /*ADDR CHANGE*/
    VoC_loop_i_short(9,DEFAULT)
    VoC_startloop0

    VoC_sw32inc_p(REG67,REG1,DEFAULT);
    VoC_lw32inc_p(REG67,REG2,DEFAULT);
    VoC_sw32inc_p(REG45,REG3,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_endloop0


    /* Update the array pointers */
    VoC_lw16i_short(REG4,18,DEFAULT);
    VoC_lw16i_short(REG5,9,IN_PARALLEL);

    VoC_add16_rd(REG6,REG5,STATIC_PT_SACF_ADDR);    //REG6:pt_sacf

    VoC_bne16_rd(ACF_AVERAGING101,REG4,STATIC_PT_SACF_ADDR);
//      VoC_lw16z(REG6,DEFAULT);
    VoC_lw16i_short(REG6,0,DEFAULT);

ACF_AVERAGING101:
//VoC_NOP();
    VoC_lw16i_short(REG4,27,DEFAULT);

    VoC_add16_rd(REG7,REG5,STATIC_PT_SAV0_ADDR);//REG7:pt_sav0

    VoC_bne16_rd(ACF_AVERAGING103,REG4,STATIC_PT_SAV0_ADDR)
//      VoC_lw16z(REG6,DEFAULT);
    VoC_lw16i_short(REG7,0,DEFAULT);
ACF_AVERAGING103:
//VoC_NOP();
    VoC_sw16_d(REG6,STATIC_PT_SACF_ADDR);
    VoC_sw16_d(REG7,STATIC_PT_SAV0_ADDR);

    VoC_return;
}


/****************************************************************************
 *
 *     FUNCTION:  predictor_values
 *
 *     PURPOSE:   Computes the array rav[0..8] needed for the spectral
 *                comparison and the threshold adaptation.
 *
 *     INPUTS:    L_av1[0..8]   ACF averaged over previous four frames
 *
 *     OUTPUTS:   rav1[0..8]    ACF obtained from L_av1
 *                *scal_rav1    rav1[] scaling factor
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/


void CII_predictor_values (void)
{
    VoC_push16(RA,DEFAULT);
    VoC_jal(CII_schur_recursion);

    VoC_lw16i(REG0,VAD_PRED_VAL_VPAR_ADDR);
    VoC_lw16i(REG2,VAD_PRED_VAL_AAV1_ADDR);
    VoC_lw16i_short(REG3,8,DEFAULT);
    VoC_movreg32(RL6,REG23,DEFAULT);            //&aav1 and np

    VoC_jal(CII_step_up);
    VoC_jal(CII_compute_rav1);
    VoC_pop16 (RA,DEFAULT);
    VoC_NOP();
    VoC_return;


}
/****************************************************************************
 *
 *     FUNCTION:  schur_recursion
 *
 *     PURPOSE:   Uses the Schur recursion to compute adaptive filter
 *                reflection coefficients from an autorrelation function.
 *
 *     INPUTS:    L_av1[0..8]    autocorrelation function
 *
 *     OUTPUTS:   vpar[0..7]     reflection coefficients
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/



void CII_schur_recursion (void)
{
#if 0
    voc_short    VAD_SCHUR_RECUR_KK_ADDR_ADDR           ,9,y

#endif


    /*** Schur recursion with 16-bit arithmetic ***/


    VoC_bnez32_d(SCHUR_RECURSION101,VAD_COMPUTATION_L_AV1_ADDR);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_set_inc (REG0,VAD_PRED_VAL_VPAR_ADDR,2);
    VoC_loop_i_short(4,DEFAULT)
    VoC_startloop0
    VoC_sw32inc_p (ACC0,REG0, DEFAULT);
    VoC_endloop0

    VoC_return

SCHUR_RECURSION101:


    VoC_lw32_d (ACC0,VAD_COMPUTATION_L_AV1_ADDR);

    VoC_lw16i_set_inc(REG3,0,-1);
    VoC_loop_i_short(31,DEFAULT)
    VoC_startloop0
    VoC_bnlt32_rd(SCHUR_NORM_L_1_EXIT,ACC0,CONST_0x40000000_ADDR)
    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_endloop0

SCHUR_NORM_L_1_EXIT:


//  VoC_sub16_dr(REG4,CONST_0_ADDR,REG0);
    //reg2:-temp

    VoC_lw16i_set_inc(REG0,VAD_COMPUTATION_L_AV1_ADDR,2);
    VoC_lw16i_set_inc(REG1,VAD_SCHUR_RECUR_PP_ADDR_ADDR,1);
    VoC_lw16i_set_inc(REG2,VAD_SCHUR_RECUR_KK_ADDR_ADDR+9,-1);

    VoC_lw32inc_p(REG45,REG0, DEFAULT);
    VoC_lw32inc_p(REG67,REG0,DEFAULT);

    VoC_loop_i_short(5,DEFAULT);
    VoC_startloop0
    VoC_shr32_rr (REG45, REG3, DEFAULT);
    VoC_shr32_rr (REG67, REG3, DEFAULT);
    VoC_sw16inc_p (REG5,REG1, DEFAULT);
    VoC_sw16inc_p (REG5,REG2, DEFAULT);
    VoC_lw32inc_p (REG45,REG0, IN_PARALLEL);
    VoC_sw16inc_p (REG7,REG1, DEFAULT);
    VoC_sw16inc_p (REG7,REG2, DEFAULT);
    VoC_lw32inc_p (REG67,REG0, IN_PARALLEL);

    VoC_endloop0


    /*** Compute Parcor coefficients: ***/

    VoC_lw16i_set_inc(REG1,VAD_PRED_VAL_VPAR_ADDR,1);   //&vpar[n]

    VoC_lw16i(RL6_LO,VAD_PRED_VAL_VPAR_ADDR+8);
    VoC_lw16i(RL6_HI,VAD_PRED_VAL_VPAR_ADDR+7);     //RL6:&vpar[8] and &vpar[7]

    VoC_loop_i(1,8)

    VoC_lw32_d(REG23,VAD_SCHUR_RECUR_PP_ADDR_ADDR);         //pp[0] and pp[1]

    VoC_bez16_r(SCHUR_RECURSION104,REG2)
    VoC_movreg16(REG4,REG3,DEFAULT);
    VoC_bnltz16_r(SCHUR_RECURSION_ABS_A,REG4)
    VoC_sub16_dr(REG4,CONST_0_ADDR,REG4);                   //REG4:|pp[1]|
SCHUR_RECURSION_ABS_A:
    VoC_bngt16_rr(SCHUR_RECURSION105,REG4,REG2)

SCHUR_RECURSION104:
    VoC_movreg16(REG6,RL6_LO,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_sub16_rr(REG6,REG6,REG1,DEFAULT);
    VoC_loop_r(0,REG6)
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0

    VoC_return

SCHUR_RECURSION105:

    /************************************
    * div_s
    * INPUT:        REG45(L_num),REG23(L_denum)
    * OUTPUT:       REG0
    * REQUIREMENT:  REG0 inc val=1
    * USED:         loop_reg[0]
    ************************************/

    VoC_push16(RA,DEFAULT);
    VoC_push16(REG1,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);
    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_movreg16(REG1,REG2,IN_PARALLEL);
    VoC_jal(CII_DIV_S);
    VoC_movreg16(REG0,REG2,DEFAULT);
    VoC_pop16(REG1,IN_PARALLEL);

    VoC_pop16(RA,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);
    VoC_bngtz16_d(SCHUR_RECURSION106,VAD_SCHUR_RECUR_PP_ADDR_ADDR+1);
    VoC_sub16_dr(REG0,CONST_0_ADDR,REG0);

SCHUR_RECURSION106:
    VoC_movreg16(REG7,RL6_HI,DEFAULT);              //&vpar[7]
    VoC_sw16_p (REG0,REG1, DEFAULT);
    VoC_bne16_rr(SCHUR_RECURSION107,REG1,REG7)
    VoC_return

SCHUR_RECURSION107:
    /*** Schur recursion: ***/

    VoC_lw16i_set_inc(REG0,VAD_SCHUR_RECUR_PP_ADDR_ADDR,1);     //&pp[0]
    VoC_lw16i_set_inc(REG2,VAD_SCHUR_RECUR_PP_ADDR_ADDR+1,1);   //&pp[1]
    VoC_lw16inc_p(REG6,REG1,DEFAULT);                   //REG6:vpar[n];&vpar[n]++

    VoC_multf32inc_rp(REG45,REG6,REG2,DEFAULT);         //REG2:&pp[2] now
    VoC_lw32_d(ACC0,CONST_0x00008000_ADDR);         //ACC0:0x00008000
    VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);

    VoC_add16_rp (REG5, REG5,REG0,DEFAULT);
//VoC_NOP();
    VoC_lw16i_set_inc(REG3,VAD_SCHUR_RECUR_KK_ADDR_ADDR+8,-1);      //REG3:&kk[8]
    VoC_sw16inc_p (REG5,REG0, DEFAULT);                 //REG0:&pp[1] now
    VoC_movreg16(REG7,RL6_LO,IN_PARALLEL);              //&vpar[8]
    VoC_sub16_rr(REG7,REG7,REG1,DEFAULT);               //LOOP NUMBER

    VoC_push16(REG1,DEFAULT);
    VoC_movreg16(REG1,REG6,IN_PARALLEL);                //REG1:vpar[n]

    VoC_bngtz16_r(SHUR0401,REG7)
    VoC_loop_r_short(REG7,DEFAULT)
    VoC_startloop0
    VoC_multf32_rp(REG45,REG1,REG3,DEFAULT);
    VoC_multf32_rp(REG67,REG1,REG2,DEFAULT);
    VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);
    VoC_add32_rr(REG67,REG67,ACC0,DEFAULT);
    VoC_add16inc_rp(REG5,REG5,REG2,DEFAULT);
    VoC_add16_rp(REG7,REG7,REG3,IN_PARALLEL);
    VoC_NOP();
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
//VoC_NOP();
    VoC_sw16inc_p(REG7,REG3,IN_PARALLEL);
    VoC_endloop0
SHUR0401:
    VoC_pop16(REG1,DEFAULT);
    VoC_endloop1
    VoC_return;
}

/****************************************************************************
 *
 *     FUNCTION:  step_up
 *
 *     PURPOSE:   Computes the transversal filter coefficients from the
 *                reflection coefficients.
 *
 *     INPUTS:    np               filter order (2..8)
 *                vpar[0..np-1]    reflection coefficients
 *
 *     OUTPUTS:   aav1[0..np]      transversal filter coefficients
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/

/*PARAMETER PASSING STYLE
reg0:       &vpar
rl6_lo:     &aav1
rl6_hi:     np

  */
void CII_step_up (void)
{


    /*** Initialization of the step-up recursion ***/


//  VoC_lw16z(REG6,);
    VoC_lw16i_short(REG6,0,DEFAULT );
    VoC_lw16i(REG7,0x2000);
//VoC_NOP();
    //&vpar[0]
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_sw32_d(REG67,VAD_STEP_UP_L_COEF_ADDR_ADDR);

    VoC_lw16inc_p(REG6,REG0,DEFAULT);                   //REG6:vpar[0];REG0:&vpar[1]
//  VoC_lw16z(REG7,);
    VoC_lw16i_short(REG7,0,IN_PARALLEL );
    VoC_bnltz16_r(STEP_UP_00,REG6)
    VoC_lw16i_short(REG7,-1,DEFAULT);
STEP_UP_00:

    VoC_shr32_ri(REG67,-14,DEFAULT);
//VoC_NOP();

    /*** Loop on the LPC analysis order: ***/

    VoC_lw16i_set_inc(REG2,VAD_STEP_UP_L_COEF_ADDR_ADDR+2, 2);
    VoC_sw32_d(REG67,VAD_STEP_UP_L_COEF_ADDR_ADDR+2);

    VoC_lw16i_set_inc(REG3,VAD_STEP_UP_L_WORK_ADDR_ADDR+2, 2);
    VoC_movreg32(RL7,REG23,DEFAULT);                    //&L_coef[1] and &L_work[1]
    // reg1 &L_coef[m-i]

    VoC_lw16i_short (REG6,1, DEFAULT);                  // REG6 for m-1
    VoC_movreg16(REG7,RL6_HI,IN_PARALLEL);              //np
    VoC_sub16_rd(REG7,REG7,CONST_1_ADDR);               //loop number

    VoC_loop_r(1,REG7)
    VoC_movreg32(REG23,RL7,DEFAULT);                //&L_coef[1] and &L_work[1]
    VoC_lw16i_set_inc(REG1,VAD_STEP_UP_L_COEF_ADDR_ADDR,-2);
    VoC_add16_rr(REG1,REG1,REG6,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);           //reg4: vpar[m-1]
    VoC_add16_rd(REG1,REG1,CONST_1_ADDR);
    VoC_add16_rr(REG1,REG1,REG6,DEFAULT);      //reg1 addr of hi(L_coef[m-1])

    VoC_loop_r_short(REG6,DEFAULT);            // number of loop m-1
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);      // acc0  L_coef[i]
    VoC_mac32inc_rp(REG4,REG1,DEFAULT);    // acc0  L_work[i]
    VoC_NOP();
    VoC_NOP();
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);      // save acc0 to L_work[i]
    VoC_endloop0

    VoC_movreg32(REG23,RL7,DEFAULT);            //&L_coef[1] and &L_work[1]
    VoC_loop_r_short(REG6,DEFAULT);                // number of loop m-1
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG3,DEFAULT);    // ACC0 L_work[i]
    VoC_NOP();
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);    // save ACC0 to L_coef[i]
    VoC_endloop0

    VoC_lw16i_short (REG5, 0xffff, DEFAULT);
    VoC_bltz16_r(STEP_UP_01,REG4)
    VoC_lw16i_short(REG5,0,DEFAULT);
STEP_UP_01:
    VoC_shr32_ri(REG45,-14,DEFAULT);            // reg45 L_shl (L_deposit_l (vpar[m - 1]), 14)
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);               // m++
    VoC_sw32_p(REG45,REG2,DEFAULT);            // save reg45 to L_coef[m]

    VoC_endloop1


    /*** Keep the aav1[0..np] in 15 bits ***/
    VoC_lw16i(REG2,VAD_STEP_UP_L_COEF_ADDR_ADDR);   //&L_coef[0]
    VoC_movreg16(REG0,RL6_LO,DEFAULT);                          //&aav1[0]
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);       //loop number


    VoC_loop_r_short(REG6,DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p (REG45,REG2, DEFAULT);
    VoC_shr32_ri (REG45, 3, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p (REG5,REG0, DEFAULT);
    VoC_endloop0


    VoC_return;
}

/****************************************************************************
 *
 *     FUNCTION:  compute_rav1
 *
 *     PURPOSE:   Computes the autocorrelation function of the adaptive
 *                filter coefficients.
 *
 *     INPUTS:    aav1[0..8]     adaptive filter coefficients
 *
 *     OUTPUTS:   rav1[0..8]     ACF of aav1
 *                *scal_rav1     rav1[] scaling factor
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/



void CII_compute_rav1 (void)
{


#if 0
    voc_short    VAD_COMP_RAV1_L_WORK_ADDR           ,100,y
    voc_short    VAD_COMP_RAV1_AAV1_ADDR_ADDR           ,20,y

#endif


    /*COPY aav1 to RAM_Y*/
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_set_inc(REG0,VAD_PRED_VAL_AAV1_ADDR,2);
    VoC_lw16i_set_inc(REG1,VAD_COMP_RAV1_AAV1_ADDR_ADDR,2);
    VoC_lw16i_short(REG6,2,DEFAULT);
    VoC_jal(CII_copy_xy);

    VoC_sw32inc_p(ACC0,REG1,DEFAULT);

    VoC_lw16i(REG0,VAD_COMP_RAV1_L_WORK_ADDR);
    VoC_lw16i_set_inc(REG2,VAD_PRED_VAL_AAV1_ADDR,1);
    VoC_lw16i_set_inc(REG3,VAD_COMP_RAV1_AAV1_ADDR_ADDR,1);
    VoC_lw16i_short(REG4,9,DEFAULT);                        //reg4:9-i
    VoC_lw16i_short(REG5,0,IN_PARALLEL );
    VoC_lw16i_short(REG6,-1,DEFAULT);
    VoC_lw16i_short(REG7,0, IN_PARALLEL);
    VoC_movreg32(RL6,REG23,DEFAULT);                        //RL6:&aav1[0] and &aav1'[0]
    VoC_movreg32(RL7,REG67,IN_PARALLEL);                    //RL7:hi:0,lo:-1
    VoC_lw32z(ACC0,DEFAULT);
    VoC_movreg32(REG23,RL6,IN_PARALLEL);    //&aav1[0] and &aav1'[0]
    VoC_loop_i(1,9);


    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);   //&aav1'[i]

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_startloop0
    VoC_mac32inc_pp(REG2,REG3,DEFAULT);
    VoC_endloop0
    VoC_movreg32(REG23,RL6,DEFAULT);    //&aav1[0] and &aav1'[0]
    VoC_add32_rr(REG45,REG45,RL7,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_endloop1

    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_bez32_d(COMPUTE_RAV101,VAD_COMP_RAV1_L_WORK_ADDR);


    VoC_lw32_d(REG45,VAD_COMP_RAV1_L_WORK_ADDR);


    /*****************************************************************************
     * norm_l(REG0,REG23)
     * INPUT:           REG23
     * OUTPUT:          REG0,(REG23 is normlized)
     * REQUIREMENTS:    REG0 inc val=1;PAGE[2]=2;
     * USED:            REG23,loop_reg[0]
     *****************************************************************************/

    VoC_loop_i_short(31,DEFAULT)
    VoC_startloop0
    VoC_bnlt32_rd(RAV1_NORM_L_1_EXIT,REG45,CONST_0x40000000_ADDR)
    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_endloop0

RAV1_NORM_L_1_EXIT:

COMPUTE_RAV101:

    VoC_lw16i(REG0,VAD_COMP_RAV1_L_WORK_ADDR);
    VoC_sw16_d(REG2,VAD_COMPUTATION_SCAL_RAV1_ADDR);


    VoC_lw16i(REG3,VAD_COMPUTATION_RAV1_ADDR);
    VoC_sub16_dr(REG2,CONST_0_ADDR,REG2);

    VoC_lw32inc_p(REG45,REG0,DEFAULT);//REG45:value of L_work[i]
    VoC_shr32_rr(REG45,REG2,DEFAULT);

    VoC_pop16(RA,DEFAULT);
    VoC_loop_i_short(9,DEFAULT);
    VoC_startloop0
    VoC_NOP();
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);//REG45:value of L_work[i]
    VoC_shr32_rr(REG45,REG2,DEFAULT);
    VoC_endloop0


    VoC_return;
}
/****************************************************************************
 *
 *     FUNCTION:  spectral_comparison
 *
 *     PURPOSE:   Computes the stat flag needed for the threshold
 *                adaptation decision.
 *
 *     INPUTS:    rav1[0..8]      ACF obtained from L_av1
 *                *scal_rav1      rav1[] scaling factor
 *                L_av0[0..8]     ACF averaged over last four frames
 *
 *     OUTPUTS:   none
 *
 *     RETURN VALUE: flag to indicate spectral stationarity
 *
 ***************************************************************************/




void CII_spectral_comparison (void)
{


#if 0
    voc_short    SPECTRAL_COMPARISON_SAV0_ADDR           ,9,y

#endif


    /*** Re-normalize L_av0[0..8] ***/
    VoC_push16(RA,DEFAULT);

    VoC_bnez32_d(SPECTRAL_COMPARISON101,VAD_COMPUTATION_L_AV0_ADDR)

    VoC_lw16i_set_inc(REG0,SPECTRAL_COMPARISON_SAV0_ADDR,2);       /*REG0 for sav0[i]*/
    VoC_lw16i(REG5,0x0fff);               /*REG5=0x0fff*/
    VoC_movreg16(REG4,REG5,DEFAULT);

    VoC_loop_i_short(5,DEFAULT);
    VoC_startloop0
    VoC_sw32inc_p (REG45,REG0, DEFAULT);/*sav0[i]=reg5*/
    VoC_endloop0

    VoC_jump(SPECTRAL_COMPARISON102);

SPECTRAL_COMPARISON101:

    VoC_lw32_d (REG23,VAD_COMPUTATION_L_AV0_ADDR);/*reg23=L_av0[0]*/

//   CII_norm_l(REG1,REG23,SPECTRAL_COMPARISON9);/*reg1=norm_l (L_av0[0])*/
    /*****************************************************************************
    * norm_l(REG1,REG23)
    * INPUT:           REG23
    * OUTPUT:          REG1,(REG23 is normlized)
    * REQUIREMENTS:    REG1 inc val=1;PAGE[2]=2;
    * USED:            REG23,loop_reg[0]
    *****************************************************************************/
    VoC_lw16i_set_inc(REG1,0,1);
    VoC_loop_i_short(31,DEFAULT)
    VoC_startloop0
    VoC_bnlt32_rd(SPC_NORM_L_1_EXIT,REG23,CONST_0x40000000_ADDR)
    VoC_shr32_ri(REG23,-1,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);
    VoC_endloop0

SPC_NORM_L_1_EXIT:

    VoC_sub16_dr (REG1,CONST_3_ADDR,REG1);

    VoC_lw16i_set_inc(REG0,VAD_COMPUTATION_L_AV0_ADDR,2);
    VoC_lw16i_set_inc(REG3,SPECTRAL_COMPARISON_SAV0_ADDR,1);

    VoC_lw32inc_p (REG45,REG0, DEFAULT);/*reg45=L_av0[i]*/
    VoC_shr32_rr (REG45, REG1, DEFAULT);/*reg45=L_shl (L_av0[i], shift)*/

    VoC_loop_i_short( 9,DEFAULT);
    VoC_startloop0
    VoC_NOP();
    VoC_sw16inc_p (REG5,REG3, DEFAULT);/*sav0[i]=reg3*/
    VoC_lw32inc_p (REG45,REG0, IN_PARALLEL);/*reg45=L_av0[i]*/
    VoC_shr32_rr (REG45, REG1, DEFAULT);/*reg45=L_shl (L_av0[i], shift)*/
    VoC_endloop0

SPECTRAL_COMPARISON102:


    /*** Compute partial sum of dm ***/

    VoC_lw32z(ACC0,DEFAULT);


    VoC_lw16i_set_inc(REG0,SPECTRAL_COMPARISON_SAV0_ADDR+1,1);/*reg0=sav0[i]*/
    VoC_lw16i_set_inc(REG1,VAD_COMPUTATION_RAV1_ADDR+1,1);/*reg1=rav1[i]*/

    VoC_loop_i_short( 8,DEFAULT);
    VoC_startloop0
    VoC_mac32inc_pp (REG0, REG1, DEFAULT);
    VoC_endloop0


    VoC_NOP();

    /*** Compute the division of the partial sum by sav0[0] ***/
    VoC_movreg32(RL6,ACC0,DEFAULT);             //RL6:L_sump
    VoC_movreg32(REG45,ACC0,IN_PARALLEL);       //REG45:L_temp

    VoC_bnltz32_r(SPECTRAL_COMPARISON104,ACC0);
    VoC_sub32_dr(REG45,CONST_WORD32_0_ADDR,REG45);

SPECTRAL_COMPARISON104:

    VoC_lw32z(REG67,DEFAULT);                   //REG67:L_dm
    VoC_lw16i_short(REG1,0,IN_PARALLEL );
    VoC_bez32_r(SPECTRAL_COMPARISON106,REG45)

    VoC_lw16_d(REG2,SPECTRAL_COMPARISON_SAV0_ADDR);
    VoC_shr16_ri(REG2,-3,DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG2,SPECTRAL_COMPARISON_SAV0_ADDR);


    /*****************************************************************************
     * norm_l(REG1,REG23)
     * INPUT:           REG23
     * OUTPUT:          REG1,(REG23 is normlized)
     * REQUIREMENTS:    REG1 inc val=1;PAGE[2]=2;
     * USED:            REG23,loop_reg[0]
     *****************************************************************************/

    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_loop_i_short(31,DEFAULT)
    VoC_startloop0
    VoC_bnlt32_rd(SPC_NORM_L_2_EXIT,REG45,CONST_0x40000000_ADDR)
    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);
    VoC_endloop0

SPC_NORM_L_2_EXIT:
    ////REG45 is normlized in the norm_l;                   REG1:shift
    VoC_movreg16(REG4,REG5,DEFAULT);
    VoC_bgt16_rr(SPECTRAL_COMPARISON108,REG4,REG2)
    /************************************
    * div_s
    * INPUT:        REG45(L_num),REG23(L_denum)
    * OUTPUT:       REG0
    * REQUIREMENT:  REG0 inc val=1
    * USED:         loop_reg[0]
    ************************************/
    VoC_push16(REG1,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);
    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_movreg16(REG1,REG2,IN_PARALLEL);
    VoC_jal(CII_DIV_S);
    VoC_movreg16(REG0,REG2,DEFAULT);
    //reg0:temp
    VoC_pop16(REG1,IN_PARALLEL);

    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);
    VoC_jump(SPECTRAL_COMPARISON109)

SPECTRAL_COMPARISON108:
    VoC_sub16_rr(REG4,REG4,REG2,DEFAULT);
    /************************************
        * div_s
        * INPUT:        REG45(L_num),REG23(L_denum)
        * OUTPUT:       REG0
        * REQUIREMENT:  REG0 inc val=1
        * USED:         loop_reg[0]
    ************************************/
    VoC_push16(REG1,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);
    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_movreg16(REG1,REG2,IN_PARALLEL);
    VoC_jal(CII_DIV_S);
    VoC_movreg16(REG0,REG2,DEFAULT);
    VoC_pop16(REG1,IN_PARALLEL);
    VoC_lw16i_short(REG2,1,DEFAULT);                //REG2:divshift
    VoC_pop32(RL6,IN_PARALLEL);
SPECTRAL_COMPARISON109:
    VoC_lw32z(REG67,DEFAULT);                           //L_dm=0
    VoC_bne16_rd(SPECTRAL_COMPARISON110,REG2,CONST_1_ADDR)
    VoC_lw16i(REG6,0x8000);

SPECTRAL_COMPARISON110:
    VoC_movreg16(REG4,REG0,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL );
    VoC_bnltz16_r(SPECTRAL_COMPARISON111,REG4)
    VoC_lw16i_short(REG5,-1,DEFAULT);

SPECTRAL_COMPARISON111:

    VoC_add32_rr (REG67,REG67,REG45, DEFAULT);
    VoC_shr32_ri (REG67, -1, DEFAULT);


    VoC_bnltz32_r(SPECTRAL_COMPARISON112,RL6);/*if (reg23 < 0L)*/
    VoC_sub32_dr(REG67,CONST_WORD32_0_ADDR,REG67);

SPECTRAL_COMPARISON112:

SPECTRAL_COMPARISON106:


    /*** Re-normalization and final computation of L_dm ***/

    VoC_shr32_ri (REG67, -14, DEFAULT);

    VoC_shr32_rr (REG67, REG1, DEFAULT);/* reg23 = L_shr (reg23, reg4);*/
//  VoC_lw16z(REG5,);
    VoC_lw16i_short(REG5,0,IN_PARALLEL );
    VoC_lw16_d(REG4,VAD_COMPUTATION_RAV1_ADDR);
    VoC_bnltz16_r(SPC2_DPL_EXIT,REG4)
    VoC_lw16i_short(REG5,0xffff,DEFAULT);
SPC2_DPL_EXIT:

    VoC_shr32_ri (REG45, -11, DEFAULT);/*reg45=L_shl (reg45, 11)*/
    VoC_add32_rr (REG67, REG67, REG45, DEFAULT);/*reg23=L_add(reg23,reg45)*/

    VoC_lw16_d(REG4,VAD_COMPUTATION_SCAL_RAV1_ADDR);/*reg4=scal_rav1*/
    VoC_shr32_rr (REG67, REG4, DEFAULT);/* reg23 = L_shr (reg23, reg4);*/



    /*** Compute the difference and save L_dm ***/


    VoC_sub32_rd (REG45,REG67,STATIC_L_LASTDM_ADDR);


    VoC_bnltz32_r(SPECTRAL_COMPARISON113,REG45)
    VoC_sub32_dr(REG45,CONST_WORD32_0_ADDR,REG45);
SPECTRAL_COMPARISON113:

    VoC_sw32_d(REG67, STATIC_L_LASTDM_ADDR);

    /*** Evaluation of the stat flag ***/

    VoC_lw16i (REG2,STAT_THRESH);
    VoC_lw16i_short (REG3,0,DEFAULT);           /*reg23=STAT_THRESH*/
    VoC_lw16i_short(REG6,1,IN_PARALLEL);        //REG6:sta

    VoC_pop16(RA,DEFAULT);

    VoC_bgt32_rr(SPECTRAL_COMPARISON115,REG23,REG45);/*if (reg45 < 0L)*/
    VoC_lw16i_short(REG6,0,DEFAULT);

SPECTRAL_COMPARISON115:


    VoC_return;

}


/****************************************************************************
 *
 *     FUNCTION:  threshold_adaptation
 *
 *     PURPOSE:   Evaluates the secondary VAD decision.  If speech is not
 *                present then the noise model rvad and adaptive threshold
 *                thvad are updated.
 *
 *     INPUTS:    stat          flag to indicate spectral stationarity
 *                ptch          flag to indicate a periodic signal component
 *                tone          flag to indicate a tone signal component
 *                rav1[0..8]    ACF obtained from L_av1
 *                scal_rav1     rav1[] scaling factor
 *                pvad          filtered signal energy (mantissa+exponent)
 *                acf0          signal frame energy (mantissa+exponent)
 *
 *     OUTPUTS:   rvad[0..8]    autocorrelated adaptive filter coefficients
 *                *scal_rvad    rvad[] scaling factor
 *                *thvad        decision threshold (mantissa+exponent)
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/


void CII_threshold_adaptation (void)
{
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC1,1,DEFAULT);


    VoC_lw16i_short (REG2,0,DEFAULT);                       //reg2:comp

    /*** Test if acf0 < pth; if yes set thvad to plev ***/


    VoC_lw16i_short (REG7,E_PTH,IN_PARALLEL);
    VoC_lw16i(REG6,M_PTH);

    VoC_bngt32_rd(THRESHOLD_ADAPTATION103,REG67,VAD_COMPUTATION_ACF0_ADDR);
    VoC_lw16i_short(REG6,E_PLEV,DEFAULT);
    VoC_lw16i(REG7,M_PLEV);
    VoC_NOP();
    VoC_sw32_d (REG67,STATIC_THVAD_ADDR);

    VoC_return


THRESHOLD_ADAPTATION103:



    /*** Test if an adaption is required ***/
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_bne16_rd(THRESHOLD_ADAPTATION104,REG6,GLOBAL_PTCH_ADDR);
    VoC_lw16i_short (REG2,1,DEFAULT);

THRESHOLD_ADAPTATION104:
    VoC_bnez16_d(THRESHOLD_ADAPTATION105,VAD_COMPUTATION_STAT_ADDR);
    VoC_lw16i_short (REG2,1,DEFAULT);

THRESHOLD_ADAPTATION105:
    VoC_bne16_rd(THRESHOLD_ADAPTATION106,REG6,VAD_COMPUTATION_TONE_ADDR);
    VoC_lw16i_short (REG2,1,DEFAULT);

THRESHOLD_ADAPTATION106:



    VoC_bne16_rd(THRESHOLD_ADAPTATION107,REG2,CONST_1_ADDR);
//      VoC_lw16z(REG7,DEFAULT);
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_NOP();
    VoC_sw16_d (REG7,STATIC_ADAPTCOUNT_ADDR);
    VoC_return

THRESHOLD_ADAPTATION107:

    /*** Increment adaptcount ***/

    VoC_add16_rd (REG7,REG6,STATIC_ADAPTCOUNT_ADDR);
    VoC_NOP();
    VoC_sw16_d (REG7,STATIC_ADAPTCOUNT_ADDR);
    VoC_bgt16_rd(THRESHOLD_ADAPTATION108,REG7,CONST_8_ADDR);
    VoC_return

THRESHOLD_ADAPTATION108:

    /*** computation of thvad-(thvad/dec) ***/
    VoC_lw16i(REG0,STATIC_THVAD_ADDR);
    VoC_NOP();
    VoC_lw16inc_p(REG5,REG0,DEFAULT);                           //REG5:thvad.e
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);                       //REG4:thvad.m

    VoC_movreg16(REG6,REG4,DEFAULT);
    VoC_shr16_ri (REG6,5,DEFAULT);
    VoC_sub16_rr (REG4,REG4,REG6,DEFAULT);

    VoC_bnlt16_rd(THRESHOLD_ADAPTATION109,REG4,CONST_0x4000_ADDR)
    VoC_shr16_ri(REG4,-1,DEFAULT);
    VoC_sub16_rd (REG5, REG5,CONST_1_ADDR);

THRESHOLD_ADAPTATION109:
    /*** computation of pvad*fac ***/

    /*---------------------------------------------------------
    |   According to the program,REG6:p_temp.m,REG7:p_temp.e    |
    -----------------------------------------------------------*/
    VoC_lw16i (REG1,FAC);
    VoC_multf32_rd (REG67, REG1, VAD_COMPUTATION_PVAD_ADDR+1);
    VoC_lw16i_short (REG0,1,DEFAULT);
    VoC_shr32_ri (REG67,15,DEFAULT);
    VoC_add16_rd (REG1,REG0,VAD_COMPUTATION_PVAD_ADDR);

    VoC_bngt32_rd(THRESHOLD_ADAPTATION110,REG67,CONST_0x00007FFF_ADDR)
    VoC_shr32_ri (REG67, 1, DEFAULT);
    VoC_add16_rr (REG1, REG1,REG0,IN_PARALLEL);

THRESHOLD_ADAPTATION110:
    VoC_movreg16(REG7,REG1,DEFAULT);                //REG67:p_temp.m and p_temp.e

    /*** test if thvad < pvad*fac ***/
    VoC_bngt16_rr(THRESHOLD_ADAPTATION111,REG7,REG5);
    VoC_lw16i_short (REG2,1,DEFAULT);

THRESHOLD_ADAPTATION111:
    VoC_bne16_rr(THRESHOLD_ADAPTATION112,REG5,REG7);
    VoC_bngt16_rr(THRESHOLD_ADAPTATION112,REG6,REG4);
    VoC_lw16i_short (REG2,1,DEFAULT);

THRESHOLD_ADAPTATION112:
    /*** compute minimum(thvad+(thvad/inc), pvad*fac) when comp = 1 ***/

    VoC_bne16_rd(THRESHOLD_ADAPTATION117,REG2,CONST_1_ADDR);

    /*** compute thvad + (thvad/inc) ***/
    VoC_movreg16(REG0,REG5,DEFAULT);        //REG0:thvad.e temprarily
//      VoC_lw16z(REG5,);           //REG45:L_temp
    VoC_lw16i_short(REG5,0,IN_PARALLEL );
    VoC_movreg16(REG2,REG4,DEFAULT);
//      VoC_lw16z(REG3,);
    VoC_lw16i_short(REG3,0,IN_PARALLEL );
    VoC_shr16_ri(REG2,4,DEFAULT);
    VoC_add32_rr(REG45,REG45,REG23,DEFAULT);

    VoC_bngt32_rd(THRESHOLD_ADAPTATION113,REG45,CONST_0x00007FFF_ADDR)
    VoC_shr32_ri (REG45,1,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);

THRESHOLD_ADAPTATION113:
    VoC_movreg16(REG5,REG0,DEFAULT);        //REG5:thvad.e
//      VoC_lw16z(REG2,);
    VoC_lw16i_short(REG2,0,IN_PARALLEL );

    VoC_bngt16_rr(THRESHOLD_ADAPTATION115,REG5,REG7);
    VoC_lw16i_short (REG2,1,DEFAULT);

THRESHOLD_ADAPTATION115:
    VoC_bne16_rr(THRESHOLD_ADAPTATION116,REG7,REG5);
    VoC_bngt16_rr(THRESHOLD_ADAPTATION116,REG4,REG6) ;
    VoC_lw16i_short (REG2,1,DEFAULT);

THRESHOLD_ADAPTATION116:

    VoC_bne16_rd(THRESHOLD_ADAPTATION117,REG2,CONST_1_ADDR);
    VoC_movreg32(REG45,REG67,DEFAULT);

THRESHOLD_ADAPTATION117:
    /*** compute pvad + margin ***/
    /*-----------------------------------------------------
        |   PROTECT thvad and REG45 for pvad    |           */
    VoC_NOP();
    VoC_push32(REG45,DEFAULT);
    /*-------------------------------------------------------*/
    VoC_lw16_d(REG4,VAD_COMPUTATION_PVAD_ADDR+1);       //REG4:pvad.m
    VoC_lw16_d(REG5,VAD_COMPUTATION_PVAD_ADDR);     //REG5:pvad.e

    VoC_lw16i_short (REG3,E_MARGIN,DEFAULT);
    VoC_lw16i(REG2,M_MARGIN);
    VoC_bne16_rr(THRESHOLD_ADAPTATION123,REG3,REG5);
    VoC_movreg16(REG6,REG4,DEFAULT);
//      VoC_lw16z(REG7,);
    VoC_lw16i_short(REG7,0,IN_PARALLEL );
//      VoC_lw16z(REG3,DEFAULT);
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_add32_rr(REG67,REG67,REG23,DEFAULT);
    VoC_shr32_ri(REG67,1,DEFAULT);                  //REG6:p_temp.m
    VoC_add16_rd(REG7,REG5,CONST_1_ADDR);           //REG7:p_temp.e
    VoC_jump(THRESHOLD_ADAPTATION124)

THRESHOLD_ADAPTATION123:
    VoC_bngt16_rr(THRESHOLD_ADAPTATION120,REG5,REG3)
    VoC_sub16_rr(REG0,REG5,REG3,DEFAULT);
    VoC_movreg16(REG6,REG4,IN_PARALLEL);
    VoC_shr16_rr(REG2,REG0,DEFAULT);
//          VoC_lw16z(REG3,);
    VoC_lw16i_short(REG3,0,IN_PARALLEL );
//          VoC_lw16z(REG7,DEFAULT);
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_add32_rr(REG67,REG67,REG23,DEFAULT);
    VoC_movreg16(REG0,REG5,IN_PARALLEL);        //REG0:p_temp.e
    VoC_bngt32_rd(THRESHOLD_ADAPTATION121,REG67,CONST_0x00007FFF_ADDR)
    VoC_shr32_ri(REG67,1,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);

THRESHOLD_ADAPTATION121:
    VoC_movreg16(REG7,REG0,DEFAULT);            //REG67:p_temp
    VoC_jump(THRESHOLD_ADAPTATION124)

THRESHOLD_ADAPTATION120:
    VoC_sub16_rr(REG0,REG3,REG5,DEFAULT);
    VoC_movreg16(REG6,REG4,IN_PARALLEL);
    VoC_shr16_rr(REG6,REG0,DEFAULT);
    VoC_movreg16(REG0,REG3,IN_PARALLEL);        //REG0:E_MARGIN
//          VoC_lw16z(REG3,DEFAULT);
    VoC_lw16i_short(REG3,0,DEFAULT);
//          VoC_lw16z(REG7,);
    VoC_lw16i_short(REG7,0,IN_PARALLEL );
    VoC_add32_rr(REG67,REG67,REG23,DEFAULT);
    VoC_bngt32_rd(THRESHOLD_ADAPTATION122,REG67,CONST_0x00007FFF_ADDR)
    VoC_shr32_ri(REG67,1,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);

THRESHOLD_ADAPTATION122:
    VoC_movreg16(REG7,REG0,DEFAULT);            //REG67:p_temp

THRESHOLD_ADAPTATION124:
    /*----------------------------------------------------------------------
    |   //RESTORE thvad and REG45 for thvad |                               */
    VoC_pop32(REG45,DEFAULT);
    /*------------------------------------------------------------------------*/

    /*** Test if thvad > pvad + margin ***/

    VoC_lw16i_short (REG2,0,DEFAULT);
    VoC_bngt16_rr(THRESHOLD_ADAPTATION125,REG5,REG7);
    VoC_lw16i_short (REG2,1,DEFAULT);

THRESHOLD_ADAPTATION125:
    VoC_bne16_rr(THRESHOLD_ADAPTATION126,REG7,REG5);
    VoC_bngt16_rr(THRESHOLD_ADAPTATION126,REG4,REG6);
    VoC_lw16i_short (REG2,1,DEFAULT);

THRESHOLD_ADAPTATION126:

    VoC_bne16_rd(THRESHOLD_ADAPTATION127,REG2,CONST_1_ADDR)
    VoC_movreg32(REG45,REG67,DEFAULT);

THRESHOLD_ADAPTATION127:

    /*here thvad can be saved*/
    /*** Normalise and retain rvad[0..8] in memory ***/
    VoC_lw16i(REG0,STATIC_THVAD_ADDR);
    VoC_lw16_d (REG1,VAD_COMPUTATION_SCAL_RAV1_ADDR);
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    VoC_sw16_p(REG4,REG0,DEFAULT);

    VoC_lw16i (REG0,STATIC_RVAD_ADDR);
    VoC_sw16_d (REG1,STATIC_SCAL_RVAD_ADDR);

    VoC_lw16i (REG1,VAD_COMPUTATION_RAV1_ADDR);

    /*** Set adaptcount to adp + 1 ***/
    VoC_lw16i_short(REG3,9,DEFAULT);

    VoC_loop_i_short(9,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p (REG2,REG1, DEFAULT);
    VoC_sw16_d(REG3,STATIC_ADAPTCOUNT_ADDR);
    VoC_sw16inc_p (REG2,REG0, DEFAULT);
    VoC_endloop0

    VoC_return;

}

/****************************************************************************
 *
 *     FUNCTION:  tone_detection
 *
 *     PURPOSE:   Computes the tone flag needed for the threshold
 *                adaptation decision.
 *
 *     INPUTS:    rc[0..3]    reflection coefficients calculated in the
 *                            speech encoder short term predictor
 *
 *     OUTPUTS:   *tone       flag to indicate a periodic signal component
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/

void CII_tone_detection (void)
{

    VoC_lw16i_short(REG6,0,DEFAULT);

    /*** Calculate filter coefficients ***/

    VoC_lw16i (REG0,LPC_RC_ADDR);/*rc's address*/
    VoC_sw16_d(REG6,VAD_COMPUTATION_TONE_ADDR);


    VoC_lw16i (REG2,VAD_COMPUTATION_A_ADDR);/*array a address*/
    VoC_lw16i_short(REG3,2,DEFAULT);
    VoC_movreg32(RL6,REG23,DEFAULT);

    VoC_push16(RA,IN_PARALLEL);
    VoC_jal (CII_step_up);
    VoC_pop16 (RA,DEFAULT);

    /*** Calculate ( a[1] * a[1] ) ***/

    VoC_lw16_d (REG6,VAD_COMPUTATION_A_ADDR+1);
    VoC_shr16_ri (REG6, -3, DEFAULT);
    VoC_multf32_rr (REG45, REG6, REG6, DEFAULT);        //reg45:L_dn


    /*** Calculate ( 4*a[2] - a[1]*a[1] ) ***/
    VoC_lw16_d (REG7, VAD_COMPUTATION_A_ADDR+2);
    VoC_lw16i_short(REG6,0,DEFAULT);

    VoC_shr32_ri (REG67, -3,DEFAULT );
    VoC_sub32_rr (REG67, REG67, REG45, DEFAULT);        //reg67:L_num

    /*** Check if pole frequency is less than 385 Hz ***/
    VoC_bgtz32_r(TONE_DETECTION102,REG67);
    VoC_return

TONE_DETECTION102:
    VoC_lw16i (REG0,FREQTH);
    VoC_multf32_rr (REG45, REG5, REG0, DEFAULT);
    VoC_bnltz16_d(TONE_DETECTION101,VAD_COMPUTATION_A_ADDR+1)
    VoC_bngt32_rr(TONE_DETECTION101,REG45,REG67);/*if (L_temp < 0){return;}*/
    VoC_return

TONE_DETECTION101:

    /*** Calculate normalised prediction error ***/
    VoC_lw16i (REG0,0x7fff);    /* prederr = 0x7fff;*/
    VoC_lw16i_set_inc(REG1,LPC_RC_ADDR,1);
    VoC_movreg16 (REG6,REG0,DEFAULT);


    VoC_multf16inc_pp (REG4, REG1, REG1, DEFAULT);
    VoC_loop_i_short( 4,DEFAULT );
    VoC_startloop0
    VoC_sub16_rr (REG4, REG6, REG4, DEFAULT);
    VoC_multf16inc_pp (REG4, REG1, REG1, DEFAULT);
    exit_on_warnings =OFF;
    VoC_multf16_rr (REG0, REG0, REG4, DEFAULT);
    exit_on_warnings =ON;
    VoC_endloop0



    /*** Test if prediction error is smaller than threshold ***/
    VoC_lw16i (REG6,PREDTH);
    VoC_lw16i_short (REG1,1,DEFAULT);

    VoC_bngt16_rr(TONE_DETECTION_RET,REG6,REG0); /*if (temp < 0){}*/
    VoC_sw16_d(REG1,VAD_COMPUTATION_TONE_ADDR);


TONE_DETECTION_RET:;

    VoC_return;

}

/****************************************************************************
 *
 *     FUNCTION:  vad_decision
 *
 *     PURPOSE:   Computes the VAD decision based on the comparison of the
 *                floating point representations of pvad and thvad.
 *
 *     INPUTS:    pvad          filtered signal energy (mantissa+exponent)
 *                thvad         decision threshold (mantissa+exponent)
 *
 *     OUTPUTS:   none
 *
 *     RETURN VALUE: vad decision before hangover is added
 *
 ***************************************************************************/

/*OUTPUT:REG2*/
void CII_vad_decision (void)
{

    VoC_lw32_d(REG67,STATIC_THVAD_ADDR);//REG67:thvad
    VoC_lw16i_short(REG2,1,DEFAULT);    //REG2:vvad


    VoC_bnlt16_rd(VAD_DECISION102,REG6,VAD_COMPUTATION_PVAD_ADDR)
    VoC_return

VAD_DECISION102:
    VoC_bne16_rd(VAD_DECISION101,REG6,VAD_COMPUTATION_PVAD_ADDR)
    VoC_bnlt16_rd(VAD_DECISION101,REG7,VAD_COMPUTATION_PVAD_ADDR+1)
    VoC_return

VAD_DECISION101:

    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_return;
}
/****************************************************************************
 *
 *     FUNCTION:  vad_hangover
 *
 *     PURPOSE:   Computes the final VAD decision for the current frame
 *                being processed.
 *
 *     INPUTS:    vvad           vad decision before hangover is added
 *
 *     OUTPUTS:   none
 *
 *     RETURN VALUE: vad decision after hangover is added
 *
 ***************************************************************************/

//short my_hangcount[200];
//short h_c;
/*INPUT:REG2*/

void CII_vad_hangover (void)
{

    VoC_lw16_d(REG6,STATIC_BURSTCOUNT_ADDR);    //REG6:burstcount
    VoC_lw16_d(REG7,STATIC_HANGCOUNT_ADDR); //reg7:hangcount
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);
    VoC_be16_rd(VAD_HANDOVER101,REG2,CONST_1_ADDR);
    VoC_lw16i_short(REG6,0,DEFAULT);

VAD_HANDOVER101:
    VoC_blt16_rd(VAD_HANDOVER103,REG6,CONST_3_ADDR)
    VoC_lw16i_short(REG7,HANGCONST,DEFAULT);
    VoC_lw16i_short(REG6,BURSTCONST,DEFAULT);


VAD_HANDOVER103:
    VoC_sw16_d(REG7,STATIC_HANGCOUNT_ADDR);
    VoC_sw16_d(REG6,STATIC_BURSTCOUNT_ADDR);    //save burstcount

    VoC_bnltz16_r(VAD_HANDOVER104,REG7);
    VoC_sw16_d(REG2,VAD_COMPUTATION_VAD_ADDR);
    VoC_return

VAD_HANDOVER104:
    VoC_sub16_rd (REG7,REG7,CONST_1_ADDR);
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_sw16_d(REG7,STATIC_HANGCOUNT_ADDR);
    VoC_sw16_d(REG3,VAD_COMPUTATION_VAD_ADDR);

    VoC_return;

}

/****************************************************************************
 *
 *     FUNCTION:  periodicity_update
 *
 *     PURPOSE:   Computes the ptch flag needed for the threshold
 *                adaptation decision for the next frame.
 *
 *     INPUTS:    lags[0..1]       speech encoder long term predictor lags
 *
 *     OUTPUTS:   *ptch            Boolean voiced / unvoiced decision
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/

void CII_periodicity_update(void)
{

    VoC_lw16i_set_inc(REG0,COD_AMR_T_OP_ADDRESS,1);     /* REG0 is used for lags*/

    VoC_lw16i_short(REG6,0,DEFAULT);                            /*REG6 = lagcount*/
    VoC_lw16_d(REG7,STRUCT_VADSTATE1_OLDLAG_ADDR);              /* REG7 = oldlag */

    VoC_loop_i_short(2,DEFAULT)
    VoC_startloop0

    /*** Search the maximum and minimum of consecutive lags ***/
    VoC_lw16_p(REG2,REG0,DEFAULT);
    VoC_bngt16_rr(PERIODICITY00,REG7, REG2);
    VoC_lw16_p(REG2,REG0,DEFAULT);                  /* REG2 = minlag */
    VoC_movreg16(REG3,REG7,IN_PARALLEL);            /* REG3 = maxlag*/
    VoC_jump(PERIODICITY01);

PERIODICITY00:
    VoC_movreg16(REG2,REG7,DEFAULT);                /* REG2 = minlag */
    VoC_lw16_p(REG3,REG0,IN_PARALLEL);              /* REG3 = maxlag*/

PERIODICITY01:
    /*** Save the current LTP lag ***/
    VoC_lw16inc_p( REG7,REG0,DEFAULT );             //REG7 for old_lag

    VoC_sub16_rr(REG3,REG3,REG2,IN_PARALLEL);
    VoC_bnlt16_rd(PERIODICITY02,REG3,CONST_2_ADDR)
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR); /* REG6 = lagcount */
PERIODICITY02:;
    VoC_NOP();
    VoC_endloop0

    /*** Update the veryoldlagcount and oldlagcount ***/
    VoC_sw16_d( REG7,STRUCT_VADSTATE1_OLDLAG_ADDR);

    VoC_lw16_d(REG2,STRUCT_VADSTATE1_OLDLAG_COUNT_ADDR);
    VoC_sw16_d(REG6,STRUCT_VADSTATE1_OLDLAG_COUNT_ADDR);

    /*** Make ptch decision ready for next frame ***/
    VoC_add16_rr(REG3,REG6,REG2,DEFAULT);
    VoC_sub16_rd(REG3,REG3,CONST_4_ADDR);
    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_bnltz16_r(PERIODICITY04,REG3);
    VoC_lw16i_short(REG5,0,DEFAULT);
PERIODICITY04:
    VoC_sw16_d(REG2,STATIC_VERYOLDLAGCOUNT_ADDR);
    VoC_sw16_d(REG5,GLOBAL_PTCH_ADDR);


    VoC_return;
}


void CII_vad_computation (void)
{
//vad_count++;


#if 0

    voc_short VAD_COMPUTATION_ACF0_ADDR                 ,2,x        //[2]shorts
    voc_short VAD_COMPUTATION_PVAD_ADDR                 ,2,x        //[2]shorts
    voc_short VAD_COMPUTATION_L_AV0_ADDR                ,18,x       //[18]shorts
    voc_short VAD_COMPUTATION_L_AV1_ADDR                ,18,x        //[18]shorts
    voc_short VAD_COMPUTATION_RAV1_ADDR                 ,9,x         //[9]shorts
    voc_short VAD_COMPUTATION_SCAL_RAV1_ADDR            ,x       //1 short
    voc_short VAD_COMPUTATION_STAT_ADDR                 ,x       //1 short
    voc_short VAD_COMPUTATION_TONE_ADDR                 ,x       //1 short
    voc_short VAD_COMPUTATION_VVAD_ADDR                 ,x       //1 short
    voc_short VAD_COMPUTATION_VAD_ADDR                  ,7,x         //[7]shorts
    voc_short VAD_PRED_VAL_VPAR_ADDR                    ,10,x        //[10]shorts
    voc_short VAD_PRED_VAL_AAV1_ADDR                    ,20,x        //[20]shorts
    voc_short VAD_COMPUTATION_A_ADDR                    ,10,x        //[10]shorts
    voc_short VAD_STEP_UP_L_COEF_ADDR_ADDR              ,9,x         //[9]shorts
    voc_short VAD_COMPUTATION_A_ADDR_END                ,x       //1 short
    voc_short VAD_SCHUR_RECUR_PP_ADDR_ADDR              ,10,x        //[10]shorts
    voc_short VAD_STEP_UP_L_WORK_ADDR_ADDR              ,80,x        //[80]shorts

#endif



    VoC_push16(RA,DEFAULT);
    VoC_jal (CII_energy_computation);

    VoC_sw32_d(REG45,VAD_COMPUTATION_PVAD_ADDR);

    VoC_jal (CII_acf_averaging);
    VoC_jal (CII_predictor_values);
    VoC_jal (CII_spectral_comparison);

    VoC_sw16_d(REG6,VAD_COMPUTATION_STAT_ADDR);

    VoC_jal (CII_tone_detection);

    VoC_jal (CII_threshold_adaptation);
    VoC_jal (CII_vad_decision);
    /*REG2 FOR RETURN VALUE*/
    VoC_sw16_d(REG2,VAD_COMPUTATION_VVAD_ADDR);

    VoC_jal (CII_vad_hangover);


    VoC_lw16_d (REG2,VAD_COMPUTATION_VAD_ADDR);
    VoC_pop16 (RA,DEFAULT);

    VoC_sw16_d (REG2,GLOBAL_OUTPUT_Vad_ADDR);

    VoC_return;

}
void CII_tx_dtx(void)
{
    //REG4:VAD_flag;REG5:txdtx_ctrl;REG6:txdtx_N_elapsed;REG7:txdtx_hangover;

    VoC_lw16_d(REG4,GLOBAL_OUTPUT_Vad_ADDR);
    VoC_lw16_d(REG5,GLOBAL_TXDTX_CTRL_ADDR);

    VoC_lw16_d(REG6,STATIC_TXDTX_N_ELAPSED_ADDR);
    VoC_lw16_d(REG7,STATIC_TXDTX_HANGOVER_ADDR);

    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);

    VoC_bne16_rd(TX_DTX_01_ELSE,REG4,CONST_1_ADDR)
    VoC_lw16i_short(REG7,DTX_HANGOVER,DEFAULT);
    VoC_lw16i_short(REG5,0x0003,DEFAULT);//TX_SP_FLAG|TX_VAD_FLAG
    VoC_jump(TX_DTX_01_END)

TX_DTX_01_ELSE:
    VoC_bnez16_r(TX_DTX_01B_01_ELSE,REG7)
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_movreg16(REG3,REG5,DEFAULT);
    VoC_and16_ri(REG3,TX_HANGOVER_ACTIVE);
    VoC_bez16_r(TX_DTX_01B_01A_01_ELSE,REG3)

    VoC_lw32_d(ACC0,CONST_0x70816958_ADDR);
    VoC_lw16i_short(REG5,0x0018,DEFAULT);//TX_PREV_HANGOVER_ACTIVE|TX_SID_UPDATE
    VoC_sw32_d(ACC0,GLOBAL_L_PN_SEED_TX_ADDR);
    VoC_jump(TX_DTX_01_END)

TX_DTX_01B_01A_01_ELSE:
    VoC_lw16i_short(REG5,TX_SID_UPDATE,DEFAULT);
    VoC_jump(TX_DTX_01_END)

TX_DTX_01B_01_ELSE:
    VoC_sub16_rd(REG7,REG7,CONST_1_ADDR);
    VoC_add16_rr(REG3,REG6,REG7,DEFAULT);
    VoC_lw16i_short(REG4,DTX_ELAPSED_THRESHOLD,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_bnltz16_r(TX_DTX_01B_01B_01_ELSE,REG3)
    VoC_lw16i(REG5,TX_USE_OLD_SID);
    VoC_jump(TX_DTX_01_END)

TX_DTX_01B_01B_01_ELSE:
    VoC_movreg16(REG3,REG5,DEFAULT);
    VoC_and16_ri(REG3,TX_HANGOVER_ACTIVE);
    VoC_bez16_r(TX_DTX_01B_01B_01B_01_ELSE,REG3)
    VoC_lw16i_short(REG5,0x000d,DEFAULT);//TX_PREV_HANGOVER_ACTIVE|TX_HANGOVER_ACTIVE|TX_SP_FLAG
    VoC_jump(TX_DTX_01_END)

TX_DTX_01B_01B_01B_01_ELSE:
    VoC_lw16i_short(REG5,0x0005,DEFAULT);//TX_HANGOVER_ACTIVE|TX_SP_FLAG
TX_DTX_01_END:


    VoC_sw16_d(REG6,STATIC_TXDTX_N_ELAPSED_ADDR);

    VoC_sw16_d(REG7,STATIC_TXDTX_HANGOVER_ADDR);

    VoC_return
}
void CII_aver_lsf_history(void)
{
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_movreg16(REG4,REG2,DEFAULT);
    VoC_lw16i(REG5,0x0fff);


    VoC_loop_i(1,M)
    VoC_multf32inc_pd(ACC0,REG0,CONST_0x3fff_ADDR);
    VoC_multf32inc_pd(REG67,REG1,CONST_0x3fff_ADDR);
    VoC_NOP();
    VoC_add32_rr(REG67,REG67,ACC0,DEFAULT);
    VoC_multf32_rr(ACC0,REG7,REG5,DEFAULT);
    VoC_loop_i_short(DTX_HANGOVER,DEFAULT)
    VoC_startloop0
    VoC_mac32inc_rp(REG5, REG2,DEFAULT);
    VoC_endloop0

    VoC_add16_rd(REG4,REG4,CONST_1_ADDR);
    VoC_movreg16(REG7,ACC0_HI,DEFAULT);
    VoC_movreg16(REG2,REG4,DEFAULT);
    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    VoC_endloop1

    VoC_return
}

void CII_update_lsf_history(void)
{
    /* shift LSF data to make room for LSFs from current frame */


    VoC_push16(REG0,DEFAULT);
    VoC_lw16i_short(REG6,60,IN_PARALLEL);   //(10*(DTX_HANGOVER-1))
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);   //&lsf_old[DTX_HANGOVER-1][0]
    VoC_movreg16(REG1,REG0,DEFAULT);

    VoC_lw16i_short(INC0,-10,DEFAULT);
    VoC_lw16i_short(INC1,-10,IN_PARALLEL);

    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_inc_p(REG1,DEFAULT);                //&lsf_old[DTX_HANGOVER-2][0]

    VoC_loop_i(1,(DTX_HANGOVER-1))
    VoC_movreg32(REG23,REG01,DEFAULT);
    VoC_loop_i_short((M/2),DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(REG67,REG3,DEFAULT);
    VoC_NOP();
    VoC_sw32inc_p(REG67,REG2,DEFAULT);
    VoC_endloop0
    VoC_inc_p(REG0,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);
    VoC_endloop1

    /* Store new LSF data to lsf_old buffer */

    VoC_movreg32(REG23,RL6,DEFAULT);
    VoC_pop16(REG0,IN_PARALLEL);

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_loop_i_short((M/2),DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_lw32inc_p(REG67,REG3,DEFAULT);
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_shr16_ri(REG5,1,IN_PARALLEL);
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_shr16_ri(REG7,1,IN_PARALLEL);
    VoC_add16_rr(REG6,REG4,REG6,DEFAULT);
    VoC_add16_rr(REG7,REG5,REG7,IN_PARALLEL);
    VoC_NOP();
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_endloop0

    VoC_return

}
void CII_update_lsf_p_CN(void)
{
    /*0:&lsf_old[0];1:&lsf_p_CN*/
    VoC_movreg16(REG3,REG0,DEFAULT);            //&lsf_old[0][j];incr is 1
    VoC_lw16i(REG4,INV_DTX_HANGOVER);           //for INV_DTX_HANGOVER
    VoC_lw16i_short(INC2,10,DEFAULT);       //for lsf_old[2*i+1][j]
    VoC_lw16i_short(INC3,1,IN_PARALLEL);

    VoC_loop_i(1,M)
    VoC_movreg16(REG0,REG3,DEFAULT);        //&lsf_old[0][2*j]
    VoC_inc_p(REG3,DEFAULT);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_loop_i_short(DTX_HANGOVER,DEFAULT)
    VoC_startloop0
    VoC_mac32inc_rp(REG4,REG0,DEFAULT);
    VoC_endloop0
    VoC_NOP();
    VoC_add32_rd(REG67,ACC0,CONST_0x00008000_ADDR);
    VoC_NOP();
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop1


    VoC_return
}

/*************************************************************************
 *
 *   FUNCTION NAME: update_gcode0_CN
 *
 *   PURPOSE: Update the reference fixed codebook gain parameter value.
 *            The reference value is computed by averaging the quantized
 *            fixed codebook gain parameter values which exist in the
 *            fixed codebook gain parameter history.
 *
 *   INPUTS:      gain_code_old[0..4*DTX_HANGOVER-1]
 *                              fixed codebook gain parameter history
 *
 *   OUTPUTS:     none
 *
 *   RETURN VALUE: Computed reference fixed codebook gain
 *
 *************************************************************************/

//input gain_code_old addr : REG0 INC 2
//return REG5

void CII_update_gcode0_CN (void)  //return REG5
{
    VoC_lw16i(REG6,0x1fff);
    VoC_movreg16(REG7,REG6,DEFAULT);            //REG67:0x1fff,1fff
    VoC_lw16i(REG1,INV_DTX_HANGOVER);           //reg1:INV_DTX_HANGOVER
    VoC_lw32z(REG45,DEFAULT);                   //REG45:L_ret
    VoC_loop_i(1,7);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_loop_i_short(2,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_rp(REG67,REG0);
    VoC_endloop0
    VoC_NOP();
    VoC_movreg16(REG3,ACC0_HI,DEFAULT);
    VoC_multf32_rr(REG23,REG1,REG3,DEFAULT);
    VoC_NOP();
    VoC_add32_rr(REG45,REG45,REG23,DEFAULT);
    VoC_endloop1

    VoC_return;
}


void CII_compute_CN_excitation_gain(void)
{

    /* norm = 0;  */
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);

COMPUTE_CN01:
    /* overfl = 0; */     /* L_temp = 0L */
    VoC_lw16i_short(REG6,0,DEFAULT);                //reg6:overfl
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0,COD_AMR_RES_ADDRESS,2);/* REG0 used as a point to res2 */
    VoC_loop_i_short(L_SUBFR/2,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG0,DEFAULT);/*REG6 = res2[]*/
    VoC_shr16_rr(REG4,REG7,DEFAULT);
    VoC_shr16_rr(REG5,REG7,IN_PARALLEL);
    VoC_bimac32_rr(REG45,REG45);
    VoC_endloop0
    VoC_NOP();
    VoC_bne32_rd( COMPUTE_CN00, ACC0,CONST_0x7FFFFFFF_ADDR )
    VoC_lw16i_short(REG6,1,DEFAULT);            //reg6:overfl
    VoC_add16_rr(REG7,REG7,REG6,DEFAULT);
COMPUTE_CN00:
    VoC_bnez16_r(COMPUTE_CN01,REG6);

    VoC_add32_rd(ACC0,ACC0,CONST_0x00000001_ADDR);  // ACC0 for L_temp

    VoC_jal(CII_NORM_L_ACC0);
    VoC_movreg16(REG0,REG1,DEFAULT);
    VoC_movreg16(REG1,ACC0_HI,DEFAULT); //REG1 for temp;reg0 :norm1;REG7 for norm

    VoC_multf32_rr(ACC0,REG1,REG1,DEFAULT);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_shr32_ri(ACC0,3,DEFAULT);
    VoC_lw16i(REG5,0x3000);
    VoC_sub32_rr(ACC0,REG45,ACC0,DEFAULT);/*ACC0=L_temp*/

    VoC_lw16i(REG4,24576);
    VoC_mac32_rr(REG1,REG4,DEFAULT);
    VoC_movreg16(REG5,REG0,DEFAULT);
    /*Accumulator[0] = L_temp*/
    VoC_movreg16(REG1,ACC0_HI,DEFAULT); /*REG1 = temp*/

    VoC_and16_rd(REG5,CONST_1_ADDR);
    VoC_bez16_r(COMPUTE_CN02,REG5);

    VoC_lw16i(REG5,23170);
    VoC_multf32_rr(ACC0,REG1,REG5,DEFAULT);
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);
    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000_ADDR);

COMPUTE_CN02:
    VoC_movreg16(REG1,ACC0_HI,DEFAULT); //REG1 for temp;reg0 :norm1;REG7 for norm
    VoC_lw16i(REG4,10362);
    VoC_multf32_rr(REG23,REG1,REG4,DEFAULT);
    VoC_shr16_ri(REG0,1,DEFAULT);
    VoC_add32_rd(REG23,REG23,CONST_0x00008000_ADDR);

    VoC_sub16_rr(REG0,REG0,REG7,DEFAULT);

    VoC_bltz16_r(COMPUTE_CN05,REG6)
    VoC_shr16_rr(REG3,REG0,DEFAULT);/*REG3=temp,REG0=norm1*/
    VoC_jump(COMPUTE_CN06)
COMPUTE_CN05:
    VoC_sub16_dr(REG0,CONST_0_ADDR,REG0);
    VoC_shr16_rr(REG3,REG0,DEFAULT);
COMPUTE_CN06:

    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG3,GLOBAL_CN_EXCITATION_GAIN_ADDR);

    VoC_return;
}

// REG3 for GAIN_CODE
void CII_update_gain_code_history_tx(void)
{
    /* begin of update_gain_code_history_tx (*gain, gain_code_old_tx); */
    VoC_lw16i(REG2,STATIC_BUF_P_TX_ADDR);      //REG2:addr of buf_p_tx
    VoC_lw16i(REG0,GLOBAL_GAIN_CODE_OLD_TX_ADDR);//REG0:addr of gain_code_old_tx[0]
//  VoC_shr16_ri(REG3,1,DEFAULT);
    VoC_add16_rp(REG0,REG0,REG2,DEFAULT);   //REG0:addr of gain_code_old_tx[buf_p_tx]
//if (sub (buf_p_tx, (4 * DTX_HANGOVER - 1)) == 0)
    VoC_lw16i_short(REG6,27,DEFAULT);
    VoC_sw16_p(REG3,REG0,DEFAULT);
    VoC_sub16_pr(REG3,REG2,REG6,DEFAULT);
    VoC_bnez16_r(UPDATE_GAIN_CODE_HISTORY_TX103,REG3);
// buf_p_tx = 0;
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_NOP();
    VoC_sw16_p(REG0,REG2,DEFAULT);
    VoC_jump(UPDATE_GAIN_CODE_HISTORY_TX104);
UPDATE_GAIN_CODE_HISTORY_TX103://else
//buf_p_tx = add (buf_p_tx, 1);
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_add16_rp(REG0,REG0,REG2,DEFAULT);
    VoC_NOP();
    VoC_sw16_p(REG0,REG2,DEFAULT);
UPDATE_GAIN_CODE_HISTORY_TX104:
    /* end of update_gain_code_history_tx (*gain, gain_code_old_tx); */
    VoC_return;
}

void CII_CN_encoding(void)
{
    VoC_lw16_d(REG6,GLOBAL_TXDTX_CTRL_ADDR);
    VoC_and16_ri(REG6,TX_SID_UPDATE);
    VoC_bez16_r(CII_CN_encoding01,REG6)
    /* Store new CN parameters in memory to be used later as old
       CN parameters */

    /* LPC parameter indices */
    VoC_lw16i_set_inc(REG0,COD_AMR_ANA_ADDR,2);
    VoC_lw16i_set_inc(REG1,STATIC_OLD_CN_MEM_TX_ADDR,2);
    VoC_lw16_d(REG7,COD_AMR_ANA_ADDR+56);

    VoC_loop_i_short(3,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(REG23,REG0,DEFAULT);
    VoC_NOP();
    VoC_sw32inc_p(REG23,REG1,DEFAULT);
    VoC_endloop0
    /* Codebook index computed in last subframe */
    VoC_sw16_d(REG7,STATIC_OLD_CN_MEM_TX_ADDR+5);

CII_CN_encoding01:
    VoC_lw16_d(REG6,GLOBAL_TXDTX_CTRL_ADDR);
    VoC_and16_ri(REG6,TX_USE_OLD_SID);
    VoC_bez16_r(CII_CN_encoding02,REG6)
    /* Use old CN parameters previously stored in memory */
    VoC_lw16i_set_inc(REG0,STATIC_OLD_CN_MEM_TX_ADDR,1);
    VoC_lw16i_set_inc(REG1,COD_AMR_ANA_ADDR,1);
    VoC_lw16_d(REG7,STATIC_OLD_CN_MEM_TX_ADDR+5);
    VoC_loop_i_short(5,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG2,REG0,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG2,REG1,DEFAULT);
    VoC_endloop0
    VoC_sw16_d(REG7,COD_AMR_ANA_ADDR+17);
    VoC_sw16_d(REG7,COD_AMR_ANA_ADDR+30);
    VoC_sw16_d(REG7,COD_AMR_ANA_ADDR+43);
    VoC_sw16_d(REG7,COD_AMR_ANA_ADDR+56);

CII_CN_encoding02:
    /* Set all the rest of the parameters to zero (SID codeword will
    be written later) */
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_set_inc(REG0,COD_AMR_ANA_ADDR+5,1);
    VoC_lw16i_set_inc(REG1,COD_AMR_ANA_ADDR+18,1);
    VoC_lw16i_set_inc(REG2,COD_AMR_ANA_ADDR+31,1);
    VoC_lw16i_set_inc(REG3,COD_AMR_ANA_ADDR+44,1);
    VoC_loop_i_short(12,DEFAULT)
    VoC_startloop0
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,DEFAULT);
    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    VoC_endloop0

    VoC_return
}

void CII_sid_codeword_encoding(void)
{
    VoC_lw16i_set_inc(REG0,STATIC_CONST_SID_CODEWORD_BIT_IDX_ADDR,2);
    VoC_lw16i(REG6,BIT_SERIAL_ADDR_RAM);
    VoC_movreg16(REG7,REG6,DEFAULT);
    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_loop_i_short(47,DEFAULT)
    VoC_startloop0
    //reg45:&ser2[SID...[2*i]] and &ser2[SID...[2*i+1]]
    VoC_add32inc_rp(REG45,REG67,REG0,DEFAULT);
    VoC_NOP();
    VoC_sw16_p(REG1,REG4,DEFAULT);
    VoC_sw16_p(REG1,REG5,DEFAULT);
    VoC_endloop0
    VoC_sw16_d(REG1,BIT_SERIAL_ADDR_RAM+221);   //SID_codeword_bit_idx[94]=221

    VoC_return

}

void CII_plsf5_dtx1(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16_d(REG6,GLOBAL_TXDTX_CTRL_ADDR);
    VoC_movreg16(REG7,REG6,DEFAULT);
    VoC_movreg32(REG45,REG67,DEFAULT);
    VoC_and16_ri(REG4,TX_SP_FLAG);
    VoC_and16_ri(REG5,TX_SID_UPDATE);
    VoC_and16_ri(REG6,TX_HANGOVER_ACTIVE);
    VoC_and16_ri(REG7,TX_PREV_HANGOVER_ACTIVE);
    VoC_sw32_d(REG45,Q_PLSF_5_DTX_AND_SP_ADDR);
    VoC_sw32_d(REG67,Q_PLSF_5_DTX_AND_HAN_ADDR);

    VoC_bnez16_d(Q_PLSF_5_01,Q_PLSF_5_DTX_AND_SP_ADDR)
    VoC_bez16_d(Q_PLSF_5_01,Q_PLSF_5_DTX_AND_PREV_HAN_ADDR)
    /* Update LSF CN quantizer "memory" */
    VoC_lw16i_set_inc(REG0,GLOBAL_LSF_OLD_TX_ADDR,10);
    VoC_lw16i_set_inc(REG1,STATIC_LSF_P_CN_ADDR,1);
    VoC_jal(CII_update_lsf_p_CN);

Q_PLSF_5_01:
    VoC_bez16_d(Q_PLSF_5_02,Q_PLSF_5_DTX_AND_SID_ADDR);
    /* New SID frame is to be sent:
    Compute average of the current LSFs and the LSFs in the history */

    VoC_lw16i(REG0,Q_PLSF_5_LSF1_ADDR);
    VoC_lw16i(REG1,Q_PLSF_5_LSF2_ADDR);
    VoC_lw16i_set_inc(REG2,GLOBAL_LSF_OLD_TX_ADDR,10);
    VoC_lw16i_set_inc(REG3,Q_PLSF_5_LSF_AVER_ADDR,1);
    VoC_jal(CII_aver_lsf_history);

Q_PLSF_5_02:
    VoC_bnez16_d(Q_PLSF_5_03,Q_PLSF_5_DTX_AND_SP_ADDR);
    /* Update LSF history with unquantized LSFs when no speech activity
     is present */
    VoC_lw16i(REG0,Q_PLSF_5_LSF1_ADDR);
    VoC_lw16i(REG1,Q_PLSF_5_LSF2_ADDR);
    VoC_movreg32(RL6,REG01,DEFAULT);
    VoC_lw16i(REG0,GLOBAL_LSF_OLD_TX_ADDR);
    VoC_jal(CII_update_lsf_history);

Q_PLSF_5_03:
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}
void CII_plsf5_dtx2(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16i(REG0,Q_PLSF_5_LSF_AVER_ADDR);
    VoC_lw16i(REG1,Q_PLSF_5_WF2_ADDR);
    VoC_jal(CII_Lsf_wt);

    VoC_lw16i_set_inc(REG0,Q_PLSF_5_WF1_ADDR,2);        //REG0 addr of wf1[i]
    VoC_lw16i_set_inc(REG1,Q_PLSF_5_LSF_AVER_ADDR,2);   //REG1 addr of lsf_aver[i]
    VoC_lw16i_set_inc(REG2,Q_PLSF_5_LSF1_ADDR,2);       //REG2 addr of lsf1[i]
    VoC_lw16i_set_inc(REG3,Q_PLSF_5_LSF2_ADDR,2);       //REG3 addr of lsf2[i]
    VoC_lw32z(ACC0,DEFAULT);

    VoC_pop16(RA,DEFAULT);
    VoC_loop_i_short((M/2),DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(REG67,REG1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG2,DEFAULT);
    VoC_sw32inc_p(REG67,REG3,DEFAULT);
    VoC_endloop0
    VoC_return;

}
void CII_plsf5_dtx3(void)
{
    VoC_lw16i_set_inc(REG0, Q_PLSF_5_LSF1_ADDR,2);          //REG0 addr of lsf1[i]
    VoC_lw16i_set_inc(REG1, Q_PLSF_5_LSF_R1_ADDR,2);        //REG1 addr of lsf_r1[i]
    VoC_lw16i_set_inc(REG2, Q_PLSF_5_LSF2_ADDR,2);          //REG2 addr of lsf1[i]
    VoC_lw16i_set_inc(REG3, Q_PLSF_5_LSF_R2_ADDR,2);        //REG3 addr of lsf_r1[i]

    VoC_lw16i(REG4,STATIC_LSF_P_CN_ADDR);//REG1 addr of lsf_p_CN[i]
    VoC_lw16i_short(REG5,2,DEFAULT);
    VoC_lw32_p(REG67,REG4,DEFAULT);
    VoC_loop_i_short((M/2),DEFAULT)
    VoC_startloop0
    VoC_sub16inc_pr(REG6,REG0,REG6,DEFAULT);
    VoC_sub16inc_pr(REG7,REG0,REG7,IN_PARALLEL);
    VoC_movreg32(RL6,REG67,DEFAULT);
    VoC_lw32_p(REG67,REG4,IN_PARALLEL);
    VoC_sub16inc_pr(REG6,REG2,REG6,DEFAULT);
    VoC_sub16inc_pr(REG7,REG2,REG7,IN_PARALLEL);
    VoC_sw32inc_p(RL6,REG1,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);
    VoC_sw32inc_p(REG67,REG3,DEFAULT);
    VoC_lw32_p(REG67,REG4,DEFAULT);
    VoC_endloop0
    VoC_pop16(REG2,DEFAULT);    //pop16(n-3)     REG2:&lsf2[i]
    VoC_pop16(REG0,DEFAULT);    //pop16(n-2)     REG0:&lsf1[i]
    VoC_return;
}
void CII_plsf5_dtx4(void)
{
    VoC_lw16i_set_inc(REG0,STRUCT_Q_PLSFSTATE_PAST_RQ_ADDR,2);
    VoC_lw32z(RL6,DEFAULT);

    VoC_loop_i_short((M/2),DEFAULT);
    VoC_startloop0
    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_endloop0
    VoC_loop_i_short(2,DEFAULT);
    VoC_startloop0
    VoC_pop16(REG0,DEFAULT);
    VoC_pop32(REG23,IN_PARALLEL);
    VoC_endloop0
    VoC_return;
}

void CII_PRM2_BITS_POST_PROCESS_EFR(void)
{
    VoC_pop16(REG3,DEFAULT);
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
//   Copy(&st->old_speech[L_FRAME], &st->old_speech[0], L_TOTAL - L_FRAME);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_OLD_SPEECH_ADDR+160,2);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_OLD_SPEECH_ADDR,2);


    VoC_jal(CII_copy_xy);

    VoC_lw16_d(REG5, STRUCT_COD_AMRSTATE_OLD_WSP_ADDR+160);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_OLD_WSP_ADDR+160+1,2);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_OLD_WSP_ADDR+1,2);
    VoC_sw16_d(REG5, STRUCT_COD_AMRSTATE_OLD_WSP_ADDR);

    VoC_lw32inc_p(REG67,REG0,DEFAULT);

    exit_on_warnings = OFF;
    VoC_loop_i(0,71)
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG1,DEFAULT);
    VoC_endloop0

    exit_on_warnings = ON;
    VoC_bnez16_d(COD12D2_NO_CN_ENCODING,GLOBAL_OUTPUT_SP_ADDR)
    /* Write comfort noise parameters into the parameter frame.
    Use old parameters in case SID frame is not to be updated */

    VoC_jal(CII_CN_encoding);
COD12D2_NO_CN_ENCODING:
// begin of prm2bits
    VoC_lw16i_set_inc(REG0,STATIC_CONST_BITNO_ADDR,1);  //REG0:&bitno
    VoC_lw16i(REG5,57);
    VoC_jal(CII_prm2bits_amr_efr);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
// end of prm2bits
    VoC_return;

}




void CII_EFR_DTX_PART1(void)
{
    VoC_push16(RA,DEFAULT);
    //   Residu(Aq, speech, res2, L_SUBFR);
    VoC_lw16_sd(REG0,3,DEFAULT);

    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG2,COD_AMR_RES_ADDRESS,1);
    VoC_lw16_sd(REG6,1,DEFAULT);    // i_subfr
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR,1);
    VoC_add16_rr(REG3,REG6,REG3,DEFAULT);
    VoC_jal(CII_Residu_new);
    /* Compute comfort noise excitation gain based on
    LP residual energy */
    //VoC_lw16i_set_inc(REG0,COD_AMR_RES_ADDRESS,2);
    //VoC_lw16i_set_inc(REG1,COD_AMR_RES2_ADDRESS,2);
    //VoC_lw16i_short(REG6,10,DEFAULT);
    // VoC_jal(CII_copy_xy);

//            CN_excitation_gain = compute_CN_excitation_gain (res2);
    VoC_jal(CII_compute_CN_excitation_gain);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16_sd(REG1,6,IN_PARALLEL);
    VoC_add16_rd(REG1,REG1,CONST_12_ADDR);
    VoC_sw16_d(REG0,COD_AMR_GAIN_PIT_ADDRESS);
    VoC_sw16_sd(REG1,6,DEFAULT);
    /* update LTP lag history
    if ((subfrNr == 0) && (st->ol_gain_flg[0] > 0))
    {
       st->old_lags[1] = T0;     move16 ();
    }
    */
    VoC_lw16_d(REG7,COD_AMR_T0_ADDRESS);
    VoC_lw16_sd(REG6,5,DEFAULT);
    VoC_bnez16_r(cod_amr1116,REG6)
    VoC_bngtz16_d(cod_amr1116,STRUCT_COD_AMRSTATE_OL_GAIN_FLG_ADDR)
    VoC_sw16_d(REG7,STRUCT_COD_AMRSTATE_OLD_LAGS_ADDR+1);
cod_amr1116:

    /*  if ((sub(subfrNr, 3) == 0) && (st->ol_gain_flg[1] > 0))
          {
             st->old_lags[0] = T0;     move16 ();
          }
    */
    VoC_bne16_rd(cod_amr1117,REG6,CONST_3_ADDR)
    VoC_bngtz16_d(cod_amr1117,STRUCT_COD_AMRSTATE_OL_GAIN_FLG_ADDR+1)
    VoC_sw16_d(REG7,STRUCT_COD_AMRSTATE_OLD_LAGS_ADDR);
cod_amr1117:
    // build_CN_code ( &L_pn_seed_tx,code);
    VoC_lw32_d(ACC0,GLOBAL_L_PN_SEED_TX_ADDR); // L_pn_seed_rx
    VoC_lw16i_set_inc(REG0,COD_AMR_CODE_ADDRESS,2);
    VoC_jal(CII_build_CN_code);
    VoC_sw32_d(ACC0,GLOBAL_L_PN_SEED_TX_ADDR);
    VoC_lw16_d(REG1,GLOBAL_TXDTX_CTRL_ADDR);
    VoC_and16_rd(REG1,CONST_8_ADDR);       //REG1:addr of txdtx_ctrl
    VoC_bez16_r(Q_GAIN_CODE104,REG1);            //if ((txdtx_ctrl & TX_PREV_HANGOVER_ACTIVE) = 0,jump!
    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_bnez16_r(Q_GAIN_CODE104,REG0);//REG0:addr of i_subfr
//input gain_code_old addr : REG0
//return REG5
    VoC_lw16i_set_inc(REG0,GLOBAL_GAIN_CODE_OLD_TX_ADDR,2);
    VoC_jal(CII_update_gcode0_CN);


//gcode0_CN = shl (gcode0_CN, 4);
    VoC_shr16_ri(REG5,-4,DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG5,GLOBAL_GCODE0_CN_ADDR);
Q_GAIN_CODE104:
//*gain = CN_excitation_gain;
    VoC_lw16_d(REG7,GLOBAL_CN_EXCITATION_GAIN_ADDR);//REG0:addr of CN_excitation_gain
    VoC_lw16i(REG0,GLOBAL_TXDTX_CTRL_ADDR);//REG0:addr of txdtx_ctrl
    VoC_sw16_d(REG7,COD_AMR_GAIN_CODE_ADDRESS);
// if ((txdtx_ctrl & TX_SID_UPDATE) != 0)
    VoC_lw16_p(REG1,REG0,DEFAULT);//REG1:value of txdtx_ctrl
    VoC_movreg16(REG0,REG1,DEFAULT);
    VoC_and16_ri(REG0,TX_SID_UPDATE);
    VoC_bez16_r(Q_GAIN_CODE105,REG0);
    //if ((txdtx_ctrl & TX_SID_UPDATE) = 0,jump!

    /* begin of aver_gain = aver_gain_code_history (CN_excitation_gain,gain_code_old_tx); */
    VoC_lw16i(REG4,0x470);
    VoC_lw16i(REG5,0x470);
    VoC_lw16i_set_inc(REG0,GLOBAL_GAIN_CODE_OLD_TX_ADDR,2); /* REG0 for gain_code_old[] address */
    VoC_multf32_rr(ACC0,REG4,REG7,DEFAULT);         /* L_ret = L_mult (0x470, CN_excitation_gain); */

    VoC_aligninc_pp(REG0,REG0,DEFAULT);
    VoC_loop_i_short(14,DEFAULT)                    /* for (i = 0; i < (4 * DTX_HANGOVER); i++) */
    VoC_startloop0
    VoC_bimac32inc_rp(REG45,REG0);              /* L_ret = L_mac (L_ret, 0x470, gain_code_old[i]); */
    VoC_endloop0
//  VoC_lw16i_set_inc(REG1,STATIC_CONST_QUA_GAIN_CODE_ADDR,1);//REG1:addr of qua_gain_code[0]
    /* return extract_h (L_ret); */

    VoC_lw16i_set_inc(REG1,STATIC_CONST_qua_gain_code_ADDR,3);

    /* end of aver_gain = aver_gain_code_history (CN_excitation_gain,gain_code_old_tx); */

// err_min = abs_s (sub (aver_gain,  mult (gcode0_CN, qua_gain_code[0])));
    VoC_lw16_d(REG0,GLOBAL_GCODE0_CN_ADDR);    //REG0: gcode0_CN

    VoC_multf16_rp(REG4,REG0,REG1,DEFAULT);//REG0:mult result
    VoC_movreg32(REG23,ACC0,DEFAULT);
    VoC_sub16_rr(REG2,REG3,REG4,DEFAULT);//REG2:sub result

//CII_abs16(REG2,REG2,Q_GAIN_CODE4);   //REG2:value of err_min
    VoC_bnltz16_r(Q_GAIN_CODE_ABS_02,REG2)
    VoC_sub16_dr(REG2,CONST_0_ADDR,REG2);
Q_GAIN_CODE_ABS_02:
//index = 0;

    VoC_lw16i_short(RL6_LO,0,DEFAULT);
    VoC_lw16_sd(REG6,6,IN_PARALLEL);

//for (i = 1; i < NB_QUA_CODE; i++)
//   {err = abs_s (sub (aver_gain,  mult (gcode0_CN, qua_gain_code[i])));
//   if (sub (err, err_min) < 0){err_min = err; index = i; }
//     }
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_inc_p(REG1,DEFAULT);                /* REG1:addr of qua_gain_code[1] */
//  VoC_lw16_p(REG0,REG0,IN_PARALLEL);
    VoC_loop_i_short(31,DEFAULT);
    VoC_startloop0
    VoC_multf16inc_rp(REG4,REG0,REG1,DEFAULT);
    VoC_NOP();
    VoC_sub16_rr(REG4,REG3,REG4,DEFAULT);
    VoC_bnltz16_r(Q_GAIN_CODE_ABS3,REG4)
    VoC_sub16_dr(REG4,CONST_0_ADDR,REG4);
Q_GAIN_CODE_ABS3:
    VoC_sub16_rr(REG5,REG4,REG2,DEFAULT);
    VoC_bnltz16_r(Q_GAIN_CODE106,REG5);
    VoC_movreg16(REG2,REG4,DEFAULT);    //err_min = err
    VoC_movreg16(RL6_LO,REG7,IN_PARALLEL);
Q_GAIN_CODE106:
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);  //i++
    VoC_endloop0
    exit_on_warnings = OFF;
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);
    VoC_sw16_p(RL6_LO,REG6,DEFAULT);
    VoC_sw16_sd(REG6,6,DEFAULT);
    exit_on_warnings = ON;
Q_GAIN_CODE105:
//  VoC_lw16_d(REG3,COD_AMR_GAIN_CODE_ADDRESS);
//  VoC_shr16_ri(REG3,-1,DEFAULT);
    VoC_lw16i_set_inc(REG1,STRUCT_GC_PREDST_PAST_QUA_EN_MR122_ADDR,1);
    VoC_lw16i(REG7,-2381);
    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0
    VoC_lw16_d(REG3,COD_AMR_GAIN_CODE_ADDRESS);
    VoC_jal(CII_update_gain_code_history_tx);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

void CII_EFR_LEVINSON_PART (void)
{
    VoC_lw16i_set_inc(REG2,LEVINSON_AL_ADDR-2,-1);

    VoC_lw16i_set_inc(REG0,LEVINSON_AL_ADDR+2,1);
    VoC_lw16_sd(REG4, 0,DEFAULT);    // i
    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);   //&Al[i-1]
    VoC_add16_rd(REG2, REG2,CONST_1_ADDR);   //piont to hi
    VoC_lw16i_set_inc(REG3,LEVINSON_ANL_ADDR,2);
    VoC_lw16i_set_inc(REG1,CONST_0x00010001_ADDR,0);//&0x00010001
    VoC_sub16_rd(REG4, REG4,CONST_1_ADDR);
    VoC_loop_r_short(REG4,DEFAULT)                   //   for (j = 1; j < i; j++)
    VoC_startloop0
    VoC_multf32_rp(ACC0, REG7, REG2,DEFAULT);
    VoC_multf16inc_rp(REG5, REG6, REG2,DEFAULT);
    VoC_multf16inc_rp(REG4, REG7, REG2,DEFAULT);
    VoC_mac32inc_pd(REG0,CONST_1_ADDR);
    VoC_bimac32inc_rp(REG45,REG1);                //    t0 = Mpy_32 (Kh, Kl, Ah[i - j], Al[i - j])

    VoC_multf32inc_pd(REG45,REG0,CONST_0x7FFF_ADDR);
    VoC_NOP();
    VoC_add32_rr(REG45, REG45, ACC0,DEFAULT);  //   t0 = L_add(t0, L_Comp(Ah[j], Al[j]));
    VoC_shru16_ri(REG4,1,DEFAULT);
//      VoC_and16_ri(REG4,0x7FFF);         //  L_Extract (t0, &Anh[j], &Anl[j]);
    VoC_NOP();
    VoC_sw32inc_p(REG45, REG3,DEFAULT);
    VoC_endloop0
    VoC_return;
}



void CII_EFR_DTX_ENC_RESET(void)
{
    VoC_lw16i_set_inc(REG0, STATIC_SCAL_RVAD_ADDR,2);
    VoC_lw32z(ACC1,DEFAULT);
    VoC_loop_i(0,72)
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG0, STATIC_L_SACF_ADDR,2);
    VoC_loop_i(0,27)
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_endloop0
    VoC_lw16i_set_inc(REG0, STATIC_L_SAV0_ADDR,2);
    VoC_loop_i(0,36)
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_endloop0



    VoC_lw16i_set_inc(REG0, STRUCT_COD_AMRSTATE_H1_ADDR,2);
    VoC_loop_i(0,20)
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i_short(REG6,7,DEFAULT);
    VoC_lw16i_short(REG4,20,IN_PARALLEL);
    VoC_lw16i(REG5,27083);
    VoC_sw16_d(REG6,STATIC_SCAL_RVAD_ADDR);
    VoC_sw16_d(REG6,STATIC_TXDTX_HANGOVER_ADDR);    // txdtx_hangover = DTX_HANGOVER;
    VoC_sw32_d(REG45,STATIC_THVAD_ADDR);
    VoC_lw16i(REG6,0x6000);
    VoC_lw16i_short(REG7,-1,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);
    VoC_lw16i(REG5,0x7FFF);
    VoC_sw16_d(REG6,STATIC_RVAD_ADDR);
    VoC_sw16_d(REG7,STATIC_HANGCOUNT_ADDR);
    VoC_sw16_d(REG4,GLOBAL_PTCH_ADDR);
    VoC_lw16i_short(REG7,3,DEFAULT);
    VoC_lw32_d(ACC1,CONST_0x70816958_ADDR);
    VoC_sw16_d(REG5,STATIC_TXDTX_N_ELAPSED_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_LSF_OLD_TX_ADDR,2);
    VoC_loop_i(1,7)
    VoC_lw16i_set_inc(REG1,TABLE_MEAN_LSF_ADDR,2);
    VoC_loop_i_short(5,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(ACC1,REG1,DEFAULT);
    VoC_sw16_d(REG7,GLOBAL_TXDTX_CTRL_ADDR);
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_endloop0
    VoC_sw32_d(ACC1,GLOBAL_L_PN_SEED_TX_ADDR);
    VoC_endloop1
    VoC_return;
}



void CII_efr_dtx_func11(void)
{
    VoC_lw16_d(REG7,GLOBAL_RXDTX_CTRL_ADDR);
    VoC_and16_ri(REG7,1);
    VoC_bez16_r(Ecgains_L3_bis,REG7);

    VoC_bez16_d(Ecgains_L3_bis,STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR);//    if (prev_bf != 0)  {
    VoC_lw16_p(REG7,REG2,DEFAULT);
    VoC_bngt16_rr(Ecgains_L3_bis,REG6,REG7);             //        if (sub (*gain_pitch, st->prev_gp) > 0)
    VoC_movreg16(REG6,REG7,DEFAULT);                 //        {  *gain_pitch = st->prev_gp;
    VoC_sw16_d(REG7,DEC_AMR_GAIN_PIT_ADDRESS);           //}
Ecgains_L3_bis:
    VoC_return;
}

void CII_SUBFRAME_EFR_PART(void)
{
    VoC_lw32z(ACC1,DEFAULT);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_MEM_ERROR_ADDR,2);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_MEM_W0_ADDR,2);
    VoC_loop_i_short(5,DEFAULT)
    VoC_startloop0
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_sw32inc_p(ACC1,REG1,DEFAULT);
    VoC_endloop0
    VoC_pop16(RL6_LO,DEFAULT);      // RL6_LO for kShift
    VoC_pop16(REG1,DEFAULT);
    VoC_return;
}


#ifdef voc_compile_only
// VoC_directive: FAKE_FUNCTION_ON
void CII_vad1(void) {};
void CII_tx_dtx_handler(void) {};
void CII_rx_dtx_handler(void) {};
void CII_vad_pitch_detection(void) {};
void CII_PRM2_BITS_POST_PROCESS(void) {};
void CII_check_lsp(void) {};
void CII_dtx_enc(void) {};
void CII_dtx_buffer(void) {};
void CII_gc_pred(void) {};
void CII_Q_plsf_3(void) {};
void CII_dtx_dec(void) {};
void CII_AMR_subfunc0(void) {};
void CII_RESET_AMR_CONSTANT(void) {};
void CII_Post_Process(void) {};

// VoC_directive: FAKE_FUNCTION_OFF

#endif
