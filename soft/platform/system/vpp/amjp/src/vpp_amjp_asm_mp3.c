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
#include "vppp_amjp_asm_mp3.h"
#include "vppp_amjp_asm_sections.h"

#include "voc2_library.h"
#include "voc2_define.h"


#if 0
voc_struct MP3_TABLE_RAM_X_STRUCT, x

voc_short TABLE_hufftab0_ADDR                       ,2  ,x
voc_short TABLE_hufftab1_ADDR                       ,8  ,x
voc_short TABLE_hufftab2_ADDR                       ,16 ,x
voc_short TABLE_hufftab3_ADDR                       ,16 ,x
voc_short TABLE_hufftab5_ADDR                       ,26 ,x
voc_short TABLE_hufftab6_ADDR                       ,28 ,x
voc_short TABLE_hufftab7_ADDR                       ,64 ,x
voc_short TABLE_hufftab8_ADDR                       ,66 ,x
voc_short TABLE_hufftab9_ADDR                       ,54 ,x
voc_short TABLE_hufftab10_ADDR                      ,102,x
voc_short TABLE_hufftab11_ADDR                      ,104,x
voc_short TABLE_hufftab12_ADDR                      ,96 ,x
voc_short TABLE_hufftab13_ADDR                      ,398,x
voc_short TABLE_hufftab15_ADDR                      ,380,x
voc_short TABLE_hufftab16_ADDR                      ,402,x
voc_short TABLE_hufftab24_ADDR                      ,386,x
voc_short TABLE_hufftabA_ADDR                       ,28 ,x
voc_short TABLE_hufftabB_ADDR                       ,16 ,x

voc_struct_end

voc_struct MP3_TABLE_RAM_Y_STRUCT, y

voc_word  CONST_0x00000800_ADDR                         ,y
voc_short CONST_neg1_ADDR                               ,y
voc_short CONST_0xe0ff_ADDR                             ,y
voc_short CONST_1152_ADDR                               ,y
voc_short CONST_17_ADDR                                 ,y
voc_short CONST_576_ADDR                                ,y
voc_short CONST_1024_ADDR                               ,y
voc_word  CONST_0x00001000_ADDR                         ,y
voc_word  CONST_0x100010_ADDR                           ,y
voc_short TABLE_imdct36_y_tab_ADDR                  ,18 ,y
voc_short TABLE_D_ADDR                              ,544,y
voc_short TABLE_sflen_table_ADDR                    ,32 ,y
voc_short TABLE_nsfb_table_ADDR                     ,72 ,y
voc_short TABLE_pretab_ADDR                         ,22 ,y
voc_short TABLE_cs_ADDR                             ,8  ,y
voc_short TABLE_ca_ADDR                             ,8  ,y
voc_short TABLE_imdct_s_ADDR                        ,36 ,y
voc_short TABLE_window_l_ADDR                       ,36 ,y
voc_short TABLE_window_s_ADDR                       ,12 ,y
voc_short TABLE_is_table_ADDR                       ,8  ,y
voc_short TABLE_is_lsf_table_ADDR                   ,30 ,y
voc_short TABLE_bitrate_table_ADDR                  ,80 ,y
voc_short TABLE_imdct36_tab_ADDR                    ,188,y
voc_short TABLE_dct32_tab_ADDR                      ,32 ,y
voc_short TABLE_imdct36_x_tab_ADDR                  ,36 ,y
voc_short TABLE_sfb_48000_long_ADDR                 ,22 ,y
voc_short TABLE_sfb_44100_long_ADDR                 ,22 ,y
voc_short TABLE_sfb_32000_long_ADDR                 ,22 ,y
voc_short TABLE_sfb_48000_short_ADDR                ,40 ,y
voc_short TABLE_sfb_44100_short_ADDR                ,40 ,y
voc_short TABLE_sfb_32000_short_ADDR                ,40 ,y
voc_short TABLE_sfb_48000_mixed_ADDR                ,38 ,y
voc_short TABLE_sfb_44100_mixed_ADDR                ,38 ,y
voc_short TABLE_sfb_32000_mixed_ADDR                ,38 ,y
voc_short TABLE_sfb_24000_long_ADDR                 ,22 ,y
voc_short TABLE_sfb_22050_long_ADDR                 ,22 ,y
voc_short TABLE_sfb_24000_short_ADDR                ,40 ,y
voc_short TABLE_sfb_22050_short_ADDR                ,40 ,y
voc_short TABLE_sfb_16000_short_ADDR                ,40 ,y
voc_short TABLE_sfb_24000_mixed_ADDR                ,36 ,y
voc_short TABLE_sfb_22050_mixed_ADDR                ,36 ,y
voc_short TABLE_sfb_16000_mixed_ADDR                ,36 ,y
voc_short TABLE_sfb_8000_long_ADDR                  ,22 ,y
voc_short TABLE_sfb_8000_short_ADDR                 ,40 ,y
voc_short TABLE_sfb_8000_mixed_ADDR                 ,40 ,y
voc_short TABLE_sfbwidth_table_ADDR                 ,28 ,y
voc_short TABLE_mad_huff_pair_table_ADDR            ,64 ,y

voc_struct_end
#endif


#if 0
voc_alias TABLE_sfb_16000_long_ADDR   TABLE_sfb_22050_long_ADDR                ,y
voc_alias TABLE_sfb_12000_long_ADDR   TABLE_sfb_16000_long_ADDR                ,y
voc_alias TABLE_sfb_11025_long_ADDR   TABLE_sfb_12000_long_ADDR                ,y
voc_alias TABLE_sfb_12000_short_ADDR  TABLE_sfb_16000_short_ADDR               ,y
voc_alias TABLE_sfb_11025_short_ADDR  TABLE_sfb_12000_short_ADDR               ,y
voc_alias TABLE_sfb_12000_mixed_ADDR  TABLE_sfb_16000_mixed_ADDR               ,y
voc_alias TABLE_sfb_11025_mixed_ADDR  TABLE_sfb_12000_mixed_ADDR               ,y

voc_alias CONST_2_ADDR                (26 + TABLE_imdct36_x_tab_ADDR)          ,y
voc_alias CONST_4_ADDR                (30 + TABLE_imdct36_x_tab_ADDR)          ,y
voc_alias CONST_6_ADDR                (32 + TABLE_imdct36_x_tab_ADDR)          ,y
voc_alias CONST_8_ADDR                (24 + TABLE_imdct36_x_tab_ADDR)          ,y
voc_alias CONST_10_ADDR               (12 + TABLE_imdct36_x_tab_ADDR)          ,y
voc_alias CONST_12_ADDR               (6 + TABLE_imdct36_x_tab_ADDR)           ,y
voc_alias CONST_14_ADDR               (0 + TABLE_imdct36_x_tab_ADDR)           ,y
voc_alias CONST_16_ADDR               (18 + TABLE_imdct36_x_tab_ADDR)          ,y
voc_alias CONST_20_ADDR               (1 + TABLE_imdct36_x_tab_ADDR)           ,y
voc_alias CONST_28_ADDR               (33 + TABLE_imdct36_x_tab_ADDR)          ,y
voc_alias CONST_30_ADDR               (31 + TABLE_imdct36_x_tab_ADDR)          ,y
voc_alias CONST_32_ADDR               (27 + TABLE_imdct36_x_tab_ADDR)          ,y
voc_alias CONST_36_ADDR               (10 + TABLE_imdct36_x_tab_ADDR)          ,y
voc_alias CONST_3_ADDR                (15 + TABLE_nsfb_table_ADDR)             ,y
voc_alias CONST_7_ADDR                (14 + TABLE_nsfb_table_ADDR)             ,y
voc_alias CONST_15_ADDR               (32+ TABLE_nsfb_table_ADDR)              ,y
voc_alias CONST_0_WORD32_ADDR         (26 + TABLE_nsfb_table_ADDR)             ,y
voc_alias CONST_0_ADDR                CONST_0_WORD32_ADDR                      ,y
voc_alias CONST_0x0010_ADDR           CONST_16_ADDR                            ,y
voc_alias CONST_0x0020_ADDR           CONST_32_ADDR                            ,y
voc_alias CONST_1_WORD32_ADDR         TABLE_hufftab0_ADDR                      ,y
voc_alias CONST_1_ADDR                CONST_1_WORD32_ADDR                      ,y
voc_alias CONST_0x0011_ADDR           CONST_17_ADDR                            ,y
voc_alias CONST_2048_ADDR             CONST_0x00000800_ADDR                    ,y
#endif


#if 0
voc_word STRUCT_left_dither_error_ADDR                                   ,3   ,y
voc_word STRUCT_right_dither_error_ADDR                                  ,3   ,y
#endif


#if 0
voc_struct STRUCT_MAD_HEADER_ADDR                                               ,y
voc_short STRUCT_MAD_HEADER_LAYER_ADDR                                      ,y
voc_short STRUCT_MAD_HEADER_MODE_ADDR                                       ,y
voc_short STRUCT_MAD_HEADER_MODE_EXTENSION_ADDR                             ,y
voc_short STRUCT_MAD_HEADER_EMPHASIS_ADDR                                   ,y
voc_short STRUCT_MAD_HEADER_BITRATE_ADDR                                    ,y
voc_short STRUCT_MAD_HEADER_SAMPLERATE_ADDR                                 ,y
voc_short STRUCT_MAD_HEADER_CRC_CHECK_ADDR                                  ,y
voc_short STRUCT_MAD_HEADER_CRC_TARGET_ADDR                                 ,y
voc_short STRUCT_MAD_HEADER_FLAGS_ADDR                                      ,y
voc_short STRUCT_MAD_HEADER_PRIVATE_BITS_ADDR                               ,y
voc_short STRUCT_MAD_TIMER_DURATION_SECONDS_ADDR                            ,y
voc_short STRUCT_MAD_TIMER_DURATION_FRACTION_ADDR                           ,y
voc_short STRUCT_MAD_FRAME_OPTIONS_ADDR                                     ,y
voc_short GLOBAL_MAD_SYNTH_PHASE_ADDR                                       ,y
voc_struct_end
#endif

#if 0
voc_struct STRUCT_MAD_STREAM_ADDR                                               ,y
voc_short STRUCT_MAD_STREAM_BUFFER_ADDR_ADDR                              ,2,y
voc_short STRUCT_MAD_STREAM_SKIPLEN_ADDR                                    ,y
voc_short STRUCT_MAD_STREAM_SYNC_ADDR                                       ,y
voc_short STRUCT_MAD_STREAM_FREERATE_ADDR                                   ,y
voc_short STRUCT_MAD_STREAM_THIS_FRAME_ADDR_ADDR                            ,y
voc_short STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR                            ,y
voc_short STRUCT_BITPTR_PTR_BYTE_ADDR_ADDR                                  ,y
voc_short STRUCT_BITPTR_PTR_CACHE_ADDR                                   ,2 ,y
voc_short STRUCT_BITPTR_PTR_LEFT_ADDR                                       ,y
voc_short STRUCT_BITPTR_ANC_PTR_BYTE_ADDR_ADDR                              ,y
voc_short STRUCT_BITPTR_ANC_PTR_CACHE_ADDR                               ,2 ,y
voc_short STRUCT_BITPTR_ANC_PTR_LEFT_ADDR                                   ,y
voc_short STRUCT_MAD_STREAM_ANC_BITLEN_ADDR                                 ,y
voc_short STRUCT_MAD_STREAM_MD_LEN_ADDR                                     ,y
voc_short STRUCT_MAD_STREAM_OPTIONS_ADDR                                    ,y
voc_short STRUCT_MAD_STREAM_MAD_ERROR_ADDR                                  ,y
voc_short GLOBLE_WRITE_POINTER_ADDR                                         ,y
voc_short GLOBAL_NCH_ADDR                                                   ,y
voc_short GLOBAL_NS_ADDR                                                    ,y
voc_short GLOBAL_NGR_ADDR                                                   ,y
voc_short GLOBAL_WRAP_INDEX_ADDR                                            ,y
voc_short STRUCT_SIDEINFO_ADDR                                              ,y
voc_alias STRUCT_SIDEINFO_main_data_begin_ADDR  STRUCT_SIDEINFO_ADDR        ,y
voc_short STRUCT_SIDEINFO_private_bits_ADDR                                 ,y
voc_short STRUCT_SIDEINFO_scfsi_ADDR                                     ,2 ,y
voc_struct_end
#endif

#if 0
voc_struct STRUCT_GRANULE0_BEGIN_ADDR                                           ,y
voc_short STRUCT_GRANULE0CH0_part2_3_length_ADDR                            ,y
voc_short STRUCT_GRANULE0CH0_big_values_ADDR                                ,y
voc_short STRUCT_GRANULE0CH0_global_gain_ADDR                               ,y
voc_short STRUCT_GRANULE0CH0_scalefac_compress_ADDR                         ,y
voc_short STRUCT_GRANULE0CH0_flags_ADDR                                     ,y
voc_short STRUCT_GRANULE0CH0_block_type_ADDR                                ,y
voc_short STRUCT_GRANULE0CH0_table_select_ADDR                          ,3  ,y
voc_short STRUCT_GRANULE0CH0_subblock_gain_ADDR                         ,3  ,y
voc_short STRUCT_GRANULE0CH0_region0_count_ADDR                             ,y
voc_short STRUCT_GRANULE0CH0_region1_count_ADDR                             ,y
voc_short STRUCT_GRANULE0CH0_scalefac_ADDR                              ,40 ,y
voc_short STRUCT_GRANULE0CH1_part2_3_length_ADDR                            ,y
voc_short STRUCT_GRANULE0CH1_big_values_ADDR                                ,y
voc_short STRUCT_GRANULE0CH1_global_gain_ADDR                               ,y
voc_short STRUCT_GRANULE0CH1_scalefac_compress_ADDR                         ,y
voc_short STRUCT_GRANULE0CH1_flags_ADDR                                     ,y
voc_short STRUCT_GRANULE0CH1_block_type_ADDR                                ,y
voc_short STRUCT_GRANULE0CH1_table_select_ADDR                          ,3  ,y
voc_short STRUCT_GRANULE0CH1_subblock_gain_ADDR                         ,3  ,y
voc_short STRUCT_GRANULE0CH1_region0_count_ADDR                             ,y
voc_short STRUCT_GRANULE0CH1_region1_count_ADDR                             ,y
voc_short STRUCT_GRANULE0CH1_scalefac_ADDR                              ,40 ,y
voc_struct_end
#endif

#if 0
voc_struct STRUCT_GRANULE1_BEGIN_ADDR                                           ,y
voc_short STRUCT_GRANULE1CH0_part2_3_length_ADDR                            ,y
voc_short STRUCT_GRANULE1CH0_big_values_ADDR                                ,y
voc_short STRUCT_GRANULE1CH0_global_gain_ADDR                               ,y
voc_short STRUCT_GRANULE1CH0_scalefac_compress_ADDR                         ,y
voc_short STRUCT_GRANULE1CH0_flags_ADDR                                     ,y
voc_short STRUCT_GRANULE1CH0_block_type_ADDR                                ,y
voc_short STRUCT_GRANULE1CH0_table_select_ADDR                          ,3  ,y
voc_short STRUCT_GRANULE1CH0_subblock_gain_ADDR                         ,3  ,y
voc_short STRUCT_GRANULE1CH0_region0_count_ADDR                             ,y
voc_short STRUCT_GRANULE1CH0_region1_count_ADDR                             ,y
voc_short STRUCT_GRANULE1CH0_scalefac_ADDR                              ,40 ,y
voc_short STRUCT_GRANULE1CH1_part2_3_length_ADDR                            ,y
voc_short STRUCT_GRANULE1CH1_big_values_ADDR                                ,y
voc_short STRUCT_GRANULE1CH1_global_gain_ADDR                               ,y
voc_short STRUCT_GRANULE1CH1_scalefac_compress_ADDR                         ,y
voc_short STRUCT_GRANULE1CH1_flags_ADDR                                     ,y
voc_short STRUCT_GRANULE1CH1_block_type_ADDR                                ,y
voc_short STRUCT_GRANULE1CH1_table_select_ADDR                          ,3  ,y
voc_short STRUCT_GRANULE1CH1_subblock_gain_ADDR                         ,3  ,y
voc_short STRUCT_GRANULE1CH1_region0_count_ADDR                             ,y
voc_short STRUCT_GRANULE1CH1_region1_count_ADDR                             ,y
voc_short STRUCT_GRANULE1CH1_scalefac_ADDR                              ,40 ,y
voc_short PTR_CACHE_ADDR                                                ,2  ,y
voc_short PTR_BYTE_ADDR_ADDR                                                ,y
voc_short PTR_LEFT_ADDR                                                     ,y
voc_short mad_layer_III_result                                              ,y
voc_short GLOBAL_MAIN_DATA_OFFSET_ADDR                                      ,y
voc_short STRUCT_GRANULE1_RESERVED                                      ,2  ,y
voc_struct_end
#endif

#if 0
voc_short STATIC_MAD_OVERLAP_ADDR                                         ,2304 ,y
voc_short GLOBAL_MAD_SYNTH_FILTER_ADDR                                    ,2048 ,y
voc_short STATIC_MAD_MAIN_DATA_ADDR                                       ,900  ,y
#endif



void VPP_MP3_DECODE(void)
{

#if 0
    voc_short OUTPUT_PCM_BUFFER1_ADDR                                         ,2304 ,x
    voc_short STATIC_MAD_SBSAMPLE_ADDR                                        ,4608 ,x
#endif


    VoC_push16(RA,DEFAULT);

    VoC_bnez16_d(main_decode_while_new_add,GLOBAL_RESET)   //reset
    VoC_jump(main_decode_while);
main_decode_while_new_add:


    VoC_lw16i_short(FORMAT16,-1,DEFAULT);
    VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);

    VoC_lw16i_short(FORMATX,14,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_lw16i_short(INC3,1,DEFAULT);

//register initial

    VoC_lw16i_short(WRAP0,0,IN_PARALLEL);
    VoC_lw16i_short(WRAP1,0,DEFAULT);
    VoC_lw16i_short(WRAP2,0,IN_PARALLEL);
    VoC_lw16i_short(WRAP3,0,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);
    VoC_sw16_d(REG7,CFG_DMA_WRAP);
    VoC_lw16i(STATUS,STATUS_CLR);

//end
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_set_inc(REG0,STRUCT_MAD_HEADER_LAYER_ADDR, 2);

    VoC_sw16_d(ACC0_LO,GLOBAL_MAIN_DATA_OFFSET_ADDR);


    //VoC_loop_i(0,64);
    VoC_loop_i(0,129);
    VoC_sw32inc_p(ACC0, REG0,DEFAULT);
    VoC_endloop0

    VoC_sw16_d(REG7,GLOBAL_MAD_SYNTH_PHASE_ADDR);
    VoC_lw16i(REG5, GLOBAL_INPUT_DATA_BUFFER*2  + MPEG_BUFSZ );
    VoC_lw16i(REG4, GLOBAL_INPUT_DATA_BUFFER*2   );

    VoC_sw16_d(REG4, STRUCT_MAD_STREAM_BUFFER_ADDR_ADDR);
    VoC_sw16_d(REG4, STRUCT_MAD_STREAM_THIS_FRAME_ADDR_ADDR);
    VoC_sw16_d(REG4, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR);

    VoC_sw16_d(REG4, GLOBLE_WRITE_POINTER_ADDR);

    VoC_lw16i_set_inc(REG0,STATIC_MAD_OVERLAP_ADDR,2);
    VoC_lw16i_set_inc(REG1,STATIC_MAD_SBSAMPLE_ADDR,2);
    VoC_lw16i_set_inc(REG2,GLOBAL_MAD_SYNTH_FILTER_ADDR,2);

    VoC_loop_i(1,16);
    VoC_loop_i(0,72);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);   //MAD_OVERLAP
    VoC_endloop0

    VoC_loop_i(0,144);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);   //sbsample;
    VoC_endloop0

    VoC_loop_i(0,64);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);   //mad_synth_mute(synth);
    VoC_endloop0

    VoC_NOP();
    VoC_endloop1

    //synth->phase = 0;    synth->pcm.samplerate = 0;
    //synth->pcm.channels = 0; synth->pcm.length = 0;

    VoC_lw16i_set_inc(REG0,STRUCT_left_dither_error_ADDR,2);
    VoC_loop_i_short(8,DEFAULT);
    VoC_startloop0
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop0

    VoC_jal(CII_mad_frame_mute);

    VoC_jal(vpp_AmjpCircBuffInit);

main_decode_do:

    VoC_lw16i_short(REG3, 1,DEFAULT);
    VoC_lw16i_short(REG6,-1,IN_PARALLEL);
    VoC_lw16_d(REG7, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);
    VoC_sw16_d(REG3, STRUCT_MAD_STREAM_SYNC_ADDR);

    VoC_be16_rd(main_decode_while_100_1, REG7, CONST_1_ADDR)  //  while (stream->error == MAD_ERROR_BUFLEN);
    VoC_jump(main_decode_while_100);
main_decode_while_100_1:

    VoC_sw16_d(REG6,GLOBAL_ERR_STATUS);

    VoC_jal(CII_reload_data_temp);

    VoC_jump(main_decode_exit);

main_decode_while_100:

    VoC_lw16i_short(BITCACHE,0,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);   //    return result;
    VoC_bne16_rd(main_decode_100_2, REG7, CONST_0x0010_ADDR);   //    case MAD_FLOW_STOP:    goto done;
    VoC_jump(main_decode_exit);
main_decode_100_2:
    VoC_bne16_rd(main_decode_while, REG7, CONST_0x0011_ADDR);   //     case MAD_FLOW_BREAK:    goto fail;
    VoC_jump(main_decode_exit);

main_decode_while:

    VoC_lw16i_short(WRAP0,10,DEFAULT);
    VoC_jal(CII_mad_header_decode);    //return in REG7
    VoC_lw16i_short(WRAP0,16,DEFAULT);
    VoC_bne16_rd(main_decode_mad_header_decode, REG7,CONST_neg1_ADDR)   //if (mad_header_decode(&frame->header, stream) == -1) {
    VoC_lw16_d(REG6, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);
    VoC_movreg16(REG7, REG6,DEFAULT);
    VoC_and16_ri(REG7,0xff00);                          //     if (!MAD_RECOVERABLE(stream->error))
    VoC_bez16_r(main_decode_do, REG7)    //    break;
    VoC_jal(CII_decode_error);        //    switch (error_func(error_data, stream, frame)) {
    VoC_bne16_rd(main_decode_while_2, REG7, CONST_0x0010_ADDR);   //    case MAD_FLOW_STOP:    goto done;
    VoC_jump(main_decode_exit);

main_decode_while_2:
    VoC_bne16_rd(main_decode_while_200, REG7, CONST_0x0011_ADDR);   //     case MAD_FLOW_BREAK:    goto fail;
    VoC_jump(main_decode_exit);                                                                                                //  case MAD_FLOW_IGNORE:
main_decode_while_200:
    VoC_be16_rd(main_decode_while,REG7,CONST_0x0020_ADDR);
    VoC_be16_rd(main_decode_while,REG7,CONST_0_ADDR);
main_decode_mad_header_decode:

    VoC_jal(CII_mad_frame_decode);

    //return in REG7
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_bne16_rd(mad_frame_decode_l10a, REG7,CONST_neg1_ADDR)   //  if (mad_frame_decode(frame, stream) == -1) {
    VoC_lw16_d(REG6, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);
    VoC_movreg16(REG7, REG6,DEFAULT);
    VoC_and16_ri(REG7,0xff00);                           //if (!MAD_RECOVERABLE(stream->error))
    VoC_bez16_r(main_decode_do, REG7)  // break;
    VoC_jal(CII_decode_error);               //     switch (error_func(error_data, stream, frame)) {
    VoC_bne16_rd(main_decode_mad_header_decode_1, REG7, CONST_0x0010_ADDR);  //      case MAD_FLOW_STOP:   goto done;
    VoC_jump(main_decode_exit);
main_decode_mad_header_decode_1:
    VoC_bne16_rd(main_decode_mad_header_decode_2, REG7, CONST_0x0011_ADDR); // case MAD_FLOW_BREAK:    goto fail;
    VoC_jump(main_decode_exit);
main_decode_mad_header_decode_2:
    VoC_be16_rd(mad_frame_decode_l10a, REG7, CONST_0x0020_ADDR);  // case MAD_FLOW_IGNORE:  break;
    //  VoC_bne16_rd(mad_frame_decode_10000,REG7,CONST_0_ADDR);
    VoC_jump(main_decode_while);
    //       VoC_jump(main_decode_exit);    //   case MAD_FLOW_CONTINUE:     default:  continue;
//mad_frame_decode_10000:
//  VoC_sw16_d(REG6, bad_last_frame_ADDR);//    bad_last_frame = 0;
mad_frame_decode_l10a:

    VoC_jal(CII_reload_data_temp);

    VoC_lw16_d(REG4, STRUCT_MAD_HEADER_LAYER_ADDR);  //  define MAD_NSBSAMPLES(header)
    VoC_lw16_d(REG3, STRUCT_MAD_HEADER_FLAGS_ADDR); //  ((header)->layer == MAD_LAYER_I ? 12 :
    VoC_lw16i_short(REG5,12,DEFAULT);                              // (((header)->layer == MAD_LAYER_III &&
    VoC_be16_rd(NS_end, REG4,CONST_1_ADDR)               // ((header)->flags & MAD_FLAG_LSF_EXT)) ? 18 : 36))
    VoC_lw16i(REG5,36);                                                         // (header)->layer == MAD_LAYER_I
    VoC_bne16_rd(NS_end, REG4, CONST_3_ADDR)
    VoC_and16_ri(REG3,MAD_FLAG_LSF_EXT);
    VoC_bez16_r(NS_end, REG3)
    VoC_lw16i_short(REG5,18,DEFAULT);
NS_end:
    // synchronize the stream with the audio output
    // call voc
    VoC_lw16i_short(WRAP0,16,DEFAULT);
    VoC_sw16_d(REG5, GLOBAL_NS_ADDR);

    VoC_lw16i(REG2, OUTPUT_PCM_BUFFER1_ADDR);

    VoC_jal(CII_mad_synth_frame);
    VoC_lw16i_short(REG7,MAD_FLOW_CONTINUE,DEFAULT);
main_decode_exit:

    VoC_lw16_d(REG0,GLOBAL_ERR_STATUS);

    VoC_bnez16_r(main_decode_output_2,REG0);

    VoC_lw32_d(REG45,GLOBAL_NCH_ADDR); //REG4 : NCH, REG5 : NS

    VoC_lw16i(REG0,OUTPUT_PCM_BUFFER1_ADDR);     //REG0 : sample_lo_ptr
//  VoC_lw32_d(RL6,DEC_OUTSIDE_BUFFER_ADDR_ADDR);
    VoC_lw32_d(RL6,GLOBAL_STRM_BUF_OUT_PTR);

    VoC_lw16i_short(REG6,16,DEFAULT);
    VoC_mult16_rr(REG5,REG6,REG5,DEFAULT);     //REG5 : tc = ns*16;
    VoC_shr16_ri(REG0,1,DEFAULT); // for dma begin address/2
    VoC_be16_rd(main_decode_output_3,REG4,CONST_1_ADDR)
    VoC_shr16_ri(REG5,-1,DEFAULT);
main_decode_output_3:
    VoC_bez16_r(main_decode_output_2,REG5);
    VoC_sw32_d(REG45,GLOBAL_NCH_ADDR);        //REG4 : NCH, REG5 : tc


    VoC_sw16_d(REG5,CFG_DMA_SIZE);
    VoC_sw16_d(REG0,CFG_DMA_LADDR);
    VoC_sw32_d(RL6,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();

    // clear the event
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

main_decode_output_2:

    VoC_lw32_d(REG45,GLOBAL_NCH_ADDR);     // REG4 : NCH, REG5 : tc

    VoC_pop16(RA,DEFAULT);

    VoC_sw16_d(REG4,GLOBAL_NB_CHAN);     // GLOBAL_NB_CHAN

    VoC_return;
}

/**************************************************************************************
 * Function:    CII_reload_data_temp
 *
 * Description: reload data
 *
 * Inputs:      no
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.1  modify by  Xuml       03/29/2007
 **************************************************************************************/

void CII_reload_data_temp(void)
{
    // output:REG2:length
    VoC_lw16_d(REG7,STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR);
    VoC_and16_ri(REG7,0x7ff);
    VoC_lw16_d(REG6,GLOBLE_WRITE_POINTER_ADDR);
    VoC_sw16_d(REG7,STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR);

    // compute the circular buffer status
    VoC_sub16_rr(REG5,REG6,REG7,DEFAULT);
    VoC_bgtz16_r(CII_reload_data_temp_L0,REG5)
    VoC_add16_rd(REG5,REG5,CONST_2048_ADDR);
CII_reload_data_temp_L0:

    // check reload condition
    VoC_lw16i(REG3,1024);
    VoC_bgt16_rr(CII_reload_data_temp_L1,REG5,REG3)

    VoC_shr16_ri(REG6,2,DEFAULT);
    VoC_shr16_ri(REG7,2,IN_PARALLEL);
    VoC_sub16_rr(REG7,REG7,REG6,DEFAULT);

    VoC_bgtz16_r(CII_reload_data_temp_L2,REG7)
    VoC_lw16i(REG5,512);
    VoC_add16_rr(REG7,REG7,REG5,DEFAULT);
CII_reload_data_temp_L2:

    // REG6 : begin_address   REG7 : Length
    VoC_and16_ri(REG7,0xfffc); // for 4 int aligned

    // wrap on 2^10 short
    VoC_lw16i_short(REG4,10,DEFAULT);
    VoC_lw16i(REG5,0x1ff);

    VoC_push16(RA,DEFAULT);

    // ----------------------------
    // fct : vpp_AmjpDmaRdCircBuff
    // in :  REG4 -> local buffer wrap exponent
    //       REG5 -> local buffer wrap mask
    //       REG6 -> local buffer address
    //       REG7 -> load size (words)
    // out : REG5 -> new local buffer address
    // not modified : REG0
    // unused : REG012, ACC0, ACC1
    // ----------------------------

    // read data from external input circular buffer
    VoC_jal(vpp_AmjpCircBuffDmaRd);

    VoC_pop16(RA,DEFAULT);

    // store new local address
    VoC_sw16_d(REG5,GLOBLE_WRITE_POINTER_ADDR);

CII_reload_data_temp_L1:

    VoC_return;
}



void CII_mad_frame_mute(void)
{
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_set_inc(REG0, STATIC_MAD_SBSAMPLE_ADDR, 2);
    VoC_lw16i_set_inc(REG1, STATIC_MAD_OVERLAP_ADDR, 2);
    VoC_lw16i_set_inc(REG2, STATIC_MAD_SBSAMPLE_ADDR + 1152, 2);
    VoC_loop_i(1, 3)
    VoC_loop_i(0, 192)
    VoC_sw32inc_p(ACC0, REG0,DEFAULT);
    VoC_sw32inc_p(ACC0, REG1,DEFAULT);
    VoC_sw32inc_p(ACC0, REG2,DEFAULT);
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1
    VoC_return
}



/*
 * NAME:    header->decode()
 * DESCRIPTION: read the next frame header from the stream
 */
//return :REG7
/*
 * NAME:    header->decode()
 * DESCRIPTION: read the next frame header from the stream
 */
//return :REG7
void CII_mad_header_decode(void )
{
    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_push16(RA, DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);            //used for all saved 0
    VoC_lw16d_set_inc(REG2, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR,1);     //   ptr = stream->next_frame;
    VoC_lw16d_set_inc(REG3, GLOBLE_WRITE_POINTER_ADDR,1);    //    end = stream->bufend;
// REG2 ,    ptr               REG3  :    end
    VoC_lw16i_short(REG7,-1,DEFAULT);
    VoC_lw16i_short(REG6, MAD_ERROR_BUFPTR,IN_PARALLEL);
    VoC_bngt16_rd(mad_header_decode_100, REG2,CONST_2048_ADDR)   // if (ptr == 0) {
    VoC_sw16_d(REG6, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);           //      stream->error = MAD_ERROR_BUFPTR;
    VoC_jump(mad_header_decode_fail);                                                     //          goto fail;
mad_header_decode_100:
    /* stream skip */
    /*
     if (stream->skiplen) {
       if (!stream->sync)
         ptr = stream->this_frame;

       if (end - ptr < stream->skiplen) {
         stream->skiplen   -= end - ptr;
         stream->next_frame = end;

         stream->error = MAD_ERROR_BUFLEN;
         goto fail;
       }

       ptr += stream->skiplen;
       stream->skiplen = 0;

       stream->sync = 1;
     }
    */
    VoC_bez16_d(mad_header_decode_skip, STRUCT_MAD_STREAM_SKIPLEN_ADDR)

    VoC_bnez16_d(mad_header_decode_skip_100, STRUCT_MAD_STREAM_SYNC_ADDR)
    VoC_lw16_d(REG2, STRUCT_MAD_STREAM_THIS_FRAME_ADDR_ADDR);
mad_header_decode_skip_100:

    VoC_sub16_rr(REG4, REG3, REG2,DEFAULT);
    VoC_lw16i_short(REG6, MAD_ERROR_BUFLEN,IN_PARALLEL);
    VoC_bngtz16_r(mad_header_decode_skip_200, REG4)
    VoC_bnlt16_rd(mad_header_decode_skip_200, REG4, STRUCT_MAD_STREAM_SKIPLEN_ADDR)
    VoC_sub16_dr(REG4, STRUCT_MAD_STREAM_SKIPLEN_ADDR, REG4);
    VoC_blt16_rd(mad_frame_decode101,REG3,CONST_2048_ADDR)
    VoC_sub16_rd(REG3,REG3,CONST_2048_ADDR);
mad_frame_decode101:
    VoC_NOP();
    VoC_sw16_d(REG3, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR);
    VoC_sw16_d(REG4, STRUCT_MAD_STREAM_SKIPLEN_ADDR);
    VoC_sw16_d(REG6, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);
    VoC_jump(mad_header_decode_fail);
mad_header_decode_skip_200:
    VoC_NOP();
    VoC_add16_rd(REG2, REG2, STRUCT_MAD_STREAM_SKIPLEN_ADDR);
    VoC_sw16_d(RL7_HI, STRUCT_MAD_STREAM_SKIPLEN_ADDR);
    VoC_sw16_d(REG6, STRUCT_MAD_STREAM_SYNC_ADDR);    //    stream->sync = 1;
mad_header_decode_skip:
// sync:
    /* synchronize */

mad_header_decode_sync:
    // if (stream->sync) {
    VoC_bez16_d(mad_header_decode_sync_100, STRUCT_MAD_STREAM_SYNC_ADDR)
//    if (end - ptr < MAD_BUFFER_GUARD) {
    VoC_sub16_rr(REG4, REG3, REG2,DEFAULT);         //
    VoC_lw16i_short(REG6, MAD_ERROR_BUFLEN,IN_PARALLEL);
    VoC_bgt16_rd(mad_header_decode111,REG4,CONST_0_ADDR)
    VoC_add16_rd(REG4,REG4,CONST_2048_ADDR);
mad_header_decode111:

    VoC_bnlt16_rd(mad_header_decode_sync_200, REG4,CONST_8_ADDR)          //if (end - ptr < MAD_BUFFER_GUARD) {
    VoC_blt16_rd(mad_header_decode101,REG2,CONST_2048_ADDR)
    VoC_sub16_rd(REG2,REG2,CONST_2048_ADDR);
mad_header_decode101:
    VoC_NOP();
    VoC_sw16_d(REG2, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR);     // stream->next_frame = ptr;
    VoC_sw16_d(REG6, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);             // stream->error = MAD_ERROR_BUFLEN;
    VoC_jump(mad_header_decode_fail);                  //         goto fail;
mad_header_decode_sync_200:
//   else if (!(ptr[0] == 0xff && (ptr[1] & 0xe0) == 0xe0)) {
    /* mark point where frame sync word was expected */
    VoC_lw16i_short(WRAP1,10,DEFAULT);
    VoC_movreg16(REG1, REG2,DEFAULT);
    VoC_movreg16(REG4, REG2,IN_PARALLEL);
    VoC_shru16_ri(REG1, 1,DEFAULT);
    VoC_and16_ri(REG4, 1);

    VoC_lw16inc_p(REG5, REG1,DEFAULT);
    VoC_bez16_r(mad_header_decode_sync_even, REG4)
    VoC_shru16_ri(REG5,8,DEFAULT);
    VoC_lw16inc_p(REG4, REG1,IN_PARALLEL);
    VoC_shru16_ri(REG4,-8,DEFAULT);
    VoC_or16_rr(REG5, REG4,DEFAULT);
mad_header_decode_sync_even:
    VoC_lw16i_short(WRAP1,16,DEFAULT);
    VoC_and16_rd(REG5,   CONST_0xe0ff_ADDR);
    VoC_add16_rd(REG7, REG2,CONST_1_ADDR);
    VoC_lw16i(REG6, MAD_ERROR_LOSTSYNC);
    VoC_bne16_rd(no_mad_header_decode_sync_100_end, REG5,  CONST_0xe0ff_ADDR )
    VoC_jump(mad_header_decode_sync_100_end);
no_mad_header_decode_sync_100_end:

    VoC_blt16_rd(mad_header_decode102,REG7,CONST_2048_ADDR)
    VoC_sub16_rd(REG7,REG7,CONST_2048_ADDR);
mad_header_decode102:
    VoC_NOP();
    VoC_sw16_d(REG2, STRUCT_MAD_STREAM_THIS_FRAME_ADDR_ADDR);  //    stream->this_frame = ptr;
    VoC_sw16_d(REG7, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR);  //     stream->next_frame = ptr + 1;
    VoC_sw16_d(REG6, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);     //  stream->error = MAD_ERROR_LOSTSYNC;
    VoC_jump(mad_header_decode_fail);                                       //      goto fail;

mad_header_decode_sync_100:
    //else {
//  mad_bit_init(&stream->ptr, ptr);

    VoC_lw16i_short(BITCACHE,0,DEFAULT);
    VoC_lw32z(RL6,DEFAULT);
    VoC_movreg16(REG0, REG2,IN_PARALLEL);

    VoC_movreg16(REG4, REG2,DEFAULT);
    VoC_and16_ri(REG4,1);
    VoC_shru16_ri(REG0,1,DEFAULT);
    VoC_lw16i_short(WRAP0,10,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_bez16_r(mad_header_decode_mad_bit_init_even_100, REG4)
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,8,DEFAULT);
mad_header_decode_mad_bit_init_even_100:

    VoC_NOP();
    VoC_jal(CII_mad_stream_sync);
    VoC_lw16i_short(WRAP0,16,DEFAULT);

//   if (mad_stream_sync(stream) == -1) {

    VoC_be16_rd(no2_mad_header_decode_sync_100_end, REG7,CONST_neg1_ADDR)
    VoC_jump(mad_header_decode_sync_100_end);
no2_mad_header_decode_sync_100_end:
    VoC_lw16i(REG6, MAD_ERROR_BUFLEN);
    VoC_sub16_rd(REG7, REG3, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR);
    VoC_bgt16_rd(mad_header_decode104,REG7,CONST_0_ADDR)
    VoC_add16_rd(REG7,REG7,CONST_2048_ADDR);
mad_header_decode104:
    VoC_sub16_rd(REG4, REG3, CONST_8_ADDR);
    VoC_bgtz16_r(mad_header_decode_sync_403,REG4)
    VoC_add16_rd(REG4,REG4,CONST_2048_ADDR);
mad_header_decode_sync_403:
    VoC_bgtz16_r(mad_header_decode_sync_402,REG7)
    VoC_sub16_dr(REG7,CONST_0_ADDR,REG7);
mad_header_decode_sync_402:
    VoC_blt16_rd(mad_header_decode_sync_400, REG7,CONST_8_ADDR)         //  if (end - stream->next_frame >= MAD_BUFFER_GUARD)
    VoC_blt16_rd(mad_header_decode103,REG4,CONST_2048_ADDR)
    VoC_sub16_rd(REG4,REG4,CONST_2048_ADDR);
mad_header_decode103:
    VoC_NOP();
    VoC_sw16_d(REG4, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR);    // stream->next_frame = end - MAD_BUFFER_GUARD;
mad_header_decode_sync_400:
    VoC_sw16_d(REG6, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);    //   stream->error = MAD_ERROR_BUFLEN;
    VoC_jump(mad_header_decode_fail);                                       //      goto fail;
    //the  REG2 is  ptr return frome  CII_mad_stream_sync  functions!    //    ptr = mad_bit_nextbyte(&stream->ptr);
mad_header_decode_sync_100_end:
    /* begin processing */
// REG2   :     ptr
//REG3    :    end
    VoC_push16(REG3,DEFAULT);
//  stream->this_frame = ptr;
//  stream->next_frame = ptr + 1;  /* possibly bogus sync word */

    VoC_add16_rd(REG3, REG2,CONST_1_ADDR);
    VoC_blt16_rd(mad_header_decode105,REG3,CONST_2048_ADDR)
    VoC_sub16_rd(REG3,REG3,CONST_2048_ADDR);
mad_header_decode105:
    VoC_sw16_d(REG2, STRUCT_MAD_STREAM_THIS_FRAME_ADDR_ADDR);
    VoC_sw16_d(REG3, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR);

// mad_bit_init(&stream->ptr, stream->this_frame);
    VoC_lw16i_short(BITCACHE,0,DEFAULT);
    VoC_lw32z(RL6,DEFAULT);
    VoC_movreg16(REG0, REG2,IN_PARALLEL);
    VoC_and16_ri(REG2,1);
    VoC_shru16_ri(REG0,1,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_bez16_r(mad_header_decode_mad_bit_init_even, REG2)
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,8,DEFAULT);
mad_header_decode_mad_bit_init_even:
    VoC_lw16i_set_inc(REG2, STRUCT_MAD_HEADER_ADDR, 1);

    VoC_jal(CII_decode_header);

    VoC_sw16_d(REG0, STRUCT_BITPTR_PTR_BYTE_ADDR_ADDR);
    VoC_sw32_d(RL6, STRUCT_BITPTR_PTR_CACHE_ADDR);
    VoC_sw16_d(BITCACHE, STRUCT_BITPTR_PTR_LEFT_ADDR);

    VoC_sw16_d(REG5, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);
    VoC_pop16(REG3,DEFAULT);

//  if (decode_header(header, stream) == -1)
//    goto fail;
    VoC_bne16_rd(no_mad_header_decode_fail, REG1,CONST_neg1_ADDR);
    VoC_jump(mad_header_decode_fail);
no_mad_header_decode_fail:

    VoC_lw16i(REG6, MAD_ERROR_LOSTSYNC);
    VoC_bnez16_d(mad_header_bitrate , STRUCT_MAD_HEADER_BITRATE_ADDR)

//   stream->error = MAD_ERROR_LOSTSYNC;
    VoC_sw16_d(REG6, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);
    VoC_jump(mad_header_decode_fail);                      //      goto fail;

mad_header_bitrate:

    /* calculate beginning of next frame */
//  pad_slot = (header->flags & MAD_FLAG_PADDING) ? 1 : 0;

    VoC_lw16i_short(REG4, 1,DEFAULT);
    VoC_lw16_d(REG5, STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_and16_ri(REG5, MAD_FLAG_PADDING);
    VoC_bgtz16_r(mad_header_decode_pad_slot, REG5)
    VoC_lw16i_short(REG4, 0,DEFAULT);
mad_header_decode_pad_slot:
//REG4   :    pad_slot
//      VoC_setf32(0);
    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_lw32z(ACC1,DEFAULT);
    VoC_lw16_d(ACC1_LO, STRUCT_MAD_HEADER_SAMPLERATE_ADDR);
    VoC_lw16_d(REG6, STRUCT_MAD_HEADER_BITRATE_ADDR);

    VoC_lw16i_short(REG1, MAD_LAYER_III, DEFAULT);
    VoC_shru32_ri(ACC1,3,IN_PARALLEL);               //          /8

    VoC_lw16i(REG2,72);
    VoC_bne16_rd(mad_header_decode_no_MAD_LAYER_III, REG1, STRUCT_MAD_HEADER_LAYER_ADDR)
    VoC_lw16_d(REG5, STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_and16_ri(REG5, MAD_FLAG_LSF_EXT);
    VoC_bgtz16_r(mad_header_decode_no_MAD_LAYER_72, REG5)
mad_header_decode_no_MAD_LAYER_III:
    VoC_shr16_ri(REG2,-1,DEFAULT);
mad_header_decode_no_MAD_LAYER_72:
    //REG2   :    slots_per_frame

    VoC_lw16i_short(REG1, MAD_LAYER_I, DEFAULT);
    //if (header->layer == MAD_LAYER_I)
    VoC_bne16_rd(mad_header_decode_no_MAD_LAYER_I, REG1, STRUCT_MAD_HEADER_LAYER_ADDR);
    // N = ((12 * header->bitrate*1000 / header->samplerate) + pad_slot) * 4;
    VoC_lw16i_short(REG2, 12, DEFAULT);
mad_header_decode_no_MAD_LAYER_I:
    /*
     else {
       unsigned int slots_per_frame;

       slots_per_frame = (header->layer == MAD_LAYER_III &&
              (header->flags & MAD_FLAG_LSF_EXT)) ? 72 : 144;

       N = (slots_per_frame * header->bitrate*1000 / header->samplerate) + pad_slot;
     }
      */
    VoC_lw16i(REG7,125);
    VoC_mult16_rr(REG2, REG2, REG7,DEFAULT);
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_multf32_rr(ACC0, REG6,REG2,DEFAULT);
    VoC_NOP();
mad_header_decode_div_102:
    VoC_bgt32_rr(mad_header_decode_div_mode101, ACC1,ACC0)
    VoC_sub32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_add16_rd(REG5,REG5,CONST_1_ADDR);
    VoC_jump(mad_header_decode_div_102);
mad_header_decode_div_mode101:

    VoC_add16_rr(REG5, REG5, REG4,DEFAULT);
    VoC_bne16_rd(mad_header_decode_no_MAD_LAYER_I_2, REG1, STRUCT_MAD_HEADER_LAYER_ADDR);
    VoC_shr16_ri(REG5, -2,DEFAULT);
mad_header_decode_no_MAD_LAYER_I_2:
//      VoC_add16_rd(REG5,REG5,STRUCT_MAD_STREAM_THIS_FRAME_ADDR_ADDR);
//      VoC_setf32(-1);
    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
    //REG5    :     N
    //REG3   :     end

    /* verify there is enough data left in buffer to decode this frame */
    /**/
    VoC_add16_rd(REG4, REG5,CONST_8_ADDR);
    VoC_sub16_rd(REG2, REG3, STRUCT_MAD_STREAM_THIS_FRAME_ADDR_ADDR);
    VoC_bgtz16_r(MAD_HEADER_101,REG2)
    VoC_add16_rd(REG2,REG2,CONST_2048_ADDR);
MAD_HEADER_101:
    VoC_lw16i(REG6, MAD_ERROR_BUFLEN);
    VoC_lw16_d(REG7, STRUCT_MAD_STREAM_THIS_FRAME_ADDR_ADDR);
    VoC_add16_rr(REG5, REG5, REG7,DEFAULT); //  stream->this_frame + N;
// if (N + MAD_BUFFER_GUARD > end - stream->this_frame) {
    VoC_bngt16_rr(mad_header_N, REG4, REG2)
    VoC_blt16_rd(mad_header_decode106,REG7,CONST_2048_ADDR)
    VoC_sub16_rd(REG7,REG7,CONST_2048_ADDR);
mad_header_decode106:
    VoC_sw16_d(REG6, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);  //      stream->error = MAD_ERROR_BUFLEN;
    VoC_sw16_d(REG7, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR);  // stream->next_frame = stream->this_frame;
    VoC_jump(mad_header_decode_fail);                      //       goto fail;
    //}
mad_header_N:
//  stream->next_frame = stream->this_frame + N;
    VoC_bngt16_rd(MAD_HEADER102,REG5,CONST_2048_ADDR)
    VoC_sub16_rd(REG5,REG5,CONST_2048_ADDR);
MAD_HEADER102:
    VoC_NOP();
    VoC_sw16_d(REG5, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR);
    /* check that a valid frame header follows this frame */
    /*  if (!stream->sync) {

    ptr = stream->next_frame;
      if (!(ptr[0] == 0xff && (ptr[1] & 0xe0) == 0xe0)) {
        ptr = stream->next_frame = stream->this_frame + 1;
        goto sync;
      }

      stream->sync = 1;
    }
    */
    VoC_lw16_d(REG5, STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_or16_ri(REG5, MAD_FLAG_INCOMPLETE);       //  header->flags |= MAD_FLAG_INCOMPLETE;

    VoC_bnez16_d(mad_header_sync, STRUCT_MAD_STREAM_SYNC_ADDR)
    VoC_lw16d_set_inc(REG2, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR,1);
    VoC_movreg16(REG4, REG2,DEFAULT);
    VoC_movreg16(REG1, REG2,IN_PARALLEL);

    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_shru16_ri(REG1, 1,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_and16_ri(REG4, 1);
    VoC_lw16inc_p(REG7, REG1,DEFAULT);
    VoC_bez16_r(mad_header_decode_sync_even_333, REG4)
    VoC_shru16_ri(REG7,8,DEFAULT);
    VoC_lw16inc_p(REG4, REG1,IN_PARALLEL);
    VoC_shru16_ri(REG4,-8,DEFAULT);
    VoC_or16_rr(REG7, REG4,DEFAULT);
mad_header_decode_sync_even_333:
    VoC_and16_rd(REG7, CONST_0xe0ff_ADDR);
    VoC_lw16_d(REG4, STRUCT_MAD_STREAM_THIS_FRAME_ADDR_ADDR);
    VoC_add16_rd(REG2, REG4,CONST_1_ADDR);    //   ptr = stream->next_frame = stream->this_frame + 1;
    VoC_be16_rd(mad_header_sync_100, REG7, CONST_0xe0ff_ADDR)
    VoC_blt16_rd(mad_header_decode108,REG2,CONST_2048_ADDR)
    VoC_sub16_rd(REG2,REG2,CONST_2048_ADDR);
mad_header_decode108:
    VoC_NOP();
    VoC_sw16_d(REG2, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR);
    VoC_jump(mad_header_decode_sync);
mad_header_sync_100:
    VoC_sw16_d(REG6, STRUCT_MAD_STREAM_SYNC_ADDR);
mad_header_sync:
    VoC_sw16_d(REG5, STRUCT_MAD_HEADER_FLAGS_ADDR);        // header->flags |= MAD_FLAG_INCOMPLETE;
    VoC_lw16i_short(REG7, 0,DEFAULT);
    VoC_jump(mad_header_decode_return0);   //    return 0;
mad_header_decode_fail:
    VoC_lw16i_short(REG6, 0,DEFAULT);
    VoC_lw16i_short(REG7, -1,DEFAULT);                                          // fail:
    VoC_sw16_d(REG6, STRUCT_MAD_STREAM_SYNC_ADDR);           // stream->sync = 0;   return -1;
mad_header_decode_return0:
    VoC_pop16(RA, DEFAULT);
    VoC_NOP();
    VoC_return;
}

/*
 * NAME:    decode_header()
 * DESCRIPTION: read header data and following CRC word
 * INPUT:   REG2      STRUCT_MAD_HEADER_ADDR
 * OUTPUT: REG5 for stream->error REG1 for return value
 */
void CII_decode_header(void)
{
    //  header->flags        = 0;
    //  header->private_bits = 0;
    /* header() */
    VoC_lw16i_short(REG1,-1,DEFAULT);     // return -1;
    VoC_add16_rd(REG3, REG2,CONST_8_ADDR); //point to  header->flags & private_bits
    VoC_lbinc_p(REG0);
    /* syncword */
    //  mad_bit_skip(&stream->ptr, 11);
    VoC_rbinc_i(REG4,11,DEFAULT);
    VoC_lw32z(REG67,IN_PARALLEL);  // REG6 for flags   // REG7 for private_bits
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,1,DEFAULT);
    /* MPEG 2.5 indicator (really part of syncword) */
    // if (mad_bit_read(&stream->ptr, 1) == 0)
    VoC_bnez16_r(decode_header101, REG4)
    VoC_or16_ri(REG6,0X4000);       // header->flags |= MAD_FLAG_MPEG_2_5_EXT;
decode_header101:
    /* ID */
    //  if (mad_bit_read(&stream->ptr, 1) == 0)
    VoC_rbinc_i(REG4,1,DEFAULT);
    VoC_bnez16_r(decode_header102, REG4)
    VoC_or16_ri(REG6,0X1000);    //header->flags |= MAD_FLAG_LSF_EXT;
    VoC_jump(decode_header103);
decode_header102:
    // else if (header->flags & MAD_FLAG_MPEG_2_5_EXT) {
    VoC_movreg16(REG5,REG6,DEFAULT);
    VoC_and16_ri(REG5,0X4000);
    VoC_bez16_r(decode_header103, REG5)
    VoC_lw16i(REG5,0X101);  //   stream->error = MAD_ERROR_LOSTSYNC;
    VoC_sw32_p(REG67,REG3,DEFAULT);                    // VoC_sw32_d(REG67,STRUCT_MAD_HEADER_FLAGS_ADDR);

    VoC_return;
decode_header103:

    VoC_rbinc_i(REG4,2,DEFAULT);
    /* layer */
    // header->layer = 4 - mad_bit_read(&stream->ptr, 2);
    VoC_sub16_dr(REG5,CONST_4_ADDR,REG4);   // REG5 for layer
    // if (header->layer == 4) {
    VoC_bnez16_r(decode_header104,REG4)
    VoC_lw16i(REG5,0X102);  //      stream->error = MAD_ERROR_BADLAYER;
    VoC_sw32_p(REG67,REG3,DEFAULT);     //VoC_sw32_d(REG67,STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_return;     // return -1;
decode_header104:
    //VoC_sw16_p(REG5,STRUCT_MAD_HEADER_LAYER_ADDR);
    VoC_sw16_p(REG5,REG2,DEFAULT);
    /* protection_bit */
    // if (mad_bit_read(&stream->ptr, 1) == 0) {
    VoC_rbinc_i(REG4,1,DEFAULT);
    VoC_push16(REG5,IN_PARALLEL);
    VoC_bnez16_r(decode_header105, REG4)
    VoC_or16_ri(REG6,0X0010);    // header->flags    |= MAD_FLAG_PROTECTION;
    // header->crc_check = mad_bit_crc(stream->ptr, 16, 0xffff);

    VoC_push16(REG0,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);
    VoC_push16(BITCACHE, DEFAULT);
    VoC_push32(REG67,IN_PARALLEL);

    VoC_push16(RA,DEFAULT);
    VoC_push32(REG23,IN_PARALLEL);

    VoC_jal(CII_creat_crc_table);

    VoC_lw16i_short(REG7,16, DEFAULT);
    VoC_lw16i_short(REG4,-1, IN_PARALLEL);

    VoC_push16(REG1,DEFAULT);  //xuml add
    VoC_jal(CII_mad_bit_crc);
    VoC_pop16(REG1,DEFAULT);  //xuml add

    VoC_pop16(RA,DEFAULT);
    VoC_pop32(REG23,IN_PARALLEL);
    VoC_add16_rd(REG4, REG2,CONST_6_ADDR);

    VoC_pop16(BITCACHE, DEFAULT);//, "BITCACHE"
    VoC_pop32(REG67,IN_PARALLEL);
    // VoC_sw16_d(RL7_LO, STRUCT_MAD_HEADER_CRC_CHECK_ADDR);  // // header->crc_check = mad_bit_crc(stream->ptr, 16, 0xffff);
    VoC_sw16_p(RL7_LO, REG4, DEFAULT );
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_pop16(REG0,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);

decode_header105:
    /* bitrate_index */
    // index = mad_bit_read(&stream->ptr, 4);
    VoC_rbinc_i(REG4,4,DEFAULT);        // REG4 for index
    VoC_pop16(REG5,IN_PARALLEL);
    // if (index == 15) {
    VoC_bne16_rd(decode_header106,REG4,CONST_15_ADDR)
    VoC_lw16i(REG5,0X103);  //     stream->error = MAD_ERROR_BADBITRATE;
    //VoC_sw32_d(REG67,STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_sw32_p(REG67,REG3,DEFAULT);
    VoC_return;     // return -1;
decode_header106:
    /*  if (header->flags & MAD_FLAG_LSF_EXT)
        header->bitrate = bitrate_table[3 + (header->layer >> 1)][index];
      else
        header->bitrate = bitrate_table[header->layer - 1][index]; */

    VoC_push32(REG23,DEFAULT);
    VoC_movreg16(REG2,REG6,IN_PARALLEL);
    VoC_and16_ri(REG2,MAD_FLAG_LSF_EXT);

    VoC_sub16_rd(REG3,REG5,CONST_1_ADDR);
    VoC_bez16_r(decode_header107, REG2)
    VoC_movreg16(REG3,REG5,DEFAULT);
    VoC_shr16_ri(REG3,1,DEFAULT);
    VoC_add16_rd(REG3,REG3,CONST_3_ADDR);
decode_header107:

    VoC_mult16_rd(REG3,REG3,CONST_16_ADDR);
    VoC_lw16i(REG2,TABLE_bitrate_table_ADDR);
    VoC_NOP();
    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_add16_rr(REG5,REG3,REG2,DEFAULT);
    /* sampling_frequency */
    //  index = mad_bit_read(&stream->ptr, 2);
    VoC_rbinc_i(REG4,2,DEFAULT);        // REG4 for index
    VoC_pop32(REG23,IN_PARALLEL);
    VoC_lw16_p(ACC1_LO,REG5,DEFAULT);   // REG5 for bitrate
    VoC_sw32_p(REG67,REG3,DEFAULT);
    VoC_sw16_d(ACC1_LO,GLOBAL_BIT_RATE);       //new add by wangbin 2007.9.12
    //  if (index == 3) {
    VoC_bne16_rd(decode_header108,REG4,CONST_3_ADDR)
    VoC_lw16i(REG5,0X104);  //      stream->error = MAD_ERROR_BADSAMPLERATE;
//  VoC_lw16i_short(REG1,0XFFFF,DEFAULT);
    VoC_return;      // return -1;
decode_header108:
    //  header->samplerate = samplerate_table[index];

    VoC_lw16i(REG1,44100);
    VoC_bez16_r(decode_header_samplerate,REG4)
    VoC_lw16i(REG1,48000);
    VoC_be16_rd(decode_header_samplerate,REG4,CONST_1_ADDR)
    VoC_lw16i(REG1,32000);
decode_header_samplerate:           // REG1 for samplerate

    VoC_movreg16(REG5,REG6,DEFAULT);
    VoC_and16_ri(REG5,0X1000);                  // if (header->flags & MAD_FLAG_LSF_EXT) {
    VoC_bez16_r(decode_header109,REG5)
    VoC_shru16_ri(REG1,1,DEFAULT);          // header->samplerate /= 2;
    VoC_movreg16(REG5,REG6,DEFAULT);
    VoC_and16_ri(REG5,0X4000);
    VoC_bez16_r(decode_header109,REG5)      // if (header->flags & MAD_FLAG_MPEG_2_5_EXT)
    VoC_shr16_ri(REG1,1,DEFAULT);       // header->samplerate /= 2;
decode_header109:
    /* padding_bit */
    VoC_lw32z(ACC0,DEFAULT);                     //new add by wangbin 2007.11.13
    VoC_movreg16(ACC0_LO,REG1,DEFAULT);          //new add by wangbin 2007.11.13
    VoC_rbinc_i(REG4,1,DEFAULT);
    VoC_movreg16(ACC1_HI, REG1,IN_PARALLEL);
    VoC_sw32_d(ACC0,GLOBAL_SAMPLE_RATE);       //new add by wangbin 2007.11.13

    VoC_add16_rd(REG5, REG2, CONST_4_ADDR);
    VoC_bez16_r(decode_header110,REG4)          //  if (mad_bit_read(&stream->ptr, 1))
    VoC_or16_ri(REG6,0X0080);               //    header->flags |= MAD_FLAG_PADDING;
decode_header110:
    VoC_sw32_p(ACC1,REG5 ,DEFAULT);            // STRUCT_MAD_HEADER_BITRATE_ADDR &  STRUCT_MAD_HEADER_SAMPLERATE_ADDR
    /* private_bit */
    VoC_rbinc_i(REG4,1,DEFAULT);
//  VoC_sw16_d(REG1,STRUCT_MAD_HEADER_SAMPLERATE_ADDR);
    VoC_bez16_r(decode_header111,REG4)          //  if (mad_bit_read(&stream->ptr, 1))
    VoC_or16_ri(REG7,0X0100);               //    header->private_bits |= MAD_PRIVATE_HEADER;
decode_header111:
    /* mode */
    VoC_rbinc_i(REG4,2,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_sub16_dr(REG4,CONST_3_ADDR,REG4);       // header->mode = 3 - mad_bit_read(&stream->ptr, 2);
    /* mode_extension */
    VoC_rbinc_i(REG5,2,DEFAULT);        //  header->mode_extension = mad_bit_read(&stream->ptr, 2);
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_sw16inc_p(REG5,REG2,DEFAULT);

    /* copyright */
    VoC_rbinc_i(REG4,1,DEFAULT);
    VoC_bez16_r(decode_header112,REG4)          //  if (mad_bit_read(&stream->ptr, 1))
    VoC_or16_ri(REG6,0X0020);               //    header->flags |= MAD_FLAG_COPYRIGHT;
decode_header112:
    /* original/copy */
    VoC_rbinc_i(REG4,1,DEFAULT);
    VoC_bez16_r(decode_header113,REG4)          //  if (mad_bit_read(&stream->ptr, 1))
    VoC_or16_ri(REG6,0X0040);               //    header->flags |= MAD_FLAG_ORIGINAL;
decode_header113:
    /* emphasis */
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_rbinc_i(REG4,2,DEFAULT);        //  header->emphasis = mad_bit_read(&stream->ptr, 2);
    VoC_bne16_rd(decode_header114,REG4,CONST_2_ADDR)    //  if (header->emphasis == 2)
    VoC_lw16i(REG5,0X0105);                     //  stream->error = MAD_ERROR_BADEMPHASIS;
decode_header114:

    //VoC_sw16_d(REG4,STRUCT_MAD_HEADER_EMPHASIS_ADDR);
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    /* crc_check */
    VoC_add16_rd(REG2, REG2,CONST_3_ADDR);
    VoC_movreg16(REG1,REG6,DEFAULT);
    VoC_and16_ri(REG1,0X0010);
    VoC_lbinc_p(REG0);
    VoC_bez16_r(decode_header115,REG1)          // if (header->flags & MAD_FLAG_PROTECTION)
    VoC_rbinc_i(REG4,16,DEFAULT);                       // header->crc_target = mad_bit_read(&stream->ptr, 16);
    VoC_lbinc_p(REG0);
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
decode_header115:
    VoC_sw32_p(REG67,REG3,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    VoC_return;
}


/*
 * NAME:    stream->sync()
 * DESCRIPTION: locate the next stream sync word
 */
//Input : REG0 ->STRUCT_BITPTR_PTR_BYTE_ADDR_ADDR
//           REG2  ->     ptr
//               REG3  ->     end
//output :  REG7
//used  : REG025,REG7,RL6
//REG3 not change
void CII_mad_stream_sync( void)
{
    //   ptr = mad_bit_nextbyte(&stream->ptr);
    //  end = stream->bufend;
    /*
      while (ptr < end - 1 &&
         !(ptr[0] == 0xff && (ptr[1] & 0xe0) == 0xe0))
        ++ptr;
      */
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG6,8,DEFAULT);
mad_stream_sync_200:
//  VoC_NOP();
    VoC_lbinc_p(REG0);
    VoC_inc_p(REG2,DEFAULT);
    VoC_blt16_rd(mad_stream_sync_201,REG2,CONST_2048_ADDR)
    VoC_sub16_rd(REG2,REG2,CONST_2048_ADDR);
mad_stream_sync_201:
    VoC_rbinc_i(REG7,8,DEFAULT);
    VoC_shru16_ri(REG7, -8,DEFAULT);
    VoC_or16_rr(REG7, REG6,DEFAULT);
    VoC_movreg16(REG6, REG7,DEFAULT);
    VoC_shru16_ri(REG6, 8,DEFAULT);

    VoC_and16_rd(REG7,CONST_0xe0ff_ADDR);
    VoC_sub16_rr(REG5,REG3,REG2,DEFAULT);
    VoC_bltz16_r(mad_stream_sync_101,REG5)
    VoC_bngt16_rd(mad_stream_sync_100, REG5, CONST_1_ADDR)
mad_stream_sync_101:
    VoC_add16_rd(REG5,REG5,CONST_2048_ADDR);
    VoC_bngt16_rd(mad_stream_sync_100, REG5, CONST_1_ADDR)
    VoC_bne16_rd(mad_stream_sync_200, REG7, CONST_0xe0ff_ADDR);
mad_stream_sync_100:
    VoC_sub16_rd(REG2, REG2,CONST_1_ADDR);

    VoC_sub16_rr(REG5, REG3, REG2,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_bltz16_r(mad_stream_sync_301,REG5)
    VoC_bnlt16_rd(mad_stream_sync_300, REG5,CONST_8_ADDR)      // if (end - ptr < MAD_BUFFER_GUARD)
    VoC_jump(mad_stream_sync_302);
mad_stream_sync_301:
    VoC_add16_rd(REG5,REG5,CONST_2048_ADDR);
    VoC_bnlt16_rd(mad_stream_sync_300, REG5,CONST_8_ADDR)      // if (end - ptr < MAD_BUFFER_GUARD)
mad_stream_sync_302:
    VoC_lw16i_short(REG7,-1,DEFAULT);                                         //   return -1;
    VoC_return;
mad_stream_sync_300:
    //  mad_bit_init(&stream->ptr, ptr);
    VoC_sw16_d(REG0, STRUCT_BITPTR_PTR_BYTE_ADDR_ADDR);
    VoC_sw32_d(RL6, STRUCT_BITPTR_PTR_CACHE_ADDR);
    VoC_sw16_d(BITCACHE, STRUCT_BITPTR_PTR_LEFT_ADDR);
    VoC_return;               // return 0;
}

/*
 * NAME:    bit->crc()
 * DESCRIPTION: compute CRC-check word
 */
//input :
//REG7  :  si_len
//REG0   :  stream->ptr
//REG4  :  init

//output :
//        RL7_lo

void CII_mad_bit_crc( void )
{
    /*  for (crc = init; len >= 32; len -= 32) {
       register unsigned long data;

       data = mad_bit_read(&bitptr, 32);

       crc = (crc << 8) ^ crc_table[((crc >> 8) ^ (data >> 24)) & 0xff];
       crc = (crc << 8) ^ crc_table[((crc >> 8) ^ (data >> 16)) & 0xff];
       crc = (crc << 8) ^ crc_table[((crc >> 8) ^ (data >>  8)) & 0xff];
       crc = (crc << 8) ^ crc_table[((crc >> 8) ^ (data >>  0)) & 0xff];
     }
    */
    VoC_push16(REG7,DEFAULT);
    VoC_lw16i_short(REG5, 0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2, 24,-8);

    VoC_shru16_ri(REG7,5,DEFAULT);
    VoC_movreg32(RL7, REG45,IN_PARALLEL);

//  VoC_lw16i_set_inc(REG1, TABLE_crc_table_ADDR,2);
    VoC_lw16i_set_inc(REG1, OUTPUT_PCM_BUFFER1_ADDR,2);
    VoC_bez16_r(mad_bit_crc_200, REG7)
    VoC_loop_r(1,REG7)
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG7,16,DEFAULT);
    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,16,DEFAULT);             //   data = mad_bit_read(&bitptr, 32);
    VoC_loop_i_short(4,DEFAULT)

    VoC_startloop0
    VoC_movreg32(ACC0,REG67, DEFAULT);
    VoC_movreg32(REG45,RL7, IN_PARALLEL);   //  crc = init;
    VoC_shru32_rr(ACC0, REG2,DEFAULT);
    VoC_shru32_ri(REG45, 8,IN_PARALLEL);
    VoC_xor32_rr(REG45, ACC0,DEFAULT);
    VoC_shru32_ri(RL7, -8,IN_PARALLEL);
    VoC_and16_ri(REG4, 0xff );
    VoC_add16_rr(REG4, REG1, REG4,DEFAULT);
    VoC_inc_p(REG2,DEFAULT);
    VoC_lw16i_short(REG5, 0,IN_PARALLEL);
    VoC_lw16_p(REG4, REG4,DEFAULT);
    VoC_xor32_rr(RL7,REG45,DEFAULT);
    VoC_endloop0
    VoC_lw16i_short(REG2,24,DEFAULT);
    VoC_endloop1
    //RL7   :   crc
    /*
      switch (len / 8) {
      case 3: crc = (crc << 8) ^
            crc_table[((crc >> 8) ^ mad_bit_read(&bitptr, 8)) & 0xff];
      case 2: crc = (crc << 8) ^
            crc_table[((crc >> 8) ^ mad_bit_read(&bitptr, 8)) & 0xff];
      case 1: crc = (crc << 8) ^
            crc_table[((crc >> 8) ^ mad_bit_read(&bitptr, 8)) & 0xff];

      len %= 8;

      case 0: break;
      }
    */
mad_bit_crc_200:
    VoC_pop16(REG7,DEFAULT);//,    "len "
    VoC_lw16i_short(REG5, 0,IN_PARALLEL);

    VoC_and16_ri(REG7,0x1f);
    VoC_movreg16(REG2, REG7,DEFAULT);
    VoC_shru16_ri(REG7,3,IN_PARALLEL);
    VoC_bez16_r(mad_bit_crc_100, REG7)
    VoC_lbinc_p(REG0);

    VoC_loop_r_short(REG7,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_startloop0
    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_movreg32(REG45,RL7, DEFAULT);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_shru32_ri(REG45, 8,IN_PARALLEL);
    VoC_xor32_rr(REG45, REG67,DEFAULT);
    VoC_shru32_ri(RL7, -8,IN_PARALLEL);
    VoC_and16_ri(REG4, 0xff );
    VoC_add16_rr(REG4, REG1, REG4,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_lbinc_p(REG0);

    VoC_lw16_p(REG4, REG4,DEFAULT);
    VoC_xor32_rr(RL7,REG45,DEFAULT);
    VoC_endloop0
mad_bit_crc_100:

    /*
      while (len--) {
        register unsigned int msb;

        msb = mad_bit_read(&bitptr, 1) ^ (crc >> 15);

        crc <<= 1;
        if (msb & 1)
          crc ^= CRC_POLY;
      }
    */
    VoC_and16_ri(REG2,0x7);
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_bez16_r(mad_bit_crc_100_end, REG2)
    VoC_loop_r_short(REG2,DEFAULT);
    VoC_movreg32(REG45,RL7, IN_PARALLEL);
    VoC_startloop0
    VoC_rbinc_i(REG6,1,DEFAULT);
    VoC_shru32_ri(RL7, -1,IN_PARALLEL);

    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_shru32_ri(REG45, 15,IN_PARALLEL);

    VoC_xor32_rr(REG45, REG67,DEFAULT);
    VoC_and16_rr(REG4,REG2,DEFAULT);
    VoC_bez16_r(mad_bit_crc_100_end_9, REG4)
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i(ACC0_LO,0x8005);
    VoC_xor32_rr(RL7, ACC0,DEFAULT);
mad_bit_crc_100_end_9:
    VoC_movreg32(REG45,RL7, DEFAULT);
    VoC_endloop0
mad_bit_crc_100_end:
    VoC_return                //  return crc & 0xffff;
    //  RL7_lo  is   crc
}

void CII_creat_crc_table(void)
{
    /*
       for(i=0x80; i!=0; i>>=1)
             {
           if((crc&0x8000)!=0) {crc<<=1; crc^=P;}
               else crc<<=1;
          if((*ptr&i)!=0) crc^=P;
             }
    */
    VoC_lw16i_set_inc(REG2,OUTPUT_PCM_BUFFER1_ADDR,1);
    VoC_lw16i_short(REG0,0,DEFAULT);

creat_crc_table_lable100:
    VoC_lw16i(REG3,0x80);
    VoC_lw16i_short(REG4,0,DEFAULT);
creat_crc_table_lable200:
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i(REG6,0x8000);
    VoC_and16_rr(REG6,REG4,DEFAULT);
    VoC_movreg16(ACC0_LO,REG4,IN_PARALLEL);
    //        VoC_shru16_ri(REG4,-1,DEFAULT);
    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_movreg16(REG4,ACC0_LO,DEFAULT);
    VoC_bez16_r(creat_crc_table_lable300,REG6)
    VoC_xor16_ri(REG4,0x8005);

creat_crc_table_lable300:
    VoC_movreg16(REG7,REG0,DEFAULT);
    VoC_and16_rr(REG7,REG3,DEFAULT);
    VoC_bez16_r(creat_crc_table_lable400,REG7)
    VoC_xor16_ri(REG4,0x8005);
creat_crc_table_lable400:
    //  VoC_shru16_ri(REG3,1,DEFAULT);
    VoC_shr16_ri(REG3,1,DEFAULT);
    VoC_bnez16_r(creat_crc_table_lable200,REG3)
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_1_ADDR);
    VoC_lw16i(REG4,256);
//  VoC_blt16_rd(creat_crc_table_lable100,REG0,CONST_256_ADDR)
    VoC_bgt16_rr(creat_crc_table_lable100,REG4,REG0)
    VoC_return;
}


void  Coolsand_layer12_code_reload( void )
{
    /*****************  DMA mpeg layer 1 and 2 Const ************************/
    VoC_lw32_d(ACC0,GLOBAL_MP12_CODE_PTR);
    VoC_lw16i(REG6,SECTION_1_START);//local addr
    VoC_lw16i(REG7,576);//const size

    VoC_sw16_d(REG6,CFG_DMA_LADDR);
    VoC_sw16_d(REG7,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    /*****************  DMA mpeg layer 1 and 2 Const ************************/
    VoC_return

}






/*
 * NAME:    frame->decode()
 * DESCRIPTION: decode a single frame from a bitstream
 */
//return   REG7
void  CII_mad_frame_decode( void )
{

    VoC_push16(RA, DEFAULT);
    VoC_lw16_d(REG6,STRUCT_MAD_STREAM_OPTIONS_ADDR);
    VoC_lw16_d(REG7,STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_sw16_d(REG6,STRUCT_MAD_FRAME_OPTIONS_ADDR);   //    frame->options = stream->options;
    /* header() */
    /* error_check() */
    VoC_and16_ri(REG7,MAD_FLAG_INCOMPLETE );
    VoC_bgtz16_r(mad_frame_decode_audio_data, REG7)               //    if (!(frame->header.flags & MAD_FLAG_INCOMPLETE) &&
    VoC_jal(CII_mad_header_decode);                                    //       mad_header_decode(&frame->header, stream) == -1)
    VoC_bne16_rd(mad_frame_decode_audio_data, REG7,CONST_neg1_ADDR)//  goto fail;
    VoC_jump(mad_frame_decode_fail);
mad_frame_decode_audio_data:

    /* audio_data() */
    VoC_lw16_d(REG7,STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_and16_ri(REG7, ~MAD_FLAG_INCOMPLETE);
    VoC_lw16_d(REG6,STRUCT_MAD_HEADER_LAYER_ADDR);
    VoC_sw16_d(REG7,STRUCT_MAD_HEADER_FLAGS_ADDR);   //   frame->header.flags &= ~MAD_FLAG_INCOMPLETE;

    /*
      if (decoder_table[frame->header.layer - 1](stream, frame) == -1) {
        if (!MAD_RECOVERABLE(stream->error))
          stream->next_frame = stream->this_frame;

        goto fail;
      }
    */


    VoC_bne16_rd(mad_frame_decode_no_I, REG6,CONST_1_ADDR)
    VoC_jal(Coolsand_layer12_code_reload);
    VoC_jal(CII_mad_layer_I);
    VoC_jump(mad_frame_decode_audio_data_end)
mad_frame_decode_no_I:
    VoC_bne16_rd(mad_frame_decode_no_II, REG6,CONST_2_ADDR)
    VoC_jal(Coolsand_layer12_code_reload);
    VoC_jal(CII_mad_layer_II);
    VoC_jump(mad_frame_decode_audio_data_end)
mad_frame_decode_no_II:
    VoC_jal(CII_mad_layer_III);    // maybe the work for elisa
    VoC_lw16_d(REG1, mad_layer_III_result);
mad_frame_decode_audio_data_end:

    VoC_bne16_rd(mad_frame_decode_go_ancillary_data, REG1,CONST_neg1_ADDR)
    VoC_lw16_d(REG6, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);
    VoC_and16_ri(REG6,0xff00);
    VoC_lw16_d(REG7, STRUCT_MAD_STREAM_THIS_FRAME_ADDR_ADDR);
    VoC_bgtz16_r(mad_frame_decode_fail, REG6)
    VoC_sw16_d(REG7, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR);
    VoC_jump(mad_frame_decode_fail);
mad_frame_decode_go_ancillary_data:

    /* ancillary_data() */

    /*  if (frame->header.layer != MAD_LAYER_III) {
        struct mad_bitptr next_frame;

        mad_bit_init(&next_frame, stream->next_frame);

        stream->anc_ptr    = stream->ptr;
        stream->anc_bitlen = mad_bit_length(&stream->ptr, &next_frame);

        mad_bit_finish(&next_frame);
      }
    */
    VoC_pop16(RA, DEFAULT);
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_return             //         return 0;

mad_frame_decode_fail:
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_pop16(RA, DEFAULT);
    VoC_lw16i_short(REG7,-1,IN_PARALLEL);
    VoC_sw16_d(REG6,STRUCT_MAD_STREAM_ANC_BITLEN_ADDR);   //   stream->anc_bitlen = 0;
    VoC_return              //   return -1;
}



//input REG6 / REG7
//output   REG5 is REG6/REG7     ; REG6 = REG6%REG7
void CII_div_mode(void)
{
    VoC_push16(REG4,DEFAULT);
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);
CII_div_mode102:
//  VoC_loop_i(0,255)
    VoC_bgt16_rr(CII_div_mode101, REG7,REG6)
    VoC_sub16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG4 ,IN_PARALLEL);
//  VoC_endloop0
    VoC_jump(CII_div_mode102);
CII_div_mode101:
    VoC_pop16(REG4,DEFAULT);
    VoC_return;
}


/*****************************************
Function: CII_mad_synth_frame
      Input:
         REG0=0 -> first half part of sbsample
         REG0=1 -> second half part of sbsample
         REG2 -> the address of pcm output buffer
Version 1.0 Created by kenneth    05/30/2005
Version 1.1 Optimized by kenneth    07/12/2005
Version 2.0 Rewroted for Full precision MP3 decoder    06/2006
*************************************************/

void CII_mad_synth_frame(void)
{
#if 0
    // WARNING : pay attention to the alignment of the D table!
    // This table should be placed in RAM_X at address N*1024 + 128
    voc_alias TABLE_D_ADDR_X  (GLOBAL_INPUT_DATA_BUFFER+128)      , x

    voc_word  LOCAL_mad_synth_frame_addr           , 32, y
    voc_short LOCAL_output_addr_addr               ,  1, y
    voc_short LOCAL_output_raw_addr_addr            , 1, y
#endif



    VoC_push16(RA,DEFAULT);  //,"syn0_0"
    VoC_lw32_d(REG45,GLOBAL_NCH_ADDR);      //nch = MAD_NCHANNELS(&frame->header);

    VoC_sw16_d(REG2,LOCAL_output_addr_addr);
    VoC_sw16_d(REG2,LOCAL_output_raw_addr_addr);
    VoC_lw16i(REG2,LOCAL_mad_synth_frame_addr);
    //ns  = MAD_NSBSAMPLES(&frame->header);

    VoC_push16(REG0,DEFAULT);//,"dither_struct"

    VoC_push16(REG0,DEFAULT);//,"sb_stat"
    VoC_lw32z(REG67,IN_PARALLEL);
    VoC_push16(REG0,DEFAULT);  //,"syn1_0"           //stack16[1]   first flag
    VoC_push16(REG2,DEFAULT); //,"syn2_0"            //stack16[2]   output buffer
    VoC_push32(REG45,IN_PARALLEL); //,"ns"           //stack32[0]   ns/nch

    //synth_full(synth, frame, nch, ns);
    /*****************************************
    Function: CII_synth_full
          Input:
             REG0=0 -> first half part of sbsample
             REG0=1 -> second half part of sbsample
             REG4 -> nch
             REG5 -> ns
             REG2 -> the address of pcm output buffer
    Version 1.0 Created by kenneth    05/30/2005
    *************************************************/


    // IMPORTANT :
    // As the D table is placed in a very special location
    // (N*1024 + 128) and in order not to constrain the memory
    // map definition, the D table is copied at each fct call
    // at the special start location and all data contained at
    // that location (TABLE_D_ADDR_X) is swapped with D.
    // This gives a small processing overhead, which is compensated
    // by the fact that the constants and the data are in different
    // memmories during the PCM synthesis.

    VoC_lw16i_set_inc(REG0,TABLE_D_ADDR_X, 2);     // in X
    VoC_lw16i_set_inc(REG1,TABLE_D_ADDR,   2);     // in Y
    VoC_lw16i_set_inc(REG2,TABLE_D_ADDR_X, 2);     // in X
    VoC_lw16i_set_inc(REG3,TABLE_D_ADDR,   2);     // in Y

    // copy D to RAM_X and keep a copy of the huffman tables
    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_lw32inc_p(RL7,REG1,IN_PARALLEL);

    VoC_loop_i(0,136)
    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_lw32inc_p(RL7,REG1,IN_PARALLEL);
    VoC_sw32inc_p(RL6,REG3,DEFAULT);
    VoC_sw32inc_p(RL7,REG2,IN_PARALLEL);
    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_lw32inc_p(RL7,REG1,IN_PARALLEL);
    VoC_sw32inc_p(RL6,REG3,DEFAULT);
    VoC_sw32inc_p(RL7,REG2,IN_PARALLEL);
    VoC_endloop0


    VoC_lw16_d(ACC0_LO,GLOBAL_MAD_SYNTH_PHASE_ADDR);
    VoC_lw16d_set_inc(REG1,STRUCT_MAD_HEADER_SAMPLERATE_ADDR,2);
    VoC_movreg16(ACC0_HI,ACC0_LO,DEFAULT);

    VoC_push16(REG6,DEFAULT);  //,"phase1"           // stack16[3]
    VoC_lw16i(REG0,STATIC_MAD_SBSAMPLE_ADDR);  // REG0->sbsample
    VoC_lw16i(REG6,STRUCT_left_dither_error_ADDR);
    VoC_sw32_d(ACC0,LOCAL_Y_PHASE_ADDR);
    VoC_lw16i(REG7,GLOBAL_MAD_SYNTH_FILTER_ADDR);
    VoC_sw16_sd(REG0,3,DEFAULT);//,"sb_stat"
    VoC_sw16_sd(REG6,4,DEFAULT);//,"dither_struct"

    VoC_lw16_d(REG6,LOCAL_Y_PHASE_ADDR);    // load phase1

syn_full_LBegin:
    VoC_bez16_r(syn_full_LEnd,REG4);        //for (ch = 0; ch < nch; ++ch) {
    VoC_sw32_sd(REG45,0,DEFAULT);//,"ns"
    VoC_push32(REG67,DEFAULT);  //,"filt"          //stack32[1]   filter/phase
    //  sbsample = &frame->sbsample[ch];
    //  filter   = &synth->filter[ch];
    //  phase    = synth->phase;
    //  pcm1     = synth->pcm.samples[ch];
syn_full_LSecBg:
    VoC_bez16_r(syn_full_LSecEnd,REG5)      //for (s = 0; s < ns; ++s) {
    VoC_push16(REG5,DEFAULT);  //,"syn3_0"           //stack16[4]   s
    VoC_lw32_sd(REG67,0,IN_PARALLEL);        //filter/phase

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_movreg16(REG5,REG6,IN_PARALLEL);
    VoC_and16_rr(REG6,REG4,DEFAULT);           //phase & 1
    VoC_shr16_ri(REG5,1,IN_PARALLEL);         //phase >> 1
    VoC_push16(REG0,DEFAULT);  //,"syn4_0"   //stack16[5]   sbsample
    VoC_shr16_ri(REG6,-8,IN_PARALLEL);      // reg6*128*2 ,for words address
    VoC_add16_rr(REG2,REG6,REG7,DEFAULT);

    VoC_lw16i_short(WRAP1,16,IN_PARALLEL);
    VoC_lw16i(REG3,512);

    VoC_add16_rr(REG3,REG2,REG3,DEFAULT);
    /***************************************
    Function: CII_dct32
    Input: reg0 ->in[32]
           reg5 -> slot
           reg2 ->lo[]
           reg3 ->hi[]
      Version 1.0 Created by kenneth    06/04/2005
    ****************************************/
    VoC_jal(CII_dct32);                             //  dct32((*sbsample)[s], phase >> 1,
    //  (*filter)[0][ phase & 1], (*filter)[1][phase & 1]);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw32_sd(REG23,0,IN_PARALLEL);               //  "filt" load fliter/phase

    VoC_lw16i_short(REG7,-2,DEFAULT);
    VoC_movreg16(REG5,REG2,IN_PARALLEL);

    VoC_and16_rr(REG7,REG2,DEFAULT);                //  pe = (phase & ~1)>>1;
    VoC_sub16_rr(REG6,REG5,REG4,IN_PARALLEL);       //  po = (((phase - 1) & 0xf)>>1)+16;

    VoC_and16_ri(REG6,0xF);                         // phase always below 16

    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_shr16_ri(REG7,1,IN_PARALLEL);               // reg7->pe

    VoC_not16_r(REG5,DEFAULT);
    VoC_lw16i_short(REG1,16,IN_PARALLEL);

    VoC_and16_rr(REG5,REG4,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG1,IN_PARALLEL);       //  reg6->po

    VoC_and16_rr(REG2,REG4,DEFAULT);
    VoC_shr16_ri(REG5,-8,IN_PARALLEL);              //  left shift 8 ,equ multiply 256

    VoC_shr16_ri(REG2,-8,DEFAULT);
    VoC_add16_rr(REG0,REG5,REG3,IN_PARALLEL);       //  fx = &(*filter)[0][~phase & 1][0];

    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);           //  fe = &(*filter)[0][ phase & 1][0];
    VoC_lw16i_short(WRAP1,3,IN_PARALLEL);

    VoC_lw16i(REG4,496);

    VoC_add16_rr(REG3,REG0,REG4,DEFAULT);           //  fo = &(*filter)[1][~phase & 1][0];  512 - 16 = 496
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw16i(REG4,TABLE_D_ADDR_X);                 //  Dptr = &D[0];

    VoC_movreg32(RL6,REG23,DEFAULT);
    VoC_add16_rr(REG1,REG4,REG6,IN_PARALLEL);       //  ptr = *Dptr + po;

    VoC_push32(REG67,DEFAULT);//,"pope"
    VoC_movreg16(REG2,REG1,IN_PARALLEL);

    VoC_lw16i_short(INC2,-1,DEFAULT);
    VoC_lw16i_short(WRAP2,3,IN_PARALLEL);

    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_lw16i_short(FORMATX,0,IN_PARALLEL);

    //  ML0(hi, lo, (*fx)[0], ptr[ 0]);
    //  MLA(hi, lo, (*fx)[1], ptr[14]);
    //  MLA(hi, lo, (*fx)[2], ptr[12]);
    //  MLA(hi, lo, (*fx)[3], ptr[10]);
    //  MLA(hi, lo, (*fx)[4], ptr[ 8]);
    //  MLA(hi, lo, (*fx)[5], ptr[ 6]);
    //  MLA(hi, lo, (*fx)[6], ptr[ 4]);
    //  MLA(hi, lo, (*fx)[7], ptr[ 2]);

    VoC_loop_i_short(8,DEFAULT)
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_startloop0
    VoC_msuXinc_pp(REG0,REG1,DEFAULT);
    VoC_msu32inc_pp(REG0,REG2,IN_PARALLEL);
    VoC_endloop0

    VoC_lw32_sd(REG67,0,DEFAULT);                      //  "pope"  MLN(hi, lo);
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);

    VoC_add16_rr(REG1,REG4,REG7,DEFAULT);              //  ptr = *Dptr + pe;
    VoC_add16_rr(REG2,REG4,REG7,IN_PARALLEL);          //  ptr = *Dptr + pe;

    //lo = L_mac(lo,  (Word16)(*fe)[0], ptr[0]);
    //lo = L_mac(lo,  (Word16)(*fe)[1], ptr[7]);
    //lo = L_mac(lo,  (Word16)(*fe)[2], ptr[6]);
    //lo = L_mac(lo,  (Word16)(*fe)[3], ptr[5]);
    //lo = L_mac(lo,  (Word16)(*fe)[4], ptr[4]);
    //lo = L_mac(lo,  (Word16)(*fe)[5], ptr[3]);
    //lo = L_mac(lo,  (Word16)(*fe)[6], ptr[2]);
    //lo = L_mac(lo,  (Word16)(*fe)[7], ptr[1])

    VoC_loop_i_short(8,DEFAULT)
    VoC_startloop0
    VoC_macXinc_pp(REG0,REG1,DEFAULT);
    VoC_mac32inc_pp(REG0,REG2,IN_PARALLEL);
    VoC_endloop0

    VoC_lw16_sd(REG5,3,DEFAULT);                       //  "syn2_0"reg2->pcm

    VoC_lw16i_set_inc(REG3,32,-2);                     // count = 32

    VoC_shr32_ri(ACC1,-15,DEFAULT);
    VoC_add32_rr(ACC1,ACC0,ACC1,DEFAULT);

    VoC_add32_rd(RL6,RL6,CONST_0x100010_ADDR);         //  ++fo; ++fe;

    VoC_lw32_sd(REG67,0,DEFAULT);
    VoC_movreg16(REG0,RL6_HI,IN_PARALLEL);

    VoC_shr32_ri(ACC1,2,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_add16_rd(REG4,REG4,CONST_32_ADDR);             //  ++Dptr;

    VoC_sw32inc_p(ACC1,REG5,DEFAULT);                  //  *pcm1++ = SHIFT(MLZ(hi, lo));
    VoC_add16_rr(REG5,REG5,REG3,IN_PARALLEL);              //  pcm_mid = pcm + 32;

    VoC_add16_rr(REG1,REG4,REG6,DEFAULT);          //  ptr = *Dptr + po;
    VoC_add16_rr(REG2,REG4,REG6,IN_PARALLEL);          //  ptr = *Dptr + po;

    // for (sb = 0; sb < 16; ++sb)  //{
    VoC_loop_i(1,15)

    //lo = L_msu(     (Word16)(*fo)[0], ptr[0]);
    //lo = L_msu(lo,  (Word16)(*fo)[1], ptr[7]);
    //lo = L_msu(lo,  (Word16)(*fo)[2], ptr[6]);
    //lo = L_msu(lo,  (Word16)(*fo)[3], ptr[5]);
    //lo = L_msu(lo,  (Word16)(*fo)[4], ptr[4]);
    //lo = L_msu(lo,  (Word16)(*fo)[5], ptr[3]);
    //lo = L_msu(lo,  (Word16)(*fo)[6], ptr[2]);
    //lo = L_msu(lo,  (Word16)(*fo)[7], ptr[1]);

    VoC_loop_i_short(8,DEFAULT)
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_startloop0
    VoC_msuXinc_pp(REG0,REG1,DEFAULT);
    VoC_msu32inc_pp(REG0,REG2,IN_PARALLEL);
    VoC_endloop0

    VoC_lw32_sd(REG67,0,DEFAULT);
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);

    VoC_add16_rr(REG1,REG4,REG7,DEFAULT);   //ptr = *Dptr + pe;
    VoC_add16_rr(REG2,REG4,REG7,IN_PARALLEL);   //ptr = *Dptr + pe;

    //lo = L_mac(lo,  (Word16)(*fe)[0], ptr[0]);
    //lo = L_mac(lo,  (Word16)(*fe)[1], ptr[7]);
    //lo = L_mac(lo,  (Word16)(*fe)[2], ptr[6]);
    //lo = L_mac(lo,  (Word16)(*fe)[3], ptr[5]);
    //lo = L_mac(lo,  (Word16)(*fe)[4], ptr[4]);
    //lo = L_mac(lo,  (Word16)(*fe)[5], ptr[3]);
    //lo = L_mac(lo,  (Word16)(*fe)[6], ptr[2]);
    //lo = L_mac(lo,  (Word16)(*fe)[7], ptr[1]);

    VoC_loop_i_short(8,DEFAULT)
    VoC_inc_p(REG3,IN_PARALLEL); // count -= 2
    VoC_startloop0
    VoC_macXinc_pp(REG0,REG1,DEFAULT);
    VoC_mac32inc_pp(REG0,REG2,IN_PARALLEL);
    VoC_endloop0

    VoC_lw32_sd(REG67,0,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_shr32_ri(ACC1,-15,DEFAULT);
    VoC_lw16i_short(REG6,23,IN_PARALLEL);

    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_sub16_rr(REG1,REG4,REG7,IN_PARALLEL);  // ptr = *Dptr - pe;

    VoC_shr32_ri(ACC0,2,DEFAULT);
    VoC_sub16_rr(REG7,REG5,REG3,IN_PARALLEL);   // pcm1 = pcm_mid - count

    VoC_add16_rr(REG1,REG1,REG6,DEFAULT);
    VoC_add16_rr(REG2,REG1,REG6,IN_PARALLEL);

    VoC_movreg16(REG0,RL6_LO,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_sw32_p(ACC0,REG7,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    //lo = L_mult(    (Word16)(*fe)[0],   ptr[23]);
    //lo = L_mac(lo,  (Word16)(*fe)[1], ptr[24]);
    //lo = L_mac(lo,  (Word16)(*fe)[2], ptr[25]);
    //lo = L_mac(lo,  (Word16)(*fe)[3], ptr[26]);
    //lo = L_mac(lo,  (Word16)(*fe)[4], ptr[27]);
    //lo = L_mac(lo,  (Word16)(*fe)[5], ptr[28]);
    //lo = L_mac(lo,  (Word16)(*fe)[6], ptr[29]);
    //lo = L_mac(lo,  (Word16)(*fe)[7], ptr[30]);

    VoC_loop_i_short(8,DEFAULT)
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_startloop0
    VoC_macXinc_pp(REG0,REG1,DEFAULT);
    VoC_mac32inc_pp(REG0,REG2,IN_PARALLEL);
    VoC_endloop0                                 //ptr = *Dptr - po+16;

    VoC_lw32_sd(REG67,0,DEFAULT);   //,"pope"
    VoC_lw16i_short(REG7,23,DEFAULT);

    VoC_sub16_rr(REG1,REG4,REG6,DEFAULT);   //ptr = *Dptr - po+16;
    VoC_movreg16(REG0,RL6_HI,IN_PARALLEL);

    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_add16_rr(REG2,REG1,REG7,IN_PARALLEL);

    VoC_add32_rd(RL6,RL6,CONST_0x100010_ADDR);       //++fo; ++fe;

    //lo = L_mac(lo,  (Word16)(*fo)[0], ptr[7]);
    //lo = L_mac(lo,  (Word16)(*fo)[1], ptr[8]);
    //lo = L_mac(lo,  (Word16)(*fo)[2], ptr[9]);
    //lo = L_mac(lo,  (Word16)(*fo)[3], ptr[10]);
    //lo = L_mac(lo,  (Word16)(*fo)[4], ptr[11]);
    //lo = L_mac(lo,  (Word16)(*fo)[5], ptr[12]);
    //lo = L_mac(lo,  (Word16)(*fo)[6], ptr[13]);
    //lo = L_mac(lo,  (Word16)(*fo)[7], ptr[14]);

    VoC_loop_i_short(8,DEFAULT)
    VoC_startloop0
    VoC_macXinc_pp(REG0,REG1,DEFAULT);
    VoC_mac32inc_pp(REG0,REG2,IN_PARALLEL);
    VoC_endloop0

    VoC_lw32_sd(REG67,0,DEFAULT);    //,"pope"
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);

    VoC_shr32_ri(ACC1,-15,DEFAULT);
    VoC_lw16i_short(INC2,-1,IN_PARALLEL);

    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_movreg16(REG0,RL6_HI,IN_PARALLEL);

    VoC_shr32_ri(ACC0,2,DEFAULT);
    VoC_add16_rr(REG7,REG5,REG3,IN_PARALLEL);   // pcm2 = pcm_mid + count

    VoC_add16_rd(REG4,REG4,CONST_32_ADDR);  //++Dptr;

    VoC_add16_rr(REG1,REG4,REG6,DEFAULT);//     ptr = *Dptr + po;
    VoC_add16_rr(REG2,REG4,REG6,IN_PARALLEL);//     ptr = *Dptr + po;

    VoC_sw32_p(ACC0,REG7,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_endloop1;                                   //}/ /++Dptr;ptr = *Dptr + po;

    //lo = L_msu(lo,  (Word16)(*fo)[0], ptr[0]);
    //lo = L_msu(lo,  (Word16)(*fo)[1], ptr[7]);
    //lo = L_msu(lo,  (Word16)(*fo)[2], ptr[6]);
    //lo = L_msu(lo,  (Word16)(*fo)[3], ptr[5]);
    //lo = L_msu(lo,  (Word16)(*fo)[4], ptr[4]);
    //lo = L_msu(lo,  (Word16)(*fo)[5], ptr[3]);
    //lo = L_msu(lo,  (Word16)(*fo)[6], ptr[2]);
    //lo = L_msu(lo,  (Word16)(*fo)[7], ptr[1]);

    VoC_loop_i_short(8,DEFAULT)
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_startloop0
    VoC_msuXinc_pp(REG0,REG1,DEFAULT);
    VoC_msu32inc_pp(REG0,REG2,IN_PARALLEL);
    VoC_endloop0

    VoC_pop32(REG67,DEFAULT);
    VoC_lw16i_short(WRAP2,0,IN_PARALLEL);

    VoC_shr32_ri(ACC1,-15,DEFAULT);
    VoC_lw32_sd(REG67,0,IN_PARALLEL);           //pcm1 += 16;

    VoC_lw16i_set_inc(REG2,LOCAL_mad_synth_frame_addr,2);

    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);

    VoC_shr32_ri(ACC0,2,DEFAULT);
    VoC_sw16_sd(REG2,3,IN_PARALLEL);  //,"syn2_0"phase = (phase + 1) % 16;

    VoC_lw16i(REG5,64);

    VoC_sw32_p(ACC0,REG5,DEFAULT);    //*pcm1 =L_shr(lo,SHIFT_VALUE);

    VoC_add16_rr(REG6,REG6,REG3,DEFAULT);
    VoC_pop16(REG0,IN_PARALLEL);          // ,"syn4_0"pop stack16[5] sbsample

    VoC_and16_ri(REG6,0xF);

    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_lw16i_short(WRAP1,16,IN_PARALLEL);

    VoC_pop16(REG5,DEFAULT);              //,"syn3_0"pop stack16[4]   s
    VoC_sw32_sd(REG67,0,IN_PARALLEL);     //,"filt"

    VoC_sub16_rr(REG5,REG5,REG3,DEFAULT);
    VoC_lw16_sd(REG2,1,IN_PARALLEL);          // LOCAL_mad_synth_frame_addr

    VoC_lw16_sd(REG1,4,DEFAULT);//,"dither_struct"
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    VoC_push16(REG5,DEFAULT);

    VoC_jal(CoolSand_audio_linear_dither);

    VoC_lw16i_short(INC1,-1,DEFAULT);
    VoC_pop16(REG5,IN_PARALLEL);

    VoC_jump(syn_full_LSecBg);

syn_full_LSecEnd:                               //}

    VoC_pop32(REG67,DEFAULT);               //,"filt" pop stack32[1]  filter/phase
    VoC_lw16_sd(REG2,0,IN_PARALLEL);//,"phase1"

    VoC_bnez16_r(syn_full_L4,REG2);

    VoC_sw16_d(REG6,LOCAL_Y_PHASE_ADDR);    // store phase1
    VoC_jump(syn_full_L5);
syn_full_L4:
    VoC_sw16_d(REG6,LOCAL_Y_PHASE_ADDR+1);  // store phase2
syn_full_L5:


    VoC_lw16_d(REG2,LOCAL_output_raw_addr_addr);//xuml add
    VoC_add16_rd(REG2,REG2,CONST_1_ADDR);//xuml add

    VoC_lw32_sd(REG45,0,DEFAULT);

    VoC_sw16_d(REG2,LOCAL_output_addr_addr);  //xuml add

    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16_sd(REG0,3,DEFAULT);//,"sb_stat"
    VoC_lw16i(REG6,2304);
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);    //jump sbsample[0] to sasample[1]

    VoC_add16_rd(REG7,REG7,CONST_1024_ADDR); //jump filter[0] to filter[1]
    VoC_lw16i(REG6,STRUCT_right_dither_error_ADDR);

    VoC_sub16_rr(REG4,REG4,REG2,DEFAULT);
    VoC_sw16_sd(REG2,0,IN_PARALLEL);     // ,"phase1"

    VoC_sw16_sd(REG6,4,DEFAULT);//,"dither_struct"
    VoC_lw16_d(REG6,LOCAL_Y_PHASE_ADDR+1);  // load phase2
    VoC_jump(syn_full_LBegin);
syn_full_LEnd:                                  //}

    VoC_lw16i_set_inc(REG1,TABLE_D_ADDR_X, 2); // in X
    VoC_lw16i_set_inc(REG0,TABLE_D_ADDR,   2); // in Y
    VoC_lw16i_set_inc(REG3,TABLE_D_ADDR_X, 2); // in X
    VoC_lw16i_set_inc(REG2,TABLE_D_ADDR,   2); // in Y

    // Swap back D and the original data contained at the
    // special location TABLE_D_ADDR_X
    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_lw32inc_p(RL7,REG1,IN_PARALLEL);

    VoC_loop_i(0,136)
    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_lw32inc_p(RL7,REG1,IN_PARALLEL);
    VoC_sw32inc_p(RL6,REG3,DEFAULT);
    VoC_sw32inc_p(RL7,REG2,IN_PARALLEL);
    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_lw32inc_p(RL7,REG1,IN_PARALLEL);
    VoC_sw32inc_p(RL6,REG3,DEFAULT);
    VoC_sw32inc_p(RL7,REG2,IN_PARALLEL);
    VoC_endloop0

    VoC_pop16(REG3,DEFAULT);              //,"phase1" pop stack16[3]
    VoC_pop16(REG3,DEFAULT);              //,"syn2_0" pop stack16[2] output buffer
    VoC_pop16(REG0,DEFAULT);              //,"syn1_0" pop stack16[1] first flag
    VoC_pop32(REG45,IN_PARALLEL);           //,"ns" pop stack32[0] ns/nch
    VoC_pop16(REG3,DEFAULT);//,"sb_stat"

    VoC_pop16(REG3,DEFAULT);//,"dither_struct"

    VoC_lw16_d(REG6,GLOBAL_MAD_SYNTH_PHASE_ADDR);

    VoC_lw16i_short(REG7,0xF,DEFAULT);
    VoC_add16_rr(REG6,REG5,REG6,DEFAULT);   //synth->phase = (synth->phase + ns) % 16;
    VoC_and16_rr(REG6,REG7,DEFAULT);

    VoC_pop16(RA,DEFAULT);                        //,"syn0_0" pop stack16[0]
    VoC_sw16_d(REG6,GLOBAL_MAD_SYNTH_PHASE_ADDR);


    // TODO : this fix is dirty, it is meant for
    // placing the synth buffer and D in different memories
    // A fix would consist in placing D in the RAM_X constants
    VoC_return;
}


/******************************************************
 Function: CoolSand_audio_linear_dither
    Input: reg1 -> dither_struct
           REG2 -> PCM IN


 Version 1.0 Created by Cui    09/26/2007
*******************************************************/


void CoolSand_audio_linear_dither(void)
{

    VoC_lw32inc_p(ACC1,REG1,DEFAULT); //error[0]
    VoC_lw32inc_p(RL6,REG1,DEFAULT);  //error[1]
    VoC_lw32inc_p(RL7,REG1,DEFAULT);  //error[2]

    VoC_sub16_rd(REG1,REG1,CONST_6_ADDR);
    VoC_lw32_d(REG67,GLOBAL_NCH_ADDR);      //nch = MAD_NCHANNELS(&frame->header);

    VoC_lw16d_set_inc(REG3,LOCAL_output_addr_addr,2);
    VoC_be16_rd(CoolSand_audio_linear_dither_L0,REG6,CONST_2_ADDR)
    VoC_lw16d_set_inc(REG3,LOCAL_output_addr_addr,1);
CoolSand_audio_linear_dither_L0:

    VoC_loop_i(0,32)

    // sample += dither->error[0] - dither->error[1] + dither->error[2];

    // dither->error[2] = dither->error[1];
    // dither->error[1] = dither->error[0] / 2;

    VoC_sub32_rr(REG67,ACC1,RL6,DEFAULT);

    VoC_add32_rr(REG67,REG67,RL7,DEFAULT);
    VoC_movreg32(RL7,RL6,IN_PARALLEL);

    VoC_add32inc_rp(REG67,REG67,REG2,DEFAULT);
    VoC_movreg32(RL6,ACC1,IN_PARALLEL);

    // output = sample + (1L << (MAD_F_FRACBITS + 1 - bits - 1));
    VoC_add32_rd(REG45,REG67,CONST_0x00001000_ADDR);

    // the clipping is done by shifting the result by (MAD_F_FRACBITS - 31),
    // which will result in a signed saturation of the result.
    VoC_shr32_ri(REG45,-3,DEFAULT);
    VoC_shr32_ri(REG67,-3,IN_PARALLEL);

    // output &= ~mask;
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_shr32_ri(RL6,1,IN_PARALLEL);

    //dither->error[0] = sample - output;
    VoC_sub32_rr(ACC1,REG67,REG45,DEFAULT);
    VoC_shr32_ri(ACC1,3,DEFAULT);

    VoC_sw16inc_p(REG5,REG3,IN_PARALLEL);

    VoC_endloop0

    VoC_lw16i_short(INC3,-2,DEFAULT);

    VoC_sw32inc_p(ACC1,REG1,DEFAULT); //error[0]
    VoC_sw32inc_p(RL6,REG1,DEFAULT);  //error[1]
    VoC_sw32inc_p(RL7,REG1,DEFAULT);  //error[2]

    VoC_sw16_d(REG3,LOCAL_output_addr_addr);

    VoC_return;

}

/******************************************************
 Function: CII_dct32
    Input: reg0 ->in[32]
           reg5 -> slot
           reg2 ->lo[]
           reg3 ->hi[]
 Version 1.0 Created by kenneth    06/04/2005
 Version 2.0 rewroted by Cui       06/21/2006  Full precision version!
*******************************************************/
void CII_dct32(void)
{
#if 0
    voc_short LOCAL_X_DCT32_TX1_T_ADDR, 2, x
    voc_short LOCAL_X_DCT32_TX1_ADDR, 32, x
    voc_short LOCAL_X_DCT32_TX2_T_ADDR, 2, x
    voc_short LOCAL_X_DCT32_TX2_ADDR, 32, x
#endif

#if 0
    voc_short LOCAL_Y_DCT32_TY2_T_ADDR, 2, y
    voc_short LOCAL_Y_DCT32_TY2_ADDR, 32, y

    voc_short LOCAL_X_DCT32_IN_ADDR ,34, y
    voc_short LOCAL_Y_DCT32_TX1_T_ADDR ,2, y
    voc_short LOCAL_Y_DCT32_TY1_ADDR   ,32, y
    voc_short LOCAL_Y_PHASE_ADDR       ,2, y
    voc_short CONST_DCT32_A24_ADDR_ADDR ,2,y

    voc_alias LOCAL_X_DCT32_A_ADDR STRUCT_SIDEINFO_ADDR, y
    voc_alias LOCAL_DCT32_A24_ADDR (46+LOCAL_X_DCT32_A_ADDR), y

#endif

    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG5,IN_PARALLEL);

    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_add16_rr(REG2,REG2,REG5,DEFAULT);
    VoC_lw16i_short(FORMATX,0,IN_PARALLEL);

    VoC_lw16i(REG1,240);

    VoC_add16_rr(REG3,REG3,REG1,DEFAULT);           // REG3->hi[15][slot]
    VoC_lw16i_short(INC2,2,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,LOCAL_X_DCT32_IN_ADDR,2);
    VoC_push32(REG23,DEFAULT);              // push stack32[0] hi/lo
    VoC_add16_rd(REG2,REG0,CONST_32_ADDR);
    VoC_lw32_d(RL6,CONST_0x00000800_ADDR);

    // copy in[16]~in[31] to RAM_X,  the local buffer called  LOCAL_X_DCT32_IN_ADDR !
    VoC_loop_i_short(16,DEFAULT);
    VoC_lw32inc_p(RL7,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(RL7,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(RL7,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_sub16_rd(REG1,REG1,CONST_2_ADDR);

    // now the REG0 point to in[0] in RAM_Y,  REG1 point to in[31] in RAM_X

    VoC_lw16i_set_inc(REG2,LOCAL_X_DCT32_TX1_ADDR-2,2);
    VoC_lw16i_set_inc(REG3,TABLE_dct32_tab_ADDR,1);

    VoC_lw16i_short(FORMAT32,-4,DEFAULT);
    VoC_lw16i_short(INC1,-2,IN_PARALLEL);

    VoC_loop_i_short(8,DEFAULT);                        //tX1[0]~tX1[15]
    VoC_sub32_pp(RL7,REG0,REG1,IN_PARALLEL);            //t16  = MUL(in[0]  - in[31], costab1);
    VoC_startloop0

    VoC_add32inc_pp(ACC1,REG0,REG1,DEFAULT);        //t0   = in[0]  + in[31];
    VoC_add32_rr(REG67,RL7,RL6,IN_PARALLEL);

    VoC_sub32_pp(RL7,REG0,REG1,DEFAULT);            //t16  = MUL(in[0]  - in[31], costab1);
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_multf32_rp(ACC0,REG7,REG3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,IN_PARALLEL);

    VoC_macXinc_rp(REG6,REG3,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,IN_PARALLEL);

    VoC_endloop0

    VoC_NOP();
    VoC_lw16i(REG2,LOCAL_Y_DCT32_TY1_ADDR-2);//tY1[0]~tY1[15]
    VoC_sw32_p(ACC0,REG2,DEFAULT);

    VoC_loop_i_short(8,DEFAULT);                //tX1[0]~tX1[15]
    VoC_startloop0

    VoC_add32_pp(ACC1,REG0,REG1,DEFAULT);    //t0   = in[0]  + in[31];
    VoC_sub32inc_pp(REG67,REG0,REG1,IN_PARALLEL);   //t16  = MUL(in[0]  - in[31], costab1);

    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,IN_PARALLEL);

    VoC_multf32_rp(ACC0,REG7,REG3,DEFAULT);
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_macXinc_rp(REG6,REG3,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);

    VoC_endloop0

    VoC_lw16i_set_inc(REG0,LOCAL_X_DCT32_TX1_ADDR,4);
    VoC_lw16i_set_inc(REG1,LOCAL_Y_DCT32_TY1_ADDR+28,-4);
    VoC_lw16i_set_inc(REG2,LOCAL_X_DCT32_TX2_ADDR-2,2);

    VoC_sw32_p(ACC0,REG2,DEFAULT);

    VoC_loop_i_short(4,DEFAULT);                        //tX2[0]~tX1[7]
    VoC_sub32_pp(RL7,REG0,REG1,IN_PARALLEL);
    VoC_startloop0

    VoC_add32inc_pp(ACC1,REG0,REG1,DEFAULT);
    VoC_add32_rr(REG67,RL7,RL6,IN_PARALLEL);

    VoC_sub32_pp(RL7,REG0,REG1,DEFAULT);
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_multf32_rp(ACC0,REG7,REG3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,IN_PARALLEL);

    VoC_macXinc_rp(REG6,REG3,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,IN_PARALLEL);

    VoC_endloop0

    VoC_NOP();
    VoC_lw16i(REG2,LOCAL_Y_DCT32_TY2_ADDR+14);
    VoC_sw32_p(ACC0,REG2,DEFAULT);

    VoC_loop_i_short(4,DEFAULT);                //tY2[8]~tY2[15]
    VoC_startloop0

    VoC_add32_pp(ACC1,REG0,REG1,DEFAULT);
    VoC_sub32inc_pp(REG67,REG0,REG1,IN_PARALLEL);

    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,IN_PARALLEL);

    VoC_multf32_rp(ACC0,REG7,REG3,DEFAULT);
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_macXinc_rp(REG6,REG3,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);

    VoC_endloop0

    VoC_sub16_rd(REG3,REG3,CONST_8_ADDR);               //
    VoC_lw16i_set_inc(REG0,LOCAL_X_DCT32_TX1_ADDR+2,4);
    VoC_lw16i_set_inc(REG1,LOCAL_Y_DCT32_TY1_ADDR+30,-4);
    VoC_lw16i_set_inc(REG2,LOCAL_X_DCT32_TX2_ADDR+14,2);

    VoC_sw32_p(ACC0,REG2,DEFAULT);
    VoC_lw32_p(ACC0,REG2,DEFAULT);          // used for protect LOCAL_X_DCT32_TX2_ADDR+14

    VoC_loop_i_short(4,DEFAULT);                        //tX2[8]~tX2[15]
    VoC_sub32_pp(RL7,REG0,REG1,IN_PARALLEL);
    VoC_startloop0

    VoC_add32inc_pp(ACC1,REG0,REG1,DEFAULT);
    VoC_add32_rr(REG67,RL7,RL6,IN_PARALLEL);

    VoC_sub32_pp(RL7,REG0,REG1,DEFAULT);
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_multf32_rp(ACC0,REG7,REG3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,IN_PARALLEL);

    VoC_macXinc_rp(REG6,REG3,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,IN_PARALLEL);

    VoC_endloop0

    VoC_NOP();
    VoC_lw16i(REG2,LOCAL_Y_DCT32_TY2_ADDR-2);           //tY2[0]~tY2[7]
    VoC_sw32_p(ACC0,REG2,DEFAULT);

    VoC_loop_i_short(4,DEFAULT);
    VoC_startloop0

    VoC_add32_pp(ACC1,REG0,REG1,DEFAULT);
    VoC_sub32inc_pp(REG67,REG0,REG1,IN_PARALLEL);

    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,IN_PARALLEL);

    VoC_multf32_rp(ACC0,REG7,REG3,DEFAULT);
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_macXinc_rp(REG6,REG3,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);

    VoC_endloop0

    // begine of tX2[0], tY2[14])
    VoC_lw16i_set_inc(REG0,LOCAL_X_DCT32_TX2_ADDR,4);
    VoC_lw16i_set_inc(REG1,LOCAL_Y_DCT32_TY2_ADDR+28,-4);
    VoC_lw16i_set_inc(REG2,LOCAL_X_DCT32_TX1_ADDR-2,2);

    VoC_sw32_p(ACC0,REG2,DEFAULT);

    VoC_loop_i(1,2);

    VoC_loop_i_short(2,DEFAULT);                    //tX1[0]~tX1[3]
    VoC_startloop0

    VoC_add32_pp(ACC1,REG0,REG1,DEFAULT);
    VoC_sub32inc_pp(REG67,REG0,REG1,IN_PARALLEL);

    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,IN_PARALLEL);

    VoC_multf32_rp(ACC0,REG7,REG3,DEFAULT);
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_macXinc_rp(REG6,REG3,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);

    VoC_endloop0

    VoC_movreg16(REG4,REG2,DEFAULT);                // store &tx1[3]
    VoC_movreg16(REG5,REG4,IN_PARALLEL);

    VoC_sw32_p(ACC0,REG2,DEFAULT);
    VoC_lw16i(REG2,LOCAL_Y_DCT32_TY1_ADDR+22);       //tY1[12]~tY1[15]

    VoC_endloop1

    VoC_sub16_rd(REG3,REG3,CONST_4_ADDR);
    VoC_movreg16(REG2,REG5,DEFAULT);                // load &tx1[3]
    VoC_lw32_p(ACC0,REG5,IN_PARALLEL);

    VoC_loop_i(1,2);

    VoC_loop_i_short(2,DEFAULT);                    // tX1[4]~ tX1[7]
    VoC_startloop0

    VoC_add32_pp(ACC1,REG0,REG1,DEFAULT);
    VoC_sub32inc_pp(REG67,REG0,REG1,IN_PARALLEL);

    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,IN_PARALLEL);

    VoC_multf32_rp(ACC0,REG7,REG3,DEFAULT);
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_macXinc_rp(REG6,REG3,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);

    VoC_endloop0

    VoC_movreg16(REG4,REG2,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);

    VoC_sw32_p(ACC0,REG2,DEFAULT);
    VoC_lw16i(REG2,LOCAL_Y_DCT32_TY1_ADDR+14);

    VoC_endloop1

    VoC_sub16_rd(REG3,REG3,CONST_4_ADDR);               // ptr_hi -=6;       ptr_lo -=6;

    VoC_movreg16(REG2,REG5,DEFAULT);
    VoC_lw32_p(ACC0,REG5,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,LOCAL_X_DCT32_TX2_ADDR+2,4);
    VoC_lw16i_set_inc(REG1,LOCAL_Y_DCT32_TY2_ADDR+30,-4);

    VoC_loop_i(1,2);

    VoC_loop_i_short(2,DEFAULT);                    // tX1[8]~ tX1[11]
    VoC_startloop0

    VoC_add32_pp(ACC1,REG0,REG1,DEFAULT);
    VoC_sub32inc_pp(REG67,REG0,REG1,IN_PARALLEL);

    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,IN_PARALLEL);

    VoC_multf32_rp(ACC0,REG7,REG3,DEFAULT);
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_macXinc_rp(REG6,REG3,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);

    VoC_endloop0

    VoC_movreg16(REG4,REG2,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);
    VoC_sw32_p(ACC0,REG2,DEFAULT);
    VoC_lw16i(REG2,LOCAL_Y_DCT32_TY1_ADDR+6);

    VoC_endloop1

    VoC_sub16_rd(REG3,REG3,CONST_4_ADDR);               // ptr_hi -=6;       ptr_lo -=6;
    VoC_movreg16(REG2,REG5,DEFAULT);
    VoC_lw32_p(ACC0,REG5,IN_PARALLEL);

    VoC_loop_i(1,2);

    VoC_loop_i_short(2,DEFAULT);                    // tX1[12]~ tX1[15]
    VoC_startloop0

    VoC_add32_pp(ACC1,REG0,REG1,DEFAULT);
    VoC_sub32inc_pp(REG67,REG0,REG1,IN_PARALLEL);

    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,IN_PARALLEL);

    VoC_multf32_rp(ACC0,REG7,REG3,DEFAULT);
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_macXinc_rp(REG6,REG3,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);

    VoC_endloop0

    VoC_movreg16(REG4,REG2,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);

    VoC_sw32_p(ACC0,REG2,DEFAULT);
    VoC_lw16i(REG2,LOCAL_Y_DCT32_TY1_ADDR-2);

    VoC_endloop1


    VoC_lw16i_set_inc(REG0,LOCAL_X_DCT32_TX1_ADDR,4);
    VoC_lw16i_set_inc(REG1,LOCAL_Y_DCT32_TY1_ADDR+28,-4);

    VoC_movreg16(REG4,REG3,DEFAULT);

    VoC_lw16i_set_inc(REG2,LOCAL_X_DCT32_TX2_ADDR,2);//used for save  t113 + t114 and so on,
    VoC_lw16i_set_inc(REG3,LOCAL_Y_DCT32_TY2_ADDR,2);//used for save  MUL(t113 - t114, costab16) and so on

    VoC_loop_i(1,2);

    VoC_add32inc_pp(ACC0,REG0,REG1,DEFAULT);    //t1 = L_add( tX1[0],tY1[14] );  t113 = t69  + t70;
    VoC_add32inc_pp(RL7,REG0,REG1,DEFAULT); //t2 = L_add( tX1[2],tY1[12]);  t114 = t71  + t72;

    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);      //t113 + t114
    VoC_sub32_rr(REG67,ACC0,RL7,IN_PARALLEL);// t113 - t114

    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);

    VoC_multf32_rp(ACC1,REG7,REG4,DEFAULT);
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_macX_rp(REG6,REG4,IN_PARALLEL);  // MUL(t113 - t114, costab16)

    VoC_loop_i_short(3,DEFAULT);
    VoC_startloop0

    VoC_add32inc_pp(ACC0,REG0,REG1,DEFAULT);    //t1 = L_add( tX1[0],tY1[14] );  t113 = t69  + t70;
    VoC_add32inc_pp(RL7,REG0,REG1,DEFAULT); //t2 = L_add( tX1[2],tY1[12]);  t114 = t71  + t72;

    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);      //t113 + t114
    VoC_sub32_rr(REG67,ACC0,RL7,IN_PARALLEL);// t113 - t114

    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);
    VoC_sw32inc_p(ACC1,REG3,IN_PARALLEL);

    VoC_multf32_rp(ACC1,REG7,REG4,DEFAULT);
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_macX_rp(REG6,REG4,IN_PARALLEL);  // MUL(t113 - t114, costab16)

    VoC_endloop0

    VoC_lw16i_set_inc(REG0,LOCAL_X_DCT32_TX1_ADDR+2,4);
    VoC_lw16i_set_inc(REG1,LOCAL_Y_DCT32_TY1_ADDR+30,-4);

    VoC_sw32inc_p(ACC1,REG3,DEFAULT);

    VoC_endloop1

    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_lw16i_short(FORMATX,-4,DEFAULT);

    VoC_lw16i_short(INC3,1,IN_PARALLEL);

    // now in the buffer of LOCAL_X_DCT32_TX2_ADDR,saved as t113 + t114,t32,t58,t67 ,t93,t98,t104 and t110
    // in buffer LOCAL_Y_DCT32_TY2_ADDR saved as  MUL(t113 - t114, costab16),MUL(t115 - t116, costab16),and so on

    VoC_movreg32(RL7,REG23,DEFAULT);
    VoC_movreg16(REG3,REG4,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,LOCAL_X_DCT32_TX1_ADDR,4);
    VoC_lw16i_set_inc(REG1,LOCAL_Y_DCT32_TY1_ADDR+28,-4);

    VoC_inc_p(REG3,DEFAULT);
    VoC_movreg16(REG2,RL7_HI,IN_PARALLEL);

    VoC_loop_i(1,2);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_sub32inc_pp(REG67,REG0,REG1,IN_PARALLEL);   //t1 = L_add( tX1[0],tY1[14] );  t113 = t69  - t70;

    VoC_lw32z(ACC1,DEFAULT);
    VoC_sub32inc_pp(REG45,REG0,REG1,IN_PARALLEL);

    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);
    VoC_add32_rr(REG45,REG45,RL6,IN_PARALLEL);

    VoC_macX_rp(REG7,REG3,DEFAULT);
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_mac32inc_rp(REG6,REG3,DEFAULT);     //ACC0 :t141 = MUL(t69 - t70, costab8);
    VoC_shru16_ri(REG4,12,IN_PARALLEL);

    VoC_movreg16(RL7_HI,REG2,DEFAULT);
    VoC_macX_rp(REG5,REG3,IN_PARALLEL);

    VoC_movreg16(REG2,RL7_LO,DEFAULT);
    VoC_mac32inc_rp(REG4,REG3,IN_PARALLEL);  //ACC1: t142 = MUL(t71 - t72, costab24);

    VoC_sub16_rd(REG3,REG3,CONST_3_ADDR);

    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);      // t143 = t141 + t142;
    VoC_sub32_rr(REG67,ACC0,ACC1,IN_PARALLEL);// t141 - t142;

    VoC_lw32z(ACC1,DEFAULT);
    VoC_add32_rr(REG67,REG67,RL6,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);
    VoC_macX_rp(REG7,REG3,IN_PARALLEL);

    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_macX_rp(REG7,REG3,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_mac32_rp(REG6,REG3,IN_PARALLEL);     //ACC1: MUL(t141 - t142, costab16)

    VoC_movreg16(RL7_LO,REG2,DEFAULT);
    VoC_mac32inc_rp(REG6,REG3,IN_PARALLEL);  //ACC1: MUL(t141 - t142, costab16)

    VoC_loop_i(0,3);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_sub32inc_pp(REG67,REG0,REG1,IN_PARALLEL);   //t1 = L_add( tX1[0],tY1[14] );  t113 = t69  - t70;

    VoC_movreg16(REG2,RL7_HI,DEFAULT);
    VoC_sub32inc_pp(REG45,REG0,REG1,IN_PARALLEL);

    VoC_lw32z(ACC1,DEFAULT);
    VoC_add32_rr(REG67,REG67,RL6,IN_PARALLEL);

    VoC_sw32inc_p(ACC1,REG2,DEFAULT);
    VoC_add32_rr(REG45,REG45,RL6,IN_PARALLEL);

    VoC_macX_rp(REG7,REG3,DEFAULT);
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_mac32inc_rp(REG6,REG3,DEFAULT);     //ACC0 :t141 = MUL(t69 - t70, costab8);
    VoC_shru16_ri(REG4,12,IN_PARALLEL);

    VoC_movreg16(RL7_HI,REG2,DEFAULT);
    VoC_macX_rp(REG5,REG3,IN_PARALLEL);

    VoC_movreg16(REG2,RL7_LO,DEFAULT);
    VoC_mac32inc_rp(REG4,REG3,IN_PARALLEL);  //ACC1: t142 = MUL(t71 - t72, costab24);

    VoC_sub16_rd(REG3,REG3,CONST_3_ADDR);

    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);      // t143 = t141 + t142;
    VoC_sub32_rr(REG67,ACC0,ACC1,IN_PARALLEL);// t141 - t142;

    VoC_lw32z(ACC1,DEFAULT);
    VoC_add32_rr(REG67,REG67,RL6,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);
    VoC_macX_rp(REG7,REG3,IN_PARALLEL);

    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_macX_rp(REG7,REG3,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_mac32_rp(REG6,REG3,IN_PARALLEL);     //ACC1: MUL(t141 - t142, costab16)

    VoC_movreg16(RL7_LO,REG2,DEFAULT);
    VoC_mac32inc_rp(REG6,REG3,IN_PARALLEL);  //ACC1: MUL(t141 - t142, costab16)

    VoC_endloop0

    VoC_movreg16(REG2,RL7_HI,DEFAULT);

    VoC_lw16i_set_inc(REG0,LOCAL_X_DCT32_TX1_ADDR+2,4);
    VoC_lw16i_set_inc(REG1,LOCAL_Y_DCT32_TY1_ADDR+30,-4);

    VoC_sw32inc_p(ACC1,REG2,DEFAULT);

    VoC_endloop1


    VoC_lw32_sd(REG45,0,DEFAULT);
    VoC_lw16i_short(INC2,-16,IN_PARALLEL);


    VoC_lw16i_set_inc(REG0,LOCAL_X_DCT32_TX2_ADDR,2);
    VoC_lw16i_set_inc(REG1,LOCAL_X_DCT32_TX2_ADDR+2,2);//point to t32
    VoC_lw16i_set_inc(REG3,LOCAL_X_DCT32_TX1_ADDR,2);//used for save some variables needed by compute lo[]

    /*  0 */
    /*  1 */
    /*  2 */

    VoC_movreg16(REG2,REG5,DEFAULT);
    VoC_add32inc_rp(REG45,RL6,REG0,IN_PARALLEL);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);

    VoC_loop_i_short(2,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_startloop0

    VoC_add32inc_rp(REG45,RL6,REG0,DEFAULT);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);       // hi[15][slot]  //hi[14][slot] = SHIFT(t32);

    VoC_shru16_ri(REG4,1,DEFAULT);

    VoC_endloop0                                    // hi[13][slot] = SHIFT(t58);

    VoC_lw32inc_p(ACC1,REG0,DEFAULT);

    VoC_shr32_ri(ACC1,-1,DEFAULT);                  // (t67 * 2)
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          // hi[15][slot]  //hi[14][slot] = SHIFT(t32);

    /*  3 */
    VoC_sub32inc_rp(RL7,ACC1,REG1,DEFAULT);         // RL7 :   t49  = (t67 * 2) - t32;

    VoC_add32_rr(REG45,RL7,RL6,DEFAULT);
    VoC_add32inc_rp(REG67,RL6,REG0,IN_PARALLEL);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_shr32_ri(REG67,DCT32_SHIFT,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_lw32inc_p(ACC1,REG0,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_shr32_ri(ACC1,-1,IN_PARALLEL);              // (t98 * 2)

    /*  4 */
    VoC_sub32_rr(RL7,ACC1,RL7,DEFAULT);             // t68  = (t98 * 2) - t49;
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          // hi[12][slot] = SHIFT(t49);

    VoC_add32_rr(REG45,RL7,RL6,DEFAULT);
    VoC_sw32inc_p(REG67,REG2,IN_PARALLEL);          // hi[11][slot] = SHIFT(t93);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);              // (t104 * 2)

    /*  5 */
    VoC_sub32inc_rp(ACC1,ACC0,REG1,DEFAULT);       // t82  = (t104 * 2) - t58;

    VoC_add32_rr(REG45,ACC1,RL6,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          // hi[10][slot] = SHIFT(t68);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shr32_ri(REG67,-1,IN_PARALLEL);

    /*  6 */ /*  7 */
    VoC_sub32inc_rp(REG45,REG67,REG1,DEFAULT);       // t87  = (t110 * 2) - t67;

    VoC_add32_rr(ACC0,REG45,REG45,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          // hi[ 9][slot] = SHIFT(t82);

    VoC_sub32_rr(RL7,ACC0,RL7,DEFAULT);             // t77  = (t87 * 2) - t68;
    VoC_push32(REG45,IN_PARALLEL);                  // push t87

    VoC_add32_rr(REG45,RL7,RL6,DEFAULT);
    VoC_add32inc_rp(REG67,RL6,REG0,IN_PARALLEL);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_shr32_ri(REG67,DCT32_SHIFT,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    /*  8 */
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);

    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          // hi[ 8][slot] = SHIFT(t77);

    VoC_sub32_rr(RL7,ACC0,RL7,DEFAULT);             // t88  = (t146 * 2) - t77;
    VoC_sw32inc_p(REG67,REG2,IN_PARALLEL);          // hi[ 7][slot] = SHIFT(t143);

    VoC_add32_rr(REG45,RL7,RL6,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_sub32_rr(REG67,ACC0,ACC1,IN_PARALLEL);      // t105 = (t150 * 2) - t82;

    /*  9 */
    VoC_add32_rr(REG45,REG67,RL6,DEFAULT);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          // hi[ 6][slot] = SHIFT(t88);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_pop32(REG45,DEFAULT);                       // pop t87
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);

    VoC_sub32_rr(REG45,ACC0,REG45,DEFAULT);         // t111 = (t154 * 2) - t87;
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          // hi[ 5][slot] = SHIFT(t105);

    /* 10 */
    VoC_add32_rr(ACC0,REG45,REG45,DEFAULT);

    VoC_sub32_rr(RL7,ACC0,RL7,DEFAULT);             // t99  = (t111 * 2) - t88;
    VoC_push32(REG45,IN_PARALLEL);                  // push t111

    VoC_add32_rr(REG45,RL7,RL6,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_sub32inc_rp(ACC0,ACC0,REG1,IN_PARALLEL);    // t127 = (t159 * 2) - t93;

    /* 11 */
    VoC_add32_rr(REG45,ACC0,RL6,DEFAULT);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          // hi[ 4][slot] = SHIFT(t99);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,IN_PARALLEL);           // save t127

    /* 12 */
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);

    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          // hi[ 3][slot] = SHIFT(t127);

    VoC_sub32inc_rp(ACC0,ACC0,REG1,DEFAULT);        // t130 = (t163 * 2) - t98;

    VoC_add32_rr(ACC1,ACC0,ACC0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_sub32_rr(RL7,ACC1,RL7,DEFAULT);             // t112 = (t130 * 2) - t99;
    VoC_sw32inc_p(ACC0,REG3,IN_PARALLEL);           // save t130

    VoC_add32_rr(REG45,RL7,RL6,DEFAULT);
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_sub32inc_rp(ACC0,ACC0,REG1,IN_PARALLEL);    // t134 = (t168 * 2) - t104;

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_add32_rr(ACC1,ACC0,ACC0,IN_PARALLEL);

    /* 13 */
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);               // save t134
    VoC_sub32_rr(ACC1,ACC1,REG67,IN_PARALLEL);      // t120 = (t134 * 2) - t105;

    VoC_add32_rr(REG45,ACC1,RL6,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          // hi[ 2][slot] = SHIFT(t112);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);

    /* 14 */
    VoC_lw16i_set_inc(REG0,LOCAL_Y_DCT32_TY2_ADDR,2);

    VoC_sub32inc_rp(ACC0,ACC0,REG1,DEFAULT);        // t138 = (t173 * 2) - t110;
    VoC_pop32(REG45,IN_PARALLEL);                   // pop t111

    VoC_add32_rr(ACC0,ACC0,ACC0,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          // hi[ 1][slot] = SHIFT(t120);

    VoC_sub32_rr(ACC0,ACC0,REG45,DEFAULT);          // t123 = (t138 * 2) - t111;
    VoC_sw32inc_p(ACC0,REG3,IN_PARALLEL);           // save t138

    VoC_add32_rr(REG67,ACC0,ACC0,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);

    VoC_sub32_rr(RL7,REG67,RL7,DEFAULT);            //  t117 = (t123 * 2) - t112;
    VoC_movreg16(REG3,REG2,IN_PARALLEL);

    VoC_add32_rr(REG45,RL7,RL6,DEFAULT);
    VoC_movreg16(REG2,REG4,IN_PARALLEL);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_add32inc_rp(REG67,RL6,REG0,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shr32_ri(REG67,DCT32_SHIFT,IN_PARALLEL);

    /* 15 */
    VoC_lw16i_set_inc(REG3,LOCAL_X_DCT32_TX1_ADDR,2);

    VoC_sw32_p(REG45,REG3,DEFAULT);                 // hi[ 0][slot] = SHIFT(t117);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_lw16i_short(INC2,16,DEFAULT);
    VoC_push32(ACC0,IN_PARALLEL);                   // pop t123

    /* 16 */
    VoC_lw32inc_p(REG67,REG0,DEFAULT);

    VoC_shr32_ri(REG67,-1,DEFAULT);
    VoC_sw32inc_p(REG67,REG2,IN_PARALLEL);          // lo[ 0][slot] = SHIFT(MUL(t113 - t114, costab16));

    VoC_sub32_rr(RL7,REG67,RL7,DEFAULT);            // t124 = (MUL(t115 - t116, costab16) * 2) - t117;
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_add32_rr(REG45,RL7,RL6,DEFAULT);
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_sub32_rr(ACC1,ACC0,ACC1,IN_PARALLEL);       // t135 = (MUL(t118 - t119, costab16) * 2) - t120;

    /* 17 */
    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_add32_rr(REG67,ACC1,RL6,IN_PARALLEL);

    VoC_shr32_ri(REG67,DCT32_SHIFT,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          // lo[ 1][slot] = SHIFT(t124);

    /* 18 */
    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);

    VoC_sub32_rr(REG45,ACC0,REG45,DEFAULT);         // t139 = (MUL(t121 - t122, costab16) * 2) - t123;
    VoC_sw32inc_p(REG67,REG2,IN_PARALLEL);          // lo[ 2][slot] = SHIFT(t135);

    VoC_add32_rr(REG67,REG45,REG45,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_sub32_rr(RL7,REG67,RL7,DEFAULT);            //t131 = (t139 * 2) - t124;
    VoC_push32(REG45,IN_PARALLEL);                  // push t139

    VoC_add32_rr(REG45,RL7,RL6,DEFAULT);
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_sub32_rp(ACC0,ACC0,REG3,IN_PARALLEL);       //  t160 = (MUL(t125 - t126, costab16) * 2) - t127;

    /* 19 */
    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_add32_rr(REG67,ACC0,RL6,IN_PARALLEL);

    VoC_shr32_ri(REG67,DCT32_SHIFT,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          // lo[ 3][slot] = SHIFT(t131);

    /* 20 */
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);

    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_sw32inc_p(REG67,REG2,IN_PARALLEL);          //  lo[ 4][slot] = SHIFT(t160);

    VoC_sub32_rp(REG67,ACC0,REG3,DEFAULT);          // t164 = (MUL(t128 - t129, costab16) * 2) - t130;
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_shr32_ri(REG67,-1,DEFAULT);
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);

    VoC_sub32_rr(RL7,REG67,RL7,DEFAULT);            //  t140 = (t164 * 2) - t131;
    VoC_sw32inc_p(REG67,REG3,IN_PARALLEL);

    VoC_add32_rr(REG45,RL7,RL6,DEFAULT);
    VoC_sub32_rp(ACC0,ACC0,REG3,IN_PARALLEL);       //t169 = (MUL(t132 - t133, costab16) * 2) - t134;

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_add32_rr(REG67,ACC0,ACC0,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_sub32_rr(ACC1,REG67,ACC1,IN_PARALLEL);     //t151 = (t169 * 2) - t135;

    /* 21 */
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_add32_rr(REG45,ACC1,RL6,IN_PARALLEL);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);         // lo[ 5][slot] = SHIFT(t140);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    /* 22 */
    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);                  // pop t139

    VoC_sub32_rp(ACC0,ACC0,REG3,DEFAULT);          //  t174 = (MUL(t136 - t137, costab16) * 2) - t138;
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);         // lo[ 6][slot] = SHIFT(t151);

    VoC_add32_rr(REG67,ACC0,ACC0,DEFAULT);

    VoC_sub32_rr(REG45,REG67,REG45,DEFAULT);        // t155 = (t174 * 2) - t139;
    VoC_sw32inc_p(ACC0,REG3,IN_PARALLEL);

    /* 23 */
    VoC_add32_rr(REG67,REG45,REG45,DEFAULT);
    VoC_add32inc_rp(REG45,RL6,REG0,IN_PARALLEL);

    VoC_sub32_rr(RL7,REG67,RL7,DEFAULT);            // t147 = (t155 * 2) - t140;
    VoC_push32(REG45,IN_PARALLEL);                  // push t155

    VoC_add32_rr(REG67,RL7,RL6,DEFAULT);
    VoC_shr32_ri(REG45,DCT32_SHIFT,IN_PARALLEL);

    VoC_shr32_ri(REG67,DCT32_SHIFT,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    /* 24 */
    VoC_add32_rr(ACC0,ACC0,ACC0,DEFAULT);

    VoC_sub32_rr(RL7,ACC0,RL7,DEFAULT);             //  t156 = (((MUL(t144 - t145, costab16) * 2) - t146) * 2) - t147;
    VoC_sw32inc_p(REG67,REG2,IN_PARALLEL);          //  lo[ 7][slot] = SHIFT(t147);

    VoC_add32_rr(REG45,RL7,RL6,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          //  lo[ 8][slot]

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);

    /* 25 */
    VoC_sub32_rr(REG67,ACC0,ACC1,DEFAULT);          //  t170 = (((MUL(t148 - t149, costab16) * 2) - t150) * 2) - t151;
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_add32_rr(REG45,REG67,RL6,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          //  lo[ 9][slot] = SHIFT(t156);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_pop32(ACC1,IN_PARALLEL);                    // pop t155

    /* 26 */
    VoC_sub32_rr(ACC1,ACC0,ACC1,DEFAULT);           // t175 = (((MUL(t152 - t153, costab16) * 2) - t154) * 2) - t155;

    VoC_add32_rr(ACC1,ACC1,ACC1,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          // lo[10][slot] = SHIFT(t170);

    VoC_sub32_rr(RL7,ACC1,RL7,DEFAULT);             // t165 = (t175 * 2) - t156;
    VoC_push32(ACC1,IN_PARALLEL);                   // push t175

    VoC_add32_rr(REG45,RL7,RL6,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG3,LOCAL_X_DCT32_TX1_ADDR,2);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_shru32_ri(ACC0,-1,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_sub32inc_rp(ACC1,ACC0,REG3,IN_PARALLEL);    //   SHIFT((((MUL(t157 - t158, costab16) * 2) - t159) * 2) - t160);

    /* 27 */
    VoC_add32_rr(REG45,ACC1,RL6,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          //  lo[11][slot] = SHIFT(t165);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shru32_ri(ACC0,-1,IN_PARALLEL);

    /* 28 */
    VoC_sub32inc_rp(ACC1,ACC0,REG3,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_shru32_ri(ACC1,-1,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          // lo[12][slot]

    VoC_sub32_rr(RL7,ACC1,RL7,DEFAULT);             //t176 = (((((MUL(t161 - t162, costab16) * 2) -  t163) * 2) - t164) * 2) - t165;
    VoC_shru32_ri(ACC0,-1,IN_PARALLEL);

    VoC_add32_rr(REG45,RL7,RL6,DEFAULT);
    VoC_sub32inc_rp(ACC1,ACC0,REG3,IN_PARALLEL);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_shru32_ri(ACC1,-1,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_sub32_rr(REG67,ACC1,REG67,IN_PARALLEL);

    /* 29 */
    VoC_add32_rr(REG45,REG67,RL6,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);

    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          //lo[13][slot] = SHIFT(t176);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shru32_ri(ACC0,-1,IN_PARALLEL);

    /* 30 */
    VoC_sub32inc_rp(ACC1,ACC0,REG3,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);                   // pop t175

    VoC_shru32_ri(ACC1,-1,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,IN_PARALLEL);          //  lo[14][slot] = SHIFT((((((MUL(t166 - t167, costab16) * 2) -  t168) * 2) - t169) * 2) - t170);

    VoC_sub32_rr(ACC1,ACC1,REG45,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_shru32_ri(ACC1,-1,DEFAULT);
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);

    VoC_sub32_rr(RL7,ACC1,RL7,DEFAULT);
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);

    VoC_add32_rr(REG45,RL7,RL6,DEFAULT);
    VoC_shr32_ri(REG45,DCT32_SHIFT,DEFAULT);
    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_NOP();
    VoC_sw32_p(REG45,REG2,DEFAULT);

    VoC_return;
}

/*
 * NAME:    III_sideinfo()
 * DESCRIPTION: decode frame side information from a bitstream
 */
//input :
//  REG5  :   lsf
// output   :
//
//         REG6     :   data_bitlen
//         REG7     :   priv_bitlen
//        RL7_lo    :   result

void  CII_III_sideinfo(void)
{
    VoC_lw32z(RL7,DEFAULT);                //   enum mad_error result = MAD_ERROR_NONE;
    VoC_lw16_d(REG2, GLOBAL_NCH_ADDR);

    VoC_lw16i_short(REG6, 0,DEFAULT);         // REG6  *data_bitlen = 0;
    VoC_lw16i_short(REG7, 1,IN_PARALLEL);
    VoC_be16_rr(III_sideinfo_100,  REG2 , REG7  );
    VoC_lw16i_short(REG7, 2,DEFAULT);
III_sideinfo_100:

    VoC_lw16i_short(REG3, 8,DEFAULT);
    VoC_bnez16_r(III_sideinfo_200, REG5)
    VoC_lw16i_short(REG7, 5,DEFAULT);
    VoC_lw16i_short(REG3, 9,IN_PARALLEL);
    VoC_be16_rd( III_sideinfo_200,  REG2 ,CONST_1_ADDR)
    VoC_lw16i_short(REG7,3,DEFAULT);     // *priv_bitlen = lsf ? ((nch == 1) ? 1 : 2) : ((nch == 1) ? 5 : 3);
III_sideinfo_200:
    //REG6     :   data_bitlen        //REG5     :   lsf
    //REG7     :   priv_bitlen
    VoC_lw16i_set_inc(REG1, STRUCT_SIDEINFO_main_data_begin_ADDR, 1);
    VoC_lbinc_p(REG0);

    VoC_rbinc_r(REG4, REG3,DEFAULT);
    VoC_rbinc_r(REG4, REG7,DEFAULT);
    exit_on_warnings =OFF;
    VoC_sw16inc_p(REG4, REG1,DEFAULT);    //si->main_data_begin = mad_bit_read(ptr, lsf ? 8 : 9);
    VoC_lw16i_short(REG3, 1,IN_PARALLEL);   // ngr = 1;
    exit_on_warnings =ON;
    VoC_sw16inc_p(REG4, REG1,DEFAULT);   // si->private_bits    = mad_bit_read(ptr, *priv_bitlen);
    VoC_bnez16_r(III_sideinfo_300, REG5)       //   if (!lsf) {
    VoC_lbinc_p(REG0);
    VoC_loop_r_short(REG2,DEFAULT)          //   for (ch = 0; ch < nch; ++ch)
    VoC_lw16i_short(REG3, 2,IN_PARALLEL);    // ngr = 2;
    VoC_startloop0
    VoC_rbinc_i(REG4, 4,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG4, REG1,DEFAULT);  //     si->scfsi[ch] = mad_bit_read(ptr, 4);
    VoC_endloop0;
    VoC_NOP();
III_sideinfo_300:
// for (gr = 0; gr < ngr; ++gr) {
//   struct granule *granule = &si->gr[gr];
    VoC_lw16i_set_inc(REG1, STRUCT_GRANULE0_BEGIN_ADDR, 1);
III_sideinfo_gr_loop_begin:
    VoC_push16(REG3,DEFAULT);//,"ngr"

    VoC_lw32_d(ACC0,STRUCT_SIDEINFO_scfsi_ADDR);
    VoC_loop_r(1, REG2)                   //     for (ch = 0; ch < nch; ++ch) {
    VoC_lbinc_p(REG0);                        //    struct channel *channel = &granule->ch[ch];
    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,12,DEFAULT);
    VoC_add16_rr(REG6, REG6, REG4,DEFAULT);   //    *data_bitlen += channel->part2_3_length;
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,9,DEFAULT);
    exit_on_warnings =OFF;
    VoC_sw16inc_p(REG4, REG1,DEFAULT);         //   channel->part2_3_length    = mad_bit_read(ptr, 12);
    VoC_lbinc_p(REG0);

    VoC_movreg16(REG3, REG4,DEFAULT);
    VoC_sw16inc_p(REG4, REG1,IN_PARALLEL);          //    channel->big_values        = mad_bit_read(ptr, 9);
    exit_on_warnings =ON;
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_lw16i_short(REG2,9,DEFAULT);
    VoC_sw16inc_p(REG4, REG1,DEFAULT);           //    channel->global_gain       = mad_bit_read(ptr, 8);

    VoC_bnez16_r(III_sideinfo_ch_loop_100, REG5)
    VoC_lw16i_short(REG2,4,DEFAULT);
III_sideinfo_ch_loop_100:
    VoC_rbinc_r(REG4,REG2,DEFAULT);                              //   channel->scalefac_compress = mad_bit_read(ptr, lsf ? 9 : 4);
    VoC_lw16i(REG2,288);
    VoC_bngt16_rr(III_sideinfo_ch_loop_200, REG3,REG2)
//      VoC_bngt16_rd(III_sideinfo_ch_loop_200, REG3,CONST_288_ADDR)
    VoC_bnez32_r(III_sideinfo_ch_loop_200, RL7)                 //   if (channel->big_values > 288 && result == 0)
    VoC_lw16i(RL7_LO, MAD_ERROR_BADBIGVALUES);         //  result = MAD_ERROR_BADBIGVALUES;
III_sideinfo_ch_loop_200:
    VoC_sw16inc_p(REG4, REG1,DEFAULT);

    /* window_switching_flag */
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,1,DEFAULT);
    VoC_sw16inc_p(RL7_HI, REG1,IN_PARALLEL);              //     channel->flags = 0;
    VoC_bez16_r(III_sideinfo_ch_loop_300, REG4)   //if (mad_bit_read(ptr, 1)) {
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,2,DEFAULT);         //    channel->block_type = mad_bit_read(ptr, 2);

    VoC_bnez16_r(III_sideinfo_ch_loop_1000, REG4)
    VoC_bnez32_r(III_sideinfo_ch_loop_1000, RL7)     //  if (channel->block_type == 0 && result == 0)
    VoC_lw16i(RL7_LO, MAD_ERROR_BADBLOCKTYPE);    //   result = MAD_ERROR_BADBLOCKTYPE;
III_sideinfo_ch_loop_1000:
    VoC_sw16inc_p(REG4, REG1,DEFAULT);

    VoC_bne16_rd(III_sideinfo_ch_loop_1100, REG4,CONST_2_ADDR)
    VoC_bnez16_r(III_sideinfo_ch_loop_1100, REG5)
    VoC_bnez32_r(III_sideinfo_ch_loop_1100, RL7)
    VoC_movreg16(REG3,ACC0_LO,DEFAULT);
    VoC_bngtz16_r(III_sideinfo_ch_loop_1100, REG3)      //  if (!lsf && channel->block_type == 2 && si->scfsi[ch] && result == 0)
    VoC_lw16i(RL7_LO, MAD_ERROR_BADSCFSI);      //     result = MAD_ERROR_BADSCFSI;
III_sideinfo_ch_loop_1100:
    // REG4   :       channel->block_type
    /*
    if (mad_bit_read(ptr, 1))
      channel->flags |= mixed_block_flag;
    else if (channel->block_type == 2)
      channel->region0_count = 8;
    */
    VoC_sub16_rd(REG2, REG1,CONST_2_ADDR);
    VoC_rbinc_i(REG4,1,DEFAULT);
    VoC_movreg16(REG3,REG4,IN_PARALLEL);
    VoC_bngtz16_r(III_sideinfo_ch_loop_2000, REG4)
    VoC_lw16_p(REG4, REG2,DEFAULT);
    VoC_or16_ri(REG4, mixed_block_flag);
    VoC_NOP();
    VoC_sw16_p(REG4, REG2,DEFAULT);
III_sideinfo_ch_loop_2000:
    VoC_lw16i_short(REG2, 7,DEFAULT);     // channel->region0_count = 7;
    VoC_bne16_rd(III_sideinfo_ch_loop_2100, REG3,CONST_2_ADDR)
    VoC_lw16i_short(REG2, 8,DEFAULT);
III_sideinfo_ch_loop_2100:
//  for (i = 0; i < 2; ++i)
//    channel->table_select[i] = mad_bit_read(ptr, 5);

    VoC_NOP();
    VoC_lbinc_p(REG0);
    VoC_loop_i_short(2,DEFAULT);
    VoC_startloop0
    VoC_rbinc_i(REG4,5,DEFAULT);
    VoC_lw16i_short(ACC1_LO,4,DEFAULT);
    VoC_sw16inc_p(REG4, REG1,DEFAULT);
    VoC_endloop0
    VoC_sw16inc_p(ACC1_LO, REG1,DEFAULT);

//  for (i = 0; i < 3; ++i)
//    channel->subblock_gain[i] = mad_bit_read(ptr, 3);
    VoC_lbinc_p(REG0);
    VoC_loop_i_short(3,DEFAULT);
    VoC_startloop0
    VoC_rbinc_i(REG4,3,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG4, REG1,DEFAULT);
    VoC_endloop0

    VoC_lw16i(REG4, 36);    //  channel->region1_count = 36;
    VoC_sw16inc_p(REG2, REG1,DEFAULT);
    VoC_sw16inc_p(REG4, REG1,DEFAULT);
    VoC_jump(III_sideinfo_ch_loop_300_end)
III_sideinfo_ch_loop_300:
    VoC_sw16inc_p(RL7_HI, REG1,DEFAULT);           //       channel->block_type = 0;

    VoC_lbinc_p(REG0);
    VoC_lbinc_p(REG0);
    VoC_loop_i_short(3,DEFAULT);
    VoC_startloop0                       //for (i = 0; i < 3; ++i)
    VoC_rbinc_i(REG4,5,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG4, REG1,DEFAULT);      //       channel->table_select[i] = mad_bit_read(ptr, 5);
    VoC_endloop0

    VoC_lbinc_p(REG0);

    VoC_add16_rd(REG1, REG1,CONST_3_ADDR);
    VoC_rbinc_i(REG4,4,DEFAULT);                    //channel->region0_count = mad_bit_read(ptr, 4);
    VoC_rbinc_i(REG4,3,DEFAULT);                    //channel->region1_count = mad_bit_read(ptr, 3);
    exit_on_warnings =OFF;
    VoC_sw16inc_p(REG4, REG1,DEFAULT);
    VoC_sw16inc_p(REG4, REG1,DEFAULT);
    exit_on_warnings =ON;
III_sideinfo_ch_loop_300_end:
    /* [preflag,] scalefac_scale, count1table_select */
    //   channel->flags |= mad_bit_read(ptr, lsf ? 2 : 3);
    VoC_sub16_rd(REG2, REG1, CONST_10_ADDR); //REG2 ->       channel->flags

    VoC_lw16i_short(REG4,2,DEFAULT);
    VoC_lw16_p(REG3, REG2,DEFAULT);
    VoC_bgtz16_r(III_sideinfo_ch_loop_400, REG5)
    VoC_lw16i_short(REG4,3,DEFAULT);
III_sideinfo_ch_loop_400:
    VoC_rbinc_r(REG4, REG4,DEFAULT);
    VoC_or16_rr(REG3, REG4,DEFAULT);
    VoC_lw16i_short(REG4,40,IN_PARALLEL);
//      VoC_add16_rd(REG1, REG1,CONST_40_ADDR);
    VoC_add16_rr(REG1, REG1,REG4,DEFAULT);
    VoC_sw16_p(REG3, REG2,DEFAULT);
    VoC_movreg16(ACC0_LO,ACC0_HI,IN_PARALLEL);
    VoC_endloop1

    VoC_lw16_d(REG2, GLOBAL_NCH_ADDR);

    VoC_pop16(REG3,DEFAULT);//,"ngr"

    VoC_lw16i_set_inc(REG1, STRUCT_GRANULE1_BEGIN_ADDR, 1);
    VoC_sub16_rd(REG3, REG3,CONST_1_ADDR);
    VoC_bnez16_r(III_sideinfo_gr_loop_begin, REG3)

    VoC_sw16_d(REG0, STRUCT_BITPTR_PTR_BYTE_ADDR_ADDR);
    VoC_sw32_d(RL6, STRUCT_BITPTR_PTR_CACHE_ADDR);
    VoC_sw16_d(BITCACHE, STRUCT_BITPTR_PTR_LEFT_ADDR);
    VoC_return                        // return result;
}
void  CII_mad_layer_III( void )
{

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_sw16_d(REG7, mad_layer_III_result);
    VoC_bez16_d(NCHANNELS_100, STRUCT_MAD_HEADER_MODE_ADDR)
    VoC_lw16i_short(REG5,2,DEFAULT);      // nch = MAD_NCHANNELS(header);
NCHANNELS_100:     //REG5   :    nch

    VoC_lw16i(REG6, MAD_FLAG_LSF_EXT);
    VoC_sw16_d(REG5, GLOBAL_NCH_ADDR);

    VoC_and16_rd(REG6, STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_lw16i_short(REG7, 9,DEFAULT);
    VoC_be16_rd(mad_layer_III_si_len_100, REG5,CONST_1_ADDR);
    VoC_lw16i_short(REG7, 17,DEFAULT);
mad_layer_III_si_len_100:
    VoC_bnez16_r(mad_layer_III_si_len_500, REG6)
    VoC_lw16i_short(REG7, 17,DEFAULT);
    VoC_be16_rd(mad_layer_III_si_len_500, REG5,CONST_1_ADDR);  //  si_len = (header->flags & MAD_FLAG_LSF_EXT) ?
    VoC_lw16i(REG7, 32);                             //   (nch == 1 ? 9 : 17) : (nch == 1 ? 17 : 32);
mad_layer_III_si_len_500:             //REG7     :       si_len

    /* check frame sanity */
    /*
      if (stream->next_frame - mad_bit_nextbyte(&stream->ptr) <

          (signed int) si_len) {

        stream->error = MAD_ERROR_BADFRAMELEN;
        stream->md_len = 0;
        return -1;
      }
    */
    VoC_lw16_d(REG2, STRUCT_BITPTR_PTR_BYTE_ADDR_ADDR );
    VoC_push16(REG7,DEFAULT);//,"reg7"
    VoC_shr16_ri(REG2, -1,DEFAULT);
    VoC_lw16i_short(REG7,8,IN_PARALLEL);

    VoC_lw16_d(REG6,STRUCT_BITPTR_PTR_LEFT_ADDR);

    VoC_jal(CII_div_mode);
    VoC_sub16_rr(REG2, REG2,REG5,DEFAULT);
    VoC_pop16(REG7,IN_PARALLEL);

    VoC_sub16_dr(REG1, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR, REG2);
    VoC_bgt16_rd(mad_layer_III101,REG1,CONST_0_ADDR)
    VoC_add16_rd(REG1,REG1,CONST_2048_ADDR);
mad_layer_III101:
    VoC_bngt16_rr(mad_layer_III_check_frame, REG7, REG1)
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    VoC_lw16i(REG7, MAD_ERROR_BADFRAMELEN);
    VoC_sw16_d(REG1, STRUCT_MAD_STREAM_MD_LEN_ADDR);            //    stream->md_len = 0;
    VoC_lw16i_short(REG6, -1,DEFAULT);
    VoC_sw16_d(REG7, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);    //   stream->error = MAD_ERROR_BADFRAMELEN;
    VoC_sw16_d(REG7, mad_layer_III_result);
    VoC_return
mad_layer_III_check_frame:
    /* check CRC word */
    /*
      if (header->flags & MAD_FLAG_PROTECTION) {
        header->crc_check =
          mad_bit_crc(stream->ptr, si_len * CHAR_BIT, header->crc_check);

        if (header->crc_check != header->crc_target &&
        !(frame->options & MAD_OPTION_IGNORECRC)) {
          stream->error = MAD_ERROR_BADCRC;
          result = -1;
        }
      }
    */
    VoC_lw16i_short(REG1, MAD_FLAG_PROTECTION,DEFAULT);
//  VoC_set_wrap_i(REG0,10,IN_PARALLEL);
    VoC_lw16i_short(WRAP0,10,IN_PARALLEL);
    VoC_and16_rd(REG1, STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_bez16_r(mad_layer_III_check_CRC, REG1)
    //REG7  :  si_len
    //REG0  :  stream->ptr
    VoC_lw16_d(REG4, STRUCT_MAD_HEADER_CRC_CHECK_ADDR);

    VoC_lw16d_set_inc(REG0, STRUCT_BITPTR_PTR_BYTE_ADDR_ADDR, 1);
    VoC_lw32_d(RL6, STRUCT_BITPTR_PTR_CACHE_ADDR);
    VoC_lw16_d(BITCACHE,  STRUCT_BITPTR_PTR_LEFT_ADDR);
    VoC_shr16_ri(REG7,-3,DEFAULT);
    VoC_jal(CII_mad_bit_crc);
    VoC_sw16_d(RL7_LO,STRUCT_MAD_HEADER_CRC_CHECK_ADDR);

    VoC_movreg16(REG4, RL7_LO,DEFAULT);

    VoC_be16_rd(mad_layer_III_check_CRC, REG4, STRUCT_MAD_HEADER_CRC_TARGET_ADDR)
    VoC_lw16i_short(REG4, MAD_OPTION_IGNORECRC,DEFAULT);
    VoC_lw16i_short(REG7, -1,IN_PARALLEL);
    VoC_and16_rd(REG4, STRUCT_MAD_FRAME_OPTIONS_ADDR);      // !(frame->options & MAD_OPTION_IGNORECRC)
    VoC_lw16i(REG6, MAD_ERROR_BADCRC);
    VoC_bnez16_r(mad_layer_III_check_CRC, REG4)
    VoC_sw16_d(REG6, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);     //      stream->error = MAD_ERROR_BADCRC;
    VoC_sw16_d(REG7, mad_layer_III_result);                                    //       result = -1;
mad_layer_III_check_CRC:
    /* decode frame side information */

// error = III_sideinfo(&stream->ptr, nch, header->flags & MAD_FLAG_LSF_EXT,
//             &si, &data_bitlen, &priv_bitlen);

    VoC_lw16i(REG5, MAD_FLAG_LSF_EXT);
    VoC_and16_rd(REG5, STRUCT_MAD_HEADER_FLAGS_ADDR);

    VoC_lw16d_set_inc(REG0, STRUCT_BITPTR_PTR_BYTE_ADDR_ADDR, 1);
    VoC_lw32_d(RL6, STRUCT_BITPTR_PTR_CACHE_ADDR);
    VoC_lw16_d(BITCACHE,  STRUCT_BITPTR_PTR_LEFT_ADDR);
//input :   REG5  :   lsf
// output   :   REG6     :   data_bitlen         REG7     :   priv_bitlen         RL7_lo    :   error

    VoC_jal(CII_III_sideinfo);



    VoC_movreg16(REG5, RL7_LO,DEFAULT);
    VoC_lw16i_short(REG4, -1,IN_PARALLEL);
//          VoC_set_wrap_i(REG0,16,DEFAULT);
    VoC_lw16i_short(WRAP0,16,DEFAULT);
    VoC_bez16_r(mad_layer_III_check_sideinfo, REG5)
    VoC_bnez16_d(mad_layer_III_check_sideinfo, mad_layer_III_result)     //     if (error && result == 0) {
    VoC_sw16_d(REG5, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);      //  stream->error = error;
    VoC_sw16_d(REG4, mad_layer_III_result);                                     //      result = -1;
mad_layer_III_check_sideinfo:

    VoC_lw32_d(REG45, STRUCT_MAD_HEADER_FLAGS_ADDR );
    VoC_or16_rr(REG4, REG7, DEFAULT);                               //    header->flags        |= priv_bitlen;
    VoC_push16(REG6,IN_PARALLEL);//,  "data_bitlen"

    VoC_or16_rd(REG5, STRUCT_SIDEINFO_private_bits_ADDR);    //     header->private_bits |= si.private_bits;
    VoC_lw16i_short(BITCACHE,0,DEFAULT);
    VoC_sw32_d(REG45, STRUCT_MAD_HEADER_FLAGS_ADDR );

    /* find main_data of next frame */
    VoC_lw16d_set_inc(REG0, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR, 1);
    VoC_lw32z(RL6,DEFAULT);
    VoC_movreg16(REG4, REG0,IN_PARALLEL);

    VoC_and16_ri(REG4,1);
    VoC_shru16_ri(REG0,1,DEFAULT);
    VoC_lw16i_short(REG7, 0,IN_PARALLEL);     // init next_md_begin
    VoC_blt16_rd(mad_layer_III_beyond1,REG0,CONST_1024_ADDR)
    VoC_sub16_rd(REG0,REG0,CONST_1024_ADDR);
mad_layer_III_beyond1:
    VoC_lw16i_short(WRAP0,10,DEFAULT);
    VoC_bez16_r(mad_layer_III_peek_init_even, REG4)
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,8,DEFAULT);
mad_layer_III_peek_init_even:      // mad_bit_init(&peek, stream->next_frame);
    VoC_NOP();
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,16,DEFAULT);        // //    header = mad_bit_read(&peek, 32);    //REG45    :    header
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_lw16i_short(RL7_HI,0xffe6,DEFAULT);
    VoC_lw16i_short(ACC1_HI,0xffe2,DEFAULT);

    VoC_and32_rr(RL7,REG45,DEFAULT);
    VoC_bne32_rr(mad_layer_III_find_main_data, RL7, ACC1) //   if ((header & 0xffe60000L) /* syncword | layer */ == 0xffe20000L) {

    VoC_lw16i_short(ACC1_HI,0x0001,DEFAULT);
    VoC_and32_rr(ACC1,REG45,DEFAULT);
    VoC_bnez32_r(mad_layer_III_find_main_data_100, ACC1)  //      if (!(header & 0x00010000L))  /* protection_bit */
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG6,16,DEFAULT);                                 //    mad_bit_skip(&peek, 16);  /* crc_check */
mad_layer_III_find_main_data_100:
    VoC_NOP();
    VoC_lbinc_p(REG0);
    VoC_lw16i_short(REG6, 9,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_lw16i_short(ACC1_HI,0x0008,DEFAULT);
    VoC_and32_rr(REG45,ACC1,DEFAULT);
    //      VoC_and32_rd(REG45,CONST_0x00080000_ADDR);

    VoC_bgtz32_r(mad_layer_III_find_main_data_200, REG45)
    VoC_lw16i_short(REG6, 8,DEFAULT);
mad_layer_III_find_main_data_200:          //      next_md_begin =
    VoC_rbinc_r(REG7,REG6,DEFAULT);           //    mad_bit_read(&peek, (header & 0x00080000L) /* ID */ ? 9 : 8);
mad_layer_III_find_main_data:
    // REG7     :   next_md_begin
    VoC_lw16i_short(WRAP0,16,DEFAULT);
    /* find main_data of this frame */

    VoC_lw16_d(REG6, STRUCT_BITPTR_PTR_LEFT_ADDR);
    VoC_push16(REG7,DEFAULT);//,"REG7"
    VoC_lw16i_short(REG7,8,IN_PARALLEL);
    VoC_jal(CII_div_mode);
    VoC_lw16_d(REG4, STRUCT_BITPTR_PTR_BYTE_ADDR_ADDR);
    VoC_shr16_ri(REG4,-1,DEFAULT);
    VoC_sub16_rr(REG4, REG4, REG5,DEFAULT);
    VoC_pop16(REG7,IN_PARALLEL);
    VoC_sub16_dr(REG6, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR, REG4); //  frame_space = stream->next_frame - mad_bit_nextbyte(&stream->ptr);
    VoC_bgt16_rd(mad_layer_III102,REG6,CONST_0_ADDR)
    VoC_add16_rd(REG6,REG6,CONST_2048_ADDR);
mad_layer_III102:
    //REG6     :    frame_space
    VoC_add16_rd(REG5, REG6, STRUCT_SIDEINFO_main_data_begin_ADDR);
    VoC_bngt16_rr(find_main_data_this_frame_100, REG7, REG5)         //   if (next_md_begin > si.main_data_begin + frame_space)
    VoC_lw16i_short(REG7, 0,DEFAULT);                             //     next_md_begin = 0;
find_main_data_this_frame_100:

    VoC_sub16_rr(REG5, REG6, REG7,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);          //     frame_used = 0;

    VoC_add16_rd(REG5, REG5, STRUCT_SIDEINFO_main_data_begin_ADDR);     //   md_len = si.main_data_begin + frame_space - next_md_begin;
    // REG4    :    frame_used        REG5    :    md_len
//  REG6     :    frame_space       REG7     :   next_md_begin
//if (si.main_data_begin == 0) {
    VoC_bnez16_d(find_main_data_this_frame_500, STRUCT_SIDEINFO_main_data_begin_ADDR)
    VoC_sw16_d(REG4, STRUCT_MAD_STREAM_MD_LEN_ADDR);  // stream->md_len = 0;
    VoC_movreg16(REG4,REG5,DEFAULT);                                 //     frame_used = md_len;
    VoC_lw16i_short(REG2, 1,IN_PARALLEL);
    VoC_lw16d_set_inc(REG0, STRUCT_BITPTR_PTR_BYTE_ADDR_ADDR,1);
    VoC_lw32_d(RL6, STRUCT_BITPTR_PTR_CACHE_ADDR);
    VoC_lw16_d(BITCACHE, STRUCT_BITPTR_PTR_LEFT_ADDR);  //    ptr = stream->ptr;
    VoC_sw16_d(REG2, GLOBAL_WRAP_INDEX_ADDR);
    VoC_sw16_d(REG0, PTR_BYTE_ADDR_ADDR);
    VoC_sw32_d(RL6, PTR_CACHE_ADDR);
    VoC_sw16_d(BITCACHE, PTR_LEFT_ADDR);  //    ptr

    VoC_jump(find_main_data_this_frame_500_end);
find_main_data_this_frame_500:

    VoC_lw16_d(REG3, STRUCT_SIDEINFO_main_data_begin_ADDR);
    VoC_bngt16_rd(find_main_data_this_frame_550, REG3, STRUCT_MAD_STREAM_MD_LEN_ADDR)  //  if (si.main_data_begin > stream->md_len) {
    VoC_lw16i(REG3, MAD_ERROR_BADDATAPTR);
    VoC_lw16i_short(REG2, -1,DEFAULT);
    VoC_bez16_d(find_main_data_this_frame_500_1, mad_layer_III_result)   //   if (result == 0) {
    VoC_jump(find_main_data_this_frame_500_end);
find_main_data_this_frame_500_1:
    VoC_sw16_d(REG3, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);    //   stream->error = MAD_ERROR_BADDATAPTR;
    VoC_sw16_d(REG2, mad_layer_III_result);                         //  result = -1;
    VoC_jump(find_main_data_this_frame_500_end);
find_main_data_this_frame_550:          // else {

    VoC_lw16i_short(BITCACHE,0,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0, STATIC_MAD_MAIN_DATA_ADDR, 1);

    VoC_shru16_ri(REG0,-1,DEFAULT);
    VoC_push16(REG4,IN_PARALLEL);//, "REG4"
    VoC_add16_rd(REG0, REG0, GLOBAL_MAIN_DATA_OFFSET_ADDR);
    VoC_add16_rd(REG0, REG0, STRUCT_MAD_STREAM_MD_LEN_ADDR);
    VoC_movreg16(REG3, REG0,DEFAULT);

    VoC_sub16_rd(REG0, REG0, STRUCT_SIDEINFO_main_data_begin_ADDR);

    VoC_shru16_ri(REG0,1,DEFAULT);
    VoC_movreg16(REG4, REG0,IN_PARALLEL);
    VoC_and16_ri(REG4,1);
    VoC_bez16_r(mad_layer_III_md_lent_even, REG4)
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,8,DEFAULT);               //     mad_bit_init(&ptr,
mad_layer_III_md_lent_even:             //         *stream->main_data + stream->md_len - si.main_data_begin);
    VoC_pop16(REG4,DEFAULT);
    VoC_lw16i_short(REG2, 0,IN_PARALLEL);
    VoC_sw16_d(REG0, PTR_BYTE_ADDR_ADDR);
    VoC_sw32_d(RL6, PTR_CACHE_ADDR);
    VoC_sw16_d(BITCACHE, PTR_LEFT_ADDR);  //    ptr
    VoC_sw16_d(REG2, GLOBAL_WRAP_INDEX_ADDR);

//    if (md_len > si.main_data_begin) {
//  assert(stream->md_len + md_len -
//         si.main_data_begin <= MAD_BUFFER_MDLEN);

    VoC_bgt16_rd(no_find_main_data_this_frame_500_end, REG5, STRUCT_SIDEINFO_main_data_begin_ADDR)
    VoC_jump(find_main_data_this_frame_500_end);
no_find_main_data_this_frame_500_end:

//  memcpy(*stream->main_data + stream->md_len,
//         mad_bit_nextbyte(&stream->ptr),
//         frame_used = md_len - si.main_data_begin);

    //REG3   :  2  (  *stream->main_data + stream->md_len),

    VoC_sub16_rd(REG4, REG5, STRUCT_SIDEINFO_main_data_begin_ADDR);   //        frame_used = md_len - si.main_data_begin
    VoC_add16_rd(REG2, REG4, STRUCT_MAD_STREAM_MD_LEN_ADDR);
    VoC_push32(REG67,DEFAULT);
    VoC_push32(REG45,DEFAULT);

    VoC_lw16d_set_inc(REG0, STRUCT_BITPTR_PTR_BYTE_ADDR_ADDR,1);
    VoC_lw32_d(RL6, STRUCT_BITPTR_PTR_CACHE_ADDR);
    VoC_lw16_d(BITCACHE, STRUCT_BITPTR_PTR_LEFT_ADDR);


    VoC_movreg16(REG2, REG3,DEFAULT);
    VoC_shru16_ri(REG3,1,IN_PARALLEL);

    VoC_and16_ri(REG2,1);
    VoC_lw16i_short(WRAP0,10,DEFAULT);
    VoC_blt16_rd(mad_layer_III_beyond3,REG0,CONST_1024_ADDR)
    VoC_sub16_rd(REG0,REG0,CONST_1024_ADDR);
mad_layer_III_beyond3:
    VoC_bez16_r(mad_layer_III_copy_500, REG2)
    VoC_lw16_p(REG6, REG3,DEFAULT);
    VoC_and16_ri(REG6,0x00ff);
    VoC_rbinc_i(REG7,8,DEFAULT);
    VoC_shru16_ri(REG7, -8,DEFAULT);
    VoC_or16_rr(REG6, REG7,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_sub16_rd(REG4, REG4,CONST_1_ADDR);
    VoC_sw16inc_p(REG6, REG3,DEFAULT);
mad_layer_III_copy_500:

    VoC_add16_rd(REG6, REG4,CONST_1_ADDR);   //maybe copy  one more byte!!!

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_shr16_ri(REG6,2,IN_PARALLEL);

    VoC_add16_rd(REG6, REG6,CONST_1_ADDR);   //maybe copy  one more byte!!!
    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_loop_r_short(REG6,DEFAULT)
    VoC_startloop0
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_shru16_ri(REG5, -8,IN_PARALLEL);
    VoC_rbinc_i(REG7,8,DEFAULT);
    VoC_or16_rr(REG4, REG5,DEFAULT);
    VoC_shru16_ri(REG7, -8,IN_PARALLEL);
    VoC_or16_rr(REG6, REG7,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_sw16inc_p(REG4, REG3,IN_PARALLEL);
    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_sw16inc_p(REG6, REG3,IN_PARALLEL);
    VoC_endloop0

    VoC_lw16i_short(WRAP0,16,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);
    VoC_pop32(REG67,DEFAULT);
find_main_data_this_frame_500_end:
    // REG4    :    frame_used        REG5    :    md_len
//  REG6     :    frame_space       REG7     :   next_md_begin
    VoC_sub16_rr(REG6, REG6, REG4,DEFAULT);         //      frame_free = frame_space - frame_used;
    VoC_pop16(REG4,DEFAULT );//,  "data_bitlen"
//Now the  'frame_used' and  ' frame_space'  is useless, and REG6 is put for  'frame_free'
// The REG4 is poped for  "data_bitlen "  ,Then all those are push to stack32 !!!
//   REG4   :    data_bitlen       REG5    :    md_len     REG6     :    frame_free        REG7     :   next_md_begin
    VoC_push32(REG67,DEFAULT);
    VoC_push32(REG45,DEFAULT);
    /* decode main_data */

    /*  if (result == 0) {
        error = III_decode(&ptr, frame, &si, nch);
        if (error) {
          stream->error = error;
          result = -1;
        }
      }
    */
//    VoC_sw16_d(REG0, STRUCT_BITPTR_PTR_BYTE_ADDR_ADDR);
    //    VoC_sw32_d(RL6, STRUCT_BITPTR_PTR_CACHE_ADDR);
    //     VoC_sw16_d(BITCACHE, STRUCT_BITPTR_PTR_LEFT_ADDR);  //    ptr = stream->ptr;
    VoC_bnez16_d(decode_main_data_end, mad_layer_III_result)

    VoC_jal(CII_III_decode);
    VoC_lw16i_short(REG7, -1,DEFAULT);
    VoC_bez16_r(decode_main_data_end, REG4)
    VoC_sw16_d(REG4, STRUCT_MAD_STREAM_MAD_ERROR_ADDR);    //    stream->error = error;
    VoC_sw16_d(REG7, mad_layer_III_result);                         //  result = -1;

decode_main_data_end:
    /* designate ancillary bits */
    VoC_pop32(REG45,DEFAULT);   // //   REG4   :    data_bitlen       REG5    :    md_len
    /***************************************************
      stream->anc_ptr    = ptr;
    *************************************/
    VoC_lw16i_set_inc(REG0, PTR_BYTE_ADDR_ADDR, 1);
    VoC_lw16i_set_inc(REG1, STRUCT_BITPTR_ANC_PTR_BYTE_ADDR_ADDR, 1);
    VoC_lw16inc_p(REG3, REG0,DEFAULT);
    VoC_loop_i_short(4,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG3, REG0,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(REG3, REG1,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop0

    VoC_movreg16(REG3, REG5,DEFAULT);
    VoC_shr16_ri(REG3, -3,DEFAULT);
    VoC_sub16_rr(REG3, REG3, REG4,DEFAULT);
    VoC_pop32(REG67,DEFAULT);       //   REG6     :    frame_free        REG7     :   next_md_begin
    VoC_sw16_d(REG3, STRUCT_MAD_STREAM_ANC_BITLEN_ADDR);//  stream->anc_bitlen = md_len * CHAR_BIT - data_bitlen;

    /* preload main_data buffer with up to 511 bytes for next frame(s) */

//   REG6     :    frame_free        REG7     :   next_md_begin
    VoC_bgt16_rr(preload_main_data, REG7, REG6);   // if (frame_free >= next_md_begin)
    //    stream->md_len = next_md_begin;
    VoC_sw16_d(REG7, STRUCT_MAD_STREAM_MD_LEN_ADDR);
//   memcpy(*stream->main_data,
//     stream->next_frame - next_md_begin, next_md_begin);
    VoC_lw16i_set_inc(REG1, STATIC_MAD_MAIN_DATA_ADDR, 2);
    VoC_sub16_dr(REG0, STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR, REG7);
    VoC_bgtz16_r(mad_layer_III103,REG0)
    VoC_add16_rd(REG0,REG0,CONST_2048_ADDR);
mad_layer_III103:
    VoC_movreg16(REG4, REG0,DEFAULT);
    VoC_and16_ri(REG4,1);
    VoC_shru16_ri(REG0,1,DEFAULT);
    VoC_lw16i_short(WRAP0,10,IN_PARALLEL);

    VoC_blt16_rd(mad_layer_III_beyond2,REG0,CONST_1024_ADDR)
    VoC_sub16_rd(REG0,REG0,CONST_1024_ADDR);
mad_layer_III_beyond2:
    VoC_sw16_d(REG4, GLOBAL_MAIN_DATA_OFFSET_ADDR);

    VoC_shr16_ri(REG7, 2,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_add16_rd(REG7, REG7,CONST_1_ADDR);
    VoC_loop_r_short(REG7,DEFAULT);
    VoC_lw16inc_p(REG5,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32inc_p(REG45, REG1,DEFAULT);
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop0
    VoC_jump(preload_main_data_end)

preload_main_data:
//        REG5    :    md_len     //   REG6     :    frame_free        REG7     :   next_md_begin
    VoC_lw16i_short(WRAP0,16,DEFAULT);
    VoC_lw16i_short(REG2,0,DEFAULT);      //  else    stream->md_len = 0;

    VoC_blt16_rd(no_preload_main_data_100, REG5, STRUCT_SIDEINFO_main_data_begin_ADDR)
    VoC_jump(preload_main_data_100);
no_preload_main_data_100:
    // if (md_len < si.main_data_begin) {
    //        unsigned int extra;
    // extra = si.main_data_begin - md_len;
    VoC_sub16_dr(REG2, STRUCT_SIDEINFO_main_data_begin_ADDR, REG5);
    //   REG2  :    extra
    VoC_add16_rr(REG4, REG2, REG6,DEFAULT);
    VoC_bngt16_rr(preload_main_data_200, REG4, REG7)   //     if (extra + frame_free > next_md_begin)
    VoC_sub16_rr(REG2, REG7, REG6,DEFAULT);        //   extra = next_md_begin - frame_free;

preload_main_data_200:
    /*
          if (extra < stream->md_len) {
        memmove(*stream->main_data,
            *stream->main_data + stream->md_len - extra, extra);
        stream->md_len = extra;
          }
    */
    VoC_bnlt16_rd(preload_main_data_1aa, REG2, STRUCT_MAD_STREAM_MD_LEN_ADDR)
    VoC_lw16i_set_inc(REG3, STATIC_MAD_MAIN_DATA_ADDR, 2);
//      VoC_mult16_rd(REG4, REG3,CONST_2_ADDR);
    VoC_push16(REG2,DEFAULT);
    VoC_lw16_d(REG4,GLOBAL_MAIN_DATA_OFFSET_ADDR);
//      VoC_add16_rd(REG4, REG4, GLOBAL_MAIN_DATA_OFFSET_ADDR);
    VoC_add16_rd(REG0, REG4, STRUCT_MAD_STREAM_MD_LEN_ADDR);

    VoC_sub16_rr(REG0, REG0, REG2,DEFAULT);

    VoC_movreg16(REG4, REG0,DEFAULT);
    VoC_shru16_ri(REG0,1,IN_PARALLEL);
    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);
    VoC_and16_ri(REG4,1);
//  VoC_lw16i_short(WRAP0,10,DEFAULT);
    VoC_shr16_ri(REG2,2,DEFAULT);

    VoC_add16_rd(REG2, REG2,CONST_1_ADDR);  //maybe move  more byte!!!
    VoC_sw16_d(REG4, GLOBAL_MAIN_DATA_OFFSET_ADDR);
    VoC_lw16inc_p(REG4,REG0,DEFAULT);

    VoC_loop_r_short(REG2,DEFAULT);
    VoC_lw16inc_p(REG5,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32inc_p(REG45, REG3,DEFAULT);
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop0

//  VoC_lw16i_short(WRAP0,16,DEFAULT);
    VoC_pop16(REG2,DEFAULT);
    VoC_jump(preload_main_data_100)
preload_main_data_1aa:
    VoC_lw16_d(REG2, STRUCT_MAD_STREAM_MD_LEN_ADDR);
preload_main_data_100:
//    memcpy(*stream->main_data + stream->md_len,
//     stream->next_frame - frame_free, frame_free);
    VoC_lw16i_set_inc(REG3, STATIC_MAD_MAIN_DATA_ADDR, 1);
    VoC_sw16_d(REG2, STRUCT_MAD_STREAM_MD_LEN_ADDR);

    VoC_shru16_ri(REG3,-1,DEFAULT);
    VoC_add16_rd(REG3, REG3, GLOBAL_MAIN_DATA_OFFSET_ADDR);
    VoC_add16_rr(REG3, REG3, REG2,DEFAULT);

    VoC_movreg16(REG2, REG3,DEFAULT);
    VoC_shru16_ri(REG3,1,IN_PARALLEL);

    VoC_sub16_dr(REG0,STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR, REG6);
    VoC_bgt16_rd(mad_layer_III104,REG0,CONST_0_ADDR)
    VoC_add16_rd(REG0,REG0,CONST_2048_ADDR);
mad_layer_III104:

    VoC_lw16i_short(BITCACHE,0,DEFAULT);
    VoC_lw32z(RL6,DEFAULT);
    VoC_movreg16(REG4, REG0,IN_PARALLEL);

    VoC_and16_ri(REG4,1);
    VoC_shru16_ri(REG0,1,DEFAULT);
    VoC_lw16i_short(WRAP0,10,IN_PARALLEL);

    VoC_blt16_rd(mad_layer_III_beyond4,REG0,CONST_1024_ADDR)
    VoC_sub16_rd(REG0,REG0,CONST_1024_ADDR);
mad_layer_III_beyond4:
    VoC_bez16_r(mad_layer_III_copy_200, REG4)
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,8,DEFAULT);
mad_layer_III_copy_200:


    VoC_add16_rd(REG4, REG6, STRUCT_MAD_STREAM_MD_LEN_ADDR);
    VoC_lbinc_p(REG0);
    VoC_sw16_d(REG4, STRUCT_MAD_STREAM_MD_LEN_ADDR);        //  stream->md_len += frame_free;

    VoC_and16_ri(REG2,1);
    VoC_bez16_r(mad_layer_III_copy_300, REG2)
    VoC_lw16_p(REG4, REG3,DEFAULT);
    VoC_and16_ri(REG4,0x00ff);
    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_shru16_ri(REG5, -8,DEFAULT);
    VoC_or16_rr(REG4, REG5,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_sub16_rd(REG6, REG6,CONST_1_ADDR);
    VoC_sw16inc_p(REG4, REG3,DEFAULT);
mad_layer_III_copy_300:
    VoC_NOP();
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_shr16_ri(REG6,2,IN_PARALLEL);

    VoC_add16_rd(REG6, REG6,CONST_1_ADDR);   //maybe copy  one more woprd!!!
    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_loop_r_short(REG6,DEFAULT)
    VoC_startloop0
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_shru16_ri(REG5, -8,IN_PARALLEL);
    VoC_rbinc_i(REG7,8,DEFAULT);
    VoC_or16_rr(REG4, REG5,DEFAULT);
    VoC_shru16_ri(REG7, -8,IN_PARALLEL);

    VoC_or16_rr(REG6, REG7,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_sw16inc_p(REG4, REG3,IN_PARALLEL);
    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_sw16inc_p(REG6, REG3,IN_PARALLEL);
    VoC_endloop0

preload_main_data_end:
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(WRAP0,16,DEFAULT);
    VoC_return         //  return result;

}

/****************************************************************************************
 *Function : III_reorder()
 *INPUT:      xr[]        ,    ->REG0
 *            channel     ,    ->REG7
 *            sfbwidth[]  ,    ->REG1(incr 1)
 *
 *OUTPUT:
 *CHANGED registers : REG0
 *Description:
 *Version 1.0  Created by BinWang   06/01/2005
 *Version 1.1  optimized by Kenneth   07/11/2005
 ***************************************************************************************/
void CII_III_reorder(void)
{
#if 0
    voc_short LOCAL_sbw_ADDR, 3, y
    voc_short LOCAL_sw_ADDR, 3, y
    voc_alias LOCAL_III_reorder_tmp_ADDR  (STATIC_MAD_SBSAMPLE_ADDR+3*576*2), y
#endif

    VoC_add16_rd(REG5,REG7,CONST_4_ADDR);                     //REG5 :  &channel->flags
    VoC_lw16i_set_inc(REG2,LOCAL_sbw_ADDR,2);
    VoC_lw16_p(REG6,REG5,DEFAULT);                            //REG6 : channel->flags
    VoC_lw16i_short(REG3,mixed_block_flag,IN_PARALLEL);
    VoC_and16_rr(REG6,REG3,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);//sb = 0
    VoC_bez16_r(III_reorder_L2,REG6);                         //if (channel->flags & mixed_block_flag)

    VoC_lw16i_short(REG7,2,DEFAULT);                          //sb = 2
    VoC_lw16i_short(REG6,0,IN_PARALLEL);                      //l = 0

III_reorder_L1:
    VoC_add16inc_rp(REG6,REG6,REG1,DEFAULT);                     //l += *sfbwidth++
    VoC_blt16_rd(III_reorder_L1,REG6,CONST_36_ADDR);          //while (l < 36)

III_reorder_L2:
    VoC_lw32z(REG45,DEFAULT);           //REG5 : w=0

    VoC_loop_i_short(3,DEFAULT);                              //for (w = 0; w < 3; ++w)
    VoC_movreg16(ACC0_HI,REG2,IN_PARALLEL); //ACC0_HI : sb
    VoC_startloop0
    VoC_sw32inc_p(REG45,REG2,DEFAULT);                     //sbw[w] = sb  sw[w]  = 0
    VoC_lw16i_short(REG3,18,IN_PARALLEL);
    VoC_endloop0

    VoC_mult16_rr(REG3,REG7,REG3,DEFAULT);
    VoC_movreg16(REG2,ACC0_HI,DEFAULT);

    VoC_lw16inc_p(REG5,REG1,DEFAULT);                      //f = *sfbwidth++
    VoC_add16_rr(REG0,REG0,REG3,IN_PARALLEL);                  //REG0 : &xr[l]
    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);
    VoC_sub16_dr(REG7,CONST_576_ADDR,REG3);
    VoC_add16_rr(REG7,REG7,REG7,DEFAULT);
    VoC_movreg16(RL7_LO,REG0,DEFAULT);
    VoC_movreg16(RL7_HI,REG7,IN_PARALLEL);
    VoC_lw16i(ACC0_LO,LOCAL_III_reorder_tmp_ADDR);
III_reorder_L3:
    VoC_blt16_rd(no_III_reorder_L4,REG3,CONST_576_ADDR);         //for (l = 18 * sb; l < 576; ++l)
    VoC_jump(III_reorder_L4);
no_III_reorder_L4:

    VoC_bnez16_r(reorder_L3_1,REG5);                           //if (f-- == 0)
    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_movreg16(REG6,REG4,IN_PARALLEL);
    //w = (w + 1) % 3;// w in reg4
    VoC_bnez16_r(III_reorder_Lw1,REG4);
    VoC_lw16i_short(REG4,2,DEFAULT);
    VoC_jump(reorder_L3_1);
III_reorder_Lw1:
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_bne16_rd(reorder_L3_1,REG6,CONST_2_ADDR);
    VoC_lw16i_short(REG4,4,DEFAULT);
reorder_L3_1:
    VoC_add16_rr(REG2,REG4,REG2,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_sub16_rd(REG5,REG5,CONST_1_ADDR);                      //f = *sfbwidth++ - 1
    VoC_movreg32(RL6,REG45,DEFAULT);
    VoC_lw32_p(REG67,REG2,IN_PARALLEL);
    VoC_mult16_rd(REG4,REG4,CONST_6_ADDR);
    VoC_mult16_rd(REG5,REG6,CONST_36_ADDR);
    VoC_NOP();
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_movreg16(REG5,ACC0_LO,DEFAULT);                         //REG2 :  sw[w]
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);
    VoC_add16_rr(REG7,REG5,REG7,IN_PARALLEL);

    VoC_bne16_rd(reorder_L3_2,REG7,CONST_6_ADDR);              //if (sw[w] == 6)
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG5,IN_PARALLEL);
reorder_L3_2:
    VoC_lw32inc_p(ACC1,REG0,DEFAULT);
    VoC_sw32_p(REG67,REG2,DEFAULT);
    VoC_movreg16(REG2,ACC0_HI,IN_PARALLEL);
    VoC_sw32_p(ACC1,REG4,DEFAULT);
    VoC_movreg32(REG45,RL6,IN_PARALLEL);
    VoC_jump(III_reorder_L3);
III_reorder_L4:
    VoC_movreg16(REG2,ACC0_LO,DEFAULT);
    VoC_movreg32(REG01,RL7,IN_PARALLEL);
    VoC_shr16_ri(REG1,3,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_loop_i(1,2)
    VoC_loop_r(0,REG1);
    VoC_lw32inc_p(REG67,REG2,DEFAULT);
    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1

    VoC_return;
}

void CII_III_decode(void )
{
#if 0

    voc_short LOCAL_III_decode_sfbwidth_ADDR, 2, y
#endif

    {
#if 0
        voc_short LOCAL_III_decode_sblimit_ADDR, 2, y
        voc_word  LOCAL_III_decode_output_ADDR, 36,y
#endif
    }

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_lw16_d(REG6, STRUCT_MAD_HEADER_SAMPLERATE_ADDR);  //    sfreq = header->samplerate;
    VoC_lw16i(REG3, MAD_FLAG_MPEG_2_5_EXT);
    VoC_and16_rd(REG3, STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_bngtz16_r(III_decode_sfreq, REG3)      //  if (header->flags & MAD_FLAG_MPEG_2_5_EXT)
    VoC_shr32_ri(REG67,-1,DEFAULT);          //   sfreq *= 2;
III_decode_sfreq:
    VoC_movreg32(REG45, REG67,DEFAULT);      //  /* 48000 => 0, 44100 => 1, 32000 => 2,
    VoC_shru32_ri(REG67,7,IN_PARALLEL);          //    24000 => 3, 22050 => 4, 16000 => 5 */

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_shru32_ri(REG45,15,IN_PARALLEL);
    VoC_and16_ri(REG6,  0x000f );
    VoC_and16_ri(REG4,  0x0001 );
    VoC_add16_rr(REG6, REG6, REG4,DEFAULT);     //    sfreqi = ((sfreq >>  7) & 0x000f) +
    VoC_sub16_rd(REG6, REG6,CONST_8_ADDR);  //             ((sfreq >> 15) & 0x0001) - 8;
    VoC_bngtz16_r(III_decode_sfreq_100, REG3)      //  if (header->flags & MAD_FLAG_MPEG_2_5_EXT)
    VoC_add16_rd(REG6, REG6, CONST_3_ADDR);         //      sfreqi += 3;
III_decode_sfreq_100:      // REG6    :     sfreqi
    VoC_mult16_rd(REG6, REG6,CONST_3_ADDR);
    VoC_lw16i(REG3, TABLE_sfbwidth_table_ADDR);
    VoC_add16_rr(REG3, REG6, REG3,DEFAULT);       //REG3   ->   sfbwidth_table[sfreqi].l
    /* scalefactors, Huffman decoding, requantization */
    VoC_lw16i(REG5, MAD_FLAG_LSF_EXT);
    VoC_and16_rd(REG5, STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_bgtz16_r(III_decode_ngr, REG5)
    VoC_lw16i_short(REG7,2,DEFAULT);    //  ngr = (header->flags & MAD_FLAG_LSF_EXT) ? 1 : 2;
III_decode_ngr:      //REG7  :    ngr
    VoC_lw16i_set_inc(REG1, STRUCT_GRANULE0_BEGIN_ADDR, 1);
    VoC_sw16_d(REG7, GLOBAL_NGR_ADDR);
//  ngr = REGS[7].reg;
    VoC_lw16i_short(REG7, 0,DEFAULT);
III_decode_ngr_loop_begin:
    VoC_push16(REG1,DEFAULT);   //," granule"
    VoC_push16(REG7,DEFAULT);//," gr"
    VoC_lw16i_short(REG7, 0,IN_PARALLEL);
    VoC_push16(REG3, DEFAULT);//," sfbwidth"
    VoC_lw16i_set_inc(REG2, LOCAL_III_decode_sfbwidth_ADDR,1);

    VoC_lw16d_set_inc(REG0, PTR_BYTE_ADDR_ADDR,1);
    VoC_lw32_d(RL6, PTR_CACHE_ADDR);
    VoC_lw16_d(BITCACHE, PTR_LEFT_ADDR);  //    ptr
    VoC_bez16_d(III_decode_nch_loop_begin, GLOBAL_WRAP_INDEX_ADDR)
    VoC_lw16i_short(WRAP0,10,DEFAULT);
III_decode_nch_loop_begin:
    VoC_push16(REG7,DEFAULT);//, " ch"
    VoC_push16(REG3, DEFAULT);//,"REG3"
    VoC_add16_rd(REG5, REG1,CONST_4_ADDR);  // point to channel->flags
    VoC_lw16inc_p(REG4,REG3,DEFAULT);           //   sfbwidth_table[sfreqi].l;
    VoC_lw32_p(REG67, REG5,DEFAULT);           // REG6 :  channel->flags  REG7 :  channel->block_type

    VoC_bne16_rd(III_decode_sfbwidth_100, REG7,CONST_2_ADDR)
    VoC_lw16inc_p(REG4, REG3,DEFAULT);   //  sfbwidth_table[sfreqi].m
    VoC_and16_ri(REG6,mixed_block_flag);
    VoC_bngtz16_r(III_decode_sfbwidth_100, REG6)
    VoC_lw16inc_p(REG4, REG3,DEFAULT);     // sfbwidth_table[sfreqi].s;
III_decode_sfbwidth_100:
    VoC_pop16(REG3, DEFAULT);
    VoC_sw16inc_p(REG4, REG2,DEFAULT);

    VoC_lw16i(REG5,MAD_FLAG_LSF_EXT );
    VoC_push32(REG23,DEFAULT);//,"REG23/n"

    VoC_and16_rd(REG5, STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_push16(REG1,DEFAULT );//,"channel/n "
    VoC_push16(REG0,DEFAULT);//, " REG0/n  "
    VoC_push16(BITCACHE, DEFAULT);//, "BITCACHE"
    VoC_bngtz16_r(III_decode_III_scalefactors, REG5)    //        if (header->flags & MAD_FLAG_LSF_EXT) {
    //REG0  : ->    ptr           REG1  : -> channel
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16_sd(REG7,  3,IN_PARALLEL);          // load  ch
    VoC_bez16_r(III_decode_III_scalefactors_100, REG7)
    VoC_lw16i(REG5, STRUCT_GRANULE1CH1_scalefac_ADDR);
III_decode_III_scalefactors_100:
//          part2_length = III_scalefactors_lsf(ptr, channel,
//                      ch == 0 ? 0 : &si->gr[1].ch[1],
//                      header->mode_extension);
    VoC_jal(CII_III_scalefactors_lsf);
    VoC_jump(III_decode_III_scalefactors_end);
III_decode_III_scalefactors:
    //REG0  : ->    ptr           REG1  : -> channel
//  part2_length = III_scalefactors(ptr, channel, &si->gr[0].ch[ch],
//                  gr == 0 ? 0 : si->scfsi[ch]);
    VoC_lw16i_set_inc(REG2, STRUCT_GRANULE0_BEGIN_ADDR, 1);
    VoC_lw16_sd(REG7,  3,  DEFAULT);          // load  ch
    VoC_bez16_r(III_decode_III_scalefactors_120, REG7)
    VoC_lw16i_set_inc(REG2, STRUCT_GRANULE0CH1_part2_3_length_ADDR, 1);
III_decode_III_scalefactors_120:
    VoC_lw16i_short(RL7_LO,0,DEFAULT);              // scfsi
    VoC_lw16i(REG6, STRUCT_SIDEINFO_scfsi_ADDR);
    VoC_add16_rr(REG6, REG6, REG7, DEFAULT);
    VoC_lw16_sd(REG7,  5,  IN_PARALLEL);          // load  gr
    VoC_bez16_r(III_decode_III_scalefactors_110, REG7)
    VoC_lw16_p(RL7_LO, REG6,DEFAULT);                      // scfsi
III_decode_III_scalefactors_110:
    VoC_jal(CII_III_scalefactors);
III_decode_III_scalefactors_end:

    VoC_sw16_d(BITCACHE, PTR_LEFT_ADDR);
    VoC_pop16(REG6, DEFAULT);  // , "BITCACHE"
    VoC_pop16(REG5,DEFAULT);//, " REG0/n  "
    VoC_sub16_rr(REG7, REG0, REG5,DEFAULT);
    VoC_bnltz16_r(III_decode_III_scalefactors_part2_length, REG7)
    VoC_sub16_dr(REG5,CONST_1024_ADDR, REG5);
    VoC_add16_rr(REG7, REG5,REG0,DEFAULT);
III_decode_III_scalefactors_part2_length:
    VoC_sub16_dr(REG6, PTR_LEFT_ADDR,REG6);
    //      VoC_bnltz16_r(III_decode_III_abs, REG6)
//              VoC_sub16_dr(REG6,CONST_0_ADDR, REG6);
//III_decode_III_abs:
    VoC_shr16_ri(REG7,-4,DEFAULT);
    VoC_sub16_rr(REG3, REG7, REG6,DEFAULT);          //REG3  :    part2_length
    VoC_lw16_sd(REG4, 0,IN_PARALLEL);   // , "channel/n " load REG4 to point to  channel
    //REG0 :   ptr

    VoC_lw16i(REG6, OUTPUT_PCM_BUFFER1_ADDR);//This is XR address.
    VoC_lw16_d(REG5, LOCAL_III_decode_sfbwidth_ADDR);
    VoC_lw16_sd(REG2, 1  ,DEFAULT);//,    " ch"
    VoC_bez16_r(III_decode_III_huffdecode, REG2)

    VoC_lw16i(REG6, OUTPUT_PCM_BUFFER1_ADDR);
    VoC_add16_rd(REG6,REG6,CONST_1152_ADDR);

    VoC_lw16_d(REG5, LOCAL_III_decode_sfbwidth_ADDR + 1);
III_decode_III_huffdecode:

    //    error = III_huffdecode(ptr, xr[ch], channel, sfbwidth[ch], part2_length);
    /**************************************************************
      Function:  CII_III_huffdecode
      Input:  reg0-> ptr
              RL6-> bit cache
              REG6-> xr[ch]
              REG4-> channel
              REG5-> sfbwidth[ch]
              REG3-> part2_length
      Ouput:  REG7-> error
      Version 1.0  Created by kenneth  2005/06/17
    **************************************************************/

//  spy_start_addr=0x1FED;
//  spy_end_addr=0x1FEE;
// spy_on_addr=OFF;
    VoC_jal(CII_III_huffdecode);
//  spy_on_addr=OFF;

    VoC_pop16(REG1,DEFAULT);//,"channel/n "
    VoC_movreg16(REG4,REG7,IN_PARALLEL);
    VoC_lw16i(REG7,54);
//  VoC_add16_rd(REG1, REG1, CONST_54_ADDR);       //   point to ch[1]
    VoC_add16_rr(REG1, REG1, REG7,DEFAULT);       //   point to ch[1]
    VoC_pop32(REG23,DEFAULT);//,"REG23/n"
    VoC_pop16(REG7,IN_PARALLEL);//, " ch"


    VoC_add16_rd(REG7, REG7,CONST_1_ADDR);
    VoC_bnez16_r(III_decode_L100,REG4);         //if (error) return error;
    VoC_bnlt16_rd(no_III_decode_nch_loop_begin, REG7,GLOBAL_NCH_ADDR)  //  }
    VoC_jump(III_decode_nch_loop_begin);
no_III_decode_nch_loop_begin:
// VoC_directive: PARSER_OFF
    /*
        {
       Word32 xr[2][576];
        int i,j,nch;
        VoC_lw16_d(REG7,GLOBAL_NCH_ADDR);
        VoC_NOP();
        VoC_lw16_d(REG6,DEC_OUTPUT_ADDR_ADDR);
        VoC_NOP();
        nch=REGS[7].reg;
        for(i=0;i<nch;i++)
           for(j=0;j<576;j++)
            {
                xr[i][j] = RAM_X[(REGS[6].reg-RAM_X_BEGIN_ADDR)/2+576*i+j];
            }

          for(i=0; i <nch; i++)
            {
            if (fwrite (&xr[i][0], sizeof (Word32), 576, fp_XR) != 576) {
                printf("err");
                //fprintf(stderr, "\nerror writing xr  file: %s\n", strerror(errno));

                exit(0);
                //return -1;
        }

            }
            fflush(fp_XR);
            }
    */
// VoC_directive: PARSER_ON
    /* joint stereo processing */

    /*    if (header->mode == MAD_MODE_JOINT_STEREO && header->mode_extension) {
          error = III_stereo(xr, granule, header, sfbwidth[0]);
          if (error)
        return error;
        }
        */

    VoC_lw16i_short(WRAP0,16,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_lw16i_short(REG7, MAD_MODE_JOINT_STEREO,DEFAULT);

    VoC_lw16_sd(REG1,2,IN_PARALLEL);//," granule"
    VoC_bne16_rd(III_decode_III_no_stereo, REG7, STRUCT_MAD_HEADER_MODE_ADDR)
    VoC_lw16_d(REG7, STRUCT_MAD_HEADER_MODE_EXTENSION_ADDR);
    VoC_bngtz16_r(III_decode_III_no_stereo, REG7)


    VoC_lw16i_set_inc(REG0, OUTPUT_PCM_BUFFER1_ADDR, 1);
    VoC_lw16d_set_inc(REG3, LOCAL_III_decode_sfbwidth_ADDR,1);

    VoC_jal(CII_III_stereo  );

// *OUTPUT: REG4
    VoC_bez16_r(III_decode_III_no_stereo, REG4)

III_decode_L100:
    VoC_pop16(REG3, DEFAULT);//," sfbwidth"
    VoC_pop16(REG7,DEFAULT);
    VoC_pop16(REG1,DEFAULT);
    VoC_jump(III_decode_end);
III_decode_III_no_stereo:
//stack16 : RA ,    " granule"  ,    " gr"   ,  " sfbwidth_table[sfreqi]"



    /* reordering, alias reduction, IMDCT, overlap-add, frequency inversion */
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16_sd(REG1, 2,IN_PARALLEL); //," granule"    struct channel const *channel = &granule->ch[ch]


    VoC_lw16i_set_inc(REG2, OUTPUT_PCM_BUFFER1_ADDR, 2);
    VoC_lw16i(REG4, STATIC_MAD_SBSAMPLE_ADDR);
    VoC_lw16i(REG5, STATIC_MAD_OVERLAP_ADDR);

III_decode_nch_loop2_begin:         //  for (ch = 0; ch < nch; ++ch) {
    VoC_lw16_sd(REG6, 1,DEFAULT);
    VoC_mult16_rd(REG6, REG6,CONST_1152_ADDR);
    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_add16_rr(REG4, REG4, REG6,DEFAULT);  //   Word16   (*sample)[32] = &frame->sbsample[ch][18 * gr];
    VoC_add16_rd(REG5, REG1,CONST_4_ADDR);  // point to channel->flags
    exit_on_warnings = OFF;
    VoC_push16(REG7,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);//,"sbsample&overlap"
    exit_on_warnings = ON;
    VoC_lw16i_short(REG7, 31, DEFAULT);          //   lines/18 - 1
    VoC_lw32_p(REG45, REG5,IN_PARALLEL);           // REG4 :  channel->flags  REG5 :  channel->block_type
//  if (channel->block_type == 2) {
//  III_reorder(xr[ch], channel, sfbwidth[ch]);
    /*
     * According to ISO/IEC 11172-3, "Alias reduction is not applied for
     * granules with block_type == 2 (short block)." However, other
     * sources suggest alias reduction should indeed be performed on the
     * lower two subbands of mixed blocks. Most other implementations do
     * this, so by default we will too.
     */
    /*  if (channel->flags & mixed_block_flag)
          III_aliasreduce(xr[ch], 36);
          }
          else
        III_aliasreduce(xr[ch], 576);
    */
    VoC_add16_rd(REG0, REG2,CONST_36_ADDR);   ////REG0   :  - >      &xr[0] + 18
    VoC_push16(REG2,DEFAULT);//,"XR"
    VoC_push32(REG45,IN_PARALLEL);//, "flags&block_type"
    VoC_bne16_rd(III_decode_nch_loop2_block_type, REG5,CONST_2_ADDR)
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG4,DEFAULT);

    VoC_lw16_sd(REG0,2,DEFAULT);           // *INPUT:      xr[]        ,    ->REG0
    VoC_movreg16(REG7, REG1,IN_PARALLEL);   // *            channel     ,    ->REG7
    VoC_lw16_sd(REG6,3, DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_lw16d_set_inc(REG1,LOCAL_III_decode_sfbwidth_ADDR, 1);//  sfbwidth[]  ,    ->REG1(incr 1)
    VoC_bez16_r(III_reorder_ch0, REG6)
    VoC_lw16d_set_inc(REG1,LOCAL_III_decode_sfbwidth_ADDR+1, 1);
III_reorder_ch0:
    VoC_jal(CII_III_reorder);
    VoC_pop16(REG4,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_pop16(REG0,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_and16_ri(REG4, mixed_block_flag);
    VoC_bngtz16_r(III_decode_nch_loop2_block_type_end, REG4)
    VoC_lw16i_short(REG7, 1, DEFAULT);  //    lines/18 - 1
III_decode_nch_loop2_block_type:
    VoC_jal(CII_III_aliasreduce);
III_decode_nch_loop2_block_type_end:


    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    // load 0x8000 in RL6
    VoC_lw16i(RL6_LO,0x8000);
    VoC_lw16i_short(RL6_HI,0,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);

    VoC_jal(vpp_AmjpAudioEqualizer);

    VoC_pop32(RL6,DEFAULT);

    VoC_lw16_sd(REG2,0,DEFAULT);
    VoC_lw16i_short(INC2,-2,IN_PARALLEL);
    VoC_lw16i_set_inc(REG3,576,-1);                        // i = 576;
    VoC_add16_rd(REG2, REG2,CONST_1152_ADDR);
    VoC_lw16i_short(REG7,18,DEFAULT);
    VoC_inc_p(REG2,DEFAULT);

    VoC_loop_i(1,3)
    VoC_loop_i(0,192)
    VoC_bngt16_rd(III_decode_nch_loop2_sblimit, REG3, CONST_36_ADDR)
    VoC_lw32inc_p(ACC0, REG2,DEFAULT);
    VoC_bnez32_r(III_decode_nch_loop2_sblimit, ACC0)// while (i > 36 && xr[ch][i - 1] == 0)
    VoC_inc_p(REG3,DEFAULT);                              //--i;
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1

III_decode_nch_loop2_sblimit:

    VoC_sub16_dr(REG6,CONST_576_ADDR, REG3);    // elisa wrong place 260613
    VoC_jal(CII_div_mode);
    VoC_sub16_dr(REG6,CONST_32_ADDR, REG5);//   sblimit = 32 - (576 - i) / 18;
    VoC_lw32_sd(REG45,0, DEFAULT);//, "flags&block_type"
    VoC_sw16_d(REG6, LOCAL_III_decode_sblimit_ADDR);
    /* subbands 0-1 */
    VoC_movreg16(REG6, REG5,DEFAULT);
    VoC_movreg16(REG7, REG5,IN_PARALLEL);

    VoC_and16_ri(REG4, mixed_block_flag);  //block_type = channel->block_type;
    VoC_bngtz16_r(III_decode_nch_loop2_block_subbands_100, REG4)  //if (channel->flags & mixed_block_flag)
    VoC_lw16i_short(REG7,0,DEFAULT);                 //    block_type = 0;
III_decode_nch_loop2_block_subbands_100:
    //REG6   :   channel->block_type    REG7   :   block_type
    VoC_lw16i_short(REG5,0,DEFAULT);    //sb
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
III_decode_nch_loop2_blocks_begin:                                //   for (sb = 0; sb < sblimit; ++sb, l += 18) {
    VoC_blt16_rd(III_decode_nch_loop2_blocks_100, REG5,CONST_2_ADDR)  // if( 2 < sb )
    VoC_movreg16(REG7, REG6,DEFAULT);                     //   block_type = channel->block_type;
III_decode_nch_loop2_blocks_100:

    VoC_lw16i_set_inc(REG1, LOCAL_III_decode_output_ADDR, 2);
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG5,DEFAULT);
    VoC_push32(REG67,IN_PARALLEL);//, "block_type"
    VoC_be16_rd(III_decode_nch_loop2_short_blocks, REG7,CONST_2_ADDR)       //   if (block_type != 2) {  /* long blocks */
    //  VoC_jal(CII_III_equalizer);
    VoC_jal(CII_III_imdct_l);                                                                  //  III_imdct_l(&xr[ch][l], output, block_type);
    VoC_jump( III_decode_nch_loop2_blocks_end);                //}
III_decode_nch_loop2_short_blocks:             //  else {          /* short blocks */
    //  VoC_jal(CII_III_equalizer);
    VoC_jal(CII_III_imdct_s);                //  III_imdct_s(&xr[ch][l], output);
III_decode_nch_loop2_blocks_end:            //  }

    VoC_lw16i_set_inc(REG2, LOCAL_III_decode_output_ADDR, 2);
    VoC_lw16i_set_inc(REG3, LOCAL_III_decode_output_ADDR+36, 2);     //REG3 : output[i+18]
    VoC_lw16_sd(REG7,0,DEFAULT);//, "sb"
    VoC_lw32_sd(REG01, 2 ,IN_PARALLEL);//,"sbsample&overlap"
    VoC_mult16_rd(REG6, REG7, CONST_36_ADDR );
    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG7,IN_PARALLEL);                     //REG0 : sample[0][sb]
    VoC_add16_rr(REG1, REG1, REG6,DEFAULT);
    VoC_lw16i_short(INC0, 64, IN_PARALLEL);

    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
// CII_III_overlap begin           //     III_overlap(output, (*frame->overlap)[ch][sb], sample, sb);

    VoC_loop_i_short(18,DEFAULT)                              //for (i = 0; i < 18; ++i)
    VoC_lw32inc_p(ACC0,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC1,REG3,DEFAULT);           //xuml modify
    VoC_add32_rp(ACC0,ACC0,REG1,DEFAULT);

    VoC_lw32inc_p(ACC0,REG2,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);                     //sample[i][sb] = add(output[i],overlap[i]);
    VoC_sw32inc_p(ACC1,REG1,IN_PARALLEL);                     //overlap[i]    = output[i + 18];
    exit_on_warnings = ON;
    VoC_endloop0;
// CII_III_overlap end



    VoC_pop16(REG5,DEFAULT);//,"sb"
    VoC_pop32(REG67,IN_PARALLEL);// , "block_type"
    VoC_add16_rd(REG5, REG5,CONST_1_ADDR);
    VoC_pop16(REG0,DEFAULT);//    ,"xr"
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_add16_rd(REG0, REG0,CONST_36_ADDR);
    VoC_bnlt16_rd(no_III_decode_nch_loop2_blocks_begin, REG5,LOCAL_III_decode_sblimit_ADDR)
    VoC_jump(III_decode_nch_loop2_blocks_begin);
no_III_decode_nch_loop2_blocks_begin:
    VoC_lw32_sd(REG67, 1,DEFAULT);      //,"sbsample&overlap"  remaining (zero) subbands
    VoC_movreg16(REG1, REG7,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_lw16_d(REG7, LOCAL_III_decode_sblimit_ADDR);
    VoC_mult16_rd(REG3, REG7, CONST_36_ADDR );
    VoC_sub16_dr(REG5, CONST_32_ADDR,REG7);
    VoC_add16_rr(REG1,REG1,REG3,DEFAULT);      //REG1 : overlap[0]
    VoC_lw32z(ACC0,IN_PARALLEL);              //ACC0 : 0

    VoC_add16_rr(REG6,REG6,REG7,DEFAULT);             //REG0 : sample[0][sb]
//  VoC_set_incr_i(REG0, 64,IN_PARALLEL);
    VoC_lw16i_short(INC0,64,IN_PARALLEL);
    VoC_add16_rr(REG7,REG6,REG7,DEFAULT);
    VoC_bez16_r(III_decode_nch_loop2_overlap_z, REG5)//      for (sb = sblimit; sb < 32; ++sb) {
    VoC_loop_r(1,REG5)
    //         VoC_jal(CII_III_overlap_z);                                //  III_overlap_z((*frame->overlap)[ch][sb], sample, sb);
// CII_III_overlap_z  begin
    VoC_movreg16(REG0, REG7,DEFAULT);
    VoC_lw32_p(ACC1,REG1,IN_PARALLEL);                    //REG2 : overlap[i]
    VoC_loop_i_short(18,DEFAULT)                      //for (i = 0; i < 18; ++i)
    VoC_startloop0
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);             //overlap[i]  = 0;
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);             //sample[i][sb] = overlap[i];
    VoC_lw32_p(ACC1,REG1,IN_PARALLEL);                //REG2 : overlap[i]
    VoC_endloop0
// CII_III_overlap_z  end
    VoC_add16_rd(REG7, REG7,CONST_2_ADDR);
    VoC_endloop1
III_decode_nch_loop2_overlap_z:

    VoC_lw16i(RL6_HI,(128*2));

    VoC_lw32_sd(REG67, 1,DEFAULT);//,"sbsample&overlap"
    VoC_movreg16(RL6_LO,RL6_HI,IN_PARALLEL);

    VoC_lw16i_short(RL7_HI,4,DEFAULT);
    VoC_lw16i_short(RL7_LO,4,IN_PARALLEL);
    VoC_lw16i(REG1,194);
//  VoC_add16_rd(REG7,REG6,CONST_194_ADDR);
    VoC_add16_rr(REG7,REG6,REG1,DEFAULT);
    VoC_lw16i(REG1,66);
//  VoC_add16_rd(REG6,REG6,CONST_66_ADDR);
    VoC_add16_rr(REG6,REG6,REG1,DEFAULT);

    VoC_movreg32(REG01, REG67,DEFAULT);      //REG0 :  &sample[1][sb]
    VoC_lw32z(REG23,IN_PARALLEL);    //REG1 :  &sample[3][sb]

    VoC_loop_i(1,16)               //   for (sb = 1; sb < 32; sb += 2) {
//  VoC_jal(CII_III_freqinver);         //     III_freqinver(sample, 1);

    VoC_sub32_rp(ACC0,REG23,REG0,DEFAULT);                     //ACC0  : -tmp1
    VoC_add32_rr(REG67, RL7, REG67,IN_PARALLEL);
    VoC_loop_i_short(4,DEFAULT);                              //for (i = 1; i < 13; i += 4)
    VoC_sub32_rp(ACC1,REG23,REG1,IN_PARALLEL);                 //ACC1  : -tmp2
    VoC_startloop0
    VoC_sw32_p(ACC0,REG0,DEFAULT);                    //sample[i + 0][sb] = -tmp1;
    VoC_add32_rr(REG01,REG01,RL6,IN_PARALLEL);      //REG1: &sample[i + 6][sb]
    exit_on_warnings=OFF;
    VoC_sw32_p(ACC1,REG1,DEFAULT);     //sample[i + 2][sb] = -tmp2;
    exit_on_warnings=ON;                               //REG0: &sample[i + 4][sb];
    VoC_sub32_rp(ACC0,REG23,REG0,DEFAULT);             //REG4  : -tmp1
    VoC_sub32_rp(ACC1,REG23,REG1,DEFAULT);             //REG5  : -tmp2
    VoC_endloop0
    VoC_movreg32(REG01, REG67,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32_p(ACC0,REG0,DEFAULT);                            //sample[17][sb] = -tmp1;
    exit_on_warnings = ON;
    VoC_endloop1

    VoC_pop32(REG45,DEFAULT);//, "flags&block_type" elisa tag end
    VoC_pop16(REG2,IN_PARALLEL);//,"XR"
    VoC_pop32(REG45,DEFAULT);//,"sbsample&overlap"
    VoC_pop16(REG7,IN_PARALLEL);//,"ch"

    VoC_add16_rd(REG7, REG7,CONST_1_ADDR);

    VoC_lw16i_set_inc(REG2, OUTPUT_PCM_BUFFER1_ADDR, 1);
    VoC_add16_rd(REG2,REG2,CONST_1152_ADDR);
    VoC_lw16_sd(REG1, 2,DEFAULT); // ," granule"   struct channel const *channel = &granule->ch[ch]
    VoC_lw16i(REG4, 54);
//  VoC_add16_rd(REG1, REG1, CONST_54_ADDR);
    VoC_add16_rr(REG1, REG1, REG4,DEFAULT);
    VoC_lw16i(REG4, STATIC_MAD_SBSAMPLE_ADDR+1152*2);
    VoC_lw16i(REG5, STATIC_MAD_OVERLAP_ADDR+576*2);

    VoC_bnlt16_rd(no_III_decode_nch_loop2_begin, REG7, GLOBAL_NCH_ADDR)
    VoC_jump(III_decode_nch_loop2_begin);
no_III_decode_nch_loop2_begin:

    VoC_pop16(REG3, DEFAULT);//," sfbwidth"

    VoC_pop16(REG7,DEFAULT);//," gr"
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_add16_rd(REG7, REG7,CONST_1_ADDR);
    VoC_pop16(REG1,DEFAULT);//  ," granule"
    VoC_lw16i_set_inc(REG1, STRUCT_GRANULE1_BEGIN_ADDR, 1);
    VoC_bnlt16_rd(no_III_decode_ngr_loop_begin, REG7, GLOBAL_NGR_ADDR)
    VoC_jump(III_decode_ngr_loop_begin);
no_III_decode_ngr_loop_begin:

    VoC_lw16i_short(REG4, MAD_ERROR_NONE, DEFAULT); //   return MAD_ERROR_NONE;



III_decode_end:
    /*
        {
            int i ,j,l,kk,ll=0;
           for (i = 0;i < 2; i++)
            {
               for(j=0;j<36;j++)
               {
                    for(l=0;l<32;l++)
                    {
                       kk = RAM_Y[(STATIC_MAD_SBSAMPLE_ADDR +2304*i - RAM_Y_BEGIN_ADDR)/2 + 32*j + l];
                       ll++;
                    }
                }
                if (fwrite ( &RAM_Y[(STATIC_MAD_SBSAMPLE_ADDR +2304*i - RAM_Y_BEGIN_ADDR)/2], sizeof (Word32), 32*36, file_SB) != 32*36) {
                fprintf(stderr, "\nerror writing sbsample  file: \n");
                return -1;
            }
            fflush(file_SB);
            }
        }
    */


    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

/*
 * NAME:    III_scalefactors()
 * DESCRIPTION: decode channel scalefactors of one granule from a bitstream
 */
//
//REG0  : ->    ptr           REG1  : -> channel
//REG2  : ->    gr0ch      RL7_LO  :   scfsi

void  CII_III_scalefactors(void)

{
    VoC_add16_rd(REG4, REG1, CONST_3_ADDR);    //  ->   channel->scalefac_compress
    VoC_lw16i_set_inc(REG3,TABLE_sflen_table_ADDR,2);
    VoC_lw16_p(REG4, REG4,DEFAULT);           //   channel->scalefac_compress
    VoC_shr16_ri(REG4,-1,DEFAULT);

    VoC_add16_rr(REG3, REG3, REG4,DEFAULT);
    VoC_add16_rd(REG4, REG1, CONST_4_ADDR);   // ->      channel->flags
    VoC_lw32_p(REG67, REG3,DEFAULT);    // REG6          :      slen1         REG7         :      slen2

    VoC_lw32_p(REG45, REG4,DEFAULT);    //  channel->flags         channel->block_type
    VoC_movreg32(ACC1, REG67,IN_PARALLEL); //  slen1 = sflen_table[channel->scalefac_compress].slen1;    //  slen2 = sflen_table[channel->scalefac_compress].slen2;
    VoC_add16_rd(REG1, REG1, CONST_14_ADDR);     //REG1   ->  channel->scalefac

    VoC_bne16_rd(III_scalefactors_block_type, REG5,CONST_2_ADDR)  //  if (channel->block_type == 2) {

    VoC_lw16i_short(REG3, 17,DEFAULT);          //   unsigned int nsfb;
    VoC_and16_ri(REG4, mixed_block_flag);
    VoC_bnez16_r(III_scalefactors_100, REG4)   //    sfbi = 0;
    VoC_lw16i_short(REG3, 18,DEFAULT);
III_scalefactors_100:      //    nsfb = (channel->flags & mixed_block_flag) ? 8 + 3 * 3 : 6 * 3;
    VoC_NOP();
    VoC_lbinc_p(REG0);
    VoC_loop_r_short(REG3,DEFAULT)           //    while (nsfb--)
    VoC_startloop0
    VoC_rbinc_r(REG4, REG6,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_sw16inc_p(REG4, REG1,DEFAULT);      //      channel->scalefac[sfbi++] = mad_bit_read(ptr, slen1);
    VoC_endloop0

    VoC_lbinc_p(REG0);             //    nsfb = 6 * 3;
    VoC_loop_i_short(18,DEFAULT)                      //   while (nsfb--)
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_startloop0
    VoC_rbinc_r(REG4, REG7,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_sw16inc_p(REG4, REG1,DEFAULT);       //     channel->scalefac[sfbi++] = mad_bit_read(ptr, slen2);
    VoC_endloop0
//    nsfb = 1 * 3;
//   while (nsfb--)
    //    channel->scalefac[sfbi++] = 0;
//  }
    VoC_loop_i_short(3,DEFAULT)
    VoC_startloop0
    VoC_sw16inc_p(REG3, REG1,DEFAULT);
    VoC_endloop0
    VoC_jump(III_scalefactors_block_type_end)
III_scalefactors_block_type:    // else {  /* channel->block_type != 2 */

    VoC_add16_rd(REG2, REG2, CONST_14_ADDR);    //  REG2  ->  gr0ch->scalefac

    VoC_movreg16(REG3, REG1,DEFAULT);

    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_lw16i_short(INC3,2,DEFAULT);
    VoC_loop_i_short(20,DEFAULT);
    VoC_lw32inc_p(ACC0, REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0, REG2,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32inc_p(ACC0, REG3,DEFAULT);
    exit_on_warnings = ON ;
    VoC_endloop0

    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_lw16i_short(REG4, 8,DEFAULT);
    VoC_lw16i_short(REG6, 6,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,4,-1);
    VoC_loop_i(1,4)
    VoC_movreg16(REG5, RL7_LO,DEFAULT);    // scfsi
    VoC_movreg16(REG7,ACC1_LO, IN_PARALLEL);

    VoC_and16_rr(REG5,REG4,DEFAULT);
    VoC_shr16_ri(REG4, 1,IN_PARALLEL);

    VoC_bgt16_rd(III_scalefactors_block_type_else_10, REG2,CONST_2_ADDR);
    VoC_movreg16(REG7,ACC1_HI, DEFAULT);
III_scalefactors_block_type_else_10:
    VoC_bgtz16_r(III_scalefactors_block_type_else, REG5)
    VoC_lbinc_p(REG0);
    VoC_loop_r_short(REG6,DEFAULT);
    VoC_movreg16(REG3, REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_rbinc_r(REG5, REG7,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_sw16inc_p(REG5, REG3,DEFAULT);
    VoC_endloop0
III_scalefactors_block_type_else:
    VoC_add16_rr(REG1, REG1,REG6,DEFAULT);
    VoC_lw16i_short(REG6, 5,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_endloop1
    VoC_sw16_p(REG2, REG1,DEFAULT);   //    channel->scalefac[21] = 0;
III_scalefactors_block_type_end:
    //    return mad_bit_length(&start, ptr);
    VoC_return;
}


/*
 * NAME:    III_scalefactors_lsf()
 * DESCRIPTION: decode channel scalefactors for LSF from a bitstream
 */
//REG0  : ->    ptr           REG1  : -> channel
//  REG5  : ->  gr1ch

void CII_III_scalefactors_lsf(void )
{
#if 0
    voc_short LOCAL_III_scalefactors_lsf_slen_ADDR, 4, y
#endif

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_add16_rd(REG3, REG1,CONST_3_ADDR);   // point to  channel->scalefac_compress;
    VoC_add16_rd(REG1, REG1,CONST_14_ADDR);   // point to   channel->scalefac;

    VoC_movreg16(REG2, REG1,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_loop_i_short(20,DEFAULT);
    VoC_startloop0
    VoC_sw32inc_p(ACC0, REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG2, LOCAL_III_scalefactors_lsf_slen_ADDR, 1);
// scalefac_compress = channel->scalefac_compress;
// REG4 :  scalefac_compress
    VoC_lw16inc_p(REG4, REG3,DEFAULT);    //Now the REG3 point to  channel->flags

    VoC_lw32_p(REG67, REG3,DEFAULT);     // channel->flags   &  channel->block_type
    VoC_movreg16(RL7_LO, REG3,IN_PARALLEL);   //save the point

//  index = (channel->block_type == 2) ?
//    ((channel->flags & mixed_block_flag) ? 2 : 1) : 0;

    VoC_lw16i_short(REG3,8,DEFAULT);
    VoC_and16_ri(REG6, mixed_block_flag);
    VoC_bgtz16_r(III_scalefactors_lsf_100, REG6)
    VoC_lw16i_short(REG3,4,DEFAULT);
III_scalefactors_lsf_100:
    VoC_be16_rd(III_scalefactors_lsf_200, REG7,CONST_2_ADDR);
    VoC_lw16i_short(REG3,0,DEFAULT);
III_scalefactors_lsf_200:

    // REG3 :  index
    VoC_lw16i_short(REG6, I_STEREO, DEFAULT);
    VoC_and16_rd(REG6, STRUCT_MAD_HEADER_MODE_EXTENSION_ADDR);
    VoC_bngtz16_r(III_scalefactors_lsf_10000, REG6)
    VoC_bgtz16_r(III_scalefactors_lsf_half, REG5)    // if (!((mode_extension & I_STEREO) && gr1ch)) {
III_scalefactors_lsf_10000:
    VoC_lw16i(REG6,400);
    VoC_bngt16_rr(III_scalefactors_lsf_1000,REG6, REG4)
//  VoC_bnlt16_rd(III_scalefactors_lsf_1000, REG4,CONST_400_ADDR)  //    if (scalefac_compress < 400) {
    VoC_movreg16(REG6, REG4,DEFAULT);
    VoC_shru16_ri(REG6, 4,DEFAULT);
    VoC_lw16i_short(REG7,5,IN_PARALLEL);  //    slen[0] = (scalefac_compress >> 4) / 5;
    VoC_jal(CII_div_mode);               //     slen[1] = (scalefac_compress >> 4) % 5;

    VoC_sw16inc_p(REG5, REG2,DEFAULT);
    VoC_movreg16(REG5, REG4,IN_PARALLEL);
    VoC_sw16inc_p(REG6, REG2,DEFAULT);
    VoC_movreg16(REG6, REG4,IN_PARALLEL);
//     slen[2] = (scalefac_compress % 16) >> 2;
//     slen[3] =  scalefac_compress %  4;
    VoC_and16_ri(REG5,0x0f);
    VoC_shr16_ri(REG5, 2,DEFAULT);
    VoC_and16_ri(REG6,0x03);
    VoC_lw16i(REG7, TABLE_nsfb_table_ADDR);
    VoC_sw16inc_p(REG5, REG2,DEFAULT);
    VoC_add16_rr(REG3, REG7, REG3,IN_PARALLEL);
    VoC_sw16inc_p(REG6, REG2,DEFAULT);
//     nsfb = nsfb_table[0][index];
    //    VoC_lw16_p(REG3, REG7,DEFAULT);
    VoC_jump(III_scalefactors_lsf_1500)
III_scalefactors_lsf_1000:

    VoC_lw16i(REG6,500);
    VoC_bngt16_rr(III_scalefactors_lsf_1100,REG6, REG4)
//      VoC_bnlt16_rd(III_scalefactors_lsf_1100, REG4,CONST_500_ADDR)  //   else if (scalefac_compress < 500) {

//      VoC_sub16_rd(REG4, REG4, CONST_400_ADDR);   //   scalefac_compress -= 400;
    VoC_lw16i(REG6,400);
    VoC_sub16_rr(REG4, REG4, REG6,DEFAULT);   //   scalefac_compress -= 400;


    VoC_movreg16(REG6, REG4,DEFAULT);
    VoC_shr16_ri(REG6, 2,DEFAULT);
    VoC_lw16i_short(REG7,5,IN_PARALLEL);

    VoC_jal(CII_div_mode);
    VoC_and16_ri(REG4,0x03);
    VoC_sw16inc_p(REG5, REG2,DEFAULT);  //     slen[0] = (scalefac_compress >> 2) / 5;
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_sw16inc_p(REG6, REG2,DEFAULT); //     slen[1] = (scalefac_compress >> 2) % 5;

    VoC_lw16i(REG7, TABLE_nsfb_table_ADDR+12);
    VoC_sw16inc_p(REG4, REG2,DEFAULT);          //     slen[2] =  scalefac_compress %  4;
    VoC_add16_rr(REG3, REG7, REG3,IN_PARALLEL);  //  nsfb = nsfb_table[1][index];
    VoC_sw16inc_p(REG5, REG2,DEFAULT);       //      slen[3] = 0;
    VoC_jump(III_scalefactors_lsf_1500)
III_scalefactors_lsf_1100:    //   else {

    VoC_lw16i(REG7,500);
    VoC_sub16_rr(REG6, REG4, REG7,DEFAULT);   //     scalefac_compress -= 500;

    VoC_lw16i_short(REG7,3,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);
    VoC_jal(CII_div_mode);

    VoC_sw16inc_p(REG5, REG2,DEFAULT);  //      slen[0] = scalefac_compress / 3;
    VoC_movreg16(REG5,RL7_LO, IN_PARALLEL);
    VoC_sw16inc_p(REG6, REG2,DEFAULT);  //      slen[1] = scalefac_compress % 3;
    VoC_lw16_p(REG6, REG5,DEFAULT);
    VoC_or16_ri(REG6,preflag);

    VoC_lw16i(REG7, TABLE_nsfb_table_ADDR+24);
    VoC_sw16inc_p(REG4, REG2,DEFAULT);       //      slen[2] = 0;
    VoC_add16_rr(REG3, REG7, REG3,IN_PARALLEL);  //      nsfb = nsfb_table[2][index];
    VoC_sw16inc_p(REG4, REG2,DEFAULT);       //     slen[3] = 0;
    VoC_sw16_p(REG6, REG5,DEFAULT);   //      channel->flags |= preflag;
III_scalefactors_lsf_1500:
    /*  n = 0;
      for (part = 0; part < 4; ++part) {
        for (i = 0; i < nsfb[part]; ++i)
    channel->scalefac[n++] = mad_bit_read(ptr, slen[part]);
      }
    */
    VoC_lw16i_set_inc(REG2, LOCAL_III_scalefactors_lsf_slen_ADDR, 1);
    VoC_lbinc_p(REG0);
    VoC_loop_i(1 ,4)
    VoC_lw16inc_p(REG7, REG3,DEFAULT);

    VoC_bez16_r(III_scalefactors_lsf_1510, REG7)
    VoC_loop_r_short(REG7,DEFAULT);
    VoC_lw16inc_p(REG6, REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_rbinc_r(REG5, REG6,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_sw16inc_p(REG5, REG1,DEFAULT);
    VoC_endloop0
III_scalefactors_lsf_1510:
    VoC_NOP();
    VoC_endloop1
//    while (n < 39)
//      channel->scalefac[n++] = 0;
//  }
    VoC_jump(III_scalefactors_lsf_end);
III_scalefactors_lsf_half:
//  else {  /* (mode_extension & I_STEREO) && gr1ch (i.e. ch == 1) */
//  scalefac_compress >>= 1;

    VoC_shr16_ri(REG4, 1,DEFAULT);
    VoC_movreg16(RL7_HI, REG5,IN_PARALLEL);           //gr1ch
    VoC_lw16i(REG6,180);
    VoC_bngt16_rr(III_scalefactors_lsf_1600,REG6, REG4)
//  VoC_bnlt16_rd(III_scalefactors_lsf_1600, REG4, CONST_180_ADDR)  //       if (scalefac_compress < 180) {
    VoC_movreg16(REG6, REG4,DEFAULT);
    VoC_lw16i_short(REG4,0 ,DEFAULT);
    VoC_lw16i_short(REG7,36,IN_PARALLEL);
    VoC_jal(CII_div_mode);

    VoC_sw16inc_p(REG5,REG2,DEFAULT);    //      slen[0] =  scalefac_compress / 36;
    VoC_lw16i_short(REG7,6, IN_PARALLEL);
    VoC_jal(CII_div_mode);

    VoC_lw16i(REG7, TABLE_nsfb_table_ADDR+36);
    VoC_sw16inc_p(REG5,REG2,DEFAULT);                 //     slen[1] = (scalefac_compress % 36) / 6;
    VoC_add16_rr(REG3, REG7, REG3,IN_PARALLEL);     //      nsfb = nsfb_table[3][index];
    VoC_sw16inc_p(REG6,REG2,DEFAULT);   //      slen[2] = (scalefac_compress % 36) % 6;

    VoC_sw16inc_p(REG4,REG2,DEFAULT); //      slen[3] = 0;

    VoC_jump(III_scalefactors_lsf_2000);
III_scalefactors_lsf_1600:
    VoC_lw16i(REG6,244);
    VoC_bngt16_rr(III_scalefactors_lsf_1700, REG6, REG4) //  else if (scalefac_compress < 244) {
//  VoC_bnlt16_rd(III_scalefactors_lsf_1700, REG4, CONST_244_ADDR) //  else if (scalefac_compress < 244) {

//      VoC_sub16_rd(REG4, REG4, CONST_180_ADDR);  //      scalefac_compress -= 180;
    VoC_lw16i(REG6,180);
    VoC_sub16_rr(REG4, REG4, REG6,DEFAULT);  //      scalefac_compress -= 180;


    /*      slen[0] = (scalefac_compress % 64) >> 4;
         slen[1] = (scalefac_compress % 16) >> 2;
         slen[2] =  scalefac_compress %  4;
         slen[3] = 0;
    */
    VoC_movreg16(REG5, REG4,DEFAULT);
    VoC_movreg16(REG6, REG4,IN_PARALLEL);
    VoC_and16_ri(REG5,0xff);
    VoC_and16_ri(REG6,0x0f);
    VoC_and16_ri(REG4,0x03);
    VoC_shru16_ri(REG5,4,DEFAULT);
    VoC_shru16_ri(REG6,2,IN_PARALLEL);
    VoC_lw16i(REG7, TABLE_nsfb_table_ADDR+48);
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_sw16inc_p(REG6,REG2,DEFAULT);
    VoC_add16_rr(REG3, REG7, REG3,IN_PARALLEL);
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
//    nsfb = nsfb_table[4][index];
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    VoC_jump(III_scalefactors_lsf_2000)
III_scalefactors_lsf_1700:
    /*   else {
         scalefac_compress -= 244;
         slen[0] = scalefac_compress / 3;
         slen[1] = scalefac_compress % 3;
         slen[2] = 0;
         slen[3] = 0;
    */
//  VoC_sub16_rd(REG6, REG4, CONST_244_ADDR);
    VoC_lw16i(REG6,244);
    VoC_sub16_rr(REG6, REG4, REG6,DEFAULT);

    VoC_lw16i_short(REG7, 3,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);
    VoC_jal(CII_div_mode);
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    VoC_lw16i(REG7, TABLE_nsfb_table_ADDR+60);
    VoC_sw16inc_p(REG6,REG2,DEFAULT);
    VoC_add16_rr(REG3, REG7, REG3,IN_PARALLEL);    //     nsfb = nsfb_table[5][index];
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
III_scalefactors_lsf_2000:
    VoC_movreg16(REG2, RL7_HI,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_loop_i_short(11,DEFAULT)
    VoC_startloop0
    VoC_sw32inc_p(ACC0, REG2,DEFAULT);
    VoC_endloop0
    VoC_lw16i_set_inc(REG2, LOCAL_III_scalefactors_lsf_slen_ADDR, 1);
    /*    n = 0;
        for (part = 0; part < 4; ++part) {
          unsigned int max, is_pos;

          max = (1 << slen[part]) - 1;

          for (i = 0; i < nsfb[part]; ++i) {
        is_pos = mad_bit_read(ptr, slen[part]);

        channel->scalefac[n] = is_pos;
        gr1ch->scalefac[n++] = (is_pos == max);
          }
        }
    */
    VoC_lbinc_p(REG0);
    VoC_lw16i_short(REG4, 1,DEFAULT);
    VoC_loop_i(1,4)
    VoC_lw16i_short(REG6, 1,DEFAULT);
    VoC_lw16inc_p(REG7, REG2, IN_PARALLEL);     //   slen[part]
    VoC_sub16_dr(REG5,CONST_0_ADDR, REG7);
    VoC_shr16_rr(REG6, REG5,DEFAULT);
    VoC_lw16inc_p(REG5, REG3,IN_PARALLEL);
    VoC_sub16_rd(REG6, REG6,CONST_1_ADDR);   //     max = (1 << slen[part]) - 1;

    VoC_movreg16(RL7_LO, REG3,DEFAULT);
    VoC_movreg16(REG3, RL7_HI,IN_PARALLEL);
    VoC_bez16_r(III_scalefactors_lsf_2046, REG5)
    VoC_loop_r_short(REG5,DEFAULT)
    VoC_startloop0
    VoC_rbinc_r(REG5, REG7,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_sw16inc_p(REG5, REG1,DEFAULT);
    VoC_bne16_rr(III_scalefactors_lsf_2050, REG5, REG6)
    VoC_sw16_p(REG4, REG3,DEFAULT);
III_scalefactors_lsf_2050:
    VoC_inc_p(REG3,DEFAULT);
    VoC_endloop0
III_scalefactors_lsf_2046:

    VoC_movreg16(RL7_HI, REG3,DEFAULT);
    VoC_movreg16(REG3, RL7_LO,IN_PARALLEL);
    VoC_endloop1
//    while (n < 39) {
//      channel->scalefac[n] = 0;
//      gr1ch->scalefac[n++] = 0;  /* apparently not illegal */
//   }
// }
III_scalefactors_lsf_end:
// return mad_bit_length(&start, ptr);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return
}


/********************************************************
  Function: CII_huff_sub1
        Input:  reg2->value
                reg7->2
  Version 1.0 created by kenneth  2005/06/23
********************************************************/
void CII_huff_sub1(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_bnez16_r(huff_sub_L1,REG2);                 //switch (value) {
    //case 0:
    //  xrptr[0] = 0;
    VoC_jump(huff_sub_Lend);                        //  break;
huff_sub_L1:
    VoC_be16_rd(no_huff_sub_L2,REG2,CONST_15_ADDR);   //case 15:
    VoC_jump(huff_sub_L2);
no_huff_sub_L2:
    VoC_add16_rd(REG7,REG7,LOCAL_HUFF_LINBITS_ADDR);//  if (cachesz < linbits + 2) {
    VoC_lw16_sd(REG4,6,DEFAULT);//,"cachesz"
    VoC_lw32_sd(RL7,1,IN_PARALLEL);//,"bitcache"
    VoC_bngt16_rr(huff_sub_L3,REG7,REG4);
    VoC_lbinc_p(REG0);                              //bitcache   = (bitcache << 16) | mad_bit_read(&peek, 16);
    VoC_rbinc_i(REG6,16,DEFAULT);
    VoC_lw16i_short(REG1,16,IN_PARALLEL);
    VoC_lw16_sd(REG3,11,DEFAULT);//,"bits_l"
    VoC_shru32_ri(RL7,-16,IN_PARALLEL);
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG1,IN_PARALLEL);       //bits_left -= 16;
    VoC_or32_rr(RL7,REG67,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG1,IN_PARALLEL);       //cachesz   += 16;
    VoC_sw16_sd(REG3,11,DEFAULT);//,"bits_l"
    VoC_sw32_sd(RL7,1,DEFAULT);     // ,"bitcache" }
huff_sub_L3:
    VoC_lw32_d(REG67,CONST_1_ADDR);
    VoC_lw16_d(REG3,LOCAL_HUFF_LINBITS_ADDR);       //  value += MASK(bitcache, cachesz, linbits);
    VoC_sub16_rr(REG1,REG5,REG3,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG3,IN_PARALLEL);       //  MUSK  (((cache) >> ((sz) - (bits))) & ((1 << (bits)) - 1))
    VoC_shr32_rr(REG67,REG1,DEFAULT);
    VoC_shru32_rr(RL7,REG4,IN_PARALLEL);
    VoC_sub32_rd(REG67,REG67,CONST_1_ADDR);
    VoC_and32_rr(REG67,RL7,DEFAULT);
    VoC_add16_rr(REG4,REG2,REG6,DEFAULT);
    VoC_sw16_sd(REG4,6,IN_PARALLEL);     // ,"cachesz" cachesz -= linbits;
    VoC_lw16_sd(REG6,3,DEFAULT);       //    ,"exp"
    VoC_jal(CII_III_requantize);                    //  requantized = III_requantize(value, exp);
    VoC_jump(huff_sub_L4);                          //  goto x_final;
huff_sub_L2:                                            //default:
    VoC_movreg16(REG4,REG2,DEFAULT);
    VoC_lw16i(REG1,LOCAL_HUFF_REQCACHE_ADDR);
    VoC_lw16_d(REG7,LOCAL_HUFF_REQHITS_ADDR);
    VoC_add16_rr(REG1,REG1,REG2,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG2,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_sub16_rr(REG2,REG5,REG2,DEFAULT);
    VoC_movreg16(REG3,REG7,IN_PARALLEL);
    VoC_shru16_rr(REG6,REG2,DEFAULT);
    VoC_and16_rr(REG3,REG6,DEFAULT);
    VoC_bnez16_r(huff_sub_L5,REG3);                 //  if (reqhits & (1 << value))  requantized = reqcache[value];
    VoC_or16_rr(REG7,REG6,DEFAULT);                 //  else {
    VoC_lw16_sd(REG6,3,DEFAULT);//,"exp"
    VoC_sw16_d(REG7,LOCAL_HUFF_REQHITS_ADDR);       //    reqhits |= (1 << value);
    VoC_push16(REG1,DEFAULT);                       //    requantized = reqcache[value] = III_requantize(value, exp);
    VoC_jal(CII_III_requantize);                    //  }
    VoC_pop16(REG1,DEFAULT);
    VoC_NOP();
    VoC_sw32_p(ACC0,REG1,DEFAULT);
huff_sub_L5:
    VoC_lw32_p(ACC0,REG1,DEFAULT);
huff_sub_L4:                                            //x_final:
    VoC_lw16i_short(REG4,1,DEFAULT);                //  xrptr[0] = MASK1BIT(bitcache, cachesz--) ?  -requantized : requantized;
    VoC_lw16_sd(REG2,6,IN_PARALLEL);     //,"cachesz"}     ((cache) & (1 << ((sz) - 1)))
    VoC_lw32_sd(RL7,1,DEFAULT);//,"bitcache"
    VoC_sub16_rr(REG6,REG2,REG4,IN_PARALLEL);
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_sub16_rr(REG3,REG4,REG2,IN_PARALLEL);
    VoC_sw16_sd(REG6,6,DEFAULT);//,"cachesz"
    VoC_shru32_rr(REG45,REG3,IN_PARALLEL);

    VoC_and32_rr(RL7,REG45,DEFAULT);

    VoC_bez32_r(huff_sub_Lend,RL7);
    VoC_sub32_dr(ACC0,CONST_0_ADDR,ACC0);
huff_sub_Lend:
    VoC_lw16_sd(REG1,1,DEFAULT);//,"xrptr"
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_pop16(RA,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);      //   xrptr[0] =
    VoC_lw32_sd(REG23,0,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);
    VoC_sw16_sd(REG1,0,DEFAULT);  // ,"xrptr"
    VoC_movreg16(REG2,REG3,IN_PARALLEL);
    VoC_return;
}

/********************************************************
  Function: CII_huff_sub2
  Input:  reg2->value

  Version 1.0 created by kenneth  2005/06/23
********************************************************/
void CII_huff_sub2(void)
{
    //value = pair->value.x;

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_bez16_r(huff_sub2_Lend,REG2);               //if (value == 0) xrptr[0] = 0;
    //else {
    VoC_movreg16(REG4,REG2,DEFAULT);
    VoC_lw16i(REG1,LOCAL_HUFF_REQCACHE_ADDR);
    VoC_lw16_d(REG7,LOCAL_HUFF_REQHITS_ADDR);
    VoC_add16_rr(REG1,REG1,REG2,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG2,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_sub16_rr(REG2,REG5,REG2,DEFAULT);
    VoC_movreg16(REG3,REG7,IN_PARALLEL);
    VoC_shru16_rr(REG6,REG2,DEFAULT);
    VoC_and16_rr(REG3,REG6,DEFAULT);
    VoC_bnez16_r(huff_sub2_L1,REG3);                        //  if (reqhits & (1 << value))  requantized = reqcache[value];
    VoC_or16_rr(REG7,REG6,DEFAULT);                 //  else {
    VoC_lw16_sd(REG6,3,DEFAULT);//,"exp"
    VoC_sw16_d(REG7,LOCAL_HUFF_REQHITS_ADDR);       //    reqhits |= (1 << value);
    VoC_push16(REG1,DEFAULT);                       //    requantized = reqcache[value] = III_requantize(value, exp);
    VoC_jal(CII_III_requantize);                    //  }
    VoC_pop16(REG1,DEFAULT);
    VoC_NOP();
    VoC_sw32_p(ACC0,REG1,DEFAULT);
huff_sub2_L1:
    VoC_lw32_p(ACC0,REG1,DEFAULT);

    VoC_lw16i_short(REG4,1,DEFAULT);                //  xrptr[0] = MASK1BIT(bitcache, cachesz--) ?  -requantized : requantized;
    VoC_lw16_sd(REG2,6,IN_PARALLEL);     //},"cachesz"     ((cache) & (1 << ((sz) - 1)))
    VoC_lw32_sd(RL7,1,DEFAULT);//,"bitcache
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_sub16_rr(REG3,REG4,REG2,DEFAULT);
    VoC_sub16_rr(REG2,REG2,REG4,IN_PARALLEL);
    VoC_shr32_rr(REG45,REG3,DEFAULT);
    VoC_and32_rr(RL7,REG45,DEFAULT);
    VoC_sw16_sd(REG2,6,IN_PARALLEL);      //  ,"cachesz"
    //  VoC_movreg16(REG5,REG7,DEFAULT);
    VoC_bez32_r(huff_sub2_Lend,RL7);
    VoC_sub32_dr(ACC0,CONST_0_ADDR,ACC0);
huff_sub2_Lend:
    VoC_lw16_sd(REG1,1,DEFAULT);//,"xrptr"
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_pop16(RA,DEFAULT);
//  VoC_sw16inc_p(REG5,REG1,DEFAULT);               //   xrptr[0] =
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_lw32_sd(REG23,0,DEFAULT);
    VoC_sw16_sd(REG1,0,DEFAULT);   //,"xrptr"
    VoC_movreg16(REG2,REG3,IN_PARALLEL);
    VoC_return;
}

/********************************************************
  Function: CII_huff_sub2
  Input:  stack32[0]: x/y
          stack32[1]: v/w

  Version 1.0 created by kenneth  2005/06/23
********************************************************/
void CII_huff_sub3(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16_sd(REG1,1,DEFAULT);     // ,"xrptr"
    VoC_bne16_rd(huff_sub3_L1,REG1,LOCAL_HUFF_SFBOUND_ADDR);//if (xrptr == sfbound) {
    VoC_lw16_sd(REG2,10,DEFAULT);           //,"sfbw"  sfbound += *sfbwidth++;
    VoC_lw16_d(REG6,LOCAL_HUFF_SFBOUND_ADDR);
    VoC_add16_rp(REG6,REG6,REG2,DEFAULT);
    VoC_add16inc_rp(REG6,REG6,REG2,DEFAULT);
    VoC_lw16_sd(REG3,4,DEFAULT);//,"expptr"
    VoC_sw16_d(REG6,LOCAL_HUFF_SFBOUND_ADDR);
    VoC_lw16inc_p(REG5,REG3,DEFAULT);
    VoC_sw16_sd(REG2,10,DEFAULT);//,"sfbw"
    VoC_lw16_sd(REG6,3,DEFAULT);//,"exp"
    VoC_sw16_sd(REG3,4,DEFAULT);//,"expptr"
    VoC_be16_rr(huff_sub3_L1,REG5,REG6);            //  if (exp != *expptr) {
    VoC_sw16_sd(REG5,3,DEFAULT);             // ,"exp"   exp = *expptr;
    VoC_push16(REG4,DEFAULT);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_movreg16(REG6,REG5,IN_PARALLEL);
    VoC_jal(CII_III_requantize);                    //    requantized = III_requantize(1, exp);
    VoC_pop16(REG4,DEFAULT);                                                //  }
    //  ++expptr;
huff_sub3_L1:                                           //}
    /* v (0..1) */
    VoC_lw32_sd(REG23,1,DEFAULT);
    VoC_lw32z(REG67,IN_PARALLEL);
    VoC_lw16_sd(REG1,1,DEFAULT);//,"xrptr"
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_bez16_r(huff_sub3_L2,REG3);                 //xrptr[0] = quad->value.v ?
    VoC_lw32_sd(RL7,2,DEFAULT);         //,"bitcache"  (MASK1BIT(bitcache, cachesz--) ? -requantized : requantized) : 0;
    VoC_lw32_d(ACC1,CONST_1_ADDR);                  //((cache) & (1 << ((sz) - 1)))
    VoC_sub16_rd(REG4,REG4,CONST_1_ADDR);
    VoC_sub16_rr(REG3,REG5,REG4,DEFAULT);
    VoC_shr32_rr(ACC1,REG3,DEFAULT);
    VoC_and32_rr(ACC1,RL7,DEFAULT);
    VoC_movreg32(REG67,ACC0,IN_PARALLEL);
    VoC_bez32_r(huff_sub3_L2,ACC1);
    VoC_sub32_dr(REG67,CONST_0_ADDR,REG67);
huff_sub3_L2:
    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_lw32_d(ACC1,CONST_1_ADDR);
    VoC_sw32inc_p(REG67,REG1,DEFAULT);

    /* w (0..1) */
    VoC_lw32z(REG67,IN_PARALLEL);
    VoC_bez16_r(huff_sub3_L3,REG2);                 //xrptr[1] = quad->value.w ?
    VoC_sub16_rd(REG4,REG4,CONST_1_ADDR);           //  (MASK1BIT(bitcache, cachesz--) ? -requantized : requantized) : 0;
    VoC_sub16_rr(REG3,REG5,REG4,DEFAULT);
    VoC_shr32_rr(ACC1,REG3,DEFAULT);
    VoC_lw32_sd(RL7,2,IN_PARALLEL); //,"bitcache"
    VoC_and32_rr(ACC1,RL7,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_bez32_r(huff_sub3_L3,ACC1);
    VoC_sub32_dr(REG67,CONST_0_ADDR,REG67);
huff_sub3_L3:
    VoC_lw32_sd(REG23,0,DEFAULT);
    VoC_sw32inc_p(REG67,REG1,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_sw32_sd(REG23,1,IN_PARALLEL);
    VoC_sw16_sd(REG1,0,DEFAULT);           //xrptr += 2;
    VoC_return;
}



/**************************************************************
  Function:  CII_III_huffdecode
  Input:  reg0-> ptr
          RL6-> bitcache
          REG6-> xr[ch]
          REG4-> channel
          REG5-> sfbwidth[ch]
          REG3-> part2_length
  Ouput:  REG7-> error
  Version 1.0  Created by kenneth  2005/06/17
**************************************************************/
void CII_III_huffdecode(void)
{
#if 0
    voc_short LOCAL_HUFF_SFBOUND_ADDR, 2, y
    voc_word  LOCAL_HUFF_REQCACHE_ADDR, 16,y
    voc_short LOCAL_HUFF_TABLE_ADDR, y
    voc_short LOCAL_HUFF_LINBITS_ADDR, y
    voc_short LOCAL_HUFF_STARTBITS_ADDR, y
    voc_short LOCAL_HUFF_REQHITS_ADDR, y
    voc_short LOCAL_HUFF_REGION_ADDR, y
    voc_short LOCAL_HUFF_XRPTR_ADDR, y
#endif

//  spy_on_addr=ON;
    VoC_push16(RA,DEFAULT);
    VoC_sub16_pr(REG1,REG4,REG3,DEFAULT);   //bits_left = (signed) channel->part2_3_length - (signed) part2_length;
    //VoC_lw16i_short(REG7,MAD_ERROR_BADPART3LEN,IN_PARALLEL);
    VoC_lw16i(REG7,MAD_ERROR_BADPART3LEN);
    VoC_bnltz16_r(huffdec_L1,REG1);         //if (bits_left < 0)
    VoC_jump(huffdec_Lend);                 //  return MAD_ERROR_BADPART3LEN;
huffdec_L1:
    VoC_push16(REG1,DEFAULT); //,"bits_l"    // push16 stack16[1]
    VoC_push16(REG5,DEFAULT); //,"sfbw"      // push16 stack16[2]
    VoC_push16(REG4,DEFAULT); //,"chan"      // push16 stack16[3]
    VoC_lw16i_short(WRAP0,16,IN_PARALLEL);
    VoC_push16(REG6,DEFAULT);  //,"xr"       // push16 stack16[4]
    VoC_push16(REG0,DEFAULT);  //,"ptr"      // push16 stack16[5]
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_jal(CII_III_exponents);             //III_exponents(channel, sfbwidth, exponents);
    VoC_bez16_d(huffdec_L1111, GLOBAL_WRAP_INDEX_ADDR)
    VoC_lw16i_short(WRAP0,10,DEFAULT);
huffdec_L1111:
    VoC_lw16_sd(REG0,0,DEFAULT);   //,"ptr"
    VoC_push32(RL6,IN_PARALLEL);            // push32 stack32[0]
    VoC_push16(BITCACHE,DEFAULT);           // save BFS6  stack16[6]
    VoC_lw16i_short(REG4,15,IN_PARALLEL);   //mad_bit_skip(ptr, bits_left);

    VoC_lw16_sd(REG5,5,DEFAULT);//,"bits_l"
    VoC_lw16_sd(REG6, 0,DEFAULT);           //BFS6
    VoC_bngt16_rr(huffdec_L2, REG5,REG6 )
    VoC_sub16_rr(REG3, REG5, REG6,DEFAULT);
    VoC_lw16i_short(BITCACHE, 0,IN_PARALLEL);

    VoC_movreg16(REG5,REG3,DEFAULT);
    VoC_shr16_ri(REG3,4,IN_PARALLEL);
    VoC_add16_rr(REG0, REG0, REG3,DEFAULT);
    VoC_bez16_d(huffdec_L22, GLOBAL_WRAP_INDEX_ADDR)
    VoC_blt16_rd(huffdec_L22, REG0,CONST_1024_ADDR)
    VoC_sub16_rd(REG0, REG0, CONST_1024_ADDR);
huffdec_L22:
    VoC_and16_rr(REG5,REG4,DEFAULT);
    VoC_lbinc_p(REG0);
huffdec_L2:

    VoC_bngt16_rd(huffdec_L21111, REG5,CONST_16_ADDR)
    VoC_rbinc_i(REG6,16,DEFAULT);
    VoC_sub16_rd(REG5, REG5, CONST_16_ADDR);
huffdec_L21111:
    VoC_rbinc_r(REG6,REG5,DEFAULT);

    VoC_sw16_d(REG0,PTR_BYTE_ADDR_ADDR);
    VoC_sw32_d(RL6,PTR_CACHE_ADDR);
    VoC_sw16_d(BITCACHE,PTR_LEFT_ADDR);

    /* align bit reads to byte boundaries */
    VoC_lw16_sd(REG0,1,DEFAULT);     //peek = *ptr;
    VoC_pop32(RL6,IN_PARALLEL);             // pop32 stack32[0]
    VoC_pop16(REG3,DEFAULT);            //cachesz  = mad_bit_bitsleft(&peek); stack16[6]
    VoC_lw16i_short(REG4,16,IN_PARALLEL);
    VoC_movreg16(BITCACHE,REG3,DEFAULT);
    VoC_lw16i_short(REG5,31,IN_PARALLEL);
    VoC_bngt16_rd(huffdec_L2a,REG3,CONST_16_ADDR);
    VoC_sub16_rr(REG3,REG3,REG4,DEFAULT);
huffdec_L2a:
    VoC_bngt16_rd(huffdec_L2b,REG3,CONST_8_ADDR);
    VoC_sub16_rd(REG3,REG3,CONST_8_ADDR);
huffdec_L2b:
    VoC_sub16_rr(REG5,REG5,REG3,DEFAULT);   //cachesz += ((32 - 1 - 24) + (24 - cachesz)) & ~7;
    VoC_and16_ri(REG5,0xFFF8);
    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_lw16_sd(REG1,1,DEFAULT);              //xrptr = &xr[0];
    VoC_sub16_rr(REG5,REG3,REG4,IN_PARALLEL);

    VoC_lbinc_p(REG0);
    VoC_rbinc_r(REG7,REG5,DEFAULT);         //bitcache   = mad_bit_read(&peek, cachesz);
    VoC_lw16_sd(REG2,3,IN_PARALLEL); //,"sfbw"
    VoC_lbinc_p(REG0);                      // bitcache in REG67
    VoC_lw16_sd(REG5,4,DEFAULT);//,"bits_l"
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_rbinc_i(REG6,16,DEFAULT);           //bits_left -= cachesz;
    VoC_push16(REG3,IN_PARALLEL);    //,"cachesz"  push16 stack16[6] cachesz
    VoC_sub16_rr(REG5,REG5,REG3,DEFAULT);
    VoC_sw16_d(ACC0_HI,LOCAL_HUFF_REGION_ADDR);     // region = 0
    VoC_sw16_sd(REG5,5,DEFAULT);//,"bits_l"
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    /* big_values */
    //{
    //unsigned int region, rcount;
    //struct hufftable const *entry;
    //union huffpair const *table;
    //unsigned int linbits, startbits, big_values, reqhits;
    //Word16  reqcache[16];

    VoC_add16_rp(REG4,REG1,REG2,DEFAULT);
    VoC_add16inc_rp(REG4,REG4,REG2,DEFAULT);     //sfbound = xrptr + *sfbwidth++;
    VoC_lw16_sd(REG3,3,IN_PARALLEL);   //,"chan"     //  xrptr in reg1
    VoC_push32(REG67,DEFAULT);          //,"bitcache" push32 stack32[0]
    VoC_sw16_sd(REG2,4,IN_PARALLEL);//,"sfbw"
    VoC_add16_rd(REG7,REG3,CONST_6_ADDR);
    VoC_add16_rd(REG6,REG3,CONST_12_ADDR);

    VoC_lw16i_short(REG2,2,DEFAULT);
    VoC_mult16_rp(REG7,REG2,REG7,DEFAULT);
//  VoC_mult16_pd(REG7,REG7,CONST_2_ADDR);

    VoC_add16_rd(REG2,REG3,CONST_1_ADDR);           //big_values = channel->big_values;
    VoC_add16_pd(REG6,REG6,CONST_1_ADDR);           //rcount  = channel->region0_count + 1;
    VoC_lw16_p(REG2,REG2,DEFAULT);

    VoC_lw16i(REG5,TABLE_mad_huff_pair_table_ADDR);
    VoC_add16_rr(REG3,REG5,REG7,DEFAULT);           //entry     = &mad_huff_pair_table[channel->table_select[region = 0]];
    VoC_sw16_d(REG4,LOCAL_HUFF_SFBOUND_ADDR);
    VoC_lw16inc_p(REG4,REG3,DEFAULT);               //table     = entry->table;

    VoC_bnez16_r(huffdec_L3,REG4);                  //if (table == 0)
    VoC_lw16i(REG7,MAD_ERROR_BADHUFFTABLE);         //  return MAD_ERROR_BADHUFFTABLE;

    VoC_jump(huffdec_Lpopend2);
huffdec_L3:
    VoC_lw16inc_p(REG5,REG3,DEFAULT);               //linbits   = entry->linbits;
    VoC_movreg16(REG7,REG5,DEFAULT);                //startbits = entry->startbits;
    VoC_and16_ri(REG5,0x00ff);
    VoC_shr16_ri(REG7,8,DEFAULT);

    VoC_sw32_d(REG45,LOCAL_HUFF_TABLE_ADDR);
    VoC_lw16i(REG3,LOCAL_EXPONENTS_ADDR);           //expptr  = &exponents[0];
    VoC_sw16_d(REG7,LOCAL_HUFF_STARTBITS_ADDR);

    VoC_lw16inc_p(REG7,REG3,DEFAULT);               //exp     = *expptr++;
    VoC_lw16i_short(REG4,0,IN_PARALLEL);
    VoC_push16(REG6,DEFAULT);                       // push16 stack16[7]  rcount
    VoC_push16(REG3,DEFAULT);             // ,"expptr" push16 stack16[8]  expptr
    VoC_push16(REG7,DEFAULT);                //,"exp" push16 stack16[9]  exp
    VoC_sw16_d(REG4,LOCAL_HUFF_REQHITS_ADDR);       //reqhits = 0;
//  spy_on_addr=OFF;
    VoC_push16(REG2,DEFAULT);         //,"big_values" push16 stack16[10]  big_values
//  spy_on_addr=ON;
    VoC_push16(REG1,DEFAULT);              //,"xrptr" push16 stack16[11]

huffdec_L4:
    VoC_lw16_sd(REG2,1,DEFAULT);//,"big_values"
    VoC_sub16_rd(REG2,REG2,CONST_1_ADDR);                   //while (big_values-- && cachesz + bits_left > 0) {
    VoC_bltz16_r(huffdec_L5,REG2);
    VoC_lw16_sd(REG6,5,DEFAULT);//,"cachesz"
    VoC_lw16_sd(REG7,10,DEFAULT);//,"bits_l"
    VoC_add16_rr(REG5,REG6,REG7,DEFAULT);
    VoC_bgtz16_r(huffdec_L6,REG5);
huffdec_L5:
    VoC_jump(huffdec_LbgEnd);
    //union huffpair const *pair;
    //unsigned int clumpsz, value;
    //register Word16  requantized;
huffdec_L6:
//  spy_on_addr=OFF;
    VoC_sw16_sd(REG2,1,DEFAULT);      //,"big_values"
//  spy_on_addr=ON;
    VoC_lw16_sd(REG2,9,DEFAULT);//,"sfbw"
    VoC_be16_rd(no_huffdec_L7,REG1,LOCAL_HUFF_SFBOUND_ADDR);  //if (xrptr == sfbound) {
    VoC_jump(huffdec_L7);
no_huffdec_L7:
    //  VoC_add16_pd(REG5,REG2,LOCAL_HUFF_SFBOUND_ADDR);     //  sfbound += *sfbwidth++;
    VoC_lw16_d(REG5,LOCAL_HUFF_SFBOUND_ADDR);
    VoC_add16_rp(REG5,REG5,REG2,DEFAULT);


    VoC_add16inc_rp(REG5,REG5,REG2,DEFAULT);
    VoC_lw16_sd(REG4,4,DEFAULT);
    VoC_sub16_rd(REG4,REG4,CONST_1_ADDR);
    VoC_sw16_d(REG5,LOCAL_HUFF_SFBOUND_ADDR);
    VoC_sw16_sd(REG2,9,DEFAULT);//,"sfbw"
    VoC_bnez16_r(huffdec_L8,REG4);                          //  if (--rcount == 0) {
    VoC_lw16_sd(REG5,8,DEFAULT);//,"chan"
    VoC_bnez16_d(huffdec_L9,LOCAL_HUFF_REGION_ADDR);        //    if (region == 0)
    VoC_lw16i_short(REG4,13,DEFAULT);
    VoC_add16_rr(REG4,REG5,REG4,DEFAULT);
    //      VoC_add16_rd(REG4,REG5,CONST_13_ADDR);
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_add16_rp(REG4,REG3,REG4,DEFAULT);                   //      rcount = channel->region1_count + 1;
huffdec_L9:                                                     //    else
    //      rcount = 0;
    VoC_lw16_d(REG2,LOCAL_HUFF_REGION_ADDR);
    VoC_add16_rd(REG5,REG5,CONST_6_ADDR);
    VoC_inc_p(REG2,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG2,DEFAULT);                   //    entry     = &mad_huff_pair_table[channel->table_select[++region]];
    VoC_sw16_d(REG2,LOCAL_HUFF_REGION_ADDR);
//      VoC_mult16_pd(REG5,REG5,CONST_2_ADDR);
    VoC_lw16i_short(REG3,2,DEFAULT);
    VoC_mult16_rp(REG5,REG3,REG5,DEFAULT);

    VoC_lw16i(REG3,TABLE_mad_huff_pair_table_ADDR);
    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);
    VoC_lw16i_set_inc(REG2,LOCAL_HUFF_TABLE_ADDR,1);
    VoC_lw16inc_p(REG1,REG3,DEFAULT);                       //    table     = entry->table;
    VoC_bnez16_r(huffdec_L10,REG1);                         //    if (table == 0)
    VoC_lw16i(REG7,MAD_ERROR_BADHUFFTABLE);                 //      return MAD_ERROR_BADHUFFTABLE;
    VoC_jump(huffdec_Lpopend);
huffdec_L10:
    VoC_lw16inc_p(ACC0_HI,REG3,DEFAULT);                    //    linbits   = entry->linbits;
    VoC_sw16inc_p(REG1,REG2,DEFAULT);
    VoC_movreg16(REG1,ACC0_HI,DEFAULT);                     //    startbits = entry->startbits;
    VoC_and16_ri(REG1,0x00ff);
    VoC_movreg16(REG1,ACC0_HI,DEFAULT);                     //  }
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG1,REG2,DEFAULT);
    VoC_shr16_ri(REG1,8,IN_PARALLEL);
    VoC_NOP();
    VoC_sw16inc_p(REG1,REG2,DEFAULT);                    //  }
    exit_on_warnings=ON;
huffdec_L8:
    VoC_sw16_sd(REG4,4,DEFAULT);
    VoC_lw16_sd(REG2,3,DEFAULT);//,"expptr"
    VoC_lw16_sd(REG4,2,DEFAULT);//,"exp"
    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_be16_rr(huffdec_L11,REG4,REG5);                     //  if (exp != *expptr) {
    VoC_sw16_sd(REG5,2,DEFAULT);                     //,"exp"    exp = *expptr;
    VoC_sw16_d(REG3,LOCAL_HUFF_REQHITS_ADDR);               //    reqhits = 0;
    //  }
huffdec_L11:
    VoC_sw16_sd(REG2,3,DEFAULT);                  //  ++expptr;
    //}
huffdec_L7:
    VoC_lw16i_short(REG3,21,DEFAULT);
    VoC_bngt16_rr(huffdec_L12,REG3,REG6);          //if (cachesz < 21) {
//  VoC_bnlt16_rd(huffdec_L12,REG6,CONST_21_ADDR);          //if (cachesz < 21) {
    //  unsigned int bits;
    VoC_lw16i_short(REG3,31,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG6,DEFAULT);
//  VoC_sub16_dr(REG3,CONST_31_ADDR,REG6);                  //  bits       = ((32 - 1 - 21) + (21 - cachesz)) & ~7;
    VoC_and16_ri(REG3,0xFFF8);
    VoC_lw32_sd(RL7,0,DEFAULT);                 //  bitcache   = (bitcache << bits) | mad_bit_read(&peek, bits);
    VoC_sub16_rr(REG7,REG7,REG3,IN_PARALLEL);
    VoC_sub16_dr(REG4,CONST_0_ADDR,REG3);
    VoC_add16_rr(REG6,REG6,REG3,DEFAULT);
    VoC_shru32_rr(RL7,REG4,IN_PARALLEL);
    VoC_lw32z(REG45,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_bgt16_rd(huffdec_L13,REG3,CONST_16_ADDR);

    VoC_rbinc_r(REG4,REG3,DEFAULT);

    VoC_jump(huffdec_L14);
huffdec_L13:
    VoC_sub16_rd(REG3,REG3,CONST_16_ADDR);
    VoC_rbinc_r(REG5,REG3,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,16,DEFAULT);
huffdec_L14:
    VoC_or32_rr(RL7,REG45,DEFAULT);
    //  cachesz   += bits;
    //  bits_left -= bits;
    VoC_sw16_sd(REG6,5,DEFAULT);//,"cachesz"
    VoC_sw16_sd(REG7,10,DEFAULT);//,"bits_l"
    VoC_sw32_sd(RL7,0,IN_PARALLEL);                         //,"bitcache"}
huffdec_L12:
    /* hcod (0..19) */
    VoC_lw32_sd(RL7,0,DEFAULT);//,"bitcache"
    VoC_lw32_d(ACC0,CONST_1_ADDR);
    VoC_lw16_d(REG5,LOCAL_HUFF_STARTBITS_ADDR);             //clumpsz = startbits;
    VoC_sub16_rr(REG3,REG6,REG5,DEFAULT);                   //pair    = &table[MASK(bitcache, cachesz, clumpsz)];
    VoC_sub16_dr(REG4,CONST_0_ADDR,REG5);
    VoC_shr32_rr(ACC0,REG4,DEFAULT);                        // (((cache) >> ((sz) - (bits))) & ((1 << (bits)) - 1))
    VoC_shru32_rr(RL7,REG3,IN_PARALLEL);
    VoC_sub32_rd(REG23,ACC0,CONST_1_ADDR);
    VoC_and32_rr(REG23,RL7,DEFAULT);
    VoC_add16_rd(REG7,REG2,LOCAL_HUFF_TABLE_ADDR);

huffdec_L15:
    VoC_lw32_sd(RL7,0,DEFAULT);//,"bitcache"
    VoC_lw16_p(REG4,REG7,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);
    VoC_and16_rr(REG3,REG4,DEFAULT);
    VoC_bnez16_r(huffdec_L16,REG3);                          //while (!pair->final) {
    VoC_sub16_rr(REG6,REG6,REG5,DEFAULT);                   //  cachesz -= clumpsz;
    VoC_movreg16(REG5,REG4,IN_PARALLEL);
    VoC_shr16_ri(REG5,1,DEFAULT);                          //  clumpsz = pair->ptr.bits;
    VoC_shr16_ri(REG4,4,IN_PARALLEL);
    VoC_and16_ri(REG5,0x7);                                                                 // pair->ptr.bits in REG5
    VoC_lw16i(REG2,0x0FFF);
    VoC_and16_rr(REG4,REG2,DEFAULT);                                                // pair->ptr.offset in REG4
    VoC_sub16_rr(REG3,REG6,REG5,IN_PARALLEL);
    VoC_lw32_d(ACC0,CONST_1_ADDR);
    VoC_sub16_dr(REG2,CONST_0_ADDR,REG5);
    VoC_shr32_rr(ACC0,REG2,DEFAULT);
    VoC_shru32_rr(RL7,REG3,IN_PARALLEL);
    VoC_sub32_rd(REG23,ACC0,CONST_1_ADDR);
    VoC_and32_rr(REG23,RL7,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_add16_rd(REG7,REG2,LOCAL_HUFF_TABLE_ADDR);          //  pair    = &table[pair->ptr.offset + MASK(bitcache, cachesz, clumpsz)];
    VoC_jump(huffdec_L15);                                  //}
huffdec_L16:
    VoC_movreg16(REG3,REG4,DEFAULT);                        // pair in reg3
    VoC_shr16_ri(REG4,1,DEFAULT);                           //cachesz -= pair->value.hlen;
    VoC_lw16i_short(REG7,0xF,IN_PARALLEL);
    VoC_and16_ri(REG4,0x7);
    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_movreg16(REG2,REG3,IN_PARALLEL);
    VoC_shr16_ri(REG2,4,DEFAULT);
    VoC_and16_rr(REG2,REG7,DEFAULT);
    VoC_shr16_ri(REG3,8,IN_PARALLEL);
    VoC_and16_rr(REG3,REG7,DEFAULT);
    VoC_sw16_sd(REG6,5,IN_PARALLEL);//,"cachesz"
    VoC_lw16i_short(REG7,2,DEFAULT);
    VoC_push32(REG23,DEFAULT);
    VoC_bez16_d(huffdec_L20,LOCAL_HUFF_LINBITS_ADDR);       //if (linbits) {
    /* x (0..14) */
    VoC_jal(CII_huff_sub1);
    /* y (0..14) */
    VoC_jal(CII_huff_sub1);
    VoC_jump(huffdec_L30);                          //}
huffdec_L20:                                            //      else {
    /* x (0..1) */
    VoC_jal(CII_huff_sub2);
    /* y (0..1) */
    VoC_jal(CII_huff_sub2);                         //}
huffdec_L30:
    VoC_pop32(REG23,DEFAULT);                       //xrptr += 2;
    VoC_jump(huffdec_L4);                           // } end of while (big_values-- && cachesz + bits_left > 0)

huffdec_LbgEnd:                                         //      }  /* big_values end*/
    /* big_values overrun */
    VoC_lw16_sd(REG4,5,DEFAULT);      //  ,"cachesz"
    VoC_lw16_sd(REG5,10,DEFAULT); //,"bits_l"
    VoC_add16_rr(REG6,REG4,REG5,DEFAULT);
    VoC_bnltz16_r(huffdec_L39,REG6);                //if (cachesz + bits_left < 0)
    VoC_lw16i(REG7,MAD_ERROR_BADHUFFDATA);          //return MAD_ERROR_BADHUFFDATA;
    VoC_jump(huffdec_Lpopend);
    /* count1 */
huffdec_L39:

    VoC_lw16_sd(REG6,2,DEFAULT);//,"exp"
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(CII_III_requantize);                    //requantized = III_requantize(1, exp);
    VoC_lw16_sd(REG6,8,DEFAULT);            //,"chan" table = mad_huff_quad_table[channel->flags & count1table_select];
    VoC_lw16i_short(REG4,4,IN_PARALLEL);
    VoC_add16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_lw16_sd(REG2,7,IN_PARALLEL); // ,"xr"
    VoC_lw16i(REG5,1144);
    VoC_add16_rr(REG2,REG2,REG5,DEFAULT);
//  VoC_add16_rd(REG2,REG2,CONST_1144_ADDR);

    VoC_lw16_p(REG5,REG6,DEFAULT);
    VoC_and16_ri(REG5,0x1);

    VoC_lw16i(REG6,TABLE_hufftabA_ADDR);
    VoC_bez16_r(TABLE_hufftabA,REG5)
    VoC_lw16i(REG6,TABLE_hufftabB_ADDR);

TABLE_hufftabA:
    VoC_lw16_sd(REG4,5,DEFAULT);     //,"cachesz"
    VoC_sw16_d(REG2,LOCAL_HUFF_XRPTR_ADDR);         // &xr[572] stored in LOCAL_HUFF_XRPTR_ADDR

    VoC_lw16_sd(REG5,10,DEFAULT);// ,"bits_l"
    VoC_sw16_d(REG6,LOCAL_HUFF_TABLE_ADDR);
    VoC_add16_rr(REG6,REG4,REG5,DEFAULT);
    VoC_lw16_sd(REG1,0,IN_PARALLEL);//,"xrptr"

huffdec_L40:                                            //{   requantized in reg7
    VoC_bgt16_rd(huffdec_L41,REG1,LOCAL_HUFF_XRPTR_ADDR);
    VoC_bngtz16_r(huffdec_L41,REG6);                //while (cachesz + bits_left > 0 && xrptr <= &xr[572]) {
    VoC_jump(huffdec_L42);
huffdec_L41:
    VoC_jump(huffdec_L50);
huffdec_L42:
    /* hcod (1..6) */
    VoC_lw32_sd(RL7,0,DEFAULT); //,"bitcache"

    VoC_bnlt16_rd(huffdec_L43,REG4,CONST_10_ADDR);  //if (cachesz < 10) {
    VoC_lbinc_p(REG0);                              //bitcache   = (bitcache << 16) | mad_bit_read(&peek, 16);
    VoC_rbinc_i(REG6,16,DEFAULT);
    VoC_lw32z(REG23,IN_PARALLEL);
    VoC_movreg16(REG2,REG6,DEFAULT);
    VoC_shru32_ri(RL7,-16,IN_PARALLEL);

    VoC_or32_rr(RL7,REG23,DEFAULT);
    VoC_lw16i_short(REG1,16,IN_PARALLEL);
    VoC_add16_rr(REG4,REG4,REG1,DEFAULT);           //cachesz   += 16;
    VoC_sub16_rr(REG5,REG5,REG1,IN_PARALLEL);   //bits_left -= 16;
    VoC_sw32_sd(RL7,0,DEFAULT); //,"bitcache"
    VoC_sw16_sd(REG5,10,DEFAULT);         //,"bits_l"}
huffdec_L43:                                                                            //reg4 reg7
    VoC_movreg32(REG23,RL7,DEFAULT);                        //quad = &table[MASK(bitcache, cachesz, 4)];
    VoC_lw32z(RL7,IN_PARALLEL);
    VoC_sub16_rd(REG5,REG4,CONST_4_ADDR);           // (((cache) >> ((sz) - (bits))) & ((1 << (bits)) - 1))
//  VoC_lw32_d(RL7,CONST_0x0000000f_ADDR);
    VoC_lw16i_short(RL7_LO,0xf,DEFAULT);
    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_shru32_rr(REG23,REG5,IN_PARALLEL);

    VoC_and32_rr(REG23,RL7,DEFAULT);
    VoC_add16_rd(REG2,REG2,LOCAL_HUFF_TABLE_ADDR);
    VoC_NOP();
    VoC_lw16_p(REG3,REG2,DEFAULT);
    VoC_and16_rr(REG5,REG3,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);
    /* quad tables guaranteed to have at most one extra lookup */
    VoC_bnez16_r(huffdec_L44,REG5);                 //if (!quad->final) {
    VoC_sub16_rd(REG4,REG4,CONST_4_ADDR);           //  cachesz -= 4;
    VoC_movreg16(REG6,REG3,DEFAULT);                //  quad = &table[quad->ptr.offset +
    VoC_shr16_ri(REG3,1,IN_PARALLEL);              //                MASK(bitcache, cachesz, quad->ptr.bits)];
    VoC_and16_ri(REG3,0x7);
    VoC_shr16_ri(REG6,4,DEFAULT);
    VoC_and16_ri(REG6,0x0FFF);
    VoC_add16_rd(REG6,REG6,LOCAL_HUFF_TABLE_ADDR);
    VoC_lw32_sd(RL7,0,DEFAULT);                    //,"bitcache"
    VoC_sub16_rr(REG5,REG4,REG3,IN_PARALLEL);
    VoC_sub16_dr(REG3,CONST_0_ADDR,REG3);
    VoC_shru16_rr(REG2,REG3,DEFAULT);
    VoC_shru32_rr(RL7,REG5,IN_PARALLEL);
    VoC_sub16_rd(REG2,REG2,CONST_1_ADDR);
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_and32_rr(REG23,RL7,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_NOP();
    VoC_lw16_p(REG3,REG2,DEFAULT);          //}
huffdec_L44:
    VoC_movreg16(REG5,REG3,DEFAULT);
    VoC_shr16_ri(REG5,1,DEFAULT);
    VoC_movreg16(REG1,REG3,IN_PARALLEL);
    VoC_and16_ri(REG5,0x7);
    VoC_sub16_rr(REG4,REG4,REG5,DEFAULT);   //cachesz -= quad->value.hlen;
    VoC_movreg16(REG6,REG3,IN_PARALLEL);
    VoC_shr16_ri(REG1,4,DEFAULT);
    VoC_movreg16(REG2,REG3,IN_PARALLEL);
    VoC_sw16_sd(REG4,5,DEFAULT);//,"cachesz"
    VoC_shr16_ri(REG6,5,IN_PARALLEL);
    VoC_shr16_ri(REG3,6,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_and16_rr(REG1,REG5,DEFAULT);
    VoC_shr16_ri(REG2,7,IN_PARALLEL);
    VoC_and16_rr(REG6,REG5,DEFAULT);
    VoC_movreg16(ACC1_HI,REG1,IN_PARALLEL);
    VoC_and16_rr(REG2,REG5,DEFAULT);
    VoC_movreg16(ACC1_LO,REG6,IN_PARALLEL);
    VoC_and16_rr(REG3,REG5,DEFAULT);
    VoC_push32(ACC1,DEFAULT);
    VoC_push32(REG23,DEFAULT);

    VoC_jal(CII_huff_sub3);
    VoC_jal(CII_huff_sub3);

    VoC_sw16_sd(REG4,5,DEFAULT);  //,"cachesz"
    VoC_pop32(ACC1,IN_PARALLEL);
    VoC_lw16_sd(REG5,10,DEFAULT);//,"bits_l"
    VoC_pop32(ACC1,IN_PARALLEL);
    VoC_add16_rr(REG6,REG4,REG5,DEFAULT);
    VoC_lw16_sd(REG1,0,IN_PARALLEL);         // ,"xrptr"
    VoC_jump(huffdec_L40);                                  //}
huffdec_L50:
    VoC_lw32z(ACC0,DEFAULT);
    VoC_bnltz16_r(huffdec_L49,REG6);                        //    if (cachesz + bits_left < 0)
    VoC_sub16_rd(REG1,REG1,CONST_8_ADDR);                   //{
    VoC_NOP();                                              //  xrptr -= 4;
    VoC_sw16_sd(REG1,0,DEFAULT);                   //,"xrptr"}
huffdec_L49:
    //assert(-bits_left <= MAD_BUFFER_GUARD * CHAR_BIT);
    /* rzero */
    VoC_lw16i_short(REG6,8,DEFAULT);
    VoC_lw16i_short(REG4,2,IN_PARALLEL);
    VoC_add16_rd(REG6,REG6,LOCAL_HUFF_XRPTR_ADDR);

    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG1,IN_PARALLEL);
    VoC_and16_rr(REG4,REG6,DEFAULT);
    VoC_shr16_ri(REG6,2,IN_PARALLEL);

    VoC_bngtz16_r(huffdec_L51_1,REG6);
huffdec_L51_2:
    VoC_bngtz16_r(huffdec_L51_1,REG6);               //while (xrptr < &xr[576]) {

    VoC_sw32inc_p(ACC0,REG1,DEFAULT);       //  xrptr[0] = 0;  xrptr[1] = 0;
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sub16_rd(REG6,REG6,CONST_1_ADDR);
    VoC_jump(huffdec_L51_2);

huffdec_L51_1:
    VoC_bez16_r(huffdec_L51,REG4);                  //  xrptr += 2;
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);               //  xrptr[0] = 0;  xrptr[1] = 0;
    //}
huffdec_L51:
    VoC_lw16i(REG7,MAD_ERROR_NONE);                         //return MAD_ERROR_NONE;
huffdec_Lpopend:
    VoC_pop16(REG5,DEFAULT);                       //,"xrptr" stack16[11]
    VoC_pop16(REG5,DEFAULT);                  //,"big_values" stack16[10]
    VoC_pop16(REG5,DEFAULT);                         //,"exp" pop16 stack16[9]  exp
    VoC_pop16(REG5,DEFAULT);                      //,"expptr" pop16 stack16[8]  expptr
    VoC_pop16(REG5,DEFAULT);                                // pop16 stack16[7]  rcount
huffdec_Lpopend2:
    VoC_pop16(REG5,DEFAULT);                     //,"cachesz" pop16 stack16[6] cachesz
    VoC_pop16(REG5,DEFAULT);                         //,"ptr" pop16 stack16[5]
    VoC_pop16(REG1,DEFAULT);                          //,"xr" pop16 stack16[4]
    VoC_pop16(REG5,DEFAULT);                        //,"chan" pop16 stack16[3]
    VoC_pop16(REG5,DEFAULT);                        //,"sfbw" pop16 stack16[2]
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_pop16(REG5,DEFAULT);                      // ,"bits_l"pop16 stack16[1]
    VoC_pop32(RL7,IN_PARALLEL);       // ,"bitcache"
huffdec_Lend:
    VoC_pop16(RA,DEFAULT);
    VoC_lw16_d(REG5,PTR_LEFT_ADDR);
    VoC_lw16_d(REG0,PTR_BYTE_ADDR_ADDR);
    VoC_lw32_d(RL6,PTR_CACHE_ADDR);
    VoC_movreg16(BITCACHE,REG5,DEFAULT);
//  spy_on_addr=OFF;
    VoC_return;
}
/**************************************************************
  Function:  CII_III_exponents
  Input:  REG4-> channel                // stack16[n+2]
          REG5-> sfbwidth[ch]           // stack16[n+3]
  Version 1.0  Created by kenneth  2005/06/17
**************************************************************/
void CII_III_exponents(void)
{
#if 0
    voc_short LOCAL_EXPONENTS_ADDR, 39, y
#endif
    VoC_movreg16(REG2,REG5,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_add16_rd(REG6,REG4,CONST_2_ADDR);
    VoC_add16_rd(REG7,REG4,CONST_4_ADDR);
//  VoC_sub16_pd(REG6,REG6,CONST_210_ADDR);         //gain = (signed short) channel->global_gain - 210;
    VoC_lw16i(REG0,210);
    VoC_sub16_pr(REG6,REG6,REG0,DEFAULT);         //gain = (signed short) channel->global_gain - 210;


    VoC_lw32_p(REG01,REG7,DEFAULT);
    VoC_lw16i_short(REG3,2,IN_PARALLEL);
    VoC_and16_rr(REG3,REG0,DEFAULT);                //scalefac_multiplier = (channel->flags & scalefac_scale) ? 2 : 1;
    VoC_lw16i_short(REG7,-2,IN_PARALLEL);
    VoC_bnez16_r(exponents_L1,REG3);
    VoC_lw16i_short(REG7,-1,DEFAULT);
exponents_L1:
    VoC_movreg32(ACC1,REG67,DEFAULT);
    VoC_movreg32(REG67,REG01,IN_PARALLEL);

    VoC_add16_rd(REG3,REG4,CONST_14_ADDR);
    VoC_lw16i_set_inc(REG1,TABLE_pretab_ADDR,1);
    VoC_lw16i_set_inc(REG0,LOCAL_EXPONENTS_ADDR,1);

    VoC_be16_rd(no_exponents_L2,REG7,CONST_2_ADDR);   //if (channel->block_type == 2) {
    VoC_jump(exponents_L2);
no_exponents_L2:
    VoC_lw16i_short(REG7,0,DEFAULT);                //  sfbi = l = 0;
    VoC_lw16i_short(REG5,8,IN_PARALLEL);
    VoC_and16_rr(REG5,REG6,DEFAULT);                //if (channel->flags & mixed_block_flag) {
    VoC_lw32z(RL7,IN_PARALLEL);


    VoC_bez16_r(exponents_L3,REG5);
    VoC_and16_ri(REG6,4);                           //premask = (channel->flags & preflag) ? ~0 : 0;
    VoC_bez16_r(exponents_L5,REG6);
    VoC_not16_r(REG7,DEFAULT);
exponents_L5:
    VoC_movreg16(RL7_LO,REG7,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_movreg32(REG67,ACC1,IN_PARALLEL);
    /* long block subbands 0-1 */
exponents_L4:
    VoC_lw16i(REG5,35);
    VoC_bgt16_rr(exponents_L3,REG4,REG5);  //while (l < 36)
//      VoC_bgt16_rd(exponents_L3,REG4,CONST_35_ADDR);  //while (l < 36)
    VoC_lw16inc_p(REG5,REG1,DEFAULT);               //{
    VoC_movreg16(REG4,RL7_LO,IN_PARALLEL);
    VoC_and16_rr(REG5,REG4,DEFAULT);                //exponents[sfbi] = gain -
    VoC_add16inc_rp(REG5,REG5,REG3,DEFAULT);
    VoC_movreg16(RL7_LO,REG4,IN_PARALLEL);
    VoC_shr16_rr(REG5,REG7,DEFAULT);                //  (signed short) ((channel->scalefac[sfbi] + (pretab[sfbi] & premask)) <<
    VoC_movreg16(REG4,RL7_HI,IN_PARALLEL);
    VoC_sub16_rr(REG5,REG6,REG5,DEFAULT);           //              scalefac_multiplier);
    VoC_add16inc_rp(REG4,REG4,REG2,DEFAULT);                //l += sfbwidth[sfbi++];
    VoC_sw16inc_p(REG5,REG0,DEFAULT);               //}
    VoC_movreg16(RL7_HI,REG4,IN_PARALLEL);          //  l in RL7_HI
    VoC_jump(exponents_L4);
    //}
exponents_L3:
    /* this is probably wrong for 8000 Hz short/mixed blocks */
    VoC_lw16_sd(REG5,2,DEFAULT);//,"chan"
    VoC_movreg32(REG67,ACC1,IN_PARALLEL);
    VoC_lw16i_short(REG1,9,DEFAULT);
    VoC_add16_rr(REG1,REG5,REG1,DEFAULT);
//      VoC_add16_rd(REG1,REG5,CONST_9_ADDR);           // reg1->channel->subblock_gain[0]
    VoC_push32(REG23,DEFAULT);
    VoC_lw16inc_p(REG4,REG1,DEFAULT);               //gain0 = gain - 8 * (signed short) channel->subblock_gain[0];
    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_shr16_ri(REG4,-3,IN_PARALLEL);
    VoC_sub16_rr(REG4,REG6,REG4,DEFAULT);
    VoC_shr16_ri(REG5,-3,IN_PARALLEL);
    VoC_sub16_rr(REG5,REG6,REG5,DEFAULT);           //gain1 = gain - 8 * (signed short) channel->subblock_gain[1];
    VoC_lw16inc_p(REG2,REG1,IN_PARALLEL);
    VoC_shr16_ri(REG2,-3,DEFAULT);                  //gain2 = gain - 8 * (signed short) channel->subblock_gain[2];
    VoC_movreg32(ACC0,REG45,IN_PARALLEL);           // gain1/gain0 in acc0
    VoC_pop32(REG23,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG2,IN_PARALLEL);       // gain2 in reg6
    VoC_movreg16(REG5,RL7_HI,DEFAULT);
exponents_L20:
    VoC_lw16i(REG1,575);
    VoC_bngt16_rr(no_exponents_Lend,REG5,REG1);       //while (l < 576) {
//  VoC_bngt16_rd(no_exponents_Lend,REG5,CONST_575_ADDR);       //while (l < 576) {
    VoC_jump(exponents_Lend);
no_exponents_Lend:

    VoC_lw16inc_p(REG1,REG3,DEFAULT);               //  exponents[sfbi + 0] = gain0 -
    VoC_movreg32(REG45,ACC0,IN_PARALLEL);
    VoC_shr16_rr(REG1,REG7,DEFAULT);                //(signed short) (channel->scalefac[sfbi + 0] << scalefac_multiplier);
    VoC_lw16i_short(INC2,3,IN_PARALLEL);
    VoC_sub16_rr(REG1,REG4,REG1,DEFAULT);
    VoC_lw16inc_p(REG4,REG3,IN_PARALLEL);           //  exponents[sfbi + 1] = gain1 -
    VoC_shr16_rr(REG4,REG7,DEFAULT);                //(signed short) (channel->scalefac[sfbi + 1] << scalefac_multiplier);
    VoC_sub16_rr(REG4,REG5,REG4,DEFAULT);
    VoC_sw16inc_p(REG1,REG0,IN_PARALLEL);
    VoC_lw16inc_p(REG1,REG3,DEFAULT);               //  exponents[sfbi + 2] = gain2 -
    VoC_shr16_rr(REG1,REG7,DEFAULT);                //(signed short) (channel->scalefac[sfbi + 2] << scalefac_multiplier);
    VoC_sw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_sub16_rr(REG1,REG6,REG1,DEFAULT);
    VoC_lw16i_short(REG4,3,IN_PARALLEL);
    VoC_movreg16(REG5,RL7_HI,DEFAULT);
    VoC_mult16inc_rp(REG4,REG4,REG2,IN_PARALLEL);

    VoC_sw16inc_p(REG1,REG0,DEFAULT);           //  sfbi += 3;
    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);           //  l    += 3 * sfbwidth[sfbi];
    VoC_movreg16(RL7_HI,REG5,DEFAULT);              //}
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_jump(exponents_L20);                               //}
exponents_L2:
    //  else {   channel->block_type != 2
    VoC_lw16i_short(REG5,4,DEFAULT);
    VoC_and16_rr(REG5,REG6,DEFAULT);
    VoC_movreg32(REG67,ACC1,IN_PARALLEL);

    VoC_bez16_r(exponents_L6,REG5);                         //if (channel->flags & preflag) {
//      VoC_add16inc_pp(REG4,REG3,REG1,DEFAULT);
    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_add16inc_rp(REG4,REG4,REG3,DEFAULT);

    VoC_loop_i_short(22,DEFAULT);                           //  for (sfbi = 0; sfbi < 22; ++sfbi) {
    VoC_shr16_rr(REG4,REG7,IN_PARALLEL);
    VoC_startloop0
    //exponents[sfbi] = gain -
    //  (signed short) ((channel->scalefac[sfbi] + pretab[sfbi]) <<
    VoC_sub16_rr(REG4,REG6,REG4,DEFAULT);            //              scalefac_multiplier);
    VoC_lw16inc_p(REG5,REG1,IN_PARALLEL);
    VoC_add16inc_rp(REG4,REG5,REG3,DEFAULT);
    //      VoC_add16inc_pp(REG4,REG3,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    VoC_shr16_rr(REG4,REG7,IN_PARALLEL);
    exit_on_warnings=ON;                             //  }
    VoC_endloop0
    VoC_jump(exponents_Lend);                               //}
exponents_L6:                                                   //else {
    VoC_lw16inc_p(REG4,REG3,DEFAULT);
    VoC_loop_i_short(22,DEFAULT);                           //  for (sfbi = 0; sfbi < 22; ++sfbi) {
    VoC_shr16_rr(REG4,REG7,IN_PARALLEL);
    VoC_startloop0

    VoC_sub16_rr(REG4,REG6,REG4,DEFAULT);            //  (signed short) (channel->scalefac[sfbi] << scalefac_multiplier);
    VoC_lw16inc_p(REG4,REG3,DEFAULT);                //  }
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    VoC_shr16_rr(REG4,REG7,IN_PARALLEL);
    exit_on_warnings=ON;                             //  }
    VoC_endloop0                 //}
exponents_Lend:
    VoC_NOP();
    VoC_return;
}
/******************************************************************
 Function name: CII_bitoff
 input:  REG6 <- L_var1

 output  REG6 -> var_out
 Version 1.0  created by wangbin   2006/06/12

*******************************************************************/

void CII_bitoff(void)
{
    VoC_lw16i_set_inc(REG1,8,-1);
    VoC_bnez16_r(bitoff_L1,REG6);
    VoC_jump(bitoff_end);
bitoff_L1:
    VoC_bgtz16_r(bitoff_L2,REG6);
    VoC_jump(bitoff_end);
bitoff_L2:
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);
    VoC_jump(bitoff_L1);
bitoff_end:
    VoC_movreg16(REG6,REG1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_return
}

/******************************************************************
 Function name: CII_III_requantize
 input:  reg4->value
         reg6->exp
 output  reg7
 Version 1.0  created by kenneth   2005/06/20
 Version 1.1  optimized by kenneth   2005/07/10
 Version 1.2  changed by elisa     2006/06/21
 output  ACC0    32bit output requantiaed
*******************************************************************/

void  CII_III_requantize(void)
{
    //Word16 shift = 0;
    VoC_push16(RA,DEFAULT);
    VoC_movreg16(REG7,REG6,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_bnltz16_r(III_requantize_L1,REG7);
    VoC_sub16_rr(REG7,REG5,REG7,DEFAULT);
III_requantize_L1:
    VoC_movreg16(REG3, REG7,DEFAULT);
    VoC_lw16i_short(WRAP0,16,IN_PARALLEL);
    VoC_shr16_ri(REG3,2,DEFAULT);           //exp /= 4;->REG3
    VoC_and16_ri(REG7,0x3);                 //frac = exp % 4;->REG7
    VoC_bnltz16_r(III_requantize_L2,REG6);
    VoC_sub16_rr(REG7,REG5,REG7,DEFAULT);
    VoC_sub16_rr(REG3,REG5,REG3,IN_PARALLEL);
III_requantize_L2:
    VoC_movreg16(REG6, REG3,DEFAULT);

    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);
    VoC_push32(REG67,DEFAULT);        //->REG67:exp = reg6 / frac = reg7
    VoC_push16(REG0,IN_PARALLEL);
    //CII_rq_compute;                //requantized = rq_compute(value, &shift);
    /******************************************************************
     Function name: CII_rq_compute
     input:  reg4->value
             reg3->exp
     output  RL7
     Version 1.0  created by kenneth   2005/06/20
     Version 1.1  optimized by kenneth   2005/07/10
    *******************************************************************/
    // begin of CII_rq_compute
    VoC_lw32z(RL7,DEFAULT);
    VoC_bez16_r(rq_compute_L0,REG4);        //if(value == 0)
    //{       *exp =0;
    //        return (0);
    //}
    VoC_bne16_rd(rq_compute_L1,REG4,CONST_1_ADDR);  //if(value==1)
    VoC_lw16i_short(REG3,2,DEFAULT);        //{     *exp=2;
    VoC_lw16i(RL7_HI,0x400);         //      return(0x04000000);

rq_compute_L0:
    VoC_jump(rq_compute_Lend);              //}
rq_compute_L1:
//  VoC_lw16i_set_inc(REG2,TABLE_rq_r_ADDR+2,2);

    VoC_movreg16(RL7_HI,REG4,DEFAULT);      //value_ref = (value << 16);
    VoC_lw32z(REG45,IN_PARALLEL);           //temp1 = 0;      //Q26
//  VoC_lw32inc_p(ACC1,REG2,DEFAULT);

    VoC_lw16i(ACC1_HI,0x4000);

    VoC_lw16i_short(ACC1_LO,0,DEFAULT);   //rq_r[i];
    VoC_lw16i_short(FORMATX,14,IN_PARALLEL);

    VoC_movreg32(REG67,ACC1,DEFAULT);
    VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);

    // in this part of the loop ACC1_LO is always 0,
    // so RL6_LO, REG6 & REG4 is always 0

    VoC_loop_i_short(14,DEFAULT);                   //for(i=1;i<15;i++)
    VoC_startloop0                                  //{

    VoC_multf32_rr(REG01,REG7,REG7,DEFAULT);     // temp = Mpy_32(S_Temp1,S_Temp2,S_Temp1,S_Temp2);
    VoC_shr32_ri(ACC1,1,DEFAULT);

    VoC_multf32_rr(ACC0,REG7,REG1,DEFAULT);     // temp = Mpy_32(  S_Temp3,S_Temp4,S_Temp1,S_Temp2);
    VoC_shru16_ri(REG0,1,IN_PARALLEL);

    VoC_macX_rr(REG6,REG1,DEFAULT);
    VoC_macX_rr(REG7,REG0,DEFAULT);

    VoC_movreg32(REG67,REG45,DEFAULT);
    VoC_movreg32(RL6,REG67,IN_PARALLEL);

    VoC_bgt32_rr(rq_compute_L2,ACC0,RL7);       // if(temp <= value_ref)
    VoC_movreg32(REG45,RL6,DEFAULT);        //            temp1 = temp2;
    VoC_movreg32(REG67,RL6,IN_PARALLEL);
rq_compute_L2:

    VoC_or32_rr(REG67,ACC1,DEFAULT);            // temp2=temp1 | rq_r[i]

    VoC_endloop0                                    //}


    // in this part of the loop ACC1_HI is always 0 (at the point where it is used)

    VoC_loop_i_short(17,DEFAULT);                   //for(i=1;i<15;i++)
    VoC_startloop0                                  //{

    VoC_lw16i_short(FORMATX,13,DEFAULT);
    VoC_movreg32(RL6,REG67,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG7,REG7,DEFAULT);     // temp = Mpy_32(S_Temp1,S_Temp2,S_Temp1,S_Temp2);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_macX_rr(REG7,REG6,DEFAULT);
    VoC_shr32_ri(ACC1,1,DEFAULT);

    VoC_movreg32(REG01,ACC0,DEFAULT);
    VoC_lw16i_short(FORMATX,14,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG7,REG1,DEFAULT);     // temp = Mpy_32(  S_Temp3,S_Temp4,S_Temp1,S_Temp2);
    VoC_shru16_ri(REG0,1,IN_PARALLEL);

    VoC_macX_rr(REG6,REG1,DEFAULT);
    VoC_macX_rr(REG7,REG0,DEFAULT);

    VoC_movreg32(REG67,REG45,DEFAULT);

    VoC_bgt32_rr(rq_compute_L2_bis,ACC0,RL7);   //        if(temp <= value_ref)
    VoC_movreg32(REG45,RL6,DEFAULT);        //                temp1 = temp2;
    VoC_movreg32(REG67,RL6,IN_PARALLEL);
rq_compute_L2_bis:
    VoC_or32_rr(REG67,ACC1,DEFAULT); //        temp2=temp1 | rq_r[i]

    VoC_endloop0


    VoC_movreg32(REG67,RL7,DEFAULT);  //value_ref -> RL7
    //temp1 -> RL6
    //CII_gsm_norm;                          //        shift = norm_l(value_ref);
    //begin of gsm_norm
    VoC_lw16i_short(REG0,0,IN_PARALLEL);
    VoC_bnltz32_r(gsm_norm_L1,REG67);                       //if (a < 0)
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i(ACC0_HI,0xc000);
    VoC_bgt32_rr(gsm_norm_L2,REG67,ACC0); //{     if (a <= -1073741824) return 0;
//  VoC_bgt32_rd(gsm_norm_L2,REG67,CONST_0xc0000000L_ADDR); //{     if (a <= -1073741824) return 0;
    VoC_lw16i(REG6,CONST_0_ADDR);
    VoC_jump(gsm_norm_Lend);
gsm_norm_L2:
    VoC_sub32_dr(REG67,CONST_0_ADDR,REG67);         //      a = ~a;
gsm_norm_L1:                                            //}

//  VoC_movreg32(ACC0,REG67,DEFAULT);
//  VoC_and32_rd(ACC0,CONST_0xFFFF0000_ADDR);       //return          a & 0xffff0000

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0xFFFF,DEFAULT);
    VoC_and32_rr(ACC0,REG67,DEFAULT);       //return          a & 0xffff0000

    VoC_bez32_r(gsm_norm_L3,ACC0);

//      VoC_movreg32(ACC0,REG67,DEFAULT);
//      VoC_and32_rd(ACC0,CONST_0XFF000000_ADDR);       // ? ( a & 0xff000000
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i(ACC0_HI,0xff00);
    VoC_and32_rr(ACC0,REG67,DEFAULT);

    VoC_bez32_r(gsm_norm_L4,ACC0);
    VoC_shr32_ri(REG67,24,DEFAULT);                 //        ?  -1 + bitoff[ 0xFF & (a >> 24) ]
    VoC_lw16i_short(REG0,-1,IN_PARALLEL);
    VoC_jump(gsm_norm_L6);
gsm_norm_L4:
    VoC_shr32_ri(REG67,16,DEFAULT);                 //                :   7 + bitoff[ 0xFF & (a >> 16) ] )
    VoC_lw16i_short(REG0,7,IN_PARALLEL);
    VoC_jump(gsm_norm_L6);
gsm_norm_L3:
//  VoC_movreg32(ACC0,REG67,DEFAULT);
//  VoC_and32_rd(ACC0,CONST_0X0000FF00_ADDR);       //              : ( a & 0xff00

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i(ACC0_LO,0xff00);
    VoC_and32_rr(ACC0,REG67,DEFAULT);


    VoC_bez32_r(gsm_norm_L5,ACC0);
    VoC_shr32_ri(REG67,8,DEFAULT);                  //                ?  15 + bitoff[ 0xFF & (a >> 8) ]
    VoC_lw16i_short(REG0,15,IN_PARALLEL);
    VoC_jump(gsm_norm_L6);
gsm_norm_L5:
    VoC_lw16i_short(REG0,23,DEFAULT);               //                :  23 + bitoff[ 0xFF & a ] );
gsm_norm_L6:
    VoC_lw16i(REG1,255);
    VoC_and16_rr(REG6,REG1,DEFAULT);
//  VoC_and16_rd(REG6,CONST_255_ADDR);
gsm_norm_Lend:


    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_jal(CII_bitoff);

    VoC_add16_rr(REG6,REG6,REG0,DEFAULT);
    VoC_sub16_rr(REG6,REG2,REG6,DEFAULT);

    // end of gsm_norm
    VoC_shru16_ri(REG4,1,IN_PARALLEL);              //      L_Extract (temp1,&S_Temp1, &S_Temp2);

    VoC_shr32_rr(RL7,REG6,DEFAULT);                 //      value_ref = L_shl(value_ref, (short)(shift));
    VoC_movreg16(REG3,REG6,IN_PARALLEL);
    VoC_movreg32(REG67,RL7,DEFAULT);            //      L_Extract (value_ref,&S_Temp3, &S_Temp4);
    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_multf32_rr(ACC0,REG7,REG5,DEFAULT);
    VoC_macX_rr(REG7,REG4,DEFAULT);               //temp1 = Mpy_32(  S_Temp3,S_Temp4,S_Temp1,S_Temp2);      // Q6.26
    VoC_macX_rr(REG6,REG5,DEFAULT);

    //CII_gsm_norm;                          //*exp = norm_l(temp1);
    //begin of gsm_norm
    VoC_lw16i_short(REG2,4,IN_PARALLEL);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_movreg32(RL7,ACC0,DEFAULT);
    VoC_bnltz32_r(gsm_norm_L1a,RL7);                       //if (a < 0)
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i(ACC0_HI,0xc000);
    VoC_bgt32_rr(gsm_norm_L2a,RL7,ACC0); //{     if (a <= -1073741824) return 0;
//  VoC_bgt32_rd(gsm_norm_L2a,ACC0,CONST_0xc0000000L_ADDR); //{     if (a <= -1073741824) return 0;
    VoC_lw16i(REG6,CONST_0_ADDR);
    VoC_jump(gsm_norm_Lenda);
gsm_norm_L2a:
    VoC_sub32_dr(ACC0,CONST_0_ADDR,RL7);         //      a = ~a;
gsm_norm_L1a:                                            //}
    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0xFFFF,DEFAULT);
    VoC_and32_rr(ACC0,REG67,DEFAULT);

//      VoC_and32_rd(ACC0,CONST_0xFFFF0000_ADDR);       //return          a & 0xffff0000
    VoC_bez32_r(gsm_norm_L3a,ACC0);

//      VoC_movreg32(ACC0,REG67,DEFAULT);
//      VoC_and32_rd(ACC0,CONST_0XFF000000_ADDR);       // ? ( a & 0xff000000

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i(ACC0_HI,0xff00);
    VoC_and32_rr(ACC0,REG67,DEFAULT);

    VoC_bez32_r(gsm_norm_L4a,ACC0);
    VoC_shr32_ri(REG67,24,DEFAULT);                 //        ?  -1 + bitoff[ 0xFF & (a >> 24) ]
    VoC_lw16i_short(REG0,-1,IN_PARALLEL);
    VoC_jump(gsm_norm_L6a);
gsm_norm_L4a:
    VoC_shr32_ri(REG67,16,DEFAULT);                 //                :   7 + bitoff[ 0xFF & (a >> 16) ] )
    VoC_lw16i_short(REG0,7,IN_PARALLEL);
    VoC_jump(gsm_norm_L6a);
gsm_norm_L3a:
//      VoC_movreg32(ACC0,REG67,DEFAULT);
//      VoC_and32_rd(ACC0,CONST_0X0000FF00_ADDR);       //              : ( a & 0xff00

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i(ACC0_LO,0xff00);
    VoC_and32_rr(ACC0,REG67,DEFAULT);

    VoC_bez32_r(gsm_norm_L5a,ACC0);
    VoC_shr32_ri(REG67,8,DEFAULT);                  //                ?  15 + bitoff[ 0xFF & (a >> 8) ]
    VoC_lw16i_short(REG0,15,IN_PARALLEL);
    VoC_jump(gsm_norm_L6a);
gsm_norm_L5a:
    VoC_lw16i_short(REG0,23,DEFAULT);               //                :  23 + bitoff[ 0xFF & a ] );
gsm_norm_L6a:
//      VoC_and16_rd(REG6,CONST_255_ADDR);
    VoC_lw16i(REG1,255);
    VoC_and16_rr(REG6,REG1,DEFAULT);
gsm_norm_Lenda:
    VoC_add16_rd(REG3,REG3,CONST_17_ADDR);
    VoC_jal(CII_bitoff);

    VoC_add16_rr(REG6,REG6,REG0,DEFAULT);
    VoC_sub16_rr(REG6,REG2,REG6,DEFAULT);
    //end of gsm_norm
    VoC_shr32_rr(RL7,REG6,DEFAULT);                //temp1 = L_shl(temp1, (short)(*exp-4));
    VoC_add16_rr(REG3,REG6,REG3,IN_PARALLEL);      //     *exp = 21 - *exp-shift;
    // return(temp1);
rq_compute_Lend:
    // end of CII_rq_compute

    VoC_pop32(REG67,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_add16_rr(REG6,REG6,REG3,DEFAULT);   //exp += shift;
    VoC_lw16i_short(REG4,1,IN_PARALLEL);
    /*****************************************************************/
    /*end of rq_computer*/
    /*  exp-> reg6         frac -> reg7     requantized -> RL7     */
    /*****************************************************************/

    VoC_bnltz16_r(III_requantize_L3,REG6);  //if (exp < 0) {
    VoC_lw16i(REG3,-31);
    VoC_bngt16_rr(III_requantize_L5,REG3,REG6)
//      VoC_bnlt16_rd(III_requantize_L5,REG6,CONST_NEG31_ADDR); //  if (-exp >= sizeof(mad_fixed_t) * CHAR_BIT) {
    VoC_lw32z(RL7,DEFAULT);                 //    requantized = 0;
    VoC_jump(III_requantize_L4);            //  }
III_requantize_L5:                              //  else {

    VoC_add16_rr(REG3,REG4,REG6,DEFAULT);   //    requantized += 1L << (-exp - 1);
    VoC_shr32_rr(REG45,REG3,DEFAULT);
    VoC_add32_rr(RL7,RL7,REG45,DEFAULT);
    VoC_sub16_dr(REG3,CONST_0_ADDR,REG6);
    VoC_shr32_rr(RL7,REG3,DEFAULT);         //    requantized >>= -exp;
    //  }
    VoC_sub16_rr(REG3,REG4,REG6,DEFAULT);

    /*requantized -> RL7  */
    VoC_jump(III_requantize_L4);            //}
III_requantize_L3:                              //else {
    VoC_lw16i_short(REG3,5,DEFAULT);
    VoC_bgt16_rr(III_requantize_L6,REG3,REG6)
//  VoC_blt16_rd(III_requantize_L6,REG6,CONST_5_ADDR);      //  if (exp >= 5) {
//  VoC_lw32_d(RL7,CONST_0x7fffffff_ADDR);  //    requantized = MAD_F_MAX;
    VoC_lw16i(RL7_HI,0x7fff);
    VoC_lw16i_short(RL7_LO,0xffff,DEFAULT);
    VoC_jump(III_requantize_L4);            //  }
III_requantize_L6:                              //  else
    VoC_sub16_dr(REG3,CONST_0_ADDR,REG6);   //    requantized <<= exp;
    VoC_shr32_rr(RL7,REG3,DEFAULT);         //}
III_requantize_L4:


//  VoC_lw16i(REG3,TABLE_root_table_ADDR+3);
//  VoC_add16_rr(REG3,REG3,REG7,DEFAULT);         //root_table[3 + frac] -> REG3

    VoC_lw16i(REG3,0x0983);
    VoC_lw16i_short(REG4,-3,DEFAULT);
    VoC_lw16i_short(REG5,-2,IN_PARALLEL);

    VoC_be16_rr(root_table_end,REG7,REG4)
    VoC_lw16i(REG3,0x0b50);
    VoC_be16_rr(root_table_end,REG7,REG5)

    VoC_lw16i(REG3,0x0d74);
    VoC_be16_rd(root_table_end,REG7,CONST_neg1_ADDR)
    VoC_lw16i(REG3,0x1000);
    VoC_be16_rd(root_table_end,REG7,CONST_0_ADDR)
    VoC_lw16i(REG3,0x1307);
    VoC_be16_rd(root_table_end,REG7,CONST_1_ADDR)
    VoC_lw16i(REG3,0x16a1);
    VoC_be16_rd(root_table_end,REG7,CONST_2_ADDR)
    VoC_lw16i(REG3,0x1ae9);
root_table_end:


    VoC_movreg32(REG45,RL7,DEFAULT);
    //modified by elisa for 20*16bit mutiply 060619

    VoC_lw16i_short(FORMATX,0,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);


    VoC_add32_rd(REG45,REG45,CONST_0x00000800_ADDR);

    VoC_shru16_ri(REG4,12,DEFAULT);
    VoC_multf32_rr(ACC0,REG5,REG3,DEFAULT);
    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_shru32_ri(ACC0,-4,DEFAULT);
    VoC_macX_rr(REG4,REG3,DEFAULT);

// end modify by elisa 060619

    VoC_bnez16_r(III_requantize_Lend,REG7);  //      return frac ? Q28to14(requantized1)  : Q28to14(requantized);
    VoC_movreg32(ACC0,RL7,DEFAULT);//added by elisa pour ACC0 <- RL7 <- requantized
III_requantize_Lend:

    VoC_pop16(REG0,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);
    VoC_pop16(RA,DEFAULT);
    VoC_bez16_d(III_requantize_exit, GLOBAL_WRAP_INDEX_ADDR)
    VoC_lw16i_short(WRAP0,10,DEFAULT);
III_requantize_exit:
    VoC_return;
}


/*
 * NAME:    decode->error()
 * DESCRIPTION: handle a decoding error
 */


//input REG6   :  stream->error
//output  REG7

void CII_decode_error(void)
{
    VoC_push16(RA,DEFAULT);                                          // switch (stream->error) {
    VoC_lw16i_short(REG7, MAD_FLOW_CONTINUE, DEFAULT);  //     return MAD_FLOW_CONTINUE;
    VoC_lw16i_short(WRAP0,10,DEFAULT);
    VoC_lw16i(REG0,0x235);
    VoC_bne16_rr(no_decode_error_exit, REG6, REG0    )
//  VoC_bne16_rd(no_decode_error_exit, REG6, CONST_0x235_ADDR    )     //  case MAD_ERROR_BADDATAPTR:
    VoC_jump(decode_error_exit);
no_decode_error_exit:
//    VoC_lw16i(REG0,0x101);
//  VoC_be16_rr(no_decode_error_default, REG6,REG0);  //    case MAD_ERROR_LOSTSYNC:
////    VoC_be16_rd(no_decode_error_default, REG6,CONST_0x101_ADDR);  //    case MAD_ERROR_LOSTSYNC:
//  VoC_jump(decode_error_default);
//no_decode_error_default:
//      VoC_lw16d_set_inc(REG0,STRUCT_MAD_STREAM_THIS_FRAME_ADDR_ADDR,1);
////        VoC_sub16_dr(REG5, STRUCT_MAD_STREAM_BUFEND_ADDR_ADDR,REG0 );   //   stream->bufend - stream->this_frame
//
//      VoC_lw32z(RL6,DEFAULT);
//      VoC_movreg16(REG4, REG0,IN_PARALLEL);
//      VoC_and16_ri(REG4,1);
//      VoC_shru16_ri(REG0,1,DEFAULT);
//      VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);
//
//      VoC_blt16_rd(DECODE_ERROR_BEYOND1, REG0,CONST_1024_ADDR)
//          VoC_sub16_rd(REG0,REG0,CONST_1024_ADDR);
//DECODE_ERROR_BEYOND1:
//      VoC_bez16_r(decode_error_even, REG4)
//          VoC_lbinc_p(REG0);
//          VoC_rbinc_i(REG4,8,DEFAULT);
//decode_error_even:       //      REG0    :     stream->this_frame
//
//      VoC_jal( CII_id3_tag_query );//   tagsize = id3_tag_query(stream->this_frame,    stream->bufend - stream->this_frame);
//      VoC_movreg16(REG4, RL7_LO,DEFAULT);
//      VoC_add16_rd(REG4, REG4, STRUCT_MAD_STREAM_SKIPLEN_ADDR);
//decoder_error103:
//      VoC_blt16_rd(decoder_error102,REG4,CONST_2048_ADDR)
//          VoC_push16(REG6,DEFAULT);
//          VoC_lw16_d(REG6,GLOBLE_WRITE_POINTER_ADDR);
//          VoC_lw16i_short(REG5,1,DEFAULT);
//          VoC_lw16i(REG7,512);
//          VoC_sw16_d(REG5,STRUCT_MAD_STREAM_SYNC_ADDR);
//          VoC_sw16_d(REG6,STRUCT_MAD_STREAM_THIS_FRAME_ADDR_ADDR);
//          VoC_sw16_d(REG6,STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR);
//          VoC_jal(CII_reload_data_temp);
//          VoC_sub16_rd(REG4,REG4,CONST_2048_ADDR);
//          VoC_pop16(REG6,DEFAULT);
//          VoC_jump(decoder_error103);
//decoder_error102:
//      VoC_bngtz16_r(decode_error_default, REG4)
//      VoC_sw16_d(REG4, STRUCT_MAD_STREAM_SKIPLEN_ADDR);
//      VoC_jump(decode_error_exit);
//    /* fall through */
//decode_error_default:
    VoC_lw16i(REG0,0x201);
    VoC_bne16_rr(decode_error_exit, REG6, REG0  ) //   if (stream->error == MAD_ERROR_BADCRC) {
    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_jal(CII_mad_frame_mute);
    VoC_lw16i(REG7, MAD_FLOW_IGNORE);
decode_error_exit:
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(WRAP0,16,DEFAULT);
    VoC_return


}

//
///*
// * NAME:  tag->query()
// * DESCRIPTION:   if a tag begins at the given location, return its size
// */
//   //RL7  :  length
////REG0  :  *data
//void  CII_id3_tag_query(void)
//{
//  VoC_push16(RA,DEFAULT);
//  VoC_push32(REG67,IN_PARALLEL);
//  VoC_push16(REG0,DEFAULT);
//  VoC_push32(RL6,IN_PARALLEL);
//  VoC_push16(BITCACHE,DEFAULT);
//
//  VoC_jal(CII_tagtype);                 //switch (tagtype(data, length)) {
//  VoC_pop16(BITCACHE,DEFAULT);
//  VoC_pop32(RL6,IN_PARALLEL);
//
//  VoC_pop16(REG0,DEFAULT);
//  VoC_lw32z(RL7,IN_PARALLEL);
//  VoC_lw16i(RL7_LO, 128);
//  VoC_be16_rd(id3_tag_query_exit, REG3, CONST_1_ADDR)      //  case TAGTYPE_ID3V1:     return 128;
//  VoC_bne16_rd(id3_tag_query_go, REG3, CONST_2_ADDR)     //  case TAGTYPE_ID3V2:
//      VoC_jal(CII_parse_header);     //  parse_header(&data, &version, &flags, &size);
//      VoC_lw16i_short(REG6,10,DEFAULT);
//      VoC_add32_rr(RL7, RL7, REG67,DEFAULT);
//      VoC_and16_ri(REG4, ID3_TAG_FLAG_FOOTERPRESENT);  //
//      VoC_bez16_r(id3_tag_query_exit, REG4)       //  if (flags & ID3_TAG_FLAG_FOOTERPRESENT)
//          VoC_add32_rr(RL7, RL7, REG67,DEFAULT);     //   size += 10;
//          VoC_jump(id3_tag_query_exit)                     //   return 10 + size;
//id3_tag_query_go:
//      VoC_lw32z(RL7,DEFAULT);
//      VoC_bez16_r(id3_tag_query_exit, REG3)   //   case TAGTYPE_NONE:    return 0;
//          VoC_jal(CII_parse_header);     // case TAGTYPE_ID3V2_FOOTER:     parse_header(&data, &version, &flags, &size);
//          VoC_lw16i_short(REG6,10,DEFAULT);
//          VoC_sub32_dr(RL7, CONST_0_ADDR, RL7);
//          VoC_sub32_rr(RL7, RL7, REG67,DEFAULT); //  return -size - 10;
//id3_tag_query_exit:
//  VoC_pop16(RA,DEFAULT);
//  VoC_pop32(REG67,DEFAULT);
//  VoC_return
//
//}
//
////output :
////REG4   flags
////RL7     size
//void CII_parse_header(void)
//{
//  VoC_lbinc_p(REG0);
//  VoC_rbinc_i(REG4,16,DEFAULT);
//  VoC_lbinc_p(REG0);
//  VoC_rbinc_i(REG4,16,DEFAULT);     //     *ptr += 3;
//
//  VoC_lbinc_p(REG0);
//  VoC_rbinc_i(REG4,8,DEFAULT);         // *version = id3_parse_uint(ptr, 2);
//  VoC_lw32z(REG67,IN_PARALLEL);
//
//  VoC_rbinc_i(REG4,8,DEFAULT);        // *flags   = id3_parse_uint(ptr, 1);
//  VoC_lw32z(RL7,IN_PARALLEL);
//
//  VoC_loop_i_short(4,DEFAULT)
//  VoC_startloop0
//      VoC_lbinc_p(REG0);
//      VoC_rbinc_i(REG6,8,DEFAULT);
//      VoC_shru32_ri(RL7,-7,IN_PARALLEL);
//      VoC_and16_ri(REG7,0x7f);
//      VoC_or32_rr(RL7, REG67,DEFAULT);   // *size    = id3_parse_syncsafe(ptr, 4);
//  VoC_endloop0
//  VoC_return
//}
//
//
////REG5  :  length
////REG0  :  *data
////output:   REG3
//void  CII_tagtype( void )
//{
//  VoC_lbinc_p(REG0);
//  VoC_rbinc_i(REG4,8,DEFAULT);     //  data[0]
//  VoC_rbinc_i(REG6,8,DEFAULT);      //data[1]
//  VoC_lbinc_p(REG0);
//  VoC_rbinc_i(REG7,8,DEFAULT);      //data[2]
//  VoC_shru16_ri(REG6, -8,IN_PARALLEL);
//
//  VoC_or16_rr(REG6, REG4,DEFAULT);
//  VoC_lw16i_short(REG3, TAGTYPE_ID3V1, IN_PARALLEL);
//  VoC_and16_ri(REG7,0x00ff);         //REG67   :   data[2]  data[1,0]
//  VoC_lw16i(RL7_LO,0x4154);
//  VoC_lw16i(RL7_HI,0x47);
//
//  VoC_blt16_rd(tagtype_go_on, REG5,CONST_3_ADDR)         //   if (length >= 3 &&
////    VoC_bne32_rd(tagtype_go_on, REG67, CONST_0x474154_ADDR)  //  data[0] == 'T' && data[1] == 'A' && data[2] == 'G')
//  VoC_bne32_rr(tagtype_go_on, REG67, RL7)  //  data[0] == 'T' && data[1] == 'A' && data[2] == 'G')
//      VoC_return   //   return TAGTYPE_ID3V1;
//tagtype_go_on:
//
//  VoC_lw16i_short(REG3, TAGTYPE_NONE, DEFAULT);
//  VoC_blt16_rd(tagtype_exit_quick, REG5,CONST_10_ADDR)            // if (length >= 10 &&
//      VoC_lw16i(RL7_LO,0x4449);
//      VoC_lw16i(RL7_HI,0x33);
//      VoC_be32_rr(tagtype_exit_go, REG67, RL7)     //   ((data[0] == 'I' && data[1] == 'D' && data[2] == '3') ||
////        VoC_be32_rd(tagtype_exit_go, REG67, CONST_0x334449_ADDR)     //   ((data[0] == 'I' && data[1] == 'D' && data[2] == '3') ||
//          VoC_lw16i(RL7_LO,0x4433);
//      VoC_lw16i(RL7_HI,0x49);
//      VoC_bne32_rr(tagtype_exit_quick, REG67, RL7)     //     (data[0] == '3' && data[1] == 'D' && data[2] == 'I')) &&
//          VoC_jump(tagtype_exit_go);
//tagtype_exit_quick:
//          VoC_return         //    return TAGTYPE_NONE;
//
//tagtype_exit_go:
//  VoC_NOP();
//  VoC_lbinc_p(REG0);
//  VoC_rbinc_i(REG6,8,DEFAULT);      //data[3]
//  VoC_rbinc_i(REG7,8,DEFAULT);      //data[4]
//  VoC_lw16i(REG2,0x00ff);
//  VoC_bngt16_rr(tagtype_exit, REG2, REG6)
//      VoC_bngt16_rr(tagtype_exit, REG2, REG7)      //        data[3] < 0xff && data[4] < 0xff &&
//          VoC_lbinc_p(REG0);
//          VoC_rbinc_i(REG6,8,DEFAULT);      //data[5]
//          VoC_lbinc_p(REG0);
//          VoC_rbinc_i(REG6,8,DEFAULT);      //data[6]
//          VoC_rbinc_i(REG7,8,DEFAULT);      //data[7]
//          VoC_lw16i(REG2,0x0080);
//          VoC_bngt16_rr(tagtype_exit, REG2, REG6)
//          VoC_bngt16_rr(tagtype_exit, REG2, REG7)
//              VoC_lbinc_p(REG0);
//              VoC_rbinc_i(REG6,8,DEFAULT);      //data[8]
//              VoC_rbinc_i(REG7,8,DEFAULT);      //data[9]
//          VoC_bngt16_rr(tagtype_exit, REG2, REG6)
//          VoC_bngt16_rr(tagtype_exit, REG2, REG7)   //   data[6] < 0x80 && data[7] < 0x80 && data[8] < 0x80 && data[9] < 0x80)
//              VoC_lw16i_short(REG3, TAGTYPE_ID3V2, DEFAULT);
//          VoC_bnez16_r(tagtype_exit, REG4)
//              VoC_lw16i_short(REG3, TAGTYPE_ID3V2_FOOTER, DEFAULT);   //  return data[0] == 'I' ? TAGTYPE_ID3V2 : TAGTYPE_ID3V2_FOOTER;
//tagtype_exit:
//  VoC_return         //    return TAGTYPE_NONE;
//
//}
/*************************************************************************************
 *Function : CII_III_stereo()
 *INPUT:      &xr[][]       ,    ->REG0  (incr 1)
 *            &granule      ,    ->REG1  (incr 1)
 *            sfbwidth      ,    ->REG3  (incr 1)
 *
 *OUTPUT:  REG4
 *CHANGED registers :
 *Description:
 *Version 1.0  Created by BinWang    06/06/2005
 *Version 1.1  optimized by BinWang  23/06/2005
 *Version 1.2  optimized by Kenneth  20/07/2005
 ***************************************************************************************/

void CII_III_stereo(void)
{

#if 0
    voc_short LOCAL_modes_ADDR, 40, y
    voc_short LOCAL_bound_ADDR, 4 ,y
#endif

    VoC_lw16i_short(FORMATX,0,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_add16_rd(REG4,REG1,CONST_4_ADDR);                     //REG4 : &granule->ch[0].flags
    VoC_lw16i(REG5,58);
    VoC_add16_rr(REG1,REG1,REG5,DEFAULT);                    //REG5 : &granule->ch[1].flags
    VoC_lw32_p(REG45,REG4,DEFAULT);                           //REG4 : granule->ch[0].flags
    VoC_lw16i_short(REG2,mixed_block_flag,IN_PARALLEL);       //REG5 : granule->ch[0].block_type
    VoC_lw32_p(REG67,REG1,DEFAULT);                           //REG6 : granule->ch[1].flags
    //REG7 : granule->ch[1].block_type

    VoC_bne16_rr(III_stereo_Lerr,REG5,REG7);                   //granule->ch[0].block_type !=granule->ch[1].block_type
    VoC_and16_rr(REG4,REG2,DEFAULT);                      //granule->ch[0].flags & mixed_block_flag
    VoC_and16_rr(REG6,REG2,DEFAULT);                      //granule->ch[1].flags & mixed_block_flag
    VoC_be16_rr(III_stereo_Lbeg,REG4,REG6);                   //(granule->ch[0].flags & mixed_block_flag) !=(granule->ch[1].flags & mixed_block_flag)
III_stereo_Lerr:
    VoC_lw16i(REG4,MAD_ERROR_BADSTEREO);
    VoC_jump(III_stereo_L52);               //return MAD_ERROR_BADSTEREO
III_stereo_Lbeg:
    VoC_push16(REG0,DEFAULT);               // stack16[0]  xr
    VoC_push16(REG3,DEFAULT);           // stack16[1]  sfbw
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,LOCAL_modes_ADDR,2);             //REG2 : &modes[]
    VoC_lw16_d(REG4,STRUCT_MAD_HEADER_MODE_EXTENSION_ADDR); //REG4 : header->mode_extension
    VoC_movreg16(REG5,REG4,DEFAULT);
    VoC_push16(REG1,IN_PARALLEL);       // stack16[2] right_ch


    VoC_loop_i_short(19,DEFAULT);                            //for (i = 0; i < 39; ++i)
    VoC_lw32z(RL7,IN_PARALLEL);
    VoC_startloop0
    VoC_sw32inc_p(REG45,REG2,DEFAULT);               //modes[i] = header->mode_extension
    VoC_movreg16(RL6_LO,REG4,IN_PARALLEL);
    VoC_endloop0
    VoC_sw16_p(REG4,REG2,DEFAULT);


    VoC_and16_ri(REG4,I_STEREO);                              //REG4 : header->mode_extension & I_STEREO
    VoC_bnez16_r(III_stereo_Le0,REG4);                        //if (header->mode_extension & I_STEREO)
    VoC_jump(III_stereo_L44);
III_stereo_Le0:

    VoC_lw16_d(REG4,STRUCT_MAD_HEADER_FLAGS_ADDR);            //REG6 : header->flags
    VoC_or16_ri(REG4,MAD_FLAG_I_STEREO);                  //header->flags | MAD_FLAG_I_STEREO
    VoC_add16_rd(REG2,REG0,CONST_1152_ADDR);                   //REG2 : *right_xr = xr[1]
    VoC_sw16_d(REG4,STRUCT_MAD_HEADER_FLAGS_ADDR);            //header->flags |= MAD_FLAG_I_STEREO
    VoC_movreg16(ACC1_LO,REG2,DEFAULT);          //ACC1_LO : &xr[1]
    VoC_lw32z(REG45,IN_PARALLEL);                //REG4 : l = 0

    // first determine which scalefactor bands are to be processed

    VoC_be16_rd(no_III_stereo_L18,REG7,CONST_2_ADDR);           //if (right_ch->block_type == 2)

    VoC_jump(III_stereo_L18);
no_III_stereo_L18:
    VoC_sw32_d(RL7,LOCAL_bound_ADDR);              //REG2(incr 1) : &bound[]
    VoC_sw16_d(RL7_LO,LOCAL_bound_ADDR+2);

    VoC_lw16i_short(RL6_HI,0,DEFAULT);             //RL7_LO : lower=0  RL7_HI : start=0
    VoC_lw16i_short(REG0,0,IN_PARALLEL);           //REG7 : sfbi=0


    VoC_bez16_r(III_stereo_L7,REG6);           //if (right_ch->flags & mixed_block_flag)

III_stereo_L3:
    VoC_lw16inc_p(REG6,REG3,DEFAULT);           //REG6 : n = sfbwidth[sfbi]
    VoC_inc_p(REG0,IN_PARALLEL);                    //++sfbi
    VoC_add16_rr(REG1,REG2,REG6,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);       //l += n
    VoC_add16_rr(REG1,REG1,REG6,DEFAULT);
    VoC_movreg16(ACC0_LO,REG4,IN_PARALLEL);          //ACC0_LO : l
    VoC_lw32inc_p(REG45,REG2,DEFAULT);               //REG45 : right_xr[i]

    VoC_loop_r(0,REG6);                             //for (i = 0; i < n; ++i)

    VoC_bez32_r(III_stereo_L4,REG45);         //if (right_xr[i])
    VoC_movreg16(RL7_LO,REG0,DEFAULT);            //RL7_LO : lower = sfbi
    VoC_jump(III_stereo_L5);                      //break
III_stereo_L4:
    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_endloop0

III_stereo_L5:
    VoC_movreg16(REG2,REG1,DEFAULT);
    VoC_movreg16(REG4,ACC0_LO,IN_PARALLEL);
    VoC_blt16_rd(III_stereo_L3,REG4,CONST_36_ADDR);           //while (l < 36)

    VoC_movreg16(RL7_HI,REG0,DEFAULT);                    //RL6_HI : start = sfbi

III_stereo_L7:

    VoC_lw16i_short(REG1,0,DEFAULT);                   // w = 0
    VoC_lw16i(REG7,LOCAL_bound_ADDR);
III_stereo_L8:

    VoC_lw16inc_p(REG6,REG3,DEFAULT);              //REG6 : n = sfbwidth[sfbi]
    VoC_inc_p(REG0,IN_PARALLEL);                           //REG0 : sfbi+1
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);       //l += n;

    VoC_add16_rr(REG5,REG6,REG2,IN_PARALLEL);   //right_xr += n;
    VoC_movreg16(ACC0_LO,REG4,DEFAULT);             //ACC0_LO : l
    VoC_add16_rr(REG5,REG5,REG6,IN_PARALLEL);
    VoC_movreg16(RL6_HI,REG5,DEFAULT);
    VoC_lw32inc_p(REG45,REG2,IN_PARALLEL);               //REG45 : right_xr[i]

    VoC_loop_r(0,REG6);                                //for (i = 0; i < n; ++i)

    VoC_bez32_r(III_stereo_L9,REG45);           //if (right_xr[i])
    VoC_add16_rr(REG5,REG7,REG1,DEFAULT);      //REG7 : &bound[w]
    VoC_movreg16(ACC1_HI,REG0,DEFAULT);         //max = sfbi
    VoC_sw16_p(REG0,REG5,DEFAULT);
    VoC_jump(III_stereo_L10);                  //break
III_stereo_L9:
    VoC_lw32inc_p(REG45,REG2,DEFAULT);          //REG45 : right_xr[i]
    VoC_endloop0

III_stereo_L10:

    VoC_movreg16(REG2,RL6_HI,DEFAULT);
    VoC_movreg16(REG4,ACC0_LO,IN_PARALLEL);
    //w = (w + 1) % 3
    VoC_movreg16(REG6,REG1,DEFAULT);

    VoC_bnez16_r(III_stereo_Lw1,REG1);
    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_jump(III_stereo_Lw2);
III_stereo_Lw1:
    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_bne16_rd(III_stereo_Lw2,REG6,CONST_1_ADDR);
    VoC_lw16i_short(REG1,2,DEFAULT);
III_stereo_Lw2:
    VoC_blt16_rd(III_stereo_L8,REG4,CONST_576_ADDR); //while (l < 576)

    VoC_movreg16(REG4,ACC1_HI,DEFAULT);             //REG4 : max
    VoC_bez16_r(III_stereo_L12,REG4);                  //if (max)
    VoC_movreg16(RL7_LO,RL7_HI,DEFAULT);               //lower = start
III_stereo_L12:
    // long blocks


    VoC_movreg16(REG5,RL6_LO,DEFAULT);
    VoC_movreg16(REG4,RL7_LO,IN_PARALLEL);
    VoC_and16_ri(REG5,0xfffe);                         //REG5 : header->mode_extension & ~I_STEREO
    VoC_lw16i_set_inc(REG1,LOCAL_modes_ADDR,1);
    VoC_bngtz16_r(III_stereo_L12_1,REG4);

    VoC_loop_r_short(REG4,DEFAULT);                    //for (i = 0; i < lower; ++i)
    VoC_startloop0
    VoC_sw16inc_p(REG5,REG1,DEFAULT);              //modes[i] = header->mode_extension & ~I_STEREO
    VoC_endloop0

III_stereo_L12_1:
    // short blocks

    VoC_lw16i_short(REG6,0,DEFAULT);                     //w = 0
    VoC_movreg16(REG0,ACC1_HI,DEFAULT);                   //REG0 : max
    VoC_movreg16(REG4,RL7_HI,IN_PARALLEL);               //REG4 : start
    VoC_lw16i_set_inc(REG1,LOCAL_modes_ADDR,1);
III_stereo_L13:

    VoC_bngt16_rr(III_stereo_L17,REG0,REG4);              //for (i = start; i < max; ++i)
    VoC_add16_rr(REG2,REG7,REG6,DEFAULT);                //REG2 : &bound[w]
    VoC_add16_rr(REG3,REG1,REG4,DEFAULT);                //REG1 : &modes[i]

    VoC_lw16_p(REG2,REG2,DEFAULT);                       //REG2 : bound[w]
    VoC_bngt16_rr(III_stereo_L16,REG2,REG4);              //if (i < bound[w])
    VoC_sw16inc_p(REG5,REG3,DEFAULT);                    //modes[i] = header->mode_extension & ~I_STEREO
III_stereo_L16:
    //w = (w + 1) % 3
    VoC_movreg16(REG3,REG6,DEFAULT);
    VoC_bnez16_r(III_stereo_Lw3,REG6);
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_jump(III_stereo_Lw4);
III_stereo_Lw3:
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_bne16_rd(III_stereo_Lw4,REG3,CONST_1_ADDR);
    VoC_lw16i_short(REG6,2,DEFAULT);
III_stereo_Lw4:
    VoC_add16_rd(REG4,REG4,CONST_1_ADDR);                //REG4 : ++i
    VoC_jump(III_stereo_L13);

III_stereo_L17:

    VoC_jump(III_stereo_L23);

III_stereo_L18:
    // right_ch->block_type != 2
    VoC_lw16i_short(REG0,0,DEFAULT);                    // sfbi= 0
    VoC_lw16i_short(REG7,0,DEFAULT);                     //bound=0
    VoC_lw16i_short(REG4,0,IN_PARALLEL);                 //l=0

III_stereo_L19:
    //for (sfbi = l = 0; l < 576; l += n)
    VoC_lw16inc_p(REG6,REG3,DEFAULT);                //n = sfbwidth[sfbi++]
    VoC_add16_rr(REG1,REG2,REG6,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);
    VoC_add16_rr(REG1,REG1,REG6,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);        //l += n
    VoC_movreg16(ACC0_LO,REG4,DEFAULT);      //ACC0_LO : l
    VoC_lw32inc_p(REG45,REG2,IN_PARALLEL);             //REG45 : right_xr[i]

    VoC_loop_r(0,REG6);                              //for (i = 0; i < n; ++i)
    VoC_bez32_r(III_stereo_L20,REG45);            //if (right_xr[i])
    VoC_movreg16(REG7,REG0,DEFAULT);             //{bound = sfbi
    VoC_jump(III_stereo_L21);                    //break}
III_stereo_L20:
    VoC_lw32inc_p(REG45,REG2,DEFAULT);            //REG45 : right_xr[i]
    VoC_endloop0

III_stereo_L21:
    VoC_movreg16(REG2,REG1,DEFAULT);
    VoC_movreg16(REG4,ACC0_LO,IN_PARALLEL);
    VoC_blt16_rd(III_stereo_L19,REG4,CONST_576_ADDR);    //for (sfbi = l = 0; l < 576; l += n)

    VoC_movreg16(REG5,RL6_LO,DEFAULT);                     //REG5 : header->mode_extension
    VoC_lw16i(REG2,LOCAL_modes_ADDR);
    VoC_and16_ri(REG5,0xfffe);                            //REG5 : header->mode_extension & ~I_STEREO

    VoC_bngtz16_r(III_stereo_L23,REG7);
    VoC_lw16i_short(INC2,1,DEFAULT);

    VoC_loop_r_short(REG7,DEFAULT);                      //for (i = 0; i < bound; ++i)
    VoC_startloop0
    VoC_sw16inc_p(REG5,REG2,DEFAULT);            //modes[i] = header->mode_extension & ~I_STEREO
    VoC_endloop0

    VoC_lw16i_short(INC2,2,DEFAULT);
III_stereo_L23:

    /* now do the actual processing */
    VoC_lw16_sd(REG3,1,DEFAULT);//,"sfbw"
    VoC_lw16_sd(REG0,0,DEFAULT);            //,"right_ch" reg0==>&ch[1].flag
    VoC_lw16i(REG4,MAD_FLAG_LSF_EXT);                     //REG4 : MAD_FLAG_LSF_EXT
    VoC_and16_rd(REG4,STRUCT_MAD_HEADER_FLAGS_ADDR);      //REG4 : header->flags & MAD_FLAG_LSF_EXT


    VoC_bez16_r(III_stereo_L35,REG4);                     //if (header->flags & MAD_FLAG_LSF_EXT)

    VoC_sub16_rd(REG5,REG0,CONST_1_ADDR);                //REG5 : right_ch->scalefac_compress
    VoC_lw16i(REG6,TABLE_is_lsf_table_ADDR);
    VoC_add16_rd(REG0,REG0,CONST_10_ADDR);              // reg0->right_ch->scalefac[0]
    VoC_lw16i_set_inc(REG1,STRUCT_GRANULE1CH1_scalefac_ADDR,1);   //*illegal_pos
    VoC_lw16i_set_inc(REG2,LOCAL_modes_ADDR,1);
    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_and16_ri(REG5,0x1);                             //REG5 : right_ch->scalefac_compress & 0x1
    VoC_shr16_ri(REG5,-4,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG6,DEFAULT);                 //REG5 : lsf_scale = is_lsf_table[right_ch->scalefac_compress & 0x1]
    VoC_lw16i_short(REG4,0,IN_PARALLEL);                  //l=0

    VoC_movreg16(RL6_HI,REG5,DEFAULT);                    //RL6_HI : lsf_scale
//  VoC_movreg16()

III_stereo_L24:
    VoC_blt16_rd(III_stereo_L25,REG4,CONST_576_ADDR);
    VoC_jump(III_stereo_L44);
III_stereo_L25:                     //for (sfbi = l = 0; l < 576; ++sfbi, l += n) {
    VoC_lw16inc_p(REG6,REG3,DEFAULT);       //n = sfbwidth[sfbi];
    VoC_movreg16(RL6_LO,REG4,IN_PARALLEL);
    VoC_movreg16(ACC1_LO,REG6,DEFAULT);       //ACC1_LO : n
    VoC_lw16_p(REG5,REG2,DEFAULT);          //if (!(modes[sfbi] & I_STEREO))
    VoC_lw16i_short(REG4,1,IN_PARALLEL);
    VoC_and16_rr(REG4,REG5,DEFAULT);        //  continue;
    VoC_lw16inc_p(REG7,REG1,IN_PARALLEL);
    VoC_bez16_r(III_stereo_La10,REG4);
    VoC_and16_ri(REG5,0xFFFE);          //if (illegal_pos[sfbi]) {

    VoC_bez16_r(III_stereo_L26,REG7);       //  modes[sfbi] &= ~I_STEREO;


    VoC_sw16_p(REG5,REG2,DEFAULT);          //  continue;
    VoC_jump(III_stereo_La10);          //}
III_stereo_L26:
    VoC_lw16_p(REG7,REG0,DEFAULT);          //is_pos = right_ch->scalefac[sfbi];
    VoC_bngtz16_r(III_stereo_La10,REG6);        // n in reg6    l in reg4
    //is_pos in reg7
    VoC_push32(ACC1,DEFAULT);
    VoC_push32(REG01,DEFAULT);
    VoC_movreg16(REG4,RL6_LO,IN_PARALLEL);
    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_lw16_sd(REG0,2,IN_PARALLEL);
    VoC_sub16_rr(REG5,REG7,REG1,DEFAULT);
    VoC_movreg16(RL7_HI,REG7,IN_PARALLEL);
    VoC_and16_rr(REG7,REG1,DEFAULT);
    VoC_shr16_ri(REG5,1,IN_PARALLEL);
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);       // reg0->xr[0][l + 0];
    VoC_movreg16(RL7_LO,REG7,IN_PARALLEL);
    VoC_movreg16(REG1,RL6_HI,DEFAULT);
    //VoC_movreg16(REG7,RL7_HI,IN_PARALLEL);
    VoC_movreg16(REG4,REG6,IN_PARALLEL);
//      VoC_lw16_p(REG4,REG0,DEFAULT);          //   left = xr[0][l + i];
    VoC_lw32_p(REG67,REG0,DEFAULT);

    VoC_add16_rr(REG5,REG5,REG1,IN_PARALLEL);
    VoC_add16_rd(REG1,REG0,CONST_1152_ADDR);
    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);


    VoC_loop_r(0,REG4);             //for (i = 0; i < n; ++i) {
    VoC_movreg16(REG4,RL7_HI,DEFAULT);

    VoC_bez16_r(III_stereo_La11,REG4);  //  if (is_pos == 0)
    VoC_push32(REG67,DEFAULT);//,"left"
    VoC_lw32_d(ACC1,CONST_0x00000800_ADDR);
    VoC_add32_rr(REG67,ACC1,REG67,DEFAULT);


    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_shru32_ri(ACC0,-4,DEFAULT);
    VoC_macX_rr(REG5,REG6,DEFAULT);
    VoC_NOP();

    //  VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    //    xr[1][l + i] = left;
    VoC_movreg16(REG4,RL7_LO,DEFAULT);  //  else {
    VoC_pop32(REG67,IN_PARALLEL);//,"left"
//          VoC_multf32_rr(ACC0,REG6,REG5,IN_PARALLEL);//    opposite =extract_h(L_mult(left, lsf_scale[(is_pos - 1)/2]));
    VoC_bez16_r(III_stereo_La12,REG4);  //    if (is_pos & 1) {
    VoC_sw32_p(ACC0,REG0,DEFAULT);  //      xr[0][l + i] = opposite;
    // xr[1][l + i] = left;
    VoC_jump(III_stereo_La11);      //    }
III_stereo_La12:                        //    else
    VoC_movreg32(REG67,ACC0,DEFAULT);   //      xr[1][l + i] = opposite;
III_stereo_La11:                        //  }
    VoC_inc_p(REG0,DEFAULT);

//          VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_sw32inc_p(REG67,REG1,DEFAULT);

//          VoC_lw16_p(REG6,REG0,DEFAULT);      //   left = xr[0][l + i];
    VoC_lw32_p(REG67,REG0,DEFAULT);

    VoC_movreg16(REG4,RL7_HI,IN_PARALLEL);

    VoC_endloop0                    //}
    VoC_pop32(REG01,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_pop32(ACC1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

III_stereo_La10:

    VoC_movreg16(REG4,RL6_LO,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_movreg16(REG6,ACC1_LO,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);   //}
    VoC_inc_p(REG0,IN_PARALLEL);
    VoC_jump(III_stereo_L24);

III_stereo_L35:
    VoC_lw16i_short(REG6,0,DEFAULT);
    // !(header->flags & MAD_FLAG_LSF_EXT)
    VoC_lw16i_set_inc(REG1,LOCAL_modes_ADDR,1);
    VoC_add16_rd(REG0,REG0,CONST_10_ADDR);          // reg0->right_ch->scalefac[0]
III_stereo_Lb01:
    VoC_blt16_rd(III_stereo_Lb,REG6,CONST_576_ADDR);
    VoC_jump(III_stereo_L44);       // for (sfbi = l = 0; l < 576; ++sfbi, l += n) {
III_stereo_Lb:
    VoC_lw16inc_p(REG5,REG3,DEFAULT);   //n = sfbwidth[sfbi];
    VoC_lw16i_short(REG4,1,IN_PARALLEL);
    VoC_lw16_p(REG2,REG1,DEFAULT);
    VoC_movreg16(RL7_HI,REG6,IN_PARALLEL);
    VoC_and16_rr(REG4,REG2,DEFAULT);
    VoC_lw16inc_p(REG7,REG0,IN_PARALLEL);   //is_pos = right_ch->scalefac[sfbi];
    VoC_bez16_r(III_stereo_Lb10,REG4);  //if (!(modes[sfbi] & I_STEREO))
    //  continue;
    VoC_blt16_rd(III_stereo_Lb0,REG7,CONST_7_ADDR);     //if (is_pos >= 7) {

//  VoC_sub16_rd(REG1,REG1,CONST_1_ADDR);
    VoC_and16_ri(REG2,0xFFFE);      //  modes[sfbi] &= ~I_STEREO;
    VoC_NOP();              //  continue;
    VoC_sw16_p(REG2,REG1,DEFAULT);      //}
    VoC_jump(III_stereo_Lb10);
III_stereo_Lb0:
    VoC_bngtz16_r(III_stereo_Lb10,REG5);
    VoC_push16(REG3,DEFAULT);                             //// stack16[2]  sfbwidth
    VoC_push32(REG01,IN_PARALLEL);
    VoC_movreg16(REG3,REG7,DEFAULT);
    VoC_lw16_sd(REG0,3,IN_PARALLEL);
    VoC_lw16i(REG4,TABLE_is_table_ADDR);
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);
    VoC_add16_rr(REG6,REG4,REG3,IN_PARALLEL);               //REG6 : &is_table[    is_pos]
    VoC_movreg16(REG2,REG0,DEFAULT);
    VoC_sub16_rr(REG3,REG4,REG3,IN_PARALLEL);
    VoC_add16_rd(REG3,REG3,CONST_6_ADDR);                   //REG3 : &is_table[6 - is_pos]
    VoC_add16_rd(REG1,REG0,CONST_1152_ADDR);
    VoC_lw16_p(REG4,REG6,DEFAULT);                          //REG4 : is_table[    is_pos]
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    //VoC_lw16inc_p(REG67,REG2,DEFAULT);            //left = xr[0][l + i];
    VoC_lw16_p(REG3,REG3,DEFAULT);              //REG3 : is_table[6 - is_pos]
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    VoC_loop_r(0,REG5)                  //for (i = 0; i < n; ++i) {
    VoC_lw32_d(ACC0,CONST_0x00000800_ADDR);
    VoC_lw32inc_p(ACC1,REG2,DEFAULT);               // left = xr[0][l + i];
    VoC_add32_rr(REG67,ACC0,ACC1,DEFAULT);

    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_multf32_rr(ACC0,REG4,REG7,DEFAULT);
    VoC_NOP();
    VoC_shru32_ri(ACC0,-4,DEFAULT);
    VoC_macX_rr(REG4,REG6,DEFAULT);
    VoC_NOP();
    VoC_NOP();
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);               //xr[0][l + i] = mad_f_mul(left, is_table[    is_pos]);


    VoC_lw32_d(ACC0,CONST_0x00000800_ADDR);
    VoC_add32_rr(REG67,ACC0,ACC1,DEFAULT);

    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_multf32_rr(ACC0,REG3,REG7,DEFAULT);
    VoC_NOP();
    VoC_shru32_ri(ACC0,-4,DEFAULT);
    VoC_macX_rr(REG3,REG6,DEFAULT);
    VoC_NOP();
    VoC_NOP();
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);              //xr[1][l + i] = mad_f_mul(left, is_table[6 - is_pos]);

    VoC_endloop0                        //}
    VoC_pop32(REG01,DEFAULT);
    VoC_pop16(REG3,IN_PARALLEL);
    VoC_lw16i_short(INC0,1,DEFAULT);
III_stereo_Lb10:
    VoC_movreg16(REG6,RL7_HI,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_add16_rr(REG6,REG6,REG5,DEFAULT);
    VoC_inc_p(REG1,DEFAULT);                //}
    VoC_jump(III_stereo_Lb01);

III_stereo_L44:
    // middle/side stereo
    VoC_pop16(REG1,DEFAULT);                // ,"right_ch"stack16[2] right_ch
    VoC_lw16i_short(REG7,MS_STEREO,IN_PARALLEL);
//  VoC_set_status(4,1);  //set round mode on
    VoC_lw16_d(REG1,STRUCT_MAD_HEADER_MODE_EXTENSION_ADDR);     //REG1 : header->mode_extension
    VoC_pop16(REG3,DEFAULT);//,"sfbw"
    VoC_and16_rr(REG1,REG7,DEFAULT);                               //REG1 : header->mode_extension & MS_STEREO
    VoC_pop16(REG0,DEFAULT);//,"xr"

    VoC_bez16_r(III_stereo_L50,REG1);                           //if (header->mode_extension & MS_STEREO)

//  VoC_lw16_d(REG7,TABLE_root_table_ADDR + 1);                 //REG7 : root_table[1]
    VoC_lw16i(REG7,0x0b50);
    VoC_lw16_d(REG6,STRUCT_MAD_HEADER_FLAGS_ADDR);              //REG6 : header->flags
    VoC_or16_ri(REG6,MAD_FLAG_MS_STEREO);                       //REG6 : header->flags | MAD_FLAG_MS_STEREO


    VoC_add16_rd(REG1,REG0,CONST_1152_ADDR);
    VoC_movreg32(RL7,REG01,DEFAULT);            //xr[0]->rl7_l0  ;  xr[1]->rl7_hi
    VoC_movreg16(REG1,REG3,IN_PARALLEL);            //REG1->sfbwidth[0];
//  VoC_setf16(14);
    VoC_lw32z(REG45,DEFAULT);
    VoC_sw16_d(REG6,STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_lw16i_set_inc(REG0,LOCAL_modes_ADDR,1);
III_stereo_L45:
    VoC_blt16_rd(III_stereo_L45_1,REG4,CONST_576_ADDR);          //for (sfbi = l = 0; l < 576; ++sfbi, l += n)
    VoC_jump(III_stereo_L50);
III_stereo_L45_1:
    VoC_lw16inc_p(REG6,REG3,DEFAULT);                           //n = sfbwidth[sfbi];
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);             //ACC1_LO ：l
    VoC_lw16inc_p(REG5,REG0,DEFAULT);               //REG5 ： modes[sfbi]
    VoC_movreg16(ACC1_HI,REG4,IN_PARALLEL);             //ACC1_HI ：l
    VoC_be16_rd(III_stereo_L45_2,REG5,CONST_2_ADDR);
    VoC_jump(III_stereo_L49);
III_stereo_L45_2:
    VoC_bngtz16_r(III_stereo_L49,REG6);
    VoC_push16(REG3,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_movreg32(REG23,ACC1,DEFAULT);
    VoC_shru32_ri(REG23,-1,DEFAULT);

    VoC_add32_rr(REG23,RL7,REG23,DEFAULT);              //REG2 : xr[0][l + i]    REG3 : xr[1][l + i]
//  VoC_movreg32(ACC0,REG01,IN_PARALLEL);
    VoC_push32(REG01,IN_PARALLEL);
    VoC_movreg16(REG0,REG2,DEFAULT);
//  VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);


    VoC_loop_r(0,REG6);
    VoC_lw32_p(RL6,REG2,DEFAULT);                   //m = xr[0][l + i];
    VoC_lw32_p(ACC0,REG3,DEFAULT);                  //s = xr[1][l + i];
    VoC_add32_rr(RL6,RL6,ACC0,DEFAULT);                //m + s
    VoC_sub32_rr(REG01,RL6,ACC0,IN_PARALLEL);          //m - s

    VoC_lw32_d(ACC0,CONST_0x00000800_ADDR);
    VoC_add32_rr(REG45,ACC0,RL6,DEFAULT);
    VoC_shru16_ri(REG4,12,DEFAULT);
    VoC_multf32_rr(ACC0,REG7,REG5,DEFAULT);

    VoC_NOP();
    VoC_NOP();
    VoC_shru32_ri(ACC0,-4,DEFAULT);
    VoC_macX_rr(REG7,REG4,DEFAULT);
    VoC_NOP();
    VoC_NOP();
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);

    VoC_lw32_d(ACC0,CONST_0x00000800_ADDR);
    VoC_add32_rr(REG45,ACC0,REG01,DEFAULT);

    VoC_shru16_ri(REG4,12,DEFAULT);
    VoC_multf32_rr(ACC0,REG7,REG5,DEFAULT);

    VoC_NOP();
    VoC_shru32_ri(ACC0,-4,DEFAULT);
    VoC_macX_rr(REG7,REG4,DEFAULT);
    VoC_NOP();
    VoC_NOP();
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_endloop0

    VoC_pop16(REG3,DEFAULT);
    VoC_movreg16(REG4,ACC1_HI,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);
III_stereo_L49:
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_jump(III_stereo_L45);
III_stereo_L50:

    VoC_lw16i_short(REG4,MAD_ERROR_NONE,DEFAULT);
III_stereo_L52:
    VoC_return;
}




/*
 * NAME:    III_imdct_l()
 * DESCRIPTION: perform IMDCT and windowing for long blocks
 */
/*
 * NAME:     void CII_III_imdct_l ( void )
 * INPUT:     REG0  --> X[0];  incr = 2
 *             REG1  --> Z[0];  incr = 2
*              REG7               block_type
 * OUTPUT:
 * DESCRIPTION:
 * USED REGISTER:   All
 */

void CII_III_imdct_l(void)
{
    VoC_push16(RA, DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);

    VoC_lw16i_short(FORMATX,0,DEFAULT);
    VoC_lw16i_short(FORMAT32,-4,IN_PARALLEL);

    VoC_push16(REG7, DEFAULT);
    VoC_movreg16(REG4, REG1,IN_PARALLEL);

    /* IMDCT */

    VoC_jal(CII_imdct36);

    VoC_pop32(REG01, DEFAULT);
    VoC_pop16(REG7, IN_PARALLEL);

    VoC_pop16(RA, DEFAULT);
    VoC_movreg16(REG0, REG1,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2, TABLE_window_l_ADDR, 1);
    VoC_lw32_d(RL6,CONST_0x00000800_ADDR);

    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);

    /* windowing */
// switch (block_type) {

//  VoC_set_incr_i(REG3,2,DEFAULT);

    VoC_bnez16_r(III_imdct_l_100, REG7)

    /* normal window */
    /*
    case 0:

        for (i = 0; i < 36; i += 4)
        {

          z[i + 0] = mad_f_mul(z[i + 0], window_l[i + 0]);
          z[i + 1] = mad_f_mul(z[i + 1], window_l[i + 1]);
          z[i + 2] = mad_f_mul(z[i + 2], window_l[i + 2]);
          z[i + 3] = mad_f_mul(z[i + 3], window_l[i + 3]);
        }
        break;
    */


    VoC_add32inc_rp(REG67,RL6,REG1,DEFAULT);
    VoC_multf32_rp(ACC1,REG7,REG2,DEFAULT);
    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_shru32_ri(ACC1,-4,DEFAULT);
    VoC_mac32inc_rp(REG6,REG2,IN_PARALLEL);

    VoC_loop_i_short(36,DEFAULT)
    VoC_add32inc_rp(REG67,RL6,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_multf32_rp(ACC1,REG7,REG2,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC1, REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_shru32_ri(ACC1,-4,DEFAULT);
    VoC_mac32inc_rp(REG6,REG2,IN_PARALLEL);

    VoC_add32inc_rp(REG67,RL6,REG1,DEFAULT);
    VoC_endloop0

    VoC_jump(III_imdct_l_end);
III_imdct_l_100:
    VoC_bne16_rd(III_imdct_l_200, REG7,CONST_1_ADDR)

//  case 1:  /* start block */
//    for (i =  0; i < 18; ++i) z[i] = mad_f_mul(z[i], window_l[i]);
//    /*  (i = 18; i < 24; ++i) z[i] unchanged */
//    for (i = 24; i < 30; ++i) z[i] = mad_f_mul(z[i], window_s[i - 18]);
//    for (i = 30; i < 36; ++i) z[i] = 0;
//    break;

    VoC_lw16i_short(REG5,18,DEFAULT);
    VoC_loop_i(1,2)

    VoC_movreg16(REG0, REG1,DEFAULT);
    VoC_add32inc_rp(REG67,RL6,REG1,DEFAULT);


    VoC_multf32_rp(ACC1,REG7,REG2,DEFAULT);
    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_shru32_ri(ACC1,-4,DEFAULT);
    VoC_mac32inc_rp(REG6,REG2,IN_PARALLEL);

    VoC_loop_r_short(REG5,DEFAULT)
    VoC_add32inc_rp(REG67,RL6,REG1,IN_PARALLEL);
    VoC_startloop0

    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_multf32_rp(ACC1,REG7,REG2,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC1, REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_shru32_ri(ACC1,-4,DEFAULT);
    VoC_mac32inc_rp(REG6,REG2,IN_PARALLEL);

    VoC_add32inc_rp(REG67,RL6,REG1,DEFAULT);
    VoC_lw16i_short(REG5,6,DEFAULT);

    VoC_endloop0
    VoC_lw16i_set_inc(REG2, TABLE_window_s_ADDR+24-18, 1);
    VoC_add16_rd(REG1, REG1,CONST_8_ADDR);
    VoC_endloop1

    VoC_loop_i_short(6,DEFAULT)
    VoC_startloop0
    VoC_sw32inc_p(RL7, REG0,DEFAULT);
    VoC_endloop0
    VoC_jump(III_imdct_l_end)
III_imdct_l_200:
    VoC_bne16_rd(III_imdct_l_end, REG7,CONST_3_ADDR)
    //case 3:  /* stop block */
    /*    for (i =  0; i <  6; ++i) z[i] = 0;
        for (i =  6; i < 12; ++i)
            z[i] =round( L_mult(z[i]  ,window_s[i-6])) ;

    */
    /*  (i = 12; i < 18; ++i) z[i] unchanged */
    /*  for (i = 18; i < 36; ++i)
    z[i] = round( L_mult(z[i] ,window_l[i])) ;
      break;
    }
    */
    VoC_loop_i_short(6,DEFAULT)
    VoC_lw16i_short(REG5,6,IN_PARALLEL);
    VoC_startloop0
    VoC_sw32inc_p(RL7, REG1,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG2, TABLE_window_s_ADDR, 1);
    VoC_loop_i(1,2)

    VoC_movreg16(REG0, REG1,DEFAULT);
    VoC_add32inc_rp(REG67,RL6,REG1,DEFAULT);

    VoC_multf32_rp(ACC1,REG7,REG2,DEFAULT);
    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_shru32_ri(ACC1,-4,DEFAULT);
    VoC_mac32inc_rp(REG6,REG2,IN_PARALLEL);

    VoC_loop_r_short(REG5,DEFAULT)
    VoC_add32inc_rp(REG67,RL6,REG1,IN_PARALLEL);
    VoC_startloop0

    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_multf32_rp(ACC1,REG7,REG2,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC1, REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_shru32_ri(ACC1,-4,DEFAULT);
    VoC_mac32inc_rp(REG6,REG2,IN_PARALLEL);

    VoC_add32inc_rp(REG67,RL6,REG1,DEFAULT);
    VoC_lw16i_short(REG5,18,DEFAULT);

    VoC_endloop0
    VoC_lw16i_set_inc(REG2, TABLE_window_l_ADDR+18, 1);
    VoC_add16_rd(REG1, REG1,CONST_8_ADDR);
    VoC_endloop1

III_imdct_l_end:

    VoC_return
}

/*****************************************************************************************/
/************************* input : REG0  --> X[0];  incr =2*******************************/
/************** CII_III_imdct_s() function modified by elisa 060710  *********************/
/*****************************************************************************************/
/*****************************************************************************************/

void CII_III_imdct_s(void)
{
#if 0
    voc_word LOCAL_III_imdct_s_Y_ADDR   ,20, x
#endif

    //// input : REG0  --> X[0];  incr = 2
    VoC_lw16i_set_inc(REG1, TABLE_imdct_s_ADDR, 1);
    VoC_lw32_d(RL7,CONST_0x00000800_ADDR);

    VoC_push32(REG01,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2, LOCAL_III_imdct_s_Y_ADDR+12, 2);
    VoC_lw16i_set_inc(REG3, LOCAL_III_imdct_s_Y_ADDR+22, -2);
    VoC_lw16i_short(REG5,4,DEFAULT);//loop counter  is 3, this use shift to calculate counter number!
    VoC_lw16i_short(FORMATX,0,IN_PARALLEL);

    VoC_lw16i(RL6_LO, LOCAL_III_imdct_s_Y_ADDR);   //RL6  ->y[0], y[0]+5
    VoC_lw16i(RL6_HI, LOCAL_III_imdct_s_Y_ADDR+10); // +5

    VoC_lw16i_short(FORMAT32,0,DEFAULT);

III_imdcdt_loop_100:

    VoC_loop_i(1,3)
    VoC_lw16i_short(REG4,2,DEFAULT);//loop counter,use shift
III_imdcdt_loop_110:
    VoC_lw32z(ACC0,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_loop_i_short(6,DEFAULT)
    VoC_add32inc_rp(REG67, RL7,REG0,IN_PARALLEL);
    VoC_startloop0

    VoC_multf32_rp(ACC1,REG7,REG1,DEFAULT);
    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_shru32_ri(ACC1,-4,DEFAULT);
    VoC_macXinc_rp(REG6,REG1,IN_PARALLEL);

    VoC_add32inc_rp(REG67, RL7,REG0,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_endloop0

    VoC_movreg32(REG23, RL6,DEFAULT);     //REG23<-y[0], y[0]+5
    VoC_movreg32(RL6,REG23 ,IN_PARALLEL);

    VoC_bez16_r(III_imdcdt_loop_120,REG4);
    VoC_sub32_dr(ACC0,CONST_0_ADDR,ACC0);
III_imdcdt_loop_120:

    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0, REG2,DEFAULT);
    VoC_lw32_sd(REG67,0,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_sw32inc_p(ACC0, REG3,DEFAULT);
    VoC_movreg16(REG0, REG6,IN_PARALLEL);
    VoC_bnez16_r(III_imdcdt_loop_110,REG4);

    VoC_endloop1

    VoC_lw16i_short(ACC0_LO,18,DEFAULT);
    VoC_lw16i_short(ACC0_HI,30,IN_PARALLEL);

    VoC_pop32(REG67,DEFAULT);
    VoC_add16_rd(REG6, REG6,CONST_12_ADDR);

    VoC_movreg16(REG1, REG7,DEFAULT);
    VoC_movreg16(REG0, REG6,IN_PARALLEL);

    VoC_push32(REG67,DEFAULT);
    VoC_shru16_ri(REG5,1,IN_PARALLEL);

    VoC_add32_rr(REG23, REG23, ACC0,DEFAULT);
    VoC_add32_rr(RL6, RL6, ACC0,IN_PARALLEL);

    VoC_bnez16_r(III_imdcdt_loop_100,REG5);

    VoC_lw16i_set_inc(REG1,LOCAL_III_decode_output_ADDR,12);

    /* windowing, overlapping and concatenation */

    VoC_lw16i_set_inc(REG0, LOCAL_III_imdct_s_Y_ADDR, 12); //   REG0 <- yptr = &y[0];
    VoC_lw16i_set_inc(REG2, TABLE_window_s_ADDR, 0);   //REG2 <-  wptr = &window_s[0];
    VoC_lw16i_set_inc(REG3, TABLE_window_s_ADDR+6, 0);  //+6
    VoC_movreg32(RL6, REG01,DEFAULT);   //RL6 <- REG01 <- y[0]+z[0]

    VoC_loop_i(1,6)
    VoC_lw32z(ACC0,DEFAULT);

    VoC_add32inc_rp(REG67, RL7,REG0,DEFAULT);

    VoC_multf32_rp(ACC1,REG7,REG2,DEFAULT);
    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_shru32_ri(ACC1,-4,DEFAULT);
    VoC_macX_rp(REG6,REG2,IN_PARALLEL);
    VoC_add32inc_rp(REG67, RL7,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sw32inc_p(ACC1,REG1,DEFAULT);  // z[i +  6] = mad_f_mul(yptr[ 0 + 0], wptr[0]);
    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_multf32_rp(ACC1,REG7,REG3,IN_PARALLEL);

    VoC_loop_i_short(2,DEFAULT)
    VoC_startloop0
    // MAD_F_ML0(hi, lo, yptr[ 0 + 6], wptr[6]);
    VoC_shru32_ri(ACC1,-4,DEFAULT);
    VoC_add32inc_rp(REG45, RL7,REG0,IN_PARALLEL);


    VoC_multf32_rp(ACC0,REG5,REG2,DEFAULT);
    VoC_NOP();
    VoC_shru16_ri(REG4,12,DEFAULT);

    VoC_shru32_ri(ACC0,-4,DEFAULT);
    VoC_macX_rp(REG6,REG3,IN_PARALLEL);

    // MAD_F_MLA(hi, lo, yptr[12 + 0], wptr[0]);
    VoC_macX_rp(REG4,REG2,DEFAULT);
    VoC_add32inc_rp(REG67, RL7,REG0,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_multf32_rp(ACC1,REG7,REG3,IN_PARALLEL);
    //z[i + 12] = MAD_F_MLZ(hi, lo);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_endloop0

    //MAD_F_ML0(hi, lo, yptr[12 + 6], wptr[6]);
    //MAD_F_MLA(hi, lo, yptr[24 + 0], wptr[0]);
    // z[i + 24] = mad_f_mul(yptr[24 + 6], wptr[6]);

    VoC_shru32_ri(ACC1,-4,DEFAULT);
    VoC_macX_rp(REG6,REG3,IN_PARALLEL);
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);
    VoC_add32_rr(REG23, REG23, REG67,DEFAULT);
    VoC_add32_rr(RL6, RL6, REG67,IN_PARALLEL);
    VoC_sw32inc_p(ACC1,REG1,DEFAULT);
    VoC_add32_rr(RL6, RL6, REG67,IN_PARALLEL);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_movreg32(REG01,RL6,IN_PARALLEL);
    // MAD_F_ML0(hi, lo, yptr[ 0 + 6], wptr[6]);
    VoC_endloop1

    VoC_pop32(REG01,DEFAULT);
    VoC_return
}




/************************************************************
  NAME: III_aliasreduce()
  DESCRIPTION:  perform frequency line alias reduction

  REG0   :  - >      &xr[0] + 18   inc = 1
  REG7  :         lines/18 - 1
*************************************************************/

void CII_III_aliasreduce(void)
{
    VoC_lw16i_short(FORMATX,0,DEFAULT);
    VoC_lw16i_short(FORMAT32,-4,DEFAULT);

    VoC_lw16i_set_inc(REG2, TABLE_cs_ADDR,1);
    VoC_lw16i_set_inc(REG3, TABLE_ca_ADDR,1);

    VoC_lw16i_short(INC1,-2,DEFAULT);
    VoC_lw16i_short(INC0,2,DEFAULT);

    VoC_push32(REG23,DEFAULT);//,"cs_ca"

    VoC_lw32_d(RL6,CONST_0x00000800_ADDR);

    VoC_sub16_rd(REG1,REG0,CONST_2_ADDR);

    VoC_loop_r(1, REG7)

    // b = xr[     i];
    // a = xr[-1 - i];

    VoC_lw32_sd(REG23,0,DEFAULT);   //,"cs_ca"

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_loop_i(0,8)

    VoC_add32_rp(REG67,RL6,REG1,DEFAULT);
    VoC_movreg32(RL7,REG01,IN_PARALLEL);

    VoC_sub32_rp(REG45,RL6,REG0,DEFAULT);
    VoC_add32_rp(REG01,RL6,REG0,IN_PARALLEL);

    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_mac32_rp(REG1,REG2,IN_PARALLEL);

    VoC_shru16_ri(REG4,12,DEFAULT);
    VoC_shru16_ri(REG0,12,IN_PARALLEL);

    VoC_mac32_rp(REG5,REG3,DEFAULT);
    VoC_mac32_rp(REG7,REG3,IN_PARALLEL);

    VoC_macX_rp(REG6,REG2,DEFAULT);
    VoC_macX_rp(REG0,REG2,IN_PARALLEL);

    VoC_macX_rp(REG4,REG3,DEFAULT);
    VoC_macXinc_rp(REG6,REG3,IN_PARALLEL);

    VoC_mac32inc_rp(REG7,REG2,DEFAULT);
    VoC_movreg32(REG01,RL7,IN_PARALLEL);

    VoC_NOP();

    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_endloop0

    VoC_add16_rd(REG0,REG0,CONST_20_ADDR);
    VoC_sub16_rd(REG1,REG0,CONST_2_ADDR);

    VoC_endloop1

    VoC_lw16i_short(FORMAT32,0,DEFAULT);

    VoC_pop32(REG23,DEFAULT);
    VoC_return

}
/*************************************************
 * NAME:     void CII_imdct36 ( void )
 * INPUT:     REG0  --> y[0];  incr = 2
 *             REG4  --> x[0];
 * OUTPUT:
 * DESCRIPTION:
 * USED REGISTER:   All
 *************************************************/

void CII_imdct36(void)
{

#if 0
    // align the local X buffers on a word address
    voc_word LOCAL_III_imdct36_X_16_ADDR ,9, x
    voc_word LOCAL_III_imdct36_X_4_ADDR  ,9, x
    voc_word LOCAL_III_imdct36_t_16_ADDR ,4, x
    voc_word LOCAL_III_imdct36_t_4_ADDR  ,4, x

#endif

    VoC_push16(REG0,DEFAULT);              // stack16[0]  &Y[0]
    VoC_push16(REG4,DEFAULT);              // stack16[1]  &x[0]

    VoC_lw16i_set_inc(REG1,TABLE_imdct36_y_tab_ADDR,1);
    VoC_lw16i_set_inc(REG2,LOCAL_III_imdct36_X_16_ADDR,1);
    VoC_lw16i_set_inc(REG3,LOCAL_III_imdct36_X_4_ADDR,1);

    VoC_add16inc_rp(REG4,REG0,REG1,DEFAULT);
    VoC_add16inc_rp(REG5,REG0,REG1,DEFAULT);

    VoC_lw32_d(RL6,CONST_0x00000800_ADDR);

    VoC_loop_i_short(9,DEFAULT);
    VoC_startloop0

    VoC_add32_rp(REG67,RL6,REG4,DEFAULT);
    VoC_add32_rp(REG45,RL6,REG5,DEFAULT);

    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,IN_PARALLEL);

    VoC_shru16_ri(REG4,12,DEFAULT);
    VoC_sw16inc_p(REG5,REG2,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    VoC_add16inc_rp(REG4,REG0,REG1,IN_PARALLEL);

    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_add16inc_rp(REG5,REG0,REG1,IN_PARALLEL);

    VoC_endloop0

    VoC_lw16i_set_inc(REG1,TABLE_imdct36_y_tab_ADDR + 2,1);
    VoC_NOP();

    VoC_add16inc_rp(REG2,REG0,REG1,DEFAULT);
    VoC_add16inc_rp(REG3,REG0,REG1,DEFAULT);

    VoC_add16inc_rp(REG2,REG0,REG1,DEFAULT);
    VoC_add32_rp(REG45,RL6,REG2,IN_PARALLEL);

    VoC_add16inc_rp(REG3,REG0,REG1,DEFAULT);
    VoC_add32_rp(REG67,RL6,REG3,IN_PARALLEL);

    VoC_lw16i(REG2,LOCAL_III_imdct36_t_16_ADDR);

    VoC_sub32_rp(REG45,REG45,REG2,DEFAULT);         //REG45 : t14 = X[1] - X[10]

    VoC_lw16i(REG3,LOCAL_III_imdct36_t_4_ADDR);

    VoC_shru16_ri(REG4,12,DEFAULT);
    VoC_add32_rp(REG67,REG67,REG3,IN_PARALLEL);         //REG67 : t15 = X[7] + X[16]

    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_sw16inc_p(REG5,REG2,IN_PARALLEL);           //t14_hi -> RAM

    VoC_add16inc_rp(REG4,REG0,REG1,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,IN_PARALLEL);               //t15_hi -> RAM

    VoC_add16inc_rp(REG5,REG0,REG1,DEFAULT);
    VoC_sw16inc_p(REG4,REG3,IN_PARALLEL);               //t14_lo -> RAM

    VoC_sw16inc_p(REG6,REG3,DEFAULT);               //t15_lo -> RAM
    VoC_add16inc_rp(REG4,REG0,REG1,IN_PARALLEL);

    VoC_lw32_p(ACC0,REG4,DEFAULT);                         //ACC0 : X[0]
    VoC_add16inc_rp(REG5,REG0,REG1,IN_PARALLEL);

    VoC_lw32_p(ACC1,REG5,DEFAULT);                         //ACC1 : X[2]
    VoC_add16inc_rp(REG4,REG0,REG1,IN_PARALLEL);

    VoC_lw32_p(RL7,REG4,DEFAULT);                  //RL7 : X[3]
    VoC_add16inc_rp(REG5,REG0,REG1,IN_PARALLEL);

    VoC_lw32_p(REG67,REG5,DEFAULT);                        //REG67 : X[5]
    VoC_add16inc_rp(REG4,REG0,REG1,IN_PARALLEL);

    VoC_sub32_rp(REG67,REG67,REG4,DEFAULT);                //REG67 : X[5] - X[6]
    VoC_add16inc_rp(REG5,REG0,REG1,IN_PARALLEL);

    VoC_sub32_rp(RL7,RL7,REG5,DEFAULT);            //RL7 :   X[3] - X[8]
    VoC_add16inc_rp(REG4,REG0,REG1,IN_PARALLEL);

    VoC_sub32_rp(ACC1,ACC1,REG4,DEFAULT);              //ACC1 :  X[2] - X[9]
    VoC_add16inc_rp(REG5,REG0,REG1,IN_PARALLEL);

    VoC_sub32_rp(ACC0,ACC0,REG5,DEFAULT);              //ACC0 :  X[0] - X[11]
    VoC_add32_rr(ACC1,ACC1,RL6,IN_PARALLEL);           //ACC1 : X[2] - X[9]  - X[14] + 0x00000800

    VoC_sub32_rp(ACC0,ACC0,REG4,DEFAULT);              //ACC0 : t8  = X[0] - X[11] - X[12]
    VoC_add16inc_rp(REG4,REG0,REG1,IN_PARALLEL);

    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);               //ACC0 : X[0] - X[11] - X[12] + 0x00000800
    VoC_sub32_rp(ACC1,ACC1,REG5,IN_PARALLEL);          //ACC1 : t9  = X[2] - X[9]  - X[14]

    VoC_add16inc_rp(REG5,REG0,REG1,DEFAULT);
    VoC_sub32_rp(RL7,RL7,REG4,IN_PARALLEL);            //RL7 :   t10 = X[3] - X[8]  - X[15]

    VoC_sub32_rr(REG45,ACC0,RL7,DEFAULT);          //REG45 : t12 = X[0] - X[3] + X[8] - X[11] - X[12] + X[15] + 0x00000800

    VoC_sub32_rp(REG67,REG67,REG5,DEFAULT);            //REG67 : t11 = X[5] - X[6]  - X[17]
    VoC_shru16_ri(REG4,12,IN_PARALLEL);

    VoC_sw16inc_p(REG5,REG2,DEFAULT);              //t12_hi -> RAM
    VoC_add32_rr(REG67,ACC1,REG67,IN_PARALLEL);        //REG67 : t13 = X[2] + X[5] - X[6] - X[9]  - X[14] - X[17] + 0x00000800

    VoC_sw16inc_p(REG4,REG3,DEFAULT);              //t12_lo -> RAM
    VoC_push32(REG67,IN_PARALLEL);          //stack32[0] <- t11

    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_movreg32(REG45,ACC0,IN_PARALLEL);

    VoC_sw16inc_p(REG7,REG2,DEFAULT);              //t13_hi -> RAM
    VoC_shru16_ri(REG4,12,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG3,DEFAULT);              //t13_lo -> RAM

    VoC_sw16inc_p(REG5,REG2,DEFAULT);              //t8_hi -> RAM
    VoC_movreg32(REG67,ACC1,IN_PARALLEL);

    VoC_sw16inc_p(REG4,REG3,DEFAULT);              //t8_lo -> RAM
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_sw16inc_p(REG7,REG2,DEFAULT);              //t9_hi -> RAM
    VoC_movreg32(REG45,RL7,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG3,DEFAULT);              //t9_lo -> RAM
    VoC_add32_rr(REG45,REG45,RL6,IN_PARALLEL);

    VoC_shru16_ri(REG4,12,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);               //,"t11"stack32[0] -> REG67

    VoC_sw16inc_p(REG5,REG2,DEFAULT);              //t10_hi -> RAM
    VoC_add32_rr(REG67,REG67,RL6,IN_PARALLEL);

    VoC_sw16inc_p(REG4,REG3,DEFAULT);              //t10_lo -> RAM
    VoC_shru16_ri(REG6,12,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,LOCAL_III_imdct36_X_16_ADDR,2);
    VoC_lw16i_set_inc(REG1,LOCAL_III_imdct36_X_4_ADDR,2);
    VoC_lw16i_set_inc(REG3,TABLE_imdct36_tab_ADDR,2);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_sw16_p(REG6,REG3,IN_PARALLEL);             //t11_lo -> RAM

    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,IN_PARALLEL);              //t11_hi -> RAM

    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);

    VoC_lw16i(REG0,LOCAL_III_imdct36_t_16_ADDR);
    VoC_lw16i(REG1,LOCAL_III_imdct36_t_4_ADDR);

    VoC_push32(ACC0,DEFAULT);           //stack32[0] <- t6

    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);

    VoC_inc_p(REG0,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);

    VoC_lw32inc_p(REG45,REG3,DEFAULT);

    VoC_push32(ACC0,DEFAULT);               //stack32[1] <- t0

    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);

    VoC_lw16i(REG2,TABLE_imdct36_x_tab_ADDR);
    VoC_lw16i(REG7,LOCAL_III_decode_output_ADDR);

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_lw16i(REG0,LOCAL_III_imdct36_t_16_ADDR + 4);

    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);
    VoC_sw32_p(ACC0,REG6,IN_PARALLEL);                // x[7]  = MAD_F_MLZ(hi, lo);

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);

    VoC_lw16i(REG1,LOCAL_III_imdct36_t_4_ADDR + 4);

    VoC_sw32_p(ACC1,REG6,DEFAULT);                        // x[10] = -x[7];

    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);

    VoC_lw32_sd(RL7,0,DEFAULT);         //RL7 : t0
    VoC_sub32_rr(ACC0,ACC0,RL7,DEFAULT);
    VoC_add16inc_rp(REG5,REG7,REG2,IN_PARALLEL);
    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);

    VoC_lw16i(REG0,LOCAL_III_imdct36_t_16_ADDR + 2);
    VoC_lw16i(REG1,LOCAL_III_imdct36_t_4_ADDR + 2);

    VoC_sw32_p(ACC0,REG5,DEFAULT);                //x[19] = MAD_F_MLZ(hi, lo) - t0;
    VoC_sw32_p(ACC0,REG6,DEFAULT);                //x[34] = MAD_F_MLZ(hi, lo) - t0;

    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);                //MAD_F_MLA(hi, lo, t13,  MAD_F(0x061f78aa));

    VoC_bimac32inc_rp(REG45,REG0);              //MAD_F_ML0(hi, lo, t12, -MAD_F(0x0ec835e8));
    VoC_bimacXinc_rp(REG45,REG1);               //MAD_F_MLA(hi, lo, t13,  MAD_F(0x061f78aa));

    VoC_lw16i(REG1,LOCAL_III_imdct36_X_4_ADDR + 2);

    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);                      //MAD_F_MLZ(hi, lo) + t0;
    VoC_add16inc_rp(REG5,REG7,REG2,IN_PARALLEL);
    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);

    VoC_lw16i(REG0,LOCAL_III_imdct36_X_16_ADDR + 2);

    VoC_sw32_p(ACC0,REG5,DEFAULT);                           //x[22] = MAD_F_MLZ(hi, lo) + t0;
    VoC_sw32_p(ACC0,REG6,DEFAULT);                           //x[31] = MAD_F_MLZ(hi, lo) + t0;

    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);

    VoC_lw32_sd(RL7,1,DEFAULT);                 //RL7 : t6
    VoC_add32_rr(RL7,RL7,ACC0,DEFAULT);         //RL7 : t1 = MAD_F_MLZ(hi, lo) + t6;

    VoC_loop_i_short(6,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_endloop0

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_add16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL7,IN_PARALLEL);        //[11] = -x[6];

    VoC_lw16i(REG0,LOCAL_III_imdct36_X_16_ADDR + 6);

    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);
    VoC_sw32_p(ACC0,REG6,IN_PARALLEL);

    VoC_lw16i(REG1,LOCAL_III_imdct36_X_4_ADDR + 6);

    VoC_sw32_p(ACC1,REG5,DEFAULT);

    VoC_loop_i_short(6,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_endloop0

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_add16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL7,IN_PARALLEL);

    VoC_lw16i(REG0,LOCAL_III_imdct36_X_16_ADDR + 6);

    VoC_sw32_p(ACC0,REG6,DEFAULT);
    VoC_sw32_p(ACC0,REG5,DEFAULT);

    VoC_lw16i(REG1,LOCAL_III_imdct36_X_4_ADDR + 6);

    VoC_loop_i_short(6,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_endloop0

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_add16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_sub32_rr(ACC0,ACC0,RL7,IN_PARALLEL);

    VoC_lw16i(REG0,LOCAL_III_imdct36_X_16_ADDR );

    VoC_sw32_p(ACC0,REG6,DEFAULT);
    VoC_sw32_p(ACC0,REG5,DEFAULT);

    VoC_lw16i(REG1,LOCAL_III_imdct36_X_4_ADDR );

    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);

    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_movreg32(RL7,ACC0,DEFAULT);            //RL7 : t7 = MAD_F_MLZ(hi, lo);

    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);

    VoC_NOP();

    VoC_loop_i_short(6,DEFAULT);
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);           //RL6 : t2 = MAD_F_MLZ(hi, lo);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_endloop0

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_add16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_sub32_rr(ACC1,ACC1,ACC0,IN_PARALLEL);

    VoC_lw16i(REG0,LOCAL_III_imdct36_X_16_ADDR + 6);

    VoC_sw32_p(ACC0,REG6,DEFAULT);                            //x[5]  = MAD_F_MLZ(hi, lo);
    VoC_sw32_p(ACC1,REG5,DEFAULT);                            //x[12] = -x[5];

    VoC_lw16i(REG1,LOCAL_III_imdct36_X_4_ADDR + 6);

    VoC_loop_i_short(6,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_endloop0

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_add16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL6,IN_PARALLEL);
    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);

    VoC_lw16i(REG0,LOCAL_III_imdct36_X_16_ADDR + 6);

    VoC_sw32_p(ACC0,REG6,DEFAULT);               //x[0]  = MAD_F_MLZ(hi, lo) + t2;
    VoC_sw32_p(ACC1,REG5,DEFAULT);           //x[17] = -x[0];

    VoC_lw16i(REG1,LOCAL_III_imdct36_X_4_ADDR + 6);

    VoC_loop_i_short(6,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_endloop0

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_add16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_add32_rr(ACC1,ACC0,RL6,IN_PARALLEL);

    VoC_lw16i(REG0,LOCAL_III_imdct36_X_16_ADDR + 2);

    VoC_sw32_p(ACC1,REG6,DEFAULT);                       //x[24] = MAD_F_MLZ(hi, lo) + t2;
    VoC_sw32_p(ACC1,REG5,DEFAULT);               //x[29] = MAD_F_MLZ(hi, lo) + t2;

    VoC_lw16i(REG1,LOCAL_III_imdct36_X_4_ADDR + 2);

    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);

    VoC_lw16i(REG0,LOCAL_III_imdct36_X_16_ADDR + 6);

    VoC_add32_rr(RL6,ACC0,RL7,DEFAULT);                          //RL6 : t3 = MAD_F_MLZ(hi, lo) + t7;

    VoC_lw16i(REG1,LOCAL_III_imdct36_X_4_ADDR + 6);

    VoC_loop_i_short(6,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_endloop0

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_add16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL6,IN_PARALLEL);

    VoC_lw16i(REG0,LOCAL_III_imdct36_X_16_ADDR + 6);

    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);
    VoC_sw32_p(ACC0,REG6,IN_PARALLEL);

    VoC_lw16i(REG1,LOCAL_III_imdct36_X_4_ADDR + 6);

    VoC_sw32_p(ACC1,REG5,DEFAULT); //x[9] = -x[8];

    VoC_loop_i_short(6,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_endloop0

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_add16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL6,IN_PARALLEL);

    VoC_lw16i(REG0,LOCAL_III_imdct36_X_16_ADDR + 6);

    VoC_sw32_p(ACC0,REG6,DEFAULT);
    VoC_sw32_p(ACC0,REG5,DEFAULT);

    VoC_lw16i(REG1,LOCAL_III_imdct36_X_4_ADDR + 6);

    VoC_loop_i_short(6,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_endloop0

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_add16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_sub32_rr(ACC0,ACC0,RL6,IN_PARALLEL);

    VoC_lw16i(REG0,LOCAL_III_imdct36_t_16_ADDR);

    VoC_sw32_p(ACC0,REG6,DEFAULT);
    VoC_sw32_p(ACC0,REG5,DEFAULT);

    VoC_lw16i(REG1,LOCAL_III_imdct36_t_4_ADDR);

    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);

    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_sub32_rr(RL6,ACC0,RL7,DEFAULT);            //RL6 : t4 = MAD_F_MLZ(hi, lo) - t7;
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_add16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL6,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);
    VoC_movreg32(RL7,REG01,IN_PARALLEL);

    VoC_sw32_p(ACC0,REG6,DEFAULT);              //x[4]  = MAD_F_MLZ(hi, lo) + t4;
    VoC_sw32_p(ACC1,REG5,DEFAULT);              //x[13] = -x[4];
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_add16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL6,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);
    VoC_movreg32(REG01,RL7,IN_PARALLEL);

    VoC_sw32_p(ACC0,REG6,DEFAULT);          // x[1]  = MAD_F_MLZ(hi, lo) + t4;
    VoC_sw32_p(ACC1,REG5,DEFAULT);      // x[16] = -x[1];
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_add16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL6,IN_PARALLEL);

    VoC_lw16i(REG0,LOCAL_III_imdct36_X_16_ADDR + 2);

    VoC_sw32_p(ACC0,REG6,DEFAULT);                 //x[25] = x[28] = MAD_F_MLZ(hi, lo) + t4;
    VoC_sw32_p(ACC0,REG5,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw16i(REG1,LOCAL_III_imdct36_X_4_ADDR + 2);

    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);

    VoC_pop32(RL6,DEFAULT);                      //,"t0"stack32[0] -> RL6
    VoC_pop32(RL6,DEFAULT);                      //,"t6"stack32[1] -> RL6

    VoC_sub32_rr(RL6,ACC0,RL6,DEFAULT);                 //RL6 : t5 = MAD_F_MLZ(hi, lo) - t6;

    VoC_loop_i_short(6,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_endloop0

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_add16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL6,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);

    VoC_lw16i(REG0,LOCAL_III_imdct36_X_16_ADDR + 6);

    VoC_sw32_p(ACC0,REG6,DEFAULT);                    //x[2]  = MAD_F_MLZ(hi, lo) + t5;
    VoC_sw32_p(ACC1,REG5,DEFAULT);                    //x[15] = -x[2];

    VoC_lw16i(REG1,LOCAL_III_imdct36_X_4_ADDR + 6);

    VoC_loop_i_short(6,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_endloop0

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_add16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL6,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);

    VoC_lw16i(REG0,LOCAL_III_imdct36_X_16_ADDR + 6);

    VoC_sw32_p(ACC0,REG6,DEFAULT);                     //x[3]  = MAD_F_MLZ(hi, lo) + t5;
    VoC_sw32_p(ACC1,REG5,DEFAULT);                    //x[14] = -x[3];

    VoC_lw16i(REG1,LOCAL_III_imdct36_X_4_ADDR + 6);

    VoC_loop_i_short(6,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_bimac32inc_rp(REG45,REG0);
    VoC_bimacXinc_rp(REG45,REG1);
    VoC_endloop0

    VoC_add16inc_rp(REG6,REG7,REG2,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_add16inc_rp(REG5,REG7,REG2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL6,IN_PARALLEL);

    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_pop16(REG4,IN_PARALLEL);         // stack16[1]  &x[0]

    VoC_pop16(REG0,DEFAULT);             // stack16[0]  &Y[0]
    VoC_lw16i_short(INC0,2,DEFAULT);

    VoC_sw32_p(ACC0,REG6,DEFAULT);                  //x[26] = MAD_F_MLZ(hi, lo) + t5;
    VoC_sw32_p(ACC0,REG5,DEFAULT);                  //x[27] = MAD_F_MLZ(hi, lo) + t5;

    VoC_return
}









