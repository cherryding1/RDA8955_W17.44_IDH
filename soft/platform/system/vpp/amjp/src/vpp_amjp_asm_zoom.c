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



#include "vppp_amjp_asm_map.h"
#include "vppp_amjp_asm_zoom.h"
#include "vppp_amjp_asm_sections.h"

#include "voc2_library.h"
#include "voc2_define.h"


#if 0

voc_struct ZOOM_TABLE_RAM_Y_STRUCT,y

voc_word CONST_YUV2RGB_STRUCT_COMPRESSED[69]

voc_struct_end

#endif


void vpp_AmjpZoom(void)
{

#if 0

    voc_struct CONST_YUV2RGB_STRUCT,x

    voc_short   CONST_TabV2R_ADDR[256]
    voc_short   CONST_TableV2G_ADDR[256]
    voc_short   CONST_TabU2G_ADDR[256]
    voc_short   CONST_TabU2B_ADDR[256]

    voc_struct_end

//320+320 = 640 *2 = 1280
    voc_short   GLOBAL_TEMP_BUF_ADDR[1280],  X
    voc_alias   GLOBAL_TEMP_PTMP2_ADDR    GLOBAL_TEMP_BUF_ADDR+640 , X
    voc_alias   CONST_ZOOM_1_ADDR         CONST_TableV2G_ADDR+2 ,X
    voc_alias   CONST_ZOOM_2_ADDR         CONST_TableV2G_ADDR+3 ,X
    voc_short   GLOBAL_BUF_IN_ADDR[1280*2],      X // two lines of max size 1280

#endif


#if 0

    voc_short   GLOBAL_INLINE_NUM_1_ADDR,       Y//28,
    voc_short   GLOBAL_INLINE_NUM_2_ADDR,       Y
    voc_short   TEMP_ADDRY_ADDR,               Y
    voc_short   TEMP_ADDRU_ADDR,               Y
    voc_short   TEMP_ADDRV_ADDR,               Y
    voc_short   GLOBAL_INNER_Y_ADDR      ,      Y
    voc_short   GLOBAL_INNER_U_ADDR      ,      Y
    voc_short   GLOBAL_BUFIN_U_ADDR      ,      Y
    voc_short   GLOBAL_INNER_V_ADDR      ,      Y
    voc_short   GLOBAL_BUFIN_V_ADDR      ,      Y

    voc_short   GLOBAL_SRCW_HALF_ADDR,          Y//28,
    voc_short   GLOBAL_SRCW_QUA_ADDR,           Y
    voc_short   GLOBAL_NUM1_ADDR,               Y
    voc_short   GLOBAL_NUM3_ADDR,               Y
    voc_short   GLOBAL_MASK_ADDR[2],            Y
    voc_short   GLOBAL_DMAOUT_SIZE_ADDR,       Y
    voc_short   GLOBAL_DEAD_OUT_LINE_ADDR,      Y//128*74=9472  176*54=9504
    voc_short   GLOBAL_DMAOUT_INC_ADDR[2],     Y

    voc_short   GLOBAL_INLINE_COUNT_ADDR,            Y//28*src_w
    voc_short   GLOBAL_INLINE_COUNT_UV_ADDR,         Y//28*src_w
    voc_short   GLOBAL_DMAIN_SIZE_ADDR[2],           Y//(28*src_w)*2
    voc_short   GLOBAL_DMAIN_SIZE_UV_ADDR[2],           Y//(28*src_w)*2
    voc_short   GLOBAL_SCALEX_SIZE_ADDR[2],          Y
    voc_short   GLOBAL_SCALEY_SIZE_ADDR[2],          Y

    voc_short   GLOBAL_P1_ADDR,                     Y
    voc_short   GLOBAL_P2_ADDR,                     Y
    voc_short   GLOBAL_BUF_OUT_ADDR[OUTPUT_COUNT],  Y

#endif

    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,DEFAULT);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,DEFAULT);
    VoC_sw16_d(REG0,CFG_DMA_WRAP);
    VoC_lw16i(STATUS,STATUS_CLR);
    VoC_lw16i_short(BITCACHE,0,DEFAULT);

    VoC_lw32_d(ACC0,GLOBAL_ZOOM_CONST_Y_PTR);
    VoC_lw16i(REG2,ZOOM_TABLE_RAM_Y_STRUCT/2);//local addr
    VoC_lw16i(REG3,ZOOM_CONST_Y_SIZE);//

    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_push16(RA,DEFAULT);
    VoC_jal(vpp_AmjpZoomGenerateYuv2RgbTable);
    VoC_pop16(RA,DEFAULT);

VoC_yuv4112rgb_DMA_L0:

    VoC_lw16_d(REG4,Source_width_ADDR);
    //inline number
    VoC_lw16i_set_inc(REG0,0,1);
    VoC_lw16i(REG7,INTPUT_COUNT);
    VoC_lw16_d(REG6,Source_width_ADDR);
LABLE_LOOP_DIV_INLINE:
    VoC_sub16_rr(REG7,REG7,REG6,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);
    VoC_bnlt16_rd(LABLE_LOOP_DIV_INLINE,REG7,Zoomed_width_ADDR);
    VoC_shru16_ri(REG0,1,DEFAULT);
    VoC_shru16_ri(REG0,-1,DEFAULT);
    //////////////////////////////////////////////
    VoC_movreg16(REG1,REG0,DEFAULT);//i
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_sub32_rd(REG67,REG01,CONST_ZOOM_1_ADDR);//i-1   i-2

    VoC_lw16i_short(FORMAT16,-16+2,DEFAULT);
    VoC_shru16_ri(REG1,1,IN_PARALLEL);//i_uv
    VoC_sw32_d(REG67,GLOBAL_INLINE_NUM_1_ADDR);//GLOBAL_INLINE_NUM_1_ADDR
    //y  dma_size dma_inc
    VoC_multf32_rr(ACC1,REG7,REG4,DEFAULT);//NUMIN_LINE_2*src_w>>1  inc
    VoC_multf16_rr(REG2,REG0,REG4,IN_PARALLEL);//  numcount = numline*src_w;//[byte]->[int]

    VoC_sub16_rd(REG1,REG1,CONST_ZOOM_1_ADDR);//1/2 - 1
    VoC_lw16i_set_inc(REG3,GLOBAL_INNER_Y_ADDR,1);
    //uv  dma_size dma_inc
    VoC_lw16i_short(FORMAT32,1,DEFAULT);
    VoC_shru16_ri(REG2,-1,IN_PARALLEL);//(numline*src_w>>1)
    VoC_sw16_d(REG2,GLOBAL_INLINE_COUNT_ADDR);//dma size of y
    ////////////////
    VoC_lw32_d(REG45,Source_width_ADDR);
    VoC_sub16_rd(REG6,REG4,Cut_width_ADDR);
    VoC_shru16_ri(REG6,1,DEFAULT);//OFFX
    VoC_movreg16(REG7,REG6,IN_PARALLEL);

    ///////////////
    VoC_lw16i(REG5,GLOBAL_BUF_IN_ADDR);
    VoC_add16_rr(REG6,REG5,REG6,DEFAULT);//py
    VoC_shru16_ri(REG7,2,IN_PARALLEL);//OFFX/2
    VoC_add16_rr(REG5,REG5,REG2,DEFAULT);//pu
    VoC_shru16_ri(REG2,3,IN_PARALLEL);//uv dma size[int]
    VoC_add16_rr(REG5,REG5,REG7,DEFAULT);//pu+OFFX/2
    VoC_movreg16(REG7,REG5,IN_PARALLEL);//pu
    VoC_shru16_ri(REG7,1,DEFAULT);
    VoC_sw16inc_p(REG6,REG3,IN_PARALLEL);//GLOBAL_INNER_Y_ADDR
    VoC_sw16inc_p(REG5,REG3,DEFAULT);//GLOBAL_INNER_U_ADDR
    VoC_multf32_rr(ACC0,REG1,REG4,IN_PARALLEL);//dma addr inc of u and v
    VoC_sw16inc_p(REG7,REG3,DEFAULT);//pu/2
    VoC_shru16_ri(REG2,-1,IN_PARALLEL);
    VoC_sw16_d(REG2,GLOBAL_INLINE_COUNT_UV_ADDR);//dma size of u and v
    VoC_sw32_d(ACC1,GLOBAL_DMAIN_SIZE_ADDR);//dma addr inc of y
    VoC_sw32_d(ACC0,GLOBAL_DMAIN_SIZE_UV_ADDR);
    VoC_add16_rr(REG5,REG5,REG2,DEFAULT);//pv+OFFX/2
    VoC_shru16_ri(REG4,1,IN_PARALLEL);//SRC_W>>1
    VoC_shru16_ri(REG2,1,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);
    VoC_shru16_ri(REG5,1,DEFAULT);//SRC_W>>2
    VoC_sw16inc_p(REG5,REG3,IN_PARALLEL);//GLOBAL_INNER_V_ADDR
    VoC_add16_rr(REG7,REG7,REG2,DEFAULT);//pv
    VoC_push16(RA,IN_PARALLEL);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(ACC0_LO,-2,IN_PARALLEL);
    VoC_sw16inc_p(REG7,REG3,DEFAULT);//pv/2
    VoC_sw32_p(REG45,REG3,DEFAULT);//GLOBAL_SRCW_HALF_ADDR

    VoC_lw16i(ACC0_LO,0X1FFF);
    VoC_sw32_d(ACC0,GLOBAL_NUM1_ADDR);//GLOBAL_NUM1_ADDR
    VoC_sw32_d(ACC0,GLOBAL_MASK_ADDR);//GLOBAL_MASK_ADDR


    VoC_lw16i(REG7,OUTPUT_COUNT);
    VoC_lw16_d(REG6,Zoomed_width_ADDR);
LABLE_LOOP_DIV:
    VoC_sub16_rr(REG7,REG7,REG6,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);
    VoC_bnlt16_rd(LABLE_LOOP_DIV,REG7,Zoomed_width_ADDR);

    VoC_mult16_rd(REG4,REG0,Zoomed_width_ADDR);
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR);
    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);//dma out size
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_shru16_ri(REG4,-2,IN_PARALLEL);//dma out inc
    VoC_sw32_d(REG45,GLOBAL_DMAOUT_SIZE_ADDR);//GLOBAL_DEAD_OUT_LINE_ADDR
    VoC_sw32_d(REG45,GLOBAL_DMAOUT_INC_ADDR);//DMA OUT DMA INC

    VoC_lw16i_set_inc(REG0,GLOBAL_SCALEX_SIZE_ADDR,2);
    VoC_lw32_d(REG67,Cut_width_ADDR);
    VoC_lw32_d(RL7,Zoomed_width_ADDR);
    VoC_movreg16(REG7,RL7_LO,DEFAULT);
    VoC_movreg16(RL7_LO,REG7,IN_PARALLEL);
    VoC_jal(CoolSand_Div2);//scalex=(cut_w<<S15)/dec_w;
    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sw32inc_p(RL6,REG0,IN_PARALLEL);
    VoC_jal(CoolSand_Div2);//scaley=(cut_h<<S15)/dec_h;
    VoC_sw32inc_p(RL6,REG0,DEFAULT);

    VoC_jal(COOLSAND_DMA_READ);
    VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR,1);
    VoC_lw32z(REG67,DEFAULT);//w
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_lw16_d(REG4,Zoomed_height_ADDR );
    VoC_sub16_rd(REG4,REG4,CONST_ZOOM_1_ADDR);
    VoC_lw16i_short(FORMATX,S15-16,DEFAULT);
    VoC_lw16i_short(FORMAT32,S15-16,IN_PARALLEL);

    VoC_loop_r(1,REG4);//for (j=0;j<dec_h-1;j++)


    VoC_shru32_ri(REG67,S15,DEFAULT);//num1 = (w>>S15);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_push32(REG67,DEFAULT);//save w
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_blt16_rd(LABLE_PREVIEW_L0,REG6,GLOBAL_INLINE_NUM_2_ADDR);//if (num1>=numline-2)
    VoC_jal(COOLSAND_DMA_READ);//memcpy(bufin,pin,numcount);//pin+=(numline-1)*src_w;
    VoC_sub16_rd(REG4,REG6,GLOBAL_INLINE_NUM_2_ADDR);//num1-=numline-2;
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_shru32_ri(REG45,-S15,DEFAULT);//w = num1<<S15;
    VoC_movreg16(REG6,REG4,IN_PARALLEL);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_sw32_sd(REG45,0,DEFAULT);
LABLE_PREVIEW_L0:

    VoC_sub16_rd(REG7,REG6,GLOBAL_NUM1_ADDR);//num1-num2
    VoC_sw16_d(REG6,GLOBAL_NUM1_ADDR);
    VoC_lw16i(REG2,GLOBAL_TEMP_BUF_ADDR);
    VoC_lw16i_set_inc(REG3,GLOBAL_TEMP_PTMP2_ADDR,1);
    VoC_bne16_rr(LABLE_PREVIEW_L1,REG7,REG4)//if (num1-num2==1)

    VoC_bnez16_d(LABLE_PREVIEW_L3,GLOBAL_NUM3_ADDR);//if (num3==0)
    VoC_movreg16(REG2,REG3,DEFAULT);
    VoC_movreg16(REG3,REG2,IN_PARALLEL);
    VoC_sw16_d(REG4,GLOBAL_NUM3_ADDR);//num3=1;
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);//p2 = tmpbuf;//p1 = ptmp2;
    VoC_jal(Coolsand_UpdateP4_addr);
    VoC_jal(Coolsand_zoomline_yuv411);//zoom_line_yuv411(p4,p4u,p4v,scalex,dec_w,tmpbuf);

    VoC_jump(LABLE_PREVIEW_L2);
LABLE_PREVIEW_L3://else
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);//p1 = tmpbuf;//p2 = ptmp2;
    VoC_sw16_d(REG7,GLOBAL_NUM3_ADDR);//num3 = 0;
    VoC_jal(Coolsand_UpdateP4_addr);
    VoC_jal(Coolsand_zoomline_yuv411);//zoom_line_yuv411(p4,p4u,p4v,scalex,dec_w,ptmp2);

    VoC_jump(LABLE_PREVIEW_L2);
LABLE_PREVIEW_L1:
    VoC_bez16_r(LABLE_PREVIEW_L2,REG7);//else if (num1-num2!=0)
    VoC_lw16i_short(REG7,0,DEFAULT);//num3 = 0;
    VoC_movreg16(REG2,REG3,DEFAULT);
    VoC_movreg16(REG3,REG2,IN_PARALLEL);
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);//p1 = tmpbuf;//p2 = ptmp2;
    VoC_sw16_d(REG7,GLOBAL_NUM3_ADDR);//num3 = 0;
    VoC_jal(Coolsand_UpdateP3_addr);
    VoC_jal(Coolsand_zoomline_yuv411);//zoom_line_yuv411(p3,p3u,p3v,scalex,dec_w,tmpbuf);
    VoC_lw16i_set_inc(REG3,GLOBAL_TEMP_PTMP2_ADDR,1);
    VoC_jal(CoolSand_Updatep3p4);
    VoC_jal(Coolsand_zoomline_yuv411);//zoom_line_yuv411(p4,p4u,p4v,scalex,dec_w,ptmp2);

LABLE_PREVIEW_L2:
    VoC_lw16_d(REG4,Zoomed_width_ADDR);
    VoC_lw16i(REG5,Q15);

    VoC_lw32_sd(REG67,0,DEFAULT);//w
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_and32_rd(REG67,GLOBAL_MASK_ADDR);//wb = w&MASK;
    VoC_sub16_rr(REG7,REG5,REG6,DEFAULT);//wa = Q15 - wb;
    VoC_push16(REG1,IN_PARALLEL);

    VoC_lw32_d(REG23,GLOBAL_P1_ADDR);//p3 = p1;//p4 = p2;
    VoC_add16_rd(REG0,REG2,Zoomed_width_ADDR);//P5
    VoC_add16_rd(REG1,REG3,Zoomed_width_ADDR);//P6
    ////////////////////////////////////////////////////////////////////////////
    VoC_lw16i(RL6_LO,CONST_TabU2G_ADDR);
    VoC_lw16i(RL6_HI,CONST_TableV2G_ADDR);
    VoC_lw16i(RL7_HI,CONST_TabV2R_ADDR);
    VoC_lw16i(RL7_LO,CONST_TabU2B_ADDR);


    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);


    VoC_loop_r(0,REG4);//for (i=0;i<dec_w/2;i++)


    VoC_multf32inc_rp(ACC0,REG7,REG2,DEFAULT);    //y = *p3++;y1 = *p4++;y = ((y*wa + y1*wb)>>S15);
    VoC_multf32inc_rp(ACC1,REG7,REG2,IN_PARALLEL);   //u = *p5++;u1 = *p6++;u = ((u*wa + u1*wb)>>S15);

    VoC_macXinc_rp(REG6,REG3,DEFAULT);
    VoC_mac32inc_rp(REG6,REG3,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG7,REG0,DEFAULT);     //y1 = *p3++;y2 = *p4++;y1 = ((y1*wa + y2*wb)>>S15);
    VoC_multf32inc_rp(ACC1,REG7,REG0,IN_PARALLEL); //v = *p5++;v1 = *p6++;v = ((v*wa + v1*wb)>>S15);

    VoC_movreg16(REG4,ACC0_HI,DEFAULT);//R4:Y
    VoC_movreg16(REG5,ACC1_HI,IN_PARALLEL);//R5:Y1

    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_mac32inc_rp(REG6,REG1,IN_PARALLEL);

    VoC_push32(REG67,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_movreg16(REG6,ACC0_HI,DEFAULT);//R6:U
    VoC_movreg16(REG7,ACC1_HI,IN_PARALLEL);//R7:V

    VoC_add32_rr(REG01,REG67,RL6,DEFAULT); //TabU2G[u] ADDR  //gTableV2G[v] ADDR
    VoC_add32_rr(REG67,REG67,RL7,IN_PARALLEL);  //TabU2B[u] ADDR      //TabV2R[v] ADDR

    VoC_push32(REG01,DEFAULT);
    VoC_add16_rp(REG7,REG4,REG7,DEFAULT);//r = irgb_sat[y + TabV2R[v]];

    VoC_lw16_p(REG0,REG0,DEFAULT);//TabU2G[u]
    VoC_push32(REG67,IN_PARALLEL);

    VoC_bgtz16_r(LABLE_CUTZOOM_R1,REG7);
    VoC_lw16i_short(REG7,0,DEFAULT);
LABLE_CUTZOOM_R1:
    VoC_sub16_rp(REG1,REG0,REG1,DEFAULT);//TabU2G[u] - gTableV2G[v]
    VoC_shr16_ri(REG7,-7,IN_PARALLEL);

    VoC_add16_rp(REG6,REG4,REG6,DEFAULT);//b = irgb_sat[y + TabU2B[u]];
    VoC_shru16_ri(REG7,10,IN_PARALLEL);

    VoC_bgtz16_r(LABLE_CUTZOOM_B1,REG6);
    VoC_lw16i_short(REG6,0,DEFAULT);
LABLE_CUTZOOM_B1:

    VoC_add16_rr(REG0,REG4,REG1,DEFAULT);//g = irgb_sat[y + TabU2G[u] - gTableV2G[v]];
    VoC_shru16_ri(REG7,-11,IN_PARALLEL);

    VoC_bgtz16_r(LABLE_CUTZOOM_G1,REG0);
    VoC_lw16i_short(REG0,0,DEFAULT);
LABLE_CUTZOOM_G1:

    VoC_push16(REG1,DEFAULT);
    VoC_shr16_ri(REG0,-7,IN_PARALLEL);

    VoC_shru16_ri(REG0,9,DEFAULT);
    VoC_shr16_ri(REG6,-7,IN_PARALLEL);

    VoC_shru16_ri(REG6,10,DEFAULT);
    VoC_shru16_ri(REG0,-5,IN_PARALLEL);

    VoC_or16_rr(REG7,REG6,DEFAULT);
    VoC_lw16_sd(REG1,1,IN_PARALLEL);//pb addr

    VoC_or16_rr(REG7,REG0,DEFAULT);//DRV_RGB2PIXEL565(r,g,b);
    VoC_pop16(REG0,IN_PARALLEL);

    VoC_pop32(REG67,DEFAULT);
    VoC_add16_rr(REG0,REG5,REG0,IN_PARALLEL);//g = irgb_sat[y + TabU2G[u] - gTableV2G[v]];

    VoC_sw16inc_p(REG7,REG1,DEFAULT);//*pb++ = DRV_RGB2PIXEL565(r,g,b);
    VoC_shr16_ri(REG0,-7,IN_PARALLEL);

    VoC_bgtz16_r(LABLE_CUTZOOM_G,REG0);
    VoC_lw16i_short(REG0,0,DEFAULT);
LABLE_CUTZOOM_G:
    VoC_add16_rp(REG7,REG5,REG7,DEFAULT);//r = irgb_sat[y + TabV2R[v]];
    VoC_shru16_ri(REG0,9,IN_PARALLEL);

    VoC_bgtz16_r(LABLE_CUTZOOM_R,REG7);
    VoC_lw16i_short(REG7,0,DEFAULT);
LABLE_CUTZOOM_R:
    VoC_add16_rp(REG6,REG5,REG6,DEFAULT);//b = irgb_sat[y + TabU2B[u]];
    VoC_shr16_ri(REG7,-7,IN_PARALLEL);

    VoC_bgtz16_r(LABLE_CUTZOOM_B,REG6);
    VoC_lw16i_short(REG6,0,DEFAULT);
LABLE_CUTZOOM_B:

    VoC_shru16_ri(REG7,10,DEFAULT);
    VoC_shr16_ri(REG6,-7,IN_PARALLEL);

    VoC_shru16_ri(REG7,-11,DEFAULT);
    VoC_shru16_ri(REG6,10,IN_PARALLEL);

    VoC_or16_rr(REG7,REG6,DEFAULT);
    VoC_shru16_ri(REG0,-5,IN_PARALLEL);

    VoC_or16_rr(REG7,REG0,DEFAULT);//DRV_RGB2PIXEL565(r,g,b);
    VoC_lw16i_short(INC1,2,DEFAULT);


    VoC_sw16inc_p(REG7,REG1,DEFAULT);//*pb++ = DRV_RGB2PIXEL565(r,g,b);

    VoC_pop32(REG01,DEFAULT);     //P5 P6
    VoC_sw16_sd(REG1,0,IN_PARALLEL);//SAVE REG1

    VoC_pop32(REG67,DEFAULT);    //WB WA

    VoC_endloop0

    VoC_pop32(REG67,DEFAULT);//w+=scaley;
    VoC_pop16(REG1,IN_PARALLEL);

    VoC_bne16_rd(LABLE_PREVIEW_L4,REG1,GLOBAL_DEAD_OUT_LINE_ADDR);
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR/2);
    VoC_lw16_d(REG4,GLOBAL_DMAOUT_SIZE_ADDR);
    VoC_lw32_d(ACC0,GLOBAL_VID_BUF_OUT_PTR);//ex addr[byte]

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_add32_rd(ACC0,ACC0,GLOBAL_DMAOUT_INC_ADDR);
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);


    VoC_lw16i_set_inc(REG1,GLOBAL_BUF_OUT_ADDR,1);
    VoC_sw32_d(ACC0,GLOBAL_VID_BUF_OUT_PTR);
LABLE_PREVIEW_L4:

    VoC_add32_rd(REG67,REG67,GLOBAL_SCALEY_SIZE_ADDR);

    VoC_endloop1


    //memcpy(pb, pb-dec_w,dec_w<<1);
    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_sub16_rd(REG2,REG1,Zoomed_width_ADDR);
    VoC_lw16_d(REG4,Zoomed_width_ADDR);
    VoC_shr16_ri(REG4,2,DEFAULT);
    VoC_loop_r(0,REG4)
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_endloop0


    VoC_pop16(RA,DEFAULT);
    VoC_lw16i(REG5,GLOBAL_BUF_OUT_ADDR)
    VoC_be16_rr(LABLE_PREVIEW_L7,REG1,REG5);

    VoC_sub16_rr(REG4,REG1,REG5,DEFAULT);
    VoC_shru16_ri(REG5,1,IN_PARALLEL);//in addr[int]
    VoC_shru16_ri(REG4,1,DEFAULT);//size  [int]
    VoC_lw32_d(ACC0,GLOBAL_VID_BUF_OUT_PTR);//ex addr[byte]

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
LABLE_PREVIEW_L7:
    VoC_NOP();

    VoC_return;

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

void Coolsand_UpdateP3_addr(void)
{
    //input num1:reg6
    VoC_mult16_rd(REG4,REG6,GLOBAL_SRCW_HALF_ADDR);//(num1*src_w>>1
    VoC_lw16i_set_inc(REG0,TEMP_ADDRY_ADDR,1);
    VoC_shru16_ri(REG6,1,DEFAULT);//(num1>>1)
    VoC_mult16_rd(REG6,REG6,GLOBAL_SRCW_QUA_ADDR);//((num1>>1)*src_w>>2)
    VoC_add16_rd(REG4,REG4,GLOBAL_INNER_Y_ADDR);//p3 = pa + (num1*src_w>>1);
    VoC_add16_rd(REG5,REG6,GLOBAL_INNER_U_ADDR);//p3u = pbu + ((num1>>1)*src_w>>2);
    VoC_add16_rd(REG6,REG6,GLOBAL_INNER_V_ADDR);//p3v = pbv + ((num1>>1)*src_w>>2);
    VoC_sw32_d(REG45,TEMP_ADDRY_ADDR);
    VoC_sw16_d(REG6,TEMP_ADDRV_ADDR);

    VoC_return;
}

void CoolSand_Updatep3p4(void)
{
    VoC_lw32_d(REG45,TEMP_ADDRY_ADDR);
    VoC_lw16_d(REG6,TEMP_ADDRV_ADDR);
    VoC_add32_rd(REG45,REG45,GLOBAL_SRCW_HALF_ADDR);//p4 = p3 + (src_w>>1);//p4u = p3u + (src_w>>2);
    VoC_add16_rd(REG6,REG6,GLOBAL_SRCW_QUA_ADDR);//p4v = p3v + (src_w>>2);
    VoC_sw32_d(REG45,TEMP_ADDRY_ADDR);
    VoC_sw16_d(REG6,TEMP_ADDRV_ADDR);

    VoC_return;
}

void Coolsand_UpdateP4_addr(void)
{
    //input num1:reg6
    VoC_mult16_rd(REG4,REG6,GLOBAL_SRCW_HALF_ADDR);//(num1*src_w>>1
    VoC_lw16i_set_inc(REG0,TEMP_ADDRY_ADDR,1);
    VoC_shru16_ri(REG6,1,DEFAULT);//(num1>>1)
    VoC_mult16_rd(REG6,REG6,GLOBAL_SRCW_QUA_ADDR);//((num1>>1)*src_w>>2)
    VoC_add16_rd(REG4,REG4,GLOBAL_INNER_Y_ADDR);//p3 = pa + (num1*src_w>>1);
    VoC_add16_rd(REG5,REG6,GLOBAL_INNER_U_ADDR);//p3u = pbu + ((num1>>1)*src_w>>2);
    VoC_add16_rd(REG6,REG6,GLOBAL_INNER_V_ADDR);//p3v = pbv + ((num1>>1)*src_w>>2);
    VoC_add32_rd(REG45,REG45,GLOBAL_SRCW_HALF_ADDR);//p4 = p3 + (src_w>>1);//p4u = p3u + (src_w>>2);
    VoC_add16_rd(REG6,REG6,GLOBAL_SRCW_QUA_ADDR);//p4v = p3v + (src_w>>2);
    VoC_sw32_d(REG45,TEMP_ADDRY_ADDR);
    VoC_sw16_d(REG6,TEMP_ADDRV_ADDR);

    VoC_return;

}

void Coolsand_zoomline_yuv411(void)
{
    //  y:r0  u:r1  v:r2  out:r3_a(must be fixed befor the fun runs and inc3=1)

    VoC_lw32z(REG67,DEFAULT);//wn = 0;
    VoC_push16(REG1,IN_PARALLEL);//r1

    VoC_lw32_d(RL6,GLOBAL_MASK_ADDR);
    VoC_lw16i(REG0,Q15);

    VoC_lw16i_set_inc(REG2,TEMP_ADDRY_ADDR,1);

    VoC_add16_rd(REG1,REG3,Zoomed_width_ADDR);//pbu
    VoC_lw32z(REG45,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_push16(REG2,DEFAULT);//addry
    VoC_push32(REG01,IN_PARALLEL);//q15  pbu

    VoC_lw16d_set_inc(REG1,Zoomed_width_ADDR,1);
    VoC_shru16_ri(REG1,1,DEFAULT);
    VoC_sub16_rd(REG1,REG1,CONST_ZOOM_1_ADDR);



    VoC_loop_r(0,REG1);//for (i=0;i<(dec_w>>1);i++)


    //VoC_and32_rr(REG67,RL6,DEFAULT);//wb = wn&MASK;
    //VoC_movreg32(REG45,REG67,IN_PARALLEL);
    VoC_movreg32(RL7,REG45,DEFAULT);//save wn
    VoC_shr32_ri(REG45,S15,IN_PARALLEL);//w  = (wn>>S15);

    VoC_sub16_rr(REG7,REG0,REG6,DEFAULT);//wa = Q15 - wb;
    VoC_lw16_sd(REG2,0,IN_PARALLEL);//addry

    VoC_movreg16(REG5,REG4,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);//(w>>1)


    VoC_add16inc_rp(REG0,REG4,REG2,DEFAULT);//pa = src_y+(w>>1);
    VoC_movreg32(ACC0,REG67,IN_PARALLEL);
    VoC_add16inc_rp(REG1,REG4,REG2,DEFAULT);//pau=src_u+(w>>1);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_and16_rr(REG5,REG6,DEFAULT);
    VoC_add16_rp(REG2,REG4,REG2,IN_PARALLEL);//pav=src_v+(w>>1);

    VoC_bnez16_r(LABLE_ZOOMLINE_L1,REG5)//if ((w&1)==0)
    VoC_lw16_p(REG5,REG0,DEFAULT);
    VoC_movreg32(REG67,ACC0,IN_PARALLEL);
    VoC_lw16i(REG0,0xff);
    VoC_shru16_ri(REG5,8,DEFAULT);//y1 = ((*pa)>>8)&0xff;
    VoC_movreg16(REG4,REG5,IN_PARALLEL);
    VoC_and16_rr(REG4,REG0,DEFAULT);//y  = (*pa)&0xff;
    VoC_multf32_rr(ACC0,REG5,REG6,IN_PARALLEL);//*pb++ =(y*wa + y1*wb)>>S15;
    VoC_mac32_rr(REG4,REG7,DEFAULT);
    VoC_lw16_p(REG4,REG1,IN_PARALLEL);
    ///
    VoC_and16_rr(REG4,REG0,DEFAULT);//u = ( *pau)&0xff;
    VoC_lw16_p(REG5,REG1,IN_PARALLEL);

    VoC_shru16_ri(REG5,8,DEFAULT);//u1 =((*pau)>>8)&0xff;
    VoC_multf32_rr(ACC1,REG4,REG7,IN_PARALLEL);//*pbu++ =(u*wa+u1*wb)>>S15;//128;//

    VoC_lw16_p(REG5,REG2,DEFAULT);
    VoC_mac32_rr(REG5,REG6,IN_PARALLEL);

    VoC_lw16_p(REG4,REG2,DEFAULT);

    VoC_and16_rr(REG4,REG0,DEFAULT);//v  =( *pav)&0xff;
    VoC_shru16_ri(REG5,8,IN_PARALLEL);//v1 =((*pav)>>8)&0xff;
    VoC_jump(LABLE_ZOOMLINE_L2);
LABLE_ZOOMLINE_L1://else

    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_movreg32(REG67,ACC0,IN_PARALLEL);

    VoC_lw16i(REG0,0xff);

    VoC_lw16_p(REG5,REG0,DEFAULT);
    VoC_shru16_ri(REG4,8,IN_PARALLEL);//y = ((*pa)>>8)&0xff;

    VoC_and16_rr(REG5,REG0,DEFAULT);//y1  = (*(pa+1))&0xff;
    VoC_multf32_rr(ACC0,REG4,REG7,IN_PARALLEL);

    VoC_mac32_rr(REG5,REG6,DEFAULT);//*pb++ =(y*wa + y1*wb)>>S15;
    VoC_lw16inc_p(REG4,REG1,IN_PARALLEL);

    VoC_lw16_p(REG5,REG1,DEFAULT);
    VoC_shru16_ri(REG4,8,IN_PARALLEL);//u1  =( *(pau+1))&0xff;

    VoC_and16_rr(REG5,REG0,DEFAULT);//u =((*pau)>>8)&0xff;
    VoC_multf32_rr(ACC1,REG4,REG7,IN_PARALLEL);//*pbu++ =(u*wa+u1*wb)>>S15;//128;//


    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_mac32_rr(REG5,REG6,IN_PARALLEL);

    VoC_lw16_p(REG5,REG2,DEFAULT);

    VoC_and16_rr(REG5,REG0,DEFAULT);//v  =( *pav)&0xff;
    VoC_shru16_ri(REG4,8,IN_PARALLEL);//v1 =((*pav)>>8)&0xff;
LABLE_ZOOMLINE_L2:

    VoC_add32_rd(RL7,RL7,GLOBAL_SCALEX_SIZE_ADDR);//wn+=scalex;


    VoC_multf32_rr(ACC0,REG5,REG6,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG3,IN_PARALLEL);

    VoC_mac32_rr(REG4,REG7,DEFAULT);//*pbu++ =(v*wa+v1*wb)>>S15;//128;//
    VoC_lw32_sd(REG01,0,IN_PARALLEL);//q15

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_and32_rr(REG67,RL6,DEFAULT);//wb = wn&MASK;
    VoC_sw16inc_p(ACC1_HI,REG1,IN_PARALLEL);
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    VoC_movreg32(REG45,RL7,IN_PARALLEL);
    VoC_sw32_sd(REG01,0,DEFAULT);


    VoC_endloop0

    VoC_sub16_rd(REG2,REG3,CONST_ZOOM_1_ADDR);
    VoC_sub16_rd(REG2,REG1,CONST_ZOOM_2_ADDR);
    VoC_lw16_p(ACC0_HI,REG2,DEFAULT);
    VoC_lw16_p(ACC1_HI,REG2,DEFAULT);
    VoC_sub16_rd(REG2,REG1,CONST_ZOOM_1_ADDR);
    VoC_sw16_p(ACC0_HI,REG3,DEFAULT);//*pb++ = *(pb-1);
    VoC_lw16_p(REG2,REG2,DEFAULT);
    VoC_sw16inc_p(ACC1_HI,REG1,DEFAULT);//*pbu++ = *(pbu-2);
    VoC_sw16_p(REG2,REG1,DEFAULT);//*pbu++ = *(pbu-2);
    VoC_inc_p(REG3,IN_PARALLEL);
    ////////////////////////////////////////////////////////////////////////
    VoC_lw16_d(REG1,Zoomed_width_ADDR);
    VoC_shru16_ri(REG1,1,DEFAULT);
    VoC_pop16(REG2,IN_PARALLEL);//addry
    VoC_sub16_rd(REG1,REG1,CONST_ZOOM_1_ADDR);

    VoC_loop_r(0,REG1);//for (i=0;i<(dec_w>>1);i++)

    VoC_movreg32(RL7,REG45,DEFAULT);//save wn
    VoC_shr32_ri(REG45,S15,IN_PARALLEL);//w  = (wn>>S15);

    VoC_sub16_rr(REG7,REG0,REG6,DEFAULT);//wa = Q15 - wb;
    VoC_lw16i_short(REG1,1,IN_PARALLEL);

    VoC_movreg16(REG5,REG4,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);//(w>>1)

    VoC_and16_rr(REG5,REG1,DEFAULT);
    VoC_add16_rp(REG0,REG4,REG2,IN_PARALLEL);//pa = src_y+(w>>1);

    VoC_bnez16_r(LABLE_ZOOMLINE_L3,REG5);//if ((w&1)==0)
    VoC_lw16_p(REG4,REG0,DEFAULT);
    VoC_lw16_p(REG5,REG0,IN_PARALLEL);
    VoC_and16_ri(REG4,0xff);
    VoC_shru16_ri(REG5,8,DEFAULT);//y1 = ((*pa)>>8)&0xff;
    VoC_multf32_rr(ACC0,REG4,REG7,IN_PARALLEL);
    VoC_jump(LABLE_ZOOMLINE_L4);
LABLE_ZOOMLINE_L3://        else
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_lw16_p(REG5,REG0,DEFAULT);
    VoC_shru16_ri(REG4,8,IN_PARALLEL);//y1 = ((*pa)>>8)&0xff;
    VoC_and16_ri(REG5,0xff);
    VoC_multf32_rr(ACC0,REG4,REG7,DEFAULT);
LABLE_ZOOMLINE_L4:
    VoC_mac32_rr(REG5,REG6,DEFAULT);
    VoC_lw32_sd(REG01,0,IN_PARALLEL);

    VoC_add32_rd(REG67,RL7,GLOBAL_SCALEX_SIZE_ADDR);//wn+=scalex;

    VoC_and32_rr(REG67,RL6,DEFAULT);
    VoC_movreg32(REG45,REG67,IN_PARALLEL);

    VoC_sw16inc_p(ACC0_HI,REG3,DEFAULT);//*pb++ =(y*wa + y1*wb)>>S15;

    VoC_endloop0

    //*pb++ = *(pb-1);
    VoC_sub16_rd(REG2,REG3,CONST_ZOOM_1_ADDR);
    VoC_pop32(ACC0,DEFAULT);
    VoC_lw16_p(REG2,REG2,DEFAULT);
    VoC_pop16(REG1,DEFAULT);
    VoC_sw16_p(REG2,REG3,DEFAULT);

    VoC_return;
}

void COOLSAND_DMA_READ(void)
{
    //memcpy(bufin,pin,numcount);
    //pin+=NUMIN_LINE_2*src_w>>1;
    VoC_lw16i(REG5,GLOBAL_BUF_IN_ADDR/2);
    VoC_lw16_d(REG4,GLOBAL_INLINE_COUNT_ADDR);
    VoC_lw32_d(ACC0,GLOBAL_VID_BUF_IN_START);

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_add32_rd(ACC0,ACC0,GLOBAL_DMAIN_SIZE_ADDR);//pin+=(numline-2)*src_w;
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_sw32_d(ACC0,GLOBAL_VID_BUF_IN_START);
    //memcpy(pbu,pu,numcount_uv);
    //pu+=NUMIN_LINE_UV_1*src_w>>2;
    VoC_lw16_d(REG5,GLOBAL_BUFIN_U_ADDR);
    VoC_lw16_d(REG4,GLOBAL_INLINE_COUNT_UV_ADDR);
    VoC_lw32_d(ACC0,INPUT_ADDR_U_ADDR);

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_add32_rd(ACC0,ACC0,GLOBAL_DMAIN_SIZE_UV_ADDR);//pin+=(numline-1)*src_w;
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_sw32_d(ACC0,INPUT_ADDR_U_ADDR);
    //memcpy(pbv,pv,numcount_uv);
    //pv+=NUMIN_LINE_UV_1*src_w>>2;
    VoC_lw16_d(REG5,GLOBAL_BUFIN_V_ADDR);
    VoC_lw16_d(REG4,GLOBAL_INLINE_COUNT_UV_ADDR);
    VoC_lw32_d(ACC0,INPUT_ADDR_V_ADDR);

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_add32_rd(ACC0,ACC0,GLOBAL_DMAIN_SIZE_UV_ADDR);//pin+=(numline-1)*src_w;
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_sw32_d(ACC0,INPUT_ADDR_V_ADDR);


    VoC_return;
}



void vpp_AmjpZoomGenerateYuv2RgbTable(void)
{

    // **************************************************************************************
    //   Function:    vpp_AmjpDecompressVssAdpcmTable
    //
    //   Description: decompress a table compressed using Varible Sample Size and recursive ADPCM
    //
    //   Inputs:      REG0-> destination (decompressed table); inrc0=2
    //                REG1-> source (compressed table); inrc1=1
    //                REG2-> table size, incr2 = -1;
    //                REG3-> iterations, incr3 = -1;
    //                REG4-> step;
    //                REG5-> size/step;
    //   Outputs:
    //
    //   Used :       REG01234567, RL6
    //
    //   Version 1.0  Create by  Yordan  07/10/2008
    //
    //   *************************************************************************************


    VoC_push32(REG01,DEFAULT);
    VoC_push32(REG23,DEFAULT);
    VoC_push32(REG45,DEFAULT);
    VoC_push32(REG67,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,CONST_YUV2RGB_STRUCT,1);
    VoC_lw16i_set_inc(REG1,CONST_YUV2RGB_STRUCT_COMPRESSED,1);
    VoC_lw16i_set_inc(REG2,1024,-1);
    VoC_lw16i_set_inc(REG3,2,-1);
    VoC_lw16i(REG4,1);
    VoC_lw16i(REG5,1024);

    VoC_jal(vpp_AmjpDecompressVssAdpcm16Table);

    VoC_pop32(REG67,DEFAULT);
    VoC_pop16(RA,IN_PARALLEL);

    VoC_pop32(REG45,DEFAULT);
    VoC_pop32(REG23,DEFAULT);
    VoC_pop32(REG01,DEFAULT);

    VoC_return
}

