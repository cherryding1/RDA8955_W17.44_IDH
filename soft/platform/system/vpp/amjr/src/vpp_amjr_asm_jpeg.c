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




#include "vppp_amjr_asm_jpeg.h"
#include "vppp_amjr_asm_map.h"
#include "vppp_amjr_asm_sections.h"

#include "voc2_library.h"
#include "voc2_define.h"

#if 0

// --------------------------
// JPEG global vars in RAM_X
// --------------------------

// --------------------------
// JPEG global vars in RAM_Y
// --------------------------

voc_struct VPP_AMJR_JPEG_CONST_STRUCT,y

voc_short JPEGENC_MYDCT_ADDR[12]
voc_short CONST_MYQUN_TAB0_ADDR[128]
voc_short CONST_MYQUN_TAB1_ADDR[128]
voc_short CONST_MYHUFF_DC0_ADDR[24]
voc_short CONST_MYHUFF_DC1_ADDR[24]
voc_word  CONST_MYHUFF_AC_COMPRESSED[179]
voc_short MATRIC_RGB_ADDR[8]
voc_short CONST_inverse_Zig_Zag_ADDR[64]
voc_short CONST_0xff_ADDR
voc_short CONST_NEG7_ADDR
voc_short CONST_640_INT_ADDR
voc_short CONST_640BY8_INT_ADDR
voc_short CONST_640BY9_INT_ADDR
voc_short CONST_479_ADDR
voc_word  CONST_640_ADDR
voc_word  CONST_0X80_ADDR

voc_struct_end

voc_alias CONST_0_ADDR            0 + CONST_inverse_Zig_Zag_ADDR    ,y
voc_alias CONST_1_ADDR            1 + CONST_inverse_Zig_Zag_ADDR    ,y
voc_alias CONST_7_ADDR           28 + CONST_inverse_Zig_Zag_ADDR    ,y
voc_alias CONST_8_ADDR            2 + CONST_inverse_Zig_Zag_ADDR    ,y
voc_alias CONST_15_ADDR          42 + CONST_inverse_Zig_Zag_ADDR    ,y
voc_alias CONST_16_ADDR           3 + CONST_inverse_Zig_Zag_ADDR    ,y
voc_alias CONST_24_ADDR           9 + CONST_inverse_Zig_Zag_ADDR    ,y

voc_short GLOBAL_WIDTH_SAVE_ADDR         ,y
voc_short GLOBAL_HEIGHT_SAVE_ADDR        ,y

#endif


// ************************************************
//   Function:     vpp_AmjrPreviewAndJpegEncode
//   Description:  MJPEG & Preview switch
//   Inputs:
//   Outputs:
//   Used :        all
//   Version 1.0   Create by Yordan  29/10/2008
//   **********************************************

void vpp_AmjrPreviewAndJpegEncode(void)
{

    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(REG6,VPP_AMJR_ERROR_VIDEO_BAD_PARAMETER,DEFAULT);

    // skip preview if the preview pointer is NULL
    VoC_bez16_d(no_preview,GLOBAL_PREVIEW_VIDEO_BUF_START)

    //VoC_jal(vpp_AmjrPreviewZoom);
    VoC_jal(vpp_AmjrPreview);
    VoC_lw16i_short(REG6,VPP_AMJR_ERROR_NONE,DEFAULT);

no_preview:

    // skip jpeg encoding if the output pointer is NULL
    VoC_bez16_d(no_jpeg_encode,GLOBAL_OUT_VIDEO_BUF_START)

    VoC_jal(vpp_AmjrJpegEncode);
    VoC_lw16i_short(REG6,VPP_AMJR_ERROR_NONE,DEFAULT);

no_jpeg_encode:

    VoC_pop16(RA,DEFAULT);

    // do not overwrite previous errors
    VoC_bnez16_d(keep_previous_error,GLOBAL_ERR_STATUS)

    VoC_sw16_d(REG6,GLOBAL_ERR_STATUS);

keep_previous_error:

    VoC_return;
}

// ************************************************
//   Function:     vpp_AmjrPreview
//   Description:  Perform drop & crop on the input
//                 bmp and write to output buffer
//   Inputs:
//   Outputs:
//   Used :        all
//   Version 1.0   Create by Yordan  29/10/2008
//   **********************************************

void vpp_AmjrPreview(void)
{

#if 0

    voc_short BMP_IN_PREVIEW_BUFF[1280],x
    voc_short BMP_OUT_PREVIEW_BUFF[1280],x

    voc_struct LOCAL_PREVIEW_RESCALE,y

    voc_short LOCAL_PREVIEW_HEIGHT_DROP
    voc_short LOCAL_PREVIEW_HEIGHT_CROP
    voc_short LOCAL_PREVIEW_WIDTH_DROP
    voc_short LOCAL_PREVIEW_WIDTH_CROP

    voc_struct_end


#endif

    // ------------------------------
    // Compute the rescale parameters
    // ------------------------------

    VoC_lw16i_set_inc(REG1,LOCAL_PREVIEW_RESCALE,1);

    // first turn (height)
    VoC_lw16_d(REG4,GLOBAL_VIDEO_HEIGHT);
    VoC_lw16_d(REG5,GLOBAL_PREVIEW_VIDEO_HEIGHT);


    VoC_loop_i(1,2)

    VoC_lw16i_set_inc(REG0,0,1);

    VoC_loop_i_short(15,DEFAULT)
    VoC_startloop0

    VoC_bgt16_rr(preview_drop_done,REG5,REG4);

    VoC_inc_p(REG0,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG5,IN_PARALLEL);

    VoC_endloop0

preview_drop_error:

    // division is not finished
    VoC_lw16i_short(REG6,VPP_AMJR_ERROR_PREVIEW_BAD_PARAMETER,DEFAULT);
    VoC_jump(preview_end);

preview_drop_done:

    // check whether the drop value is valid
    VoC_bez16_r(preview_drop_error,REG0);

    // crop the same nb of pxls on both sides
    VoC_shr16_ri(REG4,1,DEFAULT);

    // save the drop and crop values
    VoC_sw16inc_p(REG0,REG1,DEFAULT);
    VoC_sw16inc_p(REG4,REG1,DEFAULT);

    // second turn (width)
    VoC_lw16_d(REG4,GLOBAL_VIDEO_WIDTH);
    VoC_lw16_d(REG5,GLOBAL_PREVIEW_VIDEO_WIDTH);

    VoC_endloop1


    // ------------------------------
    // Do the rescale
    // ------------------------------

    // do GLOBAL_PREVIEW_VIDEO_HEIGHT times
    // {
    //    read one out of LOCAL_PREVIEW_HEIGHT_DROP lines starting from line N癓OCAL_PREVIEW_HEIGHT_CROP
    //    do GLOBAL_PREVIEW_VIDEO_WIDTH times
    //    {
    //        read one out of LOCAL_PREVIEW_WIDTH_DROP pixels starting from pixel N癓OCAL_PREVIEW_WIDTH_DROP
    //    }
    //    write rescaled line to preview buffer
    // }

    VoC_lw16i_short(FORMAT32,-1,DEFAULT);

    // compute the start pointer offset (vertical crop)
    // in_start[height_crop][0] = in_start + (in_width * height_crop * 2)
    VoC_lw16_d(REG4,GLOBAL_VIDEO_WIDTH);
    VoC_lw32_d(ACC0,GLOBAL_IN_VIDEO_BUF_START);
    VoC_mac32_rd(REG4,LOCAL_PREVIEW_HEIGHT_CROP);

    // input eaddr step = in_width * height_drop * 2
    VoC_multf32_rd(RL6,REG4,LOCAL_PREVIEW_HEIGHT_DROP);

    // input laddr
    VoC_lw16i(REG5,(BMP_IN_PREVIEW_BUFF/2));

    // input line pointer and inc
    VoC_lw16i(REG0,BMP_IN_PREVIEW_BUFF);
    VoC_add16_rd(REG0,REG0,LOCAL_PREVIEW_WIDTH_CROP);
    VoC_lw16_d(INC0,LOCAL_PREVIEW_WIDTH_DROP);

    // output line pointer and inc
    VoC_lw16i_set_inc(REG1,BMP_OUT_PREVIEW_BUFF,1);

    VoC_lw16_d(REG3,GLOBAL_PREVIEW_VIDEO_HEIGHT);
    VoC_lw16_d(REG6,GLOBAL_PREVIEW_VIDEO_WIDTH);

    // output laddr
    VoC_lw16i(REG7,(BMP_OUT_PREVIEW_BUFF/2));

    // output eaddr
    VoC_lw32_d(ACC1,GLOBAL_PREVIEW_VIDEO_BUF_START);

    // output eaddr step
    VoC_movreg16(RL7_LO,REG6,DEFAULT);
    VoC_lw16i_short(RL7_HI,0,IN_PARALLEL);
    VoC_shr32_ri(RL7,-1,DEFAULT);

    // input size in words
    VoC_shr16_ri(REG4,1,DEFAULT);

    // output size in words
    VoC_shr16_ri(REG6,1,IN_PARALLEL);

    // repeat GLOBAL_PREVIEW_VIDEO_HEIGHT times
    VoC_loop_r(1,REG3)

    // read one line
    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    // save pointers
    VoC_push32(REG01,DEFAULT);

    // update in eaddr
    VoC_add32_rr(ACC0,ACC0,RL6,IN_PARALLEL);

    // wait the transfer to finish
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    // repeat GLOBAL_PREVIEW_VIDEO_WIDTH times
    VoC_loop_r_short(REG6,DEFAULT)
    VoC_lw16inc_p(REG2,REG0,IN_PARALLEL);
    VoC_startloop0

    VoC_lw16inc_p(REG2,REG0,DEFAULT);
    VoC_sw16inc_p(REG2,REG1,DEFAULT);
    VoC_lw16inc_p(REG2,REG0,DEFAULT);
    VoC_sw16inc_p(REG2,REG1,DEFAULT);

    VoC_endloop0

    // store one preview line
    VoC_sw32_d(REG67,CFG_DMA_SIZE);
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);

    // restore the pointers
    VoC_pop32(REG01,DEFAULT);

    // update out eaddr
    VoC_add32_rr(ACC1,ACC1,RL7,IN_PARALLEL);

    // wait the transfer to finish
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_endloop1

    // no error
    VoC_lw16i_short(REG6,VPP_AMJR_ERROR_NONE,DEFAULT);

preview_end:

    VoC_bez16_r(preview_return,REG6)

    VoC_sw16_d(REG6,GLOBAL_ERR_STATUS);

preview_return:


    VoC_return;
}

// ************************************************
//   Function:     vpp_AmjrPreviewZoom
//   Description:  Perform drop & crop on the input
//                 bmp and write to output buffer
//   Inputs:
//   Outputs:
//   Used :        all
//   Version 1.0   Create by Yordan  29/10/2008
//   **********************************************

void vpp_AmjrPreviewZoom(void)
{

#if 0

    voc_short BMP_IN_PREVIEW_BUFF[1280],x
    voc_short BMP_OUT_PREVIEW_BUFF[1280],x

    voc_struct LOCAL_PREVIEW_RESCALE,y

    voc_short LOCAL_PREVIEW_HEIGHT_DROP
    voc_short LOCAL_PREVIEW_HEIGHT_CROP
    voc_short LOCAL_PREVIEW_WIDTH_DROP
    voc_short LOCAL_PREVIEW_WIDTH_CROP

    voc_struct_end

#endif

    // ------------------------------
    // Calculate drop := incr = (1<<INC_PRECISION)*ZOOM_PRECISION/(zoom+1);
    // ------------------------------

    VoC_lw16_d(REG6, GLOBAL_PREVIEW_VIDEO_SCALEFACTOR);

    // ZOOM_PRECISION
    VoC_lw16i(REG5, ZOOM_PRECISION);

    VoC_lw16i_short(REG7, 1, DEFAULT);
    VoC_lw16i_short(FORMAT32, 0, IN_PARALLEL);

    // Add to zoom 1 to round up
    VoC_add16_rr(REG6, REG6, REG7, DEFAULT);
    // 1 << INC_PRECISION
    VoC_shr16_ri(REG7, -INC_PRECISION, IN_PARALLEL);

    // (1<<INC_PRECISION)*ZOOM_PRECISION
    VoC_multf32_rr(REG23, REG7, REG5, DEFAULT);
    VoC_lw16i_short(REG7, 0, IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,0,1);

    VoC_bgt32_rr(preview_calc_drop_done,REG67,REG23);

preview_division:

    // incr = (1<<INC_PRECISION)*ZOOM_PRECISION/(zoom+1);
    VoC_inc_p(REG0,DEFAULT);
    VoC_sub32_rr(REG23,REG23,REG67,IN_PARALLEL);
    VoC_bngt32_rr(preview_division,REG67, REG23);

preview_calc_drop_done:



    // ------------------------------
    // Do the rescale
    // ------------------------------

    // Express src dimension in 2048th of pixels
    // Express needed src pixels to construct destination in 2048th of pixels
    // Calculate number of pixels not used to construct destination and divide by 2 to center
    // Make the result even
    // srcStartX = ((((SrcFbw->fb.width<<INC_PRECISION)-(DstFbw->roi.width*incr))>>(INC_PRECISION+1))>>1)<<1 ;
    // srcStartY = ((((SrcFbw->fb.height<<INC_PRECISION)-(DstFbw->roi.height*incr))>>(INC_PRECISION+1))>>1)<<1;

    VoC_lw16_d(ACC1_LO,GLOBAL_VIDEO_WIDTH);
    VoC_lw16_d(ACC0_LO,GLOBAL_VIDEO_HEIGHT);

    VoC_lw16i_short(ACC1_HI, 0, DEFAULT);
    VoC_lw16i_short(ACC0_HI, 0, IN_PARALLEL);

    // (SrcFbw->fb.width<<INC_PRECISION)
    VoC_shr32_ri(ACC1, -INC_PRECISION, DEFAULT);
    // (SrcFbw->fb.height<<INC_PRECISION)
    VoC_shr32_ri(ACC0, -INC_PRECISION, IN_PARALLEL);

    // (SrcFbw->fb.height<<INC_PRECISION)-(DstFbw->roi.height*incr)
    VoC_msu32_rd(REG0, GLOBAL_PREVIEW_VIDEO_HEIGHT);

    // incr
    VoC_movreg16(RL6_LO, REG0, DEFAULT);
    VoC_lw16i_short(REG5, 1, IN_PARALLEL);

    VoC_lw16i_short(REG4, INC_PRECISION, DEFAULT);

    // INC_PRECISION + 1
    VoC_add16_rr(REG5, REG5, REG4, DEFAULT);
    VoC_movreg32(REG23, ACC0, IN_PARALLEL);

    VoC_movreg32(ACC0, ACC1, DEFAULT);
    // ((SrcFbw->fb.height<<INC_PRECISION)-(DstFbw->roi.height*incr))>>(INC_PRECISION+1)
    VoC_shr32_rr(REG23, REG5, IN_PARALLEL);

    // (SrcFbw->fb.width<<INC_PRECISION)-(DstFbw->roi.width*incr)
    VoC_msu32_rd(REG0, GLOBAL_PREVIEW_VIDEO_WIDTH);
    VoC_shr32_ri(REG23, 1, DEFAULT);
    // REG23 <= srcStartY
    VoC_shr32_ri(REG23, -1, DEFAULT);

    // ( (SrcFbw->fb.width<<INC_PRECISION)-(DstFbw->roi.width*incr))>>(INC_PRECISION+1))
    VoC_shr32_rr(ACC0, REG5, IN_PARALLEL);

    VoC_shr32_ri(ACC0, 1, DEFAULT);

    VoC_shr32_ri(ACC0, -1, DEFAULT);
    VoC_lw16i_short(INC2, 1, IN_PARALLEL);

    // srcStartX
    VoC_movreg16(REG5, ACC0_LO, DEFAULT);
    VoC_lw16i_short(INC0, 2, IN_PARALLEL);

    VoC_lw32_d(ACC0, GLOBAL_IN_VIDEO_BUF_START);
    // input eadr = SrcFbw->fb.buffer + srcStartY*SrcFbw->fb.width + srcStartX
    VoC_lw16_d(REG6, GLOBAL_VIDEO_WIDTH);
    VoC_mac32_rr(REG2, REG6, DEFAULT);
    // input size in words
    VoC_shr16_ri(REG6, 1, IN_PARALLEL);
    // input laddr
    VoC_lw16i(REG7,(BMP_IN_PREVIEW_BUFF/2));

    VoC_lw16i_set_inc(REG0, BMP_IN_PREVIEW_BUFF, 2);
    // output eadr
    VoC_lw32_d(ACC1,GLOBAL_PREVIEW_VIDEO_BUF_START);
    // output size
    VoC_lw16_d(REG4, GLOBAL_PREVIEW_VIDEO_WIDTH);

    VoC_add16_rr(REG0, REG0, REG5, DEFAULT);
    // incrY=0;
    VoC_lw32z(RL7,IN_PARALLEL);

    // output laddr
    VoC_lw16i(REG5,(BMP_OUT_PREVIEW_BUFF/2));
    // output size in words
    VoC_shr16_ri(REG4, 1, DEFAULT);

    // output line pointer and inc
    VoC_lw16i_set_inc(REG1,BMP_OUT_PREVIEW_BUFF,1);


    VoC_lw16_d(REG3,GLOBAL_PREVIEW_VIDEO_HEIGHT);
    VoC_lw16_d(REG2,GLOBAL_PREVIEW_VIDEO_WIDTH);

    // repeat GLOBAL_PREVIEW_VIDEO_HEIGHT times
    VoC_loop_r(1,REG3)

    // read one line
    VoC_sw32_d(REG67,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    // save pointers
    VoC_push32(REG01,DEFAULT);
    VoC_push16(REG2, IN_PARALLEL);

    // Save input buffer
    VoC_movreg16(RL6_HI, REG0, DEFAULT);
    VoC_movreg16(REG3, REG0, IN_PARALLEL);

    // wait the transfer to finish
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_lw16inc_p(ACC0_HI,REG0,DEFAULT);
    VoC_lw16inc_p(ACC0_LO,REG0,IN_PARALLEL);

    // Swap pixels in the line GLOBAL_PREVIEW_VIDEO_WIDTH times
    VoC_loop_r_short(REG6,DEFAULT)
    VoC_lw16i_short(INC3, 2, IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(ACC0_HI,REG0,DEFAULT);
    VoC_lw16inc_p(ACC0_LO,REG0,IN_PARALLEL);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_endloop0


    VoC_movreg32(ACC0, REG45, DEFAULT);
    VoC_movreg16(REG0, RL6_HI, IN_PARALLEL);

    // incrX=0;
    VoC_lw32z(REG45, DEFAULT);
    // incrY
    VoC_push32(RL7, IN_PARALLEL);

    VoC_lw16i_short(RL6_HI, 0, DEFAULT);
    VoC_shr16_ri(REG2, -1, IN_PARALLEL);

    // repeat GLOBAL_PREVIEW_VIDEO_WIDTH times
    VoC_loop_r_short(REG2,DEFAULT)
    // *scrptr
    VoC_lw16_p(REG3,REG0,IN_PARALLEL);
    VoC_startloop0

    //  incrX+= incr
    VoC_add32_rr(REG45, REG45, RL6,  DEFAULT);

    // locincr = (incrX>>INC_PRECISION)
    VoC_shr32_ri(REG45, INC_PRECISION, DEFAULT);
    VoC_movreg32(RL7, REG45, IN_PARALLEL);

    // (srcptr + (incrX>>INC_PRECISION))
    VoC_add16_rr(REG2, REG0, REG4, DEFAULT);
    VoC_sw16inc_p(REG3,REG1,IN_PARALLEL);

    VoC_NOP();
    VoC_NOP();

    VoC_lw16_p(REG3,REG2,DEFAULT);
    // REG3 <= incrX
    VoC_movreg32(REG45, RL7, IN_PARALLEL);

    VoC_endloop0

    // incrY
    VoC_pop32(REG01, DEFAULT);
    VoC_movreg32(REG45, ACC0, IN_PARALLEL);

    // REG0 <= incrY
    VoC_lw16i_short(RL6_HI, 0, DEFAULT);

    //  GLOBAL_PREVIEW_VIDEO_WIDTH
    VoC_lw16_d(REG2, GLOBAL_PREVIEW_VIDEO_WIDTH);
    //  incrY += incr;
    VoC_add32_rr(REG01, REG01, RL6, IN_PARALLEL);

    VoC_movreg32(RL7, REG01, DEFAULT);
    VoC_lw16i_short(REG3, 0, IN_PARALLEL);

    VoC_shr32_ri(REG23, -1, DEFAULT);

    // store one preview line
    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);
    // SrcFbw->fb.buffer
    VoC_lw32_d(ACC0, GLOBAL_IN_VIDEO_BUF_START);

    // update out eaddr
    // dstptr += DstFbw->fb.width;
    VoC_add32_rr(ACC1,ACC1,REG23,DEFAULT);
    // (incrY>>INC_PRECISION)
    VoC_shr32_ri(REG01, INC_PRECISION, IN_PARALLEL);

    VoC_shr32_ri(REG01, -1, DEFAULT);
    VoC_pop16(REG2, IN_PARALLEL);

    //  srcptr = SrcFbw->fb.buffer + (incrY>>INC_PRECISION)*SrcFbw->fb.width;
    VoC_mac32_rd(REG0, GLOBAL_VIDEO_WIDTH);

    VoC_pop32(REG01, DEFAULT);

    // wait the transfer to finish
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_endloop1

    // no error
    VoC_lw16i_short(REG6,VPP_AMJR_ERROR_NONE,DEFAULT);

preview_zoom_end:

    VoC_bez16_r(preview_zoom_return,REG6)

    VoC_sw16_d(REG6,GLOBAL_ERR_STATUS);

preview_zoom_return:


    VoC_return;
}



// ************************************************
//   Function:     vpp_AmjrJpegEncode
//   Description:  Encore a JPEG frame
//   Inputs:
//   Outputs:
//   Used :        all
//   Version 1.0   Create by Yordan  29/10/2008
//   **********************************************

void vpp_AmjrJpegEncode(void)
{

#if 0

    // --------------------------
    // JPEG local vars in RAM_X
    // --------------------------

    voc_struct VPP_AMJR_JPEG_YUV_STRUCT,x

    voc_short JPEGENC_BUFF_Y_ADDR[64]
    voc_short JPEGENC_BUFF_Y1_ADDR[64]
    voc_short JPEGENC_BUFF_Y2_ADDR[64]
    voc_short JPEGENC_BUFF_Y3_ADDR[64]
    voc_short JPEGENC_BUFF_CB_ADDR[64]
    voc_short JPEGENC_BUFF_CR_ADDR[64]

    voc_struct_end

    voc_short GLOBAL_4WIDTH_ADDR            ,x
    voc_short GLOBAL_OLD_DCY_ADDR           ,x
    voc_short GLOBAL_OLD_DCU_ADDR           ,x
    voc_short GLOBAL_OLD_DCV_ADDR           ,x

    voc_struct VPP_AMJR_JPEG_RGB_STRUCT,x

    voc_short JPEGENC_BUFF_RGB_640_0_ADDR[640]
    voc_short JPEGENC_BUFF_RGB_640_1_ADDR[640]
    voc_short JPEGENC_BUFF_RGB_640_2_ADDR[640]
    voc_short JPEGENC_BUFF_RGB_640_3_ADDR[640]
    voc_short JPEGENC_BUFF_RGB_640_4_ADDR[640]
    voc_short JPEGENC_BUFF_RGB_640_5_ADDR[640]
    voc_short JPEGENC_BUFF_RGB_640_6_ADDR[640]
    voc_short JPEGENC_BUFF_RGB_640_7_ADDR[640]
    voc_short JPEGENC_BUFF_RGB_640_8_ADDR[640]

    voc_struct_end


    // --------------------------
    // JPEG local vars in RAM_Y
    // --------------------------

    voc_short PUT_BITS_BUF[512]             ,y

    voc_word  CONST_BLOCKSIZE_BYTE_ADDR     ,y
    voc_short CONST_WIDTH_ADD8              ,y
    voc_short CONST_WIDTH_SUB8              ,y
    voc_short CONST_WIDTH_MPY8              ,y
    voc_short CONST_WIDTH_MPY8_ADD8         ,y

    voc_short GLOBAL_SHORT_1_ADDR           ,y
    voc_short GLOBAL_CURR_POS_ADDR          ,y
    voc_short GLOBAL_CURRBITS_ADDR          ,y
    voc_short GLOBAL_CURRBYTE_ADDR          ,y
    voc_short GLOBAL_pp_ADDR                ,y
    voc_short GLOBAL_Q_ADDR                 ,y

    voc_struct VPP_AMJR_JPEG_OUT_STRUCT,y

    voc_short GLOBAL_OUT_FOR1280BY1024_ADDR[48]
    voc_short GLOBAL_OUT_48_1_ADDR[48]
    voc_short GLOBAL_OUT_48_2_ADDR[48]
    voc_short GLOBAL_OUT_48_3_ADDR[48]
    voc_short GLOBAL_OUT_48_4_ADDR[48]
    voc_short GLOBAL_OUT_48_5_ADDR[48]
    voc_short GLOBAL_OUT_48_6_ADDR[48]
    voc_short GLOBAL_OUT_48_7_ADDR[48]

    voc_struct_end

    voc_alias GLOBAL_OUT_32_1_ADDR   32 + GLOBAL_OUT_FOR1280BY1024_ADDR ,y
    voc_alias GLOBAL_OUT_32_2_ADDR   32 + GLOBAL_OUT_32_1_ADDR          ,y
    voc_alias GLOBAL_OUT_32_3_ADDR   32 + GLOBAL_OUT_32_2_ADDR          ,y
    voc_alias GLOBAL_OUT_32_4_ADDR   32 + GLOBAL_OUT_32_3_ADDR          ,y
    voc_alias GLOBAL_OUT_32_5_ADDR   32 + GLOBAL_OUT_32_4_ADDR          ,y
    voc_alias GLOBAL_OUT_32_6_ADDR   32 + GLOBAL_OUT_32_5_ADDR          ,y
    voc_alias GLOBAL_OUT_32_7_ADDR   32 + GLOBAL_OUT_32_6_ADDR          ,y

    voc_short CONST_MYHUFF_AC0_ADDR[324],y
    voc_short CONST_MYHUFF_AC1_ADDR[324],y


#endif

    VoC_push16(RA,DEFAULT);

    VoC_jal(vpp_AmjrGenerateJpegEncHuffTable);

    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_lw16i_short(WRAP1,0,DEFAULT);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,DEFAULT);
    VoC_sw16_d(REG2,CFG_DMA_WRAP);
    VoC_lw16i(STATUS,STATUS_CLR);
    VoC_lw16i_short(BITCACHE,0,DEFAULT);

    //////////select quality table//////////////////////
    VoC_be16_rd(LABLE_RESET_QUALITY_1,REG6,GLOBAL_VIDEO_QUALITY);
    VoC_lw16i(REG6,CONST_MYQUN_TAB0_ADDR); //LOW
    VoC_jump(LABLE_RESET_QUALITY_END);
LABLE_RESET_QUALITY_1:
    VoC_lw16i(REG6,CONST_MYQUN_TAB1_ADDR);//HIGH
LABLE_RESET_QUALITY_END:

    VoC_lw16i_short(ACC1_HI,0,DEFAULT);//VoC_lw32z(ACC0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_lw16i(ACC1_LO,607);//length of head
    VoC_sw16_d(REG6,GLOBAL_Q_ADDR);//q_table
    VoC_sw32_d(ACC1,GLOBAL_VIDEO_OUT_LEN);//encode length and mode
    ////////const prepare/////////////////////////////////
    VoC_sw16_d(ACC0_LO,GLOBAL_OLD_DCY_ADDR);
    VoC_sw32_d(ACC0,GLOBAL_OLD_DCU_ADDR);
    VoC_lw16i_short(ACC0_LO,8,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32_d(ACC0,GLOBAL_SHORT_1_ADDR);
    exit_on_warnings = ON;
    VoC_sw32_d(ACC0,GLOBAL_CURRBITS_ADDR);
    /////////////////////////////////////////////////////////
    VoC_lw16i(REG4,0x500);
    VoC_lw16i(REG5,0x400);
    VoC_bne32_rd(LABLE_NONE_1280BY1024,REG45,GLOBAL_VIDEO_WIDTH);
    VoC_bez16_d(LABLE_JPEG_RGB,GLOBAL_VIDEO_IN_FORMAT);
    VoC_jal(COOLSAND_1280BY1024_YUV422_JPEG);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return
LABLE_JPEG_RGB:
    VoC_jal(COOLSAND_1280BY1024_JPEG);//VoC_jal(COOLSAND_1280BY1024_YUV422_JPEG);//
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return
LABLE_NONE_1280BY1024:
    VoC_lw32_d(REG45,GLOBAL_VIDEO_WIDTH);
    VoC_lw16i(REG6,350);//411
    VoC_lw16i(REG7,700);//422 444
    VoC_blt16_rd(LABLE_JPEG_4222,REG6,GLOBAL_VIDEO_WIDTH);//if w<=560  411
    VoC_bez16_d(LABLE_JPEG_RGB_411,GLOBAL_VIDEO_IN_FORMAT);
    VoC_jal(COOLSAND_YUV2JPEG411);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
LABLE_JPEG_RGB_411:
    VoC_jal(COOLSAND_RGB2JPEG411);//VoC_jal(COOLSAND_YUV2JPEG411);//
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
LABLE_JPEG_4222:
    VoC_blt16_rd(LABLE_WRONG_SIZE,REG7,GLOBAL_VIDEO_WIDTH);//if w<=1120 422
    VoC_bez16_d(LABLE_JPEG_RGB_422,GLOBAL_VIDEO_IN_FORMAT);
    VoC_jal(COOLSAND_YUVJPEG422);
    VoC_jump(LABLE_WRONG_SIZE);
LABLE_JPEG_RGB_422:
    VoC_jal(COOLSAND_RGB2JPEG422);//VoC_jal(COOLSAND_YUVJPEG422);//
LABLE_WRONG_SIZE:
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

void  COOLSAND_RGB2JPEG411(void)
{

    VoC_lw32_d(REG45,GLOBAL_VIDEO_WIDTH);//w h
    VoC_push16(RA,DEFAULT);
    VoC_movreg32(REG67,REG45,IN_PARALLEL);
    VoC_shr16_ri(REG4,-5,DEFAULT);//w*32 byte
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_and16_ri(REG6,15);
    VoC_and16_ri(REG7,15);
    VoC_sw16_d(REG5,GLOBAL_HEIGHT_SAVE_ADDR);//
    VoC_sw32_d(REG45,CONST_BLOCKSIZE_BYTE_ADDR);
    VoC_lw32_d(REG45,GLOBAL_VIDEO_WIDTH);//w h
    VoC_bez16_r(LABLE_WIDTH_MULTIPLE16,REG6);
    VoC_lw16i_short(REG6,15,DEFAULT);
    VoC_lw16i_short(REG0,1,IN_PARALLEL);

    VoC_sw16_d(REG4,GLOBAL_WIDTH_SAVE_ADDR);//original width;
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_and16_ri(REG4,0xfff0);

    VoC_sw16_d(REG0,GLOBAL_HEIGHT_SAVE_ADDR);//whether the width has changed;
    VoC_sw16_d(REG4,GLOBAL_VIDEO_WIDTH);
LABLE_WIDTH_MULTIPLE16:
    VoC_bez16_r(LABLE_WIDTH_HEIGHT_MULTIPLE16,REG7);
    VoC_add16_rd(REG5,REG5,CONST_15_ADDR);
    VoC_and16_ri(REG5,0xfff0);
    VoC_NOP();
    VoC_sw16_d(REG5,GLOBAL_VIDEO_HEIGHT);
LABLE_WIDTH_HEIGHT_MULTIPLE16:
    VoC_add16_rd(REG6,REG4,CONST_8_ADDR);//W+8
    VoC_sub16_rd(REG7,REG4,CONST_8_ADDR);//W-8
    VoC_shr16_ri(REG4,-3,DEFAULT);//W*8
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    VoC_add16_rd(REG5,REG4,CONST_8_ADDR);//W*8+8

    VoC_sw32_d(REG67,CONST_WIDTH_ADD8);
    VoC_sw32_d(REG45,CONST_WIDTH_MPY8);
    ///////////////////////////////////////////////////
LABLE_REG2JPEG_LOOP_HEIGHT_411:
    VoC_lw16i_short(REG0,-4,DEFAULT);//16=2**4
    VoC_jal(DEMAI_READ_RGBDATA);
    VoC_lw16i_set_inc(REG0,VPP_AMJR_JPEG_RGB_STRUCT,1);
    VoC_lw16i_short(REG5,0,DEFAULT);
LABLE_REG2JPEG_LOOP_WIDTH_411:
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);
    VoC_push32(REG01,DEFAULT);//r0: rgb addr   r1:height loop
    VoC_push16(REG5,IN_PARALLEL);//width loop
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_jal(MCU_QUA_RGB2YUV411);


    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_8_ADDR);
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR+4,1);
    VoC_jal(MCU_QUA_RGB2YUV411);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_WIDTH_MPY8);
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y2_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR+32,1);
    VoC_jal(MCU_QUA_RGB2YUV411);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_WIDTH_MPY8_ADD8);
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y3_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR+36,1);
    VoC_jal(MCU_QUA_RGB2YUV411);

    //ENOCDE BLOCK
    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y2_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y3_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);


    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);

    ////////////////////////////////
    VoC_pop32(REG01,DEFAULT);
    VoC_pop16(REG5,IN_PARALLEL);
    VoC_lw16i_short(REG4,16,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);
    /////////////////////////////////
    VoC_bnlt16_rd(LABLE_CON1_411,REG5,GLOBAL_VIDEO_WIDTH);
    VoC_jump(LABLE_REG2JPEG_LOOP_WIDTH_411);
LABLE_CON1_411:
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//height loop
    VoC_bnlt16_rd(LABLE_CON2_411,REG1,GLOBAL_VIDEO_HEIGHT);
    VoC_jump(LABLE_REG2JPEG_LOOP_HEIGHT_411);
LABLE_CON2_411:
    ////////////////////////////////////////////////////////////////////
    VoC_jal(COOLSNAD_flush_bits);
    //whether there is  odd byte left
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}



void  MCU_QUA_RGB2YUV411(void)
{
    //push 16
    //martic RGB BUF   0

    //INPUT:
    //r0:rgb buf  r1:CONST BUF(INT RAM_X) r2:y   r3:u v

    //USED BUF:
    //R0 R1 R2 R3 R4 R5 R6 R7 ACC0 ACC1

    //RETURN : NONE



    VoC_push16(REG1,DEFAULT);
    VoC_loop_i(1,4);

    VoC_loop_i(0,4);


    //1st point
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_movreg16(REG6,REG4,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);
    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_lw16_sd(REG1,0,IN_PARALLEL);
    VoC_shru16_ri(REG5,-5,DEFAULT);
    VoC_shru16_ri(REG4,-3,IN_PARALLEL);//r
    VoC_shru16_ri(REG5,10,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);



    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru16_ri(REG5,-2,IN_PARALLEL);//g
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG6,DEFAULT);//save b to acc1
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_multf32inc_rp(ACC0,REG4,REG1,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_movreg16(REG7,ACC0_LO,IN_PARALLEL);
    exit_on_warnings = ON;
    VoC_sub16_rd(REG7,REG7,CONST_0X80_ADDR);

    //u = (((r * -43) + (g * -85) + (b << 7)) >> 8);
    VoC_shru32_ri(ACC1,-7,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);//u
    VoC_sw16inc_p(REG7,REG2,DEFAULT);//write into Y_BUF1[0]
    VoC_shru32_ri(ACC0,8,IN_PARALLEL);


    //V = (((r << 7) + (g * -107) + (b * -21)) >> 8)
    VoC_multf32inc_rp(ACC0,REG5,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_mac32_rp(REG6,REG1,DEFAULT);
    VoC_sw16_p(ACC0_LO,REG3,IN_PARALLEL);//write into U_BUF
    exit_on_warnings = ON;
    VoC_shru32_ri(ACC1,-7,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_add32_rr(ACC1,ACC0,ACC1,DEFAULT);
    VoC_lw16_sd(REG1,0,IN_PARALLEL);
    VoC_shru32_ri(ACC1,8,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);

    //the 2nd point
    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_movreg16(REG6,REG4,IN_PARALLEL);
    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);
    VoC_shru16_ri(REG5,10,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);



    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru16_ri(REG5,-2,IN_PARALLEL);//g
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_lw16i(REG7,64);
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG3,IN_PARALLEL);
    VoC_movreg16(REG7,ACC0_LO,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16_p(ACC1_LO,REG7,DEFAULT);//write to V_BUF
    exit_on_warnings = ON;
    VoC_sub16_rd(REG7,REG7,CONST_0X80_ADDR);
    VoC_inc_p(REG3,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,DEFAULT);//write to Y_BUF

    VoC_endloop0

    //////////////////////////////////////////
    VoC_add16_rd(REG0,REG0,CONST_WIDTH_SUB8);//换行  奇数行
    ////////////////////////////////////////////
    VoC_loop_i(0,4);

    //1st point
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_movreg16(REG6,REG4,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);
    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_lw16_sd(REG1,0,IN_PARALLEL);
    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);
    VoC_shru16_ri(REG5,10,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);



    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru16_ri(REG5,-2,IN_PARALLEL);//g
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);
    VoC_movreg16(REG7,ACC0_LO,DEFAULT);
    VoC_movreg16(REG6,REG4,IN_PARALLEL);
    VoC_sub16_rd(REG7,REG7,CONST_0X80_ADDR);



    //the 2nd point
    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);
    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_lw16_sd(REG1,0,IN_PARALLEL);//REWIN R1
    VoC_shru16_ri(REG5,10,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);



    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru16_ri(REG5,-2,IN_PARALLEL);//g
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,DEFAULT);//write to  Y_BUF
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_movreg16(REG7,ACC0_LO,DEFAULT);
    VoC_sub16_rd(REG7,REG7,CONST_0X80_ADDR);
    VoC_lw16i_short(REG4,4,DEFAULT);//FOR U/V INC
    VoC_sw16inc_p(REG7,REG2,DEFAULT);//write to Y_BUF

    VoC_endloop0
    VoC_add16_rd(REG0,REG0,CONST_WIDTH_SUB8);//换行  偶数行
    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);

    VoC_endloop1

    VoC_pop16(REG1,DEFAULT);//MATRIC

    VoC_return;



}

////////////////////////////////////////////////
void  COOLSAND_RGB2JPEG422()
{
    VoC_lw32_d(REG45,GLOBAL_VIDEO_WIDTH);//w h
    VoC_push16(RA,DEFAULT);
    VoC_movreg32(REG67,REG45,IN_PARALLEL);
    VoC_shr16_ri(REG4,-4,DEFAULT);//w*32 byte
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_and16_ri(REG6,15);
    VoC_and16_ri(REG7,7);
    VoC_sw16_d(REG5,GLOBAL_HEIGHT_SAVE_ADDR);//
    VoC_sw32_d(REG45,CONST_BLOCKSIZE_BYTE_ADDR);
    VoC_lw32_d(REG45,GLOBAL_VIDEO_WIDTH);//w h
    VoC_bez16_r(LABLE_WIDTH_MULTIPLE16_422,REG6);
    VoC_lw16i_short(REG6,15,DEFAULT);
    VoC_lw16i_short(REG0,1,IN_PARALLEL);

    VoC_sw16_d(REG4,GLOBAL_WIDTH_SAVE_ADDR);//original width;
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_and16_ri(REG4,0xfff0);

    VoC_sw16_d(REG0,GLOBAL_HEIGHT_SAVE_ADDR);//whether the width has changed;
    VoC_sw16_d(REG4,GLOBAL_VIDEO_WIDTH);
LABLE_WIDTH_MULTIPLE16_422:
    VoC_bez16_r(LABLE_WIDTH_HEIGHT_MULTIPLE16_422,REG7);
    VoC_add16_rd(REG5,REG5,CONST_7_ADDR);
    VoC_and16_ri(REG5,0xfff8);
    VoC_NOP();
    VoC_sw16_d(REG5,GLOBAL_VIDEO_HEIGHT);
LABLE_WIDTH_HEIGHT_MULTIPLE16_422:
    VoC_add16_rd(REG6,REG4,CONST_8_ADDR);//W+8
    VoC_sub16_rd(REG7,REG4,CONST_8_ADDR);//W-8
    VoC_shr16_ri(REG4,-3,DEFAULT);//W*8
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    //VoC_add16_rd(REG5,REG4,CONST_8_ADDR);//W*8+8

    VoC_sw32_d(REG67,CONST_WIDTH_ADD8);
    VoC_shr16_ri(REG4,-1,DEFAULT);//VoC_shr16_ri(REG4,-2,DEFAULT);
    VoC_sw32_d(REG45,CONST_WIDTH_MPY8);
    ///////////////////////////////////////////////////
LABLE_REG2JPEG_LOOP_HEIGHT_422:
    VoC_lw16i_short(REG0,-3,DEFAULT);//8=2**3
    VoC_jal(DEMAI_READ_RGBDATA);
    VoC_lw16i_set_inc(REG0,VPP_AMJR_JPEG_RGB_STRUCT,1);
    VoC_lw16i_short(REG5,0,DEFAULT);
LABLE_REG2JPEG_LOOP_WIDTH_422:
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);
    VoC_push32(REG01,DEFAULT);//r0: rgb addr   r1:height loop
    VoC_push16(REG5,IN_PARALLEL);//width loop
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_jal(MCU_QUA_RGB2YUV422);


    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_8_ADDR);
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR+4,1);
    VoC_jal(MCU_QUA_RGB2YUV422);



    //ENOCDE BLOCK
    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);


    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);

    ////////////////////////////////
    VoC_pop32(REG01,DEFAULT);
    VoC_pop16(REG5,IN_PARALLEL);
    VoC_lw16i_short(REG4,16,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);
    /////////////////////////////////
    VoC_bnlt16_rd(LABLE_CON1_422,REG5,GLOBAL_VIDEO_WIDTH);
    VoC_jump(LABLE_REG2JPEG_LOOP_WIDTH_422);
LABLE_CON1_422:
    VoC_lw16i_short(REG4,8,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//height loop
    VoC_bnlt16_rd(LABLE_CON2_422,REG1,GLOBAL_VIDEO_HEIGHT);
    VoC_jump(LABLE_REG2JPEG_LOOP_HEIGHT_422);
LABLE_CON2_422:
    ////////////////////////////////////////////////////////////////////
    VoC_jal(COOLSNAD_flush_bits);
    //whether there is  odd byte left
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}



void  MCU_QUA_RGB2YUV422(void)
{
    //push 16
    //martic RGB BUF   0

    //INPUT:
    //r0:rgb buf  r1:CONST BUF(INT RAM_X) r2:y   r3:u v

    //USED BUF:
    //R0 R1 R2 R3 R4 R5 R6 R7 ACC0 ACC1

    //RETURN : NONE



    VoC_push16(REG1,DEFAULT);
    VoC_loop_i(1,8);

    VoC_loop_i(0,4);


    //1st point
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_movreg16(REG6,REG4,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);
    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_lw16_sd(REG1,0,IN_PARALLEL);
    VoC_shru16_ri(REG5,-5,DEFAULT);
    VoC_shru16_ri(REG4,-3,IN_PARALLEL);//r
    VoC_shru16_ri(REG5,10,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);



    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru16_ri(REG5,-2,IN_PARALLEL);//g
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG6,DEFAULT);//save b to acc1
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_multf32inc_rp(ACC0,REG4,REG1,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_movreg16(REG7,ACC0_LO,IN_PARALLEL);
    exit_on_warnings = ON;
    VoC_sub16_rd(REG7,REG7,CONST_0X80_ADDR);

    //u = (((r * -43) + (g * -85) + (b << 7)) >> 8);
    VoC_shru32_ri(ACC1,-7,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);//u
    VoC_sw16inc_p(REG7,REG2,DEFAULT);//write into Y_BUF1[0]
    VoC_shru32_ri(ACC0,8,IN_PARALLEL);


    //V = (((r << 7) + (g * -107) + (b * -21)) >> 8)
    VoC_multf32inc_rp(ACC0,REG5,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_mac32_rp(REG6,REG1,DEFAULT);
    VoC_sw16_p(ACC0_LO,REG3,IN_PARALLEL);//write into U_BUF
    exit_on_warnings = ON;
    VoC_shru32_ri(ACC1,-7,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_add32_rr(ACC1,ACC0,ACC1,DEFAULT);
    VoC_lw16_sd(REG1,0,IN_PARALLEL);
    VoC_shru32_ri(ACC1,8,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);

    //the 2nd point
    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_movreg16(REG6,REG4,IN_PARALLEL);
    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);
    VoC_shru16_ri(REG5,10,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);



    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru16_ri(REG5,-2,IN_PARALLEL);//g
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_lw16i(REG7,64);
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG3,IN_PARALLEL);
    VoC_movreg16(REG7,ACC0_LO,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16_p(ACC1_LO,REG7,DEFAULT);//write to V_BUF
    exit_on_warnings = ON;
    VoC_sub16_rd(REG7,REG7,CONST_0X80_ADDR);
    VoC_inc_p(REG3,DEFAULT);
    VoC_lw16i_short(REG4,4,IN_PARALLEL);
    VoC_sw16inc_p(REG7,REG2,DEFAULT);//write to Y_BUF

    VoC_endloop0
    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_WIDTH_SUB8);//换行  奇数行

    VoC_endloop1

    VoC_pop16(REG1,DEFAULT);//MATRIC

    VoC_return;



}

////////////////////////////////////////////////
//void  COOLSAND_RGB2JPEG444()
//{
//  VoC_push16(RA,DEFAULT);
//  VoC_lw16_d(REG4,GLOBAL_VIDEO_WIDTH);
//  VoC_add16_rd(REG6,REG4,CONST_8_ADDR);//W+8
//  VoC_sub16_rd(REG7,REG4,CONST_8_ADDR);//W-8
//  VoC_shr16_ri(REG4,-3,DEFAULT);//W*8
//  VoC_lw16i_short(REG1,0,IN_PARALLEL);
//  //VoC_add16_rd(REG5,REG4,CONST_8_ADDR);//W*8+8
//
//  VoC_sw32_d(REG67,CONST_WIDTH_ADD8);
//  VoC_shr16_ri(REG4,-1,DEFAULT);//VoC_shr16_ri(REG4,-2,DEFAULT);
//  VoC_lw16i_short(REG5,0,IN_PARALLEL);
//  exit_on_warnings = OFF;
//  VoC_sw32_d(REG45,CONST_WIDTH_MPY8);
//  exit_on_warnings = ON;
//  VoC_sw32_d(REG45,CONST_BLOCKSIZE_BYTE_ADDR);
//
//   ///////////////////////////////////////////////////
//LABLE_REG2JPEG_LOOP_HEIGHT_444:
//  VoC_jal(DEMAI_READ_RGBDATA);
//  VoC_lw16i_set_inc(REG0,VPP_AMJR_JPEG_RGB_STRUCT,1);
//  VoC_lw16i_short(REG5,0,DEFAULT);
//LABLE_REG2JPEG_LOOP_WIDTH_444:
//    VoC_lw16i_short(INC0,1,DEFAULT);
//  VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
//
//    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y_ADDR,1);
//  VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);
//  VoC_push32(REG01,DEFAULT);//r0: rgb addr   r1:height loop
//  VoC_push16(REG5,IN_PARALLEL);//width loop
//    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
//  VoC_jal(MCU_QUA_RGB2YUV444);
//
//
//  //ENOCDE BLOCK
//  VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
//  VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
//  VoC_lw16i_short(REG4,0,DEFAULT);
//  VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);
//
//  VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
//  VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
//  VoC_lw16i_short(REG4,1,DEFAULT);
//  VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);
//
//  VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
//  VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
//  VoC_lw16i_short(REG4,1,DEFAULT);
//  VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);
//
//  ////////////////////////////////
//  VoC_pop32(REG01,DEFAULT);
//    VoC_pop16(REG5,IN_PARALLEL);
//  VoC_lw16i_short(REG4,8,DEFAULT);
//  VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
//    VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);
//  /////////////////////////////////
//  VoC_bnlt16_rd(LABLE_CON1_444,REG5,GLOBAL_VIDEO_WIDTH);
//  VoC_jump(LABLE_REG2JPEG_LOOP_WIDTH_444);
//LABLE_CON1_444:
//  VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//height loop
//    VoC_bnlt16_rd(LABLE_CON2_444,REG1,GLOBAL_VIDEO_HEIGHT);
//  VoC_jump(LABLE_REG2JPEG_LOOP_HEIGHT_444);
//LABLE_CON2_444:
//  ////////////////////////////////////////////////////////////////////
//  VoC_jal(COOLSNAD_flush_bits);
//  //whether there is  odd byte left
//    VoC_pop16(RA,DEFAULT);
//  VoC_NOP();
//
//  VoC_return;
//}
//
//
//
//void  MCU_QUA_RGB2YUV444(void)
//{
//  //push 16
//  //martic RGB BUF   0
//
//  //INPUT:
//    //r0:rgb buf  r1:CONST BUF(INT RAM_X) r2:y   r3:u v
//
//  //USED BUF:
//  //R0 R1 R2 R3 R4 R5 R6 R7 ACC0 ACC1
//
//  //RETURN : NONE
//
//
//
//  VoC_push16(REG1,DEFAULT);
//  VoC_loop_i(1,8);
//
//  VoC_loop_i(0,8);
//
//
//      //1st point
//     VoC_lw16inc_p(REG4,REG0,DEFAULT);
//     VoC_lw32z(ACC1,IN_PARALLEL);
//     VoC_movreg16(REG6,REG4,DEFAULT);
//     VoC_movreg16(REG5,REG4,IN_PARALLEL);
//     VoC_shru16_ri(REG4,11,DEFAULT);
//     VoC_lw16_sd(REG1,0,IN_PARALLEL);
//     VoC_shru16_ri(REG5,-5,DEFAULT);
//     VoC_shru16_ri(REG4,-3,IN_PARALLEL);//r
//     VoC_shru16_ri(REG5,10,DEFAULT);
//     VoC_shru16_ri(REG6,-11,IN_PARALLEL);
//
//
//
//     //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
//     VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
//     VoC_shru16_ri(REG5,-2,IN_PARALLEL);//g
//     VoC_mac32inc_rp(REG5,REG1,DEFAULT);
//     VoC_shru16_ri(REG6,8,IN_PARALLEL);//b
//     VoC_mac32inc_rp(REG6,REG1,DEFAULT);
//     VoC_movreg16(ACC1_LO,REG6,DEFAULT);//save b to acc1
//     VoC_shru32_ri(ACC0,8,DEFAULT);
//     VoC_multf32inc_rp(ACC0,REG4,REG1,IN_PARALLEL);
//       exit_on_warnings = OFF;
//     VoC_mac32inc_rp(REG5,REG1,DEFAULT);
//     VoC_movreg16(REG7,ACC0_LO,IN_PARALLEL);
//       exit_on_warnings = ON;
//     VoC_sub16_rd(REG7,REG7,CONST_0X80_ADDR);
//
//     //u = (((r * -43) + (g * -85) + (b << 7)) >> 8);
//     VoC_shru32_ri(ACC1,-7,DEFAULT);
//     VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
//     VoC_lw32z(ACC1,IN_PARALLEL);//u
//     VoC_sw16inc_p(REG7,REG2,DEFAULT);//write into Y_BUF1[0]
//     VoC_shru32_ri(ACC0,8,IN_PARALLEL);
//
//
//     //V = (((r << 7) + (g * -107) + (b * -21)) >> 8)
//     VoC_multf32inc_rp(ACC0,REG5,REG1,DEFAULT);
//     VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);
//     exit_on_warnings = OFF;
//     VoC_mac32_rp(REG6,REG1,DEFAULT);
//     VoC_sw16_p(ACC0_LO,REG3,IN_PARALLEL);//write into U_BUF
//     exit_on_warnings = ON;
//     VoC_shru32_ri(ACC1,-7,DEFAULT);
//       VoC_lw16i_short(REG7,64,IN_PARALLEL);
//     VoC_add32_rr(ACC1,ACC0,ACC1,DEFAULT);
//     VoC_lw16_sd(REG1,0,IN_PARALLEL);
//     VoC_shru32_ri(ACC1,8,DEFAULT);
//       VoC_add16_rr(REG7,REG7,REG3,IN_PARALLEL);
//     VoC_inc_p(REG3,DEFAULT);
//     VoC_sw16_p(ACC1_LO,REG7,DEFAULT);//write to V_BUF
//
//     VoC_endloop0
//
//     VoC_add16_rd(REG0,REG0,CONST_WIDTH_SUB8);//换行
//
//     VoC_endloop1
//
//     VoC_pop16(REG1,DEFAULT);//MATRIC
//
//     VoC_return;
//
//}
//process 1280*1024 size image

void COOLSAND_Double2Single(void)
{
    //r1:pin    r0:pout
    //used buf: r0 r1 r2 r4 r5 r6 r7 rl6

    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);

    VoC_movreg16(REG5,REG4,DEFAULT);
    VoC_shru16_ri(REG4,11,IN_PARALLEL);

    VoC_movreg16(REG6,REG5,DEFAULT);
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);

    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_shru16_ri(REG5,10,IN_PARALLEL);

    VoC_shru16_ri(REG5,-2,DEFAULT);//g
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b

    VoC_loop_i_short(8,DEFAULT);
    VoC_startloop0

    VoC_push16(REG0,DEFAULT);//caution???
    VoC_sw16inc_p(REG4,REG0,IN_PARALLEL);//save r

    VoC_lw16inc_p(REG4,REG1,DEFAULT);//r1g1b1

    VoC_sw16inc_p(REG5,REG0,DEFAULT);//save g
    VoC_movreg16(REG5,REG4,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG0,DEFAULT);//save b
    VoC_movreg16(REG6,REG4,IN_PARALLEL);

    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);

    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_shru16_ri(REG5,10,IN_PARALLEL);

    VoC_pop16(REG2,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);

    VoC_shru16_ri(REG5,-2,DEFAULT);//g
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b

    VoC_add16inc_rp(REG4,REG4,REG2,DEFAULT);
    VoC_movreg32(RL6,REG45,IN_PARALLEL);

    VoC_add16inc_rp(REG5,REG5,REG2,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_add16inc_rp(REG7,REG6,REG2,DEFAULT);
    VoC_shru16_ri(REG5,1,IN_PARALLEL)

    VoC_shru16_ri(REG7,1,DEFAULT);
    VoC_sw16inc_p(REG4,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    VoC_movreg32(REG45,RL6,IN_PARALLEL);

    VoC_sw16inc_p(REG7,REG0,DEFAULT);

    VoC_endloop0

    VoC_return;

}

void COOLSAND_Double2SingleLastRow(void)
{
    //r1:pin    r0:pout
    //used buf: r0 r1 r2 r4 r5 r6 r7 rl6

    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);

    VoC_movreg16(REG6,REG4,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);

    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);

    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_shru16_ri(REG5,10,IN_PARALLEL);

    VoC_shru16_ri(REG5,-2,DEFAULT);//g
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b

    VoC_loop_i_short(7,DEFAULT);
    VoC_startloop0

    VoC_push16(REG0,DEFAULT);//caution???
    VoC_sw16inc_p(REG4,REG0,IN_PARALLEL);//save r

    VoC_lw16inc_p(REG4,REG1,DEFAULT);//r1g1b1

    VoC_sw16inc_p(REG5,REG0,DEFAULT);//save g
    VoC_movreg16(REG5,REG4,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG0,DEFAULT);//save b
    VoC_movreg16(REG6,REG4,IN_PARALLEL);

    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);

    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_shru16_ri(REG5,10,IN_PARALLEL);

    VoC_pop16(REG2,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);

    VoC_shru16_ri(REG5,-2,DEFAULT);//g
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b

    VoC_add16inc_rp(REG4,REG4,REG2,DEFAULT);
    VoC_movreg32(RL6,REG45,IN_PARALLEL);

    VoC_add16inc_rp(REG5,REG5,REG2,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_add16inc_rp(REG7,REG6,REG2,DEFAULT);
    VoC_shru16_ri(REG5,1,IN_PARALLEL)

    VoC_shru16_ri(REG7,1,DEFAULT);
    VoC_sw16inc_p(REG4,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    VoC_movreg32(REG45,RL6,IN_PARALLEL);

    VoC_sw16inc_p(REG7,REG0,DEFAULT);

    VoC_endloop0


    VoC_sw16inc_p(REG4,REG0,DEFAULT);//save r
    VoC_sw16inc_p(REG5,REG0,DEFAULT);//save g
    VoC_sw16inc_p(REG6,REG0,DEFAULT);//save b

    VoC_sw16inc_p(REG4,REG0,DEFAULT);//save r
    VoC_sw16inc_p(REG5,REG0,DEFAULT);//save g
    VoC_sw16inc_p(REG6,REG0,DEFAULT);//save b

    VoC_return;
}
void COOLSAND_Double2Row(void)
{
    //in1:r2 INC2=2 ;  in2:r3 INC3=1 ; out:r0  INC0=2;
    VoC_loop_i_short(8,DEFAULT)
    VoC_startloop0

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_lw32inc_p(REG67,REG2,DEFAULT);

    VoC_add16inc_rp(REG4,REG4,REG3,DEFAULT);
    VoC_add16inc_rp(REG5,REG5,REG3,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);//r
    VoC_add16inc_rp(REG6,REG6,REG3,DEFAULT);
    VoC_shru16_ri(REG5,1,IN_PARALLEL);//g

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(REG45,REG0,DEFAULT);//save rg
    VoC_shru16_ri(REG6,1,IN_PARALLEL);//b
    exit_on_warnings=ON;

    VoC_add16inc_rp(REG7,REG7,REG3,DEFAULT);//r
    VoC_add16inc_rp(REG4,REG4,REG3,DEFAULT);//g
    VoC_shru16_ri(REG7,1,IN_PARALLEL);
    VoC_add16inc_rp(REG5,REG5,REG3,DEFAULT);//b
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_shru16_ri(REG5,1,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);//save br
    VoC_sw32inc_p(REG45,REG0,DEFAULT);//save gb

    VoC_endloop0

    VoC_return;
}

void COOLSAN_Encode422MCUBlock(void)
{
    //r0:rgb buf(Y)  r1:CONST BUF(INT RAM_Y)   r2:y   r3:u v(X)
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);


    VoC_loop_i(1,8);
    VoC_loop_i(0,4);

    VoC_lw16inc_p(REG4,REG0,DEFAULT);//R
    VoC_push16(REG1,IN_PARALLEL);//X

    VoC_lw16inc_p(REG5,REG0,DEFAULT);//G
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_lw16inc_p(REG6,REG0,DEFAULT);//B
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG6,IN_PARALLEL);//save b to acc1
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_shru32_ri(ACC1,-7,DEFAULT);//b << 7
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_sub32_rd(ACC0,ACC0,CONST_0X80_ADDR);
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);//(r * -43)
    exit_on_warnings=OFF;
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);//(g * -85)
    VoC_sw16inc_p(ACC0_LO,REG2,IN_PARALLEL);//write into Y_BUF1[0]
    exit_on_warnings=ON;
    VoC_lw16i(REG7,64);
    //u = (((r * -43) + (g * -85) + (b << 7)) >> 8);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_shru32_ri(ACC0,8,DEFAULT);
    //V = (((r << 7) + (g * -107) + (b * -21)) >> 8)
    VoC_multf32inc_rp(ACC0,REG5,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_mac32_rp(REG6,REG1,DEFAULT);
    VoC_sw16_p(ACC0_LO,REG3,IN_PARALLEL);//write into U_BUF
    exit_on_warnings = ON;
    VoC_lw16inc_p(REG4,REG0,DEFAULT);//r
    VoC_shru32_ri(ACC1,-7,IN_PARALLEL);

    VoC_lw16inc_p(REG5,REG0,DEFAULT);//g
    VoC_lw16_sd(REG1,0,IN_PARALLEL);

    VoC_lw16inc_p(REG6,REG0,DEFAULT);//b
    VoC_add32_rr(ACC1,ACC0,ACC1,IN_PARALLEL);//v
    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru32_ri(ACC1,8,IN_PARALLEL);
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG3,IN_PARALLEL);

    VoC_mac32inc_rp(REG6,REG1,DEFAULT);

    VoC_inc_p(REG3,DEFAULT);//op
    VoC_sw16_p(ACC1_LO,REG7,IN_PARALLEL);//write to V_BUF

    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_sub32_rd(ACC0,ACC0,CONST_0X80_ADDR);

    VoC_pop16(REG1,DEFAULT);
    VoC_lw16i_short(REG4,4,IN_PARALLEL);

    VoC_sw16inc_p(ACC0_LO,REG2,DEFAULT);//write to Y_BUF
    VoC_endloop0
    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_24_ADDR);//换行
    VoC_endloop1

    //////////////////////////////////////////////////////////
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR+24,1);
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR+4,1);

    VoC_loop_i(1,8);
    VoC_loop_i(0,4);

    VoC_lw16inc_p(REG4,REG0,DEFAULT);//R
    VoC_push16(REG1,IN_PARALLEL);//X

    VoC_lw16inc_p(REG5,REG0,DEFAULT);//G
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_lw16inc_p(REG6,REG0,DEFAULT);//B
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG6,IN_PARALLEL);//save b to acc1
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_shru32_ri(ACC1,-7,DEFAULT);//b << 7
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_sub32_rd(ACC0,ACC0,CONST_0X80_ADDR);
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);//(r * -43)
    exit_on_warnings=OFF;
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);//(g * -85)
    VoC_sw16inc_p(ACC0_LO,REG2,IN_PARALLEL);//write into Y_BUF1[0]
    exit_on_warnings=ON;
    VoC_lw16i(REG7,64);
    //u = (((r * -43) + (g * -85) + (b << 7)) >> 8);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_shru32_ri(ACC0,8,DEFAULT);
    //V = (((r << 7) + (g * -107) + (b * -21)) >> 8)
    VoC_multf32inc_rp(ACC0,REG5,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_mac32_rp(REG6,REG1,DEFAULT);
    VoC_sw16_p(ACC0_LO,REG3,IN_PARALLEL);//write into U_BUF
    exit_on_warnings = ON;
    VoC_lw16inc_p(REG4,REG0,DEFAULT);//r
    VoC_shru32_ri(ACC1,-7,IN_PARALLEL);

    VoC_lw16inc_p(REG5,REG0,DEFAULT);//g
    VoC_lw16_sd(REG1,0,IN_PARALLEL);

    VoC_lw16inc_p(REG6,REG0,DEFAULT);//b
    VoC_add32_rr(ACC1,ACC0,ACC1,IN_PARALLEL);//v
    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru32_ri(ACC1,8,IN_PARALLEL);
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG3,IN_PARALLEL);

    VoC_mac32inc_rp(REG6,REG1,DEFAULT);

    VoC_inc_p(REG3,DEFAULT);//op
    VoC_sw16_p(ACC1_LO,REG7,IN_PARALLEL);//write to V_BUF

    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_sub32_rd(ACC0,ACC0,CONST_0X80_ADDR);

    VoC_pop16(REG1,DEFAULT);
    VoC_lw16i_short(REG4,4,IN_PARALLEL);

    VoC_sw16inc_p(ACC0_LO,REG2,DEFAULT);//write to Y_BUF
    VoC_endloop0
    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_24_ADDR);//换行
    VoC_endloop1
    ///////////////////////////////////////////////////////
    //ENOCDE BLOCK
    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);


    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);
    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

    VoC_return;


}
void COOLSAND_MEMCopy(void)
{
    //r2:in   r3:out
    //used  buf: r2 r3 RL6 rl7
    VoC_loop_i_short(12,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(RL6,REG2,DEFAULT);
    VoC_lw32inc_p(RL7,REG2,DEFAULT);
    VoC_sw32inc_p(RL6,REG3,DEFAULT);
    VoC_sw32inc_p(RL7,REG3,DEFAULT);
    VoC_endloop0

    VoC_return;

}


void vpp_AmjrJpegRead(void)
{
    // read data
    VoC_sw32_d(REG67,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    // do pixel swap if requested
    VoC_bez16_d(no_pixel_swap_needed,GLOBAL_VIDEO_PXL_SWAP);

    VoC_push32(REG01,DEFAULT);
    VoC_shr16_ri(REG7,-1,IN_PARALLEL);

    VoC_movreg16(REG0,REG7,DEFAULT);
    VoC_movreg16(REG1,REG7,IN_PARALLEL);

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_lw16i_short(INC1,2,DEFAULT);

    VoC_lw16inc_p(ACC0_HI,REG0,DEFAULT);
    VoC_lw16inc_p(ACC0_LO,REG0,IN_PARALLEL);

pixel_swap_loop:

    VoC_lw16inc_p(ACC0_HI,REG0,DEFAULT);
    VoC_lw16inc_p(ACC0_LO,REG0,IN_PARALLEL);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG7,IN_PARALLEL);

    VoC_bne16_rr(pixel_swap_loop,REG6,REG7);

    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_pop32(REG01,DEFAULT);

no_pixel_swap_needed:

    VoC_return;

}

void COOLSAND_DMAIRead_1280BY1024Even(void)
{
    //INPUT:R4 LINE1 ,r5 line2
    //USED BUF: R6 R7 ACC0 FORMAT32
    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
    VoC_lw16i(REG7,(VPP_AMJR_JPEG_RGB_STRUCT)/2);//IN addr[INT]
    VoC_multf32_rd(ACC0,REG4,CONST_640_ADDR);//[byte]
    VoC_lw16_d(REG6,CONST_640BY8_INT_ADDR);//size [int]
    VoC_add32_rd(ACC0,ACC0,GLOBAL_IN_VIDEO_BUF_START);//out [BYTE]

    VoC_push16(RA,DEFAULT);
    VoC_jal(vpp_AmjrJpegRead);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}

void COOLSAND_DMAIRead_1280BY1024Odd(void)
{
    //INPUT:R4 LINE1 ,r5 line2
    //USED BUF: R6 R7 ACC0 FORMAT32
    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
    VoC_lw16i(REG7,(VPP_AMJR_JPEG_RGB_STRUCT)/2);//IN addr[INT]
    VoC_multf32_rd(ACC0,REG4,CONST_640_ADDR);//640*LINE1

    VoC_bnlt16_rd(LABLE_BIGTHEN479,REG5,CONST_479_ADDR);
    VoC_lw16_d(REG6,CONST_640BY9_INT_ADDR);//size [int]
    VoC_add32_rd(ACC0,ACC0,GLOBAL_IN_VIDEO_BUF_START);//out [BYTE]

    VoC_push16(RA,DEFAULT);
    VoC_jal(vpp_AmjrJpegRead);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;

LABLE_BIGTHEN479:

    VoC_lw16_d(REG6,CONST_640BY8_INT_ADDR);//size [int]
    VoC_add32_rd(ACC0,ACC0,GLOBAL_IN_VIDEO_BUF_START);//out [BYTE]

    VoC_push16(RA,DEFAULT);
    VoC_jal(vpp_AmjrJpegRead);
    VoC_pop16(RA,DEFAULT);

    ///////////////////////////////////////////////////////////////
    VoC_multf32_rd(ACC0,REG5,CONST_640_ADDR);
    VoC_lw16_d(REG6,CONST_640_INT_ADDR);//size [int]
    VoC_lw16i(REG7,(JPEGENC_BUFF_RGB_640_8_ADDR)/2);//IN Addr[INT]
    VoC_add32_rd(ACC0,ACC0,GLOBAL_IN_VIDEO_BUF_START);//out [BYTE]

    VoC_push16(RA,DEFAULT);
    VoC_jal(vpp_AmjrJpegRead);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}

void COOLSAND_1280BY1024_JPEG()
{
#if 0
    voc_short TEMP_BUFF_FOR1280BY1024_TMP1[48]  ,  y
    voc_short TEMP_BUFF_FOR1280BY1024_TMP2[48]  ,  y
#endif

    VoC_lw16i_short(REG5,7,DEFAULT);//line2
    VoC_lw16i_short(REG4,0,IN_PARALLEL);//line1

    VoC_push16(RA,DEFAULT);
LABLE_START_1280BY1024:
    VoC_push32(REG45,DEFAULT);//line2  line1
    //even-16   first-8
    VoC_lw16i_short(REG0,79,IN_PARALLEL);//count=79;

    VoC_lw16i_set_inc(REG1,VPP_AMJR_JPEG_RGB_STRUCT,1);//pin_addr
    VoC_jal(COOLSAND_DMAIRead_1280BY1024Even);
LABLE_DO_BEGIN_FRONT_EVEN:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);//r0:pout
    VoC_jal(COOLSAND_Double2Single);//0


    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//int
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy);      //1

    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);//3


    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);//5

    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_7_ADDR);
    VoC_jal(COOLSAND_Double2Single);//7

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_3_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_3_ADDR,2);//4
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_5_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_5_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_7_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);


    VoC_jal(COOLSAN_Encode422MCUBlock);


    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW1,REG0);
    VoC_jump(LABLE_DO_BEGIN_FRONT_EVEN);
LABLE_LASTROW1:
    //last row
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);//r2:pout
    VoC_jal(COOLSAND_Double2SingleLastRow);//0


    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//in
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy);              //1

    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);//3


    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);//5

    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_7_ADDR);
    VoC_jal(COOLSAND_Double2SingleLastRow);//7

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_3_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_3_ADDR,2);//4
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_5_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_5_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_7_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);


    VoC_jal(COOLSAN_Encode422MCUBlock);
    ////evev-16  second-8////////////////////
    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_RGB_640_3_ADDR,1);
    VoC_lw16i(REG0,79);//count=79;

LABLE_DO_BEGIN_BEHIND_EVEN:
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    exit_on_warnings = OFF;
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);
    exit_on_warnings = ON;
    //奇数行
    VoC_jal(COOLSAND_Double2Single);


    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_1_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*5,pout+48*3);//11
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*6,pout+48*5);//13
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*7,pout+48*7);//15
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_7_ADDR);
    VoC_jal(COOLSAND_Double2Single);

    //偶数行
    //  double2row(16,bufftmp,pout+48,pout);//8
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_1_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48,pout+48*3,pout+48*2);//10
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_3_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*3,pout+48*5,pout+48*4);//12
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_3_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_5_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*5,pout+48*7,pout+48*6);//14
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_5_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_7_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock);

    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW2,REG0);
    VoC_jump(LABLE_DO_BEGIN_BEHIND_EVEN);
LABLE_LASTROW2:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    //奇数行
    //double2single(8,pin+640*3,bufftmp);//7
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //double2single(8,pin+640*4,pout+48);//9
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_1_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*5,pout+48*3);//11
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*6,pout+48*5);//13
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*7,pout+48*7);//15
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_7_ADDR);
    VoC_jal(COOLSAND_Double2SingleLastRow);
    //偶数行
    //  double2row(16,bufftmp,pout+48,pout);//8
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_1_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48,pout+48*3,pout+48*2);//10
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_3_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*3,pout+48*5,pout+48*4);//12
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_3_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_5_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*5,pout+48*7,pout+48*6);//14
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_5_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_7_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock);

    //ODD-16 FIRST-8
    VoC_lw32_sd(REG45,0,DEFAULT);//LINE2 LINE1
    VoC_lw16i_short(REG6,7,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);//      line1+=7;
    VoC_add16_rr(REG5,REG5,REG6,IN_PARALLEL);//  line2+=7;
    VoC_jal(COOLSAND_DMAIRead_1280BY1024Odd);
    VoC_sw32_sd(REG45,0,DEFAULT);//save line2 line1
    VoC_lw16i_short(REG0,79,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,VPP_AMJR_JPEG_RGB_STRUCT,1);
LABLE_DO_BEGIN_FRONT_ODD:
    //  double2single(8,pin,bufftmp);   //-1
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    exit_on_warnings = OFF;
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);
    exit_on_warnings = ON;
    VoC_jal(COOLSAND_Double2Single);

    //double2single(8,pin+640,pout+48*2);  //2
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*2,pout+48*4);//4
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);


    //  double2single(8,pin+640*3,pout+48*6);//6
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*4,&bufftmp[48]);//8
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP2,1);
    VoC_jal(COOLSAND_Double2Single);

    //  double2row(16,bufftmp,pout+48*2,pout);//0
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_2_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);


    // memcpy(pout+48,pout,48*2);            //1
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//in
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy);

    //奇数行
    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//3
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_4_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//5
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_6_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*6,&bufftmp[48],pout+48*7);//7
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP2,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock);

    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW3,REG0);
    VoC_jump(LABLE_DO_BEGIN_FRONT_ODD);
LABLE_LASTROW3:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count

    //  double2single(8,pin,bufftmp);   //-1
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //double2single(8,pin+640,pout+48*2);  //2
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*2,pout+48*4);//4
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);


    //  double2single(8,pin+640*3,pout+48*6);//6
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*4,&bufftmp[48]);//8
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP2,1);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2row(16,bufftmp,pout+48*2,pout);//0
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_2_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);


    // memcpy(pout+48,pout,48*2);            //1
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//in
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy);

    //奇数行
    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//3
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_4_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//5
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_6_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*6,&bufftmp[48],pout+48*7);//7
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP2,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock);

    ////evev-16  second-8////////////////////
    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_RGB_640_4_ADDR,1);
    VoC_lw16i(REG0,79);//count=79;
LABLE_DO_BEGIN_BEHIND_ODD:
    //  double2single(8,pin+640*4,pout);//8
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);
    exit_on_warnings = OFF;
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);
    exit_on_warnings = ON;
    VoC_jal(COOLSAND_Double2Single);

    //double2single(8,pin+640*5,pout+48*2);//10
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*6,pout+48*4);//12
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*7,pout+48*6);//14
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*8,bufftmp);//-2
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2Single);


    //奇数行
    //  double2row(16,pout,pout+48*2,pout+48*1);//9
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_2_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_1_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//11
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_4_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//13
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_6_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*6,bufftmp,pout+48*7);//15
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock);

    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW4,REG0);
    VoC_jump(LABLE_DO_BEGIN_BEHIND_ODD);
LABLE_LASTROW4:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count

    //  double2single(8,pin+640*4,pout);//8
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //double2single(8,pin+640*5,pout+48*2);//10
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*6,pout+48*4);//12
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*7,pout+48*6);//14
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*8,bufftmp);//-2
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2SingleLastRow);


    //奇数行
    //  double2row(16,pout,pout+48*2,pout+48*1);//9
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_2_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_1_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//11
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_4_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//13
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_6_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*6,bufftmp,pout+48*7);//15
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock);

    VoC_pop32(REG45,DEFAULT);
    VoC_lw16i_short(REG6,8,IN_PARALLEL);
    //update line1 and line2
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG6,IN_PARALLEL);

    VoC_bgt16_rd(LABLE_END_1280BY1024,REG5,CONST_479_ADDR);
    VoC_jump(LABLE_START_1280BY1024);
LABLE_END_1280BY1024:
    ////////////////////////////////////////////////////////////////////
    VoC_jal(COOLSNAD_flush_bits);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;


}


////////////////////////////////////////////////
void DEMAI_READ_RGBDATA(void)
{
    /*  DMA_I
       IN_ADDR : INT
       SIZE:     INT
       EX_ADDR:  BYTE
    */
    VoC_lw32_d(ACC0,GLOBAL_IN_VIDEO_BUF_START);//[BYTE]
    VoC_lw16i(REG6,VPP_AMJR_JPEG_RGB_STRUCT);//VoC_shr16_ri(REG0,1,IN_PARALLEL);//dmai-addr[int]
    VoC_shr16_ri(REG6,1,DEFAULT);//[int]
    VoC_lw16i_short(INC2,-1,IN_PARALLEL);


    VoC_lw32_d(REG23,CONST_BLOCKSIZE_BYTE_ADDR);//SIZE [BYTE]  32-BIT
    VoC_add32_rr(RL6,REG23,ACC0,DEFAULT);//NEW EX_ADDR [BYTE]
    VoC_shr16_ri(REG2,2,IN_PARALLEL);//SIZE [INT]

    VoC_movreg16(REG6,REG2,DEFAULT);
    VoC_movreg16(REG7,REG6,IN_PARALLEL);

    VoC_push16(RA,DEFAULT);
    VoC_jal(vpp_AmjrJpegRead);
    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);

    VoC_sw32_d(RL6,GLOBAL_IN_VIDEO_BUF_START);

    /////////////////////////////////////////////////////////
    VoC_bnez16_d(LABLE_NONE_MULTIPLE,GLOBAL_HEIGHT_SAVE_ADDR);
    VoC_return;

LABLE_NONE_MULTIPLE:

    VoC_lw16_d(REG2,GLOBAL_WIDTH_SAVE_ADDR);//original width
    VoC_lw16_d(REG3,GLOBAL_VIDEO_WIDTH);//new width

    VoC_sub16_rr(REG4,REG3,REG2,DEFAULT);
    VoC_shru16_rr(REG6,REG0,IN_PARALLEL);//2**N

    VoC_shru16_rr(REG2,REG0,DEFAULT);//16*original width  or 8*original width
    VoC_shru16_rr(REG3,REG0,IN_PARALLEL);//16*new width  or 8*new width

    VoC_lw16i(REG5,VPP_AMJR_JPEG_RGB_STRUCT)
    VoC_add16_rr(REG2,REG2,REG5,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG5,IN_PARALLEL);

    VoC_sub16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);


    VoC_sub16_rr(REG2,REG2,REG7,DEFAULT);//last
    VoC_sub16_rr(REG3,REG3,REG7,IN_PARALLEL);//last
    VoC_sub16_rr(REG0,REG6,REG7,DEFAULT);


    VoC_lw16i(REG5,254);
    VoC_loop_r(1,REG0);//num of rows - 1
    VoC_movreg32(ACC0,REG23,DEFAULT);
    VoC_lw16i(REG7,127);
    VoC_lw16_d(REG6,GLOBAL_WIDTH_SAVE_ADDR);//original width
LABLE_LOOP0_START:
    VoC_bgt16_rr(LABLE_WIDTH_255,REG6,REG5);
    VoC_add16_rd(REG7,REG6,CONST_1_ADDR);
    VoC_shru16_ri(REG7,1,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_jump(LABLE_LOOP_BEGIN);
LABLE_WIDTH_255:
    VoC_sub16_rr(REG6,REG6,REG5,DEFAULT);
LABLE_LOOP_BEGIN:
    VoC_loop_r(0,REG7)
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_lw16inc_p(REG0,REG2,DEFAULT);
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_sw16inc_p(REG0,REG3,DEFAULT);
    VoC_endloop0
    VoC_bnez16_r(LABLE_LOOP0_START,REG6);
    VoC_movreg32(REG23,ACC0,DEFAULT);
    VoC_sub16_rd(REG2,REG2,GLOBAL_WIDTH_SAVE_ADDR);
    VoC_sub16_rd(REG3,REG3,GLOBAL_VIDEO_WIDTH);
    VoC_endloop1

    VoC_return;
}


/* encode_block:
 *  Encodes an 8x8 basic block of coefficients of given type (luminance or
 *  chrominance) and writes it to the output stream.
 */
void COOLSAND_encode_block(void)
{

    //input:r4:type  r2:old_dc addr
    //output:write old_dc  and  encoded  information into ram
    //all buf have been used

    VoC_push16(RA,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);//data  ONLY R0
    VoC_bne16_rd(LABLE_ENCODEBLOCK_TYPE1,REG4,CONST_0_ADDR);//if (type ==0)
    VoC_lw16i(REG0,CONST_MYHUFF_DC0_ADDR);//dc
    VoC_lw16i_set_inc(REG1,CONST_MYHUFF_AC0_ADDR+2,-2);//ac
    VoC_jump(LABLE_ENCODEBLOCK_TYPE_END);
LABLE_ENCODEBLOCK_TYPE1://  else if (type==1)
    VoC_lw16i(REG0,CONST_MYHUFF_DC1_ADDR);//dc
    VoC_lw16i_set_inc(REG1,CONST_MYHUFF_AC1_ADDR+2,-2);//ac
LABLE_ENCODEBLOCK_TYPE_END:

    // VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);

    VoC_push16(REG2,DEFAULT);//old_dcy/u/v addr
    VoC_lw16i_short(REG6,64,IN_PARALLEL);

    VoC_mult16_rr(REG6,REG6,REG4,DEFAULT);//64*type
    VoC_pop32(REG01,IN_PARALLEL);//data  ONLY R0
    exit_on_warnings = OFF;
    VoC_push32(REG01,DEFAULT);
    exit_on_warnings = ON;
    VoC_add16_rd(REG2,REG6,GLOBAL_Q_ADDR);//pTempQuanTab = quant_table + 64*type;


    VoC_jal(apply_fdct);//apply_fdct(data);//r2 is not used in this fun


    VoC_pop16(REG1,DEFAULT);//GLOBAL_OLD_DCY_ADDR
    VoC_lw16i_short(FORMAT16,1,IN_PARALLEL);//>>17

    VoC_push16(REG0,DEFAULT);//boloc[0] addr

    VoC_multf16inc_pp(REG4,REG0,REG2,DEFAULT);//dc_tmp = MPY_16_16_l19(block[0],pTempQuanTab[0]);//dc_tmp = (block[0] * quant_table[0]) >> 19;

    VoC_lw16i_short(INC0,1,DEFAULT);

    VoC_sub16_rp(REG4,REG4,REG1,DEFAULT);//value = dc_tmp - *old_dc;
    exit_on_warnings = OFF;
    VoC_sw16_p(REG4,REG1,DEFAULT);//*old_dc = dc_tmp;
    exit_on_warnings = ON;
    VoC_pop32(REG01,IN_PARALLEL);//r0:dc addr   r1:ac addr




    VoC_jal(format_number);//input:r4     //output: r0:Addr  r4:cat   r5:bits
    //unsed buf: r1 r2

    //put_bits(dc_table->code[category]->encoded_value,dc_table->code[category]->bits_length);
    VoC_lw32_p(REG67,REG0,DEFAULT);
    VoC_movreg16(REG0,REG1,IN_PARALLEL);//r0: ac addr
    VoC_push16(REG1,DEFAULT);//ac_table
    VoC_lw16i_short(REG3,0,IN_PARALLEL);//num_zeroes = 0;
    VoC_lw16i_set_inc(REG1,CONST_inverse_Zig_Zag_ADDR+1,1);
    VoC_jal(COOLSAND_put_bits);
    VoC_jal(COOLSAND_put_bits);
    //put_bits(REGS[5].reg, REGS[4].reg);


    //r1:inverse_Zig_Zag
    //r2:pTempQuanTab
    //r3:num_zeroes
    VoC_loop_i(1,63);//for (index = 1; index < 64; index++)

    VoC_lw16_sd(REG5,1,DEFAULT);//value = block[inverse_Zig_Zag[index]];
    VoC_add16inc_rp(REG5,REG5,REG1,DEFAULT);//value addr


    VoC_lw16i_short(FORMAT16,1,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_lw16_p(REG6,REG5,DEFAULT);//value
    VoC_multf16inc_rp(REG4,REG6,REG2,DEFAULT);
    VoC_shru16_ri(REG6,15,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);//value = MPY_16_16_l19((short)value,pTempQuanTab[index]) + ((value >> 31) & 0x1);// value = ((value * quant_table[index]) >> 19) + ((value >> 31) & 0x1);

    VoC_bne16_rd(LABLE_ENCODEBLOCK_VALUE_else,REG4,CONST_0_ADDR);//if (value == 0)
    VoC_inc_p(REG3,DEFAULT);//num_zeroes++;
    VoC_jump(LABLE_ENCODEBLOCK_VALUE_END);
LABLE_ENCODEBLOCK_VALUE_else:
    VoC_push16(REG4,DEFAULT);//value
LABLE_ENCODEBLOCK_num_zeroes_bgin:

    VoC_bngt16_rd(LABLE_ENCODEBLOCK_num_zeroes,REG3,CONST_15_ADDR);//while (num_zeroes > 15)
    VoC_lw32_p(REG67,REG0,DEFAULT);
    VoC_jal(COOLSAND_put_bits);
    //put_bits(ac_table->code[0xf0]->encoded_value,ac_table->code[0xf0]->bits_length);
    VoC_sub16_rd(REG3,REG3,CONST_16_ADDR);//num_zeroes -= 16;
    VoC_bgt16_rd(LABLE_ENCODEBLOCK_num_zeroes_bgin,REG3,CONST_15_ADDR);
LABLE_ENCODEBLOCK_num_zeroes:
    VoC_pop16(REG4,DEFAULT);//value

    VoC_push16(REG3,DEFAULT);//num_zeroes
    VoC_lw16i_short(FORMAT16,-16,IN_PARALLEL);
    VoC_jal(format_number);//format_number(value, &category, &bits);
    VoC_pop16(REG3,DEFAULT);//REGS[3].reg = (short)num_zeroes;
    VoC_lw16i_short(REG6,20,IN_PARALLEL);
    VoC_mult16_rr(REG3,REG3,REG6,DEFAULT);//num * 20
    VoC_NOP();
    VoC_add16_rr(REG0,REG3,REG0,DEFAULT);//ac addr
    VoC_lw16i_short(REG3,0,IN_PARALLEL);//num_zeroes = 0;

    VoC_lw16_sd(REG0,0,DEFAULT);//VoC_movreg16(REG0,REG1,DEFAULT);
    exit_on_warnings = OFF;
    VoC_lw32_p(REG67,REG0,DEFAULT);
    exit_on_warnings = ON;
    //put_bits(ac_table->code[value]->encoded_value,ac_table->code[value]->bits_length);
    VoC_jal(COOLSAND_put_bits);
    VoC_jal(COOLSAND_put_bits);


LABLE_ENCODEBLOCK_VALUE_END:
    VoC_lw16i_short(INC0,-2,DEFAULT);
    VoC_endloop1


    VoC_inc_p(REG0,DEFAULT);//r0-2
    VoC_pop16(REG6,IN_PARALLEL);//AC_ADDR

    VoC_pop16(REG6,DEFAULT);//BLOCK

    VoC_bngt16_rd(LABLE_ENCODEBLOCK_RETURN,REG3,CONST_0_ADDR);//if (num_zeroes > 0)
    VoC_lw32_p(REG67,REG0,DEFAULT);//put_bits(ac_table->code[0x0]->encoded_value,ac_table->code[0x0]->bits_length);
    VoC_jal(COOLSAND_put_bits);
LABLE_ENCODEBLOCK_RETURN:

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
//  return 0;
}
/* apply_fdct:
 *  Applies the forward discrete cosine transform to the given input block,
 *  in the form of a vector of 64 coefficients.
 *  This uses integer fixed point math and is based on code by the IJG.
 */
void apply_fdct(void)
{
#if 0
    voc_short TEMP_BUFF_64_ADDR[32] ,y
#endif
    //input:data// VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,2);
    //output:data
    //used buf: reg0 reg1 reg3 reg4 reg5 reg6 reg7  acc0 acc1 rl6 rl7
    //unused buf:reg2
    /////////////////////////////////////////////////////////////////////
    VoC_lw16i_set_inc(REG1,TEMP_BUFF_64_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    exit_on_warnings = OFF;
    VoC_push32(REG01,DEFAULT);//o
    exit_on_warnings = ON;
    VoC_loop_i_short(16,DEFAULT);//64 take the even element to the temp_buff_64_addr
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);//even part
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings = ON;

    VoC_endloop0

    VoC_lw32_sd(REG01,0,DEFAULT);//VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,2);//even
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,TEMP_BUFF_64_ADDR+3,-1);//odd
    VoC_lw16i_set_inc(REG3,JPEGENC_MYDCT_ADDR,1);

    VoC_loop_i(0,8);//for (i = 8; i; i--)
    //VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    //VoC_startloop0
    VoC_add16inc_pp(REG4,REG1,REG0,DEFAULT);//tmp0 = dataptr[0] + dataptr[7];
    VoC_add16inc_pp(REG5,REG1,REG0,DEFAULT);//tmp2 = dataptr[2] + dataptr[5];
    VoC_add16inc_pp(REG6,REG1,REG0,DEFAULT);//tmp3 = dataptr[4] + dataptr[3];
    VoC_add16_pp(REG7,REG1,REG0,DEFAULT);//tmp1 = dataptr[6] + dataptr[1];
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);//tmp10 = tmp0 + tmp3;
    VoC_sub16_rr(REG6,REG4,REG6,IN_PARALLEL);//tmp13 = tmp0 - tmp3;

    VoC_add16_rr(REG5,REG7,REG5,DEFAULT);//tmp11 = tmp1 + tmp2;
    VoC_sub16_rr(REG7,REG7,REG5,IN_PARALLEL);//tmp12 = tmp1 - tmp2;

    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);//dataptr[0] = (tmp10 + tmp11) << 2;
    VoC_sub16_rr(REG5,REG4,REG5,IN_PARALLEL);//dataptr[4] = (tmp10 - tmp11) << 2;

    VoC_shr16_ri(REG4,-2,DEFAULT);
    VoC_shr16_ri(REG5,-2,IN_PARALLEL);

    //z1 = MPY_16_16((short)(tmp12 + tmp13) , 4433);
    VoC_movreg32(RL6,REG45,DEFAULT);//RL6:0 4
    VoC_add16_rr(REG4,REG7,REG6,IN_PARALLEL);//(tmp12 + tmp13)

    VoC_multf32inc_rp(ACC0,REG4,REG3,DEFAULT);//z1
    VoC_lw16i_short(INC0,-2,IN_PARALLEL);
    /////////////////////////////////////////////////
    //dataptr[2] = (z1 + MPY_16_16(tmp13 ,6270)) >> 11;
    //VoC_lw16i(REG3,6270);
    VoC_mac32inc_rp(REG6,REG3,DEFAULT);
    exit_on_warnings = OFF;
    VoC_movreg32(ACC1,ACC0,DEFAULT);
    exit_on_warnings = ON;
    //dataptr[6] = (z1 + MPY_16_16(tmp12 , -15137)) >> 11;
    //VoC_lw16i(REG3,-15137);
    VoC_shr32_ri(ACC0,11,DEFAULT);//dataptr[2]
    VoC_mac32inc_rp(REG7,REG3,IN_PARALLEL);
    //////////////////////////////////////////////////////////////////

    VoC_sub16inc_pp(REG6,REG1,REG0,DEFAULT);//tmp6 = dataptr[1] - dataptr[6];
    VoC_sub16inc_pp(REG4,REG1,REG0,DEFAULT);//tmp4 = dataptr[3] - dataptr[4];
    VoC_sub16inc_pp(REG5,REG0,REG1,DEFAULT);//tmp5 = dataptr[2] - dataptr[5];
    VoC_shr32_ri(ACC1,11,IN_PARALLEL);//dataptr[6]
    VoC_sub16_pp(REG7,REG0,REG1,DEFAULT);//tmp7 = dataptr[0] - dataptr[7];
    VoC_lw16i_short(INC0,2,IN_PARALLEL);


    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);//z1 = tmp4 + tmp7;
    VoC_push16(REG4,IN_PARALLEL);//tmp4
    /////write dataptr 0 2 4 6/////////////////OP
    VoC_sw16inc_p(RL6_LO,REG0,DEFAULT);//0
    VoC_multf32inc_rp(ACC0,REG4,REG3,IN_PARALLEL);//z1 *= -7373;
    VoC_pop16(REG4,DEFAULT);//tmp4
    VoC_mac32inc_rp(REG4,REG3,DEFAULT);//tmp4 *= 2446; -> tmp4 + z1->acc0
    VoC_sw16inc_p(ACC0_LO,REG0,IN_PARALLEL);//2
    exit_on_warnings = OFF;
    VoC_push32(ACC0,DEFAULT);//z1
    exit_on_warnings = ON;
    VoC_sw16inc_p(RL6_HI,REG0,DEFAULT);//4
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);//z3 = tmp4 + tmp6;
    VoC_sw16_p(ACC1_LO,REG0,DEFAULT);//6
    VoC_multf32inc_rp(RL6,REG4,REG3,IN_PARALLEL);//z3 *= -16069;
    //////////////////////////////////////////
    VoC_multf32inc_rp(RL7,REG7,REG3,DEFAULT);//tmp7 *= 12299;
    VoC_add16_rr(REG7,REG7,REG5,IN_PARALLEL);//z4 = tmp5 + tmp7;

    VoC_multf32inc_rp(RL6,REG5,REG3,DEFAULT);//tmp5 *= 16819;
    VoC_add16_rr(REG4,REG4,REG7,IN_PARALLEL);//z3+z4
    exit_on_warnings = OFF;
    VoC_push32(RL6,DEFAULT);//z3
    exit_on_warnings = ON;
    VoC_add16_rr(REG6,REG5,REG6,DEFAULT);//z2 = tmp5 + tmp6;
    VoC_multf32inc_rp(ACC1,REG6,REG3,IN_PARALLEL);//tmp6 *= 25172;
    VoC_push32(RL7,DEFAULT);//tmp7
    VoC_multf32inc_rp(RL7,REG6,REG3,DEFAULT);//z2 *= -20995;
    VoC_multf32inc_rp(REG67,REG7,REG3,DEFAULT);//z4 *= -3196;

    VoC_add32_rr(RL6,RL6,RL7,DEFAULT);//tmp5 + z2
    VoC_add32_rr(ACC1,ACC1,RL7,IN_PARALLEL);//tmp6 + z2

    VoC_pop32(RL7,DEFAULT);//tmp7
    VoC_push16(REG4,IN_PARALLEL);//(z3 + z4)

    VoC_lw32_sd(REG45,1,DEFAULT);//z1
    VoC_add32_rr(RL7,REG45,RL7,DEFAULT);//tmp7 + z1
    VoC_pop16(REG4,IN_PARALLEL);

    VoC_multf32_rp(REG45,REG4,REG3,DEFAULT);//z5 = (z3 + z4) * 9633;
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_push32(ACC1,DEFAULT);
    VoC_add32_rr(REG67,REG67,REG45,DEFAULT);//z4 += z5;
    VoC_lw32_sd(ACC1,1,IN_PARALLEL);//z3
    VoC_add32_rr(REG45,ACC1,REG45,DEFAULT);//z3 += z5;
    VoC_pop32(ACC1,IN_PARALLEL);


    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT);//dataptr[7] = (tmp4 + z1 + z3) >> 11;//->acc0
    VoC_add32_rr(RL6,RL6,REG67,IN_PARALLEL);//dataptr[5] = (tmp5 + z2 + z4) >> 11;//rl6
    VoC_add32_rr(ACC1,ACC1,REG45,DEFAULT);//dataptr[3] = (tmp6 + z2 + z3) >> 11;//acc1
    VoC_add32_rr(RL7,RL7,REG67,IN_PARALLEL);//dataptr[1] = (tmp7 + z1 + z4) >> 11;//rl7

    VoC_shr32_ri(RL7,11,DEFAULT);
    VoC_lw16i_short(REG6,-5,IN_PARALLEL);


    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);//dataprt[1] addr
    VoC_shr32_ri(ACC1,11,IN_PARALLEL);

    VoC_pop32(REG45,DEFAULT);//z3
    VoC_shr32_ri(RL6,11,IN_PARALLEL);


    VoC_pop32(REG45,DEFAULT);//z1
    VoC_shr32_ri(ACC0,11,IN_PARALLEL);

    VoC_lw16i_short(REG7,4,DEFAULT);
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);

    VoC_sw16inc_p(RL7_LO,REG0,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_sw16inc_p(ACC1_LO,REG0,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);
    VoC_sw16inc_p(RL6_LO,REG0,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG6,IN_PARALLEL);//r0
    exit_on_warnings = OFF;
    VoC_sw16_p(ACC0_LO,REG0,DEFAULT);
    exit_on_warnings = ON;
    VoC_lw16i_set_inc(REG3,JPEGENC_MYDCT_ADDR,1);
    VoC_endloop0

    /////part 2//////////////////////////////
    VoC_lw32_sd(REG01,0,DEFAULT);//dataptr = data;
    VoC_lw16i_short(REG4,32,IN_PARALLEL);

    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);//dataptr[32] addr
    VoC_lw16i_short(INC0,8,IN_PARALLEL);

    VoC_lw16i_short(INC1,-1,DEFAULT);
    VoC_lw16i_short(REG5,-7,IN_PARALLEL);
    /////loop of move begin///////////////
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_lw16i_short(REG7,3,IN_PARALLEL);
    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_lw16i_short(REG7,8,IN_PARALLEL);
    VoC_loop_i(1,8);
    VoC_loop_i_short(2,DEFAULT);
    VoC_movreg16(REG4,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop0
    VoC_add16_rr(REG0,REG4,REG5,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_endloop1

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    //VoC_loop_i_short(8,DEFAULT);//for (i = 8; i; i--)
    VoC_loop_i(0,8)
    //VoC_startloop0

    VoC_push16(REG0,DEFAULT);
    VoC_sub16_pp(REG7,REG0,REG1,DEFAULT);//tmp7 = dataptr[0] - dataptr[56];
    VoC_add16inc_pp(REG4,REG0,REG1,DEFAULT);//tmp0 = dataptr[0] + dataptr[56];
    VoC_sub16_pp(REG6,REG0,REG1,DEFAULT);//tmp6 = dataptr[8] - dataptr[48];
    VoC_add16inc_pp(REG5,REG0,REG1,DEFAULT);//tmp1 = dataptr[8] + dataptr[48];
    VoC_push32(REG67,DEFAULT);//<-tmp6 tmp7
    VoC_push32(REG45,DEFAULT);//<-tmp0 tmp1
    VoC_sub16_pp(REG5,REG0,REG1,DEFAULT);//tmp5 = dataptr[16] - dataptr[40];
    VoC_add16inc_pp(REG6,REG0,REG1,DEFAULT);//tmp2 = dataptr[16] + dataptr[40];
    VoC_sub16_pp(REG4,REG0,REG1,DEFAULT);//tmp4 = dataptr[24] - dataptr[32];
    VoC_add16inc_pp(REG7,REG0,REG1,DEFAULT);//tmp3 = dataptr[24] + dataptr[32];
    VoC_pop32(ACC0,DEFAULT);//->tmp0 tmp1
    VoC_push32(REG45,DEFAULT);//<-tmp4 tmp5
    VoC_movreg32(REG45,ACC0,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);//tmp10 = tmp0 + tmp3;
    VoC_sub16_rr(REG7,REG4,REG7,IN_PARALLEL);//tmp13 = tmp0 - tmp3;
    VoC_add16_rr(REG5,REG6,REG5,DEFAULT);//tmp11 = tmp1 + tmp2;
    VoC_sub16_rr(REG6,REG5,REG6,IN_PARALLEL);//tmp12 = tmp1 - tmp2;

    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);//dataptr[0] = (tmp10 + tmp11) >> 2;
    VoC_sub16_rr(REG5,REG4,REG5,IN_PARALLEL);//dataptr[32] = (tmp10 - tmp11) >> 2;

    VoC_shr16_ri(REG4,2,DEFAULT);
    VoC_shr16_ri(REG5,2,IN_PARALLEL);

    //z1 = (tmp12 + tmp13) * FIX_0_541196100;
    VoC_movreg32(RL6,REG45,DEFAULT);//dataptr[0] dataptr[32
    VoC_add16_rr(REG4,REG6,REG7,IN_PARALLEL);//tmp12 + tmp13

    VoC_multf32inc_rp(ACC0,REG4,REG3,DEFAULT);
    VoC_lw16i_short(INC0,16,IN_PARALLEL);
    /////////////////////////////////////////////
    VoC_mac32inc_rp(REG7,REG3,DEFAULT);//dataptr[16] = (z1 + (tmp13 * FIX_0_765366865)) >> 15;
    exit_on_warnings = OFF;
    VoC_movreg32(ACC1,ACC0,DEFAULT);
    exit_on_warnings = ON;
    VoC_shr32_ri(ACC0,15,DEFAULT);
    VoC_mac32inc_rp(REG6,REG3,IN_PARALLEL);//dataptr[48] = (z1 + (tmp12 * -FIX_1_847759065)) >> 15;

    VoC_lw32_sd(REG45,0,DEFAULT);//tmp4 tmp5
    VoC_lw16_sd(REG0,0,IN_PARALLEL);
    VoC_lw32_sd(REG67,1,DEFAULT);//tmp6 tmp7
    VoC_shr32_ri(ACC1,15,IN_PARALLEL);
    ///store some elements in to the ram//  op
    VoC_sw16inc_p(RL6_LO,REG0,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG7,IN_PARALLEL);//z1 = tmp4 + tmp7;
    VoC_sw16inc_p(ACC0_LO,REG0,DEFAULT);
    VoC_multf32inc_rp(ACC0,REG4,REG3,IN_PARALLEL);//z1 *= -FIX_0_899976223;
    VoC_sw16inc_p(RL6_HI,REG0,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);
    VoC_sw16inc_p(ACC1_LO,REG0,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);
    ///////////////////////////////////////

    VoC_mac32inc_rp(REG4,REG3,DEFAULT);//tmp4 *= FIX_0_298631336;
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);//z3 = tmp4 + tmp6;

    VoC_multf32inc_rp(RL6,REG4,REG3,DEFAULT);//z3 *= -16069;
    exit_on_warnings = OFF;
    VoC_push32(ACC0,DEFAULT);//z1
    exit_on_warnings = ON;
    VoC_multf32inc_rp(RL7,REG7,REG3,DEFAULT);//tmp7 *= 12299;
    VoC_add16_rr(REG7,REG7,REG5,IN_PARALLEL);//z4 = tmp5 + tmp7;
    VoC_push32(RL6,DEFAULT);//z3
    VoC_multf32inc_rp(RL6,REG5,REG3,DEFAULT);//tmp5 *= 16819;
    VoC_add16_rr(REG4,REG4,REG7,IN_PARALLEL);//z3+z4

    VoC_add16_rr(REG6,REG5,REG6,DEFAULT);//z2 = tmp5 + tmp6;
    VoC_multf32inc_rp(ACC1,REG6,REG3,IN_PARALLEL);//tmp6 *= 25172;
    VoC_push32(RL7,DEFAULT);//tmp7
    VoC_multf32inc_rp(RL7,REG6,REG3,DEFAULT);//z2 *= -20995;
    VoC_multf32inc_rp(REG67,REG7,REG3,DEFAULT);//z4 *= -3196;

    VoC_add32_rr(RL6,RL6,RL7,DEFAULT);//tmp5 + z2
    VoC_add32_rr(ACC1,ACC1,RL7,IN_PARALLEL);//tmp6 + z2

    VoC_pop32(RL7,DEFAULT);//tmp7
    VoC_push16(REG4,IN_PARALLEL);//(z3 + z4)

    VoC_lw32_sd(REG45,1,DEFAULT);//z1
    VoC_add32_rr(RL7,REG45,RL7,DEFAULT);//tmp7 + z1
    VoC_pop16(REG4,IN_PARALLEL);

    VoC_multf32_rp(REG45,REG4,REG3,DEFAULT);//z5 = (z3 + z4) * 9633;
    VoC_push32(ACC1,DEFAULT);
    VoC_add32_rr(REG67,REG67,REG45,DEFAULT);//z4 += z5;
    VoC_lw32_sd(ACC1,1,IN_PARALLEL);//z3
    VoC_add32_rr(REG45,ACC1,REG45,DEFAULT);//z3 += z5;
    VoC_pop32(ACC1,IN_PARALLEL);

    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT);//dataptr[56] = (tmp4 + z1 + z3) >> 15;//->acc0
    VoC_add32_rr(RL6,RL6,REG67,IN_PARALLEL);//dataptr[40] = (tmp5 + z2 + z4) >> 15;//rl6
    VoC_add32_rr(ACC1,ACC1,REG45,DEFAULT);//dataptr[24] = (tmp6 + z2 + z3) >> 15//acc1
    VoC_add32_rr(RL7,RL7,REG67,IN_PARALLEL);//dataptr[8] = (tmp7 + z1 + z4) >> 15//rl7

    VoC_lw16i_short(REG6,8,DEFAULT);//dataptr[8]
    VoC_pop16(REG0,IN_PARALLEL);

    VoC_shr32_ri(ACC1,15,DEFAULT);

    VoC_shr32_ri(RL6,15,DEFAULT);
    VoC_shr32_ri(ACC0,15,IN_PARALLEL);

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_shr32_ri(RL7,15,IN_PARALLEL);

    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);//dataptr[8] addr
    VoC_add16_rr(REG7,REG0,REG7,IN_PARALLEL);//prepare r0 for the next loop

    VoC_lw16i_set_inc(REG3,JPEGENC_MYDCT_ADDR,1);

    VoC_sw16inc_p(RL7_LO,REG0,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);//z1
    VoC_sw16inc_p(ACC1_LO,REG0,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);//z3
    VoC_sw16inc_p(RL6_LO,REG0,DEFAULT);
    VoC_movreg16(REG0,REG7,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_sw16_p(ACC0_LO,REG0,DEFAULT);
    VoC_lw16i_short(INC0,8,IN_PARALLEL);
    exit_on_warnings = ON;

    VoC_endloop0

    VoC_pop32(REG01,DEFAULT);
    VoC_lw16i_short(INC0,-2,IN_PARALLEL);


    VoC_return;
}

/* format_number:
 *  Computes the category and bits of a given number.
 */
void format_number(void)
{

    // REGS[4].reg = (short)num;
    //input:r4
    //output: r0:Addr  r4:cat   r5:bits
    //used buf: r3 r4 r5 r6 r7
    VoC_lw16i_short(INC3,-1,DEFAULT);

    VoC_movreg16(REG5,REG4,DEFAULT);//mask = num >> 31;
    VoC_shr16_ri(REG4,15,IN_PARALLEL);//r4:mask  r5:num

    VoC_xor16_rr(REG4,REG5,DEFAULT);//abs_num = (num ^ mask) - mask;
    VoC_movreg16(REG6,REG4,IN_PARALLEL);//r6:mask  r4:(num ^ mask)

    VoC_shru16_ri(REG6,15,DEFAULT);//num_sign = mask&0x1;
    VoC_sub16_rr(REG4,REG4,REG6,IN_PARALLEL);//abs_num

    VoC_loop_i_short(16,DEFAULT);//for (cat = 0; abs_num; cat++)
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_startloop0;
    VoC_be16_rd(LABLE_FORMAT_NUM_CAT,REG4,CONST_0_ADDR);//abs_num >>= 1;
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);//r3:-cat//*category = cat;
    VoC_endloop0;
LABLE_FORMAT_NUM_CAT://r4==0

    VoC_shr16_rr(REG6,REG3,DEFAULT);//(num_sign <<cat)
    VoC_sub16_rr(REG5,REG5,REG6,IN_PARALLEL);//num-num_sign

    VoC_add16_rr(REG5,REG5,REG6,DEFAULT);//*bits = num +(num_sign <<cat)-num_sign;
    VoC_sub16_rr(REG4,REG4,REG3,IN_PARALLEL);//cab

    VoC_shr16_ri(REG3,-1,DEFAULT);
    VoC_sub16_rr(REG0,REG0,REG3,DEFAULT);//r0+2*cat

    //*bits = REGS[5].reg;
    //*category = REGS[4].reg;
    VoC_return;
}

/* put_bits:
 *   Writes some bits to the output stream.
 */
void COOLSAND_put_bits(void)
{
    //input:value:r7  numbits:r6
    //used buf:r3 r4 r5 r6 r7
    //output: r45->r67

    VoC_push16(REG2,DEFAULT);
    VoC_push16(REG3,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);

    VoC_lw32_d(REG45,GLOBAL_CURRBITS_ADDR);
    VoC_lw16i(REG2,PUT_BITS_BUF);

    VoC_bnlt16_rd(LABLE_CHANGE_5,REG6,GLOBAL_CURRBITS_ADDR);
    VoC_jump(LABLE_PUTBITS_WILE_END);
LABLE_CHANGE_5:
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_be16_rd(LABLE_PUTBITS_WILE_BEGIN1,REG3,GLOBAL_SHORT_1_ADDR);
    VoC_lw16_d(REG3,GLOBAL_pp_ADDR);
    VoC_NOP();
    VoC_push16(REG3,DEFAULT);

LABLE_PUTBITS_WILE_BEGIN1:
    VoC_lw16d_set_inc(REG3,GLOBAL_CURR_POS_ADDR,1);

LABLE_PUTBITS_WILE_BEGIN:
    //while(num_bits >= current_bit)
    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);//num_bits -= current_bit;

    VoC_shru16_rr(REG7,REG6,DEFAULT);//current_byte |= (value >> num_bits);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_or16_rr(REG5,REG7,DEFAULT);
    VoC_push32(REG67,IN_PARALLEL);
    exit_on_warnings = ON;

    VoC_and16_ri(REG5,0x00ff);

    VoC_bne16_rd(LABLE_PUTBITS_0XFF,REG5,CONST_0xff_ADDR);//if ((current_byte&0xff) == 0xff)

    //*chunkptr++ = 0;
    VoC_add16_rr(REG3,REG3,REG2,DEFAULT);
    VoC_be16_rd(LABLE_PUTBITS_WRITE_short_1,REG6,GLOBAL_SHORT_1_ADDR);//=1

    VoC_shru16_ri(REG5,-8,DEFAULT);
    VoC_pop16(REG7,IN_PARALLEL);//
    VoC_or16_rr(REG5,REG7,DEFAULT);
    VoC_push16(REG6,DEFAULT);

LABLE_PUTBITS_WRITE_short_1://=0

    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG2,IN_PARALLEL);

    VoC_jump(LABLE_PUTBITS_WRITE_1024_begin);

LABLE_PUTBITS_0XFF://*chunkptr++ = current_byte;

    VoC_be16_rd(LABLE_PUTBITS_WRITE_ORNOT,REG6,GLOBAL_SHORT_1_ADDR);//=1

    VoC_pop16(REG7,DEFAULT);//op
    VoC_shru16_ri(REG5,-8,IN_PARALLEL);

    VoC_or16_rr(REG5,REG7,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG2,IN_PARALLEL);

    VoC_sw16_d(REG6,GLOBAL_SHORT_1_ADDR);

    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG2,IN_PARALLEL);

    VoC_jump(LABLE_PUTBITS_WRITE_ORNOT_END);

LABLE_PUTBITS_WRITE_ORNOT://=0

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_push16(REG5,DEFAULT);
    VoC_sw16_d(REG7,GLOBAL_SHORT_1_ADDR);

LABLE_PUTBITS_WRITE_ORNOT_END:

LABLE_PUTBITS_WRITE_1024_begin:
    VoC_lw16i(REG4,512);
    VoC_bgt16_rr(LABLE_PUTBITS_WRITE_1024,REG4,REG3);//if(chunkptr == chunk_end)

    VoC_push16(REG2,DEFAULT);
    ////////dmai/////////////////////
    VoC_lw16i(REG2,256);//size[int]
    VoC_push16(RA,DEFAULT);
    VoC_lw16i(REG3,PUT_BITS_BUF/2);//ADDR-[int]
    VoC_jal(DMAI_WRITE);//return r2=1024
    VoC_pop16(RA,DEFAULT);
    //////dmae//////////////////////

    //compute  codelength//
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_add32_rd(REG23,REG23,GLOBAL_VIDEO_OUT_LEN);//byte

    VoC_lw16i_short(REG3,0,DEFAULT);//recover the pointor
    VoC_pop16(REG2,IN_PARALLEL);

    exit_on_warnings = OFF;
    VoC_sw32_d(REG23,GLOBAL_VIDEO_OUT_LEN);
    exit_on_warnings = ON;
LABLE_PUTBITS_WRITE_1024:


    VoC_lw16i_short(REG4,8,DEFAULT);//current_bit = 8;
    VoC_pop32(REG67,DEFAULT);//r7:value  r6:num
    VoC_lw16i_short(REG5,0,IN_PARALLEL);//current_byte = 0;
    VoC_bngt16_rr(LABLE_PUTBITS_WILE_BEGIN,REG4,REG6);//while(num_bits >= current_bit)


    VoC_lw16i_short(REG3,0,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16_d(REG3,GLOBAL_CURR_POS_ADDR);
    exit_on_warnings = ON;


    VoC_be16_rd(LABLE_PUTBITS_WILE_END,REG3,GLOBAL_SHORT_1_ADDR);
    VoC_pop16(REG3,DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG3,GLOBAL_pp_ADDR);
LABLE_PUTBITS_WILE_END:
    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);//current_bit -= num_bits;
    VoC_sub16_rr(REG4,REG4,REG6,IN_PARALLEL);

    VoC_shru16_rr(REG7,REG6,DEFAULT);//current_byte |= (value << current_bit);
    VoC_or16_rr(REG5,REG7,DEFAULT);

    VoC_pop32(REG67,DEFAULT);
    VoC_pop16(REG3,IN_PARALLEL);
    VoC_pop16(REG2,DEFAULT);

    VoC_sw32_d(REG45,GLOBAL_CURRBITS_ADDR);

    VoC_return;//return 0;
}

void COOLSNAD_flush_bits(void)
{
    //used buf:r3 r4 r5 r6 r7
    VoC_push16(REG2,DEFAULT);

    VoC_lw32_d(REG45,GLOBAL_CURRBITS_ADDR);
    VoC_lw16d_set_inc(REG3,GLOBAL_CURR_POS_ADDR,1);
    VoC_lw16i(REG2,PUT_BITS_BUF);

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_sub16_rr(REG4,REG6,REG4,DEFAULT);//current_bit--;
    ////////////////////////////////////////////////////////////////////
    VoC_bgt16_rd(LABLE_CONNS,REG4,CONST_NEG7_ADDR);
    VoC_jump(LABLE_FLUSH_WHILE_END2);
LABLE_CONNS:
    VoC_bne16_rd(LABLE_FLUSH_WHILE_BEGIN,REG6,GLOBAL_SHORT_1_ADDR);
    VoC_lw16_d(REG7,GLOBAL_pp_ADDR);//=1
    VoC_NOP();
    VoC_push16(REG7,DEFAULT);
    /////////////////////////////////////////////////////////////////////
LABLE_FLUSH_WHILE_BEGIN://  while (current_bit < 7)  -current_bit > -7
    VoC_shru16_rr(REG6,REG4,DEFAULT);//1 << current_bit
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);//current_bit--;

    VoC_or16_rr(REG5,REG6,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_bngt16_rd(LABLE_FLUSH_WHILE_BEGIN,REG4,CONST_0_ADDR);//if (current_bit < 0)


    //if (current_byte == 0xff) _jpeg_putc(0);
    VoC_bne16_rd(LABLE_FLUSH_0XFF,REG5,CONST_0xff_ADDR);//if ((current_byte&0xff) == 0xff)

    VoC_add16_rr(REG3,REG3,REG2,DEFAULT);
    VoC_be16_rd(LABLE_FLUSH_WRITE_short_1,REG6,GLOBAL_SHORT_1_ADDR);//=1

    VoC_pop16(REG7,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_or16_ri(REG7,0xff);
    VoC_push16(REG6,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG2,IN_PARALLEL);

    VoC_jump(LABLE_FLUSH_WRITE_1024_begin);

LABLE_FLUSH_WRITE_short_1://=0

    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG2,IN_PARALLEL);

    VoC_jump(LABLE_FLUSH_WRITE_1024_begin);

LABLE_FLUSH_0XFF:

    //_jpeg_putc(/*current_byte*/);
    VoC_bne16_rd(LABLE_FLUSH_WRITE_ORNOT,REG6,GLOBAL_SHORT_1_ADDR);//=1

    VoC_pop16(REG7,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG2,IN_PARALLEL);

    VoC_shru16_ri(REG5,-8,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);

    VoC_or16_rr(REG7,REG5,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    exit_on_warnings = OFF;
    VoC_sw16_d(REG6,GLOBAL_SHORT_1_ADDR);
    exit_on_warnings = ON;

    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG2,IN_PARALLEL);

    VoC_jump(LABLE_FLUSH_WRITE_ORNOT_END);

LABLE_FLUSH_WRITE_ORNOT://=0

    VoC_push16(REG5,DEFAULT);
    VoC_sw16_d(REG6,GLOBAL_SHORT_1_ADDR);

LABLE_FLUSH_WRITE_ORNOT_END:


LABLE_FLUSH_WRITE_1024_begin:
    VoC_lw16i(REG4,512);
    VoC_bne16_rr(LABLE_CHANGE_10,REG3,REG4);//if(chunkptr == chunk_end)
    VoC_push16(REG2,DEFAULT);
    //////dmai///////////////////////
    VoC_lw16i(REG2,256);//size[int]
    VoC_push16(RA,DEFAULT);
    VoC_lw16i(REG3,PUT_BITS_BUF/2);//ADDR-[int]
    VoC_jal(DMAI_WRITE);
    VoC_pop16(RA,DEFAULT);
    //////dmae//////////////////////
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_add32_rd(REG23,REG23,GLOBAL_VIDEO_OUT_LEN);//byte

    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_pop16(REG2,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_sw32_d(REG23,GLOBAL_VIDEO_OUT_LEN);
    exit_on_warnings = ON;
LABLE_CHANGE_10:
    VoC_sw16_d(REG3,GLOBAL_CURR_POS_ADDR);

    VoC_bne16_rd(LABLE_FLUSH_WHILE_END2,REG6,GLOBAL_SHORT_1_ADDR);
    VoC_pop16(REG7,DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG7,GLOBAL_pp_ADDR);//=1
LABLE_FLUSH_WHILE_END2:


    //flush  ff d9

    VoC_lw16i(REG6,0xd9ff);
    VoC_lw16d_set_inc(REG3,GLOBAL_CURR_POS_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG2,IN_PARALLEL);

    VoC_be16_rd(LABLE_WRITE_ODD,REG4,GLOBAL_SHORT_1_ADDR);

    VoC_lw16_d(REG5,GLOBAL_pp_ADDR);
    VoC_or16_ri(REG5,0xff00);
    VoC_lw16i(REG6,0xd9);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);

LABLE_WRITE_ODD:

    VoC_sw16inc_p(REG6,REG3,DEFAULT);//_jpeg_putw(CHUNK_EOI);
    VoC_sub16_rr(REG3,REG3,REG2,IN_PARALLEL);

    /////VoC_jal(DMAI_WRITE);////////////////////////
    VoC_lw16i(REG5,PUT_BITS_BUF/2);
    VoC_shru16_ri(REG3,-1,DEFAULT);//byte  size

    VoC_movreg16(ACC0_LO,REG3,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);

    VoC_lw16i_short(REG7,4,DEFAULT);//[kind]
    VoC_lw16i_short(REG4,15,IN_PARALLEL);
    VoC_add32_rd(ACC0,ACC0,GLOBAL_VIDEO_OUT_LEN);//r6 is the total byte of encode data

    VoC_add16_rr(REG4,REG3,REG4,DEFAULT);//VoC_lw16i(REG4,256);

    VoC_sw32_d(ACC0,GLOBAL_VIDEO_OUT_LEN);

    VoC_shr16_ri(REG4,4,DEFAULT);
    VoC_shr16_ri(REG4,-2,DEFAULT);
    VoC_lw32_d(ACC1,GLOBAL_OUT_VIDEO_BUF_START);//BYTE

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);


    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_pop16(REG2,DEFAULT);

    VoC_return;

}


/////////////////////////////////////////////////////////
void  DMAI_WRITE(void)
{
    VoC_lw32_d(ACC1,GLOBAL_OUT_VIDEO_BUF_START);//BYTE

    VoC_sw32_d(REG23,CFG_DMA_SIZE);

    VoC_shru16_ri(REG2,-2,DEFAULT);//->byte
    VoC_lw16i_short(REG3,0,IN_PARALLEL);//

    VoC_add32_rr(ACC1,ACC1,REG23,DEFAULT);//VoC_lw16i(ACC1_HI,0x4000
    exit_on_warnings = OFF;
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);
    exit_on_warnings = ON;
    VoC_sw32_d(ACC1,GLOBAL_OUT_VIDEO_BUF_START);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_return;

}


//////////////////JPEG YUV PROCESS/////////////////////////////
//  VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y_ADDR,1);
//  VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);
//  VoC_push32(REG01,DEFAULT);//r0: rgb addr   r1:height loop
//  VoC_push16(REG5,IN_PARALLEL);//width loop

void Convert_YUV422_Block(void)
{
    //r0:inbuf inc0=2
    //r1:y1  r2:y2  r3:u

    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);

    VoC_movreg16(ACC0_LO,REG0,DEFAULT);
    VoC_lw16i_short(REG4,64,IN_PARALLEL);
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);//
    VoC_movreg16(REG5,REG4,DEFAULT);//r5 = 64
    VoC_shru16_ri(REG4,-1,IN_PARALLEL);//r4 = 128


    VoC_loop_i(1,8);//for (j=0;j<8;j++)
    //pyuv = (short*)buff + j*width;

    VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i+=2)
    VoC_startloop0
    VoC_lbinc_p(REG0);//read a short ->rl6// temp = *pyuv++;
    //*py1++ = (temp&0xff)-128;
    //*pcb++ = ((temp>>8)&0xff)-128;
    VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6

    VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//y

    VoC_lbinc_p(REG0);//temp = *pyuv++;

    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);//u
    VoC_sw16inc_p(REG6,REG1,IN_PARALLEL);

    VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6
    VoC_sw16_p(REG7,REG3,DEFAULT);//*pcr++ = ((temp>>8)&0xff)-128;


    VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//*py1++ = (temp&0xff)-128;

    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_add16_rr(REG6,REG3,REG5,IN_PARALLEL);
    VoC_sw16inc_p(REG6,REG1,DEFAULT);//y
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_sw16inc_p(REG7,REG6,DEFAULT);//v

    VoC_endloop0

    VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i+=2)
    VoC_startloop0
    VoC_lbinc_p(REG0);//read a short ->rl6// temp = *pyuv++;
    //*py1++ = (temp&0xff)-128;
    //*pcb++ = ((temp>>8)&0xff)-128;
    VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6
    VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);//y
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);//u

    VoC_lbinc_p(REG0);//temp = *pyuv++;


    VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6
    VoC_sw16inc_p(REG6,REG2,IN_PARALLEL);

    VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6
    VoC_sw16_p(REG7,REG3,IN_PARALLEL);//*pcr++ = ((temp>>8)&0xff)-128;


    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//*py1++ = (temp&0xff)-128;

    VoC_add16_rr(REG6,REG3,REG5,DEFAULT);
    VoC_sw16inc_p(REG6,REG2,DEFAULT);//y
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_sw16inc_p(REG7,REG6,DEFAULT);//v

    VoC_endloop0

    VoC_movreg16(REG0,ACC0_LO,DEFAULT);
    VoC_add16_rd(REG0,REG0,GLOBAL_VIDEO_WIDTH);
    VoC_movreg16(ACC0_LO,REG0,DEFAULT);

    VoC_endloop1

    VoC_return;
}

void  COOLSAND_YUVJPEG422(void)
{
    VoC_lw32_d(REG45,GLOBAL_VIDEO_WIDTH);//w h
    VoC_push16(RA,DEFAULT);
    VoC_movreg32(REG67,REG45,IN_PARALLEL);
    VoC_shr16_ri(REG4,-4,DEFAULT);//w*32 byte
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_and16_ri(REG6,15);
    VoC_and16_ri(REG7,7);
    VoC_sw16_d(REG5,GLOBAL_HEIGHT_SAVE_ADDR);//
    VoC_sw32_d(REG45,CONST_BLOCKSIZE_BYTE_ADDR);
    VoC_lw32_d(REG45,GLOBAL_VIDEO_WIDTH);//w h
    VoC_bez16_r(LABLE_WIDTH_MULTIPLE16_YUV422,REG6);
    VoC_lw16i_short(REG6,15,DEFAULT);
    VoC_lw16i_short(REG0,1,IN_PARALLEL);

    VoC_sw16_d(REG4,GLOBAL_WIDTH_SAVE_ADDR);//original width;
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_and16_ri(REG4,0xfff0);

    VoC_sw16_d(REG0,GLOBAL_HEIGHT_SAVE_ADDR);//whether the width has changed;
    VoC_sw16_d(REG4,GLOBAL_VIDEO_WIDTH);
LABLE_WIDTH_MULTIPLE16_YUV422:
    VoC_bez16_r(LABLE_WIDTH_HEIGHT_MULTIPLE16_YUV422,REG7);
    VoC_add16_rd(REG5,REG5,CONST_7_ADDR);
    VoC_and16_ri(REG5,0xfff8);
    VoC_NOP();
    VoC_sw16_d(REG5,GLOBAL_VIDEO_HEIGHT);
LABLE_WIDTH_HEIGHT_MULTIPLE16_YUV422:
    VoC_lw16i_short(REG1,0,DEFAULT);

LABLE_REG2JPEG_LOOP_HEIGHT_YUV422:
    VoC_lw16i_short(REG0,-3,DEFAULT);//8=2**3
    VoC_jal(DEMAI_READ_RGBDATA);
    VoC_lw16i_set_inc(REG0,VPP_AMJR_JPEG_RGB_STRUCT,1);
    VoC_lw16i_short(REG5,0,DEFAULT);
LABLE_REG2JPEG_LOOP_WIDTH_YUV422:
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);


    VoC_push32(REG01,DEFAULT);//r0: rgb addr   r1:height loop
    VoC_push16(REG5,IN_PARALLEL);//width loop
    VoC_jal(Convert_YUV422_Block);


    //ENOCDE BLOCK
    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);


    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);

    ////////////////////////////////
    VoC_pop32(REG01,DEFAULT);
    VoC_pop16(REG5,IN_PARALLEL);
    VoC_lw16i_short(REG4,16,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);
    /////////////////////////////////
    VoC_bnlt16_rd(LABLE_CON1_YUV422,REG5,GLOBAL_VIDEO_WIDTH);
    VoC_jump(LABLE_REG2JPEG_LOOP_WIDTH_YUV422);
LABLE_CON1_YUV422:
    VoC_lw16i_short(REG4,8,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//height loop
    VoC_bnlt16_rd(LABLE_CON2_YUV422,REG1,GLOBAL_VIDEO_HEIGHT);
    VoC_jump(LABLE_REG2JPEG_LOOP_HEIGHT_YUV422);
LABLE_CON2_YUV422:
    ////////////////////////////////////////////////////////////////////
    VoC_jal(COOLSNAD_flush_bits);
    //whether there is  odd byte left
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}

void Convert_YUV411_Block(void)
{
    //r0:inbuf inc0=2
    //r1:y1  r2:y2  r3:u

    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);

    VoC_movreg16(ACC0_LO,REG0,DEFAULT);
    VoC_lw16i_short(REG4,64,IN_PARALLEL);
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw16i_short(REG6,2,IN_PARALLEL);
    VoC_movreg16(REG5,REG4,DEFAULT);//r5 = 64
    VoC_shru16_ri(REG4,-1,IN_PARALLEL);//r4 = 128



LABLE_Convert_YUV411_L1:
    VoC_push16(REG6,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);//

    VoC_loop_i(1,4);//for (j=0;j<4;j++)

    //even row////////////////////////////////////////
    VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i+=2)
    VoC_startloop0
    VoC_lbinc_p(REG0);//read a short ->rl6// temp = *pyuv++;
    //*py1++ = (temp&0xff)-128;
    //*pcb++ = ((temp>>8)&0xff)-128;
    VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6

    VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//y

    VoC_lbinc_p(REG0);//temp = *pyuv++;

    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);//u
    VoC_sw16inc_p(REG6,REG1,IN_PARALLEL);

    VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6
    VoC_sw16_p(REG7,REG3,DEFAULT);//*pcr++ = ((temp>>8)&0xff)-128;


    VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//*py1++ = (temp&0xff)-128;

    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_add16_rr(REG6,REG3,REG5,IN_PARALLEL);
    VoC_sw16inc_p(REG6,REG1,DEFAULT);//y
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_sw16inc_p(REG7,REG6,DEFAULT);//v

    VoC_endloop0

    VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i+=2)
    VoC_startloop0
    VoC_lbinc_p(REG0);//read a short ->rl6// temp = *pyuv++;
    //*py1++ = (temp&0xff)-128;
    //*pcb++ = ((temp>>8)&0xff)-128;
    VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6
    VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);//y
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);//u

    VoC_lbinc_p(REG0);//temp = *pyuv++;


    VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6
    VoC_sw16inc_p(REG6,REG2,IN_PARALLEL);

    VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6
    VoC_sw16_p(REG7,REG3,IN_PARALLEL);//*pcr++ = ((temp>>8)&0xff)-128;


    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//*py1++ = (temp&0xff)-128;

    VoC_add16_rr(REG6,REG3,REG5,DEFAULT);
    VoC_sw16inc_p(REG6,REG2,DEFAULT);//y
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_sw16inc_p(REG7,REG6,DEFAULT);//v

    VoC_endloop0

    VoC_movreg16(REG0,ACC0_LO,DEFAULT);
    VoC_add16_rd(REG0,REG0,GLOBAL_VIDEO_WIDTH);//pyuv = (short*)buff + j*width;
    VoC_movreg16(ACC0_LO,REG0,DEFAULT);
    ///////odd row//////////////////////////////////
    VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i+=2)
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
    VoC_startloop0


    VoC_shru16_ri(REG6,-8,DEFAULT);
    VoC_lw16inc_p(REG7,REG0,IN_PARALLEL);

    VoC_shru16_ri(REG6,8,DEFAULT);
    VoC_shru16_ri(REG7,-8,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_shru16_ri(REG7,8,IN_PARALLEL);


    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0

    VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i+=2)
    VoC_startloop0

    VoC_shru16_ri(REG6,-8,DEFAULT);
    VoC_lw16inc_p(REG7,REG0,IN_PARALLEL);

    VoC_shru16_ri(REG6,8,DEFAULT);
    VoC_shru16_ri(REG7,-8,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_shru16_ri(REG7,8,IN_PARALLEL);


    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG2,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,DEFAULT);
    VoC_endloop0

    VoC_movreg16(REG0,ACC0_LO,DEFAULT);
    VoC_add16_rd(REG0,REG0,GLOBAL_VIDEO_WIDTH);//pyuv = (short*)buff + j*width;
    VoC_movreg16(ACC0_LO,REG0,DEFAULT);
    VoC_endloop1

    VoC_pop16(REG6,DEFAULT);
    VoC_sub16_rd(REG6,REG6,CONST_1_ADDR);
    VoC_bgt16_rd(LABLE_Convert_YUV411_L2,REG6,CONST_0_ADDR);
    VoC_return;
LABLE_Convert_YUV411_L2:
    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_Y2_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y3_ADDR,1);
    VoC_jump(LABLE_Convert_YUV411_L1);

}



void  COOLSAND_YUV2JPEG411(void)
{

    VoC_lw32_d(REG45,GLOBAL_VIDEO_WIDTH);//w h
    VoC_push16(RA,DEFAULT);
    VoC_movreg32(REG67,REG45,IN_PARALLEL);
    VoC_shr16_ri(REG4,-5,DEFAULT);//w*32 byte
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_and16_ri(REG6,15);
    VoC_and16_ri(REG7,15);
    VoC_sw16_d(REG5,GLOBAL_HEIGHT_SAVE_ADDR);//
    VoC_sw32_d(REG45,CONST_BLOCKSIZE_BYTE_ADDR);
    VoC_lw32_d(REG45,GLOBAL_VIDEO_WIDTH);//w h
    VoC_bez16_r(LABLE_WIDTH_MULTIPLE16_YUV,REG6);
    VoC_lw16i_short(REG6,15,DEFAULT);
    VoC_lw16i_short(REG0,1,IN_PARALLEL);

    VoC_sw16_d(REG4,GLOBAL_WIDTH_SAVE_ADDR);//original width;
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_and16_ri(REG4,0xfff0);

    VoC_sw16_d(REG0,GLOBAL_HEIGHT_SAVE_ADDR);//whether the width has changed;
    VoC_sw16_d(REG4,GLOBAL_VIDEO_WIDTH);
LABLE_WIDTH_MULTIPLE16_YUV:
    VoC_bez16_r(LABLE_WIDTH_HEIGHT_MULTIPLE16_YUV,REG7);
    VoC_add16_rd(REG5,REG5,CONST_15_ADDR);
    VoC_and16_ri(REG5,0xfff0);
    VoC_NOP();
    VoC_sw16_d(REG5,GLOBAL_VIDEO_HEIGHT);
LABLE_WIDTH_HEIGHT_MULTIPLE16_YUV:

    VoC_lw16i_short(REG1,0,DEFAULT);

    ///////////////////////////////////////////////////
LABLE_REG2JPEG_LOOP_HEIGHT_411_YUV:
    VoC_lw16i_short(REG0,-4,DEFAULT);//16=2**4
    VoC_jal(DEMAI_READ_RGBDATA);
    VoC_lw16i_set_inc(REG0,VPP_AMJR_JPEG_RGB_STRUCT,1);
    VoC_lw16i_short(REG5,0,DEFAULT);
LABLE_REG2JPEG_LOOP_WIDTH_411_YUV:
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_push32(REG01,DEFAULT);//r0: rgb addr   r1:height loop
    VoC_push16(REG5,IN_PARALLEL);//width loop
    VoC_jal(Convert_YUV411_Block);


    //ENOCDE BLOCK
    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y2_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y3_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);


    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);

    ////////////////////////////////
    VoC_pop32(REG01,DEFAULT);
    VoC_pop16(REG5,IN_PARALLEL);
    VoC_lw16i_short(REG4,16,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);
    /////////////////////////////////
    VoC_bnlt16_rd(LABLE_CON1_411_YUV,REG5,GLOBAL_VIDEO_WIDTH);
    VoC_jump(LABLE_REG2JPEG_LOOP_WIDTH_411_YUV);
LABLE_CON1_411_YUV:
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//height loop
    VoC_bnlt16_rd(LABLE_CON2_411_YUV,REG1,GLOBAL_VIDEO_HEIGHT);
    VoC_jump(LABLE_REG2JPEG_LOOP_HEIGHT_411_YUV);
LABLE_CON2_411_YUV:
    ////////////////////////////////////////////////////////////////////
    VoC_jal(COOLSNAD_flush_bits);
    //whether there is  odd byte left
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}

///////////////////1280*1024  yuv422/////////////////////////////////////////////////////
void COOLSAND_Double2Single_YUV(void)
{
    //r1:pin    r0:pout

    //y1 = ((*pin)&0xff);//0
    //u  = (((*pin++)>>8)&0xff);
    //y2 = ((*pin)&0xff);//1
    //v  = (((*pin++)>>8)&0xff);
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);

    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG4,8,DEFAULT);//y1
    VoC_rbinc_i(REG5,8,DEFAULT);//u

    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG6,8,DEFAULT);//y2
    VoC_rbinc_i(REG7,8,DEFAULT);//v
    //wid = wid>>1;
    VoC_loop_i_short(4,DEFAULT);//for (i=wid;i>0;i--)
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG0,DEFAULT);//*pout++ = y1;
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);//y1+y2
    VoC_sw16inc_p(REG5,REG0,DEFAULT);//*pout++ = u;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);//(y1+y2)>>1

    VoC_lbinc_p(REG1);//get next 16-bit data
    VoC_sw16inc_p(REG4,REG0,DEFAULT);//*pout++ =(y1+y2)>>1;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);//*pout++ = v;
    VoC_movreg16(REG3,REG5,IN_PARALLEL);//save u

    VoC_rbinc_i(REG4,8,DEFAULT);//y1 = ((*pin)&0xff);//2
    VoC_rbinc_i(REG5,8,DEFAULT);//u1 = (((*pin++)>>8)&0xff);
    VoC_sw16inc_p(REG6,REG0,IN_PARALLEL);//*pout++ = y2;


    VoC_lbinc_p(REG1);//get next 16-bit data
    VoC_rbinc_i(REG4,8,DEFAULT);//y22 = ((*pin++)&0xff);
    VoC_movreg32(ACC1,REG45,IN_PARALLEL);//y1 u1

    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);//(u+u1)
    VoC_movreg16(REG5,ACC1_LO,IN_PARALLEL);//y1

    VoC_shru16_ri(REG3,1,DEFAULT);//(u+u1)>>1
    VoC_add16_rr(REG2,REG5,REG6,IN_PARALLEL);//(y1+y2)

    VoC_rbinc_i(REG5,8,DEFAULT);//v1
    VoC_shru16_ri(REG2,1,IN_PARALLEL);//(y1+y2)>>1

    VoC_sw16inc_p(REG3,REG0,DEFAULT);//*pout++ = (u+u1)>>1;
    VoC_add16_rr(REG7,REG5,REG7,IN_PARALLEL);//(v+v1)

    VoC_shru16_ri(REG7,1,DEFAULT);//(v+v1)>>1
    VoC_sw16inc_p(REG2,REG0,IN_PARALLEL);//*pout++ =(y1+y2)>>1;

    VoC_movreg32(REG67,REG45,DEFAULT);
    VoC_movreg32(REG45,ACC1,IN_PARALLEL);

    VoC_sw16inc_p(REG7,REG0,DEFAULT);//*pout++ = (v+v1)>>1;


    VoC_endloop0

    VoC_return;
}

void COOLSAND_Double2SingleLastRow_YUV(void)
{
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);

    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG4,8,DEFAULT);//y1
    VoC_rbinc_i(REG5,8,DEFAULT);//u

    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG6,8,DEFAULT);//y2
    VoC_rbinc_i(REG7,8,DEFAULT);//v
    //wid = wid>>1;
    VoC_loop_i_short(3,DEFAULT);//for (i=wid;i>0;i--)
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG0,DEFAULT);//*pout++ = y1;
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);//y1+y2
    VoC_sw16inc_p(REG5,REG0,DEFAULT);//*pout++ = u;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);//(y1+y2)>>1

    VoC_lbinc_p(REG1);//get next 16-bit data
    VoC_sw16inc_p(REG4,REG0,DEFAULT);//*pout++ =(y1+y2)>>1;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);//*pout++ = v;
    VoC_movreg16(REG3,REG5,IN_PARALLEL);//save u

    VoC_rbinc_i(REG4,8,DEFAULT);//y1 = ((*pin)&0xff);//2
    VoC_rbinc_i(REG5,8,DEFAULT);//u1 = (((*pin++)>>8)&0xff);
    VoC_sw16inc_p(REG6,REG0,IN_PARALLEL);//*pout++ = y2;


    VoC_lbinc_p(REG1);//get next 16-bit data
    VoC_rbinc_i(REG4,8,DEFAULT);//y22 = ((*pin++)&0xff);
    VoC_movreg32(ACC1,REG45,IN_PARALLEL);//y1 u1

    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);//(u+u1)
    VoC_movreg16(REG5,ACC1_LO,IN_PARALLEL);//y1

    VoC_shru16_ri(REG3,1,DEFAULT);//(u+u1)>>1
    VoC_add16_rr(REG2,REG5,REG6,IN_PARALLEL);//(y1+y2)

    VoC_rbinc_i(REG5,8,DEFAULT);//v1
    VoC_shru16_ri(REG2,1,IN_PARALLEL);//(y1+y2)>>1

    VoC_sw16inc_p(REG3,REG0,DEFAULT);//*pout++ = (u+u1)>>1;
    VoC_add16_rr(REG7,REG5,REG7,IN_PARALLEL);//(v+v1)

    VoC_shru16_ri(REG7,1,DEFAULT);//(v+v1)>>1
    VoC_sw16inc_p(REG2,REG0,IN_PARALLEL);//*pout++ =(y1+y2)>>1;

    VoC_movreg32(REG67,REG45,DEFAULT);
    VoC_movreg32(REG45,ACC1,IN_PARALLEL);

    VoC_sw16inc_p(REG7,REG0,DEFAULT);//*pout++ = (v+v1)>>1;

    VoC_endloop0

    VoC_sw16inc_p(REG4,REG0,DEFAULT);//*pout++ = y1;
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);//y1+y2
    VoC_sw16inc_p(REG5,REG0,DEFAULT);//*pout++ = u;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);//(y1+y2)>>1

    VoC_NOP();
    VoC_sw16inc_p(REG4,REG0,DEFAULT);//*pout++ =(y1+y2)>>1;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);//*pout++ = v;

    VoC_sw16inc_p(REG6,REG0,DEFAULT);//*pout++ = y2;
    VoC_sw16inc_p(REG5,REG0,DEFAULT);//*pout++ = u;
    VoC_sw16inc_p(REG6,REG0,DEFAULT);//*pout++ = y2;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);//*pout++ = V;

    VoC_return;
}
void COOLSAND_Double2Row_YUV(void)
{
    //(R2 + R3)/2 -> R0
    VoC_lw32inc_p(REG45,REG2,DEFAULT);

    VoC_loop_i_short(16,DEFAULT);
    VoC_lw32inc_p(REG67,REG3,IN_PARALLEL);
    VoC_startloop0

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG7,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_lw32inc_p(REG67,REG3,IN_PARALLEL);

    VoC_shru16_ri(REG5,1,DEFAULT);

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_sw32inc_p(REG45,REG0,DEFAULT);


    VoC_endloop0

    VoC_return;
}

void COOLSAN_Encode422MCUBlock_YUV(void)
{
    //r0:rgb buf(Y)  r1:Y(X)   r2:Y1(X)   r3:u v(X)
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i(REG5,128);



    VoC_loop_i(1,8);

    VoC_loop_i(0,4);

    VoC_lw32inc_p(REG67,REG0,DEFAULT);

    VoC_sub16_rr(REG6,REG6,REG5,DEFAULT);//y11 = (*ptemp++)-128;
    VoC_sub16_rr(REG7,REG7,REG5,DEFAULT);//u =  (*ptemp++)-128;

    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_movreg16(ACC0_LO,REG7,IN_PARALLEL);
    VoC_sw16inc_p(REG6,REG1,DEFAULT);//*y1_ptr++ = y11;
    VoC_sub16_rr(REG6,REG6,REG5,IN_PARALLEL);//y22 = (*ptemp++)-128;

    VoC_sw16_p(ACC0_LO,REG3,DEFAULT);
    VoC_lw16i_short(REG4,64,IN_PARALLEL);//*cb_ptr++ = u;

    VoC_sub16_rr(REG7,REG7,REG5,DEFAULT);//v =  (*ptemp++)-128;
    VoC_add16_rr(REG4,REG4,REG3,IN_PARALLEL);//v addr

    VoC_sw16inc_p(REG6,REG1,DEFAULT);//*y1_ptr++ = y22;
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_sw16inc_p(REG7,REG4,DEFAULT);//*cr_ptr++ = v;

    VoC_endloop0
    ///////////////////////////////////////////////////////

    VoC_loop_i(0,4);

    VoC_lw32inc_p(REG67,REG0,DEFAULT);

    VoC_sub16_rr(REG6,REG6,REG5,DEFAULT);//y11 = (*ptemp++)-128;
    VoC_sub16_rr(REG7,REG7,REG5,DEFAULT);//u =  (*ptemp++)-128;

    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_movreg16(ACC0_LO,REG7,IN_PARALLEL);
    VoC_sw16inc_p(REG6,REG2,DEFAULT);//*y1_ptr++ = y11;
    VoC_sub16_rr(REG6,REG6,REG5,IN_PARALLEL);//y22 = (*ptemp++)-128;

    VoC_sw16_p(ACC0_LO,REG3,DEFAULT);
    VoC_lw16i_short(REG4,64,IN_PARALLEL);//*cb_ptr++ = u;

    VoC_sub16_rr(REG7,REG7,REG5,DEFAULT);//v =  (*ptemp++)-128;
    VoC_add16_rr(REG4,REG4,REG3,IN_PARALLEL);//v addr

    VoC_sw16inc_p(REG6,REG2,DEFAULT);//*y1_ptr++ = y22;
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_sw16inc_p(REG7,REG4,DEFAULT);//*cr_ptr++ = v;
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1


    //ENOCDE BLOCK
    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);


    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);
    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

    VoC_return;


}
void COOLSAND_MEMCopy_YUV(void)
{
    //r2:in   r3:out
    //used  buf: r2 r3 RL6 rl7
    VoC_loop_i_short(8,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(RL6,REG2,DEFAULT);
    VoC_lw32inc_p(RL7,REG2,DEFAULT);
    VoC_sw32inc_p(RL6,REG3,DEFAULT);
    VoC_sw32inc_p(RL7,REG3,DEFAULT);
    VoC_endloop0

    VoC_return;

}
void COOLSAND_1280BY1024_YUV422_JPEG(void)
{
//#if 0
//  voc_short TEMP_BUFF_FOR1280BY1024_TMP1[48]  ,  y
//  voc_short TEMP_BUFF_FOR1280BY1024_TMP2[48]  ,  y
//#endif

    VoC_lw16i_short(REG5,7,DEFAULT);//line2
    VoC_lw16i_short(REG4,0,IN_PARALLEL);//line1

    VoC_push16(RA,DEFAULT);
LABLE_START_1280BY1024_YUV:
    VoC_push32(REG45,DEFAULT);//line2  line1
    //even-16   first-8
    VoC_lw16i_short(REG0,79,IN_PARALLEL);//count=79;

    VoC_lw16i_set_inc(REG1,VPP_AMJR_JPEG_RGB_STRUCT,1);//pin_addr
    VoC_jal(COOLSAND_DMAIRead_1280BY1024Even);
LABLE_DO_BEGIN_FRONT_EVEN_YUV:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);//r0:pout
    VoC_jal(COOLSAND_Double2Single_YUV);//0


    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//int
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy_YUV);      //1

    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);//3


    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);//5

    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_7_ADDR);
    VoC_jal(COOLSAND_Double2Single_YUV);//7

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_3_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_3_ADDR,2);//4
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_5_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_5_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_7_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);


    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);


    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW1_YUV,REG0);
    VoC_jump(LABLE_DO_BEGIN_FRONT_EVEN_YUV);
LABLE_LASTROW1_YUV:
    //last row
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);//r2:pout
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);//0


    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//in
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy_YUV);              //1

    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);//3


    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);//5

    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_7_ADDR);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);//7

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_3_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_3_ADDR,2);//4
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_5_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_5_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_7_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);


    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);
    ////evev-16  second-8////////////////////
    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_RGB_640_3_ADDR,1);
    VoC_lw16i(REG0,79);//count=79;

LABLE_DO_BEGIN_BEHIND_EVEN_YUV:
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    exit_on_warnings = OFF;
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);
    exit_on_warnings = ON;
    //奇数行
    VoC_jal(COOLSAND_Double2Single_YUV);


    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_1_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*5,pout+48*3);//11
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*6,pout+48*5);//13
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*7,pout+48*7);//15
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_7_ADDR);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //偶数行
    //  double2row(16,bufftmp,pout+48,pout);//8
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_1_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48,pout+48*3,pout+48*2);//10
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_3_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*3,pout+48*5,pout+48*4);//12
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_3_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_5_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*5,pout+48*7,pout+48*6);//14
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_5_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_7_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);

    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW2_YUV,REG0);
    VoC_jump(LABLE_DO_BEGIN_BEHIND_EVEN_YUV);
LABLE_LASTROW2_YUV:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    //奇数行
    //double2single(8,pin+640*3,bufftmp);//7
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //double2single(8,pin+640*4,pout+48);//9
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_1_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*5,pout+48*3);//11
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*6,pout+48*5);//13
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*7,pout+48*7);//15
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_7_ADDR);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);
    //偶数行
    //  double2row(16,bufftmp,pout+48,pout);//8
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_1_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48,pout+48*3,pout+48*2);//10
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_3_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*3,pout+48*5,pout+48*4);//12
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_3_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_5_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*5,pout+48*7,pout+48*6);//14
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_5_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_7_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);

    //ODD-16 FIRST-8
    VoC_lw32_sd(REG45,0,DEFAULT);//LINE2 LINE1
    VoC_lw16i_short(REG6,7,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);//      line1+=7;
    VoC_add16_rr(REG5,REG5,REG6,IN_PARALLEL);//  line2+=7;
    VoC_jal(COOLSAND_DMAIRead_1280BY1024Odd);
    VoC_sw32_sd(REG45,0,DEFAULT);//save line2 line1
    VoC_lw16i_short(REG0,79,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,VPP_AMJR_JPEG_RGB_STRUCT,1);
LABLE_DO_BEGIN_FRONT_ODD_YUV:
    //  double2single(8,pin,bufftmp);   //-1
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    exit_on_warnings = OFF;
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);
    exit_on_warnings = ON;
    VoC_jal(COOLSAND_Double2Single_YUV);

    //double2single(8,pin+640,pout+48*2);  //2
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*2,pout+48*4);//4
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);


    //  double2single(8,pin+640*3,pout+48*6);//6
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*4,&bufftmp[48]);//8
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP2,1);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2row(16,bufftmp,pout+48*2,pout);//0
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_2_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);


    // memcpy(pout+48,pout,48*2);            //1
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//in
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy_YUV);

    //奇数行
    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//3
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_4_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//5
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_6_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*6,&bufftmp[48],pout+48*7);//7
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP2,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);

    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW3_YUV,REG0);
    VoC_jump(LABLE_DO_BEGIN_FRONT_ODD_YUV);
LABLE_LASTROW3_YUV:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count

    //  double2single(8,pin,bufftmp);   //-1
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //double2single(8,pin+640,pout+48*2);  //2
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*2,pout+48*4);//4
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);


    //  double2single(8,pin+640*3,pout+48*6);//6
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*4,&bufftmp[48]);//8
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP2,1);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2row(16,bufftmp,pout+48*2,pout);//0
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_2_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);


    // memcpy(pout+48,pout,48*2);            //1
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//in
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy_YUV);

    //奇数行
    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//3
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_4_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//5
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_6_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*6,&bufftmp[48],pout+48*7);//7
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP2,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);

    ////evev-16  second-8////////////////////
    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_RGB_640_4_ADDR,1);
    VoC_lw16i(REG0,79);//count=79;
LABLE_DO_BEGIN_BEHIND_ODD_YUV:
    //  double2single(8,pin+640*4,pout);//8
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);
    exit_on_warnings = OFF;
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);
    exit_on_warnings = ON;
    VoC_jal(COOLSAND_Double2Single_YUV);

    //double2single(8,pin+640*5,pout+48*2);//10
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*6,pout+48*4);//12
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*7,pout+48*6);//14
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*8,bufftmp);//-2
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2Single_YUV);


    //奇数行
    //  double2row(16,pout,pout+48*2,pout+48*1);//9
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_2_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_1_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//11
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_4_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//13
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_6_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*6,bufftmp,pout+48*7);//15
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP1,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);

    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW4_YUV,REG0);
    VoC_jump(LABLE_DO_BEGIN_BEHIND_ODD_YUV);
LABLE_LASTROW4_YUV:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count

    //  double2single(8,pin+640*4,pout);//8
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //double2single(8,pin+640*5,pout+48*2);//10
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*6,pout+48*4);//12
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*7,pout+48*6);//14
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*8,bufftmp);//-2
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);


    //奇数行
    //  double2row(16,pout,pout+48*2,pout+48*1);//9
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_2_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_1_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//11
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_4_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//13
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_6_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*6,bufftmp,pout+48*7);//15
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP1,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);

    VoC_pop32(REG45,DEFAULT);
    VoC_lw16i_short(REG6,8,IN_PARALLEL);
    //update line1 and line2
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG6,IN_PARALLEL);

    VoC_bgt16_rd(LABLE_END_1280BY1024_YUV,REG5,CONST_479_ADDR);
    VoC_jump(LABLE_START_1280BY1024_YUV);
LABLE_END_1280BY1024_YUV:
    ////////////////////////////////////////////////////////////////////
    VoC_jal(COOLSNAD_flush_bits);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}


// **************************************************************************************
//   Function:    vpp_AmjrGenerateJpegEncHuffTable
//
//   Description: generate the jpeg encoder huffman table
//
//   Inputs:
//
//   Outputs:
//
//   Used :       REG01234567
//
//   Version 1.0  Create by  Yordan  07/10/2008
//
//   *************************************************************************************

void vpp_AmjrGenerateJpegEncHuffTable(void)
{
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

    VoC_lw16i_set_inc(REG0,CONST_MYHUFF_AC0_ADDR,2);
    VoC_lw16i_set_inc(REG1,CONST_MYHUFF_AC_COMPRESSED,1);
    VoC_lw16i_set_inc(REG2,324,-1);
    VoC_lw16i_set_inc(REG3,2,-1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_movreg16(REG5,REG2,IN_PARALLEL);
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);

    VoC_jal(vpp_AmjrDecompressVssAdpcmTable);

    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_set_inc(REG0,CONST_MYHUFF_AC0_ADDR,2);

    VoC_loop_i(1,2)

    VoC_loop_i(0,162)
    VoC_lw32_p(REG45,REG0,DEFAULT);
    VoC_shru32_ri(REG45,-8,DEFAULT);
    VoC_shru16_ri(REG4,8,DEFAULT);
    VoC_NOP();
    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_endloop0
    VoC_NOP();

    VoC_endloop1

    VoC_return;

}




