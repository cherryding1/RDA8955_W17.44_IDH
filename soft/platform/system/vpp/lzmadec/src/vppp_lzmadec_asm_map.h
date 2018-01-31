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

#define COMMON_GLOBAL_X_SIZE                                    9272
#define COMMON_GLOBAL_Y_SIZE                                    8192
#define COMMON_LOCAL_X_SIZE                                     0
#define COMMON_LOCAL_Y_SIZE                                     0

/**************************
 **  SECTION_BEGIN_ADDR  **
 **************************/

#define COMMON_GLOBAL_X_BEGIN_ADDR                              (0 + RAM_X_BEGIN_ADDR)
#define COMMON_GLOBAL_Y_BEGIN_ADDR                              (0 + RAM_Y_BEGIN_ADDR)
#define COMMON_LOCAL_X_BEGIN_ADDR                               (9272 + RAM_X_BEGIN_ADDR)
#define COMMON_LOCAL_Y_BEGIN_ADDR                               (8192 + RAM_Y_BEGIN_ADDR)

/*********************
 **  COMMON_GLOBAL  **
 *********************/

//GLOBAL_VARS
#define GLOBAL_INBUF                                        (0 + COMMON_GLOBAL_X_BEGIN_ADDR) //1024 short
#define probs_p                                             (1024 + COMMON_GLOBAL_X_BEGIN_ADDR) //8192 short
//VPP_LZMADEC_CFG_STRUCT
#define VPP_LZMADEC_CFG_STRUCT                              (9216 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define GLOBAL_IN_PTR                                       (0 + VPP_LZMADEC_CFG_STRUCT) //1 word
#define GLOBAL_OUT_PTR                                      (2 + VPP_LZMADEC_CFG_STRUCT) //1 word
#define GLABAL_OUT_PARA                                     (4 + VPP_LZMADEC_CFG_STRUCT) //1 word

//VPP_LZMADEC_STATUS_STRUCT
#define VPP_LZMADEC_STATUS_STRUCT                           (9222 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define GLOBAL_USED_MODE                                    (0 + VPP_LZMADEC_STATUS_STRUCT) //1 short
#define GLOBAL_LZMADEC_STATUS                               (1 + VPP_LZMADEC_STATUS_STRUCT) //1 short
#define GLOBAL_OUTSIZE                                      (2 + VPP_LZMADEC_STATUS_STRUCT) //1 word

//lzma_header
#define lzma_header                                         (9226 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define lzma_header_pos                                     (0 + lzma_header) //1 short
#define lzma_header_reserve                                 (1 + lzma_header) //1 short
#define lzma_header_dict_size                               (2 + lzma_header) //1 word
#define lzma_header_dst_size                                (4 + lzma_header) //2 word

//cstate
#define cstate                                              (9234 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define cstate_state                                        (0 + cstate) //1 short
#define cstate_reserve                                      (1 + cstate) //1 short
#define cstate_rep0                                         (2 + cstate) //1 word
#define cstate_rep1                                         (4 + cstate) //1 word
#define cstate_rep2                                         (6 + cstate) //1 word
#define cstate_rep3                                         (8 + cstate) //1 word

//writer
#define writer                                              (9244 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define writer_previous_byte                                (0 + writer) //1 short
#define writer_reserve                                      (1 + writer) //1 short
#define writer_buffer_pos                                   (2 + writer) //1 word
#define writer_bufsize                                      (4 + writer) //1 word

//rc
#define rc                                                  (9250 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define rc_ptr                                              (0 + rc) //1 short
#define rc_reserve                                          (1 + rc) //1 short
#define rc_code                                             (2 + rc) //1 word
#define rc_range                                            (4 + rc) //1 word
#define rc_bound                                            (6 + rc) //1 word

#define LeftShift_1_RC_TOP_BITS                             (9258 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define pos_state_mask                                      (9260 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define literal_pos_mask                                    (9261 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_lc                                           (9262 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define num_probs                                           (9263 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define pos_state                                           (9264 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define prob                                                (9265 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define OutputDMASize                                       (9266 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define WriteBufP                                           (9268 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define BITCACHE_VALUE                                      (9269 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define prob_len                                            (9270 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_0x100_ADDR                                    (9271 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short

//GLOBAL_VARS
#define writer_buffer                                       (0 + COMMON_GLOBAL_Y_BEGIN_ADDR) //8192 short

/********************
 **  COMMON_LOCAL  **
 ********************/

//RAM_X: size 0x2800, used 0x2438   RAM_Y: size 0x2800, used 0x2000

#endif
