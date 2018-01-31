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


#include "voc2_library.h"


#include "vppp_audiojpegenc_common.h"
#include "vppp_audiojpegenc_asm_map.h"
#include "vppp_audiojpegenc_sections.h"

#include "vppp_audiojpegenc_mp3enc.h"
#include "vppp_audiojpegenc_amrjpegenc.h"


// VoC_directive: voc_stack16
#define SP_16_ADDR                                      RAM_X_MAX - 1 // measured minimum : 0x1000 - 0x27
// VoC_directive: voc_stack32
#define SP_32_ADDR                                      RAM_Y_MAX - 2 // measured minimum : 0x1800 - 0x19


#if 0


voc_struct vpp_AudioJpeg_ENC_IN_STRUCT __attribute__((export)) ,y

voc_short  Input_mode_addr,y  //i  mode
voc_short  Input_reset_addr,y  //i reset
voc_word   Input_inStreamBufAddr_addr,y  //i  Input stream buffer address
voc_word   Input_outStreamBufAddr_addr,y  //i Output stream buffer address
voc_word   Input_samplerate_addr,y
voc_short  Input_bitrate_addr,y
voc_short  Input_JPEGENC_QUALITY_addr,y
voc_short  Input_JPEGENC_WIDTH_addr,y
voc_short  Input_JPEGENC_HEIGHT_addr,y
voc_short  Input_Channel_Num_addr,y
voc_short  Input_reserved_addr,y
//voc_word   Input_DataBufAddr_addr,y
voc_struct_end

voc_alias GlOBAL_ImgInw Input_samplerate_addr,y
voc_alias GlOBAL_ImgInh Input_samplerate_addr+1,y

voc_alias GlOBAL_ImgOutw Input_JPEGENC_WIDTH_addr,y
voc_alias GlOBAL_ImgOuth Input_JPEGENC_HEIGHT_addr,y

voc_struct vpp_AudioJpeg_ENC_OUT_STRUCT __attribute__((export)) ,y

voc_short Output_mode_addr,y//o
voc_short Output_streamStatus_addr,y//o
voc_word Output_output_len_addr,y
voc_word Output_consumedLen_addr,y//o
voc_short Output_ERR_Status_addr,y //o
voc_short Output_reserve0_addr,y

voc_struct_end





voc_struct VPP_MP3_Enc_Code_ExternalAddr_addr __attribute__((export)) ,y


voc_word MP3_Code_ExternalAddr_addr,y
voc_word MP3_ConstX_ExternalAddr_addr,y
voc_word MP3_ConstY_ExternalAddr_addr,y
voc_word MP3_Const_rqmy_ExternalAddr_addr,y
voc_word MP3_Const_zig_ExternalAddr_addr,y

voc_struct_end


voc_struct VPP_AMR_Enc_Code_ExternalAddr_addr __attribute__((export)) ,y

voc_word AMRJPEG_Code_ExternalAddr_addr,y
voc_word AMRJPEG_ConstX_ExternalAddr_addr,y
voc_word AMRJPEG_ConstY_ExternalAddr_addr,y

voc_struct_end

voc_word RGBYUV_ROTATE_Code_ExternalAddr_addr __attribute__((export)) ,y

voc_short GLOBAL_MIC_DIGITAL_GAIN_ADDR __attribute__((export)) ,y

voc_short Output_reserve1_addr,y
#endif



/**************************************************************************************
 * Function:    Coolsand_AACDecode_MAIN
 *
 * Description: decode AAC frame
 *
 * Inputs:      no
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       09/03/2007
 *
 **************************************************************************************/


void Coolsand_AudioEncode_MAIN(void)
{

    // Init : clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);
AudioDecode_MAIN_L0:
    // Enable the SOF1 event only
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
    VoC_lw16i(SP16,SP_16_ADDR);
    VoC_lw16i(SP32,SP_32_ADDR);
    // Clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);
    // Disable the SOF1 event, enable DMAI event.
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_MASK);

    VoC_lw16_d(REG0,Input_mode_addr);
    VoC_be16_rd(AudioDecode_MAIN_L2,REG0,Output_mode_addr);
    VoC_jal(Coolsand_VoC_code_reload);
AudioDecode_MAIN_L2:
    // jal to the main code
    VoC_lw16i_short(REG1,0x10,DEFAULT);//VPP_AAC_DEC_STREAM_STOP;
    VoC_lw16i_short(REG3,-23,IN_PARALLEL);
    VoC_lw16_d(REG0,Input_mode_addr);
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_sw16_d(REG0,Output_mode_addr);
    VoC_bne16_rr(AudioDecode_MAIN_AMR,REG0,REG7)
    VoC_jal(L3_compress);
    VoC_jump(AudioDecode_MAIN_L3);
AudioDecode_MAIN_AMR:
    VoC_lw16i_short(REG6,2,DEFAULT);
    VoC_lw16i_short(REG7,4,IN_PARALLEL);
    VoC_bne16_rr(AudioDecode_MAIN_JPEG,REG0,REG6)
    VoC_jal(COOLSAND_ENCODE_AMRRING);
    VoC_jump(AudioDecode_MAIN_L3);
AudioDecode_MAIN_JPEG:
    VoC_bne16_rr(AudioDecode_MAIN_YUV_ROTATE,REG0,REG7)
    VoC_jal(COOLSAND_RGB2JPEG);
    VoC_jump(AudioDecode_MAIN_L3);

AudioDecode_MAIN_YUV_ROTATE:
    VoC_lw16i_short(REG6,10,DEFAULT);
    VoC_lw16i_short(REG7,11,IN_PARALLEL);
    VoC_bne16_rr(AudioDecode_MAIN_RGB_ROTATE,REG0,REG6);
    VoC_jal(VOC_YUV420_rotate);
    VoC_jump(AudioDecode_MAIN_L3);
AudioDecode_MAIN_RGB_ROTATE:
    VoC_bne16_rr(AudioDecode_MAIN_MODEERR,REG0,REG7);
    VoC_jal(VOC_RGB565_rotate);
    VoC_jump(AudioDecode_MAIN_L3);

AudioDecode_MAIN_MODEERR:
    VoC_sw16_d(REG1,Output_streamStatus_addr);
    VoC_sw16_d(REG3,Output_ERR_Status_addr);
AudioDecode_MAIN_L3:
    VoC_lw16i(REG1,0x0010);//VPP_AAC_DEC_STREAM_STOP;
    VoC_bez16_d(AudioDecode_MAIN_L4,Output_ERR_Status_addr)
    VoC_sw16_d(REG1,Output_streamStatus_addr);
AudioDecode_MAIN_L4:

    VoC_jump(AudioDecode_MAIN_L0);
}


void Coolsand_VoC_code_reload(void)
{

    VoC_lw16i_short(REG1,0,DEFAULT);
    //VoC_lw16_d(REG1,Input_reset_addr);
    VoC_bne16_rr(VoC_code_reload_L2,REG0,REG1);//mp3

    /*****************  DMA MP3 code ************************/
    VoC_lw32_d(ACC0,MP3_Code_ExternalAddr_addr);
    VoC_lw16i(REG2,MP3_DEC_START);//local addr
    VoC_lw16i(REG3,SECTION_MP3_ENC_SIZE);//codesize
    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    /*****************  DMA MP3 code ************************/


    /*****************  DMA MP3 constX ************************/
    VoC_lw32_d(ACC0,MP3_ConstX_ExternalAddr_addr);
    VoC_lw16i(REG2,TABLE_t1HB_tab_ADDR/2);//local addr
    VoC_lw16i(REG3,MP3_ENC_ConstX_size);
    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    /*****************  DMA MP3 constX ************************/


    /*****************  DMA MP3 constY ************************/
    VoC_lw32_d(ACC0,MP3_ConstY_ExternalAddr_addr);
    VoC_lw16i(REG2,TABLE_fl_tab_ADDR/2);//local addr
    VoC_lw16i(REG3,MP3_ENC_ConstY_size);
    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    /*****************  DMA MP3 constY ************************/

    /*****************  DMA MP3 const rqmy ************************/
    VoC_lw32_d(ACC0,MP3_Const_rqmy_ExternalAddr_addr);
    VoC_lw16i(REG2,STATIC_RQ_ADDR/2);//local addr
    VoC_lw16i(REG3,MP3_ENC_Const_rqmy_size);
    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    /*****************  DMA MP3 const rqmy ************************/

    /*****************  DMA MP3 const zig ************************/
    VoC_lw32_d(ACC0,MP3_Const_zig_ExternalAddr_addr);
    VoC_lw16i(REG2,MP3_CONST_inverse_Zig_Zag_ADDR/2);//local addr
    VoC_lw16i(REG3,MP3_ENC_Const_Zig_size);
    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    /*****************  DMA MP3 constY zig ************************/

    VoC_jump(VoC_code_reload_End);
VoC_code_reload_L2:
    VoC_lw16i_short(REG1,2,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);
    //VoC_lw16_d(REG1,Input_reset_addr);
    VoC_be16_rr(VoC_code_reload_L3,REG0,REG1);//AMR
    VoC_be16_rr(VoC_code_reload_L3,REG0,REG2);//JPEG
    VoC_jump(VoC_code_reload_L4);
VoC_code_reload_L3:

    VoC_lw16_d(REG3,Output_mode_addr);
    VoC_be16_rr(VoC_code_reload_L4,REG1,REG3);
    VoC_be16_rr(VoC_code_reload_L4,REG2,REG3);

    /*****************  DMA AMR code ************************/
    VoC_lw32_d(ACC0,AMRJPEG_Code_ExternalAddr_addr);
    VoC_lw16i(REG2,AMR_DEC_START);//local addr
    VoC_lw16i(REG3,SECTION_AMR_ENC_SIZE);//codesize
    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    /*****************  DMA AMR code ************************/

    /*****************  DMA AMR constX ************************/
    VoC_lw32_d(ACC0,AMRJPEG_ConstX_ExternalAddr_addr);
    VoC_lw16i(REG2,CONST_inverse_Zig_Zag_ADDR/2);//local addr
    VoC_lw16i(REG3,AMRJPEG_ConstX_size);//
    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    /*****************  DMA AMR constX ************************/

    /*****************  DMA AMR constY ************************/
    VoC_lw32_d(ACC0,AMRJPEG_ConstY_ExternalAddr_addr);
    VoC_lw16i(REG2,CONST32_ADDR/2);//local addr
    VoC_lw16i(REG3,AMRJPEG_ConstY_size);//
    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    /*****************  DMA AMR constY ************************/

    VoC_jump(VoC_code_reload_End);
VoC_code_reload_L4:
    VoC_lw16i_short(REG1,10,DEFAULT);
    VoC_lw16i_short(REG2,11,IN_PARALLEL);
    VoC_be16_rr(VoC_code_reload_L5,REG0,REG1);
    VoC_be16_rr(VoC_code_reload_L5,REG0,REG2,);
    VoC_jump(VoC_code_reload_L6);
VoC_code_reload_L5:
    /****************DMA YUV code***************/
    VoC_lw32_d(ACC0,RGBYUV_ROTATE_Code_ExternalAddr_addr);
    VoC_lw16i(REG2,RGBYUV_ROTATE_START);//local addr
    VoC_lw16i(REG3,SECTION_RGBYUV_ROTATE_SIZE);//codesize
    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    /*******************************************/
    VoC_jump(VoC_code_reload_End);

VoC_code_reload_L6:
    VoC_NOP();

VoC_code_reload_End:
    VoC_NOP();

    VoC_return;
}



