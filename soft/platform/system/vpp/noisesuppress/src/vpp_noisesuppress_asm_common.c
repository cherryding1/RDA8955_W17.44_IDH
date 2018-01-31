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




#include "vppp_noisesuppress_asm_common.h"
#include "vppp_noisesuppress_asm_map.h"
#include "vppp_noisesuppress_asm_sections.h"
#include "vppp_noisesuppress_asm_preprocess.h"

#include "voc2_library.h"
#include "voc2_define.h"

// VoC_directive: PARSER_OFF
#include <stdio.h>
// VoC_directive: PARSER_ON

#if 0
// GLOBAL

voc_struct VPP_NOISESUPPRESS_CFG_STRUCT __attribute__((export)),y
// ----------------
// common config
// ----------------
// voc_short GLOBAL_MODE
// voc_short GLOBAL_RESET
voc_word GLOBAL_NOISESUPPRESS_CONSTX_ADDR ,y
voc_word SPEEX_NS_Code_ExternalAddr_addr    ,y
voc_word  GLABAL_OUT_PARA
voc_short GLOBAL_MIC_NOISE_SUPPRESS_RESET_ADDR ,y
voc_short GLOBAL_RECEIVER_NOISE_SUPPRESS_RESET_ADDR ,y


voc_struct_end


voc_struct VPP_NOISESUPPRESS_STATUS_STRUCT __attribute__((export)),y

// ----------------
// common status
// ----------------
voc_short GLOBAL_USED_MODE
// ----------------
// hello status
// ----------------
voc_short  GLOBAL_NOISESUPPRESS_STATUS
voc_word GLOBAL_OUTSIZE

voc_struct_end


voc_short GLABAL_NOISESUPPRESS_RESET_ADDR,y
voc_short GLABAL_NOISESUPPRESS_INPUTADDR_ADDR,y
voc_short ENC_INPUT_ADDR_ADDR,160,y



#endif



void PREPROCESS_RUN_MAIN(void)
{
    // Init : clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);
SPEEX_AEC_MAIN:
    // Enable the SOF0 event only
    VoC_cfg_lg(CFG_WAKEUP_SOF0,CFG_WAKEUP_MASK);
    // Stall the execution and generate an interrupt
    VoC_cfg(CFG_CTRL_STALL|CFG_CTRL_IRQ_TO_XCPU);

// VoC_directive: PARSER_OFF
    // used for simulation
    VoC_cfg_stop;
// VoC_directive: PARSER_ON

    VoC_NOP();
    VoC_NOP();
    // Here the execution is woken up
    VoC_lw16i(SP16,SP16_ADDR);
    VoC_lw16i(SP32,SP32_ADDR);

    VoC_lw16i_short(REG0,0,DEFAULT);
    // Clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);
    // Disable the SOF0 event, enable DMAI event.
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_MASK);
    VoC_sw16_d(REG0,CFG_DMA_WRAP);

    VoC_jal(Coolsand_VoC_code_reload);

//VoC_directive:PARSER_OFF
//  VoC_lw16_d(REG0, SPEEX_PRE_FRAME_NUM);
//  VoC_bnez16_r(INIT_NOT_NEEDED, REG0)
//      VoC_add16_rd(REG0, REG0, SPEEX_CONST_1);
//      VoC_NOP();
//      VoC_sw16_d(REG0, SPEEX_PRE_FRAME_NUM);
//      VoC_jal(speex_preprocess_state_init);
//INIT_NOT_NEEDED:
//VoC_directive:PARSER_ON


    VoC_lw16_d(REG1,GLOBAL_MIC_NOISE_SUPPRESS_RESET_ADDR);
    VoC_lw16i(REG0,ENC_INPUT_ADDR_ADDR);
    VoC_sw16_d(REG1,GLABAL_NOISESUPPRESS_RESET_ADDR);

    //NOISE SUPPRESS
    VoC_jal(Rda_NoiseSuppress);
//  VoC_jal(speex_preprocess_run);

    VoC_lw16_d(REG1,GLABAL_NOISESUPPRESS_RESET_ADDR);
    VoC_NOP();
    VoC_sw16_d(REG1,GLOBAL_MIC_NOISE_SUPPRESS_RESET_ADDR);







//  VoC_lw16_d(REG1,GLOBAL_RECEIVER_NOISE_SUPPRESS_RESET_ADDR);
//  VoC_lw16_d(REG0,DEC_OUTPUT_ADDR_ADDR);
//  VoC_sw16_d(REG1,GLABAL_NOISESUPPRESS_RESET_ADDR);
//
//  //NOISE SUPPRESS
//  VoC_jal(Rda_NoiseSuppress);
//
//  VoC_lw16_d(REG1,GLABAL_NOISESUPPRESS_RESET_ADDR);
//  VoC_NOP();
//  VoC_sw16_d(REG1,GLOBAL_RECEIVER_NOISE_SUPPRESS_RESET_ADDR);


    VoC_jump(SPEEX_AEC_MAIN);
}

void Coolsand_VoC_code_reload(void)
{
    VoC_push16(RA,DEFAULT);

    /*****************  DMA SPEEX_NS Code ************************/
    VoC_lw32_d(REG45,SPEEX_NS_Code_ExternalAddr_addr);
    VoC_lw16i(REG2,SPEEX_AEC_START);
    VoC_lw16i(REG3,SECTION_SPEEX_NS_SIZE);
    VoC_jal(Rda_RunDma);

//  /*****************  DMA SPEEX_NS CONST X***************/
//  VoC_lw32_d(REG45,GLOBAL_NOISESUPPRESS_CONSTX_ADDR);
//  VoC_lw16i(REG2,SpxPpState_window/2);
//  VoC_lw16i(REG3,SPEEX_NOISESUPPRESS_CONSTX_SIZE);
//      VoC_jal(Rda_RunDma);


    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

void Rda_RunDma(void)
{

    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(REG45,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

//  VoC_sw16_d(REG2,CFG_DMA_LADDR);
//  VoC_sw16_d(REG3,CFG_DMA_SIZE);
//  VoC_sw32_d(ACC0,CFG_DMA_EADDR);
//  VoC_cfg(CFG_CTRL_STALL);
//  VoC_NOP();
//  VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    VoC_return;
}

