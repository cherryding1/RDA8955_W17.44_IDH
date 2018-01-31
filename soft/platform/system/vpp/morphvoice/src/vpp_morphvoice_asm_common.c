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




#include "vppp_morphvoice_asm_common.h"
#include "vppp_morphvoice_asm_map.h"
#include "vppp_morphvoice_asm_sections.h"

#include "voc2_library.h"
#include "voc2_define.h"


#if 0

// voc_enum mode
voc_alias VPP_HELLO_MODE_NO              -1, x
voc_alias VPP_HELLO_MODE_HI               0, x
voc_alias VPP_HELLO_MODE_BYE              1, x
// voc_enum_end

// voc_enum error
voc_alias VPP_HELLO_ERROR_NO              0, x
voc_alias VPP_HELLO_ERROR_YES            -1, x
// voc_enum_end

// voc_enum hello status
voc_alias VPP_HELLO_SAYS_HI          0x0111, x
voc_alias VPP_HELLO_SAYS_BYE         0x0B1E, x
// voc_enum_end


voc_struct VPP_HELLO_CFG_STRUCT __attribute__((export)),y

// ----------------
// common config
// ----------------
voc_short GLOBAL_MODE
voc_short GLOBAL_RESET
voc_short PITCH_SHIFT_ADDR
voc_short RESEVER10_ADDR

voc_struct_end


voc_struct VPP_HELLO_STATUS_STRUCT __attribute__((export)),y

// ----------------
// common status
// ----------------
voc_short GLOBAL_USED_MODE
voc_short GLOBAL_ERR_STATUS

// ----------------
// hello status
// ----------------
voc_word  GLOBAL_HELLO_STATUS

voc_struct_end


voc_struct VPP_HELLO_CODE_CFG_STRUCT __attribute__((export)),y

voc_word  GLOBAL_HI_CODE_PTR
voc_word  GLOBAL_CONST_PTR

voc_struct_end







voc_struct VPP_HELLO_CONST_STRUCT,x

voc_word VPP_HELLO_CONST_SAYS_HI
voc_word VPP_HELLO_CONST_SAYS_BYE

voc_struct_end


voc_short COS_TABLE_ADDR[129],x
voc_short TAN_TABLE_ADDR[33],x


voc_short ingSumPhase_ADDR[129],x
voc_short gRover_ADDR,x



#endif

// ----------------------------
// fct : vpp_HelloMain
// in  :
// out :
// not modified :
// used : all
// ----------------------------

void vpp_HelloMain(void)
{
    // set the stack pointers
    VoC_lw16i(SP16,SP16_ADDR);
    VoC_lw16i(SP32,SP32_ADDR);

    // Init : clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);

HELLO_LABEL_MAIN_LOOP:

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

    // Clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);

    // Disable all programed events, enable DMAI event.
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_MASK);

    VoC_jal(vpp_HelloCodeConstReload);

    VoC_jal(CII_smbPitchShift);

    VoC_jump(HELLO_LABEL_MAIN_LOOP);

}


// ----------------------------
// fct : vpp_HelloCodeConstReload
// in :  REG0 -> GLOBAL_MODE
// not modified : REG0
// unused : REG45, RL67, ACC1
// ----------------------------
void vpp_HelloCodeConstReload(void)
{
    // old (status) mode

    VoC_push16(RA,DEFAULT);

    VoC_lw32_d(ACC0,GLOBAL_CONST_PTR);

    VoC_lw16i(REG2,COS_TABLE_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_CONST_SIZE);
    VoC_jal(vpp_HelloRunDma);

    // check which section to reload
    VoC_lw32_d(ACC0,GLOBAL_HI_CODE_PTR);
    VoC_lw16i(REG2,SECTION_HI_START);
    VoC_lw16i(REG3,SECTION_HI_SIZE);

    VoC_jal(vpp_HelloRunDma);

    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

    VoC_return;
}


// ----------------------------
// fct : vpp_HelloRunDma
// in :  REG2 -> LADDR
//       REG3 -> SIZE
//       ACC0 -> EADDR
// modified : none
// ----------------------------
void vpp_HelloRunDma(void)
{
    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    VoC_return;
}

