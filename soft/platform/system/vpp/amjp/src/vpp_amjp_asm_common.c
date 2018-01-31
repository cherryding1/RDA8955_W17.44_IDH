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




#include <stdio.h>
#include <string.h>

#include "vppp_amjp_asm_aac.h"
#include "vppp_amjp_asm_mp3.h"
#include "vppp_amjp_asm_jpeg.h"
#include "vppp_amjp_asm_zoom.h"
#include "vppp_amjp_asm_amr.h"
#include "vppp_amjp_asm_map.h"
#include "vppp_amjp_asm_sections.h"

#include "voc2_library.h"
#include "voc2_define.h"



#if 0

// voc_enum audio mode
voc_alias VPP_AMJP_MODE_NO              0x0, x
voc_alias VPP_AMJP_MODE_AMR             0x1, x
voc_alias VPP_AMJP_MODE_MP3             0x2, x
voc_alias VPP_AMJP_MODE_AAC             0x3, x
voc_alias VPP_AMJP_MODE_WMA             0x4, x
// voc_enum_end

// voc_enum video mode
voc_alias VPP_AMJP_MODE_JPEG          0x100, x
voc_alias VPP_AMJP_MODE_ZOOM          0x200, x
// voc_enum_end

// voc_enum stream id
voc_alias VPP_AMJP_STRM_ID_NO           0x0, x
voc_alias VPP_AMJP_STRM_ID_AUDIO        0x1, x
voc_alias VPP_AMJP_STRM_ID_VIDEO        0x2, x
voc_alias VPP_AMJP_STRM_ID_INIT         0x3, x
// voc_enum_end

// voc_enum rotate mode
voc_alias VPP_AMJP_ROTATE_NO            0x0, x
voc_alias VPP_AMJP_ROTATE_90            0x1, x
voc_alias VPP_AMJP_ROTATE_180           0x2, x
voc_alias VPP_AMJP_ROTATE_270           0x3, x
// voc_enum_end

// voc_enum equalizer mode
voc_alias VPP_AMJP_EQUALIZER_OFF     0xffff, x
voc_alias VPP_AMJP_EQUALIZER_NORMAL     0x0, x
voc_alias VPP_AMJP_EQUALIZER_BASS       0x1, x
voc_alias VPP_AMJP_EQUALIZER_DANCE      0x2, x
voc_alias VPP_AMJP_EQUALIZER_CLASSICAL  0x3, x
voc_alias VPP_AMJP_EQUALIZER_TREBLE     0x4, x
voc_alias VPP_AMJP_EQUALIZER_PARTY      0x5, x
voc_alias VPP_AMJP_EQUALIZER_POP        0x6, x
voc_alias VPP_AMJP_EQUALIZER_ROCK       0x7, x
voc_alias VPP_AMJP_EQUALIZER_CUSTOM     0x8, x
// voc_enum_end

// voc_enum video buffer mode (swap or circular)
voc_alias VPP_AMJP_BUF_MODE_CIRC        0x0, x
voc_alias VPP_AMJP_BUF_MODE_SWAP        0x1, x
// voc_enum_end


voc_struct VPP_AMJP_CFG_STRUCT __attribute__((export)),y

// general cfg
voc_short GLOBAL_MODE_IN
voc_short GLOBAL_RESET

// audio cfg
voc_short GLOBAL_EQ_TYPE
voc_short GLOBAL_CFG_RESERVED
voc_word  GLOBAL_STRM_BUF_IN_START
voc_word  GLOBAL_STRM_BUF_IN_END
voc_word  GLOBAL_STRM_BUF_OUT_PTR
voc_word  GLOBAL_STRM_BUF_IFC_PTR

// jpeg video cfg
voc_short GLOBAL_ZOOM_MODE
voc_short GLOBAL_ROTATE_MODE
voc_short GLOBAL_RESIZE_TO_WIDTH
voc_short GLOBAL_VID_BUF_MODE
voc_word  GLOBAL_VID_BUF_IN_START
voc_word  GLOBAL_VID_BUF_IN_END
voc_word  GLOBAL_VID_BUF_OUT_PTR

// zoom video cfg
voc_short Source_width_ADDR
voc_short Source_height_ADDR
voc_short Cut_width_ADDR
voc_short Cut_height_ADDR
voc_short Zoomed_width_ADDR
voc_short Zoomed_height_ADDR
voc_word  INPUT_ADDR_U_ADDR
voc_word  INPUT_ADDR_V_ADDR

voc_struct_end

voc_struct VPP_AMJP_STATUS_STRUCT __attribute__((export)),y

// general status
voc_short GLOBAL_MODE_OUT           ,y
voc_short GLOBAL_ERR_STATUS         ,y
voc_short GLOBAL_STRM_STATUS        ,y

// audio status
voc_short GLOBAL_NB_CHAN            ,y
voc_word  GLOBAL_SAMPLE_RATE        ,y
voc_short GLOBAL_BIT_RATE           ,y
voc_short GLOBAL_DISPLAY_BARS_LOG ,5,y
voc_word  GLOBAL_STRM_BUF_IN_CURR   ,y
voc_word  GLOBAL_STRM_BUF_IFC_CURR  ,y

// video status
voc_short GLOBAL_IMG_WIDTH          ,y
voc_short GLOBAL_IMG_HEIGHT         ,y
voc_word  GLOBAL_VID_BUF_IN_CURR    ,y

voc_struct_end

voc_struct VPP_AMJP_CODE_CFG_STRUCT __attribute__((export)),y

voc_word  GLOBAL_JPEG_CODE_PTR      ,y
voc_word  GLOBAL_JPEG_CONST_Y_PTR   ,y
voc_word  GLOBAL_MP12_CODE_PTR      ,y
voc_word  GLOBAL_MP12_CONST_Y_PTR   ,y
voc_word  GLOBAL_MP3_CODE_PTR       ,y
voc_word  GLOBAL_MP3_CONST_X_PTR    ,y
voc_word  GLOBAL_MP3_CONST_Y_PTR    ,y
voc_word  GLOBAL_AMR_CODE_PTR       ,y
voc_word  GLOBAL_AMR_CONST_Y_PTR    ,y
voc_word  GLOBAL_AAC_CODE_PTR       ,y
voc_word  GLOBAL_AAC_CONST_Y_PTR    ,y
voc_word  GLOBAL_EQ_TAB_PTR         ,y
voc_word  GLOBAL_ZOOM_CODE_PTR      ,y
voc_word  GLOBAL_ZOOM_CONST_Y_PTR   ,y
voc_struct_end

voc_short GLOBAL_DISPLAY_BARS       ,10,y
voc_word  GLOBAL_STRM_BUF_IN_PTR       ,y
voc_word  GLOBAL_VID_BUF_IN_PTR        ,y

#endif


#if 0

// IMPORTANT : this buffer is circular, so its size should be a power of 2
// !!!!!!!!! : and it should be aligned on a multiple of its size.

voc_short GLOBAL_INPUT_DATA_BUFFER  ,1024,x
voc_word  GLOBAL_TAB_EQUALIZER_ADDR    ,5,x


#endif


void vpp_AmjpCodeConstReload(void);


/**************************************************************************************
 * Function:    vpp_AmjpMain
 *
 * Description: decode AAC frame
 *
 * Inputs:      global cfg structure
 *
 * Outputs:     global status structure
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       09/03/2007
 *
 **************************************************************************************/


void vpp_AmjpMain(void)
{
    // read the wakeup mask
    VoC_lw16_d(REG0,CFG_WAKEUP_MASK);

    // Init : clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);

    // push the wakeup mask on the stack
    VoC_push16(REG0,DEFAULT);

    // generate first interrupt
    VoC_cfg(CFG_CTRL_IRQ_TO_XCPU | CFG_CTRL_CONTINUE);

AMJP_LABEL_MAIN_LOOP:

    // pop the wakeup mask from the stack
    VoC_pop16(REG0,DEFAULT);
    VoC_NOP();

    // configure the wakeup mask
    VoC_sw16_d(REG0,CFG_WAKEUP_MASK);
    VoC_NOP();

    // Stall the execution
    // wait for the next audio event
    VoC_cfg(CFG_CTRL_STALL);

// VoC_directive: PARSER_OFF
    // used for simulation
    VoC_cfg_stop;
// VoC_directive: PARSER_ON

    VoC_NOP();

    // all processing is scheduled only on the audio wakeup events
    // for this reason the video mask (SOF0) is not enabled.
    // We need to read the status (and not the cause) to see if
    // a video event is recorded.
    VoC_lw16_d(REG4,CFG_WAKEUP_STATUS);

    // Clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);

    // Disable the SOF1 event, enable DMAI event.
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_MASK);

    // push the wakeup mask on the stack
    VoC_push16(REG0,DEFAULT);

    // Read the IFC read pointer position at wakeup.
    // This is used by the upper layer to detect for eventual
    // event miss due to very long processing.

    VoC_lw32z(RL7,DEFAULT);
    VoC_lw32_d(RL6,GLOBAL_STRM_BUF_IFC_PTR);
    VoC_sw32_d(RL7,GLOBAL_STRM_BUF_IFC_CURR);

    VoC_bez32_r(no_rd_ifc_curr_pos, RL6)

    VoC_lw16i_short(RL7_HI,1,DEFAULT);
    VoC_lw16i(RL7_LO,GLOBAL_STRM_BUF_IFC_CURR/2);

    VoC_sw16_d(RL7_HI,CFG_DMA_SIZE);
    VoC_sw16_d(RL7_LO,CFG_DMA_LADDR);
    VoC_sw32_d(RL6,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

no_rd_ifc_curr_pos:

    // check if code and constants reload is necessary
    // (compare the new and the old (status) mode
    VoC_lw16_d(REG0,GLOBAL_MODE_IN);

    // ----------------------------
    // fct : vpp_AmjpCodeConstReload
    // in :  REG0 -> GLOBAL_MODE_IN
    // not modified : REG0
    // unused : REG45, RL67, ACC1
    // ----------------------------
    VoC_jal(vpp_AmjpCodeConstReload);


    // reset all global stream pointers
    VoC_bez16_d(no_reset_global_ptrs,GLOBAL_RESET);

    VoC_lw32_d(RL6,GLOBAL_STRM_BUF_IN_START);
    VoC_lw32_d(RL7,GLOBAL_VID_BUF_IN_START);
    VoC_sw32_d(RL6,GLOBAL_STRM_BUF_IN_CURR);
    VoC_sw32_d(RL6,GLOBAL_STRM_BUF_IN_PTR);
    VoC_sw32_d(RL7,GLOBAL_VID_BUF_IN_CURR);
    VoC_sw32_d(RL7,GLOBAL_VID_BUF_IN_PTR);

no_reset_global_ptrs:


    // check for a video stream event & clear the event (if any)
    VoC_lw16i_short(REG7,CFG_WAKEUP_SOF0,DEFAULT);
    VoC_and16_rr(REG7,REG4,DEFAULT);
    VoC_and16_ri(REG4,(~CFG_WAKEUP_SOF0));

    // check if the video mode is enabled & clear the mode (if any)
    VoC_lw16i(REG6,0xff00);
    VoC_and16_rr(REG6,REG0,DEFAULT);
    VoC_and16_ri(REG0,0xff);

    // VPP_AMJP_STREAM_ID_NO == 0
    VoC_lw16i_short(REG7,VPP_AMJP_STRM_ID_NO,DEFAULT);

    // store the video event
    VoC_push32(REG67,DEFAULT);

    // clear previous stream errors
    VoC_sw16_d(REG7,GLOBAL_ERR_STATUS);

    // check for an audio stream event
    VoC_bez16_r(AMJP_LABEL_MAIN_NO_AUDIO,REG4);

    // jal to the main code
    VoC_lw16i_short(REG4,VPP_AMJP_MODE_AMR,DEFAULT);
    VoC_lw16i_short(REG5,VPP_AMJP_MODE_MP3,IN_PARALLEL);
    VoC_lw16i_short(REG6,VPP_AMJP_MODE_AAC,DEFAULT);
    VoC_lw16i_short(REG7,-23,IN_PARALLEL);

    VoC_bne16_rr(AMJP_LABEL_MAIN_NO_AMR,REG0,REG4)

    VoC_jal(CII_MRRING_Decode);
    VoC_jump(AMJP_LABEL_MAIN_AUDIO_DONE);

AMJP_LABEL_MAIN_NO_AMR:

    VoC_bne16_rr(AMJP_LABEL_MAIN_NO_MP3,REG0,REG5)

    VoC_jal(VPP_MP3_DECODE);
    VoC_jump(AMJP_LABEL_MAIN_AUDIO_DONE);

AMJP_LABEL_MAIN_NO_MP3:

    VoC_bne16_rr(AMJP_LABEL_MAIN_NO_AAC,REG0,REG6)

    VoC_jal(Coolsand_AACDecode);
    VoC_jump(AMJP_LABEL_MAIN_AUDIO_DONE);

AMJP_LABEL_MAIN_NO_AAC:
AMJP_LABEL_MAIN_AUDIO_MODEERR:

    VoC_sw16_d(REG7,GLOBAL_ERR_STATUS);

AMJP_LABEL_MAIN_AUDIO_DONE:

    VoC_lw16i_short(REG5,VPP_AMJP_STRM_ID_AUDIO,DEFAULT);
    VoC_lw32_d(RL6,GLOBAL_STRM_BUF_IN_PTR);


    // status : audio frame decoding done
    VoC_sw16_d(REG5,GLOBAL_STRM_STATUS);

    // status : audio stream pointer
    // pass only the end-of-frame value of the stream pointer
    // in the status structure to avoid that the XCPU reads
    // an intermediary (false status) while VoC is running
    VoC_sw32_d(RL6,GLOBAL_STRM_BUF_IN_CURR);


    // generate an interrupt for audio done
    VoC_cfg(CFG_CTRL_IRQ_TO_XCPU | CFG_CTRL_CONTINUE);


AMJP_LABEL_MAIN_NO_AUDIO:

    // pop the video event
    VoC_pop32(REG67,DEFAULT);

    // check if a video event is received
    VoC_bez16_r(AMJP_LABEL_MAIN_NO_VIDEO,REG7);

    // jal to the main code
    VoC_lw16i(REG4,VPP_AMJP_MODE_JPEG);
    VoC_lw16i(REG5,VPP_AMJP_MODE_ZOOM);
    VoC_lw16i_short(REG7,-23,DEFAULT);

    VoC_bne16_rr(AMJP_LABEL_MAIN_NO_JPEG,REG6,REG4);

    VoC_jal(CII_JPEG_Decode);
    VoC_jump(AMJP_LABEL_MAIN_VIDEO_DONE);

AMJP_LABEL_MAIN_NO_JPEG:

    VoC_bne16_rr(AMJP_LABEL_MAIN_NO_ZOOM,REG6,REG5);

    VoC_jal(vpp_AmjpZoom);
    VoC_jump(AMJP_LABEL_MAIN_VIDEO_DONE);

AMJP_LABEL_MAIN_NO_ZOOM:
AMJP_LABEL_MAIN_VIDEO_MODEERR:

    VoC_sw16_d(REG7,GLOBAL_ERR_STATUS);

AMJP_LABEL_MAIN_VIDEO_DONE:

    VoC_lw16i_short(REG5,VPP_AMJP_STRM_ID_VIDEO,DEFAULT);
    VoC_lw32_d(RL6,GLOBAL_VID_BUF_IN_PTR);

    // status : video frame decoding done
    VoC_sw16_d(REG5,GLOBAL_STRM_STATUS);

    // status : video stream pointer
    // pass only the end-of-frame value of the stream pointer
    // in the status structure to avoid that the XCPU reads
    // an intermediary (false status) while VoC is running
    VoC_sw32_d(RL6,GLOBAL_VID_BUF_IN_CURR);

    // generate an interrupt for video done
    VoC_cfg(CFG_CTRL_IRQ_TO_XCPU | CFG_CTRL_CONTINUE);


AMJP_LABEL_MAIN_NO_VIDEO:

    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_NOP();

    // clear the reset
    VoC_sw16_d(REG0,GLOBAL_RESET);

    VoC_jump(AMJP_LABEL_MAIN_LOOP);

}


void vpp_AmjpRunDma(void)
{
    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    VoC_return;
}

// ----------------------------
// fct : vpp_AmjpCodeConstReload
// in :  REG0 -> GLOBAL_MODE_IN
// not modified : REG0
// unused : REG45, RL67, ACC1
// ----------------------------

void vpp_AmjpCodeConstReload(void)
{
    // old (status) mode
    VoC_lw16_d(REG1,GLOBAL_MODE_OUT);
    VoC_be16_rr(CODE_RELOAD_NO,REG0,REG1);

    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(REG6,0xf,DEFAULT);
    VoC_lw16i_short(REG7,0xf,IN_PARALLEL);

    VoC_and16_rr(REG6,REG0,DEFAULT);
    VoC_and16_rr(REG7,REG1,DEFAULT);

    // check if sec2 is to be reloaded
    VoC_be16_rr(CODE_RELOAD_NO_SEC2,REG6,REG7);

    // check if the new mode is AMR
    VoC_lw16i_short(REG7,VPP_AMJP_MODE_AMR,DEFAULT);
    VoC_bne16_rr(CODE_RELOAD_NO_AMR,REG6,REG7);

    // AMR code
    VoC_lw32_d(ACC0,GLOBAL_AMR_CODE_PTR);
    VoC_lw16i(REG2,SECTION_AMR_DEC_START);
    VoC_lw16i(REG3,SECTION_AMR_DEC_SIZE);
    VoC_jal(vpp_AmjpRunDma);

    // AMR const Y
    VoC_lw32_d(ACC0,GLOBAL_AMR_CONST_Y_PTR);
    VoC_lw16i(REG2,AMR_TABLE_RAM_Y_STRUCT/2);
    VoC_lw16i(REG3,AMR_Dec_ConstY_size);
    VoC_jal(vpp_AmjpRunDma);

CODE_RELOAD_NO_AMR:

    // check if the new mode is MP3
    VoC_lw16i_short(REG7,VPP_AMJP_MODE_MP3,DEFAULT);
    VoC_bne16_rr(CODE_RELOAD_NO_MP3,REG6,REG7);

    // MP3 code
    VoC_lw32_d(ACC0,GLOBAL_MP3_CODE_PTR);
    VoC_lw16i(REG2,SECTION_MP3_DEC_START);
    VoC_lw16i(REG3,SECTION_MP3_DEC_SIZE);
    VoC_jal(vpp_AmjpRunDma);

    // MP3 const X
    VoC_lw32_d(ACC0,GLOBAL_MP3_CONST_X_PTR);
    VoC_lw16i(REG2,MP3_TABLE_RAM_X_STRUCT/2);
    VoC_lw16i(REG3,MP3_DEC_CONST_X_SIZE);
    VoC_jal(vpp_AmjpRunDma);

    // MP3 const Y
    VoC_lw32_d(ACC0,GLOBAL_MP3_CONST_Y_PTR);
    VoC_lw16i(REG2,MP3_TABLE_RAM_Y_STRUCT/2);
    VoC_lw16i(REG3,MP3_DEC_CONST_Y_SIZE);
    VoC_jal(vpp_AmjpRunDma);

CODE_RELOAD_NO_MP3:

    // check if the new mode is AAC
    VoC_lw16i_short(REG7,VPP_AMJP_MODE_AAC,DEFAULT);
    VoC_bne16_rr(CODE_RELOAD_NO_AAC,REG6,REG7);

    // AAC code
    VoC_lw32_d(ACC0,GLOBAL_AAC_CODE_PTR);
    VoC_lw16i(REG2,SECTION_AAC_DEC_START);
    VoC_lw16i(REG3,SECTION_AAC_DEC_SIZE);
    VoC_jal(vpp_AmjpRunDma);

    // AAC const Y
    VoC_lw32_d(ACC0,GLOBAL_AAC_CONST_Y_PTR);
    VoC_lw16i(REG2,AAC_Dec_ConstY_start/2);
    VoC_lw16i(REG3,AAC_ConstY_size);
    VoC_jal(vpp_AmjpRunDma);

CODE_RELOAD_NO_AAC:
CODE_RELOAD_NO_SEC2:

    VoC_lw16i(REG6,VPP_AMJP_MODE_JPEG);
    VoC_movreg16(REG7,REG6,DEFAULT);

    VoC_and16_rr(REG6,REG0,DEFAULT);
    VoC_and16_rr(REG7,REG1,DEFAULT);

    // check if the new mode includes JPEG, while the old does not
    VoC_bngt16_rr(CODE_RELOAD_NO_SEC1,REG6,REG7);

    // JPEG code
    VoC_lw32_d(ACC0,GLOBAL_JPEG_CODE_PTR);
    VoC_lw16i(REG2,SECTION_JPEG_DEC_START);
    VoC_lw16i(REG3,SECTION_JPEG_DEC_SIZE);
    VoC_jal(vpp_AmjpRunDma);

    // JPEG const Y
    VoC_lw32_d(ACC0,GLOBAL_JPEG_CONST_Y_PTR);
    VoC_lw16i(REG2,JPEG_TABLE_RAM_Y_STRUCT/2);
    VoC_lw16i(REG3,JPEG_Dec_ConstY_size);
    VoC_jal(vpp_AmjpRunDma);

CODE_RELOAD_NO_SEC1:

    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL);

    // update the mode status
    VoC_sw16_d(REG0,GLOBAL_MODE_OUT);

    // re-enable the global reset
    VoC_sw16_d(REG1,GLOBAL_RESET);

CODE_RELOAD_NO:

    VoC_return;
}


// ----------------------------
// fct : vpp_AmjpCircBuffDmaRd
// in :  REG4 -> local buffer wrap exponent
//       REG5 -> local buffer wrap mask
//       REG6 -> local buffer address
//       REG7 -> load size (words)
// out : REG5 -> new local buffer address
// not modified : REG0
// unused : REG012, ACC0, ACC1
// ----------------------------

void vpp_AmjpCircBuffDmaRd(void)
{
    // RL6 : current read pointer
    VoC_lw32_d(RL6,GLOBAL_STRM_BUF_IN_PTR);

    // REG3 : size until end of buffer in ints
    VoC_sub32_dr(RL7,GLOBAL_STRM_BUF_IN_END,RL6);
    VoC_movreg16(REG3,RL7_LO,DEFAULT);
    VoC_shr16_ri(REG3,2,DEFAULT);

    // cast load size to 32-bit
    VoC_movreg16(RL7_LO,REG7,DEFAULT);
    VoC_lw16i_short(RL7_HI,0,IN_PARALLEL);
    VoC_shr32_ri(RL7,-2,DEFAULT);

    // compute end load pointer
    VoC_add32_rr(RL7,RL7,RL6,DEFAULT);

    // configure wrap & laddr
    VoC_sw16_d(REG4,CFG_DMA_WRAP);
    VoC_sw16_d(REG6,CFG_DMA_LADDR);

    // check external buffer wrap condition
    VoC_blt32_rd(no_ext_buff_wrap,RL7,GLOBAL_STRM_BUF_IN_END)

    VoC_add16_rr(REG6,REG3,REG6,DEFAULT);
    VoC_and16_rr(REG6,REG5,DEFAULT);

    // read until the end of the buffer
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(RL6,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    // subtract read value
    VoC_sub16_rr(REG7,REG7,REG3,DEFAULT);

    // read from the buffer start
    VoC_lw32_d(RL6,GLOBAL_STRM_BUF_IN_START);

no_ext_buff_wrap:

    // check if the remaining size is NULL
    VoC_bez16_r(remaining_load_size_is_null,REG7);

    // no need to reconfigure wrap
    VoC_sw16_d(REG6,CFG_DMA_LADDR);
    VoC_sw16_d(REG7,CFG_DMA_SIZE);
    VoC_sw32_d(RL6,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

remaining_load_size_is_null:

    // compute the new laddr
    VoC_add16_rr(REG6,REG7,REG6,DEFAULT);
    VoC_and16_rr(REG5,REG6,DEFAULT);

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_shr32_ri(REG67,14,DEFAULT);

    // here REG6 == 0
    VoC_sw16_d(REG6,CFG_DMA_WRAP);

    VoC_add32_rr(RL6,RL6,REG67,DEFAULT);

    VoC_shr16_ri(REG5, -2, DEFAULT);

    VoC_sw32_d(RL6,GLOBAL_STRM_BUF_IN_PTR);

    VoC_return;

}


void vpp_AmjpCircBuffInit(void)
{

    VoC_lw32_d(RL6,GLOBAL_STRM_BUF_IN_START);
    VoC_lw16i(REG3,GLOBAL_INPUT_DATA_BUFFER/2);
    VoC_lw16i(REG2,512);

    VoC_sw16_d(REG3,CFG_DMA_LADDR);
    VoC_sw16_d(REG2,CFG_DMA_SIZE);
    VoC_sw32_d(RL6,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();

    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_shr16_ri(REG2,-2,IN_PARALLEL);
    VoC_add32_rr(RL6,RL6,REG23,DEFAULT);

    // check external buffer wrap condition
    VoC_bne32_rd(CII_buf_in_ptr_no_wrap,RL6,GLOBAL_STRM_BUF_IN_END)

    VoC_lw32_d(RL6,GLOBAL_STRM_BUF_IN_START);

CII_buf_in_ptr_no_wrap:

    // clear the event
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    // init the stream buffer pointer
    VoC_sw32_d(RL6,GLOBAL_STRM_BUF_IN_PTR);

    VoC_return;

}





/**************************************************************************************
 * Function:    vpp_AmjpAudioEqualizer
 *
 * Description: equalizer and bars;
 *
 * Inputs:      REG1-> inptr; inrc2=2
 *              RL6 -> imdctBands per eqBand counter
 * Outputs:
 *
 * Used :       all
 *
 * Version 2.0  Create by  Yordan  06/10/2008
 *
 **************************************************************************************/

void vpp_AmjpAudioEqualizer(void)
{

    // push all used regs
    VoC_push32(ACC0,DEFAULT);
    VoC_push32(RL7,DEFAULT);

    VoC_push32(ACC1,DEFAULT);
    VoC_push16(REG0,IN_PARALLEL);

    VoC_push32(REG45,DEFAULT);
    VoC_push16(REG1,IN_PARALLEL);

    VoC_push32(REG67,DEFAULT);
    VoC_push16(REG3,IN_PARALLEL);

    VoC_lw16i_short(REG6,VPP_AMJP_EQUALIZER_CUSTOM,DEFAULT);
    VoC_lw16_d(REG7,GLOBAL_EQ_TYPE);

    // do nothing if the equalizer is off or if it is invalid
    VoC_bltz16_r(audio_equalizer_no,REG7);
    VoC_bgt16_rr(audio_equalizer_no,REG7,REG6);

    // ---------------------------
    // get the equalization table
    // ---------------------------

    // compute the index (each equalizer is a 10 short = 20 bytes table)
    VoC_lw16i_short(REG7,20,DEFAULT);
    VoC_mult16_rd(REG6,REG7,GLOBAL_EQ_TYPE);
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_add32_rd(ACC0,REG67,GLOBAL_EQ_TAB_PTR);

    // load only the requested equalizer (10 short)
    VoC_lw16i_short(REG6,5,DEFAULT);
    VoC_lw16i(REG7,GLOBAL_TAB_EQUALIZER_ADDR/2);
    VoC_NOP();

    // configure the DMA
    VoC_sw32_d(REG67,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    // ---------------------------
    // Equalization principle
    // ---------------------------

    // We use a WMP type equalizer definition with logarithmic frequency bands.
    // The bands are approximately centered at :
    //
    // 31, 62, 125, 250, 500,  1k,   2k,   4k,   8k,   16k for 44.1k sampling rate
    // 31, 62, 125, 250, 500,  1k,   2k,   4k,   8k,   16k for 48k   sampling rate
    // 25, 50, 100, 200, 400, 800, 1.6k, 3.2k, 6.4k, 12.8k for 32k   sampling rate
    //
    // As the IMDCT frequency bands are equi-distributed, they should be grouped as shown
    // below, each group being multiplied by the same equalizer gain:
    //
    //  1,  1,   2,   4,   8,  16,   32,   64,  128,  320   (totally 576 bands)
    //
    // The equalizer table is composed of band gains in Q10 format (1.0 = 0x0400)


    // ---------------------------
    // Display bars principle
    // ---------------------------

    // Compute the mean power of each eq band
    // Use a low pass filter to reduce variations

    // ---------------------------
    // start equalization
    // compute the bars
    // ---------------------------

#define EQUALIZER_FORMAT       5
#define DISPLAY_BARS_RESCALE   14
#define DISPLAY_BARS_LOG_MEAN  2
#define DISPLAY_BARS_ALPHA     ((1 << DISPLAY_BARS_LOG_MEAN) -1)

    VoC_lw16i_set_inc(REG0,GLOBAL_TAB_EQUALIZER_ADDR,1);
    VoC_lw16i_set_inc(REG3,GLOBAL_DISPLAY_BARS,1);

    VoC_lw16i(REG4,DISPLAY_BARS_ALPHA);

    VoC_lw16i_short(FORMATX,(15-EQUALIZER_FORMAT),DEFAULT);
    VoC_lw16i_short(FORMAT32,(-EQUALIZER_FORMAT),IN_PARALLEL);

    // -------------
    // first eq band
    // -------------

    // load 0x8000 in RL7
    VoC_lw16i_short(RL7_HI,0,DEFAULT);
    VoC_lw16i(RL7_LO,0x8000);

    // duplicate sample pointer
    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_movreg16(REG2,REG1,IN_PARALLEL);

    // while (nb_bands_to_equalize_per_eq_band == 0)
    // {pEqTable++; pDisplayBar++}

nb_bands_to_equalize:

    // round(RL6_HI), RL6 << = 1
    VoC_add32_rr(ACC0,RL6,RL7,DEFAULT)
    VoC_movreg16(REG5,ACC0_HI,DEFAULT);
    VoC_shr32_ri(RL6,-1,IN_PARALLEL);

    // if (!round(RL6_HI))
    // increment eq pointers and loop back
    VoC_bnez16_r(reached_first_band_to_equalize,REG5)

    VoC_inc_p(REG0,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);

    VoC_jump(nb_bands_to_equalize);

reached_first_band_to_equalize:

    // compute the remaining nb if EqBands
    VoC_lw16i(REG5,GLOBAL_DISPLAY_BARS+8);
    VoC_sub16_rr(REG5,REG5,REG3,DEFAULT);

    VoC_lw32inc_p(REG67,REG2,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_shr32_ri(REG67,-1,DEFAULT);

    // acc1 = bar*alpha
    VoC_multf32_rp(ACC1,REG4,REG3,IN_PARALLEL);
    VoC_NOP();
    VoC_shr32_ri(ACC1,(EQUALIZER_FORMAT-DISPLAY_BARS_RESCALE),DEFAULT);

    // sample*eq
    VoC_mac32_rp(REG7,REG0,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_macXinc_rp(REG6,REG0,DEFAULT);

    // acc1 += sample?
    VoC_mac32_rr(REG7,REG7,IN_PARALLEL);
    VoC_NOP();

    // bar = (bar*alpha + sample)/(alpha+1)
    VoC_shr32_ri(ACC1,(DISPLAY_BARS_LOG_MEAN+DISPLAY_BARS_RESCALE),DEFAULT);

    // eq bands n to 9
    VoC_loop_r(1,REG5);

    // store new_bar
    VoC_sw16inc_p(ACC1_LO,REG3,DEFAULT);

    // compute bar*alpha
    // (need to rescale in the opposite direction)
    VoC_multf32_rp(ACC1,REG4,REG3,DEFAULT);
    VoC_add32_rr(RL6,RL6,RL7,IN_PARALLEL);

    VoC_movreg16(REG5,RL6_HI,DEFAULT);
    VoC_sub32_rr(RL6,RL6,RL7,IN_PARALLEL);

    VoC_loop_r_short(REG5,DEFAULT)
    VoC_shr32_ri(ACC1,(EQUALIZER_FORMAT-DISPLAY_BARS_RESCALE),IN_PARALLEL);
    VoC_startloop0

    VoC_lw32inc_p(REG67,REG2,DEFAULT);
    VoC_shr32_ri(REG67,-1,DEFAULT);

    // store equalized sample
    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,IN_PARALLEL);

    VoC_mac32_rp(REG7,REG0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_macX_rp(REG6,REG0,DEFAULT);

    // acc1 += bar?
    VoC_mac32_rr(REG7,REG7,IN_PARALLEL);

    VoC_endloop0

    // double next band width
    VoC_shr32_ri(RL6,-1,DEFAULT);

    // (old_bar*alpha + new)/(alpha+1)
    VoC_shr32_ri(ACC1,(DISPLAY_BARS_LOG_MEAN+DISPLAY_BARS_RESCALE),DEFAULT);

    // point to the next eq coeff
    VoC_inc_p(REG0,DEFAULT);

    VoC_endloop1

    // store new_bar
    VoC_sw16inc_p(ACC1_LO,REG3,DEFAULT);
    VoC_multf32_rp(ACC1,REG4,REG3,DEFAULT);

    // remaining size is 320 for MP3, else RL6_HI
    VoC_lw16i_short(REG5,10,DEFAULT);
    VoC_shr32_ri(RL6,5,IN_PARALLEL);

    VoC_shr32_ri(ACC1,(EQUALIZER_FORMAT-DISPLAY_BARS_RESCALE),DEFAULT);

    VoC_lw16i(REG6,VPP_AMJP_MODE_JPEG);
    VoC_lw16i_short(REG7,VPP_AMJP_MODE_MP3,DEFAULT);

    VoC_and16_rd(REG6,GLOBAL_MODE_IN);
    VoC_bne16_rr(audio_mode_not_mp3,REG6,REG7);

    VoC_movreg16(REG5,RL6_HI,DEFAULT);

audio_mode_not_mp3:

    // 1ast eq band
    VoC_loop_i(1,32);

    VoC_loop_r_short(REG5,DEFAULT)
    VoC_startloop0

    VoC_lw32inc_p(REG67,REG2,DEFAULT);
    VoC_shr32_ri(REG67,-1,DEFAULT);

    // store equalized sample
    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,IN_PARALLEL);

    VoC_mac32_rp(REG7,REG0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_macX_rp(REG6,REG0,DEFAULT);

    // acc1 += bar?
    VoC_mac32_rr(REG7,REG7,IN_PARALLEL);

    VoC_endloop0

    VoC_NOP();

    VoC_endloop1

    // (old_bar*alpha + new)/(alpha+1)
    VoC_shr32_ri(ACC1,(DISPLAY_BARS_LOG_MEAN+DISPLAY_BARS_RESCALE),DEFAULT);

    // store last equalized sample
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);

    // store last new_bar
    VoC_sw16inc_p(ACC1_LO,REG3,DEFAULT);

    // ---------------------------
    // compute the log of the bars
    // ---------------------------

    VoC_lw16i_set_inc(REG0,GLOBAL_DISPLAY_BARS_LOG,1);
    VoC_lw16i_set_inc(REG1,GLOBAL_DISPLAY_BARS,2);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);

    VoC_loop_i(1,5)

    VoC_lw32inc_p(REG67,REG1,DEFAULT);
    VoC_lw32z(REG23,IN_PARALLEL)

    VoC_loop_i_short(16,DEFAULT)
    VoC_startloop0

    VoC_bez16_r(bar_norm_done_0,REG6)
    VoC_inc_p(REG2,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_endloop0

    VoC_NOP();

bar_norm_done_0:

    VoC_loop_i_short(16,DEFAULT)
    VoC_startloop0

    VoC_bez16_r(bar_norm_done_1,REG7)
    VoC_inc_p(REG3,DEFAULT);
    VoC_shru16_ri(REG7,1,IN_PARALLEL);

    VoC_endloop0

    VoC_NOP();

bar_norm_done_1:

    // cast on 8 bits
    VoC_shru16_ri(REG2,-8,DEFAULT);
    VoC_shru32_ri(REG23,8,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG2,REG0,DEFAULT);

    VoC_endloop1

    VoC_NOP();

audio_equalizer_no:


    // pop all used regs
    VoC_pop32(REG67,DEFAULT);
    VoC_pop16(REG3,IN_PARALLEL);

    VoC_pop32(REG45,DEFAULT);
    VoC_pop16(REG1,IN_PARALLEL);

    VoC_pop32(ACC1,DEFAULT);
    VoC_pop16(REG0,IN_PARALLEL);

    VoC_pop32(RL7,DEFAULT);
    VoC_pop32(ACC0,DEFAULT);

    VoC_return;
}







// **************************************************************************************
//   Function:    vpp_AmjpDecompressVssAdpcmTable
//
//   Description: decompress a table compressed using 4,8,32 bit Varible Sample Size and recursive ADPCM
//
//   Inputs:      REG0-> destination (decompressed table); inrc0=2
//                REG1-> source (compressed table); inrc1=1
//                REG2-> table size, incr2 = -1;
//                REG3-> iterations, incr3 = -1;
//                REG4-> step;
//                REG5-> size/step;
//   Outputs:
//
//   Used :       REG01234567
//
//   Version 1.0  Create by  Yordan  07/10/2008
//
//   *************************************************************************************

void vpp_AmjpDecompressVssAdpcmTable(void)
{

    // Because of the regularity of their pattern, some tables can be coded
    // with an ADPCM based algo : the difference of the samples are computed,
    // then the difference of the difference, etc... After 5 iterations
    // almost all resulting values can be coded on majority on 4 or 8 bits.
    // Further compressing involves saving the samples on variable size.
    // In order to distinguish between the different possible sizes,
    // value on more than 4 bits are preceeded by a special codeword :
    //
    // 8 : for  8-bit (except 80)
    // 808 : for 32-bit
    //
    // The compression ratio depends on the regularity of the pattern,
    // it is of about 6:1 for standard sin-windowing tables

    // ----------------------
    // ADPCM compression algo
    // ----------------------

    //    for (i = 0 ; i < size; ++i)
    //    {
    //        tableCompressed[i] = tableIn[i];
    //    }
    //
    //    for (k = 0; k < step; ++k)
    //    {
    //        for (j = 0; j < iterations; ++j)
    //        {
    //            for (i = (size/step) - j - 2; i > -1; i--)
    //            {
    //                tableCompressed[step*(i+j+1) + k] = tableCompressed[step*(i+j) + k] - tableCompressed[step*(i+j+1) + k];
    //            }
    //        }
    //    }

    // -----------------------------
    // variable bit size compression
    // -----------------------------

    // if val = tableCompressed[i] is in [-0x7, 0x7]
    // write the 4 lsb bits of val in the compressed bitstream
    // else if val = tableCompressed[i] is in [-0x81, 0x7f]
    // write the 8 lsb bits of val preceeded by 0x8 on 4 bits in the compressed bitstream (12 bits written)
    // else val = tableCompressed[i]
    // write the 32 bits of val preceeded by 0x808 on 12 bits in the compressed bitstream (44 bits written)

    // -----------------------------
    // variable sample size decompression
    // -----------------------------

    // val = (signed)(read next 4 bits)
    // if val is in [-0x7, 0x7] tableDeCompress[i] = val;
    // else val = (signed)(read next 8 bits)
    // if val is in [-0x81, 0x7f] tableDeCompress[i] = val;
    // else val = (signed)(read next 32 bits)
    // tableDeCompress[i] = val;

    // ------------------------
    // ADPCM decompression algo
    // ------------------------

    //    for (k = 0; k < step; ++k)
    //    {
    //        for (j = iterations; j > 0 ; --j)
    //        {
    //            for (i = j - 1; i < (size/step); ++i)
    //            {
    //                tableCompressed[step*(i+1) + k] = tableCompressed[step*(i) + k] - tableCompressed[step*(i+1) + k];
    //            }
    //        }
    //    }

    // ----------------
    // VSS decompression
    // ----------------

    VoC_push16(BITCACHE,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);

    VoC_push16(REG2,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);

    VoC_push16(REG0,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);

    VoC_lbinc_p(REG1);


continue_vbs:

    VoC_lbinc_p(REG1);

    // val = (signed)(read next 4 bits)
    VoC_rbinc_i(REG6,4,DEFAULT);

    VoC_shru32_ri(REG67,-28,DEFAULT);
    VoC_movreg16(REG4,REG6,IN_PARALLEL);

    VoC_shr32_ri(REG67,28,DEFAULT);
    VoC_lw16i_short(REG5,0x8,IN_PARALLEL);

    VoC_bne16_rr(write_uncompressed_sample,REG4,REG5);

    // val = (signed)(read next 8 bits)
    VoC_rbinc_i(REG6,8,DEFAULT);

    VoC_shru32_ri(REG67,-24,DEFAULT);
    VoC_movreg16(REG4,REG6,IN_PARALLEL);

    VoC_shr32_ri(REG67,24,DEFAULT);
    VoC_shru16_ri(REG5,-4,IN_PARALLEL);

    VoC_bne16_rr(write_uncompressed_sample,REG4,REG5);

    // val = (signed)(read next 32 bits)
    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG6,16,DEFAULT);

    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG7,16,DEFAULT);

    VoC_NOP();

write_uncompressed_sample:

    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);

    VoC_bnez16_r(continue_vbs,REG2);


    // ------------------------
    // ADPCM decompression algo
    // ------------------------

    //    for (k = 0; k < step; ++k)
    //    {
    //        for (j = iterations; j > 0 ; --j)
    //        {
    //            for (i = j - 1; i < (size/step); ++i)
    //            {
    //                tableCompressed[step*(i+1) + k] = tableCompressed[step*(i) + k] - tableCompressed[step*(i+1) + k];
    //            }
    //        }
    //    }

    // pop step, size/step
    VoC_pop32(REG45,DEFAULT);

    // push iteration
    VoC_push16(REG3,DEFAULT);

    // INC0 = 2*step
    VoC_add16_rr(REG7,REG4,REG4,IN_PARALLEL);
    VoC_movreg16(INC0,REG7,DEFAULT);

    // put size/step in stack
    VoC_sw16_sd(REG5,2,DEFAULT);

    // k = 0
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    // for (k = 0; k < step; ++k)
    VoC_loop_r(1,REG4)

    // iterations
    VoC_lw16_sd(REG3,0,DEFAULT);

    // for (j = iterations; j > 0 ; --j)
    VoC_loop_r(0,REG3)

    // i' = size/step - j
    VoC_lw16_sd(REG2,2,DEFAULT);
    VoC_sub16_rr(REG2,REG2,REG3,DEFAULT);

    // j--
    VoC_inc_p(REG3,DEFAULT);

    // &tableDeCompress[0]
    VoC_lw16_sd(REG0,1,IN_PARALLEL);

    // (j-1)*step
    VoC_mult16_rr(REG6,REG3,REG4,DEFAULT);
    VoC_NOP();

    // &tableDeCompress[(j-1)*step + k]
    VoC_add16_rr(REG6,REG6,REG5,DEFAULT);
    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);

    VoC_NOP();

    // tableDeCompress[(j-1)*step + k]
    VoC_lw32inc_p(REG67,REG0,DEFAULT);

    // tableDeCompress[step*i + k] - tableDeCompress[step*(i+1)+k];
    VoC_sub32_rp(REG67,REG67,REG0,DEFAULT);
    VoC_NOP();

continue_adpcm_decompression:

    // tableDeCompress[step*(i+1) + k] = tableDeCompress[step*i + k] - tableDeCompress[step*(i+1)+k];
    VoC_sw32inc_p(REG67,REG0,DEFAULT);

    // tableDeCompress[step*i + k] - tableDeCompress[step*(i+1)+k];
    VoC_sub32_rp(REG67,REG67,REG0,DEFAULT);

    // i'--
    VoC_inc_p(REG2,IN_PARALLEL);

    VoC_bnez16_r(continue_adpcm_decompression,REG2);

    VoC_NOP();

    VoC_endloop0

    // k++
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG7,DEFAULT);

    VoC_endloop1

    VoC_pop16(REG3,DEFAULT);
    VoC_pop16(REG0,DEFAULT);
    VoC_pop16(REG2,DEFAULT);

    VoC_pop16(BITCACHE,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);

    VoC_return;

}



// **************************************************************************************
//   Function:    vpp_AmjpDecompressVssAdpcm16Table
//
//   Description: same as previous but for 16-bit samples (2,4,16 bit VSS)
//
//   Inputs:      REG0-> destination (decompressed table); inrc0=1
//                REG1-> source (compressed table); inrc1=1
//                REG2-> table size, incr2 = -1;
//                REG3-> iterations, incr3 = -1;
//                REG4-> step;
//                REG5-> size/step;
//   Outputs:
//
//   Used :       REG01234567
//
//   Version 1.0  Create by  Yordan  07/10/2008
//
//   *************************************************************************************

void vpp_AmjpDecompressVssAdpcm16Table(void)
{

    // ----------------
    // VSS decompression
    // ----------------

    VoC_push16(BITCACHE,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);

    VoC_push16(REG2,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);

    VoC_push16(REG0,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);

    VoC_lbinc_p(REG1);


continue_vbs_16:

    VoC_lbinc_p(REG1);

    // val = (signed)(read next 2 bits)
    VoC_rbinc_i(REG6,2,DEFAULT);

    VoC_shru16_ri(REG6,-14,DEFAULT);
    VoC_movreg16(REG4,REG6,IN_PARALLEL);

    VoC_shr16_ri(REG6,14,DEFAULT);
    VoC_lw16i_short(REG5,0x2,IN_PARALLEL);

    VoC_bne16_rr(write_uncompressed_sample_16,REG4,REG5);

    // val = (signed)(read next 4 bits)
    VoC_rbinc_i(REG6,4,DEFAULT);

    VoC_shru16_ri(REG6,-12,DEFAULT);
    VoC_movreg16(REG4,REG6,IN_PARALLEL);

    VoC_shr16_ri(REG6,12,DEFAULT);
    VoC_shru16_ri(REG5,-2,IN_PARALLEL);

    VoC_bne16_rr(write_uncompressed_sample_16,REG4,REG5);

    // val = (signed)(read next 32 bits)
    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG6,16,DEFAULT);
    VoC_NOP();

write_uncompressed_sample_16:

    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);

    VoC_bnez16_r(continue_vbs_16,REG2);

    // pop step, size/step
    VoC_pop32(REG45,DEFAULT);

    // push iteration
    VoC_push16(REG3,DEFAULT);

    // INC0 = step
    VoC_movreg16(INC0,REG4,DEFAULT);

    // put size/step in stack
    VoC_sw16_sd(REG5,2,DEFAULT);

    // k = 0
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    // for (k = 0; k < step; ++k)
    VoC_loop_r(1,REG4)

    // iterations
    VoC_lw16_sd(REG3,0,DEFAULT);

    // for (j = iterations; j > 0 ; --j)
    VoC_loop_r(0,REG3)

    // i' = size/step - j
    VoC_lw16_sd(REG2,2,DEFAULT);
    VoC_sub16_rr(REG2,REG2,REG3,DEFAULT);

    // j--
    VoC_inc_p(REG3,DEFAULT);

    // &tableDeCompress[0]
    VoC_lw16_sd(REG0,1,IN_PARALLEL);

    // (j-1)*step
    VoC_mult16_rr(REG6,REG3,REG4,DEFAULT);
    VoC_NOP();

    // &tableDeCompress[(j-1)*step + k]
    VoC_add16_rr(REG6,REG6,REG5,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);

    VoC_NOP();

    // tableDeCompress[(j-1)*step + k]
    VoC_lw16inc_p(REG6,REG0,DEFAULT);

    // tableDeCompress[step*i + k] - tableDeCompress[step*(i+1)+k];
    VoC_sub16_rp(REG6,REG6,REG0,DEFAULT);
    VoC_NOP();

continue_adpcm_decompression_16:

    // tableDeCompress[step*(i+1) + k] = tableDeCompress[step*i + k] - tableDeCompress[step*(i+1)+k];
    VoC_sw16inc_p(REG6,REG0,DEFAULT);

    // tableDeCompress[step*i + k] - tableDeCompress[step*(i+1)+k];
    VoC_sub16_rp(REG6,REG6,REG0,DEFAULT);

    // i'--
    VoC_inc_p(REG2,IN_PARALLEL);

    VoC_bnez16_r(continue_adpcm_decompression_16,REG2);

    VoC_NOP();

    VoC_endloop0

    // k++
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG7,DEFAULT);

    VoC_endloop1

    VoC_pop16(REG3,DEFAULT);
    VoC_pop16(REG0,DEFAULT);
    VoC_pop16(REG2,DEFAULT);

    VoC_pop16(BITCACHE,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);

    VoC_return;

}

