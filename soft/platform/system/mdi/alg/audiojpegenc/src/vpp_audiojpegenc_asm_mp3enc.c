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

voc_word STRUCT_INPUT_slot_lag_ADDR ,y
voc_word STRUCT_INPUT_frac_slots_per_frame_ADDR  ,y

voc_short STRUCT_INPUT_whole_slots_per_frame_ADDR  ,y
voc_short MP3ENC_MP3_CONST_bitrate_map_ADDR  ,15,y

voc_struct STRUCT_CONFIG_ADDR,y
voc_short  STRUCT_CONFIG_mpeg_bitrate_index_ADDR,y
voc_short  STRUCT_CONFIG_mpeg_samplerate_index_ADDR,y
voc_short  STRUCT_CONFIG_mpeg_padding_ADDR,y



voc_short  STRUCT_CONFIG_mpeg_type_ADDR,y
voc_short  STRUCT_CONFIG_mpeg_crc_ADDR,y
voc_short  STRUCT_CONFIG_mpeg_mode_ADDR,y
voc_short  STRUCT_CONFIG_mpeg_emph_ADDR,y
voc_short  STRUCT_CONFIG_mpeg_ext_ADDR,y
voc_short  STRUCT_CONFIG_mpeg_mode_ext_ADDR,y
voc_short  STRUCT_CONFIG_mpeg_copyright_ADDR,y
voc_short  STRUCT_CONFIG_mpeg_original_ADDR,2,y
voc_struct_end



//global x
voc_short OUTPUT_MP3_BUFFER_ADDR,1024,x
voc_short GLOBAL_XW_begine_ADDR, 1024,x

voc_struct IN_MP3_BUFFER,x
voc_short  IN_MP3_BUFFER_10,576,x
voc_short  IN_MP3_BUFFER_11,576,x
voc_short  IN_MP3_BUFFER_20,576,x
voc_short  IN_MP3_BUFFER_21,576,x
voc_struct_end

voc_alias  GLOBAL_L3ENC_gr1 IN_MP3_BUFFER,x

voc_struct  SHINE_GLOBAL_VARS_X_START_ADDR , x
voc_short STATIC_PartHoldersInitialized_ADDR, x
voc_short STATIC_buf_bit_idx_ADDR,x
voc_short STATIC_buf_short_idx_ADDR,x
voc_short STATIC_buf_long_idx_ADDR,x
voc_word  STATIC_buf_chache_ADDR,x
voc_word  STATIC_BitCount_ADDR,x
voc_word  STATIC_ThisFrameSize_ADDR,x
voc_word  STATIC_BitsRemaining_ADDR,x
voc_word  GLOBAL_OFF_ADDR,x
voc_short  STATIC_FIRSTCALL_ADDR,x
voc_short  GLOBAL_main_data_begin_ADDR,x
voc_word   GLOBAL_xrmax_ADDR,x
voc_short  STATIC_ResvSize_ADDR,x
voc_short  STATIC_scalefac_band_long_ADDR,x
voc_short  STATIC_change_ADDR,x
voc_short  STATIC_nGranules_ADDR,x
voc_word   STRUCT_frameData_frameLength_ADDR,x
voc_word   GLOBLE_channel_ADDR,x
voc_struct_end

voc_struct STRUCT_frameData_ADDR,x
voc_word  STRUCT_frameData_STRUCT_header_ADDR,x
voc_word  STRUCT_frameData_STRUCT_frameSI_ADDR,x
voc_word  STRUCT_frameData_STRUCT_channelSI_ADDR,2,x
voc_word  STRUCT_frameData_STRUCT_spectrumSI_ADDR,4,x
voc_struct_end

voc_struct GLOBAL_MDCT_FREQ_00_ADDR,x
voc_short  GLOBAL_Z_ADDR,1024,x
voc_short  GLOBAL_Y_ADDR,140, x
voc_struct_end

voc_alias   TEMP_MDCT_ADDR GLOBAL_Y_ADDR,x
voc_alias    GLOBAL_mdct_in_ADDR    TEMP_MDCT_ADDR ,x
voc_alias    LOCAL_f_tab_ADDR       (72 + TEMP_MDCT_ADDR),x
voc_alias    TEMP_A_ADDR            (108 + TEMP_MDCT_ADDR) ,x
voc_alias    TEMP_A_ADDR_9          (18 + TEMP_A_ADDR),x

voc_short GLOBAL_L3ENC_gr0,2304,x


//const dat
voc_short TABLE_t1HB_tab_ADDR,2,x
voc_short TABLE_t2HB_tab_ADDR,5,x
voc_short TABLE_t3HB_tab_ADDR,5,x
voc_short TABLE_t5HB_tab_ADDR,8,x
voc_short TABLE_t6HB_tab_ADDR,8,x
voc_short TABLE_t7HB_tab_ADDR,18,x
voc_short TABLE_t8HB_tab_ADDR,18,x
voc_short TABLE_t9HB_tab_ADDR,18,x
voc_short TABLE_t10HB_tab_ADDR,32,x
voc_short TABLE_t11HB_tab_ADDR,32,x
voc_short TABLE_t12HB_tab_ADDR,32,x
voc_short TABLE_t13HB_tab_ADDR,128,x
voc_short TABLE_t15HB_tab_ADDR,128,x
voc_short TABLE_t16HB_tab_ADDR,256,x
voc_short TABLE_t24HB_tab_ADDR,256,x
voc_short TABLE_t32HB_tab_ADDR,8,x
voc_short TABLE_t33HB_tab_ADDR,8,x
voc_short TABLE_t1l_tab_ADDR,2,x
voc_short TABLE_t2l_tab_ADDR,5,x
voc_short TABLE_t3l_tab_ADDR,5,x
voc_short TABLE_t5l_tab_ADDR,8,x
voc_short TABLE_t6l_tab_ADDR,8,x
voc_short TABLE_t7l_tab_ADDR,18,x
voc_short TABLE_t8l_tab_ADDR,18,x
voc_short TABLE_t9l_tab_ADDR,18,x
voc_short TABLE_t10l_tab_ADDR,32,x
voc_short TABLE_t11l_tab_ADDR,32,x
voc_short TABLE_t12l_tab_ADDR,32,x
voc_short TABLE_t13l_tab_ADDR,128,x
voc_short TABLE_t15l_tab_ADDR,128,x
voc_short TABLE_t16l_tab_ADDR,128,x
voc_short TABLE_t24l_tab_ADDR,128,x
voc_short TABLE_t32l_tab_ADDR,8,x
voc_short TABLE_t33l_tab_ADDR,8,x
voc_short TABLE_ht_tab_ADDR,170,x
voc_short TABLE_sfBandIndex_tab_ADDR,222,x
voc_short TABLE_subdv_table_tab_ADDR,24,x
voc_short TABLE_coef_tab_ADDR,72,x
voc_short TABLE_encodeSideInfo_tab_ADDR,12,x
voc_short MP3_CONST_inverse_Zig_Zag_ADDR,78,x
voc_short MP3_CONST_1152_ADDR ,x
voc_short MP3_CONST_NEG7_ADDR ,x
voc_short MP3_CONST_1_WORD32_ADDR ,2 ,x
voc_short MP3_CONST_neg1_ADDR,x
voc_short MP3_CONST_NEG31_ADDR ,x
voc_short MP3_CONST_0x90d0614_ADDR, 2,x
voc_short MP3_CONST_0X28514_165140_ADDR ,2,x
voc_short MP3_CONST_10000_WORD32_ADDR , 2,x
voc_short MP3_CONST_0X33E40_212544_ADDR ,2,x
voc_short MP3_CONST_0X186A0_100000_ADDR, 2,x




voc_struct STRUCT_SIDEINFO_ADDR,y
voc_short  STRUCT_SIDEINFO_main_data_begin_ADDR,2,y
voc_short  STRUCT_SIDEINFO_private_bits_ADDR,2,y
voc_short  STRUCT_SIDEINFO_resvDrain_ADDR,2,y
voc_short  STRUCT_SIDEINFO_scfsi_ADDR,16,y
voc_short  STRUCT_SIDEINFO_gr0_ch0_part2_3_length_ADDR,92,y
voc_short  STRUCT_SIDEINFO_gr1_ch0_part2_3_length_ADDR,92,y
voc_struct_end

voc_struct STRUCT_GRINFO_ADDR,y
voc_short  STRUCT_GRINFO_part2_3_length_ADDR,2,y
voc_short  STRUCT_GRINFO_big_values_ADDR,2,y
voc_short  STRUCT_GRINFO_count1_ADDR,2,y
voc_short  STRUCT_GRINFO_global_gain_ADDR,2,y
voc_short  STRUCT_GRINFO_scalefac_compress_ADDR,2,y
voc_short  STRUCT_GRINFO_table_select_ADDR,2,y
voc_short  STRUCT_GRINFO_table_select1_ADDR,2,y
voc_short  STRUCT_GRINFO_table_select2_ADDR,2,y
voc_short  STRUCT_GRINFO_region0_count_ADDR,2,y
voc_short  STRUCT_GRINFO_region1_count_ADDR,2,y
voc_short  STRUCT_GRINFO_preflag_ADDR,2,y
voc_short  STRUCT_GRINFO_scalefac_scale_ADDR,2,y
voc_short  STRUCT_GRINFO_count1table_select_ADDR,2,y
voc_short  STRUCT_GRINFO_part2_length_ADDR,2,y
voc_short  STRUCT_GRINFO_sfb_lmax_ADDR,2,y
voc_short  STRUCT_GRINFO_address1_ADDR,2,y
voc_short  STRUCT_GRINFO_address2_ADDR,2,y
voc_short  STRUCT_GRINFO_address3_ADDR,2,y
voc_short  STRUCT_GRINFO_quantizerStepSize_ADDR,2,y
voc_struct_end

voc_short  STATIC_RQ_ADDR,100,y

voc_struct GLOBAL_l3_sb_sample_ADDR,y
voc_short  l3_sb_sample_0_0_0_0_ADDR   ,1152,y
voc_short  l3_sb_sample_0_1_0_0_ADDR   ,1152,y
voc_short  l3_sb_sample_1_0_0_0_ADDR   ,1152,y
voc_short  l3_sb_sample_1_1_0_0_ADDR   ,1152,y
voc_struct_end

voc_short  GLOBAL_MDCT_FREQ_01_ADDR,1164,y
voc_short  GLOBAL_RXAB_ADDR,1152,y


voc_alias  SHINE_LOCAL_VARS_Y_START_ADDR    (GLOBAL_MDCT_FREQ_01_ADDR)  ,y

voc_alias    STRUCT_frameData_STRUCT_header_element_ADDR                (0   +    SHINE_LOCAL_VARS_Y_START_ADDR),y
voc_alias    STRUCT_frameData_STRUCT_frameSI_element_ADDR               (30   +    SHINE_LOCAL_VARS_Y_START_ADDR),y
voc_alias    STRUCT_frameData_STRUCT_channelSI_ch0_element_ADDR         (36   +    SHINE_LOCAL_VARS_Y_START_ADDR),y
voc_alias    STRUCT_frameData_STRUCT_channelSI_ch1_element_ADDR         (44   +    SHINE_LOCAL_VARS_Y_START_ADDR),y
voc_alias    STRUCT_frameData_STRUCT_spectrumSI_gr0ch0_element_ADDR     (52   +    SHINE_LOCAL_VARS_Y_START_ADDR),y
voc_alias    STRUCT_frameData_STRUCT_spectrumSI_gr0ch1_element_ADDR     (78   +    SHINE_LOCAL_VARS_Y_START_ADDR),y
voc_alias    STRUCT_frameData_STRUCT_spectrumSI_gr1ch0_element_ADDR     (104   +    SHINE_LOCAL_VARS_Y_START_ADDR),y
voc_alias    STRUCT_frameData_STRUCT_spectrumSI_gr1ch1_element_ADDR     (130   +    SHINE_LOCAL_VARS_Y_START_ADDR),y
voc_alias    STRUCT_frameData_STRUCT_codedData_gr0ch0_element_ADDR       (160   +    SHINE_LOCAL_VARS_Y_START_ADDR),y//2000

voc_alias    TEMP_B_ADDR        STRUCT_GRINFO_ADDR ,y
voc_alias    TEMP_B_ADDR_9      (18+ TEMP_B_ADDR)  ,y
//const data
voc_short  TABLE_fl_tab_ADDR,992,y
voc_short  TABLE_ew_tab_ADDR,500,y
voc_short  TABLE_w_tab_ADDR,36,y
voc_short  TABLE_w2_tab_ADDR,18,y
voc_short  TABLE_win_tab_ADDR,72,y
voc_short  TABLE_steptabi_tab_ADDR,256,y
voc_short  TABLE_pow075_tab_tab_ADDR,24,y
voc_short  TABLE_putmask_tab_ADDR,10,y
voc_short  TABLE_ca_encode_tab_ADDR,16,y
voc_short  TABLE_cs_encode_tab_ADDR,16,y


//CONST DEFINE
voc_alias   MP3_CONST_2_ADDR           (5 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_3_ADDR           (6 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_4_ADDR           (14 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_5_ADDR           (15 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_6_ADDR           (27 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_7_ADDR           (28 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_8_ADDR           (2 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_9_ADDR           (4 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_10_ADDR          (7 + MP3_CONST_inverse_Zig_Zag_ADDR)   ,x
voc_alias   MP3_CONST_11_ADDR          (13 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_12_ADDR          (16 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_13_ADDR          (26 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_14_ADDR          (29 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_15_ADDR          (42 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_16_ADDR          (3 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_17_ADDR          ( 8 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_18_ADDR          ( 12 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_19_ADDR          (17 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_20_ADDR          (25 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_21_ADDR          (30 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_22_ADDR          (41 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_23_ADDR          (43 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_24_ADDR          (9 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_25_ADDR          (11 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_26_ADDR          (18 + MP3_CONST_inverse_Zig_Zag_ADDR),x
voc_alias   MP3_CONST_27_ADDR          (24 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_28_ADDR          (31 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_29_ADDR          (40 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_30_ADDR          (44 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_31_ADDR           (53 + MP3_CONST_inverse_Zig_Zag_ADDR),x
voc_alias   MP3_CONST_32_ADDR          (10 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_33_ADDR          (19 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_34_ADDR          (23 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_35_ADDR          (32 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_36_ADDR          (39 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_37_ADDR          (45 + MP3_CONST_inverse_Zig_Zag_ADDR),x
voc_alias   MP3_CONST_38_ADDR          (52 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_39_ADDR          (54 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_40_ADDR          (20 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_41_ADDR          (22 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_42_ADDR           (33 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_43_ADDR          (38 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_44_ADDR          (46 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_45_ADDR          (51 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_46_ADDR          (55 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_47_ADDR          (60 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_48_ADDR          (21 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_49_ADDR           (34 + MP3_CONST_inverse_Zig_Zag_ADDR),x
voc_alias   MP3_CONST_50_ADDR           (37 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_51_ADDR           (47 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_52_ADDR           (50 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_53_ADDR           (56 + MP3_CONST_inverse_Zig_Zag_ADDR),x
voc_alias   MP3_CONST_54_ADDR           (59 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_55_ADDR           (61 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_56_ADDR           (35 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_57_ADDR           (36 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_58_ADDR           (48 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_59_ADDR           (49 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_60_ADDR           ( 57 + MP3_CONST_inverse_Zig_Zag_ADDR),x
voc_alias   MP3_CONST_61_ADDR           (58 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_62_ADDR           ( 62 + MP3_CONST_inverse_Zig_Zag_ADDR) ,x
voc_alias   MP3_CONST_63_ADDR           (63 + MP3_CONST_inverse_Zig_Zag_ADDR)  ,x
voc_alias   MP3_CONST_64_ADDR           (52 + TABLE_t16HB_tab_ADDR)  ,x
voc_alias   MP3_CONST_66_ADDR           (80 + TABLE_t16HB_tab_ADDR)  ,x

voc_alias   MP3_CONST_122_ADDR           (22 + TABLE_t24HB_tab_ADDR)  ,x
voc_alias   MP3_CONST_128_ADDR           (37 + TABLE_t24HB_tab_ADDR)  ,x
voc_alias   MP3_CONST_164_ADDR           (71 + TABLE_t16HB_tab_ADDR)  ,x

voc_alias   MP3_CONST_255_ADDR            (107 + TABLE_ht_tab_ADDR) ,x
voc_alias   MP3_CONST_0xff_ADDR           (MP3_CONST_255_ADDR) ,x
voc_alias   MP3_CONST_511_ADDR            (147 + TABLE_ht_tab_ADDR) ,x
voc_alias   MP3_CONST_512_ADDR            (190 + TABLE_steptabi_tab_ADDR) ,y
voc_alias   MP3_CONST_576_ADDR            (22 + TABLE_sfBandIndex_tab_ADDR) ,x
voc_alias   MP3_CONST_4095_ADDR           (15 + TABLE_cs_encode_tab_ADDR)  ,y
////////////////////MP3_CONST_32/////////////////////////////
voc_alias   MP3_CONST_0_ADDR               (TABLE_ht_tab_ADDR)  ,x
voc_alias   MP3_CONST_0_WORD32_ADDR        (MP3_CONST_0_ADDR)  ,x
voc_alias   MP3_CONST_1_ADDR               (MP3_CONST_inverse_Zig_Zag_ADDR + 1 )  ,x
voc_alias   MP3_CONST_8192_WORD32_ADDR     (158 + TABLE_steptabi_tab_ADDR )  ,y
voc_alias   MP3_CONST_0X40000000_ADDR      ( 22 + TABLE_steptabi_tab_ADDR )   ,y
#endif


void L3_compress(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_bez16_d(LABLE_AFTER_MP3INIT,Input_reset_addr);
    VoC_jal(MP3_reset);
LABLE_AFTER_MP3INIT:
    VoC_bez16_d(L3_compress_L0,STRUCT_INPUT_frac_slots_per_frame_ADDR);

    VoC_lw16i(REG7,32768);
    VoC_shru32_ri(REG67,16,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_sub32_dr(REG67,STRUCT_INPUT_frac_slots_per_frame_ADDR,REG67);
    VoC_bnlt32_rd(L3_compress_L1,REG67,STRUCT_INPUT_slot_lag_ADDR);
    VoC_lw32_d(REG67,STRUCT_INPUT_frac_slots_per_frame_ADDR);
    VoC_lw16i_short(REG5,0,DEFAULT);
L3_compress_L1:
    VoC_sub32_dr(REG67,STRUCT_INPUT_slot_lag_ADDR,REG67);
    VoC_sw16_d(REG5,STRUCT_CONFIG_mpeg_padding_ADDR);
L3_compress_L0:
    VoC_lw16i_short(REG2,2,DEFAULT);
    VoC_bne16_rd(LABLE_MP3_MONO,REG2,Input_Channel_Num_addr);
    VoC_jump(LABLE_MP3_STERO);
LABLE_MP3_MONO:
    ///dmai  read data////////////////////
    VoC_lw32_d(ACC1,Input_inStreamBufAddr_addr);
    VoC_lw16i(REG2,288);//size[int]
    VoC_lw16i(REG3,(IN_MP3_BUFFER)/2);//addr[int]
    VoC_lw32z(RL6,DEFAULT);
    VoC_sw32_d(REG23,CFG_DMA_SIZE);
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    ////////////2/4///////////////////////////
    VoC_add16_rr(REG3,REG3,REG2,DEFAULT);
    VoC_lw16i(RL6_LO,1152);//byte
    VoC_sw32_d(REG23,CFG_DMA_SIZE);
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    ////////////3/4///////////////////////////
    VoC_add16_rr(REG3,REG3,REG2,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,RL6,DEFAULT);
    VoC_sw32_d(REG23,CFG_DMA_SIZE);
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    ////////////4/4///////////////////////////
    VoC_add16_rr(REG3,REG3,REG2,DEFAULT);
    VoC_NOP();
    VoC_sw32_d(REG23,CFG_DMA_SIZE);
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    VoC_jump(LABLE_MP3_ENC_BEGIN);

LABLE_MP3_STERO:
    VoC_lw32_d(ACC1,Input_inStreamBufAddr_addr);
    VoC_lw16i(REG4,576);//size[int]
    VoC_lw16i(REG5,(GLOBAL_RXAB_ADDR)/2);//addr[int]  in ramy
    VoC_lw32z(RL6,DEFAULT);
    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_lw16i(RL6_LO,2304);//byte
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_lw16i_set_inc(REG0,GLOBAL_RXAB_ADDR,2);
    VoC_lw16i_set_inc(REG1,IN_MP3_BUFFER_10,1);



    VoC_lw32inc_p(REG45,REG0,DEFAULT);//1
    VoC_movreg32(RL7,REG45,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,IN_MP3_BUFFER_11,1);

    VoC_add32_rr(ACC1,ACC1,RL6,DEFAULT);
    VoC_loop_i(0,144)
    VoC_lw32inc_p(REG67,REG0,DEFAULT);//2
    VoC_sw16inc_p(REG4,REG1,IN_PARALLEL);//_1

    VoC_sw16inc_p(REG5,REG2,DEFAULT);//_2
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);//3

    VoC_sw16inc_p(REG6,REG1,DEFAULT);//_3
    VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);//4

    exit_on_warnings = OFF;
    VoC_sw16inc_p(REG7,REG2,DEFAULT);//_4


    VoC_sw16inc_p(REG4,REG1,DEFAULT);//_5
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);//1

    VoC_sw16inc_p(REG5,REG2,DEFAULT);//_6
    exit_on_warnings = ON;
    VoC_sw16inc_p(REG6,REG1,DEFAULT);//_7
    VoC_sw16inc_p(REG7,REG2,DEFAULT);//_8

    VoC_endloop0

    ////////////////
    VoC_sw32_d(RL7,CFG_DMA_SIZE);
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_lw16i_set_inc(REG0,GLOBAL_RXAB_ADDR,2);
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_lw16i_set_inc(REG1,IN_MP3_BUFFER_20,1);

    VoC_lw32inc_p(REG45,REG0,DEFAULT);//1
    VoC_lw16i_set_inc(REG2,IN_MP3_BUFFER_21,1);

    VoC_loop_i(0,144)
    VoC_lw32inc_p(REG67,REG0,DEFAULT);//2
    VoC_sw16inc_p(REG4,REG1,IN_PARALLEL);//_1

    VoC_sw16inc_p(REG5,REG2,DEFAULT);//_2
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);//3

    VoC_sw16inc_p(REG6,REG1,DEFAULT);//_3
    VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);//4

    exit_on_warnings = OFF;
    VoC_sw16inc_p(REG7,REG2,DEFAULT);//_4


    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);//1

    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    exit_on_warnings = ON;
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,DEFAULT);

    VoC_endloop0
    /////////////////////////////////////////////////////////////
LABLE_MP3_ENC_BEGIN:
    VoC_lw16_d(REG4,STRUCT_INPUT_whole_slots_per_frame_ADDR);
    VoC_add16_rd(REG4,REG4,STRUCT_CONFIG_mpeg_padding_ADDR);
    VoC_shr16_ri(REG4,-3,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);
    VoC_lw16i(REG2,288);

    VoC_sub16_rr(REG6,REG4,REG2,DEFAULT);//r2=288
    VoC_shr16_ri(REG6,1,DEFAULT);//mean_bits
    VoC_lw16i_short(REG4,0,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_sw16_d(REG4,STRUCT_frameData_frameLength_ADDR);
    exit_on_warnings = ON;

    VoC_sw16_d(REG0,STATIC_buf_short_idx_ADDR);
    VoC_lw16i(REG2,l3_sb_sample_0_1_0_0_ADDR);//  p_sb1 = &l3_sb_sample[0][1][0][0];
    VoC_lw16i(REG3,l3_sb_sample_1_1_0_0_ADDR);//  p_sb2 = &l3_sb_sample[1][1][0][0];


    VoC_lw16i(REG7,GLOBAL_L3ENC_gr0);
    VoC_lw16i_set_inc(REG0,IN_MP3_BUFFER,1);

    VoC_push16(REG7,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_push16(REG6,DEFAULT);
    VoC_push32(REG23,DEFAULT);
    VoC_push16(REG4,IN_PARALLEL);

    VoC_push32(REG45,DEFAULT);
    VoC_push16(REG5,IN_PARALLEL);

    VoC_push16(REG3,DEFAULT);


LABEL_L3_compress_LOOP2:// for(gr=0;gr<2;gr++)

    VoC_push16(REG0,DEFAULT);//buffer[1] addr
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,GLOBAL_Z_ADDR,2);


    VoC_loop_i(1,2)//initialization    for (i=1;i<=6;i++)
    VoC_loop_i_short(7,DEFAULT);   //z[i]=0;  z[512-i]=0;  z[0]=0
    VoC_sw16_sd(REG7,5,IN_PARALLEL);
    VoC_startloop0
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_endloop0
    VoC_lw16i_set_inc(REG1,GLOBAL_Z_ADDR+1022,-2);
    VoC_endloop1

    VoC_lw16_sd(REG5,2,DEFAULT);
    VoC_sw32_sd(REG23,1,IN_PARALLEL);

    VoC_sw16_sd(REG4,3,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);


    /*for(channel=config.wave.channels-1; channel>=0;channel-- )*/
LABEL_L3_compress_LOOP1:/*for(i=0;i<18;i++)*/
    VoC_sw16_sd(REG3,1,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_sw32_sd(REG45,0,DEFAULT);
    VoC_sw16_sd(REG0,0,IN_PARALLEL);

    VoC_jal(L3_window_filter_subband);

    VoC_lw32_sd(REG45,0,DEFAULT);//r6:i
    VoC_lw16_sd(REG3,1,IN_PARALLEL);

    VoC_add16_rd(REG4,REG4,MP3_CONST_1_ADDR);
    VoC_add16_rd(REG3,REG3,MP3_CONST_64_ADDR);//r3:*&l3_sb_sample[channel][1][i][0]*

    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_add16_rd(REG0,REG0,MP3_CONST_32_ADDR);

    VoC_blt16_rd(LABEL_L3_compress_LOOP1,REG4,MP3_CONST_18_ADDR);//
    VoC_sub16_rd(REG5,REG5,MP3_CONST_1_ADDR);


    VoC_lw32_sd(REG23,1,DEFAULT);
    VoC_lw16i_short(REG4,0,DEFAULT);//i=0;
    VoC_movreg16(REG3,REG2,IN_PARALLEL);

    VoC_bnltz16_r(LABEL_L3_compress_LOOP1,REG5);

    VoC_lw32_sd(REG23,1,DEFAULT);
    VoC_jal(L3_mdct_sub);


    // VoC_counter_stats(&voc_counter_default);
    // L3_iteration_loop(mdct_freq,&side_info, l3_enc, mean_bits,&scalefactor,0,xrabs,gr);
    VoC_lw16_sd(REG7,3,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_jal(L3_iteration_loop);

    // L3_iteration_loop(mdct_freq,&side_info, l3_enc, mean_bits,&scalefactor,1,xrabs,gr);
    VoC_lw16_sd(REG7,3,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_jal(L3_iteration_loop);
    ////////////////////////////////////////////
    //       for ( j =  0; j < config.wave.channels; j++ )
    //        {
    //            int *pi = &l3_enc[gr][j][0];//r0
    //            long *pr = &mdct_freq[j][0];//r1
    //            for ( i = 0; i < 576; i++, pr++, pi++ )
    //            {
    //                if ( (*pr < 0) && (*pi > 0) )
    //                    *pi *= -1;
    //            }
    //        }
    VoC_lw16_sd(REG0,5,DEFAULT);//&l3_enc[gr][0][0]
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_add16_rd(REG1,REG0,MP3_CONST_1152_ADDR);//&l3_enc[gr][1][0]
    VoC_lw16i_short(INC3,2,DEFAULT);
    VoC_lw16i_set_inc(REG2,GLOBAL_MDCT_FREQ_00_ADDR,2);
    VoC_lw16i_set_inc(REG3,GLOBAL_MDCT_FREQ_01_ADDR,2);
    VoC_loop_i(1,18)
    VoC_loop_i_short(32,DEFAULT);
    VoC_movreg32(REG45,REG01,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);//GLOBAL_MDCT_FREQ_00_ADDR
    VoC_lw32inc_p(ACC1,REG0,DEFAULT);//&l3_enc[gr][0][0]
    VoC_lw32inc_p(RL6,REG3,IN_PARALLEL);//GLOBAL_MDCT_FREQ_01_ADDR
    VoC_lw32inc_p(RL7,REG1,DEFAULT);//&l3_enc[gr][1][0]
    VoC_bnltz32_r(LABEL_L3_compress_OPP,ACC0);
    VoC_bngtz32_r(LABEL_L3_compress_OPP,ACC1);
    VoC_sub32_dr(ACC1,MP3_CONST_0_ADDR,ACC1);//
    VoC_NOP();
    VoC_sw32_p(ACC1,REG4,DEFAULT);
LABEL_L3_compress_OPP:
    VoC_bnltz32_r(LABEL_L3_compress_OPP1,RL6);
    VoC_bngtz32_r(LABEL_L3_compress_OPP1,RL7);
    VoC_sub32_dr(RL7,MP3_CONST_0_ADDR,RL7);//
    VoC_NOP();
    VoC_sw32_p(RL7,REG5,DEFAULT);
LABEL_L3_compress_OPP1:
    VoC_movreg32(REG45,REG01,DEFAULT);
    VoC_endloop0
    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_endloop1
    /////udpdate///////////////////////////////////

    VoC_lw16i(REG0,IN_MP3_BUFFER_20);
    VoC_lw16_sd(REG4,3,DEFAULT);

    VoC_lw16i(REG7,GLOBAL_L3ENC_gr1);

    VoC_lw16i(REG2,l3_sb_sample_0_0_0_0_ADDR);//  p_sb1 = &l3_sb_sample[0][1][0][0];
    VoC_lw16i(REG3,l3_sb_sample_1_0_0_0_ADDR);//

    VoC_pop16(REG5,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);

    VoC_bnlt16_rd(LABLE_COMPILE_10,REG4,MP3_CONST_2_ADDR);//<2
    VoC_jump(LABEL_L3_compress_LOOP2);
LABLE_COMPILE_10:

    VoC_pop32(REG45,DEFAULT);
    VoC_pop16(REG6,IN_PARALLEL);

    VoC_pop32(REG23,DEFAULT);
    VoC_pop16(REG6,IN_PARALLEL);
    VoC_pop16(REG6,DEFAULT);
    VoC_pop16(REG6,DEFAULT);
    VoC_pop16(REG7,DEFAULT);
    /* apply mdct to the polyphase output */
    /*L3_format_bitstream(l3_enc,&side_info,&scalefactor, &bs,mdct_freq,NULL,0);*/
    //    clear_status();

    VoC_jal(CS_L3_format_bitstream);

    //////DMAI/////////////////////////////////////
    VoC_lw16_d(REG6,STATIC_buf_short_idx_ADDR);
    VoC_shr16_ri(REG6,-1,DEFAULT);//[byte]
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_add16_rd(REG2,REG6,MP3_CONST_15_ADDR);
    VoC_shr16_ri(REG2,4,DEFAULT);
    VoC_shr16_ri(REG2,-2,DEFAULT);
    //VoC_lw16i(REG2,160);
    VoC_lw16i_short(REG7,0,DEFAULT);          //new add by wangbin
    VoC_lw32_d(ACC1,Input_outStreamBufAddr_addr);//VoC_lw16i(ACC1_HI,0X4000);
    VoC_sw32_d(REG23,CFG_DMA_SIZE);
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_lw16i(REG4,2304);//VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_lw16i_short(REG2,2,DEFAULT);
    VoC_bne16_rd(LABLE_CONSUME_LEN,REG2,Input_Channel_Num_addr);
    VoC_shru32_ri(REG45,-1,DEFAULT);
LABLE_CONSUME_LEN:
    VoC_sw32_d(REG67,Output_output_len_addr);
    VoC_sw16_d(REG3,Output_mode_addr);
    VoC_sw32_d(REG45,Output_consumedLen_addr);
    VoC_NOP();
    VoC_return;
//         VoC_cfg(CFG_CTRL_STOP);//|CFG_CTRL_IRQ_TO_XCPU

}

void MP3_reset(void)
{

    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,DEFAULT);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,DEFAULT);
    VoC_sw16_d(REG0,CFG_DMA_WRAP);
    VoC_lw16i(STATUS,STATUS_CLR);


    VoC_lw16i(ACC0_HI,44100);
    VoC_shru32_ri(ACC0,16,DEFAULT);
    VoC_lw16i_short(REG2,0,IN_PARALLEL);

    VoC_lw16i_short(BITCACHE,0,DEFAULT);

    VoC_sw16_d(REG2,STRUCT_CONFIG_mpeg_padding_ADDR);

    VoC_bne32_rd(MP3_reset_L0,ACC0,Input_samplerate_addr);

    //44100Hz
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_lw16i(REG3,417);
    VoC_lw16i(REG7,31431);
    VoC_lw16i(REG5,-31431);
    VoC_shru32_ri(REG67,16,DEFAULT);
    VoC_shr32_ri(REG45,16,IN_PARALLEL);

    VoC_sw16_d(REG2,STRUCT_CONFIG_mpeg_samplerate_index_ADDR);// 44100=>0
    VoC_sw16_d(REG3,STRUCT_INPUT_whole_slots_per_frame_ADDR);
    VoC_sw32_d(REG67,STRUCT_INPUT_frac_slots_per_frame_ADDR);
    VoC_sw32_d(REG45,STRUCT_INPUT_slot_lag_ADDR);
    VoC_jump(MP3_reset_L2);
MP3_reset_L0:
    VoC_lw16i(ACC0_HI,32000);
    VoC_lw16i(ACC1_HI,48000);
    VoC_shru32_ri(ACC0,16,DEFAULT);
    VoC_shru32_ri(ACC1,16,IN_PARALLEL);
    VoC_bne32_rd(MP3_reset_L1,ACC1,Input_samplerate_addr);

    //4800Hz

    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_lw16_d(REG6,Input_bitrate_addr);
    VoC_shr16_ri(REG6,3,DEFAULT);
    VoC_lw16i_short(REG3,24,IN_PARALLEL);

    VoC_lw32z(REG45,DEFAULT);
    VoC_mult16_rr(REG3,REG3,REG6,IN_PARALLEL);


    VoC_sw16_d(REG1,STRUCT_CONFIG_mpeg_samplerate_index_ADDR);// 48000=>1
    VoC_sw32_d(REG45,STRUCT_INPUT_frac_slots_per_frame_ADDR);
    VoC_sw16_d(REG3,STRUCT_INPUT_whole_slots_per_frame_ADDR);

    VoC_jump(MP3_reset_L2);
MP3_reset_L1:
    VoC_bne32_rd(MP3_reset_L2,ACC0,Input_samplerate_addr);

    //32000Hz
    VoC_lw16i_short(REG1,2,DEFAULT);
    VoC_lw16_d(REG6,Input_bitrate_addr);
    VoC_shr16_ri(REG6,3,DEFAULT);
    VoC_lw16i_short(REG3,36,IN_PARALLEL);

    VoC_lw32z(REG45,DEFAULT);
    VoC_mult16_rr(REG3,REG3,REG6,IN_PARALLEL);

    VoC_sw16_d(REG1,STRUCT_CONFIG_mpeg_samplerate_index_ADDR);// 32000=>2
    VoC_sw32_d(REG45,STRUCT_INPUT_frac_slots_per_frame_ADDR);
    VoC_sw16_d(REG3,STRUCT_INPUT_whole_slots_per_frame_ADDR);
MP3_reset_L2:

    VoC_lw16i_set_inc(REG0,MP3ENC_MP3_CONST_bitrate_map_ADDR,1);
    VoC_lw16i(REG1,0);
    VoC_lw16i(REG2,32);
    VoC_lw16i(REG3,40);
    VoC_lw16i(REG4,48);
    VoC_lw16i(REG5,56);
    VoC_lw16i(REG6,64);
    VoC_lw16i(REG7,80);

    VoC_sw16inc_p(REG1,REG0,DEFAULT);
    VoC_sw16inc_p(REG2,REG0,DEFAULT);
    VoC_sw16inc_p(REG3,REG0,DEFAULT);
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    VoC_sw16inc_p(REG7,REG0,DEFAULT);


    VoC_lw16i(REG1,96);
    VoC_lw16i(REG2,112);
    VoC_lw16i(REG3,128);
    VoC_lw16i(REG4,160);
    VoC_lw16i(REG5,192);
    VoC_lw16i(REG6,224);
    VoC_lw16i(REG7,256);
    VoC_lw16i(ACC0_LO,320);
    VoC_sw16inc_p(REG1,REG0,DEFAULT);
    VoC_sw16inc_p(REG2,REG0,DEFAULT);
    VoC_sw16inc_p(REG3,REG0,DEFAULT);
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    VoC_sw16inc_p(ACC0_LO,REG0,DEFAULT);



    VoC_lw16i_set_inc(REG0,MP3ENC_MP3_CONST_bitrate_map_ADDR,1);
    VoC_lw16_d(REG1,Input_bitrate_addr);
    VoC_loop_i_short(15,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG2,REG0,DEFAULT);
    VoC_be16_rr(MP3_reset_L3,REG1,REG2);
    VoC_endloop0

MP3_reset_L3:
    VoC_lw16i(REG2,MP3ENC_MP3_CONST_bitrate_map_ADDR);
    VoC_sub16_rr(REG0,REG0,REG2,DEFAULT);
    VoC_sub16_rd(REG0,REG0,MP3_CONST_1_ADDR);
    VoC_NOP();
    VoC_sw16_d(REG0,STRUCT_CONFIG_mpeg_bitrate_index_ADDR);





    //VoC_lw16i_short(REG5,2,IN_PARALLEL);

    //VoC_sw16_d(REG3,STRUCT_CONFIG_mpeg_samplerate_index_ADDR);
    //VoC_sw16_d(REG7,STRUCT_CONFIG_mpeg_bitrate_index_ADDR);
    VoC_lw16i_set_inc(REG0,SHINE_GLOBAL_VARS_X_START_ADDR,2);


    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_lw16i(REG7,-1152);
    VoC_loop_i_short(21,DEFAULT);
    VoC_startloop0
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop0




    //VoC_sw16_d(REG5,GLOBLE_channel_ADDR);
    VoC_sw16_d(REG7,STATIC_change_ADDR);

    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16i_short(REG3,2,IN_PARALLEL);

    VoC_sw16_d(REG4,Input_reset_addr);
    VoC_sw16_d(REG4,STRUCT_CONFIG_mpeg_emph_ADDR);//config.mpeg.emph = 0;
    VoC_sw16_d(REG4,STRUCT_CONFIG_mpeg_crc_ADDR);//config.mpeg.crc  = 0;
    VoC_sw16_d(REG4,STRUCT_CONFIG_mpeg_ext_ADDR);//config.mpeg.ext  = 0;
    VoC_sw16_d(REG4,STRUCT_CONFIG_mpeg_copyright_ADDR);//config.mpeg.copyright = 0;

    VoC_sw16_d(REG2,STRUCT_CONFIG_mpeg_type_ADDR);// config.mpeg.type = 1;
    VoC_sw16_d(REG2,STRUCT_CONFIG_mpeg_mode_ADDR);//config.mpeg.mode = 1;


    VoC_sw16_d(REG2,STRUCT_CONFIG_mpeg_original_ADDR);//config.mpeg.original  = 1;
    VoC_sw16_d(REG3,STRUCT_CONFIG_mpeg_mode_ext_ADDR);//config.mpeg.mode_ext  = 2;
    VoC_sw16_d(REG3,STATIC_nGranules_ADDR);//frameData->nGranules   = 2;



    VoC_lw16i_set_inc(REG0, GLOBAL_XW_begine_ADDR,2); // MP3_CONST_XW_ADDR
    VoC_lw16i_set_inc(REG1, GLOBAL_XW_begine_ADDR+512,2);   //MP3_CONST_XW_ADDR
    VoC_loop_i(1, 2)                            //for(i=2; i-- ; )
    VoC_loop_i(0,128)                     // for(j=HAN_SIZE; j--; )
    VoC_sw32inc_p(ACC0, REG0,DEFAULT);  //  xw[i][j] = 0;
    VoC_sw32inc_p(ACC0, REG1,DEFAULT);
    VoC_endloop0
    VoC_sw32_d(ACC0, GLOBAL_OFF_ADDR);
    VoC_endloop1

    VoC_lw16i_set_inc(REG0, STRUCT_SIDEINFO_ADDR,2);
    VoC_loop_i(0,103)
    VoC_sw32inc_p(ACC0, REG0,DEFAULT);  // memset((char *)&side_info,0,sizeof(L3_side_info_t));
    VoC_endloop0

    VoC_lw16i_short(REG1,8,DEFAULT);
    //VoC_lw16i(REG0,OUTPUT_MP3_BUFFER_ADDR);

    VoC_sw32_d(ACC0,STATIC_BitCount_ADDR);
    VoC_sw32_d(ACC0,STATIC_ThisFrameSize_ADDR);
    VoC_sw32_d(ACC0,STATIC_BitsRemaining_ADDR);

    VoC_sw16_d(ACC0_LO,STATIC_buf_long_idx_ADDR);
    VoC_sw16_d(REG1,STATIC_buf_bit_idx_ADDR);
    //VoC_sw16_d(REG0,STATIC_buf_short_idx_ADDR);
    VoC_sw32_d(ACC0,STATIC_buf_chache_ADDR);



    VoC_lw16i_set_inc(REG0,l3_sb_sample_0_0_0_0_ADDR,2);
    VoC_lw16i_set_inc(REG1,l3_sb_sample_1_0_0_0_ADDR,2);
    //VoC_lw16i_set_inc(REG2,GLOBAL_MDCT_FREQ_00_ADDR,2);
    //VoC_lw16i_set_inc(REG3,GLOBAL_MDCT_FREQ_01_ADDR,2);
    VoC_loop_i(1,4)
    VoC_loop_i(0,144)
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    //VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    //VoC_sw32inc_p(ACC0,REG3,IN_PARALLEL);
    VoC_endloop0

    //VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    //VoC_sw32inc_p(ACC0,REG3,IN_PARALLEL);
    VoC_sw16_d(ACC0_LO,STATIC_ResvSize_ADDR);
    VoC_endloop1

//         VoC_sw32inc_p(ACC0,REG2,DEFAULT);
//         VoC_sw32inc_p(ACC0,REG3,IN_PARALLEL);
//         VoC_sw32inc_p(ACC0,REG2,DEFAULT);
//         VoC_sw32inc_p(ACC0,REG3,IN_PARALLEL);

    VoC_return;
}


void L3_window_filter_subband(void)
{

    VoC_lw16i(REG4,GLOBAL_OFF_ADDR);//r4: off addr;  r5:k

    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);//off[k] addr short
    VoC_push16(REG3,IN_PARALLEL);//optimize

    VoC_lw16i_set_inc(REG1,GLOBAL_XW_begine_ADDR,-2);//r1:xw  short
    // for (i=31;i>=0;i--) r1:xw  r0:buffer
    // xw[k][i+off[k]] = ((long)*(*buffer)++) << 16;
    //xw:  k*512 + i + off[k]     //xw[2][512]是16-bit
    VoC_shr16_ri(REG5,-9,DEFAULT);    //reg5:k*512
    VoC_lw16_p(REG7,REG4,IN_PARALLEL); //REG7:off[k]

    VoC_add16_rr(REG1,REG1,REG5,DEFAULT);//reg5:k*512+xw XW[K] ADDR
    VoC_push16(REG4,IN_PARALLEL);

    VoC_add16_rd(REG5,REG7,MP3_CONST_30_ADDR);//r3:off[k]+31

    VoC_add16_rr(REG1,REG5,REG1,DEFAULT);//r1:xw+k*512+off[k]+31
    VoC_movreg16(REG2,REG1,IN_PARALLEL);//R2:XW[K]

    VoC_loop_i_short(8,DEFAULT);//loop time=32  //optimize
    VoC_lw16i_short(FORMAT32,-4,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);

    VoC_movreg16(REG4,REG5,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);

    VoC_movreg16(ACC0_HI,ACC0_LO,DEFAULT);
    VoC_movreg16(ACC0_LO,ACC0_HI,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_endloop0
    //
    // for (i=7;i<=63;i++)
    //  {
    //    z[i] = Mpy_28(xw[k][(i+off[k])&511],ew[i-7]);
    //    z[512-i] = Mpy_28(xw[k][(512-i+off[k])&511],-ew[i-7]);
    //  }
    //  z[64] = Mpy_28(xw[k][(64+off[k])&511],ew[57]);
    //  z[448] = Mpy_28(xw[k][(448+off[k])&511],ew[57]);
    // r0:z[k](long)  r1:ew[k](long)  r2: xw[k](short)  r3:xw[k][(512-i+off[k])&511]
    // off[k](short) ; r4:   r5:    r6:z[512-7]   r7:
    VoC_lw16i_set_inc(REG0,GLOBAL_Z_ADDR,2);//REG0:z[I]  inc0:2
    VoC_add16_rd(REG0,REG0,MP3_CONST_14_ADDR);//z[7] addr
    VoC_lw16i_set_inc(REG1,TABLE_ew_tab_ADDR,2);//REG1:ew[k] inc1:2

    //-----loop buffer reg2-----------------
    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16i_short(WRAP2,9,IN_PARALLEL);//r2:x[k][0]
    //--------------------------------------------

    //-----loop buffer reg3-----------------------
    VoC_add16_rd(REG3,REG2,MP3_CONST_511_ADDR);//r3:x[k][512] addr

    VoC_lw16i_short(WRAP3,9,DEFAULT);
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);
    //--------------------------------------------
    VoC_lw16i(REG4,505);//r7:off[k]

    VoC_add16_rr(REG3,REG4,REG7,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);
    VoC_and16_ri(REG3,511);
    VoC_add16_rr(REG3,REG3,REG2,DEFAULT);//r3:xw[k][512-7+off[k]]
    VoC_lw16i_short(REG5,57,IN_PARALLEL);//loop time

    VoC_lw16i(REG4,996);//r7:off[k]

    VoC_add16_rr(REG6,REG4,REG0,DEFAULT);//r6:z[512-7]
    VoC_push16(REG7,IN_PARALLEL);//save off[k]

    VoC_add16_rd(REG2,REG2,MP3_CONST_7_ADDR);//r2:xw[k][7];

    VoC_lw16i_short(FORMATX,11,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);//r2:xw[k][7+off[k]]

    ////loop begin///////////////////////////////////////////

    VoC_loop_i(1,4)
    VoC_loop_r(0,REG5)
    VoC_lw32_p(REG45,REG1,DEFAULT);//ew[i]
    VoC_lw16inc_p(REG7,REG2,IN_PARALLEL);//xw[k][i+off[k]]

    VoC_shru16_ri(REG4,1,DEFAULT);//vb2
    VoC_multf32_rr(ACC1,REG7,REG5,IN_PARALLEL);//hi*hi<<4

    VoC_lw16inc_p(REG7,REG3,DEFAULT);//xw[k][512-i+off[k]]
    VoC_macX_rr(REG4,REG7,IN_PARALLEL);//hi*lo>>11     ->acc1

    /*VoC_sub32_dr(REG45,MP3_CONST_0_ADDR,REG45);//-ew*/
    VoC_sub32inc_rp(REG45,RL6,REG1,DEFAULT);//-ew

    VoC_shru16_ri(REG4,1,DEFAULT);//  -ew
    VoC_multf32_rr(ACC0,REG7,REG5,IN_PARALLEL);//->acc0

    VoC_macX_rr(REG4,REG7,DEFAULT);//->acc0
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);//   ->z[i]
    VoC_lw16i_short(REG4,2,DEFAULT);

    VoC_sw32_p(ACC0,REG6,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);
    VoC_endloop0

    VoC_lw32_p(REG45,REG1,DEFAULT);//ew[i]  //vb2
    VoC_lw16inc_p(REG7,REG2,IN_PARALLEL);//xw[k][i+off[k]]

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_multf32_rr(ACC1,REG7,REG5,IN_PARALLEL);//hi*hi<<4

    VoC_lw16inc_p(REG7,REG3,DEFAULT);//xw[k][512-i+off[k]]
    VoC_macX_rr(REG4,REG7,IN_PARALLEL);//hi*lo>>11     ->acc1

    VoC_inc_p(REG1,DEFAULT);//VoC_shru16_ri(REG4,1,DEFAULT);//
    VoC_multf32_rr(ACC0,REG7,REG5,IN_PARALLEL);//->acc0

    VoC_macX_rr(REG4,REG7,DEFAULT);//->acc0
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);//   ->z[i]

    VoC_lw16i_short(REG4,2,DEFAULT);
    VoC_lw16i_short(REG5,63,IN_PARALLEL);

    VoC_sw32_p(ACC0,REG6,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);
    VoC_endloop1
    //off[k] = (off[k] + 480) & (HAN_SIZE-1); /* offset is modulo (HAN_SIZE)*/

    VoC_lw16i(REG4,480);
    VoC_pop16(REG3,DEFAULT);
    VoC_lw16i_short(WRAP2,0,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG3,DEFAULT);
    VoC_pop16(REG6,IN_PARALLEL);

    VoC_and16_rd(REG4,MP3_CONST_511_ADDR);// off[k] = (off[k] + 480) & 511; /* offset is modulo (HAN_SIZE)*/
    //  for(i=0;i<64;i++)
    //      for(j=0,y[i]=0;j<8;j++)
    //          y[i] += z[i+(j<<6)];

    VoC_lw16i_set_inc(REG0,GLOBAL_Z_ADDR,64);//reg0:Z  LONG
    VoC_lw16i_set_inc(REG1,GLOBAL_Y_ADDR,2);//reg1:Y  LONG

    VoC_sw16_p(REG4,REG6,DEFAULT);
    VoC_movreg16(REG3,REG0,IN_PARALLEL);//optimize
    VoC_lw16i_set_inc(REG2,0,2);


    VoC_loop_i(1,64)
    VoC_loop_i_short(8,DEFAULT)
    VoC_lw32z(ACC0,IN_PARALLEL);//acc0:used for continue add   acc0=0
    VoC_startloop0
    VoC_add32inc_rp(ACC0,ACC0,REG0,DEFAULT);//y[i] += z[i+(j<<6)];
    VoC_inc_p(REG0,DEFAULT);
    VoC_endloop0

    VoC_inc_p(REG2,DEFAULT);//i++
    VoC_movreg16(REG0,REG3,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG2,IN_PARALLEL);
    VoC_endloop1

    //for (j=0;j<=14;j+=2)//j1  8times
    //      {
    //
    //          t1 = y[j] + y[32-j];
    //          t2 = Mpy_28(fll[i][j],t1);
    //          sumt1 +=t2;
    //
    //          t1 = y[j+1] + y[31-j];
    //          t2 = Mpy_28(fll[i][j+1],t1);
    //          sumt2 +=t2;
    //
    //          t1 = y[33+j]-y[63-j];
    //        t2 = Mpy_28(fll[i][j+16],t1);
    //        sumt2 +=t2;
    //
    //           t1 = y[34+j]-y[62-j];
    //         t2 = Mpy_28(fll[i][j+17],t1);
    //         sumt1 +=t2;
    //      }
    VoC_lw16i(REG7,0x800);

    VoC_pop16(REG3,DEFAULT);
    VoC_lw16i_short(FORMATX,11,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,GLOBAL_Y_ADDR,2);//reg1:Y  LONG
    VoC_lw16i_set_inc(REG1,TABLE_fl_tab_ADDR,2);//REG0:f1 long fl[32][64]

    VoC_add16_rd(REG2,REG0,MP3_CONST_64_ADDR);//y[32] addr
    VoC_add16_rd(REG4,REG3,MP3_CONST_62_ADDR);//s[31] addr

    VoC_lw16i_short(INC2,-2,DEFAULT);
    VoC_movreg16(RL6_LO,REG4,IN_PARALLEL);//s[31] addr

    VoC_push16(REG7,DEFAULT);
    VoC_lw16i_short(WRAP3,0,IN_PARALLEL);
    /////////////////////////loop begin////////////////////

    //vb2
    VoC_loop_i(1,16)

    VoC_lw32z(ACC1,DEFAULT);//sumt1=0;->ACC1
    VoC_lw32z(ACC0,IN_PARALLEL);//sumt2=0;->ACC0

    VoC_lw32inc_p(REG67,REG0,DEFAULT);//y[j]
    VoC_lw16i_short(FORMAT32,-4,IN_PARALLEL);

    VoC_loop_i_short(8, DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_startloop0
    //          t1 = y[j] + y[32-j];  **1**
    //          t2 = Mpy_28(fll[i][j],t1);
    //          sumt1 +=t2;->acc1
    VoC_add32inc_rp(REG67,REG67,REG2,DEFAULT);// y[j] + y[32-j];  //optimize
    VoC_lw32inc_p(REG45,REG1,IN_PARALLEL);//fl[i][j]

    VoC_shru16_ri(REG4,1,DEFAULT);//this is equal to u>>1
    VoC_mac32_rr(REG5,REG7,IN_PARALLEL);//hi*hi<<4

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_macX_rr(REG4,REG7,IN_PARALLEL);

    VoC_lw32inc_p(REG67,REG0,DEFAULT);//y[j+1]
    VoC_macX_rr(REG5,REG6,IN_PARALLEL);
    //          t1 = y[j+1] + y[31-j];   **2**
    //          t2 = Mpy_28(fll[i][j+1],t1);
    //          sumt2 +=t2; ->acc0

    VoC_add32inc_rp(REG67,REG67,REG2,DEFAULT);//y[31-j]
    VoC_lw32inc_p(REG45,REG1,IN_PARALLEL);//fl[i][j]

    VoC_mac32_rr(REG5,REG7,DEFAULT);//hi*hi<<4->acc1
    VoC_shru16_ri(REG4,1,IN_PARALLEL);//this is equal to u>>1


    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_macX_rr(REG5,REG6,DEFAULT);//
    VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);//y[j]

    VoC_endloop0
    ///////////Mpy_28(y[16],0x8000000);->acc1///////////////////////////////
    VoC_lw16_sd(REG5,0,DEFAULT);
    //VoC_lw16i_short(FORMATX,11,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_mac32_rr(REG7,REG5,IN_PARALLEL);//<<4
    /////////////////////////////////////////
    VoC_add16_rd(REG0,REG0,MP3_CONST_32_ADDR);//y[33] addr
    VoC_add16_rd(REG2,REG0,MP3_CONST_60_ADDR);//y[63] addr
    /////////////////////////////////////////
    VoC_loop_i_short(8, DEFAULT);
    VoC_macX_rr(REG6,REG5,IN_PARALLEL);//>>11
    VoC_startloop0
    //           t1 = y[33+j]-y[63-j];                 *3*
    //         t2 = Mpy_28(fll[i][j+16],t1);
    //         sumt2 +=t2;->acc0
    VoC_lw32inc_p(REG67,REG0,DEFAULT);//y[33+j]
    //VoC_lw16i_short(FORMATX,26,IN_PARALLEL);

    VoC_sub32inc_rp(REG67,REG67,REG2,DEFAULT);// y[33+j]-y[63-j]
    VoC_lw32inc_p(REG45,REG1,IN_PARALLEL);//fl[i][j]

    VoC_mac32_rr(REG5,REG7,DEFAULT);//hi*hi<<4
    VoC_shru16_ri(REG4,1,IN_PARALLEL);//this is equal to u>>1

    VoC_macX_rr(REG4,REG7,DEFAULT);//>>11
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_macX_rr(REG5,REG6,DEFAULT);
    VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);//y[34+j]
    //          t1 = y[34+j]-y[62-j];             *4*
    //        t2 = Mpy_28(fll[i][j+17],t1);
    //        sumt1 +=t2;->acc1

    VoC_sub32inc_rp(REG67,REG67,REG2,DEFAULT);//y[34+j]-y[62-j]
    VoC_lw32inc_p(REG45,REG1,IN_PARALLEL);//fl[i][j]

    VoC_shru16_ri(REG4,1,DEFAULT);//this is equal to u>>1
    VoC_mac32_rr(REG5,REG7,IN_PARALLEL);//hi*hi<<4->acc1

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_macX_rr(REG4,REG7,IN_PARALLEL);

    VoC_lw16i_short(REG7,0,DEFAULT);//no use
    VoC_macX_rr(REG5,REG6,IN_PARALLEL);//

    VoC_endloop0

    VoC_lw16i_set_inc(REG0,GLOBAL_Y_ADDR,2);//REG1:Y[0] ADDR

    VoC_add32_rr(ACC0,ACC1,ACC0,DEFAULT);//sumt1 + sumt2
    VoC_sub32_rr(ACC1,ACC1,ACC0,IN_PARALLEL);//sumt1 - sumt2


    VoC_lw16i_short(REG4,2,DEFAULT);
    VoC_movreg16(REG2,RL6_LO,IN_PARALLEL);

    VoC_sub16_rr(REG1,REG1,REG4,DEFAULT);//compensate the more 2
    VoC_lw16i_short(REG7,64,IN_PARALLEL);

    VoC_sw32inc_p(ACC1,REG2,DEFAULT);
    VoC_movreg16(RL6_LO,REG2,IN_PARALLEL);

    VoC_add16_rr(REG2,REG0,REG7,DEFAULT);//REG2:Y[32] ADDR
    VoC_sw32inc_p(ACC0,REG3,IN_PARALLEL);


    VoC_endloop1

    VoC_pop16(REG7,DEFAULT);

    VoC_return;

}



///////////////////////////////////////////////////////////////////////

void cii_mdct18(void)
{


    VoC_push16(REG3,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw16i(REG0,TABLE_w_tab_ADDR);//w addr
    VoC_lw16i(REG1,LOCAL_f_tab_ADDR);//f addr
    VoC_lw16i(REG2,TABLE_w2_tab_ADDR);//w addr

    VoC_lw16i_short(FORMAT32,-4,DEFAULT);
    VoC_lw16i_short(FORMATX,11,IN_PARALLEL);

    VoC_sw32_d(ACC0,TEMP_A_ADDR_9);
    VoC_sw32_d(ACC0,TEMP_B_ADDR_9);
    //    for ( p = 0; p < 4; p++ )
    VoC_lw16i_short(REG3,0,DEFAULT)//compile
    VoC_loop_i(0,5);
    //VoC_lw16i_short(REG3,0,IN_PARALLEL);//p
    //VoC_startloop0
    //g1 = Mpy_28(w[p],f[p]);

    VoC_add16_rr(REG4,REG0,REG3,DEFAULT);//w[p] addr
    VoC_add16_rr(REG6,REG1,REG3,DEFAULT);//f[p] addr

    VoC_lw32_p(REG45,REG4,DEFAULT);
    VoC_lw32_p(REG67,REG6,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);


    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);

    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_lw16i_short(REG7,34,IN_PARALLEL);

    VoC_macX_rr(REG5,REG6,DEFAULT);
    VoC_sub16_rr(REG4,REG7,REG3,IN_PARALLEL);
    //     g2 = Mpy_28(w[17 - p] , f[17 - p]);
    //VoC_sub16_dr(REG4,MP3_CONST_34_ADDR,REG3);//17-p
    VoC_add16_rr(REG4,REG0,REG4,DEFAULT);//w[17 - p] addr
    VoC_add16_rr(REG6,REG1,REG4,IN_PARALLEL);//f[17 - p] addr
    //---------------------------
    VoC_movreg32(RL6,ACC0,DEFAULT);//g1
    //---------------------------
    VoC_lw32_p(REG45,REG4,DEFAULT);
    VoC_lw32_p(REG67,REG6,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);

    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_add16_rr(REG4,REG2,REG3,IN_PARALLEL);//w2[p] addr

    VoC_macX_rr(REG5,REG6,DEFAULT);
    //ap = g1 + g2;
    VoC_lw32_p(REG45,REG4,DEFAULT);//w2[p]

    VoC_add32_rr(RL6,RL6,ACC0,DEFAULT);//ap = g1 + g2;  ->rl6
    VoC_sub32_rr(REG67,RL6,ACC0,IN_PARALLEL);//( g1 - g2 )

    //bp = Mpy_28(w2[p] , ( g1 - g2 ));
    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_macX_rr(REG5,REG6,DEFAULT);
    //VoC_be16_rd(LABEL_MDCT,REG3,MP3_CONST_8_ADDR);//compile
    VoC_bne16_rd(LABLE_COMPILE_1,REG3,MP3_CONST_8_ADDR);
    VoC_jump(LABEL_MDCT);
LABLE_COMPILE_1:
    //      g1 = Mpy_28(w[8 - p] , f[8 - p]);
    VoC_sub16_dr(REG4,MP3_CONST_16_ADDR,REG3);//8-p
    VoC_add16_rr(REG4,REG0,REG4,DEFAULT);//w[8 - p] addr
    VoC_add16_rr(REG6,REG1,REG4,IN_PARALLEL);//f[8 - p] addr
    //---------------------------
    VoC_movreg32(RL7,ACC0,DEFAULT);//bp->rl7
    //---------------------------
    VoC_lw32_p(REG45,REG4,DEFAULT);

    VoC_lw32_p(REG67,REG6,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);

    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_lw16i_short(REG7,18,IN_PARALLEL);
    VoC_macX_rr(REG5,REG6,DEFAULT);
    VoC_add16_rr(REG4,REG3,REG7,IN_PARALLEL);//9+p
    //      g2 = Mpy_28(w[9 + p] , f[9 + p]);
    //VoC_add16_rd(REG4,REG3,MP3_CONST_18_ADDR);//9+p
    VoC_add16_rr(REG4,REG0,REG4,DEFAULT);//w[9 + p] addr
    VoC_add16_rr(REG6,REG1,REG4,IN_PARALLEL);//f[9 + p] addr
    //---------------------------
    VoC_movreg32(ACC1,ACC0,DEFAULT);//g1
    //---------------------------
    VoC_lw32_p(REG45,REG4,DEFAULT);
    VoC_lw32_p(REG67,REG6,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);

    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_lw16i_short(REG7,16,IN_PARALLEL);
    VoC_macX_rr(REG5,REG6,DEFAULT);
    VoC_sub16_rr(REG4,REG7,REG3,IN_PARALLEL);
    //VoC_sub16_dr(REG4,MP3_CONST_16_ADDR,REG3);//8-p
    VoC_add16_rr(REG4,REG2,REG4,DEFAULT);//w2[8 - p] addr
    //      a8p = g1 + g2;  // a[8-p]
    VoC_add32_rr(ACC1,ACC1,ACC0,DEFAULT);//a8p->acc1
    VoC_sub32_rr(REG67,ACC1,ACC0,IN_PARALLEL);
    //      b8p = Mpy_28(w2[8 - p] , ( g1 - g2 ));  // b[8-p]
    VoC_lw32_p(REG45,REG4,DEFAULT);//w2[8 - p]
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);

    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_macX_rr(REG5,REG6,DEFAULT);
    //      a[p] = ap + a8p;
    VoC_lw16i(REG7,TEMP_A_ADDR);
    VoC_add32_rr(REG45,RL6,ACC1,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG3,IN_PARALLEL);//a[p] addr
    VoC_lw16i_short(REG6,10,DEFAULT);
    VoC_sw32_p(REG45,REG7,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG6,IN_PARALLEL);//a[5 + p] addr
    //      a[5 + p] = ap - a8p;
    VoC_sub32_rr(REG45,RL6,ACC1,DEFAULT);
    VoC_lw16i(REG6,TEMP_B_ADDR);
    VoC_sw32_p(REG45,REG7,DEFAULT);
    VoC_lw16i_short(REG7,10,IN_PARALLEL);
    //      b[p] = bp + b8p;//      b[5 + p] = bp - b8p;
    VoC_add32_rr(REG45,RL7,ACC0,DEFAULT);//bp + b8p;
    VoC_add16_rr(REG6,REG6,REG3,IN_PARALLEL);//b[p] ADDR

    VoC_sub32_rr(ACC1,RL7,ACC0,DEFAULT);//bp - b8p;
    VoC_add16_rr(REG7,REG6,REG7,IN_PARALLEL);//b[5 + p] ADDR

    VoC_sw32_p(REG45,REG6,DEFAULT);
    VoC_add16_rd(REG3,REG3,MP3_CONST_2_ADDR);
    VoC_sw32_p(ACC1,REG7,DEFAULT);
    VoC_endloop0

LABEL_MDCT:
    VoC_lw16i_set_inc(REG0,TEMP_A_ADDR,2);//r0:a
    VoC_lw16i_set_inc(REG1,TEMP_B_ADDR,2);//r1:b
    VoC_add16_rr(REG4,REG0,REG3,DEFAULT);//a[4] addr
    VoC_add16_rr(REG5,REG1,REG3,IN_PARALLEL);//b[4] addr
    VoC_lw16i_set_inc(REG2,TABLE_coef_tab_ADDR,2);//r2:coef
    VoC_sw32_p(RL6,REG4,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_sw32_p(ACC0,REG5,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_pop16(REG3,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    //    y[0] = ( a[0] + a[1] + a[2] + a[3] + a[4] )>>1;
    //    y[1] = ( b[0] + b[1] + b[2] + b[3] + b[4] )>>1;
    VoC_loop_i_short(5,DEFAULT);
    VoC_startloop0
    VoC_add32inc_rp(ACC0,ACC0,REG0,DEFAULT);
    VoC_add32inc_rp(ACC1,ACC1,REG1,IN_PARALLEL);
    VoC_endloop0
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_lw16i_short(REG4,8,IN_PARALLEL);
    VoC_shr32_ri(ACC1,1,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG4,IN_PARALLEL);//coef[1][0] ADDR
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_sw32_p(ACC1,REG3,DEFAULT);//y->1


    //    y[2] = Mpy_28(coef[1][0] , a[5]) + Mpy_28(coef[1][1] , a[6]) + Mpy_28(coef[1][2] , a[7])+ Mpy_28(coef[1][3] , a[8]);
    //    y[3] = Mpy_28(coef[1][0] , b[5]) + Mpy_28(coef[1][1] , b[6]) + Mpy_28(coef[1][2] , b[7])+ Mpy_28(coef[1][3] , b[8]) - y[1];
    VoC_loop_i(1,2)
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG2,DEFAULT);//coef[][]
    VoC_lw32inc_p(REG67,REG1,IN_PARALLEL);//b[]

    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_macX_rr(REG5,REG6,DEFAULT);
    VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);//a[]

    VoC_shru16_ri(REG6,1,DEFAULT);


    VoC_add32_rr(RL6,RL6,ACC0,DEFAULT);//y[3]
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);


    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_macX_rr(REG5,REG6,DEFAULT);

    VoC_NOP();
    VoC_add32_rr(ACC1,ACC1,ACC0,DEFAULT);//y[2]

    VoC_endloop0

    //    y[1] = y[1] - y[0];
    //    y[2] = y[2] - y[1];
    VoC_sub16_rd(REG4,REG3,MP3_CONST_2_ADDR);//y[0] addr
    VoC_lw32_p(ACC0,REG3,DEFAULT);//acc0->y[1] val

    VoC_sub32_rr(RL6,RL6,ACC0,DEFAULT);      //y[3]-y[1]->y[3]
    VoC_sub32_rp(ACC0,ACC0,REG4,IN_PARALLEL);//y[1] - y[0]->y[1];


    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);//y[2] - y[1];
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);//y[1] y->2

    VoC_sub32_rp(ACC1,ACC1,REG0,DEFAULT);//Y[2]-A[9]
    VoC_sub32_rp(RL6,RL6,REG1,DEFAULT);//Y[3]-B[9]

    VoC_sw32inc_p(ACC1,REG3,DEFAULT);//y[2]  acc1->y[2] val
    VoC_sw32_p(RL6,REG3,DEFAULT);//y[3]

    VoC_lw16i(REG0,TEMP_A_ADDR);
    VoC_lw16i(REG1,TEMP_B_ADDR);
    VoC_endloop1
    //    y[4] = Mpy_28(coef[2][0] , a[0]) + Mpy_28(coef[2][1] , a[1]) + Mpy_28(coef[2][2] , a[2])+ Mpy_28(coef[2][3] , a[3]) - a[4];
    //    y[5] = Mpy_28(coef[2][0] , b[0]) + Mpy_28(coef[2][1] , b[1]) + Mpy_28(coef[2][2] , b[2])+ Mpy_28(coef[2][3] , b[3]) - b[4] - y[3];
    //    y[3] = y[3] - y[2];
    //    y[4] = y[4] - y[3];
    //---------------------------------------------------------------------//
    //    y[6] = Mpy_28(coef[3][0] , ( a[5] - a[7] - a[8] ));
    //    y[7] = Mpy_28(coef[3][0] , ( b[5] - b[7] - b[8] )) - y[5];
    //    y[5] = y[5] - y[4];
    //    y[6] = y[6] - y[5];
    VoC_add16_rd(REG0,REG0,MP3_CONST_10_ADDR);//a[5] addr
    VoC_add16_rd(REG1,REG1,MP3_CONST_10_ADDR);//a[5] addr

    VoC_lw32_p(REG45,REG2,DEFAULT);//coef[3][0]

    VoC_lw32inc_p(RL6,REG0,DEFAULT);//a[5]
    VoC_lw32inc_p(RL7,REG1,IN_PARALLEL);//b[5]
    VoC_inc_p(REG0,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);

    VoC_sub32inc_rp(RL6,RL6,REG0,DEFAULT);
    VoC_sub32inc_rp(RL7,RL7,REG1,IN_PARALLEL);

    VoC_sub32inc_rp(REG67,RL6,REG0,DEFAULT);//a[5] - a[7] - a[8]
    VoC_sub32inc_rp(RL7,RL7,REG1,IN_PARALLEL);//b[5] - b[7] - b[8]

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);

    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_macX_rr(REG5,REG6,DEFAULT);
    VoC_movreg32(REG67,RL7,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);

    VoC_movreg32(RL6,ACC0,DEFAULT);//y[6]->rl6
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);


    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_macX_rr(REG5,REG6,DEFAULT);

    VoC_sub16_rd(REG4,REG3,MP3_CONST_2_ADDR);//y[4]  addr
    VoC_sub32_rp(ACC0,ACC0,REG3,DEFAULT);//y[7]=y[7]-y[5]

    VoC_lw32_p(RL7,REG3,DEFAULT);//y[5] val
    VoC_sub32_rp(RL7,RL7,REG4,DEFAULT);//y[5] = y[5] - y[4];


    VoC_sub32_rr(RL6,RL6,RL7,DEFAULT);//y[6] = y[6] - y[5];
    VoC_sw32inc_p(RL7,REG3,DEFAULT);//->y[5]
    VoC_add16_rd(REG2,REG2,MP3_CONST_8_ADDR);//coef[4][0] addr
    VoC_sw32inc_p(RL6,REG3,DEFAULT);//->y[6]
    VoC_sw32_p(ACC0,REG3,DEFAULT);//->y[7]
    //*******************************************************************************//

    //    y[8] = Mpy_28(coef[4][0] , a[0]) + Mpy_28(coef[4][1] , a[1]) + Mpy_28(coef[4][2] , a[2])+ Mpy_28(coef[4][3] , a[3]) + a[4];
    //    y[9] = Mpy_28(coef[4][0] , b[0]) + Mpy_28(coef[4][1] , b[1]) + Mpy_28(coef[4][2] , b[2])+ Mpy_28(coef[4][3] , b[3]) + b[4] - y[7];
    //    y[7] = y[7] - y[6];
    //    y[8] = y[8] - y[7];
    //
    //    y[10] = Mpy_28(coef[5][0] , a[5]) + Mpy_28(coef[5][1] , a[6]) + Mpy_28(coef[5][2] , a[7])+ Mpy_28(coef[5][3] , a[8]);
    //    y[11] = Mpy_28(coef[5][0] , b[5]) + Mpy_28(coef[5][1] , b[6]) + Mpy_28(coef[5][2] , b[7])+ Mpy_28(coef[5][3] , b[8]) - y[9];
    //    y[9] = y[9] - y[8];
    //    y[10] = y[10] - y[9];
    VoC_lw16i_set_inc(REG0,TEMP_A_ADDR,2);
    VoC_lw16i_set_inc(REG1,TEMP_B_ADDR,2);

    VoC_loop_i(1,2)
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG2,DEFAULT);//coef[][]
    VoC_lw32inc_p(REG67,REG1,IN_PARALLEL);//b[]

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);

    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_macX_rr(REG5,REG6,DEFAULT);
    VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);//a[]

    VoC_shru16_ri(REG6,1,DEFAULT);

    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_add32_rr(RL6,RL6,ACC0,IN_PARALLEL);//y[3]

    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_macX_rr(REG5,REG6,DEFAULT);

    VoC_NOP();
    VoC_add32_rr(ACC1,ACC1,ACC0,DEFAULT);//y[2]

    VoC_endloop0

    VoC_sub16_rd(REG4,REG3,MP3_CONST_2_ADDR);//y[0] addr
    VoC_lw32_p(ACC0,REG3,DEFAULT);//acc0->y[1] val

    VoC_sub32_rr(RL6,RL6,ACC0,DEFAULT);      //y[3]-y[1]->y[3]
    VoC_sub32_rp(ACC0,ACC0,REG4,IN_PARALLEL);//y[1] - y[0]->y[1];


    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);//y[2] - y[1];
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);//y[1]

    VoC_add32inc_rp(ACC1,ACC1,REG0,DEFAULT);//Y[2]+A[9]
    VoC_add32inc_rp(RL6,RL6,REG1,DEFAULT);//Y[3]+B[9]

    VoC_sw32inc_p(ACC1,REG3,DEFAULT);//y[2]  acc1->y[2] val
    VoC_sw32_p(RL6,REG3,DEFAULT);//y[3]

    VoC_endloop1
    //------------------------------------------------------
    //    y[12] = ( a[0] + a[2] + a[3] )/2 - a[1] - a[4];
    //    y[13] = ( b[0] + b[2] + b[3] )/2 - b[1] - b[4] - y[11];
    //    y[11] = y[11] - y[10];
    //    y[12] = y[12] - y[11];
    VoC_lw16i(REG0,TEMP_A_ADDR);
    VoC_lw16i(REG1,TEMP_B_ADDR);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);//a[0]
    VoC_lw32inc_p(ACC1,REG1,DEFAULT);

    VoC_lw32inc_p(REG45,REG0,DEFAULT);//a[1]
    VoC_lw32inc_p(REG67,REG1,IN_PARALLEL);

    VoC_add32inc_rp(ACC0,ACC0,REG0,DEFAULT);//a[2]
    VoC_add32inc_rp(ACC1,ACC1,REG1,IN_PARALLEL);

    VoC_add32inc_rp(ACC0,ACC0,REG0,DEFAULT);//a[3]
    VoC_add32inc_rp(ACC1,ACC1,REG1,IN_PARALLEL);


    VoC_movreg16(REG2,ACC0_LO,DEFAULT);
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_bnltz32_r(LABEL_ADD1,ACC0);//if acc0>0
    VoC_and16_ri(REG2,1);
    VoC_bez16_r(LABEL_ADD1,REG2);

    VoC_add32_rd(ACC0,ACC0,MP3_CONST_1_WORD32_ADDR);
LABEL_ADD1:
    VoC_movreg16(REG2,ACC1_LO,DEFAULT);
    VoC_shr32_ri(ACC1,1,DEFAULT);
    VoC_bnltz32_r(LABEL_ADD2,ACC1);//if acc0>0
    VoC_and16_ri(REG2,1);
    VoC_bez16_r(LABEL_ADD2,REG2);
    VoC_add32_rd(ACC1,ACC1,MP3_CONST_1_WORD32_ADDR);
LABEL_ADD2:

    VoC_sub32inc_rp(ACC0,ACC0,REG0,DEFAULT);
    VoC_sub32inc_rp(ACC1,ACC1,REG1,IN_PARALLEL);

    VoC_sub32_rr(ACC0,ACC0,REG45,DEFAULT);
    VoC_sub32_rr(ACC1,ACC1,REG67,IN_PARALLEL);


    VoC_sub32_rp(ACC1,ACC1,REG3,DEFAULT);//Y[13]=Y[13]-Y[11]
    VoC_sub16_rd(REG4,REG3,MP3_CONST_2_ADDR);//Y[10] ADDR
    VoC_lw32_p(RL6,REG3,DEFAULT);//Y[11] VAL
    VoC_sub32_rp(RL6,RL6,REG4,DEFAULT);//y[11] - y[10];
    VoC_sub32_rr(ACC0,ACC0,RL6,DEFAULT);//y[12] = y[12] - y[11];
    VoC_sw32inc_p(RL6,REG3,DEFAULT);//->y[11]
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);//->y[12]
    VoC_sw32_p(ACC1,REG3,DEFAULT);//->y[13]
    //**************************************************************
    //    y[14] = Mpy_28(coef[7][0] , a[5]) + Mpy_28(coef[7][1] , a[6]) + Mpy_28(coef[7][2] , a[7])+ Mpy_28(coef[7][3] , a[8]);
    //    y[15] = Mpy_28(coef[7][0] , b[5]) + Mpy_28(coef[7][1] , b[6]) + Mpy_28(coef[7][2] , b[7])+ Mpy_28(coef[7][3] , b[8]) - y[13];
    //    y[13] = y[13] - y[12];
    //    y[14] = y[14] - y[13];
    //
    //    y[16] = Mpy_28(coef[8][0] , a[0]) + Mpy_28(coef[8][1] , a[1]) + Mpy_28(coef[8][2] , a[2])+ Mpy_28(coef[8][3] , a[3]) + a[4];
    //    y[17] = Mpy_28(coef[8][0] , b[0]) + Mpy_28(coef[8][1] , b[1]) + Mpy_28(coef[8][2] , b[2])+ Mpy_28(coef[8][3] , b[3]) + b[4] - y[15];
    //    y[15] = y[15] - y[14];
    //    y[16] = y[16] - y[15];
    //    y[17] = y[17] - y[16];
    VoC_lw16i(REG2,TABLE_coef_tab_ADDR);
    VoC_add16_rd(REG2,REG2,MP3_CONST_56_ADDR);
    VoC_loop_i(1,2)
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG2,DEFAULT);//coef[][]
    VoC_lw32inc_p(REG67,REG1,IN_PARALLEL);//b[]

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);

    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_macX_rr(REG5,REG6,DEFAULT);
    VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);//a[]

    VoC_shru16_ri(REG6,1,DEFAULT);


    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_add32_rr(RL6,RL6,ACC0,IN_PARALLEL);//y[3]


    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_macX_rr(REG5,REG6,DEFAULT);
    VoC_NOP();

    VoC_add32_rr(ACC1,ACC1,ACC0,DEFAULT);//y[2]

    VoC_endloop0

    VoC_sub16_rd(REG4,REG3,MP3_CONST_2_ADDR);//y[0] addr
    VoC_lw32_p(ACC0,REG3,DEFAULT);//acc0->y[1] val

    VoC_sub32_rr(RL6,RL6,ACC0,DEFAULT);      //y[3]-y[1]->y[3]
    VoC_sub32_rp(ACC0,ACC0,REG4,IN_PARALLEL);//y[1] - y[0]->y[1];


    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);//y[2] - y[1];
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);//y[1]

    VoC_add32inc_rp(ACC1,ACC1,REG0,DEFAULT);//Y[2]+A[9]
    VoC_add32inc_rp(RL6,RL6,REG1,DEFAULT);//Y[3]+B[9]

    VoC_sw32inc_p(ACC1,REG3,DEFAULT);//y[2]  acc1->y[2] val
    VoC_sw32_p(RL6,REG3,DEFAULT);//y[3]

    VoC_lw16i(REG0,TEMP_A_ADDR);
    VoC_lw16i(REG1,TEMP_B_ADDR);

    VoC_endloop1
    VoC_sub32_rr(RL6,RL6,ACC1,DEFAULT);
    VoC_shr32_ri(RL6,-4,DEFAULT);
    VoC_lw16i_short(INC3,-2,IN_PARALLEL);
    VoC_NOP();
    VoC_sw32inc_p(RL6,REG3,DEFAULT);//[17]
    VoC_movreg16(REG2,REG3,IN_PARALLEL);//[16]  //optimize
    //-------------------------------------
    VoC_loop_i_short(8,DEFAULT)
    VoC_lw32inc_p(RL6,REG3,IN_PARALLEL);//[16]
    VoC_startloop0
    VoC_shr32_ri(RL6,-4,DEFAULT);
    VoC_lw32_p(RL7,REG3,IN_PARALLEL);//[15]

    VoC_shr32_ri(RL7,-4,DEFAULT);
    VoC_sw32_p(RL6,REG2,DEFAULT);//[16]

    VoC_sw32inc_p(RL7,REG3,DEFAULT);//[15]
    VoC_movreg16(REG2,REG3,IN_PARALLEL);//[14]

    VoC_lw32inc_p(RL6,REG3,DEFAULT);//[14]

    VoC_endloop0
    VoC_shr32_ri(RL6,-4,DEFAULT);

    VoC_return;
}

/*
* L3_mdct_sub:
* ------------
*/
void L3_mdct_sub(void)
{
    //  clear_status();
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_lw16i(REG5,GLOBAL_MDCT_FREQ_01_ADDR);

    VoC_push16(REG7,DEFAULT);//band=0;
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_push16(REG7,DEFAULT);//band=0;
    VoC_push32(REG23,IN_PARALLEL);


    VoC_push16(REG5,DEFAULT);
    VoC_push16(REG4,DEFAULT);
FOR_START1://ch
    /*mdct_enc = (long (*)[18]) mdct_freq[ch];*/
    VoC_sw16_sd(REG5,1,DEFAULT);
    VoC_lw16i_short(INC3,4,IN_PARALLEL);
    VoC_sw16_sd(REG3,3,DEFAULT);
    VoC_sw16_sd(REG4,0,DEFAULT);
    //    for(k=1; k<=17; k+=2 )  9
    //        for(band=1; band<=31; band+=2 ) 16
    //              sb_sample[ch][1][k][band] *= -1;     start addr = ch*2304 + 1152 + 64 + 2=ch*2304+1218
    VoC_add16_rd(REG3,REG3,MP3_CONST_66_ADDR);////&sb_sample[ch][1][1][1]
    VoC_movreg16(REG2,REG3,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_loop_i(1,9)
    VoC_loop_i_short(8,DEFAULT);
    VoC_sub32inc_rp(ACC1,ACC0,REG3,IN_PARALLEL);
    VoC_startloop0
    VoC_sub32_rp(RL6,ACC0,REG3,DEFAULT);
    VoC_sw32_p(ACC1,REG2,DEFAULT);
    VoC_sw32inc_p(RL6,REG3,DEFAULT);
    VoC_movreg16(REG2,REG3,IN_PARALLEL);
    VoC_sub32inc_rp(ACC1,ACC0,REG3,DEFAULT);

    VoC_endloop0
    VoC_add16_rd(REG3,REG2,MP3_CONST_64_ADDR);
    VoC_movreg16(REG2,REG3,DEFAULT);
    VoC_endloop1


LABEL_BAND:
    //for(band=0;band<32;band++ )
    //
    //        for(k=0;k<18;k++ )
    //        {
    //   reg2       mdct_in[k]    = sb_sample[ch][ 0 ][k][band];  reg1
    //   reg3       mdct_in[k+18] = sb_sample[ch][1][k][band];    reg0
    VoC_lw16_sd(REG7,3,DEFAULT);
    VoC_lw16i_short(INC1,64,IN_PARALLEL);
    VoC_lw16i_short(INC3,2,DEFAULT);
    //VoC_lw16_d(REG6,STATIC_change_ADDR);

    //VoC_mult16_rd(REG1,REG6,MP3_CONST_1152_ADDR);

    VoC_movreg16(REG0,REG7,DEFAULT);//vb2
    VoC_lw16i_short(INC0,64,IN_PARALLEL);


    VoC_add16_rd(REG1,REG7,STATIC_change_ADDR);



    VoC_lw16i_set_inc(REG2,GLOBAL_mdct_in_ADDR,2);
    VoC_add16_rd(REG3,REG2,MP3_CONST_36_ADDR);

    VoC_loop_i_short(18,DEFAULT);
    VoC_movreg32(RL7,REG23,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG1,DEFAULT);
    VoC_lw32inc_p(ACC1,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC1,REG3,DEFAULT);
    VoC_endloop0
    VoC_add16_rd(REG7,REG7,MP3_CONST_2_ADDR);

    //-------------------------------------------------------------------
    VoC_lw16i_set_inc(REG1,TABLE_win_tab_ADDR,2);
    VoC_sw16_sd(REG7,3,DEFAULT);
    VoC_movreg32(REG23,RL7,IN_PARALLEL);
    //              x2 = &mdct_in[18];
    //              x1 = &mdct_in[0];
    //for ( j = 0; j < 9; j++ )
    //              {
    //                  fast_mdct_in[j]     =  Mpy_28(win[26-j],x2[8-j]) +  Mpy_28(win[27+j],x2[9 +j]);
    //                  fast_mdct_in[9+j]   =  Mpy_28(win[j],x1[j])      +  Mpy_28(win[17-j],x1[17-j]);
    //              }
    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);
    VoC_lw16i_short(FORMATX,11,DEFAULT);
    VoC_lw16i_short(FORMAT32,-4,IN_PARALLEL);

    VoC_loop_i(0,9);//VoC_loop_i_short(9,DEFAULT)  //compile
    //  VoC_lw16i_short(REG0,0,IN_PARALLEL);
    //VoC_startloop0
    //**********1************
    VoC_sub16_dr(REG4,MP3_CONST_52_ADDR,REG0);
    VoC_add16_rr(REG4,REG1,REG4,DEFAULT);//win[26-j] addr
    VoC_sub16_dr(REG6,MP3_CONST_16_ADDR,REG0);

    VoC_add16_rr(REG6,REG3,REG6,DEFAULT);//x2[8-j] addr
    VoC_lw32_p(REG45,REG4,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_lw32_p(REG67,REG6,DEFAULT);


    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);


    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_lw16i_short(REG7,54,IN_PARALLEL);

    VoC_macX_rr(REG5,REG6,DEFAULT);
    VoC_add16_rr(REG4,REG0,REG7,IN_PARALLEL);
    //***********2******************
    //-----------------------------------------------------

    VoC_add16_rr(REG4,REG1,REG4,DEFAULT);//win[27+j] addr
    VoC_lw16i_short(REG7,18,IN_PARALLEL);
    //-----------------------------
    VoC_movreg32(RL6,ACC0,DEFAULT);//Mpy_28(win[26-j],x2[8-j])
    VoC_add16_rr(REG6,REG0,REG7,IN_PARALLEL);
    //-----------------------------

    VoC_add16_rr(REG6,REG3,REG6,DEFAULT);//x2[9+j] addr
    VoC_lw32_p(REG45,REG4,IN_PARALLEL);


    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_lw32_p(REG67,REG6,DEFAULT);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);


    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_add16_rr(REG4,REG1,REG0,IN_PARALLEL);//win[j] addr
    VoC_macX_rr(REG5,REG6,DEFAULT);
    VoC_add16_rr(REG6,REG2,REG0,IN_PARALLEL);//x1[j] addr
    //***************3*********************
    VoC_lw32_p(REG45,REG4,DEFAULT);
    VoC_movreg32(RL7,ACC0,DEFAULT);//Mpy_28(win[27+j],x2[9 +j])

    VoC_lw32_p(REG67,REG6,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);

    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_lw16i_short(REG7,34,IN_PARALLEL);
    VoC_macX_rr(REG5,REG6,DEFAULT);
    VoC_sub16_rr(REG4,REG7,REG0,IN_PARALLEL);//17-j
    //*****************4*********************

    VoC_add16_rr(REG4,REG1,REG4,DEFAULT);//win[17-j] addr
    VoC_add16_rr(REG6,REG2,REG4,IN_PARALLEL);//x1[17-j] addr

    VoC_movreg32(ACC1,ACC0,DEFAULT);//Mpy_28(win[j],x1[j])
    VoC_lw32_p(REG45,REG4,DEFAULT);


    VoC_lw32_p(REG67,REG6,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);

    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_macX_rr(REG5,REG6,DEFAULT);//Mpy_28(win[17-j],x1[17-j])
    VoC_lw16i_short(REG7,18,IN_PARALLEL);
    //-----------------------------------
    VoC_lw16i(REG4,LOCAL_f_tab_ADDR);

    VoC_add32_rr(RL6,RL6,RL7,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG0,IN_PARALLEL);//fast_mdct_in[j]


    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_shr32_ri(RL6,4,IN_PARALLEL);

    VoC_shr32_ri(ACC0,4,DEFAULT);
    VoC_add16_rr(REG6,REG4,REG7,IN_PARALLEL);//fast_mdct_in[j+9]

    VoC_sw32_p(RL6,REG4,DEFAULT);
    VoC_sw32_p(ACC0,REG6,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);//j
    VoC_endloop0
    //              cii_mdct18(fast_mdct_in,fast_mdct_out);
    VoC_lw16_sd(REG3,1,DEFAULT);
    VoC_jal(cii_mdct18);
    VoC_sw32_p(RL6,REG2,DEFAULT);//optimize

    //--------------------------------------------------------------
    VoC_lw16_sd(REG5,2,DEFAULT);
    VoC_add16_rd(REG5,REG5,MP3_CONST_1_ADDR);
    VoC_lw16_sd(REG3,1,DEFAULT);
    VoC_add16_rd(REG3,REG3,MP3_CONST_36_ADDR);
    VoC_sw16_sd(REG5,2,DEFAULT);
    VoC_sw16_sd(REG3,1,DEFAULT);
    //VoC_blt16_rd(LABEL_BAND,REG5,MP3_CONST_32_ADDR); //compile
    VoC_bnlt16_rd(LABLE_COMPILE_2,REG5,MP3_CONST_32_ADDR);
    VoC_jump(LABEL_BAND);
LABLE_COMPILE_2:
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_sub16_rd(REG5,REG3,MP3_CONST_1152_ADDR);//mdct_enc[0]

    VoC_lw16i_set_inc(REG0,TABLE_ca_encode_tab_ADDR,2);//ca addr:r0
    VoC_lw16i_set_inc(REG1,TABLE_cs_encode_tab_ADDR,2);//cs:r1
    VoC_sw16_sd(REG7,2,DEFAULT);
    VoC_lw16i_short(INC3,-2,IN_PARALLEL);
    VoC_add16_rd(REG2,REG5,MP3_CONST_36_ADDR);//mdct_enc[band+1] addr:r2
    VoC_add16_rd(REG3,REG5,MP3_CONST_34_ADDR);//mdct_enc[band][17] addr:r3


    //      for(band=0;band<31;band++ )
    //        for(k=0;k<8;k++)
    //        {
    //          bu = muls(mdct_enc[band][17-k],cs[k]) + muls(mdct_enc[band+1][k],ca[k]);
    //          bd = muls(mdct_enc[band+1][k],cs[k]) - muls(mdct_enc[band][17-k],ca[k]);
    //          mdct_enc[band][17-k] = bu;
    //          mdct_enc[band+1][k]  = bd;
    //        }


    VoC_loop_i(1,31)
    VoC_loop_i(0,8);//compile


    //********1***********
    VoC_lw32_p(REG67,REG1,DEFAULT);//cs
    VoC_lw32_p(REG45,REG3,DEFAULT);//mdct_enc[band][17-k]
    //          VoC_shru16_ri(REG6,1,IN_PARALLEL);//this is equal to u>>1


    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);


    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_macX_rr(REG5,REG6,DEFAULT);
    //muls(mdct_enc[band+1][k],ca[k])
    //********2************
    VoC_lw32_p(REG67,REG0,DEFAULT);//ca
    VoC_lw32_p(REG45,REG2,DEFAULT);//mdct_enc[band+1][k] addr
    //          VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);//muls(mdct_enc[band][17-k],cs[k])

    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);


    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_macX_rr(REG5,REG6,DEFAULT);//muls(mdct_enc[band+1][k],ca[k])

    //**********3***********Mpy_28(mdct_enc[band+1][k],cs[k])
    VoC_lw32inc_p(REG67,REG1,DEFAULT);//cs
    VoC_lw32_p(REG45,REG2,DEFAULT);//mdct_enc[band+1]
    //             VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_add32_rr(RL6,RL6,ACC0,IN_PARALLEL);//bu

    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);


    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_macX_rr(REG5,REG6,DEFAULT);

    //***************4**************
    VoC_lw32inc_p(REG67,REG0,DEFAULT);//ca
    VoC_lw32_p(REG45,REG3,DEFAULT);//mdct_enc[band][17-k]
    //          VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_movreg32(RL7,ACC0,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_macX_rr(REG4,REG7,DEFAULT);
    VoC_macX_rr(REG5,REG6,DEFAULT);
    //------------------------------------------------------
    VoC_sw32inc_p(RL6,REG3,DEFAULT);//mdct_enc[band][17-k] = bu;
    VoC_sub32_rr(RL7,RL7,ACC0,DEFAULT);
    VoC_lw16i_short(FORMAT32,-4,DEFAULT);
    VoC_sw32inc_p(RL7,REG2,DEFAULT);// //mdct_enc[band+1][k]  = bd;
    VoC_endloop0
    VoC_lw16i_set_inc(REG0,TABLE_ca_encode_tab_ADDR,2);//ca addr:r0
    VoC_lw16i_set_inc(REG1,TABLE_cs_encode_tab_ADDR,2);//cs:r1

    VoC_add16_rd(REG3,REG3,MP3_CONST_52_ADDR);
    VoC_add16_rd(REG2,REG2,MP3_CONST_20_ADDR);
    VoC_endloop1

    VoC_lw32_sd(REG23,0,DEFAULT);
    VoC_movreg16(REG3,REG2,DEFAULT);
    VoC_lw16_sd(REG4,0,IN_PARALLEL);
    VoC_sub16_rd(REG4,REG4,MP3_CONST_1_ADDR);
    VoC_lw16i(REG5,GLOBAL_MDCT_FREQ_00_ADDR);
    VoC_bltz16_r(LABLE_MP3_CHANNEL_CON,REG4);//ch=mychannel;ch>=0;
    VoC_jump(FOR_START1);
LABLE_MP3_CHANNEL_CON:
    //VoC_lw16_d(REG7,STATIC_change_ADDR);
    //VoC_mult16_rd(REG7,REG7,MP3_CONST_neg1_ADDR);
    VoC_pop16(REG4,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_pop16(REG5,DEFAULT);
    VoC_sub16_rd(REG7,REG7,STATIC_change_ADDR);

    VoC_pop16(REG4,DEFAULT);//band=0;
    VoC_pop32(REG23,IN_PARALLEL);

    VoC_pop16(REG2,DEFAULT);//band=0;
    VoC_pop16(RA,DEFAULT);

    VoC_sw16_d(REG7,STATIC_change_ADDR);//optimize
    //              VoC_counter_stats(&voc_counter_default);
    VoC_return;

}
///////////////////////////////////////////////////////////////
void inner_loop(void)//after this fun the acc1=0
{

    VoC_push16(RA,DEFAULT);//optimize
    VoC_lw16_d(REG4,STRUCT_GRINFO_quantizerStepSize_ADDR);
    VoC_bnltz32_r(LABEL_inner_loop,ACC1);//if acc1>=0 go to LABEL_inner_loop_DO
    VoC_sub16_rd(REG4,REG4,MP3_CONST_1_ADDR);


LABEL_inner_loop:
    //  max_bits:stack 32 [0]
    //  xrmax:   stack 32 [1]
    VoC_push16(REG4,DEFAULT);
LABEL_inner_loop_DO:
    //  do
    /*while(quantize(ix,ixabs,++cod_info->quantizerStepSize) > 8192);*/ /* within table range? */
    VoC_add16_rd(REG4,REG4,MP3_CONST_1_ADDR);

    VoC_lw32_sd(ACC0,1,DEFAULT);
    VoC_lw16_sd(REG3,4,IN_PARALLEL);

    VoC_sw16_sd(REG4,0,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);

    VoC_jal(quantize);  //return rl6  this fun makes acc1=0



    VoC_bgt32_rd(LABEL_inner_loop_DO,RL6,MP3_CONST_8192_WORD32_ADDR);//  change

    /*calc_runlen(ix,cod_info);*/          /* rzero,count1,big_values*/
    VoC_lw16_sd(REG3,4,DEFAULT);
    VoC_lw16i_short(INC3,-2,IN_PARALLEL);
    VoC_jal(calc_runlen);
    /*bits = count1_bitcount(ix,cod_info);*/    /* count1_table selection*/
    VoC_lw16_sd(REG2,4,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_jal(count1_bitcount);
    VoC_push32(ACC0,DEFAULT);
    /*subdivide(cod_info);*/
    VoC_jal(subdivide);
    /*bigv_tab_select(ix,cod_info);*/
    VoC_lw16_sd(REG0,4,DEFAULT);
    VoC_jal(bigv_tab_select);
    /*bits += bigv_bitcount( ix, cod_info );*/  /* bit count */
    VoC_lw16_sd(REG1,4,DEFAULT);
    VoC_jal(bigv_bitcount);
    VoC_lw32_sd(ACC1,0,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_pop32(ACC1,IN_PARALLEL);

    /*while(bits>max_bits);*/
    VoC_lw32_sd(ACC1,0,DEFAULT);
    VoC_lw16_sd(REG4,0,IN_PARALLEL);
    VoC_bgt32_rr(LABEL_inner_loop_DO,ACC0,ACC1);

    VoC_pop16(REG0,DEFAULT);
    VoC_lw16i_short(REG5,-1,IN_PARALLEL);
    VoC_pop16(RA,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_sw32_d(REG45,STRUCT_GRINFO_quantizerStepSize_ADDR);
    VoC_return;
}

/*
* outer_loop:
* -----------
*  Function: The outer iteration loop controls the masking conditions
*  of all scalefactorbands. It computes the best scalefac and
*  global gain. This module calls the inner iteration loop.
*/
void outer_loop(void)
{
    //      Accumulator[0]=xrmax;
    //      Accumulator[1]=max_bits;
    //      REGS[4].reg = gr;
    //      REGS[5].reg = ch;
    /*  gr_info *cod_info = &side_info->gr[gr].ch[ch];*/
    VoC_push16(RA,DEFAULT);
    VoC_push32(ACC0,IN_PARALLEL);//stack32 [1]

    VoC_push32(ACC1,DEFAULT);//stack32 [0]
    VoC_push16(REG0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,STRUCT_GRINFO_ADDR,2);
    //  loop ////
    VoC_loop_i_short(18,DEFAULT)//19 elements
    VoC_lw32inc_p(RL6,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(RL7,REG0,DEFAULT);

    VoC_sw32inc_p(RL6,REG1,DEFAULT);
    VoC_movreg32(RL6,RL7,IN_PARALLEL);
    VoC_endloop0
    VoC_sw32_p(RL7,REG1,DEFAULT);
    //end loop ////////
    VoC_jal(bin_search_StepSize);
    VoC_lw32_sd(ACC0,1,DEFAULT);
    VoC_lw32_sd(ACC1,0,DEFAULT);

    VoC_jal(inner_loop);//return acc0


    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_short(RL6_HI,0,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,STRUCT_GRINFO_ADDR,2);
    VoC_sw32_d(ACC0,STRUCT_GRINFO_part2_3_length_ADDR);
    VoC_sw32_d(ACC1,STRUCT_GRINFO_part2_length_ADDR);
    //cod_info->global_gain = cod_info->quantizerStepSize+210;
    VoC_lw32_d(ACC1,STRUCT_GRINFO_quantizerStepSize_ADDR);
    VoC_lw16i(RL6_LO,210);
    VoC_add32_rr(ACC1,ACC1,RL6,DEFAULT);
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_sw32_d(ACC1,STRUCT_GRINFO_global_gain_ADDR);
    /////////////////////////////////////////
    VoC_loop_i_short(18,DEFAULT)//23 elements
    VoC_lw32inc_p(RL6,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(RL7,REG1,DEFAULT);

    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_movreg32(RL6,RL7,IN_PARALLEL);
    VoC_endloop0
    VoC_sw32_p(RL7,REG0,DEFAULT);
    ///////////////////////////////////////////
    VoC_pop32(ACC1,DEFAULT);
    VoC_pop16(REG0,IN_PARALLEL);

    VoC_pop16(RA,DEFAULT);
    VoC_pop32(ACC1,DEFAULT);


    VoC_return;//return  Accumulator[0];
}

/*
* L3_iteration_loop:
* ------------------
*/
//  REGS[6].reg = channel;
//    REGS[7].reg = gr;
//    REGS[4].reg = mean_bits;
void L3_iteration_loop(void)
{
    //  clear_status();

    VoC_push16(RA,DEFAULT);
    VoC_push32(REG67,IN_PARALLEL);//optimze
    VoC_push16(REG6,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    //  main_data_begin = &side_info->main_data_begin;
    //
    //  if ( firstcall )
    //  {
    //      *main_data_begin = 0;
    //      firstcall=0;
    //  }
    VoC_bez16_d(LABEL_L3_iteration_loop_FIRSTCALL,STATIC_FIRSTCALL_ADDR);//if r4==0 go to INTERATION_LOOP_NEXT
    VoC_sw16_d(REG5,STATIC_FIRSTCALL_ADDR);
    VoC_sw16_d(REG5,STRUCT_SIDEINFO_main_data_begin_ADDR);
LABEL_L3_iteration_loop_FIRSTCALL:
    /*scalefac_band_long  = &sfBandIndex[config.mpeg.samplerate_index + (config.mpeg.type * 3)].l[0];*/
    VoC_lw16_d(REG5,STRUCT_CONFIG_mpeg_type_ADDR);//r5:config.mpeg.type

    VoC_mult16_rd(REG5,REG5,MP3_CONST_3_ADDR);//r5:(config.mpeg.type * 3)
    VoC_lw16i(REG3,37);
    VoC_add16_rd(REG5,REG5,STRUCT_CONFIG_mpeg_samplerate_index_ADDR);//r5:config.mpeg.samplerate_index + (config.mpeg.type * 3)

    VoC_mult16_rr(REG3,REG3,REG5,DEFAULT);
    VoC_lw16i(REG1,TABLE_sfBandIndex_tab_ADDR);
    VoC_add16_rr(REG1,REG1,REG3,DEFAULT);
    VoC_lw16i_set_inc(REG0,GLOBAL_MDCT_FREQ_00_ADDR,2);//gr==0
    VoC_sw16_d(REG1,STATIC_scalefac_band_long_ADDR);
    //  if(channel==0)
    VoC_bnez16_r(INTERATION_LOOP_NOIF,REG6);//if REG6!=0 go to INTERATION_LOOP_IF
    VoC_lw16i_set_inc(REG1,GLOBAL_MDCT_FREQ_01_ADDR,2);
    VoC_sw16_sd(REG0,0,DEFAULT);


    //              for (i = 0; i < 400; i++)
    //              {
    //                  int l, r;
    //                  l = (mdct_freq_org[gr][0][i]>>1);             // 0x10000000 * sqrt2*0.5 = 189812531
    //                  r = (mdct_freq_org[gr][1][i]>>1);             //sqrt2*0.5=0.7071067811865475244;
    //                  mdct_freq_org[gr][0][i] = Mpy_27((l+r),151850024);//(l+r) * (FLOAT8)(SQRT2*0.5);
    //
    //                  mdct_freq_org[gr][1][i] = Mpy_27((l-r),151850024);//(l-r) * (FLOAT8)(SQRT2*0.5);
    //
    //              }
    VoC_lw32_d(REG67,MP3_CONST_0x90d0614_ADDR);   //change
    VoC_lw16i_short(FORMAT32,-5,DEFAULT);
    VoC_lw16i_short(FORMATX,25,IN_PARALLEL);

    VoC_movreg32(REG23,REG01,DEFAULT);
    //////////////////////////////////////////
    VoC_loop_i(1,4)
    VoC_loop_i(0,100)
    VoC_lw32inc_p(REG45,REG0,DEFAULT);//mdct_freq_org[gr][0][i]
    VoC_lw32inc_p(RL7,REG1,IN_PARALLEL);//mdct_freq_org[gr][1][i]

    VoC_shr32_ri(REG45,1,DEFAULT);//l:(mdct_freq_org[gr][0][i]>>1);
    VoC_shr32_ri(RL7,1,IN_PARALLEL);//r:(mdct_freq_org[gr][1][i]>>1);

    VoC_add32_rr(REG45,REG45,RL7,DEFAULT);//(l+r)->r45
    VoC_sub32_rr(RL7,RL7,REG45,IN_PARALLEL);//(r-l)->rl7


    VoC_shru16_ri(REG4,1,DEFAULT);//this is equal to u>>1
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);

    VoC_macX_rr(REG4,REG6,DEFAULT);//hi*hi
    VoC_lw16i_short(FORMAT32,10,IN_PARALLEL);

    VoC_mac32_rr(REG4,REG7,DEFAULT);
    VoC_mac32_rr(REG5,REG6,DEFAULT);
    VoC_movreg32(REG45,RL7,IN_PARALLEL);//l-r

    VoC_lw16i_short(FORMAT32,-5,DEFAULT);
    VoC_shru16_ri(REG4,1,DEFAULT);//this is equal to u>>1

    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);//vb3
    VoC_sw32_p(ACC0,REG2,IN_PARALLEL);//mdct_freq_org[gr][0][i] = Mpy_27((l+r),151850024);
    VoC_macX_rr(REG4,REG6,DEFAULT);//hi*hi
    VoC_lw16i_short(FORMAT32,10,IN_PARALLEL);

    VoC_mac32_rr(REG4,REG7,DEFAULT);
    VoC_mac32_rr(REG5,REG6,DEFAULT);
    VoC_lw16i_short(FORMAT32,-5,DEFAULT);
    VoC_movreg32(REG23,REG01,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32_p(ACC0,REG3,DEFAULT);
    exit_on_warnings = ON;

    VoC_endloop0
    VoC_lw32z(ACC0,DEFAULT);
    VoC_endloop1
    //              for (i = 400; i < 576; i++)//176 times
    //              {
    //                  mdct_freq_org[gr][0][i]=0;
    //                  mdct_freq_org[gr][1][i]=0;
    //              }

    VoC_loop_i(0,176)
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,IN_PARALLEL);
    VoC_endloop0



    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_be16_rd(INTERATION_LOOP_XR,REG7,MP3_CONST_0x90d0614_ADDR+1);//if channel==0 ,r2==r7, go to next

INTERATION_LOOP_NOIF:

    VoC_lw16i_set_inc(REG0,GLOBAL_MDCT_FREQ_01_ADDR,2);
INTERATION_LOOP_XR:
    VoC_pop16(REG1,DEFAULT);

    //                  for (i=0, xrmax=0; i<576;i++)
    //              {
    //                    xrabs[i] = xr[i];//r0
    //                  if(xr[i]<0)
    //                     xrabs[i] =0 - xr[i];
    //                  if(xrabs[i]>xrmax)
    //                     xrmax=xrabs[i];
    //              }
    VoC_lw32z(ACC0,DEFAULT);//xrmax=0->acc0
    VoC_lw32z(ACC1,IN_PARALLEL);//acc1=0

    VoC_lw16i_set_inc(REG1,GLOBAL_RXAB_ADDR,2);//r1=xrab addr
    VoC_lw32inc_p(RL6,REG0,DEFAULT);//xr[i]
    VoC_loop_i(1,4)
    VoC_loop_i(0,144)//576=4*144

    VoC_bnltz32_r(INTERATION_LOOP_ABS,RL6);//if xr[i]>=0 go to INTERATION_LOOP_ABS
    VoC_sub32_rr(RL6,ACC1,RL6,DEFAULT);//0 - xr[i]
INTERATION_LOOP_ABS:
    VoC_bgt32_rr(INTERATION_LOOP_BIG,ACC0,RL6);
    VoC_movreg32(ACC0,RL6,DEFAULT);//   if(xrabs[i]>xrmax) xrmax=xrabs[i];
INTERATION_LOOP_BIG:
    VoC_sw32inc_p(RL6,REG1,DEFAULT);//xrabs[i] = xr[i]; or xrabs[i] =0 - xr[i];
    VoC_lw32inc_p(RL6,REG0,DEFAULT);//xr[i]
    VoC_endloop0
    VoC_lw32_sd(REG67,0,DEFAULT);
    VoC_endloop1
    VoC_sw32_d(ACC0,GLOBAL_xrmax_ADDR);

    //          cod_info = (gr_info *) &(side_info->gr[gr].ch[channel]);//gr*92+ch*46+inc(2)
    VoC_lw16i_set_inc(REG0,STRUCT_SIDEINFO_gr0_ch0_part2_3_length_ADDR,2);//&(side_info->gr[0].ch[0])
    VoC_lw16i(REG4,46);

    VoC_mult16_rr(REG1,REG6,REG4,DEFAULT);//ch*46->r1
    VoC_shr16_ri(REG4,-1,IN_PARALLEL);//46*2=92

    VoC_mult16_rr(REG5,REG7,REG4,DEFAULT);//gr*92->r5
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);//ch*46 + gr*92

    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);//&(side_info->gr[gr].ch[channel])
    VoC_pop32(REG67,DEFAULT);

    VoC_loop_i_short(3,DEFAULT)//from part2_3_length to part2_lenth
    VoC_push16(REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_endloop0

    VoC_inc_p(REG0,DEFAULT);//global_gain
    VoC_inc_p(REG0,DEFAULT);//scalefac_compress

    VoC_loop_i_short(10,DEFAULT)//from table_select to sfb_lmax
    VoC_startloop0
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_endloop0


    VoC_lw16_sd(REG0,7,DEFAULT);
    VoC_mult16_rd(REG3,REG6,MP3_CONST_1152_ADDR);//ch*1152

    VoC_lw32_d(ACC0,GLOBAL_xrmax_ADDR);//Accumulator[0]=xrmax;

    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);//ix
    VoC_lw16_sd(REG4,6,DEFAULT);/*max_bits = ResvMaxBits(side_info,mean_bits);*/

    VoC_push16(REG0,DEFAULT);
    VoC_shr16_ri(REG4,1,IN_PARALLEL);//mean_bits = mean_bits>>1;  //optimize
    VoC_bngt16_rd(RESVMAXBITS_4095,REG4,MP3_CONST_4095_ADDR);//if mean_bits<=4095
    VoC_lw16_d(REG4,MP3_CONST_4095_ADDR);
RESVMAXBITS_4095:
    VoC_lw16i_short(ACC1_HI,0,DEFAULT);
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);
    /* if(xrmax)*/
    //          cod_info->part2_3_length = outer_loop(max_bits,ix,xrabs,
    //                                              scalefactor, gr,channel/*ch*/,side_info );
    VoC_bez32_r(LABEL_L3_iteration_loop_XRMAX,ACC0);
    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_jal(outer_loop);//return acc0
LABEL_L3_iteration_loop_XRMAX:
    /*ResvAdjust(cod_info, side_info, mean_bits );*/
    /*ResvSize = (mean_bits / 2) - gi->part2_3_length;*///optimize

    VoC_movreg16(REG5,ACC0_LO,DEFAULT);//cod_info->part2_3_length
    VoC_lw16_sd(REG4,7,IN_PARALLEL);
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_sub16_rr(REG6,REG4,REG5,DEFAULT);
    VoC_lw16_sd(REG4,7,IN_PARALLEL);
    VoC_and16_ri(REG4,1);
    VoC_sw16_d(REG6,STATIC_ResvSize_ADDR);
    /* for ch */
    /*ResvFrameEnd(side_info,mean_bits);*/
    //  if((mean_bits & 1))
    //    ResvSize += 1;

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0,STRUCT_SIDEINFO_gr0_ch0_part2_3_length_ADDR,46);
    VoC_bez16_r(RESVFRAMEEND_END,REG4);//if r4==0 go to end
    //    if ( (gi->part2_3_length + stuffingBits) < 4095 )
    //        gi->part2_3_length += stuffingBits;
    VoC_add16_rp(REG2,REG4,REG0,DEFAULT);//gi->part2_3_length + stuffingBits get its low(short)

    VoC_bnlt16_rd(RESVFRAMEEND_4095,REG2,MP3_CONST_4095_ADDR);//if r2>=4095 go to RESVFRAMEEND_4095
    VoC_sw16_p(REG2,REG0,DEFAULT);//gi->part2_3_length += stuffingBits;
    VoC_jump(RESVFRAMEEND_END);
RESVFRAMEEND_4095:


    VoC_loop_i_short(4,DEFAULT);//optimize
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_startloop0

    VoC_bez16_r(RESVFRAMEEND_END,REG4);//if r4==0 go to RESVFRAMEEND_END
    // VoC_sub16_dp(REG2,MP3_CONST_4095_ADDR,REG0);//extraBits = 4095 - gi->part2_3_length;//r2
    VoC_lw16_d(REG2,MP3_CONST_4095_ADDR);
    VoC_sub16_rp(REG2,REG2,REG0,DEFAULT);
    VoC_bgt16_rr(RESVFRAMEEND_bitsThisGr,REG4,REG2,);//if extraBits =< stuffingBits go to label
    VoC_movreg16(REG2,REG4,DEFAULT);    // bitsThisGr =stuffingBits;
RESVFRAMEEND_bitsThisGr:
    VoC_add32_rp(ACC0,REG23,REG0,DEFAULT);//gi->part2_3_length += bitsThisGr;
    VoC_sub16_rr(REG4,REG4,REG2,DEFAULT);//stuffingBits -= bitsThisGr;
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);

    VoC_endloop0
    /*l3_side->resvDrain = stuffingBits;*/
    VoC_sw16_d(REG4,STRUCT_SIDEINFO_resvDrain_ADDR);

RESVFRAMEEND_END:

    VoC_pop16(REG5,DEFAULT);
    VoC_pop16(REG5,DEFAULT);

    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG7,STATIC_ResvSize_ADDR);//optimize
    //              VoC_counter_stats(&voc_counter_default);
    VoC_return;
}


void quantize(void)
{

    VoC_lw16i(REG0,TABLE_steptabi_tab_ADDR);//long
    VoC_lw16i(REG1,254);//127*2

    VoC_shr16_ri(REG4,-1,DEFAULT);//r4*2
    VoC_add16_rr(REG0,REG0,REG1,IN_PARALLEL);//scalei = steptabi[stepsize+127]; /* 2**(-stepsize/4) */

    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);//steptabi[stepsize+127] addr
    //    i = Mpy_r(xrmax,scalei)  ///////////////////////////
    VoC_movreg32(REG67,ACC0,IN_PARALLEL);//xrmax

    VoC_lw16i_short(FORMAT32,-1,DEFAULT);//will change
    VoC_lw16i_short(FORMATX,-1,IN_PARALLEL);//which will not be changed in the fun

    VoC_lw32_p(REG45,REG0,DEFAULT);//scalei
    VoC_lw16i_short(RL7_HI,0,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);//L_32 = (L_mult1 (hi1, hi2))<<1;
    VoC_multf32_rr(ACC1,REG5,REG6,IN_PARALLEL);//acc1:hi*lo

    VoC_lw16i_short(FORMAT32,14,DEFAULT);
    VoC_macX_rr(REG4,REG7,IN_PARALLEL);//acc1:hi*lo

    VoC_lw32z(RL6,DEFAULT);//RL6 =0 ;
    VoC_mac32_rr(REG4,REG6,IN_PARALLEL);//L_32_t2 = L_shr(L_32_t2,14);

    VoC_lw16i(RL7_LO,0x4000);

    VoC_add32_rr(ACC1,ACC1,RL7,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);//INC0 = 1  this will be used in the following
    VoC_shr32_ri(ACC1,15,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);//final result
    /////////////end of i = Mpy_r(xrmax,scalei)////////////////////////
    //  if( i > 165140) /* 8192**(4/3) */
    VoC_bngt32_rd(LABEL_quantize_ELSE,ACC0,MP3_CONST_0X28514_165140_ADDR);//if i<=0x28514
    VoC_lw16i(RL6_LO,16384);//max = 16384;
    VoC_return;
LABEL_quantize_ELSE:
    //  else//      for(i=0, max=0;i<samp_per_frame2;i++)

    VoC_lw32z(RL6,DEFAULT); //max =0 ;
    VoC_lw16i_short(REG0,1,IN_PARALLEL);//loop time = 1;

    VoC_lw16i_set_inc(REG2,GLOBAL_RXAB_ADDR,2);//xab addr

LABEL_quantize_LOOP576:
    VoC_push16(REG0,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);
    // ln = Mpy_r((ixabs[i]),scalei);
    VoC_lw32inc_p(REG67,REG2,DEFAULT);//ixabs[i]
    VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);//L_32 = (L_mult1 (hi1, hi2))<<1;  hi*hi<<1

    VoC_multf32_rr(ACC1,REG5,REG6,DEFAULT);//hi*lo -> acc1
    VoC_lw16i_short(RL7_HI,0,IN_PARALLEL);

    VoC_lw16i_short(FORMAT32,14,DEFAULT);
    VoC_macX_rr(REG4,REG7,IN_PARALLEL);//vb3


    VoC_lw16i(RL7_LO,0x4000);

    VoC_add32_rr(ACC1,ACC1,RL7,DEFAULT);//L_32_t2 = L_add(L_32_t2,1<<14);
    VoC_mac32_rr(REG4,REG6,IN_PARALLEL);//L_32_t2 = L_shr(L_32_t2,14);

    VoC_lw16i(RL7_LO,201);//used in the following

    VoC_shr32_ri(ACC1,15,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    /////////////end of i = Mpy_r(xrmax,scalei)////////////////////////
    VoC_bnez32_r(LABEL_quantize_ELSE_IF1,ACC0);//           if(ln == 0)
    VoC_lw32z(ACC0,DEFAULT);//              ix[i] = 0;
    VoC_jump(LABEL_quantize_ELSE_IX);

LABEL_quantize_ELSE_IF1:
    VoC_bne32_rd(LABEL_quantize_ELSE_IF2,ACC0,MP3_CONST_1_ADDR);//          else if(ln == 1)
    VoC_lw32_d(ACC0,MP3_CONST_1_WORD32_ADDR);// ix[i] = 1;
    VoC_jump(LABEL_quantize_ELSE_IX);

LABEL_quantize_ELSE_IF2:
    //if(ln<1602)  ix[i]=rqmy[ln];

    VoC_movreg16(REG0,ACC0_LO,DEFAULT);
    VoC_movreg16(REG5,ACC0_LO,IN_PARALLEL);
    VoC_bgt32_rr(LABEL_quantize_ELSE_IF3,ACC0,RL7);//else if(ln<=1601)  ix[i]=rqmy[ln];

    VoC_and16_rr(REG5,REG1,DEFAULT);
    VoC_shr16_ri(REG0,1,IN_PARALLEL);//LN/2

    VoC_lw16i(REG4,STATIC_RQ_ADDR-1);
    VoC_add16_rr(REG4,REG4,REG0,DEFAULT);//rqmy[ln] ADDR
    VoC_bnez16_r(LABEL_quantize_HIGH,REG5);
    VoC_lw16_p(REG0,REG4,DEFAULT);
    VoC_and16_ri(REG0,0x00ff);
    VoC_jump(LABEL_quantize_rq_add);
LABEL_quantize_HIGH:
    VoC_lw16_p(REG0,REG4,DEFAULT);
    VoC_and16_ri(REG0,0xff00);
    VoC_shru16_ri(REG0,8,DEFAULT);
LABEL_quantize_rq_add:
    VoC_movreg16(ACC0_LO,REG0,DEFAULT);
    VoC_jump(LABEL_quantize_ELSE_IX);

    VoC_bgt32_rr(LABEL_quantize_ELSE_IF3,ACC0,ACC1);
    VoC_lw16i(REG0,STATIC_RQ_ADDR-2);
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);
    VoC_NOP();
    VoC_lw16_p(ACC0_LO,REG0,DEFAULT);
    VoC_jump(LABEL_quantize_ELSE_IX);

LABEL_quantize_ELSE_IF3:

    VoC_blt32_rd(LABLE_COMPILE_3,ACC0,MP3_CONST_10000_WORD32_ADDR);//else if(ln<10000) /* ln < 10000 catches most values */
    VoC_jump(LABEL_quantize_ELSE_ELSE);
LABLE_COMPILE_3:
    //ix[i] = compute_rq(ln); /* quick look up method */
    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
    VoC_lw16i_short(FORMATX,13,IN_PARALLEL);
    VoC_movreg16(REG1,ACC0_LO,DEFAULT);

    VoC_lw32_d(ACC1,MP3_CONST_0X40000000_ADDR);
    VoC_lw32z(REG67,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);//value_ref = input_value<<16;
    VoC_loop_i(1,2)

    VoC_loop_i_short(31,DEFAULT)
    VoC_lw32z(REG45,IN_PARALLEL);//temp1=0

    VoC_startloop0


    VoC_or32_rr(REG67,ACC1,DEFAULT);//temp2=temp1 | (rq_r >> (i-1))
    VoC_movreg16(RL7_HI,REG6,DEFAULT);


    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG7,REG7,IN_PARALLEL);

    VoC_macX_rr(REG7,REG6,DEFAULT);
    VoC_movreg16(REG6,RL7_HI,DEFAULT);

    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_shr32_ri(ACC1,1,IN_PARALLEL);//rq_r >> (i-1)

    VoC_bgt32_rr(COMPUTE_RQ_TEMP1,ACC0,REG01);//if(temp > value_ref) go next
    VoC_movreg32(REG45,REG67,DEFAULT);//temp1 = temp2;
    VoC_bne32_rr(COMPUTE_RQ_TEMP1,ACC0,REG01);//if temp != value_ref
    VoC_jump(COMPUTE_RQ_BREAK1);
COMPUTE_RQ_TEMP1:
    VoC_movreg32(REG67,REG45,DEFAULT);
    VoC_endloop0

COMPUTE_RQ_BREAK1:
    VoC_lw16i_short(FORMATX,14,DEFAULT);
    VoC_movreg32(RL7,REG45,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG1,REG5,IN_PARALLEL);


    VoC_macX_rr(REG4,REG1,DEFAULT);//vb3


    VoC_lw32_d(ACC1,MP3_CONST_0X40000000_ADDR);

    VoC_movreg32(REG01,ACC0,DEFAULT);//change
    VoC_lw16i_short(FORMATX,13,IN_PARALLEL);
    VoC_endloop1

    /*value = (temp1+0x33e40) >>19;*/
    VoC_add32_rd(ACC0,RL7,MP3_CONST_0X33E40_212544_ADDR);//optimize
    VoC_shr32_ri(ACC0,19,DEFAULT);

    //////////////end of ix[i] = compute_rq(ln)///////////////////////////
    VoC_jump(LABEL_quantize_ELSE_IX);
LABEL_quantize_ELSE_ELSE:

    VoC_lw16i_short(REG0,-1,DEFAULT);//R0:expx
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_movreg32(ACC1,ACC0,DEFAULT);//ACC1=X

LABLE_LOG2:
    VoC_movreg32(ACC0,ACC1,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);

    VoC_shr32_rr(ACC0,REG0,DEFAULT);
    VoC_lw16i_short(REG1,13,IN_PARALLEL);//prepare for the follow process
    VoC_bgt32_rd(LABLE_LOG2,ACC0,MP3_CONST_1_WORD32_ADDR);//ix[i]=POW075(ln);
    //////////////////////end of log2//////////////////////////////////////

    VoC_movreg16(FORMAT32,REG0,DEFAULT);
    VoC_sub16_rr(REG4,REG0,REG1,IN_PARALLEL);//r4:expx

    VoC_lw16i_set_inc(REG0,TABLE_pow075_tab_tab_ADDR,2);//pow075_tab[0][0] addr


    VoC_shr16_ri(REG4,-2,DEFAULT);//expx * 4
    VoC_lw16i_short(REG5,2,IN_PARALLEL);

    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);// pow075_tab[expx][0] addr
    VoC_lw16i_short(REG4,6,IN_PARALLEL);


    VoC_add16_rr(REG1,REG0,REG5,DEFAULT);//pow075_tab[expx][1] addr
    VoC_sub32_rp(REG67,ACC1,REG0,DEFAULT);//(x-pow075_tab[expx][0])

    VoC_add16_rr(REG7,REG0,REG4,DEFAULT);//pow075_tab[expx+1][1] addr
    VoC_lw32_p(RL7,REG1,DEFAULT);//pow075_tab[expx][1]

    VoC_sub32_pr(REG45,REG7,RL7,DEFAULT);//(pow075_tab[expx+1][1]-pow075_tab[expx][1])

    //both the results from upper are short ,so the mult is :lo*lo
    VoC_multf32_rr(ACC0,REG4,REG6,DEFAULT);//((int)((temp1*temp2))>>(expx+13))
    VoC_NOP();
    VoC_add32_rp(ACC0,ACC0,REG0,DEFAULT);//temp = pow075_tab[expx][0]+ ((int)((temp1*temp2))>>(expx+13));
    VoC_NOP();
    /////////end of ix[i]=POW075(ln);//////////////////////////////////////
LABEL_quantize_ELSE_IX:
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_bngt32_rr(LABEL_quantize_ELSE_END_IF,ACC0,RL6);//if(max < ix[i])        max = ix[i];
    VoC_movreg32(RL6,ACC0,DEFAULT);
LABEL_quantize_ELSE_END_IF:
    VoC_pop16(REG0,DEFAULT);
    VoC_pop32(REG45,DEFAULT);

    VoC_inc_p(REG0,DEFAULT);//time++
    VoC_lw16i_short(FORMATX,-1,IN_PARALLEL);
    VoC_bgt16_rd(LABLE_COMPILE_4,REG0,MP3_CONST_576_ADDR);//if r0<=576
    VoC_jump(LABEL_quantize_LOOP576);
LABLE_COMPILE_4:
    VoC_NOP();

    VoC_return;//return REG_L6;
}
/////////////////////////////////////////////////////////////////
/*
* calc_runlen:
* ------------
* Function: Calculation of rzero, count1, big_values
* (Partitions ix into big values, quadruples and zeros).
input:  all in the rams
optput: none
optimized by : zouying 2006/9/20

  r0: 1
  r1: cod_info->count1
  r2: 2
  r3: ix_addr
  r4: i
  r5: 4
  acc0 ,REG67:ix[i]

    */
void calc_runlen(void)
{




    VoC_lw16i(REG4,1150);
    VoC_shr16_ri(REG4,1,DEFAULT);//575
    VoC_add16_rr(REG3,REG3,REG4,IN_PARALLEL);//point to the last member

    // for ( i = 575; i > 0; i -= 2 )
    //     if (ix[i] || ix[i-1])
    //      break;
    VoC_loop_i(1,16)//0-575
    VoC_loop_i_short(18,DEFAULT)//loop time = 288
    VoC_lw16i_short(REG2,2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG67,REG3,DEFAULT);
    VoC_lw32inc_p(ACC0,REG3,DEFAULT);
    VoC_bnez32_r(LABLE_calc_runlen_i,REG67);
    VoC_bnez32_r(LABLE_calc_runlen_i,ACC0);
    VoC_sub16_rr(REG4,REG4,REG2,DEFAULT);
    VoC_endloop0
    VoC_lw16i_short(REG5,4,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    VoC_endloop1

LABLE_calc_runlen_i:

    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);//r3+4
    VoC_lw16i_short(REG0,1,IN_PARALLEL);

    ///* cod_info->count1 = 0 ;*/
    ////  for ( ; i > 2; i -= 4 )
    ////    if ( ix[i-1] <= 1 && ix[i-2] <= 1 && ix[i-3] <= 1 && ix[i] <= 1 )
    ////      cod_info->count1++;
    ////    else
    ////      break;

    VoC_bngt16_rr(LABLE_calc_runlen_end,REG4,REG2);//if r4<=2 go to the end
LABLE_calc_runlen_count:
    VoC_lw32inc_p(REG67,REG3,DEFAULT);
    VoC_bgt32_rd(LABLE_calc_runlen_end,REG67,MP3_CONST_1_ADDR);
    VoC_lw32inc_p(REG67,REG3,DEFAULT);
    VoC_bgt32_rd(LABLE_calc_runlen_end,REG67,MP3_CONST_1_ADDR);
    VoC_lw32inc_p(REG67,REG3,DEFAULT);
    VoC_bgt32_rd(LABLE_calc_runlen_end,REG67,MP3_CONST_1_ADDR);
    VoC_lw32inc_p(REG67,REG3,DEFAULT);
    VoC_bgt32_rd(LABLE_calc_runlen_end,REG67,MP3_CONST_1_ADDR);
    VoC_add16_rr(REG1,REG1,REG0,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG5,IN_PARALLEL);//re represent i
    VoC_bgt16_rd(LABLE_calc_runlen_count,REG4,MP3_CONST_2_ADDR);//if r4>2 go back
LABLE_calc_runlen_end:
    VoC_add16_rr(REG4,REG4,REG0,DEFAULT);
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_sw16_d(REG1,STRUCT_GRINFO_count1_ADDR);
    VoC_sw16_d(REG4,STRUCT_GRINFO_big_values_ADDR); /* cod_info->big_values = (i+1)>>1;*/

    VoC_return;
}
//////////////////////////////////////////////////////////////////////
/*
* count1_bitcount:
* ----------------
* Determines the number of bits to encode the quadruples.
input:
ix:      ram
cod_info:ram
output:
none


    used regs:
    r0:  signbits
    r1:  ht[32].hlen[p] addr
    r2:  ix addr
    r3:  p
    r4:  v
    r5:  w
    r6:  x
    r7:  y
    rl7_o: ht[33].hlen value
    acc0:sum0
    acc1:sum1
    optimized by: zouying  2006/9/19
*/
void count1_bitcount(void)
{


    VoC_lw16_d(REG3,STRUCT_GRINFO_big_values_ADDR);//i  value
    VoC_lw16_d(REG6,STRUCT_GRINFO_count1_ADDR);//k   value

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_shr16_ri(REG3,-2,DEFAULT);//i<<1
    VoC_lw16i_short(REG0,0,IN_PARALLEL);//signbits=0
    VoC_bez16_r(LABEL_COUNT1_BITCOUNT_0,REG6);


    VoC_lw16i(REG1,TABLE_ht_tab_ADDR);//ht addr


    VoC_add16_rd(REG4,REG1,MP3_CONST_164_ADDR);//ht[32].hlen addr
    VoC_add16_rd(REG7,REG4,MP3_CONST_5_ADDR);//ht[33].hlen addr

    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);//ix[i] addr
    VoC_lw16i_short(INC0,1,IN_PARALLEL);

    VoC_lw16_p(REG1,REG4,DEFAULT);//ht[32].hlen value

    VoC_lw16_p(REG7,REG7,DEFAULT);//ht[33].hlen  value
    VoC_movreg16(RL7_LO,REG7,DEFAULT);//

    ////  for(i=cod_info->big_values<<1, k=0; k<cod_info->count1; i+=4, k++)
    VoC_loop_r(0,REG6)//k
    ////    v = ix[i];//this show i must be short,then k must be short too.
    ////    w = ix[i+1];
    ////    x = ix[i+2];
    ////    y = ix[i+3];
    ////    p = v + (w<<1) + (x<<2) + (y<<3);
    VoC_lw16inc_p(REG4,REG2,DEFAULT);//    v = ix[i];

    VoC_lw16inc_p(REG5,REG2,DEFAULT);//    w = ix[i+1];


    VoC_lw16inc_p(REG6,REG2,DEFAULT);//    x = ix[i+2];
    VoC_shr16_ri(REG5,-1,IN_PARALLEL);//(w<<1)

    VoC_lw16inc_p(REG7,REG2,DEFAULT);//    y = ix[i+3];
    VoC_shr16_ri(REG6,-2,IN_PARALLEL);//(x<<2)

    VoC_shr16_ri(REG7,-3,DEFAULT);//(y<<3)
    VoC_add16_rr(REG3,REG4,REG5,IN_PARALLEL);

    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);//    p = v + (w<<1) + (x<<2) + (y<<3);  short
    ////    if(v!=0) signbits++;
    ////    if(w!=0) signbits++;
    ////    if(x!=0) signbits++;
    ////    if(y!=0) signbits++;
    VoC_bez16_r(LABEL_COUNT1_BITCOUNT_SIGN1,REG4);//if v==0 go to LABEL_COUNT1_BITCOUNT_SIGN
    VoC_inc_p(REG0,DEFAULT);
LABEL_COUNT1_BITCOUNT_SIGN1:
    VoC_bez16_r(LABEL_COUNT1_BITCOUNT_SIGN2,REG5);//if w==0 go to LABEL_COUNT1_BITCOUNT_SIGN
    VoC_inc_p(REG0,DEFAULT);
LABEL_COUNT1_BITCOUNT_SIGN2:
    VoC_bez16_r(LABEL_COUNT1_BITCOUNT_SIGN3,REG6);//if x==0 go to LABEL_COUNT1_BITCOUNT_SIGN
    VoC_inc_p(REG0,DEFAULT);
LABEL_COUNT1_BITCOUNT_SIGN3:
    VoC_bez16_r(LABEL_COUNT1_BITCOUNT_SIGN4,REG7);//if y==0 go to LABEL_COUNT1_BITCOUNT_SIGN
    VoC_inc_p(REG0,DEFAULT);
LABEL_COUNT1_BITCOUNT_SIGN4:
    ////    sum0 += ht[32].hlen[p];  //r3:p
    ////    sum1 += ht[33].hlen[p];
    //*************************
    VoC_movreg16(REG7,REG3,DEFAULT);
    VoC_shr16_ri(REG3,1,IN_PARALLEL);

    VoC_add16_rr(REG5,REG3,REG1,DEFAULT);//ht[32].hlen[p] addr
    VoC_movreg16(REG6,RL7_LO,IN_PARALLEL);

    VoC_add16_rr(REG6,REG6,REG3,DEFAULT);//ht[33].hlen[p] addr
    VoC_movreg16(REG4,ACC0_LO,IN_PARALLEL);

    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_lw16_p(REG6,REG6,DEFAULT);

    VoC_and16_ri(REG7,1);


    VoC_bnez16_r(LABEL_HIGH,REG7);
    VoC_and16_ri(REG5,0x00ff);
    VoC_and16_ri(REG6,0x00ff);
    VoC_jump(LABEL_ADD_HT);
LABEL_HIGH:
    VoC_and16_ri(REG5,0xff00);
    VoC_and16_ri(REG6,0xff00);
    VoC_shru16_ri(REG5,8,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);
    //*************************

LABEL_ADD_HT:
    VoC_movreg16(REG7,ACC1_LO,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);//    sum0 += ht[32].hlen[p];

    VoC_add16_rr(REG7,REG7,REG6,DEFAULT);//    sum1 += ht[33].hlen[p];

    VoC_movreg16(ACC1_LO,REG7,DEFAULT);
    VoC_movreg16(ACC0_LO,REG4,IN_PARALLEL);
    VoC_endloop0
    // if(sum0<sum1)  // return the smaller one
    //  {
    //    cod_info->count1table_select = 0;
    //    return (sum0+signbits);
    //  }
    //  else
    //  {
    //    cod_info->count1table_select = 1;
    //    return (sum1+signbits);
    //  }
LABEL_COUNT1_BITCOUNT_0:
    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_bgt32_rr(LABEL_COUNT1_BITCOUNT_SUM0,ACC1,ACC0);//if     sum0<sum1 go to LABEL_COUNT1_BITCOUNT_SUM0
    VoC_add32_rr(ACC0,ACC1,REG01,DEFAULT);
    VoC_sw16_d(REG5,STRUCT_GRINFO_count1table_select_ADDR);
    VoC_return;
LABEL_COUNT1_BITCOUNT_SUM0:
    VoC_add32_rr(ACC0,ACC0,REG01,DEFAULT);
    VoC_sw16_d(REG1,STRUCT_GRINFO_count1table_select_ADDR);
    VoC_return;
}

/*
* subdivide:
* ----------
* presumable subdivides the bigvalue region which will use separate Huffman tables.
input:
all in the ram
output:
none
optimized by: zouying  2006/9/20
r0:subdv_table addr
r1:scalefac_band_long addr
r2:index
r3:temp
r4:bigvalues_region
r5:thiscount
r6:scfb_anz
r7:1
*/
void subdivide(void)
{

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16_d(REG4,STRUCT_GRINFO_big_values_ADDR);//R4:cod_info->big_values



    VoC_bnez16_r(LABEL_SUBDIV_ELSE,REG4);//if cod_info->big_values!=0 go to else
    VoC_sw32_d(ACC0,STRUCT_GRINFO_region0_count_ADDR);//    cod_info->region0_count = 0;
    VoC_sw32_d(ACC0,STRUCT_GRINFO_region1_count_ADDR);//    cod_info->region1_count = 0;
    VoC_return;
LABEL_SUBDIV_ELSE:
    VoC_lw16i(REG0,TABLE_subdv_table_tab_ADDR);//addr
    VoC_lw16d_set_inc(REG1,STATIC_scalefac_band_long_ADDR,1);

    VoC_shr16_ri(REG4,-1,DEFAULT);//bigvalues_region = 2 * cod_info->big_values;
    VoC_lw16i_short(REG6,-1,IN_PARALLEL);//scfb_anz

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_movreg16(REG3,REG1,IN_PARALLEL);

LABEL_SUBDIV_WHILE1:
    VoC_lw16inc_p(REG5,REG1,DEFAULT);//while ( scalefac_band_long[scfb_anz] < bigvalues_region )
    VoC_add16_rr(REG6,REG7,REG6,IN_PARALLEL);////        scfb_anz++ -> r6
    VoC_bgt16_rr(LABEL_SUBDIV_WHILE1,REG4,REG5);//if bigvalues_region>scalefac_band_long[scfb_anz]


    VoC_movreg16(REG1,REG3,DEFAULT);//scalefac_band_long addr
    VoC_add16_rr(REG0,REG0,REG6,IN_PARALLEL);//subdv_table[scfb_anz].region0_count addr

    VoC_add16_rr(REG3,REG1,REG7,DEFAULT);//scalefac_band_long[1] ADDR
    VoC_lw16_p(REG5,REG0,DEFAULT);//subdv_table[scfb_anz].region0_count= thiscount
    VoC_and16_ri(REG5,0xff);//r5:thiscount
    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);//scalefac_band_long[index] ADDR

LABEL_SUBDIV_WHILE2:
    VoC_bez16_r(LABEL_SUBDIV_NEXT,REG5);////      while ( thiscount && (scalefac_band_long[index] > bigvalues_region) )
    VoC_lw16_p(REG3,REG3,DEFAULT);//scalefac_band_long[index]
    VoC_bngt16_rr(LABEL_SUBDIV_NEXT,REG3,REG4);//if bigvalues_region>=scalefac_band_long[index]
    VoC_sub16_rr(REG5,REG5,REG7,DEFAULT);//thiscount--;
    VoC_jump(LABEL_SUBDIV_WHILE2);
LABEL_SUBDIV_NEXT:

    VoC_sw16_d(REG5,STRUCT_GRINFO_region0_count_ADDR);//cod_info->region0_count = thiscount;

    VoC_lw16_p(REG5,REG0,DEFAULT);
    VoC_movreg16(REG0,REG5,IN_PARALLEL);//r0:cod_info->region0_count

    VoC_and16_ri(REG5,0xff00);//cod_info->region1_count = (subdv_table[scfb_anz]&0xff00)>>8;
    VoC_shru16_ri(REG5,8,DEFAULT);

    VoC_add16_rr(REG2,REG5,REG0,DEFAULT);
    VoC_add16_rd(REG2,REG2,MP3_CONST_2_ADDR);/*index = cod_info->region0_count + cod_info->region1_count + 2;*/


    VoC_add16_rr(REG3,REG1,REG2,DEFAULT);//scalefac_band_long[index] ADDR
LABEL_SUBDIV_WHILE3:
    VoC_bez16_r(LABEL_SUBDIV_NEXT1,REG5);////      while ( thiscount && (scalefac_band_long[index] > bigvalues_region) )
    VoC_lw16_p(REG3,REG3,DEFAULT);//scalefac_band_long[index]
    VoC_bngt16_rr(LABEL_SUBDIV_NEXT1,REG3,REG4);//if bigvalues_region>=scalefac_band_long[index]
    VoC_sub16_rr(REG5,REG5,REG7,DEFAULT);//thiscount--;
    VoC_sub16_rr(REG2,REG2,REG7,IN_PARALLEL);//index--
    VoC_jump(LABEL_SUBDIV_WHILE3);
LABEL_SUBDIV_NEXT1:


    VoC_sw16_d(REG5,STRUCT_GRINFO_region1_count_ADDR);/*cod_info->region1_count = thiscount;*/

    VoC_add16_rr(REG2,REG0,REG7,DEFAULT);//cod_info->region0_count+1
    VoC_add16_rr(REG5,REG5,REG7,IN_PARALLEL);//cod_info->region1_count+1

    VoC_add16_rr(REG6,REG1,REG2,DEFAULT);//scalefac_band_long[cod_info->region0_count+1] addr


    VoC_sw16_d(REG4,STRUCT_GRINFO_address3_ADDR);//     cod_info->address3 = bigvalues_region;

    VoC_add16_rr(REG1,REG6,REG5,DEFAULT);//scalefac_band_long[cod_info->region0_count+cod_info->region1_count + 2 ] addr
    VoC_lw16_p(REG7,REG6,DEFAULT);//STRUCT_GRINFO_address1_ADDR

    VoC_lw16_p(REG1,REG1,DEFAULT);


    VoC_sw16_d(REG7,STRUCT_GRINFO_address1_ADDR);
    VoC_sw16_d(REG1,STRUCT_GRINFO_address2_ADDR);//    cod_info->address2 = scalefac_band_long[cod_info->region0_count+cod_info->region1_count + 2 ];

    VoC_return;
}
//////////////////////////////////////////////////////////////////////////////////

/*
* bigv_tab_select:
* ----------------
* Function: Select huffman code tables for bigvalues regions
input:
ix:       ram
cod_infor:ram
output:
none
written by: zouying
*/
void bigv_tab_select(void)
{


    VoC_push16(REG0,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);


    VoC_push16(RA,DEFAULT);// change positions   optimize

    VoC_lw16_d(REG6,STRUCT_GRINFO_address1_ADDR);
    VoC_sw16_d(REG5,STRUCT_GRINFO_table_select_ADDR);
    VoC_sw16_d(REG5,STRUCT_GRINFO_table_select1_ADDR);
    VoC_sw16_d(REG5,STRUCT_GRINFO_table_select2_ADDR);
    VoC_bngtz16_r(LABLE_bigv_tab_select_IF1,REG6);//r6<=0 go
    VoC_jal(new_choose_table);
    VoC_sw16_d(REG0,STRUCT_GRINFO_table_select_ADDR);
LABLE_bigv_tab_select_IF1:
    //      if ( cod_info->address2 > cod_info->address1 )
    //          cod_info->table_select[1] = new_choose_table( ix, cod_info->address1, cod_info->address2 );
    VoC_lw16_d(REG5,STRUCT_GRINFO_address1_ADDR);
    VoC_lw16_d(REG6,STRUCT_GRINFO_address2_ADDR);
    VoC_bngt16_rr(LABLE_bigv_tab_select_IF2,REG6,REG5);//if r5<=r6 go
    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_jal(new_choose_table);
    VoC_sw16_d(REG0,STRUCT_GRINFO_table_select1_ADDR);
LABLE_bigv_tab_select_IF2:
    //      if ( cod_info->big_values<<1 > cod_info->address2 )
    //          cod_info->table_select[2] = new_choose_table( ix, cod_info->address2, cod_info->big_values<<1 );
    VoC_lw16_d(REG5,STRUCT_GRINFO_address2_ADDR);
    VoC_lw16_d(REG6,STRUCT_GRINFO_big_values_ADDR);
    VoC_shr16_ri(REG6,-1,DEFAULT);//cod_info->big_values<<1
    VoC_lw16_sd(REG0,1,IN_PARALLEL);
    VoC_bngt16_rr(LABLE_bigv_tab_select_END,REG6,REG5);//if r5<=r6 go
    VoC_jal(new_choose_table);
    VoC_sw16_d(REG0,STRUCT_GRINFO_table_select2_ADDR);
LABLE_bigv_tab_select_END:
    //           cod_info->table_select[0]=RAM_Y[(STRUCT_GRINFO_table_select_ADDR-RAM_Y_BEGIN_ADDR)/2];
    //       cod_info->table_select[1]=RAM_Y[(STRUCT_GRINFO_table_select1_ADDR-RAM_Y_BEGIN_ADDR)/2];
    //       cod_info->table_select[2]=RAM_Y[(STRUCT_GRINFO_table_select2_ADDR-RAM_Y_BEGIN_ADDR)/2];

    VoC_pop16(RA,DEFAULT);
    VoC_pop16(REG0,DEFAULT);
    VoC_return;
}

/*
* new_choose_table:
* -----------------
* Choose the Huffman table that will encode ix[begin..end] with
* the fewest bits.
* Note: This code contains knowledge about the sizes and characteristics
* of the Huffman tables as defined in the IS (Table B.7), and will not work
* with any arbitrary tables.
* input :
begin:  REGS[5]
end:     REGS[6]
ix :     stack
output:
none
unused regs:
none
opimized by:zouying  2006/9/19

  *
*/

void new_choose_table(void)
{

    VoC_push16(REG0,DEFAULT);
    VoC_movreg16(REG7,REG5,IN_PARALLEL);//r7 :begin

    VoC_shr16_ri(REG5,-1,DEFAULT);//begin*2
    VoC_push16(RA,IN_PARALLEL);//change positions  optimize

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG5,IN_PARALLEL);//ix[begin] addr

    VoC_sub16_rr(REG4,REG6,REG7,DEFAULT);//r4:end - start
    VoC_lw32z(ACC0,IN_PARALLEL);//max  should be short

    VoC_lw32inc_p(ACC1,REG0,DEFAULT);// ix[i]
    VoC_shr16_ri(REG4,1,IN_PARALLEL);//times = (end - start)/2

    VoC_loop_r(0,REG4)//  for(i=begin;i<end;i++)
    VoC_bngt32_rr(LABEL_IMAX1,ACC1,ACC0);//    if(max >= ix[i]) go next
    VoC_movreg32(ACC0,ACC1,DEFAULT);     //      max = ix[i];
LABEL_IMAX1:
    VoC_lw32inc_p(ACC1,REG0,DEFAULT);
    VoC_bngt32_rr(LABEL_IMAX2,ACC1,ACC0);//    if(max >= ix[i]) go next
    VoC_movreg32(ACC0,ACC1,DEFAULT);     //      max = ix[i];
LABEL_IMAX2:
    VoC_lw32inc_p(ACC1,REG0,DEFAULT);
    VoC_endloop0                         //  max = Accumulator[0];

    ////////end of  ix_max///////////////////////////
    VoC_lw32z(REG01,DEFAULT);//  choice[0] = 0; choice[1] = 0;
    VoC_bez32_r(LABEL_new_choose_table_ENDALL,ACC0);//  if(!max)//    return 0;


    VoC_push32(REG67,DEFAULT);
    VoC_movreg16(REG4,ACC0_LO,IN_PARALLEL);//r4:max  short
    //  if(max<15)  is  as follows: //
    VoC_lw16i_set_inc(REG3,TABLE_ht_tab_ADDR,5);//r3:ht
    VoC_blt16_rd(LABLE_COMPILE_6,REG4,MP3_CONST_15_ADDR);//if max>=15
    VoC_jump(LABEL_new_choose_table_ELSE);
LABLE_COMPILE_6:
    VoC_lw16i_set_inc(REG1,0x00ff,2);
    VoC_lw16inc_p(REG2,REG3,DEFAULT);//ht[i].xlen
    VoC_and16_rr(REG2,REG1,DEFAULT);

    VoC_loop_i_short(14,DEFAULT);                            //    for (i=0;i<14;i++)
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_startloop0
    VoC_bngt16_rr(LABEL_new_choose_table_COM1,REG2,REG4);//IF ht[i].xlen <= max  go to next
    VoC_jump(LABEL_new_choose_table_BREAK1);            // if ( ht[i].xlen > max )  choice[0] = i;break;
LABEL_new_choose_table_COM1:
    VoC_lw16inc_p(REG2,REG3,DEFAULT);//ht[i].xlen
    VoC_and16_rr(REG2,REG1,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG5,IN_PARALLEL);//R1=R1+1
    VoC_endloop0

LABEL_new_choose_table_BREAK1:
    /* REGS[4].reg = table;REGS[5].reg = start;REGS[6].reg = end;*/
    /*sum[0] = count_bit( ix, begin, end, choice[0] );*/
    VoC_movreg16(REG4,REG0,DEFAULT);
    VoC_push16(REG0,IN_PARALLEL);

    VoC_lw16_sd(REG1,2,DEFAULT);
    VoC_jal(count_bit);//the result is in r2=sum[0]


    VoC_push16(REG2,DEFAULT);

    VoC_lw16_sd(REG0,1,DEFAULT);//switch (choice[0])
    VoC_lw32_sd(REG67,0,IN_PARALLEL);
    VoC_movreg16(REG5,REG7,DEFAULT);//begin
    //      case 2:
    //        sum[1] = count_bit( ix, begin, end, 3 );
    //        if ( sum[1] <= sum[0] )
    //          choice[0] = 3;
    //        break;
    VoC_bne16_rd(LABEL_new_choose_table_SWITCH5,REG0,MP3_CONST_2_ADDR);//if choice[0]!=2 go to
    VoC_lw16i_short(REG4,3,DEFAULT);//table=3;
    VoC_lw16_sd(REG1,3,IN_PARALLEL);

    VoC_jal(count_bit);//reg2: sum1

    VoC_lw16_sd(REG0,1,DEFAULT);//recover reg0
    VoC_lw16_sd(REG3,0,DEFAULT);//r3:sum0

    VoC_bgt16_rr(LABEL_new_choose_table_SWITCH_END,REG2,REG3);//if sum[1] > sum[0]
    VoC_lw16i_short(REG0,3,DEFAULT);
    VoC_jump(LABEL_new_choose_table_SWITCH_END);

LABEL_new_choose_table_SWITCH5:
    //      case 5:
    //        sum[1] = count_bit( ix, begin, end, 6 );
    //        if ( sum[1] <= sum[0] )
    //          choice[0] = 6;
    //        break;
    VoC_bne16_rd(LABEL_new_choose_table_SWITCH7,REG0,MP3_CONST_5_ADDR);//if choice[0]!=5 go to
    VoC_lw16i_short(REG4,6,DEFAULT);//table=6;
    VoC_lw16_sd(REG1,3,DEFAULT);

    VoC_jal(count_bit);//r2:sum1

    VoC_lw16_sd(REG0,1,DEFAULT);//recover choice[0]
    VoC_lw16_sd(REG3,0,DEFAULT);//r2:sum0
    VoC_bgt16_rr(LABEL_new_choose_table_SWITCH_END,REG2,REG3);//if   sum[1] > sum[0]
    VoC_lw16i_short(REG0,6,DEFAULT);
    VoC_jump(LABEL_new_choose_table_SWITCH_END);
LABEL_new_choose_table_SWITCH7:
    //      case 7:
    //        sum[1] = count_bit( ix, begin, end, 8 );
    //        if ( sum[1] <= sum[0] )
    //        {
    //          choice[0] = 8;
    //          sum[0] = sum[1];
    //        }
    //        sum[1] = count_bit( ix, begin, end, 9 );
    //        if ( sum[1] <= sum[0] )
    //          choice[0] = 9;
    //        break;
    VoC_bne16_rd(LABEL_new_choose_table_SWITCH10,REG0,MP3_CONST_7_ADDR);//if choice[0]!=5 go to
    VoC_lw16i_short(REG4,8,DEFAULT);//table=8;
    VoC_lw16_sd(REG1,3,IN_PARALLEL);

    VoC_jal(count_bit);//reg2:sum1

    VoC_lw16_sd(REG0,1,DEFAULT);//recover choice[0]
    VoC_lw16_sd(REG3,0,DEFAULT);//r3:sum0
    VoC_bgt16_rr(LABEL_new_choose_table_SWITCH7_IF1,REG2,REG3);//if      sum[1] > sum[0]
    VoC_lw16i_short(REG0,8,DEFAULT);
    VoC_sw16_sd(REG2,0,DEFAULT);//r2:sum0
    VoC_sw16_sd(REG0,1,DEFAULT);//update r0:choice[0]
LABEL_new_choose_table_SWITCH7_IF1:
    VoC_lw32_sd(REG67,0,DEFAULT);
    VoC_lw16i_short(REG4,9,IN_PARALLEL);//table=9;

    VoC_lw16_sd(REG1,3,DEFAULT);


    VoC_jal(count_bit);

    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_lw16_sd(REG3,0,DEFAULT);//r2:sum0
    VoC_bgt16_rr(LABEL_new_choose_table_SWITCH_END,REG2,REG3);//if   sum[1] > sum[0]
    VoC_lw16i_short(REG0,9,DEFAULT);
    VoC_jump(LABEL_new_choose_table_SWITCH_END);

LABEL_new_choose_table_SWITCH10:
    //      case 10:
    //        sum[1] = count_bit( ix, begin, end, 11 );
    //        if ( sum[1] <= sum[0] )
    //        {
    //          choice[0] = 11;
    //          sum[0] = sum[1];
    //        }
    //        sum[1] = count_bit( ix, begin, end, 12 );
    //        if ( sum[1] <= sum[0] )
    //          choice[0] = 12;
    //        break;
    VoC_bne16_rd(LABEL_new_choose_table_SWITCH13,REG0,MP3_CONST_10_ADDR);//if choice[0]!=5 go to
    VoC_lw16i_short(REG4,11,DEFAULT);//table=11;
    VoC_lw16_sd(REG1,3,IN_PARALLEL);

    VoC_jal(count_bit);

    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_lw16_sd(REG3,0,DEFAULT);//r2:sum0

    VoC_bgt16_rr(LABEL_new_choose_table_SWITCH10_IF1,REG2,REG3);//if     sum[1] > sum[0]
    VoC_lw16i_short(REG0,11,DEFAULT);
    VoC_sw16_sd(REG2,0,DEFAULT);//r2:sum0
    VoC_sw16_sd(REG0,1,DEFAULT);//update r0:choice[0]
LABEL_new_choose_table_SWITCH10_IF1:
    VoC_lw32_sd(REG67,0,DEFAULT);
    VoC_lw16i_short(REG4,12,IN_PARALLEL);//table=12;

    VoC_lw16_sd(REG1,3,DEFAULT);


    VoC_jal(count_bit);

    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_lw16_sd(REG3,0,DEFAULT);//r2:sum0
    VoC_bgt16_rr(LABEL_new_choose_table_SWITCH_END,REG2,REG3);//if   sum[1] > sum[0]
    VoC_lw16i_short(REG0,12,DEFAULT);
    VoC_jump(LABEL_new_choose_table_SWITCH_END);

LABEL_new_choose_table_SWITCH13:
    //      case 13:
    //        sum[1] = count_bit( ix, begin, end, 15 );
    //        if ( sum[1] <= sum[0] )
    //          choice[0] = 15;
    //        break;
    VoC_bne16_rd(LABEL_new_choose_table_SWITCH_END,REG0,MP3_CONST_13_ADDR);//if choice[0]!=5 go to
    VoC_lw16i_short(REG4,15,DEFAULT);//table=15;
    VoC_lw16_sd(REG1,3,IN_PARALLEL);

    VoC_jal(count_bit);

    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_lw16_sd(REG3,0,DEFAULT);//r2:sum0
    VoC_bgt16_rr(LABEL_new_choose_table_SWITCH_END,REG2,REG3);//if   sum[1] > sum[0]
    VoC_lw16i_short(REG0,15,DEFAULT);

LABEL_new_choose_table_SWITCH_END:
    VoC_pop16(REG2,DEFAULT);
    VoC_pop16(REG2,DEFAULT);
    VoC_jump(LABEL_new_choose_table_END);

LABEL_new_choose_table_ELSE:
    //   else

    VoC_lw16i_short(REG0,15,DEFAULT);//reg5:i
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_lw16i_short(REG6,77,IN_PARALLEL);//chaged

    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);//ht[15].linmax addr
    VoC_sub16_rr(REG4,REG4,REG0,DEFAULT);/*max -= 15;*/

    //      for(i=15;i<24;i++)//9 times
    //          if(ht[i].linmax>=max)
    //          {
    //              choice[0] = i;
    //              break;
    //          }
    VoC_loop_i_short(9,DEFAULT);
    VoC_lw16inc_p(REG2,REG3,IN_PARALLEL);
    VoC_startloop0
    VoC_bgt16_rr(LABEL_new_choose_table_ELSE_FOR1_IF,REG4,REG2);//if max>ht[i].linmax
    VoC_jump(LABEL_new_choose_table_ELSE_FOR2);
LABEL_new_choose_table_ELSE_FOR1_IF:
    VoC_lw16inc_p(REG2,REG3,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG7,IN_PARALLEL);
    VoC_endloop0

LABEL_new_choose_table_ELSE_FOR2:
    //          for(i=24;i<32;i++)
    //              if(ht[i].linmax>=max)
    //              {
    //                  choice[1] = i;
    //                  break;
    //              }
    VoC_lw16i_set_inc(REG2,TABLE_ht_tab_ADDR,5);
    //VoC_lw16i_short(REG6,122,IN_PARALLEL);




    VoC_add16_rd(REG2,REG2,MP3_CONST_122_ADDR);//ht[24].linmax addr

    VoC_lw16i_short(REG1,24,DEFAULT);
    VoC_loop_i_short(8,DEFAULT);
    VoC_lw16inc_p(REG3,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_bgt16_rr(LABEL_new_choose_table_ELSE_FOR2_IF,REG4,REG3);//if max>ht[i].linmax
    VoC_jump(LABEL_new_choose_table_ELSE_FOR3);
LABEL_new_choose_table_ELSE_FOR2_IF:
    VoC_lw16inc_p(REG3,REG2,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);
    VoC_endloop0
LABEL_new_choose_table_ELSE_FOR3:

    VoC_push16(REG0,DEFAULT);
    VoC_movreg16(REG4,REG1,IN_PARALLEL);

    VoC_lw16_sd(REG1,2,DEFAULT);
    VoC_lw32_sd(REG67,0,IN_PARALLEL);


    VoC_push16(REG4,DEFAULT);

    VoC_jal(count_bit);/* sum[1] = count_bit(ix,begin,end,choice[0]);*/

    VoC_push16(REG2,DEFAULT);
    VoC_lw16_sd(REG4,2,DEFAULT);
    VoC_lw32_sd(REG67,0,DEFAULT);
    VoC_lw16_sd(REG1,4,IN_PARALLEL);

    /*sum[0] = count_bit(ix,begin,end,choice[1]);*/
    VoC_jal(count_bit);//r2:sum0
    VoC_lw16_sd(REG3,0,DEFAULT);//r3:sum1
    //           if (sum[1]<sum[0])
    //               choice[0] = choice[1];
    VoC_lw16_sd(REG0,2,DEFAULT);
    VoC_bngt16_rr(LABEL_new_choose_table_ELSE_END,REG2,REG3);
    VoC_lw16_sd(REG0,1,DEFAULT);
LABEL_new_choose_table_ELSE_END:
    VoC_pop16(REG3,DEFAULT);
    VoC_pop16(REG4,DEFAULT);
    VoC_pop16(REG4,DEFAULT);

LABEL_new_choose_table_END:
    //   return choice[0];
    //   the result is in reg0
    VoC_pop32(REG67,DEFAULT);
LABEL_new_choose_table_ENDALL:

    VoC_pop16(RA,DEFAULT);
    VoC_pop16(REG1,DEFAULT);
    VoC_return;//  return REGS[0].reg;

}
////////////////////////////////////////////////////////////////
/*
* bigv_bitcount:
* --------------
* Function: Count the number of bits necessary to code the bigvalues region.
input:
ix :  stack
gi :  ram_x
output:
bits: acc0
unused regs:
RL6_HI
optimized by :zouying  2006/9/19
*/
void bigv_bitcount(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);//bits=0

    VoC_lw16i_set_inc(REG0,STRUCT_GRINFO_table_select_ADDR,2);//R0:gi->table_select[0] ADDR
    VoC_lw16i_set_inc(REG2,STRUCT_GRINFO_sfb_lmax_ADDR,2);//R1:temp[0] ADDR


    VoC_lw32z(ACC1,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);

    VoC_movreg16(RL6_LO,REG0,DEFAULT);
    VoC_lw16i_short(ACC1_LO,3,IN_PARALLEL);//ACC1:LOOP TIME

    VoC_movreg16(RL7_HI,REG1,DEFAULT);

LABEL_bigv_bitcount_FOR:        //for (i=0;i<3;i++)
    VoC_bez16_r(LABEL_bigv_bitcount_IF,REG4);//if gi->table_select[i]==0 go
    VoC_lw16inc_p(REG7,REG2,DEFAULT);//short

    VoC_lw16_p(REG6,REG2,DEFAULT);
    VoC_movreg16(REG1,RL7_HI,DEFAULT);

    VoC_jal(count_bit);//r2:bits

    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_add32_rr(ACC0,ACC0,REG23,DEFAULT);//bits = bits + my;
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
LABEL_bigv_bitcount_IF:
    VoC_movreg16(REG0,RL6_LO,DEFAULT);//R0:gi->table_select ADDR

    VoC_sub32_rd(ACC1,ACC1,MP3_CONST_1_ADDR);
    VoC_add16_rd(REG2,REG0,MP3_CONST_18_ADDR);//temp addr

    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_movreg16(RL6_LO,REG0,IN_PARALLEL);

    VoC_bnez32_r(LABEL_bigv_bitcount_FOR,ACC1);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;// return Accumulator[0];

}

/*
* count_bit:
* ----------
* Function: Count the number of bits necessary to code the subregion.
//count_bit// : which is a critical function
input:      ix_addr:reg1
end  :reg6
start:reg7
table:reg4

  output:     sum:reg2

    unused regs:acc0 acc1 rl6 rl7_hi
    written by: zouying
    optimized : zouying    2006/9/15
*/
void count_bit(void)
//unsigned int start,
//unsigned int end,
//unsigned int table
{

    VoC_lw16i_short(REG2,0,DEFAULT);//  sum=0
    VoC_lw16i_short(REG5,5,IN_PARALLEL);
    VoC_bez16_r(LABEL_COUNT_BIT_END,REG4);// if table==0 return

    VoC_mult16_rr(REG4,REG4,REG5,DEFAULT);//table*5 -> r4
    VoC_sub16_rr(REG6,REG6,REG7,IN_PARALLEL);//end-start

    VoC_lw16i_set_inc(REG0,TABLE_ht_tab_ADDR,1);//ht addr

    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);//ht[table] addr
    VoC_shr16_ri(REG6,1,IN_PARALLEL);//(end-start)/2=loop times ->reg6

    VoC_lw16i(REG3,0xff00);

    VoC_lw16_p(REG4,REG0,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);//ht[table].linbits addr

    VoC_and16_rr(REG4,REG3,DEFAULT);//get the high 8 bits
    VoC_shr16_ri(REG7,-1,IN_PARALLEL);//start*2

    VoC_lw16_p(REG3,REG0,DEFAULT);//ht[table].linbits ->r3
    VoC_shru16_ri(REG4,8,IN_PARALLEL);//ylen -> r4

    VoC_add16_rd(REG0,REG0,MP3_CONST_3_ADDR);//ht[table].hlen addr

    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);//ix[start] addr
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_lw16_p(REG0,REG0,DEFAULT);//ht[table].hlen value
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    VoC_movreg16(RL7_LO,REG0,DEFAULT);



    VoC_loop_r(1,REG6)             //   for(i=start;i<end;i+=2)
    VoC_loop_i_short(2,DEFAULT)//       for (j=0;j<2;j++)
    VoC_lw16i_short(REG5,0,IN_PARALLEL);//r5:j
    VoC_startloop0

    VoC_lw16inc_p(REG6,REG1,DEFAULT);//r6:    x = ix[i+j];
    VoC_bez16_r(LABEL_COUNT_BIT_IF1,REG6);// if (x)
    VoC_inc_p(REG2,DEFAULT);              // sum++;
LABEL_COUNT_BIT_IF1:
    VoC_bngt16_rd(LABEL_COUNT_BIT_IF2,REG6,MP3_CONST_14_ADDR);//<=14    if(x>14 && table>15)
    VoC_bngt16_rd(LABEL_COUNT_BIT_IF2,REG4,MP3_CONST_15_ADDR);//<=15    { x = 15;
    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);// sum += linbits;           sum += linbits;
    VoC_lw16i_short(REG6,15,IN_PARALLEL);// r6:x                    }
LABEL_COUNT_BIT_IF2:
    VoC_bnez16_r(LABEL_COUNT_BIT_IF3,REG5);//     if (j==0)
    VoC_mult16_rr(REG0,REG6,REG4,DEFAULT);//          y = x*ylen;
    VoC_jump(LABEL_COUNT_BIT_IF31);       //       else
LABEL_COUNT_BIT_IF3:
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT); //       y = y + x;    r0:y
LABEL_COUNT_BIT_IF31:
    VoC_lw16i_short(REG5,1,DEFAULT);      //       j+=1           r5:j
    VoC_lw16i_short(REG7,1,IN_PARALLEL);
    VoC_endloop0


    VoC_movreg16(REG6,REG0,DEFAULT);//     save y
    VoC_shr16_ri(REG0,1,IN_PARALLEL);//    y/2

    VoC_and16_rr(REG7,REG6,DEFAULT); //    confirm the y is odd or even
    VoC_movreg16(REG5,RL7_LO,IN_PARALLEL);
    VoC_add16_rr(REG6,REG5,REG0,DEFAULT);//h->hlen[y] addr

    VoC_bez16_r(LABEL_ADD_HT1,REG7); // odd
    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_and16_ri(REG6,0xff00);//get the high 8 bits
    VoC_shru16_ri(REG6,8,DEFAULT);
    VoC_jump(LABEL_HIGHI11);
LABEL_ADD_HT1://even low
    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_and16_ri(REG6,0x00ff);//get the low 8 bits

LABEL_HIGHI11:
    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);//          sum += h->hlen[y];
    VoC_endloop1

LABEL_COUNT_BIT_END:
    VoC_return;

}


/*
* bin_search_StepSize:
* --------------------
* Succesive approximation approach to obtaining a initial quantizer
* step size.
* The following optional code written by Seymour Shlien
* will speed up the outer_loop code which is called
* by iteration_loop. When BIN_SEARCH is defined, the
* outer_loop function precedes the call to the function inner_loop
* with a call to bin_search gain defined below, which
* returns a good starting quantizerStepSize.

  stact16           stack32

    r2:last            acc0:xrmax
    r3:next            acc1:desired_rate
    r4:times_loop      r01:top_bot

      */


void bin_search_StepSize(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG1,0,DEFAULT);//bot

    VoC_lw16i_short(REG0,-120,IN_PARALLEL);//top

    VoC_movreg16(REG3,REG0,DEFAULT);//next = -120;
    VoC_lw16i_short(REG4,0,IN_PARALLEL);//times_loop

    VoC_push16(REG2,DEFAULT);

    VoC_push16(REG3,DEFAULT);
    VoC_push32(ACC1,IN_PARALLEL);

    VoC_push16(REG4,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);

    //   do
LABEL_bin_search_StepSize_DO:
    //    {
    VoC_sw32_sd(REG01,0,DEFAULT);

    VoC_sw16_sd(REG3,2,DEFAULT);//last = next
    VoC_add16_rr(REG3,REG0,REG1,IN_PARALLEL);/*next = (top+bot) >> 1;*/

    VoC_shr16_ri(REG3,1,DEFAULT);/*next = (top+bot) >> 1;*/
    VoC_lw32_sd(ACC0,3,DEFAULT);//quantize: acc0:xrmax

    VoC_movreg16(REG4,REG3,DEFAULT);//quantize: r4:stepsize
    VoC_sw16_sd(REG3,1,IN_PARALLEL);

    VoC_lw16_sd(REG3,6,DEFAULT);//quantize :  r3:ix_addr
    VoC_lw16i_short(INC3,2,IN_PARALLEL);

    VoC_jal(quantize);//return bits to rl6



    VoC_bgt32_rd(LABEL_bin_search_StepSize_ELSE,RL6,MP3_CONST_8192_WORD32_ADDR);//        if(quantize(ix,ixabs,next) <= 8192)

    /*calc_runlen(ix,cod_info);*/            /* rzero,count1,big_values */
    VoC_lw16_sd(REG3,6,DEFAULT);
    VoC_lw16i_short(INC3,-2,IN_PARALLEL);
    VoC_jal(calc_runlen);

    /*bit = count1_bitcount(ix, cod_info);*/ /* count1_table selection */
    VoC_lw16_sd(REG2,6,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_jal(count1_bitcount);//return acc0
    VoC_push32(ACC0,DEFAULT);

    /*subdivide(cod_info);   */              /* bigvalues sfb division */
    VoC_jal(subdivide);

    /*bigv_tab_select(ix,cod_info); */       /* codebook selection */
    VoC_lw16_sd(REG0,6,DEFAULT);
    VoC_jal(bigv_tab_select);

    /*bit += bigv_bitcount(ix,cod_info); */  /* bit count */
    VoC_lw16_sd(REG1,6,DEFAULT);
    VoC_jal(bigv_bitcount);
    VoC_pop32(ACC1,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);//bit->acc0

    VoC_jump(LABEL_bin_search_StepSize_IFEND);

LABEL_bin_search_StepSize_ELSE:
    //        else
    //            bit = 100000;  /* fail */
    VoC_lw32_d(ACC0,MP3_CONST_0X186A0_100000_ADDR);
    ///////////////////////////////////////
LABEL_bin_search_StepSize_IFEND:
    //        if (bit>desired_rate)
    //            top = next;
    //        else
    //            bot = next;

    VoC_lw32_sd(ACC1,1,DEFAULT);
    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_bngt32_rr(LABEL_bin_search_StepSize_IFEND11,ACC0,ACC1);//acc0: bit  acc1:desired_rate
    VoC_lw16_sd(REG0,1,DEFAULT);//bot=next
    VoC_jump(LABEL_bin_search_StepSize_IFEND11_NEXT);
LABEL_bin_search_StepSize_IFEND11:
    VoC_lw16_sd(REG1,1,DEFAULT);//top=next
LABEL_bin_search_StepSize_IFEND11_NEXT:
    ///////////////////////////////////////
    /*times_loop++;*/
    VoC_lw16_sd(REG4,0,DEFAULT);
    VoC_add16_rd(REG4,REG4,MP3_CONST_1_ADDR);
    /*if(times_loop>8) break;*/
    VoC_bngt16_rd(LABEL_bin_search_StepSize_IFEND2,REG4,MP3_CONST_8_ADDR);
    VoC_jump(LABEL_bin_search_StepSize_BREAK);
LABEL_bin_search_StepSize_IFEND2:
    VoC_sw16_sd(REG4,0,DEFAULT);
    //    }
    /*while((bit!=desired_rate) && abs(last-next)>1);*/
    VoC_lw16_sd(REG3,1,DEFAULT);

    VoC_be32_rr(LABEL_bin_search_StepSize_BREAK,ACC1,ACC0);
    VoC_lw16_sd(REG2,2,DEFAULT);
    VoC_sub16_rr(REG5,REG2,REG3,DEFAULT);

    //LABEL_bin_search_StepSize_ABS:
    VoC_bngt16_rd(LABLE_COMPILE_8,REG5,MP3_CONST_1_ADDR);//>1
    VoC_jump(LABEL_bin_search_StepSize_DO);
LABLE_COMPILE_8:
    VoC_bnlt16_rd(LABEL_bin_search_StepSize_BREAK,REG5,MP3_CONST_neg1_ADDR);//<-1  //optimize
    VoC_jump(LABEL_bin_search_StepSize_DO);
LABEL_bin_search_StepSize_BREAK:
    /*return next;*/

    VoC_pop16(REG4,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);

    VoC_pop16(REG3,DEFAULT);
    VoC_pop32(ACC1,IN_PARALLEL);

    VoC_pop16(REG2,DEFAULT);

    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG3,STRUCT_GRINFO_quantizerStepSize_ADDR);
    VoC_return;
}

//////////////////////bitsream format///////////////////////////////////////////////
void CS_L3_format_bitstream(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_bnez16_d(L3_format_bitstream_Initial_done,STATIC_PartHoldersInitialized_ADDR)
    VoC_lw16i_set_inc(REG0,STRUCT_frameData_ADDR,2);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_loop_i_short(8,DEFAULT)
    VoC_startloop0
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop0
    VoC_lw16i_set_inc(REG0,STRUCT_frameData_STRUCT_header_ADDR+1,2);
    VoC_lw16i(REG2,STRUCT_frameData_STRUCT_header_element_ADDR);
    VoC_lw16i(REG3,STRUCT_frameData_STRUCT_frameSI_element_ADDR);
    VoC_sw16inc_p(REG2,REG0,DEFAULT);
    VoC_sw16inc_p(REG3,REG0,DEFAULT);
    VoC_lw16i(REG2,STRUCT_frameData_STRUCT_channelSI_ch0_element_ADDR);
    VoC_lw16i(REG3,STRUCT_frameData_STRUCT_channelSI_ch1_element_ADDR);
    VoC_sw16inc_p(REG2,REG0,DEFAULT);
    VoC_sw16inc_p(REG3,REG0,DEFAULT);

    VoC_lw16i(REG2,STRUCT_frameData_STRUCT_spectrumSI_gr0ch0_element_ADDR);
    VoC_lw16i(REG3,STRUCT_frameData_STRUCT_spectrumSI_gr0ch1_element_ADDR);
    VoC_sw16inc_p(REG2,REG0,DEFAULT);
    VoC_sw16inc_p(REG3,REG0,DEFAULT);

    VoC_lw16i(REG2,STRUCT_frameData_STRUCT_spectrumSI_gr1ch0_element_ADDR);
    VoC_lw16i(REG3,STRUCT_frameData_STRUCT_spectrumSI_gr1ch1_element_ADDR);
    VoC_sw16inc_p(REG2,REG0,DEFAULT);
    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_sw16inc_p(REG3,REG0,DEFAULT);

    VoC_sw16_d(REG2,STATIC_PartHoldersInitialized_ADDR);
L3_format_bitstream_Initial_done:

    VoC_jal(CS_encodeSideInfo);

    VoC_jal(CS_encodeMainData);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return

}
///////////////////////////////////////////////////////////////
void CS_encodeMainData(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);//gr


    VoC_lw16i_set_inc(REG2, GLOBAL_L3ENC_gr0, 2);
    VoC_lw16i_set_inc(REG1, STRUCT_SIDEINFO_gr0_ch0_part2_3_length_ADDR, 2);
encodeMainData_gr_loop_begin:
    VoC_lw16i_short(REG6, 0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_push16(REG6,DEFAULT);
    exit_on_warnings=ON;
encodeMainData_ch_loop_begin:

    VoC_push16(REG6,DEFAULT);

    //In the shine, The scale facter alway zero, and  the gi->scalefac_compress alway be 0 too, So the
    //slen1 and slen2 are zero, This means the code of above in useless!!


    VoC_push16(REG2,DEFAULT);
    VoC_push16(REG1,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0, STRUCT_frameData_STRUCT_codedData_gr0ch0_element_ADDR, 2);
    VoC_jal(CS_Huffmancodebits);  //   Huffmancodebits( &codedDataPH[gr][ch], ix, gi );

    VoC_movreg16(REG2,REG3,DEFAULT);
    VoC_lw16i_set_inc(REG3, STRUCT_frameData_STRUCT_codedData_gr0ch0_element_ADDR, 2);

    VoC_lw16i_short(INC3,2,DEFAULT);

    VoC_jal(CS_BF_BitstreamFrame);


    VoC_pop16(REG1,DEFAULT);
    VoC_pop16(REG2,DEFAULT);

    VoC_add16_rd(REG2, REG2,MP3_CONST_1152_ADDR);
    VoC_add16_rd(REG1, REG1,MP3_CONST_46_ADDR);
    VoC_pop16(REG6,DEFAULT);

    VoC_add16_rd(REG6, REG6,MP3_CONST_1_ADDR);

    VoC_blt16_rd(encodeMainData_ch_loop_begin, REG6,MP3_CONST_2_ADDR)  /* for ch */
    VoC_pop16(REG6,DEFAULT);
    VoC_add16_rd(REG6, REG6,MP3_CONST_1_ADDR);
    VoC_lw16i_set_inc(REG2, GLOBAL_L3ENC_gr1, 2);
    VoC_blt16_rd(encodeMainData_gr_loop_begin, REG6,MP3_CONST_2_ADDR) /* for gr */
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return

}



void CS_encodeSideInfo(void)
{
    VoC_lw32_d(REG01,STRUCT_frameData_STRUCT_header_ADDR);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_lw16i(REG6,0xfff);
    VoC_lw16i_short(REG7,12,DEFAULT);
    VoC_movreg32(RL7,REG01,DEFAULT);
    // headerPH->part->nrEntries = 0;
    VoC_sw32inc_p(REG67,REG1,DEFAULT);//  headerPH = BF_addEntry( headerPH, 0xfff,                          12 );
    VoC_lw16_d(REG6,STRUCT_CONFIG_mpeg_type_ADDR);
    VoC_lw16i_short(REG7,1,DEFAULT);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,2,IN_PARALLEL);
    VoC_sw32inc_p(REG67,REG1,DEFAULT);//    headerPH = BF_addEntry( headerPH, config.mpeg.type,               1 );
    /* HEADER HARDCODED SHOULDN`T BE THIS WAY ! */
    VoC_lw16_d(REG6,STRUCT_CONFIG_mpeg_crc_ADDR);
    VoC_sw32inc_p(REG45,REG1,DEFAULT);//   headerPH = BF_addEntry( headerPH, 1/*config.mpeg.layr*/,          2 );
    VoC_not16_r(REG6,DEFAULT);
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_lw16_d(REG4,STRUCT_CONFIG_mpeg_bitrate_index_ADDR);
    VoC_sw32inc_p(REG67,REG1,DEFAULT);//  headerPH = BF_addEntry( headerPH, !config.mpeg.crc,               1 );
    VoC_lw16i_short(REG5,4,IN_PARALLEL);
    VoC_lw16_d(REG6,STRUCT_CONFIG_mpeg_samplerate_index_ADDR);
    VoC_sw32inc_p(REG45,REG1,DEFAULT);//headerPH = BF_addEntry( headerPH, config.mpeg.bitrate_index,      4 );
    VoC_lw16i_short(REG7,2,IN_PARALLEL);
    VoC_lw16_d(REG4,STRUCT_CONFIG_mpeg_padding_ADDR);
    VoC_sw32inc_p(REG67,REG1,DEFAULT);   //   headerPH = BF_addEntry( headerPH, config.mpeg.samplerate_index,   2 );
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_lw16_d(REG6,STRUCT_CONFIG_mpeg_ext_ADDR);
    VoC_sw32inc_p(REG45,REG1,DEFAULT);//headerPH = BF_addEntry( headerPH, config.mpeg.padding,            1 );
    VoC_lw16i_short(REG7,1,IN_PARALLEL);
    VoC_lw16_d(REG4,STRUCT_CONFIG_mpeg_mode_ADDR);
    VoC_sw32inc_p(REG67,REG1,DEFAULT); // headerPH = BF_addEntry( headerPH, config.mpeg.ext,                1 );
    VoC_lw16i_short(REG5,2,IN_PARALLEL);
    VoC_lw16_d(REG6,STRUCT_CONFIG_mpeg_mode_ext_ADDR);
    VoC_sw32inc_p(REG45,REG1,DEFAULT);// headerPH = BF_addEntry( headerPH, config.mpeg.mode,               2 );
    VoC_lw16i_short(REG7,2,IN_PARALLEL);
    VoC_lw16_d(REG4,STRUCT_CONFIG_mpeg_copyright_ADDR);
    VoC_sw32inc_p(REG67,REG1,DEFAULT); //  headerPH = BF_addEntry( headerPH, config.mpeg.mode_ext,           2 );
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_lw16_d(REG6,STRUCT_CONFIG_mpeg_original_ADDR);
    VoC_sw32inc_p(REG45,REG1,DEFAULT);// headerPH = BF_addEntry( headerPH, config.mpeg.copyright,          1 );
    VoC_lw16i_short(REG7,1,IN_PARALLEL);
    VoC_lw16_d(REG4,STRUCT_CONFIG_mpeg_emph_ADDR);
    VoC_sw32inc_p(REG67,REG1,DEFAULT);//  headerPH = BF_addEntry( headerPH, config.mpeg.original,           1 );
    VoC_lw16i_short(REG5,2,IN_PARALLEL);
    VoC_lw16i_short(RL7_LO,0xd,DEFAULT);
    VoC_sw32inc_p(REG45,REG1,DEFAULT);// headerPH = BF_addEntry( headerPH, config.mpeg.emph,               2 );
    VoC_sw32_d(RL7,STRUCT_frameData_STRUCT_header_ADDR);

    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_lw16i_set_inc(REG0,STRUCT_frameData_STRUCT_frameSI_ADDR,2);
    VoC_loop_i_short(7,DEFAULT);
    VoC_startloop0
    VoC_sw16inc_p(REG1,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG2,STRUCT_SIDEINFO_main_data_begin_ADDR,2);
    VoC_lw32_d(REG01,STRUCT_frameData_STRUCT_frameSI_ADDR);
    VoC_lw32inc_p(REG67,REG2,DEFAULT);
    VoC_lw16i_short(REG7,9,DEFAULT);
    VoC_lw16i_short(REG5,2,DEFAULT);//VoC_lw16_d(REG5,MP3_CONST_2_ADDR);//REG5:channels
    VoC_sw32inc_p(REG67,REG1,DEFAULT);
    VoC_lw32inc_p(REG67,REG2,DEFAULT);
    VoC_lw16i_short(REG7,3,DEFAULT);
//     VoC_be16_rd(encodeSideInfo_channels_2,REG5,MP3_CONST_2_ADDR)
//         VoC_lw16i_short(REG7,5,DEFAULT);
//encodeSideInfo_channels_2:
    VoC_lw16i_short(REG4,2,DEFAULT);
    VoC_sw32inc_p(REG67,REG1,DEFAULT);
    VoC_sw16_d(REG4,STRUCT_frameData_STRUCT_frameSI_ADDR);

    VoC_lw16i_set_inc(REG0,STRUCT_frameData_STRUCT_channelSI_ADDR,2);
    VoC_lw32inc_p(REG67,REG2,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_lw32_p(RL6,REG0,DEFAULT);
    VoC_movreg16(REG1,RL6_HI,DEFAULT);

    VoC_loop_r(1,REG5)
    VoC_loop_i_short(4,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);
    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);
    VoC_sw32inc_p(REG67,REG1,DEFAULT);
    VoC_endloop0
    VoC_sw16inc_p(REG3,REG0,DEFAULT);
    VoC_lw32_p(RL6,REG0,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_movreg16(REG1,RL6_HI,DEFAULT);
    VoC_endloop1
    VoC_lw16i_set_inc(REG0,STRUCT_frameData_STRUCT_spectrumSI_ADDR,2);
    VoC_lw16i_short(REG4,2,DEFAULT);
encodeSideInfo_gr_loop:
    VoC_loop_r(1,REG5)
    VoC_lw32_p(RL7,REG0,DEFAULT);

    VoC_lw16i_set_inc(REG3,TABLE_encodeSideInfo_tab_ADDR,1);
    VoC_loop_i_short(2,DEFAULT)
    VoC_movreg16(REG1,RL7_HI,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    VoC_lw16inc_p(REG7,REG3,IN_PARALLEL);
    VoC_NOP();
    VoC_sw32inc_p(REG67,REG1,DEFAULT);//    *pph = BF_addEntry( *pph, gi->part2_3_length,        12 );
    VoC_endloop0
    VoC_lw32inc_p(REG67,REG2,DEFAULT);
    VoC_loop_i_short(2,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    VoC_lw16inc_p(REG7,REG3,IN_PARALLEL);
    VoC_NOP();
    VoC_sw32inc_p(REG67,REG1,DEFAULT);//    *pph = BF_addEntry( *pph, gi->part2_3_length,        12 );
    VoC_endloop0
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);
    VoC_NOP();
    VoC_sw32inc_p(REG67,REG1,DEFAULT);//    *pph = BF_addEntry( *pph, gi->part2_3_length,        12 );
    VoC_loop_i_short(8,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    VoC_lw16inc_p(REG7,REG3,IN_PARALLEL);
    VoC_NOP();
    VoC_sw32inc_p(REG67,REG1,DEFAULT);//    *pph = BF_addEntry( *pph, gi->part2_3_length,        12 );
    VoC_endloop0
    VoC_lw16i_short(REG7,13,DEFAULT);
    VoC_add16_rd(REG2,REG2,MP3_CONST_20_ADDR);
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    VoC_endloop1
    VoC_lw16i_set_inc(REG2,STRUCT_SIDEINFO_gr1_ch0_part2_3_length_ADDR,2);
    VoC_sub16_rd(REG4,REG4,MP3_CONST_1_ADDR);
    VoC_bnez16_r(encodeSideInfo_gr_loop,REG4)
    VoC_return
}


/*
Note the discussion of huffmancodebits() on pages 28
and 29 of the IS, as well as the definitions of the side
information on pages 26 and 27.
*/
// REG0 :REG3, codedDataPH[gr][ch]  , REG1:     gi , REG2:ix
void CS_Huffmancodebits(void)
{

    VoC_push16(RA,DEFAULT);

    VoC_lw32z(ACC1,IN_PARALLEL);

    /* 1: Write the bigvalues */
    VoC_lw16_d(REG6,STRUCT_CONFIG_mpeg_type_ADDR);
    VoC_mult16_rd(REG6,REG6,MP3_CONST_3_ADDR);
    VoC_add16_rd(REG3, REG1,MP3_CONST_16_ADDR);
    VoC_add16_rd(REG6,REG6,STRUCT_CONFIG_mpeg_samplerate_index_ADDR);
    VoC_mult16_rd(REG6,REG6,MP3_CONST_37_ADDR);
    VoC_lw16i(REG4,TABLE_sfBandIndex_tab_ADDR);
    VoC_add16_rr(REG6,REG6,REG4,DEFAULT);//int *scalefac = &sfBandIndex[config.mpeg.samplerate_index+(config.mpeg.type*3)].l[0];
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_add16_rd(REG7,REG4,MP3_CONST_1_ADDR);// scalefac_index = gi->region0_count + 1;
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_add16_rd(REG4,REG4,MP3_CONST_1_ADDR);
    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);// scalefac_index += gi->region1_count + 1;
    VoC_add16_rr(REG7,REG6,REG7,DEFAULT);
    VoC_add16_rr(REG5,REG6,REG4,DEFAULT);
    VoC_lw16_p(REG4,REG7,DEFAULT);// region1Start = scalefac[ scalefac_index ];
    VoC_add16_rd(REG7, REG1,MP3_CONST_2_ADDR);
    VoC_lw16_p(REG5,REG5,DEFAULT);//  region2Start = scalefac[ scalefac_index ];
    VoC_lw16_p(REG7, REG7,DEFAULT);
    VoC_shru16_ri(REG7,-1,DEFAULT);//   bigvalues = gi->big_values <<1;

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_bez16_r(Huffmancodebits_part2,REG7)
Huffmancodebits_bigvalues_loop_start:  //for ( i = 0; i < bigvalues; i += 2 )
    // {
    VoC_add16_rd(REG3,REG1,MP3_CONST_10_ADDR);// tableindex = gi->table_select[0];
    VoC_push32(REG67,DEFAULT);

    VoC_bgt16_rr(Huffmancodebits_bigvalues_loop_100,REG4,REG6)
    VoC_add16_rd(REG3,REG1,MP3_CONST_12_ADDR); //tableindex = gi->table_select[1];
    VoC_bgt16_rr(Huffmancodebits_bigvalues_loop_100,REG5,REG6)
    VoC_add16_rd(REG3,REG1,MP3_CONST_14_ADDR);
Huffmancodebits_bigvalues_loop_100:

    VoC_lw32inc_p(RL6,REG2,DEFAULT);// x = ix[i];
    VoC_lw32inc_p(RL7,REG2,DEFAULT);//    y = ix[i + 1];
    VoC_lw16_p(REG3,REG3,DEFAULT);// tableindex
    /* get huffman code */
    VoC_push32(REG45,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);
    VoC_bez16_r(Huffmancodebits_bigvalues_loop_200,REG3)
    VoC_push32(REG01,DEFAULT);
    VoC_push32(ACC1,DEFAULT);
    VoC_jal(CS_HuffmanCode);// bits = HuffmanCode( tableindex, x, y, &code, &ext, &cbits, &xbits );
    //output: REG0 : *code   REG1 : *cbits
    //        REG2 : *xbits  REG3 :return
    //        RL6  : *ext
    VoC_movreg16(REG6,REG0,DEFAULT);
    VoC_movreg16(ACC0_LO,REG1,IN_PARALLEL);

    VoC_pop32(REG45,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG3,IN_PARALLEL);
    VoC_add16_rd(REG4,REG4,MP3_CONST_1_ADDR);
    VoC_lw16i_short(ACC0_HI,0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_movreg16(REG6,REG2,IN_PARALLEL);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);// *pph = BF_addEntry( *pph,  code, cbits );
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_bez32_r(Huffmancodebits_1010,REG67)
    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_add16_rd(REG4,REG4,MP3_CONST_1_ADDR);//       *pph = BF_addEntry( *pph,  ext, xbits );
Huffmancodebits_1010:
    VoC_movreg32(ACC1,REG45,DEFAULT);//   bitsWritten += rt = bits;
Huffmancodebits_bigvalues_loop_200:
    VoC_pop32(REG45,DEFAULT);
    VoC_pop16(REG2,IN_PARALLEL);
    VoC_pop32(REG67,DEFAULT);
    VoC_add16_rd(REG6,REG6,MP3_CONST_2_ADDR);
    VoC_bgt16_rr(Huffmancodebits_bigvalues_loop_start,REG7,REG6)//         }


Huffmancodebits_part2:

    /* 2: Write count1 area */
    VoC_add16_rd(REG4,REG1,MP3_CONST_4_ADDR);
    VoC_add16_rd(REG5,REG1,MP3_CONST_24_ADDR);
    VoC_lw16_p(REG4,REG4,DEFAULT);//gi->count1
    VoC_lw16_p(REG5,REG5,DEFAULT);//gi->count1table_select

    VoC_add16_rd(REG5,REG5,MP3_CONST_32_ADDR);
    VoC_mult16_rd(REG5,REG5,MP3_CONST_5_ADDR);
    VoC_lw16i_set_inc(REG3,TABLE_ht_tab_ADDR,1);
    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);//  h = &ht[gi->count1table_select + 32];
    VoC_push16(REG1,IN_PARALLEL);
    VoC_add16_rd(REG3,REG3,MP3_CONST_3_ADDR); //point to h->table
    VoC_bez16_r(Huffmancodebits_101010,REG4)
    VoC_loop_r_short(REG4,DEFAULT)// for ( i = bigvalues; i < count1End; i += 4 )
    VoC_lw32inc_p(REG45,REG2,IN_PARALLEL);// v = ix[i];
    VoC_startloop0//{
    VoC_lw32inc_p(RL6,REG2,DEFAULT);// w = ix[i+1];
    VoC_lw32inc_p(REG67,REG2,DEFAULT);// x = ix[i+2];
    VoC_movreg16(REG5,RL6_LO,IN_PARALLEL);
    VoC_lw32inc_p(RL6,REG2,DEFAULT);//  y = ix[i+3];
    VoC_movreg16(REG7,RL6_LO,DEFAULT);
    //REG4 : v ,REG5 :w ,REG6 : x , REG7 : y
    VoC_jal(CS_L3_huffman_coder_count1); //bitsWritten += L3_huffman_coder_count1( pph, h, v, w, x, y );
    VoC_lw32inc_p(REG45,REG2,DEFAULT);// v = ix[i];
    VoC_endloop0//}
Huffmancodebits_101010:

    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG3,REG1,MP3_CONST_26_ADDR);
    VoC_lw16_p(REG4,REG1,DEFAULT);
    VoC_movreg16(REG5,ACC1_HI,IN_PARALLEL);
    VoC_sub16_rp(REG4,REG4,REG3,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_movreg16(REG3,ACC1_LO,IN_PARALLEL);
    VoC_bez16_r(Huffmancodebits_end,REG4)// if ( (stuffingBits = gi->part2_3_length - gi->part2_length - bitsWritten) )
    VoC_movreg16(REG5,REG4,DEFAULT);//int stuffingWords = stuffingBits / 32;
    VoC_shru16_ri(REG4,5,IN_PARALLEL);
    VoC_and16_ri(REG5,0x1f);        //int remainingBits = stuffingBits % 32;
    VoC_lw16i_short(REG7,-1,DEFAULT);
    VoC_lw16i_short(REG6,-1,IN_PARALLEL);

    VoC_bez16_r(Huffmancodebits_300,REG4)
    VoC_loop_r_short(REG4,DEFAULT)    // while ( stuffingWords-- )
    VoC_lw16i_short(ACC0_LO,32,IN_PARALLEL);
    VoC_startloop0
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);// *pph = BF_addEntry( *pph, ~0, 32 );
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_endloop0
Huffmancodebits_300:

    VoC_bez16_r(Huffmancodebits_end,REG5)// if ( remainingBits )
    VoC_movreg16(ACC0_LO,REG5,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);//   *pph = BF_addEntry( *pph, ~0, remainingBits );
    VoC_inc_p(REG3,IN_PARALLEL);
Huffmancodebits_end:
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return
}

//input: REG0: *pph , REG3: h, REG4 : v ,REG5 :w ,REG6 : x , REG7 : y
void CS_L3_huffman_coder_count1(void)
{
    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_push32(REG23,IN_PARALLEL);
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);
    VoC_bgtz16_r(L3_huffman_coder_count1_abs_and_sign_v,REG4)
    VoC_sub16_rr(REG4,REG1,REG4,DEFAULT);
    VoC_lw16i_short(RL6_LO,1,IN_PARALLEL);//  signv = abs_and_sign( &v );
L3_huffman_coder_count1_abs_and_sign_v:
    VoC_bgtz16_r(L3_huffman_coder_count1_abs_and_sign_w,REG5)
    VoC_sub16_rr(REG5,REG1,REG5,DEFAULT);
    VoC_lw16i_short(RL6_HI,1,IN_PARALLEL);
L3_huffman_coder_count1_abs_and_sign_w://  signw = abs_and_sign( &w );
    VoC_bgtz16_r(L3_huffman_coder_count1_abs_and_sign_x,REG6)
    VoC_sub16_rr(REG6,REG1,REG6,DEFAULT);
    VoC_lw16i_short(RL7_LO,1,IN_PARALLEL);
L3_huffman_coder_count1_abs_and_sign_x:// signx = abs_and_sign( &x );
    VoC_bgtz16_r(L3_huffman_coder_count1_abs_and_sign_y,REG7)
    VoC_sub16_rr(REG7,REG1,REG7,DEFAULT);
    VoC_lw16i_short(RL7_HI,1,IN_PARALLEL);
L3_huffman_coder_count1_abs_and_sign_y:// signy = abs_and_sign( &y );
    VoC_lw16inc_p(REG2,REG3,DEFAULT);//h->table
    VoC_shr16_ri(REG7,-3,DEFAULT);
    VoC_push32(REG67,IN_PARALLEL);

    VoC_shr16_ri(REG5,-1,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);

    VoC_shr16_ri(REG6,-2,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG4,IN_PARALLEL);

    VoC_add16_rr(REG7,REG7,REG6,DEFAULT);

    VoC_add16_rr(REG7,REG7,REG5,DEFAULT);// p = v + (w << 1) + (x << 2) + (y << 3);
    VoC_lw16inc_p(REG3,REG3,IN_PARALLEL);//h->hlen
    VoC_movreg16(REG6,REG7,DEFAULT);
    VoC_shr16_ri(REG7,1,IN_PARALLEL); //divided by 2
    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);
    VoC_and16_ri(REG6,1);
    VoC_lw16_p(REG2,REG2,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_lw16_p(REG3,REG3,DEFAULT);
    VoC_bez16_r(L3_huffman_coder_count1_even,REG6)
    VoC_shru16_ri(REG2,8,DEFAULT);
    VoC_shru16_ri(REG3,8,IN_PARALLEL);
    VoC_jump(L3_huffman_coder_count1_even_end)
L3_huffman_coder_count1_even:
    VoC_and16_ri(REG2,0x00ff);
    VoC_and16_ri(REG3,0x00ff);
L3_huffman_coder_count1_even_end:
    //REG2 :    huffbits = h->table[p];
    //REG3 :     len = h->hlen[ p ];
    VoC_movreg16(ACC0_LO,REG2,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    VoC_pop32(REG45,DEFAULT);
    VoC_lw32z(REG67,IN_PARALLEL);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_movreg16(ACC0_LO,REG3,IN_PARALLEL);
    VoC_lw16i_short(ACC0_LO,1,DEFAULT);
    VoC_movreg16(REG6,RL6_LO,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,REG23,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_bez16_r(L3_huffman_coder_count1_v,REG4)
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,REG23,IN_PARALLEL);
L3_huffman_coder_count1_v:
    VoC_movreg16(REG6,RL6_HI,DEFAULT);

    VoC_bez16_r(L3_huffman_coder_count1_w,REG5)
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,REG23,IN_PARALLEL);
L3_huffman_coder_count1_w:
    VoC_movreg16(REG6,RL7_LO,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);
    VoC_bez16_r(L3_huffman_coder_count1_x,REG4)
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,REG23,IN_PARALLEL);
L3_huffman_coder_count1_x:
    VoC_movreg16(REG6,RL7_HI,DEFAULT);
    VoC_bez16_r(L3_huffman_coder_count1_y,REG5)
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,REG23,IN_PARALLEL);
L3_huffman_coder_count1_y:

    VoC_pop32(REG23,DEFAULT);
    VoC_return
}



/* Implements the pseudocode of page 98 of the IS */
//input:REG3 : table_select,RL6 : x, RL7 : y
//output: REG0 : *code   REG1 : *cbits
//        REG2 : *xbits  REG3 :return
//        RL6  : *ext
void CS_HuffmanCode(void)
{
    VoC_bnez16_r(HuffmanCode_go,REG3)
    VoC_lw32z(REG01,DEFAULT);
    VoC_lw32z(REG23,IN_PARALLEL);
    VoC_lw32z(RL6,DEFAULT);
    VoC_jump(HuffmanCode_end)
HuffmanCode_go:
    VoC_mult16_rd(REG4,REG3,MP3_CONST_5_ADDR);
    VoC_lw16i_set_inc(REG2,TABLE_ht_tab_ADDR,1);
    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);

    VoC_lw32z(ACC1,DEFAULT);// signx, signy
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_bgtz32_r(HuffmanCode_abs_and_sign_100,RL6)
    VoC_lw16i_short(ACC1_LO,1,DEFAULT);
    VoC_sub32_rr(RL6,ACC0,RL6,IN_PARALLEL);
HuffmanCode_abs_and_sign_100:
    VoC_bgtz32_r(HuffmanCode_abs_and_sign_200,RL7)
    VoC_lw16i_short(ACC1_HI,1,DEFAULT);
    VoC_sub32_rr(RL7,ACC0,RL7,IN_PARALLEL);
HuffmanCode_abs_and_sign_200:
    VoC_lw16inc_p(REG4,REG2,DEFAULT); //xlen = h->xlen; ylen = h->ylen; in REG4, the lower 8 bits is  xlen and high 8 bits is ylen !
    VoC_movreg16(REG6,RL6_LO,IN_PARALLEL);

    VoC_lw16inc_p(REG5,REG2,DEFAULT);//linbits = h->linbits;
    VoC_movreg16(REG7,RL7_LO,IN_PARALLEL);//move x, y to REG67

    VoC_inc_p(REG2,DEFAULT);//point to h->table
    VoC_shru16_ri(REG4,8,IN_PARALLEL);//get ylen


    VoC_bgt16_rd(LBALE_COMPILE_9,REG3,MP3_CONST_15_ADDR);
    VoC_jump(HuffmanCode_table_select_lt15);
LBALE_COMPILE_9:
    VoC_lw16i_short(REG0,15,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    VoC_push32(ACC1,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);//linbitsx = linbitsy = 0;
    VoC_lw32z(ACC1,DEFAULT);
    VoC_bgt32_rr(HuffmanCode_table_select_xgt14,REG01,RL6)
    VoC_sub32_rr(ACC0,RL6,REG01,DEFAULT);// linbitsx = x - 15;
    VoC_lw16i_short(REG6,15,IN_PARALLEL);// x = 15;
HuffmanCode_table_select_xgt14:
    VoC_bgt32_rr(HuffmanCode_table_select_ygt14,REG01,RL7)
    VoC_sub32_rr(ACC1,RL7,REG01,DEFAULT);//  linbitsy = y - 15;
    VoC_lw16i_short(REG7,15,IN_PARALLEL);// y = 15;
HuffmanCode_table_select_ygt14:

    VoC_mult16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_lw16inc_p(REG1,REG2,DEFAULT);// h->table
    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);// idx = (x * ylen) + y;

    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_shr16_ri(REG0,1,DEFAULT); //divided by 2!
    VoC_lw16inc_p(REG2,REG2,IN_PARALLEL);//h->hlen

    VoC_add16_rr(REG2,REG2,REG0,DEFAULT);//h->hlen [idx];
    VoC_add16_rr(REG0,REG1,REG0,IN_PARALLEL);//h->table[idx];

    VoC_bgt16_rd(HuffmanCode_table_select_32,REG3,MP3_CONST_31_ADDR)
    VoC_add16_rr(REG0,REG1,REG4,DEFAULT);
HuffmanCode_table_select_32:
    VoC_and16_ri(REG4,0x1);//keep LSB
    VoC_lw16_p(REG1,REG2,DEFAULT);// *cbits = h->hlen [idx];
    VoC_lw16_p(REG0,REG0,DEFAULT);//*code  = h->table[idx];
    VoC_movreg16(REG2,REG0,DEFAULT);
    VoC_bez16_r(HuffmanCode_table_select_odd,REG4)
    VoC_shr16_ri(REG0,8,DEFAULT);
    VoC_shr16_ri(REG1,8,IN_PARALLEL);
    VoC_jump(HuffmanCode_table_select_odd_end)
HuffmanCode_table_select_odd:
    VoC_and16_ri(REG0,0x00ff);
    VoC_and16_ri(REG1,0x00ff);
HuffmanCode_table_select_odd_end:
    VoC_bgt16_rd(HuffmanCode_table_select_32_2,REG3,MP3_CONST_31_ADDR)
    VoC_movreg16(REG0,REG2,DEFAULT);
HuffmanCode_table_select_32_2:
    //REG0,  *code   REG1 ,*cbits
    VoC_pop32(RL7,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);//  *ext   = 0;
    VoC_lw16i_short(REG2,0,DEFAULT);//  *xbits = 0;
    VoC_bngt16_rd(HuffmanCode_100,REG6,MP3_CONST_14_ADDR)
    VoC_or32_rr(RL6,ACC0,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG5,IN_PARALLEL);
HuffmanCode_100:
    VoC_bez16_r(HuffmanCode_101,REG6)
    VoC_shru32_ri(RL6,-1,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_movreg16(ACC0_LO,RL7_LO,DEFAULT);
    VoC_or32_rr(RL6,ACC0,DEFAULT);
    VoC_add16_rd(REG2,REG2,MP3_CONST_1_ADDR);
HuffmanCode_101:
    VoC_bngt16_rd(HuffmanCode_200,REG7,MP3_CONST_14_ADDR)
    VoC_sub16_dr(REG6,MP3_CONST_0_ADDR,REG5);
    VoC_shru32_rr(RL6,REG6,DEFAULT);
    VoC_or32_rr(RL6,ACC1,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG5,IN_PARALLEL);
HuffmanCode_200:

    VoC_bez16_r(HuffmanCode_201,REG7)
    VoC_shru32_ri(RL6,-1,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_movreg16(ACC0_LO,RL7_HI,DEFAULT);
    VoC_or32_rr(RL6,ACC0,DEFAULT);
    VoC_add16_rd(REG2,REG2,MP3_CONST_1_ADDR);
HuffmanCode_201:
    VoC_jump(HuffmanCode_end);
HuffmanCode_table_select_lt15:
    /* No ESC-words */
    VoC_mult16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_lw16inc_p(REG1,REG2,DEFAULT);// h->table
    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);// idx = (x * ylen) + y;


    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_lw16inc_p(REG2,REG2,IN_PARALLEL);//h->hlen
    VoC_shr16_ri(REG0,1,DEFAULT); //divided by 2!

    VoC_add16_rr(REG2,REG2,REG0,DEFAULT);//h->hlen [idx];
    VoC_add16_rr(REG0,REG1,REG0,IN_PARALLEL);//h->table[idx];

    VoC_and16_ri(REG4,0x1);//keep LSB
    VoC_lw16_p(REG1,REG2,DEFAULT);// *cbits = h->hlen [idx];
    VoC_lw16_p(REG0,REG0,DEFAULT);//*code  = h->table[idx];

    VoC_lw32z(RL6,IN_PARALLEL);//  *ext   = 0;
    VoC_lw16i_short(REG2,0,DEFAULT);//  *xbits = 0;

    VoC_bez16_r(HuffmanCode_table_select_odd_2,REG4)
    VoC_shr16_ri(REG0,8,DEFAULT);
    VoC_shr16_ri(REG1,8,IN_PARALLEL);
    VoC_jump(HuffmanCode_table_select_odd_2_end)
HuffmanCode_table_select_odd_2:
    VoC_and16_ri(REG0,0x00ff);
    VoC_and16_ri(REG1,0x00ff);
HuffmanCode_table_select_odd_2_end:
    //REG0,  *code   REG1 ,*cbits
    VoC_bez16_r(HuffmanCode_t100x,REG6)
    VoC_shru16_ri(REG0,-1,DEFAULT);
    VoC_movreg16(REG6,ACC1_LO,IN_PARALLEL);
    VoC_or16_rr(REG0,REG6,DEFAULT);
    VoC_add16_rd(REG1,REG1,MP3_CONST_1_ADDR);
HuffmanCode_t100x:
    VoC_bez16_r(HuffmanCode_end,REG7)
    VoC_shru16_ri(REG0,-1,DEFAULT);
    VoC_movreg16(REG6,ACC1_HI,IN_PARALLEL);
    VoC_or16_rr(REG0,REG6,DEFAULT);
    VoC_add16_rd(REG1,REG1,MP3_CONST_1_ADDR);
HuffmanCode_end:
    VoC_add16_rr(REG3,REG1,REG2,DEFAULT);
    VoC_return
}
//////////////////////////////////////////////////////////////
void CS_putbits(void)
{
    //extern FILE* output_file;
    VoC_push32(REG23,DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);
CS_putbits_start:
    VoC_bngtz16_r(CS_putbits_exit,REG4)
    VoC_lw16i_set_inc(REG2,TABLE_putmask_tab_ADDR,1);
    VoC_movreg16(REG5,REG4,DEFAULT);
    VoC_movreg32(ACC1,ACC0,IN_PARALLEL);
    VoC_bgt16_rr(CS_putbits_min,REG7,REG4)
    VoC_movreg16(REG5,REG7,DEFAULT);
CS_putbits_min:                 //   k = MIN(j, bs->buf_bit_idx);
    VoC_sub16_rr(REG3,REG4,REG5,DEFAULT);//(j-k)
    VoC_add16_rr(REG2,REG2,REG5,IN_PARALLEL);
    VoC_shru32_rr(ACC1,REG3,DEFAULT);//  tmp = val >> (j-k);
    VoC_sub16_rr(REG3,REG5,REG7,IN_PARALLEL);//  -(bs->buf_bit_idx-k)
    VoC_lw16_p(RL7_LO,REG2,DEFAULT);
    VoC_and32_rr(ACC1,RL7,DEFAULT);
    VoC_shru32_rr(ACC1,REG3,DEFAULT);
    VoC_shru32_rr(ACC1,REG6,DEFAULT);
    VoC_or32_rr(RL6,ACC1,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG5,IN_PARALLEL);// bs->buf_bit_idx -= k;
    VoC_bnez16_r(CS_putbits_100,REG7)
    VoC_lw16i_short(REG7,8,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_bgt16_rd(CS_putbits_100,REG6,MP3_CONST_NEG31_ADDR)
    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);
    VoC_lw16i_short(REG6,0,DEFAULT);
    /////////////////transfer  data  to  outside buf////////////
//      VoC_blt16_rd(CS_putbits_100,REG0,END_OF_OUTPUT_MP3_BUFFER_ADDR);
//    //DMAE BEGIN//////////
//    VoC_lw16i(REG3,2048);//SIZE
//    VoC_lw32_d(ACC1,Input_outStreamBufAddr_addr);
//    VoC_sw32_d(REG23,CFG_DMA_DATA_SINGLE);
//    VoC_cfg(CFG_CTRL_STALL|CFG_CTRL_IRQ_TO_XCPU);
//    VoC_cfg_lg(CFG_WAKEUP_DMAE,CFG_WAKEUP_STATUS);
//      //DMAE end//////////
//    VoC_lw16i_set_inc(REG0,OUTPUT_MP3_BUFFER_ADDR,2);
    /////////////////////////////////////////////////////////////
CS_putbits_100:
    VoC_sub16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_jump(CS_putbits_start)
CS_putbits_exit:
    VoC_pop32(REG23,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_return
}
///////////////////////////////////////////////////////
void CS_BF_BitstreamFrame(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16d_set_inc(REG0,STATIC_buf_short_idx_ADDR,2);
    VoC_lw32_d(RL6,STATIC_buf_chache_ADDR);
    VoC_lw16_d(REG7,STATIC_buf_bit_idx_ADDR);//
    VoC_lw16_d(REG6,STATIC_buf_long_idx_ADDR);
    //  VoC_jal(CS_main_data);
    VoC_jal(CS_writePartMainData);
    VoC_sw32_d(RL6,STATIC_buf_chache_ADDR);
    VoC_sw16_d(REG7,STATIC_buf_bit_idx_ADDR);//
    VoC_sw16_d(REG6,STATIC_buf_long_idx_ADDR);
    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG0,STATIC_buf_short_idx_ADDR);
    VoC_return;
}
//////////////////////////////////////////////////////////
void CS_writePartMainData(void)
{

    VoC_push16(RA,DEFAULT);
writePartMainData_nrEntries_loop:
    VoC_bez16_r(writePartMainData_nrEntries_loop_end,REG2)

    VoC_push16(REG2,DEFAULT);
    VoC_jal(CS_WriteMainDataBits);

    VoC_pop16(REG2,DEFAULT);
    VoC_sub16_rd(REG2,REG2,MP3_CONST_1_ADDR);
    VoC_jump(writePartMainData_nrEntries_loop)

writePartMainData_nrEntries_loop_end:
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return
}


/*
*INPUT: REG1 : part
*
*
*/
void CS_writePartSideInfo(void)
{

    VoC_lw32z(ACC0,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);

    VoC_lw32_p(REG23,REG2,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);

    VoC_push16(RA,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
CS_writePartSideInfo_nrEntries_loop:
    VoC_bez16_r(CS_writePartSideInfo_nrEntries_loop_end,REG2)
    VoC_lw16inc_p(ACC0_LO,REG3,DEFAULT);//ep->value
    VoC_lw16inc_p(REG4,REG3,DEFAULT);//ep->length
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);
    VoC_jal(CS_putbits);
    VoC_sub16_rd(REG2,REG2,MP3_CONST_1_ADDR);
    VoC_bgtz16_r(CS_writePartSideInfo_nrEntries_loop,REG2)
CS_writePartSideInfo_nrEntries_loop_end:
    VoC_pop16(RA,DEFAULT);
    VoC_pop16(REG2,DEFAULT);

    VoC_return
}
////////////////////////////////////////////////
void CS_WriteMainDataBits(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_lw32_d(RL7, STATIC_BitCount_ADDR);

    VoC_bne32_rd(CS_WriteMainDataBits_100, RL7, STATIC_ThisFrameSize_ADDR)
    VoC_push16(REG3, DEFAULT);
    VoC_jal(CS_write_side_info);
    VoC_pop16(REG3, DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);
    VoC_movreg16(RL7_LO,REG1,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);

    VoC_sub32_dr(ACC0,STATIC_ThisFrameSize_ADDR,RL7);
    VoC_sw32_d(RL7,STATIC_BitCount_ADDR);
    VoC_sw32_d(ACC0,STATIC_BitsRemaining_ADDR);

CS_WriteMainDataBits_100:
    VoC_lw32inc_p(ACC0,REG3,DEFAULT);
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_bez16_r(CS_WriteMainDataBits_exit,REG4)
    VoC_bngt32_rd(CS_WriteMainDataBits_200,REG45,STATIC_BitsRemaining_ADDR)

    VoC_sub32_rd(REG45,REG45, STATIC_BitsRemaining_ADDR);//nbits -= BitsRemaining;
    VoC_shru32_rr(ACC0,REG4,DEFAULT);// unsigned extra = val >> (nbits - BitsRemaining);
    VoC_push32(ACC0,IN_PARALLEL);
    VoC_push32(REG45,DEFAULT);
    VoC_lw16_d(REG4,STATIC_BitsRemaining_ADDR);
    VoC_jal(CS_putbits);        // (*PutBits)( extra, BitsRemaining );
    VoC_jal(CS_write_side_info);// BitCount = write_side_info();
    VoC_lw32z(RL7,DEFAULT);
    VoC_movreg16(RL7_LO,REG1,DEFAULT);

    VoC_sub32_dr(ACC0,STATIC_ThisFrameSize_ADDR,RL7);
    VoC_sw32_d(RL7,STATIC_BitCount_ADDR);
    VoC_sw32_d(ACC0,STATIC_BitsRemaining_ADDR);//  BitsRemaining = ThisFrameSize - BitCount;

    VoC_pop32(REG45,DEFAULT);//
    VoC_pop32(ACC0,DEFAULT);

CS_WriteMainDataBits_200:
    VoC_add32_rd(ACC1,REG45,STATIC_BitCount_ADDR);
    VoC_sub32_dr(RL7,STATIC_BitsRemaining_ADDR,REG45);
    VoC_sw32_d(ACC1,STATIC_BitCount_ADDR);
    VoC_sw32_d(RL7,STATIC_BitsRemaining_ADDR);
    VoC_jal(CS_putbits);  //     (*PutBits)( val, nbits );

CS_WriteMainDataBits_exit:


    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return
}




void CS_write_side_info(void)
{
    VoC_lw32_d(ACC0,STRUCT_frameData_frameLength_ADDR);
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    VoC_sw32_d(ACC0,STATIC_ThisFrameSize_ADDR);// ThisFrameSize = si->frameLength;

    VoC_lw16i_set_inc(REG2,STRUCT_frameData_STRUCT_header_ADDR,2);
    VoC_jal(CS_writePartSideInfo);
    VoC_lw16i_set_inc(REG2,STRUCT_frameData_STRUCT_frameSI_ADDR,2);
    VoC_jal(CS_writePartSideInfo);

    VoC_lw16i_set_inc(REG2,STRUCT_frameData_STRUCT_channelSI_ADDR,2);
    //VoC_lw16_d(REG5,MP3_CONST_2_ADDR);
    VoC_loop_i_short(2,DEFAULT)
    VoC_startloop0
    VoC_jal(CS_writePartSideInfo);
    VoC_lw16i_set_inc(REG2,STRUCT_frameData_STRUCT_channelSI_ADDR+2,2);
    VoC_endloop0

    VoC_lw16_d(REG4,STATIC_nGranules_ADDR);
    VoC_lw16i_set_inc(REG2,STRUCT_frameData_STRUCT_spectrumSI_ADDR,2);
    VoC_loop_r(1,REG4)
    //  VoC_lw16_d(REG5,STATIC_nChannels_ADDR);
    VoC_loop_i_short(2,DEFAULT)
    VoC_startloop0

    VoC_jal(CS_writePartSideInfo);//optimize

    VoC_add16_rd(REG2,REG2,MP3_CONST_2_ADDR);

    VoC_endloop0
    VoC_lw16i_set_inc(REG2,STRUCT_frameData_STRUCT_spectrumSI_ADDR+4,2);
    VoC_endloop1

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return

}












