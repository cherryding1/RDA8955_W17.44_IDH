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

#define COMMON_GLOBAL_X_SIZE                                    9772
#define COMMON_GLOBAL_Y_SIZE                                    9270
#define COMMON_LOCAL_X_SIZE                                     4
#define COMMON_LOCAL_Y_SIZE                                     0

/**************************
 **  SECTION_BEGIN_ADDR  **
 **************************/

#define COMMON_GLOBAL_X_BEGIN_ADDR                              (0 + RAM_X_BEGIN_ADDR)
#define COMMON_GLOBAL_Y_BEGIN_ADDR                              (0 + RAM_Y_BEGIN_ADDR)
#define COMMON_LOCAL_X_BEGIN_ADDR                               (9772 + RAM_X_BEGIN_ADDR)
#define COMMON_LOCAL_Y_BEGIN_ADDR                               (9270 + RAM_Y_BEGIN_ADDR)

/*********************
 **  COMMON_GLOBAL  **
 *********************/

//GLOBAL_VARS
#define GLOBAL_INBUF                                        (0 + COMMON_GLOBAL_X_BEGIN_ADDR) //1024 short
#define GLOBAL_byteleft                                     (1024 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_OutPutAddr                                   (1026 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_PingPangFlag                                 (1028 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_resever1                                     (1029 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define rgb_output_ptr                                      (1030 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define alpha_output_ptr                                    (1032 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define bkgrd_ptr                                           (1034 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define imgOutPtr                                           (1036 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define PNGSignature0                                       (1038 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define PNGSignature1                                       (1040 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_RawOutBufAddr                                (1042 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_PreRow                                       (1043 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
//FILECHUNKS
#define FILECHUNKS                                          (1044 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define FileChunks_IHDRNum                                  (0 + FILECHUNKS) //1 short
#define FileChunks_PLTENum                                  (1 + FILECHUNKS) //1 short
#define FileChunks_IDATNum                                  (2 + FILECHUNKS) //1 short
#define FileChunks_IENDNum                                  (3 + FILECHUNKS) //1 short
#define FileChunks_cHRMNum                                  (4 + FILECHUNKS) //1 short
#define FileChunks_gAMANum                                  (5 + FILECHUNKS) //1 short
#define FileChunks_iCCPNum                                  (6 + FILECHUNKS) //1 short
#define FileChunks_sBITNum                                  (7 + FILECHUNKS) //1 short
#define FileChunks_sRGBNum                                  (8 + FILECHUNKS) //1 short
#define FileChunks_bKGDNum                                  (9 + FILECHUNKS) //1 short
#define FileChunks_hISTNum                                  (10 + FILECHUNKS) //1 short
#define FileChunks_tRNSNum                                  (11 + FILECHUNKS) //1 short
#define FileChunks_pHYsNum                                  (12 + FILECHUNKS) //1 short
#define FileChunks_sPLTNum                                  (13 + FILECHUNKS) //1 short
#define FileChunks_tIMENum                                  (14 + FILECHUNKS) //1 short
#define FileChunks_iTXtNum                                  (15 + FILECHUNKS) //1 short
#define FileChunks_tEXtNum                                  (16 + FILECHUNKS) //1 short
#define FileChunks_zTXtNum                                  (17 + FILECHUNKS) //1 short

//IHDRCHUNK
#define IHDRCHUNK                                           (1062 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define IHDRInfo_Width                                      (0 + IHDRCHUNK) //1 short
#define IHDRInfo_Height                                     (1 + IHDRCHUNK) //1 short
#define IHDRInfo_BitDepth                                   (2 + IHDRCHUNK) //1 short
#define IHDRInfo_ColorType                                  (3 + IHDRCHUNK) //1 short
#define IHDRInfo_Compression                                (4 + IHDRCHUNK) //1 short
#define IHDRInfo_Fileter                                    (5 + IHDRCHUNK) //1 short
#define IHDRInfo_Interlace                                  (6 + IHDRCHUNK) //1 short
#define IHDRInfo_reserve                                    (7 + IHDRCHUNK) //1 short

//sBITInfo
#define sBITInfo                                            (1070 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define sBITInfo_red                                        (0 + sBITInfo) //1 short
#define sBITInfo_green                                      (1 + sBITInfo) //1 short
#define sBITInfo_blue                                       (2 + sBITInfo) //1 short
#define sBITInfo_grey                                       (3 + sBITInfo) //1 short
#define sBITInfo_alpha                                      (4 + sBITInfo) //1 short
#define sBITInfo_reserve                                    (5 + sBITInfo) //1 short

//pHYsINFO
#define pHYsINFO                                            (1076 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define pHYsInfo_xPixPerUint                                (0 + pHYsINFO) //1 word
#define pHYsInfo_yPixPerUnit                                (2 + pHYsINFO) //1 word
#define pHYsInfo_UnitSpec                                   (4 + pHYsINFO) //1 short
#define pHYsInfo_reserve                                    (5 + pHYsINFO) //1 short

//PLTECHUNK
#define PLTECHUNK                                           (1082 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define DefaultBkgd_Red                                     (0 + PLTECHUNK) //1 short
#define DefaultBkgd_Green                                   (1 + PLTECHUNK) //1 short
#define DefaultBkgd_Blue                                    (2 + PLTECHUNK) //1 short
#define DefaultBkgd_reserve                                 (3 + PLTECHUNK) //1 short

//bKGDINFO
#define bKGDINFO                                            (1086 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define bKGDInfo_bKGDGrey                                   (0 + bKGDINFO) //1 short
#define bKGDInfo_bKGDRed                                    (1 + bKGDINFO) //1 short
#define bKGDInfo_bKGDGreen                                  (2 + bKGDINFO) //1 short
#define bKGDInfo_bKGDBlue                                   (3 + bKGDINFO) //1 short
#define bKGDInfo_bKGDPlteIndex                              (4 + bKGDINFO) //1 short
#define bKGDInfo_reserve                                    (5 + bKGDINFO) //1 short

//TRANSINFO
#define TRANSINFO                                           (1092 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define tRNSInfo_tRNSGrey                                   (0 + TRANSINFO) //1 short
#define tRNSInfo_tRNSRed                                    (1 + TRANSINFO) //1 short
#define tRNSInfo_tRNSGreen                                  (2 + TRANSINFO) //1 short
#define tRNSInfo_tRNSBlue                                   (3 + TRANSINFO) //1 short

//z_stream
#define z_stream                                            (1096 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define PNGzstream_next_in                                  (0 + z_stream) //1 word
#define PNGzstream_avail_in                                 (2 + z_stream) //1 word
#define PNGzstream_total_in                                 (4 + z_stream) //1 word
#define PNGzstream_next_out                                 (6 + z_stream) //1 word
#define PNGzstream_avail_out                                (8 + z_stream) //1 word
#define PNGzstream_total_out                                (10 + z_stream) //1 word
#define PNGzstream_data_type                                (12 + z_stream) //1 word
#define PNGzstream_adler                                    (14 + z_stream) //1 word

//inflate_state
#define inflate_state                                       (1112 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define state_mode                                          (0 + inflate_state) //1 short
#define state_length                                        (1 + inflate_state) //1 short
#define state_offset                                        (2 + inflate_state) //1 word
#define state_last                                          (4 + inflate_state) //1 short
#define state_wrap                                          (5 + inflate_state) //1 short
#define state_havedict                                      (6 + inflate_state) //1 short
#define state_flags                                         (7 + inflate_state) //1 short
#define state_dmax                                          (8 + inflate_state) //1 word
#define state_check                                         (10 + inflate_state) //1 word
#define state_total                                         (12 + inflate_state) //1 word
#define state_wbits                                         (14 + inflate_state) //1 short
#define state_reserve001                                    (15 + inflate_state) //1 short
#define state_wsize                                         (16 + inflate_state) //1 word
#define state_whave                                         (18 + inflate_state) //1 word
#define state_write                                         (20 + inflate_state) //1 word
#define state_window                                        (22 + inflate_state) //1 word
#define state_hold                                          (24 + inflate_state) //1 word
#define state_bits                                          (26 + inflate_state) //1 short
#define state_extra                                         (27 + inflate_state) //1 short
#define state_lencode                                       (28 + inflate_state) //1 short
#define state_distcode                                      (29 + inflate_state) //1 short
#define state_lenbits                                       (30 + inflate_state) //1 short
#define state_distbits                                      (31 + inflate_state) //1 short
#define state_ncode                                         (32 + inflate_state) //1 short
#define state_nlen                                          (33 + inflate_state) //1 short
#define state_ndist                                         (34 + inflate_state) //1 short
#define state_have                                          (35 + inflate_state) //1 short
#define state_next                                          (36 + inflate_state) //1 short
#define state_reserve                                       (37 + inflate_state) //1 short
#define state_lens                                          (38 + inflate_state) //320 short
#define state_work                                          (358 + inflate_state) //288 short
#define state_codes_op                                      (646 + inflate_state) //2048 short
#define state_codes_bits                                    (2694 + inflate_state) //2048 short
#define state_codes_val                                     (4742 + inflate_state) //2048 short

//PNGCONSTS_X
#define PNGCONSTS_X                                         (7902 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define crc_table                                           (0 + PNGCONSTS_X) //256 word
#define lbase                                               (512 + PNGCONSTS_X) //31 short
#define lext                                                (543 + PNGCONSTS_X) //31 short
#define dbase                                               (574 + PNGCONSTS_X) //32 short
#define dext                                                (606 + PNGCONSTS_X) //32 short

//ROWINFO
#define ROWINFO                                             (8540 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define RowInfo_Width                                       (0 + ROWINFO) //1 word
#define RowInfo_RowBytes                                    (2 + ROWINFO) //1 word
#define RowInfo_ColorType                                   (4 + ROWINFO) //1 short
#define RowInfo_BitDepth                                    (5 + ROWINFO) //1 short
#define RowInfo_Channels                                    (6 + ROWINFO) //1 short
#define RowInfo_PixelDepth                                  (7 + ROWINFO) //1 short

//cHRMINFO
#define cHRMINFO                                            (8548 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define cHRMInfo_WhiteX                                     (0 + cHRMINFO) //1 word
#define cHRMInfo_WhiteY                                     (2 + cHRMINFO) //1 word
#define cHRMInfo_RedX                                       (4 + cHRMINFO) //1 word
#define cHRMInfo_RedY                                       (6 + cHRMINFO) //1 word
#define cHRMInfo_GreenX                                     (8 + cHRMINFO) //1 word
#define cHRMInfo_GreenY                                     (10 + cHRMINFO) //1 word
#define cHRMInfo_BlueX                                      (12 + cHRMINFO) //1 word
#define cHRMInfo_BlueY                                      (14 + cHRMINFO) //1 word

#define Palete                                              (8564 + COMMON_GLOBAL_X_BEGIN_ADDR) //768 short
#define GLOBAL_BITCACHE                                     (9332 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define ImgChannels                                         (9333 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define RowBytes                                            (9334 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define png_resized_width                                   (9336 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define png_resized_height                                  (9337 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define PngCutOffsetX1                                      (9338 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define PngCutOffsetY1                                      (9339 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define PngCutOffsetX2                                      (9340 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define PngCutOffsetY2                                      (9341 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define IDATSize                                            (9342 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define transform                                           (9344 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define CurPass                                             (9345 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define CurPassRowNum                                       (9346 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define CurInterlaceRowWidth                                (9347 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define CurPassRowBytes                                     (9348 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define Pass_Xstart                                         (9350 + COMMON_GLOBAL_X_BEGIN_ADDR) //7 short
#define Pass_Xinc                                           (9357 + COMMON_GLOBAL_X_BEGIN_ADDR) //7 short
#define Pass_Ystart                                         (9364 + COMMON_GLOBAL_X_BEGIN_ADDR) //7 short
#define Pass_Yinc                                           (9371 + COMMON_GLOBAL_X_BEGIN_ADDR) //7 short
#define PixelDepth                                          (9378 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define MaxBitDepth                                         (9379 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define TransNum                                            (9380 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define NewColorType                                        (9381 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define NewBiteDepth                                        (9382 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define NewTansNum                                          (9383 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define NewRowBytes                                         (9384 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define ImagePass                                           (9386 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define IDATHandle_imgOutStride                             (9387 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define IDATHandle_start                                    (9388 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define IDATHandle_end                                      (9389 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define IDATHandle_ErrCode                                  (9390 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define IDATHandle_skipRow                                  (9391 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define CurRowNumber                                        (9392 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define png_decode_mode                                     (9393 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define png_bkgrd_width                                     (9394 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define png_bkgrd_height                                    (9395 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define order                                               (9396 + COMMON_GLOBAL_X_BEGIN_ADDR) //20 short
#define count                                               (9416 + COMMON_GLOBAL_X_BEGIN_ADDR) //16 short
#define offs                                                (9432 + COMMON_GLOBAL_X_BEGIN_ADDR) //16 short
#define inflate_in                                          (9448 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define inflate_out                                         (9450 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define inflate_table_type                                  (9452 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_lens                                  (9453 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_codes                                 (9454 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_table                                 (9455 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_bits                                  (9456 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_work                                  (9457 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_root                                  (9458 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_max                                   (9459 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_min                                   (9460 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_left                                  (9461 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_base                                  (9462 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_extra                                 (9463 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_end                                   (9464 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_huff                                  (9465 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_len                                   (9466 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_curr                                  (9467 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_low                                   (9468 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_used                                  (9469 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_mask                                  (9470 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_drop                                  (9471 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_next                                  (9472 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_table_copy                                  (9473 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define state_codes_op_addr                                 (9474 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define state_codes_bits_addr                               (9475 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define state_codes_val_addr                                (9476 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_fast_len                                    (9477 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define inflate_fast_dist                                   (9478 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define zmemcpy_Length                                      (9480 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define zmemcpy_SrcAddr                                     (9482 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define zmemcpy_DstAddr                                     (9484 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define updatewindow_copy                                   (9486 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define updatewindow_dist                                   (9488 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define BkgdProcess_data_tmp0                               (9490 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define BkgdProcess_data_tmp1                               (9491 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define BkgdProcess_data_tmp2                               (9492 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define BkgdProcess_RowWidth                                (9493 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define BkgdProcess_sp                                      (9494 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define BkgdProcess_dp                                      (9495 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define BkgdProcess_bkgrdNeed                               (9496 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define BkgdProcess_alphaNeed                               (9497 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define PassMask                                            (9498 + COMMON_GLOBAL_X_BEGIN_ADDR) //7 short
#define inflate_ret                                         (9505 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define FilterRow_istop                                     (9506 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_CONST_8_ADDR                                 (9507 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define PlteSize                                            (9508 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define gammer                                              (9510 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_INBUF_ADDR                                   (9512 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_CONST_512_ADDR                               (9513 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_CONST_1024_ADDR                              (9514 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define tRNSIndex                                           (9516 + COMMON_GLOBAL_X_BEGIN_ADDR) //256 short

//GLOBAL_VARS
#define VPP_PNGDEC_MODE_NO                                  (-1) //alias
#define VPP_PNGDEC_MODE_HI                                  (0) //alias
#define VPP_PNGDEC_MODE_BYE                                 (1) //alias
#define VPP_PNGDEC_ERROR_NO                                 (0) //alias
#define VPP_PNGDEC_ERROR_YES                                (-1) //alias
#define VPP_PNGDEC_SAYS_HI                                  (0x0111) //alias
#define VPP_PNGDEC_SAYS_BYE                                 (0x0B1E) //alias
#define HEAD                                                (0) //alias
#define FLAGS                                               (1) //alias
#define TIME                                                (2) //alias
#define OS                                                  (3) //alias
#define EXLEN                                               (4) //alias
#define EXTRA                                               (5) //alias
#define NAME                                                (6) //alias
#define COMMENT                                             (7) //alias
#define HCRC                                                (8) //alias
#define DICTID                                              (9) //alias
#define DICT                                                (10) //alias
#define TYPE                                                (11) //alias
#define TYPEDO                                              (12) //alias
#define STORED                                              (13) //alias
#define COPY                                                (14) //alias
#define TABLE                                               (15) //alias
#define LENLENS                                             (16) //alias
#define CODELENS                                            (17) //alias
#define LEN                                                 (18) //alias
#define LENEXT                                              (19) //alias
#define DIST                                                (20) //alias
#define DISTEXT                                             (21) //alias
#define MATCH                                               (22) //alias
#define LIT                                                 (23) //alias
#define CHECK                                               (24) //alias
#define LENGTH                                              (25) //alias
#define DONE                                                (26) //alias
#define BAD                                                 (27) //alias
#define MEM                                                 (28) //alias
#define SYNC                                                (29) //alias
#define CODES                                               (0) //alias
#define LENS                                                (1) //alias
#define DISTS                                               (2) //alias
//VPP_PNGDEC_CFG_STRUCT
#define VPP_PNGDEC_CFG_STRUCT                               (0 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_MODE                                         (0 + VPP_PNGDEC_CFG_STRUCT) //1 short
#define GLOBAL_RESET                                        (1 + VPP_PNGDEC_CFG_STRUCT) //1 short
#define GLOBAL_InBufAddr                                    (2 + VPP_PNGDEC_CFG_STRUCT) //1 word
#define GLOBAL_InBufSize                                    (4 + VPP_PNGDEC_CFG_STRUCT) //1 word
#define GLOBAL_PNGOutBufferAddr                             (6 + VPP_PNGDEC_CFG_STRUCT) //1 word
#define GLOBAL_PNGOutBufferSize                             (8 + VPP_PNGDEC_CFG_STRUCT) //1 word
#define GLOBAL_AlphaOutBufferAddr                           (10 + VPP_PNGDEC_CFG_STRUCT) //1 word
#define GLOBAL_AlphaOutBufferSize                           (12 + VPP_PNGDEC_CFG_STRUCT) //1 word
#define GLOBAL_BkgrdBufferAddr                              (14 + VPP_PNGDEC_CFG_STRUCT) //1 word
#define GLOBAL_BkgrdClipX1                                  (16 + VPP_PNGDEC_CFG_STRUCT) //1 short
#define GLOBAL_BkgrdClipX2                                  (17 + VPP_PNGDEC_CFG_STRUCT) //1 short
#define GLOBAL_BkgrdClipY1                                  (18 + VPP_PNGDEC_CFG_STRUCT) //1 short
#define GLOBAL_BkgrdClipY2                                  (19 + VPP_PNGDEC_CFG_STRUCT) //1 short
#define GLOBAL_BkgrdOffsetX                                 (20 + VPP_PNGDEC_CFG_STRUCT) //1 short
#define GLOBAL_BkgrdOffsetY                                 (21 + VPP_PNGDEC_CFG_STRUCT) //1 short
#define GLOBAL_BkgrdWidth                                   (22 + VPP_PNGDEC_CFG_STRUCT) //1 short
#define GLOBAL_BkgrdHeight                                  (23 + VPP_PNGDEC_CFG_STRUCT) //1 short
#define GLOBAL_ZoomWidth                                    (24 + VPP_PNGDEC_CFG_STRUCT) //1 short
#define GLOBAL_ZoomHeight                                   (25 + VPP_PNGDEC_CFG_STRUCT) //1 short
#define GLOBAL_DecMode                                      (26 + VPP_PNGDEC_CFG_STRUCT) //1 short
#define GLOBAL_alpha_blending_to_transparentColor           (27 + VPP_PNGDEC_CFG_STRUCT) //1 short
#define GLOBAL_ImgWidth                                     (28 + VPP_PNGDEC_CFG_STRUCT) //1 word
#define GLOBAL_ImgHeigh                                     (30 + VPP_PNGDEC_CFG_STRUCT) //1 word
#define GLOBAL_WindowAddr                                   (32 + VPP_PNGDEC_CFG_STRUCT) //1 word
#define GLOBAL_rgb_temp_ptr                                 (34 + VPP_PNGDEC_CFG_STRUCT) //1 word
#define GLOBAL_PngFileHandle                                (36 + VPP_PNGDEC_CFG_STRUCT) //1 word

//VPP_PNGDEC_STATUS_STRUCT
#define VPP_PNGDEC_STATUS_STRUCT                            (38 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_USED_MODE                                    (0 + VPP_PNGDEC_STATUS_STRUCT) //1 short
#define GLOBAL_ERR_STATUS                                   (1 + VPP_PNGDEC_STATUS_STRUCT) //1 short
#define GLOBAL_PNGDEC_STATUS                                (2 + VPP_PNGDEC_STATUS_STRUCT) //1 word

//VPP_PNGDEC_CODE_CFG_STRUCT
#define VPP_PNGDEC_CODE_CFG_STRUCT                          (42 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_CONST_LENFIX_DISTFIX_PTR                     (0 + VPP_PNGDEC_CODE_CFG_STRUCT) //1 word
#define GLOBAL_BYE_CODE_PTR                                 (2 + VPP_PNGDEC_CODE_CFG_STRUCT) //1 word
#define GLOBAL_CONST_PTR                                    (4 + VPP_PNGDEC_CODE_CFG_STRUCT) //1 word

//VPP_PNGDEC_CONST_STRUCT
#define VPP_PNGDEC_CONST_STRUCT                             (48 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define VPP_PNGDEC_CONST_SAYS_HI                            (0 + VPP_PNGDEC_CONST_STRUCT) //1 word
#define VPP_PNGDEC_CONST_SAYS_BYE                           (2 + VPP_PNGDEC_CONST_STRUCT) //1 word

#define GLOBAL_RawOutBuf                                    (52 + COMMON_GLOBAL_Y_BEGIN_ADDR) //8192 short
#define GLOBAL_DMATEMPBUF                                   (8244 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1024 short
#define sRGBRenderContent                                   (9268 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define reserve0001                                         (9269 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short

/********************
 **  COMMON_LOCAL  **
 ********************/

//Coolsand_inflate_fast
#define Coolsand_inflate_fast_X_BEGIN_ADDR                  (0 + COMMON_LOCAL_X_BEGIN_ADDR)
#define inflate_fast_lmask                                  (0 + Coolsand_inflate_fast_X_BEGIN_ADDR) //1 short
#define inflate_fast_dmask                                  (1 + Coolsand_inflate_fast_X_BEGIN_ADDR) //1 short
#define inflate_fast_beg                                    (2 + Coolsand_inflate_fast_X_BEGIN_ADDR) //1 short

//RAM_X: size 0x2800, used 0x2630   RAM_Y: size 0x2800, used 0x2436

#endif
