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

#ifndef MAP_ADDR_H
#define MAP_ADDR_H

/* This file defines all the variables as memory addresses.
   It's created by voc_map automatically.*/

/********************
 **  SECTION_SIZE  **
 ********************/

#define COMMON_GLOBAL_X_SIZE                                    1034
#define COMMON_GLOBAL_Y_SIZE                                    94
#define AAC_GLOBAL_X_SIZE                                       4126
#define AAC_GLOBAL_Y_SIZE                                       4112
#define ZOOM_GLOBAL_X_SIZE                                      0
#define ZOOM_GLOBAL_Y_SIZE                                      138
#define AAC_LOCAL_X_SIZE                                        4956
#define AAC_LOCAL_Y_SIZE                                        5632
#define ZOOM_LOCAL_X_SIZE                                       4864
#define ZOOM_LOCAL_Y_SIZE                                       2592
#define MP3_GLOBAL_X_SIZE                                       2192
#define MP3_GLOBAL_Y_SIZE                                       7470
#define AMR_GLOBAL_X_SIZE                                       950
#define AMR_GLOBAL_Y_SIZE                                       5606
#define JPEG_GLOBAL_X_SIZE                                      0
#define JPEG_GLOBAL_Y_SIZE                                      1524
#define MP12_GLOBAL_X_SIZE                                      0
#define MP12_GLOBAL_Y_SIZE                                      0
#define MP12_LOCAL_X_SIZE                                       0
#define MP12_LOCAL_Y_SIZE                                       524
#define JPEG_LOCAL_X_SIZE                                       6826
#define JPEG_LOCAL_Y_SIZE                                       1100
#define MP3_LOCAL_X_SIZE                                        6980
#define MP3_LOCAL_Y_SIZE                                        172
#define AMR_LOCAL_X_SIZE                                        578
#define AMR_LOCAL_Y_SIZE                                        240
#define COMMON_LOCAL_X_SIZE                                     0
#define COMMON_LOCAL_Y_SIZE                                     0

/**************************
 **  SECTION_BEGIN_ADDR  **
 **************************/

#define COMMON_GLOBAL_X_BEGIN_ADDR                              (0 + RAM_X_BEGIN_ADDR)
#define COMMON_GLOBAL_Y_BEGIN_ADDR                              (0 + RAM_Y_BEGIN_ADDR)
#define AAC_GLOBAL_X_BEGIN_ADDR                                 (1034 + RAM_X_BEGIN_ADDR)
#define AAC_GLOBAL_Y_BEGIN_ADDR                                 (94 + RAM_Y_BEGIN_ADDR)
#define ZOOM_GLOBAL_X_BEGIN_ADDR                                (5160 + RAM_X_BEGIN_ADDR)
#define ZOOM_GLOBAL_Y_BEGIN_ADDR                                (4206 + RAM_Y_BEGIN_ADDR)
#define AAC_LOCAL_X_BEGIN_ADDR                                  (5160 + RAM_X_BEGIN_ADDR)
#define AAC_LOCAL_Y_BEGIN_ADDR                                  (4344 + RAM_Y_BEGIN_ADDR)
#define ZOOM_LOCAL_X_BEGIN_ADDR                                 (5160 + RAM_X_BEGIN_ADDR)
#define ZOOM_LOCAL_Y_BEGIN_ADDR                                 (4344 + RAM_Y_BEGIN_ADDR)
#define MP3_GLOBAL_X_BEGIN_ADDR                                 (1034 + RAM_X_BEGIN_ADDR)
#define MP3_GLOBAL_Y_BEGIN_ADDR                                 (94 + RAM_Y_BEGIN_ADDR)
#define AMR_GLOBAL_X_BEGIN_ADDR                                 (1034 + RAM_X_BEGIN_ADDR)
#define AMR_GLOBAL_Y_BEGIN_ADDR                                 (94 + RAM_Y_BEGIN_ADDR)
#define JPEG_GLOBAL_X_BEGIN_ADDR                                (3226 + RAM_X_BEGIN_ADDR)
#define JPEG_GLOBAL_Y_BEGIN_ADDR                                (7564 + RAM_Y_BEGIN_ADDR)
#define MP12_GLOBAL_X_BEGIN_ADDR                                (3226 + RAM_X_BEGIN_ADDR)
#define MP12_GLOBAL_Y_BEGIN_ADDR                                (7564 + RAM_Y_BEGIN_ADDR)
#define MP12_LOCAL_X_BEGIN_ADDR                                 (3226 + RAM_X_BEGIN_ADDR)
#define MP12_LOCAL_Y_BEGIN_ADDR                                 (7564 + RAM_Y_BEGIN_ADDR)
#define JPEG_LOCAL_X_BEGIN_ADDR                                 (3226 + RAM_X_BEGIN_ADDR)
#define JPEG_LOCAL_Y_BEGIN_ADDR                                 (9088 + RAM_Y_BEGIN_ADDR)
#define MP3_LOCAL_X_BEGIN_ADDR                                  (3226 + RAM_X_BEGIN_ADDR)
#define MP3_LOCAL_Y_BEGIN_ADDR                                  (9088 + RAM_Y_BEGIN_ADDR)
#define AMR_LOCAL_X_BEGIN_ADDR                                  (3226 + RAM_X_BEGIN_ADDR)
#define AMR_LOCAL_Y_BEGIN_ADDR                                  (9088 + RAM_Y_BEGIN_ADDR)
#define COMMON_LOCAL_X_BEGIN_ADDR                               (5160 + RAM_X_BEGIN_ADDR)
#define COMMON_LOCAL_Y_BEGIN_ADDR                               (9088 + RAM_Y_BEGIN_ADDR)

/*********************
 **  COMMON_GLOBAL  **
 *********************/

//GLOBAL_VARS
#define VPP_AMJP_MODE_NO                                    (0x0)
#define VPP_AMJP_MODE_AMR                                   (0x1)
#define VPP_AMJP_MODE_MP3                                   (0x2)
#define VPP_AMJP_MODE_AAC                                   (0x3)
#define VPP_AMJP_MODE_WMA                                   (0x4)
#define VPP_AMJP_MODE_JPEG                                  (0x100)
#define VPP_AMJP_MODE_ZOOM                                  (0x200)
#define VPP_AMJP_STRM_ID_NO                                 (0x0)
#define VPP_AMJP_STRM_ID_AUDIO                              (0x1)
#define VPP_AMJP_STRM_ID_VIDEO                              (0x2)
#define VPP_AMJP_STRM_ID_INIT                               (0x3)
#define VPP_AMJP_ROTATE_NO                                  (0x0)
#define VPP_AMJP_ROTATE_90                                  (0x1)
#define VPP_AMJP_ROTATE_180                                 (0x2)
#define VPP_AMJP_ROTATE_270                                 (0x3)
#define VPP_AMJP_EQUALIZER_OFF                              (0xffff)
#define VPP_AMJP_EQUALIZER_NORMAL                           (0x0)
#define VPP_AMJP_EQUALIZER_BASS                             (0x1)
#define VPP_AMJP_EQUALIZER_DANCE                            (0x2)
#define VPP_AMJP_EQUALIZER_CLASSICAL                        (0x3)
#define VPP_AMJP_EQUALIZER_TREBLE                           (0x4)
#define VPP_AMJP_EQUALIZER_PARTY                            (0x5)
#define VPP_AMJP_EQUALIZER_POP                              (0x6)
#define VPP_AMJP_EQUALIZER_ROCK                             (0x7)
#define VPP_AMJP_EQUALIZER_CUSTOM                           (0x8)
#define VPP_AMJP_BUF_MODE_CIRC                              (0x0)
#define VPP_AMJP_BUF_MODE_SWAP                              (0x1)
#define GLOBAL_INPUT_DATA_BUFFER                            (0 + COMMON_GLOBAL_X_BEGIN_ADDR) //1024 short
#define GLOBAL_TAB_EQUALIZER_ADDR                           (1024 + COMMON_GLOBAL_X_BEGIN_ADDR) //10 short

//GLOBAL_VARS
//VPP_AMJP_CFG_STRUCT
#define VPP_AMJP_CFG_STRUCT                                 (0 + COMMON_GLOBAL_Y_BEGIN_ADDR)
#define GLOBAL_MODE_IN                                      (0 + VPP_AMJP_CFG_STRUCT) //1 short
#define GLOBAL_RESET                                        (1 + VPP_AMJP_CFG_STRUCT) //1 short
#define GLOBAL_EQ_TYPE                                      (2 + VPP_AMJP_CFG_STRUCT) //1 short
#define GLOBAL_CFG_RESERVED                                 (3 + VPP_AMJP_CFG_STRUCT) //1 short
#define GLOBAL_STRM_BUF_IN_START                            (4 + VPP_AMJP_CFG_STRUCT) //2 short
#define GLOBAL_STRM_BUF_IN_END                              (6 + VPP_AMJP_CFG_STRUCT) //2 short
#define GLOBAL_STRM_BUF_OUT_PTR                             (8 + VPP_AMJP_CFG_STRUCT) //2 short
#define GLOBAL_STRM_BUF_IFC_PTR                             (10 + VPP_AMJP_CFG_STRUCT) //2 short
#define GLOBAL_ZOOM_MODE                                    (12 + VPP_AMJP_CFG_STRUCT) //1 short
#define GLOBAL_ROTATE_MODE                                  (13 + VPP_AMJP_CFG_STRUCT) //1 short
#define GLOBAL_RESIZE_TO_WIDTH                              (14 + VPP_AMJP_CFG_STRUCT) //1 short
#define GLOBAL_VID_BUF_MODE                                 (15 + VPP_AMJP_CFG_STRUCT) //1 short
#define GLOBAL_VID_BUF_IN_START                             (16 + VPP_AMJP_CFG_STRUCT) //2 short
#define GLOBAL_VID_BUF_IN_END                               (18 + VPP_AMJP_CFG_STRUCT) //2 short
#define GLOBAL_VID_BUF_OUT_PTR                              (20 + VPP_AMJP_CFG_STRUCT) //2 short
#define Source_width_ADDR                                   (22 + VPP_AMJP_CFG_STRUCT) //1 short
#define Source_height_ADDR                                  (23 + VPP_AMJP_CFG_STRUCT) //1 short
#define Cut_width_ADDR                                      (24 + VPP_AMJP_CFG_STRUCT) //1 short
#define Cut_height_ADDR                                     (25 + VPP_AMJP_CFG_STRUCT) //1 short
#define Zoomed_width_ADDR                                   (26 + VPP_AMJP_CFG_STRUCT) //1 short
#define Zoomed_height_ADDR                                  (27 + VPP_AMJP_CFG_STRUCT) //1 short
#define INPUT_ADDR_U_ADDR                                   (28 + VPP_AMJP_CFG_STRUCT) //2 short
#define INPUT_ADDR_V_ADDR                                   (30 + VPP_AMJP_CFG_STRUCT) //2 short

//VPP_AMJP_STATUS_STRUCT
#define VPP_AMJP_STATUS_STRUCT                              (32 + COMMON_GLOBAL_Y_BEGIN_ADDR)
#define GLOBAL_MODE_OUT                                     (0 + VPP_AMJP_STATUS_STRUCT) //1 short
#define GLOBAL_ERR_STATUS                                   (1 + VPP_AMJP_STATUS_STRUCT) //1 short
#define GLOBAL_STRM_STATUS                                  (2 + VPP_AMJP_STATUS_STRUCT) //1 short
#define GLOBAL_NB_CHAN                                      (3 + VPP_AMJP_STATUS_STRUCT) //1 short
#define GLOBAL_SAMPLE_RATE                                  (4 + VPP_AMJP_STATUS_STRUCT) //2 short
#define GLOBAL_BIT_RATE                                     (6 + VPP_AMJP_STATUS_STRUCT) //1 short
#define GLOBAL_DISPLAY_BARS_LOG                             (7 + VPP_AMJP_STATUS_STRUCT) //5 short
#define GLOBAL_STRM_BUF_IN_CURR                             (12 + VPP_AMJP_STATUS_STRUCT) //2 short
#define GLOBAL_STRM_BUF_IFC_CURR                            (14 + VPP_AMJP_STATUS_STRUCT) //2 short
#define GLOBAL_IMG_WIDTH                                    (16 + VPP_AMJP_STATUS_STRUCT) //1 short
#define GLOBAL_IMG_HEIGHT                                   (17 + VPP_AMJP_STATUS_STRUCT) //1 short
#define GLOBAL_VID_BUF_IN_CURR                              (18 + VPP_AMJP_STATUS_STRUCT) //2 short

//VPP_AMJP_CODE_CFG_STRUCT
#define VPP_AMJP_CODE_CFG_STRUCT                            (52 + COMMON_GLOBAL_Y_BEGIN_ADDR)
#define GLOBAL_JPEG_CODE_PTR                                (0 + VPP_AMJP_CODE_CFG_STRUCT) //2 short
#define GLOBAL_JPEG_CONST_Y_PTR                             (2 + VPP_AMJP_CODE_CFG_STRUCT) //2 short
#define GLOBAL_MP12_CODE_PTR                                (4 + VPP_AMJP_CODE_CFG_STRUCT) //2 short
#define GLOBAL_MP12_CONST_Y_PTR                             (6 + VPP_AMJP_CODE_CFG_STRUCT) //2 short
#define GLOBAL_MP3_CODE_PTR                                 (8 + VPP_AMJP_CODE_CFG_STRUCT) //2 short
#define GLOBAL_MP3_CONST_X_PTR                              (10 + VPP_AMJP_CODE_CFG_STRUCT) //2 short
#define GLOBAL_MP3_CONST_Y_PTR                              (12 + VPP_AMJP_CODE_CFG_STRUCT) //2 short
#define GLOBAL_AMR_CODE_PTR                                 (14 + VPP_AMJP_CODE_CFG_STRUCT) //2 short
#define GLOBAL_AMR_CONST_Y_PTR                              (16 + VPP_AMJP_CODE_CFG_STRUCT) //2 short
#define GLOBAL_AAC_CODE_PTR                                 (18 + VPP_AMJP_CODE_CFG_STRUCT) //2 short
#define GLOBAL_AAC_CONST_Y_PTR                              (20 + VPP_AMJP_CODE_CFG_STRUCT) //2 short
#define GLOBAL_EQ_TAB_PTR                                   (22 + VPP_AMJP_CODE_CFG_STRUCT) //2 short
#define GLOBAL_ZOOM_CODE_PTR                                (24 + VPP_AMJP_CODE_CFG_STRUCT) //2 short
#define GLOBAL_ZOOM_CONST_Y_PTR                             (26 + VPP_AMJP_CODE_CFG_STRUCT) //2 short

#define GLOBAL_DISPLAY_BARS                                 (80 + COMMON_GLOBAL_Y_BEGIN_ADDR) //10 short
#define GLOBAL_STRM_BUF_IN_PTR                              (90 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_VID_BUF_IN_PTR                               (92 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short

/******************
 **  AAC_GLOBAL  **
 ******************/

//GLOBAL_VARS
//STRUCT_AACDecInfo_addr
#define STRUCT_AACDecInfo_addr                              (0 + AAC_GLOBAL_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_nChans_addr            (0 + STRUCT_AACDecInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_useImpChanMap_addr         (1 + STRUCT_AACDecInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_sampRateIdx_addr           (2 + STRUCT_AACDecInfo_addr) //1 short
#define STRUCT_AACDecInfo_prevBlockID_addr                  (3 + STRUCT_AACDecInfo_addr) //1 short
#define STRUCT_AACDecInfo_currBlockID_addr                  (4 + STRUCT_AACDecInfo_addr) //1 short
#define STRUCT_AACDecInfo_currInstTag_addr                  (5 + STRUCT_AACDecInfo_addr) //1 short
#define STRUCT_AACDecInfo_sbDeinterleaveReqd_addr           (6 + STRUCT_AACDecInfo_addr) //4 short
#define STRUCT_AACDecInfo_adtsBlocksLeft_addr               (10 + STRUCT_AACDecInfo_addr) //1 short
#define STRUCT_AACDecInfo_nChans_addr                       (11 + STRUCT_AACDecInfo_addr) //1 short
#define STRUCT_AACDecInfo_bitRate_addr                      (12 + STRUCT_AACDecInfo_addr) //2 short
#define STRUCT_AACDecInfo_sampRate_addr                     (14 + STRUCT_AACDecInfo_addr) //2 short
#define STRUCT_AACDecInfo_profile_addr                      (16 + STRUCT_AACDecInfo_addr) //1 short
#define STRUCT_AACDecInfo_format_addr                       (17 + STRUCT_AACDecInfo_addr) //1 short
#define STRUCT_AACDecInfo_sbrEnabled_addr                   (18 + STRUCT_AACDecInfo_addr) //1 short
#define STRUCT_AACDecInfo_tnsUsed_addr                      (19 + STRUCT_AACDecInfo_addr) //1 short
#define STRUCT_AACDecInfo_pnsUsed_addr                      (20 + STRUCT_AACDecInfo_addr) //1 short
#define STRUCT_AACDecInfo_frameCount_addr                   (21 + STRUCT_AACDecInfo_addr) //1 short

#define STRUCT_AACDecInfo_PSInfoBase_overlap_addr           (22 + AAC_GLOBAL_X_BEGIN_ADDR) //2048 short
#define STRUCT_AACDecInfo_PSInfoBase_overlap1_addr          (2070 + AAC_GLOBAL_X_BEGIN_ADDR) //2048 short
#define STRUCT_AACDecInfo_PSInfoBase_prevWinShape_addr          (4118 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_prevWinShape1_addr         (4119 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_bitbuffer_addr                               (4120 + AAC_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_bytesLeft_addr                               (4122 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_inputbuffiled_ptr_addr                       (4123 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_inputptr_addr                                (4124 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_BITCACHE_addr                                (4125 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short

//GLOBAL_VARS
//AAC_Dec_ConstY_start
#define AAC_Dec_ConstY_start                                (0 + AAC_GLOBAL_Y_BEGIN_ADDR)
#define Tab_huffTabSpec_addr                                (0 + AAC_Dec_ConstY_start) //1242 short
#define Tab_Cos1Sin1Compressed_addr                         (1242 + AAC_Dec_ConstY_start) //140 short
#define Tab_Cos4Compressed_addr                             (1382 + AAC_Dec_ConstY_start) //98 short
#define Tab_Sin4Compressed_addr                             (1480 + AAC_Dec_ConstY_start) //286 short
#define Tab_SinwShortCompressed_addr                        (1766 + AAC_Dec_ConstY_start) //54 short
#define Tab_SinwLongCompressed_addr                         (1820 + AAC_Dec_ConstY_start) //270 short
#define Tab_KbdwShortCompressed_addr                        (2090 + AAC_Dec_ConstY_start) //92 short
#define Tab_KbdwLongCompressed_addr                         (2182 + AAC_Dec_ConstY_start) //296 short
#define Tab_twidOddCompressed_addr                          (2478 + AAC_Dec_ConstY_start) //392 short
#define Tab_TwidEvenCompressed_addr                         (2870 + AAC_Dec_ConstY_start) //286 short
#define Tab_bitrevtabCompressed_addr                        (3156 + AAC_Dec_ConstY_start) //74 short
#define Tab_invQuant3_addr                                  (3230 + AAC_Dec_ConstY_start) //32 short
#define Tab_invQuant4_addr                                  (3262 + AAC_Dec_ConstY_start) //32 short
#define Tab_sfBandTotal_addr                                (3294 + AAC_Dec_ConstY_start) //12 short
#define Tab_tnsMaxBands_addr                                (3306 + AAC_Dec_ConstY_start) //12 short
#define Tab_pow14_addr                                      (3318 + AAC_Dec_ConstY_start) //8 short
#define Tab_pow43_14_addr                                   (3326 + AAC_Dec_ConstY_start) //128 short
#define Tab_pow43_addr                                      (3454 + AAC_Dec_ConstY_start) //96 short
#define Tab_poly43lo_addr                                   (3550 + AAC_Dec_ConstY_start) //10 short
#define Tab_poly43hi_addr                                   (3560 + AAC_Dec_ConstY_start) //10 short
#define Tab_pow2exp_addr                                    (3570 + AAC_Dec_ConstY_start) //8 short
#define Tab_pow2frac_addr                                   (3578 + AAC_Dec_ConstY_start) //16 short
#define Tab_sfBandTabLongOffset_addr                        (3594 + AAC_Dec_ConstY_start) //12 short
#define Tab_sfBandTabLongCompressed_addr                    (3606 + AAC_Dec_ConstY_start) //118 short
#define Tab_huffTabSpecInfoCompressed_addr                  (3724 + AAC_Dec_ConstY_start) //88 short
#define Tab_sfBandTabShortOffset_addr                       (3812 + AAC_Dec_ConstY_start) //12 short
#define Tab_sfBandTabShort_addr                             (3824 + AAC_Dec_ConstY_start) //76 short
#define Tab_huffTabScaleFact_addr                           (3900 + AAC_Dec_ConstY_start) //122 short
#define Tab_huffTabScaleFactInfo_addr                       (4022 + AAC_Dec_ConstY_start) //22 short
#define Tab_channelMapTab_addr                              (4044 + AAC_Dec_ConstY_start) //8 short
#define Tab_sampRateTab_addr                                (4052 + AAC_Dec_ConstY_start) //24 short
#define Tab_elementNumChans_addr                            (4076 + AAC_Dec_ConstY_start) //8 short
#define const_SQRT1_2_addr                                  (4084 + AAC_Dec_ConstY_start) //2 short
#define const_0x80000000_addr                               (4086 + AAC_Dec_ConstY_start) //2 short
#define const_0x7fffffff_addr                               (4088 + AAC_Dec_ConstY_start) //2 short
#define const_0_addr                                        (4090 + AAC_Dec_ConstY_start) //2 short
#define const_1_addr                                        (4092 + AAC_Dec_ConstY_start) //2 short
#define const_2_addr                                        (4094 + AAC_Dec_ConstY_start) //1 short
#define const_3_addr                                        (4095 + AAC_Dec_ConstY_start) //1 short
#define const_4_addr                                        (4096 + AAC_Dec_ConstY_start) //1 short
#define const_5_addr                                        (4097 + AAC_Dec_ConstY_start) //1 short
#define const_6_addr                                        (4098 + AAC_Dec_ConstY_start) //1 short
#define const_7_addr                                        (4099 + AAC_Dec_ConstY_start) //1 short
#define const_8_addr                                        (4100 + AAC_Dec_ConstY_start) //1 short
#define const_9_addr                                        (4101 + AAC_Dec_ConstY_start) //1 short
#define const_10_addr                                       (4102 + AAC_Dec_ConstY_start) //1 short
#define const_11_addr                                       (4103 + AAC_Dec_ConstY_start) //1 short
#define const_13_addr                                       (4104 + AAC_Dec_ConstY_start) //1 short
#define const_14_addr                                       (4105 + AAC_Dec_ConstY_start) //1 short
#define const_15_addr                                       (4106 + AAC_Dec_ConstY_start) //1 short
#define const_16_addr                                       (4107 + AAC_Dec_ConstY_start) //1 short
#define const_32_addr                                       (4108 + AAC_Dec_ConstY_start) //1 short
#define const_0x0fff_addr                                   (4109 + AAC_Dec_ConstY_start) //1 short
#define AAC_Dec_ConstY_end                                  (4110 + AAC_Dec_ConstY_start) //1 short


/*******************
 **  ZOOM_GLOBAL  **
 *******************/

//GLOBAL_VARS
//ZOOM_TABLE_RAM_Y_STRUCT
#define ZOOM_TABLE_RAM_Y_STRUCT                             (0 + ZOOM_GLOBAL_Y_BEGIN_ADDR)
#define CONST_YUV2RGB_STRUCT_COMPRESSED                     (0 + ZOOM_TABLE_RAM_Y_STRUCT) //138 short


/*****************
 **  AAC_LOCAL  **
 *****************/

//Coolsand_AACDecode
#define Coolsand_AACDecode_X_BEGIN_ADDR                     (0 + AAC_LOCAL_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_coef_addr              (0 + Coolsand_AACDecode_X_BEGIN_ADDR) //2048 short
#define STRUCT_AACDecInfo_PSInfoBase_coef1_addr             (2048 + Coolsand_AACDecode_X_BEGIN_ADDR) //2048 short
#define STEREO_outbuf_addr                                  (STRUCT_AACDecInfo_PSInfoBase_coef_addr)
//STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr            (4096 + Coolsand_AACDecode_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_id_addr         (0 + STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_layer_addr          (1 + STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_protectBit_addr         (2 + STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_profile_addr            (3 + STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_sampRateIdx_addr            (4 + STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_privateBit_addr         (5 + STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_channelConfig_addr          (6 + STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_origCopy_addr           (7 + STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_home_addr           (8 + STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_copyBit_addr            (9 + STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_copyStart_addr          (10 + STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_frameLength_addr            (11 + STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_bufferFull_addr         (12 + STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_numRawDataBlocks_addr           (13 + STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr) //1 short

//STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr           (4110 + Coolsand_AACDecode_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_icsResBit_addr         (0 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_winSequence_addr           (1 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_winShape_addr          (2 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_maxSFB_addr            (3 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_sfGroup_addr           (4 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_predictorDataPresent_addr          (5 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_numWinGroup_addr           (6 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_winGroupLen_addr           (7 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //8 short

//STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr          (4125 + Coolsand_AACDecode_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_icsResBit_addr            (0 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_winSequence_addr          (1 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_winShape_addr         (2 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_maxSFB_addr           (3 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_sfGroup_addr          (4 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_predictorDataPresent_addr         (5 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_numWinGroup_addr          (6 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_winGroupLen_addr          (7 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //8 short

//STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr         (4140 + Coolsand_AACDecode_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo_pulseDataPresent_addr            (0 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo_numPulse_addr            (1 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo_startSFB_addr            (2 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo_offset_addr          (3 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr) //4 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo_amp_addr         (7 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr) //4 short

//STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr            (4151 + Coolsand_AACDecode_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_pulseDataPresent_addr           (0 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_numPulse_addr           (1 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_startSFB_addr           (2 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_offset_addr         (3 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr) //4 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_amp_addr            (7 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr) //4 short

//STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr           (4162 + Coolsand_AACDecode_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_tnsDataPresent_addr            (0 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_numFilt_addr           (1 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_coefRes_addr           (9 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_length_addr            (17 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_order_addr         (25 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_dir_addr           (33 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_coef_addr          (41 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr) //60 short

//STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr          (4263 + Coolsand_AACDecode_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_tnsDataPresent_addr           (0 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_numFilt_addr          (1 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_coefRes_addr          (9 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_length_addr           (17 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_order_addr            (25 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_dir_addr          (33 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_coef_addr         (41 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr) //60 short

//STRUCT_AACDecInfo_PSInfoBase_addr
#define STRUCT_AACDecInfo_PSInfoBase_addr                   (4364 + Coolsand_AACDecode_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_commonWin_addr         (0 + STRUCT_AACDecInfo_PSInfoBase_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_scaleFactors_addr          (1 + STRUCT_AACDecInfo_PSInfoBase_addr) //120 short
#define STRUCT_AACDecInfo_PSInfoBase_scaleFactors1_addr         (121 + STRUCT_AACDecInfo_PSInfoBase_addr) //120 short
#define STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook_addr           (241 + STRUCT_AACDecInfo_PSInfoBase_addr) //120 short
#define STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook1_addr          (361 + STRUCT_AACDecInfo_PSInfoBase_addr) //120 short
#define STRUCT_AACDecInfo_PSInfoBase_msMaskPresent_addr         (481 + STRUCT_AACDecInfo_PSInfoBase_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_msMaskBits_addr            (482 + STRUCT_AACDecInfo_PSInfoBase_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_pnsUsed_addr           (490 + STRUCT_AACDecInfo_PSInfoBase_addr) //2 short
#define STRUCT_AACDecInfo_PSInfoBase_pnsLastVal_addr            (492 + STRUCT_AACDecInfo_PSInfoBase_addr) //2 short
#define STRUCT_AACDecInfo_PSInfoBase_intensityUsed_addr         (494 + STRUCT_AACDecInfo_PSInfoBase_addr) //2 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsLPCBuf_addr         (496 + STRUCT_AACDecInfo_PSInfoBase_addr) //40 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsWorkBuf_addr            (536 + STRUCT_AACDecInfo_PSInfoBase_addr) //40 short
#define STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr         (576 + STRUCT_AACDecInfo_PSInfoBase_addr) //2 short


//Coolsand_AACDecode
#define Coolsand_AACDecode_Y_BEGIN_ADDR                     (0 + AAC_LOCAL_Y_BEGIN_ADDR)
#define Tab_DMA_start_addr                                  (0 + Coolsand_AACDecode_Y_BEGIN_ADDR) //4608 short
#define Tab_cos4sin4tab_addr                                (Tab_DMA_start_addr)
#define Tab_cos1sin1tab_addr                                (Tab_DMA_start_addr+2304)
#define Tab_twidTabOdd_addr                                 (Tab_DMA_start_addr+2304)
#define Tab_twidTabEven_addr                                (Tab_DMA_start_addr+2304)
#define Tab_sinWindow_addr                                  (Tab_DMA_start_addr)
#define Tab_kbdWindow_addr                                  (Tab_DMA_start_addr+2304)
#define Tab_bitrevtab_addr                                  (Tab_DMA_start_addr+2304)
#define Tab_huffTabSpecInfo_addr                            (Tab_DMA_start_addr)
#define Tab_sfBandTabLong_addr                              (Tab_DMA_start_addr+2304)
#define MONO_outbuf_addr                                    (4608 + Coolsand_AACDecode_Y_BEGIN_ADDR) //1024 short

//Coolsand_PNS
#define Coolsand_PNS_X_BEGIN_ADDR                           (4942 + AAC_LOCAL_X_BEGIN_ADDR)
#define LOCAL_PNS_gp_M_maxSFB_addr                          (0 + Coolsand_PNS_X_BEGIN_ADDR) //1 short
#define LOCAL_PNS_CH_addr                                   (1 + Coolsand_PNS_X_BEGIN_ADDR) //1 short
#define LOCAL_PNS_maxSFB_addr                               (2 + Coolsand_PNS_X_BEGIN_ADDR) //1 short
#define LOCAL_PNS_gbMask_addr                               (3 + Coolsand_PNS_X_BEGIN_ADDR) //2 short
#define LOCAL_PNS_numWinGroup_addr                          (5 + Coolsand_PNS_X_BEGIN_ADDR) //1 short
#define LOCAL_PNS_checkCorr_addr                            (6 + Coolsand_PNS_X_BEGIN_ADDR) //1 short
#define LOCAL_PNS_genNew_addr                               (7 + Coolsand_PNS_X_BEGIN_ADDR) //1 short
#define LOCAL_PNS_nSamps_addr                               (8 + Coolsand_PNS_X_BEGIN_ADDR) //1 short
#define LOCAL_PNS_gbCurrent_addr_addr                       (9 + Coolsand_PNS_X_BEGIN_ADDR) //1 short

//Coolsand_TNSFilter
#define Coolsand_TNSFilter_X_BEGIN_ADDR                     (4942 + AAC_LOCAL_X_BEGIN_ADDR)
#define LOCAL_TNSFilter_nWindows_addr                       (0 + Coolsand_TNSFilter_X_BEGIN_ADDR) //1 short
#define LOCAL_TNSFilter_winLen_addr                         (1 + Coolsand_TNSFilter_X_BEGIN_ADDR) //1 short
#define LOCAL_TNSFilter_nSFB_addr                           (2 + Coolsand_TNSFilter_X_BEGIN_ADDR) //1 short
#define LOCAL_TNSFilter_maxOrder_addr                       (3 + Coolsand_TNSFilter_X_BEGIN_ADDR) //1 short
#define LOCAL_TNSFilter_tnsMaxBand_addr                     (4 + Coolsand_TNSFilter_X_BEGIN_ADDR) //1 short
#define LOCAL_TNSFilter_order_addr                          (5 + Coolsand_TNSFilter_X_BEGIN_ADDR) //1 short
#define LOCAL_TNSFilter_gbMask_addr                         (6 + Coolsand_TNSFilter_X_BEGIN_ADDR) //2 short

//Coolsand_DequantBlock
#define Coolsand_DequantBlock_X_BEGIN_ADDR                  (4942 + AAC_LOCAL_X_BEGIN_ADDR)
#define LOCAL_DequantBlock_tab4_addr                        (0 + Coolsand_DequantBlock_X_BEGIN_ADDR) //8 short
#define LOCAL_Dequantize_sfbTab_addr_addr                   (8 + Coolsand_DequantBlock_X_BEGIN_ADDR) //1 short
#define LOCAL_Dequantize_gbCurrent_addr_addr                (9 + Coolsand_DequantBlock_X_BEGIN_ADDR) //1 short
#define LOCAL_Dequantize_numWinGroup_addr                   (10 + Coolsand_DequantBlock_X_BEGIN_ADDR) //1 short
#define LOCAL_Dequantize_maxSFB_addr                        (11 + Coolsand_DequantBlock_X_BEGIN_ADDR) //1 short
#define LOCAL_Dequantize_gbMask_addr                        (12 + Coolsand_DequantBlock_X_BEGIN_ADDR) //2 short

//Coolsand_DecodeSectionData
#define Coolsand_DecodeSectionData_X_BEGIN_ADDR             (4942 + AAC_LOCAL_X_BEGIN_ADDR)
#define LOCAL_DecodeSectionData_maxSFB                      (0 + Coolsand_DecodeSectionData_X_BEGIN_ADDR) //1 short

/******************
 **  ZOOM_LOCAL  **
 ******************/

//vpp_AmjpZoom
#define vpp_AmjpZoom_X_BEGIN_ADDR                           (0 + ZOOM_LOCAL_X_BEGIN_ADDR)
//CONST_YUV2RGB_STRUCT
#define CONST_YUV2RGB_STRUCT                                (0 + vpp_AmjpZoom_X_BEGIN_ADDR)
#define CONST_TabV2R_ADDR                                   (0 + CONST_YUV2RGB_STRUCT) //256 short
#define CONST_TableV2G_ADDR                                 (256 + CONST_YUV2RGB_STRUCT) //256 short
#define CONST_TabU2G_ADDR                                   (512 + CONST_YUV2RGB_STRUCT) //256 short
#define CONST_TabU2B_ADDR                                   (768 + CONST_YUV2RGB_STRUCT) //256 short

#define GLOBAL_TEMP_BUF_ADDR                                (1024 + vpp_AmjpZoom_X_BEGIN_ADDR) //1280 short
#define GLOBAL_TEMP_PTMP2_ADDR                              (GLOBAL_TEMP_BUF_ADDR+640)
#define CONST_ZOOM_1_ADDR                                   (CONST_TableV2G_ADDR+2)
#define CONST_ZOOM_2_ADDR                                   (CONST_TableV2G_ADDR+3)
#define GLOBAL_BUF_IN_ADDR                                  (2304 + vpp_AmjpZoom_X_BEGIN_ADDR) //2560 short

//vpp_AmjpZoom
#define vpp_AmjpZoom_Y_BEGIN_ADDR                           (0 + ZOOM_LOCAL_Y_BEGIN_ADDR)
#define GLOBAL_INLINE_NUM_1_ADDR                            (0 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_INLINE_NUM_2_ADDR                            (1 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define TEMP_ADDRY_ADDR                                     (2 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define TEMP_ADDRU_ADDR                                     (3 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define TEMP_ADDRV_ADDR                                     (4 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_INNER_Y_ADDR                                 (5 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_INNER_U_ADDR                                 (6 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_BUFIN_U_ADDR                                 (7 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_INNER_V_ADDR                                 (8 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_BUFIN_V_ADDR                                 (9 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_SRCW_HALF_ADDR                               (10 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_SRCW_QUA_ADDR                                (11 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_NUM1_ADDR                                    (12 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_NUM3_ADDR                                    (13 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_MASK_ADDR                                    (14 + vpp_AmjpZoom_Y_BEGIN_ADDR) //2 short
#define GLOBAL_DMAOUT_SIZE_ADDR                             (16 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DEAD_OUT_LINE_ADDR                           (17 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DMAOUT_INC_ADDR                              (18 + vpp_AmjpZoom_Y_BEGIN_ADDR) //2 short
#define GLOBAL_INLINE_COUNT_ADDR                            (20 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_INLINE_COUNT_UV_ADDR                         (21 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DMAIN_SIZE_ADDR                              (22 + vpp_AmjpZoom_Y_BEGIN_ADDR) //2 short
#define GLOBAL_DMAIN_SIZE_UV_ADDR                           (24 + vpp_AmjpZoom_Y_BEGIN_ADDR) //2 short
#define GLOBAL_SCALEX_SIZE_ADDR                             (26 + vpp_AmjpZoom_Y_BEGIN_ADDR) //2 short
#define GLOBAL_SCALEY_SIZE_ADDR                             (28 + vpp_AmjpZoom_Y_BEGIN_ADDR) //2 short
#define GLOBAL_P1_ADDR                                      (30 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_P2_ADDR                                      (31 + vpp_AmjpZoom_Y_BEGIN_ADDR) //1 short
#define GLOBAL_BUF_OUT_ADDR                                 (32 + vpp_AmjpZoom_Y_BEGIN_ADDR) //2560 short

/******************
 **  MP3_GLOBAL  **
 ******************/

//GLOBAL_VARS
//MP3_TABLE_RAM_X_STRUCT
#define MP3_TABLE_RAM_X_STRUCT                              (0 + MP3_GLOBAL_X_BEGIN_ADDR)
#define TABLE_hufftab0_ADDR                                 (0 + MP3_TABLE_RAM_X_STRUCT) //2 short
#define TABLE_hufftab1_ADDR                                 (2 + MP3_TABLE_RAM_X_STRUCT) //8 short
#define TABLE_hufftab2_ADDR                                 (10 + MP3_TABLE_RAM_X_STRUCT) //16 short
#define TABLE_hufftab3_ADDR                                 (26 + MP3_TABLE_RAM_X_STRUCT) //16 short
#define TABLE_hufftab5_ADDR                                 (42 + MP3_TABLE_RAM_X_STRUCT) //26 short
#define TABLE_hufftab6_ADDR                                 (68 + MP3_TABLE_RAM_X_STRUCT) //28 short
#define TABLE_hufftab7_ADDR                                 (96 + MP3_TABLE_RAM_X_STRUCT) //64 short
#define TABLE_hufftab8_ADDR                                 (160 + MP3_TABLE_RAM_X_STRUCT) //66 short
#define TABLE_hufftab9_ADDR                                 (226 + MP3_TABLE_RAM_X_STRUCT) //54 short
#define TABLE_hufftab10_ADDR                                (280 + MP3_TABLE_RAM_X_STRUCT) //102 short
#define TABLE_hufftab11_ADDR                                (382 + MP3_TABLE_RAM_X_STRUCT) //104 short
#define TABLE_hufftab12_ADDR                                (486 + MP3_TABLE_RAM_X_STRUCT) //96 short
#define TABLE_hufftab13_ADDR                                (582 + MP3_TABLE_RAM_X_STRUCT) //398 short
#define TABLE_hufftab15_ADDR                                (980 + MP3_TABLE_RAM_X_STRUCT) //380 short
#define TABLE_hufftab16_ADDR                                (1360 + MP3_TABLE_RAM_X_STRUCT) //402 short
#define TABLE_hufftab24_ADDR                                (1762 + MP3_TABLE_RAM_X_STRUCT) //386 short
#define TABLE_hufftabA_ADDR                                 (2148 + MP3_TABLE_RAM_X_STRUCT) //28 short
#define TABLE_hufftabB_ADDR                                 (2176 + MP3_TABLE_RAM_X_STRUCT) //16 short


//GLOBAL_VARS
//MP3_TABLE_RAM_Y_STRUCT
#define MP3_TABLE_RAM_Y_STRUCT                              (0 + MP3_GLOBAL_Y_BEGIN_ADDR)
#define CONST_0x00000800_ADDR                               (0 + MP3_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_neg1_ADDR                                     (2 + MP3_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_0xe0ff_ADDR                                   (3 + MP3_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_1152_ADDR                                     (4 + MP3_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_17_ADDR                                       (5 + MP3_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_576_ADDR                                      (6 + MP3_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_1024_ADDR                                     (7 + MP3_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_0x00001000_ADDR                               (8 + MP3_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_0x100010_ADDR                                 (10 + MP3_TABLE_RAM_Y_STRUCT) //2 short
#define TABLE_imdct36_y_tab_ADDR                            (12 + MP3_TABLE_RAM_Y_STRUCT) //18 short
#define TABLE_D_ADDR                                        (30 + MP3_TABLE_RAM_Y_STRUCT) //544 short
#define TABLE_sflen_table_ADDR                              (574 + MP3_TABLE_RAM_Y_STRUCT) //32 short
#define TABLE_nsfb_table_ADDR                               (606 + MP3_TABLE_RAM_Y_STRUCT) //72 short
#define TABLE_pretab_ADDR                                   (678 + MP3_TABLE_RAM_Y_STRUCT) //22 short
#define TABLE_cs_ADDR                                       (700 + MP3_TABLE_RAM_Y_STRUCT) //8 short
#define TABLE_ca_ADDR                                       (708 + MP3_TABLE_RAM_Y_STRUCT) //8 short
#define TABLE_imdct_s_ADDR                                  (716 + MP3_TABLE_RAM_Y_STRUCT) //36 short
#define TABLE_window_l_ADDR                                 (752 + MP3_TABLE_RAM_Y_STRUCT) //36 short
#define TABLE_window_s_ADDR                                 (788 + MP3_TABLE_RAM_Y_STRUCT) //12 short
#define TABLE_is_table_ADDR                                 (800 + MP3_TABLE_RAM_Y_STRUCT) //8 short
#define TABLE_is_lsf_table_ADDR                             (808 + MP3_TABLE_RAM_Y_STRUCT) //30 short
#define TABLE_bitrate_table_ADDR                            (838 + MP3_TABLE_RAM_Y_STRUCT) //80 short
#define TABLE_imdct36_tab_ADDR                              (918 + MP3_TABLE_RAM_Y_STRUCT) //188 short
#define TABLE_dct32_tab_ADDR                                (1106 + MP3_TABLE_RAM_Y_STRUCT) //32 short
#define TABLE_imdct36_x_tab_ADDR                            (1138 + MP3_TABLE_RAM_Y_STRUCT) //36 short
#define TABLE_sfb_48000_long_ADDR                           (1174 + MP3_TABLE_RAM_Y_STRUCT) //22 short
#define TABLE_sfb_44100_long_ADDR                           (1196 + MP3_TABLE_RAM_Y_STRUCT) //22 short
#define TABLE_sfb_32000_long_ADDR                           (1218 + MP3_TABLE_RAM_Y_STRUCT) //22 short
#define TABLE_sfb_48000_short_ADDR                          (1240 + MP3_TABLE_RAM_Y_STRUCT) //40 short
#define TABLE_sfb_44100_short_ADDR                          (1280 + MP3_TABLE_RAM_Y_STRUCT) //40 short
#define TABLE_sfb_32000_short_ADDR                          (1320 + MP3_TABLE_RAM_Y_STRUCT) //40 short
#define TABLE_sfb_48000_mixed_ADDR                          (1360 + MP3_TABLE_RAM_Y_STRUCT) //38 short
#define TABLE_sfb_44100_mixed_ADDR                          (1398 + MP3_TABLE_RAM_Y_STRUCT) //38 short
#define TABLE_sfb_32000_mixed_ADDR                          (1436 + MP3_TABLE_RAM_Y_STRUCT) //38 short
#define TABLE_sfb_24000_long_ADDR                           (1474 + MP3_TABLE_RAM_Y_STRUCT) //22 short
#define TABLE_sfb_22050_long_ADDR                           (1496 + MP3_TABLE_RAM_Y_STRUCT) //22 short
#define TABLE_sfb_24000_short_ADDR                          (1518 + MP3_TABLE_RAM_Y_STRUCT) //40 short
#define TABLE_sfb_22050_short_ADDR                          (1558 + MP3_TABLE_RAM_Y_STRUCT) //40 short
#define TABLE_sfb_16000_short_ADDR                          (1598 + MP3_TABLE_RAM_Y_STRUCT) //40 short
#define TABLE_sfb_24000_mixed_ADDR                          (1638 + MP3_TABLE_RAM_Y_STRUCT) //36 short
#define TABLE_sfb_22050_mixed_ADDR                          (1674 + MP3_TABLE_RAM_Y_STRUCT) //36 short
#define TABLE_sfb_16000_mixed_ADDR                          (1710 + MP3_TABLE_RAM_Y_STRUCT) //36 short
#define TABLE_sfb_8000_long_ADDR                            (1746 + MP3_TABLE_RAM_Y_STRUCT) //22 short
#define TABLE_sfb_8000_short_ADDR                           (1768 + MP3_TABLE_RAM_Y_STRUCT) //40 short
#define TABLE_sfb_8000_mixed_ADDR                           (1808 + MP3_TABLE_RAM_Y_STRUCT) //40 short
#define TABLE_sfbwidth_table_ADDR                           (1848 + MP3_TABLE_RAM_Y_STRUCT) //28 short
#define TABLE_mad_huff_pair_table_ADDR                      (1876 + MP3_TABLE_RAM_Y_STRUCT) //64 short

#define TABLE_sfb_16000_long_ADDR                           (TABLE_sfb_22050_long_ADDR)
#define TABLE_sfb_12000_long_ADDR                           (TABLE_sfb_16000_long_ADDR)
#define TABLE_sfb_11025_long_ADDR                           (TABLE_sfb_12000_long_ADDR)
#define TABLE_sfb_12000_short_ADDR                          (TABLE_sfb_16000_short_ADDR)
#define TABLE_sfb_11025_short_ADDR                          (TABLE_sfb_12000_short_ADDR)
#define TABLE_sfb_12000_mixed_ADDR                          (TABLE_sfb_16000_mixed_ADDR)
#define TABLE_sfb_11025_mixed_ADDR                          (TABLE_sfb_12000_mixed_ADDR)
#define CONST_2_ADDR                                        ((26 + TABLE_imdct36_x_tab_ADDR))
#define CONST_4_ADDR                                        ((30 + TABLE_imdct36_x_tab_ADDR))
#define CONST_6_ADDR                                        ((32 + TABLE_imdct36_x_tab_ADDR))
#define CONST_8_ADDR                                        ((24 + TABLE_imdct36_x_tab_ADDR))
#define CONST_10_ADDR                                       ((12 + TABLE_imdct36_x_tab_ADDR))
#define CONST_12_ADDR                                       ((6 + TABLE_imdct36_x_tab_ADDR))
#define CONST_14_ADDR                                       ((0 + TABLE_imdct36_x_tab_ADDR))
#define CONST_16_ADDR                                       ((18 + TABLE_imdct36_x_tab_ADDR))
#define CONST_20_ADDR                                       ((1 + TABLE_imdct36_x_tab_ADDR))
#define CONST_28_ADDR                                       ((33 + TABLE_imdct36_x_tab_ADDR))
#define CONST_30_ADDR                                       ((31 + TABLE_imdct36_x_tab_ADDR))
#define CONST_32_ADDR                                       ((27 + TABLE_imdct36_x_tab_ADDR))
#define CONST_36_ADDR                                       ((10 + TABLE_imdct36_x_tab_ADDR))
#define CONST_3_ADDR                                        ((15 + TABLE_nsfb_table_ADDR))
#define CONST_7_ADDR                                        ((14 + TABLE_nsfb_table_ADDR))
#define CONST_15_ADDR                                       ((32+ TABLE_nsfb_table_ADDR))
#define CONST_0_WORD32_ADDR                                 ((26 + TABLE_nsfb_table_ADDR))
#define CONST_0_ADDR                                        (CONST_0_WORD32_ADDR)
#define CONST_0x0010_ADDR                                   (CONST_16_ADDR)
#define CONST_0x0020_ADDR                                   (CONST_32_ADDR)
#define CONST_1_WORD32_ADDR                                 (TABLE_hufftab0_ADDR)
#define CONST_1_ADDR                                        (CONST_1_WORD32_ADDR)
#define CONST_0x0011_ADDR                                   (CONST_17_ADDR)
#define CONST_2048_ADDR                                     (CONST_0x00000800_ADDR)
#define STRUCT_left_dither_error_ADDR                       (1940 + MP3_GLOBAL_Y_BEGIN_ADDR) //6 short
#define STRUCT_right_dither_error_ADDR                      (1946 + MP3_GLOBAL_Y_BEGIN_ADDR) //6 short
//STRUCT_MAD_HEADER_ADDR
#define STRUCT_MAD_HEADER_ADDR                              (1952 + MP3_GLOBAL_Y_BEGIN_ADDR)
#define STRUCT_MAD_HEADER_LAYER_ADDR                        (0 + STRUCT_MAD_HEADER_ADDR) //1 short
#define STRUCT_MAD_HEADER_MODE_ADDR                         (1 + STRUCT_MAD_HEADER_ADDR) //1 short
#define STRUCT_MAD_HEADER_MODE_EXTENSION_ADDR               (2 + STRUCT_MAD_HEADER_ADDR) //1 short
#define STRUCT_MAD_HEADER_EMPHASIS_ADDR                     (3 + STRUCT_MAD_HEADER_ADDR) //1 short
#define STRUCT_MAD_HEADER_BITRATE_ADDR                      (4 + STRUCT_MAD_HEADER_ADDR) //1 short
#define STRUCT_MAD_HEADER_SAMPLERATE_ADDR                   (5 + STRUCT_MAD_HEADER_ADDR) //1 short
#define STRUCT_MAD_HEADER_CRC_CHECK_ADDR                    (6 + STRUCT_MAD_HEADER_ADDR) //1 short
#define STRUCT_MAD_HEADER_CRC_TARGET_ADDR                   (7 + STRUCT_MAD_HEADER_ADDR) //1 short
#define STRUCT_MAD_HEADER_FLAGS_ADDR                        (8 + STRUCT_MAD_HEADER_ADDR) //1 short
#define STRUCT_MAD_HEADER_PRIVATE_BITS_ADDR                 (9 + STRUCT_MAD_HEADER_ADDR) //1 short
#define STRUCT_MAD_TIMER_DURATION_SECONDS_ADDR              (10 + STRUCT_MAD_HEADER_ADDR) //1 short
#define STRUCT_MAD_TIMER_DURATION_FRACTION_ADDR             (11 + STRUCT_MAD_HEADER_ADDR) //1 short
#define STRUCT_MAD_FRAME_OPTIONS_ADDR                       (12 + STRUCT_MAD_HEADER_ADDR) //1 short
#define GLOBAL_MAD_SYNTH_PHASE_ADDR                         (13 + STRUCT_MAD_HEADER_ADDR) //1 short

//STRUCT_MAD_STREAM_ADDR
#define STRUCT_MAD_STREAM_ADDR                              (1966 + MP3_GLOBAL_Y_BEGIN_ADDR)
#define STRUCT_MAD_STREAM_BUFFER_ADDR_ADDR                  (0 + STRUCT_MAD_STREAM_ADDR) //2 short
#define STRUCT_MAD_STREAM_SKIPLEN_ADDR                      (2 + STRUCT_MAD_STREAM_ADDR) //1 short
#define STRUCT_MAD_STREAM_SYNC_ADDR                         (3 + STRUCT_MAD_STREAM_ADDR) //1 short
#define STRUCT_MAD_STREAM_FREERATE_ADDR                     (4 + STRUCT_MAD_STREAM_ADDR) //1 short
#define STRUCT_MAD_STREAM_THIS_FRAME_ADDR_ADDR              (5 + STRUCT_MAD_STREAM_ADDR) //1 short
#define STRUCT_MAD_STREAM_NEXT_FRAME_ADDR_ADDR              (6 + STRUCT_MAD_STREAM_ADDR) //1 short
#define STRUCT_BITPTR_PTR_BYTE_ADDR_ADDR                    (7 + STRUCT_MAD_STREAM_ADDR) //1 short
#define STRUCT_BITPTR_PTR_CACHE_ADDR                        (8 + STRUCT_MAD_STREAM_ADDR) //2 short
#define STRUCT_BITPTR_PTR_LEFT_ADDR                         (10 + STRUCT_MAD_STREAM_ADDR) //1 short
#define STRUCT_BITPTR_ANC_PTR_BYTE_ADDR_ADDR                (11 + STRUCT_MAD_STREAM_ADDR) //1 short
#define STRUCT_BITPTR_ANC_PTR_CACHE_ADDR                    (12 + STRUCT_MAD_STREAM_ADDR) //2 short
#define STRUCT_BITPTR_ANC_PTR_LEFT_ADDR                     (14 + STRUCT_MAD_STREAM_ADDR) //1 short
#define STRUCT_MAD_STREAM_ANC_BITLEN_ADDR                   (15 + STRUCT_MAD_STREAM_ADDR) //1 short
#define STRUCT_MAD_STREAM_MD_LEN_ADDR                       (16 + STRUCT_MAD_STREAM_ADDR) //1 short
#define STRUCT_MAD_STREAM_OPTIONS_ADDR                      (17 + STRUCT_MAD_STREAM_ADDR) //1 short
#define STRUCT_MAD_STREAM_MAD_ERROR_ADDR                    (18 + STRUCT_MAD_STREAM_ADDR) //1 short
#define GLOBLE_WRITE_POINTER_ADDR                           (19 + STRUCT_MAD_STREAM_ADDR) //1 short
#define GLOBAL_NCH_ADDR                                     (20 + STRUCT_MAD_STREAM_ADDR) //1 short
#define GLOBAL_NS_ADDR                                      (21 + STRUCT_MAD_STREAM_ADDR) //1 short
#define GLOBAL_NGR_ADDR                                     (22 + STRUCT_MAD_STREAM_ADDR) //1 short
#define GLOBAL_WRAP_INDEX_ADDR                              (23 + STRUCT_MAD_STREAM_ADDR) //1 short
#define STRUCT_SIDEINFO_ADDR                                (24 + STRUCT_MAD_STREAM_ADDR) //1 short
#define STRUCT_SIDEINFO_main_data_begin_ADDR                (STRUCT_SIDEINFO_ADDR)
#define STRUCT_SIDEINFO_private_bits_ADDR                   (25 + STRUCT_MAD_STREAM_ADDR) //1 short
#define STRUCT_SIDEINFO_scfsi_ADDR                          (26 + STRUCT_MAD_STREAM_ADDR) //2 short

//STRUCT_GRANULE0_BEGIN_ADDR
#define STRUCT_GRANULE0_BEGIN_ADDR                          (1994 + MP3_GLOBAL_Y_BEGIN_ADDR)
#define STRUCT_GRANULE0CH0_part2_3_length_ADDR              (0 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH0_big_values_ADDR                  (1 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH0_global_gain_ADDR                 (2 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH0_scalefac_compress_ADDR           (3 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH0_flags_ADDR                       (4 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH0_block_type_ADDR                  (5 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH0_table_select_ADDR                (6 + STRUCT_GRANULE0_BEGIN_ADDR) //3 short
#define STRUCT_GRANULE0CH0_subblock_gain_ADDR               (9 + STRUCT_GRANULE0_BEGIN_ADDR) //3 short
#define STRUCT_GRANULE0CH0_region0_count_ADDR               (12 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH0_region1_count_ADDR               (13 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH0_scalefac_ADDR                    (14 + STRUCT_GRANULE0_BEGIN_ADDR) //40 short
#define STRUCT_GRANULE0CH1_part2_3_length_ADDR              (54 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH1_big_values_ADDR                  (55 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH1_global_gain_ADDR                 (56 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH1_scalefac_compress_ADDR           (57 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH1_flags_ADDR                       (58 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH1_block_type_ADDR                  (59 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH1_table_select_ADDR                (60 + STRUCT_GRANULE0_BEGIN_ADDR) //3 short
#define STRUCT_GRANULE0CH1_subblock_gain_ADDR               (63 + STRUCT_GRANULE0_BEGIN_ADDR) //3 short
#define STRUCT_GRANULE0CH1_region0_count_ADDR               (66 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH1_region1_count_ADDR               (67 + STRUCT_GRANULE0_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE0CH1_scalefac_ADDR                    (68 + STRUCT_GRANULE0_BEGIN_ADDR) //40 short

//STRUCT_GRANULE1_BEGIN_ADDR
#define STRUCT_GRANULE1_BEGIN_ADDR                          (2102 + MP3_GLOBAL_Y_BEGIN_ADDR)
#define STRUCT_GRANULE1CH0_part2_3_length_ADDR              (0 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH0_big_values_ADDR                  (1 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH0_global_gain_ADDR                 (2 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH0_scalefac_compress_ADDR           (3 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH0_flags_ADDR                       (4 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH0_block_type_ADDR                  (5 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH0_table_select_ADDR                (6 + STRUCT_GRANULE1_BEGIN_ADDR) //3 short
#define STRUCT_GRANULE1CH0_subblock_gain_ADDR               (9 + STRUCT_GRANULE1_BEGIN_ADDR) //3 short
#define STRUCT_GRANULE1CH0_region0_count_ADDR               (12 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH0_region1_count_ADDR               (13 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH0_scalefac_ADDR                    (14 + STRUCT_GRANULE1_BEGIN_ADDR) //40 short
#define STRUCT_GRANULE1CH1_part2_3_length_ADDR              (54 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH1_big_values_ADDR                  (55 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH1_global_gain_ADDR                 (56 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH1_scalefac_compress_ADDR           (57 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH1_flags_ADDR                       (58 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH1_block_type_ADDR                  (59 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH1_table_select_ADDR                (60 + STRUCT_GRANULE1_BEGIN_ADDR) //3 short
#define STRUCT_GRANULE1CH1_subblock_gain_ADDR               (63 + STRUCT_GRANULE1_BEGIN_ADDR) //3 short
#define STRUCT_GRANULE1CH1_region0_count_ADDR               (66 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH1_region1_count_ADDR               (67 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1CH1_scalefac_ADDR                    (68 + STRUCT_GRANULE1_BEGIN_ADDR) //40 short
#define PTR_CACHE_ADDR                                      (108 + STRUCT_GRANULE1_BEGIN_ADDR) //2 short
#define PTR_BYTE_ADDR_ADDR                                  (110 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define PTR_LEFT_ADDR                                       (111 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define mad_layer_III_result                                (112 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define GLOBAL_MAIN_DATA_OFFSET_ADDR                        (113 + STRUCT_GRANULE1_BEGIN_ADDR) //1 short
#define STRUCT_GRANULE1_RESERVED                            (114 + STRUCT_GRANULE1_BEGIN_ADDR) //2 short

#define STATIC_MAD_OVERLAP_ADDR                             (2218 + MP3_GLOBAL_Y_BEGIN_ADDR) //2304 short
#define GLOBAL_MAD_SYNTH_FILTER_ADDR                        (4522 + MP3_GLOBAL_Y_BEGIN_ADDR) //2048 short
#define STATIC_MAD_MAIN_DATA_ADDR                           (6570 + MP3_GLOBAL_Y_BEGIN_ADDR) //900 short

/******************
 **  AMR_GLOBAL  **
 ******************/

//GLOBAL_VARS
//VPP_AMJP_AMR_INOUT_STRUCT
#define VPP_AMJP_AMR_INOUT_STRUCT                           (0 + AMR_GLOBAL_X_BEGIN_ADDR)
#define INPUT_BITS2_ADDR                                    (0 + VPP_AMJP_AMR_INOUT_STRUCT) //8 short
#define INPUT_ACTUAL_ADDR                                   (8 + VPP_AMJP_AMR_INOUT_STRUCT) //48 short
#define OUTPUT_SPEECH_BUFFER2_ADDR                          (56 + VPP_AMJP_AMR_INOUT_STRUCT) //160 short

#define STRUCT_POST_FILTERSTATE_RES2_ADDR                   (216 + AMR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_POST_FILTERSTATE_MEM_SYN_PST_ADDR            (256 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_PREEMPHASISSTATE_ADDR                        (266 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PREEMPHASISSTATE_MEM_PRE_ADDR                (STRUCT_PREEMPHASISSTATE_ADDR)
#define STRUCT_AGCSTATE_ADDR                                (267 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_AGCSTATE_PAST_GAIN_ADDR                      (STRUCT_AGCSTATE_ADDR)
#define STRUCT_POST_FILTERSTATE_SYNTH_BUF_ADDR              (268 + AMR_GLOBAL_X_BEGIN_ADDR) //170 short
#define STRUCT_POST_PROCESSSTATE_ADDR                       (438 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_Y2_HI_ADDR                 (439 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_Y1_LO_ADDR                 (440 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_Y1_HI_ADDR                 (441 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_X0_ADDR                    (442 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_X1_ADDR                    (443 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STURCT_SPEECH_DECODE_FRAMESTATE_PREV_MODE_ADDR          (444 + AMR_GLOBAL_X_BEGIN_ADDR) //8 short
#define TX_SP_FLAG_ADDR                                     (452 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_DECOD_AMRSTATE_OLD_EXC_ADDR                  (454 + AMR_GLOBAL_X_BEGIN_ADDR) //154 short
#define STRUCT_DECOD_AMRSTATE_EXC_ADDR                      (608 + AMR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_DECOD_AMRSTATE_LSP_OLD_ADDR                  (648 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DECOD_AMRSTATE_MEM_SYN_ADDR                  (658 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DECOD_AMRSTATE_SHARP_ADDR                    (668 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR                   (669 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR                  (670 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_PREV_PDF_ADDR                 (671 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_STATE_ADDR                    (672 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_T0_LAGBUFF_ADDR               (673 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_EXCENERGYHIST_ADDR            (674 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DECOD_AMRSTATE_INBACKGROUNDNOISE_ADDR            (684 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_VOICEDHANGOVER_ADDR           (685 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_LTPGAINHISTORY_ADDR           (686 + AMR_GLOBAL_X_BEGIN_ADDR) //9 short
#define STRUCT_DECOD_AMRSTATE_NODATASEED_ADDR               (695 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_BGN_SCDSTATE_ADDR                            (696 + AMR_GLOBAL_X_BEGIN_ADDR) //60 short
#define STRUCT_BGN_SCDSTATE_FRAMEENERGYHIST_ADDR            (STRUCT_BGN_SCDSTATE_ADDR)
#define STRUCT_BGN_SCDSTATE_BGHANGOVER_ADDR                 (756 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_CB_GAIN_AVERAGESTATE_ADDR                    (758 + AMR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STRUCT_CB_GAIN_AVERAGESTATE_CBGAINHISTORY_ADDR          (STRUCT_CB_GAIN_AVERAGESTATE_ADDR)
#define STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR          (766 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_CB_GAIN_AVERAGESTATE_HANGVAR_ADDR            (767 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_LSP_AVGSTATE_ADDR                            (768 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_LSP_AVGSTATE_LSP_MEANSAVE_ADDR               (STRUCT_LSP_AVGSTATE_ADDR)
#define STRUCT_D_PLSFSTATE_ADDR                             (778 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR                  (STRUCT_D_PLSFSTATE_ADDR)
#define STRUCT_D_PLSFSTATE_PAST_R_Q_ADDR                    (788 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_EC_GAIN_PITCHSTATE_ADDR                      (798 + AMR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_EC_GAIN_PITCHSTATE_PBUF_ADDR                 (STRUCT_EC_GAIN_PITCHSTATE_ADDR)
#define STRUCT_EC_GAIN_PITCHSTATE_PAST_GAIN_PIT_ADDR            (804 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_EC_GAIN_PITCHSTATE_PREV_GP_ADDR              (805 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_EC_GAIN_CODESTATE_ADDR                       (806 + AMR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_EC_GAIN_CODESTATE_GBUF_ADDR                  (STRUCT_EC_GAIN_CODESTATE_ADDR)
#define STRUCT_EC_GAIN_CODESTATE_PAST_GAIN_CODE_ADDR            (812 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_EC_GAIN_CODESTATE_PREV_GC_ADDR               (813 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GC_PREDSTATE_ADDR                            (814 + AMR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR                (STRUCT_GC_PREDSTATE_ADDR)
#define STRUCT_GC_PREDSTATE_PAST_QUA_EN_MR122_ADDR          (818 + AMR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_PH_DISPSTATE_ADDR                            (822 + AMR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_PH_DISPSTATE_GAINMEM_ADDR                    (STRUCT_PH_DISPSTATE_ADDR)
#define STRUCT_PH_DISPSTATE_PREVSTATE_ADDR                  (828 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PH_DISPSTATE_PREVCBGAIN_ADDR                 (829 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PH_DISPSTATE_LOCKFULL_ADDR                   (830 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PH_DISPSTATE_ONSET_ADDR                      (831 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_ADDR                            (832 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_SINCE_LAST_SID_ADDR             (STRUCT_DTX_DECSTATE_ADDR)
#define STRUCT_DTX_DECSTATE_TRUE_SID_PERIOD_INV_ADDR            (833 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
//STRUCT_AMR_DEC_INPUT_ADDR
#define STRUCT_AMR_DEC_INPUT_ADDR                           (834 + AMR_GLOBAL_X_BEGIN_ADDR)
#define STRUCT_AMR_DEC_GLOBAL_MODE                          (0 + STRUCT_AMR_DEC_INPUT_ADDR) //1 short
#define STRUCT_AMR_DEC_GLOBAL_RESET                         (1 + STRUCT_AMR_DEC_INPUT_ADDR) //1 short
#define STRUCT_AMR_DEC_INBUF_ADDR                           (2 + STRUCT_AMR_DEC_INPUT_ADDR) //2 short
#define STRUCT_AMR_DEC_OUTBUF_ADDR                          (4 + STRUCT_AMR_DEC_INPUT_ADDR) //2 short

#define STRUCT_DTX_DECSTATE_LSP_OLD_ADDR                    (840 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DTX_DECSTATE_LSF_HIST_ADDR                   (850 + AMR_GLOBAL_X_BEGIN_ADDR) //80 short
#define STRUCT_DTX_DECSTATE_LSF_HIST_PTR_ADDR               (930 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_LOG_PG_MEAN_ADDR                (931 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_LOG_EN_HIST_ADDR                (932 + AMR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STRUCT_DTX_DECSTATE_LOG_EN_HIST_PTR_ADDR            (940 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_LOG_EN_ADJUST_ADDR              (941 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DTXHANGOVERCOUNT_ADDR           (942 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DECANAELAPSEDCOUNT_ADDR         (943 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_SID_FRAME_ADDR                  (944 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_VALID_DATA_ADDR                 (945 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DTXHANGOVERADDED_ADDR           (946 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DTXGLOBALSTATE_ADDR             (947 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DATA_UPDATED_ADDR               (948 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short

//GLOBAL_VARS
//AMR_TABLE_RAM_Y_STRUCT
#define AMR_TABLE_RAM_Y_STRUCT                              (0 + AMR_GLOBAL_Y_BEGIN_ADDR)
#define CONST_AMR_0x1fff_ADDR                               (0 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_5462_ADDR                                 (1 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_0x3fff_ADDR                               (2 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_0x0800_ADDR                               (3 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_21299_ADDR                                (4 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_5443_ADDR                                 (5 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_112_ADDR                                  (6 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_105_ADDR                                  (7 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_197_ADDR                                  (8 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_31128_ADDR                                (9 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_134_ADDR                                  (10 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_0x177_ADDR                                (11 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_102_ADDR                                  (12 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_3277_ADDR                                 (13 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_0x87_ADDR                                 (14 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_NEG11_ADDR                                (15 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_31821_ADDR                                (16 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_29491_ADDR                                (17 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_9830_ADDR                                 (18 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_14746_ADDR                                (19 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_17578_ADDR                                (20 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_16384_ADDR                                (21 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_256_ADDR                                  (22 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_160_ADDR                                  (23 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_143_ADDR                                  (24 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_26214_ADDR                                (25 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_4096_ADDR                                 (26 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_64_ADDR                                   (27 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_128_ADDR                                  (28 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_205_ADDR                                  (29 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_50_ADDR                                   (30 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_44_ADDR                                   (31 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_2_ADDR                                    (32 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_3_ADDR                                    (33 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_4_ADDR                                    (34 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_6_ADDR                                    (35 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_7_ADDR                                    (36 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_8_ADDR                                    (37 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_9_ADDR                                    (38 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_10_ADDR                                   (39 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_12_ADDR                                   (40 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_16_ADDR                                   (41 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_17_ADDR                                   (42 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_18_ADDR                                   (43 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_20_ADDR                                   (44 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_22_ADDR                                   (45 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_25_ADDR                                   (46 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_30_ADDR                                   (47 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_31_ADDR                                   (48 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_32_ADDR                                   (49 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_33_ADDR                                   (50 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_40_ADDR                                   (51 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_41_ADDR                                   (52 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_15_ADDR                                   (53 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_11_ADDR                                   (54 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_3276_ADDR                                 (55 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_0x2000_ADDR                               (56 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_0x2666_ADDR                               (57 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_neg2_ADDR                                 (58 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_80_ADDR                                   (59 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_512_ADDR                                  (60 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_1024_ADDR                                 (61 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_0x199a_ADDR                               (62 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_120_ADDR                                  (63 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_124_ADDR                                  (64 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_1311_ADDR                                 (65 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_368_ADDR                                  (66 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_463_ADDR                                  (67 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_9000_ADDR                                 (68 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_5325_ADDR                                 (69 + AMR_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_AMR_0_ADDR                                    (70 + AMR_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_AMR_1_ADDR                                    (72 + AMR_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_AMR_0x40000000_ADDR                           (74 + AMR_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_AMR_0x7FFFFFFF_ADDR                           (76 + AMR_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_AMR_0x70816958_ADDR                           (78 + AMR_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_AMR_0x00010001_ADDR                           (80 + AMR_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_AMR_0x00020002_ADDR                           (82 + AMR_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_AMR_0x00080008_ADDR                           (84 + AMR_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_AMR_0xFFFFFFFF_ADDR                           (86 + AMR_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_AMR_0x00004000L_ADDR                          (88 + AMR_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_AMR_0x00007FFF_ADDR                           (90 + AMR_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_AMR_0x00008000_ADDR                           (92 + AMR_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_AMR_0x0000FFFF_ADDR                           (94 + AMR_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_AMR_0x00000005L_ADDR                          (96 + AMR_TABLE_RAM_Y_STRUCT) //2 short
#define CONST_AMR_0x40000000L_ADDR                          (CONST_AMR_0x40000000_ADDR)
#define CONST_AMR_0x7fffffff_ADDR                           (CONST_AMR_0x7FFFFFFF_ADDR)
#define CONST_AMR_WORD32_0_ADDR                             (CONST_AMR_0_ADDR)
#define CONST_AMR_0x00000001_ADDR                           (CONST_AMR_1_ADDR)
#define CONST_AMR_0x7FFF_ADDR                               (CONST_AMR_0x00007FFF_ADDR)
#define CONST_AMR_0xFFFF_ADDR                               (CONST_AMR_0x0000FFFF_ADDR)
#define CONST_AMR_0x4000_ADDR                               (CONST_AMR_0x00004000L_ADDR)
#define CONST_AMR_5_ADDR                                    (CONST_AMR_0x00000005L_ADDR)
#define CONST_AMR_0X80008_ADDR                              (CONST_AMR_0x00080008_ADDR)
#define CONST_AMR_0x00008000L_ADDR                          (CONST_AMR_0x00008000_ADDR)
#define CONST_AMR_0x7fff_ADDR                               (CONST_AMR_0x7FFF_ADDR)
#define CONST_AMR_0x400_ADDR                                (CONST_AMR_1024_ADDR)
#define CONST_AMR_0x80008_ADDR                              (CONST_AMR_0x00080008_ADDR)
#define CONST_AMR_6554_ADDR                                 (CONST_AMR_0x199a_ADDR)
#define TABLE_MEAN_LSF_ADDR                                 (98 + AMR_TABLE_RAM_Y_STRUCT) //10 short
#define TABLE_SLOPE_ADDR                                    (108 + AMR_TABLE_RAM_Y_STRUCT) //64 short
#define TABLE_LSP_LSF_ADDR                                  (172 + AMR_TABLE_RAM_Y_STRUCT) //66 short
#define TABLE_LOG2_ADDR                                     (238 + AMR_TABLE_RAM_Y_STRUCT) //34 short
#define TABLE_INV_SQRT_ADDR                                 (272 + AMR_TABLE_RAM_Y_STRUCT) //50 short
#define TABLE_POW2_ADDR                                     (322 + AMR_TABLE_RAM_Y_STRUCT) //34 short
#define STATIC_CONST_DHF_MASK_ADDR                          (356 + AMR_TABLE_RAM_Y_STRUCT) //58 short
#define STATIC_CONST_QUA_GAIN_PITCH_ADDR                    (414 + AMR_TABLE_RAM_Y_STRUCT) //16 short
#define STATIC_CONST_F_GAMMA4_ADDR                          (430 + AMR_TABLE_RAM_Y_STRUCT) //10 short
#define STATIC_CONST_F_GAMMA3_ADDR                          (440 + AMR_TABLE_RAM_Y_STRUCT) //10 short
#define STATIC_CONST_INTER_6_ADDR                           (450 + AMR_TABLE_RAM_Y_STRUCT) //62 short
#define STATIC_CONST_DGRAY_ADDR                             (512 + AMR_TABLE_RAM_Y_STRUCT) //8 short
#define TABLE_DICO1_LSF_ADDR                                (520 + AMR_TABLE_RAM_Y_STRUCT) //512 short
#define TABLE_DICO2_LSF_ADDR                                (1032 + AMR_TABLE_RAM_Y_STRUCT) //1024 short
#define TABLE_DICO3_LSF_ADDR                                (2056 + AMR_TABLE_RAM_Y_STRUCT) //1024 short
#define TABLE_DICO4_LSF_ADDR                                (3080 + AMR_TABLE_RAM_Y_STRUCT) //1024 short
#define TABLE_DICO5_LSF_ADDR                                (4104 + AMR_TABLE_RAM_Y_STRUCT) //256 short
#define STATIC_CONST_BITNO_ADDR                             (4360 + AMR_TABLE_RAM_Y_STRUCT) //30 short
#define STATIC_CONST_PRMNO_ADDR                             (4390 + AMR_TABLE_RAM_Y_STRUCT) //10 short
#define STATIC_CONST_PRMNOFSF_ADDR                          (4400 + AMR_TABLE_RAM_Y_STRUCT) //8 short
#define STATIC_CONST_bitno_MR475_ADDR                       (4408 + AMR_TABLE_RAM_Y_STRUCT) //9 short
#define STATIC_CONST_bitno_MR515_ADDR                       (4417 + AMR_TABLE_RAM_Y_STRUCT) //10 short
#define STATIC_CONST_bitno_MR59_ADDR                        (4427 + AMR_TABLE_RAM_Y_STRUCT) //10 short
#define STATIC_CONST_bitno_MR67_ADDR                        (4437 + AMR_TABLE_RAM_Y_STRUCT) //10 short
#define STATIC_CONST_bitno_MR74_ADDR                        (4447 + AMR_TABLE_RAM_Y_STRUCT) //10 short
#define STATIC_CONST_bitno_MR795_ADDR                       (4457 + AMR_TABLE_RAM_Y_STRUCT) //12 short
#define STATIC_CONST_bitno_MR102_ADDR                       (4469 + AMR_TABLE_RAM_Y_STRUCT) //20 short
#define STATIC_CONST_bitno_MRDTX_ADDR                       (4489 + AMR_TABLE_RAM_Y_STRUCT) //3 short
#define STATIC_CONST_BITNO_AMR_ADDR                         (4492 + AMR_TABLE_RAM_Y_STRUCT) //10 short
#define STATIC_CONST_startPos_ADDR                          (4502 + AMR_TABLE_RAM_Y_STRUCT) //16 short
#define STATIC_CONST_dhf_MR475_ADDR                         (4518 + AMR_TABLE_RAM_Y_STRUCT) //18 short
#define STATIC_CONST_dhf_MR515_ADDR                         (4536 + AMR_TABLE_RAM_Y_STRUCT) //20 short
#define STATIC_CONST_dhf_MR59_ADDR                          (4556 + AMR_TABLE_RAM_Y_STRUCT) //20 short
#define STATIC_CONST_dhf_MR67_ADDR                          (4576 + AMR_TABLE_RAM_Y_STRUCT) //20 short
#define STATIC_CONST_dhf_MR74_ADDR                          (4596 + AMR_TABLE_RAM_Y_STRUCT) //20 short
#define STATIC_CONST_dhf_MR795_ADDR                         (4616 + AMR_TABLE_RAM_Y_STRUCT) //24 short
#define STATIC_CONST_dhf_MR102_ADDR                         (4640 + AMR_TABLE_RAM_Y_STRUCT) //40 short
#define STATIC_CONST_dhf_MR122_ADDR                         (4680 + AMR_TABLE_RAM_Y_STRUCT) //58 short
#define STATIC_CONST_dhf_amr_ADDR                           (4738 + AMR_TABLE_RAM_Y_STRUCT) //8 short
#define STATIC_CONST_qua_gain_code_ADDR                     (4746 + AMR_TABLE_RAM_Y_STRUCT) //96 short
#define STATIC_CONST_lsp_init_data_ADDR                     (4842 + AMR_TABLE_RAM_Y_STRUCT) //10 short
#define STATIC_past_rq_init_ADDR                            (4852 + AMR_TABLE_RAM_Y_STRUCT) //80 short
#define TABLE_SQRT_L_ADDR                                   (4932 + AMR_TABLE_RAM_Y_STRUCT) //50 short
#define STATIC_CONST_WIND_200_40_ADDR                       (4982 + AMR_TABLE_RAM_Y_STRUCT) //240 short
#define STATIC_trackTable_ADDR                              (5222 + AMR_TABLE_RAM_Y_STRUCT) //20 short
#define STATIC_CONST_lsf_hist_mean_scale_ADDR               (5242 + AMR_TABLE_RAM_Y_STRUCT) //10 short
#define STATIC_CONST_dtx_log_en_adjust_ADDR                 (5252 + AMR_TABLE_RAM_Y_STRUCT) //10 short
#define STATIC_CONST_pred_ADDR                              (5262 + AMR_TABLE_RAM_Y_STRUCT) //4 short
#define STATIC_CONST_pred_MR122_ADDR                        (5266 + AMR_TABLE_RAM_Y_STRUCT) //4 short
#define STATIC_CONST_b_60Hz_ADDR                            (5270 + AMR_TABLE_RAM_Y_STRUCT) //4 short
#define STATIC_CONST_a_60Hz_ADDR                            (5274 + AMR_TABLE_RAM_Y_STRUCT) //4 short
#define STATIC_CONST_gamma3_ADDR                            (5278 + AMR_TABLE_RAM_Y_STRUCT) //10 short
#define AMR_RESET_VALUE_TABLE_ADDR                          (5288 + AMR_TABLE_RAM_Y_STRUCT) //48 short

#define CONST_TAB244_ADDR                                   (5336 + AMR_GLOBAL_Y_BEGIN_ADDR) //244 short
#define CONST_HEAD_RING                                     (5580 + AMR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_RELOAD_MODE_ADDR                             (5582 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RESET_FLAG_ADDR                              (5583 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_AMR_RESET_ADDR                               (5584 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DEC_RESET_DONE_ADDR                          (5585 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RELOAD_RESET_FLAG_ADDR                       (5586 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_BFI_ADDR                                     (5587 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_UFI_ADDR                                     (5588 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_SID_ADDR                                     (5589 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_TAF_ADDR                                     (5590 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DEC_MODE_ADDR                                (5591 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ENC_MODE_ADDR                                (5592 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ENC_USED_MODE_ADDR                           (5593 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_RESET_FLAG_ADDR                                 (5594 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_MAIN_RESET_FLAG_OLD_ADDR                        (5595 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_AMR_FRAME_TYPE_ADDR                             (5596 + AMR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define DEC_INPUT_ADDR_ADDR                                 (5598 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_OUTPUT_ADDR_ADDR                                (5599 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_INOUT_ADDR_BAK_ADDR                             (5600 + AMR_GLOBAL_Y_BEGIN_ADDR) //2 short
//GLOBAL_OUTPUT_STRUCT_ADDR
#define GLOBAL_OUTPUT_STRUCT_ADDR                           (5602 + AMR_GLOBAL_Y_BEGIN_ADDR)
#define STRUCT_AMR_DEC_mode_ADDR                            (0 + GLOBAL_OUTPUT_STRUCT_ADDR) //1 short
#define STRUCT_AMR_DEC_CONSUME_ADDR                         (1 + GLOBAL_OUTPUT_STRUCT_ADDR) //1 short
#define STRUCT_AMR_DEC_length_ADDR                          (2 + GLOBAL_OUTPUT_STRUCT_ADDR) //1 short


/*******************
 **  JPEG_GLOBAL  **
 *******************/

//GLOBAL_VARS
//JPEG_TABLE_RAM_Y_STRUCT
#define JPEG_TABLE_RAM_Y_STRUCT                             (0 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define CONST_JPEG_DEC_18_ADDR                              (0 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_367_ADDR                             (1 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_159_ADDR                             (2 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_220_ADDR                             (3 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_411_ADDR                             (4 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_NEG_29_ADDR                          (5 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_1108_ADDR                            (6 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_3784_ADDR                            (7 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_1568_ADDR                            (8 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_565_ADDR                             (9 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_2276_ADDR                            (10 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_3406_ADDR                            (11 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_2408_ADDR                            (12 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_799_ADDR                             (13 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_4017_ADDR                            (14 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_181_ADDR                             (15 + JPEG_TABLE_RAM_Y_STRUCT) //1 short
#define CONST_JPEG_DEC_0x00000040_ADDR                      (16 + JPEG_TABLE_RAM_Y_STRUCT) //2 short

//JPEG_Long_Constant
#define JPEG_Long_Constant                                  (18 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define CONST_JPEG_DEC_0xFFFFFFFF_ADDR                      (0 + JPEG_Long_Constant) //2 short
#define CONST_JPEG_DEC_0x00000080_ADDR                      (2 + JPEG_Long_Constant) //2 short
#define CONST_JPEG_DEC_0x00002000_ADDR                      (4 + JPEG_Long_Constant) //2 short
#define CONST_JPEG_DEC_0x0000ffff_ADDR                      (6 + JPEG_Long_Constant) //2 short
#define CONST_JPEG_DEC_0x0000ff80_ADDR                      (8 + JPEG_Long_Constant) //2 short
#define CONST_JPEG_DEC_0x0000fd80_ADDR                      (10 + JPEG_Long_Constant) //2 short
#define CONST_JPEG_DEC_0x0000e800_ADDR                      (12 + JPEG_Long_Constant) //2 short
#define CONST_JPEG_DEC_0x0000e000_ADDR                      (14 + JPEG_Long_Constant) //2 short

//TABLE_DClumtab
#define TABLE_DClumtab                                      (34 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_DClumtab0_ADDR                                (0 + TABLE_DClumtab) //16 short
#define TABLE_DClumtab1_ADDR                                (16 + TABLE_DClumtab) //16 short

//TABLE_DCchromtab
#define TABLE_DCchromtab                                    (66 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_DCchromtab0_ADDR                              (0 + TABLE_DCchromtab) //32 short
#define TABLE_DCchromtab1_ADDR                              (32 + TABLE_DCchromtab) //32 short

//TABLE_AClumtab
#define TABLE_AClumtab                                      (130 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_AClumtab0_ADDR                                (0 + TABLE_AClumtab) //64 short
#define TABLE_AClumtab1_ADDR                                (64 + TABLE_AClumtab) //96 short
#define TABLE_AClumtab2_ADDR                                (160 + TABLE_AClumtab) //64 short
#define TABLE_AClumtab3_ADDR                                (224 + TABLE_AClumtab) //256 short

//TABLE_ACchromtab
#define TABLE_ACchromtab                                    (610 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_ACchromtab0_ADDR                              (0 + TABLE_ACchromtab) //64 short
#define TABLE_ACchromtab1_ADDR                              (64 + TABLE_ACchromtab) //128 short
#define TABLE_ACchromtab2_ADDR                              (192 + TABLE_ACchromtab) //64 short
#define TABLE_ACchromtab3_ADDR                              (256 + TABLE_ACchromtab) //256 short

//TABLE_OtherConstant
#define TABLE_OtherConstant                                 (1122 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_inverse_Zig_Zag_ADDR                          (0 + TABLE_OtherConstant) //64 short
#define TABLE_add_para_ADDR                                 (64 + TABLE_OtherConstant) //320 short

//TABLE_AClumcodetab
#define TABLE_AClumcodetab                                  (1506 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_AClumcodetab0_ADDR                            (0 + TABLE_AClumcodetab) //2 short
#define TABLE_AClumcodetab1_ADDR                            (2 + TABLE_AClumcodetab) //2 short
#define TABLE_AClumcodetab2_ADDR                            (4 + TABLE_AClumcodetab) //2 short
#define TABLE_AClumcodetab3_ADDR                            (6 + TABLE_AClumcodetab) //2 short

//TABLE_ACchromcodetab
#define TABLE_ACchromcodetab                                (1514 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_ACchromcodetab0_ADDR                          (0 + TABLE_ACchromcodetab) //2 short
#define TABLE_ACchromcodetab1_ADDR                          (2 + TABLE_ACchromcodetab) //2 short
#define TABLE_ACchromcodetab2_ADDR                          (4 + TABLE_ACchromcodetab) //2 short
#define TABLE_ACchromcodetab3_ADDR                          (6 + TABLE_ACchromcodetab) //2 short

//TABLE_const_jpeg_ptrs
#define TABLE_const_jpeg_ptrs                               (1522 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_inverse_Zig_Zag_ADDR_ADDR                     (0 + TABLE_const_jpeg_ptrs) //1 short
#define CONST_JPEG_DEC_448_ADDR                             (1 + TABLE_const_jpeg_ptrs) //1 short

#define CONST_JPEG_DEC_0_ADDR                               ((CONST_JPEG_DEC_0x00000040_ADDR+1))
#define CONST_JPEG_DEC_64_ADDR                              (CONST_JPEG_DEC_0x00000040_ADDR)
#define CONST_JPEG_DEC_128_ADDR                             (CONST_JPEG_DEC_0x00000080_ADDR)
#define CONST_JPEG_DEC_0xFF_ADDR                            ((TABLE_inverse_Zig_Zag_ADDR-2))
#define CONST_JPEG_DEC_1_ADDR                               ((TABLE_inverse_Zig_Zag_ADDR+1))
#define CONST_JPEG_DEC_2_ADDR                               ((TABLE_inverse_Zig_Zag_ADDR+5))
#define CONST_JPEG_DEC_3_ADDR                               ((TABLE_inverse_Zig_Zag_ADDR+6))
#define CONST_JPEG_DEC_4_ADDR                               ((TABLE_inverse_Zig_Zag_ADDR+14))
#define CONST_JPEG_DEC_7_ADDR                               ((TABLE_inverse_Zig_Zag_ADDR+28))
#define CONST_JPEG_DEC_8_ADDR                               ((TABLE_inverse_Zig_Zag_ADDR+2))
#define CONST_JPEG_DEC_10_ADDR                              ((TABLE_inverse_Zig_Zag_ADDR+7))
#define CONST_JPEG_DEC_15_ADDR                              ((TABLE_inverse_Zig_Zag_ADDR+42))
#define CONST_JPEG_DEC_16_ADDR                              ((TABLE_inverse_Zig_Zag_ADDR+3))
#define CONST_JPEG_DEC_31_ADDR                              ((TABLE_inverse_Zig_Zag_ADDR+92))
#define CONST_JPEG_DEC_32_ADDR                              ((TABLE_inverse_Zig_Zag_ADDR+93))
#define CONST_JPEG_DEC_63_ADDR                              ((TABLE_inverse_Zig_Zag_ADDR+63))
#define CONST_JPEG_DEC_152_ADDR                             ((TABLE_inverse_Zig_Zag_ADDR+202))

/*******************
 **  MP12_GLOBAL  **
 *******************/

/******************
 **  MP12_LOCAL  **
 ******************/

//CII_I_sample
#define CII_I_sample_Y_BEGIN_ADDR                           (0 + MP12_LOCAL_Y_BEGIN_ADDR)
#define TABLE_offset_table_ADDR                             (0 + CII_I_sample_Y_BEGIN_ADDR) //90 short
#define TABLE_linear_table_ADDR                             (90 + CII_I_sample_Y_BEGIN_ADDR) //14 short
#define TABLE_sf_table_ADDR                                 (104 + CII_I_sample_Y_BEGIN_ADDR) //64 short
#define TABLE_sbquant_table_ADDR                            (168 + CII_I_sample_Y_BEGIN_ADDR) //320 short
#define TABLE_bitalloc_table_ADDR                           (488 + CII_I_sample_Y_BEGIN_ADDR) //16 short
#define TABLE_qc_table_ADDR                                 (504 + CII_I_sample_Y_BEGIN_ADDR) //20 short

//CII_II_samples
#define CII_II_samples_Y_BEGIN_ADDR                         (324 + MP12_LOCAL_Y_BEGIN_ADDR)
#define layer_II_samples_addr                               (0 + CII_II_samples_Y_BEGIN_ADDR) //6 short
#define layer_II_TEMP_ADDR                                  (6 + CII_II_samples_Y_BEGIN_ADDR) //2 short
#define TABLE_sf_table_ADDR_ADDR                            (8 + CII_II_samples_Y_BEGIN_ADDR) //1 short
#define layer_II_scalefactor_ADDR_ADDR                      (9 + CII_II_samples_Y_BEGIN_ADDR) //1 short
#define layer_II_SBSAMPLE_SFTABLE_ADDR                      (10 + CII_II_samples_Y_BEGIN_ADDR) //1 short
#define layer_II_SFTABLE_ADDR                               (11 + CII_II_samples_Y_BEGIN_ADDR) //1 short
#define layer_II_bound_addr                                 (12 + CII_II_samples_Y_BEGIN_ADDR) //1 short
#define layer_II_sblimit_addr                               (13 + CII_II_samples_Y_BEGIN_ADDR) //1 short

//CII_mad_layer_II
#define CII_mad_layer_II_Y_BEGIN_ADDR                       (0 + MP12_LOCAL_Y_BEGIN_ADDR)
#define layer_II_allocation_addr                            (0 + CII_mad_layer_II_Y_BEGIN_ADDR) //64 short
#define layer_II_scfsi_addr                                 (64 + CII_mad_layer_II_Y_BEGIN_ADDR) //64 short
#define layer_II_scalefactor_addr                           (128 + CII_mad_layer_II_Y_BEGIN_ADDR) //192 short
#define TABLE_bitalloc_table_ADDR_ADDR                      (320 + CII_mad_layer_II_Y_BEGIN_ADDR) //2 short
#define layer_II_BACK_JUMP1_addr                            (322 + CII_mad_layer_II_Y_BEGIN_ADDR) //1 short
#define layer_II_BACK_JUMP_addr                             (323 + CII_mad_layer_II_Y_BEGIN_ADDR) //1 short

/******************
 **  JPEG_LOCAL  **
 ******************/

//CII_JPEG_Decode
#define CII_JPEG_Decode_X_BEGIN_ADDR                        (0 + JPEG_LOCAL_X_BEGIN_ADDR)
//JPEG_LOCAL_X_VAR
#define JPEG_LOCAL_X_VAR                                    (0 + CII_JPEG_Decode_X_BEGIN_ADDR)
#define GLOBAL_MCUBuffer_ADDR                               (0 + JPEG_LOCAL_X_VAR) //640 short
#define GLOBAL_MCU_lastcoef_ADDR                            (640 + JPEG_LOCAL_X_VAR) //10 short
#define GLOBAL_ycoef_ADDR                                   (650 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_ucoef_ADDR                                   (651 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_vcoef_ADDR                                   (652 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_coef_ADDR                                    (653 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_image_control_ADDR                           (654 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_lpMCUBuffer_ADDR_ADDR                        (655 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_lastcoef_pt_ADDR                             (656 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_temp_lpJpegBuf_ADDR                          (657 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_Y_data_ADDR                                  (658 + JPEG_LOCAL_X_VAR) //256 short
#define GLOBAL_rgb_buf_ADDR                                 (914 + JPEG_LOCAL_X_VAR) //5632 short
#define GLOBAL_temp_Y_data_ADDR                             (6546 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_col_num_ADDR                                 (6547 + JPEG_LOCAL_X_VAR) //1 short
#define TABLE_qt_table_ADDR                                 (6548 + JPEG_LOCAL_X_VAR) //196 short
#define GLOBAL_IDCT_BUFFER_X_ADDR                           (6744 + JPEG_LOCAL_X_VAR) //8 short
#define GLOBAL_row_even_ADDR                                (6752 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_col_even_ADDR                                (6753 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_block_id_ADDR                                (6754 + JPEG_LOCAL_X_VAR) //2 short
#define GLOBAL_JPEG_IMAGECONTROL_ADDR                       (6756 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_ImgWidth_ADDR                                (6757 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_ImgHeight_ADDR                               (6758 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_Temp_ImgHeight_ADDR                          (6759 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_temp_ImgWidth_ADDR                           (6760 + JPEG_LOCAL_X_VAR) //1 short
#define GLOBAL_Temp_ImgWidth_ADDR                           (6761 + JPEG_LOCAL_X_VAR) //1 short


//CII_JPEG_Decode
#define CII_JPEG_Decode_Y_BEGIN_ADDR                        (0 + JPEG_LOCAL_Y_BEGIN_ADDR)
//JPEG_LOCAL_Y_VAR
#define JPEG_LOCAL_Y_VAR                                    (0 + CII_JPEG_Decode_Y_BEGIN_ADDR)
#define LOCAL_IDCT_BUFFER_Y_ADDR                            (0 + JPEG_LOCAL_Y_VAR) //8 short
#define LOCAL_JPEG_IN_BUF_ADDR                              (8 + JPEG_LOCAL_Y_VAR) //448 short
#define LOCAL_VID_BMP_BUF_ADDR                              (456 + JPEG_LOCAL_Y_VAR) //640 short
#define LOCAL_VID_BUF_OUT_PTR                               (1096 + JPEG_LOCAL_Y_VAR) //2 short


//CII_DMAI_WRITEDATA
#define CII_DMAI_WRITEDATA_X_BEGIN_ADDR                     (6762 + JPEG_LOCAL_X_BEGIN_ADDR)
#define LOCAL_JPEG_DMA_TMP                                  (0 + CII_DMAI_WRITEDATA_X_BEGIN_ADDR) //63 short

//CII_DMAI_WRITEDATA
#define CII_DMAI_WRITEDATA_Y_BEGIN_ADDR                     (1098 + JPEG_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_ImgWidth_ADDR                                 (0 + CII_DMAI_WRITEDATA_Y_BEGIN_ADDR) //1 short

/*****************
 **  MP3_LOCAL  **
 *****************/

//VPP_MP3_DECODE
#define VPP_MP3_DECODE_X_BEGIN_ADDR                         (0 + MP3_LOCAL_X_BEGIN_ADDR)
#define OUTPUT_PCM_BUFFER1_ADDR                             (0 + VPP_MP3_DECODE_X_BEGIN_ADDR) //2304 short
#define STATIC_MAD_SBSAMPLE_ADDR                            (2304 + VPP_MP3_DECODE_X_BEGIN_ADDR) //4608 short

//CII_mad_synth_frame
#define CII_mad_synth_frame_X_BEGIN_ADDR                    (6912 + MP3_LOCAL_X_BEGIN_ADDR)
#define TABLE_D_ADDR_X                                      ((GLOBAL_INPUT_DATA_BUFFER+128))

//CII_mad_synth_frame
#define CII_mad_synth_frame_Y_BEGIN_ADDR                    (0 + MP3_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_mad_synth_frame_addr                          (0 + CII_mad_synth_frame_Y_BEGIN_ADDR) //64 short
#define LOCAL_output_addr_addr                              (64 + CII_mad_synth_frame_Y_BEGIN_ADDR) //1 short
#define LOCAL_output_raw_addr_addr                          (65 + CII_mad_synth_frame_Y_BEGIN_ADDR) //1 short

//CII_dct32
#define CII_dct32_X_BEGIN_ADDR                              (6912 + MP3_LOCAL_X_BEGIN_ADDR)
#define LOCAL_X_DCT32_TX1_T_ADDR                            (0 + CII_dct32_X_BEGIN_ADDR) //2 short
#define LOCAL_X_DCT32_TX1_ADDR                              (2 + CII_dct32_X_BEGIN_ADDR) //32 short
#define LOCAL_X_DCT32_TX2_T_ADDR                            (34 + CII_dct32_X_BEGIN_ADDR) //2 short
#define LOCAL_X_DCT32_TX2_ADDR                              (36 + CII_dct32_X_BEGIN_ADDR) //32 short

//CII_dct32
#define CII_dct32_Y_BEGIN_ADDR                              (66 + MP3_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_Y_DCT32_TY2_T_ADDR                            (0 + CII_dct32_Y_BEGIN_ADDR) //2 short
#define LOCAL_Y_DCT32_TY2_ADDR                              (2 + CII_dct32_Y_BEGIN_ADDR) //32 short
#define LOCAL_X_DCT32_IN_ADDR                               (34 + CII_dct32_Y_BEGIN_ADDR) //34 short
#define LOCAL_Y_DCT32_TX1_T_ADDR                            (68 + CII_dct32_Y_BEGIN_ADDR) //2 short
#define LOCAL_Y_DCT32_TY1_ADDR                              (70 + CII_dct32_Y_BEGIN_ADDR) //32 short
#define LOCAL_Y_PHASE_ADDR                                  (102 + CII_dct32_Y_BEGIN_ADDR) //2 short
#define CONST_DCT32_A24_ADDR_ADDR                           (104 + CII_dct32_Y_BEGIN_ADDR) //2 short
#define LOCAL_X_DCT32_A_ADDR                                (STRUCT_SIDEINFO_ADDR)
#define LOCAL_DCT32_A24_ADDR                                ((46+LOCAL_X_DCT32_A_ADDR))

//CII_III_decode
#define CII_III_decode_Y_BEGIN_ADDR                         (0 + MP3_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_III_decode_sfbwidth_ADDR                      (0 + CII_III_decode_Y_BEGIN_ADDR) //2 short

//CII_III_decode_inner_0
#define CII_III_decode_inner_0_Y_BEGIN_ADDR                 (2 + MP3_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_III_decode_sblimit_ADDR                       (0 + CII_III_decode_inner_0_Y_BEGIN_ADDR) //2 short
#define LOCAL_III_decode_output_ADDR                        (2 + CII_III_decode_inner_0_Y_BEGIN_ADDR) //72 short

//CII_III_scalefactors_lsf
#define CII_III_scalefactors_lsf_Y_BEGIN_ADDR               (2 + MP3_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_III_scalefactors_lsf_slen_ADDR                (0 + CII_III_scalefactors_lsf_Y_BEGIN_ADDR) //4 short

//CII_III_huffdecode
#define CII_III_huffdecode_Y_BEGIN_ADDR                     (2 + MP3_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_HUFF_SFBOUND_ADDR                             (0 + CII_III_huffdecode_Y_BEGIN_ADDR) //2 short
#define LOCAL_HUFF_REQCACHE_ADDR                            (2 + CII_III_huffdecode_Y_BEGIN_ADDR) //32 short
#define LOCAL_HUFF_TABLE_ADDR                               (34 + CII_III_huffdecode_Y_BEGIN_ADDR) //1 short
#define LOCAL_HUFF_LINBITS_ADDR                             (35 + CII_III_huffdecode_Y_BEGIN_ADDR) //1 short
#define LOCAL_HUFF_STARTBITS_ADDR                           (36 + CII_III_huffdecode_Y_BEGIN_ADDR) //1 short
#define LOCAL_HUFF_REQHITS_ADDR                             (37 + CII_III_huffdecode_Y_BEGIN_ADDR) //1 short
#define LOCAL_HUFF_REGION_ADDR                              (38 + CII_III_huffdecode_Y_BEGIN_ADDR) //1 short
#define LOCAL_HUFF_XRPTR_ADDR                               (39 + CII_III_huffdecode_Y_BEGIN_ADDR) //1 short

//CII_III_stereo
#define CII_III_stereo_Y_BEGIN_ADDR                         (2 + MP3_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_modes_ADDR                                    (0 + CII_III_stereo_Y_BEGIN_ADDR) //40 short
#define LOCAL_bound_ADDR                                    (40 + CII_III_stereo_Y_BEGIN_ADDR) //4 short

//CII_III_reorder
#define CII_III_reorder_Y_BEGIN_ADDR                        (2 + MP3_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_sbw_ADDR                                      (0 + CII_III_reorder_Y_BEGIN_ADDR) //3 short
#define LOCAL_sw_ADDR                                       (3 + CII_III_reorder_Y_BEGIN_ADDR) //3 short
#define LOCAL_III_reorder_tmp_ADDR                          ((STATIC_MAD_SBSAMPLE_ADDR+3*576*2))

//CII_III_imdct_s
#define CII_III_imdct_s_X_BEGIN_ADDR                        (6912 + MP3_LOCAL_X_BEGIN_ADDR)
#define LOCAL_III_imdct_s_Y_ADDR                            (0 + CII_III_imdct_s_X_BEGIN_ADDR) //40 short

//CII_III_exponents
#define CII_III_exponents_Y_BEGIN_ADDR                      (42 + MP3_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_EXPONENTS_ADDR                                (0 + CII_III_exponents_Y_BEGIN_ADDR) //39 short

//CII_imdct36
#define CII_imdct36_X_BEGIN_ADDR                            (6912 + MP3_LOCAL_X_BEGIN_ADDR)
#define LOCAL_III_imdct36_X_16_ADDR                         (0 + CII_imdct36_X_BEGIN_ADDR) //18 short
#define LOCAL_III_imdct36_X_4_ADDR                          (18 + CII_imdct36_X_BEGIN_ADDR) //18 short
#define LOCAL_III_imdct36_t_16_ADDR                         (36 + CII_imdct36_X_BEGIN_ADDR) //8 short
#define LOCAL_III_imdct36_t_4_ADDR                          (44 + CII_imdct36_X_BEGIN_ADDR) //8 short

/*****************
 **  AMR_LOCAL  **
 *****************/

//CII_AMR_Decode
#define CII_AMR_Decode_X_BEGIN_ADDR                         (0 + AMR_LOCAL_X_BEGIN_ADDR)
#define DEC_AMR_LSP_NEW_ADDRESS                             (0 + CII_AMR_Decode_X_BEGIN_ADDR) //10 short
#define LOCAL_ENC_SO_ADDR                                   (DEC_AMR_LSP_NEW_ADDRESS)
#define DEC_AMR_LSP_MID_ADDRESS                             (10 + CII_AMR_Decode_X_BEGIN_ADDR) //10 short
#define DEC_AMR_PREV_LSF_ADDRESS                            (20 + CII_AMR_Decode_X_BEGIN_ADDR) //10 short
#define DEC_AMR_LSF_I_ADDRESS                               (30 + CII_AMR_Decode_X_BEGIN_ADDR) //50 short
#define DEC_AMR_EXCP_ADDRESS                                (80 + CII_AMR_Decode_X_BEGIN_ADDR) //40 short
#define DEC_AMR_EXC_ENHANCED_ADDRESS                        (120 + CII_AMR_Decode_X_BEGIN_ADDR) //44 short
#define DEC_AMR_T0_ADDRESS                                  (164 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_T0_FRAC_ADDRESS                             (165 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_INDEX_ADDRESS                               (166 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_INDEX_MR475_ADDRESS                         (167 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_GAIN_PIT_ADDRESS                            (168 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_GAIN_CODE_ADDRESS                           (169 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_GAIN_CODE_MIX_ADDRESS                       (170 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_PIT_SHARP_ADDRESS                           (171 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_PIT_FLAG_ADDRESS                            (172 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_PIT_FAC_ADDRESS                             (173 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_T0_MIN_ADDRESS                              (174 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_T0_MAX_ADDRESS                              (175 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_DELTA_FRC_LOW_ADDRESS                       (176 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_DELTA_FRC_RANGE_ADDRESS                     (177 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP_SHIFT_ADDRESS                          (178 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP_ADDRESS                                (179 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_L_TEMP_ADDRESS                              (180 + CII_AMR_Decode_X_BEGIN_ADDR) //2 short
#define DEC_AMR_FLAG4_ADDRESS                               (182 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_CAREFULFLAG_ADDRESS                         (183 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_EXCENERGY_ADDRESS                           (184 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_SUBFRNR_ADDRESS                             (185 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_EVENSUBFR_ADDRESS                           (186 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_NEWDTXSTATE_ADDRESS                         (187 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_BFI_ADDRESS                                 (188 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_PDFI_ADDRESS                                (189 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_A_T_ADDRESS                                 (190 + CII_AMR_Decode_X_BEGIN_ADDR) //44 short
#define DEC_AMR_PARM_ADDRESS                                (234 + CII_AMR_Decode_X_BEGIN_ADDR) //58 short
#define COD_AMR_CODE_ADDRESS                                (292 + CII_AMR_Decode_X_BEGIN_ADDR) //40 short
#define DEC_AMR_CODE_ADDRESS                                (COD_AMR_CODE_ADDRESS)

//CII_AMR_Decode
#define CII_AMR_Decode_Y_BEGIN_ADDR                         (0 + AMR_LOCAL_Y_BEGIN_ADDR)
#define AMR_DEC_BUFOUT_ADDR                                 (0 + CII_AMR_Decode_Y_BEGIN_ADDR) //160 short

//CII_Syn_filt
#define CII_Syn_filt_Y_BEGIN_ADDR                           (160 + AMR_LOCAL_Y_BEGIN_ADDR)
#define SYN_FILT_TMP_ADDR                                   (0 + CII_Syn_filt_Y_BEGIN_ADDR) //80 short

//CII_bits2prm_amr_efr
#define CII_bits2prm_amr_efr_X_BEGIN_ADDR                   (332 + AMR_LOCAL_X_BEGIN_ADDR)
#define AMR_DEC_BUFIN_ADDR                                  (0 + CII_bits2prm_amr_efr_X_BEGIN_ADDR) //246 short

//CII_amr_dec_122
#define CII_amr_dec_122_X_BEGIN_ADDR                        (332 + AMR_LOCAL_X_BEGIN_ADDR)
#define DEC_AMR_TEMP0_ADDRESS                               (0 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP1_ADDRESS                               (1 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP2_ADDRESS                               (2 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP3_ADDRESS                               (3 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP4_ADDRESS                               (4 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP5_ADDRESS                               (5 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP6_ADDRESS                               (6 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP7_ADDRESS                               (7 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP8_ADDRESS                               (8 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP9_ADDRESS                               (9 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP10_ADDRESS                              (10 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP11_ADDRESS                              (11 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP12_ADDRESS                              (12 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP13_ADDRESS                              (13 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP14_ADDRESS                              (14 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP15_ADDRESS                              (15 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP16_ADDRESS                              (16 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP17_ADDRESS                              (17 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP18_ADDRESS                              (18 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP19_ADDRESS                              (19 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP20_ADDRESS                              (20 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP21_ADDRESS                              (21 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP22_ADDRESS                              (22 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short

//CII_Int_lpc_1and3_dec
#define CII_Int_lpc_1and3_dec_Y_BEGIN_ADDR                  (160 + AMR_LOCAL_Y_BEGIN_ADDR)
#define INT_LPC_LSP_ADDR                                    (0 + CII_Int_lpc_1and3_dec_Y_BEGIN_ADDR) //10 short
#define F1_ADDR                                             (10 + CII_Int_lpc_1and3_dec_Y_BEGIN_ADDR) //12 short
#define F2_ADDR                                             (22 + CII_Int_lpc_1and3_dec_Y_BEGIN_ADDR) //12 short

//CII_gmed_n
#define CII_gmed_n_Y_BEGIN_ADDR                             (160 + AMR_LOCAL_Y_BEGIN_ADDR)
#define MR475_gain_quant_coeff_ADDRESS                      (0 + CII_gmed_n_Y_BEGIN_ADDR) //10 short
#define MR475_gain_quant_coeff_lo_ADDRESS                   (10 + CII_gmed_n_Y_BEGIN_ADDR) //10 short

/********************
 **  COMMON_LOCAL  **
 ********************/

//RAM_X: size 0x2800, used 0x27de   RAM_Y: size 0x2800, used 0x27cc

#endif
