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
#define COMMON_GLOBAL_Y_SIZE                                    74
#define AAC_GLOBAL_X_SIZE                                       10128
#define AAC_GLOBAL_Y_SIZE                                       10090
#define AAC_LOCAL_X_SIZE                                        0
#define AAC_LOCAL_Y_SIZE                                        0
#define RM_GLOBAL_X_SIZE                                        3256
#define RM_GLOBAL_Y_SIZE                                        7540
#define RM_LOCAL_X_SIZE                                         4096
#define RM_LOCAL_Y_SIZE                                         1162
#define H264_GLOBAL_X_SIZE                                      8314
#define H264_GLOBAL_Y_SIZE                                      8286
#define H264_LOCAL_X_SIZE                                       84
#define H264_LOCAL_Y_SIZE                                       20
#define MPEG4_GLOBAL_X_SIZE                                     8698
#define MPEG4_GLOBAL_Y_SIZE                                     9684
#define MPEG4_LOCAL_X_SIZE                                      4
#define MPEG4_LOCAL_Y_SIZE                                      0
#define H263_GLOBAL_X_SIZE                                      3156
#define H263_GLOBAL_Y_SIZE                                      1380
#define H263_LOCAL_X_SIZE                                       0
#define H263_LOCAL_Y_SIZE                                       0
#define JPEG2_GLOBAL_X_SIZE                                     9600
#define JPEG2_GLOBAL_Y_SIZE                                     9470
#define JPEG2_LOCAL_X_SIZE                                      0
#define JPEG2_LOCAL_Y_SIZE                                      0
#define MP3_GLOBAL_X_SIZE                                       10156
#define MP3_GLOBAL_Y_SIZE                                       9818
#define MP3_LOCAL_X_SIZE                                        68
#define MP3_LOCAL_Y_SIZE                                        338
#define AMR_GLOBAL_X_SIZE                                       1120
#define AMR_GLOBAL_Y_SIZE                                       6532
#define AMR_LOCAL_X_SIZE                                        578
#define AMR_LOCAL_Y_SIZE                                        240
#define JPEG_GLOBAL_X_SIZE                                      9184
#define JPEG_GLOBAL_Y_SIZE                                      10104
#define JPEG_LOCAL_X_SIZE                                       0
#define JPEG_LOCAL_Y_SIZE                                       0
#define H263zoom_GLOBAL_X_SIZE                                  8272
#define H263zoom_GLOBAL_Y_SIZE                                  9890
#define H263zoom_LOCAL_X_SIZE                                   0
#define H263zoom_LOCAL_Y_SIZE                                   64
#define SBCENC_GLOBAL_X_SIZE                                    7404
#define SBCENC_GLOBAL_Y_SIZE                                    9992
#define SBCENC_LOCAL_X_SIZE                                     1786
#define SBCENC_LOCAL_Y_SIZE                                     140
#define DRCMODE_GLOBAL_X_SIZE                                   9160
#define DRCMODE_GLOBAL_Y_SIZE                                   9790
#define DRCMODE_LOCAL_X_SIZE                                    0
#define DRCMODE_LOCAL_Y_SIZE                                    0
#define COMMON_LOCAL_X_SIZE                                     0
#define COMMON_LOCAL_Y_SIZE                                     0

/**************************
 **  SECTION_BEGIN_ADDR  **
 **************************/

#define COMMON_GLOBAL_X_BEGIN_ADDR                              (0 + RAM_X_BEGIN_ADDR)
#define COMMON_GLOBAL_Y_BEGIN_ADDR                              (0 + RAM_Y_BEGIN_ADDR)
#define AAC_GLOBAL_X_BEGIN_ADDR                                 (0 + RAM_X_BEGIN_ADDR)
#define AAC_GLOBAL_Y_BEGIN_ADDR                                 (74 + RAM_Y_BEGIN_ADDR)
#define AAC_LOCAL_X_BEGIN_ADDR                                  (10128 + RAM_X_BEGIN_ADDR)
#define AAC_LOCAL_Y_BEGIN_ADDR                                  (10164 + RAM_Y_BEGIN_ADDR)
#define RM_GLOBAL_X_BEGIN_ADDR                                  (0 + RAM_X_BEGIN_ADDR)
#define RM_GLOBAL_Y_BEGIN_ADDR                                  (74 + RAM_Y_BEGIN_ADDR)
#define RM_LOCAL_X_BEGIN_ADDR                                   (3256 + RAM_X_BEGIN_ADDR)
#define RM_LOCAL_Y_BEGIN_ADDR                                   (7614 + RAM_Y_BEGIN_ADDR)
#define H264_GLOBAL_X_BEGIN_ADDR                                (0 + RAM_X_BEGIN_ADDR)
#define H264_GLOBAL_Y_BEGIN_ADDR                                (74 + RAM_Y_BEGIN_ADDR)
#define H264_LOCAL_X_BEGIN_ADDR                                 (8314 + RAM_X_BEGIN_ADDR)
#define H264_LOCAL_Y_BEGIN_ADDR                                 (8360 + RAM_Y_BEGIN_ADDR)
#define MPEG4_GLOBAL_X_BEGIN_ADDR                               (0 + RAM_X_BEGIN_ADDR)
#define MPEG4_GLOBAL_Y_BEGIN_ADDR                               (74 + RAM_Y_BEGIN_ADDR)
#define MPEG4_LOCAL_X_BEGIN_ADDR                                (8698 + RAM_X_BEGIN_ADDR)
#define MPEG4_LOCAL_Y_BEGIN_ADDR                                (9758 + RAM_Y_BEGIN_ADDR)
#define H263_GLOBAL_X_BEGIN_ADDR                                (0 + RAM_X_BEGIN_ADDR)
#define H263_GLOBAL_Y_BEGIN_ADDR                                (74 + RAM_Y_BEGIN_ADDR)
#define H263_LOCAL_X_BEGIN_ADDR                                 (3156 + RAM_X_BEGIN_ADDR)
#define H263_LOCAL_Y_BEGIN_ADDR                                 (1454 + RAM_Y_BEGIN_ADDR)
#define JPEG2_GLOBAL_X_BEGIN_ADDR                               (0 + RAM_X_BEGIN_ADDR)
#define JPEG2_GLOBAL_Y_BEGIN_ADDR                               (74 + RAM_Y_BEGIN_ADDR)
#define JPEG2_LOCAL_X_BEGIN_ADDR                                (9600 + RAM_X_BEGIN_ADDR)
#define JPEG2_LOCAL_Y_BEGIN_ADDR                                (9544 + RAM_Y_BEGIN_ADDR)
#define MP3_GLOBAL_X_BEGIN_ADDR                                 (0 + RAM_X_BEGIN_ADDR)
#define MP3_GLOBAL_Y_BEGIN_ADDR                                 (74 + RAM_Y_BEGIN_ADDR)
#define MP3_LOCAL_X_BEGIN_ADDR                                  (10156 + RAM_X_BEGIN_ADDR)
#define MP3_LOCAL_Y_BEGIN_ADDR                                  (9892 + RAM_Y_BEGIN_ADDR)
#define AMR_GLOBAL_X_BEGIN_ADDR                                 (0 + RAM_X_BEGIN_ADDR)
#define AMR_GLOBAL_Y_BEGIN_ADDR                                 (74 + RAM_Y_BEGIN_ADDR)
#define AMR_LOCAL_X_BEGIN_ADDR                                  (1120 + RAM_X_BEGIN_ADDR)
#define AMR_LOCAL_Y_BEGIN_ADDR                                  (6606 + RAM_Y_BEGIN_ADDR)
#define JPEG_GLOBAL_X_BEGIN_ADDR                                (0 + RAM_X_BEGIN_ADDR)
#define JPEG_GLOBAL_Y_BEGIN_ADDR                                (74 + RAM_Y_BEGIN_ADDR)
#define JPEG_LOCAL_X_BEGIN_ADDR                                 (9184 + RAM_X_BEGIN_ADDR)
#define JPEG_LOCAL_Y_BEGIN_ADDR                                 (10178 + RAM_Y_BEGIN_ADDR)
#define H263zoom_GLOBAL_X_BEGIN_ADDR                            (0 + RAM_X_BEGIN_ADDR)
#define H263zoom_GLOBAL_Y_BEGIN_ADDR                            (74 + RAM_Y_BEGIN_ADDR)
#define H263zoom_LOCAL_X_BEGIN_ADDR                             (8272 + RAM_X_BEGIN_ADDR)
#define H263zoom_LOCAL_Y_BEGIN_ADDR                             (9964 + RAM_Y_BEGIN_ADDR)
#define SBCENC_GLOBAL_X_BEGIN_ADDR                              (0 + RAM_X_BEGIN_ADDR)
#define SBCENC_GLOBAL_Y_BEGIN_ADDR                              (74 + RAM_Y_BEGIN_ADDR)
#define SBCENC_LOCAL_X_BEGIN_ADDR                               (7404 + RAM_X_BEGIN_ADDR)
#define SBCENC_LOCAL_Y_BEGIN_ADDR                               (10066 + RAM_Y_BEGIN_ADDR)
#define DRCMODE_GLOBAL_X_BEGIN_ADDR                             (0 + RAM_X_BEGIN_ADDR)
#define DRCMODE_GLOBAL_Y_BEGIN_ADDR                             (74 + RAM_Y_BEGIN_ADDR)
#define DRCMODE_LOCAL_X_BEGIN_ADDR                              (9160 + RAM_X_BEGIN_ADDR)
#define DRCMODE_LOCAL_Y_BEGIN_ADDR                              (9864 + RAM_Y_BEGIN_ADDR)
#define COMMON_LOCAL_X_BEGIN_ADDR                               (10156 + RAM_X_BEGIN_ADDR)
#define COMMON_LOCAL_Y_BEGIN_ADDR                               (10164 + RAM_Y_BEGIN_ADDR)

/*********************
 **  COMMON_GLOBAL  **
 *********************/

//GLOBAL_VARS
#define Input_mode_addr                                     (0 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Input_reset_addr                                    (1 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Flag_need_bluetooth_ADDR                            (2 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Input_audioItf_ADDR                                 (3 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Input_inStreamBufAddr_addr                          (4 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define Input_outStreamBufAddr_addr                         (6 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define DEC_EQ_Type_ADDR                                    (8 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Input_zoom_mode_addr                                (9 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Source_width_ADDR                                   (10 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STRUCT_sbc_frame_mode                               (Source_width_ADDR)
#define Source_height_ADDR                                  (11 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STRUCT_sbc_frame_allocation                         (Source_height_ADDR)
#define Cut_width_ADDR                                      (12 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STRUCT_sbc_frame_bitpool                            (Cut_width_ADDR)
#define Cut_height_ADDR                                     (13 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STRUCT_sbc_frame_frequency                          (Cut_height_ADDR)
#define Zoomed_width_ADDR                                   (14 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Zoomed_height_ADDR                                  (15 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define INPUT_ADDR_U_ADDR                                   (16 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define INPUT_ADDR_V_ADDR                                   (18 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define Output_mode_addr                                    (20 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Output_nbChannel_addr                               (21 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Output_output_len_addr                              (22 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define Output_SampleRate_addr                              (24 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define Output_BitRate_addr                                 (26 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Output_consumedLen_addr                             (27 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Output_streamStatus_addr                            (28 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Output_ERR_Status_addr                              (29 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Output_image_ImgWidth_addr                          (30 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Output_image_ImgHeight_addr                         (31 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Output_Reserve_for_bar                              (32 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define MP3_FRAMESIZE                                       ((Output_output_len_addr))
#define STRUCT_SBC_RESET_ADDR                               ((Output_Reserve_for_bar))
#define STRUCT_SBC_RESAMPLE_RATE_ADDR                       ((Output_Reserve_for_bar+1))
#define Speak_AntiDistortion_Filter_Coef_addr               (34 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define Current_Audio_Mode_addr                             (36 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Current_Video_Mode_addr                             (37 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define AAC_Code_ExternalAddr_addr                          (38 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define MP3L12_ConstX_ExternalAddr_addr                     (40 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define AAC_ConstY_ExternalAddr_addr                        (42 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define Tab_huffTabSpec_START_addr                          (44 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define Tab_cos4sin4tab_START_addr                          (46 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define Tab_twidTabOdd_START_addr                           (48 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define Tab_sinWindow_START_addr                            (50 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define MP3_Code_ExternalAddr_addr                          (52 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define MP3_ConstX_ExternalAddr_addr                        (54 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define MP3_ConstY_ExternalAddr_addr                        (56 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define AMR_Dec_Code_ExternalAddr_addr                      (58 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define AMR_Dec_ConstX_ExternalAddr_addr                    (60 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define AMR_Dec_ConstY_ExternalAddr_addr                    (62 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define JPEG_Dec_Code_ExternalAddr_addr                     (64 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define DRC_MODE_Code_ExternalAddr_addr                     (JPEG_Dec_Code_ExternalAddr_addr)
#define JPEG_Dec_ConstY_ExternalAddr_addr                   (66 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define DRC_MODE_Const_ExternalAddr_addr                    (JPEG_Dec_ConstY_ExternalAddr_addr)
#define H263_Zoom_Code_ExternalAddr_addr                    (68 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define DRC_MODE_historydata_ExternalAddr_addr              (H263_Zoom_Code_ExternalAddr_addr)
#define H263_Zoom_ConstX_ExternalAddr_addr                  (70 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define SBC_ENC_Code_ExternalAddr_addr                      (72 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short

/******************
 **  AAC_GLOBAL  **
 ******************/

//GLOBAL_VARS
#define STATIC_readBuf_addr                                 (0 + AAC_GLOBAL_X_BEGIN_ADDR) //1024 short
#define STRUCT_AACDecInfo_PSInfoBase_overlap_addr           (1024 + AAC_GLOBAL_X_BEGIN_ADDR) //2048 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsLPCBuf_addr         (3072 + AAC_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsWorkBuf_addr            (3112 + AAC_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr         (3152 + AAC_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_AACDecInfo_PSInfoBase_prevWinShape_addr          (3154 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_prevWinShape1_addr         (3155 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_bitbuffer_addr                               (3156 + AAC_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_bytesLeft_addr                               (3158 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_inputbuffiled_ptr_addr                       (3159 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_inputptr_addr                                (3160 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_BITCACHE_addr                                (3161 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_TNSFilter_nWindows_addr                       (3162 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_TNSFilter_winLen_addr                         (3163 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_TNSFilter_nSFB_addr                           (3164 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_TNSFilter_maxOrder_addr                       (3165 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_TNSFilter_tnsMaxBand_addr                     (3166 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_TNSFilter_order_addr                          (3167 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_TNSFilter_gbMask_addr                         (3168 + AAC_GLOBAL_X_BEGIN_ADDR) //2 short
#define LOCAL_Dequantize_gbMask_addr                        (3170 + AAC_GLOBAL_X_BEGIN_ADDR) //2 short
#define LOCAL_DecodeSectionData_maxSFB                      (3172 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_PNS_gp_M_maxSFB_addr                          (3173 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_PNS_CH_addr                                   (3174 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_PNS_maxSFB_addr                               (3175 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
//STRUCT_AACDecInfo_addr
#define STRUCT_AACDecInfo_addr                              (3176 + AAC_GLOBAL_X_BEGIN_ADDR)
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

//STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr
#define STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr            (3198 + AAC_GLOBAL_X_BEGIN_ADDR)
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
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr           (3212 + AAC_GLOBAL_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_icsResBit_addr         (0 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_winSequence_addr           (1 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_winShape_addr          (2 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_maxSFB_addr            (3 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_sfGroup_addr           (4 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_predictorDataPresent_addr          (5 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_numWinGroup_addr           (6 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo_winGroupLen_addr           (7 + STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr) //8 short

//STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr          (3227 + AAC_GLOBAL_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_icsResBit_addr            (0 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_winSequence_addr          (1 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_winShape_addr         (2 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_maxSFB_addr           (3 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_sfGroup_addr          (4 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_predictorDataPresent_addr         (5 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_numWinGroup_addr          (6 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_icsInfo1_winGroupLen_addr          (7 + STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr) //8 short

#define RESERVED_FOR_H263                                   (3242 + AAC_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_outbuf_addr                                  (3244 + AAC_GLOBAL_X_BEGIN_ADDR) //2048 short
#define STRUCT_AACDecInfo_PSInfoBase_coef_addr              (5292 + AAC_GLOBAL_X_BEGIN_ADDR) //2048 short
#define STRUCT_AACDecInfo_PSInfoBase_coef1_addr             (7340 + AAC_GLOBAL_X_BEGIN_ADDR) //2048 short
#define STRUCT_AACDecInfo_PSInfoBase_commonWin_addr         (9388 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_scaleFactors_addr          (9389 + AAC_GLOBAL_X_BEGIN_ADDR) //120 short
#define STRUCT_AACDecInfo_PSInfoBase_scaleFactors1_addr         (9509 + AAC_GLOBAL_X_BEGIN_ADDR) //120 short
#define STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook_addr           (9629 + AAC_GLOBAL_X_BEGIN_ADDR) //120 short
#define STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook1_addr          (9749 + AAC_GLOBAL_X_BEGIN_ADDR) //120 short
#define STRUCT_AACDecInfo_PSInfoBase_msMaskPresent_addr         (9869 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_msMaskBits_addr            (9870 + AAC_GLOBAL_X_BEGIN_ADDR) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_pnsUsed_addr           (9878 + AAC_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_AACDecInfo_PSInfoBase_pnsLastVal_addr            (9880 + AAC_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_AACDecInfo_PSInfoBase_intensityUsed_addr         (9882 + AAC_GLOBAL_X_BEGIN_ADDR) //2 short
//STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr         (9884 + AAC_GLOBAL_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo_pulseDataPresent_addr            (0 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo_numPulse_addr            (1 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo_startSFB_addr            (2 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo_offset_addr          (3 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr) //4 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo_amp_addr         (7 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr) //4 short

//STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr            (9895 + AAC_GLOBAL_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_pulseDataPresent_addr           (0 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_numPulse_addr           (1 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_startSFB_addr           (2 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_offset_addr         (3 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr) //4 short
#define STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_amp_addr            (7 + STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr) //4 short

//STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr           (9906 + AAC_GLOBAL_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_tnsDataPresent_addr            (0 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_numFilt_addr           (1 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_coefRes_addr           (9 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_length_addr            (17 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_order_addr         (25 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_dir_addr           (33 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo_coef_addr          (41 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr) //60 short

//STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr          (10007 + AAC_GLOBAL_X_BEGIN_ADDR)
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_tnsDataPresent_addr           (0 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_numFilt_addr          (1 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_coefRes_addr          (9 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_length_addr           (17 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_order_addr            (25 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_dir_addr          (33 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr) //8 short
#define STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_coef_addr         (41 + STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr) //60 short

#define LOCAL_DequantBlock_tab4_addr                        (10108 + AAC_GLOBAL_X_BEGIN_ADDR) //8 short
#define LOCAL_Dequantize_sfbTab_addr_addr                   (10116 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_Dequantize_gbCurrent_addr_addr                (10117 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_Dequantize_numWinGroup_addr                   (10118 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_Dequantize_maxSFB_addr                        (10119 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_PNS_gbMask_addr                               (10120 + AAC_GLOBAL_X_BEGIN_ADDR) //2 short
#define LOCAL_PNS_numWinGroup_addr                          (10122 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_PNS_checkCorr_addr                            (10123 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_PNS_genNew_addr                               (10124 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_PNS_nSamps_addr                               (10125 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_PNS_gbCurrent_addr_addr                       (10126 + AAC_GLOBAL_X_BEGIN_ADDR) //1 short

//GLOBAL_VARS
#define GLOBAL_NON_CLIP_HISTORYDATA_L_AAC_ADDR              (0 + AAC_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_NON_CLIP_HISTORYDATA_R_AAC_ADDR              (2 + AAC_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_DIGITAL_GAIN_AAC_ADDR                        (4 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DIGITAL_MAXVALUE_AAC_ADDR                    (5 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Output_Display_Bars_addr                            (6 + AAC_GLOBAL_Y_BEGIN_ADDR) //4 short
#define Tab_twidTabEven_addr                                (10 + AAC_GLOBAL_Y_BEGIN_ADDR) //1008 short
#define Tab_start_addr                                      (Tab_twidTabEven_addr)
#define Tab_cos1sin1tab_addr                                (1018 + AAC_GLOBAL_Y_BEGIN_ADDR) //1028 short
#define Tab_bitrevtab_addr                                  (2046 + AAC_GLOBAL_Y_BEGIN_ADDR) //146 short
#define Tab_invQuant3_addr                                  (2192 + AAC_GLOBAL_Y_BEGIN_ADDR) //32 short
#define Tab_invQuant4_addr                                  (2224 + AAC_GLOBAL_Y_BEGIN_ADDR) //32 short
#define Tab_sfBandTotal_addr                                (2256 + AAC_GLOBAL_Y_BEGIN_ADDR) //12 short
#define Tab_tnsMaxBands_addr                                (2268 + AAC_GLOBAL_Y_BEGIN_ADDR) //12 short
#define Tab_pow14_addr                                      (2280 + AAC_GLOBAL_Y_BEGIN_ADDR) //8 short
#define Tab_pow43_14_addr                                   (2288 + AAC_GLOBAL_Y_BEGIN_ADDR) //128 short
#define Tab_pow43_addr                                      (2416 + AAC_GLOBAL_Y_BEGIN_ADDR) //96 short
#define Tab_poly43lo_addr                                   (2512 + AAC_GLOBAL_Y_BEGIN_ADDR) //10 short
#define Tab_poly43hi_addr                                   (2522 + AAC_GLOBAL_Y_BEGIN_ADDR) //10 short
#define Tab_pow2exp_addr                                    (2532 + AAC_GLOBAL_Y_BEGIN_ADDR) //8 short
#define Tab_pow2frac_addr                                   (2540 + AAC_GLOBAL_Y_BEGIN_ADDR) //16 short
#define Tab_sfBandTabLongOffset_addr                        (2556 + AAC_GLOBAL_Y_BEGIN_ADDR) //12 short
#define Tab_sfBandTabLong_addr                              (2568 + AAC_GLOBAL_Y_BEGIN_ADDR) //326 short
#define Tab_huffTabSpecInfo_addr                            (2894 + AAC_GLOBAL_Y_BEGIN_ADDR) //242 short
#define Tab_sfBandTabShortOffset_addr                       (3136 + AAC_GLOBAL_Y_BEGIN_ADDR) //12 short
#define Tab_sfBandTabShort_addr                             (3148 + AAC_GLOBAL_Y_BEGIN_ADDR) //76 short
#define Tab_huffTabScaleFact_addr                           (3224 + AAC_GLOBAL_Y_BEGIN_ADDR) //122 short
#define Tab_huffTabScaleFactInfo_addr                       (3346 + AAC_GLOBAL_Y_BEGIN_ADDR) //22 short
#define Tab_channelMapTab_addr                              (3368 + AAC_GLOBAL_Y_BEGIN_ADDR) //8 short
#define Tab_sampRateTab_addr                                (3376 + AAC_GLOBAL_Y_BEGIN_ADDR) //24 short
#define Tab_elementNumChans_addr                            (3400 + AAC_GLOBAL_Y_BEGIN_ADDR) //8 short
#define const_SQRT1_2_addr                                  (3408 + AAC_GLOBAL_Y_BEGIN_ADDR) //2 short
#define const_0x80000000_addr                               (3410 + AAC_GLOBAL_Y_BEGIN_ADDR) //2 short
#define const_0x7fffffff_addr                               (3412 + AAC_GLOBAL_Y_BEGIN_ADDR) //2 short
#define const_0_addr                                        (3414 + AAC_GLOBAL_Y_BEGIN_ADDR) //2 short
#define const_1_addr                                        (3416 + AAC_GLOBAL_Y_BEGIN_ADDR) //2 short
#define const_2_addr                                        (3418 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define const_3_addr                                        (3419 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define const_4_addr                                        (3420 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define const_5_addr                                        (3421 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define const_6_addr                                        (3422 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define const_7_addr                                        (3423 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define const_8_addr                                        (3424 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define const_9_addr                                        (3425 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define const_10_addr                                       (3426 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define const_11_addr                                       (3427 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define const_13_addr                                       (3428 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define const_14_addr                                       (3429 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define const_15_addr                                       (3430 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define const_16_addr                                       (3431 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define const_32_addr                                       (3432 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define const_0x0fff_addr                                   (3433 + AAC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STRUCT_AACDecInfo_PSInfoBase_overlap1_addr          (3434 + AAC_GLOBAL_Y_BEGIN_ADDR) //2048 short
#define Tab_DMA_start_addr                                  (5482 + AAC_GLOBAL_Y_BEGIN_ADDR) //4608 short
#define Tab_DMA_start_word_addr                             (Tab_DMA_start_addr/2)
#define Tab_huffTabSpec_addr                                (Tab_DMA_start_addr)
#define Tab_cos4sin4tab_addr                                (Tab_DMA_start_addr)
#define Tab_twidTabOdd_addr                                 (Tab_DMA_start_addr+2304)
#define Tab_Equalizer_long_addr                             (Tab_twidTabOdd_addr+2016)
#define Tab_Equalizer_long_SampleFrequeyBin_Map_addr            (Tab_Equalizer_long_addr+132)
#define Tab_Equalizer_short_addr                            (Tab_Equalizer_long_SampleFrequeyBin_Map_addr+12)
#define Tab_Equalizer_short_SampleFrequeyBin_Map_addr           (Tab_Equalizer_short_addr+132)
#define Tab_sinWindow_addr                                  (Tab_DMA_start_addr)
#define Tab_kbdWindow_addr                                  (Tab_DMA_start_addr+2304)

/*****************
 **  AAC_LOCAL  **
 *****************/

/*****************
 **  RM_GLOBAL  **
 *****************/

//GLOBAL_VARS
#define RMVB_FOR_MP3_AMR_RAM_X_ALIGNED                      (0 + RM_GLOBAL_X_BEGIN_ADDR) //3244 short
//VPP_RMVB_CFG_STRUCT
#define VPP_RMVB_CFG_STRUCT                                 (3244 + RM_GLOBAL_X_BEGIN_ADDR)
#define GLOBAL_RM_RESET                                     (0 + VPP_RMVB_CFG_STRUCT) //1 short
#define GLOBAL_RM_STATUS                                    (1 + VPP_RMVB_CFG_STRUCT) //1 short
#define GLOBAL_MB_NUM                                       (2 + VPP_RMVB_CFG_STRUCT) //1 short
#define GLOBAL_RV8_FLAG                                     (3 + VPP_RMVB_CFG_STRUCT) //1 short
#define GLOBAL_PIC_WIDTH                                    (4 + VPP_RMVB_CFG_STRUCT) //1 short
#define GLOBAL_PIC_HEIGHT                                   (5 + VPP_RMVB_CFG_STRUCT) //1 short
#define GLOBAL_PITCH                                        (6 + VPP_RMVB_CFG_STRUCT) //1 short
#define GLOBAL_FILTER_PASS                                  (7 + VPP_RMVB_CFG_STRUCT) //1 short
#define GLOBAL_PARA_START_ADDR                              (8 + VPP_RMVB_CFG_STRUCT) //2 short
#define GLOBAL_CONTEX_STORE_ADDR                            (10 + VPP_RMVB_CFG_STRUCT) //2 short


//GLOBAL_VARS
#define RMVB_FOR_MP3_AMR_RAM_Y_ALIGNED                      (0 + RM_GLOBAL_Y_BEGIN_ADDR) //7456 short
//RMVB_SHORT_Constant
#define RMVB_SHORT_Constant                                 (7456 + RM_GLOBAL_Y_BEGIN_ADDR)
#define CONST_RMVB_DEC_0x0001_ADDR                          (0 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x0002_ADDR                          (1 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x0003_ADDR                          (2 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x0004_ADDR                          (3 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x0005_ADDR                          (4 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x0006_ADDR                          (5 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x0007_ADDR                          (6 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x0008_ADDR                          (7 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x0009_ADDR                          (8 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x000A_ADDR                          (9 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x000B_ADDR                          (10 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x000C_ADDR                          (11 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x000D_ADDR                          (12 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x000E_ADDR                          (13 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x000F_ADDR                          (14 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x0010_ADDR                          (15 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x0014_ADDR                          (16 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x0018_ADDR                          (17 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x0020_ADDR                          (18 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x0040_ADDR                          (19 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x0100_ADDR                          (20 + RMVB_SHORT_Constant) //1 short
#define CONST_RMVB_DEC_0x0180_ADDR                          (21 + RMVB_SHORT_Constant) //1 short

//RMVB_LONG_Constant
#define RMVB_LONG_Constant                                  (7478 + RM_GLOBAL_Y_BEGIN_ADDR)
#define CONST_RMVB_DEC_0x00000001_ADDR                      (0 + RMVB_LONG_Constant) //2 short
#define CONST_RMVB_DEC_0x00000004_ADDR                      (2 + RMVB_LONG_Constant) //2 short
#define CONST_RMVB_DEC_0x00000200_ADDR                      (4 + RMVB_LONG_Constant) //2 short
#define CONST_DMA_UNSIGNB2SRD_0x10000000_ADDR               (6 + RMVB_LONG_Constant) //2 short
#define CONST_DMA_UNSIGNB2SWR_0x50000000_ADDR               (8 + RMVB_LONG_Constant) //2 short
#define CONST_DMA_SIGNB2SRD_0x30000000_ADDR                 (10 + RMVB_LONG_Constant) //2 short
#define CONST_DMA_SIGNB2SWR_0x70000000_ADDR                 (12 + RMVB_LONG_Constant) //2 short

//RMVB_TABLE_Dquanttab
#define RMVB_TABLE_Dquanttab                                (7492 + RM_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_dec_single_scan                               (0 + RMVB_TABLE_Dquanttab) //16 short
#define TABLE_FILTER_DITHER                                 (16 + RMVB_TABLE_Dquanttab) //32 short


/****************
 **  RM_LOCAL  **
 ****************/

//CII_RMVBMbDec
#define CII_RMVBMbDec_X_BEGIN_ADDR                          (0 + RM_LOCAL_X_BEGIN_ADDR)
//RMVB_DATA_PARA_STRUCT
#define RMVB_DATA_PARA_STRUCT                               (0 + CII_RMVBMbDec_X_BEGIN_ADDR)
#define INTRA16_16                                          (0 + RMVB_DATA_PARA_STRUCT) //1 short
#define INTRA_FLAG                                          (1 + RMVB_DATA_PARA_STRUCT) //1 short
#define MB_TOP                                              (2 + RMVB_DATA_PARA_STRUCT) //1 short
#define MB_LEFT                                             (3 + RMVB_DATA_PARA_STRUCT) //1 short
#define MB_TOPRIGHT                                         (4 + RMVB_DATA_PARA_STRUCT) //1 short
#define MB_OFFSETX                                          (5 + RMVB_DATA_PARA_STRUCT) //1 short
#define BIDIR_PRED                                          (6 + RMVB_DATA_PARA_STRUCT) //1 short
#define MB_PSKIP                                            (7 + RMVB_DATA_PARA_STRUCT) //1 short
#define RATIO_FW                                            (8 + RMVB_DATA_PARA_STRUCT) //1 short
#define RATIO_BW                                            (9 + RMVB_DATA_PARA_STRUCT) //1 short
#define INTER16                                             (10 + RMVB_DATA_PARA_STRUCT) //1 short
#define INTER16_16                                          (11 + RMVB_DATA_PARA_STRUCT) //1 short
#define DEQUANT_Y_QP                                        (12 + RMVB_DATA_PARA_STRUCT) //1 short
#define DEQUANT_Y_QDC0                                      (13 + RMVB_DATA_PARA_STRUCT) //1 short
#define DEQUANT_Y_QDC                                       (14 + RMVB_DATA_PARA_STRUCT) //1 short
#define DEQUANT_C_QDC                                       (15 + RMVB_DATA_PARA_STRUCT) //1 short
#define DEQUANT_C_QP                                        (16 + RMVB_DATA_PARA_STRUCT) //1 short
#define FILTER_ALPHA                                        (17 + RMVB_DATA_PARA_STRUCT) //1 short
#define FILTER_BETA                                         (18 + RMVB_DATA_PARA_STRUCT) //1 short
#define FILTER_BETA2                                        (19 + RMVB_DATA_PARA_STRUCT) //1 short
#define FILTER_BETA2_C                                      (20 + RMVB_DATA_PARA_STRUCT) //1 short
#define FILTER_MB_CLIP0                                     (21 + RMVB_DATA_PARA_STRUCT) //1 short
#define FILTER_MB_CLIP2                                     (22 + RMVB_DATA_PARA_STRUCT) //1 short
#define FILTER_MB_CLIP                                      (23 + RMVB_DATA_PARA_STRUCT) //1 short
#define FILTER_MB_CLIP_LEFT                                 (24 + RMVB_DATA_PARA_STRUCT) //1 short
#define FILTER_MB_CLIP_UP                                   (25 + RMVB_DATA_PARA_STRUCT) //1 short
#define REFDIFF_UP                                          (26 + RMVB_DATA_PARA_STRUCT) //1 short
#define REFDIFF_LEFT                                        (27 + RMVB_DATA_PARA_STRUCT) //1 short
#define BNZS_UP                                             (28 + RMVB_DATA_PARA_STRUCT) //2 short
#define BNZS_LEFT                                           (30 + RMVB_DATA_PARA_STRUCT) //2 short
#define BNZS                                                (32 + RMVB_DATA_PARA_STRUCT) //2 short
#define BH_FILTER                                           (34 + RMVB_DATA_PARA_STRUCT) //2 short
#define BV_FILTER                                           (36 + RMVB_DATA_PARA_STRUCT) //2 short
#define H_FILTER                                            (38 + RMVB_DATA_PARA_STRUCT) //2 short
#define V_FILTER                                            (40 + RMVB_DATA_PARA_STRUCT) //2 short
#define MB_CBP                                              (42 + RMVB_DATA_PARA_STRUCT) //2 short
#define VLD_COF                                             (44 + RMVB_DATA_PARA_STRUCT) //400 short
#define INTRA_MODE                                          (444 + RMVB_DATA_PARA_STRUCT) //16 short
#define MOTION_VECTORS                                      (460 + RMVB_DATA_PARA_STRUCT) //16 short
#define INTERPOLATE_YBLK_ADDR                               (476 + RMVB_DATA_PARA_STRUCT) //8 short
#define INTERPOLATE_UBLK_ADDR                               (484 + RMVB_DATA_PARA_STRUCT) //8 short
#define INTERPOLATE_VBLK_ADDR                               (492 + RMVB_DATA_PARA_STRUCT) //8 short
#define INTERPOLATE_YBLK_B_ADDR                             (500 + RMVB_DATA_PARA_STRUCT) //8 short
#define INTERPOLATE_UBLK_B_ADDR                             (508 + RMVB_DATA_PARA_STRUCT) //8 short
#define INTERPOLATE_VBLK_B_ADDR                             (516 + RMVB_DATA_PARA_STRUCT) //8 short
#define OUT_BUFFER_Y_PTR                                    (524 + RMVB_DATA_PARA_STRUCT) //2 short
#define OUT_BUFFER_U_PTR                                    (526 + RMVB_DATA_PARA_STRUCT) //2 short
#define OUT_BUFFER_V_PTR                                    (528 + RMVB_DATA_PARA_STRUCT) //2 short

//RMVB_LOCAL_INTER_BLK
#define RMVB_LOCAL_INTER_BLK                                (530 + CII_RMVBMbDec_X_BEGIN_ADDR)
#define LOCAL_INTERPOLATE_YBLK0                             (0 + RMVB_LOCAL_INTER_BLK) //208 short
#define LOCAL_INTERPOLATE_YBLK1                             (208 + RMVB_LOCAL_INTER_BLK) //208 short
#define LOCAL_INTERPOLATE_YBLK2                             (416 + RMVB_LOCAL_INTER_BLK) //208 short
#define LOCAL_INTERPOLATE_YBLK3                             (624 + RMVB_LOCAL_INTER_BLK) //208 short
#define LOCAL_INTERPOLATE_UBLK0                             (832 + RMVB_LOCAL_INTER_BLK) //40 short
#define LOCAL_INTERPOLATE_UBLK1                             (872 + RMVB_LOCAL_INTER_BLK) //40 short
#define LOCAL_INTERPOLATE_UBLK2                             (912 + RMVB_LOCAL_INTER_BLK) //40 short
#define LOCAL_INTERPOLATE_UBLK3                             (952 + RMVB_LOCAL_INTER_BLK) //40 short
#define LOCAL_INTERPOLATE_VBLK0                             (992 + RMVB_LOCAL_INTER_BLK) //40 short
#define LOCAL_INTERPOLATE_VBLK1                             (1032 + RMVB_LOCAL_INTER_BLK) //40 short
#define LOCAL_INTERPOLATE_VBLK2                             (1072 + RMVB_LOCAL_INTER_BLK) //40 short
#define LOCAL_INTERPOLATE_VBLK3                             (1112 + RMVB_LOCAL_INTER_BLK) //40 short
#define LOCAL_INTERPOLATE_YBLK0_B                           (1152 + RMVB_LOCAL_INTER_BLK) //208 short
#define LOCAL_INTERPOLATE_YBLK1_B                           (1360 + RMVB_LOCAL_INTER_BLK) //208 short
#define LOCAL_INTERPOLATE_YBLK2_B                           (1568 + RMVB_LOCAL_INTER_BLK) //208 short
#define LOCAL_INTERPOLATE_YBLK3_B                           (1776 + RMVB_LOCAL_INTER_BLK) //208 short
#define LOCAL_INTERPOLATE_UBLK0_B                           (1984 + RMVB_LOCAL_INTER_BLK) //40 short
#define LOCAL_INTERPOLATE_UBLK1_B                           (2024 + RMVB_LOCAL_INTER_BLK) //40 short
#define LOCAL_INTERPOLATE_UBLK2_B                           (2064 + RMVB_LOCAL_INTER_BLK) //40 short
#define LOCAL_INTERPOLATE_UBLK3_B                           (2104 + RMVB_LOCAL_INTER_BLK) //40 short
#define LOCAL_INTERPOLATE_VBLK0_B                           (2144 + RMVB_LOCAL_INTER_BLK) //40 short
#define LOCAL_INTERPOLATE_VBLK1_B                           (2184 + RMVB_LOCAL_INTER_BLK) //40 short
#define LOCAL_INTERPOLATE_VBLK2_B                           (2224 + RMVB_LOCAL_INTER_BLK) //40 short
#define LOCAL_INTERPOLATE_VBLK3_B                           (2264 + RMVB_LOCAL_INTER_BLK) //40 short

//RMVB_LOCAL_BLK_X
#define RMVB_LOCAL_BLK_X                                    (2834 + CII_RMVBMbDec_X_BEGIN_ADDR)
#define LOCAL_BLOCK14_4                                     (0 + RMVB_LOCAL_BLK_X) //32 short

//RMVB_LOCAL_PRED_BLK_X
#define RMVB_LOCAL_PRED_BLK_X                               (2866 + CII_RMVBMbDec_X_BEGIN_ADDR)
#define RMVB_LOCAL_MB_TOP                                   (0 + RMVB_LOCAL_PRED_BLK_X) //1 short
#define RMVB_LOCAL_MB_LEFT                                  (1 + RMVB_LOCAL_PRED_BLK_X) //1 short
#define RMVB_LOCAL_LEFT_ARRAY                               (2 + RMVB_LOCAL_PRED_BLK_X) //8 short
#define FILTER_YLEFT                                        (10 + RMVB_LOCAL_PRED_BLK_X) //64 short
#define FILTER_YUP                                          (74 + RMVB_LOCAL_PRED_BLK_X) //64 short
#define FILTER_ULEFT                                        (138 + RMVB_LOCAL_PRED_BLK_X) //32 short
#define FILTER_UUP                                          (170 + RMVB_LOCAL_PRED_BLK_X) //32 short
#define FILTER_VLEFT                                        (202 + RMVB_LOCAL_PRED_BLK_X) //32 short
#define FILTER_VUP                                          (234 + RMVB_LOCAL_PRED_BLK_X) //32 short
#define YINTRA_UP_ARRAY                                     (266 + RMVB_LOCAL_PRED_BLK_X) //24 short
#define YINTRA_LEFT_ARRAY                                   (290 + RMVB_LOCAL_PRED_BLK_X) //24 short
#define UINTRA_UP_ARRAY                                     (314 + RMVB_LOCAL_PRED_BLK_X) //16 short
#define UINTRA_LEFT_ARRAY                                   (330 + RMVB_LOCAL_PRED_BLK_X) //16 short
#define VINTRA_UP_ARRAY                                     (346 + RMVB_LOCAL_PRED_BLK_X) //16 short
#define VINTRA_LEFT_ARRAY                                   (362 + RMVB_LOCAL_PRED_BLK_X) //16 short
#define Y_ROW_ARRAY                                         (378 + RMVB_LOCAL_PRED_BLK_X) //400 short
#define U_ROW_ARRAY                                         (778 + RMVB_LOCAL_PRED_BLK_X) //200 short
#define V_ROW_ARRAY                                         (978 + RMVB_LOCAL_PRED_BLK_X) //200 short
#define YINTRA_UPLEFT                                       (1178 + RMVB_LOCAL_PRED_BLK_X) //1 short
#define UINTRA_UPLEFT                                       (1179 + RMVB_LOCAL_PRED_BLK_X) //1 short
#define VINTRA_UPLEFT                                       (1180 + RMVB_LOCAL_PRED_BLK_X) //1 short
#define LOCAL_STUFF                                         (1181 + RMVB_LOCAL_PRED_BLK_X) //1 short

//RMVB_PARA_ADDR_STRUCT
#define RMVB_PARA_ADDR_STRUCT                               (4048 + CII_RMVBMbDec_X_BEGIN_ADDR)
#define VLD_COF_ADDR                                        (0 + RMVB_PARA_ADDR_STRUCT) //1 short
#define INTRA_MODE_ADDR                                     (1 + RMVB_PARA_ADDR_STRUCT) //1 short
#define UVINTRA_UP_ARRAY_ADDR                               (2 + RMVB_PARA_ADDR_STRUCT) //1 short
#define UVINTRA_LEFT_ARRAY_ADDR                             (3 + RMVB_PARA_ADDR_STRUCT) //1 short
#define UV_VLD_COF_ADDR                                     (4 + RMVB_PARA_ADDR_STRUCT) //1 short
#define UV_PRED_BLK_ADDR                                    (5 + RMVB_PARA_ADDR_STRUCT) //1 short

//RMVB_LOCAL_PARA_STRUCT
#define RMVB_LOCAL_PARA_STRUCT                              (4054 + CII_RMVBMbDec_X_BEGIN_ADDR)
#define PITCHX4                                             (0 + RMVB_LOCAL_PARA_STRUCT) //2 short
#define DOUBLE_FRM                                          (2 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define UV_MB_CBP                                           (3 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define LOCAL_EDGEBIT                                       (4 + RMVB_LOCAL_PARA_STRUCT) //2 short
#define LOCAL_LEFT_EDGEBIT                                  (6 + RMVB_LOCAL_PARA_STRUCT) //2 short
#define LOCAL_UP_EDGEBIT                                    (8 + RMVB_LOCAL_PARA_STRUCT) //2 short
#define LOCAL_LOW_EDGEBIT                                   (10 + RMVB_LOCAL_PARA_STRUCT) //2 short
#define LOCAL_FILTER_EDGE0_UP                               (12 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define LOCAL_FILTER_EDGE0                                  (13 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define LOCAL_FILTER_EDGE0_LEFT                             (14 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define FILTER_CHROMA                                       (15 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define LOCAL_FILTER_EDGE4_UP                               (16 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define LOCAL_FILTER_EDGE4_DOWN                             (17 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define FILTER_DITHER_PT                                    (18 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define LOCAL_NO_FILTER_EDGE4                               (19 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define FILTER_BLK_CLIP                                     (20 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define FILTER_BLK_CLIP_HVAL                                (21 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define FILTER_BLK_CLIP_VVAL                                (22 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define FILTER_BLK_CLIP_LEFT                                (23 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define FILTER_PITCH                                        (24 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define FILTER_STRONG                                       (25 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define FILTER_CR                                           (26 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define FILTER_CL                                           (27 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define FILTER_AR                                           (28 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define FILTER_AL                                           (29 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define INTRA_PRED_OFFSET4                                  (30 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define INTRA_PRED_OFFSET5                                  (31 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define INTRA_PRED_OFFSET6                                  (32 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define INTRA_PRED_OFFSET7                                  (33 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define INTRA_PRED_OFFSET8                                  (34 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define INTRA_PRED_OFFSET9                                  (35 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define INTRA_PRED_OFFSET10                                 (36 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define INTRA_PRED_OFFSET11                                 (37 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define INTRA_PRED_OFFSET12                                 (38 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define INTRA_PRED_OFFSET13                                 (39 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define INTRA_PRED_OFFSET14                                 (40 + RMVB_LOCAL_PARA_STRUCT) //1 short
#define INTRA_PRED_OFFSET15                                 (41 + RMVB_LOCAL_PARA_STRUCT) //1 short


//CII_RMVBMbDec
#define CII_RMVBMbDec_Y_BEGIN_ADDR                          (0 + RM_LOCAL_Y_BEGIN_ADDR)
//RMVB_LOCAL_BLK_Y
#define RMVB_LOCAL_BLK_Y                                    (0 + CII_RMVBMbDec_Y_BEGIN_ADDR)
#define LOCAL_BLOCK4_4                                      (0 + RMVB_LOCAL_BLK_Y) //16 short
#define LOCAL_BLOCKN4_4                                     (16 + RMVB_LOCAL_BLK_Y) //32 short

//RMVB_LOCAL_PRED_BLK_Y
#define RMVB_LOCAL_PRED_BLK_Y                               (48 + CII_RMVBMbDec_Y_BEGIN_ADDR)
#define RMVB_LOCAL_PRED_BLKY                                (0 + RMVB_LOCAL_PRED_BLK_Y) //256 short
#define RMVB_LOCAL_PRED_BLKUV                               (256 + RMVB_LOCAL_PRED_BLK_Y) //128 short
#define RMVB_LOCAL_PRED_BLKY_B                              (384 + RMVB_LOCAL_PRED_BLK_Y) //256 short
#define RMVB_LOCAL_PRED_BLKUV_B                             (640 + RMVB_LOCAL_PRED_BLK_Y) //128 short
#define RMVB_LOCAL_PRED_TBUFF                               (768 + RMVB_LOCAL_PRED_BLK_Y) //336 short
#define RMVB_LOCAL_PIXEL_TMP0                               (1104 + RMVB_LOCAL_PRED_BLK_Y) //3 short
#define RMVB_LOCAL_PIXEL_TMP1                               (1107 + RMVB_LOCAL_PRED_BLK_Y) //3 short
#define RMVB_LOCAL_PIXEL_UPLEFT                             (1110 + RMVB_LOCAL_PRED_BLK_Y) //2 short
#define RMVB_LOCAL_UP_ARRAY_POINT                           (1112 + RMVB_LOCAL_PRED_BLK_Y) //1 short
#define RMVB_LOCAL_LEFT_ARRAY_POINT                         (1113 + RMVB_LOCAL_PRED_BLK_Y) //1 short


/*******************
 **  H264_GLOBAL  **
 *******************/

//GLOBAL_VARS
//VPP_H264_DEC_INPUT_STRUCT
#define VPP_H264_DEC_INPUT_STRUCT                           (0 + H264_GLOBAL_X_BEGIN_ADDR)
#define H264_GX_BS_BUFF                                     (0 + VPP_H264_DEC_INPUT_STRUCT) //512 short
#define H264_GX_BS_BUFF_EXTADDR                             (512 + VPP_H264_DEC_INPUT_STRUCT) //2 short
#define H264_GX_BS_REMAIN_SIZE                              (514 + VPP_H264_DEC_INPUT_STRUCT) //2 short
#define H264_GX_BS_CACHE                                    (516 + VPP_H264_DEC_INPUT_STRUCT) //2 short
#define H264_GX_BS_CACHEBITS                                (518 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define H264_GX_BS_NEED_DMA                                 (519 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define H264_GX_BS_TOT_BITS                                 (520 + VPP_H264_DEC_INPUT_STRUCT) //2 short
#define H264_GX_BS_USED_BITS                                (522 + VPP_H264_DEC_INPUT_STRUCT) //2 short
#define H264_GX_BS_HALF_CONSUME                             (524 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define H264_INIT0                                          (525 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GlOBAL_H264_H_S_MB_SKIP_RUN_ADDR                    (526 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GLOBAL_H_SLICE_TYPE                                 (527 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GLOBAL_H264_SIMPLE_ADDR                             (528 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GLOBAL_H264_deblocking_filter_ADDR                  (529 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GlOBAL_H264_H_S_PICTURE_STRUCTURE_ADDR              (530 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GlOBAL_H264_H_S_mb_stride_ADDR                      (531 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GlOBAL_H264_H_MB_AFF_FRAMR_ADDR                     (532 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GlOBAL_H264_H_slice_num_ADDR                        (533 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GlOBAL_H264_H_pps_constrained_intra_pred_ADDR           (534 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GlOBAL_H264_H_pps_transform_8x8_mode_ADDR           (535 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GlOBAL_H264_H_S_qscale_ADDR                         (536 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GlOBAL_H264_H_list_count_ADDR                       (537 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GlOBAL_H264_H_S_width_ADDR                          (538 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GlOBAL_H264_H_S_height_ADDR                         (539 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GlOBAL_H264_H_S_mb_width_ADDR                       (540 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GlOBAL_H264_H_S_mb_height_ADDR                      (541 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define GlOBAL_H264_H_ref_count_ADDR                        (542 + VPP_H264_DEC_INPUT_STRUCT) //2 short
#define DMA_Linesize_SIMPLE_ADDR                            (544 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define DMA_Uvlinesize_SIMPLE_ADDR                          (545 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define MB_X_SIMPLE_ADDR                                    (546 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define MB_Y_SIMPLE_ADDR                                    (547 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define SLICE_ALPHA_C0_OFFSET                               (548 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define SLICE_BETA_OFFSET                                   (549 + VPP_H264_DEC_INPUT_STRUCT) //1 short
#define CHROMA_QP_INDEX_OFFSET                              (550 + VPP_H264_DEC_INPUT_STRUCT) //2 short
#define GLOBAL_H264_chroma_qp_ADDR                          (552 + VPP_H264_DEC_INPUT_STRUCT) //2 short

#define GlOBAL_H264_H_cbp_ADDR                              (554 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_H264_H_mb_xy_ADDR                            (555 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_H264_H_topleft_samples_available_ADDR            (556 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_H264_H_top_samples_available_ADDR            (557 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_H264_H_left_samples_available_ADDR           (558 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_topright_samples_available_ADDR              (559 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_H264_H_left_mb_xy0_ADDR                      (560 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_H264_H_left_mb_xy1_ADDR                      (561 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_H264_H_top_mb_xy_ADDR                        (562 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_H264_H_b_stride_ADDR                         (563 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_H264_H_b8_stride_ADDR                        (564 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_H264_H_sub_mb_type_ADDR                      (565 + H264_GLOBAL_X_BEGIN_ADDR) //4 short
#define MB_TYPE_ADDR                                        (569 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define MB_TYPE_LEFT_ADDR                                   (570 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define MB_TYPE_TOP_ADDR                                    (571 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define tempp_ADDR                                          (572 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define QP_ADDR                                             (573 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define QP_LEFT_ADDR                                        (574 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define QP_TOP_ADDR                                         (575 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define CUR_MB_SLICE_NUM                                    (576 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define LEFT_MB_SLICE_NUM                                   (577 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define TOP_MB_SLICE_NUM                                    (578 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_H264_H_MB_field_decoding_flag_ADDR           (579 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_H264_chroma_pred_mode_ADDR                   (580 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_H264_intra16x16_pred_mode_ADDR               (581 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_FIELD_PICTURE_ADDR                           (582 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_CONST_suffix_limit_ADDR_ADDR                 (583 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_CONST_scan8_ADDR_ADDR                        (584 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_non_zero_count_cache_ADDR_ADDR               (585 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define CHROMA_QP_LEFT                                      (586 + H264_GLOBAL_X_BEGIN_ADDR) //2 short
#define CHROMA_QP_TOP                                       (588 + H264_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_H264_MB_ADDR                                 (590 + H264_GLOBAL_X_BEGIN_ADDR) //384 short
#define GLOBAL_non_zero_count_cache_ADDR                    (974 + H264_GLOBAL_X_BEGIN_ADDR) //48 short
#define GLOBAL_intra4x4_pred_mode_cache_ADDR                (1022 + H264_GLOBAL_X_BEGIN_ADDR) //40 short
#define GLOBAL_intra4x4_pred_mode_ADDR                      (1062 + H264_GLOBAL_X_BEGIN_ADDR) //24 short
#define MV0_CACHE_ADDR                                      (1086 + H264_GLOBAL_X_BEGIN_ADDR) //80 short
#define REF0_CACHE_ADDR                                     (1166 + H264_GLOBAL_X_BEGIN_ADDR) //40 short
#define MV1_CACHE_ADDR                                      (1206 + H264_GLOBAL_X_BEGIN_ADDR) //80 short
#define REF1_CACHE_ADDR                                     (1286 + H264_GLOBAL_X_BEGIN_ADDR) //40 short
#define REF0_CACHE_ADDR_ADDR                                (1326 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define REF1_CACHE_ADDR_ADDR                                (1327 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define MV0_CACHE_ADDR_ADDR                                 (1328 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define MV1_CACHE_ADDR_ADDR                                 (1329 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define REF_X_EXP_ADDR                                      (1330 + H264_GLOBAL_X_BEGIN_ADDR) //2 short
#define REF_Y_EXP_ADDR                                      (1332 + H264_GLOBAL_X_BEGIN_ADDR) //2 short
#define MV_X_ADDR                                           (1334 + H264_GLOBAL_X_BEGIN_ADDR) //16 short
#define MV_Y_ADDR                                           (1350 + H264_GLOBAL_X_BEGIN_ADDR) //16 short
#define GLOBAL_current_picture_mb_type_base_ADDR            (1366 + H264_GLOBAL_X_BEGIN_ADDR) //706 short
#define GLOBAL_current_picture_mb_type_ADDR                 ((GLOBAL_current_picture_mb_type_base_ADDR+2*((((((480 + 15)/16) + 1) + 1)/2)*2)+1))
#define GLOBAL_H264_mb2b_xy_ADDR                            (2072 + H264_GLOBAL_X_BEGIN_ADDR) //672 short
#define GLOBAL_H264_direct_8x8_inference_flag               (2744 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GlOBAL_H264_H_direct_spatial_mv_pred_ADDR           (2745 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_H264_H_x264_built_direct_flag_ADDR           (2746 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_H264_tmp_for_alignment_ADDR                  (2747 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
//GLOBAL_H264_EXTRA_DATA_ADDR
#define GLOBAL_H264_EXTRA_DATA_ADDR                         (2748 + H264_GLOBAL_X_BEGIN_ADDR)
#define TEMP_dest_y_backup_ADDR                             (0 + GLOBAL_H264_EXTRA_DATA_ADDR) //84 short
#define TEMP_dest_y_ADDR                                    (84 + GLOBAL_H264_EXTRA_DATA_ADDR) //316 short
#define TEMP_dest_cb_backup_ADDR                            (400 + GLOBAL_H264_EXTRA_DATA_ADDR) //52 short
#define TEMP_dest_cb_ADDR                                   (452 + GLOBAL_H264_EXTRA_DATA_ADDR) //92 short
#define TEMP_dest_cr_backup_ADDR                            (544 + GLOBAL_H264_EXTRA_DATA_ADDR) //52 short
#define TEMP_dest_cr_ADDR                                   (596 + GLOBAL_H264_EXTRA_DATA_ADDR) //92 short
#define REF_TEMP_ADDR                                       (688 + GLOBAL_H264_EXTRA_DATA_ADDR) //442 short

#define H264_top_pixel_y                                    (3878 + H264_GLOBAL_X_BEGIN_ADDR) //1008 short
#define H264_top_pixel_cb                                   (4886 + H264_GLOBAL_X_BEGIN_ADDR) //512 short
#define H264_top_pixel_cr                                   (5398 + H264_GLOBAL_X_BEGIN_ADDR) //512 short
//LOCAL_H264_TEMP_DATA_ADDR
#define LOCAL_H264_TEMP_DATA_ADDR                           (5910 + H264_GLOBAL_X_BEGIN_ADDR)
#define TEMP_ARRAY_ADDR                                     (0 + LOCAL_H264_TEMP_DATA_ADDR) //10 short
#define TEMP_ARRAY1_ADDR                                    (10 + LOCAL_H264_TEMP_DATA_ADDR) //90 short
#define TEMP_topright_samples_available_ADDR                (100 + LOCAL_H264_TEMP_DATA_ADDR) //2 short
#define TEMP_topright_ADDR                                  (102 + LOCAL_H264_TEMP_DATA_ADDR) //4 short
#define TEMP_z0_ADDR                                        (106 + LOCAL_H264_TEMP_DATA_ADDR) //1 short
#define TEMP_z1_ADDR                                        (107 + LOCAL_H264_TEMP_DATA_ADDR) //1 short
#define TEMP_TEMP_ADDR                                      (108 + LOCAL_H264_TEMP_DATA_ADDR) //16 short
#define TEMP_TEMP1_ADDR                                     (124 + LOCAL_H264_TEMP_DATA_ADDR) //16 short
#define TEMP_ADDR_ADDR                                      (140 + LOCAL_H264_TEMP_DATA_ADDR) //1 short

#define GLOBAL_H264_6tap_coeffs_ADDR                        (6051 + H264_GLOBAL_X_BEGIN_ADDR) //6 short
#define DMA_CHAIN_REF_UV_STATUS_ADDR                        (6057 + H264_GLOBAL_X_BEGIN_ADDR) //1 short
#define DMA_CHAIN_REF_Y_EADDR_ADDR                          (6058 + H264_GLOBAL_X_BEGIN_ADDR) //32 short
#define DMA_CHAIN_REF_UV_EADDR_ADDR                         (6090 + H264_GLOBAL_X_BEGIN_ADDR) //64 short
#define GLOBAL_chroma_mc_coeffs_ADDR                        (6154 + H264_GLOBAL_X_BEGIN_ADDR) //4 short
//GLOBAL_H264_weight_pred_ADDR
#define GLOBAL_H264_weight_pred_ADDR                        (6158 + H264_GLOBAL_X_BEGIN_ADDR)
#define GLOBAL_H264_use_weight_ADDR                         (0 + GLOBAL_H264_weight_pred_ADDR) //1 short
#define GLOBAL_H264_use_weight_chroma_ADDR                  (1 + GLOBAL_H264_weight_pred_ADDR) //1 short
#define GLOBAL_H264_luma_log2_weight_denom_ADDR             (2 + GLOBAL_H264_weight_pred_ADDR) //1 short
#define GLOBAL_H264_chroma_log2_weight_denom_ADDR           (3 + GLOBAL_H264_weight_pred_ADDR) //1 short
#define GLOBAL_H264_luma_weight_ADDR                        (4 + GLOBAL_H264_weight_pred_ADDR) //32 short
#define GLOBAL_H264_luma_offset_ADDR                        (36 + GLOBAL_H264_weight_pred_ADDR) //32 short
#define GLOBAL_H264_chroma_weight_ADDR                      (68 + GLOBAL_H264_weight_pred_ADDR) //64 short
#define GLOBAL_H264_chroma_offset_ADDR                      (132 + GLOBAL_H264_weight_pred_ADDR) //64 short
#define GLOBAL_H264_implicit_weight_ADDR                    (196 + GLOBAL_H264_weight_pred_ADDR) //256 short

#define GLOBAL_H264_luma_weight0_ADDR                       (GLOBAL_H264_luma_weight_ADDR)
#define GLOBAL_H264_luma_weight1_ADDR                       ((GLOBAL_H264_luma_weight_ADDR+16))
#define GLOBAL_H264_luma_offset0_ADDR                       (GLOBAL_H264_luma_offset_ADDR)
#define GLOBAL_H264_luma_offset1_ADDR                       ((GLOBAL_H264_luma_offset_ADDR+16))
#define GLOBAL_H264_chroma_weight0_ADDR                     (GLOBAL_H264_chroma_weight_ADDR)
#define GLOBAL_H264_chroma_weight1_ADDR                     ((GLOBAL_H264_chroma_weight_ADDR+32))
#define GLOBAL_H264_chroma_offset0_ADDR                     (GLOBAL_H264_chroma_offset_ADDR)
#define GLOBAL_H264_chroma_offset1_ADDR                     ((GLOBAL_H264_chroma_offset_ADDR+32))
#define GLOBAL_H264_MB_METADATA_ADDR                        (6610 + H264_GLOBAL_X_BEGIN_ADDR) //600 short
#define GLOBAL_h_map_col_to_list00_ADDR                     (7210 + H264_GLOBAL_X_BEGIN_ADDR) //16 short
#define GLOBAL_h_map_col_to_list01_ADDR                     (7226 + H264_GLOBAL_X_BEGIN_ADDR) //16 short
#define GLOBAL_h_dist_scale_factor_ADDR                     (7242 + H264_GLOBAL_X_BEGIN_ADDR) //16 short
#define TOP_LINE_BACKUP_BUFFER_ADDR                         (7258 + H264_GLOBAL_X_BEGIN_ADDR) //480 short
#define Temp_MC_List1_Buf_ADDR                              (7738 + H264_GLOBAL_X_BEGIN_ADDR) //512 short
#define GLOBAL_REF_IDX_LIST0_TOPLEFT_BUF_ADDR               (8250 + H264_GLOBAL_X_BEGIN_ADDR) //32 short
#define GLOBAL_REF_IDX_LIST1_TOPLEFT_BUF_ADDR               (8282 + H264_GLOBAL_X_BEGIN_ADDR) //32 short

//GLOBAL_VARS
#define H264_RESERVE_FOR_ERR                                (0 + H264_GLOBAL_Y_BEGIN_ADDR) //16 short
//VPP_H264_EXTERN_ADDR_STRUCT
#define VPP_H264_EXTERN_ADDR_STRUCT                         (16 + H264_GLOBAL_Y_BEGIN_ADDR)
#define GlOBAL_H264_total_zeros_vlc_EXT_ADDR                (0 + VPP_H264_EXTERN_ADDR_STRUCT) //30 short
#define GlOBAL_H264_chroma_dc_total_zeros_vlc_EXT_ADDR          (30 + VPP_H264_EXTERN_ADDR_STRUCT) //6 short
#define GlOBAL_H264_current_picture_data_EXT_ADDR           (36 + VPP_H264_EXTERN_ADDR_STRUCT) //6 short
#define GLOBAL_H264_left_border_EXT_ADDR                    (42 + VPP_H264_EXTERN_ADDR_STRUCT) //2 short
#define GLOBAL_top_borders_EXT_ADDR                         (44 + VPP_H264_EXTERN_ADDR_STRUCT) //2 short
#define GLOBAL_H264_non_zero_count_EXT_ADDR                 (46 + VPP_H264_EXTERN_ADDR_STRUCT) //2 short
#define GLOBAL_H264_intra4x4_pred_mode_EXT_ADDR             (48 + VPP_H264_EXTERN_ADDR_STRUCT) //2 short
#define GLOBAL_H264_current_picture_motion_val_EXT_ADDR         (50 + VPP_H264_EXTERN_ADDR_STRUCT) //4 short
#define GLOBAL_H264_current_picture_ref_index_EXT_ADDR          (54 + VPP_H264_EXTERN_ADDR_STRUCT) //4 short
#define GLOBAL_H264_direct_col_ref_index_EXT_ADDR           (58 + VPP_H264_EXTERN_ADDR_STRUCT) //4 short
#define GLOBAL_H264_direct_col_motion_val_EXT_ADDR          (62 + VPP_H264_EXTERN_ADDR_STRUCT) //4 short
#define GLOBAL_H264_direct_col_mb_type_EXT_ADDR             (66 + VPP_H264_EXTERN_ADDR_STRUCT) //2 short
#define GLOBAL_H264_current_picture_mb_type_EXT_ADDR            (68 + VPP_H264_EXTERN_ADDR_STRUCT) //2 short
#define GLOBAL_H264_ref_list0_EXT_ADDR                      (70 + VPP_H264_EXTERN_ADDR_STRUCT) //96 short
#define GLOBAL_H264_ref_list1_EXT_ADDR                      (166 + VPP_H264_EXTERN_ADDR_STRUCT) //96 short

#define IMG_Y_ADDR_ADDR                                     (278 + H264_GLOBAL_Y_BEGIN_ADDR) //2 short
#define IMG_CB_ADDR_ADDR                                    (280 + H264_GLOBAL_Y_BEGIN_ADDR) //2 short
#define IMG_CR_ADDR_ADDR                                    (282 + H264_GLOBAL_Y_BEGIN_ADDR) //2 short
#define H264_topright_borders_EXT_ADDR                      (284 + H264_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_H264_top_borders_EXT_ADDR                    (286 + H264_GLOBAL_Y_BEGIN_ADDR) //2 short
#define REF_EXT_ADDR_Y                                      (288 + H264_GLOBAL_Y_BEGIN_ADDR) //32 short
#define REF_EXT_ADDR_CB                                     (320 + H264_GLOBAL_Y_BEGIN_ADDR) //32 short
#define REF_EXT_ADDR_CR                                     (352 + H264_GLOBAL_Y_BEGIN_ADDR) //32 short
#define GLOBAL_non_zero_count_ADDR                          (384 + H264_GLOBAL_Y_BEGIN_ADDR) //48 short
#define GlOBAL_H264_s_current_picture_qscale_ADDR           (432 + H264_GLOBAL_Y_BEGIN_ADDR) //640 short
#define GLOBAL_H264_H_slice_table_base_ADDR                 (1072 + H264_GLOBAL_Y_BEGIN_ADDR) //704 short
#define GLOBAL_H264_H_slice_table_ADDR                      ((GLOBAL_H264_H_slice_table_base_ADDR + 2*((((((480 + 15)/16) + 1) + 1)/2)*2) + 1))
//GLOBAL_H264_CONSTY_STRUCT
#define GLOBAL_H264_CONSTY_STRUCT                           (1776 + H264_GLOBAL_Y_BEGIN_ADDR)
#define GLOBAL_CONST_128_ADDR                               (0 + GLOBAL_H264_CONSTY_STRUCT) //2 short
#define CONST_x_offset_ADDR                                 (2 + GLOBAL_H264_CONSTY_STRUCT) //4 short
#define GLOBAL_CONST_scan8_ADDR                             (6 + GLOBAL_H264_CONSTY_STRUCT) //24 short
#define GLOBAL_H264_CONST_blkOffset_ADDR                    (30 + GLOBAL_H264_CONSTY_STRUCT) //96 short
#define CONST_b_mb_type_info_ADDR                           (126 + GLOBAL_H264_CONSTY_STRUCT) //46 short
#define CONST_p_mb_type_info_ADDR                           (172 + GLOBAL_H264_CONSTY_STRUCT) //10 short
#define CONST_i_mb_type_info_ADDR                           (182 + GLOBAL_H264_CONSTY_STRUCT) //26 short
#define CONST_p_sub_mb_type_info_ADDR                       (208 + GLOBAL_H264_CONSTY_STRUCT) //8 short
#define CONST_b_sub_mb_type_info_ADDR                       (216 + GLOBAL_H264_CONSTY_STRUCT) //26 short
#define CONST_h264_non_zero_count_cache_ref_ADDR            (242 + GLOBAL_H264_CONSTY_STRUCT) //16 short
#define CONST_h264_non_zero_count_ref_ADDR                  (258 + GLOBAL_H264_CONSTY_STRUCT) //16 short
#define CONST_h264_write_back_non_zero_count_cache_ref_ADDR         (274 + GLOBAL_H264_CONSTY_STRUCT) //14 short
#define CONST_h264_intra4x4_pred_mode_top_ADDR              (288 + GLOBAL_H264_CONSTY_STRUCT) //12 short
#define CONST_h264_intra4x4_pred_mode_left_ADDR             (300 + GLOBAL_H264_CONSTY_STRUCT) //12 short
#define CONST_golomb_to_intra4x4_cbp_ADDR                   (312 + GLOBAL_H264_CONSTY_STRUCT) //48 short
#define CONST_golomb_to_inter_cbp_ADDR                      (360 + GLOBAL_H264_CONSTY_STRUCT) //48 short
#define CONST_luma_dc_zigzag_scan_ADDR                      (408 + GLOBAL_H264_CONSTY_STRUCT) //16 short
#define CONST_luma_dc_field_scan_ADDR                       (424 + GLOBAL_H264_CONSTY_STRUCT) //16 short
#define CONST_zigzag_scan_ADDR                              (440 + GLOBAL_H264_CONSTY_STRUCT) //16 short
#define CONST_chroma_dc_scan_ADDR                           (456 + GLOBAL_H264_CONSTY_STRUCT) //4 short
#define CONST_suffix_limit_ADDR                             (460 + GLOBAL_H264_CONSTY_STRUCT) //8 short
#define CONST_ff_ue_golomb_vlc_code_ADDR                    (468 + GLOBAL_H264_CONSTY_STRUCT) //64 short
#define CONST_ff_se_golomb_vlc_code_ADDR                    (532 + GLOBAL_H264_CONSTY_STRUCT) //64 short
#define CONST_pred_mode_top_ADDR                            (596 + GLOBAL_H264_CONSTY_STRUCT) //8 short
#define CONST_pred_mode_left_ADDR                           (604 + GLOBAL_H264_CONSTY_STRUCT) //8 short
#define CONST_H264_0x04000000_ADDR                          (612 + GLOBAL_H264_CONSTY_STRUCT) //2 short
#define CONST_H264_255_ADDR                                 (614 + GLOBAL_H264_CONSTY_STRUCT) //1 short
#define CONST_H264_191_ADDR                                 (615 + GLOBAL_H264_CONSTY_STRUCT) //1 short
#define CONST_H264_127_ADDR                                 (616 + GLOBAL_H264_CONSTY_STRUCT) //1 short
#define CONST_H264_111_ADDR                                 (617 + GLOBAL_H264_CONSTY_STRUCT) //1 short
#define CONST_H264_79_ADDR                                  (618 + GLOBAL_H264_CONSTY_STRUCT) //1 short
#define CONST_H264_63_ADDR                                  (619 + GLOBAL_H264_CONSTY_STRUCT) //1 short
#define CONST_h264_left_block0_ADDR                         (620 + GLOBAL_H264_CONSTY_STRUCT) //8 short
#define CONST_h264_chroma_qp_ADDR                           (628 + GLOBAL_H264_CONSTY_STRUCT) //52 short
#define CONST_H264_dequant4_coeff_ADDR                      (680 + GLOBAL_H264_CONSTY_STRUCT) //832 short
#define CONST_H264_chroma_dc_coeff_token_vlc_ADDR           (1512 + GLOBAL_H264_CONSTY_STRUCT) //264 short
#define CONST_H264_coeff_token_vlc0_ADDR                    (1776 + GLOBAL_H264_CONSTY_STRUCT) //528 short
#define CONST_H264_coeff_token_vlc1_ADDR                    (2304 + GLOBAL_H264_CONSTY_STRUCT) //340 short
#define CONST_H264_coeff_token_vlc2_ADDR                    (2644 + GLOBAL_H264_CONSTY_STRUCT) //288 short
#define CONST_H264_coeff_token_vlc3_ADDR                    (2932 + GLOBAL_H264_CONSTY_STRUCT) //264 short
#define CONST_H264_chroma_dc_total_zeros_vlc_ADDR           (3196 + GLOBAL_H264_CONSTY_STRUCT) //24 short
#define CONST_H264_run_vlc_ADDR                             (3220 + GLOBAL_H264_CONSTY_STRUCT) //48 short
#define CONST_H264_run7_vlc_ADDR                            (3268 + GLOBAL_H264_CONSTY_STRUCT) //96 short
#define CONST_H264_total_zeros_vlc00_ADDR                   (3364 + GLOBAL_H264_CONSTY_STRUCT) //28 short
#define CONST_H264_total_zeros_vlc01_ADDR                   (3392 + GLOBAL_H264_CONSTY_STRUCT) //20 short
#define CONST_H264_total_zeros_vlc02_ADDR                   (3412 + GLOBAL_H264_CONSTY_STRUCT) //20 short
#define CONST_H264_total_zeros_vlc03_ADDR                   (3432 + GLOBAL_H264_CONSTY_STRUCT) //16 short
#define CONST_H264_total_zeros_vlc04_ADDR                   (3448 + GLOBAL_H264_CONSTY_STRUCT) //16 short
#define CONST_H264_total_zeros_vlc05_ADDR                   (3464 + GLOBAL_H264_CONSTY_STRUCT) //16 short
#define CONST_H264_total_zeros_vlc06_ADDR                   (3480 + GLOBAL_H264_CONSTY_STRUCT) //16 short
#define CONST_H264_total_zeros_vlc07_ADDR                   (3496 + GLOBAL_H264_CONSTY_STRUCT) //16 short
#define CONST_H264_total_zeros_vlc08_ADDR                   (3512 + GLOBAL_H264_CONSTY_STRUCT) //16 short
#define CONST_H264_total_zeros_vlc09_ADDR                   (3528 + GLOBAL_H264_CONSTY_STRUCT) //12 short
#define CONST_H264_total_zeros_vlc10_ADDR                   (3540 + GLOBAL_H264_CONSTY_STRUCT) //10 short
#define CONST_H264_total_zeros_vlc11_ADDR                   (3550 + GLOBAL_H264_CONSTY_STRUCT) //10 short
#define CONST_H264_total_zeros_vlc12_ADDR                   (3560 + GLOBAL_H264_CONSTY_STRUCT) //8 short
#define CONST_H264_total_zeros_vlc13_ADDR                   (3568 + GLOBAL_H264_CONSTY_STRUCT) //8 short
#define CONST_H264_total_zeros_vlc14_ADDR                   (3576 + GLOBAL_H264_CONSTY_STRUCT) //8 short
#define CONST_total_zeros_vlc_location_ADDR                 (3584 + GLOBAL_H264_CONSTY_STRUCT) //16 short
#define CONST_H264_alpha_table_ADDR                         (3600 + GLOBAL_H264_CONSTY_STRUCT) //34 short
#define CONST_H264_beta_table_ADDR                          (3634 + GLOBAL_H264_CONSTY_STRUCT) //34 short
#define REF2FRM                                             (3668 + GLOBAL_H264_CONSTY_STRUCT) //34 short
#define TC0_TABLE                                           (3702 + GLOBAL_H264_CONSTY_STRUCT) //312 short
#define CONST_H264_1_ADDR                                   (CONST_h264_non_zero_count_cache_ref_ADDR)
#define CONST_H264_2_ADDR                                   ((CONST_h264_non_zero_count_cache_ref_ADDR+1))
#define CONST_H264_4_ADDR                                   ((CONST_h264_non_zero_count_cache_ref_ADDR+2))
#define CONST_H264_5_ADDR                                   ((CONST_h264_non_zero_count_cache_ref_ADDR+3))
#define CONST_H264_6_ADDR                                   ((CONST_h264_non_zero_count_cache_ref_ADDR+4))
#define CONST_H264_7_ADDR                                   ((CONST_h264_non_zero_count_cache_ref_ADDR+5))
#define CONST_H264_25_ADDR                                  ((CONST_h264_non_zero_count_cache_ref_ADDR+6))
#define CONST_H264_26_ADDR                                  ((CONST_h264_non_zero_count_cache_ref_ADDR+7))
#define CONST_H264_11_ADDR                                  ((CONST_h264_non_zero_count_cache_ref_ADDR+8))
#define CONST_H264_19_ADDR                                  ((CONST_h264_non_zero_count_cache_ref_ADDR+9))
#define CONST_H264_8_ADDR                                   ((CONST_h264_non_zero_count_ref_ADDR+1))
#define CONST_H264_9_ADDR                                   ((CONST_h264_non_zero_count_ref_ADDR))
#define CONST_H264_16_ADDR                                  ((CONST_luma_dc_field_scan_ADDR+2))
#define CONST_H264_32_ADDR                                  ((CONST_luma_dc_field_scan_ADDR+1))
#define CONST_H264_95_ADDR                                  ((CONST_suffix_limit_ADDR+5))
#define CONST_H264_31_ADDR                                  ((CONST_golomb_to_intra4x4_cbp_ADDR+1))
#define CONST_H264_15_ADDR                                  ((CONST_golomb_to_intra4x4_cbp_ADDR+2))
#define CONST_H264_0_ADDR                                   ((CONST_ff_se_golomb_vlc_code_ADDR))
#define CONST_H264_3_ADDR                                   ((CONST_h264_left_block0_ADDR+3))
#define CONST_H264_64_ADDR                                  ((CONST_luma_dc_zigzag_scan_ADDR+5))
#define CONST_H264_14_ADDR                                  ((CONST_zigzag_scan_ADDR+14))
#define CONST_H264_24_ADDR                                  ((CONST_golomb_to_intra4x4_cbp_ADDR+36))
#define CONST_H264_50_ADDR                                  ((CONST_H264_alpha_table_ADDR+20))
#define CONST_H264_10_ADDR                                  ((CONST_h264_intra4x4_pred_mode_left_ADDR+2))
#define CONST_H264_0xFFFF_ADDR                              ((CONST_i_mb_type_info_ADDR+0))

//GLOBAL_H264_EXTRA_DATA_Y_ADDR
#define GLOBAL_H264_EXTRA_DATA_Y_ADDR                       (5790 + H264_GLOBAL_Y_BEGIN_ADDR)
#define GLOBAL_H264_left_border_ADDR                        (0 + GLOBAL_H264_EXTRA_DATA_Y_ADDR) //72 short
#define GLOBAL_H264_top_borders_ADDR                        (72 + GLOBAL_H264_EXTRA_DATA_Y_ADDR) //36 short

#define DMA_CMD_Y_OUT_LOCAL_ADDR                            (5898 + H264_GLOBAL_Y_BEGIN_ADDR) //400 short
#define DMA_CMD_Cb_OUT_LOCAL_ADDR                           (6298 + H264_GLOBAL_Y_BEGIN_ADDR) //144 short
#define DMA_CMD_Y_OUT_EADDR_ADDR                            (6442 + H264_GLOBAL_Y_BEGIN_ADDR) //2 short
#define DMA_CMD_UV_OUT_EADDR_ADDR                           (6444 + H264_GLOBAL_Y_BEGIN_ADDR) //2 short
#define REF_BLOCK_ADDR                                      (6446 + H264_GLOBAL_Y_BEGIN_ADDR) //504 short
#define REF_BLOCK_UV_ADDR                                   (6950 + H264_GLOBAL_Y_BEGIN_ADDR) //108 short
#define SEC_REF_BLOCK_ADDR                                  (7058 + H264_GLOBAL_Y_BEGIN_ADDR) //504 short
#define SEC_REF_BLOCK_UV_ADDR                               (7562 + H264_GLOBAL_Y_BEGIN_ADDR) //108 short
#define MC_TEMP_OUTPUT_ADDR                                 (7670 + H264_GLOBAL_Y_BEGIN_ADDR) //352 short
#define DMA_H264_STATUS_ADDR                                (8022 + H264_GLOBAL_Y_BEGIN_ADDR) //1 short
#define REF_BLOCK_ADDR_ADDR                                 (8023 + H264_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DMA_REF_BLOCK_ADDR_ADDR                             (8024 + H264_GLOBAL_Y_BEGIN_ADDR) //1 short
#define REF_BLOCK_UV_ADDR_ADDR                              (8025 + H264_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DMA_REF_BLOCK_UV_ADDR_ADDR                          (8026 + H264_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DMA_H264_DATA_READY_ADDR                            (8027 + H264_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DMA_CMD_UV_OUT_LOCAL_ADDR_ADDR                      (8028 + H264_GLOBAL_Y_BEGIN_ADDR) //1 short
#define for_align                                           (8029 + H264_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_REF_IDX_LIST0_BUFFER_ADDR                    (8030 + H264_GLOBAL_Y_BEGIN_ADDR) //128 short
#define GLOBAL_REF_IDX_LIST1_BUFFER_ADDR                    (8158 + H264_GLOBAL_Y_BEGIN_ADDR) //128 short

/******************
 **  H264_LOCAL  **
 ******************/

//CS_fill_caches
#define CS_fill_caches_Y_BEGIN_ADDR                         (0 + H264_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_fill_caches_left_type0_ADDR                   (0 + CS_fill_caches_Y_BEGIN_ADDR) //1 short
#define LOCAL_fill_caches_left_type1_ADDR                   (1 + CS_fill_caches_Y_BEGIN_ADDR) //1 short
#define LOCAL_fill_caches_topleft_type_ADDR                 (2 + CS_fill_caches_Y_BEGIN_ADDR) //1 short
#define LOCAL_fill_caches_top_type_ADDR                     (3 + CS_fill_caches_Y_BEGIN_ADDR) //1 short
#define LOCAL_fill_caches_topright_type_ADDR                (4 + CS_fill_caches_Y_BEGIN_ADDR) //1 short
#define LOCAL_fill_caches_for_deblock_ADDR                  (5 + CS_fill_caches_Y_BEGIN_ADDR) //1 short
#define LOCAL_fill_caches_topleft_xy_ADDR                   (6 + CS_fill_caches_Y_BEGIN_ADDR) //1 short
#define LOCAL_fill_caches_topright_xy_ADDR                  (7 + CS_fill_caches_Y_BEGIN_ADDR) //1 short

//CS_pred_direct_motion
#define CS_pred_direct_motion_X_BEGIN_ADDR                  (0 + H264_LOCAL_X_BEGIN_ADDR)
#define LOCAL_l1mv0_ADDR                                    (0 + CS_pred_direct_motion_X_BEGIN_ADDR) //32 short
#define LOCAL_l1mv1_ADDR                                    (32 + CS_pred_direct_motion_X_BEGIN_ADDR) //32 short
#define LOCAL_l1ref0_ADDR                                   (64 + CS_pred_direct_motion_X_BEGIN_ADDR) //8 short
#define LOCAL_l1ref1_ADDR                                   (72 + CS_pred_direct_motion_X_BEGIN_ADDR) //8 short
#define LOCAL_direct_tmp_ref_ADDR                           (80 + CS_pred_direct_motion_X_BEGIN_ADDR) //2 short
#define LOCAL_l1ref0_ADDR_ADDR                              (82 + CS_pred_direct_motion_X_BEGIN_ADDR) //1 short
#define LOCAL_l1ref1_ADDR_ADDR                              (83 + CS_pred_direct_motion_X_BEGIN_ADDR) //1 short

//CS_pred_motion
#define CS_pred_motion_Y_BEGIN_ADDR                         (14 + H264_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_pred_motion_ref_ADDR                          (0 + CS_pred_motion_Y_BEGIN_ADDR) //1 short
#define LOCAL_pred_motion_top_ref_ADDR                      (1 + CS_pred_motion_Y_BEGIN_ADDR) //1 short
#define LOCAL_pred_motion_left_ref_ADDR                     (2 + CS_pred_motion_Y_BEGIN_ADDR) //1 short
#define LOCAL_pred_motion_A_ADDR                            (3 + CS_pred_motion_Y_BEGIN_ADDR) //1 short
#define LOCAL_pred_motion_B_ADDR                            (4 + CS_pred_motion_Y_BEGIN_ADDR) //1 short

//CS_decode_mb_cavlc_partition_count
#define CS_decode_mb_cavlc_partition_count_Y_BEGIN_ADDR         (0 + H264_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_sub_partition_count_ADDR                      (0 + CS_decode_mb_cavlc_partition_count_Y_BEGIN_ADDR) //4 short
#define LOCAL_mb_cavlc_partition_count_ref_ADDR             (4 + CS_decode_mb_cavlc_partition_count_Y_BEGIN_ADDR) //8 short
#define LOCAL_IS_DIR_ADDR                                   (12 + CS_decode_mb_cavlc_partition_count_Y_BEGIN_ADDR) //1 short

//CS_decode_residual
#define CS_decode_residual_X_BEGIN_ADDR                     (0 + H264_LOCAL_X_BEGIN_ADDR)
#define LOCAL_residual_level_ADDR                           (0 + CS_decode_residual_X_BEGIN_ADDR) //16 short
#define LOCAL_residual_block_ADDR                           (16 + CS_decode_residual_X_BEGIN_ADDR) //1 short
#define LOCAL_residual_n_ADDR                               (17 + CS_decode_residual_X_BEGIN_ADDR) //1 short
#define LOCAL_residual_max_coeff_ADDR                       (18 + CS_decode_residual_X_BEGIN_ADDR) //1 short
#define LOCAL_scantable_ADDR                                (19 + CS_decode_residual_X_BEGIN_ADDR) //1 short
#define LOCAL_residual_n_GT_24_flag_ADDR                    (20 + CS_decode_residual_X_BEGIN_ADDR) //1 short

//CS_MC_prepare
#define CS_MC_prepare_X_BEGIN_ADDR                          (0 + H264_LOCAL_X_BEGIN_ADDR)
#define LOCAL_mc_check_ref_exp_ADDR                         (0 + CS_MC_prepare_X_BEGIN_ADDR) //1 short

//CS_h264_mc
#define CS_h264_mc_X_BEGIN_ADDR                             (0 + H264_LOCAL_X_BEGIN_ADDR)
#define LOCAL_block_mc_is_dir_list0_ADDR                    (0 + CS_h264_mc_X_BEGIN_ADDR) //1 short
#define LOCAL_block_mc_is_dir_list1_ADDR                    (1 + CS_h264_mc_X_BEGIN_ADDR) //1 short

//CS_h264_mc
#define CS_h264_mc_Y_BEGIN_ADDR                             (0 + H264_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_weight_coeffs_ADDR                            (0 + CS_h264_mc_Y_BEGIN_ADDR) //4 short

/********************
 **  MPEG4_GLOBAL  **
 ********************/

//GLOBAL_VARS
#define GLOBAL_MPEG4_X_ADDR                                 (0 + MPEG4_GLOBAL_X_BEGIN_ADDR) //3244 short
//GLOBAL_MPEG4_X_STRUCT
#define GLOBAL_MPEG4_X_STRUCT                               (3244 + MPEG4_GLOBAL_X_BEGIN_ADDR)
#define GLOBAL_MPEG4_bs_tail_ADDR                           (0 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_quant_ADDR                                    (1 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_x_pos_ADDR                                    (2 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_y_pos_ADDR                                    (3 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_MPEG4_mb_width_ADDR                          (4 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_MPEG4_mb_height_ADDR                         (5 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_acpred_bound_flag_ADDR                       (6 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_acpred_flag_ADDR                             (7 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_intra_dc_threshold_ADDR                      (8 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_cbp_ADDR                                     (10 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_bound_ADDR                                   (12 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_fcode_ADDR                                    (14 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_rounding_ADDR                                 (15 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_edged_width_ADDR                              (16 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_edged_hight_ADDR                              (17 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_width_ADDR                                    (18 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_hight_ADDR                                    (19 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_stride_u_ADDR                                 (20 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_stride_v_ADDR                                 (21 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_acpred_dir_ADDR                              (22 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_start_coeff_ADDR                              (23 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_hw_direction_ADDR                             (24 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_dc_pred_ADDR                                  (25 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_iDcScaler_ADDR                                (26 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DC_ADDR                                       (27 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_predQp_ADDR                                   (28 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_MPEG4_bound_ADDR                             (30 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_pCoeffBuffRow_ADDR_ADDR                       (32 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_pCoeffBuffCol_ADDR_ADDR                       (33 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_pMVBuffPred1_ADDR                             (34 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_pMVBuffPred2_ADDR                             (35 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_0X80000000_ADDR                               (36 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_uv_dx_ADDR                                    (38 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_uv_dy_ADDR                                    (39 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_y_add_temp_ADDR                               (40 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_uv_add_temp_ADDR                              (42 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_dx_dy_ADDR                                    (44 + GLOBAL_MPEG4_X_STRUCT) //10 short
#define MPEG4_coordx_tmp2_ADDR                              (54 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_coordx_tmp_ADDR                               (55 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_coordy_tmp2_ADDR                              (56 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_coordy_tmp_ADDR                               (57 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_88_stride_ADDR                                (58 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_MPEG4_DCT3D_ADDR                             (60 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_bs_pos_ADDR                                   (62 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_bs_buf_EXTADDR                                (64 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_bs_tail_EXTADDR                               (66 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_DEC_BLOCK_ADDR                               (68 + GLOBAL_MPEG4_X_STRUCT) //384 short
#define GLOBAL_DEC_DATA_ADDR                                (452 + GLOBAL_MPEG4_X_STRUCT) //384 short
#define MPEG4_IDCT_BUFFER_X_ADDR                            (836 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_IDCT_BUFFER_X1_ADDR                           (837 + GLOBAL_MPEG4_X_STRUCT) //7 short
#define GLOBAL_mv_buffer_base_ADDR                          (844 + GLOBAL_MPEG4_X_STRUCT) //752 short
#define GLOBAL_mvs_ADDR                                     (1596 + GLOBAL_MPEG4_X_STRUCT) //16 short
#define GLOBAL_MPEG4_ac_pred_base_ADDR                      (1612 + GLOBAL_MPEG4_X_STRUCT) //1504 short
#define GLOBAL_MPEG4_ac_pred_above_y_ADDR                   (3116 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_MPEG4_ac_pred_left_y_ADDR                    (3117 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_MPEG4_ac_pred_above_u_ADDR                   (3118 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_MPEG4_ac_pred_left_u_ADDR                    (3119 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_MPEG4_ac_pred_above_v_ADDR                   (3120 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_MPEG4_ac_pred_left_v_ADDR                    (3121 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_TEMP_DMA_DATA_ADDR                            (3122 + GLOBAL_MPEG4_X_STRUCT) //400 short
#define MPEG4_MB_DEC_ID_ADDR                                (3522 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_y_row_start_EXTADDR                           (3524 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_u_row_start_EXTADDR                           (3526 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_v_row_start_EXTADDR                           (3528 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_quant_bits_ADDR                               (3530 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_time_inc_bits_ADDR                            (3531 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_MPEG4_CONSTX_ADDR                            (3532 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_refn0y_ADDR                                  (3534 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_refn0u_ADDR                                  (3536 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_refn0v_ADDR                                  (3538 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_mv_buffer_base_EXTADDR                       (3540 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_DCT0_ADDR                                    (3542 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_DCT1_ADDR                                    (3544 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_IDCT_BUFFER_Y_ADDR                            (3546 + GLOBAL_MPEG4_X_STRUCT) //8 short
#define MPEG4_bs_consume_ADDR                               (3554 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_bs_needDMA_ADDR                               (3556 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_QUIT_VOC_ADDR                                 (3557 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_pQpBuffcurrent_ADDR                          (3558 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_pQpBuffLeft_ADDR                             (3559 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_pQpBuffAbove_ADDR                            (3560 + GLOBAL_MPEG4_X_STRUCT) //45 short
#define GLOBAL_MPEG4_bs_start_align_ADDR                    (3605 + GLOBAL_MPEG4_X_STRUCT) //319 short
#define GLOBAL_MPEG4_bs_start_ADDR              (3924 + GLOBAL_MPEG4_X_STRUCT) //1024 short
#define GLOBAL_MPEG4_IDCT_BUFFER_X_ADDR         (4948 + GLOBAL_MPEG4_X_STRUCT) //8 short
#define GLOBAL_MPEG4_DMA_STATUS_ADDR            (4956 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_MPEG4_DEC_lowres_ADDR            (4957 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_MPEG4_DEC_lowres_BACKUP0_ADDR    (4958 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_MPEG4_DEC_lowres_BACKUP1_ADDR    (4960 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_MPEG4_DEC_lowres_BACKUP2_ADDR    (4962 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_MPEG4_DEC_lowres_short_BACKUP0_ADDR       (4964 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_MPEG4_DEC_lowres_short_BACKUP1_ADDR        (4965 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define GLOBAL_bmvs_ADDR                         (4966 + GLOBAL_MPEG4_X_STRUCT) //8 short
#define MPEG4_b_dx_dy_ADDR                       (4974 + GLOBAL_MPEG4_X_STRUCT) //10 short
#define GLOBAL_refn_f_y_ADDR                     (4984 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_refn_f_u_ADDR                     (4986 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_refn_f_v_ADDR                     (4988 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_refn_b_y_ADDR                     (4990 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_refn_b_u_ADDR                     (4992 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define GLOBAL_refn_b_v_ADDR                     (4994 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_last_mb_start_EXTADDR              (4996 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_DEC_quarterpel_ADDR                (4998 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_ASP_resync_len_ADDR            (4999 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_quarter_h_flag                 (5000 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_quarter_v_flag                 (5001 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_src_ADDR                       (5002 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_dst_ADDR                       (5003 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_src_stride_ADDR                (5004 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_dst_stride_ADDR                (5005 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_last_mb_mode_EADDR             (5006 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_DEC_last_mb_mvs_EADDR              (5008 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_DEC_last_mb_mode                   (5010 + GLOBAL_MPEG4_X_STRUCT) //46 short
#define MPEG4_DEC_last_mb_mvs                    (5056 + GLOBAL_MPEG4_X_STRUCT) //360 short
#define MPEG4_DEC_fcode_forward                  (5416 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_fcode_backward                 (5417 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_F_MVBuffPred                   (5418 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_DEC_B_MVBuffPred                   (5420 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_DEC_time_bp                        (5422 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_time_pp                        (5423 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_GMC_num_wp                     (5424 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_GMC_accuracy                   (5425 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_GMC_sW                         (5426 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_GMC_sH                         (5427 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_GMC_dU                         (5428 + GLOBAL_MPEG4_X_STRUCT) //4 short
#define MPEG4_DEC_GMC_dV                         (5432 + GLOBAL_MPEG4_X_STRUCT) //4 short
#define MPEG4_DEC_GMC_Uo                         (5436 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_DEC_GMC_Vo                         (5438 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_DEC_GMC_Uco                        (5440 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_DEC_GMC_Vco                        (5442 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_DEC_GMC_warp                       (5444 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_GMC_mcsel                      (5445 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_GMC_U                          (5446 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_DEC_GMC_V                          (5448 + GLOBAL_MPEG4_X_STRUCT) //2 short
#define MPEG4_DEC_GMC_1Pt_FlAG                   (5450 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_GMC_OFFSET_X_BASE              (5451 + GLOBAL_MPEG4_X_STRUCT) //1 short
#define MPEG4_DEC_GMC_OFFSET_Y_BASE              (5452 + GLOBAL_MPEG4_X_STRUCT) //1 short


//GLOBAL_VARS
#define GLOBAL_MPEG4_Y_ADDR                                 (0 + MPEG4_GLOBAL_Y_BEGIN_ADDR) //7456 short
//GLOBAL_MPEG4_CONSTY_STRUCT
#define GLOBAL_MPEG4_CONSTY_STRUCT                          (7456 + MPEG4_GLOBAL_Y_BEGIN_ADDR)
#define GLOBAL_MPEG4_MPEG_DC_SCALAR_ADDR                    (0 + GLOBAL_MPEG4_CONSTY_STRUCT) //64 short
#define GLOBAL_MPEG4_dc_lum_tab_ADDR                        (64 + GLOBAL_MPEG4_CONSTY_STRUCT) //16 short
#define GLOBAL_MPEG4_MPEG_DC_SCALAR_MULT_ADDR               (80 + GLOBAL_MPEG4_CONSTY_STRUCT) //64 short
#define GLOBAL_MPEG4_iQuant_MULT_ADDR                       (144 + GLOBAL_MPEG4_CONSTY_STRUCT) //32 short
#define CONST_MPEG4_DEC_0x00000080_ADDR                     (176 + GLOBAL_MPEG4_CONSTY_STRUCT) //2 short
#define CONST_MPEG4_DEC_1108_ADDR                           (178 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define CONST_MPEG4_DEC_3784_ADDR                           (179 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define CONST_MPEG4_DEC_1568_ADDR                           (180 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define CONST_MPEG4_DEC_565_ADDR                            (181 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define CONST_MPEG4_DEC_2276_ADDR                           (182 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define CONST_MPEG4_DEC_3406_ADDR                           (183 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define CONST_MPEG4_DEC_2408_ADDR                           (184 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define CONST_MPEG4_DEC_799_ADDR                            (185 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define CONST_MPEG4_DEC_4017_ADDR                           (186 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define CONST_MPEG4_DEC_181_ADDR                            (187 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define CONST_MPEG4_DEC_0x00000040_ADDR                     (188 + GLOBAL_MPEG4_CONSTY_STRUCT) //2 short
#define CONST_MPEG4_DEC_4_ADDR                              (190 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define CONST_MPEG4_DEC_32_ADDR                             (191 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define CONST_MPEG4_DEC_0x00002000_ADDR                     (192 + GLOBAL_MPEG4_CONSTY_STRUCT) //2 short
#define CONST_MPEG4_DEC_63_ADDR                             (194 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define CONST_MPEG4_DEC_56_ADDR                             (195 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define CONST_MPEG4_DEC_31_ADDR                             (196 + GLOBAL_MPEG4_CONSTY_STRUCT) //44 short
#define GLOBAL_MPEG4_FIX_0_541196100_ADDR                   (240 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_FIX_1_847759065_ADDR                   (241 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_FIX_0_765366865_ADDR                   (242 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_FIX_1_306562965_ADDR                   (243 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_FIX_0_899976223_ADDR                   (244 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_FIX_0_601344887_ADDR                   (245 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_0x0020_ADDR                            (246 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_0x0400_ADDR                            (247 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_0x00000400_ADDR                        (248 + GLOBAL_MPEG4_CONSTY_STRUCT) //2 short
#define GLOBAL_MPEG4_0x04000000_ADDR                        (250 + GLOBAL_MPEG4_CONSTY_STRUCT) //2 short
#define GLOBAL_MPEG4_0x00000003_ADDR                        (252 + GLOBAL_MPEG4_CONSTY_STRUCT) //2 short
#define GLOBAL_MPEG4_0x0001_ADDR                            (254 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_0x0002_ADDR                            (255 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_0x0004_ADDR                            (256 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_0x0008_ADDR                            (257 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_0x0006_ADDR                            (258 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_0x000E_ADDR                            (259 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_0x00ff_ADDR                            (260 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_0xff00_ADDR                            (261 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_0x000f_ADDR                            (262 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_round_chroma_0xC0F8_ADDR               (263 + GLOBAL_MPEG4_CONSTY_STRUCT) //1 short
#define GLOBAL_MPEG4_0x00000010_ADDR                        (264 + GLOBAL_MPEG4_CONSTY_STRUCT) //2 short
#define GLOBAL_MPEG4_0x0000001c_ADDR                        (266 + GLOBAL_MPEG4_CONSTY_STRUCT) //2 short
#define GLOBAL_MPEG4_0x00000014_ADDR                        (268 + GLOBAL_MPEG4_CONSTY_STRUCT) //2 short
#define GLOBAL_MPEG4_scan_tables_ADDR                       (270 + GLOBAL_MPEG4_CONSTY_STRUCT) //192 short
#define GLOBAL_MPEG4_max_level_voc_ADDR                     (462 + GLOBAL_MPEG4_CONSTY_STRUCT) //128 short
#define GLOBAL_MPEG4_max_run_voc_ADDR                       (590 + GLOBAL_MPEG4_CONSTY_STRUCT) //128 short
#define GLOBAL_MPEG4_mcbpc_inter_table_ADDR                 (718 + GLOBAL_MPEG4_CONSTY_STRUCT) //514 short
#define GLOBAL_MPEG4_cbpy_table_ADDR                        (1232 + GLOBAL_MPEG4_CONSTY_STRUCT) //128 short
#define GLOBAL_MPEG4_TMNMVtab0voc_ADDR                      (1360 + GLOBAL_MPEG4_CONSTY_STRUCT) //28 short
#define GLOBAL_MPEG4_TMNMVtab1voc_ADDR                      (1388 + GLOBAL_MPEG4_CONSTY_STRUCT) //192 short
#define GLOBAL_MPEG4_TMNMVtab2voc_ADDR                      (1580 + GLOBAL_MPEG4_CONSTY_STRUCT) //248 short
#define GLOBAL_MPEG4_mcbpc_intra_table_voc_ADDR             (1828 + GLOBAL_MPEG4_CONSTY_STRUCT) //128 short
#define GLOBAL_MPEG4_roundtab_76_ADDR                       (1956 + GLOBAL_MPEG4_CONSTY_STRUCT) //16 short
#define GLOBAL_MPEG4_max_level_h263_ADDR                    (1972 + GLOBAL_MPEG4_CONSTY_STRUCT) //128 short
#define GLOBAL_MPEG4_max_run_h263_ADDR                      (2100 + GLOBAL_MPEG4_CONSTY_STRUCT) //128 short


/*******************
 **  MPEG4_LOCAL  **
 *******************/

//CS_dec_mb_gmc_16x16
#define CS_dec_mb_gmc_16x16_X_BEGIN_ADDR                    (0 + MPEG4_LOCAL_X_BEGIN_ADDR)
#define MPEG4_TEMP_DMA_DATA_16x16_NEW_ADDR                  (0 + CS_dec_mb_gmc_16x16_X_BEGIN_ADDR) //1 short

//CS_dec_mb_gmc_8x8
#define CS_dec_mb_gmc_8x8_X_BEGIN_ADDR                      (0 + MPEG4_LOCAL_X_BEGIN_ADDR)
#define GLOBAL_MPEG4_0x000C_ADDR                            (0 + CS_dec_mb_gmc_8x8_X_BEGIN_ADDR) //1 short
#define GLOBAL_MPEG4_GMC_SW_FLAG                            (1 + CS_dec_mb_gmc_8x8_X_BEGIN_ADDR) //1 short
#define MPEG4_TEMP_DMA_DATA_8x8_NEW_ADDR                    (2 + CS_dec_mb_gmc_8x8_X_BEGIN_ADDR) //1 short

/*******************
 **  H263_GLOBAL  **
 *******************/

//GLOBAL_VARS
#define H263_G_X_bs_buf                                     (0 + H263_GLOBAL_X_BEGIN_ADDR) //512 short
//H263_GLOBAL_X_STRUCT
#define H263_GLOBAL_X_STRUCT                                (512 + H263_GLOBAL_X_BEGIN_ADDR)
#define H263_G_X_mb_width                                   (0 + H263_GLOBAL_X_STRUCT) //1 short
#define H263_G_X_mb_height                                  (1 + H263_GLOBAL_X_STRUCT) //1 short
#define H263_G_X_gob                                        (2 + H263_GLOBAL_X_STRUCT) //1 short
#define H263_G_X_bs_bitcnt                                  (3 + H263_GLOBAL_X_STRUCT) //1 short
#define H263_G_X_bs_cache                                   (4 + H263_GLOBAL_X_STRUCT) //2 short
#define H263_G_X_bs_buf_extaddr                             (6 + H263_GLOBAL_X_STRUCT) //2 short
#define H263_G_X_prev_I_P_frame0_extaddr                    (8 + H263_GLOBAL_X_STRUCT) //2 short
#define H263_G_X_prev_I_P_frame1_extaddr                    (10 + H263_GLOBAL_X_STRUCT) //2 short
#define H263_G_X_prev_I_P_frame2_extaddr                    (12 + H263_GLOBAL_X_STRUCT) //2 short
#define H263_G_X_current_frame0_extaddr                     (14 + H263_GLOBAL_X_STRUCT) //2 short
#define H263_G_X_current_frame1_extaddr                     (16 + H263_GLOBAL_X_STRUCT) //2 short
#define H263_G_X_current_frame2_extaddr                     (18 + H263_GLOBAL_X_STRUCT) //2 short

#define H263_G_X_errorstate                                 (532 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_newgob                                     (533 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_MV                                         (534 + H263_GLOBAL_X_BEGIN_ADDR) //1104 short
#define H263_G_X_MV_topline                                 (1638 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_MV_curline                                 (1639 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_modemap                                    (1640 + H263_GLOBAL_X_BEGIN_ADDR) //92 short
#define H263_G_X_modemap_topline                            (1732 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_modemap_curline                            (1733 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_fault                                      (1734 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_bs_half_consume                            (1735 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_bs_buf_pos                                 (1736 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_bs_need_dma                                (1737 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_pgfid                                      (1738 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_UFEP                                       (1739 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_rtype                                      (1740 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_source_format                              (1741 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_coded_picture_width                        (1742 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_coded_picture_height                       (1743 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_chrom_width                                (1744 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_chrom_height                               (1745 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_pict_type                                  (1746 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_long_vectors                               (1747 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_syntax_arith_coding                        (1748 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_mv_outside_frame                           (1749 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_pb_frame                                   (1750 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_quant                                      (1751 + H263_GLOBAL_X_BEGIN_ADDR) //1 short
#define H263_G_X_dma_buf                                    (1752 + H263_GLOBAL_X_BEGIN_ADDR) //600 short
#define H263_G_X_rec_blcok                                  (2352 + H263_GLOBAL_X_BEGIN_ADDR) //384 short
#define H263_G_X_blcok                                      (2736 + H263_GLOBAL_X_BEGIN_ADDR) //384 short
#define H263_G_X_idct_tmpbuf                                (3120 + H263_GLOBAL_X_BEGIN_ADDR) //8 short
//H263_L_X_STRUCT_get_I_P_MBs
#define H263_L_X_STRUCT_get_I_P_MBs                         (3128 + H263_GLOBAL_X_BEGIN_ADDR)
#define H263_L_X_MBA                                        (0 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_COD                                        (1 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_bx                                         (2 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_by                                         (3 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_MBAmax                                     (4 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_MCBPC                                      (5 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_CBPY                                       (6 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_DQUANT                                     (7 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_CBP                                        (8 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_Mode                                       (9 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_gobheader_read                             (10 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_bp                                         (11 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_mvx                                        (12 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_mvy                                        (13 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_pmv0                                       (14 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_pmv1                                       (15 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_xpos                                       (16 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_ypos                                       (17 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_startmv                                    (18 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_stopmv                                     (19 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_offset                                     (20 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_bsize                                      (21 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_last_done                                  (22 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_pCBP                                       (23 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_pnewgob                                    (24 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_ModeBit                                    (25 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_LOWRES                                     (26 + H263_L_X_STRUCT_get_I_P_MBs) //1 short
#define H263_L_X_STRIDE                                     (27 + H263_L_X_STRUCT_get_I_P_MBs) //1 short


//GLOBAL_VARS
#define H263_G_Y_CONST_START                                (0 + H263_GLOBAL_Y_BEGIN_ADDR) //16 short
//H263_GLOBAL_CONSTY_STRUCT
#define H263_GLOBAL_CONSTY_STRUCT                           (16 + H263_GLOBAL_Y_BEGIN_ADDR)
#define H263_G_Y_MCBPCtabintra                              (0 + H263_GLOBAL_CONSTY_STRUCT) //32 short
#define H263_G_Y_MCBPCtab0                                  (32 + H263_GLOBAL_CONSTY_STRUCT) //256 short
#define H263_G_Y_MCBPCtab1                                  (288 + H263_GLOBAL_CONSTY_STRUCT) //8 short
#define H263_G_Y_TMNMVtab0                                  (296 + H263_GLOBAL_CONSTY_STRUCT) //14 short
#define H263_G_Y_TMNMVtab1                                  (310 + H263_GLOBAL_CONSTY_STRUCT) //96 short
#define H263_G_Y_TMNMVtab2                                  (406 + H263_GLOBAL_CONSTY_STRUCT) //123 short
#define H263_G_Y_CONST_1108                                 (529 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_3784                                 (530 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_1568                                 (531 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_565                                  (532 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_2276                                 (533 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_3406                                 (534 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_2408                                 (535 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_799                                  (536 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_4017                                 (537 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_181                                  (538 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_4                                    (539 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_32                                   (540 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_63                                   (541 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_56                                   (542 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_31                                   (543 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_0x00000080                           (544 + H263_GLOBAL_CONSTY_STRUCT) //2 short
#define H263_G_Y_CONST_0x00002000                           (546 + H263_GLOBAL_CONSTY_STRUCT) //2 short
#define H263_G_Y_CONST_0x0020                               (548 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_0x0400                               (549 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_0x00000400                           (550 + H263_GLOBAL_CONSTY_STRUCT) //2 short
#define H263_G_Y_CONST_0x04000000                           (552 + H263_GLOBAL_CONSTY_STRUCT) //2 short
#define H263_G_Y_CONST_0x00000003                           (554 + H263_GLOBAL_CONSTY_STRUCT) //2 short
#define H263_G_Y_CONST_0x0001                               (556 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_0x0002                               (557 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_0x0004                               (558 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_0x0008                               (559 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_0x0006                               (560 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_0x000E                               (561 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_0x00ff                               (562 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_0xff00                               (563 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_0x000f                               (564 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CONST_round_chroma_0xC0F8                  (565 + H263_GLOBAL_CONSTY_STRUCT) //1 short
#define H263_G_Y_CBPYtab                                    (566 + H263_GLOBAL_CONSTY_STRUCT) //48 short
#define H263_G_Y_DCT3Dtab0                                  (614 + H263_GLOBAL_CONSTY_STRUCT) //224 short
#define H263_G_Y_DCT3Dtab1                                  (838 + H263_GLOBAL_CONSTY_STRUCT) //192 short
#define H263_G_Y_DCT3Dtab2                                  (1030 + H263_GLOBAL_CONSTY_STRUCT) //242 short
#define H263_G_Y_zig_zag_scan                               (1272 + H263_GLOBAL_CONSTY_STRUCT) //64 short

#define H263_G_Y_idct_tmpbuf                                (1352 + H263_GLOBAL_Y_BEGIN_ADDR) //8 short
#define H263_DMA_Ystridesize_ADDR                           (1360 + H263_GLOBAL_Y_BEGIN_ADDR) //1 short
#define H263_DMA_UVstridesize_ADDR                          (1361 + H263_GLOBAL_Y_BEGIN_ADDR) //1 short
#define H263_DMA_OUT_Y_ADDR                                 (1362 + H263_GLOBAL_Y_BEGIN_ADDR) //2 short
#define H263_DMA_OUT_U_ADDR                                 (1364 + H263_GLOBAL_Y_BEGIN_ADDR) //2 short
#define H263_DMA_OUT_V_ADDR                                 (1366 + H263_GLOBAL_Y_BEGIN_ADDR) //2 short
#define H263_DMA_IN_mcsx                                    (1368 + H263_GLOBAL_Y_BEGIN_ADDR) //1 short
#define H263_DMA_IN_mcsy                                    (1369 + H263_GLOBAL_Y_BEGIN_ADDR) //1 short
#define H263_DMA_IN_Y_ADDR                                  (1370 + H263_GLOBAL_Y_BEGIN_ADDR) //2 short
#define H263_DMA_IN_U_ADDR                                  (1372 + H263_GLOBAL_Y_BEGIN_ADDR) //2 short
#define H263_DMA_IN_V_ADDR                                  (1374 + H263_GLOBAL_Y_BEGIN_ADDR) //2 short
#define H263_MC_Y_Pred_type                                 (1376 + H263_GLOBAL_Y_BEGIN_ADDR) //1 short
#define H263_MC_UV_Pred_type                                (1377 + H263_GLOBAL_Y_BEGIN_ADDR) //1 short
#define H263_DMA_Cur_Modebit                                (1378 + H263_GLOBAL_Y_BEGIN_ADDR) //1 short

/******************
 **  H263_LOCAL  **
 ******************/

/********************
 **  JPEG2_GLOBAL  **
 ********************/

//GLOBAL_VARS
#define JPEG_GX_bs_buf                                      (0 + JPEG2_GLOBAL_X_BEGIN_ADDR) //512 short
//JPEG_GLOBAL_X_STRUCT
#define JPEG_GLOBAL_X_STRUCT                                (512 + JPEG2_GLOBAL_X_BEGIN_ADDR)
#define JPEG_GX_bs_cache                                    (0 + JPEG_GLOBAL_X_STRUCT) //2 short
#define JPEG_GX_bs_buf_extaddr                              (2 + JPEG_GLOBAL_X_STRUCT) //2 short
#define JPEG_GX_bs_bitcnt                                   (4 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_dec_complete_flag                           (5 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_bs_bytes_in_buffer                          (6 + JPEG_GLOBAL_X_STRUCT) //2 short
#define JPEG_GX_dest_buffer_EXT                             (8 + JPEG_GLOBAL_X_STRUCT) //2 short
#define JPEG_GX_dest_bufferU_EXT                            (10 + JPEG_GLOBAL_X_STRUCT) //2 short
#define JPEG_GX_dest_bufferV_EXT                            (12 + JPEG_GLOBAL_X_STRUCT) //2 short
#define JPEG_GX_width                                       (14 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_height                                      (15 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_dest_width                                  (16 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_dest_height                                 (17 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_output_width                                (18 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_output_height                               (19 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_MCU_vert_offset                             (20 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_MCU_rows_per_iMCU_row                       (21 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_MCU_ctr                                     (22 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_MCUs_per_row                                (23 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_total_iMCU_rows                             (24 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_blocks_in_MCU                               (25 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_comps_in_scan                               (26 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_restart_interval                            (27 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_restarts_to_go                              (28 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_insufficient_data                           (29 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_MCU_cut_row_start                           (30 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_MCU_cut_col_start                           (31 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_MCU_cut_row_end                             (32 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_MCU_cut_col_end                             (33 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_component_needed                            (34 + JPEG_GLOBAL_X_STRUCT) //3 short
#define JPEG_GX_MCU_blocks                                  (37 + JPEG_GLOBAL_X_STRUCT) //3 short
#define JPEG_GX_DCT_scaled_size                             (40 + JPEG_GLOBAL_X_STRUCT) //3 short
#define JPEG_GX_MCU_width                                   (43 + JPEG_GLOBAL_X_STRUCT) //3 short
#define JPEG_GX_MCU_height                                  (46 + JPEG_GLOBAL_X_STRUCT) //3 short
#define JPEG_GX_last_col_width                              (49 + JPEG_GLOBAL_X_STRUCT) //3 short
#define JPEG_GX_last_row_height                             (52 + JPEG_GLOBAL_X_STRUCT) //3 short
#define JPEG_GX_MCU_sample_width                            (55 + JPEG_GLOBAL_X_STRUCT) //3 short
#define JPEG_GX_width_in_blocks                             (58 + JPEG_GLOBAL_X_STRUCT) //3 short
#define JPEG_GX_v_samp_factor                               (61 + JPEG_GLOBAL_X_STRUCT) //3 short
#define JPEG_GX_h_samp_factor                               (64 + JPEG_GLOBAL_X_STRUCT) //3 short
#define JPEG_GX_bs_half_consume                             (67 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_dc_tbl_no                                   (68 + JPEG_GLOBAL_X_STRUCT) //3 short
#define JPEG_GX_ac_tbl_no                                   (71 + JPEG_GLOBAL_X_STRUCT) //3 short
#define JPEG_GX_MCU_membership                              (74 + JPEG_GLOBAL_X_STRUCT) //10 short
#define JPEG_GX_dc_needed                                   (84 + JPEG_GLOBAL_X_STRUCT) //10 short
#define JPEG_GX_ac_needed                                   (94 + JPEG_GLOBAL_X_STRUCT) //10 short
#define JPEG_GX_finish_output                               (104 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_process_data                                (105 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_decompress_data                             (106 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_decode_mcu                                  (107 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_post_process_data                           (108 + JPEG_GLOBAL_X_STRUCT) //1 short
#define JPEG_GX_bs_skip_bits                                (109 + JPEG_GLOBAL_X_STRUCT) //1 short

#define JPEG_GX_bs_need_dma                                 (622 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_out_row_ctr                                 (623 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_scanline                             (624 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_yoffset                                     (625 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_input_iMCU_row                              (626 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_MCU_col_num                                 (627 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_blkn                                        (628 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_ci                                          (629 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_useful_width                                (630 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_useful_height                               (631 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_start_col                                   (632 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_col                                  (633 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_ptr                                  (634 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_yindex                                      (635 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_xindex                                      (636 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_MCU_buffer_ptr                              (637 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_last_dc_val                                 (638 + JPEG2_GLOBAL_X_BEGIN_ADDR) //4 short
#define JPEG_GX_YUV_output_buffer_ptr                       (642 + JPEG2_GLOBAL_X_BEGIN_ADDR) //3 short
#define JPEG_GX_decode_mcu_blkn                             (645 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_ci_01                                       (646 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_huffval_ptr                                 (647 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_maxcode_ptr                                 (648 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_valoffset_ptr                               (649 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_look_nbits_ptr                              (650 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_look_sym_ptr                                (651 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_s                                           (652 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_iquant_dct_table_ptr                        (653 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_buffer_stride                        (654 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_YUV_output_buffer_stride                    (655 + JPEG2_GLOBAL_X_BEGIN_ADDR) //3 short
#define JPEG_GX_MCU_buffer                                  (658 + JPEG2_GLOBAL_X_BEGIN_ADDR) //640 short
#define JPEG_GX_YUV_output_buffer                           (1298 + JPEG2_GLOBAL_X_BEGIN_ADDR) //7680 short
#define JPEG_GX_Cr_r_tab                                    (8978 + JPEG2_GLOBAL_X_BEGIN_ADDR) //256 short
#define JPEG_GX_Cb_b_tab                                    (9234 + JPEG2_GLOBAL_X_BEGIN_ADDR) //256 short
#define JPEG_GX_xbuffer0_ptr                                (9490 + JPEG2_GLOBAL_X_BEGIN_ADDR) //3 short
#define JPEG_GX_xbuffer1_ptr                                (9493 + JPEG2_GLOBAL_X_BEGIN_ADDR) //3 short
#define JPEG_GX_is_yuv_format                               (9496 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_YUV_output_buffer_current_ptr               (9497 + JPEG2_GLOBAL_X_BEGIN_ADDR) //3 short
#define JPEG_GX_smp_cut_row_start                           (9500 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_smp_cut_col_start                           (9501 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_smp_cut_row_end                             (9502 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_smp_cut_col_end                             (9503 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_smp_cut_row_length                          (9504 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_smp_cut_col_length                          (9505 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_iMCU_row_ctr                                (9506 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_context_state                               (9507 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_min_DCT_scaled_size                         (9508 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_main_whichptr                               (9509 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_buffer_full                                 (9510 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_out_color_space                             (9511 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_dest_buffer_align                           (9512 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_dest_buffer_height                          (9513 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_upsample_output_startX                      (9514 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_upsample_output_endX                        (9515 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_num_components                              (9516 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_comp_info                                   (9517 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_half_Y                               (9518 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_half_UV                              (9519 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_half_RGB                             (9520 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_idict_half_YUV                              (9521 + JPEG2_GLOBAL_X_BEGIN_ADDR) //3 short
#define JPEG_GX_output_cut_col_num                          (9524 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_cut_row_num                          (9525 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_post_process_data_input_buf_ADDR            (9526 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_rowgroup_ctr                                (9527 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_rowgroups_avail                             (9528 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_out_rows_avail                              (9529 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_upsample_spare_full                         (9530 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_upsample_out_row_width                      (9531 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_upsample_num_rows                           (9532 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_upsample_rows_to_go                         (9533 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_upsample_work_ptrs                          (9534 + JPEG2_GLOBAL_X_BEGIN_ADDR) //2 short
#define JPEG_GX_upsample_next_row_out                       (9536 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_max_v_samp_factor                           (9537 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_shrink_output_startX                        (9538 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_shrink_output_startY                        (9539 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_shrink_output_endX                          (9540 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_shrink_output_endY                          (9541 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_k                      (9542 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_df                     (9543 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_nx                     (9544 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_ny                     (9545 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_qx0                    (9546 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_qx1                    (9547 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_qy0                    (9548 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_qy1                    (9549 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_q0                     (9550 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_q1                     (9551 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_q2                     (9552 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_q3                     (9553 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_ux_offset              (9554 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_pixelBytes             (9555 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_output_width           (9556 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_output_shrink_rgb565_dest_width             (9557 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_CONST_1                                        (9558 + JPEG2_GLOBAL_X_BEGIN_ADDR) //2 short
#define JPEG_CONST_2                                        (9560 + JPEG2_GLOBAL_X_BEGIN_ADDR) //2 short
#define JPEG_CONST_4                                        (9562 + JPEG2_GLOBAL_X_BEGIN_ADDR) //2 short
#define JPEG_CONST_8                                        (9564 + JPEG2_GLOBAL_X_BEGIN_ADDR) //2 short
#define JPEG_CONST_16                                       (9566 + JPEG2_GLOBAL_X_BEGIN_ADDR) //2 short
#define JPEG_CONST_128                                      (9568 + JPEG2_GLOBAL_X_BEGIN_ADDR) //2 short
#define JPEG_CONST_256                                      (9570 + JPEG2_GLOBAL_X_BEGIN_ADDR) //2 short
#define JPEG_CONST_65536                                    (9572 + JPEG2_GLOBAL_X_BEGIN_ADDR) //2 short
#define JPEG_GX_put_pixel_rows                              (9574 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_upsample_method                             (9575 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_color_convert                               (9576 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_next_restart_num                            (9577 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_pro_mode                                    (9578 + JPEG2_GLOBAL_X_BEGIN_ADDR) //2 short
#define JPEG_GX_output_iMCU_row                             (9580 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_last_iMCU_row                               (9581 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
#define JPEG_GX_height_in_blocks                            (9582 + JPEG2_GLOBAL_X_BEGIN_ADDR) //3 short
#define JPEG_PRO_GX_block_rows                              (9585 + JPEG2_GLOBAL_X_BEGIN_ADDR) //1 short
//JPEG_PRO_GLOBAL_X_STRUCT
#define JPEG_PRO_GLOBAL_X_STRUCT                            (9586 + JPEG2_GLOBAL_X_BEGIN_ADDR)
#define JPEG_PRO_GX_COEF_IMGE_IN_ADDR                       (0 + JPEG_PRO_GLOBAL_X_STRUCT) //6 short
#define JPEG_PRO_GX_COEF_blocksperrow_stride                (6 + JPEG_PRO_GLOBAL_X_STRUCT) //6 short
#define JPEG_PRO_GX_COEF_buffer_row_addr                    (12 + JPEG_PRO_GLOBAL_X_STRUCT) //2 short


//GLOBAL_VARS
#define JPEG_GY_CONST_START                                 (0 + JPEG2_GLOBAL_Y_BEGIN_ADDR) //16 short
//JPEG_GY_CONSTY_STRUCT
#define JPEG_GY_CONSTY_STRUCT                               (16 + JPEG2_GLOBAL_Y_BEGIN_ADDR)
#define JPEG_GY_dc_maxcode0                                 (0 + JPEG_GY_CONSTY_STRUCT) //36 short
#define JPEG_GY_dc_valoffset0                               (36 + JPEG_GY_CONSTY_STRUCT) //34 short
#define JPEG_GY_dc_look_nbits0                              (70 + JPEG_GY_CONSTY_STRUCT) //128 short
#define JPEG_GY_dc_look_sym0                                (198 + JPEG_GY_CONSTY_STRUCT) //128 short
#define JPEG_GY_dc_maxcode1                                 (326 + JPEG_GY_CONSTY_STRUCT) //36 short
#define JPEG_GY_dc_valoffset1                               (362 + JPEG_GY_CONSTY_STRUCT) //34 short
#define JPEG_GY_dc_look_nbits1                              (396 + JPEG_GY_CONSTY_STRUCT) //128 short
#define JPEG_GY_dc_look_sym1                                (524 + JPEG_GY_CONSTY_STRUCT) //128 short
#define JPEG_GY_ac_maxcode0                                 (652 + JPEG_GY_CONSTY_STRUCT) //36 short
#define JPEG_GY_ac_valoffset0                               (688 + JPEG_GY_CONSTY_STRUCT) //34 short
#define JPEG_GY_ac_look_nbits0                              (722 + JPEG_GY_CONSTY_STRUCT) //128 short
#define JPEG_GY_ac_look_sym0                                (850 + JPEG_GY_CONSTY_STRUCT) //128 short
#define JPEG_GY_ac_maxcode1                                 (978 + JPEG_GY_CONSTY_STRUCT) //36 short
#define JPEG_GY_ac_valoffset1                               (1014 + JPEG_GY_CONSTY_STRUCT) //34 short
#define JPEG_GY_ac_look_nbits1                              (1048 + JPEG_GY_CONSTY_STRUCT) //128 short
#define JPEG_GY_ac_look_sym1                                (1176 + JPEG_GY_CONSTY_STRUCT) //128 short
#define JPEG_GY_dc_huffval0                                 (1304 + JPEG_GY_CONSTY_STRUCT) //128 short
#define JPEG_GY_dc_huffval1                                 (1432 + JPEG_GY_CONSTY_STRUCT) //128 short
#define JPEG_GY_ac_huffval0                                 (1560 + JPEG_GY_CONSTY_STRUCT) //128 short
#define JPEG_GY_ac_huffval1                                 (1688 + JPEG_GY_CONSTY_STRUCT) //128 short
#define JPEG_GY_jpeg_natural_order                          (1816 + JPEG_GY_CONSTY_STRUCT) //64 short
#define JPEG_GY_iquant_dct_tableY                           (1880 + JPEG_GY_CONSTY_STRUCT) //64 short
#define JPEG_GY_iquant_dct_tableUV                          (1944 + JPEG_GY_CONSTY_STRUCT) //64 short
#define JPEG_GY_rgroup                                      (2008 + JPEG_GY_CONSTY_STRUCT) //3 short
#define JPEG_GY_rows_left                                   (2011 + JPEG_GY_CONSTY_STRUCT) //3 short
#define JPEG_GY_last_rowgroups_avail                        (2014 + JPEG_GY_CONSTY_STRUCT) //1 short
#define JPEG_GY_reserved                                    (2015 + JPEG_GY_CONSTY_STRUCT) //1 short

#define JPEG_GY_tmpbuf                                      (2032 + JPEG2_GLOBAL_Y_BEGIN_ADDR) //8 short
#define JPEG_GY_Cr_g_tab                                    (2040 + JPEG2_GLOBAL_Y_BEGIN_ADDR) //512 short
#define JPEG_GY_Cb_g_tab                                    (2552 + JPEG2_GLOBAL_Y_BEGIN_ADDR) //512 short
#define JPEG_GY_RGB_output_buffer_ptr                       (3064 + JPEG2_GLOBAL_Y_BEGIN_ADDR) //2 short
#define JPEG_GY_RGB_output_buffer_now                       (3066 + JPEG2_GLOBAL_Y_BEGIN_ADDR) //1 short
#define JPEG_GY_RGB_output_buffer_now_reserved              (3067 + JPEG2_GLOBAL_Y_BEGIN_ADDR) //1 short
#define JPEG_GY_RGB_upsample_spare_row                      (3068 + JPEG2_GLOBAL_Y_BEGIN_ADDR) //960 short
#define JPEG_GY_RGB_output_buffer0                          (4028 + JPEG2_GLOBAL_Y_BEGIN_ADDR) //960 short
#define JPEG_GY_RGB_output_buffer1                          (4988 + JPEG2_GLOBAL_Y_BEGIN_ADDR) //2 short
#define JPEG_GY_RGB565_output_buffer                        (4990 + JPEG2_GLOBAL_Y_BEGIN_ADDR) //640 short
#define JPEG_GY_output_shrink_rgb565_f                      (5630 + JPEG2_GLOBAL_Y_BEGIN_ADDR) //3840 short

/*******************
 **  JPEG2_LOCAL  **
 *******************/

/******************
 **  MP3_GLOBAL  **
 ******************/

//GLOBAL_VARS
#define GLOBAL_INPUT0_DATA_ADDR                             (0 + MP3_GLOBAL_X_BEGIN_ADDR) //1024 short
#define CONST_0x00000800_ADDR                               (1024 + MP3_GLOBAL_X_BEGIN_ADDR) //2 short
#define TABLE_RAM_X_BEGINE_ADDR                             (CONST_0x00000800_ADDR)
#define CONST_neg1_ADDR                                     (1026 + MP3_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_0xe0ff_ADDR                                   (1027 + MP3_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_1152_ADDR                                     (1028 + MP3_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_17_ADDR                                       (1029 + MP3_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_576_ADDR                                      (1030 + MP3_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_1024_ADDR                                     (1031 + MP3_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_2048_ADDR                                     (CONST_0x00000800_ADDR)
#define TABLE_hufftab0_ADDR                                 (1032 + MP3_GLOBAL_X_BEGIN_ADDR) //2 short
#define TABLE_hufftab1_ADDR                                 (1034 + MP3_GLOBAL_X_BEGIN_ADDR) //8 short
#define TABLE_hufftab2_ADDR                                 (1042 + MP3_GLOBAL_X_BEGIN_ADDR) //16 short
#define TABLE_hufftab3_ADDR                                 (1058 + MP3_GLOBAL_X_BEGIN_ADDR) //16 short
#define TABLE_hufftab5_ADDR                                 (1074 + MP3_GLOBAL_X_BEGIN_ADDR) //26 short
#define TABLE_hufftab6_ADDR                                 (1100 + MP3_GLOBAL_X_BEGIN_ADDR) //28 short
#define TABLE_hufftab7_ADDR                                 (1128 + MP3_GLOBAL_X_BEGIN_ADDR) //64 short
#define TABLE_hufftab8_ADDR                                 (1192 + MP3_GLOBAL_X_BEGIN_ADDR) //66 short
#define TABLE_hufftab9_ADDR                                 (1258 + MP3_GLOBAL_X_BEGIN_ADDR) //54 short
#define TABLE_hufftab10_ADDR                                (1312 + MP3_GLOBAL_X_BEGIN_ADDR) //102 short
#define TABLE_hufftab11_ADDR                                (1414 + MP3_GLOBAL_X_BEGIN_ADDR) //104 short
#define TABLE_hufftab12_ADDR                                (1518 + MP3_GLOBAL_X_BEGIN_ADDR) //96 short
#define TABLE_hufftab13_ADDR                                (1614 + MP3_GLOBAL_X_BEGIN_ADDR) //398 short
#define TABLE_hufftab15_ADDR                                (2012 + MP3_GLOBAL_X_BEGIN_ADDR) //380 short
#define TABLE_hufftab16_ADDR                                (2392 + MP3_GLOBAL_X_BEGIN_ADDR) //402 short
#define TABLE_hufftab24_ADDR                                (2794 + MP3_GLOBAL_X_BEGIN_ADDR) //386 short
#define TABLE_mad_huff_pair_table_ADDR                      (3180 + MP3_GLOBAL_X_BEGIN_ADDR) //64 short
#define OUTPUT_PCM_BUFFER1_ADDR                             (3244 + MP3_GLOBAL_X_BEGIN_ADDR) //2304 short
#define OUTPUT_PCM_BUFFER2_ADDR                             ((1152+OUTPUT_PCM_BUFFER1_ADDR))
#define STATIC_MAD_SBSAMPLE_ADDR                            (5548 + MP3_GLOBAL_X_BEGIN_ADDR) //4608 short
#define GLOBAL_NON_CLIP_CROSSZEROINDEX_MP3                  ((STATIC_MAD_SBSAMPLE_ADDR+2304))
#define GLOBAL_NON_CLIP_TEMP_BUFFER_MP3                     (STATIC_MAD_SBSAMPLE_ADDR)
#define LOCAL_X_START_ADDR                                  ((4608+STATIC_MAD_SBSAMPLE_ADDR))
#define LOCAL_BX_ADDR                                       (LOCAL_X_START_ADDR)

//GLOBAL_VARS
#define GLOBAL_NON_CLIP_HISTORYDATA_L_MP3_ADDR              (0 + MP3_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_NON_CLIP_HISTORYDATA_R_MP3_ADDR              (2 + MP3_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_DIGITAL_GAIN_MP3_ADDR                        (4 + MP3_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DIGITAL_MAXVALUE_MP3_ADDR                    (5 + MP3_GLOBAL_Y_BEGIN_ADDR) //1 short
#define CONST_reseverd_ADDR                                 (6 + MP3_GLOBAL_Y_BEGIN_ADDR) //2 short
#define TABLE_hufftabA_ADDR                                 (8 + MP3_GLOBAL_Y_BEGIN_ADDR) //28 short
#define TABLE_RAM_Y_BEGINE_ADDR                             (TABLE_hufftabA_ADDR)
#define TABLE_imdct36_y_tab_ADDR                            (36 + MP3_GLOBAL_Y_BEGIN_ADDR) //18 short
#define TABLE_D_ADDR                                        (54 + MP3_GLOBAL_Y_BEGIN_ADDR) //544 short
#define TABLE_hufftabB_ADDR                                 (598 + MP3_GLOBAL_Y_BEGIN_ADDR) //16 short
#define TABLE_sflen_table_ADDR                              (614 + MP3_GLOBAL_Y_BEGIN_ADDR) //32 short
#define TABLE_nsfb_table_ADDR                               (646 + MP3_GLOBAL_Y_BEGIN_ADDR) //72 short
#define TABLE_pretab_ADDR                                   (718 + MP3_GLOBAL_Y_BEGIN_ADDR) //22 short
#define TABLE_cs_ADDR                                       (740 + MP3_GLOBAL_Y_BEGIN_ADDR) //8 short
#define TABLE_ca_ADDR                                       (748 + MP3_GLOBAL_Y_BEGIN_ADDR) //8 short
#define TABLE_imdct_s_ADDR                                  (756 + MP3_GLOBAL_Y_BEGIN_ADDR) //36 short
#define TABLE_window_l_ADDR                                 (792 + MP3_GLOBAL_Y_BEGIN_ADDR) //36 short
#define TABLE_window_s_ADDR                                 (828 + MP3_GLOBAL_Y_BEGIN_ADDR) //12 short
#define TABLE_is_table_ADDR                                 (840 + MP3_GLOBAL_Y_BEGIN_ADDR) //8 short
#define TABLE_is_lsf_table_ADDR                             (848 + MP3_GLOBAL_Y_BEGIN_ADDR) //30 short
#define TABLE_bitrate_table_ADDR                            (878 + MP3_GLOBAL_Y_BEGIN_ADDR) //80 short
#define TABLE_imdct36_tab_ADDR                              (958 + MP3_GLOBAL_Y_BEGIN_ADDR) //188 short
#define TABLE_dct32_tab_ADDR                                (1146 + MP3_GLOBAL_Y_BEGIN_ADDR) //32 short
#define TABLE_imdct36_x_tab_ADDR                            (1178 + MP3_GLOBAL_Y_BEGIN_ADDR) //36 short
#define TABLE_sfb_48000_long_ADDR                           (1214 + MP3_GLOBAL_Y_BEGIN_ADDR) //22 short
#define TABLE_sfb_44100_long_ADDR                           (1236 + MP3_GLOBAL_Y_BEGIN_ADDR) //22 short
#define TABLE_sfb_32000_long_ADDR                           (1258 + MP3_GLOBAL_Y_BEGIN_ADDR) //22 short
#define TABLE_sfb_48000_short_ADDR                          (1280 + MP3_GLOBAL_Y_BEGIN_ADDR) //40 short
#define TABLE_sfb_44100_short_ADDR                          (1320 + MP3_GLOBAL_Y_BEGIN_ADDR) //40 short
#define TABLE_sfb_32000_short_ADDR                          (1360 + MP3_GLOBAL_Y_BEGIN_ADDR) //40 short
#define TABLE_sfb_48000_mixed_ADDR                          (1400 + MP3_GLOBAL_Y_BEGIN_ADDR) //38 short
#define TABLE_sfb_44100_mixed_ADDR                          (1438 + MP3_GLOBAL_Y_BEGIN_ADDR) //38 short
#define TABLE_sfb_32000_mixed_ADDR                          (1476 + MP3_GLOBAL_Y_BEGIN_ADDR) //38 short
#define TABLE_sfb_24000_long_ADDR                           (1514 + MP3_GLOBAL_Y_BEGIN_ADDR) //22 short
#define TABLE_sfb_22050_long_ADDR                           (1536 + MP3_GLOBAL_Y_BEGIN_ADDR) //22 short
#define TABLE_sfb_24000_short_ADDR                          (1558 + MP3_GLOBAL_Y_BEGIN_ADDR) //40 short
#define TABLE_sfb_22050_short_ADDR                          (1598 + MP3_GLOBAL_Y_BEGIN_ADDR) //40 short
#define TABLE_sfb_16000_short_ADDR                          (1638 + MP3_GLOBAL_Y_BEGIN_ADDR) //40 short
#define TABLE_sfb_24000_mixed_ADDR                          (1678 + MP3_GLOBAL_Y_BEGIN_ADDR) //36 short
#define TABLE_sfb_22050_mixed_ADDR                          (1714 + MP3_GLOBAL_Y_BEGIN_ADDR) //36 short
#define TABLE_sfb_16000_mixed_ADDR                          (1750 + MP3_GLOBAL_Y_BEGIN_ADDR) //36 short
#define TABLE_sfb_8000_long_ADDR                            (1786 + MP3_GLOBAL_Y_BEGIN_ADDR) //22 short
#define TABLE_sfb_8000_short_ADDR                           (1808 + MP3_GLOBAL_Y_BEGIN_ADDR) //40 short
#define TABLE_sfb_8000_mixed_ADDR                           (1848 + MP3_GLOBAL_Y_BEGIN_ADDR) //40 short
#define TABLE_sfbwidth_table_ADDR                           (1888 + MP3_GLOBAL_Y_BEGIN_ADDR) //28 short
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
#define FOR_DMA_4INT_ALIGNED                                (1916 + MP3_GLOBAL_Y_BEGIN_ADDR) //2 short
#define STRUCT_left_dither_error_ADDR                       (1918 + MP3_GLOBAL_Y_BEGIN_ADDR) //6 short
#define STRUCT_right_dither_error_ADDR                      (1924 + MP3_GLOBAL_Y_BEGIN_ADDR) //6 short
#define STRUCT_stats_peak_clipping_ADDR                     (1930 + MP3_GLOBAL_Y_BEGIN_ADDR) //2 short
#define STRUCT_stats_peak_sample_ADDR                       (1932 + MP3_GLOBAL_Y_BEGIN_ADDR) //2 short
//STRUCT_MAD_HEADER_ADDR
#define STRUCT_MAD_HEADER_ADDR                              (1934 + MP3_GLOBAL_Y_BEGIN_ADDR)
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
#define STRUCT_MAD_STREAM_ADDR                              (1948 + MP3_GLOBAL_Y_BEGIN_ADDR)
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
#define STRUCT_GRANULE0_BEGIN_ADDR                          (1976 + MP3_GLOBAL_Y_BEGIN_ADDR)
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
#define STRUCT_GRANULE1_BEGIN_ADDR                          (2084 + MP3_GLOBAL_Y_BEGIN_ADDR)
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
#define DEC_DMA_DATA_SINGLE                                 (114 + STRUCT_GRANULE1_BEGIN_ADDR) //2 short

//DRC_GLOBAL_STRUCT_DRC_PARAM
#define DRC_GLOBAL_STRUCT_DRC_PARAM                         (2200 + MP3_GLOBAL_Y_BEGIN_ADDR)
#define DRC_G_Y_alc_enable                                  (0 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short
#define DRC_G_Y_param_thres                                 (1 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short
#define DRC_G_Y_param_width                                 (2 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short
#define DRC_G_Y_param_R                                     (3 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short
#define DRC_G_Y_param_R1                                    (4 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short
#define DRC_G_Y_param_R2                                    (5 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short
#define DRC_G_Y_param_limit                                 (6 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short
#define DRC_G_Y_param_M                                     (7 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short
#define DRC_G_Y_param_alpha1                                (8 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short
#define DRC_G_Y_param_alpha2                                (9 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short
#define DRC_G_Y_param_noise_gate                            (10 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short
#define DRC_G_Y_param_alpha_max                             (11 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short
#define DRC_G_Y_param_Thr_dB                                (12 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short
#define DRC_G_Y_param_mm_gain                               (13 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short
#define DRC_G_Y_param_channel                               (14 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short
#define DRC_G_Y_reserved                                    (15 + DRC_GLOBAL_STRUCT_DRC_PARAM) //1 short

#define DRC_G_Y_alg_gain                                    (2216 + MP3_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DRC_G_Y_crosszero_window                            (2217 + MP3_GLOBAL_Y_BEGIN_ADDR) //1 short
#define IIR_ENABLE_FLAG_ADDR                                (2218 + MP3_GLOBAL_Y_BEGIN_ADDR) //1 short
#define IIR_ENABLE_RESERVED                                 (2219 + MP3_GLOBAL_Y_BEGIN_ADDR) //1 short
//m_IIRProcPara_ADDR
#define m_IIRProcPara_ADDR                                  (2220 + MP3_GLOBAL_Y_BEGIN_ADDR)
#define m_IIRProcPara_band0_coeffs                          (0 + m_IIRProcPara_ADDR) //6 short
#define m_IIRProcPara_band1_coeffs                          (6 + m_IIRProcPara_ADDR) //6 short
#define m_IIRProcPara_band2_coeffs                          (12 + m_IIRProcPara_ADDR) //6 short
#define m_IIRProcPara_band3_coeffs                          (18 + m_IIRProcPara_ADDR) //6 short
#define m_IIRProcPara_band4_coeffs                          (24 + m_IIRProcPara_ADDR) //6 short
#define m_IIRProcPara_band5_coeffs                          (30 + m_IIRProcPara_ADDR) //6 short
#define m_IIRProcPara_band6_coeffs                          (36 + m_IIRProcPara_ADDR) //6 short

#define STATIC_MAD_OVERLAP_ADDR                             (2262 + MP3_GLOBAL_Y_BEGIN_ADDR) //2304 short
#define GLOBAL_MAD_SYNTH_FILTER_ADDR                        (4566 + MP3_GLOBAL_Y_BEGIN_ADDR) //2048 short
#define STATIC_MAD_MAIN_DATA_ADDR                           (6614 + MP3_GLOBAL_Y_BEGIN_ADDR) //900 short
#define FOR_JEPG_DEC_RAM_Y_ALIGNED                          (7514 + MP3_GLOBAL_Y_BEGIN_ADDR) //2304 short
#define LOCAL_Y_START_ADDR                                  ((2304 + FOR_JEPG_DEC_RAM_Y_ALIGNED))
#define TABLE_offset_table_ADDR                             (FOR_JEPG_DEC_RAM_Y_ALIGNED+1600)
#define TABLE_linear_table_ADDR                             ((90 + TABLE_offset_table_ADDR))
#define TABLE_sf_table_ADDR                                 ((14 + TABLE_linear_table_ADDR))
#define TABLE_sbquant_table_ADDR                            ((64 + TABLE_sf_table_ADDR))
#define TABLE_bitalloc_table_ADDR                           ((160*2 + TABLE_sbquant_table_ADDR))
#define TABLE_qc_table_ADDR                                 ((16 + TABLE_bitalloc_table_ADDR))
#define LOCAL_AX_ADDR                                       (LOCAL_Y_START_ADDR)

/*****************
 **  MP3_LOCAL  **
 *****************/

//CII_mad_synth_frame
#define CII_mad_synth_frame_Y_BEGIN_ADDR                    (0 + MP3_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_mad_synth_frame_addr                          (0 + CII_mad_synth_frame_Y_BEGIN_ADDR) //64 short
#define LOCAL_output_addr_addr                              (64 + CII_mad_synth_frame_Y_BEGIN_ADDR) //1 short
#define LOCAL_output_raw_addr_addr                          (65 + CII_mad_synth_frame_Y_BEGIN_ADDR) //1 short

//CII_mad_layer_II
#define CII_mad_layer_II_Y_BEGIN_ADDR                       (0 + MP3_LOCAL_Y_BEGIN_ADDR)
#define layer_II_allocation_addr                            (0 + CII_mad_layer_II_Y_BEGIN_ADDR) //64 short
#define layer_II_scfsi_addr                                 (64 + CII_mad_layer_II_Y_BEGIN_ADDR) //64 short
#define layer_II_scalefactor_addr                           (128 + CII_mad_layer_II_Y_BEGIN_ADDR) //192 short
#define TABLE_bitalloc_table_ADDR_ADDR                      (320 + CII_mad_layer_II_Y_BEGIN_ADDR) //2 short
#define layer_II_BACK_JUMP1_addr                            (322 + CII_mad_layer_II_Y_BEGIN_ADDR) //1 short
#define layer_II_BACK_JUMP_addr                             (323 + CII_mad_layer_II_Y_BEGIN_ADDR) //1 short

//CII_dct32
#define CII_dct32_X_BEGIN_ADDR                              (0 + MP3_LOCAL_X_BEGIN_ADDR)
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

//CII_II_samples
#define CII_II_samples_Y_BEGIN_ADDR                         (324 + MP3_LOCAL_Y_BEGIN_ADDR)
#define layer_II_samples_addr                               (0 + CII_II_samples_Y_BEGIN_ADDR) //6 short
#define layer_II_TEMP_ADDR                                  (6 + CII_II_samples_Y_BEGIN_ADDR) //2 short
#define TABLE_sf_table_ADDR_ADDR                            (8 + CII_II_samples_Y_BEGIN_ADDR) //1 short
#define layer_II_scalefactor_ADDR_ADDR                      (9 + CII_II_samples_Y_BEGIN_ADDR) //1 short
#define layer_II_SBSAMPLE_SFTABLE_ADDR                      (10 + CII_II_samples_Y_BEGIN_ADDR) //1 short
#define layer_II_SFTABLE_ADDR                               (11 + CII_II_samples_Y_BEGIN_ADDR) //1 short
#define layer_II_bound_addr                                 (12 + CII_II_samples_Y_BEGIN_ADDR) //1 short
#define layer_II_sblimit_addr                               (13 + CII_II_samples_Y_BEGIN_ADDR) //1 short

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

//Coolsand_III_equalizer
#define Coolsand_III_equalizer_Y_BEGIN_ADDR                 (2 + MP3_LOCAL_Y_BEGIN_ADDR)
#define MP3_Tab_Equalizer_addr                              (0 + Coolsand_III_equalizer_Y_BEGIN_ADDR) //144 short
#define MP3_Tab_Equalizer_SampleFrequeyBin_Map_addr         (MP3_Tab_Equalizer_addr+11*12)

//CII_III_exponents
#define CII_III_exponents_Y_BEGIN_ADDR                      (42 + MP3_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_EXPONENTS_ADDR                                (0 + CII_III_exponents_Y_BEGIN_ADDR) //39 short

//CII_imdct36
#define CII_imdct36_X_BEGIN_ADDR                            (0 + MP3_LOCAL_X_BEGIN_ADDR)
#define LOCAL_III_imdct_s_Y_ADDR                            (LOCAL_BX_ADDR)

//CII_imdct36
#define CII_imdct36_Y_BEGIN_ADDR                            (2 + MP3_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_III_imdct36_X_16_ADDR                         ((72+LOCAL_III_decode_output_ADDR))
#define LOCAL_III_imdct36_X_4_ADDR                          ((18+LOCAL_III_imdct36_X_16_ADDR))
#define LOCAL_III_imdct36_t_16_ADDR                         ((36+LOCAL_III_imdct36_X_16_ADDR))
#define LOCAL_III_imdct36_t_4_ADDR                          ((44+LOCAL_III_imdct36_X_16_ADDR))

/******************
 **  AMR_GLOBAL  **
 ******************/

//GLOBAL_VARS
//VPP_SPEECH_DEC_IN_STRUCT
#define VPP_SPEECH_DEC_IN_STRUCT                            (0 + AMR_GLOBAL_X_BEGIN_ADDR)
#define INPUT_BITS2_ADDR                                    (0 + VPP_SPEECH_DEC_IN_STRUCT) //8 short
#define INPUT_ACTUAL_ADDR                                   (8 + VPP_SPEECH_DEC_IN_STRUCT) //48 short

//VPP_SPEECH_DEC_OUT_STRUCT
#define VPP_SPEECH_DEC_OUT_STRUCT                           (56 + AMR_GLOBAL_X_BEGIN_ADDR)
#define OUTPUT_SPEECH_BUFFER2_ADDR                          (0 + VPP_SPEECH_DEC_OUT_STRUCT) //160 short
#define OUTPUT_SPEECH_BUFFER1_ADDR                          (160 + VPP_SPEECH_DEC_OUT_STRUCT) //160 short

#define STRUCT_POST_FILTERSTATE_RES2_ADDR                   (376 + AMR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_POST_FILTERSTATE_MEM_SYN_PST_ADDR            (416 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_PREEMPHASISSTATE_ADDR                        (426 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PREEMPHASISSTATE_MEM_PRE_ADDR                (STRUCT_PREEMPHASISSTATE_ADDR)
#define STRUCT_AGCSTATE_ADDR                                (427 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_AGCSTATE_PAST_GAIN_ADDR                      (STRUCT_AGCSTATE_ADDR)
#define STRUCT_POST_FILTERSTATE_SYNTH_BUF_ADDR              (428 + AMR_GLOBAL_X_BEGIN_ADDR) //170 short
#define STRUCT_POST_PROCESSSTATE_ADDR                       (598 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_Y2_HI_ADDR                 (599 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_Y1_LO_ADDR                 (600 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_Y1_HI_ADDR                 (601 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_X0_ADDR                    (602 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_X1_ADDR                    (603 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STURCT_SPEECH_DECODE_FRAMESTATE_PREV_MODE_ADDR          (604 + AMR_GLOBAL_X_BEGIN_ADDR) //8 short
#define TX_SP_FLAG_ADDR                                     (612 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_DECOD_AMRSTATE_OLD_EXC_ADDR                  (614 + AMR_GLOBAL_X_BEGIN_ADDR) //154 short
#define STRUCT_DECOD_AMRSTATE_EXC_ADDR                      (768 + AMR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_DECOD_AMRSTATE_LSP_OLD_ADDR                  (808 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DECOD_AMRSTATE_MEM_SYN_ADDR                  (818 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DECOD_AMRSTATE_SHARP_ADDR                    (828 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR                   (829 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR                  (830 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_PREV_PDF_ADDR                 (831 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_STATE_ADDR                    (832 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_T0_LAGBUFF_ADDR               (833 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_EXCENERGYHIST_ADDR            (834 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DECOD_AMRSTATE_INBACKGROUNDNOISE_ADDR            (844 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_VOICEDHANGOVER_ADDR           (845 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_LTPGAINHISTORY_ADDR           (846 + AMR_GLOBAL_X_BEGIN_ADDR) //9 short
#define STRUCT_DECOD_AMRSTATE_NODATASEED_ADDR               (855 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_BGN_SCDSTATE_ADDR                            (856 + AMR_GLOBAL_X_BEGIN_ADDR) //60 short
#define STRUCT_BGN_SCDSTATE_FRAMEENERGYHIST_ADDR            (STRUCT_BGN_SCDSTATE_ADDR)
#define STRUCT_BGN_SCDSTATE_BGHANGOVER_ADDR                 (916 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_CB_GAIN_AVERAGESTATE_ADDR                    (918 + AMR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STRUCT_CB_GAIN_AVERAGESTATE_CBGAINHISTORY_ADDR          (STRUCT_CB_GAIN_AVERAGESTATE_ADDR)
#define STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR          (926 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_CB_GAIN_AVERAGESTATE_HANGVAR_ADDR            (927 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_LSP_AVGSTATE_ADDR                            (928 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_LSP_AVGSTATE_LSP_MEANSAVE_ADDR               (STRUCT_LSP_AVGSTATE_ADDR)
#define STRUCT_D_PLSFSTATE_ADDR                             (938 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR                  (STRUCT_D_PLSFSTATE_ADDR)
#define STRUCT_D_PLSFSTATE_PAST_R_Q_ADDR                    (948 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_EC_GAIN_PITCHSTATE_ADDR                      (958 + AMR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_EC_GAIN_PITCHSTATE_PBUF_ADDR                 (STRUCT_EC_GAIN_PITCHSTATE_ADDR)
#define STRUCT_EC_GAIN_PITCHSTATE_PAST_GAIN_PIT_ADDR            (964 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_EC_GAIN_PITCHSTATE_PREV_GP_ADDR              (965 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_EC_GAIN_CODESTATE_ADDR                       (966 + AMR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_EC_GAIN_CODESTATE_GBUF_ADDR                  (STRUCT_EC_GAIN_CODESTATE_ADDR)
#define STRUCT_EC_GAIN_CODESTATE_PAST_GAIN_CODE_ADDR            (972 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_EC_GAIN_CODESTATE_PREV_GC_ADDR               (973 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GC_PREDSTATE_ADDR                            (974 + AMR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR                (STRUCT_GC_PREDSTATE_ADDR)
#define STRUCT_GC_PREDSTATE_PAST_QUA_EN_MR122_ADDR          (978 + AMR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_PH_DISPSTATE_ADDR                            (982 + AMR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_PH_DISPSTATE_GAINMEM_ADDR                    (STRUCT_PH_DISPSTATE_ADDR)
#define STRUCT_PH_DISPSTATE_PREVSTATE_ADDR                  (988 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PH_DISPSTATE_PREVCBGAIN_ADDR                 (989 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PH_DISPSTATE_LOCKFULL_ADDR                   (990 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PH_DISPSTATE_ONSET_ADDR                      (991 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_ADDR                            (992 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_SINCE_LAST_SID_ADDR             (STRUCT_DTX_DECSTATE_ADDR)
#define STRUCT_DTX_DECSTATE_TRUE_SID_PERIOD_INV_ADDR            (993 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
//STRUCT_AMR_DEC_INPUT_ADDR
#define STRUCT_AMR_DEC_INPUT_ADDR                           (994 + AMR_GLOBAL_X_BEGIN_ADDR)
#define STRUCT_AMR_DEC_INPUT_mode_ADDR                      (0 + STRUCT_AMR_DEC_INPUT_ADDR) //1 short
#define STRUCT_AMR_DEC_INPUT_reset_ADDR                     (1 + STRUCT_AMR_DEC_INPUT_ADDR) //1 short
#define STRUCT_AMR_DEC_INBUF_ADDR                           (2 + STRUCT_AMR_DEC_INPUT_ADDR) //2 short
#define STRUCT_AMR_DEC_OUTBUF_ADDR                          (4 + STRUCT_AMR_DEC_INPUT_ADDR) //2 short
#define AMR_RESERVE_blank1                                  (6 + STRUCT_AMR_DEC_INPUT_ADDR) //8 short

#define STRUCT_DTX_DECSTATE_LSP_OLD_ADDR                    (1008 + AMR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DTX_DECSTATE_LSF_HIST_ADDR                   (1018 + AMR_GLOBAL_X_BEGIN_ADDR) //80 short
#define STRUCT_DTX_DECSTATE_LSF_HIST_PTR_ADDR               (1098 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_LOG_PG_MEAN_ADDR                (1099 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_LOG_EN_HIST_ADDR                (1100 + AMR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STRUCT_DTX_DECSTATE_LOG_EN_HIST_PTR_ADDR            (1108 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_LOG_EN_ADJUST_ADDR              (1109 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DTXHANGOVERCOUNT_ADDR           (1110 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DECANAELAPSEDCOUNT_ADDR         (1111 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_SID_FRAME_ADDR                  (1112 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_VALID_DATA_ADDR                 (1113 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DTXHANGOVERADDED_ADDR           (1114 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DTXGLOBALSTATE_ADDR             (1115 + AMR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DATA_UPDATED_ADDR               (1116 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short
#define AMR_RESERVE_blank2                                  (1118 + AMR_GLOBAL_X_BEGIN_ADDR) //2 short

//GLOBAL_VARS
#define GLOBAL_RELOAD_MODE_ADDR                             (0 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RESET_FLAG_ADDR                              (1 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RESET_ADDR                                   (2 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DEC_RESET_DONE_ADDR                          (3 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RELOAD_RESET_FLAG_ADDR                       (4 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_BFI_ADDR                                     (5 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_UFI_ADDR                                     (6 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_SID_ADDR                                     (7 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_TAF_ADDR                                     (8 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DEC_MODE_ADDR                                (9 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ENC_MODE_ADDR                                (10 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ENC_USED_MODE_ADDR                           (11 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_RESET_FLAG_ADDR                                 (12 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_MAIN_RESET_FLAG_OLD_ADDR                        (13 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_AMR_FRAME_TYPE_ADDR                             (14 + AMR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define DEC_INPUT_ADDR_ADDR                                 (16 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_OUTPUT_ADDR_ADDR                                (17 + AMR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_INOUT_ADDR_BAK_ADDR                             (18 + AMR_GLOBAL_Y_BEGIN_ADDR) //2 short
//GLOBAL_OUTPUT_STRUCT_ADDR
#define GLOBAL_OUTPUT_STRUCT_ADDR                           (20 + AMR_GLOBAL_Y_BEGIN_ADDR)
#define STRUCT_AMR_DEC_mode_ADDR                            (0 + GLOBAL_OUTPUT_STRUCT_ADDR) //1 short
#define STRUCT_AMR_DEC_CONSUME_ADDR                         (1 + GLOBAL_OUTPUT_STRUCT_ADDR) //1 short
#define STRUCT_AMR_DEC_length_ADDR                          (2 + GLOBAL_OUTPUT_STRUCT_ADDR) //1 short

#define AMRJPEG_CONST_BLANK                                 (23 + AMR_GLOBAL_Y_BEGIN_ADDR) //927 short
//Short_Constant
#define Short_Constant                                      (950 + AMR_GLOBAL_Y_BEGIN_ADDR)
#define CONST_AMR_0x1fff_ADDR                               (0 + Short_Constant) //1 short
#define CONST_AMR_5462_ADDR                                 (1 + Short_Constant) //1 short
#define CONST_AMR_0x3fff_ADDR                               (2 + Short_Constant) //1 short
#define CONST_AMR_0x0800_ADDR                               (3 + Short_Constant) //1 short
#define CONST_AMR_21299_ADDR                                (4 + Short_Constant) //1 short
#define CONST_AMR_5443_ADDR                                 (5 + Short_Constant) //1 short
#define CONST_AMR_112_ADDR                                  (6 + Short_Constant) //1 short
#define CONST_AMR_105_ADDR                                  (7 + Short_Constant) //1 short
#define CONST_AMR_197_ADDR                                  (8 + Short_Constant) //1 short
#define CONST_AMR_31128_ADDR                                (9 + Short_Constant) //1 short
#define CONST_AMR_134_ADDR                                  (10 + Short_Constant) //1 short
#define CONST_AMR_0x177_ADDR                                (11 + Short_Constant) //1 short
#define CONST_AMR_102_ADDR                                  (12 + Short_Constant) //1 short
#define CONST_AMR_3277_ADDR                                 (13 + Short_Constant) //1 short
#define CONST_AMR_0x87_ADDR                                 (14 + Short_Constant) //1 short
#define CONST_AMR_NEG11_ADDR                                (15 + Short_Constant) //1 short
#define CONST_AMR_31821_ADDR                                (16 + Short_Constant) //1 short
#define CONST_AMR_29491_ADDR                                (17 + Short_Constant) //1 short
#define CONST_AMR_9830_ADDR                                 (18 + Short_Constant) //1 short
#define CONST_AMR_14746_ADDR                                (19 + Short_Constant) //1 short
#define CONST_AMR_17578_ADDR                                (20 + Short_Constant) //1 short
#define CONST_AMR_16384_ADDR                                (21 + Short_Constant) //1 short
#define CONST_AMR_256_ADDR                                  (22 + Short_Constant) //1 short
#define CONST_AMR_160_ADDR                                  (23 + Short_Constant) //1 short
#define CONST_AMR_143_ADDR                                  (24 + Short_Constant) //1 short
#define CONST_AMR_26214_ADDR                                (25 + Short_Constant) //1 short
#define CONST_AMR_4096_ADDR                                 (26 + Short_Constant) //1 short
#define CONST_AMR_64_ADDR                                   (27 + Short_Constant) //1 short
#define CONST_AMR_128_ADDR                                  (28 + Short_Constant) //1 short
#define CONST_AMR_205_ADDR                                  (29 + Short_Constant) //1 short
#define CONST_AMR_50_ADDR                                   (30 + Short_Constant) //1 short
#define CONST_AMR_44_ADDR                                   (31 + Short_Constant) //1 short
#define CONST_AMR_2_ADDR                                    (32 + Short_Constant) //1 short
#define CONST_AMR_3_ADDR                                    (33 + Short_Constant) //1 short
#define CONST_AMR_4_ADDR                                    (34 + Short_Constant) //1 short
#define CONST_AMR_6_ADDR                                    (35 + Short_Constant) //1 short
#define CONST_AMR_7_ADDR                                    (36 + Short_Constant) //1 short
#define CONST_AMR_8_ADDR                                    (37 + Short_Constant) //1 short
#define CONST_AMR_9_ADDR                                    (38 + Short_Constant) //1 short
#define CONST_AMR_10_ADDR                                   (39 + Short_Constant) //1 short
#define CONST_AMR_12_ADDR                                   (40 + Short_Constant) //1 short
#define CONST_AMR_16_ADDR                                   (41 + Short_Constant) //1 short
#define CONST_AMR_17_ADDR                                   (42 + Short_Constant) //1 short
#define CONST_AMR_18_ADDR                                   (43 + Short_Constant) //1 short
#define CONST_AMR_20_ADDR                                   (44 + Short_Constant) //1 short
#define CONST_AMR_22_ADDR                                   (45 + Short_Constant) //1 short
#define CONST_AMR_25_ADDR                                   (46 + Short_Constant) //1 short
#define CONST_AMR_30_ADDR                                   (47 + Short_Constant) //1 short
#define CONST_AMR_31_ADDR                                   (48 + Short_Constant) //1 short
#define CONST_AMR_32_ADDR                                   (49 + Short_Constant) //1 short
#define CONST_AMR_33_ADDR                                   (50 + Short_Constant) //1 short
#define CONST_AMR_40_ADDR                                   (51 + Short_Constant) //1 short
#define CONST_AMR_41_ADDR                                   (52 + Short_Constant) //1 short
#define CONST_AMR_15_ADDR                                   (53 + Short_Constant) //1 short
#define CONST_AMR_11_ADDR                                   (54 + Short_Constant) //1 short
#define CONST_AMR_3276_ADDR                                 (55 + Short_Constant) //1 short
#define CONST_AMR_0x2000_ADDR                               (56 + Short_Constant) //1 short
#define CONST_AMR_0x2666_ADDR                               (57 + Short_Constant) //1 short
#define CONST_AMR_neg2_ADDR                                 (58 + Short_Constant) //1 short
#define CONST_AMR_80_ADDR                                   (59 + Short_Constant) //1 short
#define CONST_AMR_512_ADDR                                  (60 + Short_Constant) //1 short
#define CONST_AMR_1024_ADDR                                 (61 + Short_Constant) //1 short
#define CONST_AMR_0x199a_ADDR                               (62 + Short_Constant) //1 short
#define CONST_AMR_120_ADDR                                  (63 + Short_Constant) //1 short
#define CONST_AMR_124_ADDR                                  (64 + Short_Constant) //1 short
#define CONST_AMR_1311_ADDR                                 (65 + Short_Constant) //1 short
#define CONST_AMR_368_ADDR                                  (66 + Short_Constant) //1 short
#define CONST_AMR_463_ADDR                                  (67 + Short_Constant) //1 short
#define CONST_AMR_9000_ADDR                                 (68 + Short_Constant) //1 short
#define CONST_AMR_5325_ADDR                                 (69 + Short_Constant) //1 short

//Long_Constant
#define Long_Constant                                       (1020 + AMR_GLOBAL_Y_BEGIN_ADDR)
#define CONST_AMR_0_ADDR                                    (0 + Long_Constant) //2 short
#define CONST_AMR_1_ADDR                                    (2 + Long_Constant) //2 short
#define CONST_AMR_0x40000000_ADDR                           (4 + Long_Constant) //2 short
#define CONST_AMR_0x7FFFFFFF_ADDR                           (6 + Long_Constant) //2 short
#define CONST_AMR_0x70816958_ADDR                           (8 + Long_Constant) //2 short
#define CONST_AMR_0x00010001_ADDR                           (10 + Long_Constant) //2 short
#define CONST_AMR_0x00020002_ADDR                           (12 + Long_Constant) //2 short
#define CONST_AMR_0x00080008_ADDR                           (14 + Long_Constant) //2 short
#define CONST_AMR_0xFFFFFFFF_ADDR                           (16 + Long_Constant) //2 short
#define CONST_AMR_0x00004000L_ADDR                          (18 + Long_Constant) //2 short
#define CONST_AMR_0x00007FFF_ADDR                           (20 + Long_Constant) //2 short
#define CONST_AMR_0x00008000_ADDR                           (22 + Long_Constant) //2 short
#define CONST_AMR_0x0000FFFF_ADDR                           (24 + Long_Constant) //2 short
#define CONST_AMR_0x00000005L_ADDR                          (26 + Long_Constant) //2 short
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

//EFR_TABLE_START
#define EFR_TABLE_START                                     (1048 + AMR_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_MEAN_LSF_ADDR                                 (0 + EFR_TABLE_START) //10 short
#define TABLE_SLOPE_ADDR                                    (10 + EFR_TABLE_START) //64 short
#define TABLE_LSP_LSF_ADDR                                  (74 + EFR_TABLE_START) //66 short
#define TABLE_LOG2_ADDR                                     (140 + EFR_TABLE_START) //34 short
#define TABLE_INV_SQRT_ADDR                                 (174 + EFR_TABLE_START) //50 short
#define TABLE_POW2_ADDR                                     (224 + EFR_TABLE_START) //34 short
#define STATIC_CONST_DHF_MASK_ADDR                          (258 + EFR_TABLE_START) //58 short
#define STATIC_CONST_QUA_GAIN_PITCH_ADDR                    (316 + EFR_TABLE_START) //16 short
#define STATIC_CONST_F_GAMMA4_ADDR                          (332 + EFR_TABLE_START) //10 short
#define STATIC_CONST_F_GAMMA3_ADDR                          (342 + EFR_TABLE_START) //10 short
#define STATIC_CONST_INTER_6_ADDR                           (352 + EFR_TABLE_START) //62 short
#define STATIC_CONST_DGRAY_ADDR                             (414 + EFR_TABLE_START) //8 short
#define TABLE_DICO1_LSF_ADDR                                (422 + EFR_TABLE_START) //512 short
#define TABLE_DICO2_LSF_ADDR                                (934 + EFR_TABLE_START) //1024 short
#define TABLE_DICO3_LSF_ADDR                                (1958 + EFR_TABLE_START) //1024 short
#define TABLE_DICO4_LSF_ADDR                                (2982 + EFR_TABLE_START) //1024 short
#define TABLE_DICO5_LSF_ADDR                                (4006 + EFR_TABLE_START) //256 short
#define STATIC_CONST_BITNO_ADDR                             (4262 + EFR_TABLE_START) //30 short

//AMR_TABLE_START
#define AMR_TABLE_START                                     (5340 + AMR_GLOBAL_Y_BEGIN_ADDR)
#define STATIC_CONST_PRMNO_ADDR                             (0 + AMR_TABLE_START) //10 short
#define STATIC_CONST_PRMNOFSF_ADDR                          (10 + AMR_TABLE_START) //8 short
#define STATIC_CONST_bitno_MR475_ADDR                       (18 + AMR_TABLE_START) //9 short
#define STATIC_CONST_bitno_MR515_ADDR                       (27 + AMR_TABLE_START) //10 short
#define STATIC_CONST_bitno_MR59_ADDR                        (37 + AMR_TABLE_START) //10 short
#define STATIC_CONST_bitno_MR67_ADDR                        (47 + AMR_TABLE_START) //10 short
#define STATIC_CONST_bitno_MR74_ADDR                        (57 + AMR_TABLE_START) //10 short
#define STATIC_CONST_bitno_MR795_ADDR                       (67 + AMR_TABLE_START) //12 short
#define STATIC_CONST_bitno_MR102_ADDR                       (79 + AMR_TABLE_START) //20 short
#define STATIC_CONST_bitno_MRDTX_ADDR                       (99 + AMR_TABLE_START) //3 short
#define STATIC_CONST_BITNO_AMR_ADDR                         (102 + AMR_TABLE_START) //10 short
#define STATIC_CONST_startPos_ADDR                          (112 + AMR_TABLE_START) //16 short
#define STATIC_CONST_dhf_MR475_ADDR                         (128 + AMR_TABLE_START) //18 short
#define STATIC_CONST_dhf_MR515_ADDR                         (146 + AMR_TABLE_START) //20 short
#define STATIC_CONST_dhf_MR59_ADDR                          (166 + AMR_TABLE_START) //20 short
#define STATIC_CONST_dhf_MR67_ADDR                          (186 + AMR_TABLE_START) //20 short
#define STATIC_CONST_dhf_MR74_ADDR                          (206 + AMR_TABLE_START) //20 short
#define STATIC_CONST_dhf_MR795_ADDR                         (226 + AMR_TABLE_START) //24 short
#define STATIC_CONST_dhf_MR102_ADDR                         (250 + AMR_TABLE_START) //40 short
#define STATIC_CONST_dhf_MR122_ADDR                         (290 + AMR_TABLE_START) //58 short
#define STATIC_CONST_dhf_amr_ADDR                           (348 + AMR_TABLE_START) //8 short
#define STATIC_CONST_qua_gain_code_ADDR                     (356 + AMR_TABLE_START) //96 short
#define STATIC_CONST_lsp_init_data_ADDR                     (452 + AMR_TABLE_START) //10 short
#define STATIC_past_rq_init_ADDR                            (462 + AMR_TABLE_START) //80 short
#define TABLE_SQRT_L_ADDR                                   (542 + AMR_TABLE_START) //50 short
#define STATIC_CONST_WIND_200_40_ADDR                       (592 + AMR_TABLE_START) //240 short
#define STATIC_trackTable_ADDR                              (832 + AMR_TABLE_START) //20 short
#define STATIC_CONST_lsf_hist_mean_scale_ADDR               (852 + AMR_TABLE_START) //10 short
#define STATIC_CONST_dtx_log_en_adjust_ADDR                 (862 + AMR_TABLE_START) //10 short
#define STATIC_CONST_pred_ADDR                              (872 + AMR_TABLE_START) //4 short
#define STATIC_CONST_pred_MR122_ADDR                        (876 + AMR_TABLE_START) //4 short
#define STATIC_CONST_b_60Hz_ADDR                            (880 + AMR_TABLE_START) //4 short
#define STATIC_CONST_a_60Hz_ADDR                            (884 + AMR_TABLE_START) //4 short
#define STATIC_CONST_gamma3_ADDR                            (888 + AMR_TABLE_START) //10 short
#define AMR_RESET_VALUE_TABLE_ADDR                          (898 + AMR_TABLE_START) //48 short

#define GLOBAL_OUT244_ADDR                                  (6286 + AMR_GLOBAL_Y_BEGIN_ADDR) //244 short
#define CONST_TAB244_ADDR                                   (GLOBAL_OUT244_ADDR)
#define TEMP_MYRING_ADDR                                    (6530 + AMR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define CONST_HEAD_RING                                     (TEMP_MYRING_ADDR)

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

/*******************
 **  JPEG_GLOBAL  **
 *******************/

//GLOBAL_VARS
#define JEPG_DEC_FOR_MP3_AMR_RAM_X_ALIGNED                  (0 + JPEG_GLOBAL_X_BEGIN_ADDR) //3244 short
//JPEG_GLOBAL_X_VAR
#define JPEG_GLOBAL_X_VAR                                   (3244 + JPEG_GLOBAL_X_BEGIN_ADDR)
#define GLOBAL_MCUBuffer_ADDR                               (0 + JPEG_GLOBAL_X_VAR) //640 short
#define GLOBAL_MCU_lastcoef_ADDR                            (640 + JPEG_GLOBAL_X_VAR) //10 short
#define GLOBAL_ycoef_ADDR                                   (650 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_ucoef_ADDR                                   (651 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_vcoef_ADDR                                   (652 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_coef_ADDR                                    (653 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_image_control_ADDR                           (654 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_lpMCUBuffer_ADDR_ADDR                        (655 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_lastcoef_pt_ADDR                             (656 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_temp_lpJpegBuf_ADDR                          (657 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_Y_data_ADDR                                  (658 + JPEG_GLOBAL_X_VAR) //256 short
#define GLOBAL_InnerY_ADDR                                  (914 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_InnerU_ADDR                                  (915 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_InnerV_ADDR                                  (916 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_Reserved1_ADDR                               (917 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_DMA_Y_SIZE_ADDR                              (918 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_DMA_UV_SIZE_ADDR                             (919 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_DMA_Y_INC_ADDR                               (920 + JPEG_GLOBAL_X_VAR) //2 short
#define GLOBAL_DMA_UV_INC_ADDR                              (922 + JPEG_GLOBAL_X_VAR) //2 short
#define GLOBAL_rgb_buf_ADDR                                 (924 + JPEG_GLOBAL_X_VAR) //4800 short
#define GLOBAL_Output_U_ADDR                                (GLOBAL_rgb_buf_ADDR + 3200)
#define GLOBAL_Output_V_ADDR                                (GLOBAL_Output_U_ADDR + 800)
#define GLOBAL_temp_Y_data_ADDR                             (5724 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_col_num_ADDR                                 (5725 + JPEG_GLOBAL_X_VAR) //1 short
#define TABLE_qt_table_ADDR                                 (5726 + JPEG_GLOBAL_X_VAR) //196 short
#define GLOBAL_IDCT_BUFFER_X_ADDR                           (5922 + JPEG_GLOBAL_X_VAR) //8 short
#define GLOBAL_row_even_ADDR                                (5930 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_col_even_ADDR                                (5931 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_block_id_ADDR                                (5932 + JPEG_GLOBAL_X_VAR) //2 short
#define GLOBAL_JPEG_IMAGECONTROL_ADDR                       (5934 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_ImgWidth_ADDR                                (5935 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_ImgHeight_ADDR                               (5936 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_Temp_ImgHeight_ADDR                          (5937 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_temp_ImgWidth_ADDR                           (5938 + JPEG_GLOBAL_X_VAR) //1 short
#define GLOBAL_Temp_ImgWidth_ADDR                           (5939 + JPEG_GLOBAL_X_VAR) //1 short


//GLOBAL_VARS
#define JEPG_DEC_FOR_MP3_AMR_RAM_Y_ALIGNED                  (0 + JPEG_GLOBAL_Y_BEGIN_ADDR) //7456 short
//JPEG_Short_Constant
#define JPEG_Short_Constant                                 (7456 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define CONST_JPEG_DEC_56_ADDR                              (0 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0xdbff_ADDR                          (1 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0xc0ff_ADDR                          (2 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0xc4ff_ADDR                          (3 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0xdaff_ADDR                          (4 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0x00c4_ADDR                          (5 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_1448_ADDR                            (6 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_63_ADDR                              (7 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_784_ADDR                             (8 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0xFF_ADDR                            (9 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0xef_ADDR                            (10 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0xF0_ADDR                            (11 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0x3ff_ADDR                           (12 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0x3df_ADDR                           (13 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0x3e0_ADDR                           (14 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_464_ADDR                             (15 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0xfd7_ADDR                           (16 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0x1cf_ADDR                           (17 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_4056_ADDR                            (18 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0x1bf_ADDR                           (19 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_448_ADDR                             (20 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_2676_ADDR                            (21 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0xFF00_ADDR                          (22 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0x001f_ADDR                          (23 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0x07ff_ADDR                          (24 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_152_ADDR                             (25 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_176_ADDR                             (26 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_320_ADDR                             (27 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_reserve1_ADDR                        (28 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_367_ADDR                             (29 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_159_ADDR                             (30 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_220_ADDR                             (31 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_411_ADDR                             (32 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_29_ADDR                              (33 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_reserve2_ADDR                        (34 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_565_ADDR                             (35 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_3406_ADDR                            (36 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_2276_ADDR                            (37 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_2408_ADDR                            (38 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_799_ADDR                             (39 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_4017_ADDR                            (40 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_1108_ADDR                            (41 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_3784_ADDR                            (42 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_1568_ADDR                            (43 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_181_ADDR                             (44 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0x3f_ADDR                            (45 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_0_ADDR                               (46 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_1_ADDR                               (47 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_2_ADDR                               (48 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_3_ADDR                               (49 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_4_ADDR                               (50 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_5_ADDR                               (51 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_6_ADDR                               (52 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_7_ADDR                               (53 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_8_ADDR                               (54 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_10_ADDR                              (55 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_15_ADDR                              (56 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_16_ADDR                              (57 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_18_ADDR                              (58 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_31_ADDR                              (59 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_32_ADDR                              (60 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_64_ADDR                              (61 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_80_ADDR                              (62 + JPEG_Short_Constant) //1 short
#define CONST_JPEG_DEC_128_ADDR                             (63 + JPEG_Short_Constant) //1 short

//JPEG_Long_Constant
#define JPEG_Long_Constant                                  (7520 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define CONST_JPEG_DEC_0xFFFFFFFF_ADDR                      (0 + JPEG_Long_Constant) //2 short
#define CONST_JPEG_DEC_0x00000020_ADDR                      (2 + JPEG_Long_Constant) //2 short
#define CONST_JPEG_DEC_0x00000080_ADDR                      (4 + JPEG_Long_Constant) //2 short
#define CONST_JPEG_DEC_0x00000040_ADDR                      (6 + JPEG_Long_Constant) //2 short
#define CONST_JPEG_DEC_0x00002000_ADDR                      (8 + JPEG_Long_Constant) //2 short
#define CONST_JPEG_DEC_0x0000ffff_ADDR                      (10 + JPEG_Long_Constant) //2 short
#define CONST_JPEG_DEC_0x0000ff7f_ADDR                      (12 + JPEG_Long_Constant) //2 short
#define CONST_JPEG_DEC_0x0000ff80_ADDR                      (14 + JPEG_Long_Constant) //2 short

//TABLE_DClumtab
#define TABLE_DClumtab                                      (7536 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_DClumtab0_ADDR                                (0 + TABLE_DClumtab) //16 short
#define TABLE_DClumtab1_ADDR                                (16 + TABLE_DClumtab) //16 short

//TABLE_DCchromtab
#define TABLE_DCchromtab                                    (7568 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_DCchromtab0_ADDR                              (0 + TABLE_DCchromtab) //32 short
#define TABLE_DCchromtab1_ADDR                              (32 + TABLE_DCchromtab) //32 short

//TABLE_AClumtab
#define TABLE_AClumtab                                      (7632 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_AClumtab0_ADDR                                (0 + TABLE_AClumtab) //64 short
#define TABLE_AClumtab1_ADDR                                (64 + TABLE_AClumtab) //96 short
#define TABLE_AClumtab2_ADDR                                (160 + TABLE_AClumtab) //64 short
#define TABLE_AClumtab3_ADDR                                (224 + TABLE_AClumtab) //256 short

//TABLE_ACchromtab
#define TABLE_ACchromtab                                    (8112 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_ACchromtab0_ADDR                              (0 + TABLE_ACchromtab) //64 short
#define TABLE_ACchromtab1_ADDR                              (64 + TABLE_ACchromtab) //128 short
#define TABLE_ACchromtab2_ADDR                              (192 + TABLE_ACchromtab) //64 short
#define TABLE_ACchromtab3_ADDR                              (256 + TABLE_ACchromtab) //256 short

//TABLE_OtherConstant
#define TABLE_OtherConstant                                 (8624 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_inverse_Zig_Zag_ADDR                          (0 + TABLE_OtherConstant) //64 short
#define TABLE_add_para_ADDR                                 (64 + TABLE_OtherConstant) //320 short
#define GLOBAL_IDCT_BUFFER_Y_ADDR                           (384 + TABLE_OtherConstant) //8 short

//JPEG_GLOBAL_Y_VAR
#define JPEG_GLOBAL_Y_VAR                                   (9016 + JPEG_GLOBAL_Y_BEGIN_ADDR)
#define GLOBAL_lpJpegBuf_ADDR                               (0 + JPEG_GLOBAL_Y_VAR) //448 short
#define GLOBAL_output_buf_ADDR                              (448 + JPEG_GLOBAL_Y_VAR) //640 short


/******************
 **  JPEG_LOCAL  **
 ******************/

/***********************
 **  H263zoom_GLOBAL  **
 ***********************/

//GLOBAL_VARS
#define H263_ZOOM_FOR_MP3_AMR_RAM_X_ALIGNED                 (0 + H263zoom_GLOBAL_X_BEGIN_ADDR) //3244 short
#define CONST_TabV2R_ADDR                                   (3244 + H263zoom_GLOBAL_X_BEGIN_ADDR) //256 short
#define CONST_TableV2G_ADDR                                 (3500 + H263zoom_GLOBAL_X_BEGIN_ADDR) //256 short
#define CONST_TabU2G_ADDR                                   (3756 + H263zoom_GLOBAL_X_BEGIN_ADDR) //256 short
#define CONST_TabU2B_ADDR                                   (4012 + H263zoom_GLOBAL_X_BEGIN_ADDR) //256 short
#define CONST_H263_1_ADDR                                   (CONST_TableV2G_ADDR+2)
#define CONST_H263_2_ADDR                                   (CONST_TableV2G_ADDR+3)
#define GLOBAL_TEMP_PTMP2_ADDR                              (4268 + H263zoom_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_TEMP_PTMP3_ADDR                              (4269 + H263zoom_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_TEMP_PTMP4_ADDR                              (4270 + H263zoom_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_BUF_OUT_ADDR                                 (4271 + H263zoom_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_TEMP_BUF_ADDR                                ((GLOBAL_BUF_OUT_ADDR+1))
#define GLOBAL_OUTPUT_YUV2RGB_ADDR                          (4272 + H263zoom_GLOBAL_X_BEGIN_ADDR) //4000 short

//GLOBAL_VARS
#define H263_ZOOM_FOR_MP3_AMR_RAM_Y_ALIGNED                 (0 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //7456 short
#define GLOBAL_INLINE_NUM_1_ADDR                            (7456 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_INLINE_NUM_2_ADDR                            (7457 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define TEMP_ADDRY_ADDR                                     (7458 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define TEMP_ADDRU_ADDR                                     (7459 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define TEMP_ADDRV_ADDR                                     (7460 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_BUFIN_Y_ADDR                                 (7461 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_BUFIN_U_ADDR                                 (7462 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_BUFIN_V_ADDR                                 (7463 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_SRCW_HALF_ADDR                               (7464 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_SRCW_QUA_ADDR                                (7465 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_NUM1_ADDR                                    (7466 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_NUM3_ADDR                                    (7467 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_MASK_ADDR                                    (7468 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_DMAOUT_SIZE_ADDR                             (7470 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DEAD_OUT_LINE_ADDR                           (7471 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DMAOUT_INC_ADDR                              (7472 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_INLINE_COUNT_ADDR                            (7474 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_INLINE_COUNT_UV_ADDR                         (7475 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DMAIN_SIZE_ADDR                              (7476 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_DMAIN_SIZE_UV_ADDR                           (7478 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_SCALEX_SIZE_ADDR                             (7480 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_SCALEY_SIZE_ADDR                             (7482 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_P1_ADDR                                      (7484 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_P2_ADDR                                      (7485 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOABL_N_ADDR                                       (7486 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RESERVED_ADDR                                (7487 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_OUTLINE_ADDR                                 (7488 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ROTATE_POINTER_ADDR                          (7489 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_BUF_IN_ADDR                                  (7490 + H263zoom_GLOBAL_Y_BEGIN_ADDR) //2400 short

/**********************
 **  H263zoom_LOCAL  **
 **********************/

//Coolsand_Set_RotatePara
#define Coolsand_Set_RotatePara_Y_BEGIN_ADDR                (0 + H263zoom_LOCAL_Y_BEGIN_ADDR)
#define TMP_BUF_FOR_ROTATE                                  (0 + Coolsand_Set_RotatePara_Y_BEGIN_ADDR) //32 short
#define TMP_BUF_FOR_ROTATE2                                 (32 + Coolsand_Set_RotatePara_Y_BEGIN_ADDR) //32 short

/*********************
 **  SBCENC_GLOBAL  **
 *********************/

//GLOBAL_VARS
#define SBCENC_FOR_MP3_AMR_RAM_X_ALIGNED                    (0 + SBCENC_GLOBAL_X_BEGIN_ADDR) //5548 short
#define SMALL_FILTER_IMP_ADDR                               (5548 + SBCENC_GLOBAL_X_BEGIN_ADDR) //1536 short
#define Const_anamatrix8                                    (7084 + SBCENC_GLOBAL_X_BEGIN_ADDR) //8 short
#define Const_crc_table                                     (7092 + SBCENC_GLOBAL_X_BEGIN_ADDR) //128 short
#define Const_sbc_offset8                                   (7220 + SBCENC_GLOBAL_X_BEGIN_ADDR) //32 short
#define Const_sbc_proto_8                                   (7252 + SBCENC_GLOBAL_X_BEGIN_ADDR) //70 short
#define Const_in_index                                      (7322 + SBCENC_GLOBAL_X_BEGIN_ADDR) //74 short
#define Const_t_index                                       (7396 + SBCENC_GLOBAL_X_BEGIN_ADDR) //8 short
#define CONST_SBC_32bit_1_ADDR                              ((SMALL_FILTER_IMP_ADDR+1418))
#define CONST_SBC_1_ADDR                                    ((Const_in_index+9))
#define CONST_SBC_2_ADDR                                    ((Const_t_index+4))
#define CONST_SBC_16_ADDR                                   ((Const_in_index+2))
#define CONST_NEG_16_ADDR                                   ((SMALL_FILTER_IMP_ADDR+1129))

//GLOBAL_VARS
#define SBCENC_FOR_MP3_AMR_RAM_Y_ALIGNED                    (0 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //7524 short
#define STRUCT_sbc_frame_channels                           (Output_nbChannel_addr)
#define SMALL_FILTER_IMPD_ADDR                              (7524 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //1536 short
#define CONST_Pmask_ADDR                                    (9060 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //2 short
#define CONST_D_Npc_ADDR                                    (9062 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //2 short
#define CONST_SBC_reseverd2_ADDR                            (9064 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_OUTPUT_ADDR_ADDR                             (9068 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define CONST_SBC_reseverd_ADDR                             (9069 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STRUCT_sbc_INPUT_ADDR                               (9070 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STRUCT_sbc_INPUT_END_ADDR                           (9071 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Tmp_crc_pos_addr                                    (9072 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Tmp_produced_addr                                   (9073 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_sbc_encoder_state_position                   (9074 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_sbc_encoder_state_position1                  ((GLOBAL_sbc_encoder_state_position+1))
#define GLOBAL_sbc_encoder_state_X                          (9076 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //640 short
#define GLOBAL_sbc_encoder_state_X_ch1                      ((GLOBAL_sbc_encoder_state_X+320))
#define RESAMPLE_TOTAL_LEN_ADDR                             (9716 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define RESAMPLE_USED_LEN_ADDR                              (9717 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define RESAMPLE_VPTR_ADDR                                  (9718 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define RESAMPLE_FLAG_ADDR                                  (9719 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define RESAMPLE_INC_ADDR                                   (9720 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //2 short
#define RESAMPLE_OFS_ADDR                                   (9722 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //2 short
#define RESAMPLE_SRC_TMP_ADDR                               (9724 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //264 short
#define RESAMPLE_X1_ADDR                                    (RESAMPLE_SRC_TMP_ADDR)
#define RESAMPLE_X2_ADDR                                    ((RESAMPLE_SRC_TMP_ADDR+128+4))
#define RESAMPLE_Nx_ADDR                                    (9988 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define RESAMPLE_BuffSize_ADDR                              (9989 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //1 short
#define LOCAL_SrcUp_dtb_ADDR                                (9990 + SBCENC_GLOBAL_Y_BEGIN_ADDR) //2 short

/********************
 **  SBCENC_LOCAL  **
 ********************/

//CS_sbc_encode
#define CS_sbc_encode_X_BEGIN_ADDR                          (0 + SBCENC_LOCAL_X_BEGIN_ADDR)
#define SBC_sb_sample_f                                     (0 + CS_sbc_encode_X_BEGIN_ADDR) //512 short
#define SBC_sb_sample_f_ch1                                 ((SBC_sb_sample_f+16))
#define Out_put_addr                                        (512 + CS_sbc_encode_X_BEGIN_ADDR) //1000 short
#define Out_put_addr_1                                      ((Out_put_addr+1))

//CS_ResampleAudio_SBCEnc
#define CS_ResampleAudio_SBCEnc_X_BEGIN_ADDR                (1512 + SBCENC_LOCAL_X_BEGIN_ADDR)
#define LOCAL_SrcUp_endTime_ADDR                            (0 + CS_ResampleAudio_SBCEnc_X_BEGIN_ADDR) //2 short

//CS_sbc_analyze_audio
#define CS_sbc_analyze_audio_X_BEGIN_ADDR                   (1514 + SBCENC_LOCAL_X_BEGIN_ADDR)
#define SBC_pcm_sample                                      (0 + CS_sbc_analyze_audio_X_BEGIN_ADDR) //256 short
#define SBC_pcm_sample_ch1                                  ((SBC_pcm_sample+128))

//CS_SBC_PackFrame
#define CS_SBC_PackFrame_X_BEGIN_ADDR                       (1514 + SBCENC_LOCAL_X_BEGIN_ADDR)
#define SBC_scale_factor                                    (0 + CS_SBC_PackFrame_X_BEGIN_ADDR) //16 short
#define SBC_scale_factor_ch1                                ((SBC_scale_factor+1))
#define Tmp_Bits                                            (16 + CS_SBC_PackFrame_X_BEGIN_ADDR) //16 short
#define Tmp_Bits_ch1                                        ((Tmp_Bits + 1))

//CS_SBC_PackFrame
#define CS_SBC_PackFrame_Y_BEGIN_ADDR                       (0 + SBCENC_LOCAL_Y_BEGIN_ADDR)
#define sb_sample_j                                         (0 + CS_SBC_PackFrame_Y_BEGIN_ADDR) //64 short
#define sb_sample_j_ch1                                     ((sb_sample_j + 2))
#define Tmp_Scale_factor                                    (64 + CS_SBC_PackFrame_Y_BEGIN_ADDR) //32 short
#define Tmp_Scale_factor_ch1                                ((Tmp_Scale_factor+2))
#define Tmp_Levels                                          (96 + CS_SBC_PackFrame_Y_BEGIN_ADDR) //32 short
#define Tmp_Levels_ch1                                      ((Tmp_Levels + 2))
#define Tmp_crc_header_addr                                 (128 + CS_SBC_PackFrame_Y_BEGIN_ADDR) //11 short
#define STRUCT_sbc_frame_joint                              (139 + CS_SBC_PackFrame_Y_BEGIN_ADDR) //1 short

//CS_sbc_analyze_eight
#define CS_sbc_analyze_eight_X_BEGIN_ADDR                   (1770 + SBCENC_LOCAL_X_BEGIN_ADDR)
#define Tmp_T_addr                                          (0 + CS_sbc_analyze_eight_X_BEGIN_ADDR) //16 short

//CS_sbc_analyze_eight
#define CS_sbc_analyze_eight_Y_BEGIN_ADDR                   (0 + SBCENC_LOCAL_Y_BEGIN_ADDR)
#define Tmp_S_addr                                          (0 + CS_sbc_analyze_eight_Y_BEGIN_ADDR) //16 short

//CS_SBC_Calculate_Bits
#define CS_SBC_Calculate_Bits_X_BEGIN_ADDR                  (1546 + SBCENC_LOCAL_X_BEGIN_ADDR)
#define Tmp_bitneed_addr                                    (0 + CS_SBC_Calculate_Bits_X_BEGIN_ADDR) //16 short
#define Tmp_bitneed_addr_ch1                                ((Tmp_bitneed_addr+1))

/**********************
 **  DRCMODE_GLOBAL  **
 **********************/

//GLOBAL_VARS
#define DRC_FOR_MP3_RAM_X_ALIGNED                           (0 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //5548 short
#define DRC_G_X_drc_input_buf_addr                          (5548 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_in_pcm_len                                  (5549 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_frame_len                                   (5550 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_music_drc_gain_amp                          (5551 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_music_drc_instant_gain                      (5552 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1152 short
#define DRC_G_X_music_drc_output_ptr                        (6704 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_music_drc_historydata_ptr                   (6705 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_music_drc_frame                             (6706 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_music_drc_shift_num                         (6707 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_music_drc_crosszeropeak                     (6708 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define DRC_G_X_music_drc_crosszeroindex_i                  (6710 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_music_drc_size                              (6711 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_music_drc_historydata_cur_ptr               (6712 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_music_drc_pow10_fxp_Q                       (6713 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_music_drc_CrossZeroSize                     (6714 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_music_drc_align_tmp                         (6715 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_peak_last1                                  (6716 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define DRC_G_X_peak_last2                                  (6718 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define DRC_G_X_history_peak1                               (6720 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_history_peak2                               (6721 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_crosszero_offset1                           (6722 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_crosszero_offset2                           (6723 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_gain_smooth1                                (6724 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_gain_smooth2                                (6725 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define DRC_G_X_history_parm_data_addr                      (DRC_G_X_peak_last1)
#define DRC_G_X_drc_historydata_buf                         (6726 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2304 short
#define GLOBAL_IIR_BAND_NUM0_USED_ADDR_ADDR                 (9030 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_NUM2_USED_ADDR_ADDR                 (9031 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_DEN1_USED_ADDR_ADDR                 (9032 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define IIR_INPUT_ADDR_ADDR                                 (9033 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_NUM0_ADDR                           (9034 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_NUM1_ADDR                           (9035 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_NUM2_ADDR                           (9036 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_DEN0_ADDR                           (9037 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_DEN1_ADDR                           (9038 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_DEN2_ADDR                           (9039 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_XN_1_USED_ADDR_ADDR                 (9040 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_XN_2_USED_ADDR_ADDR                 (9041 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_YN_1_USED_ADDR_ADDR                 (9042 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_YN_2_USED_ADDR_ADDR                 (9043 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND0_XN_1_ADDR                          (9044 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND0_XN_2_ADDR                          (9046 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND0_YN_1_ADDR                          (9048 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND0_YN_2_ADDR                          (9050 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND1_XN_1_ADDR                          (9052 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND1_XN_2_ADDR                          (9054 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND1_YN_1_ADDR                          (9056 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND1_YN_2_ADDR                          (9058 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND2_XN_1_ADDR                          (9060 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND2_XN_2_ADDR                          (9062 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND2_YN_1_ADDR                          (9064 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND2_YN_2_ADDR                          (9066 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND3_XN_1_ADDR                          (9068 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND3_XN_2_ADDR                          (9070 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND3_YN_1_ADDR                          (9072 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND3_YN_2_ADDR                          (9074 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND4_XN_1_ADDR                          (9076 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND4_XN_2_ADDR                          (9078 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND4_YN_1_ADDR                          (9080 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND4_YN_2_ADDR                          (9082 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND5_XN_1_ADDR                          (9084 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND5_XN_2_ADDR                          (9086 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND5_YN_1_ADDR                          (9088 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND5_YN_2_ADDR                          (9090 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND6_XN_1_ADDR                          (9092 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND6_XN_2_ADDR                          (9094 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND6_YN_1_ADDR                          (9096 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND6_YN_2_ADDR                          (9098 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND0R_XN_1_ADDR                         (9100 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND0R_XN_2_ADDR                         (9102 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND0R_YN_1_ADDR                         (9104 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND0R_YN_2_ADDR                         (9106 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND1R_XN_1_ADDR                         (9108 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND1R_XN_2_ADDR                         (9110 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND1R_YN_1_ADDR                         (9112 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND1R_YN_2_ADDR                         (9114 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND2R_XN_1_ADDR                         (9116 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND2R_XN_2_ADDR                         (9118 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND2R_YN_1_ADDR                         (9120 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND2R_YN_2_ADDR                         (9122 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND3R_XN_1_ADDR                         (9124 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND3R_XN_2_ADDR                         (9126 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND3R_YN_1_ADDR                         (9128 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND3R_YN_2_ADDR                         (9130 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND4R_XN_1_ADDR                         (9132 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND4R_XN_2_ADDR                         (9134 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND4R_YN_1_ADDR                         (9136 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND4R_YN_2_ADDR                         (9138 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND5R_XN_1_ADDR                         (9140 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND5R_XN_2_ADDR                         (9142 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND5R_YN_1_ADDR                         (9144 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND5R_YN_2_ADDR                         (9146 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND6R_XN_1_ADDR                         (9148 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND6R_XN_2_ADDR                         (9150 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND6R_YN_1_ADDR                         (9152 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_IIR_BAND6R_YN_2_ADDR                         (9154 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define IIR_CLIP_MAX_SHORT                                  (9156 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define IIR_CLIP_MIN_SHORT                                  (9158 + DRCMODE_GLOBAL_X_BEGIN_ADDR) //2 short

//GLOBAL_VARS
#define DRC_FOR_MP3_RAM_Y_ALIGNED                           (0 + DRCMODE_GLOBAL_Y_BEGIN_ADDR) //7518 short
//DRC_GLOBAL_CONSTY_STRUCT
#define DRC_GLOBAL_CONSTY_STRUCT                            (7518 + DRCMODE_GLOBAL_Y_BEGIN_ADDR)
#define DRC_G_Y_music_drc_log_table                         (0 + DRC_GLOBAL_CONSTY_STRUCT) //256 short
#define DRC_G_Y_pow10_fxp_const_table                       (256 + DRC_GLOBAL_CONSTY_STRUCT) //258 short
#define DRC_G_Y_pow10_fxp_const_tens_table                  (514 + DRC_GLOBAL_CONSTY_STRUCT) //10 short
#define DRC_G_Y_pow10_fxp_const_Q_table                     (524 + DRC_GLOBAL_CONSTY_STRUCT) //4 short
#define DRC_G_Y_music_drc_const_Q_20                        (528 + DRC_GLOBAL_CONSTY_STRUCT) //1 short
#define DRC_G_Y_music_drc_const_Q_09375                     (529 + DRC_GLOBAL_CONSTY_STRUCT) //1 short
#define DRC_G_Y_music_drc_const_Q_005                       (530 + DRC_GLOBAL_CONSTY_STRUCT) //1 short
#define DRC_G_Y_music_drc_const_4096                        (531 + DRC_GLOBAL_CONSTY_STRUCT) //1 short
#define DRC_G_Y_music_drc_const_1536                        (532 + DRC_GLOBAL_CONSTY_STRUCT) //1 short
#define DRC_G_Y_music_drc_const_3072                        (533 + DRC_GLOBAL_CONSTY_STRUCT) //1 short
#define DRC_G_Y_music_drc_const_32767                       (534 + DRC_GLOBAL_CONSTY_STRUCT) //2 short
#define DRC_G_Y_music_drc_const_32768                       (536 + DRC_GLOBAL_CONSTY_STRUCT) //2 short
#define DRC_G_Y_music_drc_const_0x40000000                  (538 + DRC_GLOBAL_CONSTY_STRUCT) //2 short
#define DRC_G_Y_music_drc_const_0x80000000                  (540 + DRC_GLOBAL_CONSTY_STRUCT) //2 short
#define DRC_G_Y_music_drc_const_0xc0000000                  (542 + DRC_GLOBAL_CONSTY_STRUCT) //2 short

#define DRC_G_Y_music_drc_CrossZeroIndex                    (8062 + DRCMODE_GLOBAL_Y_BEGIN_ADDR) //576 short
#define DRC_G_Y_music_drc_CrossZeroPeakBuffer               (8638 + DRCMODE_GLOBAL_Y_BEGIN_ADDR) //1152 short

/*********************
 **  DRCMODE_LOCAL  **
 *********************/

/********************
 **  COMMON_LOCAL  **
 ********************/

//RAM_X: size 0x2800, used 0x27f0   RAM_Y: size 0x2800, used 0x27f6

#endif
