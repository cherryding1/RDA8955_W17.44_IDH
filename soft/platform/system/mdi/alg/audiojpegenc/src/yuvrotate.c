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


// VoC_directive: voc_stack16
#define SP_16_ADDR                                      RAM_X_MAX - 1 // measured minimum : 0x1000 - 0x27
// VoC_directive: voc_stack32
#define SP_32_ADDR                                      RAM_Y_MAX - 2 // measured minimum : 0x1800 - 0x19

#if 0
/* ram_x */
voc_word YUV_ROTATE_X_INPUT_ADDR ,x
voc_word YUV_ROTATE_X_OUTPUT_YADDR, x
voc_word YUV_ROTATE_X_OUTPUT_UADDR, x
voc_word YUV_ROTATE_X_OUTPUT_VADDR, x


voc_short YUV_ROTATE_X_Srcwidth ,x
voc_short YUV_ROTATE_X_Srcheight ,x
voc_short YUV_ROTATE_X_Dstwidth ,x
voc_short YUV_ROTATE_X_Dstheight,x
voc_short YUV_ROTATE_X_Direction ,x
voc_short YUV_ROTATE_X_Stride  ,x
voc_short YUV_ROTATE_X_xpos ,x
voc_short YUV_ROTATE_X_ypos ,x
voc_short YUV_ROTATE_X_mb_width ,x
voc_short YUV_ROTATE_X_mb_height ,x
voc_short YUV_ROTATE_X_MBA, x
voc_short YUV_ROTATE_X_MBAmax, x


voc_short YUV_ROTATE_X_BufY[64] ,x    // store rotate yuv420 Y8x8
voc_short YUV_ROTATE_X_reserved[16] ,x


//rgb rotate parameter
voc_struct GX_RGB_ROTATE_CFG_STRUCT  __attribute__((export)) ,x

voc_word RGB_ROTATE_X_INPUT_ADDR    ,x
voc_word RGB_ROTATE_X_OUTPUT_ADDR  ,x

voc_short RGB_ROTATE_X_srcwidth ,x
voc_short RGB_ROTATE_X_srcheight, x
voc_short RGB_ROTATE_X_dstwidth  ,x
voc_short RGB_ROTATE_X_dstheight ,x
voc_short RGB_ROTATE_X_direction ,x
voc_short RGB_ROTATE_X_stride   ,x
voc_short RGB_ROTATE_X_xpos ,x
voc_short RGB_ROTATE_X_ypos ,x
voc_short RGB_ROTATE_X_mb_width ,x
voc_short RGB_ROTATE_X_mb_height ,x
voc_short RGB_ROTATE_X_MBA ,x
voc_short RGB_ROTATE_X_MBAmax  ,x

voc_short RGB_ROTATE_X_out_buf[64] ,x
voc_short RGB_ROTATE_X_reserved[16] ,x

voc_struct_end




/* ram_y */
voc_short YUV_ROTATE_Y_IN_BUF[16*8*3],y // yuv422  2 8x8 block ,byte to unsigned short
voc_short YUV_ROTATE_Y_BufU[20] ,y    //store rotate yuv420 U4x4
voc_short YUV_ROTATE_Y_BufV[20] ,y   //store rotate yuv420 V4x4

voc_short YUV_ROTATE_Y_const_17, y
voc_short YUV_ROTATE_Y_const_65, y
voc_short YUV_ROTATE_Y_const_1 ,y
voc_short YUV_ROTATE_dma_in_flag ,y


voc_struct GY_RGB_ROTATE_CFG_STRUCT , y

voc_short RGB_ROTATE_Y_in_block[8*8*3] ,y

voc_short RGB_ROTATE_Y_dma_status ,y
voc_short RGB_ROTATE_Y_const_1  ,y

voc_struct_end







#endif

void YUV_ROTATE_SEND_DMA_CMD(void)
{
    VoC_or32_rr(ACC0,RL7,DEFAULT);
    VoC_sw16_d(REG0,CFG_DMA_2D_STRIDE);
    VoC_sw16_d(REG1,CFG_DMA_2D_LINES);
    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_return;
}

void YUV_ROTATE_WAIT_DMA_DONE(void)
{
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    VoC_return;
}

/**************************************************
note:
function: YUV422--->YUV420---->ROTATE 90 OR -90
input format:YUV422 ,1 pixel = 2bytes

input parameters:
src_width ,src_height : mutiple of 8  pixels
direction: 1--->flip right 90, -1----->flip left 90

output format: YUV420

***************************************************/
void VOC_YUV420_rotate(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);

    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,IN_PARALLEL);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,IN_PARALLEL);

    exit_on_warnings =ON;
    //initial
    VoC_lw32_d(REG01,YUV_ROTATE_X_Srcwidth);
    VoC_movreg32(REG67,REG01,DEFAULT);
    VoC_lw16i(REG2,17);
    VoC_lw16i(REG3,65);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    VoC_movreg16(REG0,REG1,DEFAULT);
    VoC_movreg16(REG1,REG0,IN_PARALLEL);
    VoC_shr16_ri(REG6,3,DEFAULT);
    VoC_shr16_ri(REG7,3,IN_PARALLEL);
    VoC_sw32_d(REG01,YUV_ROTATE_X_Dstwidth); //dst_width,dst_height
    VoC_sw32_d(REG67,YUV_ROTATE_X_mb_width); //mb_width,mb_height

    VoC_mult16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_sw32_d(RL7,YUV_ROTATE_X_xpos); //xpos=ypos=0;
    VoC_sw16_d(REG2,YUV_ROTATE_Y_const_17);
    VoC_sw16_d(REG3,YUV_ROTATE_Y_const_65);
    VoC_sw32_d(REG45,YUV_ROTATE_Y_const_1); //const_1 ,dma_in_flag =1
    VoC_sw16_d(RL7_LO,YUV_ROTATE_X_MBA);

    VoC_sw16_d(REG6,YUV_ROTATE_X_MBAmax);

    VoC_lw32_d(ACC0,YUV_ROTATE_X_INPUT_ADDR);
    VoC_lw16_d(REG0,YUV_ROTATE_X_Stride);
    VoC_lw16i(REG2,(YUV_ROTATE_Y_IN_BUF)/2); //dma local addr
    VoC_lw16i_short(REG1,8,DEFAULT); //dma lines
    VoC_shr16_ri(REG0,1,IN_PARALLEL);
    VoC_lw16i_short(REG3,4,DEFAULT); //dma size=2 (word)
    VoC_sub16_rr(REG0,REG0,REG3,DEFAULT); //dma stride =the remain size (word)
    VoC_lw16i_short(RL7_LO,0,IN_PARALLEL);
    VoC_lw16i(RL7_HI,(DMA_UNSIGNED_B2S>>16)); //dma read,byte to unsigned short
    VoC_jal(YUV_ROTATE_SEND_DMA_CMD);
    VoC_jal(YUV_ROTATE_WAIT_DMA_DONE);


YUV_ROTATE_loop_start:
    VoC_lw16_d(REG4,YUV_ROTATE_X_MBA);
    VoC_blt16_rd(YUV_ROTATE_MBA_lt_MBAmax,REG4,YUV_ROTATE_X_MBAmax);
    VoC_jump(YUV_ROTATE_end);

YUV_ROTATE_MBA_lt_MBAmax:
    VoC_lw16i_set_inc(REG1,0,1);
    VoC_lw16_d(REG0,YUV_ROTATE_X_MBA);
    VoC_lw16_d(REG3,YUV_ROTATE_X_mb_width);

    VoC_movreg16(REG5,REG0,DEFAULT);//MBA
    VoC_lw16i_short(FORMAT32,-4,IN_PARALLEL);
YUV_ROTATE_xypos_MBA_loop:
    //xpos = MBA % mb_width;
    //ypos = MBA / mb_width;
    VoC_sub16_rr(REG5,REG5,REG3,DEFAULT);//xpos
    VoC_movreg16(REG0,REG5,IN_PARALLEL);
    VoC_blt16_rd(YUV_ROTATE_xypos_MBA_loop_end,REG0,YUV_ROTATE_X_mb_width);
    VoC_inc_p(REG1,DEFAULT);//ypos++
    VoC_jump(YUV_ROTATE_xypos_MBA_loop);
YUV_ROTATE_xypos_MBA_loop_end:
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_sw32_d(REG01,YUV_ROTATE_X_xpos);//xpos, ypos
    VoC_lw32_d(ACC0,YUV_ROTATE_X_INPUT_ADDR);

    VoC_add16_rd(REG4,REG4,YUV_ROTATE_Y_const_1); //MBA+1
    VoC_inc_p(REG0,DEFAULT); //xpos+1
    VoC_bne16_rd(YUV_ROTATE_dma_in_next,REG0,YUV_ROTATE_X_mb_width);
    VoC_inc_p(REG1,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);
    VoC_bne16_rd(YUV_ROTATE_dma_in_next,REG1,YUV_ROTATE_X_mb_height);
    VoC_sw16_d(REG0,YUV_ROTATE_dma_in_flag);
    VoC_jump(YUV_ROTATE_dma_in_over);

YUV_ROTATE_dma_in_next:
    VoC_and16_ri(REG4,1); //(MBA+1) & 1 , 0: IN_BUF0  ,1: IN_BUF1

    VoC_mac32_rd(REG1,YUV_ROTATE_X_Stride);
    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_shr32_ri(REG01,-4,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,REG01,DEFAULT);
    VoC_shr16_ri(REG4,-6,IN_PARALLEL);  //128>>1

    //DMA IN  YUV422 8X8 BLOCK
    //VoC_lw32_d(ACC0,YUV_ROTATE_X_INPUT_ADDR); //dma in eaddr
    VoC_lw16_d(REG0,YUV_ROTATE_X_Stride);
    VoC_lw16i(REG2,(YUV_ROTATE_Y_IN_BUF)/2); //dma local addr
    VoC_lw16i_short(REG1,8,DEFAULT); //dma lines
    VoC_shr16_ri(REG0,1,IN_PARALLEL);
    VoC_lw16i_short(REG3,4,DEFAULT); //dma size=2 (word)
    VoC_add16_rr(REG2,REG2,REG4,IN_PARALLEL); //which buf0 or buf1

    VoC_sub16_rr(REG0,REG0,REG3,DEFAULT); //dma stride =the remain size (word)
    VoC_lw16i_short(RL7_LO,0,IN_PARALLEL);
    VoC_lw16i(RL7_HI,(DMA_UNSIGNED_B2S>>16)); //dma read,byte to unsigned short
    VoC_jal(YUV_ROTATE_SEND_DMA_CMD);
    //VoC_jal(YUV_ROTATE_WAIT_DMA_DONE);

YUV_ROTATE_dma_in_over:
    VoC_lw16_d(REG5,YUV_ROTATE_X_MBA);
    VoC_lw16_d(REG4,YUV_ROTATE_X_Direction);
    VoC_and16_ri(REG5,1); //MBA & 1
    VoC_shr16_ri(REG5,-7,DEFAULT); //128
    VoC_bltz16_r(YUV_ROTATE_left_90,REG4);
    // direction 1 :right 90
    VoC_lw16i_set_inc(REG0,YUV_ROTATE_Y_IN_BUF,2);
    VoC_lw16i_set_inc(REG1,YUV_ROTATE_X_BufY+7,8);
    VoC_lw16i_set_inc(REG2,YUV_ROTATE_Y_BufU+3,4);
    VoC_lw16i_set_inc(REG3,YUV_ROTATE_Y_BufV+3,4);

    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);

    VoC_loop_i(1,4)    //YUV422 -->YUV420
    VoC_loop_i(0,2)
    VoC_lw32inc_p(REG45,REG0,DEFAULT); // Y U
    VoC_lw32inc_p(REG67,REG0,DEFAULT); //Y V
    VoC_lw32inc_p(RL6,REG0,DEFAULT); // Y U
    VoC_lw32inc_p(RL7,REG0,DEFAULT); //Y V

    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_sw16inc_p(REG7,REG3,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_sw16inc_p(REG5,REG2,IN_PARALLEL);

    VoC_sw16inc_p(RL6_LO,REG1,DEFAULT);
    VoC_sw16inc_p(RL7_HI,REG3,IN_PARALLEL);

    VoC_sw16inc_p(RL7_LO,REG1,DEFAULT);
    VoC_sw16inc_p(RL6_HI,REG2,IN_PARALLEL);
    VoC_endloop0
    VoC_sub16_rd(REG1,REG1,YUV_ROTATE_Y_const_65);

    VoC_loop_i(0,2)
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_lw16inc_p(REG5,REG0,DEFAULT);

    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_lw16inc_p(REG7,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0
    VoC_sub16_rd(REG2,REG2,YUV_ROTATE_Y_const_17);
    VoC_sub16_rd(REG3,REG3,YUV_ROTATE_Y_const_17);
    VoC_sub16_rd(REG1,REG1,YUV_ROTATE_Y_const_65);
    VoC_endloop1

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,-3,IN_PARALLEL);
    VoC_lw32_d(REG45,YUV_ROTATE_X_xpos); //xpos, ypos
    VoC_sub16_dr(REG7,YUV_ROTATE_X_mb_height,REG7);

    VoC_sub16_rr(REG4,REG7,REG5,DEFAULT); //dst_xpos
    VoC_movreg16(REG5,REG4,IN_PARALLEL); //dst_ypos
    VoC_lw32_d(ACC0,YUV_ROTATE_X_OUTPUT_YADDR);
    VoC_mac32_rd(REG5,YUV_ROTATE_X_Dstwidth);
    VoC_movreg32(REG67,REG45,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_shr32_ri(REG45,-3,DEFAULT);
    VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);
    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT);

    VoC_multf32_rd(ACC1,REG7,YUV_ROTATE_X_Dstwidth);
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_shr32_ri(REG67,-2,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,REG67,DEFAULT);

    VoC_bez16_d(YUV_ROTATE_dma_in_no_wait0,YUV_ROTATE_dma_in_flag);
    VoC_jal(YUV_ROTATE_WAIT_DMA_DONE);
YUV_ROTATE_dma_in_no_wait0:
    //dma out y
    VoC_lw16i(REG2,(YUV_ROTATE_X_BufY)/2); //laddr
    VoC_lw16_d(REG0,YUV_ROTATE_X_Dstwidth);
    VoC_lw16i_short(REG1,8,DEFAULT);  //lines
    VoC_lw16i_short(REG3,2,IN_PARALLEL); //size (word)
    VoC_shr16_ri(REG0,2,DEFAULT);
    VoC_lw16i_short(RL7_LO,0,IN_PARALLEL);
    VoC_sub16_rr(REG0,REG0,REG3,DEFAULT); //stride (word)
    VoC_lw16i(RL7_HI,(DMA_WRITE|DMA_UNSIGNED_B2S)>>16);
    VoC_jal(YUV_ROTATE_SEND_DMA_CMD);
    VoC_jal(YUV_ROTATE_WAIT_DMA_DONE);

    VoC_add32_rd(ACC0,ACC1,YUV_ROTATE_X_OUTPUT_UADDR);
    VoC_lw16i(REG2,(YUV_ROTATE_Y_BufU)/2); //laddr
    VoC_lw16_d(REG0,YUV_ROTATE_X_Dstwidth);
    VoC_lw16i_short(REG1,4,DEFAULT);  //lines
    VoC_lw16i_short(REG3,1,IN_PARALLEL); //size (word)
    VoC_shr16_ri(REG0,3,DEFAULT);
    VoC_lw16i_short(RL7_LO,0,IN_PARALLEL);
    VoC_sub16_rr(REG0,REG0,REG3,DEFAULT); //stride (word)
    VoC_lw16i(RL7_HI,(DMA_WRITE|DMA_UNSIGNED_B2S)>>16);
    VoC_jal(YUV_ROTATE_SEND_DMA_CMD);
    VoC_jal(YUV_ROTATE_WAIT_DMA_DONE);

    VoC_add32_rd(ACC0,ACC1,YUV_ROTATE_X_OUTPUT_VADDR);
    VoC_lw16i(REG2,(YUV_ROTATE_Y_BufV)/2); //laddr
    VoC_jal(YUV_ROTATE_SEND_DMA_CMD);
    VoC_jal(YUV_ROTATE_WAIT_DMA_DONE);
    VoC_jump(YUV_ROTATE_next_block);
YUV_ROTATE_left_90:
    //direction -1: left 90
    VoC_lw16i_set_inc(REG0,YUV_ROTATE_Y_IN_BUF,2);
    VoC_lw16i_set_inc(REG1,YUV_ROTATE_X_BufY+7*8,-8);
    VoC_lw16i_set_inc(REG2,YUV_ROTATE_Y_BufU+3*4,-4);
    VoC_lw16i_set_inc(REG3,YUV_ROTATE_Y_BufV+3*4,-4);
    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);

    VoC_loop_i(1,4)
    VoC_loop_i(0,2)
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_lw32inc_p(RL7,REG0,DEFAULT);

    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_sw16inc_p(REG7,REG3,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_sw16inc_p(REG5,REG2,IN_PARALLEL);

    VoC_sw16inc_p(RL6_LO,REG1,DEFAULT);
    VoC_sw16inc_p(RL7_HI,REG3,IN_PARALLEL);

    VoC_sw16inc_p(RL7_LO,REG1,DEFAULT);
    VoC_sw16inc_p(RL6_HI,REG2,IN_PARALLEL);
    VoC_endloop0
    VoC_add16_rd(REG1,REG1,YUV_ROTATE_Y_const_65);

    VoC_loop_i(0,2)
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_lw16inc_p(REG5,REG0,DEFAULT);

    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_lw16inc_p(REG7,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0
    VoC_add16_rd(REG2,REG2,YUV_ROTATE_Y_const_17);
    VoC_add16_rd(REG3,REG3,YUV_ROTATE_Y_const_17);
    VoC_add16_rd(REG1,REG1,YUV_ROTATE_Y_const_65);
    VoC_endloop1

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,-3,IN_PARALLEL);
    VoC_lw32_d(REG45,YUV_ROTATE_X_xpos);
    VoC_sub16_dr(REG7,YUV_ROTATE_X_mb_width,REG7);

    VoC_sub16_rr(REG5,REG7,REG4,DEFAULT); //dst_ypos
    VoC_movreg16(REG4,REG5,IN_PARALLEL); //dst_xpos

    VoC_lw32_d(ACC0,YUV_ROTATE_X_OUTPUT_YADDR);
    VoC_mac32_rd(REG5,YUV_ROTATE_X_Dstwidth);
    VoC_movreg32(REG67,REG45,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_shr32_ri(REG45,-3,DEFAULT);
    VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);
    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT);

    VoC_multf32_rd(ACC1,REG7,YUV_ROTATE_X_Dstwidth);
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_shr32_ri(REG67,-2,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,REG67,DEFAULT);

    VoC_bez16_d(YUV_ROTATE_dma_in_no_wait1,YUV_ROTATE_dma_in_flag);
    VoC_jal(YUV_ROTATE_WAIT_DMA_DONE);
YUV_ROTATE_dma_in_no_wait1:

    //dma out y
    VoC_lw16i(REG2,(YUV_ROTATE_X_BufY)/2); //laddr
    VoC_lw16_d(REG0,YUV_ROTATE_X_Dstwidth);
    VoC_lw16i_short(REG1,8,DEFAULT);  //lines
    VoC_lw16i_short(REG3,2,IN_PARALLEL); //size (word)
    VoC_shr16_ri(REG0,2,DEFAULT);
    VoC_lw16i_short(RL7_LO,0,IN_PARALLEL);
    VoC_sub16_rr(REG0,REG0,REG3,DEFAULT); //stride (word)
    VoC_lw16i(RL7_HI,(DMA_WRITE|DMA_UNSIGNED_B2S)>>16);
    VoC_jal(YUV_ROTATE_SEND_DMA_CMD);
    VoC_jal(YUV_ROTATE_WAIT_DMA_DONE);

    VoC_add32_rd(ACC0,ACC1,YUV_ROTATE_X_OUTPUT_UADDR);
    VoC_lw16i(REG2,(YUV_ROTATE_Y_BufU)/2); //laddr
    VoC_lw16_d(REG0,YUV_ROTATE_X_Dstwidth);
    VoC_lw16i_short(REG1,4,DEFAULT);  //lines
    VoC_lw16i_short(REG3,1,IN_PARALLEL); //size (word)
    VoC_shr16_ri(REG0,3,DEFAULT);
    VoC_lw16i_short(RL7_LO,0,IN_PARALLEL);
    VoC_sub16_rr(REG0,REG0,REG3,DEFAULT); //stride (word)
    VoC_lw16i(RL7_HI,(DMA_WRITE|DMA_UNSIGNED_B2S)>>16);
    VoC_jal(YUV_ROTATE_SEND_DMA_CMD);
    VoC_jal(YUV_ROTATE_WAIT_DMA_DONE);

    VoC_add32_rd(ACC0,ACC1,YUV_ROTATE_X_OUTPUT_VADDR);
    VoC_lw16i(REG2,(YUV_ROTATE_Y_BufV)/2); //laddr
    VoC_jal(YUV_ROTATE_SEND_DMA_CMD);
    VoC_jal(YUV_ROTATE_WAIT_DMA_DONE);

YUV_ROTATE_next_block:
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_add16_rd(REG4,REG4,YUV_ROTATE_X_MBA);
    VoC_NOP();
    VoC_sw16_d(REG4,YUV_ROTATE_X_MBA);
    VoC_jump(YUV_ROTATE_loop_start);


YUV_ROTATE_end:
    exit_on_warnings =OFF;

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


/*********************************************
function: rgb565 rotate 90 or -90
parameters:
input format: rgb565 , 1 pixel =1 short
src_width,src_height:  the multiple of 8
stride: the number of pixels in a line , the multiple of 8, stride >= width
direction: 1:rotate right 90; -1 rotate left 90
dst_width,dst_height: dst_width=src_height,dst_height=src_width

add by WZQUAN
2013-10-31
*********************************************/
void VOC_RGB565_rotate(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,IN_PARALLEL);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,IN_PARALLEL);

    exit_on_warnings=ON;
    VoC_lw32_d(REG01,RGB_ROTATE_X_srcwidth);
    VoC_movreg32(REG67,REG01,DEFAULT);

    VoC_movreg16(REG0,REG1,DEFAULT); //dst_width=height
    VoC_movreg16(REG1,REG0,IN_PARALLEL); //dst_height= width
    VoC_shr16_ri(REG6,3,DEFAULT);
    VoC_shr16_ri(REG7,3,IN_PARALLEL);

    VoC_sw32_d(REG01,RGB_ROTATE_X_dstwidth);
    VoC_sw32_d(REG67,RGB_ROTATE_X_mb_width);

    VoC_mult16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_sw32_d(RL7,RGB_ROTATE_X_xpos);
    VoC_sw16_d(RL7_LO,RGB_ROTATE_X_MBA); //MBA=0;
    VoC_sw32_d(REG45,RGB_ROTATE_Y_dma_status);  //initial dma_status=1: need dma
    VoC_sw16_d(REG6,RGB_ROTATE_X_MBAmax); //MBAmax=mb_width*mb_height, total blocks

    VoC_lw16_d(REG0,RGB_ROTATE_X_stride);
    VoC_lw32_d(ACC0,RGB_ROTATE_X_INPUT_ADDR);
    VoC_shr16_ri(REG0,1,DEFAULT);
    VoC_lw16i_short(REG1,8,IN_PARALLEL); //dma lines
    VoC_lw16i(REG2,RGB_ROTATE_Y_in_block/2);
    VoC_lw16i_short(REG3,4,DEFAULT); // rgb565 ,8 pixels=16 bytes/4=4 word
    VoC_lw32z(RL7,IN_PARALLEL);
    VoC_sub16_rr(REG0,REG0,REG3,DEFAULT); //dma stride (word)
    VoC_jal(YUV_ROTATE_SEND_DMA_CMD);
    VoC_jal(YUV_ROTATE_WAIT_DMA_DONE);

rgb_rotate_loop_start:

    VoC_lw16_d(REG4,RGB_ROTATE_X_MBA);
    VoC_blt16_rd(rgb_rotate_mba_lt_mbamax,REG4,RGB_ROTATE_X_MBAmax);
    VoC_jump(rgb_rotate_end);
rgb_rotate_mba_lt_mbamax:
    VoC_lw16i_set_inc(REG1,0,1);
    VoC_lw16_d(REG0,RGB_ROTATE_X_MBA);
    VoC_lw16_d(REG3,RGB_ROTATE_X_mb_width);
    VoC_movreg16(REG5,REG0,DEFAULT);
    VoC_lw16i_short(FORMAT32,-4,IN_PARALLEL);
rgb_rotate_mba_loop:
    VoC_sub16_rr(REG5,REG5,REG3,DEFAULT);
    VoC_movreg16(REG0,REG5,IN_PARALLEL);
    VoC_blt16_rd(rgb_rotate_mba_loop_end,REG0,RGB_ROTATE_X_mb_width);
    VoC_inc_p(REG1,DEFAULT);
    VoC_jump(rgb_rotate_mba_loop);
rgb_rotate_mba_loop_end:
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);
    VoC_sw32_d(REG01,RGB_ROTATE_X_xpos);
    VoC_lw32_d(ACC0,RGB_ROTATE_X_INPUT_ADDR); //dma next block

    VoC_add16_rr(REG4,REG4,REG3,DEFAULT); //MBA+1
    VoC_inc_p(REG0,IN_PARALLEL);
    VoC_blt16_rd(rgb_rotate_dma_next,REG0,RGB_ROTATE_X_mb_width);
    VoC_inc_p(REG1,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);
    VoC_blt16_rd(rgb_rotate_dma_next,REG1,RGB_ROTATE_X_mb_height);
    VoC_sw16_d(REG0,RGB_ROTATE_Y_dma_status);
    VoC_jump(rgb_rotate_dma_over);
rgb_rotate_dma_next:
    VoC_and16_ri(REG4,1);
    VoC_mac32_rd(REG1,RGB_ROTATE_X_stride); //addr (byte)
    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_shr32_ri(REG01,-4,DEFAULT);
    VoC_shr16_ri(REG4,-5,IN_PARALLEL); //buf0=&in_block , buf1=&in_block+32 , (word)

    VoC_add32_rr(ACC0,ACC0,REG01,DEFAULT); //dma eaddr
    VoC_lw16_d(REG0,RGB_ROTATE_X_stride);
    VoC_lw16i(REG2,RGB_ROTATE_Y_in_block/2);
    VoC_lw16i_short(REG1,8,DEFAULT); //dma lines
    VoC_shr16_ri(REG0,1,IN_PARALLEL); //short--->word
    VoC_lw16i_short(REG3,4,DEFAULT); //dma sizes
    VoC_add16_rr(REG2,REG2,REG4,IN_PARALLEL); //buf0 or buf1

    VoC_sub16_rr(REG0,REG0,REG3,DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);
    VoC_jal(YUV_ROTATE_SEND_DMA_CMD);

rgb_rotate_dma_over:
    VoC_lw16_d(REG5,RGB_ROTATE_X_MBA);
    VoC_lw16_d(REG4,RGB_ROTATE_X_direction);
    VoC_and16_ri(REG5,1);
    VoC_shr16_ri(REG5,-6,DEFAULT);
    VoC_bltz16_r(rgb_rotate_left_90,REG4);
    //right 90
    VoC_lw16i_set_inc(REG0,RGB_ROTATE_Y_in_block,2);  //line_cur
    VoC_lw16i_set_inc(REG1,RGB_ROTATE_Y_in_block+8,2);//line_next
    VoC_lw16i_set_inc(REG2,RGB_ROTATE_X_out_buf+6,8);

    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG5,IN_PARALLEL);

    VoC_loop_i(1,4)
    VoC_loop_i(0,2)
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_lw32inc_p(REG45,REG1,DEFAULT);

    VoC_lw32inc_p(RL7,REG0,DEFAULT);
    VoC_lw32inc_p(RL6,REG1,DEFAULT);

    VoC_movreg16(REG5,REG6,DEFAULT);
    VoC_movreg16(REG6,REG5,IN_PARALLEL);

    VoC_movreg16(RL6_HI,RL7_LO,DEFAULT);
    VoC_movreg16(RL7_LO,RL6_HI,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG2,DEFAULT);
    VoC_sw32inc_p(REG67,REG2,DEFAULT);
    VoC_sw32inc_p(RL6,REG2,DEFAULT);
    VoC_sw32inc_p(RL7,REG2,DEFAULT);
    VoC_endloop0
    VoC_lw16i(REG4,66);
    VoC_lw16i_short(REG5,8,DEFAULT);
    VoC_sub16_rr(REG2,REG2,REG4,IN_PARALLEL);
    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG5,IN_PARALLEL);
    VoC_endloop1

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,-4,IN_PARALLEL);
    VoC_lw32_d(REG45,RGB_ROTATE_X_xpos);
    VoC_sub16_dr(REG7,RGB_ROTATE_X_mb_height,REG7);

    VoC_sub16_rr(REG4,REG7,REG5,DEFAULT);  //dst_xpos
    VoC_movreg16(REG5,REG4,IN_PARALLEL);  //dst_ypos

    VoC_lw32_d(ACC0,RGB_ROTATE_X_OUTPUT_ADDR);
    VoC_mac32_rd(REG5,RGB_ROTATE_X_dstwidth); //1 pixel= 2bytes
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_shr32_ri(REG45,-4,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT); //out dst addr

    VoC_bez16_d(rgb_rotate_dma_no_waiting0,RGB_ROTATE_Y_dma_status);
    VoC_jal(YUV_ROTATE_WAIT_DMA_DONE);
rgb_rotate_dma_no_waiting0:
    VoC_lw16_d(REG0,RGB_ROTATE_X_dstwidth);
    VoC_lw16i(REG2,RGB_ROTATE_X_out_buf/2); //dma laddr
    VoC_shr16_ri(REG0,1,DEFAULT);  //short to word
    VoC_lw16i_short(REG4,1,IN_PARALLEL);
    VoC_add16_rd(REG4,REG4,RGB_ROTATE_X_MBA); //MBA++

    VoC_lw16i_short(REG1,8,DEFAULT); //dma lines
    VoC_lw16i_short(REG3,4,IN_PARALLEL); //dma sizes (word)
    VoC_sub16_rr(REG0,REG0,REG3,DEFAULT); //dma stride (word)
    VoC_lw16i_short(RL7_LO,0,IN_PARALLEL);

    VoC_sw16_d(REG4,RGB_ROTATE_X_MBA);

    VoC_lw16i(RL7_HI,DMA_WRITE>>16);
    VoC_jal(YUV_ROTATE_SEND_DMA_CMD);
    VoC_jal(YUV_ROTATE_WAIT_DMA_DONE);


    VoC_jump(rgb_rotate_loop_start);

rgb_rotate_left_90:
    VoC_lw16i_set_inc(REG0,RGB_ROTATE_Y_in_block,2);  //line_cur
    VoC_lw16i_set_inc(REG1,RGB_ROTATE_Y_in_block+8,2);//line_next
    VoC_lw16i_set_inc(REG2,RGB_ROTATE_X_out_buf+7*8,-8);

    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG5,IN_PARALLEL); //buf0 or buf1

    VoC_loop_i(1,4)
    VoC_loop_i(0,2)
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_lw32inc_p(REG67,REG1,DEFAULT);

    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_lw32inc_p(RL7,REG1,DEFAULT);

    VoC_movreg16(REG5,REG6,DEFAULT);
    VoC_movreg16(REG6,REG5,IN_PARALLEL);
    VoC_movreg16(RL6_HI,RL7_LO,DEFAULT);
    VoC_movreg16(RL7_LO,RL6_HI,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG2,DEFAULT);
    VoC_sw32inc_p(REG67,REG2,DEFAULT);
    VoC_sw32inc_p(RL6,REG2,DEFAULT);
    VoC_sw32inc_p(RL7,REG2,DEFAULT);
    VoC_endloop0
    VoC_lw16i(REG4,66);
    VoC_lw16i_short(REG5,8,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG4,IN_PARALLEL);
    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG5,IN_PARALLEL);
    VoC_endloop1

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,-4,IN_PARALLEL);
    VoC_lw32_d(REG45,RGB_ROTATE_X_xpos);
    VoC_sub16_dr(REG7,RGB_ROTATE_X_mb_width,REG7);
    VoC_movreg16(REG4,REG5,DEFAULT);
    VoC_sub16_rr(REG5,REG7,REG4,IN_PARALLEL);

    VoC_lw32_d(ACC0,RGB_ROTATE_X_OUTPUT_ADDR);
    VoC_mac32_rd(REG5,RGB_ROTATE_X_dstwidth);
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_shr32_ri(REG45,-4,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT); //out dst addr

    VoC_bez16_d(rgb_rotate_no_wait_last_dma,RGB_ROTATE_Y_dma_status);
    VoC_jal(YUV_ROTATE_WAIT_DMA_DONE);
rgb_rotate_no_wait_last_dma:
    VoC_lw16_d(REG0,RGB_ROTATE_X_dstwidth);
    VoC_lw16i(REG2,RGB_ROTATE_X_out_buf/2); //dma laddr
    VoC_shr16_ri(REG0,1,DEFAULT);  //short to word
    VoC_lw16i_short(REG4,1,IN_PARALLEL);
    VoC_add16_rd(REG4,REG4,RGB_ROTATE_X_MBA); //MBA++

    VoC_lw16i_short(REG1,8,DEFAULT); //dma lines
    VoC_lw16i_short(REG3,4,IN_PARALLEL); //dma sizes (word)
    VoC_sub16_rr(REG0,REG0,REG3,DEFAULT); //dma stride (word)
    VoC_lw16i_short(RL7_LO,0,IN_PARALLEL);

    VoC_sw16_d(REG4,RGB_ROTATE_X_MBA);

    VoC_lw16i(RL7_HI,DMA_WRITE>>16);
    VoC_jal(YUV_ROTATE_SEND_DMA_CMD);
    VoC_jal(YUV_ROTATE_WAIT_DMA_DONE);

    VoC_jump(rgb_rotate_loop_start);

rgb_rotate_end:

    exit_on_warnings =OFF;

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;

}