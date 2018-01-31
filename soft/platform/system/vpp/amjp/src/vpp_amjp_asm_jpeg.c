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
#include "vppp_amjp_asm_jpeg.h"

#include "voc2_library.h"
#include "voc2_define.h"



#if 0

voc_struct JPEG_TABLE_RAM_Y_STRUCT , y

// these constants are used for YUV2RGB,
// do not modify the order
voc_short CONST_JPEG_DEC_18_ADDR,y
voc_short CONST_JPEG_DEC_367_ADDR,y
voc_short CONST_JPEG_DEC_159_ADDR,y
voc_short CONST_JPEG_DEC_220_ADDR,y
voc_short CONST_JPEG_DEC_411_ADDR,y
voc_short CONST_JPEG_DEC_NEG_29_ADDR,y
voc_short CONST_JPEG_DEC_1108_ADDR,y
voc_short CONST_JPEG_DEC_3784_ADDR,y
voc_short CONST_JPEG_DEC_1568_ADDR,y
voc_short CONST_JPEG_DEC_565_ADDR,y
voc_short CONST_JPEG_DEC_2276_ADDR,y
voc_short CONST_JPEG_DEC_3406_ADDR,y
voc_short CONST_JPEG_DEC_2408_ADDR,y
voc_short CONST_JPEG_DEC_799_ADDR,y
voc_short CONST_JPEG_DEC_4017_ADDR,y
voc_short CONST_JPEG_DEC_181_ADDR,y
voc_word  CONST_JPEG_DEC_0x00000040_ADDR,y

voc_struct_end

voc_struct JPEG_Long_Constant     , y

voc_word CONST_JPEG_DEC_0xFFFFFFFF_ADDR,y
voc_word CONST_JPEG_DEC_0x00000080_ADDR,y
voc_word CONST_JPEG_DEC_0x00002000_ADDR,y
voc_word CONST_JPEG_DEC_0x0000ffff_ADDR,y
voc_word CONST_JPEG_DEC_0x0000ff80_ADDR,y
voc_word CONST_JPEG_DEC_0x0000fd80_ADDR,y
voc_word CONST_JPEG_DEC_0x0000e800_ADDR,y
voc_word CONST_JPEG_DEC_0x0000e000_ADDR,y

voc_struct_end

voc_struct TABLE_DClumtab  , y

voc_word TABLE_DClumtab0_ADDR ,8,y
voc_word TABLE_DClumtab1_ADDR ,8,y

voc_struct_end

voc_struct TABLE_DCchromtab , y

voc_word TABLE_DCchromtab0_ADDR ,16,y
voc_word TABLE_DCchromtab1_ADDR ,16,y

voc_struct_end

voc_struct TABLE_AClumtab   , y

voc_word TABLE_AClumtab0_ADDR ,32,y
voc_word TABLE_AClumtab1_ADDR ,48,y
voc_word TABLE_AClumtab2_ADDR ,32,y
voc_word TABLE_AClumtab3_ADDR ,128,y

voc_struct_end

voc_struct TABLE_ACchromtab  , y

voc_word TABLE_ACchromtab0_ADDR ,32,y
voc_word TABLE_ACchromtab1_ADDR ,64,y
voc_word TABLE_ACchromtab2_ADDR ,32,y
voc_word TABLE_ACchromtab3_ADDR ,128,y

voc_struct_end

voc_struct TABLE_OtherConstant    , y

voc_word  TABLE_inverse_Zig_Zag_ADDR,32,y
voc_short TABLE_add_para_ADDR,320,y

voc_struct_end

voc_struct TABLE_AClumcodetab   , y

voc_word TABLE_AClumcodetab0_ADDR, y
voc_word TABLE_AClumcodetab1_ADDR, y
voc_word TABLE_AClumcodetab2_ADDR, y
voc_word TABLE_AClumcodetab3_ADDR, y

voc_struct_end

voc_struct TABLE_ACchromcodetab   , y

voc_word TABLE_ACchromcodetab0_ADDR, y
voc_word TABLE_ACchromcodetab1_ADDR, y
voc_word TABLE_ACchromcodetab2_ADDR, y
voc_word TABLE_ACchromcodetab3_ADDR, y

voc_struct_end

voc_struct TABLE_const_jpeg_ptrs   , y

voc_short TABLE_inverse_Zig_Zag_ADDR_ADDR, y
voc_short CONST_JPEG_DEC_448_ADDR        , y

voc_struct_end


voc_alias CONST_JPEG_DEC_0_ADDR     (CONST_JPEG_DEC_0x00000040_ADDR+1) ,y
voc_alias CONST_JPEG_DEC_64_ADDR     CONST_JPEG_DEC_0x00000040_ADDR    ,y
voc_alias CONST_JPEG_DEC_128_ADDR    CONST_JPEG_DEC_0x00000080_ADDR    ,y
voc_alias CONST_JPEG_DEC_0xFF_ADDR  (TABLE_inverse_Zig_Zag_ADDR-2)     ,y
voc_alias CONST_JPEG_DEC_1_ADDR     (TABLE_inverse_Zig_Zag_ADDR+1)     ,y
voc_alias CONST_JPEG_DEC_2_ADDR     (TABLE_inverse_Zig_Zag_ADDR+5)     ,y
voc_alias CONST_JPEG_DEC_3_ADDR     (TABLE_inverse_Zig_Zag_ADDR+6)     ,y
voc_alias CONST_JPEG_DEC_4_ADDR     (TABLE_inverse_Zig_Zag_ADDR+14)    ,y
voc_alias CONST_JPEG_DEC_7_ADDR     (TABLE_inverse_Zig_Zag_ADDR+28)    ,y
voc_alias CONST_JPEG_DEC_8_ADDR     (TABLE_inverse_Zig_Zag_ADDR+2)     ,y
voc_alias CONST_JPEG_DEC_10_ADDR    (TABLE_inverse_Zig_Zag_ADDR+7)     ,y
voc_alias CONST_JPEG_DEC_15_ADDR    (TABLE_inverse_Zig_Zag_ADDR+42)    ,y
voc_alias CONST_JPEG_DEC_16_ADDR    (TABLE_inverse_Zig_Zag_ADDR+3)     ,y
voc_alias CONST_JPEG_DEC_31_ADDR    (TABLE_inverse_Zig_Zag_ADDR+92)    ,y
voc_alias CONST_JPEG_DEC_32_ADDR    (TABLE_inverse_Zig_Zag_ADDR+93)    ,y
voc_alias CONST_JPEG_DEC_63_ADDR    (TABLE_inverse_Zig_Zag_ADDR+63)    ,y
voc_alias CONST_JPEG_DEC_152_ADDR   (TABLE_inverse_Zig_Zag_ADDR+202)   ,y


#endif


//////////////////////////////////////////////////////////////////////////
/*JPEG_DECODER*/
//////////////////////////////////////////////////////////////////////////

void CII_JPEG_Decode(void)
{

#if 0

    voc_struct JPEG_LOCAL_X_VAR,x

    voc_short GLOBAL_MCUBuffer_ADDR                         ,640,x //[640]shorts
    voc_short GLOBAL_MCU_lastcoef_ADDR                      ,10,x //[10]shorts
    voc_short GLOBAL_ycoef_ADDR                             ,x//1 short
    voc_short GLOBAL_ucoef_ADDR                             ,x //1 short
    voc_short GLOBAL_vcoef_ADDR                             ,x //1 short
    voc_short GLOBAL_coef_ADDR                              ,x //1 short
    voc_short GLOBAL_image_control_ADDR                     ,x//1 short
    voc_short GLOBAL_lpMCUBuffer_ADDR_ADDR                  ,x //1 short
    voc_short GLOBAL_lastcoef_pt_ADDR                       ,x //1 short
    voc_short GLOBAL_temp_lpJpegBuf_ADDR                    ,x //1 short
    voc_short GLOBAL_Y_data_ADDR                            ,256,x //[256]shorts
    voc_short GLOBAL_rgb_buf_ADDR                           ,5632,x //[5632]shorts
    voc_short GLOBAL_temp_Y_data_ADDR                       ,x //1 short
    voc_short GLOBAL_col_num_ADDR                           ,x //1 short
    voc_short TABLE_qt_table_ADDR                           ,196,x //[192]shorts
    voc_short GLOBAL_IDCT_BUFFER_X_ADDR                     ,8,x//[8]shorts
    voc_short GLOBAL_row_even_ADDR                          ,x //[1]shorts
    voc_short GLOBAL_col_even_ADDR                          ,x
    voc_short GLOBAL_block_id_ADDR                          ,2,x //[2]shorts
    voc_short GLOBAL_JPEG_IMAGECONTROL_ADDR                 ,x
    voc_short GLOBAL_ImgWidth_ADDR                          ,x
    voc_short GLOBAL_ImgHeight_ADDR                         ,x
    voc_short GLOBAL_Temp_ImgHeight_ADDR                    ,x
    voc_short GLOBAL_temp_ImgWidth_ADDR                     ,x
    voc_short GLOBAL_Temp_ImgWidth_ADDR                     ,x

    voc_struct_end

    voc_struct JPEG_LOCAL_Y_VAR,y

    voc_short LOCAL_IDCT_BUFFER_Y_ADDR                      ,8,y
    voc_short LOCAL_JPEG_IN_BUF_ADDR                        ,448,y
    voc_short LOCAL_VID_BMP_BUF_ADDR                        ,640,y
    voc_word  LOCAL_VID_BUF_OUT_PTR                         ,y

    voc_struct_end

#endif


    VoC_lw32_d(ACC0,GLOBAL_VID_BUF_IN_START);

    VoC_bez16_d(use_jpeg_swap_buffer,GLOBAL_VID_BUF_MODE);

    // if the video buffer mode is 1 (swap)
    // use swap buffer mode (always update in_ptr)
    VoC_sw32_d(ACC0,GLOBAL_VID_BUF_IN_PTR);

use_jpeg_swap_buffer:


    VoC_lw32_d(ACC0,GLOBAL_VID_BUF_OUT_PTR);

    VoC_push16(RA,DEFAULT);

    VoC_sw32_d(ACC0,LOCAL_VID_BUF_OUT_PTR);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,DEFAULT);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,DEFAULT);
    VoC_sw16_d(REG0,CFG_DMA_WRAP);
    VoC_lw16i(STATUS,STATUS_CLR);
    VoC_sw32_d(ACC0,GLOBAL_ycoef_ADDR);//ycoef and ucoef
    VoC_sw16_d(ACC0_HI,GLOBAL_vcoef_ADDR);

    VoC_jal(CII_DMAI_READDATA);

    VoC_lw16i_set_inc(REG0,LOCAL_JPEG_IN_BUF_ADDR,1);//lp=lpJpegBuf+2;
    VoC_lw16i(REG2,0xd8ff);                         //0xffd8
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_lw16inc_p(REG7,REG0,DEFAULT);
    VoC_lw16i(REG3,0xe0ff);                         //0xffe0

    VoC_bne16_rr(Decode_106,REG2,REG6);
    VoC_bne16_rr(Decode_106,REG3,REG7);

    VoC_lw32z(RL6,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);

    VoC_NOP();

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,16,DEFAULT);//llength=0x10

    VoC_shru16_ri(REG4,1,DEFAULT);

    VoC_loop_r(1,REG4)

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_endloop1


    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,16,DEFAULT); //llength=MAKEAWORD(*(lp+1),*lp);
    VoC_lw16i(REG4,80);
    VoC_bgt16_rr(M_DQT_CASE0,REG4,REG5);
    VoC_jump(M_DQT_CASE1);
M_DQT_CASE0:

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_lw16i_set_inc(REG3,TABLE_qt_table_ADDR,1);

    VoC_NOP();
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_loop_i_short(31,DEFAULT);//qt_table[0]
    VoC_startloop0
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_endloop0

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);


    VoC_inc_p(REG0,DEFAULT);//lp+2
    VoC_inc_p(REG0,DEFAULT);//llength=MAKEAWORD(*(lp+1),*lp);

    VoC_loop_i_short(32,DEFAULT);//qt_table[1]
    VoC_startloop0
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_endloop0


    VoC_jump(M_SOF0_CASE);

M_DQT_CASE1:


    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_lw16i_set_inc(REG3,TABLE_qt_table_ADDR,1);
    VoC_NOP();
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_loop_i_short(31,DEFAULT);//qt_table[0]
    VoC_startloop0
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_endloop0

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_loop_i_short(32,DEFAULT);//qt_table[1]
    VoC_startloop0
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_endloop0

M_SOF0_CASE:
    VoC_inc_p(REG0,DEFAULT);//lp+2
    VoC_movreg16(REG6,REG0,DEFAULT);//store the lp
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG7,16,DEFAULT);//llength=MAKEAWORD(*(lp+1),*lp);
    VoC_shru16_ri(REG7,1,DEFAULT);//llength>>1


    VoC_lbinc_p(REG0);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_rbinc_i(REG5,16,DEFAULT);//first_ImgHeight=MAKEAWORD(*(lp+4),*(lp+3));
    VoC_lbinc_p(REG0);

    VoC_add16_rd(REG5,REG5,CONST_JPEG_DEC_15_ADDR);
    VoC_shru16_ri(REG5,4,DEFAULT);
    VoC_shr16_ri(REG5,-4,DEFAULT);//ImgHeight = ((first_ImgHeight+SampRate_Y_V*8-1)/(SampRate_Y_V*8))*(SampRate_Y_V*8);
    VoC_rbinc_i(REG4,16,DEFAULT);//first_ImgWidth=MAKEAWORD(*(lp+6),*(lp+5));

    VoC_sw16_d(REG5,GLOBAL_ImgHeight_ADDR);
    VoC_shr16_ri(REG5,4,DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG5,GLOBAL_Temp_ImgHeight_ADDR);//ImgHeight/16
    VoC_rbinc_i(REG5,8,DEFAULT);


    VoC_add16_rd(REG4,REG4,CONST_JPEG_DEC_15_ADDR);
    VoC_shru16_ri(REG4,4,DEFAULT);
    VoC_shr16_ri(REG4,-4,DEFAULT);//ImgWidth = ((first_ImgWidth+SampRate_Y_H*8-1)/(SampRate_Y_H*8))*(SampRate_Y_H*8);

    VoC_sub16_rd(REG5,REG4,CONST_JPEG_DEC_16_ADDR);
    VoC_add16_rr(REG0,REG6,REG7,DEFAULT);//lp+=llength;
    VoC_sw16_d(REG4,GLOBAL_ImgWidth_ADDR);
    VoC_shr16_ri(REG4,4,DEFAULT);
    VoC_sw16_d(REG5,GLOBAL_temp_ImgWidth_ADDR);//ImgWidth-16
    VoC_sw16_d(REG4,GLOBAL_Temp_ImgWidth_ADDR);//ImgWidth/16

    VoC_inc_p(REG0,DEFAULT);


    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);

    VoC_lbinc_p(REG0);//0xc400
    VoC_movreg16(REG6,REG0,DEFAULT);//store the lp
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_rbinc_i(REG5,16,DEFAULT);//llength=MAKEAWORD(*(lp+1),*lp);
    VoC_rbinc_i(REG7,8,DEFAULT);
    VoC_bngt16_rd(M_DHT_CASE0,REG5,CONST_JPEG_DEC_0xFF_ADDR);
    VoC_movreg16(REG2,REG5,DEFAULT);
    VoC_and16_rd(REG2,CONST_JPEG_DEC_1_ADDR);
    VoC_bez16_r(M_SOS_CASE1,REG2);
    VoC_jump(M_SOS_CASE0);

M_DHT_CASE0:
    VoC_movreg16(REG2,REG5,DEFAULT);
    VoC_and16_rd(REG2,CONST_JPEG_DEC_1_ADDR);
    VoC_bez16_r(DHT1,REG2);

DHT0:
    VoC_add16_rd(REG6,REG6,CONST_JPEG_DEC_1_ADDR);//lp+2
    VoC_shru16_ri(REG5,1,DEFAULT);
    VoC_add16_rr(REG0,REG6,REG5,DEFAULT);//lp+=llength;
    VoC_movreg16(REG6,REG0,DEFAULT);//store the lp
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,16,DEFAULT);//llength=MAKEAWORD(*(lp+1),*lp);

    VoC_movreg16(REG2,REG5,DEFAULT);
    VoC_and16_rd(REG2,CONST_JPEG_DEC_1_ADDR);
    VoC_inc_p(REG3,DEFAULT);
    VoC_blt16_rd(DHT0_CONTINUE,REG3,CONST_JPEG_DEC_3_ADDR);
    VoC_jump(COMPARE0);
DHT0_CONTINUE:
    VoC_bez16_r(DHT0,REG2);
    VoC_add16_rd(REG6,REG6,CONST_JPEG_DEC_1_ADDR);

DHT1:

    VoC_shru16_ri(REG5,1,DEFAULT);
    VoC_add16_rr(REG0,REG6,REG5,DEFAULT);//lp+=llength;

    VoC_NOP();
    VoC_lbinc_p(REG0);
    VoC_movreg16(REG6,REG0,DEFAULT);//store the lp
    VoC_lbinc_p(REG0);//0xc400
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_rbinc_i(REG5,16,DEFAULT);//llength=MAKEAWORD(*(lp+1),*lp);
    VoC_rbinc_i(REG7,8,DEFAULT);



    VoC_movreg16(REG2,REG5,DEFAULT);
    VoC_and16_rd(REG2,CONST_JPEG_DEC_1_ADDR);
    VoC_inc_p(REG3,DEFAULT);
    VoC_blt16_rd(DHT2_CONTINUE,REG3,CONST_JPEG_DEC_3_ADDR);
    VoC_jump(COMPARE1);
DHT2_CONTINUE:
    VoC_bnez16_r(DHT0,REG2);
    VoC_jump(DHT1);

COMPARE0:
    VoC_bez16_r(M_SOS_CASE0,REG2);
    VoC_add16_rd(REG6,REG6,CONST_JPEG_DEC_1_ADDR);
    VoC_jump(M_SOS_CASE1);
COMPARE1:
    VoC_bez16_r(M_SOS_CASE1,REG2);


M_SOS_CASE0:

    VoC_add16_rd(REG6,REG6,CONST_JPEG_DEC_1_ADDR);
    VoC_shru16_ri(REG5,1,DEFAULT);
    VoC_add16_rr(REG0,REG6,REG5,DEFAULT);
    VoC_movreg16(REG6,REG0,DEFAULT);//store the lp
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,16,DEFAULT);//llength=MAKEAWORD(*(lp+1),*lp);


    VoC_shru16_ri(REG5,1,DEFAULT);

    VoC_add16_rr(REG0,REG6,REG5,DEFAULT);//lp+=llength;
    VoC_lw32z(RL7,IN_PARALLEL);//CurByte=0
    VoC_lw16i_short(REG1,0,DEFAULT);//BitPos=0;
    VoC_jump(Decode_begin_work);

M_SOS_CASE1:


    VoC_shru16_ri(REG5,1,DEFAULT);
    VoC_add16_rr(REG0,REG6,REG5,DEFAULT);
    VoC_movreg16(REG6,REG0,DEFAULT);//store the lp
    VoC_lbinc_p(REG0);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_rbinc_i(REG7,8,DEFAULT);

    VoC_shru16_ri(REG5,1,DEFAULT);
    VoC_add16_rr(REG0,REG6,REG5,DEFAULT);//lp+=llength;

    VoC_NOP();

    VoC_lw16inc_p(REG2,REG0,DEFAULT);
    VoC_lw16i_short(REG1,8,IN_PARALLEL);//BitPos=8;
    VoC_shru16_ri(REG2,8,DEFAULT);
    VoC_movreg16(RL7_LO,REG2,DEFAULT);//CurByte
    VoC_lw16i_short(RL7_HI,0,DEFAULT);
//initial  finish


//now jpeg decoder begin work


Decode_begin_work:


    VoC_lw16i_short(REG5,0,DEFAULT);//sizei
    VoC_lw16i_short(REG7,0,IN_PARALLEL);//DEFAULT original image
    VoC_lw16_d(REG7,GLOBAL_ZOOM_MODE);//image_control
    VoC_sw16_d(REG5,GLOBAL_temp_Y_data_ADDR);
    VoC_sw16_d(REG7,GLOBAL_image_control_ADDR);
Decode_100:

    VoC_lw16i_short(REG3,0,DEFAULT); //sizej
    VoC_lw16i_set_inc(REG2,GLOBAL_rgb_buf_ADDR,1);

    VoC_sw16_d(REG5,GLOBAL_block_id_ADDR);
    VoC_push16(REG5,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
Decode_101:

    VoC_push32(REG23,DEFAULT);

    VoC_jal(CII_DecodeMCUBlock);

    VoC_lw32_sd(REG23,0,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_jal(CII_StoreBuffer);

    VoC_pop32(REG23,DEFAULT);//pop the old rgb_buf address
    VoC_lw16_d(REG7,GLOBAL_image_control_ADDR);

    VoC_be16_rd(small_image_case,REG7,CONST_JPEG_DEC_2_ADDR);

    VoC_add16_rd(REG2,REG2,CONST_JPEG_DEC_8_ADDR);
small_image_case:
    VoC_add16_rd(REG2,REG2,CONST_JPEG_DEC_8_ADDR);// StoreBuffer((unsigned char *)(RGB_buf+(DWORD)(sizei*lbyte+sizej*3)));

    VoC_add16_rd(REG3,REG3,CONST_JPEG_DEC_1_ADDR);
    // GLOBAL_Temp_ImgWidth_ADDR=176 >> 4=11    320 >>4=20
    VoC_blt16_rd(Decode_101,REG3,GLOBAL_Temp_ImgWidth_ADDR);// sizej>=ImgWidth

    VoC_be16_rd(Decode_enlarge_picture,REG7,CONST_JPEG_DEC_1_ADDR);


    VoC_jal(CII_DMAI_WRITEDATA);


    VoC_pop16(REG5,DEFAULT);
    VoC_jump(Decode_102);
Decode_enlarge_picture:
    VoC_sw16_d(REG0,GLOBAL_temp_lpJpegBuf_ADDR);//store the old lpJpegBuf address



//DMAE WRITE DATA begin(CALL 9 times)

    VoC_pop16(REG5,DEFAULT);


    VoC_push16(REG1,DEFAULT);//BitPos
    VoC_push32(RL7,IN_PARALLEL);//CurByte
    VoC_push16(REG5,DEFAULT);

    VoC_jal(CII_zoom);

    VoC_pop16(REG5,DEFAULT);
    VoC_pop16(REG1,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);

Decode_102:
    VoC_add16_rd(REG5,REG5,CONST_JPEG_DEC_1_ADDR);//GLOBAL_Temp_ImgHeight_ADDR = 144 >> 4=9
    //240 >> 4=15
    VoC_lw16_d(REG0,GLOBAL_temp_lpJpegBuf_ADDR);//load the old lpJpegBuf address

    VoC_bnlt16_rd(Decode_103,REG5,GLOBAL_Temp_ImgHeight_ADDR);// sizei>=ImgHeight
    VoC_jump(Decode_100);
Decode_103:
    VoC_lw16_d(REG7,GLOBAL_image_control_ADDR);
    VoC_bez16_r(Decode_105,REG7);
    VoC_lw16i(REG4,320);
    VoC_lw16i(REG5,240);
    VoC_bne16_rd(Decode_104,REG7,CONST_JPEG_DEC_2_ADDR);

    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_shr16_ri(REG5,1,DEFAULT);
Decode_104:
    VoC_sw16_d(REG4,GLOBAL_ImgWidth_ADDR);
    VoC_sw16_d(REG5,GLOBAL_ImgHeight_ADDR);
Decode_105:
    VoC_lw32z(ACC0,DEFAULT);

    VoC_lw16_d(REG4,GLOBAL_ImgWidth_ADDR);
    VoC_lw16_d(REG5,GLOBAL_ImgHeight_ADDR);
    VoC_lw16i_short(REG3,0,DEFAULT);

    VoC_sw16_d(REG4,GLOBAL_IMG_WIDTH);
    VoC_sw16_d(REG5,GLOBAL_IMG_HEIGHT);
    VoC_jump(Decode_107);
Decode_106:
    VoC_lw16i_short(REG4,-1,DEFAULT);
    VoC_sw16_d(REG4,GLOBAL_ERR_STATUS);

Decode_107:

    // find next sync word (if available)
    VoC_jal(CII_DMAI_SYNCDATA);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;

}

void CII_zoom(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_lw16_d(REG7,GLOBAL_block_id_ADDR);//REG7:block id   (0~8)


    VoC_lw16i_short(REG4,16,DEFAULT);
    VoC_be16_rd(zoom_begin,REG7,CONST_JPEG_DEC_0_ADDR)

    VoC_lw16i_short(REG4,24,DEFAULT);
    VoC_be16_rd(zoom_begin,REG7,CONST_JPEG_DEC_1_ADDR)

    VoC_lw16i(REG4,32);
    VoC_blt16_rd(zoom_begin,REG7,CONST_JPEG_DEC_8_ADDR)

    VoC_lw16i_short(REG4,16,DEFAULT);

zoom_begin:

    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_sub16_rd(REG4,REG4,CONST_JPEG_DEC_1_ADDR);

    VoC_lw16i_set_inc(REG1,LOCAL_VID_BMP_BUF_ADDR,1);
    VoC_loop_r(1,REG4);

    VoC_movreg16(REG2,REG3,DEFAULT);//i1
    VoC_push16(REG3,DEFAULT);
    VoC_and16_rd(REG2,CONST_JPEG_DEC_1_ADDR);
    VoC_lw16i_set_inc(REG0,TABLE_add_para_ADDR,1);
    VoC_bnez16_r(zoom_100,REG2);
    VoC_movreg16(REG6,REG3,DEFAULT);
    VoC_movreg16(REG7,REG3,IN_PARALLEL);
    VoC_sw16_d(REG2,GLOBAL_row_even_ADDR);
    VoC_shr16_ri(REG6,1,DEFAULT);//  m_min = (i1>>1)
    VoC_shr16_ri(REG7,1,IN_PARALLEL);
    VoC_jump(zoom_101);
zoom_100:
    VoC_movreg16(REG6,REG3,DEFAULT);
    VoC_sw16_d(REG2,GLOBAL_row_even_ADDR);
    VoC_shr16_ri(REG6,1,DEFAULT);//  m_min = (i1>>1)
    VoC_add16_rd(REG7,REG6,CONST_JPEG_DEC_1_ADDR);//m_max = m_min +1;

zoom_101:

    VoC_lw16i(REG3,176);

    VoC_mult16_rr(REG6,REG6,REG3,DEFAULT);//m_min*wid
    VoC_mult16_rr(REG7,REG7,REG3,DEFAULT);//m_max*wid

    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i(REG2,GLOBAL_rgb_buf_ADDR);
    VoC_movreg16(ACC0_LO,REG6,DEFAULT);
    VoC_movreg16(ACC0_HI,REG6,IN_PARALLEL);
    VoC_movreg16(ACC1_LO,REG7,DEFAULT);
    VoC_movreg16(ACC1_HI,REG7,IN_PARALLEL);
    VoC_movreg16(RL6_LO,REG2,DEFAULT);//rgb_buf
    VoC_movreg16(RL6_HI,REG2,IN_PARALLEL);//rgb_buf
    VoC_sw16_d(REG3,GLOBAL_col_num_ADDR);

zoom_105:

    VoC_loop_i(0,160);


    VoC_lw16inc_p(REG2,REG0,DEFAULT);//j = add_para[160*j1+j2];
    VoC_movreg16(REG3,REG2,DEFAULT);
    VoC_push16(REG0,IN_PARALLEL);
    VoC_and16_rd(REG3,CONST_JPEG_DEC_1_ADDR);

    VoC_bnez16_r(zoom_102,REG3);
    VoC_movreg16(REG4,REG2,DEFAULT);
    VoC_movreg16(REG5,REG2,IN_PARALLEL);
    VoC_sw16_d(REG3,GLOBAL_col_even_ADDR);
    VoC_shr16_ri(REG4,1,DEFAULT);//   n_min = (j>>1)
    VoC_shr16_ri(REG5,1,IN_PARALLEL);//n_max = n_min;
    VoC_jump(zoom_103);
zoom_102:
    VoC_movreg16(REG4,REG2,DEFAULT);
    VoC_sw16_d(REG3,GLOBAL_col_even_ADDR);
    VoC_shr16_ri(REG4,1,DEFAULT); //  n_min = (j>>1)
    VoC_add16_rd(REG5,REG4,CONST_JPEG_DEC_1_ADDR);//n_max = n_min +1;
zoom_103:

//          piex_min_min    =   *(rgb_buf +m_min*wid+n_min);
//          piex_min_max    =   *(rgb_buf +m_min*wid+n_max);
//          piex_max_min    =   *(rgb_buf +m_max*wid+n_min);
//          piex_max_max    =   *(rgb_buf +m_max*wid+n_max);
    VoC_add32_rr(RL7,ACC0,REG45,DEFAULT);
    VoC_add32_rr(REG67,ACC1,REG45,IN_PARALLEL);
    VoC_add32_rr(REG45,RL6,RL7,DEFAULT);//piex_min_min&piex_min_max
    VoC_add32_rr(REG67,RL6,REG67,IN_PARALLEL);//piex_max_min&piex_max_max
    VoC_lw16_d(REG3,GLOBAL_row_even_ADDR);
    VoC_lw16_p(REG4,REG4,DEFAULT);
    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_movreg16(REG0,REG4,IN_PARALLEL);
    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_movreg16(REG2,REG4,IN_PARALLEL);
    VoC_lw16_p(REG7,REG7,DEFAULT);

    VoC_shru16_ri(REG4,11,IN_PARALLEL);
    VoC_and16_ri(REG0,0x7ff);

    VoC_shru16_ri(REG0,5,DEFAULT);
    VoC_and16_ri(REG2,0x1f);


    VoC_and16_rd(REG3,CONST_JPEG_DEC_1_ADDR);
    VoC_bnez16_r(zoom_col,REG3);

    VoC_movreg16(REG6,REG5,DEFAULT);
    VoC_movreg16(REG7,REG5,IN_PARALLEL);

    VoC_and16_ri(REG6,0x7ff);
    VoC_shru16_ri(REG5,11,DEFAULT);
    VoC_shru16_ri(REG6,5,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);
    VoC_shr16_ri(REG4,1,DEFAULT);//R  =   ( R_min_min+ R_min_max ) >>1;
    VoC_lw32z(RL7,IN_PARALLEL);
    VoC_and16_ri(REG7,0x1f);

    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);
    VoC_shr16_ri(REG0,1,DEFAULT);//G  =   ( G_min_min + G_min_max) >>1;
    VoC_shr16_ri(REG2,1,IN_PARALLEL);//B  =   ( B_min_min + B_min_max ) >>1;

    VoC_jump(zoom_104);

zoom_col:
    VoC_lw16_d(REG3,GLOBAL_col_even_ADDR);
    VoC_and16_rd(REG3,CONST_JPEG_DEC_1_ADDR);
    VoC_bnez16_r(zoom_col_odd,REG3);


    VoC_movreg16(REG5,REG6,DEFAULT);
    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    VoC_and16_ri(REG5,0x7ff);
    VoC_shru16_ri(REG6,11,DEFAULT);//piex_min_max>>11
    VoC_shr16_ri(REG5,5,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);
    VoC_shru16_ri(REG4,1,DEFAULT);//R =   ( R_min_min + R_max_min ) >>1;
    VoC_lw32z(RL7,IN_PARALLEL);
    VoC_and16_ri(REG7,0x1f);

    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);
    VoC_shru16_ri(REG0,1,DEFAULT);//G =   ( G_min_min+ G_max_min) >>1;
    VoC_shru16_ri(REG2,1,IN_PARALLEL);//B =   ( B_min_min+ B_max_min) >>1;

    VoC_jump(zoom_104);

zoom_col_odd:
    VoC_push16(REG5,DEFAULT);//piex_min_max
    VoC_push32(REG67,IN_PARALLEL);//piex_max_min&piex_max_max

    VoC_shru16_ri(REG5,11,DEFAULT);
    VoC_shru16_ri(REG6,11,IN_PARALLEL);
    VoC_shru16_ri(REG7,11,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_lw16_sd(REG5,0,IN_PARALLEL);
    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);
    VoC_lw32_sd(REG67,0,IN_PARALLEL);



    VoC_and16_ri(REG5,0x7ff);
    VoC_and16_ri(REG6,0x7ff);
    VoC_and16_ri(REG7,0x7ff);
    VoC_shr16_ri(REG4,2,DEFAULT);//R  =( R_min_min + R_min_max+ R_max_min + R_max_max) >>2;
    VoC_shru16_ri(REG5,5,IN_PARALLEL);

    VoC_shru16_ri(REG6,5,DEFAULT);
    VoC_shru16_ri(REG7,5,IN_PARALLEL);



    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_pop16(REG5,IN_PARALLEL);
    VoC_shr16_ri(REG0,2,DEFAULT);//G  =( G_min_min +G_min_max + G_max_min+ G_max_max ) >>2;


    VoC_pop32(REG67,IN_PARALLEL);

    VoC_and16_ri(REG5,0x1f);
    VoC_and16_ri(REG6,0x1f);
    VoC_and16_ri(REG7,0x1f);


    VoC_add16_rr(REG2,REG2,REG5,DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);
    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
    VoC_shr16_ri(REG2,2,DEFAULT);//B =( B_min_min + B_min_max + B_max_min + B_max_max ) >>2;
zoom_104:
    VoC_movreg16(RL7_HI,REG4,DEFAULT);
    VoC_shr32_ri(RL7,5,DEFAULT);
    VoC_movreg16(REG4,RL7_LO,DEFAULT);
    VoC_shr16_ri(REG0,-5,IN_PARALLEL);
    VoC_and16_ri(REG0,0x7ff);
    VoC_and16_ri(REG2,0x7ff);
    VoC_or16_rr(REG4,REG0,DEFAULT);
    VoC_or16_rr(REG4,REG2,DEFAULT);//(R<<11) |((G<<5) & 0x07ff) | (B & 0x001f);
    VoC_pop16(REG0,DEFAULT);
    VoC_sw16inc_p(REG4,REG1,DEFAULT);

    VoC_endloop0

    VoC_lw16_d(REG2,GLOBAL_col_num_ADDR);
    VoC_add16_rd(REG2,REG2,CONST_JPEG_DEC_1_ADDR);
    VoC_lw16_d(REG5,GLOBAL_row_even_ADDR);

    VoC_sw16_d(REG2,GLOBAL_col_num_ADDR);
    VoC_bgt16_rd(zoom_106,REG2,CONST_JPEG_DEC_1_ADDR);
    VoC_jump(zoom_105);
zoom_106:
    VoC_bne16_rd(zoom_contiune,REG5,CONST_JPEG_DEC_1_ADDR);

    VoC_jal(CII_DMAI_WRITEISPDATA);
    VoC_lw16i_set_inc(REG1,LOCAL_VID_BMP_BUF_ADDR,1);
zoom_contiune:
    VoC_pop16(REG3,DEFAULT);//i1
    VoC_NOP();
    VoC_inc_p(REG3,DEFAULT);

    VoC_endloop1
    VoC_lw16i_set_inc(REG1,LOCAL_VID_BMP_BUF_ADDR,1);
    VoC_lw16i_set_inc(REG2,LOCAL_VID_BMP_BUF_ADDR+320,1);
    VoC_loop_i(1,2);
    VoC_loop_i(0,160);
    VoC_lw16inc_p(REG3,REG1,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG3,REG2,DEFAULT);
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1
    VoC_jal(CII_DMAI_WRITEISPDATA);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


/*void CII_DMAI_READDATA(void)
{
    VoC_lw16i(REG4,224);
    VoC_lw16i(REG5,(LOCAL_JPEG_IN_BUF_ADDR)/2);

    VoC_lw32_d(ACC1,GLOBAL_VID_BUF_IN_PTR);
    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_shru16_ri(REG4,-2,IN_PARALLEL);
    VoC_add32_rr(ACC1,ACC1,REG45,DEFAULT);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();

    // clear the event
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_sw32_d(ACC1,GLOBAL_VID_BUF_IN_PTR);

    VoC_return;
}*/



void CII_DMAI_READDATA(void)
{
    VoC_push32(RL6,DEFAULT);
    VoC_push16(REG3,IN_PARALLEL);

    VoC_lw16i(REG4,(LOCAL_JPEG_IN_BUF_ADDR)/2);
    VoC_lw16i(REG5,224);

    VoC_lw32_d(RL6,GLOBAL_VID_BUF_IN_PTR);

    // cast load size to 32-bit
    VoC_movreg16(ACC1_LO,REG5,DEFAULT);
    VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);
    VoC_shr32_ri(ACC1,-2,DEFAULT);

    // compute end load pointer
    VoC_add32_rr(ACC1,ACC1,RL6,DEFAULT);

    // check external buffer wrap condition
    VoC_blt32_rd(no_ext_vidbuff_wrap,ACC1,GLOBAL_VID_BUF_IN_END)

    // REG3 : size until end of buffer in ints
    VoC_sub32_dr(ACC1,GLOBAL_VID_BUF_IN_END,RL6);
    VoC_shru32_ri(ACC1,2,DEFAULT);
    VoC_movreg16(REG3,ACC1_LO,DEFAULT);

    // read until the end of the buffer
    VoC_sw16_d(REG4,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(RL6,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    // subtract read value, increment laddr
    VoC_add16_rr(REG4,REG4,REG3,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG3,IN_PARALLEL);

    // read from the buffer start
    VoC_lw32_d(RL6,GLOBAL_VID_BUF_IN_START);

no_ext_vidbuff_wrap:

    // check if size is not NULL
    VoC_bez16_r(ext_vidbuff_load_done,REG5)

    // no need to reconfigure wrap
    VoC_sw16_d(REG4,CFG_DMA_LADDR);
    VoC_sw16_d(REG5,CFG_DMA_SIZE);
    VoC_sw32_d(RL6,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    // compute the new eaddr
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_shru32_ri(REG45,14,DEFAULT);
    VoC_add32_rr(RL6,RL6,REG45,DEFAULT);

ext_vidbuff_load_done:

    VoC_pop32(RL6,DEFAULT);
    VoC_pop16(REG3,IN_PARALLEL);

    // new laddr
    VoC_sw32_d(RL6,GLOBAL_VID_BUF_IN_PTR);

    VoC_return;
}


void CII_DMAI_SYNCDATA(void)
{
    // sync only if the buffer mode is circular
    VoC_bnez16_d(no_sync_necessary,GLOBAL_VID_BUF_MODE);

    // find the next frame sync word
    VoC_lw16i(RL6_LO,0xd8ff);                         //0xffd8
    VoC_lw16i(RL6_HI,0xe0ff);                         //0xffe0

    VoC_lw16i_set_inc(REG0,223*4,-4);
    VoC_lw16i_set_inc(REG1,LOCAL_JPEG_IN_BUF_ADDR+2,2);

    VoC_loop_i(0,223)

    VoC_lw32inc_p(RL7,REG1,DEFAULT);
    VoC_be32_rr(jpeg_sync_found,RL6,RL7);
    VoC_inc_p(REG0,DEFAULT);

    VoC_endloop0

jpeg_sync_found:

    VoC_lw16i_short(REG1,0,DEFAULT);

    VoC_sub32_dr(RL6,GLOBAL_VID_BUF_IN_PTR,REG01);

    VoC_bnlt32_rd(no_dewrap_buf,RL6,GLOBAL_VID_BUF_IN_START);

    VoC_add32_rd(RL6,RL6,GLOBAL_VID_BUF_IN_END);
    VoC_sub32_rd(RL6,RL6,GLOBAL_VID_BUF_IN_START);

no_dewrap_buf:

    VoC_NOP();
    VoC_sw32_d(RL6,GLOBAL_VID_BUF_IN_PTR);

no_sync_necessary:

    VoC_return;
}







void CII_DMAI_WRITEDATA(void)
{

    // -------------------------------------------------------------------------
    // This version of the write & rotate function is implementing an optimized
    // mechanism for the transfers : the program execution is not stalled after
    // the dma programming, but just before the next dma programming. This makes
    // the dma writing less dependent on the AHB bus latency, as we only stall
    // after the next transfer is prefetched in the source temporary buffer.
    // As the previous transfer may not be completed while we prefetch new data,
    // a swap buffer mechanism needs to be used.
    // -------------------------------------------------------------------------

#if 0
    // We need 2*16 short for a 16-short swap buffer aligned on 32.
    // So we define a twice bigger buffer, and align inside as shown below...
    voc_short LOCAL_JPEG_DMA_TMP,63,x
    voc_short LOCAL_ImgWidth_ADDR,y
#endif

    // compute effective immage width
    VoC_lw16_d(REG4,GLOBAL_ImgWidth_ADDR);
    VoC_lw16_d(REG5,GLOBAL_RESIZE_TO_WIDTH);

    // use original size if the parameter is invalid ( < 1)
    VoC_bngtz16_r(image_width_unchanged,REG5)
    // use original size if the parameter is bigger than the actual size
    VoC_bngt16_rr(image_width_unchanged,REG4,REG5)

    // resize if the image width is
    // strictly bigger than the requested size

    // -----------------
    // reorder the lines
    // -----------------

    // compute the offset
    VoC_movreg16(REG4,REG5,DEFAULT);
    VoC_sub16_rr(REG5,REG4,REG5,IN_PARALLEL);

    // resize should be a multiple of 4
    VoC_and16_ri(REG4,0xfffc);

    // cut the same size from both sides
    // compute the number of loops
    VoC_shr16_ri(REG5,1,DEFAULT);
    VoC_movreg16(REG6,REG4,IN_PARALLEL);
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);

    // (GLOBAL_RESIZE_TO_WIDTH/2) - 1
    VoC_sub16_rd(REG6,REG6,CONST_JPEG_DEC_1_ADDR);

    VoC_lw16i_set_inc(REG0,GLOBAL_rgb_buf_ADDR,2);
    VoC_lw16i_set_inc(REG1,GLOBAL_rgb_buf_ADDR,2);

    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_shr16_ri(REG5,-1,IN_PARALLEL);

    // for all 16 lines
    VoC_loop_i(1,16)

    VoC_loop_r_short(REG6,DEFAULT);
    VoC_lw32inc_p(RL6,REG0,IN_PARALLEL);
    VoC_startloop0

    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_sw32inc_p(RL6,REG1,DEFAULT);

    VoC_endloop0

    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_sw32inc_p(RL6,REG1,DEFAULT);

    VoC_endloop1

    VoC_pop32(REG01,DEFAULT);
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);


image_width_unchanged:


    VoC_sw16_d(REG4,LOCAL_ImgWidth_ADDR);

    VoC_lw16i_short(REG5,VPP_AMJP_ROTATE_NO,DEFAULT);
    VoC_bne16_rd(rotation_required,REG5,GLOBAL_ROTATE_MODE)

    // ----------------------------------
    // case without rotation
    // ----------------------------------

    VoC_bne16_rd(Decode_original_picture,REG7,CONST_JPEG_DEC_2_ADDR);
    VoC_lw16i(REG4,80);
    VoC_jump(DMA_begin);
Decode_original_picture:
    VoC_lw16_d(REG4,LOCAL_ImgWidth_ADDR);

DMA_begin:
    VoC_lw16i(REG5,(GLOBAL_rgb_buf_ADDR)/2);
    VoC_shr16_ri(REG4,-3,DEFAULT);
    VoC_lw32_d(RL6,LOCAL_VID_BUF_OUT_PTR);

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(RL6,CFG_DMA_EADDR);

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_shru16_ri(REG4,-2,IN_PARALLEL);
    VoC_add32_rr(RL6,RL6,REG45,DEFAULT);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();

    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_sw32_d(RL6,LOCAL_VID_BUF_OUT_PTR);
    VoC_return;


rotation_required:

    VoC_push32(REG01,DEFAULT);
    VoC_push32(REG23,DEFAULT);
    VoC_push32(REG67,DEFAULT);
    VoC_push32(RL7,DEFAULT);

    VoC_lw16i_short(REG5,VPP_AMJP_ROTATE_180,DEFAULT);
    VoC_lw16_d(REG4,GLOBAL_ROTATE_MODE);
    VoC_bne16_rr(rotation_90_or_270_required,REG5,REG4)

    // ----------------------------------
    // case when 180? rotation is required
    // ----------------------------------

    // w*32 in REG67
    VoC_lw16_d(REG6,LOCAL_ImgWidth_ADDR);
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_shr32_ri(REG67,-5,DEFAULT);

    // compute the pointer for the next call
    VoC_sub32_dr(RL6,LOCAL_VID_BUF_OUT_PTR,REG67);

    // 2*w in REG6
    VoC_shr32_ri(REG67,4,DEFAULT);

    // 2*w*h in RL7 (FORMAT32 = 0)
    VoC_multf32_rd(RL7,REG6,GLOBAL_ImgHeight_ADDR);

    // store the pointer for the next call
    VoC_sw32_d(RL6,LOCAL_VID_BUF_OUT_PTR);

    // compute the pointer for the current transfer
    VoC_add32_rr(RL6,RL6,RL7,DEFAULT);

    // 16*w in REG6
    VoC_shr32_ri(REG67,-3,DEFAULT);

    // REG1 = s + w*16 - 1
    VoC_lw16i_set_inc(REG1,GLOBAL_rgb_buf_ADDR-1,-1);
    VoC_add16_rr(REG1,REG1,REG6,DEFAULT);

    // REG0 = s
    VoC_lw16i_set_inc(REG0,GLOBAL_rgb_buf_ADDR,1);

    // w/4 in REG6 (the width should be a multiple of 4)
    VoC_shr32_ri(REG67,6,DEFAULT);

    // loop (4*8)*(w/4) times
    VoC_loop_i(1,32)

    VoC_loop_r_short(REG6,DEFAULT)
    VoC_startloop0

    // swap the pixels
    VoC_lw16_p(REG4,REG0,DEFAULT);
    VoC_lw16_p(REG5,REG1,DEFAULT);
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_sw16inc_p(REG5,REG0,DEFAULT);

    VoC_endloop0

    VoC_NOP();

    VoC_endloop1

    // w*8 in REG6
    VoC_shr32_ri(REG67,-5,DEFAULT);
    VoC_lw16i(REG7,(GLOBAL_rgb_buf_ADDR)/2);

    VoC_NOP();

    VoC_sw32_d(REG67,CFG_DMA_SIZE);
    VoC_sw32_d(RL6,CFG_DMA_EADDR);

    VoC_jump(rotation_end);


rotation_90_or_270_required:

    // ----------------------------------
    // case when +/- 90? rotation is required
    // ----------------------------------

    // 32L in RL6
    VoC_lw16i_short(RL6_HI,0,DEFAULT);
    VoC_lw16i_short(RL6_LO,32,IN_PARALLEL);

    // 2*h in RL7
    VoC_lw16i_short(RL7_HI,0,DEFAULT);
    VoC_lw16_d(RL7_LO,GLOBAL_ImgHeight_ADDR);
    VoC_shr32_ri(RL7,-1,DEFAULT);

    // i' = w in REG7
    VoC_lw16_d(REG7,LOCAL_ImgWidth_ADDR);

    // 17*w + 1 in REG6
    VoC_lw16i_short(REG6,17,DEFAULT);
    VoC_mult16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG0,DEFAULT);

    // w in REG4
    VoC_movreg16(REG4,REG7,IN_PARALLEL);

    // src in REG1
    VoC_lw16i(REG1,GLOBAL_rgb_buf_ADDR);

    VoC_lw16i_short(REG0,VPP_AMJP_ROTATE_90,DEFAULT);
    VoC_bne16_rd(rotation_270,REG0,GLOBAL_ROTATE_MODE)

    // dst -= 32
    VoC_sub32_dr(RL6,LOCAL_VID_BUF_OUT_PTR,RL6);

    // 15*w in REG2
    VoC_lw16i_short(REG2,15,DEFAULT);
    VoC_mult16_rr(REG2,REG2,REG7,DEFAULT);

    // new dst pointer (for next fct call)
    VoC_sw32_d(RL6,LOCAL_VID_BUF_OUT_PTR);

    // s + 15*w in REG1
    VoC_add16_rr(REG1,REG1,REG2,DEFAULT);

    VoC_jump(rotation_90_end);

rotation_270:

    // dst += 32
    VoC_add32_rd(RL6,RL6,LOCAL_VID_BUF_OUT_PTR);

    // dst -= 2*h (eaddr)
    VoC_sub32_dr(RL6,LOCAL_VID_BUF_OUT_PTR,RL7);

    // new dst pointer (for next fct call)
    VoC_sw32_d(RL6,LOCAL_VID_BUF_OUT_PTR);

    // s + w - 1 in REG1
    VoC_sub16_rr(REG1,REG1,REG0,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);

    // invert REG4 and REG6
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_sub16_rr(REG4,REG2,REG4,DEFAULT);
    VoC_sub16_rr(REG6,REG2,REG6,DEFAULT);


rotation_90_end:

    // first partial line, decrement i'
    VoC_sub16_rr(REG7,REG7,REG0,DEFAULT);

    // &tmp
    // here we align the pointer to 32-short and set the wrap register to 32 = 2^5
    VoC_lw16i_set_inc(REG0,((LOCAL_JPEG_DMA_TMP + 31) & 0xffe0),1);
    VoC_lw16i_short(WRAP0,5,DEFAULT);
    VoC_movreg16(REG3,REG0,IN_PARALLEL);
    VoC_shr16_ri(REG3,1,DEFAULT);

    // here REG1 = &src[15*w] for 90?
    // here REG1 = &src[w-1] for 270?

    // preload data
    VoC_lw16_p(REG5,REG1,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG4,IN_PARALLEL);

    // dst += (2*h)
    // for (j = 0; j < 16; ++j)
    VoC_loop_i_short(16,DEFAULT)
    VoC_add32_rr(RL6,RL6,RL7,IN_PARALLEL);
    VoC_startloop0

    // tmp[i] = src[(15-j)*w] for 90?
    // tmp[i] = src[j*w-1] for 270?
    VoC_lw16_p(REG5,REG1,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG4,IN_PARALLEL);
    VoC_sw16inc_p(REG5,REG0,DEFAULT);

    VoC_endloop0

    // transfer 16 pixels
    VoC_lw16i_short(REG2,8,DEFAULT);

    // here REG1 = &src[-2*w], so add (17*w+1) for 90?
    // here REG1 = &src[18*w-1], so sub (17*w+1) for 270?
    VoC_add16_rr(REG1,REG1,REG6,DEFAULT);

    // launch the first dma and do not stall
    VoC_sw32_d(REG23,CFG_DMA_SIZE);
    VoC_sw32_d(RL6,CFG_DMA_EADDR);


    // for (i = 1, i' = w-1; i < w; ++i, --i')
continue_dma_90:

    // here REG1 = &src[15*w+i] for 90?
    // here REG1 = &src[w-1-i] for 270?

    // get new ladd
    VoC_movreg16(REG3,REG0,DEFAULT);
    VoC_shr16_ri(REG3,1,DEFAULT);

    // preload data
    VoC_lw16_p(REG5,REG1,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG4,IN_PARALLEL);

    // dst += (2*h)
    // for (j = 0; j < 16; ++j)
    VoC_loop_i_short(16,DEFAULT)
    VoC_add32_rr(RL6,RL6,RL7,IN_PARALLEL);
    VoC_startloop0

    // tmp[i] = src[(15-j)*w+i] for 90?
    // tmp[i] = src[j*w-1-i] for 270?
    VoC_lw16_p(REG5,REG1,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG4,IN_PARALLEL);

    // the pointer to the temporary source buffer is wrapping on 32-short
    // so it will alternatively fill the lower and then the upper part
    // of the 32-short swap buffer
    VoC_sw16inc_p(REG5,REG0,DEFAULT);

    VoC_endloop0

    // here REG1 = &src[-2*w+i], so add (17*w+1) for 90?
    // here REG1 = &src[18*w-1-i], so sub (17*w+1) for 270?
    VoC_add16_rr(REG1,REG1,REG6,DEFAULT);

    // wait for the completion of the previous dma.
    // At best, the transfer will already be completed, so
    // the program will not go to sleep at all!...
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();

    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    // program the new dma transfer
    VoC_sw32_d(REG23,CFG_DMA_SIZE);
    VoC_sw32_d(RL6,CFG_DMA_EADDR);

    VoC_sub16_rd(REG7,REG7,CONST_JPEG_DEC_1_ADDR);

    VoC_bnez16_r(continue_dma_90,REG7);


rotation_end:

    // wait for the completion of the last dma.
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();

    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_pop32(RL7,DEFAULT);
    VoC_pop32(REG67,DEFAULT);

    VoC_pop32(REG23,DEFAULT);
    VoC_lw16i_short(WRAP0,0,IN_PARALLEL);

    VoC_pop32(REG01,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_return;
}


/*void CII_DMAI_WRITEDATA(void)
{

    VoC_bne16_rd(Decode_original_picture,REG7,CONST_JPEG_DEC_2_ADDR);
    VoC_lw16i(REG4,80);
    VoC_jump(DMA_begin);
Decode_original_picture:
    VoC_lw16_d(REG4,GLOBAL_ImgWidth_ADDR);

DMA_begin:
    VoC_lw16i(REG5,(GLOBAL_rgb_buf_ADDR)/2);
    VoC_shr16_ri(REG4,-3,DEFAULT);
    VoC_lw32_d(RL6,LOCAL_VID_BUF_OUT_PTR);

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(RL6,CFG_DMA_EADDR);

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_shru16_ri(REG4,-2,IN_PARALLEL);
    VoC_add32_rr(RL6,RL6,REG45,DEFAULT);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();

    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_sw32_d(RL6,LOCAL_VID_BUF_OUT_PTR);
    VoC_return;
}*/


/*void CII_DMAI_WRITEDATA_90(void)
{

#if 0
    voc_word LOCAL_JPEG_DMA_TMP,8,x
#endif

    VoC_push32(REG01,DEFAULT);
    VoC_push32(REG23,DEFAULT);
    VoC_push32(REG67,DEFAULT);
    VoC_push32(RL7,DEFAULT);

    // dst = dts0 - 32
    VoC_lw16i_short(RL7_HI,0,DEFAULT);
    VoC_lw16i_short(RL7_LO,32,IN_PARALLEL)
    VoC_sub32_dr(RL6,LOCAL_VID_BUF_OUT_PTR,RL7);

    // 2*h in RL7
    VoC_lw16_d(RL7_LO,GLOBAL_ImgHeight_ADDR);
    VoC_shr32_ri(RL7,-1,DEFAULT);

    // i' = w in REG7
    VoC_lw16_d(REG7,GLOBAL_ImgWidth_ADDR);

    // s + 15*w in REG1
    VoC_lw16i_short(REG4,15,DEFAULT);
    VoC_lw16i_short(REG2,8,IN_PARALLEL);
    VoC_mult16_rr(REG4,REG4,REG7,DEFAULT);
    VoC_lw16i(REG1,GLOBAL_rgb_buf_ADDR);
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);

    // 17*w + 1 in REG6
    VoC_lw16i_short(REG6,17,IN_PARALLEL);
    VoC_mult16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG0,DEFAULT);

    // w in REG4
    VoC_movreg16(REG4,REG7,IN_PARALLEL);

    // new dst pointer
    VoC_sw32_d(RL6,LOCAL_VID_BUF_OUT_PTR);

    // laddr
    VoC_lw16i(REG3,(LOCAL_JPEG_DMA_TMP/2));


    // for (i = 0, i' = w; i < w; ++i, --i')
continue_dma_90:

        // here REG1 = &src[15*w + i]
        VoC_lw16i_set_inc(REG0,LOCAL_JPEG_DMA_TMP,1);

        // preload data
        VoC_lw16_p(REG5,REG1,DEFAULT);
        VoC_sub16_rr(REG1,REG1,REG4,IN_PARALLEL);

        // for (j = 0; j < 16; ++j)
        // dst += (2*h)
        VoC_loop_i_short(16,DEFAULT)
        VoC_add32_rr(RL6,RL6,RL7,IN_PARALLEL);
        VoC_startloop0

            // tmp[i] = src[(15-j)*w +i]
            VoC_lw16_p(REG5,REG1,DEFAULT);
            VoC_sub16_rr(REG1,REG1,REG4,IN_PARALLEL);
            VoC_sw16inc_p(REG5,REG0,DEFAULT);

        VoC_endloop0

        // here REG1 = &src[-2*w + i], so add (17*w + 1)
        VoC_add16_rr(REG1,REG1,REG6,DEFAULT);

        VoC_sw32_d(REG23,CFG_DMA_SIZE);
        VoC_sw32_d(RL6,CFG_DMA_EADDR);

        VoC_cfg(CFG_CTRL_STALL);
        VoC_NOP();

        VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

        VoC_sub16_rd(REG7,REG7,CONST_JPEG_DEC_1_ADDR);

    VoC_bnez16_r(continue_dma_90,REG7);

    VoC_pop32(RL7,DEFAULT);
    VoC_pop32(REG67,DEFAULT);
    VoC_pop32(REG23,DEFAULT);

    VoC_pop32(REG01,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_return;
}*/


/*void CII_DMAI_WRITEDATA_270(void)
{

#if 0
    voc_word LOCAL_JPEG_DMA_TMP,8,x
#endif

    VoC_push32(REG01,DEFAULT);
    VoC_push32(REG23,DEFAULT);
    VoC_push32(REG67,DEFAULT);
    VoC_push32(RL7,DEFAULT);

    // dst = dts0 + 32
    VoC_lw16i_short(RL7_HI,0,DEFAULT);
    VoC_lw16i_short(RL7_LO,32,IN_PARALLEL);

    VoC_add32_rd(RL6,RL7,LOCAL_VID_BUF_OUT_PTR);

    // eaddr
    VoC_lw32_d(RL6,LOCAL_VID_BUF_OUT_PTR);

    // new dst pointer
    VoC_sw32_d(RL6,LOCAL_VID_BUF_OUT_PTR);

    // 2*h in RL7
    VoC_lw16_d(RL7_LO,GLOBAL_ImgHeight_ADDR);
    VoC_shr32_ri(RL7,-1,DEFAULT);

    // i' = w in REG7
    VoC_lw16_d(REG7,GLOBAL_ImgWidth_ADDR);

    // s + w - 1 in REG1
    VoC_lw16i_short(REG2,8,DEFAULT);
    VoC_lw16i(REG1,GLOBAL_rgb_buf_ADDR-1);
    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);

    // 17*w + 1 in REG6
    VoC_lw16i_short(REG6,17,IN_PARALLEL);
    VoC_mult16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG0,DEFAULT);

    // w in REG4
    VoC_movreg16(REG4,REG7,IN_PARALLEL);

    // laddr
    VoC_lw16i(REG3,(LOCAL_JPEG_DMA_TMP/2));


    // for (i = 0, i' = w; i < w; ++i, --i')
continue_dma_90:

        // here REG1 = &src[w - i - 1]
        VoC_lw16i_set_inc(REG0,LOCAL_JPEG_DMA_TMP,1);

        // preload data
        VoC_lw16_p(REG5,REG1,DEFAULT);
        VoC_add16_rr(REG1,REG1,REG4,IN_PARALLEL);

        // for (j = 0; j < 16; ++j)
        // dst += (2*h)
        VoC_loop_i_short(16,DEFAULT)
        VoC_startloop0

            // tmp[i] = src[j*w - i - 1]
            VoC_lw16_p(REG5,REG1,DEFAULT);
            VoC_add16_rr(REG1,REG1,REG4,IN_PARALLEL);
            VoC_sw16inc_p(REG5,REG0,DEFAULT);

        VoC_endloop0

        // here REG1 = &src[18*w - i - 1], so sub (17*w + 1)
        VoC_sub16_rr(REG1,REG1,REG6,DEFAULT);

        VoC_sw32_d(REG23,CFG_DMA_SIZE);
        VoC_sw32_d(RL6,CFG_DMA_EADDR);

        VoC_cfg(CFG_CTRL_STALL);
        VoC_NOP();

        VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

        VoC_add32_rr(RL6,RL6,RL7,DEFAULT);

        VoC_sub16_rd(REG7,REG7,CONST_JPEG_DEC_1_ADDR);

    VoC_bnez16_r(continue_dma_90,REG7);

    VoC_pop32(RL7,DEFAULT);
    VoC_pop32(REG67,DEFAULT);
    VoC_pop32(REG23,DEFAULT);

    VoC_pop32(REG01,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_return;
}*/


void CII_DMAI_WRITEISPDATA(void)
{

    VoC_lw16i(REG4,320);
    VoC_lw16i(REG5,(LOCAL_VID_BMP_BUF_ADDR)/2);

    VoC_lw32_d(RL6,GLOBAL_VID_BUF_IN_PTR);

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(RL6,CFG_DMA_EADDR);

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_shru16_ri(REG4,-2,IN_PARALLEL);
    VoC_add32_rr(RL6,RL6,REG45,DEFAULT);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();

    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_sw32_d(RL6,LOCAL_VID_BUF_OUT_PTR);

    VoC_return;
}


/*
v_yu_temp = V_YtoU/2 = 1;
h_yv_temp = H_YtoV/2 = 1;
v_yv_temp = V_YtoV/2 = 1;
h_yu_temp = H_YtoU/2 = 1;
REG0: i
REG1: rgb_buf
REG3: j

*/
//#define CONST_18_ADDR 1;
//#define CONST_367_ADDR    1;
//#define CONST_159_ADDR    1;
//#define CONST_220_ADDR    1;
//#define CONST_411_ADDR    1;
//#define CONST_29_ADDR 1;
//#define CONST_0x0000ffff_ADDR 2960;       // in ram_x


void CII_storebuffer_intern(void)
{

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_push32(RL7,IN_PARALLEL);

    VoC_lw16i(ACC1_LO,CONST_JPEG_DEC_18_ADDR);

    // IMPORTANT : here is used the fact that
    // ((val << 15)) >> 15
    // is automatically saturated by the HW

    VoC_lw16i_short(FORMAT32,-15,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw16i_short(RL6_LO,0,DEFAULT);
    VoC_lw16i_short(RL7_LO,0,IN_PARALLEL);

    VoC_lw16i(REG4,128);

    VoC_loop_i(0,8);

    // y_temp = (y+128)<<8;
    VoC_add16inc_rp(REG4,REG4,REG0,DEFAULT);
    VoC_add16inc_rp(REG5,REG4,REG0,IN_PARALLEL);

    VoC_movreg16(ACC0_HI,REG4,DEFAULT);
    VoC_lw16inc_p(REG6,REG1,IN_PARALLEL);

    VoC_movreg16(REG0,ACC1_LO,DEFAULT);
    VoC_push16(REG0,IN_PARALLEL);

    // - 7 =  (8 - 15)
    VoC_shr32_ri(ACC0,-7,DEFAULT);
    VoC_lw16inc_p(REG7,REG3,IN_PARALLEL);

    VoC_bimac32inc_rp(REG67,REG0);        // rr=(y_temp+18*u+367*v)>>8;

    VoC_movreg16(RL7_HI,REG5,DEFAULT);
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);

    VoC_bnltz32_r(CII_StoreBuffer100,ACC0)
    VoC_lw32z(ACC0,DEFAULT);
CII_StoreBuffer100:

    VoC_movreg16(REG4,ACC0_HI,DEFAULT);         //  REG6 for rr
    VoC_movreg32(ACC0,RL6,IN_PARALLEL);

    VoC_bimsu32inc_rp(REG67,REG0);      //              gg=(y_temp-159*u-220*v)>>8;

    VoC_shru16_ri(REG4,10,DEFAULT);
    VoC_shr32_ri(RL7,-7,IN_PARALLEL);

    VoC_bnltz32_r(CII_StoreBuffer106,ACC0)
    VoC_lw32z(ACC0,DEFAULT);
CII_StoreBuffer106:

    VoC_movreg16(REG5,ACC0_HI,DEFAULT);         //  REG5 for gg
    VoC_movreg32(ACC0,RL6,IN_PARALLEL);

    VoC_bimac32inc_rp(REG67,REG0);      //              bb=(y_temp+411*u-29*v)>>8;

    VoC_shru16_ri(REG5,9,DEFAULT);
    VoC_shru16_ri(REG4,-6,IN_PARALLEL);

    VoC_bnltz32_r(CII_StoreBuffer101,ACC0)
    VoC_lw32z(ACC0,DEFAULT);
CII_StoreBuffer101:

    VoC_or16_rr(REG4,REG5,DEFAULT);
    VoC_movreg16(REG5,ACC0_HI,IN_PARALLEL);         //  REG7 for bb

    VoC_movreg16(REG0,ACC1_LO,DEFAULT);
    VoC_shru16_ri(REG5,10,IN_PARALLEL);

    VoC_shru16_ri(REG4,-5,DEFAULT);
    VoC_movreg32(ACC0,RL7,IN_PARALLEL);

    VoC_bimac32inc_rp(REG67,REG0);      //              rr=(y_temp+18*u+367*v)>>8;

    VoC_or16_rr(REG4,REG5,DEFAULT);

    VoC_bnltz32_r(CII_StoreBuffer102,ACC0)
    VoC_lw32z(ACC0,DEFAULT);
CII_StoreBuffer102:

    VoC_movreg16(REG5,ACC0_HI,DEFAULT);         //  REG6 for rr
    VoC_movreg32(ACC0,RL7,IN_PARALLEL);

    VoC_bimsu32inc_rp(REG67,REG0);      // gg=(y_temp-159*u-220*v)>>8;

    VoC_shru16_ri(REG5,10,DEFAULT);
    VoC_sw16inc_p(REG4,REG2,IN_PARALLEL);       //  *lpbmp++ = DRV_RGB2PIXEL565(irgb_sat[rr],irgb_sat[gg],irgb_sat[bb]);

    VoC_bnltz32_r(CII_StoreBuffer108,ACC0)
    VoC_lw32z(ACC0,DEFAULT);
CII_StoreBuffer108:

    VoC_movreg16(REG4,ACC0_HI,DEFAULT);         //  REG5 for gg
    VoC_movreg32(ACC0,RL7,IN_PARALLEL);

    VoC_bimac32inc_rp(REG67,REG0);      //              bb=(y_temp+411*u-29*v)>>8;

    VoC_shru16_ri(REG4,9,DEFAULT);
    VoC_shru16_ri(REG5,-6,IN_PARALLEL);

    VoC_bnltz32_r(CII_StoreBuffer110,ACC0)
    VoC_lw32z(ACC0,DEFAULT);
CII_StoreBuffer110:

    VoC_or16_rr(REG4,REG5,DEFAULT);
    VoC_movreg16(REG5,ACC0_HI,IN_PARALLEL);         //  REG7 for bb

    VoC_shru16_ri(REG5,10,DEFAULT);
    VoC_shru16_ri(REG4,-5,IN_PARALLEL);

    VoC_or16_rr(REG4,REG5,DEFAULT);
    VoC_pop16(REG0,IN_PARALLEL);

    VoC_lw16i(REG4,128);

    VoC_sw16inc_p(REG4,REG2,DEFAULT);       //  *lpbmp++ = DRV_RGB2PIXEL565(irgb_sat[rr],irgb_sat[gg],irgb_sat[bb]);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_endloop0;

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);

    VoC_lw16i_short(FORMAT32,0,DEFAULT);

    VoC_return;

}

void CII_small_image(void)
{
    VoC_loop_i(0,8);

    VoC_add16inc_pd(REG4,REG0,CONST_JPEG_DEC_128_ADDR);//y_temp = (y+128)<<8; REG4 for y_temp
    VoC_movreg16(ACC0_HI,REG4,DEFAULT);//(y+128)<<8 maybe above 16bit, so put the y_temp to the high bit
    VoC_shr32_ri(ACC0,8,DEFAULT);  //of ACC0 and right shift with sign
    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_mac32_pd(REG1,CONST_JPEG_DEC_18_ADDR);      //              rr=(y_temp+18*u+367*v)>>8;
    VoC_mac32_pd(REG3,CONST_JPEG_DEC_367_ADDR);     //              rr=(y_temp+18*u+367*v)>>8;
    VoC_NOP();
    VoC_bnltz32_r(CII_small_image100,ACC0)
    VoC_lw32z(ACC0,DEFAULT);
CII_small_image100:
    VoC_bngt32_rd(CII_small_image103,ACC0,CONST_JPEG_DEC_0x0000ffff_ADDR)
    VoC_lw16i_short(ACC0_LO,0xffff,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);
CII_small_image103:
    VoC_movreg16(REG6,ACC0_LO,DEFAULT);         //  REG6 for rr
    VoC_movreg32(ACC0,RL6,DEFAULT);
    VoC_msu32_pd(REG1,CONST_JPEG_DEC_159_ADDR);     //              gg=(y_temp-159*u-220*v)>>8;
    VoC_msu32_pd(REG3,CONST_JPEG_DEC_220_ADDR);     //              gg=(y_temp-159*u-220*v)>>8;
    VoC_and16_ri(REG6,0xF800);
    VoC_bnltz32_r(CII_small_image106,ACC0)
    VoC_lw32z(ACC0,DEFAULT);
CII_small_image106:
    VoC_bngt32_rd(CII_small_image107,ACC0,CONST_JPEG_DEC_0x0000ffff_ADDR)
    VoC_lw16i_short(ACC0_LO,0xffff,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);
CII_small_image107:

    VoC_movreg16(REG5,ACC0_LO,DEFAULT);         //  REG5 for gg
    VoC_movreg32(ACC0,RL6,DEFAULT);
    VoC_shru16_ri(REG5,5,IN_PARALLEL);
    VoC_mac32inc_pd(REG1,CONST_JPEG_DEC_411_ADDR);      //              bb=(y_temp+411*u-29*v)>>8;
    VoC_mac32inc_pd(REG3,CONST_JPEG_DEC_NEG_29_ADDR);       //              bb=(y_temp+411*u-29*v)>>8;
    VoC_and16_ri(REG5,0x07E0);
    VoC_bnltz32_r(CII_small_image101,ACC0)
    VoC_lw32z(ACC0,DEFAULT);
CII_small_image101:
    VoC_bngt32_rd(CII_small_image104,ACC0,CONST_JPEG_DEC_0x0000ffff_ADDR)
    VoC_lw16i_short(ACC0_LO,0xffff,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);
CII_small_image104:
    VoC_movreg16(REG7,ACC0_LO,DEFAULT);         //  REG7 for bb
    VoC_add16_rr(REG6,REG6,REG5,IN_PARALLEL);
    VoC_shru16_ri(REG7,11,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_add16_rr(REG7,REG7,REG6,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG7,REG2,DEFAULT);       //  *lpbmp++ = DRV_RGB2PIXEL565(irgb_sat[rr],irgb_sat[gg],irgb_sat[bb]);

    VoC_endloop0;

    VoC_return;


}


void CII_StoreBuffer(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_push16(REG1,DEFAULT);//store the BitPos

    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,GLOBAL_MCUBuffer_ADDR+256,1);    // U address
    VoC_lw16i_set_inc(REG3,GLOBAL_MCUBuffer_ADDR+320,1);    // V address
    VoC_lw16i_set_inc(REG0,GLOBAL_Y_data_ADDR,1);   // Y address

    VoC_lw16_d(REG7,GLOBAL_image_control_ADDR);

    VoC_bne16_rd(StoreBuffer_begin,REG7,CONST_JPEG_DEC_2_ADDR);
    VoC_jump(small_image);

StoreBuffer_begin:

    VoC_bez16_r(block_4,REG7);

    VoC_lw16_d(REG7,GLOBAL_block_id_ADDR);//REG7:block id   (0~8)

    VoC_bne16_rd(block_1,REG7,CONST_JPEG_DEC_0_ADDR)

    VoC_jump(row_select_case0);
block_1:
    VoC_bne16_rd(block_2,REG7,CONST_JPEG_DEC_1_ADDR)

    VoC_jump(row_select_case1);
block_2:
    VoC_bne16_rd(block_3,REG7,CONST_JPEG_DEC_7_ADDR)

    VoC_jump(row_select_case1);
block_3:
    VoC_bne16_rd(block_4,REG7,CONST_JPEG_DEC_8_ADDR)

    VoC_jump(row_select_case0);
block_4:

    //seelct total 16 row
    VoC_loop_i(1,8)

    VoC_jal(CII_storebuffer_intern);

    VoC_add16_rd(REG2,REG2,GLOBAL_temp_ImgWidth_ADDR);// lpbmp address;//here temp_ImgWidth=(lbyte - 2 * 16)/2
    VoC_sub16_rd(REG1,REG1,CONST_JPEG_DEC_8_ADDR);  // U address
    VoC_sub16_rd(REG3,REG3,CONST_JPEG_DEC_8_ADDR);  // V address

    VoC_jal(CII_storebuffer_intern);

    VoC_add16_rd(REG2,REG2,GLOBAL_temp_ImgWidth_ADDR);// lpbmp address

    VoC_endloop1;

    VoC_jump(storeBuffer_end);

row_select_case0:

    //select even row
    VoC_loop_i(1,4)

    VoC_jal(CII_storebuffer_intern);

    VoC_add16_rd(REG0,REG0,CONST_JPEG_DEC_15_ADDR);
    VoC_add16_rd(REG2,REG2,GLOBAL_temp_ImgWidth_ADDR);// lpbmp address;//here temp_ImgWidth=(lbyte - 2 * 16)/2

    VoC_jal(CII_storebuffer_intern);

    VoC_add16_rd(REG0,REG0,CONST_JPEG_DEC_15_ADDR);
    VoC_add16_rd(REG2,REG2,GLOBAL_temp_ImgWidth_ADDR);// lpbmp address

    VoC_endloop1;
    VoC_jump(storeBuffer_end);


row_select_case1:

    //select even row
    VoC_loop_i(1,2)

    VoC_jal(CII_storebuffer_intern);

    VoC_add16_rd(REG0,REG0,CONST_JPEG_DEC_15_ADDR);
    VoC_add16_rd(REG2,REG2,GLOBAL_temp_ImgWidth_ADDR);// lpbmp address; here temp_ImgWidth=(lbyte - 2 * 16)/2

    VoC_jal(CII_storebuffer_intern);

    VoC_add16_rd(REG0,REG0,CONST_JPEG_DEC_15_ADDR);
    VoC_add16_rd(REG2,REG2,GLOBAL_temp_ImgWidth_ADDR);// lpbmp address

    VoC_endloop1;

    //select the last 8 row
    VoC_loop_i(1,4)

    VoC_jal(CII_storebuffer_intern);

    VoC_add16_rd(REG2,REG2,GLOBAL_temp_ImgWidth_ADDR);// lpbmp address; //here temp_ImgWidth=(lbyte - 2 * 16)/2
    VoC_sub16_rd(REG1,REG1,CONST_JPEG_DEC_8_ADDR);  // U address
    VoC_sub16_rd(REG3,REG3,CONST_JPEG_DEC_8_ADDR);  // V address

    VoC_jal(CII_storebuffer_intern);

    VoC_add16_rd(REG2,REG2,GLOBAL_temp_ImgWidth_ADDR);// lpbmp address

    VoC_endloop1;

    VoC_jump(storeBuffer_end);

small_image:


    VoC_lw16i_short(INC0,2,DEFAULT);    // Y address INC=2

    VoC_loop_i(1,8)

    VoC_jal(CII_small_image);

    VoC_add16_rd(REG2,REG2,CONST_JPEG_DEC_152_ADDR);// lpbmp address;//here temp_ImgWidth=(lbyte - 2 * 16)/2
    VoC_add16_rd(REG0,REG0,CONST_JPEG_DEC_16_ADDR);

    VoC_endloop1;


storeBuffer_end:

    VoC_pop16(REG1,DEFAULT);//pop the BitPos
    VoC_pop16(RA,DEFAULT);
    VoC_lw16d_set_inc(REG0,GLOBAL_temp_lpJpegBuf_ADDR,1);//load the old lpJpegBuf address

    VoC_return;
}



void  CII_DecodeMCUBlock(void)
{

//      lpMCUBuffer = MCUBuffer;                     //MCUBuffer size: 10*64 , one dimension
//      lastcoef_pt = MCU_lastcoef;
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_set_inc(REG2,GLOBAL_MCUBuffer_ADDR,2);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_sw16_d(REG2,GLOBAL_lpMCUBuffer_ADDR_ADDR);

    VoC_loop_i(0,192)
    VoC_sw32inc_p(ACC0, REG2,DEFAULT);//memset(lpMCUBuffer,0,(Y_in_MCU+2)*64*sizeof(short));
    VoC_endloop0                          //memset(lastcoef_pt,0,(Y_in_MCU+2)*sizeof(short));


//      Y = Y_data;
    /*      for (i=0;i<SampRate_Y_V;i++)  //Y
            {
                for(j=0;j<SampRate_Y_H;j++)
                { */
    VoC_lw16i(REG3,GLOBAL_Y_data_ADDR+136);
    VoC_lw16i(REG2,GLOBAL_Y_data_ADDR+128);
    VoC_lw16i(REG5,GLOBAL_Y_data_ADDR+8);
    VoC_lw16i(REG4,GLOBAL_Y_data_ADDR);

    VoC_push16(REG3,DEFAULT);
    VoC_push16(REG2,DEFAULT);
    VoC_push16(REG5,DEFAULT);
    VoC_lw32z(REG23,IN_PARALLEL);

Y_IDCT_START:
    VoC_lw16i(REG5,GLOBAL_ycoef_ADDR);
    VoC_push16(REG3,DEFAULT);//store the count
    VoC_push16(REG4,DEFAULT);//Y_data

    VoC_lw16i_short(REG3,0,DEFAULT);//GLOBAL_YDcIndex_ADDR
    VoC_lw16i_short(REG2,2,IN_PARALLEL);//GLOBAL_YAcIndex_ADDR


    VoC_lw16i(REG4,TABLE_qt_table_ADDR);//qttable

//              funcret = HufBlock(&ycoef,YDcIndex,YAcIndex,YQtTable);
    VoC_jal(CII_HufBlock);

//              (*idct_array[(*temp_MCU_lastcoef)&0x3f])(temp_lpMCUBuffer);


    VoC_and16_ri(REG3,0x3f);//*temp_MCU_lastcoef)&0x3f
    VoC_push32(RL7,DEFAULT);//store the CurByte
    VoC_push32(REG01,DEFAULT);//store current address of lpJpegBuf & BitPos


    VoC_bnez16_r(Y_IDCT_44_C,REG3);
    VoC_jal(CII_IDCT_11_C);
    VoC_jump(Y_IDCT_end);
Y_IDCT_44_C:
    VoC_bnlt16_rd(Y_IDCT_88_C,REG3,CONST_JPEG_DEC_10_ADDR);
    VoC_jal(CII_IDCT_44_C);
    VoC_jump(Y_IDCT_end);
Y_IDCT_88_C:
    VoC_jal(CII_IDCT_88_C);
Y_IDCT_end:


    VoC_pop32(REG01,DEFAULT);//pop current address of lpJpegBuf & the BitPos
//              if((SampRate_Y_H==1)&&(SampRate_Y_V==1))
//              {
//                  Y = temp_lpMCUBuffer;
//              }else
//              {

    VoC_lw16d_set_inc(REG2,GLOBAL_lpMCUBuffer_ADDR_ADDR,2);

    VoC_lw16i_short(INC0,1,DEFAULT); //change the INC0  as before
    VoC_pop16(REG3,IN_PARALLEL);//current address of Y_data
    VoC_lw16i_short(INC3,2,DEFAULT);

    VoC_lw32inc_p(ACC0,REG2,IN_PARALLEL);

    VoC_loop_i(1,8)
    VoC_loop_i(0,4);
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);
    exit_on_warnings =OFF ;
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    exit_on_warnings =ON ;
    VoC_endloop0
    VoC_add16_rd(REG3,REG3,CONST_JPEG_DEC_8_ADDR);
    VoC_endloop1

    VoC_pop16(REG3,DEFAULT);//current count
    VoC_pop32(RL7,IN_PARALLEL);//pop the old CurByte


    VoC_lw16_d(REG2,GLOBAL_lpMCUBuffer_ADDR_ADDR);

    VoC_add16_rd(REG2,REG2,CONST_JPEG_DEC_64_ADDR);//temp_lpMCUBuffer+64;

    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_sw16_d(REG2,GLOBAL_lpMCUBuffer_ADDR_ADDR);

    VoC_bnlt16_rd(U_IDCT_START,REG3,CONST_JPEG_DEC_3_ADDR);//count the loop number,if count>=3,jump end
    VoC_inc_p(REG3,DEFAULT);
    VoC_pop16(REG4,IN_PARALLEL);//pop the new address of Y_data

    VoC_jump(Y_IDCT_START);

U_IDCT_START:
//      U = lpMCUBuffer;
    /*      for (i=0;i<SampRate_U_V;i++)  //U
            {
                for(j=0;j<SampRate_U_H;j++)
                {*/
//              temp_lpMCUBuffer = lpMCUBuffer;//add by longyl
//              temp_MCU_lastcoef = lastcoef_pt;
    VoC_lw16i_short(REG3,1,DEFAULT);//GLOBAL_UVDcIndex_ADDR
    VoC_lw16i_short(REG2,3,IN_PARALLEL);//GLOBAL_UVAcIndex_ADDR

    VoC_lw16i(REG5,GLOBAL_ucoef_ADDR);
    VoC_lw16i(REG4,TABLE_qt_table_ADDR+64);//qttable


//              funcret=HufBlock(&ucoef,UVDcIndex,UVAcIndex,UQtTable);
    VoC_jal(CII_HufBlock);

//              (*idct_array[(*temp_MCU_lastcoef)&0x3f])(temp_lpMCUBuffer);


    VoC_and16_ri(REG3,0x3f);//*temp_MCU_lastcoef)&0x3f
    VoC_push32(RL7,DEFAULT);//store the CurByte
    VoC_push32(REG01,DEFAULT);//store current address of lpJpegBuf & the BitPos

    VoC_bnez16_r(U_IDCT_44_C,REG3);
    VoC_jal(CII_IDCT_11_C);
    VoC_jump(U_IDCT_END);
U_IDCT_44_C:
    VoC_bnlt16_rd(U_IDCT_88_C,REG3,CONST_JPEG_DEC_10_ADDR);
    VoC_jal(CII_IDCT_44_C);
    VoC_jump(U_IDCT_END);
U_IDCT_88_C:
    VoC_jal(CII_IDCT_88_C);
U_IDCT_END:

    VoC_pop32(REG01,DEFAULT);//pop current address of lpJpegBuf & BitPos

    VoC_lw16_d(REG2,GLOBAL_lpMCUBuffer_ADDR_ADDR);

    VoC_add16_rd(REG2,REG2,CONST_JPEG_DEC_64_ADDR);//temp_lpMCUBuffer+64;

    VoC_pop32(RL7,DEFAULT);//pop the old CurByte
    VoC_lw16i_short(INC0,1,IN_PARALLEL);//change the INC0  as before
    VoC_sw16_d(REG2,GLOBAL_lpMCUBuffer_ADDR_ADDR);


//      V = lpMCUBuffer;
    /*      for (i=0;i<SampRate_V_V;i++)  //V
            {
                for(j=0;j<SampRate_V_H;j++)
                {*/
//              temp_lpMCUBuffer = lpMCUBuffer;//add by longyl
//              temp_MCU_lastcoef = lastcoef_pt;
    VoC_lw16i_short(REG3,1,DEFAULT);//GLOBAL_UVDcIndex_ADDR
    VoC_lw16i_short(REG2,3,IN_PARALLEL);//GLOBAL_UVAcIndex_ADDR

    VoC_lw16i(REG5,GLOBAL_vcoef_ADDR);
    VoC_lw16i(REG4,TABLE_qt_table_ADDR+64);//qttable


    VoC_jal(CII_HufBlock);
//              funcret=HufBlock(&vcoef,UVDcIndex,UVAcIndex,VQtTable);
//              (*idct_array[(*temp_MCU_lastcoef)&0x3f])(temp_lpMCUBuffer);


    VoC_sw16_d(REG0,GLOBAL_temp_lpJpegBuf_ADDR);//store the current address of lpJpegBuf
    VoC_and16_ri(REG3,0x3f);//*temp_MCU_lastcoef)&0x3f

    VoC_push32(RL7,DEFAULT);//store the CurByte
    VoC_push16(REG1,IN_PARALLEL);//store the BitPos
    VoC_bnez16_r(V_IDCT_44_C,REG3);
    VoC_jal(CII_IDCT_11_C);
    VoC_jump(V_IDCT_END);
V_IDCT_44_C:
    VoC_bnlt16_rd(V_IDCT_88_C,REG3,CONST_JPEG_DEC_10_ADDR);
    VoC_jal(CII_IDCT_44_C);
    VoC_jump(V_IDCT_END);
V_IDCT_88_C:
    VoC_jal(CII_IDCT_88_C);
V_IDCT_END:
    VoC_pop16(REG1,DEFAULT);//pop the BitPos
    VoC_pop32(RL7,IN_PARALLEL);//pop the old CurByte
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(INC0,1,DEFAULT); //change the INC0  as before
    VoC_return;
}

/*
input:
       REG2: achufindex
       REG3: dchufindex
       REG4: qttable
       REG5: coef
output:REG3: count

unchange:REG0: lpJpegBuf
         REG1: BitPos
         RL7:  CurByte
*/
//////////////////////////////////////////////////////////////////
//int HufBlock(short* coef,BYTE dchufindex,BYTE achufindex,short * qttable)
void CII_HufBlock(void)
{


    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_push32(REG45,DEFAULT);//qttable & coef
    VoC_push16(REG2,IN_PARALLEL);//push achufindex

    VoC_lw32_d(ACC0,CONST_JPEG_DEC_0xFFFFFFFF_ADDR);

    VoC_bnez16_r(DecodeChromDc_CASE,REG3);

    VoC_bnlt16_rd(DecodeLumDc_100,REG1,CONST_JPEG_DEC_8_ADDR);//BitPos<n

    VoC_jal(CII_ReadByte);//output:RL6;// RL7 , REG45   CurByte

DecodeLumDc_100:

    /*
                if(code<15)
        {
            size = DClumtab0[code].val;
            flushbits(DClumtab0[code].len);
            if(size == 0)
            {
                vvalue = 0;
                return FUNC_OK;
            }
        }else
    */

    VoC_movreg32(REG45,RL7,DEFAULT);//CurByte

    VoC_sub16_rd(REG7,REG1,CONST_JPEG_DEC_8_ADDR); //(BitPos-8)

    VoC_shr32_rr(REG45,REG7,DEFAULT);//CurByte>>(BitPos-8)
    VoC_lw32z(RL6,IN_PARALLEL);

    //here REG4 is code
    VoC_and16_ri(REG4,0xff); //&0xff;
    VoC_movreg16(REG5,REG4,DEFAULT);

    VoC_shru16_ri(REG5,4,DEFAULT);
    VoC_lw16i(REG2,TABLE_DClumtab0_ADDR);
    VoC_lw16i(REG6,0xf0);

    VoC_bgt16_rr(DecodeLumDc_101,REG6,REG4);//if(code>=0xf0)

    VoC_sub16_rr(REG5,REG4,REG6,DEFAULT);//code -= 0xf0;
    VoC_lw16i(REG2,TABLE_DClumtab1_ADDR);

DecodeLumDc_101:

    //now REG5 is the code
    VoC_add16_rr(REG2,REG2,REG5,DEFAULT);
    VoC_NOP();

    VoC_lbinc_p(REG2);
    VoC_rbinc_i(REG6,8,DEFAULT);//size = DClumtab1[code].val;
    VoC_rbinc_i(REG4,8,DEFAULT);//DClumtab1[code].len

    VoC_sub16_rr(REG1,REG1,REG4,DEFAULT);// BitPos -= n

    VoC_bnez16_r(DecodeDc_100,REG6);
    VoC_lw16i_short(REG4,0,DEFAULT);//vvalue = 0;

    VoC_jump(HufBlock_100);

DecodeChromDc_CASE:
DecodeChromDc_CIRCLE:

    VoC_bnlt16_rd(DecodeChromDc_100,REG1,CONST_JPEG_DEC_10_ADDR);//BitPos<n

    VoC_jal(CII_ReadByte);//output:RL6;
    VoC_jump(DecodeChromDc_CIRCLE)

DecodeChromDc_100:

    VoC_movreg32(REG45,RL7,DEFAULT);//CurByte
    VoC_sub16_rd(REG7,REG1,CONST_JPEG_DEC_10_ADDR); //(BitPos-10)

    VoC_shr32_rr(REG45,REG7,DEFAULT);//CurByte>>(BitPos-10)
    VoC_lw32z(RL6,IN_PARALLEL);

    //here REG4 is code
    VoC_and16_ri(REG4,0x3ff); //&0x3ff;
    VoC_movreg16(REG5,REG4,DEFAULT);

    VoC_shru16_ri(REG5,5,DEFAULT);//    code >>= 5;
    VoC_lw16i(REG2,TABLE_DCchromtab0_ADDR);
    VoC_lw16i(REG6,0x3e0);

    VoC_bgt16_rr(DecodeChromDc_101,REG6,REG4);//if(code>0x3df)

    VoC_sub16_rr(REG5,REG4,REG6,DEFAULT);//code -= 0x3e0;
    VoC_lw16i(REG2,TABLE_DCchromtab1_ADDR);

DecodeChromDc_101:

    //now REG5 is the code

    VoC_add16_rr(REG2,REG2,REG5,DEFAULT);
    VoC_NOP();
    VoC_lbinc_p(REG2);
    VoC_rbinc_i(REG6,8,DEFAULT);//size = DClumtab1[code].val;
    VoC_rbinc_i(REG4,8,DEFAULT);//DClumtab1[code].len

    VoC_sub16_rr(REG1,REG1,REG4,DEFAULT);// BitPos -= n

    VoC_bnez16_r(DecodeDc_100,REG6);
    VoC_lw16i_short(REG4,0,DEFAULT);//vvalue = 0;
    VoC_jump(HufBlock_100);

DecodeDc_100:

    VoC_bngt16_rr(DecodeDc_101,REG6,REG1);//BitPos<n

DecodeDc_CIRCLE0:

    VoC_jal(CII_ReadByte);//output:RL6;

    VoC_bgt16_rr(DecodeDc_CIRCLE0,REG7,REG1);//BitPos<n

DecodeDc_101:

    VoC_sub16_dr(REG4,CONST_JPEG_DEC_32_ADDR,REG6);

    VoC_movreg32(ACC1,RL7,DEFAULT); //CurByte
    VoC_sub16_rr(REG1,REG1,REG6,IN_PARALLEL);// BitPos -= n

    VoC_shr32_rr(ACC1,REG1,DEFAULT);
    VoC_shru32_rr(ACC0,REG4,IN_PARALLEL);

    VoC_and32_rr(ACC1,ACC0,DEFAULT);//CurByte>>(BitPos-n)&And[n];

    VoC_sub16_rd(REG6,REG6,CONST_JPEG_DEC_1_ADDR);

    VoC_movreg32(RL6,ACC1,DEFAULT);//vvalue = CurByte>>(BitPos-size)&And[size];
    VoC_shru32_rr(ACC1,REG6,IN_PARALLEL);

    VoC_add32_rd(ACC1,ACC1,CONST_JPEG_DEC_0xFFFFFFFF_ADDR);

    VoC_shru32_rr(ACC1,REG4,DEFAULT);
    VoC_sub32_rr(REG67,RL6,ACC1,DEFAULT);//vvalue -= (unsigned)((vvalue>>(size-1))-1) >> ((sizeof(vvalue)<<3)-size);

HufBlock_100:


    /*  vvalue += *coef;
        *coef = vvalue;
        *lpMCUBuffer = vvalue*qttable[0];
    */

    VoC_pop32(REG23,DEFAULT);//qttable & coef
    VoC_pop16(REG5,IN_PARALLEL);//pop achufindex
    VoC_lw16_d(REG7,GLOBAL_lpMCUBuffer_ADDR_ADDR);
    VoC_add16_rp(REG6,REG6,REG3,DEFAULT);
    VoC_movreg32(RL6,REG23,DEFAULT);
    VoC_mult16_rp(REG4,REG6,REG2,IN_PARALLEL);

    VoC_sw16_p(REG6,REG3,DEFAULT);

    VoC_lw16i_short(REG3,1,DEFAULT);//count=1
    VoC_movreg16(REG2,REG5,IN_PARALLEL);//pop achufindex

    VoC_sw16_p(REG4,REG7,DEFAULT);//*lpMCUBuffer = vvalue*qttable[0];
    VoC_lw16i_short(INC3,1,IN_PARALLEL);

    //ac
//  HufTabIndex=achufindex;

//  for(count=1;;count++)
//  {
    // funcret=(*vld_array[HufTabIndex])();


HufBlock_101:

    VoC_push32(REG23,DEFAULT);
    VoC_lw16i_short(REG7,2,IN_PARALLEL);

    VoC_push32(RL6,DEFAULT);
    VoC_lw16i_short(REG6,16,IN_PARALLEL);

    VoC_bne16_rr(DecodeChromAc_CASE,REG2,REG7)

DecodeLumAc_CIRCLE:

    VoC_bngt16_rr(DecodeLumAc_100,REG6,REG1);//BitPos<n

    VoC_jal(CII_ReadByte);//output:RL6;
    VoC_jump(DecodeLumAc_CIRCLE)

    // fake NOP (never reached)
    // added for code alignment in this critical loop
    VoC_NOP();

DecodeLumAc_100:

    VoC_sub16_rr(REG7,REG1,REG6,DEFAULT);//BitPos-16
    VoC_movreg32(REG45,RL7,IN_PARALLEL);

    VoC_shr32_rr(REG45,REG7,DEFAULT);
    VoC_lw16i_short(REG5,0,DEFAULT);

    // if ((unigned)code < 0xff80)
    VoC_blt32_rd(DecodeLumAc_101,REG45,CONST_JPEG_DEC_0x0000ff80_ADDR);

    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);

    VoC_add32_rd(REG23,REG45,TABLE_AClumcodetab3_ADDR); // tab3 - 0xff80*2 + 2*code
    VoC_jump(DecodeLumAc_104);

    // fake NOP (never reached)
    // added for code alignment in this critical loop
    VoC_NOP();

DecodeLumAc_101:

    // if ((unigned)code < 0xfd80)
    VoC_blt32_rd(DecodeLumAc_102,REG45,CONST_JPEG_DEC_0x0000fd80_ADDR);

    VoC_shr32_ri(REG45,4,DEFAULT);

    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);

    VoC_add32_rd(REG23,REG45,TABLE_AClumcodetab2_ADDR); // tab2 - 0xfd8*2 + 2*code

    VoC_jump(DecodeLumAc_104);

DecodeLumAc_102:

    // if ((unigned)code < 0xe800)
    VoC_blt32_rd(DecodeLumAc_103,REG45,CONST_JPEG_DEC_0x0000e800_ADDR);

    VoC_shr32_ri(REG45,7,DEFAULT);

    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);

    VoC_add32_rd(REG23,REG45,TABLE_AClumcodetab1_ADDR); // tab1 - 0x1d0*2 + 2*code
    VoC_jump(DecodeLumAc_104);

DecodeLumAc_103:

    VoC_shr32_ri(REG45,11,DEFAULT);
    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_add32_rd(REG23,REG45,TABLE_AClumcodetab0_ADDR); // tab0 + 2*code
    VoC_lw32z(RL6,DEFAULT);

DecodeLumAc_104:

    VoC_lbinc_p(REG2);

    VoC_rbinc_i(REG7,8,DEFAULT);//rrun = AClumtab2[code].run;

    VoC_rbinc_i(REG6,8,DEFAULT);//size = AClumtab2[code].level;
    VoC_sub16_rp(REG1,REG1,REG2,IN_PARALLEL);// BitPos -= n //n=AClumtab2[code].len

    VoC_bnez16_r(DecodeAc_100,REG6);//size == 0

    VoC_lw16i_short(ACC0_LO,0,DEFAULT);//vvalue = 0;
    VoC_pop32(REG45,IN_PARALLEL);//qttable & coef

    VoC_pop32(REG23,DEFAULT);//count & achufindex
    VoC_jump(HufBlock_102);

DecodeChromAc_CASE:
DecodeChromAc_CIRCLE:

    VoC_bnlt16_rd(DecodeChromAc_100,REG1,CONST_JPEG_DEC_16_ADDR);//BitPos<n

    VoC_jal(CII_ReadByte);//output:x;
    VoC_jump(DecodeChromAc_CIRCLE);

DecodeChromAc_100:

    VoC_sub16_rr(REG7,REG1,REG6,DEFAULT); //BitPos-16
    VoC_movreg32(REG45,RL7,IN_PARALLEL);

    VoC_shr32_rr(REG45,REG7,DEFAULT);
    VoC_lw16i_short(REG5,0,DEFAULT);//code = CurByte>>(BitPos-16)&0xffff;

    // if ((unigned)code < 0xff80)
    VoC_blt32_rd(DecodeChromAc_101,REG45,CONST_JPEG_DEC_0x0000ff80_ADDR);

    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);

    VoC_add32_rd(REG23,REG45,TABLE_ACchromcodetab3_ADDR); // tab3 - 0xff80*2 + 2*code
    VoC_jump(DecodeChromAc_104);

    // fake NOP (never reached)
    // added for code alignment in this critical loop
    VoC_NOP();

DecodeChromAc_101:

    // if ((unigned)code < 0xfd80)
    VoC_blt32_rd(DecodeChromAc_102,REG45,CONST_JPEG_DEC_0x0000fd80_ADDR);

    VoC_shr32_ri(REG45,4,DEFAULT);

    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);

    VoC_add32_rd(REG23,REG45,TABLE_ACchromcodetab2_ADDR); // tab2 - 0xfd8*2 + 2*code

    VoC_jump(DecodeChromAc_104);

DecodeChromAc_102:

    // if ((unigned)code < 0xe000)
    VoC_blt32_rd(DecodeChromAc_103,REG45,CONST_JPEG_DEC_0x0000e000_ADDR);

    VoC_shr32_ri(REG45,7,DEFAULT);

    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);

    VoC_add32_rd(REG23,REG45,TABLE_ACchromcodetab1_ADDR); // tab1 - 0x1d0*2 + 2*code
    VoC_jump(DecodeChromAc_104);

DecodeChromAc_103:

    VoC_shr32_ri(REG45,11,DEFAULT);
    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_add32_rd(REG23,REG45,TABLE_ACchromcodetab0_ADDR); // tab0 + 2*code
    VoC_lw32z(RL6,DEFAULT);

DecodeChromAc_104:

    VoC_lbinc_p(REG2);

    VoC_rbinc_i(REG7,8,DEFAULT);//rrun = ACchromtab3[code].run;

    VoC_rbinc_i(REG6,8,DEFAULT);//size = ACchromtab3[code].level;
    VoC_sub16_rp(REG1,REG1,REG2,IN_PARALLEL);// BitPos -= n  //    n=ACchromtab3[code].len

    VoC_bnez16_r(DecodeAc_100,REG6);//size == 0

    VoC_lw16i_short(ACC0_LO,0,DEFAULT);//vvalue = 0;
    VoC_pop32(REG45,IN_PARALLEL);//qttable & coef

    VoC_pop32(REG23,DEFAULT);//count & achufindex

    VoC_jump(HufBlock_102);

DecodeAc_100:

    VoC_bngt16_rr(DecodeAc_101,REG6,REG1);//BitPos<n

    VoC_jal(CII_ReadByte);//output:x;
    VoC_jump(DecodeAc_100);

DecodeAc_101:

    // TRICK : use the bitread instructions
    // to fetch CurByte>>(BitPos-n)&And[n]

    // move CurByte to the BITLOAD register (RL6)
    // and sert the bitcache to an offset of BitPos
    VoC_movreg16(BITCACHE,REG1,DEFAULT);
    VoC_movreg32(RL6,RL7,IN_PARALLEL);

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_short(REG2,32,IN_PARALLEL);

    // read n bits from BitPos <=> CurByte>>(BitPos-n)&And[n];
    VoC_rb_r(REG4,REG6,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG6,IN_PARALLEL);// BitPos -= n

    // isn't this the correct implementation?
    //VoC_sub16_dr(REG6,CONST_JPEG_DEC_1_ADDR,REG6);
    //VoC_lw16i_short(ACC1_LO,-1,DEFAULT);
    //VoC_lw16i_short(ACC1_HI,-1,IN_PARALLEL);
    //VoC_shr32_rr(ACC1,REG6,DEFAULT);
    //VoC_add32_rr(REG45,REG45,ACC1,DEFAULT);
    //VoC_shru32_ri(REG45,31,DEFAULT);//vvalue -= (unsigned)((vvalue>>(size-1))-1) >> (32-size);

    VoC_movreg32(ACC0,REG45,DEFAULT);//vvalue = showbits(size);
    VoC_shru32_ri(REG45,-1,IN_PARALLEL);

    VoC_shru32_rr(REG45,REG6,DEFAULT);
    VoC_sub16_rr(REG2,REG2,REG6,IN_PARALLEL);

    VoC_add32_rd(ACC1,REG45,CONST_JPEG_DEC_0xFFFFFFFF_ADDR);

    VoC_shru32_rr(ACC1,REG2,DEFAULT);//vvalue -= (unsigned)((vvalue>>(size-1))-1) >> (32-size);
    VoC_pop32(REG45,IN_PARALLEL);//qttable & coef

    VoC_sub32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_pop32(REG23,IN_PARALLEL);//count & achufindex

HufBlock_102:

    // huffman decode the final step,for IQ preparation,Z-Z inverse scan to get table qualitized while encoding before
    // if ((rrun==0)&&(vvalue==0))
    // {
    //lpMCUBuffer = temp_lpMCUBuffer+64;
    //*lastcoef_pt++ = count;
    //break;
    // }

    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);//count += rrun;
    VoC_movreg16(REG6,ACC0_LO,IN_PARALLEL);

    VoC_bez32_r(HufBlock_103,REG67);//REG6:vvalue& REG7:rrun

    //  else
    //{
    //    count += rrun;
    //    lpMCUBuffer[inverse_Zig_Zag[count]] = vvalue*qttable[count];
    //    fwrite(&vvalue,1,4,DCfile); //test by longyl
    //}
    //

    VoC_add16_rr(REG7,REG4,REG3,DEFAULT);
    VoC_movreg32(RL6,REG45,IN_PARALLEL);

    VoC_add16_rd(REG7,REG3,TABLE_inverse_Zig_Zag_ADDR_ADDR);

    VoC_mult16_rp(REG6,REG6,REG7,DEFAULT); //vvalue*qttable[count]
    VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);

    VoC_add16_pd(REG7,REG7,GLOBAL_lpMCUBuffer_ADDR_ADDR);//lpMCUBuffer[inverse_Zig_Zag[count]]

    VoC_bnlt16_rd(HufBlock_113,REG3,CONST_JPEG_DEC_63_ADDR);

    VoC_sw16_p(REG6,REG7,DEFAULT);//lpMCUBuffer[inverse_Zig_Zag[count]] = vvalue*qttable[count];
    VoC_inc_p(REG3,IN_PARALLEL);

    VoC_jump(HufBlock_101);

HufBlock_113:

    VoC_sw16_p(REG6,REG7,DEFAULT);//lpMCUBuffer[inverse_Zig_Zag[count]] = vvalue*qttable[count];

HufBlock_103:

    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,DEFAULT);

    VoC_return;
}

//////////////////////////////////////////////////////////////////////////////

/*
REG0: lp=jpegbuf=lpJpegBuf+headerlen;
REG6:  BitPos;
output: RL6    // RL7 , REG45   CurByte
*/
void CII_ReadByte(void)
{

    VoC_lw32z(RL6,DEFAULT);

    VoC_lw16i(REG2,LOCAL_JPEG_IN_BUF_ADDR);

    VoC_sub16_rr(REG4,REG0,REG2,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);

    VoC_blt16_rd(ReadByte_begin,REG4,CONST_JPEG_DEC_448_ADDR);

    VoC_push16(RA,DEFAULT);
    VoC_jal(CII_DMAI_READDATA);
    VoC_lw16i(REG0,LOCAL_JPEG_IN_BUF_ADDR);
    VoC_pop16(RA,DEFAULT);

ReadByte_begin:

    VoC_lw16_d(REG4,GLOBAL_temp_Y_data_ADDR);
    VoC_lbinc_p(REG0);//i=*(lp++);

    VoC_lw16i_short(REG2,1,DEFAULT);   //WheHei = 1;
    VoC_lw16i_short(REG3,-8,IN_PARALLEL);//BitReturn = 8;

    VoC_bne16_rd(ReadByte_100,REG4,CONST_JPEG_DEC_1_ADDR);//if (WheHei==1)

    VoC_rbinc_i(REG4,16,DEFAULT);

    VoC_be16_rd(ReadByte_1022,REG4,CONST_JPEG_DEC_0xFF_ADDR); //if(i==0xff)
    VoC_lw16i_short(REG2,0,DEFAULT);   //WheHei = 1;

    VoC_jump(ReadByte_1022)

ReadByte_100:

    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_bne16_rd(ReadByte_101,REG4,CONST_JPEG_DEC_0xFF_ADDR); //if(i==0xff)

    VoC_movreg16(RL6_LO,REG4,DEFAULT);

    VoC_jump(ReadByte_102)

ReadByte_101:

    VoC_lw16i_short(REG3,-16,DEFAULT);//BitReturn = 16;
    VoC_bne16_rd(ReadByte_102,REG5,CONST_JPEG_DEC_0xFF_ADDR);//if(j==0xff)
ReadByte_1022:
    VoC_sw16_d(REG2,GLOBAL_temp_Y_data_ADDR);//WheHei = 1;

ReadByte_102:
    VoC_sub16_rr(REG1,REG1,REG3,DEFAULT);//  BitPos += 16;

    VoC_shru32_rr(RL7,REG3,DEFAULT);
    VoC_or32_rr(RL7,RL6,DEFAULT);     //CurByte = (CurByte<<16)|ReadByte()

    VoC_return

}

void CII_IDCT_11_C(void)
{

    VoC_lw16_d(REG1,GLOBAL_lpMCUBuffer_ADDR_ADDR);//lpMCUBuffer = MCUBuffer
    VoC_lw16i_short(REG2,4,DEFAULT);

    VoC_add16_rp(REG2,REG2,REG1,DEFAULT); //blk[0]+4

    VoC_shr16_ri(REG2,-4,DEFAULT);
    VoC_shr16_ri(REG2,7,DEFAULT); //iclp[(block[0]+4)>>3]

    VoC_movreg16(REG3,REG2,DEFAULT);

    VoC_loop_i_short(32,DEFAULT)
    VoC_startloop0
    VoC_sw32inc_p(REG23,REG1,DEFAULT);
    VoC_endloop0
    VoC_return
}

///////////////////////////////////////////////////////////////////////
void CII_IDCT_44_C(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(INC3,-1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_lw16d_set_inc(REG1,GLOBAL_lpMCUBuffer_ADDR_ADDR,2);//lpMCUBuffer = MCUBuffer

    VoC_loop_i(1,4);
    VoC_jal(CII_idctrow4);
    VoC_endloop1


    VoC_lw16d_set_inc(REG1,GLOBAL_lpMCUBuffer_ADDR_ADDR,8);//lpMCUBuffer = MCUBuffer

    VoC_loop_i(1,8);
    VoC_jal(CII_idctcol4);
    VoC_endloop1


    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_return
}

///////////////////////////////////////////////////////////////////////
void CII_IDCT_88_C(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);

    VoC_lw16d_set_inc(REG1,GLOBAL_lpMCUBuffer_ADDR_ADDR,2);//lpMCUBuffer = MCUBuffer

    VoC_loop_i(1,8);

    VoC_jal(CII_idctrow);
    VoC_endloop1


    VoC_lw16d_set_inc(REG1,GLOBAL_lpMCUBuffer_ADDR_ADDR,8);//lpMCUBuffer = MCUBuffer
    VoC_loop_i(1,8);

    VoC_jal(CII_idctcol);

    VoC_endloop1
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_return
}

/*
REG1: blk=&temp_lpMCUBuffer+i*8
RL6:    x1
ACC1_LO:x2
REG4:   x3
REG3:   x4
ACC1_HI:x5
REG7:   x6
REG5:   x7

optimised by Cui 20070912
optimized by Yordan 20080826
*/

void CII_idctrow(void)
{

    VoC_lw16i_short(INC1,2,DEFAULT);

    VoC_push16(REG1,DEFAULT);//&blk

    VoC_lw32inc_p(REG23,REG1,DEFAULT);//blk[0]&x4 = blk[1]

    VoC_lw32inc_p(REG45,REG1,DEFAULT);//x3 = blk[2]&x7 = blk[3]
    VoC_lw32z(RL6,IN_PARALLEL);

    VoC_lw32inc_p(REG67,REG1,DEFAULT);//REG7=x6 = blk[5]
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw32inc_p(ACC1,REG1,DEFAULT);//x2 = blk[6]&x5 = blk[7]
    VoC_movreg16(RL6_HI,REG6,IN_PARALLEL);

    VoC_shr32_ri(RL6,5,DEFAULT);//x1 = blk[4]<<11
    VoC_movreg16(ACC0_HI,REG2,IN_PARALLEL);
    /*
        if (!((x1 = blk[4]<<11) | (x2 = blk[6]) | (x3 = blk[2]) |
            (x4 = blk[1]) | (x5 = blk[7]) | (x6 = blk[5]) | (x7 = blk[3])))
        {
            blk[0]=blk[1]=blk[2]=blk[3]=blk[4]=blk[5]=blk[6]=blk[7]=blk[0]<<3;
            return;
        }
    */
    VoC_bnez16_r(idctrow_100,REG3);
    VoC_bnez32_r(idctrow_100,REG45);
    VoC_bnez32_r(idctrow_100,RL6);
    VoC_bnez16_r(idctrow_100,REG7);
    VoC_bnez32_r(idctrow_100,ACC1);

    VoC_shr16_ri(REG2,-3,DEFAULT);

    VoC_pop16(REG1,DEFAULT);//blk
    VoC_movreg16(REG3,REG2,IN_PARALLEL);

    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0
    VoC_sw32inc_p(REG23,REG1,DEFAULT);
    VoC_endloop0

    VoC_return

idctrow_100:

    VoC_lw16i_set_inc(REG2,CONST_JPEG_DEC_1108_ADDR,1);

    VoC_movreg32(REG01,ACC1,DEFAULT);                 //x5 in REG1, x2 in REG0
    VoC_shr32_ri(ACC0,5,IN_PARALLEL);

    VoC_add32_rd(ACC0,ACC0,CONST_JPEG_DEC_0x00000080_ADDR);//x0 = (blk[0]<<11) + 128;

    VoC_add16_rr(REG6,REG4,REG0,DEFAULT);             //x3+x2
    VoC_sub32_rr(RL7,ACC0,RL6,IN_PARALLEL);           //x0 = x0 - x1;

    VoC_multf32_rp(ACC0,REG6,REG2,DEFAULT);           //x1 = W6*(x3+x2);
    VoC_multf32inc_rp(ACC1,REG6,REG2,IN_PARALLEL);    //x1 = W6*(x3+x2);

    VoC_add32_rr(RL6,ACC0,RL6,DEFAULT);               //x8 = x0 + x1;
    VoC_msu32inc_rp(REG0,REG2,IN_PARALLEL);           //x2 = x1 - (W2+W6)*x2;

    VoC_mac32inc_rp(REG4,REG2,DEFAULT);               //x3 = x1 + (W2-W6)*x3;
    VoC_add16_rr(REG6,REG3,REG1,IN_PARALLEL);         //x4 + x5

    VoC_lw16i_set_inc(REG0,GLOBAL_IDCT_BUFFER_X_ADDR,2);

    VoC_add32_rr(ACC0,RL6,ACC0,DEFAULT);              //x7 = x8 + x3;
    VoC_sub32_rr(RL6,RL6,ACC0,IN_PARALLEL);           //x8 = x8 - x3;

    VoC_add32_rr(RL7,RL7,ACC1,DEFAULT);               //x3 = x0 + x2;
    VoC_sub32_rr(ACC1,RL7,ACC1,IN_PARALLEL);          //x0 = x0 - x2;
    //x6 in REG7
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);                 //x7
    VoC_multf32_rp(ACC0,REG6,REG2,IN_PARALLEL);       //x8 = W7*(x4+x5);

    VoC_sw32inc_p(RL7,REG0,DEFAULT);                  //x3
    VoC_multf32inc_rp(ACC1,REG6,REG2,IN_PARALLEL);    //x8 = W7*(x4+x5);

    VoC_mac32inc_rp(REG3,REG2,DEFAULT);               //x4 = x8 + (W1-W7)*x4;
    VoC_sw32inc_p(ACC1,REG0,IN_PARALLEL);             //x0

    VoC_sw32inc_p(RL6,REG0,DEFAULT);                  //x8 (previous)
    VoC_msu32inc_rp(REG1,REG2,IN_PARALLEL);           //x5 = x8 - (W1+W7)*x5;

    VoC_movreg32(RL7,ACC0,DEFAULT);
    VoC_add16_rr(REG3,REG7,REG5,DEFAULT);             //x6 + x7

    VoC_multf32_rp(ACC0,REG3,REG2,DEFAULT);           //x8 = W3*(x6+x7);
    VoC_multf32inc_rp(ACC1,REG3,REG2,IN_PARALLEL);    //x8 = W3*(x6+x7);

    VoC_movreg32(RL6,ACC1,DEFAULT);
    VoC_msu32inc_rp(REG7,REG2,IN_PARALLEL);           //x6 = x8 - (W3-W5)*x6;

    VoC_msu32inc_rp(REG5,REG2,DEFAULT);               //x7 = x8 - (W3+W5)*x7;
    VoC_lw16i_short(FORMATX,0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,LOCAL_IDCT_BUFFER_Y_ADDR,2);

    VoC_add32_rr(REG67,RL7,ACC1,DEFAULT);             //x1 = x4 + x6;
    VoC_sub32_rr(RL7,RL7,ACC1,IN_PARALLEL);           //x4 = x4 - x6;

    VoC_add32_rr(RL6,RL6,ACC0,DEFAULT);              //x6 = x5 + x7;
    VoC_sub32_rr(ACC1,RL6,ACC0,IN_PARALLEL);          //x5 = x5 - x7;

    VoC_sw32inc_p(REG67,REG0,DEFAULT);                //x1
    VoC_lw16i_short(FORMAT32,-15,IN_PARALLEL);

    VoC_add32_rr(REG45,RL7,ACC1,DEFAULT);             //x4 + x5
    VoC_sub32_rr(REG67,RL7,ACC1,IN_PARALLEL);         //x4 - x5

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_multf32_rp(ACC0,REG5,REG2,DEFAULT);           //L_32 = (L_mult1 (hi1, lo2))<<16;
    VoC_multf32_rp(ACC1,REG7,REG2,IN_PARALLEL);       //L_32 = (L_mult1 (hi1, lo2))<<16;

    VoC_macX_rp(REG4,REG2,DEFAULT);
    VoC_macXinc_rp(REG6,REG2,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,GLOBAL_IDCT_BUFFER_X_ADDR,2);

    VoC_add32_rp(ACC0,ACC0,REG2,DEFAULT);
    VoC_add32_rp(ACC1,ACC1,REG2,IN_PARALLEL);

    VoC_shr32_ri(ACC0,7,DEFAULT);                     //x2 = (181*(x4+x5)+128)>>8;
    VoC_shr32_ri(ACC1,7,IN_PARALLEL);                 //x4 = (181*(x4-x5)+128)>>8;

    VoC_lw16i_short(REG3,7,DEFAULT);
    VoC_pop16(REG1,IN_PARALLEL);                      //&blk

    VoC_sw32inc_p(ACC0,REG0,DEFAULT);                 //x2
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_sw32inc_p(ACC1,REG0,DEFAULT);                 //x4
    VoC_add16_rr(REG3,REG1,REG3,IN_PARALLEL);         //&blk+7

    VoC_lw16i_set_inc(REG0,LOCAL_IDCT_BUFFER_Y_ADDR,2);

    VoC_sw32_p(RL6,REG0,DEFAULT);                     //x6
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_add32_pp(ACC0,REG2,REG0,DEFAULT);            //x7 + x1
    VoC_sub32inc_pp(ACC1,REG2,REG0,IN_PARALLEL);     //x7 - x1

    VoC_shr32_ri(ACC0,8,DEFAULT);
    VoC_add32_pp(RL6,REG2,REG0,IN_PARALLEL);         //x3 + x2

    VoC_shr32_ri(ACC1,8,DEFAULT);
    VoC_sub32inc_pp(RL7,REG2,REG0,IN_PARALLEL);      //x3 - x2

    VoC_shr32_ri(RL6,8,DEFAULT);
    VoC_sw16inc_p(ACC0_LO,REG1,IN_PARALLEL);         //blk[0] = (x7+x1)>>8;

    VoC_shr32_ri(RL7,8,DEFAULT);
    VoC_sw16inc_p(ACC1_LO,REG3,IN_PARALLEL);         //blk[7] = (x7-x1)>>8;

    VoC_add32_pp(ACC0,REG2,REG0,DEFAULT);            //x0 + x4
    VoC_sub32inc_pp(ACC1,REG2,REG0,IN_PARALLEL);     //x0 - x4

    VoC_shr32_ri(ACC0,8,DEFAULT);
    VoC_sw16inc_p(RL6_LO,REG1,IN_PARALLEL);          //blk[1] = (x3+x2)>>8

    VoC_shr32_ri(ACC1,8,DEFAULT);
    VoC_sw16inc_p(RL7_LO,REG3,IN_PARALLEL);          //blk[6] = (x3-x2)>>8

    VoC_add32_pp(RL6,REG2,REG0,DEFAULT);             //x8 + x6
    VoC_sub32inc_pp(RL7,REG2,REG0,IN_PARALLEL);      //x8 + x6

    VoC_shr32_ri(RL6,8,DEFAULT);
    VoC_sw16inc_p(ACC0_LO,REG1,IN_PARALLEL);         //blk[2] = (x0+x4)>>8;

    VoC_shr32_ri(RL7,8,DEFAULT);
    VoC_sw16inc_p(ACC1_LO,REG3,IN_PARALLEL);         //blk[5] = (x0-x4)>>8;

    VoC_sw16inc_p(RL6_LO,REG1,DEFAULT);              //blk[3] = (x8+x6)>>8;
    VoC_lw16i_short(REG7,4,IN_PARALLEL);

    VoC_sw16inc_p(RL7_LO,REG3,DEFAULT);              //blk[4] = (x8-x6)>>8;
    VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

    VoC_return
}




/*
REG1: blk=&temp_lpMCUBuffer+i
RL6:    x1
ACC1_LO:x2
REG4:   x3
REG3:   x4
ACC1_HI:x5
REG7:   x6
REG5:   x7

optimized by Cui 20070913
optimized by Yordan 20080826

*/

void CII_idctcol(void)
{


    VoC_push16(REG1,DEFAULT);//blk
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw16inc_p(ACC0_HI,REG1,DEFAULT);//blk[0]
    VoC_lw32z(RL6,IN_PARALLEL);

    VoC_lw16inc_p(REG3,REG1,DEFAULT);//x4 = blk[8]

    VoC_lw16inc_p(REG4,REG1,DEFAULT);//x3 = blk[16]
    VoC_lw16inc_p(REG5,REG1,DEFAULT);//x7 = blk[24]
    VoC_lw16inc_p(RL6_HI,REG1,DEFAULT);
    VoC_shr32_ri(RL6,8,DEFAULT);//x1 = blk[32]<<8
    VoC_lw16inc_p(REG7,REG1,IN_PARALLEL);//x6 = blk[40]
    VoC_lw16inc_p(REG0,REG1,DEFAULT);//x2 = blk[48]
    VoC_lw16inc_p(REG1,REG1,DEFAULT);//x5 = blk[56]
    /*
       if (!((x1 = (blk[32]<<8)) | (x2 = blk[48]) | (x3 = blk[16]) |
            (x4 = blk[8]) | (x5 = blk[56]) | (x6 = blk[40]) | (x7 = blk[24])))
        {
            blk[0]=blk[8]=blk[16]=blk[24]=blk[32]=blk[40]
                =blk[48]=blk[56]=iclp[(blk[0]+32)>>6];
            return;
        }
    */

    VoC_bnez16_r(idctcol_100,REG3);
    VoC_bnez32_r(idctcol_100,REG45);
    VoC_bnez32_r(idctcol_100,RL6);
    VoC_bnez16_r(idctcol_100,REG7);
    VoC_bnez32_r(idctcol_100,REG01);

    VoC_movreg16(REG2,ACC0_HI,DEFAULT);

    VoC_add16_rd(REG2,REG2,CONST_JPEG_DEC_32_ADDR);
    VoC_shr16_ri(REG2,-1,DEFAULT);

    VoC_shr16_ri(REG2,7,DEFAULT);  //iclp[(blk[0]+32)>>6];
    VoC_pop16(REG1,IN_PARALLEL);

    VoC_loop_i_short(8,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG2,REG1,DEFAULT);
    VoC_endloop0

    VoC_sub16_rd(REG1,REG1,CONST_JPEG_DEC_63_ADDR);

    VoC_return

idctcol_100:

    VoC_lw16i_set_inc(REG2,CONST_JPEG_DEC_1108_ADDR,1);

    VoC_shr32_ri(ACC0,8,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_add32_rd(RL7,ACC0,CONST_JPEG_DEC_0x00002000_ADDR); //x0 = (blk[8*0]<<8) + 8192

    VoC_lw16i_short(ACC0_LO,4,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL)

    VoC_add16_rr(REG6,REG4,REG0,DEFAULT);                   //x3 + x2
    VoC_lw16i_short(FORMATX,0,DEFAULT);

    VoC_push32(ACC0,DEFAULT);
    VoC_movreg32(ACC1,ACC0,IN_PARALLEL);

    VoC_mac32_rp(REG6,REG2,DEFAULT);                        //x1 = W6*(x3+x2) + 4;
    VoC_mac32inc_rp(REG6,REG2,IN_PARALLEL);                 //x1 = W6*(x3+x2) + 4;

    VoC_add16_rr(REG6,REG3,REG1,DEFAULT);                   //x4 + x5
    VoC_msu32inc_rp(REG0,REG2,IN_PARALLEL);                 //x2 = x1 - (W2+W6)*x2;

    VoC_lw16i_set_inc(REG0,GLOBAL_IDCT_BUFFER_X_ADDR,2);

    VoC_mac32inc_rp(REG4,REG2,DEFAULT);                     //x3 = x1 + (W2-W6)*x3;
    VoC_shr32_ri(ACC1,3,IN_PARALLEL);                       //x2 = (x1-(W2+W6)*x2)>>3;

    VoC_add32_rr(RL6,RL7,RL6,DEFAULT);                      //x8 = x0 + x1;
    VoC_sub32_rr(RL7,RL7,RL6,IN_PARALLEL);                  //x0 -= x1;

    VoC_shr32_ri(ACC0,3,DEFAULT);                           //x3 = (x1+(W2-W6)*x3)>>3;
    VoC_add16_rr(REG4,REG7,REG5,IN_PARALLEL);               //x6 + x7

    VoC_add32_rr(ACC0,RL6,ACC0,DEFAULT);                    //x7 = x8 + x3;
    VoC_sub32_rr(RL6,RL6,ACC0,IN_PARALLEL);                 //x8 -= x3;

    VoC_add32_rr(RL7,RL7,ACC1,DEFAULT);                     //x3 = x0 + x2;
    VoC_sub32_rr(ACC1,RL7,ACC1,IN_PARALLEL);                //x0 -= x2;

    VoC_sw32inc_p(ACC0,REG0,DEFAULT);                       //x7
    VoC_lw32_sd(ACC0,0,IN_PARALLEL);

    VoC_mac32_rp(REG6,REG2,DEFAULT);                        //x8 = W7*(x4+x5) + 4;
    VoC_sw32inc_p(RL7,REG0,IN_PARALLEL);                    //x3

    VoC_movreg32(ACC1,ACC0,DEFAULT);
    VoC_mac32inc_rp(REG6,REG2,IN_PARALLEL);                 //x8 = W7*(x4+x5) + 4;

    VoC_sw32inc_p(ACC1,REG0,DEFAULT);                       //x0
    VoC_mac32inc_rp(REG3,REG2,IN_PARALLEL);                 //x4 = x8 + (W1-W7)*x4;

    VoC_msu32inc_rp(REG1,REG2,DEFAULT);                     //x5 = x8 - (W1+W7)*x5;
    VoC_sw32inc_p(RL6,REG0,IN_PARALLEL);                    //x8

    VoC_movreg32(RL7,ACC1,DEFAULT);                         //ACC1 is the result of mac
    VoC_lw32_sd(ACC1,0,IN_PARALLEL);

    VoC_movreg32(RL6,ACC0,DEFAULT);                         //ACC0 is the result of msu
    VoC_pop32(ACC0,IN_PARALLEL);

    VoC_mac32_rp(REG4,REG2,DEFAULT);                        //x8 = W3*(x6+x7) + 4;
    VoC_mac32inc_rp(REG4,REG2,IN_PARALLEL);                 //x8 = W3*(x6+x7) + 4;

    VoC_shr32_ri(RL7,3,DEFAULT);                            //x4 = (x8+(W1-W7)*x4)>>3;
    VoC_msu32inc_rp(REG7,REG2,IN_PARALLEL);                 //x6 = x8 - (W3-W5)*x6;

    VoC_msu32inc_rp(REG5,REG2,DEFAULT);                     //x7 = x8 - (W3+W5)*x7;
    VoC_shr32_ri(RL6,3,IN_PARALLEL);                        //x5 = x8 - (W1+W7)*x5>>3;

    VoC_shr32_ri(ACC1,3,DEFAULT);                           //x6 = (x8-(W3-W5)*x6)>>3;
    VoC_lw16i_short(INC3,-8,IN_PARALLEL);

    VoC_shr32_ri(ACC0,3,DEFAULT);                           //x7 = (x8-(W3+W5)*x7)>>3;
    VoC_lw16i_short(FORMAT32,-15,IN_PARALLEL);

    VoC_add32_rr(REG45,RL6,ACC0,DEFAULT);                   //x6 = x5 + x7;
    VoC_sub32_rr(REG67,RL6,ACC0,IN_PARALLEL);               //x5 -= x7;

    VoC_add32_rr(RL6,RL7,ACC1,DEFAULT);                     //x1 = x4 + x6;
    VoC_sub32_rr(RL7,RL7,ACC1,IN_PARALLEL);                 //x4 -= x6;

    VoC_add32_rr(REG01,RL7,REG67,DEFAULT);                  //x4+x5
    VoC_sub32_rr(REG67,RL7,REG67,IN_PARALLEL);              //x4-x5

    VoC_multf32_rp(ACC0,REG1,REG2,DEFAULT);                 //L_32 = (L_mult1 (hi1, lo2))<<16;
    VoC_multf32_rp(ACC1,REG7,REG2,IN_PARALLEL);             //L_32 = (L_mult1 (hi1, lo2))<<16;

    VoC_shru16_ri(REG0,1,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_macX_rp(REG0,REG2,DEFAULT);                         //L_32_t0 = L_mult1 (lo1, lo2)<<1;
    VoC_macXinc_rp(REG6,REG2,IN_PARALLEL);                  //L_32_t0 = L_mult1 (lo1, lo2)<<1;

    VoC_lw16i_set_inc(REG2,LOCAL_IDCT_BUFFER_Y_ADDR,2);

    VoC_add32_rp(REG67,ACC0,REG2,DEFAULT);
    VoC_add32_rp(RL7,ACC1,REG2,IN_PARALLEL);

    VoC_shr32_ri(REG67,7,DEFAULT);                          //x2 = (181*(x4+x5)+128)>>8;
    VoC_movreg16(REG0,REG2,IN_PARALLEL);                    //&blk

    VoC_sw32inc_p(RL6,REG2,DEFAULT);                        //x1
    VoC_shr32_ri(RL7,7,IN_PARALLEL);                        //x4 = (181*(x4-x5)+128)>>8;

    VoC_sw32inc_p(REG67,REG2,DEFAULT);                      //x2
    VoC_pop16(REG1,IN_PARALLEL);                            //&blk

    VoC_sw32inc_p(RL7,REG2,DEFAULT);                        //x4
    VoC_lw16i_short(REG7,56,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,GLOBAL_IDCT_BUFFER_X_ADDR,2);

    VoC_sw32_p(REG45,REG2,DEFAULT);                         //x6
    VoC_add16_rr(REG3,REG1,REG7,IN_PARALLEL);               //&blk+56

    VoC_add32_pp(ACC0,REG0,REG2,DEFAULT);                   //x7+x1
    VoC_sub32inc_pp(ACC1,REG2,REG0,IN_PARALLEL);            //x7-x1

    VoC_shr32_ri(ACC0,-9,DEFAULT);
    VoC_shr32_ri(ACC1,-9,IN_PARALLEL);

    VoC_shr32_ri(ACC0,7,DEFAULT);
    VoC_add32_pp(RL6,REG0,REG2,IN_PARALLEL);                //x3+x2

    VoC_shr32_ri(ACC1,7,DEFAULT);
    VoC_sub32inc_pp(RL7,REG2,REG0,IN_PARALLEL);             //x3-x2

    VoC_shr32_ri(RL6,-9,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG1,IN_PARALLEL);                //blk[0]  = iclp[(x7+x1)>>14];

    VoC_shr32_ri(RL7,-9,DEFAULT);
    VoC_sw16inc_p(ACC1_HI,REG3,IN_PARALLEL);                //blk[56] = iclp[(x7-x1)>>14];

    VoC_shr32_ri(RL6,7,DEFAULT);
    VoC_add32_pp(ACC0,REG0,REG2,IN_PARALLEL);               //x0+x4

    VoC_shr32_ri(RL7,7,DEFAULT);
    VoC_sub32inc_pp(ACC1,REG2,REG0,IN_PARALLEL);            //x0-x4

    VoC_shr32_ri(ACC0,-9,DEFAULT);
    VoC_sw16inc_p(RL6_HI,REG1,IN_PARALLEL);                 //blk[8]  = iclp[(x3+x2)>>14];

    VoC_shr32_ri(ACC1,-9,DEFAULT);
    VoC_sw16inc_p(RL7_HI,REG3,IN_PARALLEL);                 //blk[48] = iclp[(x3-x2)>>14];

    VoC_shr32_ri(ACC0,7,DEFAULT);
    VoC_add32_pp(RL6,REG0,REG2,IN_PARALLEL);                //x8+x6

    VoC_shr32_ri(ACC1,7,DEFAULT);
    VoC_sub32inc_pp(RL7,REG2,REG0,IN_PARALLEL);             //x8-x6

    VoC_shr32_ri(RL6,-9,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG1,IN_PARALLEL);                //blk[16] = iclp[(x0+x4)>>14];

    VoC_shr32_ri(RL7,-9,DEFAULT);
    VoC_sw16inc_p(ACC1_HI,REG3,IN_PARALLEL);                //blk[40] = iclp[(x0-x4)>>14];

    VoC_shr32_ri(RL6,7,DEFAULT);
    VoC_lw16i_short(REG7,31,IN_PARALLEL);

    VoC_shr32_ri(RL7,7,DEFAULT);
    VoC_sw16inc_p(RL6_HI,REG1,DEFAULT);                     //blk[24] = iclp[(x8+x6)>>14];

    VoC_sub16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_sw16inc_p(RL7_HI,REG3,IN_PARALLEL);                 //blk[32] = iclp[(x8-x6)>>14];

    VoC_return
}

/*
REG1: blk=&temp_lpMCUBuffer+i*8
RL6:    x1
ACC1_LO:x2
REG4:   x3
REG3:   x4
ACC1_HI:x5
REG7:   x6
REG5:   x7

optimized by Cui 20070913
*/
void CII_idctrow4()
{


    VoC_push16(REG1,DEFAULT);//blk

    VoC_lw32inc_p(REG23,REG1,DEFAULT);//blk[0] & x4 = blk[1]
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_lw32inc_p(REG45,REG1,DEFAULT);//x3 = blk[2]&x7 = blk[3]
    VoC_lw32z(ACC0,IN_PARALLEL);

    /*
        if(((x3 = blk[2]) | (x4 = blk[1]) | (x7 = blk[3])) == 0)
        {
            blk[0]=blk[1]=blk[2]=blk[3]=blk[4]=blk[5]=blk[6]=blk[7]=blk[0]<<3;
            return;
        }
    */

    VoC_bnez16_r(idctrow4_100,REG3);
    VoC_bnez32_r(idctrow4_100,REG45);
    VoC_shr16_ri(REG2,-3,DEFAULT);

    VoC_pop16(REG1,DEFAULT);
    VoC_movreg16(REG3,REG2,IN_PARALLEL);

    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0
    VoC_sw32inc_p(REG23,REG1,DEFAULT);
    VoC_endloop0

    VoC_return
idctrow4_100:

    VoC_movreg16(ACC0_HI,REG2,DEFAULT);
    VoC_shr32_ri(ACC0,5,DEFAULT);

    VoC_add32_rd(RL7,ACC0,CONST_JPEG_DEC_0x00000080_ADDR);//x0 = (blk[0]<<11) + 128;


    VoC_multf32_rd(ACC0,REG4,CONST_JPEG_DEC_1108_ADDR);//x2 = W6*x3;
    VoC_mac32_rd(REG4,CONST_JPEG_DEC_1568_ADDR);//x3 = x2 + (W2-W6)*x3;

    VoC_movreg32(ACC1,ACC0,DEFAULT);//ACC0 is the result of multf


    VoC_lw16i_set_inc(REG2,GLOBAL_IDCT_BUFFER_X_ADDR,2);

    VoC_add32_rr(ACC0,RL7,ACC0,DEFAULT);//x7 = x0 + x3;
    VoC_sub32_rr(RL6,RL7,ACC0,IN_PARALLEL);//x8 = x0 - x3;

    VoC_add32_rr(RL7,RL7,ACC1,DEFAULT);//x3 = x0 + x2;
    VoC_sub32_rr(ACC1,RL7,ACC1,IN_PARALLEL);//x0 -= x2;

    VoC_sw32inc_p(ACC0,REG2,DEFAULT);//x7
    VoC_sw32inc_p(RL7,REG2,DEFAULT);//x3



    VoC_multf32_rd(ACC0,REG3,CONST_JPEG_DEC_565_ADDR);//x5 = W7*x4;
    VoC_mac32_rd(REG3,CONST_JPEG_DEC_2276_ADDR);//x4 = x5 + (W1-W7)*x4;
    VoC_movreg32(ACC1,ACC0,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,IN_PARALLEL);//x0

    VoC_movreg32(RL7,ACC0,DEFAULT);
    VoC_sw32inc_p(RL6,REG2,IN_PARALLEL);//x8

    VoC_multf32_rd(ACC0,REG5,CONST_JPEG_DEC_2408_ADDR);//x6 = W3*x7;
    VoC_msu32_rd(REG5,CONST_JPEG_DEC_4017_ADDR);//x7 = x6 - (W3+W5)*x7;
    VoC_movreg32(RL6,ACC0,DEFAULT);//ACC0 is the result of mult
    VoC_lw16i_short(FORMAT32,-15,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,LOCAL_IDCT_BUFFER_Y_ADDR,2);


    VoC_add32_rr(RL6,RL7,RL6,DEFAULT);//x1 = x4 + x6;
    VoC_sub32_rr(RL7,RL7,RL6,IN_PARALLEL);//x4 -= x6;

    VoC_add32_rr(REG45,ACC1,ACC0,DEFAULT);//x6 = x5 + x7;
    VoC_sub32_rr(ACC1,ACC1,ACC0,IN_PARALLEL);//x5 -= x7;


    VoC_add32_rr(REG01,RL7,ACC1,DEFAULT);//x4+x5
    VoC_sub32_rr(ACC1,RL7,ACC1,IN_PARALLEL);//x4-x5


    VoC_multf32_rd(ACC0,REG1,CONST_JPEG_DEC_181_ADDR);//L_32 = (L_mult1 (hi1, lo2))<<16;
    VoC_shru16_ri(REG0,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_mac32_rd(REG0,CONST_JPEG_DEC_181_ADDR);


    VoC_sw32inc_p(RL6,REG2,DEFAULT);//x1
    VoC_add32_rd(REG67,ACC0,CONST_JPEG_DEC_0x00000040_ADDR);
    VoC_shr32_ri(REG67,7,DEFAULT);//x2 = (181*(x4+x5)+128)>>8;
    VoC_lw16i_short(FORMAT32,-15,IN_PARALLEL);

    VoC_movreg32(REG01,ACC1,DEFAULT);

    VoC_multf32_rd(ACC0,REG1,CONST_JPEG_DEC_181_ADDR);//L_32 = (L_mult1 (hi1, lo2))<<16;
    VoC_shru16_ri(REG0,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_mac32_rd(REG0,CONST_JPEG_DEC_181_ADDR);


    VoC_sw32inc_p(REG67,REG2,DEFAULT);//x2

    VoC_add32_rd(RL7,ACC0,CONST_JPEG_DEC_0x00000040_ADDR);
    VoC_shr32_ri(RL7,7,DEFAULT);//  x4 = (181*(x4-x5)+128)>>8;


    VoC_lw16i_set_inc(REG0,LOCAL_IDCT_BUFFER_Y_ADDR,2);

    VoC_sw32inc_p(RL7,REG2,DEFAULT);//x4
    VoC_pop16(REG1,IN_PARALLEL);//&blk+7

    VoC_sw32inc_p(REG45,REG2,DEFAULT);//x6
    VoC_lw16i_short(INC1,1,IN_PARALLEL);


    VoC_lw16i_set_inc(REG2,GLOBAL_IDCT_BUFFER_X_ADDR,2);

    VoC_add16_rd(REG3,REG1,CONST_JPEG_DEC_7_ADDR);

//increase of REG3 is -1 ;

    VoC_add32inc_pp(REG45,REG2,REG0,DEFAULT);//x7+x1;x3+x2;x0+x4;x8+x6
    VoC_sub32_pp(REG67,REG2,REG0,IN_PARALLEL);//x7-x1;x3-x2;x0-x4;x8+x6

    VoC_loop_i_short(4,DEFAULT);
    VoC_shr32_ri(REG45,8,IN_PARALLEL);
    VoC_startloop0

    VoC_shr32_ri(REG67,8,DEFAULT);
    VoC_sw16inc_p(REG4,REG1,DEFAULT);//blk[0] = (x7+x1)>>8;blk[1] = (x3+x2)>>8;blk[2] = (x0+x4)>>8;blk[3] = (x8+x6)>>8;

    VoC_add32inc_pp(REG45,REG2,REG0,DEFAULT);//x7+x1;x3+x2;x0+x4;x8+x6
    VoC_sub32_pp(REG67,REG2,REG0,IN_PARALLEL);//x7-x1;x3-x2;x0-x4;x8+x6

    VoC_sw16inc_p(REG6,REG3,DEFAULT);//blk[7] = (x7-x1)>>8;blk[6] = (x3-x2)>>8;blk[5] = (x0-x4)>>8;blk[4] = (x8-x6)>>8;
    VoC_shr32_ri(REG45,8,IN_PARALLEL);

    VoC_endloop0

    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_JPEG_DEC_4_ADDR);
    VoC_return
}


/*
REG1: blk=&temp_lpMCUBuffer+i*8
RL6:    x1
ACC1_LO:x2
REG4:   x3
REG3:   x4
ACC1_HI:x5
REG7:   x6
REG5:   x7
optimized by Cui 20070914

*/
void CII_idctcol4(void)
{


    VoC_push16(REG1,DEFAULT);//blk

    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw16inc_p(REG2,REG1,DEFAULT);//blk[0]
    VoC_lw16inc_p(REG6,REG1,DEFAULT);//x4 = blk[8]
    VoC_lw16inc_p(REG4,REG1,DEFAULT);//x3 = blk[16]
    VoC_lw16inc_p(REG5,REG1,DEFAULT);//x7 = blk[24]
    VoC_movreg16(ACC0_HI,REG2,IN_PARALLEL);

    VoC_bnez16_r(idctcol4_100,REG6);
    VoC_bnez32_r(idctcol4_100,REG45);

    /*
     if(((x3 = blk[16]) | (x4 = blk[8]) | (x7 = blk[24])) == 0)
        {
            blk[0]=blk[8]=blk[16]=blk[24]=blk[32]=blk[40]=blk[48]=blk[56]=
                iclp[(blk[0]+32)>>6];
            return;
        }
    */
    VoC_add16_rd(REG2,REG2,CONST_JPEG_DEC_32_ADDR);
    VoC_shr16_ri(REG2,-1,DEFAULT);
    VoC_shr16_ri(REG2,7,DEFAULT);
    VoC_pop16(REG1,IN_PARALLEL);


    VoC_loop_i_short(8,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG2,REG1,DEFAULT);
    VoC_endloop0

    VoC_sub16_rd(REG1,REG1,CONST_JPEG_DEC_63_ADDR);

    VoC_return

idctcol4_100:

    VoC_shr32_ri(ACC0,8,DEFAULT);

    VoC_add32_rd(RL7,ACC0,CONST_JPEG_DEC_0x00002000_ADDR);//x0 = (blk[8*0]<<8) + 8192;


    VoC_multf32_rd(ACC0,REG4,CONST_JPEG_DEC_1108_ADDR);//x2 = W6*x3;
    VoC_mac32_rd(REG4,CONST_JPEG_DEC_1568_ADDR);//x3 = x2 + (W2-W6)*x3;

    VoC_movreg32(ACC1,ACC0,DEFAULT);//ACC0 is the result of multf
    VoC_shr32_ri(ACC0,3,DEFAULT);//x3 = (x2 + (W2-W6)*x3)>>3;
    VoC_shr32_ri(ACC1,3,IN_PARALLEL);//x2 >>= 3;

    VoC_lw16i_set_inc(REG2,GLOBAL_IDCT_BUFFER_X_ADDR,2);

    VoC_add32_rr(ACC0,RL7,ACC0,DEFAULT);//x7 = x0 + x3;
    VoC_sub32_rr(RL6,RL7,ACC0,IN_PARALLEL);//x8 = x0 - x3;

    VoC_add32_rr(RL7,RL7,ACC1,DEFAULT);//x3 = x0 + x2;
    VoC_sub32_rr(ACC1,RL7,ACC1,IN_PARALLEL);//x0 -= x2;


    VoC_sw32inc_p(ACC0,REG2,DEFAULT);//x7
    VoC_sw32inc_p(RL7,REG2,DEFAULT);//x3

    VoC_multf32_rd(ACC0,REG6,CONST_JPEG_DEC_565_ADDR);//x5 = W7*x4;
    VoC_mac32_rd(REG6,CONST_JPEG_DEC_2276_ADDR);//x4 = x5 + (W1-W7)*x4;
    VoC_movreg32(ACC1,ACC0,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,IN_PARALLEL);//x0
    VoC_movreg32(RL7,ACC0,DEFAULT);
    VoC_sw32inc_p(RL6,REG2,IN_PARALLEL);//x8
    VoC_shr32_ri(ACC1,3,DEFAULT);//x5 >>= 3;
    VoC_shr32_ri(RL7,3,IN_PARALLEL);//x4 = (x5 + (W1-W7)*x4)>>3;


    VoC_multf32_rd(ACC0,REG5,CONST_JPEG_DEC_2408_ADDR);//x6 = W3*x7;
    VoC_msu32_rd(REG5,CONST_JPEG_DEC_4017_ADDR);//x7 = x6 - (W3+W5)*x7;
    VoC_movreg32(RL6,ACC0,DEFAULT);//ACC0 is the result of mult
    VoC_lw16i_short(FORMAT32,-15,IN_PARALLEL);
    VoC_shr32_ri(RL6,3,DEFAULT);//x6 >>= 3;
    VoC_shr32_ri(ACC0,3,IN_PARALLEL);//x7 = (x6 - (W3+W5)*x7)>>3;

    VoC_lw16i_set_inc(REG2,LOCAL_IDCT_BUFFER_Y_ADDR,2);


    VoC_add32_rr(RL6,RL7,RL6,DEFAULT);//x1 = x4 + x6;
    VoC_sub32_rr(RL7,RL7,RL6,IN_PARALLEL);//x4 -= x6;

    VoC_add32_rr(REG45,ACC1,ACC0,DEFAULT);//x6 = x5 + x7;
    VoC_sub32_rr(ACC1,ACC1,ACC0,IN_PARALLEL);//x5 -= x7;



    VoC_add32_rr(REG01,RL7,ACC1,DEFAULT);//x4+x5
    VoC_sub32_rr(ACC1,RL7,ACC1,IN_PARALLEL);//x4-x5

    VoC_multf32_rd(ACC0,REG1,CONST_JPEG_DEC_181_ADDR);//L_32 = (L_mult1 (hi1, lo2))<<16;
    VoC_shru16_ri(REG0,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_mac32_rd(REG0, CONST_JPEG_DEC_181_ADDR);//L_32_t0 = L_mult1 (lo1, lo2)<<1;

    VoC_sw32inc_p(RL6,REG2,DEFAULT);//x1

    VoC_add32_rd(REG67,ACC0,CONST_JPEG_DEC_0x00000040_ADDR);
    VoC_shr32_ri(REG67,7,DEFAULT);//x2 = (181*(x4+x5)+128)>>8;
    VoC_lw16i_short(FORMAT32,-15,IN_PARALLEL);

    VoC_movreg32(REG01,ACC1,DEFAULT);

    VoC_multf32_rd(ACC0,REG1,CONST_JPEG_DEC_181_ADDR);//L_32 = (L_mult1 (hi1, lo2))<<16;
    VoC_shru16_ri(REG0,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);


    VoC_mac32_rd(REG0, CONST_JPEG_DEC_181_ADDR);//L_32_t0 = L_mult1 (lo1, lo2)<<1;
    VoC_sw32inc_p(REG67,REG2,DEFAULT);//x2


    VoC_add32_rd(RL7,ACC0,CONST_JPEG_DEC_0x00000040_ADDR);
    VoC_shr32_ri(RL7,7,DEFAULT);//  x4 = (181*(x4-x5)+128)>>8;

    VoC_lw16i_set_inc(REG0,GLOBAL_IDCT_BUFFER_X_ADDR,2);

    VoC_sw32inc_p(RL7,REG2,DEFAULT);//x4
    VoC_lw16i_short(INC3,-8,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG2,DEFAULT);//x6

    VoC_lw16i_set_inc(REG2,LOCAL_IDCT_BUFFER_Y_ADDR,2);

    VoC_pop16(REG1,DEFAULT);//&blk
    VoC_lw16i_short(REG3,56,IN_PARALLEL);

    VoC_add16_rr(REG3,REG3,REG1,DEFAULT); //&blk+56

    VoC_loop_i_short(4,DEFAULT);
    VoC_add32_pp(REG45,REG0,REG2,IN_PARALLEL);//x7+x1;x3+x2;x0+x4;x8+x6
    VoC_startloop0

    VoC_shr32_ri(REG45,-9,DEFAULT);
    VoC_sub32inc_pp(REG67,REG0,REG2,IN_PARALLEL);//x7-x1;x3-x2;x0-x4;x8+x6

    VoC_shr16_ri(REG5,7,DEFAULT);
    VoC_shr32_ri(REG67,-9,IN_PARALLEL);

    VoC_shr16_ri(REG7,7,DEFAULT);
    VoC_add32_pp(REG45,REG0,REG2,IN_PARALLEL);//x7+x1;x3+x2;x0+x4;x8+x6

    VoC_sw16inc_p(REG5,REG1,DEFAULT);//blk[0]  = iclp[(x7+x1)>>14];
    VoC_sw16inc_p(REG7,REG3,DEFAULT);//blk[56] = iclp[(x7-x1)>>14];
    //blk[8]  = iclp[(x3+x2)>>14];
    //blk[48] = iclp[(x3-x2)>>14];
    //blk[16] = iclp[(x0+x4)>>14];
    //blk[40] = iclp[(x0-x4)>>14];
    //blk[24] = iclp[(x8+x6)>>14];
    //blk[32] = iclp[(x8-x6)>>14];

    VoC_endloop0

    VoC_sub16_rd(REG1,REG1,CONST_JPEG_DEC_31_ADDR);

    VoC_return

}










