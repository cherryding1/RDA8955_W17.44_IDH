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
#include "vpp_speech_asm_agc.h"


#if 0
voc_short For_AGC_align   ,816,x

// local memory for nonclip
voc_short GLOBAL_NON_CLIP_CROSSZEROINDEX[FRAMESIZE],x
// local memory for nonclip

voc_struct STRUCT_DigitalAgc_addr,x
voc_word STRUCT_DigitalAgc_capacitorSlow_addr
voc_word STRUCT_DigitalAgc_capacitorFast_addr
voc_word STRUCT_DigitalAgc_gain_addr
voc_word STRUCT_DigitalAgc_gainTable_addr[32]
voc_short STRUCT_DigitalAgc_gatePrevious_addr
voc_short STRUCT_DigitalAgc_upper_thr_addr
voc_short STRUCT_DigitalAgc_lower_thr_addr
voc_short STRUCT_DigitalAgc_decay_value_addr
voc_short STRUCT_DigitalAgc_capacitorSlowFactor_addr
voc_short STRUCT_DigitalAgc_capacitorFastFactor_addr
voc_short STRUCT_DigitalAgc_NoiseCompressFactor_addr
voc_short STRUCT_DigitalAgc_NoiseGateThr_addr
voc_struct_end

voc_struct STRUCT_AgcVadNearend_addr,x
voc_word STRUCT_AgcVadNearend_downState_addr[8]
voc_short STRUCT_AgcVadNearend_HPstate_addr
voc_short STRUCT_AgcVadNearend_counter_addr
voc_short STRUCT_AgcVadNearend_logRatio_addr             // log( P(active) / P(inactive) ) (Q10)
voc_short STRUCT_AgcVadNearend_meanLongTerm_addr         // Q10
voc_word STRUCT_AgcVadNearend_varianceLongTerm_addr      //Q8
voc_short STRUCT_AgcVadNearend_stdLongTerm_addr          // Q10
voc_short STRUCT_AgcVadNearend_meanShortTerm_addr        // Q10
voc_word STRUCT_AgcVadNearend_varianceShortTerm_addr     // Q8
voc_short STRUCT_AgcVadNearend_stdShortTerm_addr         // Q10
//voc_short STRUCT_AgcVadNearend_alignfiller
voc_short STRUCT_AgcVadNearend_NoiseStdShortTermFactor_addr
voc_struct_end

voc_struct STRUCT_AgcConfig_addr,x
voc_short STRUCT_AgcConfig_enable_addr
voc_short STRUCT_AgcConfig_targetLevelDbfs_addr      // default 3 (-3 dBOv)
voc_short STRUCT_AgcConfig_compressionGaindB_addr    // default 9 dB
voc_short STRUCT_AgcConfig_limiterEnable_addr        // default kAgcTrue (on)
//voc_short STRUCT_AgcConfig_alignfiller
voc_short STRUCT_AgcConfig_upper_thr_addr
voc_short STRUCT_AgcConfig_lower_thr_addr
voc_short STRUCT_AgcConfig_decay_value_addr
voc_short STRUCT_AgcConfig_capacitorSlowFactor_addr
voc_short STRUCT_AgcConfig_capacitorFastFactor_addr
voc_short STRUCT_AgcConfig_NoiseCompressFactor_addr
voc_short STRUCT_AgcConfig_NoiseGateThr_addr
voc_short STRUCT_AgcConfig_NoiseStdShortTermFactor_addr
voc_struct_end

voc_short CONST_SubFrameNum_addr,x           //= 1;
voc_short buf_addr_addr,x

voc_short FS_addr,x
//for vad function
voc_short Global_ProcessVad_dB,   x

//function: sqrt
voc_short Global_Sqrt_k_sqrt_2,   x
voc_short Global_Sqrt_sh, x

//////////////for vad function/////////////////////
voc_short Global_ProcessVad_buf1[8],  x
voc_short Global_ProcessVad_buf2[4],  x

voc_short Global_ProcessVad_in_cur_ptr,   x
voc_short Global_ProcessVad_nrSamples,    x
//////////////for vad function/////////////////////

// local vars for Agc1_Main
voc_alias LOCAL_X_digCompGaindB_addr STRUCT_AgcConfig_compressionGaindB_addr,x
voc_alias LOCAL_X_analogTarget_addr STRUCT_AgcConfig_compressionGaindB_addr,x

voc_word LOCAL_X_limiterLvlshf_addr,x
voc_word LOCAL_X_den_addr,x

voc_short LOCAL_maxGain_addr,x
voc_short LOCAL_zeroGainLvl_addr,x
voc_short LOCAL_diffGain_addr,x
voc_short LOCAL_X_constMaxGain_addr,x
voc_short LOCAL_X_limiterIdx_addr,x
voc_short LOCAL_X_intPart_addr,x
voc_short LOCAL_X_fracPart_addr,x

// local vars for Agc_ProcessDigital
voc_word APD_LOCAL_X_env_addr[20],x
voc_word APD_LOCAL_X_gains_addr[21],x
voc_word APD_LOCAL_X_7fffshf_addr,x
voc_word APD_LOCAL_X_gain32_addr,x

voc_short APD_LOCAL_X_L_addr,x
voc_short APD_LOCAL_X_L2_addr,x
voc_short APD_LOCAL_X_decay_addr,x

// IIR
voc_short GLOBAL_IIR_BAND_NUM0_USED_ADDR_ADDR,x
voc_short GLOBAL_IIR_BAND_NUM2_USED_ADDR_ADDR,x
voc_short GLOBAL_IIR_BAND_DEN1_USED_ADDR_ADDR,x
voc_short GLOBAL_IIR_BAND_Reversed,x

voc_short GLOBAL_IIR_BAND_NUM0_ADDR,x
voc_short GLOBAL_IIR_BAND_NUM1_ADDR,x
voc_short GLOBAL_IIR_BAND_NUM2_ADDR,x
voc_short GLOBAL_IIR_BAND_DEN0_ADDR,x
voc_short GLOBAL_IIR_BAND_DEN1_ADDR,x
voc_short GLOBAL_IIR_BAND_DEN2_ADDR,x

voc_short GLOBAL_IIR_BAND_XN_1_USED_ADDR_ADDR,x
voc_short GLOBAL_IIR_BAND_XN_2_USED_ADDR_ADDR,x
voc_short GLOBAL_IIR_BAND_YN_1_USED_ADDR_ADDR,x
voc_short GLOBAL_IIR_BAND_YN_2_USED_ADDR_ADDR,x

voc_word GLOBAL_IIR_BAND0_XN_1_ADDR,x
voc_word GLOBAL_IIR_BAND0_XN_2_ADDR,x
voc_word GLOBAL_IIR_BAND0_YN_1_ADDR,x
voc_word GLOBAL_IIR_BAND0_YN_2_ADDR,x

voc_word GLOBAL_IIR_BAND1_XN_1_ADDR,x
voc_word GLOBAL_IIR_BAND1_XN_2_ADDR,x
voc_word GLOBAL_IIR_BAND1_YN_1_ADDR,x
voc_word GLOBAL_IIR_BAND1_YN_2_ADDR,x

voc_word GLOBAL_IIR_BAND2_XN_1_ADDR,x
voc_word GLOBAL_IIR_BAND2_XN_2_ADDR,x
voc_word GLOBAL_IIR_BAND2_YN_1_ADDR,x
voc_word GLOBAL_IIR_BAND2_YN_2_ADDR,x

voc_word GLOBAL_IIR_BAND3_XN_1_ADDR,x
voc_word GLOBAL_IIR_BAND3_XN_2_ADDR,x
voc_word GLOBAL_IIR_BAND3_YN_1_ADDR,x
voc_word GLOBAL_IIR_BAND3_YN_2_ADDR,x

voc_word GLOBAL_IIR_BAND4_XN_1_ADDR,x
voc_word GLOBAL_IIR_BAND4_XN_2_ADDR,x
voc_word GLOBAL_IIR_BAND4_YN_1_ADDR,x
voc_word GLOBAL_IIR_BAND4_YN_2_ADDR,x

voc_word GLOBAL_IIR_BAND5_XN_1_ADDR,x
voc_word GLOBAL_IIR_BAND5_XN_2_ADDR,x
voc_word GLOBAL_IIR_BAND5_YN_1_ADDR,x
voc_word GLOBAL_IIR_BAND5_YN_2_ADDR,x

voc_word GLOBAL_IIR_BAND6_XN_1_ADDR,x
voc_word GLOBAL_IIR_BAND6_XN_2_ADDR,x
voc_word GLOBAL_IIR_BAND6_YN_1_ADDR,x
voc_word GLOBAL_IIR_BAND6_YN_2_ADDR,x

voc_short kGenFuncTable_addr[128],x

voc_short CONST_kGenFuncTableSize_addr,x                   //=128
//for vad function
voc_short   Global_ProcessVad_kAvgDecayTime,  x


voc_word agc_const_int_0_addr,x
voc_word agc_const_int_1_addr,x
voc_word agc_const_int_10_addr,x
voc_word agc_const_int_39_addr,x               //int = 39000
voc_word CONST_int_kLog10_addr,x               //int = 54426, log2(10)     in Q14

voc_short CONST_kLog10_addr,x                  //= 54426, log2(10)     in Q14
voc_short CONST_kLog10sh1_addr,x               //= 27213, log2(10)/2   in Q14
voc_short CONST_kLog10_2_addr,x                //= 49321, 10*log10(2)  in Q14
voc_short CONST_kLog10_2sh1_addr,x             //= 24660, kLog10_2/2   in Q14
voc_short CONST_kLogE_1_addr,x                 //= 23637, log2(e)      in Q14
voc_short CONST_kCompRatio_addr,x              //= 3;
voc_short CONST_kCompRatio_m1_addr,x           //= kCompRatio-1 = 2
voc_short CONST_kSoftLimiterLeft_addr,x        //= 1;
voc_short CONST_LinApprox_tmp16_1_addr,x       //= 9951 = (2 << 14) - constLinApprox;
voc_short CONST_LinApprox_tmp16_2_addr,x       //= 6433 = constLinApprox - (1 << 14);

//variables with initial value
voc_short limiterOffset_addr,x                        //=0


voc_short agc_const_1_addr,x
voc_short agc_const_2_addr,x
voc_short agc_const_15_addr,x
voc_short agc_const_9_addr,x
voc_short agc_const_8_addr,x
#endif

#if 0

voc_word    Global_DownsampleBy2_kResampleAllpass2[3],  x
voc_word    Global_DownsampleBy2_kResampleAllpass1[3],  x

voc_word    Global_ProcessVad_Const_WORD32_MAX, x
voc_word    Global_ProcessVad_Const_0x00007fff, x
voc_word    Global_ProcessVad_Const_0xffff8000, x
voc_word    Global_ProcessVad_Const_0x0000b504, x  //sqrt(0x7fffffff)

#endif

#if 0

voc_word apd_const_int_1_addr,x                //=0x1
voc_word apd_const_int_0x01000000_addr,x       //=0x1000000
voc_word apd_const_int_0x7fffff_addr,x         //=0x7fffff
voc_word apd_const_int_0x2D413CC_addr,x        //=0x2D413CC
voc_word apd_const_int_0x7f_addr,x             //=0x7f

//variables with initial value
//voc_short upper_thr_addr,x                   //= 1024; // Q10
//voc_short lower_thr_addr,x                   //= 0; // Q10

voc_short agc_const_8000_addr,x
voc_short agc_const_16000_addr,x
voc_short apd_const_0x0fff_addr,x            //=0x0fff=4095
voc_short apd_const_0xf000_addr,x            //=0xf000=-4096



#endif

void Agc1_Main(void)
{


    VoC_push16(RA,DEFAULT);

    //initial registers
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,DEFAULT);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,DEFAULT);
    VoC_sw16_d(REG0,CFG_DMA_WRAP);
    VoC_lw16i(STATUS,STATUS_CLR);
    VoC_lw16i_short(BITCACHE,0,DEFAULT);

    VoC_lw16i_short(REG0, 10, DEFAULT);
    VoC_lw16i(REG1, 8000);
    VoC_sw16_d(REG0,CONST_SubFrameNum_addr);
    VoC_sw16_d(REG1, FS_addr);

    VoC_jal(Agc_Init);

    VoC_lw16i_set_inc(REG0,STRUCT_AgcConfig_upper_thr_addr,1);
    VoC_lw16i_set_inc(REG1,STRUCT_DigitalAgc_upper_thr_addr,1);

    VoC_loop_i_short(7,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG2,REG0,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG2,REG1,DEFAULT);
    VoC_endloop0

    VoC_lw16_p(REG2,REG0,DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG2,STRUCT_AgcVadNearend_NoiseStdShortTermFactor_addr);


    //AgcConfig still in REG0,1,2, and load in the function in fact can be optimized

    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    //VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_lw16_d(REG1,LOCAL_X_digCompGaindB_addr);
    VoC_lw16_d(REG2,STRUCT_AgcConfig_limiterEnable_addr);
    VoC_lw16_d(REG3,LOCAL_X_analogTarget_addr)
    VoC_lw16_d(REG4,CONST_kCompRatio_addr);

    VoC_sub16_rr(REG5,REG1,REG3,DEFAULT);                             //digCompGaindB - analogTarget
    VoC_sub16_rd(REG6,REG4,agc_const_1_addr);                     //kCompRatio - 1


    /*****************************************/
    VoC_multf32_rr(ACC0,REG5,REG6,DEFAULT);                   //tmp32no1 = MUL_16_16(digCompGaindB - analogTarget, kCompRatio - 1);
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);    //bak kCompRatio to ACC1_LO

    VoC_sub16_rd(REG7,REG3,STRUCT_AgcConfig_targetLevelDbfs_addr);         //REG7 = tmp16no1 = analogTarget - targetLevelDbfs;

    VoC_lw16i_short(REG5,0,DEFAULT);      //clr msbs of (kCompRatio>>1), means kCompRatio > 0 in default
    VoC_shr16_ri(REG4,1,IN_PARALLEL);     //REG4 = kCompRatio >> 1

    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT); //+tmp32no1
    VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);              //clr msbs of kCompRatio, which is taken as >0 in default
    //VoC_movreg16(RL6_LO,REG5,IN_PARALLEL);             //REG5=kCompRatio, REG4=kCompRatio>>1
    VoC_jal(CII_div_mode32);

    /*
    REG0:
    REG1: LOCAL_X_digCompGaindB_addr
    REG2: STRUCT_AgcConfig_limiterEnable_addr
    REG3: LOCAL_X_analogTarget_addr
    REG4: (kCompRatio >> 1)
    REG5: 0
    REG6: (kCompRatio - 1)
    REG7: tmp16no1 = (analogTarget - targetLevelDbfs)
    ACC0: remainder
    ACC1: kCompRatio
    RL6:  not used
    RL7:  quotient
    */

    VoC_movreg16(REG5,RL7_LO,DEFAULT);
    VoC_add16_rr(REG5,REG7,REG5,DEFAULT);         //REG5= (tmp16no1+quotient)
    /*****************************************/



    VoC_bngt16_rr(LABLE_MAX_GAIN,REG7,REG5);
    VoC_movreg16(REG5,REG7,DEFAULT);
LABLE_MAX_GAIN:                                 //REG5= maxGain
    VoC_sw16_d(REG5,LOCAL_maxGain_addr);

    VoC_movreg16(REG7,ACC1_LO,DEFAULT);                   //move kCompRatio to REG7
    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);               //tmp32no1 = WEBRTC_SPL_MUL_16_16(maxGain, kCompRatio);
    VoC_movreg16(ACC1_LO,REG6,IN_PARALLEL);               //bake (kCompRatio-1) to ACC1_LO
    VoC_lw16i_short(REG7,0,DEFAULT);                      //clr msbs of ((kCompRatio-1)>>1)
    VoC_shr16_ri(REG6,1,IN_PARALLEL);                     //REG6 = ((kCompRatio-1)>>1)
    VoC_add32_rr(ACC0,ACC0,REG67,DEFAULT); //+tmp32no1
    //VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);              //clr msbs of (kCompRatio-1), which is taken as >0 in default, can be omitted
    VoC_jal(CII_div_mode32);

    /*
    REG0:
    REG1: LOCAL_X_digCompGaindB_addr
    REG2: STRUCT_AgcConfig_limiterEnable_addr
    REG3: LOCAL_X_analogTarget_addr
    REG4: (kCompRatio >> 1)
    REG5: maxGain
    REG6: ((kCompRatio - 1)>>1)
    REG7: 0
    ACC0: remainder
    ACC1: (kCompRatio-1)
    RL6:  not used
    RL7:  quotient
    */

    VoC_movreg16(REG7,RL7_LO,DEFAULT);
    VoC_sub16_rr(REG5,REG1,REG7,DEFAULT);                //REG5 = zeroGainLvl

    VoC_bgt16_rr(Agc_Set_Config_L0,REG1,REG3)            //if(digCompGaindB <= analogTarget)
    VoC_bez16_r(Agc_Set_Config_L0,REG2);                 //if(limiterEnable==0)
    VoC_lw16_d(REG6,CONST_kSoftLimiterLeft_addr);              //REG6 = kSoftLimiterLeft
    VoC_sub16_rr(REG7,REG3,REG1,DEFAULT);
    VoC_lw16i_short(REG2,0,IN_PARALLEL)
    VoC_add16_rr(REG7,REG7,REG6,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG7,DEFAULT);                //zeroGainLvl += (analogTarget - digCompGaindB + kSoftLimiterLeft);
    VoC_sw16_d(REG2,limiterOffset_addr);                 //limiterOffset=0

    /*
    REG0:
    REG1: LOCAL_X_digCompGaindB_addr
    REG2: 0
    REG3: LOCAL_X_analogTarget_addr
    REG4: (kCompRatio >> 1)
    REG5: zeroGainLvl
    REG6: kSoftLimiterLeft
    REG7: no use
    ACC0: remainder
    ACC1: (kCompRatio-1)
    RL6:  not used
    RL7:  quotient
    */
    //use REG2,6,7 and 5

Agc_Set_Config_L0:
    VoC_sw16_d(REG5,LOCAL_zeroGainLvl_addr);

    VoC_movreg16(REG2,ACC1_LO,DEFAULT);               //REG2=(kCompRatio-1)
    VoC_multf32_rr(ACC0,REG1,REG2,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);      //clr msbs of (kCompRatio>>1), means kCompRatio > 0 in default
    VoC_lw16_d(ACC1_LO,CONST_kCompRatio_addr)         //ACC1=kCompRatio
    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT);            //ACC0 = tmp32no1 + (kCompRatio >> 1)
    VoC_jal(CII_div_mode32);

    /*
    REG0:
    REG1: LOCAL_X_digCompGaindB_addr
    REG2: (kCompRatio-1)
    REG3: LOCAL_X_analogTarget_addr
    REG4: (kCompRatio >> 1)
    REG5: 0
    REG6: kSoftLimiterLeft
    REG7: no use
    ACC0: remainder
    ACC1: kCompRatio
    RL6:  not used
    RL7:  quotient
    */

    VoC_movreg16(REG5,RL7_LO,DEFAULT);               //REG5=diffGain
    VoC_bltz16_r(Agc_Set_Config_L1,REG5);
    VoC_blt16_rd(Agc_Set_Config_L2,REG5,CONST_kGenFuncTableSize_addr);
Agc_Set_Config_L1:
    VoC_return;                                  //maybe should add return value later

Agc_Set_Config_L2:
    VoC_sw16_d(REG5,LOCAL_diffGain_addr);

    //keep REG2,3,4 maybe 5
    VoC_lw16i(REG0,kGenFuncTable_addr);
    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_lw16_d(REG6,limiterOffset_addr);             //REG6 = limiterOffset
    VoC_lw16_p(REG0,REG0,DEFAULT);                   //REG0 = constMaxGain = kGenFuncTable[diffGain]; // in Q8
    VoC_add16_rr(REG4,REG6,REG4,IN_PARALLEL);        //REG4 = limiterOffset + (kCompRatio >> 1)

    VoC_movreg16(ACC0_LO,REG4,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);
    VoC_sw16_d(REG0,LOCAL_X_constMaxGain_addr);
    VoC_jal(CII_div_mode32);
    /*
    REG0: constMaxGain
    REG1: LOCAL_X_digCompGaindB_addr
    REG2: (kCompRatio-1)
    REG3: LOCAL_X_analogTarget_addr
    REG4: limiterOffset + (kCompRatio >> 1)
    REG5: diffGain
    REG6: limiterOffset
    REG7: no use
    ACC0: remainder
    ACC1: kCompRatio
    RL6:  not used
    RL7:  quotient
    */

    //REG1,4,5,7
    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG6,IN_PARALLEL);            //REG3 = limiterLvlX = analogTarget - limiterOffset
    VoC_lw16_d(REG4,CONST_kLog10_2_addr);                //REG4 = kLog10_2
    VoC_add16_rd(REG6,REG6,STRUCT_AgcConfig_targetLevelDbfs_addr);       //REG6 = limiterLvl = targetLevelDbfs + tmp16no1;
    VoC_movreg16(ACC0_LO,REG3,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);
    VoC_shr32_ri(ACC0,-13,DEFAULT);
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);

    VoC_shr32_ri(ACC1,1,DEFAULT);
    VoC_shr32_ri(REG67,-14,IN_PARALLEL);                 //REG67 = limiterLvl << 14;  // Q14, need to be stored

    //REG1,3 definitely can be used
    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_lw16i_short(REG3,20,IN_PARALLEL);
    VoC_sw32_d(REG67,LOCAL_X_limiterLvlshf_addr);
    VoC_jal(CII_div_mode32);

    VoC_and16_ri(REG0,0x0001);
    VoC_shru16_ri(REG1,1,DEFAULT);
    VoC_movreg16(REG6,RL7_LO,IN_PARALLEL);                   //limiterIdx，还没加尔
    VoC_add16_rd(REG6,REG6,agc_const_2_addr);


    VoC_lw16i_short(FORMATX,-1,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_bez16_r(Agc_Set_Config_L3,REG0);
    VoC_movreg16(ACC0_LO,REG3,DEFAULT);
Agc_Set_Config_L3:
    VoC_macX_rr(REG1,REG3,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_sw16_d(REG6,LOCAL_X_limiterIdx_addr);            //store limiterIdx
    VoC_NOP();
    VoC_sw32_d(ACC0,LOCAL_X_den_addr);                   //store den


    /*
    REG0: constMaxGain&1, seems useless
    REG1: constMaxGain>>1, seems useless
    REG2: (kCompRatio-1)
    REG3: 20
    REG4: kLog10_2
    REG5: diffGain
    REG6: limiterIdx
    REG7: no use
    ACC0: den
    ACC1: kLog10_2>>1
    RL6:  not used
    RL7:  quotient
    */
    VoC_lw16i_set_inc(REG0,0,1);
    VoC_loop_i(1,32);
    VoC_sub16_rd(REG1,REG0,agc_const_1_addr);
    VoC_multf32_rd(REG67,REG1,CONST_kCompRatio_m1_addr);         //In REG6: tmp16 = (short)WEBRTC_SPL_MUL_16_16(kCompRatio - 1, i - 1);

    /*******************************************/
    VoC_lw16_d(REG4,CONST_kLog10_2sh1_addr);
    VoC_lw16i_short(ACC0_HI,0,DEFAULT);
    VoC_bnltz16_r(Agc_Set_Config_L4,REG6);
    VoC_lw16i_short(ACC0_HI,-1,DEFAULT);                       //ACC0_HI = 0xffff

Agc_Set_Config_L4:
    VoC_movreg16(ACC0_LO,REG6,DEFAULT);
    VoC_macX_rr(REG6,REG4,DEFAULT);
    VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);
    /*******************************************/

//    VoC_lw16_d(REG4,CONST_kLog10_2_addr);
//    VoC_movreg16(REG3,REG4,DEFAULT);
//    VoC_shru16_ri(REG4,1,IN_PARALLEL);
//
//    VoC_and16_ri(REG3,0x0001);
//    VoC_lw32z(ACC0,DEFAULT);
//    VoC_bez16_r(Agc_Set_Config_L4,REG3);
//      VoC_movreg16(ACC0_LO,REG6,DEFAULT);
//Agc_Set_Config_L4:
//    VoC_macX_rr(REG6,REG4,DEFAULT);
    /******************tmp32 = WEBRTC_SPL_MUL_16_U16(tmp16, kLog10_2),can be optimized later on, by add a kLog10_2>>1 const*************************/

    VoC_lw16_d(ACC1_LO,CONST_kCompRatio_addr);
    VoC_add32_rd(ACC0,ACC0,agc_const_int_1_addr);

    VoC_jal(Div_SignedInt);                                     //RL7: quotient

    /*
    REG0: i=0
    REG1: i-1=-1
    REG2: (kCompRatio-1)
    REG3: kLog10_2&1
    REG4: kLog10_2>>1
    REG5: diffGain
    REG6: tmp16 = (short)WEBRTC_SPL_MUL_16_16(kCompRatio - 1, i - 1);
    REG7: no use
    ACC0: tmp32
    ACC1: kCompRatio
    RL6:  not used
    RL7:  quotient
    */
    VoC_lw16_d(RL6_LO,LOCAL_diffGain_addr);
    VoC_lw16i_short(RL6_HI,0,DEFAULT);            //RL6: now is (int)diffGain
    VoC_shr32_ri(RL6,-14,DEFAULT);
    VoC_sub32_rr(RL7,RL6,RL7,DEFAULT);                //RL7: inLevel = ((int)diffGain << 14) - inLevel;  // Q14

    VoC_movreg32(RL6,RL7,DEFAULT)                     //RL6: = inLevel
    VoC_bnltz32_r(Agc_ABS_0,RL6);
    VoC_sub32_dr(RL6,agc_const_int_0_addr,RL6);
Agc_ABS_0:                                    //RL6: = absInLevel
    VoC_movreg32(REG67,RL6,DEFAULT);
    VoC_movreg32(REG45,RL6,IN_PARALLEL);              //in fact, only need REG4, diffGain can still be reserved
    VoC_shru32_ri(REG67,14,DEFAULT);                   //REG6 = intPart = (unsigned short)(absInLevel >> 14);
    VoC_lw16i(REG1,kGenFuncTable_addr);
    VoC_add16_rr(REG1,REG1,REG6,DEFAULT);         //REG1 = &kGenFuncTable[intPart]
    VoC_and16_ri(REG4,0x3fff);                        //REG4 = fracPart = (unsigned short)(absInLevel & 0x00003FFF);
    VoC_lw16inc_p(REG7,REG1,DEFAULT);                  //REG7 = kGenFuncTable[intPart]
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);
    VoC_sub16_pr(REG5,REG1,REG7,DEFAULT);              //REG5 = tmpU16 = kGenFuncTable[intPart + 1] - kGenFuncTable[intPart]; // Q8
    VoC_movreg16(ACC0_LO,REG7,IN_PARALLEL);
    VoC_shru32_ri(ACC0,-14,DEFAULT);                    //consider IN_PARALLEL with next insn********!!!!!!!!!!!!!
    VoC_mac32_rr(REG5,REG4,DEFAULT);                   //ACC0 = tmpU32no1, >>8 to be logApprox
    VoC_sw16_d(REG6,LOCAL_X_intPart_addr);
    VoC_sw16_d(REG4,LOCAL_X_fracPart_addr);
    VoC_movreg32(REG45,ACC0,DEFAULT);                  //REG45 = tmpU32no1
    VoC_shru32_ri(ACC0,8,IN_PARALLEL);
    VoC_lw16i_short(FORMAT32,-16,DEFAULT);



    /*
    REG0: i=0
    REG1: &kGenFuncTable[intPart+1]
    REG2: (kCompRatio-1)
    REG3: kLog10_2&1
    REG4: tmpU32no1,together with REG5
    REG5:
    REG6: intPart
    REG7: kGenFuncTable[intPart]
    ACC0: logApprox
    ACC1: kCompRatio
    RL6:  absInLevel
    RL7:  InLevel
    */

    //can be used REG1,2,3,5,7,4,6, ACC0,1, and RL6,7; almost all of the regs,except REG0 for loop counter
    VoC_bnltz32_r(Agc_Set_Config_L6,RL7);          //if (inLevel < 0):
    //VoC_push32(RL7,DEFAULT);
    //VoC_push32(RL6,DEFAULT);                   //seems unnecessary
    VoC_movreg32(REG67,RL6,DEFAULT);                               //REG67 = absInLevel
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_jal(Coolsand_CLZU);
    VoC_lw16_d(REG1,CONST_kLogE_1_addr);

    VoC_blt16_rd(Agc_Set_Config_L7,REG2,agc_const_15_addr);        //if (zeros < 15),else:

    VoC_movreg16(REG3,REG6,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_and16_ri(REG3,0x0001);
    VoC_lw16i(STATUS,CARRY_ON);
    VoC_lw16i(STATUS,CARRY_CLR);
    VoC_bez16_r(Agc_Set_Config_L8,REG3);
    VoC_movreg16(ACC0_LO,REG1,DEFAULT);
Agc_Set_Config_L8:
    VoC_macX_rr(REG6,REG1,DEFAULT);
    VoC_mac32_rr(REG7,REG1,DEFAULT);                             //absInLevel is in range of signed int, thus can be directly multiplied
    VoC_NOP();
    VoC_lw16i(STATUS,CARRY_OFF);
    VoC_shru32_ri(ACC0,6,DEFAULT);                                //ACC0 = tmpU32no2, treated as unsigned
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    VoC_jump(Agc_Set_Config_L11);

Agc_Set_Config_L7:
    VoC_sub16_dr(REG3,agc_const_15_addr,REG2);
    VoC_shru32_rr(REG67,REG3,DEFAULT);                            //in fact, no difference with signed shr

    VoC_movreg16(REG3,REG6,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_and16_ri(REG3,0x0001);
    VoC_lw16i(STATUS,CARRY_ON);
    VoC_lw16i(STATUS,CARRY_CLR);
    VoC_bez16_r(Agc_Set_Config_L9,REG3);
    VoC_movreg16(ACC0_LO,REG1,DEFAULT);
Agc_Set_Config_L9:
    VoC_macX_rr(REG6,REG1,DEFAULT);

    VoC_mac32_rr(REG7,REG1,DEFAULT);
    VoC_lw16i(STATUS,CARRY_OFF);
    VoC_blt16_rd(Agc_Set_Config_L10,REG2,agc_const_9_addr);        //if (zeros < 9),else:
    VoC_sub16_rd(REG3,REG2,agc_const_9_addr);
    VoC_shru32_rr(ACC0,REG3,DEFAULT);                             //tmpU32no2 >>= zeros - 9;  // Q22
    VoC_lw16i_short(REG1,0,IN_PARALLEL);                         //REG1 = zerosScale = 0;
    VoC_jump(Agc_Set_Config_L11);

Agc_Set_Config_L10:
    VoC_sub16_dr(REG1,agc_const_9_addr,REG2);                    //REG1 = zerosScale = 9 - zeros;
    VoC_shru32_rr(REG45,REG1,DEFAULT);                            //tmpU32no1 >>= zerosScale;  // Q(zeros+13)

    /*
    REG0: i=0
    REG1: zerosScale
    REG2: zeros
    REG3: no matter
    REG4: tmpU32no1,together with REG5
    REG5:
    REG6: used out
    REG7: used out
    ACC0: tmpU32no2
    ACC1: kCompRatio
    RL6:  never mind
    RL7:  never mind
    */

Agc_Set_Config_L11:
    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_bngt32_rr(Agc_Set_Config_L6,REG45,REG67);                    //no matter signed or not, relative comparation and +/- will not be differed
    VoC_sub32_rr(ACC0,REG45,REG67,DEFAULT);
    VoC_sub16_dr(REG1,agc_const_8_addr,REG1);
    VoC_shru32_rr(ACC0,REG1,DEFAULT);                              //ACC0 = logApprox = (tmpU32no1 - tmpU32no2) >> (8 - zerosScale);  //Q14
    /*
    REG0: i=0
    REG1: 8-zerosScale
    REG2: zeros
    REG3: no matter
    REG4: tmpU32no1,together with REG5
    REG5:
    REG6: tmpU32no2,together with REG7
    REG7:
    ACC0: logApprox
    ACC1: kCompRatio
    RL6:  never mind
    RL7:  never mind
    */

//except ACC0,REG0, all other can be used
Agc_Set_Config_L6:
    VoC_lw16_d(REG1,LOCAL_X_constMaxGain_addr);
    VoC_lw16_d(REG2,LOCAL_maxGain_addr);
    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_movreg16(REG3,REG1,DEFAULT);
    VoC_shru16_ri(REG1,1,IN_PARALLEL);
    VoC_and16_ri(REG3,0x0001);
    VoC_bez16_r(Agc_Set_Config_MulU0,REG3);
    VoC_movreg16(ACC0_LO,REG2,DEFAULT);
Agc_Set_Config_MulU0:
    VoC_macX_rr(REG1,REG2,DEFAULT);
    VoC_movreg16(REG5,REG6,IN_PARALLEL);
    VoC_lw16_d(REG4,LOCAL_diffGain_addr);
    VoC_shr32_ri(ACC0,-6,DEFAULT);                             //ACC0 = numFIX = (maxGain * constMaxGain) << 6;  // Q14
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_and16_ri(REG5,0x0001);

    VoC_msuX_rr(REG6,REG4,DEFAULT);

    VoC_lw16i_short(ACC1_HI,0,DEFAULT);
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);                    //ACC1 = diffGain in 32 signed
    VoC_bez16_r(Agc_Set_Config_MulU1,REG5);
    VoC_sub32_rr(ACC0,ACC0,ACC1,DEFAULT);
Agc_Set_Config_MulU1:
    VoC_msu32_rr(REG7,REG4,DEFAULT);                           //ACC0 = numFIX -= (int)logApprox * diffGain;  // Q14

    /*
    REG0: i=0
    REG1: constMaxGain>>1
    REG2: maxGain
    REG3: constMaxGain&1
    REG4: diffGain
    REG5: logApprox&1
    REG6: logApprox_L16>>1
    REG7: logApprox_H16
    ACC0: numFIX
    ACC1: diffGain
    RL6:  never mind
    RL7:  never mind
    */

//except REG0,ACC0, all other can be used
    VoC_lw32_d(REG67,LOCAL_X_den_addr);
    VoC_movreg32(ACC1,REG67,DEFAULT);
    VoC_shr32_ri(REG67,8,IN_PARALLEL);
    VoC_bngt32_rr(Agc_Set_Config_L12,ACC0,REG67);
    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_jal(Coolsand_CLZS);
    VoC_jump(Agc_Set_Config_L13);

Agc_Set_Config_L12:
    VoC_movreg32(RL6,ACC1,DEFAULT);
    VoC_jal(Coolsand_CLZS);
    VoC_add16_rd(REG2,REG2,agc_const_8_addr);

Agc_Set_Config_L13:
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_sub16_rr(REG2,REG4,REG2,DEFAULT);                  //REG2 = -zeros
    VoC_add16_rd(REG3,REG2,agc_const_8_addr);                  //REG3 = 8-zeros
    VoC_shru32_rr(ACC0,REG2,DEFAULT);                           //ACC0 = numFIX <<= zeros;  // Q(14+zeros)
    VoC_shru32_rr(ACC1,REG3,IN_PARALLEL);                       //ACC1 = tmp32no1 = WEBRTC_SPL_SHIFT_W32(den, zeros - 8); // Q(zeros)

    /*
    REG0: i=0
    REG1: constMaxGain>>1
    REG2: -zeros
    REG3: 8-zeros
    REG4: 0
    REG5: logApprox&1
    REG6: den>>8, together with REG7
    REG7:
    ACC0: numFIX  // Q(14+zeros)
    ACC1: tmp32no1  // Q(zeros)
    RL6:  never mind
    RL7:  never mind
    */

    VoC_movreg32(RL6,ACC1,DEFAULT);
    VoC_shr32_ri(RL6,1,DEFAULT);                               //RL6 = tmp32no1 / 2
    VoC_bnltz32_r(Agc_Set_Config_L14,ACC0);
    VoC_lw16i(STATUS,CARRY_ON);
    VoC_lw16i(STATUS,CARRY_SET);
    VoC_sub32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_lw16i(STATUS,CARRY_OFF);
    VoC_jump(Agc_Set_Config_L15);

Agc_Set_Config_L14:
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);

Agc_Set_Config_L15:
    VoC_jal(Div_SignedInt);                                   //RL7 = y32 = numFIX / tmp32no1;  // in Q14

    VoC_bez16_d(Agc_Set_Config_L16,STRUCT_AgcConfig_limiterEnable_addr);                 //if(limiterEnable==0)
    VoC_bnlt16_rd(Agc_Set_Config_L16,REG0,LOCAL_X_limiterIdx_addr);
    VoC_sub16_rd(REG1,REG0,agc_const_1_addr);              //REG1 = i-1;

    VoC_lw16_d(REG2,CONST_kLog10_2sh1_addr);
    VoC_lw16i_short(ACC0_HI,0,DEFAULT);
    VoC_bnltz16_r(Agc_Set_Config_MulU2,REG1);
    VoC_lw16i_short(ACC0_HI,-1,DEFAULT);

Agc_Set_Config_MulU2:
    VoC_movreg16(ACC0_LO,REG1,DEFAULT);
    VoC_macX_rr(REG1,REG2,DEFAULT);                        //ACC0 = tmp32 = WEBRTC_SPL_MUL_16_U16(i - 1, kLog10_2); // Q14
    VoC_NOP();
    VoC_sub32_rd(ACC0,ACC0,LOCAL_X_limiterLvlshf_addr);
    VoC_add32_rd(ACC0,ACC0,agc_const_int_10_addr);

    VoC_lw16i_short(ACC1_LO,20,DEFAULT);
    VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);

    VoC_jal(Div_SignedInt);

    /*
    REG0: i=0
    REG1: i-1
    REG2:
    REG3:
    REG4:
    REG5:
    REG6:
    REG7:
    ACC0: never mind
    ACC1: never mind
    RL6:  never mind
    RL7:  y32
    */


Agc_Set_Config_L16:
    VoC_lw16_d(REG2,CONST_kLog10sh1_addr);                             //REG2 = kLog10>>1
    VoC_lw16i_short(ACC0_HI,0,DEFAULT);
    VoC_movreg32(REG67,RL7,IN_PARALLEL);                               //REG67 = y32
    VoC_lw32z(RL6,DEFAULT);
    VoC_bngt32_rd(Agc_Set_Config_L17,RL7,agc_const_int_39_addr);
    VoC_shr32_ri(REG67,1,DEFAULT);
    VoC_movreg16(REG3,REG6,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_and16_ri(REG3,0x0001);
    VoC_bez16_r(Agc_Set_Config_Mul32_0,REG3);
    VoC_movreg16(ACC0_LO,REG2,DEFAULT);
Agc_Set_Config_Mul32_0:
    VoC_macX_rr(REG6,REG2,DEFAULT);
    VoC_mac32_rr(REG7,REG2,DEFAULT);
    VoC_lw16i(RL6_LO,0x1000);
    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_shr32_ri(ACC0,13,DEFAULT);
    VoC_jump(Agc_Set_Config_L18);

Agc_Set_Config_L17:
    VoC_movreg16(REG3,REG6,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_and16_ri(REG3,0x0001);
    VoC_bez16_r(Agc_Set_Config_Mul32_1,REG3);
    VoC_movreg16(ACC0_LO,REG2,DEFAULT);
Agc_Set_Config_Mul32_1:
    VoC_macX_rr(REG6,REG2,DEFAULT);
    VoC_mac32_rr(REG7,REG2,DEFAULT);
    VoC_lw16i(RL6_LO,0x2000);
    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_shr32_ri(ACC0,14,DEFAULT);

    /*
    REG0: i=0
    REG1: i-1
    REG2: kLog10>>1
    REG3:
    REG4:
    REG5:
    REG6:
    REG7:
    ACC0: tmp32
    ACC1: never mind
    RL6:  never mind
    RL7:  y32
    */

Agc_Set_Config_L18:
    VoC_lw16i_short(ACC1_LO,0,DEFAULT);
    VoC_lw16i_short(ACC1_HI,4,IN_PARALLEL);              //ACC1 = 16 << 14 = 0x40000;
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);                //ACC0 = tmp32 += 16 << 14;

    VoC_bgt32_rd(Agc_Set_Config_L19,ACC0,agc_const_int_0_addr);         //if (tmp32 <= 0),else == if (tmp32 > 0)
    VoC_lw32z(ACC0,DEFAULT);
    VoC_jump(Agc_Set_Config_L22);

Agc_Set_Config_L19:
    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_shr32_ri(ACC0,14,IN_PARALLEL);
    VoC_and16_ri(REG6,0x3fff);                         //REG6 = fracPart = (unsigned short)(tmp32 & 0x00003FFF); // in Q14
    VoC_movreg16(REG2,ACC0_LO,DEFAULT);                //REG2 = intPart = (short)(tmp32 >> 14);
    VoC_movreg16(REG7,REG6,IN_PARALLEL);               //REG7 = fracPart
    VoC_shr16_ri(REG7,13,DEFAULT);
    VoC_lw16i_short(FORMAT32,13,IN_PARALLEL);
    VoC_lw16i(REG3,0x4000);                            //REG3 = 0x4000 = (1 << 14)
    VoC_bez16_r(Agc_Set_Config_L20,REG7)               //if ((fracPart >> 13) != 0)
    VoC_sub16_rr(REG4,REG3,REG6,DEFAULT);                    //tmp32no2 = (1 << 14) - fracPart;
    VoC_multf32_rd(REG67,REG4,CONST_LinApprox_tmp16_1_addr);         //REG67 = tmp32no2 *= tmp16;
    VoC_NOP();
    VoC_sub16_rr(REG6,REG3,REG6,DEFAULT);                    //tmp32no2 = (1 << 14) - tmp32no2; REG6 = fracPart = (unsigned short)tmp32no2;
    VoC_jump(Agc_Set_Config_L21);

Agc_Set_Config_L20:
    VoC_multf32_rd(REG67,REG6,CONST_LinApprox_tmp16_2_addr);
    VoC_NOP();

    /*
    REG0: i=0
    REG1: i-1
    REG2: intPart
    REG3: 0x4000 = (1 << 14)
    REG4: tmp32no2 = (1 << 14) - fracPart
    REG5:
    REG6: fracPart
    REG7: fracPart_hi, no use
    ACC0: tmp32
    ACC1: 16 << 14 = 0x40000
    RL6:  never mind
    RL7:  y32
    */

Agc_Set_Config_L21:                                      //REG6 = fracPart, REG2 = intPart
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_short(REG3,14,IN_PARALLEL);
    VoC_sub16_rr(REG4,REG7,REG2,DEFAULT);              //REG4 = 0 - intPart;
    VoC_sub16_rr(REG5,REG3,REG2,IN_PARALLEL);          //REG5 = 14 - intPart;
    VoC_lw16i_short(ACC0_LO,1,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);
    VoC_shr32_rr(ACC0,REG4,DEFAULT);
    VoC_shr32_rr(REG67,REG5,IN_PARALLEL);
    VoC_add32_rr(ACC0,ACC0,REG67,DEFAULT);                     //ACC0 = gainTable[i]

Agc_Set_Config_L22:
    VoC_lw16i(REG1,STRUCT_DigitalAgc_gainTable_addr);
    VoC_movreg16(REG2,REG0,DEFAULT);
    VoC_shr16_ri(REG2,-1,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG2,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_NOP();
    VoC_sw32_p(ACC0,REG1,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);                       //i++
    VoC_endloop1

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;

}


/**************************************************************************************
 * Function:    Agc_Init
 *
 * Description: init STRUCT_DigitalAgc_addr and STRUCT_AgcVadNearend_addr
 *
 * Inputs:
 *
 * Outputs:
 *
 * Used : REG3,REG4,REG5,REG6,REG7,ACC0,REG0
 *
 * Version 1.0  Create by  sixu       //2016
 **************************************************************************************/

void Agc_Init(void)
{
    VoC_lw16i(REG4,0x0000);
    VoC_lw16i(REG5,0x0800);           //0x08000000 = (int)(0.125f * 32768.0f * 32768.0f);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_short(REG3,3,IN_PARALLEL);
    VoC_lw16i_short(REG6,0x0000,DEFAULT);
    VoC_lw16i_short(REG7,0x0001,IN_PARALLEL);           //=65536
    VoC_sw32_d(REG45,STRUCT_DigitalAgc_capacitorSlow_addr);
    VoC_sw32_d(ACC0,STRUCT_DigitalAgc_capacitorFast_addr);
    VoC_sw32_d(REG67,STRUCT_DigitalAgc_gain_addr);
    VoC_sw16_d(REG6,STRUCT_DigitalAgc_gatePrevious_addr);

    VoC_sw16_d(REG3,STRUCT_AgcVadNearend_counter_addr);
    VoC_sw16_d(REG6,STRUCT_AgcVadNearend_HPstate_addr);
    VoC_sw16_d(REG6,STRUCT_AgcVadNearend_logRatio_addr);
    VoC_lw16i(REG3,0x3C00);           //REG3 = 0x3C00 = 15<<10, initial value of average input level (Q10)
    VoC_lw16i(REG6,0xF400);           //REG67 = 0x0001F400 = 500<<8, the initial value of variance(Q8)
    VoC_sw16_d(ACC0_HI,STRUCT_AgcVadNearend_stdLongTerm_addr);
    VoC_sw16_d(ACC0_HI,STRUCT_AgcVadNearend_stdShortTerm_addr);
    VoC_sw16_d(REG3,STRUCT_AgcVadNearend_meanLongTerm_addr);
    VoC_sw16_d(REG3,STRUCT_AgcVadNearend_meanShortTerm_addr);
    VoC_sw32_d(REG67,STRUCT_AgcVadNearend_varianceLongTerm_addr);
    VoC_sw32_d(REG67,STRUCT_AgcVadNearend_varianceShortTerm_addr);

    VoC_lw16i_set_inc(REG3,STRUCT_AgcVadNearend_downState_addr,2);
    VoC_loop_i_short(8,DEFAULT);
    VoC_startloop0;
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_endloop0;

    VoC_return;
}


/**************************************************************************************
 * Function:    Agc_Set_Config
 *
 * Description: calc gaintable use AgcConfig
 *
 * Inputs:
              STRUCT_AgcConfig_compressionGaindB_addr: for LOCAL_X_digCompGaindB_addr and LOCAL_X_analogTarget_addr both
              STRUCT_AgcConfig_targetLevelDbfs_addr: targetLevelDbfs
              STRUCT_AgcConfig_limiterEnable_addr: limiterEnable

              (
              at this point of time,
              REG0: STRUCT_AgcConfig_targetLevelDbfs_addr
              REG1: STRUCT_AgcConfig_compressionGaindB_addr
              REG2: STRUCT_AgcConfig_limiterEnable_addr
              )
 *
 * Outputs:     STRUCT_DigitalAgc_gainTable_addr
 *
 * Used : All.
 *
 * Version 1.0  Create by  sixu       //2016
 **************************************************************************************/



//input ACC0 / ACC1, both positive
//output   RL7 = ACC0/ACC1; ACC0 = ACC0%ACC1
void CII_div_mode32(void)
{
    VoC_push32(RL6,DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);
    VoC_lw16i_short(RL6_LO,1,DEFAULT);
    VoC_lw16i_short(RL6_HI,0,IN_PARALLEL);
CII_div_mode32_102:
//  VoC_loop_i(0,255)
    VoC_bgt32_rr(CII_div_mode32_101, ACC1,ACC0)
    VoC_sub32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_add32_rr(RL7,RL7,RL6,IN_PARALLEL);
//  VoC_endloop0
    VoC_jump(CII_div_mode32_102);
CII_div_mode32_101:
    VoC_pop32(RL6,DEFAULT);
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_CLZU
 *
 * Description:     count leading zeros on unsigned integer x
 *
 * Inputs:      RL6->x;
 *
 *
 * Outputs:     REG2->return
 *
 * Used : REG2,REG3,RL6,RL7
 *
 * Version 1.0  Create by  Xuml       07/17/2007
 *
 **************************************************************************************/
void Coolsand_CLZU(void)
{
    VoC_lw16i(REG2,31);
    VoC_bez32_r(CLZ_End,RL6)
    VoC_movreg32(RL7,RL6,DEFAULT);
    VoC_shru32_ri(RL6,16,IN_PARALLEL);

    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16i_short(REG3,16,IN_PARALLEL);

    VoC_bnez32_r(CLZ_L0,RL6)//if (!((unsigned int)x >> 16)) { numZeros += 16; x <<= 16; }
    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);
    VoC_shru32_ri(RL7,-16,IN_PARALLEL);
CLZ_L0:
    VoC_movreg32(RL6,RL7,DEFAULT);
    VoC_shru32_ri(RL6,24,DEFAULT);
    VoC_lw16i_short(REG3,8,IN_PARALLEL);
    VoC_bnez32_r(CLZ_L1,RL6)//if (!((unsigned int)x >> 24)) { numZeros +=  8; x <<=  8; }
    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);
    VoC_shru32_ri(RL7,-8,IN_PARALLEL);
CLZ_L1:
    VoC_movreg32(RL6,RL7,DEFAULT);
    VoC_shru32_ri(RL6,28,DEFAULT);
    VoC_lw16i_short(REG3,4,IN_PARALLEL);
    VoC_bnez32_r(CLZ_L2,RL6)//if (!((unsigned int)x >> 28)) { numZeros +=  4; x <<=  4; }
    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);
    VoC_shru32_ri(RL7,-4,IN_PARALLEL);
CLZ_L2:
    VoC_movreg32(RL6,RL7,DEFAULT);
    VoC_shru32_ri(RL6,30,DEFAULT);
    VoC_lw16i_short(REG3,2,IN_PARALLEL);
    VoC_bnez32_r(CLZ_L3,RL6)//if (!((unsigned int)x >> 30)) { numZeros +=  2; x <<=  2; }
    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);
    VoC_shru32_ri(RL7,-2,IN_PARALLEL);
CLZ_L3:

    VoC_shru32_ri(RL7,31,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_sub32_rr(REG23,REG23,RL7,DEFAULT);  //numZeros -= ((unsigned int)x >> 31);

CLZ_End:
    VoC_return;


}


/**************************************************************************************
 * Function:    Coolsand_CLZS
 *
 * Description:     count leading zeros on positive signed integer x
 *
 * Inputs:      RL6->x;
 *
 *
 * Outputs:     REG2->return
 *
 * Used : REG2,REG3,RL6,RL7
 *
 * Version 1.0  Create by  Xuml       07/17/2007
 *
 **************************************************************************************/
void Coolsand_CLZS(void)
{
    VoC_lw16i(REG2,32);
    VoC_bez32_r(CLZS_End,RL6)
    VoC_movreg32(RL7,RL6,DEFAULT);
    VoC_shru32_ri(RL6,16,IN_PARALLEL);

    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_lw16i_short(REG3,16,IN_PARALLEL);

    VoC_bnez32_r(CLZS_L0,RL6)//if (!((unsigned int)x >> 16))    { numZeros += 16; x <<= 16; }
    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);
    VoC_shru32_ri(RL7,-16,IN_PARALLEL);
CLZS_L0:
    VoC_movreg32(RL6,RL7,DEFAULT);
    VoC_shru32_ri(RL6,24,DEFAULT);
    VoC_lw16i_short(REG3,8,IN_PARALLEL);
    VoC_bnez32_r(CLZS_L1,RL6)//if (!((unsigned int)x >> 24))    { numZeros +=  8; x <<=  8; }
    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);
    VoC_shru32_ri(RL7,-8,IN_PARALLEL);
CLZS_L1:
    VoC_movreg32(RL6,RL7,DEFAULT);
    VoC_shru32_ri(RL6,28,DEFAULT);
    VoC_lw16i_short(REG3,4,IN_PARALLEL);
    VoC_bnez32_r(CLZS_L2,RL6)//if (!((unsigned int)x >> 28))    { numZeros +=  4; x <<=  4; }
    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);
    VoC_shru32_ri(RL7,-4,IN_PARALLEL);
CLZS_L2:
    VoC_movreg32(RL6,RL7,DEFAULT);
    VoC_shru32_ri(RL6,30,DEFAULT);
    VoC_lw16i_short(REG3,2,IN_PARALLEL);
    VoC_bnez32_r(CLZS_L3,RL6)//if (!((unsigned int)x >> 30))    { numZeros +=  2; x <<=  2; }
    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);
    VoC_shru32_ri(RL7,-2,IN_PARALLEL);
CLZS_L3:

    VoC_shru32_ri(RL7,31,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_sub32_rr(REG23,REG23,RL7,DEFAULT);  //numZeros -= ((unsigned int)x >> 31);

CLZS_End:
    VoC_return;


}


/**************************************************************************************
 * Function:    Div_SignedInt
 *
 * Description:     Div for signed int data
 *
 * Inputs:      ACC0: dividend
 *              ACC1: divisor
 *
 * Outputs:     RL7: quotient
 *
 * Used : REG2,REG3,RL6,RL7 in Coolsand_CLZS()
 *        REG4,REG5,REG6,REG7
 *
 * Version 1.0  Create by  sixu       //2016
 *
 **************************************************************************************/
void Div_SignedInt(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);


    VoC_bnltz32_r(ABS_0,ACC0);
    VoC_xor16_rr(REG4,REG7,DEFAULT);
    VoC_sub32_rr(ACC0,RL7,ACC0,IN_PARALLEL);
ABS_0:
    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_jal(Coolsand_CLZS);                               //return in REG2
    VoC_sub16_rr(REG6,REG5,REG2,DEFAULT);                 //REG6 = left shift bits of dividend
    VoC_lw32z(RL7,IN_PARALLEL);                           //clr RL7 again

    VoC_bnltz32_r(ABS_1,ACC1);
    VoC_xor16_rr(REG4,REG7,DEFAULT);
    VoC_sub32_rr(ACC1,RL7,ACC1,IN_PARALLEL);
ABS_1:
    VoC_movreg32(RL6,ACC1,DEFAULT);
    VoC_jal(Coolsand_CLZS);
    VoC_sub16_rr(REG7,REG5,REG2,DEFAULT);                 //REG7 = left shift bits of divisor


    VoC_shr32_rr(ACC0,REG6,DEFAULT);
    VoC_shr32_rr(ACC1,REG7,IN_PARALLEL);


    VoC_lw32z(RL7,DEFAULT);
    VoC_lw16i_short(RL6_LO,1,DEFAULT);
    VoC_lw16i_short(RL6_HI,0,IN_PARALLEL);

    VoC_lw16i(STATUS,CARRY_ON);
    VoC_loop_i_short(31,DEFAULT);
    VoC_startloop0
    VoC_lw16i(STATUS,CARRY_SET);
    VoC_sub32_rr(REG23,ACC0,ACC1,DEFAULT);
    VoC_shru32_ri(RL7,-1,IN_PARALLEL);//r3   <<= 1;
    VoC_bltz32_r(LABLE_DIV_L0,REG23);
    VoC_movreg32(ACC0,REG23,DEFAULT);
    VoC_add32_rr(RL7,RL7,RL6,IN_PARALLEL);//r3 = r3 + r2;
LABLE_DIV_L0:
    VoC_shru32_ri(ACC0,-1,DEFAULT);//divd <<= 1;
    VoC_endloop0
    VoC_lw16i(STATUS,CARRY_OFF);

    VoC_sub16_rr(REG2,REG6,REG7,DEFAULT);
    VoC_lw16i_short(REG3,30,IN_PARALLEL);
    VoC_sub16_rr(REG2,REG3,REG2,DEFAULT);
    VoC_shr32_rr(RL7,REG2,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);                           //clr RL7 again
    VoC_bez16_r(ABS_2,REG4)                              //if quotient > 0, else
    VoC_sub32_rr(RL7,RL6,RL7,DEFAULT);

ABS_2:
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}







/*****************************************
Global RAM_Y const
******************************************/


/****************************************
function: WebRtcAgc_ProcessVad;

  add by quanwz 2016/04/05
*****************************************/
void Agc_ProcessVad(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,IN_PARALLEL);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,IN_PARALLEL);

    //test code
    //end

    exit_on_warnings =ON;

    //todo: dma in buf

    VoC_lw16_d(REG0,buf_addr_addr);

    VoC_lw16_d(REG7,CONST_SubFrameNum_addr);
    VoC_lw16_d(REG4,APD_LOCAL_X_L2_addr);

    VoC_movreg16(REG5,REG7,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);//subfr = 0

    VoC_lw32z(RL7,DEFAULT);//nrg = 0;
    VoC_shr16_rr(REG5,REG4,IN_PARALLEL);

    VoC_sw16_d(REG0,Global_ProcessVad_in_cur_ptr);
    VoC_sw16_d(REG5,Global_ProcessVad_nrSamples);

agc_processvad_subfr_loop_start:
    VoC_bngt16_rr(agc_processvad_subfr_loop_end,REG7,REG6)
    // downsample to 4 kHz
    VoC_lw16_d(REG5,Global_ProcessVad_nrSamples);

    VoC_lw16i(REG4,160);
    VoC_push32(REG67,DEFAULT);

    VoC_bne16_rr(agc_processvad_nrSamples_ne_16,REG5,REG4) //if (nrSamples == 160)
    VoC_lw16d_set_inc(REG0,Global_ProcessVad_in_cur_ptr,1);
    VoC_lw16i_set_inc(REG1,Global_ProcessVad_buf1,1);

    VoC_lw16inc_p(ACC0_HI,REG0,DEFAULT);
    VoC_lw16i_short(REG5,2,IN_PARALLEL);

    VoC_lw16inc_p(ACC1_HI,REG0,DEFAULT);
    VoC_lw16i_short(REG4,8,IN_PARALLEL);

    VoC_loop_i(1,8)  //for (k = 0; k < 8; k++)
    VoC_shr32_ri(ACC0,16,DEFAULT);
    VoC_shr32_ri(ACC1,16,IN_PARALLEL);

    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw16inc_p(REG7,REG0,IN_PARALLEL);

    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_lw16inc_p(ACC1_HI,REG0,IN_PARALLEL);

    VoC_movreg16(ACC0_HI,REG7,DEFAULT);

    VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
    VoC_endloop1

    VoC_sub16_rr(REG5,REG0,REG5,DEFAULT);//in +=18-2;

    VoC_lw16i_set_inc(REG0,Global_ProcessVad_buf1,1);
    VoC_jump(agc_processvad_nrSamples_else_end);
agc_processvad_nrSamples_ne_16:
    VoC_lw16_d(REG5,Global_ProcessVad_in_cur_ptr);

    VoC_lw16i_short(REG4,8,DEFAULT);
    VoC_movreg16(REG0,REG5,IN_PARALLEL);

    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);//in +=8;
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
agc_processvad_nrSamples_else_end:
    VoC_lw16i_set_inc(REG1,Global_ProcessVad_buf2,1);
    VoC_sw16_d(REG5,Global_ProcessVad_in_cur_ptr);

    //reg0: in; reg1: out; reg4: len;
    VoC_jal(VOC_Agc_DownsampleBy2);

    VoC_lw16_d(RL6_HI,STRUCT_AgcVadNearend_HPstate_addr);
    VoC_lw16i_set_inc(REG0,Global_ProcessVad_buf2,1);

    VoC_shr32_ri(RL6,16,DEFAULT);//RL6: HPstate (int)
    //RL7: nrg
    VoC_lw16i(REG1,75); //600/1024 = 75/128;

    VoC_loop_i(0,4)
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_shr32_ri(REG45,16,DEFAULT);//buf2[k]
    VoC_lw16i_short(FORMATX,-16,IN_PARALLEL);

    VoC_add32_rr(REG67,REG45,RL6,DEFAULT); //REG67:out = buf2[k] + HPstate;

    //tmp32 = 600 * out;
    //HPstate = (short)((tmp32 >> 10) - buf2[k]);
    VoC_macX_rr(REG7,REG1,DEFAULT);
    VoC_movreg32(REG23,REG67,IN_PARALLEL); //REG23:out

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_short(FORMATX,-8,IN_PARALLEL);

    VoC_shr32_ri(REG67,-8,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_macX_rr(REG7,REG1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);

    VoC_mac32_rr(REG6,REG1,DEFAULT);
    VoC_lw32z(REG67,IN_PARALLEL);

    VoC_bgtz32_r(agc_processvad_abs_out,REG23);
    VoC_sub32_rr(REG23,REG67,REG23,DEFAULT);
agc_processvad_abs_out:
    //REG23: abs(out) 0~0x10000
    VoC_blt32_rd(agc_processvad_out_not_saturation,REG23,Global_ProcessVad_Const_0x0000b504)
    VoC_lw32_d(REG23,Global_ProcessVad_Const_0x0000b504);
agc_processvad_out_not_saturation:
    VoC_shr32_ri(ACC0,7,DEFAULT); //ACC0: (tmp32 >> 10)
    VoC_lw16i_short(FORMAT32,-16,IN_PARALLEL);

    VoC_sub32_rr(RL6,ACC0,REG45,DEFAULT);
    VoC_shr32_ri(REG23,-8,IN_PARALLEL);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_shru32_ri(RL6,-16,IN_PARALLEL);

    VoC_lw16i_short(FORMATX,-9,DEFAULT);
    VoC_shr32_ri(RL6,16,IN_PARALLEL);

    VoC_mac32_rr(REG3,REG3,DEFAULT);
    VoC_shru16_ri(REG2,8,IN_PARALLEL);

    VoC_macX_rr(REG3,REG2,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_mac32_rr(REG2,REG2,DEFAULT);
    VoC_NOP();
    VoC_shr32_ri(ACC0,6,DEFAULT);
    VoC_add32_rr(RL7,RL7,ACC0,DEFAULT);

    VoC_endloop0

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);

    VoC_sw16_d(RL6_LO,STRUCT_AgcVadNearend_HPstate_addr);
    VoC_add16_rr(REG6,REG6,REG4,DEFAULT);

    VoC_jump(agc_processvad_subfr_loop_start)
agc_processvad_subfr_loop_end:

    // find number of leading zeros
    //rl7: nrg
    VoC_jal(VOC_Agc_MATH_CLZ);

    VoC_lw16i_short(REG2,15,DEFAULT);
    VoC_lw16_d(REG3,Global_ProcessVad_kAvgDecayTime);
    VoC_sub16_rr(REG1,REG2,REG0,DEFAULT);

    VoC_shr16_ri(REG1,-11,DEFAULT);//dB = (15 - zeros) << 11;
    VoC_lw16i_short(REG0,1,IN_PARALLEL);

    VoC_bngt16_rd(agc_processvad_counter_nlt_kAvgDecayTime,REG3,STRUCT_AgcVadNearend_counter_addr)
    VoC_add16_rd(REG0,REG0,STRUCT_AgcVadNearend_counter_addr);//state->counter++;
    VoC_NOP();
    VoC_sw16_d(REG0,STRUCT_AgcVadNearend_counter_addr);
agc_processvad_counter_nlt_kAvgDecayTime:
    /*
    VoC_movreg16(ACC0_HI,REG1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_sw16_d(REG1,Global_ProcessVad_dB);

    VoC_shr32_ri(ACC0,16,DEFAULT);//(int)dB

    VoC_mac32_rd(REG2,STRUCT_AgcVadNearend_meanShortTerm_addr);

    VoC_lw32_d(ACC1,STRUCT_AgcVadNearend_varianceShortTerm_addr);

    VoC_lw16i_short(FORMAT32,12,DEFAULT);
    VoC_shr32_ri(ACC1,-4,IN_PARALLEL);

    VoC_sub32_rd(ACC1,ACC1,STRUCT_AgcVadNearend_varianceShortTerm_addr);//state->varianceShortTerm * 15

    VoC_shr32_ri(ACC0,4,DEFAULT);
    VoC_mac32_rr(REG1,REG1,IN_PARALLEL);

    VoC_movreg16(REG0,ACC0_LO,DEFAULT); //state->meanShortTerm = (short)(tmp32 >> 4);
    VoC_sw16_d(ACC0_LO,STRUCT_AgcVadNearend_meanShortTerm_addr);
    */
    //REG1 = dB
    VoC_lw16_d(REG4,STRUCT_AgcVadNearend_NoiseStdShortTermFactor_addr); //REG4 = state->NoiseStdShortTermFactor
    VoC_lw16i_short(REG0,16,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_multf32_rr(ACC1,REG1,REG4,DEFAULT);    //ACC1 = (dB*state->NoiseStdShortTermFactor)
    VoC_sub16_rr(REG0,REG0,REG4,IN_PARALLEL);   //REG0 = (16-state->NoiseStdShortTermFactor)

    VoC_lw16_d(REG3,STRUCT_AgcVadNearend_meanShortTerm_addr);
    VoC_sw16_d(REG1,Global_ProcessVad_dB);

    VoC_lw16i_short(FORMAT32,12,DEFAULT);
    VoC_lw16i_short(FORMATX,0,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG1,REG1,DEFAULT); //ACC0 = (dB * dB) >> 12)
    VoC_macX_rr(REG3,REG0,IN_PARALLEL);

    VoC_jal(VOC_MUL_W32W16ResW32);  //ACC0 = tmp32 = ((dB * dB) >> 12)*state->NoiseStdShortTermFactor;

    VoC_shr32_ri(ACC1,4,DEFAULT); //ACC1 = state->meanShortTerm = (short)(tmp32 >> 4);
    VoC_movreg16(REG4,REG0,IN_PARALLEL);

    VoC_movreg16(REG0,ACC1_LO,DEFAULT);
    VoC_movreg32(ACC1,ACC0,IN_PARALLEL);

    VoC_lw32_d(ACC0,STRUCT_AgcVadNearend_varianceShortTerm_addr);

    VoC_jal(VOC_MUL_W32W16ResW32);  //ACC0 = state->varianceShortTerm * (16-state->NoiseStdShortTermFactor);

    VoC_sw16_d(REG0,STRUCT_AgcVadNearend_meanShortTerm_addr);
    VoC_add32_rr(ACC1,ACC1,ACC0,DEFAULT);

    VoC_shr32_ri(ACC1,4,DEFAULT);//state->varianceShortTerm = tmp32 / 16;
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG0,REG0,DEFAULT);
    VoC_sw32_d(ACC1,STRUCT_AgcVadNearend_varianceShortTerm_addr);
    VoC_shr32_ri(ACC1,-12,DEFAULT);
    //VoC_NOP();
    VoC_sub32_rr(ACC0,ACC1,ACC0,DEFAULT);//tmp32 = (state->varianceShortTerm << 12) - tmp32;

    VoC_jal(VOC_Agc_WebRtcSpl_Sqrt);
    //reg45: return

    VoC_lw16_d(ACC0_HI,Global_ProcessVad_dB);
    VoC_sw16_d(REG4,STRUCT_AgcVadNearend_stdShortTerm_addr);

    VoC_lw16_d(REG4,STRUCT_AgcVadNearend_counter_addr);

    VoC_shr32_ri(ACC0,16,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_mac32_rd(REG4,STRUCT_AgcVadNearend_meanLongTerm_addr);

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);

    //ACC0: int num, reg4:short den,reg5: 0

    VoC_jal(VOC_WebRtcSpl_DivW32W16ResW16);
    //reg0: return

    VoC_sw16_d(REG0,STRUCT_AgcVadNearend_meanLongTerm_addr);

    // update long-term estimate of variance in energy level (Q8)
    //tmp32 = (dB * dB) >> 12;
    //tmp32 += state->varianceLongTerm * state->counter;
    //state->varianceLongTerm = WebRtcSpl_DivW32W16(
    //tmp32, WebRtcSpl_AddSatW16(state->counter, 1));
    VoC_lw16i_short(FORMAT32,12,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_lw16_d(REG1,Global_ProcessVad_dB);
    VoC_lw16_d(REG4,STRUCT_AgcVadNearend_counter_addr);

    VoC_multf32_rr(ACC1,REG1,REG1,DEFAULT);//tmp32 = (dB * dB) >> 12;
    VoC_lw32_d(ACC0,STRUCT_AgcVadNearend_varianceLongTerm_addr);

    VoC_jal(VOC_MUL_W32W16ResW32);//state->varianceLongTerm * state->counter;

    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_jal(VOC_WebRtcSpl_DivW32W16);

    //rl7: return
    VoC_movreg32(ACC0,RL7,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_lw16_d(REG4,STRUCT_AgcVadNearend_meanLongTerm_addr);

    VoC_shr32_ri(ACC0,-12,DEFAULT);
    VoC_sw32_d(RL7,STRUCT_AgcVadNearend_varianceLongTerm_addr);

    VoC_msu32_rr(REG4,REG4,DEFAULT);

    VoC_jal(VOC_Agc_WebRtcSpl_Sqrt);
    //reg45: return

    VoC_sw16_d(REG4,STRUCT_AgcVadNearend_stdLongTerm_addr);

    VoC_lw16_d(REG0,Global_ProcessVad_dB);

    VoC_lw16i_short(REG1,3,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_sub16_rd(REG0,REG0,STRUCT_AgcVadNearend_meanLongTerm_addr);

    VoC_shr16_ri(REG1,-12,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG1,REG0,DEFAULT);

    //acc0: tmp32; reg45: state->stdLongTerm
    VoC_jal(VOC_WebRtcSpl_DivW32W16);

    //rl7: return

    //tmp32b = WEBRTC_SPL_MUL_16_U16(state->logRatio, tmpU16);

    VoC_lw16i_short(REG4,13,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_lw16i_short(FORMAT32,-8,DEFAULT);
    VoC_shru32_ri(REG45,-20,IN_PARALLEL);

    VoC_multf32_rd(ACC0,REG5,STRUCT_AgcVadNearend_logRatio_addr);

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_shr32_ri(ACC0,10,DEFAULT);
    VoC_shr16_ri(REG6,-11,IN_PARALLEL); //reg6: 2048

    VoC_add32_rr(REG01,ACC0,RL7,DEFAULT);//tmp32 += tmp32b >> 10;
    VoC_lw32z(REG45,IN_PARALLEL);

    VoC_shr32_ri(REG01,6,DEFAULT);//state->logRatio = (short)(tmp32 >> 6);
    VoC_sub32_rr(REG45,REG45,REG67,IN_PARALLEL);//reg4: -2048

    VoC_bngt16_rr(agc_processvad_logRatio_ngt_2048,REG0,REG6)
    VoC_movreg32(REG01,REG67,DEFAULT);
agc_processvad_logRatio_ngt_2048:

    VoC_bgt16_rr(agc_processvad_logRatio_gt_neg2048,REG0,REG4)
    VoC_movreg32(REG01,REG45,DEFAULT);
    VoC_NOP();
agc_processvad_logRatio_gt_neg2048:

    VoC_sw16_d(REG0,STRUCT_AgcVadNearend_logRatio_addr);

agc_processvad_exit:

    exit_on_warnings =OFF;

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

/******************************************
function: WebRtcSpl_DownsampleBy2
input:
    reg0: *in
    reg4: len
    reg1: *out
    state->downState :filtState

note: RL7 must be in stack

*******************************************/
void VOC_Agc_DownsampleBy2(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);

    VoC_push32(RL7,DEFAULT);
    VoC_shr16_ri(REG4,1,IN_PARALLEL);//len>>1

    VoC_lw16i_set_inc(REG2,Global_DownsampleBy2_kResampleAllpass2,2);

    VoC_lw16i(RL6_LO,1024); //RL6:1024

    VoC_loop_r(0,REG4) //for (i = (len >> 1); i > 0; i--){}

    VoC_lw32z(RL7,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);

    // lower allpass filter
    VoC_lw16inc_p(RL7_HI,REG0,DEFAULT);

    VoC_lw32inc_p(REG45,REG2,DEFAULT);//kResampleAllpass2[0]
    VoC_shr32_ri(RL7,6,IN_PARALLEL);//in32

    VoC_sub32_rd(REG67,RL7,STRUCT_AgcVadNearend_downState_addr+2);//diff = in32 - state1;

    VoC_jal(MUL_ACCUM_FUN);

    VoC_add32_rd(REG67,ACC0,STRUCT_AgcVadNearend_downState_addr+0); //tmp1
    VoC_sw32_d(RL7,STRUCT_AgcVadNearend_downState_addr+0);//state0 = in32;

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_movreg32(RL7,REG67,IN_PARALLEL);

    VoC_sub32_rd(REG67,REG67,STRUCT_AgcVadNearend_downState_addr+4);
    VoC_jal(MUL_ACCUM_FUN);

    VoC_add32_rd(REG67,ACC0,STRUCT_AgcVadNearend_downState_addr+2);
    VoC_sw32_d(RL7,STRUCT_AgcVadNearend_downState_addr+2);

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_movreg32(RL7,REG67,IN_PARALLEL);

    VoC_sub32_rd(REG67,REG67,STRUCT_AgcVadNearend_downState_addr+6);
    VoC_jal(MUL_ACCUM_FUN);

    VoC_add32_rd(REG67,ACC0,STRUCT_AgcVadNearend_downState_addr+4);
    VoC_sw32_d(RL7,STRUCT_AgcVadNearend_downState_addr+4);

    VoC_lw32z(RL7,DEFAULT);
    VoC_sw32_d(REG67,STRUCT_AgcVadNearend_downState_addr+6);

    //upper allpass filter
    VoC_lw16inc_p(RL7_HI,REG0,DEFAULT);

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_shr32_ri(RL7,6,IN_PARALLEL);

    VoC_sub32_rd(REG67,RL7,STRUCT_AgcVadNearend_downState_addr+10);
    VoC_jal(MUL_ACCUM_FUN);

    VoC_add32_rd(REG67,ACC0,STRUCT_AgcVadNearend_downState_addr+8);
    VoC_sw32_d(RL7,STRUCT_AgcVadNearend_downState_addr+8);

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_movreg32(RL7,REG67,IN_PARALLEL);

    VoC_sub32_rd(REG67,REG67,STRUCT_AgcVadNearend_downState_addr+12);
    VoC_jal(MUL_ACCUM_FUN);

    VoC_add32_rd(REG67,ACC0,STRUCT_AgcVadNearend_downState_addr+10);
    VoC_sw32_d(RL7,STRUCT_AgcVadNearend_downState_addr+10);

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_movreg32(RL7,REG67,IN_PARALLEL);

    VoC_sub32_rd(REG67,REG67,STRUCT_AgcVadNearend_downState_addr+14);
    VoC_jal(MUL_ACCUM_FUN);

    VoC_add32_rd(REG67,ACC0,STRUCT_AgcVadNearend_downState_addr+12);
    VoC_sw32_d(RL7,STRUCT_AgcVadNearend_downState_addr+12);
    VoC_sw32_d(REG67,STRUCT_AgcVadNearend_downState_addr+14);

    VoC_add32_rd(REG67,REG67,STRUCT_AgcVadNearend_downState_addr+6); //state3 + state7

    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);
    VoC_pop16(REG2,IN_PARALLEL);

    VoC_shr32_ri(REG67,11,DEFAULT);//out32 = (state3 + state7 + 1024) >> 11;

    VoC_lw32_d(ACC0,Global_ProcessVad_Const_0x00007fff);
    VoC_bgt32_rr(SatW32ToW16_max_or_min,REG67,ACC0)
    VoC_lw32_d(ACC0,Global_ProcessVad_Const_0xffff8000);
    VoC_bgt32_rr(SatW32ToW16_max_or_min,ACC0,REG67)
    VoC_movreg32(ACC0,REG67,DEFAULT);
    VoC_NOP();
SatW32ToW16_max_or_min:
    VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);

    VoC_endloop0

    VoC_pop32(RL7,DEFAULT);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

/***************************************
function: WEBRTC_SPL_SCALEDIFF32(a, b, 0)
input:
    reg45: a; unsigned short
    reg67: b; int
           c =0;
    acc0: return
***************************************/
void MUL_ACCUM_FUN(void)
{
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_short(FORMATX,-8,IN_PARALLEL);

    VoC_shr32_ri(REG45,-8,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_macX_rr(REG7,REG5,DEFAULT);
    VoC_shru16_ri(REG4,8,IN_PARALLEL);

    VoC_mac32_rr(REG7,REG4,DEFAULT);// acc0: (B >> 16) * A
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_shr32_ri(REG67,-8,DEFAULT);

    VoC_mac32_rr(REG7,REG5,DEFAULT);
    VoC_shru16_ri(REG6,8,DEFAULT);

    VoC_movreg16(REG4,REG5,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_movreg32(ACC1,ACC0,IN_PARALLEL);

    VoC_mac32_rr(REG6,REG5,DEFAULT);

    VoC_bimacX_rr(REG67,REG45);

    VoC_NOP();

    VoC_shr32_ri(ACC0,16,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);

    VoC_return;
}

/**************************************
function: WebRtcSpl_Sqrt(|value|)
input:
    acc0: value ;int

    REG45: return A
used: all regs
add by quanwz 20160406
***************************************/
void VOC_Agc_WebRtcSpl_Sqrt(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw32z(ACC1,DEFAULT);
    VoC_movreg32(REG45,ACC0,IN_PARALLEL); //REG45: sqrt(0) = 0

    VoC_bez32_r(webrtcspl_sqrt_return,ACC0)
    VoC_bgtz32_r(webrtcspl_sqrt_A_gtz,ACC0)
    VoC_sub32_rr(ACC0,ACC1,ACC0,DEFAULT);
webrtcspl_sqrt_A_gtz:
    VoC_jal(VOC_WebRtcSpl_NormW32);
    //reg0: sh; acc0: A

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i(REG7,23170);
    VoC_sw16_d(REG0,Global_Sqrt_sh);
    VoC_sw16_d(REG7,Global_Sqrt_k_sqrt_2);// 1/sqrt2 (==5a82)

    VoC_sub16_rr(REG0,REG5,REG0,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_lw32z(RL6,DEFAULT);
    VoC_shru32_rr(ACC0,REG0,IN_PARALLEL);//A = WEBRTC_SPL_LSHIFT_W32(A, sh);

    VoC_lw32_d(ACC1,Global_ProcessVad_Const_WORD32_MAX);
    VoC_add32_rd(ACC0,ACC0,Global_ProcessVad_Const_0x00007fff);

    VoC_bngt32_rr(webrtcspl_sqrt_A_EQ_MAX,ACC1,ACC0)
    VoC_add32_rr(ACC1,ACC0,REG45,DEFAULT);//A = A + ((int)32768);
webrtcspl_sqrt_A_EQ_MAX:
    //ACC1: A
    VoC_shr32_ri(ACC1,16,DEFAULT);//x_norm = (short)(A >> 16);

    VoC_shru32_ri(ACC1,-16,DEFAULT);
    VoC_bgtz32_r(webrtcspl_sqrt_ABS_A,ACC1)//A = abs(x_norm<<16)
    VoC_sub32_rr(ACC1,RL6,ACC1,DEFAULT);
webrtcspl_sqrt_ABS_A:

    VoC_jal(VOC_WebRtcSpl_SqrtLocal);
    //ACC1: A
    VoC_movreg32(REG45,ACC1,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL);

    VoC_lw16_d(REG0,Global_Sqrt_sh);

    VoC_and16_rr(REG0,REG1,DEFAULT);
    VoC_shr32_ri(REG45,16,IN_PARALLEL);

    VoC_bnez16_r(webrtcspl_sqrt_sh_odd,REG0)
    //Even shift value case
    VoC_lw32z(ACC0,DEFAULT);
    VoC_movreg16(REG7,REG4,IN_PARALLEL); //t16

    VoC_lw16i_short(ACC0_LO,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);

    VoC_multf32_rd(REG45,REG7,Global_Sqrt_k_sqrt_2);

    VoC_shr32_ri(ACC0,-15,DEFAULT);
    VoC_lw16i(REG6,0x7fff);

    VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);

    VoC_and16_rr(REG5,REG6,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);

    VoC_shr32_ri(REG45,15,DEFAULT);
webrtcspl_sqrt_sh_odd:
    VoC_lw16i_short(REG0,-1,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_lw16_d(REG1,Global_Sqrt_sh);

    VoC_and16_rr(REG4,REG0,DEFAULT);
    VoC_shr16_ri(REG1,1,IN_PARALLEL);

    VoC_shr32_rr(REG45,REG1,DEFAULT);//A >>= nshift;

webrtcspl_sqrt_return:
    //REG45: return
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


/****************************************
function: WebRtcSpl_NormW32
input:
    ACC0: a; int

  reg0:  return zeros;
*****************************************/
void VOC_WebRtcSpl_NormW32(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_push32(ACC0,DEFAULT);//save a

    VoC_movreg32(RL7,ACC0,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);

    VoC_lw16i_short(REG6,-1,DEFAULT);
    VoC_lw16i_short(REG7,-1,IN_PARALLEL);

    VoC_bez32_r(webrtcspl_normw32_return,ACC0)
    VoC_bgtz32_r(webrtcspl_normw32_positive_a,ACC0)
    VoC_xor32_rr(RL7,REG67,DEFAULT);//a = ~a;
webrtcspl_normw32_positive_a:
    VoC_jal(VOC_Agc_MATH_CLZ);
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);
webrtcspl_normw32_return:
    VoC_pop32(ACC0,DEFAULT);
    //reg0: zeros
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


/**********************************************
function: WebRtcSpl_SqrtLocal
input:
    ACC1: in; int

  ACC1: return
***********************************************/
void VOC_WebRtcSpl_SqrtLocal(void)
{
    VoC_lw16i(RL7_HI,0x4000);

    VoC_lw16i_short(RL7_LO,0,DEFAULT);
    VoC_shr32_ri(ACC1,1,IN_PARALLEL); //B = in / 2;

    VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);//B = B - ((int)0x40000000);
    VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);

    VoC_movreg32(REG67,ACC1,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,RL7,IN_PARALLEL);

    VoC_shr32_ri(REG67,16,DEFAULT);//x_half = (short)(B >> 16);
    VoC_add32_rr(ACC1,ACC1,RL7,IN_PARALLEL);

    //ACC1: B; REG67: x_half

    VoC_multf32_rr(REG45,REG6,REG6,DEFAULT);//x2 = ((int)x_half) * ((int)x_half) * 2;
    VoC_lw32z(RL6,IN_PARALLEL);

    VoC_NOP();

    VoC_sub32_rr(ACC0,RL6,REG45,DEFAULT);//A = -x2;

    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_movreg32(REG23,ACC0,IN_PARALLEL);

    VoC_add32_rr(ACC1,ACC1,ACC0,DEFAULT);//B = B + (A >> 1);
    VoC_shr32_ri(REG23,16,IN_PARALLEL);//A >>= 16;

    //ACC1:B; REG23: A

    VoC_multf32_rr(ACC0,REG2,REG2,DEFAULT);//A = A * A * 2;

    VoC_lw16i(REG3,-20480);
    VoC_movreg16(REG2,ACC0_HI,DEFAULT); //t16 = (short)(A >> 16);

    VoC_multf32_rr(ACC0,REG3,REG2,DEFAULT);//WEBRTC_SPL_MUL_16_16(-20480, t16) * 2;
    VoC_multf32_rr(RL6,REG6,REG2,DEFAULT);//A = WEBRTC_SPL_MUL_16_16(x_half, t16) * 2;

    //RL6:A

    VoC_lw16i(REG3,28672);
    VoC_add32_rr(ACC1,ACC1,ACC0,DEFAULT);
    VoC_movreg16(REG2,RL6_HI,IN_PARALLEL);

    //ACC1: B
    VoC_multf32_rr(ACC0,REG3,REG2,DEFAULT);//WEBRTC_SPL_MUL_16_16(28672, t16) * 2;
    //t16 = (short)(x2 >> 16);
    //A = WEBRTC_SPL_MUL_16_16(x_half, t16) * 2;
    VoC_multf32_rr(RL6,REG6,REG5,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_add32_rr(ACC1,ACC1,ACC0,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    VoC_shr32_ri(RL6,1,DEFAULT);
    VoC_shr32_ri(REG23,-15,IN_PARALLEL);//reg23: 32768

    VoC_add32_rr(ACC1,ACC1,RL6,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,REG23,DEFAULT);

    VoC_return;
}



/**************************
function:
 input: RL7: ;int

        REG0: zeros; short
    not use REG6
***************************/
//find number of leading zeros
void VOC_Agc_MATH_CLZ(void)
{
    VoC_lw32z(RL6,DEFAULT);
    VoC_movreg32(ACC0,RL7,IN_PARALLEL);

    VoC_lw16i_short(RL6_HI,-1,DEFAULT);
    VoC_lw16i_short(REG1,-16,IN_PARALLEL);

    VoC_and32_rr(ACC0,RL6,DEFAULT);
    VoC_movreg16(REG0,REG1,IN_PARALLEL);//zeros = 16;

    VoC_bez32_r(math_clz_zero_eq_16,ACC0)
    VoC_lw16i_short(REG0,0,DEFAULT);// zeros = 0;
math_clz_zero_eq_16:

    VoC_shru32_ri(RL6,-8,DEFAULT);
    VoC_movreg32(ACC0,RL7,IN_PARALLEL);

    VoC_shru32_rr(ACC0,REG0,DEFAULT);
    VoC_shr16_ri(REG1,1,IN_PARALLEL);

    VoC_and32_rr(ACC0,RL6,DEFAULT);
    VoC_bnez32_r(math_clz_zero_no_add_8,ACC0)
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);
math_clz_zero_no_add_8:

    VoC_shru32_ri(RL6,-4,DEFAULT);
    VoC_movreg32(ACC0,RL7,IN_PARALLEL);

    VoC_shru32_rr(ACC0,REG0,DEFAULT);
    VoC_shr16_ri(REG1,1,IN_PARALLEL);

    VoC_and32_rr(ACC0,RL6,DEFAULT);
    VoC_bnez32_r(math_clz_zero_no_add_4,ACC0)
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);
math_clz_zero_no_add_4:

    VoC_shru32_ri(RL6,-2,DEFAULT);
    VoC_movreg32(ACC0,RL7,IN_PARALLEL);

    VoC_shru32_rr(ACC0,REG0,DEFAULT);
    VoC_shr16_ri(REG1,1,IN_PARALLEL);

    VoC_and32_rr(ACC0,RL6,DEFAULT);
    VoC_bnez32_r(math_clz_zero_no_add_2,ACC0)
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);
math_clz_zero_no_add_2:

    VoC_shru32_ri(RL6,-1,DEFAULT);
    VoC_movreg32(ACC0,RL7,IN_PARALLEL);

    VoC_shru32_rr(ACC0,REG0,DEFAULT);
    VoC_shr16_ri(REG1,1,IN_PARALLEL);

    VoC_and32_rr(ACC0,RL6,DEFAULT);
    VoC_bnez32_r(math_clz_zero_no_add_1,ACC0)
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);
math_clz_zero_no_add_1:
    VoC_abs16_r(REG0,DEFAULT);
    VoC_return;
}



/****************************************
function: WebRtcSpl_DivW32W16
input:
    acc0: num; int
    reg45: den; short

    rl7: return
*****************************************/

void VOC_WebRtcSpl_DivW32W16(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw32z(REG23,DEFAULT)
    VoC_lw16i_short(RL7_LO,-1,IN_PARALLEL);

    VoC_lw16i(RL7_HI,0x7fff);//REG01:0x7fffffff;

    VoC_bez16_r(divw32w16_return,REG4)
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);
    VoC_bnltz32_r(divw32w16_num_positive,ACC0)
    VoC_xor16_rr(REG6,REG7,DEFAULT);
    VoC_sub32_rr(ACC0,REG23,ACC0,IN_PARALLEL);
divw32w16_num_positive:

    VoC_bgtz16_r(divw32w16_den_positive,REG4)
    VoC_xor16_rr(REG6,REG7,DEFAULT);
    VoC_sub16_rr(REG4,REG2,REG4,IN_PARALLEL);
divw32w16_den_positive:

    VoC_push16(REG6,DEFAULT);//sign flag

    VoC_jal(VOC_MATH_POSDIV32_S);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_pop16(REG6,IN_PARALLEL);

    VoC_bez16_r(divw32w16_return,REG6)
    VoC_sub32_rr(RL7,ACC0,RL7,DEFAULT);
divw32w16_return:
    //rl7: return
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


/***********************************************
function: WebRtcSpl_DivW32W16ResW16
input:
    ACC0: int num;
    REG45: short den;

  REG0: return
************************************************/

void VOC_WebRtcSpl_DivW32W16ResW16(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw32z(REG23,IN_PARALLEL);

    VoC_lw16i(REG0,0x7fff);

    VoC_bez16_r(divw32w16resw16_return,REG4);//if(den!=0)
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);

    VoC_bnltz32_r(divw32w16resw16_num_positive,ACC0)
    VoC_xor16_rr(REG6,REG7,DEFAULT);
    VoC_sub32_rr(ACC0,REG23,ACC0,IN_PARALLEL);
divw32w16resw16_num_positive:

    VoC_bgtz16_r(divw32w16resw16_den_positive,REG4)
    VoC_xor16_rr(REG6,REG7,DEFAULT);
    VoC_sub16_rr(REG4,REG2,REG4,IN_PARALLEL);
divw32w16resw16_den_positive:

    VoC_push16(REG6,DEFAULT);
    //ACC0: abs(num);REG4: abs(den)
    VoC_jal(VOC_MATH_POSDIV32_S);

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i(REG6,0x7fff);

    VoC_bngt32_rr(divw32w16resw16_res_saturation,RL7,REG67)
    VoC_movreg32(RL7,REG67,DEFAULT);
divw32w16resw16_res_saturation:

    VoC_lw32z(ACC0,DEFAULT);
    VoC_pop16(REG6,IN_PARALLEL);

    VoC_bez16_r(divw32w16resw16_res_positive,REG6)
    VoC_sub32_rr(RL7,ACC0,RL7,DEFAULT);
divw32w16resw16_res_positive:
    VoC_movreg16(REG0,RL7_LO,DEFAULT);
divw32w16resw16_return:
    //REG0: return

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

/******************************************
FUNCTION: MATH_Divide(int a, short b)
      a,b must be positive, b!=0
input:
    ACC0: a; int
    REG45: b; short

  RL7: return
*******************************************/

void VOC_MATH_POSDIV32_S(void)
{
    VoC_lw16i_short(RL6_HI,0,DEFAULT);
    VoC_lw16i_short(RL6_LO,1,IN_PARALLEL);

    VoC_movreg32(RL7,ACC0,DEFAULT);
    VoC_be32_rr(agc_posdiv32_end,REG45,RL6)//if(divisor==1)
    //RL6: bit_position = 1;
agc_posdiv32_while1_start:
    VoC_bngt32_rr(agc_posdiv32_while1_end,ACC0,REG45)
    VoC_bltz32_r(agc_posdiv32_while1_end,REG45)
    VoC_shru32_ri(REG45,-1,DEFAULT);
    VoC_shru32_ri(RL6,-1,IN_PARALLEL);
    VoC_jump(agc_posdiv32_while1_start);
agc_posdiv32_while1_end:

    VoC_lw32z(RL7,DEFAULT);
    VoC_bgtz32_r(agc_posdiv32_while2_start,REG45)
    VoC_shru32_ri(REG45,1,DEFAULT);
    VoC_shru32_ri(RL6,1,IN_PARALLEL);

agc_posdiv32_while2_start:
    VoC_bngtz32_r(agc_posdiv32_end,RL6)
    VoC_bgt32_rr(agc_posdiv32_dividend_ngt_divisor,REG45,ACC0)
    VoC_sub32_rr(ACC0,ACC0,REG45,DEFAULT);
    VoC_add32_rr(RL7,RL7,RL6,IN_PARALLEL);
agc_posdiv32_dividend_ngt_divisor:
    VoC_shru32_ri(REG45,1,DEFAULT);
    VoC_shru32_ri(RL6,1,IN_PARALLEL);
    VoC_jump(agc_posdiv32_while2_start);
agc_posdiv32_end:

    VoC_NOP();
    VoC_return;
}


/***********************************************
function: MULT(int a, short b)
input:
    ACC0: a; int
    REG4: b; short

  return: ACC0;int
NOTE: use REG4,REG6,REG7,ACC0
************************************************/
void VOC_MUL_W32W16ResW32(void)
{
    VoC_lw16i_short(FORMATX,-16,DEFAULT);
    VoC_movreg32(REG67,ACC0,IN_PARALLEL);

    VoC_lw32z(ACC0,DEFAULT);

    VoC_macX_rr(REG7,REG4,DEFAULT);

    VoC_lw16i_short(FORMAT32,-8,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_shru32_ri(REG67,-8,DEFAULT);

    VoC_mac32_rr(REG7,REG4,DEFAULT);

    VoC_shru16_ri(REG6,8,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_mac32_rr(REG6,REG4,DEFAULT);
    VoC_NOP();
    VoC_return;
}









/**************************************************************************************
 * Function:    Agc_ProcessDigital
 *
 * Description:
 *
 * Inputs:
              FS = sample_rate = 8000
              logratio = -237
              buf = {-104, -168, -216, -240, -240, -200, -136, -64};
              DigitalAgcInst:
                        capacitorSlow = 134217728;
                        capacitorFast = 0;
                        gain = 65536
                        gatePrevious = 0
              DigitalAgcInst->vadNearend:
                        downState =
                         {-139264,
                          -226985,
                          -240363,
                          -243917,
                          -65536,
                          -195717,
                          -227453,
                          -254238};
                        HPstate = 163;
                        counter = 4;
                        logRatio = -237;
                        meanLongTerm = 9420;
                        varianceLongTerm = 112435;
                        stdLongTerm = 19282;
                        meanShortTerm = 13504;
                        varianceShortTerm = 123136;
                        stdShortTerm = 17947;

 *
 * Outputs:
                  buf = {-104, -187, -263, -318, -343, -308, -224, -113};
                  DigitalAgcInst:
                            capacitorSlow = 134217728;
                            capacitorFast = 57600;
                            gain = 121744
                            gatePrevious = 0

 *
 * Used : All.
 *
 * Version 1.0  Create by  sixu       //2016
 **************************************************************************************/

void Agc_ProcessDigital(void)
{


    VoC_push16(RA,DEFAULT);

    //initial registers
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,DEFAULT);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,DEFAULT);
    VoC_sw16_d(REG0,CFG_DMA_WRAP);
    VoC_lw16i(STATUS,STATUS_CLR);
    VoC_lw16i_short(BITCACHE,0,DEFAULT);

    VoC_lw16i_short(FORMAT32,0,DEFAULT);

    VoC_lw16_d(REG0,FS_addr);
    VoC_bne16_rd(Agc_ProcessDigital_L0,REG0,agc_const_8000_addr);
    VoC_lw16i_short(REG4,8,DEFAULT);                              //REG4 = L = 8;
    VoC_lw16i_short(REG5,-3,IN_PARALLEL);                          //REG5 = L2 = 3;
    VoC_jump(Agc_ProcessDigital_L2);
Agc_ProcessDigital_L0:
    VoC_bne16_rd(Agc_ProcessDigital_L1,REG0,agc_const_16000_addr);
    VoC_lw16i_short(REG4,16,DEFAULT);
    VoC_lw16i_short(REG5,-4,IN_PARALLEL);
    VoC_jump(Agc_ProcessDigital_L2);
Agc_ProcessDigital_L1:
    VoC_lw16i_short(REG0,-1,DEFAULT); //return -1;   //maybe should add return value later
    VoC_jump(Agc_ProcessDigital_L28);

Agc_ProcessDigital_L2:
    VoC_sw16_d(REG4,APD_LOCAL_X_L_addr);
    VoC_sw16_d(REG5,APD_LOCAL_X_L2_addr);

    VoC_jal(Agc_ProcessVad);
    VoC_lw16_d(REG4,APD_LOCAL_X_L_addr);
    VoC_lw16_d(REG5,APD_LOCAL_X_L2_addr);

    VoC_lw16_d(REG3,STRUCT_AgcVadNearend_logRatio_addr);            //REG3 = logRatio
    VoC_bngt16_rd(Agc_ProcessDigital_L3,REG3,STRUCT_DigitalAgc_upper_thr_addr);       //if (logratio > upper_thr)
    VoC_lw16_d(REG6,STRUCT_DigitalAgc_decay_value_addr);                                          //REG6 = decay = -65;
    VoC_jump(Agc_ProcessDigital_L5);
Agc_ProcessDigital_L3:
    VoC_bnlt16_rd(Agc_ProcessDigital_L4,REG3,STRUCT_DigitalAgc_lower_thr_addr);            //else if (logratio < lower_thr)
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_jump(Agc_ProcessDigital_L5);
Agc_ProcessDigital_L4:                                            //else
    VoC_sub16_dr(REG1,STRUCT_DigitalAgc_lower_thr_addr,REG3);
    VoC_lw16_d(REG2,STRUCT_DigitalAgc_decay_value_addr);
    VoC_lw16i_short(FORMAT32,10,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_sub16_rr(REG2,REG7,REG2,DEFAULT);                         //-STRUCT_DigitalAgc_decay_value_addr
    VoC_multf32_rr(REG67,REG1,REG2,DEFAULT);                              //REG67 = tmp32 =  WEBRTC_SPL_MUL_16_16((lower_thr - logratio), 65);

    /*
    REG0: FS_addr, dead
    REG1: lower_thr - logratio, no use
    REG2: 65, dead
    REG3: logRatio, dead
    REG4: L
    REG5: L2
    REG6: tmp32, together with REG7, in fact is decay
    REG7:
    ACC0:
    ACC1:
    RL6:
    RL7:
    */


Agc_ProcessDigital_L5:
    //VoC_sw16_d(REG6,APD_LOCAL_X_decay_addr);
    VoC_lw16_d(REG0,STRUCT_AgcVadNearend_stdLongTerm_addr);
    VoC_lw16i(REG3,4000);
    VoC_lw16i(REG2,8096);
    VoC_bngt16_rr(Agc_ProcessDigital_L6,REG3,REG0);
    VoC_lw16i_short(REG6,0,DEFAULT);
Agc_ProcessDigital_L6:
    VoC_bngt16_rr(Agc_ProcessDigital_L7,REG2,REG0);
    VoC_sub16_rr(REG0,REG0,REG3,DEFAULT);
    VoC_lw16i_short(FORMAT32,12,IN_PARALLEL);
    VoC_multf32_rr(REG67,REG0,REG6,DEFAULT);                      //REG67 = tmp32 = WEBRTC_SPL_MUL_16_16((stt->vadNearend.stdLongTerm - 4000), decay);
    VoC_NOP();
    VoC_NOP();
Agc_ProcessDigital_L7:
    VoC_sw16_d(REG6,APD_LOCAL_X_decay_addr);

    /*
    REG0: (stt->vadNearend.stdLongTerm - 4000)
    REG1: lower_thr - logratio, no use
    REG2: 8096
    REG3: 4000
    REG4: L
    REG5: L2
    REG6: tmp32, together with REG7, in fact is decay
    REG7:
    ACC0:
    ACC1:
    RL6:
    RL7:
    */
    //dead: REG0,3,2,7, and maybe REG1

    VoC_lw16i_short(FORMAT32,0,DEFAULT);
//  VoC_lw16i_set_inc(REG0,0,1)                              //REG0 = k
    VoC_lw16d_set_inc(REG3,buf_addr_addr,1);
    VoC_lw16_d(REG7,CONST_SubFrameNum_addr);
    VoC_lw16i_set_inc(REG2,APD_LOCAL_X_env_addr,2);
    VoC_loop_r(1,REG7);                                      //for (k = 0; k < SubFrameNum; k++)
    VoC_lw32z(RL7,DEFAULT);                              //RL7 = max_nrg = 0;
    //VoC_movreg16(REG1,REG0,IN_PARALLEL);
    //VoC_shr16_rr(REG1,REG5,DEFAULT);                       //REG1 = k * L = k << L2;
    //VoC_add16_rr(REG3,REG3,REG1,DEFAULT);
    VoC_loop_r_short(REG4,DEFAULT);                        //for (n = 0; n < L; n++)
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG3,DEFAULT);
    VoC_multf32_rr(ACC0,REG7,REG7,DEFAULT);              //ACC0 = nrg = WEBRTC_SPL_MUL_16_16(out[k * L + n], out[k * L + n]);
    VoC_NOP();
    VoC_bngt32_rr(Agc_ProcessDigital_L8,ACC0,RL7);
    VoC_movreg32(RL7,ACC0,DEFAULT);
Agc_ProcessDigital_L8:
    VoC_NOP();
    VoC_endloop0
    VoC_sw32inc_p(RL7,REG2,DEFAULT);
    //VoC_inc_p(REG0,IN_PARALLEL);
    VoC_endloop1

    /*
    REG0: k
    REG1: k * L
    REG2: &env[k]
    REG3: &buf[k * L + n]
    REG4: L
    REG5: L2
    REG6: tmp32, together with REG7, in fact is decay
    REG7: out[k * L + n]
    ACC0: nrg
    ACC1:
    RL6:
    RL7:  max_nrg
    */
    //dead: REG0,1,2,3,7
    //maybe useful REG4,5,6
    VoC_lw16i_set_inc(REG0,APD_LOCAL_X_gains_addr,2);
    VoC_lw32_d(RL7,STRUCT_DigitalAgc_gain_addr);
    VoC_lw16_d(REG7,CONST_SubFrameNum_addr);
    VoC_lw16i_set_inc(REG1,APD_LOCAL_X_env_addr,2);
    VoC_sw32inc_p(RL7,REG0,DEFAULT);
    VoC_lw16i_short(FORMATX,-1,IN_PARALLEL);
    VoC_loop_r(1,REG7);
    VoC_lw32_d(REG23,STRUCT_DigitalAgc_capacitorFast_addr);
    VoC_lw16_d(REG4,STRUCT_DigitalAgc_capacitorFastFactor_addr);                                         //if define a const, save the loading imme time, and save registers too
    VoC_movreg32(ACC0,REG23,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_movreg16(REG7,REG2,DEFAULT);
    VoC_shru16_ri(REG2,1,IN_PARALLEL);
    VoC_lw32z(ACC1,DEFAULT);
    VoC_and16_ri(REG7,0x0001);
    VoC_bez16_r(Agc_ProcessDigital_MulU0,REG7);
    VoC_movreg16(ACC1_LO,REG4,DEFAULT);
    VoC_bnltz16_r(Agc_ProcessDigital_MulU0,REG4);
    VoC_lw16i_short(ACC1_HI,-1,DEFAULT);
Agc_ProcessDigital_MulU0:
    VoC_mac32_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG2,REG4,IN_PARALLEL);
    VoC_lw32inc_p(RL7,REG1,DEFAULT);                                      //RL7 = env[k]
    VoC_shr32_ri(ACC1,16,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_bngt32_rr(Agc_ProcessDigital_L9,RL7,ACC0);
    VoC_movreg32(ACC0,RL7,DEFAULT);
    VoC_NOP();

    /*
    REG0: &gain[k]
    REG1: &env[k]
    REG2: capacitorFast_lo>>1
    REG3: capacitorFast_hi
    REG4: -1000
    REG5: L2
    REG6: decay
    REG7: capacitorFast_lo&1
    ACC0: capacitorFast_new
    ACC1: never mind
    RL6:
    RL7:  env[k]
    */
    //can be used: REG2,3,4,7,ACC1,RL6

Agc_ProcessDigital_L9:
    VoC_sw32_d(ACC0,STRUCT_DigitalAgc_capacitorFast_addr);
    VoC_lw32_d(ACC0,STRUCT_DigitalAgc_capacitorSlow_addr);
    VoC_bngt32_rr(Agc_ProcessDigital_L10,RL7,ACC0);
    VoC_lw16_d(REG4,STRUCT_DigitalAgc_capacitorSlowFactor_addr);                                         //if define a const, save the loading imme time, and save registers too,maybe no
    VoC_sub32_rr(REG23,RL7,ACC0,DEFAULT);
    VoC_jump(Agc_ProcessDigital_L11);
Agc_ProcessDigital_L10:
    VoC_lw16_d(REG4,APD_LOCAL_X_decay_addr);                             //REG4 = decay
    VoC_movreg32(REG23,ACC0,DEFAULT);
Agc_ProcessDigital_L11:
    VoC_movreg16(REG7,REG2,DEFAULT);
    VoC_shru16_ri(REG2,1,IN_PARALLEL);
    VoC_and16_ri(REG7,0x0001);
    VoC_lw32z(ACC1,DEFAULT);
    VoC_bez16_r(Agc_ProcessDigital_MulU1,REG7);
    VoC_movreg16(ACC1_LO,REG4,DEFAULT);
    VoC_bnltz16_r(Agc_ProcessDigital_MulU1,REG4);
    VoC_lw16i_short(ACC1_HI,-1,DEFAULT);
Agc_ProcessDigital_MulU1:
    VoC_mac32_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG2,REG4,IN_PARALLEL);
    VoC_NOP();
    VoC_shr32_ri(ACC1,16,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_NOP();
    VoC_sw32_d(ACC0,STRUCT_DigitalAgc_capacitorSlow_addr);
    VoC_bnlt32_rd(Agc_ProcessDigital_L12,ACC0,STRUCT_DigitalAgc_capacitorFast_addr);
    VoC_lw32_d(ACC0,STRUCT_DigitalAgc_capacitorFast_addr);                              //ACC0 = cur_level
Agc_ProcessDigital_L12:
    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_jal(Coolsand_CLZU);                                         //REG2 = zeros = WebRtcSpl_NormU32((unsigned int)cur_level);

    /*
    REG0: &gain[k]
    REG1: &env[k]
    REG2: zeros
    REG3: never mind
    REG4: never mind
    REG5: L2
    REG6: never mind
    REG7: never mind
    ACC0: cur_level
    ACC1: never mind
    RL6:
    RL7:
    */
    //can be used: REG3,4,7, ACC1,RL6,RL7, REG6 better not use
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_movreg32(REG45,ACC0,IN_PARALLEL);
    VoC_sub16_rr(REG3,REG3,REG2,DEFAULT);
    VoC_lw16i(REG7,0x7fff);
    VoC_shru32_rr(REG45,REG3,DEFAULT);
    VoC_and16_rr(REG5,REG7,DEFAULT);
    VoC_movreg16(REG7,REG2,IN_PARALLEL);

    VoC_lw16i_set_inc(REG3,STRUCT_DigitalAgc_gainTable_addr,-2);
    VoC_shr16_ri(REG7,-1,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);
    VoC_shr32_ri(REG45,19,DEFAULT);                               //REG4 = frac = (short)(tmp32 >> 19);  // Q12.
    VoC_lw32inc_p(RL7,REG3,DEFAULT);                              //RL7 = gainTable[zeros]
    VoC_lw32_p(RL6,REG3,DEFAULT);                                 //RL6 = gainTable[zeros-1]
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_sub32_rr(REG67,RL6,RL7,DEFAULT);                          //REG67 = (stt->gainTable[zeros-1] - stt->gainTable[zeros])
    VoC_lw16i_short(FORMAT32,-16,IN_PARALLEL);

    VoC_movreg16(REG5,REG6,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_and16_ri(REG5,0x0001);
    VoC_bez16_r(Agc_ProcessDigital_MulS32S16_L0,REG5);
    VoC_movreg16(ACC0_LO,REG4,DEFAULT);
Agc_ProcessDigital_MulS32S16_L0:
    VoC_macX_rr(REG6,REG4,DEFAULT);
    VoC_mac32_rr(REG7,REG4,DEFAULT);                              //ACC0 = (stt->gainTable[zeros-1] - stt->gainTable[zeros]) * frac;
    VoC_NOP();
    VoC_shr32_ri(ACC0,12,DEFAULT);
    VoC_add32_rr(RL7,RL7,ACC0,DEFAULT);                                   //RL7 = gains[k + 1] = stt->gainTable[zeros] + (tmp32 >> 12);
    VoC_NOP();
    VoC_sw32inc_p(RL7,REG0,DEFAULT);
    VoC_endloop1
    //tomorrow begins:
    /*
    REG0: &gain[k]
    REG1: &env[k]
    REG2: zeros
    REG3: &gainTable[zeros-1]
    REG4: frac
    REG5: never mind
    REG6: never mind
    REG7: never mind
    ACC0:
    ACC1: never mind
    RL6:  gainTable[zeros-1]
    RL7:  gains[k + 1]
    */
    VoC_shr16_ri(REG2,-9,DEFAULT);
    VoC_shr16_ri(REG4,3,IN_PARALLEL);
    VoC_sub16_rr(REG7,REG2,REG4,DEFAULT);                            //REG7 = zeros = (zeros << 9) - (frac >> 3);
    VoC_lw32_d(REG45,STRUCT_DigitalAgc_capacitorFast_addr);
    VoC_movreg32(RL6,REG45,DEFAULT);
    VoC_jal(Coolsand_CLZU);                                         //REG2 = zeros_fast = WebRtcSpl_NormU32((unsigned int)stt->capacitorFast);
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i(REG6,0x7fff);
    VoC_sub16_rr(REG3,REG3,REG2,DEFAULT);                          //REG3 = -zeros_fast
    VoC_shru32_rr(REG45,REG3,DEFAULT);
    VoC_and16_rr(REG5,REG6,DEFAULT);                               //REG45 = tmp32 = (stt->capacitorFast << zeros_fast) & 0x7FFFFFFF;
    VoC_shr16_ri(REG2,-9,IN_PARALLEL);
    VoC_shr32_ri(REG45,22,DEFAULT);                                //REG4 = (short)(tmp32 >> 22);
    VoC_sub16_rr(REG2,REG2,REG4,DEFAULT);                          //REG2 = zeros_fast
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    /*
    REG0: &gain[k]
    REG1: &env[k]
    REG2: zeros_fast
    REG3: -zeros_fast
    REG4: (short)(tmp32 >> 22)
    REG5: never mind
    REG6: 0x7fff
    REG7: zeros
    ACC0:
    ACC1: never mind
    RL6:  capacitorFast
    RL7:  gains[k + 1]
    */
    //only use REG2,REG7
    VoC_lw16_d(REG3,STRUCT_AgcVadNearend_stdShortTerm_addr);
    VoC_lw16_d(REG4,STRUCT_DigitalAgc_NoiseGateThr_addr);           //REG4 = stt->NoiseGateThr;

    VoC_add16_rr(REG4,REG2,REG4,DEFAULT);                          //REG4 = stt->NoiseGateThr + zeros_fast
    VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);                      //REG3 = zeros + stt->vadNearend.stdShortTerm
    VoC_sub16_rr(REG4,REG4,REG3,DEFAULT);                          //REG4 = gate
    VoC_lw16i_short(REG2,0,IN_PARALLEL);

    VoC_bltz16_r(Agc_ProcessDigital_L13,REG4);
    VoC_lw16_d(REG2,STRUCT_DigitalAgc_gatePrevious_addr);
    VoC_lw16i_short(REG3,7,DEFAULT);
    VoC_multf32_rr(ACC0,REG2,REG3,DEFAULT);                        //consider IN_PARALLEL
    VoC_lw16i_short(REG5,0,DEFAULT);                               //REG45 = (long) gate
    VoC_add32_rr(REG23,REG45,ACC0,DEFAULT);
    VoC_shr32_ri(REG23,3,DEFAULT);                                 //REG2 = (short) gate_new
    VoC_NOP();

Agc_ProcessDigital_L13:
    VoC_sw16_d(REG2,STRUCT_DigitalAgc_gatePrevious_addr);
    VoC_bngtz16_r(Agc_ProcessDigital_L14,REG2);                    //if (gate > 0)
    //Add new code here
    VoC_lw16_d(REG6,STRUCT_DigitalAgc_NoiseCompressFactor_addr);      //REG6 = stt->NoiseCompressFactor
    VoC_lw16i_short(REG7,0,DEFAULT);                               //REG7 = gain_adj = 0;
    VoC_lw16i_short(FORMAT32,-16,IN_PARALLEL);
    VoC_bngt16_rr(Agc_ProcessDigital_L15,REG6,REG2)                //if (gate < stt->NoiseCompressFactor)
    VoC_sub16_rr(REG7,REG6,REG2,DEFAULT)                         //REG7 = gain_adj = (stt->NoiseCompressFactor - gate) >> 4;
    VoC_shr16_ri(REG7,4,DEFAULT);
Agc_ProcessDigital_L15:
    VoC_shr16_ri(REG6,4,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,APD_LOCAL_X_gains_addr,2);
    VoC_sub16_rr(REG7,REG7,REG6,DEFAULT);
    VoC_shr16_ri(REG5,-7,IN_PARALLEL); //REG5 = 128

    VoC_lw16_d(REG6,CONST_SubFrameNum_addr);

    VoC_inc_p(REG0,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG5,IN_PARALLEL); //REG7 = alpha = (128 - (stt->NoiseCompressFactor >> 4)) + gain_adj;

    VoC_lw32_d(RL7,STRUCT_DigitalAgc_gainTable_addr);              //RL7 = gainTable[0]
    //REG0 = &gains[k + 1], REG7 = alpha, RL7 = gainTable[0]
    VoC_loop_r(1,REG6);
    VoC_lw32_p(RL6,REG0,DEFAULT);
    VoC_lw16i_short(FORMATX,-1,IN_PARALLEL);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_sub32_rr(REG23,RL6,RL7,IN_PARALLEL);    //REG23 = gains[k + 1] - stt->gainTable[0]
    VoC_blt32_rd(Agc_ProcessDigital_L16,REG23,apd_const_int_0x01000000_addr);
    VoC_shr32_ri(REG23,7,DEFAULT);
    VoC_movreg16(REG5,REG2,DEFAULT);
    VoC_shru16_ri(REG2,1,IN_PARALLEL);
    VoC_and16_ri(REG5,0x0001);
    VoC_bez16_r(Agc_ProcessDigital_MulS32S16_L1,REG5);
    VoC_movreg16(ACC0_LO,REG7,DEFAULT);                     //also REG7 = (s16)alpha
Agc_ProcessDigital_MulS32S16_L1:
    VoC_macX_rr(REG2,REG7,DEFAULT);
    VoC_mac32_rr(REG3,REG7,DEFAULT);
    VoC_jump(Agc_ProcessDigital_L17);
Agc_ProcessDigital_L16:
    VoC_movreg16(REG5,REG2,DEFAULT);
    VoC_shru16_ri(REG2,1,IN_PARALLEL);
    VoC_and16_ri(REG5,0x0001);
    VoC_bez16_r(Agc_ProcessDigital_MulS32S16_L2,REG5);
    VoC_movreg16(ACC0_LO,REG7,DEFAULT);                     //also REG7 = (s16)alpha
Agc_ProcessDigital_MulS32S16_L2:
    VoC_macX_rr(REG2,REG7,DEFAULT);
    VoC_mac32_rr(REG3,REG7,DEFAULT);
    VoC_NOP();
    VoC_shr32_ri(ACC0,7,DEFAULT);
Agc_ProcessDigital_L17:
    VoC_add32_rr(RL6,RL7,ACC0,DEFAULT);
    VoC_NOP();
    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_endloop1;
    VoC_NOP();

    /*
    REG0: &gain[k]
    REG1: &env[k]
    REG2: (gains[k + 1] - stt->gainTable[0]), with REG3
    REG3:
    REG4:
    REG5:
    REG6:
    REG7:
    ACC0: tmp32
    ACC1: never mind
    RL6:
    RL7:
    */
    //all can be used
Agc_ProcessDigital_L14:
    VoC_lw16i_set_inc(REG0,APD_LOCAL_X_gains_addr,2);
    VoC_lw16i_set_inc(REG1,APD_LOCAL_X_env_addr,2);
    VoC_lw16_d(REG7,CONST_SubFrameNum_addr);
    VoC_inc_p(REG0,DEFAULT);
    //REG0 = &gains[k + 1], REG1 = &env[k]
    VoC_loop_r(1,REG7);
    VoC_lw16i_short(REG2,10,DEFAULT);                          //REG2 = zeros = 10;
    VoC_lw32_p(ACC1,REG0,IN_PARALLEL);                      //ACC1 = gains[k + 1]
    VoC_bngt32_rd(Agc_ProcessDigital_L18,ACC1,apd_const_int_0x2D413CC_addr);
    VoC_movreg32(RL6,ACC1,DEFAULT);
    VoC_lw16i_short(REG6,16,IN_PARALLEL);
    VoC_jal(Coolsand_CLZS);
    VoC_sub16_rr(REG2,REG6,REG2,DEFAULT);
Agc_ProcessDigital_L18:
    VoC_movreg32(REG45,ACC1,DEFAULT);
    VoC_shr32_rr(REG45,REG2,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_add32_rd(REG45,REG45,apd_const_int_1_addr)             //REG45 = gain32 = (gains[k + 1] >> zeros) + 1; in fact, REG4 can totally hold

    VoC_movreg16(REG3,REG4,DEFAULT);
    VoC_lw16i_short(FORMAT32,-2,IN_PARALLEL);
    VoC_and16_ri(REG3,0x0001);
    VoC_bez16_r(Agc_ProcessDigital_MulU16U16_L0,REG3);
    VoC_add32_rr(ACC0,REG45,REG45,DEFAULT);
    VoC_sub32_rd(ACC0,ACC0,apd_const_int_1_addr);
Agc_ProcessDigital_MulU16U16_L0:
    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,11,IN_PARALLEL);
    VoC_mac32_rr(REG4,REG4,DEFAULT);                           //ACC0 = gain32 *= gain32;
    VoC_sub16_rr(REG5,REG2,REG5,IN_PARALLEL);

    /*
    REG0: &gain[k]
    REG1: &env[k]
    REG2: zeros
    REG3: never mind
    REG4: gain32_old,with REG5
    REG5:
    REG6: 16
    REG7:
    ACC0: gain32_new
    ACC1: gains[k + 1]
    RL6:
    RL7:
    */
    VoC_lw16i_short(RL7_HI,0,DEFAULT);
    VoC_shr16_ri(REG5,-1,IN_PARALLEL);                         //REG5 = 2 * (zeros - 10 - 1)
    VoC_lw16i(RL7_LO,0x7fff);
    VoC_shr32_rr(RL7,REG5,DEFAULT);                            //RL7 = WEBRTC_SPL_SHIFT_W32((int)32767, 2 * (1 - zeros + 10))
    VoC_lw32inc_p(RL6,REG1,DEFAULT);
    VoC_sw32_d(RL7,APD_LOCAL_X_7fffshf_addr);
    VoC_shr32_ri(RL6,12,DEFAULT);
    VoC_movreg32(RL7,ACC1,IN_PARALLEL);
    VoC_add32_rd(RL6,RL6,apd_const_int_1_addr);                    //RL6 = (env[k] >> 12) + 1
    //above for while loop,
    /*
    REG0: &gain[k]
    REG1: &env[k]
    REG2: zeros
    ACC0: gain32
    ACC1: gains[k + 1]
    RL6: (env[k] >> 12) + 1
    RL7: gains[k + 1]
    */
    //can be used REG3,4,5,6,7,ACC0,ACC1
Agc_ProcessDigital_L22:
    //VoC_sw32_d(ACC0,APD_LOCAL_X_gain32_addr);
    VoC_movreg32(REG45,ACC0,DEFAULT);
    VoC_movreg16(REG3,ACC0_LO,IN_PARALLEL);
    VoC_shr32_ri(REG45,13,DEFAULT);                            //REG4 = (gain32)>>13
    VoC_movreg32(REG67,RL6,IN_PARALLEL);
    VoC_and16_ri(REG3,0x1fff);                                 //REG3 = (0x00001FFF & (gain32))
    VoC_lw16i_short(FORMAT32,-16,DEFAULT);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_movreg16(REG5,REG6,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_and16_ri(REG5,0x0001);
    VoC_bez16_r(Agc_ProcessDigital_MulS32S16_L3,REG5);
    VoC_movreg16(ACC0_LO,REG4,DEFAULT);
    VoC_movreg16(ACC1_LO,REG3,IN_PARALLEL);
Agc_ProcessDigital_MulS32S16_L3:
    VoC_macX_rr(REG6,REG4,DEFAULT);
    VoC_macX_rr(REG6,REG3,IN_PARALLEL);
    VoC_mac32_rr(REG7,REG4,DEFAULT);
    VoC_mac32_rr(REG7,REG3,IN_PARALLEL);
    VoC_lw32_d(REG45,APD_LOCAL_X_7fffshf_addr)
    VoC_shr32_ri(ACC1,13,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);                      //ACC0 = AGC_MUL32((env[k] >> 12) + 1, gain32)

    /*
    REG0: &gain[k]
    REG1: &env[k]
    REG2: zeros
    REG3: never mind
    REG4: never mind
    REG5: never mind
    REG6:
    REG7:
    ACC0: AGC_MUL32((env[k] >> 12) + 1, gain32)
    ACC1: never mind
    RL6:  (env[k] >> 12) + 1
    RL7:  gains[k + 1]
    */
    VoC_bngt32_rr(Agc_ProcessDigital_L19,ACC0,REG45);
    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_bngt32_rd(Agc_ProcessDigital_L20,REG67,apd_const_int_0x7fffff_addr);
    VoC_shr32_ri(REG67,8,DEFAULT);
    VoC_sub32_rr(RL7,RL7,REG67,DEFAULT);
    VoC_shr32_ri(REG67,-1,IN_PARALLEL);
    VoC_sub32_rr(RL7,RL7,REG67,DEFAULT);
    VoC_jump(Agc_ProcessDigital_L21);
Agc_ProcessDigital_L20:
    VoC_shr32_ri(REG67,-8,DEFAULT);
    VoC_sub32_rr(REG67,REG67,RL7,DEFAULT)
    VoC_shr32_ri(RL7,-1,IN_PARALLEL);
    VoC_sub32_rr(RL7,REG67,RL7,DEFAULT);
Agc_ProcessDigital_L21:

    VoC_movreg32(REG45,RL7,DEFAULT);
    VoC_shr32_rr(REG45,REG2,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_add32_rd(REG45,REG45,apd_const_int_1_addr)             //REG45 = gain32 = (gains[k + 1] >> zeros) + 1; in fact, REG4 can totally hold

    VoC_movreg16(REG3,REG4,DEFAULT);
    VoC_lw16i_short(FORMAT32,-2,IN_PARALLEL);
    VoC_and16_ri(REG3,0x0001);
    VoC_bez16_r(Agc_ProcessDigital_MulU16U16_L1,REG3);
    VoC_add32_rr(ACC0,REG45,REG45,DEFAULT);
    VoC_sub32_rd(ACC0,ACC0,apd_const_int_1_addr);
Agc_ProcessDigital_MulU16U16_L1:
    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_mac32_rr(REG4,REG4,DEFAULT);                           //ACC0 = gain32 *= gain32;
    VoC_jump(Agc_ProcessDigital_L22);

Agc_ProcessDigital_L19:
    VoC_sw32inc_p(RL7,REG0,DEFAULT);
    VoC_endloop1

    /*
    REG0: &gain[k]
    REG1: &env[k]
    REG2: zeros
    REG3: never mind
    REG4: never mind
    REG5: never mind
    REG6:
    REG7:
    ACC0: gain32 *= gain32
    ACC1: never mind
    RL6:  (env[k] >> 12) + 1
    RL7:  gains[k + 1]
    */

    VoC_lw16_d(REG7,CONST_SubFrameNum_addr);
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_bngt16_rr(Agc_ProcessDigital_L23,REG7,REG6);
    VoC_sub16_rr(REG5,REG7,REG6,DEFAULT);                                 //consider IN_PARALLEL with next DEFAULT insn
    VoC_lw16i_set_inc(REG0,APD_LOCAL_X_gains_addr,2);//k=0
    VoC_NOP();
    VoC_inc_p(REG0,DEFAULT);//k=1
    VoC_movreg16(REG1,REG0,DEFAULT);//here maybe not right, adjust sequence to get correct, REG1 = k=1
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_lw32inc_p(RL6,REG0,DEFAULT);//k=2,RL6=gains[1]
    VoC_loop_r_short(REG5,DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p(RL7,REG0,DEFAULT);//k=3,RL7=gains[2]
    VoC_bngt32_rr(Agc_ProcessDigital_L24,RL6,RL7);
    VoC_sw32_p(RL7,REG1,DEFAULT);//gains[k=1]=gains[2]
Agc_ProcessDigital_L24:
    VoC_movreg32(RL6,RL7,DEFAULT);//RL6=gains[2]
    VoC_inc_p(REG1,IN_PARALLEL);//sw k=2
    VoC_endloop0
    VoC_NOP();

Agc_ProcessDigital_L23:
    VoC_lw16i_set_inc(REG0,APD_LOCAL_X_gains_addr,2);
    VoC_shr16_ri(REG7,-1,DEFAULT);
    VoC_add16_rr(REG1,REG0,REG7,DEFAULT);
    VoC_NOP();
    VoC_lw32_p(ACC1,REG1,DEFAULT);
    VoC_lw32inc_p(RL6,REG0,DEFAULT);                            //RL6 = gains[0],maybe move upward to replace nop?
    VoC_sw32_d(ACC1,STRUCT_DigitalAgc_gain_addr);
    VoC_lw32_p(RL7,REG0,DEFAULT);                               //RL7 = gains[1]
    VoC_lw16i_short(REG3,-4,IN_PARALLEL);
    VoC_lw16_d(REG2,APD_LOCAL_X_L2_addr);                       //REG2 = L2
    VoC_sub32_rr(RL7,RL7,RL6,DEFAULT);
    VoC_sub16_rr(REG2,REG3,REG2,IN_PARALLEL);                   //REG2 = -4-L2
    VoC_shr32_rr(RL7,REG2,DEFAULT);                             //RL7 = delta = (gains[1] - gains[0]) << (4 - L2);
    VoC_shr32_ri(RL6,-4,IN_PARALLEL);                           //RL6 = gain32 = gains[0] << 4;

    /*
    REG0: &gain[1]
    REG1: gains[SubFrameNum]
    REG2: L2 - 4
    REG3: 4
    REG4: never mind
    REG5: never mind
    REG6: 1
    REG7: SubFrameNum<<1
    ACC0: gain32 *= gain32
    ACC1: gains[SubFrameNum]
    RL6:  gain32
    RL7:  delta
    */
    VoC_lw16_d(REG7,APD_LOCAL_X_L_addr);
    VoC_lw16d_set_inc(REG0,buf_addr_addr,1);
    VoC_lw16i_short(FORMAT32,-16,DEFAULT);
    VoC_loop_r(1,REG7);
    VoC_add32_rd(REG45,RL6,apd_const_int_0x7f_addr);
    VoC_shr32_ri(REG45,7,DEFAULT);                          //REG45 = ((gain32 + 127) >> 7)
    VoC_lw16_p(REG6,REG0,IN_PARALLEL);                   //REG6 = out[n]

    VoC_movreg16(REG3,REG4,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_and16_ri(REG3,0x0001);
    VoC_bez16_r(Agc_ProcessDigital_MulS32S16_L4,REG3);
    VoC_movreg16(ACC0_LO,REG6,DEFAULT);
    VoC_bnltz16_r(Agc_ProcessDigital_MulS32S16_L4,REG6);
    VoC_lw16i_short(ACC0_HI,-1,DEFAULT);
Agc_ProcessDigital_MulS32S16_L4:
    VoC_macX_rr(REG4,REG6,DEFAULT);
    VoC_mac32_rr(REG5,REG6,DEFAULT);
    VoC_NOP();
    VoC_movreg16(REG1,ACC0_HI,DEFAULT);
    VoC_bngt16_rd(Agc_ProcessDigital_L25,REG1,apd_const_0x0fff_addr);
    VoC_lw16i(REG2,0x7fff);
    VoC_jump(Agc_ProcessDigital_L27);
Agc_ProcessDigital_L25:
    VoC_bnlt16_rd(Agc_ProcessDigital_L26,REG1,apd_const_0xf000_addr);
    VoC_lw16i(REG2,0x8000);
    VoC_jump(Agc_ProcessDigital_L27);
Agc_ProcessDigital_L26:
    VoC_movreg32(REG45,RL6,DEFAULT);
    VoC_shr32_ri(REG45,4,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_movreg16(REG3,REG4,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_and16_ri(REG3,0x0001);
    VoC_bez16_r(Agc_ProcessDigital_MulS32S16_L5,REG3);
    VoC_movreg16(ACC0_LO,REG6,DEFAULT);
    VoC_bnltz16_r(Agc_ProcessDigital_MulS32S16_L5,REG6);
    VoC_lw16i_short(ACC0_HI,-1,DEFAULT);
Agc_ProcessDigital_MulS32S16_L5:
    VoC_macX_rr(REG4,REG6,DEFAULT);
    VoC_mac32_rr(REG5,REG6,DEFAULT);
    VoC_NOP();
    VoC_movreg16(REG2,ACC0_HI,DEFAULT);
Agc_ProcessDigital_L27:
    VoC_add32_rr(RL6,RL6,RL7,DEFAULT);
    VoC_sw16inc_p(REG2,REG0,DEFAULT);
    VoC_endloop1

    /*
    REG0: buf_addr=&out[n]
    REG1: out_tmp
    REG2: out[n]  //write to
    REG3: never mind
    REG4: never mind
    REG5: never mind
    REG6: out[n]  //read from
    REG7: L
    ACC0: tmp32
    ACC1: gains[SubFrameNum]
    RL6:  gain32
    RL7:  delta
    */
    VoC_lw16_d(REG5,CONST_SubFrameNum_addr);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_bngt16_rr(Agc_ProcessDigital_L28,REG5,REG4);
    VoC_sub16_rr(REG5,REG5,REG4,DEFAULT);                                 //consider IN_PARALLEL with next DEFAULT insn
    //VoC_lw16i_set_inc(REG0,1,1);
    VoC_lw16i_set_inc(REG1,APD_LOCAL_X_gains_addr,2);
    VoC_NOP();
    //VoC_lw16i_set_inc(REG2,buf_addr,1);
    VoC_inc_p(REG1,DEFAULT);//k=1
    VoC_NOP();
    VoC_lw32inc_p(RL6,REG1,DEFAULT);                       //RL6 = gains[k]

    /*
    REG0: k
    REG1: &gains[k+1]
    REG2: buf_addr
    REG3: never mind
    REG4: 1
    REG5: SubFrameNum-1
    REG6: out[n]  //read from
    REG7: L
    ACC0: tmp32
    ACC1: gains[SubFrameNum]
    RL6:  gain32
    RL7:  delta
    */
    //loop keep regs: REG0,1,2,7
    //can be used: REG3,4,5,6

    VoC_loop_r(1,REG5);
    VoC_lw32inc_p(RL7,REG1,DEFAULT);                     //RL7 = gains[k+1]
    VoC_lw16i_short(REG3,-4,IN_PARALLEL);
    VoC_sub32_rr(ACC1,RL7,RL6,DEFAULT);
    VoC_lw16_d(REG6,APD_LOCAL_X_L2_addr);                //REG6 = L2
    VoC_sub16_rr(REG3,REG3,REG6,DEFAULT);                   //REG3 = -4-L2
    VoC_shr32_rr(ACC1,REG3,DEFAULT);                            //ACC1 = delta = (gains[k+1] - gains[k]) << (4 - L2);
    VoC_shr32_ri(RL6,-4,IN_PARALLEL);                           //RL6 = gain32 = gains[k] << 4;
    //VoC_movreg16(REG4,REG0,DEFAULT);
    //VoC_shr16_rr(REG4,REG6,DEFAULT);
    //VoC_add16_rr(REG2,REG2,REG4,DEFAULT);                       //REG2 = &out[k * L]
    //loop keep regs: REG0,1,2,7
    /*
    REG0: k
    REG1: &gains[k+1]
    REG2: buf_addr
    REG7: L
    ACC1: delta
    RL6: gain32
    RL7: gains[k+1]
    */
    VoC_loop_r_short(REG7,DEFAULT);
    VoC_startloop0
    VoC_lw16_p(REG6,REG0,DEFAULT);
    VoC_movreg32(REG45,RL6,IN_PARALLEL);
    VoC_shr32_ri(REG45,4,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_movreg16(REG3,REG4,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_and16_ri(REG3,0x0001);
    VoC_bez16_r(Agc_ProcessDigital_MulS32S16_L6,REG3);
    VoC_movreg16(ACC0_LO,REG6,DEFAULT);
    VoC_bnltz16_r(Agc_ProcessDigital_MulS32S16_L6,REG6);
    VoC_lw16i_short(ACC0_HI,-1,DEFAULT);
Agc_ProcessDigital_MulS32S16_L6:
    VoC_macX_rr(REG4,REG6,DEFAULT);
    VoC_mac32_rr(REG5,REG6,DEFAULT);
    VoC_add32_rr(RL6,RL6,ACC1,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);
    VoC_endloop0
    VoC_movreg32(RL6,RL7,DEFAULT);
    VoC_endloop1
    VoC_NOP();
Agc_ProcessDigital_L28:
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_NonClipDigitalGain
 *
 * Description: no
 *
 * Inputs:  REG0
 *          REG1
 *          REG2
 *
 * Outputs: REG0
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       07/05/2011
 **************************************************************************************/
void Coolsand_NonClipDigitalGain(void)
{
    VoC_lw16_p(REG4,REG3,DEFAULT);

    VoC_push32(REG23,DEFAULT);
    VoC_movreg32(RL7,REG01,IN_PARALLEL);

    VoC_sw16_d(REG4,GLOBAL_DIGITAL_OLD_GAIN_ADDR);

    VoC_lw16i(REG7,FRAMESIZE);

    VoC_add16_rr(REG3,REG1,REG7,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);

    VoC_loop_i(0,(FRAMESIZE/2))
    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(REG45,REG1,DEFAULT);
    VoC_sw32inc_p(REG67,REG3,DEFAULT);
    VoC_endloop0

    VoC_lw32z(REG67,DEFAULT);//REG6:i;REG7:CrossZeroConter
    VoC_movreg16(REG1,RL7_HI,IN_PARALLEL);//REG0;outbuf;REG1:HistoryData

    VoC_lw16i(REG5,FRAMESIZE*2-1);

    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,GLOBAL_NON_CLIP_CROSSZEROINDEX,1);

Coolsand_NonClipDigitalGain_L0:
    VoC_lw16inc_p(REG2,REG1,DEFAULT);
    VoC_lw16_p(REG3,REG1,DEFAULT);
    VoC_bltz16_r(Coolsand_NonClipDigitalGain_L1,REG2)
    VoC_bnltz16_r(Coolsand_NonClipDigitalGain_L1,REG3)
    VoC_sw16inc_p(REG6,REG0,DEFAULT);//CrossZeroIndex[j]=i;
    VoC_add16_rr(REG7,REG7,REG4,IN_PARALLEL);//CrossZeroConter++;

    VoC_lw16i(REG2,FRAMESIZE);
    VoC_bngt16_rr(Coolsand_NonClipDigitalGain_L3,REG2,REG6);
    VoC_jump(Coolsand_NonClipDigitalGain_L2)
Coolsand_NonClipDigitalGain_L1:
    VoC_bgtz16_r(Coolsand_NonClipDigitalGain_L2,REG2)
    VoC_bngtz16_r(Coolsand_NonClipDigitalGain_L2,REG3)
    VoC_sw16inc_p(REG6,REG0,DEFAULT);//CrossZeroIndex[j]=i;
    VoC_add16_rr(REG7,REG7,REG4,IN_PARALLEL);//CrossZeroConter++;

    VoC_lw16i(REG2,FRAMESIZE);
    VoC_bngt16_rr(Coolsand_NonClipDigitalGain_L3,REG2,REG6);
Coolsand_NonClipDigitalGain_L2:
    VoC_add16_rr(REG6,REG6,REG4,DEFAULT);//i++
    VoC_bgt16_rr(Coolsand_NonClipDigitalGain_L0,REG5,REG6)

Coolsand_NonClipDigitalGain_L3:

    //REG7:CrossZeroConter++
    //REG6:i


    VoC_lw16i_short(REG2,2,DEFAULT);

// VoC_directive: PARSER_OFF

    printf("CrossZeroConter:%4d\n",REGS[7].reg);
// VoC_directive: PARSER_ON
    VoC_bgt16_rr(Coolsand_NonClipDigitalGain_End,REG2,REG7)

//      VoC_lw16i_short(INC0,1,DEFAULT);
//          VoC_movreg32(REG01,RL7,IN_PARALLEL);

//      VoC_lw16i_short(INC1,1,DEFAULT);
//      VoC_lw16i_short(REG3,1,IN_PARALLEL);

//      VoC_add16_rd(REG2,REG3,GLOBAL_NON_CLIP_CROSSZEROINDEX);

//      VoC_loop_r_short(REG2,DEFAULT);
//      VoC_lw16inc_p(REG4,REG1,IN_PARALLEL);
//      VoC_startloop0
//          VoC_lw16inc_p(REG4,REG1,DEFAULT);
//          VoC_sw16inc_p(REG4,REG0,DEFAULT);
//      VoC_endloop0


    // find the peak.
    //for (i=0;i<CrossZeroConter-1;i++)

    VoC_lw16i_short(INC0,1,DEFAULT);

    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);

    VoC_movreg32(REG01,RL7,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG3,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,GLOBAL_NON_CLIP_CROSSZEROINDEX,1);

    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG3,IN_PARALLEL);

    VoC_movreg32(RL6,REG01,DEFAULT);

    //VoC_loop_r(1,REG7)
    VoC_lw16i_short(REG6, 0, DEFAULT);
Coolsand_NonClipDigitalGain_LoopStart:
    VoC_be16_rr(Coolsand_NonClipDigitalGain_LoopEnd, REG6, REG7)
    VoC_push32(REG67, DEFAULT);

    VoC_lw16inc_p(REG3,REG2,DEFAULT);

    VoC_sub16_pr(REG4,REG2,REG3,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG3,IN_PARALLEL);

    VoC_add16_rr(REG1,REG1,REG3,DEFAULT);
    VoC_lw32z(REG67,IN_PARALLEL);//CrossZeroPeak

    VoC_movreg16(REG3,REG1,DEFAULT);

    VoC_loop_r(0,REG4)
    VoC_lw16inc_p(REG5,REG1,DEFAULT);//int temp_value=HistoryData[CrossZeroIndex[i]+j];
    VoC_bnltz16_r(Coolsand_NonClipDigitalGain_L5,REG5)
    VoC_sub16_rr(REG5,REG6,REG5,DEFAULT);
Coolsand_NonClipDigitalGain_L5:
    VoC_bngt16_rr(Coolsand_NonClipDigitalGain_L6,REG5,REG7)
    VoC_movreg16(REG7,REG5,DEFAULT);
Coolsand_NonClipDigitalGain_L6:
    VoC_NOP();
    VoC_endloop0

    VoC_movreg16(REG1,REG3,DEFAULT);

    // Thr in GLOBAL_DIGITAL_MAXVALUE_ADDR
    VoC_bngt16_rd(Coolsand_NonClipDigitalGain_L7,REG7,GLOBAL_DIGITAL_MAXVALUE_ADDR) // REG7 CrossZeroPeak

    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw16i_short(FORMAT16,(4-16),DEFAULT);
    VoC_shr32_ri(REG67,16,IN_PARALLEL);

    VoC_lw16i(ACC0_HI,30000);

    VoC_shr32_ri(ACC0,12,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_loop_i(0,255)
    VoC_sub32_rr(ACC0,ACC0,REG67,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG5,IN_PARALLEL);

    VoC_bgt32_rr(Coolsand_NonClipDigitalGain_L8,REG67,ACC0);
    VoC_endloop0
    // temp_mutiple in REG3
    VoC_jump(Coolsand_NonClipDigitalGain_L8);
Coolsand_NonClipDigitalGain_L7:
    //VoC_lw16i(REG5, 1000);
    VoC_lw16_d(REG5, GLOBAL_NON_CLIP_SMOOTH_THRESHOLD);
    VoC_NOP();
    VoC_bngt16_rr(Coolsand_NonClipDigitalGain_L10,REG7,REG5) // REG7 CrossZeroPeak
    VoC_lw16i(REG5,0);

    VoC_lw16i_short(FORMAT16,(4-16),DEFAULT);
    VoC_lw16_d(REG3,GLOBAL_DIGITAL_GAIN_ADDR);
    VoC_bez16_d(Coolsand_NonClipDigitalGain_L8,GLOBAL_DIGITAL_GAIN_ADDR)

    VoC_bgt16_rr(Coolsand_NonClipDigitalGain_L9,REG7,REG5)

    VoC_lw16_d(REG7,GLOBAL_DIGITAL_OLD_GAIN_ADDR);
    //VoC_lw16i_short(FORMAT16,(4-16),DEFAULT);
    VoC_lw16_d(REG3,GLOBAL_NOISE_DIGITAL_GAIN_ADDR);

    VoC_bngt16_rr(Coolsand_NonClipDigitalGain_L8,REG7,REG3)
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_sub16_rr(REG3,REG7,REG3,DEFAULT);
    VoC_jump(Coolsand_NonClipDigitalGain_L8);
Coolsand_NonClipDigitalGain_L9:
    VoC_lw16_d(REG7,GLOBAL_DIGITAL_OLD_GAIN_ADDR);
    //  VoC_lw16i_short(FORMAT16,(4-16),DEFAULT);
    VoC_lw16_d(REG3,GLOBAL_DIGITAL_GAIN_ADDR);

    VoC_bngt16_rr(Coolsand_NonClipDigitalGain_L8,REG3,REG7)
    VoC_lw16i_short(REG3,2,DEFAULT);
    VoC_add16_rr(REG3,REG7,REG3,DEFAULT);
Coolsand_NonClipDigitalGain_L8:

    VoC_loop_r(0,REG4)

    VoC_multf16_rp(REG7,REG3,REG1,DEFAULT);

    VoC_NOP();

    VoC_NOP();

    VoC_sw16inc_p(REG7,REG1,DEFAULT);

    VoC_endloop0

    VoC_sw16_d(REG3,GLOBAL_DIGITAL_OLD_GAIN_ADDR);

    VoC_jump(Coolsand_NonClipDigitalGain_L12);

Coolsand_NonClipDigitalGain_L10:
    //VoC_lw16i(REG5, 500);
    VoC_lw16_d(REG5, GLOBAL_NON_CLIP_NOISE_THRESHOLD);
    VoC_NOP();
    VoC_bngt16_rr(Coolsand_NonClipDigitalGain_L11,REG7,REG5) // REG7 CrossZeroPeak
    VoC_NOP();
    VoC_jump(Coolsand_NonClipDigitalGain_L12);
Coolsand_NonClipDigitalGain_L11:
    VoC_loop_r(0, REG4)
    VoC_lw16_p(REG7, REG1, DEFAULT);
    VoC_shr16_ri(REG7, 1, DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG7, REG1, DEFAULT);
    //VoC_NOP();
    VoC_endloop0
Coolsand_NonClipDigitalGain_L12:
    VoC_pop32(REG67, DEFAULT);
    VoC_lw16i_short(REG0, 1, IN_PARALLEL);
    VoC_NOP();
    VoC_add16_rr(REG6, REG6, REG0, DEFAULT);

    //GLOBAL_RECEIVER_DIGITAL_MAXVALUE_ADDR
    VoC_movreg32(REG01,RL6,DEFAULT);

    VoC_jump(Coolsand_NonClipDigitalGain_LoopStart);
Coolsand_NonClipDigitalGain_LoopEnd:
    //VoC_endloop1


Coolsand_NonClipDigitalGain_End:


    VoC_pop32(REG23,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_lw16_d(REG4,GLOBAL_DIGITAL_OLD_GAIN_ADDR);

    VoC_movreg32(REG01,RL7,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    VoC_sw16_p(REG4,REG3,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);

    VoC_lw32inc_p(REG45,REG1,DEFAULT);

    VoC_loop_i(0,(FRAMESIZE/2))
    VoC_lw32inc_p(REG45,REG1,DEFAULT);
    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_endloop0

    VoC_loop_i(0,(FRAMESIZE/2))
    VoC_lw32inc_p(REG45,REG1,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,DEFAULT);
    VoC_endloop0

    VoC_return;

}

void Rda_ProcessWebRtcAgc(void)
{
    VoC_push16(RA, DEFAULT);

    VoC_jal(Agc_ProcessDigital);

    VoC_lw16_d(REG0, buf_addr_addr);
    VoC_lw16i(REG1, 80);
    VoC_add16_rr(REG0, REG0, REG1, DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG0, buf_addr_addr);

    VoC_jal(Agc_ProcessDigital);

    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/*
 * working now
 */
/*
void Rda_ProcessIIRBand(void)
{
    VoC_lw16d_set_inc(REG3,ENC_INPUT_ADDR_ADDR,1);

    VoC_lw16_d(REG0,GLOBAL_IIR_BAND_DEN1_ADDR);
    VoC_lw16_d(REG1,GLOBAL_IIR_BAND_DEN2_ADDR);

    VoC_lw16_d(REG2,GLOBAL_IIR_BAND_NUM1_ADDR); // num[1]

    VoC_lw32_d(REG45,GLOBAL_IIR_BAND_XN_1_ADDR);
    VoC_lw32_d(REG67,GLOBAL_IIR_BAND_XN_2_ADDR);

    VoC_lw32_d(RL6, GLOBAL_IIR_BAND_YN_1_ADDR);
    VoC_lw32_d(RL7, GLOBAL_IIR_BAND_YN_2_ADDR);

    VoC_lw16i_short(FORMAT32,14,DEFAULT);
    VoC_lw16i_short(FORMATX,0,DEFAULT);
    VoC_lw16i_short(FORMAT16,(16-14),DEFAULT);

    VoC_lw16i_short(ACC1_HI, 0, DEFAULT);
    VoC_lw16i_short(ACC1_LO, 1, IN_PARALLEL);
    VoC_shr32_ri(ACC1, (1-IIR_DATA_SHIFT), DEFAULT);

    VoC_loop_i(0,160)
        VoC_push32(REG45, DEFAULT);
        VoC_push32(REG67, DEFAULT);

        // num[1] * xn_1
        VoC_shr32_ri(REG45, -2, DEFAULT);
        VoC_shru16_ri(REG4, 2, DEFAULT); // split 30:14 13:0

        VoC_multf32_rr(ACC0, REG2, REG4, DEFAULT);
        VoC_macX_rr(REG2, REG5, DEFAULT);

        VoC_lw16_p(REG5, REG3, DEFAULT);
        VoC_lw16i_short(REG4, 0, IN_PARALLEL);

        VoC_lw16_d(REG2, GLOBAL_IIR_BAND_NUM0_ADDR);

        // num[0] * x
        VoC_shr32_ri(REG45, (16-13), DEFAULT);
        VoC_push16(REG3, IN_PARALLEL);

        VoC_shr32_ri(REG45, -2, DEFAULT);
        VoC_shru16_ri(REG4, 2, DEFAULT);

        VoC_mac32_rr(REG2, REG4, DEFAULT);
        VoC_macX_rr(REG2, REG5, DEFAULT);

        VoC_lw16_d(REG2, GLOBAL_IIR_BAND_NUM2_ADDR);
        // num[2] * xn_2
        VoC_shr32_ri(REG67, -2, DEFAULT);
        VoC_shru16_ri(REG6, 2, DEFAULT);

        VoC_mac32_rr(REG2, REG6, DEFAULT);
        VoC_macX_rr(REG2, REG7, DEFAULT);

        VoC_movreg32(REG45, RL7, DEFAULT);
        VoC_movreg32(REG67, RL6, IN_PARALLEL);

        // den[2] * yn_2
        VoC_shr32_ri(REG45, -2, DEFAULT);
        VoC_shr32_ri(REG67, -2, IN_PARALLEL);

        VoC_shru16_ri(REG4, 2, DEFAULT); // split 30:14 13:0
        VoC_shru16_ri(REG6, 2, IN_PARALLEL);

        VoC_mac32_rr(REG1, REG4, DEFAULT);
        VoC_macX_rr(REG1, REG5, DEFAULT);

        // den[1] * yn_1
        VoC_mac32_rr(REG0, REG6, DEFAULT);
        VoC_macX_rr(REG0, REG7, DEFAULT);

        // shift y
        VoC_movreg32(RL7, RL6, DEFAULT);
        VoC_movreg32(RL6, ACC0, DEFAULT);

        VoC_add32_rr(ACC0, ACC0, ACC1, DEFAULT);
        VoC_shr32_ri(ACC0, 13, DEFAULT);

        VoC_pop32(REG67, DEFAULT);
        VoC_pop32(REG45, DEFAULT);

        VoC_lw16_p(REG3, REG3, DEFAULT);
        // shift x
        VoC_movreg32(REG67, REG45, DEFAULT);
        VoC_lw16i_short(REG2, 0, IN_PARALLEL);

        VoC_shr32_ri(REG23, (16-13), DEFAULT);
        VoC_movreg32(REG45, REG23, DEFAULT);

        VoC_pop16(REG3, DEFAULT);
        VoC_NOP();
        VoC_sw16inc_p(ACC0_LO, REG3, DEFAULT);

        VoC_lw16_d(REG2, GLOBAL_IIR_BAND_NUM1_ADDR);
    VoC_endloop0

    VoC_sw32_d(REG45,GLOBAL_IIR_BAND_XN_1_ADDR);
    VoC_sw32_d(REG67,GLOBAL_IIR_BAND_XN_2_ADDR);

    VoC_sw32_d(RL6,GLOBAL_IIR_BAND_YN_1_ADDR);
    VoC_sw32_d(RL7,GLOBAL_IIR_BAND_YN_2_ADDR);

    VoC_return;
}
*/
// coeffs are Q14 and IIR_DATA_SHIFT is 13
void Rda_ProcessIIRBand(void)
{
    VoC_lw16d_set_inc(REG3,IIR_INPUT_ADDR_ADDR,1);

    VoC_lw16_d(REG0,GLOBAL_IIR_BAND_DEN1_ADDR);
    VoC_lw16_d(REG1,GLOBAL_IIR_BAND_DEN2_ADDR);

    VoC_lw16_d(REG2,GLOBAL_IIR_BAND_NUM1_ADDR); // num[1]

    /*
    VoC_lw32_d(REG45,GLOBAL_IIR_BAND_XN_1_ADDR);
    VoC_lw32_d(REG67,GLOBAL_IIR_BAND_XN_2_ADDR);

    VoC_lw32_d(RL6, GLOBAL_IIR_BAND_YN_1_ADDR);
    VoC_lw32_d(RL7, GLOBAL_IIR_BAND_YN_2_ADDR);
    */
    VoC_lw16_d(REG6, GLOBAL_IIR_BAND_YN_1_USED_ADDR_ADDR);
    VoC_lw16_d(REG7, GLOBAL_IIR_BAND_YN_2_USED_ADDR_ADDR);

    VoC_lw32_p(RL6, REG6, DEFAULT);
    VoC_lw32_p(RL7, REG7, DEFAULT);

    VoC_lw16_d(REG4, GLOBAL_IIR_BAND_XN_1_USED_ADDR_ADDR);
    VoC_lw16_d(REG6, GLOBAL_IIR_BAND_XN_2_USED_ADDR_ADDR);

    VoC_lw32_p(REG45, REG4, DEFAULT);
    VoC_lw32_p(REG67, REG6, DEFAULT);

    VoC_lw16i_short(FORMAT32,14,DEFAULT);
    VoC_lw16i_short(FORMATX,0,DEFAULT);
    VoC_lw16i_short(FORMAT16,(16-14),DEFAULT);

    VoC_lw16i_short(ACC1_HI, 0, DEFAULT);
    VoC_lw16i_short(ACC1_LO, 1, IN_PARALLEL);
    VoC_shr32_ri(ACC1, (1-IIR_DATA_SHIFT), DEFAULT);

    VoC_loop_i(0,160)
    VoC_push32(REG45, DEFAULT);
    VoC_push32(REG67, DEFAULT);

    // num[1] * xn_1
    VoC_shr32_ri(REG45, -2, DEFAULT);
    VoC_shru16_ri(REG4, 2, DEFAULT); // split 30:14 13:0

    VoC_multf32_rr(ACC0, REG2, REG4, DEFAULT);
    VoC_macX_rr(REG2, REG5, DEFAULT);

    VoC_lw16_p(REG5, REG3, DEFAULT);
    VoC_lw16i_short(REG4, 0, IN_PARALLEL);

    VoC_lw16_d(REG2, GLOBAL_IIR_BAND_NUM0_ADDR);

    // num[0] * x
    VoC_shr32_ri(REG45, (16-IIR_DATA_SHIFT), DEFAULT);
    VoC_push16(REG3, IN_PARALLEL);

    VoC_shr32_ri(REG45, -2, DEFAULT);
    VoC_shru16_ri(REG4, 2, DEFAULT);

    VoC_mac32_rr(REG2, REG4, DEFAULT);
    VoC_macX_rr(REG2, REG5, DEFAULT);

    VoC_lw16_d(REG2, GLOBAL_IIR_BAND_NUM2_ADDR);
    // num[2] * xn_2
    VoC_shr32_ri(REG67, -2, DEFAULT);
    VoC_shru16_ri(REG6, 2, DEFAULT);

    VoC_mac32_rr(REG2, REG6, DEFAULT);
    VoC_macX_rr(REG2, REG7, DEFAULT);

    VoC_movreg32(REG45, RL7, DEFAULT);
    VoC_movreg32(REG67, RL6, IN_PARALLEL);

    // den[2] * yn_2
    VoC_shr32_ri(REG45, -2, DEFAULT);
    VoC_shr32_ri(REG67, -2, IN_PARALLEL);

    VoC_shru16_ri(REG4, 2, DEFAULT); // split 30:14 13:0
    VoC_shru16_ri(REG6, 2, IN_PARALLEL);

    VoC_mac32_rr(REG1, REG4, DEFAULT);
    VoC_macX_rr(REG1, REG5, DEFAULT);

    // den[1] * yn_1
    VoC_mac32_rr(REG0, REG6, DEFAULT);
    VoC_macX_rr(REG0, REG7, DEFAULT);

    // shift y
    VoC_movreg32(RL7, RL6, DEFAULT);
    VoC_movreg32(RL6, ACC0, DEFAULT);

    VoC_add32_rr(ACC0, ACC0, ACC1, DEFAULT);
    VoC_shr32_ri(ACC0, 13, DEFAULT);

    VoC_pop32(REG67, DEFAULT);
    VoC_pop32(REG45, DEFAULT);

    VoC_lw16_p(REG3, REG3, DEFAULT);
    // shift x
    VoC_movreg32(REG67, REG45, DEFAULT);
    VoC_lw16i_short(REG2, 0, IN_PARALLEL);

    VoC_shr32_ri(REG23, (16-IIR_DATA_SHIFT), DEFAULT);
    VoC_movreg32(REG45, REG23, DEFAULT);

    VoC_pop16(REG3, DEFAULT);

    // clip output pcm
    VoC_bngt32_rd(IIR_CLIP_MIN_SHORT, ACC0, HPF_CLIP_MAX_SHORT)
    VoC_lw16i(ACC0_LO, 0x7fff);
    VoC_jump(IIR_CLIP_END);
IIR_CLIP_MIN_SHORT:
    VoC_bnlt32_rd(IIR_CLIP_END, ACC0, HPF_CLIP_MIN_SHORT)
    VoC_lw16i(ACC0_LO, 0x8000);
IIR_CLIP_END:

    VoC_NOP();
    VoC_sw16inc_p(ACC0_LO, REG3, DEFAULT);

    VoC_lw16_d(REG2, GLOBAL_IIR_BAND_NUM1_ADDR);
    VoC_endloop0

    /*
    VoC_sw32_d(REG45,GLOBAL_IIR_BAND_XN_1_ADDR);
    VoC_sw32_d(REG67,GLOBAL_IIR_BAND_XN_2_ADDR);

    VoC_sw32_d(RL6,GLOBAL_IIR_BAND_YN_1_ADDR);
    VoC_sw32_d(RL7,GLOBAL_IIR_BAND_YN_2_ADDR);
    */
    VoC_lw16_d(REG0, GLOBAL_IIR_BAND_YN_1_USED_ADDR_ADDR);
    VoC_lw16_d(REG1, GLOBAL_IIR_BAND_YN_2_USED_ADDR_ADDR);

    VoC_sw32_p(RL6, REG0, DEFAULT);
    VoC_sw32_p(RL7, REG1, DEFAULT);

    VoC_lw16_d(REG2, GLOBAL_IIR_BAND_XN_1_USED_ADDR_ADDR);
    VoC_lw16_d(REG3, GLOBAL_IIR_BAND_XN_2_USED_ADDR_ADDR);

    VoC_sw32_p(REG45, REG2, DEFAULT);
    VoC_sw32_p(REG67, REG3, DEFAULT);

    VoC_return;
}

void Rda_ProcessIIR(void)
{
    VoC_push16(RA, DEFAULT);

    VoC_bez16_d(Rda_ProcessIIR_Reset, GLOBAL_RESET_ADDR)
    // init iir history data(size: 56 bytes)
    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_set_inc(REG0, GLOBAL_IIR_BAND0_XN_1_ADDR, 2);
    VoC_loop_i(0, 28)
    VoC_sw32inc_p(ACC0, REG0, DEFAULT);
    VoC_endloop0
Rda_ProcessIIR_Reset:

    // INIT IIR BAND COEFFS
    VoC_lw16i(REG0, m_IIRProcPara_band0_coeffs);
    VoC_lw16i(REG1, m_IIRProcPara_band0_coeffs+2);
    VoC_lw16i(REG2, m_IIRProcPara_band0_coeffs+4);
    VoC_lw16_d(REG3, DEC_OUTPUT_ADDR_ADDR);

    VoC_sw16_d(REG0, GLOBAL_IIR_BAND_NUM0_USED_ADDR_ADDR);
    VoC_sw16_d(REG1, GLOBAL_IIR_BAND_NUM2_USED_ADDR_ADDR);
    VoC_sw16_d(REG2, GLOBAL_IIR_BAND_DEN1_USED_ADDR_ADDR);
    VoC_sw16_d(REG3, IIR_INPUT_ADDR_ADDR);

    VoC_lw16i(REG0, GLOBAL_IIR_BAND0_YN_1_ADDR);
    VoC_lw16i(REG1, GLOBAL_IIR_BAND0_YN_2_ADDR);
    VoC_lw16i(REG2, GLOBAL_IIR_BAND0_XN_1_ADDR);
    VoC_lw16i(REG3, GLOBAL_IIR_BAND0_XN_2_ADDR);

    VoC_sw16_d(REG0, GLOBAL_IIR_BAND_YN_1_USED_ADDR_ADDR);
    VoC_sw16_d(REG1, GLOBAL_IIR_BAND_YN_2_USED_ADDR_ADDR);
    VoC_sw16_d(REG2, GLOBAL_IIR_BAND_XN_1_USED_ADDR_ADDR);
    VoC_sw16_d(REG3, GLOBAL_IIR_BAND_XN_2_USED_ADDR_ADDR);

    VoC_loop_i(1, 7)
    VoC_lw16_d(REG0, GLOBAL_IIR_BAND_NUM0_USED_ADDR_ADDR);
    VoC_lw16_d(REG1, GLOBAL_IIR_BAND_NUM2_USED_ADDR_ADDR);
    VoC_lw16_d(REG2, GLOBAL_IIR_BAND_DEN1_USED_ADDR_ADDR);

    VoC_lw32_p(ACC0, REG0, DEFAULT);
    VoC_lw16i_short(REG3, 6, IN_PARALLEL);

    VoC_lw32_p(ACC1, REG1, DEFAULT);
    VoC_add16_rr(REG0, REG0, REG3, IN_PARALLEL);

    VoC_lw32_p(RL7, REG2, DEFAULT);
    VoC_add16_rr(REG1, REG1, REG3, IN_PARALLEL);

    VoC_add16_rr(REG2, REG2, REG3, DEFAULT);

    VoC_sw32_d(ACC0, GLOBAL_IIR_BAND_NUM0_ADDR);
    VoC_sw32_d(ACC1, GLOBAL_IIR_BAND_NUM2_ADDR);
    VoC_sw32_d(RL7, GLOBAL_IIR_BAND_DEN1_ADDR);

    VoC_sw16_d(REG0, GLOBAL_IIR_BAND_NUM0_USED_ADDR_ADDR);
    VoC_sw16_d(REG1, GLOBAL_IIR_BAND_NUM2_USED_ADDR_ADDR);
    VoC_sw16_d(REG2, GLOBAL_IIR_BAND_DEN1_USED_ADDR_ADDR);

    VoC_jal(Rda_ProcessIIRBand);

    VoC_lw16_d(REG0, GLOBAL_IIR_BAND_YN_1_USED_ADDR_ADDR);
    VoC_lw16_d(REG1, GLOBAL_IIR_BAND_YN_2_USED_ADDR_ADDR);
    VoC_lw16_d(REG2, GLOBAL_IIR_BAND_XN_1_USED_ADDR_ADDR);
    VoC_lw16_d(REG3, GLOBAL_IIR_BAND_XN_2_USED_ADDR_ADDR);

    VoC_lw16i_short(REG4, 8, DEFAULT);

    VoC_add16_rr(REG0, REG0, REG4, DEFAULT);
    VoC_add16_rr(REG1, REG1, REG4, IN_PARALLEL);

    VoC_add16_rr(REG2, REG2, REG4, DEFAULT);
    VoC_add16_rr(REG3, REG3, REG4, IN_PARALLEL);

    VoC_sw16_d(REG0, GLOBAL_IIR_BAND_YN_1_USED_ADDR_ADDR);
    VoC_sw16_d(REG1, GLOBAL_IIR_BAND_YN_2_USED_ADDR_ADDR);
    VoC_sw16_d(REG2, GLOBAL_IIR_BAND_XN_1_USED_ADDR_ADDR);
    VoC_sw16_d(REG3, GLOBAL_IIR_BAND_XN_2_USED_ADDR_ADDR);
    VoC_endloop1

    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}