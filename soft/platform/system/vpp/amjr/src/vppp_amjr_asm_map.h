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
#define COMMON_GLOBAL_Y_SIZE                                    56
#define JPEG_GLOBAL_X_SIZE                                      0
#define JPEG_GLOBAL_Y_SIZE                                      758
#define MP3_GLOBAL_X_SIZE                                       3908
#define MP3_GLOBAL_Y_SIZE                                       6790
#define MP3_LOCAL_X_SIZE                                        6288
#define MP3_LOCAL_Y_SIZE                                        2610
#define AMR_GLOBAL_X_SIZE                                       1548
#define AMR_GLOBAL_Y_SIZE                                       5068
#define AMR_LOCAL_X_SIZE                                        996
#define AMR_LOCAL_Y_SIZE                                        1002
#define JPEG_LOCAL_X_SIZE                                       6148
#define JPEG_LOCAL_Y_SIZE                                       1684
#define COMMON_LOCAL_X_SIZE                                     0
#define COMMON_LOCAL_Y_SIZE                                     0

/**************************
 **  SECTION_BEGIN_ADDR  **
 **************************/

#define COMMON_GLOBAL_X_BEGIN_ADDR                              (0 + RAM_X_BEGIN_ADDR)
#define COMMON_GLOBAL_Y_BEGIN_ADDR                              (0 + RAM_Y_BEGIN_ADDR)
#define JPEG_GLOBAL_X_BEGIN_ADDR                                (0 + RAM_X_BEGIN_ADDR)
#define JPEG_GLOBAL_Y_BEGIN_ADDR                                (56 + RAM_Y_BEGIN_ADDR)
#define MP3_GLOBAL_X_BEGIN_ADDR                                 (0 + RAM_X_BEGIN_ADDR)
#define MP3_GLOBAL_Y_BEGIN_ADDR                                 (814 + RAM_Y_BEGIN_ADDR)
#define MP3_LOCAL_X_BEGIN_ADDR                                  (3908 + RAM_X_BEGIN_ADDR)
#define MP3_LOCAL_Y_BEGIN_ADDR                                  (7604 + RAM_Y_BEGIN_ADDR)
#define AMR_GLOBAL_X_BEGIN_ADDR                                 (0 + RAM_X_BEGIN_ADDR)
#define AMR_GLOBAL_Y_BEGIN_ADDR                                 (814 + RAM_Y_BEGIN_ADDR)
#define AMR_LOCAL_X_BEGIN_ADDR                                  (1548 + RAM_X_BEGIN_ADDR)
#define AMR_LOCAL_Y_BEGIN_ADDR                                  (5882 + RAM_Y_BEGIN_ADDR)
#define JPEG_LOCAL_X_BEGIN_ADDR                                 (3908 + RAM_X_BEGIN_ADDR)
#define JPEG_LOCAL_Y_BEGIN_ADDR                                 (7604 + RAM_Y_BEGIN_ADDR)
#define COMMON_LOCAL_X_BEGIN_ADDR                               (10196 + RAM_X_BEGIN_ADDR)
#define COMMON_LOCAL_Y_BEGIN_ADDR                               (10214 + RAM_Y_BEGIN_ADDR)

/*********************
 **  COMMON_GLOBAL  **
 *********************/

//GLOBAL_VARS
#define VPP_AMJR_MODE_NO                                    (0x0)
#define VPP_AMJR_MODE_AMR                                   (0x1)
#define VPP_AMJR_MODE_MP3                                   (0x2)
#define VPP_AMJR_MODE_AAC                                   (0x3)
#define VPP_AMJR_MODE_WMA                                   (0x4)
#define VPP_AMJR_MODE_JPEG                                  (0x100)
#define VPP_AMJR_STRM_ID_NO                                 (0x0)
#define VPP_AMJR_STRM_ID_AUDIO                              (0x1)
#define VPP_AMJR_STRM_ID_VIDEO                              (0x2)
#define VPP_AMJR_STRM_ID_INIT                               (0x3)
#define VPP_AMJR_ERROR_NONE                                 (0)
#define VPP_AMJR_ERROR_UNKNOWN_AUDIO_MODE                   (-1)
#define VPP_AMJR_ERROR_UNKNOWN_VIDEO_MODE                   (-2)
#define VPP_AMJR_ERROR_VIDEO_BAD_PARAMETER                  (-3)
#define VPP_AMJR_ERROR_PREVIEW_BAD_PARAMETER                (-4)

//GLOBAL_VARS
//VPP_AMJR_CFG_STRUCT
#define VPP_AMJR_CFG_STRUCT                                 (0 + COMMON_GLOBAL_Y_BEGIN_ADDR)
#define GLOBAL_MODE                                         (0 + VPP_AMJR_CFG_STRUCT) //1 short
#define GLOBAL_RESET                                        (1 + VPP_AMJR_CFG_STRUCT) //1 short
#define GLOBAL_IN_STRM_BUF_START                            (2 + VPP_AMJR_CFG_STRUCT) //2 short
#define GLOBAL_OUT_STRM_BUF_START                           (4 + VPP_AMJR_CFG_STRUCT) //2 short
#define GLOBAL_IN_STRM_SAMPLE_RATE                          (6 + VPP_AMJR_CFG_STRUCT) //2 short
#define GLOBAL_OUT_STRM_BIT_RATE                            (8 + VPP_AMJR_CFG_STRUCT) //1 short
#define GLOBAL_IN_STRM_NB_CHAN                              (9 + VPP_AMJR_CFG_STRUCT) //1 short
#define GLOBAL_IN_STRM_BUF_IFC_PTR                          (10 + VPP_AMJR_CFG_STRUCT) //2 short
#define GLOBAL_IN_VIDEO_BUF_START                           (12 + VPP_AMJR_CFG_STRUCT) //2 short
#define GLOBAL_OUT_VIDEO_BUF_START                          (14 + VPP_AMJR_CFG_STRUCT) //2 short
#define GLOBAL_VIDEO_WIDTH                                  (16 + VPP_AMJR_CFG_STRUCT) //1 short
#define GLOBAL_VIDEO_HEIGHT                                 (17 + VPP_AMJR_CFG_STRUCT) //1 short
#define GLOBAL_VIDEO_QUALITY                                (18 + VPP_AMJR_CFG_STRUCT) //1 short
#define GLOBAL_VIDEO_IN_FORMAT                              (19 + VPP_AMJR_CFG_STRUCT) //1 short
#define GLOBAL_VIDEO_PXL_SWAP                               (20 + VPP_AMJR_CFG_STRUCT) //1 short
#define GLOBAL_VIDEO_RESERVED                               (21 + VPP_AMJR_CFG_STRUCT) //1 short
#define GLOBAL_PREVIEW_VIDEO_BUF_START                      (22 + VPP_AMJR_CFG_STRUCT) //2 short
#define GLOBAL_PREVIEW_VIDEO_WIDTH                          (24 + VPP_AMJR_CFG_STRUCT) //1 short
#define GLOBAL_PREVIEW_VIDEO_HEIGHT                         (25 + VPP_AMJR_CFG_STRUCT) //1 short
#define GLOBAL_PREVIEW_VIDEO_OUT_FORMAT                     (26 + VPP_AMJR_CFG_STRUCT) //1 short
#define GLOBAL_PREVIEW_VIDEO_SCALEFACTOR                    (27 + VPP_AMJR_CFG_STRUCT) //1 short

//VPP_AMJR_STATUS_STRUCT
#define VPP_AMJR_STATUS_STRUCT                              (28 + COMMON_GLOBAL_Y_BEGIN_ADDR)
#define GLOBAL_USED_MODE                                    (0 + VPP_AMJR_STATUS_STRUCT) //1 short
#define GLOBAL_ERR_STATUS                                   (1 + VPP_AMJR_STATUS_STRUCT) //1 short
#define GLOBAL_STRM_STATUS                                  (2 + VPP_AMJR_STATUS_STRUCT) //1 short
#define GLOBAL_RSVD_STATUS                                  (3 + VPP_AMJR_STATUS_STRUCT) //1 short
#define GLOBAL_STRM_OUT_LEN                                 (4 + VPP_AMJR_STATUS_STRUCT) //2 short
#define GLOBAL_STRM_IN_LEN                                  (6 + VPP_AMJR_STATUS_STRUCT) //2 short
#define GLOBAL_IN_STRM_BUF_IFC_CURR                         (8 + VPP_AMJR_STATUS_STRUCT) //2 short
#define GLOBAL_VIDEO_OUT_LEN                                (10 + VPP_AMJR_STATUS_STRUCT) //2 short

//VPP_AMJR_CODE_CFG_STRUCT
#define VPP_AMJR_CODE_CFG_STRUCT                            (40 + COMMON_GLOBAL_Y_BEGIN_ADDR)
#define GLOBAL_AMR_CODE_PTR                                 (0 + VPP_AMJR_CODE_CFG_STRUCT) //2 short
#define GLOBAL_AMR_CONST_X_PTR                              (2 + VPP_AMJR_CODE_CFG_STRUCT) //2 short
#define GLOBAL_AMR_CONST_Y_PTR                              (4 + VPP_AMJR_CODE_CFG_STRUCT) //2 short
#define GLOBAL_MP3_CODE_PTR                                 (6 + VPP_AMJR_CODE_CFG_STRUCT) //2 short
#define GLOBAL_MP3_CONST_X_PTR                              (8 + VPP_AMJR_CODE_CFG_STRUCT) //2 short
#define GLOBAL_MP3_CONST_Y_PTR                              (10 + VPP_AMJR_CODE_CFG_STRUCT) //2 short
#define GLOBAL_JPEG_CODE_PTR                                (12 + VPP_AMJR_CODE_CFG_STRUCT) //2 short
#define GLOBAL_JPEG_CONST_Y_PTR                             (14 + VPP_AMJR_CODE_CFG_STRUCT) //2 short


/*******************
 **  JPEG_GLOBAL  **
 *******************/

//GLOBAL_VARS
//VPP_AMJR_JPEG_CONST_STRUCT
#define VPP_AMJR_JPEG_CONST_STRUCT                          (0 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define JPEGENC_MYDCT_ADDR                                  (0 + VPP_AMJR_JPEG_CONST_STRUCT) //12 short
#define CONST_MYQUN_TAB0_ADDR                               (12 + VPP_AMJR_JPEG_CONST_STRUCT) //128 short
#define CONST_MYQUN_TAB1_ADDR                               (140 + VPP_AMJR_JPEG_CONST_STRUCT) //128 short
#define CONST_MYHUFF_DC0_ADDR                               (268 + VPP_AMJR_JPEG_CONST_STRUCT) //24 short
#define CONST_MYHUFF_DC1_ADDR                               (292 + VPP_AMJR_JPEG_CONST_STRUCT) //24 short
#define CONST_MYHUFF_AC_COMPRESSED                          (316 + VPP_AMJR_JPEG_CONST_STRUCT) //358 short
#define MATRIC_RGB_ADDR                                     (674 + VPP_AMJR_JPEG_CONST_STRUCT) //8 short
#define CONST_inverse_Zig_Zag_ADDR                          (682 + VPP_AMJR_JPEG_CONST_STRUCT) //64 short
#define CONST_0xff_ADDR                                     (746 + VPP_AMJR_JPEG_CONST_STRUCT) //1 short
#define CONST_NEG7_ADDR                                     (747 + VPP_AMJR_JPEG_CONST_STRUCT) //1 short
#define CONST_640_INT_ADDR                                  (748 + VPP_AMJR_JPEG_CONST_STRUCT) //1 short
#define CONST_640BY8_INT_ADDR                               (749 + VPP_AMJR_JPEG_CONST_STRUCT) //1 short
#define CONST_640BY9_INT_ADDR                               (750 + VPP_AMJR_JPEG_CONST_STRUCT) //1 short
#define CONST_479_ADDR                                      (751 + VPP_AMJR_JPEG_CONST_STRUCT) //1 short
#define CONST_640_ADDR                                      (752 + VPP_AMJR_JPEG_CONST_STRUCT) //2 short
#define CONST_0X80_ADDR                                     (754 + VPP_AMJR_JPEG_CONST_STRUCT) //2 short

#define CONST_0_ADDR                                        (0 + CONST_inverse_Zig_Zag_ADDR)
#define CONST_1_ADDR                                        (1 + CONST_inverse_Zig_Zag_ADDR)
#define CONST_7_ADDR                                        (28 + CONST_inverse_Zig_Zag_ADDR)
#define CONST_8_ADDR                                        (2 + CONST_inverse_Zig_Zag_ADDR)
#define CONST_15_ADDR                                       (42 + CONST_inverse_Zig_Zag_ADDR)
#define CONST_16_ADDR                                       (3 + CONST_inverse_Zig_Zag_ADDR)
#define CONST_24_ADDR                                       (9 + CONST_inverse_Zig_Zag_ADDR)
#define GLOBAL_WIDTH_SAVE_ADDR                              (756 + JPEG_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_HEIGHT_SAVE_ADDR                             (757 + JPEG_GLOBAL_Y_BEGIN_ADDR) //1 short

/******************
 **  MP3_GLOBAL  **
 ******************/

//GLOBAL_VARS
#define OUTPUT_MP3_BUFFER_ADDR                              (0 + MP3_GLOBAL_X_BEGIN_ADDR) //1024 short
#define GLOBAL_XW_begine_ADDR                               (1024 + MP3_GLOBAL_X_BEGIN_ADDR) //1024 short
//VPP_AMJR_MP3_CONST_X_STRUCT
#define VPP_AMJR_MP3_CONST_X_STRUCT                         (2048 + MP3_GLOBAL_X_BEGIN_ADDR)
#define TABLE_ca_encode_tab_ADDR                            (0 + VPP_AMJR_MP3_CONST_X_STRUCT) //16 short
#define TABLE_cs_encode_tab_ADDR                            (16 + VPP_AMJR_MP3_CONST_X_STRUCT) //16 short
#define TABLE_ew_tab_compressed_ADDR                        (32 + VPP_AMJR_MP3_CONST_X_STRUCT) //226 short
#define TABLE_t1HB_tab_ADDR                                 (258 + VPP_AMJR_MP3_CONST_X_STRUCT) //2 short
#define TABLE_t2HB_tab_ADDR                                 (260 + VPP_AMJR_MP3_CONST_X_STRUCT) //5 short
#define TABLE_t3HB_tab_ADDR                                 (265 + VPP_AMJR_MP3_CONST_X_STRUCT) //5 short
#define TABLE_t5HB_tab_ADDR                                 (270 + VPP_AMJR_MP3_CONST_X_STRUCT) //8 short
#define TABLE_t6HB_tab_ADDR                                 (278 + VPP_AMJR_MP3_CONST_X_STRUCT) //8 short
#define TABLE_t7HB_tab_ADDR                                 (286 + VPP_AMJR_MP3_CONST_X_STRUCT) //18 short
#define TABLE_t8HB_tab_ADDR                                 (304 + VPP_AMJR_MP3_CONST_X_STRUCT) //18 short
#define TABLE_t9HB_tab_ADDR                                 (322 + VPP_AMJR_MP3_CONST_X_STRUCT) //18 short
#define TABLE_t10HB_tab_ADDR                                (340 + VPP_AMJR_MP3_CONST_X_STRUCT) //32 short
#define TABLE_t11HB_tab_ADDR                                (372 + VPP_AMJR_MP3_CONST_X_STRUCT) //32 short
#define TABLE_t12HB_tab_ADDR                                (404 + VPP_AMJR_MP3_CONST_X_STRUCT) //32 short
#define TABLE_t13HB_tab_ADDR                                (436 + VPP_AMJR_MP3_CONST_X_STRUCT) //128 short
#define TABLE_t15HB_tab_ADDR                                (564 + VPP_AMJR_MP3_CONST_X_STRUCT) //128 short
#define TABLE_t16HB_tab_ADDR                                (692 + VPP_AMJR_MP3_CONST_X_STRUCT) //256 short
#define TABLE_t24HB_tab_ADDR                                (948 + VPP_AMJR_MP3_CONST_X_STRUCT) //256 short
#define TABLE_t32HB_tab_ADDR                                (1204 + VPP_AMJR_MP3_CONST_X_STRUCT) //8 short
#define TABLE_t33HB_tab_ADDR                                (1212 + VPP_AMJR_MP3_CONST_X_STRUCT) //8 short
#define TABLE_ht_tab_ADDR                                   (1220 + VPP_AMJR_MP3_CONST_X_STRUCT) //170 short
#define TABLE_sfBandIndex_tab_ADDR                          (1390 + VPP_AMJR_MP3_CONST_X_STRUCT) //222 short
#define TABLE_subdv_table_tab_ADDR                          (1612 + VPP_AMJR_MP3_CONST_X_STRUCT) //24 short
#define TABLE_coef_tab_ADDR                                 (1636 + VPP_AMJR_MP3_CONST_X_STRUCT) //72 short
#define TABLE_encodeSideInfo_tab_ADDR                       (1708 + VPP_AMJR_MP3_CONST_X_STRUCT) //12 short
#define MP3_CONST_inverse_Zig_Zag_ADDR                      (1720 + VPP_AMJR_MP3_CONST_X_STRUCT) //78 short
#define MP3_CONST_1152_ADDR                                 (1798 + VPP_AMJR_MP3_CONST_X_STRUCT) //1 short
#define MP3_CONST_NEG7_ADDR                                 (1799 + VPP_AMJR_MP3_CONST_X_STRUCT) //1 short
#define MP3_CONST_1_WORD32_ADDR                             (1800 + VPP_AMJR_MP3_CONST_X_STRUCT) //2 short
#define MP3_CONST_neg1_ADDR                                 (1802 + VPP_AMJR_MP3_CONST_X_STRUCT) //1 short
#define MP3_CONST_NEG31_ADDR                                (1803 + VPP_AMJR_MP3_CONST_X_STRUCT) //1 short
#define MP3_CONST_0x90d0614_ADDR                            (1804 + VPP_AMJR_MP3_CONST_X_STRUCT) //2 short
#define MP3_CONST_0X28514_165140_ADDR                       (1806 + VPP_AMJR_MP3_CONST_X_STRUCT) //2 short
#define MP3_CONST_10000_WORD32_ADDR                         (1808 + VPP_AMJR_MP3_CONST_X_STRUCT) //2 short
#define MP3_CONST_0X33E40_212544_ADDR                       (1810 + VPP_AMJR_MP3_CONST_X_STRUCT) //2 short
#define MP3_CONST_0X186A0_100000_ADDR                       (1812 + VPP_AMJR_MP3_CONST_X_STRUCT) //2 short
#define MP3_CONST_8192_WORD32_ADDR                          (1814 + VPP_AMJR_MP3_CONST_X_STRUCT) //2 short
#define MP3_CONST_0X40000000_ADDR                           (1816 + VPP_AMJR_MP3_CONST_X_STRUCT) //2 short

#define MP3_CONST_0_ADDR                                    ((0 + TABLE_ht_tab_ADDR))
#define MP3_CONST_1_ADDR                                    ((1 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_2_ADDR                                    ((5 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_3_ADDR                                    ((6 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_4_ADDR                                    ((14 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_5_ADDR                                    ((15 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_7_ADDR                                    ((28 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_8_ADDR                                    ((2 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_10_ADDR                                   ((7 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_12_ADDR                                   ((16 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_13_ADDR                                   ((26 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_14_ADDR                                   ((29 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_15_ADDR                                   ((42 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_16_ADDR                                   ((3 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_18_ADDR                                   ((12 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_20_ADDR                                   ((25 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_24_ADDR                                   ((9 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_26_ADDR                                   ((18 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_30_ADDR                                   ((44 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_31_ADDR                                   ((53 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_32_ADDR                                   ((10 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_34_ADDR                                   ((23 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_36_ADDR                                   ((39 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_37_ADDR                                   ((45 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_46_ADDR                                   ((55 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_52_ADDR                                   ((50 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_56_ADDR                                   ((35 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_60_ADDR                                   ((57 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_62_ADDR                                   ((62 + MP3_CONST_inverse_Zig_Zag_ADDR))
#define MP3_CONST_64_ADDR                                   ((52 + TABLE_t16HB_tab_ADDR))
#define MP3_CONST_66_ADDR                                   ((80 + TABLE_t16HB_tab_ADDR))
#define MP3_CONST_122_ADDR                                  ((22 + TABLE_t24HB_tab_ADDR))
#define MP3_CONST_164_ADDR                                  ((71 + TABLE_t16HB_tab_ADDR))
#define MP3_CONST_511_ADDR                                  ((147 + TABLE_ht_tab_ADDR))
#define MP3_CONST_576_ADDR                                  ((22 + TABLE_sfBandIndex_tab_ADDR))
//SHINE_GLOBAL_VARS_X_START_ADDR
#define SHINE_GLOBAL_VARS_X_START_ADDR                      (3866 + MP3_GLOBAL_X_BEGIN_ADDR)
#define STATIC_PartHoldersInitialized_ADDR                  (0 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STATIC_buf_bit_idx_ADDR                             (1 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STATIC_buf_short_idx_ADDR                           (2 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STATIC_buf_long_idx_ADDR                            (3 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STATIC_buf_chache_ADDR                              (4 + SHINE_GLOBAL_VARS_X_START_ADDR) //2 short
#define STATIC_BitCount_ADDR                                (6 + SHINE_GLOBAL_VARS_X_START_ADDR) //2 short
#define STATIC_ThisFrameSize_ADDR                           (8 + SHINE_GLOBAL_VARS_X_START_ADDR) //2 short
#define STATIC_BitsRemaining_ADDR                           (10 + SHINE_GLOBAL_VARS_X_START_ADDR) //2 short
#define GLOBAL_OFF_ADDR                                     (12 + SHINE_GLOBAL_VARS_X_START_ADDR) //2 short
#define STATIC_FIRSTCALL_ADDR                               (14 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define GLOBAL_main_data_begin_ADDR                         (15 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define GLOBAL_xrmax_ADDR                                   (16 + SHINE_GLOBAL_VARS_X_START_ADDR) //2 short
#define STATIC_ResvSize_ADDR                                (18 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STATIC_scalefac_band_long_ADDR                      (19 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STATIC_change_ADDR                                  (20 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STATIC_nGranules_ADDR                               (21 + SHINE_GLOBAL_VARS_X_START_ADDR) //1 short
#define STRUCT_frameData_frameLength_ADDR                   (22 + SHINE_GLOBAL_VARS_X_START_ADDR) //2 short
#define GLOBLE_channel_ADDR                                 (24 + SHINE_GLOBAL_VARS_X_START_ADDR) //2 short

//STRUCT_frameData_ADDR
#define STRUCT_frameData_ADDR                               (3892 + MP3_GLOBAL_X_BEGIN_ADDR)
#define STRUCT_frameData_STRUCT_header_ADDR                 (0 + STRUCT_frameData_ADDR) //2 short
#define STRUCT_frameData_STRUCT_frameSI_ADDR                (2 + STRUCT_frameData_ADDR) //2 short
#define STRUCT_frameData_STRUCT_channelSI_ADDR              (4 + STRUCT_frameData_ADDR) //4 short
#define STRUCT_frameData_STRUCT_spectrumSI_ADDR             (8 + STRUCT_frameData_ADDR) //8 short


//GLOBAL_VARS
//VPP_AMJR_MP3_CONST_Y_STRUCT
#define VPP_AMJR_MP3_CONST_Y_STRUCT                         (0 + MP3_GLOBAL_Y_BEGIN_ADDR)
#define STATIC_RQ_ADDR                                      (0 + VPP_AMJR_MP3_CONST_Y_STRUCT) //100 short
#define TABLE_fl_tab_ADDR                                   (100 + VPP_AMJR_MP3_CONST_Y_STRUCT) //992 short
#define TABLE_w_tab_ADDR                                    (1092 + VPP_AMJR_MP3_CONST_Y_STRUCT) //36 short
#define TABLE_w2_tab_ADDR                                   (1128 + VPP_AMJR_MP3_CONST_Y_STRUCT) //18 short
#define TABLE_win_tab_ADDR                                  (1146 + VPP_AMJR_MP3_CONST_Y_STRUCT) //72 short
#define TABLE_pow075_tab_tab_ADDR                           (1218 + VPP_AMJR_MP3_CONST_Y_STRUCT) //24 short
#define TABLE_putmask_tab_ADDR                              (1242 + VPP_AMJR_MP3_CONST_Y_STRUCT) //10 short
#define TABLE_t1l_tab_ADDR                                  (1252 + VPP_AMJR_MP3_CONST_Y_STRUCT) //2 short
#define TABLE_t2l_tab_ADDR                                  (1254 + VPP_AMJR_MP3_CONST_Y_STRUCT) //5 short
#define TABLE_t3l_tab_ADDR                                  (1259 + VPP_AMJR_MP3_CONST_Y_STRUCT) //5 short
#define TABLE_t5l_tab_ADDR                                  (1264 + VPP_AMJR_MP3_CONST_Y_STRUCT) //8 short
#define TABLE_t6l_tab_ADDR                                  (1272 + VPP_AMJR_MP3_CONST_Y_STRUCT) //8 short
#define TABLE_t7l_tab_ADDR                                  (1280 + VPP_AMJR_MP3_CONST_Y_STRUCT) //18 short
#define TABLE_t8l_tab_ADDR                                  (1298 + VPP_AMJR_MP3_CONST_Y_STRUCT) //18 short
#define TABLE_t9l_tab_ADDR                                  (1316 + VPP_AMJR_MP3_CONST_Y_STRUCT) //18 short
#define TABLE_t10l_tab_ADDR                                 (1334 + VPP_AMJR_MP3_CONST_Y_STRUCT) //32 short
#define TABLE_t11l_tab_ADDR                                 (1366 + VPP_AMJR_MP3_CONST_Y_STRUCT) //32 short
#define TABLE_t12l_tab_ADDR                                 (1398 + VPP_AMJR_MP3_CONST_Y_STRUCT) //32 short
#define TABLE_t13l_tab_ADDR                                 (1430 + VPP_AMJR_MP3_CONST_Y_STRUCT) //128 short
#define TABLE_t15l_tab_ADDR                                 (1558 + VPP_AMJR_MP3_CONST_Y_STRUCT) //128 short
#define TABLE_t16l_tab_ADDR                                 (1686 + VPP_AMJR_MP3_CONST_Y_STRUCT) //128 short
#define TABLE_t24l_tab_ADDR                                 (1814 + VPP_AMJR_MP3_CONST_Y_STRUCT) //128 short
#define TABLE_t32l_tab_ADDR                                 (1942 + VPP_AMJR_MP3_CONST_Y_STRUCT) //8 short
#define TABLE_t33l_tab_ADDR                                 (1950 + VPP_AMJR_MP3_CONST_Y_STRUCT) //8 short

#define MP3_CONST_4095_ADDR                                 ((15 + TABLE_cs_encode_tab_ADDR))
//GLOBAL_l3_sb_sample_ADDR
#define GLOBAL_l3_sb_sample_ADDR                            (1958 + MP3_GLOBAL_Y_BEGIN_ADDR)
#define l3_sb_sample_0_0_0_0_ADDR                           (0 + GLOBAL_l3_sb_sample_ADDR) //1152 short
#define l3_sb_sample_0_1_0_0_ADDR                           (1152 + GLOBAL_l3_sb_sample_ADDR) //1152 short
#define l3_sb_sample_1_0_0_0_ADDR                           (2304 + GLOBAL_l3_sb_sample_ADDR) //1152 short
#define l3_sb_sample_1_1_0_0_ADDR                           (3456 + GLOBAL_l3_sb_sample_ADDR) //1152 short

//STRUCT_CONFIG_ADDR
#define STRUCT_CONFIG_ADDR                                  (6566 + MP3_GLOBAL_Y_BEGIN_ADDR)
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
#define STRUCT_SIDEINFO_ADDR                                (6578 + MP3_GLOBAL_Y_BEGIN_ADDR)
#define STRUCT_SIDEINFO_main_data_begin_ADDR                (0 + STRUCT_SIDEINFO_ADDR) //2 short
#define STRUCT_SIDEINFO_private_bits_ADDR                   (2 + STRUCT_SIDEINFO_ADDR) //2 short
#define STRUCT_SIDEINFO_resvDrain_ADDR                      (4 + STRUCT_SIDEINFO_ADDR) //2 short
#define STRUCT_SIDEINFO_scfsi_ADDR                          (6 + STRUCT_SIDEINFO_ADDR) //16 short
#define STRUCT_SIDEINFO_gr0_ch0_part2_3_length_ADDR         (22 + STRUCT_SIDEINFO_ADDR) //92 short
#define STRUCT_SIDEINFO_gr1_ch0_part2_3_length_ADDR         (114 + STRUCT_SIDEINFO_ADDR) //92 short

#define STRUCT_INPUT_slot_lag_ADDR                          (6784 + MP3_GLOBAL_Y_BEGIN_ADDR) //2 short
#define STRUCT_INPUT_frac_slots_per_frame_ADDR              (6786 + MP3_GLOBAL_Y_BEGIN_ADDR) //2 short
#define STRUCT_INPUT_whole_slots_per_frame_ADDR             (6788 + MP3_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STRUCT_INPUT_TMP                                    (6789 + MP3_GLOBAL_Y_BEGIN_ADDR) //1 short

/*****************
 **  MP3_LOCAL  **
 *****************/

//vpp_AmjrMp3Encode
#define vpp_AmjrMp3Encode_X_BEGIN_ADDR                      (0 + MP3_LOCAL_X_BEGIN_ADDR)
//GLOBAL_MDCT_FREQ_00_ADDR
#define GLOBAL_MDCT_FREQ_00_ADDR                            (0 + vpp_AmjrMp3Encode_X_BEGIN_ADDR)
#define GLOBAL_Z_ADDR                                       (0 + GLOBAL_MDCT_FREQ_00_ADDR) //1024 short
#define GLOBAL_Y_ADDR                                       (1024 + GLOBAL_MDCT_FREQ_00_ADDR) //140 short

//IN_MP3_BUFFER
#define IN_MP3_BUFFER                                       (1164 + vpp_AmjrMp3Encode_X_BEGIN_ADDR)
#define IN_MP3_BUFFER_10                                    (0 + IN_MP3_BUFFER) //576 short
#define IN_MP3_BUFFER_11                                    (576 + IN_MP3_BUFFER) //576 short
#define IN_MP3_BUFFER_20                                    (1152 + IN_MP3_BUFFER) //576 short
#define IN_MP3_BUFFER_21                                    (1728 + IN_MP3_BUFFER) //576 short

#define GLOBAL_L3ENC_gr1                                    (IN_MP3_BUFFER)
#define GLOBAL_L3ENC_gr0                                    (3468 + vpp_AmjrMp3Encode_X_BEGIN_ADDR) //2304 short
#define MP3ENC_MP3_CONST_bitrate_map_ADDR                   (5772 + vpp_AmjrMp3Encode_X_BEGIN_ADDR) //16 short
#define TABLE_ew_tab_ADDR                                   (5788 + vpp_AmjrMp3Encode_X_BEGIN_ADDR) //500 short
#define TEMP_MDCT_ADDR                                      (( 0 + GLOBAL_Y_ADDR))
#define GLOBAL_mdct_in_ADDR                                 (( 0 + TEMP_MDCT_ADDR))
#define LOCAL_f_tab_ADDR                                    (( 72 + TEMP_MDCT_ADDR))
#define TEMP_A_ADDR                                         ((108 + TEMP_MDCT_ADDR))
#define TEMP_A_ADDR_9                                       (( 18 + TEMP_A_ADDR))

//vpp_AmjrMp3Encode
#define vpp_AmjrMp3Encode_Y_BEGIN_ADDR                      (0 + MP3_LOCAL_Y_BEGIN_ADDR)
//STRUCT_GRINFO_ADDR
#define STRUCT_GRINFO_ADDR                                  (0 + vpp_AmjrMp3Encode_Y_BEGIN_ADDR)
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

#define GLOBAL_MDCT_FREQ_01_ADDR                            (38 + vpp_AmjrMp3Encode_Y_BEGIN_ADDR) //1164 short
#define GLOBAL_RXAB_ADDR                                    (1202 + vpp_AmjrMp3Encode_Y_BEGIN_ADDR) //1152 short
#define SHINE_LOCAL_VARS_Y_START_ADDR                       ((GLOBAL_MDCT_FREQ_01_ADDR))
#define STRUCT_frameData_STRUCT_header_element_ADDR         ((0 + SHINE_LOCAL_VARS_Y_START_ADDR))
#define STRUCT_frameData_STRUCT_frameSI_element_ADDR            ((30 + SHINE_LOCAL_VARS_Y_START_ADDR))
#define STRUCT_frameData_STRUCT_channelSI_ch0_element_ADDR          ((36 + SHINE_LOCAL_VARS_Y_START_ADDR))
#define STRUCT_frameData_STRUCT_channelSI_ch1_element_ADDR          ((44 + SHINE_LOCAL_VARS_Y_START_ADDR))
#define STRUCT_frameData_STRUCT_spectrumSI_gr0ch0_element_ADDR          ((52 + SHINE_LOCAL_VARS_Y_START_ADDR))
#define STRUCT_frameData_STRUCT_spectrumSI_gr0ch1_element_ADDR          ((78 + SHINE_LOCAL_VARS_Y_START_ADDR))
#define STRUCT_frameData_STRUCT_spectrumSI_gr1ch0_element_ADDR          ((104 + SHINE_LOCAL_VARS_Y_START_ADDR))
#define STRUCT_frameData_STRUCT_spectrumSI_gr1ch1_element_ADDR          ((130 + SHINE_LOCAL_VARS_Y_START_ADDR))
#define STRUCT_frameData_STRUCT_codedData_gr0ch0_element_ADDR           ((160 + SHINE_LOCAL_VARS_Y_START_ADDR))
#define TEMP_B_ADDR                                         (STRUCT_GRINFO_ADDR)
#define TEMP_B_ADDR_9                                       ((18 + TEMP_B_ADDR))

//quantize
#define quantize_Y_BEGIN_ADDR                               (2354 + MP3_LOCAL_Y_BEGIN_ADDR)
#define steptabi_tab_ADDR                                   (0 + quantize_Y_BEGIN_ADDR) //256 short

/******************
 **  AMR_GLOBAL  **
 ******************/

//GLOBAL_VARS
//VPP_AMJR_AMR_CONST_X_STRUCT
#define VPP_AMJR_AMR_CONST_X_STRUCT                         (0 + AMR_GLOBAL_X_BEGIN_ADDR)
#define AMR_CONST_0x00000000_ADDR                           (0 + VPP_AMJR_AMR_CONST_X_STRUCT) //2 short
#define AMR_CONST_0x00000001_ADDR                           (2 + VPP_AMJR_AMR_CONST_X_STRUCT) //2 short
#define AMR_CONST_0x0000FFFF_ADDR                           (4 + VPP_AMJR_AMR_CONST_X_STRUCT) //2 short
#define AMR_CONST_0x00008000_ADDR                           (6 + VPP_AMJR_AMR_CONST_X_STRUCT) //2 short
#define AMR_CONST_0x00010001_ADDR                           (8 + VPP_AMJR_AMR_CONST_X_STRUCT) //2 short
#define AMR_CONST_0x00020002_ADDR                           (10 + VPP_AMJR_AMR_CONST_X_STRUCT) //2 short
#define AMR_CONST_0x00080008_ADDR                           (12 + VPP_AMJR_AMR_CONST_X_STRUCT) //2 short
#define AMR_CONST_0x40000000_ADDR                           (14 + VPP_AMJR_AMR_CONST_X_STRUCT) //2 short
#define AMR_CONST_0x7FFFFFFF_ADDR                           (16 + VPP_AMJR_AMR_CONST_X_STRUCT) //2 short
#define AMR_CONST_0x80000000_ADDR                           (18 + VPP_AMJR_AMR_CONST_X_STRUCT) //2 short
#define AMR_CONST_0x8000FFFF_ADDR                           (20 + VPP_AMJR_AMR_CONST_X_STRUCT) //2 short
#define AMR_CONST_3_ADDR                                    (22 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_4_ADDR                                    (23 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_5_ADDR                                    (24 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_6_ADDR                                    (25 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_10_ADDR                                   (26 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_11_ADDR                                   (27 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_16_ADDR                                   (28 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_20_ADDR                                   (29 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_22_ADDR                                   (30 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_30_ADDR                                   (31 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_31_ADDR                                   (32 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_32_ADDR                                   (33 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_33_ADDR                                   (34 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_39_ADDR                                   (35 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_40_ADDR                                   (36 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_44_ADDR                                   (37 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_80_ADDR                                   (38 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_neg3_ADDR                                 (39 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_143_ADDR                                  (40 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_160_ADDR                                  (41 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_15565_ADDR                                (42 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_26214_ADDR                                (43 + VPP_AMJR_AMR_CONST_X_STRUCT) //1 short
#define AMR_CONST_TAB244_ADDR                               (44 + VPP_AMJR_AMR_CONST_X_STRUCT) //244 short

#define AMR_CONST_0_ADDR                                    (( 0 + AMR_CONST_0x00000000_ADDR))
#define AMR_CONST_1_ADDR                                    (( 0 + AMR_CONST_0x00000001_ADDR))
#define AMR_CONST_2_ADDR                                    (( 0 + AMR_CONST_0x00020002_ADDR))
#define AMR_CONST_8_ADDR                                    (( 0 + AMR_CONST_0x00080008_ADDR))
#define AMR_CONST_0xFFFF_ADDR                               (( 0 + AMR_CONST_0x0000FFFF_ADDR))
#define AMR_CONST_0x7FFF_ADDR                               (( 1 + AMR_CONST_0x7FFFFFFF_ADDR))
#define AMR_CONST_0x4000_ADDR                               (( 1 + AMR_CONST_0x40000000_ADDR))
#define STATIC_L_SACF_ADDR                                  (288 + AMR_GLOBAL_X_BEGIN_ADDR) //56 short
#define SCAL_ACF_ADDR_P                                     (344 + AMR_GLOBAL_X_BEGIN_ADDR) //4 short
#define PRAM4_TABLE_ADDR                                    (348 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
//STRUCT_PRE_PROCESSSTATE
#define STRUCT_PRE_PROCESSSTATE                             (358 + AMR_GLOBAL_X_BEGIN_ADDR)
#define STRUCT_PRE_PROCESSSTATE_Y2_LO_ADDR                  (0 + STRUCT_PRE_PROCESSSTATE) //1 short
#define STRUCT_PRE_PROCESSSTATE_Y2_HI_ADDR                  (1 + STRUCT_PRE_PROCESSSTATE) //1 short
#define STRUCT_PRE_PROCESSSTATE_Y1_LO_ADDR                  (2 + STRUCT_PRE_PROCESSSTATE) //1 short
#define STRUCT_PRE_PROCESSSTATE_Y1_HI_ADDR                  (3 + STRUCT_PRE_PROCESSSTATE) //1 short
#define STRUCT_PRE_PROCESSSTATE_X0_ADDR                     (4 + STRUCT_PRE_PROCESSSTATE) //1 short
#define STRUCT_PRE_PROCESSSTATE_X1_ADDR                     (5 + STRUCT_PRE_PROCESSSTATE) //1 short

//STRUCT_COD_AMRSTATE
#define STRUCT_COD_AMRSTATE                                 (364 + AMR_GLOBAL_X_BEGIN_ADDR)
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

#define STRUCT_LEVINSONSTATE_ADDR                           (1363 + AMR_GLOBAL_X_BEGIN_ADDR) //11 short
#define STRUCT_LSPSTATE_ADDR                                (1374 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_LSPSTATE_LSP_OLD_Q_ADDR                      (1384 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_Q_PLSFSTATE_ADDR                             (1394 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_CLLTPSTATE_ADDR                              (1404 + AMR_GLOBAL_X_BEGIN_ADDR) //18 short
#define STRUCT_GC_PREDST_ADDR                               (1422 + AMR_GLOBAL_X_BEGIN_ADDR) //30 short
//STRUCT_TONSTABSTATE
#define STRUCT_TONSTABSTATE                                 (1452 + AMR_GLOBAL_X_BEGIN_ADDR)
#define STRUCT_TONSTABSTATE_ADDR                            (0 + STRUCT_TONSTABSTATE) //7 short
#define STRUCT_TONSTABSTATE_COUNT_ADDR                      (7 + STRUCT_TONSTABSTATE) //1 short

#define STRUCT_COD_AMRSTATE_MEM_SYN_ADDR                    (1460 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_MEM_W0_ADDR                     (1470 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_MEM_W_ADDR                      (1480 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_MEM_ERROR_ADDR                  (1490 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_ERROR_ADDR                      (1500 + AMR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_SHARP_ADDR                      (1540 + AMR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STRUCT_COD_AMRSTATE_OLD_SPEECH_ADDR                 (STRUCT_COD_AMRSTATE_ADDR)
#define STRUCT_LEVINSONSTATE_OLD_A_ADDR                     (STRUCT_LEVINSONSTATE_ADDR)
#define STRUCT_LSPSTATE_LSP_OLD_ADDR                        (STRUCT_LSPSTATE_ADDR)
#define STRUCT_Q_PLSFSTATE_PAST_RQ_ADDR                     (STRUCT_Q_PLSFSTATE_ADDR)
#define STRUCT_PITCH_FRSTATE_T0_PREV_SUBFRAME_ADDR          (STRUCT_CLLTPSTATE_ADDR)
#define STRUCT_TONSTABSTATE_GP_ADDR                         (STRUCT_TONSTABSTATE_ADDR)

//GLOBAL_VARS
//VPP_AMJR_AMR_CONST_Y_STRUCT
#define VPP_AMJR_AMR_CONST_Y_STRUCT                         (0 + AMR_GLOBAL_Y_BEGIN_ADDR)
#define AMR_CONST_D_1_4_ADDR                                (0 + VPP_AMJR_AMR_CONST_Y_STRUCT) //2 short
#define AMR_CONST_D_1_8_ADDR                                (2 + VPP_AMJR_AMR_CONST_Y_STRUCT) //2 short
#define AMR_CONST_D_1_16_ADDR                               (4 + VPP_AMJR_AMR_CONST_Y_STRUCT) //2 short
#define AMR_CONST_D_1_32_ADDR                               (6 + VPP_AMJR_AMR_CONST_Y_STRUCT) //2 short
#define AMR_CONST_D_1_64_ADDR                               (8 + VPP_AMJR_AMR_CONST_Y_STRUCT) //2 short
#define TABLE_WINDOW_160_80_ADDR                            (10 + VPP_AMJR_AMR_CONST_Y_STRUCT) //240 short
#define TABLE_WINDOW_232_8_ADDR                             (250 + VPP_AMJR_AMR_CONST_Y_STRUCT) //240 short
#define TABLE_MEAN_LSF_ADDR                                 (490 + VPP_AMJR_AMR_CONST_Y_STRUCT) //10 short
#define TABLE_SLOPE_ADDR                                    (500 + VPP_AMJR_AMR_CONST_Y_STRUCT) //64 short
#define TABLE_LSP_LSF_ADDR                                  (564 + VPP_AMJR_AMR_CONST_Y_STRUCT) //66 short
#define TABLE_LOG2_ADDR                                     (630 + VPP_AMJR_AMR_CONST_Y_STRUCT) //34 short
#define TABLE_LAG_L_ADDR                                    (664 + VPP_AMJR_AMR_CONST_Y_STRUCT) //10 short
#define TABLE_LAG_H_ADDR                                    (674 + VPP_AMJR_AMR_CONST_Y_STRUCT) //10 short
#define TABLE_INV_SQRT_ADDR                                 (684 + VPP_AMJR_AMR_CONST_Y_STRUCT) //50 short
#define TABLE_GRID_ADDR                                     (734 + VPP_AMJR_AMR_CONST_Y_STRUCT) //62 short
#define TABLE_POW2_ADDR                                     (796 + VPP_AMJR_AMR_CONST_Y_STRUCT) //34 short
#define STATIC_CONST_QUA_GAIN_PITCH_ADDR                    (830 + VPP_AMJR_AMR_CONST_Y_STRUCT) //16 short
#define STATIC_CONST_INTER_6_ADDR                           (846 + VPP_AMJR_AMR_CONST_Y_STRUCT) //62 short
#define STATIC_CONST_INTER_6_25_ADDR                        (908 + VPP_AMJR_AMR_CONST_Y_STRUCT) //26 short
#define STATIC_CONST_F_GAMMA2_ADDR                          (934 + VPP_AMJR_AMR_CONST_Y_STRUCT) //10 short
#define STATIC_CONST_F_GAMMA1_ADDR                          (944 + VPP_AMJR_AMR_CONST_Y_STRUCT) //10 short
#define STATIC_CONST_GRAY_ADDR                              (954 + VPP_AMJR_AMR_CONST_Y_STRUCT) //8 short
#define TABLE_DICO1_LSF_ADDR                                (962 + VPP_AMJR_AMR_CONST_Y_STRUCT) //512 short
#define TABLE_DICO2_LSF_ADDR                                (1474 + VPP_AMJR_AMR_CONST_Y_STRUCT) //1024 short
#define TABLE_DICO3_LSF_ADDR                                (2498 + VPP_AMJR_AMR_CONST_Y_STRUCT) //1024 short
#define TABLE_DICO4_LSF_ADDR                                (3522 + VPP_AMJR_AMR_CONST_Y_STRUCT) //1024 short
#define TABLE_DICO5_LSF_ADDR                                (4546 + VPP_AMJR_AMR_CONST_Y_STRUCT) //256 short
#define STATIC_CONST_BITNO_ADDR                             (4802 + VPP_AMJR_AMR_CONST_Y_STRUCT) //30 short
#define STATIC_CONST_qua_gain_code_ADDR                     (4832 + VPP_AMJR_AMR_CONST_Y_STRUCT) //96 short
#define STATIC_CONST_pred_MR122_ADDR                        (4928 + VPP_AMJR_AMR_CONST_Y_STRUCT) //4 short
#define AMR_ENC_RESET_TABLE_ADDR                            (4932 + VPP_AMJR_AMR_CONST_Y_STRUCT) //56 short

#define AMR_CONST_0x1000_ADDR                               (( 0 + AMR_CONST_D_1_8_ADDR))
#define AMR_CONST_0x2000_ADDR                               (( 0 + AMR_CONST_D_1_4_ADDR))
#define STRUCT_COD_AMRSTATE_H0_ADDR                         (4988 + AMR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_H1_ADDR                         (5028 + AMR_GLOBAL_Y_BEGIN_ADDR) //40 short

/*****************
 **  AMR_LOCAL  **
 *****************/

//vpp_AmjrAmrEncode
#define vpp_AmjrAmrEncode_X_BEGIN_ADDR                      (0 + AMR_LOCAL_X_BEGIN_ADDR)
#define INPUT_SPEECH_BUFFER_ADDR                            (0 + vpp_AmjrAmrEncode_X_BEGIN_ADDR) //160 short
#define OUTPUT_BITS_ADDR                                    (160 + vpp_AmjrAmrEncode_X_BEGIN_ADDR) //16 short
//AMR_LOCAL_VARS_X_LEVEL0_START_ADDR
#define AMR_LOCAL_VARS_X_LEVEL0_START_ADDR                  (176 + vpp_AmjrAmrEncode_X_BEGIN_ADDR)
#define COD_AMR_XN_ADDRESS                                  (0 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR) //40 short
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

//AMR_LOCAL_VARS_X_LEVEL1_START_ADDR
#define AMR_LOCAL_VARS_X_LEVEL1_START_ADDR                  (564 + vpp_AmjrAmrEncode_X_BEGIN_ADDR)
#define SEARCH_10I40_RRV_ADDR                               (0 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) //40 short
#define SEARCH_CODE_CP_RR_ADDR                              (40 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) //320 short
#define SEARCH_CODE_PS0_TEMP_ADDR                           (360 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) //1 short
#define SEARCH_10I40_PS0_ADDR                               (361 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) //1 short
#define C1035PF_IPOS_ADDR                                   (362 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) //10 short
#define C1035PF_POS_MAX_ADDR                                (372 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) //10 short
#define C1035PF_CODVEC_ADDR                                 (382 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) //10 short
#define C1035PF_SIGN_ADDR                                   (392 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) //40 short

#define TEMP_MYRING_ADDR                                    (( 0 + AMR_LOCAL_VARS_X_LEVEL0_START_ADDR))
#define AZ_LSP_IP_ADDR                                      (( 0 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define Q_plsf_5_X                                          (( 0 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define Q_PLSF_5_LSF1_ADDR                                  (( 0 + Q_plsf_5_X))
#define Q_PLSF_5_LSF2_ADDR                                  (( 10 + Q_plsf_5_X))
#define Q_PLSF_5_WF1_ADDR                                   (( 20 + Q_plsf_5_X))
#define Q_PLSF_5_WF2_ADDR                                   (( 30 + Q_plsf_5_X))
#define Q_PLSF_5_LSF_R1_ADDR                                (( 40 + Q_plsf_5_X))
#define Q_PLSF_5_LSF_R2_ADDR                                (( 50 + Q_plsf_5_X))
#define Q_PLSF_5_LSF1_Q_ADDR                                (( 60 + Q_plsf_5_X))
#define Q_PLSF_5_LSF2_Q_ADDR                                (( 70 + Q_plsf_5_X))
#define Pitch_fr6_max_loop_addr                             (( 0 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define Pitch_fr6_t0_min_addr                               (( 2 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define Pitch_delta_search_addr                             (( 4 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define Pitch_fr6_corr_v_addr                               (( 200 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define PITCH_OL_M122_SCALED_SIGNAL2_ADDR                   (( 0 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define COR_H_H2_ADDR                                       (( 0 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define COR_H_X_Y32_ADDR                                    (( 0 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define build_code_sign_ADDR                                (( 0 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define SET_SIGN_EN_ADDR                                    (( 0 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define PRE_BIG_AP1_ADDR                                    (( 10 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define BIT_SERIAL_ADDR_RAM                                 (( 20 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define AUTOCORR_Y_ADDR                                     (( 2 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define LSP_LSP_NEW_Q_ADDR                                  (( 200 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define LSP_LSP_MID_ADDR                                    (( 210 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define LSP_LSP_MID_Q_ADDR                                  (( 220 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define LEVINSON_ANL_ADDR                                   (( 242 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define LPC_RLOW_ADDR                                       (( 300 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define LPC_RHIGH_ADDR                                      (( 312 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define LPC_RC_ADDR                                         (( 330 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define AZ_LSP_F1_ADDR                                      (( 40 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define AZ_LSP_F2_ADDR                                      (( 50 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))
#define AZ_LSP_F1_1_ADDR                                    (( 1 + AZ_LSP_F1_ADDR))
#define AZ_LSP_F2_1_ADDR                                    (( 1 + AZ_LSP_F2_ADDR))
#define PRE_BIG_AP2_ADDR                                    (( 30 + AMR_LOCAL_VARS_X_LEVEL1_START_ADDR))

//vpp_AmjrAmrEncode
#define vpp_AmjrAmrEncode_Y_BEGIN_ADDR                      (0 + AMR_LOCAL_Y_BEGIN_ADDR)
//AMR_LOCAL_VARS_Y_LEVEL0_START_ADDR
#define AMR_LOCAL_VARS_Y_LEVEL0_START_ADDR                  (0 + vpp_AmjrAmrEncode_Y_BEGIN_ADDR)
#define COD_AMR_ANA_ADDR                                    (0 + AMR_LOCAL_VARS_Y_LEVEL0_START_ADDR) //60 short

#define COD_AMR_CODE_SF0_ADDRESS                            (( 0 + AMR_LOCAL_VARS_Y_LEVEL0_START_ADDR))
#define COD_AMR_XN_SF0_ADDRESS                              (( 42 + AMR_LOCAL_VARS_Y_LEVEL0_START_ADDR))
#define COD_AMR_Y2_SF0_ADDRESS                              (( 82 + AMR_LOCAL_VARS_Y_LEVEL0_START_ADDR))
#define COD_AMR_T0_SF0_ADDRESS                              (( 122 + AMR_LOCAL_VARS_Y_LEVEL0_START_ADDR))
#define COD_AMR_T0_FRAC_SF0_ADDRESS                         (( 123 + AMR_LOCAL_VARS_Y_LEVEL0_START_ADDR))
#define COD_AMR_I_SUBFR_SF0_ADDRESS                         (( 124 + AMR_LOCAL_VARS_Y_LEVEL0_START_ADDR))
#define COD_AMR_GAIN_PIT_SF0_ADDRESS                        (( 126 + AMR_LOCAL_VARS_Y_LEVEL0_START_ADDR))
#define COD_AMR_GAIN_CODE_SF0_ADDRESS                       (( 127 + AMR_LOCAL_VARS_Y_LEVEL0_START_ADDR))
//AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR
#define AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR                  (60 + vpp_AmjrAmrEncode_Y_BEGIN_ADDR)
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
#define COR_H_H2_COPY_ADDR                                  (902 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) //40 short

#define Pitch_ol_M122                                       (( 0 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR))
#define PITCH_OL_M122_SCALED_SIGNAL_ADDR                    (( 0 + Pitch_ol_M122))
#define PITCH_OL_M122_P_MAX1_ADDR                           (( 310 + Pitch_ol_M122))
#define PITCH_OL_M122_MAX1_ADDR                             (( 311 + Pitch_ol_M122))
#define PITCH_OL_M122_P_MAX2_ADDR                           (( 312 + Pitch_ol_M122))
#define PITCH_OL_M122_MAX2_ADDR                             (( 313 + Pitch_ol_M122))
#define PITCH_OL_M122_P_MAX3_ADDR                           (( 314 + Pitch_ol_M122))
#define PITCH_OL_M122_MAX3_ADDR                             (( 315 + Pitch_ol_M122))
#define PITCH_OL_M122_SCAL_FAC_ADDR                         (( 316 + Pitch_ol_M122))
#define PITCH_OL_M122_CORR_ADDR                             (( 322 + Pitch_ol_M122))
#define PITCH_F6_EXCF_ADDR                                  (( 0 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR))
#define G_PITCH_SCALED_Y1_ADDR                              (( 0 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR))
#define G_PITCH_Y1_ADDR                                     (( 40 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR))
#define COD_AMR_SYNTH_ADDR                                  (( 300 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR))
#define F1_ADDR                                             (( 10 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR))
#define F2_ADDR                                             (( 22 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR))
#define SYN_FILT_TMP_ADDR                                   (( 0 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR))
#define INT_LPC_LSP_ADDR                                    (( 0 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR))
#define BUILD_CODE_POINT_ADDR_ADDR                          (( 0 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR))
#define AUTOCORR_Y_COPY_ADDR                                (( 4 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR))
#define LEVINSON_AL_ADDR                                    (( 4 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR))
#define PARA4_ADDR_ADDR                                     (( 3 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR))
#define CHEBPS_B2_ADDR                                      (( 10 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR))
#define Q_PLSF_5_LSF_P_ADDR                                 (( 0 + AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR))

/******************
 **  JPEG_LOCAL  **
 ******************/

//vpp_AmjrPreview
#define vpp_AmjrPreview_X_BEGIN_ADDR                        (0 + JPEG_LOCAL_X_BEGIN_ADDR)
#define BMP_IN_PREVIEW_BUFF                                 (0 + vpp_AmjrPreview_X_BEGIN_ADDR) //1280 short
#define BMP_OUT_PREVIEW_BUFF                                (1280 + vpp_AmjrPreview_X_BEGIN_ADDR) //1280 short

//vpp_AmjrPreview
#define vpp_AmjrPreview_Y_BEGIN_ADDR                        (0 + JPEG_LOCAL_Y_BEGIN_ADDR)
//LOCAL_PREVIEW_RESCALE
#define LOCAL_PREVIEW_RESCALE                               (0 + vpp_AmjrPreview_Y_BEGIN_ADDR)
#define LOCAL_PREVIEW_HEIGHT_DROP                           (0 + LOCAL_PREVIEW_RESCALE) //1 short
#define LOCAL_PREVIEW_HEIGHT_CROP                           (1 + LOCAL_PREVIEW_RESCALE) //1 short
#define LOCAL_PREVIEW_WIDTH_DROP                            (2 + LOCAL_PREVIEW_RESCALE) //1 short
#define LOCAL_PREVIEW_WIDTH_CROP                            (3 + LOCAL_PREVIEW_RESCALE) //1 short


//vpp_AmjrJpegEncode
#define vpp_AmjrJpegEncode_X_BEGIN_ADDR                     (0 + JPEG_LOCAL_X_BEGIN_ADDR)
//VPP_AMJR_JPEG_YUV_STRUCT
#define VPP_AMJR_JPEG_YUV_STRUCT                            (0 + vpp_AmjrJpegEncode_X_BEGIN_ADDR)
#define JPEGENC_BUFF_Y_ADDR                                 (0 + VPP_AMJR_JPEG_YUV_STRUCT) //64 short
#define JPEGENC_BUFF_Y1_ADDR                                (64 + VPP_AMJR_JPEG_YUV_STRUCT) //64 short
#define JPEGENC_BUFF_Y2_ADDR                                (128 + VPP_AMJR_JPEG_YUV_STRUCT) //64 short
#define JPEGENC_BUFF_Y3_ADDR                                (192 + VPP_AMJR_JPEG_YUV_STRUCT) //64 short
#define JPEGENC_BUFF_CB_ADDR                                (256 + VPP_AMJR_JPEG_YUV_STRUCT) //64 short
#define JPEGENC_BUFF_CR_ADDR                                (320 + VPP_AMJR_JPEG_YUV_STRUCT) //64 short

#define GLOBAL_4WIDTH_ADDR                                  (384 + vpp_AmjrJpegEncode_X_BEGIN_ADDR) //1 short
#define GLOBAL_OLD_DCY_ADDR                                 (385 + vpp_AmjrJpegEncode_X_BEGIN_ADDR) //1 short
#define GLOBAL_OLD_DCU_ADDR                                 (386 + vpp_AmjrJpegEncode_X_BEGIN_ADDR) //1 short
#define GLOBAL_OLD_DCV_ADDR                                 (387 + vpp_AmjrJpegEncode_X_BEGIN_ADDR) //1 short
//VPP_AMJR_JPEG_RGB_STRUCT
#define VPP_AMJR_JPEG_RGB_STRUCT                            (388 + vpp_AmjrJpegEncode_X_BEGIN_ADDR)
#define JPEGENC_BUFF_RGB_640_0_ADDR                         (0 + VPP_AMJR_JPEG_RGB_STRUCT) //640 short
#define JPEGENC_BUFF_RGB_640_1_ADDR                         (640 + VPP_AMJR_JPEG_RGB_STRUCT) //640 short
#define JPEGENC_BUFF_RGB_640_2_ADDR                         (1280 + VPP_AMJR_JPEG_RGB_STRUCT) //640 short
#define JPEGENC_BUFF_RGB_640_3_ADDR                         (1920 + VPP_AMJR_JPEG_RGB_STRUCT) //640 short
#define JPEGENC_BUFF_RGB_640_4_ADDR                         (2560 + VPP_AMJR_JPEG_RGB_STRUCT) //640 short
#define JPEGENC_BUFF_RGB_640_5_ADDR                         (3200 + VPP_AMJR_JPEG_RGB_STRUCT) //640 short
#define JPEGENC_BUFF_RGB_640_6_ADDR                         (3840 + VPP_AMJR_JPEG_RGB_STRUCT) //640 short
#define JPEGENC_BUFF_RGB_640_7_ADDR                         (4480 + VPP_AMJR_JPEG_RGB_STRUCT) //640 short
#define JPEGENC_BUFF_RGB_640_8_ADDR                         (5120 + VPP_AMJR_JPEG_RGB_STRUCT) //640 short


//vpp_AmjrJpegEncode
#define vpp_AmjrJpegEncode_Y_BEGIN_ADDR                     (0 + JPEG_LOCAL_Y_BEGIN_ADDR)
#define PUT_BITS_BUF                                        (0 + vpp_AmjrJpegEncode_Y_BEGIN_ADDR) //512 short
#define CONST_BLOCKSIZE_BYTE_ADDR                           (512 + vpp_AmjrJpegEncode_Y_BEGIN_ADDR) //2 short
#define CONST_WIDTH_ADD8                                    (514 + vpp_AmjrJpegEncode_Y_BEGIN_ADDR) //1 short
#define CONST_WIDTH_SUB8                                    (515 + vpp_AmjrJpegEncode_Y_BEGIN_ADDR) //1 short
#define CONST_WIDTH_MPY8                                    (516 + vpp_AmjrJpegEncode_Y_BEGIN_ADDR) //1 short
#define CONST_WIDTH_MPY8_ADD8                               (517 + vpp_AmjrJpegEncode_Y_BEGIN_ADDR) //1 short
#define GLOBAL_SHORT_1_ADDR                                 (518 + vpp_AmjrJpegEncode_Y_BEGIN_ADDR) //1 short
#define GLOBAL_CURR_POS_ADDR                                (519 + vpp_AmjrJpegEncode_Y_BEGIN_ADDR) //1 short
#define GLOBAL_CURRBITS_ADDR                                (520 + vpp_AmjrJpegEncode_Y_BEGIN_ADDR) //1 short
#define GLOBAL_CURRBYTE_ADDR                                (521 + vpp_AmjrJpegEncode_Y_BEGIN_ADDR) //1 short
#define GLOBAL_pp_ADDR                                      (522 + vpp_AmjrJpegEncode_Y_BEGIN_ADDR) //1 short
#define GLOBAL_Q_ADDR                                       (523 + vpp_AmjrJpegEncode_Y_BEGIN_ADDR) //1 short
//VPP_AMJR_JPEG_OUT_STRUCT
#define VPP_AMJR_JPEG_OUT_STRUCT                            (524 + vpp_AmjrJpegEncode_Y_BEGIN_ADDR)
#define GLOBAL_OUT_FOR1280BY1024_ADDR                       (0 + VPP_AMJR_JPEG_OUT_STRUCT) //48 short
#define GLOBAL_OUT_48_1_ADDR                                (48 + VPP_AMJR_JPEG_OUT_STRUCT) //48 short
#define GLOBAL_OUT_48_2_ADDR                                (96 + VPP_AMJR_JPEG_OUT_STRUCT) //48 short
#define GLOBAL_OUT_48_3_ADDR                                (144 + VPP_AMJR_JPEG_OUT_STRUCT) //48 short
#define GLOBAL_OUT_48_4_ADDR                                (192 + VPP_AMJR_JPEG_OUT_STRUCT) //48 short
#define GLOBAL_OUT_48_5_ADDR                                (240 + VPP_AMJR_JPEG_OUT_STRUCT) //48 short
#define GLOBAL_OUT_48_6_ADDR                                (288 + VPP_AMJR_JPEG_OUT_STRUCT) //48 short
#define GLOBAL_OUT_48_7_ADDR                                (336 + VPP_AMJR_JPEG_OUT_STRUCT) //48 short

#define GLOBAL_OUT_32_1_ADDR                                (32 + GLOBAL_OUT_FOR1280BY1024_ADDR)
#define GLOBAL_OUT_32_2_ADDR                                (32 + GLOBAL_OUT_32_1_ADDR)
#define GLOBAL_OUT_32_3_ADDR                                (32 + GLOBAL_OUT_32_2_ADDR)
#define GLOBAL_OUT_32_4_ADDR                                (32 + GLOBAL_OUT_32_3_ADDR)
#define GLOBAL_OUT_32_5_ADDR                                (32 + GLOBAL_OUT_32_4_ADDR)
#define GLOBAL_OUT_32_6_ADDR                                (32 + GLOBAL_OUT_32_5_ADDR)
#define GLOBAL_OUT_32_7_ADDR                                (32 + GLOBAL_OUT_32_6_ADDR)
#define CONST_MYHUFF_AC0_ADDR                               (908 + vpp_AmjrJpegEncode_Y_BEGIN_ADDR) //324 short
#define CONST_MYHUFF_AC1_ADDR                               (1232 + vpp_AmjrJpegEncode_Y_BEGIN_ADDR) //324 short

//COOLSAND_1280BY1024_JPEG
#define COOLSAND_1280BY1024_JPEG_Y_BEGIN_ADDR               (1556 + JPEG_LOCAL_Y_BEGIN_ADDR)
#define TEMP_BUFF_FOR1280BY1024_TMP1                        (0 + COOLSAND_1280BY1024_JPEG_Y_BEGIN_ADDR) //48 short
#define TEMP_BUFF_FOR1280BY1024_TMP2                        (48 + COOLSAND_1280BY1024_JPEG_Y_BEGIN_ADDR) //48 short

//apply_fdct
#define apply_fdct_Y_BEGIN_ADDR                             (1652 + JPEG_LOCAL_Y_BEGIN_ADDR)
#define TEMP_BUFF_64_ADDR                                   (0 + apply_fdct_Y_BEGIN_ADDR) //32 short

/********************
 **  COMMON_LOCAL  **
 ********************/

//RAM_X: size 0x2800, used 0x27d4   RAM_Y: size 0x2800, used 0x27e6

#endif
