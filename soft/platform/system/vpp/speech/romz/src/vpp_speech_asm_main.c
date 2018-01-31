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

#if 0

voc_struct  VPP_SPEECH_DEC_IN_STRUCT __attribute__((export)),x
voc_short INPUT_BITS2_ADDR[26],x
voc_struct_end

voc_struct  VPP_SPEECH_ENC_OUT_STRUCT __attribute__((export)),x
voc_short OUTPUT_BITS2_ADDR,22,x
voc_struct_end

voc_struct VPP_SPEECH_ENC_IN_STRUCT __attribute__((export)),x
voc_short INPUT_SPEECH_BUFFER2_ADDR,160,x
voc_short INPUT_SPEECH_BUFFER1_ADDR,160,x
voc_struct_end

voc_struct   VPP_SPEECH_DEC_OUT_STRUCT   __attribute__((export)),x
voc_short OUTPUT_SPEECH_BUFFER2_ADDR,160,x
voc_short OUTPUT_SPEECH_BUFFER1_ADDR,160,x
voc_struct_end

#endif

#if 0
voc_short GLOBAL_SDF_SAMPLS_ADDR,64,x
#endif

#if 0

voc_struct VPP_SPEECH_INIT_STRUCT          __attribute__((export)) , y
voc_short GLOBAL_LOADED_VOCODER_ADDR              ,y
voc_short GLOBAL_LOADED_AMR_EFR_ADDR,               y
voc_short GLOBAL_LOADED_AMR_ENC_MODE_ADDR,          y
voc_short GLOBAL_LOADED_AMR_DEC_MODE_ADDR,          y
voc_short GLOBAL_VOCODER_SOURCE_CODE_ADDR_ADDR,6,   y
voc_short GLOBAL_AMR_EFR_SOURCE_CODE_ADDR_ADDR,4,        y
voc_short GLOBAL_AMR_ENC_MODE_SOURCE_CODE_ADDR_ADDR,16 ,y
voc_short GLOBAL_AMR_DEC_MODE_SOURCE_CODE_ADDR_ADDR,16, y
voc_short GLOBAL_RESET_ADDR                            ,y
voc_short GLOBAL_IRQGEN_ADDR                           ,y
voc_struct_end


voc_struct VPP_SPEECH_AUDIO_CFG_STRUCT  __attribute__((export)),y
voc_short GLOBAL_ECHO_CANCEL_ON,y
voc_short GLOBAL_ECHO_CANCEL_EXP_MU,y
voc_word  GLOBAL_ECHO_CANCEL_EXP_REL_MIN,y
voc_short GLOBAL_EchoRstThr_ADDR,2,y
voc_short GLOBAL_ENC_MICGAIN_ADDR,y
voc_short GLOBAL_DEC_SPKGAIN_ADDR,y
voc_short GLOBAL_SDF_ADDR,2,y
voc_short GLOBAL_MDF_ADDR,2,y
voc_struct_end

voc_short GLOBAL_RELOAD_MODE_ADDR,                  y
voc_short GLOBAL_AMR_EFR_RESET_ADDR,                  y

voc_short GLOBAL_OLD_MICGAIN_ADDR,y
voc_short GLOBAL_OLD_SPKGAIN_ADDR,y
voc_short GLOBAL_SDF_COEFFS_ADDR, 64,               y

voc_short GLOBAL_BFI_ADDR,                          y
voc_short GLOBAL_UFI_ADDR,                          y

voc_short GLOBAL_SID_ADDR,                          y
voc_short GLOBAL_TAF_ADDR,                          y
voc_short GLOBAL_DEC_MODE_ADDR,                     y

voc_short GLOBAL_ENC_MODE_ADDR,                     y
voc_short GLOBAL_ENC_USED_MODE_ADDR,                y

voc_short GLOBAL_EFR_FLAG_ADDR,                    y
voc_short DEC_RESET_FLAG_ADDR,                      y
voc_short DEC_MAIN_RESET_FLAG_OLD_ADDR,             y

#endif

#if 0
voc_short GLOBAL_MDF_SAMPLS_ADDR,64,x
#endif

#if 0
voc_short GLOBAL_MDF_COEFFS_ADDR,64,         y
voc_short GLOBAL_ECHO_CANCEL_COR_PWR,2,      y
voc_short GLOBAL_ECHO_CANCEL_ENC_PWR,4,      y

voc_short GLOBAL_ECHO_CANCEL_DEC_PWR,2,      y
voc_short GLOBAL_ECHO_CANCEL_SAMPLS_ADDR,296,y
voc_short GLOBAL_ECHO_CANCEL_COEFFS_ADDR,256,y

voc_short GLOBAL_VOCODER_RESET_ADDR,      y
voc_short GLOBAL_OUTPUT_Vad_ADDR,            y
voc_short GLOBAL_OUTPUT_SP_ADDR,             y
voc_short DEC_AMR_FRAME_TYPE_ADDR,           y

voc_short ENC_INPUT_ADDR_ADDR,y
voc_short DEC_OUTPUT_ADDR_ADDR,y
voc_short ENC_INPUT_ADDR_BAK_ADDR,y
voc_short DEC_OUTPUT_ADDR_BAK_ADDR,y
voc_short GLOBAL_DTX_ADDR,y
voc_short Reserved_ADDR,y
#endif






void CII_VOCODER_OPEN(void)
{
    // Clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);
    VoC_cfg(CFG_CTRL_IRQ_TO_XCPU|CFG_CTRL_CONTINUE);
    VoC_NOP();
}

void CII_VOCODER(void)
{


CII_VOCODER_EXIT:

    // Stall the execution
    VoC_cfg(CFG_CTRL_STALL);

    // VoC_directive: PARSER_OFF
    VoC_cfg_stop;
    // VoC_directive: PARSER_ON

    VoC_NOP();
    VoC_NOP();

    // Here the execution is woken up
    VoC_lw16i(SP16,SP16_ADDR);


    // The wakeup mask is set on ifc0/sof0 or on ifc1/sof1 only.
    // so that we collect the 0/1 events together.
    // For this reason we get the wakeup status
    // (and no the cause = maksed status) in REG7
    VoC_lw16_d(REG7,CFG_WAKEUP_STATUS);

    VoC_lw16i(SP32,SP32_ADDR);
    // clear the wakeup status
    VoC_sw16_d(REG7,CFG_WAKEUP_STATUS);

    // if the events are SW, shift by 4
    VoC_blt16_rd(is_hw_event,REG7,CONST_16_ADDR);
    VoC_shr16_ri(REG7,4,DEFAULT);
is_hw_event:

    VoC_bez16_r(CII_VOCODER_EXIT,REG7)

    VoC_lw32_d(ACC0,GLOBAL_ENC_MICGAIN_ADDR);
    VoC_lw16i_short(REG2,-1,DEFAULT);

    //if (reset_flag != 0)
    VoC_bez16_d(CII_MAIN_VOCODER,GLOBAL_RESET_ADDR)

    // ramp mode (00 or 11)
    // mic gain mode (00 or 11)
    VoC_sw32_d(ACC0,GLOBAL_OLD_MICGAIN_ADDR);

    VoC_sw16_d(REG2,GLOBAL_RELOAD_MODE_ADDR);
    VoC_sw16_d(REG2,GLOBAL_VOCODER_RESET_ADDR);
    VoC_sw16_d(REG2,GLOBAL_AMR_EFR_RESET_ADDR);

    VoC_lw16i(REG2, INPUT_SPEECH_BUFFER2_ADDR);
    VoC_lw16i(REG3, OUTPUT_SPEECH_BUFFER2_ADDR);

    VoC_lw16i(REG4, INPUT_SPEECH_BUFFER1_ADDR);
    VoC_lw16i(REG5, OUTPUT_SPEECH_BUFFER1_ADDR);

    VoC_sw32_d(REG23,ENC_INPUT_ADDR_ADDR);
    VoC_sw32_d(REG45,ENC_INPUT_ADDR_BAK_ADDR);

    // clear GLOBAL_SDF_SAMPLS_ADDR and GLOBAL_MDF_SAMPLS_ADDR buffers
    VoC_lw16i_set_inc(REG2, GLOBAL_SDF_SAMPLS_ADDR,2);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_loop_i(0,64)
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,IN_PARALLEL);

    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,IN_PARALLEL);

CII_MAIN_VOCODER:

    // SAVE PARAMETERS
    VoC_movreg16(REG6,REG7,DEFAULT);
    VoC_and16_ri(REG6,1);

    VoC_bez16_r(CII_DECODER_RUN,REG6)

    //store  Mode value, Not push to RAM_X to avoid stack crash in MR74 and MR795
    VoC_push32(REG67,DEFAULT);

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_bne16_rd(CII_ECHO_CANCEL,REG6,GLOBAL_ECHO_CANCEL_ON)

    VoC_jal(CII_MAIN_ECHO_CANCEL);
    VoC_sw32_d(ACC0,GLOBAL_ECHO_CANCEL_COR_PWR)

CII_ECHO_CANCEL:

    //REG6 : dtxOn,  REG7 : encModeNext
    VoC_lw32_d(REG67, INPUT_BITS2_ADDR)

    VoC_jal(CII_MAIN_ENCODER);

    VoC_pop32(REG67,DEFAULT);
    VoC_lw32_d(ACC0,GLOBAL_ECHO_CANCEL_COR_PWR);
    VoC_lw16i_short(REG6,2,DEFAULT);

    VoC_bngt32_rd(CII_DECODER_RUN,ACC0,GLOBAL_EchoRstThr_ADDR)

    VoC_sw16_d(REG6,GLOBAL_ECHO_CANCEL_ON);

CII_DECODER_RUN:

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_and16_ri(REG7,2);

    VoC_lw16i(REG0, INPUT_BITS2_ADDR);

    VoC_bez16_r(CII_DECODER100,REG7)

    // SAVE PARAMETERS
    VoC_lw32inc_p(REG67,REG0,DEFAULT);//mode

    VoC_lw32inc_p(REG23,REG0,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,DEFAULT);

    VoC_sw16_d(REG2,DEC_AMR_FRAME_TYPE_ADDR);

    VoC_sw16_d(REG3,GLOBAL_BFI_ADDR);
    VoC_lw32inc_p(REG23,REG0,DEFAULT);

    VoC_sw16_d(REG4,GLOBAL_SID_ADDR);
    VoC_sw16_d(REG5,GLOBAL_TAF_ADDR);

    VoC_sw16_d(REG2,GLOBAL_UFI_ADDR);

    VoC_jal(CII_MAIN_DECODER);

CII_DECODER100:

    VoC_lw16i_short(REG1,0xFFFF,DEFAULT);

    VoC_lw32z(ACC0,DEFAULT);

    VoC_be16_rd(CII_VOCODER_not_update_reset,REG1,(INPUT_BITS2_ADDR+1)) //mode = 0xFFFF :

    // clear the vocoder reset only if the mode is valid
    VoC_sw16_d(ACC0_LO,GLOBAL_VOCODER_RESET_ADDR);

CII_VOCODER_not_update_reset:

    // clear the general reset
    VoC_sw16_d(ACC0_LO,GLOBAL_RESET_ADDR);

    VoC_lw32_d(REG45,ENC_INPUT_ADDR_ADDR);
    VoC_lw32_d(REG23,ENC_INPUT_ADDR_BAK_ADDR);
    VoC_sw32_d(REG45,ENC_INPUT_ADDR_BAK_ADDR);
    VoC_sw32_d(REG23,ENC_INPUT_ADDR_ADDR);

    VoC_jump(CII_VOCODER_EXIT);

}


void CII_MAIN_ENCODER(void)
{

    VoC_push16(RA,DEFAULT);


    VoC_sw16_d(REG6,GLOBAL_DTX_ADDR);

    // ****************
    // function : CII_DigitalGain
    // input  : REG0 : old gain pointer,  INC0 = 2
    //          REG3 : work buffer,       INC3 = 2
    //          REG4 : new gain
    // output : none
    // used : REG01345
    // ****************

    VoC_lw16i_set_inc(REG0,GLOBAL_OLD_MICGAIN_ADDR,2);
    VoC_lw16d_set_inc(REG3,ENC_INPUT_ADDR_ADDR,2);
    VoC_lw16_d(REG4,GLOBAL_ENC_MICGAIN_ADDR);

    VoC_jal(CII_DigitalGain);

    // ****************
    // input :  REG3 : coeffs_buffers, INC3 = 2
    // output : none
    // used : REG34, ACC0 1
    // not modified : REG2
    // ****************

    VoC_lw16i_set_inc(REG3,(GLOBAL_MDF_COEFFS_ADDR),2);
    VoC_lw32_d(ACC1,GLOBAL_MDF_ADDR);

    VoC_jal(CII_LoadSpFormatFilter);

    // ****************
    // input : REG0 : filter samples, INC0 = 2
    //         REG1 : out buffer,     INC1 = 2
    //         REG2 : filter coeffs,  INC2 = 2
    //         REG3 : source buffer,  INC3 = 2
    // output : none
    // used : REG012345
    // ****************
    VoC_lw16i_set_inc(REG0,GLOBAL_MDF_SAMPLS_ADDR,2);
    VoC_lw16d_set_inc(REG1,ENC_INPUT_ADDR_ADDR,2);
    VoC_lw16i_set_inc(REG2,GLOBAL_MDF_COEFFS_ADDR,2);
    VoC_lw16d_set_inc(REG3,ENC_INPUT_ADDR_ADDR,2);

    VoC_jal(CII_RunAntiDistorsionFilter);

    // bypass in calibration mode
    VoC_be16_rd(CII_MAIN_ENCODER_end,REG7,CONST_0xFFFF_ADDR);

    // reload encoder
    VoC_jal(CII_reload_mode);

// VoC_directive: PARSER_OFF

    // main function call reset part in it
    VoC_bnlt16_rd(CII_MAIN_ENCODER101,REG7,CONST_0x1fff_ADDR); //0x80
    VoC_jal(CII_AMR_Encode);
    VoC_jump(CII_MAIN_ENCODER_end);
CII_MAIN_ENCODER101:
    VoC_bne16_rd(CII_MAIN_ENCODER102,REG7,CONST_0x1fff_ADDR);
    VoC_jal(CII_FR_Encode);
    VoC_jump(CII_MAIN_ENCODER_end);
CII_MAIN_ENCODER102:

// VoC_directive: PARSER_ON

    // case is no needed in binary simulation
    // because all functions have the same start address
    VoC_jal(CII_HR_Encode);

CII_MAIN_ENCODER_end:

    // output parameters
    VoC_lw16i_set_inc(REG0,OUTPUT_BITS2_ADDR,2);
    VoC_movreg16(REG7,REG6,DEFAULT);
    VoC_lw16_d(REG6,GLOBAL_RELOAD_MODE_ADDR);

    VoC_lw16_d(REG2,GLOBAL_OUTPUT_Vad_ADDR);
    VoC_lw16_d(REG3,GLOBAL_OUTPUT_SP_ADDR);         //sp_flag
    VoC_sw32inc_p(REG67,REG0,DEFAULT);  //enc_frame_type(only AMR) , enc_mode

    VoC_sw32inc_p(REG23,REG0,DEFAULT);
    VoC_pop16(RA,DEFAULT);

    // generate an interrupt if the flag is enabled
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_and16_rd(REG7,GLOBAL_IRQGEN_ADDR);
    VoC_bez16_r(no_irqgen_enc,REG7)
    VoC_cfg(CFG_CTRL_IRQ_TO_XCPU|CFG_CTRL_CONTINUE);
no_irqgen_enc:


    VoC_return
}



// VoC_directive: ALIGN

void CII_MAIN_DECODER(void)
{

    VoC_push16(RA,DEFAULT);

    // ****************
    // input :  REG3 : coeffs_buffers, INC3 = 2
    // output : none
    // used : REG34, ACC0 1
    // not modified : REG2
    // ****************

    VoC_lw16i_set_inc(REG3,(GLOBAL_SDF_COEFFS_ADDR),2);
    VoC_lw32_d(ACC1,GLOBAL_SDF_ADDR);

    VoC_jal(CII_LoadSpFormatFilter);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16d_set_inc(REG3,(DEC_OUTPUT_ADDR_ADDR),2);

    VoC_loop_i_short(80,DEFAULT);
    VoC_startloop0
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_endloop0

    // bypass in calibration mode
    VoC_be16_rd(CII_MAIN_DECODER_end,REG7,CONST_0xFFFF_ADDR);

    // reload decoder
    VoC_jal(CII_reload_mode);

    // main function call reset part in it
    VoC_bgt16_rd(CII_MAIN_DECODER102,REG7,CONST_0x177_ADDR);
    VoC_jal(CII_AMR_Decode);
    VoC_jump(CII_MAIN_DECODER_end);
CII_MAIN_DECODER102:
    VoC_bne16_rd(CII_MAIN_DECODER104,REG7,CONST_0x1fff_ADDR);
    VoC_jal(CII_FR_Decode);
    VoC_jump(CII_MAIN_DECODER_end);
CII_MAIN_DECODER104:
    VoC_jal(CII_HR_Decode);
CII_MAIN_DECODER_end:

    // ****************
    // input : REG0 : filter samples, INC0 = 2
    //         REG1 : target buffer,  INC1 = 2
    //         REG2 : filter coeffs,  INC2 = 2
    //         REG3 : source buffer,  INC3 = 2
    // output : none
    // used : REG012345
    // ****************

    VoC_lw16i_set_inc(REG0,GLOBAL_SDF_SAMPLS_ADDR,2);
    VoC_lw16d_set_inc(REG1,DEC_OUTPUT_ADDR_ADDR,2);
    VoC_lw16i_set_inc(REG2,GLOBAL_SDF_COEFFS_ADDR,2);
    VoC_lw16d_set_inc(REG3,DEC_OUTPUT_ADDR_ADDR,2);

    // ****************
    // input : REG0 : filter samples, INC0 = 2
    //         REG1 : target buffer,  INC1 = 2
    //         REG2 : filter coeffs,  INC2 = 2
    //         REG3 : source buffer,  INC3 = 2
    // output : none
    // used :
    // ****************

    VoC_jal(CII_RunAntiDistorsionFilter);

    // load source and destination pointers
    VoC_lw16i_set_inc(REG0,GLOBAL_OLD_SPKGAIN_ADDR,2);
    VoC_lw16d_set_inc(REG3,DEC_OUTPUT_ADDR_ADDR,2);
    VoC_lw16_d(REG4,GLOBAL_DEC_SPKGAIN_ADDR);

    // ****************
    // function : CII_DigitalGain
    // input  : REG0 : old gain pointer,  INC0 = 2
    //          REG3 : work buffer,       INC3 = 2
    //          REG4 : new gain
    // output : none
    // used : REG01345
    // ****************

    VoC_jal(CII_DigitalGain);

    VoC_pop16(RA,DEFAULT);

    // generate an interrupt if the flag is enabled
    VoC_lw16i_short(REG5,2,DEFAULT);
    VoC_and16_rd(REG5,GLOBAL_IRQGEN_ADDR);
    VoC_bez16_r(no_irqgen_dec,REG5)
    VoC_cfg(CFG_CTRL_IRQ_TO_XCPU|CFG_CTRL_CONTINUE);
no_irqgen_dec:

    VoC_return;

}



// ****************************
// input :
//         REG1 <- enc_in
//         REG0 <- dec_out
// output: ACC0 <- enc_corr_pwr
//         REG7 <- func_id = 2
// used : REG3467
// ****************************

#define echo_format32 6


void CII_MAIN_ECHO_CANCEL(void)
{

    VoC_lw16d_set_inc(REG0,DEC_OUTPUT_ADDR_BAK_ADDR,2);
    VoC_lw16d_set_inc(REG1,ENC_INPUT_ADDR_ADDR,2);

    VoC_lw16_d(REG7,GLOBAL_ECHO_CANCEL_ON);

    // ------------------------------------
    // copy history
    // ------------------------------------

    VoC_lw16i_set_inc(REG2,GLOBAL_ECHO_CANCEL_SAMPLS_ADDR,2);
    VoC_lw16i_set_inc(REG3,LOCAL_ECHO_CANCEL_SAMPLS_ADDR,2);

    VoC_lw32inc_p(ACC0,REG2,DEFAULT);
    VoC_loop_i(0,128)
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop0;


    // ------------------------------------
    // copy last dec_out samples to local
    // ------------------------------------

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_loop_i(0,80)
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop0;


    // ------------------------------------
    // reset if necessary
    // ------------------------------------

    VoC_be16_rd(echo_coeffs_no_init,REG7,CONST_1_ADDR)

    VoC_lw16i_set_inc(REG2,GLOBAL_ECHO_CANCEL_COEFFS_ADDR,2);
    VoC_lw16i_set_inc(REG3,LOCAL_ECHO_CANCEL_SAMPLS_ADDR,2);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_loop_i(0,128)
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,IN_PARALLEL);
    VoC_endloop0

    // init power to 0
    VoC_sw32_d(ACC0,GLOBAL_ECHO_CANCEL_DEC_PWR);

echo_coeffs_no_init:


    // ------------------------------------
    // save echo cancel history samples
    // ------------------------------------

    VoC_lw16i_set_inc(REG3,LOCAL_ECHO_CANCEL_SAMPLS_ADDR+160,2);
    VoC_lw16i_set_inc(REG2,GLOBAL_ECHO_CANCEL_SAMPLS_ADDR,2);

    VoC_lw32inc_p(ACC0,REG3,DEFAULT);
    VoC_loop_i(0,128)
    VoC_lw32inc_p(ACC0,REG3,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop0;

    // format32 is set to echo_format32 in the whole function
    VoC_lw16i_short(FORMAT32,echo_format32,DEFAULT);
    // compute enc_in PWR
    VoC_lw32z(ACC0,DEFAULT);
    VoC_movreg16(REG2,REG1,IN_PARALLEL);
    VoC_loop_i(0,80)
    VoC_bimac32inc_pp(REG2,REG2);
    VoC_endloop0;


    // ------------------------------------
    // do echo-cancellation on 160 samples
    // ------------------------------------

    VoC_lw16i_set_inc(REG2,LOCAL_ECHO_CANCEL_SAMPLS_ADDR+256,-2);

    // i = 160
    VoC_lw16i_set_inc(REG3,160,-1);

    // store enc_in PWR
    VoC_sw32_d(ACC0,GLOBAL_ECHO_CANCEL_ENC_PWR);

echo_sample_process:

    // &in2[i-256]
    VoC_sub16_rd(REG4,REG2,CONST_256_ADDR);

    // pwr
    VoC_lw32_d(ACC1,GLOBAL_ECHO_CANCEL_DEC_PWR);

    // path buffer
    VoC_lw16i_set_inc(REG0,GLOBAL_ECHO_CANCEL_COEFFS_ADDR,2);

    // sub in2[i-256]*in2[i-256] from pwr in ACC1 (!)
    VoC_inc_p(REG3,DEFAULT);
    VoC_msu32_pp(REG4,REG4,IN_PARALLEL);

    // add in2[i]*in2[i] to pwr in ACC1 (!)
    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_mac32_pp(REG2,REG2,IN_PARALLEL);

    // save &in2[i] & i
    VoC_push32(REG23,DEFAULT);

    // STACK16           STACK32
    // -------------------------
    //                   &in2[i] & i

    // compute conv
    VoC_aligninc_pp(REG2,REG2,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_loop_i(1,128)
    VoC_bimac32inc_pp(REG0,REG2);
    VoC_endloop1;

    // save current power
    VoC_sw32_d(ACC1,GLOBAL_ECHO_CANCEL_DEC_PWR);

    // format conv
    VoC_shr32_ri(ACC0,-1-echo_format32,DEFAULT);

    // out[i] -= extract_hi(conv), pwr_thrsh
    VoC_movreg16(REG0,ACC0_HI,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_sub16_pr(REG0,REG1,REG0,DEFAULT);
    VoC_lw16i_short(ACC0_LO,1,IN_PARALLEL);

    // compute pwr thresholds
    VoC_lw32_d(REG67,GLOBAL_ECHO_CANCEL_ENC_PWR);
    VoC_lw32_d(REG45,GLOBAL_ECHO_CANCEL_EXP_REL_MIN);
    VoC_shr32_rr(REG67,REG4,DEFAULT);
    VoC_shr32_rr(ACC0,REG5,DEFAULT);

    // save out[i]
    VoC_sw16inc_p(REG0,REG1,IN_PARALLEL);

    // if (pwr > enc_pwr) && (pwr>pwr_thrsh)
    VoC_bngt32_rr(echo_canceller_01,ACC1,REG67);
    VoC_bngt32_rr(echo_canceller_01,ACC1,ACC0);

    // normalization value (inc3 = -1)
    VoC_lw16i_short(REG3,0,DEFAULT);

    // -normalize_pwr and error
    VoC_bez32_r(NORM_PWR_EXIT,ACC1)
    VoC_loop_i(1,31)
    VoC_bnlt32_rd(NORM_PWR_EXIT,ACC1,CONST_0x40000000L_ADDR)
    VoC_shr32_ri(ACC1,-1,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_endloop1
NORM_PWR_EXIT:

    // extract_hi(pwr) to REG1, out[i++]
    VoC_movreg16(REG1,ACC1_HI,DEFAULT);
    VoC_push16(REG1,IN_PARALLEL);

    // STACK16           STACK32
    // -------------------------
    // &out[i+1]         &in2[i] & i

    /**********************
     * div_s
     * input  : REG0,REG1
     * output : REG2
     * other used : RL6,RL7
     **********************/

    VoC_bnltz16_r(echo_conv_pos,REG0);
    VoC_sub16_dr(REG0,CONST_0_ADDR,REG0);
    VoC_jal(CII_DIV_S);
    VoC_sub16_dr(REG2,CONST_0_ADDR,REG2);
    VoC_jump(echo_conv_neg);
echo_conv_pos:
    VoC_jal(CII_DIV_S);
echo_conv_neg:

    // division result in REG2
    // &in2[i]
    VoC_lw32_sd(REG01,0,DEFAULT);

    // normalize = exp_mu-normalize_pwr
    VoC_lw16i_short(INC0,-2,IN_PARALLEL);
    VoC_add16_rd(REG7,REG3,GLOBAL_ECHO_CANCEL_EXP_MU);

    // path[j] = path[j] + 0.5 + (err*in2[i-j]  << normalize)
    VoC_lw32_d(RL6,CONST_0x00008000L_ADDR);
    VoC_aligninc_pp(REG0,REG0,DEFAULT);
    VoC_movreg16(RL7_LO,REG7,IN_PARALLEL);

    // path
    VoC_lw16i_set_inc(REG1,GLOBAL_ECHO_CANCEL_COEFFS_ADDR,2);
    VoC_lw16i_set_inc(REG3,GLOBAL_ECHO_CANCEL_COEFFS_ADDR,2);

    // mult & accumulate
    VoC_multf32inc_rp(ACC0,REG2,REG0,DEFAULT);
    VoC_multf32inc_rp(ACC1,REG2,REG0,IN_PARALLEL);

    VoC_loop_i(1,128)

    VoC_shr32_rr(ACC0,REG7,DEFAULT);
    VoC_shr32_rr(ACC1,REG7,IN_PARALLEL);

    VoC_add32_rr(REG45,ACC0,RL6,DEFAULT);
    VoC_add32_rr(REG67,ACC1,RL6,IN_PARALLEL);

    VoC_add16inc_rp(REG4,REG5,REG1,DEFAULT);
    VoC_add16inc_rp(REG5,REG7,REG1,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG2,REG0,DEFAULT);
    VoC_multf32inc_rp(ACC1,REG2,REG0,IN_PARALLEL);

    exit_on_warnings = OFF;
    // save previous
    VoC_sw32inc_p(REG45,REG3,DEFAULT);
    exit_on_warnings = ON;

    // restore normalize
    VoC_movreg16(REG7,RL7_LO,IN_PARALLEL);

    VoC_endloop1;

    // unpile &out[i+1]
    VoC_pop16(REG1,DEFAULT);
    VoC_lw16i_short(INC2,-2,DEFAULT);
    // unpile RA
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);

echo_canceller_01:

    // STACK16           STACK32
    // -------------------------
    //                   &in2[i] & i

    // unpile stack
    VoC_pop32(REG23,DEFAULT);
    VoC_add16_rd(REG2,REG2,CONST_1_ADDR);

    // while i > 0
    VoC_bnez16_r(echo_sample_process,REG3);


    // compute corrected pwr
    VoC_sub16_rd(REG1,REG1,CONST_160_ADDR);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_loop_i(0,80)
    VoC_bimac32inc_pp(REG1,REG1);
    VoC_endloop0

    // format32
    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
    // Echo Corr Pwr in ACCO
    // fct_id = echo flag = 2
    VoC_lw16i_short(REG7,2,DEFAULT);
    VoC_NOP();
    VoC_cfg(CFG_CTRL_STOP);
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
    VoC_bnlt32_rd(G_PITCH_NORM_L_3_EXIT,ACC0,CONST_0x40000000L_ADDR)
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


// ****************
// input :  REG3 : coeffs_buffers, INC3 = 2
// output : none
// used : REG34, ACC0 1
// not modified : REG2
// ****************

void CII_LoadSpFormatFilter(void)
{
    VoC_lw16i_short(FORMAT16,(15-16),DEFAULT);
    VoC_lw16i_short(FORMAT32,-1,DEFAULT);

    // execute only when general reset
    VoC_bez16_d(DONE_MSDF,GLOBAL_RESET_ADDR)

    // if pointer == NULL, apply ID filter
    VoC_bnez32_r(LOAD_MSDF,ACC1);

    VoC_lw32z(RL7,DEFAULT);
    VoC_lw32_d(ACC1,CONST_0x40000000L_ADDR);

    VoC_loop_i_short(15,DEFAULT)
    VoC_startloop0
    VoC_sw32inc_p(RL7,REG3,DEFAULT);
    VoC_endloop0;

    VoC_sw32_p(ACC1,REG3,DEFAULT);

    VoC_jump(DONE_MSDF);

LOAD_MSDF:
    // configure addresses of burst
    VoC_shr16_ri(REG3,1,DEFAULT);
    VoC_cfg_lg(SECTION_MSDF_SIZE,CFG_DMA_SIZE);
    // store local address
    VoC_sw16_d(REG3,CFG_DMA_LADDR);  // local address
    // store extern address
    VoC_sw32_d(ACC1,CFG_DMA_EADDR); // read non-single access

goto_sleep0:

    VoC_cfg(CFG_CTRL_STALL);

    // check we woke ON the event DMAI
    VoC_lw16i_short(REG3,CFG_WAKEUP_DMAI,DEFAULT);
    VoC_and16_rd(REG3,CFG_WAKEUP_CAUSE);

    VoC_bez16_r(goto_sleep0,REG3);
    // clear the event
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);


DONE_MSDF:
    VoC_NOP();
    VoC_return;
}


void CII_encoder_homing_frame_test(void)
{
    VoC_lw16i_short(REG4,0,DEFAULT);

    VoC_loop_i(0,80)
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_bne32_rd(CII_encoder_homing_frame_test_01,REG67,CONST_0x00080008_ADDR)
    VoC_endloop0

    VoC_lw16i_short(REG4,1,DEFAULT);

CII_encoder_homing_frame_test_01:

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
    VoC_lw16i_short(REG2,0,DEFAULT);

    VoC_be16_rr(NO_SECTION_RESET,REG6,REG4);
    VoC_lw16i_short(REG2,1,DEFAULT);
NO_SECTION_RESET:

    // reset if vocoder reload is detected now or during the previous turn
    VoC_or16_rd(REG2,GLOBAL_VOCODER_RESET_ADDR);

    VoC_sw16_d(REG7,GLOBAL_RELOAD_MODE_ADDR);
    VoC_lw16i_set_inc(REG3,GLOBAL_DEC_MODE_ADDR,1);

    // keep reset flag & new mode for next turn
    VoC_sw16_d(REG2,GLOBAL_VOCODER_RESET_ADDR);


    // ******************
    // AMR MODE FLAGS
    // ******************

    // mode flag for AMR modes
    VoC_loop_i_short(2,DEFAULT)

    VoC_startloop0

    VoC_lw16i_short(REG6,0xf,DEFAULT);
    VoC_and16_rr(REG6,REG7,DEFAULT);

    // check if a section was reloaded
    VoC_be16_rd(SECTION_4_DONE,REG6,CONST_8_ADDR)

    // mode flags
    VoC_sw16inc_p(REG6,REG3,DEFAULT);

SECTION_4_DONE:

    VoC_shr16_ri(REG7,4,DEFAULT);

    VoC_lw16i_set_inc(REG3,GLOBAL_ENC_MODE_ADDR,1);
    VoC_endloop0;

    VoC_sw16_d(REG6,GLOBAL_ENC_USED_MODE_ADDR);


    // ******************
    // EFR MODE FLAG
    // ******************

    // mode flag for EFR
    VoC_lw16i_short(REG6,0xf,DEFAULT);
    VoC_and16_rr(REG6,REG7,DEFAULT);

    // check if a section was reloaded
    VoC_be16_rd(SECTION_2_DONE,REG6,CONST_15_ADDR)
    VoC_sw16_d(REG6,GLOBAL_EFR_FLAG_ADDR);
SECTION_2_DONE:


    VoC_return;

}




void CII_reload_mode(void)
{

    // AMR (decoder mode i) : 0x00fj
    // AMR (encoder mode j) : 0x00jf

    // EFR                  : 0x0177
    // FR                   : 0x1fff
    // HR                   : 0x2fff

    // Bypass               : 0xffff
    // f <=> don't care

    VoC_push16(REG7,DEFAULT);
    VoC_push16(RA,DEFAULT);

    // ******************
    // SIDE INFO
    // ******************

    VoC_jal(CII_reload_sideinfo);
    VoC_lw16_sd(REG7,1,DEFAULT);

    // ******************
    // AMR DECODER SECTION
    // ******************

    VoC_lw16i(REG6, SECTION_AMR_DEC_SIZE );         // transfer size
    VoC_lw16i(REG5,SECTION_4_START);                            // local addr
    VoC_lw16i(REG4,GLOBAL_LOADED_AMR_DEC_MODE_ADDR);            // &curr_mode
    VoC_lw16i(REG3,GLOBAL_AMR_DEC_MODE_SOURCE_CODE_ADDR_ADDR);  // reload table

    // ****************************
    // func: CII_reload_section
    // input : REG3 <- reload table ptr
    //         REG4 <- mode info ptr
    //         REG5 <- local section ptr
    //         REG6 <- transfer size
    //         REG7 <- mode flag
    // output: REG1 <- new mode (if no reload = 0xf)
    // used : REG1345
    // ****************************

    // reload section 1
    VoC_jal(CII_reload_section);

    // ******************
    // AMR ENCODER SECTION
    // ******************

    VoC_lw16i(REG6,SECTION_AMR_ENC_SIZE );      // transfer size
    VoC_lw16i(REG5,SECTION_3_START);                            // local addr
    VoC_lw16i(REG4,GLOBAL_LOADED_AMR_ENC_MODE_ADDR);            // &curr_mode
    VoC_lw16i(REG3,GLOBAL_AMR_ENC_MODE_SOURCE_CODE_ADDR_ADDR);  // reload table

    // ****************************
    // func: CII_reload_section
    // input : REG3 <- reload table ptr
    //         REG4 <- mode info ptr
    //         REG5 <- local section ptr
    //         REG6 <- transfer size
    //         REG7 <- mode flag
    // output: REG1 <- new mode (if no reload = 0xf)
    // used : REG1345
    // ****************************

    // reload section 1
    VoC_jal(CII_reload_section);

    // ******************
    // AMR or EFR SECTION
    // ******************

    VoC_lw16i(REG6,SECTION_AMR_SPE_SIZE );  // transfer size
    VoC_lw16i(REG5,SECTION_2_START);                        // local addr
    VoC_lw16i(REG4,GLOBAL_LOADED_AMR_EFR_ADDR);             // &curr_amr_or_efr
    VoC_lw16i(REG3,GLOBAL_AMR_EFR_SOURCE_CODE_ADDR_ADDR);   // &reload table

    // ****************************
    // func: CII_reload_section
    // input : REG3 <- reload table ptr
    //         REG4 <- mode info ptr
    //         REG5 <- local section ptr
    //         REG6 <- transfer size
    //         REG7 <- mode flag
    // output: REG1 <- new mode (if no reload = 0xf)
    // used : REG1345
    // ****************************

    // reload section 2
    VoC_jal(CII_reload_section);

    // ******************
    // VOCODER SECTION
    // ******************

    // reload vocoder FR, HR
    VoC_lw16i(REG6,(SECTION_2_START-SECTION_1_START));  // transfer size for EFR or AMR
    VoC_bez16_r(AMR_EFR_COMMON_LOAD,REG7)
    VoC_lw16i(REG6, SECTION_VOCODER_SIZE ); // transfer size for the rest
AMR_EFR_COMMON_LOAD:


    VoC_lw16i(REG5,SECTION_1_START);            // local addr
    VoC_lw16i(REG4,GLOBAL_LOADED_VOCODER_ADDR);     // &curr_vocoder
    VoC_lw16i(REG3,GLOBAL_VOCODER_SOURCE_CODE_ADDR_ADDR);   // &reload table

    // ****************************
    // func: CII_reload_section
    // input : REG3 <- reload table ptr
    //         REG4 <- mode info ptr
    //         REG5 <- local section ptr
    //         REG6 <- transfer size
    //         REG7 <- mode flag
    // output: REG1 <- new mode (if no reload = 0xf)
    // used : REG1345
    // ****************************

    // reload section 1
    VoC_jal(CII_reload_section);

    VoC_pop16(RA,DEFAULT);
    VoC_pop16(REG7,DEFAULT);
    VoC_return;

}


// ****************************
// func: CII_prepare_reload
// input : REG1 <- new mode
//         REG3 <- reload table ptr
//         REG4 <- mode info ptr
//         REG5 <- local section ptr
// output : none
// used : REG1345
// ****************************


void CII_reload_section(void)
{
    // section flag
    VoC_lw16i_short(REG1,0xf,DEFAULT);
    VoC_and16_rr(REG1,REG7,DEFAULT);
    VoC_shr16_ri(REG7,4,DEFAULT);

    // curr_mode in REG0
    VoC_lw16_p(REG0,REG4,IN_PARALLEL);

    // update mode info
    VoC_sw16_p(REG1,REG4,DEFAULT);

    // check if reload is necessary
    VoC_be16_rd(NO_RELOAD,REG1,CONST_15_ADDR)
    VoC_be16_rr(NO_RELOAD,REG1,REG0)

    // &reload_table_addr[mode]
    VoC_add16_rr(REG3,REG3,REG1,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG1,DEFAULT);

    VoC_sw16_d(REG5,CFG_DMA_LADDR);    // store local
    VoC_lw32_p(REG45,REG3,DEFAULT);             // get extern

    VoC_sw16_d(REG6,CFG_DMA_SIZE);
    VoC_sw32_d(REG45,CFG_DMA_EADDR); // read non-single access


goto_sleep1:

    VoC_cfg(CFG_CTRL_STALL);

    // check we woke ON the event DMAI
    VoC_lw16i_short(REG3,CFG_WAKEUP_DMAI,DEFAULT);
    VoC_and16_rd(REG3,CFG_WAKEUP_CAUSE);

    VoC_bez16_r(goto_sleep1,REG3);
    // clear the event
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);


    // VoC_directive: PARSER_OFF
    printf("Copying %d bursts of 4 from extern %08x to local %04x address\n",REGS[REG6].reg);
    // VoC_directive: PARSER_ON

NO_RELOAD:
    VoC_return;


}

// ****************
// input : REG0 : filter samples, INC0 = 2
//         REG3 : source buffer,  INC3 = 2
//         REG4 : nb_tap
// output : none
// used : REG02345, ACC01
// ****************


void CII_shift_filter (void)
{

    VoC_push16(REG0,DEFAULT);
    VoC_sub16_dr(REG5,CONST_80_ADDR,REG4);

    // pointers
    VoC_lw16i_set_inc(REG2,MIC_FILT_ADDR,2);    // in RAM_X

    // copy old frame values in the temp buffer
    VoC_loop_r_short(REG4,DEFAULT)
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop0

    // copy new frame values in the temp buffer
    VoC_loop_r_short(REG5,DEFAULT)
    VoC_lw32inc_p(ACC0,REG3,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG3,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop0

    // continue copy new frame values in the temp buffer
    // save last values of the new frame
    VoC_pop16(REG0,DEFAULT);

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_startloop0
    VoC_movreg32(ACC1,ACC0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG3,DEFAULT);

    exit_on_warnings = OFF;
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    exit_on_warnings = ON;
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_endloop0

    VoC_return;

}




// ****************
// input : REG0 : filter samples, INC0 = 2
//         REG1 : out buffer,     INC1 = 2
//         REG2 : filter coeffs,  INC2 = 2
//         REG3 : source buffer,  INC3 = 2
// output : none
// used :
// ****************


void CII_RunAntiDistorsionFilter (void)
{
    // ---------------------------------
    // copy and set the appropriate gain
    // gain 1.0 <=> 0x4000
    // ---------------------------------

    VoC_push16(RA,DEFAULT);
    VoC_push16(REG2,DEFAULT);
    VoC_push16(REG1,DEFAULT);


    // STACK 16         STACK32
    // ------------------------------------
    // RA               REG45
    // filt_coeffs
    // out_buffer

    VoC_lw16i_short(REG4,16,DEFAULT);

    // ****************
    // input : REG0 : filter samples, INC0 = 2
    //         REG3 : source buffer,  INC3 = 2
    //         REG4 : nb_tap
    // output : none
    // used : REG02345, ACC01
    // ****************

    VoC_jal(CII_shift_filter);


    // ----------------------------------------
    // apply filter and store in the out buffer
    // ----------------------------------------


    // STACK 16         STACK32
    // ------------------------------------
    // filt_coeffs
    // out_buffer


    // filt_coeffs & out_buffer
    VoC_lw16_sd(REG2,1,DEFAULT);                    // in RAM_Y
    VoC_pop16(REG3,DEFAULT);
    // out_buffer
    VoC_lw16i_short(INC3,1,IN_PARALLEL);

    // STACK 16         STACK32
    // ------------------------------------
    // filt_coeffs



    VoC_lw16i_short(FORMAT32,3,DEFAULT);
    VoC_lw16i_set_inc(REG0,MIC_FILT_ADDR+1,1);      // in RAM_X
    VoC_lw16i_set_inc(REG1,MIC_FILT_ADDR+1,2);      // in RAM_X

    VoC_loop_i(1,160)

    VoC_aligninc_pp(REG1,REG1,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);

    VoC_loop_i_short(16,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_bimac32inc_pp(REG1,REG2);
    VoC_endloop0

    VoC_lw16_sd(REG2,0,DEFAULT);                // in RAM_Y
    VoC_shr32_ri(ACC0,-5,DEFAULT);
    VoC_movreg16(REG1,REG0,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(ACC0_HI,REG3,DEFAULT);
    exit_on_warnings = ON;

    VoC_endloop1

    VoC_pop16(REG0,DEFAULT);
    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
    VoC_return;


}




// ****************
// function : CII_DigitalGain
// input  : REG0 : old gain pointer,  INC0 = 2
//          REG3 : work buffer,       INC3 = 2
//          REG4 : new gain
// output : none
// used : REG01345
// ****************

void CII_DigitalGain (void)
{

    // get old gain
    VoC_lw16_p(REG1,REG0,DEFAULT);

    // old gain next = new gain
    VoC_sw16_p(REG4,REG0,DEFAULT);

    // compute gain diff (new-old)
    VoC_sub16_rr(REG4,REG4,REG1,DEFAULT);

    // start point : old ramp, end point new ramp
    VoC_shr16_ri(REG4,5,DEFAULT);

    VoC_movreg16(INC1,REG4,DEFAULT);
    // ****************
    // function : CII_DigitalRamp
    // input  : REG1 : old gain,  INC1 = gain_diff >> 5
    //          REG3 : work buffer,  INC3 = 2, INC0 = 2
    // output : none
    // used : REG01345
    // ****************

    VoC_lw16i_short(FORMAT16,(10-16),DEFAULT);
    // duplicate work buffer
    VoC_movreg16(REG0,REG3,DEFAULT);

    VoC_multf16inc_rp(REG4,REG1,REG3,DEFAULT);
    VoC_multf16inc_rp(REG5,REG1,REG3,IN_PARALLEL);

    // compute samples
    VoC_loop_i_short(32,DEFAULT)
    VoC_startloop0
    VoC_multf16inc_rp(REG4,REG1,REG3,DEFAULT);
    VoC_multf16inc_rp(REG5,REG1,REG3,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    exit_on_warnings = ON;
    VoC_inc_p(REG1,DEFAULT);
    VoC_endloop0

    VoC_loop_i_short(48,DEFAULT)
    VoC_startloop0
    VoC_multf16inc_rp(REG4,REG1,REG3,DEFAULT);
    VoC_multf16inc_rp(REG5,REG1,REG3,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    exit_on_warnings = ON;
    VoC_NOP();
    VoC_endloop0

    // last mult not saved...
// removed this instruction for below is finish of vocoder or init in CII_LoadSpFormatFilter
//  VoC_lw16i_short(FORMAT16,(15-16),DEFAULT);

//  VoC_NOP();
    VoC_return;
}






