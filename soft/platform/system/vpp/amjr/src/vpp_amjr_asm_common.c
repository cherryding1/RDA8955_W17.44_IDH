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




#include "vppp_amjr_asm_amr.h"
#include "vppp_amjr_asm_mp3.h"
#include "vppp_amjr_asm_jpeg.h"
#include "vppp_amjr_asm_map.h"
#include "vppp_amjr_asm_sections.h"

#include "voc2_library.h"
#include "voc2_define.h"


#if 0

// voc_enum mode
voc_alias VPP_AMJR_MODE_NO              0x0, x
voc_alias VPP_AMJR_MODE_AMR             0x1, x
voc_alias VPP_AMJR_MODE_MP3             0x2, x
voc_alias VPP_AMJR_MODE_AAC             0x3, x
voc_alias VPP_AMJR_MODE_WMA             0x4, x
voc_alias VPP_AMJR_MODE_JPEG          0x100, x
// voc_enum_end

// voc_enum stream_id
voc_alias VPP_AMJR_STRM_ID_NO           0x0, x
voc_alias VPP_AMJR_STRM_ID_AUDIO        0x1, x
voc_alias VPP_AMJR_STRM_ID_VIDEO        0x2, x
voc_alias VPP_AMJR_STRM_ID_INIT         0x3, x
// voc_enum_end


// voc_enum error
voc_alias VPP_AMJR_ERROR_NONE                   0, x
voc_alias VPP_AMJR_ERROR_UNKNOWN_AUDIO_MODE    -1, x
voc_alias VPP_AMJR_ERROR_UNKNOWN_VIDEO_MODE    -2, x
voc_alias VPP_AMJR_ERROR_VIDEO_BAD_PARAMETER   -3, x
voc_alias VPP_AMJR_ERROR_PREVIEW_BAD_PARAMETER -4, x
// voc_enum_end



voc_struct VPP_AMJR_CFG_STRUCT __attribute__((export)),y

// ----------------
// common config
// ----------------
voc_short GLOBAL_MODE
voc_short GLOBAL_RESET

// ----------------
// audio config
// ----------------
voc_word  GLOBAL_IN_STRM_BUF_START
voc_word  GLOBAL_OUT_STRM_BUF_START
voc_word  GLOBAL_IN_STRM_SAMPLE_RATE
voc_short GLOBAL_OUT_STRM_BIT_RATE
voc_short GLOBAL_IN_STRM_NB_CHAN
voc_word  GLOBAL_IN_STRM_BUF_IFC_PTR

// ----------------
// video config
// ----------------
voc_word  GLOBAL_IN_VIDEO_BUF_START
voc_word  GLOBAL_OUT_VIDEO_BUF_START
voc_short GLOBAL_VIDEO_WIDTH
voc_short GLOBAL_VIDEO_HEIGHT
voc_short GLOBAL_VIDEO_QUALITY
voc_short GLOBAL_VIDEO_IN_FORMAT
voc_short GLOBAL_VIDEO_PXL_SWAP
voc_short GLOBAL_VIDEO_RESERVED
voc_word  GLOBAL_PREVIEW_VIDEO_BUF_START
voc_short GLOBAL_PREVIEW_VIDEO_WIDTH
voc_short GLOBAL_PREVIEW_VIDEO_HEIGHT
voc_short GLOBAL_PREVIEW_VIDEO_OUT_FORMAT
voc_short GLOBAL_PREVIEW_VIDEO_SCALEFACTOR

voc_struct_end


voc_struct VPP_AMJR_STATUS_STRUCT __attribute__((export)),y

// ----------------
// common status
// ----------------
voc_short GLOBAL_USED_MODE
voc_short GLOBAL_ERR_STATUS
voc_short GLOBAL_STRM_STATUS
voc_short GLOBAL_RSVD_STATUS

// ----------------
// audio status
// ----------------
voc_word  GLOBAL_STRM_OUT_LEN
voc_word  GLOBAL_STRM_IN_LEN
voc_word  GLOBAL_IN_STRM_BUF_IFC_CURR

// ----------------
// video status
// ----------------
voc_word  GLOBAL_VIDEO_OUT_LEN

voc_struct_end


voc_struct VPP_AMJR_CODE_CFG_STRUCT __attribute__((export)),y

voc_word  GLOBAL_AMR_CODE_PTR
voc_word  GLOBAL_AMR_CONST_X_PTR
voc_word  GLOBAL_AMR_CONST_Y_PTR
voc_word  GLOBAL_MP3_CODE_PTR
voc_word  GLOBAL_MP3_CONST_X_PTR
voc_word  GLOBAL_MP3_CONST_Y_PTR
voc_word  GLOBAL_JPEG_CODE_PTR
voc_word  GLOBAL_JPEG_CONST_Y_PTR

voc_struct_end

#endif


void vpp_AmjrCodeConstReload(void);



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

AMJR_LABEL_MAIN_LOOP:

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
    // for this reason the video mask (SOF1) is not enabled.
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
    VoC_lw32_d(RL6,GLOBAL_IN_STRM_BUF_IFC_PTR);
    VoC_sw32_d(RL7,GLOBAL_IN_STRM_BUF_IFC_CURR);

    VoC_bez32_r(no_rd_ifc_curr_pos, RL6)

    VoC_lw16i_short(RL7_HI,1,DEFAULT);
    VoC_lw16i(RL7_LO,GLOBAL_IN_STRM_BUF_IFC_CURR/2);

    VoC_sw16_d(RL7_HI,CFG_DMA_SIZE);
    VoC_sw16_d(RL7_LO,CFG_DMA_LADDR);
    VoC_sw32_d(RL6,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

no_rd_ifc_curr_pos:

    // check if code and constants reload is necessary
    // (compare the new and the old (status) mode
    VoC_lw16_d(REG0,GLOBAL_MODE);

    // ----------------------------
    // fct : vpp_AmjpCodeConstReload
    // in :  REG0 -> GLOBAL_MODE_IN
    // not modified : REG0
    // unused : REG45, RL67, ACC1
    // ----------------------------
    VoC_jal(vpp_AmjrCodeConstReload);


    // check for a video stream event & clear the event (if any)
    VoC_lw16i(REG6,CFG_WAKEUP_SOF1);
    VoC_and16_rr(REG6,REG4,DEFAULT);
    VoC_and16_ri(REG4,(~CFG_WAKEUP_SOF1));

    // check if the video mode is enabled & clear the mode (if any)
    VoC_lw16i(REG7,VPP_AMJR_MODE_JPEG);
    VoC_and16_rr(REG7,REG0,DEFAULT);
    VoC_and16_ri(REG0,(~VPP_AMJR_MODE_JPEG));

    // VPP_AMJP_STREAM_ID_NO == 0
    VoC_lw16i_short(REG7,VPP_AMJR_STRM_ID_NO,DEFAULT);

    // store the video event
    VoC_push32(REG67,DEFAULT);

    // clear previous stream errors
    VoC_sw16_d(REG7,GLOBAL_ERR_STATUS);

    // check for an audio stream event
    VoC_bez16_r(AMJR_LABEL_MAIN_NO_AUDIO,REG4);

    // jal to the main code
    VoC_lw16i_short(REG5,VPP_AMJR_MODE_MP3,DEFAULT);
    VoC_lw16i_short(REG7,VPP_AMJR_ERROR_UNKNOWN_AUDIO_MODE,IN_PARALLEL);

    VoC_bne16_rr(AMJR_LABEL_MAIN_NO_MP3,REG0,REG5)

    VoC_jal(vpp_AmjrMp3Encode);
    VoC_jump(AMJR_LABEL_MAIN_AUDIO_DONE);

AMJR_LABEL_MAIN_NO_MP3:

    VoC_lw16i_short(REG5,VPP_AMJR_MODE_AMR,DEFAULT);
    VoC_bne16_rr(AMJR_LABEL_MAIN_NO_AMR,REG0,REG5)

    VoC_jal(vpp_AmjrAmrEncode);
    VoC_jump(AMJR_LABEL_MAIN_AUDIO_DONE);

AMJR_LABEL_MAIN_NO_AMR:
AMJR_LABEL_MAIN_NO_AAC:
AMJR_LABEL_MAIN_MODEERR:

    VoC_sw16_d(REG7,GLOBAL_ERR_STATUS);

AMJR_LABEL_MAIN_AUDIO_DONE:

    VoC_lw16i_short(REG5,VPP_AMJR_STRM_ID_AUDIO,DEFAULT);
    VoC_NOP();

    // status : audio frame decoding done
    VoC_sw16_d(REG5,GLOBAL_STRM_STATUS);

    // generate an interrupt for audio done
    VoC_cfg(CFG_CTRL_IRQ_TO_XCPU | CFG_CTRL_CONTINUE);


AMJR_LABEL_MAIN_NO_AUDIO:

    // pop the video event
    VoC_pop32(REG67,DEFAULT);

    VoC_bez16_r(AMJR_LABEL_MAIN_NO_VIDEO,REG6);

    VoC_lw16i_short(REG6,VPP_AMJR_ERROR_UNKNOWN_VIDEO_MODE,DEFAULT);

    VoC_bez16_r(AMJR_LABEL_MAIN_NO_JPEG,REG7);

    VoC_jal(vpp_AmjrPreviewAndJpegEncode);

    VoC_lw16i_short(REG5,VPP_AMJR_STRM_ID_VIDEO,DEFAULT);
    VoC_NOP();

    // status : video frame decoding done
    VoC_sw16_d(REG5,GLOBAL_STRM_STATUS);

    // generate an interrupt for video done
    VoC_cfg(CFG_CTRL_IRQ_TO_XCPU | CFG_CTRL_CONTINUE);

    VoC_jump(AMJR_LABEL_MAIN_NO_VIDEO);

AMJR_LABEL_MAIN_NO_JPEG:
AMJR_LABEL_MAIN_VIDEOEERR:

    VoC_sw16_d(REG6,GLOBAL_ERR_STATUS);

AMJR_LABEL_MAIN_NO_VIDEO:


    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_NOP();

    // clear the reset
    VoC_sw16_d(REG0,GLOBAL_RESET);

    VoC_jump(AMJR_LABEL_MAIN_LOOP);

}


void vpp_AmjrRunDma(void)
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
// fct : vpp_AmjrCodeConstReload
// in :  REG0 -> GLOBAL_MODE_IN
// not modified : REG0
// unused : REG45, RL67, ACC1
// ----------------------------

void vpp_AmjrCodeConstReload(void)
{
    // old (status) mode
    VoC_lw16_d(REG1,GLOBAL_USED_MODE);
    VoC_be16_rr(CODE_RELOAD_NO,REG0,REG1);

    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(REG6,0xf,DEFAULT);
    VoC_lw16i_short(REG7,0xf,IN_PARALLEL);

    VoC_and16_rr(REG6,REG0,DEFAULT);
    VoC_and16_rr(REG7,REG1,DEFAULT);

    // check if sec2 is to be reloaded
    VoC_be16_rr(CODE_RELOAD_NO_SEC2,REG6,REG7);

    // check if the new mode is AMR
    VoC_lw16i_short(REG7,VPP_AMJR_MODE_AMR,DEFAULT);
    VoC_bne16_rr(CODE_RELOAD_NO_AMR,REG6,REG7);

    // AMR code
    VoC_lw32_d(ACC0,GLOBAL_AMR_CODE_PTR);
    VoC_lw16i(REG2,SECTION_AMR_ENC_START);
    VoC_lw16i(REG3,SECTION_AMR_ENC_SIZE);
    VoC_jal(vpp_AmjrRunDma);

    // AMR const X
    VoC_lw32_d(ACC0,GLOBAL_AMR_CONST_X_PTR);
    VoC_lw16i(REG2,VPP_AMJR_AMR_CONST_X_STRUCT/2);
    VoC_lw16i(REG3,AMR_ENC_ConstX_size);
    VoC_jal(vpp_AmjrRunDma);

    // AMR const Y
    VoC_lw32_d(ACC0,GLOBAL_AMR_CONST_Y_PTR);
    VoC_lw16i(REG2,VPP_AMJR_AMR_CONST_Y_STRUCT/2);
    VoC_lw16i(REG3,AMR_ENC_ConstY_size);
    VoC_jal(vpp_AmjrRunDma);

CODE_RELOAD_NO_AMR:

    // check if the new mode is MP3
    VoC_lw16i_short(REG7,VPP_AMJR_MODE_MP3,DEFAULT);
    VoC_bne16_rr(CODE_RELOAD_NO_MP3,REG6,REG7);

    // MP3 code
    VoC_lw32_d(ACC0,GLOBAL_MP3_CODE_PTR);
    VoC_lw16i(REG2,SECTION_MP3_ENC_START);
    VoC_lw16i(REG3,SECTION_MP3_ENC_SIZE);
    VoC_jal(vpp_AmjrRunDma);

    // MP3 const X
    VoC_lw32_d(ACC0,GLOBAL_MP3_CONST_X_PTR);
    VoC_lw16i(REG2,VPP_AMJR_MP3_CONST_X_STRUCT/2);
    VoC_lw16i(REG3,MP3_ENC_ConstX_size);
    VoC_jal(vpp_AmjrRunDma);

    // MP3 const Y
    VoC_lw32_d(ACC0,GLOBAL_MP3_CONST_Y_PTR);
    VoC_lw16i(REG2,VPP_AMJR_MP3_CONST_Y_STRUCT/2);
    VoC_lw16i(REG3,MP3_ENC_ConstY_size);
    VoC_jal(vpp_AmjrRunDma);

CODE_RELOAD_NO_MP3:
CODE_RELOAD_NO_AAC:
CODE_RELOAD_NO_WMA:
CODE_RELOAD_NO_SEC2:

    VoC_lw16i(REG6,VPP_AMJR_MODE_JPEG);
    VoC_lw16i_short(REG7,REG6,DEFAULT);

    VoC_and16_rr(REG6,REG0,DEFAULT);
    VoC_and16_rr(REG7,REG1,DEFAULT);

    // check if the new mode includes JPEG, while the old does not
    VoC_bngt16_rr(CODE_RELOAD_NO_SEC1,REG6,REG7);

    // JPEG code
    VoC_lw32_d(ACC0,GLOBAL_JPEG_CODE_PTR);
    VoC_lw16i(REG2,SECTION_JPEG_ENC_START);
    VoC_lw16i(REG3,SECTION_JPEG_ENC_SIZE);
    VoC_jal(vpp_AmjrRunDma);

    // JPEG const Y
    VoC_lw32_d(ACC0,GLOBAL_JPEG_CONST_Y_PTR);
    VoC_lw16i(REG2,VPP_AMJR_JPEG_CONST_STRUCT/2);
    VoC_lw16i(REG3,JPEG_ConstY_size);
    VoC_jal(vpp_AmjrRunDma);

CODE_RELOAD_NO_SEC1:

    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL);

    // update the mode status
    VoC_sw16_d(REG0,GLOBAL_USED_MODE);

    // re-enable the global reset
    VoC_sw16_d(REG1,GLOBAL_RESET);

CODE_RELOAD_NO:

    VoC_return;
}





// **************************************************************************************
//   Function:    vpp_AmjrDecompressVssAdpcmTable
//
//   Description: decompress a table compressed using Varible Sample Size and recursive ADPCM
//
//   Inputs:      REG0-> destination (decompressed table); inrc0=2
//                REG1-> source (compressed table); inrc1=1
//                REG2-> table size, incr2 = -1;
//                REG3-> iterations, incr3 = -1;
//                REG4-> step;
//                REG5-> size/step;
//                REG6-> vss mode (0 -> 4,32; 1 -> 8,16,32);
//   Outputs:
//
//   Used :       REG01234567
//
//   Version 1.0  Create by  Yordan  07/10/2008
//
//   *************************************************************************************

void vpp_AmjrDecompressVssAdpcmTable(void)
{

    // Because of the regularity of their pattern, some tables can be coded
    // with an ADPCM based algo : the difference of the samples are computed,
    // then the difference of the difference, etc... After 5 iterations
    // almost all resulting values can be coded on majority on 4 or 8 bits.
    // Further compressing involves saving the samples on variable size.
    // In order to distinguish between the different possible sizes,
    // value on more than 4 bits are preceeded by a special codeword
    // (example with 4,8,32 compression):
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
    // example with 4,8,32 compression
    // -----------------------------

    // if val = tableCompressed[i] is in [-0x7, 0x7]
    // write the 4 lsb bits of val in the compressed bitstream
    // else if val = tableCompressed[i] is in [-0x81, 0x7f]
    // write the 8 lsb bits of val preceeded by 0x8 on 4 bits in the compressed bitstream (12 bits written)
    // else val = tableCompressed[i]
    // write the 32 bits of val preceeded by 0x808 on 12 bits in the compressed bitstream (44 bits written)

    // -----------------------------
    // variable sample size decompression
    // example with 4,8,32 compression
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

    VoC_bez16_r(continue_vbs_4_32,REG6)
    VoC_NOP();

continue_vbs_8_16_32:

    // ------------------------
    // 8,16,32 VSS decompression
    // ------------------------

    VoC_lbinc_p(REG1);

    // val = (signed)(read next 8 bits)
    VoC_rbinc_i(REG6,8,DEFAULT);

    VoC_shru32_ri(REG67,-24,DEFAULT);
    VoC_movreg16(REG4,REG6,IN_PARALLEL);

    VoC_shr32_ri(REG67,24,DEFAULT);
    VoC_lw16i(REG5,0x80);

    VoC_bne16_rr(write_uncompressed_sample_8_16_32,REG4,REG5);

    VoC_lbinc_p(REG1);

    // val = (signed)(read next 16 bits)
    VoC_rbinc_i(REG7,16,DEFAULT);

    VoC_shr32_ri(REG67,16,DEFAULT);
    VoC_shru16_ri(REG5,-8,IN_PARALLEL);

    VoC_bne16_rr(write_uncompressed_sample_8_16_32,REG6,REG5);

    // val = (signed)(read next 32 bits)
    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG6,16,DEFAULT);

    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG7,16,DEFAULT);

    VoC_NOP();

write_uncompressed_sample_8_16_32:

    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);

    VoC_bnez16_r(continue_vbs_8_16_32,REG2);

    VoC_jump(do_recursive_adpcm);


continue_vbs_4_32:

    // ------------------------
    // 4,32 VSS decompression
    // ------------------------

    VoC_lbinc_p(REG1);

    // val = (signed)(read next 4 bits)
    VoC_rbinc_i(REG6,4,DEFAULT);

    VoC_shru32_ri(REG67,-28,DEFAULT);
    VoC_movreg16(REG4,REG6,IN_PARALLEL);

    VoC_shr32_ri(REG67,28,DEFAULT);
    VoC_lw16i_short(REG5,0x8,IN_PARALLEL);

    VoC_bne16_rr(write_uncompressed_sample_4_32,REG4,REG5);

    // val = (signed)(read next 32 bits)
    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG6,16,DEFAULT);

    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG7,16,DEFAULT);

    VoC_NOP();

write_uncompressed_sample_4_32:

    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);

    VoC_bnez16_r(continue_vbs_4_32,REG2);

do_recursive_adpcm:

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
