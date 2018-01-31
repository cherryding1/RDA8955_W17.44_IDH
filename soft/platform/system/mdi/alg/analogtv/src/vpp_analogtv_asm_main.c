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


#include "vppp_analogtv_common.h"
#include "vppp_analogtv_asm_map.h"

// VoC_directive: voc_stack16
#define SP_16_ADDR                                      RAM_X_MAX - 1 // measured minimum : 0x1000 - 0x27
// VoC_directive: voc_stack32
#define SP_32_ADDR                                      RAM_Y_MAX - 2 // measured minimum : 0x1800 - 0x19


#if 0
//X ADDR

voc_struct  GLOBAL_CONST_X_STRUCT  __attribute__((export)),X
voc_short   CONST_TabV2R_ADDR[256],   X
voc_short   CONST_TableV2G_ADDR[256],   X
voc_alias   CONST_1_ADDR    CONST_TableV2G_ADDR+2 ,X
voc_short   CONST_R_ARRAY[512+104],       X
voc_short   CONST_B_ARRAY[512],       X
voc_alias   CONST_B128_ARRAY  CONST_B_ARRAY+256,X
voc_alias   CONST_R128_ARRAY  CONST_R_ARRAY+256,X

voc_struct_end

voc_short   CONST_R_ADDR[2],X
voc_short   CONST_G_ADDR[2],X
voc_short   CONST_B_ADDR[2],X


//voc_short   CONST_CMD_ADDR[6],   X
//320*2*2 =1280  old
//240*3*3 =2160  new
voc_short   GLOBAL_TEMP_BUF_ADDR[1440],  X
voc_alias   GLOBAL_TEMP_PTMP2_ADDR    GLOBAL_TEMP_BUF_ADDR+720 , X

voc_short   GLOBAL_BUF_IN_ADDR[6400],      X

voc_short   GLOBAL_BUF_OUT_ADDR[240],      X
voc_short   GLOBAL_BUF_OUT_ADDR2[240],      X




//Y ADDR
voc_struct  GLOBAL_INPUT_STRUCT  __attribute__((export)),Y
voc_short   INPUT_ADDR_ADDR[2]//read
voc_short   OUTPUT_ADDR_ADDR[2]//write
voc_short   BLEND_ADDR_ADDR[2]//read
voc_short   INPUT_SRC_W_ADDR
voc_short   INPUT_SRC_H_ADDR
voc_short   INPUT_CUT_W_ADDR
voc_short   INPUT_CUT_H_ADDR
voc_short   INPUT_DEC_W_ADDR
voc_short   INPUT_DEC_H_ADDR
voc_short   INPUT_RESET_ADDR
voc_short   INPUT_ROTATE_ADDR
voc_struct_end


voc_struct  GLOBAL_CONST_Y_STRUCT  __attribute__((export)),Y
voc_short   CONST_TabU2G_ADDR[256],   y
voc_short   CONST_TabU2B_ADDR[256],   y
voc_short   CONST_G_ARRAY[512],Y
voc_alias   CONST_G128_ARRAY  CONST_G_ARRAY+256,Y
voc_struct_end

voc_short   INPUT_IN_MODE_ADDR             ,Y
voc_short   GLOBAL_BLEND_DMASIZE_ADDR      ,Y
voc_short   GLOBAL_BLEND_DMAINC_ADDR[2]    ,Y
voc_short  GLOBAL_DEAD_UX_ADDR,         Y//pa
voc_short  GLOBAL_TEMP_BUF2_ADDR,       Y//ptmp2
voc_short  GLOBAL_INLINE_NUM_ADDR,      Y//28
voc_short  GLOBAL_INLINE_COUNT_ADDR,    Y//28*src_w
voc_short  GLOBAL_DEAD_OUT_LINE_ADDR,   Y//128*74=9472  176*54=9504
voc_short  GLOBAL_INLINE_NUM_1_ADDR,        Y
voc_short  GLOBAL_NUM1_ADDR,           Y
voc_short  GLOBAL_NUM3_ADDR,           Y
voc_short  GLOBAL_P1_ADDR,           Y
voc_short  GLOBAL_P2_ADDR,           Y


voc_short  GLOBAL_DMAIN_SIZE_ADDR[2],           Y//(28*src_w)*2
voc_short  GLOBAL_SCALEX_SIZE_ADDR[2],          Y
voc_short  GLOBAL_SCALEY_SIZE_ADDR[2],          Y
voc_short  GLOBAL_MASK_ADDR[2],            Y
voc_short  CONST_0XFF00FF_ADDR[2],         Y
voc_short  GLOBAL_FLAG_WRITE_ADDR,           Y
voc_short  GLOBAL_FLAG_BLEND_ADDR,           Y

voc_short   GLOBAL_ROTATE_BUF_OUT_ADDR[6600] ,Y
voc_alias   GLOBAL_TEMP_PTMP3_ADDR GLOBAL_ROTATE_BUF_OUT_ADDR,Y
//voc_alias  GLOBAL_BUF_OUT_ADDR   GLOBAL_ROTATE_BUF_OUT_ADDR,      Y
//voc_alias  GLOBAL_BUF_OUT_ADDR2  GLOBAL_BUF_OUT_ADDR+320   ,      Y
#endif
/***************************************
this program is used to deal with the
data from sensor siv100b whose format
is u y2 v y1; the process contians:
cut + zoom in/out + yuv2rgb
**note***
1.the max number of pelses is 320*240 [short]
2.the dec_w and dec_h must be multiple of 2
****************************************/
#if 1
void Coolsand_MAIN(void)
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

    VoC_jal(Coolsand_CameraPreview_Entrance);

    VoC_jump(AudioDecode_MAIN_L0);
}

void CoolSand_SignBits2(void)
{
    //reg4: x;  reg5: n
    //used buf: reg3
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_lw16i_short(REG5,-30,IN_PARALLEL);
Lable_X_L1:
    //for( n = 30 x >>= 1; --n );
    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG3,IN_PARALLEL);
    VoC_bnez16_r(Lable_X_L1,REG4);

    VoC_return;//return n;
}


void CoolSand_Div2(void)
{
    //reg6:     divd;  reg7: divs
    //output:   rl6_lo
    //used buf:r3 r4 r5  r6 r7 acc0 acc1 rl6
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);
    //dexp = signbits(divd)-1;
    VoC_movreg16(REG4,REG6,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_jal(CoolSand_SignBits2);
    VoC_movreg16(REG6,REG5,DEFAULT);
    VoC_movreg16(ACC0_LO,REG6,IN_PARALLEL);
    //sexp = signbits(divs)-1;
    VoC_movreg16(REG4,REG7,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_jal(CoolSand_SignBits2);
    VoC_movreg16(ACC1_LO,REG7,DEFAULT);
    VoC_shru32_rr(ACC0,REG6,IN_PARALLEL);//acc0:divd = divd << (dexp);

    VoC_shru32_rr(ACC1,REG5,DEFAULT);//acc1:divs = divs << (sexp);
    VoC_sub16_rr(REG4,REG5,REG6,IN_PARALLEL);

    VoC_lw16i_short(REG7,4,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);//r4: exp  = dexp-sexp+4;
    VoC_lw16i_short(REG7,0,IN_PARALLEL);//r67: 0x0001

    VoC_lw16i_short(REG3,S15+4,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);
LABLE_DIV_COUNTDOWN:
    VoC_bgt32_rr(LABLE_DIV_L0,ACC1,ACC0);//if (divs <= divd)
    VoC_sub32_rr(ACC0,ACC0,ACC1,DEFAULT);//divd = divd - divs;
    VoC_add32_rr(RL6,RL6,REG67,IN_PARALLEL);//r3 = r3 + r2;
LABLE_DIV_L0:
    VoC_shru32_ri(RL6,-1,DEFAULT);//r3   <<= 1;
    VoC_shru32_ri(ACC0,-1,IN_PARALLEL);//divd <<= 1;

    VoC_inc_p(REG3,DEFAULT);
    VoC_bnez16_r(LABLE_DIV_COUNTDOWN,REG3);

    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);

    VoC_shru32_rr(RL6,REG4,DEFAULT);//r3=r3>>exp;    //loop - exp

    VoC_return;//return (int)r3;
}
////////////zoom_blit_default = 0////////////////////
void COOLSAND_DMA_READ(void)
{
    //memcpy(bufin,pin,numcount);
    VoC_lw16i(REG5,GLOBAL_BUF_IN_ADDR/2);
    VoC_lw16_d(REG4,GLOBAL_INLINE_COUNT_ADDR);
    VoC_lw32_d(ACC0,INPUT_ADDR_ADDR);

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_add32_rd(ACC0,ACC0,GLOBAL_DMAIN_SIZE_ADDR);//pin+=(numline-1)*src_w;
    VoC_NOP();
    VoC_sw32_d(ACC0,INPUT_ADDR_ADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);


    VoC_return;
}
void COOLSAND_DMA_WRITE(void)
{
    //check out blend dma's status
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_bez16_d(LABLE_WHETHER_UPDATE_ADDR,INPUT_RESET_ADDR);

    VoC_lw16i_short(REG4,4,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR/2);
    VoC_sw16_d(REG5,INPUT_RESET_ADDR);

    VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);//ex addr[byte]

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_return;
LABLE_WHETHER_UPDATE_ADDR:

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_xor16_rd(REG4,GLOBAL_FLAG_WRITE_ADDR);
    VoC_bnez16_d(LABLE_FLAG_WRITE1,GLOBAL_FLAG_WRITE_ADDR);
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR/2);
    VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR2,1);
    VoC_jump(LABLE_FLAG_WRITE2);
LABLE_FLAG_WRITE1:
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR2/2);
    VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR,1);
LABLE_FLAG_WRITE2:
    VoC_sw16_d(REG4,GLOBAL_FLAG_WRITE_ADDR);

    VoC_lw16_d(REG4,GLOBAL_BLEND_DMASIZE_ADDR);
    VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);//ex addr[byte]

    //judge the dma's condition
    //VoC_cfg(CFG_CTRL_STALL);//VoC_cfg(CFG_CTRL_CONTINUE);
    //VoC_NOP();
    //VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);


    VoC_add32_rd(ACC0,ACC0,GLOBAL_BLEND_DMAINC_ADDR);
    VoC_NOP();
    VoC_sw32_d(ACC0,OUTPUT_ADDR_ADDR);

    //VoC_cfg(CFG_CTRL_STALL);
    //VoC_NOP();
    //VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_return;
}

void COOLSAND_DMA_WRITE_withoutblend(void)
{

    VoC_bez16_d(LABLE_WHETHER_UPDATE_ADDR_withoutblend,INPUT_RESET_ADDR);

    VoC_lw16i_short(REG4,4,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR/2);
    VoC_sw16_d(REG5,INPUT_RESET_ADDR);

    VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);//ex addr[byte]

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_return;
LABLE_WHETHER_UPDATE_ADDR_withoutblend:

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_xor16_rd(REG4,GLOBAL_FLAG_WRITE_ADDR);
    VoC_bnez16_d(LABLE_FLAG_WRITE1_withoutblend,GLOBAL_FLAG_WRITE_ADDR);
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR/2);
    VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR2,1);
    VoC_jump(LABLE_FLAG_WRITE2_withoutblend);
LABLE_FLAG_WRITE1_withoutblend:
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR2/2);
    VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR,1);
LABLE_FLAG_WRITE2_withoutblend:
    VoC_sw16_d(REG4,GLOBAL_FLAG_WRITE_ADDR);

    VoC_lw16_d(REG4,GLOBAL_BLEND_DMASIZE_ADDR);
    VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);//ex addr[byte]

    //judge the dma's condition
    //VoC_cfg(CFG_CTRL_STALL);//VoC_cfg(CFG_CTRL_CONTINUE);
    //VoC_NOP();
    //VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);


    VoC_add32_rd(ACC0,ACC0,GLOBAL_BLEND_DMAINC_ADDR);
    VoC_NOP();
    VoC_sw32_d(ACC0,OUTPUT_ADDR_ADDR);

    //VoC_cfg(CFG_CTRL_STALL);
    //VoC_NOP();
    //VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_return;
}

void COOLSAND_DMA_READ_BlendBuf(void)
{
    //memcpy(bufin,pin,numcount);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_xor16_rd(REG4,GLOBAL_FLAG_BLEND_ADDR);
    VoC_bnez16_d(LABLE_FLAG_BLEND1,GLOBAL_FLAG_BLEND_ADDR);
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR/2);
    VoC_jump(LABLE_FLAG_BLEND2);
LABLE_FLAG_BLEND1:
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR2/2);
LABLE_FLAG_BLEND2:
    VoC_sw16_d(REG4,GLOBAL_FLAG_BLEND_ADDR);


    VoC_lw16_d(REG4,GLOBAL_BLEND_DMASIZE_ADDR);
    VoC_lw32_d(ACC0,BLEND_ADDR_ADDR);

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_add32_rd(ACC0,ACC0,GLOBAL_BLEND_DMAINC_ADDR);//pin+=(numline-1)*src_w;
    VoC_NOP();
    VoC_sw32_d(ACC0,BLEND_ADDR_ADDR);


//  VoC_cfg(CFG_CTRL_STALL);//VoC_cfg(CFG_CTRL_CONTINUE);
//  VoC_NOP();
//    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);


    VoC_return;
}

//void  Coolsand_WriteLast(void)
//{
//       VoC_sub16_rr(REG4,REG1,REG5,DEFAULT);
//       VoC_shru16_ri(REG5,1,IN_PARALLEL);//in addr[int]
//       VoC_shru16_ri(REG4,1,DEFAULT);//size  [int]
//       VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);//ex addr[byte]
//
//       VoC_sw32_d(REG45,CFG_DMA_SIZE);
//       VoC_sw32_d(ACC0,CFG_DMA_EADDR);
//
//       VoC_cfg(CFG_CTRL_STALL);
//       VoC_NOP();
//       VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
//
//       VoC_return;
//}

//void Coolsand_DMAWRITE_slowly(void)
//{
//           VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR/2);
//           VoC_lw16i_short(REG4,WRITE_BURST_SIZE,DEFAULT);//VoC_lw16_d(REG4,GLOBAL_BLEND_DMASIZE_ADDR);
//           VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);//ex addr[byte]
//
//               VoC_lw16i_short(ACC1_HI,0,DEFAULT);
//               VoC_lw16i_short(ACC1_LO,WRITE_BURST_SIZE*4,IN_PARALLEL);
//           VoC_lw16d_set_inc(REG3,GLOBAL_BLEND_DMASIZE_ADDR,-WRITE_BURST_SIZE);
//dma_write_slowly:
//
//           VoC_sw32_d(REG45,CFG_DMA_SIZE);
//           VoC_sw32_d(ACC0,CFG_DMA_EADDR);
//
//           VoC_cfg(CFG_CTRL_STALL);
//             VoC_NOP();
//           VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
//
//              VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
//           VoC_inc_p(REG3,DEFAULT);
//           VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);
//
//
//           VoC_bnez16_r(dma_write_slowly,REG3);
//
//              VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);
//              VoC_add32_rd(ACC0,ACC0,GLOBAL_BLEND_DMAINC_ADDR);
//           VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR,1);
//              VoC_sw32_d(ACC0,OUTPUT_ADDR_ADDR);
//
//           VoC_return;
//}
//
//void Coolsand_WriteLast_slowly(void)
//{
//       VoC_lw16i_short(INC3,-WRITE_BURST_SIZE,DEFAULT);
//       VoC_sub16_rr(REG3,REG1,REG5,DEFAULT);
//       VoC_shru16_ri(REG5,1,IN_PARALLEL);//in addr[int]
//       VoC_shru16_ri(REG3,1,DEFAULT);//size  [int]
//       VoC_lw16i_short(REG4,WRITE_BURST_SIZE,IN_PARALLEL);
//
//         VoC_lw16i_short(ACC1_HI,0,DEFAULT);
//         VoC_lw16i_short(ACC1_LO,WRITE_BURST_SIZE*4,IN_PARALLEL);
//       VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);//ex addr[byte]
//dma_write_final_slowly:
//       VoC_sw32_d(REG45,CFG_DMA_SIZE);
//       VoC_sw32_d(ACC0,CFG_DMA_EADDR);
//
//       VoC_cfg(CFG_CTRL_STALL);
//       VoC_NOP();
//       VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
//
//          VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
//       VoC_inc_p(REG3,DEFAULT);
//       VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);
//
//       VoC_bnez16_r(dma_write_final_slowly,REG3);
//
//       VoC_return;
//}


////////////zoom_blit_sb33bf = 1////////////////////
//void COOLSAND_DMA_READ_Slowly(void)
//{
//  //memcpy(bufin,pin,numcount);
//  VoC_lw16i(REG5,GLOBAL_BUF_IN_ADDR/2);
//
//  VoC_lw32z(ACC1,DEFAULT);
//  VoC_lw16i_short(REG4,READ_BURST_SIZE,IN_PARALLEL);
//
//    VoC_lw32_d(ACC0,INPUT_ADDR_ADDR);
//
//    VoC_lw16i(ACC1_LO,READ_BURST_SIZE*4);
//  //VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);
//
//  VoC_lw16d_set_inc(REG3,GLOBAL_INLINE_COUNT_ADDR,-READ_BURST_SIZE);
//
//loop_slowread_dma:
//
//      VoC_sw32_d(REG45,CFG_DMA_SIZE);
//      VoC_sw32_d(ACC0,CFG_DMA_EADDR);
//
//      VoC_cfg(CFG_CTRL_STALL);//VoC_cfg(CFG_CTRL_CONTINUE);
//      VoC_NOP();
//      VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
//
//      VoC_inc_p(REG3,DEFAULT);
//      VoC_add32_rr(ACC0,ACC0,ACC1,IN_PARALLEL);
//
//      VoC_add16_rr(REG5,REG5,REG4,DEFAULT);
//
//      VoC_bnez16_r(loop_slowread_dma,REG3);
//
//  VoC_lw32_d(ACC0,INPUT_ADDR_ADDR);
//  VoC_add32_rd(ACC0,ACC0,GLOBAL_DMAIN_SIZE_ADDR);//pin+=(numline-1)*src_w;
//  VoC_NOP();
//      VoC_sw32_d(ACC0,INPUT_ADDR_ADDR);
//  VoC_return;
//}
//
//void COOLSAND_DMA_READ_BlendBuf_Slowly(void)
//{
//
//  //memcpy(bufin,pin,numcount);
//  VoC_lw32z(ACC1,DEFAULT);
//  VoC_lw16i_short(REG4,READ_BURST_SIZE,IN_PARALLEL);
//
//  VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR/2);
//    VoC_lw32_d(ACC0,BLEND_ADDR_ADDR);
//
//  VoC_lw16i(ACC1_LO,READ_BURST_SIZE*4);
//
//  VoC_lw16d_set_inc(REG3,GLOBAL_BLEND_DMASIZE_ADDR,-READ_BURST_SIZE);
//
//loop_slow_dma:
//
//      VoC_sw32_d(REG45,CFG_DMA_SIZE);
//      VoC_sw32_d(ACC0,CFG_DMA_EADDR);
//
//      VoC_cfg(CFG_CTRL_STALL);//VoC_cfg(CFG_CTRL_CONTINUE);
//      VoC_NOP();
//      VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
//
//      VoC_inc_p(REG3,DEFAULT);
//      VoC_add32_rr(ACC0,ACC0,ACC1,IN_PARALLEL);
//
//      VoC_add16_rr(REG5,REG5,REG4,DEFAULT);
//
//      VoC_bnez16_r(loop_slow_dma,REG3);
//
//      VoC_lw32_d(ACC0,BLEND_ADDR_ADDR);
//      VoC_add32_rd(ACC0,ACC0,GLOBAL_BLEND_DMAINC_ADDR);//pin+=(numline-1)*src_w;
//      VoC_NOP();
//          VoC_sw32_d(ACC0,BLEND_ADDR_ADDR);
//
//  VoC_return;
//}

//void Coolsand_DMAWRITE_sb33bf(void)
//{
//           VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR/2);
//           VoC_lw16i_short(REG4,WRITE_BURST_SIZE,DEFAULT);//VoC_lw16_d(REG4,GLOBAL_BLEND_DMASIZE_ADDR);
//           VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);//ex addr[byte]
//
//               //VoC_lw16i_short(ACC1_HI,0,DEFAULT);
//               //VoC_lw16i_short(ACC1_LO,4,IN_PARALLEL);
//           VoC_lw16d_set_inc(REG3,GLOBAL_BLEND_DMASIZE_ADDR,-WRITE_BURST_SIZE);
//dma_write_slowlysb33bf:
//
//           VoC_sw32_d(REG45,CFG_DMA_SIZE);
//           VoC_sw32_d(ACC0,CFG_DMA_EADDR);
//
//           VoC_cfg(CFG_CTRL_STALL);
//             VoC_NOP();
//           VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
//
//              // VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
//           VoC_inc_p(REG3,DEFAULT);
//           VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);
//
//
//           VoC_bnez16_r(dma_write_slowlysb33bf,REG3);
//
//              //VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);
//              //VoC_add32_rd(ACC0,ACC0,GLOBAL_BLEND_DMAINC_ADDR);
//           VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR,1);
//              //VoC_sw32_d(ACC0,OUTPUT_ADDR_ADDR);
//
//           VoC_return;
//}
//
//void Coolsand_WriteLast_sb33bf_blitLCD(void)
//{
//       VoC_lw16i_short(INC3,-WRITE_BURST_SIZE,DEFAULT);
//       VoC_sub16_rr(REG3,REG1,REG5,DEFAULT);
//       VoC_shru16_ri(REG5,1,IN_PARALLEL);//in addr[int]
//       VoC_shru16_ri(REG3,1,DEFAULT);//size  [int]
//       VoC_lw16i_short(REG4,WRITE_BURST_SIZE,IN_PARALLEL);
//         //VoC_lw16i_short(ACC1_HI,0,DEFAULT);
//         //VoC_lw16i_short(ACC1_LO,4,IN_PARALLEL);
//       VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);//ex addr[byte]
//dma_write_final_slowlysb33bf:
//       VoC_sw32_d(REG45,CFG_DMA_SIZE);
//       VoC_sw32_d(ACC0,CFG_DMA_EADDR);
//
//       VoC_cfg(CFG_CTRL_STALL);
//       VoC_NOP();
//       VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
//
//          //VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
//       VoC_inc_p(REG3,DEFAULT);
//       VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);
//
//       VoC_bnez16_r(dma_write_final_slowlysb33bf,REG3);
//
//       VoC_return;
//}

//////main program's start entrance/////////////////////////////
void Coolsand_CameraPreview_Entrance(void)
{
    VoC_push16(RA,DEFAULT);
    //do clear job here
    VoC_lw16i_short(REG0,0,IN_PARALLEL);

    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,IN_PARALLEL);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,IN_PARALLEL);
    VoC_sw16_d(REG0,CFG_DMA_WRAP);
    VoC_lw16i(STATUS,STATUS_CLR);



    //////Write const values to cmd reg to active the lcd bf533////
    //this part need to be changed
//  VoC_bez16_d(LABLE_SEND_CONST,INPUT_BLIT_MODE_ADDR);
//  VoC_lw16i_short(REG2,1,DEFAULT);//size
//  VoC_lw16i_set_inc(REG3,CONST_CMD_ADDR/2,1);//addr
//  VoC_lw32_d(ACC0,CAM_ADDR_ADDR);
//  VoC_loop_i_short(3,DEFAULT)
//  VoC_startloop0
//        VoC_sw32_d(REG23,CFG_DMA_SIZE);
//      VoC_sw32_d(ACC0,CFG_DMA_EADDR);
//
//      VoC_cfg(CFG_CTRL_STALL);
//      VoC_NOP();
//      VoC_inc_p(REG3,DEFAULT);
//      VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
//  VoC_endloop0
//LABLE_SEND_CONST:
    VoC_lw16i(REG4,CONST_R128_ARRAY);
    VoC_lw16i(REG5,CONST_G128_ARRAY);
    VoC_lw16i(REG6,CONST_B128_ARRAY);
    VoC_sw16_d(REG4,CONST_R_ADDR);
    VoC_sw16_d(REG4,CONST_R_ADDR+1);
    VoC_sw16_d(REG5,CONST_G_ADDR);
    VoC_sw16_d(REG5,CONST_G_ADDR+1);
    VoC_sw16_d(REG6,CONST_B_ADDR);
    VoC_sw16_d(REG6,CONST_B_ADDR+1);
    VoC_lw32_d(REG45,INPUT_CUT_W_ADDR);
    VoC_bne16_rd(LABLE_SELECT_ZOOMIN,REG4,INPUT_DEC_W_ADDR);
    VoC_bne16_rd(LABLE_SELECT_ZOOMIN,REG5,INPUT_DEC_H_ADDR);
    //VoC_bez16_d(LABLE_BEGIN_YUV2RGB,INPUT_RESET_ADDR);
    VoC_jal(Coolsand_InitGlobal_Normal);

//LABLE_BEGIN_YUV2RGB:
    VoC_bnez16_d(LABLE_YUV2RGB_ROTATE,INPUT_ROTATE_ADDR);
    VoC_jal(Coolsand_YUV2RGB565_Directly);
    VoC_jump(LABLE_SELECT_ENTRANCE);
LABLE_YUV2RGB_ROTATE:
    VoC_jal(Coolsand_YUV2RGB565_Rotate);
    VoC_jump(LABLE_SELECT_ENTRANCE);
LABLE_SELECT_ZOOMIN:
    VoC_bnlt16_rd(LABLE_SELECT_ZOOMOUT,REG4,INPUT_DEC_W_ADDR);
    VoC_jal(Coolsand_InitGlobal_withZoom);
    VoC_jal(COOLSAND_DMA_READ);
    // VoC_jal(COOLSAND_DMA_READ_BlendBuf);
    VoC_lw16i(REG4,640);
    VoC_bne16_rd(LABLE_ZOOMIN_WITHOUT_BLEND,REG4,INPUT_SRC_W_ADDR);
    VoC_jal(Coolsand_bilinear_preview_yuv_zoomin);
    VoC_jump(LABLE_SELECT_ENTRANCE);
LABLE_ZOOMIN_WITHOUT_BLEND:
    VoC_jal(Coolsand_bilinear_preview_yuv_zoomin_withoutblend);
    VoC_jump(LABLE_SELECT_ENTRANCE);
LABLE_SELECT_ZOOMOUT:
    VoC_jal(Coolsand_InitGlobal_withZoom);
//  VoC_lw16i(REG4,CONST_R128_ARRAY);
//  VoC_lw16i(REG5,CONST_G128_ARRAY);
//  VoC_lw16i(REG6,CONST_B128_ARRAY);
//  VoC_sw16_d(REG4,CONST_R_ADDR);
//  VoC_sw16_d(REG4,CONST_R_ADDR+1);
//  VoC_sw16_d(REG5,CONST_G_ADDR);
//  VoC_sw16_d(REG5,CONST_G_ADDR+1);
//  VoC_sw16_d(REG6,CONST_B_ADDR);
//  VoC_sw16_d(REG6,CONST_B_ADDR+1);
    VoC_jal(COOLSAND_DMA_READ);
    // VoC_jal(COOLSAND_DMA_READ_BlendBuf);
    VoC_lw16i(REG4,640);
    VoC_bne16_rd(LABLE_ZOOMOUT_WITHOUT_BLEND,REG4,INPUT_SRC_W_ADDR);
    VoC_jal(Coolsand_bilinear_preview_yuv_zoomout);
    VoC_jump(LABLE_SELECT_ENTRANCE);
LABLE_ZOOMOUT_WITHOUT_BLEND:
    VoC_jal(Coolsand_bilinear_preview_yuv_zoomout_withoutblend);
LABLE_SELECT_ENTRANCE:

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}
void Coolsand_InitGlobal_withZoom(void)
{
    VoC_lw16i(REG4,0XFF);
    VoC_movreg16(REG5,REG4,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0,INPUT_SRC_W_ADDR,2);
    VoC_sw32_d(REG45,CONST_0XFF00FF_ADDR);

    VoC_lw16i_short(FORMAT16,-16+1,DEFAULT);
    VoC_lw16i_short(REG4,NUMLINE,IN_PARALLEL);

    //ADD for SRC WIDTH==640
    VoC_lw16i(REG5,640);
    VoC_bne16_rd(LABLE2_WIDTH_640,REG5,INPUT_SRC_W_ADDR);
    VoC_shru16_ri(REG4,1,DEFAULT);
LABLE2_WIDTH_640:

    VoC_multf16_rp(REG5,REG4,REG0,DEFAULT);             //numcount = numline*src_w>>1;
    VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);//SRC_W  SRC_H

    VoC_lw32_d(ACC0,INPUT_ADDR_ADDR);

    VoC_sub16_rr(REG4,REG4,REG2,DEFAULT);//numline-1
    VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);

    VoC_sw32_d(REG45,GLOBAL_INLINE_NUM_ADDR);//numline = 28;  numcount = numline*src_w>>1;

    VoC_multf32_rr(ACC1,REG6,REG4,DEFAULT);//(numline-1)*src_w<<1
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);//CUT_W  CUT_H

    VoC_sw16_d(REG4,GLOBAL_INLINE_NUM_1_ADDR);
    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);//offx = (src_w - cut_w)>>1;
    VoC_sub16_rr(REG7,REG7,REG5,IN_PARALLEL);//offx = (offx>>1)<<1;

    VoC_shr16_ri(REG6,2,DEFAULT);//offy = (src_h - cut_h)>>1;
    VoC_shr16_ri(REG7,1,IN_PARALLEL);//offy
    VoC_shr16_ri(REG6,-1,DEFAULT);//offx
    VoC_lw16i_short(INC3,1,IN_PARALLEL);

    VoC_sw32_d(ACC1,GLOBAL_DMAIN_SIZE_ADDR);

    VoC_lw16i(REG4,GLOBAL_BUF_IN_ADDR);
    VoC_add16_rr(REG6,REG6,REG4,DEFAULT);//pa = bufin + offx;
    VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);

    VoC_mac32_rd(REG7,INPUT_SRC_W_ADDR);//pin = srcbuf + offy*src_w;

    VoC_sw16_d(REG6,GLOBAL_DEAD_UX_ADDR);

    VoC_lw32_d(REG67,INPUT_CUT_W_ADDR);
    VoC_lw32_d(RL7,INPUT_DEC_W_ADDR);

    VoC_sw32_d(ACC0,INPUT_ADDR_ADDR);//update buffer in addr
    VoC_push16(RA,DEFAULT);
    VoC_movreg16(REG7,RL7_LO,DEFAULT);
    VoC_movreg16(RL7_LO,REG7,IN_PARALLEL);
    VoC_lw16i(REG0,GLOBAL_SCALEX_SIZE_ADDR);
    VoC_sub16_rd(REG6,REG6,CONST_1_ADDR);
    VoC_jal(CoolSand_Div2);//scalex=(cut_w<<S15)/dec_w;
    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sw32inc_p(RL6,REG0,IN_PARALLEL);
    VoC_sub16_rd(REG6,REG6,CONST_1_ADDR);
    VoC_jal(CoolSand_Div2);//scaley=(cut_h<<S15)/dec_h;
    VoC_pop16(RA,DEFAULT);

    VoC_lw16i(REG5,GLOBAL_TEMP_PTMP2_ADDR);
    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);//w  = 0;
    VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR,1);//REG1 : OUT BUF
    VoC_sw16_d(REG5,GLOBAL_TEMP_BUF2_ADDR);
    VoC_lw16i_short(ACC1_LO,-2,DEFAULT);
    VoC_lw16i(ACC1_LO,MASK);
    VoC_sw32_d(ACC1,GLOBAL_NUM1_ADDR);//num2 = -2;//num3 = 0;
    VoC_sw32_d(ACC1,GLOBAL_MASK_ADDR);


    //just output one line
    VoC_lw16_d(REG4,INPUT_DEC_W_ADDR);
    VoC_shru16_ri(REG4,1,DEFAULT);//[int]
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_shru16_ri(REG4,-2,IN_PARALLEL);//[BYTE]
    VoC_sw16_d(REG4,GLOBAL_BLEND_DMASIZE_ADDR);
    VoC_sw32_d(REG45,GLOBAL_BLEND_DMAINC_ADDR);
    VoC_sw32_d(ACC0,GLOBAL_FLAG_WRITE_ADDR);

    //begin to load input data and back data to the voc ram
    //VoC_jal(COOLSAND_DMA_READ);
    VoC_lw16i_short(FORMATX,S15-16,DEFAULT);
    VoC_lw16i_short(FORMAT32,S15-16,IN_PARALLEL);

    VoC_return;
}

void Coolsand_InitGlobal_Normal(void)
{
    VoC_lw16i(REG4,0XFF);
    VoC_movreg16(REG5,REG4,DEFAULT);
    //VoC_lw16i_short(REG2,0,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0,INPUT_SRC_W_ADDR,2);
    VoC_sw32_d(REG45,CONST_0XFF00FF_ADDR);

    VoC_lw16i_short(FORMAT16,-16+1,DEFAULT);
    VoC_lw16i_short(REG4,YUV2RGBLINE,IN_PARALLEL);

    VoC_multf16_rp(REG5,REG4,REG0,DEFAULT);             //numcount = numline*src_w>>1;
    VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);//SRC_W  SRC_H

    VoC_multf32_rr(ACC1,REG6,REG4,DEFAULT);//(numline-1)*src_w<<1
    VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);

    VoC_sw16_d(REG4,GLOBAL_INLINE_NUM_1_ADDR);

    VoC_sw32_d(REG45,GLOBAL_INLINE_NUM_ADDR);//numline = 28;  numcount = numline*src_w>>1;

    VoC_lw32inc_p(REG45,REG0,DEFAULT);//CUT_W  CUT_H

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);//offx = (src_w - cut_w)>>1;
    VoC_sub16_rr(REG7,REG7,REG5,IN_PARALLEL);//offy = (src_h - cut_h)>>1;

    VoC_shr16_ri(REG6,2,DEFAULT);//offx = (offx>>1)<<1;
    VoC_shr16_ri(REG7,1,IN_PARALLEL);//offy
    VoC_shr16_ri(REG6,-1,DEFAULT);//offx
    VoC_lw16i_short(INC3,1,IN_PARALLEL);

    VoC_sw32_d(ACC1,GLOBAL_DMAIN_SIZE_ADDR);

    VoC_lw16i(REG4,GLOBAL_BUF_IN_ADDR);
    VoC_add16_rr(REG6,REG6,REG4,DEFAULT);//pa = bufin + offx;
    VoC_lw32z(ACC1,IN_PARALLEL);//w  = 0;

    VoC_lw32_d(ACC0,INPUT_ADDR_ADDR);

    VoC_mac32_rd(REG7,INPUT_SRC_W_ADDR);//pin = srcbuf + offy*src_w;
    VoC_lw16i(ACC1_LO,MASK);
    VoC_sw16_d(REG6,GLOBAL_DEAD_UX_ADDR);

    VoC_sw32_d(ACC0,INPUT_ADDR_ADDR);//update buffer in addr

    VoC_bez16_d(LABLE_CUTZOOM_NO_ROTATE,INPUT_ROTATE_ADDR);
    VoC_sw32_d(ACC1,GLOBAL_MASK_ADDR);

    VoC_lw16i(REG1,GLOBAL_ROTATE_BUF_OUT_ADDR);
    VoC_lw16i_short(INC1,YUV2RGBLINE,DEFAULT);
    VoC_lw16i_short(REG5,YUV2RGBLINE-1,IN_PARALLEL);

    VoC_add16_rr(REG1,REG1,REG5,DEFAULT);//OUPUT ADDR
    VoC_lw16i_short(REG5,YUV2RGBLINE,IN_PARALLEL);



    VoC_sub16_dr(REG6,INPUT_DEC_H_ADDR,REG5);
    VoC_shru16_ri(REG6,-1,DEFAULT);//SHORT->BYTE
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_add32_rd(REG67,REG67,OUTPUT_ADDR_ADDR);

    VoC_lw16i_short(REG4,YUV2RGBLINE/2,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_shr16_ri(REG4,-2,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_sw16_d(REG4,GLOBAL_BLEND_DMASIZE_ADDR);
    VoC_sw32_d(REG45,GLOBAL_BLEND_DMAINC_ADDR);
    VoC_lw16_d(ACC0_LO,INPUT_DEC_H_ADDR);
    VoC_shru32_ri(ACC0,-1,DEFAULT);
    VoC_sw32_d(REG67,OUTPUT_ADDR_ADDR);
    VoC_sw32_d(ACC0,GLOBAL_P1_ADDR);
    VoC_jump(LABLE_CUTZOOM_INIT_QUITE);
LABLE_CUTZOOM_NO_ROTATE:
    //VoC_sw32_d(ACC0,INPUT_ADDR_ADDR);//update buffer in addr
    VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR,2);//REG1 : OUT BUF

    VoC_sw32_d(ACC1,GLOBAL_MASK_ADDR);

    //just output one line
    VoC_lw16_d(REG4,INPUT_DEC_W_ADDR);
    VoC_shru16_ri(REG4,1,DEFAULT);//[int]
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_shru16_ri(REG4,-2,IN_PARALLEL);//[BYTE]
    VoC_sw16_d(REG4,GLOBAL_BLEND_DMASIZE_ADDR);
    VoC_sw32_d(REG45,GLOBAL_BLEND_DMAINC_ADDR);
    VoC_sw32_d(ACC0,GLOBAL_FLAG_WRITE_ADDR);
LABLE_CUTZOOM_INIT_QUITE:

    //use inner ram
    //VoC_lw16_d(REG5,INPUT_DEC_H_ADDR)
    //VoC_lw32_d(RL7,OUTPUT_ADDR_ADDR);

    //VoC_sw16_d(REG5,GLOBAL_NUM3_ADDR);
    //VoC_sw32_d(RL7,GLOBAL_SCALEY_SIZE_ADDR);

    VoC_lw16i(RL6_LO,CONST_TabU2G_ADDR);
    VoC_lw16i(RL6_HI,CONST_TableV2G_ADDR);
    VoC_lw16i(RL7_HI,CONST_TabV2R_ADDR);
    VoC_lw16i(RL7_LO,CONST_TabU2B_ADDR);

    VoC_return;
}
/************************************************************************/
/* yuv -> rgb      320*240  rotate 90-degree
   rotate the image 90 degree
   output can mostly hold  8960/320=28-line
   but for convenience  I will make the output line the same as  input line
*/
/************************************************************************/
void Coolsand_YUV2RGB565_Rotate(void)
{
    //int i=0;
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG4,YUV2RGBLINE,IN_PARALLEL);
    VoC_sub16_dr(REG4,INPUT_DEC_H_ADDR,REG4);
    VoC_NOP();
    VoC_sw16_d(REG4,INPUT_DEC_H_ADDR);

LABLE_YUV2RGB_ROTATE_BEGIN:
    //VoC_jal(COOLSAND_DMA_READ);
    VoC_lw16i(REG5,GLOBAL_BUF_IN_ADDR/2);
    VoC_lw16_d(REG4,GLOBAL_INLINE_COUNT_ADDR);
    VoC_lw32_d(ACC0,INPUT_ADDR_ADDR);

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_add32_rd(ACC0,ACC0,GLOBAL_DMAIN_SIZE_ADDR);//pin+=(numline-1)*src_w;
    VoC_NOP();
    VoC_sw32_d(ACC0,INPUT_ADDR_ADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);



//  VoC_blt16_rd(LABLE_SHORT_HEIGHT1,REG4,INPUT_DEC_H_ADDR);
//    VoC_lw16_d(REG4,INPUT_DEC_H_ADDR);
//LABLE_SHORT_HEIGHT1:

    //VoC_sub16_dr(REG5,INPUT_DEC_H_ADDR,REG4);
    VoC_lw16_d(REG0,GLOBAL_DEAD_UX_ADDR);
    //VoC_sw16_d(REG5,GLOBAL_SCALEX_SIZE_ADDR);

    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_lw16i_short(REG4,YUV2RGBLINE,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_push16(REG1,IN_PARALLEL);

    VoC_loop_r(1,REG4)
    //i++;
    //conver one line yuv to rgb565


    VoC_jal(Coolsand_yuv2rgb565_oneline_whenRotate);


    VoC_endloop1


    VoC_jal(Coolsand_yuv2rgb565_oneline_with_rotateDMA);

    //printf("i=%d\n",i);
    VoC_pop16(REG1,DEFAULT);
    VoC_lw16i_short(REG4,YUV2RGBLINE,IN_PARALLEL);
    VoC_bez16_d(LABLE_YUV2RGB_ROTATE_EXITE,INPUT_DEC_H_ADDR)
    VoC_sub16_dr(REG4,INPUT_DEC_H_ADDR,REG4);
    VoC_NOP();
    VoC_sw16_d(REG4,INPUT_DEC_H_ADDR);
    VoC_jump(LABLE_YUV2RGB_ROTATE_BEGIN);
LABLE_YUV2RGB_ROTATE_EXITE:

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}

/************************************************************************/
/*     240*240                                                          */
/************************************************************************/

void Coolsand_YUV2RGB565_Directly(void)
{
    //int i=0;
    //make sure the height and YUV2RGBLINE must be even


    //VoC_bgtz16_d(LABLE_YUV2RGB_PROCESS,GLOBAL_NUM3_ADDR);
    //  VoC_return;

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG4,YUV2RGBLINE,IN_PARALLEL);
    VoC_sub16_dr(REG5,INPUT_DEC_H_ADDR,REG4);
    VoC_NOP();
    VoC_sw16_d(REG5,GLOBAL_NUM3_ADDR);


LABLE_YUV2RGB_PROCESS:


//LABLE_YUV2RGB_PROCESS_BEGIN:
    //VoC_jal(COOLSAND_DMA_READ);
    VoC_lw16i(REG5,GLOBAL_BUF_IN_ADDR/2);
    VoC_lw16_d(REG4,GLOBAL_INLINE_COUNT_ADDR);
    VoC_lw32_d(ACC0,INPUT_ADDR_ADDR);

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_add32_rd(ACC0,ACC0,GLOBAL_DMAIN_SIZE_ADDR);//pin+=(numline-1)*src_w;
    VoC_NOP();
    VoC_sw32_d(ACC0,INPUT_ADDR_ADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);


    //yuv2rgb
    VoC_lw16i_short(REG6,2,DEFAULT);
    VoC_lw16i_short(REG4,YUV2RGBLINE,IN_PARALLEL);
    VoC_sub16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_lw16_d(REG0,GLOBAL_DEAD_UX_ADDR);
//  VoC_bgt16_rd(LABLE_YUV2RGB_LAST,REG4,GLOBAL_NUM3_ADDR);
//    VoC_sub16_dr(REG5,GLOBAL_NUM3_ADDR,REG4);
//    VoC_sub16_rr(REG4,REG4,REG6,DEFAULT);
//  VoC_sw16_d(REG5,GLOBAL_NUM3_ADDR);
//  VoC_jump(LABLE_LOOP_YUV2RGB_H);
//LABLE_YUV2RGB_LAST:
//  VoC_lw16_d(REG4,GLOBAL_NUM3_ADDR);
//    VoC_lw16i_short(REG5,0,DEFAULT);
//    VoC_sub16_rr(REG4,REG4,REG6,DEFAULT);
//  VoC_sw16_d(REG5,GLOBAL_NUM3_ADDR);



//LABLE_LOOP_YUV2RGB_H:
    VoC_push16(REG4,DEFAULT);//LOOP-2
//pre-loop
    VoC_push16(REG0,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);


    VoC_jal(Coolsand_yuv2rgb565_oneline);

    VoC_pop16(REG0,DEFAULT);
    VoC_add16_rd(REG0,REG0,INPUT_SRC_W_ADDR);

    //dma out one line data
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_xor16_rd(REG4,GLOBAL_FLAG_WRITE_ADDR);
    VoC_bnez16_d(LABLE_FLAG_WRITE1_ONELINE_PRE,GLOBAL_FLAG_WRITE_ADDR);
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR/2);
    VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR2,2);
    VoC_jump(LABLE_FLAG_WRITE2_ONELINE_PRE);
LABLE_FLAG_WRITE1_ONELINE_PRE:
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR2/2);
    VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR,2);
LABLE_FLAG_WRITE2_ONELINE_PRE:
    VoC_sw16_d(REG4,GLOBAL_FLAG_WRITE_ADDR);

    VoC_lw16_d(REG4,GLOBAL_BLEND_DMASIZE_ADDR);
    VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);//ex addr[byte]GLOBAL_SCALEY_SIZE_ADDR

    //start DMA
    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_add32_rd(ACC0,ACC0,GLOBAL_BLEND_DMAINC_ADDR);
    VoC_NOP();
    VoC_sw32_d(ACC0,OUTPUT_ADDR_ADDR);//GLOBAL_SCALEY_SIZE_ADDR

    //VoC_cfg(CFG_CTRL_STALL);
    //VoC_NOP();
    //VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

//loop
    VoC_pop16(REG4,DEFAULT);
    VoC_bez16_r(LABLE_AFTER_LOOP,REG4);

    VoC_loop_r(1,REG4)
    //i++;
    //conver one line yuv to rgb565
    VoC_push16(REG0,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_jal(Coolsand_yuv2rgb565_oneline);

    VoC_pop16(REG0,DEFAULT);
    VoC_add16_rd(REG0,REG0,INPUT_SRC_W_ADDR);

    //dma out one line data
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_xor16_rd(REG4,GLOBAL_FLAG_WRITE_ADDR);
    VoC_bnez16_d(LABLE_FLAG_WRITE1_ONELINE,GLOBAL_FLAG_WRITE_ADDR);
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR/2);
    VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR2,2);
    VoC_jump(LABLE_FLAG_WRITE2_ONELINE);
LABLE_FLAG_WRITE1_ONELINE:
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR2/2);
    VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR,2);
LABLE_FLAG_WRITE2_ONELINE:
    VoC_sw16_d(REG4,GLOBAL_FLAG_WRITE_ADDR);

    VoC_lw16_d(REG4,GLOBAL_BLEND_DMASIZE_ADDR);
    VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);//ex addr[byte]GLOBAL_SCALEY_SIZE_ADDR

    //Check out DMA status
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    //start DMA
    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_add32_rd(ACC0,ACC0,GLOBAL_BLEND_DMAINC_ADDR);
    VoC_NOP();
    VoC_sw32_d(ACC0,OUTPUT_ADDR_ADDR);//GLOBAL_SCALEY_SIZE_ADDR

    //VoC_cfg(CFG_CTRL_STALL);
    //VoC_NOP();
    //VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_endloop1

LABLE_AFTER_LOOP:
    VoC_push16(REG0,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_jal(Coolsand_yuv2rgb565_oneline);

    VoC_pop16(REG0,DEFAULT);
    VoC_add16_rd(REG0,REG0,INPUT_SRC_W_ADDR);

    //dma out one line data
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_xor16_rd(REG4,GLOBAL_FLAG_WRITE_ADDR);
    VoC_bnez16_d(LABLE_FLAG_WRITE1_ONELINE_AFTER,GLOBAL_FLAG_WRITE_ADDR);
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR/2);
    VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR2,2);
    VoC_jump(LABLE_FLAG_WRITE2_ONELINE_AFTER);
LABLE_FLAG_WRITE1_ONELINE_AFTER:
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR2/2);
    VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR,2);
LABLE_FLAG_WRITE2_ONELINE_AFTER:
    VoC_sw16_d(REG4,GLOBAL_FLAG_WRITE_ADDR);

    VoC_lw16_d(REG4,GLOBAL_BLEND_DMASIZE_ADDR);
    VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);//ex addr[byte]GLOBAL_SCALEY_SIZE_ADDR

    //Check out DMA status
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    //start DMA
    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_add32_rd(ACC0,ACC0,GLOBAL_BLEND_DMAINC_ADDR);
    VoC_NOP();
    VoC_sw32_d(ACC0,OUTPUT_ADDR_ADDR);//GLOBAL_SCALEY_SIZE_ADDR

    //Check out DMA status , finish transmitting data
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);


//LABLE_YUV2RGB_EXIT:
//  VoC_bngtz16_d(LABLE_YUV2RGB_EXIT2,GLOBAL_NUM3_ADDR);
//    VoC_lw16i_short(REG6,2,DEFAULT);
//      VoC_jump(LABLE_YUV2RGB_PROCESS_BEGIN);
//LABLE_YUV2RGB_EXIT2:
    VoC_bez16_d(LABLE_YUV2RGB_EXIT,GLOBAL_NUM3_ADDR);
    VoC_lw16i_short(REG4,YUV2RGBLINE,DEFAULT);
    VoC_sub16_dr(REG4,GLOBAL_NUM3_ADDR,REG4);
    VoC_NOP();
    VoC_sw16_d(REG4,GLOBAL_NUM3_ADDR);
    VoC_jump(LABLE_YUV2RGB_PROCESS);
LABLE_YUV2RGB_EXIT:
    //printf("i=%d\n",i);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}

//yuv2rgb
void Coolsand_yuv2rgb565_oneline(void)
{
    /*****y1 u y2 v ***********
    fun:
        used to transform yuv format data to r g  b format
    input parameters:
       yuv buf addr: r0
       rgb buf addr: r1
    used regs:

    zouying  added  2008-12-2
    ***************************/
    VoC_lw32_d(ACC0,CONST_0XFF00FF_ADDR);
    VoC_lw16_d(REG2,INPUT_CUT_W_ADDR);

    VoC_lw16i(ACC1_LO,CONST_R_ADDR);//VoC_lw16i(ACC1_LO,128);
    VoC_movreg16(ACC1_HI,ACC1_LO,DEFAULT);
    VoC_shru16_ri(REG2,1,IN_PARALLEL);
    //beging to process
    VoC_lw32_p(REG67,REG0,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);

    VoC_shru16_ri(REG6,8,DEFAULT);//u
    VoC_shru16_ri(REG7,8,IN_PARALLEL);//v

    VoC_loop_r(0,REG2);

    //VoC_lw32_p(REG67,REG0,DEFAULT);
    //VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);
    //r4:y1 r5:y2 r6:u r7:v
    //VoC_shru16_ri(REG6,8,DEFAULT);//u
    //VoC_shru16_ri(REG7,8,IN_PARALLEL);//v

    VoC_and32_rr(REG45,ACC0,DEFAULT);
    VoC_add32_rr(REG23,REG67,RL6,IN_PARALLEL);//TableV2G[v]    TabU2G[u]
    VoC_add32_rr(REG67,REG67,RL7,DEFAULT);//TabV2R[v]  TabU2B[u]
    VoC_push16(REG1,IN_PARALLEL);

    VoC_sub16_pp(REG2,REG2,REG3,DEFAULT);//TabU2G[u] - TableV2G[v]
    VoC_movreg16(REG1,ACC1_LO,IN_PARALLEL);//128

    VoC_add16_rp(REG3,REG4,REG7,DEFAULT);//r = irgb_sat[y + TabV2R[v]];
    VoC_add16_rp(REG7,REG5,REG7,IN_PARALLEL);//r1 = irgb_sat[y + TabV2R[v]];
    VoC_add16_rr(REG4,REG4,REG2,DEFAULT);//g = irgb_sat[y + TabU2G[u] - TableV2G[v]];
    VoC_add16_rp(REG6,REG4,REG6,IN_PARALLEL);//b = irgb_sat[y + TabU2B[u]];
    VoC_add16_rr(REG5,REG5,REG2,DEFAULT);//g1 = irgb_sat[y + TabU2G[u] - TableV2G[v]];
    VoC_add16_rp(REG2,REG5,REG6,IN_PARALLEL);//b1 = irgb_sat[y + TabU2B[u]];

    //r  g  b :r3 r4 r6
    //r1 g1 b1:r7 r5 r2
    //VoC_add32_rd(REG23,REG23,CONST_R_ADDR);//X RAM
    VoC_add16inc_rp(REG3,REG3,REG1,DEFAULT);//r
    VoC_add16inc_rp(REG7,REG7,REG1,IN_PARALLEL);
    //VoC_add32_rd(REG45,REG45,CONST_G_ADDR);//Y RAM
    VoC_add16inc_rp(REG4,REG4,REG1,DEFAULT);
    VoC_add16inc_rp(REG5,REG5,REG1,IN_PARALLEL);
    //VoC_add32_rd(REG67,REG67,CONST_B_ADDR);//X RAM
    VoC_add16inc_rp(REG2,REG2,REG1,DEFAULT);
    VoC_add16inc_rp(REG6,REG6,REG1,IN_PARALLEL);

    VoC_lw16_p(REG3,REG3,DEFAULT);//r
    VoC_lw16_p(REG4,REG4,IN_PARALLEL);//g

    VoC_or16_rr(REG4,REG3,DEFAULT);
    VoC_lw16_p(REG6,REG6,IN_PARALLEL);

    VoC_lw16_p(REG7,REG7,DEFAULT);
    VoC_lw16_p(REG5,REG5,IN_PARALLEL);

    VoC_or16_rr(REG4,REG6,DEFAULT);
    VoC_lw16_p(REG2,REG2,IN_PARALLEL);

    VoC_or16_rr(REG5,REG7,DEFAULT);
    VoC_lw32_p(REG67,REG0,IN_PARALLEL);

    VoC_or16_rr(REG5,REG2,DEFAULT);
    VoC_pop16(REG1,IN_PARALLEL);

    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG1,DEFAULT);
    VoC_shru16_ri(REG7,8,IN_PARALLEL);
//  VoC_shr16_ri(REG3,-8,DEFAULT);
//    VoC_shr16_ri(REG4,-8,IN_PARALLEL);
//
//  VoC_shr16_ri(REG3,8,DEFAULT);
//  VoC_shr16_ri(REG4,8,IN_PARALLEL);
//
//  VoC_add16_rr(REG3,REG3,REG1,DEFAULT);
//  VoC_add16_rr(REG4,REG4,REG1,IN_PARALLEL);
//  /////////////////////////////////////////////
//  VoC_shr16_ri(REG6,-8,DEFAULT);
//  VoC_shr16_ri(REG7,-8,IN_PARALLEL);
//
//  VoC_shr16_ri(REG6,8,DEFAULT);
//  VoC_shr16_ri(REG7,8,IN_PARALLEL);
//
//  VoC_add16_rr(REG6,REG6,REG1,DEFAULT);
//  VoC_add16_rr(REG7,REG7,REG1,IN_PARALLEL);
//  /////////////////////////////////////////////
//  VoC_shr16_ri(REG5,-8,DEFAULT);
//  VoC_shr16_ri(REG2,-8,IN_PARALLEL);
//
//  VoC_shr16_ri(REG5,8,DEFAULT);
//  VoC_shr16_ri(REG2,8,IN_PARALLEL);
//
//  VoC_add16_rr(REG5,REG5,REG1,DEFAULT);
//  VoC_add16_rr(REG2,REG2,REG1,IN_PARALLEL);
//
//  /////////////////////////////////////////////
//  //r  g  b :r3 r4 r6
//  //r1 g1 b1:r7 r5 r2
//  VoC_pop16(REG1,DEFAULT);
//  VoC_shru16_ri(REG3,3,IN_PARALLEL);
//
//  VoC_shru16_ri(REG4,2,DEFAULT);
//  VoC_shru16_ri(REG6,3,IN_PARALLEL);
//
//  VoC_shru16_ri(REG3,-11,DEFAULT);
//  VoC_shru16_ri(REG4,-5,IN_PARALLEL);
//
//  VoC_or16_rr(REG4,REG3,DEFAULT);
//  VoC_shru16_ri(REG7,3,IN_PARALLEL);
//
//  VoC_or16_rr(REG4,REG6,DEFAULT);//first
//  VoC_shru16_ri(REG5,2,IN_PARALLEL);
//
//  VoC_shru16_ri(REG7,-11,DEFAULT);
//  VoC_shru16_ri(REG5,-5,IN_PARALLEL);
//
//  VoC_or16_rr(REG5,REG7,DEFAULT);
//  VoC_shru16_ri(REG2,3,IN_PARALLEL);
//
//  VoC_or16_rr(REG5,REG2,DEFAULT);
//    VoC_lw32_p(REG67,REG0,IN_PARALLEL);
//
//  VoC_shru16_ri(REG6,8,DEFAULT);//u
//  VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);
//
//  VoC_sw32inc_p(REG45,REG1,DEFAULT);
//  VoC_shru16_ri(REG7,8,IN_PARALLEL);//v

    VoC_endloop0

    VoC_return;
}

void Coolsand_yuv2rgb565_oneline_whenRotate(void)
{
    /*****y1 u y2 v ***********
    fun:
        used to transform yuv format data to r g  b format
    input parameters:
       yuv buf addr: r0
       rgb buf addr: r1
    used regs:

    zouying  added  2008-12-2
    ***************************/

    VoC_push32(REG01,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_lw32_d(ACC0,CONST_0XFF00FF_ADDR);
    VoC_lw16_d(REG2,INPUT_CUT_W_ADDR);

    VoC_lw16i_short(REG6,0,DEFAULT);

    VoC_lw16i(ACC1_LO,CONST_R_ADDR);// VoC_lw16i(ACC1_LO,128);//

    VoC_shru16_ri(REG2,1,DEFAULT);
    VoC_push16(REG6,IN_PARALLEL);

    VoC_lw16i(ACC1_HI,128);

    //beging to process
    VoC_lw32_p(REG67,REG0,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);

    VoC_shru16_ri(REG6,8,DEFAULT);//u
    VoC_shru16_ri(REG7,8,IN_PARALLEL);//v

    VoC_loop_r(0,REG2);

    //VoC_lw32_p(REG67,REG0,DEFAULT);
    //VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);
    //r4:y1 r5:y2 r6:u r7:v
    //VoC_shru16_ri(REG6,8,DEFAULT);//u
    //VoC_shru16_ri(REG7,8,IN_PARALLEL);//v

    VoC_and32_rr(REG45,ACC0,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);


    VoC_bez16_d(Coolsand_sharp_L0,INPUT_RESET_ADDR)

    VoC_shr32_ri(REG45,-2,DEFAULT);
    VoC_movreg32(REG23,REG45,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG4,REG3,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG2,IN_PARALLEL);

    VoC_movreg16(REG1,ACC1_HI,DEFAULT);
    VoC_lw16_p(REG0,REG0,IN_PARALLEL);

    VoC_pop16(REG2,DEFAULT);
    VoC_shru16_ri(REG0,-8,IN_PARALLEL);

    VoC_push16(REG3,DEFAULT);
    VoC_shru16_ri(REG0,8,IN_PARALLEL);


    VoC_sub16_rr(REG4,REG4,REG2,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG0,IN_PARALLEL);


    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_shr16_ri(REG5,1,IN_PARALLEL);


    VoC_sub16_rr(REG4,REG4,REG1,DEFAULT);//-128
    VoC_sub16_rr(REG5,REG5,REG1,IN_PARALLEL);//-128

    VoC_shr16_ri(REG4,-8,DEFAULT);
    VoC_shr16_ri(REG5,-8,IN_PARALLEL);

    VoC_shr16_ri(REG4,8,DEFAULT);
    VoC_shr16_ri(REG5,8,IN_PARALLEL);
Coolsand_sharp_L0:

    //  VoC_shr16_ri(REG1,0,IN_PARALLEL);

    VoC_add32_rr(REG23,REG67,RL6,DEFAULT);//TableV2G[v]    TabU2G[u]
    VoC_add32_rr(REG67,REG67,RL7,IN_PARALLEL);//TabV2R[v]  TabU2B[u]

    VoC_bez16_d(Coolsand_sharp_L1,INPUT_RESET_ADDR)

    VoC_add16_rr(REG4,REG4,REG1,DEFAULT);//+128
    VoC_add16_rr(REG5,REG5,REG1,IN_PARALLEL);//+128
Coolsand_sharp_L1:
    VoC_sub16_pp(REG2,REG2,REG3,DEFAULT);//TabU2G[u] - TableV2G[v]
    VoC_movreg16(REG0,ACC1_LO,IN_PARALLEL);//128

    VoC_add16_rp(REG3,REG4,REG7,DEFAULT);//r = irgb_sat[y + TabV2R[v]];
    VoC_add16_rp(REG7,REG5,REG7,IN_PARALLEL);//r1 = irgb_sat[y + TabV2R[v]];
    VoC_add16_rr(REG4,REG4,REG2,DEFAULT);//g = irgb_sat[y + TabU2G[u] - TableV2G[v]];
    VoC_add16_rp(REG6,REG4,REG6,IN_PARALLEL);//b = irgb_sat[y + TabU2B[u]];
    VoC_add16_rr(REG5,REG5,REG2,DEFAULT);//g1 = irgb_sat[y + TabU2G[u] - TableV2G[v]];
    VoC_add16_rp(REG2,REG5,REG6,IN_PARALLEL);//b1 = irgb_sat[y + TabU2B[u]];

    //r  g  b :r3 r4 r6
    //r1 g1 b1:r7 r5 r2
    //VoC_add32_rd(REG23,REG23,CONST_R_ADDR);//X RAM
    VoC_add16inc_rp(REG3,REG3,REG0,DEFAULT);//r
    VoC_add16inc_rp(REG7,REG7,REG0,IN_PARALLEL);
    //VoC_add32_rd(REG45,REG45,CONST_G_ADDR);//Y RAM
    VoC_add16inc_rp(REG4,REG4,REG0,DEFAULT);
    VoC_add16inc_rp(REG5,REG5,REG0,IN_PARALLEL);
    //VoC_add32_rd(REG67,REG67,CONST_B_ADDR);//X RAM
    VoC_add16inc_rp(REG2,REG2,REG0,DEFAULT);
    VoC_add16inc_rp(REG6,REG6,REG0,IN_PARALLEL);

    VoC_lw16_p(REG3,REG3,DEFAULT);//r
    VoC_lw16_p(REG4,REG4,IN_PARALLEL);//g

    VoC_or16_rr(REG4,REG3,DEFAULT);
    VoC_lw16_p(REG6,REG6,IN_PARALLEL);

    VoC_lw16_p(REG7,REG7,DEFAULT);
    VoC_lw16_p(REG5,REG5,IN_PARALLEL);

    VoC_or16_rr(REG4,REG6,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);

    VoC_or16_rr(REG5,REG7,DEFAULT);
    VoC_lw16_p(REG2,REG2,IN_PARALLEL);

    VoC_or16_rr(REG5,REG2,DEFAULT);
    VoC_lw32_p(REG67,REG0,IN_PARALLEL);


    VoC_shru16_ri(REG7,8,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);

    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG5,REG1,DEFAULT);

//  VoC_shr16_ri(REG3,-8,DEFAULT);
//    VoC_shr16_ri(REG4,-8,IN_PARALLEL);
//
//  VoC_shr16_ri(REG3,8,DEFAULT);
//  VoC_shr16_ri(REG4,8,IN_PARALLEL);
//
//  VoC_add16_rr(REG3,REG3,REG1,DEFAULT);
//  VoC_add16_rr(REG4,REG4,REG1,IN_PARALLEL);
//  /////////////////////////////////////////////
//  VoC_shr16_ri(REG6,-8,DEFAULT);
//  VoC_shr16_ri(REG7,-8,IN_PARALLEL);
//
//  VoC_shr16_ri(REG6,8,DEFAULT);
//  VoC_shr16_ri(REG7,8,IN_PARALLEL);
//
//  VoC_add16_rr(REG6,REG6,REG1,DEFAULT);
//  VoC_add16_rr(REG7,REG7,REG1,IN_PARALLEL);
//  /////////////////////////////////////////////
//  VoC_shr16_ri(REG5,-8,DEFAULT);
//  VoC_shr16_ri(REG2,-8,IN_PARALLEL);
//
//  VoC_shr16_ri(REG5,8,DEFAULT);
//  VoC_shr16_ri(REG2,8,IN_PARALLEL);
//
//  VoC_add16_rr(REG5,REG5,REG1,DEFAULT);
//  VoC_add16_rr(REG2,REG2,REG1,IN_PARALLEL);
//
//  /////////////////////////////////////////////
//  //r  g  b :r3 r4 r6
//  //r1 g1 b1:r7 r5 r2
//  VoC_pop16(REG1,DEFAULT);
//  VoC_shru16_ri(REG3,3,IN_PARALLEL);
//
//  VoC_shru16_ri(REG4,2,DEFAULT);
//  VoC_shru16_ri(REG6,3,IN_PARALLEL);
//
//  VoC_shru16_ri(REG3,-11,DEFAULT);
//  VoC_shru16_ri(REG4,-5,IN_PARALLEL);
//
//  VoC_or16_rr(REG4,REG3,DEFAULT);
//  VoC_shru16_ri(REG7,3,IN_PARALLEL);
//
//  VoC_or16_rr(REG4,REG6,DEFAULT);//first
//  VoC_shru16_ri(REG5,2,IN_PARALLEL);
//
//  VoC_shru16_ri(REG7,-11,DEFAULT);
//  VoC_shru16_ri(REG5,-5,IN_PARALLEL);
//
//  VoC_or16_rr(REG5,REG7,DEFAULT);
//  VoC_shru16_ri(REG2,3,IN_PARALLEL);
//
//  VoC_or16_rr(REG5,REG2,DEFAULT);
//    VoC_lw32_p(REG67,REG0,IN_PARALLEL);
//
//  VoC_shru16_ri(REG6,8,DEFAULT);//u
//  VoC_sw16inc_p(REG4,REG1,IN_PARALLEL);
//
//  VoC_lw32inc_p(REG45,REG0,DEFAULT);
//
//  VoC_sw16inc_p(REG5,REG1,DEFAULT);//VoC_sw32inc_p(REG45,REG1,DEFAULT);
//  VoC_shru16_ri(REG7,8,IN_PARALLEL);//v

    VoC_endloop0

    VoC_pop32(REG01,DEFAULT);
    VoC_lw16i_short(REG4,-1,IN_PARALLEL);

    VoC_add16_rd(REG0,REG0,INPUT_SRC_W_ADDR);

    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);
    VoC_pop16(REG4,IN_PARALLEL);


    VoC_return;
}

void Coolsand_yuv2rgb565_oneline_with_rotateDMA(void)
{
    /*****y1 u y2 v ***********
    fun:
        used to transform yuv format data to r g  b format
    input parameters:
       yuv buf addr: r0
       rgb buf addr: r1
    used regs:

    zouying  added  2008-12-2
    ***************************/
    VoC_lw16i(ACC1_LO,CONST_R_ADDR);//VoC_lw16i(ACC1_LO,128);
    VoC_lw16_d(REG2,INPUT_CUT_W_ADDR);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_shru16_ri(REG2,1,IN_PARALLEL);


    VoC_sub16_rr(REG2,REG2,REG4,DEFAULT);//loop-1
    VoC_movreg16(REG3,REG1,IN_PARALLEL);

    VoC_shr16_ri(REG3,1,DEFAULT);//int addr
    VoC_lw16i_short(INC3,YUV2RGBLINE/2,IN_PARALLEL);

    VoC_lw32_d(REG67,OUTPUT_ADDR_ADDR);//ex addr[byte]//GLOBAL_SCALEY_SIZE_ADDR
    VoC_lw32_d(ACC0,CONST_0XFF00FF_ADDR);

    VoC_push16(REG3,DEFAULT);//inner addr
    VoC_push32(REG67,IN_PARALLEL);//extra-addr


    VoC_push16(REG2,DEFAULT);//loop-1
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_lw16i(ACC1_HI,128);
    VoC_push16(REG6,DEFAULT);

    //beging to process
    VoC_lw32_p(REG67,REG0,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);

    VoC_shru16_ri(REG6,8,DEFAULT);//u
    VoC_shru16_ri(REG7,8,IN_PARALLEL);//v

    VoC_and32_rr(REG45,ACC0,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);

    VoC_bez16_d(Coolsand_sharp_L2,INPUT_RESET_ADDR)

    VoC_shr32_ri(REG45,-2,DEFAULT);
    VoC_movreg32(REG23,REG45,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG4,REG3,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG2,IN_PARALLEL);

    VoC_movreg16(REG1,ACC1_HI,DEFAULT);
    VoC_lw16_p(REG0,REG0,IN_PARALLEL);


    VoC_pop16(REG2,DEFAULT);
    VoC_shru16_ri(REG0,-8,IN_PARALLEL);

    VoC_push16(REG3,DEFAULT);
    VoC_shru16_ri(REG0,8,IN_PARALLEL);


    VoC_sub16_rr(REG4,REG4,REG2,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG0,IN_PARALLEL);


    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_shr16_ri(REG5,1,IN_PARALLEL);


    VoC_sub16_rr(REG4,REG4,REG1,DEFAULT);//-128
    VoC_sub16_rr(REG5,REG5,REG1,IN_PARALLEL);//-128


    VoC_shr16_ri(REG4,-8,DEFAULT);
    VoC_shr16_ri(REG5,-8,IN_PARALLEL);

    VoC_shr16_ri(REG4,8,DEFAULT);
    VoC_shr16_ri(REG5,8,IN_PARALLEL);
Coolsand_sharp_L2:

    //  VoC_shr16_ri(REG1,0,IN_PARALLEL);

    VoC_add32_rr(REG23,REG67,RL6,DEFAULT);//TableV2G[v]    TabU2G[u]
    VoC_add32_rr(REG67,REG67,RL7,IN_PARALLEL);//TabV2R[v]  TabU2B[u]

    VoC_bez16_d(Coolsand_sharp_L3,INPUT_RESET_ADDR)

    VoC_add16_rr(REG4,REG4,REG1,DEFAULT);//+128
    VoC_add16_rr(REG5,REG5,REG1,IN_PARALLEL);//+128
Coolsand_sharp_L3:

    VoC_sub16_pp(REG2,REG2,REG3,DEFAULT);//TabU2G[u] - TableV2G[v]
    VoC_movreg16(REG0,ACC1_LO,IN_PARALLEL);//128

    VoC_add16_rp(REG3,REG4,REG7,DEFAULT);//r = irgb_sat[y + TabV2R[v]];
    VoC_add16_rp(REG7,REG5,REG7,IN_PARALLEL);//r1 = irgb_sat[y + TabV2R[v]];
    VoC_add16_rr(REG4,REG4,REG2,DEFAULT);//g = irgb_sat[y + TabU2G[u] - TableV2G[v]];
    VoC_add16_rp(REG6,REG4,REG6,IN_PARALLEL);//b = irgb_sat[y + TabU2B[u]];
    VoC_add16_rr(REG5,REG5,REG2,DEFAULT);//g1 = irgb_sat[y + TabU2G[u] - TableV2G[v]];
    VoC_add16_rp(REG2,REG5,REG6,IN_PARALLEL);//b1 = irgb_sat[y + TabU2B[u]];

    //r  g  b :r3 r4 r6
    //r1 g1 b1:r7 r5 r2
    VoC_add16inc_rp(REG3,REG3,REG0,DEFAULT);//r
    VoC_add16inc_rp(REG7,REG7,REG0,IN_PARALLEL);
    //VoC_add32_rd(REG45,REG45,CONST_G_ADDR);//Y RAM
    VoC_add16inc_rp(REG4,REG4,REG0,DEFAULT);
    VoC_add16inc_rp(REG5,REG5,REG0,IN_PARALLEL);
    //VoC_add32_rd(REG67,REG67,CONST_B_ADDR);//X RAM
    VoC_add16inc_rp(REG2,REG2,REG0,DEFAULT);
    VoC_add16inc_rp(REG6,REG6,REG0,IN_PARALLEL);

    VoC_lw16_p(REG3,REG3,DEFAULT);//r
    VoC_lw16_p(REG4,REG4,IN_PARALLEL);//g

    VoC_or16_rr(REG4,REG3,DEFAULT);
    VoC_lw16_p(REG6,REG6,IN_PARALLEL);

    VoC_lw16_p(REG7,REG7,DEFAULT);
    VoC_lw16_p(REG5,REG5,IN_PARALLEL);

    VoC_or16_rr(REG4,REG6,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);

    VoC_or16_rr(REG5,REG7,DEFAULT);
    VoC_lw16_p(REG2,REG2,IN_PARALLEL);

    VoC_or16_rr(REG5,REG2,DEFAULT);
    VoC_lw32_p(REG67,REG0,IN_PARALLEL);


    VoC_shru16_ri(REG7,8,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);

    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG5,REG1,DEFAULT);

    VoC_pop16(ACC1_HI,DEFAULT);

    ///////////////////DMA////////////////////////////////////
    VoC_lw16_d(REG2,GLOBAL_BLEND_DMASIZE_ADDR);//int
    VoC_lw16_sd(REG3,1,DEFAULT);//inner addr  int
    VoC_push32(RL7,IN_PARALLEL);

    VoC_lw32_sd(RL7,1,DEFAULT);//extra addr  byte
    VoC_pop16(REG2,IN_PARALLEL);//loop - 1

    VoC_sw32_d(REG23,CFG_DMA_SIZE);
    VoC_sw32_d(RL7,CFG_DMA_EADDR);


    VoC_add32_rd(RL7,RL7,GLOBAL_P1_ADDR);//dec_h
    VoC_inc_p(REG3,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);
    VoC_sw16_sd(REG3,0,DEFAULT);
    VoC_sw32_sd(RL7,0,IN_PARALLEL);
    ///////////////////////////////////////////////////////


    VoC_push16(ACC1_HI,DEFAULT);


    VoC_loop_r(0,REG2);

    //VoC_lw32_p(REG67,REG0,DEFAULT);
    //VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);
    //r4:y1 r5:y2 r6:u r7:v
    //VoC_shru16_ri(REG6,8,DEFAULT);//u
    //VoC_shru16_ri(REG7,8,IN_PARALLEL);//v

    VoC_lw16i(ACC1_HI,128);
    VoC_and32_rr(REG45,ACC0,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);

    VoC_bez16_d(Coolsand_sharp_L4,INPUT_RESET_ADDR)

    VoC_shr32_ri(REG45,-2,DEFAULT);
    VoC_movreg32(REG23,REG45,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG4,REG3,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG2,IN_PARALLEL);

    VoC_movreg16(REG1,ACC1_HI,DEFAULT);
    VoC_lw16_p(REG0,REG0,IN_PARALLEL);

    VoC_pop16(REG2,DEFAULT);
    VoC_shru16_ri(REG0,-8,IN_PARALLEL);

    VoC_push16(REG3,DEFAULT);
    VoC_shru16_ri(REG0,8,IN_PARALLEL);


    VoC_sub16_rr(REG4,REG4,REG2,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG0,IN_PARALLEL);


    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_shr16_ri(REG5,1,IN_PARALLEL);


    VoC_sub16_rr(REG4,REG4,REG1,DEFAULT);//-128
    VoC_sub16_rr(REG5,REG5,REG1,IN_PARALLEL);//-128

    VoC_shr16_ri(REG4,-8,DEFAULT);
    VoC_shr16_ri(REG5,-8,IN_PARALLEL);

    VoC_shr16_ri(REG4,8,DEFAULT);
    VoC_shr16_ri(REG5,8,IN_PARALLEL);
Coolsand_sharp_L4:

    //  VoC_shr16_ri(REG1,0,IN_PARALLEL);

    VoC_add32_rr(REG23,REG67,RL6,DEFAULT);//TableV2G[v]    TabU2G[u]
    VoC_add32_rr(REG67,REG67,RL7,IN_PARALLEL);//TabV2R[v]  TabU2B[u]

    VoC_bez16_d(Coolsand_sharp_L5,INPUT_RESET_ADDR)

    VoC_add16_rr(REG4,REG4,REG1,DEFAULT);//+128
    VoC_add16_rr(REG5,REG5,REG1,IN_PARALLEL);//+128
Coolsand_sharp_L5:

    VoC_sub16_pp(REG2,REG2,REG3,DEFAULT);//TabU2G[u] - TableV2G[v]
    VoC_movreg16(REG0,ACC1_LO,IN_PARALLEL);//128

    VoC_add16_rp(REG3,REG4,REG7,DEFAULT);//r = irgb_sat[y + TabV2R[v]];
    VoC_add16_rp(REG7,REG5,REG7,IN_PARALLEL);//r1 = irgb_sat[y + TabV2R[v]];
    VoC_add16_rr(REG4,REG4,REG2,DEFAULT);//g = irgb_sat[y + TabU2G[u] - TableV2G[v]];
    VoC_add16_rp(REG6,REG4,REG6,IN_PARALLEL);//b = irgb_sat[y + TabU2B[u]];
    VoC_add16_rr(REG5,REG5,REG2,DEFAULT);//g1 = irgb_sat[y + TabU2G[u] - TableV2G[v]];
    VoC_add16_rp(REG2,REG5,REG6,IN_PARALLEL);//b1 = irgb_sat[y + TabU2B[u]];

    //r  g  b :r3 r4 r6
    //r1 g1 b1:r7 r5 r2
    VoC_add16inc_rp(REG3,REG3,REG0,DEFAULT);//r
    VoC_add16inc_rp(REG7,REG7,REG0,IN_PARALLEL);
    //VoC_add32_rd(REG45,REG45,CONST_G_ADDR);//Y RAM
    VoC_add16inc_rp(REG4,REG4,REG0,DEFAULT);
    VoC_add16inc_rp(REG5,REG5,REG0,IN_PARALLEL);
    //VoC_add32_rd(REG67,REG67,CONST_B_ADDR);//X RAM
    VoC_add16inc_rp(REG2,REG2,REG0,DEFAULT);
    VoC_add16inc_rp(REG6,REG6,REG0,IN_PARALLEL);

    VoC_lw16_p(REG3,REG3,DEFAULT);//r
    VoC_lw16_p(REG4,REG4,IN_PARALLEL);//g

    VoC_or16_rr(REG4,REG3,DEFAULT);
    VoC_lw16_p(REG5,REG5,IN_PARALLEL);

    VoC_lw16_p(REG6,REG6,DEFAULT);
    ///////////////////DMA////////////////////////////////////
    VoC_push32(RL7,IN_PARALLEL);

    VoC_lw32_sd(RL7,2,DEFAULT);//extra addr
    VoC_lw16_sd(REG3,1,IN_PARALLEL);//inner addr

    VoC_lw16_d(ACC1_HI,GLOBAL_BLEND_DMASIZE_ADDR);

    //Check out DMA status
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_sw16_d(ACC1_HI,CFG_DMA_SIZE);
    VoC_sw16_d(REG3,CFG_DMA_LADDR);
    VoC_sw32_d(RL7,CFG_DMA_EADDR);

    VoC_add32_rd(RL7,RL7,GLOBAL_P1_ADDR);

    VoC_inc_p(REG3,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);

    VoC_sw32_sd(RL7,1,DEFAULT);
    //VoC_sw16_sd(REG3,1,DEFAULT);
    ///////////////////////////////////////////////////////
    VoC_lw16_p(REG7,REG7,IN_PARALLEL);


    VoC_or16_rr(REG4,REG6,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);

    VoC_or16_rr(REG5,REG7,DEFAULT);
    VoC_lw16_p(REG2,REG2,IN_PARALLEL);

    VoC_or16_rr(REG5,REG2,DEFAULT);
    VoC_lw32_p(REG67,REG0,IN_PARALLEL);


    VoC_shru16_ri(REG7,8,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);

    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG5,REG1,DEFAULT);




    ///////////////////DMA////////////////////////////////////
    VoC_lw16_d(REG2,GLOBAL_BLEND_DMASIZE_ADDR);
    //VoC_lw16_sd(REG3,1,DEFAULT);
    VoC_lw32_sd(RL7,0,DEFAULT);//extra addr

    VoC_push32(RL7,DEFAULT);
    //Check out DMA status
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_sw32_d(REG23,CFG_DMA_SIZE);
    VoC_sw32_d(RL7,CFG_DMA_EADDR);


    VoC_add32_rd(RL7,RL7,GLOBAL_P1_ADDR);
    VoC_inc_p(REG3,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);
    VoC_sw16_sd(REG3,1,DEFAULT);
    VoC_sw32_sd(RL7,0,IN_PARALLEL);
    ///////////////////////////////////////////////////////

    VoC_endloop0

    VoC_pop16(REG2,DEFAULT);

    ///////////////////DMA////////////////////////////////////
    VoC_lw16_d(REG2,GLOBAL_BLEND_DMASIZE_ADDR);
    VoC_pop16(REG3,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);//extra addr
    //VoC_lw16_sd(REG3,1,DEFAULT);



    //Check out DMA status
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_sw32_d(REG23,CFG_DMA_SIZE);
    VoC_sw32_d(REG67,CFG_DMA_EADDR);

    //////////////////update/////////////////////////////////////
    VoC_lw32_d(REG45,OUTPUT_ADDR_ADDR);//GLOBAL_SCALEY_SIZE_ADDR
    VoC_sub32_rd(REG45,REG45,GLOBAL_BLEND_DMAINC_ADDR);

    VoC_NOP();

    VoC_sw32_d(REG45,OUTPUT_ADDR_ADDR);//GLOBAL_SCALEY_SIZE_ADDR

    //Check out DMA status
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_return;
}
/************************************************************************/
/* zoom out                                                             */
/************************************************************************/
void Coolsand_bilinear_preview_yuv_zoomout(void)
{
    //int i=0;
    VoC_push16(RA,DEFAULT);
    VoC_lw32z(REG67,DEFAULT);//w
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    VoC_lw16i(RL6_LO,CONST_TabU2G_ADDR);
    VoC_lw16i(RL6_HI,CONST_TableV2G_ADDR);

    VoC_push16(REG7,DEFAULT);//extra
    //judge the dma's condition
//  VoC_cfg(CFG_CTRL_STALL);//VoC_cfg(CFG_CTRL_CONTINUE);
//  VoC_NOP();
//    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);


LABLE_CUTZOOM_H_2:
    VoC_lw16_d(REG4,INPUT_DEC_H_ADDR);
    VoC_shru16_ri(REG4,1,DEFAULT);//h/2

    VoC_sw16_d(REG5,INPUT_IN_MODE_ADDR);
    VoC_loop_r(1,REG4);//for (j=0;j<dec_h;j++)

    VoC_shru32_ri(REG67,S15,DEFAULT);//num1 = (w>>S15);
    VoC_lw16_sd(REG4,0,IN_PARALLEL);//extra
    VoC_push32(REG67,DEFAULT);//save w
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_lw16_d(REG7,GLOBAL_NUM1_ADDR);
    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);//num1 = (w>>S15) - extra;
    VoC_blt16_rd(LABLE_PREVIEW_L0,REG6,GLOBAL_INLINE_NUM_1_ADDR);//if (num1>=numline-1)
    VoC_jal(COOLSAND_DMA_READ);//memcpy(bufin,pin,numcount);//pin+=(numline-1)*src_w;
    //VoC_lw32_sd(REG45,0,DEFAULT);
    //VoC_and32_rd(REG45,GLOBAL_MASK_ADDR);
    VoC_lw16_d(REG5,GLOBAL_INLINE_NUM_1_ADDR);
    VoC_lw16_sd(REG4,0,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG5,IN_PARALLEL);//num1-=numline-1;
    VoC_sub16_rr(REG7,REG7,REG5,DEFAULT);//VoC_NOP();
    VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);//extra+=numline-1;
    VoC_NOP();
    VoC_sw16_sd(REG4,0,DEFAULT);
    //VoC_sw32_sd(REG45,0,DEFAULT);
LABLE_PREVIEW_L0:
    VoC_jal(COOLSAND_DMA_READ_BlendBuf);//add blend

    VoC_mult16_rd(REG4,REG6,INPUT_SRC_W_ADDR);//p3 = pa + (num1*src_w);
    VoC_sub16_rr(REG7,REG6,REG7,DEFAULT);//VoC_sub16_rd(REG7,REG6,GLOBAL_NUM1_ADDR);//num1-num2
    VoC_sw16_d(REG6,GLOBAL_NUM1_ADDR);
    VoC_add16_rd(REG4,REG4,GLOBAL_DEAD_UX_ADDR);//P3
    VoC_add16_rd(REG5,REG4,INPUT_SRC_W_ADDR);//p4 = p3 + src_w;

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);//VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_bne16_rr(LABLE_PREVIEW_L1,REG7,REG6)//if (num1-num2==1)

    VoC_bnez16_d(LABLE_PREVIEW_L3,GLOBAL_NUM3_ADDR);//if (num3==0)
    VoC_lw16i_set_inc(REG3,GLOBAL_TEMP_BUF_ADDR,2);//ptmp2    p2 = tmpbuf;
    VoC_lw16d_set_inc(REG2,GLOBAL_TEMP_BUF2_ADDR,1);//tmpbuf   p1 = ptmp2;
    VoC_sw16_d(REG6,GLOBAL_NUM3_ADDR);//num3=1;
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);
    VoC_movreg16(REG2,REG3,DEFAULT);
    VoC_movreg16(REG0,REG5,IN_PARALLEL);//P4
    VoC_jal(Coolsand_zoomline_yuv);//zoom_line(p4,tmpbuf,dec_w,scalex);
    VoC_jump(LABLE_PREVIEW_L2);
LABLE_PREVIEW_L3://else
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16d_set_inc(REG3,GLOBAL_TEMP_BUF2_ADDR,2);//ptmp2  p2 = ptmp2;
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_BUF_ADDR,1);//tmpbuf   p1 = tmpbuf;
    VoC_sw16_d(REG6,GLOBAL_NUM3_ADDR);//num3 = 0;
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);
    VoC_movreg16(REG2,REG3,DEFAULT);//ptmp2
    VoC_movreg16(REG0,REG5,IN_PARALLEL);//P4
    VoC_jal(Coolsand_zoomline_yuv);//zoom_line(p4,ptmp2,dec_w,scalex);
    VoC_jump(LABLE_PREVIEW_L2);
LABLE_PREVIEW_L1:
    VoC_bez16_r(LABLE_PREVIEW_L2,REG7);//else if (num1-num2!=0)
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16d_set_inc(REG3,GLOBAL_TEMP_BUF2_ADDR,2);//ptmp2  p2 = ptmp2;
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_BUF_ADDR,1);//tmpbuf   p1 = tmpbuf;
    VoC_sw16_d(REG6,GLOBAL_NUM3_ADDR);//num3 = 0;
    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_push16(REG5,IN_PARALLEL);
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);
    VoC_jal(Coolsand_zoomline_yuv);//zoom_line(p3,tmpbuf,dec_w,scalex);
    VoC_pop16(REG0,DEFAULT);
    VoC_lw16_d(REG2,GLOBAL_TEMP_BUF2_ADDR);
    VoC_jal(Coolsand_zoomline_yuv);//zoom_line(p4,ptmp2,dec_w,scalex);
LABLE_PREVIEW_L2:

    VoC_jal(COOLSAND_DMA_WRITE);

    VoC_lw16_d(REG4,INPUT_DEC_W_ADDR);
    VoC_lw16i_set_inc(REG3,Q15,2);

    VoC_lw32_sd(REG67,0,DEFAULT);//w
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_and32_rd(REG67,GLOBAL_MASK_ADDR);//wb = w&MASK;
    VoC_sub16_rr(REG7,REG3,REG6,DEFAULT);//wa = Q15 - wb;
    VoC_push16(REG1,IN_PARALLEL);//pb

    VoC_lw32_d(REG23,GLOBAL_P1_ADDR);//p3 = p1;//p4 = p2;
    VoC_add16_rd(REG0,REG2,INPUT_DEC_W_ADDR);//P5
    VoC_add16_rd(REG1,REG3,INPUT_DEC_W_ADDR);//P6
    //VoC_lw16i(RL6_LO,CONST_TabU2G_ADDR);
    //VoC_lw16i(RL6_HI,CONST_TableV2G_ADDR);
    VoC_lw16i(RL7_HI,CONST_TabV2R_ADDR);
    VoC_lw16i(RL7_LO,CONST_TabU2B_ADDR);
    //VoC_lw16i(REG5,128);
    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);


    VoC_push32(REG67,DEFAULT);//wa wb
    //VoC_push16(REG5,IN_PARALLEL);//128
    VoC_loop_r(0,REG4);//for (i=0;i<dec_w/2;i++)
    //if (i==106)
    //{
    //  printf("stop");
    //}
    //i+=2;
    VoC_multf32inc_rp(ACC0,REG7,REG0,DEFAULT);     //u = *p3++;y2 = *p4++;y1 = ((y1*wa + y2*wb)>>S15);
    VoC_multf32inc_rp(ACC1,REG7,REG0,IN_PARALLEL); //v = *p5++;v1 = *p6++;v = ((v*wa + v1*wb)>>S15);

    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_mac32inc_rp(REG6,REG1,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG7,REG2,DEFAULT);    //y = *p3++;y1 = *p4++;y = ((y*wa + y1*wb)>>S15);
    VoC_multf32inc_rp(ACC1,REG7,REG2,IN_PARALLEL);   //y1 = *p5++;u1 = *p6++;u = ((u*wa + u1*wb)>>S15);

    VoC_movreg16(REG4,ACC0_HI,DEFAULT);//R4:u
    VoC_movreg16(REG5,ACC1_HI,IN_PARALLEL);//R5:v

    VoC_macXinc_rp(REG6,REG3,DEFAULT);
    VoC_mac32inc_rp(REG6,REG3,IN_PARALLEL);

    VoC_add32_rr(REG45,REG45,RL6,DEFAULT); //TabU2G[u] ADDR  //gTableV2G[v] ADDR
    VoC_add32_rr(REG67,REG45,RL7,IN_PARALLEL);  //TabU2B[u] ADDR      //TabV2R[v] ADDR

    VoC_movreg16(REG0,ACC0_HI,DEFAULT);//y
    VoC_push32(REG23,IN_PARALLEL);

    VoC_push32(REG01,DEFAULT);
    VoC_movreg16(REG2,ACC1_HI,IN_PARALLEL);//y1
    //VoC_lw16_p(REG4,REG4,DEFAULT);
    VoC_sub16_pp(REG4,REG4,REG5,DEFAULT);//TabU2G[u] - gTableV2G[v]
    VoC_lw16i(REG1,CONST_R_ADDR);

    VoC_add16_rr(REG4,REG0,REG4,DEFAULT);//g
    VoC_add16_rr(REG5,REG2,REG4,IN_PARALLEL);//g1

    VoC_add16_rp(REG6,REG0,REG7,DEFAULT);//r = irgb_sat[y + TabV2R[v]];
    VoC_add16_rp(REG7,REG2,REG7,IN_PARALLEL);//r1

    VoC_add16_rp(REG2,REG0,REG6,DEFAULT);//b
    VoC_add16_rp(REG3,REG2,REG6,IN_PARALLEL);//b1


    //VoC_add32_rd(REG23,REG23,CONST_R_ADDR);//X RAM
    VoC_add16inc_rp(REG6,REG6,REG1,DEFAULT);
    VoC_add16inc_rp(REG7,REG7,REG1,IN_PARALLEL);
    //VoC_add32_rd(REG45,REG45,CONST_G_ADDR);//Y RAM
    VoC_add16inc_rp(REG4,REG4,REG1,DEFAULT);
    VoC_add16inc_rp(REG5,REG5,REG1,IN_PARALLEL);
    //VoC_add32_rd(REG67,REG67,CONST_B_ADDR);//X RAM
    VoC_add16inc_rp(REG2,REG2,REG1,DEFAULT);
    VoC_add16inc_rp(REG3,REG3,REG1,IN_PARALLEL);

    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_lw16_p(REG4,REG4,IN_PARALLEL);

    VoC_or16_rr(REG6,REG4,DEFAULT);
    VoC_lw16_p(REG2,REG2,IN_PARALLEL);

    VoC_or16_rr(REG6,REG2,DEFAULT);
    VoC_lw16_p(REG3,REG3,IN_PARALLEL);

    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_lw16_p(REG7,REG7,IN_PARALLEL);

    VoC_or16_rr(REG7,REG3,DEFAULT);
    VoC_lw16_sd(REG2,0,IN_PARALLEL);//pb

    VoC_or16_rr(REG7,REG5,DEFAULT);

    VoC_lw32_p(REG45,REG2,DEFAULT);
    VoC_bez16_r(AverageTwoLine_blend3,REG4);
    VoC_movreg16(REG6,REG4,DEFAULT);
AverageTwoLine_blend3:
    VoC_bez16_r(AverageTwoLine_blend4,REG5);
    VoC_movreg16(REG7,REG5,DEFAULT);
AverageTwoLine_blend4:

    VoC_lw32_sd(REG67,2,DEFAULT);//wa wb

    VoC_sw32inc_p(REG67,REG2,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);

    VoC_sw16_sd(REG2,0,DEFAULT);
    VoC_pop32(REG23,IN_PARALLEL);

    VoC_endloop0

    //VoC_pop16(REG5,DEFAULT);//128
    VoC_pop32(REG67,DEFAULT);//wa wb

    VoC_pop32(REG67,DEFAULT);//w+=scaley;
    VoC_pop16(REG1,IN_PARALLEL);

    ///////judge wether to dma out data & read back data in//////////////
//             VoC_bne16_rd(LABLE_CUTZOOM_DMA_BLENDBUF,REG1,GLOBAL_DEAD_OUT_LINE_ADDR);
//             VoC_bnez16_d(LABLE_BLITMODE1,INPUT_BLIT_MODE_ADDR);
//             VoC_jal(COOLSAND_DMA_WRITE);
//           VoC_jump(LABLE_BLITMODE_END);
//LABLE_BLITMODE1:
//           VoC_jal(Coolsand_DMAWRITE_sb33bf);
//LABLE_BLITMODE_END:
//           VoC_jal(COOLSAND_DMA_WRITE);
//           VoC_jal(COOLSAND_DMA_READ_BlendBuf);//add blend

//LABLE_CUTZOOM_DMA_BLENDBUF:

    VoC_add32_rd(REG67,REG67,GLOBAL_SCALEY_SIZE_ADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_endloop1

    VoC_lw16_d(REG5,INPUT_IN_MODE_ADDR);
    VoC_sub16_rd(REG5,REG5,CONST_1_ADDR);
    VoC_bltz16_r(LABLE_CUTZOOM_GOON,REG5);
    VoC_jump(LABLE_CUTZOOM_H_2);
LABLE_CUTZOOM_GOON:

    //dma out the last data
//           VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR)
//           VoC_be16_rr(LABLE_PREVIEW_L7,REG1,REG5);
////             VoC_bnez16_d(LABLE_Last_BLITMODE1,INPUT_BLIT_MODE_ADDR);
////             VoC_jal(Coolsand_WriteLast);
////             VoC_jump(LABLE_PREVIEW_L7);
////LABLE_Last_BLITMODE1:
////             VoC_jal(Coolsand_WriteLast_sb33bf_blitLCD);
//           VoC_jal(Coolsand_WriteLast);
//LABLE_PREVIEW_L7:
    //VoC_jal(COOLSAND_DMA_WRITE);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_xor16_rd(REG4,GLOBAL_FLAG_WRITE_ADDR);
    VoC_bnez16_d(LABLE_FLAG_WRITE1_last1,GLOBAL_FLAG_WRITE_ADDR);
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR/2);
    VoC_jump(LABLE_FLAG_WRITE2_last1);
LABLE_FLAG_WRITE1_last1:
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR2/2);
LABLE_FLAG_WRITE2_last1:
    VoC_sw16_d(REG4,GLOBAL_FLAG_WRITE_ADDR);

    VoC_lw16_d(REG4,GLOBAL_BLEND_DMASIZE_ADDR);
    VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);//ex addr[byte]

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    ///////////////////////////////////////////////

    VoC_pop16(REG4,DEFAULT);
    VoC_pop16(RA,DEFAULT);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_return;


}

void Coolsand_bilinear_preview_yuv_zoomout_withoutblend(void)
{
    //int i=0;
    VoC_push16(RA,DEFAULT);
    VoC_lw32z(REG67,DEFAULT);//w
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    VoC_lw16i(RL6_LO,CONST_TabU2G_ADDR);
    VoC_lw16i(RL6_HI,CONST_TableV2G_ADDR);

    VoC_push16(REG7,DEFAULT);//extra
    //judge the dma's condition
//  VoC_cfg(CFG_CTRL_STALL);//VoC_cfg(CFG_CTRL_CONTINUE);
//  VoC_NOP();
//    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);


LABLE_CUTZOOM_H_2_withoutblend:
    VoC_lw16_d(REG4,INPUT_DEC_H_ADDR);
    VoC_shru16_ri(REG4,1,DEFAULT);//h/2

    VoC_sw16_d(REG5,INPUT_IN_MODE_ADDR);
    VoC_loop_r(1,REG4);//for (j=0;j<dec_h;j++)

    VoC_shru32_ri(REG67,S15,DEFAULT);//num1 = (w>>S15);
    VoC_lw16_sd(REG4,0,IN_PARALLEL);//extra
    VoC_push32(REG67,DEFAULT);//save w
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_lw16_d(REG7,GLOBAL_NUM1_ADDR);
    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);//num1 = (w>>S15) - extra;
    VoC_blt16_rd(LABLE_PREVIEW_L0_withoutblend,REG6,GLOBAL_INLINE_NUM_1_ADDR);//if (num1>=numline-1)
    VoC_jal(COOLSAND_DMA_READ);//memcpy(bufin,pin,numcount);//pin+=(numline-1)*src_w;
    //VoC_lw32_sd(REG45,0,DEFAULT);
    //VoC_and32_rd(REG45,GLOBAL_MASK_ADDR);
    VoC_lw16_d(REG5,GLOBAL_INLINE_NUM_1_ADDR);
    VoC_lw16_sd(REG4,0,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG5,IN_PARALLEL);//num1-=numline-1;
    VoC_sub16_rr(REG7,REG7,REG5,DEFAULT);//VoC_NOP();
    VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);//extra+=numline-1;
    VoC_NOP();
    VoC_sw16_sd(REG4,0,DEFAULT);
    //VoC_sw32_sd(REG45,0,DEFAULT);
LABLE_PREVIEW_L0_withoutblend:
    VoC_jal(COOLSAND_DMA_WRITE_withoutblend);

    VoC_mult16_rd(REG4,REG6,INPUT_SRC_W_ADDR);//p3 = pa + (num1*src_w);
    VoC_sub16_rr(REG7,REG6,REG7,DEFAULT);//VoC_sub16_rd(REG7,REG6,GLOBAL_NUM1_ADDR);//num1-num2
    VoC_sw16_d(REG6,GLOBAL_NUM1_ADDR);
    VoC_add16_rd(REG4,REG4,GLOBAL_DEAD_UX_ADDR);//P3
    VoC_add16_rd(REG5,REG4,INPUT_SRC_W_ADDR);//p4 = p3 + src_w;

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);//VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_bne16_rr(LABLE_PREVIEW_L1_withoutblend,REG7,REG6)//if (num1-num2==1)

    VoC_bnez16_d(LABLE_PREVIEW_L3_withoutblend,GLOBAL_NUM3_ADDR);//if (num3==0)
    VoC_lw16i_set_inc(REG3,GLOBAL_TEMP_BUF_ADDR,2);//ptmp2    p2 = tmpbuf;
    VoC_lw16d_set_inc(REG2,GLOBAL_TEMP_BUF2_ADDR,1);//tmpbuf   p1 = ptmp2;
    VoC_sw16_d(REG6,GLOBAL_NUM3_ADDR);//num3=1;
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);
    VoC_movreg16(REG2,REG3,DEFAULT);
    VoC_movreg16(REG0,REG5,IN_PARALLEL);//P4
    VoC_jal(Coolsand_zoomline_yuv);//zoom_line(p4,tmpbuf,dec_w,scalex);
    VoC_jump(LABLE_PREVIEW_L2_withoutblend);
LABLE_PREVIEW_L3_withoutblend://else
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16d_set_inc(REG3,GLOBAL_TEMP_BUF2_ADDR,2);//ptmp2  p2 = ptmp2;
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_BUF_ADDR,1);//tmpbuf   p1 = tmpbuf;
    VoC_sw16_d(REG6,GLOBAL_NUM3_ADDR);//num3 = 0;
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);
    VoC_movreg16(REG2,REG3,DEFAULT);//ptmp2
    VoC_movreg16(REG0,REG5,IN_PARALLEL);//P4
    VoC_jal(Coolsand_zoomline_yuv);//zoom_line(p4,ptmp2,dec_w,scalex);
    VoC_jump(LABLE_PREVIEW_L2_withoutblend);
LABLE_PREVIEW_L1_withoutblend:
    VoC_bez16_r(LABLE_PREVIEW_L2_withoutblend,REG7);//else if (num1-num2!=0)
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16d_set_inc(REG3,GLOBAL_TEMP_BUF2_ADDR,2);//ptmp2  p2 = ptmp2;
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_BUF_ADDR,1);//tmpbuf   p1 = tmpbuf;
    VoC_sw16_d(REG6,GLOBAL_NUM3_ADDR);//num3 = 0;
    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_push16(REG5,IN_PARALLEL);
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);
    VoC_jal(Coolsand_zoomline_yuv);//zoom_line(p3,tmpbuf,dec_w,scalex);
    VoC_pop16(REG0,DEFAULT);
    VoC_lw16_d(REG2,GLOBAL_TEMP_BUF2_ADDR);
    VoC_jal(Coolsand_zoomline_yuv);//zoom_line(p4,ptmp2,dec_w,scalex);
LABLE_PREVIEW_L2_withoutblend:



    VoC_lw16_d(REG4,INPUT_DEC_W_ADDR);
    VoC_lw16i_set_inc(REG3,Q15,2);

    VoC_lw32_sd(REG67,0,DEFAULT);//w
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_and32_rd(REG67,GLOBAL_MASK_ADDR);//wb = w&MASK;
    VoC_sub16_rr(REG7,REG3,REG6,DEFAULT);//wa = Q15 - wb;
    VoC_push16(REG1,IN_PARALLEL);//pb

    VoC_lw32_d(REG23,GLOBAL_P1_ADDR);//p3 = p1;//p4 = p2;
    VoC_add16_rd(REG0,REG2,INPUT_DEC_W_ADDR);//P5
    VoC_add16_rd(REG1,REG3,INPUT_DEC_W_ADDR);//P6
    //VoC_lw16i(RL6_LO,CONST_TabU2G_ADDR);
    //VoC_lw16i(RL6_HI,CONST_TableV2G_ADDR);
    VoC_lw16i(RL7_HI,CONST_TabV2R_ADDR);
    VoC_lw16i(RL7_LO,CONST_TabU2B_ADDR);
    //VoC_lw16i(REG5,128);
    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);


    VoC_push32(REG67,DEFAULT);//wa wb
    //VoC_push16(REG5,IN_PARALLEL);//128
    VoC_loop_r(0,REG4);//for (i=0;i<dec_w/2;i++)
    //if (i==106)
    //{
    //  printf("stop");
    //}
    //i+=2;
    VoC_multf32inc_rp(ACC0,REG7,REG0,DEFAULT);     //u = *p3++;y2 = *p4++;y1 = ((y1*wa + y2*wb)>>S15);
    VoC_multf32inc_rp(ACC1,REG7,REG0,IN_PARALLEL); //v = *p5++;v1 = *p6++;v = ((v*wa + v1*wb)>>S15);

    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_mac32inc_rp(REG6,REG1,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG7,REG2,DEFAULT);    //y = *p3++;y1 = *p4++;y = ((y*wa + y1*wb)>>S15);
    VoC_multf32inc_rp(ACC1,REG7,REG2,IN_PARALLEL);   //y1 = *p5++;u1 = *p6++;u = ((u*wa + u1*wb)>>S15);

    VoC_movreg16(REG4,ACC0_HI,DEFAULT);//R4:u
    VoC_movreg16(REG5,ACC1_HI,IN_PARALLEL);//R5:v

    VoC_macXinc_rp(REG6,REG3,DEFAULT);
    VoC_mac32inc_rp(REG6,REG3,IN_PARALLEL);

    VoC_add32_rr(REG45,REG45,RL6,DEFAULT); //TabU2G[u] ADDR  //gTableV2G[v] ADDR
    VoC_add32_rr(REG67,REG45,RL7,IN_PARALLEL);  //TabU2B[u] ADDR      //TabV2R[v] ADDR

    VoC_movreg16(REG0,ACC0_HI,DEFAULT);//y
    VoC_push32(REG23,IN_PARALLEL);

    VoC_push32(REG01,DEFAULT);
    VoC_movreg16(REG2,ACC1_HI,IN_PARALLEL);//y1
    //VoC_lw16_p(REG4,REG4,DEFAULT);
    VoC_sub16_pp(REG4,REG4,REG5,DEFAULT);//TabU2G[u] - gTableV2G[v]
    VoC_lw16i(REG1,CONST_R_ADDR);

    VoC_add16_rr(REG4,REG0,REG4,DEFAULT);//g
    VoC_add16_rr(REG5,REG2,REG4,IN_PARALLEL);//g1

    VoC_add16_rp(REG6,REG0,REG7,DEFAULT);//r = irgb_sat[y + TabV2R[v]];
    VoC_add16_rp(REG7,REG2,REG7,IN_PARALLEL);//r1

    VoC_add16_rp(REG2,REG0,REG6,DEFAULT);//b
    VoC_add16_rp(REG3,REG2,REG6,IN_PARALLEL);//b1


    //VoC_add32_rd(REG23,REG23,CONST_R_ADDR);//X RAM
    VoC_add16inc_rp(REG6,REG6,REG1,DEFAULT);
    VoC_add16inc_rp(REG7,REG7,REG1,IN_PARALLEL);
    //VoC_add32_rd(REG45,REG45,CONST_G_ADDR);//Y RAM
    VoC_add16inc_rp(REG4,REG4,REG1,DEFAULT);
    VoC_add16inc_rp(REG5,REG5,REG1,IN_PARALLEL);
    //VoC_add32_rd(REG67,REG67,CONST_B_ADDR);//X RAM
    VoC_add16inc_rp(REG2,REG2,REG1,DEFAULT);
    VoC_add16inc_rp(REG3,REG3,REG1,IN_PARALLEL);

    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_lw16_p(REG4,REG4,IN_PARALLEL);

    VoC_or16_rr(REG6,REG4,DEFAULT);
    VoC_lw16_p(REG2,REG2,IN_PARALLEL);

    VoC_or16_rr(REG6,REG2,DEFAULT);
    VoC_lw16_p(REG3,REG3,IN_PARALLEL);

    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_lw16_p(REG7,REG7,IN_PARALLEL);

    VoC_or16_rr(REG7,REG3,DEFAULT);
    VoC_lw16_sd(REG2,0,IN_PARALLEL);//pb

    VoC_or16_rr(REG7,REG5,DEFAULT);

    VoC_lw32_sd(REG67,2,DEFAULT);//wa wb

    VoC_sw32inc_p(REG67,REG2,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);

    VoC_sw16_sd(REG2,0,DEFAULT);
    VoC_pop32(REG23,IN_PARALLEL);

    VoC_endloop0

    //VoC_pop16(REG5,DEFAULT);//128
    VoC_pop32(REG67,DEFAULT);//wa wb

    VoC_pop32(REG67,DEFAULT);//w+=scaley;
    VoC_pop16(REG1,IN_PARALLEL);

    ///////judge wether to dma out data & read back data in//////////////
//             VoC_bne16_rd(LABLE_CUTZOOM_DMA_BLENDBUF,REG1,GLOBAL_DEAD_OUT_LINE_ADDR);
//             VoC_bnez16_d(LABLE_BLITMODE1,INPUT_BLIT_MODE_ADDR);
//             VoC_jal(COOLSAND_DMA_WRITE);
//           VoC_jump(LABLE_BLITMODE_END);
//LABLE_BLITMODE1:
//           VoC_jal(Coolsand_DMAWRITE_sb33bf);
//LABLE_BLITMODE_END:
//           VoC_jal(COOLSAND_DMA_WRITE);
//           VoC_jal(COOLSAND_DMA_READ_BlendBuf);//add blend

//LABLE_CUTZOOM_DMA_BLENDBUF:

    VoC_add32_rd(REG67,REG67,GLOBAL_SCALEY_SIZE_ADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_endloop1

    VoC_lw16_d(REG5,INPUT_IN_MODE_ADDR);
    VoC_sub16_rd(REG5,REG5,CONST_1_ADDR);
    VoC_bltz16_r(LABLE_CUTZOOM_GOON_withoutblend,REG5);
    VoC_jump(LABLE_CUTZOOM_H_2_withoutblend);
LABLE_CUTZOOM_GOON_withoutblend:

    //dma out the last data
//           VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR)
//           VoC_be16_rr(LABLE_PREVIEW_L7,REG1,REG5);
////             VoC_bnez16_d(LABLE_Last_BLITMODE1,INPUT_BLIT_MODE_ADDR);
////             VoC_jal(Coolsand_WriteLast);
////             VoC_jump(LABLE_PREVIEW_L7);
////LABLE_Last_BLITMODE1:
////             VoC_jal(Coolsand_WriteLast_sb33bf_blitLCD);
//           VoC_jal(Coolsand_WriteLast);
//LABLE_PREVIEW_L7:
    VoC_jal(COOLSAND_DMA_WRITE_withoutblend);
    ///////////////////////////////////////////////

    VoC_pop16(REG4,DEFAULT);
    VoC_pop16(RA,DEFAULT);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_return;


}
//zoom out
void Coolsand_zoomline_yuv(void)
{
    //R0: IN BUF  inc0=2  inc1=1
    //R2: OUT BUF
    //used buf: all
    VoC_lw16i(REG3,Q15);
    VoC_lw32z(REG67,DEFAULT);//wn = 0;
    VoC_push16(REG1,IN_PARALLEL);//[2]
    VoC_push16(REG3,DEFAULT);//q15[1]
    VoC_push32(RL6,IN_PARALLEL);
    VoC_lw16_d(REG3,INPUT_DEC_W_ADDR);
    VoC_shru16_ri(REG3,1,DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);
    VoC_add16_rd(REG1,REG2,INPUT_DEC_W_ADDR);//u v addr
    /////////////loop1/////////////////////////////////////////
    VoC_push16(REG3,DEFAULT);//dec_w/2  [0]
    VoC_lw32z(REG45,IN_PARALLEL);//wn

    VoC_loop_r(0,REG3);//for (i=0;i<dec_w/2;i++)

    VoC_shru16_ri(REG4,-1,DEFAULT);//w<<1
    VoC_movreg16(REG5,REG4,IN_PARALLEL);//w
    /////////u v//////////
    VoC_movreg16(REG1,REG0,DEFAULT);//srcbuf
    VoC_movreg32(RL6,REG01,IN_PARALLEL);

    VoC_add32_rr(REG01,REG01,REG45,DEFAULT);//pa = srcbuf + (w<<1); pa = srcbuf + w ;
    VoC_lw16_sd(REG3,1,IN_PARALLEL);//q15

    VoC_add32_rd(RL7,RL7,GLOBAL_SCALEX_SIZE_ADDR);//wn+=scalex;

    VoC_lw32inc_p(REG45,REG0,DEFAULT);//u = (*(pa))>>8;    v = (*(pa+1))>>8;
    VoC_sub16_rr(REG7,REG3,REG6,IN_PARALLEL);//wa = Q15 - wb;

    VoC_shru16_ri(REG4,8,DEFAULT);//u
    VoC_shru16_ri(REG5,8,IN_PARALLEL);//v

    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_multf32_rr(ACC1,REG4,REG7,IN_PARALLEL);

    VoC_lw32_p(REG45,REG0,DEFAULT);//u1 = (*(pa+2))&0xff;  v1 = (*(pa+3))&0xff;

    VoC_shru16_ri(REG4,8,DEFAULT);
    VoC_shru16_ri(REG5,8,IN_PARALLEL);

    VoC_mac32_rr(REG5,REG6,DEFAULT);//(v*wa+v1*wb)
    VoC_mac32_rr(REG4,REG6,IN_PARALLEL);//(u*wa+u1*wb)

    VoC_lw16inc_p(REG4,REG1,DEFAULT);//y

    VoC_lw16_p(REG5,REG1,DEFAULT);//y1
    VoC_movreg32(REG01,RL6,IN_PARALLEL);

    VoC_and32_rd(REG45,CONST_0XFF00FF_ADDR);

    VoC_sw16inc_p(ACC1_HI,REG1,DEFAULT);
    VoC_multf32_rr(ACC0,REG6,REG5,IN_PARALLEL);

    VoC_mac32_rr(REG7,REG4,DEFAULT);//(y*wa + y1*wb)
    VoC_sw16inc_p(ACC0_HI,REG1,IN_PARALLEL);//*pbu++ = (v*wa+v1*wb)>>S15;

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_movreg32(REG45,RL7,IN_PARALLEL);

    VoC_and32_rd(REG67,GLOBAL_MASK_ADDR);//wb = wn&MASK;

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);//*pb++ =(y*wa + y1*wb)>>S15;
    VoC_shru32_ri(REG45,S15,IN_PARALLEL);//w  = (wn>>S15);

    VoC_endloop0
    ////////////////loop2//////////////////////////////////////////////////////////

    VoC_pop16(REG3,DEFAULT);//dec_w/2
    VoC_pop16(RL6_LO,DEFAULT);//q15
    VoC_add16_rr(REG1,REG0,REG4,IN_PARALLEL);//srcbuf + w
    //VoC_lw32_d(ACC1,GLOBAL_SCALEX_SIZE_ADDR)

    VoC_loop_r(0,REG3);//for (i=0;i<dec_w;i++)
    //r6;wb   r4:w

    VoC_add32_rd(RL7,RL7,GLOBAL_SCALEX_SIZE_ADDR);//wn+=scalex;

    VoC_lw16inc_p(REG4,REG1,DEFAULT);//y1
    VoC_lw16_p(REG5,REG1,DEFAULT);//y
    VoC_movreg16(REG3,RL6_LO,IN_PARALLEL);

    VoC_and32_rd(REG45,CONST_0XFF00FF_ADDR);

    VoC_multf32_rr(ACC0,REG5,REG6,DEFAULT);
    VoC_sub16_rr(REG7,REG3,REG6,IN_PARALLEL);//wa = Q15 - wb;

    VoC_mac32_rr(REG4,REG7,DEFAULT);
    VoC_movreg32(REG45,RL7,IN_PARALLEL);//wn

    VoC_shru32_ri(REG45,S15,DEFAULT);//w  = (wn>>S15);
    VoC_movreg32(REG67,RL7,IN_PARALLEL);

    VoC_and32_rd(REG67,GLOBAL_MASK_ADDR);//wb = wn&MASK;

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);//*pb++ =(y*wa + y1*wb)>>S15;
    VoC_add16_rr(REG1,REG0,REG4,IN_PARALLEL);//srcbuf + w

    VoC_endloop0

    VoC_pop16(REG1,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);

    VoC_return;
}
/************************************************************************/
/* zoom in                                                              */
/************************************************************************/
void Coolsand_bilinear_preview_yuv_zoomin_withoutblend(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw32z(REG67,DEFAULT);//w
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    VoC_lw16i(RL6_LO,CONST_TabU2G_ADDR);
    VoC_lw16i(RL6_HI,CONST_TableV2G_ADDR);
    VoC_lw16i(RL7_HI,CONST_TabV2R_ADDR);
    VoC_lw16i(RL7_LO,CONST_TabU2B_ADDR);

    VoC_push16(REG7,DEFAULT);//extra

    //judge the dma's condition
//  VoC_cfg(CFG_CTRL_STALL);//VoC_cfg(CFG_CTRL_CONTINUE);
//  VoC_NOP();
//    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

LABLE_CUTZOOM_zoomin_loop_withoutblend:
    VoC_lw16_d(REG4,INPUT_DEC_H_ADDR);
    VoC_shru16_ri(REG4,1,DEFAULT);//h/2


    VoC_sw16_d(REG5,INPUT_IN_MODE_ADDR);//this is used to deal with long width
    VoC_loop_r(1,REG4);//for (j=0;j<dec_h;j++)

    VoC_shru32_ri(REG67,S15,DEFAULT);//num1 = (w>>S15);
    VoC_lw16_sd(REG4,0,IN_PARALLEL);//extra

    VoC_push32(REG67,DEFAULT);//save wn
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);//num1 = (w>>S15) - extra;

    VoC_lw16_d(REG7,GLOBAL_NUM1_ADDR);
    VoC_blt16_rd(LABLE_ZOOMIN_L0_withoutblend,REG6,GLOBAL_INLINE_NUM_1_ADDR);//if (num1>=numline-1)
    VoC_jal(COOLSAND_DMA_READ);//memcpy(bufin,pin,numcount);//pin+=(numline-1)*src_w;
    //VoC_lw32_sd(REG45,0,DEFAULT);
    //VoC_and32_rd(REG45,GLOBAL_MASK_ADDR);
    VoC_lw16_d(REG5,GLOBAL_INLINE_NUM_1_ADDR);
    VoC_lw16_sd(REG4,0,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG5,IN_PARALLEL);//num1-=numline-1;
    VoC_sub16_rr(REG7,REG7,REG5,DEFAULT);//VoC_NOP();
    VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);//extra+=numline-1;
    VoC_NOP();
    VoC_sw16_sd(REG4,0,DEFAULT);
    //VoC_sw32_sd(REG45,0,DEFAULT);
LABLE_ZOOMIN_L0_withoutblend:

    VoC_jal(COOLSAND_DMA_WRITE_withoutblend);
    //compute p3 and p4
    VoC_mult16_rd(REG4,REG6,INPUT_SRC_W_ADDR);//p3 = pa + (num1*src_w);
    VoC_sub16_rr(REG7,REG6,REG7,DEFAULT);//VoC_sub16_rd(REG7,REG6,GLOBAL_NUM1_ADDR);//num1-num2
    VoC_sw16_d(REG6,GLOBAL_NUM1_ADDR);
    VoC_add16_rd(REG4,REG4,GLOBAL_DEAD_UX_ADDR);//P3
    VoC_add16_rd(REG5,REG4,INPUT_SRC_W_ADDR);//p4 = p3 + src_w;
    //zoom a line
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);//VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_bne16_rr(LABLE_ZOOMIN_L1_withoutblend,REG7,REG6)//if (num1-num2==1)

    VoC_bnez16_d(LABLE_ZOOMIN_L3_withoutblend,GLOBAL_NUM3_ADDR);//if (num3==0)
    //set p1 p2 and num3
    VoC_lw16i_set_inc(REG3,GLOBAL_TEMP_BUF_ADDR,2);//ptmp2    p2 = tmpbuf;
    VoC_lw16d_set_inc(REG2,GLOBAL_TEMP_BUF2_ADDR,1);//tmpbuf   p1 = ptmp2;
    VoC_sw16_d(REG6,GLOBAL_NUM3_ADDR);//num3=1;
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);
    //conver yuv2rgb of a line data
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_PTMP3_ADDR,2);
    VoC_movreg16(REG0,REG5,DEFAULT);//P4
    VoC_jal(Coolsand_yuv2rgb_oneline);//Coolsand_yuv2rgb(p4,ptmp3,cut_w);
    //zoom rgb line
    VoC_lw16d_set_inc(REG3,GLOBAL_P2_ADDR,1);
    VoC_lw16i_short(INC0,1,DEFAULT)
    VoC_jal(Coolsand_zoomin_rgbline);//Coolsand_Zoomin_rgbline(ptmp3,tmpbuf,dec_w,scalex);
    VoC_jump(LABLE_ZOOMIN_L2_withoutblend);
LABLE_ZOOMIN_L3_withoutblend://else
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16d_set_inc(REG3,GLOBAL_TEMP_BUF2_ADDR,2);//ptmp2  p2 = ptmp2;
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_BUF_ADDR,1);//tmpbuf   p1 = tmpbuf;
    VoC_sw16_d(REG6,GLOBAL_NUM3_ADDR);//num3 = 0;
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);

    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_PTMP3_ADDR,2);
    VoC_movreg16(REG0,REG5,DEFAULT);//P4
    VoC_jal(Coolsand_yuv2rgb_oneline);

    VoC_lw16d_set_inc(REG3,GLOBAL_P2_ADDR,1);
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_jal(Coolsand_zoomin_rgbline);//zoom_line(p4,ptmp2,dec_w,scalex);
    VoC_jump(LABLE_ZOOMIN_L2_withoutblend);
LABLE_ZOOMIN_L1_withoutblend:
    VoC_bez16_r(LABLE_ZOOMIN_L2_withoutblend,REG7);//else if (num1-num2!=0)
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16d_set_inc(REG3,GLOBAL_TEMP_BUF2_ADDR,2);//ptmp2  p2 = ptmp2;
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_BUF_ADDR,1);//tmpbuf   p1 = tmpbuf;
    VoC_sw16_d(REG6,GLOBAL_NUM3_ADDR);//num3 = 0;
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);

    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_push16(REG5,IN_PARALLEL);
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_PTMP3_ADDR,2);
    VoC_jal(Coolsand_yuv2rgb_oneline);//Coolsand_yuv2rgb(p3,ptmp3,cut_w);
    VoC_lw16d_set_inc(REG3,GLOBAL_P1_ADDR,1);
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_jal(Coolsand_zoomin_rgbline);//Coolsand_Zoomin_rgbline(ptmp3,tmpbuf,dec_w,scalex);


    VoC_pop16(REG0,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL)
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_PTMP3_ADDR,2);
    VoC_jal(Coolsand_yuv2rgb_oneline);//Coolsand_yuv2rgb(p4,ptmp3,cut_w);
    VoC_lw16d_set_inc(REG3,GLOBAL_P2_ADDR,1);
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_jal(Coolsand_zoomin_rgbline);//Coolsand_Zoomin_rgbline(ptmp3,ptmp2,dec_w,scalex);

LABLE_ZOOMIN_L2_withoutblend:

    VoC_lw16i_set_inc(REG3,Q15,2);

    VoC_lw32_sd(REG67,0,DEFAULT);//wn
    VoC_lw16i_short(INC2,2,IN_PARALLEL);

    VoC_and32_rd(REG67,GLOBAL_MASK_ADDR);//wb = wn&MASK;
    VoC_sub16_rr(REG7,REG3,REG6,DEFAULT);//wa = Q15 - wb;
    VoC_lw32_d(REG23,GLOBAL_P1_ADDR);
    VoC_jal(Coolsand_AverageTwoLine_withoutblend);

    ///////judge wether to dma out data & read back data in//////////////
//             VoC_bne16_rd(LABLE_ZOOMIN_DMA_BLENDBUF,REG1,GLOBAL_DEAD_OUT_LINE_ADDR);
//             VoC_bnez16_d(LABLE_ZOOMIN_BLITMODE1,INPUT_BLIT_MODE_ADDR);
//             VoC_jal(COOLSAND_DMA_WRITE);
//           VoC_jump(LABLE_BLITMODE_ZOOMIN_END);
//LABLE_ZOOMIN_BLITMODE1:
//           VoC_jal(Coolsand_DMAWRITE_sb33bf);
//LABLE_BLITMODE_ZOOMIN_END:

//           VoC_jal(COOLSAND_DMA_WRITE);
//           VoC_jal(COOLSAND_DMA_READ_BlendBuf);//add blend
//LABLE_ZOOMIN_DMA_BLENDBUF:

    VoC_pop32(REG67,DEFAULT);//wn
    VoC_add32_rd(REG67,REG67,GLOBAL_SCALEY_SIZE_ADDR);//update

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_endloop1

    VoC_lw16_d(REG5,INPUT_IN_MODE_ADDR);
    VoC_sub16_rd(REG5,REG5,CONST_1_ADDR);
    VoC_bltz16_r(LABLE_ZOOMIN_GOON_withoutblend,REG5);
    VoC_jump(LABLE_CUTZOOM_zoomin_loop_withoutblend);
LABLE_ZOOMIN_GOON_withoutblend:

    //dma out the last data
//           VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR)
//           VoC_be16_rr(LABLE_ZOOMIN_L7,REG1,REG5);
////             VoC_bnez16_d(LABLE_LastZOOMIN_BLITMODE1,INPUT_BLIT_MODE_ADDR);
////             VoC_jal(Coolsand_WriteLast);
////             VoC_jump(LABLE_ZOOMIN_L7);
////LABLE_LastZOOMIN_BLITMODE1:
////             VoC_jal(Coolsand_WriteLast_sb33bf_blitLCD);
//
//           VoC_jal(Coolsand_WriteLast);
//LABLE_ZOOMIN_L7:
    VoC_jal(COOLSAND_DMA_WRITE_withoutblend);
    /////////////////////////////////////////////////



    VoC_pop16(REG4,DEFAULT);//extra
    VoC_pop16(RA,DEFAULT);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    //VoC_NOP();
    VoC_return;

}
void Coolsand_bilinear_preview_yuv_zoomin(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw32z(REG67,DEFAULT);//w
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    VoC_lw16i(RL6_LO,CONST_TabU2G_ADDR);
    VoC_lw16i(RL6_HI,CONST_TableV2G_ADDR);
    VoC_lw16i(RL7_HI,CONST_TabV2R_ADDR);
    VoC_lw16i(RL7_LO,CONST_TabU2B_ADDR);

    VoC_push16(REG7,DEFAULT);//extra

    //judge the dma's condition
//  VoC_cfg(CFG_CTRL_STALL);//VoC_cfg(CFG_CTRL_CONTINUE);
//  VoC_NOP();
//    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

LABLE_CUTZOOM_zoomin_loop:
    VoC_lw16_d(REG4,INPUT_DEC_H_ADDR);
    VoC_shru16_ri(REG4,1,DEFAULT);//h/2


    VoC_sw16_d(REG5,INPUT_IN_MODE_ADDR);//this is used to deal with long width
    VoC_loop_r(1,REG4);//for (j=0;j<dec_h;j++)

    VoC_shru32_ri(REG67,S15,DEFAULT);//num1 = (w>>S15);
    VoC_lw16_sd(REG4,0,IN_PARALLEL);//extra

    VoC_push32(REG67,DEFAULT);//save wn
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);//num1 = (w>>S15) - extra;

    VoC_lw16_d(REG7,GLOBAL_NUM1_ADDR);
    VoC_blt16_rd(LABLE_ZOOMIN_L0,REG6,GLOBAL_INLINE_NUM_1_ADDR);//if (num1>=numline-1)
    VoC_jal(COOLSAND_DMA_READ);//memcpy(bufin,pin,numcount);//pin+=(numline-1)*src_w;
    //VoC_lw32_sd(REG45,0,DEFAULT);
    //VoC_and32_rd(REG45,GLOBAL_MASK_ADDR);
    VoC_lw16_d(REG5,GLOBAL_INLINE_NUM_1_ADDR);
    VoC_lw16_sd(REG4,0,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG5,IN_PARALLEL);//num1-=numline-1;
    VoC_sub16_rr(REG7,REG7,REG5,DEFAULT);//VoC_NOP();
    VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);//extra+=numline-1;
    VoC_NOP();
    VoC_sw16_sd(REG4,0,DEFAULT);
    //VoC_sw32_sd(REG45,0,DEFAULT);
LABLE_ZOOMIN_L0:
    VoC_jal(COOLSAND_DMA_READ_BlendBuf);//add blend

    //compute p3 and p4
    VoC_mult16_rd(REG4,REG6,INPUT_SRC_W_ADDR);//p3 = pa + (num1*src_w);
    VoC_sub16_rr(REG7,REG6,REG7,DEFAULT);//VoC_sub16_rd(REG7,REG6,GLOBAL_NUM1_ADDR);//num1-num2
    VoC_sw16_d(REG6,GLOBAL_NUM1_ADDR);
    VoC_add16_rd(REG4,REG4,GLOBAL_DEAD_UX_ADDR);//P3
    VoC_add16_rd(REG5,REG4,INPUT_SRC_W_ADDR);//p4 = p3 + src_w;
    //zoom a line
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);//VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_bne16_rr(LABLE_ZOOMIN_L1,REG7,REG6)//if (num1-num2==1)

    VoC_bnez16_d(LABLE_ZOOMIN_L3,GLOBAL_NUM3_ADDR);//if (num3==0)
    //set p1 p2 and num3
    VoC_lw16i_set_inc(REG3,GLOBAL_TEMP_BUF_ADDR,2);//ptmp2    p2 = tmpbuf;
    VoC_lw16d_set_inc(REG2,GLOBAL_TEMP_BUF2_ADDR,1);//tmpbuf   p1 = ptmp2;
    VoC_sw16_d(REG6,GLOBAL_NUM3_ADDR);//num3=1;
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);
    //conver yuv2rgb of a line data
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_PTMP3_ADDR,2);
    VoC_movreg16(REG0,REG5,DEFAULT);//P4
    VoC_jal(Coolsand_yuv2rgb_oneline);//Coolsand_yuv2rgb(p4,ptmp3,cut_w);
    //zoom rgb line
    VoC_lw16d_set_inc(REG3,GLOBAL_P2_ADDR,1);
    VoC_lw16i_short(INC0,1,DEFAULT)
    VoC_jal(Coolsand_zoomin_rgbline);//Coolsand_Zoomin_rgbline(ptmp3,tmpbuf,dec_w,scalex);
    VoC_jump(LABLE_ZOOMIN_L2);
LABLE_ZOOMIN_L3://else
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16d_set_inc(REG3,GLOBAL_TEMP_BUF2_ADDR,2);//ptmp2  p2 = ptmp2;
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_BUF_ADDR,1);//tmpbuf   p1 = tmpbuf;
    VoC_sw16_d(REG6,GLOBAL_NUM3_ADDR);//num3 = 0;
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);

    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_PTMP3_ADDR,2);
    VoC_movreg16(REG0,REG5,DEFAULT);//P4
    VoC_jal(Coolsand_yuv2rgb_oneline);

    VoC_lw16d_set_inc(REG3,GLOBAL_P2_ADDR,1);
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_jal(Coolsand_zoomin_rgbline);//zoom_line(p4,ptmp2,dec_w,scalex);
    VoC_jump(LABLE_ZOOMIN_L2);
LABLE_ZOOMIN_L1:
    VoC_bez16_r(LABLE_ZOOMIN_L2,REG7);//else if (num1-num2!=0)
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16d_set_inc(REG3,GLOBAL_TEMP_BUF2_ADDR,2);//ptmp2  p2 = ptmp2;
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_BUF_ADDR,1);//tmpbuf   p1 = tmpbuf;
    VoC_sw16_d(REG6,GLOBAL_NUM3_ADDR);//num3 = 0;
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);

    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_push16(REG5,IN_PARALLEL);
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_PTMP3_ADDR,2);
    VoC_jal(Coolsand_yuv2rgb_oneline);//Coolsand_yuv2rgb(p3,ptmp3,cut_w);
    VoC_lw16d_set_inc(REG3,GLOBAL_P1_ADDR,1);
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_jal(Coolsand_zoomin_rgbline);//Coolsand_Zoomin_rgbline(ptmp3,tmpbuf,dec_w,scalex);


    VoC_pop16(REG0,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL)
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_PTMP3_ADDR,2);
    VoC_jal(Coolsand_yuv2rgb_oneline);//Coolsand_yuv2rgb(p4,ptmp3,cut_w);
    VoC_lw16d_set_inc(REG3,GLOBAL_P2_ADDR,1);
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_jal(Coolsand_zoomin_rgbline);//Coolsand_Zoomin_rgbline(ptmp3,ptmp2,dec_w,scalex);

LABLE_ZOOMIN_L2:

    VoC_jal(COOLSAND_DMA_WRITE);
    VoC_lw16i_set_inc(REG3,Q15,2);

    VoC_lw32_sd(REG67,0,DEFAULT);//wn
    VoC_lw16i_short(INC2,2,IN_PARALLEL);

    VoC_and32_rd(REG67,GLOBAL_MASK_ADDR);//wb = wn&MASK;
    VoC_sub16_rr(REG7,REG3,REG6,DEFAULT);//wa = Q15 - wb;
    VoC_lw32_d(REG23,GLOBAL_P1_ADDR);
    VoC_jal(Coolsand_AverageTwoLine);

    ///////judge wether to dma out data & read back data in//////////////
//             VoC_bne16_rd(LABLE_ZOOMIN_DMA_BLENDBUF,REG1,GLOBAL_DEAD_OUT_LINE_ADDR);
//             VoC_bnez16_d(LABLE_ZOOMIN_BLITMODE1,INPUT_BLIT_MODE_ADDR);
//             VoC_jal(COOLSAND_DMA_WRITE);
//           VoC_jump(LABLE_BLITMODE_ZOOMIN_END);
//LABLE_ZOOMIN_BLITMODE1:
//           VoC_jal(Coolsand_DMAWRITE_sb33bf);
//LABLE_BLITMODE_ZOOMIN_END:

//           VoC_jal(COOLSAND_DMA_WRITE);
//           VoC_jal(COOLSAND_DMA_READ_BlendBuf);//add blend
//LABLE_ZOOMIN_DMA_BLENDBUF:

    VoC_pop32(REG67,DEFAULT);//wn
    VoC_add32_rd(REG67,REG67,GLOBAL_SCALEY_SIZE_ADDR);//update

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_endloop1

    VoC_lw16_d(REG5,INPUT_IN_MODE_ADDR);
    VoC_sub16_rd(REG5,REG5,CONST_1_ADDR);
    VoC_bltz16_r(LABLE_ZOOMIN_GOON,REG5);
    VoC_jump(LABLE_CUTZOOM_zoomin_loop);
LABLE_ZOOMIN_GOON:

    //dma out the last data
//           VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR)
//           VoC_be16_rr(LABLE_ZOOMIN_L7,REG1,REG5);
////             VoC_bnez16_d(LABLE_LastZOOMIN_BLITMODE1,INPUT_BLIT_MODE_ADDR);
////             VoC_jal(Coolsand_WriteLast);
////             VoC_jump(LABLE_ZOOMIN_L7);
////LABLE_LastZOOMIN_BLITMODE1:
////             VoC_jal(Coolsand_WriteLast_sb33bf_blitLCD);
//
//           VoC_jal(Coolsand_WriteLast);
//LABLE_ZOOMIN_L7:
    //VoC_jal(COOLSAND_DMA_WRITE);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_xor16_rd(REG4,GLOBAL_FLAG_WRITE_ADDR);
    VoC_bnez16_d(LABLE_FLAG_WRITE1_last,GLOBAL_FLAG_WRITE_ADDR);
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR/2);
    VoC_jump(LABLE_FLAG_WRITE2_last);
LABLE_FLAG_WRITE1_last:
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR2/2);
LABLE_FLAG_WRITE2_last:
    VoC_sw16_d(REG4,GLOBAL_FLAG_WRITE_ADDR);

    VoC_lw16_d(REG4,GLOBAL_BLEND_DMASIZE_ADDR);
    VoC_lw32_d(ACC0,OUTPUT_ADDR_ADDR);//ex addr[byte]

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    /////////////////////////////////////////////////



    VoC_pop16(REG4,DEFAULT);//extra
    VoC_pop16(RA,DEFAULT);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    //VoC_NOP();
    VoC_return;

}

////zoom in
void Coolsand_yuv2rgb_oneline(void)
{
    /*****y1 u y2 v ***********
    fun:
        used to transform yuv format data to r g  b format
    input parameters:
       yuv buf addr: r0  inc0=2
       rgb buf addr: r2  inc2=1
    used regs:

    zouying  added  2008-12-2
    ***************************/
    VoC_lw16i(REG3,128);
    VoC_lw32_d(ACC0,CONST_0XFF00FF_ADDR);
    VoC_lw16_d(REG4,INPUT_CUT_W_ADDR);
    VoC_push16(REG3,DEFAULT);//128
    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);


    VoC_loop_r(0,REG4);

    VoC_lw32_p(REG45,REG0,DEFAULT);
    VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);
    //r4:y1 r5:y2 r6:u r7:v
    VoC_shru16_ri(REG6,8,DEFAULT);//u
    VoC_shru16_ri(REG7,8,IN_PARALLEL);//v

    VoC_add32_rr(REG01,REG67,RL6,DEFAULT);//TableV2G[v]    TabU2G[u]
    VoC_movreg32(ACC1,REG01,IN_PARALLEL);//VoC_push32(REG01,IN_PARALLEL);

    VoC_and32_rr(REG45,ACC0,DEFAULT);
    VoC_add32_rr(REG67,REG67,RL7,IN_PARALLEL);//TabV2R[v]  TabU2B[u]

    VoC_sub16_pp(REG3,REG0,REG1,DEFAULT);//TabU2G[u] - TableV2G[v]

    VoC_add16_rp(REG0,REG4,REG7,DEFAULT);//r = irgb_sat[y + TabV2R[v]];
    VoC_add16_rr(REG1,REG4,REG3,IN_PARALLEL);//g = irgb_sat[y + TabU2G[u] - TableV2G[v]];

    VoC_add16_rp(REG6,REG4,REG6,DEFAULT);//b = irgb_sat[y + TabU2B[u]];
    VoC_add16_rp(REG7,REG5,REG7,IN_PARALLEL);//r1 = irgb_sat[y + TabV2R[v]];

    VoC_add16_rp(REG5,REG5,REG6,DEFAULT);//b1 = irgb_sat[y + TabU2B[u]];
    VoC_add16_rr(REG4,REG5,REG3,IN_PARALLEL);//g1 = irgb_sat[y + TabU2G[u] - TableV2G[v]];

    //r g b: r0 r1 R6
    //r1 g1 b1: r7 r4 r5



    //VoC_sub16_rr(REG0,REG0,REG3,DEFAULT);
    //VoC_sub16_rr(REG1,REG1,REG3,IN_PARALLEL);

    VoC_shr16_ri(REG0,-8,DEFAULT);
    VoC_shr16_ri(REG1,-8,IN_PARALLEL);

    VoC_shr16_ri(REG0,8,DEFAULT);
    VoC_shr16_ri(REG1,8,IN_PARALLEL);




    //VoC_shr16_ri(REG0,3,DEFAULT);
    //VoC_shr16_ri(REG1,2,IN_PARALLEL);

    //VoC_sub16_rr(REG6,REG6,REG3,DEFAULT);
    //VoC_sub16_rr(REG7,REG7,REG3,IN_PARALLEL);

    VoC_shr16_ri(REG6,-8,DEFAULT);
    VoC_shr16_ri(REG7,-8,IN_PARALLEL);

    VoC_shr16_ri(REG6,8,DEFAULT);
    VoC_shr16_ri(REG7,8,IN_PARALLEL);



    //VoC_shr16_ri(REG6,3,DEFAULT);
    //VoC_shr16_ri(REG7,3,IN_PARALLEL);

    //VoC_sub16_rr(REG4,REG4,REG3,DEFAULT);
    //VoC_sub16_rr(REG5,REG5,REG3,IN_PARALLEL);

    VoC_shr16_ri(REG4,-8,DEFAULT);
    VoC_lw16_sd(REG3,1,IN_PARALLEL);//VoC_movreg16(REG3,ACC1_LO,IN_PARALLEL);

    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG3,IN_PARALLEL);

    VoC_add16_rr(REG6,REG6,REG3,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG3,IN_PARALLEL);

    VoC_shr16_ri(REG4,8,DEFAULT);
    VoC_shr16_ri(REG5,-8,IN_PARALLEL);


    VoC_shr16_ri(REG5,8,DEFAULT);
    VoC_sw32inc_p(REG01,REG2,IN_PARALLEL);


    VoC_add16_rr(REG4,REG4,REG3,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG3,IN_PARALLEL);

    VoC_sw32inc_p(REG67,REG2,DEFAULT);//sw r
    VoC_movreg32(REG01,ACC1,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG2,DEFAULT);

    VoC_endloop0

    VoC_pop16(REG2,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_pop16(REG3,DEFAULT);

    VoC_return;
}
//{
//   /*****y1 u y2 v ***********
//   fun:
//       used to transform yuv format data to r g  b format
//   input parameters:
//     yuv buf addr: r0  inc0=2
//     rgb buf addr: r2  inc2=1
//   used regs:
//
//   zouying  added  2008-12-2
//   ***************************/
//
//  VoC_lw32_d(ACC0,CONST_0XFF00FF_ADDR);
//  VoC_lw16_d(REG4,INPUT_CUT_W_ADDR);
//  VoC_shru16_ri(REG4,1,DEFAULT);
//    VoC_push16(REG2,IN_PARALLEL);
//
//  VoC_loop_r(0,REG4);
//
//  VoC_lw32_p(REG45,REG0,DEFAULT);
//  VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);
//    //r4:y1 r5:y2 r6:u r7:v
//    VoC_shru16_ri(REG6,8,DEFAULT);//u
//  VoC_shru16_ri(REG7,8,IN_PARALLEL);//v
//
//  VoC_add32_rr(REG01,REG67,RL6,DEFAULT);//TableV2G[v]    TabU2G[u]
//  VoC_push32(REG01,IN_PARALLEL);
//
//  VoC_and32_rr(REG45,ACC0,DEFAULT);
//  VoC_add32_rr(REG67,REG67,RL7,IN_PARALLEL);//TabV2R[v]  TabU2B[u]
//
//  VoC_sub16_pp(REG3,REG0,REG1,DEFAULT);//TabU2G[u] - TableV2G[v]
//
//  VoC_add16_rp(REG0,REG4,REG7,DEFAULT);//r = irgb_sat[y + TabV2R[v]];
//  VoC_add16_rr(REG1,REG4,REG3,IN_PARALLEL);//g = irgb_sat[y + TabU2G[u] - TableV2G[v]];
//
//  VoC_add16_rp(REG6,REG4,REG6,DEFAULT);//b = irgb_sat[y + TabU2B[u]];
//  VoC_add16_rp(REG7,REG5,REG7,IN_PARALLEL);//r1 = irgb_sat[y + TabV2R[v]];
//
//  VoC_add16_rp(REG5,REG5,REG6,DEFAULT);//b1 = irgb_sat[y + TabU2B[u]];
//  VoC_add16_rr(REG4,REG5,REG3,IN_PARALLEL);//g1 = irgb_sat[y + TabU2G[u] - TableV2G[v]];
//
//    //r g b: r0 r1 R6
//  //r1 g1 b1: r7 r4 r5
//  VoC_bnltz16_r(LABLE_R0_ZERO,REG0);
//    VoC_lw16i_short(REG0,0,DEFAULT);
//LABLE_R0_ZERO:
//
//  VoC_bnltz16_r(LABLE_G0_ZERO,REG1);
//  VoC_lw16i_short(REG1,0,DEFAULT);
//LABLE_G0_ZERO:
//
//  VoC_shr16_ri(REG0,-7,DEFAULT);
//  VoC_shr16_ri(REG1,-7,IN_PARALLEL);
//
//  VoC_shru16_ri(REG0,10,DEFAULT);
//  VoC_shru16_ri(REG1,9,IN_PARALLEL);
//
//  VoC_bnltz16_r(LABLE_B0_ZERO,REG6);
//  VoC_lw16i_short(REG6,0,DEFAULT);
//LABLE_B0_ZERO:
//
//  VoC_bnltz16_r(LABLE_R1_ZERO,REG7);
//    VoC_lw16i_short(REG7,0,DEFAULT);
//LABLE_R1_ZERO:
//
//  VoC_shr16_ri(REG6,-7,DEFAULT);
//  VoC_shr16_ri(REG7,-7,IN_PARALLEL);
//
//  VoC_shru16_ri(REG6,10,DEFAULT);
//  VoC_shru16_ri(REG7,10,IN_PARALLEL);
//
//  VoC_bnltz16_r(LABLE_G1_ZERO,REG4);
//    VoC_lw16i_short(REG4,0,DEFAULT);
//LABLE_G1_ZERO:
//
//  VoC_bnltz16_r(LABLE_B1_ZERO,REG5);
//    VoC_lw16i_short(REG5,0,DEFAULT);
//LABLE_B1_ZERO:
//
//  VoC_shr16_ri(REG4,-7,DEFAULT);
//  VoC_shr16_ri(REG5,-7,IN_PARALLEL);
//
//  VoC_shru16_ri(REG4,9,DEFAULT);
//  VoC_sw32inc_p(REG01,REG2,IN_PARALLEL);
//
//  VoC_shru16_ri(REG5,10,DEFAULT);
//    VoC_sw32inc_p(REG67,REG2,IN_PARALLEL);//sw r
//
//  VoC_pop32(REG01,DEFAULT);
//    VoC_sw32inc_p(REG45,REG2,DEFAULT);
//
//  VoC_endloop0
//
//  VoC_pop16(REG2,DEFAULT);
//  VoC_lw16i_short(INC2,1,IN_PARALLEL);
//
//  VoC_return;
//}

void Coolsand_zoomin_rgbline(void)
{
    /************************************************************************/
    /*INPUT para:
                 r2-inbuf  INC2=1
                 r3-outbuf INC3=1
                           INC0=1
      OUTPUT FORMAT: R1 R2 G1 G2 B1 B2
    /************************************************************************/

    VoC_lw16_d(REG4,INPUT_DEC_W_ADDR);
    VoC_push32(RL7,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_lw32z(RL7,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);

    VoC_push16(REG1,DEFAULT);
    VoC_lw32z(REG67,IN_PARALLEL);//wn = 0;

    VoC_lw16i(REG1,Q15);
    VoC_lw16i_short(REG5,3,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);


    //VoC_sub16_rd(REG4,REG4,CONST_1_ADDR);
    VoC_lw32_d(RL6,GLOBAL_SCALEX_SIZE_ADDR);
    VoC_lw32_d(ACC1,GLOBAL_MASK_ADDR);
    VoC_loop_r(0,REG4);//for (i=0;i<dec_w;i++)
    //r6;wb   r4:w

    VoC_add16_rr(REG0,REG2,REG5,DEFAULT);//PA1
    VoC_sub16_rr(REG7,REG1,REG6,IN_PARALLEL);//wa = Q15 - wb;

    VoC_multf32inc_rp(ACC0,REG7,REG2,DEFAULT);//r
    VoC_add32_rr(RL7,RL7,RL6,IN_PARALLEL);//wn+=scalex;

    VoC_mac32inc_rp(REG6,REG0,DEFAULT);//r
    VoC_movreg32(REG45,RL7,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG7,REG2,DEFAULT);//g
    VoC_lw16i_short(INC3,2,IN_PARALLEL);

    VoC_mac32inc_rp(REG6,REG0,DEFAULT);//g
    VoC_shru32_ri(REG45,S15,IN_PARALLEL);//w = wn>>S15

    VoC_sw16inc_p(ACC0_HI,REG3,DEFAULT);//sw r
    VoC_multf32_rp(ACC0,REG7,REG2,IN_PARALLEL);//b

    VoC_mac32_rp(REG6,REG0,DEFAULT);//b
    VoC_lw16i_short(REG5,3,IN_PARALLEL);

    VoC_sw16inc_p(ACC0_HI,REG3,DEFAULT);//sw g
    VoC_mult16_rr(REG4,REG4,REG5,IN_PARALLEL);//w*3

    VoC_lw16_sd(REG2,0,DEFAULT);//start addr
    VoC_movreg32(REG67,RL7,IN_PARALLEL);

    VoC_and32_rr(REG67,ACC1,DEFAULT);//VoC_and32_rd(REG67,GLOBAL_MASK_ADDR);//wb = wn&MASK;
    VoC_add16_rr(REG2,REG2,REG4,IN_PARALLEL);//pa + 3*w

    VoC_sub16_rr(REG3,REG3,REG5,DEFAULT);//POINT TO R1 POSITION
    VoC_sw16_p(ACC0_HI,REG3,IN_PARALLEL);//sw b
    //2
    VoC_add16_rr(REG0,REG2,REG5,DEFAULT);//pa1 addr
    VoC_sub16_rr(REG7,REG1,REG6,IN_PARALLEL);//wa = Q15 - wb;

    VoC_multf32inc_rp(ACC0,REG7,REG2,DEFAULT);//r
    VoC_add32_rr(RL7,RL7,RL6,IN_PARALLEL);//wn+=scalex;//wn+=scalex;
    VoC_mac32inc_rp(REG6,REG0,DEFAULT);
    VoC_movreg32(REG45,RL7,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG7,REG2,DEFAULT);//g
    VoC_shru32_ri(REG45,S15,IN_PARALLEL);//w = wn>>S15

    VoC_mac32inc_rp(REG6,REG0,DEFAULT);//G
    VoC_lw16i_short(REG5,3,IN_PARALLEL);

    VoC_multf32_rp(ACC0,REG7,REG2,DEFAULT);//b
    VoC_sw16inc_p(ACC0_HI,REG3,IN_PARALLEL);//sw r


    VoC_mac32_rp(REG6,REG0,DEFAULT);//B
    VoC_movreg32(REG67,RL7,IN_PARALLEL);

    VoC_sw16inc_p(ACC0_HI,REG3,DEFAULT);//sw g
    VoC_mult16_rr(REG4,REG4,REG5,IN_PARALLEL);//w*3

    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_lw16_sd(REG2,0,IN_PARALLEL);

    VoC_and32_rr(REG67,ACC1,DEFAULT);//VoC_and32_rd(REG67,GLOBAL_MASK_ADDR);//wb = wn&MASK;


    VoC_sw16inc_p(ACC0_HI,REG3,DEFAULT);//sw b
    VoC_add16_rr(REG2,REG2,REG4,IN_PARALLEL);//pa1

    VoC_endloop0


    VoC_pop16(REG2,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);

    VoC_pop16(REG1,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);

    VoC_return;
}



void Coolsand_AverageTwoLine(void)
{
    //outbuf:r1
    //inbuf: r2 r3   r6:wb  r7:wa

    VoC_lw16_d(REG4,INPUT_DEC_W_ADDR);

    VoC_lw16i_short(FORMAT32,S15+3-16,DEFAULT);
    VoC_lw16i_short(FORMATX,S15+2-16,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG7,REG2,DEFAULT);
    VoC_multf32inc_rp(ACC1,REG7,REG2,IN_PARALLEL);

    VoC_mac32inc_rp(REG6,REG3,DEFAULT);
    VoC_mac32inc_rp(REG6,REG3,IN_PARALLEL);

    VoC_lw16i(REG0,Q15);

    VoC_loop_r(0,REG4)

    //////////////////r//////////////////////
    //    VoC_multf32inc_rp(ACC0,REG7,REG2,DEFAULT);
    //    VoC_multf32inc_rp(ACC1,REG7,REG2,IN_PARALLEL);
    //
    //    VoC_mac32inc_rp(REG6,REG3,DEFAULT);
    //    VoC_mac32inc_rp(REG6,REG3,IN_PARALLEL);

    //    VoC_NOP();

    VoC_lw16i_short(ACC0_LO,0,DEFAULT);
    VoC_lw16i_short(ACC1_LO,0,IN_PARALLEL);

    VoC_shru32_ri(ACC0,-6,DEFAULT);
    VoC_shru32_ri(ACC1,-6,IN_PARALLEL);
    ////////////////g///////////////////
    VoC_macXinc_rp(REG7,REG2,DEFAULT);
    VoC_macXinc_rp(REG7,REG2,IN_PARALLEL);

    VoC_macXinc_rp(REG6,REG3,DEFAULT);
    VoC_macXinc_rp(REG6,REG3,IN_PARALLEL);

    VoC_push32(REG67,DEFAULT);

    VoC_lw16i_short(ACC0_LO,0,DEFAULT);
    VoC_lw16i_short(ACC1_LO,0,IN_PARALLEL);

    VoC_shru32_ri(ACC0,-5,DEFAULT);
    VoC_shru32_ri(ACC1,-5,IN_PARALLEL);
    ////////////////B////////////////////////
    VoC_mac32inc_rp(REG7,REG2,DEFAULT);
    VoC_mac32inc_rp(REG7,REG2,IN_PARALLEL);

    VoC_mac32inc_rp(REG6,REG3,DEFAULT);
    VoC_mac32inc_rp(REG6,REG3,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG7,REG2,DEFAULT);
    VoC_multf32inc_rp(ACC1,REG7,REG2,IN_PARALLEL);

    VoC_movreg16(REG4,ACC0_HI,DEFAULT);
    VoC_movreg16(REG5,ACC1_HI,IN_PARALLEL);

    VoC_mac32inc_rp(REG6,REG3,DEFAULT);
    VoC_mac32inc_rp(REG6,REG3,IN_PARALLEL);

    VoC_lw32_p(REG67,REG1,DEFAULT);

    VoC_bez16_r(AverageTwoLine_blend1,REG6);
    VoC_movreg16(REG4,REG6,DEFAULT);
AverageTwoLine_blend1:
    VoC_bez16_r(AverageTwoLine_blend2,REG7);
    VoC_movreg16(REG5,REG7,DEFAULT);
    VoC_NOP();
AverageTwoLine_blend2:


    VoC_pop32(REG67,DEFAULT);
    VoC_sw32inc_p(REG45,REG1,IN_PARALLEL);

    VoC_endloop0

    VoC_lw16i_short(FORMATX,S15-16,DEFAULT);
    VoC_lw16i_short(FORMAT32,S15-16,IN_PARALLEL);

    VoC_return;
}

void Coolsand_AverageTwoLine_withoutblend(void)
{
    //outbuf:r1
    //inbuf: r2 r3   r6:wb  r7:wa

    VoC_lw16_d(REG4,INPUT_DEC_W_ADDR);

    VoC_lw16i_short(FORMAT32,S15+3-16,DEFAULT);
    VoC_lw16i_short(FORMATX,S15+2-16,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG7,REG2,DEFAULT);
    VoC_multf32inc_rp(ACC1,REG7,REG2,IN_PARALLEL);

    VoC_mac32inc_rp(REG6,REG3,DEFAULT);
    VoC_mac32inc_rp(REG6,REG3,IN_PARALLEL);

    VoC_lw16i(REG0,Q15);

    VoC_loop_r(0,REG4)

    //////////////////r//////////////////////
    //    VoC_multf32inc_rp(ACC0,REG7,REG2,DEFAULT);
    //    VoC_multf32inc_rp(ACC1,REG7,REG2,IN_PARALLEL);
    //
    //    VoC_mac32inc_rp(REG6,REG3,DEFAULT);
    //    VoC_mac32inc_rp(REG6,REG3,IN_PARALLEL);

    //    VoC_NOP();

    VoC_lw16i_short(ACC0_LO,0,DEFAULT);
    VoC_lw16i_short(ACC1_LO,0,IN_PARALLEL);

    VoC_shru32_ri(ACC0,-6,DEFAULT);
    VoC_shru32_ri(ACC1,-6,IN_PARALLEL);
    ////////////////g///////////////////
    VoC_macXinc_rp(REG7,REG2,DEFAULT);
    VoC_macXinc_rp(REG7,REG2,IN_PARALLEL);

    VoC_macXinc_rp(REG6,REG3,DEFAULT);
    VoC_macXinc_rp(REG6,REG3,IN_PARALLEL);

    VoC_push32(REG67,DEFAULT);

    VoC_lw16i_short(ACC0_LO,0,DEFAULT);
    VoC_lw16i_short(ACC1_LO,0,IN_PARALLEL);

    VoC_shru32_ri(ACC0,-5,DEFAULT);
    VoC_shru32_ri(ACC1,-5,IN_PARALLEL);
    ////////////////B////////////////////////
    VoC_mac32inc_rp(REG7,REG2,DEFAULT);
    VoC_mac32inc_rp(REG7,REG2,IN_PARALLEL);

    VoC_mac32inc_rp(REG6,REG3,DEFAULT);
    VoC_mac32inc_rp(REG6,REG3,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG7,REG2,DEFAULT);
    VoC_multf32inc_rp(ACC1,REG7,REG2,IN_PARALLEL);

    VoC_movreg16(REG4,ACC0_HI,DEFAULT);
    VoC_movreg16(REG5,ACC1_HI,IN_PARALLEL);

    VoC_mac32inc_rp(REG6,REG3,DEFAULT);
    VoC_mac32inc_rp(REG6,REG3,IN_PARALLEL);

    VoC_lw32_p(REG67,REG1,DEFAULT);

    VoC_pop32(REG67,DEFAULT);
    VoC_sw32inc_p(REG45,REG1,IN_PARALLEL);

    VoC_endloop0

    VoC_lw16i_short(FORMATX,S15-16,DEFAULT);
    VoC_lw16i_short(FORMAT32,S15-16,IN_PARALLEL);

    VoC_return;
}
#endif

