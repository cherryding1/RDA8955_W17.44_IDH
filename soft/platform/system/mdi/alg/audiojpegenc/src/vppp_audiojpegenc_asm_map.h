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

#define COMMON_GLOBAL_X_SIZE                                    0
#define COMMON_GLOBAL_Y_SIZE                                    42
#define MP3_GLOBAL_X_SIZE                                       10124
#define MP3_GLOBAL_Y_SIZE                                       9240
#define MP3_LOCAL_X_SIZE                                        0
#define MP3_LOCAL_Y_SIZE                                        0
#define AMR_GLOBAL_X_SIZE                                       10098
#define AMR_GLOBAL_Y_SIZE                                       7616
#define AMR_LOCAL_X_SIZE                                        0
#define AMR_LOCAL_Y_SIZE                                        128
#define YUVROTATE_GLOBAL_X_SIZE                                 196
#define YUVROTATE_GLOBAL_Y_SIZE                                 622
#define YUVROTATE_LOCAL_X_SIZE                                  0
#define YUVROTATE_LOCAL_Y_SIZE                                  0
#define COMMON_LOCAL_X_SIZE                                     0
#define COMMON_LOCAL_Y_SIZE                                     0

/**************************
 **  SECTION_BEGIN_ADDR  **
 **************************/

#define COMMON_GLOBAL_X_BEGIN_ADDR                              (0 + RAM_X_BEGIN_ADDR)
#define COMMON_GLOBAL_Y_BEGIN_ADDR                              (0 + RAM_Y_BEGIN_ADDR)
#define MP3_GLOBAL_X_BEGIN_ADDR                                 (0 + RAM_X_BEGIN_ADDR)
#define MP3_GLOBAL_Y_BEGIN_ADDR                                 (42 + RAM_Y_BEGIN_ADDR)
#define MP3_LOCAL_X_BEGIN_ADDR                                  (10124 + RAM_X_BEGIN_ADDR)
#define MP3_LOCAL_Y_BEGIN_ADDR                                  (9282 + RAM_Y_BEGIN_ADDR)
#define AMR_GLOBAL_X_BEGIN_ADDR                                 (0 + RAM_X_BEGIN_ADDR)
#define AMR_GLOBAL_Y_BEGIN_ADDR                                 (42 + RAM_Y_BEGIN_ADDR)
#define AMR_LOCAL_X_BEGIN_ADDR                                  (10098 + RAM_X_BEGIN_ADDR)
#define AMR_LOCAL_Y_BEGIN_ADDR                                  (7658 + RAM_Y_BEGIN_ADDR)
#define YUVROTATE_GLOBAL_X_BEGIN_ADDR                           (0 + RAM_X_BEGIN_ADDR)
#define YUVROTATE_GLOBAL_Y_BEGIN_ADDR                           (42 + RAM_Y_BEGIN_ADDR)
#define YUVROTATE_LOCAL_X_BEGIN_ADDR                            (196 + RAM_X_BEGIN_ADDR)
#define YUVROTATE_LOCAL_Y_BEGIN_ADDR                            (664 + RAM_Y_BEGIN_ADDR)
#define COMMON_LOCAL_X_BEGIN_ADDR                               (10124 + RAM_X_BEGIN_ADDR)
#define COMMON_LOCAL_Y_BEGIN_ADDR                               (9282 + RAM_Y_BEGIN_ADDR)

/*********************
 **  COMMON_GLOBAL  **
 *********************/

//GLOBAL_VARS
//vpp_AudioJpeg_ENC_IN_STRUCT
#define vpp_AudioJpeg_ENC_IN_STRUCT                         (0 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define Input_mode_addr                                     (0 + vpp_AudioJpeg_ENC_IN_STRUCT) //1 short
#define Input_reset_addr                                    (1 + vpp_AudioJpeg_ENC_IN_STRUCT) //1 short
#define Input_inStreamBufAddr_addr                          (2 + vpp_AudioJpeg_ENC_IN_STRUCT) //1 word
#define Input_outStreamBufAddr_addr                         (4 + vpp_AudioJpeg_ENC_IN_STRUCT) //1 word
#define Input_samplerate_addr                               (6 + vpp_AudioJpeg_ENC_IN_STRUCT) //1 word
#define Input_bitrate_addr                                  (8 + vpp_AudioJpeg_ENC_IN_STRUCT) //1 short
#define Input_JPEGENC_QUALITY_addr                          (9 + vpp_AudioJpeg_ENC_IN_STRUCT) //1 short
#define Input_JPEGENC_WIDTH_addr                            (10 + vpp_AudioJpeg_ENC_IN_STRUCT) //1 short
#define Input_JPEGENC_HEIGHT_addr                           (11 + vpp_AudioJpeg_ENC_IN_STRUCT) //1 short
#define Input_Channel_Num_addr                              (12 + vpp_AudioJpeg_ENC_IN_STRUCT) //1 short
#define Input_reserved_addr                                 (13 + vpp_AudioJpeg_ENC_IN_STRUCT) //1 short

#define GlOBAL_ImgInw                                       (Input_samplerate_addr) //alias
#define GlOBAL_ImgInh                                       (Input_samplerate_addr+1) //alias
#define GlOBAL_ImgOutw                                      (Input_JPEGENC_WIDTH_addr) //alias
#define GlOBAL_ImgOuth                                      (Input_JPEGENC_HEIGHT_addr) //alias
//vpp_AudioJpeg_ENC_OUT_STRUCT
#define vpp_AudioJpeg_ENC_OUT_STRUCT                        (14 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define Output_mode_addr                                    (0 + vpp_AudioJpeg_ENC_OUT_STRUCT) //1 short
#define Output_streamStatus_addr                            (1 + vpp_AudioJpeg_ENC_OUT_STRUCT) //1 short
#define Output_output_len_addr                              (2 + vpp_AudioJpeg_ENC_OUT_STRUCT) //1 word
#define Output_consumedLen_addr                             (4 + vpp_AudioJpeg_ENC_OUT_STRUCT) //1 word
#define Output_ERR_Status_addr                              (6 + vpp_AudioJpeg_ENC_OUT_STRUCT) //1 short
#define Output_reserve0_addr                                (7 + vpp_AudioJpeg_ENC_OUT_STRUCT) //1 short

//VPP_MP3_Enc_Code_ExternalAddr_addr
#define VPP_MP3_Enc_Code_ExternalAddr_addr                  (22 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define MP3_Code_ExternalAddr_addr                          (0 + VPP_MP3_Enc_Code_ExternalAddr_addr) //1 word
#define MP3_ConstX_ExternalAddr_addr                        (2 + VPP_MP3_Enc_Code_ExternalAddr_addr) //1 word
#define MP3_ConstY_ExternalAddr_addr                        (4 + VPP_MP3_Enc_Code_ExternalAddr_addr) //1 word
#define MP3_Const_rqmy_ExternalAddr_addr                    (6 + VPP_MP3_Enc_Code_ExternalAddr_addr) //1 word
#define MP3_Const_zig_ExternalAddr_addr                     (8 + VPP_MP3_Enc_Code_ExternalAddr_addr) //1 word

//VPP_AMR_Enc_Code_ExternalAddr_addr
#define VPP_AMR_Enc_Code_ExternalAddr_addr                  (32 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define AMRJPEG_Code_ExternalAddr_addr                      (0 + VPP_AMR_Enc_Code_ExternalAddr_addr) //1 word
#define AMRJPEG_ConstX_ExternalAddr_addr                    (2 + VPP_AMR_Enc_Code_ExternalAddr_addr) //1 word
#define AMRJPEG_ConstY_ExternalAddr_addr                    (4 + VPP_AMR_Enc_Code_ExternalAddr_addr) //1 word

#define RGBYUV_ROTATE_Code_ExternalAddr_addr                (38 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_MIC_DIGITAL_GAIN_ADDR                        (40 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Output_reserve1_addr                                (41 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short

/******************
 **  MP3_GLOBAL  **
 ******************/

//GLOBAL_VARS
#define OUTPUT_MP3_BUFFER_ADDR                              (0 + MP3_GLOBAL_X_BEGIN_ADDR) //1024 short
#define GLOBAL_XW_begine_ADDR                               (1024 + MP3_GLOBAL_X_BEGIN_ADDR) //1024 short
//IN_MP3_BUFFER
#define IN_MP3_BUFFER                                       (2048 + MP3_GLOBAL_X_BEGIN_ADDR) //struct
#define IN_MP3_BUFFER_10                                    (0 + IN_MP3_BUFFER) //576 short
#define IN_MP3_BUFFER_11                                    (576 + IN_MP3_BUFFER) //576 short
#define IN_MP3_BUFFER_20                                    (1152 + IN_MP3_BUFFER) //576 short
#define IN_MP3_BUFFER_21                                    (1728 + IN_MP3_BUFFER) //576 short

#define GLOBAL_L3ENC_gr1                                    (IN_MP3_BUFFER) //alias
//SHINE_GLOBAL_VARS_X_START_ADDR
#define SHINE_GLOBAL_VARS_X_START_ADDR                      (4352 + MP3_GLOBAL_X_BEGIN_ADDR) //struct
#define STATIC_PartHoldersInitialized_ADDR                  (0 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STATIC_buf_bit_idx_ADDR                             (1 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STATIC_buf_short_idx_ADDR                           (2 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STATIC_buf_long_idx_ADDR                            (3 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STATIC_buf_chache_ADDR                              (4 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 word
#define STATIC_BitCount_ADDR                                (6 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 word
#define STATIC_ThisFrameSize_ADDR                           (8 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 word
#define STATIC_BitsRemaining_ADDR                           (10 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 word
#define GLOBAL_OFF_ADDR                                     (12 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 word
#define STATIC_FIRSTCALL_ADDR                               (14 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define GLOBAL_main_data_begin_ADDR                         (15 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define GLOBAL_xrmax_ADDR                                   (16 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 word
#define STATIC_ResvSize_ADDR                                (18 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STATIC_scalefac_band_long_ADDR                      (19 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STATIC_change_ADDR                                  (20 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STATIC_nGranules_ADDR                               (21 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STRUCT_frameData_frameLength_ADDR                   (22 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 word
#define GLOBLE_channel_ADDR                                 (24 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 word

//STRUCT_frameData_ADDR
#define STRUCT_frameData_ADDR                               (4378 + MP3_GLOBAL_X_BEGIN_ADDR) //struct
#define STRUCT_frameData_STRUCT_header_ADDR                 (0 + STRUCT_frameData_ADDR) //1 word
#define STRUCT_frameData_STRUCT_frameSI_ADDR                (2 + STRUCT_frameData_ADDR) //1 word
#define STRUCT_frameData_STRUCT_channelSI_ADDR              (4 + STRUCT_frameData_ADDR) //2 word
#define STRUCT_frameData_STRUCT_spectrumSI_ADDR             (8 + STRUCT_frameData_ADDR) //4 word

//GLOBAL_MDCT_FREQ_00_ADDR
#define GLOBAL_MDCT_FREQ_00_ADDR                            (4394 + MP3_GLOBAL_X_BEGIN_ADDR) //struct
#define GLOBAL_Z_ADDR                                       (0 + GLOBAL_MDCT_FREQ_00_ADDR) //1024 short
#define GLOBAL_Y_ADDR                                       (1024 + GLOBAL_MDCT_FREQ_00_ADDR) //140 short

#define TEMP_MDCT_ADDR                                      (GLOBAL_Y_ADDR) //alias
#define GLOBAL_mdct_in_ADDR                                 (TEMP_MDCT_ADDR) //alias
#define LOCAL_f_tab_ADDR                                    ((72 + TEMP_MDCT_ADDR)) //alias
#define TEMP_A_ADDR                                         ((108 + TEMP_MDCT_ADDR)) //alias
#define TEMP_A_ADDR_9                                       ((18 + TEMP_A_ADDR)) //alias
#define GLOBAL_L3ENC_gr0                                    (5558 + MP3_GLOBAL_X_BEGIN_ADDR) //2304 short
#define TABLE_t1HB_tab_ADDR                                 (7862 + MP3_GLOBAL_X_BEGIN_ADDR) //2 short
#define TABLE_t2HB_tab_ADDR                                 (7864 + MP3_GLOBAL_X_BEGIN_ADDR) //5 short
#define TABLE_t3HB_tab_ADDR                                 (7869 + MP3_GLOBAL_X_BEGIN_ADDR) //5 short
#define TABLE_t5HB_tab_ADDR                                 (7874 + MP3_GLOBAL_X_BEGIN_ADDR) //8 short
#define TABLE_t6HB_tab_ADDR                                 (7882 + MP3_GLOBAL_X_BEGIN_ADDR) //8 short
#define TABLE_t7HB_tab_ADDR                                 (7890 + MP3_GLOBAL_X_BEGIN_ADDR) //18 short
#define TABLE_t8HB_tab_ADDR                                 (7908 + MP3_GLOBAL_X_BEGIN_ADDR) //18 short
#define TABLE_t9HB_tab_ADDR                                 (7926 + MP3_GLOBAL_X_BEGIN_ADDR) //18 short
#define TABLE_t10HB_tab_ADDR                                (7944 + MP3_GLOBAL_X_BEGIN_ADDR) //32 short
#define TABLE_t11HB_tab_ADDR                                (7976 + MP3_GLOBAL_X_BEGIN_ADDR) //32 short
#define TABLE_t12HB_tab_ADDR                                (8008 + MP3_GLOBAL_X_BEGIN_ADDR) //32 short
#define TABLE_t13HB_tab_ADDR                                (8040 + MP3_GLOBAL_X_BEGIN_ADDR) //128 short
#define TABLE_t15HB_tab_ADDR                                (8168 + MP3_GLOBAL_X_BEGIN_ADDR) //128 short
#define TABLE_t16HB_tab_ADDR                                (8296 + MP3_GLOBAL_X_BEGIN_ADDR) //256 short
#define TABLE_t24HB_tab_ADDR                                (8552 + MP3_GLOBAL_X_BEGIN_ADDR) //256 short
#define TABLE_t32HB_tab_ADDR                                (8808 + MP3_GLOBAL_X_BEGIN_ADDR) //8 short
#define TABLE_t33HB_tab_ADDR                                (8816 + MP3_GLOBAL_X_BEGIN_ADDR) //8 short
#define TABLE_t1l_tab_ADDR                                  (8824 + MP3_GLOBAL_X_BEGIN_ADDR) //2 short
#define TABLE_t2l_tab_ADDR                                  (8826 + MP3_GLOBAL_X_BEGIN_ADDR) //5 short
#define TABLE_t3l_tab_ADDR                                  (8831 + MP3_GLOBAL_X_BEGIN_ADDR) //5 short
#define TABLE_t5l_tab_ADDR                                  (8836 + MP3_GLOBAL_X_BEGIN_ADDR) //8 short
#define TABLE_t6l_tab_ADDR                                  (8844 + MP3_GLOBAL_X_BEGIN_ADDR) //8 short
#define TABLE_t7l_tab_ADDR                                  (8852 + MP3_GLOBAL_X_BEGIN_ADDR) //18 short
#define TABLE_t8l_tab_ADDR                                  (8870 + MP3_GLOBAL_X_BEGIN_ADDR) //18 short
#define TABLE_t9l_tab_ADDR                                  (8888 + MP3_GLOBAL_X_BEGIN_ADDR) //18 short
#define TABLE_t10l_tab_ADDR                                 (8906 + MP3_GLOBAL_X_BEGIN_ADDR) //32 short
#define TABLE_t11l_tab_ADDR                                 (8938 + MP3_GLOBAL_X_BEGIN_ADDR) //32 short
#define TABLE_t12l_tab_ADDR                                 (8970 + MP3_GLOBAL_X_BEGIN_ADDR) //32 short
#define TABLE_t13l_tab_ADDR                                 (9002 + MP3_GLOBAL_X_BEGIN_ADDR) //128 short
#define TABLE_t15l_tab_ADDR                                 (9130 + MP3_GLOBAL_X_BEGIN_ADDR) //128 short
#define TABLE_t16l_tab_ADDR                                 (9258 + MP3_GLOBAL_X_BEGIN_ADDR) //128 short
#define TABLE_t24l_tab_ADDR                                 (9386 + MP3_GLOBAL_X_BEGIN_ADDR) //128 short
#define TABLE_t32l_tab_ADDR                                 (9514 + MP3_GLOBAL_X_BEGIN_ADDR) //8 short
#define TABLE_t33l_tab_ADDR                                 (9522 + MP3_GLOBAL_X_BEGIN_ADDR) //8 short
#define TABLE_ht_tab_ADDR                                   (9530 + MP3_GLOBAL_X_BEGIN_ADDR) //170 short
#define TABLE_sfBandIndex_tab_ADDR                          (9700 + MP3_GLOBAL_X_BEGIN_ADDR) //222 short
#define TABLE_subdv_table_tab_ADDR                          (9922 + MP3_GLOBAL_X_BEGIN_ADDR) //24 short
#define TABLE_coef_tab_ADDR                                 (9946 + MP3_GLOBAL_X_BEGIN_ADDR) //72 short
#define TABLE_encodeSideInfo_tab_ADDR                       (10018 + MP3_GLOBAL_X_BEGIN_ADDR) //12 short
#define MP3_CONST_inverse_Zig_Zag_ADDR                      (10030 + MP3_GLOBAL_X_BEGIN_ADDR) //78 short
#define MP3_CONST_1152_ADDR                                 (10108 + MP3_GLOBAL_X_BEGIN_ADDR) //1 short
#define MP3_CONST_NEG7_ADDR                                 (10109 + MP3_GLOBAL_X_BEGIN_ADDR) //1 short
#define MP3_CONST_1_WORD32_ADDR                             (10110 + MP3_GLOBAL_X_BEGIN_ADDR) //2 short
#define MP3_CONST_neg1_ADDR                                 (10112 + MP3_GLOBAL_X_BEGIN_ADDR) //1 short
#define MP3_CONST_NEG31_ADDR                                (10113 + MP3_GLOBAL_X_BEGIN_ADDR) //1 short
#define MP3_CONST_0x90d0614_ADDR                            (10114 + MP3_GLOBAL_X_BEGIN_ADDR) //2 short
#define MP3_CONST_0X28514_165140_ADDR                       (10116 + MP3_GLOBAL_X_BEGIN_ADDR) //2 short
#define MP3_CONST_10000_WORD32_ADDR                         (10118 + MP3_GLOBAL_X_BEGIN_ADDR) //2 short
#define MP3_CONST_0X33E40_212544_ADDR                       (10120 + MP3_GLOBAL_X_BEGIN_ADDR) //2 short
#define MP3_CONST_0X186A0_100000_ADDR                       (10122 + MP3_GLOBAL_X_BEGIN_ADDR) //2 short
#define MP3_CONST_2_ADDR                                    ((5 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_3_ADDR                                    ((6 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_4_ADDR                                    ((14 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_5_ADDR                                    ((15 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_6_ADDR                                    ((27 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_7_ADDR                                    ((28 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_8_ADDR                                    ((2 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_9_ADDR                                    ((4 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_10_ADDR                                   ((7 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_11_ADDR                                   ((13 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_12_ADDR                                   ((16 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_13_ADDR                                   ((26 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_14_ADDR                                   ((29 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_15_ADDR                                   ((42 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_16_ADDR                                   ((3 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_17_ADDR                                   (( 8 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_18_ADDR                                   (( 12 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_19_ADDR                                   ((17 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_20_ADDR                                   ((25 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_21_ADDR                                   ((30 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_22_ADDR                                   ((41 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_23_ADDR                                   ((43 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_24_ADDR                                   ((9 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_25_ADDR                                   ((11 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_26_ADDR                                   ((18 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_27_ADDR                                   ((24 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_28_ADDR                                   ((31 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_29_ADDR                                   ((40 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_30_ADDR                                   ((44 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_31_ADDR                                   ((53 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_32_ADDR                                   ((10 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_33_ADDR                                   ((19 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_34_ADDR                                   ((23 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_35_ADDR                                   ((32 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_36_ADDR                                   ((39 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_37_ADDR                                   ((45 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_38_ADDR                                   ((52 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_39_ADDR                                   ((54 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_40_ADDR                                   ((20 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_41_ADDR                                   ((22 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_42_ADDR                                   ((33 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_43_ADDR                                   ((38 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_44_ADDR                                   ((46 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_45_ADDR                                   ((51 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_46_ADDR                                   ((55 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_47_ADDR                                   ((60 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_48_ADDR                                   ((21 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_49_ADDR                                   ((34 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_50_ADDR                                   ((37 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_51_ADDR                                   ((47 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_52_ADDR                                   ((50 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_53_ADDR                                   ((56 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_54_ADDR                                   ((59 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_55_ADDR                                   ((61 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_56_ADDR                                   ((35 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_57_ADDR                                   ((36 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_58_ADDR                                   ((48 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_59_ADDR                                   ((49 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_60_ADDR                                   (( 57 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_61_ADDR                                   ((58 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_62_ADDR                                   (( 62 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_63_ADDR                                   ((63 + MP3_CONST_inverse_Zig_Zag_ADDR)) //alias
#define MP3_CONST_64_ADDR                                   ((52 + TABLE_t16HB_tab_ADDR)) //alias
#define MP3_CONST_66_ADDR                                   ((80 + TABLE_t16HB_tab_ADDR)) //alias
#define MP3_CONST_122_ADDR                                  ((22 + TABLE_t24HB_tab_ADDR)) //alias
#define MP3_CONST_128_ADDR                                  ((37 + TABLE_t24HB_tab_ADDR)) //alias
#define MP3_CONST_164_ADDR                                  ((71 + TABLE_t16HB_tab_ADDR)) //alias
#define MP3_CONST_255_ADDR                                  ((107 + TABLE_ht_tab_ADDR)) //alias
#define MP3_CONST_0xff_ADDR                                 ((MP3_CONST_255_ADDR)) //alias
#define MP3_CONST_511_ADDR                                  ((147 + TABLE_ht_tab_ADDR)) //alias
#define MP3_CONST_576_ADDR                                  ((22 + TABLE_sfBandIndex_tab_ADDR)) //alias
#define MP3_CONST_0_ADDR                                    ((TABLE_ht_tab_ADDR)) //alias
#define MP3_CONST_0_WORD32_ADDR                             ((MP3_CONST_0_ADDR)) //alias
#define MP3_CONST_1_ADDR                                    ((MP3_CONST_inverse_Zig_Zag_ADDR + 1 )) //alias

//GLOBAL_VARS
#define STRUCT_INPUT_slot_lag_ADDR                          (0 + MP3_GLOBAL_Y_BEGIN_ADDR) //1 word
#define STRUCT_INPUT_frac_slots_per_frame_ADDR              (2 + MP3_GLOBAL_Y_BEGIN_ADDR) //1 word
#define STRUCT_INPUT_whole_slots_per_frame_ADDR             (4 + MP3_GLOBAL_Y_BEGIN_ADDR) //1 short
#define MP3ENC_MP3_CONST_bitrate_map_ADDR                   (5 + MP3_GLOBAL_Y_BEGIN_ADDR) //15 short
//STRUCT_CONFIG_ADDR
#define STRUCT_CONFIG_ADDR                                  (20 + MP3_GLOBAL_Y_BEGIN_ADDR) //struct
#define STRUCT_CONFIG_mpeg_bitrate_index_ADDR               (0 + STRUCT_CONFIG_ADDR) //1 short
#define STRUCT_CONFIG_mpeg_samplerate_index_ADDR            (1 + STRUCT_CONFIG_ADDR) //1 short
#define STRUCT_CONFIG_mpeg_padding_ADDR                     (2 + STRUCT_CONFIG_ADDR) //1 short
#define STRUCT_CONFIG_mpeg_type_ADDR                        (3 + STRUCT_CONFIG_ADDR) //1 short
#define STRUCT_CONFIG_mpeg_crc_ADDR                         (4 + STRUCT_CONFIG_ADDR) //1 short
#define STRUCT_CONFIG_mpeg_mode_ADDR                        (5 + STRUCT_CONFIG_ADDR) //1 short
#define STRUCT_CONFIG_mpeg_emph_ADDR                        (6 + STRUCT_CONFIG_ADDR) //1 short
#define STRUCT_CONFIG_mpeg_ext_ADDR                         (7 + STRUCT_CONFIG_ADDR) //1 short
#define STRUCT_CONFIG_mpeg_mode_ext_ADDR                    (8 + STRUCT_CONFIG_ADDR) //1 short
#define STRUCT_CONFIG_mpeg_copyright_ADDR                   (9 + STRUCT_CONFIG_ADDR) //1 short
#define STRUCT_CONFIG_mpeg_original_ADDR                    (10 + STRUCT_CONFIG_ADDR) //2 short

//STRUCT_SIDEINFO_ADDR
#define STRUCT_SIDEINFO_ADDR                                (32 + MP3_GLOBAL_Y_BEGIN_ADDR) //struct
#define STRUCT_SIDEINFO_main_data_begin_ADDR                (0 + STRUCT_SIDEINFO_ADDR) //2 short
#define STRUCT_SIDEINFO_private_bits_ADDR                   (2 + STRUCT_SIDEINFO_ADDR) //2 short
#define STRUCT_SIDEINFO_resvDrain_ADDR                      (4 + STRUCT_SIDEINFO_ADDR) //2 short
#define STRUCT_SIDEINFO_scfsi_ADDR                          (6 + STRUCT_SIDEINFO_ADDR) //16 short
#define STRUCT_SIDEINFO_gr0_ch0_part2_3_length_ADDR         (22 + STRUCT_SIDEINFO_ADDR) //92 short
#define STRUCT_SIDEINFO_gr1_ch0_part2_3_length_ADDR         (114 + STRUCT_SIDEINFO_ADDR) //92 short

//STRUCT_GRINFO_ADDR
#define STRUCT_GRINFO_ADDR                                  (238 + MP3_GLOBAL_Y_BEGIN_ADDR) //struct
#define STRUCT_GRINFO_part2_3_length_ADDR                   (0 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_big_values_ADDR                       (2 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_count1_ADDR                           (4 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_global_gain_ADDR                      (6 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_scalefac_compress_ADDR                (8 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_table_select_ADDR                     (10 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_table_select1_ADDR                    (12 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_table_select2_ADDR                    (14 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_region0_count_ADDR                    (16 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_region1_count_ADDR                    (18 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_preflag_ADDR                          (20 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_scalefac_scale_ADDR                   (22 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_count1table_select_ADDR               (24 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_part2_length_ADDR                     (26 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_sfb_lmax_ADDR                         (28 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_address1_ADDR                         (30 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_address2_ADDR                         (32 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_address3_ADDR                         (34 + STRUCT_GRINFO_ADDR) //2 short
#define STRUCT_GRINFO_quantizerStepSize_ADDR                (36 + STRUCT_GRINFO_ADDR) //2 short

#define STATIC_RQ_ADDR                                      (276 + MP3_GLOBAL_Y_BEGIN_ADDR) //100 short
//GLOBAL_l3_sb_sample_ADDR
#define GLOBAL_l3_sb_sample_ADDR                            (376 + MP3_GLOBAL_Y_BEGIN_ADDR) //struct
#define l3_sb_sample_0_0_0_0_ADDR                           (0 + GLOBAL_l3_sb_sample_ADDR) //1152 short
#define l3_sb_sample_0_1_0_0_ADDR                           (1152 + GLOBAL_l3_sb_sample_ADDR) //1152 short
#define l3_sb_sample_1_0_0_0_ADDR                           (2304 + GLOBAL_l3_sb_sample_ADDR) //1152 short
#define l3_sb_sample_1_1_0_0_ADDR                           (3456 + GLOBAL_l3_sb_sample_ADDR) //1152 short

#define GLOBAL_MDCT_FREQ_01_ADDR                            (4984 + MP3_GLOBAL_Y_BEGIN_ADDR) //1164 short
#define GLOBAL_RXAB_ADDR                                    (6148 + MP3_GLOBAL_Y_BEGIN_ADDR) //1152 short
#define SHINE_LOCAL_VARS_Y_START_ADDR                       ((GLOBAL_MDCT_FREQ_01_ADDR)) //alias
#define STRUCT_frameData_STRUCT_header_element_ADDR         ((0 + SHINE_LOCAL_VARS_Y_START_ADDR)) //alias
#define STRUCT_frameData_STRUCT_frameSI_element_ADDR            ((30 + SHINE_LOCAL_VARS_Y_START_ADDR)) //alias
#define STRUCT_frameData_STRUCT_channelSI_ch0_element_ADDR          ((36 + SHINE_LOCAL_VARS_Y_START_ADDR)) //alias
#define STRUCT_frameData_STRUCT_channelSI_ch1_element_ADDR          ((44 + SHINE_LOCAL_VARS_Y_START_ADDR)) //alias
#define STRUCT_frameData_STRUCT_spectrumSI_gr0ch0_element_ADDR          ((52 + SHINE_LOCAL_VARS_Y_START_ADDR)) //alias
#define STRUCT_frameData_STRUCT_spectrumSI_gr0ch1_element_ADDR          ((78 + SHINE_LOCAL_VARS_Y_START_ADDR)) //alias
#define STRUCT_frameData_STRUCT_spectrumSI_gr1ch0_element_ADDR          ((104 + SHINE_LOCAL_VARS_Y_START_ADDR)) //alias
#define STRUCT_frameData_STRUCT_spectrumSI_gr1ch1_element_ADDR          ((130 + SHINE_LOCAL_VARS_Y_START_ADDR)) //alias
#define STRUCT_frameData_STRUCT_codedData_gr0ch0_element_ADDR           ((160 + SHINE_LOCAL_VARS_Y_START_ADDR)) //alias
#define TEMP_B_ADDR                                         (STRUCT_GRINFO_ADDR) //alias
#define TEMP_B_ADDR_9                                       ((18+ TEMP_B_ADDR)) //alias
#define TABLE_fl_tab_ADDR                                   (7300 + MP3_GLOBAL_Y_BEGIN_ADDR) //992 short
#define TABLE_ew_tab_ADDR                                   (8292 + MP3_GLOBAL_Y_BEGIN_ADDR) //500 short
#define TABLE_w_tab_ADDR                                    (8792 + MP3_GLOBAL_Y_BEGIN_ADDR) //36 short
#define TABLE_w2_tab_ADDR                                   (8828 + MP3_GLOBAL_Y_BEGIN_ADDR) //18 short
#define TABLE_win_tab_ADDR                                  (8846 + MP3_GLOBAL_Y_BEGIN_ADDR) //72 short
#define TABLE_steptabi_tab_ADDR                             (8918 + MP3_GLOBAL_Y_BEGIN_ADDR) //256 short
#define TABLE_pow075_tab_tab_ADDR                           (9174 + MP3_GLOBAL_Y_BEGIN_ADDR) //24 short
#define TABLE_putmask_tab_ADDR                              (9198 + MP3_GLOBAL_Y_BEGIN_ADDR) //10 short
#define TABLE_ca_encode_tab_ADDR                            (9208 + MP3_GLOBAL_Y_BEGIN_ADDR) //16 short
#define TABLE_cs_encode_tab_ADDR                            (9224 + MP3_GLOBAL_Y_BEGIN_ADDR) //16 short
#define MP3_CONST_512_ADDR                                  ((190 + TABLE_steptabi_tab_ADDR)) //alias
#define MP3_CONST_4095_ADDR                                 ((15 + TABLE_cs_encode_tab_ADDR)) //alias
#define MP3_CONST_8192_WORD32_ADDR                          ((158 + TABLE_steptabi_tab_ADDR )) //alias
#define MP3_CONST_0X40000000_ADDR                           (( 22 + TABLE_steptabi_tab_ADDR )) //alias

/*****************
 **  MP3_LOCAL  **
 *****************/

/******************
 **  AMR_GLOBAL  **
 ******************/

//GLOBAL_VARS
#define JPEGENC_OUT_ADDR                                    (0 + AMR_GLOBAL_X_BEGIN_ADDR) //744 short
#define OUTPUT_BITS_ADDR                                    (JPEGENC_OUT_ADDR) //alias
#define INPUT_SPEECH_BUFFER_ADDR                            ((16 + JPEGENC_OUT_ADDR)) //alias
#define TEMP_MYRING_ADDR                                    (744 + AMR_GLOBAL_X_BEGIN_ADDR) //364 short
#define STATIC_L_SACF_ADDR                                  (1108 + AMR_GLOBAL_X_BEGIN_ADDR) //56 short
#define SCAL_ACF_ADDR_P                                     (1164 + AMR_GLOBAL_X_BEGIN_ADDR) //4 short
#define PRAM4_TABLE_ADDR                                    (1168 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
//STRUCT_PRE_PROCESSSTATE
#define STRUCT_PRE_PROCESSSTATE                             (1178 + AMR_GLOBAL_X_BEGIN_ADDR) //struct
#define STRUCT_PRE_PROCESSSTATE_Y2_LO_ADDR                  (0 + STRUCT_PRE_PROCESSSTATE) //1 short
#define STRUCT_PRE_PROCESSSTATE_Y2_HI_ADDR                  (1 + STRUCT_PRE_PROCESSSTATE) //1 short
#define STRUCT_PRE_PROCESSSTATE_Y1_LO_ADDR                  (2 + STRUCT_PRE_PROCESSSTATE) //1 short
#define STRUCT_PRE_PROCESSSTATE_Y1_HI_ADDR                  (3 + STRUCT_PRE_PROCESSSTATE) //1 short
#define STRUCT_PRE_PROCESSSTATE_X0_ADDR                     (4 + STRUCT_PRE_PROCESSSTATE) //1 short
#define STRUCT_PRE_PROCESSSTATE_X1_ADDR                     (5 + STRUCT_PRE_PROCESSSTATE) //1 short

//STRUCT_COD_AMRSTATE
#define STRUCT_COD_AMRSTATE                                 (1184 + AMR_GLOBAL_X_BEGIN_ADDR) //struct
#define STRUCT_COD_AMRSTATE_ADDR                            (0 + STRUCT_COD_AMRSTATE) //40 short
#define STRUCT_COD_AMRSTATE_P_WINDOW_12K2_ADDR              (40 + STRUCT_COD_AMRSTATE) //40 short
#define STRUCT_COD_AMRSTATE_P_WINDOW_ADDR                   (80 + STRUCT_COD_AMRSTATE) //40 short
#define STRUCT_COD_AMRSTATE_SPEECH_ADDR                     (120 + STRUCT_COD_AMRSTATE) //40 short
#define STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR                 (160 + STRUCT_COD_AMRSTATE) //161 short
#define STRUCT_COD_AMRSTATE_OLD_WSP_ADDR                    (321 + STRUCT_COD_AMRSTATE) //143 short
#define STRUCT_COD_AMRSTATE_WSP_ADDR                        (464 + STRUCT_COD_AMRSTATE) //160 short
#define STRUCT_COD_AMRSTATE_OLD_LAGS_ADDR                   (624 + STRUCT_COD_AMRSTATE) //6 short
#define STRUCT_COD_AMRSTATE_OL_GAIN_FLG_ADDR                (630 + STRUCT_COD_AMRSTATE) //2 short
#define STRUCT_COD_AMRSTATE_OLD_EXC_ADDR                    (632 + STRUCT_COD_AMRSTATE) //154 short
#define STRUCT_COD_AMRSTATE_EXC_ADDR                        (786 + STRUCT_COD_AMRSTATE) //161 short
#define STRUCT_COD_AMRSTATE_AI_ZERO_ADDR                    (947 + STRUCT_COD_AMRSTATE) //11 short
#define STRUCT_COD_AMRSTATE_ZERO_ADDR                       (958 + STRUCT_COD_AMRSTATE) //41 short

#define STRUCT_LEVINSONSTATE_ADDR                           (2183 + AMR_GLOBAL_X_BEGIN_ADDR) //11 short
#define STRUCT_LSPSTATE_ADDR                                (2194 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_LSPSTATE_LSP_OLD_Q_ADDR                      (2204 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_Q_PLSFSTATE_ADDR                             (2214 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_CLLTPSTATE_ADDR                              (2224 + AMR_GLOBAL_X_BEGIN_ADDR) //18 short
#define STRUCT_GC_PREDST_ADDR                               (2242 + AMR_GLOBAL_X_BEGIN_ADDR) //30 short
//STRUCT_TONSTABSTATE
#define STRUCT_TONSTABSTATE                                 (2272 + AMR_GLOBAL_X_BEGIN_ADDR) //struct
#define STRUCT_TONSTABSTATE_ADDR                            (0 + STRUCT_TONSTABSTATE) //7 short
#define STRUCT_TONSTABSTATE_COUNT_ADDR                      (7 + STRUCT_TONSTABSTATE) //1 short

#define STRUCT_VADSTATE1                                    (2280 + AMR_GLOBAL_X_BEGIN_ADDR) //70 short
//STRUCT_DTX_ENCSTATE
#define STRUCT_DTX_ENCSTATE                                 (2350 + AMR_GLOBAL_X_BEGIN_ADDR) //struct
#define STRUCT_DTX_ENCSTATE_ADDR                            (0 + STRUCT_DTX_ENCSTATE) //80 short
#define STRUCT_DTX_ENCSTATE_LOG_EN_HIST_ADDR                (80 + STRUCT_DTX_ENCSTATE) //8 short
#define STRUCT_DTX_ENCSTATE_HIST_PTR_ADDR                   (88 + STRUCT_DTX_ENCSTATE) //1 short
#define STRUCT_DTX_ENCSTATE_LOG_EN_INDEX_ADDR               (89 + STRUCT_DTX_ENCSTATE) //1 short
#define STRUCT_DTX_ENCSTATE_INIT_LSF_VQ_INDEX_ADDR          (90 + STRUCT_DTX_ENCSTATE) //2 short
#define STRUCT_DTX_ENCSTATE_LSP_INDEX_ADDR                  (92 + STRUCT_DTX_ENCSTATE) //4 short
#define STRUCT_DTX_ENCSTATE_DTXHANGOVERCOUNT_ADDR           (96 + STRUCT_DTX_ENCSTATE) //1 short
#define STRUCT_DTX_ENCSTATE_DECANAELAPSEDCOUNT_ADDR         (97 + STRUCT_DTX_ENCSTATE) //1 short

#define STRUCT_COD_AMRSTATE_MEM_SYN_ADDR                    (2448 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_MEM_W0_ADDR                     (2458 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_MEM_W_ADDR                      (2468 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_MEM_ERROR_ADDR                  (2478 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_ERROR_ADDR                      (2488 + AMR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_SHARP_ADDR                      (2528 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
//AMR_LOCAL_VARS_X_LEVEL0_START_ADDR
#define AMR_LOCAL_VARS_X_LEVEL0_START_ADDR                  (2530 + AMR_GLOBAL_X_BEGIN_ADDR) //struct
#define BEGIN_OF_COD_AMR                                    (0 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //40 short
#define COD_AMR_XN2_ADDRESS                                 (40 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //40 short
#define COD_AMR_Y1_ADDRESS                                  (80 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //40 short
#define COD_AMR_Y2_ADDRESS                                  (120 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //40 short
#define COD_AMR_RES_ADDRESS                                 (160 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //40 short
#define COD_AMR_MEM_SYN_SAVE_ADDRESS                        (200 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //10 short
#define COD_AMR_MEM_W0_SAVE_ADDRESS                         (210 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //10 short
#define COD_AMR_MEM_ERR_SAVE_ADDRESS                        (220 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //10 short
#define COD_AMR_GCOEFF_ADDRESS                              (230 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //4 short
#define COD_AMR_SUBFRNR_ADDRESS                             (234 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //1 short
#define COD_AMR_I_SUBFR_ADDRESS                             (235 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //1 short
#define COD_AMR_T_OP_ADDRESS                                (236 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //2 short
#define COD_AMR_T0_ADDRESS                                  (238 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //1 short
#define COD_AMR_T0_FRAC_ADDRESS                             (239 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //1 short
#define COD_AMR_GAIN_PIT_ADDRESS                            (240 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //1 short
#define COD_AMR_GAIN_CODE_ADDRESS                           (241 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //1 short
#define COD_AMR_EXC_ADDR_ADDRESS                            (242 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //2 short
#define COD_AMR_LSP_FLAG_ADDRESS                            (244 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //1 short
#define COD_AMR_GP_LIMIT_ADDRESS                            (245 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //1 short
#define COD_AMR_VAD_FLAG_ADDRESS                            (246 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //1 short
#define COD_AMR_COMPUTE_SID_FLAG_ADDRESS                    (247 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //1 short
#define COD_AMR_A_T_ADDRESS                                 (248 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //44 short
#define COD_AMR_AQ_T_ADDRESS                                (292 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //44 short
#define COD_AMR_LSP_NEW_ADDRESS                             (336 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //10 short
#define COD_AMR_SHARP_SAVE_ADDRESS                          (346 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //1 short
#define COD_AMR_EVENSUBFR_ADDRESS                           (347 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //1 short
#define COD_AMR_CODE_ADDRESS                                (348 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //40 short

#define AMR_LOCAL_VARS_X_LEVEL1_START_ADDR                  ((388+AMR_LOCAL_VARS_X_LEVEL0_START_ADDR)) //alias
//SET_SIGN_X
#define SET_SIGN_X                                          (2918 + AMR_GLOBAL_X_BEGIN_ADDR) //struct
#define SEARCH_10I40_RRV_ADDR                               (0 + SET_SIGN_X) //40 short
#define SEARCH_CODE_CP_RR_ADDR                              (40 + SET_SIGN_X) //320 short
#define SEARCH_CODE_PS0_TEMP_ADDR                           (360 + SET_SIGN_X) //1 short
#define SEARCH_10I40_PS0_ADDR                               (361 + SET_SIGN_X) //1 short
#define C1035PF_IPOS_ADDR                                   (362 + SET_SIGN_X) //10 short
#define C1035PF_POS_MAX_ADDR                                (372 + SET_SIGN_X) //10 short
#define C1035PF_CODVEC_ADDR                                 (382 + SET_SIGN_X) //10 short
#define C1035PF_SIGN_ADDR                                   (392 + SET_SIGN_X) //40 short

#define AZ_LSP_IP_ADDR                                      ((0+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define Q_plsf_5_X                                          ((0+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define Q_PLSF_5_LSF1_ADDR                                  ((0 + Q_plsf_5_X)) //alias
#define Q_PLSF_5_LSF2_ADDR                                  ((10 + Q_plsf_5_X)) //alias
#define Q_PLSF_5_WF1_ADDR                                   ((20 + Q_plsf_5_X)) //alias
#define Q_PLSF_5_WF2_ADDR                                   ((30 + Q_plsf_5_X)) //alias
#define Q_PLSF_5_LSF_R1_ADDR                                ((40 + Q_plsf_5_X)) //alias
#define Q_PLSF_5_LSF_R2_ADDR                                ((50 + Q_plsf_5_X)) //alias
#define Q_PLSF_5_LSF1_Q_ADDR                                ((60 + Q_plsf_5_X)) //alias
#define Q_PLSF_5_LSF2_Q_ADDR                                ((70 + Q_plsf_5_X)) //alias
#define CONST_inverse_Zig_Zag_ADDR                          (3350 + AMR_GLOBAL_X_BEGIN_ADDR) //64 short
#define CONST_128_ADDR                                      (3414 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_256_ADDR                                      (3415 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_neg3_ADDR                                     (3416 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_143_ADDR                                      (3417 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_160_ADDR                                      (3418 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_26214_ADDR                                    (3419 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_205_ADDR                                      (3420 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_0x199a_ADDR                                   (3421 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_640_INT_ADDR                                  (3422 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_640BY8_INT_ADDR                               (3423 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_640BY9_INT_ADDR                               (3424 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_479_ADDR                                      (3425 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_640_ADDR                                      (3426 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define CONST_0X80_ADDR                                     (3428 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define CONST_0x00010001_ADDR                               (3430 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define CONST_0x00020002_ADDR                               (3432 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define CONST_0x0000FFFF_ADDR                               (3434 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define CONST_0xFFFF_ADDR                                   (CONST_0x0000FFFF_ADDR) //alias
#define CONST_0x0000ffffL_ADDR                              (CONST_0x0000FFFF_ADDR) //alias
#define CONST_0x00080008_ADDR                               (3436 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define CONST_0x40000000_ADDR                               (3438 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define CONST_0x7FFFFFFF_ADDR                               (3440 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define CONST_0x80000000_ADDR                               (3442 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define CONST_0x8000FFFF_ADDR                               (3444 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define CONST_TAB244_ADDR                                   (3446 + AMR_GLOBAL_X_BEGIN_ADDR) //244 short
#define CONST_BLOCKSIZE_BYTE_ADDR                           (3690 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define CONST_WIDTH_ADD8                                    (3692 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_WIDTH_SUB8                                    (3693 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_WIDTH_MPY8                                    (3694 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_WIDTH_MPY8_ADD8                               (3695 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEGENC_BUFF_Y_ADDR                                 (3696 + AMR_GLOBAL_X_BEGIN_ADDR) //64 short
#define JPEGENC_BUFF_Y1_ADDR                                (3760 + AMR_GLOBAL_X_BEGIN_ADDR) //64 short
#define JPEGENC_BUFF_Y2_ADDR                                (3824 + AMR_GLOBAL_X_BEGIN_ADDR) //64 short
#define JPEGENC_BUFF_Y3_ADDR                                (3888 + AMR_GLOBAL_X_BEGIN_ADDR) //64 short
#define JPEGENC_BUFF_CB_ADDR                                (3952 + AMR_GLOBAL_X_BEGIN_ADDR) //64 short
#define JPEGENC_BUFF_CR_ADDR                                (4016 + AMR_GLOBAL_X_BEGIN_ADDR) //64 short
#define GLOBAL_4WIDTH_ADDR                                  (4080 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_OLD_DCY_ADDR                                 (4081 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_OLD_DCU_ADDR                                 (4082 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_OLD_DCV_ADDR                                 (4083 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_SHORT_1_ADDR                                 (4084 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_CURR_POS_ADDR                                (4085 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_CURRBITS_ADDR                                (4086 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_CURRBYTE_ADDR                                (4087 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_pp_ADDR                                      (4088 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_Q_ADDR                                       (4089 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEGENC_BUFF_RGB_640_m1_ADDR                        (4090 + AMR_GLOBAL_X_BEGIN_ADDR) //248 short
#define JPEGENC_BUFF_RGB_ADDR                               (4338 + AMR_GLOBAL_X_BEGIN_ADDR) //640 short
#define JPEGENC_BUFF_RGB_640_1_ADDR                         (4978 + AMR_GLOBAL_X_BEGIN_ADDR) //640 short
#define JPEGENC_BUFF_RGB_640_2_ADDR                         (5618 + AMR_GLOBAL_X_BEGIN_ADDR) //640 short
#define JPEGENC_BUFF_RGB_640_3_ADDR                         (6258 + AMR_GLOBAL_X_BEGIN_ADDR) //640 short
#define JPEGENC_BUFF_RGB_640_4_ADDR                         (6898 + AMR_GLOBAL_X_BEGIN_ADDR) //640 short
#define JPEGENC_BUFF_RGB_640_5_ADDR                         (7538 + AMR_GLOBAL_X_BEGIN_ADDR) //640 short
#define JPEGENC_BUFF_RGB_640_6_ADDR                         (8178 + AMR_GLOBAL_X_BEGIN_ADDR) //640 short
#define JPEGENC_BUFF_RGB_640_7_ADDR                         (8818 + AMR_GLOBAL_X_BEGIN_ADDR) //640 short
#define JPEGENC_BUFF_RGB_640_8_ADDR                         (9458 + AMR_GLOBAL_X_BEGIN_ADDR) //640 short
#define GLOBAL_TempZommBuf                                  (JPEGENC_BUFF_RGB_640_m1_ADDR) //alias
#define GLOBAL_ORG_END_ADDR                                 (JPEGENC_BUFF_RGB_ADDR+4224) //alias
#define GLOBAL_ORGI_CONST_ADDR                              (GLOBAL_ORG_END_ADDR) //alias
#define GLOBAL_ORGI_NUM_1_ADDR                              (GLOBAL_ORGI_CONST_ADDR) //alias
#define GLOBAL_ORGI_DMASIZE_ADDR                            (GLOBAL_ORGI_CONST_ADDR+1) //alias
#define GLOBAL_ORGI_DMAINC_ADDR                             (GLOBAL_ORGI_CONST_ADDR+2) //alias
#define GLOBAL_SCALEX_SIZE_ADDR                             (GLOBAL_ORGI_CONST_ADDR+4) //alias
#define GLOBAL_SCALEY_SIZE_ADDR                             (GLOBAL_ORGI_CONST_ADDR+6) //alias
#define GLOBAL_MASK_ADDR                                    (GLOBAL_ORGI_CONST_ADDR+8) //alias
#define CONST_0XFF00FF_ADDR                                 (GLOBAL_ORGI_CONST_ADDR+10) //alias
#define GLOBAL_NUM1_ADDR                                    (GLOBAL_ORGI_CONST_ADDR +12) //alias
#define GLOBAL_NUM3_ADDR                                    (GLOBAL_ORGI_CONST_ADDR +13) //alias
#define GLOBAL_P1_ADDR                                      (GLOBAL_ORGI_CONST_ADDR +14) //alias
#define GLOBAL_P2_ADDR                                      (GLOBAL_ORGI_CONST_ADDR +15) //alias
#define GLOBAL_INPUT_ADDR_ADDR                              (GLOBAL_ORGI_CONST_ADDR + 16) //alias
#define GLOBAL_INPUT_END_ADDR_ADDR                          (GLOBAL_ORGI_CONST_ADDR + 17) //alias
#define GLOBAL_FlagForAverage_ADDR                          (GLOBAL_ORGI_CONST_ADDR + 18) //alias
#define GLOBAL_ORGI_SOURCE_ADDR                             (GLOBAL_ORGI_CONST_ADDR + 20) //alias
#define AMR_EFR_RESET_Offset_X                              (144) //alias
#define STRUCT_COD_AMRSTATE_OLD_SPEECH_ADDR                 (STRUCT_COD_AMRSTATE_ADDR) //alias
#define STRUCT_LEVINSONSTATE_OLD_A_ADDR                     (STRUCT_LEVINSONSTATE_ADDR) //alias
#define STRUCT_LSPSTATE_LSP_OLD_ADDR                        (STRUCT_LSPSTATE_ADDR) //alias
#define STRUCT_Q_PLSFSTATE_PAST_RQ_ADDR                     (STRUCT_Q_PLSFSTATE_ADDR) //alias
#define STRUCT_PITCH_FRSTATE_T0_PREV_SUBFRAME_ADDR          (STRUCT_CLLTPSTATE_ADDR) //alias
#define STRUCT_TONSTABSTATE_GP_ADDR                         (STRUCT_TONSTABSTATE_ADDR) //alias
#define STRUCT_DTX_ENCSTATE_LSP_HIST_ADDR                   (STRUCT_DTX_ENCSTATE_ADDR) //alias
#define COD_AMR_XN_ADDRESS                                  (BEGIN_OF_COD_AMR) //alias
#define Pitch_fr6_max_loop_addr                             (AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) //alias
#define Pitch_fr6_t0_min_addr                               ((2+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define Pitch_delta_search_addr                             ((4+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define Pitch_fr6_corr_v_addr                               ((200+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define COR_H_H2_ADDR                                       (AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) //alias
#define COR_H_X_Y32_ADDR                                    (AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) //alias
#define build_code_sign_ADDR                                (AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) //alias
#define cbsearch_dn_sign_ADDR                               (C1035PF_SIGN_ADDR) //alias
#define SET_SIGN_EN_ADDR                                    (AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) //alias
#define C1035PF_IPOS_D_ADDR                                 (C1035PF_IPOS_ADDR) //alias
#define DEC_TEMP_BEGIN_ADDR                                 ((10+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define BIT_SERIAL_ADDR_RAM                                 ((20+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define AUTOCORR_Y_ADDR                                     ((2+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define LEVINSON_ANL_ADDR                                   ((242+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define LSP_LSP_NEW_Q_ADDR                                  ((200+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define LSP_LSP_MID_ADDR                                    ((210+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define LSP_LSP_MID_Q_ADDR                                  ((220+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define AZ_LSP_F1_ADDR                                      ((40+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define AZ_LSP_F2_ADDR                                      ((50+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define AZ_LSP_F1_1_ADDR                                    ((1+AZ_LSP_F1_ADDR)) //alias
#define AZ_LSP_F2_1_ADDR                                    ((1+AZ_LSP_F2_ADDR)) //alias
#define PRE_BIG_AP1_ADDR                                    ((10+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define PRE_BIG_AP2_ADDR                                    ((30+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define PITCH_OL_M122_SCALED_SIGNAL2_ADDR                   ((0+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define LPC_RLOW_ADDR                                       ((300+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define LPC_RHIGH_ADDR                                      ((312+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define LPC_RC_ADDR                                         ((330+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)) //alias
#define CONST_1_ADDR                                        (1 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_2_ADDR                                        (5 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_3_ADDR                                        (6 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_4_ADDR                                        (14 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_5_ADDR                                        (15 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_6_ADDR                                        (27 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_7_ADDR                                        (28 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_8_ADDR                                        (2 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_9_ADDR                                        (4 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_10_ADDR                                       (7 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_11_ADDR                                       (13 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_12_ADDR                                       (16 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_13_ADDR                                       (26 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_14_ADDR                                       (29 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_15_ADDR                                       (42 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_16_ADDR                                       (3 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_17_ADDR                                       (8 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_18_ADDR                                       (12 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_19_ADDR                                       (17 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_20_ADDR                                       (25 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_21_ADDR                                       (30 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_22_ADDR                                       (41 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_23_ADDR                                       (43 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_24_ADDR                                       (9 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_25_ADDR                                       (11 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_26_ADDR                                       (18 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_27_ADDR                                       (24 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_28_ADDR                                       (31 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_29_ADDR                                       (40 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_30_ADDR                                       (44 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_31_ADDR                                       (53 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_32_ADDR                                       (10 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_33_ADDR                                       (19 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_34_ADDR                                       (23 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_35_ADDR                                       (32 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_36_ADDR                                       (39 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_37_ADDR                                       (45 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_38_ADDR                                       (52 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_39_ADDR                                       (54 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_40_ADDR                                       (20 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_41_ADDR                                       (22 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_42_ADDR                                       (33 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_43_ADDR                                       (38 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_44_ADDR                                       (46 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_45_ADDR                                       (51 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_46_ADDR                                       (55 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_47_ADDR                                       (60 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_48_ADDR                                       (21 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_49_ADDR                                       (34 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_50_ADDR                                       (37 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_51_ADDR                                       (47 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_52_ADDR                                       (50 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_53_ADDR                                       (56 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_54_ADDR                                       (59 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_55_ADDR                                       (61 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_56_ADDR                                       (35 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_57_ADDR                                       (36 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_58_ADDR                                       (48 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_59_ADDR                                       (49 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_60_ADDR                                       (57 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_61_ADDR                                       (58 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_62_ADDR                                       (62 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_63_ADDR                                       (63 + CONST_inverse_Zig_Zag_ADDR) //alias
#define CONST_0x40000000L_ADDR                              (CONST_0x40000000_ADDR) //alias
#define CONST_0x7fffffff_ADDR                               (CONST_0x7FFFFFFF_ADDR) //alias
#define CONST_RX_LOST_SID_FRAME_ADDR                        (CONST_16_ADDR) //alias
#define CONST_RX_INVALID_SID_FRAME_ADDR                     (CONST_32_ADDR) //alias
#define CONST_0x00000001_ADDR                               (CONST_1_ADDR) //alias
#define TABLE_hufftab0_ADDR                                 (CONST_0x00000001_ADDR) //alias
#define CONST_0x0010_ADDR                                   (CONST_16_ADDR) //alias
#define CONST_0x0011_ADDR                                   (CONST_17_ADDR) //alias
#define CONST_0x0020_ADDR                                   (CONST_32_ADDR) //alias

//GLOBAL_VARS
#define GLOBAL_RESET_FLAG_ADDR                              (0 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RELOAD_RESET_FLAG_ADDR                       (1 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define AMR_GLOBAL_VARS_Y_START_ADDR                        (2 + AMR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_H1_ADDR                         (42 + AMR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define AMR_LOCAL_VARS_Y_LEVEL0_START_ADDR                  ((40+STRUCT_COD_AMRSTATE_H1_ADDR)) //alias
#define COD_AMR_H1_SF0_ADDRESS                              (82 + AMR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define COD_AMR_ANA_ADDR                                    (122 + AMR_GLOBAL_Y_BEGIN_ADDR) //60 short
//AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR
#define AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR                  (182 + AMR_GLOBAL_Y_BEGIN_ADDR) //struct
#define C1035PF_DN_ADDR                                     (0 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //40 short
#define C1035PF_RR_DIAG_ADDR                                (40 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //40 short
#define C1035PF_RR_SIDE_ADDR                                (80 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //780 short
#define SEARCH_CODE_RRV_COE_ADDR                            (860 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_CODE_RR_COE_ADDR                             (861 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_10I40_I_ADDR                                 (862 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //2 short
#define SEARCH_10I40_PSK_ADDR                               (864 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_10I40_ALPK_ADDR                              (865 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define C1035PF_RR_DIAG_ADDR_2_ADDR                         (866 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_CODE_CP_RR_ADDR_ADDR                         (867 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_CODE_DN_ADDR_ADDR                            (868 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_CODE_RRV_ADDR_ADDR                           (869 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define C1035PF_RR_SIDE_ADDR_ADDR                           (870 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define C1035PF_RR_DIAG_ADDR_ADDR                           (871 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_CODE_CP_RR_7_ADDR_ADDR                       (872 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define RR_SIDE_DIAG_TEMP_ADDR                              (873 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_10I40_I2_TEMP_ADDR                           (874 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_10I40_I2I3_TEMP_ADDR                         (875 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_CODE_DN_RRV_ADDR_ADDR                        (876 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //2 short
#define SEARCH_CODE_ALP0_TEMP_ADDR                          (878 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //2 short
#define SEARCH_10I40_I0_ADDR                                (880 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_10I40_I1_ADDR                                (881 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_10I40_I2_ADDR                                (882 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_10I40_I3_ADDR                                (883 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_10I40_I4_ADDR                                (884 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_10I40_I5_ADDR                                (885 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_10I40_I6_ADDR                                (886 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_10I40_I7_ADDR                                (887 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_10I40_I8_ADDR                                (888 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define SEARCH_10I40_I9_ADDR                                (889 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //1 short
#define linear_signs_ADDR                                   (890 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //4 short
#define linear_codewords_ADDR                               (894 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //8 short

#define Pitch_ol_M122                                       ((0+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)) //alias
#define PITCH_OL_M122_SCALED_SIGNAL_ADDR                    ((0 + Pitch_ol_M122)) //alias
#define PITCH_OL_M122_P_MAX1_ADDR                           ((310 + Pitch_ol_M122)) //alias
#define PITCH_OL_M122_MAX1_ADDR                             ((311 + Pitch_ol_M122)) //alias
#define PITCH_OL_M122_P_MAX2_ADDR                           ((312 + Pitch_ol_M122)) //alias
#define PITCH_OL_M122_MAX2_ADDR                             ((313 + Pitch_ol_M122)) //alias
#define PITCH_OL_M122_P_MAX3_ADDR                           ((314 + Pitch_ol_M122)) //alias
#define PITCH_OL_M122_MAX3_ADDR                             ((315 + Pitch_ol_M122)) //alias
#define PITCH_OL_M122_SCAL_FAC_ADDR                         ((316 + Pitch_ol_M122)) //alias
#define PITCH_OL_M122_CORR_ADDR                             ((322 + Pitch_ol_M122)) //alias
#define EFR_GLOBAL_AMR_LOCAL                                ((80+AMR_GLOBAL_VARS_Y_START_ADDR)) //alias
#define COD_AMR_CODE_SF0_ADDRESS                            ((0 + EFR_GLOBAL_AMR_LOCAL)) //alias
#define COD_AMR_XN_SF0_ADDRESS                              ((42 + EFR_GLOBAL_AMR_LOCAL)) //alias
#define COD_AMR_Y2_SF0_ADDRESS                              ((82 + EFR_GLOBAL_AMR_LOCAL)) //alias
#define COD_AMR_T0_SF0_ADDRESS                              ((122 + EFR_GLOBAL_AMR_LOCAL)) //alias
#define COD_AMR_T0_FRAC_SF0_ADDRESS                         ((123 + EFR_GLOBAL_AMR_LOCAL)) //alias
#define COD_AMR_I_SUBFR_SF0_ADDRESS                         ((124 + EFR_GLOBAL_AMR_LOCAL)) //alias
#define COD_AMR_GAIN_PIT_SF0_ADDRESS                        ((126 + EFR_GLOBAL_AMR_LOCAL)) //alias
#define COD_AMR_GAIN_CODE_SF0_ADDRESS                       ((127 + EFR_GLOBAL_AMR_LOCAL)) //alias
#define CONST32_ADDR                                        ((linear_codewords_ADDR +8)) //alias
#define CONST_D_1_4_ADDR                                    (1084 + AMR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define CONST_D_1_8_ADDR                                    (1086 + AMR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define CONST_D_1_16_ADDR                                   (1088 + AMR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define CONST_D_1_32_ADDR                                   (1090 + AMR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define CONST_D_1_64_ADDR                                   (1092 + AMR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define JPEGENC_MYDCT_ADDR                                  (1094 + AMR_GLOBAL_Y_BEGIN_ADDR) //12 short
#define CONST_MYQUN_TAB0_ADDR                               (1106 + AMR_GLOBAL_Y_BEGIN_ADDR) //128 short
#define CONST_MYQUN_TAB1_ADDR                               (1234 + AMR_GLOBAL_Y_BEGIN_ADDR) //128 short
#define CONST_MYHUFF_DC0_ADDR                               (1362 + AMR_GLOBAL_Y_BEGIN_ADDR) //24 short
#define CONST_MYHUFF_DC1_ADDR                               (1386 + AMR_GLOBAL_Y_BEGIN_ADDR) //24 short
#define CONST_MYHUFF_AC0_ADDR                               (1410 + AMR_GLOBAL_Y_BEGIN_ADDR) //324 short
#define CONST_MYHUFF_AC1_ADDR                               (1734 + AMR_GLOBAL_Y_BEGIN_ADDR) //324 short
#define TABLE_WINDOW_160_80_ADDR                            (2058 + AMR_GLOBAL_Y_BEGIN_ADDR) //240 short
#define TABLE_WINDOW_232_8_ADDR                             (2298 + AMR_GLOBAL_Y_BEGIN_ADDR) //240 short
#define TABLE_MEAN_LSF_ADDR                                 (2538 + AMR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define TABLE_SLOPE_ADDR                                    (2548 + AMR_GLOBAL_Y_BEGIN_ADDR) //64 short
#define TABLE_LSP_LSF_ADDR                                  (2612 + AMR_GLOBAL_Y_BEGIN_ADDR) //66 short
#define TABLE_LOG2_ADDR                                     (2678 + AMR_GLOBAL_Y_BEGIN_ADDR) //34 short
#define TABLE_LAG_L_ADDR                                    (2712 + AMR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define TABLE_LAG_H_ADDR                                    (2722 + AMR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define TABLE_INV_SQRT_ADDR                                 (2732 + AMR_GLOBAL_Y_BEGIN_ADDR) //50 short
#define TABLE_GRID_ADDR                                     (2782 + AMR_GLOBAL_Y_BEGIN_ADDR) //62 short
#define TABLE_POW2_ADDR                                     (2844 + AMR_GLOBAL_Y_BEGIN_ADDR) //34 short
#define STATIC_CONST_QUA_GAIN_PITCH_ADDR                    (2878 + AMR_GLOBAL_Y_BEGIN_ADDR) //16 short
#define STATIC_CONST_INTER_6_ADDR                           (2894 + AMR_GLOBAL_Y_BEGIN_ADDR) //62 short
#define STATIC_CONST_INTER_6_25_ADDR                        (2956 + AMR_GLOBAL_Y_BEGIN_ADDR) //26 short
#define STATIC_CONST_F_GAMMA2_ADDR                          (2982 + AMR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define STATIC_CONST_F_GAMMA1_ADDR                          (2992 + AMR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define STATIC_CONST_GRAY_ADDR                              (3002 + AMR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define TABLE_DICO1_LSF_ADDR                                (3010 + AMR_GLOBAL_Y_BEGIN_ADDR) //512 short
#define TABLE_DICO2_LSF_ADDR                                (3522 + AMR_GLOBAL_Y_BEGIN_ADDR) //1024 short
#define TABLE_DICO3_LSF_ADDR                                (4546 + AMR_GLOBAL_Y_BEGIN_ADDR) //1024 short
#define TABLE_DICO4_LSF_ADDR                                (5570 + AMR_GLOBAL_Y_BEGIN_ADDR) //1024 short
#define TABLE_DICO5_LSF_ADDR                                (6594 + AMR_GLOBAL_Y_BEGIN_ADDR) //256 short
#define STATIC_CONST_BITNO_ADDR                             (6850 + AMR_GLOBAL_Y_BEGIN_ADDR) //30 short
#define STATIC_CONST_qua_gain_code_ADDR                     (6880 + AMR_GLOBAL_Y_BEGIN_ADDR) //96 short
#define STATIC_CONST_gamma1_ADDR                            (6976 + AMR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define STATIC_CONST_pred_MR122_ADDR                        (6986 + AMR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define AMR_ENC_RESET_TABLE_ADDR                            (6990 + AMR_GLOBAL_Y_BEGIN_ADDR) //202 short
#define MATRIC_RGB_ADDR                                     (7192 + AMR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_OUT_FOR1280BY1024_ADDR                       (7200 + AMR_GLOBAL_Y_BEGIN_ADDR) //48 short
#define GLOBAL_OUT_48_1_ADDR                                (7248 + AMR_GLOBAL_Y_BEGIN_ADDR) //48 short
#define GLOBAL_OUT_48_2_ADDR                                (7296 + AMR_GLOBAL_Y_BEGIN_ADDR) //48 short
#define GLOBAL_OUT_48_3_ADDR                                (7344 + AMR_GLOBAL_Y_BEGIN_ADDR) //48 short
#define GLOBAL_OUT_48_4_ADDR                                (7392 + AMR_GLOBAL_Y_BEGIN_ADDR) //48 short
#define GLOBAL_OUT_48_5_ADDR                                (7440 + AMR_GLOBAL_Y_BEGIN_ADDR) //48 short
#define GLOBAL_OUT_48_6_ADDR                                (7488 + AMR_GLOBAL_Y_BEGIN_ADDR) //48 short
#define GLOBAL_OUT_48_7_ADDR                                (7536 + AMR_GLOBAL_Y_BEGIN_ADDR) //48 short
#define GLOBAL_TEMP_BUF_ADDR                                (GLOBAL_OUT_FOR1280BY1024_ADDR) //alias
#define GLOBAL_TEMP_BUF2_ADDR                               (GLOBAL_TEMP_BUF_ADDR+352) //alias
#define GLOBAL_xScale                                       (7584 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_yScale                                       (7586 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_input_p_Y_Row                                (7588 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_input_p_Y_Colume                             (7590 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_input_p_UV_Row                               (7592 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_input_p_UV_Colume                            (7594 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_McuZommInRow                                 (7596 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_McuZoomINColume                              (7597 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DmaInputRows                                 (7598 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DmaInputColumes                              (7599 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_InputRowsOffset                              (7600 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_McuInputBuffAddr                             (7601 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DmaInputBufferDddr                           (7602 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_RGBToYUV422Coef                              (7604 + AMR_GLOBAL_Y_BEGIN_ADDR) //12 short
#define GLOBAL_OUT_32_1_ADDR                                (GLOBAL_OUT_FOR1280BY1024_ADDR+32) //alias
#define GLOBAL_OUT_32_2_ADDR                                (GLOBAL_OUT_32_1_ADDR+32) //alias
#define GLOBAL_OUT_32_3_ADDR                                (GLOBAL_OUT_32_2_ADDR+32) //alias
#define GLOBAL_OUT_32_4_ADDR                                (GLOBAL_OUT_32_3_ADDR+32) //alias
#define GLOBAL_OUT_32_5_ADDR                                (GLOBAL_OUT_32_4_ADDR+32) //alias
#define GLOBAL_OUT_32_6_ADDR                                (GLOBAL_OUT_32_5_ADDR+32) //alias
#define GLOBAL_OUT_32_7_ADDR                                (GLOBAL_OUT_32_6_ADDR+32) //alias
#define PITCH_F6_EXCF_ADDR                                  (AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //alias
#define G_PITCH_SCALED_Y1_ADDR                              (AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //alias
#define G_PITCH_Y1_ADDR                                     ((40+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)) //alias
#define COR_H_H2_COPY_ADDR                                  ((860+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)) //alias
#define COPY_C1035PF_SIGN_ADDR                              ((900+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)) //alias
#define DEC_AMR_SYNTH_ADDRESS                               ((300+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)) //alias
#define COD_AMR_SYNTH_ADDR                                  (DEC_AMR_SYNTH_ADDRESS) //alias
#define F1_ADDR                                             ((10+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)) //alias
#define F2_ADDR                                             ((22+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)) //alias
#define SYN_FILT_TMP_ADDR                                   (AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //alias
#define INT_LPC_LSP_ADDR                                    (AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //alias
#define BUILD_CODE_POINT_ADDR_ADDR                          (AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //alias
#define AUTOCORR_Y_COPY_ADDR                                ((4+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)) //alias
#define LEVINSON_AL_ADDR                                    ((4+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)) //alias
#define PARA4_ADDR_ADDR                                     ((3+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)) //alias
#define CHEBPS_B2_ADDR                                      ((10+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)) //alias
#define Q_PLSF_5_LSF_P_ADDR                                 ((0+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)) //alias
#define CONST_0_ADDR                                        (25 + STATIC_CONST_INTER_6_25_ADDR) //alias
#define CONST_0x2000_ADDR                                   (CONST_MYQUN_TAB1_ADDR) //alias
#define CONST_8192_ADDR_Y                                   (CONST_MYQUN_TAB1_ADDR) //alias
#define CONST_16384_ADDR                                    (1 + CONST_MYQUN_TAB1_ADDR) //alias
#define CONST_0x4000_ADDR                                   (CONST_16384_ADDR) //alias
#define CONST_0x1000_ADDR                                   (15 + CONST_MYQUN_TAB1_ADDR) //alias
#define CONST_4096_ADDR                                     (CONST_0x1000_ADDR) //alias
#define CONST_1024_ADDR                                     (CONST_MYQUN_TAB0_ADDR) //alias
#define STATIC_CONST_gray_ADDR                              (STATIC_CONST_GRAY_ADDR) //alias
#define STATIC_CONST_gamma2_ADDR                            (STATIC_CONST_F_GAMMA2_ADDR) //alias
#define STATIC_CONST_gamma1_12K2_ADDR                       (STATIC_CONST_F_GAMMA1_ADDR) //alias
#define CONST_1_16_ADDR                                     (CONST_D_1_16_ADDR) //alias
#define CONST_15565_ADDR                                    (( 10+ STATIC_CONST_QUA_GAIN_PITCH_ADDR )) //alias
#define CONST_13926_ADDR                                    (( 8+ STATIC_CONST_QUA_GAIN_PITCH_ADDR )) //alias
#define CONST_3277_ADDR                                     (( 1+ STATIC_CONST_QUA_GAIN_PITCH_ADDR )) //alias
#define CONST_0x87_ADDR                                     (( 33+ STATIC_CONST_INTER_6_ADDR )) //alias
#define CONST_9830_ADDR                                     (( 4+ STATIC_CONST_QUA_GAIN_PITCH_ADDR )) //alias
#define CONST_14746_ADDR                                    (( 9+ STATIC_CONST_QUA_GAIN_PITCH_ADDR )) //alias
#define CONST_0x3333_ADDR                                   ((7+ STATIC_CONST_QUA_GAIN_PITCH_ADDR)) //alias
#define CONST_0x7fc5_ADDR                                   ((225+ TABLE_WINDOW_232_8_ADDR)) //alias
#define CONST_1000_ADDR                                     ((344+TABLE_DICO1_LSF_ADDR)) //alias
#define CONST_80_ADDR                                       ((316+TABLE_DICO1_LSF_ADDR)) //alias
#define CONST_512_ADDR                                      ((252+TABLE_DICO1_LSF_ADDR)) //alias
#define CONST_0x5a82_ADDR                                   ((16+ TABLE_LSP_LSF_ADDR)) //alias
#define CONST_0x00ff_ADDR                                   ((484+ TABLE_DICO3_LSF_ADDR)) //alias
#define CONST_572_ADDR                                      ((173+ TABLE_DICO5_LSF_ADDR )) //alias
#define CONST_500_ADDR                                      ((356 + TABLE_DICO1_LSF_ADDR)) //alias
#define CONST_1152_ADDR                                     ((620+ TABLE_DICO2_LSF_ADDR)) //alias
#define CONST_0xfff8_ADDR                                   ((104 + TABLE_DICO1_LSF_ADDR)) //alias
#define CONST_120_ADDR                                      ((440+ TABLE_DICO2_LSF_ADDR)) //alias
#define CONST_575_ADDR                                      ((460+ TABLE_DICO2_LSF_ADDR)) //alias
#define CONST_ALPHA_UP2_ADDR                                ((540 + TABLE_DICO2_LSF_ADDR)) //alias
#define CONST_124_ADDR                                      ((476+ TABLE_DICO1_LSF_ADDR)) //alias
#define CONST_1311_ADDR                                     ((814+ TABLE_DICO2_LSF_ADDR)) //alias
#define CONST_368_ADDR                                      ((222+ TABLE_DICO1_LSF_ADDR)) //alias
#define CONST_0x235_ADDR                                    ((247+ TABLE_DICO1_LSF_ADDR)) //alias
#define CONST_0x101_ADDR                                    ((877+ TABLE_DICO4_LSF_ADDR)) //alias
#define CONST_97_ADDR                                       ((394+ TABLE_DICO2_LSF_ADDR)) //alias
#define CONST_400_ADDR                                      ((648+ TABLE_DICO4_LSF_ADDR)) //alias
#define CONST_0x00004000L_ADDR                              ((48+TABLE_INV_SQRT_ADDR )) //alias
#define CONST_0x00007FFF_ADDR                               ((32+TABLE_LOG2_ADDR )) //alias
#define CONST_0x00008000_ADDR                               ((64+TABLE_LSP_LSF_ADDR )) //alias
#define CONST_WORD32_0_ADDR                                 (CONST_0_ADDR) //alias
#define CONST_0x7FFF_ADDR                                   (CONST_0x00007FFF_ADDR) //alias
#define CONST_0x8000_ADDR                                   (CONST_0x00008000_ADDR) //alias
#define CONST_0x00008000L_ADDR                              (CONST_0x00008000_ADDR) //alias
#define CONST_0x7fff_ADDR                                   (CONST_0x7FFF_ADDR) //alias
#define CONST_0x400_ADDR                                    (CONST_1024_ADDR) //alias
#define COSNT_1_2_ADDR                                      (CONST_0x00004000L_ADDR) //alias
#define CONST_0x00007fffL_ADDR                              (CONST_0x00007FFF_ADDR) //alias
#define CONST_NEG7_ADDR                                     (629 + JPEGENC_MYDCT_ADDR) //alias
#define CONST_0xff_ADDR                                     (CONST_0x00ff_ADDR) //alias

/*****************
 **  AMR_LOCAL  **
 *****************/

//apply_fdct
#define apply_fdct_Y_BEGIN_ADDR                             (96 + AMR_LOCAL_Y_BEGIN_ADDR)
#define TEMP_BUFF_64_ADDR                                   (0 + apply_fdct_Y_BEGIN_ADDR) //32 short

//COOLSAND_1280BY1024_JPEG
#define COOLSAND_1280BY1024_JPEG_Y_BEGIN_ADDR               (0 + AMR_LOCAL_Y_BEGIN_ADDR)
#define TEMP_BUFF_FOR1280BY1024_TMP1                        (0 + COOLSAND_1280BY1024_JPEG_Y_BEGIN_ADDR) //48 short
#define TEMP_BUFF_FOR1280BY1024_TMP2                        (48 + COOLSAND_1280BY1024_JPEG_Y_BEGIN_ADDR) //48 short

/************************
 **  YUVROTATE_GLOBAL  **
 ************************/

//GLOBAL_VARS
#define YUV_ROTATE_X_INPUT_ADDR                             (0 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 word
#define YUV_ROTATE_X_OUTPUT_YADDR                           (2 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 word
#define YUV_ROTATE_X_OUTPUT_UADDR                           (4 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 word
#define YUV_ROTATE_X_OUTPUT_VADDR                           (6 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 word
#define YUV_ROTATE_X_Srcwidth                               (8 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 short
#define YUV_ROTATE_X_Srcheight                              (9 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 short
#define YUV_ROTATE_X_Dstwidth                               (10 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 short
#define YUV_ROTATE_X_Dstheight                              (11 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 short
#define YUV_ROTATE_X_Direction                              (12 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 short
#define YUV_ROTATE_X_Stride                                 (13 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 short
#define YUV_ROTATE_X_xpos                                   (14 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 short
#define YUV_ROTATE_X_ypos                                   (15 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 short
#define YUV_ROTATE_X_mb_width                               (16 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 short
#define YUV_ROTATE_X_mb_height                              (17 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 short
#define YUV_ROTATE_X_MBA                                    (18 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 short
#define YUV_ROTATE_X_MBAmax                                 (19 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //1 short
#define YUV_ROTATE_X_BufY                                   (20 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //64 short
#define YUV_ROTATE_X_reserved                               (84 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //16 short
//GX_RGB_ROTATE_CFG_STRUCT
#define GX_RGB_ROTATE_CFG_STRUCT                            (100 + YUVROTATE_GLOBAL_X_BEGIN_ADDR) //struct
#define RGB_ROTATE_X_INPUT_ADDR                             (0 + GX_RGB_ROTATE_CFG_STRUCT) //1 word
#define RGB_ROTATE_X_OUTPUT_ADDR                            (2 + GX_RGB_ROTATE_CFG_STRUCT) //1 word
#define RGB_ROTATE_X_srcwidth                               (4 + GX_RGB_ROTATE_CFG_STRUCT) //1 short
#define RGB_ROTATE_X_srcheight                              (5 + GX_RGB_ROTATE_CFG_STRUCT) //1 short
#define RGB_ROTATE_X_dstwidth                               (6 + GX_RGB_ROTATE_CFG_STRUCT) //1 short
#define RGB_ROTATE_X_dstheight                              (7 + GX_RGB_ROTATE_CFG_STRUCT) //1 short
#define RGB_ROTATE_X_direction                              (8 + GX_RGB_ROTATE_CFG_STRUCT) //1 short
#define RGB_ROTATE_X_stride                                 (9 + GX_RGB_ROTATE_CFG_STRUCT) //1 short
#define RGB_ROTATE_X_xpos                                   (10 + GX_RGB_ROTATE_CFG_STRUCT) //1 short
#define RGB_ROTATE_X_ypos                                   (11 + GX_RGB_ROTATE_CFG_STRUCT) //1 short
#define RGB_ROTATE_X_mb_width                               (12 + GX_RGB_ROTATE_CFG_STRUCT) //1 short
#define RGB_ROTATE_X_mb_height                              (13 + GX_RGB_ROTATE_CFG_STRUCT) //1 short
#define RGB_ROTATE_X_MBA                                    (14 + GX_RGB_ROTATE_CFG_STRUCT) //1 short
#define RGB_ROTATE_X_MBAmax                                 (15 + GX_RGB_ROTATE_CFG_STRUCT) //1 short
#define RGB_ROTATE_X_out_buf                                (16 + GX_RGB_ROTATE_CFG_STRUCT) //64 short
#define RGB_ROTATE_X_reserved                               (80 + GX_RGB_ROTATE_CFG_STRUCT) //16 short


//GLOBAL_VARS
#define YUV_ROTATE_Y_IN_BUF                                 (0 + YUVROTATE_GLOBAL_Y_BEGIN_ADDR) //384 short
#define YUV_ROTATE_Y_BufU                                   (384 + YUVROTATE_GLOBAL_Y_BEGIN_ADDR) //20 short
#define YUV_ROTATE_Y_BufV                                   (404 + YUVROTATE_GLOBAL_Y_BEGIN_ADDR) //20 short
#define YUV_ROTATE_Y_const_17                               (424 + YUVROTATE_GLOBAL_Y_BEGIN_ADDR) //1 short
#define YUV_ROTATE_Y_const_65                               (425 + YUVROTATE_GLOBAL_Y_BEGIN_ADDR) //1 short
#define YUV_ROTATE_Y_const_1                                (426 + YUVROTATE_GLOBAL_Y_BEGIN_ADDR) //1 short
#define YUV_ROTATE_dma_in_flag                              (427 + YUVROTATE_GLOBAL_Y_BEGIN_ADDR) //1 short
//GY_RGB_ROTATE_CFG_STRUCT
#define GY_RGB_ROTATE_CFG_STRUCT                            (428 + YUVROTATE_GLOBAL_Y_BEGIN_ADDR) //struct
#define RGB_ROTATE_Y_in_block                               (0 + GY_RGB_ROTATE_CFG_STRUCT) //192 short
#define RGB_ROTATE_Y_dma_status                             (192 + GY_RGB_ROTATE_CFG_STRUCT) //1 short
#define RGB_ROTATE_Y_const_1                                (193 + GY_RGB_ROTATE_CFG_STRUCT) //1 short


/***********************
 **  YUVROTATE_LOCAL  **
 ***********************/

/********************
 **  COMMON_LOCAL  **
 ********************/

//RAM_X: size 0x2800, used 0x278c   RAM_Y: size 0x2800, used 0x2442

#endif
